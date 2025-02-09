
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
__attribute__((unused)) static const char *rcsid = "$Id: pic.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: pic.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1995/08/25  20:27:27  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.2  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:51  fdh
 * Initial revision
 *
 */

#include "lib.h"
#include "82c59a.h"

void main()
{
  int x;
  int vector;
  unsigned base,isr,irr;

  printf("pic testing\n");

  base = 0xA0;
  outportb(base+0,0x11);
  outportb(base+1,0x08);
  outportb(base+1,0x04);
  outportb(base+1,0x01);
  outportb(base+1,0xff);
  outportb(0x4D0,0xDE);

  outportb(base+0,0xA);		/* select the IRR */
  irr = inportb(base+0);
  printf("%x irr: %x\n",base,irr);

  outportb(base+0,0xB);		/* select the ISR */
  isr = inportb(base+0);
  printf("%x isr: %x\n",base,isr);

  if (irr) {
    outportb(base+0,0x20);	/* generate EOI */
    outportb(base+0,0xA);	/* select the IRR */
    irr = inportb(base+0);
    printf("%x irr: %x\n",base,irr);
    
    outportb(base+0,0xB);	/* select the ISR */
    isr = inportb(base+0);
    printf("%x isr: %x\n",base,isr);
    
    x = 1000;
    vector = inIack();
    while (--x);
    vector = inIack();
    printf("vector = %x\n",vector);
  }

  base = 0x20;
  outportb(base+0,0x11);
  outportb(base+1,0x08);
  outportb(base+1,0x04);
  outportb(base+1,0x01);
  outportb(base+1,0xff);
  outportb(0x4D0,0xF8);

  outportb(base+0,0xA);		/* select the IRR */
  irr = inportb(base+0);
  printf("%x irr: %x\n",base,irr);

  outportb(base+0,0xB);		/* select the ISR */
  isr = inportb(base+0);
  printf("%x isr: %x\n",base,isr);

  if (irr) {
    outportb(base+0,0x20);	/* generate EOI */
    outportb(base+0,0xA);	/* select the IRR */
    irr = inportb(base+0);
    printf("%x irr: %x\n",base,irr);
    
    outportb(base+0,0xB);	/* select the ISR */
    isr = inportb(base+0);
    printf("%x isr: %x\n",base,isr);
    
    x = 1000;
    vector = inIack();
    while (--x);
    vector = inIack();
    printf("vector = %x\n",vector);
  }
  (*(int(*)())0x3000000)();
}
