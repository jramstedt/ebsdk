#ifndef __LX164MEM_H_LOADED
#define __LX164MEM_H_LOADED
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
 *  $Id: sx164mem.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 */

/*
 * $Log: sx164mem.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1997/05/23  12:48:09  fdh
 * Merged dc21164p.h into dc21164.h.
 *
 * Revision 1.1  1997/05/01  20:12:19  pbell
 * Initial revision
 *
 *
 */

#ifdef SX164

#include "system.h"
#include "lib.h"
#include "pyxis.h"

#ifndef MAKEDEPEND
#include "cserve.h"
#include "dc21164.h"
#endif


/*
 *  Macros used to access the memory controller
 *  csr's.
 */
#define MEM_MCR(x)        (((ul)MC_MCR_BASE<<MC_MCR_BASE_SHIFT)|(ul)(x))
#define _IN_MEM_MCR(p)    (ReadL(MEM_MCR(p)))
#define _OUT_MEM_MCR(p,d) WriteL((MEM_MCR(p)),d);mb();

#define MEM_GCR(x)        (((ul)MC_GCR_BASE<<MC_GCR_BASE_SHIFT)|(ul)(x))
#define _IN_MEM_GCR(p)    (ReadL(MEM_GCR(p)))
#define _OUT_MEM_GCR(p,d) WriteL((MEM_GCR(p)),d);mb();

#endif /* SX164 */
#endif /* __SX164MEM_H_LOADED */
