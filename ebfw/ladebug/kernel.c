/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: kernel.c,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $";
#endif

/*
 * $Log: kernel.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.44  1998/08/26  18:37:36  thulin
 * Add support for CLUBS
 *
 * Revision 1.43  1998/06/26  18:49:48  gries
 * return +4 on arith trap (print message)
 *
 * Revision 1.42  1997/12/15  20:45:51  pbell
 * Updated for dp264.
 *
 * Revision 1.41  1997/08/12  05:39:18  fdh
 * Modified to use the __EMULATE_FLOATING_POINT conditional.
 *
 * Revision 1.40  1997/08/01  19:15:42  pbell
 * Converted all address and register display routines that print quad words to
 * use the new PRINTF_sl_ macro for the size modifier in printf control strings.
 *
 * Revision 1.39  1997/06/10  18:19:22  fdh
 * Modified due to function name change.
 *
 * Revision 1.38  1997/06/02  04:36:17  fdh
 * Moved the initialization of "child_state" and "stopped"
 * into kinitdebugger(0.
 *
 * Revision 1.37  1997/04/24  21:13:24  pbell
 * Cleaned up warnings
 *
 * Revision 1.36  1997/03/12  11:02:19  fdh
 * Made optional pointer override offset to floating point registers.
 *
 * Revision 1.35  1997/02/26  21:46:02  fdh
 * Modified the printReg function to accept an optional argument
 * which points to a user specified register save state array.
 *
 * Modified ktrap to copy the emulated floating point registers
 * to the debugger's save state area before calling the debugger
 * as though a break point was encountered.
 *
 * Revision 1.34  1997/02/25  21:23:40  fdh
 * Added kinitdebugger and use the noDebugger runtime
 * conditional to disable only the features that impact
 * another debugger debugging the current Debug Monitor.
 *
 * Revision 1.33  1997/02/20  16:56:23  fdh
 * Modified to handle trap state saved on the stack rather
 * than in the breakpoint save state area.  This facilitates
 * debugging within the trap handling procedures.
 *
 * Added printReg() and changeReg() for reading or modifying
 * register state during debugging.
 *
 * Added conditions for using IEEE emulation code for performing
 * all floating point operations when the CPU's floating point unit
 * is disabled.
 *
 * Revision 1.32  1996/05/22  21:49:45  fdh
 * Modified tracing.
 * Reset HAE when a breakpoint is encountered.
 *
 * Revision 1.31  1996/01/26  22:32:52  cruz
 * Fixed bug which caused backward branches to fail with step command.
 *
 * Revision 1.30  1996/01/26  15:10:10  cruz
 * Fixed bug introduced with the next command which prevented
 * stepping into subroutines called with BSR.
 *
 * Revision 1.29  1996/01/26  00:02:48  cruz
 * Added the ability to step over bsr, jsr, and jump instructions.
 *
 * Revision 1.28  1995/12/15  14:55:34  cruz
 * Print arguments to ktrap as 64-bit values.
 *
 * Revision 1.27  1995/12/05  18:37:15  cruz
 * Save the instruction at the breakpoint address at the
 * time the breakpoints are installed (at "run" time) instead
 * of when the user specified the breakpoint.  This fixes a
 * problem which occurred when there were previous breakpoints
 * and a new image was loaded in the range of the breakpoints.
 * The instructions from the old code would be written into the
 * new code once the breakpoints were removed.
 *
 * Revision 1.26  1995/11/09  21:46:49  cruz
 * Changed type of DisStartAddress to "ul" instead of long.
 *
 * Revision 1.25  1995/10/30  16:59:26  cruz
 * Commented out routine(s) not in use.
 *
 * Revision 1.24  1995/10/27  14:24:34  cruz
 * Fixed up prototypes.
 *
 * Revision 1.23  1995/10/24  13:55:34  fdh
 * Moved some global definitions to a header file and
 * removed references to the obsoleted interrupt save
 * register array.
 *
 * Revision 1.22  1995/10/23  22:27:56  cruz
 * Included mon.h which was inadvertantly deleted in the last cleanup.
 *
 * Revision 1.21  1995/10/23  20:49:36  cruz
 * Performed some clean up.
 *
 * Revision 1.20  1995/10/22  05:11:10  fdh
 * Indluded mon.h.  Renamed user_main() to main_loop().
 * Other cleanup.
 *
 * Revision 1.19  1995/10/10  14:17:02  fdh
 * Renamed ladbx.h to ladebug.h.
 *
 * Revision 1.18  1995/08/30  23:03:33  fdh
 * Implemented kremoveallbreak().
 * Made restore_breakpoint_instrs() an external routine.
 *
 * Revision 1.17  1995/06/29  14:16:59  cruz
 * Added a few PRTRACE statements to help with debugging.
 *
 * Revision 1.16  1994/10/18  11:05:46  berent
 * Fixed single step to use ReadL and WriteL (instead of ReadQ
 * and WriteQ  when reading and writing instructions. Hence gets
 * rid of allignment error when single stepping a branch to an odd
 * location.
 *
 * Revision 1.15  1994/08/09  01:19:50  fdh
 * Fix data type used with the switch statement.
 *
 * Revision 1.14  1994/08/05  20:17:45  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.13  1994/06/29  16:16:22  berent
 * Added reporting of unexpected traps and exceptions.  Also
 * added   clearing of machine checks before the monior attempts
 * to continue from a machine check
 *
 * Revision 1.12  1994/06/28  20:12:18  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.11  1994/06/23  10:42:33  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.10  1994/06/17  19:36:43  fdh
 * Clean-up...
 *
 * Revision 1.9  1994/04/01  22:42:50  fdh
 * Modified data type for DisStartAddress.
 *
 * Revision 1.8  1994/03/09  12:48:33  berent
 * Made NT compilable and tidied up
 *
 * Revision 1.7  1994/01/19  11:05:37  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.6  1993/10/03  00:16:53  berent
 * Merge in development by Anthony Berent
 *
 *>> Revision 1.7  1993/10/01  15:45:48  berent
 *>> stop command handling modified so that it no longer needs to look at the
 *>> registers
 *>>
 *>> Revision 1.6  1993/08/09  11:41:15  berent
 *>> Changes for new ethernet interface
 *
 * Revision 1.5  1993/06/18  17:49:34  fdh
 * Properly cast value for DisStartAddress.
 *
 * Revision 1.4  1993/06/18  17:31:18  fdh
 * Point disassembler to current address when stopped at a breakpoint.
 *
 * Revision 1.3  1993/06/17  19:12:24  fdh
 * Added kprint_breakpoints() to be used from the
 * command line to print the current breakpoint
 * status.
 *
 * Revision 1.2  1993/06/08  22:32:06  berent
 * Changed to improve ladbx server communications
 *
 * Revision 1.1  1993/06/08  19:56:35  fdh
 * Initial revision
 *
 */


