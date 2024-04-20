/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:05:14 by OpenVMS SDL EV1-31     */
/* Source:   8-MAR-1994 08:15:47 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]COMBO_DEF.SD */
/********************************************************************************************************************************/
/*                                                                          */
/* Copyright (C) 1992 by                                                    */
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
/* Abstract:	VTI 82C106 Combo Chip definitions                           */
/*                                                                          */
/* Author:	Judith E. Gold                                              */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/* 	jeg	15-Oct-1992	add printer port definitions.               */
/*		                                                            */
/*                                                                          */
/*** MODULE $combodef ***/
/* base addresses of various devices within combo chip                      */
#define COM1 1016
#define COM2 760
#define LPTD 956
#define LPTS 957
#define LPTC 958
#define KBD_MS 96
#define TOY_RAM 49520
/* COMM register offsets from base                                          */
#define RBR 0
#define THR 0
#define DLL 0
#define IER 1
#define DLM 1
#define IIR 2
#define FCR 2
#define LCR 3
#define MCR 4
#define LSR 5
#define MSR 6
#define SCR 7
/* FCR - FIFO Control Register (16550s only, unimplemented on 16450s)       */
#define FCR$M_FE 0x1
#define FCR$M_RFR 0x2
#define FCR$M_TFR 0x4
#define FCR$M_DMS 0x8
#define FCR$M_R 0x30
#define FCR$M_RTL 0x40
#define FCR$M_RTM 0x80
struct FCRR {
    unsigned FCR$V_FE : 1;              /* FIFO enable                      */
    unsigned FCR$V_RFR : 1;             /* receive FIFO reset               */
    unsigned FCR$V_TFR : 1;             /* transmit FIFO reset              */
    unsigned FCR$V_DMS : 1;             /* DMA mode select                  */
    unsigned FCR$V_R : 2;               /* reserved                         */
    unsigned FCR$V_RTL : 1;             /* receive trigger (LSB)            */
    unsigned FCR$V_RTM : 1;             /* receive trigger (MSB)            */
    } ;
/* LCR - Line Control Register                                              */
#define LCR$M_WLS 0x3
#define LCR$M_SBS 0x4
#define LCR$M_PE 0x8
#define LCR$M_EPS 0x10
#define LCR$M_SP 0x20
#define LCR$M_BC 0x40
#define LCR$M_DLA 0x80
struct LCRR {
    unsigned LCR$V_WLS : 2;             /* word length select               */
    unsigned LCR$V_SBS : 1;             /* stop bit select                  */
    unsigned LCR$V_PE : 1;              /* parity enable                    */
    unsigned LCR$V_EPS : 1;             /* even parity select               */
    unsigned LCR$V_SP : 1;              /* stick parity                     */
    unsigned LCR$V_BC : 1;              /* break control                    */
    unsigned LCR$V_DLA : 1;             /* divisor latch access bit         */
    } ;
#define CHAR_5BIT 0
#define CHAR_6BIT 1
#define CHAR_7BIT 2
#define CHAR_8BIT 3
/* LSR - Line Status Register                                               */
#define LSR$M_DR 0x1
#define LSR$M_OE 0x2
#define LSR$M_PE 0x4
#define LSR$M_FE 0x8
#define LSR$M_BI 0x10
#define LSR$M_THRE 0x20
#define LSR$M_TEMT 0x40
#define LSR$M_UNUSED 0x80
struct LSRR {
    unsigned LSR$V_DR : 1;              /* data ready                       */
    unsigned LSR$V_OE : 1;              /* overrun error                    */
    unsigned LSR$V_PE : 1;              /* parity error                     */
    unsigned LSR$V_FE : 1;              /* framing error                    */
    unsigned LSR$V_BI : 1;              /* break interrupt                  */
    unsigned LSR$V_THRE : 1;            /* tx holding register empty        */
    unsigned LSR$V_TEMT : 1;            /* transmitter empty                */
    unsigned LSR$V_UNUSED : 1;          /* not used                         */
    } ;
/* MSR - Modem Status Register                                              */
#define MSR$M_DCTS 0x1
#define MSR$M_DDSR 0x2
#define MSR$M_TERI 0x4
#define MSR$M_DDCD 0x8
#define MSR$M_CTS 0x10
#define MSR$M_DSR 0x20
#define MSR$M_RI 0x40
#define MSR$M_DCD 0x80
struct MSRR {
    unsigned MSR$V_DCTS : 1;            /* delta clear to send              */
    unsigned MSR$V_DDSR : 1;            /* delta data set ready             */
    unsigned MSR$V_TERI : 1;            /* trailing edge of ring indicator  */
    unsigned MSR$V_DDCD : 1;            /* delta data carrier detect        */
    unsigned MSR$V_CTS : 1;             /* clear to send                    */
    unsigned MSR$V_DSR : 1;             /* data set ready                   */
    unsigned MSR$V_RI : 1;              /* ring indicator                   */
    unsigned MSR$V_DCD : 1;             /* data carrier detect              */
    } ;
