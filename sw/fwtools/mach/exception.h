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
 *	@(#)$RCSfile: exception.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:39 $
 */ 
/*
 */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */

#ifndef	_MACH_EXCEPTION_H_
#define _MACH_EXCEPTION_H_

#include <mach/machine/exception.h>

/*
 *	Machine-independent exception definitions.
 */

#define EXC_BAD_ACCESS		1L	/* Could not access memory */
		/* Code contains kern_return_t describing error. */
		/* Subcode contains bad memory address. */

#define EXC_BAD_INSTRUCTION	2L	/* Instruction failed */
		/* Illegal or undefined instruction or operand */

#define EXC_ARITHMETIC		3L	/* Arithmetic exception */
		/* Exact nature of exception is in code field */

#define EXC_EMULATION		4L	/* Emulation instruction */
		/* Emulation support instruction encountered */
		/* Details in code and subcode fields	*/

#define EXC_SOFTWARE		5L	/* Software generated exception */
		/* Exact exception is in code field. */
		/* Codes 0 - 0xFFFF reserved to hardware */
		/* Codes 0x10000 - 0x1FFFF reserved for OS emulation (Unix) */

#define EXC_BREAKPOINT		6L	/* Trace, breakpoint, etc. */
		/* Details in code field. */

#define	EXC_AST			7L	/* ast */
		/* excutive mode ast, supervisor mode ast, kernel mode ast */

#endif	/*_MACH_EXCEPTION_H_*/
