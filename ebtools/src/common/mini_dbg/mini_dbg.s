/* 
 *  $Id: mini_dbg.s,v 1.2 1999/01/21 19:06:50 gries Exp $
 *
 * This file implements the minidebugger commands.  It requires the file
 * serialio.s in order to communicate thru the SROM port.
 *
 *	Author: Rogelio R. Cruz, Digital Equipment Corporation
 *	Date: 6-Sept-1994
 *      Origins: Toni Camuso (Alcor)
 *
 * $Log: mini_dbg.s,v $
 * Revision 1.2  1999/01/21 19:06:50  gries
 * First Release to cvs gries
 *
# Revision 1.37  1998/04/03  20:57:52  gries
# make it a dual system minidbg
#
# Revision 1.36  1998/03/26  19:24:27  gries
# turn off ecc for memory test/writes etc
#
# Revision 1.35  1998/02/11  16:16:59  gries
# lastest for ev6 pass2 and q1 q2 tests
#
# Revision 1.34  1997/12/15  18:57:58  pbell
# Added a hook to use the oldminid.s file instead for EV4.
# Added a switch to remove the checksum code for size considerations.
#
# Revision 1.33  1997/12/04  21:39:37  pbell
# Merged 21264 changes and extended 21164PC using new 21264 commands.
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

#include "ascitab.h"
#include "mini_dbg.h"

#ifdef USE_OLD_MINI_DEBUGGER
#include <oldminid.s>
#else

#ifndef MAKEDEPEND
#include "mini_rcs.h"
#endif

/* EV4 does not have a large enough SROM to add the checksum command */
#if !defined(DC21064) && !defined(DC21064A) && !defined(DC21066) && !defined(DC21068) 
#define CHECKSUM_ENABLED
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
 *	For ev6, PT32 (i.e. gpr save-block location 32) is used for saving
 *	the minidebugger state.
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

#ifdef DC21264
/* EV6 exits hardware reset with FPE=1 and IC_EN=3 (all other I_CTL bits are zero)*/

#ifndef DEBUG
/* ======================================================================
 * = PALcode entry Handlers for stand alone case.			=
 * ======================================================================
*/

	/* *DTBM3 */
PAL_EXCEPT(DTBM_DOUBLE_3,	EV6__DTBM_DOUBLE_3_ENTRY)
	pal_exception(a_star, a_D, a_T, a_B, a_M, a_3, a_cr, a_nl)

	/* *DTBM4 */
PAL_EXCEPT(DTBM_DOUBLE_4,	EV6__DTBM_DOUBLE_4_ENTRY)
	pal_exception(a_star, a_D, a_T, a_B, a_M, a_4, a_cr, a_nl)

	/* *FEN* */
PAL_EXCEPT(FEN,	EV6__FEN_ENTRY)
	pal_exception(a_star, a_F, a_E, a_N, a_star, a_nul, a_cr, a_nl)

	/* *UnAli */
PAL_EXCEPT(UNALIGN,	EV6__UNALIGN_ENTRY)
	pal_exception(a_star, a_U, a_n, a_A, a_l, a_i, a_cr, a_nl)

	/* *DTBM* */
PAL_EXCEPT(DTBM_SINGLE,	EV6__DTBM_SINGLE_ENTRY)
	pal_exception(a_star, a_D, a_T, a_B, a_M, a_star, a_cr, a_nl)

	/* *DFlt* */
PAL_EXCEPT(DFAULT,	EV6__DFAULT_ENTRY)
	pal_exception(a_star, a_D, a_F, a_l, a_t, a_star, a_cr, a_nl)

	/* *OPDEC */
PAL_EXCEPT(OPCDEC,	EV6__OPCDEC_ENTRY)
	pal_exception(a_star, a_O, a_P, a_D, a_E, a_C, a_cr, a_nl)

	/* *AcVio */
PAL_EXCEPT(IACV,	EV6__IACV_ENTRY)
	pal_exception(a_star, a_A, a_c, a_V, a_i, a_o, a_cr, a_nl)

	/* *MCHK* */
PAL_EXCEPT(MCHK,	EV6__MCHK_ENTRY)
	pal_exception(a_star, a_M, a_C, a_H, a_K, a_star, a_cr, a_nl)

	/* *ITBM* */
PAL_EXCEPT(ITB_MISS,	EV6__ITB_MISS_ENTRY)
	pal_exception(a_star, a_I, a_T, a_B, a_M, a_star, a_cr, a_nl)

	/* *ARITH */
PAL_EXCEPT(ARITH,	EV6__ARITH_ENTRY)
	pal_exception(a_star, a_A, a_R, a_I, a_T, a_H, a_cr, a_nl)

	/* *INTR */
PAL_EXCEPT(INTERRUPT,	EV6__INTERRUPT_ENTRY)
	pal_exception(a_star, a_I, a_N, a_T, a_R, a_nul, a_cr, a_nl)

	/* *FPCR */
PAL_EXCEPT(MT_FPCR,	EV6__MT_FPCR_ENTRY)
	pal_exception(a_star, a_F, a_P, a_C, a_R, a_nul, a_cr, a_nl)


PAL_EXCEPT(RESET_ENTRY,	EV6__RESET_ENTRY)
	addq	r31,r31,r0		/* initialize Int. Reg. 0*/
	addq	r31,r31,r1		/* initialize Int. Reg. 1*/
	addq	r31,r31,r0		/* initialize Int. Reg. 0*/
	addq	r31,r31,r1		/* initialize Int. Reg. 1*/

	addq	r31,r31,r2		/* initialize Int. Reg. 2*/
	addq	r31,r31,r3		/* initialize Int. Reg. 3*/
	addq	r31,r31,r2		/* initialize Int. Reg. 2*/
	addq	r31,r31,r3		/* initialize Int. Reg. 3*/

	addq	r31,r31,r4		/* initialize Int. Reg. 4*/
	addq	r31,r31,r5		/* initialize Int. Reg. 5*/
	addq	r31,r31,r4		/* initialize Int. Reg. 4*/
	addq	r31,r31,r5		/* initialize Int. Reg. 5*/

	addq	r31,r31,r6		/* initialize Int. Reg. 6*/
	addq	r31,r31,r7		/* initialize Int. Reg. 7*/
	addq	r31,r31,r6		/* initialize Int. Reg. 6*/
	addq	r31,r31,r7		/* initialize Int. Reg. 7*/

	addq	r31,r31,r8		/* initialize Int. Reg. 8*/
	addq	r31,r31,r9		/* initialize Int. Reg. 9*/
	addq	r31,r31,r8		/* initialize Int. Reg. 8*/
	addq	r31,r31,r9		/* initialize Int. Reg. 9*/

	addq	r31,r31,r10		/* initialize Int. Reg. 10*/
	addq	r31,r31,r11		/* initialize Int. Reg. 11*/
	addq	r31,r31,r10		/* initialize Int. Reg. 10*/
	addq	r31,r31,r11		/* initialize Int. Reg. 11*/

	addq	r31,r31,r12		/* initialize Int. Reg. 12*/
	addq	r31,r31,r13		/* initialize Int. Reg. 13*/
	addq	r31,r31,r12		/* initialize Int. Reg. 12*/
	addq	r31,r31,r13		/* initialize Int. Reg. 13*/

	addq	r31,r31,r14		/* initialize Int. Reg. 14*/
	addq	r31,r31,r15		/* initialize Int. Reg. 15*/
	addq	r31,r31,r14		/* initialize Int. Reg. 14*/
	addq	r31,r31,r15		/* initialize Int. Reg. 15*/

	addq	r31,r31,r16		/* initialize Int. Reg. 16*/
	addq	r31,r31,r17		/* initialize Int. Reg. 17*/
	addq	r31,r31,r16		/* initialize Int. Reg. 16*/
	addq	r31,r31,r17		/* initialize Int. Reg. 17*/

	addq	r31,r31,r18		/* initialize Int. Reg. 18*/
	addq	r31,r31,r19		/* initialize Int. Reg. 19*/
	addq	r31,r31,r18		/* initialize Int. Reg. 18*/
	addq	r31,r31,r19		/* initialize Int. Reg. 19*/

	addq	r31,r31,r20		/* initialize Int. Reg. 20*/
	addq	r31,r31,r21		/* initialize Int. Reg. 21*/
	addq	r31,r31,r20		/* initialize Int. Reg. 20*/
	addq	r31,r31,r21		/* initialize Int. Reg. 21*/

	addq	r31,r31,r22		/* initialize Int. Reg. 22*/
	addq	r31,r31,r23		/* initialize Int. Reg. 23*/
	addq	r31,r31,r22		/* initialize Int. Reg. 22*/
	addq	r31,r31,r23		/* initialize Int. Reg. 23*/

	addq	r31,r31,r24		/* initialize Int. Reg. 24*/
	addq	r31,r31,r25		/* initialize Int. Reg. 25*/
	addq	r31,r31,r24		/* initialize Int. Reg. 24*/
	addq	r31,r31,r25		/* initialize Int. Reg. 25*/

	addq	r31,r31,r26		/* initialize Int. Reg. 26*/
	addq	r31,r31,r27		/* initialize Int. Reg. 27*/
	addq	r31,r31,r26		/* initialize Int. Reg. 26*/
	addq	r31,r31,r27		/* initialize Int. Reg. 27*/

	addq	r31,r31,r28		/* initialize Int. Reg. 28*/
	addq	r31,r31,r29		/* initialize Int. Reg. 29*/
	addq	r31,r31,r28		/* initialize Int. Reg. 28*/
	addq	r31,r31,r29		/* initialize Int. Reg. 29*/

	addq	r31,r31,r30		/* initialize Int. Reg. 30*/
	addq	r31,r31,r30		/* initialize Int. Reg. 30*/
	addq	r31,r31,r0		/* initialize retirator 63*/
	addq	r31,r31,r0		/* initialize retirator 64*/

	addq	r31,r31,r0		/* initialize retirator 65*/
	addq	r31,r31,r0		/* initialize retirator 66*/
	addq	r31,r31,r0		/* initialize retirator 67*/
	addq	r31,r31,r0		/* initialize retirator 68*/

	addq	r31,r31,r0		/* initialize retirator 69*/
	addq	r31,r31,r0		/* initialize retirator 70*/
	addq	r31,r31,r0		/* initialize retirator 71*/
	addq	r31,r31,r0		/* initialize retirator 72*/

	addq	r31,r31,r0		/* initialize retirator 73*/
	addq	r31,r31,r0		/* initialize retirator 74*/
	addq	r31,r31,r0		/* initialize retirator 75*/
	addq	r31,r31,r0		/* initialize retirator 76*/

	addq	r31,r31,r0		/* initialize retirator 77*/
	addq	r31,r31,r0		/* initialize retirator 78*/
	addq	r31,r31,r0		/* initialize retirator 79*/
	addq	r31,r31,r0		/* initialize retirator 80*/


/* stop deleting*/

	mtpr r31,EV6__ITB_IA		/* flush the ITB (SCRBRD=4)  *** this also turns on mapper source enables ****/
	mtpr r31,EV6__DTB_IA		/* flush the DTB (SCRBRD=7)*/
	mtpr r31,EV6__VA_CTL		/* clear VA_CTL (SCRBRD=5)*/
	mtpr r31,EV6__M_CTL		/* clear M_CTL (SCRBRD=6)*/

	mtpr r31,EV6__DTB_ASN0	/* clear DTB_ASN0 (SCRBRD=4) creates a map-stall under the above mtpr to SCRBRD=4*/
	mtpr r31,EV6__DTB_ASN1	/* clear DTB_ASN1 (SCRBRD=7)*/
	mtpr r31,EV6__CC_CTL		/* clear CC_CTL (SCRBRD=5)*/
	mtpr r31,EV6__DTB_ALT_MODE	/* clear DTB_ALT_MODE (SCRBRD=6)*/

	addq	r31,r31,r0		/* nop*/
	addq	r31,r31,r0		/* nop*/
	addq	r31,r31,r0		/* nop*/
	br	r31, tch0		/* fetch in next block*/

nxt0:	lda	r0,0x0086(r31)		/* load I_CTL.....*/
	mtpr r0,EV6__I_CTL		/* .....SDE=2, IC_EN=3 (SCRBRD=4)*/
	br	r31, nxt1		/* continue executing in next block*/
tch0:	br	r31, tch1		/* fetch in next block*/

nxt1:	mtpr r31,EV6__IER_CM		/* clear IER_CM (SCRBRD=4) creates a map-stall under the above mtpr to SCRBRD=4*/
	addq	r31,r31,r0		/* nop*/
	br	r31, nxt2		/* continue executing in next block*/
tch1:	br	r31, tch2		/* fetch in next block*/

nxt2:	addq	r31,r31,r0		/* 1st buffer fetch block for above map-stall*/
	addq	r31,r31,r0		/* nop*/
	br	r31, nxt3		/* continue executing in next block*/
tch2:	br	r31, tch3		/* fetch in next block*/

nxt3:	addq	r31,r31,r0		/* 2nd buffer fetch block for above map-stall*/
	addq	r31,r31,r0		/* nop*/
	br	r31, nxt4		/* continue executing in next block*/
tch3:	br	r31, tch4		/* fetch in next block*/

nxt4:	addq	r31,r31,r0		/* need 3rd buffer fetch block to get correct SDE bit for next fetch block*/
	addq	r31,r31,r0		/* nop*/
	br	r31, nxt5		/* continue executing in next block*/
tch4:	br	r31, tch5		/* fetch in next block*/

nxt5:	addq	r31,r31,r4		/* initialize Shadow Reg. 0*/
	addq	r31,r31,r5		/* initialize Shadow Reg. 1*/
	br	r31, nxt6		/* continue executing in next block*/
tch5:	br	r31, tch6		/* fetch in next block*/

nxt6:	addq	r31,r31,r6		/* initialize Shadow Reg. 2*/
	addq	r31,r31,r7		/* initialize Shadow Reg. 3*/
	br	r31, nxt7		/* continue executing in next block*/
tch6:	br	r31, tch7		/* fetch in next block*/

nxt7:	addq	r31,r31,r20		/* initialize Shadow Reg. 4*/
	addq	r31,r31,r21		/* initialize Shadow Reg. 5*/
	br	r31, nxt8		/* continue executing in next block*/
tch7:	br	r31, tch8		/* fetch in next block*/

