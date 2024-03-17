/*****************************************************************************

       Copyright ) 1993, 1994 Digital Equipment Corporation,
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

#define RCSID "$Id: srom.s,v 1.9 1999/04/05 15:09:58 gries Exp $"

/*
 * $Log: srom.s,v $
 * Revision 1.9  1999/04/05 15:09:58  gries
 * new web setting
 *
 * Revision 1.8  1999/03/19 13:43:08  gries
 * change tpqmmax for brick
 *
 * Revision 1.7  1999/03/03 17:18:35  gries
 * Use only 2 bit in bcache size for web
 *
 * Revision 1.6  1999/02/24 20:57:00  gries
 * let's get a new version number
 *
 * Revision 1.5  1999/02/11 19:59:49  gries
 * Changes for White
 *
 * Revision 1.4  1999/02/11 17:49:28  gries
 * update for nt
 *
 * Revision 1.3  1999/01/25 18:55:52  gries
 * added flashing led and led left on if fatal error on brick
 *
 * Revision 1.2  1999/01/21 19:06:04  gries
 * First Release to cvs gries
 *
 * Revision 1.83  1998/10/23  19:24:34  gries
 * changes for Acer
 *
 * Revision 1.82  1998/10/14  19:53:32  gries
 * conditioned softrest
 *
 * Revision 1.81  1998/10/06  18:08:20  gries
 * tok out jim's arib (set with switch 7)
 *
 * Revision 1.80  1998/09/30  20:04:36  gries
 * make memory switch work not for dual cpu
 *
 * Revision 1.79  1998/09/30  18:27:41  gries
 * added test for dual to set memory to 2-3 or 3-3
 *
 * Revision 1.78  1998/09/28  15:39:05  gries
 * make memory speed a function of dual cpu
 *
 * Revision 1.77  1998/09/17  16:06:01  gries
 * Jim request for arb bit
 *
 * Revision 1.76  1998/09/10  15:15:57  gries
 * error write to LCD and new set for tsunami
 *
 * Revision 1.75  1998/09/03  19:18:25  gries
 * chages for LCD write pass 3 and Arbiter bit is 801.3800.01C0
 *
 * Revision 1.74  1998/07/30  21:43:20  gries
 * Changes for soft rest
 *
 * Revision 1.73  1998/07/14  21:36:06  gries
 * change in pctl0 time out vale
 *
 * Revision 1.72  1998/06/11  21:10:05  gries
 * removed pctl string
 *
 * Revision 1.71  1998/06/10  17:24:01  gries
 * Changes for pc264_debug
 *
 * Revision 1.70  1998/05/28  17:30:55  gries
 * sync with jb for pc264
 *
 * Revision 1.69  1998/05/22  14:41:08  gries
 * Fix a promblem with memory size calculation
 * added rev d hack (I uses bit 7 of mb configure for arb)
 * changes for pc264 condition include)
 *
 * Revision 1.68  1998/05/18  14:19:38  gries
 * changes for pc264
 *
 * Revision 1.67  1998/05/12  13:00:10  gries
 * Changes for PC264
 *
 * Revision 1.66  1998/04/10  19:40:28  gries
 * turn off ecc until ready to jump
 *
 * Revision 1.65  1998/04/03  20:52:38  gries
 * turn off Bcache after Bcache init
 *
 * Revision 1.64  1998/03/26  19:29:23  gries
 * fix ecc and minidebug
 *
 * Revision 1.63  1998/03/26  14:39:56  gries
 * set ecc error in dc_ctl
 *
 * Revision 1.62  1998/03/20  13:40:00  gries
 * last fix for sort of array dimm size
 *
 * Revision 1.61  1998/03/19  22:05:32  gries
 * sort by memory size
 *
 * Revision 1.60  1998/03/18  20:38:41  gries
 * change for iic memory detect
 *
 * Revision 1.59  1998/02/25  18:53:10  gries
 * ceb for the bcache size
 *
 * Revision 1.58  1998/02/23  19:54:45  gries
 * write the cache block in init bcache
 *
 * Revision 1.57  1998/02/19  14:35:19  gries
 * put back pass1 cache init
 *
 * Revision 1.56  1998/02/12  17:57:44  gries
 * Fixed CPU ID in r16
 *
 * Revision 1.55  1998/02/11  19:21:55  gries
 * fixed bcache size when bcache is off
 *
 * Revision 1.54  1998/01/13  14:59:01  gries
 * misc changes for pass2
 *
 * Revision 1.53  1998/01/07  17:42:57  gries
 * added perr max..
 *
 * Revision 1.52  1998/01/07  15:02:20  gries
 * added more pass2 stuff
 *
 * Revision 1.51  1997/12/29  19:28:16  gries
 * pass2 changes
 *
 * Revision 1.50  1997/11/19  19:46:51  gries
 * New dual version with fall back boot
 *
 * Revision 1.49  1997/10/31  21:46:19  fdh
 * Another problem grabbing the presence detects..
 *
 * Revision 1.48  1997/10/31  21:16:16  fdh
 * Corrected bug in reading presence detect jumpers.
 *
 * Revision 1.47  1997/10/31  20:33:08  fdh
 * Modified the memory detection to support all memory presence
 * detect combinations.  The banks are not yet sorted. Thus the
 * user must populate the biggest banks first.  None of these
 * combinations are programmed to support split bank DIMMs
 * either.  It was suggest that PD{4] might indicate when split
 * banks should be used???
 *
 * Revision 1.46  1997/10/30  10:38:40  fdh
 * Implemented a interprocessor communication protocol used to init
 * a multi-processor system.
 *
 * Modified init sequences to take advantage of the interprocessor
 * communication protocol in multi-processor systems.
 *
 * Flush caches and enable external probes prior to transferring
 * control to the next level of firmware.
 *
 * Revision 1.45  1997/10/09  19:32:12  fdh
 * Init write many chain to ensure no external activity it
 * will be changed when that bcache is swept.
 *
 * Enable and sweep bcache to clean tags even when jumpers are
 * set to disable the bcache.  If jumpers say to disable the bcache,
 * disable it after sweeping it.
 *
 * Revision 1.44  1997/09/25  22:06:47  fdh
 * Added jumper support to configure Bcache sizes 1MB, 2MB, and 4MB.
 * Modified the Bcache init process to acknowledge everything
 * internal to the CPU until the init process is completed.
 * Modified the layout of the cache configuration jumpers.
 * Added at least a 500us delay before measuring the CPU speed.
 * Added code to to some of the Cypress init for DMA etc.
 * It doesn't really belong here though.
 *
 * Revision 1.43  1997/09/23  07:26:26  fdh
 * Added back the call to srom_wrtest when writing STR and CSC.
 *
 * Revision 1.42  1997/09/22  18:14:12  fdh
 * Condifure Memory/Tsunami timing based on jumpers.
 * Configure  BCache based on jumpers. Currently only 2MB or disabled.
 *
 * Revision 1.41  1997/09/17  20:28:43  fdh
 * Disable debug interface on array 3.
 *
 * Revision 1.40  1997/09/16  19:51:27  fdh
 * zeroblk_enable_a	1 -> 3
 * set_dirty_enable_a	0 -> 6
 *
 * Revision 1.39  1997/09/16  16:11:35  fdh
 * Disable Probes for now.  Revisit this when the Multi-processor
 * stuff is added.
 *
 * Revision 1.38  1997/09/15  22:32:59  fdh
 * Autobaud, or not, last breakpoint based on DEBUG_BOARD conditional.
 *
 * Revision 1.37  1997/09/15  20:40:49  fdh
 * Added the DEBUG_BOARD conditional around a couple of breakpoints.
 *
 * Revision 1.36  1997/09/15  20:21:18  fdh
 * Config system based on jumpers and DRAM presence detect signals.
 * 	* Configure Memory based on presence detects.  However, only
 * 	  4MB X 72 DIMMs with 0xDA presence detects are supported in
 * 	  this version.
 * 	* Added normal jumpers, boot option, fail-safe booter, etc.
 * 	  Also added a temporary jumper to disable a Dcache set.
 *
 * Pass out parameters... i.e. standard one in r15-r21 and r1 containing DC_CTL.
 * 	* Autodetected CPU speed.
 * 	* Autosized memory size.
 *
 * Enable Probes after memory is initialized.
 *
 * Corrected use of the DEBUG_BOARD conditional.
 *
 * Revision 1.35  1997/09/09  20:18:18  fdh
 * Removed hardcoded firmware destination address.
 *
 * Revision 1.34  1997/08/26  19:00:10  fdh
 * Load ROM to 0x200000 and execute.
 * Disabled a couple of breakpoints...
 *
 * Revision 1.33  1997/08/24  20:52:30  fdh
 * Removed one of the breakpoints.
 *
 * Revision 1.32  1997/08/24  18:37:40  fdh
 * Start searching the flash from offset 0x20000.
 *
 * Revision 1.31  1997/08/24  17:44:52  fdh
 * Enable only one DCache set instead of both.
 * Init entire DCache for SROM mini-debugger storage.
 * Added the ISP_SIMULATION compile time conditional.
 * Added code to detect the CPU speed, sweep memory,
 * load image from flash, jump to next level.
 *
 * Revision 1.30  1997/08/04  11:01:48  fdh
 * WriteLed 0xab after tsunami_init.
 *
 * Revision 1.29  1997/07/31  19:56:29  fdh
 * Updated TTR setting.
 *
 * Revision 1.28  1997/07/31  19:21:20  fdh
 * Set 48 bit virtual mode.
 *
 * Revision 1.27  1997/07/31  19:16:08  fdh
 * Added DTB miss handling...
 * Corrected the TDR register initialization.
 *
 * Revision 1.26  1997/07/30  15:50:57  fdh
 * Re-enable AUTOBAUD before displaying anything.
 *
 * Revision 1.25  1997/07/30  03:28:35  fdh
 * Adjusted baud rate for ISP simulations.
 *
 * Revision 1.24  1997/07/30  01:24:49  fdh
 * Minor adjustment to baud rate calculation.
 *
 * Revision 1.23  1997/07/29  15:24:22  fdh
 * Init Pchip 1.
 *
 * Revision 1.22  1997/07/28  21:57:43  fdh
 * put the minidebugger at 0x2000.
 *
 * Revision 1.21  1997/07/28  21:56:32  fdh
 * Modified to use array 1 instead of array 0.
 * Corrected DRAM burst length.
 *
 * Revision 1.20  1997/07/26  21:27:32  fdh
 * Print system banner and RCS lock signature.
 *
 * Revision 1.19  1997/07/26  18:43:16  fdh
 * Modified srom_wrtest to echo data good and bad.
 * Now it indicates bad data with an X.
 *
 * Revision 1.18  1997/07/26  17:25:19  fdh
 * Removed a call to putString.
 *
 * Revision 1.17  1997/07/26  12:49:18  fdh
 * Modified to use swizzled Tsunami CSR deposit values.
 * Added mini-debugger breakpoint before tsunami init.
 * Display RCS version.
 * Write then read back CSR deposits and display that data.
 * Added additional labels.
 *
 * Revision 1.16  1997/07/24  19:38:56  fdh
 * Minor adjustment for insuring alignment.
 *
 * Revision 1.15  1997/07/24  18:08:23  fdh
 * Added back a missing label.
 *
 * Revision 1.14  1997/07/24  17:55:17  fdh
 * Added handlers to dump IPRs on PALcode exceptions.
 * Corrected an alignment problem setting up to load
 * the write_many chain.
 * Added code to init the PALbase in the event that this
 * code it not loaded at zero.
 * Enabled the banner print.
 *
 * Revision 1.13  1997/07/23  16:34:19  fdh
 * Removed an unnecessary conditional.
 *
 * Revision 1.12  1997/07/22  20:24:10  fdh
 * Disabled Banner print.
 *
 * Revision 1.11  1997/07/22  17:08:32  fdh
 * Enable loop on Print 'U'
 *
 * Revision 1.10  1997/07/22  13:52:21  fdh
 * Disabled branching on printig a 'U'
 *
 * Revision 1.9  1997/07/22  04:14:56  fdh
 * Moved the save state init to the power-up flow.
 * Changed .align 5 to .align 6.
 *
 * Revision 1.8  1997/07/21  21:01:45  fdh
 * Corrected baud rate calculation.
 *
 * Revision 1.7  1997/07/21  17:42:19  fdh
 * *** empty log message ***
 *
 * Revision 1.6  1997/07/18  12:57:06  fdh
 * More comment edits.
 *
 * Revision 1.5  1997/07/18  12:32:12  fdh
 * Comments!
 *
 * Revision 1.4  1997/07/18  10:38:04  fdh
 * Added comments from ejs.
 *
 * Revision 1.3  1997/07/18  04:05:02  fdh
 * Modified the write-many chain.
 * Removed the save-state init code.
 *
 * Revision 1.2  1997/07/17  17:00:18  fdh
 * Updated the refresh interval and Pchip control settings.
 * Also updated the delays required between some of the CSR
 * writes.
 *
 * Revision 1.1  1997/07/17  00:07:29  fdh
 * Initial revision
 *
 */

#include "dc21264.h"
#include "wm.h"
#include "tsunami.h"
#include "io.h"
#ifndef white
#include "testing.h"
#endif

#ifndef MAKEDEPEND
#include "rcsv.h"
#endif

#ifdef ISP_SIMULATION
#undef PRINTS8(lw_lo, lw_hi)
#undef PRINTS4(lw_lo)
#undef PRINTS2(c1, c2)
#undef PRINTL(reg)
#undef PRINTB(reg)
#undef PRINTC

#define PRINTS8(lw_lo, lw_hi)
#define PRINTS4(lw_lo)
#define PRINTS2(c1, c2)
#define PRINTL(reg)
#define PRINTB(reg)
#define PRINTC(c)
#endif /* ISP_SIMULATION */

#ifndef DEBUG_BOARD
#define ECHO_STARTUP
#endif


#if 0
#ifdef DEBUG_BOARD
#define DEBUG_SYNC
#endif
#endif

#define MAX_CPUS	2
#define CPU_MASK	((1<<MAX_CPUS)-1)

/* Sync Commands */
#define SYNC_CONTINUE	1
#define SYNC_DATA	2

#define SyncCPUSequence(Command, Data)		\
	LDLI(r9, (((Data)|(Command))>>32), r31)	; \
	sll	r9, 32, r9			; \
	LDLI(r8, ((Data)|(Command)), r31)	; \
	zap	r8, 0xf0, r8			; \
	bis	r8, r9, r8			; \
	bsr	r26, SyncronizeCPUs		;

#define SyncCPUData(Register, Data)		\
	bis	r31, Register, r16		; \
	SyncCPUSequence(SYNC_DATA, Data)	; \
	bis	r31, r16, Register		;


#ifdef EV6_PASS1
#define DC_CTL_INIT_K	0x1
#else
#define DC_CTL_INIT_K	0x3
#endif

EntryPoint:


	/* *DTBM3 */
PAL_EXCEPT(DTBM_DOUBLE_3,	EV6__DTBM_DOUBLE_3_ENTRY)
	pal_exception_prompt(a_star, a_D, a_T, a_B, a_M, a_3, a_cr, a_nl)

	/* *DTBM4 */
PAL_EXCEPT(DTBM_DOUBLE_4,	EV6__DTBM_DOUBLE_4_ENTRY)
	pal_exception_prompt(a_star, a_D, a_T, a_B, a_M, a_4, a_cr, a_nl)

	/* *FEN* */
PAL_EXCEPT(FEN,	EV6__FEN_ENTRY)
	pal_exception_prompt(a_star, a_F, a_E, a_N, a_star, a_nul, a_cr, a_nl)

	/* *UnAli */
PAL_EXCEPT(UNALIGN,	EV6__UNALIGN_ENTRY)
	pal_exception_prompt(a_star, a_U, a_n, a_A, a_l, a_i, a_cr, a_nl)

	/* *DTBM* */
PAL_EXCEPT(DTBM_SINGLE,	EV6__DTBM_SINGLE_ENTRY)
	pal_exception_prompt(a_star, a_D, a_T, a_B, a_M, a_star, a_cr, a_nl)
	mfpr	r0, EV6__EXC_ADDR		/* (0L,1) get exception address	\*
	mfpr	r1, EV6__VA			/* (4-7,1L,3) get original va	*/
	lda	r2, 0x3301(r31)			/* all r/w enable		*/
	srl	r1, 13, r2			/* shift out the byte offset	*/
	sll	r2, EV6__DTB_PTE0__PFN__S, r2	/* get pfn into position	*/
	bis	r2, r2, r2			/* produce the pte		*/

	mtpr	r1, EV6__DTB_TAG0		/* (2&6,0L) write tag0		*/
	mtpr	r1, EV6__DTB_TAG1		/* (1&5,1L) write tag1		*/
	mtpr	r2, EV6__DTB_PTE0		/* (0&4,0L) write pte0		*/
	mtpr	r2, EV6__DTB_PTE1		/* (3&7,1L) write pte1		*/
	hw_rets/jmp	(r0)			/* (0L) return			*/

	/* *DFlt* */
PAL_EXCEPT(DFAULT,	EV6__DFAULT_ENTRY)
	pal_exception_prompt(a_star, a_D, a_F, a_l, a_t, a_star, a_cr, a_nl)

	/* *OPDEC */
PAL_EXCEPT(OPCDEC,	EV6__OPCDEC_ENTRY)
	pal_exception_prompt(a_star, a_O, a_P, a_D, a_E, a_C, a_cr, a_nl)

	/* *AcVio */
PAL_EXCEPT(IACV,	EV6__IACV_ENTRY)
	pal_exception_prompt(a_star, a_A, a_c, a_V, a_i, a_o, a_cr, a_nl)

	/* *MCHK* */
PAL_EXCEPT(MCHK,	EV6__MCHK_ENTRY)
//	pal_exception_prompt(a_star, a_M, a_C, a_H, a_K, a_star, a_cr, a_nl)
	mb
	mb
	mb
	mb
	br	r31, FatalError
	mb
	mb
	mb

	/* *ITBM* */
PAL_EXCEPT(ITB_MISS,	EV6__ITB_MISS_ENTRY)
	pal_exception_prompt(a_star, a_I, a_T, a_B, a_M, a_star, a_cr, a_nl)

	/* *ARITH */
PAL_EXCEPT(ARITH,	EV6__ARITH_ENTRY)
	pal_exception_prompt(a_star, a_A, a_R, a_I, a_T, a_H, a_cr, a_nl)

	/* *INTR */
PAL_EXCEPT(INTERRUPT,	EV6__INTERRUPT_ENTRY)
	pal_exception_prompt(a_star, a_I, a_N, a_T, a_R, a_nul, a_cr, a_nl)

	/* *FPCR */
PAL_EXCEPT(MT_FPCR,	EV6__MT_FPCR_ENTRY)
	pal_exception_prompt(a_star, a_F, a_P, a_C, a_R, a_nul, a_cr, a_nl)


PAL_EXCEPT(RESET_ENTRY,	EV6__RESET_ENTRY)
/*
** INITIALIZE_RETIRATOR_AND_MAPPER
**
** EV6 exits hardware reset with FPE=1 and IC_EN=3.
** All other I_CTL bits are zero.
**
** Initialize 80 retirator "done" status bits and mapper.
** They must be initialized in the manner and order below.
** NO SOURCES other than x31 may be used until
** "mapper source enables" are turned on with a MTPR ITB_IA.
**
** On pass1, we map only integer registers. On later passes,
** we map integer and floating registers.
**
** The retirator and mapper must be initialized in the
** manner and order below. NO SOURCES other than
** x31 may be used until "mapper source enable" is
** turned on with a EV6_ITB_IA.
**
** (1) Initialize 80 retirator "done" status bits and
** the integer and floating mapper destinations.
** (2) Do A MTPR ITB_IA, which turns on the mapper source
** enables.
** (3) Create a map stall to complete the ITB_IA.
**
** State after execution of this code:
**	retirator initialized
**	destinations mapped
**	source mapping enabled
**	itb flushed
**
** The PALcode need not assume the following since the SROM is not
** required to do these:
**	dtb 		flushed
**	dtb_asn0 	0
**	dtb_asn1	0
**	dtb_alt_mode	0
*/

/*
** Initialize retirator and destination map, doing 80 retires.
*/
#ifdef EV6_PASS1
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

#else
	addq	r31,r31,r0		/* initialize Int. Reg. 0*/
	addq	r31,r31,r1		/* initialize Int. Reg. 1*/
	addt	f31,f31,f0		/* initialize F.P. Reg. 0*/
	mult	f31,f31,f1		/* initialize F.P. Reg. 1*/

	addq	r31,r31,r2		/* initialize Int. Reg. 2*/
	addq	r31,r31,r3		/* initialize Int. Reg. 3*/
	addt	f31,f31,f2		/* initialize F.P. Reg. 2*/
	mult	f31,f31,f3		/* initialize F.P. Reg. 3*/

	addq	r31,r31,r4		/* initialize Int. Reg. 4*/
	addq	r31,r31,r5		/* initialize Int. Reg. 5*/
	addt	f31,f31,f4		/* initialize F.P. Reg. 4*/
	mult	f31,f31,f5		/* initialize F.P. Reg. 5*/

	addq	r31,r31,r6		/* initialize Int. Reg. 6*/
	addq	r31,r31,r7		/* initialize Int. Reg. 7*/
	addt	f31,f31,f6		/* initialize F.P. Reg. 6*/
	mult	f31,f31,f7		/* initialize F.P. Reg. 7*/

	addq	r31,r31,r8		/* initialize Int. Reg. 8*/
	addq	r31,r31,r9		/* initialize Int. Reg. 9*/
	addt	f31,f31,f8		/* initialize F.P. Reg. 8*/
	mult	f31,f31,f9		/* initialize F.P. Reg. 9*/

	addq	r31,r31,r10		/* initialize Int. Reg. 10*/
	addq	r31,r31,r11		/* initialize Int. Reg. 11*/
	addt	f31,f31,f10		/* initialize F.P. Reg. 10*/
	mult	f31,f31,f11		/* initialize F.P. Reg. 11*/

	addq	r31,r31,r12		/* initialize Int. Reg. 12*/
	addq	r31,r31,r13		/* initialize Int. Reg. 13*/
	addt	f31,f31,f12		/* initialize F.P. Reg. 12*/
	mult	f31,f31,f13		/* initialize F.P. Reg. 13*/

	addq	r31,r31,r14		/* initialize Int. Reg. 14*/
	addq	r31,r31,r15		/* initialize Int. Reg. 15*/
	addt	f31,f31,f14		/* initialize F.P. Reg. 14*/
	mult	f31,f31,f15		/* initialize F.P. Reg. 15*/

	addq	r31,r31,r16		/* initialize Int. Reg. 16*/
	addq	r31,r31,r17		/* initialize Int. Reg. 17*/
	addt	f31,f31,f16		/* initialize F.P. Reg. 16*/
	mult	f31,f31,f17		/* initialize F.P. Reg. 17*/

	addq	r31,r31,r18		/* initialize Int. Reg. 18*/
	addq	r31,r31,r19		/* initialize Int. Reg. 19*/
	addt	f31,f31,f18		/* initialize F.P. Reg. 18*/
	mult	f31,f31,f19		/* initialize F.P. Reg. 19*/

	addq	r31,r31,r20		/* initialize Int. Reg. 20*/
	addq	r31,r31,r21		/* initialize Int. Reg. 21*/
	addt	f31,f31,f20		/* initialize F.P. Reg. 20*/
	mult	f31,f31,f21		/* initialize F.P. Reg. 21*/

	addq	r31,r31,r22		/* initialize Int. Reg. 22*/
	addq	r31,r31,r23		/* initialize Int. Reg. 23*/
	addt	f31,f31,f22		/* initialize F.P. Reg. 22*/
	mult	f31,f31,f23		/* initialize F.P. Reg. 23*/

	addq	r31,r31,r24		/* initialize Int. Reg. 24*/
	addq	r31,r31,r25		/* initialize Int. Reg. 25*/
	addt	f31,f31,f24		/* initialize F.P. Reg. 24*/
	mult	f31,f31,f25		/* initialize F.P. Reg. 25*/

	addq	r31,r31,r26		/* initialize Int. Reg. 26*/
	addq	r31,r31,r27		/* initialize Int. Reg. 27*/
	addt	f31,f31,f26		/* initialize F.P. Reg. 26*/
	mult	f31,f31,f27		/* initialize F.P. Reg. 27*/

	addq	r31,r31,r28		/* initialize Int. Reg. 28*/
	addq	r31,r31,r29		/* initialize Int. Reg. 29*/
	addt	f31,f31,f28		/* initialize F.P. Reg. 28*/
	mult	f31,f31,f29		/* initialize F.P. Reg. 29*/

	addq	r31,r31,r30		/* initialize Int. Reg. 30*/
	addt	f31,f31,f30		/* initialize F.P. Reg. 30*/
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

#endif
/* stop deleting*/

	mtpr r31,EV6__ITB_IA		/* flush the ITB (SCRBRD=4)  *** this also turns on mapper source enables ****/
	mtpr r31,EV6__DTB_IA		/* flush the DTB (SCRBRD=7)*/
	mtpr r31,EV6__VA_CTL		/* clear VA_CTL (SCRBRD=5)*/
	mtpr r31,EV6__M_CTL		/* clear M_CTL (SCRBRD=6)*/

/*
** Create a stall outside the IQ until the mtpr EV6__ITB_IA retires.
** We can use DTB_ASNx even though we don't seem to follow the restriction on
** scoreboard bits (4-7).It's okay because there are no real dstream operations
** happening.
*/
	mtpr r31,EV6__DTB_ASN0	/* clear DTB_ASN0 (SCRBRD=4) creates a map-stall under the above mtpr to SCRBRD=4*/
	mtpr r31,EV6__DTB_ASN1	/* clear DTB_ASN1 (SCRBRD=7)*/
	mtpr r31,EV6__CC_CTL		/* clear CC_CTL (SCRBRD=5)*/
	mtpr r31,EV6__DTB_ALT_MODE	/* clear DTB_ALT_MODE (SCRBRD=6)*/


/*
** MAP_SHADOW_REGISTERS
**
** The shadow registers are mapped. This code may be done by the SROM
** or the PALcode, but it must be done in the manner and order below.
**
** It assumes that the retirator has been initialized, that the
** non-shadow registers are mapped, and that mapper source enables are on.
**
** Source enables are on. For fault-reset and wake from sleep, we need to
** ensure we are in the icache so we don't fetch junk that touches the
** shadow sources before we write the destinations. For normal reset,
** we are already in the icache. However, so this macro is useful for
** all cases, force the code into the icache before doing the mapping.
**
** Assume for fault-reset, and wake from sleep case, the exc_addr is
** stored in r1.
*/
	addq	r31,r31,r0		/* nop*/
	addq	r31,r31,r0		/* nop*/
	addq	r31,r31,r0		/* nop*/
	br	r31, tch0		/* fetch in next block*/

	.align 3
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
nxt9:


/*
** INIT_WRITE_MANY
**
** Write the cbox write many chain, initializing the bcache configuration.
**
** This code is on a cache block boundary,
**
** *** the bcache is initialized OFF for the burnin test ***
*/

/*
** Because we aligned on and fit into a icache block, and because sbe=0,
** and because we do an mb at the beginning (which blocks further progress
** until the entire block has been fetched in), we don't have to
** fool with pulling this code in before executing it.
*/

