/*
 *      VID: [2.0] PT: [Thu Aug 21 16:25:40 1997] SF: [osfpal.s]
 *       TI: [/sae_share/apps/bin/vice -iosfpal.s -l// -p# -DEB164 -DLX164 -h -m -Xlint -XDEBUG -XPAL_K_REV -XKDEBUG -XDISABLE_CRD -XDISABLE_MACHINE_CHECKS -XSROM_SERIAL_PORT -XCONSOLE_ENTRY -alx164 ]
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
** 	osfpal.s
** 
**  MODULE DESCRIPTION:
** 
**	Common OSF/1 PALcode for the DECchip 21164
** 
**  AUTHOR: ER 
** 
**  CREATION-DATE: 09-Dec-1993
**
**  $Id: osfpal.s,v 1.2 1999/04/15 20:56:22 paradis Exp $
** 
**  MODIFICATION HISTORY: 
**
**  $Log: osfpal.s,v $
**  Revision 1.2  1999/04/15 20:56:22  paradis
**  Fixed typo in FETCH detection
**
**  Revision 1.1.1.1  1998/12/29 21:36:07  paradis
**  Initial CVS checkin
**
**  Revision 1.46  1997/05/23  20:47:05  fdh
**  For obsolete platform.
**
**  Revision 1.45  1997/03/27 19:13:44  fdh
**  Removed platform code (moved to platform module).
**  Added DEBUG conditional to enable platform includes
**  and debug writes to LED diagnostic card.
**  Compute location of impure area when entered via swappal.
**  Corrected register usage (p3 reserved for ps use).
**
**  Revision 1.44  1997/03/26  18:28:51  fdh
**  Compute the location of the impure area as the first
**  page boundary beyond the Pal_End label.
**
**  Revision 1.43  1996/06/24  15:16:53  fdh
**  Corrections in machine check reporting.
**
**  Revision 1.42  1996/06/17  16:38:59  fdh
**  Separated EB164/SD164 around conditionals.
**
**  Revision 1.41  1996/06/17  15:49:07  fdh
**  Corrected SwapPAL problem introduced in revision 1.37
**  which cleared the swap flag unconditionally.
**
**  Added the ENABLE_MACHINE_CHECKS conditional to allow
**  the PALcode to be built conditionally with machine checks
**  enabled after PALcode reset has completed.
**
**  Corrected errors in the CRD flows and Logout Frame layout.
**
**  Revision 1.40  1995/12/05  16:08:54  cruz
**  Ignore bits 63-32 of the signature when determining if
**  a valid signature is present.
**
**  Revision 1.39  1995/11/08  15:19:03  samberg
**  Continuation of last edit. At label 2 in dtbmiss, no
**  longer have to/should re-read mm_stat. Turn into nop.
**
**  Revision 1.38  1995/11/08  15:01:29  samberg
**  In dtbmiss, use p2 for ptbr, leaving p1 for mmstat, and rearrange
**  code, using p2 for va after the physical mode test.
**
**  Revision 1.37  1995/11/07  09:53:51  cruz
**  Added code for saving GPRs in the reset flow before they
**  get trashed.
**
**  Revision 1.36  1995/04/03  17:30:20  samberg
**  Add save of bc_cfg_off in reset (and restore in swppal)
**
**  Revision 1.35  1995/04/03  15:54:00  samberg
**  When clearing lock flag in reset, use p6 insted of clobbering t0
**
**  Revision 1.34  1995/02/14  15:36:26  samberg
**  Eliminate dangling code after ISP_EGORE hw_rei.
**
**  Revision 1.33  1995/02/06  18:06:21  samberg
**  Remove include of config.h
**
**  Revision 1.32  1995/02/06  18:00:21  samberg
**  Create ISP_EGORE for vice, eliminate config.h
**
**  Revision 1.31  1995/02/06  15:43:05  samberg
**  Move platform-specific stuff from osfpal.s to platform.s
**
**  Revision 1.30  1995/02/02  19:34:14  samberg
**  Cleaned up machine check and crd
**
**  Revision 1.29  1995/01/17  19:44:39  samberg
**  Use PTBR<63> of PCB to flag physical mode
**
**  Revision 1.28  1994/12/08  17:14:14  samberg
**  Wipe out signature in a3 to be neat
**
**  Revision 1.27  1994/11/30  15:40:26  samberg
**  Fix SaveState to use r4,r5 for icsr manipulation, so
**  	we don't trash icsr save value. Save r4,r5 before
**  	using as scratch.
**
**  Revision 1.26  1994/10/17  17:59:19  samberg
**  Missing hw_rei at the end of MCHK_REI conditional code.
**
**  Revision 1.25  1994/09/26  14:17:18  samberg
**  Complete VICE and EB164/SD164 breakout.
**
**  Revision 1.24  1994/09/07  17:40:13  samberg
**  Take out RAX and ACORE, had problem with embedding with VICE
**
**  Revision 1.23  1994/09/07  16:48:45  samberg
**  Fix comments on DISABLE_CRD
**
**  Revision 1.22  1994/09/07  16:47:42  samberg
**  Use DISABLE_CRD, with default enabled (since mces forces ignore anyway)
**
**  Revision 1.21  1994/09/07  15:38:39  samberg
**  Modified for use with Makfile.vpp
**
**  Revision 1.20  1994/08/08  14:05:36  ericr
**  Added hw_rei on machine check for debugging
**
**  Revision 1.19  1994/07/26  17:39:19  samberg
**  Changes for SD164, debug code still in.
**
**  Revision 1.18  1994/07/12  18:20:26  samberg
**  Changes to sync up with enterconsole and savestate
**
**  Revision 1.17  1994/07/08  17:00:13  samberg
**  Changes to support platform specific additions
**
**  Revision 1.16  1994/06/01  19:29:13  ericr
**  Implemented physical translation mode
**
**  Revision 1.15  1994/05/27  20:12:26  ericr
**  Reorganized interrupt and machine check handling
**
**  Revision 1.14  1994/05/26  19:33:43  ericr
**  Updated to Rev. 0.41 - 0.46 of internal EV5 OSF/1 PALcode
**
**  Removed I-cache flush from TBI flows that invalidate ITB entries
**
**  Added miscellaneous PAL temp initialization in RESET
**  (mafMode, ptUsp, ptKgp, ptSysVal)
**
**  Revision 1.13  1994/05/26  15:11:11  ericr
**  Moved RAX reset code to platform.s
**
**  Shadow BC_CTL in PALcode impure scratch area
**
**  Revision 1.12  1994/05/25  22:20:05  ericr
**  SaveState & RestoreState: save more IPRs, impure pointer manipulations
**  modified to handle larger console save state area.
**
**  Revision 1.11  1994/05/25  14:28:28  ericr
**  Updated to Rev. 0.37 of internal EV5 OSF/1 PALcode
**
**  Revision 1.10  1994/05/24  21:15:38  ericr
**  BUG: swpipl: PS was being loaded with translated IPL, should be loaded 
**       with new IPL passed in r16.
**
**  BUG: callsys: Forgot to set Ibox/Mbox modes to kernel.
**
**  BUG: MM handlers: Miscalculated shift amount for isolating PTE write 
**       protection bits in PteInvalid handler
**
**  BUG: Dfault: GPR containing VA of faulting reference trashed by stack 
**       frame builder.
**
**  BUG: ItbMiss: EXC_ADDR must be saved in pt6 for use by the PteInvalid 
**       MM fault handler.
**
**  Revision 1.9  1994/05/20  18:08:23  ericr
**  Changed line comments to C++ style comment character
**
**  Revision 1.8  1994/02/28  18:45:57  ericr
**  Fixed EGORE related bugs
**
**  Revision 1.7  1994/01/31  19:29:26  ericr
**  Fixed PVC violation MT DC_MODE/MAF_MODE -> MBOX INST in RestoreState
**
**  Moved MT ICSR out of LD/ST shadow to fix PVC violation in SaveState 
**  and RestoreState
**
**  Fixed MT ICSR<FPE,HWE> -> HW_REI IN 3 restriction in WrFen
**
**  FEN -> OPCDEC should save PC+4 on stack
**
**  Revision 1.6  1994/01/26  22:29:17  ericr
**  Updated to Rev. 0.35 of internal EV5 OSF/1 PALcode
**
**  Revision 1.5  1994/01/06  18:12:38  ericr
**  Fixed numerous PVC violations
**
**  Revision 1.4  1994/01/03  19:34:03  ericr
**  Added ICSR initialization constant
**
**  Revision 1.3  1993/12/22  20:39:45  eric
**  Added PCB update routine
**  Added processor machine check handler
**  Filled out reset entry
**
**  Revision 1.2  1993/12/17  17:50:13  eric
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
	.asciz "$Id: osfpal.s,v 1.2 1999/04/15 20:56:22 paradis Exp $"
#endif

#include "dc21164.h"		// DECchip 21164 specific definitions
#include "osf.h"		// OSF/1 specific definitions
#include "macros.h"		// Global macro definitions
#include "impure.h"		// Scratch area data structure definitions
#if defined(DEBUG)
#include "platform.h"		// !!!! DEBUG !!!!
#endif

#define	PAL_K_MAJ	1	// PALcode major version
#define PAL_K_MIN	16	// PALcode minor version

#if !defined(PAL_K_REV)
#define PAL_K_REV	0	// System specific revision number
#endif


/*======================================================================*/
/*                        LOCAL MACRO DEFINITIONS                       */
/*======================================================================*/

#define ICSR_K_INIT \
	((1<<(ICSR_V_SDE-16)) | \
	 (2<<(ICSR_V_SPE-16)) | \
	 (1<<(ICSR_V_FPE-16)) | \
	 (1<<(ICSR_V_BSE-16)))

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

	.global SaveState
	.global RestoreState

	.global CallPal_Rti
	.global HaltDoubleMchk
	.global MchkCommon


/*======================================================================*/
/*              DECCHIP 21164 HARDWARE VECTOR ENTRY POINTS              */
/*======================================================================*/

/*
**                    Offset   Size
** Entry Point        (hex)   (bytes)	Why We Are Here
** ---------------    ------  -------	---------------------------------
**
** Reset               0000     128	Reset request
**
** I-stream ACV        0080     128	I-stream access violation
**
** Interrupt	       0100     128     Interrupt, or corrected 
**                                      hardware error
**
** I-stream TB Miss    0180     128     ITB miss
**
** D-stream TB Miss    0200     128     DTB miss from native code
** (single)
**
** D-stream TB Miss    0280     128     DTB miss from PAL code
** (double)
**
** Unaligned Data      0300     128     Unaligned data access
**
** D-stream Fault      0380     128     ACV, TNV, FOR, FOW
**
** Machine Check       0400     128     Uncorrected hardware error
**
** Illegal Opcode      0480     128     Reserved or privileged opcode
**
** Arithmetic Trap     0500     128	Arithmetic exception
**
** Floating Point      0580     128	FP operation attempted with:
** Disabled
**					FP instructions disabled.
**
**                                      FP IEEE round to +/- infinity.
**
**					FP IEEE with data type other
**                                      than S,T,Q
**
** Call PAL            2000             CALL_PAL instruction.
** (Privileged)				128 locations based on
**					instruction bits <7> & <5:0>
**
** Call PAL
** (Unprivileged)      3000
**
*/

	.text	0

	. = 0x0000

Pal_Base:


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  0000     128    Reset
**
**
** FUNCTIONAL DESCRIPTION:
** 
**	Reset initializes the DECchip 21164 to a known state, 
**	initializes the PAL state and the privileged processor 
**	state, performs any necessary system initialization, and
**	dispatches to the console/firmware entry point.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch when the
**	DECchip 21164 reset line is pulled low, or branched to
**	via the CALL_PAL SWPPAL instruction.
** 
** INPUT PARAMETERS:
** 
**	r2  (t1) = Base address of scratch (impure) area
**	r3  (t2) = Halt code
**
** OUTPUT PARAMETERS:
** 
** 
** SIDE EFFECTS:
**
**	Registers t0..t6, and v0 are UNPREDICTABLE upon return.
** 
*/ 
        HDW_VECTOR(PAL_RESET_ENTRY)

Pal_Reset:

	mtpr	t0, pt4			// Save t0 (bcControl in ARC).
        br	t0, ResetContinue	// Branch over static data.
/*
** This data block is used to grab the pointer to the impure area.
** Do not change it without changing the reset code that fetches the pointer!
*/
        .long	(PAL_K_MAJ<<8) | (PAL_K_MIN<<0)
        .long	(PAL_K_REV<<0)
        .long	_end
	.long	Pal_End		// Pointer to the end of PALcode
        .long	0

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  0080     128    I-Stream Access Violation
**
**
** FUNCTIONAL DESCRIPTION:
**
** 	The I-stream access violation trap handler builds a stack 
**	frame on the kernel stack, loads the GP with the KGP, loads 
**	the VA of the faulting target I-stream reference into a0, 
**	loads the access violation fault code into a1, loads the 
**	value -1 into a2 (I-fetch) indicating the faulting target
**	reference type, and dispatches to the kernel memory management 
**	fault handler pointed to by the entMM operating system entry 
**	point.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch on an I-stream 
**	access violation or PC sign check error.
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
** 
**	r16 (a0) = VA of faulting target reference
**	r17 (a1) = MM fault code (ACV)
**	r18 (a2) = Reference type (I-fetch)
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched from 
**	user to kernel mode.
** 
*/ 
        HDW_VECTOR(PAL_IACCVIO_ENTRY)

Pal_iAccVio:
	NOP				// E0
	mtpr	zero, ips		// Set the Ibox mode to kernel

	bis	zero, ps, p4		// Save a copy of the PS shadow
	mfpr	p5, excAddr		// Get VA of faulting target reference

	mtpr	zero, dtbCm		// Set the Mbox mode to kernel
	bic	p5, 3, p5		// Clean PC of faulting instruction

	STACK_FRAME(p4,p5,p7)

	mfpr	p6, ptEntMM		// Get pointer to kernel fault handler

	lda	a1, MM_K_ACV(zero)	// Pass ACV fault code in a1

	bis	zero, p5, a0		// Pass faulting VA in a0
	mtpr	p6, excAddr		// Load kernel fault handler entry pt
	subq	zero, 1, a2		// Pass I-fetch reference type in a2
	mfpr	gp, ptKgp		// Load the kernel global pointer

	hw_rei				// Transfer to kernel fault handler ...

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  0100     128    Interrupt
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The priority for parsing interrupt requests is as follows:
**	A pending HALT interrupt is serviced first, otherwise, the 
**	highest priority pending interrupt whose target interrupt 
**	priority level (IPL) is greater than the current processor 
**	IPL is serviced.  The interrupt handler builds a stack
**	frame on the kernel stack, loads the GP with the KGP, and
**	dispatches to the system specific interrupt handler.  
**
**	A PAL temporary register, ptIntMask, is used to map OSF/1
**	IPL encodings to DECchip 21164 internal IPL encodings. The 
**	contents of the INTID Ibox IPR, representing the target IPL 
**	of the highest priority pending interrupt, are stored encoded
**	as a DECchip 21164 internal IPL value.  A translation to an 
**	OSF/1 IPL is necessary in order to raise the IPL of the 
**	processor to the level of the interrupt before dispatching 
**	to the kernel interrupt handler.
**
**	The mapping between DECchip 21164 and OSF/1 IPL encodings is 
**	as follows:
**
**                  6                 3 3                 0
**                  3                 2 1                 0
**                 +----+----+----+----+----+----+----+----+
**      ptIntMask: | 1F | xx | xx | xx | xx | 02 | 01 | 00 |
**                 +----+----+----+----+----+----+----+----+
**
**      OSF/1 IPL:  IPL7 IPL6 IPL5 IPL4 IPL3 IPL2 IPL1 IPL0
**
**	xx = platform-specific
**
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch when an
**	interrupt occurs.
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
** 
**	r16 (a0) - Interrupt entry type
**	r17 (a1) - Platform-specific interrupt vector or UNPREDICTABLE
**	r18 (a2) - Pointer to machine check logout area or UNPREDICTABLE
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched from
**	user to kernel mode, and the current IPL of the processor 
**	may be raised.
** 
*/ 
        HDW_VECTOR(PAL_INTERRUPT_ENTRY)

Pal_Interrupt:
	mfpr	p7, isr			// Get summary of pending interrupts.
	srl	p7, ISR_V_HLT, p1	// Isolate the halt interrupt bit.

	mfpr	p6, ipl			// Get the IPL.
	NOP				// May delete when scheduling!!

	mfpr	p5, intId		// Get target IPL of pending interrupt.
	NOP				// May delete when scheduling!!

/*
** The HALT interrupt does not have a target IPL at which it can
** be masked.  On a HALT interrupt, INTID indicates the NEXT highest
** priority pending interrupt.  PALcode must first check the ISR<HLT>
** bit to determine if a HALT interrupt has occured before proceeding
** with the processing of interrupts with maskable target IPLs.
*/
	mtpr	zero, dtbCm		// Set the Mbox mode to kernel.
	blbs	p1, HaltHandler		// If halt interrupt, go handle ...
/*
** Not a HALT interrupt, so service the highest priority pending
** interrupt whose target IPL is greater than the current processor
** IPL.
*/
	cmple	p5, p6, p0		// Is target IPL > current IPL?
	bne	p0, PassiveRelease	// If not, ignore the interrupt.

	mfpr	p6, excAddr		// Get return addr to place on stack.
	bis	zero, ps, p1		// Save a copy of the PS shadow.

	STACK_FRAME(p1,p6,p4)		// Build a frame on kernel stack ...

					// Still need to update ps shadow
					//  with new IPL!!

                                        // Ibox PS is cleared if a store to the
pvc$osf00$354:				// stack faulted, so ignore violation
	mtpr	zero, ips		// VIRTUAL MBOX INST -> MT PS IN 0,1

	br	zero, InterruptContinue	// Continue handling elsewhere ...

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  0180     128    I-Stream TB Miss
**
**
** FUNCTIONAL DESCRIPTION:
**
**	Fetch the PTE of the target virtual address using a
**	virtual access method.  If the PTE is valid, load it
**	into the I-stream TB and continue, otherwise, sort 
**	out TNV, ACV, or FOE exception.
** 
**	Physical mode may be enabled by setting PTBR<0> = 1.
**	This allows a simple 1-to-1 va -> pa mapping to be
**	used in place of the page table translation.
**
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch when an
**	I-stream TB miss occurs.
** 
** INPUT PARAMETERS:
**
**	None 
** 
** OUTPUT PARAMETERS:
** 
**	None
** 
** SIDE EFFECTS:
**
**	The virtual access method used to fetch the PTE of
**	the target reference may potentially cause a double 
**	TB miss trap to occur.
** 
*/ 
        HDW_VECTOR(PAL_ITB_MISS_ENTRY)

Pal_ItbMiss:
	mfpr	p0, iFaultVaForm	// Get the VA of target reference's PTE
	mfpr	p2, excAddr		// Get PC in case we fault on PTE read

	NOP
	mtpr	p2, pt6			// Save faulting PC

Itb_LdVpte:
	ld_vpte	p0, 0(p0)		// Fetch the level 3 PTE (may fault)
	mtpr	p2, excAddr		// Restore original faulting PC

	mfpr	zero, va		// Unlock VA in case we double faulted
	mfpr	p1, ptPtbr		// Get the physical page table base

	and	p0, (PTE_M_FOE | PTE_M_VALID), p7 // Isolate FOE and V bits
	blbs	p1, 1f			// If physical mode, map va->pa 1-to-1

	cmpeq	p7, PTE_M_VALID, p7	// FOE should be clear, valid bit set.
	beq	p7, 2f			// If not, go sort out ACV/TNV/FOE.

	NOP
	mtpr	p0, itbPte		// Write the PTE and tag into the ITB

	hw_rei_stall			// Return and try access again ...
/*
**  Simple 1-to-1 va->pa mapping
*/
	ALIGN_BRANCH_TARGET

1:	srl	p2, VA_S_OFF, p1	// Clean off byte-within-page offset
	sll	p1, PTE_V_PFN, p1	// Shift to form PFN

	lda	p1, 0x3371(p1)		// Fabricate protection bits
	mtpr	p1, itbPte		// Write the PTE and tag into the ITB

	hw_rei_stall			// Return and try access again ...

	ALIGN_BRANCH_TARGET

2:	and	p0, PTE_M_VALID, p1	// Check if PTE valid bit is set.
	beq	p1, PteInvalid		// If not, go sort out ACV or TNV.

	br	zero, ItbFoe		// Otherwise, go sort out ACV or FOE.


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  0200     128    D-Stream TB Miss (Single)
**
**
** FUNCTIONAL DESCRIPTION:
**
**	Fetch the PTE of the target virtual address using a
**	virtual access method.  If the PTE is valid, load it
**	into the D-stream TB and continue, otherwise, sort 
**	out TNV, ACV, or FOx exception.
** 
**	Physical mode may be enabled by setting PTBR<0> = 1.
**	This allows a simple 1-to-1 va -> pa mapping to be
**	used in place of the page table translation.
**
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch when a
**	single D-stream TB miss occurs.
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
** 
**	None
** 
** SIDE EFFECTS:
**
**	The virtual access method used to fetch the PTE of
**	the target reference may potentially cause a double 
**	TB miss trap to occur.
** 
*/ 
        HDW_VECTOR(PAL_DTB_MISS_ENTRY)

Pal_DtbMiss:
	mfpr	p0, vaForm		// Get VA of target reference's PTE
	mfpr	p2, excAddr		// Get PC of faulting instruction

	mfpr	p1, mmStat		// Save MM_STAT in case we fault
	mtpr	p2, pt6			// Save PC in case we fault on PTE read

