#ifndef __SMC_37c669_H__
#define __SMC_37c669_H__
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
 * $Id: smcc669.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 *
 * Abstract:
 *
 *  This file contains definitions necessary for the combo
 *  driver for the SMC 37c669 Super I/O chip.
 *
 * Author:
 *
 *  Jeff Wiedemeier   25-Nov-1996
 *
 * $Log: smcc669.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/05/01  20:10:21  pbell
 * Initial revision
 *
 */

/*
 *  configuration on/off keys
 */
#define CONFIG_ON_KEY  0x55
#define CONFIG_OFF_KEY 0xAA
#define DEVICE_ID      0x0D

/*
 *  default device addresses
 */
#define COM1_BASE             0x3f8
#define COM1_INTERRUPT        4
#define COM2_BASE             0x2f8
#define COM2_INTERRUPT        3
#define PARP_BASE	      0x3BC
#define PARP_INTERRUPT	      7

//++
// Constants
//--
#define SMC37c669_DEVICE_ID     (0x3)

//
// 37c669 device IRQs
//
#define SMC37c669_DEVICE_IRQ_A         (SMC37c669_DEVICE_IRQ(0x01))
#define SMC37c669_DEVICE_IRQ_B         (SMC37c669_DEVICE_IRQ(0x02))
#define SMC37c669_DEVICE_IRQ_C         (SMC37c669_DEVICE_IRQ(0x03))
#define SMC37c669_DEVICE_IRQ_D         (SMC37c669_DEVICE_IRQ(0x04))
#define SMC37c669_DEVICE_IRQ_E         (SMC37c669_DEVICE_IRQ(0x05))
#define SMC37c669_DEVICE_IRQ_F         (SMC37c669_DEVICE_IRQ(0x06))
/*      SMC37c669_DEVICE_IRQ_G         ***RESERVED*** (spec p134)*/
#define SMC37c669_DEVICE_IRQ_H         (SMC37c669_DEVICE_IRQ(0x08))

//
// 37c669 device DMA Channels
//
#define SMC37c669_DEVICE_DMA_A         (SMC37c669_DEVICE_DMA(0x01))
#define SMC37c669_DEVICE_DMA_B         (SMC37c669_DEVICE_DMA(0x02))
#define SMC37c669_DEVICE_DMA_C         (SMC37c669_DEVICE_DMA(0x03))

//++
// Structure definitions
//--

//++
// Config register indexes
//--
#define SMC37c669_CR00_INDEX    (0x00)
#define SMC37c669_CR01_INDEX    (0x01)
#define SMC37c669_CR02_INDEX    (0x02)
#define SMC37c669_CR03_INDEX    (0x03)
#define SMC37c669_CR04_INDEX    (0x04)
#define SMC37c669_CR05_INDEX    (0x05)
#define SMC37c669_CR06_INDEX    (0x06)
#define SMC37c669_CR07_INDEX    (0x07)
#define SMC37c669_CR08_INDEX    (0x08)
#define SMC37c669_CR09_INDEX    (0x09)
#define SMC37c669_CR10_INDEX    (0x10)
#define SMC37c669_CR0A_INDEX    (0x0A)
#define SMC37c669_CR0B_INDEX    (0x0B)
#define SMC37c669_CR0C_INDEX    (0x0C)
#define SMC37c669_CR0D_INDEX    (0x0D)
#define SMC37c669_CR0E_INDEX    (0x0E)
#define SMC37c669_CR0F_INDEX    (0x0F)
#define SMC37c669_CR11_INDEX    (0x11)
#define SMC37c669_CR12_INDEX    (0x12)
#define SMC37c669_CR13_INDEX    (0x13)
#define SMC37c669_CR14_INDEX    (0x14)
#define SMC37c669_CR15_INDEX    (0x15)
#define SMC37c669_CR16_INDEX    (0x16)
#define SMC37c669_CR17_INDEX    (0x17)
#define SMC37c669_CR18_INDEX    (0x18)
#define SMC37c669_CR19_INDEX    (0x19)
#define SMC37c669_CR1A_INDEX    (0x1A)
#define SMC37c669_CR1B_INDEX    (0x1B)
#define SMC37c669_CR1C_INDEX    (0x1C)
#define SMC37c669_CR1D_INDEX    (0x1D)
#define SMC37c669_CR1E_INDEX    (0x1E)
#define SMC37c669_CR1F_INDEX    (0x1F)
#define SMC37c669_CR20_INDEX    (0x20)
#define SMC37c669_CR21_INDEX    (0x21)
#define SMC37c669_CR22_INDEX    (0x22)
#define SMC37c669_CR23_INDEX    (0x23)
#define SMC37c669_CR24_INDEX    (0x24)
#define SMC37c669_CR25_INDEX    (0x25)
#define SMC37c669_CR26_INDEX    (0x26)
#define SMC37c669_CR27_INDEX    (0x27)
#define SMC37c669_CR28_INDEX    (0x28)
#define SMC37c669_CR29_INDEX    (0x29)

//++
// Aliases for some config registers
//--
#define SMC37c669_DEVICE_ID_INDEX               (SMC37c669_CR0D_INDEX)
#define SMC37c669_DEVICE_REVISION_INDEX         (SMC37c669_CR0E_INDEX)
#define SMC37c669_FDC_BASE_ADDRESS_INDEX        (SMC37c669_CR20_INDEX)
#define SMC37c669_IDE_BASE_ADDRESS_INDEX        (SMC37c669_CR21_INDEX)
#define SMC37c669_IDE_ALTERNATE_ADDRESS_INDEX   (SMC37c669_CR22_INDEX)
#define SMC37c669_PARALLEL0_BASE_ADDRESS_INDEX  (SMC37c669_CR23_INDEX)
#define SMC37c669_SERIAL0_BASE_ADDRESS_INDEX    (SMC37c669_CR24_INDEX)
#define SMC37c669_SERIAL1_BASE_ADDRESS_INDEX    (SMC37c669_CR25_INDEX)
#define SMC37c669_PARALLEL_FDC_DMA_INDEX        (SMC37c669_CR26_INDEX)
#define SMC37c669_PARALLEL_FDC_IRQ_INDEX        (SMC37c669_CR27_INDEX)
#define SMC37c669_SERIAL_IRQ_INDEX              (SMC37c669_CR28_INDEX)

#define	FDC_BASE	(0x3F0)

#endif // __SMC_37c669_H__
