#ifndef __PC64_H_LOADED
#define __PC64_H_LOADED
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
 *  $Id: pc64.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 */

/*
 * $Log: pc64.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.7  1996/05/09  18:15:41  fdh
 * Updated banner.
 *
 * Revision 1.6  1995/12/17  22:48:00  fdh
 * Added TARGET_NAME definition.
 *
 * Revision 1.5  1995/12/15  16:43:00  cruz
 * Defined FWUPDATE_FILE_ALT.
 *
 * Revision 1.4  1995/10/10  15:14:48  fdh
 * Renamed EB64L to PC64.
 *
 * Revision 1.3  1995/07/05  20:14:07  cruz
 * Added constants needed by flash drivers.
 *
 * Revision 1.2  1995/02/10  02:21:59  fdh
 * Corrected AlphaPC64 banner and prompt.
 *
 * Revision 1.1  1994/12/08  16:42:29  fdh
 * Initial revision
 *
 */

#ifdef PC64
/*
 * Declare special definitions for systems with only
 * minor variations from the standard system described
 * by this file.  To build for a variant board, declare
 * the variant definition in addition to the standard
 * definition.  example... -DEB64P -DPC64
 */
#include "eb64p.h"

#undef TARGET_NAME
#define TARGET_NAME "PC64"
#undef BANNER
#define BANNER "AlphaPC 64 Evaluation Board Debug Monitor"
#undef PROMPT
#define PROMPT "AlphaPC64> "
#undef ROMSIZE
#define ROMSIZE 0x100000

#define NEEDFLASHMEMORY
#ifdef NEEDFLASHMEMORY
#define INTEL_28F008SA      		/* Build in support for this Flash part. */
#define AMD_29F080         		/* Build in support for this Flash part. */
#define FLASH_ADDRESS   ROMBASE
#define HAE_MASK        (0xF8000000)
#define FLASH_BASE_ADDRESS_PHYSICAL FLASH_ADDRESS
#define FLASH_ACCESS_HAE_MASK HAE_MASK
#define FLASH_ACCESS_HAE_VALUE (FLASH_ADDRESS & HAE_MASK)
#define FLASH_WINDOW_SIZE  0x80000   /* 512 kB */
#define FLASH_ADDRESS19_PORT_PHYSICAL 0x800
#endif /* NEEDFLASHMEMORY */

#endif /* PC64 */
#endif /* __PC64_H_LOADED */