#include "ladebug.h"
#include "bptable.h"
#include "mon.h"
#include "paldata.h"
#include "palosf.h"

#if defined(DP264) || defined(CLUBS)
#define BREAKINST 0x00000080
#else
#define BREAKINST 0x000000AD
#endif
extern ul DisStartAddress;

#ifdef __EMULATE_FLOATING_POINT
extern int emulate_float(ui code,ul *p_temp);
extern register_value *EmulatedFloatingPointRegisters;
#endif

static char UnexpectedErrorString[] ="Unexpected trap or exception occured\n";

volatile int dbug_remotely;	/* TRUE when the ladebug remote server is enabled.
				    Otherwise, ladebug is used as a native debugger. */
volatile int stopped;           /* TRUE if the child process is halted (at a breakpoint or otherwise);
                                   volatile because it may be set from an interrupt */
int child_state;		/* State of child process */
address_value saved_user_pc;    /* Location in which user's PC is saved by the interrupt handler
                                   or poll ethernet routine */
int kdebug_save_ipl;

/* Forward declarations of internal functions */

static void kstepoverbreak(address_value breakpc,instruction_value code);
static void kwritetoistream(address_value address,instruction_value value);
static void (* breakpointfunc)(void);

/* 
 * The next 3 arrays allow the setting of two temporary breakpoints; these are used
 * for single stepping. Two are needed to avoid having to work out which way a conditional
 * branch is going to go.
 */
static instruction_value temp_breakpoint_instr[2];
static address_value temp_breakpoint_address[2];
static int temp_breakpoint_set[2];

/* 
 * register_ptr points to the current register save area; this may be either the breakpoint area
 * or the interrupt area depending how the code was entered.
 */
static register_value * register_ptr;

static register_value * kpc_ptr;

static int skipinstr;


/* kinitdebugger - Initializes the debugger facility */
void kinitdebugger(void)
{
  dbug_remotely = FALSE;

  /* point to the Breakpoint save state area */
  register_ptr = SavedIntegerRegisters;
  kpc_ptr = SavedExceptionAddressRegister;
	
  PRTRACE1("About to initialize debugger\n");
  /* Initialize the breakpoint table */
  bptinitialize();

  /* Initialize the PALcode callbacks */
  if (!noDebugger) kinitpalentry();

  /* Set up variables and enable interrupts */
  kstart();

  child_state = PROCESS_STATE_PROCESS_TERMINATED;
  PRTRACE1("kinitdebugger() child_state = PROCESS_STATE_PROCESS_TERMINATED\n");

  stopped = TRUE;
  PRTRACE2("kinitdebugger() stopped = %s\n", stopped ? "TRUE" : "FALSE");
}


/* kinitpalentry - called to initialise the callback address from PAL */
void kinitpalentry(void)
{
  /* Initialize the pal code breakpoint handling */
  kutilinitbreaks();
}


/* kinstall_breakpoints:
 *     This function inserts all the breakpoints into the debuggee's code before
 *     it runs.
 */
void kinstall_breakpoints(void)
{
    address_value returned_addr;
    instruction_value savedinst;
    int bpt_count = 0;

    /* Scan the breakpoint table */
    for(bpt_count = 0; bpt_count < BPTMAX; bpt_count++) {
	if(bptgetn(bpt_count,&returned_addr,&savedinst) == SUCCESS) {

            savedinst = ReadL(returned_addr);    /* read the instruction at breakpoint */
            bptmodify(returned_addr, savedinst); /* Save instruction at breakpoint */
	    /* Put in the breakpoint instruction */

	    PRTRACE2("Writing a breakpoint at %x\n", returned_addr);
	    kwritetoistream(returned_addr,BREAKINST);
	}
    }
}

