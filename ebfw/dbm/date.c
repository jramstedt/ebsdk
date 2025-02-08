
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
__attribute__((unused)) static const char *rcsid = "$Id: date.c,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $";
#endif

/*
 * $Log: date.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.21  1997/08/13  14:21:54  fdh
 * Added timeouts to RTC init flow.
 *
 * Revision 1.20  1996/05/22  22:18:43  fdh
 * Added commentary.
 *
 * Revision 1.19  1995/12/06  19:33:38  cruz
 * Fixed time() routine which miscomputed the number of seconds
 * since Jan 1, 1970 00:00:00 because it didn't take into account
 * that the months and days fields had a base of 1, instead of 0.
 * The solution was to subtract 1 from these two fields before
 * computing the number of seconds they contribute to the total.
 *
 * Revision 1.18  1995/10/26  22:42:50  cruz
 * Disabled variable DefaultDate
 *
 * Revision 1.17  1995/10/26  22:03:59  cruz
 * Performed some clean.  Removed extra code, fixed prototypes, etc
 *
 * Revision 1.16  1995/10/13  14:21:37  fdh
 * Modified to warn on "Bad Date" but not change it.
 *
 * Revision 1.15  1995/10/04  23:35:16  fdh
 * Updated local function prototypes.
 *
 * Revision 1.14  1995/09/26  23:35:55  fdh
 * Modified setDate() so that it no longer disabled
 * the periodic interrupts when setting the DSE,
 * 24-Hr, and Binary bits.  Created OutputDate().
 * Also made use of standard data structures.
 *
 * Revision 1.13  1995/08/23  14:10:00  fdh
 * Added return to time() and moved localtime() and
 * gettime() to lib/time.c.
 *
 * Revision 1.12  1995/08/22  23:48:12  cruz
 * Rewrote function time() to compute time in seconds since
 * Jan 1, 1970 (Coordinated Universal Time).
 * Wrote function localtime() to take the output from time()
 * and return a structure with the components of the date and
 * time easily accessible.
 *
 * Revision 1.11  1994/08/07  20:33:25  fdh
 * Removed unnecessary tracing.
 *
 * Revision 1.10  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.9  1994/07/22  21:03:33  fdh
 * Added prtrace.h and some tracing.
 *
 * Revision 1.8  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.7  1994/06/03  18:28:02  fdh
 * Corrected return value for time() and fixed up include declaration.
 *
 * Revision 1.6  1994/04/02  06:39:36  fdh
 * Added time() function which just call gettime()
 * for now.  time() should probably replace gettime().
 *
 * Revision 1.5  1994/01/19  10:40:08  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.4  1993/08/09  20:37:07  fdh
 * Access RTC at based address determined by global variable.
 *
 * Revision 1.3  1993/07/30  19:16:35  fdh
 * Insure that the RTC is enabled.
 *
 * Revision 1.2  1993/07/26  20:11:17  fdh
 * Change RTC base to 0x70 from 0x170
 *
 * Revision 1.1  1993/06/08  19:56:20  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "lib.h"
#include "mon.h"
#include "time.h"

#define	SECONDS	0x0
#define	MINUTES	0x2
#define	HOURS	0x4
#define	DAYS	0x7
#define	MONTHS	0x8
#define	YEARS	0x9
#define RTCA	0xA
#define RTCB	0xB
#define RTCC	0xC
#define RTCD	0xD

static char	month_list[] = {
	'J','a','n','\0','F','e','b','\0','M','a','r','\0','A','p','r','\0',
	'M','a','y','\0','J','u','n','\0','J','u','l','\0','A','u','g','\0',
	'S','e','p','\0','O','c','t','\0','N','o','v','\0','D','e','c','\0'
};

#define UPDATE_TIMEOUT	1000

/* Internal Function Prototypes */
static ub inRTC(int offset);
static void outRTC(int offset , ui data);
static void OutputDate(struct tm * time_s);

static ub inRTC(int offset)
{
  outportb(rtcBase, offset);
  return (inportb(rtcBase+1));
}

static void outRTC(int offset, ui data)
{
  outportb(rtcBase, offset);
  outportb(rtcBase+1, data);
}

