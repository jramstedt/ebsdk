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
 *	@(#)$RCSfile: thread_info.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:43 $
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
 *	File:	mach/thread_info
 *
 *	Thread information structure and definitions.
 *
 *	The defintions in this file are exported to the user.  The kernel
 *	will translate its internal data structures to these structures
 *	as appropriate.
 *
 */

#ifndef	_MACH_THREAD_INFO_H_
#define _MACH_THREAD_INFO_H_

#include <mach/policy.h>
#include <mach/time_value.h>
#include <sys/types.h>

/*
 *	Generic information structure to allow for expansion.
 */
#ifdef	__alpha
typedef	long	*thread_info_t;		/* varying array of long */
#else
typedef	int	*thread_info_t;		/* varying array of int */
#endif

#define THREAD_INFO_MAX		(1024)	/* maximum array size */
#ifdef	__alpha
typedef	long	thread_info_data_t[THREAD_INFO_MAX];
#else
typedef	int	thread_info_data_t[THREAD_INFO_MAX];
#endif

/*
 *	Currently defined information.
 */
#define THREAD_BASIC_INFO	1		/* basic information */

struct thread_basic_info {
	time_value_t	user_time;	/* user run time */
	time_value_t	system_time;	/* system run time */
	int		cpu_usage;	/* scaled cpu usage percentage */
	int		base_priority;	/* base scheduling priority */
	int		cur_priority;	/* current scheduling priority */
	int		run_state;	/* run state (see below) */
	int		flags;		/* various flags (see below) */
	int		suspend_count;	/* suspend count for thread */
	long		sleep_time;	/* number of seconds that thread
					   has been sleeping */
        long            wait_event;     /* event thread is waiting on */
#define WMESGLEN        7
        char            wait_mesg[WMESGLEN+1]; /* wait event message */
};

typedef struct thread_basic_info	thread_basic_info_data_t;
typedef struct thread_basic_info	*thread_basic_info_t;
#ifdef	__alpha				/* round-up */
#define THREAD_BASIC_INFO_COUNT	\
	((sizeof(thread_basic_info_data_t) + sizeof(long) - 1) / sizeof(long))
#else
#define THREAD_BASIC_INFO_COUNT	\
		(sizeof(thread_basic_info_data_t) / sizeof(int))
#endif

/*
 *	Scale factor for usage field.
 */

#define TH_USAGE_SCALE	1000

/*
 *	Thread run states (state field).
 */

#define TH_STATE_RUNNING	1	/* thread is running normally */
#define TH_STATE_STOPPED	2	/* thread is stopped */
#define TH_STATE_WAITING	3	/* thread is waiting normally */
#define TH_STATE_UNINTERRUPTIBLE 4	/* thread is in an uninterruptible
					   wait */
#define TH_STATE_HALTED		5	/* thread is halted at a
					   clean point */

/*
 *	Thread flags (flags field).
 */
#define TH_FLAGS_SWAPPED	0x1	/* thread is swapped out */
#define TH_FLAGS_IDLE		0x2	/* thread is an idle thread */

#define THREAD_SCHED_INFO	2

#define TH_BOUND_PROCESSOR	0x1
#define TH_BOUND_PSET		0x2

struct thread_sched_info {
	int		policy;		/* scheduling policy */
	int		data;		/* associated data */
	int		base_priority;	/* base priority */
	int		max_priority;   /* max priority */
	int		cur_priority;	/* current priority */
	boolean_t	depressed;	/* depressed ? */
	int		depress_priority; /* priority depressed from */
	int		cpu;		/* current cpu # */
	int		bound;		/* bound to cpu/pset ? */
	int		pset;		/* pset number */
};

typedef struct thread_sched_info	thread_sched_info_data_t;
typedef struct thread_sched_info	*thread_sched_info_t;
#ifdef	__alpha				/* round-up */
#define	THREAD_SCHED_INFO_COUNT	\
	((sizeof(thread_sched_info_data_t) + sizeof(long) - 1) / sizeof(long))
#else
#define	THREAD_SCHED_INFO_COUNT	\
		(sizeof(thread_sched_info_data_t) / sizeof(int))
#endif

#include <mach/events_info.h>
#define THREAD_EVENTS_INFO      3

typedef events_info_data_t              thread_events_info_data_t;
typedef events_info_data_t              *thread_events_info_t;

#ifdef	__alpha				/* round-up */
#define THREAD_EVENTS_INFO_COUNT        \
                ((sizeof(thread_events_info_data_t) + sizeof(long) - 1) / sizeof(long))
#else
#define THREAD_EVENTS_INFO_COUNT        \
                (sizeof(thread_events_info_data_t) / sizeof(int))
#endif

/*
 *	Currently defined information.
 */
#define THREAD_SIGNAL_INFO	4		/* signal information */

struct thread_signal_info {
	int		cursig;		/* current signal */
	sigset_t	sigpending;	/* pending signals */
	sigset_t	sigmask;	/* blocked signals */
};

typedef struct thread_signal_info	thread_signal_info_data_t;
typedef struct thread_signal_info	*thread_signal_info_t;
#define THREAD_SIGNAL_INFO_COUNT	\
	((sizeof(thread_signal_info_data_t) + sizeof(long) - 1) / sizeof(long))
#else

#endif	/* _MACH_THREAD_INFO_H_ */
