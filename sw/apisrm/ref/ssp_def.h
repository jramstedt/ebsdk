/*
 * Copyright (C) 1990, 1991, 1992 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	SSP (Storage Systems Port) Definitions
 *
 * Author:	Stephen F. Shirron
 *		jds - Jim Sawin
 *
 * Modifications:
 *
 *	jds	27-May-1992	Added pdreg def and ssp_m_sa_ei for Alpha SSP ECO.
 *
 *	jds	12-Apr-1991	Added KDM70 support for Calypso platform.
 *
 *	sfs	26-Jun-1990	Initial entry.
 */
 
#define ssp_k_int_when_done 0x40000000
#define ssp_k_owned_by_cont 0x80000000

#define ssp_k_maxmsg 128					/* Min 60 for response packets (per SSP) */

#define ssp_s_rsp_ring_len 3
#define ssp_m_rsp_ring_len 7
#define ssp_k_rsp_ring_len 8
#define ssp_s_cmd_ring_len 3
#define ssp_m_cmd_ring_len 7
#define ssp_k_cmd_ring_len 8

#define ssp_v_sa_step1 11
#define ssp_v_sa_step2 12
#define ssp_v_sa_step3 13
#define ssp_v_sa_step4 14
#define ssp_v_sa_error 15
#define ssp_m_sa_ei 8
#define ssp_m_sa_steps 0xf800
#define ssp_m_sa_error 0x8000

#define ssp_k_scratchsize 34					/* This value is carefully chosen: see below */

volatile struct pkt {
    unsigned short int size;
    unsigned credits : 4;
    unsigned type : 4;
    unsigned char conn_id;
    unsigned char buffer [ssp_k_maxmsg];
    } ;

volatile struct comm {
    char scratch[ssp_k_scratchsize];				/* Scratch area: hexaword-aligned! */
/* 
 * NOTE: ssp_k_scratchsize is chosen so that the rsp_desc field will be quadword-aligned.
 */
    unsigned short int scratch_size;				/* Optional scratch area is used by KDM70 */
    unsigned long int scratch_addr;
    union
    {	struct
	{   unsigned short int PFN_mask;
	    unsigned hw : 1;
	    unsigned et : 1;
	    unsigned un : 2;
	    unsigned PSI : 4;
	    char fill;
	} xmi;							/* XMI-specific field defs */
	long int fill;
    } bus_specific;
    unsigned short int cmd_int;
    unsigned short int rsp_int;
    unsigned long int rsp_desc [ssp_k_rsp_ring_len];		/* First descriptor must be quadword-aligned! */
    unsigned long int cmd_desc [ssp_k_cmd_ring_len];
    struct pkt rsp_pkt [ssp_k_rsp_ring_len];			/* Packets must be longword-aligned! */
    struct pkt cmd_pkt [ssp_k_cmd_ring_len];
    } ;

volatile struct step {
    unsigned vector : 7;
    unsigned ie : 1;
    unsigned rsp_len : 3;
    unsigned cmd_len : 3;
    unsigned diag_wrap : 1;
    unsigned IV : 1;						/* Must Be One for non-XMI */
    unsigned long int comm_base;
    unsigned go : 1;
    unsigned last_fail : 1;
    unsigned burst : 6;
    unsigned sf : 1;
    unsigned nn : 1;
    unsigned cs : 1;						/* Must be set for KDM70 */
    unsigned mbz : 5;
    } ;

volatile union pdreg {						/* PD register (used for XMI devices) */
    unsigned lwd;
    struct {
	unsigned short int imask;				/* Interrupt Destination mask */
	unsigned ilevel : 4;					/* Interrupt level */
	unsigned ivect_h : 6;					/* Interrupt vector bits <15:10> if applicable */
	unsigned rsvd : 6;					/* Reserved */
	} bits;
    };
