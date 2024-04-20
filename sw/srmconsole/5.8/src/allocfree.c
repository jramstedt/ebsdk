/* file:	allocfree.c
 *
 * Copyright (C) 1990 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Cobra Firmware
 *
 *  MODULE DESCRIPTION:
 *                                                
 *	Dynamic memory allocation/deallocation for EVAX/Alpha
 *	firmware.  This is a functional superset of C's MALLOC
 *	and FREE functions.
 *
 *  AUTHORS:
 *
 *	AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      07-Mar-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	jrk	13-Mar-1994	Added in check into dyn$_consistency() such
 *				that it does not do a check to see if 
 *				free blocks in memzone are flooded.  If
 *				the -z qualifier is specified and is targeting
 *				memzone, then this check is done.
 *
 *	ajb	17-Aug-1992	Implement a trap routine that catches
 *				references to memory locations.  The check is
 *				done when blocks are freed or allocated.
 *				
 *--
 */  

#include	"cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"
#include "cp$inc:prototypes.h"

#define	floor(modulus, x) (((x) / (modulus)) * (modulus))
#define ceiling(modulus, x) (floor ((modulus), (x) + (modulus)))

/*
 * Global references
 */
extern int printf ();
extern int pprintf ();
extern int krn$_post ();
extern int krn$_wait ();
extern struct PCB *getpcb ();
extern unsigned __int64 mem_size;

extern int quadset ();

#define	free_bias	(offsetof (struct DYNAMIC, free_fl))
#define	adjacent_bias	(offsetof (struct DYNAMIC, adj_fl))
	
extern struct ZONE _align (QUADWORD) defzone;
extern struct ZONE *memzone;
extern struct ZONE *zlist [MAX_ZONES];
extern int	zcount;		/* number of zones */

extern INT	dyn_flood_malloc;
extern INT	dyn_flood_free;

