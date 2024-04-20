/*
 * Copyright (C) 1992 by
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * ABSTRACT:	Event log driver.
 * 
 *	The event log is a line oriented fifo queue of ascii lines used to
 *	capture events that may be of future interest to the operator.
 *
 * OVERVIEW:
 *
 *	The event log is available very early on during system powerup
 *	via the qprintf call.  After the driver is initialized, it is
 *	also available as a standard file call "el".  This file may
 *	be written to and read from as with any other file.  Access
 *	through the standard i/o calls entails line buffering on
 *	reads and writes to preserve the line atomicity of the
 *	event log (only whole lines are stored in the log).
 *
 * AUTHOR:	Stephen F. Shirron
 *
 * MODIFICATION HISTORY:
 *
 *	seh	15-Jan-1997	Corrections to file header for SRM update.
 *
 *	pel	12-Feb-1993	remove MAX_ELBUF def, it's in platform.h now.
 *
 *	ajb	05-Aug-1992	Don't let the read routine drain the buffer.
 *				Still keep the line orientation.
 *
 *	ajb	01-Apr-1992	Overhauled such that it works before the
 *				file system is running.  More efficient use
 *				of memory (important when we need to run in
 *				1 MB of backup cache).  Put in some hooks
 *				to allow for future dynamic expansion of
 *				the log.
 *				
 *	ajb	10-Feb1-1992	Turn qprintf into a pprintf if the EL driver
 *				hasn't been initialized.
 *
 *	ajb	15-Jan-1992	fix memory leak on close
 *
 *      phk     24-Oct-1991     Add the EL_DB buffers to 
 *                              handle split lines on write
 *                              and read character by character on read
 *
 *      phk     08-Oct-1991     Added a EL driver, 
 *				removed the EL builtin command
 *
 *	ajb	23-Aug-1990	Make handling of variable length argument
 *				lists independent of call mechanics.
 *
 *	sfs	01-Aug-1990	Save printf text instead of arguments
 *
 *	ajb	30-Jul-1990	Enhanced spinlock data structures
 *
 *	sfs	19-Jul-1990	Initial entry.
 */

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"


#define EL_MAXLINE	256	/* maximum line size for the event logger */

#if MAX_ELBUF <= EL_MAXLINE
#error "MAX_ELBUF <= EL_MAXLINE"
#endif

#if !MODULAR
char el_ibuf [MAX_ELBUF];	/* initial buffer until we can expand */
char *el_buffer;
volatile int el_in;
volatile int el_out;
volatile int el_len;			/* number of byte in log	*/
volatile int el_size;			/* space allocated for log	*/
volatile int el_lostlines;		/* number of lost lines		*/
struct FILE *el_fp;
#endif

#if MODULAR
extern char el_ibuf [];			/* initial buffer until we can expand */
extern char *el_buffer;
extern volatile int el_in;
extern volatile int el_out;
extern volatile int el_len;		/* number of byte in log	*/
extern volatile int el_size;		/* space allocated for log	*/
extern volatile int el_lostlines;	/* number of lost lines		*/
extern struct FILE *el_fp;
#endif

extern struct LOCK el_spinlock;

struct EL_DB {
  int  wr;			/* index into write buffer	*/
  int  rd;			/* index into read buffer	*/
  int  rdlen;			/* length of read buffer 	*/
  char *rbuf;			/* read buffer			*/
  int  wr_overflow;
  char wbuf [EL_MAXLINE];	/* write buffer			*/
};

int el_read ();
int el_write ();
int el_open ();
int el_close ();
int el_delete ();
int null_procedure ();