/* kprint_breakpoints - print a list of the breakpoints */

void kprint_breakpoints(void)
{
    address_value returned_addr;
    instruction_value savedinst;
    int bpt_count = 0;

    for(bpt_count = 0; bpt_count < BPTMAX; bpt_count++) {
	if(bptgetn(bpt_count,&returned_addr,&savedinst) == SUCCESS) {
	    /* Put in the breakpoint instruction */
	    printf("{break} at ");
	    dis(3, returned_addr, returned_addr);
	}
    }
}

/* restore_breakpoint_instrs:
 *     This function replaces all breakpoints with the original instructions at those locations;
 *     it is called whenever the debuggee stops (at a breakpoint or otherwise) so that monitor
 *     commands accessing memory don't have to take special action at breakpoints.
 */
void restore_breakpoint_instrs(void)
{
    address_value returned_addr;
    instruction_value savedinst;
    int bpt_count;

    for(bpt_count = 0; bpt_count < BPTMAX; bpt_count++) {
	if(bptgetn(bpt_count,&returned_addr,&savedinst) == SUCCESS) {
	    kwritetoistream(returned_addr,savedinst);
	}
    }
}

/* kwaitforcontinue:
 *    called when the debuggee reaches a breakpoint or completes a step; 
 *    waits for a message telling the debuggee to continue.
 */
static void kwaitforcontinue(void)
{
    PRTRACE2("Waiting at %x\n",kpc());
    stopped = TRUE;
    PRTRACE2("kwaitforcontinue(void) entered.  stopped = %s\n", stopped ? "TRUE" : "FALSE");

    /* Disable interrupts; the application's interrupt state will be restored when its PS
     *  register is restored
     */

     swpipl(7);
  
    /* Restore the original contents of the instructions on which breakpoints were set */

    restore_breakpoint_instrs();

    /* Note both the conditions dbug_remotely & stopped are volatile variables
       that may be changed by the interrupt functions.  Therefore, they must be
       tested simultaneously */
    while(dbug_remotely && stopped) read_packets();

    PRTRACE1("kwaitforcontinue(void) done reading packets.\n");
    if (!dbug_remotely) {
      dis(3, kpc(), kpc());
      DisStartAddress = (unsigned long) kpc(); /* point dissassembler to current location */

      /* Loop at command line until told to go again */
      while(stopped == TRUE) {
	CommandExec();
      }
    }

    /* Set the new state */
    child_state = PROCESS_STATE_PROCESS_RUNNING;
    PRTRACE1("kwaitforcontinue(void) exiting. child_state = PROCESS_STATE_PROCESS_RUNNING\n");
}
    
/* katbpt - called from debuggee (via kreenter) when a breakpoint is reached */
static void katbpt(void)
{
    PRTRACE1("We have encountered a breakpoint\n");
    /* The PAL code entry point leaves the PC at the instruction following the breakpoint;
       so back it up */
    ksetpc(kpc() - 4);
    child_state = PROCESS_STATE_PROCESS_AT_BREAK;
    PRTRACE1("katbpt(void) child_state = PROCESS_STATE_PROCESS_AT_BREAK\n");
    kwaitforcontinue();
}

#ifdef __EMULATE_FLOATING_POINT
#define good_return 0
#define math_error -1
#define op_dec -2
#endif

