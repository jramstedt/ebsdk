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
 *	@(#)$RCSfile: vm_param.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:44 $
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
 *	File:	mach/vm_param.h
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Copyright (C) 1985, Avadis Tevanian, Jr., Michael Wayne Young
 *
 *	Machine independent virtual memory parameters.
 *
 */

#ifndef	_MACH_VM_PARAM_H_
#define _MACH_VM_PARAM_H_

#ifndef	KERNEL

;YOU HAVE MADE A MISTAKE BY INCLUDING THIS FILE;

THIS FILE SHOULD NOT BE VISIBLE TO USER PROGRAMS.

USE <mach/machine/vm_param.h> TO GET MACHINE-DEPENDENT ADDRESS
SPACE AND PAGE SIZE ITEMS.

USE <mach/machine/vm_types.h> TO GET TYPE DECLARATIONS USED IN
THE MACH KERNEL INTERFACE.

IN ALL PROBABILITY, YOU SHOULD GET ALL OF THE TYPES USED IN THE
INTERFACE FROM <mach/mach_types.h>

#endif	/*!KERNEL*/

#include <mach/machine/vm_param.h>
#include <mach/machine/vm_types.h>

/*
 *	The machine independent pages are refered to as PAGES.  A page
 *	is some number of hardware pages, depending on the target machine.
 */

/*
 *	All references to the size of a page should be done with PAGE_SIZE
 *	or PAGE_SHIFT.  The fact they are variables is hidden here so that
 *	we can easily make them constant if we so desire.
 */

/*
 *	Regardless whether it is implemented with a constant or a variable,
 *	the PAGE_SIZE is assumed to be a power of two throughout the
 *	virtual memory system implementation.
 */

#ifdef	PAGE_SIZE_FIXED

#define page_size	PAGE_SIZE	/* defined in machine/vm_param.h */
#define page_mask	PAGE_MASK	/* defined in machine/vm_param.h */
#define page_shift	PAGE_SHIFT	/* defined in machine/vm_param.h */

#else	/*PAGE_SIZE_FIXED*/

#define PAGE_SIZE	page_size	/* size of page in addressible units */
#define PAGE_MASK	page_mask	/* mask of bits for offset in a page */
#define PAGE_SHIFT	page_shift	/* number of bits to shift for pages */
extern vm_size_t	page_size;
extern vm_size_t	page_mask;
extern int		page_shift;

#endif	/*PAGE_SIZE_FIXED*/

#ifndef	__ASSEMBLER__
/*
 *	Convert addresses to pages and vice versa.
 *	No rounding is used.
 */

#define atop(x)		(((vm_offset_t)(x)) >> PAGE_SHIFT)
#define ptoa(x)		(((vm_offset_t)(x)) << PAGE_SHIFT)

/*
 *	Round off or truncate to the nearest page.  These will work
 *	for either addresses or counts.  (i.e. 1 byte rounds to 1 page
 *	bytes.
 */

#define round_page(x)	((vm_offset_t)((((vm_offset_t)(x)) + PAGE_MASK) & \
								~PAGE_MASK))
#define trunc_page(x)	((vm_offset_t)(((vm_offset_t)(x)) & ~PAGE_MASK))

/*
 *	Determine whether an address is page-aligned, or a count is
 *	an exact page multiple.
 */

#define	page_aligned(x)	((((vm_offset_t)(x)) & PAGE_MASK) == 0)

extern vm_size_t	mem_size;	/* size of physical memory (bytes) */
extern vm_offset_t	first_addr;	/* first physical page */
extern vm_offset_t	last_addr;	/* last physical page */

#endif	/*__ASSEMBLER__*/
#endif	/*_MACH_VM_PARAM_H_*/
