/* file:	semaphore.c
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
 *	Semaphore routines and data structures for the cobra/laser console
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      17-Sep-1991
 *
 *  MODIFICATION HISTORY:
 *
 *	dtr	24-may-1995	ipl testing with symbolic value and not 0 or
 *				non-zero check.  Getting ready for polling.
 *
 *	ajb	30-jan-1992	Moved krn$_walksemaphore to sem_cmd so that
 *				we have finer granularity in building the
 *				system.
 *
 *	pel	19-Nov-1991	Negate alignment check for semaphores if VAX
 *
 *	ajb	18-Nov-1991	Put in alignment check for semaphores
 *
 *	ajb	17-Sep-1991	Broken out from kernel.c
 *--
 */

#include "cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:prdef.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:platform_cpu.h"
#include "cp$inc:prototypes.h"

#if RAWHIDE || TURBO
#define EXTRA 1
#endif

struct QUEUE	_align (QUADWORD) semq;		/* linked list of all known semaphores	*/
int		_align (QUADWORD) krn$_scount;	/* semaphore count		*/

extern struct QUEUE	_align (QUADWORD) readyq [];
extern struct LOCK spl_kernel;
extern volatile int	_align (QUADWORD) console_mode [MAX_PROCESSOR_ID];


/*+
 * ============================================================================
 * = krn$_sem_startup - Set up data structures for semaphores                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Initialize the data structures for the semaphores in the system.  Make
 *	no assumptions about prior system/semaphore state.  Semaphore startup
 *	entails initializing the queue header and the semaphore count.
 *
 *
 * FORM OF CALL:
 *
 *	krn$_sem_startup ()
 *
 * RETURNS:
 *
 *      None
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
void krn$_sem_startup () {
	semq.flink = (void *) &semq.flink;
	semq.blink = (void *) &semq.flink;
	krn$_scount = 0;
}

/*+
 * ============================================================================
 * = krn$_seminit - Create and initialize a semaphore                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Initialize a semaphore.  This entails putting the semaphore's queue in
 *	order, setting the count, and putting the semaphore on a global
 *	semaphore queue.  The global semaphore queue (semq) is a linked list
 *	of all semaphores known to the system.  The semaphore is also put
 *	on a per process semaphore queue, so that upon image rundown, all
 *	semaphores allocated by this process can be found.
 *
 *
 * FORM OF CALL:
 *
 *	krn$_seminit ( s, value, name ) 
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      struct SEMAPHORE *s	- address of semaphore
 *	int n			- value to which initial count is set
 *	char *name		- semaphore's name (not necessarily unique)
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void krn$_seminit (struct SEMAPHORE *s, int n, char *name) {

	/* Semaphores should be aligned for performance reasons */
#define	SEM_ALIGNMENT	3	/* longword */
#if VALIDATE
	if ((unsigned int) s & SEM_ALIGNMENT) {
	    pprintf ("Semaphore %s at %08X is not aligned\n", name, s);
	}
#endif

	krn$_scount++;
	s->flink = s;
	s->blink = s;
	s->count = n;
	s->value = 0;
	s->owner = 0;
	strncpy (&s->name [0], name, sizeof (s->name));
	s->name [sizeof s->name - 1] = 0;

	insq_lock (&s->gsq, &semq);
}

/*+
 * ============================================================================
 * = krn$_semrelease - Release a semaphore                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Release a semaphore.  Releasing a semaphore entails taking it off of 
 *	the global semaphore queue, and the per process semaphore queue.
 *
 * FORM OF CALL:
 *
 *	krn$_semrelease ( s ) 
 *
 * RETURNS:
 *
 *      0 - success
 *
 * ARGUMENTS:
 *
 *      struct SEMAPHORE *s - address of semaphore
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void krn$_semrelease (struct SEMAPHORE *s) {

	if (s->count < 0) {
	    pprintf ("krn$_semrelease:  semaphore %s has waiters\n", s->name);
	    sysfault (22);
	}
	spinlock (&spl_kernel);

	remq (&s->gsq);	/* global queue 	*/
	krn$_scount--;

	spinunlock (&spl_kernel);
}

