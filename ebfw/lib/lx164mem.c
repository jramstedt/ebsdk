
/*****************************************************************************

Copyright © 1993, Digital Equipment Corporation, Maynard, Massachusetts. 

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
__attribute__((unused)) static const char *rcsid = "$Id: lx164mem.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $ (EB64 Debug Monitor)";
#endif

/*
 * $Log: lx164mem.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/02/21  03:44:06  bissen
 * Initial revision
 *
 */

#include "lx164mem.h"

#ifdef NOT_IN_USE
void out_mem_mcr(ui p, ui d)
{
  _OUT_MEM_MCR(p, d);
}

void out_mem_gcr(ui p, ui d)
{
  _OUT_MEM_GCR(p, d);
}

#endif

ui in_mem_mcr(ui p)
{
  return(_IN_MEM_MCR(p));
}


ui in_mem_gcr(ui p)
{
  return(_IN_MEM_GCR(p));
}


static unsigned long banksize(const unsigned int bcr_reg)
{
  unsigned int csr;
  unsigned long size;
  csr = in_mem_mcr(bcr_reg);
  if ((csr & MC_BCR_M_BANK_ENABLE) == 0) return(0);
  size = ((csr & MC_BCR_M_BANK_SIZE) >> MC_BCR_V_BANK_SIZE);
  if (size == 8) {
	  size = 2048;
  } else {
	  // size = (1 << (7 - size)) * 8;
    size = 1024 >> size;
  }
  return(size * 1024 * 1024);
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
} 

void wr_bcache (ui argc, ul arg1, ul arg2, ui select)
{
  ul  bc_cfg, bc_ctl;
  

  if (argc < 3) {        /* If only one argument is valid */
    if (select) {        /* if wbcfg */
      bc_cfg = arg1;
      bc_ctl = cServe(0, 0, CSERVE_K_RD_BCCTL); 
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
         cServe(0, 0, CSERVE_K_RD_BCCTL),
         cServe(0, 0, CSERVE_K_RD_BCCFG));
  change_bcache_cfg (bc_ctl, bc_cfg);
}

#ifdef NOT_IN_USE
static int bc_orig_read = FALSE;
static ul bc_cfg_orig = 0;
static ul bc_ctl_orig = 0;

void DisableBCache(void)
{
  ul bc_ctl, bc_cfg_off, bc_ctl_off;
  int ipl_prev;
  
  if (!sysdata.valid)
    printf("Warning: Verify the current BC_CTL value.");

  if (!bc_orig_read)        /* Read the configuration upon powerup */
     {
	bc_cfg_orig =  cServe(0, 0, CSERVE_K_RD_BCCFG);
	bc_ctl_orig =  cServe(0, 0, CSERVE_K_RD_BCCTL);
	bc_orig_read = TRUE;
     }
 
  bc_ctl =  cServe(0, 0, CSERVE_K_RD_BCCTL); 
  printf("Old BC_CTL = 0x%08lX  &  BC_CFG = 0x%08lX\n",
         cServe(0, 0, CSERVE_K_RD_BCCTL),
         cServe(0, 0, CSERVE_K_RD_BCCFG));


  if (!(bc_ctl & BC_M_BC_ENA))
     {
	printf ("BCache already disabled.  Command aborted.\n");
	return;
     }
  
/*
 *  Insure that the memory and BCache are coherent.
 */
  ipl_prev = swpipl(7);  /* Disable interrupts */
  CleanBCache((ul)(MAXIMUM_SYSTEM_CACHE*2));

/*
 *  To disable the cache, we must write four registers (2 in CPU and 2
 *  in memory controller) and guarantee that no I-stream or D-stream
 *  references are going on.  This is best handled in assembly so we
 *  have added the WR_BCACHE CServe call to PALcode.  See change_bcache_cfg().
 *
 *  The procedure is as follows:
 *
 *	1) Obtain the BC Configuration register value which was
 *	   computed by the SROM and passed to us.
 *	2) Obtain the BC Control register value which was also
 * 	   computed by the SROM and clear the ENABLE bit.
 *	3) Perform a bcache configuration change.
 *	4) Sweep the BCache.
 */
  bc_cfg_off = cServe(0, 0, CSERVE_K_RD_BCCFG_OFF);
  bc_ctl_off = bc_ctl & ~(ui)BC_M_BC_ENA;
  change_bcache_cfg (bc_ctl_off, bc_cfg_off);
  CleanBCache((ul)(MAXIMUM_SYSTEM_CACHE*2));  
  swpipl(ipl_prev);  /* Re-enable interrupts */
}

void EnableBCache(void)
{
  ui bc_ctl;
  int ipl_prev;
  
  if (!sysdata.valid)
    printf("Warning: Verify the current BC_CTL value.");

  if (!bc_orig_read)        /* Read the configuration upon powerup */
     {
	bc_cfg_orig =  cServe(0, 0, CSERVE_K_RD_BCCFG);
	bc_ctl_orig =  cServe(0, 0, CSERVE_K_RD_BCCTL);
	bc_orig_read = TRUE;
     }

/*
 *  Enable BCache by writing to the BCCTL control
 *  and MCR registers.  Note that the BCache cannot be
 *  turned on if the system was not brought up with it on!
 *
 *  To enable the bcache must write four registers (2 in CPU and 2
 *  in memory controller) and guarantee that no I-stream or D-stream
 *  references are going on.  This is best handled in assembly so we
 *  have added the WR_BCACHE CServe call to PALcode.
 *
 *  The procedure is as follows:
 *
 *	1) Obtain the BC Configuration register value which was
 *	   computed by the SROM and passed to us.
 *	2) Obtain the BC Control register value which was also
 * 	   computed by the SROM and clear the ENABLE bit.
 *	3) Read the memory controller register and set the cache size
 *	   to zero.
 *	4) Read the acknowledge register in the memory controller and
 *	   clear the BCACHE VICTIM acknowledge bit. 
 */
  bc_ctl = (ui) cServe(0, 0, CSERVE_K_RD_BCCTL); 
  printf("Old BC_CTL = 0x%08lX  &  BC_CFG = 0x%08lX\n",
         (ui) cServe(0, 0, CSERVE_K_RD_BCCTL),
         (ui) cServe(0, 0, CSERVE_K_RD_BCCFG));
  if (bc_ctl & BC_M_BC_ENA)
     {
	printf ("BCache already enabled.  Command aborted.\n");
	return;
     }

  if (!(bc_ctl_orig & BC_M_BC_ENA))
     {
	printf ("The system was brought up with the BCache OFF and cannot be\n");
	printf ("turned on at this point.\n");
	return;	
     }
  
  ipl_prev = swpipl(7);  /* Disable interrupts */
  change_bcache_cfg(bc_ctl_orig, bc_cfg_orig);
  CleanBCache((ul)(MAXIMUM_SYSTEM_CACHE*2));  /* Insure that the memory and */
                                              /* BCache are coherent. */
  swpipl(ipl_prev);  /* Re-enable interrupts */
}
#endif
