/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:31:20 by OpenVMS SDL EV1-33     */
/* Source:  21-JUN-2000 10:18:29 AFW_USER$:[CONSOLE.V58.LX164.SRC]PLATFORM_CPU.S */
/********************************************************************************************************************************/
/* file:	lx164.sdl                                                   */
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
/* and  should  not  be  construed  as a commitment by digital equipment    */
/* corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by digital.                  */
/*                                                                          */
/*                                                                          */
/* Abstract:	AlphaPC 164/LX specific definitions for SRM Console firmware. */
/*                                                                          */
/* Author:	Eric Rasmussen                                              */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	er	20-Nov-1996	Created                                     */
/*                                                                          */
/*** MODULE $machdep ***/
#ifndef	LX164_DEF
#define LX164_DEF 1
#ifdef IPL_RUN
#undef IPL_RUN
#endif
#define IPL_RUN 21                      /* Run console drivers in polled mode */
/*                                                                          */
/* AlphaPC 164/LX specific parameters of the HWRPB                          */
/*                                                                          */
#define HWRPB$_PAGESIZE 8192            /* System page size in bytes        */
#define HWRPB$_PAGEBITS 13              /* System page size in bits         */
#define HWRPB$_PA_SIZE 40               /* Physical address size in bits    */
#define HWRPB$_ASN_MAX 127              /* Maximum ASN value                */
#define HWRPB$_SYSTYPE 26               /* Alpha system type (EB164/PC164)  */
#define HWRPB$_SYSVAR 4                 /* System variation - Embedded Console */
#define HWRPB$_NPROC 1                  /* Number of processor supported    */
#define HWRPB$_ICLK_FREQ 4194304        /* Interval timer frequency         */
#define HWRPB$_CC_FREQ 100000000        /* Cycle counter frequency (@10ns)  */
#define HWRPB$_CC_FREQ_50 200000000     /* Cycle counter frequency (@5ns)   */
#define HWRPB$_CC_FREQ_60 166666667     /* Cycle counter frequency (@6ns)   */
#define HWRPB$_CC_FREQ_66 151515152     /* Cycle counter frequency (@6.6ns) */
#define HWRPB$_CC_FREQ_70 142857143     /* Cycle counter frequency (@7ns)   */
#define SLOT$_CPU_TYPE 7                /* Processor type (EV56)            */
#define SLOT$_CPU_VAR 3                 /* Processor Variation (VAX/IEEE FP) */
#define SLOT$_PAL_LOGOUT 24576          /* PAL logout area                  */
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
#define COM1_VECTOR 132                 /* serial port COM1 vector irq4     */
#define COM2_VECTOR 131                 /* serial port COM2 vector irq3     */
#define KEYBD_VECTOR 129                /* keyboard vector         irq1     */
#define MOUSE_VECTOR 140                /* mouse    vector	   irq12    */
#define ISA_BASE_VECTOR 128             /* SCB vector base for ISA devices  */
#define PCI_BASE_VECTOR 144             /* SCB vector base for PCI devices  */
#define LX164$PRIMARY_CPU 0
#define LX164$DISPLAY_CPU 1             /* Display CPU registers            */
#define LX164$DISPLAY_PYXIS 2           /* Display Pyxis registers          */
#define LX164$DISPLAY_PC 4              /* Display Process info             */
#define LX164$MEM_TOTAL 2               /* Total memory banks for AlphaPC 164/LX */
struct MCHK$LOGOUT {
    unsigned int ei_stat [2];
    unsigned int ei_addr [2];
    unsigned int pyxis_err [2];
    unsigned int pyxis_stat [2];
    unsigned int pyxis_err_mask [2];
    unsigned int ecc_syn [2];
    unsigned int mear [2];
    unsigned int mesr [2];
    unsigned int pyxis_err_data [2];
    unsigned int pci_err0 [2];
    unsigned int pci_err1 [2];
    unsigned int pci_err2 [2];
    unsigned int sio_nmisc [2];
    } ;
