/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: memtest.c,v 1.4 1999/01/25 22:16:01 gries Exp $";
#endif

/*
 * $Log: memtest.c,v $
 * Revision 1.4  1999/01/25 22:16:01  gries
 * Remove the joke son
 *
 * Revision 1.3  1999/01/21 21:16:35  paradis
 * Added cixes for Visual C++
 * Added conditional compilation for DP264-specific stuff
 * Removed fpu.c from WIN32 builds
 *
 * Revision 1.2  1999/01/21 19:05:08  gries
 * First Release to cvs gries
 *
 * Revision 1.33  1998/06/26  18:44:41  gries
 * check for Control-C more often
 *
 * Revision 1.32  1997/10/08  21:04:05  pbell
 * Added a macro for the printf type size of the datatype used
 * in the memory test algorithm.
 *
 * Revision 1.31  1997/10/03  15:14:24  pbell
 * reinit the mempool and network buffers after memtest
 * finishes.
 *
 * Revision 1.30  1997/10/03  14:48:28  fdh
 * Modified to use a typdef and preprocessor definitions to
 * define the data type used in the memory test.
 *
 * Revision 1.29  1997/08/24  11:51:35  fdh
 * Added conditional to disable floating point.
 *
 * Revision 1.28  1997/08/01  19:11:28  pbell
 * Converted addresses from pointers to ul's so that NT builds can
 * test memory above 4gb.
 *
 * Revision 1.27  1997/06/11  18:45:40  fdh
 * Modified to provide defaults for memory range and increment
 * when values of zero are received.
 *
 * Revision 1.26  1997/06/09  15:44:47  fdh
 * Included "mcheck.h".
 *
 * Revision 1.25  1997/02/21  03:40:26  fdh
 * Report iterations pending...
 *
 * Revision 1.24  1996/07/12 20:45:55  fdh
 * Moved walking 1/0 tests to the end.
 *
 * Revision 1.23  1996/06/18  03:06:46  fdh
 * Included callback.h.
 *
 * Revision 1.22  1996/06/17  18:04:57  fdh
 * Modified to use UserPutChar instead of putChar.
 *
 * Revision 1.21  1996/05/22  20:53:08  fdh
 * Modified memtest to stride through memory to create cache
 * victims with each access to stress the cache more.
 * Modified to enable machine checks during test to report
 * corrected errors.
 *
 * Revision 1.20  1995/12/08  12:37:01  fdh
 * Force step, specified in bytes as inc, to be properly aligned.
 *
 * Revision 1.19  1995/12/07  23:46:25  cruz
 * Call netman_stop_all_devices instead of netman_stop_monitor_device
 * before doing memory test.
 * ..
 *
 * Revision 1.18  1995/10/26  23:43:42  cruz
 * added casting and included ether.h and console.h
 *
 * Revision 1.17  1995/10/20  13:52:21  fdh
 * Pass memtest command interations argument into memtest().
 * Keep/print a running total of errors across iterations.
 * Added check for Control-C to interrupt between each pass
 * through the memory range.
 *
 * Revision 1.16  1995/10/18  18:51:01  fdh
 * Print a little status indicator after the long memory test loops.
 *
 * Revision 1.15  1995/10/18  18:02:22  fdh
 * Stop Ethernet device to preempt DMA activity.
 *
 * Revision 1.14  1995/10/11  21:42:34  fdh
 * Modified to avoid the need for memory barriers.
 *
 * Revision 1.13  1995/09/12  21:36:40  fdh
 * Patch to avoid divide by zero case for memtest over small
 * ranges.
 *
 * Revision 1.12  1995/05/24  17:47:05  cruz
 * Added MB instruction after writes performed by the walking 1's
 * test to ensure that the reads do not find the data in the
 * write buffer.
 * Corrected print statements to prevent sign-extension in
 * LWs on a 32-bit compiler (NT).
 *
 * Revision 1.11  1994/11/07  12:05:39  rusling
 * Now include system.h as well as lib.h
 *
 * Revision 1.10  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.9  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.8  1994/04/02  06:16:06  fdh
 * Removed obsoleted function
 *
 * Revision 1.7  1994/03/23  21:57:29  fdh
 * Zeroed memory before doing uniqueness test.
 * Test for zero during the uniqueness test.
 *
 * Revision 1.6  1994/03/16  00:14:43  fdh
 * Moved some code from cmd.c to here.
 *
 * Revision 1.5  1994/03/13  16:19:23  fdh
 * Remove runtime computation.
 * Abort test when start range exceedes end range.
 * Return tested range when done.
 *
 * Revision 1.4  1994/01/26  22:46:22  fdh
 * Added timer to memory test.
 *
 * Revision 1.3  1993/10/23  12:48:37  fdh
 * Reduce error output page size.
 *
 * Revision 1.2  1993/10/15  23:03:33  fdh
 * Reset error count at appropriate time.
 *
 * Revision 1.1  1993/09/27  18:58:03  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "mon.h"
#include "lib.h"
#include "ether.h"
#include "console.h"
#include "callback.h"
#include "palcsrv.h"
#include "mcheck.h"
#include "div_table.h"

#ifndef MT_DATA_TYPE
#define MT_DATA_TYPE ul
#endif

#ifndef MTRead
#define MTRead ReadQ
#endif

#ifndef MTWrite
#define MTWrite WriteQ
#endif

#ifndef MTSeed
#define MTSeed 0x5a5a5a5a5a5a5a5a
#endif

typedef MT_DATA_TYPE mt_datum_t;

//#define PRINTF_mt_datum_t   "h"		/* us */
#define PRINTF_mt_datum_t   "l"		/* ui */
//#define PRINTF_mt_datum_t   PRINTF_sl_	/* ul */

