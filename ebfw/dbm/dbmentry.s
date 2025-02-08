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
.data
#ifdef __linux__
.asciz "$Id: dbmentry.s,v 1.3 1999/01/22 15:42:46 bissen Exp $"
#else
.asciiz "$Id: dbmentry.s,v 1.3 1999/01/22 15:42:46 bissen Exp $"
#endif
.text
#endif

/*
 * $Log: dbmentry.s,v $
 * Revision 1.3  1999/01/22 15:42:46  bissen
 * Use the values in $(TARGET).h to determine where the default
 * location for the rom image should be.
 *
 * Revision 1.2  1999/01/21 20:08:27  bissen
 * Force argc(a0) to 0 it CNS__PLACE_HOLDER18 is not defined.
 *
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.27  1998/08/26  18:46:30  thulin
 * Add support for CLUBS
 *
 * Revision 1.26  1998/08/03  17:16:29  gries
 * changes for new signature
 *
 * Revision 1.25  1997/12/15  20:52:48  pbell
 * Updated for dp264.
 *
 * Revision 1.25  1997/11/19  19:56:57  gries
 * added dual code
 *
 * Revision 1.24  1997/08/12  04:59:49  fdh
 * Include paldata.h instead of inpure.h.
 *
 * Revision 1.23  1997/06/02  04:22:33  fdh
 * Added call to EstablishInitData() to initialize data
 * that require more than just being zeroed.
 *
 * Revision 1.22  1997/04/10  17:21:03  fdh
 * Added the .asciz directive for linux.
 * Removed punctuation from comment.
 * The GNU assembler coughed on it.
 *
 * Revision 1.21  1997/03/17 13:48:22  fdh
 * Fixed up load for MINIMUM_SYSTEM_MEMORY.
 *
 * Revision 1.20  1997/03/17  09:45:11  fdh
 * Included "palosf.h" and "palcsrv.h"
 *
 * Revision 1.19  1995/10/19  18:16:10  fdh
 * Changed point at which global variable containing
 * the initial stack pointer is initialized.
 *
 * Revision 1.18  1995/09/02  03:37:43  fdh
 * Save initial stack pointer into a global variable.
 *
 * Revision 1.17  1995/08/30  23:19:36  fdh
 * Call_pal HALT if the Debug Monitor exits.
 * Setup pv register before calling functions.
 *
 * Revision 1.16  1995/08/25  20:13:23  fdh
 * Pass process mode (User/DBM) argument into initdata.
 *
 * Revision 1.15  1995/05/19  20:37:16  fdh
 * Save away PALcode Halt Code.
 *
 * Revision 1.14  1995/02/24  16:08:18  fdh
 * Modified to use the regdefs.h and paldefs.h include files.
 *
 * Revision 1.13  1994/11/28  19:40:45  fdh
 * Added writes to LED port.
 *
 * Revision 1.12  1994/11/24  04:17:41  fdh
 * Exclude PALcode files for make depend.
 *
 * Revision 1.11  1994/08/05  20:18:01  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.10  1994/07/13  14:28:30  fdh
 * Removed initialization of PalImpureBase.  Moved to doinitdata().
 *
 * Revision 1.9  1994/06/20  18:01:45  fdh
 * Modified to use new ANSI compliant PALcode definitions.
 *
 * Revision 1.8  1994/04/06  04:22:38  fdh
 * Enable Floating Point.
 * Modified for updated cserve.h file.
 *
 * Revision 1.7  1994/04/01  13:54:51  fdh
 * Default initial stack pointer based on the
 * system specific MINIMUM_SYSTEM_MEMORY definition.
 *
 * Revision 1.6  1994/03/16  15:46:21  berent
 * Alligned pointer to  impure area signiture after reading to avoid
 * trapping on very corrupt  impure areas.
 *
 * Revision 1.5  1994/03/14  20:47:21  fdh
 * Recover lost revision 1.2 edit.
 * Use default stack location when memory size is unknown.
 * More clean-up.
 *
 * Revision 1.4  1994/03/14  15:53:49  rusling
 * Fixed up unaligned data access on PalImpureBase.
 *
 * Revision 1.3  1994/03/14  14:24:19  rusling
 * Fixed up some WNT problems.
 *
 * Revision 1.2  1994/03/11  20:24:08  fdh
 * Load proper offset to PALcode Impure Scratch Area.
 *
 * Revision 1.1  1994/03/11  03:03:23  fdh
 * Initial revision
 *
 */

/*
 * Debug Monitor Entry code
 */

