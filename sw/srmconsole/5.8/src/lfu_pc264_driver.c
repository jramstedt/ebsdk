/* file:	lfu_pc264_driver.c
 *
 * Copyright (C) 1998 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha SRM Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	LFU flash ROM device driver that treats the flash ROM
 *      as uniform unstructured byte stream.  All reading and 
 *	writing of the flash ROM, under LFU, is handled by the 
 *	driver.
 *
 *  AUTHORS:
 *
 *	Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *      16-Feb-1998
 *
 *  MODIFICATION HISTORY:
 *
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$inc:platform_io.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:impure_def.h"
#include	"cp$src:romhead.h"
#include	"cp$src:from_def.h"
#include	"cp$src:flash_layout.h"

/* External routine declarations */

extern int whoami( void );
extern int null_procedure( void );
extern unsigned int read_flash_rom( unsigned int src, int size, unsigned char *dst );
extern int header_valid( romheader_t *hdr );
extern void dump_rom_header( romheader_t *hdr );
extern unsigned __int64 lfu_get_rev( char *fw_rev );

/* Local routine declarations */

static void get_srom_revision( char *str );
static int flash_read( struct FILE *fp, const int size, const int number, char *buf );
static int flash_write( struct FILE *fp, const int size, const int number, char *buf );
static int flash_open( struct FILE *fp, char *info, char *next, char *mode );
static int flash_close( struct FILE *fp );
static int flash_inquire( char *rev, char *fname, struct FILE *fp );
static int flash_validate( 
		int command, 
		char *name, 
		char *rev, 
		char *data,
		int size,
		struct FILE *fp );

/* Local data definitions */

static struct DDB flash_ddb = {
	"",			/* how this routine wants to be called	*/
	flash_read,		/* read routine				*/
	flash_write,		/* write routine			*/
	flash_open,		/* open routine				*/
	flash_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	flash_validate,		/* validation routine			*/
	flash_inquire,		/* class specific use			*/
	0,			/* allows information			*/
	0,			/* must be stacked			*/
	1,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};

static struct FROM from[ ] =
    {
	{ "srm",		SRMROM_SECTOR_START,	SECTOR_SIZE,	SRMROM_SIZE,	ATTR$M_READ | ATTR$M_WRITE },
#if API_PLATFORM && !SWORDFISH
	{ "diags",		DIAGSROM_SECTOR_START,	SECTOR_SIZE,	DIAGSROM_SIZE,	ATTR$M_READ | ATTR$M_WRITE },
#endif
#if NAUTILUS || GLXYTRAIN || TINOSA
	{ "srm",		SRMROM_SECTOR_START,	SECTOR_SIZE,	SRMROM_SIZE,	ATTR$M_READ | ATTR$M_WRITE },
#endif
#if TINOSA
#endif
#if !API_PLATFORM || SWORDFISH
	{ "nt",			NTROM_SECTOR_START,		SECTOR_SIZE,	NTROM_SIZE,		ATTR$M_READ | ATTR$M_WRITE },
#if !WEBBRICK && !API_PLATFORM
	{ "fsb",		FSROM_SECTOR_START,		SECTOR_SIZE,	FSROM_SIZE,		ATTR$M_READ | ATTR$M_WRITE }
#endif
#endif
    };

/* 
** Serialization semaphore to allow only one reader/writer
** at a time to get at the flash device.
*/
static struct SEMAPHORE flash_sync;


#define MAX_REV_LEVELS 8
#define RCS_SIGNATURE 0x55

static void get_srom_revision( char *str )
{
    struct impure *impure_ptr;
    unsigned int signature;
    int id, i, rev;
    char tmp[2];

    id = whoami( );
    impure_ptr = ( struct impure * )( PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE + ( id * PAL$IMPURE_SPECIFIC_SIZE ) );
    signature = *( unsigned __int64 * )( impure_ptr->cns$srom_signature );
    if ( ( signature >> 16 ) == 0xDECB ) {
	rev = ( int )( ( * ( unsigned __int64 * )( impure_ptr->cns$srom_srom_rev ) ) >> ( 8 * ( MAX_REV_LEVELS - 1 ) ) ) & 0xFF;
	if ( ( rev == ( int )( RCS_SIGNATURE & 0xFF ) ) || ( rev == ( int )( ~RCS_SIGNATURE & 0xFF ) ) ) {
	    str[0] = 0;
	    for ( i = 0; i < MAX_REV_LEVELS; i++ ) {
		rev = ( int )( ( * ( unsigned __int64 * )( impure_ptr->cns$srom_srom_rev ) ) >> ( 8 * i ) ) & 0xFF;
		if ( rev == 0 ) break;
		sprintf( tmp, "%s%d", ( i == 0 ) ? "" : ".", rev );
		strcat( str, tmp );
	    }
	}
    }
}


