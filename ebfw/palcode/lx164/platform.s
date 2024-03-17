/*
 *      VID: [2.0] PT: [Thu Aug 21 16:25:42 1997] SF: [platform.s]
 *       TI: [/sae_share/apps/bin/vice -iplatform.s -l// -p# -DEB164 -DLX164 -h -m -Xlint -XDEBUG -XPAL_K_REV -XKDEBUG -XDISABLE_CRD -XDISABLE_MACHINE_CHECKS -XSROM_SERIAL_PORT -XCONSOLE_ENTRY -alx164 ]
 */
/*
*****************************************************************************
**                                                                          *
**  Copyright © 1993, 1994						    *
**  by Digital Equipment Corporation, Maynard, Massachusetts.		    *
**                                                                          *
**  All Rights Reserved							    *
**                                                                          *
**  Permission  is  hereby  granted  to  use, copy, modify and distribute   *
**  this  software  and  its  documentation,  in  both  source  code  and   *
**  object  code  form,  and without fee, for the purpose of distribution   *
**  of this software  or  modifications  of this software within products   *
**  incorporating  an  integrated   circuit  implementing  Digital's  AXP   *
**  architecture,  regardless  of the  source of such integrated circuit,   *
**  provided that the  above copyright  notice and this permission notice   *
**  appear  in  all copies,  and  that  the  name  of  Digital  Equipment   *
**  Corporation  not  be  used  in advertising or publicity pertaining to   *
**  distribution of the  document  or  software without specific, written   *
**  prior permission.							    *
**                                                                          *
**  Digital  Equipment  Corporation   disclaims  all   warranties  and/or   *
**  guarantees  with  regard  to  this  software,  including  all implied   *
**  warranties of fitness for  a  particular purpose and merchantability,   *
**  and makes  no  representations  regarding  the use of, or the results   *
**  of the use of, the software and documentation in terms of correctness,  *
**  accuracy,  reliability,  currentness  or  otherwise;  and you rely on   *
**  the software, documentation and results solely at your own risk.	    *
**                                                                          *
**  AXP is a trademark of Digital Equipment Corporation.		    *
**                                                                          *
*****************************************************************************
**
**  FACILITY:	
**
**	DECchip 21164 PALcode
**
**  MODULE:
**
** 	platform.s
** 
**  MODULE DESCRIPTION:
** 
**	Machine specific OSF/1 PALcode for the DECchip 21164
** 
**  AUTHOR: ER 
** 
**  CREATION-DATE: 16-Dec-1993
**
**  $Id: platform.s,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $
** 
**  MODIFICATION HISTORY: 
**
**  $Log: platform.s,v $
**  Revision 1.1.1.1  1998/12/29 21:36:07  paradis
**  Initial CVS checkin
**
**  Revision 1.29  1997/08/21  20:14:27  pbell
**  Turned on byte/word instructions for pc164.
**
**  Revision 1.28  1997/08/18  18:29:28  fdh
**  Merged in SX164 support.
**
**  Revision 1.27  1997/03/27 19:03:18  fdh
**  Added the DISABLE_MACHINE_CHECKS conditional for the System
**  Machine Checks enabled in the CIA Control register.
**
**  Revision 1.26  1997/03/27  18:50:13  fdh
**  Merged in LX code to control machine check enable state.
**
**  Revision 1.25  1997/03/26  18:28:21  fdh
**  Place the Pal_End label at the end and any text
**  or data segments contained in this code.
**  Compute the location of the impure area as the first
**  page boundary beyond the Pal_End label.
**
**  Revision 1.24  1996/06/24  14:57:25  fdh
**  Corrected pointer to logout frame in Sys_MchkLogOut.
**
**  Revision 1.23  1996/06/17  16:38:10  fdh
**  Separated EB164/SD164 around conditionals.
**
**  Revision 1.22  1996/06/17  16:03:55  fdh
**  Disabled serial port and real-time clock initialization
**  in sys_reset.
**
**  Disabled System Machine checks by default in sys-reset.
**
**  Expanded sys_mchklogout to save additional logout information.
**
**  Added cserves with conditionals to support communications
**  through the SROM Serial Port.
**
**  Revision 1.21  1995/12/05  16:07:02  cruz
**  Set IPL to 7 before entering console.
**  Ignore bits 32-63 of the signature when determining if
**  a valid signature is present.
**
**  Revision 1.20  1995/11/30  20:50:48  fdh
**  Added EB164 code to initialize the CIA Control Register.
**
**  Revision 1.19  1995/11/07  09:55:19  cruz
**  Added instruction for moving the halt code to t0 before
**  entering console.
**
**  Revision 1.18  1995/04/03  17:44:25  samberg
**  Use conditionals to avoid problems with SD164 (who cares?)
**
**  Revision 1.17  1995/04/03  17:30:58  samberg
**  Add rd_bc_cfg_off. Also fix assignment of parameters to wr_bcache.
**
**  Revision 1.16  1995/02/06  18:06:10  samberg
**  Remove include of config.h
**
**  Revision 1.15  1995/02/06  18:00:48  samberg
**  Create ISP_EGORE for vice, eliminate config.h
**
**  Revision 1.14  1995/02/06  15:43:33  samberg
**  Move platform-specific stuff from osfpal.s to platform.s
**
**  Revision 1.13  1995/02/02  19:32:50  samberg
**  Cleaned up machine check and crd, added wr_bcache cserve
**
**  Revision 1.12  1995/01/17  19:32:26  samberg
**  Remove unused Sys_ExitConsole, doesn't restore mapping mode properly
**
**  Revision 1.11  1995/01/04  16:32:50  samberg
**  Add istream prefetch quieter to Wr_bcCtl and Wr_bcCfg
**
**  Revision 1.10  1994/12/19  17:50:46  samberg
**  In wr_bcCfg, write to bcCfg instead of bcCtl (dah)
**
**  Revision 1.9  1994/12/08  17:13:53  samberg
**  Add cserve subfunctions to write bcCtl and bcCfg
**
**  Revision 1.8  1994/11/30  15:42:32  samberg
**  In clock interrupt, use p6,p7 as inport and outport temps,
**  	so that we don't trash dispatch address
**
**  Revision 1.7  1994/09/26  14:16:33  samberg
**  Completed VICE work and EB164/SD164 breakout.
**
**  Revision 1.6  1994/09/07  17:40:31  samberg
**  Take out RAX and ACORE, had problem with embedding with VICE
**
**  Revision 1.5  1994/09/07  15:38:52  samberg
**  Modified for use with Makefile.vpp
**
**  Revision 1.4  1994/08/30  14:40:55  samberg
**  Remove SIO configuration register init, done by someone else
**
**  Revision 1.3  1994/07/26  17:40:27  samberg
**  Changes for SD164, debug code still in.
**
**  Revision 1.2  1994/07/12  18:19:40  samberg
**  Changes to sync up with enterconsole and savestate
**
**  Revision 1.1  1994/07/08  17:00:44  samberg
**  Initial revision
**
**  Revision 1.13  1994/06/01  19:29:09  ericr
**  Implemented physical translation mode
**
**  Revision 1.12  1994/05/27  20:12:03  ericr
**  Reorganized platform-specific interrupt and machine check handling
**
**  Revision 1.11  1994/05/26  19:31:30  ericr
**  Updated to Rev. 0.41 - 0.46 of internal EV5 OSF/1 PALcode
**
**  Revision 1.10  1994/05/26  15:11:54  ericr
**  SysReset: added RAX reset code
**
**  Revision 1.9  1994/05/25  22:22:01  ericr
**  ExitConsole:  Load impure pointer into r1.
**
**  CboxInit:  Modified code to use less space - located at the end
**  of PALcode since it needs to reside at the end of a page.
**
**  Revision 1.8  1994/05/24  20:52:43  ericr
**  Moved STACK_FRAME macro to osf.h
**  Moved SAVE_IPR macro to impure.h
**
**  Revision 1.7  1994/05/20  18:08:08  ericr
**  Changed line comments to C++ style comment character
**
**  Revision 1.6  1994/02/28  18:45:44  ericr
**  Fixed EGORE related bugs
**
**  Revision 1.5  1994/01/26  22:29:08  ericr
**  Updated to Rev. 0.35 of internal EV5 OSF/1 PALcode
**
**  Revision 1.4  1994/01/06  18:12:47  ericr
**  Fixed numerous PVC violations
**
**  Revision 1.3  1994/01/03  19:33:27  ericr
**  Stub out additional system specific code flows
**
**  Revision 1.2  1993/12/17  17:50:04  eric
**  Fixed problem with RAX conditional
**
**  Revision 1.1  1993/12/16  21:55:05  eric
**  Initial revision
**
**
**-- 
*/
#if !defined(lint)
	.data
	.asciz "$Id: platform.s,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $"
