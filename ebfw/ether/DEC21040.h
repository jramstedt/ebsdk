#ifndef __DECCHIP21040_H_LOADED
#define __DECCHIP21040_H_LOADED
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
 * $Id: DEC21040.h,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $
 */

/*
 * MODULE DESCRIPTION:
 *
 *    Parameters and logicals for 21x4x driver in EB64 monitor
 *
 * HISTORY:
 *
 * $Log: DEC21040.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.19  1997/07/23  20:07:37  pbell
 * Added additional bit definitions for the DC21143.
 *
 * Revision 1.18  1997/07/03  21:03:57  pbell
 * Cleaned up device names used by eshow and removed most of the
 * 21040 components of names and strings using 21x4x instead.
 *
 * Revision 1.17  1997/07/01  19:46:17  pbell
 * Added MII port support for DC2114[023] 10/100 Mbit.
 *
 * Revision 1.16  1997/05/23  15:14:17  pbell
 * Added recognition support for DC21140A, DC21142, DC21143 and
 * operational support for DC21140A on 10Base-T.
 *
 * Revision 1.15  1997/05/14  14:44:37  pbell
 * Removed debug register definitions TDES1_ADD, RDES1_ADD, and PCI_DIAG
 *
 * Revision 1.14  1996/06/03  15:02:06  fdh
 * Added Vendor and Device ID definitions to support the
 * 21041 and 21140 in addition to the 21040.
 *
 * Revision 1.13  1996/05/30  18:10:01  fdh
 * Added RCS Log directive.
 *
 */

#include "pci.h"

/*  The embedded Ethernet controller on the EB66 and EB64+ is a
 *  DECchip 21040 PCI device.
 *
 *  Please see the PCI and DECchip 21040 literature for definitive lists 
 *  and further detail.
 *  
 */

/*****************************************************************************
 * General                                                                   *
 *****************************************************************************/

/*
 *  PCI Vendors and devices.
 */
#define DIGITAL_VID		0x1011
#define DECCHIP_21040_DID	0x0002
#define DECCHIP_21041_DID	0x0014
#define DECCHIP_21140_DID	0x0009
#define DECCHIP_21142_DID	0x0019
#define DECCHIP_21143_DID	0x0019

#define DECCHIP_21040_TYPE	0
#define DECCHIP_21041_TYPE	1
#define DECCHIP_21140_TYPE	2
#define DECCHIP_21142_TYPE	3
#define DECCHIP_21143_TYPE	4

/* 
 *  Maximum number of DECchip 21x4xs supported in the EB66 and
 *  64+ monitors.  This is assuming 1 embedded device and 2 PCI
 *  slots.  Of course this changes if a bridge card is used.
 */

#define DECCHIP21x4x_MAX_DEVICES	3

/*****************************************************************************
 * ROM ID                                                                    *
 *****************************************************************************/
/*
 *  As the ROM ID is only used by the DECchip 21x4x, then this information
 *  can go here.  Note that this information is the same for both EB66
 *  and EB64P.
 *
 *  To read this device you must first write which page you want
 *  into the PAGE_REGISTER.  The bit settings look like:
 *   
 *  SxxA AAAA
 *
 *  Where S = 0 = BBRAM, S = 1 = Ethernet ID ROM.
 *
 *  A AAAA select the high order address lines of the BBRAM
 *  and are ignored for the Ethernet ID ROM.
 */
#define PAGE_REGISTER    0xc00
#define SELECT_ROMID     0x80
#define ROMID_BASE       0x800

/*****************************************************************************
 * CSRs, these exist in PCI I/O space                                        *
 *****************************************************************************/
/*
 *  CSR offsets.
 */

