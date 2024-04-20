/* file:	process.c
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
 *	Machine independent kernel routines that manipualate processes.
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
 *	pel	13-May-1992	krn$_create; malloc stack from memzone if
 *				negative stack size argument.
 *				
 *	ajb	13-Feb-1992	reschedule at end of krn$_create isn't
 *				necessary.
 *
 *	ajb	17-Sep-1991	Broken out from kernel.c
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:prdef.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"


struct QUEUE		_align (QUADWORD) pcbq;		/* linked list of all known processes	*/
struct SEMAPHORE	_align (QUADWORD) create$;
struct SEMAPHORE	_align (QUADWORD) deadpcb$;
struct QUEUE		_align (QUADWORD) dead_pcbq;	/* dead pcbs that need to be freed	*/
int			_align (QUADWORD) krn$_pcount;	/* process count	*/

#define	LOG_START	1
#define LOG_STOP	2
#define LOG_STACK	4
#define	FLOODSTACK_VALUE	0xfe

#if MODULAR
#define LOG_REPORT_LIMIT 80
#define LOG_DEFAULT LOG_STACK
#else
#define LOG_DEFAULT 0
#endif

int			process_logging = LOG_DEFAULT;	/* by default is off	*/
							/* bit 0 - log creation */
							/* bit 1 - log death		*/
							/* bit 2 - stack flood/check	*/

extern int		_align (QUADWORD) krn$_sequence;
extern struct QUEUE	_align (QUADWORD) readyq [];
extern struct LOCK	_align (QUADWORD) spl_kernel;
extern sysfault ();
extern null_procedure ();
extern struct ZONE defzone;		/* firmware heap header */
extern struct ZONE *memzone;		/* memzone heap  header */
extern int primary_cpu;

/*+
 * ============================================================================
 * = krn$_process - perform process startup/rundown functions                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This reentrant routine is used to perform process startup and rundown
 *	in process context.  When a process is started by krn$_create, the PC
 *	stored in the PCB points to this routine.  This routine opens up the
 *	process' files, invokes the process itself, and handles running down
 *	of the various resources the process has consumed.
 *
 *	Posts parent's sem to let krn$_create know when it has opened the 
 *	stdin/out/err files.
 *  
 * FORM OF CALL:
 *  
 *	This routine is never directly called.
 *  
 * RETURNS:
 *
 *	This routine never returns directly, but invokes krn$_kill
 *       
 * ARGUMENTS:
 *
 *	none - this is not a routine that is called
-*/
void krn$_process () {
	struct FILE *fp;
	struct PCB  *pcb;
	int	i;
	int	*arg;

        pcb = getpcb();   

	pcb->pcb$l_exit_status = msg_success;


	/*
	 * If this process has a completion semaphore, change that semaphore's
	 * name to the pcb address (so that it shows up in the ps command).
	 */
	if (pcb->pcb$a_completion) {
	    spinlock (&spl_kernel);
	    sprintf (pcb->pcb$a_completion->name, "%08X", pcb);
	    spinunlock (&spl_kernel);
	}

	    
	/*
	 * The filenames point to malloc'ed memory, change ownership of that
	 * memory to this process 
	 */
	dyn$_chown( pcb->pcb$a_stdin_name);
	dyn$_chown( pcb->pcb$a_stdout_name);
	dyn$_chown( pcb->pcb$a_stderr_name);

	/*
	 * Call the user defined startup routine
	 */
	arg = & pcb->pcb$l_argv [0];
	(*pcb->pcb$a_startup) (arg [0], arg [1], arg [2], arg [3]);

	pcb->pcb$a_stderr = 0;
	pcb->pcb$a_stdin  = 0;
	pcb->pcb$a_stdout = 0;

	/*
	 * Open up the standard i/o channels
	 */
	do {
	    if ((pcb->pcb$a_stderr = fopen (pcb->pcb$a_stderr_name, pcb->pcb$b_stderr_mode)) == NULL) continue;
	    if ((pcb->pcb$a_stdin  = fopen (pcb->pcb$a_stdin_name, pcb->pcb$b_stdin_mode)) == NULL) continue;
	    if ((pcb->pcb$a_stdout = fopen (pcb->pcb$a_stdout_name, pcb->pcb$b_stdout_mode)) == NULL) continue;
	    krn$_post( pcb->pcb$a_opened); /* notify that std files are opened */
	    pcb->pcb$l_affinity = pcb->pcb$l_created_affinity;
	    reschedule (1);
	    /*
	     * Call the task itself then the rundown routine
	     */
	    if (!setjmp(PROC_KILL_SJ_ID)) {
		pcb->pcb$l_exit_status = (*pcb->pcb$a_task) (
		    arg [0],
		    arg [1],
		    arg [2],
		    arg [3]
		);
	    }
	    (*pcb->pcb$a_rundown) (arg [0], arg [1], arg [2], arg [3]);
	} while (0);

	/*
	 * Verify that the files were opened correctly
	 */
	if ( (pcb->pcb$a_stderr == NULL) || (pcb->pcb$a_stdin == NULL)
	  || (pcb->pcb$a_stdout == NULL) )
	    krn$_post( pcb->pcb$a_opened); /* notify that std files are opened */

	/*
	 * Close out all open files.
	 */

	pcb->pcb$a_stderr = 0;
	while ((fp = pcb->pcb$r_fq.flink) != &pcb->pcb$r_fq.flink) {
	    fclose (fp);
	}

	/*
	 * Release all the dynamic memory associated with this process
	 * except for the pcb itself.
	 */
	dyn$_flush (&pcb->pcb$r_dq);

	krn$_kill ();
	sysfault (21);
}

