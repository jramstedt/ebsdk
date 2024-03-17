/* 
 *  $Id: oldminid.s,v 1.1.1.1 1998/12/29 21:36:25 paradis Exp $
 *
 * This file implements the minidebugger commands.  It requires the file
 * serialio.s in order to communicate thru the SROM port.
 *
 *	Author: Rogelio R. Cruz, Digital Equipment Corporation
 *	Date: 6-Sept-1994
 *      Origins: Toni Camuso (Alcor)
 *
 * $Log: oldminid.s,v $
 * Revision 1.1.1.1  1998/12/29 21:36:25  paradis
 * Initial CVS checkin
 *
# Revision 1.1  1997/12/15  18:54:22  pbell
# New.
#
# Revision 1.32  1997/05/30  02:10:07  fdh
# Merged in support for the DC21164PC.
#
# Revision 1.31  1996/05/22  17:43:01  cruz
# Added definition of STALL in case it's not defined in
# the proper include files.
#
# Revision 1.30  1996/05/15  21:37:10  cruz
# Include sromio.s now that those routines have been taken
# out of serialio.s
#
# Revision 1.29  1996/05/06  21:22:24  cruz
# Made a change in serialio.s (v1.16) to increase the delay after sending
# out a character.  No changes were made to this file.
#
# Revision 1.28  1996/04/29  18:10:15  cruz
# Fixed bug in AddICFlush where I didn't update the
# destination address.
#
# Revision 1.27  1996/04/29  15:47:51  cruz
# Incremented number of stalls after ICFlush to
# guarantee that they cover more than one ICache
# block.
#
# Revision 1.26  1996/04/23  22:58:42  cruz
# No changes here, but I wanted to increment the version number
# so it reflects the new autobaud algorithm added to serialio.s
#
# Revision 1.25  1996/04/19  18:53:42  cruz
# Replaced interface call MC_IO_AUTO_BAUD with
# MC_IO_MINI_PROMPT which jumps to the SROM
# prompt without doing an auto-baud call.
#
# Revision 1.24  1996/04/18  21:09:42  cruz
# Commented out machine check entry point for non stand-alone version
#
# Revision 1.23  1996/04/11  22:48:56  cruz
# Made sure RestoreState saves R15 (the flags and baud rate) in pt13.
#
# Revision 1.22  1996/04/10  21:18:01  cruz
# Moved machine check handler closer to DumpIPRs to save
# branching to it.  Removed code that read excAddr and enabled
# PALmode.
# We no longer have the InitCPU code when the DEBUG swithc is
# enabled.  It saved several instructions.
#
# Revision 1.21  1996/04/09  23:59:13  cruz
# Changed the names of many of the interal cpu registers.
#
# Revision 1.20  1996/04/09  16:07:10  cruz
# Ignore wa and !d flags if we're handling a copy command.
# Changed flag name for fr and fw commands to match the
# command pneumonic more closely.
#
# Revision 1.19  1996/04/05  22:03:48  cruz
# Fixed the DepositData routine to get the correct behavior of the
# negate flag.
#
# Revision 1.18  1996/04/02  16:51:10  cruz
# Removed instructions that set the QW flag when loading
# an IPR since it's no longer needed with the new serial_io
# routines.
#
# Revision 1.17  1996/04/01  16:44:59  cruz
# Another major rewrite.  The common examine/deposit code
# was re-written to allow more flexible use of new commands.
# New commands include: mt - memory test, fr - follow with read,
# fw - follow with write, fl - print flags,  di - turn display
# on/off, qw - use qw stores on/off, ba - use base address on/off,
# sb - set base address, wa - use address when writing, !d - negate
# data when writing, lo - loop on/off
#
# Revision 1.16  1996/03/03  22:19:35  cruz
# Always autobaud when minidebugger is entered when using
# MC_IO_MINIDEBUGGER
#
# Revision 1.15  1996/03/01  23:56:36  cruz
# Added PRINT_BYTE to the serial I/O interface.
#
# Revision 1.14  1996/01/23  21:59:09  cruz
# Added two additional commands to turn on/off two new flags.
# Command "wa" sets/clears a flag which instructs all writes
# to use the destination address as the data to be written.
# Command "!d" sets/clears a flag which instructs all writes
# to negate the data to be written.  This can be useful when
# an alternating sequence is desired to measure voltage changes
# due to data lines being activated within the CPU.
# Also added was a small delay before waiting for the shift-U.
#
# Revision 1.13  1995/12/11  01:52:15  fdh
# Include serialio.s instead of serial_io.s
#
# Revision 1.12  1995/11/14  23:32:46  cruz
# Changed compile define from DEBUG_BOARD to DEBUG
#
# Revision 1.11  1995/10/06  21:26:29  cruz
# Fixed a PALcode violation for 21064 versions in the RT command.
# Removed some old debugging code.
#
# Revision 1.10  1995/05/19  20:01:28  cruz
# Rewrote the read/write routines to take up less space.
# Rewrote the preg command to take up less space.
# Added CP (copy) command.
# Changed some names of PVC labels.
#
# Revision 1.9  1995/05/04  21:37:00  cruz
# Fixed a problem which prevented the version number from
# being printed sometimes.
# Added the compare (CM) command.
#
# Revision 1.8  1995/04/19  23:57:37  cruz
# Changed the way the ST command was implemented from using
# HW_REI to just a simple RET.
# Added a few missing PVC labels.
#
# Revision 1.7  1995/04/19  21:35:36  cruz
# Changed IPR name in EC command.
#
# Revision 1.6  1995/04/19  21:18:12  cruz
# Put back the save and restore state code since it can
# still fit in an 8KB srom with it enabled.
#
# Revision 1.5  1995/04/19  19:55:00  cruz
# Changed xtbzap to xtbZap.
#
# Revision 1.4  1995/04/19  19:30:40  cruz
# Fixed "xb" command to restore state before jumping.
# Added conditionals to disable the new commands that take
# up alot of space.
#
# Revision 1.3  1995/04/18  23:02:45  cruz
# Corrected a couple of typos in 21064 versions.
#
# Revision 1.2  1995/04/14  15:28:08  cruz
# Set mc_m_xm flag in XM command.
# Print RCS version after Shift-U.
#
# Revision 1.1  1995/04/12  23:17:27  cruz
# Initial revision
#
 *
 */

// The following includes are already included by the new minidebugger source
// before it realizes that this source should be used
//#include "cpu.h"
//#include "ascitab.h"
//#include "mini_dbg.h"

#ifndef MAKEDEPEND
#include "mini_rcs.h"
#endif
 

/* ======================================================================
 * = 		ENTRY POINT FOR SROM MINI-DEBUGGER.			=
 * ======================================================================
 *
 *  FORM OF CALL:
 *       bsr r0, EntryPoint
 *
 *  ARGUMENTS: (Applicable only if DEBUG is defined)
 *	R27:R28 - Arguments to be passed along to internal routines.
 *	R29	- Code for routine to be called:
 *
 *		MC_IO_MINIDEBUGGER - Jumps to the minidebugger code.
 *		MC_IO_PRINT_STRING - Jumps to the print string routine.
 *		MC_IO_PRINT_LONG   - "                " longword routine.
 *		MC_IO_PRINT_CHAR   - "		      " character routine.
 *		MC_IO_READ_CHAR    - "		 " read character routine.
 *		MC_IO_SET_BAUD     - Sets the baud rate for transmission.
 *		MC_IO_MINI_PROMPT  - Jumps to minidebugger prompt (no auto baud)
 *		MC_IO_PRINT_BYTE   - "		      " byte routine.
 *		Any other value defaults to minidebugger code.
 *
 *	Note that ALL available PALtemps will be used to save the general
 *	purpose registers to make it possible to return to the calling
 *	code without worrying about overwritten registers.
 *	This code is free to use any register AS LONG AS it's saved in a
 *	PALtemp.
 *
 *
 *	REGISTER USAGE AND CONVENTIONS
 *
 *	R0	Function Return-Value Register. Use this register for returning
 *		values from a subroutine call or as a scratch register.
 *	R1-R3	Scratch registers.  Values are not preserved in a routine call. 
 *		Use these registers if more than one value needs to be returned
 *		from a function call.
 *		(Serial IO routines use R0-R3)
 *	R4-R9	Scratch registers.  Values are not preserved in a routine call.
 *		(Used in MiniDebugger code only).
 *	R10-R12	Global Registers.
 *		R12<63:32>  Holds the starting address of Xloaded code.
 *		R12<31:0>  Holds the starting address of ICache Flush code.
 *	R13	Global Register.
 *		Can't use this register in minidebugger since the minidbg
 *		needs to use PT13.
 *	R14	Global Register.
 *		The minidebugger uses this register as the base address for
 *		memory operations if the base_flag is set.
 *	R15	Local-Global reg.  Serves the function of a static variable.  
 *		Use this register to hold values that need to be "seen" from
 *		different points in the code.
 *		The minidebugger uses this register to hold the flag values 
 *		used by several commands.  Only the commands that affect the 
 *		flags need to change this register, others will just read it.
 *
 *	R16-R19 Argument Registers for use in passing values to routines. May
 *		be modified by called routine and/or used as return registers.
 *
 *	R20-R23 Return-Address Registers.  Use these registers only for holding
 *		the return address of subroutine call.
 *
 *
 *  PALTEMP REGISTER USAGE AND CONVENTIONS
 *
 *	PT0-PT23 Contents of R0-R23 respectively, with the exception of R13
 *		 which cannot be used because PT13 is needed by minidebugger 
 *		 for storage.
 *
 * ======================================================================
** = PVC Labels.  							=
** ======================================================================
**	Label					Last X
**	-----					------
**	pvc$serial_autosync_X$4010		100
**	pvc$serial_putchar_X$4001		101
**	pvc$serial_getchar_X$4002		100
**	pvc$serial_putx_X$4003			122
**	pvc$serial_getstring_X$4004		101
**	pvc$serial_getlong_X$4006		103
**	pvc$serial_putreg_X$4008		163
**	pvc$serial_keypressed_X$4009		103
**	pvc$mini_initcpu_X$5000			1
**	pvc$mini_initcpu_ret$5000.X		1
**	pvc$mini_getdaddr_X$5001		9
**	pvc$mini_getdaddr_ret$5001.X		2
**	pvc$mini_addicflush_X$5003		1
**	pvc$mini_addicflush_ret$5003.X		1
**	pvc$mini_trapkeypressed_X$5004		8
**	pvc$mini_trapkeypressed_ret$5004.X:	1
**	pvc$mini_entrypt_ret$5006.X:		1
**	pvc$initbr$5007 - pvc$initbr$5010
**	pvc$serial_putquad_X$4011:		102
**
 */