nxt8:	addq	r31,r31,r22		/* initialize Shadow Reg. 6*/
	addq	r31,r31,r23		/* initialize Shadow Reg. 7*/
	br	r31, nxt9		/* continue executing in next block*/
tch8:	br	r31, nxt0		/* go back to 1st block and start executing*/


/* initialize the bcache config in the cbox*/
/* *** the bcache is initialized OFF for the burnin test ****/
/*  c_reg_csr->bc_enable_a = 0*/
/*  c_reg_csr->init_mode_a = 0*/
/*  c_reg_csr->bc_size_a = 0*/
/*  c_reg_csr->zeroblk_enable_a = 1*/
/*  c_reg_csr->enable_evict_a = 0*/
/*  c_reg_csr->set_dirty_enable_a = 0*/
/*  c_reg_csr->bc_bank_enable_a = 0*/
/*  c_reg_csr->bc_wrt_sts_a = 0*/
/*  c_reg_ipr->write_many_a = 0x000840000*/
/*  value in data register  = 0x000021000*/

/* ***NOTE***  this must be a cache block boundary*/
/* initialize the bcache config in the cbox*/

nxt9:	mb				/* wait for all istream/dstream to complete*/
	lda	r1,0x1000(r31)		/* data<15:00> = 0x1000*/
	ldah	r1,0x0002(r1)	        /* data<31:16> = 0x0002*/
	addq	r31,6,r0		/* shift in 6x 6-bits*/

bccshf:	mtpr	r1,EV6__DATA		/* shift in 6 bits*/
	subq	r0,1,r0			/* decrement R0*/
	beq	r0,bccend		/* done if R0 is zero*/
        srl	r1,6,r1			/* align next 6 bits*/

	br	r31,bccshf		/* continue shifting*/
bccend:	mtpr	r31,EV6__EXC_ADDR + 16	/* dummy IPR write - sets SCBD bit 4  */
	addq	r31,r31,r0		/* nop*/
	addq	r31,r31,r1		/* nop*/

	mtpr	r31,EV6__EXC_ADDR + 16	/* also a dummy IPR write - stalls until above write retires*/
	beq	r31, bccnxt		/* predicts fall through in PALmode*/
	br	r31, .-4		/* fools ibox predictor into infinite loop*/
	addq	r31,r31,r1		/* nop*/

bccnxt:	addq	r31,4,r0		/* load PCTX.....*/
	mtpr r0,EV6__PROCESS_CONTEXT		/* ..... FPE=1 (SCRBRD=4)*/
	lda	r0,0x0003(r31)		/* load DC_CTL.....*/
	mtpr r0,EV6__DC_CTL		/* .....ECC_EN=0, FHIT=0, SET_EN=3 (SCRBRD=6)*/

	addq	r31,r31,r0		/* nop*/
	addq	r31,r31,r1		/* nop*/
	lda	r0,0xff61(r31)		/* R0 = ^xff61  (superpage) */
	zap	r0,0xfc,r0		/* PTE protection for DTB write in next block*/

	mtpr	r31,EV6__DTB_TAG0	/* write DTB_TAG0 (SCRBRD=2,6)*/
	mtpr	r31,EV6__DTB_TAG1	/* write DTB_TAG1 (SCRBRD=1,5)*/
	mtpr	r0,EV6__DTB_PTE0	/* write DTB_PTE0 (SCRBRD=0,4)*/
	mtpr	r0,EV6__DTB_PTE1	/* write DTB_PTE1 (SCRBRD=3,7)*/

	mtpr r31,EV6__SIRR		/* clear SIRR (SCRBRD=4)*/
	lda	r0,0x08FF(r31)		/* load FPCR.....*/
	sll	r0,52,r0		/* .....initial FPCR value*/
	addq	r31,r31,r0		/* nop	itoft	r0,f0			; value = 0x8FF0000000000000*/

	addq	r31,r31,r0		/* nop	mt_fpcr	f0,f0,f0		; do the load*/
	lda	r0,0x2086(r31)		/* load I_CTL.....*/
	ldah	r0,0x0050(r0)		/* .....TB_MB_EN=1, CALL_PAL_R23=1, SL_XMIT=1, SBE=0, SDE=2, IC_EN=3*/
	mtpr	r0,EV6__I_CTL		/* value = 0x0000000000502086 (SCRBRD=4)*/

	mtpr r31,EV6__CC		/* clear CC (SCRBRD=5)*/
	lda	r0,0x001F(r31)		/* write-one-to-clear bits in HW_INT_CLR, I_STAT and DC_STAT*/
	sll	r0,28,r0		/* value = 0x00000001F0000000*/
	mtpr	r0,EV6__HW_INT_CLR	/* clear bits in HW_INT_CLR (SCRBRD=4)*/

	mtpr	r0,EV6__I_STAT		/* clear bits in I_STAT (SCRBRD=4) creates a map-stall under the above mtpr to SCRBRD=4*/
	lda	r0,0x001F(r31)		/* value = 0x000000000000001F*/
	mtpr	r0,EV6__DC_STAT		/* clear bits in DC_STAT (SCRBRD=6)*/
	addq	r31,r31,r0		/* nop*/

	mtpr	r31,EV6__PCTR_CTL	/* 1st buffer fetch block for above map-stall and 1st clear PCTR_CTL (SCRBRD=4)*/
	bis	r31,1,r0		/* set up value for demon write*/
	bis	r31,1,r0		/* set up value for demon write*/
	mulq/v	r31,r31,r0		/* nop*/

	mtpr	r31,EV6__PCTR_CTL	/* 2nd buffer fetch block for above map-stall and 2nd clear PCTR_CTL (SCRBRD=4)*/
	bis	r31,1,r0		/* set up value for demon write*/
	bis	r31,1,r0		/* set up value for demon write*/
	mulq	r31,r31,r0		/* nop*/

	lda	r0,0x780(r31)           /* this is new initialization stuff to prevent */
	wh64	(r0)                    /* ld/st below from going off-chip */
	mb
	bis	r31,1,r0		/* set up value for demon write*/


	ldq_p r1,0x780(r31)		/* flush Pipe 0 LD logic*/
	ldq_p r0,0x788(r31)		/* flush Pipe 1 LD logic*/
	mb				/* wait for LD's to complete*/
	mb				/* wait for LD's to complete*/

	stq_p r1,0x780(r31)		/* flush Pipe 0 ST logic*/
	stq_p r0,0x788(r31)		/* flush Pipe 1 ST logic*/
	bis	r31, 32, r0		/* load loop count of 32*/
jsr_init_loop:
	bsr	r31,jsr_init_loop_nxt			/* JSR to PC+4*/
jsr_init_loop_nxt:
	stq_p r1,0x780(r31)		/* flush Pipe 0 ST logic*/
	subq	r0,1,r0		/* decrement loop count*/
	beq	r0,jsr_init_done	/* done?*/
	br	r31,jsr_init_loop	/* continue loop*/

jsr_init_done:
/*	lda	r0,0x03FF(r31)		; create FP zero.....*/
/*	sll	r0,52,r0		; .....value = 0x3FF0000000000000*/
/*	itoft	r0,f0			; put it into F0 reg*/
/*	addq	r31,r31,r1		; nop (also clears R1)*/

/*	mult	f0,f0,f0		; flush mul-pipe*/
/*	addt	f0,f0,f0		; flush add-pipe*/
/*	divt	f0,f0,f0		; flush div-pipe*/
/*	cvtqt	f0,f0			; flush add-pipe (integer logic)*/

/* SQRT:   add this for pass-3 schematics which enable the sqrt unit.*/
/*	sqrtf	f0,f1*/
/*	addq	r31,r31,r0*/
/* INTNEW: add this for pass-3 schematics which enable the 3-cycle latency unit.*/
/*	perr	r31,r31,r0*/
/*	addq	r31,r31,r0*/

	rc	r0			/* clear interrupt flag*/

	
/*
 * This palbase init exists for the rare cases
 * when this code is loaded into upper memory.
 * That is the case when this code is loaded
 * and executed in memory on a system that has
 * already been initialized.  This technique
 * can sometimes be used to debug snippets of
 * this code.
 */
	br r31,palbase_init
palbase_init:
	br	r0, br60		/* r0 <- current location  */
br60:	lda	r1, (EntryPoint-br60)(r0) /* r1 <- location of codebase */
	mtpr	r1, EV6__PAL_BASE	/* set up pal_base register */
	mb

	bis	r31, 2, r0
	mtpr	r0, EV6__VA_CTL

	bis	r31, 8, r0
	mtpr	r0, EV6__M_CTL

	br	r0, jmp0
jmp0:	addq	r0, (jmp1-jmp0+1), r0
	hw_jmp/stall	(r0)
jmp1:	
	
	
/*
** Now initialize the dcache to allow the
** minidebugger so save gpr's
*/
sweep_dcache:
	lda	r0, KSEG_PREFIX(r31)	/* Kseg = 0xffff8000.00000000	*/
	sll	r0, 32, r0		/* Shift into place		*/
	ldah	r0, 1(r0)		/* r0 = 0xffff8000.00010000	*/
sweep_dcache0:
	subq	r0, 64, r0
	wh64	(r0)
	zap	r0, 0xf0, r1
	bne	r1, sweep_dcache0
	mb

#endif /* not DEBUG */
#endif /* DC21264 */

#if defined (DC21064) || defined (DC21064A) || defined (DC21066) || defined (DC21068) 
#ifndef DEBUG
	br	r31, mchk_end		/* Skip over machine check code. */
/* ======================================================================
 * = Machine Check Handler for stand alone case.			=
 * ======================================================================
*/
.= 0x20
mchk_start:
	br	r31, print_pal_exception
mchk_end:
#endif /* ifndef DEBUG */
#endif /* DC21064 || DC21064A || DC21066 || DC21068 */

	MTPT( r0, 0 )		/* Save R0-R12 & R14-R23 to PALtemps */
	MTPT( r1, 1 )
	MTPT( r2, 2 )
	MTPT( r3, 3 )
	MTPT( r4, 4 )
	MTPT( r5, 5 )
	MTPT( r6, 6 )
	MTPT( r7, 7 )
	MTPT( r8, 8 )
	MTPT( r9, 9 )
	MTPT( r10, 10 )
	MTPT( r11, 11 )
	MTPT( r12, 12 )
#ifdef DC21264
	MTPT( r13, 13 )
#endif
	MTPT( r14, 14 )
#ifndef DC21264
	MTPT( r15, 15 )
#endif
	MTPT( r16, 16 )
	MTPT( r17, 17 )
	MTPT( r18, 18 )
	MTPT( r19, 19 )
	MTPT( r20, 20 )
	MTPT( r21, 21 )
	MTPT( r22, 22 )
	MTPT( r23, 23 )
#ifdef DC21264
//	MTPT( r24, 24 )
	MTPT( r25, 25 )
	MTPT( r26, 26 )
	MTPT( r27, 27 )
	MTPT( r28, 28 )
	MTPT( r29, 29 )
//	MTPT( r30, 30 )

/* Enable the SROM Serial Port interrupt */
	lda	r0, 0x1(r31)
	sll	r0, 32, r0		/* value = 0x0000000100000000 */
	mtpr	r0, EV6__IER
	
#endif /* DC21264 */

#ifdef DEBUG
restore_line_state:	
	STALL
	STALL
	STALL
#ifdef DC21264
#if 0
	MFPT	(r15, 32)		/* Restore current flags.	*/
#endif
#else
	MFPT	(r15, 13)
#endif
mc_io_dispatch:
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
	cmpeq	r29, MC_IO_REPORT_EXC, r0
	beq	r0, next7
	br	r31, print_pal_exception

next7:
	cmpeq	r29, MC_IO_MINI_PROMPT, r0
	bne	r0, PrintVersionID
			/* Fall thru sromConsole */
#else

/*
 *  This loop is here to give the SROM mux a chance to switch before
 *  attempting the autobaud.  It becomes a problem in fast CPUs.
 *  It's only needed when the minidebugger is used as stand alone.
 */
	lda	r0, 30(r31)
waitawhile:
	subq	r0, 1, r0
	bne	r0, waitawhile

pvc$mini_initcpu_1$5000:
#ifdef DC21264
/* DC21264 has  hard-wired reset s/w at 0x780.  it is run first.
   When it is done, the code branches to the top of this segment,
   falls thru to here, and then goes off to the srom console.  It
   is unnecessary to run the initcpu stuff again.
   */
        br      r31,sromConsole
#else
       	bsr 	r23, InitCPU		/* Initialize the CPU.		*/
#endif
#endif /* DEBUG */

/*
 *  Perform baud detection
 */
sromConsole:
pvc$serial_autosync_100$4010:
mdq_autosync_pc1:
	bsr	r21, autobaud_sync	/* do the autobaud thing	*/

mdq_autosync_pc2:

PrintVersionID:				/* Keep version in sync with RCS id */

	LOAD_STR4(a_cr,a_cr,a_nl,a_2)
pvc$serial_putx_100$4003:
	bsr	r21, putString
#if   defined(DC21064)			/* Generate the processor string. */
	LOAD_STR8(a_1,a_0,a_6,a_4,a_period,a_period,a_period, (RCS_ENCODED_REV_H>>(8*3)))
#elif defined(DC21064A)
	LOAD_STR8(a_1,a_0,a_6,a_4,a_A     ,a_period,a_period, (RCS_ENCODED_REV_H>>(8*3)))
#elif defined(DC21066)
	LOAD_STR8(a_1,a_0,a_6,a_6,a_period,a_period,a_period, (RCS_ENCODED_REV_H>>(8*3)))
#elif defined(DC21068)
	LOAD_STR8(a_1,a_0,a_6,a_8,a_period,a_period,a_period, (RCS_ENCODED_REV_H>>(8*3)))
#elif defined(DC21164)
	LOAD_STR8(a_1,a_1,a_6,a_4,a_period,a_period,a_period, (RCS_ENCODED_REV_H>>(8*3)))
#elif defined(DC21164PC)
	LOAD_STR8(a_1,a_1,a_6,a_4,a_P     ,a_C     ,a_period, (RCS_ENCODED_REV_H>>(8*3)))
#elif defined(DC21264)
	LOAD_STR8(a_1,a_2,a_6,a_4,a_period,a_period,a_period, (RCS_ENCODED_REV_H>>(8*3)))
#else
	LOAD_STR8(a_1,a_question,a_question,a_question,a_question,a_question,a_period, (RCS_ENCODED_REV_H>>(8*3)))
