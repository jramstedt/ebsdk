/* file:	dynamic_def.h                                               */
/*                                                                          */
/* Copyright (C) 1990 by                                                    */
/* Digital Equipment Corporation, Maynard, Massachusetts.                   */
/* All rights reserved.                                                     */
/*                                                                          */
/* This software is furnished under a license and may be used and copied    */
/* only  in  accordance  of  the  terms  of  such  license  and with the    */
/* inclusion of the above copyright notice. This software or  any  other    */
/* copies thereof may not be provided or otherwise made available to any    */
/* other person.  No title to and  ownership of the  software is  hereby    */
/* transferred.                                                             */
/*                                                                          */
/* The information in this software is  subject to change without notice    */
/* and  should  not  be  construed  as a commitment by digital equipment    */
/* corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by digital.                  */
/*                                                                          */
/*                                                                          */
/* Abstract:	Dynamic memory allocation/deallocation data structures.     */
/*                                                                          */
/* Author:	AJ Beaverson                                                */
/*                                                                          */
/* Modifications:                                                           */
/* 								 	    */
/*	ajb	07-Feb-1992	Change space to pid and misc fields	    */
/*                                                                          */
/*      djm     23-Jul-1991     Added hiwater for tracking zone max size    */
/*                                                                          */
/*	ajb	20-May-1991	Remove lookaside lists			    */
/*                                                                          */
/*	phk	21-Dec-1990	Add DYN$M_NOWAIT                            */
/*                                                                          */
/*	ajb	21-Dec-1990	Adapted from SDL file			    */
/*                                                                          */
/*	ajb	17-Dec-1990	Add DYN$K_ZONE and DYN$K_RANDOM             */
/*                                                                          */
/*	jad	06-Dec-1990	Add NI_LOOKASIDE                            */
/*                                                                          */
/*	ajb	15-Oct-1990	Add ability to specify alignment on malloc  */
/*                                                                          */
/*	jad	26-Sep-1990	Make flodding an option.                    */
/*                                                                          */
/*	ajb	07-Aug-1990	Add lookaside lists                         */
/*                                                                          */
/*	ajb	12-Jul-1990	Add dyn$m_print                             */
/*                                                                          */
/*	ajb	25-Jun-1990	Add ownership option to dynamic memory      */
/*				routines so that allocated blocks are       */
/*				by default put on an ownership queue, and   */
/*				optionally not.                             */
/*                                                                          */
/*	ajb	07-Mar-1990	Initial entry.                              */
/*                                                                          */

/*
 * Our allocation granularity is also our alignment.  For example, if the
 * granularity is 32, the all blocks (including overhead) are n*32 bytes long,
 * where 1 <= n <= (some large number).
 *
 * When allocating a block from the free list, it is broken up only if the
 * the number of bytes left over exceeds DYN$K_FRAG.
 */
#ifndef DYNAMIC_DEF
#define DYNAMIC_DEF 1

#define DYN$K_GRAN		 32
#define DYN$K_FRAG		 (DYN$K_GRAN * 2)

#define DYN$M_FLOOD_INIT	 1
#define DYN$M_FLOOD_MALLOC	 2
#define DYN$M_FLOOD_FREE	 4

#define DYN$M_SYNC		 1
#define DYN$M_OWN		 2
#define DYN$M_PRINT		 4
#define DYN$M_FLOOD		 8
#define DYN$M_ALIGN		 16	/* buffer has alignment             */
#define DYN$M_ZONE		 32	/* user specified heap              */
#define DYN$M_WAIT  		 128	/* return a null pointer...         */
                                        /* if not enough memory             */

#define DYN$K_NOSYNC		 DYN$M_SYNC
#define DYN$K_SYNC		 0
#define DYN$K_NOOWN		 DYN$M_OWN
#define DYN$K_OWN		 0
#define DYN$K_PRINT		 DYN$M_PRINT
#define DYN$K_NOPRINT		 0
#define DYN$K_NOFLOOD		 DYN$M_FLOOD
#define DYN$K_FLOOD		 0
#define DYN$K_ALIGN		 DYN$M_ALIGN
#define DYN$K_NOALIGN		 0
#define DYN$K_ZONE		 DYN$M_ZONE
#define DYN$K_WAIT               0               
#define DYN$K_NOWAIT             DYN$M_WAIT  