EntryPoint:

#if defined (DC21064) || defined (DC21064A) || defined (DC21066) || defined (DC21068) 
#ifndef DEBUG
	br	r31, mchk_end		/* Skip over machine check code. */
/* ======================================================================
 * = Machine Check Handler for stand alone case.			=
 * ======================================================================
*/
.= 0x20
mchk_start:
	br	r31, machine_check
mchk_end:
#endif /* ifndef DEBUG */
#endif /* DC21064 || DC21064A || DC21066 || DC21068 */

	SAVE_GPR_STATE()		/* Save R0-R12 & R14-R23 to PALtemps */

#ifdef DEBUG
	STALL
	STALL
	STALL
	mfpr	r15, pt13
	cmpeq	r29, MC_IO_PRINT_STRING, r0
	beq	r0, next1
	bis	r28, r28, r16
	bsr	r21, putString
	br	r31, RestoreState

next1:
	cmpeq	r29, MC_IO_PRINT_LONG, r0
	beq	r0, next2
	bis	r28, r28, r17
	bsr	r21, putLong
	br	r31, RestoreState

next2:
	cmpeq	r29, MC_IO_PRINT_CHAR, r0
	beq	r0, next3
	bis	r28, r28, r17
	bsr	r22, putChar
	br	r31, RestoreState
next3:
	cmpeq	r29, MC_IO_READ_CHAR, r0
	beq	r0, next4
	bsr	r22, getChar
	bis	r0, r0, r28	/* Save character to be returned in R28 */
	br	r31, RestoreState
next4:
	cmpeq	r29, MC_IO_SET_BAUD, r0
	beq	r0, next5
	bis	r28, r28, r0
	bsr	r21, set_baud
	br	r31, RestoreState
next5:
	cmpeq	r29, MC_IO_PRINT_BYTE, r0
	beq	r0, next6
	bis	r28, r28, r17
	bsr	r21, putByte
	br	r31, RestoreState

next6:
	cmpeq	r29, MC_IO_MINI_PROMPT, r0
	bne	r0, printPrompt
			/* Fall thru sromConsole */
#else

/*
 *  This loop is here to give the SROM mux a chance to switch before
 *  attempting the autobaud.  It becomes a problem in fast CPUs.
 *  It's only needed when the minidebugger is used as stand alone.
 */
	lda	r0, 3000(r31)
waitawhile:
	subq	r0, 1, r0
	bne	r0, waitawhile

pvc$mini_initcpu_1$5000:
       	bsr 	r23, InitCPU		/* Initialize the CPU.		*/
#endif /* DEBUG */

/*
 *  Perform baud detection
 */
sromConsole:
pvc$serial_autosync_100$4010:
	bsr	r21, autobaud_sync	/* do the autobaud thing	*/


PrintVersionID:				/* Keep version in sync with RCS id */
	LOAD_STR4(a_cr, a_cr,a_nl,a_V)
pvc$serial_putx_100$4003:
	bsr	r21, putString
	LDLI(r17, RCS_ENCODED_REV_L)
pvc$serial_putx_118$4003:
	bsr	r21, putLong

	lda	r15, mc_m_display(r15)	/* Display ON by default.	*/

printPrompt:
#define	mc_clears (mc_m_write | mc_m_block | mc_m_echo | mc_m_cp | mc_m_cmp | mc_m_mt)
	lda	r0, mc_clears(r31)
	bic	r15, r0, r15		/* clear out non sticky bits	*/
	lda	r15, mc_m_echo(r15)	/* Set echo back on.  Don't touch*/
					/* Print "<cr><lf>SROM> " 	*/
	LOAD_STR8(a_cr,a_nl,a_S,a_R,a_O,a_M,a_rangle,a_sp)
pvc$serial_putx_101$4003:
	bsr	r21, putString

pvc$serial_getstring_100$4004:
	bsr	r21, getString		/* r0 <- command string		*/
	beq	r0, endcase		/* short circuit		*/
chop:
	extbl	r0, 2, r1		/* Look at 3rd char (rt-to-lft) */
	beq	r1, continue		/* Find the first two chars.	*/
	srl	r0, 8, r0		/* Ignore 1 character.		*/
	br	r31, chop
continue:


/* ======================================================================
 * = CASE on the command string.					=
 * ======================================================================
 */
case_xm:
	/*--------------------------------------------------------------*
	 *  "XM" - External download into memory 			*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_X,a_M, case_em)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
pvc$mini_getdaddr_1$5001:
	bsr	r19, getAddress		/* R0 <- the load address 	*/
	bis	r0, r0, r7		/* R7 <- the load address 	*/
	bis	r7, r7, r11		/* R11 <- the load address 	*/
pvc$mini_getdaddr_2$5001:
	bsr	r19, getData		/* R0 <- the byte count 	*/
	addq	r0, r7, r12		/* R12 <- end address of transfer*/
xm10:					/* REPEAT 			*/
	bis	r31, r31, r9		/*  Clear the data field 	*/
xm20:					/*  REPEAT 			*/
pvc$serial_getchar_100$4002:
	bsr	r22, getChar		/*   Get a byte of data 	*/
	insbl	r0,  r7,  r0		/*   Put it into the right byte */
	bis	r0, r9, r9		/*   Put it into our data QW 	*/
	addq	r7,  1,  r7		/*   Update byte count 		*/
	and	r7, 7, r0		/*   Have we got a quadword?	*/
	bne	r0,  xm20		/*  UNTIL we have a QW.		*/

	stq_p	r9, -8(r7)		/*  Store the quadword 		*/
	ldq_p	r9, -8(r7)		/* : : Read it back  		*/

	cmpult	r7, r12, r0		/* UNTIL we have copied the 	*/
	bne	r0,  xm10		/* whole image.			*/
	mb				/* Make sure it gets out there	*/
	lda	r0, mc_m_xm(r31)
	bis	r15, r0, r15		/* XM has been executed.	*/
	sll	r11, 32, r11		/* Move it to the upper LW.	*/
	bis	r12, r11, r12		/* Combine with flush address	*/
	br	r31, endcase		/* ENDIF */

case_em:
	/*--------------------------------------------------------------*
	 *  "EM" - Examine a memory location				*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_E,a_M, case_bm)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	br	r31, common_ex_dep	/* Go to common examine code	*/

case_bm:
	/*--------------------------------------------------------------*
	 *  "BM" - Read a block of memory				*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_B,a_M, case_dm)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	lda	r15, mc_m_block(r15)	/* Set flag for block	 	*/
	br	r31, common_ex_dep	/* Go to common examine code	*/

case_dm:
	/*--------------------------------------------------------------*
	 *  "DM" - Deposit data to a memory location			*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_D,a_M, case_fm)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	lda	r15, mc_m_write(r15)	/* Flag a write.		*/
	br	r31, common_ex_dep	/* Go to common deposit code 	*/

case_fm:
	/*--------------------------------------------------------------*
	 *  "FM" - Fill an area of memory				*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_F,a_M, case_sb)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	lda	r15, (mc_m_block | mc_m_write)(r15) /* Set flags.	*/
	br	r31, common_ex_dep	/* Go to common deposit code.	*/