Dtb_LdVpte:
	ld_vpte	p0, 0(p0)		// Fetch the level 3 PTE (may miss or fault)
	mfpr	p2, ptPtbr		// Get the physical page table base

	blbs	p2, 1f			// If physical mode, map va->pa 1-to-1

	mfpr	p2, va			// Unlock VA in case we double faulted

	mtpr	p0, dtbPte		// Load the PTE
	blbc	p0, 2f			// Check if the PTE is invalid.

	mtpr	p2, dtbTag		// Write the PTE and tag into the DTB
	mfpr	p2, pt6			// Restore PC of faulting instruction

	mtpr	p2, excAddr		// Load PC
	STALL

	hw_rei				// Return and try access again ...
/*
**  Simple 1-to-1 va->pa mapping
*/
	ALIGN_BRANCH_TARGET

1:	mfpr	p0, va			// Get VA of target reference
	srl	p0, VA_S_OFF, p1	// Clean off byte-within-page offset

	sll	p1, PTE_V_PFN, p1	// Shift to form PFN
	lda	p1, 0x3371(p1)		// Fabricate protection bits

	mtpr	p1, dtbPte		// Load the PTE
	NOP				// E1

	mtpr	p0, dtbTag		// Write the PTE and tag into the DTB
	mfpr	p2, pt6			// Restore PC of faulting instruction

	mtpr	p2, excAddr		// Load PC
	STALL

	hw_rei				// Return and try access again ...

	ALIGN_BRANCH_TARGET

2:	NOP
	br	zero, PteInvalid	// Go sort out ACV/TNV ...


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  0280     128    D-Stream TB Miss (Double)
**
**
** FUNCTIONAL DESCRIPTION:
**
**	Fetch the PTE of the target virtual address using a
**	multi-level page table lookup.  If the PTE is valid, 
**	load it into the TB and continue, otherwise, sort out 
**	TNV or ACV exception.
** 
**	Physical mode may be enabled by setting PTBR<0> = 1.
**	This allows a simple 1-to-1 va -> pa mapping to be
**	used in place of the page table translation.
**
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch on a double
**	TB miss trap from the single TB miss handlers.
** 
** INPUT PARAMETERS:
** 
**	r8  (p0) = VA of target reference
**	r9  (p1) = MM_STAT or scratch
**	r10 (p2) = PC of faulting instruction
** 
** OUTPUT PARAMETERS:
** 
** 
** SIDE EFFECTS:
** 
*/ 
        HDW_VECTOR(PAL_DOUBLE_MISS_ENTRY)

Pal_DoubleMiss:
/*
**
** Fetch Level 1 PTE:
**
** Due to the virtual scheme, we can skip the level 1 lookup and go
** right to the level 2 lookup.
**
*/
	NOP				// E0
	mtpr	t8, pt4			// Save t8
/*
**
** Fetch Level 2 PTE:
**
** The Page Table Base Register (PTBR) contains the physical Page
** Frame Number (PFN) of the base of the highest level (level 2)
** page table. VA<SEG2> bits of the virtual address are used to 
** index into the level 2 page table to obtain the Page Table Entry
** (PTE) that maps the physical PFN of the base of the level 3 page
** table.  The level 2 PTE is accessed as follows:
**
**	L2PTE <- ({PTBR * PageSize} + {8 * VA<SEG2>})
**
** NOTE:  The PAL temporary register, ptPtbr, contains the Page Table 
** Base value shifted into the correct bit position. 
** (ptPtbr = PTBR * PageSize)
**
*/
	sll	p0, 64-(2*VA_S_SEG+VA_S_OFF), t8 // Clear off upper bits of VA
	mtpr	t9, pt5			// Save t9

	srl	t8, 61-VA_S_SEG, t8	// Get 8 * VA<SEG2>
	mfpr	t9, ptPtbr		// Get the page table base

	blbs	t9, 2f			// If physical mode, map va->pa 1-to-1
	addq	t9, t8, t9		// Form the address of level 2 PTE

	sll	p0, 64-(1*VA_S_SEG+VA_S_OFF), t8 // Clear off upper bits of VA
	ldq_p	t9, 0(t9)		// Read (physical) the L2PTE

	srl	t8, 61-VA_S_SEG, t8	// Get 8 * VA<SEG3>

	blbc	t9, FakePte 		// Check for an invalid L2PTE
/*
** Fetch Level 3 PTE:
**
** The level 2 PTE contains the physical PFN of the base of the level
** 3 page table.  VA<SEG3> bits of the virtual address are used to
** index into the level 3 page table to obtain the PTE that maps the
** physical PFN of the page being referenced.  The level 3 PTE is
** accessed as follows:
**
**	L3PTE <- ({L2PTE<PFN> * PageSize} + {8 * VA<SEG3>})
**
*/
	srl	t9, PTE_V_PFN, t9	// Extract level 2 PFN

	sll	t9, VA_S_OFF, t9	// L2PTE<PFN> * PageSize

	addq	t9, t8, t9		// Form the address of the level 3 PTE

	ldq_p	t9, 0(t9)		// Read (physical) the L3PTE
	blbc	t9, FakePte 		// Check for an invalid L3PTE
/*
** Update the TB:
**
** The translation buffer is loaded with the level 3 PTE that maps the
** physical PFN of the page being referenced.  This PFN is concatenated
** with the VA<OFFSET> bits (byte-within-page) of the virtual address to 
** obtain the physical address of the location being accessed.
**
*/
	mtpr	t9, dtbPte		// Load the PTE

	mfpr	t9, pt5			// Restore scratch registers
	mtpr	p0, dtbTag		// Write the PTE and tag into the DTB

1:	mfpr	t8, pt4 
	NOP				// Pad out tag write

	NOP				// No virtual references for 3 cycles
	NOP

	NOP
	NOP

	hw_rei				// Return and try access again ...

2:	mfpr	t8, excAddr		// Get PC of faulting reference.
	addq	t8, 4, t8		// Increment PC past offending PTE read
	mtpr	t8, excAddr		// Load PC.
	mfpr	t9, pt5			// Restore scratch registers.

	br	zero, 1b

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  0300     128    Unaligned Data Fault
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The unaligned data access fault handler builds a stack
**	frame on the kernel stack, loads the GP with the KGP,
**	loads the faulting VA into a0, loads the opcode of the
**	faulting instruction into a1, loads the target register
**	into a2, and dispatches to the kernel unaligned data
**	access handler pointed to by the entUna operating system
**	entry point.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch on an
**	unaligned D-stream reference.
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
** 
**	r16 (a0) = Faulting VA
**	r17 (a1) = Opcode of faulting instruction
**	r18 (a2) = src/dst register number
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched
**	from user to kernel mode.
** 
*/ 
        HDW_VECTOR(PAL_UNALIGN_ENTRY)

Pal_Unalign:
	bis	zero, ps, p4		// Save PS shadow
	mtpr	zero, ips		// Set Ibox mode to kernel

	mfpr	p5, mmStat		// Get MM_STAT
	mfpr	p6, excAddr		// Get PC of faulting instruction

	srl	p5, MMSTAT_V_RA, p5	// Shift Ra field into lsb
	blbs	p6, MchkBugCheck	// Check if unaligned in PAL

	mfpr	p7, va			// Unlock VA
	addq	p6, 4, p6		// Increment PC past faulting LD/ST

	mtpr	zero, dtbCm		// Set Mbox mode to kernel

	mtpr	p7, pt0			// Stash VA in case we miss on stack

	STACK_FRAME(p4,p6,p7)

	mfpr	p7, ptEntUna		// Get pointer to kernel fault handler

	and	p5, 0x1F, a2		// Clean Ra field to 5 bits, pass in a2

	mfpr	a0, pt0			// Pass unlocked VA in a0
	srl	p5, (MMSTAT_V_OPC-MMSTAT_V_RA), a1 // Pass opcode in a1

	mtpr	p7, excAddr		// Load the kernel handler entry point
	and	a1, 0x3F, a1		// Clean opcode field to 6 bits

	mfpr	gp, ptKgp		// Get the KGP

	hw_rei				// Transfer to kernel fault handler ...


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  0380     128    D-Stream Fault
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The D-stream fault handler builds a stack frame on the
**	kernel stack, loads the GP with the KGP, loads the VA
**	of the faulting D-stream reference into a0, sorts out
**	the reason for the fault (ignoring faults on FETCH and
**	FETCH_M instructions), loads the appropriate memory 
**	management fault code into a1 (ACV/FOR/FOW), loads a
**	value of either 0 or 1 into a0 (D-read/D-write) indicating
**	the faulting reference type, and dispatches to the kernel 
**	memory management fault handler pointed to by the entMM 
**	operating system entry point.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch on a
**	D-stream fault or VA sign check error.
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
**
**	r16 (a0) = VA of faulting reference
**	r17 (a1) = Memory management fault code (ACV/FOR/FOW)
** 	r18 (a2) = Reference type (R/W)
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched
**	from user to kernel mode.
** 
*/ 
        HDW_VECTOR(PAL_D_FAULT_ENTRY)

Pal_Dfault:
	NOP				// E0
	mtpr	zero, ips		// Set the Ibox mode to kernel

	mfpr	p5, mmStat		// Get the memory mgmt status bits
	mfpr	p0, excAddr		// Get PC of faulting instruction

	srl	p5, MMSTAT_V_OPC, p1	// Get opcode of faulting instruction
	bis	zero, ps, p4		// Save a copy of the PS shadow

	mtpr	zero, dtbCm		// Set the Mbox mode to kernel
	and	p1, 0x3F, p1		// Clean opcode to 6 bits

	and	p5, 0xF, p5		// Isolate mem mgmt status fault bits
	br	r31, DfaultContinue

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  0400     128    Machine Check
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The machine check handler flushes the I-cache and
**	dispatches to the system specific machine check
**	handler.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch on a
**	machine check.
** 
** INPUT PARAMETERS:
** 
** 
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
** 
*/ 
        HDW_VECTOR(PAL_MCHK_ENTRY)

Pal_Mchk:

	mtpr	zero, icFlush		// Flush the I-cache
	br	zero, MchkContinue	// Continue handling elsewhere ...

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  0480     128    Illegal Opcode Fault
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The reserved or privileged instruction fault handler builds
**	a stack frame on the kernel stack, loads the GP with the KGP,
**	loads the value 4 into a0 (OPCDEC), and dispatches to the
**	kernel illegal instruction fault handler pointed to by the 
**	entIF operating system entry point.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch on a
**	reserved or privileged instruction fault.
** 
** INPUT PARAMETERS:
** 
** 
** OUTPUT PARAMETERS:
**
**	r16 (a0) = Instruction fault code
**	r17 (a1) = UNPREDICTABLE
**	r18 (a2) = UNPREDICTABLE
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched
**	from user to kernel mode.
** 
*/ 
        HDW_VECTOR(PAL_OPCDEC_ENTRY)

Pal_OpcDec:
	bis	zero, ps, p4		// Save a copy of the PS shadow
	mtpr	zero, ips		// Set Ibox mode to kernel

	mfpr	p6, excAddr		// Get address of faulting instruction

	blbs	p6, MchkBugCheck	// Check for OPCDEC from PAL

	mtpr	zero, dtbCm		// Set Mbox mode to kernel
	addq	p6, 4, p6		// Increment PC past faulting instr

	STACK_FRAME(p4,p6,p7)

	mfpr	p5, ptEntIF		// Get pointer to kernel fault handler

	bis	zero, IF_K_OPCDEC, a0	// Pass OPCDEC code in a0

	mtpr	p5, excAddr		// Load the kernel handler entry point

	mfpr	gp, ptKgp		// Load the kernel global pointer

	hw_rei				// Transfer to kernel fault handler ...


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  0500     128    Arithmetic Trap
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The arithmetic trap handler builds a stack frame on
**	the kernel stack, loads the GP with the KGP, loads
**	the exception summary flags into a0, loads the dirty
**	register mask into a1, and dispatches to the kernel
**	arithmetic trap handler pointed to by the entArith
**	operating system entry point.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch when
**	an arithmetic trap/exception occurs.
** 
** INPUT PARAMETERS:
** 
** 
** OUTPUT PARAMETERS:
**
**	r16 (a0) = Exception Summary
**	r17 (a1) = Exception register mask
**	r18 (a2) = UNPREDICTABLE
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched
**	from user to kernel mode.
** 
*/ 
        HDW_VECTOR(PAL_ARITH_ENTRY)

Pal_Arith:
	NOP				// E0
	mtpr	zero, ips		// Set the Ibox mode to kernel

	bis	zero, ps, p4		// Save a copy of the PS shadow
	mfpr	zero, va		// Unlock the Mbox

	mtpr	zero, dtbCm		// Set the Mbox mode to kernel
	mfpr	p6, excAddr		// Get PC of faulting instruction

	NOP				// E0
	blbs	p6, MchkBugCheck	// Check for arith trap from PAL
	
	STACK_FRAME(p4,p6,p7)

	mfpr	p5, excSum		// Get the exception summary

	mfpr	a1, excMask		// Pass exception register mask in a1

	mfpr	p4, ptEntArith		// Get pointer to kernel trap handler 
	srl 	p5, EXC_V_SWC, a0	// Shift data into lsb, pass in a0

	mtpr	zero, excSum		// Unlock exc summary and register mask

	mtpr	p4, excAddr		// Load the kernel handler entry point

	mfpr	gp, ptKgp		// Load the kernel global pointer

	hw_rei				// Transfer to kernel fault handler ...

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  0580     128    Floating Point Fault
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The floating point fault handler builds a stack frame
**	on the kernel stack, loads the GP with the KGP, loads
**	the value 3 (FP disabled) or 4 (OPCDEC) into a0, and
**	dispatches to the kernel instruction fault handler
**	pointed to by the operating system entIF entry point.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch when a
**	floating point operation is executed with FP disabled,
**	or with FP IEEE round to +/- infinity, or when an FP
**	IEEE operation is executed with data type field other
**	than S, T, or Q.
** 
** INPUT PARAMETERS:
** 
**	r16 (a0) = Instruction fault code 
**	r17 (a1) = UNPREDICTABLE
**	r18 (a2) = UNPREDICTABLE
** 
** OUTPUT PARAMETERS:
**
**	None
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched
**	from user to kernel mode.
** 
*/ 
        HDW_VECTOR(PAL_FEN_ENTRY)

Pal_Fen:
	mtpr	zero, dtbCm		// Set Mbox mode to kernel
	mtpr	zero, ips		// Set Ibox mode to kernel

	mfpr	p6, excAddr		// Get PC of faulting instruction

	blbs	p6, MchkBugCheck	// Check for FEN trap from PAL

	mfpr	p5, icsr		// Get the Ibox control/status
	bis	zero, ps, p4		// Save a copy of the PS shadow

	srl	p5, ICSR_V_FPE, p5	// Shift FP enable bit into lsb
	addq	p6, 4, p7		// Calculate PC+4

	cmovlbs	p5, p7, p6		// If FP enabled, use PC+4

	STACK_FRAME(p4,p6,p7)

	mfpr	p6, ptEntIF		// Get pointer to kernel fault handler

	and	p5, 1, p5		// Clean FP enable to just one bit

	addq	p5, IF_K_FEN, a0	// FEN if FP enable = 0, else OPCDEC

	mtpr	p6, excAddr		// Load the kernel handler entry point

	mfpr	gp, ptKgp		// Load the kernel global pointer

	hw_rei				// Transfer to kernel fault handler ...

/*======================================================================*/
/*                OSF/1 PRIVILEGED CALL_PAL ENTRY POINTS                */
/*======================================================================*/

/*
**			Offset
**	Entry Point	 (hex)	Description
**	-----------	------	-------------------------------
**	halt		 2000	Halt the processor
**	cflush		 2040	Cache flush
**	draina		 2080	Drain aborts
**	cserve		 2240	Console service
**	swppal		 2280	Swap PALcode
**	wripir		 2340	Write interprocessor interrupt request
**	rdmces		 2400	Read machine check error summary
**	wrmces		 2440	Write machine check error summary
**	wrfen		 2AC0	Write floating point enable
**	wrvptptr	 2B40	Write virtual page table pointer
**	swpctx		 2C00	Swap process context
**	wrval		 2C40	Write system value
**	rdval		 2C80	Read system value
**	tbi		 2CC0	Translation buffer invalidate
**	wrent		 2D00	Write system entry address
**	swpipl		 2D40	Swap IPL
**	rdps		 2D80	Read processor status
**	wrkgp		 2DC0	Write kernel global pointer
**	wrusp		 2E00	Write user stack pointer
**	rdusp		 2E80	Read user stack pointer
**	whami		 2F00	Who am I
**	retsys		 2F40	Return from system call
**	rti		 2FC0	Return from trap, fault, or interrupt
*/
	.text	1

	. = 0x2000

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2000     64     Halt
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The halt function (halt) stops normal instruction processing, 
**	and depending on the HALT action, either enters console mode 
**	or the restart sequence.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
** 
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
** 
*/ 
        CALL_PAL_PRIV(PAL_HALT_ENTRY)

CallPal_Halt:

	STALL
	STALL

	mfpr	p4, excAddr		// Get PC of instr following HALT
	subq	p4, 4, p4		// Decrement PC to point to HALT
	mtpr	p4, excAddr		// Load PC of HALT

	mtpr	v0, pt0			// Save r0
pvc$osf01$5000:				// PVC subroutine branch origin
	bsr	v0, UpdatePCB		// Update the PCB

	lda	v0, HLT_K_SW_HALT(zero)	// Set halt code to software halt
	br	zero, Sys_EnterConsole	// Go enter the console ...

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2040     64     Cache Flush
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The cache flush (cflush) function flushes an entire physical 
**	page, specified by a PFN passed in a0, from all D-stream 
**	caches associated with the current processor.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	r16 (a0) = PFN of the page to be flushed
** 
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
** 
*/ 
        CALL_PAL_PRIV(PAL_CFLUSH_ENTRY)

CallPal_Cflush:
	br	zero, Sys_Cflush



/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2080     64     Drain Aborts
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The drain aborts (draina) function stalls instruction 
**	issuing until all prior instructions are guaranteed to 
**	complete without incurring aborts.
**	
**	For the DECchip 21164 implementation, this means waiting
**	until all pending D-stream reads are returned.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
** 
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
** 
*/ 
        CALL_PAL_PRIV(PAL_DRAINA_ENTRY)

CallPal_Draina:
	ldah	p6, 0x100(zero)		// Initialize counter
	NOP				// E1

1:	subq	p6, 1, p6		// Decrement counter
	mfpr	p5, mafMode		// Fetch miss address file status

	srl	p5, MAF_V_DREAD_PENDING, p5 // Shift D-read pending bit to lsb
	ble	p6, CallPal_Halt	// If its been too long, then halt

	NOP				// E0
	blbs	p5, 1b			// Wait until all D-reads clear

	hw_rei

	CALL_PAL_PRIV(0x0003)

CallPal_OpcDec03:
	br	zero, CallPal_OpcDec

	CALL_PAL_PRIV(0x0004)

CallPal_OpcDec04:
	br	zero, CallPal_OpcDec

	CALL_PAL_PRIV(0x0005)

CallPal_OpcDec05:
	br	zero, CallPal_OpcDec

	CALL_PAL_PRIV(0x0006)

CallPal_OpcDec06:
	br	zero, CallPal_OpcDec

	CALL_PAL_PRIV(0x0007)

CallPal_OpcDec07:
	br	zero, CallPal_OpcDec

	CALL_PAL_PRIV(0x0008)

CallPal_OpcDec08:
	br	zero, CallPal_OpcDec


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2240     64     Console Service
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The console service (cserve) function performs console 
**	and/or platform implementation specific functions.
**
**	The primary use of this function is within the generic 
**	I/O console callback routines.  These routines require
**	that the console be able to translate virtual addresses.
**	Since the PALcode image used by the operating system 
**	may differ from that used by the console, the console
**	may not have direct knowledge of the active memory 
**	management policy.  This function may be used to provide
**	a "get physical address" translation of virtual addresses.
**
**	Similarly, to implement the generic I/O console callback
**	routines, the console may transition to/from console
**	I/O mode.  This function may also be used to implement
**	that transition.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	r0 (v0)           = Option selector
**	r16..r21 (a0..a5) = Implementation specific entry parameters
** 
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
**	Registers a0..a5, and v0 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_CSERVE_ENTRY)

