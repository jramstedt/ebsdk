/* file:	show_iobq_cmd.c
 *
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
 *      Alpha Firmware
 *             
 *  MODULE DESCRIPTION:
 *
 *	Show iobq command
 *
 *  AUTHORS:
 *
 *      J. Kirchoff
 *
 *  CREATION DATE:
 *  
 *      28-Sep-1993
 *
 *  MODIFICATION HISTORY:
 *             
 *	cto	27-Jun-1994	Make show command call this routine
 *
 *	jrk	28-Sep-1993	Broken out from filesys
 *--
 */

#include "cp$src:platform.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:diag_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:parse_def.h"

extern struct QUEUE _align (QUADWORD) iobq;	/* system wide IO status block q*/
extern struct LOCK _align (QUADWORD) spl_iobq;
extern int iobq_count;				/* count of system wide iobq entries */

/*+
 * ============================================================================
 * = show iobq - Show the I/O counter blocks.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	The iobq is a queue of all I/O blocks in the system.   I/O blocks keep
 *	track of bytes read, bytes written, I/O counts, etc. on a per file and
 *	process basis.
 *  
 *   COMMAND NOFMT:	show_iobq 0 0 0 show_iobq
 *
 *   COMMAND FORM:
 *
 * 	show_iobq
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	None
 *
 *   COMMAND OPTION(S):
 *
 * 	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>show_iobq
 *
 *~
 *   COMMAND REFERENCES:
 *
 * 	None
 *
 * FORM OF CALL:
 *  
 *	show_iobq (int argc, char *argv [])
 *  
 * RETURN CODES:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

#define	MAX_PROG_NAME 12

int show_iobq () {
    struct QUEUE	*p;
    struct IOB		*iob;
    diag_common 	*diag;
    int			p_estimate;	/* approx number of IOBs */
    int			p_found;       	/* number of IOBs found	*/

    struct {
	unsigned long int	pid;
	unsigned long int	passcount;
	unsigned long int	hard_errors;   
	unsigned long int	soft_errors;
	INT			bytes_read;
	INT			bytes_written;
	INT			io_count;
	unsigned long int	retry_count;
	char		dev_name[MAX_NAME];
	char		prog_name[MAX_PROG_NAME];
    } *lp, *clone;

	/*
	 * Allocate a character buffer that will hold copies of the
	 * variables we're interested in.
	 */
    p_found = 0;
    p_estimate = iobq_count + 12;
    clone = (void *) dyn$_malloc (p_estimate * sizeof(*clone), 
		DYN$K_SYNC|DYN$K_NOWAIT|DYN$K_NOFLOOD);
    if (!clone) {
	err_printf( msg_insuff_mem, p_estimate * sizeof( *clone) );
	return msg_insuff_mem;
    }

	/*
	 * Quickly freeze the system and extract all the necessary information
	 * into a local buffer.
	 */
    lp = clone;
    spinlock (&spl_iobq);
    p = iobq.flink;
    while (p != &iobq) {
  	if (p_found >= iobq_count) break;   
  	iob = (struct IOB *) ((unsigned int) p - offsetof (struct IOB, sysq));
   
	lp->pid       = ((struct PCB *)(iob->pcb))->pcb$l_pid;
	if ( (diag = (diag_common *)
             ((struct PCB *)(iob->pcb))->pcb$a_diag_common) != NULL)
	 lp->passcount = diag->diag$l_pass_count;
	else 
	 lp->passcount = 0;
	strncpy (&lp->prog_name [0], ((struct PCB *)(iob->pcb))->pcb$b_name,
		 MAX_PROG_NAME);
	strncpy (&lp->dev_name [0], iob->name, MAX_NAME);
	lp->hard_errors   = iob->hard_errors;
	lp->soft_errors   = iob->soft_errors;
	lp->bytes_read	  = *(INT*) &iob->bytes_read; 
	lp->bytes_written = *(INT*) &iob->bytes_written;
	lp->io_count	  = *(INT*) &iob->io_count;
	lp->retry_count	  = iob->retry_count;
	p_found++;
	lp++;
	p = p->flink;
    }
    spinunlock (&spl_iobq);

	/*
	 * Now we can print out the data at our leisure.
	 */
    printf( msg_show_iob_hdr1);
    printf( msg_show_iob_hdr2);

    lp = clone;
    while (p_found--) {
	if (killpending ()) {
	    free (clone);
	    return msg_success;
	}
  	printf ("%08x %12.12s %-12.12s %6d %4d %4d %13d %13d\n", 
		lp->pid,
		lp->prog_name,
		lp->dev_name,
		lp->passcount,
		lp->hard_errors,
		lp->soft_errors,
		lp->bytes_written,
		lp->bytes_read
	);
	lp++;
    }
  
    free (clone);
    return msg_success;
}

