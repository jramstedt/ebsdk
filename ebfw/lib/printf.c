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
__attribute__((unused)) static const char *rcsid = "$Id: printf.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: printf.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.35  1998/08/03  17:42:09  gries
 * a fix?
 *
 * Revision 1.34  1997/12/15  20:44:05  pbell
 * Updated for dp264.
 *
 * Revision 1.33  1997/09/30  15:38:37  pbell
 * Fixed printf %4.0f so that the number width accounts for the lack
 * of the decimal point in the output.
 *
 * Revision 1.32  1997/09/30  15:06:54  fdh
 * Added compile time conditional to disable floating point.
 *
 * Revision 1.31  1997/08/01  19:11:28  pbell
 * Enhanced to properly support the 'L' size modifier for integers.
 *
 * Revision 1.30  1997/05/30  18:58:27  pbell
 * Added new function fprintf.
 *
 * Revision 1.29  1997/04/23  17:46:39  pbell
 * Fixed the return value to be the actual number of
 * characters generated. Changed 'f', 'e', 'g', 'G', 'E'
 * formats.  Added rounding to all floating point formats.
 * Fixed misc. problems in floating point print functions.
 *
 * Revision 1.28  1997/03/14  18:35:21  fdh
 * Replaced <machine/fpu.h> and <fp_class.h> with locally defined
 * "ftype.h" for portability to other environments.
 *
 * Revision 1.27  1997/02/25  21:30:58  fdh
 * Replaced calls to puts() with calls to the locally
 * defined PutString() which does not append a newline.
 *
 * Revision 1.26  1997/02/21  19:37:24  fdh
 * Adjusted precision.
 *
 * Revision 1.25  1997/02/21  03:23:11  gries
 * Included <machine/fpu.h> and <fp_class.h> to facilitate
 * reporting denormalized numbers while avoiding arithmeticexceptions.
 *
 * Revision 1.24  1997/02/20  01:22:04  fdh
 * Corrected endless loop when printing a floating point value of 0.
 *
 * Revision 1.23  1997/01/30  02:40:22  gries
 * Modified to print exponential values.
 *
 * Revision 1.22  1996/06/03  14:49:54  fdh
 * Added MessageString definition.
 *
 * Revision 1.21  1996/05/22  21:09:18  fdh
 * odified to use callback routines instead of local routines.
 * The callback routine pointers get redefined properly when
 * handling multiple ports.
 *
 * Revision 1.20  1995/11/10  19:02:14  fdh
 * Added a special exception and comments for
 * lint warnings which results from intentional
 * data truncation resulting from (casts).
 *
 * Revision 1.19  1995/11/09  21:34:43  cruz
 * Changed argument of PutNumber from type long to type "sl".
 *
 * Revision 1.18  1995/11/06  23:06:22  fdh
 * ANSI says that unsigned modifiers can be used.
 *
 * Revision 1.17  1995/11/06  22:18:01  fdh
 * Corrected data types used with printf modifiers.
 *
 * Revision 1.16  1995/10/31  22:51:09  cruz
 * Added casting.
 *
 * Revision 1.15  1995/10/31  18:26:50  fdh
 * Parse precision specifier.
 *
 * Revision 1.14  1995/10/31  16:33:09  fdh
 * Pass correct argument to PutNumber().
 * Rearranged the format specifier parsing.
 *
 * Revision 1.13  1995/10/27  14:17:53  cruz
 * Added return to putnumber().
 *
 * Revision 1.12  1995/10/05  00:03:17  fdh
 * Fixed up some internal declarations.
 *
 * Revision 1.11  1995/10/03  03:15:51  fdh
 * Added prototypes for internal functions.
 *
 * Revision 1.10  1995/08/31  21:50:33  fdh
 * Implemented vprintf().  Modified printf(), sprintf(), and
 * vprintf() to return the output character count.
 *
 * Revision 1.9  1995/08/25  19:47:20  fdh
 * Added support for more Standard C printf() specifiers.
 * Implemented sprintf().
 * l
 *
 * Revision 1.8  1994/10/06  20:29:08  fdh
 * Corrected unsigned long declaration.
 *
 * Revision 1.7  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.6  1994/06/21  15:41:54  rusling
 * fixedup WNT compiler warnings
 *
 * Revision 1.5  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.4  1994/01/19  10:40:08  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.3  1993/11/02  21:57:45  fdh
 * Fixed sign extension problem introduced in version 1.2
 *
 * Revision 1.2  1993/10/13  15:29:02  rusling
 * Added floating point support in printf.  This meant adding variable arguments to
 * it and FormatItem() and including stdarg.h.
 *
 * Revision 1.1  1993/06/08  19:56:24  fdh
 * Initial revision
 *
 */

