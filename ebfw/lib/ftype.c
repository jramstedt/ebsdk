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
__attribute__((unused)) static const char *rcsid = "$Id:";
#endif

/*
 * $Log: ftype.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1997/12/15  20:44:02  pbell
 * Merged dp264 changes.
 *
 * Revision 1.1  1997/03/14  18:33:55  fdh
 * Initial revision
 *
 */

#include "lib.h"
#include "ftype.h"


/**************************************************************************
 * Ftype routines                                                         *
 **************************************************************************/

#ifndef __NO_FLOATING_POINT
int isfinite(double x){return(IS_FINITE_DOUBLE(x));}
int isfinitef(float x){return(IS_FINITE_FLOAT(x));}
int isnan(double x){return(IS_NAN_DOUBLE(x));}
int isnanf(float x){return(IS_NAN_FLOAT(x));}

int ftype(double x)
{
  int sign;

  sign = (int) (VAL_H_DOUBLE(x) & 0x80000000u);

  if (IS_SNAN_DOUBLE(x))
    return (FP_SNAN);

  if (IS_QNAN_DOUBLE(x))
    return (FP_QNAN);

  if (IS_INF_DOUBLE(x))
    return (sign ? FP_NEG_INF : FP_POS_INF);

  if (IS_DENORMAL_DOUBLE(x))
    return (sign ? FP_NEG_DENORM : FP_POS_DENORM);

  if (IS_ZERO_DOUBLE(x))
    return (sign ? FP_NEG_ZERO : FP_POS_ZERO);

  if (IS_FINITE_DOUBLE(x))
    return (sign ? FP_NEG_NORM : FP_POS_NORM);
}

int ftypef(float x)
{
  int sign;

  sign = (int) (VAL_FLOAT(x) & 0x80000000u);

  if (IS_SNAN_FLOAT(x))
    return (FP_SNAN);

  if (IS_QNAN_FLOAT(x))
    return (FP_QNAN);

  if (IS_INF_FLOAT(x))
    return (sign ? FP_NEG_INF : FP_POS_INF);

  if (IS_DENORMAL_FLOAT(x))
    return (sign ? FP_NEG_DENORM : FP_POS_DENORM);

  if (IS_ZERO_FLOAT(x))
    return (sign ? FP_NEG_ZERO : FP_POS_ZERO);

  if (IS_FINITE_FLOAT(x))
    return (sign ? FP_NEG_NORM : FP_POS_NORM);
}
#endif /* __NO_FLOATING_POINT */
