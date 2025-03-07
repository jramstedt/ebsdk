
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
__attribute__((unused)) static const char *rcsid = "$Id: beep.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 * $Log: beep.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.11  1996/07/11  14:29:57  fdh
 * Removed msleep()... moved to another module.
 *
 * Revision 1.10  1996/05/07  23:52:13  fdh
 * Replaced WaitUs with usleep
 *
 * Revision 1.9  1996/02/21  21:42:53  fdh
 * Implemented BeepCode().
 *
 * Revision 1.8  1995/10/26  23:48:32  cruz
 * Changed the name of a variable to avoid conflict.
 *
 * Revision 1.7  1995/09/02  06:23:35  fdh
 * Included time.h
 *
 * Revision 1.6  1994/11/29  16:27:37  fdh
 * Disable speaker when not in use.
 *
 * Revision 1.5  1994/11/07  12:05:39  rusling
 * Now include system.h as well as lib.h
 *
 * Revision 1.4  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/07/13  14:19:39  fdh
 * Modified msleep to use WaitUs to more accurately control wait loops.
 * This way the beeps still work when compiled with optimizations.
 *
 * Revision 1.2  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.1  1993/06/08  19:56:19  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "lib.h"
#include "time.h"

void tone(int period,int duration)
{
  outportb(0x43, 0xb6);
  outportb(0x42, period & 0xff);
  outportb(0x42, (period >> 8) & 0xff);
  msleep(duration);
  outportb(0x43, 0xb2);
  outportb(0x42, 0);
  outportb(0x42, 0);
}

void Beep(int msec,int freq)
{
  freq = 1193180/freq;

  outportb(0x61, inportb(0x61)|0x03);  /* turn on gate and speaker data for counter 2 */
  tone(freq,msec);
  outportb(0x61, inportb(0x61)&(~0x03));  /* turn off gate and speaker data for counter 2 */
}

void BeepCode(int code)
{
  int count;
  int scale;

  scale = 1;
  while (scale*10 < code) scale = scale*10;

  while (code > 0) {
    count = code/scale;
    code = code - count*scale;
    while (count > 0) {
      Beep(100, 1000);
      if (--count == 0) break;
      msleep(300);
    }
    scale = scale/10;
    if (code == 0) break;
    msleep(600);
  }
}

