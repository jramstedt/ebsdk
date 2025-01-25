
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
static char *rcsid = "$Id: cxx.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * $Log: cxx.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.7  1997/04/24  21:15:50  pbell
 * Cleaned up warnings
 *
 * Revision 1.6  1997/04/10  17:09:11  fdh
 * Added the _WIN32 conditional.
 *
 * Revision 1.5  1997/02/24 22:12:15  fdh
 * Report calls to stubs using puts.
 *
 * Revision 1.4  1997/02/21  03:46:45  fdh
 * Added __cxx_gem_dispatch().
 *
 * Revision 1.3  1995/12/18  21:39:24  fdh
 * Added _purecall() stub.
 *
 * Revision 1.2  1995/11/10  18:37:25  cruz
 * Added return statement to a routine requiring one.
 *
 * Revision 1.1  1995/08/25  20:44:49  fdh
 * Initial revision
 *
 */

#include "lib.h"

int _purecall() {return 0;}		/* Visual C++ */

int __cxx_dispatch() {puts("__cxx_dispatch() called"); return 0;}
int __cxx_gem_dispatch() {puts("__cxx_gem_dispatch() called"); return 0;}

int __pure_virtual() {puts("__pure_virtual() called"); return 0;} /* GNU C++ */