case_sb:
	/*--------------------------------------------------------------*
	 *  "SB" - Set the base address.				*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_S,a_B, case_rt)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
pvc$mini_getdaddr_3$5001:
	bsr	r19, getAddress		/* R0 <- the base address 	*/
	bis	r0, r0, r14		/* Save away the base address	*/
	bic	r15, mc_m_base, r15	/* Clear base flag. 		*/
	br	r31, PrintBase

case_rt:
	/*--------------------------------------------------------------*
	 *   "RT" - Return to calling program.				*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_R,a_T, case_dc)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/	
	lda	r1, (mc_m_base | mc_m_xm)(r31) /* These bits can't be	*/
	bic	r15, r1, r15		/* preserved across calls.	*/
RestoreState:
	mtpr	r15, pt13		/* Save current flags for later.*/
	STALL				/* Needed to prevent violation	*/
	STALL
	STALL
	RESTORE_GPR_STATE()		/* Get GPR regs from PALtemps.	*/
pvc$stop_checking_1$2001:
	jsr	r0, (r0)
pvc$stop_checking_2$2003.1:
	mfpr	r15, pt13		/* Restore current flags.	*/
	br	r31, endcase

case_dc:	
	/*--------------------------------------------------------------*
	 *  "DC" - Deposit data to a cpu registers			*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_D,a_C, case_ec)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	br	r31,  SetIPRs		/* Set IPRs.			*/

case_ec:	
	/*--------------------------------------------------------------*
	 *  "EC" - Examine data in cpu registers			*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_E,a_C, case_xb)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	br	r31,  DumpIPRs		/* Print IPRs.			*/

case_xb:
	/*--------------------------------------------------------------*
	 *  "XB" - Boot last image loaded with XM command.		*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_X,a_B, case_st)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	srl	r15, mc_v_xm, r1	/* Has an XM command been 	*/
	blbc	r1, endcase		/* executed? Abort if it hasn't.*/
pvc$mini_addicflush_100$5003:
	bsr	r23, AddICFlush		/* Add IC flush code to image.	*/

	mtpr	r12, pt0		/* Save the address code in R0.	*/
	br	r31, RestoreState	/* Restore & jump to xloaded image*/

case_st:
	/*--------------------------------------------------------------*
	 *  "ST" - Start execution at specified address			*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_S,a_T, case_pr)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
pvc$mini_getdaddr_4$5001:
	bsr	r19, getAddress		/* Get starting address in R0.	*/
	mtpr	r0, pt0			/* Write addr to jump to.	*/
	br	r31, RestoreState

case_pr:
	/*--------------------------------------------------------------*
	 *  "PR" - Print out general purpose cpu Registers.		*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_P,a_R, case_cm)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	br	r31, DumpGPRs

case_cm:
	/*--------------------------------------------------------------*
	 *  "CM" - Compare a block of memory.				*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_C,a_M, case_cp)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	lda	r15, (mc_m_block | mc_m_cmp)(r15)
	br	r31, common_ex_dep	/* Go to common deposit code	*/

case_cp:
	/*--------------------------------------------------------------*
	 *  "CP" - Copy a block of memory.				*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_C,a_P, case_mt)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	lda	r15, (mc_m_block | mc_m_cp)(r15)
	br	r31, common_ex_dep	/* Go to common deposit code	*/

case_mt:
	/*--------------------------------------------------------------*
	 *  "MT" - Simple memory test.					*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_M,a_T, case_fl)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	lda	r15, (mc_m_mt | mc_m_block | mc_m_write)(r15)
	br	r31, common_ex_dep	/* Go to common deposit code	*/

case_fl:
	/*--------------------------------------------------------------*
	 *  "FL" - Prints the current flag states.			*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_F,a_L, case_ba)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	lda	r4, 0x80(r31)		/* Print flag bits 7-0		*/
flags_loop:
	bsr	r23, PrintFlagState	/* Print the current flag state	*/
	srl	r4, 1, r4
	bne	r4, flags_loop
	br	r31, endcase

case_ba:
	/*--------------------------------------------------------------*
	 *  "BA" - Enables/Disables use of base address.		*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_B,a_A, case_qw)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
PrintBase:
	bis	r14, r14, r18		/* Copy to argument register.	*/
pvc$serial_putreg_100$4008:
	bsr	r23, putRegVal
	lda	r4, mc_m_base(r31)
	br	r31, ToggleFlagState

case_qw:
	bis	r31, r31, r4		/* Initialize flag register	*/
	/*--------------------------------------------------------------*
	 *  "QW" - Quad Word output mode flag				*
	 *--------------------------------------------------------------*/ 
	CASE_ON_FLAG(a_Q,a_W, mc_m_qw)

	/*--------------------------------------------------------------*
	 *  "DI" - Enable/Disable displaying of read data.		*
	 *--------------------------------------------------------------*/ 
	CASE_ON_FLAG(a_D,a_I, mc_m_display)

	/*--------------------------------------------------------------*
	 *  "LO" - Enable/Disable looping on read/write operations.	*
	 *--------------------------------------------------------------*/ 
	CASE_ON_FLAG(a_L,a_O, mc_m_loop)

	/*--------------------------------------------------------------*
	 *  "WA" - Use destination address as data in write commands.	*
	 *--------------------------------------------------------------*/ 
	CASE_ON_FLAG(a_W,a_A, mc_m_writeaddr)

	/*--------------------------------------------------------------*
	 *  "!D" - Negates data to be written by write commands.	*
	 *--------------------------------------------------------------*/ 
	CASE_ON_FLAG( a_exc,a_D, mc_m_notdata)

	/*--------------------------------------------------------------*
	 *  "FR" - Follow command with a read.				*
	 *--------------------------------------------------------------*/ 
	CASE_ON_FLAG(a_F,a_R, mc_m_follow_w_rd)

	/*--------------------------------------------------------------*
	 *  "FW" - Follow command with a write.				*
	 *--------------------------------------------------------------*/ 
	CASE_ON_FLAG(a_F,a_W, mc_m_follow_w_wr)
	beq	r4, bad_cmd		/* If no match found, then exit	*/

ToggleFlagState:
	xor	r15, r4, r15		/* Toggle this flag and print it*/
	bsr	r23, PrintFlagState	/* Prints the current state.	*/
endcase:
bad_cmd:
	br	r31, printPrompt


/* Dummy routine for PVC checking.  We never get here. */
pvc$stop_checking_3$2001.1:
pvc$stop_checking_4$2003:
	ret	r31, (r0)

#if defined(DC21164) || defined(DC21164PC)
#ifndef DEBUG
/* ======================================================================
 * = Machine Check Handler for stand alone case.			=
 * ======================================================================
*/
mchk_start:
.= 0x400
	br	r31, machine_check
#endif /* DEBUG */
#endif /* DC21164 || DC21164PC */


/*===========================================================================
 *=									    =
 *= common_ex_dep - common examine/deposit address routine		    =
 *=									    =
 *==========================================================================*/

common_ex_dep:
pvc$mini_getdaddr_5$5001:
	bsr	r19, getAddress		/* r0 <- start address		*/
	bis	r0, r0, r7		/* r7 <- start address 		*/
	bis	r0, r0, r8		/* r8 <- default end address 	*/

	srl	r15, mc_v_block, r0	/* Are we doing this command	*/
	blbc	r0, NotBlock		/* over a range of addresses?	*/
pvc$mini_getdaddr_6$5001:
	bsr	r19, getAddress		/* If so, then get the last	*/
	bis	r0, r0, r8		/* address of the block.	*/

NotBlock:
	lda	r0, (mc_m_follow_w_rd | mc_m_follow_w_wr | mc_m_cp | mc_m_cmp)(r31)
	and	r15, r0, r0		/* If we need to follow this	*/
	beq	r0, OneCmdOnly		/* command with another, then	*/
pvc$mini_getdaddr_9$5001:
	bsr	r19, getAddress		/* get the 2nd starting address */
	bis	r0, r0, r6

OneCmdOnly:
	srl	r15, mc_v_base, r0	/* If we are using a base addr	*/
	blbc	r0, NoBaseInUse		/* then make sure it's added to	*/
	addq	r14, r7, r7		/*  1) the start address	*/
	addq	r14, r8, r8		/*  2) the end address		*/
	addq	r14, r6, r6		/*  3) the 2nd start address	*/

NoBaseInUse:
	lda	r1, 3(r31)		/* Determine the right increment*/
	cmovlbs	r15, 7, r1		/* to use in read/write cmds.	*/
	bic	r6,  r1, r6		/*   Align to proper boundary	*/
	bic	r7,  r1, r7		/*   Align to proper boundary	*/
	bic	r8,  r1, r8		/*   Align to proper boundary	*/

