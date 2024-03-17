
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
static char *rcsid = "$Id: hello.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: hello.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.6  1995/08/25  20:27:27  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.5  1994/11/07  12:55:16  rusling
 * Now include system.h as well as lib.h.
 *
 * Revision 1.4  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/06/22  13:22:29  rusling
 * Fixed up OSF build problem (removed definition of printf())
 *
 * Revision 1.2  1994/06/21  10:22:07  rusling
 * Now include lib.h for definitions.
 *
 * Revision 1.1  1993/06/08  19:56:47  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "lib.h"

main()
{
    printf ("Hello World\n\n");
    return 1;
}