#undef bc_enable_a
#undef init_mode_a
#undef bc_size_a
#undef zeroblk_enable_a
#undef enable_evict_a
#undef set_dirty_enable_a
#undef bc_bank_enable_a
#undef bc_wrt_sts_a

#define bc_enable_a             0
#define init_mode_a             0
#define bc_size_a               0
#define zeroblk_enable_a        1
#define enable_evict_a          0
#define set_dirty_enable_a      0
#define bc_bank_enable_a        0
#define bc_wrt_sts_a            0

loadwm:
	lda	r1, WRITE_MANY_CHAIN_H(r31)
	sll	r1, 32, r1		/* data<35:32> */
	LDLI(r1, WRITE_MANY_CHAIN_L, r1)/* data<31:00> */
	addq	r31,6,r0		/* shift in 6x 6-bits*/
	mb				/* wait for all istream/dstream to complete*/

	br	r31, bccshf
	.align 6
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
	mtpr 	r0,EV6__PROCESS_CONTEXT	/* ..... FPE=1 (SCRBRD=4)*/
	lda	r0,DC_CTL_INIT_K(r31)	/* load DC_CTL.....*/
	mtpr 	r0,EV6__DC_CTL		/* .....ECC_EN=0, FHIT=0, SET_EN=3 (SCRBRD=6)*/

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
#ifdef EV6_PASS1
	addq	r31,r31,r0		/* nop	itoft	r0,f0			; value = 0x8FF0000000000000*/

	addq	r31,r31,r0		/* nop	mt_fpcr	f0,f0,f0		; do the load*/
#else
	itoft   r0, f0			/* nop	itoft	r0,f0			; value = 0x8FF0000000000000*/

	mt_fpcr f0			/* nop	mt_fpcr	f0,f0,f0		; do the load*/
#endif
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

	lda    r0,0x780(r31)            /*this is new initialization stuff to prevent*/
#ifdef bad_day
	addq	r31,8,r1		/* initialize retirator 80*/
	mtpr r1,EV6__M_CTL		/* clear M_CTL (SCRBRD=6)*/
	lda	r1, 0x8000(r31)		/* Kseg = 0xffff8000.00000000	*/
	sll	r1, 32, r1		/* Shift into place		*/
	bis	r1, r0, r0
#endif
	mb
	whint   r0                       /* ld/st below from going off-chip */
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
#ifndef EV6_PASS1
	lda	r0,0x03FF(r31)		/* create FP one..... */
	sll	r0,52,r0		/* .....value = 0x3FF0000000000000 */
	itoft	r0,f0			/* put it into F0 reg */
	addq	r31,r31,r1		/* nop (also clears R1) */

	mult	f0,f0,f0		/* flush mul-pipe */
	addt	f0,f0,f0		/* flush add-pipe */
	divt	f0,f0,f0		/* flush div-pipe */
	sqrtt	f0,f0			/* flush div-pipe */

	cvtqt	f0,f0			/* flush add-pipe (integer logic) */
        perr    r31,r31,r0  	        /* flush MVI logic */
        maxuw4  r31,r31,r0              /* flush MVI logic */
        pkwb    r31,r0                  /* flush MVI logic */
/* INTNEW: add this for pass-3 schematics which enable the 3-cycle latency unit.*/
#else
	addq	r31,r31,r0
	addq	r31,r31,r0
	addq	r31,r31,r0
	addq	r31,r31,r0
#endif

	rc	r0			/* clear interrupt flag*/
	addq	r31,r31,r1		/* nop (also clears R1)*/
	addq	r31,r31,r1		/* nop (also clears R1)*/
	addq	r31,r31,r1		/* nop (also clears R1)*/


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


	bis	r31, 2, r0
	mtpr	r0, EV6__VA_CTL

	bis	r31, 8, r0
	mtpr	r0, EV6__M_CTL

	br	r0, jmp0
jmp0:	addq	r0, (jmp1-jmp0+1), r0
	hw_rets/jmp	(r0)
jmp1:

	lda	r1,  1(r31)		/* r1 <- cc_ctl enable bit 	*/
	sll	r1,  32, r1
	mtpr	r1, EV6__CC_CTL 	/* Enable/clear the cycle counter. */

/*
** Now initialize the dcache to allow the
** minidebugger so save gpr's
*/
#ifdef DDR_Cache
	LDLI(r16, 900000, r31)	/* Wait 9000 cycles		*/
	bsr	r27, wait_n_cycles 	/* Wait a while.		*/

#endif
sweep_dcache:
	lda	r0, 0x8000(r31)		/* Kseg = 0xffff8000.00000000	*/
	sll	r0, 32, r0		/* Shift into place		*/
#ifdef ISP_SIMULATION
	lda	r0, 0x200(r0)		/* r0 = 0xffff8000.00010000	*/
#else
	ldah	r0, 1(r0)		/* r0 = 0xffff8000.00010000	*/
#endif /* ISP_SIMULATION */
sweep_dcache0:
	subq	r0, 64, r0
	whint	r0
	zap	r0, 0xf0, r1
	bne	r1, sweep_dcache0
	mb

/*
!*************************************************************************
! MTR - Memory Timing Register        memory_timing_throttle  == 11
!				      10 [fastest] --9-8-7-6-5-4-3-2-1-- 0 [slowest]
!       fastest_ref_int_percent == 25%    1 = fastest
!       random_ref_int_percent  == 25%    1 thru 0xE random
!       typical_ref_int_percent == 25%    0xE = typical (14.4us)
!       slower_ref_int_percent  == 15%    0xE thru 0x2F  = slower
!       disable_ref_int_percent == 5%    0x0 = disabled
! MTR_MPH  = 0x00           ! [45:40]  R/W   Max Page Hits
! MTR_PHCW = 0x0F           ! [39:36]  R/W   Page hit Cycles for writes
! MTR_PHCR = 0x0F           ! [35:32]  R/W   Page hit Cycles for reads
! MTR_RI   = 0x04           ! [29:24]  R/W   Refresh Interval
! MTR_MPD  = 0x00           ! [20]     R/W   Mask Pipeline Delay
! MTR_RRD  = 0x01           ! [16]     R/W   Minimum Same-Array Different-Bank RAS-to-RAS delay.
! MTR_RPT  = 0x02           ! [13:12]  R/W   Minimum RAS Precharge Time
! MTR_RPW  = 0x03           ! [9:8]    R/W   Minimum RAS Pulse Width.
! MTR_IRD  = 0x02           ! [6:4]    R/W   Issue to RAS delay.
! MTR_CAT  = 0x01           ! [2]      R/W   CAS Access Time.
! MTR_RCD  = 0x01           ! [0]      R/W   RAS-To-CAS Delay.
!*************************************************************************
! SYSCONF - System Configuration Register
! SYSCONF_PBQMAX  = 0x01    ! [58:56] R/W  CPU probe queue max, 0 means 8 entries
! SYSCONF_PRQMAX  = 0x02    ! [54:52] R/W  Max requests to one pchips until ack, mod 8
! SYSCONF_PDTMAX  = 0x01    ! [50:48] R/W  Max data xfers to one pchip until ack, mod 8
! SYSCONF_FPQPMAX = 0x01    ! [46:44] R/W  Max entries in FPQ on Dchips, mod 8
! SYSCONF_FPQCMAX = 0x01    ! [42:40] R/W  True max entries in FQP on Dchips, mod 8
! SYSCONF_TPQMMAX = 0x01    ! [38:36] R/W  Max entres on TPQM on Dchips mod 8
! SYSCONF_B3D     = 0x01    ! [35]    R/W  Bypass 3 Disable
! SYSCONF_B2D     = 0x01    ! [34]    R/W  Bypass 2 Disable
! SYSCONF_B1D     = 0x01    ! [33]    R/W  Bypass 1 Disable
! SYSCONF_FTI     = 0x01    ! [32]    R/W  Force Throttle Issue
! SYSCONF_EFT     = 0x01    ! [31]    R/W  Extract to Fill Turnaround Cycles
! SYSCONF_QDI     = 0x00    ! [30:28] R/W  Queue Drain Interval
! SYSCONF_FET     = 0x02    ! [27:26] R/W  Fill to extract Turnaround Cycles
! SYSCONF_QPM     = 0x00    ! [25]    R/W  Queue Priority Mode 0-uni, 1-dual
! SYSCONF_PME     = 0x00    ! [24]    R/W  Page Mode Enable
! SYSCONF_DRTP    = 0x03    ! [21:20] R/W  Minimum delay through Dchip from Memory bus to PADbus
! SYSCONF_DWFP    = 0x03    ! [19:18] R/W  Minimum delay through Dchip from Padbus to CPU or Memory bus
! SYSCONF_DWTP    = 0x03    ! [17:16] R/W  Minimun delay through Dchip from CPU bus to PAD bus
! SYSCONF_P1P     = 0x00    ! [14]    RO   Pchip 1 Present
! SYSCONF_IDDW    = 0x02    ! [13:12] RO   Issue to Data Delay for memory Writes
! SYSCONF_IDDR    = 0x03    ! [11:9]  RO   Issue to Data Delay for memory Reads
! SYSCONF_AW      = 0x01    ! [8]     RO   Array Width
! SYSCONF_FW      = 0x00    ! [7]     RO   Available for firmware
! SYSCONF_SFD     = 0x00    ! [6]     RO   SysDC fill delay
! SYSCONF_SED     = 0x02    ! [5:4]   RO   SysDC Extract Delay. The number of cycles from the SysDC
! SYSCONF_C1CFP   = 0x00    ! [3]     RO   CPU 1 Clock Forward Preset
! SYSCONF_C0CFP   = 0x00    ! [2]     RO   CPU 0 Clock Forward Preset
! SYSCONF_BC      = 0x03    ! [1:0]   RO   Base Configuration
!*************************************************************************
! AAR0 - Array Address Register
! AARx_ADDR[0] = 0x00000000    ! [31:24]  R/W  Base Address.
! AARx_ADDR[0] = 0x00000000    ! [34:24]  R/W  Base Address.
! AARx_DBG[0]  = 0x00     ! [16]     R/Q  Debug.
! AARx_ASIZ[0] = 0x04    ! [14:12]  R/W  Array Size  128MB
! AARx_SA[0]   = 0x00    ! [8]      R/W  Split Array.
! AARx_ROWS[0] = 0x00    ! [1:0]    R/W  Number of Row Address bits in the DRAMs
                                   !               Bank=2 Number of Bank bits in the DRAMs
                                   !               Row=11 Number of Row Address bits in the DRAMs
!*************************************************************************
! AAR3 - Array Address Register
! AARx_ADDR[3] = 0x00000000    ! [31:24]  R/W  Base Address.
! AARx_ADDR[3] = 0x00000000    ! [34:24]  R/W  Base Address.
! AARx_DBG[3]  = 0x01	       ! [16]     R/Q  Debug.
! AARx_ASIZ[3] = 0x00    ! [14:12]  R/W  Array Size  128MB
! AARx_SA[3]   = 0x00    ! [8]      R/W  Split Array.
! AARx_ROWS[3] = 0x00    ! [1:0]    R/W  Number of Row Address bits in the DRAMs
                                   !               Bank=2 Number of Bank bits in the DRAMs
                                   !               Row=11 Number of Row Address bits in the DRAMs
!*************************************************************************
! MPR0 - Memory Programming Registers
!*************************************************************************
! TTR - TIG-bus timing register
! TTR_ID  = 0x07  ! [14:12] R/W  Interrupt Starting Device
! TTR_IRT = 0x03  ! [9:8]   R/W  Interrupt Read Time
! TTR_IS  = 0x03  ! [5:4]   R/W  Interrupt Setup Time
! TTR_AH  = 0x01  ! [1]     R/W    Address Hold after AS_L before CS_L
! TTR_AS  = 0x01  ! [0]     R/W    Address Setup to the address latch before AS_L
!*************************************************************************
! TDR - TIG-bus timing register
! TDR_WH3 = 0x00 ! [63]     R/W   Write hold time
! TDR_WP3 = 0x00 ! [62:60]  R/W   Write pulse width
! TDR_WS3 = 0x00 ! [57:56]  R/W   Write setup time
! TDR_RD3 = 0x00 ! [54:52]  R/W   Read output disable time
! TDR_RA3 = 0x00 ! [51:48]  R/W   Read access time
! TDR_WH2 = 0x00 ! [47]     R/W   Write hold time
! TDR_WP2 = 0x00 ! [46:44]  R/W   Write pulse width
! TDR_WS2 = 0x00 ! [41:40]  R/W   Write setup time
! TDR_RD2 = 0x00 ! [38:36]  R/W   Read output disable time
! TDR_RA2 = 0x00 ! [35:32]  R/W   Read access time
! TDR_WH1 = 0x00 ! [31]     R/W   Write hold time
! TDR_WP1 = 0x00 ! [30:28]  R/W   Write pulse width
! TDR_WS1 = 0x00 ! [25:24]  R/W   Write setup time
! TDR_RD1 = 0x00 ! [22:20]  R/W   Read output disable time
! TDR_RA1 = 0x00 ! [19:16]  R/W   Read access time
! TDR_WH0 = 0x01 ! [15]     R/W   Write hold time
! TDR_WP0 = 0x07 ! [14:12]  R/W   Write pulse width
! TDR_WS0 = 0x03 ! [9:8]    R/W   Write setup time
! TDR_RD0 = 0x07 ! [6:4]    R/W   Read output disable time
! TDR_RA0 = 0x0F ! [3:0]    R/W   Read access time
!*************************************************************************
! SYSCONF - Dchip System Configuration Register
! DSYSCONF_P1P   = 0x00   ! [6]    RO  Pchip 1 Present
! DSYSCONF_C3CFP = 0x00   ! [5]    RO   CPU 3 Clock Forward Preset
! DSYSCONF_C2CFP = 0x00   ! [4]    RO   CPU 2 Clock Forward Preset
! DSYSCONF_C1CFP = 0x00   ! [3]    RO   CPU 1 Clock Forward Preset
! DSYSCONF_C0CFP = 0x00   ! [2]    RO   CPU 0 Clock Forward Preset
! DSYSCONF_BC    = 0x03   ! [1:0]  RO  Base Configuration
!*************************************************************************
! STR - System Timing Register
! STR_IDDW = 0x02  ! [5:4]  R/W  Issue to Data Delay for memory Write
! STR_IDDR = 0x03  ! [3:1]  R/W  Issue to Data Delay for memory Reads
! STR_AW   = 0x01  ! [0]    R/W  Array Width
!*************************************************************************
! PCTL0 - Pchip Control
! PCTLx_PID[0]      = 0x00  ! [47:46] RO   Pchip Id
! PCTLx_RPP[0]      = 0x00  ! [45]    RO   Remote PChip Present
! PCTLx_PTEVRFY[0]  = 0x00  ! [44]    R/W  PTE Verify for DMA Read
! PCTLx_FDWDIS[0]   = 0x00  ! [43]    R/W  Fast DMA Read cache blk Wrap request disable
! PCTLx_FDSDIS[0]   = 0x00  ! [42]    R/W  Fast DMA Start and SGTE request disable
!*PCTLx_PCLKX[0]    = 0x02  ! [41:40] RO   PCI Clock Frequency multiplier
!*PCTLx_PTPMAX[0]   = 0x02  ! [39:36] R/W  max ptp requests to Cchip from both Pchips
!*PCTLx_CRQMAX[0]   = 0x01  ! [35:32] R/W  max number of data requests
! PCTLx_REV[0]      = 0x01  ! [31:24] R/W  Revision
!*PCTLx_CDQMAX[0]   = 0x01  ! [23:20] R/W  max number of data transfers
!*PCTLx_PADMODE[0]  = 0x00  ! [19]    RO   ECC Checking 0=8bit 1=4bit
! PCTLx_ECCEN[0]    = 0x00  ! [18]    R/W  ECC Enable disabled=0 enabled=1
! PCTLx_PRIGRP[0]   = 0x00  ! [14:8]  R/W  Priority group, one bit per PCI slot
!*PCTLx_ARBENA[0]   = 0x01  ! [7]     R/W  arbiter enable
! PCTLx_MWIN[0]     = 0x00  ! [6]     R/W  monster window enable
! PCTLx_PCHOLE[0]   = 0x00  ! [5]     R/W  PC hole window enable
! PCTLx_TGTLAT[0]   = 0x00  ! [4]     R/W  target latency timers enable
! PCTLx_CHAINDIS[0] = 0x00  ! [3]     R/W  disable chaining
! PCTLx_THDIS[0]    = 0x00  ! [2]     R/W  disable tlb thrash protection mode
! PCTLx_FBTB[0]     = 0x00  ! [1]     R/W  fast back to back enable
! PCTLx_FDSC[0]     = 0x00  ! [0]     R/W  fast discard enable
!*************************************************************************

0x801B0000840 -data 0x2727272727272727		STR

	< wait 20 sysclks - (20 * 8)>

0x801A0000000 -data 0x0121111F883F0000 		CSC

	< wait 20 sysclks - (20 * 8)>

0x801A0000040 -data 0x000000FF04012325 		MTR     < turn on refreshes >

	< wait for 8 auto-refreshes >		MAX_RI * 64 sysclks = 64*64 = 4096 sysclks

0x801A0000040 -data 0x000000FF00012325 		MTR     < turn off refreshes >
0x801A0000400 -data 0x0000000000000039 		MPR0
0x801A0000040 -data 0x000000FF04012325 		MTR     < turn on refreshes >
0x801A0000100 -data 0x0000000000004000 		AAR0
0x801A0000140 -data 0x0000000051000000 		AAR1
0x801A00001C0 -data 0x0000000000010000          AAR3
0x801A0000580 -data 0x0000000000002103          TTR
0x801A00005C0 -data 0xF37FF37FF37FF37F          TDR
0x80180000300 -data 0x0000002101100080 		PCTL0   < to turn on ARBEN >
*/

/*
**	*** Byte swizzle ***
**	b7 b6 b5 b4 b3 b2 b1 b0 =>
**	b7 b3 b6 b2 b5 b1 b4 b0
**
*/

#define STR_INIT0_K	0x22
#ifdef slow_memory
#define STR_INIT1_K	0x26
#else
#define STR_INIT1_K	0x24
#endif

#ifdef Brick
#define CSC_INIT_H_K	0x00424440
#else
#define CSC_INIT_H_K	0x00424400
#endif
//#define CSC_INIT_H_K	0x0142400f
//#define CSC_INIT_L_K	0x15190000
//#define CSC_INIT_L_K	0x15010000
#ifdef Brick
#define CSC_INIT_L_K	0x14150000
//#define CSC_INIT_L_K	0x152a0000
#else
//#define CSC_INIT_L_K	0x15050000
#define CSC_INIT_L_K	0x15150000
#endif
#ifdef Brick
#define MTR_INIT0_H_K	0x00002f74
#else
#define MTR_INIT0_H_K	0x00002f63
#endif
#define MTR_INIT0_L_K	0x14000130
#ifdef Brick
#define MTR_INIT1_H_K	0x00002f85
#else
#define MTR_INIT1_H_K	0x00002f63
#endif
#ifdef slow_memory
#define MTR_INIT1_L_K	0x14001325
#else
#define MTR_INIT1_L_K	0x14000234
#endif

#define MTR_REF_MASK_H	0x00000000
#define MTR_REF_MASK_L	0x3F000000

#define MPR0_INIT_L_K	0x29

#define TTR_INIT_H_K	0x00000000
#ifdef Brick
//#define HARD_SPEED 0x873
#define TTR_INIT_L_K	0x00003103
#else
#define TTR_INIT_L_K	0x00002103
#endif

#define TDR_INIT_H_K	0xF37FF37F
#define TDR_INIT_L_K	0xF37FF37F

#define PCTL0_INIT_H_K	0x00001044
#define PCTL0_INIT_L_K	0x00440081
#define PCTL0_INIT_H_K_PC	0x00001044
#define PCTL0_INIT_L_K_PC	0x00400081

#define PCTL1_INIT_H_K	0x00001044
#define PCTL1_INIT_L_K	0x00440081

#define MAX_SYS_CLOCK_RATIO	24

#ifdef ISP_SIMULATION
#define BAUD	((300*1000*1000)/0x8e)	/* 1 cycle per bit */
#else
#define BAUD	9600
#endif

/*
 * TIG Bus Address Map [23:21]
 */

#ifdef Brick
#define Flash_ROM       0       /* 801.0000.0000 */
#define MB_CONFIG	5       /* 801.2800.0000 */
#define CPU0_CONFIG     6       /* 801.3000.0000 */
#else
#define	Flash_ROM	0	/* 801.0000.0000 */
#define	Array0_PD	1	/* 801.0800.0000 */
#define	Array1_PD	2	/* 801.1000.0000 */
#define	Array2_PD	3	/* 801.1800.0000 */
#define	Array3_PD	4	/* 801.2000.0000 */
#define	MB_CONFIG	5	/* 801.2800.0000 */
#define	PC264_MB_CONFIG	1	/* 801.0800.0000 */
#define	CPU0_CONFIG	6	/* 801.3000.0000 */
#define	CPU1_CONFIG	7	/* 801.3800.0000 */
#endif
/*
 * DP264 JUMPER CONFIGURATION IN R30
 *
 * |3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1
 * |1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | | | |   |     | |     | |   | | | | | |               | |     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | |    |    |   |   |   |  |  |                |        |   |
 *  | |    |    |   |   |   |  |  |                |        |   +---    CPU ID
 *  | |    |    |   |   |   |  |  |                |        |
 *  | |    |    |   |   |   |  |  |                |        +-------    Primary CPU Flag
 *  | |    |    |   |   |   |  |  |                |
 *  | |    |    |   |   |   |  |  |                +----------------    Active Processor Mask
 *  | |    |    |   |   |   |  |  |
 *  | |    |    |   |   |   |  |  |                                     Mother Board Configuration Jumpers  801.2800.0000
 *  | |    |    |   |   |   |  |  |                                     =================================================
 *  | |    |    |   |   |   |  |  +---------------------------------    con_bit[0]   Boot option        (FSB)
 *  | |    |    |   |   |   |  |                                                        Out:    Boot second image (default)
 *  | |    |    |   |   |   |  |                                                        In:     Boot first image
 *  | |    |    |   |   |   |  |
 *  | |    |    |   |   |   |  +------------------------------------    con_bit[2:1] Memory Timing      (CAC_B  CAC_A)
 *  | |    |    |   |   |   |                                                           In  In:  2-2 mem timing (fastest)
 *  | |    |    |   |   |   |                                                                      STR:    2323 2323 2323 2323
 *  | |    |    |   |   |   |                                                                      MTR:    0000 0463 1100 0130
 *  | |    |    |   |   |   |                                                           In  Out: 2-3 mem timing w/16 page hits
 *  | |    |    |   |   |   |                                                                      STR:    2525 2525 2525 2525
 *  | |    |    |   |   |   |                                                                      MTR:    0000 1674 1300 0234
 *  | |    |    |   |   |   |                                                           Out In:  2-3 mem timing
 *  | |    |    |   |   |   |                                                                      STR:    2525 2525 2525 2525
 *  | |    |    |   |   |   |                                                                      MTR:    0000 0474 1300 0234
 *  | |    |    |   |   |   |                                                           Out Out: 3-3 mem timing (slowest)
 *  | |    |    |   |   |   |                                                                      STR:    2727 2727 2727 2727
 *  | |    |    |   |   |   |                                                                      MTR:    0000 0475 1900 1325
 *  | |    |    |   |   |   |
 *  | |    |    |   |   |   +--------------------------------------     con_bit[3]   SROM mini_debugger (MINI)
 *  | |    |    |   |   |                                                               Out:    Disable mini debugger (default)
 *  | |    |    |   |   |                                                               In:     Enable SROM mini debugger
 *  | |    |    |   |   |
 *  | |    |    |   |   +------------------------------------------     con_bit[6:4] Tsunami speed   See System Spec...
 *  | |    |    |   |
 *  | |    |    |   +----------------------------------------------     con_bit[7]   DBM Port           (T_MEM)
 *  | |    |    |                                                                    Debug Monitor Communications Port
 *  | |    |    |                                                                       In:     SROM Serial Port
 *  | |    |    |                                                                       Out:    COM1/Keyboard
 *  | |    |    |
 *  | |    |    |                                                       CPU 0 Configuration Jumpers             801.3000.0000
 *  | |    |    |                                                       CPU 1 Configuration Jumpers             801.3800.0000
 *  | |    |    |                                                       =====================================================
 *  | |    |    +--------------------------------------------------     cpu[2:0]     cpu1_speed See System Spec...
 *  | |    |
 *  | |    +-------------------------------------------------------     cpu[4:3]     Bcache_config      (BC_CNFG[1:0])
 *  | |                                                                                              In  In:  disabled
 *  | |                                                                                              In  Out: 1 MB Bcache
 *  | |                                                                                              Out In:  2 MB Bcache
 *  | |                                                                                              Out Out: 4 MB Bcache
 *  | |
 *  | |                                                                 cpu[5]       Unused             (BC_CNFG[2])
 *  | |
 *  | +------------------------------------------------------------     cpu[6]       Dcache_config      (BC_CNFG[3])
 *  |                                                                                   In:     Enable 2 Dcache sets
 *  |                                                                                   Out:    Enable 1 Dcache set
 *  |
 *  +--------------------------------------------------------------     cpu[7]          cpu_present     0 - CPU present
 *                                                                                                      1 - CPU not present
 *
 * |6 6 6 6|5 5 5 5|5 5 5 5|5 5 4 4|4 4 4 4|4 4 4 4|3 3 3 3|3 3 3 3|
 * |3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |               |               |               |               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *         |               |               |               |            See Jedec spec for 200 pin SDRAM DIMMS
 *         |               |               |               |            ======================================
 *         |               |               |               +--------    array0_pd[7:0]  801.0800.0000
 *         |               |               +------------------------    array1_pd[7:0]  801.1000.0000
 *         |               +----------------------------------------    array2_pd[7:0]  801.1800.0000
 *         +-------------------------------------------------------     array3_pd[7:0]  801.2000.0000
 */
#define JMP_V_CPU_ID		0
#define CPU_ID_MASK		3	/* Tsunami Spec. */
#define JMP_V_PRIMARY		3
#define JMP_V_PRESENT		4
#define JMP_MB_INDEX		16
#define JMP_V_ALT_BOOT 		(0+JMP_MB_INDEX)	/* Boot alternate image jumper	*/
#define JMP_V_MEM_CONFIG	(1+JMP_MB_INDEX)	/* Memory timing		*/
#define JMP_V_MINIDBGR		(3+JMP_MB_INDEX)	/* Trap to minidbgr jumper.	*/

/*
 * CPU Configuration Jumpers
 */
#define JMP_CPU_INDEX		(JMP_MB_INDEX+8)
#define JMP_V_CACHE_SIZE	(3+JMP_CPU_INDEX)
#define JMP_V_DisableDCACHE	(6+JMP_CPU_INDEX)	/* Out:	Both Sets, In: One Set	*/

/*
 * Memory array Presence Detect
 */
#define JMP_Arr0_INDEX	(JMP_CPU_INDEX+(8*1))
#define JMP_Arr1_INDEX	(JMP_CPU_INDEX+(8*2))
#define JMP_Arr2_INDEX	(JMP_CPU_INDEX+(8*3))
#define JMP_Arr3_INDEX	(JMP_CPU_INDEX+(8*4))

init_tsunami:
#ifdef ISP_SIMULATION
	jump_to_minidebugger()		/* trap to Mini-Debugger.	*/
#endif