/* MCR - Modem Control Register                                             */
#define MCR$M_DTR 0x1
#define MCR$M_RTS 0x2
#define MCR$M_OUT1 0x4
#define MCR$M_OUT2 0x8
#define MCR$M_LOOP 0x10
#define MCR$M_UNUSED 0xE0
struct MCRR {
    unsigned MCR$V_DTR : 1;             /* data terminal ready              */
    unsigned MCR$V_RTS : 1;             /* request to send                  */
    unsigned MCR$V_OUT1 : 1;            /*                                  */
    unsigned MCR$V_OUT2 : 1;            /*                                  */
    unsigned MCR$V_LOOP : 1;            /*                                  */
    unsigned MCR$V_UNUSED : 3;          /* unused                           */
    } ;
/* IIR - Interrupt Identification Register                                  */
#define IIR$M_IP 0x1
#define IIR$M_HPIP 0x6
#define IIR$M_UNUSED 0xF8
struct IIRR {
    unsigned IIR$V_IP : 1;              /* interrupt pending                */
    unsigned IIR$V_HPIP : 2;            /* highest level interrupt pending  */
    unsigned IIR$V_UNUSED : 5;          /* unused                           */
    } ;
/* IER - Interrupt Enable Register                                          */
#define IER$M_RDAIE 0x1
#define IER$M_THREIE 0x2
#define IER$M_RLSIE 0x4
#define IER$M_MSIE 0x8
#define IER$M_UNUSED 0xF0
struct IERR {
    unsigned IER$V_RDAIE : 1;           /* received data available int. enable */
    unsigned IER$V_THREIE : 1;          /* tx holding register empty int. enable */
    unsigned IER$V_RLSIE : 1;           /* receive line status int. enable  */
    unsigned IER$V_MSIE : 1;            /* modem status interrupt enable    */
    unsigned IER$V_UNUSED : 4;          /* unused                           */
    } ;
/* LPTS - Line Printer Port Status                                          */
#define LPTS$M_UNUSED 0x3
#define LPTS$M_IRQ 0x4
#define LPTS$M_ERROR 0x8
#define LPTS$M_SLCT 0x10
#define LPTS$M_PE 0x20
#define LPTS$M_ACK 0x40
#define LPTS$M_BUSY 0x80
struct LPTSR {
    unsigned LPTS$V_UNUSED : 2;         /*                                  */
    unsigned LPTS$V_IRQ : 1;            /* interrupt                        */
    unsigned LPTS$V_ERROR : 1;          /* error                            */
    unsigned LPTS$V_SLCT : 1;           /* select                           */
    unsigned LPTS$V_PE : 1;             /* paper empty                      */
    unsigned LPTS$V_ACK : 1;            /* acknowledge                      */
    unsigned LPTS$V_BUSY : 1;           /* busy                             */
    } ;
/* LPTC - Line Printer Port Control                                         */
#define LPTC$M_STROBE 0x1
#define LPTC$M_AUTO_FD 0x2
#define LPTC$M_INIT 0x4
#define LPTC$M_SLCT_IN 0x8
#define LPTC$M_IRQ_EN 0x10
#define LPTC$M_DIR 0x20
#define LPTC$M_UNUSED 0x40
struct LPTCR {
    unsigned LPTC$V_STROBE : 1;         /*                                  */
    unsigned LPTC$V_AUTO_FD : 1;        /* auto feed                        */
    unsigned LPTC$V_INIT : 1;           /* NOT initialize printer           */
    unsigned LPTC$V_SLCT_IN : 1;        /* select input                     */
    unsigned LPTC$V_IRQ_EN : 1;         /* interrupt request enable         */
    unsigned LPTC$V_DIR : 1;            /* direction                        */
    unsigned LPTC$V_UNUSED : 1;         /*                                  */
    unsigned LPTC$V_FILL_0 : 1;
    } ;
/* baud rates for DLL and DLM                                               */
#define BAUD_50 2304
#define BAUD_75 1536
#define BAUD_110 1047
#define BAUD_1345 857
#define BAUD_150 768
#define BAUD_300 384
#define BAUD_600 192
#define BAUD_1200 96
#define BAUD_1800 64
#define BAUD_2000 58
#define BAUD_2400 48
#define BAUD_3600 32
#define BAUD_4800 24
#define BAUD_7200 16
#define BAUD_9600 12
#define BAUD_19200 6
#define BAUD_38400 3
#define BAUD_56000 2
#define BAUD_115200 1
 
