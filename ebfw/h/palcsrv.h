#ifndef PALCSRV_H_LOADED
#define PALCSRV_H_LOADED
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
 *  $Id: palcsrv.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 *  $Log: palcsrv.h,v $
 *  Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 *  Initial CVS checkin
 *
 * Revision 1.4  1998/08/26  18:50:16  thulin
 * *** empty log message ***
 *
 * Revision 1.3  1998/08/26  18:31:54  thulin
 * Add support for CLUBS
 *
 * Revision 1.2  1997/07/10  00:32:26  fdh
 * included ev6_csrv.h.
 *
 * Revision 1.1  1997/02/21  04:19:26  fdh
 * Initial revision
 *
 */

#ifndef MAKEDEPEND
#if (defined(DP264) || defined(CLUBS))
#include "ev6_csrv.h"
#else
#include "cserve.h"
#endif /* DP264 || CLUBS*/
#endif

#endif /* PALCSRV_H_LOADED */
