/* file:	from_driver.c
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
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha SRM Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      Device driver that treats the flash ROM as uniform
 *	unstructured byte stream.  All reading and writing is
 *	handled by the driver.
 *
 * 	The driver exports/imports an unstructured byte stream for
 *	the flash ROM.
 *
 *  AUTHORS:
 *
 *	Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *      12-Mar-1997
 *
 *  MODIFICATION HISTORY:
 *
 *      egg	7-Jan-1999	Merged PKO version with SHR version of this file.
 *	er	18-Feb-1998	Changed flash layout for PC264.
 *	er	19-Dec-1997	Fixed from_write to update a sector at a time.
 *	wcc	5-Sep-1997	Added to common pool. Allow for platform 
 *				definitions to define the flash rom sizes and
 *				inode declarations.
 *	er	18-Jul-1997	Added flash device type for failsafe loader.
 *--
 */
/* $INCLUDE_OPTIONS$ */
/* $INCLUDE_OPTIONS_END$ */
#include	"cp$src:platform.h"
#include	"cp$inc:platform_io.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:from_def.h"  /* struct FROM */

static struct FROM from[ ] =
    {
#if LX164
	{ "fsrom", FSROM_BASE, SEGSIZE, FSROM_SIZE,   ATTR$M_READ },
	{ "fwrom", FWROM_BASE, SEGSIZE, FWROM_SIZE,   ATTR$M_READ },
	{ "eerom", EEROM_BASE, SEGSIZE, EEROM_LENGTH, ATTR$M_READ | ATTR$M_WRITE }
#elif PC264 || WEBBRICK
     	{ "flash",  FLASH_ROM_START,	 SECTOR_SIZE, FLASH_ROM_SIZE, ATTR$M_READ },
     	{ "srom",   SROM_SECTOR_START,   SECTOR_SIZE, SROM_SIZE,      ATTR$M_READ },
     	{ "fsrom",  FSROM_SECTOR_START,  SECTOR_SIZE, FSROM_SIZE,     ATTR$M_READ },
     	{ "eerom",  EEROM_SECTOR_START,  SECTOR_SIZE, EEROM_SIZE,     ATTR$M_READ | ATTR$M_WRITE },
     	{ "srmrom", SRMROM_SECTOR_START, SECTOR_SIZE, SRMROM_SIZE,    ATTR$M_READ },
     	{ "ntrom",  NTROM_SECTOR_START,  SECTOR_SIZE, NTROM_SIZE,     ATTR$M_READ }
#elif SX164
       	{ "srom",  SROM_SECTOR_START,  SROM_SECTOR_SIZE,  SROM_SIZE,  ATTR$M_READ },
       	{ "eerom", EEROM_SECTOR_START, EEROM_SECTOR_SIZE, EEROM_SIZE, ATTR$M_READ | ATTR$M_WRITE },
       	{ "fsrom", FSROM_SECTOR_START, FSROM_SECTOR_SIZE, FSROM_SIZE, ATTR$M_READ },
       	{ "fwrom", FWROM_SECTOR_START, FWROM_SECTOR_SIZE, FWROM_SIZE, ATTR$M_READ }
#elif WILDFIRE
     	{ "flash", 0x0, 0x10000, 0x200000, ATTR$M_READ | ATTR$M_WRITE },
#else  
#if WFSTDIO
     	{ "wfrom", 0x200000, 0x10000, 0x200000, ATTR$M_READ | ATTR$M_WRITE },
#endif
       	{ "srom",  SROM_SECTOR_START,  SROM_SECTOR_SIZE,  SROM_SIZE,  ATTR$M_READ },
       	{ "eerom", EEROM_SECTOR_START, EEROM_SECTOR_SIZE, EEROM_SIZE, ATTR$M_READ | ATTR$M_WRITE },
     	{ "fsrom", FSROM_SECTOR_START, FSROM_SECTOR_SIZE, FSROM_SIZE, ATTR$M_READ },
     	{ "fwrom", FWROM_SECTOR_START, FWROM_SECTOR_SIZE, FWROM_SIZE, ATTR$M_READ | ATTR$M_WRITE },
     	{ "srmrom", SRMROM_SECTOR_START, SRMROM_SECTOR_SIZE, SRMROM_SIZE, ATTR$M_READ | ATTR$M_WRITE },
     	{ "tigrom", TIGROM_SECTOR_START, TIGROM_SECTOR_SIZE, TIGROM_SIZE,  ATTR$M_READ },
     	{ "abrom", ABROM_SECTOR_START, ABROM_SECTOR_SIZE, ABROM_SIZE, ATTR$M_READ | ATTR$M_WRITE },
     	{ "flash", FLASH_SECTOR_START, FLASH_SECTOR_SIZE, FLASH_SIZE, ATTR$M_READ | ATTR$M_WRITE }
#endif
    };

extern int null_procedure( void );

int from_init( void );
int from_read( struct FILE *fp, const int size, const int number, char *buf );
int from_write( struct FILE *fp, const int size, const int number, char *buf );
int from_open( struct FILE *fp, char *info, char *next, char *mode );
int from_close( struct FILE *fp );

struct DDB from_ddb = {
	"from",			/* how this routine wants to be called	*/
	from_read,		/* read routine				*/
	from_write,		/* write routine			*/
	from_open,		/* open routine				*/
	from_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};

