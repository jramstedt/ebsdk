#ifndef __LX164_H_LOADED
#define __LX164_H_LOADED
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
 *  $Id: lx164.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $
 */

/*
 * $Log: lx164.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1997/03/26  15:08:05  pbell
 * Added the CPU_SUPPORTS_BYTE_WORD definition.
 *
 * Revision 1.1  1997/02/21  04:19:46  fdh
 * Initial revision
 *
 */

#ifdef LX164
/*
 * Declare special definitions for systems with only
 * minor variations from the standard system described
 * by this file.  To build for a variant board, declare
 * the variant definition in addition to the standard
 * definition.  example... -DEB164 -DLX164
 */
#include "eb164.h"

#undef TARGET_NAME
#define TARGET_NAME "LX164"
#undef BANNER
#define BANNER " ************ AlphaPC 164LX Debug Monitor ************"
#undef PROMPT
#define PROMPT "AlphaPC164LX> "

/*
 * FSB_SINGLE_FLASH_IMAGE defines the default flash usage to
 * reserve the first 64KB segment for the Fail-Safe booter.
 * The remaining segments will accomodate one other firmware
 * image and its environment space if required.
 */
#define FSB_SINGLE_FLASH_IMAGE
#define DISABLE_BOOTOPTION

#define NEED_SMC_ULTRA_IO

/*
 * AMD am79c960 option
 */
#define NEEDAM79C960

#define CPU_SUPPORTS_BYTE_WORD
#endif /* LX164 */
#endif /* __LX164_H_LOADED */