#endif
	bsr	r21, putString
	LDLI(r17, RCS_ENCODED_REV_L)
pvc$serial_putx_118$4003:
	bsr	r21, putLong

#ifdef DC21264
#define mc_defaults	(mc_m_display | mc_m_qw)
#else
#define mc_defaults	(mc_m_display)
#endif /* DC21264 */

	lda	r0, (mc_defaults|mc_m_echo)(r31)/* Load defaults	*/
	bis	r15, r0, r15		/* OR them in			*/

printPrompt:
#define	mc_clears	( mc_m_write | mc_m_block| mc_m_echo \
			| mc_m_cp | mc_m_cmp | mc_m_mt \
			| mc_m_wh | mc_m_ecb | mc_m_ck \
			| mc_m_byte | mc_m_word )

	LDLI(r0, mc_clears)
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
#ifdef NEVER_DC21264
/* This block had 21264 as the controlling def and therefore never 
   actually generated any code.  If it is needed for some reason change
   the def and doccument why */
/* use wh64 to allocate the blocks in the dcache */
xm_264_0:
	bic	r7,0xf,r0       /* create a 64-byte address by zapping the low 4 bits of address */
	addq	r12,64,r9       /* add an extra 64 bytes to the byte-count */
	srl	r9,4,r9         /* shift the byte count to get a 64-byte count */
xm_264_1:
	wh64	(r0)            /* allocate the first 64-byte qty */
	addq	r0,0x40,r0      /* bump the address pointer */
	subq	r9,1,r9         /* decrement the 64-byte count */
	bne	r9,xm_264_1     /* keep doing this until all 64-byte chunks have been allocated */
#endif
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
	LDLI(r0, mc_m_xm)
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
	CASE_ON( a_B,a_M, case_ck)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	lda	r15, mc_m_block(r15)	/* Set flag for block	 	*/
	br	r31, common_ex_dep	/* Go to common examine code	*/

case_ck:
#ifdef CHECKSUM_ENABLED
	/*--------------------------------------------------------------*
	 *  "CK" - Checksum a block of memory				*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_C,a_K, case_rb)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	LDLI(r2, (mc_m_block|mc_m_ck))	/* Set flags: block and checksum*/
	bis	r15, r2, r15		/* Set flags.	*/
	br	r31, common_ex_dep	/* Go to common examine code	*/
#endif
case_rb:	/* this label is left so the above code can be common   */
#if defined(DC21264) || defined(DC21164PC)
	/*--------------------------------------------------------------*
	 *  "RB" - Read a byte						*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_R,a_B, case_rw)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	LDLI(r2, mc_m_byte)		/* LDLI should let me specify the base register */
	bis	r15, r2, r15		/* Set flags.	*/
	br	r31, common_ex_dep	/* Go to common examine code	*/

case_rw:
	/*--------------------------------------------------------------*
	 *  "RW" - Read a word						*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_R,a_W, case_eb)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	LDLI(r2, mc_m_word)		/* LDLI should let me specify the base register */
	bis	r15, r2, r15		/* Set flags.	*/
	br	r31, common_ex_dep	/* Go to common examine code	*/

#endif
case_eb:
#ifdef DC21264
	/*--------------------------------------------------------------*
	 *  "EB" - Evict Cache block, 64 bytes for a block of memory	*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_E,a_B, case_dm)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	LDLI(r2, (mc_m_ecb|mc_m_block)) /* LDLI should let me specify the base register */
	bis	r15, r2, r15		/* Set flags.	*/
	br	r31, common_ex_dep	/* Go to common examine code	*/
#endif
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
	CASE_ON( a_F,a_M, case_wb)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	lda	r15, (mc_m_block | mc_m_write)(r15) /* Set flags.	*/
	br	r31, common_ex_dep	/* Go to common deposit code.	*/

case_wb:	/* this label is left so the above code can be common   */
#if defined(DC21264) || defined(DC21164PC)
	/*--------------------------------------------------------------*
	 *  "WB" - Write a byte						*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_W,a_B, case_ww)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	LDLI(r2, (mc_m_byte|mc_m_write))/* LDLI should let me specify the base register */
	bis	r15, r2, r15		/* Set flags.	*/
	br	r31, common_ex_dep	/* Go to common examine code	*/

case_ww:
	/*--------------------------------------------------------------*
	 *  "WW" - Write a word						*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_W,a_W, case_wh)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	LDLI(r2, (mc_m_word|mc_m_write))/* LDLI should let me specify the base register */
	bis	r15, r2, r15		/* Set flags.	*/
	br	r31, common_ex_dep	/* Go to common examine code	*/
#endif
case_wh:
#ifdef DC21264
	/*--------------------------------------------------------------*
	 *  "WH" - Write Hint 64 bytes for a block of memory		*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_W,a_H, case_sb)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	LDLI(r2, (mc_m_wh|mc_m_write|mc_m_block)) /* LDLI should let me specify the base register */
	bis	r15, r2, r15		/* Set flags.	*/
	br	r31, common_ex_dep	/* Go to common examine code	*/

#endif
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
	LDLI(r1, (mc_m_base | mc_m_xm))	/* These bits can't be	*/
	bic	r15, r1, r15		/* preserved across calls.	*/
RestoreState:
#ifdef DC21264
#if 0
	MTPT(r15, 32)		/* Save current flags for later.*/
#endif
#else
        MTPT(r15, 13)
#endif
	STALL				/* Needed to prevent violation	*/
	STALL
	STALL

#ifdef DC21264
	MFPT( r0, 0 )
#endif
	MFPT( r1, 1 )
	MFPT( r2, 2 )
	MFPT( r3, 3 )
	MFPT( r4, 4 )
	MFPT( r5, 5 )
	MFPT( r6, 6 )
	MFPT( r7, 7 )
	MFPT( r8, 8 )
	MFPT( r9, 9 )
	MFPT( r10, 10 )
	MFPT( r11, 11 )
	MFPT( r12, 12 )

	MFPT( r14, 14 )
#ifndef DC21264
	MFPT( r15, 15 )
#endif
	MFPT( r16, 16 )
	MFPT( r17, 17 )
	MFPT( r18, 18 )
	MFPT( r19, 19 )
	MFPT( r20, 20 )
	MFPT( r21, 21 )
	MFPT( r22, 22 )
	MFPT( r23, 23 )
#ifdef DC21264
//	MFPT( r24, 24 )
	MFPT( r25, 25 )
	MFPT( r26, 26 )
	MFPT( r27, 27 )
	MFPT( r28, 28 )
	MFPT( r29, 29 )
//	MFPT( r30, 30 )
#else
	MFPT( r0, 0 )
#endif

pvc$stop_checking_1$2001:
	jsr	r0, (r0)
pvc$stop_checking_2$2003.1:
#ifdef DC21264
#if 0
	MFPT	(r15, 32)		/* Restore current flags.	*/
#endif
#else
	MFPT	(r15, 13)
#endif
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
	CASE_ON( a_X,a_B, case_wm)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	srl	r15, mc_v_xm, r1	/* Has an XM command been 	*/
	blbc	r1, endcase		/* executed? Abort if it hasn't.*/
pvc$mini_addicflush_100$5003:
	bsr	r23, AddICFlush		/* Add IC flush code to image.	*/

#ifdef DC21264
	MTPT	(r12, 25)		/* Save the address code in R0.	*/
#else
	MTPT	(r12, 0)		/* Save the address code in R0.	*/
#endif /* DC21264 */
	br	r31, RestoreState	/* Restore & jump to xloaded image*/

case_wm:
#ifdef DC21264
	/*--------------------------------------------------------------*
	 *  "WM" - Write Write Many Chain
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_W,a_M, case_w1)	/* If match, do the following;	*/
	br	r31, WriteMany

case_w1:
	/*--------------------------------------------------------------*
	 *  "W1" - Write Literal Write Many Chain
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_W,a_1, case_tb)	/* If match, do the following;	*/
	br	r31, WriteManyLiteral

case_tb:
	/*--------------------------------------------------------------*
	 *  "TB" - test boot sequence.  this is pure testing s/w.
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_T,a_B, case_b1)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	lda     r0,0x4000(r31)           /* allocate some space at 0x4000 */
	wh64	(r0)
	addq r0,64,r0
	wh64	(r0)
	mb
	lda	r12,0x4000(r31)
	sll	r12,32,r12
	lda	r0,0x4020(r31)
	bis	r12,r0,r12		/* r12= 00004000 00004020 */
					/* 4000 = ADDQ r31,r31,r0 */
					/* 4004 = ADDQ r31,r31,r1 */
					/* 4008 = ADDQ r31,r31,r2 */
					/* 400c = ADDQ r31,r31,r3 */
					/* 4010 = ? */
#define addq_r31_r31_r0 0x43ff0400
#define addq_r31_r31_r1 0x43ff0401
#define addq_r31_r31_r2 0x43ff0402
#define addq_r31_r31_r3 0x43ff0403

	LDLI	(r2,addq_r31_r31_r0)
	stl_p	r2,-32(r0)
	LDLI	(r2,addq_r31_r31_r1)
	stl_p	r2,-28(r0)
	LDLI	(r2,addq_r31_r31_r2)
	stl_p	r2,-24(r0)
	LDLI	(r2,addq_r31_r31_r3)
	stl_p	r2,-20(r0)


tbxxx:
	bsr	r23, AddICFlush		/* Add IC flush code to image.	*/

	MTPT(r12, 25)			/* Save the address code in R0.	*/
	br	r31, RestoreState	/* Restore & jump to xloaded image*/

case_b1:
	/*--------------------------------------------------------------*
	 *  "B1" - bcache init step 1					*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_B,a_1, case_b2)	/* If match, do the following;	*/
	br	r31, BcInit1

case_b2:
	/*--------------------------------------------------------------*
	 *  "B1" - bcache init step 2					*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_B,a_2, case_b3)	/* If match, do the following;	*/
	br	r31, BcInit2

case_b3:
	/*--------------------------------------------------------------*
	 *  "B1" - bcache init step 3					*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_B,a_3, case_d1)	/* If match, do the following;	*/
	br	r31, BcInit3

case_d1:
	/*--------------------------------------------------------------*
	 *  "D1" - dual tsunami 
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_D,a_1, case_d2)	/* If match, do the following;	*/
	br	r31, dual_tsunami_no_bcache

case_d2:
	/*--------------------------------------------------------------*
	 *  "D2" - dual tsunami with bcache
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_D,a_2, case_st)	/* If match, do the following;	*/
	br	r31, dual_tsunami_bcache	

#endif
case_st:
	/*--------------------------------------------------------------*
	 *  "ST" - Start execution at specified address			*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_S,a_T, case_pr)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
pvc$mini_getdaddr_4$5001:
	bsr	r19, getAddress		/* Get starting address in R0.	*/
#ifdef DC21264
	MTPT	(r0, 25)		/* Write addr to jump to.	*/
#else
	MTPT	(r0, 0)			/* Write addr to jump to.	*/
#endif /* DC21264 */
	bis	r30, 8, r30		/* set Primary CPU Flag		*/
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
	CASE_ON( a_M,a_T, case_q1)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	lda	r15, (mc_m_mt | mc_m_block | mc_m_write)(r15)
	br	r31, common_ex_dep	/* Go to common deposit code	*/

case_q1:
#ifdef DC21264
	/*--------------------------------------------------------------*
	 *  "Q1" - BCache test	.					*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_Q,a_1, case_q2)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	br	r31, quinn_test_1	/* Go to common deposit code	*/

case_q2:
	/*--------------------------------------------------------------*
	 *  "Q2" - BCache test	.					*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_Q,a_2, case_fl)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	br	r31, quinn_test_2	/* Go to common deposit code	*/
#endif
case_fl:
	/*--------------------------------------------------------------*
	 *  "FL" - Prints the current flag states.			*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_F,a_L, case_ba)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
	lda	r4, 0x1(r31)		/* Print flag bits		*/
	sll	r4, 31, r4		/* Sweep across status register	*/
flags_loop:
	bsr	r23, PrintFlagState	/* Print the current flag state	*/
	srl	r4, 1, r4
	bne	r4, flags_loop
	br	r31, endcase

case_ba:
	/*--------------------------------------------------------------*
	 *  "BA" - Enables/Disables use of base address.		*
	 *--------------------------------------------------------------*/ 
	CASE_ON( a_B,a_A, case_flags)	/* If match, do the following;	*/
					/* otherwise, skip to next case	*/
PrintBase:
	bis	r14, r14, r18		/* Copy to argument register.	*/
pvc$serial_putreg_100$4008:
	bsr	r23, putRegVal
	lda	r4, mc_m_base(r31)
	br	r31, ToggleFlagState

case_flags:
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

#ifdef DC21264
	/*--------------------------------------------------------------*
	 *  "SW" - Enable DP264 pass 1 Swizzle patch.			*
	 *--------------------------------------------------------------*/ 
	CASE_ON_FLAG(a_S,a_W, mc_m_swzl)
#endif

	beq	r4, bad_cmd		/* If no match found, then exit	*/

ToggleFlagState:
	xor	r15, r4, r15		/* Toggle this flag and print it*/
	bsr	r23, PrintFlagState	/* Prints the current state.	*/
endcase:
	br	r31, printPrompt
bad_cmd:
	LOAD_STR8(a_E,a_r,a_r,a_o,a_r,a_colon,a_cr,a_nl)
	bsr	r21, putString	
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
.= 0x480	/* Was 0x400 */
mchk_start:
	br	r31, print_pal_exception
#endif /* DEBUG */
#endif /* DC21164 || DC21164PC */


/*===========================================================================
 *=									    =
 *= common_ex_dep - common examine/deposit address routine		    =
 *=									    =
 *==========================================================================*/

common_ex_dep:
#ifdef DC21264
	lda	r19, 3(r31)		/* load DC_CTL			*/
        mtpr    r19, EV6__DC_CTL	/* SET_EN=3 			*/
#endif /* DC21264 */
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
	blbc	r0, GetData		/* then make sure it's added to	*/
	addq	r14, r7, r7		/*  1) the start address	*/
	addq	r14, r8, r8		/*  2) the end address		*/
	addq	r14, r6, r6		/*  3) the 2nd start address	*/

#define mc_nodata (mc_m_writeaddr | mc_m_wh | mc_m_ecb | mc_m_ck)
GetData:
	LDLI(r0, mc_nodata)
	and	r0, r15, r0		/* If using addr, wh or ecb,		*/
	bne	r0, DontAskForData	/* then don't ask for data.	*/

	srl	r15, mc_v_write, r0	/* If not writing,		*/
	blbc	r0 , DontAskForData	/*  then don't ask for data.	*/

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

