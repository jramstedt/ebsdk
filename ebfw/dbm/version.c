
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
static char *rcsid = "$Id: version.c,v 1.2 1999/01/21 19:04:41 gries Exp $";
#endif

/*
 * $Log: version.c,v $
 * Revision 1.2  1999/01/21 19:04:41  gries
 * First Release to cvs gries
 *
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.21  1998/09/22  13:54:23  gries
 * new version number 3.1.6666
 *
 * Revision 1.20  1998/08/03  17:28:31  gries
 * new version #
 *
 * Revision 1.19  1997/10/03  20:17:01  pbell
 * Updated for Version 3.1
 *
 * Revision 1.18  1997/09/15  14:15:28  pbell
 * Changed version to 3.1 Beta
 *
 * Revision 1.17  1997/05/23  01:38:30  fdh
 * 2.2 Beta 3
 *
 * Revision 1.16  1997/02/21  00:51:10  fdh
 * Changed base version to "2.2 Beta 2"
 *
 * Revision 1.15  1996/08/13  18:48:32  fdh
 * Changed base version to 2.1
 *
 * Revision 1.14  1996/07/22  20:40:20  fdh
 * Changed base version to 2.1-B5.
 *
 * Revision 1.13  1996/05/22  22:23:13  fdh
 * Changed base version to 2.1-B4.
 *
 * Revision 1.12  1996/05/08  17:59:41  fdh
 * Changed base version to 2.1-B2A.
 *
 * Revision 1.11  1996/05/06  22:59:23  fdh
 * Changed base version to 2.1-B2.
 *
 * Revision 1.10  1996/04/12  20:43:08  fdh
 * Changed base version to 2.1-B1.
 *
 * Revision 1.9  1996/02/26  21:30:32  fdh
 * Changed base version to 2.1B0.
 *
 * Revision 1.8  1995/12/15  21:50:24  cruz
 * Added variable dbm_version for holding the version of the debug monitor.
 *
 * Revision 1.7  1995/10/31  18:51:41  cruz
 * Removed braces and keyword const from definition of
 * compile_date and compile_time.
 *
 * Revision 1.6  1995/02/27  19:19:33  fdh
 * Removed O/S conditionals for the version timestamp.
 *
 * Revision 1.5  1994/08/05  20:18:01  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.4  1994/06/14  10:06:48  rusling
 * Moved date and time to compile_date and compile_time to
 * avoid a clash with the time() library function.
 *
 * Revision 1.3  1994/03/17  12:16:01  rusling
 * Modified to work on WNT.
 *
 * Revision 1.2  1994/01/19  10:50:12  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.1  1993/06/08  19:56:43  fdh
 * Initial revision
 *
 */


char compile_date[] =  __DATE__;
char compile_time[] =  __TIME__;
char dbm_version[] = "3.2.0";
