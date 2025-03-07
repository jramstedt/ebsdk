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
__attribute__((unused)) static const char *rcsid = "$Id: strtoul.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: strtoul.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1997/05/29  17:45:47  pbell
 * Added 64 bit string to integer function strtoul64
 *
 * Revision 1.2  1997/04/23  17:43:34  pbell
 * Fixed sign handling.
 *
 * Revision 1.1  1997/02/21  03:30:41  fdh
 * Initial revision
 *
 */

#include <stddef.h>
#include "lib.h"

ul strtoul64( const char *nptr, char **endptr, int base)
{
  const char *ptr;
  ul value;
  int sign;
  int ch;

  ptr = nptr;

  /* Ignore white space */
  while (isspace((int)*ptr))
    ++ptr;

  if ((*ptr == '\0') || (base < 0) || (base == 1)) {
    if (endptr != NULL)
       *endptr = (char *) nptr;
    return 0;			/* Nothing to do */
  }

  /* Work out the sign */
  if (*ptr == '-')
       sign = TRUE, ++ptr;
  else if (*ptr == '+')
       sign = FALSE, ++ptr;
  else sign = FALSE;

  /* Hexadecimal */
  if ((base == 0) || (base == 16)) {
    if((ptr[0] == '0') &&
       ((ptr[1] == 'x') ||  (ptr[1] == 'X'))) {
      base = 16;
      ptr += 2;			/* Skip over 0x */
    }
  }

  /* Octal */
  if ((base == 0) || (base == 8)) {
    while (*ptr == '0') {
      base = 8;			/* If leading zero base = 8 */
      ++ptr;			/* Ignore leading zeros */
    }
  }

  /* Decimal */
  if (base == 0) base = 10;

  value = 0;
  for (ch = *ptr; ch != '\0'; ch = *++ptr) {

    if (isdigit(ch))
      ch -= '0';
    else if (isalpha(ch))
      ch = tolower(ch) - 'a' + 10;
    else break;

    if (ch >= base) break;
    value *= (ul) base;
    value += (ul) ch;
  }

  if (endptr != NULL)
    *endptr = (char *) nptr;
  return( sign ? ((ul)(-(sl)value)) : value );
}


unsigned long int strtoul( const char *nptr, char **endptr, int base)
{
    return( (unsigned long)strtoul64( nptr, endptr, base ) );
}

long int strtol( const char *nptr, char **endptr, int base)
{
  return((long) strtoul64(nptr, endptr, base));
}

int atoi(const char *nptr)
{
  return((int) strtoul64(nptr, (char **) NULL, 10));
}

long atol(const char *nptr)
{
  return((long) strtoul64(nptr, (char **) NULL, 10));
}