/*+
 * ============================================================================
 * = krn$_semreinit - Reinitialize a semaphore                                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Reinitialize a semaphore.  Reinitialize the count and value fields.
 *
 * FORM OF CALL:
 *
 *	krn$_semreinit ( s, n, v ) 
 *
 * RETURNS:
 *
 *      0 - success
 *
 * ARGUMENTS:
 *
 *      struct SEMAPHORE *s - address of semaphore
 *      int               n - initial count  
 *      int               v - semaphore value
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void krn$_semreinit (struct SEMAPHORE *s, int n, int v) {

#if EXTRA
	if (s->count < 0) {
	    pprintf ("krn$_semreinit:  semaphore %s has waiters\n", s->name);
	    sysfault (22);
	}
#endif
	spinlock (&spl_kernel);
	s->count = n;
	s->value = v;
	spinunlock (&spl_kernel);
}

/*+
 * ============================================================================
 * = krn$_wait - Wait on semaphore                                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Wait on the specified semaphore.
 *	Decrement semaphore count, save the current process context,
 *	get the next process, restore its context and return. 
 *  
 * FORM OF CALL:
 *  
 *	krn$_wait (s)
 *  
 * RETURNS:
 *
 *      x - value associated with the semaphore
 *       
 * ARGUMENTS:
 *
 * 	struct SEMAPHORE *s - address of semaphore
 *
 * SIDE EFFECTS:
 *
 *
-*/
int krn$_wait (struct SEMAPHORE *s) {
	struct PCB *pcb;
        int    x;

	pcb = getpcb ();

	spinlock (&spl_kernel);

#if EXTRA
	if (!s->flink)
	    sysfault (pcb);
#endif

	s->count--;
	if (s->count < 0) {

#if EXTRA
	    /*
	     * A wait at a non 0 ipl that blocks is not allowed
	     */
	    if (console_mode[whoami()] == INTERRUPT_MODE && 
		    spl_kernel.sav_ipl > IPL_RUN) {
		sysfault (pcb);
	    }
#endif

	    pcb->pcb$l_pstate = KRN$K_WAIT;
	    pcb->pcb$a_semp = s;
	    insq (&pcb->pcb$a_flink, s->blink);
	    schedule (1);
	    spinlock (&spl_kernel);

	} else {
	    if (s->owner) {
		s->owner->owner_pcb = pcb;
		s->owner->old_affinity = pcb->pcb$l_affinity;
		s->owner->old_priority = pcb->pcb$l_pri;
		pcb->pcb$l_affinity = s->owner->old_affinity | s->owner->new_affinity;
		pcb->pcb$l_pri = max (s->owner->old_priority, s->owner->new_priority);
	    }
	}

        x = s->value;
        s->value = 0;
	spinunlock (&spl_kernel);

	return x;
}

/*+
 * ============================================================================
 * = krn$_post - Post a semaphore                                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Post or signal a semaphore.  The semaphore's count is incremented. 
 *  
 * FORM OF CALL:
 *  
 *	krn$_post (s, v)
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 * 	struct SEMAPHORE *s	- address of semaphore
 *	int v			- integer to be or into semaphore's value
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void krn$_post (struct SEMAPHORE *s, protoarg int v) {
	struct PCB *old;
	struct PCB *new;
	int	resched;

	resched = 0;
	old = getpcb ();

	spinlock (&spl_kernel);
	if (s->owner) {
	    s->owner->owner_pcb = 0;
	    old->pcb$l_affinity = s->owner->old_affinity;
	    old->pcb$l_pri = s->owner->old_priority;
	    resched = 1;
	}

#if EXTRA
	if (!s->flink)
	    sysfault (old);
#endif

	s->count++;
	s->value |= v;

	if (s->count <= 0) {
	    new = remq (s->flink) - offsetof (struct PCB, pcb$a_flink);
	    new->pcb$l_pstate = KRN$K_READY;
	    new->pcb$a_semp = 0;
	    if (s->owner) {
		s->owner->owner_pcb = new;
		if (new->pcb$l_affinity) {
		    s->owner->old_affinity = new->pcb$l_affinity;
		    new->pcb$l_affinity = s->owner->old_affinity | s->owner->new_affinity;
		} else {
		    s->owner->old_affinity = new->pcb$l_created_affinity;
		    new->pcb$l_created_affinity = s->owner->old_affinity | s->owner->new_affinity;
		}
		s->owner->old_priority = new->pcb$l_pri;
		new->pcb$l_pri = max (s->owner->old_priority, s->owner->new_priority);
	    }
	    insq (&new->pcb$a_flink, readyq [new->pcb$l_pri].blink);

	    if (new->pcb$l_pri > old->pcb$l_pri && 
		    !(console_mode[whoami()] == INTERRUPT_MODE &&
			    spl_kernel.sav_ipl > IPL_RUN)) {
		resched = 1;
	    }
	}

	if (resched) {
	    old->pcb$l_pstate = KRN$K_READY;
	    insq (&old->pcb$a_flink, &readyq [old->pcb$l_pri].flink);
	    schedule (1);
	} else {
	    spinunlock (&spl_kernel);
	}
}

/*+
 * ============================================================================
 * = krn$_bpost - Post a binary semaphore                                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Post or signal a binary semaphore.
 *	The semaphore's count is incremented. 
 *  
 * FORM OF CALL:
 *  
 *	krn$_bpost (s , v)
 *  
 * RETURNS:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 * 	struct SEMAPHORE *s	- address of semaphore
 *	int v			- integer to be or into semaphore's value
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void krn$_bpost (struct SEMAPHORE *s, protoarg int v) {
	struct PCB *old;
	struct PCB *new;
	int	resched;
	int	new_pri;

	resched = 0;
	old = getpcb ();

	spinlock (&spl_kernel);
	if (s->owner)
	    sysfault (s);

#if EXTRA
	if (!s->flink)
	    sysfault (old);
#endif

        s->value |=v;
	if (s->count <= 0) {
	    new_pri = 0;
	    if (s->count == 0) {
		s->count = 1;
	    } else {
		s->count = 0;
		while (s->flink != & s->flink) {
		    new = remq (s->flink) - offsetof (struct PCB, pcb$a_flink);
		    new->pcb$l_pstate = KRN$K_READY;
		    new->pcb$a_semp = 0;
		    insq (&new->pcb$a_flink, readyq [new->pcb$l_pri].blink);
		    new_pri = max (new_pri, new->pcb$l_pri);
		}
	    }

	    /* Only perform the context switch if the PCB posting is less than the posted PCB. */
	    if (new_pri > old->pcb$l_pri && 
		    !(console_mode[whoami()] == INTERRUPT_MODE && 
			    spl_kernel.sav_ipl > IPL_RUN)) {
		resched = 1;
	    }
	}

	if (resched) {
	    old->pcb$l_pstate = KRN$K_READY;
	    insq (&old->pcb$a_flink, &readyq [old->pcb$l_pri].flink);
	    schedule (1);
	} else {
	    spinunlock (&spl_kernel);
	}
}

