
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
 * @(#)$RCSfile: vm_behavior.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:44 $
 */
/*
 *	File:	mach/vm_behavior.h
 *
 *	Virtual memory map behavior definitions.
 *
 */

#ifndef	_MACH_VM_BEHAVIOR_H_
#define _MACH_VM_BEHAVIOR_H_

/*
 *	Types defined:
 *
 *	vm_behavior_t	behavior codes.
 */

typedef int		vm_behavior_t;

/*
 *	Enumeration of valid values for vm_behavior_t.
 *	These describe expected page reference behavior for 
 *	for a given range of virtual memory.  For implementation 
 *	details see vm/vm_fault.c
 */

#define VM_BEHAVIOR_DEFAULT	((vm_behavior_t) 0)	/* default */
#define VM_BEHAVIOR_RANDOM	((vm_behavior_t) 1)	/* random */
#define VM_BEHAVIOR_FSEQNTL	((vm_behavior_t) 2)	/* forward sequential */
#define VM_BEHAVIOR_RSEQNTL	((vm_behavior_t) 3)	/* reverse sequential */

#endif	/*_MACH_VM_BEHAVIOR_H_*/
