
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
__attribute__((unused)) static const char *rcsid = "$Id: time.c,v 1.1.1.1 1998/12/29 21:36:12 paradis Exp $";
#endif

/*
 * $Log: time.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:12  paradis
 * Initial CVS checkin
 *
 * Revision 1.7  1996/07/11  14:29:27  fdh
 * Added msleep().
 *
 * Revision 1.6  1995/12/06  19:36:47  cruz
 * Fixed localtime() to match the code change in time().
 *
 * Revision 1.5  1995/10/26  23:45:29  cruz
 * Added casting and changed the name of a variable
 *
 * Revision 1.4  1995/10/12  21:30:05  fdh
 * Added casts to pass proper datatypes.
 *
 * Revision 1.3  1995/09/02  07:04:17  fdh
 * Added extern declaration.
 *
 * Revision 1.2  1995/09/02  06:27:16  fdh
 * Added sleep() and usleep().
 *
 * Revision 1.1  1995/08/25  20:35:39  fdh
 * Initial revision
 *
 */

#include "lib.h"
#include "time.h"

/* return seconds since Jan 1, 1970 */
time_t gettime(void)
{
  return(time(NULL));
}

struct tm *localtime(const time_t *tp)
{
  static struct tm tms;  /* Note that this MUST be static */
  time_t cvt_time = *tp;

  tms.tm_sec  = cvt_time % 60;
  tms.tm_min  = (cvt_time / 60) % 60;
  tms.tm_hour = (cvt_time / (60 * 60)) % 24;
  tms.tm_mday = ((cvt_time / (24 * 60 * 60)) % 31) + 1;
  tms.tm_mon  = ((cvt_time / (31 * 24 * 60 * 60)) % 12) + 1;
  tms.tm_year = (cvt_time / (12 * 31 * 24 * 60 * 60));
  tms.tm_year += 70;
  return &tms;
}

void msleep(int ms)
{
  int i;
  for (i = 0; i < ms; i += 1) usleep(1000);
}

void usleep(ui usecs)
{
  wait_cycles(usecs*CPU_cycles_per_usecs);
}

unsigned int sleep(unsigned int seconds)
{
  while(seconds--) {
    usleep((unsigned)(1000*1000));
  }
  return 0;
}
