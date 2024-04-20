/* file:	sem_cmd.c
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
 *  FACILITY:
 *
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	semaphore command
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      30-Jan-1992
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	30-jan-1992	Broken out from shell_commands.
 *--
 */
#include "cp$src:platform.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"


extern struct QUEUE	semq;	/* linked list of all known semaphores	*/
extern int		krn$_scount;	/* semaphore count		*/
extern struct ZONE	defzone;

extern struct LOCK _align (QUADWORD) spl_kernel;

/*+
 * ============================================================================
 * = semaphore - Show system semaphores.                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Show all the semaphores known to the system by traversing
 *	the semaphore queue. 
 *  
 *   COMMAND FMT: semaphore 0 NZ 0 semaphore
 * 
 *   COMMAND FORM:
 *  
 *	semaphore ( )
 *  
 *   COMMAND TAG: semaphore 0 RXBZ semaphore
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
 *      >>>semaphore
 *                      Name              Value   Address  First Waiter
 *      -------------------------------- -------- -------- ------------------------
 *                              dyn_sync 00000001 00050378
 *                           dyn_release 00000001 000503A0
 *                          shell_iolock 00000001 0015D684
 *                           exit_iolock 00000001 0015D770
 *                           grep_iolock 00000001 0015DB20
 *                           eval_iolock 00000001 0015DC0C
 *                          chmod_iolock 00000001 0015DCF8
 *      ^c
 *      >>> 
 *~
 *   COMMAND REFERENCES:
 *
 *	None
 *
 * FORM OF CALL:
 *  
 *	semaphore( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Success
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *      	   (should always be 1)
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int semaphore (int argc, char *argv[]) {
	krn$_walksemaphore ();
	return (int) msg_success;
}



/*+
 * ============================================================================
 * = krn$_walksemaphore - Walk down the semaphore queue                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Walk down the semaphore queues and show the state of each semaphore.
 *	We clone the semaphore queue into a private structure and print from
 *	that structure so that the data structures are not changing underneath
 *	us.  What is printed is an accurate picture of the system at the time
 *	of the capture.
 *
 * FORM OF CALL:
 *
 *	krn$_walksemaphore ()
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int krn$_walksemaphore (void) {
	struct SEMAPHORE *s;
	struct PCB	*pcb;
	struct {
		int	value;
		int	address;
		char	sname [sizeof s->name];
		char	pname [sizeof pcb->pcb$b_name];
	} *clone, *sp;
	int n;
	int allocated;

	/*
	 * Capture the state of the semaphore queue so that we
	 * get an exact copy.
	 */
	spinlock (&spl_kernel);
	allocated = krn$_scount;
	clone = (void *) dyn$_malloc (allocated * sizeof (*clone), DYN$K_NOSYNC | DYN$K_NOWAIT | DYN$M_ZONE, 0, 0, &defzone);
	if (clone == 0) {
	    spinunlock (&spl_kernel);
	    return msg_failure;
	}
	
	sp = clone;
	s = (void *) semq.flink;
	n = 0;
	while ((void *) s != (void *) &semq) {
	    n++;
	    if (n > allocated) break;
	    s = (void *) ((int) s - offsetof (struct SEMAPHORE, gsq));
	    sp->address = s;
	    strcpy (sp->sname, s->name);
	    if ((sp->value = s->count) < 0) {
		pcb = (void *) s->flink;
		strcpy (sp->pname, pcb->pcb$b_name);
	    } else {
		sp->pname [0] = 0;
	    }
	    sp++;
	    s = (void *) s->gsq.flink;
	}
	spinunlock (&spl_kernel);

	if (n != allocated) {
	    err_printf ("corrupt semaphore queue: %d != %d\n", n, allocated);
	}


	printf ("                Name              Value   Address  First Waiter\n");
	printf ("-------------------------------- -------- -------- ------------------------\n");

	do {
	    if (killpending ()) {
		break;
	    }
	    sp--;
	    printf ("%32s %08x %08X %s\n", sp->sname, sp->value, sp->address, sp->pname);
	} while (sp != clone);

	dyn$_free (clone, DYN$K_SYNC);
	return (int) msg_success;
}
