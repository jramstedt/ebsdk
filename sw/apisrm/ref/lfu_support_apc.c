/*
* file:        lfu_support_apc.c
*
* Copyright (C) 1997 by
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
*
* Abstract:	
*
*   This module contains most of the AlphaPC specific routines
*   used by LFU.
*
* Author:   Eric Rasmussen
*
* Modifications:
*
*	jmartin	19-Jan-1999	Add WEBBRICK default.
*/

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:mem_def.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:msg_def.h"
#include "cp$inc:kernel_entry.h"
#include "cp$src:pb_def.h"
#include "cp$src:probe_io_def.h"
#include "cp$src:ctype.h"
#include "cp$inc:platform_io.h"
#include "cp$src:eisa.h"
#include "cp$src:romhead.h"

#define MAX_DEV 50
#define RU(x) ( ( ( x ) + 7 ) & ~7 )

/* function declarations */
void lfu_find_fw_images( void );
long lfu_find_all_dev( long typecode, long typecode_ext, long listsize, int *devlist );
void lfu_system_reset( int );
void lfu_display_config( void );
void lfu_system_init( int );
int compute_checksum( int *adr, int cnt, int gbus );
int header_valid ( romheader_t *hdr );
void dump_rom_header ( romheader_t *hdr );

static void lfu_readme( void );
static int lfu_find_pb_pb( struct pb *pb, int tcode, int tcode_ext );
static int signature_valid ( romheader_t *hdr );
static unsigned int header_checksum ( romheader_t *hdr );

extern int lfu_support_readme_command;
extern int ( *lfu_readme_command )( );
extern console_restart( );

extern int ___verylast;
extern int rd_ddb;
extern struct ZONE *memzone;

/* local variables. */
static struct device **master_devptr = 0;
static int master_dev_index = 0;


static int signature_valid ( romheader_t *hdr )
{
    if ( ( hdr->romh.V0.signature == ROM_H_SIGNATURE ) && ( hdr->romh.V0.csignature == ~ROM_H_SIGNATURE ) )
	return( TRUE );
    else if ( ( hdr->romh.V0.signature == 0xC3E0000D ) && ( hdr->romh.V0.csignature == ~ROM_H_SIGNATURE ) )
	return( TRUE );
    else
	return( FALSE );
}

static unsigned int header_checksum ( romheader_t *hdr )
{
    char *ptr;
    unsigned int checksum = 0;

    ptr = ( char * )hdr;
    while ( ptr < ( char * )&ROM_HEADER_CHECKSUM( hdr ) ) {
    	COMPUTE_CHECKSUM( *ptr, checksum );
	++ptr;
    }
    return( checksum );
}

int header_valid ( romheader_t *hdr )
{
    return ( ( signature_valid( hdr ) && ( ROMH_VERSION( hdr ) == 0 ) ) ||
	     ( signature_valid( hdr ) && ( header_checksum( hdr ) == ROM_HEADER_CHECKSUM( hdr ) ) ) );
}

void dump_rom_header ( romheader_t *hdr )
{
    int hver;
    unsigned int checksum = 0;

    printf( "\n" );
    printf( "Header Size......... 0x%X (%d) bytes\n", 
	      hdr->romh.V0.hsize,
	      hdr->romh.V0.hsize );

    printf( "Image Checksum...... 0x%04x (%d)\n",
	      hdr->romh.V0.checksum,
	      hdr->romh.V0.checksum );

    printf( "Image Size.......... 0x%X (%d = %d KB)\n",
	      hdr->romh.V0.size,
	      hdr->romh.V0.size,
	      hdr->romh.V0.size / 1024 );

    printf( "Compression Type.... %d\n", hdr->romh.V0.decomp );

    printf( "Image Destination... 0x%08x%08x\n",
	      hdr->romh.V0.destination.high,
	      hdr->romh.V0.destination.low );

    hver = ROMH_VERSION( hdr );
    if ( hver > 0 ) {
    	printf( "Header Version...... %d\n", hver );
	printf( "Firmware ID......... %d\n", hdr->romh.V1.fw_id );
	printf( "ROM Image Size...... 0x%X (%d = %d KB)\n",
		  hdr->romh.V1.rimage_size,
		  hdr->romh.V1.rimage_size,
		  hdr->romh.V1.rimage_size / 1024 );
	printf( "Firmware ID (Opt.).. %08x%08x\n",
		  hdr->romh.V1.fwoptid.id_S.high,
		  hdr->romh.V1.fwoptid.id_S.low );
	if ( hver > 1 ) {
	    if ( hdr->romh.V2.rom_offset != 0 ) 
	    	printf( "ROM Offset.......... 0x%08x\n", hdr->romh.V2.rom_offset & ~3 );
	}
	printf( "Header Checksum..... 0x%04x\n", ROM_HEADER_CHECKSUM( hdr ) );
	checksum = header_checksum( hdr );
	if ( checksum != ROM_HEADER_CHECKSUM( hdr ) ) {
	    printf( "\nERROR: Bad ROM Header Checksum %0x04x\n", checksum );
	}
	else
	{
	    printf( "\n" );
	}
    }
}