GetData:
	srl	r15, mc_v_write, r0	/* If not writing, then don't	*/
	blbc	r0 , DontAskForData	/* ask for data.		*/
	srl	r15, mc_v_writeaddr, r0	/* If using the address as data	*/
	blbs	r0, DontAskForData	/* then don't ask for data.  	*/

pvc$mini_getdaddr_7$5001:
	bsr	r19, getData
	bis	r0, r0, r9		/* r9 <- data to write		*/

DontAskForData:
pvc$serial_keypressed_101$4009:
	bsr	r20, keypressed		/* Clear serial line interrupts */
					/* so we don't stop prematurely.*/

	/*--------------------------------------------------------------*
	 *  			Write Routines				*
	 *--------------------------------------------------------------*
 	 * Registers:							*
	 *	R6 - start address of 2nd cmd (saved in R11).		*
	 *	R7 - start address (saved in R5)			*
	 *	R8 - end address					*
	 *	R9 - data to write  (saved in R10)			*
	 *--------------------------------------------------------------*/ 
	bis	r7, r7, r5		/* Save R7 for looping.		*/
	bis	r6, r6, r11		/* Save R6 for looping.		*/
	bis	r9, r9, r10		/* Save for memory test.	*/

FirstWrite:
	bis	r7, r7, r18		/* Address to read from/write to*/

	srl	r15, mc_v_write, r1	/* Check if we're writing.	*/
	blbc	r1, FirstRead		/* If not, then goto read loop.	*/

	bsr	r22, DepositData	/* Data assumed to be in R9.	*/
	br	r31, SecondWrite

FirstRead:
	bsr	r22, ExamineData	/* Read data into R4.		*/
	bis	r0, r0, r9		/* Copy data read to R9.	*/

SecondWrite:
	bis	r6, r6, r18		/* Address to read from/write to.*/

	lda	r0, (mc_m_follow_w_wr | mc_m_cp)(r31)
	and	r15, r0, r0		/* Is it a copy cmd or do we	*/
					/* want 2nd write?  If so, then	*/
	srl	r15, mc_v_cmp, r1	/* do second write unless we're	*/
	cmovlbs r1, r31, r0		/* doing a compare command.	*/
	beq	r0, SecondRead
	bsr	r22, DepositData	/* Data assumed to be in R9.	*/
	bis	r7, r7, r18		/* If 2nd rd, use r7 for src.	*/
	srl	r15, mc_v_cp, r1	/* End of copy command.		*/
	blbs	r1, UpdatePtrs
	
SecondRead:
	lda	r1, (mc_m_follow_w_rd | mc_m_cmp)(r31)
	and	r15, r1, r1		/* Is it a compare cmd or we	*/
					/* need to do another read?	*/
	beq	r1, ReadDone		/* If no 2nd read, then skip	*/
	bsr	r22, ExamineData	/* Perform second read.		*/

ReadDone:
	srl	r15, mc_v_mt, r1	/* If not a memtest cmd, then	*/
	cmovlbc	r1, r0, r4		/* allow 2nd read to overwrite	*/
	blbc	r1, ZapSign		/* data in R4.			*/
	srl	r15, mc_v_write, r1	/* Is this a memtest in its write*/
	blbs	r1, UpdatePtrs		/* phase? If so, skip negation.	*/

MemTest:
	srl	r15, mc_v_writeaddr, r0	/* Fill with its own address?	*/
	cmovlbs r0, r7, r4		/* if so, then data = address.  */
	srl	r15, mc_v_notdata, r0	/* Use negated data?		*/
	blbc	r0, ZapSign
	ornot	r31, r4, r4		/* Negate the data to write.	*/

ZapSign:
	blbs	r15, NoZap		/* If QW mode, don't zap.	*/
	zap	r4, 0xF0, r4		/* Remove sign extension	*/
	zap	r9, 0xF0, r9		/* Remove sign extension	*/

NoZap:
	lda	r1, (mc_m_cmp | mc_m_mt)(r31)
	and	r15, r1, r1		/* If we're not doing a compare */
	beq	r1, DisplayCheck	/* or mem test cmd, then jump.	*/
	cmpeq	r9, r4, r0		/* See if they're equal.  If	*/
	bne	r0, UpdatePtrs		/* so, then skip print code.	*/
	br	r31, DisplayData	/* Ignore state of silent flag.	*/

DisplayCheck:
	srl	r15, mc_v_display, r0	/* Don't print if in silent mode*/
	blbc	r0, UpdatePtrs

DisplayData:
	srl	r15, mc_v_write, r0	/* Is this a write follow by read?*/
	blbs	r0, DisplayData2	/* If so, then don't print 1st.	*/

	bis	r7, r7, r18		/* Print source address and	*/
	bsr	r19, PrintAddrData	/* data of first read performed.*/

	srl	r15, mc_v_mt, r1	/* Is this a memory test cmd?	*/
	blbc	r1, DisplayData2	/* If not, skip mt statements.	*/

	LOAD_STR8(a_E,a_x,a_p,a_e,a_c,a_t,a_colon,a_sp)
	bsr	r21, putString
	bis	r4, r4, r18
	bsr	r19, PrintData
	br	r31, UpdatePtrs

DisplayData2:
	lda	r1, (mc_m_follow_w_rd | mc_m_cmp)(r31)
	and	r15, r1, r1		/* Is it a compare cmd or we	*/
					/* need to do another read?	*/
	beq	r1, UpdatePtrs		/* If not, then skip next print	*/
	bis	r6, r6, r18		/* Print source address and	*/
	srl	r15, mc_v_follow_w_wr, r1 /* Change source if follow with*/
	cmovlbs r1, r7, r18		/* a write before the read.	*/
	srl	r15, mc_v_cmp, r1 	/* Don't change source if it's	*/
	cmovlbs r1, r6, r18		/* a compare cmd.		*/

	bis	r4, r4, r9		/* data from second read.	*/
	bsr	r19, PrintAddrData

UpdatePtrs:
pvc$mini_trapkeypressed_4$5004:
	bsr	r22, trap_keypressed	/* Abort if a key was pressed.	*/

	lda	r1, 4(r31)		/* Determine the right increment*/
	cmovlbs	r15, 8, r1		/* to use in read/write cmds.	*/
	addq	r7, r1, r7		/* Increment destination ptr.	*/
	addq	r6, r1, r6		/* Increment 2nd destination	*/
	cmpult	r7, r8, r0		/* If we haven't reached the end*/
	bne	r0,  FirstWrite	 	/* of our block, then continue.	*/

	bis	r10, r10, r9		/* Restore starting addresses	*/
	bis	r5, r5, r7		/* and data so we loop if needed*/
	bis	r11, r11, r6
	bis	r10, r10, r4

	srl	r15, mc_v_mt, r0	/* If doing a memory test, then	*/
	blbc	r0, LoopCheck		/* handle things differently.	*/

	lda	r0, mc_m_write(r31)	/* Do we need to do the read now*/
	and	r15, r0, r1		/* Is this flag on?		*/
	xor	r15, r0, r15		/* Toggle write bit.		*/
	bne	r1, FirstWrite		/* Do the compare section of mt */
LoopCheck:
	srl	r15, mc_v_loop, r0	/* If in loop mode, then loop.	*/
	blbs	r0, FirstWrite
	br	r31, endcase


/*
**===========================================================================
** = DepositData - Writes data to specified destination(s)		    =
**===========================================================================
** OVERVIEW 
**	Writes data to specified destinations.
**
** FORM_OF_CALL:
**	bsr r22, DepositData
**  
** ARGUMENTS:
**	r18 - Address to write to.
**	r9 - Data to write.
**	r15 - flags
**
** RETURNS:
**	Nothing.  R9 may be changed.
**
**/
DepositData:
	srl	r15, mc_v_cp, r0	/* Don't change data pattern if	*/
	blbs	r0, DoDeposit		/* we're doing a copy command.	*/

	srl	r15, mc_v_writeaddr, r0	/* fill with its own address?	*/
	cmovlbs r0, r18, r9		/* if so, then data = address.  */

	ornot	r31, r9, r2		/* Negate the data to write.	*/
	srl	r15, mc_v_notdata, r0	/* Use negated data?		*/
	cmovlbs r0, r2, r9		/* if yes, then data = ~data. 	*/

DoDeposit:
	blbc	r15,  WriteLW		/* If LW operation, then do it.	*/
	stq_p	r9, 0(r18)		/* Write a QW			*/
	br	r31, WriteDone
WriteLW:
	stl_p	r9, 0(r18)		/* Write the LW.		*/

WriteDone:
	mb				/* Flush it out.		*/
	mb				/* An extra one to be sure.	*/
	ret	r31, (r22)