CallPal_Cserve:
	br	zero, Sys_Cserve


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2280     64     Swap PALcode
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The swap PALcode (swppal) function replaces the current 
**	(active) PALcode by the specified new PALcode image.  
**	This function is intended for use by operating systems 
**	only during bootstraps and restarts, or during transitions 
**	to console I/O mode.
** 
**	The PALcode descriptor passed in a0 is interpreted as
**	either a PALcode variant or the base physical address
**	of the new PALcode image.  If a variant, the PALcode
**	image must have been previously loaded.  No PALcode
**	loading occurs as a result of this function. 
**
**	NOTE:
**	This implementation of SWPPAL does not support PALcode
**	variants.  If a variant is specified in a0, a check is
**	performed to determine whether the variant is OSF/1 or
**	not and the returned status is either unknown variant
**	(if not OSF/1) or variant not loaded.
**
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
**
**	r16 (a0) = New PALcode variant or base physical address
**	r17 (a1) = New PC
**	r18 (a2) = New PCB
**	r19 (a3) = New VptPtr
** 
** 
** OUTPUT PARAMETERS:
**
**	r0 (v0) = Returned status indicating:
**			0 - Success (PALcode was switched)
**			1 - Unknown PALcode variant
**			2 - Known PALcode variant, but PALcode not loaded
** 
** SIDE EFFECTS:
**
**	The following processor state is updated:
**
**	Processor State				At exit from SWPPAL
**	------------------------------------	------------------------------
**	ASN	Address space number		ASN in PCB passed in a2
**	FEN	Floating point enable		FEN in PCB passed in a2
**	IPL	Interrupt priority level	7
**	MCES	Machine check error summary	8
**	PCBB	Privileged context block	Address of PCB passed in a2
**	PC	Program counter			PC passed in a1
**	PS	Processor status		IPL=7, CM=kernel
**	PTBR	Page table base register	PFN in PCB passed in a2
**	unique	Processor unique value		unique in PCB passed in a2
**	WHAMI	Who-Am-I			unchanged
**	sysval	System value			unchanged
**	sp	Kernel stack pointer		KSP in PCB passed in a2
**	other IPRs				UNPREDICTABLE
**	r0					zero
**	GPRs					UNPREDICTABLE, except sp & r0
**	FPRs					UNPREDICTABLE
** 
*/ 
        CALL_PAL_PRIV(PAL_SWPPAL_ENTRY)

CallPal_SwpPal:
	cmpule	a0, 255, v0		// Check for PAL variant
	cmoveq	a0, a0, v0		// If a0=0, then ok

	blbc	v0, CallPal_SwpPalCont	// No, must be new PAL base address
	cmpeq	a0, 2, v0		// Is this variant OSF/1?

	blbc	v0, 1f			// If not, return as unknown
	mfpr	a0, palBase		// Get PAL base address -- ISP/EGORE

	br	zero, CallPal_SwpPalCont

1:	addq	v0, 1, v0		// Return as unknown
	hw_rei

	
	CALL_PAL_PRIV(0x000B)

CallPal_OpcDec0B:
	br	zero, CallPal_OpcDec

	CALL_PAL_PRIV(0x000C)

CallPal_OpcDec0C:
	br	zero, CallPal_OpcDec

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  --------------------------------------
**  2340     64     Write Interprocessor Interrupt Request
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The write interprocessor interrupt request (wripir)
**	function generates an interprocessor interrupt on 
**	the processor number passed in register a0.  The 
**	interrupt request is recorded on the target processor 
**	and is initiated when the proper enabling conditions 
**	are present.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
**
**	r16 (a0) = target processor number 
** 
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
** 
*/ 
        CALL_PAL_PRIV(PAL_WRIPIR_ENTRY)

CallPal_WrIpir:
	br	zero, Sys_WrIpir

	
	CALL_PAL_PRIV(0x000E)

CallPal_OpcDec0E:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x000F)

CallPal_OpcDec0F:
	br	zero, CallPal_OpcDec

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2400     64     Read Machine Check Error Summary
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The read machine check error summary (rdmces) 
**	function returns the value of the Machine Check 
**	Error Summary (MCES) in register v0.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
** 
** OUTPUT PARAMETERS:
**
**	r0 (v0) = returned MCES value
** 
** SIDE EFFECTS:
**
** 
*/ 
        CALL_PAL_PRIV(PAL_RDMCES_ENTRY)

CallPal_RdMces:
	mfpr	v0, ptMces		// Get current MCES value
	and	v0, MCES_M_ALL, v0	// Clear all other bits

	hw_rei


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2440     64     Write Machine Check Error Summary
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The write machine check error summary (wrmces)
**	function selectively clears the machine check in 
**	progress bit, the system correctable error bit, 
**	or the processor correctable error bit.  The 
**	reporting of system or processor correctable errors 
**	are enabled/disabled with the value passed in register 
**	a0<4:3>.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	r16 (a0) = MCES<DPC> <- a0<3>,  MCES<DSC> <- a0<4>
** 
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
**	Registers t0, t8..t11, and a0 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_WRMCES_ENTRY)

CallPal_WrMces:
	and	a0, (MCES_M_MIP | MCES_M_SCE | MCES_M_PCE), p5		
	mfpr	p6, ptMces		// Get current MCES value

	ornot	zero, p5, p5		// Flip the bits
	and	a0, (MCES_M_DPC | MCES_M_DSC), p4 // Isolate DPC and DSC

	and	p6, p5, t0		// Clear MIP, SCE, PCE
	bic	t0, (MCES_M_DPC | MCES_M_DSC), t0 // Clear old DPC and DSC

	or	t0, p4, t0		// Update DPC and DSC
	mtpr	t0, ptMces		// Write MCES back

	NOP
	NOP

	hw_rei
	
	CALL_PAL_PRIV(0x0012)

CallPal_OpcDec12:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0013)

CallPal_OpcDec13:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0014)

CallPal_OpcDec14:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0015)

CallPal_OpcDec15:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0016)

CallPal_OpcDec16:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0017)

CallPal_OpcDec17:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0018)

CallPal_OpcDec18:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0019)

CallPal_OpcDec19:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x001A)

CallPal_OpcDec1A:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x001B)

CallPal_OpcDec1B:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x001C)

CallPal_OpcDec1C:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x001D)

CallPal_OpcDec1D:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x001E)

CallPal_OpcDec1E:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x001F)

CallPal_OpcDec1F:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0020)

CallPal_OpcDec20:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0021)

CallPal_OpcDec21:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0022)

CallPal_OpcDec22:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0023)

CallPal_OpcDec23:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0024)

CallPal_OpcDec24:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0025)

CallPal_OpcDec25:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0026)

CallPal_OpcDec26:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0027)

CallPal_OpcDec27:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0028)

CallPal_OpcDec28:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x0029)

CallPal_OpcDec29:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x002A)

CallPal_OpcDec2A:
	br	zero, CallPal_OpcDec

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2AC0     64     Write Floating Point Enable
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The write floating point enable (wrfen) function writes
**	the value passed in a0<0> to the floating point enable 
**	register.  The FEN value is also written to the PCB at 
**	location (PCBB+40).
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	r16 (a0) = ICSR<FPE> <- a0<0>
** 
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
**	Registers t0, t8..t11, and a0 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_WRFEN_ENTRY)

CallPal_WrFen:
	or	zero, 1, p5		// Get a '1'
	mfpr	t0, icsr		// Get current ICSR value

	sll	p5, ICSR_V_FPE, p5	// Shift '1' to ICSR<FPE> position
	and	a0, 1, a0		// Clean new FEN value to single bit

	sll	a0, ICSR_V_FPE, p4	// Shift new FEN into correct position
	bic	t0, p5, t0		// Clear ICSR<FPE>

	or	t0, p4, t0		// Write new FEN into ICSR
	mfpr	p4, ptPcbb		// Get PCBB

	mtpr	t0, icsr		// Write back ICSR
	stl_p	a0, PCB_Q_FEN(p4)	// Write new PCB<FEN>

	STALL
	STALL
	STALL
					// PVC can't distinguish which bits
pvc$osf02$225:				// have changed, so ignore violation  
	hw_rei				// MT ICSR --> HW_REI in 0,1,2,3,4

	
	CALL_PAL_PRIV(0x002C)

CallPal_OpcDec2C:
	br	zero, CallPal_OpcDec

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2B40     64     Write Virtual Page Table Pointer
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The write virtual page table pointer (wrvptptr)
**	function writes the pointer passed in a0 to the 
**	virtual page table pointer register (VPTPTR).
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
**
**	r16 (a0) = New virtual page table pointer 
** 
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
**	Registers t0, t8..t11, and a0 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_WRVPTPTR_ENTRY)

CallPal_WrVptPtr:
	mtpr	a0, mVptBr		// Load Mbox copy
	mtpr	a0, iVptBr		// Load Ibox copy

	NOP
	NOP

	hw_rei
	
	CALL_PAL_PRIV(0x002E)

CallPal_OpcDec2E:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_PRIV(0x002F)

CallPal_OpcDec2F:
	br	zero, CallPal_OpcDec

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2C00     64     Swap Process Context
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The swap process context (swpctx) function saves 
**	the current process data in the current PCB, then 
**	switches to the PCB passed in a0 and loads the
**	new process context.  The old PCB is returned in
**	v0.	
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	r16 (a0) = New PCBB 
** 
** OUTPUT PARAMETERS:
**
**	r0  (v0) = Old PCBB
** 
** SIDE EFFECTS:
**
**	Registers t0, t8..t11, and a0 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_SWPCTX_ENTRY)

CallPal_SwpCtx:
	rpcc	p5			// Get cycle counter
	mfpr	v0, ptPcbb		// Get current PCBB

	ldq_p	t8, PCB_Q_FEN(a0)	// Get new FEN
	ldq_p	t9, PCB_L_PCC(a0)	// Get new ASN 

	srl	p5, 32, p7		// Move CC<OFFSET> to low longword
	mfpr	t10, ptUsp		// Get the user stack pointer

	stq_p	sp, PCB_Q_KSP(v0)	// Store old kernel stack pointer

                                        // HW_ST can't trap except replay.  
                                        // Only problem if MF same IPR in
pvc$osf03$379:				// same shadow, so ignore violation
	mtpr	a0, ptPcbb		// PHYSICAL MBOX INST -> MT PT23 IN 0,1

	stq_p	t10, PCB_Q_USP(v0)	// Store user stack pointer
	addl	p5, p7, p7		// Set new time

	stl_p	p7, PCB_L_PCC(v0)	// Store new time
	bis	zero, 1, t10		// Get a '1'

	sll	t10, ICSR_V_FPE, t10	// Shift '1' into ICSR<FPE> position
	mfpr	p7, icsr		// Get current ICSR value

	and	t8, 1, t8		// Clean new FEN value to single bit
	br	zero, CallPal_SwpCtxCont

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2C40     64     Write System Value
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The write system value (wrval) function writes the 
**	value passed in a0 to a 64-bit system value register.  
**	The combination of wrval with the rdval function allows 
**	access by the operating system to a 64-bit per-processor 
**	value.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
**
**	r16 (a0) = New system value 
** 
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
**	Registers t0, t8..t11, and a0 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_WRVAL_ENTRY)

CallPal_WrVal:
	NOP
	mtpr	a0, ptSysVal

	NOP
	NOP

	hw_rei


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2C80     64     Read System Value
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The read system value (rdval) function returns 
**	the contents of the 64-bit system value register 
**	in v0, allowing access to a 64-bit per-processor 
**	value for use by the operating system.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
** 
** OUTPUT PARAMETERS:
**
**	r0 (v0) = Returned system value
** 
** SIDE EFFECTS:
**
**	Registers t0 and t8..t11 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_RDVAL_ENTRY)

CallPal_RdVal:
	NOP
	mfpr	v0, ptSysVal

	NOP
	hw_rei


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2CC0     64     Translation Buffer Invalidate
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The translation buffer invalidate (tbi) function 
**	removes specified entries from the I and D-stream
**	translation buffers when the mapping changes.
**	Specific entry types are removed based on the
**	selection value passed in a0.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	r16 (a0) = tbi selector type:
**
**		-2 - Flush all TB entries (tbia)
**		-1 - Invalidate all TB entries with ASM=0 (tbiap)
**		 1 - Invalidate ITB entry for va=a1 (tbisi)
**		 2 - Invalidate DTB entry for va=a1 (tbisd)
**		 3 - Invalidate both ITB and DTB entry for va=a1 (tbis)
**
**	r17 (a1) = VA for TBISx types
** 
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
**	Registers t0, t8..t11, and a0 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_TBI_ENTRY)

CallPal_Tbi:
	addq	a0, 2, a0		// Change selector range to 0-5
	br	t9, 1f			// Get base address of our code

1:	cmpult	a0, 6, t8		// Check if selector is in range
	lda	t9, Tbi_Table-1b(t9)	// Set base to start of table

	sll	a0, 4, a0		// Form index into jump table
	blbc	t8, 2f			// Return if out of range

	addq	t9, a0, t9		// Form jump table target address

pvc$osf04$2000:				// PVC computed goto origin
	jmp	zero, (t9)		// Go do it!

2:	hw_rei
	NOP


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2D00     64     Write System Entry Address
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The write system entry address (wrent) function 
**	determines the specific system entry point, based
**	on the value passed in register a1, and sets the 
**	virtual address of the specified system entry point 
**	to the value passed in register a0.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
**
**	r16 (a0) = VA of system entry point
**	r17 (a1) = System entry point selector 
** 
** OUTPUT PARAMETERS:
**
**	None
** 
** SIDE EFFECTS:
**
**	Registers t0, t8..t11, and a0..a1 are UNPREDICTABLE
**	upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_WRENT_ENTRY)

CallPal_WrEnt:

#if defined(KDEBUG)
	cmpult	a1, 7, t8		// Check if entry selector is in range
#else
	cmpult	a1, 6, t8
#endif /* KDEBUG */
	br	t9, 1f			// Get base address of our code

1:	bic	a0, 3, a0		// Clean PC<1:0>
	blbc	t8, 2f			// Return if selector is out of range

	lda	t9, WrEnt_Table-1b(t9)	// Set base to start of table
	sll	a1, 4, a1		// Form index into jump table

	addq	a1, t9, t9		// Form jump table target address

pvc$osf05$3000:				// PVC computed goto origin
	jmp	zero, (t9)		// Go do it!

2:	hw_rei


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2D40     64     Swap Interrupt Priority Level
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The swap IPL (swpipl) function returns the 
**	current value of the PS<IPL> bits in the 
**	register v0 and sets the IPL to the value
**	passed in register a0.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	r16 (a0) = New IPL
** 
** OUTPUT PARAMETERS:
**
**	r0  (v0) = Old IPL
** 
** SIDE EFFECTS:
**
**	Registers t0, t8..t11, and a0 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_SWPIPL_ENTRY)

CallPal_SwpIpl:
	and	a0, PS_M_IPL, a0	// Clean new IPL value
	mfpr	t8, ptIntMask		// Get the IPL translation mask

	extbl	t8, a0, t8		// Get translation for new IPL
	bis	zero, ps, v0		// Load old IPL

	bis	zero, a0, ps		// Load new IPL
	NOP				// Pad PAL shadow write

	mtpr	t8, ipl			// Set new IPL

pvc$osf06$217:				// MT IPL --> HW_REI violation ok here
	hw_rei


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2D80     64     Read Processor Status
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The read processor status (rdps) function returns 
**	the processor status (PS) value in the register v0. 
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
**
**	r0 (v0) = Current PS
** 
** SIDE EFFECTS:
**
**	Registers t0, t8..t11 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_RDPS_ENTRY)

CallPal_RdPs:
	bis	zero, ps, v0		// Return PS shadow
	NOP

	hw_rei


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2DC0     64     Write Kernel Global Pointer
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The write kernel global pointer (wrkgp) function
**	writes the value passed in register a0 to the
**	kernel global pointer (KGP) internal register.
**	The KGP is used to load the global pointer (GP)
**	on exceptions.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	r16 (a0) = New KGP value
** 
** OUTPUT PARAMETERS:
**
**	None
** 
** SIDE EFFECTS:
**
**	Registers t0, t8..t11, and a0 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_WRKGP_ENTRY)

CallPal_WrKgp:
	NOP
	mtpr	a0, ptKgp		// Load the new KGP 

	NOP				// Pad PALtemp write
	NOP

	hw_rei


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2E00     64     Write User Stack Pointer
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The write user stack pointer (wrusp) function writes 
**	the value passed in register a0 to the user stack 
**	pointer.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	r16 (a0) = New user stack pointer value
** 
** OUTPUT PARAMETERS:
**
**	None
** 
** SIDE EFFECTS:
**
**	Registers t0, t8..t11, and a0 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_WRUSP_ENTRY)

CallPal_WrUsp:
	NOP
	mtpr	a0, ptUsp		// Load the new user stack pointer

	NOP				// Pad out PALtemp write
	NOP

	hw_rei

	
	CALL_PAL_PRIV(0x0039)

CallPal_OpcDec39:
	br	zero, CallPal_OpcDec

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2E80     64     Read User Stack Pointer
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The read user stack pointer (rdusp) function returns 
**	the user stack pointer value in register v0.  The 
**	user stack pointer is written by the CALL_PAL function 
**	wrusp.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
**
**	r0 (v0) = User stack pointer value
** 
** SIDE EFFECTS:
**
**	Registers t0, and t8..t11 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_RDUSP_ENTRY)

CallPal_RdUsp:
	NOP
	mfpr	v0, ptUsp		// Return user stack pointer in v0

	hw_rei

	
	CALL_PAL_PRIV(0x003B)

CallPal_OpcDec3B:
	br	zero, CallPal_OpcDec

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2F00     64     Who Am I
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The who-am-I (whami) function returns the processor 
**	number for the current processor in register v0.  The  
**	processor number returned is in the range 0..maxCPU-1 
**	processors that can be configured in the system.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
**
**	None 
** 
** OUTPUT PARAMETERS:
**
**	r0 (v0) = Current processor number
** 
** SIDE EFFECTS:
**
**	Registers t0 and t8..t11 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_WHAMI_ENTRY)

CallPal_Whami:
	NOP
	mfpr	v0, ptWhami		// Return Who-Am-I identifier in v0

	extbl	v0, 1, v0		// Isolate just the WHAMI bits
	hw_rei


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  2F40     64     Return From System Call
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The return from system call (retsys) function pops the 
**	return address and the user mode global pointer from 
**	the kernel stack, saves the kernel stack pointer, sets 
**	the mode to user, set the IPL to zero, and enters the 
**	user mode code at the return address popped off the stack.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	r30 (sp) = Pointer to the top of the kernel stack
** 
** OUTPUT PARAMETERS:
**
**	r29 (gp) = Restored user mode global pointer
**	r30 (sp) = User stack pointer
** 
** SIDE EFFECTS:
**
**	Registers t0 and t8..t11 are UNPREDICTABLE upon return.
** 
*/ 
        CALL_PAL_PRIV(PAL_RETSYS_ENTRY)

CallPal_RetSys:
	lda	p7, FRM_K_SIZE(sp)	// Pop the stack
	bis	zero, p7, p6		// Stall mfpr excAddr

	mfpr	p6, excAddr		// Save excAddr in case we fault
	ldq	t9, FRM_Q_PC(sp)	// Pop the return address

	ldq	gp, FRM_Q_GP(sp)	// Get the user mode global pointer
	stl_c	zero, -4(sp)		// Clear the lock flag

	lda	ps, (1<<PS_V_CM)(zero)	// Set new mode to user
	mfpr	sp, ptUsp		// Get the user stack pointer

	bic	t9, 3, t9		// Clean return PC<1:0>
	mtpr	p7, ptKsp		// Save the kernel stack pointer

	mtpr	ps, dtbCm		// Set the Mbox mode to user
	mtpr	ps, ips			// Set the Ibox mode to user

	rc	zero			// Clear the inter_flag
	mtpr	t9, excAddr		// Load the return address

	mtpr	zero, ipl		// Set the Ibox IPL

pvc$osf07$217:				// MT IPL --> HW_REI violation ok here
	hw_rei 

	
	CALL_PAL_PRIV(0x003E)

CallPal_OpcDec3E:
	br	zero, CallPal_OpcDec

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  -------------------------------------
**  2FC0     64     Return From Trap, Fault, or Interrupt
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The return from trap, fault or interrupt (rti) 
**	function pops registers a0..a2, the GP, the PC, 
**	and the PS off the kernel stack.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	r30 (sp) = Pointer to the top of the kernel stack
** 
** OUTPUT PARAMETERS:
**
**	ps       <- (sp+00)
**	pc       <- (sp+08)
**	r29 (gp) <- (sp+16)
**	r16 (a0) <- (sp+24)
**	r17 (a1) <- (sp+32)
**	r18 (a2) <- (sp+40)
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched
**	from kernel to user mode.
** 
*/ 
        CALL_PAL_PRIV(PAL_RTI_ENTRY)

CallPal_Rti:
	lda	p7, FRM_K_SIZE(sp)	// Pop the stack
	bis	zero, p7, p6		// Stall mfpr excAddr

	mfpr	p6, excAddr		// Save PC in case we fault
	rc	zero			// Clear the inter_flag

	ldq	p4, -6*8(p7)		// Get the PS
	ldq	p5, -5*8(p7)		// Get the return PC

	ldq	a2, -1*8(p7)		// Get a2
	ldq	a1, -2*8(p7)		// Get a1

	ldq	a0, -3*8(p7)		// Get a0
	ldq	gp, -4*8(p7)		// Get gp

	bic	p5, 3, p5		// Clean return PC<1:0>
	stl_c	zero, -4(p7)		// Clear the lock flag

	and	p4, PS_M_CM, ps		// Get the current mode
	mtpr	p5, excAddr		// Load the return address

	beq	ps, CallPal_Rti_ToKern	// rti back to kernel mode
	br	zero, CallPal_Rti_ToUser// rti back to user mode

/*======================================================================*/
/*               OSF/1 UNPRIVILEGED CALL_PAL ENTRY POINTS               */
/*======================================================================*/

/*
**			Offset
**	Entry Point	 (hex)	Description
**	-----------	------	----------------------------
**	bpt		 3000	Breakpoint trap
**	bugchk		 3040	Bugcheck trap
**	callsys		 30C0	System call
**	imb		 3180	I-stream memory barrier
**	rdunique	 3780	Read unique
**	wrunique	 37C0	Write unique
**	gentrap		 3A80	Generate trap
**/
	.text	1

	. = 0x3000

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  3000     64     Breakpoint Trap
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The breakpoint trap (bpt) function builds a stack
**	frame on the kernel stack, loads the GP with the 
**	KGP, loads a value of 0 into a0, and dispatches
**	to the kernel instruction fault handler pointed 
**	to by the entIF operating system entry point.
**
**	The saved PC at (SP+8) is the address of the 
**	instruction following the instruction that
**	caused the trap.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
**
**	r16 (a0) = Code for bpt (0)
**	r17 (a1) = UNPREDICTABLE
**	r18 (a2) = UNPREDICTABLE
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched
**	from user to kernel mode.
** 
*/ 
        CALL_PAL_UNPRIV(PAL_BPT_ENTRY)

