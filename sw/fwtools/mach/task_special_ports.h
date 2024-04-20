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
 *	@(#)$RCSfile: task_special_ports.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:43 $
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
 *	File:	mach/task_special_ports.h
 *
 *	Defines codes for special_purpose task ports.  These are NOT
 *	port identifiers - they are only used for the task_get_special_port
 *	and task_set_special_port routines.
 *	
 */

#ifndef	_MACH_TASK_SPECIAL_PORTS_H_
#define _MACH_TASK_SPECIAL_PORTS_H_

#define TASK_KERNEL_PORT	1	/* Represents task to the outside
					   world.*/
#define TASK_NOTIFY_PORT	2	/* Task receives kernel IPC
					   notifications here. */
#define TASK_EXCEPTION_PORT	3	/* Exception messages for task are
					   sent to this port. */
#define TASK_BOOTSTRAP_PORT	4	/* Bootstrap environment for task. */

/*
 *	Definitions for ease of use
 */

#define task_get_kernel_port(task, port)	\
		(task_get_special_port((task), TASK_KERNEL_PORT, (port)))

#define task_set_kernel_port(task, port)	\
		(task_set_special_port((task), TASK_KERNEL_PORT, (port)))

#define task_get_notify_port(task, port)	\
		(task_get_special_port((task), TASK_NOTIFY_PORT, (port)))

#define task_set_notify_port(task, port)	\
		(task_set_special_port((task), TASK_NOTIFY_PORT, (port)))

#define task_get_exception_port(task, port)	\
		(task_get_special_port((task), TASK_EXCEPTION_PORT, (port)))

#define task_set_exception_port(task, port)	\
		(task_set_special_port((task), TASK_EXCEPTION_PORT, (port)))

#define task_get_bootstrap_port(task, port)	\
		(task_get_special_port((task), TASK_BOOTSTRAP_PORT, (port)))

#define task_set_bootstrap_port(task, port)	\
		(task_set_special_port((task), TASK_BOOTSTRAP_PORT, (port)))

#endif	/*_MACH_TASK_SPECIAL_PORTS_H_*/