/*
**===========================================================================
** = ExamineData - Reads data from specified destination		    =
**===========================================================================
** OVERVIEW 
**	Reads data from specified destination.
**
** FORM_OF_CALL:
**	bsr r22, ExamineData
**  
** ARGUMENTS:
**	r18 - Address to read from.
**	r15 - flags
**
** RETURNS:
**	R0 - Data read.
**
**/
ExamineData:
	blbc	r15,  ReadLW		/* If LW operation, then do it.	*/
	ldq_p	r0, 0(r18)		/*  read the QW			*/
	ret	r31, (r22)
ReadLW:
	ldl_p	r0, 0(r18)		/* Read the LW			*/
	ret	r31, (r22)


/*
**===========================================================================
** = PrintAddrData - Prints a 64-bit address followed by a data LW or QW    =
**===========================================================================
** OVERVIEW 
**	Prints a 64-bit address followed by a data LW or QW.
**
** FORM_OF_CALL:
**	bsr r19, PrintAddrData
**  
** ARGUMENTS:
**	r18 - Address value
**	r9 - Data value
**	r15 - flags
**
** RETURNS:
**	Nothing
**
**/
PrintAddrData:
pvc$serial_putquad_100$4011:
	bsr	r23, putQuad
	LOAD_STR2(a_colon, a_sp)	/* send ': '			*/
pvc$serial_putx_119$4003:
	bsr	r21, putString
	bis	r9, r9, r18	
PrintData:
	blbc	r15,  PrintLW		/* Print LW or QW value read.	*/
pvc$serial_putreg_101$4008:
	bsr	r23, putRegVal		/*   Print value and cr+nl	*/
	br	r31, DonePrinting
PrintLW:
pvc$serial_putreg_102$4008:
	bsr	r23, putRegValL		/*   Print value and cr+nl	*/
DonePrinting:
	ret	r31, (r19)


/*
**===========================================================================
** = PrintFlagState - Prints the current state of the specified flag.	    =
**===========================================================================
** OVERVIEW 
**	Prints the current state (on/off) of the specified flag.
**
** FORM_OF_CALL:
**	bsr r23, PrintFlagState
**  
** ARGUMENTS:
**	r4 - Mask for flag to be printed out.
**
** RETURNS:
**	Nothing
**
**/
PrintFlagState:
	LOAD_STR2(a_Q, a_W)
	cmpeq	r4, mc_m_qw, r0		/* QW mode flag?		*/
	bne	r0, GotName			

	LOAD_STR4(a_D, a_i, a_s, a_p)	/* Display on/off flag?		*/
	cmpeq	r4, mc_m_display, r0	/* QW mode flag?		*/
	bne	r0, GotName			

	LOAD_STR4(a_L, a_o, a_o, a_p)	/* Loop on/off flag		*/
	cmpeq	r4, mc_m_loop, r0	/* QW mode flag?		*/
	bne	r0, GotName			

	LOAD_STR8(a_W, a_r, a_t, a_sp, a_A, a_d, a_d, a_r)
	cmpeq	r4, mc_m_writeaddr, r0	/* Write address flag?		*/
	bne	r0, GotName
			
	LOAD_STR8(a_N, a_e, a_g, a_sp,a_D, a_a, a_t, a_a)
	cmpeq	r4, mc_m_notdata, r0	/* Negate data flag?		*/
	bne	r0, GotName

	LOAD_STR8(a_B, a_a, a_s, a_e, a_A, a_d, a_d, a_r)
 	cmpeq	r4, mc_m_base, r0	/* Base address flag?		*/
	bne	r0, GotName

	LOAD_STR8(a_F, a_o, a_l, a_l, a_o, a_w, a_R, a_d)
 	cmpeq	r4, mc_m_follow_w_rd, r0 /* Read after flag?		*/
	bne	r0, GotName

	LOAD_STR8(a_F, a_o, a_l, a_l, a_o, a_w, a_W,a_r)
GotName:
pvc$serial_putx_122$4003:
	bsr	r21, putString		/* Print the flag name.		*/
	and	r15, r4, r1		/* Is this flag on?		*/
	LOAD_STR4(a_sp,a_O, a_F,a_F)	/* Load "OFF"			*/
	bis	r16, r16, r0		/* Copy to R0 for test.		*/
	LOAD_STR4(a_sp,a_O, a_N,a_nul) 	/* Load "ON"			*/
	cmoveq	r1, r0, r16		/* Use "OFF" string if it clear */
pvc$serial_putx_103$4003:
	bsr	r21, putString		/* Print "ON" or "OFF"		*/
	LOAD_STR2(a_cr, a_nl)		/* followed by carriage retrn.	*/
pvc$serial_putx_104$4003:
	bsr	r21, putString
	ret	r31, (r23)


/*
**===========================================================================
** = trap_keypressed - trap to console prompt if a key has been pressed	    =
**===========================================================================
** OVERVIEW 
**	This routine will trap to the endcase entry point if a key has been
**	pressed.
**	Returns a 0 if the key has not been pressed.
**
** FORM_OF_CALL:
**	bsr r22, trap_keypressed
**  
** RETURNS:
**	r0  - 1 if keypressed, else 0
**
** ARGUMENTS:
**	none
**
**/
trap_keypressed:
pvc$serial_keypressed_103$4009:
	bsr	r20, keypressed		/* r0 <- 1 if keypressed, 0 if not*/
	bne	r0, endcase		/* IF keypressed, trap to Prompt */
pvc$mini_trapkeypressed_ret$5004.1:
	ret	r31, (r22)		/* ELSE RETURN			*/

/*
** ===========================================================================
** = getData - get the data to be written from the user			     =
** = getAddress - get an address from the user				     =
** ===========================================================================
** FORM OF CALL:
**	bsr	r19, getData
**	bsr	r19, getAddress
**  
** RETURNS:
**	r0 - data input by user 
**
** CALLS:
**	getLong
**	putString
**
** REGISTERS:
**	r19 - Holds the return address.
**
**/
getData:				/* Load "D> " into arg reg.	*/
	LOAD_STR4(a_D, a_rangle, a_sp, a_nul)
	br	r31, print_prompt
getAddress:				/* Load "A> " into arg reg.	*/
	LOAD_STR4(a_A, a_rangle, a_sp, a_nul)
print_prompt:
pvc$serial_putx_110$4003:
	bsr	r21, putString		/* Print string.		*/
pvc$serial_getlong_103$4006:
	bsr	r23, getLong		/* R0 <- 64-bit address		*/ 
pvc$mini_getdaddr_ret$5001.2:
	ret	r31, (r19)

/*=======================================================================
 * = DumpGPRs - Prints out CPU GPR registers 				=
 *=======================================================================
 * OVERVIEW:
 *	Use this function to print out CPU GPR registers with their
 *	values at the time the minidebugger was called.
 *
 * FORM OF CALL:
 *       br	r31, DumpGPRs
 *
 * REGISTERS:
 *	Those used by putReg routine.
*/
DumpGPRs:
#ifdef FULL_MDBG 
	mfpr 	r18, pt0 		/* R0 */
pvc$mini_preg_0$4050:
	bsr	r19, print_reg_reset
	mfpr 	r18, pt1 		/* R1 */
pvc$mini_preg_1$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt2 		/* R2 */
pvc$mini_preg_2$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt3 		/* R3 */
pvc$mini_preg_3$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt4 		/* R4 */
pvc$mini_preg_4$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt5 		/* R5 */
pvc$mini_preg_5$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt6 		/* R6 */
pvc$mini_preg_6$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt7 		/* R7 */
pvc$mini_preg_7$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt8 		/* R8 */
pvc$mini_preg_8$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt9 		/* R9 */
pvc$mini_preg_9$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt10  		/* R10*/
pvc$mini_preg_10$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt11  		/* R11*/
pvc$mini_preg_11$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt12  		/* R12*/
pvc$mini_preg_12$4050:
	bsr	r19, print_reg_normal
	bis	r13, r13, r18		/* R13*/
pvc$mini_preg_13$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt14  		/* R14*/
pvc$mini_preg_14$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt15  		/* R15*/
pvc$mini_preg_15$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt16  		/* R16*/
pvc$mini_preg_16$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt17  		/* R17*/
pvc$mini_preg_17$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt18  		/* R18*/
pvc$mini_preg_18$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt19  		/* R19*/
pvc$mini_preg_19$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt20  		/* R20*/
pvc$mini_preg_20$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt21  		/* R21*/
pvc$mini_preg_21$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt22  		/* R22*/
pvc$mini_preg_22$4050:
	bsr	r19, print_reg_normal
	mfpr 	r18, pt23  		/* R23*/
pvc$mini_preg_23$4050:
	bsr	r19, print_reg_normal
	bis	r24, r24, r18		/* R24*/
pvc$mini_preg_24$4050:
	bsr	r19, print_reg_normal
	bis	r25, r25, r18		/* R25*/
pvc$mini_preg_25$4050:
	bsr	r19, print_reg_normal
	bis	r26, r26, r18		/* R26*/
pvc$mini_preg_26$4050:
	bsr	r19, print_reg_normal
	bis	r27, r27, r18		/* R27*/
