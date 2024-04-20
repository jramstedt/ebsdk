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
 *	@(#)$RCSfile: processor_info.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:42 $
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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */

/*
 *	File:	mach/processor_info.h
 *	Author:	David L. Black
 *
 *	Copyright (C) 1988 David L. Black
 *
 *	Data structure definitions for processor_info, processor_set_info
 */

#ifndef	_MACH_PROCESSOR_INFO_H_
#define _MACH_PROCESSOR_INFO_H_

#include <mach/machine.h>

/*
 *	Generic information structure to allow for expansion.
 */
typedef int	*processor_info_t;	/* varying array of int. */

#define PROCESSOR_INFO_MAX	(1024)	/* max array size */
typedef int	processor_info_data_t[PROCESSOR_INFO_MAX];


typedef int	*processor_set_info_t;	/* varying array of int. */

#define PROCESSOR_SET_INFO_MAX	(1024)	/* max array size */
typedef int	processor_set_info_data_t[PROCESSOR_SET_INFO_MAX];

/*
 *	Currently defined information.
 */
#define	PROCESSOR_BASIC_INFO	1		/* basic information */

struct processor_basic_info {
	cpu_type_t	cpu_type;	/* type of cpu */
	cpu_subtype_t	cpu_subtype;	/* subtype of cpu */
	boolean_t	running;	/* is processor running */
	int		slot_num;	/* slot number */
	boolean_t	is_master;	/* is this the master processor */
	int 		pset_id;	/* processor pset identifier */
};

typedef	struct processor_basic_info	processor_basic_info_data_t;
typedef struct processor_basic_info	*processor_basic_info_t;
#define PROCESSOR_BASIC_INFO_COUNT \
	((sizeof(processor_basic_info_data_t)+sizeof(long)-1)/sizeof(long))


#define	PROCESSOR_SET_BASIC_INFO	1	/* basic information */

struct processor_set_basic_info {
	int		processor_count;	/* How many processors */
	int		processor_mask;		/* which processors? */
	int		task_count;		/* How many tasks */
	int		thread_count;		/* How many threads */
	int		load_average;		/* Scaled */
	int		mach_factor;		/* Scaled */
	int		pset_id;		/* unique identifier */
	int		state;			/* procesor_set state */
};

/*
 *	Scaling factor for load_average, mach_factor.
 */
#define	LOAD_SCALE	1000		

typedef	struct processor_set_basic_info	processor_set_basic_info_data_t;
typedef struct processor_set_basic_info	*processor_set_basic_info_t;
#define PROCESSOR_SET_BASIC_INFO_COUNT \
	((sizeof(processor_set_basic_info_data_t) + sizeof(long)-1)/ \
								sizeof(long))

#define PROCESSOR_SET_SCHED_INFO	2	/* scheduling info */

struct processor_set_sched_info {
	int		policies;	/* allowed policies */
	int		max_priority;	/* max priority for new threads */
};

typedef	struct processor_set_sched_info	processor_set_sched_info_data_t;
typedef struct processor_set_sched_info	*processor_set_sched_info_t;
#define PROCESSOR_SET_SCHED_INFO_COUNT \
	 ((sizeof(processor_set_sched_info_data_t) + sizeof(long)-1)/ \
								sizeof(long))


#endif	/*_MACH_PROCESSOR_INFO_H_*/