/*+
 * ============================================================================
 * = krn$_findpcb - translate a PID into a PCB 	                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This function translates a PID into a PCB and returns the PCB with
 *	elevated IPL and the kernel spinlock acquired.  If the PCB can't
 *	be found, the spinlock and IPL are restored.
 *
 *	By returning in prvileged context, callers are assured that the PCB
 *	won't be changing underneath them once found.  Callers are required
 *	to lower IPL and release the spinlock.  
 *  
 * FORM OF CALL:
 *  
 *	krn$_findpcb (pid)
 *  
 * RETURNS:
 *
 *	x - PCB address if found, 0 if not found
 *       
 * ARGUMENTS:
 *
 * 	int pid - Process ID
 *
 * SIDE EFFECTS:
 *
 *	If PCB is found, returns with elevated IPL and kernel spinlock
 *	taken out.
 *
-*/
struct PCB *krn$_findpcb (int pid) {
	struct QUEUE	*q;
	int		found;
	struct PCB	*pcb;

	spinlock (&spl_kernel);


	/*
	 * Walk down the PCB queue
	 */
	q = pcbq.flink;
	while ((void *) q != (void *) & pcbq.flink) {
	    pcb = (struct PCB *) ((int) q - offsetof (struct PCB, pcb$r_pq));
	    if (pcb->pcb$l_pid == pid) {
		return pcb;
	    }
	    q = q->flink;
	}

	/*
	 * Didn't find the process, so release the locks and return 0.
	 */
	spinunlock (&spl_kernel);
	return 0;
}


/*
 * Several programs have a need to discover the parent TTY, so that they may
 * talk to it directly (as in the case of EDIT and MORE).  Starting with self,
 * and then looking at parents, search for a TTY on stdin, stderr.
 *
 * Return the name if one is found, or a null pointer otherwise.
 */
int krn$_find_controlling_tt (char *name) {
	struct PCB *pcb;

	pcb = getpcb ();
	
	do {
	    if (isatty (pcb->pcb$a_stdin)) {
		strcpy (name, pcb->pcb$a_stdin_name);
		return name;
	    } else if (isatty (pcb->pcb$a_stderr)) {
		strcpy (name, pcb->pcb$a_stderr_name);
		return name;
	    }

	    /* advance to parent */
	    pcb = krn$_findpcb (pcb->pcb$l_parentpid);
	    spinunlock (&spl_kernel);

	} while (pcb);
	return 0;
}


/*+
 * ============================================================================
 * = krn$_setaffinity - set a process' affinity mask                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Set the affinity mask for a process.  This function needs to be
 *	synchronized because the setter and the settee may not necessarily be
 *	running on the same cpu.  Setting the affinity mask does not
 *	immeadiately cause the target process to start executing
 *	on the new processor set.
 *  
 * FORM OF CALL:
 *  
 *	krn$_setaffinity (pid, affinity)
 *  
 * RETURNS:
 *
 *	x - previous affinity mask
 *       
 * ARGUMENTS:
 *
 * 	int pid - process id
 *	int affinity - affinity bitmask
 *
 * SIDE EFFECTS:
 *
 *	Target process will continue to run on current processor until next
 *	reschedule.
 *
-*/
int krn$_setaffinity (int pid, int affinity) {
	struct PCB	*pcb;
	int		old_affinity;

	old_affinity = 0;

	if (pcb = krn$_findpcb (pid)) {
	    old_affinity = pcb->pcb$l_affinity;
	    pcb->pcb$l_affinity = affinity;
	    spinunlock (&spl_kernel);
	}
	return old_affinity;
}