/*+
 * ============================================================================
 * = lfu_pc264_init - flash ROM device initialization routine                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the flash ROM device driver.
 *
 * FORM OF CALL:
 *  
 *	lfu_pc264_init( )
 *  
 * RETURN CODES:
 *
 *      msg_success
 *       
 * ARGUMENTS:
 *
 *	None 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int lfu_pc264_init( void ) {

    struct INODE *ip;
    struct FROM *flp;
    char name[20];
    int i;
/* 
** Create an inode entry for each part of the flash, thus making the device 
** visible as a file.
*/
    for( i = 0; i < sizeof( from ) / sizeof( from[0] ); i++ ) {
	flp = &from[i];
	sprintf( name, "%s_flash", flp->name );
	allocinode( name, 1, &ip );
	ip->dva = &flash_ddb;
	ip->loc = ( char *)flp->base;
	ip->bs = flp->sector_length;
	ip->len[0] = flp->length;
	ip->attr = flp->flags;
	ip->misc = ( int * )flp;
	INODE_UNLOCK( ip );
    }
    krn$_seminit( &flash_sync, 1,"flashsync" );	

    return( msg_success );

} /* lfu_pc264_init */

       
/*+
 * ============================================================================
 * = flash_open - open the flash ROM					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Open up the flash ROM. Allow only one reader/writer at a time.
 *	When reading, allow aribtrary sized reads.  When writing, allow 
 *	only sizes that are multiples of the flash ROM segment size.
 *  
 * FORM OF CALL:
 *  
 *	flash_open( struct FILE *fp, char *info, char *next, char *mode )
 *  
 * RETURNS:
 *
 *	msg_success or msg_failure
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *
-*/
int flash_open( struct FILE *fp, char *info, char *next, char *mode ) 
{
    return( from_open( fp, info, next, mode ) );
} 


/*+
 * ============================================================================
 * = flash_close - close the flash ROM                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Close the flash ROM. The actual burn of the device already occurred,
 *	so we only need to release access.
 *
 * FORM OF CALL:
 *  
 *	flash_close( struct FILE *fp )
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *
-*/
int flash_close( struct FILE *fp )
{
    return( from_close( fp ) );
}


/*+
 * ============================================================================
 * = flash_read - read a block of data from the flash ROM		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Read a block of data from the flash ROM.
 *  
 * FORM OF CALL:
 *  
 *	flash_read( fp, int size, int number, buf )
 *  
 * RETURNS:
 *
 *      number of bytes transfered
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *	int size	- size of item
 *	int number	- number of items
 *      char *buf       - address of buffer to read into
-*/
int flash_read( struct FILE *fp, const int size, const int number, char *buf )
{
    return( from_read( fp, size, number, buf ) );
}


/*+
 * ============================================================================
 * = flash_write - write a block of characters to the flash ROM		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Write a block of characters to the flash rom.  The size of the 
 *	input buffer must be a multiple of the flash ROM segment size.
 *  
 * FORM OF CALL:
 *  
 *	flash_write( fp, size, number, buf )
 *  
 * RETURNS:
 *
 *	number of bytes transfered.
 *      0 on errors
 *
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *	int size	- size of item
 *	int number	- number of items
 *      char *buf       - address of buffer to write characters from
 *
 * SIDE EFFECTS:
 *
-*/
int flash_write( struct FILE *fp, const int size, const int number, char *buf )
{
#if API_PLATFORM
	/* So if the image has a header on it. Then reposition file so that the image gets place
	   at the position requested by the header. */
    romheader_t *hdr = buf;

	if (
	    (hdr->romh.V1.hversion >= 1) &&					/* Version 2 header or later */
		((hdr->romh.V2.rom_offset & 1) != 0) &&			/* and a headoffset valid. */
		((hdr->romh.V2.rom_offset & 0xfffffffe) != fp->ip->loc)
	   )
	{
		char temp_str[3];

	    printf("\nNew image has rom offset of %x current offset is %x\n",hdr->romh.V2.rom_offset & 0xfffffffe,fp->ip->loc);

	    read_with_prompt("Use new offset [Y/(N)]", 3, temp_str,0, 0, 1);

	    if (temp_str[0] == 'y' || temp_str[0] == 'Y')
		{
			fp->ip->loc = hdr->romh.V2.rom_offset & 0xfffffffe;
		    printf("\nNew image placed at rom offset of %x\n",fp->ip->loc);
		}
	}
#endif

    return( from_write( fp, size, number, buf ) );
}


