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
 *	@(#)$RCSfile: vm_inherit.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:44 $
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
 *	File:	mach/vm_inherit.h
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Copyright (C) 1985, Avadis Tevanian, Jr., Michael Wayne Young
 *
 *	Virtual memory map inheritance definitions.
 *
 */

#ifndef	_MACH_VM_INHERIT_H_
#define _MACH_VM_INHERIT_H_

/*
 *	Types defined:
 *
 *	vm_inherit_t	inheritance codes.
 */

typedef int		vm_inherit_t;	/* might want to change this */

/*
 *	Enumeration of valid values for vm_inherit_t.
 */

#define VM_INHERIT_SHARE	((vm_inherit_t) 0)	/* share with child */
#define VM_INHERIT_COPY		((vm_inherit_t) 1)	/* copy into child */
#define VM_INHERIT_NONE		((vm_inherit_t) 2)	/* absent from child */
#define VM_INHERIT_DONATE_COPY	((vm_inherit_t) 3)	/* copy and delete */

#define VM_INHERIT_DEFAULT	VM_INHERIT_COPY

#endif	/*_MACH_VM_INHERIT_H_*/
