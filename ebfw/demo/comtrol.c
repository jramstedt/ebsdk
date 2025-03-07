
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
__attribute__((unused)) static const char *rcsid = "$Id: comtrol.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: comtrol.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1995/08/25  20:27:27  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.2  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:46  fdh
 * Initial revision
 *
 */

#include "lib.h"

#define	LDB	(slot|0xC8D)
#define	MBX	(slot|0xC90)

#define PATTERN 0x5a
#define BUF_SIZE 0x400

void main()
{
  int	i,x,v=1;
  char  ans;
  char  *buf1 = (char*)0x400000;
  char  *buf2 = (char*)0x500000;
  int   (*restart)() = (int (*)()) 0;
  int   xfer_address=0xc00;
  unsigned int slot;

  printf("Are you sure ? [Y]");
  ans = GetChar();
  printf("\nthe answer you typed was %c",ans);
  printf("running comtrol dma exerciser...\n");
  while (1) {
    slot = (3<<12);
/*
    for (v=2;v<4;v++) {
      slot = v<<12;
*/      
    for (i=0; i<BUF_SIZE; ++i) {
      buf1[i] = i;
      buf2[i] = 0;
    }
    outportl(MBX+0, (unsigned int)&buf1[0]);
    outportw(MBX+4,BUF_SIZE/2);
    outportw(MBX+0x06, xfer_address);
    outportb(MBX+0x08, (0x01<<4));	   /* write */
    outportb(MBX+0x09, 0x00);
    outportb(LDB, 0x01);
    while ((inportb(slot|0xc99)&0xff)!=0xAA);			/* spin a bit */
    outportl(MBX+0x00, (unsigned int)&buf2[0]);
    outportb(MBX+0x08, (0x02<<4));           /* read */
    outportb(MBX+0x09, 0x00);
    outportb(LDB, 0x01);
    while ((inportb(slot|0xc99)&0xff)!=0xAA);			/* spin a bit */
    for (x=0;x<BUF_SIZE;x++) {
      if ((buf1[x]&0xff)!=(buf2[x]&0xff)) {
	printf("xfer error address:%x exp: %x act: %x\n",
	       &buf2[x],buf1[x]&0xff,buf2[x]&0xff);
	(*(int (*)())0)();
      }
    }
    printf(".");
  }
}