#define CSR0       0x0000			/* Bus Mode Register */
#define CSR1       0x0008			/* Transmit Poll Demand */
#define CSR2       0x0010			/* Receive Poll Demand */
#define CSR3       0x0018			/* Rx Ring Base Address */
#define CSR4       0x0020			/* Tx Ring Base Address */
#define CSR5       0x0028			/* Status Register */
#define CSR6       0x0030			/* Serial command register */
#define CSR7       0x0038			/* Interrupt Mask register */
#define CSR8       0x0040			/* Missed Frame counter */
#define CSR9       0x0048			/* ENET ROM register */
#define CSR10      0x0050			/* Data Diagnostic register */
#define CSR11      0x0058			/* Full duplex register */
#define CSR12      0x0060			/* SIA Status register */
#define CSR13      0x0068			/* SIA connectivity register */
#define CSR14      0x0070			/* SIA Tx Rx Register */
#define CSR15      0x0078			/* SIA General register */

/*
 *  CSR bit definitions (see the DECchip 21x4x manual for details).
 */
#define CSR0_CAL   0x0000C000		/* 15:14 Cache alignment */
#define CSR0_PBL   0x00003F00		/* 13:18 Programmable burst length */
#define CSR0_BLE   0x00000080		/* Big/Little endian */
#define CSR0_DSL   0x0000007C		/* Descriptor Skip Length */
#define CSR0_BAR   0x00000020		/* Bus Arbitration */
#define CSR0_SWR   0x00000001		/* Software Reset */

#define CSR1_TPD   0x00000001		/* Transmit Poll Demand */

#define CSR2_RPD   0x00000001		/* Receive Poll Demand */

#define CSR3_RXB   0xFFFFFFFC		/* Rx ring base address */

#define CSR4_TXB   0xFFFFFFFC		/* Tx ring base address */

#define CSR5_EB    0x03800000		/* 25:23 Error Bits */
#define CSR5_TS    0x00700000		/* 22:20 Transmission Process State */
#define CSR5_RS    0x000E0000		/* 19:17 Receive Process State */
#define CSR5_NIS   0x00010000		/* Normal interrupt Summary */
#define CSR5_AIS   0x00008000		/* Abnormal interrupt Summary */
#define CSR5_SE    0x00002000		/* System Error */
#define CSR5_LNF   0x00001000		/* link fail */
#define CSR5_FD    0x00000800		/* Full duplex short frame received */
#define CSR5_AT    0x00000400		/* AUI/TP switch */
#define CSR5_RWT   0x00000200		/* Receive watchdog time-out */
#define CSR5_RPS   0x00000100		/* Receive process stopped */
#define CSR5_RU    0x00000080		/* Receive buffer unavailable */
#define CSR5_RI    0x00000040		/* Receive Interrupt */
#define CSR5_UNF   0x00000020		/* Transmit underflow */
#define CSR5_TJT   0x00000008		/* Transmit jabber timeout */
#define CSR5_TU    0x00000004		/* Transmit buffer unavailable */
#define CSR5_TPS   0x00000002		/* Transmit process stopped */
#define CSR5_TI    0x00000001		/* Transmit interrupt */

#define CSR5_TS_SUSPENDED 0x00600000    /* 110: Transmit process suspended */
#define CSR5_RS_SUSPENDED 0x00080000    /* 100: Receive process suspended */

#define CSR6_MBO   0x02000000		/* Must be one */
#define CSR6_SCR   0x01000000		/* SCR Scrambler mode */
#define CSR6_PCS   0x00800000		/* PCS Function */
#define CSR6_TTM   0x00400000		/* Transmit Threshold Mode */
#define CSR6_SF    0x00200000		/* Store and Forward */
#define CSR6_HBD   0x00080000		/* Heart Beat Disable */
#define CSR6_PS    0x00040000		/* Port Select */
#define CSR6_TR    0x0000C000		/* 15:14 Threshold control bits */
#define CSR6_ST    0x00002000		/* Start/stop transmission */
#define CSR6_FC    0x00001000		/* Force collision mode */
#define CSR6_OM    0x00000C00		/* 11:10 Operating Mode */
#define CSR6_FD    0x00000200		/* Full duplex operating mode */
#define CSR6_FKD   0x00000100		/* Flaky oscillator disable */
#define CSR6_PM    0x00000080		/* Pass All Multicast */
#define CSR6_PR    0x00000040		/* Promiscuous mode */
#define CSR6_SB    0x00000020		/* Start/Stop Backoff counter */
#define CSR6_IF    0x00000010		/* Inverse filtering */
#define CSR6_PB    0x00000008		/* Pass Bad frames */
#define CSR6_HO    0x00000004		/* Hash-Only Filtering Mode */
#define CSR6_SR    0x00000002		/* Start/Stop Receive */
#define CSR6_HP    0x00000001		/* Hash/Perfect receive filtering mode
					 */