#ifdef CHECKSUM_ENABLED
	srl	r15, mc_v_ck, r0	/* Is this a checksum command?	*/
	cmovlbs	r0, 0, r10		/* Init Checksum.		*/
#endif

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
//	br	r31, DisplayData	/* Ignore state of silent flag.	*/

DisplayCheck:
	srl	r15, mc_v_display, r0	/* Don't print if in silent mode*/
	blbc	r0, UpdatePtrs

#ifdef DC21264
	LDLI	(r0, (mc_m_wh|mc_m_ecb))/* Don't print for eb or wh	*/
	and	r0, r15, r0
	bne	r0, UpdatePtrs
#endif
	
DisplayData:
	srl	r15, mc_v_write, r0	/* Is this a write follow by read?*/
	blbs	r0, DisplayData2	/* If so, then don't print 1st.	*/

#ifdef CHECKSUM_ENABLED
	srl	r15, mc_v_ck, r0	/* Is this a checksum command?	*/
	blbc	r0, DisplayData1	/* If not skip checksum comp.	*/
ComputeChecksum:
	lda	r1, 4(r31)		/* Determine the right increment*/
	cmovlbs	r15, 8, r1		/* to use in read/write cmds.	*/
	bis	r1, r31, r4		/* Save copy of increment.	*/
CkSum0:	
	lda	r0, 0x8000(r31)		/* Load up 0x8000		*/
	zap	r0, 0xfc, r0		/* ZAP sign extension.		*/
	srl	r10, 1, r2		/* Shift current chksum >> 1	*/
	addq	r0, r2, r0		/* Add 0x8000 to shifted cksum	*/
	cmovlbs	r10, r0, r2		/* Update cksum based on bit 0.	*/
	subq	r4, r1, r0		/* Compute next byte pointer	*/
	extbl	r9, r0, r0		/* Extract next byte.		*/
	addq	r0, r2, r10		/* Add to current cksum.	*/
	zap	r10, 0xfc, r10		/* Keep it within bounds.	*/
	subq	r1, 1, r1		/* Update byte incrementer.	*/
	bne	r1, CkSum0		/* Repeat until done		*/
	br	r31, DisplayData2
#endif

DisplayData1:
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

#ifdef DC21264
	LDLI(r0, (mc_m_wh|mc_m_ecb))	/* Is this for eb or wh		*/
	and	r0, r15, r0		/* AND with flags		*/
	cmovne	r0, 64, r1		/* Increment by cache block	*/
#endif
	
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
	sll	r9, 63, r10		
	srl	r9, 1, r9
	bis	r10, r9, r9
	bis	r9, r9, r10		/* Save for memory test.	*/
	srl	r15, mc_v_loop, r0	/* If in loop mode, then loop.	*/
	blbs	r0, FirstWrite

#ifdef CHECKSUM_ENABLED
	srl	r15, mc_v_ck, r0	/* Is this a checksum command?	*/
	blbc	r0, endcase		/* If not skip checksum display.*/

DisplayChecksum:
	bis	r10, r31, r18
	bsr	r19, PrintData
#endif
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

#ifdef DC21264
	srl	r15, mc_v_swzl, r0
	blbc	r0, WriteAddress
	
	srl	r18, 43, r0		/* Is this non-cacheable space	*/
	blbc	r0, WriteAddress	/* If so... skip swizzle	*/

WriteSwizzle:	/*  b7 b6 b5 b4 b3 b2 b1 b0 => b7 b3 b6 b2 b5 b1 b4 b0	*/
	bis	r31, r31, r4		/* init scratchpad		*/
	bis	r31, 4, r0		/* n = 4			*/
SwzlW:	subq	r0, 1, r0		/* decrement n			*/
	sll	r0, 1, r1		/* Compute n*2			*/
	extbl	r9, r0, r2		/* Extract B(n)			*/
	insbl	r2, r1, r3		/* Insert  B(n*2)		*/
	bis	r4, r3, r4		/* OR into scratchpad		*/

	addq	r0, 4, r2		/* Compute n+4			*/
	addq	r1, 1, r1		/* Compute n*2 + 1		*/
	extbl	r9, r2, r3		/* Extract B(n+4)		*/
	insbl	r3, r1, r3		/* Insert  B(n*2 + 1)		*/
	bis	r4, r3, r4		/* OR into scratchpad		*/

	bne	r0, SwzlW		/* Loop until done		*/
	
	bis	r4, r31, r9		/* Result back into data reg	*/
	
WriteAddress:
#endif
	srl	r15, mc_v_writeaddr, r0	/* fill with its own address?	*/
	cmovlbs r0, r18, r9		/* if so, then data = address.  */

	ornot	r31, r9, r2		/* Negate the data to write.	*/
	srl	r15, mc_v_notdata, r0	/* Use negated data?		*/
	cmovlbs r0, r2, r9		/* if yes, then data = ~data. 	*/

DoDeposit:
#if defined(DC21264) || defined(DC21164PC)
	LDLI	(r0, (mc_m_byte|mc_m_word|mc_m_wh|mc_m_ecb))/* Need Kseg*/
	and	r0, r15, r0
	beq	r0,  SkipWriteKseg
WriteKseg:
	ldah	r1, KSEG_PREFIX(r31)	/* Kseg = 0xffff????.00000000	*/
	sll	r1, 16, r1		/* see mini_dbg.h for values */
	bis	r18, r1, r18		/* Or in Kseg address		*/
SkipWriteKseg:
#endif
#ifdef DC21264
	LDLI	(r0, (mc_m_wh|mc_m_ecb))/* WH or EB followed by WH?	*/
	and	r0, r15, r0
	beq	r0,  SkipWHint
WHint:
	wh64	(r18)
	br	r31, WriteDone
SkipWHint:
#endif
#if defined(DC21264) || defined(DC21164PC)
	LDLI	(r0, (mc_m_byte|mc_m_word))/* Byte or Word access?	*/
	and	r0, r15, r0
	beq	r0,  WriteQW
WriteB:
	srl	r15, mc_v_byte, r0	/* Byte operation?		*/
	blbc	r0, WriteW
	stb	r9, 0(r18)		/* Write a Byte			*/
	br	r31, WriteDone
WriteW:
	stw	r9, 0(r18)		/* Write a Word			*/
	br	r31, WriteDone
#endif
WriteQW:
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
#if defined(DC21264) || defined(DC21164PC)
	LDLI	(r0, (mc_m_byte|mc_m_word|mc_m_wh|mc_m_ecb))/* Need Kseg*/
	and	r0, r15, r0
	beq	r0,  SkipReadKseg
ReadKseg:
	ldah	r1, KSEG_PREFIX(r31)		/* Kseg = 0xffff????.00000000	*/
	sll	r1, 16, r1			/* see mini_dbg.h for values */
	bis	r18, r1, r18		/* Or in Kseg address		*/
SkipReadKseg:	
#endif
#ifdef DC21264
	LDLI	(r0, (mc_m_wh|mc_m_ecb))/* WH or EB followed by EB?	*/
	and	r0, r15, r0
	beq	r0,  SkipEvictCB
EvictCB:
	ecb	(r18)
	br	r31, ExamineDone
SkipEvictCB:
#endif
#if defined(DC21264) || defined(DC21164PC)
	LDLI	(r0, (mc_m_byte|mc_m_word))/* Byte or Word access?	*/
	and	r0, r15, r0
	beq	r0,  ReadQW
	
ReadB:
	srl	r15, mc_v_byte, r0	/* Byte operation?		*/
	blbc	r0, ReadW
	ldbu	r0, 0(r18)		/* Read a Byte			*/
	br	r31, ExamineDone
ReadW:
	ldwu	r0, 0(r18)		/* Read a Word			*/
	br	r31, ExamineDone
#endif
ReadQW:
	blbc	r15,  ReadLW		/* If LW operation, then do it.	*/
	ldq_p	r0, 0(r18)		/*  read the QW			*/
	br	r31, ExamineDone
ReadLW:
	ldl_p	r0, 0(r18)		/* Read the LW			*/

ExamineDone:	
#ifdef DC21264
	srl	r15, mc_v_swzl, r1
	blbc	r1, ReturnRead
	
	srl	r18, 43, r1		/* Is this non-cacheable space	*/
	blbc	r1, ReturnRead		/* If so... skip swizzle	*/

ReadSwizzle:	/*  b7 b3 b6 b2 b5 b1 b4 b0 => b7 b6 b5 b4 b3 b2 b1 b0	*/
	bis	r0, r31, r9		/* Put data into temp register	*/

	bis	r31, r31, r4		/* init scratchpad		*/
	bis	r31, 4, r0		/* n = 4			*/
SwzlR:	subq	r0, 1, r0		/* decrement n			*/
	sll	r0, 1, r1		/* Compute n*2			*/
	extbl	r9, r1, r2		/* Extract B(n*2)		*/
	insbl	r2, r0, r3		/* Insert  B(n)			*/
	bis	r4, r3, r4		/* OR into scratchpad		*/

	addq	r0, 4, r2		/* Compute n+4			*/
	addq	r1, 1, r1		/* Compute n*2 + 1		*/
	extbl	r9, r1, r3		/* Extract B(n*2 + 1)		*/
	insbl	r3, r2, r3		/* Insert  B(n+4)		*/
	bis	r4, r3, r4		/* OR into scratchpad		*/

	bne	r0, SwzlR		/* Loop until done		*/

	bis	r4, r31, r0		/* Result back into data reg	*/

ReturnRead:
#endif
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
 	cmpeq	r4, mc_m_follow_w_wr, r0 /* Write after flag?		*/
	bne	r0, GotName
#ifdef DC21264
	LOAD_STR8(a_S, a_w, a_i, a_z, a_z, a_l, a_I, a_O)
	LDLI(r0, mc_m_swzl)
 	cmpeq	r4, r0, r0		/* DP264 p1 swizzle flag?	*/
	bne	r0, GotName
#endif
	ret	r31, (r23)		/* Nothing to do		*/
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
GetParameter:
	bsr	r21, putString		/* Print string.		*/
	LOAD_STR4(a_rangle, a_sp, a_nul, a_nul)
	br	r31, getDataPrint
getData:				/* Load "D> " into arg reg.	*/
	LOAD_STR4(a_D, a_rangle, a_sp, a_nul)
	br	r31, getDataPrint
getAddress:				/* Load "A> " into arg reg.	*/
	LOAD_STR4(a_A, a_rangle, a_sp, a_nul)
getDataPrint:
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
	MFPT( r18, 0 )	 		/* R0 */
pvc$mini_preg_0$4050:
	bsr	r19, print_reg_reset
	MFPT( r18, 1 ) 			/* R1 */
pvc$mini_preg_1$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 2 ) 			/* R2 */
pvc$mini_preg_2$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 3 )	     		/* R3 */
pvc$mini_preg_3$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 4 ) 			/* R4 */
pvc$mini_preg_4$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 5 ) 			/* R5 */
pvc$mini_preg_5$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 6 )	 		/* R6 */
pvc$mini_preg_6$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 7 ) 			/* R7 */
pvc$mini_preg_7$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 8 ) 			/* R8 */
pvc$mini_preg_8$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 9 )	 		/* R9 */
pvc$mini_preg_9$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 10)  		/* R10*/
pvc$mini_preg_10$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 11)  		/* R11*/
pvc$mini_preg_11$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 12)  		/* R12*/
pvc$mini_preg_12$4050:
	bsr	r19, print_reg_normal
	MFPT2(r18, 13)			/* R13*/
pvc$mini_preg_13$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 14)  		/* R14*/
pvc$mini_preg_14$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 15)  		/* R15*/
pvc$mini_preg_15$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 16)  		/* R16*/
pvc$mini_preg_16$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 17)  		/* R17*/
pvc$mini_preg_17$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 18)  		/* R18*/
pvc$mini_preg_18$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 19)  		/* R19*/
pvc$mini_preg_19$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 20)  		/* R20*/
pvc$mini_preg_20$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 21)  		/* R21*/
pvc$mini_preg_21$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 22)  		/* R22*/
pvc$mini_preg_22$4050:
	bsr	r19, print_reg_normal
	MFPT( r18, 23)  		/* R23*/
pvc$mini_preg_23$4050:
	bsr	r19, print_reg_normal
//	MFPT2( r18, 24)			/* R24*/
	bis	r31, r24, r18
pvc$mini_preg_24$4050:
	bsr	r19, print_reg_normal
	MFPT2( r18, 25)	    		/* R25*/
pvc$mini_preg_25$4050:
	bsr	r19, print_reg_normal
	MFPT2( r18, 26)			/* R26*/
pvc$mini_preg_26$4050:
	bsr	r19, print_reg_normal
	MFPT2( r18, 27)			/* R27*/
pvc$mini_preg_27$4050:
	bsr	r19, print_reg_normal
	MFPT2( r18, 28)			/* R28*/
pvc$mini_preg_28$4050:
	bsr	r19, print_reg_normal
	MFPT2( r18, 29)			/* R29*/
pvc$mini_preg_29$4050:
	bsr	r19, print_reg_normal
//	MFPT2( r18, 30)			/* R30*/
	bis	r31, r30, r18
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

#ifdef DC21264

/* EV6: 
     1) map the physical registers
     2) map the shadow registers
     3) init the write-many chain
     4) init the various IPRs
*/


#endif
	
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

	ldah	r0, SEXT(0xfff0)(r31)	/* r1 <- CBOX base pointer 	*/
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



#if defined(DC21064) || defined(DC21064A)
	LDLQ	(r1, 0x4E, 0x4001E644)	/* Default BIU value.		*/
	mtpr	r1, pt2
	mtpr	r1, biuCtl
#endif /* DC21064 || DC21064A */

#if defined(DC21064) || defined(DC21064A) || defined(DC21066) || defined(DC21068) 
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

#if defined(DC21064) || defined(DC21064A)
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
	bis	r0, 1, r12               /* add 1 to keep in palmode */
	