/*+
 * ============================================================================
 * = dynamic - Show the state of dynamic memory.                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Show the state of dynamic memory.  Dynamic memory is split into
 *	two main heaps, the console's private heap and the remaining
 *	memory heap.  
 *
 *	The -h and -c options only work on the default memory zone.
 *	To perform these actions on other zones you must
 *	specify the other zone using the -z option.
 *  
 *   COMMAND FMT: dynamic 2 B 0 dynamic
 *  
 *   COMMAND FORM:
 *  
 *	dynamic ( [-h] [-c] [-r] [-p]
 *		  [-extend <byte_count>] [-z <heap_address>] )
 *
 *   COMMAND TAG: dynamic 0 RXBZ dynamic
 *  
 *   COMMAND ARGUMENT(S):
 *
 * 	None
 *
 *   COMMAND OPTION(S):
 *
 *	-c    - Perform a consistency check on the heap.
 *	-h    - Display the headers of the blocks in the heap.
 *	-p    - Display dynamic memory statistics on a per process basis.
 *	-r    - Repair a broken heap by flooding free blocks with
 *		DYN$K_FLOOD_FREE if and only if they have been corrupted.
 *		Repairing broken heaps is dangerous at best, as it is masking
 *		underlying errors.  This flag takes effect only
 *		if a consistency check is being done.
 *	-extend <byte_count> - Extend the default memory zone by the
 *		byte count at the expense of the main memory zone.  This
 *		command assumes that these two zones are physically adjacent.
 *	-z <heap_address> - Perform the dynamic command on the heap specified
 *		by heap_address.
 *
 *   COMMAND EXAMPLE(S):
 *~
 * >>> dynamic
 * zone     zone       used    used       free    free       utili-  high
 * address  size       blocks  bytes      blocks  bytes      zation  water
 * -------- ---------- ------- ---------- ------- ---------- ------- ----------
 * 00097740 1048576    389     358944     17      689664      34 %   371872
 * 001D2B80 14805504   1       32         1       14805504     0 %   0
 *
 * >>>dynamic -cv -z 97740
 * zone     zone       used    used       free    free       utili-  high
 * address  size       blocks  bytes      blocks  bytes      zation  water
 * -------- ---------- ------- ---------- ------- ---------- ------- ----------
 * 00097740 1048576    398     359520     17      689088      34 %   371872
 *
 * >>>dynamic -h
 * zone     zone       used    used       free    free       utili-  high
 * address  size       blocks  bytes      blocks  bytes      zation  water
 * -------- ---------- ------- ---------- ------- ---------- ------- ----------
 * 00097740 1048576    392     359136     17      689472      34 %   389280
 * a 00097740 000E1600_001E0600 000E1608_001BF628 00000000 00097740 32
 * f 000E1600 0017E600_00097740 00189E68_00097748 FFFFFFFF 000E1600 643072
 * a 0017E600 001823C0_000E1600 001BF448_001B0D6C 00000023 0017E600 15808
 * .
 * .
 * . 
 * >>>
 *~
 *   COMMAND REFERENCES:
 *
 *	alloc, free
 *
 * FORM OF CALL:
 *  
 *	dynamic ( argc, *argv[] )
 *  
 * RETURNS:
 *
 *	0 - Success
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *      	   (should always be 1)
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int dynamic (int argc, char *argv[]) {
#define	QH	0	/* show headers			*/
#define	QC	1	/* do consistency check		*/
#define	QZ	2	/* user defined zone		*/
#define QR	3	/* fix corrupted free blocks	*/
#define	QP	4	/* print process usage		*/
#define	QEXTEND	5	/* extend default zone		*/
#define	QMAX	6
#define QSTRING "h c %xz r p %xextend"

	int	i;
	struct ZONE	*zp;
	struct QSTRUCT	qual [QMAX];
	int	header;
	struct ZONE	*zl [MAX_ZONES];
	int	zc;
	int	status;
	int	n;

    if (argc == 0) {

	qual[QH].present = 0;
	qual[QZ].present = 0;
	qual[QR].present = 0;
	qual[QEXTEND].present = 0;
	qual[QC].present = 1;
	qual[QP].present = 0;		/* 31-aug-94 jrk ;d xdelta messes up */

    } else {
	/*
	 * Decode the flags (argc,argv are adjusted and flags culled out).
	 */
	status = qscan (&argc, argv, "-", QSTRING, qual);
	if (status != msg_success) {
	    err_printf (status);
	    return status;
	}
    }
	/*
	 * By default use all the zones, unless the z qualifier is present in
	 * which case we use just that zone.
	 */
	if (qual [QZ].present) {
	    zp = qual [QZ].value.integer;
	    if (dyn$_badzone (zp)) return msg_dyn_badzone;
	    zc = 1;
	    zl [0] = zp;
	} else {
	    memcpy (zl, zlist, sizeof (zlist));
	    zc = zcount;
	}

	/*
	 * We only know how to extend the default zone.  To do this, we
	 * carve out a chunk from the main memory zone, and then give it to
	 * the default zone.
	 */
	if (qual [QEXTEND].present) {
	    int buf_size;
	    int buf_address;

	    buf_size = qual [QEXTEND].value.integer;

	    /* Get a block of memory from memzone. */
	    buf_address = dyn$_malloc (buf_size,
		DYN$K_ZONE | DYN$K_SYNC | DYN$K_NOWAIT | DYN$K_NOOWN,
		0,
		0,
		memzone
	    );
	    if (!buf_address) {
		err_printf ("Zone can't be extended\n");
		return msg_failure;
	    }

	    if (dyn$_expand (&defzone, buf_address, buf_size, 1)) return msg_failure;
	    err_printf ("Default zone extended at the expense of memzone.\nUse INIT before booting\n");	
	    return msg_success;
	}

	/*
	 * iterate over all requested zones
	 */
	header = 0;
	for (i=0; i<zc; i++) {
	    zp = zl [i];

	    dyn$_status (zp, &header);

	    if (qual [QC].present) {
		dyn$_consistency (zp, qual [QR].present, qual [QZ].present);
	    }

	    if (qual [QH].present) {
		dyn$_headers (zp);
	    }

	    if (qual [QP].present) {
		dyn$_procstat (zp, 0);
	    }

	}

	return msg_success;

#undef	QH
#undef	QV
#undef	QC
#undef	QZ
#undef	QR
#undef	QP
#undef	QEXTEND
#undef	QMAX
#undef	QSTRING
}

