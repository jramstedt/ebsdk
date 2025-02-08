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
__attribute__((unused)) static const char *rcsid = "$Id: scanf.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: scanf.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.7  1997/12/15  20:44:06  pbell
 * Updated for dp264.
 *
 * Revision 1.6  1997/05/30  20:44:15  pbell
 * Added include for system.h to get NEEDFLOPPY.
 *
 * Revision 1.5  1997/05/30  18:58:27  pbell
 * Added new function fscanf.
 *
 * Revision 1.4  1997/04/30  21:16:20  pbell
 * Enhanced scanf %f using UngetChar to support 100e+rgs properly.
 *
 * Revision 1.3  1997/04/23  17:44:08  pbell
 * Added support for ranges '%[0-9]'.
 * Fixed the handling of white space between formats.
 * Change the %c format to take any character without
 * reguard for whitespace. Changed the handling of the
 * returned count to not include ignored values and
 * missing last values. Fixed the supress mode for all
 * types so that bogus return values are not written.
 * Updated integer scan formats to write the correct size
 * arguments.
 *
 * Revision 1.2  1997/02/21  19:38:21  fdh
 * Modified logic around pmod.
 *
 * Revision 1.1  1997/02/21  03:30:19  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "lib.h"
#include "ctype.h"
#include "callback.h"

#define FIELDMAX 512

typedef struct {
  int fieldwidth;
  int precision;
  int Suppress;
  int success;
  int radix;
  int sign;
  char pmod;
} fd_t;

static int InputFunction(const char *f, va_list ap);
static const char *GetFormatItem(char *ch, const char *f, va_list *ap, int * Count );
static sl getInt(char *ch, fd_t * fd );
static double getFloat(char *ch, fd_t * fd );

static int CopyChar(void);
static void UncopyChar(char);
static int FileGetChar( void );
static void FileUngetChar( char c );

static int (*GetFunction)(void);
static void (*UngetFunction)(char);

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

int scanf(const char *f, ... )
{
  int count = 0;
  va_list ap;
  /* The following code insures that UserGetChar has been updated from
   * PortsGetChar to GetChar if this call preceeds any other input.
   */
  UngetChar( UserGetChar() );

  GetFunction = GetChar;
  UngetFunction = UngetChar;

  va_start(ap, f);

  count = InputFunction(f, ap);

  va_end(ap);         /* clean up */
  return count;
}

int sscanf( const char *str, const char *f, ...)
{
  int count = 0;
  va_list ap;
  GetFunction = CopyChar;
  UngetFunction = UncopyChar;
  FunctionData.Copy.strptr = (char *) str;

  va_start(ap, f);

  count = InputFunction(f, ap);

  va_end(ap);       /* clean up */
  FunctionData.Copy.strptr = NULL;    /* clear the copy pointer */	
  return count;
}

#ifdef NEEDFLOPPY

int fscanf( FILE * OutFile, const char * CtrlStr, ... )
{
    int count = 0;
    va_list ap;

    FunctionData.File.File = OutFile;
    GetFunction = FileGetChar;
    UngetFunction = FileUngetChar;

    va_start(ap, CtrlStr);

    count = InputFunction(CtrlStr, ap);

    va_end(ap);         /* clean up */
    return count;
}

static int FileGetChar( void )
{
    return( fgetc( FunctionData.File.File ) );
}

static void FileUngetChar( char c )
{
    fungetc( c, FunctionData.File.File );
}

#endif /* NEEDFLOPPY */

static int InputFunction(const char * f , va_list ap)
{
  int count = 0;
  char ch = (*GetFunction)();

  while (*f) {
    if( *f == '%' )
    {
      f = GetFormatItem(&ch, f + 1, &ap, &count );
      if(f == NULL)
	  break;
    }
    else {
      if (isspace(*f)) {	/* Match whitespace */
	while (isspace(*f))
	  ++f;
	while (isspace(ch)) {
	  ch = (*GetFunction)();
	}
      }
      else			/* Match literally */
      {
	  if( *f != ch )
	      break;
	  ++f;
	  ch = (*GetFunction)();
      }
    }
  }

  (*UngetFunction)( ch );
  return count;
}

static int CopyChar(void)
{
  return(*FunctionData.Copy.strptr++);
}

static void UncopyChar(char c)
{
    *(--FunctionData.Copy.strptr) = c;
}

/* scan the character class looking for a match to the character Ch */

