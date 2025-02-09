
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
__attribute__((unused)) static const char *rcsid = "$Id: wait.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: wait.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.6  1995/10/31  19:00:13  cruz
 * Removed declarations that are now present in lib.h
 *
 * Revision 1.5  1995/09/02  06:19:23  fdh
 * Include time.h
 *
 * Revision 1.4  1995/08/25  20:27:27  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.3  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.2  1994/07/13  14:33:47  fdh
 * Modified to use WaitUs to more accurately control wait loops.
 * This way the wait loops still work when compiled with optimizations.
 *
 * Revision 1.1  1993/06/08  19:56:54  fdh
 * Initial revision
 *
 */

#include "lib.h"
#include "time.h"

void main()
{
  register int x,y,clock=1;

  printf("sysdata.valid: %d\n", sysdata.valid);
  printf("CPU_cycles_per_usecs: %d\n", CPU_cycles_per_usecs);
  printf("start\n");
  while (1) {
    for (x=0;x<1000;x++) {
      usleep(1000);
    }
    printf("clock %d\n",clock++);
  }
  printf("done\n");
}
