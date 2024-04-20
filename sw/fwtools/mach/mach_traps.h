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
 *	@(#)$RCSfile: mach_traps.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:40 $
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
 *	Definitions of general Mach system traps.
 *
 *	IPC traps are defined in <mach/message.h>.
 *	Kernel RPC functions are defined in <kern/mach.h>.
 */

#ifndef	_MACH_MACH_TRAPS_H_
#define _MACH_MACH_TRAPS_H_

#define _MACH_INIT_	1

#include <mach/mach_types.h>

#ifdef	KERNEL
port_t		task_self
#else	/*KERNEL*/
task_t		task_self
#endif	/*KERNEL*/
#ifdef	LINTLIBRARY
			()
	 { return(PORT_NULL); }
#else	/*LINTLIBRARY*/
			();
#endif	/*LINTLIBRARY*/

port_t		task_data
#ifdef	LINTLIBRARY
			()
	 { return(PORT_NULL); }
#else	/*LINTLIBRARY*/
			();
#endif	/*LINTLIBRARY*/

port_t		task_notify
#ifdef	LINTLIBRARY
			()
	 { return(PORT_NULL); }
#else	/*LINTLIBRARY*/
			();
#endif	/*LINTLIBRARY*/

#ifdef	KERNEL
port_t		thread_self
#else	/*KERNEL*/
thread_t	thread_self
#endif	/*KERNEL*/
#ifdef	LINTLIBRARY
			()
	 { return(PORT_NULL); }
#else	/*LINTLIBRARY*/
			();
#endif	/*LINTLIBRARY*/

port_t		thread_reply
#ifdef	LINTLIBRARY
			()
	 { return(PORT_NULL); }
#else	/*LINTLIBRARY*/
			();
#endif	/*LINTLIBRARY*/

#ifdef	KERNEL
port_t		host_self
#else	/*KERNEL*/
host_t		host_self
#endif	/*KERNEL*/
#ifdef	LINTLIBRARY
			()
	 { return(PORT_NULL); }
#else	/*LINTLIBRARY*/
			();
#endif	/*LINTLIBRARY*/

#ifdef	KERNEL
port_t		host_priv_self
#else	/*KERNEL*/
host_priv_t	host_priv_self
#endif	/*KERNEL*/
#ifdef	LINTLIBRARY
			()
	 { return(PORT_NULL); }
#else	/*LINTLIBRARY*/
			();
#endif	/*LINTLIBRARY*/

#endif	/*_MACH_MACH_TRAPS_H_*/
