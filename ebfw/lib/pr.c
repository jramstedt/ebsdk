
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
static char *rcsid = "$Id: pr.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: pr.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.8  1995/11/06  23:08:48  fdh
 * Removed unnecessary masks...
 *
 * Revision 1.7  1995/10/26  23:47:55  cruz
 * Fixed up prototypes.
 *
 * Revision 1.6  1995/03/06  05:43:18  fdh
 * Corrected data types to prevent sign extensions
 * from printing out inadvertenly.
 *
 * Revision 1.5  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.4  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.3  1994/06/03  18:51:45  fdh
 * Removed unnecessary include file (stdio.h).
 *
 * Revision 1.2  1994/01/19  10:40:08  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.1  1993/06/08  19:56:24  fdh
 * Initial revision
 *
 */

/* pr.c */

#include "lib.h"

void PQ(ul x)
{
  printf("%016lx", x);
}

void PL(ui x)
{
  printf("%08x", x);
}

void PW(uw x)
{
  printf("%04hx", x);
}

void PB(ub x)
{
  printf("%02x", x);
}

void PutSpace(void)
{
  printf(" ");
}

void PutCR(void)
{
  printf("\n");
}