#define MEG_BOUNDARY 0xfffff
#define MEMSTRIDE (MAXIMUM_SYSTEM_CACHE/sizeof(mt_datum_t))

extern char __start;
extern int byte_number_error_count[8];
extern int bank_number_error_count[4];
extern int dimm_number_error_count[16];
extern int corrected_error_count;
static int errcount;
int byte_number_error_count[8];
int bank_number_error_count[4];
int dimm_number_error_count[16];
static ul do_memtest( ul llim, ul hlim, int step, mt_datum_t seed);
static void test1( ul llim, ul hlim, int step, int test);
static void test2( ul llim, ul hlim, int step, mt_datum_t seed);
static void test3( ul llim, ul hlim, int step);
static int dump_error(ul p, mt_datum_t data, mt_datum_t pattern);

void memtest( ul min, ul max, ui inc, ui iterations, State_t McheckState, int drivers)
{
  ul sp;
  ul range;
  time_t runtime;
  int step,i;

  if (min == 0)
    min = (((ul) (&__start)+MEG_BOUNDARY+1) & ~MEG_BOUNDARY);

  if (max == 0)
    max = (mem_size - 1);

   if (max <= min) return;

  if(inc < sizeof(mt_datum_t))
    inc = sizeof(mt_datum_t);

  step = (inc & ~(sizeof(mt_datum_t)-1));   /* Force step, specified in bytes as
				       inc, to be properly aligned. */

  if (!drivers)
    netman_stop_all_devices();	/* Stop Ethernet devices */

  SetMcheck(McheckState);	/* Establish Machine Check State */

  errcount = 0;
  corrected_error_count = 0;
  for(i=0;i<8;i++)
    byte_number_error_count[i]= 0;
  for(i=0;i<4;i++)
    bank_number_error_count[i]= 0;
  for(i=0;i<16;i++)
    dimm_number_error_count[i]= 0;
  while (iterations-- > 0) {
    sp = getsp();
    printf("\nCurrent Stack Pointer: 0x%"PRINTF_sl_"x", sp);

    runtime = gettime();
    if ((sp >= min) && (sp <= max))
      {
	sp = ((sp>>13)<<13);
	range = do_memtest(min, sp, step, (mt_datum_t) MTSeed);
	sp = sp+(1ul<<14);
	if (max > sp)
	  {
	    printf("\n\n\t///////// Skipping over Stack /////////\n");
	    range += do_memtest(sp, max, step, (mt_datum_t) MTSeed);
	  }
      }
    else
      range = do_memtest(min, max, step, (mt_datum_t) MTSeed);

    runtime = gettime() - runtime;
    if (runtime < 1) runtime = 1;

#ifndef __NO_FLOATING_POINT
    printf("\n\n%d KB tested in %d seconds. (%3f KB/second)\n",
	   range/1024, runtime, (float)range/(runtime*1024));
#else
    printf("\n\n%d KB tested in %d seconds. (%d KB/second)\n",
	   range/1024, runtime, range/(runtime*1024));
#endif /* __NO_FLOATING_POINT */

	printf("\t %d Corrected Errors, %d Total Errors\n\n",
		corrected_error_count, corrected_error_count+errcount);

	for(i=0;i<8;i++)
		printf("\t Byte #%d error count \t%8x\n",i,byte_number_error_count[i]);
	for(i=0;i<4;i++)
		printf("\t Bank #%d error count \t%8x\n",i,bank_number_error_count[i]);
	for(i=0;i<16;i++)
		printf("\t Bank #%d Dimm #%d error count \t%8x\n",i>>2,i & 3,dimm_number_error_count[i]);

	if (iterations != 0)
		printf("\t %d iterations remaining\n", iterations);
  }

  printf("\t 0 %016lx \n",*(long *)(0x801a0000100ul));
  printf("\t 1 %016lx \n",*(long *)(0x801a0000140ul));
  printf("\t 2 %016lx \n",*(long *)(0x801a0000180ul));
  printf("\t 3 %016lx \n",*(long *)(0x801a00001c0ul));
  // reinitialize the memory pool after the test has stepped on it.
  init_storage_pool();
  // reinitialize the network management code.
  netman_setup();
}


