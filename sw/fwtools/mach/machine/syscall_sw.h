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
 *	@(#)syscall_sw.h	9.2	(ULTRIX/OSF)	10/19/91
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
 * OSF/1 Release 1.0
 */
/*
 * syscall_sw.h
 *
 *	Modification History:
 *
 * 30-Apr-91	afd
 *	copy the mips version for starters
 *
 */

/* rjlfix: this needs to be re-worked for alpha */

/* 
 * derived from syscall_sw.h	2.1	(ULTRIX/OSF)	12/3/90
 */
#ifndef	_MACH_ALPHA_SYSCALL_SW_H_
#define	_MACH_ALPHA_SYSCALL_SW_H_	1

#include <machine/regdef.h>
#include <machine/asm.h>

/*
 * The Unix kernel expects arguments to be passed with the normal C calling
 * sequence, and v0 should contain the system call number.
 * On Mach we pass all the arguments in registers, the trap number is in v0
 * and the return value is placed in v0.  There are no awful hacks for
 * returning multiple values from a trap.
 *
 */

/*
 * A simple trap is one with up to 6 args. Args are passed to us
 * in registers, and we keep them there.
 */
#define simple_kernel_trap(trap_name, trap_number)	\
	.globl		trap_name; 			\
	.ent		trap_name,0;			\
trap_name:;						\
	.frame		sp,0,ra;			\
	ldiq		v0,trap_number;			\
	CHMK();						\
	RET;						\
	.end		trap_name

#define kernel_trap_0(trap_name,trap_number)		\
	simple_kernel_trap(trap_name,trap_number)
#define kernel_trap_1(trap_name,trap_number)		\
	simple_kernel_trap(trap_name,trap_number)
#define kernel_trap_2(trap_name,trap_number)		\
	simple_kernel_trap(trap_name,trap_number)
#define kernel_trap_3(trap_name,trap_number)		\
	simple_kernel_trap(trap_name,trap_number)
#define kernel_trap_4(trap_name,trap_number)		\
	simple_kernel_trap(trap_name,trap_number)
#define kernel_trap_5(trap_name,trap_number)		\
	simple_kernel_trap(trap_name,trap_number)
#define kernel_trap_6(trap_name,trap_number)		\
	simple_kernel_trap(trap_name,trap_number)


/*
 * There are no Mach traps with more than 6 args.
 * If that changes just add more macros, using
 * registers t0-t7.  And fix the kernel.
 * The following one is for reference.
 */
#define kernel_trap_7(trap_name, trap_number)	 	\
	.globl		trap_name; 			\
	.ent		trap_name,0;			\
trap_name:;						\
	.frame		sp,0,ra;			\
	ldq		t0,32(sp);			\
	ldiq		v0,trap_number;			\
	CHMK();						\
	RET;						\
	.end		trap_name

#ifdef	__STDC__
#define kernel_trap(trap_name,trap_number,nargs)	\
	kernel_trap_##nargs(trap_name,trap_number)
#else
#define kernel_trap(trap_name,trap_number,nargs)	\
	kernel_trap_/**/nargs(trap_name,trap_number)
#endif	/* __STDC__ */

#endif	/* _MACH_ALPHA_SYSCALL_SW_H_ */