pvc$mini_preg_27$4050:
	bsr	r19, print_reg_normal
	bis	r28, r28, r18		/* R28*/
pvc$mini_preg_28$4050:
	bsr	r19, print_reg_normal
	bis	r29, r29, r18		/* R29*/
pvc$mini_preg_29$4050:
	bsr	r19, print_reg_normal
	bis	r30, r30, r18		/* R30*/
pvc$mini_preg_30$4050:
	bsr	r19, print_reg_normal
	br	r31, endcase

/*=======================================================================
 * = print_reg_reset - Initializes the register counter and prints val. =
 * = print_reg_normal - Prints register number and its value.		=
 *=======================================================================
 *
 * ARGUMENTS:
 *	R18  - Value of register being printed.
*/
print_reg_reset:
	bis	r31, a_0, r4		/* Reset tens.			*/
	bis	r31, a_0, r5		/* Reset units.			*/

print_reg_normal:
	LOAD_STR4(a_R, a_nul, a_nul, a_colon)
	sll	r4, 8, r0		/* Tens place.			*/
	bis	r16, r0, r16		/* Add to string.		*/
	sll	r5, 16, r0		/* Units place.			*/
	bis	r16, r0, r16		/* Add to string.		*/	
pvc$serial_putreg_104$4008:
	bsr	r23, putReg		/* Print string and reg value.	*/
	cmpeq	r5, a_9, r1		/* Have we reached the max digit?*/
	cmovne	r1, a_slash, r5		/* Reset units (will be incr.)	*/
	addq	r5, 1, r5		/* Increment units		*/
	addq	r4, r1, r4		/* Increment tens (if necessary)*/
pvc$mini_preg_ret$4050.1:
	ret	r31, (r19)
#else
	br	r31, endcase
#endif /* FULL_MDBG  */

/************************************************************************
 *		ALL CPU DEPENDENT CODE BELOW				*
 ************************************************************************/

#ifndef DEBUG
/*=======================================================================
 * = InitCPU - Minimum CPU initialization.				=
 *=======================================================================
 * OVERVIEW:
 *	Initializes the CPU.
 *
 * FORM OF CALL:
 *       bsr r23, InitCPU
 *
 * ARGUMENTS:
 *	None
 *
 * RETURNS:
 *	R0  - Address of last instruction of code added.
 *
 * REGISTERS:
 *	R1 - NOP instruction counter.
 *	R2 - Instruction to be added.
*/
InitCPU:

#if defined(DC21164) || defined(DC21164PC)
	mtpr	r31, dcFlush		/* Flush the dcache */

#define ICSR_VAL ( (1 << (ICSR_V_TMD-16)) | (1 << (ICSR_V_FPE-16)) | (1 << (ICSR_V_HWE-16)) | \
		   (1 << (ICSR_V_SDE-16)) | (1 << (ICSR_V_SLE-16)) | (1 << (ICSR_V_TST-16)))
	LDLI(r2, ICSR_VAL)		/* 0x82.4E00.0000		*/
	sll	r2, 16, r2
	mtpr	r2, icsr

	mtpr	r31, dcMode		/* Disable DCache.		*/
	mtpr	r31, ips		/* Initialize PS to kernel mode.*/
	mtpr	r31, itbIa		/* clear the ITB 		*/
	mtpr	r31, dtbIa		/* clear the DTB 		*/
	mtpr	r31, dtbCm		/* set new ps<cm>=0, Mbox copy 	*/
	mtpr	r31, dtbAsn		/* ASNs */
	mtpr	r31, itbAsn
	mtpr	r31, aster		/* stop ASTs (enable)  		*/
	mtpr	r31, astrr		/* stop ASTs (request) interrupt*/
	mtpr	r31, sirr		/* clear software interrupts 	*/
	mtpr	r31, excSum		/* clear out exeception summary */
					/* and exc_mask 		*/
	mfpr	r31, va			/* unlock faulting va, mmstat 	*/

#define icp$stat_init (ICPERR_M_DPE | ICPERR_M_TPE  | ICPERR_M_TMR)
	lda	r2, icp$stat_init(r31)	/* Clear Icache data and tag 	*/
	mtpr	r2, icPerr		/* parity error, & timeout error*/

#define dcp$stat_init (DCPERR_M_LOCK | DCPERR_M_SEO)
	lda	r2, dcp$stat_init(r31)	/* Clear Dcache parity error 	*/
	mtpr	r2, dcPerr		/* status		 	*/

/* ======================================================================
 * = Initialize the pal_base register to where this code is loaded	=
 * ======================================================================
*/
palbase_init:
	br	r0, br60		/* r0 <- current location  */
br60:	lda	r1, (EntryPoint-br60)(r0) /* r1 <- location of codebase */
	mtpr	r1, palBase		/* set up pal_base register */

	ldah	r0, 0xfff0(r31)		/* r1 <- CBOX base pointer 	*/
	zap	r0, 0xE0, r1		/* R1 = 0000.00FF.FFF0.0000	*/

#ifndef DC21164PC
					/* Turn all 3 banks on and set 	*/
					/* blk size to 64 bytes.	*/
	LDLI	(r0, ((7 << SC_V_SET_EN) | SC_M_FLUSH | SC_M_BLK_SIZE))
	mb
	stq_p	r0, scCtl(r1)		/* leading and trailing "MB"s   */
	mb
#endif /* ! DC21164PC */

#endif /* DC21164 */



#if defined (DC21064) || defined (DC21064A)
	LDLQ	(r1, 0x4E, 0x4001E644)	/* Default BIU value.		*/
	mtpr	r1, pt2
	mtpr	r1, biuCtl
#endif /* DC21064 || DC21064A */

#if defined (DC21064) || defined (DC21064A) || defined (DC21066) || defined (DC21068) 
#define abox_init 0x0000		/* abox = dc disabled, mchks off */
	lda	r1, abox_init(r31)	/* r1 <- abox_ctl 15:0 mask */
	mtpr	r1, pt1
	mtpr	r1, aboxCtl

#define icsr_init 0x05f8

	lda	r1, icsr_init(r31)	/* R1 <- iccsr 47:32 mask	*/
	sll	r1, 32, r1		/* shift it into position	*/
	mtpr	r1, iccsr		/* write the iccsr		*/
	STALL				/* wait 3 cycles		*/
	STALL
	STALL
	STALL

/* ======================================================================
 * = Branch prediction logic						=
 * ======================================================================
 *  XXX Don't know why this is here but it was in the original code 
 *	so I left it
*/
pvc$initbr_1$5007:
	bsr	r1,  br10			/* push pc */
	br	r31, br50			/* back to the mainstream */
pvc$initbr_2$5008:
br10:	bsr	r2,  br20			/* push pc */
pvc$initbr_ret$5007.1:
	ret	r31, (r1)			/* pop pc */
pvc$initbr_3$5009:
br20:	bsr	r3,  br30			/* push pc */
pvc$initbr_4$5008.1:
	ret	r31, (r2)			/* pop pc */
pvc$initbr_5$5010:
br30:	bsr	r4,  br40			/* push pc */
pvc$initbr_6$5009.1:
	ret	r31, (r3)			/* pop pc */
br40:	STALL					/* wait 5 cycles */
	STALL
	STALL
	STALL
	STALL
	STALL
pvc$initbr_7$5010.1:
	ret	r31, (r4)			/* pop pc */
br50:

/* ======================================================================
 * = Initialize the pal_base register to where this code is loaded	=
 * ======================================================================
*/
palbase_init:
	br	r0, br60		/* r0 <- current location  */
br60:	lda	r1, (EntryPoint-br60)(r0) /* r1 <- location of codebase */
	mtpr	r1, palBase		/* set up pal_base register */

/* ======================================================================
 * = Miscellaneous IPRs.						=
 * ======================================================================
*/
	mtpr	r31, astrr		/* stop AST's */
	mtpr	r31, aster		/* : */
	mtpr	r31, hier		/* disable all interrupts */
	mtpr	r31, sier		/* :  */
	mtpr	r31, sirr		/* : */
	mtpr	r31, xtbZap		/* clear the tb's */
	mtpr	r31, slClr		/* clear the sl_clr reg. */
	STALL				/* wait for these to happen first */
	STALL
	STALL
	mfpr	r2,  va			/* unlock va */
	mfpr	r3,  dcAddr		/* unlock dc_addr */
#endif /* DC21064 || DC21064A || DC21066 || DC21068 */

#if defined (DC21064) || defined (DC21064A)
	mtpr	r31, bcTag		/* unlock bc_tag */
	mfpr	r2,  biuAddr		/* unlock biu_addr */
	mfpr	r3,  fillAddr		/* unlock fill_addr */
#endif /* DC21064 || DC21064A */

pvc$mini_initcpu_ret$5000.1:
	ret 	r31, (r23) 
#endif /* DEBUG */


