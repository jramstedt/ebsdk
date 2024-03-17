/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
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
.data
#ifdef __linux__
.asciz "$Id: kutil.s,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $"
#else
.asciiz "$Id: kutil.s,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $"
#endif
.text
#endif

/*
 * $Log: kutil.s,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.18  1997/12/15  20:45:52  pbell
 * Updated for dp264.
 *
 * Revision 1.17  1997/06/10  18:18:18  fdh
 * Removed some obsoleted functions.
 *
 * Revision 1.16  1997/04/10  17:58:53  fdh
 * Modified to have no assembler use of the assembler
 * temporary register (at).
 * Added the .asciz directive for linux.
 *
 * Revision 1.15  1997/02/26 21:34:18  fdh
 * Modified dbgentry and dbgint to save floating point register
 * state from an emulated floating point register array when
 * compiled to emulate floating point instructions.
 *
 * Revision 1.14  1997/02/20  18:59:47  fdh
 * Modified dbgtrap to save state on the stack rather than in
 * the breakpoint save state area.  This facilitates debugging,
 * setting breakpoints, within the trap handling procedures.
 *
 * Added conditions to disable use of floating point instructions
 * when using IEEE emulation code for performing all floating point
 * operations when the CPU's floating point unit is disabled.
 * To avoid infinite recursion the floating point emulation
 * path can't contain code which would cause traps to the floating
 * point emulation.
 *
 * Revision 1.13  1996/05/22  21:50:36  fdh
 * Removed kgetsp, and kwrmces.
 *
 * Revision 1.12  1995/02/24  16:07:51  fdh
 * Modified to use the regdefs.h and paldefs.h include files.
 *
 * Revision 1.11  1994/11/24  04:03:02  fdh
 * Removed dc21064.h include file.
 *
 * Revision 1.10  1994/11/22  23:47:26  fdh
 * Don't include PALcode include files for make depend.
 *
 * Revision 1.9  1994/08/05  20:17:45  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.8  1994/06/29  16:16:22  berent
 * Added reporting of unexpected traps and exceptions.  Also
 * added   clearing of machine checks before the monior attempts
 * to continue from a machine check
 *
 * Revision 1.7  1994/06/20  18:00:59  fdh
 * Modified to use new ANSI compliant PALcode definitions.
 *
 * Revision 1.6  1994/04/10  03:35:11  fdh
 * Make sure that floating point is enabled incase
 * it was disabled by the user program.
 *
 * Revision 1.5  1994/04/08  04:41:01  fdh
 * Added dc21064 include file.
 *
 * Revision 1.4  1994/03/09  12:48:33  berent
 * Fixed for NT compilation
 *
 * Revision 1.3  1994/01/19  11:05:05  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.2  1993/10/03  00:22:06  berent
 * Merge in development by Anthony Berent
 *
 *>> Revision 1.3  1993/10/01  15:48:01  berent
 *>> Rewrote interrupt function to remove need for special ethernet PAL code
 *>>
 *>> Revision 1.2  1993/08/09  11:50:56  berent
 *>> Added a print current sp function for use in debugging the monitor.
 *
 * Revision 1.1  1993/06/08  19:56:36  fdh
 * Initial revision
 *
 */


#include "palosf.h"
#include "paldefs.h"
#include "regdefs.h"
#include "fregs.h"

#define ENTBRK 6

	.set noat
	.globl saved_user_pc
	.text
	.align 4
	.ent dbgentry
	/* Debug entry point from PAL code following DBGSTOP PAL call;
	original PS, PC, GP(r29) and a0 to a2 (r16 to r18) are on the stack */
dbgentry:
	/* Set up a2 to the current address */
	br a2,dbgentry2
dbgentry2:
	/* Set up the global pointer */
	ldgp gp,0(a2)

	/* Create some stack space */
	lda sp,-8(sp)
	.frame sp,56,ra
	.prologue 0	

	/* Save the assembler temporary register in a2 before doing anything that will use it */	
	bis AT,0,a2
	ldq a1,SavedIntegerRegisters

	/* Do the registers that were not saved by PAL code first */
	stq v0,0*8(a1)
	stq t0,1*8(a1)
	stq t1,2*8(a1)
	stq t2,3*8(a1)
	stq t3,4*8(a1)
	stq t4,5*8(a1)
	stq t5,6*8(a1)
	stq t6,7*8(a1)
	stq t7,8*8(a1)
	stq s0,9*8(a1)
	stq s1,10*8(a1)
	stq s2,11*8(a1)
	stq s3,12*8(a1)
	stq s4,13*8(a1)
	stq s5,14*8(a1)
	stq s6,15*8(a1)
	stq a3,19*8(a1)
	stq a4,20*8(a1)
	stq a5,21*8(a1)
	stq t8,22*8(a1)
	stq t9,23*8(a1)
	stq t10,24*8(a1)
	stq t11,25*8(a1)
	stq ra,26*8(a1)
	stq t12,27*8(a1)
	/* Register 31 is always 0; store it anyway */
	stq zero,31*8(a1)

