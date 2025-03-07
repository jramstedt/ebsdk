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
__attribute__((unused)) static const char *rcsid = "$Id: strtod.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * strtod
 *
 *	Convert string to double. 
 *
 * $Log: strtod.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/02/21  03:26:01  gries
 * Initial revision
 *
 */

#include "lib.h"
#include "ctype.h"

double strtod(const char *str, char **ptr)
{
  char *scanptr = (char *)str ,point='.';

  /* Skip whitespace then parse fraction.
   */
  while (isspace(*scanptr)) scanptr++;

  if (*scanptr == '+' || *scanptr == '-') scanptr++ ;

  /*
   * Ensure string can be converted
   */
  if (*scanptr == point && isdigit(scanptr[1])) 
    {
      scanptr+= 2;
      while (isdigit(*scanptr)) scanptr++;
    }
  else
    {
      while (isdigit(*scanptr)) scanptr++;
      if (*scanptr == point) 
	{
	  scanptr++;
	  while (isdigit(*scanptr)) scanptr++;
	}
    }
  /* Parse exponent.*/
  if (*scanptr == 'E' || *scanptr == 'e') 
    {
      char *e_ptr = scanptr++;

      if (*scanptr == '+' || *scanptr == '-') scanptr++ ;

      if (isdigit(*scanptr++)) 
	while (isdigit(*scanptr)) scanptr++;
      else {
	/* Return pointer to beginning of 'E'|'e' if
	 * this wasn't a well formed exponent.
	 */
	scanptr = e_ptr ;
      } 
	
      /* Return pointer to terminating character
       */
      if (ptr != NULL) *ptr = (char *)scanptr;
    }

    /* Return converted string
    */
    return(atof(str));
}
