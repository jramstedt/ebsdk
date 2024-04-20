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
 *	@(#)$RCSfile: task_info.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:42 $
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
 *	Machine-independent task information structures and definitions.
 *
 *	The definitions in this file are exported to the user.  The kernel
 *	will translate its internal data structures to these structures
 *	as appropriate.
 *
 */

#ifndef	_MACH_TASK_INFO_H_
#define _MACH_TASK_INFO_H_

#include <mach/machine/vm_types.h>
#include <mach/time_value.h>

/*
 *	Generic information structure to allow for expansion.
 */
#ifdef	__alpha
typedef	long	*task_info_t;		/* varying array of long */
#else
typedef	int	*task_info_t;		/* varying array of int */
#endif

#define TASK_INFO_MAX	(1024)		/* maximum array size */
#ifdef	__alpha
typedef	long	task_info_data_t[TASK_INFO_MAX];
#else
typedef	int	task_info_data_t[TASK_INFO_MAX];
#endif

/*
 *	Currently defined information structures.
 */
#define TASK_BASIC_INFO		1	/* basic information */

struct task_basic_info {
	int		suspend_count;	/* suspend count for task */
	int		base_priority;	/* base scheduling priority */
	vm_size_t	virtual_size;	/* number of virtual pages */
	vm_size_t	resident_size;	/* number of resident pages */
	time_value_t	user_time;	/* total user run time for
					   terminated threads */
	time_value_t	system_time;	/* total system run time for
					   terminated threads */
};

typedef struct task_basic_info		task_basic_info_data_t;
typedef struct task_basic_info		*task_basic_info_t;
#ifdef	__alpha
#define TASK_BASIC_INFO_COUNT	\
	((sizeof(task_basic_info_data_t) + sizeof(long) - 1) / sizeof(long))
#else
#define TASK_BASIC_INFO_COUNT	\
		(sizeof(task_basic_info_data_t) / sizeof(int))
#endif


#define TASK_EVENTS_INFO	2	/* various event counts */

struct task_events_info {
	long		faults;		/* number of page faults */
	long		zero_fills;	/* number of zero fill pages */
	long		reactivations;	/* number of reactivated pages */
	long		pageins;	/* number of actual pageins */
	long		cow_faults;	/* number of copy-on-write faults */
	long		messages_sent;	/* number of messages sent */
	long		messages_received; /* number of messages received */
};

#include <mach/events_info.h>

/* TASK_EVENTS_INFO returns data on all deceased threads in the task */
#define TASK_EVENTS_INFO	2	/* various event counts */

/* TASK_ALL_EVENTS_INFO returns data on all threads (active and dead) */
#define TASK_ALL_EVENTS_INFO	3	/* various event counts */

typedef events_info_data_t		task_events_info_data_t;
typedef events_info_data_t		*task_events_info_t;

#ifdef	__alpha
#define TASK_EVENTS_INFO_COUNT	\
	((sizeof(task_events_info_data_t) + sizeof(long) - 1) / sizeof(long))
#else
#define TASK_EVENTS_INFO_COUNT	\
	(sizeof(task_events_info_data_t) / sizeof(int))
#endif 
#endif	/* _MACH_TASK_INFO_H_ */
