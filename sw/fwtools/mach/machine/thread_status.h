/*
 * *****************************************************************
 * *                                                               *
 * *    Copyright (c) Digital Equipment Corporation, 1991, 1996    *
 * *                                                               *
 * *   All Rights Reserved.  Unpublished rights  reserved  under   *
 * *   the copyright laws of the United States.                    *
 * *                                                               *
 * *   The software contained on this media  is  proprietary  to   *
 * *   and  embodies  the  confidential  technology  of  Digital   *
 * *   Equipment Corporation.  Possession, use,  duplication  or   *
 * *   dissemination of the software and media is authorized only  *
 * *   pursuant to a valid written license from Digital Equipment  *
 * *   Corporation.                                                *
 * *                                                               *
 * *   RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure  *
 * *   by the U.S. Government is subject to restrictions  as  set  *
 * *   forth in Subparagraph (c)(1)(ii)  of  DFARS  252.227-7013,  *
 * *   or  in  FAR 52.227-19, as applicable.                       *
 * *                                                               *
 * *****************************************************************
 */
/*
 * HISTORY
 */
/*	
 *	@(#)thread_status.h	9.2	(ULTRIX/OSF)	10/23/91
 */ 
/*
 * *****************************************************************
 * *                                                               *
 * *    Copyright (c) Digital Equipment Corporation, 1991, 1996    *
 * *                                                               *
 * *   All Rights Reserved.  Unpublished rights  reserved  under   *
 * *   the copyright laws of the United States.                    *
 * *                                                               *
 * *   The software contained on this media  is  proprietary  to   *
 * *   and  embodies  the  confidential  technology  of  Digital   *
 * *   Equipment Corporation.  Possession, use,  duplication  or   *
 * *   dissemination of the software and media is authorized only  *
 * *   pursuant to a valid written license from Digital Equipment  *
 * *   Corporation.                                                *
 * *                                                               *
 * *   RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure  *
 * *   by the U.S. Government is subject to restrictions  as  set  *
 * *   forth in Subparagraph (c)(1)(ii)  of  DFARS  252.227-7013,  *
 * *   or  in  FAR 52.227-19, as applicable.                       *
 * *                                                               *
 * *****************************************************************
 */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */

/*
 *	File:	alpha/thread_status.h
 *
 *	This file contains the structure definitions for the thread
 *	state as applied to alpha processors.
 *
 */

#ifndef	_MACH_ALPHA_THREAD_STATE_
#define	_MACH_ALPHA_THREAD_STATE_

/*
 *	Two basic structures are defined:
 *
 *	alpha_thread_state	this is the structure that is exported
 *				to user threads for use in status/mutate
 *				calls.  This structure should never
 *				change.
 *
 *	alpha_saved_state	this structure corresponds to the state
 *				of the user registers as saved on the
 *				stack upon kernel entry.  This structure
 *				is used internally only.  Since this
 *				structure may change from version to
 *				version, it is hidden from the user.
 *
 * 	Other definitions (flavors) cover alternate register sets as
 *	needed.  For instance, for the FP coprocessor.
 */

#define	ALPHA_THREAD_STATE	(1)
#define ALPHA_FLOAT_STATE	(2)

struct alpha_thread_state {
	long	r0;		/* v0	return value 		*/
	long	r1;		/* t0	callee saved 0		*/
	long	r2;		/* t1	callee saved 1		*/
	long	r3;		/* t2	callee saved 2		*/
	long	r4;		/* t3	callee saved 3		*/
	long	r5;		/* t4	callee saved 4		*/
	long	r6;		/* t5	callee saved 5		*/
	long	r7;		/* t6	callee saved 6		*/
	long	r8;		/* t7	callee saved 7		*/
	long	r9;		/* s0	caller saved 0		*/
	long	r10;		/* s1	caller saved 1		*/
	long	r11;		/* s2	caller saved 2		*/
	long	r12;		/* s3	caller saved 3		*/
	long	r13;		/* s4	caller saved 4		*/
	long	r14;		/* s5	caller saved 5		*/
	long	r15;		/* s6/fp caller saved 6		*/
	long	r16;		/* a0	argument register 0	*/
	long	r17;		/* a1	argument register 1	*/
	long	r18;		/* a2	argument register 2	*/
	long	r19;		/* a3	argument register 3	*/
	long	r20;		/* a4	argument register 4	*/
	long	r21;		/* a5	argument register 5	*/
	long	r22;		/* t8	callee saved 8		*/
	long	r23;		/* t9	callee saved 9		*/
	long	r24;		/* t10	callee saved 10		*/
	long	r25;		/* t11	callee saved 11		*/
	long	r26;		/* ra	return address		*/
	long	r27;		/* t12	callee saved 12		*/
	long	r28;		/* at	assembler temp		*/
	long	r29;		/* gp	global pointer		*/
	long	r30;		/* sp	stack pointer		*/
	long	pc;		/* user-mode PC			*/
	long	ps;		/* processor status		*/
};

