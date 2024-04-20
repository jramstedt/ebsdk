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
 *	@(#)$RCSfile: time_stamp.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:43 $
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

#ifndef	_MACH_TIME_STAMP_H_
#define _MACH_TIME_STAMP_H_

#include <mach/machine/time_stamp.h>

/*
 *	time_stamp.h -- definitions for low-overhead timestamps.
 */

struct tsval {
	unsigned	low_val;	/* least significant word */
	unsigned	high_val;	/* most significant word */
};

/*
 *	Format definitions.
 */

#ifndef	TS_FORMAT
/*
 *	Default case - Just return a tick count for machines that
 *	don't support or haven't implemented this.  Assume 100Hz ticks.
 *
 *	low_val - Always 0.
 *	high_val - tick count.
 */
#define TS_FORMAT	1

#ifdef	KERNEL
extern unsigned	ts_tick_count;
#endif	/* KERNEL */
#endif	/* TS_FORMAT */

/*
 *	List of all format definitions for convert_ts_to_tv.
 */

#define TS_FORMAT_DEFAULT	1
#define TS_FORMAT_MMAX		2

#endif	/* _MACH_TIME_STAMP_H_ */
