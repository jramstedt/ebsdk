#ifndef __TSUNAMI_H_LOADED
#define __TSUNAMI_H_LOADED
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
 *  $Id: tsunami.h,v 1.2 1999/01/21 19:06:07 gries Exp $;
 */

/*
 * $Log: tsunami.h,v $
 * Revision 1.2  1999/01/21 19:06:07  gries
 * First Release to cvs gries
 *
 * Revision 1.4  1998/09/03  19:20:29  gries
 * changes for Arbiter bit is 801.3800.01C0
 *
 * Revision 1.3  1998/07/30  21:43:45  gries
 * Changes for soft rest
 *
 * Revision 1.2  1998/06/03  16:38:02  gries
 * srmhack defines
 *
 * Revision 1.1  1997/08/13  15:25:57  fdh
 * Initial revision
 *
 */

#ifndef TSUNAMI_SYSTEM
#define TSUNAMI_SYSTEM
#endif /* TSUNAMI_SYSTEM */

#define PCI0_MEMORY	0x80000
#define TIG_BUS		0x80100
#define SOFT_RESET_BASE 0x80138
#define SOFT_RESET_check 0x0040
#define SOFT_RESET_invoke  0x0100
#define SYNCHRONIZE_CPU 0x80130
#define SYNCHRONIZE_CPU0 0x0A00
#define SYNCHRONIZE_CPU1 0x0A40
#define SYNCHRONIZE_CPU0_TRANSFER_ADDRESS_PHYS  0x100
#define SYNCHRONIZE_CPU1_TRANSFER_ADDRESS_PHYS  0x108
#define Arbiter_bit				0x1c0

/*-----------------------------*/
#define PCHIP0_CSR	0x80180
/*-----------------------------*/
#define	PCHIP0_WSBA0  	0x000
#define	PCHIP0_WSBA1  	0x040
#define	PCHIP0_WSBA2  	0x080
#define	PCHIP0_WSBA3  	0x0C0

#define	PCHIP0_WSM0  	0x100
#define	PCHIP0_WSM1  	0x140
#define	PCHIP0_WSM2  	0x180
#define	PCHIP0_WSM3  	0x1C0
#define	PCHIP0_TBA0  	0x200
#define	PCHIP0_TBA1  	0x240
#define	PCHIP0_TBA2  	0x280
#define	PCHIP0_TBA3  	0x2C0

#define	PCHIP0_PCTL  	0x300
#define	PCHIP0_PLAT  	0x340
#define	PCHIP0_RESERVED	0x380
#define	PCHIP0_PERROR	0x3c0
#define	PCHIP0_PERRMASK	0x400
#define	PCHIP0_PERRSET 	0x440
#define	PCHIP0_TLBIV  	0x480
#define	PCHIP0_TLBIA 	0x4C0
#define	PCHIP0_PMONCTL	0x500
#define	PCHIP0_PMONCNT	0x540
/*-----------------------------*/


/*-----------------------------*/
#define CCHIP_CSR	0x801A0
/*-----------------------------*/
#define	CSR_CSC		0x000
#define	CSR_MTR		0x040
#define	CSR_MISC	0x080
#define	CSR_MPD		0x0C0
#define	CSR_AAR0	0x100
#define	CSR_AAR1	0x140
#define	CSR_AAR2	0x180
#define	CSR_AAR3	0x1C0
#define	CSR_DIM0	0x200
#define	CSR_DIM1	0x240
#define	CSR_DIR0	0x280
#define	CSR_DIR1	0x2C0

#define	CSR_DRIR	0x300
#define	CSR_PRBEN	0x340
#define	CSR_IIC	       	0x380
#define	CSR_WDR	       	0x3C0
#define	CSR_MPR0	0x400
#define	CSR_MPR1	0x440
#define	CSR_MPR2	0x480
#define	CSR_MPR3	0x4C0
#define	CSR_TTR		0x580
#define	CSR_TDR		0x5C0
/*-----------------------------*/


/*-----------------------------*/
#define DCHIP_CSR	0x801B0
/*-----------------------------*/
#define	CSR_DSC	       	0x800
#define	CSR_STR		0x840
#define	CSR_DREV	0x880
/*-----------------------------*/


