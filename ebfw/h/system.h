#ifndef __SYSTEM_H_LOADED
#define __SYSTEM_H_LOADED
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
 *  $Id: system.h,v 1.2 1999/04/15 21:30:41 paradis Exp $;
 */

/*
 * $Log: system.h,v $
 * Revision 1.2  1999/04/15 21:30:41  paradis
 * Scrubbed out RX164 references
 *
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.15  1997/07/10  00:39:05  fdh
 * added dp264.h.
 *
 * Revision 1.14  1997/05/01  20:13:15  pbell
 * Added SX references.
 *
 * Revision 1.13  1997/02/21  04:23:06  bissen
 * Added lx164.h
 *
 * Revision 1.12  1996/02/20 18:41:52  cruz
 * Added PC164.
 *
 * Revision 1.11  1995/10/10  14:27:40  fdh
 * Renamed eb64l to pc64.
 *
 * Revision 1.10  1994/12/08  16:42:29  fdh
 * Added includes for eb66p.h and pc64.h
 *
 * Revision 1.9  1994/11/23  19:45:52  cruz
 * Added eb164.h
 *
 * Revision 1.8  1994/11/08  21:44:50  fdh
 * Removed special compiler definitions.  They now only
 * appear in lib.h
 *
 * Revision 1.7  1994/08/05  20:13:47  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.6  1994/06/20  14:18:59  fdh
 * Fixup header file preprocessor #include conditionals.
 *
 * Revision 1.5  1994/06/17  19:34:01  fdh
 * Clean-up...
 *
 * Revision 1.4  1994/03/16  00:12:31  fdh
 * Use TRUE and FALSE defines for all environments.
 *
 * Revision 1.3  1994/01/19  10:22:28  rusling
 * Ported to ALpha Windows NT.
 *
 * Revision 1.2  1993/11/22  13:17:13  rusling
 * Merged with PCI/21040 changes.
 *
 * Revision 1.1  1993/11/22  11:44:09  rusling
 * Initial revision
 *
 * Revision 1.1  1993/11/22  11:44:09  rusling
 * Initial revision
 *
 */


/*
 * Per evaluation board include files 
 */
#include "eb64.h"
#include "eb66.h"
#include "eb66p.h"
#include "eb64p.h"
#include "pc64.h"
#include "eb164.h"
#include "pc164.h"
#include "lx164.h"
#include "sx164.h"
#include "dp264.h"

#endif /* __SYSTEM_H_LOADED */