int CharIsInClass( const char * CharClass, char Ch )
{
    int Matched = 0;
    int Inverted;

    if( *CharClass == '^' ) /* inverted class */
	Inverted = 1, CharClass++;
    else
	Inverted = 0;

    for( ; *CharClass != 0 && *CharClass != ']'; CharClass++ )
    {
	if( *CharClass == '\\' )
	{
	    CharClass++;
	    if( *CharClass == 0 )
		break;
	}

	if( CharClass[1] == '-' )
	{
	    char FirstChar = *CharClass;
	    char LastChar = *(CharClass+=2);
	    if( LastChar == '\\' )
		LastChar = *(++CharClass);
	    if( FirstChar > LastChar )
		LastChar = FirstChar, FirstChar = *CharClass;
	    if( Ch >= FirstChar && Ch <= LastChar )
	    {
		Matched = 1;
		break;
	    }
	}
	else
	{
	    if( Ch == *CharClass )
	    {
		Matched = 1;
		break;
	    }
	}
    }

    return( Inverted ^ Matched );
}

/* find the first character after the closing ] for a character class */

const char * AfterCharClass( const char * CharClass )
{
    for( ; *CharClass != 0 && *CharClass != ']'; CharClass++ )
	if( *CharClass == '\\' )
	    CharClass++;

    if( *CharClass == ']' )
	CharClass++;

    return( CharClass );
}

const char PMOD_FLAGS[] = "hlL";

static const char *GetFormatItem(char *ch, const char *f, va_list *ap, int * Count)
{
    fd_t fd;
    char *ptr;

    /* Check for an exhausted input string */
    if( *ch == 0 )
	return( NULL );

    if( *f == '%' )
    {
	f++;
	if( *ch != '%' )
	    return( NULL );
	*ch = (*GetFunction)();
	return( f );
    }

    fd.fieldwidth = 0;
    fd.precision = 0;
    fd.Suppress = FALSE;
    fd.success = FALSE;
    fd.radix = 0;
    fd.sign = FALSE;
    fd.pmod = '\0';

    if (*f == '*')
    {
	fd.Suppress = TRUE;
	++f;
    }

    /* Parse field width specifier */
    while (isdigit(*f))
    {
	fd.fieldwidth = (fd.fieldwidth * 10) + (*f - '0');
	++f;
    }

    /* Parse scanf Modifiers */
    if ((ptr = (char *) strchr(PMOD_FLAGS, *f)) != NULL)
    {
	fd.pmod = *ptr;
	++f;
    }

    /* Parse scanf Specifier.
     * This is a double switch where the items in the first body do not
     * skip over white space between items and the
     */
    switch( *(f++) )
    {
    case 'c':	/* read the next single byte or fixed length byte array */
    {
	char *a;

	if (!fd.Suppress)
	{
	    a = va_arg(*ap, char *);
	    (*Count)++;
	}

	if (fd.fieldwidth == 0)
	    fd.fieldwidth = 1;

	for( ; *ch && fd.fieldwidth--; *ch = (*GetFunction)() )
	    if (!fd.Suppress)
		*a++ = *ch;
	break;
    }
    case '[': /* character class specification */
    {
	char *a;

	if (!fd.Suppress)
	{
	    a = va_arg(*ap, char *);
	    (*Count)++;
	}

	if (fd.fieldwidth == 0)
	    fd.fieldwidth = FIELDMAX;

	for( ; *ch && fd.fieldwidth-- && CharIsInClass( f, *ch );
	       *ch = (*GetFunction)() )
	    if (!fd.Suppress)
		*a++ = *ch;

	if (!fd.Suppress)
	    *a = '\0';

	f = AfterCharClass(f);
	break;
    }

    default:
	/* remove any white space in the input before processing the load */
	while( isspace(*ch) )
	    *ch = (*GetFunction)();

	/* Check for an exhausted input string */
	if( *ch == 0 )
	    return( NULL );

    /* secondary switch for white space delimited types */
    switch (*(f-1))
    {
    case 's':
    {
	char *a;

	if (!fd.Suppress)
	{
	    a = va_arg(*ap, char *);
	    (*Count)++;
	}

	if (fd.fieldwidth == 0)
	    fd.fieldwidth = FIELDMAX;

	for( ; *ch && fd.fieldwidth-- && !isspace(*ch); *ch = (*GetFunction)() )
	    if (!fd.Suppress)
		*a++ = *ch;

	if (!fd.Suppress)
	    *a = '\0';

	break;
    }
    case 'i':
    case 'o':
    case 'd':
    case 'u':
    case 'p':
    case 'X':
    case 'x':
    {
	sl value;

	switch(*(f-1))
	{
	case 'i': fd.radix = 0;	    break;

	case 'o': fd.radix = 8;	    break;

	case 'd':
	case 'u': fd.radix = 10;    break;

	case 'p':
	case 'X':
	case 'x': fd.radix = 16;    break;

	default:		    break;
	}

	value = getInt( ch, &fd );

	if( !fd.success )
	    return( NULL );

	if (!fd.Suppress)
	{
	    switch( fd.pmod )
	    {
	    case 'L':	*((sl *)va_arg(*ap, sl *)) = value;
			break;
	    case 'l':	*((long *)va_arg(*ap, long *)) = (long)value;
			break;
	    case 'h':	*((short *)va_arg(*ap, short *)) = (short)value;
			break;
	    default:	*((int *)va_arg(*ap, int *)) = (int)value;
			break;
	    }
	    (*Count)++;
	}

	break;
    }

#ifndef __NO_FLOATING_POINT
    case 'e':
    case 'E':
    case 'g':
    case 'G':
    case 'f':
    {
	double value = getFloat(ch, &fd );

	if( !fd.success )
	    return( NULL );

	if( !fd.Suppress )
	{
	    if (fd.pmod == 'l')
	    {
		double *a = va_arg(*ap, double *);
		*a = value;
	    }
	    else
	    {
		float *a = va_arg(*ap, float *);
		*a = (float)value;
	    }
	    (*Count)++;
	}

	break;
    }
#endif /* __NO_FLOATING_POINT */

    default:   /* unknown switch! */
        return(NULL);
    }
    } /* end of the double switch */

    return( f );
}