/*+
 * ============================================================================
 * = alloc - Allocate a block of memory from the heap.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Exports the 'malloc' routine out to the shell, so that 
 *      users may allocate a block of memory from the heap.
 *      The resulting block may then be used simultaneously by several
 *      test routines (there can be several readers but only one writer).
 *  
 *   COMMAND FMT: alloc_cmd 0 0 0 alloc
 *
 *   COMMAND FORM:
 *  
 *	alloc ( <size> [<modulus>] [<remainder>] [-flood] [-z <heap_address>] )
 *  
 *   COMMAND TAG: alloc_cmd 0 RXBZ alloc
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<size> - Specifies the size (hex) in bytes of the requested block.
 *	<modulus> - Specifies the modulus (hex) for the beginning address
 *		of the requested block.
 *	<remainder> - Specifies the remainder (hex) used in conjunction with
 *		the modulus for computing the beginning address of the
 *		requested block.
 *
 *   COMMAND OPTION(S):
 *
 *	-flood - Flood memory with 0s.  By default, alloc does not flood.
 *	-z <heap_address> - Allocate from the memory zone starting at address
 *		heap_address.  This address is usually obtained from the
 *		output of a 'dynamic' command).
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>alloc 200 
 *      00FFFE00
 *      >>>free fffe00
 *      >>>set base `alloc 400`
 *      >>>show base
 *      base                    00FFFC00
 *      >>>memtest $base
 *      >>>free $base
 *      >>>clear base
 *~
 *   COMMAND REFERENCES:
 *
 *	dynamic, free
 *
 * FORM OF CALL:
 *  
 *	alloc ( int argc, char *argv [] )
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int alloc_cmd (int argc, char *argv []) {
	void * address;
	int	size, modulus, remainder;

#define	QZ	0	/* user defined zone		*/
#define QFLOOD	1	/* flood memory on allocation	*/
#define	QMAX	2
#define QSTRING "%xz flood"

	struct ZONE	*zp;
	struct QSTRUCT	qual [QMAX];
	int status;
	int flood;

	/*
	 * Decode the flags (argc,argv are adjusted and flags culled out).
	 */
	status = qscan (&argc, argv, "-", QSTRING, qual);
	if (status != msg_success) {
	    err_printf (status);
	    return status;
	}


	flood = DYN$K_NOFLOOD;
	if (qual [QFLOOD].present) {
	    flood = DYN$K_FLOOD;
	}
 
	/*
	 * Default to the memory zone.
	 */
	if (qual [QZ].present) {
	    zp = qual [QZ].value.integer;
	} else {
	    zp = memzone;
	}
	if (dyn$_badzone (zp)) return msg_dyn_badzone;

	switch (argc) {
	case 2:
	    size = xtoi (argv [1]);
	    address = dyn$_malloc (
		size,
		DYN$K_SYNC|flood|DYN$K_ZONE|DYN$K_NOOWN|DYN$K_NOWAIT,
		0, 0, zp
	    );
	    break;
	case 0:
	case 1:
	case 3:
	    err_printf (msg_insuff_params);
	    return msg_insuff_params;
	case 4:
	    size = xtoi (argv [1]);
	    modulus = xtoi (argv [2]);
	    remainder = xtoi (argv [3]);
	    address = dyn$_malloc (
		size,
		DYN$K_SYNC|flood|DYN$K_ZONE|DYN$K_NOOWN|DYN$K_NOWAIT|DYN$K_ALIGN,
		modulus, remainder, zp
	    );
	    break;

	default:
	    err_printf (msg_extra_params);
	    return msg_extra_params;
	}

	/*
	 * Complain if we didn't allocate any memory
	 */
	if (address == 0) {
	    printf ("00000000\n");
	    err_printf (msg_insuff_mem, size);
	    return msg_insuff_mem;
	} else {
	    printf ("%08X\n", address);
	    return msg_success;
	}


#undef	QZ
#undef	QFLOOD
#undef	QMAX
#undef	QSTRING
}

/*+
 * ============================================================================
 * = free - Return an allocated block of memory to the heap.                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Frees a block of memory that has been allocated from a heap. 
 *      The block is returned to the appropriate heap.
 *  
 *   COMMAND FMT: free_cmd 0 N 0 free
 *
 *   COMMAND FORM:
 *  
 *	free ( <address>... )
 *  
 *   COMMAND TAG: free_cmd 0 RXBZ free
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<address>... - Specifies an address (hex) or list of addresses of
 *		allocated block(s) to be returned to the heap.
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>alloc 200
 *      00FFFE00
 *      >>>free fffe00
 *      >>>free `alloc 10` `alloc 20` `alloc 30`
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *
 *	alloc, dynamic
 *
 * FORM OF CALL:
 *  
 *	free ( int argc, char *argv [] )
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int free_cmd (int argc, char *argv[]) {
	int	i;

	for (i=1; i<argc; i++) {
 	    free (xtoi (argv [i]));
	}
	return msg_success;
}