/*+
 * ===========================================================================
 * = find_image_header - Find an image header				     =
 * ===========================================================================
 *
 * OVERVIEW:
 *
 *	This helper routine will walk through memory searching for a 
 *	firmware ROM header.  This is done by looking for the first
 *	longword of the signature.  If one is found, then the inverse
 *	signature pattern is look for in the second longword, otherwise,
 *	continue searching.
 *
 *	Once a valid signature has been found, the next longword field,
 *	header size, is looked at to determine the header version.  If
 *	this image contains a V0 header, then the header checksum field
 *	is not present and a corrupt header cannot be determined.  If
 *	this image contains a V1 header or greater, then the header
 *	checksum is computed and compared against the value stored in
 *	image header.  If the checksums don't match, continue scanning
 *	for a valid header.
 *
 *	Once a valid header has been found a pointer will be returned,
 *	otherwise, 0 will be returned indicating no firmware image found.
 * 
-*/
static romheader_t *find_image_header( unsigned int *start )
{
    romheader_t *header = NULL;
    unsigned int i;
    unsigned int *ptr;

    ptr = *start = RU( *start );
    for ( i = 0;  i < 0x20000;  i++ ) {
    	if ( header_valid( ptr ) ) {
	    header = ( romheader_t * )ptr;
	    break;
	}
	else {
	    ptr = ( unsigned int )ptr + 8;
	    *start = *start + 8;
	}
    }
    return( header );
}

/*+
 * ===========================================================================
 * = lfu_find_fw_images - Finds firmware images				     =
 * ===========================================================================
 *
 * OVERVIEW:
 *	This routine will look for fw images that might have been tacked onto
 *	the tail end of the code.
 * 
-*/
void lfu_find_fw_images( void ) 
{
    int i, j, fw_length;
    romheader_t *header;
    unsigned int checksum;
    char *fw_name = 0;
    struct INODE *ip;

    for ( i = &___verylast; ( header = find_image_header( &i ) ) != NULL; i += fw_length ) {
	checksum = 0;
	for ( j = 0;  j < header->romh.V0.size;  j++) {
	    COMPUTE_CHECKSUM( *( ( ( unsigned char * ) header ) + header->romh.V0.hsize + j ), checksum );
	}
	if ( checksum == header->romh.V0.checksum ) {
	    fw_length = header->romh.V0.hsize + header->romh.V0.size;
	    if ( header->romh.V1.fw_id == 1 ) {
		fw_name = "nt_fw";
	    }
	    if ( header->romh.V1.fw_id == 2 ) {
		fw_name = "srm_fw";
	    }
	    if ( ( header->romh.V1.fw_id == 0 ) || ( header->romh.V1.fw_id == 6 ) ) {
		fw_name = "fsb_fw";
	    }
	    if ( ( header->romh.V0.signature == 0xC3E0000D ) || ( header->romh.V1.fw_id == 10 ) ) {
		fw_name = "post_fw";
	    }
	    if ( fw_name ) {
		allocinode( fw_name, 1, &ip );
		ip->dva = &rd_ddb;
		strcpy( ip->name, fw_name );
		ip->attr = ATTR$M_READ;
		ip->len[0] = fw_length;
		ip->loc = ( int )header;
		ip->alloc[0] = ip->len[0];
		INODE_UNLOCK( ip );
	    }
	}
	else {
	    printf( "lfu_find_fw_images: Checksum error, expected: %x, computed: %x\n", 
		     header->romh.V0.checksum, checksum );
	    fw_length = 8;
	}
    }
}