#ifdef __EMULATE_FLOATING_POINT
	/*
	** Save the state of the emulated FP registers
	** Note that when compiled with __EMULATE_FLOATING_POINT
	** breakpoints require that the emulator handle
	** the floating point instructions to have the
	** emulated register array updated.  This is
	** necessary even when there is a perfectly good
	** floating point unit available in hardware.
	** Thus it must be disabled, Otherwise, there is
	** no state available to save here.
	*/
	lda t1,EmulatedFloatingPointRegisters
	ldq t0,SavedFloatingPointRegisters
	lda t2,32(zero)		/* Load up counter */
brksavefp:	
	subq t2,1,t2		/* Decrement counter */
	ldq t3,0(t1)		/* load up register  */
	lda t1,8(t1)		/* increment pointer */
	stq t3,0(t0)		/* store register    */
	lda t0,8(t0)		/* increment pointer */
	bne t2, brksavefp	/* break if done     */
	
#else	/* __EMULATE_FLOATING_POINT */
/*
 * Make sure that floating point is enabled incase
 * it was disabled by the user program.
 */
	bis a0, zero, t1
	lda a0, 1(zero)
	call_pal PAL_WRFEN_ENTRY
	bis t1, zero, a0

	ldq a1,SavedFloatingPointRegisters
	
	/* Save the floating point registers */
	stt f0,0*8(a1)
	stt f1,1*8(a1)
	stt f2,2*8(a1)
	stt f3,3*8(a1)
	stt f4,4*8(a1)
	stt f5,5*8(a1)
	stt f6,6*8(a1)
	stt f7,7*8(a1)
	stt f8,8*8(a1)
	stt f9,9*8(a1)
	stt f10,10*8(a1)
	stt f11,11*8(a1)
	stt f12,12*8(a1)
	stt f13,13*8(a1)
	stt f14,14*8(a1)
	stt f15,15*8(a1)
	stt f16,16*8(a1)
	stt f17,17*8(a1)
	stt f18,18*8(a1)
	stt f19,19*8(a1)
	stt f20,20*8(a1)
	stt f21,21*8(a1)
	stt f22,22*8(a1)
	stt f23,23*8(a1)
	stt f24,24*8(a1)
	stt f25,25*8(a1)
	stt f26,26*8(a1)
	stt f27,27*8(a1)
	stt f28,28*8(a1)
	stt f29,29*8(a1)
	stt f30,30*8(a1)
	stt f31,31*8(a1)
#endif /* __EMULATE_FLOATING_POINT */

	ldq a1,SavedIntegerRegisters

	/* Registers 16 to 18 have to be got from the stack */
	ldq v0,24+8(sp)
	stq v0,16*8(a1)
	ldq v0,32+8(sp)
	stq v0,17*8(a1)
	ldq v0,40+8(sp)
	stq v0,18*8(a1)

	/* Register 28 (at) was saved in a2 */
	stq a2,28*8(a1)
	/* Register 29 is the global pointer; get it off the stack */
	ldq v0,FRM_Q_GP+8(sp)
	stq v0,29*8(a1)

	/* For register 30 (the stack pointer) I have to work out whether */
	/* the debugee was running in user or kernel mode */
	ldq v0,FRM_Q_PS+8(sp) 		/* Get the original processor status word */
	and v0,0x08,v0	/* Extract mode */
	bne v0,user_process

	/* The debugee is running in kernel mode so calculate it from the current stack pointer */
	lda v0,48+8(sp)
	stq v0,30*8(a1)
	br zero,user_process_end

user_process:
	/* User mode; read the user stack pointer */
	call_pal PAL_RDUSP_ENTRY
	stq v0,30*8(a1)

