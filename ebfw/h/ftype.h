#ifndef __FTYPE_H_LOADED
#define __FTYPE_H_LOADED
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

/*
 *  $Id: ftype.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $;
 *
 * $Log: ftype.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/03/14  19:17:40  fdh
 * Initial revision
 *
 */

/*
** IEEE Encodings
**
**                Sign   Exponent   Fraction   Finite
**                -----------------------------------
**  +/- NaN        x     All-1's    Non-zero   No
**  +/- Infinity   x     All-1's    0          No
**  +Denormal      0     0          Non-zero   No
**  -Denormal      1     0          Non-zero   No
**  +Zero          0     0          0          Yes
**  -Zero          1     0          0          Yes
**  Finite         x     other      x          Yes
*/

/*
** Double Precision
**
**  3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1 
**  1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0
** +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
** |                        Fraction Low                           | :Address
** +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
** |S|      Exponent       |             Fraction High             | :Address+4
** +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
#define VAL_L_DOUBLE(val)	*((unsigned int *)&(val))
#define VAL_H_DOUBLE(val)	*((unsigned int *)&(val) + 1)
#define IS_FINITE_DOUBLE(x) \
			((VAL_H_DOUBLE(x) & 0x7ff00000) != 0x7ff00000)
#define IS_INF_DOUBLE(x) \
			(((VAL_H_DOUBLE(x) & 0x7fffffff) == 0x7ff00000) && \
			 (VAL_L_DOUBLE(x) == 0))
#define IS_QNAN_DOUBLE(x) \
			((VAL_H_DOUBLE(x) & 0x7ff80000) == 0x7ff80000)
#define IS_SNAN_DOUBLE(x) \
			(IS_NAN_DOUBLE(x) && !IS_QNAN_DOUBLE(x))
#define IS_NAN_DOUBLE(x) \
			(((VAL_H_DOUBLE(x) & 0x7ff00000) == 0x7ff00000) && \
			 ((VAL_H_DOUBLE(x) & 0x000fffff) | VAL_L_DOUBLE(x)))
#define IS_ZERO_DOUBLE(x) \
			( !((VAL_H_DOUBLE(x) & 0x7fffffff) | VAL_L_DOUBLE(x)))
#define IS_DENORMAL_DOUBLE(x) \
			( !(VAL_H_DOUBLE(x) & 0x7ff00000) && \
			 ((VAL_H_DOUBLE(x) & 0x000fffff) | VAL_L_DOUBLE(x)))

/*
** Single Precision
**
**  3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1 
**  1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0
** +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
** |S|   Exponent    |                 Fraction                    | :Address
** +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
#define VAL_FLOAT(val)	*((unsigned int *)&(val))
#define IS_FINITE_FLOAT(x) \
			((VAL_FLOAT(x) & 0x7f800000) != 0x7f800000)
#define IS_INF_FLOAT(x)	((VAL_FLOAT(x) & 0x7fffffff) == 0x7f800000)
#define IS_QNAN_FLOAT(x) \
			((VAL_FLOAT(x) & 0x7fc00000) == 0x7fc00000)
#define IS_SNAN_FLOAT(x) \
			(IS_NAN_FLOAT(x) && !IS_QNAN_FLOAT(x))
#define IS_NAN_FLOAT(x) \
			(((VAL_FLOAT(x) & 0x7f800000) == 0x7f800000) && \
			 (VAL_FLOAT(x) & 0x007fffff))
#define IS_DENORMAL_FLOAT(x) \
			( !(VAL_FLOAT(x) & 0x7f800000) && \
			 (VAL_FLOAT(x) & 0x007fffff))
#define IS_ZERO_FLOAT(x) \
			( ! (VAL_FLOAT(x) & 0x7fffffff))

#define	FP_SNAN		0
#define	FP_QNAN		1
#define	FP_POS_INF	2
#define	FP_NEG_INF	3
#define	FP_POS_NORM	4
#define	FP_NEG_NORM	5
#define	FP_POS_DENORM	6
#define	FP_NEG_DENORM	7
#define	FP_POS_ZERO	8
#define	FP_NEG_ZERO	9

extern int isfinite(double x);
extern int isfinitef(float x);
extern int ftype(double x);
extern int ftypef(float x);
extern int isnan(double x);
extern int isnanf(float x);

#endif /* __FTYPE_H_LOADED */