/*+                                                                         */
/* ================================================                         */
/* = DYNAMIC - Dynamic Memory Block Overhead Area =                         */
/* ================================================                         */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* Each dynamic memory block has two parts: an overhead area and            */
/* the memory itself.  All blocks are linked together in an                 */
/* adjacency list.  All blocks on the free list are also linked             */
/* together on a free list.                                                 */
/*                                                                          */
/*                                                                          */
/* The Dynamic Memory Block Pool is stored as a linked list of adjacent blocks */
/* ordered by increasing address. The free_fl and free_bl link the block onto */
/* the free list or if the block is allocated point to this block.          */
/*                                                                          */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       --------------------------------------------------------------------- */
/*       Structure Members    | Data Type | Description                     */
/*       --------------------------------------------------------------------- */
/*       adj_fl               | Address   | flink for adjacent list         */
/*                            |           |                                 */
/*       adj_bl               | Address   | blink for adjaceny list         */
/*                            |           |                                 */
/*       free_fl              | Address   | Flink for free/ownership/lookaside */
/*                            |           | list                            */
/*                            |           |                                 */
/*       free_bl              | Address   | Blink for free/ownership/lookaside */
/*                            |           | list                            */
/*                            |           |                                 */
/*       bsize                | Longword  | Size of the memory block including */
/*                            |           | overhead                        */
/*                            |           |                                 */
/*       pid                  | Longword  | Pid of allocating process       */
/*                            |           |                                 */
/*       zone                 | Longword  | pointer to parent zone          */
/*                            |           |                                 */
/*      backlink              | address   | points back to begining of header */
/*:      --------------------------------------------------------------------- */
/*                                                                          */
/*--                                                                        */
struct DYNAMIC {
    struct DYNAMIC *adj_fl;
    struct DYNAMIC *adj_bl;
    struct DYNAMIC *free_fl;
    struct DYNAMIC *free_bl;
    long int bsize;		/* positive when free, negative if allocated */
    long int pid;
    long int zone;		/* parent zone */
    struct DYNAMIC *backlink;
    } ;

/*+                                                                           */
/* ===========================================================================*/
/* = ZONE - dynamic memory zone descriptor	                              =*/
/* ===========================================================================*/
/*                                                                            */
/* STRUCTURE OVERVIEW:                                                        */
/*                                                                            */
/*	A ZONE is a descriptor that gives all pertinent information for       */
/*	a heap.                                                               */
/*                                                                            */
/*                                                                            */
/* STRUCTURE MEMBERS:                                                         */
/*                                                                            */
/*:                                                                           */
/*       ---------------------------------------------------------------------*/
/*       Structure Members    | Data Type  | Description                      */
/*       ---------------------------------------------------------------------*/
/*                            |           |                                   */
/*       header               | DYNAMIC   | initial header block              */
/*                            |           |                                   */
/*       sync                 | SEMAPHORE | synchronization semaphore         */
/*                            |           |                                   */
/*       release              | SEMAPHORE | semaphore to post when memory is  */
/*			      |           | freed                             */
/*                            |           |                                   */
/*       base                 | address   | base address of zone              */
/*                            |           |                                   */
/*       size                 | Longword  | size of zone in bytes             */
/*                            |           |                                   */
/*       allocated            | longword  | number of bytes in zone that are  */
/*                            |           | in use                            */
/*                            |           |                                   */
/*       hiwater              | longword  | highest allocation                */
/*                            |           |                                   */
/*:      ---------------------------------------------------------------------*/
/*-                                                                           */
struct ZONE {
	struct DYNAMIC		header;
	unsigned int		size;		/* zone size in bytes */
	unsigned int		allocated;
	unsigned int		hiwater;	/* highest allocation */
	struct SEMAPHORE	sync;
	struct SEMAPHORE	release;
};

#define MAX_ZONES 8
#define	free_bias	(offsetof (struct DYNAMIC, free_fl))
#define	adjacent_bias	(offsetof (struct DYNAMIC, adj_fl))

#endif