#include "palosf.h"
#include "palcsrv.h"
#include "paldefs.h"
#include "regdefs.h"
#include "system.h"
#include "ledcodes.h"
#include "paldata.h"

#ifndef CNS__PLACE_HOLDER18
#define DualOff
#endif

#ifdef __GNUC__
	.section .init
#else
	.text
#endif

STARTFRM = 16           # return address and padding to octaword align

        .globl  __start
        .ent    __start, 0
__start:

#if (defined(DP264) || defined(CLUBS))	
	ldah	r16, 0x8(r31)
	lda	r16, 0x01fc(r16)
	sll	r16, 24, r16
	lda	r17, -1(r31)
	sll	r17, 47, r17
	bis	r16, r17, r16
	bis	r16, 0x80, r16
	lda	r17, 0xBb(r31)
	stb	r17, 0(r16)
#endif /* DP264 || CLUBS*/

	br      t1, 2f			# get the current PC
2:	ldgp    gp, 0(t1)               # init gp

	lda	t1, halt_code
	stl	t0, 0(t1)		    # Save away Halt code.

	lda	a2, CSERVE_K_RD_IMPURE
	call_pal PAL_CSERVE_ENTRY
	lda	v0,  CNS_Q_BASE(v0)
        mb
#if (defined(DP264) || defined(CLUBS))	
	ldah	r16, 0x8(r31)
	lda	r16, 0x01fc(r16)
	sll	r16, 24, r16
	lda	r17, -1(r31)
	sll	r17, 47, r17
	bis	r16, r17, r16
	bis	r16, 0x80, r16
	lda	r17, 0xB0(r31)
	stb	r17, 0(r16)
#endif /* DP264 || CLUBS*/

#ifndef DualOff
	br	t0, GetCpuNumber	
        .long   0
        .long   0
        .long   0
        .long   0
GetCpuNumber:
/*	bic 	t0, 0x07, t0		# Clear bottom 3 bits to avoid */
        lda     t1, 1000(r31)           // lda 100 to t1
GetCpuNumberwait:
        subq    t1, 1, t1
        bne     t1, GetCpuNumberwait
        mb
        ldl_l 	t1, 0(t0)           // See if we're the first CPU here
        addl    t1, 1, t2            // increament processor id
        stl_c 	t2, 0(t0)           //  and prevent others from doing i`
        beq     t2, GetCpuNumber
        
        bne     t1, Not_primary
	stq	t0, CNS__PLACE_HOLDER18(v0) 
	lda	t10, CNS_Q_PROC_MASK(v0)
	ldq	t10, 0x00(t10)
	srl	t10, 1, t1
	and	t10, 1, t10
	and	t1, 1, t1
	addq	t1, t10, t10
	lda	t12, 10000(r31)
wait_for_checkin:
        subq    t12, 1, t12
        beq     t12, timedout
        lda     t1, 100(r31)           // lda 100 to t1
checkinwait:
        subq    t1, 1, t1
        bne     t1, checkinwait
        mb
        ldl	t1, 0(t0)           // See if we're the first CPU here
        subq    t10, t1, t11
        bne     t11, wait_for_checkin
timedout:
#endif
	bis	zero, v0, s0
	lda	t0, CNS_Q_SIGNATURE(v0)
	bic 	t0, 0x07, t0		# Clear bottom 3 bits to avoid
					# allignment errors if the
					# impure area is total rubbish
	ldq	t0, 0x00(t0)
	srl	t0, 20, t0		# Shift signature into bottom 16 bits.
	lda	t6, 0xDEC(zero)		# Load the expected valid signature.
	zap	t6, 0xFC, t6		# Clear the upper bits.
	cmpeq	t0, t6, t0		# Is this a valid signature?
	beq	t0, 1f			# Not valid, dont trust input params.

/*
 *	Init the stack at the first 8K boundary
 *	below the top of memory.
 */
	lda	t0, CNS_Q_MEM_SIZE(v0)
	ldq	t0, 0x00(t0)		# Load memory size.
	subq	t0, 1, t0		# Last address in memory
	srl	t0, 13, t0		# Align to first 8KB boundary
	sll	t0, 13, sp		# below the top of memory.
	br	zero, 2f

/*
 *	If memory size was not passed in via the
 *	PALcode impure data use the system specific
 *	MINIMUM_SYSTEM_MEMORY definition.
 */
1:
	lda	sp, MINIMUM_SYSTEM_MEMORY
	lda	t0, (8*1024)(zero)	# Allow for 8KB guard page.
	subq	sp, t0, sp

