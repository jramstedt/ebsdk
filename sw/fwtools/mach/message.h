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
 *	@(#)$RCSfile: message.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:41 $
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
 *
 *	Mach IPC message and primitive function definitions.
 *
 */

#ifndef	_MACH_MESSAGE_H_
#define _MACH_MESSAGE_H_

#include <mach/port.h>

/*
 *	Message data structures.
 *
 *	Messages consist of two parts: a fixed-size header, immediately
 *	followed by a variable-size array of typed data items.
 *
 */

typedef	unsigned int	msg_size_t;

typedef	struct {
		unsigned int	msg_unused : 24,
				msg_simple : 8;
		msg_size_t	msg_size;
		int		msg_type;
		port_t		msg_local_port;
		port_t		msg_remote_port;
		int		msg_id;
} msg_header_t;

#define MSG_SIZE_MAX	8192

/*
 *	Known values for the msg_type field.
 *	These are Accent holdovers, which should be purged when possible.
 *
 *	Only one bit in the msg_type field is used by the kernel.
 *	Others are available to user applications.  See <msg_type.h>
 *	for system application-assigned values.
 */

#define MSG_TYPE_NORMAL		0
#define MSG_TYPE_EMERGENCY	1

/*
 *	Each data item is preceded by a description of that
 *	item, including what type of data, how big it is, and
 *	how many of them are present.
 *
 *	The actual data will either follow this type
 *	descriptor ("inline") or will be specified by a pointer.
 *
 *	If the type name, size, or number is too large to be encoded
 *	in this structure, the "longform" option may be selected,
 *	and those fields must immediately follow in full integer fields.
 *
 *	For convenience, out-of-line data regions or port rights may
 *	be deallocated when the message is sent by specifying the
 *	"deallocate" field.  Beware: if the data item in question is both
 *	out-of-line and contains port rights, then both will be deallocated.
 */

typedef struct  {
	unsigned int	msg_type_name : 8,		/* What kind of data */
			msg_type_size : 8,		/* How many bits is each item */
			msg_type_number : 12,		/* How many items are there */
			msg_type_inline : 1,		/* If true, data follows; else a pointer */
			msg_type_longform : 1,		/* Name, size, number follow: see above */
			msg_type_deallocate : 1,	/* Deallocate port rights or memory */
			msg_type_unused : 1;
} msg_type_t;

typedef	struct	{
	msg_type_t	msg_type_header;
	unsigned short	msg_type_long_name;
	unsigned short	msg_type_long_size;
	unsigned int	msg_type_long_number;
} msg_type_long_t;

/*
 *	Known values for the msg_type_name field.
 *
 *	The only types known to the Mach kernel are
 *	the port types, and those types used in the
 *	kernel RPC interface.
 */

#define MSG_TYPE_UNSTRUCTURED	0
#define MSG_TYPE_BIT		0
#define MSG_TYPE_BOOLEAN	0
#define MSG_TYPE_INTEGER_16	1
#define MSG_TYPE_INTEGER_32	2
#if	!defined(KERNEL)
#define MSG_TYPE_PORT_OWNERSHIP	3
#define MSG_TYPE_PORT_RECEIVE	4
#endif	/*!defined(KERNEL) */
#define MSG_TYPE_PORT_ALL	5
#define MSG_TYPE_PORT		6
#define MSG_TYPE_CHAR		8
#define MSG_TYPE_BYTE		9
#define MSG_TYPE_INTEGER_8	9
#define MSG_TYPE_REAL		10
#define MSG_TYPE_STRING		12
#define MSG_TYPE_STRING_C	12
/*	MSG_TYPE_INVALID	13	unused */
/*	MSG_TYPE_INTERNAL_MEMORY 14	obsolete */
/*	Until MIG & netmsgserver are updated, use integer type, see also
 *	std_types.defs XXXX
 *	MSG_TYPE_PORT_NAME	15 */
#ifdef	KERNEL
#define MSG_TYPE_INTERNAL_MEMORY 14
#else	/* KERNEL */
#define MSG_TYPE_INTERNAL_MEMORY MSG_TYPE_INTEGER_8
#endif	/* KERNEL */

#define MSG_TYPE_PORT_NAME	15		/* A capability name */
#define MSG_TYPE_INTEGER_64     16
#define MSG_TYPE_LAST		17		/* Last assigned */

#define MSG_TYPE_POLYMORPHIC	((unsigned int) -1)

/*
 *	Is a given item a port type?
 */

#if	!defined(KERNEL)
#define MSG_TYPE_PORT_ANY(x)	\
	(((x) >= MSG_TYPE_PORT_OWNERSHIP) && ((x) <= MSG_TYPE_PORT))
#else	/*!defined(KERNEL)*/
#define MSG_TYPE_PORT_ANY(x)	\
	(((x) == MSG_TYPE_PORT) || ((x) == MSG_TYPE_PORT_ALL))
#endif	/*!defined(KERNEL)*/

#ifdef	__alpha
#define	MSG_TYPE_ALIGN_64(tn,tp)		\
	(MSG_TYPE_PORT_ANY(tn)		|| 	\
	((tn) == MSG_TYPE_PORT_NAME)	|| ((tn) == MSG_TYPE_INTEGER_64)  ||  \
	!(tp)->msg_type_header.msg_type_inline)

#endif
/*
 *	Other basic types
 */

typedef	unsigned int	msg_timeout_t;

/*
 *	Options to IPC primitives.
 *
 *	These can be combined by or'ing; the combination RPC call
 *	uses both SEND_ and RCV_ options at once.
 */

typedef	int		msg_option_t;

#define MSG_OPTION_NONE	0x0000	/* Terminate only when message op works */

#define SEND_TIMEOUT	0x0001	/* Terminate on timeout elapsed */
#define SEND_NOTIFY	0x0002	/* Terminate with reply message if need be */

#define SEND_INTERRUPT	0x0004	/* Terminate on software interrupt */

#ifdef	KERNEL
#define SEND_USER	(SEND_TIMEOUT|SEND_NOTIFY)
				/* Valid user options */
#define SEND_ALWAYS	0x0008	/* Never block */
/*	SEND_KERNEL	0x0010	   Kernel is originator (not used) */
#define SEND_SWITCH	0x0020	/* Use handoff scheduling */
#endif	/*KERNEL*/

#define RCV_TIMEOUT	0x0100	/* Terminate on timeout elapsed */
#define RCV_NO_SENDERS	0x0200	/* Terminate if I'm the only sender left */
#define RCV_INTERRUPT	0x0400	/* Terminate on software interrupt */

#ifdef	KERNEL
#define RCV_USER	(RCV_TIMEOUT|RCV_NO_SENDERS|RCV_INTERRUPT)
#endif	/*KERNEL*/

/*
 *	Returns from IPC primitives.
 *
 *	Values are separate in order to allow RPC users to
 *	distinguish which operation failed; for successful completion,
 *	this doesn't matter.
 */

typedef	int		msg_return_t;

#define SEND_SUCCESS		0

#define SEND_ERRORS_START	-100
#ifdef	KERNEL
#define SEND_IN_PROGRESS	-100
#endif	/*KERNEL*/
#define SEND_INVALID_MEMORY	-101	/* Message or OOL data invalid */
#define SEND_INVALID_PORT	-102	/* Reference to inacessible port */
#define SEND_TIMED_OUT		-103	/* Terminated due to timeout */
#define SEND_WILL_NOTIFY	-105	/* Msg accepted provisionally */
#define SEND_NOTIFY_IN_PROGRESS	-106	/* Already awaiting a notification */
#define SEND_KERNEL_REFUSED	-107	/* Message to the kernel refused */
#define SEND_INTERRUPTED	-108	/* Software interrupt during send */
#define SEND_MSG_TOO_LARGE	-109	/* Message specified was too large */
#define SEND_MSG_TOO_SMALL	-110	/* Data specified exceeds msg size */
/*	SEND_MSG_SIZE_CHANGE	-111	   Msg size changed during copy */
#define SEND_ERRORS_END		-111

#define msg_return_send(x)	((x) < SEND_ERRORS_START && (x) > SEND_ERRORS_END)

#define RCV_SUCCESS		0

#define RCV_ERRORS_START	-200
#ifdef	KERNEL
#define RCV_IN_PROGRESS		-200
#endif	/*KERNEL*/
#define RCV_INVALID_MEMORY	-201
#define RCV_INVALID_PORT	-202
#define RCV_TIMED_OUT		-203
#define RCV_TOO_LARGE		-204	/* Msg structure too small for data */
#define RCV_NOT_ENOUGH_MEMORY	-205	/* Can't find space for OOL data */
#define RCV_ONLY_SENDER		-206	/* Receiver is only sender */
#define RCV_INTERRUPTED		-207
#define RCV_PORT_CHANGE		-208	/* Port was put in a set */
#define RCV_ERRORS_END		-209

#define msg_return_rcv(x)	((x) < RCV_ERRORS_START && (x) > RCV_ERRORS_END)

#define RPC_SUCCESS		0

/*
 *	The IPC primitive functions themselves
 */

msg_return_t	msg_send(
#if	defined(__STDC__) || defined(c_plusplus)
	msg_header_t	*header,
	msg_option_t	option,
	msg_timeout_t	timeout);
#else	/*__STDC__ || c_plusplus*/
#if	LINTLIBRARY
			header, option, timeout)
	msg_header_t	*header;
	msg_option_t	option;
	msg_timeout_t	timeout;
	{ return(SEND_SUCCESS); }
