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
static char *rcsid = "$Id: biosem.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * $Log: biosem.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1997/04/24  21:15:17  pbell
 * Cleaned up warnings
 *
 * Revision 1.3  1997/02/24  22:11:50  fdh
 * Report calls to stubs...
 *
 * Revision 1.2  1995/11/10  18:37:25  cruz
 * Added return statement to a routine requiring one.
 *
 * Revision 1.1  1995/08/25  20:45:17  fdh
 * Initial revision
 *
 */

#include "lib.h"

int StartBiosEmulator(unsigned int *bios_return)
{
  puts("StartBiosEmulator() called");
  return 0;
}

void    SetupBiosGraphics(void)
{
  puts("SetupBiosGraphics() called");
}