user_process_end:
	/* Store the saved PC */
	ldq a1,SavedExceptionAddressRegister
	ldq v0,FRM_Q_PC+8(sp)
	stq v0,0(a1)
	
	/* Store the PS; */
	ldq v0,FRM_Q_PS+8(sp)
	lda a1,SavedProcessorStatusRegister
	stq v0,0(a1)

	/* Breakpoint */
	lda pv,kreenter
	jsr ra,(t12),kreenter

	/* We have returned from the breakpoint routine; put everything back */

	/* Restore the global pointer */
	ldgp gp,0(ra)

	/* Restore the stack pointer here since at this point there is nothing I need on the stack so I */
	/* don't have to worry about moving the stack frame (the user may have things of value on the stack */
	/* but I have to assume that he knows what he is doing) */

	/* I have to work out whether the debugee was running in user or kernel mode */
	ldq v0,FRM_Q_PS+8(sp) 		/* Get the original processor status word */
	and v0,0x08,v0	/* Extract mode */
	bne v0,restore_user_process

	ldq a1,SavedIntegerRegisters

	/* The debugee is running in kernel mode so I have to change the current stack pointer */
	ldq v0,30*8(a1)
	lda sp,-48-8(v0)
	br zero,restore_user_process_end

restore_user_process:
	/* User mode; write the user stack pointer */
	ldq a0,30*8(a1)
	call_pal PAL_WRUSP_ENTRY

restore_user_process_end:
	
	/* GP */
	ldq v0,29*8(a1)
	stq v0,FRM_Q_GP+8(sp)

	/* PS */
	ldq v0,SavedProcessorStatusRegister
	stq v0,FRM_Q_PS+8(sp)

	/* PC */
	ldq a1,SavedExceptionAddressRegister
	ldq v0,0(a1)
	stq v0,FRM_Q_PC+8(sp)

#ifdef __EMULATE_FLOATING_POINT
	/* Restore the state of the emulated FP registers */
	lda t1,EmulatedFloatingPointRegisters
	ldq t0,SavedFloatingPointRegisters
	lda t2,32(zero)		/* Load up counter */
brkloadfp:	
	subq t2,1,t2		/* Decrement counter */
	ldq t3,0(t0)		/* load up register  */
	lda t0,8(t0)		/* increment pointer */
	stq t3,0(t1)		/* store register    */
	lda t1,8(t1)		/* increment pointer */
	bne t2,brkloadfp	/* break if done     */
	
#else	/* __EMULATE_FLOATING_POINT */

	/* Restore the floating point registers */
	ldq a1,SavedFloatingPointRegisters
	
	ldt f0,0*8(a1)
	ldt f1,1*8(a1)
	ldt f2,2*8(a1)
	ldt f3,3*8(a1)
	ldt f4,4*8(a1)
	ldt f5,5*8(a1)
	ldt f6,6*8(a1)
	ldt f7,7*8(a1)
	ldt f8,8*8(a1)
	ldt f9,9*8(a1)
	ldt f10,10*8(a1)
	ldt f11,11*8(a1)
	ldt f12,12*8(a1)
	ldt f13,13*8(a1)
	ldt f14,14*8(a1)
	ldt f15,15*8(a1)
	ldt f16,16*8(a1)
	ldt f17,17*8(a1)
	ldt f18,18*8(a1)
	ldt f19,19*8(a1)
	ldt f20,20*8(a1)
	ldt f21,21*8(a1)
	ldt f22,22*8(a1)
	ldt f23,23*8(a1)
	ldt f24,24*8(a1)
	ldt f25,25*8(a1)
	ldt f26,26*8(a1)
	ldt f27,27*8(a1)
	ldt f28,28*8(a1)
	ldt f29,29*8(a1)
	ldt f30,30*8(a1)
	ldt f31,31*8(a1)
#endif /* __EMULATE_FLOATING_POINT */
	
	ldq a1,SavedIntegerRegisters
	/* Registers 16 to 18 have to be put back on the stack */
	ldq v0,16*8(a1)
	stq v0,FRM_Q_A0+8(sp)
	ldq v0,17*8(a1)
	stq v0,FRM_Q_A1+8(sp)
	ldq v0,18*8(a1)
	stq v0,FRM_Q_A2+8(sp)

	/* Restore the remaining integer registers */
	ldq v0,0*8(a1)
	ldq t0,1*8(a1)
	ldq t1,2*8(a1)
	ldq t2,3*8(a1)
	ldq t3,4*8(a1)
	ldq t4,5*8(a1)
	ldq t5,6*8(a1)
	ldq t6,7*8(a1)
	ldq t7,8*8(a1)
	ldq s0,9*8(a1)
	ldq s1,10*8(a1)
	ldq s2,11*8(a1)
	ldq s3,12*8(a1)
	ldq s4,13*8(a1)
	ldq s5,14*8(a1)
	ldq s6,15*8(a1)
	ldq a3,19*8(a1)
	ldq a4,20*8(a1)
	ldq a5,21*8(a1)
	ldq t8,22*8(a1)
	ldq t9,23*8(a1)
	ldq t10,24*8(a1)
	ldq t11,25*8(a1)
	ldq ra,26*8(a1)
	ldq t12,27*8(a1)
	ldq AT,28*8(a1)

	/* Remove this function's stack frame */
	lda sp,8(sp)

	/* All done; exit by calling return from interrupt */

	call_pal PAL_RTI_ENTRY
	.end dbgentry
	.lcomm trap_counter 8
	/* Debug entry points for unexpected traps; this entry point will be called for all conditions for
	which neither the OS nor the debugger has any other entry point */
	.text
	.align 4
	.ent dbgarith