#ifdef DC21264
#define mtpr_flushic 0x77FF1310
#define hw_ret_stall_r0 0x7be02000

	LDLI(r2, mtpr_flushic)		/* load up mtpr flushIc		*/
	stl_p	r2, 0(r0)		/* store it			*/
        LDLI(r2, 0xC0000001)		/* load up a branch to pc+4*/
        stl_p	r2,  4(r0)		/* store it */
	addq	r0,21,r2		/* re-entrance at PC of the branch */
					/* note: PAL bit set for hw_ret re-entrance */
        stl_p	r2, 8(r0)		/* store it */
        LDLI(r2, 0x6C008000)		/* load up a load instruction */
	stl_p	r2,  12(r0)		/* store it */
	LDLI(r2, hw_ret_stall_r0)	/* load up a hw_ret/stall using r0 as the base reg */
	stl_p	r2,  16(r0)		/* store it */
        LDLI(r2, 0xC0000001)		/* load up a branch to pc+4*/
	stl_p	r2,  20(r0)		/* store it */
	stl_p   r3,  24(r0)		/* store DEST of code entrypoint */
        LDLI(r2, 0x6C008000)		/* load up a load instruction */
	stl_p	r2,  28(r0)		/* store it */
	LDLI(r2, 0x6BE00000)		/* load up jmp			*/
	stl_p	r2, 32(r0)		/* store it.			*/
	mb				/* force it all out		*/
	
#endif /* DC21264 */
	
#if defined(DC21164) || defined(DC21164PC)
	LDLI(r2, 0x77FF0119)		/* load up mtpr flushIc		*/
	lda	r1, 52(r31)		/* NOP loop counter = 44 + 8   	*/
#endif /* DC21164 || DC21164PC */

#if defined(DC21064) || defined(DC21064A) || defined(DC21066) || defined(DC21068) 
	LDLI(r2, 0x77FF0055)		/* EV4 & LCA variants.		*/
	lda	r1, 14(r31)		/* NOP loop counter = 6 + 8    	*/
#endif /* DC21064 || DC21064A || DC21066 || DC21068 */

#ifndef DC21264
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
#endif

	/* Load up signature here for now. */
	ldah	r19, SEXT(0xdecb)(r31)
	zap	r19, 0xf0, r19

pvc$mini_addicflush_ret$5003.1:
	ret 	r31, (r23)


/* ======================================================================
 * = Common PALcode entry Handling.					=
 * ======================================================================
*/
print_pal_exception:
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
#ifdef DC21264

/* use pvc$serial_putreg_105$4008 to pvc$serial_putreg_134$4008 for ev6 */

/* read ebox iprs */
		
	LOAD_IPR4(a_c, a_c, a_nul, a_nul, EV6__CC)	/* read cc	*/
pvc$serial_putreg_105$4008:
	PRINT_REG()

	LOAD_IPR4(a_v, a_a, a_nul, a_nul, EV6__VA)	/* read va	*/
pvc$serial_putreg_106$4008:
	PRINT_REG()

	LOAD_IPR4(a_v, a_a, a_f, a_o, EV6__VA_FORM)	/* read vafo	*/
pvc$serial_putreg_107$4008:
	PRINT_REG()

/* read ibox iprs */

	LOAD_IPR4(a_e, a_x, a_c, a_a, EV6__EXC_ADDR)	/* read exca	*/
pvc$serial_putreg_108$4008:
	PRINT_REG()

	LOAD_IPR4(a_i, a_v, a_a, a_f, EV6__IVA_FORM)	/* read ivaf	*/
pvc$serial_putreg_109$4008:
	PRINT_REG()

	LOAD_IPR4(a_p, a_s, a_nul, a_nul, EV6__PS)	/* read ps	*/
pvc$serial_putreg_110$4008:
	PRINT_REG()

	LOAD_IPR4(a_i, a_e, a_r, a_nul, EV6__IER)	/* read ier	*/
pvc$serial_putreg_111$4008:
	PRINT_REG()
	
	LOAD_IPR4(a_i, a_e, a_r, a_c, EV6__IER_CM)	/* read ierc	*/
pvc$serial_putreg_112$4008:
	PRINT_REG()
	
	LOAD_IPR4(a_s, a_i, a_r, a_r, EV6__SIRR)	/* read sirr	*/
pvc$serial_putreg_113$4008:
	PRINT_REG()

	LOAD_IPR4(a_i, a_s, a_u, a_m, EV6__ISUM)	/* read isum	*/
pvc$serial_putreg_114$4008:
	PRINT_REG()

	LOAD_IPR4(a_e, a_x, a_c, a_s, EV6__EXC_SUM)	/* read excs	*/
pvc$serial_putreg_115$4008:
	PRINT_REG()

	LOAD_IPR4(a_p, a_a, a_l, a_b, EV6__PAL_BASE)	/* read palb	*/
pvc$serial_putreg_116$4008:
	PRINT_REG()

	LOAD_IPR4(a_i, a_c, a_t, a_l, EV6__I_CTL)	/* read ictl	*/
pvc$serial_putreg_117$4008:
	PRINT_REG()

	LOAD_IPR4(a_p, a_c, a_t, a_r, EV6__PCTR_CTL)	/* read pctr	*/
pvc$serial_putreg_118$4008:
	PRINT_REG()

	LOAD_IPR4(a_i, a_s, a_t, a_a, EV6__I_STAT)	/* read ista	*/
pvc$serial_putreg_119$4008:
	PRINT_REG()

	LOAD_IPR4(a_a, a_s, a_n, a_nul, EV6__ASN)	/* read asn	*/
pvc$serial_putreg_120$4008:
	PRINT_REG()

	LOAD_IPR4(a_a, a_s, a_t, a_e, EV6__ASTER)	/* read aste	*/
pvc$serial_putreg_121$4008:
	PRINT_REG()

	LOAD_IPR4(a_a, a_s, a_t, a_r, EV6__ASTRR)	/* read astr	*/
pvc$serial_putreg_122$4008:
	PRINT_REG()

	LOAD_IPR4(a_p, a_p, a_c, a_e, EV6__PPCE)	/* read ppce	*/
pvc$serial_putreg_123$4008:
	PRINT_REG()
		
	LOAD_IPR4(a_f, a_p, a_e, a_nul, EV6__FPE)	/* read fpe	*/
pvc$serial_putreg_124$4008:
	PRINT_REG()
	
/* read mbox iprs */

	LOAD_IPR4(a_m, a_m, a_s, a_t, EV6__MM_STAT)	/* read mmst	*/
pvc$serial_putreg_125$4008:
	PRINT_REG()

	
	LOAD_IPR4(a_d, a_c, a_s, a_t, EV6__DC_STAT)	/* read dcst	*/
pvc$serial_putreg_126$4008:
	PRINT_REG()

#endif /* DC21264 */

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

#if defined(DC21064) || defined(DC21064A) || defined(DC21066) || defined(DC21068) 
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

#if defined(DC21064) || defined(DC21064A)
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


#ifdef DC21264

/* end of generic setipr stuff. here comes ev6 */
/* ev6 ipr commands start here */
/* EV6 EV6 EV6 EV6 EV6 EV6 EV6 EV6 EV6 EV6 EV6 EV6 */
	
/* here are the commands to manipulate the EBOX iprs */
	
case_cc:
	LOAD_STR4(a_C, a_C, a_nul, a_nul)	/* case "CC" 		*/
	xor	r16, r8, r0
	bne	r0, case_cc_ctl
	mtpr	r18, EV6__CC
	br	r31, writemsg

case_cc_ctl:
	LOAD_STR4(a_C, a_C, a_T, a_L)	/* case "CCTL" 		*/
	xor	r16, r8, r0
	bne	r0, case_va_ctl
	mtpr	r18, EV6__CC_CTL
	br	r31, writemsg
	
case_va_ctl:
	LOAD_STR4(a_V, a_A, a_C, a_T)	/* case "VACT" 		*/
	xor	r16, r8, r0
	bne	r0, case_itb_tag
	mtpr	r18, EV6__VA_CTL
	br	r31, writemsg

/* here are the commands to manipulate the IBOX iprs */

case_itb_tag:
	LOAD_STR4(a_I, a_T, a_A, a_G)	/* case  "ITAG"	*/
	xor	r16, r8, r0
	bne	r0, case_itb_pte
	mtpr	r18, EV6__ITB_TAG
	br	r31, writemsg

case_itb_pte:
	LOAD_STR4(a_I, a_P, a_T, a_E)	/* case  "IPTE"	*/
	xor	r16, r8, r0
	bne	r0, case_itb_iap
	mtpr	r18, EV6__ITB_PTE
	br	r31, writemsg

case_itb_iap:
	LOAD_STR4(a_I, a_I, a_A, a_P)	/* case  "IIAP"	*/
	xor	r16, r8, r0
	bne	r0, case_itb_ia
	mtpr	r18, EV6__ITB_IAP
	br	r31, writemsg

case_itb_ia:
	LOAD_STR4(a_I, a_I, a_A, a_nul)	/* case  "IIA"	*/
	xor	r16, r8, r0
	bne	r0, case_itb_is
	mtpr	r18, EV6__ITB_IA
	br	r31, writemsg

case_itb_is:
	LOAD_STR4(a_I, a_I, a_S, a_nul)	/* case "IIS"  	*/
	xor	r16, r8, r0
	bne	r0, case_ps
	mtpr	r18, EV6__ITB_IS
	br	r31, writemsg

case_ps:
	LOAD_STR4(a_P, a_S, a_nul, a_nul)	/* case "PS"  	*/
	xor	r16, r8, r0
	bne	r0, case_ier
	mtpr	r18, EV6__PS
	br	r31, writemsg

case_ier:
	LOAD_STR4(a_I, a_E, a_R, a_nul)	/* case "IER"  	*/
	xor	r16, r8, r0
	bne	r0, case_ier_cm
	mtpr	r18, EV6__IER
	br	r31, writemsg

case_ier_cm:
	LOAD_STR4(a_I, a_E, a_R, a_C)	/* case "IERC"  	*/
	xor	r16, r8, r0
	bne	r0, case_sirr
	mtpr	r18, EV6__IER_CM
	br	r31, writemsg

case_sirr:
	LOAD_STR4(a_S, a_I, a_R, a_R)	/* case "SIRR"  	*/
	xor	r16, r8, r0
	bne	r0, case_hw_int_clr
	mtpr	r18, EV6__SIRR
	br	r31, writemsg

case_hw_int_clr:
	LOAD_STR4(a_H, a_W, a_I, a_C)	/* case "HWIC"  	*/
	xor	r16, r8, r0
	bne	r0, case_pal_base
	mtpr	r18, EV6__HW_INT_CLR
	br	r31, writemsg

case_pal_base:
	LOAD_STR4(a_P, a_A, a_L, a_B)	/* case "PALB"  	*/
	xor	r16, r8, r0
	bne	r0, case_i_ctl
	mtpr	r18, EV6__PAL_BASE
	br	r31, writemsg

case_i_ctl:
	LOAD_STR4(a_I, a_C, a_T, a_L)	/* case "ICTL"  	*/
	xor	r16, r8, r0
	bne	r0, case_ic_flush_asm
	mtpr	r18, EV6__I_CTL
	br	r31, writemsg

case_ic_flush_asm:
	LOAD_STR4(a_I, a_C, a_F, a_A)	/* case "ICFA"  	*/
	xor	r16, r8, r0
	bne	r0, case_pctr_ctl
	mtpr	r18, EV6__IC_FLUSH_ASM
	br	r31, writemsg

case_pctr_ctl:
	LOAD_STR4(a_P, a_C, a_T, a_R)	/* case "PCTR"  	*/
	xor	r16, r8, r0
	bne	r0, case_i_stat
	mtpr	r18, EV6__PCTR_CTL
	br	r31, writemsg

case_i_stat:
	LOAD_STR4(a_I, a_S, a_T, a_A)	/* case "ISTA" 	*/
	xor	r16, r8, r0
	bne	r0, case_asn
	mtpr	r18, EV6__I_STAT
	br	r31, writemsg

case_asn:
	LOAD_STR4(a_A, a_S, a_N, a_nul)	/* case "ASN"  	*/
	xor	r16, r8, r0
	bne	r0, case_aster
	mtpr	r18, EV6__ASN
	br	r31, writemsg

case_aster:
	LOAD_STR4(a_A, a_S, a_T, a_E)	/* case "ASTE"  	*/
	xor	r16, r8, r0
	bne	r0, case_astrr
	mtpr	r18, EV6__ASTER
	br	r31, writemsg

case_astrr:
	LOAD_STR4(a_A, a_S, a_T, a_R)	/* case "ASTR"  	*/
	xor	r16, r8, r0
	bne	r0, case_ppce
	mtpr	r18, EV6__ASTRR
	br	r31, writemsg

case_ppce:
	LOAD_STR4(a_P, a_P, a_C, a_E)	/* case "PPCE"  	*/
	xor	r16, r8, r0
	bne	r0, case_dtb_iap
	mtpr	r18, EV6__PPCE
	br	r31, writemsg

case_dtb_iap:
	LOAD_STR4(a_D, a_I, a_A, a_P)	/* case "DIAP"  	*/
	xor	r16, r8, r0
	bne	r0, case_dtb_ia
	mtpr	r18, EV6__DTB_IAP
	br	r31, writemsg

case_dtb_ia:
	LOAD_STR4(a_D, a_I, a_A, a_nul)	/* case "DIA"  	*/
	xor	r16, r8, r0
	bne	r0, case_dtb_is0
	mtpr	r18, EV6__DTB_IA
	br	r31, writemsg

case_dtb_is0:
	LOAD_STR4(a_D, a_I, a_S, a_0)	/* case "DIS0"  	*/
	xor	r16, r8, r0
	bne	r0, case_dtb_is1
	mtpr	r18, EV6__DTB_IS0
	br	r31, writemsg

case_dtb_is1:
	LOAD_STR4(a_D, a_I, a_S, a_1)	/* case "DIS1"  	*/
	xor	r16, r8, r0
	bne	r0, case_dtb_asn0
	mtpr	r18, EV6__DTB_IS1
	br	r31, writemsg

case_dtb_asn0:
	LOAD_STR4(a_A, a_S, a_N, a_X)	/* case "ASN0"  	*/
	xor	r16, r8, r0
	bne	r0, case_dtb_alt_mode
	mtpr	r18, EV6__DTB_ASN0
	mtpr	r18, EV6__DTB_ASN1
	br	r31, writemsg

case_dtb_alt_mode:
	LOAD_STR4(a_A, a_L, a_T, a_M)	/* case "ALTM"  	*/
	xor	r16, r8, r0
	bne	r0, case_m_ctl
	mtpr	r18, EV6__DTB_ALT_MODE
	br	r31, writemsg

