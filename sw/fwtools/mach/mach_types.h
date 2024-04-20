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
 *	@(#)$RCSfile: mach_types.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:41 $
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
 *	File:	mach/mach_types.h
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Copyright (C) 1986, Avadis Tevanian, Jr., Michael Wayne Young
 *
 *	Mach external interface definitions.
 *
 */

#ifndef	_MACH_MACH_TYPES_H_
#define _MACH_MACH_TYPES_H_

#include <mach/port.h>
#include <mach/vm_inherit.h>
#include <mach/vm_behavior.h>
#include <mach/vm_prot.h>
#include <mach/memory_object.h>
#include <mach/vm_statistics.h>
#include <mach/vm_attributes.h>
#include <mach/machine/vm_types.h>
#include <mach/machine.h>
#include <mach/thread_status.h>
#include <mach/thread_info.h>
#include <mach/thread_special_ports.h>
#include <mach/task_info.h>
#include <mach/task_special_ports.h>

#include <mach/host_info.h>
#include <mach/processor_info.h>

#ifdef	KERNEL
#include <kern/kern_mon.h>
#include <kern/task.h>
#include <kern/thread.h>

#include <vm/vm_user.h>
#include <vm/vm_object.h>

#include <kern/host.h>
#include <kern/processor.h>
#else	/*KERNEL*/
typedef	port_t		task_t;
typedef	task_t		vm_task_t;
typedef port_t		*task_array_t;
typedef	port_t		thread_t;
typedef port_t		monitor_t;
typedef	thread_t	*thread_array_t;

typedef port_t		host_t;
typedef port_t		host_priv_t;
typedef port_t		processor_t;
typedef port_t		processor_name_t;
typedef port_t		*processor_array_t;
typedef port_t		processor_set_t;
typedef port_t		processor_set_name_t;
typedef port_t		*processor_set_array_t;
typedef port_t		*processor_set_name_array_t;
#endif	/*KERNEL*/

/*
 *	Backwards compatibility, for those programs written
 *	before mach/{std,mach}_types.{defs,h} were set up.
 */
#include <mach/std_types.h>


typedef	vm_offset_t	vm_address_t;
typedef	unsigned int	vm_region_t;
typedef	vm_region_t	*vm_region_array_t;

typedef	char		vm_page_data_t[4096];


#endif	/*_MACH_MACH_TYPES_H_*/
