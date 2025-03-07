
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
__attribute__((unused)) static const char *rcsid = "$Id: clubsmck.c,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $";
#endif

/*
 * $Log: clubsmck.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1998/08/21  20:11:34  jerzy
 * Initial revision
 *
 * Revision 1.3  1998/08/03  17:17:00  gries
 * Put more better in Machine Check
 *
 * Revision 1.2  1997/12/15  20:52:49  pbell
 * Updated for dp264.
 *
 */

#include "lib.h"
#include "mon.h"
#include "mcheck.h"

#ifndef MAKEDEPEND
#include "paldata.h"		/* For logout area */
#include "palcsrv.h"
#endif

void DumpLogout(LogoutFrame_t *Frame)
{
  printf("MCHK_Code \t%d\n",Frame->MCHK_Code);
  printf("MCHK_Frame_Rev \t%d\n",Frame->MCHK_Frame_Rev);
  printf("I_STAT \t%016lx\n",Frame->I_STAT);
  printf("DC_STAT \t%016lx\n",Frame->DC_STAT);
  printf("C_ADDR \t%016lx\n",Frame->C_ADDR);
  printf("DC1_SYNDROME \t%016lx\n",Frame->DC1_SYNDROME);
  printf("DC0_SYNDROME \t%016lx\n",Frame->DC0_SYNDROME);
  printf("C_STAT \t%016lx\n",Frame->C_STAT);
  printf("C_STS \t%016lx\n",Frame->C_STS);
  printf("RESERVED0 \t%016lx\n",Frame->RESERVED0);
  printf("EXC_ADDR \t%016lx\n",Frame->EXC_ADDR);
  printf("IER_CM \t%016lx\n",Frame->IER_CM);
  printf("ISUM \t%016lx\n",Frame->ISUM);
  printf("MM_STAT \t%016lx\n",Frame->MM_STAT);
  printf("PAL_BASE \t%016lx\n",Frame->PAL_BASE);
  printf("I_CTL \t%016lx\n",Frame->I_CTL);
  printf("PCTX \t%016lx\n",Frame->PCTX);

  Beep(100,1000);
}

void ParseLogout(LogoutFrame_t *Frame)
{
  printf("MCHK_Code \t%d\n",Frame->MCHK_Code);
  printf("MCHK_Frame_Rev \t%d\n",Frame->MCHK_Frame_Rev);
  printf("I_STAT \t%016lx\n",Frame->I_STAT);
  printf("DC_STAT \t%016lx\n",Frame->DC_STAT);
  printf("C_ADDR \t%016lx\n",Frame->C_ADDR);
  printf("DC1_SYNDROME \t%016lx\n",Frame->DC1_SYNDROME);
  printf("DC0_SYNDROME \t%016lx\n",Frame->DC0_SYNDROME);
  printf("C_STAT \t%016lx\n",Frame->C_STAT);
  printf("C_STS \t%016lx\n",Frame->C_STS);
  printf("RESERVED0 \t%016lx\n",Frame->RESERVED0);
  printf("EXC_ADDR \t%016lx\n",Frame->EXC_ADDR);
  printf("IER_CM \t%016lx\n",Frame->IER_CM);
  printf("ISUM \t%016lx\n",Frame->ISUM);
  printf("MM_STAT \t%016lx\n",Frame->MM_STAT);
  printf("PAL_BASE \t%016lx\n",Frame->PAL_BASE);
  printf("I_CTL \t%016lx\n",Frame->I_CTL);
  printf("PCTX \t%016lx\n",Frame->PCTX);

  outLed(0xD0);
}

void DumpShortLogout(LogoutFrame_t *Frame)
{
  printf("MCHK_Code \t%d\n",Frame->MCHK_Code);
  printf("MCHK_Frame_Rev \t%d\n",Frame->MCHK_Frame_Rev);
  printf("I_STAT \t%016lx\n",Frame->I_STAT);
  printf("DC_STAT \t%016lx\n",Frame->DC_STAT);
  printf("C_ADDR \t%016lx\n",Frame->C_ADDR);
  printf("DC1_SYNDROME \t%016lx\n",Frame->DC1_SYNDROME);
  printf("DC0_SYNDROME \t%016lx\n",Frame->DC0_SYNDROME);
  printf("C_STAT \t%016lx\n",Frame->C_STAT);
  printf("C_STS \t%016lx\n",Frame->C_STS);
  printf("RESERVED0 \t%016lx\n",Frame->RESERVED0);
  printf("EXC_ADDR \t%016lx\n",Frame->EXC_ADDR);
  printf("IER_CM \t%016lx\n",Frame->IER_CM);
  printf("ISUM \t%016lx\n",Frame->ISUM);
  printf("MM_STAT \t%016lx\n",Frame->MM_STAT);
  printf("PAL_BASE \t%016lx\n",Frame->PAL_BASE);
  printf("I_CTL \t%016lx\n",Frame->I_CTL);
  printf("PCTX \t%016lx\n",Frame->PCTX);

  outLed(0xD1);
}

void SetMcheckSystem(State_t state)
{
  outLed(0xD2);
}