#endif

#include	"dc21164.h"	// DECchip 21164 specific definitions
#include	"osf.h"		// OSF/1 specific definitions
#include	"macros.h"	// Global macro definitions
#include	"impure.h"	// Scratch & logout area data structures
#include	"platform.h"	// Platform specific definitions
#include	"cserve.h"	// Cserve definitions


/*======================================================================*/
/*                     EXTERNAL ROUTINE DEFINITIONS                     */
/*======================================================================*/
	.global Sys_Reset
	.global Sys_ResetViaSwppal
	.global Sys_Interrupt
	.global Sys_IntMchkHandler
	.global	Sys_MchkLogOut
	.global Sys_EnterConsole
	.global	Sys_Cflush
	.global	Sys_Cserve
	.global	Sys_WrIpir
	.global Pal_End

	.global MchkCommon
	.global HaltDoubleMchk
	.global SaveState
	.global RestoreState
	.global CallPal_Rti

	.global Wr_Bcache
	.text	3

/*======================================================================*/
/*                   PLATFORM SPECIFIC INITIALIZATION                   */
/*======================================================================*/

	ALIGN_BLOCK

Sys_Reset:
#if defined(DEBUG)
	LEDWRITE(0xEB, t0, p7)
#endif

/*
**
** Create the internal IPL translation mask.
**
*/
	LDLI(t0,INT_K_MASK_HIGH)	// Load the upper longword of the mask.
	sll	t0, 32, t0		// Move into high end of quadword.
	LDLI(p7, INT_K_MASK_LOW)	// Load the lower longword of the mask.
	bis	t0, p7, t0		// Merge to create a quadword.
	mtpr	t0, ptIntMask		// Save in PALtemp.

#if defined(DISABLE_MACHINE_CHECKS)
/* LX164 */
/*
** Disable Machine Checks for now.
** Machine Check handlers have not been setup yet.
**
** Call Wr_Bcache with the following arguments.
**      a0 (r16) - New BC_CONTROL value
**	a1 (r17) - BC_CONFIG value
**	a3 (r19) - MCR value
**	a4 (r20) - GTR value
**
**      v0 (r0)  - Cbox Base Address FF.FFF0.0000
**      p1 () - MCR Address
*/
	mfpr	p4, ptImpure		// Get the base of the impure area.
	lda	t1, CNS_Q_IPR(p4)	// Point to base of IPR area.

	RESTORE_SHADOW(t0,CNS_Q_SIGNATURE,t1)	// Get Signature
	srl	t0, 16, t0		// Shift signature into lower word.
	LDLI(p4,0xDECB)			// Load the expected valid signature.
	cmpeq	p4, t0, p4		// Is it a valid signature?
	beq	p4, 2f			// If invalid, don't trust input params

	RESTORE_SHADOW(t0,CNS_Q_BC_CTL,t1)	// Get bcCtl.
	LDLI(p4, BC_K_MCHECK)		// Load up Mck Bits.
	bis	t0, p4, a0		// Disable them for now.
	bic	a0, BC_M_ALLOC_CYC, a0	// Make sure alloc_cycle is clear
	SAVE_SHADOW(a0,CNS_Q_BC_CTL,t1)	// Save shadow of bcCtl.

	ldah	v0, 0xFFF0(zero)	// v0 <- FFFF.FFFF.FFF0.0000
	zap	v0, 0xE0, v0		// Get base address of FF.FFF0.0000

	RESTORE_SHADOW(a1,CNS_Q_BC_CFG,t1)	// Load up current BC_CONFIG

	lda	p0, 1(zero)		// Get a 1
	sll	p0, 39, p0		// Place in bit 39

	lda	p1, CIA_MEM_BASE(zero)	// Form 87.5000.0000 - MCR
	sll	p1, 28, p1
	bis	p1, p0, p1		// Offset is 0, just just do bit 39
	ldl_p	a3, 0(p1)		// Read current MCR

	lda	p2, CIA_MEM_BASE(zero)	// Form 87.5000.0200 - GTR
	sll	p2, 28, p2
	lda	p2, 0x200(p2)		// Offset 0x200 (GTR)
	bis	p2, p0, p2		// Do bit 39
	ldl_p	a4, 0(p2)		// Read current GTR
	
