
/*****************************************************************************

       Copyright 1993, 1994, 1995  Digital Equipment Corporation,
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
static char *rcsid = "$Id: iobusini.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 *  Initialize whatever system buses this device might have.
 *
 * $Log: iobusini.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1996/07/11  15:59:00  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "lib.h"
#include "pci.h"

void IOBusInit(void)
{
  SetHAE(0);
  ISABusInit();

#ifdef NEEDPCI
  PCIBusInit();
  PCIInit();
#endif
}