#define CSR7_NIM   0x00010000		/* Normal interrupt summary mask */
#define CSR7_AIM   0x00008000		/* Abnormal interrupt summary mask */
#define CSR7_SEM   0x00002000		/* System Error Mask */
#define CSR7_LFM   0x00001000		/* Link fail mask */
#define CSR7_FDM   0x00000800		/* Full Duplex mask */
#define CSR7_ATM   0x00000400		/* AUI/TP switch mask */
#define CSR7_RWM   0x00000200		/* Receive watchdog timeout mask */
#define CSR7_RSM   0x00000100		/* Receive stopped mask */
#define CSR7_RUM   0x00000080		/* Receive buffer unavailable mask */
#define CSR7_RIM   0x00000040		/* Receive interrupt mask */
#define CSR7_UNM   0x00000020		/* Underflow interrupt mask */
#define CSR7_TJM   0x00000008		/* transmit jabber timeout mask */
#define CSR7_TUM   0x00000004		/* transmit buffer unavailable mask */
#define CSR7_TSM   0x00000002		/* transmission stopped mask */
#define CSR7_TIM   0x00000001		/* transmit interrupt mask */

#define CSR8_MFO   0x00010000		/* Missed frame overflow */
#define CSR8_MFC   0x0000FFFF		/* Missed frame counter */

#define CSR9_DT    0x000000FF		/* 7:0 Data */
#define CSR9_DN    0x80000000		/* 31 Data not valid, 0 = valid */

#define CSR11_FD   0x0000FFFF		/* 15:0 Full duplex auto configuration 
					 * value */

#define CSR12_ANS  0x00007000		/* Autonegotiation Arbitration State */
#define CSR12_ANS_COM  0x00005000		/* Autonegotiation complete */
#define CSR12_TRA  0x00000200		/* 10BASE-T Receive port activity */
#define CSR12_ARA  0x00000100		/* AUI Receive port activity */
#define CSR12_PAUI 0x00000001
#define CSR12_APS  0x00000008		/* Autopolarity state */
#define CSR12_LKF  0x00000004		/* Link fail status */
#define CSR12_NCR  0x00000002
#define CSR12_LS10  0x0000004		/* LS10 link status */
#define CSR12_LS100 0x0000002		/* LS100 link status */
#define CSR12_MRA  0x00000001		/* MII receive port activity */

/*
 * CSR13 - SIA programming.
 */
#define CSR13_SRL  0x00000001           /* SIA Reset r/w */
#define CSR13_PS   0x00000002           /* Pin AUI/TP Selection r/w */
#define CSR13_CAC  0x00000004           /* CSR Auto Configuration */
#define CSR13_AUI  0x00000008           /* 10 Base T or AUI */
#define CSR13_EDP  0x00000010           /* SIA PLL external input enable */
#define CSR13_ENI  0x00000020           /* Encoder Input Mux */
#define CSR13_SIM  0x00000040           /* Serial Interface Input Mux */

/*
 * CSR14 - SIA transmit and receive register
 */
#define CSR14_T4   0x00020000           /* Advertize 100Base-T4   */
#define CSR14_TXF  0x00020000           /* Advertize 100Base-TxFD */
#define CSR14_TXH  0x00010000           /* Advertize 100Base-Tx   */
#define CSR14_TH   0x00000040           /* Advertize 10Base-Tx    */
#define CSR14_RSQ  0x00000100           /* Receive Squelch Enable */

/*
 * CSR15 - SIA general purpose port register
 */
#define CSR15_ABM  0x00000008           /* AUI/BNC mode */


/*****************************************************************************
 * Macros                                                                    *
 *****************************************************************************/