/*
 *++
 * =========================================================================
 * = lfu_system_init - init the system                                     =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *      This shell routine is called by LFU at the beginning of execution.
 *	With this function we can on a per-system basis start or stop
 *	drivers and/or adjust system parameters, global flags. 
 *
 * FORM OF CALL:
 *  
 *	lfu_system_init( int noninteractive )
 *
 * RETURN CODES:
 *
 *      None.
 *       
 * ARGUMENTS:
 *
 *      None.
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
 *--
 */
void lfu_system_init( int noninteractive ) 
{
    struct FILE *fp;

    /* If there is a readme file, then setup the readme command */
    if ( fp = fopen( "readme", "rs" ) ) {
	lfu_support_readme_command = 1;
	lfu_readme_command = lfu_readme;
	fclose( fp );
    }

    /* Go see if someone placed the AlphaBIOS and/or SRM FW images onto the end */
    lfu_find_fw_images( );

    /* Get option firmware on interactive mode */
    if ( !noninteractive ) {
#if SX164
       lfu_get_options_firmware( "sx164fw" );
#endif
#if LX164
       lfu_get_options_firmware( "lx164fw" );
#endif
#if (PC264 && !WEBBRICK)
       lfu_get_options_firmware( "pc264fw" );
#endif
#if WEBBRICK
       lfu_get_options_firmware( "ds10fw" );
#endif
    }

    /* start up the lfu drivers. */
    ddb_startup( 7 );

    /* startup remaining lfu drivers in interactive mode */
    if ( !noninteractive )
       ddb_startup( 8 );
}

/*
 *++
 * =========================================================================
 * = lfu_find_all_dev - walk a window structure and find all like devices  =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *      Walks thru the configuration table and puts together an array
 *	of pointers of type device. If the caller does not supply a 
 *      large enough array, then only the first 'listsize' elements are 
 *      filled in.   The caller should check the return value which is 
 *      the actual number of devices found.
 *
 * FORM OF CALL:
 *  
 *	lfu_find_all_dev( typecode, typecode_ext, listsize, *devlist )
 *
 * RETURN CODES:
 *
 *      The number of typecode-devices found in the config table.
 *	If this number is larger than listsize, the devlist array
 *	is filled with the first listsize devices found.
 *       
 * ARGUMENTS:
 *
 *      long typecode     - the device type code to match with a 
 *                          device entry in the config tables
 *
 *      long typecode_ext - the device type code extention to match with a 
 *                          device entry in the config tables
 *
 *      long listsize     - pointer to int that will recieve the count
 *                          of the devices found.  The caller specifies
 *	                    the size of the array that is filled in. If this
 *			    value is zero then return the total of typecode
 *			    specified.
 *
 *      int *devlist      - pointer to an array of device pointers. This
 *	                    array is created and filled out with all the
 *	                    pointers to device structures of type typecode.
 *
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
 *--
 */