struct DDB el_ddb = {
	"el",			/* how this routine wants to be called	*/
	el_read,		/* read routine				*/
	el_write,		/* write routine			*/
	el_open,		/* open routine				*/
	el_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	el_delete,		/* delete routine			*/
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

#if !MODULAR
/*
 * This routine is called early in the idle process before we have any
 * file system or dynamic memory.  This allows us to use qprintf in a 
 * minimal environment.
 */
void el_setup () {
    el_in = 0;
    el_out = 0;
    el_len = 0;
    el_size = MAX_ELBUF;
    el_buffer = el_ibuf;
    el_lostlines = 0;
    el_fp = 0;
}
#endif

/*----------*/
int el_open( struct FILE *fp, char *info, char *next, char *mode ) {
    int x;
    int i;
    int o;
    struct EL_DB *elb;

    /*
     * mallocs must be unowned if we are opened permanently
     */
    if (fp->mode & MODE_M_PERM) {
	o = DYN$K_SYNC | DYN$K_NOOWN;
    } else {
	o = DYN$K_SYNC;
    }

    /* 
     * create the temporary read and write ahead buffer 
     */
    elb = (struct EL_DB *) dyn$_malloc (sizeof (struct EL_DB), o | DYN$K_FLOOD);
    fp->misc = elb;

    /*
     * If we're open for read, then we clone the event logger
     */
    if (fp->mode & MODE_M_READ) {
	elb->rbuf = dyn$_malloc (el_size, o | DYN$K_NOFLOOD);
	spinlock (&el_spinlock);
	x = el_out;
	for (i=0; i<el_len; i++) {
	    elb->rbuf [i] = el_buffer [x];
	    if (++x >= el_size) x = 0;
	}
	elb->rdlen = el_len;
	spinunlock (&el_spinlock);
    }

    return msg_success;
}

/*
 * Close the el device.  Flush any data left in the write buffer to the event
 * log.  Force a newline at the end of the write buffer to insure a well formed
 * event log.
 */
int el_close( struct FILE *fp ) {
    struct EL_DB *elb;
    struct INODE *ip;
    int overflow;
    int underflow;

    elb = fp->misc;

    /*
     * Flush the write buffer with a generated newline if there's
     * anything left in the buffer
     */
    if (elb->wr != 0) {
	char c;
	c = '\n';
	el_write (fp, 1, 1, &c);
    }

    /*
     * keep the inode length up to date 
     */
    ip = (struct INODE *) fp->ip;
    INODE_LOCK (ip);
    ip->len[0] = el_len;
    INODE_UNLOCK (ip);

    /*
     * Check for write overflow and read underflow
     */
    underflow = elb->rd != elb->rdlen;
    overflow = elb->wr_overflow;
    if (elb->rbuf) {
	dyn$_free (elb->rbuf, DYN$K_SYNC);
    }
    dyn$_free (elb, DYN$K_SYNC);

    /*
     * Return a failure if any of the lines were truncated or there was data
     * left in the read ahead buffer.
     */
    if (overflow || underflow) {
	return msg_failure;
    } else {
	return msg_success;
    }
}


/*+
 * ============================================================================
 * = el_write - write to the event log                                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine writes to the event logger.
 *
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should
 *			correspond to the EL device.
 *	int size	size of item in bytes
 *	int number 	number of items.
 *	char *buf	Address to write the data to.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes read, or 0 on error.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *      Updating of the length in the inode is not synchronized.
 *
 *--
 */
int el_write( struct FILE *fp, int size, int number, unsigned char *s ) {
    int len,l, i;
    struct EL_DB *elb;
    char c;

    elb = (struct EL_DB *)fp->misc;
    l = len = size*number;

    /*
     * Visit each character in the input buffer
     */
    while (l--) {

	/*
	 * Store the character into the write buffer and set the truncation flag
	 * if we overflow.  Always leave room at the end of the write buffer
	 * for a newline.
	 */
	elb->wbuf [elb->wr] = *s;
	elb->wr++;
	if (elb->wr >= sizeof elb->wbuf) {
	    elb->wr_overflow = 1;
	    elb->wr--;
	}

	/* flush to the event log if we've just seen a newline */
	if (*s == '\n') {

	    el_writeline (elb->wbuf, elb->wr);

	    /* write buffer is now clear */
	    elb->wr = 0;
	}

	s++;
    }

    return len;
}

#if !MODULAR
/*
 * Write a line to the event logger.  Well formed input will always have a
 * terminating newline.  Translates nulls into newlines.
 *
 * This routine will work in all contexts.  If we're being called directly from
 * qprintf, then we also print out to the terminal used polled mode and if
 * we're in the process of powering up.
 */
void el_writeline (const char *buf, int buflen) {
	int i;
	char bufc;

	/* lock up access to the event log */
	spinlock (&el_spinlock);

	/*
	 * Copy the buffer into the event log, checking
	 * for overflow into the next line.
	 */
	for (i=0; i<buflen; i++) {
	    bufc = buf [i];

	    /* check for overflow into the last line in the log */
	    if (el_len == el_size) {
		char c;
		el_lostlines++;
		/* wipe out the last line */
		do {
		    c = el_buffer [el_out];
		    if (++el_out >= el_size) el_out = 0;
		    el_len--;
		} while ((c != '\n') && (el_len != 0));
	    }

	    el_buffer [el_in] = bufc;
	    if (++el_in >= el_size) el_in = 0;
	    el_len++;
	}

	 /* release access to the event log */
	 spinunlock (&el_spinlock);
}
#endif

/*+
 * ============================================================================
 * = el_read - read the event log                                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine reads the event log.  To keep this routine simple, the
 *	event log was cloned at open time, and all reads are satsified from the
 *	clone.
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should
 *			correspond to the EL device.
 *	int size	size of item in bytes
 *	int number 	number of items.
 *	char *buf	Address to write the data to.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes read, or 0 on error.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *      NONE
 *
 *--
 */
int el_read( struct FILE *fp, int size, int number, unsigned char *s ) {
    struct EL_DB *elb;
    int xfer;

   /*
    * The read is satisfied from the cloned copy of the event log
    */
    elb = (struct EL_DB *)fp->misc;

    xfer = min (size*number, elb->rdlen);
    memcpy (s, & elb->rbuf [elb->rd], xfer);
    elb->rdlen -= xfer;
    elb->rd += xfer;

    return xfer;
}

/*
 * Make the el device visible as a file. Preserve any events that may have
 * already been logged by previous qprintfs.
 */
int el_init( )  {
    struct INODE *ip;

    allocinode (el_ddb.name, 1, &ip );

    ip->dva = & el_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;
    ip->loc = el_buffer;
    ip->len[0] = el_len;
    ip->alloc[0] = el_size;
    INODE_UNLOCK (ip);

    el_fp = fopen( "el", "pw" );

    /*
     * Resize the event log here, preserving the more recent entries if we
     * are downsizing.
     */

    return msg_success;       
}

int el_delete (struct INODE *ip) {

    /* lock up access to the event log */
    spinlock (&el_spinlock);

    el_in = 0;
    el_out = 0;
    el_len = 0;
    ip->len[0] = 0;

    /* release access to the event log */
    spinunlock (&el_spinlock);

    INODE_UNLOCK (ip);

    return msg_success;       
}