CallPal_Bpt:
	bis	zero, ps, p4		// Save PS shadow for stack write
	mtpr	zero, ips		// Set the Ibox mode to kernel

	mtpr	zero, dtbCm		// Set the Mbox mode to kernel
	br	zero, CallPal_BptCont

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  3040     64     Bugcheck Trap
** 
**
** FUNCTIONAL DESCRIPTION:
**
**	The bugcheck trap (bugchk) function builds a stack 
**	frame on the kernel stack, loads the GP with the
**	KGP, loads a value of 1 into a0, and dispatches to
**	the kernel instruction fault handler pointed to by
**	the entIF operating system entry point.
**
**	The saved PC at (SP+8) is the address of the instruction
**	following the instruction that caused the trap.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
**
**	r16 (a0) = Code for bugchk (1)
**	r17 (a1) = UNPREDICTABLE
**	r18 (a2) = UNPREDICTABLE
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched 
**	from user to kernel mode.
** 
*/ 
        CALL_PAL_UNPRIV(PAL_BUGCHK_ENTRY)

CallPal_BugChk:
	bis	zero, ps, p4		// Save PS shadow for stack write
	mtpr	zero, ips		// Set the Ibox mode to kernel

	mtpr	zero, dtbCm		// Set the Mbox mode to kernel
	br	zero, CallPal_BugChkCont
	
	CALL_PAL_UNPRIV(0x0082)

CallPal_OpcDec82:
	br	zero, CallPal_OpcDec

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  30C0     64     System Call
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The system call (callsys) function builds a callsys 
**	stack frame on the kernel stack, loads the GP with
**	the KGP, and dispatches to the kernel system call
**	handler pointed to by the entSys operating system
**	entry point.
**
**	The callsys function is supported only from user
**	mode.  Issuing a callsys from kernel mode causes
**	a machine check exception to occur.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
**
**	None
** 
** SIDE EFFECTS:
**
** 
*/ 
        CALL_PAL_UNPRIV(PAL_CALLSYS_ENTRY)

CallPal_CallSys:
	and	ps, PS_M_CM, t10	// Get the current mode
	mfpr	t8, ptKsp		// Get the kernel stack pointer

	beq	t10, CalPal_CallSys_FromKern
					// callsys from kernel mode not allowed
	mfpr	t9, excAddr		// Get PC 

	br	zero, CallPal_CallSysCont
	
	CALL_PAL_UNPRIV(0x0084)

CallPal_OpcDec84:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x0085)

CallPal_OpcDec85:
	br	zero, CallPal_OpcDec

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  3180     64     I-Stream Memory Barrier
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The I-stream memory barrier (imb) function flushes
**	the write buffer and I-cache, making I-stream coherent
**	with D-stream.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
** 
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
** 
*/ 
        CALL_PAL_UNPRIV(PAL_IMB_ENTRY)

CallPal_Imb:
	mb				// Clear the write buffer
	mfpr	zero, mcsr		// Synchronize with clear

	NOP
	NOP

	br	zero, CallPal_IcFlushCont	// Go flush the I-cache.

	
	CALL_PAL_UNPRIV(0x0087)

CallPal_OpcDec87:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x0088)

CallPal_OpcDec88:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x0089)

CallPal_OpcDec89:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x008A)

CallPal_OpcDec8A:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x008B)

CallPal_OpcDec8B:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x008C)

CallPal_OpcDec8C:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x008D)

CallPal_OpcDec8D:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x008E)

CallPal_OpcDec8E:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x008F)

CallPal_OpcDec8F:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x0090)

CallPal_OpcDec90:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x0091)

CallPal_OpcDec91:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x0092)

CallPal_OpcDec92:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x0093)

CallPal_OpcDec93:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x0094)

CallPal_OpcDec94:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x0095)

CallPal_OpcDec95:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x0096)

CallPal_OpcDec96:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x0097)

CallPal_OpcDec97:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x0098)

CallPal_OpcDec98:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x0099)

CallPal_OpcDec99:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x009A)

CallPal_OpcDec9A:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x009B)

CallPal_OpcDec9B:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x009C)

CallPal_OpcDec9C:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x009D)

CallPal_OpcDec9D:
	br	zero, CallPal_OpcDec

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  3780     64     Read Unique Value
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The read unique (rdunique) function returns the 
**	process unique value in the register v0.  The
**	write unique (wrunique) function set the process
**	unique value.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
**
**	r0 (v0) = Returned process unique value
** 
** SIDE EFFECTS:
**
** 
*/ 
        CALL_PAL_UNPRIV(PAL_RDUNIQUE_ENTRY)

CallPal_RdUnique:
	mfpr	v0, ptPcbb		// Get the process control block info
	ldq_p	v0, PCB_Q_UNIQUE(v0)	// Get the unique value

	hw_rei


/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  37C0     64     Write Unique Value
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The write unique (wrunique) function sets the process
**	unique register to the value passed in register a0.
**	The read unique (rdunique) function returns the process
**	unique value.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	r16 (a0) = New process unique value
** 
** OUTPUT PARAMETERS:
**
**	None
** 
** SIDE EFFECTS:
**
** 
*/ 
        CALL_PAL_UNPRIV(PAL_WRUNIQUE_ENTRY)

CallPal_WrUnique:
	NOP				// E0
	mfpr	p4, ptPcbb		// Get the process control block info

	stq_p	a0, PCB_Q_UNIQUE(p4)	// Set the new value
	NOP

	hw_rei

	
	CALL_PAL_UNPRIV(0x00A0)

CallPal_OpcDecA0:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00A1)

CallPal_OpcDecA1:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00A2)

CallPal_OpcDecA2:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00A3)

CallPal_OpcDecA3:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00A4)

CallPal_OpcDecA4:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00A5)

CallPal_OpcDecA5:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00A6)

CallPal_OpcDecA6:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00A7)

CallPal_OpcDecA7:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00A8)

CallPal_OpcDecA8:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00A9)

CallPal_OpcDecA9:
	br	zero, CallPal_OpcDec

/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  3A80     64     Generate Trap
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The generate trap (gentrap) function builds a stack 
**	frame on the kernel stack, loads the GP with the KGP,
**	loads a value of 2 into a0, and dispatches to the
**	kernel instruction fault handler pointed to by the
**	entIF operating system entry point.
**
**	The saved PC at (SP+8) is the address of the instruction
**	following the instruction that caused the trap.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
**
**	r16 (a0) = Code for gentrap (2)
**	r17 (a1) = UNPREDICTABLE
**	r18 (a2) = UNPREDICTABLE
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched
**	from user to kernel mode.
** 
*/ 
        CALL_PAL_UNPRIV(PAL_GENTRAP_ENTRY)

CallPal_GenTrap:
	bis	zero, ps, p4		// Save PS shadow for stack write
	mtpr	zero, ips		// Set the Ibox mode to kernel

	mtpr	zero, dtbCm		// Set the Mbox mode to kernel
	br	zero, CallPal_GenTrapCont
	
	CALL_PAL_UNPRIV(0x00AB)

CallPal_OpcDecAB:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00AC)

CallPal_OpcDecAC:
	br	zero, CallPal_OpcDec
	
#if defined(KDEBUG)
/*
**
** Offset   Size
** (hex)   (bytes)  Entry Point Description
** ------  -------  ---------------------------------
**  3B40     64     Kernel Debugger Trap
**
**
** FUNCTIONAL DESCRIPTION:
**
**	The kernel debugger trap (gentrap) function builds a stack 
**	frame on the kernel stack, loads the GP with the KGP,
**	loads a value of 0 into a0, and dispatches to the
**	kernel debugger trap handler pointed to by the
**	entDbg operating system entry point.
**
**	The saved PC at (SP+8) is the address of the instruction
**	following the instruction that caused the trap.
** 
** CALLING SEQUENCE:
** 
**	Vectored into by hardware-activated dispatch via
**	the CALL_PAL instruction.
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
**
**	r16 (a0) = Code for dbgstop(2)
**	r17 (a1) = UNPREDICTABLE
**	r18 (a2) = UNPREDICTABLE
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched
**	from user to kernel mode.
** 
*/
	CALL_PAL_UNPRIV(PAL_DBGSTOP_ENTRY)

CallPal_DbgStop:
	bis	zero, ps, p4		// Save PS shadow for stack write
	mtpr	zero, ips		// Set the Ibox mode to kernel

	mtpr	zero, dtbCm		// Set the Mbox mode to kernel
	br	zero, CallPal_DbgStopCont

#else	
	CALL_PAL_UNPRIV(0x00AD)

CallPal_OpcDecAD:
	br	zero, CallPal_OpcDec

#endif /* KDEBUG */

	
	CALL_PAL_UNPRIV(0x00AE)

CallPal_OpcDecAE:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00AF)

CallPal_OpcDecAF:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00B0)

CallPal_OpcDecB0:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00B1)

CallPal_OpcDecB1:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00B2)

CallPal_OpcDecB2:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00B3)

CallPal_OpcDecB3:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00B4)

CallPal_OpcDecB4:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00B5)

CallPal_OpcDecB5:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00B6)

CallPal_OpcDecB6:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00B7)

CallPal_OpcDecB7:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00B8)

CallPal_OpcDecB8:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00B9)

CallPal_OpcDecB9:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00BA)

CallPal_OpcDecBA:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00BB)

CallPal_OpcDecBB:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00BC)

CallPal_OpcDecBC:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00BD)

CallPal_OpcDecBD:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00BE)

CallPal_OpcDecBE:
	br	zero, CallPal_OpcDec
	
	CALL_PAL_UNPRIV(0x00BF)

CallPal_OpcDecBF:
	br	zero, CallPal_OpcDec

/*======================================================================*/
/*                   OSF/1 CALL_PAL CONTINUATION AREA                   */
/*======================================================================*/

	.text	2

	. = 0x4000
/*
** Continuation of CallPal_SwpPal
*/

CallPal_SwpPalCont:
	mfpr	t0, ptMisc		// Get ptMisc
	zap	t0, 0x40, t0		// Clean <55:48>

	lda	v0, WHAMI_K_SWAP(zero)	// Load 'S'
	sll	v0, PT16_V_SWAP, v0	// Shift into position

	bis	t0, v0, t0		// Set the bit
	mtpr	t0, ptMisc		// Update the chip

	or	zero, zero, v0		// Set return status to success
pvc$osf08$1007:				// Ignore PVC violation on goto
	jmp	zero, (a0)		// Jump to new PALcode ...


/*
** Continuation for CallPal_SwpCtx
*/

	ALIGN_BLOCK

CallPal_SwpCtxCont:
	sll	t8, ICSR_V_FPE, t8	// Shift new FEN into position
	bic	p7, t10, p7		// Clean ISR<FPE> 

	srl	t9, 32, t10		// Move ASN to low longword
	ldq_p	p6, PCB_Q_PTBR(a0)	// Get the new page table base

	sll	t10, 4, p4		// Shift ASN into ITB position
	bis	p7, t8, p7		// Form ICSR value with new FEN

	sll	t10, 57, t10		// Shift ASN into DTB position
	subl	t9, p5, p5		// Generate new CC<OFFSET> bits

	mtpr	t10, dtbAsn		// Load new ASN
	mtpr	p4, itbAsn

/*
** If in the PCB, PTBR<63> = 1, put us in physical mode by setting the
** low bit of ptPtbr.
*/
	srl	p6, 63, p4
	sll	p6, VA_S_OFF, p6	// Shift PTBR into position
	bis	p6, p4, p6
	mtpr	p7, icsr		// Write the new ICSR

	insll	p5, 4, p5		// >> 32
	ldq_p	p7, PCB_Q_USP(a0)	// Get new user stack pointer

	mtpr	p5, cc			// Set new CC<OFFSET>

                                        // HW_LD can't trap except replay.
                                        // Only problem if MF same IPR in
pvc$osf09$379:				// same shadow, so ignore violation
	mtpr	p6, ptPtbr		// PHYSICAL MBOX INST -> MT PT20 IN 0,1

	ldq_p	sp, PCB_Q_KSP(a0)	// Get the new kernel stack pointer

                                        // HW_LD can't trap except replay.
                                        // Only problem if MF same IPR in
pvc$osf10$379:				// same shadow, so ignore violation
	mtpr	p7, ptUsp		// PHYSICAL MBOX INST -> MT PT18 IN 0,1

	hw_rei_stall


/*
** Table for TB invalidate operations
*/

	ALIGN_BLOCK

Tbi_Table:
/*
** -2 tbia - Flush all TBs
*/
pvc$osf11$2000.1:			// PVC computed goto target
	mtpr	zero, dtbIa		// Flush all D-stream TB entries
	mtpr	zero, itbIa		// Flush all I-stream TB entries
	hw_rei_stall
	NOP				// Pad out table entry
/*
** -1 tbiap - Invalidate all TB entries with ASM=0
*/
pvc$osf12$2000.2:			// PVC computed goto target
	mtpr	zero, dtbIap		// Flush D-stream TB entries
	mtpr	zero, itbIap		// Flush I-stream TB entries
	hw_rei_stall
	NOP				// Pad out table entry
/*
**  0 unused 
*/
pvc$osf13$2000.3:			// PVC computed goto target
	hw_rei				// Do nothing, just return 
	NOP				// Pad out table entry
	NOP
	NOP
/*
**  1 tbisi - Invalidate I-stream TB entry for va=a1
*/
pvc$osf14$2000.4:			// PVC computed goto target
	NOP
	NOP
	mtpr	a1, itbIs		// Invalidate I-stream TB entry.
	hw_rei_stall
/*
**  2 tbisd - Invalidate D-stream TB entry for va=a1
*/
pvc$osf15$2000.5:			// PVC computed goto target
	mtpr	a1, dtbIs		// Invalidate D-stream TB entry
	NOP				// Pad out table entry
	NOP
	br	zero, OneCycle_Then_hw_rei
/*
**  3 tbis - Invalidate both I and D-stream TB entries for va=a1
*/
pvc$osf16$2000.6:			// PVC computed goto target
	mtpr	a1, dtbIs		// Invalidate D-stream TB entry
	NOP
	mtpr	a1, itbIs		// Invalidate I-stream TB entry
	hw_rei_stall

/*
** Continuation of CallPal_icFlush
*/

	ALIGN_BLOCK

CallPal_IcFlushCont:
	NOP
	mtpr	zero, icFlush		// Flush the I-cache
	NOP
	NOP
/*
** 44 NOPs = 3 RFB prefetches (24) + IC buffer->IB->slot->issue (20)
*/
	NOP
	NOP
	NOP
	NOP

	NOP
	NOP
	NOP
	NOP

	NOP
	NOP				// 10

	NOP
	NOP
	NOP
	NOP

	NOP
	NOP
	NOP
	NOP
	
	NOP
	NOP				// 20

	NOP
	NOP
	NOP
	NOP

	NOP
	NOP
	NOP
	NOP
	
	NOP
	NOP				// 30

	NOP
	NOP
	NOP
	NOP

	NOP
	NOP
	NOP
	NOP

	NOP
	NOP				// 40

	NOP
	NOP
OneCycle_Then_hw_rei:
	NOP
	NOP
	hw_rei

/*
** Write Entry Table
*/

	ALIGN_BLOCK

WrEnt_Table:
pvc$osf17$3000.1:			// PVC computed goto target
	NOP
	mtpr	a0, ptEntInt		// Set interrupt entry address
	STALL
	hw_rei

pvc$osf18$3000.2:			// PVC computed goto target
	NOP
	mtpr	a0, ptEntArith		// Set arithmetic trap entry address
	STALL
	hw_rei

pvc$osf19$3000.3:			// PVC computed goto target
	NOP
	mtpr	a0, ptEntMM		// Set memory mgmt fault entry address
	STALL
	hw_rei

pvc$osf20$3000.4:			// PVC computed goto target
	NOP
	mtpr	a0, ptEntIF		// Set instruction fault entry address
	STALL
	hw_rei

pvc$osf21$3000.5:			// PVC computed goto target
	NOP
	mtpr	a0, ptEntUna		// Set unaligned fault entry address
	STALL
	hw_rei

pvc$osf22$3000.6:			// PVC computed goto target
	NOP
	mtpr	a0, ptEntSys		// Set system call entry address
	STALL
	hw_rei

#if defined(KDEBUG)

pvc$osf23$3000.7:			// PVC computed goto target
	NOP
	mtpr	a0, ptEntDbg		// Set kernel debugger entry address
	STALL
	hw_rei

#endif /* KDEBUG */


/*
** Continuation of CallPal_Rti
*/

	ALIGN_BLOCK

CallPal_Rti_ToKern:
	and	p4, PS_M_IPL, ps	// Clean PS
	mfpr	p4, ptIntMask		// Get the IPL translation mask

	extbl	p4, ps, p4		// Get translation for this IPL
	mtpr	p7, ptKsp		// Save kernel stack pointer

	mtpr	p4, ipl			// Set the new Ibox IPL
	bis	zero, p7, sp		// Set the new stack pointer

pvc$osf24$217:				// MT IPL --> HW_REI violation ok here
	hw_rei

	ALIGN_BLOCK

CallPal_Rti_ToUser:
	mtpr	ps, dtbCm		// Set the Mbox mode to current
	mtpr	ps, ips			// Set the Ibox mode to current

	mfpr	sp, ptUsp		// Get the user stack pointer
	mtpr	zero, ipl		// Set the new Ibox IPL

	mtpr	p7, ptKsp		// Save the kernel stack pointer

pvc$osf25$248:				// MT IPL --> HW_REI violation ok here
	hw_rei


/*
** Continuation of CallPal_Bpt
*/

	ALIGN_BLOCK

CallPal_BptCont:
	mfpr	p6, excAddr		// Get PC following trap instruction

	STACK_FRAME(p4,p6,p7)

	mfpr	p5, ptEntIF		// Get pointer to kernel fault handler

	bis	zero, IF_K_BPT, a0	// Load code indicating breakpoint trap
	mtpr	p5, excAddr		// Load the kernel entry point

	mfpr	gp, ptKgp		// Load the kernel global pointer
	hw_rei

/*
** Continuation of CallPal_BugChk
*/

	ALIGN_BLOCK

CallPal_BugChkCont:
	mfpr	p6, excAddr		// Get PC following trap instruction

	STACK_FRAME(p4,p6,p7)

	mfpr	p5, ptEntIF		// Get pointer to kernel fault handler

	bis	zero, IF_K_BUGCHK, a0	// Load code indicating bugcheck trap
	mtpr	p5, excAddr		// Load the kernel entry point

	mfpr	gp, ptKgp		// Load the kernel global pointer
	hw_rei


/*
** Continuation of CallPal_CallSys
*/

	ALIGN_BLOCK

CallPal_CallSysCont:

	mtpr	zero, ips		// Set the Ibox mode to kernel.

	mtpr	zero, dtbCm		// Set the Mbox mode to kernel.

	STACK_FRAME(t10,t9,p7)

	mfpr	s3, ptEntSys		// Get pointer to kernel handler

	mfpr	gp, ptKgp		// Load the kernel global pointer
	mtpr	s3, excAddr		// Load the kernel entry point
	STALL
	hw_rei

	ALIGN_BLOCK

CalPal_CallSys_FromKern:

	mfpr	p6, excAddr		// Get PC of CALL_PAL callsys
	subq	p6, 4, p6		// Back up PC

	br	zero, MchkOSBugCheck


/*
** Continuation of CallPal_GenTrap
*/

	ALIGN_BLOCK

CallPal_GenTrapCont:
	mfpr	p6, excAddr		// Get PC of instruction following trap

	STACK_FRAME(p4,p6,p7)

	mfpr	p5, ptEntIF		// Get pointer to kernel fault handler

	bis	zero, IF_K_GENTRAP, a0	// Load code indicating gentrap
	mtpr	p5, excAddr		// Load the kernel entry point

	mfpr	gp, ptKgp		// Load the kernel global pointer
	hw_rei

#if defined(KDEBUG)
/*
** Continuation of CallPal_DbgStop
*/

	ALIGN_BLOCK

CallPal_DbgStopCont:
	mfpr	p6, excAddr		// Get PC of instruction following trap

	STACK_FRAME(p4,p6,p7)

	mfpr	p5, ptEntDbg		// Get pointer to kernel fault handler

	bis	zero, zero, a0		// Load 0, indicating dbgstop
	mtpr	p5, excAddr		// Load the kernel entry point

	mfpr	gp, ptKgp		// Load the kernel global pointer
	hw_rei
#endif /* KDEBUG */


/*
**
** FUNCTIONAL DESCRIPTION:
**
**	The reserved CALL_PAL instruction fault handler builds a
**	stack frame on the kernel stack, loads the GP with the KGP,
**	loads the value 4 into a0 (OPCDEC), and dispatches to the
**	kernel illegal instruction fault handler pointed to by the 
**	entIF operating system entry point.
** 
** CALLING SEQUENCE:
** 
** 
** 
** INPUT PARAMETERS:
** 
**	None
** 
** OUTPUT PARAMETERS:
**
**	r16 (a0) = Instruction fault code (OPCDEC)
**	r17 (a1) = UNPREDICTABLE
**	r18 (a2) = UNPREDICTABLE
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched
**	from user to kernel mode.
** 
*/ 
        ALIGN_BLOCK

