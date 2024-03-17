#ifndef	__ASCITAB_H_LOADED
#define	__ASCITAB_H_LOADED

/*****************************************************************************

Copyright © 1994, Digital Equipment Corporation, Maynard, Massachusetts.

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

#define ASCITAB_H_RCSID "$Id: ascitab.h,v 1.1.1.1 1998/12/29 21:36:12 paradis Exp $"

/*
 * --------------------------------------------------------------------
 * Header file for mapping ascii characters to #defines.
 *
 *	Author: Rogelio R. Cruz, Digital Equipment Corporation
 *	Date: 6-Sept-1994
 *      Origins: Ken Curewitz
 * $Log: ascitab.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:12  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1994/12/19  18:40:02  cruz
 * Initial revision
 *
 * Revision 1.1  1994/09/06  18:33:19  cruz
 * Initial revision
 *
 * --------------------------------------------------------------------*/

/*
 * this file contains the hex representation of the ascii
 * character set
 */
#define a_nul		0x00
#define a_soh		0x01 
#define a_stx		0x02 
#define a_etx		0x03 
#define a_eot		0x04 
#define a_enq		0x05 
#define a_ack		0x06 
#define a_bel		0x07 
#define a_bs		0x08 
#define a_ht		0x09 
#define a_nl		0x0a 
#define a_vt		0x0b 
#define a_np		0x0c 
#define a_cr		0x0d 
#define a_so		0x0e 
#define a_si		0x0f 
#define a_dle		0x10 
#define a_dc1		0x11 
#define a_dc2		0x12 
#define a_dc3		0x13 
#define a_dc4		0x14 
#define a_nak		0x15 
#define a_syn		0x16 
#define a_etb		0x17 
#define a_can		0x18 
#define a_em		0x19 
#define a_sub		0x1a 
#define a_esc		0x1b 
#define a_fs		0x1c 
#define a_gs		0x1d 
#define a_rs		0x1e 
#define a_us		0x1f 
#define a_sp		0x20 
#define a_exc		0x21 
#define a_dquote	0x22 
#define a_pound		0x23 
#define a_dollar	0x24 
#define a_percent	0x25 
#define a_ampersand	0x26 
#define a_pop		0x27 
#define a_lparen	0x28 
#define a_rparen	0x29 
#define a_star		0x2a 
#define a_plus		0x2b 
#define a_comma		0x2c 
#define a_minus		0x2d 
#define a_period	0x2e 
#define a_slash		0x2f 
#define a_0		0x30 
#define a_1		0x31 
#define a_2		0x32 
#define a_3		0x33 
#define a_4		0x34 
#define a_5		0x35 
#define a_6		0x36 
#define a_7		0x37 
#define a_8		0x38 
#define a_9		0x39 
#define a_colon		0x3a 
#define a_semi		0x3b 
#define a_langle	0x3c 
#define a_equal		0x3d 
#define a_rangle	0x3e 
#define a_question	0x3f 
#define a_at		0x40 
#define a_A		0x41 
#define a_B		0x42 
#define a_C		0x43 
#define a_D		0x44 
#define a_E		0x45 
#define a_F		0x46 
#define a_G		0x47 
#define a_H		0x48 
#define a_I		0x49 
#define a_J		0x4a 
#define a_K		0x4b 
#define a_L		0x4c 
#define a_M		0x4d 
#define a_N		0x4e 
#define a_O		0x4f 
#define a_P		0x50 
#define a_Q		0x51 
#define a_R		0x52 
#define a_S		0x53 
#define a_T		0x54 
#define a_U		0x55 
#define a_V		0x56 
#define a_W		0x57 
#define a_X		0x58 
#define a_Y		0x59 
#define a_Z		0x5a 
#define a_lsqbrack	0x5b 
#define a_bslash	0x5c 
#define a_rsqbrack	0x5d 
#define a_circum	0x5e 
#define a_under		0x5f 
#define a_bquote	0x60 
#define a_a		0x61 
#define a_b		0x62 
#define a_c		0x63 
#define a_d		0x64 
#define a_e		0x65 
#define a_f		0x66 
#define a_g		0x67 
#define a_h		0x68 
#define a_i		0x69 
#define a_j		0x6a 
#define a_k		0x6b 
#define a_l		0x6c 
#define a_m		0x6d 
#define a_n		0x6e 
#define a_o		0x6f 
#define a_p		0x70 
#define a_q		0x71 
#define a_r		0x72 
#define a_s		0x73 
#define a_t		0x74 
#define a_u		0x75 
#define a_v		0x76 
#define a_w		0x77 
#define a_x		0x78 
#define a_y		0x79 
#define a_z		0x7a 
#define a_lcurly	0x7b 
#define a_vbar		0x7c 
#define a_rcurly	0x7d 
#define a_tilde		0x7e 
#define a_del		0x7f 
#endif