#else	/*LINTLIBRARY*/
		);
#endif	/*LINTLIBRARY*/
#endif	/*__STDC__ || c_plusplus*/

msg_return_t	msg_receive(
#if	defined(__STDC__) || defined(c_plusplus)
	msg_header_t	*header,
	msg_option_t	option,
	msg_timeout_t	timeout);
#else	/*__STDC__ || c_plusplus*/
#if	LINTLIBRARY
			header, option, timeout)
	msg_header_t	*header;
	msg_option_t	option;
	msg_timeout_t	timeout;
	{ return(RCV_SUCCESS); }
#else	/*LINTLIBRARY*/
		);
#endif	/*LINTLIBRARY*/
#endif	/*__STDC__ || c_plusplus*/

msg_return_t	msg_rpc(
#if	defined(__STDC__) || defined(c_plusplus)
	msg_header_t	*header,	/* in/out */
	msg_option_t	option,
	msg_size_t	rcv_size,
	msg_timeout_t	send_timeout,
	msg_timeout_t	rcv_timeout);
#else	/*__STDC__ || c_plusplus*/
#if	LINTLIBRARY
			header, option, rcv_size,
			send_timeout, rcv_timeout)
	msg_header_t	*header;	/* in/out */
	msg_option_t	option;
	msg_size_t	rcv_size;
	msg_timeout_t	send_timeout;
	msg_timeout_t	rcv_timeout;
	{ return(RPC_SUCCESS); }
#else	/*LINTLIBRARY*/
		);
#endif	/*LINTLIBRARY*/
#endif	/*__STDC__ || c_plusplus*/

#endif	/*_MACH_MESSAGE_H_*/