struct PAL$LOGOUT {
    unsigned int r_byte_count [2];
    unsigned int sys_proc$$offset [2];
    unsigned int mchk_code [2];
    unsigned int shadow8 [2];
    unsigned int shadow9 [2];
    unsigned int shadow10 [2];
    unsigned int shadow11 [2];
    unsigned int shadow12 [2];
    unsigned int shadow13 [2];
    unsigned int shadow14 [2];
    unsigned int shadow25 [2];
    unsigned int pt0 [2];
    unsigned int pt1 [2];
    unsigned int pt2 [2];
    unsigned int pt3 [2];
    unsigned int pt4 [2];
    unsigned int pt5 [2];
    unsigned int pt6 [2];
    unsigned int pt7 [2];
    unsigned int pt8 [2];
    unsigned int pt9 [2];
    unsigned int pt10 [2];
    unsigned int pt11 [2];
    unsigned int pt12 [2];
    unsigned int pt13 [2];
    unsigned int pt14 [2];
    unsigned int pt15 [2];
    unsigned int pt16 [2];
    unsigned int pt17 [2];
    unsigned int pt18 [2];
    unsigned int pt19 [2];
    unsigned int pt20 [2];
    unsigned int pt21 [2];
    unsigned int pt22 [2];
    unsigned int pt23 [2];
    unsigned int exc_addr [2];
    unsigned int exc_sum [2];
    unsigned int exc_msk [2];
    unsigned int pal_base [2];
    unsigned int isr [2];
    unsigned int icsr [2];
    unsigned int ic_perr_stat [2];
    unsigned int dc_perr_stat [2];
    unsigned int va [2];
    unsigned int mm_stat [2];
    unsigned int sc_addr [2];
    unsigned int sc_stat [2];
    unsigned int bc_tag_addr [2];
    unsigned int ei_addr [2];
    unsigned int fill_syn [2];
    unsigned int ei_stat [2];
    unsigned int ld_lock [2];
/* Pyxis specific error registers                                           */
    unsigned int sys_reserved [2];
    unsigned int pyxis_err_data [2];
    unsigned int pyxis_err [2];
    unsigned int pyxis_stat [2];
    unsigned int pyxis_err_mask [2];
    unsigned int ecc_syn [2];
    unsigned int mear [2];
    unsigned int mesr [2];
    unsigned int pci_err0 [2];
    unsigned int pci_err1 [2];
    unsigned int sio_nmisc [2];
    unsigned int pci_err2 [2];
    unsigned int pyxis_hae_mem [2];
/* SIO specific registers                                                   */
    unsigned int sio_rev [2];
    unsigned int sio_ubcsa [2];
    unsigned int sio_ubcsb [2];
    unsigned int sio_imr0 [2];
    unsigned int sio_imr1 [2];
    unsigned int sio_irr0 [2];
    unsigned int sio_irr1 [2];
    unsigned int sio_isr0 [2];
    unsigned int sio_isr1 [2];
    unsigned int sio_dma_stat0 [2];
    unsigned int sio_dma_stat1 [2];
    unsigned int sio_status [2];
    } ;
struct PAL$LOGOUT_620 {
    unsigned int r_byte_count [2];
    unsigned int sys_proc$$offset [2];
    unsigned int mchk_code [2];
    unsigned int ei_addr [2];
    unsigned int fill_syn [2];
    unsigned int ei_stat [2];
    unsigned int isr [2];
    unsigned int ecc_syn [2];
    unsigned int mear [2];
    unsigned int mesr [2];
    unsigned int pyxis_stat [2];
    unsigned int pyxis_err [2];
    unsigned int pyxis_err_data [2];
    } ;
struct PAL$LOGOUT_630 {
    unsigned int r_byte_count [2];
    unsigned int sys_proc$$offset [2];
    unsigned int mchk_code [2];
    unsigned int ei_addr [2];
    unsigned int fill_syn [2];
    unsigned int ei_stat [2];
    unsigned int isr [2];
    unsigned int ecc_syn [2];
    unsigned int mear [2];
    unsigned int mesr [2];
    unsigned int pyxis_stat [2];
    unsigned int pyxis_err [2];
    unsigned int pyxis_err_data [2];
    } ;
#endif
 
