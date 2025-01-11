/*****************************************************************************

Copyright 1994, 1995, 1996 Digital Equipment Corporation, Maynard, Massachusetts.

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

#define RCSID "$Id: srom.s,v 1.1.1.1 1998/12/29 21:36:12 paradis Exp $"

/*
 * --------------------------------------------------------------------
 * This is source for the Serial ROM code that is loaded into the
 * Icache after CPU reset.  This code is modified to support system
 * implementations based on the Alpha AXP microprocessors.  These
 * processors provide a mechanism for loading their initial instruction
 * stream (I-stream) from a compact serial ROM (SROM) to start the
 * bootstrap procedure.  This code is limited to the size of the
 * processor's instruction cache and is relatively difficult to debug.
 * Therefore, it is suggested that the scope and purpose of this
 * code be limited to performing the system initialization
 * necessary to boot the next level of firmware contained in the
 * larger "System ROM".
 *
 * These are the initialization steps:
 *
 *	1..Initialize the CPU's Internal Processor Registers.
 *	2..Initialize the global registers
 * 	3..Set up internal caches.
 *	4..Perform the minimum I/O subsystem initialization
 *	   necessary to access the Real Time Clock (RTC)
 *	   and the System ROM.
 *	5..Detect CPU speed by polling the Periodic Interrupt
 *	   Flag in the RTC.
 *	6..Set up memory/bcache parameters based on the speed
 *	   of the CPU.
 *	7..Wake up the DRAMs.
 *	8..Size Memory.
 * 	9..Initialize memory (and BCache if on).
 * 10..Scan System ROM for special header described
 *	   herein which specifies where and how System ROM
 *	   firmware should be loaded.
 * 11..Copy the contents of the System ROM to memory and
 *	   begin its execution.
 * 12..Pass parameters as described herein up to the
 *	   next level of firmware.
 *
 *	Author: Richard Bissen, Digital Equipment Corporation
 *	Date: 6-Sept-1994
 *	Origins: PC164 SROM code by Rogelio R. Cruz,
 *		 Digital Equipment Corporation
 *
 * $Log: srom.s,v $
 * Revision 1.1.1.1  1998/12/29 21:36:12  paradis
 * Initial CVS checkin
 *
 * Revision 3.2  1997/08/14  15:26:49  pbell
 * Added a deposit into the memory controler debug register to
 * rout the signal required for the DMA prefetch fix.
 *
 * Revision 3.1  1997/05/07  17:56:54  bissen
 * Original release from RBB.
 *
 * --------------------------------------------------------------------*/

/*
 * --------------------------------------------------------------------
 *
 * When the next level of software is entered the machine state is.
 *
 *	DCACHE parity reporting is disabled.
 *
 *	Ibox Control/Status: icsr =
 *	  ICSR_SLE | ICSR_M_TMD | ICSR_M_FPE | ICSR_M_HWE | ICSR_M_SDE
 *
 *	Serial Line Interrupts are enabled.
 *	Ibox Timeout Counter is disabled.
 *	Floating Point enabled.
 *	PALRES instructions can be issued in Kernel mode.
 *
 *	PALBASE	= unchanged.  (Normally cleared by hardware reset)
 *	PS	= 0
 *	Exception Summary: excSum = 0 and mask cleared
 *	CC 	= enabled and started at 0
 *
 *	Icache
 *		is flushed.
 *
 *	Dcache
 *		is enabled
 *
 *	Scache
 *		is enabled
 *
 *	BCache 
 *		is enabled, corrects error on fill data, generate QW ECC,
 *		ignores ECC errors from Bcache or memory.
 *
 *	PYXIS
 *		Window base 0-3 set to 0
 *		Diagnostic register reset.
 *		HAE_IO set to 0.
 *		PCI Laterncy set to 0xFF
 *		
 *
 *	Memory
 *		Refresh is started
 *		all of memory is parity and ECC clean.
 *
 * --------------------------------------------------------------------
 *
 *	SROM Initialization Output Parameters
 *	=====================================
 *
 *	r1 (t0) - backup cache control register
 *	r2 (t1) - backup cache configuration register
 *	r3 (t2) - backup cache configuration register in OFF state.
 *	r15 (s6) - encoded srom.s RCS revision
 *	r16 (a0) - Processor Identification
 *	r17 (a1) - size of contiguous, good memory in bytes
 *	r18 (a2) - cycle count in picoseconds
 *	r19 (a3) - signature, and system revision id
 *	r20 (a4) - active processor mask
 *	r21 (a5) - system context value
 *
 *	Detailed descriptions:
 *	
 *	r1 (t0) - bcCtl
 *	-----------------------
 *   	BC_CONTROL (bcCtl)
 *
 *	The bcCtl register is a write-only register that controls
 *	the behavior of the backup cache.  SD164 does not have
 *	a backup cache.  This register is initialized and its
 *	value is passed on to the next level of firmware.
 *
 *	r2 (t1) - bcCfg
 *	-----------------------
 *   	BC_CONFIG (bcCfg)
 *
 *	The bcCfg register is a write-only register that controls
 *	the configuration of the backup cache.  Although the SD164
 *	lacks a backup cache, certain fields in this register must
 *	be initialized depending on the hardware setup.  In particular
 *	the BC_WR_SPD and BC_RD_SPD have to be set equal to the
 *	SYS_clock to CPU_clock ratio per the DC21164 spec.
 *
 *	r15 (s6) - encoded srom.s RCS revision
 *	--------------------------------------
 *	An encoded value representing the RCS revision of the
 *	source file ranging from revision 1.1 to 256.256.256.256.256.256.256
 *	The encoding also represents whether the source file is
 *	checked out of the library and locked for modification.
 *
 *	r16 (a0) - Processor Identification
 *	-----------------------------------
 *	The register has the following format:
 *
 *	63  ..  32          31  ..  0
 *	----------          ----------
 *	Minor Type          Major Type
 *
 *	(Alpha_SRM registered Processor Identification) 28-NOV-1995
 *          Major Type
 *             Value:
 *               1 = EV3
 *               2 = EV4 (21064)
 *                     Minor Type
 *                       Value:
 *                         0 = Pass 2 or 2.1
 *                         1 = Pass 3 (21064/150) or EV4s (21064/200)
 *               3 = Simulation
 *               4 = LCA Family
 *                       LCA4s (21066/166)
 *                       LCA4s embedded (21068/66)
 *			 LCA45 (21066A, 21068A)
 *                     Minor Type
 *                       Value:
 *                         0 = Reserved
 *                         1 = Pass 1 or 1.1 (21066)
 *                         2 = Pass 2 (21066)
 *                         3 = Pass 1 or 1.1 (21068)
 *                         4 = Pass 2   (21068)
 *			   5 = Pass 1 (21066A)
 *			   6 = Pass 1 (21068A)
 *
 *              5 = EV5  (21164)
 *                     Minor Type
 *                       Value:
 *                         0 = Reserved
 *                         1 = Pass 2 & 2.2 (Rev BA, CA)
 *                         2 = Pass 2.3 (Rev DA, EA)
 *                         3 = Pass 3
 *			   4 = Pass 3.2
 *			   5 = Pass 4
 *
 *              6 = EV45 (21064A)
 *                     Minor Type
 *                       Value:
 *                         0 = Reserved
 *                         1 = Pass 1
 *                         2 = Pass 1.1
 *                         3 = Pass 2
 *
 *              7 = EV56  (21164A)
 *                     Minor Type
 *                       Value:
 *                         0 = Reserved
 *                         1 = Pass 1
 *
 *	r17 (a1) - memory size
 *	----------------------
 *	This value is an unsigned quadword count of the number
 *	of contiguous bytes of "good" memory in the system starting
 *	at physical address zero.  This simple mechanism will be
 *	sufficient for simple systems. Systems that need to
 *	communicate more detailed memory configuration may do so
 *	via the system context value (see below).
 *
 *	r18 (a2) - cycle count in picoseconds
 *	-------------------------------------
 *	This value is the number of picoseconds that elapse for
 *	each increment of the processor cycle count (as read by
 *	the rpcc instruction).  Note that this may be a multiple
 *	of the actual internal cycle count of the microprocessor
 *	as specified in the Alpha Architecture Reference Manual
 *	(a microprocessor will increment the processor cycle count
 *	a multiple of the microprocessor clock where the multiple
 *	is a power of 2, including 2^0 = 1).
 *
 *	r19 (a3) - signature, and system revision id
 *	--------------------------------------------
 *	This register includes a signature which identifies that
 *	the transfer is following the standard protocol and that
 *	the other values may be trusted.  In addition, the signature
 *	can identify which version of the protocol is being followed.
 *	The system revision id is a 16-bit field that communicates
 *	system revisions that would be significant to operating system
 *	software.  The register has the following format:
 *
 *	63  ..  32          31 ..  16        15  ..  0
 *	----------          ---------        ---------
 *	don't care          Signature        System Revision
 *
 *	The valid signatures have the following values:
 *	0xdeca  - V1 (previous version of this spec)
 *	0xdecb  - V2 (current version of this spec)
 *
 *	r20 (a4) - active processor mask
 *	--------------------------------
 *	The processor mask identifies each processor that is present
 *	on the current system.  Each mask bit corresponds to a
 *	processor number assopyxisted by the bit number (i.e. bit 0
 *	corresponds to processor 0). A value of 1 in the mask indicates
 *	that the processor is present, a value of 0 indicates that the
 *	processor is not present.  To qualify as present a processor
 *	must be (a) physically present (b) functioning normally and
 *	(c) capable of sending and receiving interprocessor interrupt
 *	requests.  Uniprocessor systems will pass a value of 1 in
 *	this register. 
 *
 *	r21 (a5) - system context value
 *	-------------------------------
 *	The context value is interpreted in a system-specific manner.
 *	If the system needs to pass more than 1 system-specific
 *	parameter then it may pass a context value which is a physical
 *	address pointer to a data structure of many system-specific
 *	values.
 *
 */


#include "dc21164.h"
#include "io.h"
#include "pyxis.h"

#ifndef MAKEDEPEND
#include "rcsv.h"
#endif


#ifdef DEBUG_BOARD
#ifndef HARD_MEM_SIZE
#define HARD_MEM_SIZE		(64<<20)    /* Hardwire size to 64MBs */
#endif
#endif

#define STALL	mfpr	r31, pt0	/* 1 cycle stall.		*/


	
.align 3
/* ======================================================================
 * 1..Initialize the CPU's Internal Processor Registers.
 * ======================================================================
 *
 * 	Upon powerup, the DCache contains random data which can cause
 *	all sorts of problems (parity/ecc errors, etc) if we perform
 *	any type of load or store, even if they are to the CBOX 
 *	registers!  Therefore, we will flush the DCache before we
 *	attempt to do any LD/ST operation.
 *	
 *	NOTE: 	There must not be any MBOX instructions in n+1 and n+2
 *		after the write to dcFlush.
 */
initIPRs:
	mtpr	r31, dcFlush		/* Flush the dcache		*/
	mtpr	r31, dcMode		/* disable dcache		*/
	STALL				/* No MBOX instruction in cycles*/
	STALL				/* 1,2,3 and 4.			*/
	STALL
	STALL
	STALL


/*
 * 	Initialize ICCSR
 * ======================================================================
 *
 *	name	bit   value	description
 *	----	---   -----	----------------------------------------
 *	tmd	25	1	If set, disable the Ibox Timeout Counter.
 *	fpe	26	1	if set, enable floating point instructions.
 *	hwe	27	1	if set, enable PALRES instr's in kernel mode.
 *	spe<0>	28	0	if set, enable superpage mapping of Istream
 *				VA<42:30> = 1FFE directly to PA<39:30> = 0.
 *	spe<1>	29	0	if set, enable superpage mapping of Istream
 *				VA<39:13> directly to PA<39:13> if
 *				VA<42:41>=10.
 *	sde	30	0	Enable PAL shadow registers.
 *	sle	33	1	Enable serial line interrupts.
 *	ista	38	x	Read this bit to get Icache BIST status.
 *	tst	39	1	Set to 1 to assert test_status_h
 *
 * 	NOTE: There must be no PALshadow read/write in cycle n, n+1, n+2, n+3, n+4!!!
 *
 *	NT can't handle 64-bit constants so build ICSR constant as a 32 bit value.
 */
#define ICSR_VAL ( (1 << (ICSR_V_BSE-16)) | (1 << (ICSR_V_TMD-16)) | \
		   (1 << (ICSR_V_FPE-16)) | (1 << (ICSR_V_HWE-16)) | (0 << (ICSR_V_SDE-16)) | \
		   (1 << (ICSR_V_SLE-16)) | (1 << (ICSR_V_TST-16)))

	LDLI(r2, ICSR_VAL)
	sll	r2, 16, r2
	mtpr	r2, icsr

	mtpr	r31, cc			/* Initialize and start the 	*/
	lda	r1, 0x1(r31)		/* cycle counter.  		*/
	sll	r1, 32, r1		
	mtpr	r1, ccCtl		/* clear cycle count 		*/
	mtpr	r31, ips		/* Initialize PS to kernel mode.*/
	mtpr	r31, itbIa		/* clear the ITB 		*/
	mtpr	r31, dtbIa		/* clear the DTB 		*/
	mtpr	r31, dtbCm		/* set new ps<cm>=0, Mbox copy 	*/
	mtpr	r31, dtbAsn		/* ASNs */
	mtpr	r31, itbAsn
	mtpr	r31, aster		/* stop ASTs (enable)  		*/
	mtpr	r31, astrr		/* stop ASTs (request) interrupt*/
	mtpr	r31, sirr		/* clear software interrupts 	*/


/* ======================================================================
 * 2..Initialize the global registers.					=
 * ======================================================================
 * Make sure that there's no change to ICSR_M_SDE in the previous four
 * cycles; otherwise, we may be writing to the architected registers
 * instead of the shadow ones (or viceversa).
 *
 * Output Registers: 
 *	R13 - Pointer to CBOX register = FF.FFF0.0000
 *	R14 - Pointer to PYXIS base register = 87.4000.0000
 */
initGlobals:
	ldah	r0, SEXT(0xfff0)(r31)		/* r13 <- CBOX base pointer 	*/
	zap	r0, 0xE0, r13
	load_csr_base(r14)		/* Base for config registers.	*/
	bsr	r28, ClearCPUErrors	/* Clear errors before starting	*/

/* ======================================================================
 * 3..Set up internal caches.						=
 * ======================================================================
 *
 *   Previously, there was no way to determine dynamically which Scache
 *   sets were good, so we were told by manufacturing which sets to
 *   enable for a particular chip.   By default, all three sets would be 
 *   enabled.  If we had a chip with one bad set, then this code would
 *   have to be recompiled to indicate which set to use (a good set) with 
 *   one of the following switches on:
 *
 *   	Use Set		Compile Switch
 *      -------		--------------
 *	  S0		  -DSC_S0
 * 	  S1		  -DSC_S1
 *	  S2		  -DSC_S2
 *
 *   Note that EV5 requires 1 or all sets to be on. 
 *
 *   Recently, however, manufacturing has come up with a way to blow
 *   fuses on the chip to indicate which sets are bad.  These fuses
 *   can be "read" by reading from the SC_CTL register.  The current
 *   code makes use of this new feature. 
 *
 *   Note that in EV5, the SCache can not be turned off, therefore, 
 *   whenever we want something to get to memory or to be read from memory
 *   we must make sure that it's not in the SCache.  We do this by flushing
 *   the scache block that contains the data.  This is a difficult task
 *   if all three scache sets are on since it uses a random replacement
 *   algorithm to decide which block to kick out, hence, we'll just turn
 *   on one of the sets for now.
 *   
 *   Also, the DCache is left disabled until memory has been configured.
 */
#if defined (SC_S0)
#define SC_BANKS		1
#elif defined (SC_S1)
#define SC_BANKS		2
#elif defined (SC_S2)
#define SC_BANKS		4
#else
#undef SC_BANKS
#endif

SetScache:
#ifdef SC_BANKS				/* Hard-code the good sets.	*/
	LDLI	(r1, (SC_BANKS << SC_V_SET_EN))
#else
	bsr	r28, SCacheSets		/* Identify good SCache sets.	*/			 		
#endif /* ifdef SC_BANKS */
					/* Flush and set 64-byte blocks	*/
	lda	r16, (SC_M_FLUSH | SC_M_BLK_SIZE)(r31)
	bis	r1, r16, r16		/* Turn on 1 set only.		*/
	mb
	stq_p	r16, scCtl(r13)		/* leading and trailing "MB"s   */
	mb

	ldq_p	r2, scStat(r13)		/* Clear sc_stat and sc_addr 	*/
	ldq_p	r2, eiStat(r13)		/* Clear ei_stat, ei_addr, etc	*/



/*
 *	Time to do a soft reset of the Pyxis chip
 */
	LDLI(r16, 200000)		/* wait for at least 250usec	*/
	bsr	r27, wait_n_cycles	/* based on a max speed of 80MHz */


	ldah	r16, 0x4000(r14)	/* build the 87.8000.0900 address */
	lda		r8, 0x900(r16)
	LDLI(r17, 0xDEAD)
	stl_p	r17, 0(r8)			/* reset the Pyxis */
	mb

	LDLI(r16, 2000000)
	bsr	r27, wait_n_cycles

/*
 *	Program the Memory Controller Debug Register to route the signal
 *	required for the DMA prefetch fix.
 */

	LDLI	(r17, 0xAE170027)
	ldah    r16, 0x1000(r14)	/* r16 <- MCR ptr		*/
	lda		r16, 0x500(r16)		/* MDR1 offset */
	stl_p	r17, 0(r16)			/* set mdr1 register */
	mb

	ldah	r16, 0x1000(r14)	/* build the 87.5000.0040 address */
	lda		r8, 0x40(r16)
	LDLI(r17, 0x10ff)
	stl_p	r17, 0(r8)			/* reset Pyxis memory clocks */
	mb


/* ======================================================================
 * 4..Perform the minimum I/O subsystem initialization necessary to access 
 * the Real Time Clock (RTC) and the System ROM.
 * ======================================================================
 */
InitPCI:
					/* Turn off all windows.	*/
	bis	r31, r31, r17		/* r17 <- zero data 		*/
	load_pyxis_adrtrn_base(r16)	/* Base for PCI address transl.	*/
	lda	r16, w_base0(r16)	/* r16 <- addr of w_base0	*/
	lda	r7, 4(r31)		/* r7 <- loop_ctr		*/