pvc$osf81$5080:
	bsr	p6, Wr_Bcache		// Call cache-block-aligned routine
2:	
#endif /* DISABLE_MACHINE_CHECKS */

/*
**
** The SIO provides an ISA compatible interrupt controller which incorporates
** the functionality of two 82C59 interrupts controllers. The
** controllers are cascaded.
**
** We need to intialize the interrupt controller.
**
** INITIALIZATION COMMAND WORDS (ICW)
**
** Whenever a command is issued with A0=0 and D4=1, this is interpreted as
** Initialization Command Word 1 (ICW1).  ICW1 starts the initialization
** sequence.
** 
** INITIALIZATION COMMAND WORD 1 FORMAT (ICW1):
**
**   A0     D7  D6  D5  D4  D3  D2  D1  D0
** +---+  +---+---+---+---+---+---+---+---+
** | 0 |  | 0 | 0 | 0 | 1 | x | 0 | 0 | 1 |
** +---+  +---+---+---+---+---+---+---+---+
**                          ^   ^   ^   ^
**                          |   |   |   |
**                          |   |   |   +---- ICW4 needed 
**                          |   |   |
**                          |   |   +-------- Cascade mode
**                          |   |
**                          |   +------------ Call address interval of 8
**                          |
**                          +---------------- 0 = Edge triggered mode (INT2)
**                                            1 = Level triggered mode (INT1)
** 
** INITIALIZATION COMMAND WORD 2 FORMAT (ICW2):
**
**   A0     D7  D6  D5  D4  D3  D2  D1  D0
** +---+  +---+---+---+---+---+---+---+---+
** | 1 |  | 0 | 0 | 0 | 0 | 1 | 0 | 0 | 0 |
** +---+  +---+---+---+---+---+---+---+---+
**          ^   ^   ^   ^   ^
**          |   |   |   |   |
**          +---+---+---+---+---------------- T7 - T3 of interrupt vector byte
**
**
** INITIALIZATION COMMAND WORD 3 FORMAT (ICW3):
**
**   A0     D7  D6  D5  D4  D3  D2  D1  D0
** +---+  +---+---+---+---+---+---+---+---+
** | 1 |  | 0 | 0 | 0 | 0 | 0 | 1 | 1 | 0 |
** +---+  +---+---+---+---+---+---+---+---+
**                              ^   ^
**                              |   |
**                              |   +-------- Slave ID: 00110011 (INT2)
**                              |
**                              +------------ IRQ2 input has a slave (INT1)
*/

/* INITIALIZATION COMMAND WORD 4 FORMAT (ICW4):
**
**   A0     D7  D6  D5  D4  D3  D2  D1  D0
** +---+  +---+---+---+---+---+---+---+---+
** | 1 |  | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 1 |
** +---+  +---+---+---+---+---+---+---+---+
**                      ^   ^   ^   ^   ^
**                      |   |   |   |   |
**                      |   +-+-+   |   +---- 80C86/80C88 mode
**                      |     |     |
**                      |     |     +-------- Normal end of interrupt (EOI)
**                      |     |
**                      |     +-------------- Non-buffered mode
**                      |    
**                      +-------------------- Not special fully nested mode
**
** OPERATION CONTROL WORD 1 FORMAT (OCW1):
**
**   A0     D7  D6  D5  D4  D3  D2  D1  D0
** +---+  +---+---+---+---+---+---+---+---+
** | 1 |  | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 |
** +---+  +---+---+---+---+---+---+---+---+
**          ^   ^   ^   ^   ^   ^   ^   ^
**          |   |   |   |   |   |   |   |
**          +---+---+---+---+---+---+---+---- Interrupt mask
**                                            All channels inhibited
**
** OPERATION CONTROL WORD 2 FORMAT (OCW2):
**
**   A0     D7  D6  D5  D4  D3  D2  D1  D0
** +---+  +---+---+---+---+---+---+---+---+
** | 1 |  | 0 | 0 | 1 | 0 | 0 | 0 | 0 | 0 |
** +---+  +---+---+---+---+---+---+---+---+
**          ^   ^   ^
**          |   |   |
**          +---+---+
**              |
**              +---------------------------- Non-specific EOI command
**
**
** See the SD164 Engineering Specification for information on memory 
** address bit definitions and encodings for accessing ISA I/O space.
**
*/

/*
** Initialize the 82C59A priority interrupt controller (PIC)
*/
	OutPortByte(PIC2_ICW1,0x11,t0,p7)
	OutPortByte(PIC2_ICW2,0x08,t0,p7)
	OutPortByte(PIC2_ICW3,0x02,t0,p7)
	OutPortByte(PIC2_ICW4,0x01,t0,p7)

	OutPortByte(PIC2_OCW1,0xFF,t0,p7)

	OutPortByte(PIC1_ICW1,0x11,t0,p7)
	OutPortByte(PIC1_ICW2,0x00,t0,p7)
	OutPortByte(PIC1_ICW3,0x04,t0,p7)
	OutPortByte(PIC1_ICW4,0x01,t0,p7)

	OutPortByte(PIC1_OCW1,0xFF,t0,p7)

/*
** Send -INTA pulses to clear any pending interrupts ...
*/
	IACK(t0,p7)

/*
** Finish writing the 82C59A PIC Operation Control Words ...
*/
	OutPortByte(PIC2_OCW2,0x20,t0,p7)
	OutPortByte(PIC1_OCW2,0x20,t0,p7)

#if defined(DISABLE_MACHINE_CHECKS)
/*
** Setup the CIA Control Register to disable System Machine Checks.
** They can be enabled after Machine check handlers have been setup.
*/
	lda	t0, 1(zero)		// Get a 1
	sll	t0, 39, t0		// Place in bit 39
	lda	p7, CIA_BASE(zero)	// Form 87.4000.0100 - CIA_CTRL
	sll	p7, 28, p7
	lda	p7, CIA_CTRL(p7)	// Offset 0x100
	bis	p7, t0, p7		// Do bit 39
	LDLI(t0, CIA_CTRL_K_SYS_MCHK)	// Load up CIA_CTL Machine Check bits
	ldl_p	t1, 0(p7)		// Read the CIA_CTRL
	bic	t1, t0, t1		// Clear Machine check enable bits
	stl_p	t1, 0(p7)		// Write the new CIA_CTRL
#endif /* DISABLE_MACHINE_CHECKS */
pvc$osf41$5010.1:
	ret	zero, (p5)		// Back to common reset flow...


/*
** We reset via a swppal. We need to load the internal IPL translation mask.
*/