/* return seconds since Jan 1, 1970 */
time_t time(time_t *out)
{
  int timeout;
  time_t curr_time;

  timeout = UPDATE_TIMEOUT;
  while((inRTC(RTCA) & 0x80) && timeout--)
    ;
  curr_time =  (time_t) inRTC(SECONDS);
  curr_time += (time_t) (inRTC(MINUTES) * 60);
  curr_time += (time_t) (inRTC(HOURS)   * 60 * 60);
  curr_time += (time_t) ((inRTC(DAYS)-1) * 24 * 60 * 60);
  curr_time += (time_t) ((inRTC(MONTHS)-1) * 31 * 24 * 60 * 60);
  curr_time += (time_t) ((inRTC(YEARS)-70) * 12 * 31 * 24 * 60 * 60);

  if (out != NULL) *out = curr_time;
  return curr_time;
}

static void OutputDate(struct tm *time_s)
{
  char *mos;

  if (time_s->tm_mon >= 1 && time_s->tm_mon <= 12)
    mos = month_list + 4 * (time_s->tm_mon - 1);
  else
    mos = "Invalid month";
  printf(mos);
  printf(" %2d %02d:%02d:%02d 19%02d\n",
	 time_s->tm_mday & 0xff,
	 time_s->tm_hour & 0xff,
	 time_s->tm_min  & 0xff,
	 time_s->tm_sec & 0xff,
	 time_s->tm_year & 0xff
	 );
}

void printDate(void)
{
  time_t time_in_sec;
  struct tm *time_s;

  time_in_sec = time(NULL);
  time_s = localtime(&time_in_sec);

  OutputDate(time_s);
}

void InitRTC(void)
{
  time_t time_in_sec;
  struct tm *time_s;
  int timeout;
  int bad;

  bad = 0;

/*
** Initialize the real-time clock (RTC)
**
** Index into RTC Control Register A to set periodic interrupt rate
** to 976.562 and set it running.
*/

  outRTC(RTCA, 0x26);		/* Insure that the RTC is enabled */

/*
** Index into RTC Control Register B and then enable periodic
** interrupts, Daylight Savings (DSE), 24-Hour Mode, and Binary
** mode.
*/

  outRTC(RTCB, 0x47);

  timeout = UPDATE_TIMEOUT;
  while((inRTC(RTCA) & 0x80) && timeout--)
    ;

  if ((inRTC(RTCD) & 0x80) == 0 ) {
    printf ("Invalid NVRAM: ");
    bad += 1;
  }
  
  time_in_sec = time(NULL);
  time_s = localtime(&time_in_sec);

  /* Check year */
  if(time_s->tm_year>99)
    bad += 1;
  /* Check month */
  if(time_s->tm_mon<1 || time_s->tm_mon>12)
    bad += 1;
  /* Check day */
  if(time_s->tm_mday<1 || time_s->tm_mday>31)
    bad += 1;
  /* Check hour */
  if(time_s->tm_hour>23)
    bad += 1;
  /* Check minutes */
  if(time_s->tm_min>59)
    bad += 1;
  /* Check seconds */
  if(time_s->tm_sec>59)
    bad += 1;

  if(bad > 0)  {
    printf("Bad date "); OutputDate(time_s);
#if 0
    setDate((ub *) DefaultDate);
    printf("Set to "); printDate();
#endif
  }
}

void setDate(ub *date)
{
  struct tm time_s;

  if(strlen((char *)date) != 12) {
    printf("Syntax error\n");
    return;
  }

  time_s.tm_year = date[0] - '0';
  time_s.tm_year = time_s.tm_year*10 + date[1] - '0';

  time_s.tm_mon = date[2] - '0';
  time_s.tm_mon = time_s.tm_mon*10 + date[3] - '0';

  time_s.tm_mday = date[4] - '0';
  time_s.tm_mday = time_s.tm_mday*10 + date[5] - '0';

  time_s.tm_hour = date[6] - '0';
  time_s.tm_hour = time_s.tm_hour*10 + date[7] - '0';

  time_s.tm_min = date[8] - '0';
  time_s.tm_min = time_s.tm_min*10 + date[9] - '0';

  time_s.tm_sec = date[10] - '0';
  time_s.tm_sec = time_s.tm_sec*10 + date[11] - '0';

  outRTC(RTCB, inRTC(RTCB)|0x7); /* Set DSE, 24-Hr, Binary */

  outRTC(SECONDS, (ui)time_s.tm_sec);
  outRTC(MINUTES, (ui)time_s.tm_min);
  outRTC(HOURS, (ui)time_s.tm_hour);
  outRTC(DAYS, (ui)time_s.tm_mday);
  outRTC(MONTHS, (ui)time_s.tm_mon);
  outRTC(YEARS, (ui)time_s.tm_year);
  
  outRTC(RTCD, 0x80);		/* Set Valid RAM Data & Time Flag*/
}