#include <stdarg.h>
#include "system.h"
#include "lib.h"
#include "ctype.h"
#include "callback.h"
#include "ftype.h"
#include "interlock.h"


#define fabs(x) (( (x)<0.0) ? (-x) : x)
#define PUTNUMBER_SL_	((PRINTF_sl_)[0])

static int LeadingSign;
static int UpperCase;
static void (*PutFunction)(char);
volatile long print_lock= 0;

/* Prototypes for static functions */
static int PutString(char * s);
static void PutRepChar(char c , int count);
static void PutStringReverse(char * s , int index);
static int PutNumber(sl value , int radix , int width , char fill , char modifier);
static ul power(long base , long n);

#ifndef __NO_FLOATING_POINT
static int putFloat( double value, int fieldwidth, int precision, char fill, char FormatType );
static int putFloatE( double value, int fieldwidth, int precision, char fill, char ExponentChar );
static int putFloatF( double value, int fieldwidth, int precision, char fill );
static int putFloatG( double value, int fieldwidth, int precision, char fill, char ExponentChar );
#endif /* __NO_FLOATING_POINT */

static const char * FormatItem(const char * f , va_list * ap , int * count);
static int OutputFunction(const char * f , va_list ap);
static void CopyChar(char c);
static int ExtractExponent( double *value );

typedef union
{
    struct
    {
        char * strptr;
    } Copy;
#ifdef NEEDFLOPPY
    struct
    {
        FILE * File;
    } File;
#endif
} FUNCTIONDATA;

static FUNCTIONDATA FunctionData;

/* The string s is terminated by a '\0' */
static int PutString(char *s)
{
  int count = 0;
  while (*s) {
    (*PutFunction)(*s++); ++count;
  }
  return count;
}

/* print c count times */
static void PutRepChar(char c, int count)
{
  while (count--) (*PutFunction)(c);
}

/* put string reverse */
static void PutStringReverse(char *s, int index)
{
  while ((index--) > 0) (*PutFunction)(s[index]);
}

/* prints value in radix, in a field width width, with fill
   character fill
   if radix is negative, print as signed quantity
   if width is negative, left justify
   if width is 0, use whatever is needed
   if fill is 0, use ' '
 */
static int PutNumber(sl value, int radix, int width, char fill, char modifier)
{
  int count = 0;
  char buffer[40];
  ui bufferindex = 0;
  ul uvalue;
  uw digit;
  uw left = FALSE;
  uw negative = FALSE;

  if (fill == 0) fill = ' ';

  switch (modifier) {
#ifndef LINT
    /*
    ** lint warns about the tuncations that results
    ** from these (casts) but that is precisely what
    ** was intended.
    **/
  case 'h':
    value = (unsigned short int) value;
    break;
  case 'l':
    if (radix < 0)
      value = (long) value;
    else
      value = (unsigned long) value;
    break;
  case 'L': /* L is sl in NT and int on UNIX */
#   ifdef PRINTF_L_LARGE
        break;
#   endif
  default:
    if (radix < 0)
      value = (int) value;
    else
      value = (unsigned int) value;
#else
  default:
    ;				/* Just a dummy statement to close the switch */
#endif /* LINT */
  }

  if (width < 0) {
    width = -width;
    left = TRUE;
    }
  if (width < 0 || width > 80) width = 0;

  if (radix < 0) {
    radix = -radix;
    if (value < 0) {
      negative = TRUE;
      value = -value;
      }
    }
  switch (radix) {
    case 8:
    case 10:
    case 16: break;
    default: {
      count += PutString("****");
      return count;
      }
    }
  uvalue = value;
  do {
    if (radix != 16)
    {
      digit = (uw)(uvalue % radix);
      uvalue /= radix;
    }
    else
    {
      digit = (uw)(uvalue & 0xf);
      uvalue = uvalue >> 4;
    }
    buffer[bufferindex] =
      digit + ((digit <= 9) ? '0' : ((UpperCase ? 'A' : 'a') - 10));
    bufferindex += 1;
    } while (uvalue != 0);
  /* fill # ' ' and negative cannot happen at once */
  if (negative||LeadingSign) {
    buffer[bufferindex] = negative ? '-' : '+';
    bufferindex += 1;
    }
  if ((ui)width <= bufferindex) PutStringReverse(buffer, bufferindex);
  else {
    width -= bufferindex;
    if (!left) {
      count += width;
      PutRepChar(fill, width);
    }
    PutStringReverse(buffer, bufferindex);
    if (left) {
      count += width;
      PutRepChar(fill, width);
    }
  }
  count += bufferindex;
  return count;
}