wbase_loop:
	bsr	r26, srom_wrtest	/* write and test w_baseN	*/
	lda	r16, 0x100(r16)		/* increment ptr to next w_base */
	lda	r7,  -1(r7)			/* decrement loop_ctr		*/
	bgt	r7,  wbase_loop		/* branch while greater than 0. */

	lda	r16, MC_PYXIS_DIAG(r14)	/* r16 <- pointer to pyxis_diag	*/
	bsr	r26, srom_wrtest	/* write and test pyxis_diag	*/
	lda	r16, MC_HAE_IO(r14)	/* r16 <- pointer to hae_io	*/
	bsr	r26, srom_wrtest	/* write and test hae_io	*/

	lda	r16, MC_PCI_LAT(r14)	/* r16 <- pointer to pci_lat	*/
	LDLI	(r17, 0xFF00)		/* r17 <- data for pci_lat	*/
	bsr	r26, srom_wrtest	/* write and test pci_lat	*/

/*
 *  Initialize the PYXIS.
 */
#define PYXIS_CTRL_K_INIT ((1 << MC_PYXIS_CTRL_V_PCI_EN)		| \
			(0 << MC_PYXIS_CTRL_V_PCI_LOOP_EN) 	| \
			(0 << MC_PYXIS_CTRL_V_FST_BB_EN) 		| \
			(1 << MC_PYXIS_CTRL_V_PCI_MST_EN)		| \
			(1 << MC_PYXIS_CTRL_V_PCI_MEM_EN) 	| \
			(1 << MC_PYXIS_CTRL_V_PCI_REQ64_EN) 	| \
			(1 << MC_PYXIS_CTRL_V_PCI_ACK64_EN)	| \
			(0 << MC_PYXIS_CTRL_V_ADDR_PE_EN)		| \
			(0 << MC_PYXIS_CTRL_V_PERR_EN)		| \
			(0 << MC_PYXIS_CTRL_V_FILLERR_EN)		| \
			(0 << MC_PYXIS_CTRL_V_MCHKERR_EN)		| \
			(0 << MC_PYXIS_CTRL_V_ECC_CHK_EN)		| \
			(0 << MC_PYXIS_CTRL_V_ASSERT_IDLE_BC)	| \
			(0 << MC_PYXIS_CTRL_V_RD_TYPE) 		| \
			(0 << MC_PYXIS_CTRL_V_RL_TYPE) 		| \
			(0 << MC_PYXIS_CTRL_V_RM_TYPE)		)

	lda	r16, MC_PYXIS_CTRL(r14)	/* r16 <- pointer to pyxis_ctrl	*/
	LDLI	(r17, PYXIS_CTRL_K_INIT)   /* r17 <- pyxis_ctrl_k_init	*/
	bsr	r26, srom_wrtest	/* write and test pyxis_ctrl	*/

/*
 *	Initialize the interrupt configuration, flash control, and
 *	interrupt routing.
 */

	lda	r16, MC_PYXIS_CTRL1(r14)	/* r16 <- pointer to pyxis_ctrl1	*/
	bis		r31, MC_PYXIS_CTRL1_M_IOA_BEN, r17
	bsr	r26, srom_wrtest	/* write and test pyxis_ctrl	*/

	lda	r16, MC_FLASH_CTRL(r14)	/* r16 <- pointer to flash_ctrl	*/
	bis	r31, r31, r17		/* r17 <- data for flash_ctrl	*/
	bsr	r26, srom_wrtest	/* write and test pci_lat (disable flash) */

/*
 *	Route Fan and NMI to sys_mch_chk_irq, and Halt to mch_halt_irq.
 *	(This may not be required on the LX164)
 */
#define INT_ROUTE_K_INIT  (MC_INT_ROUTE_M_FAN | MC_INT_ROUTE_M_NMI | \
						   MC_INT_ROUTE_M_HALT)

	ldah	r8, 0x6000(r14)			/* Base of the Interrupt Controller Registers */
	lda	r16, MC_ICR_INT_ROUTE(r8)	/* r16 <- pointer to int_route reg */
	bis	r31, INT_ROUTE_K_INIT, r17	/* Default interrupt routing */
//	bsr	r26, srom_wrtest			/* write and test intr_routing register */

	bsr	r26, InitSIO		/* Init SIO, ISA bus and others	*/
	OutLEDPORT(0x20)

	
/*
 *  Turn on the logical devices in the SMC FDC37C93X part.  To do this, we
 *  must first put the device in its configuration state by writting
 *  0x55, 0x55 to the configuration port 0x3F0
 */
#include "smc.h"
InitSMC:
	lda	r16, CONFIG_ON_KEY(r31)
	bsr	r29, WriteSMC_Config1	/* Put device in configuration 	*/
	bsr	r29, WriteSMC_Config1	/* mode.			*/

	lda	r16, LOGICAL_DEVICE_NUMBER(r31) /* Select Real Time Clock.*/
	bsr	r29, WriteSMC_Index1	
	lda	r16, RTCL(r31)
	bsr	r29, WriteSMC_Data1

	lda	r16, 0x70(r31)		/* Set primary interrupt to IRQ8*/
	bsr	r29, WriteSMC_Index1
	lda	r16, 8(r31)		
	bsr	r29, WriteSMC_Data1

	lda	r16, ACTIVATE(r31)
	bsr	r29, WriteSMC_Index1	/* Turn RTC on.			*/
	lda	r16, DEVICE_ON(r31)
	bsr	r29, WriteSMC_Data1
	lda	r16, CONFIG_OFF_KEY(r31)
	bsr	r29, WriteSMC_Config1	/* Put device in run mode. 	*/	



/* ======================================================================
 * 5..Detect CPU speed by polling the Periodic Interrupt Flag in the RTC.
 * ======================================================================
 *
 */
DetectSpeed:
#ifdef RTC_DELAY
/* 
 * Guarantee that the RTC is stable by waiting after reset at least 500us.
 * For maximum CPU speed supported, this would be 500Mhz * 500us = 250,000
 * cycles.  To save instructions, I'll round up to 0x40000 = 262, 144 cycles
 *
 *  Disabled.  It was found that delay was not needed since the power up
 *  hardware provided enough delay.
*/
	ldah	r16, 4(r31)		
	bsr	r27, wait_n_cycles 	
#endif /* ifdef RTC_DELAY */
					
	bsr 	r28, GetCPUSpeed	/* Detect the CPU speed.	*/
	bis	r0, r0, r17			/* Speed in cycles/second (Hz). */
	lda	r16, 9600(r31)		/* Baud rate SROM serial port	*/
	bsr	r26, nintdivide		/* Compute cycles per bit.	*/

// RBB - #ifndef DEBUG_BOARD			/* Don't set baud if debug version*/
	SET_BAUD (r0)			/* Set baud rate for srom port	*/
// RBB - #endif

	PRINTS8 (CPU_STRING_H, CPU_STRING_L) /* Print CPU string.	*/
	OutLEDPORT(0x01)

/*
 *	(10**12)/(cycles per second) = CPU clock period in picoseconds
 */
	bis	r17, r17, r16		/* r16 = cycles/second		*/
	LDLI(r3, 0xe8d4a510)	/* Load up (10**12)>>8		*/
	zap	r3, 0xf0, r17		/* Remove sign extension	*/
	sll	r17, 8, r17			/* r17 = 10**12			*/
	bsr	r26, nintdivide		/* Compute CPU speed		*/
	bis r0, r0, r24			/* r24=CPU speed (picoseconds)	*/
 	OutLEDPORT(0x02)

#ifdef DEBUG_BOARD
	jump_to_mini_prompt()		/* trap to mini, no autobaud	*/
#endif	/* ifdef DEBUG_BOARD */


/* ======================================================================
 * 6..Set up memory/bcache parameters based on the speed of the CPU.	=
 * ======================================================================
 */
/*
 *	System specific Jumper configurations are
 *	Mapped into this generic configuration definition.
 *	==================================================
 *
 *	bit26:	BC_WR_Fast (0=Fast, 1=Slow)
 *	bit25:	BC_NoAllocate
 *	bit24:  Boot_Option (0=2nd image, 1= 1st image)
 *	bit23:  Mini-Debugger (0=trap to mini debugger 1= don't trap)
 *	bit22:  BC_RD_Fast (0= drop 1 read cycle, 1=don't drop)
 *	bit21:  BC_Speed<2>
 *	bit20:  BC_Speed<1>
 *	bit19:  BC_Speed<0>
 *
 *		BC_Speed<2:0>	Bcache
 *		------------	-------
 *		   000		 9ns (BC_SPD_0)
 *		   001		 6ns (BC_SPD_1)
 *		   010		 8ns (BC_SPD_2)
 *		   011		10ns (BC_SPD_3)
 *		   100		12ns (BC_SPD_4)
 *		   101		15ns (BC_SPD_5)
 *		   110		17ns (BC_SPD_6)
 *		   111		reserved (BC_SPD_7)
 *
 *	bit18:  BC_Size<2>
 *	bit17:  BC_Size<1>
 *	bit16:  BC_Size<0>
 *
 *		BC_Size<2:0>	Bcache
 *		------------	-------
 *		   000		Disabled
 *		   001		512KB
 *		   010		1MB
 *		   011		2MB
 *		   100		4MB
 *		   101		8MB
 *		   110		16MB
 *		   111		32MB
 *
 *	bit15:  bank 3 PD4 bit
 *	bit14:  bank 3 PD3 bit
 *	bit13:  bank 3 PD2 bit
 *	bit12:  bank 3 PD1 bit
 *	bit11:  bank 2 PD4 bit
 *	bit10:  bank 2 PD3 bit
 *	bit9:  bank 2 PD2 bit
 *	bit8:  bank 2 PD1 bit
 *	bit7:  bank 1 PD4 bit
 *	bit6:  bank 1 PD3 bit
 *	bit5:  bank 1 PD2 bit
 *	bit4:  bank 1 PD1 bit
 *	bit3:  bank 0 PD4 bit
 *	bit2:  bank 0 PD3 bit
 *	bit1:  bank 0 PD2 bit
 *	bit0:  bank 0 PD1 bit
 */
#define JMP_V_BNK0_SIZE		0	/* Presence Detect for bank 0.	*/
#define JMP_V_BNK0_SPEED	2	/* Presence Detect for bank 0.	*/
#define JMP_V_BNK1_SIZE		4	/* Presence Detect for bank 1.	*/
#define JMP_V_BNK1_SPEED	6	/* Presence Detect for bank 1.	*/
#define JMP_V_BNK2_SIZE		8	/* Presence Detect for bank 2.	*/
#define JMP_V_BNK2_SPEED	10	/* Presence Detect for bank 2.	*/
#define JMP_V_BNK3_SIZE		12	/* Presence Detect for bank 3.	*/
#define JMP_V_BNK3_SPEED	14	/* Presence Detect for bank 3.	*/
#define JMP_V_CACHE_SIZE	16	/* Cache size bits.		*/
#define JMP_V_CACHE_SPEED	19	/* Cache speed bits.		*/

#define JMP_S_BNK0_SIZE		2
#define JMP_S_BNK0_SPEED	2
#define JMP_S_BNK1_SIZE		2
#define JMP_S_BNK1_SPEED	2
#define JMP_S_BNK2_SIZE		2
#define JMP_S_BNK2_SPEED	2
#define JMP_S_BNK3_SIZE		2
#define JMP_S_BNK3_SPEED	2
#define JMP_S_CACHE_SIZE	3

#define JMP_M_BNK0_SIZE		(((1<<JMP_S_BNK0_SIZE)-1) << JMP_V_BNK0_SIZE)
#define JMP_M_BNK0_SPEED	(((1<<JMP_S_BNK0_SPEED)-1) << JMP_V_BNK0_SPEED)
#define JMP_M_BNK1_SIZE		(((1<<JMP_S_BNK1_SIZE)-1) << JMP_V_BNK1_SIZE)
#define JMP_M_BNK1_SPEED	(((1<<JMP_S_BNK1_SPEED)-1) << MP_V_BNK1_SPEED)
#define JMP_M_BNK2_SIZE		(((1<<JMP_S_BNK2_SIZE)-1) << JMP_V_BNK2_SIZE)
#define JMP_M_BNK2_SPEED	(((1<<JMP_S_BNK2_SPEED)-1) << JMP_V_BNK2_SPEED)
#define JMP_M_BNK3_SIZE		(((1<<JMP_S_BNK3_SIZE)-1) << JMP_V_BNK3_SIZE)
#define JMP_M_BNK3_SPEED	(((1<<JMP_S_BNK3_SPEED)-1) << JMP_V_BNK3_SPEED)
#define JMP_M_CACHE_SIZE	(((1<<JMP_S_CACHE_SIZE)-1) << JMP_V_CACHE_SIZE)


#define JMP_V_BC_WR_FAST	26	/* Write Fast jumper.		*/
#define JMP_V_BC_NOALLOC	25	/* Don't allocate jumper.	*/
#define JMP_V_ALT_BOOT 		24	/* Boot alternate image jumper	*/	
#define JMP_V_MINIDBGR		23	/* Trap to minidbgr jumper.	*/
#define JMP_V_BC_RD_FAST	22	/* Read Fast jumper		*/


#define BC_9NS	 0
#define BC_6NS	 1
#define BC_8NS	 2
#define BC_10NS	 3
#define BC_12NS	 4
#define BC_15NS	 5
#define BC_17NS	 6
#define BC_20NS	 7


#define BC_SPD_0 (0 * 8)
#define BC_SPD_1 (1 * 8)
#define BC_SPD_2 (2 * 8)
#define BC_SPD_3 (3 * 8)
#define BC_SPD_4 (0 * 8) /* Represents bytes 0-3 of upper longword */
#define BC_SPD_5 (1 * 8)
#define BC_SPD_6 (2 * 8)
#define BC_SPD_7 (3 * 8)

#define B0 (0 * 8)
#define B1 (1 * 8)
#define B2 (2 * 8)
#define B3 (3 * 8)
#define B4 (0 * 8)  /* Represents bytes 0-3 of upper longword */
#define B5 (1 * 8)
#define B6 (2 * 8)
#define B7 (3 * 8)


ReadConfigJumpers:
#ifdef HARD_JUMPERS
/*
 *	HARD_JUMPERS can be defined to override the jumper
 *	configuration on the board.  This also disables
 *	any attempt to read the configuration jumpers on
 *	the board.
 */
	LDLI(r30, HARD_JUMPERS)
	zap	r30, 0xf0, r30	/* r30 = HARD_JUMPERS<31:0> */
#else

/*
 *	LX164 Jumper configurations
 *	============================================================
 *
 *	801 - Configuration jumpers conf0-conf7.
 *
 *     Default jumper configuration: all jumpers out
 *
 *      :    :    :    :    :    :    :    :
 *     CF0  CF1  CF2  CF3  CF4  CF5  CF6  CF7
 *
 *     Input bit:					Maps to bit:
 *
 * (conf-7) bit7:  Boot_Option (0 = 1st image, 1=alternate)	[24]
 * (conf-6) bit6:  Mini-Debugger (0=enabled 1= disabled)	[23]
 * (conf-5) bit5:  BC_Speed<0>					[19]
 * (conf-4) bit4:  BC_Speed<1>					[20]
 * (conf-3) bit3:  PASSWORD Bypass
 *
 *		BC_Speed<1:0>	Bcache
 *		------------	-------
 *		   00		Reserved
 *		   01		Reserved
 *		   10		Reserved
 *		   11		9ns (default)
 *
 *
 * (conf-2) bit2:  BC_Size<0>		 			[16]
 * (conf-1) bit1:  BC_Size<1>					[17]
 * (conf-0) bit0:  CPU Speed Limit
 *
 *		BC_Size<1:0>	Bcache
 *		------------	-------
 *		   00		Disable BCache
 *		   01		1MB
 *		   10		4MB
 *		   11		2MB (default)
 *
 *  1 = jumper out, 0 = jumper in.
 */
	bsr	r29, ReadConf1		/* R0 <- Value at addr 0x801	*/
	ornot	r31, r0, r1		/* Negate boot option bit.	*/
	and	r1, 0x80, r30		/* Isolate boot bit		*/
	and	r0, 0x40, r1		/* Isolate the mini-dbg bit.	*/
	bis	r30, r1, r30		/* Merge the two bits.		*/
	bis	r30, 0x20, r30		/* Set rd_fast to don't drop.	*/
	sll	r30, 17, r30
	bis	r31, r31, r3		/* Valid for Disable Bcache 	*/
	and	r0, 6, r1			/* Isolate the cache size bits.	*/
	cmpeq	r1, 4, r2		/* Is it 1MB?  If so remap.	*/
	cmovne	r2, 2, r3
	cmpeq	r1, 6, r2		/* Is it 2MB? If so remap.	*/
	cmovne	r2, 3, r3
	cmpeq	r1, 2, r2		/* Is it 4MB?  If so remap.	*/
	cmovne	r2, 4, r3
	sll	r3, 16, r3			/* Place size in std. config.	*/
	bis	r3, r30, r30		/* Add in the cache size 	*/

#endif /* HARD_JUMPERS */

	OutLEDPORT(0x03)


#ifdef HARD_BCACHE_CONFIG
/*
 *	HARD_BCACHE_CONFIG can be defined to override the dynamic
 *	calculation of the BCACHE_CONFIG register.  The value of
 *	HARD_BCACHE_CONFIG will be loaded into the lower 32 bits
 *	of the BCACHE_CONFIG register.
 *	Here's an example for a 2MB,10ns bcache -DHARD_BCACHE_CONFIG=0x01C22782
 */
	LDLI(r21, HARD_BCACHE_CONFIG)
#else

/*
 *	LX164 (EV56 pass 1) EV56 BCACHE SETTINGS
 *	========================================
 *
 *	CPU Speed   Read Speed Pipeline   Write Speed	CpuSys Ratio
 *	400 MHz            9    4                6			6
 *	466 MHz           10    4                6			7
 *	533 MHz           12    4                7			8
 *	666 MHz           13    4                7			9
 *	666 MHz           15    4                8			10
 *
 *	                BC_CONFIG               BC_CONTROL
 *	400 MHz         00000000003F2469X       00000000C4008051
 *	466 MHz         00000000003F346AX       00000000C4008051
 *	533 MHz         00000000007F457CX       00000000C6008051
 *	600 MHz         00000000007F457DX       00000000C6008051
 *	666 MHz         0000000000FF458FX       00000000C6008051
 */

