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
 *	@(#)$RCSfile: vm_prot.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:44 $
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
 *	File:	mach/vm_prot.h
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Copyright (C) 1985, Avadis Tevanian, Jr., Michael Wayne Young
 *
 *	Virtual memory protection definitions.
 *
 */

#ifndef	_MACH_VM_PROT_H_
#define _MACH_VM_PROT_H_

/*
 *	Types defined:
 *
 *	vm_prot_t		VM protection values.
 */

typedef int		vm_prot_t;

/*
 *	Protection values, defined as bits within the vm_prot_t type
 */

#define VM_PROT_NONE	((vm_prot_t) 0x00)

#define VM_PROT_READ	((vm_prot_t) 0x01)	/* read permission */
#define VM_PROT_WRITE	((vm_prot_t) 0x02)	/* write permission */
#define VM_PROT_EXECUTE	((vm_prot_t) 0x04)	/* execute permission */
#define VM_PROT_TRY     ((vm_prot_t) 0x08)      /* used by light wire */
/*
 *	The default protection for newly-created virtual memory
 */

#define VM_PROT_DEFAULT	(VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE)

/*
 *	The maximum privileges possible, for parameter checking.
 */

#define VM_PROT_ALL	(VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE)

#endif	/*_MACH_VM_PROT_H_*/