/*=======================================================================
 * = AddICFlush - Adds code to flush the ICache at specified address.	=
 *=======================================================================
 * OVERVIEW:
 *	Use this function to add code to flush the ICache.
 *
 *	XXXXXXXX	mt	r31, flushIc	CPU dependent.
 *	C0000001	br	r0, +4
 *	????????	.long	Destination
 *	6C008000	ldl_p	r0, 0x00(r0)
 *	47FF041F	bis	r31, r31, r31
 *	   .
 *	   . 		Number of NOPs is  CPU dependent.
 *	   .
 *	47FF041F	bis	r31, r31, r31
 *	6BEF0000	jmp	r31, (r0)
 *
 * FORM OF CALL:
 *       bsr r23, AddICFlush
 *
 * ARGUMENTS:
 *	R12<0:31> - Address of QW following last byte of image. 
 *	R12<32:63> - Address where image was xloaded.  
 *	      May be modified if address does not meet the minimum 
 *	      addr. requirement.
 *
 * RETURNS:
 *	R0  - Address of last instruction of code added.
 *	R12 - Address of flush code.
 *
 * REGISTERS:
 *	R1 - NOP instruction counter.
 *	R2 - Instruction to be added.
 *	R3 - Address where image was loaded.
*/
AddICFlush:
	extll	r12, 4 , r3		/* Extract bits <63:32>		*/
	zap	r12, 0xF0, r12		/* Zero bits <63:32>		*/
	lda	r2, 0x4000(r31)		/* Minimum addr for flush code.	*/
	cmpult 	r12, r2, r0		/* Compare to min. addr 	*/
	cmovne 	r0, r2, r12		/* Set to min. addr if less.	*/
	addq	r12, (3+8), r0		/* Align to the next longword.	*/
	bic	r0, 3, r0
	bis	r0, r0, r12
	
#if defined(DC21164) || defined(DC21164PC)
	LDLI(r2, 0x77FF0119)		/* load up mtpr flushIc		*/
	lda	r1, 52(r31)		/* NOP loop counter = 44 + 8   	*/
#endif /* DC21164 || DC21164PC */

#if defined (DC21064) || defined (DC21064A) || defined (DC21066) || defined (DC21068) 
	LDLI(r2, 0x77FF0055)		/* EV4 & LCA variants.		*/
	lda	r1, 14(r31)		/* NOP loop counter = 6 + 8    	*/
#endif /* DC21064 || DC21064A || DC21066 || DC21068 */

	stl_p	r2, 0(r0)		/* store it			*/
	LDLI(r2, 0xC0000001)		/* load up branch instruction.	*/
	stl_p	r2,  4(r0)		/* store it			*/
	stl_p	r3, 8(r0)		/* store Destination.		*/
	LDLI(r2, 0x6C008000)		/* load up load	 :-}		*/
	stl_p	r2, 12(r0)		/* store it			*/
	
	LDLI(r2, 0x47FF041F)		/* load up NOPs			*/
	lda	r0, 16(r0)		/* start at the base+4 instr	*/
noploop:				/* for i = 1 to instCnt 	*/
	stl_p	r2, 0(r0)		/*   store instruction	 	*/
	subq	r1, 1, r1		/*   decrement loop count 	*/
	lda	r0, 4(r0)		/*   increment pointer 		*/
	bne	r1, noploop		/* end for			*/

	LDLI(r2, 0x6BE00000)		/* load up jmp			*/
	stl_p	r2, 0(r0)		/* store it.			*/
	mb				/* force it all out		*/
pvc$mini_addicflush_ret$5003.1:
	ret 	r31, (r23)


/* ======================================================================
 * = Machine Check Handler.						=
 * ======================================================================
*/
machine_check:
	LOAD_STR8(a_star, a_M, a_C, a_H, a_K, a_star, a_cr, a_nl)
pvc$serial_putx_111$4003:
	bsr	r21, putString
					/* Fall thru DumpIPRs. 		*/

/*=======================================================================
 * = DumpIPRs - Prints out CPU IPR registers 				=
 *=======================================================================
 * OVERVIEW:
 *	Use this function to print out CPU IPR registers.
 *
 * FORM OF CALL:
 *       br	r31, DumpIPRs
 *
 * REGISTERS:
 *	Those used by putReg routine.
*/
DumpIPRs:
#ifdef DC21164
	LOAD_IPR4(a_B, a_C, a_t, a_l, pt1)	/* BC_CONTROL	*/
pvc$serial_putreg_135$4008:
	PRINT_REG()
	LOAD_IPR4(a_B, a_C, a_f, a_g, pt2)	/* BC_CONFIG	*/
pvc$serial_putreg_136$4008:
	PRINT_REG()

	LOAD_IPR4(a_I, a_c, a_s, a_r, icsr)	/* ICSR		*/
pvc$serial_putreg_137$4008:
	PRINT_REG()
	LOAD_IPR4(a_P, a_a, a_l, a_B, palBase) /* PAL_BASE	*/
pvc$serial_putreg_138$4008:
	PRINT_REG()
	LOAD_IPR4(a_E, a_x, a_A, a_d, excAddr)	/* EXC_ADDR	*/
pvc$serial_putreg_139$4008:
	PRINT_REG()
	LOAD_IPR4(a_I, a_p, a_l, a_nul, ipl) 	/* IPL		*/
pvc$serial_putreg_142$4008:
	PRINT_REG()
	LOAD_IPR4(a_I, a_n, a_t, a_nul, intId) /* INTID	*/
pvc$serial_putreg_143$4008:
	PRINT_REG()
	LOAD_IPR4(a_I, a_s, a_r, a_nul, isr) 	/* ISR		*/
pvc$serial_putreg_144$4008:
	PRINT_REG()
	LOAD_IPR4(a_I, a_c, a_P, a_E, icPerr) 	/* ICPERR_STAT	*/
pvc$serial_putreg_145$4008:
	PRINT_REG()
	LOAD_IPR4(a_D, a_c, a_M, a_d, dcMode)	/* DC_MODE	*/
pvc$serial_putreg_146$4008:
	PRINT_REG()
	LOAD_IPR4(a_D, a_c, a_P, a_E, dcPerr) 	/* DC_PERR_STAT	*/
pvc$serial_putreg_147$4008:
	PRINT_REG()
#endif /* DC21164 */

#ifdef DC21164PC
	LOAD_IPR4(a_C, a_f, a_g, a_2, pt1)	/* BC_CONTROL	*/
pvc$serial_putreg_135$4008:
	PRINT_REG()
	LOAD_IPR4(a_B, a_C, a_f, a_g, pt2)	/* BC_CONFIG	*/
pvc$serial_putreg_136$4008:
	PRINT_REG()

	LOAD_IPR4(a_I, a_c, a_s, a_r, icsr)	/* ICSR		*/
pvc$serial_putreg_137$4008:
	PRINT_REG()
	LOAD_IPR4(a_P, a_a, a_l, a_B, palBase) /* PAL_BASE	*/
pvc$serial_putreg_138$4008:
	PRINT_REG()
	LOAD_IPR4(a_E, a_x, a_A, a_d, excAddr)	/* EXC_ADDR	*/
pvc$serial_putreg_139$4008:
	PRINT_REG()
	LOAD_IPR4(a_I, a_p, a_l, a_nul, ipl) 	/* IPL		*/
pvc$serial_putreg_142$4008:
	PRINT_REG()
	LOAD_IPR4(a_I, a_n, a_t, a_nul, intId) /* INTID	*/
pvc$serial_putreg_143$4008:
	PRINT_REG()
	LOAD_IPR4(a_I, a_s, a_r, a_nul, isr) 	/* ISR		*/
pvc$serial_putreg_144$4008:
	PRINT_REG()
	LOAD_IPR4(a_I, a_c, a_P, a_E, icPerr) 	/* ICPERR_STAT	*/
pvc$serial_putreg_145$4008:
	PRINT_REG()
	LOAD_IPR4(a_D, a_c, a_M, a_d, dcMode)	/* DC_MODE	*/
pvc$serial_putreg_146$4008:
	PRINT_REG()
	LOAD_IPR4(a_D, a_c, a_P, a_E, dcPerr) 	/* DC_PERR_STAT	*/
pvc$serial_putreg_147$4008:
	PRINT_REG()
#endif /* DC21164PC */

#if defined (DC21064) || defined (DC21064A) || defined (DC21066) || defined (DC21068)
	LOAD_IPR4(a_A,a_b,a_o,a_x, pt1) 		/* ABOX_CTL*/
pvc$serial_putreg_151$4008:
	PRINT_REG()
	LOAD_IPR4(a_I, a_c, a_s, a_r, iccsr)	/* ICCSR */
pvc$serial_putreg_152$4008:
	PRINT_REG()
	LOAD_IPR4(a_P,a_a,a_l,a_B, palBase) 	/* PAL_BASE */