static ul power(long base, long n)
{
  ul p;

  for (p = 1; n > 0; --n)
    p = p * base;
  return p;
}

static ul ten_table_hex[25] ={0x3ff0000000000000,0x4024000000000000,0x4059000000000000,0x408f400000000000,
                              0x40c3880000000000,0x40f86a0000000000,0x412e848000000000,0x416312d000000000,
                              0x4197d78400000000,0x41cdcd6500000000,0x4202a05f20000000,0x42374876e8000000,
                              0x426d1a94a2000000,0x42a2309ce5400000,0x42d6bcc41e900000,0x430c6bf526340000,
                              0x4341c37937e08000,0x4376345785d8a000,0x43abc16d674ec800,0x43e158e460913d00,
                              0x4415af1d78b58c40,0x444b1ae4d6e2ef50,0x4480f0cf064dd592,0x44b52d02c7e14af6,
                              0x44ea784379d99db4};

double *ten_table= (double *)&(ten_table_hex[0]);

#ifndef __NO_FLOATING_POINT
static int putFloat( double value, int fieldwidth, int precision, char fill, char FormatType )
{
    char ExponentChar;

    /* check for flag values and display special strings for them */

    switch (ftype(value))
    {
    case FP_SNAN:	return( PutString("*** sNAN ***") );
    case FP_QNAN:	return( PutString("*** qNAN ***") );
    case FP_POS_INF:	return( PutString("*** +INF ***") );
    case FP_NEG_INF:	return( PutString("*** -INF ***") );
    case FP_POS_DENORM: return( PutString("* +denorm **") );
    case FP_NEG_DENORM:	return( PutString("* -denorm **") );
    case FP_POS_ZERO:	return( PutString("*** +0.0 ***") );
    case FP_NEG_ZERO:	return( PutString("*** -0.0 ***") );

    case FP_POS_NORM:		/* Positive normalized */
    case FP_NEG_NORM:		/* Negative normalized */
        break;
    }

    ExponentChar = 'E';

    switch( FormatType )
    {
    case 'e':
        ExponentChar = 'e';
        /* fallthrough */
    case 'E':
        return( putFloatE( value, fieldwidth, precision, fill, ExponentChar ) );
    case 'f':
        return( putFloatF( value, fieldwidth, precision, fill ) );
    case 'g':
        ExponentChar = 'e';
        /* fallthrough */
    case 'G':
        return( putFloatG( value, fieldwidth, precision, fill, ExponentChar ) );
    }

    return( 0 );
}


static int putFloatE( double value, int fieldwidth, int precision, char fill, char ExponentChar )
{
    int sign;
    int exp, exp_sign;
    int count = 0;

    if( value < 0 )
        sign = 1, value = -value;
    else
        sign = 0;

    exp = ExtractExponent( &value );
    if( exp < 0 )
        exp = -exp, exp_sign = 1;
    else
        exp_sign = 0;

    if( fieldwidth > 0 )	/* right justified */
    {
        fieldwidth -= (precision+sign+6+(precision>0?1:0));
        while( fieldwidth > 0 )
            fieldwidth--, (*PutFunction)( fill ), count++;
        fieldwidth = 0;
    }
    if(sign)  
        (*PutFunction)('-'),		  count++; /* Output the sign*/
    if( precision == 0 ) value += 0.5;	/* round the msd if there are no others */
    (*PutFunction)((char)(((ul) value)+'0')), count++; /* Output the MSD*/

    if( precision > 0 )
    {
        double Tmp;

        (*PutFunction)('.' & 0x7f),	  count++; /* Output the decimal place. */

        Tmp = value - (sl)value;	/* remove the integer part */
        Tmp *= ten_table[precision];	/* shift the fraction into an integer */
        Tmp += 0.5;			/* round the remaining fraction */
        count += PutNumber( (sl)Tmp, 10, precision, '0', PUTNUMBER_SL_ ); /* print the digits */
    }

    (*PutFunction)( ExponentChar );	count++; /* Output the exponent char */
    (*PutFunction)((exp_sign)?'-':'+'); count++; /* Output the exponent sign */
    count += PutNumber( (sl)exp, 10, 3, '0', PUTNUMBER_SL_ );/* generate the exp. digits */

    /* for left justified data finish the width padding if necessary */
    if( fieldwidth < 0 )
    {
        fieldwidth = -fieldwidth - count;
        while( fieldwidth > 0 )
            fieldwidth--, (*PutFunction)( fill ), count++;
    }

    return count;
}