CallPal_OpcDec:
	bis	zero, ps, p4		// Save a copy of the PS shadow
	mtpr	zero, ips		// Set Ibox mode to kernel

	mtpr	zero, dtbCm		// Set Mbox mode to kernel
	mfpr	p6, excAddr		// Get address of faulting instruction

	STACK_FRAME(p4,p6,p7)

	mfpr	p5, ptEntIF		// Get pointer to kernel fault handler

	bis	zero, IF_K_OPCDEC, a0	// Pass OPCDEC code in a0
	mtpr	p5, excAddr		// Load the kernel handler entry point

	mfpr	gp, ptKgp		// Load the kernel global pointer
	hw_rei				// Transfer to kernel fault handler ...

/*======================================================================*/
/*                  HARDWARE VECTOR CONTINUATION AREA                   */
/*======================================================================*/
	
	.text 3

	. = 0x5000

ResetContinue:
	mtpr	v0, pt0			// Save v0.

	mtpr	zero, itbIa		// Flush the ITB.
	mtpr	zero, dtbIa		// Flush the DTB.

	lda	t0, -8(t0)		// Point to start of PALcode.
	mtpr	t0, palBase		// Initialize the PAL base address.
	STALL
	STALL

#if defined(DEBUG)
	LEDWRITE(0xEF, t0, v0)
#endif

	mfpr	t0, palBase		// Reload the PAL base address.
	ldl_p	v0, 0x14(t0)		// Get FIXED offset to end of PALcode
	addq	v0, t0, v0		// Form base address of impure area.

	lda	v0, 0x1FFF(v0)		// Page align the impure area
	srl	v0, 13, v0		// Shift out lower bits
	sll	v0, 13, v0		// Back into position.

	mtpr	v0, ptImpure		// Save pointer to impure area.

	lda	v0, HLT_K_RESET(zero)	// Halt code of RESET
	mtpr	v0, pt5			// Save entry type
	STALL
	STALL
	mfpr	t0, ptImpure		// Get base of impure area.
	SAVE_GPRS			// Saves GPRs to impure area.

	mtpr	zero, astrr		// Ignore AST interrupt requests.
	mtpr	zero, aster		// Disable AST interrupts.

	mtpr	zero, sirr		// Ignore software interrupt requests.
	ldah	t0, ICSR_K_INIT(zero)	// Get initial ICSR value.

#if !defined(DISABLE_CRD)

	bis	zero, 1, v0		// Get a '1'
	sll	v0, ICSR_V_CRDE, v0	// Shift it into correct bit position
	or	v0, t0, t0		// Set the CRD enable bit

#endif /* DISABLE_CRD */
	mtpr	t0, icsr		// Initialize ICSR -- enable shadow!
	br	zero, set_icsr

	ALIGN_BLOCK
/*
** Since we modify SPE, we will follow directions and flush the icache.
*/
set_icsr:
	NOP
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


	lda	t0, (2<<MCSR_V_SP)(zero) // Get a '10' (binary) in MCSR<SP>
	mtpr	t0, mcsr		// Set the super page mode enable bit
	lda	t0, DC_M_ENA(zero)	// Get a '1' in DC_MODE<ENA>
	mtpr	t0, dcMode		// Set the Dcache enable bit
	NOP

	STALL				// No Mbox instructions in 1,2,3,4
	STALL
	STALL
	STALL

	mtpr	zero, dcFlush		// Flush the Dcache

	lda	ps, IPL_K_HIGH(zero)	// Set PS shadow - Kernel mode, IPL=7
	lda	t0, 0x1F(zero)		// Set internal IPL=1F
	mtpr	t0, ipl
	mtpr	zero, ips		// Set Ibox mode to kernel
	mtpr	zero, dtbCm		// Set Mbox mode to kernel

	mtpr	zero, excSum		// Clear any pending exceptions
	mfpr	zero, va		// Unlock the Mbox

	lda	t0, ICPERR_M_ALL(zero)
	mtpr	t0, icPerr		// Clear Icache parity errors & timeout
	lda	t0, DCPERR_M_ALL(zero)	
	mtpr	t0, dcPerr		// Clear Dcache parity errors

	rc	v0			// Clear intr_flag
	mtpr	zero, ptTrap
/*
** Now check for SWAPPAL.
*/
	mfpr	v0, ptMisc		// Get swap flag.
	srl	v0, PT16_V_SWAP, t0	// Shift into position.
	zap	v0, 0x40, v0		// Clear swap flag.
	mtpr	v0, ptMisc		// Update the chip.
	lda	p4, WHAMI_K_SWAP(zero)	// Load 'S'.
	cmpeq	t0, p4, t0		// Compare.

	blbs	t0, 1f			// Check if we got here via SWPPAL.
/*
**  Not SWAPPAL
*/
	mtpr	zero, dtbAsn

	mtpr	zero, itbAsn
	lda	t0, 0x67(zero)

	sll	t0, HWINT_V_PC0C, t0
	mtpr	t0, hwIntClr		// Clear any hardware int requests

	lda	v0, (1<<MCES_V_DPC)(zero) // Get a '1' in MCES<DPC>
	mtpr	v0, ptMisc		// Store <WHAMI>!<MCES>, swap flag 0
	STALL
	STALL

	or	zero, 1, t0		// Get a '1'
	sll	t0, 32, t0		// Shift into CC<OFFSET> bit position
	mtpr	zero, cc		// Clear cycle counter

	mtpr	t0, ccCtl		// Clear and enable cycle counter
	mtpr	zero, ptSysVal		// Clear system specific value

	mtpr	zero, mafMode		// Clear miss address file mode
	mtpr	zero, ptUsp		// Clear user stack pointer
	bis	zero, 1, t0		// Get bogus kgp value
	mtpr	t0, ptKgp		// Initialize the kernel global pointer

	lda	t0, 0x2(zero)		// Get a '10' (binary)
	sll	t0, 32, t0		// Generate upper bits
	mtpr	t0, mVptBr		// Set virtual base of L3 page table
	mtpr	t0, iVptBr
	mtpr	zero, ptPtbr

	ldah	v0, SEXT(0xFFF0)(zero)	// v0 <- 0.FFF0.0000
	zap	v0, 0xE0, v0		// Get base address of CBOX IPRs

	ldq_p	zero, scStat(v0)	// Clear Scache status and error addr
	ldq_p	zero, eiStat(v0)	// Clear System/Bcache error status,
	ldq_p	zero, eiStat(v0)	// error address, Bcache tag address,
                                        // and fill syndrome
/*
** Check to see if the transfer from the POST (Power-on-Self Test) code
** is following a standard protocol and that the other input parameter
** values may be trusted. Register a3 (r19) will contain a signature if so.
**
** Register values:
**
** t0 (r1)	bcCtl value, saved into pt4			DC21164
** t0 (r1)	bcCfg2 value, saved into pt4			DC21164PC
** t1 (r2)	bcCfg value
** t2 (r3)	bcCfgOff value (values for bcache off)
**
** s6 (r15)	encoded srom.s RCS revision
** a0 (r16)	processor identification (a la SRM)
** a1 (r17)	size of contiguous, good memory in bytes
** a2 (r18)	cycle count in picoseconds
** a3 (r19)	signature (0xDECB) in <31:16> and system revision ID in <15:0>
** a4 (r20)	active processor mask
** a5 (r21)	system context value
*/
	srl	a3, 16, p4		// Shift signature into lower word.
	zap	p4, 0xFC, p4		// Ignore the upper longword.
	LDLI(p5,0xDECB)			// Load the expected valid signature.
	cmpeq	p4, p5, p4		// Is it a valid signature?
	beq	p4, 2f			// If invalid, don't trust input params
	mfpr	p4, ptImpure		// Get the base of the impure area.
	mfpr	t0, pt4			// Recover bcControl.
	lda	p5, CNS_Q_IPR(p4)	// Point to base of IPR area.

	SAVE_SHADOW(t0,CNS_Q_BC_CTL,p5)		// Save shadow of bcCtl.
	SAVE_SHADOW(t1,CNS_Q_BC_CFG,p5)		// Save shadow of bcCfg.
	SAVE_SHADOW(t2,CNS_Q_BC_CFG_OFF,p5)	// Save shadow of bcCfg.

	SAVE_SHADOW(s6,CNS_Q_SROM_REV,p5)	// Save srom revision.
	SAVE_SHADOW(a0,CNS_Q_PROC_ID,p5)	// Save processor id.
	SAVE_SHADOW(a1,CNS_Q_MEM_SIZE,p5)	// Save memory size.
	SAVE_SHADOW(a2,CNS_Q_CYCLE_CNT,p5)	// Save cycle count.
	SAVE_SHADOW(a3,CNS_Q_SIGNATURE,p5)	// Save signature and sys rev.
	SAVE_SHADOW(a4,CNS_Q_PROC_MASK,p5)	// Save processor mask.
	SAVE_SHADOW(a5,CNS_Q_SYSCTX,p5)		// Save system context.

	bis	r31, r31, a3			// Clear the signature register

/*
** We need a scratch PCB (we aren't provided one unless there's a SWPPAL.)
*/
2:
	mfpr	p4, ptImpure		// Get base of impure area.

	lda	p5, CNS_Q_SCRATCH(p4)	// Point to scratch location.
	mtpr	p5, ptPcbb		// Initialize PCBB to scratch location.

pvc$osf27$5010:				// PVC subroutine branch origin
	bsr	p5, Sys_Reset		// Do any platform specific reset

	mfpr	p6, ptImpure
	stq_cp	zero, CNS_Q_FLAG(p6)	// Clear the lock_flag (0x100)

	mfpr	t0, ptImpure		// Impure area.
	RESTORE_GPRS			// Restores GPRs from impure area.
					// t0 will be restored to its init value.
	mfpr	v0, pt5			// Get the reason for entry
	br	zero, Sys_EnterConsole

/*
**
**   Entry via SWPPAL, the following parameters are passed in:
**
**	r0  (v0) = ptMisc
**
**	r17 (a1) = New PC
**	r18 (a2) = New PCBB
**	r19 (a3) = New VPTB
*/
	ALIGN_BRANCH_TARGET

/*
** Since the impure area is part of this image and not a common area somewhere,
** we must initialize the impure area pointer.
*/
1:
	mfpr	t0, palBase		// Get the PalBase back.
	ldl_p	p4, 0x14(t0)		// Get FIXED offset to impure area ptr.
	addq	p4, t0, p4		// Form base address of impure area.
	
	lda	p4, 0x1FFF(p4)		// Page align the impure area
	srl	p4, 13, p4		// Shift out lower bits
	sll	p4, 13, p4		// Back into position.

	mtpr	p4, ptImpure		// Save pointer to impure area.

	rpcc	t0			// Get the cycle counter

	ldq_p	t8, PCB_Q_FEN(a2)	// Get new FEN/PME
	ldl_p	t9, PCB_L_PCC(a2)	// Get new cycle counter
	ldl_p	t10, PCB_L_ASN(a2)	// Get new ASN

/*
** If in the PCB, PTBR<63> = 1, put us in physical mode by setting the
** low bit of ptPtbr.
*/
	ldq_p	p7, PCB_Q_PTBR(a2)	// Get PFN of new page table base
	srl	p7, 63, p5
	sll	p7, VA_S_OFF, p7	// Convert PFN to physical address
	bis	p7, p5, p7
	mtpr	p7, ptPtbr		// Load new page table base register
	mtpr	a2, ptPcbb		// Set new process control block

	bic	a1, 3, a1		// Clean PC
	mtpr	a1, excAddr		// Load new PC
	mtpr	a3, iVptBr
	mtpr	a3, mVptBr

	ldq_p	sp, PCB_Q_USP(a2)	// Get new user stack pointer
	mtpr	sp, ptUsp		// Update the chip

	sll	t10, 57, p0
	mtpr	p0, dtbAsn
	sll	t10, 4, t10
	mtpr	t10, itbAsn

	mfpr	p7, icsr		// Get current ICSR
	lda	t10, 1(zero)		// Get a '1'
	sll	t10, ICSR_V_FPE, t10	// Shift it into ICSR<FPE> bit position
	bic	p7, t10, p7		// Clean out old FPE
	and	t8, 1, t8		// Isolate new FEN bit
	sll	t8, ICSR_V_FPE, t8	// Shift FEN into ICSR<FPE> position
	bis	t8, p7, p7		// Merge in new FPE bit
	mtpr	p7, icsr		// Update the chip

	subl	t9, t0, t0		// Generate new CC<OFFSET> bits
	insll	t0, 4, t0		// << 32
	mtpr	t0, cc			// Set new CC<OFFSET>

pvc$osf48$5060:				// PVC subroutine branch origin
	bsr	v0, Sys_ResetViaSwppal	// Do platform specific

	bis	zero, zero, v0		// Indicate success status
	ldq_p	sp, PCB_Q_KSP(a2)	// Load the new kernel stack pointer

	STALL
	hw_rei_stall


/*======================================================================*/
/*                   MEMORY MANAGEMENT FAULT HANDLERS                   */
/*======================================================================*/

/*
**
** FUNCTIONAL DESCRIPTION:
**
**	The invalid PTE handler builds a stack frame on the 
**	kernel stack, loads the GP with the KGP, loads the 
**	VA of the faulting target reference into a0, sorts 
**	out the reason for the fault (TNV or ACV), loads the 
**	appropriate memory management fault code into a1, loads 
**	the value -1, 0, or 1 into a2 (I-fetch/D-read/D-write) 
**	indicating the faulting target reference type, and 
**	dispatches to the kernel memory managment fault handler 
**	pointed to by the entMM operating system entry point. 
** 
** CALLING SEQUENCE:
** 
**	Called by the I or D-stream TB miss handler when an invalid
**	level 3 PTE is detected.
** 
** INPUT PARAMETERS:
** 
**	r8  (p0) = PTE
**	r9  (p1) = Contents of mmStat, or zero if I-stream
**	r10 (p2) = VA of faulting target reference
**	pt6      = PC of faulting instruction if D-stream
** 
** OUTPUT PARAMETERS:
**
**	r16 (a0) = VA of faulting target reference 
**	r17 (a1) = Memory management fault code (TNV/ACV)
**	r18 (a2) = Reference type (I-fetch/D-read/D-write)
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched
**	from user to kernel mode.
** 
*/ 
	ALIGN_BLOCK

PteInvalid:
	bis	zero, ps, p4		// Save a copy of the PS shadow
	mtpr	zero, ips		// Set Ibox mode to kernel

	mfpr	p6, pt6			// Get PC of faulting instruction

	blbs	p6, KspNotValid		// If in PAL, must be ksp related

	mtpr	zero, dtbCm		// Set Mbox mode to kernel
	beq	p1, 1f			// Check for I-stream vs. D-stream

	mfpr	p1, mmStat		// MM_STAT should not have changed.
	srl	p1, MMSTAT_V_OPC, p7	// Get faulting opcode into lsb

	cmpeq	p7, 0x18, p7		// Is it FETCH[_M]?

	NOP				// E0
	blbs	p7, 5f			// If so, ignore

1:	bis	zero, p2, p5		// Save VA in case we TB miss on stack
	NOP				// E1

	STACK_FRAME(p4,p6,p7)

	bge	p7, 2f			// Check for kernel vs user mode
	srl	p0, PTE_V_URE-PTE_V_KRE, p0 // Move user protection bits

2:	lda	a1, MM_K_ACV(zero)	// Assume ACV
	srl	p0, PTE_V_KRE, p0	// Get read enable bit into lsb

	mfpr	p7, ptEntMM		// Get pointer to kernel fault handler
	NOP				// E1

	blbc	p1, 3f			// Check for read vs. write reference
	srl	p0, PTE_V_KWE-PTE_V_KRE, p0 // Get write enable into lsb

3:	cmovlbs p0, MM_K_TNV, a1	// Resolve ACV or TNV
	bis	zero, p5, a0		// Pass VA of faulting reference in a0

	and	p1, 1, a2		// Isolate read/write bit.
	NOP				// E1

	bne	p1, 4f			// Check for I-stream vs. D-stream
	subq	zero, 1, a2		// Indicate I-stream reference

4:	mtpr	p7, excAddr		// Load kernel handler entry point

	mfpr	gp, ptKgp		// Load kernel global pointer

	hw_rei
/*
**
** An invalid D-stream PTE was detected during the execution of 
** a FETCH or FETCH_M instruction. Ignore the fault.
**
** Current state:
**
**	r12 (p4) = PC of faulting FETCH[_M] instruction
**
*/
	ALIGN_BRANCH_TARGET

5:	addq	p4, 4, p4		// Increment PC past offending instr.
	mtpr	p4, excAddr		// Load the new PC
	STALL
	hw_rei

/*
**
** FUNCTIONAL DESCRIPTION:
**
**	The I-stream Fault-On-Execute (FOE) handler builds a 
**	stack frame on the kernel stack, loads the GP with 
**	the KGP, loads the VA of the faulting target I-stream 
**	reference into a0, sorts out the reason for the fault
**	(FOE or ACV), loads the appropriate memory management 
**	fault code into a1, loads the value -1 into a2 (I-fetch) 
**	indicating the faulting target reference type, and 
**	dispatches to the kernel memory managment fault handler 
**	pointed to by the entMM operating system entry point. 
** 
** CALLING SEQUENCE:
** 
**	Called by the I-stream TB miss handler when a  
**	Fault-On-Execute is detected.
** 
** INPUT PARAMETERS:
** 
**	r8  (p0) = PTE
**	r10 (p2) = VA of faulting target reference
** 
** OUTPUT PARAMETERS:
**
**	r16 (a0) = VA of faulting target reference 
**	r17 (a1) = Memory management fault code (FOE/ACV)
**	r18 (a2) = Reference type (I-fetch)
** 
** SIDE EFFECTS:
**
**	The current mode of the processor may be switched
**	from user to kernel mode.
** 
*/ 
	ALIGN_BLOCK

ItbFoe:
	bis	zero, ps, p4		// Save PS shadow for stack write
	mtpr	zero, ips		// Set Ibox mode to kernel

	mtpr	zero, dtbCm		// Set Mbox mode to kernel
	bis	zero, p2, p5		// Save PC in case we TB miss on stack
	
	STACK_FRAME(p4,p5,p7)
	
	bge	p7, 1f			// Check for kernel vs user mode
	srl	p0, PTE_V_URE-PTE_V_KRE, p0 // Move user protection bits

1:	lda	a1, MM_K_ACV(zero)	// Assume ACV
	srl	p0, PTE_V_KRE, p0	// Get read enable bit into lsb

	mfpr	p7, ptEntMM		// Get pointer to kernel fault handler
	bis	zero, p5, a0		// Pass VA/PC of faulting instr in a0

	cmovlbs	p0, MM_K_FOE, a1	// Pass ACV/FOE in a1
	subq	zero, 1, a2		// Pass I-fetch reference type in a2

	mtpr	p7, excAddr		// Load kernel handler entry point

	mfpr	gp, ptKgp		// Load kernel global pointer

	hw_rei

/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Return a "fake" PTE to the single TB miss trap
**	handler.  Make it appear as if a PTE had been 
**	successfully fetched, but is not valid.  This 
**	will cause the single TB miss trap handler to 
**	be coerced into doing "the right thing" by sorting 
**	out TNV or ACV based on the protection bits returned 
**	in the "fake" PTE. 
** 
** CALLING SEQUENCE:
** 
**	Called by the double DTB miss trap handler when an
**	invalid level 2 or level 3 PTE is detected.
** 
** INPUT PARAMETERS:
**
**	r22 (t8) = Scratch
**	r23 (t9) = PTE
**
**	pt4 = Saved t8
**	pt5 = Saved t9
**	pt6 = PC of faulting instruction
** 
** OUTPUT PARAMETERS:
** 
**	NOTE:  The following register usage is CRITICAL, it must
**	be maintained in both the ITB and DTB single TB miss trap
**	handlers.
**
**	r8  (p0) = Returned "fake" PTE
** 
** SIDE EFFECTS:
**
** 
*/ 
	ALIGN_BLOCK

FakePte:
	srl	t9, PTE_V_KRE, t9	// Get KRE bit into lsb
	mfpr	t8, excAddr		// Get PC of PTE fetch in DTB miss

	lda	t8, 4(t8)		// Increment PC past PTE fetch
	lda	p0, PTE_M_PROT(zero)	// Form fake PTE with xRE and xWE set

	cmovlbc	t9, zero, p0		// Set to all zero if ACV
	mtpr	t8, excAddr		// Load return address

	mfpr	t8, pt4			// Restore scratch registers
	mfpr	t9, pt5

	hw_rei				// Back to single TB miss handler

/*
** Build a stack frame on the kernel stack and dispatch
** to the kernel memory management fault handler.
**
** Current state:
**
**	VA locked
**
**	r8  (p0) = PC of faulting instruction
**	r9  (p1) = Opcode of faulting instruction
**	r10 (p2) = Scratch
**	r12 (p4) = PS copy
**	r13 (p5) = mmStat<3:0>
**
*/
	ALIGN_BLOCK

