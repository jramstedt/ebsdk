/* file:	pi_driver.c
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
 */

/*
 *++
 * FACILITY:
 *
 *      Cobra Firmware
 *
 * ABSTRACT:	Pipe Driver
 *
 *	Supports multiple writers, only 1 reader w/ no
 *      policing to insure compliance.
 *      Pipe reader is woken after each write char is written to pipe.
 *      Pipe writers are woken after char is read from pipe.
 *      Inode append_offset is used to track where to write w/in a pipe.
 *      File desc local_offset is used to track where to read  w/in a pipe.
 *      After each char is read or writen, offsets are left pointing 
 *      where the next char will be read/written from.
 *      EOF occurs when trying to read an empty pipe & all the pipe writing
 *      processes have closed their pipe file descriptors.
 *	When errors occur an error code is written to the file desc status.
 *
 * AUTHORS:
 *
 *      Paul LaRochelle
 *
 * CREATION DATE:
 * 
 *      20-Jun-1990
 *
 * MODIFICATION HISTORY:
 *
 *	ajb	30-Apr-1992	Create a private structure for rlock and wlock
 *				semaphores so that all inodes don't have to
 *				carry the extra overhead.
 *
 *	pel	27-Nov-1991	pi_close; clear inode inuse if reader so 
 *				pipe writers won't continue to fill pipe
 *				and stall.
 *				
 *	pel	03-May-1991	pi_validate; change call to shell_cmd.
 *				
 *	pel	24-Apr-1991	pi_close; don't clear inode inuse if reader.
 *				
 *	pel	02-Apr-1991	pi_close; del pipe regardless of inuse=0 or =1
 *				i.e., del when ref count goes from 1 to 0 even
 *				if this is a pipe writer.
 *				
 *	pel	28-Mar-1991	pi_close; iolock already locked by fclose
 *				
 *	pel	12-Mar-1991	let fopen/close incr/decr inode ref & collapse
 *				wrt_ref, rd_ref into just ref.
 *				
 *	pel	26-Feb-1991	let pi_validate compile; move shell_cmd to shell
 *				
 *	pel	20-Feb-1991	pi_validate; chg to work w/ new filename syntax
 *				Also added code to call sort via shell_cmd.
 *				shell is crashing tho so I left pi_validate
 *				commented out until I get shell_cmd to work.
 *
 *	sfs	19-Feb-1991	Work on PI_OPEN (ALLOCINODE is already done).
 *
 *	ajb	05-feb-1991	Change pipe size to be a power of 2
 *
 *	ajb	04-feb-1991	Change calling sequence to xx_read and xx_write
 *
 *	pel	23-Jan-1991	pi_open, allow pipe creation
 *				
 *	pel	18-Jan-1991	Use binary sems instead of counting sems to
 *				improve pipe thruput.
 *				
 *	pel	15-Jan-1991	create_noopen, let driver print failure msg.
 *				
 *	pel	14-Jan-1991	pi_open: print own err msg instead of having
 *				fopen do it.
 *
 *	pel	08-Jan-1991	handle allocinode amibiguous filename return.
 *
 *	pel	02-Jan-1991	Decrease validate time.
 *
 *	pel	19-Nov-1990	Use hint from last allocinode in pi_open.
 *
 *	pel	16-Nov-1990	Chg for protocol tower changes.
 *
 *	ajb	08-Nov-1990	Change calling sequence to krn$_create
 *
 *	pel	05-Nov-1990	& file descr modes instead of ==.
 *
 *	pel	01-Nov-1990	pi_validate, use create_noopen.
 *
 *	pel	31-Oct-1990	init & add level of indirection to file offset
 *
 *      pel     25-Oct-1990	put err code into file desc status on error.
 *
 *      pel     24-Oct-1990	pi_close; last writer now posts rlock even if
 *				ip->len[0] == 0. Fixes rdr process hang on rlock.
 *				pi_close; if access mode != "a" don't assume
 *				it's == "r".
 *				Remove krn$_sleep calls before printf's.
 *
 *      pel     21-Oct-1990	replace pi_validate stub w/ better validation.
 *
 *      pel     12-Oct-1990	allow pi_validate to run.
 *
 *      pel     28-Sep-1990	add pi_validate routine. Add pipe_size variable
 *
 *      pel     19-Sep-1990	Return err in pi_open if invalid mode.
 *                              
 *      pel     14-Sep-1990	Trim pi_create already done by allocinode.
 *                              Don't set inuse = 1 in pi_open. Return error
 *                              if opening a pipe when inode inuse == 0.
 *                              Don't post iolock at pi_open exit.
 *                              
 *      pel     07-Sep-1990	Make it portable.
 *                              
 *      pel     06-Sep-1990	Have reader close post writer in case writer
 *                              has stalled on pipe full. For rdr abort cases.
 *                              Make routine header doc reflect actual code.
 *
 *      pel     21-Aug-1990	make it work.
 *
 *      pel      1-Aug-1990	not functional yet. rewrote pi_read, et al.
 *
 *      pel     26-Jul-1990	not functional yet. rewrote read/write 
 *                              routines to use counting semaphores instead 
 *                              of binary semaphores.
 *
 *      pel     12-Jul-1990	Autodoc updates. driver doesn't work yet.
 *--
 */

