
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
static char *rcsid = "$Id: size.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: size.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1995/10/05  01:44:56  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.3  1995/08/25  20:27:27  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.2  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:52  fdh
 * Initial revision
 *
 */

#include "lib.h"

int main()
{
  printf("\nThis simple program prints the size of\n");
  printf("various data types in bytes.\n");

  printf("\n char   = %2d", sizeof(char));
  printf("\n short  = %2d", sizeof(short));
  printf("\n int    = %2d", sizeof(int));
  printf("\n long   = %2d", sizeof(long));
  printf("\n float  = %2d", sizeof(float));
  printf("\n double = %2d", sizeof(double));
  printf("\n\n");
  return (1);
}