long lfu_find_all_dev( long typecode, long typecode_ext, long listsize, int *devlist )
{
    int lfu_find_pb_pb();
    struct io_device *iod;
    int i, j, ndev;

    ndev = 0;

    /* Initialize a local master device configuration array the first time through. */
    if ( master_devptr == 0 ) {
	master_devptr = dyn$_malloc( MAX_DEV * sizeof( struct device * ),
				     DYN$K_FLOOD | DYN$K_SYNC | DYN$K_NOOWN, 0, 0 );
	for ( i = 0; i < MAX_DEV; i++ ) {
	    master_devptr[i] = 
		dyn$_malloc( sizeof( struct device ),
			     DYN$K_FLOOD | DYN$K_SYNC | DYN$K_NOOWN, 0, 0 );
	    master_devptr[i]->tbl = 
		dyn$_malloc( sizeof( struct device_list ),
			     DYN$K_FLOOD | DYN$K_SYNC | DYN$K_NOOWN, 0, 0);
	    master_devptr[i]->tbl->mnemonic =
		dyn$_malloc( 32, DYN$K_FLOOD | DYN$K_SYNC | DYN$K_NOOWN, 0, 0 );
	}
    }

    for ( i = 0; i < master_dev_index; i++ ) {
	if ( ( master_devptr[i]->dtype == typecode ) && 
	     ( master_devptr[i]->dtype_ext == typecode_ext ) )
	    break;
    }

    /* not previously found so go look for it. */
    if ( i == master_dev_index ) {
	iod = find_io_device( TYPE_PCI, typecode, typecode_ext );
	if ( iod ) {
	    find_pb( iod->device, 0, lfu_find_pb_pb, typecode, typecode_ext );
	}
    }


    /* if found some go process them */
    if ( i < master_dev_index ) {
	for ( j = i; ( j < master_dev_index ) && ( master_devptr[j]->dtype == typecode ) &&
			( master_devptr[i]->dtype_ext == typecode_ext ); j++) {
	    if ( listsize > ndev )
		devlist[ndev] = master_devptr[j];
	    ndev++;
	}
    }
    return( ndev );
}

/*
 *++
 * =========================================================================
 * = lfu_system_reset - Reset the system                                   =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *      Reset the system.
 *
 * FORM OF CALL:
 *  
 *	lfu_system_reset ()
 *
 * RETURN CODES:
 *
 *      None.
 *       
 * ARGUMENTS:
 *
 *      None.
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
 *--
 */
void lfu_system_reset ( int noninteractive ) 
{
    struct FILE *fp;
    char *buf, *out_buf;
    int nbytes;
    struct SEMAPHORE done;

    if ( ( fp = fopen( "fwrom", "rs" ) ) != 0 ) {
#if !PC264
	out_buf = buf = ( char * )dyn$_malloc( FWROM_SIZE, DYN$K_ALIGN | 
	    DYN$K_SYNC | DYN$K_ZONE | DYN$K_NOOWN, 4 * 1024 * 1024, 0, memzone );
    /* 
     * Clear the screen,set cursor at 0,0, and turn the cursor on 
     */
	printf("\033[2J\033[0;0H\033[?25h");
    /* 
     * Display a reset message 
     */
	printf("\033[2J\033[2;2HInitializing....\n");
    	while ( nbytes = fread( buf, FWROM_SIZE, 1, fp ) ) {
	    buf += nbytes;
	}
	fclose( fp );
	krn$_seminit( &done, 0, "command done" );
	memcpy( 3 * 1024 * 1024, ( int )out_buf + 56, FWROM_SIZE - 56 );
	shell_cmd( "jtopal 300000", &done );
	krn$_wait( &done );
	krn$_semrelease( &done );
#endif
    }
    else {
    /* 
     * Clear the screen,set cursor at 0,0, and turn the cursor on 
     */
	printf("\033[2J\033[0;0H\033[?25h");
#if PC264
    /* 
     * Display a reset message 
     */
        printf("\033[2J\033[2;2HInitializing....\n");
    /* 
     * Sleep a bit so the user can see 
     */
        krn$_sleep(2000);

	outtig( NULL, 0xE00004, 1 );
#else
    /* 
     * Display a message and hang ... 
     */
	printf("\033[2J\033[2;2HPlease reset the system....\n");
    	while ( 1 )
	    krn$_sleep( 1 );
#endif
    }
}
/*
 *++
 * =========================================================================
 * = lfu_display_config - display configuration to the user command        =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *      This is a shell routine used by LFU to display the system
 *	configuration. The purpose of this for the user to associate the
 *      driver inode name to the physical hardware.
 *
 * FORM OF CALL:
 *  
 *      lfu_display_config()
 *
 * RETURN CODES:
 *
 *      None.
 *       
 * ARGUMENTS:
 *
 *      None.
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
 *--
 */