static int putFloatF( double value, int fieldwidth, int precision, char fill )
{
    int count = 0;

    if( precision == 0 )
        value += value<0 ? (-0.5) : 0.5;/* round the int part if there is no frac */

    count += PutNumber( (sl)value, -10,
                        fieldwidth > 0 ?
                        (fieldwidth - (precision>0 ? (precision + 1) : 0) ) : 0,
                        fill, PUTNUMBER_SL_);

    if( precision > 0 )
    {
        double Tmp;

        (*PutFunction)('.' & 0x7f),	  count++; /* Output the decimal place. */

        Tmp = value<0 ? (-value) : value;   /* remove the sign */
        Tmp = Tmp - (sl)Tmp;		    /* remove the integer part */
        Tmp *= ten_table[precision];	    /* shift the fraction into an integer */
        Tmp += 0.5;			    /* round the remaining fraction */
        count += PutNumber( (sl)Tmp, 10, precision, '0', PUTNUMBER_SL_ ); /* print the digits */
    }

    /* for left justified data finish the width padding if necessary */
    if( fieldwidth < 0 )
    {
        fieldwidth = -fieldwidth - count;
        while( fieldwidth > 0 )
            fieldwidth--, (*PutFunction)( fill ), count++;
    }

    return count;
}


static int putFloatG( double value, int fieldwidth, int precision, char fill, char ExponentChar )
{
    int sign;
    int exp;
    int count = 0;
    double MagValue;
    int RequiredPrecision;

    if( value < 0 )
        sign = 1, MagValue = -value;
    else
        sign = 0, MagValue = value;

    exp = ExtractExponent( &MagValue );

    /* determine the required precision for display! */
    {
        sl Tmp = (sl)(MagValue*ten_table[precision-1] + 0.5);
        for( RequiredPrecision = precision;
             RequiredPrecision > 1 && (Tmp % 10) == 0;
             RequiredPrecision--, Tmp /= 10 )
            ;
    }

    /* Dynamically decide wether f or e format is smaller for this value */
    /* expression macros to make this more readable */
#define FIXEDSIZE( PREC, EXP )	( (PREC) +				     \
                                  ((EXP) < ((PREC)-1) ? 1		:0) +\
                                  ((EXP) >=(PREC)     ? ((EXP)-(PREC)+1):0) +\
                                  ((EXP) < 0	      ? (-(EXP))	:0) )
#define EXPSIZE( PREC )		( 6+(PREC) )
    
    if( exp < precision &&
        FIXEDSIZE( RequiredPrecision, exp ) < EXPSIZE( RequiredPrecision ) )
    {
        RequiredPrecision -= (exp+1);
        if( RequiredPrecision < 0 )
            RequiredPrecision = 0;

        return( putFloatF( value, fieldwidth, RequiredPrecision, fill ) );
    }
    else
        return( putFloatE( value, fieldwidth, RequiredPrecision-1, fill, ExponentChar ) );

#undef FIXEDSIZE
#undef EXPSIZE
}


static int ExtractExponent( double *value )
{
    double MagValue;
    int sign;
    int exp;

    if( *value < 0 )
        sign = 1, MagValue = -*value;
    else
        sign = 0, MagValue = *value;

    exp = 0;
    if (MagValue<1.0)
    {
        while(MagValue*ten_table[24]<1.0)
        {
            exp-= 24;
            MagValue *= ten_table[24];
        }
        while (MagValue<1.0)
        {
            exp--;
            MagValue *= 10.0;
        }
    }
    else
    {
        while(MagValue>=ten_table[24])
        {
            exp+= 24;
            MagValue /= ten_table[24];
        }
        while(MagValue>=10.0)
        {
            exp++;
            MagValue /= 10.0;
        }
    }

    *value = sign ? (-MagValue) : MagValue;
    return( exp );
}
#endif /* __NO_FLOATING_POINT */