#define _21x4xSetCSRBits(device,csr,value) 	_21x4xWriteCSR(device,csr,_21x4xReadCSR(device,csr)|(value))
#define _21x4xClearCSRBits(device,csr,value) 	_21x4xWriteCSR(device,csr,_21x4xReadCSR(device,csr)&~(value))

#define _21x4xWriteCSR(device,csr,value) \
  (outportl((device)->PCI_IO_Base + (csr), (value)))

#define _21x4xReadCSR(device,csr) \
  (inportl((device)->PCI_IO_Base + (csr)))

#define _21x4xValidBuffer(buffer,size) \
  ( \
   (((unsigned long)(buffer) & 0x3) == 0) \
   && \
   ((unsigned long)(buffer) <= 0xFFFFFFFF) \
   && \
   (PCIValidAddress((unsigned char *) (buffer))) \
   && \
   (PCIValidAddress((unsigned char *) (buffer) + (size))) \
  )

/*****************************************************************************
 * PCI Constants                                                             *
 *****************************************************************************/
/*
 * The configuration registers for DECchip 21x4x exist in PCI configuration
 * space.  The definitions for the standard fields are in pci.h, these 
 * definitions are 21x4x specific.
 */
#define CFID     0x0000			/* Configuration ID */
#define CFCS     0x0004			/* Configuration command/status */
#define CFRV     0x0008			/* Configuration revision */
#define CFLT     0x000C			/* Configuration latency timer */
#define CBIO     0x0010			/* Configuration base IO address */
#define CBMA     0x0014			/* Configuration base memory address */
#define CFDD     0x0040			/* Configuration device driver area */

#define CBIO_MIO 0x00000001		/* Memory I/O. 1 = I/O. */
#define CSR_SIZE 0x400                  /* you can work this out by writing
					   all F's to CBIO */

/*****************************************************************************
 * Setup Frame, if needed                                                    *
 *****************************************************************************/
#define SETUP_FRAME_NEEDED   1                 /* Do we need a setup frame? */
#define SETUP_FRAME_SIZE   192
#define SETUP_PHYSICAL_ADDRESS_OFFSET 156
#define SETUP_HASH_FILTER_SIZE 128
/*****************************************************************************
 * Data Structures                                                           *
 *****************************************************************************/
/*
 * TX descriptor. Must be longword aligned in memory.
 */
typedef struct {
    union {
	unsigned int tdes0;
	struct {
	    unsigned int DE : 1;	/* deferred */
	    unsigned int UF : 1;	/* underflow error */
	    unsigned int LF : 1;	/* link fail */
	    unsigned int CC : 4;	/* 6:3 Collision count */
	    unsigned int HF : 1;	/* Heartbeat fail */
	    unsigned int EC : 1;	/* excessive collisions */
	    unsigned int LC : 1;	/* late collisions */
	    unsigned int NC : 1;	/* no carrier */
	    unsigned int LO : 1;        /* loss of carrier */
	    unsigned int unused_1 : 2;	/* 13:12 */
	    unsigned int TO : 1;	/* transmit jabber timeout */
	    unsigned int ES : 1;	/* error summary */
	    unsigned int unused_12 : 15;
					/* 30:16 */
	    unsigned int OWN : 1;	/* Own bit, 1 = 21x4x */
	} s_tdes0;
    } u_tdes0;
    union {
	unsigned int tdes1;
	struct {
	    unsigned int TBS1 : 11;	/* 10:0 transmit buffer size 1 */
	    unsigned int TBS2 : 11;	/* 21:11 transmit buffer size 2 */
	    unsigned int HP : 1;	/* Hash/Perfect filtering */
	    unsigned int DPD : 1;	/* disable padding */
	    unsigned int TCH : 1;	/* Second address chained */
	    unsigned int TER : 1;	/* transmit end of ring */
	    unsigned int AC : 1;	/* add CRC disable */
	    unsigned int SET : 1;	/* setup packet */
	    unsigned int IV : 1;	/* inverse filtering */
	    unsigned int FS : 1;	/* First segment */
	    unsigned int LS : 1;	/* last segment */
	    unsigned int IC : 1;	/* interrupt on completion */

	} s_tdes1;
    } u_tdes1;
    unsigned int tdes2;
    unsigned int tdes3;
} DECCHIP_21x4x_TX_BD;

