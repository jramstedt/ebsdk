/* file:	toy_driver.c
 *
 * Copyright (C) 1992, 1993 by
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
 *      Jensen Exerciser
 *
 *  MODULE DESCRIPTION:
 *
 *	Driver for PC/AT style clock chips.
 *
 *  AUTHORS:
 *
 *	AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *	20-Oct-1992
 *
 *
 *  MODIFICATION HISTORY:
 *
 *      ska	11-Nov-1998	Conditionalize for YukonA.
 *
 *      jje	22-Feb-1996	Conditionalize for Cortex.
 *				
 *      mr	16-Feb-1995	Add spinlock/unlock to rtc_read() rtc_write()
 *				
 *	dwb	12-sep-1994	Changed spin lock code rase ipl to ipl_sync
 *
 *	dtr	9-sep-1994	remove the last fix.  Doesn't work for tga
 *
 *	dtr	9-sep-1994	Added spin lock to rtc_read/write so that data
 *				is not lost
 *				
 *	dtr	27-Jan-1994	Add mustang to the definitons for in/outport
 *
 *	bobf	06-Jan-1994	Minor modifications to Medulla conditional
 *				compilation.
 *
 *	mdr	01-Jan-1994	Added conditional compiles to support Medulla
 *				pass1 and pass2 hardware.
 *
 *	joes	13-Oct-1993	change to new bus interface.
 *
 *	joes	16-Mar-1993	Updated rtc_read and rtc_write for Medulla
 *
 *	joes	03-Mar-1993	replaced hard coded RTC address with platform
 *				specific definition. Allows driver to work on 
 * 				non PC platforms. i.e. Medulla with PCI bus.
 *
 *	pel	25-Feb-1993	replace xbus_in/outport w/ in/outport so
 * 				they'll work w/ either xbus or eisa.
 *
 *--
 */

#if CORTEX || YUKONA
#define MEDULLA_PASS_NUMBER 3
#endif

#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:tt_def.h"
#include	"cp$src:combo_def.h"
#include	"cp$src:pb_def.h"
#include	"cp$inc:prototypes.h"

#if WILDFIRE || WFSTDIO
extern struct pb stdio_pb;
#define toy_pb &stdio_pb
#else
#define toy_pb 0
#endif

extern void outportb(struct pb *pb, int offset, unsigned char data) ;
extern unsigned char inportb(struct pb *pb, int offset);
extern int null_procedure ();
extern struct LOCK spl_atomic;

int toy_read ();
int toy_write ();
int toy_open ();
int toy_close ();

struct DDB toy_ddb = {
	"toy",			/* how this routine wants to be called	*/
	toy_read,		/* read routine				*/
	toy_write,		/* write routine			*/
	toy_open,		/* open routine				*/
	toy_close,		/* close routine			*/
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



int rtc_read (int offset) {
    unsigned long the_addr;
    unsigned char   toy_temp;
    unsigned long saved_ipl;

#if IMP
    return msg_success;
#endif


    spinlock(&spl_atomic);
    


#if MEDULLA || CORTEX || YUKONA
	
#if MEDULLA_PASS_NUMBER == 1
	the_addr = xlate_rtc_addr((unsigned long)RTC_BASE+(unsigned long)offset);
	toy_temp = inportb(NULL, (int)the_addr);
#else
	toy_temp =  inportb(NULL, (int)((unsigned long)RTC_BASE +
					(unsigned long)offset));

#endif

#else
	outportb (toy_pb, (uint64)RTC_OFFSET_REG, offset);
	toy_temp =  inportb (toy_pb, (uint64)RTC_DATA_REG);
#endif

    spinunlock(&spl_atomic);

    return((unsigned int)toy_temp);
}

void rtc_write (int offset, int data) {
    unsigned long the_addr;
    unsigned long saved_ipl;

#if IMP
    return msg_success;
#endif

    spinlock(&spl_atomic);

#if MEDULLA || CORTEX || YUKONA
	
#if MEDULLA_PASS_NUMBER == 1
	the_addr = xlate_rtc_addr((unsigned long)RTC_BASE+(unsigned long)offset);
	outportb(NULL, (int)the_addr, data);
#else
	outportb(NULL, (int)((unsigned long)RTC_BASE +
					(unsigned long)offset), data);
#endif

#else
	outportb (toy_pb, (uint64)RTC_OFFSET_REG, offset);
	outportb (toy_pb, (uint64)RTC_DATA_REG, data);
#endif
    spinunlock(&spl_atomic);
}


int toy_init ();
int toy_open (struct FILE *fp, char *info, char *next, char *mode);
int toy_close (struct FILE *fp);
int toy_read (struct FILE *fp, int size, int number, unsigned char *buf);
int toy_write (struct FILE *fp, int size, int number, unsigned char *buf);


/*
 * Initialize the driver and the inode for the Real Time Clock
 */
int toy_init () {
	struct INODE 	*ip;

	allocinode (toy_ddb.name, 1, &ip);
	ip->dva = & toy_ddb;
	ip->attr = ATTR$M_WRITE | ATTR$M_READ;
	ip->len[0] = 64;
	INODE_UNLOCK (ip);

	return msg_success;
}


int toy_open (struct FILE *fp, char *info, char *next, char *mode) {
	struct INODE *ip;
#if IMP
	/*
	 * Forgot to remove toy_driver.c from rawhide.master - so 
	 * rather than doing another MAKE, just fail the open.
	 */

	return msg_failure;
#endif

	/*
	 * Allow multiple readers but only one writer.  ip->misc keeps track
	 * of the number of writers
	 */
	ip = fp->ip;
	INODE_LOCK (ip);
	if (fp->mode & ATTR$M_WRITE) {
	    if (ip->misc) {
		INODE_UNLOCK (ip);
		return msg_failure; /* too many writers */
	    }
	    ip->misc++;
	}

	/*
	 * Treat the information field as a byte offset
	 */
	*fp->offset = xtoi (info);

	INODE_UNLOCK (ip);
	return msg_success;
}

int toy_close (struct FILE *fp) {
	struct INODE *ip;

	ip = fp->ip;
	if (fp->mode & ATTR$M_WRITE) {
	    INODE_LOCK (ip);
	    ip->misc--;
	    INODE_UNLOCK (ip);
	}

	return msg_success;
}


int toy_read (struct FILE *fp, int size, int number, unsigned char *buf) {
	int i;
	int length;
	int bytes_accessed;
	struct INODE *ip;

	/*
	 * Always access a byte at a time
	 */
	ip = fp->ip;
	length = size * number;
	bytes_accessed = 0;

	for (i=0; i<length; i++) {

	    /* verify that the offset is in range */
	    if (!inrange (*fp->offset, 0, ip->len[0])) break;

	    *buf++ = rtc_read (*fp->offset);
	    *fp->offset += 1;
	    bytes_accessed++;

	}

	return bytes_accessed;
}

int toy_write (struct FILE *fp, int size, int number, unsigned char *buf) {
	int i;
	int length;
	int bytes_accessed;
	struct INODE *ip;


	/*
	 * Always access a byte at a time
	 */
	ip = fp->ip;
	length = size * number;
	bytes_accessed = 0;

	for (i=0; i<length; i++) {

	    /* verify that the offset is in range */
	    if (!inrange (*fp->offset, 0, ip->len[0])) break;

	    rtc_write (*fp->offset, *buf);
	    *fp->offset += 1;
	    buf++;
	    bytes_accessed++;
	}

	return bytes_accessed;
}
