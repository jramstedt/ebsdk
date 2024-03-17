#ifndef __CPU_H_LOADED
#define __CPU_H_LOADED
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
 * $Id: cpu.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $;
 */

/*
 * $Log: cpu.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1997/12/15  18:59:24  pbell
 * Added the USE_OLD_MINI_DEBUGGER flag for EV4 platforms.
 *
 * Revision 1.3  1997/07/10  02:04:02  fdh
 * Added dc21264.h.
 *
 * Revision 1.2  1997/05/23  11:14:18  fdh
 * Merged dc21164p.h into dc21164.h
 *
 * Revision 1.1  1997/05/01  19:50:36  pbell
 * Initial revision
 *
 */

#if defined( DC21064 ) || defined( DC21066 )
#include "dc21064.h"
#define USE_OLD_MINI_DEBUGGER
#endif

#if defined(DC21164) || defined(DC21164PC)
#include "dc21164.h"
#endif

#if defined(DC21264)
#include "dc21264.h"
#endif

#endif /* __CPU_H_LOADED */