/* ktrap - called when an unexpected trap or fault happens */
void ktrap(unsigned int type, ul arg0, ul arg1, ul arg2, register_value *arg3)
{
    int i;
    int BreakToDebugger = TRUE;	/* Break to debugger unless told otherwise */

#ifdef __EMULATE_FLOATING_POINT
    int emulate_float_status;
#endif

    register_value * save_register_ptr = register_ptr;
    register_value * save_kpc_ptr = kpc_ptr;

#ifdef PRTRACE_KTRAP
    outLed(0x99);
    PRTRACE1("entered ktrap");
#endif

    register_ptr = arg3; /* point to the current trap save state area */
    kpc_ptr = register_ptr+(32+32);

    /* Tell the user what has happened */
#ifdef PRTRACE_KTRAP
    printf("Unexpected trap or exception occured\n");
#endif
    switch(type) {
    case 1:
	/* Arithmetic exception */
	printf("Arithmetic exception; exception summary 0x%"PRINTF_sl_"x,"
	       " register mask 0x%"PRINTF_sl_"x\n", arg0, arg1);
	printf("instruction at %0x\n",kpc());
	BreakToDebugger = FALSE;
	break;
    case 2:
	/* Memory management fault*/
	printf("Memory management fault; virtual address = 0x%"PRINTF_sl_"x,\ntype = ", arg0);

	/* Arg1 gives the fault code */
	switch((int)arg1) {
	case MM_K_TNV:
	    printf("Translation not valid ");
	    break;
	case MM_K_ACV:
	    printf("Access violation ");
	    break;
	case MM_K_FOR:
	    printf("Fault on read ");
	    break;
	case MM_K_FOE:
	    printf("Fault on execute ");
	    break;
	case MM_K_FOW:
	    printf("Fault on write ");
	    break;
	default:
	    printf("Unknown MMCSR value 0x%"PRINTF_sl_"x ",arg1);
	}
	switch((int)arg2) {
	case -1:
	    printf("caused by fetch.\n");
	    break;
	case 0:
	    printf("caused by load.\n");
	    break;
	case 1:
	    printf("caused by store.\n");
	    break;
	default:
	    printf("caused by unknown cycle type 0x%x\n", arg2);
	}
	break;
    case 3:
	/* Instruction fault; arg0 gives type */
	switch((int)arg0) {
	case IF_K_BPT: 
	    printf("User breakpoint\n");
	    ksetpc(kpc() - 4);
	    break;
	case IF_K_BUGCHK:
	    printf("Bugcheck\n");
	    ksetpc(kpc() - 4);
	    break;
	case IF_K_GENTRAP:
	    printf("Gentrap PAL call\n");
	    ksetpc(kpc() - 4);
	    break;
	case IF_K_FEN:

#ifdef __EMULATE_FLOATING_POINT
	    while ((emulate_float_status = emulate_float(*(ui *)kpc(), arg3)) 
		== good_return)
	      ;

	    if (emulate_float_status == op_dec) {
	      BreakToDebugger = FALSE;
	      break;
	    }

	    if (emulate_float_status == math_error) {
	      /* Copy FP registers to the stack for the benefit of the debugger. */
	      memcpy(arg3, EmulatedFloatingPointRegisters, sizeof(register_value)*32);

	      printf("Floating point emulator signaled arithmetic exception, address = 0x%"PRINTF_sl_"x\n", kpc());
	      printf("exception summary 0x%"PRINTF_sl_"x, register mask 0x%"PRINTF_sl_"x\n", arg0, arg1);
	      break;
	    }
#endif /* __EMULATE_FLOATING_POINT */

	    printf("Floating point not enabled (FEN fault), address = 0x%"PRINTF_sl_"x\n", kpc());
	    break;
	case IF_K_OPCDEC:
	    ksetpc(kpc() - 4);

#ifdef __EMULATE_FLOATING_POINT
	    if ((emulate_float_status = emulate_float(*(ui *)kpc(), arg3)) == 0) {
	      BreakToDebugger = FALSE;
	      break;
	    }

	    if (emulate_float_status == math_error) {
	      /* Copy FP registers to the stack for the benefit of the debugger. */
	      memcpy(arg3, EmulatedFloatingPointRegisters, sizeof(register_value)*32);

	      printf("Floating point emulator signaled Math error, address = 0x%"PRINTF_sl_"x\n", kpc());
	      break;
	    }
#endif

	    printf("Reserved instruction (opDec) fault\n");
	    break;
	default:
	    printf("Unknown instruction fault\n");
	    ksetpc(kpc() - 4);
	}
	break;
    case 4:
	printf("Unaligned access: virtual address = 0x%"PRINTF_sl_"x, \n", arg0);
	printf("(6 bit) opcode is 0x%x, source or destination register is r%d\n", arg1, arg2);
	ksetpc(kpc() - 4);
	break;
    case 5:
	printf("System call; parameters: 0x%"PRINTF_sl_"x, 0x%"PRINTF_sl_"x, 0x%"PRINTF_sl_"x\n",arg0, arg1, arg2);
	ksetpc(kpc() - 4);
	break;
    default:
	printf("Bad type; type=0x%x ",type);
	printf("Parameters; 0x%"PRINTF_sl_"x, 0x%"PRINTF_sl_"x, 0x%"PRINTF_sl_"x\n", arg0, arg1, arg2);
    }

    register_ptr = arg3; /* point to the current trap save state area */
    kpc_ptr = register_ptr+(32+32);

    if (BreakToDebugger == TRUE) {
      Beep(100,1000);

      /* if there is a  temporary breakpoint (after stop or single step) remove it */
      for(i=0;i<2;i++) {
	if(temp_breakpoint_set[i]) {
	  PRTRACE2("Removing temporary breakpoint %d\n",i);
	  WriteL(temp_breakpoint_address[i],temp_breakpoint_instr[i]);
	}
	temp_breakpoint_set[i]=FALSE;
      }

      /* Now call the debugger as though this trap was a breakpoint */
      child_state = PROCESS_STATE_PROCESS_SUSPENDED;
      PRTRACE1("ktrap(void) child_state = PROCESS_STATE_PROCESS_SUSPENDED\n");
      kwaitforcontinue();

#ifdef __EMULATE_FLOATING_POINT
      /* Copy FP registers back to emulated register area. */
      memcpy(EmulatedFloatingPointRegisters, arg3, sizeof(register_value)*32);
#endif
    }

    register_ptr = save_register_ptr;
    kpc_ptr = save_kpc_ptr;
}

/* katstop - called from user program (via PAL code) when the code is stopped as a result of an interrupt */
static void katstop(void)
{
    PRTRACE1("We have encountered a breakpoint\n");
    /* The PAL code entry point leaves the PC at the instruction following the breakpoint;
       so back it up */
    ksetpc(kpc() - 4);
    child_state = PROCESS_STATE_PROCESS_SUSPENDED;
    PRTRACE1("katstop() child_state = PROCESS_STATE_PROCESS_SUSPENDED\n");
    kwaitforcontinue();
}

/* kload_implemented - check whether the kernel will load new processes.
 * 
 * Returns TRUE if this kernel supports the loading of new processes,
 * FALSE if not.
 */ 
int kload_implemented(void) { return FALSE;}


