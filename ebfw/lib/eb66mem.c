
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
static char *rcsid = "$Id: eb66mem.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 * $Log: eb66mem.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.13  1995/10/31  22:49:31  cruz
 * Commented out code not in use.
 *
 * Revision 1.12  1995/10/27  15:31:18  cruz
 * Removed extern reference to mem_size since it's now included
 * in lib.h
 *
 * Revision 1.11  1995/10/27  15:20:06  fdh
 * Print cache size when enabling bcache.
 *
 * Revision 1.10  1995/10/20  13:53:32  fdh
 * Changed check_mem_esr() prints.
 *
 * Revision 1.9  1994/11/18  19:08:15  fdh
 * Disable and enable interrupts while probing memory.
 *
 * Revision 1.8  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.7  1994/07/21  18:07:28  fdh
 * Implemented EnableBCache() and DisableBCache() routines.
 * Also created macros for accessing Memory Controller CSR's.
 *
 * Revision 1.6  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.5  1994/06/10  14:42:05  fdh
 * Modified algorithm used to detect memory configuration.
 * This purposely ignores the SROM firmware interface for determining
 * the memory configuration.  Since the configuration registers in the
 * memory controller are "write only", memory is probed to determine
 * the memory configuration the is setup in the SROM.
 *
 * Revision 1.4  1994/06/03  18:25:41  fdh
 * Changed to dereference 64 bit pointers properly when
 * built on NT.
 *
 * Revision 1.3  1994/05/17  20:31:30  fdh
 * Report pending Memory Controller Errors and clear them.
 * Modified to support detection of all of the supported
 * SIMM types using the presence detect signals.
 *
 * Revision 1.2  1993/11/05  08:29:29  fdh
 * Implemented EB66 specific memdetect()
 *
 * Revision 1.1  1993/09/30  22:16:10  fdh
 * Initial revision
 *
 */

#include "eb66mem.h"

static int validate_mem(int mem);
static int test_memory(ui address, ui test_data);

void out_mem_csr(ui p, ul d)
{
  _OUT_MEM_CSR(p, d);
}

ul in_mem_csr(ui p)
{
  return(_IN_MEM_CSR(p));
}

void memdetect(void)
{
  ui sizex, sizey;
  int ipl;

/*
 * Make sure interrupts are disabled
 * before detecting memory because
 * this can be expected to
 * cause interrupts (Machine Checks).
 * This precludes setting breakpoints
 * in this code under the remote debugger
 * until interrupts are re-enabled.
 */
  ipl = swpipl(7);
  
  check_mem_esr(0);
  
  sizey = BANKSIZE_MAX;
  
  do {
    sizex = sizey;
    
    do {
      mem_size = (sizex+sizey)*1024*1024;
      if (validate_mem(mem_size)) {
	swpipl(ipl);
	return;
      }
      sizex /= 2;
    } while (sizex>=BANKSIZE_MIN);
    
    sizey /= 2;
  } while (sizey>=BANKSIZE_MIN);
  
  mem_size = BANKSIZE_MIN*1024*1024;
  if (validate_mem(mem_size)) {
    swpipl(ipl);
    return;
  }
  mem_size = 0;
  swpipl(ipl);
  return;
}

static int validate_mem(int mem)
{
  ui save_data;

  PRTRACE3("Testing Memory Size: 0x%x (%dMB)\n", mem, mem/(1024*1024));
  
  save_data = ReadL((NONCACHEABLE|(mem-sizeof(ui))));

  if (!test_memory((NONCACHEABLE|(mem-sizeof(ui))), PATTERN)) {
    WriteL((NONCACHEABLE|(mem-sizeof(ui))), save_data); /* Put back original data */
    return (0);
  }

  if (!test_memory((NONCACHEABLE|(mem-sizeof(ui))), ~PATTERN)) {
    WriteL((NONCACHEABLE|(mem-sizeof(ui))), save_data); /* Put back original data */
    return (0);
  }
  
  if (!test_memory((NONCACHEABLE|(mem-sizeof(ui))), save_data)) {
    return (0);
  }
  
  return (1);
}

static int test_memory(ui address, ui test_data)
{
#ifdef PRTRACE
  ui scr;
#endif

  check_mem_esr(1);

  
  WriteL(address, test_data);

#ifdef PRTRACE
  if ((scr = ReadL(address)) != test_data) {
    printf(" Test failed, Address: %08x, Wrote %x, Read %x\n",
	   address, test_data, scr);
#else
  if (ReadL(address) != test_data) {
#endif
    return (0);
  }
  
#ifdef PRTRACE
  printf(" Tested Address: %08x, Wrote %x, Read %x\n",
	 address, test_data, scr);
#endif
  
  if (!check_mem_esr(1)) return (0);
  
  return (1);
}

int check_mem_esr(int silent)
{
  ul esr;
  
  esr = in_mem_csr(0x68);
  
  if (esr & MEM_RW1C) {
    if (!silent) {
      printf("\n   %s %s error",
	     ((esr&0x10)?"Memory":"BCache"),
	     ((esr&0x8)?"Write":"Read"));
      printf("  ESR:%08x", esr);
      if (esr & 1)
	printf("  EAR:%08x", in_mem_csr(0x70));
      printf("\n");
    }

    out_mem_csr(0x68, MEM_RW1C);
    return (0);
  }
  return (1);
}

void DisableBCache(void)
{
  int ipl;

  ipl = swpipl(7);  /* Disable interrupts */

/*
 *  Insure that the memory and BCache are coherent.
 */
  CleanBCache((ul)(MAXIMUM_SYSTEM_CACHE*2));

/*
 *  Disable BCache by writing to the Cache Register (CAR).
 */
  _OUT_MEM_CSR(CAR_OFFSET,
	       (ul)(_IN_MEM_CSR(CAR_OFFSET) & ~CAR_M_BCE));
  
  printf("...CAR = %X\n", _IN_MEM_CSR(CAR_OFFSET));
  
  swpipl(ipl);  /* Re-enable interrupts */
}

void EnableBCache(void)
{
  int ipl;
  ul value;

  ipl = swpipl(7);  /* Disable interrupts */

/*
 *  Enable BCache by writing to the Cache Register (CAR).
 */
  _OUT_MEM_CSR(CAR_OFFSET,
	       (ul)(_IN_MEM_CSR(CAR_OFFSET) | CAR_M_BCE));

/*
 *  Insure that the memory and BCache are coherent.
 */
  CleanBCache((ul)(MAXIMUM_SYSTEM_CACHE*2));

  value = _IN_MEM_CSR(CAR_OFFSET);
  printf("...CAR = %X \tCache Size: %d KB\n",
	 value,
	 64 * (1<<((value>>5)&7))); /* Cache size = 64 * 2**CAR[7:5] */
  
  swpipl(ipl);  /* Re-enable interrupts */
}