static const char *FormatItem(const char *f, va_list *ap, int *count)
{
  int fieldwidth = 0;
  int precision = -1;
  int leftjust = FALSE;
  int radix = 0;
  char pmod = 0;
  char fill = ' ';
  LeadingSign = FALSE;
  UpperCase = FALSE;

  /* Parse printf Flags */
  while (strchr("-+ #0", *f) != NULL) {
    switch(*f++) {
    case '-': leftjust = TRUE;
      break;
    case '+': LeadingSign = TRUE;
      break;
    case ' ': fill = ' ';
    case '#':
      break;
    case '0': fill = '0';
      break;
    }
  }

  /* Parse field width specifier */
  while (_isdigit((unsigned char)*f)) {
    fieldwidth = (fieldwidth * 10) + (*f - '0');
    ++f;
  }

  /* Parse field precision specifier */
  if (*f == '.') {
    ++f;
    precision = 0;
    while (_isdigit((unsigned char)*f)) {
      precision = (precision * 10) + (*f - '0');
      ++f;
    }
  }

  while( 1 )
  {
    switch( *(f++) )
    {
      case '\000': return(--f);


      /* Parse printf Modifiers */
      case 'h': pmod = 'h';
        break;
      case 'l': pmod = 'l';
        break;
      case 'L': pmod = 'L';
        break;


    /* Parse printf Specifier. */
      case 'c':
        {
          const unsigned char a = (unsigned char)va_arg(*ap, int);

          if (leftjust) {(*PutFunction)((char)(a & 0x7f)); ++(*count);}
          if (fieldwidth > 0) PutRepChar(fill, fieldwidth - 1);
          if (!leftjust) {(*PutFunction)((char)(a & 0x7f)); ++(*count);}
          return(f);
        }
      case 'd': radix = -10;
        break;

#ifndef __NO_FLOATING_POINT
      case 'e':
      case 'E':
      case 'f':
      case 'g':
      case 'G':
        {
          double a = va_arg(*ap, double);
          if (leftjust)
            fieldwidth = -fieldwidth;
          if (precision<0) precision = 6;
          count += putFloat(a, fieldwidth, precision, fill, *(f-1) );
          return(f);
        }
#endif /* __NO_FLOATING_POINT */

      case 'o': radix = 8;
        break;
      case 's':
        {
          char *a = va_arg(*ap, char *);

          if (leftjust) count += PutString((char *) a);
          if (fieldwidth > (int)strlen((char *) a))
            PutRepChar(fill, fieldwidth - strlen((char *)a));
          if (!leftjust) count += PutString((char *) a);
          return(f);
        }
      case 'u': radix = 10;
        break;
      case 'x': radix = 16;
        break;
      case 'X': radix = 16;
        UpperCase = TRUE;
        break;
      case '%': (*PutFunction)('%'); ++(*count);
        return(f);
      default:   /* unknown switch! */
        radix = 3;
        break;
      }
    if (radix) break;
  }

  if (leftjust)
    fieldwidth = -fieldwidth;
  {
     sl a = va_arg(*ap, sl);
     PutNumber(a, radix, fieldwidth, fill, pmod);
  }
  return(f);
}

static int OutputFunction(const char *f, va_list ap)
{
  int count = 0;

  while (*f) {
    if (*f == '%') f = FormatItem(f + 1, &ap, &count);
    else {(*PutFunction)(*f++); ++count;}
  }
  return count;
}

int printf(const char *f, ...)
{
  int count = 0;
  va_list ap;

  while (test_set_low(&print_lock))
  {
    usleep(1);
  }

  PutFunction = UserPutChar;

  va_start(ap, f);
  count = OutputFunction(f, ap);
  va_end(ap);         /* clean up */

  print_lock = 0;

  return count;
}

static void CopyChar(char c)
{
  *FunctionData.Copy.strptr++ = c;
}

int sprintf(char *str, const char *f, ...)
{
  int count = 0;
  va_list ap;

  FunctionData.Copy.strptr = str;
  PutFunction = CopyChar;

  va_start(ap, f);

  count = OutputFunction(f, ap);

  va_end(ap);         /* clean up */
  *FunctionData.Copy.strptr = '\0';
  return count;
}

int vprintf(const char *f, va_list ap)
{
  int count = 0;
  PutFunction = UserPutChar;
  count = OutputFunction(f, ap);
  return count;
}

#ifdef NEEDFLOPPY
static void FilePutChar( char c )
{
    fputc( c, FunctionData.File.File );
}

int fprintf( FILE * OutFile, const char * CtrlStr, ... )
{
    int count = 0;
    va_list ap;

    FunctionData.File.File = OutFile;
    PutFunction = FilePutChar;

    va_start( ap, CtrlStr );

    count = OutputFunction( CtrlStr, ap );

    va_end(ap);         /* clean up */
    return count;
}
#endif /* NEEDFLOPPY */
