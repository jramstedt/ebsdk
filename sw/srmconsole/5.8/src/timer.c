/* file:	timer.c
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
 *	Timer services for the cobra/laser console.
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
 *     gdm	04-Feb-1999	Conditionalize for Eiger
 *
 *     ska	11-Nov-1998	Conditionalize for YukonA
 *
 *	er	15-Jan-1997	Fix EBxxx and PCxxx SROM bug that set the DSE bit in TOY
 *				CSR B.  Make sure this bit is cleared so UNIX does not
 *				complain about perposterous time stored in the TOY.
 *
 * 	eg	09-Oct-1996	Conditionalize using APC_PLATFORM for all EBxxx platoforms.
 *
 *	er	16-May-1996	Removed "CPU speed is ..." print for EBxxx platforms
 *
 *	jcc	25-Jan-1995	Conditionalize for CORTEX
 *
 *	noh	20-Nov-1995	Remove "CPU speed is..." print for BURNS.
 *
 *	dtr	24-may-1995	ipl checks done symbilically
 *
 *	raa	25-Apr-1995	Removal of Medulla cpu speed
 *				speed validation.
 *
 *	rbb	13-Mar-1995	Conditionalize for EB164
 *
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	er	15-Aug-1994	Conditionalize for SD164
 *
 *	er	23-Jun-1994	Conditionalize for EB64+
 *
 *	bobf	23-Jun-1994	Medulla Changes Only:
 *				validate module cpu speed in 
 *				krn$_timer_set_cycle_time.
 *
 *	dtr	12-Apr-1994	Made the declaration for console_mode external
 *				as it is already declared in kernel.
 *
 *	mc	01-Mar-93	Added OCP message for multiple CPUs.
 *
 *	bobf	 3-Feb-1994	Medulla changes only:
 *				1. added 2 global variables
 *				2. redefine CYCLES_PER_TICK and
 *				   DEFAULT_PSEC_PER_CYCLE to access global
 *				   variables
 *				3. added calculation of Medulla_Psec_Per_Cycle
 *				   to Krn$_Timer_Set_Cycle_Time
 *				Changes allow the console to dynamically adapt
 *				to a change in the CPU clock frequency
 *
 *	jmp	11-29-93	Add avanti support
 *
 *	bobf	14-Sep-1993	Modified krn$_reset_toy and timer_interrupt
 *				for MEDULLA platform only.
 *
 *	dwr	10-Sep-1993	Add start/stop_timeout routines
 *
 *	kl	19-Aug-1993	merge	
 *
 *	ajb	18-May-1993	Decouple polling and timing loops
 *
 *	joes	20-Apr-1993	Now use Medulla's RTC for 1KHz tick instead of SIO 
 *				interval timer
 *
 *	joes	13-Apr-1993	Add Medulla timer support
 *
 *	pel	22-Mar-1993	Morgan; Use TOY instead of Harley (A4i) chip
 *				for interval timer.
 *
 *	pel	12-Mar-1993	Morgan; tweak interval timer chip init value.
 *	pel	06-Mar-1993	Init Morgan A4i chip interval timer;
 *				In check_timer, wait for morgan timer interrupt.
 * 
 *	pel	08-Feb-1993	Have krn$_timer_set_cycle_time fudge cycles if
 *				MORGAN_POWERUP for platform debug.
 *				Also remove got_a_timer_interrupt code.
 *
 *	pel	26-Jan-1993	init/set got_a_timer_interrupt global var.
 *				for morgan.
 *
 *	pel	15-Jan-1993	Conditionalize for morgan
 *
 *	ajb	14-Jul-1992	jensen stuff, remove targets
 *
 *	pel	24-Apr-1992	Use rscc instead of rcc for time functions.
 *				Init cycles_per_us to 150 (for now) to time
 *				bus inits during krn$_idle.
 *
 *      phk     02-Mar-1992     Remove fbe memory refresh
 *
 *	kp 	09-Jan-1992	Remove CB_PROTO usage.
 *
 *	pel	18-Dec-1991	add memory refresh for FBE target
 *
 *	pel	17-Dec-1991	timebase_init; add TOY reset for Cobra
 *
 *	pel	27-Nov-1991	Have krn$_micro_delay use cycle timer. Add
 *				calib_micro_timer.
 *
 *	pel    	18-Nov-1991	Make krn$_micro_delay more accurate.
 *
 *	pel	08-Nov-1991	Add krn$_micro_delay.
 *
 *	ajb	22-Oct-1991	Account for process time on timer interrupts
 *
 *	jad	30-Sep-1991	pr101def should be pr1701def.h
 *
 *	ajb	17-Sep-1991	Broken out from kernel.c
 *--
 */

/* $INCLUDE_OPTIONS$ */
#include 	"cp$src:platform.h"
#include	"cp$src:common.h"
#include 	"cp$src:pal_def.h"
#include	"cp$src:prdef.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:impure_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:tt_def.h"
#include	"cp$src:parse_def.h"
#include 	"cp$inc:platform_io.h"
#include 	"cp$src:platform_cpu.h"
#include	"cp$src:alphascb_def.h"

#if API_PLATFORM
#include "cp$src:api_debug.h"
#endif

#if SABLE
#if GAMMA
#include	"cp$src:gamma_cpu.h"
#else
#include	"cp$src:cbr_sbl_cpu.h"
#endif
#include	"cp$src:sable_ocp.h"
extern int sable_ocp_write (struct FILE *fp, int size, int number, unsigned char *buf);
#endif

#if RAWHIDE || (PC264 && !WEBBRICK)
#include	"cp$src:sable_ocp.h"
extern int sable_ocp_write (struct FILE *fp, int size, int number, unsigned char *buf);
#endif

#if TURBO
#include	"cp$src:tlep_gbus_def.h"
#include	"cp$src:turbo_watch.h"
extern		int running_from_bcache;
int	        watch_chip_eco = 1;		/* Default is true */
#endif

#if MEDULLA || CORTEX || YUKONA
#if MEDULLA
#include        "cp$src:medulla_system.h"
#endif
extern 		int int_cnt;
extern 		int hbeat_diag_in_progress;
int 		hbeat_cnt=0;
int 		hbeat_flag=0;
int		medulla_psec_per_cycle;
int		medulla_cycles_per_tick;
int		dont_boot = FALSE;	/* Global flag: don't boot OS */
#define NO_HBEATS 0
#endif

#if ALCOR
#include	"cp$src:mikasa_ocp.h"
extern int mikasa_ocp_write (struct FILE *fp, int size, int number, unsigned char *buf);
extern int alcor_mod_rev;
#endif

#if PRIVATEER
extern int rmc_cmd( unsigned char code, unsigned char size, unsigned char *c, unsigned short cmd_qual );
char ocp_string[17];
#endif

#if PINNACLE
#include "cp$src:emulate.h"
#endif

#include "cp$inc:prototypes.h"

/*
 * Each platform must define the number of milliseconds per timer tick, and
 * also a constant that approximates one timer tick when put in the
 * following loop:  while (delay-- >0);
 *
 * Since timers rarely interrupt at exactly 1.0000000 msec intervals, the
 * actual interval is divided into a number of milliseconds and a nanosecond
 * residue.
 *
 */
/*Default cycle time used if no timer ticks are present*/
#ifndef DEFAULT_PSEC_PER_CYCLE
#define DEFAULT_PSEC_PER_CYCLE 10000                                                                                            
#endif                                                                                                                          
                                                                             
                                                   
#if SABLE || MEDULLA || TURBO || MIKASA || AVANTI || ALCOR || K2 || MTU || CORTEX || YUKONA || TAKARA || APC_PLATFORM || REGATTA || MONET || EIGER || FALCON
#define	MSEC_TICK	0
#define	NSEC_RESIDUE	976562	/* nano seconds per tick residue from above */
#define CYCLES_PER_TICK \
    (((MSEC_TICK*1000*1000+NSEC_RESIDUE)*1000) / DEFAULT_PSEC_PER_CYCLE)
#endif

#if MARVEL
#define	MSEC_TICK	2
#define	NSEC_RESIDUE	0	/* nano seconds per tick residue from above */
#define CYCLES_PER_TICK \
    (((MSEC_TICK*1000*1000+NSEC_RESIDUE)*1000) / DEFAULT_PSEC_PER_CYCLE)
#endif

#if WILDFIRE
#define	MSEC_TICK	2
#define	NSEC_RESIDUE	0	/* nano seconds per tick residue from above */
#define CYCLES_PER_TICK \
    (((MSEC_TICK*1000*1000+NSEC_RESIDUE)*1000) / DEFAULT_PSEC_PER_CYCLE)
#endif

#if RAWHIDE
#define MSEC_TICK       0
#define NSEC_RESIDUE    833333  /* nano seconds per tick residue from above */
#define CYCLES_PER_TICK \
    (((MSEC_TICK*1000*1000+NSEC_RESIDUE)*1000) / DEFAULT_PSEC_PER_CYCLE)
#endif

#if MEDULLA
/*
 * Re-define to access global variables for Medulla only
 */
#define	DEFAULT_PSEC_PER_CYCLE	(medulla_psec_per_cycle)
#define	CYCLES_PER_TICK		(medulla_cycles_per_tick)
#endif

/*
 * The following cells keep track of timer interrupts on a per processor
 * basis.  We also need to track if a processor is receiving timer interrupts.
 */
typedef struct {
	volatile int	tick_count;	/* number of timer interrupts seen by this processor */
	int		no_timer;	/* set if this processor isn't seeing timer interrupts */
	int		no_timer_notify;
	int		psec_cycle;	/* picoseconds per cycle */
	int		cycles_per_second;
	int		cycles_per_microsecond;
	INT		last_rscc;
	int		last_tick;
 	int		rsvd;
} TIMEBASE;
TIMEBASE timebase [MAX_PROCESSOR_ID];

int cpu_mhz [MAX_PROCESSOR_ID];

struct SEMAPHORE	_align (QUADWORD) timer;	/* for the timer task, posted by*/
struct QUEUE		_align (QUADWORD) tqh;		/* linked list of timer queue entries	*/
struct QUEUE		_align (QUADWORD) dqh;		/* linked list of delay queue entries	*/
struct LOCK 		_align (QUADWORD) spl_tq;	/* timer q spinlock	*/

struct SEMAPHORE	_align (QUADWORD) poll;		/* triggers polling	*/
struct QUEUE		_align (QUADWORD) pollq;	/* poll queue */

/*
 * We keep track of time in milliseonds.  Since the clock doesn't interrupt us
 * at exactly 1.000 msec intervals, we have to use reisdues.  These cells are
 * written only by the timer process.
 */
volatile unsigned int	_align (QUADWORD) poweron_msec [2];
volatile unsigned int	nsec_residue;

extern struct QUEUE	_align (QUADWORD) readyq [];
extern struct LOCK spl_kernel;
extern volatile int	_align (QUADWORD) console_mode [MAX_PROCESSOR_ID];

int timer_interrupt ();
extern null_procedure ();
extern sysfault ();
extern int killpending ();
extern int timer_cpu;	/* which cpu handles the timer process */
extern int cpip;	/* cpu powerup in progress */
extern int primary_cpu;
extern int cbip;
extern int in_console;

void krn$_create_delayed (
    int msec,
    int use_tt_not_nl,
    int (*code),
    int (*startup),
    struct SEMAPHORE *sem,
    int pri,
    int affinity,
    int stack,
    char *name,
    protoargs int a0,
    int a1,
    int a2,
    int a3) {
	struct DELAYQ *dqp;

	dqp = dyn$_malloc (sizeof (struct DELAYQ), DYN$K_SYNC|DYN$K_NOOWN);
	dqp->msec = msec;
	dqp->use_tt_not_nl = use_tt_not_nl;
	dqp->code = code;
	dqp->startup = startup;
	dqp->sem = sem;
	dqp->pri = pri;
	dqp->affinity = affinity;
	dqp->stack = stack;
	strncpy(dqp->name, name, MAX_NAME);
	dqp->a0 = a0;
	dqp->a1 = a1;
	dqp->a2 = a2;
	dqp->a3 = a3;

	spinlock (&spl_tq);
	insq (dqp, &dqh);
	spinunlock (&spl_tq);
}

#if ( STARTSHUT || DRIVERSHUT )
int krn$_kill_delayed (char *name) {
	struct DELAYQ *dqp;
	int found;

	found = 0;
	spinlock (&spl_tq);
	dqp = (struct DELAYQ *) dqh.flink;
	while ((void *) dqp != (void *) &dqh) {
	    if (strcmp (name, dqp->name) == 0) {
		dqp->code = 0;
		found++;
	    }
	    dqp = dqp->flink;
	}
	spinunlock (&spl_tq);
	return found;
}
#endif

#if TURBO
/*+
 * ============================================================================
 * = krn$_setquantum - set a process' quantum field                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Set the quantum for a process.  By changing the process quantum, the
 *	process is given proportionally more cpu time at the priority level
 *	that it is executing at.  A process quantum of -1 insures that the 
 *	process is never bumped due to a timer interrupt.
 *
 * FORM OF CALL:
 *  
 *	krn$_setquantum (pid, quantum)
 *  
 * RETURNS:
 *
 *	x - previous process quantum
 *	0 - couldn't find process associated with pid
 *       
 * ARGUMENTS:
 *
 * 	int pid - process id
 *	int quantum - new process quantum, measured in timer ticks
 *
 * SIDE EFFECTS:
 *
 *	Target process will continue to run on current processor until next
 *	reschedule.
 *
-*/
int krn$_setquantum (int pid, int quantum) {
	struct PCB	*pcb;
	int		old_quantum;

	old_quantum = 0;

	if (pcb = krn$_findpcb (pid)) {
	    old_quantum = pcb->pcb$l_quantum;
	    if (quantum == -1) {
		pcb->pcb$l_quantum = 1;
		pcb->pcb$l_qexpire_dec = 0;
	    } else {
		pcb->pcb$l_quantum = quantum;
		pcb->pcb$l_qexpire_dec = 1;
	    }
	    pcb->pcb$l_qexpire = pcb->pcb$l_quantum;
	    spinunlock (&spl_kernel);
	}
	return old_quantum;
}
#endif

/*+
 * ============================================================================
 * = krn$_timer - Global kernel timer task                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Timer task for the kernel.  Must be highest priority task
 *	in the system. This task runs only on the timer cpu (usually the
 *	primary), waking up at 
 *      periodic intervals so that it may walk down a queue of timer elements 
 *      (TQEs), decrementing counts and posting semaphores and then finally
 *      forcing a reschedule. The secondaries do not run this task. Instead,
 *      they simply force a reschedule in the timer isr.
 *
 *	As this task walks down the queue, it removes TQE's that are left over
 *	from dead processes.  These TQEs have the both the permanent and the 
 *	active flag set to 0.
 *
 *	On some Alpha platforms, this process uses the system cycle counter to
 *	keep track of elapsed time.  Since the system cycles counters are not
 *	synchronized in a MP system, allowing the timer process to migrate
 *	could cause time glitches (as in time running backwards!).  If it is
 *	important that the timer process be allowed to migrate, then this 
 *	should be taken into consideration.
 *
 * FORM OF CALL:
 *
 *	krn$_timer (startup)
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
 *      Tasks are scheduled.
 *          
-*/
void krn$_timer (int startup) {
	struct TIMERQ *tqp, *next_tqp;
	struct DELAYQ *dqp;
	struct DELAYQ *next_dqp;
	int	elapsed_msec;
	int	carry;
	int	x;
	int	y;
	char	*io;

	/*
	 * Initialize the timer state that is global to the entire system
	 */
	if (startup) {

	    krn$_seminit (&timer, 0, "timer");
	    poweron_msec [0] = poweron_msec [1] = 0;
	    nsec_residue = 0;
	    tqh.flink = &tqh;
	    tqh.blink = &tqh;
	    dqh.flink = &dqh;
	    dqh.blink = &dqh;
	    pollq.flink = (void *) &pollq.flink;
	    pollq.blink = (void *) &pollq.flink;

	    /* timer queue spinlock */
	    memset (&spl_tq, 0, sizeof (spl_tq));
	    spl_tq.req_ipl = IPL_SYNC;
	    spl_tq.sav_ipl = IPL_RUN;

	    memset (timebase, 0, sizeof (timebase));

	    /* start the timer ticking */
	    krn$_timebase_init ();

	    return;
	}


	/*
	 * Every time TIMER is posted from the timer ISR,
	 * walk down the list of tasks on the timer queue
	 */
	while (1) {
	    krn$_bpost (&poll);		/* trigger the polling loop */
	    krn$_wait (&timer);

	    /*
	     * Figure out how many msec have elapsed since the last
	     * posting.  We can take one of two approaches:
	     *     1) Assume that timer ticks occur at well known intervals,
	     *	      and generate an absolute time based on the number of
	     *        of ticks.
	     *     2) Assume that timer ticks don't occur at regular intervals
	     *        (as is the case when we're running with simulated timer
	     *        interrupts, see timer_check).
	     *
	     * We use the first approach.
	     */
	    nsec_residue += NSEC_RESIDUE;
	    carry = 0;
	    if (nsec_residue >= (1000*1000)) {
		nsec_residue -= 1000*1000;
		carry = 1;
	    }
	    elapsed_msec = MSEC_TICK + carry;

	    *(INT *) poweron_msec += elapsed_msec;

	    spinlock (&spl_tq);

	    /*
	     * Walk down the list of Timer Queue Elements
	     */
	    tqp = (struct TIMERQ *) tqh.flink;
	    while ((void *) tqp != (void *) &tqh) {
		next_tqp = tqp->flink;
		if (tqp->active) {
		    tqp->msec -= elapsed_msec;
		    if (tqp->msec <= 0) {
			tqp->active = 0;
			if (!tqp->perm) {
			    remq (tqp);
			}
			krn$_bpost (&tqp->sem,TIMEOUT$K_SEM_VAL);
		    }

		/*
		 * Remove the tqe if it is inactive and not permanent
		 */
		} else {
		    if (!tqp->perm) {
			remq (tqp);
		    }
		}
		tqp = next_tqp;
	    }

	    /*
	     * Walk down the list of Delay Queue Elements
	     */
	    dqp = (struct DELAYQ *) dqh.flink;
	    while ((void *) dqp != (void *) &dqh) {
		next_dqp = dqp->flink;
		dqp->msec -= elapsed_msec;
		if (dqp->msec <= 0) {
		    /*
		     * Before creating the process, make sure it can run.  It
		     * can run if its affinity mask shows that at least one CPU
		     * which it wants to run on is in console mode.  A slight
		     * twist is that the primary is counted as in console mode
		     * if it's in the middle of a callback.
		     */
		    x = dqp->affinity;
		    if (!x)
			x = (1 << primary_cpu);
		    y = in_console;
		    if (cbip)
			y |= (1 << primary_cpu);
		    if (x & y) {
			remq (dqp);
			spinunlock (&spl_tq);
			if (dqp->code) {
			    if (dqp->use_tt_not_nl)
				io = "tt";
			    else
				io = "nl";
			    krn$_create (
				    dqp->code,
				    dqp->startup,
				    dqp->sem,
				    dqp->pri,
				    dqp->affinity,
				    dqp->stack,
				    dqp->name,
				    io, "r", io, "w", io, "w",
				    dqp->a0,
				    dqp->a1,
				    dqp->a2,
				    dqp->a3);
			}
			dyn$_free (dqp, DYN$K_SYNC|DYN$K_NOOWN);
			spinlock (&spl_tq);
		    }
		}
		dqp = next_dqp;
	    }

	    spinunlock (&spl_tq);
	}
}

/*
 * Polling process for the firmware.
 */
void krn$_poll (int startup)
{
	int pri;
	struct POLLQ *pqp;
	struct PCB *pcb;

	if (startup) {
	    krn$_seminit (&poll, 0, "poll");
	    return;
	}

	pcb = getpcb ();
	pri = pcb->pcb$l_pri;

	while (1) {
	    krn$_wait (&poll);

	    /*
	     * Walk down the polling queue.
	     */
	    pqp = pollq.flink;
	    while ((void *) pqp != (void *) &pollq.flink) {
		(*pqp->routine) (pqp->param);
		pqp = pqp->flink;
	    }

	    pcb->pcb$l_pri = 2;
	    reschedule (1);
	    pcb->pcb$l_pri = pri;
	}

}

/*+
 * ============================================================================
 * = krn$_timebase_init - start timer interrupt for a cpu		      =
 * ============================================================================
 *
 *  
 *	This routine start the hardware timer ticking for the cpu on which this
 *	routine is called.  It must therefore be called by all processors in
 *	the active set, and must be called by any processor entering the set.
 *
 *	The routine assumes that it will not be interrupted and thus moved to
 *	another cpu.
 *
 *	Multiple calls to this routine should have no side effects other than
 *	to possibly generate on extra timer tick on the cpu that the routine
 *	was called on.
 *  
 * FORM OF CALL:
 *  
 *	krn$_timebase_init ()
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

void krn$_timebase_init (void) {
	TIMEBASE *tbp;

	tbp = & timebase [whoami ()];
	tbp->tick_count = 0;
	tbp->no_timer = 0;
	tbp->no_timer_notify = 0;
	tbp->last_rscc = 0;

	/*
	 * Connect to the interrupt
	 */
	int_vector_set (SCB$Q_INTV, timer_interrupt);

#if TURBO
        if (!running_from_bcache)
            set_inttim_ints(1);  	/* Enable interval timer interrupts */

        write_csr_32(WATCH$CSRB, WATCH_M_DM | WATCH_M_TF_TW);
        write_csr_32(WATCH$CSRA, CSRA_DV$K_32KHZ | CSRA_RS$K_0110);

        if ( watch_chip_eco )
            write_csr_32(WATCH$CSRB, WATCH_M_SQWE | WATCH_M_DM | WATCH_M_TF_TW);
        else 
            write_csr_32(WATCH$CSRB, WATCH_M_SQWE | WATCH_M_DM | 
                         WATCH_M_TF_TW | WATCH_M_PIE);
#endif

#if !TURBO && !SHARK
	if (primary ()) {
	    krn$_reset_toy ();
	}
#endif
}


void krn$_reset_toy (void) {
#if SABLE
    UINT two = 2;
    UINT three = 3;
    UINT crr;
#endif
    unsigned char data;
    struct FILE *fp;

#if SABLE || MIKASA || AVANTI || MONET || K2 || MTU || TAKARA || APC_PLATFORM || REGATTA || WILDFIRE || MONET || EIGER || FALCON
    if ((fp = fopen( "toy", "r+")) == NULL)                                               
	return;                                                                           
                                                                                          
#if MIKASA || AVANTI || MONET || K2 || MTU || TAKARA || APC_PLATFORM || REGATTA || WILDFIRE || MONET || EIGER || FALCON
    data = 0x26;
#endif
#if SABLE     
#if GAMMA
    data = 0x25;
#else
    crr = READ_CPU_CSR(primary_cpu, CRR_ADDR);
    if ((crr & 0xf) < 8)
	data = 0x26;
    else
	data = 0x25;
#endif
#endif
    fseek (fp, 0x0a, SEEK_SET);
    fwrite (&data, 1, 1, fp);

    fseek (fp, 0x0b, SEEK_SET);
    fread (&data, 1, 1, fp);
    data &= ~0x80;
#if MIKASA || AVANTI || K2 || MTU || TAKARA || APC_PLATFORM
    data |= 0x46;
#endif
#if (PC264 && !NAUTILUS && !TINOSA && !GLXYTRAIN && !SHARK)
/*
** Enable Square Wave Generation
*/
    data |= 0x08;
#endif
#if APC_PLATFORM
/*
** Fix the SROM bug that set the DSE bit.
*/
    data &= ~0x01;
#endif
#if SABLE || REGATTA || WILDFIRE || MONET || EIGER || FALCON
    data |= 0x0e;
#endif
    fseek (fp, 0x0b, SEEK_SET);
    fwrite (&data, 1, 1, fp);

    fclose (fp);
#endif

#if MEDULLA || CORTEX || YUKONA
    fp = fopen("toy","r+");
    if (fp == NULL)
      {
       return;
      }

    fseek(fp,0x0b,SEEK_SET);
    data = 0x90;		/* Enable TE and Pulse mode for Interrupt */
    fwrite(&data,1,1,fp);

    fseek(fp,0x09,SEEK_SET);
    fread(&data,1,1,fp);
    data &= 0x3F;		/* Enable Osc. and Heartbeat */
#if NO_HBEATS
    data |= 0x40;		/* Disable 1mS Heartbeat */
    disable_interrupt(SCB$Q_INTV); /* disable the int at the controller */
#else
    enable_interrupt(SCB$Q_INTV);   /* enable the int */
#endif
    fseek(fp,0x09,SEEK_SET);
    fwrite(&data,1,1,fp);

#if MEDULLA
    outportb(NULL, 0x2000, 0); /* clear heartbeat reg */
#else
    outportb(NULL, MOD_HBEAT_CLR, 0); /* clear heartbeat reg */
#endif

    fclose(fp);
#endif

#if ALCOR
/*
    Alcor specific code to reset the TOY chip

    Alcor intentionally does not use any of the code 
    above way too many nested #if's !!!

    We should probably move this routine to a platform 
    specific file
*/
    /* get access to the TOY driver */
    if ((fp = fopen( "toy", "r+")) == NULL)
	return;

    /* 
     Write a 0x26 to CSRA in the TOY chip to:

     Set Timebase Divisor = 32.768 Khz
     and
     Rate Select = 976.562 us
    */

    data = 0x26;
    fseek (fp, 0x0a, SEEK_SET);
    fwrite (&data, 1, 1, fp);

    /*
     Read CSRB in the TOY chip
    */

    fseek (fp, 0x0b, SEEK_SET);
    fread (&data, 1, 1, fp);

    /* clear the set bit on the TOYchip */

    data &= ~0x80;

    /* 
      Set the periodic interrupt enable,
              24 hour mode,
	      and
	      dm bits
    */
    data |= 0x46;

    /* write it back out to the TOY chip */

    fseek (fp, 0x0b, SEEK_SET);
    fwrite (&data, 1, 1, fp);

    fclose (fp);                   

#endif

}


/*+
 * ============================================================================
 * = krn$_readtime - read the system time into a quadword	              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Store the system time into a quadword.  System time is usually fine
 *	grained, on the order of usec.  Successive calls to this routine are
 *	guaranteed to be monotonically increasing across the entire system even
 *	if the process migrates to other cpus.
 *
 *
 * FORM OF CALL:
 *  
 *	krn$_readtime (result)
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 *	int *result - address of quadword into which time is read
 *
 * SIDE EFFECTS:
 *
 *
-*/
void krn$_readtime (INT *result) {

	rscc (result);		/* read system cycle counter pal call */
}

/*+
 * ============================================================================
 * = krn$_micro_delay - Delay for minimimum of specified number of microsecs  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Delay for a minimum of a specified number of microseconds.  This
 * 	routine may delay longer depending on how process scheduling affects
 *	the process calling this routine.  The caller might want to raise its
 *	process priority level (but not above 5) and temporarily set its
 *	quantum to forever before making this call if waiting longer is a 
 *	problem.
 *
 *	This routine loops for a number of times which is a function of
 *	a global variable whose value is calibrated at system startup time.
 *
 *	On a VAX this routine will always delay for 1 millesecond.
 *
 * FORM OF CALL:
 *
 *	krn$_micro_delay ( us )
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      INT us - number of microseconds to delay. Quadwd on ALPHA; lwd on VAX.
-*/
void krn$_micro_delay (INT us) {

    int cpm;
    INT limit, start, stop;

#if TURBO || RAWHIDE || PINNACLE || PC264 || REGATTA || WILDFIRE || MONET || MARVEL || FALCON
    if (platform() == ISP_MODEL)
	return;
#endif

    cpm = timebase [whoami ()].cycles_per_microsecond;
    if (!cpm)
	cpm = 1000 * 1000 / DEFAULT_PSEC_PER_CYCLE;
    rscc (&start);
    limit = (cpm * us) + start;
    do {
	rscc (&stop);

	/* check for time running backwards */
	if (stop < start) break;
    } while (stop < limit);

    return;
}


/*+
 * ============================================================================
 * = krn$_sleep - Suspend process for specified number of milliseconds        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Sleep for a specified number of milliseconds.  The actual time slept
 *	will be at least the amount specified.  The time specified is rounded
 *	up to the nearest multiple of the timer resolution.
 *
 * FORM OF CALL:
 *
 *	krn$_sleep ( milliseconds )
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      int milliseconds - number of milliseconds to sleep.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void krn$_sleep (int milliseconds) {
	char		name[17];
	struct TIMERQ 	tqe;

#if PINNACLE || PC264 || REGATTA || WILDFIRE || MONET || MARVEL || FALCON
	if (platform() == ISP_MODEL)
	    milliseconds = 1;
#endif

	/*
	 * If we are at raised IPL, use krn$_micro_delay instead
	 */
	if ((mfpr_ipl() > IPL_RUN) && 
	    (console_mode[whoami()] == INTERRUPT_MODE)) {
	    krn$_micro_delay (milliseconds * 1000);
	} else {
	    sprintf (name, "tqe %x", callers_pc ());
	    krn$_seminit (&tqe.sem, 0, name);
	    tqe.msec = milliseconds;
	    tqe.active = 1;
	    tqe.perm = 0;

	    /*
	     * Put the TQE on the timer queue
	     */
	    spinlock (&spl_tq);
	    insq (&tqe, &tqh);
	    spinunlock (&spl_tq);

	    /*
	     * Wait for the timer task to post the semaphore	
	     */
	    krn$_wait (&tqe.sem);
	    krn$_semrelease (&tqe.sem);
	}
}

/*+
 * ============================================================================
 * = krn$_init_timer - Create a timer                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Create a permanent timer queue entry and initialize it to "inactive".
 *  
 * FORM OF CALL:
 *  
 *	krn$_init_timer( tqe )
 *  
 * RETURNS:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 *      struct TIMERQ *tqe - address of a timer queue entry
 *
 * SIDE EFFECTS:
 *
 *	An entry is inserted in the global timer queue, tqh.
 *
-*/
void krn$_init_timer (struct TIMERQ *tqe) {

	tqe->active = 0;
	tqe->perm = 1;

	spinlock (&spl_tq);
	insq (tqe, &tqh);
	spinunlock (&spl_tq);
}

/*+
 * ============================================================================
 * = krn$_release_timer - Delete a timer                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Delete a timer queue entry. Simply remq the timer.
 *  
 * FORM OF CALL:
 *  
 *	krn$_release_timer( tqe )
 *  
 * RETURNS:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 *      struct TIMERQ *tqe - address of a timer queue entry
 *
 * SIDE EFFECTS:
 *
 *	An entry is removed from the global timer queue, tqh.
 *
-*/

void krn$_release_timer (struct TIMERQ *tqe) {

	spinlock (&spl_tq);
	remq (tqe);
	spinunlock (&spl_tq);
}

/*+
 * ============================================================================
 * = krn$_start_timer - Start a timer                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Load a timers timeout value and then set it to the "active" state.
 *  
 * FORM OF CALL:
 *  
 *	krn$_start_timer( tqe, milliseconds )
 *  
 * RETURNS:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 *      struct TIMERQ *tqe - address of a timer queue entry
 *	int milliseconds - the timeout value in milliseconds
 *
 * SIDE EFFECTS:
 *
 *	Timer queue entry is activated.
 *
-*/
void krn$_start_timer (struct TIMERQ *tqe, int milliseconds) {
	tqe->msec = milliseconds;
	mb();
	tqe->active = 1;
}

/*+
 * ============================================================================
 * = krn$_stop_timer - Stop a timer                                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Set a timer queue entry to the "inactive" state.
 *  
 * FORM OF CALL:
 *  
 *	krn$_stop_timer( tqe )
 *  
 * RETURNS:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 *      struct TIMERQ *tqe - address of a timer queue entry
 *
 * SIDE EFFECTS:
 *
 *	Timer queue entry is deactivated.
 *
-*/
void krn$_stop_timer (struct TIMERQ *tqe) {
	tqe->active = 0;
}


/*+
 * ============================================================================
 * = timer_interrupt - Timer interrupt service routine                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Timer interrupt service routine performs the following:
 *##
 *	  1 clears the timer interrupt,
 *  	  2 adds a MSEC_TICK to the current process time, and
 *	  3 if running on the primary, posts a semaphore that will 
 *           trigger the timer task, if running on a secondary, simply
 *           reschedule.
 *#
 *  	This routine must run within the kernel spinlock.
 *
 * FORM OF CALL:
 *  
 *	timer_interrupt ()
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	If primary, then the timer process is awakened.   
 *
-*/

void timer_interrupt () {
	struct PCB *pcb;
	struct SEMAPHORE *s;
	TIMEBASE *tbp;
	int id, cpu;

#if NAUTILUS || GLXYTRAIN || TINOSA
	rtc_read(0x0c);
#endif
#if !NO_HBEATS && (MEDULLA || CORTEX || YUKONA)
#if MEDULLA
    outportb(NULL, 0x2000, 0); /* clear heartbeat reg */
#else
    outportb(NULL, MOD_HBEAT_CLR, 0); /* clear heartbeat reg */
#endif
#endif

#if MEDULLA && 0
/*
 * Display alternating '1' and '2' when H/W Heartbeat interrupts are
 * not detected; display alternating 'x' and 'y' when they are detected
 *
 */
 id = whoami ();
 tbp = & timebase [id];

 if (++hbeat_cnt % 1000 == 0)
   {
    if (hbeat_flag)
      {
       hbeat_flag = 0;
       if (tbp->no_timer)
      	 putLED('1');
       else
      	 putLED('x');
       }
    else
       {
    	hbeat_flag = 1;
	if (tbp->no_timer)
          putLED('2');
	else
          putLED('y');
        }
   }
#endif

	/*
	 * Get the PCB
	 */
	pcb = getpcb ();

	spinlock (&spl_kernel);


	/*
	 * Update this process's cpu utilization (kept in units of timer ticks)
	 */
	*(INT *) pcb->pcb$q_cputime += 1;

	/*
	 * Adjust the quantum.
	 */
	pcb->pcb$l_qexpire -= pcb->pcb$l_qexpire_dec;

	/*
	 * Post the timer task if this is the timer cpu
	 */
	id = whoami ();
	tbp = & timebase [id];
	tbp->tick_count++;
#if MEDULLA || CORTEX || YUKONA 
	/* required for heartbeat diagnostic */
	if (hbeat_diag_in_progress && (tbp->no_timer==0)) 
		int_cnt++;
#endif
#if WILDFIRE
	log_timer_tick (id, MSEC_TICK);
#endif
	if (cbip) {
	    cpu = primary_cpu;
	} else if (!in_console) {
	    cpu = id;
	} else {
	    cpu = timer_cpu;
	}
	if (cpu == id) {

	    s = &timer;
	    s->count++;
	    if (s->count <= 0) {
		pcb = remq (s->flink) - offsetof (struct PCB, pcb$a_flink);
		pcb->pcb$l_pstate = KRN$K_READY;
		pcb->pcb$a_semp = 0;
		insq (&pcb->pcb$a_flink, readyq [pcb->pcb$l_pri].blink);
	    }
	}

	spinunlock (&spl_kernel);

}

/*+
 * ============================================================================
 * = timer_check - Check the validity of the timer in the idle loop           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is called within the idle loops of each processor to verify
 *	that timer interrupts are happening at (more or less) regular intervals.
 *
 *	If they aren't, then we need to simulate timer interrupts by calling
 *	the timer ISR at approximately the right frequency.  By doing so, we
 *	can run without at any interrupts at all, and also recover from broken
 *	timer chips (this has occured on early prototypes). The timer interuupt
 * 	will post the timer process iff it's running on the timer cpu.
 *
 *	The polling loop, which krn$_poll implements by walking the poll loop,
 *	is a distinct operation from the timing loop.  The polling loop is
 *	guarantteed to run at least once a millisecond, and considerably faster
 *	if the cpu is idle.
 *
 *	In checking for broken timers, we have to delay, and while delaying
 *	we still have to keep the polling loop alive.
 *
 *	On VAX, we always assume a valid timer interrupt, since we don't have
 *	an architecture independent time source (like the cycle counter on
 *	ALPHA)
 *
 * FORM OF CALL:
 *  
 *	timer_check ()
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *
-*/
void timer_check () {
	int	tick;
	int	last_tick;
	int	id, cpu;
	int	ticks;
	TIMEBASE	*tbp;
	INT	start, current;

	id = whoami ();
	tbp = & timebase [id];

	/*
	 * Print an error message, once, notifying the user about the lack
	 * of timer interrupts.  Only print it when we're powering up.
	 */
	if (tbp->no_timer && !tbp->no_timer_notify && (cpip & (1 << id))) {
	    tbp->no_timer_notify = 1;
#if AVANTI || MIKASA || MEDULLA || ALCOR || K2 || CORTEX || YUKONA || TAKARA || EIGER
	    /* no printout for us thanks */
#else
	    qprintf ("*** no timer interrupts on CPU %d ***\n", id);
#endif
	}

	/*
	 * Snapshot the number of timer interrupts this processor has seen
	 * and the current cycle count.
	 */
	last_tick = tbp->last_tick;
	if (last_tick == 0)
	    last_tick = tbp->tick_count;
	start = tbp->last_rscc;
	if (start == 0)
	    rscc (&start);

	/*
	 * If the timer on this cpu isn't running, delay one timer tick.
	 * If it is running, then delay ten ticks (to insure that at least one
	 * tick will occur between snapshots).
	 */
	ticks = 1;
	if (tbp->no_timer == 0) {
	    ticks = 10;
	}

	/* Delay the required number of ticks while keeping the poll loop
	 * alive.  Watch out for time going backwards (!).
	 */
	current = 0;
	do {
	    if (cbip) {
		cpu = primary_cpu;
	    } else {
		cpu = timer_cpu;
	    }
	    if (cpu == id) {
		krn$_bpost (&poll);
		krn$_micro_delay (10);
	    }
	    rscc (&current);
	    /* Time running backwards (boot/run_a_long_time/halt). PAL
	     * doesn't save/restore the system cycle counter, and furthermore
	     * it clears it on a boot, so we can in effect see time running
	     * backwards.
	     */
	    if (current < start) break;

	} while ((current - start) < (CYCLES_PER_TICK * ticks));
	tick = tbp->tick_count;

	tbp->last_rscc = current;
	tbp->last_tick = tick;

	/*
	 * Snapshot again and see if any timer interrupts came in.
	 */
	if (tick == last_tick) {
	    tbp->no_timer = 1;
	} else {
	    tbp->no_timer = 0;
	}

#if TURBO && EV5
	/* 
	 * If there is no timer, try and wake it up.  We could be on a module that
	 * still requires the WATCH chip to provide the interrupt.
	 */
	if( (tbp->no_timer) && (console_mode[id] == INTERRUPT_MODE) ) {
           if ( watch_chip_eco )
               qprintf("No timer interrupt, need to have the WATCH chip deliver\n");
           cserve( CSERVE$MTPR_INTIM_FLAG, 0 ); 
           write_csr_32(WATCH$CSRB, WATCH_M_DM | WATCH_M_TF_TW);
           write_csr_32(WATCH$CSRA, CSRA_DV$K_32KHZ | CSRA_RS$K_0110);
           write_csr_32(WATCH$CSRB, WATCH_M_SQWE | WATCH_M_DM | 
                                                   WATCH_M_TF_TW | WATCH_M_PIE);
           watch_chip_eco = 0;
        }
#endif

        /* simulate the timer if not present or if console is in polled mode */
	if ((tbp->no_timer) || (console_mode[id] == POLLED_MODE)) {
	    tbp->last_tick++;
	    timer_interrupt ();
	    reschedule (1);
	}

        return;
}


void krn$_timer_set_cycle_time () {
	int id, i, j, t[4], tc0, tc1, n1, n2;
	INT scc0, scc1, delta, freq1, freq2;
#if SABLE || ALCOR || RAWHIDE || (PC264 && !WEBBRICK)
	char ocp_string[16];
	struct FILE *fp = 0;
#endif

#if MEDULLA || CORTEX || YUKONA
	int module_id;
	int speed_in_mhz;

	medulla_cycles_per_tick = 
	(((MSEC_TICK*1000*1000+NSEC_RESIDUE)*1000) / DEFAULT_PSEC_PER_CYCLE);
#endif

	id = whoami ();

#if TURBO || RAWHIDE || PINNACLE || PC264 || REGATTA || WILDFIRE || MONET || MARVEL || FALCON
	if ( platform() == ISP_MODEL ) {
	    timebase [id].cycles_per_second = HWRPB$_CC_FREQ;
	    cpu_mhz [id] = 
	    timebase [id].cycles_per_microsecond = HWRPB$_CC_FREQ / 1000000;
	    return;
	}
#endif

#ifdef CSERVE$CALIBRATE

	/* CSERVE$CALIBRATE returns the number of cycles in one tick */
	freq1 = cserve (CSERVE$CALIBRATE);
	freq2 = freq1 * 1000000000 / (MSEC_TICK * 1000000 + NSEC_RESIDUE);
	timebase [id].psec_cycle = 1000000000000 / freq2;
	timebase [id].cycles_per_second = freq2;
	cpu_mhz [id] = 
	timebase [id].cycles_per_microsecond = (freq2 + 500000) / 1000000;

#else

	/*
	 *  We read the cycle counter for 1 timer tick or we timeout
	 *  after a best guess at 1 second.
	 */
	rscc (&scc0);
	scc1 = scc0;
	i = timebase [id].tick_count + 1;
	while ((timebase [id].tick_count < i) && 
		((scc1 - scc0) < (CYCLES_PER_TICK * 1000))) {
	    rscc (&scc1);
	}

#if WILDFIRE
#define MULTIPLIER 1000
#else
#define MULTIPLIER 100
#endif

	/*
	 *  If we got 0 timer ticks then set the defaults,
	 *  else calculate the frequency.
	 */
	if ((scc1 - scc0) >= (CYCLES_PER_TICK * 1000)) {
	    freq1 = DEFAULT_PSEC_PER_CYCLE / 10;
	} else {
	    for (j = 0; j < 100; j++) {
		/*
		 *  We're going to keep doing this until four times in a row
		 *  we get the same answer.
		 */
		i = timebase [id].tick_count + 1;
		while (timebase [id].tick_count < i) {
		}
		rscc (&scc0);
		tc0 = timebase [id].tick_count;
		i = timebase [id].tick_count + 100;
		while (timebase [id].tick_count < i) {
		}
		rscc (&scc1);
		tc1 = timebase [id].tick_count;
		/*
		 *  We now have an elapsed cycle count and an elapsed tick
		 *  count.  We divide the first into the second to get
		 *  nanoseconds per cycle.  But, since we don't do floating
		 *  point, we want to calculate this number times 100.  This
		 *  way we can express cycle times of 5ns (500), 6.25ns (625),
		 *  6.6ns (660), or 6.99ns (699).
		 */
		delta = scc1 - scc0;
		freq1 = (tc1 - tc0) * (MSEC_TICK * 1000000 + NSEC_RESIDUE);
		freq1 = (freq1 * MULTIPLIER * 10 / delta + 5) / 10;
		memcpy (t+1, t+0, 12);
		t[0] = freq1;
		if ((t[0] == t[1]) && (t[1] == t[2]) && (t[2] == t[3])) {
		    break;
		}
	    }
	}

	/*
	 *  We now calculate the number of cycles/second and display
	 *  the CPU speed in nanoseconds and MHz.
	 */
	freq2 = 1000000000;
	freq2 = freq2 * MULTIPLIER / freq1;
	n1 = freq1 / MULTIPLIER;
	n2 = freq1 % MULTIPLIER;
	timebase [id].psec_cycle = n1 * 1000 + n2 * 1000 / MULTIPLIER;
	timebase [id].cycles_per_second = freq2;
	cpu_mhz [id] = 
	timebase [id].cycles_per_microsecond = (freq2 + 500000) / 1000000;

#endif

#if AVANTI || MIKASA || MONET || ALCOR || K2 || TAKARA || APC_PLATFORM || EIGER
	/* No printouts for us ... */
#else
	PowerUpProgress (0xf4, "CPU %d speed is %d MHz\n", id, timebase [id].cycles_per_microsecond);
#endif

#if SABLE
	sprintf(ocp_string, "Starting CPU  %d ", id);
	sable_ocp_write(fp, strlen(ocp_string), 1, ocp_string);
#endif
#if RAWHIDE || (PC264 && !WEBBRICK)
    if (!(primary())) {
	sprintf(ocp_string, "Starting CPU  %d ", id);
	sable_ocp_write(fp, strlen(ocp_string), 1, ocp_string);
    }
#endif
#if ALCOR
    if (alcor_mod_rev == MODREV_ALCOR)
    {
	sprintf(ocp_string, "Starting CPU  %d ", id);
	mikasa_ocp_write(fp, strlen(ocp_string), 1, ocp_string);
    }
#endif
#if PRIVATEER
    if ( !( primary( ) ) ) {
	sprintf( ocp_string, "Starting CPU %1d  ", id );
	rmc_cmd( 3, strlen( ocp_string ), ocp_string, 0 );
    }
#endif

}

int krn$_psec_cycle (int id) {
	return timebase [id].psec_cycle;
}

int krn$_timer_get_cycle_time (int id) {
	return timebase [id].cycles_per_second;
}

/*+
 * ============================================================================
 * = Start_Timeout -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *    Start a timeout
 *
 * FORM OF CALL:
 *  
 *	start_timeout(delay,tqe)
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	time_delay  - in milliseconds
 *	tq	    - timer queue entry pointer
 *
 * SIDE EFFECTS:
 *
 *
-*/
void start_timeout(int time_delay, struct TIMERQ *tq)
{
    char name[20];

    sprintf(name, "tqs_%08x", tq);
    krn$_seminit(&tq->sem, 0, name);
    tq->sem.count = 0;
    krn$_init_timer(tq);
    if (time_delay != 0)
    	krn$_start_timer(tq, time_delay);
}

/*+
 * ============================================================================
 * = Stop_Timeout -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *    Stop a timeout
 *
 * FORM OF CALL:
 *  
 *	stop_timeout(tq)
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	tq   - timer queue entry pointer
 *
 * SIDE EFFECTS:
 *
 *
-*/
void stop_timeout(struct TIMERQ *tq)
{
    krn$_stop_timer(tq);
    krn$_semrelease(&(tq->sem));
    krn$_release_timer(tq);
}