dbgarith:
	/* Set reason for stopping as reason -1 */
	lda gp,1(zero)
	/* Go to common trap handling code */
	br zero,dbgtrap
	.end dbgarith	

	.text
	.align 4
	.ent dbgmm
dbgmm:
	/* Set reason for stopping as reason -1 */
	lda gp,2(zero)
	/* Go to common trap handling code */
	br zero,dbgtrap
	.end dbgmm	

	.text
	.align 4
	.ent dbgif
dbgif:
	/* Set reason for stopping as reason -1 */
	lda gp,3(zero)
	/* Go to common trap handling code */
	br zero,dbgtrap
	.end dbgif	

	.text
	.align 4
	.ent dbguna
dbguna:
	/* Set reason for stopping as reason -1 */
	lda gp,4(zero)
	/* Go to common trap handling code */
	br zero,dbgtrap
	.end dbguna	

	.text
	.align 4
	.ent dbgsys
dbgsys:
	/* Set reason for stopping as reason -1 */
	lda gp,5(zero)
	/* Go to common trap handling code */
	br zero,dbgtrap
	.end dbgsys	

	/*
	 * TFRM_SIZE allocates space for...
	 *	 6 - Up to 6 trap parameters.
	 *	32 - General purpose registers.
	 *	32 - Floating point registers.
	 *	 1 - Process counter
	 *	 1 - Processor Status
	 */
#define TFRM_SIZE ((6+32+32+2)*8)
	
/* Generic trap function; trap type is in g0, trap arguments in a0 to a2 */
	.text
	.align 4
	.ent dbgtrap
dbgtrap:
	/* Create some stack space */
	lda sp,-TFRM_SIZE(sp)

	/* Save the trap type and parameters on the stack */
	stq gp,0*8(sp)
	stq a0,1*8(sp)
	stq a1,2*8(sp)
	stq a2,3*8(sp)

	/* Set up a2 to the current address */
	br a2,dbgtrap2
dbgtrap2:
	/* Set up the global pointer */
	ldgp gp,0(a2)

	.frame sp,FRM_K_SIZE+TFRM_SIZE,ra
	.prologue 0	

	/* Save the assembler temporary register in a2 before doing anything that will use it */	
	bis AT,0,a2

	/* Do the registers that were not saved by PAL code first */
	stq v0,(0+6)*8(sp)
	stq t0,(1+6)*8(sp)
	stq t1,(2+6)*8(sp)
	stq t2,(3+6)*8(sp)
	stq t3,(4+6)*8(sp)
	stq t4,(5+6)*8(sp)
	stq t5,(6+6)*8(sp)
	stq t6,(7+6)*8(sp)
	stq t7,(8+6)*8(sp)
	stq s0,(9+6)*8(sp)
	stq s1,(10+6)*8(sp)
	stq s2,(11+6)*8(sp)
	stq s3,(12+6)*8(sp)
	stq s4,(13+6)*8(sp)
	stq s5,(14+6)*8(sp)
	stq s6,(15+6)*8(sp)
	stq a3,(19+6)*8(sp)
	stq a4,(20+6)*8(sp)
	stq a5,(21+6)*8(sp)
	stq t8,(22+6)*8(sp)
	stq t9,(23+6)*8(sp)
	stq t10,(24+6)*8(sp)
	stq t11,(25+6)*8(sp)
	stq ra,(26+6)*8(sp)
	stq t12,(27+6)*8(sp)
	/* Register 31 is always 0; store it anyway */
	stq zero,(31+6)*8(sp)

