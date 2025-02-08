
/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
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
__attribute__((unused)) static const char *rcsid = "$Id: isa_dma.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: isa_dma.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1996/08/06  22:09:31  fdh
 * Added mode setting to Enable...
 *
 * Revision 1.2  1996/05/26  20:19:03  fdh
 * Included lib.h
 *
 * Revision 1.1  1996/05/26  20:04:05  fdh
 * Initial revision
 *
 */

#include "lib.h"
#include "isa_bus.h"

void EnableIsaDmaChannel(int channel, int mode)
{
  if (channel > 4) {
    outportb(IO_82C37_DMA2_MODE, (mode<<6) | channel-4);
    outportb(IO_82C37_DMA2_WRMSK, channel-4);
  }
  else {
    outportb(IO_82C37_DMA1_MODE, (mode<<6) | channel);
    outportb(IO_82C37_DMA1_WRMSK, channel);
  }
}