case_m_ctl:
	LOAD_STR4(a_M, a_C, a_T, a_L)	/* case "MCTL"  	*/
	xor	r16, r8, r0
	bne	r0, case_dc_ctl
	mtpr	r18, EV6__M_CTL
	br	r31, writemsg

case_dc_ctl:
	LOAD_STR4(a_D, a_C, a_T, a_L)	/* case "DCTL"  	*/
	xor	r16, r8, r0
	bne	r0, case_dc_stat
	mtpr	r18, EV6__DC_CTL
	br	r31, writemsg

case_dc_stat:
	LOAD_STR4(a_D, a_S, a_T, a_A)	/* case "DSTA"  	*/
	xor	r16, r8, r0
	bne	r0, case_unknown
	mtpr	r18, EV6__DC_STAT
	br	r31, writemsg

/* ev6 ipr commands end here */
/* EV6 EV6 EV6 EV6 EV6 EV6 EV6 EV6 EV6 EV6 EV6 EV6 */

#endif /* DC21264 */

#ifdef DC21164
	ldah	r2, SEXT(0xfff0)(r31)	/* r2 <- CBOX base pointer 	*/
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
	ldah	r2, SEXT(0xfff0)(r31)	/* r2 <- CBOX base pointer 	*/
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


#if defined(DC21064) || defined(DC21064A)
case_biu:
	LOAD_STR4(a_B, a_C, a_T, a_L)		/* case "bctl"	*/
	xor	r16, r8, r0
	bne	r0, case_abox
	bis	r18, 4, r18			/* Make sure the OE bit is set */
	mtpr	r18, pt2
	mtpr	r18, biuCtl
	br	r31, writemsg
#endif /* DC21064 || DC21064A */

#if defined(DC21064) || defined(DC21064A) || defined(DC21066) || defined(DC21068) 
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

#ifdef DC21264

/*=======================================================================
 * = WriteMany -  Write the Write Many Chain.				=
 *=======================================================================
 * OVERVIEW:
 *	Prompts user for parameters and writes the Write Many Chain.
 *
 * FORM OF CALL:
 *       br	r31, WriteMany
 *
 * REGISTERS:
 *	R??,R??,...
*/

WriteMany:
	bis	r31, r31, r14		/* Init temporary register.	*/
WM_ENAB:
	LOAD_STR4(a_E, a_N, a_A, a_B)	/* enab (bc_enable_a)		*/
	bsr	r19, GetParameter	/* Load paramater into R0.	*/
	
	and	r0, 1, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_ENABLE(1))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 0, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/
	
	bis	r14, r31, r18		/* Print source address and	*/
	bsr	r23, putRegVal		/* data of first read performed.*/

WM_SIZE:	
	LOAD_STR4(a_S, a_I, a_Z, a_E)	/* size (bc_size_a)		*/
	bsr	r19, GetParameter	/* Load paramater into R0.	*/
	
	and	r0, 1, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_SIZE1(1))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 0, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	and	r0, 2, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_SIZE2(2))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 1, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	and	r0, 4, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_SIZE3(4))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 2, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	and	r0, 8, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_SIZE4(8))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 3, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	bis	r31, r31, r13
	and	r0, 1, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_SIZE1_H(1))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 0, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r13, r1, r13		/* Merge into Chain		*/

	and	r0, 2, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_SIZE2_H(2))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 1, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r13, r1, r13		/* Merge into Chain		*/

	and	r0, 4, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_SIZE3_H(4))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 2, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r13, r1, r13		/* Merge into Chain		*/

	and	r0, 8, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_SIZE4_H(8))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 3, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r13, r1, r13		/* Merge into Chain		*/

	sll	r13, 32, r13
	bis	r14, r13, r14

	bis	r14, r31, r18		/* Print source address and	*/
	bsr	r23, putRegVal		/* data of first read performed.*/

WM_INVAL:	
	LOAD_STR4(a_I, a_N, a_V, a_L)	/* invl (zeroblk_enable_a)	*/
	bsr	r19, GetParameter	/* Load paramater into R0.	*/

	and	r0, 1, r1		/* Mask bit.			*/
	LDLI(r16, WM_ZEROBLK_ENABLE0(1))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 0, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	and	r0, 2, r1		/* Mask bit.			*/
	LDLI(r16, WM_ZEROBLK_ENABLE1(2))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 1, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	bis	r14, r31, r18		/* Print source address and	*/
	bsr	r23, putRegVal		/* data of first read performed.*/

WM_DIRT:	
	LOAD_STR4(a_D, a_I, a_R, a_T)	/* dirt (set_dirty_enable_a)	*/
	bsr	r19, GetParameter	/* Load paramater into R0.	*/
	
	and	r0, 1, r1		/* Mask bit.			*/
	LDLI(r16, WM_SET_DIRTY_ENABLE0(1))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 0, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	and	r0, 2, r1		/* Mask bit.			*/
	LDLI(r16, WM_SET_DIRTY_ENABLE1(2))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 1, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	and	r0, 4, r1		/* Mask bit.			*/
	LDLI(r16, WM_SET_DIRTY_ENABLE2(4))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 2, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	bis	r14, r31, r18		/* Print source address and	*/
	bsr	r23, putRegVal		/* data of first read performed.*/

WM_INIT:	
	LOAD_STR4(a_I, a_N, a_I, a_T)	/* init (init_mode_a)		*/
	bsr	r19, GetParameter	/* Load paramater into R0.	*/
	
	and	r0, 1, r1		/* Mask bit.			*/
	LDLI(r16, WM_INIT_MODE(1))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 0, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	LDLI(r16, BC_ENABLE_I_MODE_MASK)
	bic	r14, r16, r16
	cmovne	r1, r16, r14		/* Clear bit if Init mode set	*/
	
	bis	r14, r31, r18		/* Print source address and	*/
	bsr	r23, putRegVal		/* data of first read performed.*/

WM_EVIC:	
	LOAD_STR4(a_E, a_V, a_I, a_C)	/* evic (enable_evict_a)	*/
	bsr	r19, GetParameter	/* Load paramater into R0.	*/

	and	r0, 1, r1		/* Mask bit.			*/
	LDLI(r16, WM_ENABLE_EVICT(1))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 0, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	bis	r14, r31, r18		/* Print source address and	*/
	bsr	r23, putRegVal		/* data of first read performed.*/

WM_WRTS:	
	LOAD_STR4(a_W, a_R, a_T, a_S)	/* wrts (bc_wrt_sts_a)		*/
	bsr	r19, GetParameter	/* Load paramater into R0.	*/
	
	and	r0, 1, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_WRT_STS0(1))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 0, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	and	r0, 2, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_WRT_STS1(2))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 1, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	and	r0, 4, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_WRT_STS2(4))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 2, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	and	r0, 8, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_WRT_STS3(8))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 3, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	bis	r14, r31, r18		/* Print source address and	*/
	bsr	r23, putRegVal		/* data of first read performed.*/

WM_BANK:	
	LOAD_STR4(a_B, a_A, a_N, a_K)	/* bank (bc_bank_enable_a)	*/
	bsr	r19, GetParameter	/* Load paramater into R0.	*/

	and	r0, 1, r1		/* Mask bit.			*/
	LDLI(r16, WM_BC_BANK_ENABLE(1))
	zap	r16, 0xf0, r16		/* Zap upper longword		*/
	srl	r1, 0, r1
	mulq	r1, r16, r1		/* Qualify WM mask		*/
	bis	r14, r1, r14		/* Merge into Chain		*/

	br	r31, PrintChain
WriteManyLiteral:	
	LOAD_STR4(a_W, a_M, a_rangle, a_sp)/* bank (bc_bank_enable_a)	*/
	bsr	r19, getDataPrint	/* Load paramater into R0.	*/
	bis	r0, r31, r14
	
PrintChain:
	bis	r14, r31, r18		/* Print source address and	*/
	bsr	r23, putRegVal		/* data of first read performed.*/

	addq	r31, 6, r0		/* shift in 6x 6-bits*/
	mb				/* wait for all istream/dstream to complete*/

	br	r31, Wmshf
	.align 6
Wmshf:	mtpr	r14, EV6__DATA		/* shift in 6 bits		*/
	subq	r0, 1, r0		/* decrement R0			*/
	beq	r0, Wmend		/* done if R0 is zero		*/
        srl	r14, 6, r14		/* align next 6 bits		*/

	br	r31, Wmshf		/* continue shifting		*/
Wmend:	mtpr	r31, EV6__EXC_ADDR + 16	/* dummy IPR write - sets SCBD bit 4  */
	addq	r31, r31, r0		/* nop				*/
	addq	r31, r31, r14		/* nop				*/

	mtpr	r31, EV6__EXC_ADDR + 16	/* also a dummy IPR write - stalls until above write retires*/
	beq	r31, Wmnxt		/* predicts fall through in PALmode*/
	br	r31, .-4		/* fools ibox predictor into infinite loop*/
	addq	r31, r31, r14		/* nop*/
Wmnxt:
	br	r31, endcase            /* return to cmd input routine*/


#ifdef	MEM_TEST
//
//	srom memory test modelled after the debug monitor code
//
//	r4 - low address
//	r5 - high address
//	r6 - test pattern
//	r9 - mem_stride (0x200000)
//
DoMemTests:
	bsr	r19, getAddress		/* r0 <- start address		*/
	bis	r0, r0, r4		/* r7 <- start address 		*/
	bis	r0, r0, r5		/* r8 <- default end address 	*/
	bsr	r19, getAddress		/* Get the last			*/
	bis	r0, r0, r5		/* address of the block.	*/
	
	ldah	r9, 0x20(r31)		/* Load stride.			*/
	
DoMemTest1:
	LDLI	(r6, 0xA5A5A5A5)	/* Load pattern.		*/
	bsr		r26, mem_test2

DoMemTest2:
	LDLI	(r6, 0x5A5A5A5A)	/* Load pattern.		*/
	bsr		r26, mem_test2

DoMemTest3:
	bsr		r26, mem_test3

DoMemTest4:
	bis		r31, 1, r6
	bsr		r26, mem_test1

DoMemTest4:
	bic		r31, r31, r6
	bsr		r26, mem_test1

MemTestsDone:
	br		r31, memtest_passed

//
//	bsr	r26, mem_test1
//
mem_test1:
//  for (d = 1; d != 0; d <<= 1) {
	bis		r31, 1, r18
	bis		r31, r18, r10
test1_for1:
//    pattern = test ? d : ~d;
	blbs	r6, test1_comp_skip
	subq	r31, r18, r10
	subq	r10, 1, r10
	zap	r10, 0xf0, r10
test1_comp_skip:		
//    
//    for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
	bis		r4, r31, r7
test1_for2:
//      for (p=ps; p<hlim; p += MEMSTRIDE) {
	bis		r7, r31, r8
test1_for3:
//	*p = pattern;
	stl_p	r10, 0(r8)
	addq	r8, r9, r8
	cmplt	r8, r5, r0
	bne		r0, test1_for3
//      }
	addq	r7, 4, r7
	addq	r4, r9, r0
	cmplt	r7, r0, r0
	bne		r0, test1_for2
//    }
//
//    for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
	bis		r4, r31, r7
test1_for4:
//      for (p=ps; p<hlim; p += MEMSTRIDE) {
	bis		r7, r31, r8
test1_for5:
//	data = *p;
	ldl_p	r2, 0(r8)
	zap	r2, 0xf0, r2
//	if (data != pattern)
//	  if (!dump_error(p, data, pattern))
//	    return;
	cmpeq	r10, r2, r0
	bne		r0, test1_data_ok

	bis	r2, r2, r18		/* Print source address and	*/
	bsr	r19, PrintAddrData	/* data of first read performed.*/

	LOAD_STR8(a_E,a_x,a_p,a_e,a_c,a_t,a_colon,a_sp)
	bsr	r21, putString
	bis	r10, r10, r18
	bsr	r19, PrintData


test1_data_ok:
	addq	r8, r9, r8
	cmplt	r8, r5, r0
	bne		r0, test1_for5
//      }
	addq	r7, 4, r7
	addq	r4, r9, r0
	cmplt	r7, r0, r0
	bne		r0, test1_for4
//    }
//    UserPutChar((char)'.');
//    if (CheckForChar(3)) return; /* Check for Control-C */

	sll		r18, 1, r18
	zap		r18, 0xf0, r18
	bne		r18, test1_for1
//  }

	ret	r31, (r26)	

//
//	bsr	r26, mem_test2
//
mem_test2:

//  pattern = seed;
	bis		r31,r6,  r10
//  for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
	bis		r4, r31, r7
test2_for1:
//    for (p=ps; p<hlim; p += MEMSTRIDE) {
	bis		r7, r31, r8
test2_for2:
//      *p = pattern;
	stl_p	r10, 0(r8)
	addq	r8, r9, r8
	cmplt	r8, r5, r0
	blbs	r0, test2_for2
//    }
	addq	r7, 4, r7
	addq	r4, r9, r0
	cmplt	r7, r0, r0
	blbs	r0, test2_for1
//  }

    }
  }

//  for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
	bis		r4, r31, r7
test2_for3:
//    for (p=ps; p<hlim; p += MEMSTRIDE) {
	bis		r7, r31, r8
test2_for4:
//      data = *p;
	ldl_p	r2, 0(r8)
//      if (data != pattern)
//			if (!dump_error(p, data, pattern))
//				return;
	cmpeq	r10, r2, r0
	blbs	r0, test2_data_ok

	bis		r2, r2, r17
	OutLEDPORT(0xFF)
	jump_to_mini_prompt()		/* trap to mini, no autobaud	*/
	ret		r31, (r26)

test2_data_ok:
	addq	r8, r9, r8
	cmplt	r8, r5, r0
	blbs	r0, test2_for4
//    }
	addq	r7, 4, r7
	addq	r4, r9, r0
	cmplt	r7, r0, r0
	blbs	r0, test2_for3
//  }

	ret		r31, (r26)


//
//	bsr	r26, mem_test3
//
mem_test3:
//  for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
	bis		r4, r31, r7
test3_for1:
//    for (p=ps; p<hlim; p += MEMSTRIDE) {
	bis		r7, r31, r8
test3_for2:
//      *p = (ui) 0;
	stl_p	r31, 0(r8)
	addq	r8, r9, r8
	cmplt	r8, r5, r0
	bne		r0, test3_for2