/* kload - load a new process.
 *
 * Arguments:
 *   name - file name of new process.
 *   argv - argument array for new process, NULL terminated.
 *   standardIn - file name of standard input.
 *   standardOut - file name of standard output.
 *   standardError - file name of standard error.
 *   loadAddress - address at which client expects process to be loaded; or 
 *                 all bits 1 if unknown.
 *   startAddress - address at which client expects process to start executing.
 *                  ignored if the load address is not set.
 *
 *   The format and interpretation of file name arguments is kernel dependent except
 *   in that an empty string always means use the default (if any).  The
 *   standard input, standard output and standard error file names may be ignored by
 *   some kernels.  Kernels will only use the load and start addresses if they have 
 *   direct control over where programs are loaded or started.
 *
 * Return Value:
 *   TRUE if successful, FALSE if not.
 */
int kload(char * name, 
	  char * argv[], 
	  char * standardIn, 
	  char * standardOut, 
	  char * standardError,
	  address_value loadAddress,
	  address_value startAddress)
{
    PRTRACE1("Load request received; not implemented\n");
    return FALSE;
}

/* kconnect_implemented - check whether the kernel will connect to existing processes.
 * 
 * Returns TRUE if this kernel supports connecting to existing  processes,
 * FALSE if not.
 */ 
int kconnect_implemented(void) {return TRUE;}

/* kconnect - connect to an existing process
 *
 * Argument:
 *    pid - process id of process to which the kernel is to be connected. The interpretation
 *          of this value is kernel dependent.  It may be ignored by kernels running
 *          in a non multiprocessing environment.
 *
 * Return Value:
 *   TRUE if successful, FALSE if not.
 */
int kconnect(ui pid) {
    /* nothing to do; the program is already loaded */
    return TRUE;
}

/* kkill_possible - checks whether this kernel can kill the current process.
 *
 * Return Value:
 *    TRUE if possible; false if not.
 */
int kkill_possible(void) { return FALSE; }

/* kkill - kill the current process.
 */
void kkill(void) {
    PRTRACE1("Kill request received; not implemented");
}

/* kdetach_possible - checks whether this kernel can detach from the current process
 *                    without killing it.
 *
 * Return Value:
 *    TRUE if possible; false if not.
 */
int kdetach_possible(void) { return TRUE; }

/* kdetach - detach from the current process without killing it. If possible the kernel will 
 *           not remove any breakpoints or continue the process if it is stopped ; but
 *           there may be kernels on which detaching can only be implemented such that
 *           it does one or both of these.
 */
void kdetach(void) {
    /* Nothing to do */
}

/* kpid - return the pid of the current process.
 */
ui kpid(void) {
    /* There is only one process on the system; always call it process 0 */
    return 0;
}

/* kreenter:
 *    called whenever the debugee executes a kernel breakpoint instruction;
 *    removes temporary breakpoints and calls the appropriate handler (either
 *    the real breapoint handler or the single step done handler).
 */
void kreenter(void)
{
    ui save_hae;
    int i;

    save_hae = SetHAE(0);	/* Put HAE into a known state. */

    PRTRACE2("entered kreenter, breakpointfunc = %x\n", breakpointfunc);
    register_ptr = SavedIntegerRegisters; /* point to the Breakpoint save state area */
    kpc_ptr = SavedExceptionAddressRegister;
    /* if there is a  temporary breakpoint (after stop or single step) remove it;
     * remove the second one first in case they both refer to the same address */
    for(i=1;i>=0;i--) {
	if(temp_breakpoint_set[i]) {
	    PRTRACE2("Removing temporary breakpoint %d\n",i);
	    kwritetoistream(temp_breakpoint_address[i],temp_breakpoint_instr[i]);
	}
	temp_breakpoint_set[i]=FALSE;
    }

    (*breakpointfunc)();

    SetHAE(save_hae);		/* Restore HAE value. */
}

/* krawgo - internal function to go without thinking about breakpoints */
static void krawgo(void)
{
    /* (Re)install the breakpoints */
    kinstall_breakpoints();

    /* Set what to do when the process stops */
    breakpointfunc = &katbpt;
    /* Clear stopped to allow the process to continue */
    child_state = PROCESS_STATE_PROCESS_RUNNING;
    PRTRACE1("krawgo(void) child_state = PROCESS_STATE_PROCESS_RUNNING\n");
    stopped=FALSE;
    PRTRACE2("krawgo(void) stopped = %s\n", stopped ? "TRUE" : "FALSE");
}    

/* kgo - run the current process until it hits a breakpoint or stops for some
 *       other reason.
 *      Called from ethernet interupt handler when a continue command is received
 */
void kgo(void)
{
    instruction_value code;

    /* If not stopped simply return */
    if(!stopped) return;

    /* If at a breakpoint do a single step first */
    if (bptisbreakat(kpc())) {
	PRTRACE2("Stepping over breakpoint at %x\n",kpc());
	bptgeta(kpc(), &code);
	kstepoverbreak(kpc(), code);
	/* process will continue on return from step */
    } else {
	krawgo();
    }
}

/* kwritetoistream - Modify instruction memory 
 *
 * Arguments:
 *      address - address to be written to;
 *      value   - 32 bit value to be written.
 */
