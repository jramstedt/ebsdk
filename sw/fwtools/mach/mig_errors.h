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
 *	@(#)$RCSfile: mig_errors.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:41 $
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
 * Mach Interface Generator errors
 *
 */

#ifndef	_MACH_MIG_ERRORS_H_
#define _MACH_MIG_ERRORS_H_

#include <mach/kern_return.h>
#include <mach/message.h>

#define MIG_TYPE_ERROR		-300		/* Type check failure */
#define MIG_REPLY_MISMATCH	-301		/* Wrong return message ID */
#define MIG_REMOTE_ERROR	-302		/* Server detected error */
#define MIG_BAD_ID		-303		/* Bad message ID */
#define MIG_BAD_ARGUMENTS	-304		/* Server found wrong arguments */
#define MIG_NO_REPLY		-305		/* Server shouldn't reply */
#define MIG_EXCEPTION		-306		/* Server raised exception */
#define MIG_ARRAY_TOO_LARGE	-307		/* User specified array not large enough
						   to hold returned array */

typedef struct {
	msg_header_t	Head;
	msg_type_t	RetCodeType;
	kern_return_t	RetCode;
} death_pill_t;

typedef struct mig_symtab {
	char	*ms_routine_name;
	int	ms_routine_number;
#ifdef	hc
	void
#else
	int
#endif
		(*ms_routine)();
} mig_symtab_t;

#endif	/*_MACH_MIG_ERRORS_H_*/
