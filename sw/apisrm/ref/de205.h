/*****************************************************************************/
/*****************************************************************************/
/*									     */
/* file:	de205.h                                                      */
/*                                                                           */
/* Copyright (C) 1994 by                                                     */
/* Digital Equipment Corporation, Maynard, Massachusetts.                    */
/* All rights reserved.                                                      */
/*                                                                           */
/* This software is furnished under a license and may be used and copied     */
/* only  in  accordance  of  the  terms  of  such  license  and with the     */
/* inclusion of the above copyright notice. This software or  any  other     */
/* copies thereof may not be provided or otherwise made available to any     */
/* other person.  No title to and  ownership of the  software is  hereby     */
/* transferred.                                                              */
/*                                                                           */
/* The information in this software is  subject to change without notice     */
/* and  should  not  be  construed  as a commitment by digital equipment     */
/* corporation.                                                              */
/*                                                                           */
/* Digital assumes no responsibility for the use  or  reliability of its     */
/* software on equipment which is not supplied by digital.                   */
/*                                                                           */
/*                                                                           */
/* Abstract:	DE205 register and data structure definitions.		     */
/*                                                                           */
/* Author:	D. Marsh                                                     */
/*                                                                           */
/* Modifications:                                                            */
/*                                                                           */
/*	dpm	27-Apr-1994	Initial entry.                               */
/*                                                                           */
/*									     */
 
/*** MODULE $de205 ***/
/*General program constants                                                  */
#define de205_k_regs1_addr 0x0300

/*Register definitions                                                       */
struct de205_regs {             /* LeMAC I/O registers */
    unsigned char de205_b_csr;	/* control status */       
    unsigned char de205_b_cr;	/* control */
    unsigned char de205_b_icr;	/* Interrupt control */
    unsigned char de205_b_tsr;	/* Transmit status */
    unsigned char de205_b_res1; /* reserved */
    unsigned char de205_b_res2; /* reserved */
    unsigned char de205_b_fmq;	/* free memory queue */
    unsigned char de205_b_fmqc;	/* free memory queue count */
    unsigned char de205_b_rq;	/* receive queue */
    unsigned char de205_b_rqc;	/* receive queue count */
    unsigned char de205_b_tq;	/* transmit queue */
    unsigned char de205_b_tqc;	/* transmit queue count */
    unsigned char de205_b_tdq;	/* transmit done queue */
    unsigned char de205_b_tdqc;	/* transmit done queue count */
    unsigned char de205_b_pir1;	/* page index reg 1 */
    unsigned char de205_b_pir2; /* page index reg 2 */
    unsigned char de205_b_dr;	/* data register */
    unsigned char de205_b_iopr;	/* I/O page register 1 */
    unsigned char de205_b_iobr;	/* I/O base register 2 */
    unsigned char de205_b_mpr;	/* memory page register */
    unsigned char de205_b_mbr;	/* memory base register */
    unsigned char de205_b_ap;	/* address prom */
    unsigned char de205_b_edr1;	/* EEROM data register 1 */
    unsigned char de205_b_edr2;	/* EEROM data register 2 */
    unsigned char de205_b_par0; /* physical address register 0 */
    unsigned char de205_b_par1;	/* physical address register 1 */
    unsigned char de205_b_par2; /* physical address register 2 */
    unsigned char de205_b_par3; /* physical address register 3 */
    unsigned char de205_b_par4; /* physical address register 4 */
    unsigned char de205_b_par5; /* physical address register 5 */
    unsigned char de205_b_cmr;  /* configuration management register */
    } ;

