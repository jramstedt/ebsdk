
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
__attribute__((unused)) static const char *rcsid = "$Id: eb164mem.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $ (EB64 Debug Monitor)";
#endif

/*
 * $Log: eb164mem.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
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

#include "eb164mem.h"

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


static int banksize(int bcr, int mba_reg)
{
  int csr;
  int size;
  csr = in_mem_mcr(mba_reg);
  if ((csr & MC_MBA_M_S0_VALID) == 0) return(0);
  size = (((csr & MC_MBA_M_MASK) >> MC_MBA_V_MASK) + 1) * 16;
  if (in_mem_mcr (bcr) & MC_MCR_M_MEM_SIZE) size *= 2;
  return(size*1024*1024);
}

void memdetect(void)
{
  mem_size = 0x0;
  
  mem_size += banksize(MC_MCR,MC_MBA0);
  mem_size += banksize(MC_MCR,MC_MBA2);
  mem_size += banksize(MC_MCR,MC_MBA4);
  mem_size += banksize(MC_MCR,MC_MBA6);
  mem_size += banksize(MC_MCR,MC_MBA8);
  mem_size += banksize(MC_MCR,MC_MBAA);
  mem_size += banksize(MC_MCR,MC_MBAC);
  mem_size += banksize(MC_MCR,MC_MBAE);
}

static void change_bcache_cfg (ul bc_ctl, ul bc_cfg)
{
  ui mc_mcr, mc_ack;
  ui cache_size;

  mc_mcr = in_mem_mcr(MC_MCR) & ~(ui)MC_MCR_M_CACHE_SIZE;
  mc_ack = in_mem_gcr(MC_CACK_EN) & ~(ui)MC_CACK_EN_M_BC_VICTIM;
  if (bc_ctl & BC_M_BC_ENA) {
    cache_size = (ui) ((bc_cfg & BC_M_SIZE) >> BC_V_SIZE) << MC_MCR_V_CACHE_SIZE;
    mc_mcr |= cache_size;
    mc_ack |= MC_CACK_EN_M_BC_VICTIM;
  }
  printf("New BC_CTL = 0x%08lX  &  BC_CFG = 0x%08lX\n", bc_ctl, bc_cfg);
  printf("    CIA_CACK_EN = 0x%X  &  CIA_MCR = 0x%08X\n", mc_ack, mc_mcr);
  cServe(bc_ctl, bc_cfg, CSERVE_K_WR_BCACHE, mc_ack, mc_mcr);
  mc_ack = in_mem_gcr(MC_CACK_EN);  /* Read to make sure it gets there */
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