#ifndef __EMULATE_FLOATING_POINT
/*
 * Make sure that floating point is enabled incase
 * it was disabled by the user program.
 */
	bis a0, zero, t1
	lda a0, 1(zero)
	call_pal PAL_WRFEN_ENTRY
	bis t1, zero, a0

	/* Save the floating point registers */
	stt f0,(0+6+32)*8(sp)
	stt f1,(1+6+32)*8(sp)
	stt f2,(2+6+32)*8(sp)
	stt f3,(3+6+32)*8(sp)
	stt f4,(4+6+32)*8(sp)
	stt f5,(5+6+32)*8(sp)
	stt f6,(6+6+32)*8(sp)
	stt f7,(7+6+32)*8(sp)
	stt f8,(8+6+32)*8(sp)
	stt f9,(9+6+32)*8(sp)
	stt f10,(10+6+32)*8(sp)
	stt f11,(11+6+32)*8(sp)
	stt f12,(12+6+32)*8(sp)
	stt f13,(13+6+32)*8(sp)
	stt f14,(14+6+32)*8(sp)
	stt f15,(15+6+32)*8(sp)
	stt f16,(16+6+32)*8(sp)
	stt f17,(17+6+32)*8(sp)
	stt f18,(18+6+32)*8(sp)
	stt f19,(19+6+32)*8(sp)
	stt f20,(20+6+32)*8(sp)
	stt f21,(21+6+32)*8(sp)
	stt f22,(22+6+32)*8(sp)
	stt f23,(23+6+32)*8(sp)
	stt f24,(24+6+32)*8(sp)
	stt f25,(25+6+32)*8(sp)
	stt f26,(26+6+32)*8(sp)
	stt f27,(27+6+32)*8(sp)
	stt f28,(28+6+32)*8(sp)
	stt f29,(29+6+32)*8(sp)
	stt f30,(30+6+32)*8(sp)
	stt f31,(31+6+32)*8(sp)
#endif /* __EMULATE_FLOATING_POINT */

	/* Registers 16 to 18 have to be got from the stack */
	ldq v0,FRM_Q_A0+TFRM_SIZE(sp)
	stq v0,(16+6)*8(sp)
	ldq v0,FRM_Q_A1+TFRM_SIZE(sp)
	stq v0,(17+6)*8(sp)
	ldq v0,FRM_Q_A2+TFRM_SIZE(sp)
	stq v0,(18+6)*8(sp)

	/* Register 28 (at) was saved in a2 */
	stq a2,(28+6)*8(sp)
	/* Register 29 is the global pointer; get it off the stack */
	ldq v0,FRM_Q_GP+TFRM_SIZE(sp)
	stq v0,(29+6)*8(sp)

	/* For register 30 (the stack pointer) I have to work out whether */
	/* the debugee was running in user or kernel mode */
	ldq v0,FRM_Q_PS+TFRM_SIZE(sp) 		/* Get the original processor status word */
	and v0,0x08,v0	/* Extract mode */
	bne v0,trap_user_process

	/* The debugee is running in kernel mode so calculate it from the current stack pointer */
	lda v0,FRM_K_SIZE+TFRM_SIZE(sp)
	stq v0,(30+6)*8(sp)
	br zero,trap_user_process_end

trap_user_process:
	/* User mode; read the user stack pointer */
	call_pal PAL_RDUSP_ENTRY
	stq v0,(30+6)*8(sp)

trap_user_process_end:
	/* Store the saved PC */
	ldq v0,FRM_Q_PC+TFRM_SIZE(sp)
	stq v0,(0+6+32+32)*8(sp)
	
	/* Store the PS; */
	ldq v0,FRM_Q_PS+TFRM_SIZE(sp)
	stq v0,(1+6+32+32)*8(sp)
	
	/* Get the arguments for the trap function off the stack */
	ldq a0,0*8(sp)
	ldq a1,1*8(sp)
	ldq a2,2*8(sp)
	ldq a3,3*8(sp)
	lda a4,6*8(sp)	/* Pointer to save state area */

	/* Throw a return address on the current stack frame to fool the debugger */
	subq ra, 4, ra
	stq ra,0*8(sp)

	/* Call trap function */
	lda pv,ktrap
	jsr ra,(t12),ktrap

	/* We have returned from the breakpoint routine; put everything back */

	/* Restore the global pointer */
	ldgp gp,0(ra)

	/* Restore the stack pointer here since at this point there is nothing I need on the stack so I */
	/* don't have to worry about moving the stack frame (the user may have things of value on the stack */
	/* but I have to assume that he knows what he is doing) */

	/* I have to work out whether the debugee was running in user or kernel mode */
	ldq v0,FRM_Q_PS+TFRM_SIZE(sp) 		/* Get the original processor status word */
	and v0,0x08,v0	/* Extract mode */
	bne v0,trap_restore_user_process

	/* The debugee is running in kernel mode so I have to change the current stack pointer */
	ldq v0,(30+6)*8(sp)
	lda sp,-(FRM_K_SIZE+TFRM_SIZE)(v0)
	br zero,trap_restore_user_process_end

trap_restore_user_process:
	/* User mode; write the user stack pointer */
	ldq a0,(30+6)*8(sp)
	call_pal PAL_WRUSP_ENTRY