2:	/* Stack pointer is now established */
	lda	t0, InitialStackPointer
	stq	sp, 0x00(t0)
	lda     sp, -STARTFRM(sp)	# Create a stack frame 
	stq     ra, 0(sp)		# Place return address on the stack
	.mask   0x84000000, -8
	.frame  sp, STARTFRM, ra


#if (defined(DP264) || defined(CLUBS))	
	ldah	r16, 0x8(r31)
	lda	r16, 0x01fc(r16)
	sll	r16, 24, r16
	lda	r17, -1(r31)
	sll	r17, 47, r17
	bis	r16, r17, r16
	bis	r16, 0x80, r16
	lda	r17, 0xB1(r31)
	stb	r17, 0(r16)
#endif /* DP264 || CLUBS*/

	lda	a0, led_k_ksp_initialized
	lda	pv, outLed
	jsr	ra, (pv)
	ldgp    gp, 0(ra)               # restore gp
#if (defined(DP264) || defined(CLUBS))	
	ldah	r16, 0x8(r31)
	lda	r16, 0x01fc(r16)
	sll	r16, 24, r16
	lda	r17, -1(r31)
	sll	r17, 47, r17
	bis	r16, r17, r16
	bis	r16, 0x80, r16
	lda	r17, 0xB2(r31)
	stb	r17, 0(r16)
#endif /* DP264 || CLUBS*/



/*
 *	Initialize data that require more than just being zeroed.
 */
	lda	pv, EstablishInitData
	jsr	ra, (pv)
	ldgp    gp, 0(ra)               # restore gp


/*
 *	Zero the un-initialized data region in memory
 */
	lda	a0, 0			# 0=DBM Mode, 1=User Mode
	lda	pv, initdata
	jsr	ra, (pv)
	ldgp    gp, 0(ra)               # restore gp


	lda	a0, led_k_sysdata_inited
	lda	pv, outLed
	jsr	ra, (pv)
	ldgp    gp, 0(ra)               # restore gp


/*
 *	Enable the Floating Point Unit
 */
#if (defined(DP264) || defined(CLUBS))	
	ldah	r16, 0x8(r31)
	lda	r16, 0x01fc(r16)
	sll	r16, 24, r16
	lda	r17, -1(r31)
	sll	r17, 47, r17
	bis	r16, r17, r16
	bis	r16, 0x80, r16
	lda	r17, 0xB3(r31)
	stb	r17, 0(r16)
#endif /* DP264 || CLUBS*/

	lda	a0, 1(zero)
	call_pal PAL_WRFEN_ENTRY
#if (defined(DP264) || defined(CLUBS))	
	ldah	r16, 0x8(r31)
	lda	r16, 0x01fc(r16)
	sll	r16, 24, r16
	lda	r17, -1(r31)
	sll	r17, 47, r17
	bis	r16, r17, r16
	bis	r16, 0x80, r16
	lda	r17, 0xB6(r31)
	stb	r17, 0(r16)
#endif /* DP264 || CLUBS*/



/*
 *	Every good C program has a main()
 */
	bis	zero, s0, a1
#ifdef CNS__PLACE_HOLDER18
	bis	zero, 1, a0
#else
	bis	zero, zero, a0
#endif
	lda	pv, main
	jsr	ra, (pv)


/*
 *	The Debug Monitor should never return.
 *	However, just incase...
 */
	call_pal PAL_HALT_ENTRY
Not_primary:
	lda	t1, 0x0800(zero)
wait_a_long_time:
	mb
	subq	t1, 1, t1
	bgt	t1, wait_a_long_time
#if (defined(DP264) || defined(CLUBS))	
	ldah	r16, 0x8(r31)
	lda	r16, 0x01fc(r16)
	sll	r16, 24, r16
	lda	r17, -1(r31)
	sll	r17, 47, r17
	bis	r16, r17, r16
	bis	r16, 0x80, r16
	lda	r17, 0xB1(r31)
	stb	r17, 0(r16)
#endif /* DP264 || CLUBS*/

        ldl	t2, 0(t0)
        bne     t2, Not_primary
	addq	t0, 8, t0
	bic 	t0, 0x07, t0		# Clear bottom 3 bits to avoid 
	ldq	sp, 0(t0)
	bis	zero, v0, a0
	lda	pv, Waitforever
	jsr	ra, (pv)
        

.end	__start

#ifdef __GNUC__
.section .fini
#endif

        .globl  _exit
        .ent    _exit, 0
_exit:
	call_pal PAL_HALT_ENTRY
.end	_exit
