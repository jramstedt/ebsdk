/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:37:08 by OpenVMS SDL EV1-33     */
/* Source:   2-APR-1996 10:45:46 AFW_USER$:[CONSOLE.V58.COMMON.SRC]SMC_DEF.SDL;1 */
/********************************************************************************************************************************/
/*                                                                          */
/* Copyright (C) 1996 by                                                    */
/* Digital Equipment Corporation, Maynard, Massachusetts.                   */
/* All rights reserved.                                                     */
/*                                                                          */
/* This software is furnished under a license and may be used and copied    */
/* only  in  accordance  of  the  terms  of  such  license  and with the    */
/* inclusion of the above copyright notice. This software or  any  other    */
/* copies thereof may not be provided or otherwise made available to any    */
/* other person.  No title to and  ownership of the  software is  hereby    */
/* transferred.                                                             */
/*                                                                          */
/* The information in this software is  subject to change without notice    */
/* and  should  not  be  construed  as a commitment by Digital Equipment    */
/* Corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by Digital.                  */
/*                                                                          */
/*                                                                          */
/* Abstract:	SMC FDC37C93X Ultra I/O Controller Definitions              */
/*                                                                          */
/* Author:	Eric A. Rasmussen                                           */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	er	02-Apr-1996	Added definitions for KYBD, PARP, and DMA_CHANNEL_SEL. */
/*                                                                          */
/*                                                                          */
/*** MODULE $smcdef ***/
#define DEVICE_ON 1
#define DEVICE_OFF 0
/* configuration on/off keys                                                */
#define CONFIG_ON_KEY 85
#define CONFIG_OFF_KEY 170
/* configuration space device definitions                                   */
#define FDC 0
#define IDE1 1
#define IDE2 2
#define PARP 3
#define SER1 4
#define SER2 5
#define RTCL 6
#define KYBD 7
#define AUXIO 8
/* Chip register offsets from base                                          */
#define CONFIG_CONTROL 0
#define INDEX_ADDRESS 3
#define LOGICAL_DEVICE_NUMBER 7
#define DEVICE_ID 32
#define DEVICE_REV 33
#define POWER_CONTROL 34
#define POWER_MGMT 35
#define OSC 36
#define ACTIVATE 48
#define ADDR_HI 96
#define ADDR_LO 97
#define INTERRUPT_SEL 112
#define DMA_CHANNEL_SEL 116
#define FDD_MODE_REGISTER 144
#define FDD_OPTION_REGISTER 145
/* values that we read back that are expected ...                           */
#define VALID_DEVICE_ID 2
/* default device addresses                                                 */
#define KYBD_INTERRUPT 1
#define COM2_BASE 760
#define COM2_INTERRUPT 3
#define COM1_BASE 1016
#define COM1_INTERRUPT 4
#define PARP_BASE 956
#define PARP_INTERRUPT 7
 
