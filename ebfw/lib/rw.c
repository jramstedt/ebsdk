
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
__attribute__((unused)) static const char *rcsid = "$Id: rw.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: rw.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.5  1997/02/21  03:34:30  fdh
 * Added float and double access functions.
 *
 * Revision 1.4  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.2  1994/01/19  10:50:12  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.1  1993/06/08  19:56:42  fdh
 * Initial revision
 *
 */



/* rw.c
 */

#include "system.h"
#include "lib.h"

#ifndef ReadB
ub ReadB(ub *adr)
{
  return(*adr);
}
#endif

#ifndef ReadW
uw ReadW(uw *adr)
{
  return(*adr);
}
#endif

#ifndef ReadL
ui ReadL(ui *adr)
{
  return(*adr);
}
#endif

#ifndef ReadQ
ul ReadQ(ul *adr)
{
  return(*adr);
}
#endif

#ifndef WriteB
void WriteB(ub *adr, ub data)
{
  *adr = data;
}
#endif

#ifndef WriteW
void WriteW(uw *adr, uw data)
{
  *adr = data;
}
#endif

#ifndef WriteL
void WriteL(ui *adr, ui data)
{
  *adr = data;
}
#endif

#ifndef WriteQ
void WriteQ(ul *adr, ul data)
{
  *adr = data;
}
#endif

float ReadFloat(float *adr)
{
  return(*adr);
}

double ReadDouble(double *adr)
{
  return(*adr);
}

void WriteFloat(float *adr, float data)
{
  *adr = data;
}

void WriteDouble(double *adr, double data)
{
  *adr = data;
}
