#ifndef __CLUBS_H_LOADED
#define __CLUBS_H_LOADED
/*****************************************************************************

Copyright 1998, Digital Equipment Corporation, Maynard, Massachusetts. 

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
 *  $Id: clubs.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $
 */

/*
 * $Log: clubs.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1998/08/26  18:59:55  thulin
 * *** empty log message ***
 *
 * Revision 1.1  1998/08/26  17:41:34  thulin
 * Initial revision
 *
 *
 */

#ifdef CLUBS

#if 1
#define SROM_PORT_BOOT
#endif

#define TARGET_NAME "CLUBS"
#define BANNER "Alpha Clubs Debug Monitor"
#define PROMPT "Clubs> "

/****************************************************************************
 * Basic                                                                    *
 ****************************************************************************/

#define NEEDPCI
#define NEEDDEBUGGER
#define NEEDFLOPPY
#define DEBUG_FLOPPY_DRIVER

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
#define NEEDAM79C960

/*
 *  The DEC Etherworks ISA card is a LANCE like device.
 */
#define DEC_ETHERWORKS

#ifdef NEEDPCI
#define NEED21040
#endif

/****************************************************************************
 * ISA Address Space                                                        *
 ****************************************************************************/

#define RTCBASE                             0x70
#define LEDPORT                             0x80  /* JDR Microdevices P.O.S.T. Diagnostics Card. */


/*
 * AMD am79c960 option
 */
#define NEEDAM79C960

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

#define MINIMUM_SYSTEM_MEMORY		    0x4000000
#define MAXIMUM_SYSTEM_CACHE     	    0x800000
#define DMA_BUFF_BASE                       0x100000
#define DEFAULT_BOOTADR			    0x300000


/****************************************************************************
 * PCI I/O Address Space                                                    *
 ****************************************************************************/
/*
 *  Defines the limits for the id_sel lines to probe.
 */
#define PCI_T0_FIRST_IDSEL	0
#define PCI_T0_LAST_IDSEL	20

/*
 * Each mask translates to a number of these units.  For
 * APECS this unit is 1Kbyte.
 */
#define PCI_MASK_UNIT                       0x800000


/*
 * ROM definitions.
 */
#define ROMBASE 0x0
#define ROMSIZE 0x200000
#define ROMINC 0x1

#define NEEDFLASHMEMORY

#ifdef NEEDFLASHMEMORY
#define FLASH_ACCESS_HAE_VALUE 0 /* Shouldn't need this */
#define FLASH_BASE_ADDRESS_PHYSICAL 0 /* Shouldn't need this */
#define FLASH_ACCESS_HAE_MASK 0 /* Shouldn't need this */
#define FLASH_ADDRESS19_PORT_PHYSICAL 0x800 /* Shouldn't need this */

#define AMD_FLASH
#define FLASH_WINDOW_SIZE  0x100000 /* 1MB */
#define DISABLE_FLASH_ADDRESS19
#define NEEDPRIVATEFLASHBUS
#endif /* NEEDFLASHMEMORY */


/*
 * FSB_SINGLE_FLASH_IMAGE defines the default flash usage to
 * reserve the first 64KB segment for the Fail-Safe booter.
 * The remaining segments will accomodate one other firmware
 * image and its environment space if required.
 */
#define FSB_SINGLE_FLASH_IMAGE

/* Defines flash usage. */
#define FW_FSB_OFFSET 0x00000
#define FW_DBM_OFFSET 0x20000
#define FW_WNT_OFFSET 0x80000
#define FW_SRM_OFFSET 0x100000
#define FW_LNX_OFFSET 0x20000
#define FW_VXW_OFFSET 0x20000
#define FW_DEF_OFFSET 0x20000


#define NEED_SMC_ULTRA_IO

#define CPU_SUPPORTS_BYTE_WORD

/* Memory Test access size */
#define MT_DATA_TYPE ul
#define MTRead ReadQ
#define MTWrite WriteQ
#define MTSeed 0x5a5a5a5a5a5a5a5aL

#endif /* CLUBS */
#endif /* __CLUBS_H_LOADED */