#define PCI0_IACK	0x801F8
#define PCI0_IO		0x801FC
#define PCI0_CONFIG	0x801FE
#define PCI1_MEMORY	0x80200


/*-----------------------------*/
#define PCHIP1_CSR	0x80380
/*-----------------------------*/
#define	PCHIP1_WSBA0  	0x000
#define	PCHIP1_WSBA1  	0x040
#define	PCHIP1_WSBA2  	0x080
#define	PCHIP1_WSBA3  	0x0C0
#define	PCHIP1_WSM0  	0x100
#define	PCHIP1_WSM1  	0x140
#define	PCHIP1_WSM2  	0x180
#define	PCHIP1_WSM3  	0x1C0

#define	PCHIP1_TBA0  	0x200
#define	PCHIP1_TBA1  	0x240
#define	PCHIP1_TBA2  	0x280
#define	PCHIP1_TBA3  	0x2C0

#define	PCHIP1_PCTL  	0x300
#define	PCHIP1_PLAT  	0x340
#define	PCHIP1_RESERVED	0x380
#define	PCHIP1_PERROR	0x3c0
#define	PCHIP1_PERRMASK	0x400
#define	PCHIP1_PERRSET	0x440
#define	PCHIP1_TLBIV  	0x480
#define	PCHIP1_TLBIA	0x4C0
#define	PCHIP1_PMONCTL	0x500
#define	PCHIP1_PMONCNT	0x540
/*-----------------------------*/


#define PCI1_IACK	0x803F8
#define PCI1_IO		0x803FC
#define PCI1_CONFIG	0x803FE

/*                                                                          */
/* TSUNAMI Pchip Error register.                                            */
/*                                                                          */
#define perror_m_lost 0x1
#define perror_m_serr 0x2
#define perror_m_perr 0x4
#define perror_m_dcrto 0x8
#define perror_m_sge 0x10
#define perror_m_ape 0x20
#define perror_m_ta 0x40
#define perror_m_rdpe 0x80
#define perror_m_nds 0x100
#define perror_m_rto 0x200
#define perror_m_uecc 0x400
#define perror_m_cre 0x800
#define perror_m_addrl 0xFFFFFFFF0000
#define perror_m_addrh 0x7000000000000
#define perror_m_cmd 0xF0000000000000
#define perror_m_syn 0xFF00000000000000

#ifdef __LANGUAGE_C__
union TPchipPERROR {
    struct  {
        unsigned int perror_v_lost : 1;
        unsigned perror_v_serr : 1;
        unsigned perror_v_perr : 1;
        unsigned perror_v_dcrto : 1;
        unsigned perror_v_sge : 1;
        unsigned perror_v_ape : 1;
        unsigned perror_v_ta : 1;
        unsigned perror_v_rdpe : 1;
        unsigned perror_v_nds : 1;
        unsigned perror_v_rto : 1;
        unsigned perror_v_uecc : 1;
        unsigned perror_v_cre : 1;
        unsigned perror_v_rsvd1 : 4;
        unsigned perror_v_addrl : 32;
        unsigned perror_v_addrh : 3;
        unsigned perror_v_rsvd2 : 1;
        unsigned perror_v_cmd : 4;
        unsigned perror_v_syn : 8;
        } perror_r_bits;
    int perror_q_whole [2];
    } ;
#endif /* __LANGUAGE_C__ */

/*                                                                          */
/* TSUNAMI Pchip Window Space Base Address register.                        */
/*                                                                          */
#define wsba_m_ena 0x1
#define wsba_m_sg 0x2
#define wsba_m_ptp 0x4
#define wsba_m_addr 0xFFF00000
#define wmask_k_sz1gb 0x3FF00000

#ifdef __LANGUAGE_C__
union TPchipWSBA {
    struct  {
        unsigned wsba_v_ena : 1;
        unsigned wsba_v_sg : 1;
        unsigned wsba_v_ptp : 1;
        unsigned wsba_v_rsvd1 : 17;
        unsigned wsba_v_addr : 12;
        unsigned wsba_v_rsvd2 : 32;
        } wsba_r_bits;
    int wsba_q_whole [2];
    } ;
#endif /* __LANGUAGE_C__ */

#endif /* __TSUNAMI_H_LOADED */
