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
 *	@(#)$RCSfile: memory_object.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:41 $
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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 *	File:	memory_object.h
 *	Author:	Michael Wayne Young
 *
 *	External memory management interface definition.
 */

#ifndef	_MACH_MEMORY_OBJECT_H_
#define _MACH_MEMORY_OBJECT_H_

/*
 *	User-visible types used in the external memory
 *	management interface:
 */

#include <mach/port.h>

typedef	port_t		memory_object_t;
					/* Represents a memory object ... */
					/*  Used by user programs to specify */
					/*  the object to map; used by the */
					/*  kernel to retrieve or store data */

typedef	port_t		memory_object_control_t;
					/* Provided to a memory manager; ... */
					/*  used to control a memory object */

typedef	port_t		memory_object_name_t;
					/* Used to describe the memory ... */
					/*  object in vm_regions() calls */

typedef	int		memory_object_copy_strategy_t;
					/* How memory manager handles copy: */
#define		MEMORY_OBJECT_COPY_NONE		0
					/* ... No special support */
					/*	1 used to be copy_call */
#define		MEMORY_OBJECT_COPY_DELAY 	2
					/* ... Memory manager doesn't ... */
					/*     change data externally. */


#define		MEMORY_OBJECT_NULL	PORT_NULL

/* Obsolete forms of the above names: */
#ifndef	KERNEL
typedef	port_t		paging_object_t;
typedef	port_t		vm_pager_request_t;
typedef	port_t		vm_pager_t;
#define vm_pager_null	((vm_pager_t) 0)
#endif	/*KERNEL*/

#endif	/*_MACH_MEMORY_OBJECT_H_*/