DfaultContinue:
	blbs	p0, 1f			// Check for faults in PALmode
	bis	zero, p0, p6		// Move PC to correct place

	cmpeq	p1, 0x18, p2		// Is this a FETCH[_M] instruction?
	bne	p2, 2f			// If so, ignore the fault

	mfpr	p1, va			// Unlock the Mbox

	STACK_FRAME(p4,p6,p7)

	and	p5, MMSTAT_M_WR, a2	// Pass reference type in a2

	bis	zero, p1, a0		// Pass VA of faulting reference in a0
	srl	p5, MMSTAT_V_ACV, a1	// Shift fault bits into lsb

	mfpr	p7, ptEntMM		// Get pointer to kernel fault handler
	cmovlbs	a1, MM_K_ACV, a1	// ACV overrides FOR and FOW

	mtpr	p7, excAddr		// Load the kernel handler entry point

	mfpr	gp, ptKgp		// Load the kernel global pointer

	hw_rei

/*
**
** A D-stream fault occurred in PALmode during one of the two 
** following cases:
**
**	- A virtual read of a PTE in either the ITB or DTB miss 
**	  trap handler
**
**	- A read/write from/to the kernel stack in either the
**	  stack frame builder or the rti/retsys CALL_PAL function.
** 
** Current state:
**
**	VA locked
**
**	r14 (p6) = PC of faulting instruction
**
*/
	ALIGN_BRANCH_TARGET

1:	bic	p6, 3, t7		// Clean faulting PC<1:0> bits
	mfpr	p2, palBase		// Get the PAL base address

	mfpr	zero, va		// Unlock the Mbox

	subq	p2, t7, t7		// Get offset to faulting instruction
/*
** Check if the fault occurred during a virtual read of the level
** 3 PTE in either the ITB or DTB miss trap handlers.
*/
	lda	p2, (Itb_LdVpte-Pal_Base)(t7) // Check ITB handler
	NOP

	beq	p2, MchkBugCheck
	lda	p2, (Dtb_LdVpte-Pal_Base)(t7) // Check DTB handler

	beq	p2, MchkBugCheck
/*
** Otherwise, the fault must have occurred while reading/writing 
** from/to the kernel stack in either the stack frame builder or
** the rti/retsys CALL_PAL functions.
*/

KspNotValid:
	bic	ps, PS_M_CM, ps		// Set PS to kernel mode

	mtpr	v0, pt0			// Save r0
	mtpr	ps, dtbCm		// Make sure Mbox mode is in sync

	mtpr	ps, ips			// Make sure Ibox mode is in sync

	mtpr	p6, excAddr		// Load PC of faulting instruction

pvc$osf29$5000:				// PVC subroutine branch origin
	bsr	v0, UpdatePCB		// Update the PCB

	lda	v0, HLT_K_KSP_INVAL(zero) // Set halt code to KSP invalid

	br	zero, Sys_EnterConsole
/*
**
** A D-stream fault occurred during the execution of a FETCH or
** FETCH_M instruction. Ignore the fault.
**
** Current state:
**
**	VA locked
**
**	r14 (p6) = PC of faulting FETCH instruction
**
*/
	ALIGN_BRANCH_TARGET

2:	mtpr	ps, dtbCm		// Make sure Mbox mode is in sync
	mtpr	ps, ips			// Make sure Ibox mode is in sync

	mfpr	zero, va		// Unlock the Mbox
	addq	p6, 4, p6		// Increment PC past offending instr

	mtpr	p6, excAddr		// Load the new PC
	STALL

	hw_rei

/*======================================================================*/
/*                      HARDWARE INTERRUPT HANDLERS                     */
/*======================================================================*/

	ALIGN_BLOCK

InterruptContinue:
	NOP				// May delete when scheduling!!
	mfpr	gp, ptKgp		// Update the global pointer
/*
** Check for a system machine check interrupt or internally 
** correctable error interrupt ...
*/
	cmpeq	p5, 31, p4		// Is it a level 31 interrupt?
	bne	p4, ErrorHandler
/*
** Check for a power fail interrupt ...
*/
	cmpeq	p5, 30, p4		// Is it a level 30 interrupt?
	bne	p4, PowerFailHandler
/*
** Check for a performance counter interrupt ...
*/
	cmpeq	p5, 29, p4		// Is it a level 29 interrupt?
	bne	p4, PerfHandler
/*
** Check for an external I/O interrupt ...
*/
	br	zero, Sys_Interrupt

/*
** Passive Release
*/
	ALIGN_BRANCH_TARGET

PassiveRelease:
	mtpr	ps, dtbCm		// Restore Mbox current mode.
	NOP
	STALL				// Pad out write to dtbCm.
	hw_rei				// Back to user ...

/*
** Halt handler for external HALT interrupt
*/
	ALIGN_BRANCH_TARGET

HaltHandler:
	mtpr	ps, dtbCm		// Restore Mbox current mode.
	mtpr	v0, pt0			// Save v0.

pvc$osf28$5000:
	bsr	v0, UpdatePCB		// Update the current PCB.
	lda	v0, HLT_K_HW_HALT(zero)	// Set code to HW halt.
	br	zero, Sys_EnterConsole	// Go to console I/O mode ...

	ALIGN_BRANCH_TARGET

ErrorHandler:
	mfpr	p4, ptIntMask		// Get the IPL translation mask.
	extbl	p4, IPL_K_MCHK, p5	// Translate to internal IPL.	

	bis	zero, IPL_K_MCHK, ps	// Set OSF/1 IPL level.
	mtpr	p5, ipl			// Load the target IPL.

	srl	p7, ISR_V_MCK, p4	// Is it a machine check interrupt?
	blbs	p4, Sys_IntMchkHandler	// If so, go handle ...

	srl	p7, ISR_V_CRD, p4	// Is it a CRD interrupt?
	blbs	p4, IntCrdHandler	// If so, go handle ...

	br	zero, CallPal_Rti	// Nothing there, back out with rti.


/*======================================================================*/
/*            INTERRUPT HANDLER FOR CORRECTABLE ERROR MACHINE CHECK     */
/*======================================================================*/

	ALIGN_BRANCH_TARGET

IntCrdHandler:
	mb				// Clear out CBOX prior to IPR reads.

	mtpr	v0, pt0			// Save v0.
	mtpr	t0, pt1			// Save t0.

	ldah	p6, SEXT(0xFFF0)(zero)
	zap	p6, 0xE0, p6		// Form CBOX IPR base address.

	ldq_p	v0, eiAddr(p6)		// Get Bcache/System error address.
	ldq_p	p2, fillSyn(p6)		// Get the fill syndrome.
	bis	v0, p2, zero		// Make sure IPR reads complete.
/*
** Scrub memory.
*/
pvc$osf100$5100:			// PVC subroutine branch origin
	bsr	p5, CrdScrubMemory
					// Clean up after routine.
	ldq_p	p1, eiStat(p6)		// Unlock eiAddr, bcTagAddr, fillSyn.
	mfpr	p5, ptMces		// Get mchk error summary bits.
	srl	p5, MCES_V_DPC, p6	// Shift MCES<DPC> into low bit.
	blbs	p6, CrdAck		// If disabled, skip MCHK frame build.

	bis	p5, MCES_M_PCE, p6	// Flag as Processor Correctable Error.
	lda	t0, SCB_Q_PROCERR(zero)	// Get SCB vector for processor error.
	sll	t0, PT16_V_SCB, t0	// Shift it into correct position.
	
	zap	p6, 0x3C, p6		// Clear space for MCHK and SCB words.
	or	p6, t0, p6		// Merge in SCB vector.

	lda	t0, MCHK_K_ECC_C(zero)	// Load code for correctable ECC error.
	sll	t0, PT16_V_MCHK, t0	// Shift it into correct position.
	or	p6, t0, p6		// Merge in MCHK code.
	mtpr	p6, ptMisc		// Update the chip.
/*
** If PCE is already set, just rewrite the flag longword as retry
** and 2nd error. Otherwise write the logout frame for a
** correctable error.
**
** Current State:
**
**	v0	Bcache/System Error Address
**	t0	Scratch
**	p1	Bcache/System Error Status
**	p2	Fill Syndrome
**	p4	Scratch
**	p5	Machine Check Error Summary 
**	p6	Scratch
**	p7	Interrupt Summary
**
**	pt0	Saved v0
**	pt1	Saved t0
**
*/
	mfpr	p6, ptImpure		// Get address of impure scratch area

	LDLI(p6,LAS_Q_BASE)		// Get base address of short frame.

	srl	p5, MCES_V_PCE, t0	// Get PCE.
	blbc	t0, 1f			// If PCE is not set, build the frame.
	lda	t0, 3(zero)		// Set retry and 2nd error flags.
	sll	t0, 30, t0		// Move into 31:30.
	stl_p	t0, LAS_L_FLAG(p6)	// Store flag longword.
	br	zero, CrdAck		// Don't log any further.
/*
** Write the first two quadwords of the frame
*/
1:	lda	t0, 0x1(zero)		  // Get a '1'
	sll	t0, 63, t0		  // Shift retry flag into high bit.
	lda	t0, LAS_K_SIZE(t0)	  // Combine retry flag and frame size
	stq_p	t0, LAS_L_SIZE(p6)	  // Store flag/frame size
	lda	t0, LAS_Q_SYS_BASE(zero)  // Get offset to system-specific info
	sll	t0, 32, t0		  // Shift offset into upper longword
	lda	t0, LAS_Q_CPU_BASE(t0)	  // Combine with offset to CPU info
	stq_p	t0, LAS_Q_OFFSET_BASE(p6) // Store offsets to system/cpu info
/*
** Write the MCHK code and error IPRs previously fetched
** to the frame.
*/
	mfpr	t0, ptMisc		// Get MCHK error summary bits
	extwl	t0, 4, t0		// Extract the MCHK code

	stq_p	t0, LAS_Q_MCHK_CODE(p6)	// Store the MCHK code

	stq_p	v0, LAS_Q_EI_ADDR(p6)	// Store Bcache/System error address
	stq_p	p2, LAS_Q_FILL_SYNDROME(p6) // Store Fill Syndrome
	stq_p	p1, LAS_Q_EI_STAT(p6)	// Store Bcache/System error status
	stq_p	p7, LAS_Q_ISR(p6)	// Store the interrupt summary

	br	zero, CrdAck		// Continue CRD error handling...

	ALIGN_BRANCH_TARGET

CrdAck:
	mfpr	v0, pt0			// Restore v0
	mfpr	t0, pt1			// Restore t0

	lda	p2, 0x1(zero)		// Get a '1'
	sll	p2, HWINT_V_CRDC, p2	// Shift it into HWINT<CRDC> position
	mtpr	p2, hwIntClr		// Acknowledge the CRD interrupt

	srl	p5, MCES_V_DPC, p0	// Get logging enabled bit.
	blbs	p0, CrdDismissInterrupt	// Dismiss if disabled.

	lda	a0, INT_K_MCHK(zero)	// Flag as MCHK/CRD interrupt 
	lda	a1, SCB_Q_PROCERR(zero) // Pass SCB vector in a1
	
	subq	zero, 1, a2		// Get a '-1'
	mfpr	p7, ptEntInt		// Get pointer to kernel handler

	srl	a2, (VA_S_SIZE-1), a2	// Shift off low bits for kseg address
	mtpr	p7, excAddr		// Load kernel interrupt handler entry

	sll	a2, (VA_S_SIZE-1), a2	// Shift kseg bits back into position
	bis	p6, a2, a2		// Pass ptr to logout area as kseg addr

	NOP
	hw_rei				// Transfer to kernel interrupt handler

	ALIGN_BRANCH_TARGET

CrdDismissInterrupt:
	NOP
	br	zero, CallPal_Rti	// Back out with rti.

/*
** Scrub Memory.
**	v0 = address of cache block
** Find the error in memory, and attempt to scrub that cache block.
** This routine just scrubs the failing octaword. Only need to
** "touch" one quadword per octaword to accomplish the scrub.
** Note: This routine must have an align block macro!
*/
	ALIGN_BLOCK

CrdScrubMemory:
	srl	v0, 39, p1		// Get high bit of bad pa.
	blbs	p1, 1f			// Don't fixup IO space.
	
	NOP				// Load must be on octaword.
	NOP

	ldq_lp	p1, 0(v0)		// Attempt to read the bad memory.
	NOP				// Keep request in E0 pipe.

	stq_cp	p1, 0(v0)		// Store it back if still there.
/*
** If store fails, it was scrubbed by someone else.
** At this point, eiStat or bcStat could be locked due to a new correctable
** error on the load, so read eiStat to unlock AFTER this routine.
*/
pvc$osf101$5100.1:	
1:	ret	zero, (p5)		// Back we go.


/*======================================================================*/
/*            INTERRUPT HANDLER FOR POWERFAIL                           */
/*======================================================================*/
	ALIGN_BRANCH_TARGET

PowerFailHandler:
	mfpr	p4, ptIntMask		// Get the IPL translation mask.
	extbl	p4, IPL_K_PFAIL, p5	// Translate to internal IPL.	

	bis	zero, IPL_K_PFAIL, ps	// Set OSF/1 IPL level.
	mtpr	p5, ipl			// Load the target IPL.

	lda	a1, SCB_Q_PWRFAIL(zero)	// Load SCB vector
	mfpr	p7, ptEntInt		// Get pointer to kernel handler

	lda	a0, INT_K_DEV(zero)	// Signal device interrupt (why???)
	mtpr	p7, excAddr		// Load kernel entry address

	NOP				// Pad excAddr write
	NOP

	hw_rei

/*======================================================================*/
/*            INTERRUPT HANDLER FOR PERFORMANCE MONITOR                 */
/*======================================================================*/
	ALIGN_BRANCH_TARGET

PerfHandler:
	mfpr	p4, ptIntMask		// Get the IPL translation mask.
	extbl	p4, IPL_K_PERF, p5	// Translate to internal IPL.	

	bis	zero, IPL_K_PERF, ps	// Set OSF/1 IPL level.
	mtpr	p5, ipl			// Load the target IPL.

	lda	a1, SCB_Q_PERFMON(zero)	// Load SCB vector
	mfpr	p7, ptEntInt		// Get pointer to kernel handler

	lda	a0, INT_K_PERF(zero)	// Signal performance counter interrupt
	mtpr	p7, excAddr		// Load kernel entry address

	NOP				// Pad excAddr write
	NOP

	hw_rei

/*======================================================================*/
/*            PALCODE-DETECTED PROCESSOR MACHINE CHECK HANDLER          */
/*======================================================================*/
/*
**
** FUNCTIONAL DESCRIPTION:
**
**	The PALcode-detected machine check handler loads a code
**	indicating the type of machine check error, loads 
**	the System Control Block (SCB) vector for the 
**	processor machine check service routine, sets the 
**	Machine-Check-In-Progress (MIP) flag in the Machine
**	Check Error Summary register (MCES), and merges
**	with the common machine check flow.
**
**	If a second processor machine check error condition 
**	is detected while the MIP flag is set, the processor 
**	is forced into console I/O mode indicating "double 
**	error abort encountered" as the reason for the halt. 
** 
** CALLING SEQUENCE:
** 
**	Called when an internal processor error is detected
**	that cannot be successfully corrected by hardware or
**	PALcode.
** 
** INPUT PARAMETERS:
**
**	r14 (p6) = Exception address 
** 
** OUTPUT PARAMETERS:
**
**	pt0		= saved v0
**	pt1		= saved t0
**	pt4		= saved t3
**	pt5		= saved t4
**	pt6		= saved t5
**	ptMisc<47:32>	= MCHK code
**	ptMisc<31:16>	= SCB vector
**	ptMces<MIP>	= Set
**	p6 		= Cbox IPR base address
** 
** SIDE EFFECTS:
**
**	r0 (v0), r1 (t0), and r4..r6 (t3..t5) are saved in
**	PAL temporaries and are available for use as scratch
**	registers by the system specific machine check 
**	handler.
** 
*/ 
	ALIGN_BRANCH_TARGET

MchkBugCheck:
	lda	p7, MCHK_K_BUGCHECK(zero)
	br	zero, 1f

	ALIGN_BRANCH_TARGET

MchkOSBugCheck:
	lda	p7, MCHK_K_OS_BUGCHECK(zero)
	br	zero, 1f

	ALIGN_BRANCH_TARGET

1:	sll	p7, 32, p7		// Move error code into upper longword
	mtpr	p6, pt10		// Save the exception address

	mtpr	p6, excAddr
	mfpr	p4, ptMces		// Get MCES and scratch bits

	zap	p4, 0x3C, p4		// Isolate just MCES bits

	bis	p4, p7, p4		// Combine MCES bits and error code
	lda	p7, SCB_Q_PROCMCHK(zero) // Load the SCB vector

	sll	p7, 16, p7		// Move SCB vector into upper word

	or	p4, p7, p7		// Combine SCB vector and MCHK bits

	mtpr	v0, pt0			// Save v0
	bis	p7, MCES_M_MIP, p7	// Set Machine-Check-In-Progress flag

	mtpr	p7, ptMces		// Save combined SCB and MCHK bits
	ldah	p6, SEXT(0xFFF0)(zero)	// p6 <- 00.FFF0.0000

	mtpr	t0, pt1			// Save t0
	zap	p6, 0xE0, p6		// p6 <- FF.FFF0.0000 Cbox IPR base

	mtpr	t3, pt4			// Save t3

	mtpr	t4, pt5			// Save t4

	mtpr	t5, pt6			// Save t5

	blbs	p4, HaltDoubleMchk	// Check if MIP flag was previously set

	br	zero, MchkCommon	// Go join common MCHK flow ...

/*======================================================================*/
/*                    PROCESSOR MACHINE CHECK HANDLER                   */
/*======================================================================*/
/*
**
** FUNCTIONAL DESCRIPTION:
**
**	The processor machine check error handler stores a code 
**	indicating a processor machine check error, loads
**	the System Control Block (SCB) vector for the
**	processor machine check service routine, sets the
**	Machine-Check-In-Progress (MIP) flag in the Machine
**	Check Error Summary register (MCES), and merges
**	with the common machine check flow.
**
**	If a second processor machine check error condition is 
**	detected while the MIP flag is set, the processor is 
**	forced into console I/O mode indicating "double error 
**	abort encountered" as the reason for the halt. 
**
** CALLING SEQUENCE:
** 
**	Called when an internal processor error is detected
**	that cannot be successfully corrected by hardware or
**	PALcode.
** 
** INPUT PARAMETERS:
**
**	icFlush requested
** 
** OUTPUT PARAMETERS:
**
**	pt0		= saved v0
**	pt1		= saved t0
**	pt4		= saved t3
**	pt5		= saved t4
**	pt6		= saved t5
**	ptMisc<47:32>	= MCHK code
**	ptMisc<31:16>	= SCB vector
**	ptMces<MIP>	= Set
**	p6 		= Cbox IPR base address
** 
** SIDE EFFECTS:
**
**	r0 (v0), r1 (t0), and r4..r6 (t3..t5) are saved in
**	PAL temporaries and are available for use as scratch
**	registers by the system specific machine check 
**	handler.
** 
*/ 

	ALIGN_BLOCK

MchkContinue:
/*
** Due to a potential 2nd Cbox register file access for uncorrectable
** errors, we need to insure that no writes to the register file occur
** for 7 cycles ...
*/
	NOP
	mtpr	v0, pt0			// Save v0, OK if Cbox trashes v0 later

	NOP
	NOP

	NOP
	NOP

	NOP
	NOP

	NOP
	NOP				// 10 instructions = 5 cycles

	NOP
	NOP

	NOP
	NOP				// Register file can now be written
/*
** Need to fill up the Icache refill buffer (32 instructions) and
** then flush the Icache again ...
*/
	lda	v0, SCB_Q_PROCMCHK(zero)// Get the SCB vector
	mfpr	p5, ptMces		// Get the error summary bits
	sll	v0, PT16_V_SCB, v0	// Shift SCB vector into position
	bis	p5, MCES_M_MIP, p6	// Set Machine-Check-In-Progress bit
	zap	p6, 0x3C, p6		// Clear space for MCHK and SCB words
	mtpr	p6, ptMces		// Update the chip - 20 instructions
	NOP
	or	p6, v0, p6		// Merge in the SCB vector
	lda	v0, MCHK_K_PROC_HERR(zero) // Get the MCHK code
	mfpr	p4, excAddr		// Get the exception address
	sll	v0, PT16_V_MCHK, v0	// Shift MCHK code into position
	mtpr	t3, pt4			// Save t3
	or	p6, v0, p6		// Merge in the MCHK code
	mtpr	p6, ptMisc		// Store updated MCES, MCHK, SCB bits
	ldah	p6, SEXT(0xFFF0)(zero)	// r14 <- 0.FFF0.0000
	mtpr	t0, pt1			// Save t0 - 30 instructions
	zap	p6, 0xE0, p6		// Form Cbox IPR base address
	mtpr	p4, pt10		// Stash the exception address

	mtpr	zero, icFlush		// Flush the Icache

	blbs	p5, HaltDoubleMchk	// Was a MCHK already in progress?
	blbs	p4, MchkFromPal		// Was MCHK while in PALmode?

	mtpr	t5, pt6			// Save t5
	mtpr	t4, pt5			// Save t4

/*======================================================================*/
/*                    COMMON MACHINE CHECK HANDLER                     */
/*======================================================================*/
/*
**
** Start to collect IPR data to store in the logout frame.
** This is a common entry point for interrupt system machine check,
** processor machine check, and pal-detected machine check.
**
** Current State:
**
**	pt0		Saved v0
**	pt1		Saved t0
**	pt4		Saved t3
**	pt5		Saved t4
**	pt6		Saved t5
**	ptMisc<47:32>	MCHK code
**	ptMisc<31:16>	SCB vector
**	ptMces<MIP>	Set
**	p6		Base address of Cbox IPRs in I/O space
**
** Registers v0, t0, and t3 .. t5 are available for use, in
** addition to the shadow registers p0 .. p2, p4, p5, and p7.
**
*/
	