#include "cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:msg_def.h"
#include "cp$inc:prototypes.h"

extern int null_procedure ();

int pi_read ();
int pi_write ();
int pi_open ();
int pi_close ();
int pi_create ();
int pi_validate ();

extern struct DDB *pi_ddb_ptr;

struct DDB pi_ddb = {
	"pi",			/* how this routine wants to be called	*/
	pi_read,		/* read routine				*/
	pi_write,		/* write routine			*/
	pi_open,		/* open routine				*/
	pi_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	pi_create,		/* create routine			*/
	null_procedure,		/* setmode				*/
	pi_validate,		/* validation routine			*/
	0,			/* class specific use			*/
	0,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	1,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};


/* static data local to pipe_driver */
#define	PIPE_SIZE	2048

struct PISYNC {
	struct SEMAPHORE rlock;
	struct SEMAPHORE wlock;
};

/*+
 * ============================================================================
 * = pi_read - read a block of data from the specified pipe                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Read up to a specified number of bytes from the specified pipe. If pipe is 
 * empty then stall until another process writes one or more chars into the 
 * pipe.  If one or more chars are read resulting in an  empty pipe but these 
 * are fewer than maxchar chars then pi_read returns as soon as the pipe goes 
 * from not empty to empty.
 *
 * EOF is detected when the EOF pipe is empty and the inode ref count = 1.
 * Once EOF occurs subsequent calls to pi_read will return 0 to signal EOF.
 *
 * Only one reader of the same pipe is supported.
 *
 * Multiple writers to the same pipe is supported.
 *
 * The following counting semaphores are used to prevent overfilling a pipe
 * and for synchronizing reading/writing to a pipe.
 *  
 * wlock  = holes in pipe, i.e., how many chars can fit; inited to pipe size
 * rlock  = char count in pipe; inited to 0
 *~
 *  implicitly input:
 *      *fp->offset         offset to calculate where chars will be read from
 *      ip->len[0]          number of chars in pipe
 *   
 *  implicitly output:
 *      *fp->offset         offset to calculate where chars will be read from
 *      ip->len[0]          (old size of pipe) - (number of chars read)
 *~
 * FORM OF CALL:
 *  
 *	pi_read( fp, size, number, buf)
 *  
 * RETURNS:
 *
 *      number of bytes read
 *
 *      0 if error or (EOF detected and no chars read)
 *       
 * ARGUMENTS:
 *
 *      struct FILE *fp - address of file descriptor
 *	int size	- size of item
 *      int number     - number of items to read
 *      char *buf       - address in which to read pipe data into
 *
 * SIDE EFFECTS:
 *
 *      After each char is read, wakes up a writer to the same pipe.
-*/

int pi_read (struct FILE *fp, int size, int number, char *buf) {
    struct INODE    *ip;
    struct PISYNC   *pisync;
    char            *dst;           /*destination of char stream*/
    int             i, xfer_len;
    int		    maxchar;

    maxchar = size * number;
    dst = buf;
    ip = fp->ip;
    pisync = (struct PISYNC *) ip->misc;

          /* pi_close posts rlock when last writer closes the pipe insuring */
          /* that pi_read won't hang on rlock if pipe empty & wrtrs exited */
    while (ip->len[0] == 0) {
	krn$_wait( &pisync->rlock);		/* wait for a char or EOF signal */
	if (ip->len[0] > 0) {	/* post rlock in case we drain the pipe & last*/
	    krn$_bpost( &pisync->rlock);	/* wtr already posted eof */
	    break;			/* go read the data from the pipe */
	}
	else
	    if (ip->ref == 1) { 			/* EOF signalled */
		krn$_bpost( &pisync->rlock); /*so another rd won't hang*/
		return 0;
	    }
    }

    INODE_LOCK (ip);
    if (ip->inuse == 0) {
		  /* post a char so next pi_read won't hang*/
	krn$_bpost( &pisync->rlock);
	INODE_UNLOCK (ip);
	fp->status = msg_fatal;
	return 0;
    }

    i = xfer_len = min( maxchar, ip->len[0]);
    ip->len[0] -= xfer_len;
    while (--i >= 0) {
	*dst++ = ip->loc[(*fp->offset)++];
	*fp->offset &= (PIPE_SIZE - 1);		/* wraparound to Q begining if needed*/
    }

    krn$_bpost( &pisync->wlock);     /*post a hole, wakeup wtr process*/
    INODE_UNLOCK (ip);
    return xfer_len;
}

/*+
 * ============================================================================
 * = pi_write - store a block of characters into a pipe                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Store a block of characters into a pipe. Specified size of block is
 * always the amount of chars written. 
 * If pipe fills it stalls until more chars can be written. 
 * Will work even if # of bytes to write > pipe capacity.
 *
 * Multiple writers to the same pipe are allowed but they are serialized since
 * they take out the inode write_lock before calling this function.
 * 
 * The inode append_offset is used to point to the next pipe write location.
 * Only pi_write updates append_offset.  Only one process can execute pi_write
 * at any given time since the file system layer will have serialized this by
 * waiting on the inode write_lock before calling pi_write.
 *
 * The following counting semaphores are used to prevent overfilling a pipe
 * and for synchronizing reading/writing to a pipe.
 *  
 * wlock  = holes in pipe, i.e., how many chars can fit; inited to pipe size
 * rlock  = char count in pipe; inited to 0
 *~
 *  implicitly input:
 *      ip->append_offset   points at addr at which next char will be written.
 *      ip->len[0]          size of pipe
 *   
 *  implicitly output:
 *      ip->append_offset   points at addr at which next char will be written.
 *      ip->len[0]          (old size of pipe) + (# of chars written)
 *~
 * FORM OF CALL:
 *  
 *	pi_write ( fp, size, number, buf);
 *  
 * RETURNS:
 *
 *  number of characters requested to be written - if successful
 *       
 *  0 - if error
 *       
 * ARGUMENTS:
 *
 *  struct FILE *fp - address of file descriptor
 *  int size	    - size of item
 *  int number	    - number of items to write before returning
 *  char *buf       - buffer address of data to be written into the pipe
 *
 * SIDE EFFECTS:
 *
 *      After each character is written, wakes up the reader of the same pipe.
-*/