/* Need real values */	
	bsr	r29, ReadConf1		/* R0 <- Value at addr 0x801	*/
	bis	r0, r0, r6		/* save config info	*/
	LDLI	(r4, 0x0C400)		/* Pipeline of 4.		*/
	lda		r5, 6(r31)			/* CpuSys ratio of 6		*/
	LDLI	(r3, 0x04406)		/* Pipeline of 3.		*/
	LDLI	(r21, 0x3F24690)	/* BC Config for 400MHz.	*/
	bis	r4, r4, r8				/* Pipeline = 4 as default.	*/
	

	lda		r3, 7(r31)			/* CpuSys ratio of 7		*/
	lda	r0, 2189(r31)			/* 466MHz = 2146 +/- 2%.	*/
	LDLI	(r1, 0x3F346A0)		/* BC Config for 466 Hz.	*/
	cmpult	r24, r0, r0			/* Is it 466Mhz or faster?	*/
	cmovne	r0, r1, r21			/* If yes, then use this value.	*/
	cmovne	r0, r4, r8			/* Pipeline = 4.		*/
	cmovne	r0, r3, r5			/* CpuSysclock ratio 7		*/

	LDLI	(r4, 0x0C600)		/* This requires EV56 pass 2 */

	lda		r3, 8(r31)			/* CpuSys ratio of 8		*/
	lda	r0, 1914(r31)			/* 533MHz = 1876ps +/- 2%.	*/
	LDLI	(r1, 0x7F457C0)		/* BC Config for 533 MHz.	*/
	cmpult	r24, r0, r0			/* Is it 533Mhz or faster?	*/
	cmovne	r0, r1, r21			/* If yes, then use this value.	*/
	cmovne	r0, r4, r8			/* Pipeline = 4.		*/
	cmovne	r0, r3, r5			/* CpuSysclock ratio 7		*/
	
	lda		r3, 9(r31)			/* CpuSys ratio of 9		*/
	lda	r0, 1700(r31)			/* 600MHz = 1667ps +/- 2%.	*/
	LDLI	(r1, 0x7F457D0)		/* BC Config for 600MHz.	*/
	cmpult	r24, r0, r0			/* Is it 600Mhz or faster?	*/
	cmovne	r0, r1, r21			/* If yes, then use this value.	*/
	cmovne	r0, r3, r5			/* CpuSysclock ratio 7		*/
	
	lda		r3, 10(r31)			/* CpuSys ratio of 10		*/
	lda	r0, 1531(r31)			/* 666MHz = 1501ps +/- 2%.	*/
	LDLI	(r1, 0xFF458F0)		/* BC Config for 666 MHz.	*/
	cmpult	r24, r0, r0			/* Is it 666Mhz or faster?	*/
	cmovne	r0, r1, r21			/* If yes, then use this value.	*/
	cmovne	r0, r3, r5			/* CpuSysclock ratio 7		*/
SkipCpuSpeeds:
	blbc	r6, CpuSpeedOK		/* OK to run at selected speed	*/
	LDLI	(r3, 0x3F34690)			/* BC Config for 400MHz.	*/
	cmpeq	r21, r3, r3		/* If 400MHz we have an illegal speed	*/
	bne	r3, CpuSpeedError
	LDLI	(r3, 0xFF448F0)		/* BC Config for 666 MHz.	*/
	cmpeq	r21, r3, r3		/* If 666MHz we have an illegal speed	*/
	beq	r3, CpuSpeedOK

CpuSpeedError:
	lda	r8, LED_F_BADCPUSPEED(r31)	// Ledcode value to print.
	br	r31, FatalError			// No DIMMS found error

CpuSpeedOK:


/*
 *	Read BC_Size jumpers.				configuration<18:16>
 *	==================================================================
 *	These jumpers are used to set the BCache size.
 *
 *	Input registers:
 *	R21 - 	Partially computed value to write to BC_CNFG register.
 *	R30 -   Configuration register in standard format.
 *
 *	Output registers:
 *	R21 - 	Partially computed value to write to BC_CNFG register.
 *
 *	Register usage:
 *	R2  -	Scratch
 *	R3  -	Scratch
 */
set_bc_size:	
	srl	r30, JMP_V_CACHE_SIZE, r2 /* Shift down Cache Size, <2:0>*/
					/* Zap bits other bits		*/
	and	r2, (JMP_M_CACHE_SIZE >> JMP_V_CACHE_SIZE), r2		
	subq	r2, 1, r3		/* Translate to BCache _SIZE	*/
	cmoveq	r2, 0, r3		/* Set size to 0 if disabled.	*/
	sll	r3, BC_V_SIZE, r3	/* Shift into BC_SIZE field	*/
	bis	r21, r3, r21		/* Merge BC_SIZE into r21	*/
	bne	r2, bc_enabled		/* Skip over disabling code.	*/

bc_disabled:
	bsr	r28, BCnfgOff		/* R0 <- Cnfg value for BCache off*/
	bis	r0, r0, r21			/* Put back into global reg.	*/
bc_enabled:
#endif /* HARD_BCACHE_CONFIG	*/

	zap	r21, 0xf0, r21		/* r21 = BC_CONFIG<31:0> 	*/
	OutLEDPORT(0x04)

#ifdef HARD_BC_CTL
/*
 *	HARD_BC_CTL can be defined to override the Dynamic
 *	calculation of the BC_CTL register.  The value of
 *	HARD_BC_CTL will be loaded into the lower 32 bits
 *	of the BC_CTL register.
 *	Here's an example -DHARD_BC_CTL=0x8051
 *	PC164:  -DHARD_BC_CTL=0x444008051
 */
	LDLI(r20, HARD_BC_CTL)
	zap	r20, 0xF0, r20		/* r20 = BC_CTL<31:0> 		*/
	lda	r0, 4(r31)
	sll	r0, 32, r0
	bis	r20, r0, r20

#else
	/* Disable error reporting, set ecc mode & correct fill data = 0x8050.*/
	LDLI(r20, (BC_M_EI_DIS_ERR | BC_M_EI_ECC_OR_PARITY | BC_M_CORR_FILL_DAT))
	zap	r20, 0xF0, r20			/* r20 = BC_CTL<31:0> 		*/

	srl	r30, JMP_V_CACHE_SIZE, r0 /* Shift down Cache Size, <2:0>*/
								/* Zap bits other bits		*/
	and	r0, (JMP_M_CACHE_SIZE >> JMP_V_CACHE_SIZE), r0		
	lda	r3, BC_M_BC_ENA(r31)	/* Load mask for enabling.	*/
	cmoveq	r0, r31, r3			/* Don't enable the bcache.	*/
	bis	r20, r3, r20			/* Enable or disable cache.	*/


	sll	r8, 16, r8				/* The rest of the BCTL bits.	*/
	bis	r20, r8, r20

#endif /* HARD_BC_CTL */

	OutLEDPORT(0x05)

#ifdef DEBUG_BOARD
	jump_to_mini_prompt()		/* trap to mini, no autobaud	*/
#endif	/* ifdef DEBUG_BOARD */


/* 
 * 	Turn off BCache.
 * ======================================================================
 *	Regardless of setting, turn off the bcache to facilitate memory
 *	sizing later on.
 *
 *	R20 - BC_CTL
 *	R21 - BC_CNFG
 *	R24 - CPU Speed
 *	R30 - Configuration Register
 */
	bsr	r28, BCnfgOff		/* Compute BC_CNFG for OFF state*/
	bis	r0, r0, r22			/* Save OFF configuration.	*/

	OutLEDPORT(0x06)

#ifdef DEBUG_BOARD
	jump_to_mini_prompt()		/* trap to mini, no autobaud	*/
#endif	/* ifdef DEBUG_BOARD */



#ifdef HARD_MEM_SIZE
/*
 *  The real work starts here.  RBB
 *	Start by setting all the hardcoded values for a 64MB system with
 *	a 2MB cache.  I will need the info from Curtis to setup the bcache
 *  and I'll assume the Samsung DIMM used by APS.
 */

/*
 * Initialize the Clock Control Register
 */
#define PYXIS_CCR_K_INIT ((1 << MC_CCR_V_CLK_DIVIDE)	| \
			(3 << MC_CCR_V_PCLK_DIVIDE) 	| \
			(2 << MC_CCR_V_PLL_RANGE) 		| \
			(1 << MC_CCR_V_LONG_RESET) 		| \
			(0 << MC_CCR_V_CONFIG_SRC) 		| \
			(0 << MC_CCR_V_DCLK_INV) 		| \
			(0 << MC_CCR_V_DCLK_FORCE) 		| \
			(1 << MC_CCR_V_DCLK_PCSEL) 		| \
			(0x29 << MC_CCR_V_DCLK_DELAY)		)

	load_csr_base(r14)		/* Base for config registers.	*/
	ldah	r8, 0x4000(r14)	/* Base of the Miscellaneous Registers */
	lda	r16, MC_CCR(r8)		/* r16 <- pointer to CCR reg */
	LDLI	(r17, PYXIS_CCR_K_INIT)
	bsr	r26, srom_wrtest		/* write and test CCR */

	lda	r16, 20000(r31) 	/* load cycles into R16.	*/
	bsr	r27, wait_n_cycles 	/* Count for this many CPU cycles*/
					/* R0 <- number of cycles waited.*/

	LDLI	(r17, 0x332)		/* r17 <- GTR bits		*/
	ldah    r8, 0x1000(r14)		/* r8 <- PYXIS mem csr base ptr	*/
	lda	r16, MC_GTR(r8) 		/* r16 <- GTR ptr		*/
	bsr	r26, srom_wrtest		/* write and test it		*/

	LDLI	(r17, 0x750)		/* r17 <- RTR bits		*/
	lda	r16, MC_RTR(r8) 		/* r16 <- RTR ptr		*/
	bsr	r26, srom_wrtest		/* write and test it		*/

/*
 *	Force Refresh
 */
	bis	r31, 1, r4				/* create the force refresh bit */
	sll	r4, MC_RTR_V_FORCE_REF, r4
	bis	r17, r4, r17
/*
 *	Lets loop for 8 times
 */
	lda	r5, 8(r31)				/* Load loop counter */
ForceRef:
	lda		r16, MC_RTR(r8)		/* force refresh */
	bsr		r26, srom_wrtest

	lda	r4, 100(r31)
ForceRefWait:
	STALL
	subq	r4, 1, r4
	bne		r4, ForceRefWait

	subq	r5, 1, r5
	bne		r5, ForceRef

	bis		r17, r31, r5		/* save r17 -> r5 */
/*
 *	Setup the memory controler section based on the hardcoded values.
 *	The MCR should only set the BCACHE_ENABLE bit if the Bcache is actually
 *	turned on.  Note: It is set now, and because with the hard coded memory size
 *	and the IIC memory sizing the bcache will always be on.  This will have
 *	to change if we run with caches off.
 */
	ldah    r16, 0x1000(r14)	/* r16 <- MCR ptr		*/
	LDLI	(r17, 0x3A1401)		/* r17 <- MCR bits		*/
	bsr	r26, srom_wrtest		/* write and test it		*/

/*
 *	Lets loop for 8 times
 */
	bis		r5, r31, r17		/* restore r17 */
	lda	r5, 8(r31)				/* Load loop counter */
ForceRef1:
	lda		r16, MC_RTR(r8)		/* force refresh */
	bsr		r26, srom_wrtest

	lda	r4, 100(r31)
ForceRefWait1:
	STALL
	subq	r4, 1, r4
	bne		r4, ForceRefWait1

	subq	r5, 1, r5
	bne		r5, ForceRef1

/*
 *	Setup the bank registers to support two banks of 32MB of Samsung DIMMs
 */
	bis	r31, r31, r17			/* r17 <- BBA0 bits		*/
	lda	r16, MC_BBA0(r8) 		/* r16 <- BBA0 ptr base 0MB */
	bsr	r26, srom_wrtest		/* write and test it	*/

	lda	r17, 0x100(r31)			/* r17 <- BBA1 bits		*/
	lda	r16, MC_BBA1(r8) 		/* r16 <- BBA1 ptr base 64MB */
	bsr	r26, srom_wrtest		/* write and test it	*/

	lda	r17, 0x22(r31)			/* r17 <- BTRn bits		*/
	lda	r16, MC_BTR0(r8) 		/* r16 <- BTR0 ptr		*/
	bsr	r26, srom_wrtest		/* write and test it	*/

	lda	r16, MC_BTR1(r8) 		/* r16 <- BTR1 ptr		*/
	bsr	r26, srom_wrtest		/* write and test it	*/

	lda	r17, 0x28(r31)			/* r17 <- BCRn bits		*/
	lda	r16, MC_BCR0(r8) 		/* r16 -> BCR0 ptr		*/
	bsr	r26, srom_wrtest		/* write and test it	*/

	lda	r16, MC_BCR1(r8) 		/* r16 -> BCR1 ptr		*/
	bsr	r26, srom_wrtest		/* write and test it	*/

	bis	r17, 1, r17				/* Enable the banks		*/
	lda	r16, MC_BCR0(r8) 		/* r16 -> BCR0 ptr		*/
	bsr	r26, srom_wrtest		/* write and test it	*/

	lda	r16, MC_BCR1(r8) 		/* r16 -> BCR1 ptr		*/
	bsr	r26, srom_wrtest		/* write and test it	*/

	LDLI	(r23, 0x8000000)		/* Memory size in bytes */

	OutLEDPORT(0x07)

#ifdef DEBUG_BOARD
	jump_to_mini_prompt()		/* trap to mini, no autobaud	*/
#endif	/* ifdef DEBUG_BOARD */

#else	// !HARD_MEM_SIZE

