/* file:	nl_driver.c
 *
 * Copyright (C) 1990 by
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
 * FACILITY:
 *
 *	Alpha Firmware
 *
 * ABSTRACT:	Null driver.
 *
 *	Driver that implements the bit bucket for the console
 *	firmware.
 *
 * AUTHOR:	AJ Beaverson
 *
 * MODIFICATION HISTORY:
 *
 *	seh	15-Jan-1997	Corrections to file header for SRM update.
 *
 *	sfs	19-Feb-1991	Fix call to ALLOCINODE.  Remove NL_OPEN,
 *				NL_CLOSE, and NL_DELETE.
 *
 *	ajb	08-Feb-1991	have nl_write return the number of bytes
 *				written
 *
 *	pel	07-Feb-1991	make the device writable.
 *
 *	pel	21-Nov-1990	nl_open; get ptr to inode if not done by fopen.
 *
 *	pel	19-Nov-1990	chg many routines into null_procedure.
 *
 *	pel	15-Nov-1990	nl_create, protocol tower change.
 *
 *	pel	14-Nov-1990	nl_open, protocol tower change.
 *
 *	pel	29-Oct-1990	init & add level of indirection to file offset
 *
 *	ajb	28-Sep-1990	Add validation entry point to driver.
 *
 *	ajb	10-Jul-1990	Autodoc updates
 *
 *	ajb	31-May-1990	Remove reference to fslock.
 *
 *	ajb	30-Apr-1990	Initial entry.
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"

extern	null_procedure ();

#define NULL_ATTRIBUTES		(ATTR$M_READ | ATTR$M_WRITE | ATTR$M_EXECUTE)

int nl_read ();
int nl_write ();

struct DDB nl_ddb = {
	"nl",			/* how this routine wants to be called	*/
	nl_read,		/* read routine				*/
	nl_write,		/* write routine			*/
	null_procedure,		/* open routine				*/
	null_procedure,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	0,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};



/*+
 * ============================================================================
 * = nl_read - read from the null device                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Open up the null device.  Stuff pointer to inode into file descriptor
 *	if it hasn't been done by fopen yet. The inode ptr is needed because
 *	file system writes take out the inode write_lock.
 *  
 * FORM OF CALL:
 *	bl_read( buffer, size, number, fp);
 *  
 * RETURNS:
 *
 *  x - number of characters read
 *  0 - if either error or end of file
 *       
 * ARGUMENTS:
 *
 *  char *buffer    - address where stream will be read into
 *  int  size       - size of each object
 *  int number      - number of objects to read
 *  struct FILE *fp - pointer to file descriptor of input stream
 *
 *  
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int nl_read( char *buffer, int size, int number, struct FILE *fp ) {
    return 0;
}

/*+
 * ============================================================================
 * = nl_write - store a block of characters to the null device                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Store a block of characters to the null device
 *  
 * FORM OF CALL:
 *  
 *	nl_write( fp, size, number, buffer);
 *  
 * RETURNS:
 *
 *	x - number of characters written 
 *	0 - if error
 *
 * ARGUMENTS:
 *
 *  char *buffer    - address where stream will be written from
 *  int  size       - size of each object
 *  int number      - number of objects to read
 *  struct FILE *fp - pointer to file descriptor of output stream
-*/
int nl_write (struct FILE *fp, int size, int number, char *buffer) {
	fp->ip->len[0] += (size * number);
	return (size * number);
}

/*+
 * ============================================================================
 * = nl_init - Initialize the null device.                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Initialize the null device.  This routine enters all the entry points
 *	for the null device into the driver data base.
 *  
 * FORM OF CALL:
 *  
 *	nl_init ()
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *       
 * 	None
-*/
int nl_init () {
	struct INODE	*ip;

	/*
	 * Create an inode entry, thus making the device visible
	 * as a file.
	 */
	allocinode (nl_ddb.name, 1, &ip);
	ip->dva = & nl_ddb;
	ip->attr = ATTR$M_READ | ATTR$M_WRITE;
	INODE_UNLOCK (ip);

	return msg_success;
}
