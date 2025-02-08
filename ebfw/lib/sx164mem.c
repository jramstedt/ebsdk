
/*****************************************************************************

Copyright � 1993, Digital Equipment Corporation, Maynard, Massachusetts. 

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
__attribute__((unused)) static const char *rcsid = "$Id: sx164mem.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $ (EB64 Debug Monitor)";
#endif

/*
 * $Log: sx164mem.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/05/01  21:13:02  pbell
 * Initial revision
 *
 * Revision 1.9  1996/05/22  21:13:53  fdh
 * Modified format passed to printf to print quadwords correctly.
 *
 * Revision 1.8  1995/10/31  18:40:50  cruz
 * Commented out some unused routines.
 *
 * Revision 1.7  1995/10/27  15:55:10  cruz
 * Commented out out_mem_gcr() since noone is currently using it.
 *
 * Revision 1.6  1995/10/27  15:44:30  cruz
 * Commented out out_mem_mcr() since noone is currently using it.
 *
 * Revision 1.5  1995/10/27  15:29:14  cruz
 * Removed extern reference to mem_size since it's now included
 * in lib.h
 *
 * Revision 1.4  1995/07/17  15:08:48  fdh
 * Moved include for dc21164.h into eb164mem.h inside
 * of MAKEDEPEND conditional.
 *
 * Revision 1.3  1995/04/21  17:41:31  cruz
 * Added casting operators and removed unused variables to
 * avoid warnings under NT.
 *
 * Revision 1.2  1995/04/20  16:36:23  cruz
 * Updated BC on and off commands
 *
 * Revision 1.1  1994/11/23  20:04:52  fdh
 * Initial revision
 *
 *
 */

#include "sx164mem.h"

ui in_mem_mcr(ui p)
{
  return(_IN_MEM_MCR(p));
}


ui in_mem_gcr(ui p)
{
  return(_IN_MEM_GCR(p));
}


static int banksize(int bcr_reg)
{
  int csr;
  int size;
  csr = in_mem_mcr(bcr_reg);
  if ((csr & MC_BCR_M_BANK_ENABLE) == 0) return(0);
  size = ((csr & MC_BCR_M_BANK_SIZE) >> MC_BCR_V_BANK_SIZE);
  if (size == 8) {
	  size = 2048;
  } else {
	  size = (1 << (7 - size)) * 8;
  }
  return(size*1024*1024);
}

void memdetect(void)
{
  mem_size = 0x0;
  
  mem_size += banksize(MC_BCR0);
  mem_size += banksize(MC_BCR1);
  mem_size += banksize(MC_BCR2);
  mem_size += banksize(MC_BCR3);
  mem_size += banksize(MC_BCR4);
  mem_size += banksize(MC_BCR5);
  mem_size += banksize(MC_BCR6);
  mem_size += banksize(MC_BCR7);
}

static void change_bcache_cfg (ul bc_ctl, ul bc_cfg)
{
#ifdef NOT_IN_USE
  ui mc_mcr, mc_gtr;

  mc_mcr = in_mem_mcr(MC_MCR);		// get current MCR value
  mc_gtr = in_mem_mcr(MC_GTR);		// get current GTR value

  if (bc_ctl & BC_M_BC_ENA) {
	  mc_mcr = mc_mcr | MC_MCR_M_OVERLAP_DISABLE | MC_MCR_M_BCACHE_ENABLE;
	  mc_gtr = (mc_gtr & ~MC_GTR_M_IDLE_BC_WIDTH) | (3 << MC_GTR_V_IDLE_BC_WIDTH);
  } else {
	  mc_mcr = mc_mcr & ~(MC_MCR_M_OVERLAP_DISABLE | MC_MCR_M_BCACHE_ENABLE);
	  mc_gtr = mc_gtr & ~MC_GTR_M_IDLE_BC_WIDTH;
  }
  printf("New BC_CTL = 0x%08lX  &  BC_CFG = 0x%08lX\n", bc_ctl, bc_cfg);
  cServe(bc_ctl, bc_cfg, CSERVE_K_WR_BCACHE, mc_mcr, mc_gtr);
#endif
} 

void wr_bcache (ui argc, ul arg1, ul arg2, ui select)
{
#ifdef NOT_IN_USE
  ul  bc_cfg, bc_cfg2;
  

  if (argc < 3) {        /* If only one argument is valid */
    if (select) {        /* if wbcfg */
      bc_cfg = arg1;
      bc_cfg2 = cServe(0, 0, CSERVE_K_RD_BCCFG2); 
    } else {
      bc_ctl = arg1;
      if (bc_ctl & BC_M_BC_ENA)
        bc_cfg = cServe(0, 0, CSERVE_K_RD_BCCFG);
      else
        bc_cfg = cServe(0, 0, CSERVE_K_RD_BCCFG_OFF);
    }
  } else {               /* If two arguments were specified */
    if (select) {        /* if wbcfg */
      bc_cfg = arg1;
      bc_ctl = arg2;
    } else {
      bc_ctl = arg1;
      bc_cfg = arg2;
    }
  }
  printf("Old BC_CTL = 0x%08lX  &  BC_CFG = 0x%08lX\n",
         cServe(0, 0, CSERVE_K_RD_BCCFG2),
         cServe(0, 0, CSERVE_K_RD_BCCFG));
  change_bcache_cfg (bc_ctl, bc_cfg);
#endif
}