/* 
** Serialization semaphore to allow only one reader/writer
** at a time to get at the flash device.
*/
struct SEMAPHORE flash_sync;

   
/*+
 * ============================================================================
 * = from_open - open the flash ROM					      =
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
 *	from_open( struct FILE *fp, char *info, char *next, char *mode )
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
int from_open( struct FILE *fp, char *info, char *next, char *mode ) {

/* 
** We only allow one process to access the flash ROM at a time. 
*/
    krn$_wait( &flash_sync );
    if ( killpending( ) ) {
	krn$_post( &flash_sync );
	return msg_failure;
    }
/*
** If the information field is present, treat it as an offset into
** the flash ROM.
*/
    *fp->offset = xtoi( info );

    return msg_success;

} /* from_open */


/*+
 * ============================================================================
 * = from_close - close the flash ROM                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Close the flash ROM. The actual burn of the device already occurred,
 *	so we only need to release access.
 *
 * FORM OF CALL:
 *  
 *	from_close( struct FILE *fp )
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
int from_close( struct FILE *fp ) {

    krn$_post( &flash_sync );

    return msg_success;

} /* from_close */


/*+
 * ============================================================================
 * = from_read - read a block of data from the flash ROM		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Read a block of data from the flash ROM.
 *  
 * FORM OF CALL:
 *  
 *	from_read( fp, int size, int number, buf )
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
int from_read( struct FILE *fp, const int size, const int number, char *buf ) {

    struct INODE *ip;
    struct FROM *fromp;              
    int length, bytes_read = 0;
    unsigned int src;                

    ip = fp->ip;
    fromp = ( struct FROM * )fp->ip->misc;
    length = min( size * number, ip->len[0] - *fp->offset );
    src = fromp->base + *fp->offset;
    if ( *fp->offset <= fromp->length ) {
	if ( read_flash_rom ( src, length, buf ) == msg_success ) {
	    *fp->offset += length;
	    bytes_read = length;       
	}
    }
    return bytes_read;

}  /* from_read */


/*+
 * ============================================================================
 * = from_write - write a block of characters to the flash ROM		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Write a block of characters to the flash rom.  The size of the 
 *   	input buffer must be a multiple of the flash ROM segment size.
 *              
 * !!!NOTE!!! 	Segment access in this routine is, in fact, redundant
 *  		with the lower level segment access amdflash_support.c driver.
 *		However, from_driver is also used with ebflash_support.c which doesn't
 * 		have segment access built in so it is left here. The purely correct
 *              answer is to remove segment access from this file, and put it in
 *		ebflash_suppport.c.(egg)
 * 
 *
 * FORM OF CALL:
 *   
 *   	from_write( fp, size, number, buf )
 *   
 * RETURNS:
 *   
 *   	number of bytes transfered.
 *      0 on errors
 *   
 * ARGUMENTS:
 *   
 *   	struct FILE *fp - address of file descriptor
 *   	int size	- size of item
 *   	int number	- number of items
 *      char *buf       - address of buffer to write characters from
 *   
 * SIDE EFFECTS:
 *   
-*/  
int from_write( struct FILE *fp, const int size, const int number, char *buf ) {
     
    struct INODE *ip;
    struct FROM *fromp;
    int bytes_written = 0;
    int length, nbytes;
    unsigned int i, nsectors;
	
    ip = fp->ip;
    fromp = ( struct FROM * )fp->ip->misc;
    nbytes = min( size * number, ip->len[0] - *fp->offset );    
    nsectors = nbytes / fromp->sector_length;
    if ( ( nbytes % fromp->sector_length ) != 0 ) {
	nsectors++;
    }  
    for ( i = 0;  i < nsectors;  i++ ) {
	if ( *fp->offset <= fromp->length ) {
	    length = min( fromp->sector_length, nbytes - bytes_written );
	    if ( write_flash_rom ( fromp->base + *fp->offset, length, buf ) != msg_success ) {
		break;
	    }
	    *fp->offset += length;
	    buf += length;
	    bytes_written += length;
	}
    }
    return bytes_written;

} /* from_write */


/*+
 * ============================================================================
 * = from_init - flash ROM device initialization routine                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the flash ROM device driver.
 *
 * FORM OF CALL:
 *  
 *	from_init( )
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
int from_init( void ) {

    struct INODE *ip;
    struct FROM *fromp;
    int i;
/* 
** Create an inode entry for each part of the flash, thus making the device 
** visible as a file.
*/
    if ( init_flash_rom( ) != msg_success ) {
	return msg_failure;
    }
    for( i = 0; i < sizeof( from ) / sizeof( from[0] ); i++ ) {
	fromp = &from[i];
	if (fromp->sector_length) { /* only allocate sectors that are real */ 
	    allocinode( fromp->name, 1, &ip );
	    ip->dva = &from_ddb;
	    ip->loc = ( char *)fromp->base;
	    ip->bs = fromp->sector_length;
	    ip->len[0] = fromp->length;
	    ip->attr = fromp->flags;
	    ip->misc = ( int * )fromp;
	    INODE_UNLOCK( ip );
	}
    }
    krn$_seminit( &flash_sync, 1,"flashsync" );	

    return msg_success;

} /* from_init */
