/*
 * file:	memtest_def.h
 *
 * Copyright (C) 1994 by
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
 *
 *
 * Abstract:	memtest data structure definition
 *
 * Author:	Chuck Olson (cto)
 *
 * Modifications:
 *
 *	cto	24-Jan-1994	Initial entry
 *
 */

#define	FV_PATTERN 0x55555555

#define	QSA     NUM_QUALS	/* Start address */
#define	QEA     NUM_QUALS + 1	/* End address */
#define	QL     	NUM_QUALS + 2	/* Length in bytes */
#define	QBS     NUM_QUALS + 3	/* block size in bytes */
#define	QF     	NUM_QUALS + 4	/* fast mode don't verify data */
#define	QM     	NUM_QUALS + 5	/* timer report pass time */
#define	QZ     	NUM_QUALS + 6	/* don't malloc or verify start address */
#define	QD     	NUM_QUALS + 7	/* Test pattern */         
#define	QH     	NUM_QUALS + 8	/* malloc from firware heap */
#define	QRS     NUM_QUALS + 9	/* Random seed */
#define	QRB     NUM_QUALS + 10	/* Random memory allocations */
#define	QI      NUM_QUALS + 11	/* Increment */
#define	QMB     NUM_QUALS + 12	/* Do mb in alpha memtest -f version */
#define QBA	NUM_QUALS + 13	/* Block address -- where block of data has
				   been deposited and set up */
#define MORE_QUALS 14

/* BCACHE block size is 16 longwords (64 bytes) 
				-- need to modify for Cobra if ever use */
#define BCACHE_BLOCK_LW 0x10	

#define DEF_LEN_SZ 8192

typedef struct externals_struct {               
		UINT    	len;	 	/* total #of bytes to test */
		UINT	     	start_addr;     /* start address */      
		UINT	     	end_addr;       /* end address */      
		UINT		block_size;	/* number of bytes to test */
		UINT		test_patt;	/* March test pattern to use */
		UINT		rand_seed;	/* Seed to use to obtain random
						   data pattern */
	    	int    		incr;	 	/* Increment */
		int		fast;	       	/* skip data compare */      
		int		timer;		/* Turn on time reporting */
		int		no_check;	/* Don't check address*/
		int		do_mb;		/* Do mb in -f alpha version */
						/* Array containing either user
						   entered of default block of
						   patterns */
		unsigned int	patt_block[BCACHE_BLOCK_LW+1];	
		int		already_freed;	/* A flag to tell rundown that
						   the memory was already freed
						   so don't try again */
		char		cpu_name[32];	/* Holds cpu name string */
} externals;                                                 