int pi_write (struct FILE *fp, int size, int number, char *buf) {
    struct INODE	*ip;
    struct PISYNC       *pisync;
    char		*src;           /*source of char stream*/
    int			xfer_len, total_xfer_len;

    src = buf;
    ip = fp->ip;
    pisync = (struct PISYNC *) ip->misc;

      /*write len chars or fill pipe then stall while chars are read from pipe*/
    total_xfer_len = size * number;
    while (total_xfer_len > 0 ) {
	if (ip->len[0] == PIPE_SIZE)
	    krn$_wait( &pisync->wlock);     /*wait on not full*/

          /* another writer could run NOW & fill up the pipe so the following */
          /* code which writes into the pipe must be able to handle */
          /* writing zero chars */

          /*store as many of the chars as will fit into the pipe*/
        INODE_LOCK (ip);
        if (ip->inuse == 0) {
		 /* post a hole so next pi_write won't hang*/
            krn$_bpost( &pisync->wlock);
            INODE_UNLOCK (ip);
	    fp->status = msg_fatal;  /* the pipe reader has exited */
            return 0;
	}
		  /* write only enough so we don't overwrite the read ptr */
        xfer_len = min( total_xfer_len, PIPE_SIZE - ip->len[0]);
	if (xfer_len > 0) {
            total_xfer_len -= xfer_len;
    	    ip->len[0] += xfer_len;
            while (--xfer_len >= 0) {
                ip->loc[(*fp->offset)++] = *src++;
                *fp->offset &= (PIPE_SIZE - 1); /*wraparnd to Q beging if necesry*/
    	    }
            krn$_bpost( &pisync->rlock); /*post not_empty, wakeup rdr */
	}
        INODE_UNLOCK (ip);
    }

    return size * number;
}

/*+
 * ============================================================================
 * = pi_open - open a pipe                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Open a pipe.  If it doesn't already exist then create it if access mode
 * has the create attribute.
 * fopen may have searched for the pipe in the inode list & passed
 * a hint (in fp->ip) for a pi_open allocinode. 
 * Sets inode inuse flag & increments inode reader or writer reference count.
 *  
 * FORM OF CALL:
 *  
 *      pi_open( fp);
 *  
 * RETURNS:
 *
 *      msg_success
 *       
 *	msg_bad_access_mode - access mode not = "r" or "a"
 *
 *	msg_fatal - file is pending delete or opened for other than append or 
 *                  read mode.
 *
 *	msg_no_file_slot - all inodes are allocated
 *
 * ARGUMENTS:
 *
 *      struct FILE *fp - address of file descriptor
-*/

int pi_open (struct FILE *fp, char *info, char *next, char *mode) {
    int status;
    int permission;
    struct INODE *ip;

    ip = fp->ip;
    INODE_LOCK (ip);
    if (!ip->inuse) {
	status = msg_fatal;
    } else {
	if (fp->mode & MODE_M_WRITE) {
	    fp->offset = &ip->append_offset;
	    status = msg_success;
	} else if (fp->mode & MODE_M_READ) {
	    fp->offset = &fp->local_offset;
	    status = msg_success;
	} else {
	    status = msg_bad_access_mode;
	}
    }
    INODE_UNLOCK (ip);

    if (status != msg_success)
	err_printf (status);
    return status;
}

/*+
 * ============================================================================
 * = pi_close - close a pipe                                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	close a pipe
 *  
 * FORM OF CALL:
 *  
 *	pi_close( fp);
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 *      struct FILE *fp - address of file descriptor
 *
 * SIDE EFFECTS:
 *
 *	Deletes the pipe if the inode ref count is now zero.
-*/

int pi_close (struct FILE *fp) {
    struct INODE    *ip;
    struct PISYNC   *pisync;

    ip = fp->ip;
    pisync = (struct PISYNC *) ip->misc;

    if (fp->mode & MODE_M_READ) {
        ip->inuse = 0;          	/* tell any writers to exit */
	krn$_bpost( &pisync->wlock);	/* wakeup pi_write in case pipe full */
    }

    if (ip->ref == 0)
        pi_delete( ip);       /* pi_delete posts iolock */
    else {
	if (ip->ref == 1)
	    krn$_bpost( &pisync->rlock); /* wakeup pi_read; signal EOF */
	INODE_UNLOCK (ip);
    }

    return msg_success;
}

/*+
 * ============================================================================
 * = pi_create - create a pipe                                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Create a pipe file.
 *  
 * FORM OF CALL:
 *  
 *	pi_create( ip, attributes)
 *  
 * RETURNS:
 *
 *	msg_success
 *	msg_no_file_slot - no inodes available
 *      msg_file_in_use  - file is open by this or another process
 *	msg_ambig_file   - filename is ambiguous
 *       
 * ARGUMENTS:
 *
 *  struct INODE *ip    - pointer to inode.
 *  int    attributes   - attr mask: read, write, execute, binary
-*/