/*
 * Receive descriptor.  Must be longword aligned in memory.
 */
typedef struct {
    union {
	unsigned int rdes0;
	struct {
	    unsigned int OF : 1;	/* overflow */
	    unsigned int CE : 1;	/* CRC error */
	    unsigned int DB : 1;	/* dribbling bit */
	    unsigned int unused_1 : 1;  /* 3 */
	    unsigned int RJ : 1;	/* Receive watchdog */
	    unsigned int FT : 1;	/* frame type */
	    unsigned int CS : 1;	/* collision seen */
	    unsigned int TL : 1;	/* frame too long */
	    unsigned int LS : 1;	/* last descriptor */
	    unsigned int FS : 1;	/* First descriptor */
	    unsigned int MF : 1;	/* Multicast frame */
	    unsigned int RF : 1;	/* runt frame */
	    unsigned int DT : 2;	/* 13:12 Data type */
	    unsigned int LE : 1;	/* length error */
	    unsigned int ES : 1;	/* error summary */
	    unsigned int FL : 15;	/* Frame Length */
	    unsigned int OWN : 1;	/* Own bit 1 = 21x4x */
	} s_rdes0;
    } u_rdes0;
    union {
	unsigned int rdes1;
	struct {
	    unsigned int RBS1 : 11;	/* 10:0 Buffer Size 1 */
	    unsigned int RBS2 : 11;	/* 21:11  Buffer Size 2 */
	    unsigned int unused_1 : 2;	/* 23:22 */
	    unsigned int RCH : 1;	/* Second address chained */
	    unsigned int RER : 1;	/* Receive end of ring */
	    unsigned int unused_2 : 6;	/* 31:26 */
	} s_rdes1;
    } u_rdes1;
    unsigned int rdes2;
    unsigned int rdes3;
} DECCHIP_21x4x_RX_BD;

/*
 *  Each device has a block of counters associated with it.
 */
typedef struct counters {
/* 
 * Transmit counters.
 */
  unsigned int tx_restarted;
  unsigned int p_tx;
  unsigned int b_tx;
  unsigned int tx_err_UF;
  unsigned int tx_err_EC;
  unsigned int tx_err_LC;
  unsigned int tx_err_NC;
  unsigned int tx_err_LO;
  unsigned int tx_err_TO;
  unsigned int tx_err_LF;
/*
 * Receive couters.
 */
  unsigned int rx_restarted;
  unsigned int p_rx;
  unsigned int mf_rx;
  unsigned int b_rx;
  unsigned int rx_err_OF;
  unsigned int rx_err_CE;
  unsigned int rx_err_CS;
  unsigned int rx_err_TL;
  unsigned int rx_err_LE;
  unsigned int rx_err_RF;
} counters_t;

/*
 *  Describe what a particular DECchip 21x4x looks like.
 */
typedef struct DECchip_21x4x_device {
    unsigned int device_type;		/* index into DC21X4X_DID_Data structure */
    unsigned int device;		/* DBM device number */
    unsigned int index;                 /* Index into static structures */
    PCIDevice_t *pci;                   /* The PCI device */
    unsigned int PCI_IO_Base;		/* Address of CSRs in PCI I/O space */
    unsigned int rx_index;              /* Next RX buffer */
    unsigned int tx_index;              /* Next TX buffer */
    struct counters *counters;          /* block of counter information */
    mac_addr hw_address;                /* This device's h/w address */
    char *setup_frame;                  /* setup frame */
    struct {
      unsigned int exists : 1;          /* Does this device exist? */
      unsigned int allocated : 1;	/* Is this device in use? */
      unsigned int initialised : 1;     /* Has the device been initialised? */
      unsigned int loopback : 1;        /* loopback mode selected */
      unsigned int setup_frame : 1;     /* setup frame needed? */
      unsigned int hw_valid : 1;	/* Is the hardware address valid? */
    } flags;
} DECchip_21x4x_device_t;

#endif /* __DECCHIP21040_H_LOADED */
