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
 *	@(#)$RCSfile: time_value.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:44 $
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

#ifndef	_MACH_TIME_VALUE_H_
#define _MACH_TIME_VALUE_H_

/*
 *	Time value returned by kernel.
 */

struct time_value {
	long	seconds;
	long	microseconds;
};
typedef	struct time_value	time_value_t;

/*
 *	Macros to manipulate time values.  Assume that time values
 *	are normalized (microseconds <= 999999).
 */
#define TIME_MICROS_MAX	(1000000)

#define time_value_add_usec(val, micros)	{	\
	if (((val)->microseconds += (micros))		\
		>= TIME_MICROS_MAX) {			\
	    (val)->microseconds -= TIME_MICROS_MAX;	\
	    (val)->seconds++;				\
	}						\
}

#define time_value_add(result, addend)		{		\
	(result)->microseconds += (addend)->microseconds;	\
	(result)->seconds += (addend)->seconds;			\
	if ((result)->microseconds >= TIME_MICROS_MAX) {	\
	    (result)->microseconds -= TIME_MICROS_MAX;		\
	    (result)->seconds++;				\
	}							\
}

#endif	/*_MACH_TIME_VALUE_H_*/
