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
 *	@(#)$RCSfile: thread_status.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:43 $
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
 *	File:	mach/thread_status.h
 *	Author:	Avadis Tevanian, Jr.
 *
 *	This file contains the structure definitions for the user-visible
 *	thread state.  This thread state is examined with the thread_get_state
 *	kernel call and may be changed with the thread_set_state kernel call.
 *
 */

#ifndef	_MACH_THREAD_STATUS_H_
#define _MACH_THREAD_STATUS_H_

/*
 *	The actual structure that comprises the thread state is defined
 *	in the machine dependent module.
 */
#include <mach/machine/thread_status.h>

/*
 *	Generic definition for machine-dependent thread status.
 */

#ifdef	__alpha
typedef	long		*thread_state_t;	/* Variable-length array */
#else
typedef	int		*thread_state_t;	/* Variable-length array */
#endif

#define THREAD_STATE_MAX	(64)		/* Maximum array size */
#ifdef	__alpha
typedef	long	thread_state_data_t[THREAD_STATE_MAX];
#else
typedef	int	thread_state_data_t[THREAD_STATE_MAX];
#endif

#define THREAD_STATE_FLAVOR_LIST	0	/* List of valid flavors */

#endif	/*_MACH_THREAD_STATUS_H_*/