static sl getInt(char *ch, fd_t *fd )
{
#define ACCEPTANDLOAD		    \
	( Width--,		    \
	  *(bufp++) = *ch,	    \
          *ch = (*GetFunction)() )  \

    sl value;
    char buf[FIELDMAX+1];
    char *bufp;
    int Width;

    bufp = buf;

    if(fd->fieldwidth == 0)
	Width = FIELDMAX;
    else
	Width = fd->fieldwidth;

    /* save the sign value incase the calling program needs it. */

    if( Width > 0 && *ch == '-' )
    {
	fd->sign = TRUE;
	ACCEPTANDLOAD;
    }

    if( Width > 0 && fd->radix == 0 )
    {
	if( *ch == '0' )
	{
	    ACCEPTANDLOAD;
	    if( Width > 0 && ( *ch == 'X' || *ch == 'x' ) )
		fd->radix = 16;
	    else
		fd->radix = 8;
	}
	else
	    fd->radix = 10;
    }

    if( fd->radix == 16 )
    {
	if( Width > 0 && *ch == '0' )
	{
	    ACCEPTANDLOAD;
	    if( Width > 0 && ( *ch == 'X' || *ch == 'x' ) )
		ACCEPTANDLOAD;
	}
	while( Width > 0 && isxdigit(*ch) )
	    ACCEPTANDLOAD;
    }
    else if( fd->radix == 8 )
    {
	while( Width > 0 && ( *ch >= '0' && *ch <= '7' ) )
	    ACCEPTANDLOAD;
    }
    else /*if( fd->radix == 10 )  includes any other values */
    {
	fd->radix = 10;
        while( Width > 0 && isdigit(*ch) )
	    ACCEPTANDLOAD;
    }

    *bufp = 0;

    if( buf[0] == 0 )
	return( 0 );
    value = strtoul64(buf, NULL, fd->radix );
    fd->success = TRUE;
    return(value);

#undef ACCEPTANDLOAD
}

#ifndef __NO_FLOATING_POINT
static double getFloat(char *ch, fd_t *fd )
{
#define ACCEPTANDLOAD		    \
	( Width--,		    \
	  *(++bufp) = (*GetFunction)() )

    double value;
    char buf[FIELDMAX+1];
    char *bufp;
    int Width;

    bufp = buf;

    if(fd->fieldwidth == 0)
	Width = FIELDMAX;
    else
	Width = fd->fieldwidth;
    *bufp = *ch;

    /* save the sign value incase the calling program needs it. */

    if( Width > 0 && *bufp == '-' )
    {
	fd->sign = TRUE;
	ACCEPTANDLOAD;
    }

    while( Width > 0 && isdigit(*bufp) )
	ACCEPTANDLOAD;

    if( Width > 0 && *bufp == '.')
	ACCEPTANDLOAD;

    while( Width > 0 && isdigit(*bufp) )
	ACCEPTANDLOAD;

    if( Width > 0 && ((*bufp == 'e') || (*bufp == 'E')) )
    {
	int CharsRead = 1;
	ACCEPTANDLOAD;
	if( Width > 0 && ((*bufp == '+') || (*bufp == '-')) )
	{
	    CharsRead++;
	    ACCEPTANDLOAD;
	}

        if( Width > 0 && isdigit(*bufp) )
	{
	    do
		ACCEPTANDLOAD;
	    while( Width > 0 && isdigit(*bufp) );
	}
	else
    	    /* if the exponent is malformed then put the characters
    	     * back so that the next pattern can get them */
	    while( CharsRead-- )
		UngetFunction( *(bufp--) );
    }

    *ch = *bufp;    /* put back the prefetch char */
    *bufp = 0;

    if( buf[0] == 0 )
	return( 0 );
    value = atof( buf );
    fd->success = TRUE;
    return(value);

#undef ACCEPTANDLOAD
}
#endif /* __NO_FLOATING_POINT */
