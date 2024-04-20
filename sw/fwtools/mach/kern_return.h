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
 *	@(#)$RCSfile: kern_return.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:40 $
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
 * OSF/1 Release 1.0
 */
/*
 *	File:	h/kern_return.h
 *	Author:	Avadis Tevanian, Jr.
 *	Copyright (C) 1985, Avadis Tevanian, Jr.
 *
 *	Kernel return codes.
 *
 */

#ifndef	_MACH_KERN_RETURN_H_
#define _MACH_KERN_RETURN_H_

#include <mach/machine/kern_return.h>

#define KERN_SUCCESS			0

#define KERN_INVALID_ADDRESS		1
		/* Specified address is not currently valid.
		 */

#define KERN_PROTECTION_FAILURE		2
		/* Specified memory is valid, but does not permit the
		 * required forms of access.
		 */

#define KERN_NO_SPACE			3
		/* The address range specified is already in use, or
		 * no address range of the size specified could be
		 * found.
		 */

#define KERN_INVALID_ARGUMENT		4
		/* The function requested was not applicable to this
		 * type of argument, or an argument
		 */

#define KERN_FAILURE			5
		/* The function could not be performed.  A catch-all.
		 */

#define KERN_RESOURCE_SHORTAGE		6
		/* A system resource could not be allocated to fulfill
		 * this request.  This failure may not be permanent.
		 */

#define KERN_NOT_RECEIVER		7
		/* The task in question does not hold receive rights
		 * for the port argument.
		 */

#define KERN_NO_ACCESS			8
		/* Bogus access restriction.
		 */

#define KERN_MEMORY_FAILURE		9
		/* During a page fault, the target address refers to a
		 * memory object that has been destroyed.  This
		 * failure is permanent.
		 */

#define KERN_MEMORY_ERROR		10
		/* During a page fault, the memory object indicated
		 * that the data could not be returned.  This failure
		 * may be temporary; future attempts to access this
		 * same data may succeed, as defined by the memory
		 * object.
		 */

#define KERN_ALREADY_IN_SET		11
		/* The port argument is already a member of a set.
		 */

#define KERN_NOT_IN_SET			12
		/* The port argument is not a member of a set.
		 */

#define KERN_NAME_EXISTS		13
		/* The task already has a translation for the name.
		 */

#define KERN_ABORTED			14
		/* The operation was aborted.  Ipc code will
		 * catch this and reflect it as a message error.
		 */

#define	KERN_PAGELOCKED			15
		/*
		 * An unmap operaton encountered a page locked by
		 * the kernel.  
		 */

#define	KERN_SEGHOLE			16
		/*
		 * An unmap operation removed a hole in a segment.
		 */

#define KERN_LIMIT_EXCEEDED             17
                /*
		 * The operation would exceed some system limit.
                 */

#define	KERN_MAPENTRIES_LIMIT		18
		/*
		 * number of map entries in a user address space has
		 * reached its limit.
		 */


/* NOTE: return values in the range 200-299 are reserved for RM and defined
 * here if they are referenced in base VM code and in rm/rm_support.h
 * if referenced only optional RM code.
 */

#define KERN_RM_ENTRY_CONTAINS           250
#define KERN_RM_BREAK_UP		 251
#define KERN_RM_UNMAP_OVERLAP		 252
#define KERN_RM_NOT_SUPPORTED	         253

#endif	/* !_MACH_KERN_RETURN_H_ */