//++
// ===========================================================================
// = srom_size_memory - size the main memory for volume                 =
// ===========================================================================
//
// OVERVIEW:
//
//        This function will size the main memory on an bank per bank basis
//
// FORM OF CALL:
//
//       bsr r26,srom_size_memory
//
// ARGUMENTS:
//
//	none
//	
// RETURNS:
//
//     r10 - size mask with total size of configurable memory
//     r11 - upper LW is speed of slowest DIMM on memory bus
//           lower LW is 1 if all DIMMs have ECC bits, else 0
//
// FUNCTIONAL DESCRIPTION:
//
//	This code uses the following definitions and assumptions:
//
//	1. A PC164LX / EV56+PYXIS based board will contain from 1 to
//	   two populated banks of SDRAM DIMMS.  Each bank will
//	   consist of two DIMMS to create the 128 bit wide memory
//	   bus.
//	2. Each DIMM is JEDEC JC-42.5-95.164 compliant or better.
//	   Thus each DIMM will have a JEDEC compliant IIC ROM
//	   containing full DIMM specification data.
//	3. DIMMS in each bank must be IDENTICAL in almost every
//	   way.  In addition all DIMMS will must meet a minimum
//	   set of timing specifications determined by reading the
//	   IIC ROM on each DIMM.  SDRAM DIMMS come in many more
//	   speed variants than older 72 pin SIMMs, and many are too
//	   slow for EV56 + PYXIS.
//	4. Single, Dual, and Quad bank DIMMS are supported in sizes
//	   from 16MB up to 256MB using either 16Mbit or 64Mbit SDRAMS.
//	5. PYXIS supports eight total banks, though PC164LX only uses
//	   two.
//
//
//	This function will size the amount of memory in the system by
//	reading the IIC ROM for each DIMM.  Banks containing mismatched
//	DIMMS will cause an error beep code and prevent normal machine
//	operation.
//
//	A data structure of the following format is built as a 
//	result of the memory sizing.  It represents the DIMM size
//	in a given bank.  PC164LX has two banks, and PYXIS supports eight.
//
//	PC164LX size_mask (DIMMs):
//	=========================
//	 63          56 55            48 
//	+--------------+----------------+
//	| 8MB          | 16MB           |
//	+--------------+----------------+
//
//	 47          40 39            32
//	+--------------+----------------+
//	| 32MB         | 64MB           |
//	+--------------+----------------+
//
//	 31          24 23            16
//	+--------------+----------------+
//	| 128MB        | 256MB          |
//	+--------------+----------------+
//
//	 15           8 7              0
//	+--------------+----------------+
//	| 512MB        | 1GB (!!)       |
//	+--------------+----------------+
//
//       Each bit within the size fields corresponds to a memory dimm size.
//	This means that for a particular bank only one bit can be set in a 
//	particular field:
//	
//	I.E.
//
//	if bank 1 is configured as a set of 128MB DIMMs then
//	bit 1  = 0   (it IS NOT a 1GB DIMM)
//	bit 9  = 1   (it IS     a 512MB DIMM)
//	bit 17 = 0   (it IS NOT a 256MB DIMM)
//
//
//	PC164LX DIMM feature bitmask (per BANK):
//	//jjd - DIMM matching code must be updated
//	//jjd - to accumulate this data per DIMM
//	//jjd - then check it for matches.
//	
//	 63          56 55      48 
//	+--------------+----------------+
//	| Mismatch err |                |
//	+--------------+----------------+
//
//	 47          40 39            32
//	+--------------+----------------+
//	| Slow Prechrg | Toshiba Mode   |
//	+--------------+----------------+
//
//	 31          24 23            16
//	+--------------+----------------+
//	| Row Addr Hld | 1=14,0=12 rows |
//	+--------------+----------------+
//
//	 15           8 7              0
//	+--------------+----------------+
//	| SDRAM 4 bank | DIMM Sub Banks |
//	+--------------+----------------+
//	
//	DIMM matching Data structures (r18, r19)	
// 
//
//	 63          56 55      48 
//	+--------------+----------------+
//	| 4 bank SDRAM | Dual Mod bank  |
//	+--------------+----------------+
//
//	 47          40 39            32
//	+--------------+----------------+
//	|              |  Column Count  |
//	+--------------+----------------+
//
//	 31          24 23            16
//	+--------------+----------------+
//	|              |                |
//	+--------------+----------------+
//
//	 15           8 7              0
//	+--------------+----------------+
//	|              |  Row Count     |
//	+--------------+----------------+
//	
//
//
//	
//
// CALLS:
//
// REGISTERS:
//
// MACROS:
//
// CONSTANTS:
//
//
// ALGORITHM:
//
//------------------------------------------------------------------------ 
// PYXIS registers
//   7
// MCR[0]     -  MODE_REQ send mode data to DIMMs
// MCR[29:16] -  DRAM_MODE mode data.  Set to ones when config complete.
// 		[18:16] Burst Length
// 			value  WT=0  WT=1
// 			000     1     1
// 			001     2     2
// 			010     4     4
// 			011     8     8
// 			100     -     -
// 			101     -     -
// 			110     -     -
// 			111    page   -
//
//		[19]    Wrap Type
//			0 = Sequential
//			1 = Interleave
//
//		[22:20] CAS Latency Mode
// 			value  Latency
// 			000     -
// 			001     1
// 			010     2
// 			011     3
// 			100     -
// 			101     -
// 			110     -
// 			111     -
//
//		[23:22] Test Mode
//			00	Mode set
//			01	-
//			10	-
//			11	-
//
//		[24]	Write Burst
//			0 = Burst mode
//			1 = Single bit mode
//
// GTR - global timing register
// 		[2:0]	MIN_RAS_PRECHARGE
// 		[5:4]	CAS Latency (must be 2 or 3)
// 		[10:8]  Idle BCache Width
// 
// RTR - refresh timing register
//		[6:4]   Refresh RAS assertion width 
//			(corresponds to DRAM tRAS)  prog_val = (desired-3)
//		[12:7]	Refresh interval            prog_val is multiplied by 64
//		[15]	Force refresh
// RHPR - Row History Policy Mask (set to 0xE880)
//
// BBAR0-BBAR7 - bank base addr reg
//		[15:6] baseaddr<33:24>
//
// BCR0-BCR7 - Bank Config Reg
//		[0]   bank_enable if set
//		[4:1] Bank Size
//		   1000   2GB  ** PYXIS is capable of these configs
//		   0000   1GB  ** but is not tested or documented.
//		   0001 512MB
//		   0010 256MB
//		   0011 128MB
//		   0100  64MB
//		   0101  32MB
//		   0110  16MB
//		   0111   8MB 
//		[5]   Sub Bank Enable
//		[6]   ROW sel 0=12, 1=14
//		[7]   4BANK
//
// BTR0-BTR7 -   Bank Timing Reg
//		[2:0] Row Addr Hold
//		[4]   Toshiba
//		[5]   CAS_Slow_prech
//
//
// 0  #bytes used in EEPROM	1-255
// 1  Total # bytes in EEPROM	2**x -> 6=64, 7=128, 8=256
// 2  Memory type on DIMM 	4=SDRAM, other=reserved
// 3  #Row addresses		1-127,128 undef, bit7 set=redundant addressing
// 4  #column addresses		1-255
// 5  #banks on module (see 17)	1-255
// 6  Data width of DIMM LSB	typically 64, 72, 80
// 7  Data width of DIMM MSB	
// 8  Voltage interface levels	0=5Vttl,1=LVttl,2=HSTL1.5,3=SSTL3.3,4=SSTL2.5
// 9  SDRAM Cycle time		[7:4]=nS, [3:0]=1/10nS. range 1.0-15.9nS
// 10 SDRAM access from clock	[7:4]=nS, [3:0]=1/10nS. range 1.0-15.9nS
// 11 DIMM config type		ECC=2, Parity=1, none=0
// 12 Refresh Rate & Type	bit7 = self refresh
//				0 =  15.625 uS (normal)
//				1 =   3.9   uS (.25 x reduced)
//				2 =   7.8   uS (.25 x reduced)
//				3 =  31.3   uS ( 2x extended)
//				4 =  62.5   uS ( 4x extended)
//				5 = 125.0   us ( 8x extended)
// 13 Primary SDRAM dev width	1-255 bits
// 14 Err chk'g SDRAM dev width	1-255 bits
// 15 Min CLK delay		???
// 16 Burst length support	More than one bit can be set
//		     +------+---+------+------+-------+------+------+------+
//		     |    7 | 6 |  5   |  4   |   3   |   2  |   1  |   0  |
//		     +------+---+------+------+-------+------+------+------+
//		     |burst |tbd|tbd   |tbd   |burst  |burst |burst |burst |
//		     |len=  |   |      |      |len=8  |len=4 |len=2 |len=1 |
//		     |page  |   |      |      |       |      |      |      | 
//		     +------+---+------+------+-------+------+------+------+
// 17 #banks per SDRAM device	1-255
// 18 CAS latency		Supported CAS latencies
//				+---+----+----+----+----+----+----+----+
//				| 7 | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
//				+---+----+----+----+----+----+----+----+
//				|tbd|CAS |CAS |CAS |CAS |CAS |CAS |CS  |
//				|   |lat |lat |lat |lat |lat |lat |lat |
//				|   | 7  | 6  | 5  | 4  | 3  | 2  | 1  |
//				+---+----+----+----+----+----+----+----+
// 19 CS latency			Supported CS latencies
//				+---+----+----+----+----+----+----+----+
//				| 7 | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
//				+---+----+----+----+----+----+----+----+
//				|tbd|CS  |CS  |CS  |CS  |CS  |CS  |CS  |
//				|   |lat |lat |lat |lat |lat |lat |lat |
//				|   | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
//				+---+----+----+----+----+----+----+----+
// 20 Write latency		Supported Write Enable Latencies
//				+---+----+----+----+----+----+----+----+
//				| 7 | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
//				+---+----+----+----+----+----+----+----+
//				|tbd|WE  |WE  |WE  |WE  |WE  |WE  |WE  |
//				|   |lat |lat |lat |lat |lat |lat |lat |
//				|   | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
//				+---+----+----+----+----+----+----+----+
//
// 21 SDRAM module attributes	
//			+---+---+------+------+-------+------+------+------+
//			| 7 | 6 |  5   |  4   |   3   |   2  |   1  |   0  |
//			+---+---+------+------+-------+------+------+------+
//			|tbd|tbd|diff  |reg'd |buff'd |On    |reg'd |reg'd |
//			|   |   |clk   |DQMB  |DQMB   |brd   |Addr  |Addr  |
//			|   |   |input |Inputs|Inputs |PLL   |& Ctl |& Ctl |
//			|   |   |      |      |       |      |inputs|inputs|
//			+---+---+------+------+-------+------+------+------+
// 22 SDRAM module attributes	
//			+---+---+------+------+-------+------+------+------+
//			| 7 | 6 |  5   |  4   |   3   |   2  |   1  |   0  |
//			+---+---+------+------+-------+------+------+------+
//			|tbd|tbd|tbd   |tbd   |support|suppt |suppt |Suppt |
//			|   |   |      |      |wr1/rd |prechg|auto  |early |
//			|   |   |      |      |burst  |all   |prech |RAS   | 
//			|   |   |      |      |       |      |      |prech | 
//			+---+---+------+------+-------+------+------+------+
//
// 23-31 reserved
// 32-63 reserved
// 
// 64-71	 Mfg JEDEC ID Code	00-N/A, FF-continue
// 72	 Mfg location code	Mfg specific
// 73-90  Mfg part number	6 bit packed ASCII
// 91-92  Mfg Revision		6 bit packed ASCII
// 93-94  Mfg date		Year + Week of Mfg
// 95-98  Mfg Serial Number
// 99-126 Mfg specific data
// 127    Chksum of 0-63
// 128+ unused
//
// 
// The following tests are made against each DIMM before it is
// sized.  These checks insure electrical compatibility with
// PC164LX & PYXIS, considering the huge range on many of the
// DIMM characteristics.  
// 
// Any DIMM which does not pass these initial checks will cause 
// a fatal error with beep codes, as the whole memory subsystem 
// will become unusable.
//
// In addition, there are several DIMM characteristics which
// affect memory bus global timing.  Installing a slow DIMM
// or one which heavily loads the addr or data bus will slow
// the operation of the whole memory system.
//
//serial  electrical
//rom     compatibility
//addr    test
//-------------------------
//  2     4
//  3     12<=val<=14
//  4     <=15
//  5     1|2
//  6     64|72  ** 72 only for PC164LX I
//  7     0
//  8     1
//  9     12.0
// 10     9.0
// 12     0|3,w/bit 7=don't care (self refr)
// 15     1
// 16     (val&4)=1
// 17     2|4
// 18     3
// 19     1
// 20     1
// 21     (val&~0xd)=0
// 22     (bit1=1)&(bit2=1)
//
// Here's one big pain in the butt from JEDEC:
// There is no one PD byte to determine DIMM size, it's in four places.
// total_DIMM_size = rows + columns + module_bank(1|0) + SDRAM_Banks(0|1)
// ...so a DIMM 12x9 DIMM could be 16MB, 32MB or 64MB
//
// bad_dimms=0;  ecc_present=1; worst_speed=0; size_bitmask=0; col=0; row=0;
// match_row=0; match_column=0;
// 
// for dimm_slot = 0 to 3
// :  
// : for pd_byte = 2 to 22
// : : 
// : : data = srom_iic_read_rom(dimm_slot, pd_byte)
// : : err = 0//
// : : 
// : : switch (pd_byte)
// : : : case 2: if (data==4) cont; else err=NOT_SDRAM; break;
// : : : case 3: if (data==11)data++  //ambiguity in JEDEC spec
// : : :         if (data==13)data++  //mfg is probably counting row 0.
// : : :         if (data==12) row=data else
// : : :         if (data==14) row=data feature_mask|=[1<<(16+dimm_slot)]
// : : :            else err=UNSUPT_ROW_CNT; break;
// : : :         //support up to 1GB DIMM (row+col)==27 (2^27*8) 
// : : :         //module banks and device banks each can multiply this by 2.
// : : : case 4: if (row+data<=27) col=data; cont; else err=TOO_LARGE; break;
// : : : case 5: if (data==1) cont else
// : : :         if (data==2) cont else err=TOO_MANY_BANKS;
// : : :         //if dual bank - make sure DIMM size<=1GB (2^26*8*2)
// : : :         if (data==2) && ((row+col)<=26) feature_mask|=(1<<dimm_slot) 
// : : :         else err=TOO_LARGE;
// : : : case 6: if (data==72) cont; else 
// : : :         if (data==64) ecc_present=0; cont; else err=UNSUPT_WIDTH; break;
// : : : case 7: if (data==0)cont; else err=UNSUPT_WIDTH; break;
// : : : case 8: if (data==1)cont; else err=UNSUPT_INTFC_VOLTAGE; break;
// : : : case 9: if (data<=0xC0)
// : : :            worst_speed=max(worst_speed,data)
// : : :            cont; else err=UNSUPT_SPEED; break;
// : : : case 10: if (data==0x90)cont; else err=UNSUPT_SPEED; break;
// : : : case 11: cont;
// : : : case 12: data&=0xEF; if (data==0)||(data==3)cont; else err=UNSUPT_REFRESH; break;
// : : : case 13: cont;
// : : : case 14: cont;
// : : : case 15: if (data==1)cont; else err=UNSUPT_CLKDLY; break;
// : : : case 16: if ((data&4)==1)cont; else err=UNSUPT_BURSTLEN; break;
// : : : case 17: if (data==2) cont else 
// : : :          if (data==4) feature_mask|=[1<<(8+dimm_slot)] 
// : : :              else err=UNSUPT_DEV_BANKS; break;
// : : :          if (row+col+[feature_mask>>dimm_slot]&1+data>27) err=too_large; break;
// : : : case 18: if (data==3)cont; else err=UNSUPT_CAS_LAT; break;
// : : : case 19: if (data==1)cont; else err=UNSUPT_CS_LAT; break;
// : : : case 20: if (data==1)cont// else err=UNSUPT_WR_LAT; break;
// : : : case 21: if ((data&0x70)!=0)cont// else err=UNSUPT_DEV_CHAR; break;
// : : : case 22: data&=6; if (data==6)cont; else err=UNSUPT_DEV_CHAR; break;
// : : end
// : : 
// : : if (err!=0)
// : : : output proper DIMM error message from table in ROM
// : : : Throw out DIMM pair from sizing algorithm
// : : end
// : :
// : end
// : 
// : //check for DIMM pair mismatch on odd slot numbers
// : if ((dimm_slot&1)==1)
// : : if (match_col==col) && (match_row==row)
// : : : //everything checks for DIMM so populate bitmap
// : : : //each dimm is 8 bytes wide
// : : : dimm_size=1<<(col+row+3+sub_bank+SDRAM_4bank)
// : : : mputs("DIMM")   mputs(" siz")  mputs("e:0x")
// : : : putl dimm_size  mputs("MB  ")
// : : : //2^27 is largest DIMM supported, set bit corresponding to 
// : : : //size of dimm and position of slot. 2^27*8=1GB
// : : : size_mask |= 1<<[((27-row-col)<<3)+(dimm_slot>>1)]
// : : else
// : : : mputs("Error! DIMM Size mismatch slots #")
// : : : putl(dimm_slot-1)  mputs(" and #") putl(dimm_slot)
// : : : feature_mask |= 1<<((slot>>1)+56) //put a mismatch flag in
// : : : maybe beep(1,1,3)
// : : end
// : else 
// : : match_row=row
// : : match_col=col
// : end
// : 
// end
// if (bad_dimms>0)
// : beep (1,3,3)
// : hang tight
// end 
// 
// registers:
// 
// r0-r4 = scratch for calculations
// r5  = pass dimm slot into IIC read routine, receive IIC data
// r6  = Current DIMM slot
// r7  = current presence detect byte offset
// r10 = good memory bit mask
// r11 = worst speed [63:32]  & ECC present [31:0]
// r12 = DIMM feature bit mask
// r17 = DIMM error flag
// r18 = DIMM current data - See above decription
// r19 = DIMM old/match data - See above decription
//
//--


srom_size_memory:

	USDELAY	(100000)		//wait 100mS for DIMMs to settle

	bis	r31, r31, r10		// zero the good memory bitmap
	bis	r31, r31, r11		// zero the speed / ECC data
	bis	r31, r31, r12		// zero the DIMM feature mask

	bis	r31, r31, r6		// dimm_slot
	bis	r31, r31, r17		// clear dimm_error flag
	bis	r31, r31, r18		// See above decription
	bis	r31, r31, r19		// See above decription
	lda	r11, 1(r31)			// upper lw=worst_speed, lower lw=ecc_present
next_slot:

pd2:						//SDRAM check
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 2(r31)			// pd_byte 
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	cmpeq	r5, 0x4, r0		// SDRAM DIMM?
	bne	r0, pd3				// continue, else
	lda	r1, 0x7fff(r31)		// iic bus error code
	cmpeq	r5, r1, r0		// bus error???
	beq	r0, pd2_a			// : NO - then not SDRAM
	lda	r7, 14(r31)			// : YES - report empty DIMM slot
pd2_a:
	br	r31, dimm_error		// :

