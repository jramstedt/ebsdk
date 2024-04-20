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
 *	@(#)$RCSfile: error.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:39 $
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
 * File:	mach/error.h
 * Purpose:
 *	error module definitions
 *
 */

#ifndef	_MACH_ERROR_H_
#define _MACH_ERROR_H_

#include <mach/kern_return.h>

/*
 *	error number layout as follows:
 *
 *	hi		 		       lo
 *	| system(6) | subsystem(12) | code(14) |
 */


#define err_none		(mach_error_t)0
#define ERR_SUCCESS		(mach_error_t)0
#define ERR_ROUTINE_NIL		(mach_error_fn_t)0


#define err_system(x)		(((x)&0x3f)<<26)
#define err_sub(x)		(((x)&0xfff)<<14)

#define err_get_system(err)	(((err)>>26)&0x3f)
#define err_get_sub(err)	(((err)>>14)&0xfff)
#define err_get_code(err)	((err)&0x3fff)

#define system_emask		(err_system(0x3f))
#define sub_emask		(err_sub(0xfff))
#define code_emask		(0x3fff)


/*	major error systems	*/
#define err_kern		err_system(0x0)		/* kernel */
#define err_us			err_system(0x1)		/* user space library */
#define err_server		err_system(0x2)		/* user space servers */
#define err_ipc			err_system(0x3)		/* mach-ipc errors */
#define err_local		err_system(0x3e)	/* user defined errors */
#define err_ipc_compat		err_system(0x3f)	/* (compatibility) mach-ipc errors */

#define err_max_system		0x3f


/*	unix errors get lumped into one subsystem  */
#define unix_err(errno)		(err_kern|err_sub(3)|errno)

typedef	kern_return_t	mach_error_t;
typedef mach_error_t	(* mach_error_fn_t)();

#endif	/*_MACH_ERROR_H_*/
