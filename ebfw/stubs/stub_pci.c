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
__attribute__((unused)) static const char *rcsid = "$Id: stub_pci.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * PCI access routine subs.
 *
 * $Log: stub_pci.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1996/08/16  11:32:59  fdh
 * Added PCIGetNumberOfBusses()
 *
 * Revision 1.1  1996/08/16  11:31:47  fdh
 * Initial revision
 *
 * Revision 1.1  1995/10/14  09:40:28  fdh
 * Initial revision
 *
 */

#include "lib.h"


ui PCIGetNumberOfBusses(void)
{
  return ((ui)0);
}

ub InPciCfgB(ui bus, ui dev, ui func, ui reg)
{
  ub data = (ub) -1;
#ifdef DEBUG_GETPCI
  printf(" prb: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
  return data;
}

uw InPciCfgW(ui bus, ui dev, ui func, ui reg)
{
  uw data = (uw) -1;
#ifdef DEBUG_GETPCI
  printf(" prw: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
  return data;
}

ui InPciCfgL(ui bus, ui dev, ui func, ui reg)
{
  ui data = (ui) -1;
#ifdef DEBUG_GETPCI
  printf(" prl: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
  return data;
}

void OutPciCfgB(ui bus, ui dev, ui func, ui reg, ub data)
{
#ifdef DEBUG_PUTPCI
  printf(" pwb: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
}

void OutPciCfgW(ui bus, ui dev, ui func, ui reg, uw data)
{
#ifdef DEBUG_PUTPCI
  printf(" pww: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
}

void OutPciCfgL(ui bus, ui dev, ui func, ui reg, ui data)
{
#ifdef DEBUG_PUTPCI
  printf(" pwl: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
}