#ifdef DEBUG_BOARD
autobaud:
	jump_to_minidebugger()

printSystem:
	/* Print Banner */
	PRINTS8(b4_to_l(a_cr,a_cr,a_nl,a_D), b4_to_l(a_P,a_2,a_6,a_4))

PrintVersionID:				/* Keep version in sync with RCS id */
	PRINTS4(b4_to_l(a_period, a_period, a_period, (RCS_ENCODED_REV_H>>(8*3))))
	LDLI(r17, RCS_ENCODED_REV_L, r31)
	bis	r31, r31, r16
#ifdef Save_Memory
	bis	r16, 1, r16
#endif
#ifdef DEBUG_BOARD
	bis	r16, 2, r16
#endif
#ifdef LCD
	bis	r16, 4, r16
#endif
#ifdef DDR_Cache
	bis	r16, 8, r16
#endif
#ifdef Brick
	bis	r16, 16, r16
#endif
#ifdef new_Arbiter_bit
	bis	r16, 32, r16
#endif
#ifdef BillClinton
	bis	r16, 64, r16
#endif
#ifdef Acer
	bis	r16, 128, r16
#endif
	sll	r16, 16, r16
	bis	r17, r16, r17
	PRINTL(r17)
#endif /* DEBUG_BOARD */

/*
 *	Establish primary CPU and determine which
 *	processors are present.
 */
	load_csr_base(r9, CCHIP_CSR)	/* Get Cchip base		*/
	lda	r9, CSR_MISC(r9)	/* Add in MISC offset		*/
	mb
	ldq_p	r30, 0(r9)		/* Read Processor ID		*/
	and	r30, CPU_ID_MASK, r30	/* Zap other bits		*/
	sll	r30, 9, r24		/* save register pointer in mimidbg */
#ifdef DEBUG_Single_CPU
        bis     r30, (1<<JMP_V_PRIMARY), r30    /* Primary cpu flag     */
	bis	r31, 1, r6
	sll	r6, JMP_V_PRESENT, r25	/* Shift up Processor mask	*/
	bis	r30, r25, r30		/* OR it into global register	*/
#else
	/*
	** Set appropriate bit in the Arbitration fields.
	*/
	lda	r6, 1(r31)		/* r6 <- 1			*/
	sll	r6, r30, r6		/* Shift into CPU mask bit	*/
	sll	r6, 16, r6		/* Shift up to Arbitration Won	*/
	sll	r6, 4, r7		/* Sign in via Arbitration Try	*/
	bis	r7, r6, r7		/* OR them together		*/
	stq_p	r7, 0(r9)		/* Write to the MISC register	*/
	mb

	/*
	** Find out if the current CPU won arbitration.
	*/
	ldq_p	r7, 0(r9)		/* Read the MISC register	*/
	and	r7, r6, r7		/* Mask off Arbitration Try bit	*/
	beq	r7, NotPrimary		/* Didn't win? Then branch	*/
	bis	r30, (1<<JMP_V_PRIMARY), r30	/* Primary cpu flag	*/

	/*
	** Determine what processors are present.  Three second
	** timeout if all possible CPUs haven't signed in.
	*/
	lda	r7, 30(r31)		/* Timeout = 3 seconds.		*/
WhoIsPresent:
	subq	r7, 1, r7
	LDLI(r16, 30000000, r31)	/* Wait 0.1 seconds		*/
	bsr	r27, wait_n_cycles 	/* Wait a while.		*/

	ldq_p	r6, 0(r9)		/* Read MISC			*/
	srl	r6, 20, r6		/* Shift down Arbitration Try	*/
	and	r6, CPU_MASK, r6	/* Mask bits.			*/

#ifdef DEBUG_SYNC
	PRINTC(a_percent)
	PRINTB(r6)			/* Print Arbitration Try field	*/
	PRINTB(r7)			/* Print Timeout counter	*/
#endif /* DEBUG_SYNC */

	beq	r7, CPUTimedOut		/* Was there a timeout yet?	*/
	cmpeq	r6, CPU_MASK, r5	/* All possible CPUs signed in?	*/
	beq	r5, WhoIsPresent	/* Loop until done		*/
CPUTimedOut:
	sll	r6, JMP_V_PRESENT, r25	/* Shift up Processor mask	*/
	bis	r30, r25, r30		/* OR it into global register	*/


NotPrimary:

	SyncCPUData(r25, 0x19)

#endif
First_InitBcache:


/*
 *	Convert R7 to Bcache size.
 *	R7	size
 *	==	========
 *	5	16MB
 */

#undef bc_enable_a
#undef zeroblk_enable_a
#undef set_dirty_enable_a
#undef init_mode_a
#undef enable_evict_a
#undef bc_wrt_sts_a
#undef bc_bank_enable_a

#define bc_enable_a             1
#define zeroblk_enable_a        1
#define set_dirty_enable_a      0
#define init_mode_a             1
#define enable_evict_a          0
#define bc_wrt_sts_a            0
#define bc_bank_enable_a        0

#undef bc_size_a
#define bc_size_a               15


	LDLI(r2, WRITE_MANY_CHAIN_H, r31)
	sll	r2, 32, r2

	LDLI(r14, WRITE_MANY_CHAIN_L, r31)
	zap	r14, 0xf0, r14
	bis	r14, r2, r14

	LDLI(r2, BC_ENABLE_I_MODE_MASK, r31)
	bic	r14, r2, r14

PrintChain:
	PRINTC(a_period)
	srl	r14, 32, r17
	PRINTB(r17)
	bis	r14, r31, r17
	PRINTL(r17)

	addq	r31, 6, r0		/* shift in 6x 6-bits		*/
	bis	r14, r31, r12		/* Make copy in R12		*/
	mb				/* wait for all istream/dstream to complete*/

	br	r31, BCshf
	.align 6
BCshf:	mtpr	r12, EV6__DATA		/* shift in 6 bits		*/
	subq	r0, 1, r0		/* decrement R0			*/
	beq	r0, BCend		/* done if R0 is zero		*/
        srl	r12, 6, r12		/* align next 6 bits		*/

	br	r31, BCshf		/* continue shifting		*/
BCend:	mtpr	r31, EV6__EXC_ADDR + 16	/* dummy IPR write - sets SCBD bit 4  */
	addq	r31, r31, r0		/* nop				*/
	addq	r31, r31, r12		/* nop				*/

	mtpr	r31, EV6__EXC_ADDR + 16	/* also a dummy IPR write - stalls until above write retires*/
	beq	r31, BCnxt		/* predicts fall through in PALmode*/
	br	r31, .-4		/* fools ibox predictor into infinite loop*/
	addq	r31, r31, r12		/* nop*/
BCnxt:
	mb				/* wait for all istream/dstream to complete*/

	bis	r31, 16, r8		/* Sweep 16MB Bcache 	*/

sweep_bcache:
	ldah	r0, 0x8000(r31)		/* Kseg = 0xffff8000.00000000	*/
	sll	r0, 16, r0		/* Shift into place		*/
	sll	r8, (20), r1		/* r1 = 2 * BCache size (bytes)	*/
	bis	r0, r1, r0

	br	r31, sweep_bcache0
	.align 6
sweep_bcache0:
	subq	r0, 64, r0
	whint	r0
	mb				/* wait for all istream/dstream to complete*/
	zap	r0, 0xf0, r1
	srl	r1, 4, r1
	mb
	bne	r1, sweep_bcache0
	mb				/* wait for all istream/dstream to complete*/

#undef bc_enable_a
#undef init_mode_a
#undef bc_size_a
#undef zeroblk_enable_a
#undef enable_evict_a
#undef set_dirty_enable_a
#undef bc_bank_enable_a
#undef bc_wrt_sts_a

#define bc_enable_a             0
#define init_mode_a             0
#define bc_size_a               0
#define zeroblk_enable_a        1
#define enable_evict_a          0
#define set_dirty_enable_a      0
#define bc_bank_enable_a        0
#define bc_wrt_sts_a            0

load_turn_off_bcache:
	lda	r1, WRITE_MANY_CHAIN_H(r31)
	sll	r1, 32, r1		/* data<35:32> */
	LDLI(r12, WRITE_MANY_CHAIN_L, r1)/* data<31:00> */
	addq	r31, 6, r0		/* shift in 6x 6-bits		*/
	mb				/* wait for all istream/dstream to complete*/

BC_again_shf:	mtpr	r12, EV6__DATA		/* shift in 6 bits		*/
	subq	r0, 1, r0		/* decrement R0			*/
	beq	r0, BC_again_end		/* done if R0 is zero		*/
        srl	r12, 6, r12		/* align next 6 bits		*/

	br	r31, BC_again_shf		/* continue shifting		*/
BC_again_end:	mtpr	r31, EV6__EXC_ADDR + 16	/* dummy IPR write - sets SCBD bit 4  */
	addq	r31, r31, r0		/* nop				*/
	addq	r31, r31, r12		/* nop				*/

	mtpr	r31, EV6__EXC_ADDR + 16	/* also a dummy IPR write - stalls until above write retires*/
	beq	r31, BC_again_nxt		/* predicts fall through in PALmode*/
	br	r31, .-4		/* fools ibox predictor into infinite loop*/
	addq	r31, r31, r12		/* nop*/
BC_again_nxt:
	mb				/* wait for all istream/dstream to complete*/

	srl	r30, JMP_V_PRIMARY, r1
	blbc	r1, ReadJumpers

dep_CSC:
#ifdef DEBUG_Single_CPU
	LDLI(r17, CSC_INIT_H_K_PC, r31)
#else
	LDLI(r17, CSC_INIT_H_K, r31)
