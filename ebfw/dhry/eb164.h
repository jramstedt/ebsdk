#ifndef __EB164_H_LOADED
#define __EB164_H_LOADED
/*****************************************************************************

Copyright © 1993, Digital Equipment Corporation, Maynard, Massachusetts. 

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
 *  $Id: eb164.h,v 1.1.1.1 1998/12/29 21:36:12 paradis Exp $;
 */

/*
 * $Log: eb164.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:12  paradis
 * Initial CVS checkin
 *
 * Revision 1.14  1997/02/21  04:21:24  fdh
 * Removed include cia.h.
 *
 * Revision 1.13  1996/08/19 04:08:24  fdh
 * Removed an obsoleted definition.
 *
 * Revision 1.12  1996/05/25  10:52:10  fdh
 * Disabled am79c960 driver again after it was enabled
 * for testing.
 *
 * Revision 1.11  1996/05/22  21:27:25  fdh
 * Modified am79c960 definitions.
 *
 * Revision 1.10  1995/12/17  22:48:00  fdh
 * Added TARGET_NAME definition.
 *
 * Revision 1.9  1995/12/15  19:29:31  cruz
 * Updated banner to reflect the name change from DECChip to Alpha.
 *
 * Revision 1.8  1995/12/15  16:43:18  cruz
 * Defined FWUPDATE_FILE_ALT.
 *
 * Revision 1.7  1995/11/09  21:26:23  fdh
 * Disabled the am79c960 ethernet driver
 * to create a smaller ROM image.
 *
 * Revision 1.6  1995/11/01  16:25:45  cruz
 * Replaced ALLIED_TELESIS_AT_1500T by NEEDAM79C960_OPTIONAL.
 *
 * Revision 1.5  1995/10/03  20:13:58  fdh
 * Added compile conditionals to enable graphics console.
 *
 * Revision 1.4  1995/07/05  18:13:59  cruz
 * Added constants needed for Flash drivers.
 *
 * Revision 1.3  1995/02/10  02:21:36  fdh
 * Corrected EB164 banner.
 *
 * Revision 1.2  1994/12/07  21:24:58  cruz
 * Added constant defining the maximum size of the BCache
 *
 * Revision 1.1  1994/11/23  19:47:30  cruz
 * Initial revision
 *
 *
 */

#ifdef EB164

#define TARGET_NAME "EB164"
#define BANNER "Alpha 21164 Evaluation Board (EB164) Debug Monitor"
#define PROMPT "EB164> "

/****************************************************************************
 * Basic                                                                    *
 ****************************************************************************/

#define NEEDPCI
#define NEEDDEBUGGER
#define NEEDFLOPPY

#ifndef DISABLE_GRAPHICS
#define NEEDVGA
#define NEEDVGA_BIOSEMULATION
#define NEEDTGA     /* Digital 21030 */
#endif

/****************************************************************************
 * Plug in cards, what does it have?                                        *
 ****************************************************************************/
/*
 * AMD am79c960 option
 */
#if 0  /* Disabled to create a smaller ROM image */
#define NEEDAM79C960
#endif

/*
 *  The DEC Etherworks ISA card is a LANCE like device.
 */
#define DEC_ETHERWORKS

#define NEED21040


/****************************************************************************
 * ISA Address Space                                                        *
 ****************************************************************************/

#define RTCBASE                             0x70
#define LEDPORT                             0x80  /* JDR Microdevices P.O.S.T. Diagnostics Card. */


/*
 * If an optional ISA plug-in card is present, then it's treated like another
 * am79c960 as far as the driver is concerned, see /h/am79c960.h
 * for more definitions.
 */
#ifdef NEEDAM79C960_OPTIONAL
#define OPTIONAL_AM79C960		/* warning only define this once - kmc	*/
#define NEEDAM79C960			/* same for this 			*/
#define OPTIONAL_AM79C960_BASE              0x360
#define OPTIONAL_AM79C960_INT               9

/*
 * This parameter determines the
 * DMA Mask for the DMA2 controller.
 *       Mask     Channel
 *       ----     -------
 *        1          5
 *        2          6
 *        3          7
 *        4          8
 */
#define OPTIONAL_AM79C960_DMA               1 /* Selects DMA Channel 5 */
#endif

#ifdef DEC_ETHERWORKS
#define DEC_ETHERWORKS_BASE                 0x300
#endif

#ifdef DEC_ETHERWORKS
#define DEC_ETHERWORKS_BASE                 0x300
#endif


/****************************************************************************
 * System Address Space                                                     *
 ****************************************************************************/

#define MINIMUM_SYSTEM_MEMORY		    0x1000000 /* 16MB */
#define MAXIMUM_SYSTEM_CACHE     	    0x800000  /* 8MB */
#define DMA_BUFF_BASE                       0x100000


/****************************************************************************
 * PCI I/O Address Space                                                    *
 ****************************************************************************/
/*
 * Definitions for the windows mapping PCI addresses into
 * system addresses
 *
 * 16 megabyte window starting at CPU address = 0.
 */

#define PCI_BASE_1_USED                     1
#define PCI_BASE_1                          0x000100000
#define PCI_MASK_1                          0x000000000
#define PCI_TBASE_1                         0x000100000

#define PCI_BASE_2_USED                     0
#define PCI_BASE_2                          0x000000000
#define PCI_MASK_2                          0x000000000
#define PCI_TBASE_2                         0x000000000

#define PCI_BASE_3_USED                     0
#define PCI_BASE_3                          0x000000000
#define PCI_MASK_3                          0x000000000
#define PCI_TBASE_3                         0x000000000

#define PCI_BASE_4_USED                     0
#define PCI_BASE_4                          0x000000000
#define PCI_MASK_4                          0x00000000
#define PCI_TBASE_4                         0x000000000

/*
 * Each mask translates to a number of these units.  For
 * APECS this unit is 1Kbyte.
 */
#define PCI_MASK_UNIT                       0x100000

/*
 * ROM definitions.
 */
#define ROMBASE 0xFFF80000
#define ROMSIZE 0x100000
#define ROMINC 0x1

#define NEEDFLASHMEMORY

#ifdef NEEDFLASHMEMORY
#define INTEL_28F008SA      		/* Build in support for this Flash part. */
#define AMD_29F080         		/* Build in support for this Flash part. */
#define FLASH_ADDRESS   ROMBASE
#define HAE_MASK        (0xe0000000)
#define FLASH_BASE_ADDRESS_PHYSICAL FLASH_ADDRESS
#define FLASH_ACCESS_HAE_MASK HAE_MASK
#define FLASH_ACCESS_HAE_VALUE (FLASH_ADDRESS & HAE_MASK)
#define FLASH_WINDOW_SIZE  0x80000   /* 512 kB */
#define FLASH_ADDRESS19_PORT_PHYSICAL 0x800
#endif /* NEEDFLASHMEMORY */

#endif /* EB164 */
#endif /* __EB164_H_LOADED */