/*+
 * ============================================================================
 * = krn$_setpriority - set a process's software priority                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Set the software priority for a process.  This function needs to be
 *	synchronized because the setter and the settee may not necessarily be
 *	running on the same cpu.  Chaning priority will not necessary have an
 *	immeadiate effect on the process' quantum or scheduling.
 *  
 * FORM OF CALL:
 *  
 *	krn$_setpriority (pid, priority)
 *  
 * RETURNS:
 *
 *	x - previous process priority
 *       
 * ARGUMENTS:
 *
 * 	int pid - process id
 *	int priority - new process priority
 *
 * SIDE EFFECTS:
 *
 *	Target process will continue to run on current processor until next
 *	reschedule.
 *
-*/
int krn$_setpriority (int pid, int priority) {
	struct PCB	*pcb;
	int		old_priority;

	old_priority = 0;

	if (pcb = krn$_findpcb (pid)) {
	    old_priority = pcb->pcb$l_pri;
	    pcb->pcb$l_pri = priority;
	    spinunlock (&spl_kernel);
	}
	return old_priority;
}


/*+
 * ============================================================================
 * = krn$_create - Create a task                                              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Create a task.  Not all fields in the PCB are passed in the
 *	argument list to the create routine; we assume a friendly 
 *	environment and tasks may modify their own PCBs
 *	as long as they are aware of the side effects.
 *
 *	If NULL is specified for files and their modes, then krn$_create
 *	uses the parent's file names and access modes.
 *
 *	Don't return until the created process has opened its stdin/err/out
 *	files.
 *
 *	Return the PID of the created process, or NUL.
 *
 * FORM OF CALL:
 *
 *	
 * krn$_create ( pc, startup, sem, pri, affinity, stack, name,
 *		in, in_mode,
 *		out, out_mode,
 *		err, err_mode,
 *		a0, a1, a2, a3)
 *
 * RETURNS:
 *
 *	x - PID of created process
 *
 * ARGUMENTS:
 *
 *	int	pc	      -	procedure address		
 *	int	startup	      - user defined startup routine
 *	struct SEMAPHORE *sem -	address of completion semaphore (0 for none)
 *	int	pri	      -	priority			
 *	int	affinity      -	cpu affinity mask		
 *	int	actual_stack  -	actual stack size; if negative then use memzone
 *	char	*name	      -	process name
 *	char	*in	      -	name of input channel	
 *	char	*in_mode      - access mode of input channel
 *	char	*out	      -	name of output channel
 *	char	*out_mode     - access mode of output channel
 *	char	*err	      -	name of error channel
 *	char	*err_mode     - access mode of error channel
 *
 * SIDE EFFECTS:
 *
 *      Another process is started.
 *
-*/
int krn$_create (
	int	pc,		/* procedure address		*/
	int	startup,	/* user defined startup		*/
	struct SEMAPHORE *sem,	/* address of completion semaphore	*/
	int	pri,		/* priority			*/
	int	affinity,	/* cpu affinity mask		*/
	int	actual_stack,	/* actual stack size		*/
	char	*name,
	char	*in,	       	/* name of input channel	*/
	char	*in_mode,	/* access mode			*/
	char	*out,		/* name of output channel	*/
	char	*out_mode,	/* access mode			*/
	char	*err,		/* name of error channel	*/
	char	*err_mode,	/* access mode			*/
	protoargs int	a0,
	int	a1,
	int	a2,
	int	a3
) {
	  /* posted by created process just after stdin/out/err files opened */
	struct SEMAPHORE opened;
	struct PCB 	*pcb, *parent;
	struct CFR	*cfp;
	struct ZONE	*zp;
	char		done_name [MAX_NAME];
	int		i;   /* at end of routine contains pid of new process*/
	int		abs_stack;
	int		stack_index;
	int		*sp;
	unsigned int	*pd;

	krn$_wait(&create$);

	parent = getpcb ();

	/*
	 * Figure out our stack size
	 */
	if (actual_stack < 0)
	    zp = memzone;
	else
	    zp = &defzone;
	abs_stack = abs( actual_stack);
	if (abs_stack < KRN$K_MINSTACK) abs_stack = KRN$K_MINSTACK;


	/*
	 * Allocate a PCB and fill in the easy stuff first.  The pcb can't
	 * go on this process's dynamic memory ownership queue, otherwise it
	 * gets deleted if this process dies.
	 */
#if MODULAR
	if (actual_stack < 0)
	    pcb = (void *) dyn$_malloc (sizeof (struct PCB) + abs_stack,
	      DYN$K_SYNC|DYN$K_NOOWN|DYN$K_NOFLOOD|DYN$K_ZONE, DYN$K_GRAN, 0, zp);
	else
	    pcb = (void *) dyn$_malloc (sizeof (struct PCB) + abs_stack,
	      DYN$K_SYNC|DYN$K_NOOWN|DYN$K_NOFLOOD, DYN$K_GRAN, 0, 0);
#else
	pcb = (void *) dyn$_malloc (sizeof (struct PCB) + abs_stack,
	  DYN$K_SYNC|DYN$K_NOOWN|DYN$K_NOFLOOD|DYN$K_ZONE, DYN$K_GRAN, 0, zp);
#endif
	memset (pcb, 0, sizeof (struct PCB));
	strncpy (& pcb->pcb$b_name [0], name, sizeof (pcb->pcb$b_name));


	pcb->pcb$a_stdin_name  = dyn$_malloc( max( in ? strlen(in) : 0, 
		strlen( parent->pcb$a_stdin_name))+1, DYN$K_SYNC | DYN$K_NOOWN);
	pcb->pcb$a_stdout_name = dyn$_malloc( max( out ? strlen(out) : 0,
		strlen(parent->pcb$a_stdout_name))+1, DYN$K_SYNC | DYN$K_NOOWN);
	pcb->pcb$a_stderr_name = dyn$_malloc( max( err ? strlen(err) : 0,
		strlen(parent->pcb$a_stderr_name))+1, DYN$K_SYNC | DYN$K_NOOWN);
	pcb->pcb$l_stacksize = abs_stack;
	pcb->pcb$l_pri = pri;
	if (affinity) {
	    pcb->pcb$l_created_affinity = affinity;
	} else {
	    pcb->pcb$l_created_affinity = 1 << primary_cpu;
	}
	pcb->pcb$l_affinity = -1;
	pcb->pcb$a_task = (void *) pc;
	if (startup)
	    pcb->pcb$a_startup = (void *) startup;
	else
	    pcb->pcb$a_startup = (void *) null_procedure;
	pcb->pcb$a_completion = sem;
	pcb->pcb$l_pstate = KRN$K_READY;
	pcb->pcb$l_killpending = 0;
	pcb->pcb$l_shellp = 0;	/* assume we're not a shell */

	/*
	 * Set up default process quantums
	 */
	pcb->pcb$l_quantum = DEF_QUANTUM;
	pcb->pcb$l_qexpire = pcb->pcb$l_quantum;
	pcb->pcb$l_qexpire_dec = DEF_QUANTUM;
	pcb->pcb$q_cputime [0] = 0;
	pcb->pcb$q_cputime [1] = 0;

	/*
	 * Pre poison the stack if enabled (usefull for measuring stack
	 * utilization at image rundown).
	 */
	if (process_logging & LOG_STACK) {
	    memset (&pcb->pcb$b_stack [0], FLOODSTACK_VALUE, pcb->pcb$l_stacksize);
	}

	/*
	 * Initialize our resource ownership queues
	 */
	pcb->pcb$r_ctrlcq.flink = (void *) & pcb->pcb$r_ctrlcq.flink;
	pcb->pcb$r_ctrlcq.blink = (void *) & pcb->pcb$r_ctrlcq.flink;

	pcb->pcb$r_dq.flink = (void *) & pcb->pcb$r_dq.flink;	/* dynamic memory */
	pcb->pcb$r_dq.blink = (void *) & pcb->pcb$r_dq.flink;

	pcb->pcb$r_hq.flink = (void *) & pcb->pcb$r_hq.flink;	/* handlers */
	pcb->pcb$r_hq.blink = (void *) & pcb->pcb$r_hq.flink;

	pcb->pcb$r_sjq.flink = (void *) & pcb->pcb$r_sjq.flink;	/* setjmps */
	pcb->pcb$r_sjq.blink = (void *) & pcb->pcb$r_sjq.flink;

	pcb->pcb$r_iobq.flink = (void *) & pcb->pcb$r_iobq.flink; /* IOBs */
	pcb->pcb$r_iobq.blink = (void *) & pcb->pcb$r_iobq.flink;

	pcb->pcb$r_fq.flink = (void *) & pcb->pcb$r_fq.flink;	/* files */
	pcb->pcb$r_fq.blink = (void *) & pcb->pcb$r_fq.flink;

	/*
	 * Supply a default rundown routine (tasks may wire in
	 * their own routines by writing the address of a procedure
	 * into their PCB).
	 */
	pcb->pcb$a_rundown = null_procedure;

	/*
	 * Set up the task's argument list, supplied to this routine as
	 * optional arguments.  Since we can't find out our own arguments,
	 * we unconditionally assume 4.
	 */
	pcb->pcb$l_argc = 4;
	pcb->pcb$l_argv [0] = a0;
	pcb->pcb$l_argv [1] = a1;
	pcb->pcb$l_argv [2] = a2;
	pcb->pcb$l_argv [3] = a3;

	/*
	 * Set up the saved context so that when the task is scheduled,
	 * a helper routine does the actual call to the task, and when
	 * the task returns, the rundown routines are called.
	 */
	sp = (int *) (((int) pcb->pcb$b_stack + pcb->pcb$l_stacksize - 8) & ~7);
	krn$_seminit ( &opened, 0, "std");
	pcb->pcb$a_opened = &opened;
	pd = getpd( krn$_process );
	pcb->pcb$r_alpha_hw_pcb.alpha_hw_pcb$q_r26 [0] = pd [2];
	pcb->pcb$r_alpha_hw_pcb.alpha_hw_pcb$q_r27 [0] = pd;
	pcb->pcb$r_alpha_hw_pcb.alpha_hw_pcb$q_r29 [0] = 0;
	pcb->pcb$r_alpha_hw_pcb.alpha_hw_pcb$q_r30 [0] = sp;
	/*
	 * Set up stdin, stdout, and stderr.  If any of these fields are NULL,
	 * then inherit the parent's values.
	 */
	strcpy (pcb->pcb$a_stdin_name, in ? in : parent->pcb$a_stdin_name);
	strcpy (pcb->pcb$b_stdin_mode, in_mode ? in_mode : parent->pcb$b_stdin_mode);

	strcpy (pcb->pcb$a_stdout_name, out ? out : parent->pcb$a_stdout_name);
	strcpy (pcb->pcb$b_stdout_mode, out_mode ? out_mode : "a");

	strcpy (pcb->pcb$a_stderr_name, err ? err : parent->pcb$a_stderr_name);
	strcpy (pcb->pcb$b_stderr_mode, err_mode ? err_mode : "a");

	krn$_post(&create$); 		/* setup completed */

	/*
	 * insert PCB onto pcb queue and readyq.  Once on the readyq
	 * its alive.
	 */
	spinlock (&spl_kernel);

	while (++krn$_sequence == 0) {
	}
	i = pcb->pcb$l_pid = krn$_sequence;
	pcb->pcb$l_parentpid = parent->pcb$l_pid;
	insq (&pcb->pcb$r_pq, &pcbq);
	insq (&pcb->pcb$a_flink, readyq [pri].blink);
	krn$_pcount++;

	/*
	 * report process startups
	 */
	if (process_logging & LOG_START) {
	    qprintf ("starting pid %08X %10.10s from %s pcb = %08X\n", 
		i, name, parent->pcb$b_name, pcb
	    );
	}

	spinunlock (&spl_kernel);

	/*
	 * wait for stdin/out/err files to be opened by created process
	 */
	krn$_wait( &opened);
	krn$_semrelease( &opened);

	return i;	/* return pid of new process */
}


/*+
 * ============================================================================
 * = krn$_delete - delete a process                                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine is called to delete a process.
 *	Process deletion will cause the target process rundown routines to
 *	be executed in the context of the deletee.  If the NOKILL flag in
 *	the PCB is set, then this routine just sets the kill pending flag,
 *	except in the case where the process is deleting itself.
 *  
 *	Release all the resources that a process has acquired.
 *	This includes open files, various exception handlers, dynamic
 *	memory, semaphores, and TQEs (timer queue elements).
 *
 *	When all resources are released, the process is removed from the
 *	system and the optional completion semaphore posted.
 *
 *	Multiple processes on multiple processors can be trying to
 *	simultaneously delete the same process.  To protect the search,
 *	and resolve the race, most of this routine runs under the kernel
 *	spinlock. Loosers of the race must recognize their loss and
 *	exit gracefully.
 *
 * FORM OF CALL:
 *  
 *	krn$_delete (pid)
 *  
 * RETURN CODES:
 *
 *	msg_success	- success
 *	msg_noprocess	- no process
 *       
 * ARGUMENTS:
 *
 * 	int pid - process id
 *
 * SIDE EFFECTS:
 *
 *	Process is deleted.
 *
-*/
int krn$_delete (int pid) {
	int   		i;
	struct PCB	*pcb, *curpcb;
	struct SEMAPHORE *s;

	/*
	 * Translate the pid into a PCB address.  If we can't find
	 * the process, we assume we lost the race condition.  If we do
	 * find the process, we'll inherit the acquired kernel lock and have
	 * won the race.
	 */
	pcb = krn$_findpcb (pid);
	if (pcb == 0) {
	    return (int) msg_noprocess;
	}

	/*
	 * If we're deleting ourselves, we need to use the krn$_kill service
	 */
        curpcb = getpcb ();
	if (curpcb == pcb) {
	    spinunlock (&spl_kernel);
	    krn$_kill (); /* never returns */
	}

	/*
	 * Set the delete pending bit and return
	 */
	pcb->pcb$l_killpending = SIGNAL_KILL;
	spinunlock (&spl_kernel);
	return (int) msg_success;
}

/*+
 * ============================================================================
 * = krn$_dead - Task that deallocates resources from dead processes.         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This task's sole purpose in life is to deallocate resources from
 *	processes that have been killed, and to then release the PCB itself.
 *	These pcbs are put on a dead PCB queue by the krn$_delete code for
 *	consumption by this task.  Processes can't easily deallocate their own
 *	PCB and other resources, since this may cause stack corruption and
 *	other non obvious side effects.  Freeing the pcb (which causes
 *	the stack to go away) destroys the return address from dyn$_free
 *	(assuming memory poisoning is on), therefore making a return
 *	impossible.
 *
 * FORM OF CALL:
 *
 *	krn$_dead ()
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
 *      Dynamic memory is released. Entries removed from dead_pcbq queue.
 *
-*/
void krn$_dead (void) {
	unsigned char		*c;
	int			i;
	struct QUEUE		*q;
	struct SEMAPHORE	*completion, *s;
	struct FILE		*fp;
	struct PCB		*pcb;
     	int			status;

	pcb = getpcb ();

	/*
	 * Run forever waiting for processes to delete
	 */
	while (1) {
	    krn$_wait (&deadpcb$);

	    /*
	     * The dead PCB queue is synchronized with the kernel spinlock
	     */
	    spinlock (&spl_kernel);
	    pcb = (struct PCB *) ((int) dead_pcbq.flink - offsetof (struct PCB, pcb$r_pq));
	    remq (dead_pcbq.flink);	/* needs to be atomic */
	    krn$_pcount--;
	    spinunlock (&spl_kernel);

	    /*
	     * check stack usage
             */
	    i = 0;
	    if (process_logging & LOG_STACK) {
		c = pcb->pcb$b_stack;
		for (i = pcb->pcb$l_stacksize; i > 0; i--) {
		    if (*c++ != FLOODSTACK_VALUE) {
			break;
		    }
		}
	    }

#if MODULAR
	    /*
	     * if more than 80% of the stack has been utilized then report to
	     * event log
	     */

	    if ( process_logging & LOG_STACK )
		if ((( i * 100 ) / pcb->pcb$l_stacksize ) >= LOG_REPORT_LIMIT )
		    qprintf ("Stack >80%% utilized - pid %08X %10.10s status %08X stack (%d/%d)\n",
		    pcb->pcb$l_pid,
		    pcb->pcb$b_name,
		    pcb->pcb$l_exit_status,
		    i,
		    pcb->pcb$l_stacksize
		);
#endif

	    /*
	     * report process rundown to the event logger
	     */
	    if (process_logging & LOG_STOP) {
		qprintf ("stopping pid %08X %10.10s status %08X stack (%d/%d)\n",
		    pcb->pcb$l_pid,
		    pcb->pcb$b_name,
		    pcb->pcb$l_exit_status,
		    i,
		    pcb->pcb$l_stacksize
		);
	    }

	    /*
	     * Deallocate the PCB directly.
	     */
	    completion = pcb->pcb$a_completion;
	    status = pcb->pcb$l_exit_status;
	    dyn$_free (pcb, DYN$K_SYNC);

            /* 
	     * Finally post the completion semaphore if the user asked for it.
	     */
	    if (completion) {
		krn$_post (completion, status);
	    }
	}
}

/*+
 * ============================================================================
 * = krn$_walkpcb - Find the state of a given process                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Walk down the PCB queue and list all processes known to the system.
 *	Walking down the queue must be synchronized, since processes may be
 *	coming and going (and thus altering flink/blinks) as the reporting is
 *	taking place.  Cloning the PCBs into a private data structure is
 *	difficult since the size of such a data structure would be large.
 *	Acquiring the spinlock for the entire duration of the report would
 *	cause a deadlock.
 *
 * FORM OF CALL:
 *
 *	krn$_walkpcb ()
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
void krn$_walkpcb (void) {
	struct QUEUE	*p;
	struct PCB	*pcb;
	char		state[25];
	int		p_estimate;	/* approx number of processes	*/
	int		p_found;       	/* number of processes found	*/
	struct {
		int	pid;
		int	pcb;
		int	pri;
                int	affinity;
                int	last_processor;
		int	cputime [2];
		int	pstate;
		char	name [MAX_NAME];
		char	sname [MAX_NAME];
	} *lp, *clone;

	/*
	 * Allocate a character buffer that will hold copies of the
	 * variables we're interested in.
	 */
	p_estimate = krn$_pcount + 8;
	clone = (void *) dyn$_malloc (p_estimate * sizeof (*clone), DYN$K_SYNC);

	/*
	 * Quickly freeze the system and extract all the necessary information
	 * into a local buffer.
	 */
	lp = clone;
	p_found = 0;
	spinlock (&spl_kernel);
	p = pcbq.flink;
	while (p != &pcbq) {
	    if (p_found >= p_estimate) break;
	    pcb = (struct PCB *) ((unsigned int) p - offsetof (struct PCB, pcb$r_pq));
	    lp->pid = pcb->pcb$l_pid;
	    lp->pcb = (int) pcb;
	    lp->affinity = pcb->pcb$l_affinity;
	    lp->last_processor = pcb->pcb$l_last_processor;
	    lp->pri = pcb->pcb$l_pri;
	    lp->cputime [0] = pcb->pcb$q_cputime [0];
	    lp->cputime [1] = pcb->pcb$q_cputime [1];
	    strcpy (&lp->name [0], pcb->pcb$b_name);
	    lp->pstate = pcb->pcb$l_pstate;
	    if (pcb->pcb$l_pstate == KRN$K_WAIT) {
		strcpy (&lp->sname [0], pcb->pcb$a_semp->name);
	    }
	    p_found++;
	    lp++;
	    p = p->flink;
	}
	spinunlock (&spl_kernel);

	
	/*
	 * Now we can print out the data at our leisure.
	 */
	printf (" ID       PCB     Pri CPU Time Affinity CPU  Program    State\n");
	printf ("-------- -------- --- -------- -------- --- ---------- ------------------------\n");
	lp = clone;
	while (p_found--) {
	    if (killpending ()) {
		dyn$_free (clone, DYN$K_SYNC);
		return;
	    }
	    switch (lp->pstate) {
		case KRN$K_NULL:
		    sprintf (state, "null");
		    break;
		case KRN$K_WAIT:
		    sprintf (state, "waiting on %-13.13s", lp->sname);
		    break;
		case KRN$K_READY:
		    sprintf (state, "ready");
		    break;
		case KRN$K_RUNNING:
		    sprintf (state, "running");
		    break;
		default:
		    sprintf (state, "?");
		    break;
	    }
#if MAX_PROCESSOR_ID > 16
	    printf ("%08x %08x %d %10d %08x %-2d %11.11s %s\n",
#else
	    printf ("%08x %08x %d %10d %08x %d %12.12s %s\n",
#endif
		lp->pid,
		lp->pcb,
		lp->pri,
		lp->cputime [0],
                lp->affinity,
		lp->last_processor,
		lp->name,
		state
	    );
	    lp++;
	}

	dyn$_free (clone, DYN$K_SYNC);
}

/*+
 * ============================================================================
 * = krn$_kill - Kill a process                                               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Take the current PCB and remove it from the system by placing it on
 *	a dead PCB queue.  Post a semaphore that triggers a routine that
 *	deallocates dead PCBs.
 *
 *  
 * FORM OF CALL:
 *  
 *	krn$_kill ()
 *  
 * RETURNS:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 * 	struct *PCB - address of pcb to be killed
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void krn$_kill () {
	struct PCB *pcb;
	struct SEMAPHORE *s;

	pcb = getpcb ();

	s = & deadpcb$;

	spinlock (&spl_kernel);
	pcb->pcb$l_pstate = KRN$K_NULL;

	remq (&pcb->pcb$r_pq.flink);
	insq (&pcb->pcb$r_pq, &dead_pcbq);

	/*
	 * remove ourselves from the control C queue
	 */
	remq (&pcb->pcb$r_ctrlcq);

	s->count++;
	if (s->count <= 0) {
	    pcb = remq (s->flink) - offsetof (struct PCB, pcb$a_flink);
	    pcb->pcb$l_pstate = KRN$K_READY;
	    insq (&pcb->pcb$a_flink, readyq [pcb->pcb$l_pri].blink);
	}

	schedule (1);
}

/*+
 * ============================================================================
 * = killpending - Return the "kill pending" state for a process              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Return the "kill pending" state for a process.  The kill pending field
 *	can have 3 states:
 *		0 - normal operation,
 *		1 - process is requested to die
 *		2 - process is requested to die by the control C handler.
 *
 * FORM OF CALL:
 *
 *	killpending( )
 *
 * RETURN CODES:
 *
 *      0 - no "kill pending" bits are set
 *      1 - at least one "kill pending" bit is set
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
int killpending( )
    {
    struct PCB *pcb;


    pcb = getpcb ();

    if (pcb->pcb$l_killpending) return 1;
    return 0;
}

/*+
 * ============================================================================
 * = check_kill - Terminate a process if the killpending bit is set.          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Cause the process to terminate if killpending is set in the PCB.
 *
 * FORM OF CALL:
 *
 *	check_kill()
 *
 * RETURN CODES:
 *
 *      0 - the "kill pending" bit is not set
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
int check_kill()
{
    struct PCB *pcb;


    pcb = getpcb ();

    if (pcb->pcb$l_killpending)
	longjmp(PROC_KILL_SJ_ID, 1);
    else
	return 0;
}


/*+
 * ============================================================================
 * = stack_left - Return the number of bytes that are left on a process stack =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Read the stack pointer, and retrun the number of bytes left on the stack
 *	before the stack overflows.  A negative number indicates that the stack
 *	is already overflowed.
 *
 * FORM OF CALL:
 *
 *	stack_left (verbose_flag)
 *
 * RETURN CODES:
 *
 *      n - number of bytes left on the stack
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
int stack_check () {
	struct PCB *pcb;
	int delta;
	char *sp;

	pcb = getpcb ();
	sp = get_sp ();
	delta = sp - & pcb->pcb$b_stack [0];

	if (delta < 300) {
	    qprintf ("stack limit check on pid %08X, process %s\n",
		pcb->pcb$l_pid,
		pcb->pcb$b_name
	    );
	}

	return delta;

}

#if MODULAR
void show_process_stack_usage ( int *sp ) {
	unsigned char		*c;
	int			i;
	struct PCB	*pcb = getpcb();

    if (mfpr_ipl() == IPL_RUN)
	spinlock (&spl_kernel);

	/*
	 * check stack usage
         */
	 
	i = 0;
	c = pcb->pcb$b_stack;
	for (i = pcb->pcb$l_stacksize; i > 0; i--) {
	    if (*c++ != FLOODSTACK_VALUE) {
		break;
	    }
	}

	pprintf ("\n\n ID       PCB         Program  StackStart StackEnd  Highwater Usage    SP\n");
	pprintf ("-------- --------     -------- ---------- --------- --------- -----    --\n");

	pprintf ("%08x %08x     %s      %08x  %08x  %08x  %x/%x %08x\n",
		    pcb->pcb$l_pid,
		    (int) pcb,
		    pcb->pcb$b_name,
		    (((int) pcb->pcb$b_stack + pcb->pcb$l_stacksize - 8) & ~7),
		    pcb->pcb$b_stack,
		    (((int) pcb->pcb$b_stack + pcb->pcb$l_stacksize) & ~7) - ( int ) i,
		    i,
		    pcb->pcb$l_stacksize,
		    sp );
	
    if (mfpr_ipl() == IPL_RUN)
	spinunlock (&spl_kernel);
}
#endif