#undef	MAX_PROG_NAME

#if VALIDATE

/*+
 * ============================================================================
 * = dump_iobq - Dump all I/O counter blocks.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	The iobq is a queue of all I/O blocks in the system.   I/O blocks keep
 *	track of bytes read, bytes written, I/O counts, etc. on a per file and
 *	proces basis.
 *  
 *   cmdNOFMT: dump_iobq 0 0 0 dump_iobq
 *
 *   cmdFORM:
 *  
 *	dump_iobq ( )
 *  
 *   cmdNOTAG: dump_iobq 0 RXBZ dump_iobq
 *
 *   cmdARGUMENT(S):
 *
 * 	None
 *
 *   cmdOPTION(S):
 *
 * 	None
 *
 *   cmdEXAMPLE(S):
 *~
 *	>>>dump_ioqb
 *
 *~
 *   cmdREFERENCES:
 *
 * 	None
 *
 * FORM OF CALL:
 *  
 *	dump_iobq (int argc, char *argv [])
 *  
 * RETURN CODES:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int dump_iobq () {
    struct PCB		*pcb;
    struct QUEUE	*p;
    struct IOB		*iob;
    int			p_estimate;	/* approx number of IOBs */
    int			p_found;       	/* number of IOBs found	*/

    struct {
	struct QUEUE 	sysq;
	struct QUEUE 	pcbq;
	INT		bytes_read;
	INT		bytes_written;
	unsigned int	sysq_addr;	/* addr of sysq.flink */
	unsigned int	pcbq_addr;	/* addr of pcbq.flink */
	char		name[MAX_NAME];
    } *lp, *clone;

	/*
	 * Allocate a character buffer that will hold copies of the
	 * variables we're interested in.
	 */
    pcb = getpcb();
    p_found = 0;
    printf ("iobq_count= %4d  iobq addr= %08x  fl,bl= %08x %08x\n                  pcbq addr= %08x  fl,bl= %08x %08x\n",
	 iobq_count, &iobq, iobq.flink, iobq.blink, 
	 &pcb->pcb$r_iobq, pcb->pcb$r_iobq.flink, pcb->pcb$r_iobq.blink);
    p_estimate = iobq_count + 12;
    clone = (void *) dyn$_malloc (p_estimate * sizeof(*clone), 
		DYN$K_SYNC|DYN$K_NOWAIT|DYN$K_NOFLOOD);
    if (!clone) {
	err_printf( msg_insuff_mem, p_estimate * sizeof( *clone) );
	return msg_insuff_mem;
    }

	/*
	 * Quickly freeze the system and extract all the necessary information
	 * into a local buffer.
	 */
    lp = clone;
    spinlock (&spl_iobq);
    p = iobq.flink;
    while (p != &iobq) {
  	if (p_found >= iobq_count) break;   
	iob = (struct IOB *) ((unsigned int) p - offsetof (struct IOB, sysq));
	lp->sysq_addr = &iob->sysq.flink;
	lp->sysq.flink = iob->sysq.flink;
	lp->sysq.blink = iob->sysq.blink;
	lp->pcbq_addr = &iob->pcbq.flink;
  	lp->pcbq.flink = iob->pcbq.flink;
	lp->pcbq.blink = iob->pcbq.blink;
   
	strncpy (&lp->name [0], iob->name, MAX_NAME);
	lp->bytes_read	  = *(INT*) &iob->bytes_read; 
	lp->bytes_written = *(INT*) &iob->bytes_written;
	p_found++;
	lp++;
	p = p->flink;
    }
    spinunlock (&spl_iobq);

	
	/*
	 * Now we can print out the data at our leisure.
	 */
 printf ("             bytes bytes\n");
 printf ("        Name read  written sysqaddr fl bl   pcbqaddr fl bl\n");
 printf ("------------ ----- ------- -------- -- --   -------- -- --\n");


    lp = clone;
    while (p_found--) {
	if (killpending ()) {
	    free (clone);
	    return msg_success;
	}

  	printf ("%*s %d %d %08x %08x %08x   %08x %08x %08x\n",
		sizeof( lp->name) - 20,
   		lp->name,
		lp->bytes_read,
		lp->bytes_written,
		lp->sysq_addr,
		lp->sysq.flink,
		lp->sysq.blink,
		lp->pcbq_addr,
	  	lp->pcbq.flink,
		lp->pcbq.blink
		);
	lp++;
    }

    free (clone);
    return msg_success;
}
#endif
