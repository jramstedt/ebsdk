/********************************************************************************************************************************/
/* Created: 27-SEP-1999 16:07:53 by OpenVMS SDL EV1-33     */
/* Source:  27-SEP-1999 16:04:53 DKA0:[ROWE.AFW.PC264.SRC]PLATFORM_CPU.SDL;1 */
/********************************************************************************************************************************/
/* file:	tinosa.sdl                                                      */
/*                                                                          */
/* Copyright (C) 1993, 1996 by                                              */
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
/* and  should  not  be  construed  as a commitment by digital equipment    */
/* corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by digital.                  */
/*                                                                          */
/*                                                                          */
/* Abstract:   	Tinosa specific definitions for Alpha firmware.				*/
/*                                                                          */
/* Author:	Eric Goehl                                                      */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	er	15-Jul-1997	Fix for EV6 based platform.								*/
/*	egg	20-Sep-1996	Initial port taken from eb164.sdl module.				*/
/*                                                                          */
/*** MODULE $machdep ***/

/*                                                                          */
/* EB164 specific parameters of the HWRPB                                   */
/*                                                                          */
#define HWRPB$_PAGESIZE 8192            /* System page size in bytes        */
#define HWRPB$_PAGEBITS 13              /* System page size in bits         */
#define HWRPB$_PA_SIZE 44               /* Physical address size in bits    */
#define HWRPB$_VA_SIZE 48               /* Virtual address size in bits     */
#define HWRPB$_ASN_MAX 255              /* Maximum ASN value                */
#define HWRPB$_SYSTYPE SYSTEM_TYPE      /* Alpha system type                */
#define HWRPB$_SYSVAR 1024              /* System variation - Embedded Console, MP capable */
#define HWRPB$_NPROC MAX_PROCESSOR_ID   /* Number of processor supported    */
#define HWRPB$_ICLK_FREQ 4194304        /* Interval timer frequency         */
#define HWRPB$_CC_FREQ 100000000        /* Cycle counter frequency (@10ns)  */
#define SLOT$_CPU_TYPE 8                /* Processor type (EV6)             */
#define SLOT$_CPU_VAR 7                 /* Processor Variation (VAX/IEEE FP) */
#define SLOT$_PAL_LOGOUT 24576          /* PAL logout area                  */
#define SLOT$_PAL_LOGOUT_LEN 1024       /* PAL logout area length           */
#define CTB$MAX_UNITS 1                 /* 1 CTB for Workstations           */
#define CTB$CSR0 0                      /* Base CSR address for LOC port    */
#define CTB$TX_IPL0 21                  /* Transmit interrupt IPL for LOC port */
#define CTB$TX_SCB0 2112                /* Transmit vector for LOC port     */
#define CTB$RX_IPL0 21                  /* Receive interrupt IPL for LOC port */
#define CTB$RX_SCB0 2112                /* Receive vector for LOC port      */
#define CTB$CSR1 0                      /* Base CSR address for RD port     */
#define CTB$TX_IPL1 21                  /* Transmit interrupt for RD port   */
#define CTB$TX_SCB1 2096                /* Transmit vector for RD port      */
#define CTB$RX_IPL1 21                  /* Receive interrupt IPL for RD port */
#define CTB$RX_SCB1 2096                /* Receive vector for RD port       */
#define KEYBD_VECTOR (1+ISA_BASE_VECTOR)/* keyboard vector irq1             */
#define COM2_VECTOR (3+ISA_BASE_VECTOR)	/* serial port COM2 vector irq3     */
#define COM1_VECTOR (4+ISA_BASE_VECTOR)	/* serial port COM1 vector irq4     */
#define FLOPPY_VECTOR (6+ISA_BASE_VECTOR)/* floppy vector irq6              */
#define PARALLEL_VECTOR (7+ISA_BASE_VECTOR)/* Parallel port irq 7			*/
#define SMB_VECTOR (9+ISA_BASE_VECTOR)	/* System management bus irq 9      */
#define MOUSE_VECTOR (12+ISA_BASE_VECTOR)/* mouse vector	irq12           */
#define IDE0_VECTOR (14+ISA_BASE_VECTOR)/* IDE0 Vector irq14                */
#define IDE1_VECTOR (15+ISA_BASE_VECTOR)/* IDE1 Vector irq15                */
#define ISA_BASE_VECTOR 128				/* SCB vector base for ISA devices  */
#define PCI_BASE_VECTOR 144             /* SCB vector base for PCI devices  */
#define PC264$DISPLAY_CPU 1             /* Display CPU registers            */
#define PC264$DISPLAY_SYS 2             /* Display System registers         */
#define PC264$DISPLAY_PC 4              /* Display Process info             */
#define OEM_VERSION_SUB					/* API Version Info					*/
