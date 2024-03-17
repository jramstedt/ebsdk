#ifndef __SX164_H_LOADED
#define __SX164_H_LOADED
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
 *  $Id: sx164.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $
 */

/*
 * $Log: sx164.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1997/05/31  03:29:26  fdh
 * Added definitions which override the defaults for
 * the definitions which determine flash usage.
 *
 * Revision 1.3  1997/05/19  15:27:04  fdh
 * Corrected spelling for DISABLE_FLASH_ADDRESS19.
 *
 * Revision 1.2  1997/05/08  18:24:24  fdh
 * Modified to properly define extensions to the eb164.h file.
 *
 * Revision 1.1  1997/05/01  20:12:10  pbell
 * Initial revision
 */

#ifdef SX164
/*
 * Declare special definitions for systems with only
 * minor variations from the standard system described
 * by this file.  To build for a variant board, declare
 * the variant definition in addition to the standard
 * definition.  example... -DEB164 -DSX164
 */
#include "eb164.h"

#undef TARGET_NAME
#define TARGET_NAME "SX164"
#undef BANNER
#define BANNER " ************* AlphaPC 164SX Debug Monitor *************"
#undef PROMPT
#define PROMPT "AlphaPC164SX> "

/*
 * FSB_SINGLE_FLASH_IMAGE defines the default flash usage to
 * reserve the first 64KB segment for the Fail-Safe booter.
 * The remaining segments will accomodate one other firmware
 * image and its environment space if required.
 */
#define FSB_SINGLE_FLASH_IMAGE

/* Defines flash usage. */
#define FW_FSB_OFFSET 0x10000
#define FW_DBM_OFFSET 0x20000
#define FW_WNT_OFFSET 0x20000
#define FW_SRM_OFFSET 0x20000
#define FW_LNX_OFFSET 0x20000
#define FW_VXW_OFFSET 0x20000
#define FW__SR_OFFSET 0x00000
#define FW_DEF_OFFSET 0x20000

#define DISABLE_BOOTOPTION

#define NEED_SMC_ULTRA_IO

/*
 * AMD am79c960 option
 */
#define NEEDAM79C960

#define CPU_SUPPORTS_BYTE_WORD

/*
 * ROM definitions.
 */
#undef	ROMBASE
#define ROMBASE 0

#ifdef NEEDFLASHMEMORY
#define AMD_29LV800		    /* Build in support for this Flash part. */

#undef	FLASH_WINDOW_SIZE
#define FLASH_WINDOW_SIZE  0x100000 /* 1MB */

#define DISABLE_FLASH_ADDRESS19
#define NEEDPRIVATEFLASHBUS
#endif /* NEEDFLASHMEMORY */

#endif /* SX164 */
#endif /* __SX164_H_LOADED */