MchkCommon:
	mb				// Serialize access to Scache IPRs
	mfpr	t0, icPerr		// Get cache parity error status bits

                                        // PVC lumps MB with Mbox instructions
pvc$osf30$379:				// but MB ok here, so ignore violation
	mfpr	p0, dcPerr		// MBOX INST->MF DC_PERR_STAT IN 0,1,2

	mtpr	zero, dcFlush		// Flush the Dcache

	STALL				// Pad Mbox instructions from dcFlush
	STALL
	STALL
	NOP

	ldq_p	p1, scAddr(p6)		// Get Scache error address
	or	p1, zero, zero		// Make sure scAddr load completes
	
	ldq_p	p2, scStat(p6)		// Get error status (unlocks scAddr)
	ldq_p	p4, eiAddr(p6)		// Get Bcache error address 
	ldq_p	p5, bcTagAddr(p6)	// Get Bcache tag
	ldq_p	v0, fillSyn(p6)		// Get the fill syndrome
	or	p4, zero, zero		// Make sure eiAddr load completes
	or	v0, zero, zero		// Make sure fillsyn load completes

	ldq_p	p7, eiStat(p6)		// Get error status (unlocks eiAddr)
	ldq_p	zero, eiStat(p6)	// Read it again to insure unlocked
/*
** NEED TO FIX -
** Code to look for re-tryable and non-retryable cases goes here ...
** Assume non-retryable for now, and just retrieve machine check code
** from ptMces.
*/

	bis	zero, zero, t4		// Assume non-retryable for now ...
	mfpr	t5, ptMisc		// Get machine check code back
	extwl	t5, 4, t5
	bic	t5, 1, t5		// Clear flag from interrupt

	br	zero, MchkLogOut	// Go build logout and stack frames ...


/*
** Double machine check or machine check while in PALmode
*/
	ALIGN_BRANCH_TARGET

HaltDoubleMchk:
pvc$osf31$5000:
	bsr	v0, UpdatePCB

	lda	v0, HLT_K_DBL_MCHK(zero)
	br	zero, Sys_EnterConsole

	ALIGN_BRANCH_TARGET

MchkFromPal:
pvc$osf32$5000:
	bsr	v0, UpdatePCB

	lda	v0, HLT_K_MCHK_FROM_PAL(zero)
	br	zero, Sys_EnterConsole

/*======================================================================*/
/*             BUILD MACHINE CHECK LOGOUT FRAME                         */
/*======================================================================*/
/*
**	This portion of the  machine check handler builds a logout frame
**	in the PAL impure scratch area, builds a stack frame on the kernel
**	stack (already built if there was an interrupt machine check),
**	loads the GP with the KGP, loads the machine check entry 
**	code in a0, loads a platform-specific interrupt vector 
**	(typically the same value as the SCB offset) in a1, loads 
**	the kseg address of the logout area in a2, and dispatches 
**	to the kernel interrupt handler pointed to by the entInt 
**	operating system entry point.
**
** Write the Machine Check error logout frame.
**
** Current State:
**
**	v0	Fill Syndrome
**	t0	Icache Parity Error Status
**	t3	Scratch
**	t4	Retry flag
**	t5	MCHK code
**	p0	Dcache Parity Error Status
**	p1	Scache Error Address
**	p2	Scache Error Status
**	p4	Bcache/System Error Address
**	p5	Bcache Tag Address
**	p6	Scratch
**	p7	Bcache/System Error Status
**
**	pt0	Saved v0
**	pt1	Saved t0
**	pt4	Saved t3
**	pt5	Saved t4
**	pt6	Saved t5
**	pt10	Saved excAddr
**	ptMisc<47:32>	= MCHK code with low bit as interrupt flag
**	ptMisc<31:16>	= SCB vector
**	ptMces<MIP>	= Set
**
** OUTPUT PARAMETERS:
** 
**	a0 (r16) = Machine check entry type
**	a1 (r17) = Platform-specific interrupt vector
**	a2 (r18) = Pointer to logout area
**
*/
	ALIGN_BRANCH_TARGET

MchkLogOut:

	mfpr	p6, ptImpure		// Get address of impure scratch area
	lda	p6, LAF_Q_BASE(p6)	// Add offset to Logout Frame
/*
** Write the first two quadwords of the frame
*/

	lda	t3, LAF_K_SIZE(t4)	  // Combine retry flag and frame size
	stq_p	t3, LAF_L_SIZE(p6)	  // Store flag/frame size
	lda	t3, LAF_Q_SYS_BASE(zero)  // Get offset to system-specific info
	sll	t3, 32, t3		  // Shift offset into upper longword
	lda	t3, LAF_Q_CPU_BASE(t3)	  // Combine with offset to CPU info
	stq_p	t3, LAF_Q_OFFSET_BASE(p6) // Store offsets to system/cpu info
/*
** Write the MCHK code and error IPRs previously fetched 
** to the frame.  Restore some GPRs from PALtemps.
*/
	mfpr	t4, pt5			// Restore t4
	stq_p	t5, LAF_Q_MCHK_CODE(p6)	// Store MCHK code
	mfpr	t3, pt4			// Restore t3
	stq_p	t0, LAF_Q_ICPERR(p6)	// Store Icache Parity Error Status
	mfpr	t5, pt6			// Restore t5
	stq_p	p0, LAF_Q_DCPERR(p6)	// Store Dcache Parity Error Status
	mfpr	t0, pt1			// Restore t0

	stq_p	p1, LAF_Q_SC_ADDR(p6)	// Store Scache Error Address
	stq_p	p2, LAF_Q_SC_STAT(p6)	// Store Scache Error Status
	stq_p	p4, LAF_Q_EI_ADDR(p6)	// Store Bcache/System Error Address
	stq_p	p5, LAF_Q_BC_TAG_ADDR(p6)   // Store Bcache Tag Address
	stq_p	v0, LAF_Q_FILL_SYNDROME(p6) // Store Fill Syndrome
	stq_p	p7, LAF_Q_EI_STAT(p6)	// Store Bcache/System Error Status
	mfpr	p7, pt10		// Get saved excAddr
	stq_p	p7, LAF_Q_EXC_ADDR(p6)	// Store Exception Address
/*
** Complete the CPU-specific section of the frame ...
*/
	STORE_IPR(mmStat,LAF_Q_MM_STAT,p6)
	STORE_IPR(va,LAF_Q_VA,p6)
	STORE_IPR(isr,LAF_Q_ISR,p6)
	STORE_IPR(icsr,LAF_Q_ICSR,p6)
	STORE_IPR(palBase,LAF_Q_PAL_BASE,p6)
	STORE_IPR(excMask,LAF_Q_EXC_MASK,p6)
	STORE_IPR(excSum,LAF_Q_EXC_SUM,p6)

	ldah	p5, SEXT(0xFFF0)(zero)	// p5 <- FF...FFF0.0000
	zap	p5, 0xE0, p5		// Form Cbox IPR base address

	ldq_p	p5, ldLock(p5)		// Get the LD_LOCK IPR
	stq_p	p5, LAF_Q_LD_LOCK(p6)	// Stash it in the logout frame
/* 
** Complete the PAL-specific section of the frame ...
*/
	STORE_IPR(pt0,LAF_Q_PT+0x00,p6)
	STORE_IPR(pt1,LAF_Q_PT+0x08,p6)
	STORE_IPR(pt2,LAF_Q_PT+0x10,p6)
	STORE_IPR(pt3,LAF_Q_PT+0x18,p6)
	STORE_IPR(pt4,LAF_Q_PT+0x20,p6)
	STORE_IPR(pt5,LAF_Q_PT+0x28,p6)
	STORE_IPR(pt6,LAF_Q_PT+0x30,p6)
	STORE_IPR(pt7,LAF_Q_PT+0x38,p6)
	STORE_IPR(pt8,LAF_Q_PT+0x40,p6)
	STORE_IPR(pt9,LAF_Q_PT+0x48,p6)
	STORE_IPR(pt10,LAF_Q_PT+0x50,p6)
	STORE_IPR(pt11,LAF_Q_PT+0x58,p6)
	STORE_IPR(pt12,LAF_Q_PT+0x60,p6)
	STORE_IPR(pt13,LAF_Q_PT+0x68,p6)
	STORE_IPR(pt14,LAF_Q_PT+0x70,p6)
	STORE_IPR(pt15,LAF_Q_PT+0x78,p6)
	STORE_IPR(pt16,LAF_Q_PT+0x80,p6)
	STORE_IPR(pt17,LAF_Q_PT+0x88,p6)
	STORE_IPR(pt18,LAF_Q_PT+0x90,p6)
	STORE_IPR(pt19,LAF_Q_PT+0x98,p6)
	STORE_IPR(pt20,LAF_Q_PT+0xA0,p6)
	STORE_IPR(pt21,LAF_Q_PT+0xA8,p6)
	STORE_IPR(pt22,LAF_Q_PT+0xB0,p6)
	STORE_IPR(pt23,LAF_Q_PT+0xB8,p6)
/*
** Complete the system-specific section of the frame ...
*/
pvc$osf33$5050:
	bsr	v0, Sys_MchkLogOut

	mfpr	v0, pt0			// Restore v0
/*
** Clear the data parity, tag parity, and timeout reset error 
** bits in the Icache parity error status register.  
*/
	STALL
	lda	p0, (ICPERR_M_DPE | ICPERR_M_TPE | ICPERR_M_TMR)(zero)
	mtpr	p0, icPerr
/* 
** Clear the second parity error and lock-on-parity-error-detect 
** bits in the Dcache parity error status register.
*/
	lda	p0, (DCPERR_M_LOCK | DCPERR_M_SEO)(zero)
	mtpr	p0, dcPerr

	mtpr	zero, excSum		// Clear the exception summary
/*
** Build the stack frame on the kernel stack and post the interrupt. 
** If we entered this flow from the machine check interrupt handler, 
** then bit <0> of the MCHK code in the ptMces will be set, indicating
** that the stack frame has already been built. 
*/
	mfpr	p7, ptMisc		// Get the MCHK error summary bits
	extwl	p7, 4, p7		// Extract the MCHK code
	blbs	p7, 1f			// Check if stack frame already built

	bis	zero, p6, p4		// Stash pointer to logout area
	bis	zero, ps, p5		// Save a copy of the PS shadow
	mfpr	p6, pt10		// Get saved exception address
	mtpr	zero, dtbCm		// Set Mbox mode to kernel
	mtpr	zero, ips		// Set Ibox mode to kernel

	STACK_FRAME(p5,p6,p7)

	mfpr	p7, ptIntMask		// Get the IPL translation mask.
	extbl	p7, IPL_K_MCHK, p7	// Translate to internal IPL.

	bis	zero, IPL_K_MCHK, ps	// Set new ps.
	mtpr	p7, ipl			// Load the target IPL.

	mfpr	p7, ptEntInt		// Get pointer to kernel handler
	lda	a0, INT_K_MCHK(zero)	// Flag as MCHK/CRD in a0
	mfpr	a1, ptMisc		// Get the MCHK error summary bits
	mtpr	p7, excAddr		// Load kernel int handler address
	subq	zero, 1, a2		// Get a '-1'
	extwl	a1, 2, a1		// Isolate SCB vector and pass in a1
	srl	a2, (VA_S_SIZE-1), a2	// Shift off low bits for kseg address
	sll	a2, (VA_S_SIZE-1), a2	// Shift kseg bits back into position
	mfpr	gp, ptKgp		// Load the kernel global pointer
	bis	p4, a2, a2		// Pass ptr to logout area as kseg addr
	hw_rei				// Transfer to kernel interrupt handler

	ALIGN_BRANCH_TARGET

1:	lda	a0, INT_K_MCHK(zero)	// Flag as MCHK/CRD in a0
	lda	a1, SCB_Q_SYSMCHK(zero)	// Load SCB vector
	subq	zero, 1, a2		// Get a '-1'
	mfpr	p7, ptEntInt		// Get pointer to kernel handler
	srl	a2, (VA_S_SIZE-1), a2	// Shift off low bits for kseg address
	mtpr	p7, excAddr		// Load kernel interrupt handler
	sll	a2, (VA_S_SIZE-1), a2	// Shift kseg bits back into position
	bis	p6, a2, a2		// Pass ptr to logout area as kseg addr
	hw_rei				// Transfer to kernel interrupt handler

/*======================================================================*/
/*             END OF MACHINE CHECK ROUTINES                            */
/*======================================================================*/

/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Update the current PCB with new SP and CC info.
** 
** CALLING SEQUENCE:
** 
** 
** INPUT PARAMETERS:
** 
**	NOTE:  Since r0 (v0) is used as the return address,
**	it should not be used by any code here (or saved
**	and restored if it must be used).
**
**	r0 (v0) = return linkage
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

UpdatePCB:
	mfpr	p4, ptPcbb		// Get PCBB
	and	ps, PS_M_CM, p7		// Isolate the current mode

	rpcc	p5			// Read the cycle counter
	beq	p7, 1f			// Don't update USP in kernel mode

	mtpr	sp, ptUsp		// Save the user stack pointer
	stq_p	sp, PCB_Q_USP(p4)	// Store user stack pointer

	br	zero, 2f		// Join common flow

1:	stq_p	sp, PCB_Q_KSP(p4)	// Store the kernel stack pointer

2:	srl	p5, 32, p6		// Get CC<OFFSET> bits
	addl	p5, p6, p6		// Merge for new time

	stl_p	p6, PCB_L_PCC(p4)	// Store new time

pvc$osf34$5000.1:			// PVC subroutine branch target
	ret	zero, (v0)


/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Save all CPU-specific state to the PALcode impure scratch
**	area.
** 
** INPUT PARAMETERS:
** 
**	r0 (v0) = Halt code
**	r1 (t0) = Base address of impure area
**	r3 (t2) = Return address
**
**	pt0	= Saved r0
**	pt4	= Saved r1
**	pt5	= Saved r3
** 
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
** The console save state area is larger than the addressibility
** of the HW_LD/ST instructions (10-bit signed byte displacement), 
** so some adjustments to the base offsets, as well as the offsets 
** within each base region, are necessary.  
**
** The console save state area is divided into two segments; the 
** CPU-specific segment and the platform-specific segment.  The 
** state that is saved in the CPU-specific segment includes GPRs, 
** FPRs, IPRs, halt code, MCHK flag, etc.  All other state is saved 
** in the platform-specific segment.
**
** The impure pointer will need to be adjusted by a different offset
** value for each region within a given segment.  The SAVE and RESTORE 
** macros will auto-magically adjust the offsets accordingly.
** 
*/ 
	ALIGN_BLOCK

SaveState:

	lda	t0, 0x200(t0)		// Point to center of CPU segment

	SAVE_GPR(zero,CNS_Q_FLAG,t0)	// Clear the valid flag
	SAVE_GPR(v0,CNS_Q_HALT,t0)	// Save the halt code

	mfpr	v0, pt0			// Get r0 back
	SAVE_GPR(v0,CNS_Q_GPR+0x00,t0)	// Save r0

	mfpr	v0, pt4			// Get r1 back
	SAVE_GPR(v0,CNS_Q_GPR+0x08,t0)	// Save r1

	SAVE_GPR(r2,CNS_Q_GPR+0x10,t0)	// Save r2

	mfpr	v0, pt5			// Get r3 back
	SAVE_GPR(v0,CNS_Q_GPR+0x18,t0)	// Save r3

	SAVE_GPR(r4,CNS_Q_GPR+0x20,t0)	// Save r4
	SAVE_GPR(r5,CNS_Q_GPR+0x28,t0)	// Save r5
/*
** At this point, the halt code has been saved, r0..r5 have
** been saved, pt0, pt4, and pt5 are lost. BE AWARE that
** SAVE_IPR and RESTORE_IPR blow away r0(v0).
**
** Register use:
**	r0(v0)	scratch
**	r1(t0)	impure area pointer
**	r2(t1)	--
**	r3(t2)	return address
**	r4(t3)	no SDE version on icsr
**	r5(t4)	saved icsr
*/
	mfpr	r5, icsr		// Get out of shadow mode ...
	ldah	r4, (1<<(ICSR_V_SDE-16))(zero)
	bic	r5, r4, r4		// ICSR with ICSR<SDE> = 0.
	mtpr	r4, icsr		// Turn off SDE.

	STALL				// ICSR<SDE> bubble 1
	STALL				// ICSR<SDE> bubble 2
	STALL				// ICSR<SDE> bubble 3
	NOP
/*
** Save all remaining GPRs ...
*/
	SAVE_GPR(r6,CNS_Q_GPR+0x30,t0)
	SAVE_GPR(r7,CNS_Q_GPR+0x38,t0)
	SAVE_GPR(r8,CNS_Q_GPR+0x40,t0)
	SAVE_GPR(r9,CNS_Q_GPR+0x48,t0)
	SAVE_GPR(r10,CNS_Q_GPR+0x50,t0)
	SAVE_GPR(r11,CNS_Q_GPR+0x58,t0)
	SAVE_GPR(r12,CNS_Q_GPR+0x60,t0)
	SAVE_GPR(r13,CNS_Q_GPR+0x68,t0)
	SAVE_GPR(r14,CNS_Q_GPR+0x70,t0)
	SAVE_GPR(r15,CNS_Q_GPR+0x78,t0)
	SAVE_GPR(r16,CNS_Q_GPR+0x80,t0)
	SAVE_GPR(r17,CNS_Q_GPR+0x88,t0)
	SAVE_GPR(r18,CNS_Q_GPR+0x90,t0)
	SAVE_GPR(r19,CNS_Q_GPR+0x98,t0)
	SAVE_GPR(r20,CNS_Q_GPR+0xA0,t0)
	SAVE_GPR(r21,CNS_Q_GPR+0xA8,t0)
	SAVE_GPR(r22,CNS_Q_GPR+0xB0,t0)
	SAVE_GPR(r23,CNS_Q_GPR+0xB8,t0)
	SAVE_GPR(r24,CNS_Q_GPR+0xC0,t0)
	SAVE_GPR(r25,CNS_Q_GPR+0xC8,t0)
	SAVE_GPR(r26,CNS_Q_GPR+0xD0,t0)
	SAVE_GPR(r27,CNS_Q_GPR+0xD8,t0)
	SAVE_GPR(r28,CNS_Q_GPR+0xE0,t0)
	SAVE_GPR(r29,CNS_Q_GPR+0xE8,t0)
	SAVE_GPR(r30,CNS_Q_GPR+0xF0,t0)
	SAVE_GPR(r31,CNS_Q_GPR+0xF8,t0)

	lda	t0, -0x200(t0)		// Restore the impure base address.
	lda	t0, CNS_Q_IPR(t0)	// Point to the base of IPR area.
/*
** Save all PAL temporary registers ...
*/	 
	SAVE_IPR(pt0,CNS_Q_PT+0x00,t0)
	SAVE_IPR(pt1,CNS_Q_PT+0x08,t0)
	SAVE_IPR(pt2,CNS_Q_PT+0x10,t0)
	SAVE_IPR(pt3,CNS_Q_PT+0x18,t0)
	SAVE_IPR(pt4,CNS_Q_PT+0x20,t0)
	SAVE_IPR(pt5,CNS_Q_PT+0x28,t0)
	SAVE_IPR(pt6,CNS_Q_PT+0x30,t0)
	SAVE_IPR(pt7,CNS_Q_PT+0x38,t0)
	SAVE_IPR(pt8,CNS_Q_PT+0x40,t0)
	SAVE_IPR(pt9,CNS_Q_PT+0x48,t0)
	SAVE_IPR(pt10,CNS_Q_PT+0x50,t0)
	SAVE_IPR(pt11,CNS_Q_PT+0x58,t0)
	SAVE_IPR(pt12,CNS_Q_PT+0x60,t0)
	SAVE_IPR(pt13,CNS_Q_PT+0x68,t0)
	SAVE_IPR(pt14,CNS_Q_PT+0x70,t0)
	SAVE_IPR(pt15,CNS_Q_PT+0x78,t0)
	SAVE_IPR(pt16,CNS_Q_PT+0x80,t0)
	SAVE_IPR(pt17,CNS_Q_PT+0x88,t0)
	SAVE_IPR(pt18,CNS_Q_PT+0x90,t0)
	SAVE_IPR(pt19,CNS_Q_PT+0x98,t0)
	SAVE_IPR(pt20,CNS_Q_PT+0xA0,t0)
	SAVE_IPR(pt21,CNS_Q_PT+0xA8,t0)
	SAVE_IPR(pt22,CNS_Q_PT+0xB0,t0)
	SAVE_IPR(pt23,CNS_Q_PT+0xB8,t0)

	STALL
	STALL
	mtpr	r5, icsr		// Restore shadow mode.
	
	STALL				// ICSR<SDE> bubble 1
	STALL				// ICSR<SDE> bubble 2
	STALL				// ICSR<SDE> bubble 3
	NOP
/*
** Save all shadow registers ...
*/
	SAVE_SHADOW(p0,CNS_Q_SHADOW+0x00,t0)
	SAVE_SHADOW(p1,CNS_Q_SHADOW+0x08,t0)
	SAVE_SHADOW(p2,CNS_Q_SHADOW+0x10,t0)
	SAVE_SHADOW(ps,CNS_Q_SHADOW+0x18,t0)
	SAVE_SHADOW(p4,CNS_Q_SHADOW+0x20,t0)
	SAVE_SHADOW(p5,CNS_Q_SHADOW+0x28,t0)
	SAVE_SHADOW(p6,CNS_Q_SHADOW+0x30,t0)
	SAVE_SHADOW(p7,CNS_Q_SHADOW+0x38,t0)