#endif
	sll	r17, 32, r17
	LDLI(r16, CSC_INIT_L_K, r31)
	zap	r16, 0xf0, r16
	bis	r17, r16, r17
	load_csr_base(r16, CCHIP_CSR)
	lda	r16, CSR_CSC(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/
	bis	r31, r18, r14		// Cchip Configuratio
#ifdef test_pc264
	bic	r14, 1, r14		// hack for test 16 byte memory
//	lda	r16, 0x4000(r31)	// Pchip 1 Present
	bic	r14, r16, r14		// hach for test only one pchip
#endif
/*
 *      ...
 *      ...
 * Wait 20 sysclocks.
 *      ...
 *      ...
 */
	LDLI(r16, (20*MAX_SYS_CLOCK_RATIO), r31)	/* load cycles into R16.	*/
	bsr	r27, wait_n_cycles 	/* Wait a while.		*/



dep_STR:
	lda r17, STR_INIT1_K(r31)
	and	r14, 1, r16		// Cchip Configuration array width
	bis	r17, r16, r17		// dchip width
	xor	r16, 1, r16
	s4addq	r16, r17, r17
	sll	r17, 8, r16
	bis	r17, r16, r17
	sll	r17, 16, r16
	bis	r17, r16, r17
	sll	r17, 32, r16
	bis	r17, r16, r17
	load_csr_base(r16, DCHIP_CSR)
	lda	r16, CSR_STR(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/


/*
 *      ...
 *      ...
 * Wait 20 sysclocks.
 *      ...
 *      ...
 */
	LDLI(r16, (20*MAX_SYS_CLOCK_RATIO), r31)	/* load cycles into R16.	*/
	bsr	r27, wait_n_cycles 	/* Wait a while.		*/




dep_PCTL0:
#ifdef DEBUG_Single_CPU
	LDLI(r17, PCTL0_INIT_H_K_PC, r31)
	sll	r17, 32, r17
	LDLI(r17, PCTL0_INIT_L_K_PC, r17)
#else
	LDLI(r17, PCTL0_INIT_H_K, r31)
	sll	r17, 32, r17
	LDLI(r17, PCTL0_INIT_L_K, r17)
#endif
#ifdef REVD
	bic	r17, 0x80, r17
#ifdef new_Arbiter_bit
	and	r14, 0x80, r16
	srl	r16, 7, r16
	load_csr_base(r18, SYNCHRONIZE_CPU) // Get srm hack base
	ldah	r18, 0x0800(r18)	// 801.3800.0000
        stl_p	r16, Arbiter_bit(r18)   // Add in Arbiter_bit offset 
#endif
#endif
	load_csr_base(r16, PCHIP0_CSR)
	lda	r16, PCHIP0_PCTL(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/
	srl	r14, 14, r0		// check for Pchip 1
	blbc	r0, dep_TTR		// skip if no Pchip 1

dep_PCTL1:
	LDLI(r17, PCTL1_INIT_H_K, r31)
	sll	r17, 32, r17
	LDLI(r17, PCTL1_INIT_L_K, r17)
	load_csr_base(r16, PCHIP1_CSR)
	lda	r16, PCHIP1_PCTL(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/


dep_TTR:
	LDLI(r17, TTR_INIT_H_K, r31)
	sll	r17, 32, r17
	LDLI(r17, TTR_INIT_L_K, r17)
	load_csr_base(r16, CCHIP_CSR)
	lda	r16, CSR_TTR(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/


dep_TDR:
	LDLI(r17, TDR_INIT_H_K, r31)
	sll	r17, 32, r17
	LDLI(r16, TDR_INIT_L_K, r31)
	zap	r16, 0xf0, r16
	bis	r17, r16, r17
	load_csr_base(r16, CCHIP_CSR)
	lda	r16, CSR_TDR(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/

	br	r31, Sync20

ReadJumpers:
	/*
	** Give the Primary a couple of seconds to catch up.
	** Don't want to poll the CSRs while the primary is
	** configuring the interface.
	*/
	bis	r30, r25, r30		/* OR it into global register	*/
	ldah	r5, 20(r31)		/* Delay 2 seconds		*/
Wait0:
	subq	r5, 1, r5
	bne	r5, Wait0

Sync20:
	SyncCPUSequence(SYNC_CONTINUE, 0x20)

	load_csr_base(r18, TIG_BUS)
	and	r30, CPU_ID_MASK, r17	/* Get Processor ID		*/
	bis	r17, CPU0_CONFIG, r17	/* OR in CPU Config offset	*/
	sll	r17, 27, r17		/* Shift into place		*/
	bis	r18, r17, r16		/* OR in TIG bus address space	*/
	ldq_p	r17, 0(r16)		/* Read Jumpers			*/
	zap	r17, 0xfe, r17		/* Zap other bits		*/
	sll	r17, JMP_CPU_INDEX, r17
	bis	r30, r17, r30		/* OR in CPU config Jumpers	*/

#ifdef DEBUG_PC264
	lda	r17, PC264_MB_CONFIG(r31)
#else
	lda	r17, MB_CONFIG(r31)
#endif
	sll	r17, 27, r17
	bis	r18, r17, r16
	ldq_p	r17, 0(r16)		/* Read Jumpers			*/
	zap	r17, 0xfe, r17		/* Zap other bits		*/
#ifdef DEBUG_PC264
	bis	r17, 0x86, r17		// slowest memory speed
#endif
#ifdef Brick
	and	r17, 0x40, r2		// mini-debugger
	and	r17, 0x20, r17		// Fail-Safe Booter
	srl	r17, 5, r17		// move to bit 0
	srl	r2, 3, r2		// move to bit 3
	bis	r17, r2, r17		// 
	bic	r17, 6, r17		// Memory speed
#endif

	sll	r17, JMP_MB_INDEX, r17
	bis	r30, r17, r30		/* OR in CPU config Jumpers	*/

#ifndef Brick
	lda	r17, Array0_PD(r31)
	sll	r17, 27, r17
	bis	r18, r17, r16
#ifdef DEBUG_PC264
	bis	r31, r31, r17		/* Read Jumpers			*/
	bis	r31, r31, r2		/* Read Jumpers			*/
	bis	r31, r31, r8		/* Read Jumpers			*/
	br	r31, merge_dimm0
#else
	ldq_p	r17, 0(r16)		/* Read Jumpers			*/
#endif
	zap	r17, 0xfe, r17		/* Zap other bits		*/
	cmpeq	r17, 0xff, r0
	beq	r0, no_check_iic_dimm
#endif
        bis     r31, r31, r5 		// pass dimm_slot
        bsr     r28, srom_iic_configure	// size returned in r2
	br	r31, merge_dimm0

no_check_iic_dimm:
	PRINTC(a_slash)
	PRINTL(r17)
	PRINTC(a_slash)
	bsr	r29, convert_ppd
merge_dimm0:
	PRINTC(a_star)
	PRINTL(r2)
	PRINTC(a_star)
	PRINTL(r8)
	PRINTC(a_star)

	sll	r2, JMP_Arr0_INDEX, r17
	bis	r30, r17, r30		/* OR in CPU config Jumpers	*/

#ifndef Brick
	lda	r17, Array1_PD(r31)
	sll	r17, 27, r17
	bis	r18, r17, r16
#ifdef DEBUG_PC264
	lda	r17, 0xda(r31)		/* Read Jumpers			*/
	zap	r17, 0xfe, r17		/* Zap other bits		*/
	cmpeq	r17, 0xff, r0
	beq	r0, no_check_iic_dimm_1
        bis     r31, 4, r5                      // pass dimm_slot
        bsr     r28, srom_iic_configure	// size returned in r2
	br	r31, merge_dimm1

#else
	ldq_p	r17, 0(r16)		/* Read Jumpers			*/
#endif
	zap	r17, 0xfe, r17		/* Zap other bits		*/
	cmpeq	r17, 0xff, r0
	beq	r0, no_check_iic_dimm_1
#endif
        bis     r31, 2, r5                      // pass dimm_slot
        bsr     r28, srom_iic_configure	// size returned in r2
	br	r31, merge_dimm1

no_check_iic_dimm_1:
	PRINTC(a_slash)
	PRINTL(r17)
	PRINTC(a_slash)
	bsr	r29, convert_ppd
merge_dimm1:

	PRINTC(a_star)
	PRINTL(r2)
	PRINTC(a_star)
	PRINTL(r8)
	PRINTC(a_star)

#ifdef Brick
	srl	r30, JMP_Arr0_INDEX, r17
	and	r17, 0xff, r17	
	cmpeq	r17, r2, r0
	bne	r0,  check_iic_dimm_2
	bis	r31, 7, r19		// 7 beeps
	bsr	r28, Beep		/* Beep error code.		*/
	bis	r31, 0xff, r17
	sll	r17, JMP_Arr0_INDEX, r17
	bic	r30, r17, r30
#else
	sll	r2, JMP_Arr1_INDEX, r17
	bis	r30, r17, r30		/* OR in CPU config Jumpers	*/

#ifdef DEBUG_PC264
	br	r31, dimms_sized
#endif
	lda	r17, Array2_PD(r31)
	sll	r17, 27, r17
	bis	r18, r17, r16
	ldq_p	r17, 0(r16)		/* Read Jumpers			*/
	zap	r17, 0xfe, r17		/* Zap other bits		*/
	cmpeq	r17, 0xff, r0
	beq	r0, no_check_iic_dimm_2
#endif
check_iic_dimm_2:
        bis     r31, 4, r5                      // pass dimm_slot
        bsr     r28, srom_iic_configure	// size returned in r2
	br	r31, merge_dimm2

no_check_iic_dimm_2:
	PRINTC(a_slash)
	PRINTL(r17)
	PRINTC(a_slash)
	bsr	r29, convert_ppd
merge_dimm2:

	PRINTC(a_star)
	PRINTL(r2)
	PRINTC(a_star)
	PRINTL(r8)
	PRINTC(a_star)

#ifdef Brick
	sll	r2, JMP_Arr1_INDEX, r17
#else
	sll	r2, JMP_Arr2_INDEX, r17
#endif
	bis	r30, r17, r30		/* OR in CPU config Jumpers	*/

#ifndef Brick
	lda	r17, Array3_PD(r31)
	sll	r17, 27, r17
	bis	r18, r17, r16
	ldq_p	r17, 0(r16)		/* Read Jumpers			*/
	zap	r17, 0xfe, r17		/* Zap other bits		*/
	cmpeq	r17, 0xff, r0
	beq	r0, no_check_iic_dimm_3
#endif
        bis     r31, 6, r5                      // pass dimm_slot
        bsr     r28, srom_iic_configure	// size returned in r2
	br	r31, merge_dimm3

no_check_iic_dimm_3:
	PRINTC(a_slash)
	PRINTL(r17)
	PRINTC(a_slash)
	bsr	r29, convert_ppd
merge_dimm3:

	PRINTC(a_star)
	PRINTL(r2)
	PRINTC(a_star)
	PRINTL(r8)
	PRINTC(a_star)

#ifdef Brick
	srl	r30, JMP_Arr1_INDEX, r17
	and	r17, 0xff, r17	
	cmpeq	r17, r2, r0
	bne	r0,  check_iic_dimm_done
	bis	r31, 7, r19		// 7 beeps
	bsr	r28, Beep		/* Beep error code.		*/
	bis	r31, 0xff, r17
	sll	r17, JMP_Arr1_INDEX, r17
	bic	r30, r17, r30
#else
	sll	r2, JMP_Arr3_INDEX, r17
	bis	r30, r17, r30		/* OR in CPU config Jumpers	*/
#endif
check_iic_dimm_done:
	bis	r31, 1, r17
	sll	r17, JMP_V_ALT_BOOT, r17
	xor	r30, r17, r30		/* Invert boot option bit	*/
dimms_sized:
#ifdef DEBUG_BOARD
	/* Print jumpers */
	PRINTC(a_period)
	srl	r30, 32, r7
	PRINTL(r7)
	PRINTL(r30)
#endif /* DEBUG_BOARD */
	SyncCPUSequence(SYNC_CONTINUE, 0x24)


	srl	r30, JMP_V_PRIMARY, r1
	blbc	r1, SkipInitArrays
InitArrays:
	srl	r30, 1, r1	// cpu mask + 1 
	and	r30, r1, r1			// and mask for cpu0 and cpu1 
	srl     r1, JMP_V_PRESENT, r1        // 1 = cpu1 and cpu0
        srl     r30, JMP_V_MEM_CONFIG, r7       //test the memory seed setting
	and	r7, r1, r1
	blbs	r1, Case1
Case0:
        srl     r30, JMP_V_MEM_CONFIG+1, r1       //test the memory seed setting
	blbs	r1, Case1
	LDLI(r17, MTR_INIT0_H_K, r31)
	sll	r17, 32, r17
	LDLI(r16, MTR_INIT0_L_K, r31)
	bis	r17, r16, r23
	lda 	r17, STR_INIT0_K(r31)
	br	r31, dep_STR1
Case1:
	sll	r1, 63, r1
	bis	r1, r14, r14
	LDLI(r17, MTR_INIT1_H_K, r31)
	sll	r17, 32, r17
	LDLI(r16, MTR_INIT1_L_K, r31)
	bis	r17, r16, r23
	lda 	r17, STR_INIT1_K(r31)
	br	r31, dep_STR1
dep_STR1:
	PRINTC(a_star)
	srl	r14, 32, r18
	PRINTL(r18)
	PRINTL(r14)
	PRINTC(a_star)
	and	r14, 1, r16		// Cchip Configuration array width
	bis	r17, r16, r17		// dchip width
	xor	r16, 1, r16
	bis	r31, r16, r18
	sll	r18, 4, r18
	addq	r18, r16, r18
	sll	r18, 32, r18
	addq	r18, r23, r23
	s4addq	r16, r17, r17
	sll	r17, 8, r16
	bis	r17, r16, r17
	sll	r17, 16, r16
	bis	r17, r16, r17
	sll	r17, 32, r16
	bis	r17, r16, r17
	sll	r17, 32, r16
	bis	r17, r16, r17
	load_csr_base(r16, DCHIP_CSR)
	lda	r16, CSR_STR(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/


/*
 *      ...
 *      ...
 * Wait 20 sysclocks.
 *      ...
 *      ...
 */
	LDLI(r16, (20*MAX_SYS_CLOCK_RATIO), r31)	/* load cycles into R16.	*/
	bsr	r27, wait_n_cycles 	/* Wait a while.		*/



	/* Turn on refreshes */
refresh_on:
	bis	r23, r31, r17
	load_csr_base(r16, CCHIP_CSR)
	lda	r16, CSR_MTR(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/


/*
 *      ...
 *      ...
 * Wait 10? refreshes. (MAX_RI * 64 sysclks = 64*64 = 4096 sysclks)
 *      ...
 *      ...
 */
	LDLI(r16, (4096*10*MAX_SYS_CLOCK_RATIO), r31)/* load cycles into R16.	*/
	bsr	r27, wait_n_cycles 	/* Wait a while.		*/



	/* Turn off refreshes */
refresh_off:
	LDLI(r16, MTR_REF_MASK_H, r31)
	sll	r16, 32, r16
	LDLI(r16, MTR_REF_MASK_L, r16)
	bic	r23, r16, r17
	load_csr_base(r16, CCHIP_CSR)
	lda	r16, CSR_MTR(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/


	bis	r31, r31, r17
	srl	r30, JMP_Arr0_INDEX, r1	/* Check Array PD bits.		*/
	and	r1, 0x7, r1
	beq	r1, dep_MPR0
	lda	r17, MPR0_INIT_L_K(r31)
	srl	r14, 63, r16
	sll	r16, 4, r16
	bis	r16, r17, r17
	and	r14, 1, r16		// Cchip Configuration array width
	xor	r16, 1, r16
	addl	r17, r16, r17		// dchip width
dep_MPR0:
	load_csr_base(r16, CCHIP_CSR)
	lda	r16, CSR_MPR0(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/

	bis	r31, r31, r17
	srl	r30, JMP_Arr1_INDEX, r1	/* Check Array PD bits.		*/
	and	r1, 0x7, r1
	beq	r1, dep_MPR1
	lda	r17, MPR0_INIT_L_K(r31)
	srl	r14, 63, r16
	sll	r16, 4, r16
	bis	r16, r17, r17
	and	r14, 1, r16		// Cchip Configuration array width
	xor	r16, 1, r16
	addl	r17, r16, r17		// dchip width
dep_MPR1:
#ifdef DEBUG_PC264
	lda	r17, 0xd2(r31)
#endif
	load_csr_base(r16, CCHIP_CSR)
	lda	r16, CSR_MPR1(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/

	bis	r31, r31, r17
	srl	r30, JMP_Arr2_INDEX, r1	/* Check Array PD bits.		*/
	and	r1, 0x7, r1
	beq	r1, dep_MPR2
	lda	r17, MPR0_INIT_L_K(r31)
	srl	r14, 63, r16
	sll	r16, 4, r16
	bis	r16, r17, r17
	and	r14, 1, r16		// Cchip Configuration array width
	xor	r16, 1, r16
	addl	r17, r16, r17		// dchip width
dep_MPR2:
	load_csr_base(r16, CCHIP_CSR)
	lda	r16, CSR_MPR2(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/

	bis	r31, r31, r17
	srl	r30, JMP_Arr3_INDEX, r1	/* Check Array PD bits.		*/
	and	r1, 0x7, r1
	beq	r1, dep_MPR3
	lda	r17, MPR0_INIT_L_K(r31)
	srl	r14, 63, r16
	sll	r16, 4, r16
	bis	r16, r17, r17
	and	r14, 1, r16		// Cchip Configuration array width
	xor	r16, 1, r16
	addl	r17, r16, r17		// dchip width
dep_MPR3:
	load_csr_base(r16, CCHIP_CSR)
	lda	r16, CSR_MPR3(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/

	/* Turn on refreshes */
refresh_on2:
	bis	r31, r23, r17
	load_csr_base(r16, CCHIP_CSR)
	lda	r16, CSR_MTR(r16)
	bsr	r26, srom_wrtest	/* write and test it		*/
#define sort
Sort_dimm_sizes:
	bis	r31, 0, r5		// Init index array 
        bis     r31, 1, r4
        insbl   r4, 1, r4		//
	bis     r4, r5, r5		//
	bis     r31, 2, r4		//
	insbl   r4, 2, r4		//
	bis     r4, r5, r5		//
	bis     r31, 3, r4		//
	insbl   r4, 3, r4		//
	bis     r4, r5, r5		//
#ifdef sort
	extbl   r30, 4, r1		//
	and     r1, 7, r1		//
	bis     r31, 0, r4		//
	extbl   r30, 5, r2		//
	and     r2, 7, r2		//
	cmpult  r1, r2, r3		//
//	mb
	cmovlbs r3, 1, r4		//
	cmovlbs r3, r2, r1		//
	extbl   r30, 6, r2		//
	and     r2, 7, r2		//
	cmpult  r1, r2, r3		//
//	mb
	cmovlbs r3, 2, r4		//
	cmovlbs r3, r2, r1		//
	extbl   r30, 7, r2		//
	and     r2, 7, r2		//
	cmpult  r1, r2, r3		//
//	mb
	cmovlbs r3, 3, r4		//
	cmovlbs r3, r2, r1		//
	mskbl   r5, r4, r5		//
	bis     r4, r5, r5		//
	extbl   r5, 1, r4		//
	addq    r4, 4, r3		//
	bis     r31, 1, r4		//
	extbl   r30, r3, r1		//
	and     r1, 7, r1		//
	extbl   r5, 2, r3		//
	addq    r3, 4, r3		//
	extbl   r30, r3, r2		//
	and     r2, 7, r2		//
	cmpult  r1, r2, r3		//
//	mb
	cmovlbs r3, 2, r4		//
	cmovlbs r3, r2, r1		//
	extbl   r5, 3, r3		//
	addq    r3, 4, r3		//
	extbl   r30, r3, r2		//
	and     r2, 7, r2		//
//	mb
	cmpult  r1, r2, r3		//
	cmovlbs r3, 3, r4		//
	cmovlbs r3, r2, r1		//
	extbl   r5, 1, r1		//
	extbl   r5, r4, r2		//
	mskbl   r5, r4, r5		//
	mskbl   r5, 1, r5		//
	insbl   r1, r4, r1		//
	insbl   r2, 1, r2		//
	bis     r2, r5, r5		//
	bis     r1, r5, r5		//
	extbl   r5, 2, r4		//
	addq    r4, 4, r3		//
	bis     r31, 2, r4		//
	extbl   r30, r3, r1		//
	and     r1, 7, r1		//
	extbl   r5, 3, r3		//
	addq    r3, 4, r3		//
	extbl   r30, r3, r2		//
	and     r2, 7, r2		//
	cmpult  r1, r2, r3		//
//	mb
	cmovlbs r3, 3, r4		//
	extbl   r5, 2, r1		//
	extbl   r5, r4, r2		//
	mskbl   r5, r4, r5		//
	mskbl   r5, 2, r5		//
	insbl   r1, r4, r1		//
	insbl   r2, 2, r2		//
	bis     r2, r5, r5		//
	bis     r1, r5, r5		//
#endif
	bis	r31, r31, r23		/* Init Memory Size parameter	*/
	bis	r31, r31, r10		/* Init Base Address counter	*/
	bis	r31, 4, r4
	and	r14, 1, r14		// array width from Cchip csr
	xor	r14, 1, r14
SetupArray:
	mb
	bis	r31, r31, r17
	and	r5, 7, r2
	addq	r2, 4, r2
	extbl   r30, r2, r3		//
	and	r3, 7, r1		// mask off only size bits
	beq	r1, dep_AARn

	subl	r1, r14, r1
	sll	r1, 12, r17		/* Shift into ASIZ		*/
	srl	r3, 7, r2		// split array bit
	sll	r2, 8, r2
	bis	r17, r2, r17
	srl	r3, 6, r2		// split array bit
	and	r2, 1, r2		// banks size
	bis     r17, r2, r17
	srl     r3, 2, r2               // number of rows	
	and	r2, 0xc, r2		// banks size
	bis     r17, r2, r17
	bis	r31, 1, r2
	addq	r1, 23, r3
	sll	r2, r3, r11		/* r11 = Array size (bytes)	*/
	bis	r17, r10, r17
	addq	r10, r11, r10		/* Increment Base address cntr	*/
	addq	r23, r11, r23		/* Increment Memory Size param	*/
dep_AARn:
	load_csr_base(r16, CCHIP_CSR)
	lda	r16, CSR_AAR0(r16)
	and	r5, 7, r2
	mulq	r2, 0x40, r2
	addq	r16, r2, r16
	bsr	r26, srom_wrtest	/* write and test it		*/
	subq	r4, 1, r4		/* Increment array counter	*/
	srl	r5, 8, r5		/* down next index		*/
	bne	r4, SetupArray
#ifdef DEBUG_BOARD
	bis	r31, r31, r25		// zero target address
	jump_to_mini_prompt()		/* trap to Mini-Debugger.	*/
#endif

/*
 *      ...
 *      ...
 * Wait 10? refreshes. (MAX_RI * 64 sysclks = 64*64 = 4096 sysclks)
 *      ...
 *      ...
 */
	LDLI(r16, (4096*10*MAX_SYS_CLOCK_RATIO), r31)/* load cycles into R16.	*/
	bsr	r27, wait_n_cycles 	/* Wait a while.		*/


SkipInitArrays:
#ifdef Acer

Config_Acer:
	ldah	r0, 0x4010(r31)
	lda     r0, 0xf000(r0)
	sll     r0, 13, r0		// 801fe000000 Linear Configuration  address 
	lda	r4, 0x8000(r31)		/* Kseg = 0xffff8000.00000000	*/
	sll	r4, 32, r4		/* Shift into place		*/
	addq	r0, r4, r0
	lda	r0, Acer_CFG(r0)		// idsel 18

	bis	r31, Acer_PIC_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_PIC(r0)
	mb
	bis	r31, Acer_IORC_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_IORC(r0)
	mb
	bis	r31, Acer_ISACI_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_ISACI(r0)
	mb
	bis	r31, Acer_ISACII_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_ISACII(r0)
	mb
	bis	r31, Acer_PIPM_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_PIPM(r0)
	mb
	bis	r31, Acer_BCSC_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_BCSC(r0)
	mb
	bis	r31, Acer_IDEIC_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_IDEIC(r0)
	mb
	bis	r31, Acer_GPOS_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_GPOS(r0)
	mb
	bis	r31, Acer_SMCCII_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_SMCCII(r0)
	mb
	bis	r31, Acer_RAM_ROM_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_RAM_ROM(r0)
	mb
	bis	r31, Acer_SCIIR_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_SCIIR(r0)
	mb
	bis	r31, Acer_USBIDS_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_USBIDS(r0)
	mb

//	bis	r31, r31, r1			// select Rom bank 0 as default
//	mb					// lets stop spec stores
//	stb	r1, Acer_GPO_flash_access(r0)
//	mb

	ldah	r0, 0x4010(r31)
	lda     r0, 0xf000(r0)
	sll     r0, 13, r0		// 801fe000000 Linear Configuration  address 
	lda	r4, 0x8000(r31)		/* Kseg = 0xffff8000.00000000	*/
	sll	r4, 32, r4		/* Shift into place		*/
	addq	r0, r4, r0
	lda	r0, Acer_IDE(r0)		// idsel 24
	bis	r31, Acer_IDE_Force_compatability_mode_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_IDE_Force_compatability_mode(r0)
	mb
	bis	r31, Acer_IDE_Locks_compatability_mode_data, r1
	mb					// lets stop spec stores
	stb	r1, Acer_IDE_Locks_compatability_mode(r0)
	mb
//	jump_to_mini_prompt()		/* trap to Mini-Debugger.	*/

#endif

	OutLEDPORT(0x20)
	mb

/* ======================================================================
 * 5..Detect CPU speed by polling the Periodic Interrupt Flag in the RTC.
 * ======================================================================
 *
 */
DetectSpeed:

/*
 * Guarantee that the RTC is stable by waiting after reset at least 500us.
 * For a maximum CPU speed of 1GHz, this would be 1GHz * 500us = 500,000
 * cycles.  To save instructions, I'll round up to 0x80000 = 524288 cycles.
 */
	ldah	r16, 8(r31)
	bsr	r27, wait_n_cycles

/*
 * Each CPU must have a turn at this.
 */
	lda	r12, MAX_CPUS(r31)	/* Load up CPU count supported	*/
SpeedLoop:
	subq	r12, 1, r12		/* Decrement CPU count		*/
	SyncCPUSequence(SYNC_CONTINUE, 0x28) /* Sync up CPUs here	*/

	and	r30, CPU_ID_MASK, r2	/* Get Processor ID		*/
	cmpeq	r2, r12, r2		/* It it the current CPU's turn?*/
	mb
	beq	r2, SkipGetSpeed	/* If not, don't go this turn	*/
	mb

	bsr 	r28, GetCPUSpeed	/* Detect the CPU speed.	*/
	bis	r0, r0, r17		/* Speed in cycles/second (Hz). */

SkipGetSpeed:
	SyncCPUSequence(SYNC_CONTINUE, 0x2C) /* Sync up CPUs here	*/
	mb
	bne	r12, SpeedLoop		/* Loop until all had a turn	*/

#ifndef DEBUG_BOARD
	LDLI(r16, BAUD, r31)		/* Baud rate SROM serial port	*/
	bsr	r26, nintdivide		/* Compute cycles per bit.	*/
	SET_BAUD (r0)			/* Set baud rate for srom port	*/

	/* Print Banner */
	PRINTS8(b4_to_l(a_cr,a_cr,a_nl,a_D), b4_to_l(a_P,a_2,a_6,a_4))

PrintVersionID:				/* Keep version in sync with RCS id */
	PRINTS4(b4_to_l(a_period, a_period, a_period, (RCS_ENCODED_REV_H>>(8*3))))
	LDLI(r0, RCS_ENCODED_REV_L, r31)
	bis	r31, r31, r16
#ifdef Save_Memory
	bis	r16, 1, r16
#endif
#ifdef DEBUG_BOARD
	bis	r16, 2, r16
#endif
#ifdef LCD
	bis	r16, 4, r16
#endif
#ifdef DDR_Cache
	bis	r16, 8, r16
#endif
#ifdef Brick
	bis	r16, 16, r16
#endif
#ifdef new_Arbiter_bit
	bis	r16, 32, r16
#endif
#ifdef BillClinton
	bis	r16, 64, r16
#endif
#ifdef Acer
	bis	r16, 128, r16
#endif
	sll	r16, 16, r16
	bis	r0, r16, r0
	PRINTL(r0)
#endif /* not DEBUG_BOARD */

	OutLEDPORT(0x01)

/*
**	Print Jumpers
*/
	PRINTC(a_period)
	srl	r30, 32, r7
	PRINTL(r7)
	PRINTL(r30)

#ifdef DEBUG_BOARD
	breakpt2:
	srl	r30, JMP_V_MINIDBGR, r1	/* Jump to minidbg on error?	*/
	blbs	r1, skipbreakpt2
	jump_to_mini_prompt()		/* trap to Mini-Debugger.	*/
skipbreakpt2:
#endif
#ifndef Brick

        srl     r30, JMP_V_PRIMARY, r1
        blbc    r1, SkipInitCypress
	bsr	r26, InitCypress	/* Init Cypress, ISA bus	*/
#endif
SkipInitCypress:

/*
 *	(10**12)/(cycles per second) = CPU clock period in picoseconds
 */
	bis	r17, r17, r16		/* r16 = cycles/second		*/
	LDLI(r3, 0xe8d4a510, r31)	/* Load up (10**12)>>8		*/
	zap	r3, 0xf0, r17		/* Remove sign extension	*/
	sll	r17, 8, r17		/* r17 = 10**12			*/
	bsr	r26, nintdivide		/* Compute CPU speed		*/
#ifdef BillClinton
	srl	r0, 5, r22
	lda	r17, 0x861(r31)
        cmpeq   r22, 0x43, r22
	cmovlbs	r22, r17, r0
#endif
	bis r0, r0, r22			/* r22=CPU speed (picoseconds)	*/
 	OutLEDPORT(0x02)

	/* Print out the CPU speed (picoseconds) */
	PRINTC(a_period)
	PRINTL(r22)

	SyncCPUData(r23, 0x30)
InitBcache:
	mb					// lets stop spec stores
	beq	r23, NoMemory
	srl	r30, JMP_V_CACHE_SIZE, r1/* Jump to minidbg on error?	*/
	and	r1, 0xf, r7		/* Only supports 1MB to 16MB	*/
	xor	r7, 0xc, r7		/* Only supports 1MB to MB	*/
#ifdef Brick
	and	r1, 3, r7             /* Only supports 1MB to 4MB     */
#endif
        OutLEDPORT(0x03)                /* Initializing Bcache          */

/*
 *      Convert R7 to Bcache size.
 *      R7      size
 *      ==      ========
 *      c       disabled
 *      d       1MB
 *      e       2MB
 *      f       4MB
 *      4       8MB
 *      5       16MB
 */
        subq    r7, 1, r1
        bis     r31, 1, r7
        sll     r7, r1, r7              /* R7 = Bcache Size (MBytes)    */


BCache1MB:
#undef bc_size_a
#define bc_size_a               0

        cmpeq   r7, 1, r1
        blbc    r1, BCache2MB
	LDLI(r2, WRITE_MANY_CHAIN_H, r31)
	sll	r2, 32, r2

	LDLI(r14, WRITE_MANY_CHAIN_L, r31)
	zap	r14, 0xf0, r14
	bis	r14, r2, r14

        br      r31, EnableInitMode

BCache2MB:
#undef bc_size_a
#define bc_size_a               1

        cmpeq   r7, 2, r1
        blbc    r1, BCache4MB
	LDLI(r2, WRITE_MANY_CHAIN_H, r31)
	sll	r2, 32, r2

	LDLI(r14, WRITE_MANY_CHAIN_L, r31)
	zap	r14, 0xf0, r14
	bis	r14, r2, r14

        br      r31, EnableInitMode

BCache4MB:
#undef bc_size_a
#define bc_size_a               3

        cmpeq   r7, 4, r1
        blbc    r1, BCache8MB
	LDLI(r2, WRITE_MANY_CHAIN_H, r31)
	sll	r2, 32, r2

	LDLI(r14, WRITE_MANY_CHAIN_L, r31)
	zap	r14, 0xf0, r14
	bis	r14, r2, r14

        br      r31, EnableInitMode

BCache8MB:
#undef bc_size_a
#define bc_size_a               7

        cmpeq   r7, 8, r1
        blbc    r1, BCache16MB
	LDLI(r2, WRITE_MANY_CHAIN_H, r31)
	sll	r2, 32, r2

	LDLI(r14, WRITE_MANY_CHAIN_L, r31)
	zap	r14, 0xf0, r14
	bis	r14, r2, r14

        br      r31, EnableInitMode


BCache16MB:
#undef bc_size_a
#define bc_size_a               15

        cmpeq   r7, 16, r1
        blbc    r1, BCache0MB
	LDLI(r2, WRITE_MANY_CHAIN_H, r31)
	sll	r2, 32, r2

	LDLI(r14, WRITE_MANY_CHAIN_L, r31)
	zap	r14, 0xf0, r14
	bis	r14, r2, r14

        br      r31, EnableInitMode

BCache0MB:
#undef bc_enable_a
#undef bc_size_a
#undef zeroblk_enable_a
#undef set_dirty_enable_a
#undef init_mode_a
#undef enable_evict_a
#undef bc_wrt_sts_a
#undef bc_bank_enable_a

#define bc_enable_a             0
#define bc_size_a               0
#define zeroblk_enable_a        3
#define set_dirty_enable_a      6
#define init_mode_a             0
#define enable_evict_a          0
#define bc_wrt_sts_a            0
#define bc_bank_enable_a        0

        LDLI(r2, WRITE_MANY_CHAIN_H, r31)
        sll     r2, 32, r2

        LDLI(r14, WRITE_MANY_CHAIN_L, r31)
        zap     r14, 0xf0, r14
        bis     r14, r2, r14

        LDLI(r2, BC_ENABLE_I_MODE_MASK, r31)
        bic     r14, r2, r14

        bis	r31, 16, r7               /* Account for bogus sizes      */
        br      r31, PrintChain2



EnableInitMode:
        LDLI(r2, BC_ENABLE_I_MODE_MASK, r31)
        bic     r14, r2, r14

#undef bc_enable_a
#undef bc_size_a
#undef zeroblk_enable_a
#undef set_dirty_enable_a
#undef init_mode_a
#undef enable_evict_a
#undef bc_wrt_sts_a
#undef bc_bank_enable_a

#define bc_enable_a             1
#define bc_size_a               0
#define zeroblk_enable_a        3
#define set_dirty_enable_a      6
#define init_mode_a             0
#define enable_evict_a          0
#define bc_wrt_sts_a            0
#define bc_bank_enable_a        0

	LDLI(r2, WM_BC_SIZE_H(0xf), r31)
	sll	r2, 32, r2

	LDLI(r0, WM_BC_SIZE(0xf), r31)
	bis	r0, r2, r0		/* Generate size mask		*/
	and	r0, r14, r0		/* Grab previous size setting	*/

	LDLI(r2, WRITE_MANY_CHAIN_H, r31)
	sll	r2, 32, r2

	LDLI(r14, WRITE_MANY_CHAIN_L, r31)
	zap	r14, 0xf0, r14
	bis	r14, r2, r14
	bis	r14, r0, r14		/* Merge in previous size	*/

PrintChain2:
	PRINTC(a_period)
	srl	r14, 32, r17
	PRINTB(r17)
	bis	r14, r31, r17
	PRINTL(r17)
	sll	r14, 16, r21		// save to pass with dc_ctl


        OutLEDPORT(0x05)


	addq	r31, 6, r0		/* shift in 6x 6-bits*/
	mb				/* wait for all istream/dstream to complete*/

	br	r31, BCshf2
	.align 6
BCshf2:	mtpr	r14, EV6__DATA		/* shift in 6 bits		*/
	subq	r0, 1, r0		/* decrement R0			*/
	beq	r0, BCend2		/* done if R0 is zero		*/
        srl	r14, 6, r14		/* align next 6 bits		*/

	br	r31, BCshf2		/* continue shifting		*/
BCend2:	mtpr	r31, EV6__EXC_ADDR + 16	/* dummy IPR write - sets SCBD bit 4  */
	addq	r31, r31, r0		/* nop				*/
	addq	r31, r31, r14		/* nop				*/

	mtpr	r31, EV6__EXC_ADDR + 16	/* also a dummy IPR write - stalls until above write retires*/
	beq	r31, BCnxt2		/* predicts fall through in PALmode*/
	br	r31, .-4		/* fools ibox predictor into infinite loop*/
	addq	r31, r31, r14		/* nop*/
BCnxt2:
	mb				/* wait for all istream/dstream to complete*/


SkipInitBcache:
	PRINTC(a_period)
	PRINTB(r7)
	PRINTC(a_period)
	ldah	r0, 0x8000(r31)		/* Kseg = 0xffff8000.00000000	*/
	sll	r0, 16, r0		/* Shift into place		*/
	sll	r7, (20), r1		/* r1 = 2 * BCache size (bytes)	*/
	bis	r0, r1, r0

	br	r31, sweep_bcache1
	.align 6
sweep_bcache1:
	subq	r0, 64, r0
	whint	r0
	mb				/* wait for all istream/dstream to complete*/
        stq     r31, 0(r0)
        stq     r31, 8(r0)
        stq     r31, 16(r0)
        stq     r31, 24(r0)

        stq     r31, 32(r0)
        stq     r31, 40(r0)
        stq     r31, 48(r0)
        stq     r31, 56(r0)
	zap	r0, 0xf0, r1
	srl	r1, 4, r1
	mb
	ecb	r0
	bne	r1, sweep_bcache1
	mb				/* wait for all istream/dstream to complete*/

	/* Now evict the valid stuff from the Dcache... */
sweep_dcache1:
	ldah	r0, 0x8000(r31)		/* Kseg = 0xffff8000.00000000	*/
	sll	r0, 16, r0		/* Shift into place		*/
	sll	r7, (20), r1		/* r1 = 2 * BCache size (bytes)	*/
	bis	r0, r1, r0
	/* ldah	r0, 2(r0)		/* r0 = 2 * DCache size (bytes)	*/

	br	r31, sweep_dcache2
	.align 6
sweep_dcache2:
	subq	r0, 64, r0
	ecb	r0
	zap	r0, 0xf0, r1
	srl	r1, 4, r1
	bne	r1, sweep_dcache2
	mb				/* wait for all istream/dstream to complete*/


#ifdef Soft_reset
	srl	r30, JMP_V_PRIMARY, r1
	blbc	r1, Soft_reseted_once
Soft_reset:
	load_csr_base(r18, SOFT_RESET_BASE)
	lda	r17, SOFT_RESET_check(r31)
	bis	r18, r17, r16		/* OR in TIG bus address space	*/
	ldl_p	r17, 0(r16)		/* Read Jumpers			*/
	mb				/* wait for all istream/dstream to complete*/
	and	r17, 0xff, r17		// only a byte
	bne	r17, Soft_reseted_once		/* Old TIG PAL		*/
        lda     r17, SOFT_RESET_invoke(r31)
        bis     r18, r17, r16           /* OR in TIG bus address space  */
        ldl_p   r17, 0(r16)             /* Read reset	                */
	mb				/* wait for all istream/dstream to complete*/
	and	r17, 0xff, r17		// only a byte
	bne	r17, Soft_reseted_once		/* Reset once already		*/
	bis	r31, 1, r17		/* make it 1 for reset		*/
	bsr	r26, srom_wrtest	/* write and test it		*/

Soft_reseted_once:
#endif
#ifdef LCD
	srl	r30, JMP_V_PRIMARY, r1
	blbc	r1, SkipInit_LCD
	bsr	r7, srom_init_lcd
	mb
#ifndef LDCad
	bis	r31, a_C, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_O, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_M, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_P, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_A, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_Q, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_sp, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_sp, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
#else
	bis	r31, a_Y, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_o, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_u, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_r, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_sp, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_A, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_D, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_sp, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
#endif
SkipInit_LCD:
#endif
/*
 *	Write good parity/ecc to memory by writing all memory
 *	locations.  This is done by re-writing the full contents
 *	of memory with the same data. Reading before writing
 *	memory lengthens the time to initialize data parity/ecc
 *	but it results in the preservation of memory state for
 *	debugging purposes.  The size of memory in bytes is held
 *	in r23 at this point.
 */
#ifndef ISP_SIMULATION
	lda	r1,DC_CTL_INIT_K(r31)	/* load DC_CTL.....*/
	and	r1, 3, r1		/* load DC_CTL.....*/
	mtpr 	r1, EV6__DC_CTL		/* SET_EN=3 (SCRBRD=6)*/
#ifdef Save_Memory
	load_csr_base(r18, SOFT_RESET_BASE)
	lda	r17, SOFT_RESET_check(r31)
	bis	r18, r17, r16		/* OR in TIG bus address space	*/
	ldl_p	r17, 0(r16)		/* Read Jumpers			*/
	mb				/* wait for all istream/dstream to complete*/
	and	r17, 0xff, r17		// only a byte
	bne	r17, Memorysweeped		/* Old TIG PAL		*/
        lda     r17, SOFT_RESET_invoke(r31)
        bis     r18, r17, r16           /* OR in TIG bus address space  */
        ldl_p   r17, 0(r16)             /* Read reset	                */
	mb				/* wait for all istream/dstream to complete*/
	and	r17, 0xff, r17		// only a byte
	bne	r17, Memorysweeped 		/* Reset once already		*/

#endif
	srl	r30, JMP_V_PRIMARY, r6
	beq	r23, Memorysweeped
	bis	r23, r31, r19		/* r23 = Memory Size	*/
	sll	r0, 16, r0		/* Shift into place		*/
	ldah	r0, 0x8000(r31)		/* Kseg = 0xffff8000.00000000	*/
	sll	r0, 16, r18		/* Shift into place		*/
	blbc	r6, SkipSweepMemory
#ifdef LCD
	bis	r31, 0xc0, r16
	bis	r31, 0, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_vbar, r7
	bis	r31, a_slash, r16
	sll	r16, 8, r16
	bis	r7, r16, r7
	bis	r31, a_minus, r16
	sll	r16, 16, r16
	bis	r7, r16, r7
	bis	r31, a_bslash, r16
	sll	r16, 24, r16
	bis	r7, r16, r7
	bis	r31, a_vbar, r16
	sll	r16, 32, r16
	bis	r7, r16, r7
	bis	r31, a_slash, r16
	sll	r16, 40, r16
	bis	r7, r16, r7
	bis	r31, a_minus, r16
	sll	r16, 48, r16
	bis	r7, r16, r7
	bis	r31, a_bslash, r16
	sll	r16, 56, r16
	bis	r7, r16, r7
	and	r7, 0xff, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	and	r7, 0xff, r16
	srl	r7, 8, r7
	sll	r16, 56, r16
        bis     r7, r16, r7
	mb
#else
#ifdef Brick
	lda	r16, 0x5555(r31)
	ldah	r16, 0x5555(r16)
	sll	r16, 32, r7
	bis	r16, r7, r7
	lda     r16, 0x8013(r31)
	sll	r16, 28, r16		/* Shift into place		*/
	stl_p	r31, 0x580(r16)		/* write SW_Alert			*/
#endif
#endif
SweepMemory:
	mb
	subq	r19, 0x40, r19	/* Decrement pointer	*/
	whint	r18
	mb
	stq   	r31, 0x00(r18)	/* Store same Quadword	*/
	stq	r31, 0x08(r18)	/* Store same Quadword	*/
	stq	r31, 0x10(r18)	/* Store same Quadword	*/
	stq	r31, 0x18(r18)	/* Store same Quadword	*/
	stq   	r31, 0x20(r18)	/* Store same Quadword	*/
	stq	r31, 0x28(r18)	/* Store same Quadword	*/
	stq	r31, 0x30(r18)	/* Store same Quadword	*/
	stq	r31, 0x38(r18)	/* Store same Quadword	*/
	mb
	ecb	r18
	addq	r18, 0x40, r18	/* increment pointer	*/
	mb					// lets stop spec stores
	sll	r19, 39, r5	// write a dot when 2^29
	bne	r5, donot_print
#ifdef LCD
	bis	r31, 0xc0, r16
	bis	r31, 0, r17
        bsr     r26, srom_access_lcd
	mb
	and	r7, 0xff, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	and	r7, 0xff, r16
	srl	r7, 8, r7
	sll	r16, 56, r16
        bis     r7, r16, r7
#else
#ifdef Brick
	stl_p	r7, 0x580(r16)		/* write SW_Alert			*/
	and	r7, 0x1, r17
	srl	r7, 1, r7
	sll	r17, 63, r17
        bis     r7, r17, r7
#endif
#endif
	mb
donot_print:
	bgt	r19, SweepMemory /* Loop until done      */
	mb
	br	r31, Memorysweeped
	.align 3
SkipSweepMemory:
	mb
	subq	r19, 0x40, r19	/* Decrement pointer	*/
	whint	r18
	mb
	stq   	r18, 0x00(r18)	/* Store same Quadword	*/
	stq	r18, 0x08(r18)	/* Store same Quadword	*/
	stq	r18, 0x10(r18)	/* Store same Quadword	*/
	stq	r18, 0x18(r18)	/* Store same Quadword	*/
	stq   	r18, 0x20(r18)	/* Store same Quadword	*/
	stq	r18, 0x28(r18)	/* Store same Quadword	*/
	stq	r18, 0x30(r18)	/* Store same Quadword	*/
	stq	r18, 0x38(r18)	/* Store same Quadword	*/
	mb
	ecb	r18
	mb
	addq	r18, 0x40, r18	/* increment pointer	*/
	mb
	bgt	r19, SkipSweepMemory /* Loop until done      */

Memorysweeped:
#endif /* ISP_SIMULATION */
 	OutLEDPORT(0x06)
	SyncCPUData(r23, 0x30)

	bis	r23, r31, r6	/* r23 = Memory Size	*/
	PRINTC(a_period)
	PRINTL(r6)

#ifdef DEBUG_BOARD
	breakpt3:
	srl	r30, JMP_V_MINIDBGR, r1	/* Jump to minidbg on error?	*/
	blbs	r1, skipbreakpt3
	jump_to_mini_prompt()		/* trap to Mini-Debugger.	*/
skipbreakpt3:
	lda	r1, DC_CTL_INIT_K(r31)	/* load DC_CTL.....*/
	mtpr 	r1, EV6__DC_CTL		/* .....ECC_EN=0, FHIT=0, SET_EN=3 (SCRBRD=6)*/
#endif

        SyncCPUSequence(SYNC_CONTINUE, 0x34)
	OutLEDPORT(0x14)


        load_csr_base(r16, CCHIP_CSR)
        lda     r16, CSR_PRBEN(r16)
        stq_p   r31, 0(r16)             /* Enable Probes                */
	mb
	SyncCPUSequence(SYNC_CONTINUE, 0x38) 



#ifdef DEBUG_BOARD
breakpt4:
	srl	r30, JMP_V_MINIDBGR, r1	/* Jump to minidbg on error?	*/
	blbs	r1, skipbreakpt4
	bis	r31, r31, r25
	jump_to_mini_prompt()		/* trap to Mini-Debugger.	*/
	bne	r25, SkipLoadSystemCode
	
skipbreakpt4:
#endif

/*
 * ======================================================================
 * Load System Code
 * ======================================================================
 */

#ifndef ISP_SIMULATION
	srl	r30, JMP_V_PRIMARY, r1
	blbc	r1, SkipLoadSystemCode

#ifdef LCD

	bis	r31, 0xc0, r16
	bis	r31, 0, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_F, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_i, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_r, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_m, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_w, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_a, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_r, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_e, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb

#endif
	bis	r30, r30, r16		/* Copy Jumpers to r16			*/
	bsr	r14, LoadSystemCode

/*
 *	At this point r8 points to first memory
 *	location following System ROM image and R25 points to the
 *	destination (the address where program execution will
 *	begin after executing the flush code.)
 */

	/* Print address used to store IC flush code */
	PRINTC(a_period)
	PRINTL(r25)

//	br	r31, OutLEDPORT_17

SkipLoadSystemCode:
OutLEDPORT_17:
#endif /* ISP_SIMULATION */
	OutLEDPORT(0x17)

/*
**	Flush the caches and enable probes.
**	This should probably be done in two passes where
**	all processors other than the primary flush on
**	the first pass and only the primary flushes on
**	the second pass.
*/
	PRINTC(a_period)
	mb


/* ======================================================================
 * 12..Passes control to the next level of firmware or mini-debugger.	=
 * ======================================================================
 *	This is the last code executed by the SROM.  It will pass control
 *	to the next level of firmware (or the mini-debugger).  It adheres
 *	to the calling interface defined at the beginning of this file.
 *	R30 is assumed to contain the value of the configuration jumpers
 *	in the standard format defined elsewhere in this file.
 *
 *	At this point:
 *	R8 - Address following last byte of code loaded.
 *	R25 - Starting address of image loaded.
 *
 *	Pass out the following registers:
 *	R23 - Memory size in bytes.
 *	r22 - CPU speed in picoseconds.
 *	R25 - Chip revision.
 *	excAddr - Address of ICache flush code to execute in PALmode.
 *
 *	The SROM Mini-Debugger is provided in the SROM.  If the mini-
 *	debugger jumper is in, the SROM code traps to the mini-debugger
 *	after all initialization is complete but before starting the
 *	execution of the System ROM code.  The System ROM image can be
 *	started from the Mini-Debugger with the "Start Execution (st)"
 *	command or the "Return (rt) command".
 *
 *	Note:  We must be careful not to overwrite the registers composing
 *	       the interface to the next level.
 *
 */
NextLevel:
//ConfigDcache:

//	SyncCPUData(r14, 0x40)

	SyncCPUData(r25, 0x44)

	srl	r30, JMP_V_PRIMARY, r5
	blbc	r5, jumpPrimary	/* Is this the Primary CPU?	*/

	load_csr_base(r9, CCHIP_CSR)	/* Get Cchip base		*/
	lda	r9, CSR_MISC(r9)	/* Add in MISC offset		*/
	lda	r5, 1(r31)		/* 1 -> r5 */
	sll	r5, 24, r5		/* arbitration clear */
	stq_p	r5, 0(r9)		/* Store it to MISC register	*/
#ifdef SRM_HACK
	mb
	load_csr_base(r9, SYNCHRONIZE_CPU) /* Get srm hack base		*/
        lda     r9, SYNCHRONIZE_CPU0(r9)   /* Add in CPU0 offset           */
        lda     r5, 69(r31)              /* 69 -> r5 */
        stq_p   r5, 0(r9)               /* Store it to MISC register    */
        lda     r9, (SYNCHRONIZE_CPU1-SYNCHRONIZE_CPU0)(r9)   /* Add in CPU0 offset           */
        stq_p   r5, 0(r9)               /* Store it to MISC register    */
	mb
	lda     r9, SYNCHRONIZE_CPU0_TRANSFER_ADDRESS_PHYS(r31)
        stq_p   r31, 0(r9)               /* Store it to MISC register    */
        stq_p   r31, 8(r9)               /* Store it to MISC register    */
#endif
	mb
jumpPrimary:
breakpt5:
	srl	r30, JMP_V_MINIDBGR, r0	/* Jump to minidbg on error?	*/
	blbs	r0, skipbreakpt5
#ifdef DEBUG_BOARD
	jump_to_mini_prompt()		/* trap to Mini-Debugger.	*/
#else
	jump_to_minidebugger()		/* trap to Mini-Debugger.	*/
#endif
skipbreakpt5:
        ldah    r0, 0x8000(r31)         /* Kseg = 0xffff8000.00000000   */
        sll     r0, 16, r0              /* Shift into place             */
        ldah    r0, 2(r0)               /* r0 = 2 * DCache size (bytes) */
	br	r31, sweep_dcachefini
	.align 4
sweep_dcachefini:
	subq	r0, 64, r0
	ecb	r0
	zap	r0, 0xf0, r1
	srl	r1, 4, r1
	bne	r1, sweep_dcachefini
	mb				/* wait for all istream/dstream to complete*/


        mb
	lda	r1, DC_CTL_INIT_K(r31)	/* load DC_CTL.....*/
#ifndef DEBUG_PC264
	bis	r1, 0xc0, r1		// set ecc and tag parity
#endif
	mtpr 	r1, EV6__DC_CTL		/* .....ECC_EN=0, FHIT=0, SET_EN=3 (SCRBRD=6)*/
	bis	r21, r1, r1		// DC_CTL..and write many chain

	srl	r30, JMP_V_PRESENT, r20	/* Shift down CPU sign-in roll	*/
	and	r20, CPU_MASK, r20	/* Active Processor Mask	*/

	LDLQ	(r15, RCS_ENCODED_REV_H, RCS_ENCODED_REV_L)
	bis	r31, r31, r16
#ifdef Save_Memory
	bis	r16, 1, r16
#endif
#ifdef DEBUG_BOARD
	bis	r16, 2, r16
#endif
#ifdef LCD
	bis	r16, 4, r16
#endif
#ifdef DDR_Cache
	bis	r16, 8, r16
#endif
#ifdef Brick
	bis	r16, 16, r16
#endif
#ifdef REVD
	bis	r16, 32, r16
#endif
#ifdef BillClinton
	bis	r16, 64, r16
#endif
#ifdef Acer
	bis	r16, 128, r16
#endif
#ifdef EV6_PASS1
	bis	r16, 256, r16
#endif
	sll	r16, 16, r16
	bis	r15, r16, r15
	and	r30, CPU_ID_MASK, r16	/* Get chip ID.			*/
	bis	r23, r23, r17		/* Get Size of memory		*/
	bis	r22, r22, r18		/* Get CPU speed in ps.		*/
	LDLI	(r19, Signature_and_System_Revision, r31)	/* Signature and System Revision*/
	zap	r19, 0xf0, r19		/* Remove sign extension	*/
	lda	r21, 0(r31)		/* System context value		*/
	bis	r25, 1, r27		/* make it palcode		*/
	srl	r30, JMP_V_PRESENT, r5	/* Shift up Processor mask	*/
	bis	r31, r31, r4		// set lowest number cpu
	and	r5, CPU_ID_MASK, r5	/* Get chip ID.			*/
	cmovlbc	r5, 1, r4
	cmpeq	r16, r4, r5
	blbs	r5, lets_go		/* Is this the Primary CPU?	*/
#ifdef SRM_HACK

        and     r30, CPU_ID_MASK, r9   	   /* Zap other bits               */
        lda     r5, SYNCHRONIZE_CPU0(r31)   /* Add in CPU0 offset           */
        lda     r4, SYNCHRONIZE_CPU1(r31)   /* Add in CPU1 offset           */
	cmovne	r9, r4, r5		 /* cpu0 or cpu 1	*/	
	load_csr_base(r4, SYNCHRONIZE_CPU) /* Get srm hack base		*/
	addq	r4, r5, r5		/* get the address		*/
	lda     r4, SYNCHRONIZE_CPU0_TRANSFER_ADDRESS_PHYS(r31)
	s8addq	r9, r4, r9		/* CPU_TRANSFER_ADDRESS */
	ldah	r4, 0x8000(r31)		/* Kseg = 0xffff8000.00000000	*/
	sll	r4, 16, r4		/* Shift into place		*/
	bis	r4, r9, r4

wait_for_srm:
	mb
        ldl_p   r4, 0(r5)               /* READ SYNCHRONIZE_CPU */
	bis	r31, r31, r31		/* make it wait		*/
	blbs	r4, wait_for_srm	/* wait for the SRM	*/
	mb
        ldl_p   r25, 0(r9)               /* READ TRANSFER_ADDRESS */
	bis	r25, 1, r27		/* make it palcode		*/
	mb
#endif
	blbc	r31, lets_go		/* Is this the Primary CPU?	*/


.align 3
lets_go:
        mb
	bis	r31, r31, r31		/* make it wait		*/
        mtpr    r31, EV6__IC_FLUSH      // (4,0L) flush the icache
        bne     r31, Parkret_flush              // pvc #24
Parkret_flush:
pvc$huf24$1007:
	bne	r31, 1f			// push prediction stack
1:	hw_rets/jmp	(r27)		/* Jump to ICflush/bootstrap	*/



/*
**	SyncronizeCPUs - Sync CPUs in a multi-processor system
**
**	This implements the interprocessor communication protocol used to
**	init a multi-processor system.  Since the init responsibilities of
**	the SROM precludes gratuitous memory usage, this protocol communicates
**	via the Cchip MISC register and passes data between processors via
**	the Cchip Device Interrupt Mask registers (DIM0 and DIM1).  Care
**	is taken to avoid race conditions and use of stale data between
**	processors.
*/
SyncronizeCPUs:
	load_csr_base(r5, CCHIP_CSR)	/* Load up Cchip base		*/
	lda	r9, CSR_MISC(r5)	/* r9, Load pointer to MISC	*/
	lda	r10, CSR_DIM0(r5)	/* r10, Load pointer to DIM0	*/
	lda	r11, CSR_DIM1(r5)	/* r11, Load pointer to DIM1	*/

	srl	r30, JMP_V_PRIMARY, r5
	blbc	r5, SyncSecondary	/* Is this the Primary CPU?	*/
SyncPrimary:
	stq_p	r16, 0(r11)		/* Store CPU data into DIM1	*/
	mb
	stq_p	r8, 0(r10)		/* Store Sequence/Data into DIM0*/
	mb

#ifdef DEBUG_SYNC
	PRINTC(a_period)
	srl	r8, 32, r6
	PRINTL(r6)
	PRINTL(r8)
	PRINTC(a_slash)
	srl	r16, 32, r6
	PRINTL(r6)
	PRINTL(r16)
#endif /* DEBUG_SYNC */

	/*
	** Sign in current CPU
	*/
	and	r30, CPU_ID_MASK, r5	/* Get Processor ID		*/
	lda	r6, 1(r31)		/* r6 <- 1			*/
	sll	r6, r5, r6		/* Shift into CPU mask bit	*/
	sll	r6, 12, r6		/* Shift up to Arbitration Try	*/
	stq_p	r6, 0(r9)		/* Store it to MISC register	*/
	mb

	/*
	** Wait for all CPUs to sign in
	*/
	srl	r30, JMP_V_PRESENT, r6	/* Shift up Processor mask	*/
	and	r6, CPU_ID_MASK, r5	/* Get Processor ID		*/
	ldah	r11, 100(r31)
SyncPrimary0:
	subq	r11, 1, r11
	beq	r11, SyncPrimary_timedout
	ldq_p	r6, 0(r9)		/* Read the MISC register	*/
	srl	r6, 8, r6		/* Shift down Arbitration Try	*/
	and	r6, CPU_MASK, r6	/* Zap other bits		*/

#ifdef DEBUG_SYNC
	PRINTC(a_pound)
	PRINTB(r6)
	PRINTB(r5)
#endif /* DEBUG_SYNC */

	cmpeq	r5, r6, r6		/* Have all CPUs signed in?	*/
	beq	r6, SyncPrimary0	/* If not, loop			*/

	/*
	** Clear data transferred in interrupt mask CSRs.
	** The secondary CPUs have signed in so they have
	** already picked up this data.
	*/
SyncPrimary_timedout:
	stq_p	r31, 0(r10)		/* Clear DIM0			*/
	stq_p	r31, (CSR_DIM1-CSR_DIM0)(r10)		/* clear DIM1			*/
	sll	r5, 8, r6		/* Shift down Arbitration Try	*/
	stq_p	r6, 0(r9)		/* Clear IPNTR the MISC register*/
	mb

	/*
	** Reading the data back here could help to
	** avoid race conditions where stale data still
	** appears in the CSRs.
	*/
	ldq_p	r5, 0(r10)		/* Read DIM0			*/
	ldq_p	r5, 0(r11)		/* Read DIM1			*/

	/*
	** Clear the arbitration fields to
	** signal to other CPUs that all
	** CPUs have synced up here.
	*/
	bne	r31, 1f			// push prediction stack
1:	bis	r26, 1, r26		//      pal mode
	hw_rets/jmp	(r26)		/* All synced up		*/


SyncSecondary:
SyncSecondary_loop:
	ldq_p	r6, 0(r10)		/* Read Sequence/Data from DIM0	*/

#ifdef DEBUG_SYNC
	PRINTC(a_plus)
	srl	r6, 32, r5
	PRINTL(r5)
	PRINTL(r6)
	PRINTC(a_period)
	srl	r8, 32, r5
	PRINTL(r5)
	PRINTL(r8)
#endif /* DEBUG_SYNC */

	cmplt	r6, r8, r5		/* Has primary written DIM0 yet?*/
	blbs	r5, SyncSecondary_loop	/* If not, loop			*/

SyncSecondar_timedout0:
	ldq_p	r16, (CSR_DIM1-CSR_DIM0)(r10)		/* Read CPU data from DIM1	*/

#ifdef DEBUG_SYNC
	PRINTC(a_slash)
	srl	r16, 32, r6
	PRINTL(r6)
	PRINTL(r16)
#endif /* DEBUG_SYNC */

	/*
	** Sign in current CPU
	*/

SyncSecondarySign_in:
	and	r30, CPU_ID_MASK, r5	/* Get Processor ID		*/
	lda	r6, 1(r31)		/* r6 <- 1			*/
	sll	r6, r5, r6		/* Shift into CPU mask bit	*/
	sll	r6, 12, r6		/* Shift up to Arbitration Try	*/
	stq_p	r6, 0(r9)		/* Store it to MISC register	*/
	mb

	/*
	** Wait for the primary CPU to signal
	** that all CPUs have synced up here.
	*/
SyncSecondary0:
	ldq_p	r6, 0(r9)		/* Read the MISC register	*/
	srl	r6, 8, r6		/* Shift down Arbitration Try	*/
	and	r6, CPU_MASK, r6	/* Zap other bits		*/
	bne	r6, SyncSecondary0	/* Loop if not cleared yet	*/

SyncSecondaryRet:
	bne	r31, 1f			// push prediction stack
1:	bis	r26, 1, r26		//      pal mode
	hw_rets/jmp	(r26)		/* All synced up		*/


/*=======================================================================
 *= InitCypress - Initializes the Cypress chip 				=
 *=======================================================================
 * OVERVIEW:
 *      This function will initialize the Cypress chip which controls the
 *	PCI and ISA bus.
 *	Access to IO space is required, therefore, the memory controller
 *	should be initialized before calling this routine.  After the
 *	initialization has been performed, access to the LED card will
 *	be possible.
 *
 * FORM OF CALL:
 *       bsr r26, InitCypress
 *
 * ARGUMENTS:
 *	None.
 *
 * RETURNS:
 *	Nothing.
 *
 * REGISTERS:
 *	r0-r3 - Scratch registers.
 *	r16 - Argument register.
 *  r26, r27, r29 - Return address registers.
 */
InitCypress:

	lda	r16, 0xF(r31)		/* Enable Special Cycle decode	*/
	bsr	r29, WriteCY_CMD_REG	/* Write Command Register	*/

	lda	r16, 0x80(r31)		/* Disable INTA routing		*/
	bsr	r29, WriteCY_INTA_ROUTE	/* Write INTA Routing Register	*/

	lda	r16, 0x80(r31)		/* Disable INTB routing		*/
	bsr	r29, WriteCY_INTB_ROUTE	/* Write INTB Routing Register	*/

	lda	r16, 0x80(r31)		/* Disable INTC routing		*/
	bsr	r29, WriteCY_INTC_ROUTE	/* Write INTC Routing Register	*/

	lda	r16, 0x80(r31)		/* Disable INTD routing		*/
	bsr	r29, WriteCY_INTD_ROUTE	/* Write INTD Routing Register	*/

	lda	r16, 0x27(r31)		/* Grant after 16clks, enable	*/
					/* Post-Write, ISA Master, and	*/
					/* DMA Line Buffer.		*/
	bsr	r29, WriteCY_CTRL_REG	/* Write PCI Control Register	*/

	lda	r16, 0xE0(r31)		/* Enable SERR & Post-Write Buffers*/
	bsr	r29, WriteCY_ERR_CTRL_REG/* Write PCI Error Control Register*/

	lda	r16, 0xF0(r31)		/* Set top of mem to 16MB	*/
	bsr	r29, WriteCY_TOP_OF_MEM	/* Write ISA/DMA Top Of Memory	*/

	lda	r16, 0x40(r31)		/* IDE route to IRQ14 and IRQ15	*/
	bsr	r29, WriteCY_ATCTRL_REG1/* Write AT Control Reigster 1	*/

	lda	r16, 0x80(r31)		/* IDE0 to IRQ14		*/
	bsr	r29, WriteCY_IDE0_ROUTE	/* Write IDE0 INTR to IRQ14	*/

	lda	r16, 0x80(r31)		/* IDE1 to IRQ15		*/
	bsr	r29, WriteCY_IDE1_ROUTE	/* Write IDE1 INTR to IRQ15	*/

	lda	r16, 0x70(r31)		/* 	*/
	bsr	r29, WriteCY_SA_USB_CTRL/* Write Stand-alone/USB Control */

	bne	r31, 1f			// push prediction stack
1:	bis	r26, 1, r26		//      pal mode
	hw_rets/jmp	(r26)		/* All synced up		*/

/*=======================================================================
 *= wait_n_cycles - Waits for the specified number of cycles to pass.	=
 *=======================================================================
 * OVERVIEW:
 *      This function will wait at least N cycles to have passed by, where
 *	N is a value specified by the caller.  The function will return the
 *	actual number of cycles it waited which sometimes may be a few more
 *	cycles than asked for.
 *
 * FORM OF CALL:
 *       bsr r27, wait_n_cycles
 *
 * ARGUMENTS:
 *	r16 - number of cycles to wait.
 *
 * RETURNS:
 *	r0 - Actual number of cycles transpired.
 *
 * REGISTERS:
 *	r0 - Current cycle count.
 *	r1 - Correction value for overflows.
 *	r2 - Scratch.
 *	r3 - Initial cycle count.
 *
 */
wait_n_cycles:
#ifndef ISP_SIMULATION
	lda	r1, 1(r31)		/* Value to add for correction	*/
	sll	r1, 32, r1		/* when there's a ctr overflow.	*/
	rpcc	r3			/* Read the initial cycle count */
time_loop:
	rpcc	r0			/* Read current count.		*/
	subl	r0, r3, r0		/* Get delta time.		*/
	bge 	r0, nowrap		/* Check for counter overflow.	*/
	addq	r1, r0, r0		/* Correct for overflow.	*/
nowrap:
	cmpult	r0, r16, r2		/* Have we reached our delta?	*/
	bne 	r2, time_loop		/* No, then loop again.		*/
#endif /* ISP_SIMULATION */
	bne	r31, 1f			// push prediction stack
1:	bis	r27, 1, r27		//      pal mode
	hw_rets/jmp	(r27)		/* All synced up		*/

convert_ppd:
	and	r17, 0x7, r2		/* r2 = PD[3:1]			*/
	addq	r2, 3, r2		/* r2 = PD[3:1] + 3		*/
	and	r17, 8, r3		/* Mask off PD[4]		*/
	xor	r3, 8, r4		// split array
	srl	r3, 3, r3		/* r3 = PD[4]			*/
	subq	r2, r3, r2		/* r2 = 3 + PD[3:1] - PD[4]	*/
	sll	r4, 1, r4
	bis	r2, r4, r2		// all in one byte
	bne	r31, 1f			// push prediction stack
1:	bis	r29, 1, r29		//      pal mode
	hw_rets/jmp	(r29)		/* All synced up		*/


#define  iic_cks    1
#define  iic_ds     2
#define  iic_ckr    4
#define  iic_dr     8

//++
// ===========================================================================
// = srom_iic_read_byte    - read a byte from PC164LX IIC bus  =
// ===========================================================================
//
// OVERVIEW:
//
//  Read a byte from the PC164LX IIC bus, assuming the bus has been
//  prepared for this operation previously.
//
// FORM OF CALL:
//
//	bsr	r29,srom_iic_read_byte
//
// ARGUMENTS:
//     none
//
// RETURNS:
//     R3 = byte read from IIC bus
//
// IIC BUS AT ROUTINE EXIT:
//     scl=0, sda=tristate
//
// FUNCTIONAL DESCRIPTION:
//
//
// CALLS:
//
//
// CONSTANTS
//
//
// REGISTERS:
//
//   r2 - loop counter for 8 bits
//   r3 - byte being read off IIC bus
//   r4 - IIC bus state
//
// ALGORITHM:
//
//
//--
// address of IIC rom is  '1010.aaa.d' where a=addr, d=direction=0-W,1-R
//
srom_iic_read_byte:
	load_csr_base(r6, CCHIP_CSR)
	lda	r6, CSR_MPD(r6)
		
	lda		r2, 8(r31)		//read 8 bits
	bis		r31, r31, r3	//accumulate in r3
read_more_bits:
	sll	r3, 1, r3		//make room for new data
	lda	r4, (iic_ds)(r31)		//scl=0, sda=1
	stl_p	r4, 0(r6)		// ds=0 cks= 0
	mb						//
	USDELAY	(50)			//wait 1 bit time
	lda	r4, (iic_ds|iic_cks)(r31)	//scl=1, sda=1
	stl_p	r4, 0(r6)		//
	mb				//
	USDELAY	(50)			//wait 1 bit time
	ldl_p	r4,  0(r6)		//get data from slave
	srl	r4, 3, r4		//mask off other junk
	bis	r4, r3, r3		//
	mb				//
	USDELAY	(50)			//wait 1 bit time
	lda	r4, (iic_ds)(r31)		//scl=0, sda=1
	stl_p	r4, 0(r6)		// ds=0 cks= 0
	mb						//
	USDELAY	(50)			//wait 1 bit time
	subl	r2, 1, r2		//decrement counter
	bne	r2, read_more_bits			//

	stl_p	r31, 0(r6)		// ds=0 cks= 0
        mb                                              //
        USDELAY (50)                    //wait 1 bit time
        lda     r4, (iic_cks)(r31)       //scl=1, sda=0
        stl_p   r4, 0(r6)              //
        mb                              //
        USDELAY (100)                    //wait 1 bit time
	stl_p	r31, 0(r6)		// ds=0 cks= 0
        mb                                              //
        USDELAY (50)                    //wait 1 bit time

	bne	r31, 1f			// push prediction stack
1:	bis	r29, 1, r29		//      pal mode
	hw_rets/jmp	(r29)		/* All synced up		*/

//++
// ===========================================================================
// = srom_iic_read_last_byte    - read a byte from PC164LX IIC bus  =
// ===========================================================================
//
// OVERVIEW:
//
//  Read a byte from the PC164LX IIC bus, assuming the bus has been
//  prepared for this operation previously.
//
// FORM OF CALL:
//
//	bsr	r29,srom_iic_read_last_byte
//
// ARGUMENTS:
//     none
//
// RETURNS:
//     R3 = byte read from IIC bus
//
// IIC BUS AT ROUTINE EXIT:
//     scl=0, sda=tristate
//
// FUNCTIONAL DESCRIPTION:
//
//
// CALLS:
//
//
// CONSTANTS
//
//
// REGISTERS:
//
//   r2 - loop counter for 8 bits
//   r3 - byte being read off IIC bus
//   r4 - IIC bus state
//
// ALGORITHM:
//
//
//--
// address of IIC rom is  '1010.aaa.d' where a=addr, d=direction=0-W,1-R
//
srom_iic_read_last_byte:
	load_csr_base(r6, CCHIP_CSR)
	lda	r6, CSR_MPD(r6)
		
	lda		r2, 8(r31)		//read 8 bits
	bis		r31, r31, r3	//accumulate in r3
last_read_more_bits:
	sll	r3, 1, r3		//make room for new data
	lda	r4, (iic_ds)(r31)		//scl=0, sda=1
	stl_p	r4, 0(r6)		// ds=0 cks= 0
	mb						//
	USDELAY	(50)			//wait 1 bit time
	lda	r4, (iic_ds|iic_cks)(r31)	//scl=1, sda=1
	stl_p	r4, 0(r6)		//
	mb				//
	USDELAY	(50)			//wait 1 bit time
	ldl_p	r4,  0(r6)		//get data from slave
	srl	r4, 3, r4		//mask off other junk
	bis	r4, r3, r3		//
	mb				//
	USDELAY	(50)			//wait 1 bit time
	lda	r4, (iic_ds)(r31)		//scl=0, sda=1
	stl_p	r4, 0(r6)		// ds=0 cks= 0
	mb						//
	USDELAY	(50)			//wait 1 bit time
	subl	r2, 1, r2		//decrement counter
	bne	r2, last_read_more_bits			//
//	write noack
	lda	r4, (iic_ds)(r31)	//scl=0, sda=1
	stl_p	r4, 0(r6)		// ds=1 cks= 0
        mb                                              //
        USDELAY (50)                    //wait 1 bit time
        lda     r4, (iic_ds | iic_cks)(r31)       //scl=1, sda=1
        stl_p   r4, 0(r6)              //
        mb                              //
        USDELAY (100)                    //wait 1 bit time
	lda	r4, (iic_ds)(r31)		//scl=0, sda=1
	stl_p	r4, 0(r6)		// ds=0 cks= 0
        mb                                              //
        USDELAY (50)                    //wait 1 bit time

	bne	r31, 1f			// push prediction stack
1:	bis	r29, 1, r29		//      pal mode
	hw_rets/jmp	(r29)		/* All synced up		*/

//++
// ===========================================================================
// = srom_iic_write_byte    - write a byte to PC164LX IIC bus   =
// ===========================================================================
//
// OVERVIEW:
//
// Writes a byte to the PC164LX IIC bus.
//
// FORM OF CALL:
//	bsr	r29,srom_iic_write_byte
//
// ARGUMENTS:
// R3 - byte to xmit
//
// RETURNS:
// R3 - IIC bus ACK in bit 3  (0-ACK, 1-NO ACK)
//
// IIC BUS AT ROUTINE EXIT:
// scl=0, sda=tristate
//
// FUNCTIONAL DESCRIPTION:
//
//
// CALLS:
//
//
// CONSTANTS
//
//
// REGISTERS:
//
//   r1 - address of IIC ctrl register
//   r2 - loop counter for 8 bits
//   r3 - byte being written to IIC bus
//   r4 - IIC bus state
//
// ALGORITHM:
//
//
//--
// link      -- r29
// scratch   -- r1, r4, r2
// Input     -- R3 = byte to xmit
// Output    -- R3 = ACK status in bit 3
// IIC @exit -- 
//
//
// address of IIC rom is  '1010.aaa.d' where a=addr, d=direction=0-W,1-R
//
srom_iic_write_byte:
	load_csr_base(r6, CCHIP_CSR)
	lda	r6, CSR_MPD(r6)
		
					//put low bit in good spot for xmit
	bis		r31, 8, r2	//loop counter in r2
write_more_bits:
	srl	r3, 6, r4		//make room for new data
	and	r4, iic_ds, r4		// only data bit
	stl_p	r4, 0(r6)		// ds=data cks= 0
	mb						//
	USDELAY	(50)			//wait 1 bit time
	bis	r4, iic_cks, r4		// data bit & clk high
	stl_p	r4, 0(r6)		// ds=data cks= 0
	mb						//
	USDELAY	(100)			//wait 1 bit time
	and	r4, iic_ds, r4		// only data bit
	stl_p	r4, 0(r6)		// ds=data cks= 0
	mb						//
	USDELAY	(50)			//wait 1 bit time
	sll	r3, 1, r3		// next bit
	subl	r2, 1, r2		//decrement counter
	bne	r2, write_more_bits

	bis	r31, iic_ds, r4		// only data bit
	stl_p	r4, 0(r6)		// ds=data cks= 0
	mb						//
	USDELAY	(50)			//wait 1 bit time
	bis	r4, iic_cks, r4		// data bit & clk high
	stl_p	r4, 0(r6)		// ds=data cks= 0
	mb						//
	USDELAY	(50)			//wait 1 bit time
	ldl_p	r3,  0(r6)		//get data from slave
	mb				//
	USDELAY	(50)			//wait 1 bit time
	stl_p	r31, 0(r6)		// ds=data cks= 0
	mb						//
	USDELAY	(50)			//wait 1 bit time
	srl	r3, 3, r3		//
	bne	r31, 1f			// push prediction stack
1:	bis	r29, 1, r29		//      pal mode
	hw_rets/jmp	(r29)		/* All synced up		*/
		

//++
// ===========================================================================
// = srom_iic_start  -  Assert IIC start state on PC164LX IIC bus =
// ===========================================================================
//
// OVERVIEW:
//
// Seize control of PC164LX IIC bus by asserting start state.
//
// FORM OF CALL:
//
//	bsr	r29,srom_iic_start
//
// ARGUMENTS:
//   None
//
// RETURNS:
//   None
//
// IIC BUS AT ROUTINE EXIT:
//   scl=1, sda=0
// 
// FUNCTIONAL DESCRIPTION:
//
//
// CALLS:
//
//
// CONSTANTS
//
//
// REGISTERS:
//
//   r1 - address of IIC ctrl register
//   r4 - IIC bus state
//
// ALGORITHM:
//
//
//--
// address of IIC rom is  '1010.aaa.d' where a=addr, d=direction=0-W,1-R
//
srom_iic_start:
	load_csr_base(r6, CCHIP_CSR)
	lda	r6, CSR_MPD(r6)
		
	lda	r4, (iic_ds|iic_cks)(r31)	//scl=1, sda=1
	stl_p	r4, 0(r6)			//
	mb				//
	USDELAY (350)			//wait 3/2 bit time
	lda	r4, (iic_cks)(r31)	//scl=1, sda=0
	stl_p	r4, 0(r6)		//
	mb				//
	USDELAY	(100)			//wait 1/2 bit time
	stl_p	r31, 0(r6)		//
	mb				//
	USDELAY	(50)			//wait 1/2 bit time
	bne	r31, 1f			// push prediction stack
1:	bis	r29, 1, r29		//      pal mode
	hw_rets/jmp	(r29)		/* All synced up		*/
	

//++
// ===========================================================================
// = srom_iic_stop  - assert stop state on PC164LX IIC bus  =
// ===========================================================================
//
// OVERVIEW:
//   Seize control of IIC bus by asserting stop state on IIC bus.
//
//
// FORM OF CALL:
//
//	bsr	r29,srom_iic_stop
//
// ARGUMENTS:
//   none
//
// RETURNS:
//   none
//
// IIC BUS AT ROUTINE EXIT:
//   scl=sda=1 (bus idle)
//
// FUNCTIONAL DESCRIPTION:
//
//
// CALLS:
//
//
// CONSTANTS
//
//
// REGISTERS:
//   r1 - address of IIC ctrl register
//   r4 - IIC bus state
//
// ALGORITHM:
//
//
//--
// address of IIC rom is  '1010.aaa.d' where a=addr, d=direction=0-W,1-R
//
srom_iic_stop:

	load_csr_base(r6, CCHIP_CSR)
	lda	r6, CSR_MPD(r6)
		
	stl_p	r31, 0(r6)			// clk low data low
	USDELAY	(50)				//wait 1/2 bit time
	lda	r4, (iic_cks)(r31)	//scl=1
	stl_p	r4, 0(r6)		// data line as it was before
	mb				//
	USDELAY	(100)			//wait 1/2 bit time
	stl_p	r31, 0(r6)			// clk low data low
	USDELAY	(100)				//wait 1/2 bit time
	lda	r4, (iic_cks)(r31)	//scl=1
	stl_p	r4, 0(r6)		// data line as it was before
	mb				//
	USDELAY	(150)			//wait 1/2 bit time
	lda	r4, (iic_ds|iic_cks)(r31)	//scl=1, sda=1
	stl_p	r4, 0(r6)		//
	mb				//
	USDELAY	(150)			//wait 1 bit time
	bne	r31, 1f			// push prediction stack
1:	bis	r29, 1, r29		//      pal mode
	hw_rets/jmp	(r29)		/* All synced up		*/

//++
// ===========================================================================
// = srom_iic_read_rom - read data from IIC ROM device  =
// ===========================================================================
//
// OVERVIEW:
//    Complete routine to read a byte or more of data from an IIC ROM
//    slave device given the slave's IIC address and the offset into 
//    the slave of the byte to be read.
//
// FORM OF CALL:
//
//	bsr	r9,srom_iic_read_rom
//
// ARGUMENTS:
//   r5 - IIC address (0-7)
//   r7 - offset of desired byte (0-255)
//
// RETURNS:
//   r5 - byte read from ROM
//
// IIC BUS AT ROUTINE EXIT:
//
//
// FUNCTIONAL DESCRIPTION:
//
//
// CALLS:
//    iic_write_byte
//    iic_start
//    iic_read_byte
//    iic_master_ack
//    iic_stop
//
//
// CONSTANTS
//
//
// REGISTERS:
//
//   r5 - IIC address (0-7)
//   r7 - offset of desired byte (0-255)
//
// ALGORITHM:
//
//
//--
// address of IIC rom is  '1010.aaa.d' where a=addr, d=direction=0-W,1-R
//
srom_iic_read_rom:
	bsr	r29, srom_iic_start		//
	and	r5, 0xe, r3		// mask off all but [2:0]
	bis	r3, 0xa0, r3		// address EEPROM device type, dir=W
	bsr	r29, srom_iic_write_byte	//
	bis	r31, r3, r8

	zap	r7, 0xfe, r3		// Send offset to be read, no dir
	bsr	r29, srom_iic_write_byte	//
	bis	r3, r8, r8

	bsr	r29, srom_iic_start		//

	and	r5, 0xe, r3		// mask off all but [2:0]
	bis	r3, 0xa1, r3		// address EEPROM device type, dir=R
	bsr	r29, srom_iic_write_byte	//
	bis	r3, r8, r8

	bne	r8, srom_iic_read_rom_err       // check return stat 
	bsr	r29, srom_iic_read_byte	// read a byte from iic device


	bne	r31, 1f			// push prediction stack
1:	bis	r9, 1, r9		//      pal mode
	hw_rets/jmp	(r9)		/* All synced up		*/

//--------------------------------------------------------------------
srom_iic_read_rom_err:
	// jjd - IIC bus error halts query, resets bus, 
	// and returns bad status
	bsr	r29, srom_iic_stop		//
	lda	r3, 0x7fff(r31)		// return IIC bus error code
	bne	r31, 1f			// push prediction stack
1:	bis	r9, 1, r9		//      pal mode
	hw_rets/jmp	(r9)		// nothing for now
//--------------------------------------------------------------------

srom_iic_configure:
        lda     r7, 2(r31)              	// pd_byte
        bsr     r9, srom_iic_read_rom  	// byte returned in r3
	cmpeq	r3, 0x04, r2			// must be sdram
	beq	r2,  return_error		//
	bsr	r29, srom_iic_read_byte		// read 3rd byte from iic device
	cmplt	r3, 0x0b, r2			// must # of rows must be less than 13
	bne	r2,  return_error               //
	subq	r3, 0xb, r8			// number of rows
	cmplt	r8, 0x4, r2			// r8 must be less than 4
	beq	r2, return_error               //
	bsr     r29, srom_iic_read_byte         // read 4th byte from iic device
	addq	r8, r3, r9			// rows + cols
	bsr     r29, srom_iic_read_byte         // read 5th byte from iic device
	addq	r9, r3, r9			// rows + cols + banks
	and	r3, 2, r3			// split array
	s4addl	r3, r8, r8			// split array to bit 3
	bsr     r29, srom_iic_read_byte         // read 6th byte from iic device
	bsr     r29, srom_iic_read_byte         // read 7th byte from iic device
        bne     r3,  return_error               //
	bsr     r29, srom_iic_read_byte         // read 8th byte from iic device
        cmpeq   r3, 0x01, r2                    // must # of rows must be less than 13
        beq     r2,  return_error               //
	bsr     r29, srom_iic_read_byte         // read 9th byte from iic device
	bsr     r29, srom_iic_read_byte         // read 10th byte from iic device
	bsr     r29, srom_iic_read_byte         // read 11th byte from iic device
	bsr     r29, srom_iic_read_byte         // read 12th byte from iic device
	bsr     r29, srom_iic_read_byte         // read 13th byte from iic device
	bsr     r29, srom_iic_read_byte         // read 14th byte from iic device
	bsr     r29, srom_iic_read_byte         // read 15th byte from iic device
	bsr     r29, srom_iic_read_byte         // read 16th byte from iic device
	bsr     r29, srom_iic_read_byte         // read 17th byte from iic device
	and	r3, 4, r3			// bank bits 
	addq	r3, r8, r8			// split banks size rows size
	srl	r3, 2, r3			// make 4 = and 2 = 0
	addq	r9, r3, r9			// increase size
        bsr     r29, srom_iic_read_last_byte    // read a byte from iic device
        bsr     r29, srom_iic_stop              //
	subq	r9, 7, r2			// array size
	sll	r8, 4, r8			//
	bis	r2, r8, r2			// all in a byte
	bne	r31, 1f			// push prediction stack
1:	bis	r28, 1, r28		//      pal mode
	hw_rets/jmp	(r28)		/* All synced up		*/


return_error:
	bsr	r29, srom_iic_read_last_byte	// read a byte from iic device
	bsr	r29, srom_iic_stop		//
	bis	r31, r31, r2			//
	bis	r31, r31, r8			// number of rows
	bne	r31, 1f			// push prediction stack
1:	bis	r28, 1, r28		//      pal mode
	hw_rets/jmp	(r28)		/* All synced up		*/

//+
//=============================================================================
// = srom_usdelay  - delay (n) microsecond					      =
// =============================================================================
//
// OVERVIEW:
//   
//	Provides a one microsecond delay
//
// FORM OF CALL:
//
//	bsr r27, srom_usdelay
//	
// RETURNS:
//
//	none
//
// ARGUMENTS:
//
//	r16	- number of microseconds to delay
//	r22	- CPU speed in psec
//
// REGISTERS:
//
//	r16	- number of microseconds to delay
//	r20	- scratch
//	r27	- return address
//	r22	- CPU speed in psec
//
// CONSTANTS:
// 
//	CPU_MHZ	- default CPU speed
//  
// SIDE EFFECTS:
//
//	none
//
//--

usdelay:						// called using r27
srom_usdelay:
	bis	r31, r31, r31
	//LDLI	(r17, 1000000)
	//bis		r22, r22, r16		// CPU speed in psec
	//bsr		r26, nintdivide		//
	lda		r20, 6667(r31)		// hard code for now
	mulq	r20, r16, r20			// total usec count
	bis	r31, r31, r31
usdelay_init:
	rpcc	r16					// : read the cycle counter
	bis	r31, r31, r31
	addq	r20, r16, r20
	zapnot	r20, 0x0f, r20		// : clear the high longword
usdelay_loop:				// REPEAT
	rpcc	r16			// : read the cycle counter
	zapnot	r16, 0x0f, r16		// : clear the high longword
	cmplt	r20, r16, r16		// : 
	beq	r16, usdelay_loop	// UNTIL we have waited time specified
	bne	r31, 1f			// push prediction stack
1:	bis	r27, 1, r27		//      pal mode
	hw_rets/jmp	(r27)		/* All synced up		*/

#ifdef LCD

//	return address 27
srom_iic_wait_for_idle:
        lda     r3, 0x1000(r31)
iic_wait_for_loop:				//
        ldbu    r5, 1(r0)			//
        ldbu    r2, 0(r1)			//
        subq    r3, 1, r3                       //
        beq     r3, iic_idle                    //
        blbc    r5, iic_wait_for_loop		//
iic_idle:                                       //
	mb					// lets stop spec stores
	bne	r31, 1f			// push prediction stack
1:        bis     r27, 1, r27                   //      pal mode
        hw_rets/jmp (r27)                   /* All synced up                */



//	return address 27
iic_wait_for_pin:				// Wait for bus to become idle

	ldah    r3, 0x7(r31)
iic_wait_for_pin_loop:
	ldbu    r4, 1(r0)
	ldbu    r2, 0(r1)
	subq    r3, 1, r3
	beq     r3, iic_pin
	srl     r4, 7, r2
	blbs    r2, iic_wait_for_pin_loop
iic_pin: 
	mb					// lets stop spec stores
	bne	r31, 1f			// push prediction stack
1:        bis     r27, 1, r27                     //      pal mode
        hw_rets/jmp (r27)                   /* All synced up                */

iic_set_addresses:
	ldah    r0, 0x4008(r31)
	lda     r0, 0xffc0(r0)
	sll     r0, 13, r0
	lda	r4, 0x8000(r31)		/* Kseg = 0xffff8000.00000000	*/
	sll	r4, 32, r4		/* Shift into place		*/
	ldah    r1, 0x4010(r31)
	lda     r1, 0xe000(r1)
	sll     r1, 13, r1
	lda     r1, 0x80(r1)
	addq	r0, r4, r0
	addq	r1, r4, r1
	mb					// lets stop spec stores
	bne	r31, 1f			// push prediction stack
1:        bis     r27, 1, r27                     //      pal mode
        hw_rets/jmp (r27)                   /* All synced up                */


//       enter with r16 the addr
//       enter with r17 the data
//	return address 28

srom_write_word:
iic_write_word:
	bsr	r27, srom_iic_wait_for_idle
	and     r16, 0xfe, r16
	mb					// lets stop spec stores
	stb     r16, 0(r0)
        ldbu    r2, 0(r1)
	bis     r31, 0xc5, r16
	bis	r31, r31, r31
	bis	r31, r31, r31
	bis	r31, r31, r31
	mb
        bis     r31, r31, r31
        bis     r31, r31, r31
        bis     r31, r31, r31

	stb     r16, 1(r0)
        ldbu    r2, 0(r1)
        bsr     r27, iic_wait_for_pin
	mb
	stb     r17, 0(r0)
        ldbu    r2, 0(r1)
        bsr     r27, iic_wait_for_pin
        bis     r31, 0xc3, r16
	mb
	stb     r16, 1(r0)
        ldbu    r2, 0(r1)
	mb					// lets stop spec stores
	bne	r31, 1f			// push prediction stack
1:        bis     r28, 1, r28                     //      pal mode
        hw_rets/jmp (r28)                   /* All synced up                */

//       enter with r16 control
//       enter with r17 contrast | rs
//	return address 26
srom_access_lcd:
	bis     r31, r17, r6
	bsr	r27, iic_set_addresses
	bis     r31, r16, r17
	bis	r31, 0x42, r16
	bsr	r28, srom_write_word
	bis	r31, 0x4, r16
	bsr	r27, usdelay
	bis     r6, 0x60, r17
	bis	r31, 0x40, r16
	bsr	r28, srom_write_word
	bis	r31, 0x4, r16
	bsr	r27, usdelay
	bis     r6, 0x40, r17
	bis	r31, 0x40, r16
	bsr	r28, srom_write_word
	bis	r31, 0x4, r16
	bsr	r27, usdelay
	bis     r31, r16, r17
	bis     r6, 0x60, r17
	bis	r31, 0x40, r16
	bsr	r28, srom_write_word
	bis	r31, 0x4, r16
	bsr	r27, usdelay
	mb					// lets stop spec stores
	bne	r31, 1f			// push prediction stack
1:        bis     r26, 1, r26                     //      pal mode
        hw_rets/jmp (r26)                   /* All synced up                */

//	return address r7
srom_init_lcd:
	ldah	r0, 0x4010(r31)
	lda     r0, 0xf000(r0)
	sll     r0, 13, r0
	lda	r4, 0x8000(r31)		/* Kseg = 0xffff8000.00000000	*/
	sll	r4, 32, r4		/* Shift into place		*/
	addq	r0, r4, r0
	lda	r0, 0x2800(r0)
	bis	r31, 0xf, r1
	mb					// lets stop spec stores
	stb	r1, CY_COMMAND_REG(r0)
	mb
	bis	r31, 0x80, r1
	stb	r1, CY_INTA_ROUTE(r0)
	mb
	bis	r31, 0x80, r1
	stb	r1, CY_INTB_ROUTE(r0)
	mb
	bis	r31, 0x80, r1
	stb	r1, CY_INTC_ROUTE(r0)
	mb
	bis	r31, 0x80, r1
	stb	r1, CY_INTD_ROUTE(r0)
	mb
	bis	r31, 0x27, r1
	stb	r1, CY_CONTROL_REG(r0)
	mb
	bis	r31, 0xe0, r1
	stb	r1, CY_ERR_CONTROL_REG(r0)
	mb
	bis	r31, 0xe0, r1
	stb	r1, CY_BIOS_CONTROL_REG(r0)
	mb
	bis	r31, 0xf0, r1
	stb	r1, CY_TOP_OF_MEM(r0)
	mb
	bis	r31, 0x40, r1
	stb	r1, CY_ATCTRL_REG1(r0)
	mb
	bis	r31, 0x80, r1
	stb	r1, CY_IDE0_ROUTE(r0)
	mb
	bis	r31, 0x80, r1
	stb	r1, CY_IDE1_ROUTE(r0)
	mb
	bis	r31, 0x70, r1
	stb	r1, CY_SA_USB_CTRL(r0)
	mb

	bsr	r27, iic_set_addresses
	bsr	r27, srom_iic_wait_for_idle

	bis     r31, 0x80, r16
	mb					// lets stop spec stores
	stb     r16, 1(r0)
        ldbu    r2, 0(r1)
	bis	r31, 0x40, r16
	bsr	r27, usdelay

	bis     r31, 0x0, r16
	mb					// lets stop spec stores
	stb     r16, 1(r0)
        ldbu    r2, 0(r1)
	bis	r31, 0x40, r16
	bsr	r27, usdelay

	bis     r31, 0x5b, r16
	mb					// lets stop spec stores
	stb     r16, 0(r0)
        ldbu    r2, 0(r1)
	bis     r31, 0x20, r16
	bis	r31, r31, r31
	bis	r31, r31, r31
	bis	r31, r31, r31
	mb
        bis     r31, r31, r31
        bis     r31, r31, r31
        bis     r31, r31, r31
	stb     r16, 1(r0)
        ldbu    r2, 0(r1)
        bsr     r27, iic_wait_for_pin


	bis     r31, 0x14, r16
	mb					// lets stop spec stores
	stb     r16, 0(r0)
        ldbu    r2, 0(r1)
	bis     r31, 0xc3, r16
	bis	r31, r31, r31
	bis	r31, r31, r31
	bis	r31, r31, r31
	mb
        bis     r31, r31, r31
        bis     r31, r31, r31
        bis     r31, r31, r31

	stb     r16, 1(r0)
        ldbu    r2, 0(r1)
	bis	r31, 0x4, r16
	bsr	r27, usdelay


	bis     r31, 0x38, r16
	bis     r31, r31, r17
	bsr	r26, srom_access_lcd
	bis     r31, 0x38, r16
	bis     r31, r31, r17
	bsr	r26, srom_access_lcd
	bis     r31, 0x38, r16
	bis     r31, r31, r17
	bsr	r26, srom_access_lcd
	bis     r31, 0x38, r16
	bis     r31, r31, r17
	bsr	r26, srom_access_lcd
	bis     r31, 0x0e, r16
	bis     r31, r31, r17
	bsr	r26, srom_access_lcd
	bis     r31, 0x02, r16
	bis     r31, r31, r17
	bsr	r26, srom_access_lcd
	bis     r31, 0x01, r16
	bis     r31, r31, r17
	bsr	r26, srom_access_lcd

	bis     r31, 0x80, r16
	bis     r31, r31, r17
	bsr	r26, srom_access_lcd

	mb					// lets stop spec stores
	bne	r31, 1f			// push prediction stack
1:        bis     r7, 1, r7                     //      pal mode
        hw_rets/jmp (r7)                   /* All synced up                */

#endif
/*=======================================================================
 * = WriteLEDS - Writes value to led port.				=
 * = WriteFBank - Writes value to the flash bank port.			=
 *=======================================================================
 * OVERVIEW:
 *      Writes specified value to the led port.
 *      Writes specified value to the flash bankport.
 *	And several other ports.
 *
 * FORM OF CALL:
 *       bsr r29, WriteLEDS
 *       bsr r29, WriteFBank
 *
 * ARGUMENTS:
 *	r2 or r16 - Byte value to write to port.
 *
 * RETURNS:
 *	r1 - Address of output port.
 *
 * REGISTERS:
 *	r1-r3 - Scratch.
 */
WriteCY_CMD_REG:
	lda	r1, CY_COMMAND_REG(r31) /* CY's Command Register.	*/
	br	r31, WriteConf

WriteCY_INTA_ROUTE:
	lda	r1, CY_INTA_ROUTE(r31) /* CY's INTA routing.	*/
	br	r31, WriteConf

WriteCY_INTB_ROUTE:
	lda	r1, CY_INTB_ROUTE(r31) /* CY's INTB routing.	*/
	br	r31, WriteConf

WriteCY_INTC_ROUTE:
	lda	r1, CY_INTC_ROUTE(r31) /* CY's INTC routing.	*/
	br	r31, WriteConf

WriteCY_INTD_ROUTE:
	lda	r1, CY_INTD_ROUTE(r31) /* CY's INTD routing.	*/
	br	r31, WriteConf

WriteCY_CTRL_REG:
	lda	r1, CY_CONTROL_REG(r31) /* CY's Control Register.	*/
	br	r31, WriteConf

WriteCY_ERR_CTRL_REG:
	lda	r1, CY_ERR_CONTROL_REG(r31) /* CY's Control Register.	*/
	br	r31, WriteConf

WriteCY_TOP_OF_MEM:
	lda	r1, CY_TOP_OF_MEM(r31) /* CY's Top of Memory register.	*/
	br	r31, WriteConf

WriteCY_ATCTRL_REG1:
	lda	r1, CY_ATCTRL_REG1(r31) /* CY's AT Control Register 1.	*/
	br	r31, WriteConf

WriteCY_IDE0_ROUTE:
	lda	r1, CY_IDE0_ROUTE(r31) /* CY's IDE0 INTR Routing.	*/
	br	r31, WriteConf

WriteCY_IDE1_ROUTE:
	lda	r1, CY_IDE1_ROUTE(r31) /* CY's IDE1 INTR Routing.	*/
	br	r31, WriteConf

WriteCY_SA_USB_CTRL:
	lda	r1, CY_SA_USB_CTRL(r31) /* CY's IDE1 INTR Routing.	*/
	br	r31, WriteConf

WriteConf:
	bis	r16, r31, r2
	LDLI(r3, PCI0_CONFIG, r31)
	sll	r3, 24, r3
	lda	r4, 0x8000(r31)
	sll	r4, 32, r4
	bis	r3, r4, r3
	lda	r16, (5<<11)(r31)	/* Cypress has device ID = 5	*/
	bis	r3, r16, r3		/* Add it to config address.	*/
	br	r31, WriteData

WriteLEDS:
	cmpeq	r2, 0x20, r1		/* If ledcode value is 0x20,	*/
	bne	r1, skipSromPort	/* then skip srom write.	*/

	bis	r29, r29, r1		/* Save r29			*/
	PRINTC(a_period)
	PRINTB(r2)
	bis	r1, r1, r29		/* Restore r29			*/
skipSromPort:
	lda	r1, LED_PORT(r31)	/* POST ledcard port.		*/
	br	r31, WriteCont

WriteSMC_Config1:
WriteSMC_Index1:
	lda	r1, SMC_ADDRESS_PORT1(r31) /* SMC's configuration port 1*/
	br	r31, WriteCont_std	/* SMC's index port 1.		*/
WriteSMC_Data1:
	lda	r1, SMC_DATA_PORT1(r31)	/* SMC's data port 1.		*/
	br	r31, WriteCont_std

WriteRTCAddr:
	lda	r1, RTC_ADDRESS_PORT(r31) /* Real Time Clock address port*/
	br	r31, WriteCont_std
WriteRTCData:
	lda	r1, RTC_DATA_PORT(r31)	/* Real Time Clock data port.	*/
	br	r31, WriteCont_std
WriteSIO_NMISC:
	lda	r1, SIO_NMISC_PORT(r31)	/* SIO's NMI status and control	*/
	br	r31, WriteCont_std
WriteSIO_TMR2:
	lda	r1, SIO_TIMER2_PORT(r31) /* SIO's timer 2.		*/
	br	r31, WriteCont_std
WriteSIO_TCW:
	lda	r1, SIO_TCW_PORT(r31)	/* SIO's timer control word.	*/

WriteCont_std:
	bis	r16, r16, r2		/* Copy to R2.			*/
WriteCont:
	lda	r3, 0x8000(r31)
	sll	r3, (32-24), r3
	LDLI(r3, PCI0_IO, r3)
	sll	r3, 24, r3

WriteData:
	bis	r1, r3, r1		/* Add port to base address.	*/
	mb
	stb	r2, 0x00(r1)		/* send the byte to the port	*/
	mb				/* make sure it gets there	*/
	bne	r31, 1f			// push prediction stack
1:	bis	r29, 1, r29		//      pal mode
	hw_rets/jmp	(r29)		/* All synced up		*/


/*=======================================================================
 * = ReadRTCData - Reads a byte from the RTC Data port.			=
 *=======================================================================
 * OVERVIEW:
 *      Reads a byte from the RTC Data port.
 *	Reads a byte from the configuration register 1.
 *	Reads a byte from the configuration register 2.
 *
 * FORM OF CALL:
 *       bsr r29, ReadRTCData
 *
 * ARGUMENTS:
 *	None.
 *
 * RETURNS:
 *	r0 - Data read from port contained in byte 0.
 *
 * REGISTERS:
 *	r1 - Address of port to load from.  Scratch.
 *	r2 - Computed CPU address for I/O port.
 *	r3 - CPU address for base of I/O space.
 */
ReadRTCData:
	lda	r2, RTC_DATA_PORT(r31)	/* Real Time Clock data port.	*/
	lda	r3, 0x8000(r31)
	sll	r3, (32-24), r3
	LDLI(r3, PCI0_IO, r3)
	sll	r3, 24, r3

ReadData:
	bis	r2, r3, r2		/* Add port to base address.	*/
	ldbu	r0, 0x00(r2)		/* Load LW containing the byte.	*/
	bne	r31, 1f			// push prediction stack
1:	bis	r29, 1, r29		//      pal mode
	hw_rets/jmp	(r29)		/* All synced up		*/


/*=======================================================================
 * = ReadConfJumper - Reads the configuration jumpers.			=
 *=======================================================================
 * OVERVIEW:
 *	Reads a byte from the configuration jumpers.
 *
 * FORM OF CALL:
 *       bsr r29, ReadConfJumper
 *
 * ARGUMENTS:
 *	None.
 *
 * RETURNS:
 *	r0 - Data read from int_req<31:24> in byte 0.
 *
 * REGISTERS:
 *	r3 - CPU address for Pyxis INT_REQ register.
 */
ReadConfJumper:
#if 0
	load_csr_base(r3)		/* Base for config registers.	*/
	ldah	r3, 0x6000(r3)	/* Base of Interrupt Control Registers */
	ldl_p	r0, MC_ICR_INT_REQ(r3)	/* Get conf bits <31:24> */
	srl		r0, 24, r0
	zap		r0, 0xFE, r0
#else
	bis	r31, r31, r0
#endif
	bne	r31, 1f			// push prediction stack
1:	bis	r29, 1, r29		//      pal mode
	hw_rets/jmp	(r29)		/* All synced up		*/

/* ======================================================================
 *	intdivide - Performs Trivial Integer Division.
 * ======================================================================
 *	NOTE:  Fractional part will be chopped off.
 *
 *	Since space is at a premium in the SROM let's
 *	use a very (stupid = simple) integer divide.
 *
 *	The quotient (result) of this integer divide
 *	operation is determined by...
 *
 *	1. Incrementing the quotient "A" beginning at 1.
 *	2. Testing the quotient A by multiplying it by
 *	   the divisor "C".
 *	3. Comparing the test result with the dividend "B".
 *
 *	Since A = B/C,
 *	Therefore (A*C) <= B
 *	Increase A until this expression is no longer True.
 *
 *	For Improved speed the test quotient is incremented
 *	exponentially by one bit shifts.  In this algorithm
 *	a 1 bit in the quotient (result) is determined with
 *	each iteration of the outer loop beginning with the
 *	most significant 1 bit.  The number of iterations of
 *	the inner loop is equal to the total number of shifts
 *	required to locate each 1 bit in the quotient.
 *
 * Inputs
 *	r16	Divisor
 *	r17	Dividend
 *	r27	return address
 * Outputs
 *	r0	Quotient
 * Modified
 *	r0,r1,r2,r3,r4
 */
intdivide:
	bis	r31, r31, r0	/* Clear result register		*/
	cmplt	r17, r16, r1	/* Is B < C?				*/
	bne	r1, divide_done	/* If True quotient = 0			*/
divide:
	lda	r1, 1(r31)	/* Init test quotient.			*/
divide1:
	addq	r0, r1, r4	/* r4 = Current test quotient		*/
	mulq	r4, r16, r2	/* Multiply by C to test		*/
	sll	r1, 1, r1	/* Pre-increment for next test quotient	*/
	cmplt	r2, r17, r3	/* Compare multiplied result to B	*/
	bne	r3, divide1	/* If (A*C)<B do another iteration	*/
	srl	r1, 2, r1	/* Undo unnecessary increments		*/
	addq	r0, r1, r0	/* Add increment to quotient		*/
	cmplt	r1, 2, r3	/* Did this iteration need an increment */
	beq	r3, divide	/* If no increment required then done.	*/
divide_done:
	bne	r31, 1f			// push prediction stack
1:	bis	r27, 1, r27		//      pal mode
	hw_rets/jmp	(r27)		/* All synced up		*/


/* ======================================================================
 * = updivide - Perform Integer Division (Rounding Up fractional result)=
 * ======================================================================
 * Inputs
 *	r16	Divisor
 *	r17	Dividend
 *	r26	return address
 * Outputs
 *	r0	Quotient
 * Modified
 *	r0,r1,r2,r3,r4,27
 */
updivide:
	bsr	r27, intdivide	/* Do integer division	*/
	mulq	r16, r0, r1	/* Divisor * Quotient	*/
	cmplt	r1, r17, r2	/* Round up?		*/
	blbc	r2, updivide_done
	addq	r0, 1, r0	/* Round up result	*/
updivide_done:
	bne	r31, 1f			// push prediction stack
1:	bis	r26, 1, r26		//      pal mode
	hw_rets/jmp	(r26)		/* All synced up		*/


/* ======================================================================
 * = nintdivide - Perform Integer Division				=
 * = (Rounding frational result toward nearest integer)			=
 * ======================================================================
 *
 * Inputs
 *	r16	Divisor
 *	r17	Dividend
 *	r26	return address
 * Outputs
 *	r0	Quotient
 * Modified
 *	r0,r1,r2,r3,r4,27
 */
nintdivide:
	bsr	r27, intdivide	/* Do integer division	*/
	mulq	r16, r0, r1	/* Divisor * Quotient	*/
	cmplt	r1, r17, r2	/* Round up?		*/
	blbc	r2, nintdivide_done
	addq	r0, 1, r0	/* Round up result	*/
	mulq	r16, r0, r2	/* Divisor * Qoutient	*/
	subq	r2, r17, r2
	subq	r17, r1, r3
	cmplt	r3, r2, r1	/* Round back down?	*/
	blbc	r1, nintdivide_done
	subq	r0, 1, r0	/* Round down result	*/
nintdivide_done:
	bne	r31, 1f			// push prediction stack
1:	bis	r26, 1, r26		//      pal mode
	hw_rets/jmp	(r26)		/* All synced up		*/

/* ======================================================================
 * =  srom_wrtest - Tests a write to a location by reading+comparing.	=
 * ======================================================================
 *  OVERVIEW:
 * 	Test a register location with a write followed by a read/compare.
 *	This is useful when writing a register in an external device.
 *	Reading the data back ensures that it has gotten to the
 *	device and the effects of writing have taken place.
 *
 *  FORM_OF_CALL:
 * 	bsr r26, srom_wrtest
 *
 *  ARGUMENTS:
 * 	r16 - Address to write/read.
 * 	r17 - Data to be written.
 *
 *  RETURNS:
 * 	r18 - value read in.
 *	r19 - xor of write data and read data.
*/
srom_wrtest:
#ifdef DP264_PASS1
WriteSwizzle:	/*  b7 b6 b5 b4 b3 b2 b1 b0 => b7 b3 b6 b2 b5 b1 b4 b0	*/
	bis	r31, r31, r4		/* init scratchpad		*/
	bis	r31, 4, r0		/* n = 4			*/
SwzlW:	subq	r0, 1, r0		/* decrement n			*/
	sll	r0, 1, r1		/* Compute n*2			*/
	extbl	r17, r0, r2		/* Extract B(n)			*/
	insbl	r2, r1, r3		/* Insert  B(n*2)		*/
	bis	r4, r3, r4		/* OR into scratchpad		*/

	addq	r0, 4, r2		/* Compute n+4			*/
	addq	r1, 1, r1		/* Compute n*2 + 1		*/
	extbl	r17, r2, r3		/* Extract B(n+4)		*/
	insbl	r3, r1, r3		/* Insert  B(n*2 + 1)		*/
	bis	r4, r3, r4		/* OR into scratchpad		*/

	bne	r0, SwzlW		/* Loop until done		*/

	bis	r4, r31, r17		/* Result back into data reg	*/
#endif /* DP264_PASS1 */

	stq_p	r17, 0(r16)		/* write			*/
	mb				/* make sure it's not queued.	*/

srom_rdtest:
	ldq_p	r18, 0(r16)		/* r18 <- read back 		*/

#ifdef DP264_PASS1
	ReadSwizzle:	/*  b7 b3 b6 b2 b5 b1 b4 b0 => b7 b6 b5 b4 b3 b2 b1 b0	*/
	bis	r31, r31, r4		/* init scratchpad		*/
	bis	r31, 4, r0		/* n = 4			*/
SwzlR:	subq	r0, 1, r0		/* decrement n			*/
	sll	r0, 1, r1		/* Compute n*2			*/
	extbl	r18, r1, r2		/* Extract B(n*2)		*/
	insbl	r2, r0, r3		/* Insert  B(n)			*/
	bis	r4, r3, r4		/* OR into scratchpad		*/

	addq	r0, 4, r2		/* Compute n+4			*/
	addq	r1, 1, r1		/* Compute n*2 + 1		*/
	extbl	r18, r1, r3		/* Extract B(n*2 + 1)		*/
	insbl	r3, r2, r3		/* Insert  B(n+4)		*/
	bis	r4, r3, r4		/* OR into scratchpad		*/

	bne	r0, SwzlR		/* Loop until done		*/

	bis	r4, r31, r18		/* Result back into data reg	*/
#endif /* DP264_PASS1 */

	xor	r18, r17, r19		/* compare read to write	*/

/*
 *  This is a test designed to print out information thru the
 *  SROM port if the data read back is not the same as the data
 *  written.
 */
#ifdef DEBUG_BOARD
	PRINTS4(b4_to_l(a_cr,a_nl,a_A,a_colon))
	srl	r16, 32, r0
	PRINTL(r0)			/* Print the address in R0.	*/
	PRINTL(r16)			/* Print the address in R16.	*/
	PRINTS4(b4_to_l(a_sp,a_W,a_colon,a_nul))
	srl	r17, 32, r0
	PRINTL(r0)			/* Print the address in R0.	*/
	PRINTL(r17)			/* Print the data to be written	*/
	PRINTS4(b4_to_l(a_sp,a_R,a_colon,a_nul))
	srl	r18, 32, r0
	PRINTL(r0)			/* Print the address in R0.	*/
	PRINTL(r18)			/* Print the data read back.	*/

	beq	r19, equal		/* If Read == Write, skip msgs.	*/
not_equal:
	PRINTS2(a_sp,a_X)
#endif

equal:
	bne	r31, 1f			// push prediction stack
1:	bis	r26, 1, r26		//      pal mode
	hw_rets/jmp	(r26)		/* All synced up		*/

/*=======================================================================
 * = FatalError - Routine called when a fatal error is encountered.	=
 *=======================================================================
 *
 * OVERVIEW:
 *      This routine is called whenever a fatal error is encountered and
 *	we cannot proceed with system initialization.  It will write
 *	0xFE (Fatal Error) to the LED port, wait about 1 second, and
 *	then write the error code to the LED port, followed by audiable
 *	beeping of the error code.  After a 2 second delay, it jumps
 *	to the minidebugger.
 *
 * FORM OF CALL:
 *       br  r31, FatalError.
 *
 * ARGUMENTS:
 *	r8 - Byte value to write to port.
 *
 * RETURNS:
 *	It never does.
 *
 * REGISTERS:
 *	R1:R3 - Scratch.
 *	R16, R17, R19 - Argument registers
 *	R27-R28   - Return address register for routines called here.
*/
FatalError:
	OutLEDPORT(0x3F)		/* Fatal Error ledcode value.	*/
	lda	r17, 500(r31)		/* Wait about .95 secs.		*/
	bsr	r27, wait_for_pif
	OutLEDPORT(r8)			/* Output error code.		*/
	bis	r8, r8, r19
	bsr	r28, Beep		/* Beep error code.		*/
	lda	r17, 1000(r31)		/* Wait about 1.95 secs.	*/
	bsr	r27, wait_for_pif

#ifdef LCD

	bis	r31, 0x80, r16
	bis	r31, 0, r17
        bsr     r26, srom_access_lcd
	mb

	bis	r31, 0xc0, r16
	bis	r31, 0, r17
        bsr     r26, srom_access_lcd
	mb

	bis	r31, a_E, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_r, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_r, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_o, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_r, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb
	bis	r31, a_sp, r16
	bis	r31, 1, r17
        bsr     r26, srom_access_lcd
	mb

	srl	r8, 4, r16
	and	r16, 0xf, r16
	addq	r16, 0x30, r16
	bis     r31, 1, r17
        bsr     r26, srom_access_lcd
        mb


	and	r8, 0xf, r16
	addq	r16, 0x30, r16
	bis     r31, 1, r17
        bsr     r26, srom_access_lcd
        mb
#else
#ifdef Brick
	lda	r17, 0x5555(r31)
	lda     r16, 0x8013(r31)
	sll	r16, 28, r16		/* Shift into place		*/
	stl_p	r17, 0x580(r16)		/* write SW_Alert			*/
#endif
#endif

	lda	r17, 1000(r31)		/* Wait about 1.95 secs.	*/
	bsr	r27, wait_for_pif

	srl	r30, JMP_V_MINIDBGR, r1	/* Jump to minidbg on error?	*/

	jump_to_minidebugger()		/* trap to Mini-Debugger.	*/
reset_FatalError:
	br	r31, reset_FatalError


/*=======================================================================
 * = Beep - Causes the speaker to beep.					=
 *=======================================================================
 *
 * OVERVIEW:
 *      This routine causes a beep.
 *
 * FORM OF CALL:
 *       bsr  r28, Beep
 *
 * ARGUMENTS:
 *	r19 - Number of beeps.
 *
 * RETURNS:
 *	Nothing.
 *
 * REGISTERS:
 *	r0:r3   - Scratch registers.
 *	r16-r17 - Argument register.
 *	r27-29  - Return address register.
*/
Beep:
	lda	r16, 3(r31)		/* Turn on speaker and counter2	*/
	bsr	r29, WriteSIO_NMISC
	lda	r16, 0xB6(r31)		/* Counter 2, R/W LSB, then MSB	*/
	bsr	r29, WriteSIO_TCW	/* & generate square output wave*/
	lda	r16, 0xE2(r31)		/* Load the frequency.	Counter	*/
					/* value = 1193180/frequency	*/
	bsr	r29, WriteSIO_TMR2	/* Value = 0x3E2.  Write LSB.	*/
	lda	r16, 3(r31)		/* Write MSB.			*/
	bsr	r29, WriteSIO_TMR2

	lda	r17, 100(r31)		/* Wait about 200ms. Each inter-*/
	bsr	r27, wait_for_pif	/* rupt takes about 1.953125ms 	*/

	lda	r16, 0xB2(r31)		/* Counter 2, R/W LSB, then MSB	*/
	bsr	r29, WriteSIO_TCW
	lda	r16, 0(r31)
	bsr	r29, WriteSIO_TMR2	/* Turn off timer.		*/
	bsr	r29, WriteSIO_TMR2	/* Turn off timer.		*/
	bsr	r29, WriteSIO_NMISC	/* Turn off speaker and counter2*/

	lda	r17, 100(r31)		/* Wait about 200ms before	*/
	bsr	r27, wait_for_pif	/* continuing.			*/
	subq	r19, 1, r19		/* Are we done beeping?		*/
	bne	r19, Beep		/* If not, then do it again.	*/
	bne	r31, 1f			// push prediction stack
1:	bis	r28, 1, r28		//      pal mode
	hw_rets/jmp	(r28)		/* All synced up		*/

/*=======================================================================
 * = GetCPUSpeed - Detects the speed of this microprocessor.		=
 *=======================================================================
 *
 * OVERVIEW:
 *	Use the real-time clock (RTC) to detect the speed of the CPU.
 *	This is done by measuring the number of cycles executed during
 *	a .125 second (64 x 1.953125ms) interval while polling the RTC's
 *	Periodic Interrupt Flag (PIF).
 *	Note:  This code assumes that the real time clock is stable
 *	by now.  You may have to put in a delay before calling this
 *      routine to guarantee that it is.
 *
 * FORM OF CALL:
 *       bsr  r28, GetCPUSpeed
 *
 * ARGUMENTS:
 *	None.
 *
 * RETURNS:
 *	R0 - CPU speed in cyles/seconds.
 *
 * REGISTERS:
 *	r0-r3   - Scratch registers.
 *	r4      - Initial cycle count.
 *	r16-r17 - Argument register.
 *	r27-29  - Return address register.
*/
GetCPUSpeed:
#ifdef HARD_SPEED
/*
 *	HARD_SPEED can be defined to override the Automatic
 *	CPU Speed Detection that is used to compute the BCache
 *	timing parameters.  This prevents the RTC from being initialized.
 *	Example -DHARD_SPEED=266  for a CPU running at 266 MHz
 */
	LDLI	(r0, HARD_SPEED*1000000, r31)
#else

/*
 * Turn on osc and set interrupt rate to 512Hz or 1.953125 ms.
*/
wait_for_not_uip:
	lda	r16, rtc$reg_a(r31)	/* R16 <- 0xA.			*/
	bsr	r29, WriteRTCAddr
	lda	r16, (rtc$m_a_osc_on | rtc$m_a_rs_512hz)(r31)
	bsr	r29, WriteRTCData	/* R16 <- 0x27			*/


/*
 * Turn on reporting of interrupts thru IRQ signal, put counter in binary
 * mode for data read and 24-hour mode (needed by Debug Monitor).
 */
	lda	r16, rtc$reg_b(r31)	/* R16 <- 0xB			*/
	bsr	r29, WriteRTCAddr
	lda	r16, (rtc$m_b_pie | rtc$m_b_dm_bin | rtc$m_b_24)(r31)
	bsr	r29, WriteRTCData	/* R16 <- 0x47			*/

        lda     r16, rtc$reg_a(r31)     /* R16 <- 0xA.                  */
        bsr     r29, WriteRTCAddr
        bsr     r29, ReadRTCData        /* R0 <- byte from RTC data port*/
        srl     r0, rtc$v_a_uip, r0     /* Shift PIF into low bit.      */
        blbs    r0, wait_for_not_uip    /* Wait for deassertion.        */


/*
 * Now that the Periodic Interrupt is going, let things settle a bit.
 * Wait for 10 or so Periodic interrupts to occur before going on.
 */
	lda	r17, 10(r31)		/* Wait for 10 PIF interrupts	*/
	bsr	r27, wait_for_pif	/* to occur before proceeding.	*/

/*
 * Get the inital cycle count.  Go through 64 transitions of this
 * interrupt and get final cycle count. Multiply the cycle count by 8
 * (since 64 cycles = 1/8 second) to get cycles per second.
 */
	lda	r17, 64(r31)		/* Initialize PIF counter.	*/
	rpcc	r4			/* Read initial cycle count.	*/
	bsr	r27, wait_for_pif
	rpcc	r0			/* Read ending cycle count.	*/

	subl	r0, r4, r0		/* check cc for wrapping	*/
	bge 	r0, noshft		/* check to see if negative	*/
	bis	r31, 1, r1		/* then we need to add 1<<32	*/
	sll	r1, 32, r1		/* shift the add amount		*/
	addq	r1, r0, r0		/* add 1<<32 to it for wrapping	*/

noshft:
	/* R0 now contains the number of cycles in 1/8th of a second.	*/
	sll	r0, 3, r0		/* Multiply by 8 to get cyc/sec	*/
#endif /* HARD_SPEED */
	bne	r31, 1f			// push prediction stack
1:	bis	r28, 1, r28		//      pal mode
	hw_rets/jmp	(r28)		/* All synced up		*/

/*=======================================================================
 *= wait_for_pif - Waits for a periodic interrupt from real time clock.	=
 *=======================================================================
 * OVERVIEW:
 *      This function will wait until specified number of periodic
 *	interrupts have been detected from the real time clock.
 *
 * FORM OF CALL:
 *       bsr r27, wait_for_pif
 *
 * ARGUMENTS:
 *	r17 - Number of interrupts to wait for.
 *
 * RETURNS:
 *	Nothing.
 *
 * REGISTERS:
 *	r0 - Contents of RTC's register C.
 *	r1-r3 r5 - Scratch registers.
 */
wait_for_pif:
	lda	r5, 10(r31)		// time out count
wait_for_pif0:
	subq	r5, 1, r5
	beq	r5, wait_assert
	lda	r16, rtc$reg_c(r31)
	bsr	r29, WriteRTCAddr
	bsr	r29, ReadRTCData	/* R0 <- byte from RTC data port*/
	srl	r0, rtc$v_c_pf, r0	/* Shift PIF into low bit.	*/
	blbs	r0, wait_for_pif0	/* Wait for deassertion.	*/

wait_assert:
        lda    r5, 10(r31)              // time out count
wait_assert0:
        subq    r5, 1, r5
        beq     r5, wait_assert_to
	bsr	r29, WriteRTCAddr
	bsr	r29, ReadRTCData	/* R0 <- byte from RTC data port*/
	srl	r0, rtc$v_c_pf, r0	/* Shift PIF into low bit.	*/
	blbc	r0, wait_assert		/* Wait for an interrupt flag.	*/
wait_assert_to:
	subq	r17, 1, r17		/* Check to see if we're done.	*/
	bne	r17, wait_for_pif
	bne	r31, 1f			// push prediction stack
1:	bis	r27, 1, r27		//      pal mode
	hw_rets/jmp	(r27)		/* All synced up		*/

/*
 * ======================================================================
 * =  LoadSystemCode - Loads firmware code from ROM.			=
 * ======================================================================
 * FUNCTIONAL DESCRIPTION:
 *	This routine will load the specified image from the ROM into
 *	memory.  If it's not found, then the first image in the ROM
 *	will be loaded if one is present.  Otherwise, the entire ROM
 *	is loaded to address 0.
 *
 *      Algorithm used for loading a ROM image:
 *
 *	1) Look at the alternate boot jumper.
 *	2) If it is equals to 1 (not jumpered), then we want to load the
 *         first (physically) valid image in the ROM.
 *	   If it is equals to 0 (jumper is in), then read the TOY RAM
 *         location 0x3F to determine which image to load.  If the value
 *         there is less than 0x80, then we want to load the image with
 *         the firmware ID that corresponds to the value in the TOY RAM.
 *	   This is done by using the map table shown below.  If the TOY
 *	   RAM value is greater than 0x80, then the image is loaded based
 *         on its position in the flash with respect to the other images.
 *	   0x81 will load the first valid image, 0x82, the second and so
 *	   on.  A value of 0x80 will load the entire flash.
 *
 *	        TOY RAM	   FIRMWARE ID	  IMAGE
 *		VALUE	   IN HEADER
 *		0x00 	   0x0		Debug monitor
 *		0x01 	   0x1		NT (ARC) firmware
 *		0x02  	   0x2		VMS (SRM) firmware
 *		0x03 	   0x2		UNIX (SRM) firmware
 *		0x07	   0x7		LINUX (MILO) firmware
 *
 *		0x8n - the 'n'th image where n=0 means "load the entire flash"
 *			and n=1 means "load the first image"
 *
 *	   R10 will hold the image number to be loaded.  If bit 0 is set,
 *	   then the search is to be done by firmware ID type and that ID
 *	   is contained in bits 8-15.  If the search is to be done based
 *	   on position, then bit 0 will be cleared and bits 8-15 will contain
 *	   the position number.
 *
 *	For PC164, if boot option bit is set, then we load the first image.
 *	Otherwise, the second image is loaded.
 *
 *	3) Begin searching for a ROM header.  This is done by looking for
 *	   the first longword of the signature.  If one is found, then we
 *	   look for the inverse pattern.  Otherwise, we continue scanning.
 *
 *	4) Once a valid signature has been found, then look at the next
 *	   field, the header size, to determine header version.  If V0,
 *	   then we can't check if the header is corrupted because a
 *	   header checksum field is not present.  If header version is 1
 *	   or greater, then we compute the checksum of the header and
 *	   compare it against the value in the image header.  If they
 *	   don't match, then we continue scanning for a valid header.
 *
 *	5) Once a valid header has been found, then we determine if
 *	   this is the right image to load by looking at its firmware
 *	   ID or its position in the flash, depending on the TOY RAM
 *	   setting.
 *
 *	6) Load the image and check its checksum.  If they match, then
 *	   we're done.  If they don't match, then continue scanning
 *
 *	7) If we reach the end of the ROM and no matches have been found,
 *         then try to load the first valid image in the ROM.
 *         If that fails, then load the first image, ignore checksums.
 *         And if that still doesn't find an image, then load the entire ROM.
 *
 *	The makerom tool is used to place a "special" header on ROM image
 *	files.  The SROM allows the System ROM to contain different
 *	ROM images, each with its own header. That header tells the
 *	SROM where to load the image and also if it has been compressed
 *	with the "makerom" tool.  For System ROMs which contain a single
 *	image the header is optional.  If the header does not exist the
 *	complete 1024KB System ROM is loaded and executed at physical
 *	address zero.
 *
 *                                                     +-- Offset
 *                                                     |         Header
 *	31                                             0   |     +-- Revisions
 *	+-----------------------------------------------+  |     |   Supported
 *	|   VALIDATION PATTERN 0x5A5AC3C3  or branch    | 0x00  all
 *	|   instruction.				|
 *	+-----------------------------------------------+
 *	|   INVERSE VALIDATION PATTERN 0xA5A53C3C       | 0x04  all
 *	+-----------------------------------------------+
 *	|   HEADER SIZE (Bytes)                         | 0x08  all
 *	+-----------------------------------------------+
 *	|   IMAGE CHECKSUM                              | 0x0C  all
 *	+-----------------------------------------------+
 *	|   IMAGE SIZE (Memory Footprint)               | 0x10  all
 *	+-----------------------------------------------+
 *	|   DECOMPRESSION FLAG                          | 0x14  all
 *	+-----------------------------------------------+
 *	|   DESTINATION ADDRESS LOWER LONGWORD          | 0x18  all
 *	+-----------------------------------------------+
 *	|   DESTINATION ADDRESS UPPER LONGWORD          | 0x1C  all
 *	+-----------------------------------------------+
 *  |   FIRMWARE ID <15:8> | HEADER REV <7:0>       | 0x20  1+
 *	|   Reserved <31:24>   | HEADER REV EXT <23:16> |
 *	+-----------------------------------------------+
 *	|   ROM IMAGE SIZE                              | 0x24  1+
 *	+-----------------------------------------------+
 *	|   OPTIONAL FIRMWARE ID <31:0>                 | 0x28  1+
 *	+-----------------------------------------------+
 *	|   OPTIONAL FIRMWARE ID <63:32>                | 0x2C  1+
 *	+-----------------------------------------------+
 *	|   ROM OFFSET<31:2>     | ROM OFFSET VALID<0>  | 0x30  2+
 *	+-----------------------------------------------+
 *	|   HEADER CHECKSUM (excluding this field)      | 0x34  1+
 *	+-----------------------------------------------+
 *
 *	VALIDATION PATTERN
 *	------------------
 *	The first quadword contains a special signature pattern
 *	that is used to verify that this "special" ROM header
 *	has been located.  The pattern is 0x5A5AC3C3A5A53C3C.
 *
 *	HEADER SIZE (Bytes)
 *	-------------------
 *	The header size is the next longword.  This is provided
 *	to allow for some backward compatibility in the event that
 *	the header is extended in the future.  When the header
 *	is located, current versions of SROM code determine where
 *	the image begins based on the header size.  Additional data
 *	added to the header in the future will simply be ignored
 *	by current SROM code. Additionally, the header size = 0x20
 *	implies version 0 of this header spec.  For any other size
 *      see HEADER REVISION to determine header version.
 *
 *
 *	IMAGE CHECKSUM
 *	--------------
 *	The next longword contains the image checksum.  This is
 *	used to verify the integrity of the ROM.  Checksum is computed
 *      in the same fashion as the header checksum.
 *
 *	IMAGE SIZE (Memory Footprint)
 *	-----------------------------
 *	The image size reflects the size of the image after it has
 *	been loaded into memory from the ROM. See ROM IMAGE SIZE.
 *
 *	DECOMPRESSION FLAG
 *	------------------
 *	The decompression flag tells the SROM code if the makerom
 *	tool was used to compress the ROM image with a "trivial
 *	repeating byte algorithm".  The SROM code contains routines
 *	which perform this decompression algorithm.  Other
 *	compression/decompression schemes may be employed which work
 *	independently from this one.
 *
 *	DESTINATION ADDRESS
 *	-------------------
 *	This quadword contains the destination address for the
 *	image.  The SROM code  will begin loading the image at this
 *	address and subsequently begin its execution there.
 *
 *	HEADER REV
 *	----------
 *	The revision of the header specifications used in this
 *	header.  This is necessary to provide compatibility to
 *	future changes to this header spec.  Version 0 headers
 *	are identified by the size of the header. See HEADER SIZE.
 *	For Version 1 or greater headers this field must be set to
 *	a value of 1.  The header revision for version 1 or greater
 *	headers is determined by the sum of this field and the
 *	HEADER REV EXT field. See HEADER REV EXT.
 *
 *	FIRMWARE ID
 *	-----------
 *	The firmware ID is a byte that specifies the firmware type.
 *	This facilitates image boot options necessary to boot
 *	different operating systems.
 *
 *		  firmware
 *	firmware    type
 *	--------  --------
 *	  DBM	     0	     Alpha Evaluation Boards Debug Monitor
 *	  WNT        1       Windows NT Firmware
 *	  SRM        2       Alpha System Reference Manual Console
 *	  Milo       7       Linux Miniloader
 *
 *	HEADER REV EXT
 *	--------------
 *	The header revision for version 1 or greater headers is
 *	determined by the sum of this field and the HEADER REV
 *	field. See HEADER REV.
 *
 *	ROM IMAGE SIZE
 *	--------------
 *	The ROM image size reflects the size of the image as it is
 *	contained in the ROM. See IMAGE SIZE.
 *
 *	OPTIONAL FW ID
 *	--------------
 *	This is an optional field that can be used to provide
 *	additional firmware information such as firmware revision
 *	or a character descriptive string up to 8 characters.
 *
 *	ROM OFFSET
 *	----------
 *	This field specifies the default ROM offset to be used
 *	when programming the Image into the ROM.
 *
 *	ROM OFFSET VALID
 *	----------------
 *	The lower bit of the ROM OFFSET field should be set when
 *	the ROM OFFSET field is specified.  When no ROM OFFSET is
 *	specified the ROM OFFSET and VALID fields will contain zero.
 *
 *	HEADER CHECKSUM
 *	---------------
 *	The checksum of the header.  This is used to validate
 *	the presence of a header beyond the validation provided
 *	by the validation pattern.  See VALIDATION PATTERN.
 *	The header checksum is computed from the beginning of
 *	the header up to but excluding the header checksum
 *	field itself.  If there are future versions of this
 *	header the header checksum should always be the last
 *	field defined in the header.  The checksum algorithm used
 *	is compatible with the standard BSD4.3 algorithm provided
 *	on most implementations of Unix.  Algorithm: The checksum
 *	is rotated right by one bit around a 16 bit field before
 *	adding in the value of each byte.
 *
 *  REGISTERS:
 *	This code needs R16 as an input register.
 *
 *	R16 - A copy of the standard configuration register.
 *
 *	It uses the following registers:
 *
 *	R0:R5 Sratch
 *	R6  - Flash offset where search should continue if this image
 *	      fails loading.
 *	R7  - Checksum register reset to 0.
 *	R8  - Destination address of image.
 *	R9  - Offset to the end of the image.
 *	R10 - <15:8> Image number to load (position dependent) or
 *	      <15:8> Firmware ID to load (position independent).
 *	      <3>    0 = Searching for specified image,
 *		     1 = Searching for first image because
 *			 specified image was not found.
 *	      <2>    0 = don't use checksums, 1 = use checksums
 *	      <1>    0 = search for headers based on position,
 *	             1 = search for headers using firmware id.
 *	      <0>    0 = header version is not 0, 1 = header is V0.
 *	R11 - Image checksum
 *	R12 - Number of headers found so far.
 *	R14 - Return address register for this routine.
 *	R15 - Scratch
 *	R16:R19 Arguments to several routines
 *	R17 - Offset to the beginning of the image.
 *	R25 - Address in memory where image was loaded.
 *	R26:R29 Return Address registers.
 *
 *	It purposely does not use the following registers:
 *	R13, R20-r24
 */
LoadSystemCode:
					/* dependent search.		*/
	lda	r10, ((1 << 8) | 4)(r31) /* First image if bit is set	*/
	lda	r1, ((2 << 8) | 4)(r31) /* 2nd image if bit is cleared	*/
	srl	r16, JMP_V_ALT_BOOT, r0	/* Check boot jumper state.	*/
	cmovlbc	r0, r1, r10		/* If cleared, then 2nd image.	*/

scanROM:
	PRINTC(a_pound)
	srl	r10, 32, r6
	PRINTL(r6)
	PRINTL(r10)
	PRINTC(a_pound)
	bis	r31, r31, r12		/* Init. count of headers found */
	bis	r31, r31, r6		/* Load starting offset.	*/

rescanROM:
	bis	r6, r6, r17		/* R17 holds offset into flash. */
	bis	r31, r31, r7		/* Init. checksum register.	*/
	bic	r10, 1, r10		/* Clear the V0 flag.		*/

	LDLI(r0, ROMSIZE, r31)		/* Load maximum size of ROM.  	*/
	cmpult	r17, r0, r0		/* Have we run out of ROM? 	*/
	blbs	r0, ValidationPattern

/*
 *  We are done with one pass thru the ROM and an image has not been
 *  found.  Let's first try to load the first valid image found in the
 *  ROM.  If we don't find one, then look for the first image (ignoring
 *  checksums) and if that fails, load the entire flash.
 */
DoneWithROM:
	/*
	 *  Check to see if we cared about checking checksums.  If we
	 *  didn't, it means that we just finished our last attempt at
	 *  finding any images, therefore, the only thing left to do
	 *  is to try to load the entire flash and hope there's an image
	 *  with no standard header.
	 */
	and	r10, 4, r0		/* Check to see if we cared 	*/
	beq	r0, NoHeader		/* Load the entire flash.	*/

	/*
	 *  If we cared about checksums, then see if we just finished
	 *  our first attempt to find the first valid image.  If we did,
	 *  then, let's try finding the first image, ignoring checksums.
	 */
	and	r10, 8, r0
	lda	r10, ((1 << 8) | 8) (r31)
	bne	r0, scanROM		/* Last attempt, without chksum */
	bis	r10, 4, r10		/* First attempt, with chksums	*/
	br	r31, scanROM		/* and start from the top	*/

ValidationPattern:
	bsr	r28, read_flash		/* R0 <- LW at address (R17)	*/
					/* R17 <- R17+4			*/
	bis	r17, r17, r6		/* Save offset for continuing.	*/
	LDLI(r1, 0x5a5ac3c3, r31)	/* Load pattern 0x5a5ac3c3	*/
	cmpeq	r0, r1, r1		/* Does it match pattern1?	*/
	bne	r1, InversePattern	/* Yes, then for inverse pattern*/
	LDLI(r1, 0xc3e0, r31)		/* See if it's a branch instr.	*/
	extwl	r0, 2, r0		/* Extract bits 31:16.		*/
	cmpeq	r0, r1, r0		/* Compare to branch instr.	*/
	beq	r0, rescanROM		/* No, then check next LW.	*/

InversePattern:
	bsr	r28, read_flash		/* R0 <- LW at address (R17)	*/
					/* R17 <- R17+4			*/
	LDLI(r1, 0xa5a53c3c, r31)	/* Load inverse pattern. 	*/
	zap	r1, 0xF0, r1		/* clear upper longword.	*/
	cmpeq	r0, r1, r0		/* Does it match pattern2?	*/
	beq	r0, rescanROM		/* No, then restart search.	*/

HeaderSize:
	bsr	r28, read_flash		/* R0 <- LW at address (R17)	*/
					/* R17 <- R17+4			*/
	cmplt	r0, 0x34, r1		/* Is this a V0 header?		*/
	bis	r10, r1, r10
	blbs	r10, HeaderIsValid	/* Yes.  Then, there's no chksum*/

	subq	r0, 0x10, r5		/* Compute bytes to header sum	*/
	srl	r5, 2, r5		/* Divide by 4 to get # of LWs	*/

HeaderCheckSum:
	bsr	r28, read_flash		/* R0 <- LW at address (R17)	*/
	subq	r5, 1, r5		/* Continue reading up to header*/
	bne	r5, HeaderCheckSum	/* checksum field.		*/

	bis	r7, r7, r5		/* Save the computed checksum	*/
	bsr	r28, read_flash		/* Read the checksum from header*/
	addq	r6, (0xC-0x4), r17	/* Point to image checksum field*/

	srl	r10, 2, r1		/* See if we need to compare	*/
	blbc	r1, HeaderIsValid	/* the checksum this pass.	*/
	cmpeq	r0, r5, r0		/* Compare checksums.		*/
	beq	r0, rescanROM		/* Continue search if mismatch.	*/

HeaderIsValid:
	addq	r12, 1, r12		/* Incr. count of headers found.*/

	bsr	r28, read_flash		/* Load image checksum field.	*/
	bis	r0, r0, r11		/* Save to another register.	*/
	bsr	r28, read_flash		/* Load image size (in memory)	*/
	bis	r0, r0, r9		/* Save to another register.	*/
	bsr	r28, read_flash		/* Load decompression flag	*/
	bis	r0, r0, r5		/* Save to another register.	*/
	bsr	r28, read_flash		/* Load lower destination addr. */
	bis	r0, r0, r8		/* Save to another register.	*/
	bsr	r28, read_flash		/* Load upper destination addr. */
	sll	r0, 32, r0		/* Shift into upper LongWord	*/
	bis	r8, r0, r8		/* Combine to get destination.	*/

	blbs	r10, SkipFirmwareID	/* V0 headers... no FirmwareID	*/
FirmwareID:				/* Firmware ID dependent search.*/
	addq	r6, (0x20-0x4), r17	/* Point to firmware id field.	*/
	bsr	r28, read_flash		/* Read firmware ID.		*/
	extbl	r0, 1, r1		/* Extract the firmware id	*/

SROMImageCheck:
	subq	r12, 1, r12		/* Sub. header count for SROM	*/
	cmpeq	r1, 10, r0		/* Is this an SROM image?	*/
	bne	r0, rescanROM		/* Yes, then restart search	*/
	addq	r12, 1, r12		/* No, Incr. header count again	*/

SkipFirmwareID:
	srl	r10, 1, r2
	cmovlbc r2, r12, r1		/* Use position instead of ID.	*/
	blbc	r2, PositionDependent	/* Position dependent search?	*/

					/* Is this a V0 header?		*/
	blbs	r10, rescanROM		/* Yes. It can't be used. Rescan*/

PositionDependent:
	extbl	r10, 1, r0		/* Get the image number.	*/
	cmpeq	r1, r0, r0		/* is this the image we want?  	*/
	beq	r0, rescanROM		/* No, then restart search	*/

	addq	r6, (0x24-0x4), r17	/* Read in ROM Image size.	*/
	bsr	r28, read_flash		/* Is this a V0 header?		*/
	cmovlbc r10, r0, r9		/* No, use ROM Image size field	*/

	addq	r6, (0x8-0x4), r17	/* Load the header size again.	*/
	bsr	r28, read_flash
	subq	r6, 0x4, r17		/* Return pointer to beginning	*/
	addq	r0, r17, r17		/* of header to compute offset	*/
	addq	r17, r9, r9		/* where image begins and ends	*/
	br	r31, headerEnd

NoHeader:
	lda	r19, BEEP_LOAD_ENTIRE_ROM(r31)
	bsr	r28, Beep		/* Announcement of flash loading*/

#if 0
 	bis	r31, r31, r8		/* Default dest. to address 0.	*/
#else
	LDLI(r8, 0x200000, r13)		/* Default dest. to 0x200000.	*/
#endif
	bis	r31, r31, r5		/* No compression. 		*/
#if 0
	bis	r31, r31, r17		/* Start loading at offset 0.	*/
#else
	LDLI(r17, 0x20000, r31)		/* Start flash offset 0x20000.	*/
#endif
	LDLI(r9, ROMSIZE, r31)		/* Load maximum size of ROM.	*/
	bic	r10, 4, r10		/* Don't use checksums.		*/

headerEnd:
	bis	r8, r8, r25		/* save a copy of start address	*/
	bis	r31, r31, r7		/* Reset the checksum register	*/
	blbs	r5, LoadComp		/* Compressed? Yes, then do it.	*/

/*
 *  REGISTERS:
 *
 *      R6  - Flash offset where search should continue if this image
 *	      fails loading.
 *	R7  - Checksum register reset to 0.
 *	R8  - Destination address of image.
 *	R9  - Offset to the end of the image.
 *	R10 - Flags and image number being searched for.
 *	R11 - Image checksum
 *	R12 - Number of headers found so far.
 *	R14 - Return address.
 *	R17 - Offset to the beginning of the image.
*/
	OutLEDPORT(0x15)		/* Write to LEDS. Uses R1, R2 & R3 */
LoadUncomp:
	bsr	r28, read_flash		/* R0 <- LW at address (R17)	*/
					/* R17 <- R17+4			*/
	stl_p	r0, 0(r8)		/* store the longword 		*/

	addq	r8, 4, r8		/* increment destination pointer*/
	cmpult	r17, r9, r0		/* Have we finished image? 	*/
	bne	r0, LoadUncomp		/* No, loop until we're done.	*/
					/* Yes, R8 now points to the	*/
					/* next LW after the last one 	*/
	br	r31, ImageCheckSums	/* Branch around.		*/

LoadComp:
	OutLEDPORT(0x16)		/* Write LEDS. Uses R1, R2 & R3 */
	bis	r31, 4, r18		/* Initialize byte counter.	*/
 	bsr	r26, getRecord		/* prime first record 		*/

LoadCInit:
	bis	r31, r31, r27		/* Initialize LW register.	*/
	bis	r31, r31, r5		/* Initialize Byte ctr.		*/

LoadCLoop:
	bsr	r26, getByte
	insbl	r16, r5, r0		/* Shift to the correct byte.	*/
	bis	r0, r27, r27		/* Add to the other bytes.	*/
	addq	r5, 1, r5		/* Increment the byte counter.	*/
	cmpeq	r5, 4, r0		/* Is the LW complete?		*/
	beq	r0, LoadCLoop		/* No, get another byte then.	*/

	stl_p	r27, 0(r8)		/* Store the longword	 	*/
	addq	r8, 4, r8		/* Inc. destination pointer 	*/
	bne	r19, LoadCInit		/* Loop 'til done	 	*/

DecompressionDone:
	lda	r5, 1(r31)		/* Decompression flag.		*/

ImageCheckSums:
	blbs	r10, codeLoaded		/* Don't check sums for V0 header*/
	srl	r10, 2, r0		/* See if we need to compare	*/
	blbc	r0, codeLoaded		/* the checksum this time.	*/
	cmpeq	r7, r11, r1		/* Compare image checksums	*/
	beq	r1, SumMismatch		/* If mismatch, then restart.	*/
					/* Otherwise, make sure memory	*/
					/* has a good copy.		*/
	blbs	r5, codeLoaded		/* Can't check compressed images*/

	ldah	r0, 0x100(r31)		/* 2 * largest BCache size = 16MB*/
flush:
	ldq_p	r31, 0x0(r0)		/* Load to kick things out.	*/
	subq	r0, 0x20, r0		/* do it in 32-byte strides	*/
	bge	r0, flush		/* Continue until < 0.		*/

	bis	r31, r31, r7		/* Set checksum to 0.		*/
	bis	r25, r25, r17		/* Starting location of image.	*/
secondcheck:
	ldl_p	r0, 0x0(r17)		/* Load data from memory.	*/
	bsr	r28, ComputeCheckSum	/* Compute checksum from data.	*/
	addq	r17, 4, r17		/* Increment pointer.		*/
	cmpult	r17, r8, r1		/* Have reached end of image?	*/
	bne	r1, secondcheck
	cmpeq	r7, r11, r1		/* Check sum from data in mem.	*/
	bne	r1, codeLoaded		/* If match, then continue.	*/
NoMemory:
	lda	r8, LED_F_MEMCHECKSUM(r31)/* Ledcode value to print.	*/
	br	r31, FatalError		/* image checksum in memory.	*/

SumMismatch:
	subq	r12, 1, r12		/* Decr. header cntr if mistmatch*/
	br	r31, rescanROM		/* If not equal, then rescan.	*/
codeLoaded:
	bne	r31, 1f			// push prediction stack
1:	bis	r14, 1, r14		//      pal mode
	hw_rets/jmp	(r14)		/* All synced up		*/


/*=======================================================================
 * = read_flash - Read a LW from Flash memory.				=
 *=======================================================================
 * OVERVIEW:
 *	This function will read a LW from offset specified into the flash
 *	memory.
 *
 * FORM OF CALL:
 *       bsr r28, read_flash.
 *
 * ARGUMENTS:
 *	R7 - Running checksum
 *	R17 - Offset into flash memory to read LW from.
 *
 * RETURNS:
 *	R0 - LW read from flash memory.
 *	R17 - Increments R17 by 4 bytes.
 *	R7 - Update checksum (falls thru to ComputeCheckSum)
 *
 * REGISTERS:
 *	R0 - Offset at which bank should be switch / pci dense mem base
 *	R2 - Scratch
 *	R3 - Scratch
 *	R4 - Address to read from.
 * 	R7 - Checksum
 *
*/
read_flash:
	ldah	r0, (ROMSIZE>>16)(r31)	/* Make sure we read from a	*/
	cmpult	r17, r0, r0		/* valid offset.  Otherwise,	*/
	cmoveq	r0, r17, r6
	beq	r0, rescanROM		/* abort and restart operation	*/

	load_csr_base(r0, TIG_BUS)
	sll	r17, 6, r4
	bis	r4, r0, r4
	ldq_p	r2, 0x00(r4)
	zap	r2, 0xfe, r2

	ldq_p	r3, 0x40(r4)
	insbl	r3, 1, r3
	bis	r3, r2, r2

	ldq_p	r3, 0x80(r4)
	insbl	r3, 2, r3
	bis	r3, r2, r2

	ldq_p	r3, 0xC0(r4)
	insbl	r3, 3, r3
	bis	r3, r2, r2

	zap	r2, 0xF0, r0		/* blow off the high longword	*/
	addq	r17, 4, r17		/* Increment offset for next RD.*/
					/* Fall-thru to ComputeCheckSum */

/*=======================================================================
 * = ComputeCheckSum - Computes checksum of input longword.		=
 *=======================================================================
 * OVERVIEW:
 *	Computes the 16-bit checksum of the input longword.  It uses the
 *	same algorithm as the unix "sum" command.
 *
 * FORM OF CALL:
 *       bsr r28, ComputeCheckSum
 *
 * ARGUMENTS:
 *	R0  - Longword of data for which we want to compute the sum.
 *	R7  - Running checksum.  Update checksum will be returned thru
 *	      this register.
 *
 * RETURNS:
 *	R7  - Running checksum.  Update checksum will be returned thru
 *	      this register.
 *
 * REGISTERS:
 *	R2  - Byte counter.
 *	R3-R4 - Scratch registers.
 *
*/
ComputeCheckSum:
	lda	r2, 0(r31)		/* Compute checksum of 4 bytes	*/
CheckSumLoop:
	srl	r7, 1, r3		/* Shift checksum to 1 bit.	*/
	LDLI(r4, 0x8000, r31)
	addq	r3, r4, r4		/* Implement bit rotation	*/
	cmovlbs r7, r4, r3
	extbl	r0, r2, r4		/* Extract byte to add to sum.	*/
	addq	r3, r4, r7		/* Add byte to checksum.	*/
	zapnot	r7, 0x03, r7		/* Mask off upper bytes.	*/
	addq	r2, 1, r2		/* Increment byte counter.	*/
	cmpeq	r2, 4, r4		/* Are we done with 4 bytes?	*/
	beq	r4, CheckSumLoop
	bne	r31, 1f			// push prediction stack
1:	bis	r28, 1, r28		//      pal mode
	hw_rets/jmp	(r28)		/* All synced up		*/


/*=======================================================================
 * = getByte - Gets the next byte to store.  				=
 * = getRecord - Gets the next record from Flash memory.		=
 *=======================================================================
 * OVERVIEW:
 *	This function will obtain the next byte to be stored to memory when
 *	decompressing the System ROM code.
 *
 *	getRecord will load the next record used for decompressing the
 *	next few bytes.  A record value less than 0 means to repeat the
 *	next byte N times, and a positive value indicates how many bytes
 *	to read to get to the next record.
 *
 * FORM OF CALL:
 *       bsr r26, getByte.
 *       bsr r26, getRecord.
 *
 * ARGUMENTS:
 *	R15 - Holds LW read from flash memory (indirect)
 *	R17 - Pointer to the next LW in flash memory (indirect).
 *	R18 - Pointer to next byte to be returned from R15 (indirect).
 *	R19 - Number of bytes in current record.
 *
 * RETURNS:
 *	R16 - Byte that should be written to memory next.
 *	R17 - Increments R17 by 4 bytes.
 *	R18 - Increments R18 by 1 byte.
 *	R19 - If getRecord is called, or R19 is 0, then R19 is initialized
 *	      to the new record value.  If getByte is called, then R19 is
 *	      incremented by 1 if it's less than 0 or decremented by 1
 *	      if greater than 0.
 *
 * REGISTERS:
 *	R28 - Return address for ReadFByte.
 *
*/
getByte:
	blt	r19, repByte		/* if neg then repeat byte 	*/
	bsr	r28, ReadFByte		/* If pos, then get next byte 	*/
	bis	r2, r2, r16		/* Move to return register.	*/
	lda	r19, -1(r19)		/* dec the count in this record */
	beq	r19, getRecord		/* end of record? get next 	*/
	bis	r26, 1, r26		//      pal mode
	hw_rets/jmp	(r26)		/* All synced up		*/

repByte:
	bis	r2, r2, r16
	lda	r19, 1(r19)		/* inc the count 		*/
	beq	r19, getRecord		/* end of record? get next 	*/
	bis	r26, 1, r26		//      pal mode
	hw_rets/jmp	(r26)		/* All synced up		*/

getRecord:
	bsr	r28, ReadFByte		/* Read header (byte count)-> R0*/
	sll	r2, 56, r19		/* sign extend the record flag	*/
	sra	r19, 56, r19		/* sign extend the record flag	*/
	bge	r19, norepeat		/* if neg then need to get 	*/
	bsr	r28, ReadFByte		/* repeating byte -> R2 	*/

norepeat:
	bis	r26, 1, r26		//      pal mode
	hw_rets/jmp	(r26)		/* All synced up		*/

#if 0
/*
**	CallPal HALT entry point.
*/
.= 0x2000
HaltEntry:
	br	r31, minidebugger
#endif


/*=======================================================================
 * = ReadFByte - Returns the next byte read from flash memory.		=
 *=======================================================================
 * OVERVIEW:
 *	This function will return the next byte read from flash memory.
 *	The code actually reads one LW at a time and extracts the next
 *	byte pointed to by a ctr (R18).
 *	memory.
 *
 * FORM OF CALL:
 *       bsr r28, ReadFByte.
 *
 * ARGUMENTS:
 *	R9  - Maximum flash offset for loading image.
 *	R15 - Holds the LW read from flash memory.
 *	R17 - Offset into flash memory to read LW from (indirect).
 *	R18 - Pointer to the next byte to extract from LW.  The first
 *	      time around, this register should be set to 4 and never
 *	      changed again (indirect).
 *
 * RETURNS:
 *	R2 - Returns the byte read in the Least Significant Byte position.
 *	R17 - Increments R17 by 4 bytes.
 *	R18 - Increments R18 by 1 byte.
 *
 * REGISTERS:
 *	R0 - Scratch.
 *
*/
ReadFByte:
	cmpeq	r18, 4, r0		/* Need to load another LW?	*/
	beq	r0, skipRd		/* No, skip the read then.	*/

	cmpult	r17, r9, r0		/* Have we reached the end?	*/
	beq	r0, DecompressionDone	/* Abort decompression now!	*/

	bis	r31, r31, r18		/* Yes, initialize the byte ctr	*/
					/* and read another LW.		*/
	bis	r28, r28, r15		/* Save Return address		*/
	bsr	r28, read_flash		/* R0 <- LW at address (R17)	*/
	bis	r15, r15, r28		/* We had to do this due to reg	*/
					/* shortage.			*/
	bis	r0, r0, r15		/* Save LW to temp. register.	*/
skipRd:
	extbl	r15, r18, r2		/* Get the byte to repeat.	*/
	lda	r18, 1(r18)		/* Increment the byte counter.	*/
	bis	r28, 1, r28		//      pal mode
	hw_rets/jmp	(r28)		/* All synced up		*/

/*
** The minidebugger requires cache block alignment.
*/
.align 6
minidebugger:				/* the minidebugger is appended */
.end
