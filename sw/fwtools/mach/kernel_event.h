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
 *	@(#)$RCSfile: kernel_event.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:40 $
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
 * OSF/1 Release 1.0
 */
/*
 *	File:	kernel_event.h
 *	Author:	Ted Lehr
 *
 *	Copyright (C) 1987, Ted Lehr
 *
 * 	Kernel Monitoring header file to be exported to user.  Contains
 *	the the definition of a kernel event.  
 *
 */

#ifndef _MACH_KERNEL_EVENT_H_
#define _MACH_KERNEL_EVENT_H_

/*
 * The kern_mon_data_t and kern_mon_buffer_t are used by MIG
 * to generate the interfaces correctly.  They are not
 * "necessary" but fulfill stylistic conventions.
 */

typedef
struct 	kernel_event {	/* unit kernel event */
	 unsigned 	event_type;	 /* the type of kernel event	*/
	 unsigned	first_element;	 /* the stopped thread 		*/
	 unsigned	second_element;  /* the started thread 		*/
	 unsigned	third_element;   /* flag and cpu number 	*/
	 unsigned 	hi_time;         /* hi time stamp 		*/
	 unsigned 	lo_time;         /* lo time stamp 		*/
} kern_mon_event, *kern_mon_event_t, kern_mon_data_t, *kern_mon_buffer_t;

#define MONITOR_MIG_BUF_SIZE    320             /* ONLY allowable size of   *
						 * buffer passed to monitor *
						 * calls                    */


/***************************************************************************
 *
 *	Kernel Sensor types:
 */
#define MONITOR_CNTXT_SWTCH	0x0;

#endif /* _MACH_KERNEL_EVENT_H_ */
