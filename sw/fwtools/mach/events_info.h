
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
 * @(#)$RCSfile: events_info.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:39 $
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
 *	Machine-independent event information structures and definitions.
 *
 *	The definitions in this file are exported to the user.  The kernel
 *	will translate its internal data structures to these structures
 *	as appropriate.
 *
 *	This data structure is used to track events that occur during
 *	thread execution, and to summarize this information for tasks.
 */

#ifndef	_MACH_EVENTS_INFO_H_
#define _MACH_EVENTS_INFO_H_

struct events_info {
	long		faults;		/* number of page faults */
	long		zero_fills;	/* number of zero fill pages */
	long		reactivations;	/* number of reactivated pages */
	long		pageins;	/* number of actual pageins */
	long		cow_faults;	/* number of copy-on-write faults */
	long		messages_sent;	/* number of messages sent */
	long		messages_received; /* number of messages received */
};
typedef struct events_info		events_info_data_t;
typedef struct events_info		*events_info_t;
#ifdef	__alpha
#define EVENTS_INFO_COUNT	\
	((sizeof(events_info_data_t) + sizeof(long) - 1) / sizeof(long))
#else
#define EVENTS_INFO_COUNT	\
		(sizeof(events_info_data_t) / sizeof(int))
#endif
#endif	/*_MACH_EVENTS_INFO_H_*/
