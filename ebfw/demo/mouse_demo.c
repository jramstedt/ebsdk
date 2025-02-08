
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
__attribute__((unused)) static const char *rcsid = "$Id: mouse_demo.c,v 1.1 1999/04/09 17:39:28 paradis Exp $";
#endif

/*
 * $Log: mouse_demo.c,v $
 * Revision 1.1  1999/04/09 17:39:28  paradis
 * Added "regtest" to default build for system exerciser
 * Fixed compiler incompatibility in loops1.c
 * Made cstrip a bit more verbose
 *
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1995/08/25  20:27:27  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.2  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:50  fdh
 * Initial revision
 *
 */




unsigned int test[5]={0x55,0,0xFA,0xAA,0};
main()
{
  unsigned int results[5];
  volatile int x;

  printf("jensen mouse test\n");

  outVti(0x170,0x6a);		/* put chip into ps/2 mode */
  x = inVti(0x171);
  outVti(0x170,0x6a);
  outVti(0x171,x&0xff);

  outVti(0x64,0xaa);		/* send test1 to keybrd cntrlr */
  x=10000;while(--x);

  results[0]=inVti(0x60);
  outVti(0x64,0xab);		/* send test2 to keybrd cntrlr */
  x=10000;while(--x);
  results[1]=inVti(0x60);

  outVti(0x64,0xae);		/* enable keyboard control */

  x=1000000;while(--x);
  outVti(0x64,0xa8);		/* enable mouse control */
  printf("testing mouse\n");
  x=1000000;while(--x);
  outVti(0x64,0xd4);		/* reset mouse, and test it */
  x=1000000;while(--x);
  outVti(0x60,0xff);
  x=1000000;while(--x);
  results[2]=inVti(0x60);
  x=10000000;while(--x);
  results[3]=inVti(0x60);
  x=10000000;while(--x);
  results[4]=inVti(0x60);
  printf("checking results: ");
  for (x=0;x<5;x++) {
    if ((test[x]&0xff)!=(results[x]&0xFF)) {
      printf("\nerror testing mouse, exp: %02x act: %02x\n",
	     test[x],results[x]);
      while(1);
    }
  }
  printf("results are correct ! mouse test passed\n\n");
  while(1);
}
