
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
__attribute__((unused)) static const char *rcsid = "$Id: ident.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: ident.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.9  1997/05/28  15:16:41  fdh
 * Corrected parameter passed to ReadB().
 *
 * Revision 1.8  1995/10/26  23:44:39  cruz
 * Included console.h
 *
 * Revision 1.7  1995/09/27  21:32:41  cruz
 * Get rid of a warning because of prototype mismatch with strncmp.
 *
 * Revision 1.6  1994/11/07  12:05:39  rusling
 * Now include system.h as well as lib.h
 *
 * Revision 1.5  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.4  1994/06/23  13:43:30  rusling
 * Fixed up WNT compiler warnings.
 *
 * Revision 1.3  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.2  1993/06/18  14:40:08  fdh
 * Shorten page break...
 *
 * Revision 1.1  1993/06/09  20:25:25  fdh
 * Initial revision
 *
 */



/* Identify files by RCS $Id: ident.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $ keyword */
/* FDH 6/9/93 */

#include "system.h"
#include "lib.h"
#include "console.h"

const char *rcsidkeyarray[] = {
  "Author: ",
  "Date: ",
  "Header: ",
  "Id: ",
  "Locker: ",
  "Log: ",
  "RCSfile: ",
  "Revision: ",
  0
};

int ident( ul first, ul last)
{
  ul i;
  ul k;
  int found;
  int j;

  found = 0;
  i = first;
  while( i <= last ) {
    if (ReadB( i ) == '$') {
      j = 0;
      k = i+1;
      while (rcsidkeyarray[j] != 0) {
	if (strncmp((char *)k, rcsidkeyarray[j], strlen(rcsidkeyarray[j])) == 0)
	  {
	    found++;
	    if (((found % 22) == 0) && (!kbdcontinue()))
	      return(found);
	    printf("%s\n", (ub *)k);
	    i += strlen((char *)k);
	    break;
	  }
	j++;
      }
    }
    i = i + 1;
  }
  
  if( !found )
    printf( "No RCS identifiers found\n" );
  return(found);
}