void lfu_display_config(void) 
{
    struct SEMAPHORE done;

    krn$_seminit(&done, 0, "command done");

    shell_cmd( "show config", &done );

    krn$_wait( &done );
    krn$_semrelease( &done );
}

/*+
 * ============================================================================
 * = lfu_find_pb_pb - fill master_devtbl information subroutine. 	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will fill into the master_devtbl port block information.
 *
 * FORM OF CALL:
 *
 *	lfu_find_pb_pb ( pb, tcode, tcode_ext )
 *      
 * RETURNS:
 *   
 *	msg_success
 *
 * ARGUMENTS:
 *
 *      struct pb *pb		- pointer to a port block.
 *	int tcode		- device typecode to double check
 *	int tcode_ext		- device typecode_ext to double check
 *
 * SIDE EFFECTS:               
 *      
 *      master_dev_index is incremented.
 *   
-*/
static int lfu_find_pb_pb( struct pb *pb, int tcode, int tcode_ext )
{
    struct INODE *ip;			/* Pointer to INODE for this driver */
    int found = 0;

    /* double check the id's some pb types are different */
    if ( pb->type == TYPE_PCI ) {
	if ( ( tcode == incfgl( pb, 0 ) ) && ( tcode_ext == incfgl( pb, 0x2c ) ) ) {
	    found = 1;
	}
    }

    /* if this one is correct then fill in the information needed */
    if ( found ) {
	sprintf( master_devptr[master_dev_index]->tbl->mnemonic, "%s%c",
		 pb->protocol, pb->controller + 'a' );
	master_devptr[master_dev_index]->unit = 0;
	master_devptr[master_dev_index]->dtype = tcode;
	master_devptr[master_dev_index]->dtype_ext = tcode_ext;
	master_devptr[master_dev_index]->hose = pb->hose;
	master_devptr[master_dev_index]->slot = pb->slot;
	master_devptr[master_dev_index]->bus = pb->bus;
	master_devptr[master_dev_index]->function = pb->function;
	master_devptr[master_dev_index]->channel = pb->channel;
	master_devptr[master_dev_index]->devdep.io_device.devspecific = ( void * )pb;

	/* Point to next entry. */
	master_dev_index++;
    }
    return msg_success;
}

/*
 *++
 * =========================================================================
 * = lfu_readme - list important release information                       =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *      This is a shell routine used by LFU to list important release
 *	information. The purpose of this command is to ensure that
 *	relevant readme first information is presented to the user
 *	should they neglect to veiw the release notes.
 *
 * FORM OF CALL:
 *  
 *      lfu_readme()
 *
 * RETURN CODES:
 *
 *      None.
 *       
 * ARGUMENTS:
 *
 *      None.
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
 *--
 */
static void lfu_readme(void) 
{
    struct SEMAPHORE done;

    krn$_seminit( &done, 0, "command done" );

    shell_cmd( "readme", &done );

    krn$_wait( &done );
    krn$_semrelease( &done );
}

/*+
 * =========================================================================
 * = compute_checksum - compute additive checksum       		   =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *  
 *	compute_checksum( adr, cnt );
 *
 * RETURN CODES:
 *
 *	complemented checksum
 *       
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
 *--
 */
int compute_checksum( int *adr, int cnt, int gbus )
{
    register int sum, i, j;
    register unsigned int u1, u2;
    register unsigned int data;

    sum = 0;
    for (i = 0; i < cnt; i++) {

        data = adr[i];

        j = sum ^ data;                 /* do operand have different signs */

        sum += data;                    /* compute sum */

        if (j >= 0) {                   /* signs the same, overflow possible */
            j = sum ^ data;             /* if sum does not also have the */
            if (j < 0) {                /* same sign, then overflow */
                j = sum ^ 1;
                sum += 1;               /* add 1 */
                if (j >= 0) {
                    j = sum ^ 1;
                    if (j < 0) {
                        sum += 1;       /* add 1 again if add 1 overflowed */
                    }
                }
            }
        }

        u1 = (unsigned int) sum << 31;  /* rotate sum */
        u2 = (unsigned int) sum >> 1;
        sum = u1 | u2;
    }
    return ~sum;                        /* return complemented sum */
}