//    }
	addq	r7, 4, r7
	addq	r4, r9, r0
	cmplt	r7, r0, r0
	bne		r0, test3_for1
//  }

//  for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
	bis		r4, r31, r7
test3_for3:
//    for (p=ps; p<hlim; p += MEMSTRIDE) {
	bis		r7, r31, r8
test3_for4:
//      *p = (ui) p;
	stl_p	r8, 0(r8)
	addq	r8, r9, r8
	cmplt	r8, r5, r0
	bne		r0, test3_for4
//    }
	addq	r7, 4, r7
	addq	r4, r9, r0
	cmplt	r7, r0, r0
	bne		r0, test3_for3
//  }

//  for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
	bis		r4, r31, r7
test3_for5:
//    for (p=ps; p<hlim; p += MEMSTRIDE) {
	bis		r7, r31, r8
test3_for6:
//      data = *p;
	ldl_p	r2, 0(r8)
//      if (data != (ui) p)
//	if (!dump_error(p, data, (ui) p))
//	  return;
	cmpeq	r8, r2, r0
	bne		r0, test3_data_ok

	bis		r2, r2, r17
	OutLEDPORT(0xFF)
	jump_to_mini_prompt()		/* trap to mini, no autobaud	*/
	ret		r31, (r26)

test3_data_ok:
	addq	r8, r9, r8
	cmplt	r8, r5, r0
	bne		r0, test3_for6
//    }
	addq	r7, 4, r7
	addq	r4, r9, r0
	cmplt	r7, r0, r0
	bne		r0, test3_for5
//  }

	ret		r31, (r26)

memtest_passed:
	br	r31, endcase
#endif	/* MEM_TEST */
	
/*
From:	AD::QUINN        "Mike Quinn 225-5155 HLO 2-3/K4" 10-OCT-1997 18:55:56.14
To:	ricks::hooker
CC:	QUINN
Subj:	srom bcache read_write loop

To make this code work perfectly you need to make the following change to
the Cbox write_once chain:

	c_reg_csr->dcvic_threshold_a(7,0) = 0x1

Also, only 1 dcache set can be on.  We'll also have to run this after
enabling/sweeping the bcache, enabling Tsunami, and sweeping main memory
(I really can't use the WH64's to initialize the dirty blocks I need).
The I_CTL should also still be in it's initialized value (ie, the SBE
bits must still be zero).

****************************************************************************
*/
quinn_test_1:
	mb				/* wait for outstanding MEMOP's to 
complete	*/
	mfpr	r0, EV6__I_CTL		/* get current I_CTL value	*/
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

	ldah	r1, 2(r31)		/* create mask for setting 
SINGLE_ISSUE bit (17)	*/
	bis	r0, r1, r0		/* set SINGLE_ISSUE bit	*/
	mtpr	r0, EV6__I_CTL		/* load new I_CTL value	*/
	br	r0, hwstall		/* create dest address for hwstall	
*/
hwstall:

	addq	r0, (rw_setup-hwstall+1), r0 /* finish computing dest address (rw_setup:) for hwstall	*/
	hw_jmp/stall	(r0)		/* hw_jmp_stall (rw_setup:) to wait 
for update of I_CTL	*/
hwstall0:
	br	r31, hwstall0		/* hang here until hw_jmp_stall 
retires	*/
	addq	r31, r31, r31		/* nop	*/

rw_setup:

	lda	r11, 0x5555(r31)
	ldah	r11, 0x5555(r11)
	sll	r11, 32, r12
	bis	r11, r12, r11		/* r11 = 0x5555555555555555	*/

	ornot	r31, r11, r12		/* r12 = 0xaaaaaaaaaaaaaaaa	*/
	ornot	r31, r31, r13		/* r13 = 0xffffffffffffffff	*/
	ldah	r14, 0x0010(r31)	/* r14 = 0x0000000000100000	*/
	ldah	r9, 0x0011(r31)	/* r9 = 0x0000000000110000	*/

/* create dirty dc_blks at PA:100000, PA:1007C0, PA:110000, PA:1107C0	*/

	stq_p	r31, 0x000(r14)
	stq_p	r31, 0x008(r14)
	stq_p	r13, 0x010(r14)
	stq_p	r13, 0x018(r14)

	stq_p	r31, 0x020(r14)
	stq_p	r31, 0x028(r14)
	stq_p	r13, 0x030(r14)
	stq_p	r13, 0x038(r14)

	stq_p	r11, 0x7C0(r14)
	stq_p	r11, 0x7C8(r14)
	stq_p	r12, 0x7D0(r14)
	stq_p	r12, 0x7D8(r14)

	stq_p	r11, 0x7E0(r14)
	stq_p	r11, 0x7E8(r14)
	stq_p	r12, 0x7F0(r14)
	stq_p	r12, 0x7F8(r14)

	mb
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

	stq_p	r13, 0x000(r9)
	stq_p	r13, 0x008(r9)
	stq_p	r31, 0x010(r9)
	stq_p	r31, 0x018(r9)

	stq_p	r13, 0x020(r9)
	stq_p	r13, 0x028(r9)
	stq_p	r31, 0x030(r9)
	stq_p	r31, 0x038(r9)

	stq_p	r12, 0x7C0(r9)
	stq_p	r12, 0x7C8(r9)
	stq_p	r11, 0x7D0(r9)
	stq_p	r11, 0x7D8(r9)

	stq_p	r12, 0x7E0(r9)
	stq_p	r12, 0x7E8(r9)
	stq_p	r11, 0x7F0(r9)
	stq_p	r11, 0x7F8(r9)

	mb
	lda	r10, 0x7fff(r31)	/* load loop counter	*/
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

/* ready to start loop	*/

	stq_p	r31, 0x000(r14)
	mb
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

rw_loop:

	stq_p	r11, 0x7C0(r14)
	stq_p	r13, 0x000(r9)
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

	mb
	subq	r10, 1, r10		/* decrement loop counter	*/
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

	stq_p	r12, 0x7C0(r9)
	stq_p	r31, 0x000(r14)
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

	mb
	beq	r10, rw_check		/* exit loop if r10 is zero ******** 
Change this to a NOP to endlessly loop *******	*/
	addq	r31, r31, r31		/* cbr's predict fall-thru in 
palmode	*/
	br	r31, rw_loop		/* continue loop	*/

rw_check:

	ldq_p	r10, 0x000(r14)	
	cmpeq	r10, r31, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x008(r14)	

	cmpeq	r10, r31, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x010(r14)	
	cmpeq	r10, r13, r10

	beq	r10, rw_failed
	ldq_p	r10, 0x018(r14)	
	cmpeq	r10, r13, r10
	beq	r10, rw_failed

	ldq_p	r10, 0x020(r14)	
	cmpeq	r10, r31, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x028(r14)	

	cmpeq	r10, r31, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x030(r14)	
	cmpeq	r10, r13, r10

	beq	r10, rw_failed
	ldq_p	r10, 0x038(r14)	
	cmpeq	r10, r13, r10
	beq	r10, rw_failed

	ldq_p	r10, 0x7C0(r9)	
	cmpeq	r10, r12, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x7C8(r9)	

	cmpeq	r10, r12, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x7D0(r9)	
	cmpeq	r10, r11, r10

	beq	r10, rw_failed
	ldq_p	r10, 0x7D8(r9)	
	cmpeq	r10, r11, r10
	beq	r10, rw_failed

	ldq_p	r10, 0x7E0(r9)	
	cmpeq	r10, r12, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x7E8(r9)	

	cmpeq	r10, r12, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x7F0(r9)	
	cmpeq	r10, r11, r10

	beq	r10, rw_failed
	ldq_p	r10, 0x7F8(r9)	
	cmpeq	r10, r11, r10
	beq	r10, rw_failed

	mb
	ldq_p	r10, 0x000(r9)	
	mb
	cmpeq	r10, r13, r10

	beq	r10, rw_failed
	ldq_p	r10, 0x7C0(r14)	
	mb
	cmpeq	r10, r11, r10

	beq	r10, rw_failed
	ldq_p	r10, 0x008(r9)	
	cmpeq	r10, r13, r10
	beq	r10, rw_failed

	ldq_p	r10, 0x010(r9)	
	cmpeq	r10, r31, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x018(r9)	

	cmpeq	r10, r31, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x020(r9)	
	cmpeq	r10, r13, r10

	beq	r10, rw_failed
	ldq_p	r10, 0x028(r9)	
	cmpeq	r10, r13, r10
	beq	r10, rw_failed

	ldq_p	r10, 0x030(r9)	
	cmpeq	r10, r31, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x038(r9)	

	cmpeq	r10, r31, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x7C8(r14)	
	cmpeq	r10, r11, r10

	beq	r10, rw_failed
	ldq_p	r10, 0x7D0(r14)	
	cmpeq	r10, r12, r10
	beq	r10, rw_failed

	ldq_p	r10, 0x7D8(r14)	
	cmpeq	r10, r12, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x7E0(r14)	

	cmpeq	r10, r11, r10
	beq	r10, rw_failed
	ldq_p	r10, 0x7E8(r14)	
	cmpeq	r10, r11, r10

	beq	r10, rw_failed
	ldq_p	r10, 0x7F0(r14)	
	cmpeq	r10, r12, r10
	beq	r10, rw_failed

	ldq_p	r10, 0x7F8(r14)	
	cmpeq	r10, r12, r10
	beq	r10, rw_failed
	mb

rw_passed:
	LOAD_STR8(a_P,a_a,a_s,a_s,a_e,a_d,a_colon,a_sp)
	bsr	r21, putString
	br	r31, endcase            /* return to cmd input routine*/

rw_failed:
	LOAD_STR8(a_F,a_a,a_i,a_l,a_e,a_d,a_colon,a_sp)
	bsr	r21, putString
	br	r31, endcase            /* return to cmd input routine*/



quinn_test_2:
	mb				/* wait for outstanding MEMOP's to 
complete	*/
	mfpr	r0, EV6__I_CTL		/* get current I_CTL value	*/
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

	ldah	r1, 2(r31)		/* create mask for setting 
SINGLE_ISSUE bit (17)	*/
	bis	r0, r1, r0		/* set SINGLE_ISSUE bit	*/
	mtpr	r0, EV6__I_CTL		/* load new I_CTL value	*/
	br	r0, hwstall1		/* create dest address for hwstall	
*/
hwstall1:

	addq	r0, (rw_setup1-hwstall1+1), r0 /* finish computing dest address (rw_setup:) for hwstall1 */
	hw_jmp/stall	(r0)		/* hw_jmp_stall (rw_setup:) to wait for update of I_CTL	*/
hwstall2:
	br	r31, hwstall2		/* hang here until hw_jmp_stall 
retires	*/
	addq	r31, r31, r31		/* nop	*/

rw_setup1:

	lda	r11, 0x5555(r31)
	ldah	r11, 0x5555(r11)
	sll	r11, 32, r12
	bis	r11, r12, r11		/* r11 = 0x5555555555555555	*/

	ornot	r31, r11, r12		/* r12 = 0xaaaaaaaaaaaaaaaa	*/
	ornot	r31, r31, r13		/* r13 = 0xffffffffffffffff	*/
	ldah	r14, 0x0010(r31)	/* r14 = 0x0000000000100000	*/
	ldah	r9, 0x0011(r31)	/* r9 = 0x0000000000110000	*/

/* create dirty dc_blks at PA:100000, PA:1007C0, PA:110000, PA:1107C0	*/

	stq_p	r31, 0x000(r14)
	stq_p	r31, 0x008(r14)
	stq_p	r13, 0x010(r14)
	stq_p	r13, 0x018(r14)

	stq_p	r31, 0x020(r14)
	stq_p	r31, 0x028(r14)
	stq_p	r13, 0x030(r14)
	stq_p	r13, 0x038(r14)

	stq_p	r11, 0x7C0(r14)
	stq_p	r11, 0x7C8(r14)
	stq_p	r12, 0x7D0(r14)
	stq_p	r12, 0x7D8(r14)

	stq_p	r11, 0x7E0(r14)
	stq_p	r11, 0x7E8(r14)
	stq_p	r12, 0x7F0(r14)
	stq_p	r12, 0x7F8(r14)

	mb
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

	stq_p	r13, 0x000(r9)
	stq_p	r13, 0x008(r9)
	stq_p	r31, 0x010(r9)
	stq_p	r31, 0x018(r9)

	stq_p	r13, 0x020(r9)
	stq_p	r13, 0x028(r9)
	stq_p	r31, 0x030(r9)
	stq_p	r31, 0x038(r9)

	stq_p	r12, 0x7C0(r9)
	stq_p	r12, 0x7C8(r9)
	stq_p	r11, 0x7D0(r9)
	stq_p	r11, 0x7D8(r9)

	stq_p	r12, 0x7E0(r9)
	stq_p	r12, 0x7E8(r9)
	stq_p	r11, 0x7F0(r9)
	stq_p	r11, 0x7F8(r9)

	mb
	lda	r10, 0x7fff(r31)	/* load loop counter	*/
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

/* ready to start loop	*/

	stq_p	r31, 0x000(r14)
	mb
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

rw_loop1:

	stq_p	r11, 0x7C0(r14)
	stq_p	r13, 0x000(r9)
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

	mb
	subq	r10, 1, r10		/* decrement loop counter	*/
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

	stq_p	r12, 0x7C0(r9)
	stq_p	r31, 0x000(r14)
	addq	r31, r31, r31		/* nop	*/
	addq	r31, r31, r31		/* nop	*/

	mb
	addq	r31, r31, r31		/* ******** Change this to a NOP to 
endlessly loop *******	*/
	addq	r31, r31, r31		/* cbr's predict fall-thru in 
palmode	*/
	br	r31, rw_loop1		/* continue loop	*/