trap_restore_user_process_end:
	
	/* GP */
	ldq v0,(29+6)*8(sp)
	stq v0,FRM_Q_GP+TFRM_SIZE(sp)

	/* PS */
	ldq v0,(1+6+32+32)*8(sp)
	stq v0,FRM_Q_PS+TFRM_SIZE(sp)

	/* PC */
	ldq v0,(0+6+32+32)*8(sp)
	stq v0,FRM_Q_PC+TFRM_SIZE(sp)

#ifndef __EMULATE_FLOATING_POINT

	/* Restore the floating point registers */
	ldt f0,(0+6+32)*8(sp)
	ldt f1,(1+6+32)*8(sp)
	ldt f2,(2+6+32)*8(sp)
	ldt f3,(3+6+32)*8(sp)
	ldt f4,(4+6+32)*8(sp)
	ldt f5,(5+6+32)*8(sp)
	ldt f6,(6+6+32)*8(sp)
	ldt f7,(7+6+32)*8(sp)
	ldt f8,(8+6+32)*8(sp)
	ldt f9,(9+6+32)*8(sp)
	ldt f10,(10+6+32)*8(sp)
	ldt f11,(11+6+32)*8(sp)
	ldt f12,(12+6+32)*8(sp)
	ldt f13,(13+6+32)*8(sp)
	ldt f14,(14+6+32)*8(sp)
	ldt f15,(15+6+32)*8(sp)
	ldt f16,(16+6+32)*8(sp)
	ldt f17,(17+6+32)*8(sp)
	ldt f18,(18+6+32)*8(sp)
	ldt f19,(19+6+32)*8(sp)
	ldt f20,(20+6+32)*8(sp)
	ldt f21,(21+6+32)*8(sp)
	ldt f22,(22+6+32)*8(sp)
	ldt f23,(23+6+32)*8(sp)
	ldt f24,(24+6+32)*8(sp)
	ldt f25,(25+6+32)*8(sp)
	ldt f26,(26+6+32)*8(sp)
	ldt f27,(27+6+32)*8(sp)
	ldt f28,(28+6+32)*8(sp)
	ldt f29,(29+6+32)*8(sp)
	ldt f30,(30+6+32)*8(sp)
	ldt f31,(31+6+32)*8(sp)
#endif /* __EMULATE_FLOATING_POINT */
	
	/* Registers 16 to 18 have to be put back on the stack */
	ldq v0,(16+6)*8(sp)
	stq v0,FRM_Q_A0+TFRM_SIZE(sp)
	ldq v0,(17+6)*8(sp)
	stq v0,FRM_Q_A1+TFRM_SIZE(sp)
	ldq v0,(18+6)*8(sp)
	stq v0,FRM_Q_A2+TFRM_SIZE(sp)

	/* Restore the remaining integer registers */
	ldq v0,(0+6)*8(sp)
	ldq t0,(1+6)*8(sp)
	ldq t1,(2+6)*8(sp)
	ldq t2,(3+6)*8(sp)
	ldq t3,(4+6)*8(sp)
	ldq t4,(5+6)*8(sp)
	ldq t5,(6+6)*8(sp)
	ldq t6,(7+6)*8(sp)
	ldq t7,(8+6)*8(sp)
	ldq s0,(9+6)*8(sp)
	ldq s1,(10+6)*8(sp)
	ldq s2,(11+6)*8(sp)
	ldq s3,(12+6)*8(sp)
	ldq s4,(13+6)*8(sp)
	ldq s5,(14+6)*8(sp)
	ldq s6,(15+6)*8(sp)
	ldq a3,(19+6)*8(sp)
	ldq a4,(20+6)*8(sp)
	ldq a5,(21+6)*8(sp)
	ldq t8,(22+6)*8(sp)
	ldq t9,(23+6)*8(sp)
	ldq t10,(24+6)*8(sp)
	ldq t11,(25+6)*8(sp)
	ldq ra,(26+6)*8(sp)
	ldq t12,(27+6)*8(sp)
	ldq AT,(28+6)*8(sp)

	/* Remove this function's stack frame */
	lda sp,TFRM_SIZE(sp)

	/* All done; exit by calling return from interrupt */

	call_pal PAL_RTI_ENTRY
	.end dbgtrap	

	.text
	.align 4
	.ent dbgint
	/* Interrupt entry point from PAL code, a0 is 0 if at a breakpoint, 1 if input to handle;
	original PS, PC, GP(r29) and a0 to a2 (r16 to r18) are on the stack */
dbgint:
	/* Set up global pointer; I can't use the global pointer set up by the PAL code since
	 * this may have been set by the application through a call to the wrkgp PAL function */
	br gp,dbgint1