/*+
 * ============================================================================
 * = krn$_post_wait - Post one semaphore and wait on another atomically       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Post one semaphore and then wait on another semaphore.
 *	The current process is placed on the semaphore queue
 *	and a resechedule is performed. 
 *  
 * FORM OF CALL:
 *  
 *	krn$_post_wait (s1, s2, v)
 *  
 * RETURNS:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 * 	struct SEMAPHORE *s1	- address of semaphore to post
 *	struct SEMAPHORE *s2 	- address of semaphore to wait on
 *	int v			- integer to or into semaphore's value
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int krn$_post_wait (struct SEMAPHORE *s0, struct SEMAPHORE *s1, protoarg int v) {
	struct PCB *old;
	struct PCB *new;

	old = getpcb ();

	spinlock (&spl_kernel);
	if (s0->owner) {
	    s0->owner->owner_pcb = 0;
	    old->pcb$l_affinity = s0->owner->old_affinity;
	    old->pcb$l_pri = s0->owner->old_priority;
	}

#if EXTRA
	if (!(s0->flink && s1->flink))
	    sysfault (old);
#endif

	/*
	 * Post the first semaphore
	 */
	s0->count++;
	s0->value |= v;
	if (s0->count <= 0) {
	    new = remq (s0->flink) - offsetof (struct PCB, pcb$a_flink);
	    new->pcb$l_pstate = KRN$K_READY;
	    new->pcb$a_semp = 0;
	    if (s0->owner) {
		s0->owner->owner_pcb = new;
		if (new->pcb$l_affinity) {
		    s0->owner->old_affinity = new->pcb$l_affinity;
		    new->pcb$l_affinity = s0->owner->old_affinity | s0->owner->new_affinity;
		} else {
		    s0->owner->old_affinity = new->pcb$l_created_affinity;
		    new->pcb$l_created_affinity = s0->owner->old_affinity | s0->owner->new_affinity;
		}
		s0->owner->old_priority = new->pcb$l_pri;
		new->pcb$l_pri = max (s0->owner->old_priority, s0->owner->new_priority);
	    }
	    insq (&new->pcb$a_flink, readyq [new->pcb$l_pri].blink);
	}

	/*
	 * Wait on the second semaphore	
	 */
	s1->count--;
	if (s1->count < 0) {
	    old->pcb$l_pstate = KRN$K_WAIT;
	    old->pcb$a_semp = s1;
	    insq (&old->pcb$a_flink, s1->blink);
	} else {
	    old->pcb$l_pstate = KRN$K_READY;
	    if (s1->owner) {
		s1->owner->owner_pcb = old;
		s1->owner->old_affinity = old->pcb$l_affinity;
		s1->owner->old_priority = old->pcb$l_pri;
		old->pcb$l_affinity = s1->owner->old_affinity | s1->owner->new_affinity;
		old->pcb$l_pri = max (s1->owner->old_priority, s1->owner->new_priority);
	    }
	    insq (&old->pcb$a_flink, &readyq [old->pcb$l_pri].flink);
	}

	schedule (1);

	return s1->value;
}
