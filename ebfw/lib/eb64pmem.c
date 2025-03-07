
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
__attribute__((unused)) static const char *rcsid = "$Id: eb64pmem.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 * $Log: eb64pmem.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.9  1996/05/09  19:03:04  fdh
 * Changed ipl to ipl_state to avoid a name conflict.
 *
 * Revision 1.8  1995/10/31  22:50:10  cruz
 * Commented out code not in use.
 *
 * Revision 1.7  1995/10/27  15:30:34  cruz
 * Removed extern reference to mem_size since it's now included
 * in lib.h
 *
 * Revision 1.6  1994/11/18  19:08:52  fdh
 * Restore ipl to previous level after sweeping through
 * the cache.
 *
 * Revision 1.5  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.4  1994/07/26  14:45:28  fdh
 * Corrected data types used with _IN_MEM_CSR().
 *
 * Revision 1.3  1994/07/21  18:05:41  fdh
 * Implemented EnableBCache() and DisableBCache() routines.
 * Also created macros for accessing Memory Controller CSR's.
 *
 * Revision 1.2  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.1  1993/09/30  22:11:35  fdh
 * Initial revision
 *
 */

#include "eb64pmem.h"

static int banksize(int bcr, int bar);

#ifdef NOT_IN_USE
void out_mem_csr(ui p, ui d)
{
  _OUT_MEM_CSR(p, d);
}
#endif

ui in_mem_csr(ui p)
{
  return(_IN_MEM_CSR(p));
}

void memdetect(void)
{
  mem_size = 0x0;
  
  /*
   * A check should be added to warn if banks
   * are configured on top of one another.
   */

  /* BANK 0 */
  mem_size += banksize(0xA00, 0x800);

  /* BANK 1 */
  mem_size += banksize(0xA20, 0x820);
  
  return;
}

static int banksize(int bcr, int bar)
{
  int csr;
  int size;

  csr = in_mem_csr(bcr);
  if ((csr&1) == 0) return(0);
  size = 1 << (10-((csr>>1)&7));
  return(size*1024*1024);
}

void DisableBCache(void)
{
  int ipl_state;

  if (!sysdata.valid)
    printf("Warning: Verify the current BIU_CTL value.");

  ipl_state = swpipl(7);  /* Disable interrupts */

/*
 *  Insure that the memory and BCache are coherent.
 */
  CleanBCache((ul)(MAXIMUM_SYSTEM_CACHE*2));

/*
 *  Disable BCache by writing to the BIU control
 *  and GCR registers.
 */
  cServe(cServe(0,0,CSERVE_K_RD_BIU) & ~(BIU_M_BC_ENA),
	 0, CSERVE_K_WR_BIU);
  
  CleanBCache((ul)(MAXIMUM_SYSTEM_CACHE*2));
  _OUT_MEM_CSR(GCR_OFFSET,
	       (_IN_MEM_CSR(GCR_OFFSET) & ~(ui)GCR_M_BCE));
  
  printf("...Biu = %X\n", cServe(0, 0, CSERVE_K_RD_BIU));
  printf("...GCR = %04X\n", (uw)_IN_MEM_CSR(GCR_OFFSET));
  
  swpipl(ipl_state);  /* Re-enable interrupts */
}

void EnableBCache(void)
{
  int ipl_state;

  if (!sysdata.valid)
    printf("Warning: Verify the current BIU_CTL value.");

  ipl_state = swpipl(7);  /* Disable interrupts */

/*
 *  Enable BCache by writing to the BIU control
 *  and GCR registers.
 */
  _OUT_MEM_CSR(GCR_OFFSET,
	       (_IN_MEM_CSR(GCR_OFFSET) & ~(ui)GCR_M_BCE));
  
  cServe(cServe(0,0,CSERVE_K_RD_BIU) | BIU_M_BC_ENA,
	 0, CSERVE_K_WR_BIU);

/*
 *  Insure that the memory and BCache are coherent.
 */
  CleanBCache((ul)(MAXIMUM_SYSTEM_CACHE*2));
  
  printf("...Biu = %X\n", cServe(0, 0, CSERVE_K_RD_BIU));
  printf("...GCR = %04X\n", (uw)_IN_MEM_CSR(GCR_OFFSET));
  
  swpipl(ipl_state);  /* Re-enable interrupts */
}