/*
** Save IPRs ...
*/
	SAVE_IPR(excAddr,CNS_Q_EXC_ADDR,t0)
	SAVE_IPR(palBase,CNS_Q_PAL_BASE,t0)
	SAVE_IPR(mmStat,CNS_Q_MM_STAT,t0)
	SAVE_IPR(va,CNS_Q_VA,t0)
	SAVE_IPR(icsr,CNS_Q_ICSR,t0)
	SAVE_IPR(ipl,CNS_Q_IPL,t0)
	SAVE_IPR(ips,CNS_Q_IPS,t0)
	SAVE_IPR(itbAsn,CNS_Q_ITB_ASN,t0)
	SAVE_IPR(aster,CNS_Q_ASTER,t0)
	SAVE_IPR(astrr,CNS_Q_ASTRR,t0)
	SAVE_IPR(sirr,CNS_Q_SIRR,t0)
	SAVE_IPR(isr,CNS_Q_ISR,t0)
	SAVE_IPR(iVptBr,CNS_Q_IVPTBR,t0)
	SAVE_IPR(mcsr,CNS_Q_MCSR,t0)
	SAVE_IPR(dcMode,CNS_Q_DC_MODE,t0)
                                            // PVC violation applies only to
pvc$osf35$379:				    // loads. HW_ST ok here, so ignore
	SAVE_IPR(mafMode,CNS_Q_MAF_MODE,t0) // MBOX INST->MF MAF_MODE IN 0,1,2
/*
** The following IPRs are saved for informational purposes only, and
** are not restored.
*/
	SAVE_IPR(icPerr,CNS_Q_ICPERR_STAT,t0)
	SAVE_IPR(PmCtr,CNS_Q_PM_CTR,t0)
	SAVE_IPR(excSum,CNS_Q_EXC_SUM,t0)
	SAVE_IPR(excMask,CNS_Q_EXC_MASK,t0)
	SAVE_IPR(intId,CNS_Q_INT_ID,t0)

	ldah	p6, SEXT(0xFFF0)(zero)
	zap	p6, 0xE0, p6		// Get base address of CBOX IPRs
	NOP				// Pad mfpr dcPerr out of shadow of
	NOP				// last store
	NOP
	SAVE_IPR(dcPerr,CNS_Q_DCPERR_STAT,t0)
/*
** Read CBOX IPR state ...
*/
	mb

	ldq_p	t1, scCtl(p6)
	ldq_p	p5, ldLock(p6)
	ldq_p	t3, scAddr(p6)
	ldq_p	t4, eiAddr(p6)
	ldq_p	t5, bcTagAddr(p6)
	ldq_p	t6, fillSyn(p6)
	bis	t3, t4, zero		// Make sure all loads complete before
	bis	t5, t6, zero		// reading registers that unlock them.

	ldq_p	p0, scStat(p6)		// Unlocks scAddr.
	ldq_p	p1, eiStat(p6)		// Unlocks eiAddr, bcTagAddr, fillSyn.
	ldq_p	zero, eiStat(p6)	// Make sure it is really unlocked.
	mb
/*
** NEED TO FIX  - Code to save CBOX IPRs goes here ...
*/

	lda	t0, -CNS_Q_IPR(t0)	// Restore the impure base address

	mfpr	v0, icsr		// Get the current ICSR 
	bis	zero, 1, t1		// Get a '1'
	sll	t1, ICSR_V_FPE, t1	// Shift it into ICSR<FPE> position
	bis	t1, v0, v0		// Enable floating point
	mtpr	v0, icsr		// Update the chip
/*
** Map the PAL impure scratch area virtually
*/
	mtpr	zero, dtbIa		// Clear all DTB entries
	srl	t0, VA_S_OFF, v0	// Clean off byte-within-page offset
	sll	v0, PTE_V_PFN, v0	// Shift to form PFN
	lda	v0, PTE_M_PROT(v0)	// Set all read/write enable bits
	mtpr	v0, dtbPte		// Load the PTE and set valid
	mtpr	t0, dtbTag		// Write the PTE and tag into the DTB
/*
** Map the next page too, in case we cross a page boundary ...
*/
	lda	t3, (1<<VA_S_OFF)(t0)	// Generate address for next page
	srl	t3, VA_S_OFF, v0	// Clean off byte-within-page offset
	sll	v0, PTE_V_PFN, v0	// Shift to form PFN
	lda	v0, PTE_M_PROT(v0)	// Set all read/write enable bits
	mtpr	v0, dtbPte		// Load the PTE and set valid
	mtpr	t3, dtbTag		// Write the PTE and tag into the DTB

	STALL
	STALL
	STALL
	NOP

	lda	t0, 0x200(t0)		// Point to center of CPU segment
/*
** Save all FPRs ...
*/
	SAVE_FPR(f0,CNS_Q_FPR+0x00,t0)
	SAVE_FPR(f1,CNS_Q_FPR+0x08,t0)
	SAVE_FPR(f2,CNS_Q_FPR+0x10,t0)
	SAVE_FPR(f3,CNS_Q_FPR+0x18,t0)
	SAVE_FPR(f4,CNS_Q_FPR+0x20,t0)
	SAVE_FPR(f5,CNS_Q_FPR+0x28,t0)
	SAVE_FPR(f6,CNS_Q_FPR+0x30,t0)
	SAVE_FPR(f7,CNS_Q_FPR+0x38,t0)
	SAVE_FPR(f8,CNS_Q_FPR+0x40,t0)
	SAVE_FPR(f9,CNS_Q_FPR+0x48,t0)
	SAVE_FPR(f10,CNS_Q_FPR+0x50,t0)
	SAVE_FPR(f11,CNS_Q_FPR+0x58,t0)
	SAVE_FPR(f12,CNS_Q_FPR+0x60,t0)
	SAVE_FPR(f13,CNS_Q_FPR+0x68,t0)
	SAVE_FPR(f14,CNS_Q_FPR+0x70,t0)
	SAVE_FPR(f15,CNS_Q_FPR+0x78,t0)
	SAVE_FPR(f16,CNS_Q_FPR+0x80,t0)
	SAVE_FPR(f17,CNS_Q_FPR+0x88,t0)
	SAVE_FPR(f18,CNS_Q_FPR+0x90,t0)
	SAVE_FPR(f19,CNS_Q_FPR+0x98,t0)
	SAVE_FPR(f20,CNS_Q_FPR+0xA0,t0)
	SAVE_FPR(f21,CNS_Q_FPR+0xA8,t0)
	SAVE_FPR(f22,CNS_Q_FPR+0xB0,t0)
	SAVE_FPR(f23,CNS_Q_FPR+0xB8,t0)
	SAVE_FPR(f24,CNS_Q_FPR+0xC0,t0)
	SAVE_FPR(f25,CNS_Q_FPR+0xC8,t0)
	SAVE_FPR(f26,CNS_Q_FPR+0xD0,t0)
	SAVE_FPR(f27,CNS_Q_FPR+0xD8,t0)
	SAVE_FPR(f28,CNS_Q_FPR+0xE0,t0)
	SAVE_FPR(f29,CNS_Q_FPR+0xE8,t0)
	SAVE_FPR(f30,CNS_Q_FPR+0xF0,t0)
	SAVE_FPR(f31,CNS_Q_FPR+0xF8,t0)

	lda	t0, -0x200(t0)		// Restore the impure base address
	lda	t0, CNS_Q_IPR(t0)	// Point to base of IPR area again
/*
** NEED TO FIX  - Code to save the FPCTL IPR goes here ...
*/
	mtpr	zero, dtbIa		// Clear all DTB entries

	lda	t0, -CNS_Q_IPR(t0)	// Back to base of impure area again,
	lda	t0, 0x200(t0)		// ... back to center of CPU segment

	bis	zero, 1, v0		// Get a '1'
	SAVE_GPR(v0,CNS_Q_FLAG,t0)	// Set the dump area valid flag
	RESTORE_GPR(v0,CNS_Q_HALT,t0)	// Restore the halt code

	lda	t0, -0x200(t0)		// Restore the impure base address

pvc$osf36$5020.1:
	ret	zero, (t2)		// Back to caller ...

/*
**
** FUNCTIONAL DESCRIPTION:
**
**	Restore the saved CPU-specific state from the PALcode
**	impure scratch area.
** 
** INPUT PARAMETERS:
** 
**	r1 (t0) = Base address of impure area
**	r3 (t2) = Return address
**
** OUTPUT PARAMETERS:
**
** 
** SIDE EFFECTS:
**
** The displacement field of the HW_LD/ST instruction holds a
** 10-bit signed byte offset.  Since the console save state
** area is larger than the addressibility of the HW_LD/ST
** instructions, the pointer to the base of the impure scratch
** area must be adjusted accordingly, to get all the loads and 
** stores to work properly.
** 
*/ 
	ALIGN_BLOCK

RestoreState:

/*
** Map the PAL impure scratch area virtually
*/
	mtpr	zero, dtbIa		// Clear all DTB entries
	srl	t0, VA_S_OFF, v0	// Clean off byte-within-page offset
	sll	v0, PTE_V_PFN, v0	// Shift to form PFN
	lda	v0, PTE_M_PROT(v0)	// Set all read/write enable bits
	mtpr	v0, dtbPte		// Load the PTE and set valid
	mtpr	t0, dtbTag		// Write the PTE and tag into the DTB
/*
** Map the next page too, in case we cross a page boundary
*/
	lda	t3, (1<<VA_S_OFF)(t0)	// Generate address for next page
	srl	t3, VA_S_OFF, v0	// Clean off byte-within-page offset
	sll	v0, PTE_V_PFN, v0	// Shift to form PFN
	lda	v0, PTE_M_PROT(v0)	// Set all read/write enable bits
	mtpr	v0, dtbPte		// Load the PTE and set valid
	mtpr	t3, dtbTag		// Write the PTE and tag into the DTB
/*
** Enable Floating point and shadow modes ... 
*/
	mfpr	v0, icsr		// Get current ICSR
	bis	zero, 1, t1		// Get a '1'
	or	t1, (1<<(ICSR_V_SDE-ICSR_V_FPE)), t1
	sll	t1, ICSR_V_FPE, t1	// Shift bits into position
	bis	v0, t1, v0		// Set ICSR<SDE> and ICSR<FPE>
	mtpr	t1, icsr		// Update the chip

	STALL				// ICSR<SDE/FPE> bubble 1
	STALL				// ICSR<SDE/FPE> bubble 2
	STALL				// ICSR<SDE/FPE> bubble 3

	lda	t0, 0x200(t0)		// Point to center of CPU segment.
/*
** Restore all floating point registers ...
*/
	RESTORE_FPR(f0,CNS_Q_FPR+0x00,t0)
	RESTORE_FPR(f1,CNS_Q_FPR+0x08,t0)
	RESTORE_FPR(f2,CNS_Q_FPR+0x10,t0)
	RESTORE_FPR(f3,CNS_Q_FPR+0x18,t0)
	RESTORE_FPR(f4,CNS_Q_FPR+0x20,t0)
	RESTORE_FPR(f5,CNS_Q_FPR+0x28,t0)
	RESTORE_FPR(f6,CNS_Q_FPR+0x30,t0)
	RESTORE_FPR(f7,CNS_Q_FPR+0x38,t0)
	RESTORE_FPR(f8,CNS_Q_FPR+0x40,t0)
	RESTORE_FPR(f9,CNS_Q_FPR+0x48,t0)
	RESTORE_FPR(f10,CNS_Q_FPR+0x50,t0)
	RESTORE_FPR(f11,CNS_Q_FPR+0x58,t0)
	RESTORE_FPR(f12,CNS_Q_FPR+0x60,t0)
	RESTORE_FPR(f13,CNS_Q_FPR+0x68,t0)
	RESTORE_FPR(f14,CNS_Q_FPR+0x70,t0)
	RESTORE_FPR(f15,CNS_Q_FPR+0x78,t0)
	RESTORE_FPR(f16,CNS_Q_FPR+0x80,t0)
	RESTORE_FPR(f17,CNS_Q_FPR+0x88,t0)
	RESTORE_FPR(f18,CNS_Q_FPR+0x90,t0)
	RESTORE_FPR(f19,CNS_Q_FPR+0x98,t0)
	RESTORE_FPR(f20,CNS_Q_FPR+0xA0,t0)
	RESTORE_FPR(f21,CNS_Q_FPR+0xA8,t0)
	RESTORE_FPR(f22,CNS_Q_FPR+0xB0,t0)
	RESTORE_FPR(f23,CNS_Q_FPR+0xB8,t0)
	RESTORE_FPR(f24,CNS_Q_FPR+0xC0,t0)
	RESTORE_FPR(f25,CNS_Q_FPR+0xC8,t0)
	RESTORE_FPR(f26,CNS_Q_FPR+0xD0,t0)
	RESTORE_FPR(f27,CNS_Q_FPR+0xD8,t0)
	RESTORE_FPR(f28,CNS_Q_FPR+0xE0,t0)
	RESTORE_FPR(f29,CNS_Q_FPR+0xE8,t0)
	RESTORE_FPR(f30,CNS_Q_FPR+0xF0,t0)
	RESTORE_FPR(f31,CNS_Q_FPR+0xF8,t0)

	lda	t0, -0x200(t0)		// Restore base address of impure area.
	lda	t0, CNS_Q_IPR(t0)	// Point to base of IPR area.
/*
** NEED TO FIX - Code to restore the FPCTL IPR goes here ...
*/

/*
** Restore all the PAL temporary registers ...
*/
	RESTORE_IPR(pt0,CNS_Q_PT+0x00,t0)
	RESTORE_IPR(pt1,CNS_Q_PT+0x08,t0)
	RESTORE_IPR(pt2,CNS_Q_PT+0x10,t0)
	RESTORE_IPR(pt3,CNS_Q_PT+0x18,t0)
	RESTORE_IPR(pt4,CNS_Q_PT+0x20,t0)
	RESTORE_IPR(pt5,CNS_Q_PT+0x28,t0)
	RESTORE_IPR(pt6,CNS_Q_PT+0x30,t0)
	RESTORE_IPR(pt7,CNS_Q_PT+0x38,t0)
	RESTORE_IPR(pt8,CNS_Q_PT+0x40,t0)
	RESTORE_IPR(pt9,CNS_Q_PT+0x48,t0)
	RESTORE_IPR(pt10,CNS_Q_PT+0x50,t0)
	RESTORE_IPR(pt11,CNS_Q_PT+0x58,t0)
	RESTORE_IPR(pt12,CNS_Q_PT+0x60,t0)
	RESTORE_IPR(pt13,CNS_Q_PT+0x68,t0)
	RESTORE_IPR(pt14,CNS_Q_PT+0x70,t0)
	RESTORE_IPR(pt15,CNS_Q_PT+0x78,t0)
	RESTORE_IPR(pt16,CNS_Q_PT+0x80,t0)
	RESTORE_IPR(pt17,CNS_Q_PT+0x88,t0)
	RESTORE_IPR(pt18,CNS_Q_PT+0x90,t0)
	RESTORE_IPR(pt19,CNS_Q_PT+0x98,t0)
	RESTORE_IPR(pt20,CNS_Q_PT+0xA0,t0)
	RESTORE_IPR(pt21,CNS_Q_PT+0xA8,t0)
	RESTORE_IPR(pt22,CNS_Q_PT+0xB0,t0)
	RESTORE_IPR(pt23,CNS_Q_PT+0xB8,t0)
/*
** Restore IPRs ...
** BE AWARE that v0 gets the value of RESTORE_IPR. This code
** takes advantage of that fact.
*/
	RESTORE_IPR(excAddr,CNS_Q_EXC_ADDR,t0)
	RESTORE_IPR(palBase,CNS_Q_PAL_BASE,t0)
	RESTORE_IPR(ipl,CNS_Q_IPL,t0)
	RESTORE_IPR(ips,CNS_Q_IPS,t0)
	mtpr	v0, dtbCm			// Set Mbox current mode too
	RESTORE_IPR(itbAsn,CNS_Q_ITB_ASN,t0)
	srl	v0, 4, v0
	sll	v0, 57, v0
	mtpr	v0, dtbAsn			// Set Mbox ASN too
	RESTORE_IPR(iVptBr,CNS_Q_IVPTBR,t0)
	mtpr	v0, mVptBr			// Set Mbox VptBr too
	RESTORE_IPR(mcsr,CNS_Q_MCSR,t0)
	RESTORE_IPR(aster,CNS_Q_ASTER,t0)
	RESTORE_IPR(astrr,CNS_Q_ASTRR,t0)
	RESTORE_IPR(sirr,CNS_Q_SIRR,t0)
	RESTORE_IPR(mafMode,CNS_Q_MAF_MODE,t0)
	STALL
	STALL
	STALL
	STALL
	STALL
/*
** Restore shadow registers ...
*/
	RESTORE_SHADOW(p0,CNS_Q_SHADOW+0x00,t0)
	RESTORE_SHADOW(p1,CNS_Q_SHADOW+0x08,t0)
	RESTORE_SHADOW(p2,CNS_Q_SHADOW+0x10,t0)
	RESTORE_SHADOW(ps,CNS_Q_SHADOW+0x18,t0)
	RESTORE_SHADOW(p4,CNS_Q_SHADOW+0x20,t0)
	RESTORE_SHADOW(p5,CNS_Q_SHADOW+0x28,t0)
	RESTORE_SHADOW(p6,CNS_Q_SHADOW+0x30,t0)
	RESTORE_SHADOW(p7,CNS_Q_SHADOW+0x38,t0)

	RESTORE_IPR(dcMode,CNS_Q_DC_MODE,t0)

	STALL
	STALL
	mfpr	v0, icsr		// Get out of shadow mode ...
	ldah	t1, (1<<(ICSR_V_SDE-16))(zero)
	bic	v0, t1, t1		// ICSR with ICSR<SDE> = 0.
	mtpr	t1, icsr		// Turn off SDE.

	STALL				// ICSR<SDE> bubble 1
	STALL				// ICSR<SDE> bubble 2
	STALL				// ICSR<SDE> bubble 3
	NOP
/*
** Restore GPRs (r0, r2 are restored later, r1 and r3 are trashed) ...
*/
	lda	t0, -CNS_Q_IPR(t0)	// Restore base address of impure area
	lda	t0, 0x200(t0)		// Point to center of CPU segment

	RESTORE_GPR(r4,CNS_Q_GPR+0x20,t0)
	RESTORE_GPR(r5,CNS_Q_GPR+0x28,t0)
	RESTORE_GPR(r6,CNS_Q_GPR+0x30,t0)
	RESTORE_GPR(r7,CNS_Q_GPR+0x38,t0)
	RESTORE_GPR(r8,CNS_Q_GPR+0x40,t0)
	RESTORE_GPR(r9,CNS_Q_GPR+0x48,t0)
	RESTORE_GPR(r10,CNS_Q_GPR+0x50,t0)
	RESTORE_GPR(r11,CNS_Q_GPR+0x58,t0)
	RESTORE_GPR(r12,CNS_Q_GPR+0x60,t0)
	RESTORE_GPR(r13,CNS_Q_GPR+0x68,t0)
	RESTORE_GPR(r14,CNS_Q_GPR+0x70,t0)
	RESTORE_GPR(r15,CNS_Q_GPR+0x78,t0)
	RESTORE_GPR(r16,CNS_Q_GPR+0x80,t0)
	RESTORE_GPR(r17,CNS_Q_GPR+0x88,t0)
	RESTORE_GPR(r18,CNS_Q_GPR+0x90,t0)
	RESTORE_GPR(r19,CNS_Q_GPR+0x98,t0)
	RESTORE_GPR(r20,CNS_Q_GPR+0xA0,t0)
	RESTORE_GPR(r21,CNS_Q_GPR+0xA8,t0)
	RESTORE_GPR(r22,CNS_Q_GPR+0xB0,t0)
	RESTORE_GPR(r23,CNS_Q_GPR+0xB8,t0)
	RESTORE_GPR(r24,CNS_Q_GPR+0xC0,t0)
	RESTORE_GPR(r25,CNS_Q_GPR+0xC8,t0)
	RESTORE_GPR(r26,CNS_Q_GPR+0xD0,t0)
	RESTORE_GPR(r27,CNS_Q_GPR+0xD8,t0)
	RESTORE_GPR(r28,CNS_Q_GPR+0xE0,t0)
	RESTORE_GPR(r29,CNS_Q_GPR+0xE8,t0)
	RESTORE_GPR(r30,CNS_Q_GPR+0xF0,t0)
	RESTORE_GPR(r31,CNS_Q_GPR+0xF8,t0)

	lda	t0, -0x200(t0)		// Restore base address of impure area.
	lda	t0, CNS_Q_IPR(t0)	// Point to base of IPR area again.

	RESTORE_IPR(icsr,CNS_Q_ICSR,t0)

	mtpr	zero, dtbIa		// Clear all DTB entries

	lda	t0, -CNS_Q_IPR(t0)	// Back to base of impure area again,
	lda	t0, 0x200(t0)		// and back to center of CPU segment

	SAVE_GPR(zero,CNS_Q_FLAG,t0)	// Clear the dump area valid flag
	mb

	RESTORE_GPR(r2,CNS_Q_GPR+0x10,t0)
	RESTORE_GPR(r0,CNS_Q_GPR+0x00,t0)

	lda	t0, -0x200(t0)		// Restore impure base address

pvc$osf37$5030.1:
	ret	zero, (t2)		// Back to caller ...

Pal_OsfpalEnd:







