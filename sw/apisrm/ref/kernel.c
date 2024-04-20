/* file:	kernel.c
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
 *	Kernel synchronization utilities.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      22-Mar-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	gdm	04-Feb-1999     Conditionalise for DMCC Eiger
 *
 *	mar	24-Dec-1998	Remove MIKASA from year-mangling code
 *
 *	mar	19-Nov-1997	Unwrap reset_cpu()              
 *
 *	mab	01-Jul-1997	Addition of dsr_check for OCP display
 *
 *	sm	10-Jul-1996	Conditionalise for Takara
 *
 *	er	23-May-1996	Added code correct date if coming from nt on EBxxx platforms
 *
 *      rhb     12-apr-1996     AVANTI: Changed APU heap offset
 *
 *	er	09-Apr-1996	EBxx: fixing banner printf in show_version()
 *
 *	jje	05-Apr-1996	CORTEX: Use speed in Mhz as model number in 
 * 				show_version()
 *
 *	er	02-Apr-1996	EBxx: Changed DSRDB access in show_version
 *
 *	twp	30-Mar-1996	Conditionalize for RHMIN
 *
 *	jcc	25-Jan-1995	Conditionalize for Cortex
 *
 *	noh	15-Jan-1996	BURNS: Changed version banner to ALPHAbook 1
 *
 *	noh	28-Dec-1995	BURNS: Set Power Management Register to Normal
 *				Console State (divide by 2).
 *
 *	noh	13-Dec-1995	BURNS: Changed version banner to ALPHAbook
 *
 *	noh	06-Dec-1995	BURNS: Changed version banner to AlphaNotebook
 *
 *	noh	22-Nov-1995	BURNS: now using PowerUpProgress.
 *
 *	noh	07-Nov-1995	Changed memory_low_limit is setup for BURNS.
 *
 *	noh	24-Aug-1995	AVMIN: Removed AVMIN conditionals.
 *
 *	cto	18-aug-1995	add SBE spinlock for TURBO
 *
 *	swt	29-Jun-1995	add Noritake platform.
 *
 *	noh	22-Jun-1995	AVMIN: Changed way boot_system and system_reset_or_error
 *				are called. This is needed to fix problem with small
 *				shell.
 *
 *	dtr	20-Jun-1995	added code correct date if coming from nt.
 *
 *	dpm	7-Jun-1995	add a global halt varible to save the haltcode
 *				when entering the console. Use in MIKASA
 *
 *	dtr	24-may-1995	made checks for ipl level symbolic rather than
 *				hard coded to zero and non-zero.  This allows
 *				for polling.
 *				
 *	adm	28-Mar-1995	add 'lineset' to initalization of null_ttpb
 *
 *	rbb	13-Mar-1995	Conditionalize for EB164
 *
 *     mr	16-Feb-1995	Add spinlock "spl_atomic" for data integrity 
 *				in toy_driver.c and esc_nvram_driver.c 
 *
 *	er	2-Feb-1995	Fix screen clear problem on halt for EB64+/EB66.
 *
 *	dtr	1-feb-1995	removed avanti from list of files that have
 *				the memory_low_limit wired to 1meg.
 *				
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	dtr	16-nov-1994	Added in code to force graphics console if
 *				nt and serial line console
 *
 *	dwb	04_Nov-1994	Issue an Eisa_EOI if ^p detected on a serial
 *				line call-back. This prevents a pending 
 *				uncompleted serial line interrupt from haning 
 *				VMS on reboot.
 *
 *	er	07-Oct-1994	Get EB64+ system name from dsrdb
 *
 *	mar 	26-Aug-1994	Sable - Console name change
 *
 *	mdr	25-Aug-1994	Remove unnecessary output during boot for
 *				Medulla
 *
 *	er	15-Aug-1994	Conditionalize for SD164
 *
 *	dwb	08-Aug-1994	Fix a TGA screen clear problem on ^p
 *
 *	er	23-Jun-1994	Conditionalize for EB64+
 *
 *	dtr	16-dec-1993	added lean support
 *
 *	dpm	2-Dec-1993	Use mustang.h instead of cobra.h for mustang
 *
 *	jmp	29-Nov-1993	Add avanti support
 *
 *	dwr	20-Oct-1993	ECO 6.15: For Turbo MP, the polling process
 *				needs to change w/ the timer cpu in console_exit()
 *
 *	dwr	29-Sep-1993	For TURBO, gate the initial xdelta break with
 *				an NVR location
 *
 *	kl	27-Sep-1993	Add support for transitioning the console 
 *				between polled mode and interrupt mode.
 *				Add krn$_set_console_mode and set_all_pcb_ipls.
 *				In the schedule routine, make the IPL checks 
 *				only if in interrupt mode. In console_exit
 *				transition the console to polled mode. In the
 *				idle process, call timer_interrupt if the
 *				the console is in polled mode. All of this
 *				code is currently #if TURBO'ed.
 *
 *	dwr	27-Sep-1993	Don't calculate cycle time for TURBO ISP.
 *
 *	bobf	15-Sep-1993	Now include medulla.h for Medulla
 *
 *	jrk	 8-Sep-1993	Overlay integration
 *
 *	kl	10-Aug-1993	OSF boot
 *
 *	hcb	22-Jul-1993	Add some sable support
 *
 *	mdr	14-Jul-1993	Conditionalize for Medulla
 *
 *      cbf     09-Jul-1993     add sable to #if condition to enable xdelta 
 *
 *	joes	07-Jul-1993	fixed misspelling of MEDULLA
 *
 *	hcb	01-Jul-1993	Use cobra type defzone init for Sable
 *
 *	jad	07-Apr-1993	Take PIC init out of here and put it in pal.
 *
 *	pel	17-Mar-1993	Morgan, Remove builder name from banner.
 *
 *	hcb	17-Mar-1993	initialize pcb field rpt_summ to 1 so error
 *				messages are reported before starting ev_driver
 *
 *	pel	09-Mar-1993	if morgan, call do_bpt early in krn$_idle.
 *
 *	pel	06-Mar-1993	add msgs after IPL lowered & around cycle time
 *
 *	pel	25-Feb-1993	Remove do_bpt call for morgan till bpt fixed.
 *
 *	pel	11-Feb-1993	Define & init exdep_inited flag in krn$_idle.
 *
 *	pel	04-Feb-1993	Remove print msg for morgan if no timer ints.
 *
 *	pel	26-Jan-1993	add breakpoint for morgan & print msg for
 *				morgan if timer int received.
 *
 *      cbf     22-Jan-1993     add secure features
 *
 *	pel	15-Jan-1993	Conditionalize for morgan
 *
 *	sfs	19-Oct-1992	Combine KRN$_REMOVE_NODE and _REPLACE_NODE,
 *				and put the code into CONSOLE_EXIT.  Now we
 *				transfer control to the O/S as if the O/S
 *				were a subroutine; we restart the console in
 *				the middle of CONSOLE_EXIT (right after the
 *				call to EXIT_CONSOLE).  This allows us to get
 *				rid of the code which performed PCB rundown.
 *
 *	pel	13-Oct-1992	show_version; use qprintf instead of printf
 *				
 *	pel	12-Oct-1992	fix unicheck routine calculation of slot addr
 *				
 *	jeg	01-Oct-1992	add delay loop in poll routine in krn$_idle.
 *
 *	hcb	14-Sep-1992	Initialize the iic_inited global variable
 *
 *	pel	08-Sep-1992	reset fmbpr when resetting fbus
 *
 *	hcb	21-Aug-1992	Add init of spl_exdep
 *
 *	ajb	14-Jul-1992	Put in jensen conditionals
 *
 *	pel	26-Jun-1992	move enable_ncr_chips_ack call from krn$_idle
 *				to n710_init
 *				
 *	pel	24-Jun-1992	krn$_idle; set screen_mode immed after lbus
 *				reset/enable in order to display more qprintf
 *				msgs in case of console hang.  Do this by
 *				reading TOY:10 here instead of waiting till
 *				powerup process.
 *				                     
 *	pel	08-Jun-1992	chg qprintf's at reset_io_buses time
 *				
 *	jad	13-May-1992	ddb_startup(1) must be on pele also.
 *
 *	pel	08-May-1992	remove creation of dg_pidlist ram disk file
 *				that was used to filter show_iobq for diags.
 *				
 *	pel	30-Apr-1992	add ddb_startup(1) for FBE in krn$_idle
 *				
 *	pel	24-Apr-1992	Reset IO buses before lowering IPL.
 *				
 *	ajb	24-Mar-1992	Add an array, indexed by whoami, that
 *				gives the pid for the idle process on 
 *				that cpu.
 *
 *	hcb   	20-Feb-1992	Make min heap size 100KB Round Code plus min
 *				heap up to the next MB boundry.
 *
 *	ajb   	13-Feb-1992	Add Console Powerup In Progress flag.
 *
 *	jad	10-Feb-1992	Take out int_vector_set for machine checks
 *				so they can be handled as exceptions.
 *
 *	hcb	27-Jan-1992	add dyn$_extend to extend the heap after
 *				the memory is tested and configured
 *
 *	kp	09-Jan-1992	Remove CB_PROTO usage, use MINIMAL.
 *
 *	pel	17-Dec-1991	Don't reset buses.  It's done from powerup now.
 *				Use reset TOY clock routine
 *
 *	ajb	02-Dec-1991	Make heap size a static variable
 *
 *	pel	02-Dec-1991	Hold bus resets for 200ms; enable fbus.
 *
 *	ajb	02-Dec-1991	Add kernel consistency check.
 *
 *	sfs	14-Nov-1991	Fix definition of HLT$C_ values.  Add some
 *				temporary code to aid Cobra debug (Lbus reset
 *				and TOY initialization should go someplace
 *				else).  Remove unused ID argument from
 *				KRN$_REMOVE_NODE.  Change 8530TT_INIT in
 *				KRN$_REPLACE_NODE to _REINIT.  Fix incorrect
 *				handling of nested exceptions in SCHEDULE.
 *
 *	pel	30-Oct-1991	Do lbus_init & tt_init from krn$_idle. Use
 *				cycle counter to time lbus reset. Have powerup
 *				process use tt driver for stdout & stderr.
 *            
 *	pel	25-Oct-1991	do tt_init from krn$_idle unless cobra, 
 *				cb_proto or pele.
 *				Otherwise do from powerup process.Seems to cure
 *				tt_tx_interrupt crash on 1st char typed.
 *	pel	24-Oct-1991	krn$_idle; put some of filesys_init1 here;
 *				create timer task sooner.          
 *
 *	jad	24-Oct-1991	Put spinlocks for cb_proto
 *
 *	ajb	22-Oct-1991	Put process timekeeping in the timer task
 *                                                   
 *	hcb	 11-Oct-1991	Move creation of memzone to memconfig_xx
 *
 *	jad	 01-Oct-1991	Add some debug stuff. 
 *
 *	hcb	 28-Aug-1991	Add memzone size of 64 MB for ADU 
 *
 *	jds	27-Aug-1991	Added eat_interrupts for first lowering IPL.
 *
 *	kl	 23-Aug-1991	Have external and node reset entries post the
 *			 	entry process. Remove build_alpha_context.
 *
 *	pel	 8-Aug-1991	add bad_page_offset & init it to zero.
 *	
 *	jad	 08-Aug-1991	Add support for cb_proto.
 *
 *	dtm	 6-Aug-1991	Move MEMZONE init to after filesys_init1 and
 *				add MEMZONE init.
 *
 *	dtm   	 2-Aug-1991	Call 8530tt_init in krn$_replace_node
 *
 *	kl	 2-Aug-1991	Allow secondary idle processes to block.
 *	
 *	ajb	31-Jul-1991	Add a bug check for waiting at high ipl.
 *                                                   
 *	dtm	19-Jul-1991	Add interval timer interrupts 
 *
 *	kl	27-Jun-1991	Update entry/exit for console SRM ECO 3.1
 *
 *	phk	14-Jun-1991	Performance improvments for 
 *				vax target:
 *                              o add the static subroutines 
 *				  _getpcb, _mfpr, _mtpr, _insq, _remq
 *				o add the #pragma nobuiltins directive
 *                                for the krn$_* subroutines nested
 *				  in kernel.c to allow kernel.c to
 *				  be compiled with the inline option
 *                                enabled to inline the subroutines
 *				  getpcb, mfpr, mtpr, insq & remq 
 *
 *	ajb	13-Jun-1991	Give FP a value on process startup
 *
 *	phk	13-Jun-1991	Undo the previous change :
 *				Restore pcb = getpcb (), mfpr & mtpr
 *
 *	phk	11-Jun-1991	Replace pcb = getpcb () by getpcb (pcb) 
 *                                      mfpr by mfprc
 *                                      mtpr by mtprc
 *                                                   
 *	kl    	04-Jun-1991	Multiprocessor entry/exit
 *
 *	ajb	22-May-1991	Initialize pointer to shell context
 *
 *	jds	 7-Jun-1991	Added xdelta()
 *
 *	jds	04-Jun-1991	Tweaked VAX callback entry dispatch.
 *
 *	ajb	17-May-1991	Remember parent's pid when creating tasks
 *
 *	twp	14-may-1991	Added two external char references to  
 *				new EV's (d_startup and d_complete)
 *
 *	ajb	13-may-1991	Bump up timer interval to 10 msec
 *
 *	ajb	09-May-1991	Add control C queue to PCB.
 *
 *	kl	29-Apr_1991	Post sleepers with a value of zero. On
 *				halt, load HWRPB slot halt fields.
 *
 *	hcb	26-Apr-1991	Allocate a main memory heap in krn$_idle
 *
 *	ajb	26-Apr-1991	Undo previous edit.  Inappropriate speed/time/
 *				flexibility tradeoff (flexibility is important).
 *
 *	ajb	24-Apr-1991	Statically allocate names for stdin, stdout
 *				and stderr in the PCB thus avoiding several
 *				mallocs and strlens.
 *
 *	kl	23-Apr-1991	Entry semaphore/process per cpu.
 *            
 *	kl	16-Apr-1991	Change callback process to entry process
 *				to generalize for callback,system reset and
 *				error entries. Modify replace_node so that it 
 *				does not lower IPL but rather tucks a zero into
 *				spl_kernel.sav_ipl.  
 *                                                                 
 *	jad	04-Apr-1991	Change the calling sequence to tt_setmode.
 *
 *	ajb	01-Apr-1991	Add fatal_procedure
 *
 *	cfc	31-Mar-1991	In krn$_idle, for shell process, assign
 *				its name of "sh" to shell param $0;
 *
 *	pel	29-Mar-1991	init system wide iobq and pcb iobq.
 *
 *
 *	dtm	28-Mar-1991	add halt code messages
 *
 *      phk     21-Mar-1991	krn$_wait clears the semaphore value after 
 *                              the current value has been saved to be returned 
 *                              to the waiter.            
 *
 *      phk     15-Mar-1991	add the value input parameter to krn$_bpost
 *
 *      phk     15-Mar-1991	add the krn$_semreinit routine
 *
 *	pel	15-Mar-1991	zero out pcb addr of stderr file descr after
 *				it's closed.
 *                                                   
 *	dtm	12-Mar-1991	fix memory leak in remove/replace node sequence.
 *				print out halt code on halt entry.  re-init
 *				tt driver and shutdown callback tt driver on
 *				halt entry.  save prbr across exit/reentry.
 *
 *	dtm	04-Mar-1991	changes to replace_node for Alpha reentry.
 *                                                                 
 *	kl	01-Mar-1991     Change HWRPB communications driver to a 
 *				TT port driver.
 *
 *	pel	28-Feb-1991	krn$_create; clone parent's diag EV mask
 *				instead of reading the environment variables.
 *
 *	ajb	19-Feb-1991	Turn off stack flooding on process creation
 *
 *	sfs	19-Feb-1991	Remove concept of "console name"; this will
 *				be handled internal to the terminal class
 *				driver.
 *
 *	ajb	14-Feb-1991	Return a value after waiting on a semaphore.
 *				This is a convenient way for a poster to pass
 *				some information to a waiter.
 *
 *	kl	12-Feb-1991	Lower IPL in replace node so scheduler can
 *				be called. Add global primary_cpu.
 *
 *	mrc	07-Feb-1991	added check_kill() function to do longjmp to
 *				krn$_process if killpending is set.  Replaced
 *				killpending() function to simply return the
 *				state of the killpending bit.
 *
 *	kl	06-Feb-1991	Allow SMP IO by taking and releasing the
 *				transmit lines for each character.
 *
 *	mrc	05-Feb-1991	Added a setjmp to krn$_process and longjmp to
 *				killpending to enable killpending to directly
 *				cause termination of a process.  Updated
 *				krn$_create to initialize the pcb$r_evs word
 *				in the PCB.  Put ev_init() before
 *				filesys_init().
 *
 *	kl	05-Feb-1991	Get rid of krn$_reschedule. Change all 
 *				krn$_reschedule calls to reschedule(1) calls. 
 *				Spinunlock in CHMK isr. Load SCB directly
 *				with ip_isr.
 *
 *	jad	05-Feb-1991	Add release timer call.
 *                                                   
 *	sfs	25-Jan-1991	Fairly massive work on kernel scheduling
 *				primitives.  The _NOSHCEDULE varieties of
 *				KRN$_POST and KRN$_BPOST are gone (no longer
 *				needed).  SCHEDULE now no longer does anything
 *				if IPL is elevated; ISR exits must now call
 *				RESCHEDULE to cause the scheduling to take
 *				place.  KRN$_POST and KRN$_BPOST have been
 *				optimized to not reschedule if the task being
 *				woken will not immediately run.  The ability
 *				to nest spinlocks is now present, and can be
 *				enabled if desired (it is currently disabled).
 *				Kernel sequence number 0 is now reserved and
 *				will never be returned.  The concept of a
 *				"lightweight" context switch for Alpha is now
 *				implemented (this lightweight context switch
 *				can now be used in all cases where the context
 *				switch is voluntary, as in the case of posting
 *				or waiting on a semaphore); fewer registers
 *				need to be saved in this case.  The ADU model
 *				now runs with the timer enabled, so the calling
 *				of TIMER_INTERRUPT from the idle loop is gone.
 *				The calling of SWAP_CONTEXT is now folded into
 *				SCHEDULE (and RESCHEDULE), controlled by a
 *				flag; only the exit from an ISR would disable
 *				the automatic swapping of context.  Adjustment
 *				of process CPU quantum has been moved from the
 *				timer ISR to RESCHEDULE; this removed the need
 *				to special-case the timer ISR exit path.
 *
 *	pel	25-Jan-1991	krn$_create; don't return until stdin/err/out
 *				files have been opened.
 *
 *	sfs	22-Jan-1991	Reorder code in KRN$_IDLE.  Make sure timer
 *				is initialized before lowering IPL.  Do this
 *				before FILESYS_INIT.
 *                                                   
 *	jds	16-Jan-1991	Removed setjmp placeholder routine.  Enabled
 *				Alpha setjmps.
 *
 *	ajb	16-Jan-1991	Have killpending print out ^c.
 *
 *	mrc	14-Jan-1991	Added initialization of status block pointer in
 *				PCB to krn$_create().
 *
 *	kl	03-Jan-1991	Initialize HWRPB communications driver.
 *
 *	ajb	03-Jan-1991	Use the character array console_name as the
 *				default filename for the system, rather than
 *				relying on the value stored in     
 *				ddb [DDB$K_TT].name
 *
 *	pel	17-Dec-1990	krn$_create: fopen stderr 1st & call user
 *				defined startup routine before fopens to plug
 *				mem leak when shell startup chowns mem.
 *
 *	ajb	12-Dec-1990	Defined a poll queue that contains poll
 *				routines exported by drivers.  Routines are
 *				called on every timer tick by the timer
 *				process.
 *                                                   
 *	dtm	11-Dec-1990	Call memconfig during init.
 *
 *	jds	07-Dec-1990	Added setjmp/longjmp generic utility routines.
 *
 *	pel	29-Nov-1990	init stdin/out/err file descriptor ptrs in pcb.
 *				malloc stdin/out/err name filenames.
 *				Change ownership of stdin/out/err name memory.
 *				
 *	ajb	21-Nov-1990	Add check when releasing semaphores to insure
 *				that they are empty.
 *
 *	ajb	20-Nov-1990	Add timer interrupts for the ADU unit.
 *
 *	kl	16-Nov-1990	Handle IP interrupt
 *
 *	kl	16-Nov-1990	Modify krn$_remove_node
 *
 *	ajb	15-Nov-1990	Improve process timekeeping.
 *                                                                 
 *	kl	15-Nov-1990	Initialize process handler queues. Change 
 *				fake_scb to vax_scb.
 *
 *	dtm	14-Nov-1990	Create callback semaphore and server process
 *                                                   
 *	ajb	14-Nov-1990  	Remove control t
 *
 *	ajb	13-Nov-1990	Add ADU targets in some locations
 *
 *	ajb	08-Nov-1990	Change calling sequence to krn$_create so that
 *				access modes for stdin, stdout and stderr are 
 *				passed.
 *
 *	ajb	07-Nov-1990	remove code to check for pipes in krn$_startup
 *
 *	pel	07-Nov-1990	walksemaphore, print 31 char semnames, not 16.
 *				                     
 *	ajb	02-Nov-1990	Add krn$_unique and remove semaphore ownership
 *				queue.
 *
 *	sfs	31-Oct-1990	Get this to run under Mannequin.
 *
 *	ajb	31-Oct-1990	Rename LASER
 *
 *	pel	25-Oct-1990	open STDOUT, STDERR as "a" if a pipe.
 *
 *	kl	25-Oct-1990	Entry_context declaration.
 *
 *	ajb	24-Oct-1990	Change calling sequence to dyn$_flush
 *
 *	sfs	24-Oct-1990	Implement SPINLOCK and SPINUNLOCK here.
 *
 *	sfs	22-Oct-1990	Modify the way kills are implemented.  Kills
 *				are now advisory:  a process is notified that
 *				it should die, and it is up to the process to
 *				do the right thing.
 *
 *	kl	18-Oct-1990	Add timer queue spinlock so that one processor
 *				can execute krn$_sleep without clobbering the
 *				the timer task running on another processor.
 *				Cannot use kernel spinlock because it blocks
 *				interrupts, something we cannot afford to do
 *				in the timer task.
 *
 *	kl	18-Oct-1990	Remove poll of node removal in timer_interrupt.
 *				Modify node_remove, node_replace.
 *
 *	ajb	16-Oct-1990	Serialize access to lex and YACC
 *
 *	dtm	15-Oct-1990	Consolidate kernel_ev_create into ev_init.
 *				remove local evlist.  add ev_def.h.
 *
 *	ajb	15-Oct-1990	Align all PCBs on a 128 byte boundary (required
 *				by alpha architecture).
 *
 *	ajb	12-Oct-1990	Remove nonportable reference to nargloc
 *
 *	ajb	11-Oct-1990	Provide default process startup routine that
 *				opens files.  This routine does not presume
 *				the existence of an argc, argv.
 *
 *	kl	05-Oct-1990	Add in_console, user_halted masks.
 *
 *	kl	05-Oct-1990	Change affinity of idle tasks,timer task
 *				and deadeater task to -1. Load PCB[last_processor]
 *				in schedule and add this field to Walkpcb.
 *
 *	kl	05-Oct-1990	setcpu,showcpu,sethost
 *
 *	kl	02-Oct-1990	Bump the cputime in Schedule rather than in
 *				Timer_interrupt.       
 *
 *	kl	02-Oct-1990	Clear INT in timer control register on an
 *				interrupt. 
 *
 *	cfc	27-Sep-1990	Remove fopen()'s in krn$_create.  Files are
 *				now opened in shell_startup() callback routine
 *				in shell.c.  This routine is passed to 
 *				krn$_create, and is executed in krn$_helper
 *				upon process startup.
 *
 *	dtm	19-Sep-1990	Initialize local evlist pointer in krn$_create.
 *
 *	ajb	14-Sep-1990	Rewrite semaphore routines in C.
 *
 *	jad	14-Sep-1990	Add routine to disallow kills. 
 *
 *	kl	12-Sep-1990	Modify remove_node,replace_node. 
 *                                                   
 *	ajb	11-Sep-1990	Idle task quantums were not being set up.
 *
 *	ajb	05-Sep-1990	Move some routines from Macro to ease port
 *
 *	ajb	04-Sep-1990	Add user defined startup routine
 *
 *	pel	19-Aug-1990	open STDOUT, STDERR in append mode.
 *
 *	dtm	16-Aug-1990	Add initialization of environment variable
 *				subsystem to krn$_idle entry sequence.
 *
 *	kl	16-Aug-1990	SCB must be EXPORT for MP. Add 
 *				krn$_replace_node and krn$_remove_node.
 *
 *	ajb	01-Aug-1990	Add process quantums.
 *
 *	ajb	30-Jul-1990	Move functions previously in krn$_startup into
 *				krn$_create.  This allows file opens to be
 *				synchronous with the caller.
 *
 *	ajb	26-Jul-1990	Synchronize access to the PCBQ when walking
 *				down that queue to report processes.
 *
 *	ajb	24-Jul-1990	Add a field, krn$_sequence, that can be used
 *				for generating unique numbers that are used
 *				in the construction of process ids and unique
 *				file names.            
 *
 *	kl	23-Jul-1990	Append affinity to process name rather than
 *				processor id.
 *
 *	kl	19-Jul-1990	Modify so that secondaries do not run a timer
 *				task. Instead, they force a reschedule from the
 *				timer_isr.
 *				
 *	kl,dm	17-Jul-1990	Multiprocessor upgrade (first pass)
 *				
 *	ajb	13-Jul-1990	Change psl_def to vaxpsl_def.  Add 
 *				krn$_setpri and krn$_setaffinity headers
 *
 *	ajb	25-Jun-1990	Add ownership queues for dynamic memory
 *                                                     
 *      pel     08-Jun-1990	init SCB w/ exception/interrupt handlers.
 *
 *	kp	05-Jun-1990	Autodoc updates.
 *
 *      pel     30-May-1990	del krn$_bpost/_wait. krn$_bpost in kernel_util
 *
 *	sfs	30-May-1990	More timer-related work.
 *
 *	sfs	24-May-1990	Timer-related work.
 *
 *	ajb	01-May-1990	Remove read$, write$ (synchronization now
 *				handled in drivers).
 *
 *	jad	26-Apr-1990   	Add console support.
 *                                                     
 *	ajb	23-Apr-1990	Add file system initialization.
 *
 *	ajb	20-Apr-1990	make getpcb a subroutine call
 *
 *	ajb	19-Apr-1990	add read$, write$
 *
 *	ajb	22-Mar-1990	Initial entry.
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:prdef.h"
#include "cp$src:pal_def.h"                  
#include "cp$src:impure_def.h"                  
#include "cp$src:alphascb_def.h"
#include "cp$src:version.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"              
#include "cp$src:ev_def.h"
#include "cp$src:tt_def.h"
#include "cp$src:hwrpb_def.h"                   
#include "cp$src:ansi_def.h"
#include "cp$src:platform_cpu.h"
#include "cp$src:kbd.h"
#include "cp$src:get_console_base.h"
#include "cp$src:alpha_arc.h"
#include "cp$src:ctype.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#if MODULAR
int console_dlb_flag = 0;
struct QUEUE pidbq;            	/* bootp: per-interface database */
struct QUEUE bootpq;           	/* bootp: list of open bootps */
struct QUEUE tftpq;            	/* bootp: list of open tftps */
struct TIMERQ arp_tq;
struct SEMAPHORE inet_lock;
struct SEMAPHORE arp_lock;
struct SEMAPHORE report_lock;
extern int arc_enable_flag;	/* ARC enable flag */
struct ZONE _align (QUADWORD) twilight_zone;
unsigned short bootp_htype;  /* used in bootp packet, patching hard-wired ethernet */
extern int print_debug_flag;
#endif

#if !MODULAR
int scsi_ref_counter;
int scsi_debug_flag;
int mscp_ref_counter;
int nport_ref_counter;
int net_ref_counter;
int cinp_debug_flag;
int cixcd_debug_flag;
int cixcd_debug_node;
#endif

#if MIKASA
int glb_hlt;
#endif

#if TURBO 
#include "cp$src:turbo_nvr.h"
#include "cp$src:emulate.h"
#include "cp$src:turbo_eeprom.h"
int ctlp_debug = 1;		/* 1 = do only TLINTRMASK on callback */
int flatmem_check_flag = 0;     /* 0 = Flat memory, 1 = not Flat      */
int mixed_cpus_flag = 0;        /* 0 = similar cpus 1 = mixed ev5 ev56s  */
extern int xdelta_shared_adr;
extern int xdelta_startup;
extern int lfu_address;
#endif

#define ENTRY_STACK 6*1024		/* Most platforms use default */
#if TURBO || RAWHIDE
#define ENTRY_STACK 0x2000	/* Turbo/Rawhide needs 8K on reboot flow */
#endif

#if RAWHIDE
#include "cp$src:pb_def.h" 	/* struct HALT_LOG for eeprom*/
#include "cp$src:rawhide_eeprom.h" /* struct HALT_LOG for eeprom*/
#endif

#define ON 1
#define OFF 0
#define ENABLE 1
#define DISABLE 0

#define hlt$c_reset          0
#define hlt$c_hw_halt        1  /* ^P,  msg_hlt_hw [] = "operator initiated halt        */
#define hlt$c_ksp_inval      2  /*    msg_hlt_ksnv [] = "kernel stack not valid halt\n";*/
#define hlt$c_scbb_inval     3  /*    msg_hlt_scbb [] = "invalid SCBB\n";               */
#define hlt$c_ptbr_inval     4  /*    msg_hlt_ptbr [] = "invalid PTBR\n"; */
#define hlt$c_sw_halt        5  /*    msg_hlt_sw [] = "HALT instruction executed\n";   */
#define hlt$c_dbl_mchk       6  /*    msg_hlt_dblerr [] = "double error halt\n";       */
#define hlt$c_mchk_from_pal  7  /*    msg_hlt_pal_mchk [] = "machine check while in PAL mode\n"; */
#define hlt$c_callback      33
#define hlt$c_lfu_start     35
#define hlt$c_fault_reset   36

#if !MODULAR
extern char initial_heap[];
#endif

extern struct HWRPB *hwrpb;
extern unsigned int *scb;
extern struct SCBV *scbv;
extern long int SERIAL_LINE_FLAG;
extern int main_shell_pid;
extern unsigned CALLBACK_ENTRY_FLAG(void);
extern int primary_cpu;
extern int _krn$_start_;
#if ARC_SUPPORT
TREE *config_tree_root;
#endif

#if !MODULAR
int scsi_ref_counter;
int mscp_controllers;
int mscp_ref_counter;
int nport_ref_counter;
int net_ref_counter;
#endif

struct QUEUE mscp_cbs;

int diag$r_evs;				/* Common diag ev data. */
int initial_heap_ptr;
int advanced_mode_flag = 0;			/* basic commands only on powerup */
int diagnostic_mode_flag = 0;
int halt_pending;
int total_error_count = 0;		/* Total error count */
int arc_fe_inited = 0;			/* ARC front end in use */ 
int arc_hw_inited = 0;			/* ARC hardware state in use */
int arc_loaded = 0;			/* ARC AlphaBios loaded */

int graphics_console = 0;		/* 1 == use graphics port as primary */
int both_console = 0;			/* 1 == serial and grpahics tee'd */
int serial_console = 1;			/* 1 == use serial port as primary */
int keybd_present = 1;			/* implement routine to set this later */
int keybd_type = 0;			/* 0= no kbd, 1 = ps/2 kbd, 2 = usb kbd */
int graphics_enabled = 0;		/* 1 == graphics inited and tested */
int graphics_term_type = 0;		/* 0 == video, 1 == hardcopy */
int graphics_page_size = 0;		/* page mode off */
int graphics_device = 0;		/* 0 is none, 1 is vga, 2 is tga */

/* 
 * Global keyboard-language and keyboad-hardware_type - holds the values of 
 *  language and kbd_hardware_type EVs; prefilled with default value.
 */ 
unsigned int kbd_lang_array = (LANG_K_ENGLISH_AMERICAN-LANG_K_DANSK) >> 1;
unsigned int ctb_kbd_lang = LANG_K_ENGLISH_AMERICAN;
unsigned int kbd_hardware_type = PCXAL;

int boot_cpu_slot;			/* boot hwrpb slot pointer */
int boot_retry_counter;			/* boot retry counter */
int cpu_present;			/* mask of cpu installed */

int null_procedure();

struct SEMAPHORE scsi_ub_lock;

#if SABLE || RAWHIDE || MIKASA
int dsr_check[3];			/* OCP display COMx check */
#endif

#if SABLE || RAWHIDE || REGATTA || WILDFIRE
#define NULL_BASE 0x1000
#define NULL_SIZE 0x1000
#endif

#if GALAXY && TURBO
#define NULL_BASE 0x1100
#define NULL_SIZE 0xf00
#endif

#if MIKASA
#define NULL_BASE 0x800
#define NULL_SIZE 0x1000
#endif

#if NULL_SIZE
char *null_base;
int null_size;
#else
#define NULL_SIZE 0
#endif

void null_flush () {
#if NULL_SIZE
    extern struct TTPB *console_ttpb;
    extern struct TTPB null_ttpb;

    if (console_ttpb == &null_ttpb)
	return;
    if (null_size) {
	pprintf ("%*.*s", null_size, null_size, null_base);
	if (null_size == NULL_SIZE) {
	    pprintf ("<---output lost beginning here\n");
	}
	null_size = 0;
    }
#endif
}

int null_txready (int port) {
    return 1;
}

void null_txsend (int port, char c) {
#if NULL_SIZE
    if (null_size < NULL_SIZE) {
	null_base [null_size++] = c;
    }
#endif
}

struct TTPB null_ttpb = {		/* null terminal port block */
  0, 0,				/* QUEUE's flink, blink */
  null_procedure,		/* (*rxoff)() */
  null_procedure,		/* (*rxon)() */
  null_procedure,		/* (*rxready)() */
  null_procedure,		/* (*rxread)() */
  null_procedure,		/* (*txoff)() */
  null_procedure,		/* (*txon)() */
  null_txready,			
  null_txsend,			
  null_procedure,		/* (*lineset)() */			
  0,				/* port */
  1,				/* translate_nl */
  -1,				/* perm_mode */
  1				/* perm_poll */
};

unsigned int memory_low_limit;
UINT memory_high_limit;
UINT mem_size=0;
int idle_pids[MAX_PROCESSOR_ID];
int _align (QUADWORD) poll_pid;
int _align (QUADWORD) bad_page_offset = 0;
int _align (QUADWORD) in_console;
struct QUEUE _align (QUADWORD) readyq[MAX_PRI];
int _align (QUADWORD) eat_interrupts[MAX_PROCESSOR_ID];
int stepping = 0;			/* set if single stepping */

struct DDB *pmem_ddb_ptr;		/* Physical memory DDB pointer */
struct DDB *pi_ddb_ptr;			/* Pipe DDB pointer */
struct DDB *tt_ddb_ptr;			/* TTY DDB pointer */
struct QUEUE tt_pbs;

struct env_table *env_table_ptr;
struct EVNODE evlist;			/* header of the list of variables */
struct EVNODE *evmem;
struct SEMAPHORE evlock;		/* serialize access for variable
					 * modification */
int ev_inited = 0;			/* has driver been initialized */
int ev_initing;
char *entry_boottype = 0;

/*
 * Console_mode indicates whether a processors is in interrupt mode or
 * polled mode. Change_mode is used as an interlock to manage the 
 * mode transition.
 */

volatile int _align (QUADWORD) change_mode;
volatile int _align (QUADWORD) console_mode[MAX_PROCESSOR_ID];

/*
 * The Console Powerup In Progress bitmask indicates
 * which cpus are currently in the process of 
 * powering up.  We use this information to allow
 * empty ready queues in the idle process.
 */
int _align (QUADWORD) cpip = 0;

#if TURBO
int _align (QUADWORD) pipp = 0;
#endif
/*
 * The Callback In Progress flag indicates that a
 * callback is active.  Drivers may take special
 * action when this flag is set.  (For example, the
 * terminal class driver may not poll for received
 * characters, else it will grab characters which
 * belong to the host O/S.)
 */
int _align (QUADWORD) cbip = 0;
int (*fe_callback)();			/* boot front ends fill this in */
int cb_ref;
int cb_fp[16];
int cb_tt_fp[16];
UINT cb_time_limit;

/*
 * Timer related data structures
 */
extern int _align (QUADWORD) timer_cpu;

/*
 * Process related data structures
 */
extern struct SEMAPHORE	_align (QUADWORD) create$;
extern struct QUEUE _align (QUADWORD) pcbq;	/* linked list of all known processes */
extern struct SEMAPHORE _align (QUADWORD) deadpcb$;
extern struct QUEUE _align (QUADWORD) dead_pcbq; /* dead pcbs that need to be freed */
extern int _align (QUADWORD) krn$_pcount;	/* process count */
extern int krn$_poll();

int _align (QUADWORD) krn$_sequence;

struct LOCK _align (QUADWORD) spl_atomic = {0, IPL_SYNC, 0, 0, 0, 0, 0};

struct LOCK _align (QUADWORD) spl_kernel = {0, IPL_SYNC, 0, 0, 0, 0, 0};

struct LOCK _align (QUADWORD) spl_exdep = {0, IPL_RUN, 0, 0, 0, 0, 0};

struct LOCK _align (QUADWORD) spl_printf = {0, IPL_SYNC, 0, 0, 0, 0, 0};

struct LOCK _align (QUADWORD) el_spinlock = {0, IPL_SYNC, 0, 0, 0, 0, 0};

struct LOCK _align (QUADWORD) spl_toy = {0, IPL_SYNC, 0, 0, 0, 0, 0};

int hlt_table[] = {msg_hlt_hw, msg_hlt_ksnv, msg_hlt_scbb, msg_hlt_ptbr,
  msg_hlt_sw, msg_hlt_dblerr, msg_hlt_pal_mchk};

extern int zcount;
int scsi_poll;
int scsi_reset;
int force_failover_auto;
int force_failover_next;
int servers_enabled;
int drivers_running;
struct pb *console_pb;
struct TTPB *console_ttpb;		/* pointer to primary console port */

extern struct TTPB serial_ttpb;		/* serial port terminal port block */
#if MODULAR
struct TTPB *serial_ttpbs;
struct TTPB *graphics_ttpbs;
#endif

extern struct LOCK spl_iobq;

#if SABLE || TURBO || RAWHIDE || REGATTA || WILDFIRE || MONET || EIGER
struct LOCK _align (QUADWORD) spl_entry = {0, IPL_SYNC, 0, 0, 0, 0, 0};
extern struct LOCK spl_mchk;
#endif

#if TURBO
extern struct LOCK spl_sbe;
#endif

#if SABLE
extern struct LOCK spl_pci;
#endif

int iic_inited;				/* Flag to indicate if we are already
					 * inited */
int *pua_spt;

#if RAWHIDE
extern unsigned int cpu_available;
extern unsigned int screen_mode;
extern int running_from_bcache;
extern int ERR_LOG_FLAG;
#endif

#if TURBO
extern int running_from_bcache;
extern unsigned int screen_mode;
extern int ERR_LOG_FLAG;
int dump_hw_state = 1;
#endif

#if MODULAR
extern int ___verylast;
#endif

struct TTPB *hwrpbtt_ttpbs[MAX_PROCESSOR_ID];
extern int cpu_mask;
#if GALAXY
extern int galaxy_node_number;
#endif

/*
 * Heap size (put here to keep dynamic.c machine independent) and base.
 * Normally, we define the heap size at compile time.  However, if
 * this is patched, the new size takes effect after an INIT.  This
 * is a convenient way to change the size and base of the heap without
 * rebuilding.
 *
 * After __last are the per cpu idle stacks, and then the base of the
 * heap.  This base address is passed to krn$_idle, and remembered in the 
 * cell end_of_code.  This cell is used iff heap base is 0.
 */
#if !MODULAR
unsigned int heap_size = HEAP_SIZE;
unsigned int heap_base = HEAP_BASE;
#else
unsigned int heap_size_val = HEAP_SIZE;
unsigned int heap_base_val = HEAP_BASE;
#endif
unsigned int high_heap_size;
unsigned int lower_heap_size;
unsigned int lower_heap_base;

extern int krn$_helper();


extern struct FILE *fopen();
extern struct ZONE defzone;
#if TURBO
extern struct ZONE smallzone;
extern struct ZONE tinyzone;
#endif
extern sysfault();
extern powerup();
extern krn$_timer();
extern system_reset_or_error();
extern boot_system();

extern struct PCB *krn$_findpcb();
extern krn$_process();
extern krn$_dead();

#if TURBO
extern struct HQE mchq_660;		/*MCHECK handler queue*/
extern int do_bpt();
extern int turbo_ipir();
extern int mcheck_handler();
extern int mcheck_handler_620();
extern int mcheck_handler_630();
extern int turbo_unexpected_int();
#endif

volatile int quilt_inited = 0;		/* flag to tell quilt to reinitialize  
					 *  */
int exdep_inited;			/* flag to show if exdep_init called
					 * yet */

int tt_inited;

#if AVANTI || ALCOR || MTU || MONET
extern int apu_start;
/* This is 8 Mbytes of space for update code + rom images	*/
/* And L2 Pte's that get allocated off the pfn map 		*/
/* apu$mapping.c allocates memory in the upper memory region	*/
#if AVANTI
#define ApuHeapOffset 0x400000
#else
#define ApuHeapOffset 0x600000
#endif   

#endif

#if !MODULAR
extern void PowerUpProgress (unsigned char value, char *string, ...);
#endif

/*+
 * ============================================================================
 * = null_procedure - Always returns success without side effects             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routines acts as a well defined procedure that always returns 
 *	success.  It can be used as place holder where a procedure is required,
 *	but activation of that procedure must be fast and have no side effects.
 *
 * FORM OF CALL:
 *
 *	null_procedure ()
 *
 * RETURNS:
 *
 *      0 - Success.
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
int null_procedure(void) 
{
    return msg_success;
}

int fatal_procedure(void) 
{
    return msg_failure;
}

/*+
 * ============================================================================
 * = krn$_unique - create a system wide unique identifier                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Given a prefix, make a unique identifier.  The unique portion is
 *	designed such that it starts with an alphabetic character, so that it
 *	is a valid identifier.
 *  
 * FORM OF CALL:
 *  
 *	krn$_unique (prefix, name)
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:           
 *
 * 	char *prefix - prefix of filename
 *	char *name - address of a buffer bug enogh to hold new name
 *
 * SIDE EFFECTS:
 *
 *	krn$_sequence is bumped.  No bounds checking is done on the length of
 *	the name.
 *
-*/
int krn$_unique(char *prefix, char *name)
{
    int sequence;

    sequence = krn$_unique_number();

    sprintf(name, "%sN%08X", prefix, sequence);
    return msg_success;
}

int krn$_unique_number()
{
    int sequence;

    spinlock(&spl_kernel);
    while (++krn$_sequence == 0) {
    }
    sequence = krn$_sequence;
    spinunlock(&spl_kernel);

    return sequence;
}

void krn$_init_spinlock(struct LOCK *lock, int req_ipl)
{
    memset(lock, 0, sizeof(struct LOCK));
    lock->req_ipl = req_ipl;
}

/*+
 * ============================================================================
 * = krn$_idle - Idle process                                                 =
 * ============================================================================
 *
 * OVERVIEW:            
 *
 *	Kernel idle processes.  This routine is executed for every cpu in the
 *	system, and becomes that cpu's idle process.
 *
 * FORM OF CALL:
 *
 *	krn$_idle (free)
 *
 * RETURNS:
 *
 *      None
 *       
 * ARGUMENTS:
 *                  
 *      char *free	- pointer to start of free memory
 *
 * SIDE EFFECTS:
 *
 *      Multiple, as the shell and i/o subsystem are started up.
 *
-*/
void krn$_idle(char *p)
{
    struct PCB *pcb;
    struct SEMAPHORE *s;
    int i;
    char name[8];
    int id;
    struct impure *IMPURE;
    int hcode;
    int delay_count;
    int size;
    int base;
    int *adr;
    char *free;
    UINT phys_base;
    int ptr;
    int *lfu_adr;

    id = whoami();

    if (primary())
	primary_cpu = id;

    free = p;

#if RAWHIDE || TURBO
    screen_mode = 1;
#endif

/* Perform primary platform specific initialization */

    platform_init1();

#if AVANTI || ALCOR || K2 || TAKARA || MONET || EIGER
    {
	unsigned char data;
	unsigned char year;
	
	year = rtc_read( 0x09);

	data = rtc_read( 0x3f );
	if (data == 1)
	{
	    if ((year >= 93) && (year <= 99))
	    {
		year -= 80;
	    }
	    else if ((year >= 0 ) && (year <= 12)) 
	    {
		year += 20;
	    }	
	}
	else
	{
	    if ((year >= 13) && (year <= 19))
	    {
		year += 80;
	    }
	    else if ((year >= 20 ) && (year <= 40))
	    {
		year -= 20;
	    }
	}
#if FWUPDATE
	if (1)
#else
#if MIKASA || APC_PLATFORM || K2 || TAKARA || EIGER
	if (0)
#else
	if (apu_start)
#endif
#endif
	{
	    /* force graphics if apu code */

	    if (data == 1)
	    {
		rtc_write( 0x17, 1);
	    }
	}
	rtc_write(0x09, year);
    }
#endif

#if SABLE

/* if the console is linked above PAL$CONSOLE_BASE then set the free */
/* memory starting at 512KB. */
    if (&_krn$_start_ > PAL$CONSOLE_BASE)
	free = 0x80000;
#endif

    if (primary()) {
#if MAX_PROCESSOR_ID - 1
#ifdef CSERVE$MP_WORK_REQUEST
#ifdef MP$RESTART
	for (i = 0; i < MAX_PROCESSOR_ID; i++)
	    cserve(CSERVE$MP_WORK_REQUEST, i, MP$RESTART);
#endif
#endif
#endif
	krn$_init_spinlock(&spl_atomic, IPL_SYNC);
	krn$_init_spinlock(&spl_kernel, IPL_SYNC);
	krn$_init_spinlock(&spl_exdep, IPL_RUN);
	krn$_init_spinlock(&spl_printf, IPL_SYNC);
	krn$_init_spinlock(&el_spinlock, IPL_SYNC);
	krn$_init_spinlock(&spl_toy, IPL_SYNC);
	krn$_init_spinlock(&spl_iobq, IPL_SYNC);
#if SABLE || TURBO || RAWHIDE || REGATTA || WILDFIRE || MONET || EIGER
	krn$_init_spinlock(&spl_mchk, IPL_SYNC);
#endif
#if TURBO
	krn$_init_spinlock(&spl_entry, IPL_SYNC);
	krn$_init_spinlock(&spl_sbe, IPL_SYNC);
#endif
#if SABLE
	krn$_init_spinlock(&spl_pci, IPL_SYNC);
#endif
	zcount = 0;
	cpip = 0;
	cbip = 0;
	el_setup();
	ev_inited = 0;
	ev_initing = 0;
	exdep_inited = 0;
	tt_inited = 0;
	krn$_sequence = 0;
	drivers_running = 1;
	pua_spt = 0;
	cb_ref = 0;

	serial_reinit();

	memset(cb_tt_fp, 0, 64);
	hwrpb = 0;

	memset(PAL$IMPURE_BASE + IMPURE$PCB_OFFSET, 0, MAX_PROCESSOR_ID * 8);
	console_pb = 0;

#if GALAXY && TURBO
	if (get_console_base_pa()) console_ttpb = &null_ttpb;
	else
#endif
	console_ttpb = &serial_ttpb;
#if MODULAR
#if GALAXY && TURBO
	if (get_console_base_pa()) serial_ttpbs = &null_ttpb;
	else
#endif
	serial_ttpbs = &serial_ttpb;
#endif
#if MIKASA
	glb_hlt = 0;		    /* initialize the saved global halt code */
	*( int *)PAL$HALT_BUTTON_PRESSED = 0; 
#endif
	iic_inited = 0;
	main_shell_pid = 0;
	memset(idle_pids, 0, sizeof(idle_pids));
	cpu_mask = 0;
	cpu_present = 0;
	in_console = 0;
	memset(hwrpbtt_ttpbs, 0, sizeof(hwrpbtt_ttpbs));
	entry_boottype = 0;
	memset(&defzone, 0, sizeof(struct ZONE));
#if TURBO
	memset(&smallzone, 0, sizeof(struct ZONE));
	memset(&tinyzone, 0, sizeof(struct ZONE));
#endif
	change_mode = 0;
	scsi_poll = 1;
	scsi_reset = 1;
	force_failover_auto = 0;
	force_failover_next = 0;
#if NULL_SIZE
	null_base = NULL_BASE;
	null_size = 0;
#endif
	arc_fe_inited = 0;
#if ARC_SUPPORT
	config_tree_root = 0;
#endif
    }

/* Set our powerup in progress flag */

    spinlock(&spl_kernel);
    cpip |= 1 << id;
#if TURBO
    pipp = cpip;
#endif
    spinunlock(&spl_kernel);

/* Say this cpu is running console */

    spinlock(&spl_kernel);
    in_console = in_console | (1 << id);
    spinunlock(&spl_kernel);

/* Perform secondary platform specific initialization */

    platform_init2();

/*
 * Initialize our resource ownership queues.  We need do this first so that
 * when dynamic memory is initialized, the semaphores belong to the idle
 * task.
 */
    pcb = getpcb();

    pcb->pcb$l_pstate = KRN$K_RUNNING;

    pcb->pcb$r_dq.flink = (void *) &pcb->pcb$r_dq.flink;
					/* dynamic memory */
    pcb->pcb$r_dq.blink = (void *) &pcb->pcb$r_dq.flink;

    pcb->pcb$r_hq.flink = (void *) &pcb->pcb$r_hq.flink;
					/* handlers */
    pcb->pcb$r_hq.blink = (void *) &pcb->pcb$r_hq.flink;

    pcb->pcb$r_sjq.flink = (void *) &pcb->pcb$r_sjq.flink;
					/* setjmps */
    pcb->pcb$r_sjq.blink = (void *) &pcb->pcb$r_sjq.flink;

    pcb->pcb$r_iobq.flink = (void *) &pcb->pcb$r_iobq.flink;
					/* IOBs */
    pcb->pcb$r_iobq.blink = (void *) &pcb->pcb$r_iobq.flink;

    pcb->pcb$r_fq.flink = (void *) &pcb->pcb$r_fq.flink;
					/* files */
    pcb->pcb$r_fq.blink = (void *) &pcb->pcb$r_fq.flink;

/* Define our quantums such that we always have the cpu. */

    pcb->pcb$l_qexpire = 1;
    pcb->pcb$l_qexpire_dec = 0;
    pcb->pcb$l_quantum = -1;
    pcb->pcb$q_cputime[0] = 0;
    pcb->pcb$q_cputime[1] = 0;
    pcb->pcb$l_pri = MAX_PRI - 1;
    pcb->pcb$a_semp = 0;

/* Finish filling in fundamental stuff. */

    pcb->pcb$l_affinity = 1 << id;
    pcb->pcb$a_flink = (void *) &pcb->pcb$a_flink;
					/* semaphore queue      */
    pcb->pcb$a_blink = (void *) &pcb->pcb$a_flink;
    strcpy(&pcb->pcb$b_name[0], "idle");
    pcb->pcb$a_stdin_name = "tt";
    pcb->pcb$a_stdout_name = "tt";
    pcb->pcb$a_stderr_name = "tt";
    pcb->pcb$a_rundown = (void *) null_procedure;
    pcb->pcb$a_task = (void *) 0xdeadbeef;
					/* we're never called */
    pcb->pcb$l_shellp = 0;
    diag$r_evs |= m_rpt_full;

#if TURBO
    lfu_address = 0;			/* clear LFU indicator */

    if (!downloaded()) {
	uart_init();			/* initialize uarts */
	if (id == 0)
	    pprintf("\n");
    }
    if (downloaded()) {
/*
 * Determine if this image also includes the LFU data.
 * Search for 'LFU0' as the second word in each 512 byte block
 * of the file just loaded and copied to 48Mb. If found, then 
 * set the flag for LFU invocation on console startup.
 * Search up to 2Mb of the file for the LFU indication.
 */
	lfu_adr = 48*1024*1024;			/* start at 48Mb */
	for (i=0; i < (2*1024*1024); i+=512) {
	    if (lfu_adr[1] == 'LFU0') {
		lfu_address = lfu_adr;
		break;
	    }
	(int)lfu_adr += 512;
	}
	if (primary())
	    if (lfu_address)
		pprintf("\nStarting new console, Please wait a minute...");
    }
#endif

/* Display starting messasge */

    PowerUpProgress(0xff, "starting console on CPU %d\n", id);

#if NONZERO_CONSOLE_BASE                        
    phys_base = get_console_base_pa();
    if (primary() && phys_base) {
	err_printf("console physical memory base is %x\n", phys_base);
    }
#endif

/* Print out the Halt Code and PC */

    IMPURE = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
      (id * PAL$IMPURE_SPECIFIC_SIZE);
    hcode = *IMPURE->cns$hlt;

#if TURBO
    if ((hcode != 0) && (hcode != 34) && (hcode != hlt$c_dbl_mchk) && (hcode != hlt$c_mchk_from_pal)) {
	pprintf("\n*** Console Error, CPU %d, Restarting ***\n", id);
	pprintf("  halt code = %d, PC = %-16x\n", hcode, *(INT *) IMPURE->cns$exc_addr & ~3);
	krn$_sleep(10000);
#if !XDELTA_ON
	if (xdelta_shared_adr)
#endif
	{
	    xdelta_startup = 1;
	    do_bpt ();
	    xdelta_startup = 0;
	}
	console_restart ();		/* Init the console */
    }
#endif

    spinlock(&spl_kernel);
    if (hcode) {
	qprintf("halt code = %d\n", hcode);
	if ((hcode >= 1) && (hcode <= 7)) {
	    qprintf(hlt_table[hcode - 1]);
	}
	qprintf("PC = %-16x\n", *(INT *) IMPURE->cns$exc_addr & ~3);
	if ((hcode == 6) || (hcode == 7)) {
	    INT *p;

	    qprintf("impure area for CPU %d (at %x)\n", id, IMPURE);
	    for (p = IMPURE; p < (INT) IMPURE + PAL$IMPURE_SPECIFIC_SIZE;
	      p++) {
		qprintf("%x: %016x\n", p, *p);
	    }
	}
    }
    spinunlock(&spl_kernel);

/* Set up the semaphore and process queue headers */

    PowerUpProgress(0xfe, "initialized idle PCB\n");

    if (primary()) {

	timer_cpu = primary_cpu;

	PowerUpProgress(0xfd, "initializing semaphores\n");

	pcbq.flink = (void *) &pcbq.flink;
	pcbq.blink = (void *) &pcbq.flink;
	krn$_sem_startup();

/* Initialize the process creation semaphore */

	krn$_seminit(&create$, 1, "create");

/* Initialize the reporting semaphore */

#if MODULAR
	krn$_seminit(&report_lock, 1, "report");
#endif

/* Initialize the semaphores for dead pcbs */

	dead_pcbq.flink = (void *) &dead_pcbq.flink;
	dead_pcbq.blink = (void *) &dead_pcbq.flink;
	krn$_seminit(&deadpcb$, 0, "dead_beef");

/* initialize the ready queue */

	for (i = 0; i < MAX_PRI; i++) {
	    readyq[i].flink = &readyq[i];
	    readyq[i].blink = &readyq[i];
	}
/*
 * Bring up dynamic memory.  Insure that the start address is
 * aligned.  If the base address of the heap is non zero, then
 * we use that.  Otherwise, we put the heap starting at __last.
 */
        PowerUpProgress(0xfc, "initializing heap\n");

#if !MODULAR
	initial_heap_ptr = &initial_heap;
#endif
#if MODULAR
	free = setup_initial_heap(free);
	initial_heap_ptr = free;
#endif

#if !MEDULLA
	PowerUpProgress(0xfb, "initial heap %x\n", initial_heap_ptr);
#endif

	memory_low_limit =
	  ((unsigned int) free + HWRPB$_PAGESIZE - 1) & ~(HWRPB$_PAGESIZE - 1);
	memory_high_limit = 0;

#if ALCOR || MIKASA || MTU || TAKARA || APC_PLATFORM
	if (memory_low_limit < 1 * 1024 * 1024)
	    memory_low_limit = 1 * 1024 * 1024;
#endif
#if SABLE
	if ((memory_low_limit < 1 * 1024 * 1024) && (&_krn$_start_ <= PAL$CONSOLE_BASE))
	    memory_low_limit = 1 * 1024 * 1024;
#endif
#if AVANTI || ALCOR || MTU || MONET
/* Check for the Update Utility     */
/* Standalone load case             */
/* Push the heap past update code   */

	if (apu_start)
	    memory_low_limit += ApuHeapOffset;
#endif
#if FWUPDATE
/* for the update utility allways save the space after it. */
	    memory_low_limit += 0x400000;
#endif
	base = ((int) initial_heap_ptr + DYN$K_FRAG - 1) & ~(DYN$K_FRAG - 1);
	size = MIN_HEAP_SIZE - DYN$K_FRAG;
	lower_heap_size = size;
	lower_heap_base = base;
	high_heap_size = 0;

#if MODULAR
	setup_twilight_zone_heap();
#endif

#if !MEDULLA
    	PowerUpProgress(0xfa, "memory low limit = %x\nheap = %x, %x\n", 
		memory_low_limit, lower_heap_base, lower_heap_size);
#endif

	dyn$_init(&defzone, lower_heap_base, lower_heap_size, 1);
#if TURBO
	ptr = malloc_noown(4*1024);
	dyn$_init(&tinyzone, ptr, 4*1024, 1);

	ptr = malloc_noown(20*1024);
	dyn$_init(&smallzone, ptr, 20*1024, 1);
#endif

/* set up the ddb and inode entries */

	PowerUpProgress(0xf9, "initializing driver structures\n");

	filesys_init();

/* initialize the scb */

	scb_init();

#if MODULAR
	ovly_init();			/* init system overlay capability */
#endif

	krn$_pcount = 1;
    } else {
	krn$_pcount++;
    }

/*
 * Put our own PCB onto the PCB queue and define our process id.  Put our
 * pid in a an array so other processes may quickly find out the pid of the
 * idle process.
 */
    PowerUpProgress(0xf8, "initializing idle process PID\n");

    spinlock(&spl_kernel);
    while (++krn$_sequence == 0) {
    }
    pcb->pcb$l_parentpid = pcb->pcb$l_pid = krn$_sequence;
    insq(&pcb->pcb$r_pq, &pcbq);
    spinunlock(&spl_kernel);
    idle_pids[id] = pcb->pcb$l_pid;

    mtpr_mces(7);
    mtpr_scbb(virt_to_phys(scb));

    if (primary()) {
#if TURBO
	int_vector_set(SCB$Q_SLE, do_bpt, 0);
	int_vector_set(SCB$Q_INTERPROCESSOR, turbo_ipir, 0);
	int_vector_set(SCB$Q_SYS_CORR_ERR, mcheck_handler_620,
	  SCB$Q_SYS_CORR_ERR);
	int_vector_set(SCB$Q_PR_CORR_ERR, mcheck_handler_630,
	  SCB$Q_PR_CORR_ERR);
	int_vector_set(SCB$Q_SYS_MCHK, mcheck_handler, SCB$Q_SYS_MCHK);
	int_vector_set(SCB$Q_PR_MCHK, mcheck_handler, SCB$Q_PR_MCHK);
	int_vector_set(SCB$Q_6F0, turbo_unexpected_int, 0);
	mchq_660.flink = &mchq_660;
	mchq_660.blink = &mchq_660;
#endif

#if TURBO
	uart_init();
#endif

#if !TURBO
	xdelta();
#endif

#if TL6 && XDELTA_ON
	if (secure_switch()) {
	    xdelta();
	    do_bpt();
	}
#endif
#if !TL6 && TURBO && XDELTA_ON
	if (nvr_flag(NVR$K_XDELTA_ENABLE)||secure_switch()) {
	    xdelta();
	    do_bpt();
	}
#endif

#if RAWHIDE && XDELTA_ON
	xdelta();
	do_bpt();
#endif

#if MODULAR
/*  init flash rom overlay index */

	qprintf("initializing flash index\n");
	hf_init();
#endif

#if !( TURBO || RAWHIDE )
	if (hcode != hlt$c_reset)
	    do_bpt();
#endif
    }

#if RAWHIDE
    err_printf("starting console on CPU %d\n", id);
#endif

#if MEDULLA || CORTEX || TAKARA || EB164 || YUKONA
    bus_init();				/* energize the bus functionality */
#endif

/*
 * Set up structures for the timer task if on a primary.
 * On all machines start the the timer ticking.
 */
    if (primary()) {

/* Initialize phase 0 drivers */

        PowerUpProgress(0xf7, "initializing file system\n");

	ddb_startup(0);			/* nl, rd, lbus inits */

/* Perform required hardware initialization */

	initialize_hardware();

        PowerUpProgress(0xf6, "initializing timer data structures\n");

	krn$_timer(1);
	krn$_poll(1);
    }

    krn$_timebase_init();
    krn$_readtime(pcb->pcb$q_timestamp);

    console_mode[id] = INTERRUPT_MODE;

/* Lower our IPL */

    PowerUpProgress(0xf5, "lowering IPL\n");

    eat_interrupts[id] = 1;		/* Eat garbage interrupts */
    mtpr_ipl(IPL_RUN);

/*
 * When do we stop eating interrupts?  How do we insure that all "garbage"
 * interrupts have been eaten before disabling eat_interrupts?
 */
    eat_interrupts[id] = 0;		/* Stop eating interrupts */

/* Compute this CPU's cycle time. */

#if TURBO && EV5

    /*
     *  Check to see if the timer is interrupting.  On some Turbo
     *  modules, the WATCH chip provides the interrupt.  The newer
     *  modules (pass 2) are normally jumpered so that the square wave
     *  output causes the interrupt.  By default, we do not enable the
     *  WATCH chip interrupts.  If we find out in timer_check, that
     *  the interrupt is not occurring, we enable the WATCH chip to interrupt.
     */
    if ( running_from_bcache ) {
	PowerUpProgress(0xf4, "timer check\n");
        cserve( CSERVE$MTPR_INTIM_FLAG, 1 ); 
	if (platform() != ISP_MODEL)
	    timer_check();           
    }

/*  TURBO HACK - set_cycle_time() not implemented in ISP */

    if (platform()==TURBO_HW)
#endif
	krn$_timer_set_cycle_time();

    if (primary()) {

        krn$_seminit( &scsi_ub_lock, 1, "scsi_ub_lock" );

/* Start up the task that deallocates dead pcbs. */

	PowerUpProgress(0xf3, "create dead_eater\n");

	krn$_create(krn$_dead, 		/* address of process           */
	  null_procedure, 		/* startup routine              */
	  0, 				/* completion semaphore         */
	  6, 				/* process priority             */
	  -1, 				/* cpu affinity mask            */
	  0, 				/* stack size                   */
	  "dead_eater", 		/* process name                 */
	  "nl", "r", "nl", "w", "nl", "w");

/* start the polling task */

	PowerUpProgress(0xf2, "create poll\n");

	poll_pid = krn$_create(krn$_poll, 
					/* address of process           */
	  null_procedure, 		/* startup routine              */
	  0, 				/* completion semaphore         */
	  MAX_PRI - 2, 			/* process priority             */
	  -1, 				/* cpu affinity mask            */
	  0, 				/* use system default stack size*/
	  "poll", 			/* process name                 */
	  "nl", "r", "nl", "w", "nl", "w", 0);

/* start the timer task */

	PowerUpProgress(0xf1, "create timer\n");

	krn$_create(krn$_timer, 	/* address of process           */
	  null_procedure, 		/* startup routine              */
	  0, 				/* completion semaphore         */
	  MAX_PRI - 1, 			/* process priority             */
	  -1, 				/* cpu affinity mask            */
	  0, 				/* use system default stack size*/
	  "timer", 			/* process name                 */
	  "nl", "r", "nl", "w", "nl", "w", 0);
    }

/*
 * Start the power up process which will control the screen and finish all 
 * the initialization. Since the tt driver isn't alive yet, this process
 * gets the nl driver for its io.
 */
    PowerUpProgress(0xf0, "create powerup\n");
#if TURBO
    if (id == 0)
	pprintf("\n");
#endif

    pcb->pcb$l_pri = 0;
    krn$_create(powerup, 		/* address of process           */
      null_procedure, 			/* startup routine              */
      0, 				/* completion semaphore         */
      3, 				/* process priority             */
      1 << id, 				/* cpu affinity mask            */
      8192, 				/* stack size                   */
      "powerup", 			/* process name                 */
      "nl", "r", "nl", "w", "nl", "w");

/* per cpu idle loop */ 
                        
    PowerUpProgress(0xf4, "entering idle loop\n");
    
    while (1) {

/* If we don't have a hardware source of timer interrupts, then  we
 * simulate timer interrupts by calling the timer interrupt service
 * routine from the idle loop at what we estimate to be 1 msec
 * intervals.  We switch dynamically from one mode to the other to
 * handle broken hardware (this actually happened on early prototypes).
 */
	timer_check();

/* A pprintf may have occured during an XOFF period, and the XON was
 * eaten by pprintf.   In this case, the sleeping thread will never be 
 * woken up. */

	if (console_ttpb->flow == 1) {
	    if (console_ttpb->txready(console_ttpb->port)) {
		console_ttpb->flow = XON;
		console_ttpb->txon(console_ttpb->port);
	    }
	}
    }
}

#if MODULAR

static void pwrcomp(void);

void powerup()
{
    int id = whoami();
    struct EVNODE *evp, evn;

    ovly_load("POWERUP");

    ovly_load("diagsupport");

    powerup_ovly();

#if GALAXY
    if (!galaxy_node_number) 
#endif
    {
    ovly_remove("diagsupport");

    ovly_remove("POWERUP");
    }

#if ARC_SUPPORT
/* 
 * Initialize environment variables used by ARC.
 */
    if (primary()) {
	evp = &evn;
	if (ev_read("arc_enable", &evp, EV$K_SYSTEM) == msg_success) {
	    if ( !( strcmp(evp->value.string, "ON") ) ) {

		arc_enable_flag = 1;

		ovly_load("ARC");

		ev_initing = 1;

		arc_ev_init();			/* create ev's */

		ev_initing = 0;

		arc_evinit ();			/* run time init ev's */
	    }
	}
    }
#endif

    if (primary()) {
	krn$_create(pwrcomp, 		/* address of process           */
	  null_procedure, 		/* startup routine              */
	  0, 				/* completion semaphore         */
	  5, 				/* process priority             */
	  1 << id, 			/* cpu affinity mask            */
	  8192, 			/* stack size                   */
	  "pwrcomp", 			/* process name                 */
	  "tt", "r", "tt", "w", "tt", "w");
    }
}

static void pwrcomp(void)
{
    int status = 1;
#if TURBO || RAWHIDE
    if (primary()) {
	krn$_sleep(1000);
	conzone_remove();
#if TURBO
	status = os_determination();		/* See if NT or LFU */
#endif
	if (status)
	    create_shell();
    }
#endif
}
#endif

/*+
 * ============================================================================
 * = schedule - find next executable process                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Dequeue highest priority ready process from the readyq that is allowed
 *	to run on this processor, and optionally swap context.
 *  
 * FORM OF CALL:
 *  
 *	schedule (flag)
 *  
 * RETURNS:
 *
 *	x - address of PCB
 *       
 * ARGUMENTS:
 *
 *	flag - true if an automatic swap_context should be done
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int schedule(int flag)
{
    int i;
    int who, who_mask;
    struct PCB *pcb;
    struct QUEUE *qp;
    int current_time[2];

    who = whoami();
    who_mask = 1 << who;
    pcb = getpcb();

/* Do the IPL checking only if in interrupt mode */


    if (console_mode[who] == INTERRUPT_MODE) {
	if (flag) {

/* If flag is set, we are here from a normal kernel operation
 * (such as KRN$_POST or KRN$_WAIT).  We disallow task switching 
 * while at elevated IPL (this is to allow ISRs to use normal
 * kernel routines without worrying about losing control of the
 * CPU).  Now, we can only get here if we've previously obtained 
 * the kernel spinlock, so we can look into that spinlock for the 
 * IPL prior to the spinlock, and THAT is the IPL that we're
 * interested in.  If IPL is indeed elevated, we merely undo the 
 * work that was done previously to stall the current task, drop
 * the kernel spinlock, and return.
 */


	    if (spl_kernel.sav_ipl != IPL_RUN) {
		if (pcb->pcb$l_pstate != KRN$K_READY)
		    sysfault();

		pcb->pcb$l_pstate = KRN$K_RUNNING;
		remq(pcb);
		spinunlock(&spl_kernel);
		return 0;
	    }

	} else {

/* If flag is clear, we are here from COMMON_ISR.  Again, we want 
 * to disallow task switching while at elevated IPL.  Now the IPL 
 * of interest is the IPL at the time of the interrupt/exception
 * which caused COMMON_ISR to be entered.  This can be found saved
 * somewhere on the stack.  If IPL is indeed elevated, we merely 
 * undo the work that was done previously to stall the current
 * task, and return without dropping the kernel spinlock.
 */
	    int ipl;
	    struct ALPHA_CTX *acp;

	    acp = pcb->pcb$a_acp;
	    if (acp) {
		ipl = (acp->acx$q_ps[0] >> 8) & 0x1f;
		if (ipl > IPL_RUN) {
		    if (pcb->pcb$l_pstate != KRN$K_READY)
			sysfault();
		    pcb->pcb$l_pstate = KRN$K_RUNNING;
		    remq(pcb);
		    return (int) pcb;
		}
	    }
	}
    }

/* Keep walking down the ready queue until we find a runnable process. */

    while (1) {
	for (i = MAX_PRI - 1; i >= 0; i--) {
	    if (readyq[i].flink == &readyq[i].flink)
		continue;

/* Find a PCB that has the right bit in the affinity mask set */

	    qp = readyq[i].flink;
	    while (qp != &readyq[i].flink) {
		pcb = (int) qp - offsetof(struct PCB, pcb$a_flink);

		if (pcb->pcb$l_affinity & who_mask) {
		    if (pcb->pcb$l_pstate != KRN$K_READY)
		    {
			sysfault();
		    }
		    remq(qp);
		    pcb->pcb$l_last_processor = who;
		    pcb->pcb$l_pstate = KRN$K_RUNNING;
		    pcb->pcb$q_timestamp[0] = current_time[0];
		    pcb->pcb$q_timestamp[1] = current_time[1];
		    if (flag) {
			swap_context(pcb);
		    }
		    return (int) pcb;
		}
		qp = qp->flink;
	    }
	}

/* We didn't find anything on the ready queue.  If we're powering up,
 * give up the spinlock to let other processes in (such as the timer), 
 * then take the spinlock and go back and scan the readyq again.  This 
 * allows the idle process of a powering up CPU to block for a while, 
 * or driver startup routines to wait on timers in the context of the
 * idle process.  In addition, since we haven't given up our current PCB
 * (though it is now on the ready queue), we must prevent it from being
 * assigned to another CPU.  We do this by setting its affinity mask to
 * our own affinity mask.
 */
	if (cpip & who_mask) {
	    int sav_ipl;
	    int affinity;

	    pcb = getpcb();
	    affinity = pcb->pcb$l_affinity;
	    pcb->pcb$l_affinity = who_mask;
	    sav_ipl = spl_kernel.sav_ipl;
	    spl_kernel.sav_ipl = IPL_SYNC;
	    spinunlock(&spl_kernel);
	    krn$_micro_delay(1000);
	    spinlock(&spl_kernel);
	    spl_kernel.sav_ipl = sav_ipl;
	    pcb->pcb$l_affinity = affinity;
	} else {
	    sysfault();
	}
    }
}

/*+
 * ============================================================================
 * = show_version - Display the current firmware version                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Display current firmware version.                 
 *
 * FORM OF CALL:
 *
 *	show_version ()
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

/* do not add new platforms to this line; add show_version() to your platform.c */
#if MEDULLA || CORTEX || MTU || K2 || TAKARA || AVANTI || MIKASA || ALCOR || APC_PLATFORM
void show_version()
{
#if ALCOR
    struct HWRPB *hwrpb_ptr = (struct HWRPB *) PAL$HWRPB_BASE;
    struct DSRDB *dsrdb_ptr;
    dsrdb_ptr = (int)hwrpb_ptr + hwrpb_ptr->DSRDB_OFFSET[0];
#endif

#if MEDULLA
    extern char v_builder[];
    extern int dsrdb_number;
    extern  struct DSRDB_INFO dsrdb_n[];
    qprintf("%s Common Console %c%d.%d-%d, built on %s at %s\n",
		dsrdb_n[dsrdb_number].name, 
	        v_variant, v_major, v_minor, v_sequence, v_date, v_time);
#endif

#if CORTEX 
#if !YUKON
    extern char v_builder[];

    qprintf("AlphaVME 4/%d Common Console %c%d.%d-%d, built on %s at %s\n",
      krn$_timer_get_cycle_time(whoami())/1000000,  v_variant, v_major, v_minor, v_sequence, v_date, v_time);
#else
    qprintf("AlphaVME 5/%d Common Console %c%d.%d-%d, built on %s at %s\n",
      krn$_timer_get_cycle_time(whoami())/1000000,  v_variant, v_major, v_minor, v_sequence, v_date, v_time);
#endif
#endif

#if MTU
    extern char v_builder[];

    qprintf("%s Common Console %c%d.%d-%d, built on %s at %s\n",
      smm_name(), v_variant, v_major, v_minor, v_sequence, v_date, v_time);
#endif

#if K2
    extern char v_builder[];

    qprintf("\n\nDigital 21064A PICMG SBC Console %c%d.%d-%d, built on %s at %s\n",
      v_variant, v_major, v_minor, v_sequence, v_date, v_time);
#endif

#if TAKARA
    extern char v_builder[];

    qprintf("\nDigital 21164 PICMG SBC Console %c%d.%d-%d, built on %s at %s\n",
      v_variant, v_major, v_minor, v_sequence, v_date, v_time);
#endif

#if AVANTI
    extern char v_builder[];
/*	 
**
**  This is a three entry table because the values of sts can be 0, 1, or 2.
**  These values are stored at address 70 of memory by the srom.  The 0 value is
**  the value stored with the initial version of the srom which didn't save it
**  and the values 1 and 2 are store by the follow on SROMs which set up the sts
**  value at 70 properly.
**  
*/	 
    char    *model_version[] = {
	"A50",
	"A50",
	"A30"
    } ;

    qprintf("\n%c%d.%d-%d, built on %s at %s\n",
	v_variant,
	v_major,
	v_minor,
	v_sequence,
	v_date,
	v_time);
#endif

#if MIKASA
    extern char v_builder[];

    qprintf("\n%c%d.%d-%d, built on %s at %s\n", v_variant,
      v_major, v_minor, v_sequence, v_date, v_time);
#endif

#if ALCOR
    printf ("\n %s Console %c%d.%d-%d\n",
        (int *)((int)dsrdb_ptr + dsrdb_ptr->NAME_OFFSET[0] + 8),
        v_variant, v_major, v_minor, v_sequence);
    printf("        built on %s at %s\n", v_date, v_time);

#endif
#if  APC_PLATFORM
    struct EVNODE *evp, evnode;
    struct EVNODE *evp_ocp_text, evn_ocp_text;
    extern char v_builder[];
    struct HWRPB *hwrpb_ptr = (struct HWRPB *)PAL$HWRPB_BASE;
    struct DSRDB *dsrdb_ptr;
    int t;
    char ocp_string[64]; 

    dsrdb_ptr = (int)hwrpb_ptr + hwrpb_ptr->DSRDB_OFFSET[0];
    evp = &evnode;
    t = ev_read("oem_string", &evp, 0);
    if ((t == msg_success) && (strlen(evp->value.string) != 0)) {
	printf("\n%s Console %c%d.%d-%d, %s %s\n", 
		evp->value.string, 
     		v_variant, v_major, v_minor, v_sequence, v_date, v_time);
    }                           
    else {                      
     	printf("\n%s Console %c%d.%d-%d, %s %s\n", 
     		(int *)((int)dsrdb_ptr + dsrdb_ptr->NAME_OFFSET[0] + 8),
     		v_variant, v_major, v_minor, v_sequence, v_date, v_time);
    }                           
#if (PC264 && !WEBBRICK)
	evp_ocp_text = &evn_ocp_text;
	if (ev_read("ocp_text", &evp_ocp_text, 0) == msg_success) {
	    if (!strlen(evp_ocp_text->value.string)) {
     	        sprintf(ocp_string, "%s", "AlphaServer DS20");
     	        sable_ocp_write((struct FILE*)0, strlen(ocp_string), 1, ocp_string);
	    } else {                      
     	        sprintf(ocp_string, "%s", evp_ocp_text->value.string);
     	        sable_ocp_write((struct FILE*)0, strlen(ocp_string), 1, ocp_string);
	    } 
	} else 
	    err_printf("ERROR: could not read OCP_TEXT EV\n");
#endif                          


    {     /* Turn off the LED's for Goldrack */                          
       struct FILE *fp= fopen("iic_8574_ocp", "sr+");
       if (fp && primary())  {                 
          char buf=0xf;                        
          fwrite(&buf, 1, 1, fp);              
          fclose(fp);                          
       }                                       
    } 
#endif
}
#endif
/*+
 * ============================================================================
 * = reschedule - Schedule next process  	                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Save current process context and place it in the ready queue.
 *	Then load the next executable process, and swap context.
 *  
 * FORM OF CALL:
 *  
 *	reschedule (flag)
 *  
 * RETURNS:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 *	int flag - swap context if true
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int reschedule(int flag)
{
    struct PCB *pcb;

    spinlock(&spl_kernel);

    pcb = getpcb();

    pcb->pcb$l_pstate = KRN$K_READY;

/* If the quantum has expired, then put it on the blink, otherwise put in
 * on the flink.
 */
    if (pcb->pcb$l_qexpire < 0) {
	pcb->pcb$l_qexpire = pcb->pcb$l_quantum;
	insq(&pcb->pcb$a_flink, readyq[pcb->pcb$l_pri].blink);
    } else {
	insq(&pcb->pcb$a_flink, &readyq[pcb->pcb$l_pri].flink);
    }
    pcb = schedule(flag);
    return pcb;
}

typedef struct ASJD SJD;

int exdep_rsvdop_handler(int param)
{
    pprintf("alpha handler NYI\n");
    return 0;
}


/*
 *++
 *  FUNCTIONAL DESCRIPTION:
 *
 *      establish_setjmp
 *
 * This routine saves a setjmp environment for the current process.  It is
 * called by the setjmp routine, which provides a pointer to the block of
 * environment data to be copied into the setjmp queue for the process.
 * The routine searches the setjmp queue for an existing block with the
 * specified environment ID (env).  If it is found, the setjmp data
 * (pointed to by sjd) is copied into the block, overwriting the previous
 * data.  If it is not found, a new queue element is allocated, the setjmp
 * data is copied into the new block, and the element is inserted into the
 * queue.
 *
 *  FORMAL PARAMETERS:
 *
 *      jmp_buf env	- Identifies an environment number (integer) to be
 *			  associated with the saved setjmp block.
 *	SJD *sjd	- Pointer to setjmp data block to be copied to the
 *			  setjmp queue of the process.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE.
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE.
 *
 *  FUNCTION VALUE:
 *
 *      msg_success	- Success.
 *
 *  SIDE EFFECTS:
 *
 *      NONE.
 *
 *--
 */
int establish_setjmp(unsigned int env, struct ASJD *sjd)
{
    struct PCB *pcb;			/* PCB pointer */
    struct SJQ *sjqe;			/* setjmp queue element pointer */
    struct SJD *sjqe_sjd;		/* pointer to data portion of sjqe */

    if ((sjqe_sjd = find_setjmp(env)) != NULL)
					/* This setjmp already exists? */
      {
	memcpy(sjqe_sjd, sjd, sizeof(SJD));
					/* YES: overwrite existing setjmp */
    }					/* if */
      else				/* New setjmp */
      {
	sjqe = (void *) dyn$_malloc(sizeof(struct SJQ), DYN$K_SYNC);
					/* Allocate memory for the new setjmp
					 * block */
	sjqe->envid = env;		/* Copy environment ID to new block */
	memcpy(&sjqe->sjd, sjd, sizeof(SJD));
					/* Copy setjmp environment to new block
					 */

	pcb = getpcb();			/* Get current PCB and... */

	insq(sjqe, &pcb->pcb$r_sjq);	/* ... insert setjmp queue entry */
    }					/* else */
    return msg_success;
}

/*
 *++
 *  FUNCTIONAL DESCRIPTION:
 *
 *      find_setjmp
 *
 * This routine searches the setjmp queue of the current process for the
 * specified setjmp environment (env).  If found, a pointer to the data
 * section of the queue element is returned.  Otherwise, NULL is returned.
 *
 *  FORMAL PARAMETERS:
 *
 *      jmp_buf env	- Identifies the environment number (integer) of the
 *			  desired setjmp block.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE.
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE.
 *
 *  FUNCTION VALUE:
 *
 *      Pointer to setjmp data block or NULL if not found.
 *
 *  SIDE EFFECTS:
 *
 *      NONE.
 *
 *--
 */
int find_setjmp(unsigned int env)
{
    struct PCB *pcb;			/* PCB pointer */
    struct SJQ *sjqe;			/* setjmp queue element pointer */

    pcb = getpcb();			/* Get current PCB */

    sjqe = pcb->pcb$r_sjq.flink;	/* Point to first setjmp element (if
					 * any) */
    while (sjqe != &pcb->pcb$r_sjq)	/* Traverse the queue */
      {
	if (sjqe->envid == env)		/* Environment ID matches? */
	  {
	    return &sjqe->sjd;		/* YES: return address of data portion
					 */
	}				/* if */
	sjqe = sjqe->flink;		/* Point to next queue element */
    }					/* while */
    return NULL;
}

void console_exit()
{
    int (**load_adr)();
    int id;
    int i;
    int j;
    int ptr;
    int poll_affinity;
    int print;
    int boottype;
    int haltcode;
    int saved_print_debug_flag;
    UINT time_limit;
    UINT time_begin;
    struct impure *impure;
    struct HWRPB *hwrpb_ptr;
    struct SLOT *slot;
    struct PCB *pcb;
    struct PCB *poll_pcb;
    struct SEMAPHORE *sem;
#if RAWHIDE
    struct EVNODE *evp,evn;
    int err_log, halt_log;
#endif
#if TURBO
    int err_log, halt_log;
#endif


    halt_pending = 0;
    id = whoami();
    pcb = getpcb();
    time_limit = krn$_timer_get_cycle_time (id) / (HWRPB$_ICLK_FREQ / 2048);

/*
 * If the drivers are still running (they won't be for ^P/continue), we
 * need to stop our drivers cleanly.  This should stop interrupts from
 * happening.  (This is only done if no files are open from callbacks.)
 * 
 *
 * Also, get the console into polled mode before exiting.
 */
    if (primary() && (cb_ref == 0)) {
	stop_drivers();
	krn$_set_console_mode(POLLED_MODE);
    }

    impure =
      PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE + PAL$IMPURE_SPECIFIC_SIZE * id;

/* If we are the primary, flush any pending output.
 * Whenever leaving console, reestablish the UARTs to the mode the operating
 * system expects as indicated in the CTB.
 */
    if (primary()) {
	tt_flush();
	reestablish_CTB_mode();
    }

/*
 * Show that this node is no longer in console mode.  Elect a new  timer
 * CPU from the set of CPUs which are still in console mode --  if this is 
 * the empty set, don't change anything.
 * Note:  krn$_findpcb returns with the kernel spinlock taken out.
 */
    poll_pcb = krn$_findpcb(poll_pid);
    in_console &= ~(1 << id);
    if ((id == timer_cpu) && in_console) {
	i = in_console;
	j = 0;
	while (!((i >> j) & 1))
	    j++;
	timer_cpu = j;
#if TURBO

/* Change the affinitiy of the polling process so secondaries don't
 * call tt_poll on re-entry.
 */
	poll_pcb->pcb$l_affinity = 1 << j;
#endif
    }
    spinunlock(&spl_kernel);

#if SECURE

/* exiting the console is an implied log out */

    set_secure(1);
#endif

#if GALAXY
#if MODULAR
    if (galaxy_shared_adr) galaxy_set_run_bit (ON);
#else
    galaxy_set_run_bit (ON);
#endif
#endif

/* Tell PALcode to start the O/S.  When the O/S halts, we will return
 * here.  At that time, if the O/S wants a callback serviced, do it and
 * loop back to start the O/S again.  For any other kind of halt, we
 * continue inline, restarting some necessary things, and then return  to 
 * our caller (who will likely exit). */

#if AVANTI || MIKASA || K2 || MTU || TAKARA || RAWHIDE || APC_PLATFORM

/* IF restore state succeeds THEN go execute loaded code */

    if (restore_term_state(impure))
	execute_loaded_code(impure);

    save_term_state(impure);		/* save the terminal state */
#else

#if TURBO
    clear_node_halt();			/* Clear TLCNR:<NHALT> bit */
    SetHaltEn(ENABLE);			/* Enable node halts on this CPU */
    if ( primary() ) {
	SetPHaltEn(ENABLE);  /* Set up UART and enable ^P on the primary */
        SetRunLED (ON);			/* Turn on front panel RUN LED */
    }
#endif

LOOP: 	/*
	  We branch back here after the primary re-enters Console
	  and completes a callback 
	*/

#if TURBO
    if ( primary() )           		/* This is here to re-enable ^P on */
	CtlpSetHalt(ENABLE);            /* the primary after a callback */
#endif

    exit_console(CSERVE$START);		/* Dispatch... */

#if TURBO
    if ( primary() )     /* Disable ^P on the primary during a callback */
        CtlpSetHalt(DISABLE);
#endif

    mtpr_datfx(1);			/* re-enable DATFX */


    if (impure->cns$hlt[0] == hlt$c_callback) {
	cbip = 1;
	rscc (&time_begin);
	cb_time_limit = time_begin + time_limit;
	impure->cns$hlt[0] = 0;
	poll_affinity = poll_pcb->pcb$l_affinity;
	poll_pcb->pcb$l_affinity = 1 << id;
	mtpr_scbb(virt_to_phys(scb));
	if (console_mode[id] == INTERRUPT_MODE)
	    mtpr_ipl(IPL_RUN);
	fe_callback(impure);
	mtpr_ipl(IPL_SYNC);
#if GALAXY && TURBO
	/*
	 * This is needed for the case of Galaxy CPU Migration and console
	 * (terminal) callbacks happening simultaneously.  It's possible that
	 * the saved affinity corresponds to a migrated CPU which has restarted
	 * the OS while the current CPU was processing the callback.  Therefore,
	 * we need to assure that the restored poll process affinity is that
	 * of a CPU which is still available.
	 */
	spinlock(&spl_kernel);
	if (poll_affinity & in_console)
	    poll_pcb->pcb$l_affinity = poll_affinity;
	else
	    poll_pcb->pcb$l_affinity = 1<<timer_cpu;
	spinunlock(&spl_kernel);
#else
	poll_pcb->pcb$l_affinity = poll_affinity;
#endif
	cbip = 0;
	if (halt_pending)
	    impure->cns$hlt[0] = hlt$c_hw_halt;
	else
	    goto LOOP;
    }

#if TURBO
#define dump_entry_state (*(load_adr[1]))
    if (impure->cns$hlt[0] == hlt$c_fault_reset) {
	int value;
	if (!primary()) goto LOOP;
	if (read_nvr(NVR$K_DUMP_HW_STATE,&value) == msg_success) {
	    impure->cns$hlt[0] = 0;
	    if (value & 2) {
		spinlock(&spl_kernel);
		in_console |= (1 << whoami());
		spinunlock(&spl_kernel);
		load_adr = ovly_load("INFO");
		saved_print_debug_flag = print_debug_flag;
		print_debug_flag = 1;
		if (load_adr)  dump_entry_state(1);
		print_debug_flag = saved_print_debug_flag;
		ovly_remove("INFO");
		spinlock(&spl_kernel);
		in_console &= ~(1 << whoami());
		spinunlock(&spl_kernel);
	    }
	    if (!(value & 1)) {			/* halt if bit 0 is set */
		goto LOOP;
	    }
	}
	else goto LOOP;
    }
#endif

#endif

/*  Okay, the O/S has halted, so we restart some stuff so that we can  get 
 * to a prompt.
 */
    mtpr_mces(7);
    mtpr_scbb(virt_to_phys(scb));

#if GALAXY
#if MODULAR
    if (galaxy_shared_adr) galaxy_set_run_bit (OFF);
#else
    galaxy_set_run_bit (OFF);
#endif
#endif

#if TURBO
    clear_node_halt();          /* Clear TLCNR:<NHALT> bit */
    SetHaltEn(DISABLE);		/* Disable node halts on this CPU */
    if ( primary () ){
       SetPHaltEn(DISABLE); 	/* Disable ^P halts on the primary */
       SetRunLED(OFF);			/* Extinguish front panel RUN LED */
    }

    spinlock(&spl_entry);

    if (nvr_flag(NVR$K_XDELTA_ENTRY)) {
	pprintf("\nCPU %d: Console entry breakpoint, type ;P to proceed\n", whoami());
	xdelta();
	do_bpt();
    }
#endif

/* Say we've halted, and show who isn't yet halted.  Say we're back in
 * console mode.  If we are the primary, or if we are the first node  back
 * into console mode, we become the timer CPU as well.
 * Print messages if the primary is back in console mode, and we're not 
 * single stepping.
 */
    spinlock(&spl_kernel);
    in_console |= (1 << id);
    if ((in_console & (1 << primary_cpu)) && !stepping)
	print = 1;
    else
	print = 0;

/* don't print if this secondary is being halted by the primary */
    if (impure->cns$hlt[0] == hlt$c_hw_halt)
	if (!primary())
	    print = 0;

/* don't print if this is an lfu_start haltcode */
    if (impure->cns$hlt[0] == hlt$c_lfu_start)
	print = 0;

    if (print) {
        krn$_micro_delay(10000*id);

/*  Before printing, reset the TGA/VGA and keyboard so we can use them. */

      	if (primary() && graphics_console) {     
	    /* Drop the kernel spinlock.  The various init/reinit routines
	     * may need to acquire the spinlock themselves.  The only
	     * drawback to doing this (dropping the spinlock) is that the
	     * messages about which CPUs are still in console mode can be
	     * less accurate (i.e., the messages will reflect the CURRENT
	     * situation, rather than the situation at console entrance).
	     * But this is not a big deal.  We do not need to serialize
	     * the callers of the init/reinit routines below -- there is
	     * only ever one caller, the primary CPU.
	     */
	    spinunlock(&spl_kernel);
#if MODULAR
	    if (tga_shared_adr)
		tga_reinit(0);	/* Checks for TGA before doing anything. */
#if !TURBO
	    if (vga_shared_adr)
		vga_reinit(0);
#endif
	    if (kbd_shared_adr)
		kbd_reinit(0);
#else
#if  APC_PLATFORM && TGA
	    tga_reinit(0);	/* Checks for TGA before doing anything. */
#else
#if (TGA && !MTU)
	    tga_reinit(1);	/* Checks for TGA before doing anything. */
#endif                                           
#endif                                           
#if !MTU
	    vga_reinit(0);
#endif      
	    kbd_reinit(0);
#endif
	    spinlock(&spl_kernel);
	}
#if TURBO
	pprintf("\nCPU %d halted\n", id);
	haltcode = impure->cns$hlt[0];
	if (print) {
	    pprintf("  halt code = %d\n  ", haltcode);
	    if ((haltcode >= 1) && (haltcode <= 7))
		pprintf(hlt_table[haltcode - 1]);
	    pprintf("  PC = %-16x\n", *(INT *) impure->cns$exc_addr & ~3);
	}
	j = cpu_mask & ~in_console;
	for (i = 0; i < MAX_PROCESSOR_ID; i++)
	    if (j & (1 << i))
		pprintf("CPU %d is not halted\n", i);
#endif
#if !TURBO
	pprintf("\nhalted CPU %d\n", id);
	j = cpu_mask & ~in_console;
#if RAWHIDE
	j &= cpu_available;
#endif
	for (i = 0; i < MAX_PROCESSOR_ID; i++)
	    if (j & (1 << i))
		pprintf("CPU %d is not halted\n", i);
#endif
    }
    if ((id != timer_cpu) &&		/* we're not the timer cpu AND */
      ((id == primary_cpu) ||		/* we're the primary  OR */
       (timer_cpu != primary_cpu) ||	/* the primary isn't the timer CPU OR */
					/* the primary isn't in console mode */
       (((1 << primary_cpu) & in_console) == 0))) {
	timer_cpu = id;
#if TURBO
	poll_pcb->pcb$l_affinity = 1 << id;
#endif
    }

/*  Print out the Halt Code and PC. */

    haltcode = impure->cns$hlt[0];
#if MIKASA
#if !CORELLE
    glb_hlt=haltcode;
#endif
#endif

/* Conditional for Takara and K2 to detect server management faults on halt path */
#if K2 || TAKARA || EIGER
    check_srvmgr();
#endif

#if GALAXY
#ifdef HALT$C_PARTITIONED
    if (haltcode == HALT$C_PARTITIONED) {
	pprintf("\ncannot load a new console on a partitioned system\n");
	if (galaxy_node_number) {
	    pprintf("try again using partition 0\n");
	} else {
	    char buf[8];

	    pprintf("proceeding will cause the system to be unpartitioned automatically\n");
	    spinunlock(&spl_kernel);
	    read_with_prompt("\ndo you want to proceed?  [Y/(N)]  ", 8, buf, 0, 0, 1 );
	    if (toupper(buf[0]) == 'Y') {
		unpartition_system();
		io_drain_interrupts();
		exit_console(CSERVE$START);
	    } else {
		spinlock(&spl_kernel);
	    }
	}
    }
#endif
#endif

#if !TURBO
    if (print) {
        krn$_micro_delay(10000*id);
	pprintf("\nhalt code = %d\n", haltcode);
	if ((haltcode >= 1) && (haltcode <= 7))
	    pprintf(hlt_table[haltcode - 1]);
	pprintf("PC = %-16x\n", *(INT *) impure->cns$exc_addr & ~3);
    }
#endif
    spinunlock(&spl_kernel);

#if TURBO
    /*
     * Determine whether there's either an error log entry or a halt condition
     * to log to eeprom 
     */
    {
     int *i = &ERR_LOG_FLAG;
     halt_log = ((haltcode == hlt$c_dbl_mchk) || (haltcode == hlt$c_mchk_from_pal));
     err_log = ((i[2*id] & 1) || (i[2*id+1] >> 28));
    }

    /*
     * Do the EEPROM logging if called for
     */
    if ((haltcode >= hlt$c_hw_halt) && (haltcode <= hlt$c_mchk_from_pal)) {
	if (halt_log) {
	    ptr = malloc(sizeof(struct HALT_LOG));
	    read_halt_data(ptr);
	    if (ptr) {
		saved_print_debug_flag = print_debug_flag;
		print_debug_flag = 1;
		print_halt_data(ptr, 0, id);	/* print failing cpu machine check */
		print_halt_data(0, 0, id);	/* print other cpus machine checks */
		print_debug_flag = saved_print_debug_flag;
		eeprom_write_halt_data(ptr);	/* record it in the eeprom */
		free(ptr);
	    }
	}
	if (err_log)
	    eeprom_write_sym_log();
    }
#endif

/*  Load the halt fields of this processor's HWRPB slot. */

    if (hwrpb)
#if TURBO
	hwrpb_load_halt(hwrpb, impure, real_slot(id));
#else
#if !RHMIN
	hwrpb_load_halt(hwrpb, impure, id);
#endif
#endif

#if MEDULLA || CORTEX || YUKONA
	/* init our interrupt impure variables */
	int_disable(0xFFFFFFFF | 0xFFFFFFFF<<32);/* call PALcode to disable device */
	/* reenable NMI int, since palcode leaves it disabled
	   after a halt switch, which restarts the console */
	int_enable(0x1<<38);		/* call PALcode to enable device */
#endif

#if RAWHIDE
	/*
	 * Always run tt in polled mode
	 */
	tt_setmode(DDB$K_POLLED);
#endif

/*  If we are in interrupt mode, then unblock interrupts. */
    if (console_mode[id] == INTERRUPT_MODE) {

#if SABLE
	clear_cpu_errors(id);
#endif
	mtpr_ipl(IPL_RUN);
    }

#if RAWHIDE
    evp = &evn;
    ev_read( "os_type", &evp, 0);
    if (evp->value.string[0] == 'N') {
       /*
	* Determine whether there's either an error log entry or a halt
	* condition to log to eeprom 
	*/
    	{
     	int *i = &ERR_LOG_FLAG;
     	halt_log = ((haltcode == hlt$c_dbl_mchk) || (haltcode == hlt$c_mchk_from_pal));
     	err_log = ((i[2*id] & 1) || (i[2*id+1] >> 28));
    	}

       /*
     	* Do the EEPROM logging if called for
     	*/
    	if ((haltcode >= hlt$c_hw_halt) && (haltcode <= hlt$c_mchk_from_pal)) {
	    if (halt_log) {
	    	ptr = malloc(sizeof(struct HALT_LOG));
	    	read_halt_data(ptr);
	    	if (ptr) {
		    print_halt_data(ptr, 0, id);	/* print failing cpu machine check */
		    print_halt_data(0, 0, id);	/* print other cpus machine checks */
		    eeprom_write_halt_data(ptr);	/* record it in the eeprom */
		    free(ptr);
	    	}
	    }
	    if (err_log)
		eeprom_write_sym_log();
    	}
     } else {
        if ( ev_read( "dump_entry_state", &evp, 0 ) == msg_success ) {
	    if ((( haltcode >= hlt$c_ksp_inval ) && ( haltcode <= hlt$c_ptbr_inval ))
	        || (( haltcode >= hlt$c_dbl_mchk ) && ( haltcode <= hlt$c_mchk_from_pal )))
		    if ( load_adr = ovly_load("INFO")) {
		        dump_entry_state();
		        ovly_remove("INFO");
		    }
        } else {
    	    if (( haltcode == hlt$c_dbl_mchk ) || ( haltcode == hlt$c_mchk_from_pal )) {
	        pprintf("\nYour system has halted due to an irrecoverable error. Record the\n");
	        pprintf("error halt code and PC and contact your Digital Services representative.\n");
	        pprintf("In addition, type INFO ");
	        if ( haltcode == hlt$c_dbl_mchk )
		    pprintf("5");
	        else
		    pprintf("3");
	        pprintf(" and INFO 8 at the console and record the results.\n");
	    }
        }
    }
#endif

#if TURBO

    /*
     * IPL has to be low for this.   See ECO note 839.
     * dump the registers that Andy Padla is interested in.   During
     * debug, the user would bring up the console, deposit with a 
     * console command to dump_hw_state, and then next time console is
     * reentered, the dump of the entry state would be printed out.      
     * Also, if this is an error halt, display the data (djm 5-Mar-95).
     */
    if (primary()) {

      dump_hw_state = nvr_flag(NVR$K_DUMP_HW_STATE);

      if ((haltcode != hlt$c_hw_halt) && !err_log ) {
         if (dump_hw_state != 0) {
           load_adr = ovly_load("INFO");
           if (load_adr)  dump_entry_state(0);
           ovly_remove("INFO");
         }
      }
    }
  
#endif

/*  Restart the timer, just in case. */

    krn$_timebase_init();
    pcb->pcb$l_pri = 4;
    reschedule(1);

/*  Tell the entry process why we halted. */

#if TURBO
    spinunlock(&spl_entry);
    if (console_mode[id] == INTERRUPT_MODE)
	mtpr_ipl(IPL_RUN);
    if (print && (cpu_mask == in_console))
	eeprom_show_status();
#endif

    switch (impure->cns$hlt[0]) {
	case hlt$c_callback: 		/* CALLBACK_ENTRY */
	    break;

	case hlt$c_lfu_start: 		/* LFU START ENTRY */
	    lfu_start();
	    break;

	case hlt$c_hw_halt: 		/* EXTERNAL_ENTRY */
	    if (primary() && entry_boottype) {
		boottype = entry_boottype;
		entry_boottype = 0;

		sem = pcb->pcb$a_completion;
		pcb->pcb$a_completion = 0;
		krn$_create(boot_system, null_procedure, sem, 3, 1 << id, 
		    ENTRY_STACK, "entry", "tt", "r", "tt", "w", "tt", "w", 
		    boottype);
	    }
	    break;

	case hlt$c_reset: 		/* NODE_RESET_ENTRY */
	    break;

	default: 
	    sem = pcb->pcb$a_completion;
	    pcb->pcb$a_completion = 0;
	    krn$_create(system_reset_or_error, null_procedure, sem, 3, 1 << id,
	      ENTRY_STACK, "entry", "tt", "r", "tt", "w", "tt", "w", 1);
	    break;
    }
    impure->cns$hlt[0] = 0;
}

void console_exit_use_tt ()
{
    struct PCB *pcb;
    struct FILE *save_stdout;
    struct FILE *save_stderr;
    struct FILE *stdout;
    struct FILE *stderr;

    pcb = getpcb();
    save_stdout = pcb->pcb$a_stdout;
    save_stderr = pcb->pcb$a_stderr;
    pcb->pcb$a_stdout = fopen("tt", "w");
    pcb->pcb$a_stderr = fopen("tt", "w");
    console_exit();
#if UNIVERSE    
    read_universe_ev();
#endif
    fclose(pcb->pcb$a_stdout);
    fclose(pcb->pcb$a_stderr);
    pcb->pcb$a_stdout = save_stdout;
    pcb->pcb$a_stderr = save_stderr;
}

#if AVANTI || MIKASA || K2 || MTU || TAKARA || RAWHIDE || APC_PLATFORM

/*+
 * ============================================================================
 * = execute_loaded_code - execute code loaded in by console                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will execute the code loaded in by the console.
 *      This routine will also handle any console callback that needs to 
 *      come back into the console's context 
 *
 * FORM OF CALL:
 *
 *	execute_loaded_code(impure_ptr)
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      impure_ptr - pointer to impure area
 *
 * SIDE EFFECTS:
 *
 *      
 *                      
-*/
void execute_loaded_code(struct impure *impure_ptr)
{
    int code_running = 1;
    int i;
    int j;
    int id;

    do {

/* Tell PALcode to start the O/S.  When the O/S halts, we will return
 * here.  At that time, if the O/S wants a callback serviced, do it and
 * loop back to start the O/S again.  For any other kind of halt, we
 * continue inline, restarting some necessary things, and then return
 * to our caller (who will likely exit).
 */
	exit_console(CSERVE$START);

/* IF we reenter the console because of a callback THEN handle the
 * callback code and then exit the console (again) ELSE reenter console
 */
	if (impure_ptr->cns$hlt[0] == hlt$c_callback) {

/* set callback in progress flag */

	    cbip = 1;

	    impure_ptr->cns$hlt[0] = 0;
	    id = whoami();
#if TURBO
	    poll_affinity = poll_pcb->pcb$l_affinity;
	    poll_pcb->pcb$l_affinity = 1 << id;
#endif
	    mtpr_scbb(virt_to_phys(scb));
	    if (console_mode[id] == INTERRUPT_MODE)
		mtpr_ipl(IPL_RUN);
	    fe_callback(impure_ptr);
	    mtpr_ipl(IPL_SYNC);
#if TURBO
	    poll_pcb->pcb$l_affinity = poll_affinity;
#endif
	    cbip = 0;
	    if (halt_pending) {
		impure_ptr->cns$hlt[0] = hlt$c_hw_halt;
		code_running = 0;
		io_issue_eoi(0, 0x84); /* clear eio that got us here */
	    }
	} else {
	    code_running = 0;
	}
    } while (code_running);
}
;

/*+
 * ============================================================================
 * = save_term_state - Save terminal state                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will call an O/S supplied routine to save the 
 *      console terminal state.
 *
 * FORM OF CALL:
 *
 *	save_term_state(impure_ptr)
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      impure_ptr - pointer to impure area
 *
 * SIDE EFFECTS:
 *
 *      
 *
-*/
void save_term_state(struct impure *impure_ptr)
{
    struct impure *save_impure_ptr =
      (struct impure *) malloc(sizeof(struct impure));
    struct HWRPB *hwrpb_ptr = (struct HWRPB *) PAL$HWRPB_BASE;
    struct SLOT *slot;
    int saved_halt_request;
    int id;

    id = whoami();

    slot =
      (int) hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET + *hwrpb_ptr->SLOT_SIZE * id;

/* IF a save term routine is available AND This is the primary CPU
 * THEN go save the terminal state
 */
    if (primary() && *(INT *)hwrpb_ptr->SAVE_TERM)

      {
	pprintf("CP - SAVE_TERM routine to be called\n");

/* save current state of the halt request */

	saved_halt_request = slot->STATE.SLOT$V_HALT_REQUESTED;

/* save the current state of the impure area */

	memcpy(save_impure_ptr, impure_ptr, sizeof(struct impure));

/* Setup the impure area so the save_term routine will run */

	impure_ptr->cns$gpr[0][2 * 25] = 0;
	impure_ptr->cns$gpr[0][2 * 25 + 1] = 0;
	impure_ptr->cns$gpr[0][2 * 26] = 0;
	impure_ptr->cns$gpr[0][2 * 26 + 1] = 0;
	impure_ptr->cns$gpr[0][2 * 27] = hwrpb_ptr->SAVE_TERM_VALUE[0];
	impure_ptr->cns$gpr[0][2 * 27 + 1] = hwrpb_ptr->SAVE_TERM_VALUE[1];
	*(INT *)impure_ptr->cns$exc_addr = *(INT *)hwrpb_ptr->SAVE_TERM;

	write_ps_high_ipl(slot);

	execute_loaded_code(impure_ptr);

/* IF the halt requested field = 1 (save_term_exit) AND bit 63 = 0 in R0 to
 * signify successful execution THEN Set context valid and restore original
 * halt request ELSE Set context invalid ( this will prevent continuing per SRM)
 */
	pprintf("CP - SAVE_TERM exited with hlt_req = %x, r0 = %08x.%08x\n",
	  slot->STATE.SLOT$V_HALT_REQUESTED, impure_ptr->cns$gpr[0][2 * 0],
	  impure_ptr->cns$gpr[0][2 * 0 + 1]);

	if ((slot->STATE.SLOT$V_HALT_REQUESTED == 1) &&
	  (impure_ptr->cns$gpr[0][2 * 0 + 1] == 0)) {
	    slot->STATE.SLOT$V_CV = 1;
	    slot->STATE.SLOT$V_HALT_REQUESTED = saved_halt_request;
	} else {

/* invalidate context and prevent subsequent calls to save term
 * /restore term
 */
	    slot->STATE.SLOT$V_CV = 0;
	    *(INT *)hwrpb_ptr->SAVE_TERM = 0;
	    *(INT *)hwrpb_ptr->RESTORE_TERM = 0;
	}

/* restore original impure area */

	memcpy(impure_ptr, save_impure_ptr, sizeof(struct impure));

    }
    free(save_impure_ptr);
}

/*+
 * ============================================================================
 * = restore_term_state - Restore terminal state                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will call an O/S supplied routine to restore the 
 *      terminal state.
 *
 * FORM OF CALL:
 *
 *	restore_term_state(impure_ptr)
 *
 * RETURNS:
 *
 *      integer  1 on success
 *               0 on failure , this means that the code should not be
 *                continued
 *
 * ARGUMENTS:
 *
 *      impure_ptr - pointer to impure area
 *
 * SIDE EFFECTS:
 *
 *      
 *
-*/
int restore_term_state(struct impure *impure_ptr)
{
    struct impure *save_impure_ptr =
      (struct impure *) malloc(sizeof(struct impure));
    struct HWRPB *hwrpb_ptr = (struct HWRPB *) PAL$HWRPB_BASE;
    struct SLOT *slot;
    int saved_halt_request;
    int id;
    int stat = 1;			/* assume success */

    id = whoami();

    slot =
      (int) hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET + *hwrpb_ptr->SLOT_SIZE * id;

    /*  IF a Restore term routine is available AND     This is the primary CPU 
     * AND     THEN    go RESTORE the terminal state */
    if (primary() && *(INT *)hwrpb_ptr->RESTORE_TERM)

      {
	pprintf("CP - RESTORE_TERM routine to be called\n");

	/* save current halt request */
	saved_halt_request = slot->STATE.SLOT$V_HALT_REQUESTED;

	/* save current state of impure area */
	memcpy(save_impure_ptr, impure_ptr, sizeof(struct impure));

	/* setup impure area so restore term routine will be started */

	impure_ptr->cns$gpr[0][2 * 25] = 0;
	impure_ptr->cns$gpr[0][2 * 25 + 1] = 0;
	impure_ptr->cns$gpr[0][2 * 26] = 0;
	impure_ptr->cns$gpr[0][2 * 26 + 1] = 0;
	impure_ptr->cns$gpr[0][2 * 27] = hwrpb_ptr->RESTORE_TERM_VALUE[0];
	impure_ptr->cns$gpr[0][2 * 27 + 1] = hwrpb_ptr->RESTORE_TERM_VALUE[1];
	*(INT *)impure_ptr->cns$exc_addr = *(INT *)hwrpb_ptr->RESTORE_TERM;

	write_ps_high_ipl(slot);

	execute_loaded_code(impure_ptr);

	/*  IF the halt requested field = 1 (restore_term_exit)     AND     bit
	 * 63 = 1 in R0 to signify successful execution  THEN      Set context 
	 * valid and restore original halt request  ELSE      Set context
	 * invalid ( this will prevent continuing per SRM) */
	pprintf("CP - RESTORE_TERM exited with hlt_req = %x, r0 = %08x.%08x\n",
	  slot->STATE.SLOT$V_HALT_REQUESTED, impure_ptr->cns$gpr[0][2 * 0],
	  impure_ptr->cns$gpr[0][2 * 0 + 1]);

	if ((slot->STATE.SLOT$V_HALT_REQUESTED == 1) &&
	  (impure_ptr->cns$gpr[0][2 * 0 + 1] == 0)) {
	    slot->STATE.SLOT$V_CV = 1;
	    slot->STATE.SLOT$V_HALT_REQUESTED = saved_halt_request;
	} else {

	    /* invalidate context and prevent subsequent calls to save term
	     * /restore term */
	    *(INT *)hwrpb_ptr->SAVE_TERM = 0;
	    *(INT *)hwrpb_ptr->RESTORE_TERM = 0;
	    slot->STATE.SLOT$V_CV = 0;
	    stat = 0;			/* set failing status */
	}

	/* restore original impure area */
	memcpy(impure_ptr, save_impure_ptr, sizeof(struct impure));

    }
    free(save_impure_ptr);
    return (stat);
}
#endif

#if VALIDATE
/*+
 * ============================================================================
 * = krn$_consistency - verify consistency of kernel data structures          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine walks down various kernel data structures and verifies
 *	that they are consistent.
 *
 * FORM OF CALL:
 *
 *	krn$_consistency ()
 *
 * RETURNS:
 *
 *      0 - Success.
 *
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      System may hang if routine trips over poorly formed queues.
 *
-*/
void krn$_consistency()
{
    unsigned int heap_lower;
    unsigned int heap_upper;
    int i;

    extern struct QUEUE tqh;
    extern struct QUEUE pollq;
    extern struct QUEUE semq;
    extern int krn$_scount;

    /* we need the lock taken out for any kernel validation */
    spinlock(&spl_kernel);
    heap_lower = defzone.base;
    heap_upper = defzone.upper;

    /* process queue */
    queue_validate(&pcbq, &pcbq, heap_upper, krn$_pcount, krn$_pcount);

    /* dead pcb queue.  There must be at least one active process (us!) */
    queue_validate(&dead_pcbq, &dead_pcbq, heap_upper, 0, krn$_pcount - 1);

    /* timer queue */
    queue_validate(&tqh, &tqh, heap_upper, 0, krn$_pcount);

    /* semaphore queue */
    queue_validate(&semq, 0, heap_upper, krn$_scount, krn$_scount);

    /* poll queue */
    queue_validate(&pollq, &pollq, heap_upper, 0, krn$_pcount);

    /* ready queue */
    for (i = 0; i < MAX_PRI; i++) {

	queue_validate(&readyq[i].flink, &readyq[i], heap_upper, 0,
	  krn$_pcount);
    }

    /* Each queue on each semaphore */
    /* tbd */
    spinunlock(&spl_kernel);
}
#endif

/*+
 * ============================================================================
 * = unicheck - Verify that a process is eligible to run only on one processor =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Occasionally processes need to be eligible to run on only one processor,
 *	so that consistent results may be obtained.  Which processors is not
 *	important, as long as it is the same one.  An example of this is
 *	examine/deposit to hardware registers.  If the the process doing the
 *	examine/deposit were to move around as a result of scheduling, then
 *	inconsistent results would be obtained.
 *
 * FORM OF CALL:
 *
 *	unicheck (message)
 *
 * RETURN CODES:
 *
 *      msg_succes - eligible on only one processor
 *	msg_unichck - eligible for multiple cpus
 *
 * ARGUMENTS:
 *
 *      char *message - text string indicating from caller
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void unicheck(char *message)
{
#if SABLE || REGATTA || WILDFIRE
    struct HWRPB *hwrpb_ptr;
    struct SLOT *slot;
    struct PCB *pcb;
    int affinity;
    int active;
    int i;

    pcb = getpcb();
    affinity = pcb->pcb$l_affinity;
    if (affinity == -1)
	affinity = pcb->pcb$l_created_affinity;
    hwrpb_ptr = (struct HWRPB *) hwrpb;

    /* Walk down each slot in the HWRPB and see if that processor is running
     * the console, and if it is, if its bit in the affinity mask is set. */
    active = 0;
    slot = (void *) ((unsigned) hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET);
    for (i = 0; i < *hwrpb_ptr->NPROC; i++, slot++) {
	if (slot->STATE.SLOT$V_PA) {
	    if (affinity & (1 << i)) {
		active++;
	    }
	}
    }

    if (active > 1) {
	err_printf(msg_unicheck, message);
    }
#endif
}

/*+
 * ============================================================================
 * = reestablish_CTB_mode -  set uart to state indicated in CTB              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	reestablish_CTB_mode()
 *
 * RETURNS:
 *
 *      0 - Success.
 *
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *
-*/
void reestablish_CTB_mode()
{
    struct HWRPB *hwrpb_ptr;
    struct CTB_VT *ctb_vt;

    hwrpb_ptr = hwrpb;
#if TURBO
    if (console_mode[whoami()] != POLLED_MODE)
	return;
    ctb_vt = (int) hwrpb_ptr + *hwrpb_ptr->CTB_OFFSET;

    /*  For the VT and WS CTB types, the IE bit offsets are the same. */
    if ((*ctb_vt->CTB_VT$Q_TYPE == CTB$K_DZ_VT) ||
      (*ctb_vt->CTB_VT$Q_TYPE == CTB$K_DZ_WS)) {
	if (ctb_vt->CTB_VT$R_TX_INT.TX$V_IE)
	    console_ttpb->txon(console_ttpb->port);
	else
	    console_ttpb->txoff(console_ttpb->port);
	if (ctb_vt->CTB_VT$R_RX_INT.RX$V_IE)
	    console_ttpb->rxon(console_ttpb->port);
	else
	    console_ttpb->rxoff(console_ttpb->port);
    }
#endif
#if AVANTI || MIKASA || MONET || ALCOR || K2 || MTU || TAKARA || APC_PLATFORM || EIGER
    if (console_ttpb != &serial_ttpb)	/* must be graphics head */
	io_enable_interrupts(0,KEYBD_VECTOR);
#endif
}


/*+
 * ============================================================================
 * = set_all_pcb_ipls - set all PCB IPL's to given IPL		              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Set all PCB's to given IPL
 *
 * FORM OF CALL:
 *
 *	set_all_pcb_ipls(ipl)
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *	ipl - desired IPL
 *   
 * SIDE EFFECTS:
 *
 *
-*/
void set_all_pcb_ipls(int ipl)
{
    struct QUEUE *p;
    struct PCB *pcb;
    struct ALPHA_CTX *acp;

    spinlock(&spl_kernel);
    p = pcbq.flink;
    while (p != &pcbq) {
	pcb = (struct PCB *) ((int) p - offsetof(struct PCB, pcb$r_pq));
	acp = pcb->pcb$a_acp;
	if (acp) {
	    acp->acx$q_ps[0] = (acp->acx$q_ps[0] & 0xFFFF00FF) | (ipl << 8);
	    if (acp->acx$q_acp[0]) {
		pprintf ("nested exceptions for PCB %x (%s)\n", pcb, pcb->pcb$b_name);
	    }
	}
	p = p->flink;
    }
    spinunlock(&spl_kernel);
}


/*+
 * ============================================================================
 * = krn$_set_console_mode - set mode to interrupt or polled.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Set console mode to interrupt or polled. This
 *	routine is called only by the primary processor.
 *	
 *
 * FORM OF CALL:
 *
 *	krn$_set_console_mode ()
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *	target_mode - INTERRUPT_MODE or POLLED_MODE
 *
 *   
 * SIDE EFFECTS:
 *
 *
-*/
void krn$_set_console_mode(int target_mode)
{
    int id;
    int secondary_cpus;
    int i, j;
    int ipl;
    int tt_mode;
    int pri;
    UINT mces_data;
    UINT timeout;
    UINT begin;
    UINT current;
    UINT elapsed;
    struct PCB *pcb;
    struct SLOT *slot;
    struct impure *IMPURE;

    id = whoami();
    IMPURE = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
      (id * PAL$IMPURE_SPECIFIC_SIZE);

/* If the node is already running in the requested mode, then we dont need 
 * to do anything. */

    if (console_mode[id] == target_mode)
	return;

    /* Set timeout to 30 seconds. */
    timeout = (UINT)krn$_timer_get_cycle_time (id) * 30;

    if (id == primary_cpu) {

	/* Get the secondaries into console mode. */

	node_halt_secondaries();

	if (target_mode == POLLED_MODE) {
	    ipl = IPL_SYNC;
	    tt_mode = DDB$K_POLLED;
	} else {
	    ipl = IPL_RUN;
	    tt_mode = DDB$K_INTERRUPT;
	}

	/* Raise process priority to the highest possible. */
	pcb = getpcb();
	pri = pcb->pcb$l_pri;
	pcb->pcb$l_pri = MAX_PRI - 1;
	reschedule(1);

	/* Create this routine as a process for all secondaries. */
	secondary_cpus = (~(1 << id) & in_console);
	for (j = 0; j < MAX_PROCESSOR_ID; j++)
	    if ((secondary_cpus >> j) & 1)
		krn$_create(krn$_set_console_mode, 
					/* address of process           */
		  null_procedure, 	/* startup routine              */
		  0, 			/* completion semaphore         */
		  MAX_PRI - 1,		/* process priority             */
		  1 << j, 		/* cpu affinity mask            */
		  0, 			/* stack size                   */
		  "set_console_mode", 	/* process name                 */
		  0, 0, 0, 0, 0, 0, target_mode);

	/* If we are going to polled mode then wait for secondaries to  raise
	 * IPL. If we are going to interrupt mode then wait for the secondaries
	 * to change their mode. */

	krn$_readtime(&begin);
	for (j = 0; j < MAX_PROCESSOR_ID; j++) {
	    if ((secondary_cpus >> j) & 1)
		while (_BBCCI(j, &change_mode)) {
		    krn$_readtime(&current);
		    elapsed = current - begin;
		    if (elapsed > timeout) {
			pprintf("CPU %d unable to complete console mode transition 1\n", j);
			pprintf("CPU %d: begin = %d, end = %d, delta = %d\n", j, begin, current, elapsed);
			break;
		    }
		}
	}

	/*  Disable the I/O interrupts if going to polled mode. */
#if 0
	if (target_mode == POLLED_MODE)
	    dis_io_int();
#endif
	mtpr_ipl(IPL_SYNC);

#if TURBO
	/* Set TT mode to target mode */

	tt_setmode(tt_mode);
#endif

	/*  Change all processes to the target IPL level */

	set_all_pcb_ipls(ipl);

	if (target_mode == POLLED_MODE) {
	    console_mode[id] = POLLED_MODE;
#if RAWHIDE
	    /* Set TT mode to target mode */
	    tt_setmode(tt_mode);
#endif
	} else if (target_mode == INTERRUPT_MODE) {

/*
 * We need to clear MCES in the event we're dumping as the result of a system
 * machine check error. As a matter of fact, Rawhide and TurboLaser VMS error
 * routines do not clear MCES prior to bugchecking. Sable VMS does clear MCES.
 * 
 * Those platforms that reinit the VGA (and use BIOS emulator calls to do so)
 * will take MCHKs while MCES is set, thereby resulting in a double error halt
 * condition - and with invalid entry context which results in no crash dump..
 */
            mces_data = mfpr_mces();
            if ( (mces_data & 1) == 1 )
                mtpr_mces((mces_data & ~7) | 1);

#if TURBO || RAWHIDE
/*  Reset the IO subsystem */
/*
 * If the "boot_in_progress" bit is set during a callback open
 * then don't do the IO reset. This is a continuation of callback
 * booting and should speed things up. If BIP is clear, then do
 * the IO reset as this is probably a system dump.
 */
	    if (cbip) {
		slot = boot_cpu_slot;
		if (slot->STATE.SLOT$V_BIP == 0)
		    reset_io();
	    } else {
		reset_io();
	    }

#if !EV6
	    /* Clear pending interrupts */

	    *IMPURE->cns$hirr = 0;
	    *IMPURE->cns$hier = 0x80;
#endif

#endif
#if 0

	    /*  Enable the I/O interrupts: */

	    en_io_int();
#endif
#if RAWHIDE
	    /* Set TT mode to target mode */
	    tt_setmode(DDB$K_POLLED);
#endif
	    console_mode[id] = INTERRUPT_MODE;

	    reset_cpu(id);

	    mtpr_ipl(ipl);

	}

	/* Tell secondaries we're ready. */
	_BBSSI(id, &change_mode);

	/*  Wait for secondaries to finish. */

	krn$_readtime(&begin);
	for (j = 0; j < MAX_PROCESSOR_ID; j++) {
	    if ((secondary_cpus >> j) & 1)
		while (_BBCCI(j, &change_mode)) {
		    krn$_readtime(&current);
		    elapsed = current - begin;
		    if (elapsed > timeout) {
			pprintf("CPU %d unable to complete console mode transition 2\n", j);
			pprintf("CPU %d: begin = %d, end = %d, delta = %d\n", j, begin, current, elapsed);
			break;
		    }
		}
	}

	_BBCCI(id, &change_mode);

	pcb->pcb$l_pri = pri;
	reschedule(1);

    /* Secondaries come here, and only if the primary created this routine as a
     * process for the secondary. Secondaries should never call this routine
     * directly. */

    } else {

	if (target_mode == POLLED_MODE) {
	    mtpr_ipl(IPL_SYNC);
	} else {
	    console_mode[id] = INTERRUPT_MODE;
	}

	_BBSSI(id, &change_mode);

	/* Wait for primary to signal the first half of the transition is
	 * complete. */

	while (!(change_mode & (1 << primary_cpu))) {
	    krn$_micro_delay(1000);
	}

	if (target_mode == POLLED_MODE) {
	    console_mode[id] = POLLED_MODE;
	} else {

#if TURBO || RAWHIDE
#if !EV6
	    /*  Clear pending interrupts */

	    *IMPURE->cns$hirr = 0;
	    *IMPURE->cns$hier = 0x80;
#endif
#endif
	    reset_cpu(id);

            /* Clear MCES' MCHK bit on secondaries as well.. */

            mces_data = mfpr_mces();
            if ( (mces_data & 1) == 1 )
                mtpr_mces((mces_data & ~7) | 1);

	    mtpr_ipl(IPL_RUN);
	}

	/* Inform the primary that second half of transition is complete. */
	_BBSSI(id, &change_mode);

    }

}