static void kwritetoistream(address_value address,instruction_value value)
{
    /* Set the new value */
    WriteL(address,value);
    /* Ensure that the change takes effect by forcing an instruction barrier */
    imb();
    PRTRACE3("kwritetoiostream() %08x written to %x\n", value, address);
}

/* ksetstepbreak - Set temporary breakpoints so that the processor breaks after one instruction 
 *
 * Argument:
 *     addr - current address
 */
static void  ksetstepbreak(address_value addr)
{
    int instr;  /* Must be signed for branch displacements */
    ui reg;
    ui opcode;

    /* Extract instruction and OP code */
    instr = ReadL(addr);
    opcode = (instr >> 26) & 0x3F;   /* Must AND with 0x3f to kill possible sign-extension */
    if(opcode == 0x1A) {
        if (!skipinstr || (((instr >> 14) & 3) == 2)) {
	/* jump instruction; don't care what kind; all jump on rb register; */
	/* get the value of the register and set a breakpoint at that address */

	reg = (instr >> 16) & 0x1F; 
	temp_breakpoint_address[0]=(address_value) kregister(reg);
	temp_breakpoint_instr[0]= (instruction_value) ReadL(temp_breakpoint_address[0]);
	kwritetoistream(temp_breakpoint_address[0],BREAKINST);
	PRTRACE3("Single stepping jump instruction;\n\tbpt addr = %x, saved instr = %x\n",
	       temp_breakpoint_address[0],
	       temp_breakpoint_instr[0]);
	temp_breakpoint_set[0]=TRUE;
        return;
        }
    }

    if ((opcode & 0x30) == 0x30) {      /* If a branch instruction or bsr then */
        if ((opcode == 0x34) && skipinstr) { /* check to see if it's a BSR and */
            ;                           /* if we're stepping over it. */
        } else {
            /* If the top 2 bits of the instruction are set it is a branch instruction; */
            /* set breakpoints for both possible destinations */

            /* Set branch taken breakpoint */
            temp_breakpoint_address[0]= addr + 4 + (((instr << 11))>>9);
            temp_breakpoint_instr[0]=ReadL(temp_breakpoint_address[0]);
            kwritetoistream(temp_breakpoint_address[0],BREAKINST);
            temp_breakpoint_set[0]=TRUE;

            /* and branch not taken breakpoint */
            temp_breakpoint_address[1]= addr + 4;
            temp_breakpoint_instr[1]=ReadL(temp_breakpoint_address[1]);
            kwritetoistream(temp_breakpoint_address[1],BREAKINST);
            temp_breakpoint_set[1]=TRUE;
            PRTRACE1("Single stepping branch instruction;\n");
            PRTRACE3("\tbpt addr 0 = %x, saved instr = %x\n",
                     temp_breakpoint_address[0],
                     temp_breakpoint_instr[0]);
            PRTRACE3("\tbtp addr 1 = %x ,saved instr = %x\n",
                     temp_breakpoint_address[1],
                     temp_breakpoint_instr[1]);
            return;
        }
    }

    /* Anything else; simply set a breakpoint on the lexically next instruction */

    temp_breakpoint_address[0]= addr+4;
    temp_breakpoint_instr[0]=ReadL(temp_breakpoint_address[0]);
    kwritetoistream(temp_breakpoint_address[0],BREAKINST);
    temp_breakpoint_set[0]=TRUE;
    
    PRTRACE1("Single stepping linear instruction;\n");
    PRTRACE3("\tbpt addr = %x, saved instr = %x\n",
             temp_breakpoint_address[0],
             temp_breakpoint_instr[0]);
}

/* kstop - stop the current process as soon as possible. 
 *      Called from ethernet interupt handler when a stop command is received 
 */
void kstop(void)
{
    /* If already stopped do nothing */
    if(stopped) return;

    /* put a temporary breakpoint at the interrupt return address */
    temp_breakpoint_address[0]= saved_user_pc;
    temp_breakpoint_instr[0]=ReadL(temp_breakpoint_address[0]);
    kwritetoistream(temp_breakpoint_address[0],BREAKINST);
    temp_breakpoint_set[0]=TRUE;

    /* Treat process stop like reaching a breakpoint */
    breakpointfunc = &katstop;
}

/* kpoll - get the state of the current process.
 * Called from ethernet interupt handler when a poll command is received 
 *
 * Return value:
 *     PROCESS_STATE_PROCESS_RUNNING  - the process is running,
 *     PROCESS_STATE_PROCESS_AT_BREAK - the process has stopped at a breakpoint,
 *     PROCESS_STATE_PROCESS_SUSPENDED - the process has stopped elsewhere,
 *     PROCESS_STATE_PROCESS_TERMINATED - the process no longer exists,
 *     PROCESS_STATE_PROCESS_LOADING - the process is loading,
 *     PROCESS_STATE_LOAD_FAILED - the process failed to load.
 */
int kpoll(void)
{
    return child_state;
}

/* kaddressok - check whether an address is readable 
 *
 * Argument:
 *    address - the address to be checked.
 *
 * Return value:
 *    TRUE if readable, FALSE if not.
 */
int kaddressok(address_value address) {
    /* All alligned addresses are readable */
    return ((address % 8) == 0);
}