#define	ALPHA_THREAD_STATE_COUNT	(sizeof(struct alpha_thread_state)/sizeof(long))


struct alpha_float_state {
	long	r0;	/* general floating point registers */
	long	r1;
	long	r2;
	long	r3;
	long	r4;
	long	r5;
	long	r6;
	long	r7;
	long	r8;
	long	r9;
	long	r10;
	long	r11;
	long	r12;
	long	r13;
	long	r14;
	long	r15;
	long	r16;
	long	r17;
	long	r18;
	long	r19;
	long	r20;
	long	r21;
	long	r22;
	long	r23;
	long	r24;
	long	r25;
	long	r26;
	long	r27;
	long	r28;
	long	r29;
	long	r30;
	long	r31;
};

#define	ALPHA_FLOAT_STATE_COUNT	(sizeof(struct alpha_float_state)/sizeof(long))

/*
 *  BEWARE this layout.  The reason for this is two-fold:
 *
 *  1) alpha has a hardware-generated exception frame which 
 *  means the order of saved registers is not the same as
 *  the numeric order.
 *
 *  2) pcb.c actually references some stuff using fields defined
 *  here.  The most troublesome one is in thread_dup(), which makes
 *  certain assumptions about the contents of "r2", "r3", and "r7".
 *
 *  This layout conforms with that defined in dec/machine/alpha/reg.h
 */
struct alpha_saved_state {
	long	r0;		/* v0	return value 		*/
	long	r1;		/* t0	callee saved 0		*/
	long	r2;		/* t1	callee saved 1		*/
	long	r3;		/* t2	callee saved 2		*/
	long	r4;		/* t3	callee saved 3		*/
	long	r5;		/* t4	callee saved 4		*/
	long	r6;		/* t5	callee saved 5		*/
	long	r7;		/* t6	callee saved 6		*/
	long	r8;		/* t7	callee saved 7		*/
	long	r9;		/* s0	caller saved 0		*/
	long	r10;		/* s1	caller saved 1		*/
	long	r11;		/* s2	caller saved 2		*/
	long	r12;		/* s3	caller saved 3		*/
	long	r13;		/* s4	caller saved 4		*/
	long	r14;		/* s5	caller saved 5		*/
	long	r15;		/* s6/fp caller saved 6		*/
	long	r19;		/* a3	argument register 3	*/
	long	r20;		/* a4	argument register 4	*/
	long	r21;		/* a5	argument register 5	*/
	long	r22;		/* t8	callee saved 8		*/
	long	r23;		/* t9	callee saved 9		*/
	long	r24;		/* t10	callee saved 10		*/
	long	r25;		/* t11	callee saved 11		*/
	long	r26;		/* ra	return address		*/
	long	r27;		/* t12	callee saved 12		*/
	long	r28;		/* at	assembler temp		*/
	long	r30;		/* sp	stack pointer		*/
	long	ps;		/* processor status		*/
	long	pc;		/* user-mode PC			*/
	long	r29;		/* gp	global pointer		*/
	long	r16;		/* a0	argument register 0	*/
	long	r17;		/* a1	argument register 1	*/
	long	r18;		/* a2	argument register 2	*/
};

#ifdef	KERNEL
#define	USER_REGS(_th)	(((struct alpha_saved_state *)(&(_th)->stack->pcb)) - 1)
#define	USER_REG(x)		((unsigned long*)USER_REGS(current_thread()))[(x)]

#endif	/* KERNEL */

#endif	/* _MACH_ALPHA_THREAD_STATE_ */
