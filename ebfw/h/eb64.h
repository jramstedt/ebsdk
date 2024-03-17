#ifndef __EB64_H_LOADED
#define __EB64_H_LOADED
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
 *  $Id: eb64.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $;
 */

/*
 * $Log: eb64.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.29  1996/06/03  18:28:49  fdh
 * Define NEEDAM79C960 only if not already defined.
 *
 * Revision 1.28  1996/05/26  20:00:11  fdh
 * Modified the am79c960 definitions to use dma channel insted of
 * the dma mask.
 *
 * Revision 1.27  1996/05/22  21:26:44  fdh
 * Modified am79c960 definitions.
 *
 * Revision 1.26  1996/01/18  12:23:10  cruz
 * If DISABLE_ED_DRIVE is defined, then assume that drives are High Density drives.
 *
 * Revision 1.25  1995/12/17  22:48:00  fdh
 * Added TARGET_NAME definition.
 *
 * Revision 1.24  1995/12/15  19:28:48  cruz
 * Updated banner to reflect the name change from DECChip to Alpha.
 *
 * Revision 1.23  1995/12/15  16:40:38  cruz
 *  Defined FWUPDATE_FILE_ALT.
 *
 * Revision 1.22  1995/11/01  16:11:15  cruz
 * Put back a defined that was erased by mistake (OPTIONAL_AM79C960).
 *
 * Revision 1.21  1995/11/01  16:06:18  cruz
 * Removed NEEDTGA which was added by mistake.
 * Renamed ALLIED_TELESIS_AT_1500T to NEEDAM79C960_OPTIONAL.
 *
 * Revision 1.20  1995/10/03  20:13:49  fdh
 * Added compile conditionals to enable graphics console.
 *
 * Revision 1.19  1995/03/05  12:06:43  fdh
 * Adjusted ROMINC to access ROM bytes at
 * longword boundaries as required.
 *
 * Revision 1.18  1995/03/05  04:18:20  fdh
 * Changed ROMBASE and ROMINC definitions to use the
 * I/O bus addresses by using inrom().
 *
 * Revision 1.17  1995/02/10  02:56:08  fdh
 * Disabled bootopt command because the current SROM doesn't support it.
 *
 * Revision 1.16  1994/11/28  18:26:46  fdh
 * Added definition to enable the special ROM access
 * required for the EB64.
 *
 * Revision 1.15  1994/11/08  21:36:10  fdh
 * Added ROM definitions.  Moved from rom.c
 *
 * Revision 1.14  1994/08/05  20:13:47  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.13  1994/07/21  18:09:32  fdh
 * Added MAXIMUM_SYSTEM_CACHE definition.
 *
 * Revision 1.12  1994/06/20  14:18:59  fdh
 * Fixup header file preprocessor #include conditionals.
 *
 * Revision 1.11  1994/04/06  05:09:11  fdh
 * Removed an ethernet driver.
 *
 * Revision 1.10  1994/04/04  15:15:44  fdh
 * Added definition for LEDPORT.
 *
 * Revision 1.9  1994/04/01  13:57:22  fdh
 * Added MINIMUM_SYSTEM_MEMORY definition and
 * removed obsoleted PAL_IMPURE definition.
 *
 * Revision 1.8  1994/03/24  21:42:14  fdh
 * Removed unnecessary compile-time conditionals.
 *
 * Revision 1.7  1994/02/14  16:25:01  rusling
 * Allow remote debug support in the NT build case.
 *
 * Revision 1.6  1994/01/19  10:22:28  rusling
 * Ported to ALpha Windows NT.
 *
 * Revision 1.5  1993/11/22  15:24:30  rusling
 * Fixed up am79c960 embedded definitions.
 *
 * Revision 1.4  1993/11/22  14:20:41  rusling
 * Modified am79c960 definitions.
 *
 * Revision 1.3  1993/11/22  13:17:13  rusling
 * Merged with PCI/21040 changes.
 *
 * Revision 1.2  1993/11/22  12:16:20  rusling
 * Added in further definitions
 *
 * Revision 1.1  1993/11/22  11:42:50  rusling
 * Initial revision
 *
 */

#ifdef EB64

#define TARGET_NAME "EB64"
#define BANNER "Alpha 21064 Evaluation Board (EB64) Debug Monitor"
#define PROMPT "EB64> "

/****************************************************************************
 * Basic                                                                    *
 ****************************************************************************/

#define NEEDSCTL
#define NEEDDEBUGGER
#define NEEDFLOPPY
#define DISABLE_ED_DRIVE     /* Disables extended density floppy drive */
#define NEEDEB64SPECIALROMACCESS
#define DISABLEBOOTOPTION   /* Current SROM does not support this */

/* The am79c960 embedded in the EB64 is hardwired to these resources. */
#ifndef NEEDAM79C960
#define NEEDAM79C960
#endif /* NEEDAM79C960 */

#define AM79C960_BASE360_INTERRUPT	9
#define AM79C960_BASE360_DMACHANNEL	5

#ifndef DISABLE_GRAPHICS
#define NEEDVGA
#define NEEDVGA_BIOSEMULATION
#endif

/****************************************************************************
 * Plug in cards, what does it have?                                        *
 ****************************************************************************/
/*
 *  The DEC Etherworks ISA card is a LANCE like device.
 */
#define DEC_ETHERWORKS

/****************************************************************************
 * ISA Address Space                                                        *
 ****************************************************************************/

#define LEDPORT                             0x80  /* JDR Microdevices P.O.S.T. Diagnostics Card. */

#ifdef DEC_ETHERWORKS
#define DEC_ETHERWORKS_BASE                 0x300
#endif

/****************************************************************************
 * System Address Space                                                     *
 ****************************************************************************/

#define MINIMUM_SYSTEM_MEMORY    0x400000
#define MAXIMUM_SYSTEM_CACHE     0x80000
#define DMA_BUFF_BASE            0x100000

#define ROMBASE 0x0
#define ROMSIZE 0x80000
#define ROMINC 0x4

#endif /* EB64 */
#endif /* __EB64_H_LOADED */