Sys_ResetViaSwppal:
	LDLI(t0,INT_K_MASK_HIGH)	// Load the upper longword of the mask.
	sll	t0, 32, t0		// Move into high end of quadword.
	LDLI(p7,INT_K_MASK_LOW)	// Load the lower longword of the mask.
	bis	t0, p7, t0		// Merge to create a quadword.
	mtpr	t0, ptIntMask		// Save in PALtemp.
pvc$osf48$5060.1:
	ret	zero, (v0)		// Back to common ResetViaSwppal.

/*======================================================================*/
/*                 PLATFORM SPECIFIC INTERRUPT HANDLERS                 */
/*======================================================================*/


/*
** INPUT PARAMETERS:
**
**	p5 - Target IPL of highest priority pending interrupt
**	p7 - Interrupt summary
**
** Registers a0 .. a2, gp, ps, and pc saved on the kernel stack.
**
** OUTPUT PARAMETERS:
**
**	a0 - Interrupt entry type
**	a1 - Platform-specific interrupt vector
**	a2 - UNPREDICTABLE
**
** NOTE:
** The OSF/1 IPL and the internal IPL still need to be set!
*/
	ALIGN_BRANCH_TARGET
Sys_Interrupt:

	cmpeq	p5, 23, p4		// Is it a level 23 interrupt?
	bne	p4, Int23Handler

	cmpeq	p5, 22, p4		// Is it a level 22 interrupt?
	bne	p4, Int22Handler

	cmpeq	p5, 21, p4		// Is it a level 21 interrupt?
	bne	p4, Int21Handler

	cmpeq	p5, 20, p4		// Is it a level 20 interrupt?
	bne	p4, Int20Handler

	NOP				// Nothing there, so back out with rti.
	br	zero, CallPal_Rti

	ALIGN_BRANCH_TARGET
/*
** Int23 Handler
**
** On the EB164 PASS2, this interrupt is reserved. Just dismiss.
*/
Int23Handler:
	NOP
	br	zero, DismissInterrupt


	ALIGN_BRANCH_TARGET
/*
** Int22 Handler
**
** On the EB164, the Int22 Handler is for the Real Time Clock.
** We need to clear the interrupt, set up the ps, set up
** the a0-a1 paramters and post the interrupt.
**
** OUTPUT PARAMETERS:
** 	ps - IPL_K_CLK
**	a0 - Interrupt entry type = 1
**	a1 - UNPREDICTABLE
**	a2 - UNPREDICTABLE
**
** The RTC internal registers are accessed using two registers
** located in ISA I/O space:
**
**	RTCADD	0x70	RTC Address Register
**	RTCDAT	0x71	RTC Data Register
**
** On the first access, the address of the desired internal
** register is loaded, and on the second, the data are read
** or written.
*/
Int22Handler:

	mfpr	p4, ptIntMask		// Get the IPL translation mask.
	extbl	p4, IPL_K_CLK, p5	// Translate to internal IPL.

	bis	zero, IPL_K_CLK, ps	// Set the OSF/1 IPL.
	mtpr	p5, ipl			// Load the target IPL.

	mfpr	p4, ptEntInt		// Get pointer to kernel handler.

	OutPortByte(RTCADD,0x0C,p6,p7)	// Set up RTCADD to index register C.
	InPortByte(RTCDAT,p6,p7)	// Read to clear interrupt.

	mtpr	p4, excAddr		// Load kernel entry address.
	bis	zero, INT_K_CLK, a0	// Signal Real Time Clock interrupt.

	NOP
	NOP

	hw_rei

	ALIGN_BRANCH_TARGET
/*
** Int21 Handler
**
** IRQ1 - The 'or' of 17 PCI interrupts (IPL3)
**
**	Each of these interrupts can be individually masked by
**	writing to three mask registers at ISA addresses 804h, 805h
**	and 806h.
**
**	A read of these registers will return the state of the
**	17 PCI interrupts and not the state of the MASKED
**	interrupts. Thus it is necessary to know the mask values
**	to determine the real source. The PALcode will let the
**	OS handle all this.
**
**	So all we do it set a1 to 0x800 and a0 to 3 (device type)
**
** OUTPUT PARAMETERS:
** 	ps - IPL_K_DEV0 = 3
**	a0 - Interrupt entry type = 1
**	a1 - 0x800
*/
Int21Handler:
	mfpr	p4, ptIntMask		// Get the IPL translation mask.
	extbl	p4, IPL_K_DEV0, p5	// Translate to internal IPL.

	bis	zero, IPL_K_DEV0, ps	// Set the OSF/1 IPL.
	mtpr	p5, ipl			// Load the target IPL.

	mfpr	p4, ptEntInt		// Get pointer to kernel handler.
	lda	a1, 0x800(zero)		// Hardcode device interrupt for now

	mtpr	p4, excAddr		// Load kernel entry address
	bis	zero, INT_K_DEV, a0	// Signal I/O device interrupt

	NOP
	NOP

	hw_rei

	ALIGN_BRANCH_TARGET
/*
** Int20 Handler
**
** NEED TO FIX -
**
** IRQ0 - Corrected system error. Corrected ECC error detected by CIA.
**	  We just take a system machine check (0x660), instead of cleaning up
**	  the error and taking a system correctable machine check (0x620), as
**	  we really should do.
*/
Int20Handler:

	mfpr	p4, ptIntMask			// Get IPL translation mask.
	extbl	p4, IPL_K_MCHK, p5		// Translate to internal IPL.

	bis	zero, IPL_K_MCHK, ps		// Set the OSF/1 IPL.
	mtpr	p5, ipl				// Load the target IPL.

	lda	p6, MCHK_K_CIA_0(zero)		// Denote as CIA error.
	br	zero, Sys_IntMchkHandlerMerge	// Turn into machine check.

	ALIGN_BRANCH_TARGET
/*
** Can't find the interrupt now. We have already stacked and made IPL
** and ps changes.
** Undo with call to rti.
*/

DismissInterrupt:
	NOP				// Nothing there, so back out with rti.
	br	zero, CallPal_Rti

