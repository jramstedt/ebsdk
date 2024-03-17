#ifndef __TIME_H_LOADED
#define __TIME_H_LOADED
/*****************************************************************************

       Copyright 1993, 1994, 1995 Digital Equipment Corporation,
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

/*
 *  $Id: time.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $
 */

/*
 * $Log: time.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.7  1996/05/07  23:57:42  fdh
 * Removed WaitUs macro.
 *
 * Revision 1.6  1995/10/26  23:54:47  cruz
 * Changed the type of elements of the tm structure to time_t
 * from ui.
 *
 * Revision 1.5  1995/09/02  07:01:53  fdh
 * Returned _TIME_T conditional around declaration
 * for time_t.
 *
 * Revision 1.4  1995/09/02  06:26:26  fdh
 * Added declarations for sleep() and usleep().
 * Turned WaitUs into a macro which aliases usleep().
 *
 * Revision 1.3  1995/08/25  19:37:47  fdh
 * Added prototypes for time() and localtime().
 *
 * Revision 1.2  1995/08/22  23:41:55  fdh
 * Fixed a minor typo. A semicolon was missing.
 *
 * Revision 1.1  1995/08/22  23:16:49  cruz
 * Initial revision
 *
 *
 */

#ifndef _TIME_T
#define _TIME_T
typedef ul time_t;
#endif

struct tm {
    time_t tm_sec;
    time_t tm_min;
    time_t tm_hour;
    time_t tm_mday;
    time_t tm_mon;
    time_t tm_year;
};



extern time_t time(time_t *out);
extern struct tm *localtime(const time_t *tp);
extern void usleep( unsigned int usecs );
extern unsigned int sleep(unsigned int seconds);

#endif /* __TIME_H_LOADED */