/* kcexamine - get a value from memory.
 * Called from ethernet interupt handler to examine an address in memory
 *
 * Argument:
 *    address - the address from which the value is to be fetched. Must be
 *              8 byte alligned.
 * 
 * Return value:
 *    The 8 byte value at address.  If there is a breakpoint within this 8 byte
 *    region the value returned is the value that would be at that address if the
 *    breakpoint were removed.
 */
ul kcexamine(address_value address)
{
    return  ReadQ(address);
}

/* kcdeposit - deposit a value in memory.
 * Called from ethernet interupt handler to store something at an address in memory 
 *
 * Arguments:
 *    address - the address at which the value is to be deposited. Must be
 *              8 byte alligned.
 *    value   - the 8 byte value to be deposited.  If there is a breakpoint within
 *              the 8 byte region the new value should not overwrite any breakpoint
 *              instruction; but instead should change the value that will be written
 *              back when any such instruction is removed.
 * 
 * Return value:
 *    TRUE if successful. FALSE if the kernel was unable to deposit the data.
 */
int kcdeposit(address_value address, ul value)
{
    instruction_value savedinst;

    /* Check if either half of the value overwrites a break point; if so change the break point's
       saved instruction */

    if(bptisbreakat(address)) {
	bptremove(address, &savedinst);
	/* Note that because AXPs are little endian the least significant bits are the new saved
	   instruction here */
	bptinsert(address, (instruction_value)(value & (ul)0x00000000FFFFFFFF));
    }
    if(bptisbreakat( address + sizeof(instruction_value))){
	bptremove(address + sizeof(instruction_value), &savedinst);
	bptinsert(address + sizeof(instruction_value), (instruction_value)((value & 0xFFFFFFFF00000000) >> 32));
    }
    /* Now write the new value */
    WriteQ(address,value);
    /* and put in an instruction barrier in case we have written to the instruction stream */
    imb();
    /* The deposit always succeeds */
    return TRUE;
}

/* krawstep - low level step function
 *
 * Argument:
 *     bptfunc - function to be called when the step completes.
 */
static void krawstep(void (* bptfunc)(void))
{
    /* Set a stepping breakpoint */
    ksetstepbreak(kpc());

    /* Set what we should do when the process stops */
    breakpointfunc = bptfunc;
    /* And let the process continue */
    child_state = PROCESS_STATE_PROCESS_RUNNING;
    PRTRACE1("krawstep() child_state = PROCESS_STATE_PROCESS_RUNNING\n");
    stopped = FALSE;
    PRTRACE2("krawstep(void) stopped = %s\n", stopped ? "TRUE" : "FALSE");
}

/* knext - steps over one instruction.  Useful for not following calls.
 */
void knext(void)
{
    PRTRACE2("Single stepping using next from pc %x\n",kpc());
    skipinstr = TRUE;
    krawstep(&katbpt);
    skipinstr = FALSE;
}

/* kstep - step one instruction.  If there is a breakpoint at the current program counter
 *          the instruction that would be at that address if the breakpoint were removed is
 *          executed.
 */
void kstep(void)
{
    PRTRACE2("Single stepping from pc %x\n",kpc());
    skipinstr = FALSE;
    krawstep(&katbpt);
    skipinstr = TRUE;
}

/* ksteppedoverbreak - called debuggee has had to step over a break point before continuing */
static void ksteppedoverbreak(void)
{
    PRTRACE1("Stepped over breakpoint \n");
    /* The PAL code entry point leaves the PC at the instruction following the breakpoint;
       so back it up */
    ksetpc(kpc() - 4);

    /* Continue by returning to the debuggee */
    PRTRACE1("Continuing after stepping over breakpoint\n");
    krawgo();
}

/* kstepoverbreak - called to step over a break point before continuing */	
static void kstepoverbreak(address_value breakpc, instruction_value code)
{
    PRTRACE1("Stepping over break \n");

    /* The application is stopped, so there are no breakpoints in the code; simply step */

    krawstep(&ksteppedoverbreak);                                           /* execute it */
}

/* kbreak - sets a breakpoint.
 *
 * Argument:
 *     address - the address at which a breakpoint is to be set. Must be 4 byte alligned.
 *
 * Return value:
 *     The result that should be sent back to the client.
 */
short int kbreak(address_value address)
{
    instruction_value savedinst;
    int s;
    
    PRTRACE2("Entered kbreak(%x)\n", address);
    
    if (!bptfull()) {

	savedinst = ReadL(address);
	
	/* Save just the instruction to be replaced; don't save the next
	   instruction since it may be modified before I remove the breakpoint */
	
	s = bptinsert(address, savedinst);
	
	if (s!=SUCCESS) PRTRACE2("Duplicate breakpoint %#010x\n", address);
	return REPLY_OK;
    }
    else {
	
	PRTRACE2("%#010x - breakpoint table full\n", address);
	return REPLY_NO_RESOURCES;
    }
}

/* kremovebreak - removes a breakpoint.
 *
 * Argument:
 *     address - the address of the breakpoint. Must be 4 byte aligned.
 *
 * Return value:
 *     The result that should be sent back to the client.
 */
short int kremovebreak(address_value address)
{
    instruction_value savedinst;
   
    /* Since the breakpoint instructions are not set up when the application is stopped simply remove the breakpoint
       from the table */

    if(bptremove(address, &savedinst) != SUCCESS) {
	PRTRACE1("Failed to remove the breakpoint\n");
	return REPLY_BAD_ADDRESS;
    }
    return REPLY_OK;
}