/*----------*/
int pi_create_helper (struct INODE *ip, int attributes) {
    struct PISYNC *pisync;

    pisync = (struct PISYNC *) dyn$_malloc (
	sizeof (struct PISYNC),
	DYN$K_SYNC | DYN$K_NOOWN
    );
    ip->misc = pisync;

    /* inode's iolock must be taken out & posted by caller */
    ip->dva = & pi_ddb;
    ip->attr = attributes;
    ip->alloc[0] = PIPE_SIZE;
    ip->loc = (char *) dyn$_malloc (PIPE_SIZE, DYN$K_SYNC | DYN$K_NOOWN);

    /*Create the read/write counting semaphores*/
    krn$_seminit( &pisync->rlock, 0, "pi_rlock");
    krn$_seminit( &pisync->wlock, 1, "pi_wlock");
    return msg_success;
}

/*----------*/
int pi_create (char *filename, int attributes) {
    int		status;
    struct INODE	*ip;

    status = msg_success;

    switch( allocinode( filename, 1, &ip)) {
    case 0:        			/* not found, not created */
        status = msg_no_file_slot;	/* no inodes available */
        break;
    case 1:        			/* found */
	status = msg_file_in_use;
	INODE_UNLOCK (ip);
        break;
    case 2:        			/* not found and created */
	status = pi_create_helper( ip, attributes);
	INODE_UNLOCK (ip);
        break;
    case 3:
        status = msg_ambig_file;		/* ambiguous inode name found */
        break;
    }

    if (status != msg_success)
	err_printf( status);
    return status;
}

/*+
 * ============================================================================
 * = pi_delete - delete a pipe                                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Delete a pipe inode.  Assume that the inode iolock is taken out.
 *  
 * FORM OF CALL:
 *  
 *	pi_delete( ip);
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 *  struct INODE *ip - pointer to inode.
 *
 * SIDE EFFECTS:
 *
 * remove inode from inode list or marks file (inuse = 0) for pending delete
 * and posts inode iolock.
-*/

int pi_delete (struct INODE *ip) {
    struct PISYNC *pisync;

    pisync = (struct PISYNC *) ip->misc;
    if (ip->ref == 0) {

          /*Release the storage associated with the pipe.*/
    	dyn$_free (ip->loc, DYN$K_SYNC);

          /*Release the read/write lock semaphores*/
    	krn$_semrelease (&pisync->rlock);
    	krn$_semrelease (&pisync->wlock);
	dyn$_free (pisync, DYN$K_SYNC);
	freeinode( ip);
        }
    else {
        ip->inuse = 0;          /* mark file for pending delete */
    	INODE_UNLOCK (ip);
        }

    return msg_success;
}

/*+
 * ============================================================================
 * = pi_validate - validate the PI driver         			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine is called by higher level routines to validate the
 *	PI driver.
 *  
 *  	Does printf w/ unique error path code if it fails.
 *  
 * FORM OF CALL:
 *  
 *	pi_validate ( )
 *  
 * RETURNS:
 *
 *	msg_success	- no errors detected
 *	msg_fatal	- failure
 *       
 * ARGUMENTS:
 *
 * 	none
-*/

#if VALIDATE

extern int cat ();	   
extern int null_procedure ();
extern int shell_cmd ();

#define MAX_STRG_LEN 80


/*----------*/
int pi_validate_wtr ( char *fin_name, char *pipeout_name) {
    char	strg[MAX_STRG_LEN];
    struct FILE *fin_fp;
    struct FILE *pipeout_fp;

    if ( (fin_fp = fopen( fin_name, "r")) == NUL ) {
	printf( msg_pi_bad_validate, 110);
	return msg_success;
    }
    if ( (pipeout_fp = fopen( pipeout_name, "a")) == NUL ) {
	printf( msg_pi_bad_validate, 120);
	return msg_success;
    }

    while (fgets( strg, MAX_STRG_LEN, fin_fp) != NUL) {
	fputs( strg, pipeout_fp);
    }

    return msg_success;
}