dbgint1:
	ldgp gp,0(gp)
	
	/* I need space on the stack to save the registers that can be corrupted by called 
	 * functions and have not already been saved by PAL code.  These are a3 to a5, v0, 
	 * t0 to t12, ra, at, f0, f1, and f10 to f30
	 */
	lda sp, -42*8(sp)
	.frame sp,(41+6)*8,ra
	.prologue 0	
	
	/* Now save the registers */
	stq a3,0*8(sp)
	stq a4,1*8(sp)
	stq a5,2*8(sp)
	stq v0,3*8(sp)
	stq t0,4*8(sp)
	stq t1,5*8(sp)
	stq t2,6*8(sp)
	stq t3,7*8(sp)
	stq t4,8*8(sp)
	stq t5,9*8(sp)
	stq t6,10*8(sp)
	stq t7,11*8(sp)
	stq t8,12*8(sp)
	stq t9,13*8(sp)
	stq t10,14*8(sp)
	stq t11,15*8(sp)
	stq t12,16*8(sp)
	stq ra,17*8(sp)
	stq AT,18*8(sp)

#ifdef __EMULATE_FLOATING_POINT
	/* Save the state of the emulated FP registers */
	lda t1,EmulatedFloatingPointRegisters
	lda t0,19*8(sp)

	/* Save f0 */
	ldq t3,0*8(t1)		/* load up f0        */	
	stq t3,0*8(t0)		/* store f0          */

	/* Save f1 */
	ldq t3,1*8(t1)		/* load up f1        */	
	stq t3,1*8(t0)		/* store f1          */
	
	/* Now save f10 - f30 */
	lda t2,(30-10+1)(zero)	/* Load up counter */
	lda t1,10*8(t1)		/* Point to f10    */
	lda t0,2*8(t0)		/* Point to f10    */
intsavefp:	
	subq t2,1,t2		/* Decrement counter */
	ldq t3,0(t1)		/* load up register  */
	lda t1,8(t1)		/* increment pointer */
	stq t3,0(t0)		/* store register    */
	lda t0,8(t0)		/* increment pointer */
	bne t2, intsavefp	/* break if done     */
	
#else	/* __EMULATE_FLOATING_POINT */
/*
 * Make sure that floating point is enabled incase
 * it was disabled by the user program.
 */
	bis a0, zero, t1
	lda a0, 1(zero)
	call_pal PAL_WRFEN_ENTRY
	bis t1, zero, a0

	/* Save the floating point registers */
	stt f0,19*8(sp)
	stt f1,20*8(sp)
	stt f10,21*8(sp)
	stt f11,22*8(sp)
	stt f12,23*8(sp)
	stt f13,24*8(sp)
	stt f14,25*8(sp)
	stt f15,26*8(sp)
	stt f16,27*8(sp)
	stt f17,28*8(sp)
	stt f18,29*8(sp)
	stt f19,30*8(sp)
	stt f20,31*8(sp)
	stt f21,32*8(sp)
	stt f22,33*8(sp)
	stt f23,34*8(sp)
	stt f24,35*8(sp)
	stt f25,36*8(sp)
	stt f26,37*8(sp)
	stt f27,38*8(sp)
	stt f28,39*8(sp)
	stt f29,40*8(sp)
	stt f30,41*8(sp)
#endif /* __EMULATE_FLOATING_POINT */

	/* I want to save the PC of the application in a global used by the debugger.  
         * This needs care since the interrupt I am handling now may have interrupted another 
	 * interrupt */
	
	/* Check whether the PC has already been saved */
	ldq t0,saved_user_pc
	bne t0,pcsaved
	
	/* The PC has not already been saved; either this is the outermost interrupt or I have
	 * interrupted the interrupt routine before it had saved the PC. Determine which by 
	 * examiming the saved PC */
	ldq t0,43*8(sp)
	lda t1,dbgint
	subq t0,t1,t2
	blt t2,savepc
	lda t1,pcsaved
	subq t0,t1,t2
	blt t2,pcsaved
savepc:
	/* This really is the first interrupt; save the pc for the debug server */
	lda t2,saved_user_pc
	stq t0,0(t2)
pcsaved:
	/* Call the C interrupt handler; the interrupt data is in a0 to a2 so will be
	 * passed as arguments */
	lda pv,data_received
	jsr ra,(pv),data_received

	/* Restore the global pointer */
	ldgp gp,0(ra)

	/* Returned from C interrupt routine; disable other interrupts to avoid the sort of
	 * messing around I had to do at the start of the routine */
	lda a0,7(zero)
	call_pal PAL_SWPIPL_ENTRY
	
	/* Now work out if this interrupt was the outermost interrupt; do this by comparing
	 * the PC saved in the global with the PC saved by the PAL code on the stack */
	ldq t0,43*8(sp)
	ldq t1,saved_user_pc
	subq t0,t1,t2
	bne t2,restore_registers
	
	/* This is the outermost interrupt; clear the saved PC */
	lda t1,saved_user_pc
	stq t2,0(t1)

