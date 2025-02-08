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
__attribute__((unused)) static const char *rcsid = "$Id: sum.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: sum.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1997/05/01  21:09:01  pbell
 * Cleaned up a warning on ReadB usage.
 *
 * Revision 1.1  1996/05/22  21:09:32  fdh
 * Initial revision
 *
 */

#include "lib.h"

int ChecksumMem(ul first, ul last)
{
    register ul i;
    register int ck;
    ck = 0;
    for (i = first; i <= last; i += 1)
      {
	if (ck & 1)
	  ck = (ck >>1) + 0x8000;
	else
	  ck = ck >>1;

	ck += ReadB(i);
	ck &= 0xffff;		/* Keep it within bounds. */
      }
    return ck;
}
