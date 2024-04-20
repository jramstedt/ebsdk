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
 *	@(#)$RCSfile: port.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:42 $
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
 *	File:	mach/port.h
 *
 *	Definition of a port
 *
 *	[The basic port_t type should probably be machine-dependent,
 *	as it must be represented by a 32-bit integer.]
 */

#ifndef	_MACH_PORT_H_
#define _MACH_PORT_H_


#ifdef	__alpha
typedef long 		port_name_t;		/* A capability's name */
#else
typedef int 		port_name_t;		/* A capability's name */
#endif
typedef port_name_t	port_set_name_t;	/* Descriptive alias */
typedef port_name_t	*port_name_array_t;

typedef int		port_type_t;		/* What kind of capability? */
typedef port_type_t	*port_type_array_t;

	/* Values for port_type_t */

#define PORT_TYPE_NONE		0		/* No rights */
#define PORT_TYPE_SEND		1		/* Send rights */
#if	!defined(KERNEL)
#define PORT_TYPE_RECEIVE	3		/* Send, receive rights */
#define PORT_TYPE_OWN		5		/* Send, ownership rights */
#endif	/*!defined(KERNEL)*/
#define PORT_TYPE_RECEIVE_OWN	7		/* Send, receive, ownership */
#define PORT_TYPE_SET		9		/* Set ownership */
#define PORT_TYPE_LAST		10		/* Last assigned */

typedef	port_name_t	port_t;			/* Port with send rights */
typedef	port_t		port_rcv_t;		/* Port with receive rights */
typedef	port_t		port_own_t;		/* Port with ownership rights */
typedef	port_t		port_all_t;		/* Port with receive and ownership */
typedef	port_t		*port_array_t;

#define PORT_NULL	((port_name_t) 0L)	/* Used to denote no port; legal value */


#ifndef	KERNEL
extern port_set_name_t PORT_ENABLED;
#endif	/*KERNEL*/
#endif	/*_MACH_PORT_H_*/