/*+
 * ============================================================================
 * = flash_inquire - Inquire about flash ROM image revision/firmware name     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine will get the firmware revision of the flash ROM image.
 *
 * FORM OF CALL:
 *  
 *	flash_inquire( fw, fname, fp );
 *  
 * RETURN CODES:
 *
 *      msg_success or msg_failure
 *       
 * ARGUMENTS:
 *
 *	char *fw	- ROM image revision string
 *	char *fname	- suggested firmware name string
 * 	struct FILE *fp - address of file descriptor
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static int flash_inquire( char *rev, char *fname, struct FILE *fp ) 
{
    struct INODE *ip;
    struct FROM *flp;
    romheader_t hdr;
    char tmp[10];

    strcpy( rev, "Unknown" );

    ip = fp->ip;
    flp = ( struct FROM * )fp->ip->misc;
    if ( strcmp( flp->name, "nt" ) == 0 ) {
	strcpy( fname, "nt_fw" );
    }
    else if ( strcmp( flp->name, "srm" ) == 0 ) {
	strcpy( fname, "srm_fw" );
    }
    else if ( strcmp( flp->name, "fsb" ) == 0 ) {
	strcpy( fname, "fsb_fw" );
    }
    else if ( strcmp( flp->name, "srom" ) == 0 ) {
	strcpy( fname, "post_fw" );
    }
#if API_PLATFORM
    else if ( strcmp( flp->name, "diags" ) == 0 ) {
	strcpy( fname, "diags_fw" );
    }
#endif
    if ( read_flash_rom( flp->base, sizeof( romheader_t ), &hdr ) == msg_success ) {
	if ( header_valid( &hdr ) && ( ROMH_VERSION( &hdr ) > 0 ) ) {
	    if ( ( hdr.romh.V1.fwoptid.id_S.low != 0 ) || ( hdr.romh.V1.fwoptid.id_S.high != 0 ) ) {
		if ( hdr.romh.V1.fw_id == 1 ) {
		    tmp[0] = '\0';
		    if ( hdr.romh.V1.fwoptid.fw_id_S.revision != 0 ) {
			sprintf( tmp, "-%X", hdr.romh.V1.fwoptid.fw_id_S.revision );
		    }
		    sprintf( rev, "%X.%X%s",
			     hdr.romh.V1.fwoptid.fw_id_S.major_version,
			     hdr.romh.V1.fwoptid.fw_id_S.minor_version,
			     tmp );
		}
		else if ( hdr.romh.V1.fw_id == 2 ) {
		    sprintf( rev, "%X.%X-%X",
			     hdr.romh.V1.fwoptid.fw_id_S.major_version,
			     hdr.romh.V1.fwoptid.fw_id_S.minor_version,
			     hdr.romh.V1.fwoptid.fw_id_S.revision );
		}
		else if ( ( hdr.romh.V1.fw_id == 0 ) || ( hdr.romh.V1.fw_id == 6 ) ) {
		    sprintf( rev, "%X.%X",
			     hdr.romh.V1.fwoptid.fw_id_S.major_version,
			     hdr.romh.V1.fwoptid.fw_id_S.minor_version );
		}
		else if ( hdr.romh.V1.fw_id == 10 ) {
		    sprintf( rev, "%X.%X",
			     hdr.romh.V1.fwoptid.fw_id_S.major_version,
			     hdr.romh.V1.fwoptid.fw_id_S.minor_version );
		}
	    }
	    else if ( hdr.romh_uint[0] == 0xC3E0000D ) {
		get_srom_revision( rev );
	    }
	}
    }
    return( msg_success );
}


/*+
 * ============================================================================
 * = flash_validate - validate or verify fimrware			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Validate the first file and return the firmware rev if good or
 *	verify the two files.
 *  
 * FORM OF CALL:
 *  
 *	flash_validate( command, name, rev, data, size, fp )
 *  
 * RETURN CODES:
 *
 *      msg_success or msg_failure
 *       
 * ARGUMENTS:
 *
 *	int command	- command field,
 *                          0 = validate file
 *                          1 = verify the file
 *                          2 = validate file no new filename
 *	char *name	- new filename if required on validate
 *	char *rev	- firmware rev of firmware on validate
 *	char *data	- Pointer to the firmware data.
 *	int size	- Firmware data size.
 * 	struct FILE *fp - address of file descriptor for selected device.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

static int flash_validate( 
    int command, 
    char *name, 
    char *rev,
    char *data, 
    int size, 
    struct FILE *fp )
{
    int status, fw_len, len;
    romheader_t *hdr;
    unsigned int i, j, k, m, checksum;
    char fw_rev[16];
    char tmp[10];
    char *buf;
    
    status = msg_success;
    if ( ( data == NULL ) || ( size == 0 ) ) {
	return( msg_failure );
    }
    if ( size > fp->ip->len[0] ) {
	return( msg_failure );
    }
    hdr = ( romheader_t * )data;
    if ( !header_valid( hdr ) ) {
	return( msg_failure );
    }
    checksum = 0;
    for ( i = 0;  i < hdr->romh.V0.size;  i++) {
	COMPUTE_CHECKSUM( *( ( ( unsigned char * ) hdr ) + hdr->romh.V0.hsize + i ), checksum );
    }
    if ( checksum != hdr->romh.V0.checksum ) {
	return( msg_failure );
    }    
    switch ( command ) {
    	case 0:
	case 2:
	    strcpy( rev, "Unknown" );
	    if ( ROMH_VERSION( hdr ) > 0 ) {
		if ( ( hdr->romh.V1.fwoptid.id_S.low != 0 ) || ( hdr->romh.V1.fwoptid.id_S.high != 0 ) ) {
		    if ( hdr->romh.V1.fw_id == 1 ) {
			tmp[0] = '\0';
			if ( hdr->romh.V1.fwoptid.fw_id_S.revision != 0 ) {
			    sprintf( tmp, "-%X", hdr->romh.V1.fwoptid.fw_id_S.revision );
			}
			sprintf( rev, "%X.%X%s",
				 hdr->romh.V1.fwoptid.fw_id_S.major_version,
				 hdr->romh.V1.fwoptid.fw_id_S.minor_version,
				 tmp );
		    }
		    else if ( hdr->romh.V1.fw_id == 2 ) {
			sprintf( rev, "%X.%X-%X",
				 hdr->romh.V1.fwoptid.fw_id_S.major_version,
				 hdr->romh.V1.fwoptid.fw_id_S.minor_version,
				 hdr->romh.V1.fwoptid.fw_id_S.revision );
		    }
		    else if ( ( hdr->romh.V1.fw_id == 0 ) || ( hdr->romh.V1.fw_id == 6 ) ) {
			sprintf( rev, "%X.%X",
				 hdr->romh.V1.fwoptid.fw_id_S.major_version,
				 hdr->romh.V1.fwoptid.fw_id_S.minor_version );
		    }
		    else if ( hdr->romh.V1.fw_id == 10 ) {
			sprintf( rev, "%X.%X",
				 hdr->romh.V1.fwoptid.fw_id_S.major_version,
				 hdr->romh.V1.fwoptid.fw_id_S.minor_version );
		    }
		}
	    }
	    if ( command == 0 ) {
		if ( ( status = flash_inquire( fw_rev, tmp, fp ) ) == msg_success ) {
		    if ( lfu_get_rev( rev ) || lfu_get_rev( fw_rev ) ) {
			if ( lfu_get_rev( fw_rev ) > lfu_get_rev( rev ) )
			    status = 2;
		    }
		    else {
			if ( strcmp_nocase( fw_rev, rev ) == 1 )
			    status = 2;
		    }
		}
	    }
	    break;
	case 1:
	    k = 0;
	    buf = malloc( 2048 );
	    fw_len = hdr->romh.V0.size + hdr->romh.V0.hsize;
	    for ( i = m = 0;  m < fw_len;  m += 2048 ) {
	    	len = fread( buf, 1, min( 2048, fw_len - m ), fp );
		for ( j = 0;  j < len;  j++, i++ ) {
		    if ( *( buf + j ) != *( data + i ) ) {
		    	qprintf( "Address %x mismatch hw %x, fw %x\n", 
				  i, *( buf + j ), *( data + i ) );
			k = 1;
			break;
		    }
		}
		if ( k ) {
		    break;
		}
	    }
	    if ( k == 0 )
	    	status = msg_success;
	    else
	    	status = msg_failure;
	    free( buf );
    }
    return( status );
}
