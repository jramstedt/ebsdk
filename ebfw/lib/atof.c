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
static char *rcsid = "$Id: atof.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 * atof
 *
 *	Convert string to double. 
 *
 * $Log: atof.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1997/12/15  20:44:01  pbell
 * Merged dp264 changes
 *
 * Revision 1.2  1997/04/23  17:42:26  pbell
 * Changed ten_table_hex to sl from long.
 * Added a LocalAtol function that takes a size parameter.
 * Fixed problems in atof converting fraction and integer
 * parts with more than 9 digits.
 *
 * Revision 1.1  1997/02/21  03:26:10  gries
 * Initial revision
 *
 */

#include <math.h>
#include "lib.h"
#include "ctype.h"


static sl ten_table_hex[25] ={
    0x3ff0000000000000,0x4024000000000000,0x4059000000000000,0x408f400000000000,
    0x40c3880000000000,0x40f86a0000000000,0x412e848000000000,0x416312d000000000,
    0x4197d78400000000,0x41cdcd6500000000,0x4202a05f20000000,0x42374876e8000000,
    0x426d1a94a2000000,0x42a2309ce5400000,0x42d6bcc41e900000,0x430c6bf526340000,
    0x4341c37937e08000,0x4376345785d8a000,0x43abc16d674ec800,0x43e158e460913d00,
    0x4415af1d78b58c40,0x444b1ae4d6e2ef50,0x4480f0cf064dd592,0x44b52d02c7e14af6,
    0x44ea784379d99db4
};

static double *ten_table= (double *)&(ten_table_hex[0]);

static long LocalAtol( const char * Str, int MaxLen )
{
    long Result = 0;
    for( ; MaxLen > 0 && isdigit(*Str); MaxLen--, Str++ )
	Result = (Result * 10) + (*Str - '0');
    return( Result );
}


#ifndef __NO_FLOATING_POINT
double atof( const char *str )
{
    const char *t;
    int sign,exp,i;
    double result, fraction;

    /* Skip whitespace then parse fraction.
     */
    while (isspace(*str)) str++;

    if (*str == '+' || *str == '-') 
    {
	sign= *str == '-';
	str++ ;
    }
    else
	sign=0;

    /* remove any leading zeros */

    while( *str == '0' ) str++;

    /* convert the leading integer portion */
    t= str;
    while (isdigit(*str)) str++;
    exp = (str-t);

    /* convert the integer portion from left to right in 9 char chunks */
    for( result=0.0, i=0; i<exp; i+=9 )
    {
	int NumChars = (exp-i)>9 ? 9 : (exp-i);
	result *= ten_table[NumChars];
	result += LocalAtol(&t[i], NumChars);
    }

    /* convert the decimal portion */
    if (*str == '.' && isdigit(str[1])) 
    {
	t= str+1;
	/*
	 * Ensure string can be converted
	 */
	str+= 2;
	while (isdigit(*str)) str++;
	i= (str-t);

	/* convert the fraction from right to left in 9 char chunks */
        for( fraction = 0.0; i>0; i-=9 )
	{
	    int NumChars = i>9 ? 9 : i;
	    fraction += LocalAtol(&t[i-NumChars], NumChars);
	    fraction /= ten_table[NumChars];
	}

	result+= fraction;
    }

    /* Parse exponent.*/
    if (*str == 'E' || *str == 'e') 
    {
	str++;
	i= atoi(str);
	if( (i+exp)>308 )
	{
	    result = HUGE_VAL;
	    if(sign) result = -result;
	    return result; 
	}
	else if( (i+exp)<-308 )
	{
	    result = 0.0;
	    if(sign) result = -result;
	    return result; 
	}

	if( i > 0 )
	{
	    for( ; i > 23; i-=23 )
		result *= ten_table[23];
	    result *= ten_table[i];
	}
	else
	{
	    for( ; i < -23; i+=23 )
		result /= ten_table[23];
	    result /= ten_table[-i];
	}
    } 
  
    /* Return converted string
     */
    if (sign) result = -result;
    return(result);
}
#endif /* __NO_FLOATING_POINT */