restore_registers:	/* Restore the saved registers from the stack */

#ifdef __EMULATE_FLOATING_POINT
	/* Restore the state of the emulated FP registers */
	lda t1,EmulatedFloatingPointRegisters
	lda t0,19*8(sp)

	/* Restore f0 */
	ldq t3,0*8(t0)		/* load up f0        */	
	stq t3,0*8(t1)		/* store f0          */

	/* Restore f1 */
	ldq t3,1*8(t0)		/* load up f1        */	
	stq t3,1*8(t1)		/* store f1          */
	
	/* Now save f10 - f30 */
	lda t2,(30-10+1)(zero)	/* Load up counter */
	lda t1,10*8(t1)		/* Point to f10    */
	lda t0,2*8(t0)		/* Point to f10    */
intloadfp:	
	subq t2,1,t2		/* Decrement counter */
	ldq t3,0(t0)		/* load up register  */
	lda t0,8(t0)		/* increment pointer */
	stq t3,0(t1)		/* store register    */
	lda t1,8(t1)		/* increment pointer */
	bne t2, intloadfp	/* break if done     */
	
#else	/* __EMULATE_FLOATING_POINT */

	/* Restore the floating point registers */
	ldt f0,19*8(sp)
	ldt f1,20*8(sp)
	ldt f10,21*8(sp)
	ldt f11,22*8(sp)
	ldt f12,23*8(sp)
	ldt f13,24*8(sp)
	ldt f14,25*8(sp)
	ldt f15,26*8(sp)
	ldt f16,27*8(sp)
	ldt f17,28*8(sp)
	ldt f18,29*8(sp)
	ldt f19,30*8(sp)
	ldt f20,31*8(sp)
	ldt f21,32*8(sp)
	ldt f22,33*8(sp)
	ldt f23,34*8(sp)
	ldt f24,35*8(sp)
	ldt f25,36*8(sp)
	ldt f26,37*8(sp)
	ldt f27,38*8(sp)
	ldt f28,39*8(sp)
	ldt f29,40*8(sp)
	ldt f30,41*8(sp)
#endif /* __EMULATE_FLOATING_POINT */

	/* Restore the integer registers */
	ldq a3,0*8(sp)
	ldq a4,1*8(sp)
	ldq a5,2*8(sp)
	ldq v0,3*8(sp)
	ldq t0,4*8(sp)
	ldq t1,5*8(sp)
	ldq t2,6*8(sp)
	ldq t3,7*8(sp)
	ldq t4,8*8(sp)
	ldq t5,9*8(sp)
	ldq t6,10*8(sp)
	ldq t7,11*8(sp)
	ldq t8,12*8(sp)
	ldq t9,13*8(sp)
	ldq t10,14*8(sp)
	ldq t11,15*8(sp)
	ldq t12,16*8(sp)
	ldq ra,17*8(sp)
	ldq AT,18*8(sp)

	/* Restore the stack */
	lda sp,42*8(sp)

	/* And end the interrupt */
	call_pal PAL_RTI_ENTRY	
 	
	.end dbgint

	.text
	.align 4
	.globl kutilinitbreaks
	.ent kutilinitbreaks
kutilinitbreaks:
	ldgp gp,0(t12)
	.frame sp,0,ra
	/* This doesn't corrupt anything that is defined to be preserved across procedure calls */
	/* so no stack frame is needed */
	
	/* Set up the break point handling */
	lda a1,ENTBRK(zero)
	lda a0,dbgentry
	call_pal PAL_WRENT_ENTRY

	/* Set up interrupt call back */
	lda a0,dbgint
	lda a1,0(zero)
	call_pal PAL_WRENT_ENTRY

	/* Set all other traps to go to the default debug traps; the OS will probably
	 reset these once it gets started */
	lda a0,dbgarith
	lda a1,1(zero)
	call_pal PAL_WRENT_ENTRY
	lda a0,dbgmm
	lda a1,2(zero)
	call_pal PAL_WRENT_ENTRY
	lda a0,dbgif
	lda a1,3(zero)
	call_pal PAL_WRENT_ENTRY
	lda a0,dbguna
	lda a1,4(zero)
	call_pal PAL_WRENT_ENTRY
	lda a0,dbgsys
	lda a1,5(zero)
	call_pal PAL_WRENT_ENTRY

	ret zero,(ra)
	.end kutilinitbreaks
