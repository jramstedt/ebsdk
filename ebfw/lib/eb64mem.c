
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
static char *rcsid = "$Id: eb64mem.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 * $Log: eb64mem.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.9  1996/05/09  19:01:21  fdh
 * Restore previous ipl after disabling interrupts temporarily.
 *
 * Revision 1.8  1995/10/27  15:30:00  cruz
 * Removed extern reference to mem_size since it's now included
 * in lib.h
 *
 * Revision 1.7  1994/08/09  08:03:26  fdh
 * Properly cast data type in _OUT_SCTL().
 *
 * Revision 1.6  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.5  1994/07/21  18:08:01  fdh
 * Implemented EnableBCache() and DisableBCache() routines.
 *
 * Revision 1.4  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.3  1993/09/30  22:07:25  fdh
 * Memory is initialized by the SROM.  The memory size is placed
 * into the system control register to make the memory sub-system
 * work properly. By simply reading this value from the register
 * this routine is greatly simplified.
 *
 * Revision 1.2  1993/08/03  13:00:18  fdh
 * Some 16MB/4Mx36 SIMMs don't generate the expected
 * PD signals.  To work around this the 16MB SIMM
 * jumper is used to force the selection of 16MB
 * SIMMs.  Users must remember to remove the jumper
 * if another SIMM type is installed.
 *
 * Revision 1.1  1993/06/08  19:56:23  fdh
 * Initial revision
 *
 */

#include "eb64mem.h"


void memdetect(void)
{
  ui sctl;
  
  /* Read System Register */
  sctl = (insctl()>>22)&0x7;
  mem_size = (4*(1<<sctl)) * 1024 * 1024;
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
   *  and SCTL registers.
   */
  cServe(cServe(0,0,CSERVE_K_RD_BIU) & ~(BIU_M_BC_ENA),
	 0, CSERVE_K_WR_BIU);
  
  CleanBCache((ul)(MAXIMUM_SYSTEM_CACHE*2));
  _OUT_SCTL((ui)(_IN_SCTL & ~SCTL_M_BCE));
  
  printf("...Biu = %X\n", cServe(0, 0, CSERVE_K_RD_BIU));
  printf("...SCTL = %X\n", _IN_SCTL);
  
  swpipl(ipl_state);  /* Restore IPL */
}

void EnableBCache(void)
{
  int ipl_state;
  if (!sysdata.valid)
    printf("Warning: Verify the current BIU_CTL value.");

  ipl_state = swpipl(7);  /* Disable interrupts */
  
  /*
   *  Enable BCache by writing to the BIU control
   *  and SCTL registers.
   */
  _OUT_SCTL((ui)(_IN_SCTL | SCTL_M_BCE));
  
  cServe(cServe(0,0,CSERVE_K_RD_BIU) | BIU_M_BC_ENA,
	 0, CSERVE_K_WR_BIU);
  
  /*
   *  Insure that the memory and BCache are coherent.
   */
  CleanBCache((ul)(MAXIMUM_SYSTEM_CACHE*2));
  
  printf("...Biu = %X\n", cServe(0, 0, CSERVE_K_RD_BIU));
  printf("...SCTL = %X\n", _IN_SCTL);
  
  swpipl(ipl_state);  /* Restore IPL */
}
