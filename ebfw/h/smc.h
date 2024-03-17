#ifndef __SMC_H_LOADED
#define __SMC_H_LOADED
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
 *  $Id: smc.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 *
 * author: Ken Curewitz
 * date: 7-aug-95
 *
 * definitions of registers and macros for accessing macros
 * for the SMC FDC37C93X Plug and Play Compatible Ultra I/O Controller
 *
 * $Log: smc.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1996/08/13  19:43:14  fdh
 * Added parallel port definitions.
 *
 * Revision 1.3  1996/07/30  19:44:45  fdh
 * Corrected the com port interrupt assignments.
 *
 * Revision 1.2  1996/05/23  13:18:22  fdh
 * Added copyright header.
 *
 */

/*
 *  configuration on/off keys
 */
#define CONFIG_ON_KEY  0x55
#define CONFIG_OFF_KEY 0xAA

/*
 * define devices in "configuration" space
 */
#define FDC   0
#define IDE1  1
#define IDE2  2
#define PARP  3
#define SER1  4
#define SER2  5
#define RTCL  6
#define KYBD  7
#define AUXIO 8

/*
 * chip level register offsets
 */
#define CONFIG_CONTROL        0x00
#define INDEX_ADDRESS         0x03
#define LOGICAL_DEVICE_NUMBER 0x07
#define DEVICE_ID             0x20
#define DEVICE_REV            0x21
#define POWER_CONTROL         0x22
#define POWER_MGMT            0x23
#define OSC                   0x24
#define ACTIVATE              0x30
#define ADDR_HI               0x60
#define ADDR_LOW              0x61
#define INTERRUPT_SEL         0x70

#define FDD_MODE_REGISTER     0x90
#define FDD_OPTION_REGISTER   0x91

#define DEVICE_ON             0x01
#define DEVICE_OFF            0x00
/*
 * values that we read back that we expect...
 */
#define VALID_DEVICE_ID       0x02

/*
 *  default device addresses
 */
#define COM1_BASE             0x3f8
#define COM1_INTERRUPT        4
#define COM2_BASE             0x2f8
#define COM2_INTERRUPT        3
#define PARP_BASE	      0x3BC
#define PARP_INTERRUPT	      7

#endif /*  __SMC_H_LOADED */
