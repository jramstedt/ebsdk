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
 *	@(#)$RCSfile: boolean.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:39 $
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
/*
 *	File:	mach/boolean.h
 *
 *	Boolean data type.
 *
 */

#ifndef	_MACH_BOOLEAN_H_
#define _MACH_BOOLEAN_H_

/*
 *	Pick up "boolean_t" type definition
 */

#ifndef	__ASSEMBLER__
#include <mach/machine/boolean.h>
#endif	/*__ASSEMBLER__*/
#endif	/*_MACH_BOOLEAN_H_*/

/*
 *	Define TRUE and FALSE, only if they haven't been before,
 *	and not if they're explicitly refused.  Note that we're
 *	outside the BOOLEAN_H_ conditional, to avoid ordering
 *	problems.
 */

#if	(defined(KERNEL) || defined(EXPORT_BOOLEAN)) && !defined(NOBOOL)

#ifndef	TRUE
#define TRUE	1
#endif	/*TRUE*/

#ifndef	FALSE
#define FALSE	0
#endif	/*FALSE*/

#endif	/*(defined(KERNEL) || defined(EXPORT_BOOLEAN)) && !defined(NOBOOL)*/