pd3:						//check SDRAM row count
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 3(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	blbc	r5, pd3_a		// row count must be adjusted if odd.
	addl	r5, 1, r5		// 11->12, 13->14
pd3_a:
	cmpeq	r5, 12, r0		// row==12?
	beq	r0, pd3_b				// 
	br	r31, pd3_d			// save row count
pd3_b:	cmpeq	r5, 14, r0	// row==14?
	bne	r0, pd3_c			//
	br	r31, dimm_error		// unsupported row count
pd3_c:	lda	r0, 1(r31)		// there are 14 rows - need to set bit in mask
	srl	r6, 1, r1			// bank = DIMM_SLOT >> 1
	addl	r1, 16, r1		// bit pos of mask is @(16+bank)
	sll	r0, r1, r0			// 1<<(16+bank)
	bis	r0, r12, r12		// save_row |= 1 << (16+bank)
pd3_d:
	zap	r5, 0xf0, r5		// mask off upper lw
	zap	r18, 0x0f, r18		// mask off lower lw
	bis	r5, r18, r18		// store row count
	
pd4:						// check SDRAM column count
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 4(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	zap	r18, 0xf0, r1		// get row count
	addl	r5, r1, r0		// total number of address bits to dimm
	cmple	r0, 27, r1		// address_bit_tot<=27 (1GB dimm!!)
	bne	r1, pd4_a			//
	br	r31, dimm_error		// >1GB DIMM!!
pd4_a:
	zap	r5, 0xf0, r5		// mask off upper lw
	zap	r18, 0xf0, r18		// mask off upper lw
	sll	r5, 32, r5			// put in upper lw
	bis	r5, r18, r18		// store col count
	
pd5:						// check assembly's bank count
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 5(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	cmpeq	r5, 1, r0		// banks==1 || bank==2
	beq	r0, pd5_a			//
	br	r31, pd6			// continue checking
pd5_a:
	cmpeq	r5, 2, r0		// 
	beq	r0, pd5_d			//
pd5_b:
	srl	r18, 32, r0			// get col count
	zap	r0, 0xfc, r0		// mask off 4bank/subbank
	zap	r18, 0xf0, r1		// get row count
	addl	r0, r1, r1		// (row+col)
	cmple	r1, 26, r0		// (row+col)<=26?
	beq	r0, pd5_c			//
	srl	r6, 1, r1			// bank = dimm_slot >> 1
	lda	r0, 1(r31)			// there sub banks - need to set bit in mask
	sll	r0, r1, r0			// 1<<bank
	bis	r0, r12, r12		// feature_mask |= 1 << bank
    lda	r0, 1(r31)			// there sub banks - need to set bit in match
	sll	r0, 48, r0			// 1<<48
	bis	r0, r18, r18		// match_data |= (1 << 48)
	br	r31, pd6			// continue checking
pd5_c:
	lda	r7, 4(r31)			// too large error code
	br	r31, dimm_error		// error - > DIMM too large
pd5_d:
	br	r31, dimm_error		// error - > Unsuppt # of banks

pd6:						// check dimm width LSB
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 6(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	cmpeq	r5, 64, r0		// no parity?
	beq	r0, pd6_a			//
	bic	r11, 1, r11			// ECC not present in all dimms
	br	r31, dimm_error		// jjd - PC164LX I needs ECC DIMMS
//////jjd	br	r31, pd7		// jjd - maybe not a PC164LX II problem
pd6_a:
	cmpeq	r5, 72, r0		// ecc?
	bne	r0, pd7				//
	br	r31, dimm_error		// incompatible dimm width
	
pd7:						// check dimm width MSB
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 7(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	beq	r5, pd8				// must be 0
	br	r31, dimm_error		// incompatible dimm width
	
pd8:						// check interface voltage	
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 8(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	cmpeq	r5, 1, r0		// interface == LVTTL
	bne	r0, pd9				//
	br	r31, dimm_error		// unsup dimm interface voltage
	
pd9:						// check dimm speed
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 9(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	cmple	r5, 0xc0, r0	// speed <= 12.0nS
	bne	r0, pd9_a			// 
	br	r31, dimm_error		// incompatible speed
pd9_a:
	srl	r11, 32, r1			// get current worst speed
	cmple	r5, r1, r0		// dimm_speed<=worst_speed?
	bne 	r0, pd10		//
	sll	r5, 32, r5			// this dimm is currently the slowest
	zap	r11, 0xf0, r11		// clear old speed data
	bis	r5, r11, r11		// merge into upper lw

pd10:						// check access from clock speed
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 10(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	cmple	r5, 0x90, r0	// access <= 9.0nS
	bne	r0, pd12			//
	br	r31, dimm_error		// unsup access from clock speed

pd12:						// check refresh type
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 12(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	and	r5, 0x7f, r5		// self refresh bit is a 'don't care'
	beq	r5, pd15			// refresh==0||refresh==3 (norm||2x extended)
	cmpeq	r5, 3, r0		// 
	bne	r0, pd15			//
	br	r31, dimm_error		// unsupprted refresh rate

pd15:						// check clock delay
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 15(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	cmpeq	r5, 1, r0		// clk_dly==1?
	bne	r0, pd16			//
	br	r31, dimm_error		// unsupported clock delay

pd16:						// check burst length
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 16(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	and	r5, 4, r5			// need at least burst len=4, others optional
	bne	r5, pd17			//
	br	r31, dimm_error		// unsupported burst length

pd17:						//check #SDRAM device banks
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 17(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	cmpeq	r5, 2, r0		// banks==2||banks==4 only
	bne	r0, pd18			//
	cmpeq	r5, 4, r0		// jjd - devices are not supported
	beq	r0, pd17_a			// jjd - this will be fixed later....
	lda	r0, 1(r31)			// SDRAM is 4 banks - need to set bit in mask
	srl	r6, 1, r1			// bank = dimm_slot >> 1
	addl	r1, 8, r1		// (bank+8)
	sll	r0, r1, r0			// 1<<(bank+8)
	bis	r0, r12, r12		// feature_mask |= [1 << (bank+8)]
    lda	r0, 2(r31)			// there 4 banks - need to set bit in match
	sll	r0, 56, r0			// 1<<56
	bis	r0, r18, r18		// match_data |= (1 << 56)
	lda	r0, 1(r31)		// there are 14 rows if 4bank set
	srl	r6, 1, r1			// bank = DIMM_SLOT >> 1
	addl	r1, 16, r1		// bit pos of mask is @(16+bank)
	sll	r0, r1, r0			// 1<<(16+bank)
	bis	r0, r12, r12		// save_row |= 1 << (16+bank)
	br	r31, pd18			// continue checking
pd17_a:
	br	r31, dimm_error		// unsup # SDRAM device banks
	lda	r7, 4(r31)			// too large error code
	br	r31, dimm_error		// error - > DIMM too large


pd18:						// check CAS latency	
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 18(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	and	r5, 4, r0			// must at least support CAS lat == 3 cycles.
	bne	r0, pd19			//
	br	r31, dimm_error		// unsupported CAS latency

pd19:						// check CS latency
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 19(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	cmpeq	r5, 1, r0		// CS_latency==1 (latency of 0)
	bne	r0, pd20			//
	br	r31, dimm_error		//unsupported CS latency
	
pd20:						// check write latency
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 20(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	cmpeq	r5, 1, r0		// WR_latency==1 (latency of 0)
//	bne	r0, pd21			//
	bne	r0, pd_end			// This fails on Samsung part?
	br	r31, dimm_error		//unsupported write latency
		
pd21:						// check 1st general dimm characteristics
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 21(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	and	r5, 0x70, r0		// We DON'T want these bits, others are OK
	beq	r0, pd22			//
	br	r31, dimm_error		//unsup general dimm char
	
pd22:						// check 2nd general dimm characteristics
	bis	r6, r6, r5			// pass dimm_slot
	lda	r7, 22(r31)			// pd_byte
	bsr	r28, srom_iic_read_rom	// byte returned in r5
	and	r5, 6, r5			//only care about bits [2:1]
	cmpeq	r5, 6, r0		//both must be set
	bne	r0, pd_end			//
	br	r31, dimm_error		//unsup general dimm char
	
pd_end:						//end DIMM checking
	blbc	r6, match_save	//check for dimm pair matching on odd slots
							// for now - just check size and row/col
							// organization.  May do more later.
	cmpeq	r18, r19, r0	//r18=current row/col org, r19=last row/col org
	beq	r0, dimm_error		//
match_good:
	srl	r18, 32, r0			//extract dimm row count
	zap	r0, 0xfc, r0		//mask off 4bank/subbank
	srl	r18, 48, r2			//extract dimm subbank
	zap	r2, 0xfe, r2		//mask off 4bank
	srl	r18, 56, r19			//extract dimm 4bank
	zap	r19, 0xfe, r19		//mask off remainder
	zap	r18, 0xf0, r1		//extract dimm col count
	lda	r3, 27(r31)			//2^27*8 (1GB!) is largest dimm supported
	subl	r3, r0, r3		//(27-row)
	subl	r3, r1, r3		//(27-row-col)
	subl	r3, r2, r3		//(27-row-col-subbank)
	subl	r3, r19, r3		//(27-row-col-subbank-4bank)
	sll	r3, 3, r3			//((27-row-col-subbank-4bank)<<3)
	srl	r6, 1, r0			//bank = (dimm_slot>>1)
	addl	r0, r3, r3		//((27-row-col-subbank-4bank)<<3)+bank)
	lda	r0, 1(r31)			//
	sll	r0, r3, r0			//1<<[((27-row-col-subbank-4bank)<<3)+bank]
	bis	r0, r10, r10		//size_mask|=1<<[((27-row-col-subbank-4bank)<<3)+bank]
	br	r31, match_end		//

dimm_error:					//record dimm mismatch
	srl	r6, 1, r0			//(dimm_slot>>1)
	addl	r0, 1, r6		//r6=((dimm_slot>>1)+1)<<1
	sll	r6, 1, r6			//ie- skip to next DIMM pair (0,2,4)
	addl	r0, 56, r0		//((dimm_slot>>1)+56)
	lda	r1, 1(r31)			//
	sll	r1, r0, r1			//1<<((dimm_slot>>1)+56)
	bis	r1, r12, r12		//feature_mask|=1<<((dimm_slot>>1)+56)
	br	r31, match_end1		//
match_save:					//get ready for next match
	bis	r18, r18, r19		//match_row=row, match_col=col
match_end:
	addl	r6, 1, r6		//do next dimm_slot
match_end1:
	cmplt	r6, 4, r0		//dimm_slot<4?
	beq	r0, end_sizing		//
	br	r31, next_slot		//
end_sizing:					//
	bne	r10, FoundDIMMS		// Check for DIMMS present
	 
	lda	r8, LED_F_NOSIMMS(r31)	// Ledcode value to print.
	br	r31, FatalError			// No DIMMS found error
FoundDIMMS:


//++
// ===========================================================================
// = srom_configure_memory - Configure the memory subsystem             =
// ===========================================================================
//
// OVERVIEW:
//
//   This function will configure memory so that the large memory banks will 
//   be in low memory and the smaller memory banks in high memory.
//
// FORM OF CALL:
//
//	bsr	r27,srom_configure_memory
//
// ARGUMENTS:
//
//	r15 - has the GRU base pointer
//	r10 - has memory size bitmask
//	r11 - has speed of slowest DIMM (upper LW), ECC preset if lower LW is 1
//
// RETURNS:
//
//	r12 - gets Total amount of memory in the system
//
// FUNCTIONAL DESCRIPTION:
//
//	This code will configure the memory using the global memory size bitmap
//	in r10 and built by the srom_size_memory() routine.
//
// CALLS:
//
//
// CONSTANTS
//
//
// REGISTERS:
//
//   r1 - used to store the bitmask
//   r2 - used to store base address
//   r3 - used to store the size of the SIMM 
//   r4 - holds the size bitmap temp 
//   r5 - size of memory in the bank 
//   r6 - used to loop through the memory types
//   r7 - used to loop through all 16 bits for each memory type
//   r10 - memory bitmap (see header of srom_size_memory for details)
//   r12	- global memory size 
//   r16 - used to store the set (MBA) number
//
// ALGORITHM:
//
// GTR = 0x34         //min_ras_prech=2, CAS lat=3
// RTR = 0x820        //refresh_width=2, Refresh_interval=16
//                    //assumes 64mS /4096 refresh/15nS sysclk/64
// RTR |= 0x1000      //force refresh
// stall 4uS
// MCR = 0x003A.0400  //dram_mode=[burst_len=4,wrap=interleave,CAS LAT=3]
//                    //bcache on (present or not always tell PYXIS it's there)
// MCR |= 1           //force SDRAM mode out to SDRAM DIMMs
// RTR |= 0x1000      //force refresh
// stall 4uS
// 
// 
// base_addr = 0
// dimm_type = 0
// bank_size = 0      //bank_size in MB
// next_base = 0      //base bank addr for next bank to be config'd
// total_memory=0
//
// This sizing loop ends up doing a lot of iterations because it is basically
// a sorting routine. It's goal is to configure the banks from largest
// to smallest.  The sort is done by sub_bank flag by dimm_type by bank.
// DIMMs which are too small or large have already been rejected.
//  
// 	dimm	  bank	w/sub	PYXIS	w/sub	size	              w/sub
// dimm_type  size	 size	banks	mask	banks	in hex	      banks
// --------- -----  -----   -----   -----   -----   ---------    ---------
//		0	  1GB	  2GB	(n/a)	1000	(n/a)	8000.0000    (n/a)
//  	1	512MB	  1GB	  2GB	0000	1000	4000.0000    8000.0000
//  	2	256MB	512MB	  1GB	0001	0000	2000.0000    4000.0000
//  	3	128MB	256MB	512MB	0010	0001	1000.0000    2000.0000
//  	4	 64MB	128MB	256MB	0011	0010	0800.0000    1000.0000
//  	5	 32MB	 64MB	128MB	0100	0011	0400.0000    0800.0000
//  	6	 16MB	 32MB	 64MB	0101	0100	0200.0000    0400.0000
//  	7	  8MB	 16MB	 32MB	0110	0101	0100.0000    0200.0000
// 
// 
//
// for dimm_type = 0 to 7   //start at low size first
// : for sub_bank = 0 to 1   //look for dimms w/subbank 1st - they're 2x bigger
// : : for bank = 0 to 2      //can be extended to eight for a system with 8 banks
// : : :
// : : : if [(feature_mask & (1<<(56+bank)) == 1]
// : : : : //there is a DIMM mismatch on this bank - disable bank
// : : : : BBAR[bank] = 0  //zero bank address
// : : : : BCR[bank] = 0   //zero/disable bank config
// : : : : BTR[bank] = 0   //zero bank timing
// : : : else
// : : : : //Bank is OK - search from dimm_size = largest to smallest
// : : : : //first checking DIMMS with a sub-bank (they are actually 2*dimm_size)
// : : : : if [(size_mask & (1<<(dimm_type+bank))) == 1] &&
// : : : :    [(feature_mask & (1<<bank)) == sub_bank)]
// : : : : //have a dimm of 'dimm_type' in 'bank' w/ sub banks. bank_size in MB
// : : : : //+1 because there are 2 DIMMs/bank,+1 if sub_bank flag set
// : : : : bank_size = 8 << (7-dimm_type+1+sub_bank) 
// : : : : total_memory += bank_size
// : : : : if (dimm_type==0) bank_config=0x1000 else bank_config=(dimm_type-1)
// : : : : BBAR[bank] = next_base
// : : : : next_base = (bank_size>>4)<<5
// : : : : BTR[bank] = 2 //row_addr_hold=2, other data dimm mfg dependent->N/A
// : : : : BCR[bank] = [(feature_mask>>bank)&1]<<5     |  //insert sub bank flag
// : : : :             [(feature_mask>>(16+bank)&1]<<6 |  //insert row flag
// : : : :             [(feature_mask>>(8+bank)&1]<<7  |  //insert 4bank flag
// : : : :             bank_config<<1                  |  //insert bank config
// : : : :             1                                  //enable bank!!
// : : : end
// : : end bank 
// : end sub_bank
// end dimm_type
// 
// return total_size
//
//--
srom_configure_memory:
/*
 * Initialize the Clock Control Register
 */
#define PYXIS_CCR_K_INIT ((1 << MC_CCR_V_CLK_DIVIDE)	| \
			(3 << MC_CCR_V_PCLK_DIVIDE) 	| \
			(2 << MC_CCR_V_PLL_RANGE) 		| \
			(1 << MC_CCR_V_LONG_RESET) 		| \
			(0 << MC_CCR_V_CONFIG_SRC) 		| \
			(0 << MC_CCR_V_DCLK_INV) 		| \
			(0 << MC_CCR_V_DCLK_FORCE) 		| \
			(1 << MC_CCR_V_DCLK_PCSEL) 		| \
			(0x29 << MC_CCR_V_DCLK_DELAY)		)

	load_csr_base(r14)		/* Base for config registers.	*/
	ldah	r8, 0x4000(r14)	/* Base of the Miscellaneous Registers */
	lda	r16, MC_CCR(r8)		/* r16 <- pointer to CCR reg */
	LDLI	(r17, PYXIS_CCR_K_INIT)
	bsr	r26, srom_wrtest		/* write and test CCR */

	lda	r16, 20000(r31) 	/* load cycles into R16.	*/
	bsr	r27, wait_n_cycles 	/* Count for this many CPU cycles*/
					/* R0 <- number of cycles waited.*/
	//
	//
	// init the GTR 
#if 0
gtr_m_lo = < -
	<2 @ gtr_v_min_ras_precharge> !-// See SDRAM specs
	<3 @ gtr_v_cas_latency> !-	// must be 2 or 3
	<6 @ gtr_v_idle_bc_width> !-	// part of cache config code
	<0>>
#endif
	LDLI	(r17, 0x332)		/* r17 <- GTR bits		*/
	ldah    r8, 0x1000(r14)		/* r8 <- PYXIS mem csr base ptr	*/
	lda	r16, MC_GTR(r8) 		/* r16 <- GTR ptr		*/
	bsr	r26, srom_wrtest		/* write and test it	*/

	// now the rtr base
#if 0
rtr_m_lo = < -
	<5 @ rtr_v_refresh_width > !-	// See SDRAM specs
	<0xe @ rtr_v_ref_interval>  !-	// 64mS/4096/15nS/64=16
	<0 @ rtr_v_force_ref> !-		// can't force refresh while changing val
	<0>>
#endif
	LDLI	(r17, 0x750)		/* r17 <- RTR bits		*/
	lda	r16, MC_RTR(r8) 		/* r16 <- RTR ptr		*/
	bsr	r26, srom_wrtest		/* write and test it	*/

/*
 *	Force Refresh
 */
	bis	r31, 1, r4				/* create the force refresh bit */
	sll	r4, MC_RTR_V_FORCE_REF, r4
	bis	r17, r4, r17
/*
 *	Lets loop for 8 times
 */
	lda	r5, 8(r31)				/* Load loop counter */
ForceRef:
	lda		r16, MC_RTR(r8)		/* force refresh */
	bsr		r26, srom_wrtest

	lda	r4, 100(r31)
ForceRefWait:
	STALL
	subq	r4, 1, r4
	bne		r4, ForceRefWait

	subq	r5, 1, r5
	bne		r5, ForceRef

	bis		r17, r31, r5		/* save r17 -> r5 */
/*
 *	Setup the memory controler section based on the hardcoded values.
 *	The MCR should only set the BCACHE_ENABLE bit if the Bcache is actually
 *	turned on.  Note: It is set now, and because with the hard coded memory size
 *	and the IIC memory sizing the bcache will always be on.  This will have
 *	to change if we run with caches off.
 */

	ldah    r16, 0x1000(r14)	/* r16 <- MCR ptr		*/
	LDLI	(r17, 0x3A1401)		/* r17 <- MCR bits		*/
	bsr	r26, srom_wrtest		/* write and test it		*/

/*
 *	Lets loop for 8 times
 */
	bis		r5, r31, r17		/* restore r17 */
	lda	r5, 8(r31)				/* Load loop counter */
ForceRef1:
	lda		r16, MC_RTR(r8)		/* force refresh */
	bsr		r26, srom_wrtest

	lda	r4, 100(r31)
ForceRefWait1:
	STALL
	subq	r4, 1, r4
	bne		r4, ForceRefWait1

	subq	r5, 1, r5
	bne		r5, ForceRef1

	//
	// Configure sized memory
	//

	bis	r31, r31, r2		// bank
	bis	r31, r31, r3		// dimm_type
	bis	r31, r31, r4		// next_base address
	bis	r31, r31, r5		// <free>
	bis	r31, r31, r6		// total_memory size in MB
	lda	r7,	 MC_BBA0(r8)	// addr of bbar0

check_dimm_type:
	lda	r0, 1(r31)			// get a '1'
	sll	r3, 3, r1			// (dimm_type<<3)
	addq	r1, r2, r1		// (bank+(dimm_type<<3))
	sll	r0, r1, r0			// [1<<(bank+(dimm_type<<3))]
	and 	r0, r10, r0		// size_mask & [1<<(bank+(dimm_type<<3))]
	beq	r0, continue_sniff	//

	//found a dimm of dimm_type in bank - configure dimm
	lda	r0, 8(r31)			//
	subq	r0, r3, r1		// (7-dimm_type+1)
	sll	r0, r1, r1			// bank_size = 8<<(7-dimm_type+1)
	addq	r6, r1, r6		// total_memory += bank_size
	bis	r4, r4, r17
	lda	r16, 0(r7)
	bsr	r26, srom_wrtest		/* write and test it	*/
	sll	r1, 2, r1			// (bank_size>>4)<<6
	addq	r4, r1, r4		// next_base += (bank_size>>4)<<6
	//
	//
	lda		r17, 0x22(r31)
	lda	r16, 0x400(r7)	// BTR[bank] = <timing data>
	bsr	r26, srom_wrtest		/* write and test it	*/
	// determine bank_config field
	subq	r3, 1, r1		// bank_config=dimm_type-1
	lda		r0, 0x1000(r31)		// temp=0x1000
	cmoveq	r3, r0, r1		// if (dimm_type==0) bank_config=temp
	sll		r1, 1, r1		// bank_config<<1
	// insert sub bank flag into bcr bits
	srl		r12, r2, r0		// (feature_mask>>bank)
	and		r0, 1, r0		// (feature_mask>>bank)&1
	sll		r0, 5, r0		// [(feature_mask>>bank)&1]<<5
	bis		r0, r1, r1		// bcr |= sub_bank_flag[bank]
	// insert row flag into bcr bits
	addq	r2, 16, r0		// (bank+16)
	srl		r12, r0, r0		// feature_mask>>(16+bank)
	and		r0, 1, r0		// (feature_mask>>(16+bank))&1
	sll		r0, 6, r0		// [(feature_mask>>(16+bank))&1]<<6
	bis		r0, r1, r1		// bcr |= row_flag[bank]
	// insert 4bank flag into bcr bits
	addq	r2, 8, r0		// (bank+8)
	srl		r12, r0, r0		// feature_mask>>(8+bank)
	and		r0, 1, r0		// feature_mask>>(8+bank)&1
	sll		r0, 7, r0		// [feature_mask>>(8+bank)&1]<<7
	bis		r0, r1, r17		// bcr |= 4bank_flag[bank]
	lda	r16, 0x200(r7)		// BCR[bank] = bcr
	bsr	r26, srom_wrtest		/* write and test it	*/
	// enable bank
	bis		r17, 1, r17		// bcr |= 1
	lda	r16, 0x200(r7)	// BCR[bank] = bcr
	bsr	r26, srom_wrtest		/* write and test it	*/
	//
continue_sniff:
	addq	r2, 1, r2		// bank=bank+1
	cmplt	r2, 2, r0		// go till all banks checked
	beq		r0, banks_done	// done with banks
	lda		r7, 0x40(r7)		// point to bbar[bank]
	br	r31, check_dimm_type	// next bank
	//
banks_done:	
	bis	r31, r31, r2		// reset bank to '0'
	lda	r7,	 MC_BBA0(r8)	// addr of bbar0

	addq	r3, 1, r3		// dimm_type=dimm_type+1
	cmple	r3, 7, r0		// go till all dimm_type's checked
	bne		r0, check_dimm_type	// 

	sll	r6, 20, r23			// move total memory size into r23

#ifdef DEBUG_BOARD
	jump_to_mini_prompt()		/* trap to mini, no autobaud	*/
#endif	/* ifdef DEBUG_BOARD */

#endif

/*
 *	Now lets setup the Bcache Control to force a hit to work around
 *	a Pyxis bug in pass 1.
 */

	OutLEDPORT(0x0C)

#ifdef DEBUG_BOARD
	jump_to_mini_prompt()		/* trap to mini, no autobaud	*/
#endif	/* ifdef DEBUG_BOARD */


/*
 *     Enable DCache.
 * ======================================================================
 *
 *	On powerup the DCache is disabled.  We'll turn it on before
 *	initializing memory in order to initialize the DCache as well.
 *	Note that we don't have to worry about invalid data in the DCache
 *	upon powerup because we performed a DCache Flush earlier in the 
 *	code AND invalid data read in from memory cannot get to the dcache
 *	because it is disabled.
 *
 * 	Relevant PALcode violations:
 *		1) No MBOX instruction in 1,2,3,4.
 *		2) No outstanding fills in 0.
 */
	lda	r2, (DC_M_ENA)(r31)	/* Enable the DCACHE		*/
	mtpr	r2, dcMode		/* init dc_mode register	*/
	STALL				/* No MBOX instruction in cycles*/
	STALL				/* 1,2,3 and 4.			*/
	STALL
	STALL
	STALL

/*
 *  We don't enable all of the OK scache sets here like we used to
 *  because now we do a checksum check of the system image after it
 *  has been written to memory.  Because of the random-replacement
 *  algorithm used for the SCache, we couldn't guarantee that the
 *  the system image is actually in memory unless only 1 set is on.
 */


/*
 * 	Turn on the BCache (if present).
 * ======================================================================
 *
 *	Earlier in the code, we specifically turned off the BCache in 
 *	order to simplify the memory sizing code.  Now that we are done, 
 *	we can turn it on (if present) so that it is initialized along 
 *	with the memory.
 *
 *	When the system is powered up the Backup Cache will contain
 *	unpredictable data in the Tag RAMs.  As the Cache is swept
 *	for initialization the old blocks (called the dirty victim
 *	blocks) will be written back to main memory.  These victim
 *	writes will occur based on the tag address (Tag) which stores
 *	the upper part of the address location for the dirty blocks
 *	of memory.  Because the Tags are unpredictable the victim
 *	writes could occur to unpredictable addresses.  Therefore
 *	these writes could be attempted to non-existant memory. If
 *	that happens the transaction will generate a non-existent
 *	memory error, therefore, we must make sure that PYXIS does
 *	not report it as an error by clearing the appropriate bit
 *	in the ERR_MSK.  However, since we haven't set that bit
 *	before and it's cleared on reset, we don't need to have
 *	any instructions clear it here.
 *
 *	Since we are about to initialize memory, we want to make sure 
 *	that any parity/ecc from memory is not reported to the CPU at 
 *	this time.
 *
 */
	bis	r21, r21, r18		/* Copy BC_CNFG.		*/
	bis	r20, r20, r19		/* Copy BC_CTL			*/

#define BC_CTL_PYXIS_PASS1 (BC_M_BC_FHIT | BC_M_EI_DIS_ERR | \
							BC_M_EI_ECC_OR_PARITY | (5<<BC_V_BC_TAG_STAT))
		  
#if 1
	LDLI	(r2, BC_CTL_PYXIS_PASS1)	/* Force hit to prevent NXMs */
#else
	LDLI	(r2, BC_M_EI_DIS_ERR)		/* Disable parity/ecc reporting	*/
#endif

	bis	r19, r2, r19		 
	bsr	r28, BCacheOnOff	/* Turn on BCache (if present)	*/
	OutLEDPORT(0x0F)

#ifdef DEBUG_BOARD
	jump_to_mini_prompt()		/* trap to mini, no autobaud	*/
#endif	/* ifdef DEBUG_BOARD */


/* ======================================================================
 * 9..Initialize memory (and BCache if on).
 * ======================================================================
 *
 *	Write good parity/ecc to memory by writing all memory
 *	locations.  This is done by re-writing the full contents
 *	of memory with the same data.  Note that in order to do
 *	this, we must make sure that parity/ecc reporting has
 *	been disabled in the CPU; otherwise, we would get a machine
 *	check as soon as the first unpredictable data from memory
 *	gets to the CPU.
 *
 *	Reading before writing memory lengthens the time to initialize 
 *	data parity/ecc but it results in the preservation of memory state 
 *	for debugging purposes.  
 *
 *	The size of memory in bytes is held in r23 at this point.
 */

	bis		r31, 2, r5			/* sweep memory twice */
SweepMemory:
	bis	r23, r23, r7
SweepMemory1:
	subq	r7, 8, r7			/* decrement address		*/
	ldq_p   r2, 0x00(r7)		/* Load Quadword		*/
	stq_p   r2, 0x00(r7)		/* Store same Quadword		*/
	bgt     r7, SweepMemory1	/* Loop until done.		*/

	subq	r5, 1, r5
	bne		r5, SweepMemory

	bis	r21, r21, r18		/* Copy BC_CNFG.		*/
	bis	r20, r20, r19		/* Copy BC_CTL			*/
	bsr	r28, BCacheOnOff	/* Restore BCache (if present)	*/

	OutLEDPORT(0x13)


/*
 *	Clear Error Registers.
 * ======================================================================
 *	Clears error registers in the CPU and Memory Controller.
 */
	bsr	r28, ClearMemContErrors
	bsr	r28, ClearCPUErrors

	bis	r23, r23, r7
SweepMemory2:
	subq	r7, 8, r7			/* decrement address		*/
	ldq_p   r2, 0x00(r7)		/* Load Quadword		*/
	stq_p   r2, 0x00(r7)		/* Store same Quadword		*/
	bgt     r7, SweepMemory2	/* Loop until done.		*/

	OutLEDPORT(0x14)

#ifdef DEBUG_BOARD
	jump_to_mini_prompt()		/* trap to mini, no autobaud	*/
#endif	/* ifdef DEBUG_BOARD */

/*
 * 10...Load System Code
 * ======================================================================
 */
	bis	r30, r30, r16
	bsr	r14, LoadSystemCode
	OutLEDPORT(0x17)


/*
 *     Enable SCache Sets.
 * ======================================================================
 *
 *	Earlier in the code, we only enabled one SCache set in order to
 *	simplify the memory sizing code.  Now that we are done, we can
 *	turn all three (or the one specified by the user) back on.
 *
 * 	Relevant PALcode violations:
 *		1) An MB before and after the write.
 *		2) No concurrent IStream reference.
 */
#ifdef SC_BANKS				/* Hard-code the good sets.	*/
	LDLI	(r0, (SC_BANKS << SC_V_SET_EN))
#else
	bsr	r28, SCacheSets		/* Identify good SCache sets.	*/			 		
#endif /* ifdef SC_BANKS */
					/* Enable sets and 64-byte blocks*/
	lda	r2, (SC_M_BLK_SIZE)(r31)
	bis	r0, r2, r2
	mb
	stq_p	r2, scCtl(r13)		/* leading and trailing "MB"s   */
	mb


/*
 *	Flush the ICache.
 * ======================================================================
 *	The following code is loaded into memory after the System
 *	ROM image.  It is placed there, then executed, to flush
 *	the SROM initialization code (that's this code) from the
 *	ICache.  The SROM initialization code is loaded into the
 *	ICache such that it maps to memory beginning at address
 *	zero.  In an attempt to transfer execution to the first
 *	page in memory, execution would just continue in the SROM
 *	initialization code at that address.  Therefore, execution
 *	has to be transfered to some address that doesn't hit in
 *	the Icache where other code can then flush the Icache.
 *	The NOPs following the Icache flush allow the instructions
 *	that were fetched before the Icache was updated to be
 *	cleared from the pipeline.
 *
 *	77FF0119	mt	r31, flushIc	EV5 specific
 *	C0000001	br	r0, +4
 *	????????	.long	Destination
 *	6C008000	ldl_p	r0, 0x00(r0)
 *	47FF041F	bis	r31, r31, r31
 *	   .
 *	   . 		44 of these.
 *	   .
 *	47FF041F	bis	r31, r31, r31
 *	6BEF0000	jmp	r31, (r0)
 *
 *	At this point r8 points to first memory
 *	location following System ROM image and R25 points to the
 *	destination (the address where program execution will
 *	begin after executing the flush code.)
 *
 * 	The 44 nops come from:  3RFB prefetches (24) + IC buffer,IB,
 *	slot,issue (20)
 *
 *	At this point:
 *	R8 - Address following last byte of code loaded.
 *	R25 - Starting address of image loaded.
 *
 */
	lda	r2, 0x4000(r31)		/* Minimum addr (16KB ICache).	*/
	cmpult r8, r2, r0		/* Compare to min. addr 	*/
	cmovne r0, r2, r8		/* Set to min. addr if necessary*/

	bis	r8, 1, r0		/* Put in in Palmode.		*/
	mtpr	r0, excAddr		/* write addr to exception addr.*/
					/* NO HW_REI in cycle 0 and 1.	*/
	LDLI(r2, 0x77FF0119)		/* load up mtpr flushIc		*/
	stl_p	r2, 0(r8)		/* store it			*/

	LDLI(r2, 0xC0000001)		/* load up branch instruction.	*/
	stl_p	r2,  4(r8)		/* store it			*/
	stl_p	r25, 8(r8)		/* store Destination.		*/
	LDLI(r2, 0x6C008000)		/* load up load	 :-}		*/
	stl_p	r2, 12(r8)		/* store it			*/
	
	LDLI(r2, 0x47FF041F)		/* load up NOPs			*/
	lda	r5, 44(r31)		/* Load loop counter = 44    	*/
	lda	r8, 16(r8)		/* start at the base+4		*/
loadInst:				/* for i = 1 to instCnt 	*/
	stl_p	r2, 0(r8)		/*   store instruction	 	*/
	subq	r5, 1, r5		/*   decrement loop count 	*/
	lda	r8, 4(r8)		/*   increment pointer 		*/
	bne	r5, loadInst		/* end for			*/

	LDLI(r2, 0x6BE00000)		/* load up jmp			*/
	stl_p	r2, 0(r8)		/* store it.			*/
	mb				/* force it all out		*/

	OutLEDPORT(0x18)


	bsr	r28, ClearMemContErrors
	bsr	r28, ClearCPUErrors	/* Clear errors before starting	*/
/*  Map the chip id obtained from EISTAT into an assigned SRM chip id.	*/
	srl	r0, 24, r2		/* Extract the chip id		*/
	and	r2, 0xF, r2		/* Ignore other bits.		*/

#define CHIP_ID_TO_SRM_L 0x00000100
#define CHIP_ID_TO_SRM_H 0x00000000
	LDLQ	(r1, CHIP_ID_TO_SRM_H, CHIP_ID_TO_SRM_L) 
	extbl	r1, r2, r1		/* Get SRM id for this processor*/

EV56_Detected:
	sll	r1, 32, r1		/* Minor type field <63:32>	*/
	bis	r1, 7, r25		/* EV56: Major Type = 7		*/



/* ======================================================================
 * 12..Passes control to the next level of firmware or mini-debugger.	=
 * ======================================================================
 *	This is the last code executed by the SROM.  It will pass control
 *	to the next level of firmware (or the mini-debugger).  It adheres
 *	to the calling interface defined at the beginning of this file.
 *	R30 is assumed to contain the value of the configuration jumpers
 *	in the standard format defined elsewhere in this file.
 *
 *	At this point, the following registers should have been computed:
 *
 *	R20 - BCache Control register value.
 *	R21 - BCache Configuration.
 *	R22 - BCache Configuration for cache in OFF state.
 *	R23 - Memory size in bytes.
 *	R24 - CPU speed in picoseconds.
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
	lda	r2, 0x19(r31)
	srl	r30, JMP_V_MINIDBGR, r7	/* Get mini-debugger jumper.	*/
	cmovlbc	r7, 0x3A, r2		/* Ledcode for mini-dbg.	*/
	OutLEDPORT(r2)

	bis	r20, r20, r1		/* Copy BC_CONTROL		*/
	bis	r21, r21, r2		/* Copy BC_CONFIG		*/
	bis	r22, r22, r3		/* Get BC_CONFIG for OFF state.	*/
	LDLQ	(r15, RCS_ENCODED_REV_H, RCS_ENCODED_REV_L)
	bis	r25, r25, r16		/* Get chip ID.			*/
	bis	r23, r23, r17		/* Get Size of memory		*/
	bis	r24, r24, r18		/* Get CPU speed in ps.		*/
	LDLI	(r19, 0xDECB0001)	/* Signature and System Revision*/
	zap	r19, 0xf0, r19		/* Remove sign extension	*/
	lda	r20, 1(r31)		/* Active Processor Mask	*/
	lda	r21, 0(r31)		/* System context value		*/

	blbs	r7, no_minidbg
	jump_to_minidebugger()		/* trap to Mini-Debugger.	*/
no_minidbg:

	hw_rei				/* transfer control to pgrm in mem*/


/*
 *		    =================================
 *		    =   Board-dependent routine.    =
 *		    =================================
 */
#ifndef SC_BANKS
/* ======================================================================
 * =  SCacheSets - Determines which SCache sets are working.		=
 * ======================================================================
 *
 *  OVERVIEW:
 *
 *	This routine will determine which SCache sets to enable.   It
 *	assumes that manufacturing has blown the fuses of the bad sets
 *	and that they can be read thru the SC_CTL register.  
 *	A blown fuse makes the corresponding bit in SC_SET_EN<2:0> be
 *	a read-only bit.
 * 
 *  FORM_OF_CALL:
 * 	bsr r28, SCacheSets
 * 
 *  RETURNS:
 * 	R0 -  Sets that can be used (in correct position).
 *	R1 -  1 set that may be enabled.
 * 
 *  ARGUMENTS:
 * 	None.
 *
 *  REGISTERS:
 *	R0:R2 - Scratch.
 *	
 */
SCacheSets:
					/* Find out which sets work.	*/
	LDLI	(r1, ((7 << SC_V_SET_EN) | SC_M_BLK_SIZE))
	mb				/* We write first because in a	*/
	stq_p	r1, scCtl(r13)		/* time out reset, this register*/
	mb				/* is not reset, thus we may 	*/
	ldq_p 	r2, scCtl(r13)		/* read an old setting.		*/

	srl	r2, SC_V_SET_EN, r2	/* Check if all 3 sets work.	*/
	and	r2, 7, r0		/* Isolate the set bits.	*/

	LDLI	(r1, (1 << SC_V_SET_EN)) /* Default set if all 3 sets OK */
	cmplt	r0, 7, r2		/* Skip if all 3 sets are OK.	*/
	beq	r2, AllSetsOn		

OneSetLoop:
	and	r0, r2, r1		/* Go thru and find out the 1st	*/
	sll	r2, 1, r2		/* good set.  Only 1 or all 	*/
	beq	r1, OneSetLoop		/* sets must be turned on.	*/
 	sll	r1, SC_V_SET_EN, r1	/* One set that may be enabled.	*/
AllSetsOn:
 	sll	r0, SC_V_SET_EN, r0
	ret	r31, (r28)
#endif /* ifndef SC_BANKS */


/* ======================================================================
 * =  BCnfgOff - Computes BC_CNFG register value when the BCache is off.=
 * ======================================================================
 *
 *  OVERVIEW:
 *
 *	This routine will compute the appropriate value for the BC_CNFG
 *	register when the bcache is to be turned off.  
 *
 *	BCache Write and Read speeds are set to cpu/sys clock ratio as
 *	required by EV5.  The rest of the fields are not changed.
 *	value and the cache size is set to 0.
 * 
 *  FORM_OF_CALL:
 * 	bsr r28, BCnfgOff
 * 
 *  RETURNS:
 * 	R0 -  BC_CNFG register value for the bcache off.
 * 
 *  ARGUMENTS:
 * 	R21 - BC_CFNG.
 *
 *  REGISTERS:
 *	R0,R1 - Scratch.
 *	Those used by get_cpusys_ratio R0-R6, R16-R17, R26-R29
 *	
 */
BCnfgOff:
	sll	r5, 4, r1		/* Place the ration in the read	*/
	bis	r5, r1, r0		/* and write fields.		*/
	sll	r0, 4, r0
	lda	r1, 0x0FFF(r31)		/* Clear the read and write	*/
	bic	r21, r1, r1		/* speeds & size from bc config.*/
	bis	r1, r0, r0		/* Merge new speeds with rest	*/
	ret	r31, (r28)


/* ======================================================================
 * =  BCacheOnOff - Writes BC_CNFG & BC_CTL and appropriate memory	=
 * =		    controller registers.				=
 * ======================================================================
 *
 *  OVERVIEW:
 *	This routine will write the CPU registers controlling the bcache
 *	functions and compute the appropriate memory controller register
 *	values depending on whether the bcache is being turn on or off.
 *
 *  FORM_OF_CALL:
 * 	bsr r28, BCacheOnOff
 * 
 *  RETURNS:
 *	Nothing
 * 
 *  ARGUMENTS:
 * 	R18 - Value to be written to BC_CFNG.
 *	R19 - Value to be written to BC_CTL.
 *
 *  REGISTERS:
 *	R0,R1,R2
 *
 */
BCacheOnOff:

#define MCR_BCSTATE_K_MASK  (MC_MCR_M_BCACHE_ENABLE | MC_MCR_M_OVERLAP_DISABLE)

	ldah	r1, 0x1000(r14)		/* MCR ponter */
	ldl_p	r0, SEXT10b(MC_MCR)(r1)		/* read MCR */
	LDLI	(r2,  MCR_BCSTATE_K_MASK)
	bic		r0, r2, r0
	blbc	r19, mcr_bc_off		/* branch if bcache is to be turned off */
	bis		r0, r2, r0
mcr_bc_off:
	ldah	r1, 0x1000(r14)		/* MCR ponter */
	stl_p	r0, SEXT10b(MC_MCR)(r1)
	mb
	ldl_p	r0, SEXT10b(MC_MCR)(r1)		/* read MCR */

	ldl_p	r0, SEXT10b(MC_GTR)(r1)		/* read GTR */
	LDLI	(r2,  MC_GTR_M_IDLE_BC_WIDTH)
	bic		r0, r2, r0
	blbc	r19, gtr_bc_off
	LDLI	(r2,  (3<<MC_GTR_V_IDLE_BC_WIDTH))
	bis		r0, r2, r0
gtr_bc_off:
	stl_p	r0, SEXT10b(MC_GTR)(r1)		/* write GTR */
	mb
	ldl_p	r0, SEXT10b(MC_GTR)(r1)		/* read GTR */

	mb
	stq_p	r19, bcCtl(r13)		/* write the BC_CONTROL register */
	mb
	stq_p	r18, bcCfg(r13)		/* write the BC_CONFIG register */
	mb

	ret	r31, (r28)


#ifndef HARD_MEM_SIZE	// RBB - This is for auto sizing memory after initial debug.
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
//   r0 - scratch
//   r1 - address of IIC ctrl register
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
	load_csr_base(r23)		/* Base for config registers.	*/
	ldah	r23, 0x6000(r23)	/* build address 0x87.A000.02C0 */
	lda		r23, 0x2C0(r23)
		
	ldl_p	r0, 0(r23)		//get current state of bus
	and		r0, 1, r0		//just need IIC data line
	sll		r0, 3, r0		//move it to data out bit
	bis		r0, 0x14, r0		//bring clock low leaving
	stl_p	r0, 0(r23)		//  data as it was before.
	mb				//

	lda		r2, 8(r31)		//read 8 bits
	bis		r31, r31, r3	//accumulate in r3
read_more_bits:
	sll		r3, 1, r3		//make room for new data
	lda		r4, 0x10(r31)		//scl=0, sda=tristate
	stl_p	r4, 0(r23)		//
	mb						//
	USDELAY	(100)			//wait 1 bit time
	lda		r4, 0x30(r31)		//scl=1, sda=tristate (data from slave)
	stl_p	r4, 0(r23)		//
	mb				//
	USDELAY	(100)			//wait 1 bit time
	ldl_p	r4,  0(r23)		//get data from slave
	and		r4, 1, r4		//mask off other junk
	bis		r4, r3, r3		//
	subl	r2, 1, r2		//decrement counter
	bne		r2, read_more_bits			//

	lda	r4, 0x10(r31)		//scl=0, sda=tristate
	stl_p	r4, 0(r23)		//
	mb				//
	USDELAY	(100)			//wait 1 bit time
	and	r3, 0xff, r3		//keep only low byte
	ret	r31, (r29)		//

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
//   r0 - scratch, current bit to xmit
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
// scratch   -- r0, r1, r4, r2
// Input     -- R3 = byte to xmit
// Output    -- R3 = ACK status in bit 3
// IIC @exit -- 
//
//
// address of IIC rom is  '1010.aaa.d' where a=addr, d=direction=0-W,1-R
//
srom_iic_write_byte:
	load_csr_base(r23)		/* Base for config registers.	*/
	ldah	r23, 0x6000(r23)	/* build address 0x87.A000.02C0 */
	lda		r23, 0x2C0(r23)
		
	lda	r2, 8(r31)		//write 8 bits
	sll	r3, 3, r3		//data to write in r3
					//put low bit in good spot for xmit
write_more_bits:
	ldl_p	r0, 0(r23)		//get current state of bus
	and	r0, 1, r0		//just need IIC data line
	sll	r0, 3, r0		//move it to data out bit
	bis	r0, 0x14, r0		//bring clock low leaving
	stl_p	r0, 0(r23)		// data line as it was before
	mb				//
	USDELAY	(50)			//wait 1/2 bit time
	srl	r3, 7, r0		//position current msb for xmit
	and	r0, 0x8, r0		//mask off all but bit to xmit
	bis	r0, 0x14, r4		//assemble control byte
	stl_p	r4, 0(r23)		//get sda on bus w/scl=0
	mb				//
	USDELAY	(100)			//wait 1 bit time
	bis	r4, 0x30, r4		//scl=1, sda=correct data
	stl_p	r4, 0(r23)		//
	mb				//
	USDELAY	(100)			//wait 1 bit time
	sll	r3, 1, r3		//position next msb
	subl	r2, 1, r2		//decrement counter
	bne	r2, write_more_bits

	bic	r4, 0x20, r4		//scl=0, sda=last data bit sent
	stl_p	r4, 0(r23)		//prepare to receive ACK bit from slave
	mb				//
	USDELAY	(50)			//wait 1/2 bit time
	lda	r4, 0x10(r31)		//scl=0, sda=tristate
	stl_p	r4, 0(r23)		//prepare to receive ACK bit from slave
	mb				//
	USDELAY	(100)			//wait 1 bit time
	lda	r4, 0x30(r31)		//scl=1, sda=tristate
	stl_p	r4, 0(r23)		//ACK bit from slave available
	mb				//
	USDELAY	(100)			//wait 1 bit time
	ldl_p	r3, 0(r23)		//ACK status is in bit 3		
	and	r3, 1, r3		//mask off other garbage
	lda	r4, 0x10(r31)		//scl=0, sda=tristate
	stl_p	r4, 0(r23)		//Prepare for read
	mb				//
	USDELAY	(100)			//wait 1 bit time
	ret	r31, (r29)		//
		

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
//   r0 - scratch
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
	load_csr_base(r23)		/* Base for config registers.	*/
	ldah	r23, 0x6000(r23)	/* build address 0x87.A000.02C0 */
	lda		r23, 0x2C0(r23)
		
	USDELAY (50)			//wait 1/2 bit time
	lda	r4, 0x3c(r31)		//scl=1, sda=1
	stl_p	r4, 0(r23)		//
	mb				//
	USDELAY (50)			//wait 1/2 bit time
	lda	r4, 0x34(r31)		//Start is sda 1->0 w/ scl=1
	stl_p	r4, 0(r23)		//
	mb				//
	USDELAY	(50)			//wait 1/2 bit time
	ret	r31, (r29)		//
	

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
//   r0 - scratch
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

	load_csr_base(r23)		/* Base for config registers.	*/
	ldah	r23, 0x6000(r23)	/* build address 0x87.A000.02C0 */
	lda		r23, 0x2C0(r23)
		
	USDELAY	(50)			//wait 1/2 bit time
	ldl_p	r0, 0(r23)		//get current state of bus
	and	r0, 1, r0		//just need IIC data line
	sll	r0, 3, r0		//move it to data out bit
	bis	r0, 0x14, r0		//bring clock low leaving
	stl_p	r0, 0(r23)		// data line as it was before
	mb				//
	USDELAY	(50)			//wait 1/2 bit time
	lda	r4, 0x14(r31)		//scl=sda=0
	stl_p	r4, 0(r23)		//
	mb				//
	USDELAY	(100)			//wait 1 bit time
	lda	r4, 0x34(r31)		//stop is sda 0->1 w/scl=1
	stl_p	r4, 0(r23)		//
	mb				//
	USDELAY	(50)			//wait 1/2 bit time
	lda	r4, 0x3c(r31)		//bus idle is scl=sda=1
	stl_p	r4, 0(r23)		//
	mb				//
	USDELAY	(50)			//wait 1/2 bit time
	ret	r31, (r29)		//

//++
// ===========================================================================
// = srom_iic_master_ack  - IIC Master ACK a data transfer from a slave  =
// ===========================================================================
//
// OVERVIEW:
//   Acknowledge the successful receipt of data by the IIC
//   master transmitted from the IIC slave.
//
//
// FORM OF CALL:
//
//	bsr	r27,srom_iic_master_ack
//
// ARGUMENTS:
//    none
//
// RETURNS:
//    none
//
// IIC BUS AT ROUTINE EXIT:
//    scl=0  sda=tristate
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
//   r0 - scratch
//   r1 - address of IIC ctrl register
//   r4 - IIC bus state
//
// ALGORITHM:
//
//
//--
// address of IIC rom is  '1010.aaa.d' where a=addr, d=direction=0-W,1-R
//
srom_iic_master_ack:

	load_csr_base(r23)		/* Base for config registers.	*/
	ldah	r23, 0x6000(r23)	/* build address 0x87.A000.02C0 */
	lda		r23, 0x2C0(r23)
		
	ldl_p	r0, 0(r23)		//get current state of bus
	and	r0, 1, r0		//just need IIC data line
	sll	r0, 3, r0		//move it to data out bit
	bis	r0, 0x14, r0		//bring clock low leaving
	stl_p	r0, 0(r23)		//scl=0// sda=prev_data
	mb				//
	USDELAY	(50)			//wait 1/2 bit time
	lda	r4, 0x14(r31)		//scl=sda=0
	stl_p	r4, 0(r23)		//sda=0 -> ack
	mb				//
	USDELAY	(100)			//wait 1 bit time
	lda	r4, 0x34(r31)		//scl=1, sda=0 ==> ACK to slave
	stl_p	r4, 0(r23)		//
	mb				//
	USDELAY	(100)			//wait 1 bit time
	ret	r31, (r29)		//

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
//	bsr	r28,srom_iic_read_rom
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
	and	r5, 0xf8, r3		// mask off all but [2:0]
	sll	r5, 1, r3		// put addr in right place
	bis	r3, 0xa0, r3		// address EEPROM device type, dir=W
	bsr	r29, srom_iic_write_byte	//
	bne	r3, srom_iic_read_rom_err	// check return stat

	zap	r7, 0xfe, r3		// Send offset to be read, no dir
	bsr	r29, srom_iic_write_byte	//
	bne	r3, srom_iic_read_rom_err	// check return stat

	bsr	r29, srom_iic_start		//

	sll	r5, 1, r3		// put addr in right place
	bis	r3, 0xa1, r3		// address EEPROM device type, dir=R
	bsr	r29, srom_iic_write_byte	//
	bne	r3, srom_iic_read_rom_err	// check return stat

	bsr	r29, srom_iic_read_byte	// read a byte from iic device
	bis	r3, r3, r5		// 

	bsr	r29, srom_iic_stop		//
	ret	r31, (r28)		//

//--------------------------------------------------------------------
srom_iic_read_rom_err:
	// jjd - IIC bus error halts query, resets bus, 
	// and returns bad status
	bsr	r29, srom_iic_stop		//
	lda	r5, 0x7fff(r31)		// return IIC bus error code
	ret	r31, (r28)		// nothing for now
//--------------------------------------------------------------------


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
//	bsr r30, srom_usdelay
//	
// RETURNS:
//
//	none
//
// ARGUMENTS:
//
//	r16	- number of microseconds to delay
//	r24	- CPU speed in psec
//
// REGISTERS:
//
//	r0	- scratch
//	r16	- number of microseconds to delay
//	r20	- scratch
//	r30	- return address
//	pt0	- scratch
//	r24	- CPU speed in psec
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

srom_usdelay:
	bis		r16, r16, r20		// save r16 -> r20
	//LDLI	(r17, 1000000)
	//bis		r24, r24, r16		// CPU speed in psec
	//bsr		r26, nintdivide		//
	lda		r0, 667(r31)		// hard code for now
	mulq	r0, r16, r0			// total usec count
usdelay_init:
	lda		r20, 1(r31)			// r20 <- 1.0000.0000
	sll		r20, 32, r20		// :
	mtpr	r20, ccCtl			// clear cycle count
	mfpr	r31, pt0			// wait 3 cyc to avoid palcode violation
	mfpr	r31, pt0
	mfpr	r31, pt0
	mfpr	r31, pt0
usdelay_loop:					// REPEAT
	rpcc	r20					// : read the cycle counter
	zapnot	r20, 0x0f, r20		// : clear the high longword
	cmplt	r0, r20, r20		// : 
	beq		r20, usdelay_loop	// UNTIL we have waited time specified
	ret		r31, (r30)			// RETURN

usdelay:						// called using r27
	bsr	r0,  saveRegisters
	bsr	r30, srom_usdelay
	br	r31, restoreRegisters

//++
// ===========================================================================
// =  saveRegisters, restoreRegisters - save/restore paltemp stack	    =
// ===========================================================================
//
// OVERVIEW:  
//
//	These routines are designed to be used in conjuntion with a jump table
//	of low-level routines.  Once saveRegisters is called, it cannot be
//	called again until restoreRegisters is called.  This provides only one
//	level of nesting below the current level of execution.
//
//	NOTES:
//
//	    1. The contents of PT1 through PT17 are clobbered.
//	    2. The call must always be made through R27.
//
// FORM_OF_CALL:
//
//	bsr r31, saveRegisters
//	bsr r31, restoreRegisters
//
// EXAMPLE:
//
//	These routines are most useful in a jump table as follows.
//	R0 will not be preserved in the following scheme, though it may be used
//	by routines in the jump table to return a value.
//	
//	mPutChar:
//	    bsr     r0, saveRegisters	// GOTO save registers
//		bsr	r27, putChar			// CALL the routine
//		br	r31, restoreRegisters	// RETURN
//		:
//		:
//		bsr	r27, mPutChar			// call putChar routine
//
// RETURNS:
//
//	none
//
// ARGUMENTS:
//
//	registers to be saved/restored
//
//--

saveRegisters:			// The contents of these registers will
    mtpr	r30, pt1	// : be preserved across calls to the
    mtpr	r2,  pt2	// : low level routines.
	mtpr	r3,  pt3
	mtpr	r4,  pt4
	mtpr	r5,  pt5
	mtpr	r6,  pt6
	mtpr	r7,  pt7
	mtpr	r8,  pt8
    mtpr	r9,  pt9
    mtpr	r16, pt10
	mtpr	r17, pt11
    mtpr	r18, pt12
    mtpr	r19, pt21
	mtpr	r20, pt14
	mtpr	r21, pt15
	mtpr	r22, pt16
    mtpr	r26, pt17
    mtpr	r27, pt18
    mtpr	r28, pt19
    mtpr	r29, pt20
	ret	r31, (r0)       // GOTO the call to the routine

restoreRegisters:		// The contents of these registers were
    mfpr	r30, pt1	// : preserved across calls to the
    mfpr	r2,  pt2	// : low level routines.
	mfpr	r3,  pt3
	mfpr	r4,  pt4
	mfpr	r5,  pt5
	mfpr	r6,  pt6
	mfpr	r7,  pt7
	mfpr	r8,  pt8
    mfpr	r9,  pt9
    mfpr	r16, pt10
    mfpr	r17, pt11
    mfpr	r18, pt12
    mfpr	r19, pt21
    mfpr	r20, pt14
    mfpr	r21, pt15
    mfpr	r22, pt16
    mfpr	r26, pt17
    mfpr	r27, pt18
    mfpr	r28, pt19
    mfpr	r29, pt20
	nop
    ret     r31, (r27)

//++
// ===========================================================================
// =  get_bcr_bits                                                           =
// ===========================================================================
//
// OVERVIEW:  
//
//	This routine ors in the required bits based on the information obtained
//	from the memory sizing.
//
//	NOTES:
//
//	    1. The contents of PT1 through PT17 are clobbered.
//	    2. The call must always be made through R27.
//
// FORM_OF_CALL:
//
//	bsr r26, get_bcr_bits
//
// RETURNS:
//
//	r0	- The needed bits set for Bank Control Register (BCRn)
//
// ARGUMENTS:
//
//	r2	- The bank number being configured.
//	r12	- The feature bitmask register.
//	r16 - scratch register
//	r17	- scratch register
//
//--

get_bcr_bits:

	bis		r31, r31, r0		// clear r0 to start
	srl		r12, r2, r16		// get bit of DIMM Sub Banks field
	bis		r0, 0x20, r17		// temp set the SUBBANK_ENABLE bit
	cmovlbs	r16, r17, r0		// set the bit if needed

	srl		r12, 8, r16			// get the 4BANK field of the feature mask
	srl		r16, r2, r16		// get the 4BANK field for that bank
	bis		r0, 0x80, r17		// temp set the 4BANK bit
	cmovlbs	r16, r17, r0		// set the bit if needed

	srl		r12, 16, r16		// get the rows field of the feature mask
	srl		r16, r2, r16		// get the rows field for that bank
	bis		r0, 0x40, r17		// temp set the ROWSEL bit
	cmovlbs	r16, r17, r0		// set the bit if needed

	ret		r31, (r26)

//++
// ===========================================================================
// =  get_btr_bits                                                           =
// ===========================================================================
//
// OVERVIEW:  
//
//	This routine ors in the required bits based on the information obtained
//	from the memory sizing for the Bank Timing Registers.
//
// FORM_OF_CALL:
//
//	bsr r26, get_btr_bits
//
// RETURNS:
//
//	r0	- The needed bits set for Bank Timing Register (BTRn)
//
// ARGUMENTS:
//
//	r2	- The bank number being configured.
//	r12	- The feature bitmask register.
//	r16 - scratch register
//	r17	- scratch register
//
//--

get_btr_bits:

	//
	// Need to add code here
	//
	ret		r31, (r26)

#endif	// HARD_MEM_SIZE


/*=======================================================================
 *= ClearCPUErrors - Clears CPU error registers.			=
 *=======================================================================
 * OVERVIEW:
 *      Clears error registers in the CPU.
 *
 * FORM OF CALL:
 *       bsr r28, ClearCPUErrors
 *
 * ARGUMENTS:
 *	None.
 *	
 * RETURNS:
 *	R0 - Value in EI_STAT.
 *	
 * REGISTERS:
 *	r0-r3 - Scratch registers.
 */
ClearCPUErrors:
	mtpr	r31, excSum		/* clear out exeception summary */
					/* and exc_mask 		*/
	mfpr	r31, va			/* unlock faulting va, mmstat 	*/

#define icp$stat_init (ICPERR_M_DPE | ICPERR_M_TPE  | ICPERR_M_TMR)
	lda	r2, icp$stat_init(r31)	/* Clear Icache data and tag 	*/
	mtpr	r2, icPerr		/* parity error, & timeout error*/

#define dcp$stat_init (DCPERR_M_LOCK | DCPERR_M_SEO)
	lda	r2, dcp$stat_init(r31)	/* Clear Dcache parity error 	*/
	mtpr	r2, dcPerr		/* status		 	*/

/*
 * Clear the Serial Line, Performance Counter,and CRD interrupt requests
 */
#define HWINT_CLR_ALL (( HWINT_M_PC0C | HWINT_M_PC1C | HWINT_M_PC2C | HWINT_M_CRDC | HWINT_M_SLC) >> 16 )
	ldah	r2, (HWINT_CLR_ALL >> 16)(r31)
	sll		r2, 16, r2
	ldah	r2,	(HWINT_CLR_ALL & 0xffff)(r2)

	mtpr	r2, hwIntClr

	ldq_p	r31, scStat(r13)	/* Clear SCache errors.		*/
	ldq_p	r0, eiStat(r13)		/* Clear ei_stat, ei_addr, etc	*/
	ret	r31, (r28)


/*=======================================================================
 *= ClearMemContErrors - Clears error registers in the PYXIS.		=
 *=======================================================================
 * OVERVIEW:
 *      Clears error registers in the PYXIS.
 *
 * FORM OF CALL:
 *       bsr r28, ClearMemContErrors
 *
 * ARGUMENTS:
 *	None.
 *	
 * RETURNS:
 *	Nothing.
 *	
 * REGISTERS:
 *	r0-r3 - Scratch registers.
 */
ClearMemContErrors:
	mb
	load_csr_base(r14)		/* Base for config registers.	*/
	lda	r2, 0x4000(r14)		/* Load the address of error	*/
	lda	r2, 0x4200(r2)		/* register.			*/
	ldl_p	r1, 0x0(r2)		/* Read Error Register		*/
	stl_p	r1, 0x0(r2)		/* Write Error Register		*/
	mb
	ldl_p	r1, 0x0(r2)		/* Make sure it's gotten there	*/
	ret	r31, (r28)

/*
 *		    =================================
 *		    =  Board-independent routines.  =
 *		    =================================
 */

/*=======================================================================
 *= InitSIO - Initializes the SIO chip (82738)				=
 *=======================================================================
 * OVERVIEW:
 *      This function will initialize the SIO chip which controls the
 *	PCI and ISA bus.  
 *	Access to IO space is required, therefore, the memory controller
 *	should be initialized before calling this routine.  After the 
 *	initialization has been performed, access to the LED card will 
 *	be possible.
 *
 * FORM OF CALL:
 *       bsr r26, InitSIO
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
 *      r26, r27, r29 - Return address registers.
 */
InitSIO:

/* 
 *  First, reset the ISA Bus.  It must be held in reset for at least 1usec.
 *  After reset, the ISA bus speed will be the SYSCLK/4 (note that the
 *  SYSCLK value must be such that the ISA speed falls within its 6 - 8.33MHz
 *  spec'ed speed), Positive decode for upper 64 KB of BIOS is enabled and
 *  for EB164, IRQ12 will provide the mouse function.  
 */
	lda	r16, 0x8(r31)		/* Assert RSTDRV signal		*/
	bsr	r29, WriteSIO_ICD	/* Write to the clock divisor	*/

	lda	r16, 1000(r31)		/* load cycles into R16.	*/
	bsr	r27, wait_n_cycles 	/* Wait at least 1usec.		*/

	lda	r16, 0x40(r31)		/* De-assert ISA Bus reset.  	*/
	bsr	r29, WriteSIO_ICD	/* Write to the clock divisor	*/

/*
 *  Configure the utility bus:
 *  UBCSA: Allow access to Real Time Clock, Keyboard, Lower and Extended Bios.
 *  (pc164 only):  Allow access to Lower and Extended Bios.
 *  UBCSB: Enable config. RAM, Port 92. Disable Port A and B, and Parallel Port.
 *
 */
#define sio$ubcsa_init (sio$ubcsa_lbios	| sio$ubcsa_xbios)
#define sio$ubcsb_init ((1<<sio$ubcsb_v_confram) | (1<<sio$ubcsb_v_port92) | \
			(3<<sio$ubcsb_v_parallel)| (3<<sio$ubcsb_v_portb)  | \
			(3<<sio$ubcsb_v_porta))

	lda	r16, sio$ubcsa_init(r31)
	bsr	r29, WriteSIO_UBCSA	/* Set ubcsa register.		*/

	lda	r16, sio$ubcsb_init(r31)
	bsr	r29, WriteSIO_UBCSB	/* Set ubcsb register.		*/

	lda	r16, 0xC(r31)		/* Turn off GAT mode, 16 cycles */
	bsr	r29, WriteSIO_PAC	/* park the bus.		*/	
	lda	r16, 0xF0(r31)		/* Priority to full rotation 	*/
	bsr	r29, WriteSIO_PAPC

	ret	r31, (r26)


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
	ret	r31, (r27)


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
WriteSIO_ICD:
	lda	r1, SIO_ISA_CLK_DIV(r31) /* SIO's ISA clock divisor.	*/
	br	r31, WriteConf

WriteSIO_PAC:
	lda	r1, SIO_ARB_CTRL(r31)	/* SIO's PCI Arbiter Control.	*/
	br	r31, WriteConf
WriteSIO_PAPC:
	lda	r1, SIO_ARB_PRIO(r31)	/* SIO's PCI Arbiter Priority Cntrl */
	br	r31, WriteConf

WriteSIO_UBCSA:
	lda	r1, SIO_UTIL_BUS_A(r31)	/* SIO's Utility Bus select A.	*/
	br	r31, WriteConf
WriteSIO_UBCSB:
	lda	r1, SIO_UTIL_BUS_B(r31)	/* SIO's Utility Bus select B.	*/

WriteConf:
	and	r1, 3, r3		/* Find out correct byte lane.	*/
	insbl	r16, r3, r2		/* Place value in its byte lane */
	sll	r1, BYTE_ENABLE_SHIFT, r1 /* Compose address.		*/
	load_pci_config_base(r3)	/* Load base of PCI conf. space.*/
	ldah	r16, (19-11)(r31)	/* SIO has device ID = 19	*/
	bis	r3, r16, r3		/* Add it to config address.	*/
	br	r31, WriteData

WriteLEDS:
	cmpeq	r2, 0x20, r1		/* If ledcode value is 0x20,	*/
	bne	r1, skipSromPort	/* then skip srom write.	*/

	bis	r29, r29, r1
	PRINTC(a_period)
	PRINTB(r2)
	bis	r1, r1, r29
skipSromPort:
	lda	r1, LED_PORT(r31)	/* POST ledcard port.		*/
	br	r31, WriteCont
WriteFBank:
	lda	r1, FLASH_PORT(r31) 	/* Flash bank select (addr 19)	*/
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

	and	r1, 3, r3		/* Find out correct byte lane.	*/
	insbl	r2, r3, r2		/* Place value in its byte lane */
	sll	r1, BYTE_ENABLE_SHIFT, r1 /* Compose address.		*/
	load_pci_io_base(r3)		/* Load base of PCI I/O space.	*/

WriteData:
	bis	r1, r3, r1		/* Add port to base address.	*/
	stl_p	r2, 0x00(r1)		/* send the byte to the port	*/
	mb				/* make sure it gets there	*/
	ret	r31, (r29)		/* Back we go			*/


/*=======================================================================
 * = ReadRTCData - Reads a byte from the RTC Data port.			=
 * = ReadConf1 - Reads the configuration 1 jumpers.			=
 * = ReadConf2 - Reads the configuration 2 jumpers.			=
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
	lda	r1, RTC_DATA_PORT(r31)	/* Real Time Clock data port.	*/
	br	r31, ReadCont
ReadConf2:
	lda	r1, CONF_JUMPERS2(r31)	/* Read the configuration jumpers*/
	br	r31, ReadCont
ReadConf1:
	lda	r1, CONF_JUMPERS1(r31)	/* Read the configuration jumpers*/

ReadCont:
	sll	r1, BYTE_ENABLE_SHIFT, r2 /* Compose address.		*/
	load_pci_io_base(r3)		/* Load base of PCI I/O space.	*/
ReadData:
	bis	r2, r3, r2		/* Add port to base address.	*/	
	ldl_p	r0, 0x00(r2)		/* Load LW containing the byte.	*/
	and	r1, 3, r1		/* Compute EXTBL value.		*/
	extbl	r0, r1, r0		/* Get the correct byte out.	*/
	ret	r31, (r29)		/* Back we go			*/


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
	ret	r31, (r27)


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
	ret	r31, (r26)


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
	ret	r31, (r26)


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
	zap	r17, 0xF0, r17		/* blow off the high longword	*/
	stl_p	r17, 0(r16)		/* write			*/
	mb				/* make sure it's not queued.	*/
srom_rdtest:
	ldl_p	r18, 0(r16)		/* r18 <- read back 		*/
	zap	r18, 0xF0, r18		/* blow off the high longword	*/
	xor	r18, r17, r19		/* compare read to write	*/

#if 0	/* #ifdef	DEBUG_BOARD  */
/*
 *  This is a test designed to print out information thru the
 *  SROM port if the data read back is not the same as the data
 *  written.
 */
	beq	r19, equal		/* If Read == Write, skip msgs.	*/
not_equal:	
	PRINTS4(b4_to_l(a_cr,a_nl,a_A,a_colon))
	srl	r16, 32, r0
	PRINTL(r0)			/* Print the address in R16.	*/
	PRINTL(r16)			/* Print the address in R16.	*/
	PRINTS4(b4_to_l(a_sp,a_W,a_colon,a_nul))
	PRINTL(r17)			/* Print the data to be written	*/
	PRINTS4(b4_to_l(a_sp,a_R,a_colon,a_nul))
	PRINTL(r18)			/* Print the data read back.	*/
equal:
#endif /* DEBUG_BOARD */
	ret	r31, (r26)


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

	srl	r30, JMP_V_MINIDBGR, r1	/* Jump to minidbg on error?	*/
	blbs	r1, FatalError		/* Loop if no jump.		*/

	jump_to_minidebugger()		/* trap to Mini-Debugger.	*/
	br	r31, FatalError


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
	ret	r31, (r28)


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
	LDLI	(r0, HARD_SPEED*1000000)
#else

/* 
 * Turn on osc and set interrupt rate to 512Hz or 1.953125 ms.
*/
	lda	r16, rtc$reg_a(r31)	/* R16 <- 0xA.			*/
	bsr	r29, WriteRTCAddr
	lda	r16, (rtc$m_a_osc_on | rtc$m_a_rs_512hz)(r31)
	bsr	r29, WriteRTCData	/* R16 <- 0x27			*/

/* 
 * Turn on reporting of interrupts thru IRQ signal, put counter in binary
 * mode for data read and 24-hour mode (needed by Debug Monitor) and set 
 * daylight savings time.
 */
	lda	r16, rtc$reg_b(r31)	/* R16 <- 0xB			*/
	bsr	r29, WriteRTCAddr
//	lda	r16, (rtc$m_b_pie | rtc$m_b_dm_bin | rtc$m_b_24 | rtc$m_b_dse)(r31)
	lda	r16, (rtc$m_b_pie | rtc$m_b_dm_bin | rtc$m_b_24)(r31)
	bsr	r29, WriteRTCData	/* R16 <- 0x47			*/

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
	ret	r31, (r28)


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
 *	r1-r3 - Scratch registers.
 */
wait_for_pif:
	lda	r16, rtc$reg_c(r31)
	bsr	r29, WriteRTCAddr
	bsr	r29, ReadRTCData	/* R0 <- byte from RTC data port*/
	srl	r0, rtc$v_c_pf, r0	/* Shift PIF into low bit.	*/
	blbs	r0, wait_for_pif	/* Wait for deassertion.	*/

wait_assert:
	bsr	r29, WriteRTCAddr
	bsr	r29, ReadRTCData	/* R0 <- byte from RTC data port*/
	srl	r0, rtc$v_c_pf, r0	/* Shift PIF into low bit.	*/
	blbc	r0, wait_assert		/* Wait for an interrupt flag.	*/
	subq	r17, 1, r17		/* Check to see if we're done.	*/
	bne	r17, wait_for_pif
	ret	r31, (r27)


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
 *  R6  - Flash offset where search should continue if this image
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
 *	R13, R20-R24
 */
LoadSystemCode:
					/* dependent search.		*/
	lda	r10, ((1 << 8) | 4)(r31) /* First image if bit is set	*/
	lda	r1, ((2 << 8) | 4)(r31) /* 2nd image if bit is cleared	*/
	srl	r16, JMP_V_ALT_BOOT, r0	/* Check boot jumper state.	*/
	cmovlbc	r0, r1, r10		/* If cleared, then 2nd image.	*/

scanROM:	
	bis	r31, r31, r12		/* Init. count of headers found */
	bis	r31, r31, r6		/* Load starting offset.	*/

rescanROM:
	bis	r6, r6, r17		/* R17 holds offset into flash. */
	bis	r31, r31, r7		/* Init. checksum register.	*/
	bic	r10, 1, r10		/* Clear the V0 flag.		*/

	LDLI	(r0, ROMSIZE)		/* Load maximum size of ROM.  	*/
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
	LDLI	(r1, 0x5a5ac3c3)	/* Load pattern 0x5a5ac3c3	*/
	cmpeq	r0, r1, r1		/* Does it match pattern1?	*/
	bne	r1, InversePattern	/* Yes, then for inverse pattern*/
	LDLI	(r1, 0xc3e0)		/* See if it's a branch instr.	*/
	extwl	r0, 2, r0		/* Extract bits 31:16.		*/	
	cmpeq	r0, r1, r0		/* Compare to branch instr.	*/
	beq	r0, rescanROM		/* No, then check next LW.	*/

InversePattern:
	bsr	r28, read_flash		/* R0 <- LW at address (R17)	*/
					/* R17 <- R17+4			*/
	LDLI	(r1, 0xa5a53c3c)	/* Load inverse pattern. 	*/
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

	srl	r10, 1, r1
	blbc	r1, PositionDependent	/* Position dependent search?	*/

FirmwareID:				/* Firmware ID dependent search.*/
					/* Is this a V0 header?		*/
	blbs	r10, rescanROM		/* Yes. It can't be used. Rescan*/
	addq	r6, (0x20-0x4), r17	/* Point to firmware id field.	*/
	bsr	r28, read_flash		/* Read firmware ID.		*/
	extbl	r0, 1, r1		/* Extract the firmware id	*/

PositionDependent:
	srl	r10, 1, r2
	cmovlbc r2, r12, r1		/* Use position instead of ID.	*/
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

 	bis	r31, r31, r8		/* Default dest. to address 0.	*/
	bis	r31, r31, r5		/* No compression. 		*/
	bis	r31, r31, r17		/* Start loading at offset 0.	*/
	LDLI	(r9, ROMSIZE)		/* Load maximum size of ROM.	*/
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

	lda	r8, LED_F_MEMCHECKSUM(r31)/* Ledcode value to print.	*/
	br	r31, FatalError		/* image checksum in memory.	*/

SumMismatch:
	subq	r12, 1, r12		/* Decr. header cntr if mistmatch*/
	br	r31, rescanROM		/* If not equal, then rescan.	*/
codeLoaded:
	ret	r31, (r14)


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

	ldah	r0,  (FLASH_BIT_SWITCH_OFFSET>>16)(r31)	 /* 0x0008.0000 */
	cmple	r0, r17, r2		/* Is 0x0008.0000 <= offset? 	*/
	sll	r2,  19, r3		/* adjust amount to base address*/
	subq	r17, r3, r4		/* subtract 0 or 0x8.0000	*/

	bsr	r29, WriteFBank		/* Switch banks. r2 is 0 or 1	*/

	ldah	r0, SEXT(FROM_BASE>>16)(r31) /* base address of Flash memory*/
	zap	r0, 0xF0, r0		/* blow off the high longword	*/
	addq	r4, r0, r4		/* Add offset to base address.	*/

	load_pci_d_mem_base(r0)		/* Get the base for dense space.*/
	addq	r0, r4, r4		/* Add address to base.		*/
	ldl_p	r0, 0(r4)		/* Load LW from flash memory	*/
	zap	r0, 0xF0, r0		/* blow off the high longword	*/	
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
	LDLI	(r4, 0x8000)		
	addq	r3, r4, r4		/* Implement bit rotation	*/
	cmovlbs r7, r4, r3
	extbl	r0, r2, r4		/* Extract byte to add to sum.	*/
	addq	r3, r4, r7		/* Add byte to checksum.	*/
	zapnot	r7, 0x03, r7		/* Mask off upper bytes.	*/
	addq	r2, 1, r2		/* Increment byte counter.	*/
	cmpeq	r2, 4, r4		/* Are we done with 4 bytes?	*/
	beq	r4, CheckSumLoop
	ret	r31, (r28)


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
	ret	r31, (r26)		/* back  			*/

repByte:
	bis	r2, r2, r16
	lda	r19, 1(r19)		/* inc the count 		*/
	beq	r19, getRecord		/* end of record? get next 	*/
	ret	r31, (r26)		/* back  			*/

getRecord:
	bsr	r28, ReadFByte		/* Read header (byte count)-> R0*/
	sll	r2, 56, r19		/* sign extend the record flag	*/
	sra	r19, 56, r19		/* sign extend the record flag	*/
	bge	r19, norepeat		/* if neg then need to get 	*/
	bsr	r28, ReadFByte		/* repeating byte -> R2 	*/

norepeat:	
	ret	r31, (r26)		/* back we go 			*/


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
	ret	r31, (r28)

.align 3
minidebugger:				/* the minidebugger is appended */
#ifndef HARD_MEM_SIZE
	ret	r31, (r0)
#endif
.end