void kremoveallbreak(void)
{
  address_value returned_addr;
  instruction_value savedinst;
  int bpt_count = 0;

  for(bpt_count = 0; bpt_count < BPTMAX; bpt_count++) {
    if(bptgetn(bpt_count,&returned_addr,&savedinst) == SUCCESS) {
      kremovebreak(returned_addr);
    }
  }
}

/* kpc - get the current program counter.
 *
 * Return value:
 *    current program counter.
 */
address_value kpc(void)
{
  return (address_value) *kpc_ptr;
}

/* ksetpc - update the program counter.
 *
 * Argument:
 *    address - new value of program counter.
 */
void ksetpc(address_value newpc)
{
  *kpc_ptr = (register_value) newpc;
}  

/* kregister - get the contents of a register
 *
 * Argument:
 *    reg - the register number. If reg is in the range 0 to 31 the function fetches the
 *          contents of fixed point register reg.  If reg is the range 32 to 63 it
 *          fetches (as an 8 byte bit pattern) the contents of floating point register
 *          reg-32.
 *
 * Return value:
 *    The 8 byte bit pattern in the selected register.
 */
register_value kregister(int reg)
{
  return register_ptr[reg];
}

/* ksetreg - writes a bit pattern to a register
 *
 * Arguments:
 *     reg - the register number, as for kregister. 
 *     value - the 8 byte bit pattern to be written.
 */ 
void ksetreg(int reg, register_value value)
{
  register_ptr[reg] = value;
}

#ifdef NOT_IN_USE
/* knullipl - clear an ignored interrupt */
void knullipl(void)
{
#define CONTROL_PORT_82C59_M 0xA0
#define CONTROL_PORT_82C59_S 0x20

    /* clear the interrupt in the interrupt controller */
    outportb(CONTROL_PORT_82C59_M,0x20);
    outportb(CONTROL_PORT_82C59_S,0x20);
}    
#endif

/* kenableserver - enable the ladbx server and switch to remote debug mode */
void kenableserver(void)
{
  dbug_remotely = TRUE;

  /* Enable RTC interrupts so that messages can reach the debug server */
  kdebug_save_ipl = swpipl(7);
  swpipl(_MIN(kdebug_save_ipl,4));

  /* Initialise comms for ladebug */
  enable_ladbx_msg();

  if (child_state == PROCESS_STATE_PROCESS_TERMINATED) {
    /* If child process has never started, allow remote debugger to connect
       to a running process that can now be started up by the monitor or
       connect to the monitor process itself to debug it. */
    child_state = PROCESS_STATE_PROCESS_RUNNING;
    PRTRACE1("kenableserver(void) child_state = PROCESS_STATE_PROCESS_RUNNING\n");
  }
  else
    kwaitforcontinue();
}

/* kstart - called at startup to initialise debug */
void kstart(void)
{
    PRTRACE1("Initializing the debugger\n");

    /* Make sure interrupts are disabled */
    if (!noDebugger) swpipl(7);

    /* Initialize the state variables */
    stopped=FALSE;
    PRTRACE2("kstart() stopped = %s\n", stopped ? "TRUE" : "FALSE");
    child_state = PROCESS_STATE_PROCESS_RUNNING;
    PRTRACE1("kstart() child_state = PROCESS_STATE_PROCESS_RUNNING\n");
    temp_breakpoint_set[0] = FALSE;
    temp_breakpoint_set[1] = FALSE;
    /* Set up the break point function to treat any break point instruction as a real break point */
    breakpointfunc = &katbpt;
}  


#define GPREGGROUP  0
#define FPREGGROUP  1

/*
This routine reads the saved registers 
*/
void printReg(ul * reg_file, int group)
{
  int i,j,x=0;
  char *str;
  union registers {
    double dval;
    ul     qval;
  } *ptr;

  ptr = (union registers *) register_ptr;

  switch(group) {
  case FPREGGROUP:
    puts("\nFloating Point Registers\n");
    str = "f";
    ptr = ptr + 32;
    break;
  case GPREGGROUP:
    puts("\nGeneral Purpose Registers\n");
    str = "r";
  }

  /* reg_file overrides the default location */
  if (reg_file != NULL)
    ptr = (union registers *) reg_file;

  printf("register file @: %08x\n", ptr);

  for (i=0; i<32; i+=4) {
    printf("%s%02d: ", str, i);
    for (j=0; j<4; j++) {
      if (group == FPREGGROUP) {

#ifndef __NO_FLOATING_POINT
	printf("%10.5g ", ptr[x+i+j].dval);
#else
	printf("%016"PRINTF_sl_"x ", ptr[x+i+j].qval);
#endif /* __NO_FLOATING_POINT */

      }
      else
	printf("%016"PRINTF_sl_"x ", ptr[x+i+j].qval);
    }
    printf("\n");
  }

  printf("PC:  %016"PRINTF_sl_"x   ",  kpc());
  printf("PS:  %016"PRINTF_sl_"x\n\n", SavedProcessorStatusRegister);
}

void changeReg(ul * reg_file, int group, int index, ul data)
{
  if (group ==  FPREGGROUP)
    index += 32;

  ksetreg(index, data);
}