static ul do_memtest( ul llim, ul hlim, int step, mt_datum_t seed)
{
  if (hlim <= llim) return(0);
//#ifdef gen_ecc
  test2(llim, hlim, step, seed);
  test2(llim, hlim, step, ~seed);
  test3(llim, hlim, step);
//#endif
  test1(llim, hlim, step, 1);
  test1(llim, hlim, step, 0);

  return( hlim-llim );
}


static void test1( ul llim, ul hlim, int step, int test)
{
  ul p;
  ul ps;
  mt_datum_t data;
  mt_datum_t pattern;
  mt_datum_t d;

  printf("\n\nWalking %d's ... range 0x%016"PRINTF_sl_"x:0x%016"PRINTF_sl_"x\n ",
	 test, llim, hlim);

  for (d = 0x0001000100010001ul; (d & 0xffff) != 0; d <<= 1) {
    pattern = test ? d : ~d;
    
    for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
      for (p=ps; p<hlim; p += MEMSTRIDE) {
	MTWrite( p, pattern );
      }
    }

    for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
      for (p=ps; p<hlim; p += MEMSTRIDE) {
	data = MTRead( p );
	if (data != pattern)
	  if (!dump_error(p, data, pattern))
	    return;
      }
    }
    UserPutChar((char)'.');
    if (CheckForChar(3)) return; /* Check for Control-C */
  }
}

static void test2( ul llim, ul hlim, int step, mt_datum_t seed)
{
  ul p;
  ul ps;
  mt_datum_t data;
  mt_datum_t pattern;
  
  printf("\n\nCheckerboard ...\n seed 0x%"PRINTF_mt_datum_t"x ... range 0x%016"PRINTF_sl_"x:0x%016"PRINTF_sl_"x",
	 seed, llim, hlim);

  pattern = seed;
  for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
    for (p=ps; p<hlim; p += MEMSTRIDE) {
      MTWrite( p, pattern );
    }
  }

  if (CheckForChar(3)) return; /* Check for Control-C */
  for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
    for (p=ps; p<hlim; p += MEMSTRIDE) {
      data = MTRead( p );
      if (data != pattern)
	if (!dump_error(p, data, pattern))
	  return;
    }
  }
}

static void test3( ul llim, ul hlim, int step)
{
  ul p;
  ul ps;
  mt_datum_t data;
  
  printf("\n\nUniqueness ... range 0x%016"PRINTF_sl_"x:0x%016"PRINTF_sl_"x", llim, hlim);

  for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
    for (p=ps; p<hlim; p += MEMSTRIDE) {
      MTWrite( p, 0 );
    }
  }

  if (CheckForChar(3)) return; /* Check for Control-C */
  for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
    for (p=ps; p<hlim; p += MEMSTRIDE) {
      MTWrite( p, (mt_datum_t)(p + inv[p & 0xff]) );
    }
  }

  if (CheckForChar(3)) return; /* Check for Control-C */
  for (ps=llim; ps<(llim+MEMSTRIDE); ps += step) {
    for (p=ps; p<hlim; p += MEMSTRIDE) {
      data = MTRead( p );
      if (data != (mt_datum_t)(p + inv[p & 0xff]))
        if (!dump_error(p, data, (mt_datum_t)(p + inv[p & 0xff])))
	  return;
    }
  }
}

#ifdef DP264
void print_dimm_number(long C_ADDR,int byte_number);
#endif

static int dump_error( ul p, mt_datum_t data, mt_datum_t pattern)
{
#ifdef DP264
  int i;
  ul mask_bits= 0xff;
  for (i=0;i<8;i++)
  {
     if ((data & mask_bits) != (pattern & mask_bits) )
       print_dimm_number(p,i);
     mask_bits<<= 8;
  }
#endif

  printf("\n* %016"PRINTF_sl_"x read: %16lx expected: %16lx",
	 p, data, pattern);
  if ((++errcount % 20) == 0) {
    return (kbdcontinue());
  }
  return TRUE;
}