rw_check1:

	ldq_p	r10, 0x000(r14)	
	cmpeq	r10, r31, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x008(r14)	

	cmpeq	r10, r31, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x010(r14)	
	cmpeq	r10, r13, r10

	beq	r10, rw_failed1
	ldq_p	r10, 0x018(r14)	
	cmpeq	r10, r13, r10
	beq	r10, rw_failed1

	ldq_p	r10, 0x020(r14)	
	cmpeq	r10, r31, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x028(r14)	

	cmpeq	r10, r31, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x030(r14)	
	cmpeq	r10, r13, r10

	beq	r10, rw_failed1
	ldq_p	r10, 0x038(r14)	
	cmpeq	r10, r13, r10
	beq	r10, rw_failed1

	ldq_p	r10, 0x7C0(r9)	
	cmpeq	r10, r12, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x7C8(r9)	

	cmpeq	r10, r12, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x7D0(r9)	
	cmpeq	r10, r11, r10

	beq	r10, rw_failed1
	ldq_p	r10, 0x7D8(r9)	
	cmpeq	r10, r11, r10
	beq	r10, rw_failed1

	ldq_p	r10, 0x7E0(r9)	
	cmpeq	r10, r12, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x7E8(r9)	

	cmpeq	r10, r12, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x7F0(r9)	
	cmpeq	r10, r11, r10

	beq	r10, rw_failed1
	ldq_p	r10, 0x7F8(r9)	
	cmpeq	r10, r11, r10
	beq	r10, rw_failed1

	mb
	ldq_p	r10, 0x000(r9)	
	mb
	cmpeq	r10, r13, r10

	beq	r10, rw_failed1
	ldq_p	r10, 0x7C0(r14)	
	mb
	cmpeq	r10, r11, r10

	beq	r10, rw_failed1
	ldq_p	r10, 0x008(r9)	
	cmpeq	r10, r13, r10
	beq	r10, rw_failed1

	ldq_p	r10, 0x010(r9)	
	cmpeq	r10, r31, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x018(r9)	

	cmpeq	r10, r31, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x020(r9)	
	cmpeq	r10, r13, r10

	beq	r10, rw_failed1
	ldq_p	r10, 0x028(r9)	
	cmpeq	r10, r13, r10
	beq	r10, rw_failed1

	ldq_p	r10, 0x030(r9)	
	cmpeq	r10, r31, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x038(r9)	

	cmpeq	r10, r31, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x7C8(r14)	
	cmpeq	r10, r11, r10

	beq	r10, rw_failed1
	ldq_p	r10, 0x7D0(r14)	
	cmpeq	r10, r12, r10
	beq	r10, rw_failed1

	ldq_p	r10, 0x7D8(r14)	
	cmpeq	r10, r12, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x7E0(r14)	

	cmpeq	r10, r11, r10
	beq	r10, rw_failed1
	ldq_p	r10, 0x7E8(r14)	
	cmpeq	r10, r11, r10

	beq	r10, rw_failed1
	ldq_p	r10, 0x7F0(r14)	
	cmpeq	r10, r12, r10
	beq	r10, rw_failed1

	ldq_p	r10, 0x7F8(r14)	
	cmpeq	r10, r12, r10
	beq	r10, rw_failed1
	mb

rw_passed1:
	LOAD_STR8(a_P,a_a,a_s,a_s,a_e,a_d,a_colon,a_sp)
	bsr	r21, putString
	br	r31, endcase            /* return to cmd input routine*/

rw_failed1:
	LOAD_STR8(a_F,a_a,a_i,a_l,a_e,a_d,a_colon,a_sp)
	bsr	r21, putString
	br	r31, endcase            /* return to cmd input routine*/





/*=======================================================================
 * = BcInit -  blindly init a 2Mbyte BCache				=
 *=======================================================================
 * OVERVIEW:
 *	Use this function to blindly init a 2Mbyte BCache
 *
 * FORM OF CALL:
 *       br	r31, BcInit
 *
 * REGISTERS:
 *	R0,R20,R21.
*/

	.align 6
BcInit1:

/* CBox write many chain */

/* bits   CSR*/
/* 0      bc_enable		1	0x7, 0x5 (0x7 w/init_mode, 0x5 without)*/
/* 1      init_mode		?*/
/* 5:2    bc_size<4:1>		1*/
/* -----------------------------------------------------------------------------*/
/* 6      bc_enable		1	0x23*/
/* 7      bc_enable		1*/
/* 11:8   bc_size<1:4>		8*/
/* -----------------------------------------------------------------------------*/
/* 12     bc_enable		0	0x20*/
/* 13     bc_enable		0*/
/* 14     bc_enable		0*/
/* 15     zeroblk_enable<1>	0*/
/* 16     enable_evict		0*/
/* 17     bc_enable		1*/
/* -----------------------------------------------------------------------------*/
/* 18     zeroblk_enable<0>	1	0x2f*/
/* 19     bc_enable		1*/
/* 20     bc_enable		1*/
/* 21     bc_enable		1*/
/* 22     set_dirty_enable<0>	0*/
/* 23     zeroblk_enable<0>	1*/
/* -----------------------------------------------------------------------------*/
/* 25:24  set_dirty_enable<2:1>	0	0x0*/
/* 25     bc_bank_enable	0*/
/* 29:27  bc_size<2:4>		0*/
/* -----------------------------------------------------------------------------*/
/* 30     bc_size<1>		1	0x3, 0x1 (0x3 w/init_mode, 0x1 without)*/
/* 31     init_mode		?*/
/* 35:32  bc_wrt_sts<0:3>	0*/


/* set dstream SPE bit*/

#if 0
        lda     r0,0x0004(r31)          /* set SPE<1> bit in M_CTL*/
        mtpr	r0,EV6__M_CTL           /* value = 0x0000000000000004 (SCRBRD=6)*/
        addq    r31,r31,r31             /* nop*/
        addq    r31,r31,r31             /* nop*/
#endif

/* set init bit*/

        lda     r0,0x0003(r31)          /* ~V~D~S~P, INIT, SIZE=2M*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 30 to 35*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0000(r31)          /* BC_SIZE=1M, ~BC_BANK_EN, SETDIRTY_EN=0*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 24 to 29*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x002F(r31)          /* ZERO_EN=1, SETDIRTY_EN=0, BC_EN*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 18 to 23*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0020(r31)          /* BC_EN, ~EVICT_EN, ZERO_EN=1*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 12 to 17*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0023(r31)          /* BC_EN, BC_SIZE=2M*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 6 to 11*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0007(r31)          /* BC_EN, BC_SIZE=2M, INIT*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 0 to 5*/
        addq    r31,r31,r31             /* nop*/

	br	r31, endcase            /* return to cmd input routine*/

	.align 6
BcInit2:
/* first write hints for all B & D cache lines */

        mtpr    r31, (EV6__EXC_ADDR+64) /* dummy IPR write - sets SCBD bit 6 (to stall untill ev6_data gets written)*/
        ldah    r20, KSEG_PREFIX(r31)   /* spe<3> kseg space*/
        sll     r20, 16, r20            /* initialize upper address bits*/
        ldah    r20, 0x10(r20)          /* start at VA 1MB*/

        lda     r21, 0x8400(r31)        /* (2MB bcache + 64k dcache)/64 byte cache lines*/
        zap     r21, 0xfc, r21          /* clear sign extension from lda*/
whline: wh64	(r20)                   /* init cache line*/
        lda     r20, 64(r20)            /* add 64 for next cache line*/

        subq    r21, 1, r21             /* decrement line counter*/
        beq     r21, whdone             /* done init?*/
        br      r31, whline             /* keep going if more lines to init*/
whdone:	mb                              /* memory barrier*/

/* second evict the Dcache residue */

        ldah    r20, KSEG_PREFIX(r31)   /* spe<3> kseg space*/
        sll     r20, 16, r20            /* initialize upper address bits*/
        ldah    r20, 0x10(r20)          /* start at VA 1MB*/
        lda     r21, 0x0400(r31)        /* (64k dcache)/64 byte cache lines*/

evline: ecb     (r20)                   /* evict cache line*/
        lda     r20, 64(r20)            /* add 64 for next cache line*/
        subq    r21, 1, r21             /* decrement line counter*/
        beq     r21, evdone             /* done evict?*/

        br      r31, evline             /* keep going if more lines to evict*/
evdone:	mb                              /* memory barrier*/
        addq    r31,r31,r31             /* nop*/
#if 0
        mtpr	r31,EV6__M_CTL          /* clear dstream SPE bit*/
#endif
	br	r31, endcase            /* return to cmd input routine*/

	
	.align 6
BcInit3:
/* clear init bit*/

        lda     r0,0x0001(r31)          /* ~V~D~S~P, ~INIT, SIZE=2M*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 30 to 35*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0000(r31)          /* BC_SIZE=1M, ~BC_BANK_EN, SETDIRTY_EN=0*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 24 to 29*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x002F(r31)          /* ZERO_EN=1, SETDIRTY_EN=0, BC_EN*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 18 to 23*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0027(r31)          /* BC_EN, ~EVICT_EN, ZERO_EN=1*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 12 to 17*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0023(r31)          /* BC_EN, BC_SIZE=2M*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 6 to 11*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0005(r31)          /* BC_EN, BC_SIZE=2M, ~INIT*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 0 to 5*/
        addq    r31,r31,r31             /* nop*/

        mtpr    r31, (EV6__EXC_ADDR+64) /* dummy IPR write - sets SCBD bit 6 (to stall untill ev6_data gets written)*/
        addq    r31,r31,r31             /* nop*/
        addq    r31,r31,r31             /* nop*/
	br	r31, endcase            /* return to cmd input routine*/


/* bits   CSR*/
/* 0      bc_enable		0	0x0 */
/* 1      init_mode		0 */
/* 5:2    bc_size<4:1>		0  */
/* -----------------------------------------------------------------------------*/
/* 6      bc_enable		0	0x0*/
/* 7      bc_enable		0*/
/* 11:8   bc_size<1:4>		0*/
/* -----------------------------------------------------------------------------*/
/* 12     bc_enable		0	0x08*/
/* 13     bc_enable		0*/
/* 14     bc_enable		0*/
/* 15     zeroblk_enable<1>	1*/
/* 16     enable_evict		0*/
/* 17     bc_enable		0*/
/* -----------------------------------------------------------------------------*/
/* 18     zeroblk_enable<0>	1	0x21*/
/* 19     bc_enable		0*/
/* 20     bc_enable		0*/
/* 21     bc_enable		0*/
/* 22     set_dirty_enable<0>	0*/
/* 23     zeroblk_enable<0>	1*/
/* -----------------------------------------------------------------------------*/
/* 25:24  set_dirty_enable<2:1>	1,1	0x3*/
/* 25     bc_bank_enable	0*/
/* 29:27  bc_size<2:4>		0*/
/* -----------------------------------------------------------------------------*/
/* 30     bc_size<1>		0	0
/* 31     init_mode		?*/
/* 35:32  bc_wrt_sts<0:3>	0*/

	
	.align 6
dual_tsunami_no_bcache:
/* clear init bit*/

        lda     r0,0x0000(r31)          /* ~V~D~S~P, ~INIT, SIZE=2M*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 30 to 35*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0003(r31)          /* BC_SIZE=1M, ~BC_BANK_EN, SETDIRTY_EN=0*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 24 to 29*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0021(r31)          /* ZERO_EN=1, SETDIRTY_EN=0, BC_EN*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 18 to 23*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0008(r31)          /* BC_EN, ~EVICT_EN, ZERO_EN=1*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 12 to 17*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0000(r31)          /* BC_EN, BC_SIZE=2M*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 6 to 11*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0000(r31)          /* BC_EN, BC_SIZE=2M, ~INIT*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 0 to 5*/
        addq    r31,r31,r31             /* nop*/

        mtpr    r31, (EV6__EXC_ADDR+64) /* dummy IPR write - sets SCBD bit 6 (to stall untill ev6_data gets written)*/
        addq    r31,r31,r31             /* nop*/
        addq    r31,r31,r31             /* nop*/
	br	r31, endcase            /* return to cmd input routine*/


/* bits   CSR*/
/* 0      bc_enable		1	0x05 */
/* 1      init_mode		0 */
/* 5:2    bc_size<4:1>		1  */
/* -----------------------------------------------------------------------------*/
/* 6      bc_enable		1	0x23*/
/* 7      bc_enable		1*/
/* 11:8   bc_size<1:4>		8*/
/* -----------------------------------------------------------------------------*/
/* 12     bc_enable		1	0x2F*/
/* 13     bc_enable		1*/
/* 14     bc_enable		1*/
/* 15     zeroblk_enable<1>	1*/
/* 16     enable_evict		0*/
/* 17     bc_enable		1*/
/* -----------------------------------------------------------------------------*/
/* 18     zeroblk_enable<0>	1	0x2F*/
/* 19     bc_enable		1*/
/* 20     bc_enable		1*/
/* 21     bc_enable		1*/
/* 22     set_dirty_enable<0>	0*/
/* 23     zeroblk_enable<0>	1*/
/* -----------------------------------------------------------------------------*/
/* 25:24  set_dirty_enable<2:1>	1,1	0x3*/
/* 25     bc_bank_enable	0*/
/* 29:27  bc_size<2:4>		0*/
/* -----------------------------------------------------------------------------*/
/* 30     bc_size<1>		1	0x1 */
/* 31     init_mode		?*/
/* 35:32  bc_wrt_sts<0:3>	0*/


	
	.align 6
dual_tsunami_bcache:	
/* clear init bit*/

        lda     r0,0x0001(r31)          /* ~V~D~S~P, ~INIT, SIZE=2M*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 30 to 35*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0003(r31)          /* BC_SIZE=1M, ~BC_BANK_EN, SETDIRTY_EN=0*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 24 to 29*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x002F(r31)          /* ZERO_EN=1, SETDIRTY_EN=0, BC_EN*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 18 to 23*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x002F(r31)          /* BC_EN, ~EVICT_EN, ZERO_EN=1*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 12 to 17*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0023(r31)          /* BC_EN, BC_SIZE=2M*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 6 to 11*/
        addq    r31,r31,r31             /* nop*/

        lda     r0,0x0005(r31)          /* BC_EN, BC_SIZE=2M, ~INIT*/
        addq    r31,r31,r31             /* nop*/
        mtpr	r0,EV6__DATA            /* shift in bits 0 to 5*/
        addq    r31,r31,r31             /* nop*/

        mtpr    r31, (EV6__EXC_ADDR+64) /* dummy IPR write - sets SCBD bit 6 (to stall untill ev6_data gets written)*/
        addq    r31,r31,r31             /* nop*/
        addq    r31,r31,r31             /* nop*/
	br	r31, endcase            /* return to cmd input routine*/
	
#include "serialio.s"
#include "sromio.s"
	bis	r31,r31,r31
	bis	r31,r31,r31

.align 6
end:
	.data
#else /* DC21264 */

#include "serialio.s"
#include "sromio.s"

#endif /* DC21264 (else block) */
#endif /* USE_OLD_MINI_DEBUGGER (else block) */

