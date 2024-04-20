/* file:	bpt.c
 *
 * Copyright (C) 1993 by
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
 *      This module implements the console BPT command.
 *
 *  AUTHORS:
 *
 *      Console Firmware team
 *
 *  CREATION DATE:
 *  
 *      11-Dec-1993
 *
 *  MODIFICATION HISTORY:
 *
 *	jrk	 11-Dec-1993	Overlay integration
 *
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$inc:kernel_entry.h"

/*+
 * ============================================================================
 * = bpt - Invoke the console XDELTA debugger.                                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      The console XDELTA debugger is a superset of the VMS XDELTA
 *      debug utility. Although a command summary appears here, consult
 *      the  VMS DELTA/XDELTA Utility Manual  for a complete description
 *      of supported commands.
 *
 *      When the console bpt command is executed, control is passed to the
 *      console's XDELTA utility by executing a private XDELTA breakpoint.
 *      XDELTA then displays the current PC address and the instruction at
 *      that location (bpt address) and then awaits a command.
 *
 *      Once in XDELTA, it is possible to examine machine state and set
 *      breakpoints in a user program. Using ;P in this context returns
 *      control to the console. The CONTINUE command may then be used to
 *      run the user program. If an XDELTA breakpoint is encountered, the
 *      address and instruction are displayed and XDELTA awaits further
 *      commands. At this point normal XDELTA debugging may proceed,
 *      including single stepping and running the user program.
 *
 *   COMMAND FMT: bpt 0 N 0 bpt
 *
 *   COMMAND FORM:
 *
 *       bpt (
 *	    sub_commands:
 *
 *		;P ;C S O U V T
 *		[<ga>];G 
 *		[<ba>][,<bn>][,<da>][,<ca>];B
 *		[{B,W,L,Q,I,C,A}
 *		[<sa>][,<ea>]/ or [<sa>][,<ea>]! or [<sa>][,<ea>]<double_quote>
 *		CTRL/J, CTRL/[, CTRL/I, RETURN
 *		<ca>;E <a>,<n>;X
 *		pd_address,stradr;A
 *		pd_address,p0,p1,...;Z
 *		;D ;L ;O ;S ;W ;Y
 *		<expression>=  )
 *
 *   COMMAND TAG: bpt 0 RXBZ bpt
 *
 *   COMMAND ARGUMENT(S):
 *
 *      None
 *
 *   SUBCOMMAND OPTION(S):
 *
 *      ;P - Proceed from breakpoint.
 *
 *	;C - Set breakpoint at (R26) then ;P
 *
 *      [<ga>];G - Go. Begin execution at address 'ga'.
 *
 *      S - Step through an instruction.
 *
 *      O - Step over a subroutine call. The subroutine is executed.
 *
 *      U - Step until
 *
 *      V - Step until. No printing.
 *
 *      T - Trace
 *
 *      [<ba>][,<bn>][,<da>][,<ca>];B - Set a breakpoint, where:
 *#o
 *          o 'ba' is the breakpoint address,
 *          o 'bn' is the number of the breakpoint (2..8),
 *          o 'da' is the address of a location to display on breakpoint,
 *          o 'ca' is the address of acommand string to execute on breakpoint,
 *#
 *      [{B,W,L,Q} - Set the data display mode to one of either
 *          B(byte), W(word), L(longword), or Q(quadword).
 *
 *      <sa>[,<ea>]/ - Open location and display contents in prevailing mode,
 *          where:
 *#o
 *          o 'sa' is the start address of the data to be displayed.
 *            Note that a symbolic address, such as R0, PC, or F3, 
 *            may also be used.
 *          o 'ea' is the end address of a range of addresses beginning
 *            with address 'sa' to be displayed.
 *#
 *      <sa>[,<ea>]! - Open location and display contents in instruction mode.
 *
 *      <sa>[,<ea>]<double_quote> - Open location and display contents
 *          in ASCII mode.
 *
 *      CTRL/J (or LF) - Close current location, open and display
 *          next location.
 *
 *      CTRL/[ (or ESC) - Close current location, open and display
 *	    previous location.
 *
 *      CTRL/I (or TAB) - Close current location, open and display
 *          indirect location.
 *
 *      CTRL/M (or RETURN) - Close current location.
 *
 *      'string' - Deposit an ASCIZ string.
 *
 *      <ca>;E - Execute the command string at address 'ca'.
 *
 *      <a>,<n>;X - Load base register with address value, where:
 *#o
 *          o 'a' is the address to deposit in a base register,
 *	    o 'n' is the number of the base register (0..F).
 *#
 *      <expression>= - Display value of an expression using +,-,*,%,
 *		and @ operators.
 *
 *	Symbolic Names - The following symbolic names for Alpha registers
 *		are also supported.
 *#o
 *          o '.' - Current address.
 *          o PC - Program counter.
 *          o PS - Processor Status.
 *          o Rn - General purpose register 'n', 0 to 31.
 *          o Fn - Floating point register 'n', 0 to 31.
 *          o Xn - Base register 'n', 0 to F, for user data structures.
 *#
 *	Special Functions - The following special functions
 *		are also supported.
 *#o
 *          o ;A - Call Command Routine.  pd_address,stradr;A
 *	    o ;C - Put bpt at (r26), Do ;P
 *	    o ;D - Dynamic Display and Consistency Check
 *	    o ;L - Locate code symbol.  address;L
 *	    o ;O - Display Overlays
 *	    o ;R - Toggle Register display
 *	    o ;S - Display Processes
 *	    o ;W - Walk Stack
 *	    o ;Y - Why are we here
 *          o ;Z - Call Routine.  pd_address,p0,p1,...;Z
 *#
 *	Symbols - The following functions for symbols
 *		are also supported.
 *#o
 *          o |sym(op) - Do symbol lookup and do op
 *	    o |sym*    - display symbol(s)
 *	    o |sym;b   - Set bpt at code address of PD
 *#
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>bpt
 *
 *      Brk 0 at 000DE16C
 *
 *      000DE16C!BPT           (New IPL = 0) S
 *      000DE170!JMP         R31,(R26) S
 *      00066A70!STL         R0,07D4(R6) S
 *      00066A74!BIS         R31,#04,R25 ;P
 *      >>>bpt
 *
 *      Brk 0 at 000DE16C
 *
 *      000DE16C!BPT          [Q
 *      PC/00000000 000DE16C
 *      PS/10000000 00000000
 *      R0/00000000 00000000 CTRL/J
 *      R1/00000000 00000000
 *      FP0/00000000 00000000 CTRL/J
 *      FP1/00000000 00000000 CTRL/J
 *      FP2/00000000 00000000 CTRL/J
 *      FP1/00000000 00000000 CTRL/[
 *      FP0/00000000 00000000
 *      1000/00000000 00001000 [L
 *      ./00001000 ;P
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *
 *	None
 *
 * FORM OF CALL:
 *
 *      bpt ()
 *
 * RETURN CODES:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      A breakpoint instruction is executed. 
 *
 *	Note, this C header file is purely for documentation purposes.
 *	The following files are actually used to implement XDELTA.
 *#o
 *	o ALPHA_XDELTA.BLI - Console Alpha XDELTA.
 *	o INST_ALPHA.MAR - Define Alpha instructions
 *	o ALPHA_XDELTA_ISRS.MAR - XDELTA support interrupt service routines.
 *	o ALPHA_XDELTA.MAR - XDELTA support linkage routines.
 *	0 DECODE_ALPHA.C - Instruction decode and support routines.
 *#
 *
-*/
#if !MODULAR

void bpt()
{
    do_bpt();
}
#endif

#if MODULAR

void bpt()
{
/* setup xdelta */

    if (!xdelta_shared_adr) {
	xdelta_shared_adr = ovly_load("XDELTA");
	if (!xdelta_shared_adr) {
	    pprintf("Load of XDELTA failed\n");
	    return;
	}
	xdelta();
    }
    do_bpt();
}
#endif