/*----------*/
int pi_validate_rdr ( char *pipein_name, char *fout_name) {
    char	strg[MAX_STRG_LEN];
    struct FILE *pipein_fp;
    struct FILE *fout_fp;

    if ( (pipein_fp = fopen( pipein_name, "r")) == NUL ) {
	printf( msg_pi_bad_validate, 210);
	return msg_success;
    }
    if ( (fout_fp = fopen( fout_name, "w")) == NUL ) {
	printf( msg_pi_bad_validate, 220);
	return msg_success;
    }

    while (fgets( strg, MAX_STRG_LEN, pipein_fp) != NUL) {
	fputs( strg, fout_fp);
    }

    return msg_success;
}


/*----------*/
int pi_validate_monitor ( ) {

#define NUMB_OF_WTRS 6
           int       wtr_pri, rdr_pri;   /* pipe wtr/rdr process priorities */
           int       i, status;
           char	     *argvp[4];
  /* cat'ing the validate script in this format will gen 9 30 column lines */
           char	     cat_in [] = "column:30/hexify/validate" ;
           char	     fin_nm [MAX_NAME];
           char	     base_nm [MAX_NAME];
           char	     sorted_base_nm [MAX_NAME];
           char	     pipe_nm [MAX_NAME];
           char	     pipe_data_nm [MAX_NAME];
           char	     sorted_pipe_data_nm [MAX_NAME];
	   char	     cmd_line_strg[80];
    struct FILE      *fin_fp;		/* = cs>semaphore >fin */
    struct FILE      *base_fp;		/* concatenation of fin_fp's */
    struct FILE      *sorted_base_fp;
    struct FILE      *pipe_fp;		/* the pipe */
    struct FILE      *pipe_data_fp;	/* the pipe is written to this file */
    struct FILE      *sorted_pipe_data_fp;
    struct PCB       *curpcb;
    struct SEMAPHORE prdr_done;	/* posted when pipe reader completes */
    struct SEMAPHORE pwtr_done;	/* posted when pipe writer completes */
    struct SEMAPHORE sort_done;	/* posted when sort completes */

    rdr_pri = wtr_pri = 2;
    curpcb = getpcb();
    sprintf( fin_nm, "fin_%d", curpcb->pcb$l_pid);
    sprintf( base_nm, "base_%d", curpcb->pcb$l_pid);
    sprintf( sorted_base_nm, "sorted_base_%d", curpcb->pcb$l_pid);
    sprintf( pipe_nm, "pipe_%d", curpcb->pcb$l_pid);
    sprintf( pipe_data_nm, "pipe_data_%d", curpcb->pcb$l_pid);
    sprintf( sorted_pipe_data_nm, "sorted_pipe_data_%d", curpcb->pcb$l_pid);

      /* sem used for notification of when pipe rdr/wtrs complete */
    krn$_seminit( &prdr_done,  0, "pi_rdr_done"); 
    krn$_seminit( &pwtr_done,  0, "pi_wtr_done"); 
    krn$_seminit( &sort_done,  0, "pi_sort_done"); 

	/* insure that the pipe exists before reader opens it & that the 
           inode ref count remains > 1 until all wtrs complete */
    if (create_noopen( pipe_nm, & pi_ddb, 0, PIPE_SIZE) != msg_success)
            return msg_shell_nopipe;
    if ((pipe_fp = fopen( pipe_nm, "w")) == NUL)
	return 5;

	/* generate an input file */
    argvp[0] = "cat";
    argvp[1] = 0;
    krn$_create( cat, null_procedure, &prdr_done, 2, -1, 1024 * 4,
                 "pi_cat", cat_in, "r",  fin_nm, "w", "tt", "w", 1, argvp);
    krn$_wait( &prdr_done);

	/* startup the pipe writer and reader processes */
    krn$_setpriority( curpcb->pcb$l_pid, 3);
    for (i=0; i<NUMB_OF_WTRS; i++) {
	krn$_create( pi_validate_wtr, null_procedure, &pwtr_done, wtr_pri,
	-1, 1024 * 4, "pi_wtr", "tt", "r", "tt", "w", "tt", "w",
	fin_nm,  pipe_nm);
    }
    krn$_create( pi_validate_rdr, null_procedure, &prdr_done, rdr_pri,
	-1, 1024 * 4, "pi_rdr", "tt", "r", "tt", "w", "tt", "w",
	pipe_nm, pipe_data_nm);
    krn$_setpriority( curpcb->pcb$l_pid, 2);

	/* concatenate NUMB_OF_WTRS fin files into base file */
    if ( (fin_fp  = fopen( fin_nm, "r")) == NUL )
         return 20;
    if ( (base_fp = fopen( base_nm, "w")) == NUL )
         return 30;
    for (i=0; i<NUMB_OF_WTRS; i++) {
	fwrite( fin_fp->ip->loc, fin_fp->ip->len[0], 1, base_fp);
    }
    fclose( fin_fp);
    fclose( base_fp);

	/* sort base file into sorted_base file */
    sprintf( cmd_line_strg, "sort <%s >%s\n", base_nm, sorted_base_nm);
    shell_cmd( cmd_line_strg, &sort_done);
    krn$_wait( &sort_done);

	/* wait for pipe writer & reader processes to complete */
    for (i=0; i<NUMB_OF_WTRS; i++) {
	krn$_wait( &pwtr_done);
    }
    fclose( pipe_fp);	/* this causes pi_close to post eof to the pipe readr*/
    krn$_wait( &prdr_done);

	/* sort the strings in pipe_data file into sorted_pipe_data file */
    sprintf( cmd_line_strg, "sort <%s >%s\n", pipe_data_nm,sorted_pipe_data_nm);
    shell_cmd( cmd_line_strg, &sort_done);
    krn$_semrelease( &pwtr_done);
    krn$_semrelease( &prdr_done);
    krn$_wait( &sort_done);
    krn$_semrelease( &sort_done);

	/* compare sorted_base to sorted_pipe_data file */
    if ( (sorted_base_fp = fopen( sorted_base_nm,  "r")) != NUL)
        sorted_pipe_data_fp = fopen( sorted_pipe_data_nm, "r");
    if ((sorted_base_fp == NUL) || (sorted_pipe_data_fp == NUL))
	return 60;

    if (sorted_base_fp->ip->len[0] != sorted_pipe_data_fp->ip->len[0])
        return 55;
    i = memcmp( sorted_base_fp->ip->loc + *sorted_base_fp->offset,
		sorted_pipe_data_fp->ip->loc + *sorted_pipe_data_fp->offset, 
		sorted_base_fp->ip->len[0]);
    if (i != 0)
	return 65;

    fclose( sorted_base_fp);
    fclose( sorted_pipe_data_fp);

    status = msg_success;

    if ((status = remove( fin_nm)) == msg_success) {
        if ((status = remove( base_nm)) == msg_success) {
    	    if ((status = remove( sorted_base_nm)) == msg_success) {
		if ((status = remove( pipe_data_nm)) == msg_success) {
		    status = remove( sorted_pipe_data_nm);
		}
	    }
	}
    }

    if (status != msg_success)
	return 70;

    return status;
}
#endif

/*----------*/
int pi_validate ( ) {

#if VALIDATE
    int	status;

    if ((status = pi_validate_monitor( )) == msg_success) {
	return msg_success;
    } else {
	    printf( msg_pi_bad_validate, status);
	    return msg_fatal;
    }

#else 
	return msg_success;
#endif
}

/*+
 * ============================================================================
 * = pi_init - enter pipe driver functions into driver data base              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Enter pipe driver functions into driver data base.
 *  
 * FORM OF CALL:
 *  
 *	pi_init ();
 *  
 * RETURNS:
 *
 *	msg_success
-*/

int pi_init () {
	pi_ddb_ptr = &pi_ddb;
	return msg_success;
}