pvc$serial_putreg_153$4008:
	PRINT_REG()
	LOAD_IPR4(a_E, a_x, a_A, a_d, excAddr)	/* EXC_ADDR	*/
pvc$serial_putreg_154$4008:
	PRINT_REG()
	LOAD_IPR4(a_D,a_c, a_S,a_t, dcStat) 	/* DC_STAT */
pvc$serial_putreg_155$4008:
	PRINT_REG()
	LOAD_IPR4(a_H,a_i, a_r,a_r, hirr) 	/* HIRR */
pvc$serial_putreg_156$4008:
	PRINT_REG()
	LOAD_IPR4(a_H,a_i, a_e,a_r, hier) 	/* HIER */
pvc$serial_putreg_162$4008:
	PRINT_REG()
#endif /* DC21064 || DC21064A || DC21066 || DC21068 */

#if defined (DC21064) || defined (DC21064A)
	LOAD_IPR4(a_B,a_C,a_t,a_l,pt2) 		/* BIU_CTL	*/
pvc$serial_putreg_157$4008:
	PRINT_REG()
	LOAD_IPR4(a_B,a_i,a_S,a_t, biuStat)	/* BIU_STAT */
pvc$serial_putreg_158$4008:
	PRINT_REG()
	LOAD_IPR4(a_B,a_i,a_A,a_d, biuAddr)	/* BIU_ADDR */
pvc$serial_putreg_159$4008:
	PRINT_REG()
	LOAD_IPR4(a_S,a_y,a_n,a_nul, fillSyndrome) /* FILL_SYNDROME */
pvc$serial_putreg_160$4008:
	PRINT_REG()
	LOAD_IPR4(a_F,a_i,a_A,a_d, fillAddr)	/* FILL_ADDR */
pvc$serial_putreg_161$4008:
	PRINT_REG()
#endif /* DC21064 || DC21064A */

	br	r31, endcase

/*=======================================================================
 * = SetIPRs - Set CPU IPR registers 					=
 *=======================================================================
 * OVERVIEW:
 *	Use this function to write to some CPU IPR registers.
 *
 * FORM OF CALL:
 *       br	r31, SetIPRs
 *
 * REGISTERS:
 *	Those used by putReg routine.
*/
SetIPRs:
	LOAD_STR8(a_I,a_P,a_R,a_rangle,a_sp,a_nul,a_nul,a_nul)
pvc$serial_putx_112$4003:
	bsr	r21, putString
pvc$serial_getstring_101$4004:
	bsr	r21, getString		/* R0 = R16 = IPR string.	*/
	beq	r0, endcase		/* Abort if nothing was typed.	*/
					/* Need to reverse string for comp.*/
	bis	r31, r31, r8		/* Initialize reversed string.	*/

l0:	extbl	r0, 0, r1		/* Examine the lsb and stop if	*/
	beq	r1, l1			/* byte is = 0.			*/
	srl	r0, 8, r0		/* Next byte of input string.	*/
 	sll	r8, 8, r8		/* Add the new byte to reversed	*/
	bis	r1, r8, r8		/* string.			*/
	br	r31, l0

l1:
pvc$mini_getdaddr_8$5001:
	bsr	r19, getData		/* r0 <- data to deposit	*/
	bis	r0, r0, r18		/* Save data to R18.		*/

#ifdef DC21164
	ldah	r2, 0xfff0(r31)		/* r2 <- CBOX base pointer 	*/
	zap	r2, 0xE0, r2		/* R2 = 0000.00FF.FFF0.0000	*/
	mb
case_bctl:
	LOAD_STR4(a_B, a_C, a_T, a_L)	/* case "bctl"		*/
	xor	r16, r8, r0
	bne	r0, case_bcfg
	mtpr	r18, pt1
	stq_p	r18, bcCtl(r2)
	mb
	br	r31, writemsg

case_bcfg:
	LOAD_STR4(a_B, a_C, a_F, a_G)	/* case "bcfg"		*/
	xor	r16, r8, r0
	bne	r0, case_icsr
	mtpr	r18, pt2
	stq_p	r18, bcCfg(r2)
	mb
	br	r31, writemsg

case_icsr:
	LOAD_STR4(a_I, a_C, a_S, a_R)	/* case "icsr"		*/
	xor	r16, r8, r0
	bne	r0, case_palb
	mtpr	r18, icsr
	br	r31, writemsg

case_palb:
	LOAD_STR4(a_P, a_A, a_L, a_B)	/* case "palb"		*/
	xor	r16, r8, r0
	bne	r0, case_dcmd
	mtpr	r18, palBase
	br	r31, writemsg


case_dcmd:
	LOAD_STR4(a_D, a_C, a_M, a_D)	/* case "dcmd"		*/
	xor	r16, r8, r0
	bne	r0, case_ipl
	mtpr	r18, dcMode
	br	r31, writemsg

case_ipl:
	LOAD_STR4(a_I, a_P, a_L, a_nul)	/* case "ipl"		*/
	xor	r16, r8, r0
	bne	r0, case_unknown
	mtpr	r18, ipl
	br	r31, writemsg
#endif /* DC21164 */


#ifdef DC21164PC
	ldah	r2, 0xfff0(r31)		/* r2 <- CBOX base pointer 	*/
	zap	r2, 0xE0, r2		/* R2 = 0000.00FF.FFF0.0000	*/
	mb
case_bcfg2:
	LOAD_STR4(a_C, a_F, a_G, a_2)	/* case "cfg2"		*/
	xor	r16, r8, r0
	bne	r0, case_bcfg
	mtpr	r18, pt1
	stq_p	r18, bcCfg2(r2)
	mb
	br	r31, writemsg

case_bcfg:
	LOAD_STR4(a_B, a_C, a_F, a_G)	/* case "bcfg"		*/
	xor	r16, r8, r0
	bne	r0, case_icsr
	mtpr	r18, pt2
	stq_p	r18, bcCfg(r2)
	mb
	br	r31, writemsg

case_icsr:
	LOAD_STR4(a_I, a_C, a_S, a_R)	/* case "icsr"		*/
	xor	r16, r8, r0
	bne	r0, case_palb
	mtpr	r18, icsr
	br	r31, writemsg

case_palb:
	LOAD_STR4(a_P, a_A, a_L, a_B)	/* case "palb"		*/
	xor	r16, r8, r0
	bne	r0, case_dcmd
	mtpr	r18, palBase
	br	r31, writemsg


case_dcmd:
	LOAD_STR4(a_D, a_C, a_M, a_D)	/* case "dcmd"		*/
	xor	r16, r8, r0
	bne	r0, case_ipl
	mtpr	r18, dcMode
	br	r31, writemsg

case_ipl:
	LOAD_STR4(a_I, a_P, a_L, a_nul)	/* case "ipl"		*/
	xor	r16, r8, r0
	bne	r0, case_unknown
	mtpr	r18, ipl
	br	r31, writemsg
#endif /* DC21164PC */


#if defined (DC21064) || defined (DC21064A)
case_biu:
	LOAD_STR4(a_B, a_C, a_T, a_L)		/* case "bctl"	*/
	xor	r16, r8, r0
	bne	r0, case_abox
	bis	r18, 4, r18			/* Make sure the OE bit is set */
	mtpr	r18, pt2
	mtpr	r18, biuCtl
	br	r31, writemsg
#endif /* DC21064 || DC21064A */

#if defined (DC21064) || defined (DC21064A) || defined (DC21066) || defined (DC21068) 
case_abox:
	LOAD_STR4(a_A, a_B, a_O, a_X)		/* case "abox"	*/
	xor	r16, r8, r0
	bne	r0, case_icsr
	mtpr	r18, pt1
	mtpr	r18, aboxCtl
	br	r31, writemsg

case_icsr:
	LOAD_STR4(a_I, a_C, a_S, a_R)		/* case "icsr"		*/
	xor	r16, r8, r0
	bne	r0, case_palb
	mtpr	r18, iccsr
	br	r31, writemsg

case_palb:
	LOAD_STR4(a_P, a_A, a_L, a_B)		/* case "palb"		*/
	xor	r16, r8, r0
	bne	r0, case_unknown
	mtpr	r18, palBase
	br	r31, writemsg
#endif /* DC21064 || DC21064A || DC21066 || DC21068 */

case_unknown:
	LOAD_STR8(a_U,a_n,a_K,a_n,a_o,a_w,a_n,a_star)

writemsg:
	STALL				/* Let the changes take effect	*/
	STALL
	STALL
	STALL
	STALL
	STALL

	LOAD_CHAR(a_star)		/* Loads character into register */
pvc$serial_putchar_101$4001:
	PRINT_CHAR()			/* Sends character out serial port*/
pvc$serial_putreg_103$4008:
	PRINT_REG()			/* R16 has register name.	*/
					/* R18 has value.		*/
	br	r31, endcase

#include "serialio.s"
#include "sromio.s"
