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
 *	@(#)$RCSfile: vm_statistics.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:44 $
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
 *	File:	mach/vm_statistics.h
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young, David Golub
 *	Copyright (C) 1986, Avadis Tevanian, Jr., Michael Wayne Young,
 *		David Golub
 *
 *	Virtual memory statistics structure.
 *
 */

#ifndef	_MACH_VM_STATISTICS_H_
#define _MACH_VM_STATISTICS_H_

#include <mach/machine/vm_statistics.h>

struct vm_statistics {
	long	pagesize;		/* page size in bytes */
	long	free_count;		/* # of pages free */
	long	active_count;		/* # of pages active */
	long	inactive_count;		/* # of pages inactive */
	long	wire_count;		/* # of pages wired down */
	long	zero_fill_count;	/* # of zero fill pages */
	long	reactivations;		/* # of pages reactivated */
	long	pageins;		/* # of pageins */
	long	pageouts;		/* # of pageouts */
	long	faults;			/* # of faults */
	long	cow_faults;		/* # of copy-on-writes */
	long	lookups;		/* object cache lookups */
	long	hits;			/* object cache hits */
#define v_pgin pageins 			/* # of pagein requests */
#define v_pgpgout pageouts		/* # of pages paged-out */
#define v_pfault cow_faults
#define v_vfault faults
#define v_atch reactivations
/* sar -r */
#define freemem free_count
};

typedef struct vm_statistics	*vm_statistics_t;
typedef struct vm_statistics	vm_statistics_data_t;

#ifdef	KERNEL
extern vm_statistics_data_t	vm_stat;
#endif	/* KERNEL */

/*
 *	Each machine dependent implementation is expected to
 *	keep certain statistics.  They may do this anyway they
 *	so choose, but are expected to return the statistics
 *	in the following structure.
 */

#ifndef PMAP_STATISTICS_EXTENSION
	/* default definitions when ./machine/vm_statistics.h is empty */
#define PMAP_STATISTICS_EXTENSION	int /* no-op declaration */
#define pmap_resident_stack(pmap)	1
#define pmap_resident_text(pmap)	0
#define pmap_resident_data(pmap)	(pmap_resident_count(pmap)	\
					 - pmap_resident_stack(pmap)	\
					 - pmap_resident_text(pmap))
#else
	/* definitions from ./machine/vm_statistics.h */
#endif /* PMAP_STATISTICS_EXTENSION */

struct pmap_statistics {
	int		resident_count;		/* # of pages mapped (total)*/
	int		max_resident_count;	/* max # of pages mapped */
	int		wired_count;		/* # of pages wired */
	PMAP_STATISTICS_EXTENSION;
};

typedef struct pmap_statistics	*pmap_statistics_t;

#endif	/*_MACH_VM_STATISTICS_H_*/