/*======================================================================*/
/*     PLATFORM SPECIFIC INTERRUPT MACHINE CHECK HANDLER                */
/*======================================================================*/
/*
** FUNCTIONAL DESCRITPION:
**
**	The interrupt handler for system machine check stores
**	a code indicating system machine check error, loads
**	the System Control Block (SCB) vector for the
**	system machine check service routine, sets the
**	Machine-Check-In-Progress (MIP) flag in the Machine
**	Check Error Summary register (MCES), sets a flag indicating
**	that this is an interrupt-type machine check, and merges
**	with the common machine check flow.
**
**	If a second processor machine check error condition is 
**	detected while the MIP flag is set, the processor is 
**	forced into console I/O mode indicating "double error 
**	abort encountered" as the reason for the halt.
**
** OUTPUT PARAMETERS:
**
**	pt0		= saved v0
**	pt1		= saved t0
**	pt4		= saved t3
**	pt5		= saved t4
**	pt6		= saved t5
**	ptMisc<47:32>	= MCHK code with low bit set to indicate interrupt
**	ptMisc<31:16>	= SCB vector
**	ptMces<MIP>	= Set
**	p6 		= Cbox IPR base address
**
*/
	ALIGN_BRANCH_TARGET
/*
** On the EB164 PASS2, SIO NMI and CIA errors come into this machine check.
** For now, handle the SIO, and just leave the machine check as a general
** system hard error if not SIO.
*/
Sys_IntMchkHandler:
	InPortByte(SIO_NMISC,p4,p7)	// Get the NMI Status/Control bits.

	srl	p4, SIO_NMISC_V_SERR, p5	// Get SERR status bit in lsb.
	blbc	p5, 3f				// Not SERR, try IOCHK.
	srl	p4, SIO_NMISC_V_SERR_EN, p5	// Get SERR ena bit into lsb.
	blbs	p5, 3f				// SERR disabled, go try IOCHK.

	bis	p4, SIO_NMISC_M_SERR_EN, p5	// Set the SERR enable bit.
	and	p5, 0x0F, p5			// Clear status bits for write.

	OutPortByteReg(SIO_NMISC, p5, p6, p7)	// Clear source of interrupt.

	bis	zero, p4, p5			// Copy back original NMISC.
	and	p5, 0x0F, p5			// Clear status bits for write.

	OutPortByteReg(SIO_NMISC, p5, p6, p7)	// Write orig NMISC value.

	lda	p6, MCHK_K_SIO_SERR(zero)	// Denote as SIO SERR.
	br	zero, Sys_IntMchkHandlerMerge	// Turn into machine check.

	ALIGN_BRANCH_TARGET

3:	srl	p4, SIO_NMISC_V_IOCHK, p5	// Get IOCHK status into lsb.
	blbc	p5, 4f				// Not IOCHK.
	srl	p4, SIO_NMISC_V_IOCHK_EN, p5	// Get IOCHK ena bit into lsb.
	blbs	p5, 4f				// IOCHK disabled.

	bis	p4, SIO_NMISC_M_IOCHK_EN, p5	// Set the IOCHK enable bit.
	and	p5, 0x0F, p5			// Clear status bits for write.

	OutPortByteReg(SIO_NMISC, p5, p6, p7)	// Clear source of interrupt.

	bis	zero, p4, p5			// Copy back original NMISC.
	and	p5, 0x0F, p5			// Clear status bits for write.

	OutPortByteReg(SIO_NMISC, p5, p6, p7)	// Write NMISC contents.

	lda	p6, MCHK_K_SIO_IOCHK(zero)	// Denote as SIO IOCHK.
	br	zero, Sys_IntMchkHandlerMerge	// Turn into system mchk.

	ALIGN_BRANCH_TARGET
/*
** CIA error. Don't think anything else comes in here. But for now
** just leave as a general system machine check.
*/
4:	lda	p6, MCHK_K_SYS_HERR(zero)	// Denote as CIA error.
	br	zero, Sys_IntMchkHandlerMerge	// Turn into machine check.

	ALIGN_BRANCH_TARGET
Sys_IntMchkHandlerMerge:
	mfpr	p4, excAddr		// Get exception address
	addq	p6, 1, p6		// Flag as interrupt

	sll	p6, PT16_V_MCHK, p6	// Shift MCHK code into upper longword
	mtpr	p4, pt10		// Save the exception address

	mfpr	p4, ptMces		// Get MCES and scratch bits

	mtpr	v0, pt0			// Save v0
	zap	p4, 0x3C, p4		// Isolate just MCES bits

	blbs	p4, HaltDoubleMchk	// Halt if MCHK already in progress

	bis	p4, p6, p4		// Combine MCES bits and MCHK code
	lda	p6, SCB_Q_SYSMCHK(zero)	// Get SCB vector

	sll	p6, PT16_V_SCB, p6	// Shift SCB vector into upper word

	bis	p4, p6, p6		// Combine SCB vector and MCHK bits

	bis	p6, MCES_M_MIP, p6	// Set Machine-Check-In-Progress flag

	mtpr	p6, ptMces		// Save combined SCB and MCHK bits

	ldah	p6, 0xFFF0(zero)	// p6 <- FF...FFF0.0000
	mtpr	t0, pt1			// Save t0

	zap	p6, 0xE0, p6		// p6 <- FF.FFF0.0000 Cbox IPR base 
	mtpr	t3, pt4			// Save t3

	mtpr	t4, pt5			// Save t4

	mtpr	t5, pt6			// Save t6

	br	zero, MchkCommon	// Go join common machine flow ...


/*======================================================================*/
/*         PLATFORM SPECIFIC MACHINE CHECK LOGOUT                       */
/*======================================================================*/
/*
** Code to complete the platform-specific section of the MCHK 
** logout frame goes here ...
**
** Register Usage Conventions:
**
**	v0	Return address
**	t0	Scratch
**	t3	Scratch
**	t4	Scratch
**	t5	Scratch
**	p0	Scratch
**	p1	Scratch
**	p2	Scratch
**	p4	Scratch
**	p5	Scratch
**	p6	Base address of MCHK logout frame
**	p7	Scratch
**
**	pt0	Saved v0
**	pt1	Saved t0
**	pt4	Saved t3
**	pt5	Saved t4
**	pt6	Saved t5
**	pt10	Saved excAddr
**
*/
	ALIGN_BLOCK

Sys_MchkLogOut:

/*
** Code to complete the platform-specific section of the MCHK 
** logout frame goes here ...
*/

	LDLI(p5, CIA_ERROR_BASE)	// p5 <- 0874.0008
	sll	p5, 12, p5		// p5 <- 87.4000.8000

	ldl_p	p7, 0(p5)		// Read the CIA_CPU_ERR0 register
	stq_p	p7, LAF_Q_CPU_ERR0(p6)	// Store it into Logout Frame

	lda	p7, CIA_CPU_ERR1(p5)	// Load offset to register.
	ldl_p	p7, 0(p7)		// Read the register.
	stq_p	p7, LAF_Q_CPU_ERR1(p6)	// Store it into Logout Frame

	lda	p7, CIA_ERR(p5)	// Load offset to register.
	ldl_p	p7, 0(p7)		// Read the register.
	stq_p	p7, LAF_Q_CIA_ERR(p6)	// Store it into Logout Frame

	lda	p7, CIA_STAT(p5)	// Load offset to register.
	ldl_p	p7, 0(p7)		// Read the register.
	stq_p	p7, LAF_Q_CIA_STAT(p6)	// Store it into Logout Frame

	lda	p7, CIA_ERR_MASK(p5)	// Load offset to register.
	ldl_p	p7, 0(p7)		// Read the register.
	stq_p	p7, LAF_Q_ERR_MASK(p6)	// Store it into Logout Frame

	lda	p7, CIA_SYN(p5)	// Load offset to register.
	ldl_p	p7, 0(p7)		// Read the register.
	stq_p	p7, LAF_Q_CIA_SYN(p6)	// Store it into Logout Frame

	lda	p7, CIA_MEM_ERR0(p5)	// Load offset to register.
	ldl_p	p7, 0(p7)		// Read the register.
	stq_p	p7, LAF_Q_MEM_ERR0(p6)	// Store it into Logout Frame

	lda	p7, CIA_MEM_ERR1(p5)	// Load offset to register.
	ldl_p	p7, 0(p7)		// Read the register.
	stq_p	p7, LAF_Q_MEM_ERR1(p6)	// Store it into Logout Frame

	lda	p7, CIA_PCI_ERR0(p5)	// Load offset to register.
	ldl_p	p7, 0(p7)		// Read the register.
	stq_p	p7, LAF_Q_PCI_ERR0(p6)	// Store it into Logout Frame

	lda	p7, CIA_PCI_ERR1(p5)	// Load offset to register.
	ldl_p	p7, 0(p7)		// Read the register.
	stq_p	p7, LAF_Q_PCI_ERR1(p6)	// Store it into Logout Frame

	lda	p7, CIA_PCI_ERR2(p5)	// Load offset to register.
	ldl_p	p7, 0(p7)		// Read the register.
	stq_p	p7, LAF_Q_PCI_ERR2(p6)	// Store it into Logout Frame
pvc$osf42$5050.1:
	ret	zero, (v0)		// Back to caller ...

/*======================================================================*/
/*                 PLATFORM SPECIFIC CALL_PAL FUNCTIONS                 */
/*======================================================================*/

	ALIGN_BLOCK

Sys_Cflush:
	hw_rei

	ALIGN_BLOCK

Sys_WrIpir:
	hw_rei


/*
**
** FUNCTIONAL DESCRIPTION:
**
**	The console service (cserve) function implements console and
**	platform specific operations.
**
** CALLING SEQUENCE:
**
**	Branched into from the CALL_PAL cserve entry point.
**
** INPUT PARAMETERS:
**
**	a0 - function parameter
**	a1 - function parameter
**	a2 - function type
**
** OUTPUT PARAMETERS:
**
**	v0 - function result
**
** SIDE EFFECTS:
**	
*/
	ALIGN_BLOCK

Sys_Cserve:

	cmpeq	a2, CSERVE_K_LDQP, v0
	bne	v0, Sys_Cserve_Ldqp

	cmpeq	a2, CSERVE_K_STQP, v0
	bne	v0, Sys_Cserve_Stqp

	cmpeq	a2, CSERVE_K_JTOPAL, v0
	bne	v0, Sys_Cserve_Jtopal

	cmpeq	a2, CSERVE_K_WR_INT, v0
	bne	v0, Sys_Cserve_Wr_Int

	cmpeq	a2, CSERVE_K_RD_IMPURE, v0
	bne	v0, Sys_Cserve_Rd_Impure

	cmpeq	a2, CSERVE_K_PUTC, v0
	bne	v0, Sys_Cserve_Putc

	cmpeq	a2, CSERVE_K_WR_ICSR, v0
	bne	v0, Sys_Cserve_Wr_Icsr

	cmpeq	a2, CSERVE_K_RD_ICSR, v0
	bne	v0, Sys_Cserve_Rd_Icsr

	cmpeq	a2, CSERVE_K_RD_BCCTL, v0
	bne	v0, Sys_Cserve_Rd_BcCtl
	cmpeq	a2, CSERVE_K_RD_BCCFG, v0
	bne	v0, Sys_Cserve_Rd_BcCfg

	cmpeq	a2, CSERVE_K_WR_BCACHE, v0
	bne	v0, Sys_Cserve_Wr_Bcache

	cmpeq	a2, CSERVE_K_RD_BCCFG_OFF, v0
	bne	v0, Sys_Cserve_Rd_BcCfgOff
#if defined(SROM_SERIAL_PORT)
	cmpeq	a2, CSERVE_K_SROM_INIT, v0
	bne	v0, Sys_Cserve_Srom_Init

	cmpeq	a2, CSERVE_K_SROM_PUTC, v0
	bne	v0, Sys_Cserve_Srom_Putc

	cmpeq	a2, CSERVE_K_SROM_GETC, v0
	bne	v0, Sys_Cserve_Srom_Getc
#endif /* SROM_SERIAL_PORT */

	hw_rei                          // If none, just return ...

/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Load (physical) quadword from memory to register v0.
**
** INPUT PARAMETERS:
**
**      a0 (r16) - Physical address
**
** OUTPUT PARAMETERS:
**
**	v0 (r0)  - Returned data
**
** SIDE EFFECTS:
**	
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Ldqp:
	ldq_p	v0, 0(a0)
	hw_rei

/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Store (physical) quadword from register a1 to memory.
**
** INPUT PARAMETERS:
**
**      a0 (r16) - Physical address
**      a1 (r17) - Data to be written
**
** OUTPUT PARAMETERS:
**
**	None
**
** SIDE EFFECTS:
**	
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Stqp:
	stq_p	a1, 0(a0)
	hw_rei

/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Transfer control to the specified address, passed in 
**      register a0, in PAL mode.
**
** INPUT PARAMETERS:
**
**      a0 (r16) - Transfer address
**
** OUTPUT PARAMETERS:
**
**   DECchip 21064 specific parameters:
**   
**      t0 (r1)  - bcCtl	DC21164
**      t0 (r1)  - bcCfg2	DC21164PC
**      t1 (r2)  - bcCfg
**
**   Firmware specific parameters:
**
**	s6 (r15) - Encoded srom.s RCS revision
**	a0 (r16) - Processor identification (a la SRM)
**      a1 (r17) - Size of good memory in bytes
**      a2 (r18) - Cycle count in picoseconds
**      a3 (r19) - Protocol signature and system revision
**      a4 (r20) - Active processor mask
**      a5 (r21) - System Context value
**
** SIDE EFFECTS:
**
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Jtopal:
	bic	a0, 3, t8		// Clear out low 2 bits of address
	bis	t8, 1, t8		// Or in PAL mode bit

	mfpr	t9, ptImpure		// Get base of impure scratch area.
        lda     t9, CNS_Q_IPR(t9)    	// Point to start of IPR area.

	RESTORE_SHADOW(a3,CNS_Q_SIGNATURE,t9)	// Get signature.
        srl     a3, 16, t0              // Shift signature into lower word.
	zap	t0, 0xFC, t0		// Ignore the upper longword.
	LDLI(t10,0xDECB)		// Load the expected valid signature.

        cmpeq   t0, t10, t0		// Check if saved signature was valid.
        blbc    t0, 1f                  // If invalid, pass nothing.
/*
** Load the processor specific parameters ...
*/
	RESTORE_SHADOW(t0,CNS_Q_BC_CTL,t9)	// Get bcCtl.
	RESTORE_SHADOW(t1,CNS_Q_BC_CFG,t9)	// Get bcCfg.
	RESTORE_SHADOW(t2,CNS_Q_BC_CFG_OFF,t9)	// Get bcCfg.
/*
** Load the firmware specific parameters ...
*/
	RESTORE_SHADOW(s6,CNS_Q_SROM_REV,t9)	// Get srom revision.
	RESTORE_SHADOW(a0,CNS_Q_PROC_ID,t9)	// Get processor id.
	RESTORE_SHADOW(a1,CNS_Q_MEM_SIZE,t9)	// Get memory size.
	RESTORE_SHADOW(a2,CNS_Q_CYCLE_CNT,t9)	// Get cycle count.
	RESTORE_SHADOW(a4,CNS_Q_PROC_MASK,t9)	// Get processor mask.
	RESTORE_SHADOW(a5,CNS_Q_SYSCTX,t9)	// Get system context.

	STALL
	STALL

1:	mtpr	zero, ptWhami		// Clear WHAMI and swap flag.
	mtpr	t8, excAddr		// Load the dispatch address.
	br	zero, 2f

	ALIGN_BLOCK

2:	NOP
	mtpr	zero, icFlush		// Flush the icache.
	NOP
	NOP

	NOP                           // Required NOPs ... 1-10
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP

	NOP                           // Required NOPs ... 11-20
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP

	NOP                           // Required NOPs ... 21-30
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP

	NOP                           // Required NOPs ... 31-40
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP



	NOP				// Required NOPs ... 41-44
	NOP
	NOP
	NOP

	hw_rei				// Dispatch in PAL mode ...

/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Write the Interrupt Map Register
**
** INPUT PARAMETERS:
**
**      a0 (r16) - New map value
**
** OUTPUT PARAMETERS:
**
**	None
**
** SIDE EFFECTS:
**
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Wr_Int:
	mtpr	a0, ptIntMask		// Write new interrupt map

	STALL
	STALL

	hw_rei

/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Return the base address of the PAL impure scratch
**      area in register v0 (r0).
**
** INPUT PARAMETERS:
**
**      None
**
** OUTPUT PARAMETERS:
**
**	v0 (r0) - Returned base address of impure scratch area.
**
** SIDE EFFECTS:
**
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Rd_Impure:
	mfpr	v0, ptImpure		// Get base of impure scratch area.
	hw_rei

/*
**
** FUNCTIONAL DESCRIPTION:
**
**      Output a character to the serial port.
**
** INPUT PARAMETERS:
**
**      a0 (r16) - Character
**
** OUTPUT PARAMETERS:
**
**	v0 (r0) - Return status
**
** SIDE EFFECTS:
**
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Putc:

	InPortByte(COM1_LSR,p0,p7)

	srl     p0, LSR_V_THRE,p0
	blbc    p0, Sys_Cserve_Putc

	OutPortByteReg(COM1_THR,a0,p0,v0)

	lda     v0, 1(zero)
	hw_rei



/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Return the Ibox Control/Status Register (ICSR) IPR
**      register v0 (r0).
**
** INPUT PARAMETERS:
**
**      None
**
** OUTPUT PARAMETERS:
**
**	v0 (r0) - Returned ICSR value
**
** SIDE EFFECTS:
**
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Rd_Icsr:
        mfpr	v0, icsr			// Read IPR.
	hw_rei

/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Write the Ibox Control/Status Register (ICSR) IPR. The
**	user is not allowed to affect SPE or SDE.
**
** INPUT PARAMETERS:
**
**      a0 (r16) - New ICSR value
**
** OUTPUT PARAMETERS:
**
**	None
**
** SIDE EFFECTS:
**
*/

#define ICSR_K_MASK \
	((1<<(ICSR_V_SDE-16)) | \
	 (2<<(ICSR_V_SPE-16)))

	ALIGN_BRANCH_TARGET

Sys_Cserve_Wr_Icsr:
	ldah	p0, ICSR_K_MASK(zero)	// Disallow affecting SPE, SDE
	bis	a0, p0, p0		// Force SPE, SDE
	
	mtpr	p0, icsr		// Write ICSR

	STALL                           // Required stall to update chip ...
        STALL
	STALL
	STALL
	STALL

	hw_rei

/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Return the shadow copy of the BC_CONTROL IPR in 
**      register v0 (r0).
**
** INPUT PARAMETERS:
**
**      None
**
** OUTPUT PARAMETERS:
**
**	v0 (r0) - Returned BC_CONTROL value
**
** SIDE EFFECTS:
**
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Rd_BcCtl:
	mfpr	p0, ptImpure            // Get base of impure scratch area
	lda	p0, CNS_Q_IPR(p0)	// Point to base of IPR area.

	RESTORE_SHADOW(v0,CNS_Q_BC_CTL,p0)	// Get shadow copy.

	STALL

	hw_rei

/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Return the shadow copy of the BC_CONFIG IPR in 
**      register v0 (r0).
**
** INPUT PARAMETERS:
**
**      None
**
** OUTPUT PARAMETERS:
**
**	v0 (r0) - Returned BC_CONFIG value
**
** SIDE EFFECTS:
**
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Rd_BcCfg:
	mfpr	p0, ptImpure            // Get base of impure scratch area
	lda	p0, CNS_Q_IPR(p0)	// Point to base of IPR area.

	RESTORE_SHADOW(v0,CNS_Q_BC_CFG,p0)	// Get shadow copy.

	STALL

	hw_rei


/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Return the shadow copy of the BC_CFG_OFF in 
**      register v0 (r0).
**
** INPUT PARAMETERS:
**
**      None
**
** OUTPUT PARAMETERS:
**
**	v0 (r0) - Returned BC_CFG_OFF value
**
** SIDE EFFECTS:
**
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Rd_BcCfgOff:
	mfpr	p0, ptImpure            // Get base of impure scratch area
	lda	p0, CNS_Q_IPR(p0)	// Point to base of IPR area.

	RESTORE_SHADOW(v0,CNS_Q_BC_CFG_OFF,p0)	// Get shadow copy.

	STALL

	hw_rei


/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Write BC_CONTROL IPR and the shadow copy,
**	and write BC_CONFIG and the shadow copy.
**	Write CIA CSR CACK_EN and Memory Control
**	Register MCR.
**
** INPUT PARAMETERS:
**
**      a0 (r16) - New BC_CONTROL value
**	a1 (r17) - New BC_CONFIG value
**	a3 (r19) - New MCR value
**
** OUTPUT PARAMETERS:
**
**	v0 (r0) - 1 for success
**
** SIDE EFFECTS:
**
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Wr_Bcache:
	ldah	v0, 0xFFF0(zero)	// v0 <- FFFF.FFFF.FFF0.0000
	zap	v0, 0xE0, v0		// Get base address of FF.FFF0.0000
	bic	a0, BC_M_ALLOC_CYC, a0	// Make sure alloc_cycle is clear

	lda	p0, 1(zero)		// Get a 1
	sll	p0, 39, p0		// Place in bit 39

	lda	p1, CIA_MEM_BASE(zero)	// Form 87.5000.0000 - MCR
	sll	p1, 28, p1
	bis	p1, p0, p1		// Offset is 0, so just do bit 39
	
	lda	p2, CIA_MEM_BASE(zero)	// Form 87.5000.0200 - GTR
	sll	p2, 28, p2
	lda p2, 0x200(p2)	// Offset is 0x200 - GTR
	bis	p2, p0, p2		// do bit 39
	
pvc$osf80$5080:
	bsr	p6, Wr_Bcache		// Call cache-block-aligned routine

	mfpr	p0, ptImpure            // Get base of impure scratch area
	lda	p0, CNS_Q_IPR(p0)	// Point to base of IPR area

	SAVE_SHADOW(a0,CNS_Q_BC_CTL,p0)	// Save shadow copy
	SAVE_SHADOW(a1,CNS_Q_BC_CFG,p0)	// Save shadow copy

	bis	zero, 1, v0		// Mark success

	STALL
	STALL

	hw_rei

/*
** This subroutine does writes. The write must be preceded by MB,
** followed by MB, and have no concurrent cacheable Istream references.
** So we loop a while to let the Istream settle out, and then work
** the branches to avoid Istream read misses.
*/
	ALIGN_BLOCK

Wr_Bcache:
	NOP
	NOP
	lda	p4, 100(zero)		// Get Istream prefetches to settle out
1:	subq	p4, 1, p4		// Decrement the counter

	bgt	p4, 2f			// Skip the write for many iterations
	mb
	stq_p	a0, bcCtl(v0)		// Write the control
	mb
2:	bgt	p4, 3f
	stq_p	a1, bcCfg(v0)		// Write the configuration
	mb
	stl_p	a3, 0(p1)		// Write CACK_EN
3:	bgt	p4, 4f
	mb
	stl_p	a4, 0(p2)
	mb
4:	bgt	p4, 1b			// The Ibox will predict as taken
					// and will hit in the Icache,
					// so will won't get a read miss
pvc$osf80$5080.1:
	ret	zero, (p6)		// We're done, so return


/*======================================================================*/
/*                 PLATFORM SPECIFIC CONSOLE FUNCTIONS                  */
/*======================================================================*/
/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Save the current machine state and enter
**	console I/O mode.
** 
**	**********************************************************
**	***                                                    ***
**	*** This routine is specifically tailored for use with ***
**	*** Debug Monitor on the DECchip Evaluation Boards.    ***
**	***                                                    ***
**	**********************************************************
**
** CALLING SEQUENCE:
** 
**	Called by PALcode as a result of a HALT, 
**	hard reset, or catastrophic error.
** 
** INPUT PARAMETERS:
** 
**	r0 (v0) = reason for halt
**
**	pt0	= Saved r0
** 
** OUTPUT PARAMETERS:
**
**	None
** 
** SIDE EFFECTS:
**
** 
*/ 
	ALIGN_BLOCK
	
Sys_EnterConsole:
	mtpr	t0, pt4			// Save t0
	mtpr	t2, pt5			// Save t2

#if defined(DEBUG)
	LEDWRITE(0xEA, t0, t2)
#endif

	mfpr	t0, ptImpure		// Get pointer to impure scratch area

pvc$osf43$5020:
	bsr	t2, SaveState		// Save the state of the world ...

	mfpr	t0, ptPtbr		// Get the physical page table base.
	bis	t0, 1, t0		// Enable physical mode translation.
	mtpr	t0, ptPtbr		// Update the chip.

	LDLI	(sp,0x00FFE000)		// Initialize the kernel stack pointer.

	mtpr	sp, ptKsp		// Update the saved KSP value.

	lda	ps, IPL_K_HIGH(zero)	// Set PS shadow - Kernel mode, IPL=7
	lda	t0, 0x1F(zero)		// Set internal IPL=1F
	mtpr	t0, ipl
	mtpr	zero, ips		// Set Ibox mode to kernel
	mtpr	zero, dtbCm		// Set Mbox mode to kernel

	mtpr	zero, dtbIa		// Flush the DTB
	mtpr	zero, itbIa		// Flush the ITB
	mtpr	zero, astrr		// Clear all ASTs ...
	mtpr	zero, aster
	mtpr	zero, sirr		// Clear all software interrupts.

	br	t0, 1f			// Branch over static data.
	.long	CONSOLE_ENTRY
1:	ldl_p	t0, 0(t0)		// Get the transfer address.
	mtpr	t0, excAddr		// Load console entry point.
	mfpr	t0, pt4			// Restore scratch registers.
	mfpr	t2, pt5
	bis	v0, v0, t0		// Move halt code to t0.

	hw_rei_stall			// Transfer to console I/O mode ...

/*
** The Pal_End label must be preceeded by any
** the text and data segments.
*/
	.data
Pal_End:
