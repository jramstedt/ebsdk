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
 *	@(#)$RCSfile: host_info.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:39 $
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
 *	File:	mach/host_info.h
 *
 *	Definitions for host_info call.
 */

#ifndef	_MACH_HOST_INFO_H_
#define	_MACH_HOST_INFO_H_

#include <mach/machine.h>
#include <mach/machine/vm_types.h>
#include <mach/time_value.h>

/*
 *	Generic information structure to allow for expansion.
 */
#ifdef	__alpha
typedef long	*host_info_t;		/* varying array of long. */
#else
typedef int	*host_info_t;		/* varying array of int. */
#endif

#define	HOST_INFO_MAX	(1024)		/* max array size */
#ifdef	__alpha
typedef long	host_info_data_t[HOST_INFO_MAX];
#else
typedef int	host_info_data_t[HOST_INFO_MAX];
#endif

#define KERNEL_VERSION_MAX (512)
typedef char	kernel_version_t[KERNEL_VERSION_MAX];
/*
 *	Currently defined information.
 */
#define HOST_BASIC_INFO		1	/* basic info */
#define HOST_PROCESSOR_SLOTS	2	/* processor slot numbers */
#define HOST_SCHED_INFO		3	/* scheduling info */
#define	HOST_PAGING_INFO	4	/* VM/paging info */

struct host_basic_info {
	int		max_cpus;	/* max number of cpus possible */
	int		avail_cpus;	/* number of cpus now available */
	vm_size_t	memory_size;	/* size of memory in bytes */
	cpu_type_t	cpu_type;	/* cpu type */
	cpu_subtype_t	cpu_subtype;	/* cpu subtype */
};

typedef	struct host_basic_info	host_basic_info_data_t;
typedef struct host_basic_info	*host_basic_info_t;
#ifdef	__alpha
#define HOST_BASIC_INFO_COUNT \
	((sizeof(host_basic_info_data_t) + sizeof(long) - 1) / sizeof(long))
#else
#define HOST_BASIC_INFO_COUNT \
		(sizeof(host_basic_info_data_t)/sizeof(int))
#endif

struct host_sched_info {
	int		min_timeout;	/* minimum timeout in milliseconds */
	int		min_quantum;	/* minimum quantum in milliseconds */
};

typedef	struct host_sched_info	host_sched_info_data_t;
typedef struct host_sched_info	*host_sched_info_t;
#ifdef	__alpha
#define HOST_SCHED_INFO_COUNT \
		((sizeof(host_sched_info_data_t) + sizeof(long) - 1) / sizeof(long))
#else
#define HOST_SCHED_INFO_COUNT \
		(sizeof(host_sched_info_data_t)/sizeof(int))
#endif
struct host_paging_info {
	time_value_t	sample_time;	/* (TOD) time sample taken */
	time_value_t	reset_time;	/* (TOD) time at last reset */

			/* Information about page queues */
	long	pagesize;		/* page size in bytes */
	long	free_count;		/* # of pages free */
	long	active_count;		/* # of pages active */
	long	inactive_count;		/* # of pages inactive */
	long	wire_count;		/* # of pages wired down */

			/* Information about page faults */
	long	faults;			/* # of faults */
	long	zero_fill_count;	/* # of zero fill pages */
	long	pageins;		/* # of faults resulting in pageins */
	long	pages_pagedin;		/* # of pages paged in */
	long	cow_faults;		/* # of copy-on-write faults */
	long	reactivations;		/* # of pages reactivated */

			/* Information about object cache performance */
	long	lookups;		/* object cache lookups */
	long	hits;			/* object cache hits */

			/* Information about page replacement algorithm */
	long	pageouts;		/* # of pageout operations (clusters) */
	long	pages_pagedout;		/* # of pages paged out */
};

typedef struct host_paging_info	host_paging_info_data_t;
typedef struct host_paging_info	*host_paging_info_t;
#define HOST_PAGING_INFO_COUNT \
		(sizeof(host_paging_info_data_t)/sizeof(int))
#endif	/*_MACH_HOST_INFO_H_*/
