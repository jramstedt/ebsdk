
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
static char *rcsid = "$Id: yellow.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: yellow.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.10  1996/05/22  22:30:43  fdh
 * Removed local msleep definition.
 *
 * Revision 1.9  1995/09/02  06:20:36  fdh
 * Included time.h
 *
 * Revision 1.8  1995/08/25  20:27:27  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.7  1994/11/29  16:27:21  fdh
 * Disable speaker when not in use.
 *
 * Revision 1.6  1994/11/07  12:55:43  rusling
 * Now include system.h as well as lib.h.
 *
 * Revision 1.5  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.4  1994/07/13  14:33:47  fdh
 * Modified msleep to use WaitUs to more accurately control wait loops.
 * This way the beeps still work when compiled with optimizations.
 *
 * Revision 1.3  1994/06/21  10:24:08  rusling
 * Now include lib.h for definitions.
 *
 * Revision 1.2  1993/06/17  20:33:06  fdh
 * Loop 3 times...
 *
 * Revision 1.1  1993/06/08  19:56:55  fdh
 * Initial revision
 *
 */



/* yellow.c
   Last modified on August 1, 1991 by stewart
 */

#include "system.h"
#include "lib.h"
#include "time.h"

void yellow();


void yellow_tone(int period, int level, int time)
{
/*  printf("period %08x time %08x\n", period, time); */
  outportb(0x43, 0xb6);
  outportb(0x42, period & 0xff);
  outportb(0x42, (period >> 8) & 0xff);
  msleep(time);
  outportb(0x43, 0xb2);
  outportb(0x42, 0);
  outportb(0x42, 0);
}

/*
#define f4e  (6250000 / 706)
#define f6d  (6250000 / 585)
#define f82  (6250000 / 658)
#define f86  (6250000 / 495)
#define f8b  (6250000 / 397)
#define f9c  (6250000 / 331)
#define fa6  (6250000 / 528)
#define fe3  (6250000 / 353)
#define ff6  (6250000 / 440)
*/
#define f4e  (625000 / 706)
#define f6d  (625000 / 585)
#define f82  (625000 / 658)
#define f86  (625000 / 495)
#define f8b  (625000 / 397)
#define f9c  (625000 / 331)
#define fa6  (625000 / 528)
#define fe3  (625000 / 353)
#define ff6  (625000 / 440)

void yellow()
{
/*
  int f4e = 706;
  int f6d = 585;
  int f82 = 658;
  int f86 = 495;
  int f8b = 397;
  int f9c = 331;
  int fa6 = 528;
  int fe3 = 353;
  int ff6 = 440;
*/
  int tbase = 18; /* was 21 */

  outportb(0x61, inportb(0x61)|0x03);  /* turn on gate and speaker data for counter 2 */
  yellow_tone(f8b,-9, tbase*10);
  yellow_tone(fe3,-9, tbase*10);
  yellow_tone(f9c,-9, tbase*20);
  yellow_tone(f8b,-9, tbase*20);
  yellow_tone(f8b,-9, tbase*30);
  yellow_tone(f8b,-9, tbase*10);
  yellow_tone(ff6,-9, tbase*20);
  yellow_tone(f8b,-9, tbase*36);
  msleep ( tbase * 4);
  yellow_tone(fe3,-9, tbase*20);
  yellow_tone(f9c,-9, tbase*20);
  yellow_tone(f8b,-9, tbase*20);
  yellow_tone(fa6,-9, tbase*30);
  yellow_tone(f6d,-9, tbase*10);
  yellow_tone(f82,-9, tbase*54);
  msleep ( tbase * 6);
  yellow_tone(f82,-9, tbase*20);
  yellow_tone(f82,-9, tbase*20);
  yellow_tone(f8b,-9, tbase*20);
  yellow_tone(f8b,-9, tbase*20);
  yellow_tone(f82,-9, tbase*20);
  yellow_tone(f82,-9, tbase*20);
  yellow_tone(f6d,-9, tbase*38);
  msleep ( tbase * 2);
  yellow_tone(fa6,-9, tbase*20);
  yellow_tone(f86,-9, tbase*20);
  yellow_tone(fa6,-9, tbase*20);
  yellow_tone(f6d,-9, tbase*30);
  yellow_tone(f82,-9, tbase*10);
  yellow_tone(f6d,-9, tbase*52);
  msleep ( tbase * 8);
  yellow_tone(f8b,-9, tbase*10);
  yellow_tone(fe3,-9, tbase*10);
  yellow_tone(f9c,-9, tbase*20);
  yellow_tone(f8b,-9, tbase*20);
  yellow_tone(f8b,-9, tbase*30);
  yellow_tone(f8b,-9, tbase*10);
  yellow_tone(ff6,-9, tbase*20);
  yellow_tone(f8b,-9, tbase*28);
  msleep ( tbase * 4);
  yellow_tone(fe3,-9, tbase*8);
  yellow_tone(f9c,-9, tbase*20);
  yellow_tone(f8b,-9, tbase*20);
  yellow_tone(fa6,-9, tbase*30);
  yellow_tone(f6d,-9, tbase*10);
  yellow_tone(f82,-9, tbase*54);
  msleep ( tbase * 6);
  yellow_tone(f8b,-9, tbase*20);
  yellow_tone(f8b,-9, tbase*20);
  yellow_tone(f4e,-9, tbase*20);
  yellow_tone(f4e,-9, tbase*30);
  yellow_tone(f4e,-9, tbase*10);
  yellow_tone(f4e,-9, tbase*20);
  yellow_tone(f82,-9, tbase*38);
  msleep ( tbase * 2);
  yellow_tone(f6d,-9, tbase*20);
  yellow_tone(fa6,-9, tbase*20);
  yellow_tone(f8b,-9, tbase*20);
  yellow_tone(f82,-9, tbase*30);
  yellow_tone(f6d,-9, tbase*10);
  yellow_tone(fa6,-9, tbase*60);
  outportb(0x61, inportb(0x61)&(~0x03));  /* turn off gate and speaker data for counter 2 */
}

void main()
{
  int i;

  printf("Speaker demo. Playing...\n\t\"The Yellow Rose of Texas\"\n");

  for (i=0;i<3;i++)
  {
    yellow();
  }
}
