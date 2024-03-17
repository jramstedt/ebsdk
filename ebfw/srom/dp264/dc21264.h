#ifndef __DC21264_LOADED
#define	__DC21264_LOADED	1
/*
*****************************************************************************
**                                                                          *
**  Copyright © 1993, 1994						    *
**  by Digital Equipment Corporation, Maynard, Massachusetts.		    *
**                                                                          *
**  All Rights Reserved							    *
**                                                                          *
**  Permission  is  hereby  granted  to  use, copy, modify and distribute   *
**  this  software  and  its  documentation,  in  both  source  code  and   *
**  object  code  form,  and without fee, for the purpose of distribution   *
**  of this software  or  modifications  of this software within products   *
**  incorporating  an  integrated   circuit  implementing  Digital's  AXP   *
**  architecture,  regardless  of the  source of such integrated circuit,   *
**  provided that the  above copyright  notice and this permission notice   *
**  appear  in  all copies,  and  that  the  name  of  Digital  Equipment   *
**  Corporation  not  be  used  in advertising or publicity pertaining to   *
**  distribution of the  document  or  software without specific, written   *
**  prior permission.							    *
**                                                                          *
**  Digital  Equipment  Corporation   disclaims  all   warranties  and/or   *
**  guarantees  with  regard  to  this  software,  including  all implied   *
**  warranties of fitness for  a  particular purpose and merchantability,   *
**  and makes  no  representations  regarding  the use of, or the results   *
**  of the use of, the software and documentation in terms of correctness,  *
**  accuracy,  reliability,  currentness  or  otherwise;  and you rely on   *
**  the software, documentation and results solely at your own risk.	    *
**                                                                          *
**  AXP is a trademark of Digital Equipment Corporation.		    *
**                                                                          *
*****************************************************************************
**
**  FACILITY:
**
**      DECchip 21264 PALcode
**
**  MODULE:
**
**      dc21264.h
**
**  MODULE DESCRIPTION:
**
**      DECchip 21264 specific definitions (aka ev6)
**
**  AUTHOR: GV
**
**
** $Id: dc21264.h,v 1.3 1999/02/11 19:59:47 gries Exp $
** --------------------------------------------------------------------
** $Log: dc21264.h,v $
** Revision 1.3  1999/02/11 19:59:47  gries
** Changes for White
**
** Revision 1.2  1999/01/21 19:05:53  gries
** First Release to cvs gries
**
** Revision 1.2  1997/07/17  20:43:01  fdh
** Minor edit.
**
** Revision 1.1  1997/07/17  20:41:50  fdh
** Initial revision
**--
*/


/*
**
**  INTERNAL PROCESSOR REGISTER DEFINITIONS
**
**  The internal processor register definitions below are annotated
**  with one of the following symbols:
**
**	RW - The register may be read and written
**	RO - The register may only be read
**	WO - The register may only be written
**
**  For RO and WO registers, all bits and fields within the register are
**  also read-only or write-only.  For RW registers, each bit or field
**  within the register is annotated with one of the following:
**
**	RW  - The bit/field may be read and written
** 	RO  - The bit/field may be read; writes are ignored
**	WO  - The bit/field may be written; reads return UNPREDICTABLE
**	WZ  - The bit/field may be written; reads return a zero value
**	W0C - The bit/field may be read; write-zero-to-clear
**	W1C - The bit/field may be read; write-one-to-clear
**	WA  - The bit/field may be read; write-anything-to-clear
**	RC  - The bit/field may be read, causing state to clear;
**	      writes are ignored
**
*/


/* here are the definitions from ev6_def file */
/* these are useful when coding but are not the actual IPRs */
/* ;; Alpha assembly macro  -m  output created by DEFNR */

/* variable that start with EV6__ are for ev6 only (obviously) */
#define Signature_and_System_Revision	 0xDECB8001
#define EV6__DTBM_DOUBLE_3_ENTRY 	 0x100 
#define EV6__DTBM_DOUBLE_4_ENTRY 	 0x180 
#define EV6__FEN_ENTRY 			 0x200 
#define EV6__UNALIGN_ENTRY 		 0x280 
#define EV6__DTBM_SINGLE_ENTRY 		 0x300 
#define EV6__DFAULT_ENTRY 		 0x380 
#define EV6__OPCDEC_ENTRY 		 0x400 
#define EV6__IACV_ENTRY 		 0x480 
#define EV6__MCHK_ENTRY 		 0x500 
#define EV6__ITB_MISS_ENTRY 		 0x580 
#define EV6__ARITH_ENTRY 		 0x600 
#define EV6__INTERRUPT_ENTRY 		 0x680 
#define EV6__MT_FPCR_ENTRY 		 0x700 
#define EV6__RESET_ENTRY 		 0x780 
#define EV6__HALT_ENTRY 		 0x2000 
#define EV6__CALL_PAL_00_ENTRY 		 0x2000 
#define EV6__CALL_PAL_01_ENTRY 		 0x2040 
#define EV6__CALL_PAL_02_ENTRY 		 0x2080 
#define EV6__CALL_PAL_03_ENTRY 		 0x20c0 
#define EV6__CALL_PAL_04_ENTRY 		 0x2100 
#define EV6__CALL_PAL_05_ENTRY 		 0x2140 
#define EV6__CALL_PAL_06_ENTRY 		 0x2180 
#define EV6__CALL_PAL_07_ENTRY 		 0x21c0 
#define EV6__CALL_PAL_08_ENTRY 		 0x2200 
#define EV6__CALL_PAL_09_ENTRY 		 0x2240 
#define EV6__CALL_PAL_0A_ENTRY 		 0x2280 
#define EV6__CALL_PAL_0B_ENTRY 		 0x22c0 
#define EV6__CALL_PAL_0C_ENTRY 		 0x2300 
#define EV6__CALL_PAL_0D_ENTRY 		 0x2340 
#define EV6__CALL_PAL_0E_ENTRY 		 0x2380 
#define EV6__CALL_PAL_0F_ENTRY 		 0x23c0 
#define EV6__CALL_PAL_10_ENTRY 		 0x2400 
#define EV6__CALL_PAL_11_ENTRY 		 0x2440 
#define EV6__CALL_PAL_12_ENTRY 		 0x2480 
#define EV6__CALL_PAL_13_ENTRY 		 0x24c0 
#define EV6__CALL_PAL_14_ENTRY 		 0x2500 
#define EV6__CALL_PAL_15_ENTRY 		 0x2540 
#define EV6__CALL_PAL_16_ENTRY 		 0x2580 
#define EV6__CALL_PAL_17_ENTRY 		 0x25c0 
#define EV6__CALL_PAL_18_ENTRY 		 0x2600 
#define EV6__CALL_PAL_19_ENTRY 		 0x2640 
#define EV6__CALL_PAL_1A_ENTRY 		 0x2680 
#define EV6__CALL_PAL_1B_ENTRY 		 0x26c0 
#define EV6__CALL_PAL_1C_ENTRY 		 0x2700 
#define EV6__CALL_PAL_1D_ENTRY 		 0x2740 
#define EV6__CALL_PAL_1E_ENTRY 		 0x2780 
#define EV6__CALL_PAL_1F_ENTRY 		 0x27c0 
#define EV6__CALL_PAL_20_ENTRY 		 0x2800 
#define EV6__CALL_PAL_21_ENTRY 		 0x2840 
#define EV6__CALL_PAL_22_ENTRY 		 0x2880 
#define EV6__CALL_PAL_23_ENTRY 		 0x28c0 
#define EV6__CALL_PAL_24_ENTRY 		 0x2900 
#define EV6__CALL_PAL_25_ENTRY 		 0x2940 
#define EV6__CALL_PAL_26_ENTRY 		 0x2980 
#define EV6__CALL_PAL_27_ENTRY 		 0x29c0 
#define EV6__CALL_PAL_28_ENTRY 		 0x2a00 
#define EV6__CALL_PAL_29_ENTRY 		 0x2a40 
#define EV6__CALL_PAL_2A_ENTRY 		 0x2a80 
#define EV6__CALL_PAL_2B_ENTRY 		 0x2ac0 
#define EV6__CALL_PAL_2C_ENTRY 		 0x2b00 
#define EV6__CALL_PAL_2D_ENTRY 		 0x2b40 
#define EV6__CALL_PAL_2E_ENTRY 		 0x2b80 
#define EV6__CALL_PAL_2F_ENTRY 		 0x2bc0 
#define EV6__CALL_PAL_30_ENTRY 		 0x2c00 
#define EV6__CALL_PAL_31_ENTRY 		 0x2c40 
#define EV6__CALL_PAL_32_ENTRY 		 0x2c80 
#define EV6__CALL_PAL_33_ENTRY 		 0x2cc0 
#define EV6__CALL_PAL_34_ENTRY 		 0x2d00 
#define EV6__CALL_PAL_35_ENTRY 		 0x2d40 
#define EV6__CALL_PAL_36_ENTRY 		 0x2d80 
#define EV6__CALL_PAL_37_ENTRY 		 0x2dc0 
#define EV6__CALL_PAL_38_ENTRY 		 0x2e00 
#define EV6__CALL_PAL_39_ENTRY 		 0x2e40 
#define EV6__CALL_PAL_3A_ENTRY 		 0x2e80 
#define EV6__CALL_PAL_3B_ENTRY 		 0x2ec0 
#define EV6__CALL_PAL_3C_ENTRY 		 0x2f00 
#define EV6__CALL_PAL_3D_ENTRY 		 0x2f40 
#define EV6__CALL_PAL_3E_ENTRY 		 0x2f80 
#define EV6__CALL_PAL_3F_ENTRY 		 0x2fc0 
#define EV6__CALL_PAL_80_ENTRY 		 0x3000 
#define EV6__CALL_PAL_81_ENTRY 		 0x3040 
#define EV6__CALL_PAL_82_ENTRY 		 0x3080 
#define EV6__CALL_PAL_83_ENTRY 		 0x30c0 
#define EV6__CALL_PAL_84_ENTRY 		 0x3100 
#define EV6__CALL_PAL_85_ENTRY 		 0x3140 
#define EV6__CALL_PAL_86_ENTRY 		 0x3180 
#define EV6__CALL_PAL_87_ENTRY 		 0x31c0 
#define EV6__CALL_PAL_88_ENTRY 		 0x3200 
#define EV6__CALL_PAL_89_ENTRY 		 0x3240 
#define EV6__CALL_PAL_8A_ENTRY 		 0x3280 
#define EV6__CALL_PAL_8B_ENTRY 		 0x32c0 
#define EV6__CALL_PAL_8C_ENTRY 		 0x3300 
#define EV6__CALL_PAL_8D_ENTRY 		 0x3340 
#define EV6__CALL_PAL_8E_ENTRY 		 0x3380 
#define EV6__CALL_PAL_8F_ENTRY 		 0x33c0 
#define EV6__CALL_PAL_90_ENTRY 		 0x3400 
#define EV6__CALL_PAL_91_ENTRY 		 0x3440 
#define EV6__CALL_PAL_92_ENTRY 		 0x3480 
#define EV6__CALL_PAL_93_ENTRY 		 0x34c0 
#define EV6__CALL_PAL_94_ENTRY 		 0x3500 
#define EV6__CALL_PAL_95_ENTRY 		 0x3540 
#define EV6__CALL_PAL_96_ENTRY 		 0x3580 
#define EV6__CALL_PAL_97_ENTRY 		 0x35c0 
#define EV6__CALL_PAL_98_ENTRY 		 0x3600 
#define EV6__CALL_PAL_99_ENTRY 		 0x3640 
#define EV6__CALL_PAL_9A_ENTRY 		 0x3680 
#define EV6__CALL_PAL_9B_ENTRY 		 0x36c0 
#define EV6__CALL_PAL_9C_ENTRY 		 0x3700 
#define EV6__CALL_PAL_9D_ENTRY 		 0x3740 
#define EV6__CALL_PAL_9E_ENTRY 		 0x3780 
#define EV6__CALL_PAL_9F_ENTRY 		 0x37c0 
#define EV6__CALL_PAL_A0_ENTRY 		 0x3800 
#define EV6__CALL_PAL_A1_ENTRY 		 0x3840 
#define EV6__CALL_PAL_A2_ENTRY 		 0x3880 
#define EV6__CALL_PAL_A3_ENTRY 		 0x38c0 
#define EV6__CALL_PAL_A4_ENTRY 		 0x3900 
#define EV6__CALL_PAL_A5_ENTRY 		 0x3940 
#define EV6__CALL_PAL_A6_ENTRY 		 0x3980 
#define EV6__CALL_PAL_A7_ENTRY 		 0x39c0 
#define EV6__CALL_PAL_A8_ENTRY 		 0x3a00 
#define EV6__CALL_PAL_A9_ENTRY 		 0x3a40 
#define EV6__CALL_PAL_AA_ENTRY 		 0x3a80 
#define EV6__CALL_PAL_AB_ENTRY 		 0x3ac0 
#define EV6__CALL_PAL_AC_ENTRY 		 0x3b00 
#define EV6__CALL_PAL_AD_ENTRY 		 0x3b40 
#define EV6__CALL_PAL_AE_ENTRY 		 0x3b80 
#define EV6__CALL_PAL_AF_ENTRY 		 0x3bc0 
#define EV6__CALL_PAL_B0_ENTRY 		 0x3c00 
#define EV6__CALL_PAL_B1_ENTRY 		 0x3c40 
#define EV6__CALL_PAL_B2_ENTRY 		 0x3c80 
#define EV6__CALL_PAL_B3_ENTRY 		 0x3cc0 
#define EV6__CALL_PAL_B4_ENTRY 		 0x3d00 
#define EV6__CALL_PAL_B5_ENTRY 		 0x3d40 
#define EV6__CALL_PAL_B6_ENTRY 		 0x3d80 
#define EV6__CALL_PAL_B7_ENTRY 		 0x3dc0 
#define EV6__CALL_PAL_B8_ENTRY 		 0x3e00 
#define EV6__CALL_PAL_B9_ENTRY 		 0x3e40 
#define EV6__CALL_PAL_BA_ENTRY 		 0x3e80 
#define EV6__CALL_PAL_BB_ENTRY 		 0x3ec0 
#define EV6__CALL_PAL_BC_ENTRY 		 0x3f00 
#define EV6__CALL_PAL_BD_ENTRY 		 0x3f40 
#define EV6__CALL_PAL_BE_ENTRY 		 0x3f80 
#define EV6__CALL_PAL_BF_ENTRY 		 0x3fc0 

#define EV6__CC__COUNTER_BOT__S 	 0x0 
#define EV6__CC__COUNTER_BOT__V 	 0x4 
#define EV6__CC__COUNTER_BOT__M 	 0xf 
#define EV6__CC__COUNTER__S 		 0x4 
#define EV6__CC__COUNTER__V 		 0x1c 
#define EV6__CC__COUNTER__M 		 0xfffffff 
#define EV6__CC__OFFSET__S 		 0x20 
#define EV6__CC__OFFSET__V 		 0x20 
#define EV6__CC__OFFSET__M 		 0xffffffff 

#define EV6__CC_CTL__RSV1__S 		 0x0 
#define EV6__CC_CTL__RSV1__V 		 0x4 
#define EV6__CC_CTL__RSV1__M 		 0xf 
#define EV6__CC_CTL__COUNTER__S 	 0x4 
#define EV6__CC_CTL__COUNTER__V 	 0x1c 
#define EV6__CC_CTL__COUNTER__M 	 0xfffffff 
#define EV6__CC_CTL__CC_ENA__S 		 0x20 
#define EV6__CC_CTL__CC_ENA__V 		 0x1 
#define EV6__CC_CTL__CC_ENA__M 		 0x1 
#define EV6__CC_CTL__RSV2__S 		 0x21 
#define EV6__CC_CTL__RSV2__V 		 0x1f 
#define EV6__CC_CTL__RSV2__M 		 0x7fffffff 

#define EV6__VA__ADDR__S 		 0x0 
#define EV6__VA__ADDR__V 		 0x40 
#define EV6__VA__ADDR__M 		 0xffffffffffffffff 

#define EV6__VA_FORM__RSV1__S 		 0x0 
#define EV6__VA_FORM__RSV1__V 		 0x3 
#define EV6__VA_FORM__RSV1__M 		 0x7 
#define EV6__VA_FORM__VA__S 		 0x3 
#define EV6__VA_FORM__VA__V 		 0x1e 
#define EV6__VA_FORM__VA__M 		 0x3fffffff 
#define EV6__VA_FORM__VPTB__S 		 0x21 
#define EV6__VA_FORM__VPTB__V 		 0x1f 
#define EV6__VA_FORM__VPTB__M 		 0x7fffffff 

#define EV6__VA_FORM_48__RSV1__S 	 0x0 
#define EV6__VA_FORM_48__RSV1__V 	 0x3 
#define EV6__VA_FORM_48__RSV1__M 	 0x7 
#define EV6__VA_FORM_48__VA__S 		 0x3 
#define EV6__VA_FORM_48__VA__V 		 0x23 
#define EV6__VA_FORM_48__VA__M 		 0x7ffffffff 
#define EV6__VA_FORM_48__VA_SEXT0__S 	 0x26 
#define EV6__VA_FORM_48__VA_SEXT0__V 	 0x1 
#define EV6__VA_FORM_48__VA_SEXT0__M 	 0x1 
#define EV6__VA_FORM_48__VA_SEXT1__S 	 0x27 
#define EV6__VA_FORM_48__VA_SEXT1__V 	 0x1 
#define EV6__VA_FORM_48__VA_SEXT1__M 	 0x1 
#define EV6__VA_FORM_48__VA_SEXT2__S 	 0x28 
#define EV6__VA_FORM_48__VA_SEXT2__V 	 0x1 
#define EV6__VA_FORM_48__VA_SEXT2__M 	 0x1 
#define EV6__VA_FORM_48__VA_SEXT3__S 	 0x29 
#define EV6__VA_FORM_48__VA_SEXT3__V 	 0x1 
#define EV6__VA_FORM_48__VA_SEXT3__M 	 0x1 
#define EV6__VA_FORM_48__VA_SEXT4__S 	 0x2a 
#define EV6__VA_FORM_48__VA_SEXT4__V 	 0x1 
#define EV6__VA_FORM_48__VA_SEXT4__M 	 0x1 
#define EV6__VA_FORM_48__VPTB__S 	 0x2b 
#define EV6__VA_FORM_48__VPTB__V 	 0x15 
#define EV6__VA_FORM_48__VPTB__M 	 0x1fffff 

#define EV6__VA_FORM_32__RSV1__S 	 0x0 
#define EV6__VA_FORM_32__RSV1__V 	 0x3 
#define EV6__VA_FORM_32__RSV1__M 	 0x7 
#define EV6__VA_FORM_32__VA__S 		 0x3 
#define EV6__VA_FORM_32__VA__V 		 0x13 
#define EV6__VA_FORM_32__VA__M 		 0x7ffff 
#define EV6__VA_FORM_32__RSV2__S 	 0x16 
#define EV6__VA_FORM_32__RSV2__V 	 0x8 
#define EV6__VA_FORM_32__RSV2__M 	 0xff 
#define EV6__VA_FORM_32__VPTB__S 	 0x1e 
#define EV6__VA_FORM_32__VPTB__V 	 0x22 
#define EV6__VA_FORM_32__VPTB__M 	 0x3ffffffff 

#define EV6__VA_CTL__B_ENDIAN__S 	 0x0 
#define EV6__VA_CTL__B_ENDIAN__V 	 0x1 
#define EV6__VA_CTL__B_ENDIAN__M 	 0x1 
#define EV6__VA_CTL__VA_48__S 		 0x1 
#define EV6__VA_CTL__VA_48__V 		 0x1 
#define EV6__VA_CTL__VA_48__M 		 0x1 
#define EV6__VA_CTL__VA_FORM_32__S 	 0x2 
#define EV6__VA_CTL__VA_FORM_32__V 	 0x1 
#define EV6__VA_CTL__VA_FORM_32__M 	 0x1 
#define EV6__VA_CTL__RSV1__S 		 0x3 
#define EV6__VA_CTL__RSV1__V 		 0x1b 
#define EV6__VA_CTL__RSV1__M 		 0x7ffffff 
#define EV6__VA_CTL__VPTB__S 		 0x1e 
#define EV6__VA_CTL__VPTB__V 		 0x22 
#define EV6__VA_CTL__VPTB__M 		 0x3ffffffff 

#define EV6__ITB_TAG__RSV1__S 		 0x0 
#define EV6__ITB_TAG__RSV1__V 		 0xd 
#define EV6__ITB_TAG__RSV1__M 		 0x1fff 
#define EV6__ITB_TAG__VA__S 		 0xd 
#define EV6__ITB_TAG__VA__V 		 0x23 
#define EV6__ITB_TAG__VA__M 		 0x7ffffffff 
#define EV6__ITB_TAG__RSV2__S 		 0x30 
#define EV6__ITB_TAG__RSV2__V 		 0x10 
#define EV6__ITB_TAG__RSV2__M 		 0xffff 

#define EV6__ITB_PTE__RSV1__S 		 0x0 
#define EV6__ITB_PTE__RSV1__V 		 0x4 
#define EV6__ITB_PTE__RSV1__M 		 0xf 
#define EV6__ITB_PTE__ASM__S 		 0x4 
#define EV6__ITB_PTE__ASM__V 		 0x1 
#define EV6__ITB_PTE__ASM__M 		 0x1 
#define EV6__ITB_PTE__GH__S 		 0x5 
#define EV6__ITB_PTE__GH__V 		 0x2 
#define EV6__ITB_PTE__GH__M 		 0x3 
#define EV6__ITB_PTE__RSV2__S 		 0x7 
#define EV6__ITB_PTE__RSV2__V 		 0x1 
#define EV6__ITB_PTE__RSV2__M 		 0x1 
#define EV6__ITB_PTE__KRE__S 		 0x8 
#define EV6__ITB_PTE__KRE__V 		 0x1 
#define EV6__ITB_PTE__KRE__M 		 0x1 
#define EV6__ITB_PTE__ERE__S 		 0x9 
#define EV6__ITB_PTE__ERE__V 		 0x1 
#define EV6__ITB_PTE__ERE__M 		 0x1 
#define EV6__ITB_PTE__SRE__S 		 0xa 
#define EV6__ITB_PTE__SRE__V 		 0x1 
#define EV6__ITB_PTE__SRE__M 		 0x1 
#define EV6__ITB_PTE__URE__S 		 0xb 
#define EV6__ITB_PTE__URE__V 		 0x1 
#define EV6__ITB_PTE__URE__M 		 0x1 
#define EV6__ITB_PTE__RSV3__S 		 0xc 
#define EV6__ITB_PTE__RSV3__V 		 0x1 
#define EV6__ITB_PTE__RSV3__M 		 0x1 
#define EV6__ITB_PTE__PFN__S 		 0xd 
#define EV6__ITB_PTE__PFN__V 		 0x1f 
#define EV6__ITB_PTE__PFN__M 		 0x7fffffff 
#define EV6__ITB_PTE__RSV4__S 		 0x2c 
#define EV6__ITB_PTE__RSV4__V 		 0x14 
#define EV6__ITB_PTE__RSV4__M 		 0xfffff 

#define EV6__ITB_IAP__RSV__S 		 0x0 
#define EV6__ITB_IAP__RSV__V 		 0x40 
#define EV6__ITB_IAP__RSV__M 		 0xffffffffffffffff 

#define EV6__ITB_IA__RSV__S 		 0x0 
#define EV6__ITB_IA__RSV__V 		 0x40 
#define EV6__ITB_IA__RSV__M 		 0xffffffffffffffff 

#define EV6__ITB_IS__RSV1__S 		 0x0 
#define EV6__ITB_IS__RSV1__V 		 0xd 
#define EV6__ITB_IS__RSV1__M 		 0x1fff 
#define EV6__ITB_IS__VA__S 		 0xd 
#define EV6__ITB_IS__VA__V 		 0x23 
#define EV6__ITB_IS__VA__M 		 0x7ffffffff 
#define EV6__ITB_IS__RSV2__S 		 0x30 
#define EV6__ITB_IS__RSV2__V 		 0x10 
#define EV6__ITB_IS__RSV2__M 		 0xffff 

#define EV6__EXC_ADDR__PAL__S 		 0x0 
#define EV6__EXC_ADDR__PAL__V 		 0x1 
#define EV6__EXC_ADDR__PAL__M 		 0x1 
#define EV6__EXC_ADDR__RSV__S 		 0x1 
#define EV6__EXC_ADDR__RSV__V 		 0x1 
#define EV6__EXC_ADDR__RSV__M 		 0x1 
#define EV6__EXC_ADDR__PC__S 		 0x2 
#define EV6__EXC_ADDR__PC__V 		 0x2e 
#define EV6__EXC_ADDR__PC__M 		 0x3fffffffffff 
#define EV6__EXC_ADDR__PC_SEXT0__S 	 0x30 
#define EV6__EXC_ADDR__PC_SEXT0__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT0__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT1__S 	 0x31 
#define EV6__EXC_ADDR__PC_SEXT1__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT1__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT2__S 	 0x32 
#define EV6__EXC_ADDR__PC_SEXT2__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT2__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT3__S 	 0x33 
#define EV6__EXC_ADDR__PC_SEXT3__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT3__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT4__S 	 0x34 
#define EV6__EXC_ADDR__PC_SEXT4__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT4__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT5__S 	 0x35 
#define EV6__EXC_ADDR__PC_SEXT5__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT5__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT6__S 	 0x36 
#define EV6__EXC_ADDR__PC_SEXT6__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT6__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT7__S 	 0x37 
#define EV6__EXC_ADDR__PC_SEXT7__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT7__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT8__S 	 0x38 
#define EV6__EXC_ADDR__PC_SEXT8__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT8__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT9__S 	 0x39 
#define EV6__EXC_ADDR__PC_SEXT9__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT9__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT10__S 	 0x3a 
#define EV6__EXC_ADDR__PC_SEXT10__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT10__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT11__S 	 0x3b 
#define EV6__EXC_ADDR__PC_SEXT11__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT11__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT12__S 	 0x3c 
#define EV6__EXC_ADDR__PC_SEXT12__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT12__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT13__S 	 0x3d 
#define EV6__EXC_ADDR__PC_SEXT13__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT13__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT14__S 	 0x3e 
#define EV6__EXC_ADDR__PC_SEXT14__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT14__M 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT15__S 	 0x3f 
#define EV6__EXC_ADDR__PC_SEXT15__V 	 0x1 
#define EV6__EXC_ADDR__PC_SEXT15__M 	 0x1 

#define EV6__IVA_FORM__RSV1__S 		 0x0 
#define EV6__IVA_FORM__RSV1__V 		 0x3 
#define EV6__IVA_FORM__RSV1__M 		 0x7 
#define EV6__IVA_FORM__VA__S 		 0x3 
#define EV6__IVA_FORM__VA__V 		 0x1e 
#define EV6__IVA_FORM__VA__M 		 0x3fffffff 
#define EV6__IVA_FORM__VPTB__S 		 0x21 
#define EV6__IVA_FORM__VPTB__V 		 0x1f 
#define EV6__IVA_FORM__VPTB__M 		 0x7fffffff 

#define EV6__IVA_FORM_48__RSV1__S 	 0x0 
#define EV6__IVA_FORM_48__RSV1__V 	 0x3 
#define EV6__IVA_FORM_48__RSV1__M 	 0x7 
#define EV6__IVA_FORM_48__VA__S 	 0x3 
#define EV6__IVA_FORM_48__VA__V 	 0x23 
#define EV6__IVA_FORM_48__VA__M 	 0x7ffffffff 
#define EV6__IVA_FORM_48__VA_SEXT0__S 	 0x26 
#define EV6__IVA_FORM_48__VA_SEXT0__V 	 0x1 
#define EV6__IVA_FORM_48__VA_SEXT0__M 	 0x1 
#define EV6__IVA_FORM_48__VA_SEXT1__S 	 0x27 
#define EV6__IVA_FORM_48__VA_SEXT1__V 	 0x1 
#define EV6__IVA_FORM_48__VA_SEXT1__M 	 0x1 
#define EV6__IVA_FORM_48__VA_SEXT2__S 	 0x28 
#define EV6__IVA_FORM_48__VA_SEXT2__V 	 0x1 
#define EV6__IVA_FORM_48__VA_SEXT2__M 	 0x1 
#define EV6__IVA_FORM_48__VA_SEXT3__S 	 0x29 
#define EV6__IVA_FORM_48__VA_SEXT3__V 	 0x1 
#define EV6__IVA_FORM_48__VA_SEXT3__M 	 0x1 
#define EV6__IVA_FORM_48__VA_SEXT4__S 	 0x2a 
#define EV6__IVA_FORM_48__VA_SEXT4__V 	 0x1 
#define EV6__IVA_FORM_48__VA_SEXT4__M 	 0x1 
#define EV6__IVA_FORM_48__VPTB__S 	 0x2b 
#define EV6__IVA_FORM_48__VPTB__V 	 0x15 
#define EV6__IVA_FORM_48__VPTB__M 	 0x1fffff 

#define EV6__IVA_FORM_32__RSV1__S 	 0x0 
#define EV6__IVA_FORM_32__RSV1__V 	 0x3 
#define EV6__IVA_FORM_32__RSV1__M 	 0x7 
#define EV6__IVA_FORM_32__VA__S 	 0x3 
#define EV6__IVA_FORM_32__VA__V 	 0x13 
#define EV6__IVA_FORM_32__VA__M 	 0x7ffff 
#define EV6__IVA_FORM_32__RSV2__S 	 0x16 
#define EV6__IVA_FORM_32__RSV2__V 	 0x8 
#define EV6__IVA_FORM_32__RSV2__M 	 0xff 
#define EV6__IVA_FORM_32__VPTB__S 	 0x1e 
#define EV6__IVA_FORM_32__VPTB__V 	 0x22 
#define EV6__IVA_FORM_32__VPTB__M 	 0x3ffffffff 

#define EV6__PS__RSV0__S 		 0x0 
#define EV6__PS__RSV0__V 		 0x3 
#define EV6__PS__RSV0__M 		 0x7 
#define EV6__PS__CM__S 			 0x3 
#define EV6__PS__CM__V 			 0x2 
#define EV6__PS__CM__M 			 0x3 
#define EV6__PS__RSV1__S 		 0x5 
#define EV6__PS__RSV1__V 		 0x8 
#define EV6__PS__RSV1__M 		 0xff 
#define EV6__PS__ASTEN__S 		 0xd 
#define EV6__PS__ASTEN__V 		 0x1 
#define EV6__PS__ASTEN__M 		 0x1 
#define EV6__PS__SIEN__S 		 0xe 
#define EV6__PS__SIEN__V 		 0xf 
#define EV6__PS__SIEN__M 		 0x7fff 
#define EV6__PS__PCEN__S 		 0x1d 
#define EV6__PS__PCEN__V 		 0x2 
#define EV6__PS__PCEN__M 		 0x3 
#define EV6__PS__CREN__S 		 0x1f 
#define EV6__PS__CREN__V 		 0x1 
#define EV6__PS__CREN__M 		 0x1 
#define EV6__PS__SLEN__S 		 0x20 
#define EV6__PS__SLEN__V 		 0x1 
#define EV6__PS__SLEN__M 		 0x1 
#define EV6__PS__EIEN__S 		 0x21 
#define EV6__PS__EIEN__V 		 0x6 
#define EV6__PS__EIEN__M 		 0x3f 
#define EV6__PS__RSV2__S 		 0x27 
#define EV6__PS__RSV2__V 		 0x19 
#define EV6__PS__RSV2__M 		 0x1ffffff 

#define EV6__IER__RSV0__S 		 0x0 
#define EV6__IER__RSV0__V 		 0x3 
#define EV6__IER__RSV0__M 		 0x7 
#define EV6__IER__CM__S 		 0x3 
#define EV6__IER__CM__V 		 0x2 
#define EV6__IER__CM__M 		 0x3 
#define EV6__IER__RSV1__S 		 0x5 
#define EV6__IER__RSV1__V 		 0x8 
#define EV6__IER__RSV1__M 		 0xff 
#define EV6__IER__ASTEN__S 		 0xd 
#define EV6__IER__ASTEN__V 		 0x1 
#define EV6__IER__ASTEN__M 		 0x1 
#define EV6__IER__SIEN__S 		 0xe 
#define EV6__IER__SIEN__V 		 0xf 
#define EV6__IER__SIEN__M 		 0x7fff 
#define EV6__IER__PCEN__S 		 0x1d 
#define EV6__IER__PCEN__V 		 0x2 
#define EV6__IER__PCEN__M 		 0x3 
#define EV6__IER__CREN__S 		 0x1f 
#define EV6__IER__CREN__V 		 0x1 
#define EV6__IER__CREN__M 		 0x1 
#define EV6__IER__SLEN__S 		 0x20 
#define EV6__IER__SLEN__V 		 0x1 
#define EV6__IER__SLEN__M 		 0x1 
#define EV6__IER__EIEN__S 		 0x21 
#define EV6__IER__EIEN__V 		 0x6 
#define EV6__IER__EIEN__M 		 0x3f 
#define EV6__IER__RSV2__S 		 0x27 
#define EV6__IER__RSV2__V 		 0x19 
#define EV6__IER__RSV2__M 		 0x1ffffff 

#define EV6__IER_CM__RSV0__S 		 0x0 
#define EV6__IER_CM__RSV0__V 		 0x3 
#define EV6__IER_CM__RSV0__M 		 0x7 
#define EV6__IER_CM__CM__S 		 0x3 
#define EV6__IER_CM__CM__V 		 0x2 
#define EV6__IER_CM__CM__M 		 0x3 
#define EV6__IER_CM__RSV1__S 		 0x5 
#define EV6__IER_CM__RSV1__V 		 0x8 
#define EV6__IER_CM__RSV1__M 		 0xff 
#define EV6__IER_CM__ASTEN__S 		 0xd 
#define EV6__IER_CM__ASTEN__V 		 0x1 
#define EV6__IER_CM__ASTEN__M 		 0x1 
#define EV6__IER_CM__SIEN__S 		 0xe 
#define EV6__IER_CM__SIEN__V 		 0xf 
#define EV6__IER_CM__SIEN__M 		 0x7fff 
#define EV6__IER_CM__PCEN__S 		 0x1d 
#define EV6__IER_CM__PCEN__V 		 0x2 
#define EV6__IER_CM__PCEN__M 		 0x3 
#define EV6__IER_CM__CREN__S 		 0x1f 
#define EV6__IER_CM__CREN__V 		 0x1 
#define EV6__IER_CM__CREN__M 		 0x1 
#define EV6__IER_CM__SLEN__S 		 0x20 
#define EV6__IER_CM__SLEN__V 		 0x1 
#define EV6__IER_CM__SLEN__M 		 0x1 
#define EV6__IER_CM__EIEN__S 		 0x21 
#define EV6__IER_CM__EIEN__V 		 0x6 
#define EV6__IER_CM__EIEN__M 		 0x3f 
#define EV6__IER_CM__RSV2__S 		 0x27 
#define EV6__IER_CM__RSV2__V 		 0x19 
#define EV6__IER_CM__RSV2__M 		 0x1ffffff 

#define EV6__SIRR__RSV1__S 		 0x0 
#define EV6__SIRR__RSV1__V 		 0xe 
#define EV6__SIRR__RSV1__M 		 0x3fff 
#define EV6__SIRR__SIR__S 		 0xe 
#define EV6__SIRR__SIR__V 		 0xf 
#define EV6__SIRR__SIR__M 		 0x7fff 
#define EV6__SIRR__RSV2__S 		 0x1d 
#define EV6__SIRR__RSV2__V 		 0x23 
#define EV6__SIRR__RSV2__M 		 0x7ffffffff 

#define EV6__ISUM__RSV0__S 		 0x0 
#define EV6__ISUM__RSV0__V 		 0x3 
#define EV6__ISUM__RSV0__M 		 0x7 
#define EV6__ISUM__ASTK__S 		 0x3 
#define EV6__ISUM__ASTK__V 		 0x1 
#define EV6__ISUM__ASTK__M 		 0x1 
#define EV6__ISUM__ASTE__S 		 0x4 
#define EV6__ISUM__ASTE__V 		 0x1 
#define EV6__ISUM__ASTE__M 		 0x1 
#define EV6__ISUM__RSV1__S 		 0x5 
#define EV6__ISUM__RSV1__V 		 0x4 
#define EV6__ISUM__RSV1__M 		 0xf 
#define EV6__ISUM__ASTS__S 		 0x9 
#define EV6__ISUM__ASTS__V 		 0x1 
#define EV6__ISUM__ASTS__M 		 0x1 
#define EV6__ISUM__ASTU__S 		 0xa 
#define EV6__ISUM__ASTU__V 		 0x1 
#define EV6__ISUM__ASTU__M 		 0x1 
#define EV6__ISUM__RSV2__S 		 0xb 
#define EV6__ISUM__RSV2__V 		 0x3 
#define EV6__ISUM__RSV2__M 		 0x7 
#define EV6__ISUM__SI__S 		 0xe 
#define EV6__ISUM__SI__V 		 0xf 
#define EV6__ISUM__SI__M 		 0x7fff 
#define EV6__ISUM__PC__S 		 0x1d 
#define EV6__ISUM__PC__V 		 0x2 
#define EV6__ISUM__PC__M 		 0x3 
#define EV6__ISUM__CR__S 		 0x1f 
#define EV6__ISUM__CR__V 		 0x1 
#define EV6__ISUM__CR__M 		 0x1 
#define EV6__ISUM__SL__S 		 0x20 
#define EV6__ISUM__SL__V 		 0x1 
#define EV6__ISUM__SL__M 		 0x1 
#define EV6__ISUM__EI__S 		 0x21 
#define EV6__ISUM__EI__V 		 0x6 
#define EV6__ISUM__EI__M 		 0x3f 
#define EV6__ISUM__RSV3__S 		 0x27 
#define EV6__ISUM__RSV3__V 		 0x19 
#define EV6__ISUM__RSV3__M 		 0x1ffffff 

#define EV6__HW_INT_CLR__RSV1__S 	 0x0 
#define EV6__HW_INT_CLR__RSV1__V 	 0x1a 
#define EV6__HW_INT_CLR__RSV1__M 	 0x3ffffff 
#define EV6__HW_INT_CLR__FBTP__S 	 0x1a 
#define EV6__HW_INT_CLR__FBTP__V 	 0x1 
#define EV6__HW_INT_CLR__FBTP__M 	 0x1 
#define EV6__HW_INT_CLR__FBDP__S 	 0x1b 
#define EV6__HW_INT_CLR__FBDP__V 	 0x1 
#define EV6__HW_INT_CLR__FBDP__M 	 0x1 
#define EV6__HW_INT_CLR__MCHK_D__S 	 0x1c 
#define EV6__HW_INT_CLR__MCHK_D__V 	 0x1 
#define EV6__HW_INT_CLR__MCHK_D__M 	 0x1 
#define EV6__HW_INT_CLR__PC__S 		 0x1d 
#define EV6__HW_INT_CLR__PC__V 		 0x2 
#define EV6__HW_INT_CLR__PC__M 		 0x3 
#define EV6__HW_INT_CLR__CR__S 		 0x1f 
#define EV6__HW_INT_CLR__CR__V 		 0x1 
#define EV6__HW_INT_CLR__CR__M 		 0x1 
#define EV6__HW_INT_CLR__SL__S 		 0x20 
#define EV6__HW_INT_CLR__SL__V 		 0x1 
#define EV6__HW_INT_CLR__SL__M 		 0x1 
#define EV6__HW_INT_CLR__RSV2__S 	 0x21 
#define EV6__HW_INT_CLR__RSV2__V 	 0x1f 
#define EV6__HW_INT_CLR__RSV2__M 	 0x7fffffff 

#define EV6__EXC_SUM__SWC__S 		 0x0 
#define EV6__EXC_SUM__SWC__V 		 0x1 
#define EV6__EXC_SUM__SWC__M 		 0x1 
#define EV6__EXC_SUM__INV__S 		 0x1 
#define EV6__EXC_SUM__INV__V 		 0x1 
#define EV6__EXC_SUM__INV__M 		 0x1 
#define EV6__EXC_SUM__DZE__S 		 0x2 
#define EV6__EXC_SUM__DZE__V 		 0x1 
#define EV6__EXC_SUM__DZE__M 		 0x1 
#define EV6__EXC_SUM__FOV__S 		 0x3 
#define EV6__EXC_SUM__FOV__V 		 0x1 
#define EV6__EXC_SUM__FOV__M 		 0x1 
#define EV6__EXC_SUM__UNF__S 		 0x4 
#define EV6__EXC_SUM__UNF__V 		 0x1 
#define EV6__EXC_SUM__UNF__M 		 0x1 
#define EV6__EXC_SUM__INE__S 		 0x5 
#define EV6__EXC_SUM__INE__V 		 0x1 
#define EV6__EXC_SUM__INE__M 		 0x1 
#define EV6__EXC_SUM__IOV__S 		 0x6 
#define EV6__EXC_SUM__IOV__V 		 0x1 
#define EV6__EXC_SUM__IOV__M 		 0x1 
#define EV6__EXC_SUM__INT__S 		 0x7 
#define EV6__EXC_SUM__INT__V 		 0x1 
#define EV6__EXC_SUM__INT__M 		 0x1 
#define EV6__EXC_SUM__REG__S 		 0x8 
#define EV6__EXC_SUM__REG__V 		 0x5 
#define EV6__EXC_SUM__REG__M 		 0x1f 
#define EV6__EXC_SUM__BAD_IVA__S 	 0xd 
#define EV6__EXC_SUM__BAD_IVA__V 	 0x1 
#define EV6__EXC_SUM__BAD_IVA__M 	 0x1 
#define EV6__EXC_SUM__RSV1__S 		 0xe 
#define EV6__EXC_SUM__RSV1__V 		 0x1b 
#define EV6__EXC_SUM__RSV1__M 		 0x7ffffff 
#define EV6__EXC_SUM__PC_OVFL__S 	 0x29 
#define EV6__EXC_SUM__PC_OVFL__V 	 0x1 
#define EV6__EXC_SUM__PC_OVFL__M 	 0x1 
#define EV6__EXC_SUM__SET_INV__S 	 0x2a 
#define EV6__EXC_SUM__SET_INV__V 	 0x1 
#define EV6__EXC_SUM__SET_INV__M 	 0x1 
#define EV6__EXC_SUM__SET_DZE__S 	 0x2b 
#define EV6__EXC_SUM__SET_DZE__V 	 0x1 
#define EV6__EXC_SUM__SET_DZE__M 	 0x1 
#define EV6__EXC_SUM__SET_FOV__S 	 0x2c 
#define EV6__EXC_SUM__SET_FOV__V 	 0x1 
#define EV6__EXC_SUM__SET_FOV__M 	 0x1 
#define EV6__EXC_SUM__SET_UNF__S 	 0x2d 
#define EV6__EXC_SUM__SET_UNF__V 	 0x1 
#define EV6__EXC_SUM__SET_UNF__M 	 0x1 
#define EV6__EXC_SUM__SET_INE__S 	 0x2e 
#define EV6__EXC_SUM__SET_INE__V 	 0x1 
#define EV6__EXC_SUM__SET_INE__M 	 0x1 
#define EV6__EXC_SUM__SET_IOV__S 	 0x2f 
#define EV6__EXC_SUM__SET_IOV__V 	 0x1 
#define EV6__EXC_SUM__SET_IOV__M 	 0x1 
#define EV6__EXC_SUM__SEXT0__S 		 0x30 
#define EV6__EXC_SUM__SEXT0__V 		 0x1 
#define EV6__EXC_SUM__SEXT0__M 		 0x1 
#define EV6__EXC_SUM__SEXT1__S 		 0x31 
#define EV6__EXC_SUM__SEXT1__V 		 0x1 
#define EV6__EXC_SUM__SEXT1__M 		 0x1 
#define EV6__EXC_SUM__SEXT2__S 		 0x32 
#define EV6__EXC_SUM__SEXT2__V 		 0x1 
#define EV6__EXC_SUM__SEXT2__M 		 0x1 
#define EV6__EXC_SUM__SEXT3__S 		 0x33 
#define EV6__EXC_SUM__SEXT3__V 		 0x1 
#define EV6__EXC_SUM__SEXT3__M 		 0x1 
#define EV6__EXC_SUM__SEXT4__S 		 0x34 
#define EV6__EXC_SUM__SEXT4__V 		 0x1 
#define EV6__EXC_SUM__SEXT4__M 		 0x1 
#define EV6__EXC_SUM__SEXT5__S 		 0x35 
#define EV6__EXC_SUM__SEXT5__V 		 0x1 
#define EV6__EXC_SUM__SEXT5__M 		 0x1 
#define EV6__EXC_SUM__SEXT6__S 		 0x36 
#define EV6__EXC_SUM__SEXT6__V 		 0x1 
#define EV6__EXC_SUM__SEXT6__M 		 0x1 
#define EV6__EXC_SUM__SEXT7__S 		 0x37 
#define EV6__EXC_SUM__SEXT7__V 		 0x1 
#define EV6__EXC_SUM__SEXT7__M 		 0x1 
#define EV6__EXC_SUM__SEXT8__S 		 0x38 
#define EV6__EXC_SUM__SEXT8__V 		 0x1 
#define EV6__EXC_SUM__SEXT8__M 		 0x1 
#define EV6__EXC_SUM__SEXT9__S 		 0x39 
#define EV6__EXC_SUM__SEXT9__V 		 0x1 
#define EV6__EXC_SUM__SEXT9__M 		 0x1 
#define EV6__EXC_SUM__SEXT10__S 	 0x3a 
#define EV6__EXC_SUM__SEXT10__V 	 0x1 
#define EV6__EXC_SUM__SEXT10__M 	 0x1 
#define EV6__EXC_SUM__SEXT11__S 	 0x3b 
#define EV6__EXC_SUM__SEXT11__V 	 0x1 
#define EV6__EXC_SUM__SEXT11__M 	 0x1 
#define EV6__EXC_SUM__SEXT12__S 	 0x3c 
#define EV6__EXC_SUM__SEXT12__V 	 0x1 
#define EV6__EXC_SUM__SEXT12__M 	 0x1 
#define EV6__EXC_SUM__SEXT13__S 	 0x3d 
#define EV6__EXC_SUM__SEXT13__V 	 0x1 
#define EV6__EXC_SUM__SEXT13__M 	 0x1 
#define EV6__EXC_SUM__SEXT14__S 	 0x3e 
#define EV6__EXC_SUM__SEXT14__V 	 0x1 
#define EV6__EXC_SUM__SEXT14__M 	 0x1 
#define EV6__EXC_SUM__SEXT15__S 	 0x3f 
#define EV6__EXC_SUM__SEXT15__V 	 0x1 
#define EV6__EXC_SUM__SEXT15__M 	 0x1 

#define EV6__PAL_BASE__RSV1__S 		 0x0 
#define EV6__PAL_BASE__RSV1__V 		 0xf 
#define EV6__PAL_BASE__RSV1__M 		 0x7fff 
#define EV6__PAL_BASE__PAL_BASE__S 	 0xf 
#define EV6__PAL_BASE__PAL_BASE__V 	 0x1d 
#define EV6__PAL_BASE__PAL_BASE__M 	 0x1fffffff 
#define EV6__PAL_BASE__RSV2__S 		 0x2c 
#define EV6__PAL_BASE__RSV2__V 		 0x14 
#define EV6__PAL_BASE__RSV2__M 		 0xfffff 

#define EV6__I_CTL__SPCE__S 		 0x0 
#define EV6__I_CTL__SPCE__V 		 0x1 
#define EV6__I_CTL__SPCE__M 		 0x1 
#define EV6__I_CTL__IC_EN__S 		 0x1 
#define EV6__I_CTL__IC_EN__V 		 0x2 
#define EV6__I_CTL__IC_EN__M 		 0x3 
#define EV6__I_CTL__SPE__S 		 0x3 
#define EV6__I_CTL__SPE__V 		 0x3 
#define EV6__I_CTL__SPE__M 		 0x7 
#define EV6__I_CTL__SDE__S 		 0x6 
#define EV6__I_CTL__SDE__V 		 0x2 
#define EV6__I_CTL__SDE__M 		 0x3 
#define EV6__I_CTL__SBE__S 		 0x8 
#define EV6__I_CTL__SBE__V 		 0x2 
#define EV6__I_CTL__SBE__M 		 0x3 
#define EV6__I_CTL__BP_MODE__S 		 0xa 
#define EV6__I_CTL__BP_MODE__V 		 0x2 
#define EV6__I_CTL__BP_MODE__M 		 0x3 
#define EV6__I_CTL__HWE__S 		 0xc 
#define EV6__I_CTL__HWE__V 		 0x1 
#define EV6__I_CTL__HWE__M 		 0x1 
#define EV6__I_CTL__SL_XMIT__S 		 0xd 
#define EV6__I_CTL__SL_XMIT__V 		 0x1 
#define EV6__I_CTL__SL_XMIT__M 		 0x1 
#define EV6__I_CTL__SL_RCV__S 		 0xe 
#define EV6__I_CTL__SL_RCV__V 		 0x1 
#define EV6__I_CTL__SL_RCV__M 		 0x1 
#define EV6__I_CTL__VA_48__S 		 0xf 
#define EV6__I_CTL__VA_48__V 		 0x1 
#define EV6__I_CTL__VA_48__M 		 0x1 
#define EV6__I_CTL__VA_FORM_32__S 	 0x10 
#define EV6__I_CTL__VA_FORM_32__V 	 0x1 
#define EV6__I_CTL__VA_FORM_32__M 	 0x1 
#define EV6__I_CTL__SINGLE_ISSUE__S 	 0x11 
#define EV6__I_CTL__SINGLE_ISSUE__V 	 0x1 
#define EV6__I_CTL__SINGLE_ISSUE__M 	 0x1 
#define EV6__I_CTL__PCT0_EN__S 		 0x12 
#define EV6__I_CTL__PCT0_EN__V 		 0x1 
#define EV6__I_CTL__PCT0_EN__M 		 0x1 
#define EV6__I_CTL__PCT1_EN__S 		 0x13 
#define EV6__I_CTL__PCT1_EN__V 		 0x1 
#define EV6__I_CTL__PCT1_EN__M 		 0x1 
#define EV6__I_CTL__CALL_PAL_R23__S 	 0x14 
#define EV6__I_CTL__CALL_PAL_R23__V 	 0x1 
#define EV6__I_CTL__CALL_PAL_R23__M 	 0x1 
#define EV6__I_CTL__MCHK_EN__S 		 0x15 
#define EV6__I_CTL__MCHK_EN__V 		 0x1 
#define EV6__I_CTL__MCHK_EN__M 		 0x1 
#define EV6__I_CTL__TB_MB_EN__S 	 0x16 
#define EV6__I_CTL__TB_MB_EN__V 	 0x1 
#define EV6__I_CTL__TB_MB_EN__M 	 0x1 
#define EV6__I_CTL__BIST_FAIL__S 	 0x17 
#define EV6__I_CTL__BIST_FAIL__V 	 0x1 
#define EV6__I_CTL__BIST_FAIL__M 	 0x1 
#define EV6__I_CTL__RSV1__S 		 0x18 
#define EV6__I_CTL__RSV1__V 		 0x6 
#define EV6__I_CTL__RSV1__M 		 0x3f 
#define EV6__I_CTL__VPTB__S 		 0x1e 
#define EV6__I_CTL__VPTB__V 		 0x12 
#define EV6__I_CTL__VPTB__M 		 0x3ffff 
#define EV6__I_CTL__VPTB_SEXT0__S 	 0x30 
#define EV6__I_CTL__VPTB_SEXT0__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT0__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT1__S 	 0x31 
#define EV6__I_CTL__VPTB_SEXT1__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT1__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT2__S 	 0x32 
#define EV6__I_CTL__VPTB_SEXT2__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT2__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT3__S 	 0x33 
#define EV6__I_CTL__VPTB_SEXT3__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT3__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT4__S 	 0x34 
#define EV6__I_CTL__VPTB_SEXT4__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT4__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT5__S 	 0x35 
#define EV6__I_CTL__VPTB_SEXT5__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT5__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT6__S 	 0x36 
#define EV6__I_CTL__VPTB_SEXT6__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT6__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT7__S 	 0x37 
#define EV6__I_CTL__VPTB_SEXT7__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT7__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT8__S 	 0x38 
#define EV6__I_CTL__VPTB_SEXT8__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT8__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT9__S 	 0x39 
#define EV6__I_CTL__VPTB_SEXT9__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT9__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT10__S 	 0x3a 
#define EV6__I_CTL__VPTB_SEXT10__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT10__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT11__S 	 0x3b 
#define EV6__I_CTL__VPTB_SEXT11__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT11__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT12__S 	 0x3c 
#define EV6__I_CTL__VPTB_SEXT12__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT12__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT13__S 	 0x3d 
#define EV6__I_CTL__VPTB_SEXT13__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT13__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT14__S 	 0x3e 
#define EV6__I_CTL__VPTB_SEXT14__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT14__M 	 0x1 
#define EV6__I_CTL__VPTB_SEXT15__S 	 0x3f 
#define EV6__I_CTL__VPTB_SEXT15__V 	 0x1 
#define EV6__I_CTL__VPTB_SEXT15__M 	 0x1 

#define EV6__IC_FLUSH_ASM__RSV__S 	 0x0 
#define EV6__IC_FLUSH_ASM__RSV__V 	 0x40 
#define EV6__IC_FLUSH_ASM__RSV__M 	 0xffffffffffffffff 

#define EV6__IC_FLUSH__RSV__S 		 0x0 
#define EV6__IC_FLUSH__RSV__V 		 0x40 
#define EV6__IC_FLUSH__RSV__M 		 0xffffffffffffffff 

#define EV6__PCTR_CTL__SL1__S 		 0x0 
#define EV6__PCTR_CTL__SL1__V 		 0x4 
#define EV6__PCTR_CTL__SL1__M 		 0xf 
#define EV6__PCTR_CTL__SL0__S 		 0x4 
#define EV6__PCTR_CTL__SL0__V 		 0x1 
#define EV6__PCTR_CTL__SL0__M 		 0x1 
#define EV6__PCTR_CTL__RSV1__S 		 0x5 
#define EV6__PCTR_CTL__RSV1__V 		 0x1 
#define EV6__PCTR_CTL__RSV1__M 		 0x1 
#define EV6__PCTR_CTL__PCTR1__S 	 0x6 
#define EV6__PCTR_CTL__PCTR1__V 	 0x14 
#define EV6__PCTR_CTL__PCTR1__M 	 0xfffff 
#define EV6__PCTR_CTL__RSV2__S 		 0x1a 
#define EV6__PCTR_CTL__RSV2__V 		 0x2 
#define EV6__PCTR_CTL__RSV2__M 		 0x3 
#define EV6__PCTR_CTL__PCTR0__S 	 0x1c 
#define EV6__PCTR_CTL__PCTR0__V 	 0x14 
#define EV6__PCTR_CTL__PCTR0__M 	 0xfffff 
#define EV6__PCTR_CTL__RSV3__S 		 0x30 
#define EV6__PCTR_CTL__RSV3__V 		 0x10 
#define EV6__PCTR_CTL__RSV3__M 		 0xffff 

#define EV6__CLR_MAP__RSV__S 		 0x0 
#define EV6__CLR_MAP__RSV__V 		 0x40 
#define EV6__CLR_MAP__RSV__M 		 0xffffffffffffffff 

#define EV6__I_STAT__RSV1__S 		 0x0 
#define EV6__I_STAT__RSV1__V 		 0x1d 
#define EV6__I_STAT__RSV1__M 		 0x1fffffff 
#define EV6__I_STAT__TPE__S 		 0x1d 
#define EV6__I_STAT__TPE__V 		 0x1 
#define EV6__I_STAT__TPE__M 		 0x1 
#define EV6__I_STAT__DPE__S 		 0x1e 
#define EV6__I_STAT__DPE__V 		 0x1 
#define EV6__I_STAT__DPE__M 		 0x1 
#define EV6__I_STAT__RSV2__S 		 0x1f 
#define EV6__I_STAT__RSV2__V 		 0x21 
#define EV6__I_STAT__RSV2__M 		 0x1ffffffff 

#define EV6__SLEEP__RSV__S 		 0x0 
#define EV6__SLEEP__RSV__V 		 0x40 
#define EV6__SLEEP__RSV__M 		 0xffffffffffffffff 

#define EV6__ASN__RSV0__S 		 0x0 
#define EV6__ASN__RSV0__V 		 0x1 
#define EV6__ASN__RSV0__M 		 0x1 
#define EV6__ASN__PPCE__S 		 0x1 
#define EV6__ASN__PPCE__V 		 0x1 
#define EV6__ASN__PPCE__M 		 0x1 
#define EV6__ASN__FPE__S 		 0x2 
#define EV6__ASN__FPE__V 		 0x1 
#define EV6__ASN__FPE__M 		 0x1 
#define EV6__ASN__RSV1__S 		 0x3 
#define EV6__ASN__RSV1__V 		 0x2 
#define EV6__ASN__RSV1__M 		 0x3 
#define EV6__ASN__ASTER__S 		 0x5 
#define EV6__ASN__ASTER__V 		 0x4 
#define EV6__ASN__ASTER__M 		 0xf 
#define EV6__ASN__ASTRR__S 		 0x9 
#define EV6__ASN__ASTRR__V 		 0x4 
#define EV6__ASN__ASTRR__M 		 0xf 
#define EV6__ASN__RSV2__S 		 0xd 
#define EV6__ASN__RSV2__V 		 0x1a 
#define EV6__ASN__RSV2__M 		 0x3ffffff 
#define EV6__ASN__ASN__S 		 0x27 
#define EV6__ASN__ASN__V 		 0x8 
#define EV6__ASN__ASN__M 		 0xff 
#define EV6__ASN__RSV3__S 		 0x2f 
#define EV6__ASN__RSV3__V 		 0x11 
#define EV6__ASN__RSV3__M 		 0x1ffff 

#define EV6__ASTER__RSV0__S 		 0x0 
#define EV6__ASTER__RSV0__V 		 0x1 
#define EV6__ASTER__RSV0__M 		 0x1 
#define EV6__ASTER__PPCE__S 		 0x1 
#define EV6__ASTER__PPCE__V 		 0x1 
#define EV6__ASTER__PPCE__M 		 0x1 
#define EV6__ASTER__FPE__S 		 0x2 
#define EV6__ASTER__FPE__V 		 0x1 
#define EV6__ASTER__FPE__M 		 0x1 
#define EV6__ASTER__RSV1__S 		 0x3 
#define EV6__ASTER__RSV1__V 		 0x2 
#define EV6__ASTER__RSV1__M 		 0x3 
#define EV6__ASTER__ASTER__S 		 0x5 
#define EV6__ASTER__ASTER__V 		 0x4 
#define EV6__ASTER__ASTER__M 		 0xf 
#define EV6__ASTER__ASTRR__S 		 0x9 
#define EV6__ASTER__ASTRR__V 		 0x4 
#define EV6__ASTER__ASTRR__M 		 0xf 
#define EV6__ASTER__RSV2__S 		 0xd 
#define EV6__ASTER__RSV2__V 		 0x1a 
#define EV6__ASTER__RSV2__M 		 0x3ffffff 
#define EV6__ASTER__ASN__S 		 0x27 
#define EV6__ASTER__ASN__V 		 0x8 
#define EV6__ASTER__ASN__M 		 0xff 
#define EV6__ASTER__RSV3__S 		 0x2f 
#define EV6__ASTER__RSV3__V 		 0x11 
#define EV6__ASTER__RSV3__M 		 0x1ffff 

#define EV6__ASTRR__RSV0__S 		 0x0 
#define EV6__ASTRR__RSV0__V 		 0x1 
#define EV6__ASTRR__RSV0__M 		 0x1 
#define EV6__ASTRR__PPCE__S 		 0x1 
#define EV6__ASTRR__PPCE__V 		 0x1 
#define EV6__ASTRR__PPCE__M 		 0x1 
#define EV6__ASTRR__FPE__S 		 0x2 
#define EV6__ASTRR__FPE__V 		 0x1 
#define EV6__ASTRR__FPE__M 		 0x1 
#define EV6__ASTRR__RSV1__S 		 0x3 
#define EV6__ASTRR__RSV1__V 		 0x2 
#define EV6__ASTRR__RSV1__M 		 0x3 
#define EV6__ASTRR__ASTER__S 		 0x5 
#define EV6__ASTRR__ASTER__V 		 0x4 
#define EV6__ASTRR__ASTER__M 		 0xf 
#define EV6__ASTRR__ASTRR__S 		 0x9 
#define EV6__ASTRR__ASTRR__V 		 0x4 
#define EV6__ASTRR__ASTRR__M 		 0xf 
#define EV6__ASTRR__RSV2__S 		 0xd 
#define EV6__ASTRR__RSV2__V 		 0x1a 
#define EV6__ASTRR__RSV2__M 		 0x3ffffff 
#define EV6__ASTRR__ASN__S 		 0x27 
#define EV6__ASTRR__ASN__V 		 0x8 
#define EV6__ASTRR__ASN__M 		 0xff 
#define EV6__ASTRR__RSV3__S 		 0x2f 
#define EV6__ASTRR__RSV3__V 		 0x11 
#define EV6__ASTRR__RSV3__M 		 0x1ffff 

#define EV6__PPCE__RSV0__S 		 0x0 
#define EV6__PPCE__RSV0__V 		 0x1 
#define EV6__PPCE__RSV0__M 		 0x1 
#define EV6__PPCE__PPCE__S 		 0x1 
#define EV6__PPCE__PPCE__V 		 0x1 
#define EV6__PPCE__PPCE__M 		 0x1 
#define EV6__PPCE__FPE__S 		 0x2 
#define EV6__PPCE__FPE__V 		 0x1 
#define EV6__PPCE__FPE__M 		 0x1 
#define EV6__PPCE__RSV1__S 		 0x3 
#define EV6__PPCE__RSV1__V 		 0x2 
#define EV6__PPCE__RSV1__M 		 0x3 
#define EV6__PPCE__ASTER__S 		 0x5 
#define EV6__PPCE__ASTER__V 		 0x4 
#define EV6__PPCE__ASTER__M 		 0xf 
#define EV6__PPCE__ASTRR__S 		 0x9 
#define EV6__PPCE__ASTRR__V 		 0x4 
#define EV6__PPCE__ASTRR__M 		 0xf 
#define EV6__PPCE__RSV2__S 		 0xd 
#define EV6__PPCE__RSV2__V 		 0x1a 
#define EV6__PPCE__RSV2__M 		 0x3ffffff 
#define EV6__PPCE__ASN__S 		 0x27 
#define EV6__PPCE__ASN__V 		 0x8 
#define EV6__PPCE__ASN__M 		 0xff 
#define EV6__PPCE__RSV3__S 		 0x2f 
#define EV6__PPCE__RSV3__V 		 0x11 
#define EV6__PPCE__RSV3__M 		 0x1ffff 

#define EV6__FPE__RSV0__S 		 0x0 
#define EV6__FPE__RSV0__V 		 0x1 
#define EV6__FPE__RSV0__M 		 0x1 
#define EV6__FPE__PPCE__S 		 0x1 
#define EV6__FPE__PPCE__V 		 0x1 
#define EV6__FPE__PPCE__M 		 0x1 
#define EV6__FPE__FPE__S 		 0x2 
#define EV6__FPE__FPE__V 		 0x1 
#define EV6__FPE__FPE__M 		 0x1 
#define EV6__FPE__RSV1__S 		 0x3 
#define EV6__FPE__RSV1__V 		 0x2 
#define EV6__FPE__RSV1__M 		 0x3 
#define EV6__FPE__ASTER__S 		 0x5 
#define EV6__FPE__ASTER__V 		 0x4 
#define EV6__FPE__ASTER__M 		 0xf 
#define EV6__FPE__ASTRR__S 		 0x9 
#define EV6__FPE__ASTRR__V 		 0x4 
#define EV6__FPE__ASTRR__M 		 0xf 
#define EV6__FPE__RSV2__S 		 0xd 
#define EV6__FPE__RSV2__V 		 0x1a 
#define EV6__FPE__RSV2__M 		 0x3ffffff 
#define EV6__FPE__ASN__S 		 0x27 
#define EV6__FPE__ASN__V 		 0x8 
#define EV6__FPE__ASN__M 		 0xff 
#define EV6__FPE__RSV3__S 		 0x2f 
#define EV6__FPE__RSV3__V 		 0x11 
#define EV6__FPE__RSV3__M 		 0x1ffff 

#define EV6__ASN_ASTER__RSV0__S 		 0x0 
#define EV6__ASN_ASTER__RSV0__V 		 0x1 
#define EV6__ASN_ASTER__RSV0__M 		 0x1 
#define EV6__ASN_ASTER__PPCE__S 		 0x1 
#define EV6__ASN_ASTER__PPCE__V 		 0x1 
#define EV6__ASN_ASTER__PPCE__M 		 0x1 
#define EV6__ASN_ASTER__FPE__S 		 0x2 
#define EV6__ASN_ASTER__FPE__V 		 0x1 
#define EV6__ASN_ASTER__FPE__M 		 0x1 
#define EV6__ASN_ASTER__RSV1__S 		 0x3 
#define EV6__ASN_ASTER__RSV1__V 		 0x2 
#define EV6__ASN_ASTER__RSV1__M 		 0x3 
#define EV6__ASN_ASTER__ASTER__S 		 0x5 
#define EV6__ASN_ASTER__ASTER__V 		 0x4 
#define EV6__ASN_ASTER__ASTER__M 		 0xf 
#define EV6__ASN_ASTER__ASTRR__S 		 0x9 
#define EV6__ASN_ASTER__ASTRR__V 		 0x4 
#define EV6__ASN_ASTER__ASTRR__M 		 0xf 
#define EV6__ASN_ASTER__RSV2__S 		 0xd 
#define EV6__ASN_ASTER__RSV2__V 		 0x1a 
#define EV6__ASN_ASTER__RSV2__M 		 0x3ffffff 
#define EV6__ASN_ASTER__ASN__S 		 0x27 
#define EV6__ASN_ASTER__ASN__V 		 0x8 
#define EV6__ASN_ASTER__ASN__M 		 0xff 
#define EV6__ASN_ASTER__RSV3__S 		 0x2f 
#define EV6__ASN_ASTER__RSV3__V 		 0x11 
#define EV6__ASN_ASTER__RSV3__M 		 0x1ffff 

#define EV6__ASN_ASTRR__RSV0__S 		 0x0 
#define EV6__ASN_ASTRR__RSV0__V 		 0x1 
#define EV6__ASN_ASTRR__RSV0__M 		 0x1 
#define EV6__ASN_ASTRR__PPCE__S 		 0x1 
#define EV6__ASN_ASTRR__PPCE__V 		 0x1 
#define EV6__ASN_ASTRR__PPCE__M 		 0x1 
#define EV6__ASN_ASTRR__FPE__S 		 0x2 
#define EV6__ASN_ASTRR__FPE__V 		 0x1 
#define EV6__ASN_ASTRR__FPE__M 		 0x1 
#define EV6__ASN_ASTRR__RSV1__S 		 0x3 
#define EV6__ASN_ASTRR__RSV1__V 		 0x2 
#define EV6__ASN_ASTRR__RSV1__M 		 0x3 
#define EV6__ASN_ASTRR__ASTER__S 		 0x5 
#define EV6__ASN_ASTRR__ASTER__V 		 0x4 
#define EV6__ASN_ASTRR__ASTER__M 		 0xf 
#define EV6__ASN_ASTRR__ASTRR__S 		 0x9 
#define EV6__ASN_ASTRR__ASTRR__V 		 0x4 
#define EV6__ASN_ASTRR__ASTRR__M 		 0xf 
#define EV6__ASN_ASTRR__RSV2__S 		 0xd 
#define EV6__ASN_ASTRR__RSV2__V 		 0x1a 
#define EV6__ASN_ASTRR__RSV2__M 		 0x3ffffff 
#define EV6__ASN_ASTRR__ASN__S 		 0x27 
#define EV6__ASN_ASTRR__ASN__V 		 0x8 
#define EV6__ASN_ASTRR__ASN__M 		 0xff 
#define EV6__ASN_ASTRR__RSV3__S 		 0x2f 
#define EV6__ASN_ASTRR__RSV3__V 		 0x11 
#define EV6__ASN_ASTRR__RSV3__M 		 0x1ffff 

#define EV6__ASN_PPCE__RSV0__S 		 0x0 
#define EV6__ASN_PPCE__RSV0__V 		 0x1 
#define EV6__ASN_PPCE__RSV0__M 		 0x1 
#define EV6__ASN_PPCE__PPCE__S 		 0x1 
#define EV6__ASN_PPCE__PPCE__V 		 0x1 
#define EV6__ASN_PPCE__PPCE__M 		 0x1 
#define EV6__ASN_PPCE__FPE__S 		 0x2 
#define EV6__ASN_PPCE__FPE__V 		 0x1 
#define EV6__ASN_PPCE__FPE__M 		 0x1 
#define EV6__ASN_PPCE__RSV1__S 		 0x3 
#define EV6__ASN_PPCE__RSV1__V 		 0x2 
#define EV6__ASN_PPCE__RSV1__M 		 0x3 
#define EV6__ASN_PPCE__ASTER__S 		 0x5 
#define EV6__ASN_PPCE__ASTER__V 		 0x4 
#define EV6__ASN_PPCE__ASTER__M 		 0xf 
#define EV6__ASN_PPCE__ASTRR__S 		 0x9 
#define EV6__ASN_PPCE__ASTRR__V 		 0x4 
#define EV6__ASN_PPCE__ASTRR__M 		 0xf 
#define EV6__ASN_PPCE__RSV2__S 		 0xd 
#define EV6__ASN_PPCE__RSV2__V 		 0x1a 
#define EV6__ASN_PPCE__RSV2__M 		 0x3ffffff 
#define EV6__ASN_PPCE__ASN__S 		 0x27 
#define EV6__ASN_PPCE__ASN__V 		 0x8 
#define EV6__ASN_PPCE__ASN__M 		 0xff 
#define EV6__ASN_PPCE__RSV3__S 		 0x2f 
#define EV6__ASN_PPCE__RSV3__V 		 0x11 
#define EV6__ASN_PPCE__RSV3__M 		 0x1ffff 

#define EV6__ASN_FPE__RSV0__S 		 0x0 
#define EV6__ASN_FPE__RSV0__V 		 0x1 
#define EV6__ASN_FPE__RSV0__M 		 0x1 
#define EV6__ASN_FPE__PPCE__S 		 0x1 
#define EV6__ASN_FPE__PPCE__V 		 0x1 
#define EV6__ASN_FPE__PPCE__M 		 0x1 
#define EV6__ASN_FPE__FPE__S 		 0x2 
#define EV6__ASN_FPE__FPE__V 		 0x1 
#define EV6__ASN_FPE__FPE__M 		 0x1 
#define EV6__ASN_FPE__RSV1__S 		 0x3 
#define EV6__ASN_FPE__RSV1__V 		 0x2 
#define EV6__ASN_FPE__RSV1__M 		 0x3 
#define EV6__ASN_FPE__ASTER__S 		 0x5 
#define EV6__ASN_FPE__ASTER__V 		 0x4 
#define EV6__ASN_FPE__ASTER__M 		 0xf 
#define EV6__ASN_FPE__ASTRR__S 		 0x9 
#define EV6__ASN_FPE__ASTRR__V 		 0x4 
#define EV6__ASN_FPE__ASTRR__M 		 0xf 
#define EV6__ASN_FPE__RSV2__S 		 0xd 
#define EV6__ASN_FPE__RSV2__V 		 0x1a 
#define EV6__ASN_FPE__RSV2__M 		 0x3ffffff 
#define EV6__ASN_FPE__ASN__S 		 0x27 
#define EV6__ASN_FPE__ASN__V 		 0x8 
#define EV6__ASN_FPE__ASN__M 		 0xff 
#define EV6__ASN_FPE__RSV3__S 		 0x2f 
#define EV6__ASN_FPE__RSV3__V 		 0x11 
#define EV6__ASN_FPE__RSV3__M 		 0x1ffff 

#define EV6__ASTER_ASTRR__RSV0__S 		 0x0 
#define EV6__ASTER_ASTRR__RSV0__V 		 0x1 
#define EV6__ASTER_ASTRR__RSV0__M 		 0x1 
#define EV6__ASTER_ASTRR__PPCE__S 		 0x1 
#define EV6__ASTER_ASTRR__PPCE__V 		 0x1 
#define EV6__ASTER_ASTRR__PPCE__M 		 0x1 
#define EV6__ASTER_ASTRR__FPE__S 		 0x2 
#define EV6__ASTER_ASTRR__FPE__V 		 0x1 
#define EV6__ASTER_ASTRR__FPE__M 		 0x1 
#define EV6__ASTER_ASTRR__RSV1__S 		 0x3 
#define EV6__ASTER_ASTRR__RSV1__V 		 0x2 
#define EV6__ASTER_ASTRR__RSV1__M 		 0x3 
#define EV6__ASTER_ASTRR__ASTER__S 		 0x5 
#define EV6__ASTER_ASTRR__ASTER__V 		 0x4 
#define EV6__ASTER_ASTRR__ASTER__M 		 0xf 
#define EV6__ASTER_ASTRR__ASTRR__S 		 0x9 
#define EV6__ASTER_ASTRR__ASTRR__V 		 0x4 
#define EV6__ASTER_ASTRR__ASTRR__M 		 0xf 
#define EV6__ASTER_ASTRR__RSV2__S 		 0xd 
#define EV6__ASTER_ASTRR__RSV2__V 		 0x1a 
#define EV6__ASTER_ASTRR__RSV2__M 		 0x3ffffff 
#define EV6__ASTER_ASTRR__ASN__S 		 0x27 
#define EV6__ASTER_ASTRR__ASN__V 		 0x8 
#define EV6__ASTER_ASTRR__ASN__M 		 0xff 
#define EV6__ASTER_ASTRR__RSV3__S 		 0x2f 
#define EV6__ASTER_ASTRR__RSV3__V 		 0x11 
#define EV6__ASTER_ASTRR__RSV3__M 		 0x1ffff 

#define EV6__ASTER_PPCE__RSV0__S 		 0x0 
#define EV6__ASTER_PPCE__RSV0__V 		 0x1 
#define EV6__ASTER_PPCE__RSV0__M 		 0x1 
#define EV6__ASTER_PPCE__PPCE__S 		 0x1 
#define EV6__ASTER_PPCE__PPCE__V 		 0x1 
#define EV6__ASTER_PPCE__PPCE__M 		 0x1 
#define EV6__ASTER_PPCE__FPE__S 		 0x2 
#define EV6__ASTER_PPCE__FPE__V 		 0x1 
#define EV6__ASTER_PPCE__FPE__M 		 0x1 
#define EV6__ASTER_PPCE__RSV1__S 		 0x3 
#define EV6__ASTER_PPCE__RSV1__V 		 0x2 
#define EV6__ASTER_PPCE__RSV1__M 		 0x3 
#define EV6__ASTER_PPCE__ASTER__S 		 0x5 
#define EV6__ASTER_PPCE__ASTER__V 		 0x4 
#define EV6__ASTER_PPCE__ASTER__M 		 0xf 
#define EV6__ASTER_PPCE__ASTRR__S 		 0x9 
#define EV6__ASTER_PPCE__ASTRR__V 		 0x4 
#define EV6__ASTER_PPCE__ASTRR__M 		 0xf 
#define EV6__ASTER_PPCE__RSV2__S 		 0xd 
#define EV6__ASTER_PPCE__RSV2__V 		 0x1a 
#define EV6__ASTER_PPCE__RSV2__M 		 0x3ffffff 
#define EV6__ASTER_PPCE__ASN__S 		 0x27 
#define EV6__ASTER_PPCE__ASN__V 		 0x8 
#define EV6__ASTER_PPCE__ASN__M 		 0xff 
#define EV6__ASTER_PPCE__RSV3__S 		 0x2f 
#define EV6__ASTER_PPCE__RSV3__V 		 0x11 
#define EV6__ASTER_PPCE__RSV3__M 		 0x1ffff 

#define EV6__ASTER_FPE__RSV0__S 		 0x0 
#define EV6__ASTER_FPE__RSV0__V 		 0x1 
#define EV6__ASTER_FPE__RSV0__M 		 0x1 
#define EV6__ASTER_FPE__PPCE__S 		 0x1 
#define EV6__ASTER_FPE__PPCE__V 		 0x1 
#define EV6__ASTER_FPE__PPCE__M 		 0x1 
#define EV6__ASTER_FPE__FPE__S 		 0x2 
#define EV6__ASTER_FPE__FPE__V 		 0x1 
#define EV6__ASTER_FPE__FPE__M 		 0x1 
#define EV6__ASTER_FPE__RSV1__S 		 0x3 
#define EV6__ASTER_FPE__RSV1__V 		 0x2 
#define EV6__ASTER_FPE__RSV1__M 		 0x3 
#define EV6__ASTER_FPE__ASTER__S 		 0x5 
#define EV6__ASTER_FPE__ASTER__V 		 0x4 
#define EV6__ASTER_FPE__ASTER__M 		 0xf 
#define EV6__ASTER_FPE__ASTRR__S 		 0x9 
#define EV6__ASTER_FPE__ASTRR__V 		 0x4 
#define EV6__ASTER_FPE__ASTRR__M 		 0xf 
#define EV6__ASTER_FPE__RSV2__S 		 0xd 
#define EV6__ASTER_FPE__RSV2__V 		 0x1a 
#define EV6__ASTER_FPE__RSV2__M 		 0x3ffffff 
#define EV6__ASTER_FPE__ASN__S 		 0x27 
#define EV6__ASTER_FPE__ASN__V 		 0x8 
#define EV6__ASTER_FPE__ASN__M 		 0xff 
#define EV6__ASTER_FPE__RSV3__S 		 0x2f 
#define EV6__ASTER_FPE__RSV3__V 		 0x11 
#define EV6__ASTER_FPE__RSV3__M 		 0x1ffff 

#define EV6__ASTRR_PPCE__RSV0__S 		 0x0 
#define EV6__ASTRR_PPCE__RSV0__V 		 0x1 
#define EV6__ASTRR_PPCE__RSV0__M 		 0x1 
#define EV6__ASTRR_PPCE__PPCE__S 		 0x1 
#define EV6__ASTRR_PPCE__PPCE__V 		 0x1 
#define EV6__ASTRR_PPCE__PPCE__M 		 0x1 
#define EV6__ASTRR_PPCE__FPE__S 		 0x2 
#define EV6__ASTRR_PPCE__FPE__V 		 0x1 
#define EV6__ASTRR_PPCE__FPE__M 		 0x1 
#define EV6__ASTRR_PPCE__RSV1__S 		 0x3 
#define EV6__ASTRR_PPCE__RSV1__V 		 0x2 
#define EV6__ASTRR_PPCE__RSV1__M 		 0x3 
#define EV6__ASTRR_PPCE__ASTER__S 		 0x5 
#define EV6__ASTRR_PPCE__ASTER__V 		 0x4 
#define EV6__ASTRR_PPCE__ASTER__M 		 0xf 
#define EV6__ASTRR_PPCE__ASTRR__S 		 0x9 
#define EV6__ASTRR_PPCE__ASTRR__V 		 0x4 
#define EV6__ASTRR_PPCE__ASTRR__M 		 0xf 
#define EV6__ASTRR_PPCE__RSV2__S 		 0xd 
#define EV6__ASTRR_PPCE__RSV2__V 		 0x1a 
#define EV6__ASTRR_PPCE__RSV2__M 		 0x3ffffff 
#define EV6__ASTRR_PPCE__ASN__S 		 0x27 
#define EV6__ASTRR_PPCE__ASN__V 		 0x8 
#define EV6__ASTRR_PPCE__ASN__M 		 0xff 
#define EV6__ASTRR_PPCE__RSV3__S 		 0x2f 
#define EV6__ASTRR_PPCE__RSV3__V 		 0x11 
#define EV6__ASTRR_PPCE__RSV3__M 		 0x1ffff 

#define EV6__ASTRR_FPE__RSV0__S 		 0x0 
#define EV6__ASTRR_FPE__RSV0__V 		 0x1 
#define EV6__ASTRR_FPE__RSV0__M 		 0x1 
#define EV6__ASTRR_FPE__PPCE__S 		 0x1 
#define EV6__ASTRR_FPE__PPCE__V 		 0x1 
#define EV6__ASTRR_FPE__PPCE__M 		 0x1 
#define EV6__ASTRR_FPE__FPE__S 		 0x2 
#define EV6__ASTRR_FPE__FPE__V 		 0x1 
#define EV6__ASTRR_FPE__FPE__M 		 0x1 
#define EV6__ASTRR_FPE__RSV1__S 		 0x3 
#define EV6__ASTRR_FPE__RSV1__V 		 0x2 
#define EV6__ASTRR_FPE__RSV1__M 		 0x3 
#define EV6__ASTRR_FPE__ASTER__S 		 0x5 
#define EV6__ASTRR_FPE__ASTER__V 		 0x4 
#define EV6__ASTRR_FPE__ASTER__M 		 0xf 
#define EV6__ASTRR_FPE__ASTRR__S 		 0x9 
#define EV6__ASTRR_FPE__ASTRR__V 		 0x4 
#define EV6__ASTRR_FPE__ASTRR__M 		 0xf 
#define EV6__ASTRR_FPE__RSV2__S 		 0xd 
#define EV6__ASTRR_FPE__RSV2__V 		 0x1a 
#define EV6__ASTRR_FPE__RSV2__M 		 0x3ffffff 
#define EV6__ASTRR_FPE__ASN__S 		 0x27 
#define EV6__ASTRR_FPE__ASN__V 		 0x8 
#define EV6__ASTRR_FPE__ASN__M 		 0xff 
#define EV6__ASTRR_FPE__RSV3__S 		 0x2f 
#define EV6__ASTRR_FPE__RSV3__V 		 0x11 
#define EV6__ASTRR_FPE__RSV3__M 		 0x1ffff 

#define EV6__PPCE_FPE__RSV0__S 		 0x0 
#define EV6__PPCE_FPE__RSV0__V 		 0x1 
#define EV6__PPCE_FPE__RSV0__M 		 0x1 
#define EV6__PPCE_FPE__PPCE__S 		 0x1 
#define EV6__PPCE_FPE__PPCE__V 		 0x1 
#define EV6__PPCE_FPE__PPCE__M 		 0x1 
#define EV6__PPCE_FPE__FPE__S 		 0x2 
#define EV6__PPCE_FPE__FPE__V 		 0x1 
#define EV6__PPCE_FPE__FPE__M 		 0x1 
#define EV6__PPCE_FPE__RSV1__S 		 0x3 
#define EV6__PPCE_FPE__RSV1__V 		 0x2 
#define EV6__PPCE_FPE__RSV1__M 		 0x3 
#define EV6__PPCE_FPE__ASTER__S 		 0x5 
#define EV6__PPCE_FPE__ASTER__V 		 0x4 
#define EV6__PPCE_FPE__ASTER__M 		 0xf 
#define EV6__PPCE_FPE__ASTRR__S 		 0x9 
#define EV6__PPCE_FPE__ASTRR__V 		 0x4 
#define EV6__PPCE_FPE__ASTRR__M 		 0xf 
#define EV6__PPCE_FPE__RSV2__S 		 0xd 
#define EV6__PPCE_FPE__RSV2__V 		 0x1a 
#define EV6__PPCE_FPE__RSV2__M 		 0x3ffffff 
#define EV6__PPCE_FPE__ASN__S 		 0x27 
#define EV6__PPCE_FPE__ASN__V 		 0x8 
#define EV6__PPCE_FPE__ASN__M 		 0xff 
#define EV6__PPCE_FPE__RSV3__S 		 0x2f 
#define EV6__PPCE_FPE__RSV3__V 		 0x11 
#define EV6__PPCE_FPE__RSV3__M 		 0x1ffff 

#define EV6__ASN_ASTER_ASTRR__RSV0__S 		 0x0 
#define EV6__ASN_ASTER_ASTRR__RSV0__V 		 0x1 
#define EV6__ASN_ASTER_ASTRR__RSV0__M 		 0x1 
#define EV6__ASN_ASTER_ASTRR__PPCE__S 		 0x1 
#define EV6__ASN_ASTER_ASTRR__PPCE__V 		 0x1 
#define EV6__ASN_ASTER_ASTRR__PPCE__M 		 0x1 
#define EV6__ASN_ASTER_ASTRR__FPE__S 		 0x2 
#define EV6__ASN_ASTER_ASTRR__FPE__V 		 0x1 
#define EV6__ASN_ASTER_ASTRR__FPE__M 		 0x1 
#define EV6__ASN_ASTER_ASTRR__RSV1__S 		 0x3 
#define EV6__ASN_ASTER_ASTRR__RSV1__V 		 0x2 
#define EV6__ASN_ASTER_ASTRR__RSV1__M 		 0x3 
#define EV6__ASN_ASTER_ASTRR__ASTER__S 		 0x5 
#define EV6__ASN_ASTER_ASTRR__ASTER__V 		 0x4 
#define EV6__ASN_ASTER_ASTRR__ASTER__M 		 0xf 
#define EV6__ASN_ASTER_ASTRR__ASTRR__S 		 0x9 
#define EV6__ASN_ASTER_ASTRR__ASTRR__V 		 0x4 
#define EV6__ASN_ASTER_ASTRR__ASTRR__M 		 0xf 
#define EV6__ASN_ASTER_ASTRR__RSV2__S 		 0xd 
#define EV6__ASN_ASTER_ASTRR__RSV2__V 		 0x1a 
#define EV6__ASN_ASTER_ASTRR__RSV2__M 		 0x3ffffff 
#define EV6__ASN_ASTER_ASTRR__ASN__S 		 0x27 
#define EV6__ASN_ASTER_ASTRR__ASN__V 		 0x8 
#define EV6__ASN_ASTER_ASTRR__ASN__M 		 0xff 
#define EV6__ASN_ASTER_ASTRR__RSV3__S 		 0x2f 
#define EV6__ASN_ASTER_ASTRR__RSV3__V 		 0x11 
#define EV6__ASN_ASTER_ASTRR__RSV3__M 		 0x1ffff 

#define EV6__ASN_ASTER_PPCE__RSV0__S 		 0x0 
#define EV6__ASN_ASTER_PPCE__RSV0__V 		 0x1 
#define EV6__ASN_ASTER_PPCE__RSV0__M 		 0x1 
#define EV6__ASN_ASTER_PPCE__PPCE__S 		 0x1 
#define EV6__ASN_ASTER_PPCE__PPCE__V 		 0x1 
#define EV6__ASN_ASTER_PPCE__PPCE__M 		 0x1 
#define EV6__ASN_ASTER_PPCE__FPE__S 		 0x2 
#define EV6__ASN_ASTER_PPCE__FPE__V 		 0x1 
#define EV6__ASN_ASTER_PPCE__FPE__M 		 0x1 
#define EV6__ASN_ASTER_PPCE__RSV1__S 		 0x3 
#define EV6__ASN_ASTER_PPCE__RSV1__V 		 0x2 
#define EV6__ASN_ASTER_PPCE__RSV1__M 		 0x3 
#define EV6__ASN_ASTER_PPCE__ASTER__S 		 0x5 
#define EV6__ASN_ASTER_PPCE__ASTER__V 		 0x4 
#define EV6__ASN_ASTER_PPCE__ASTER__M 		 0xf 
#define EV6__ASN_ASTER_PPCE__ASTRR__S 		 0x9 
#define EV6__ASN_ASTER_PPCE__ASTRR__V 		 0x4 
#define EV6__ASN_ASTER_PPCE__ASTRR__M 		 0xf 
#define EV6__ASN_ASTER_PPCE__RSV2__S 		 0xd 
#define EV6__ASN_ASTER_PPCE__RSV2__V 		 0x1a 
#define EV6__ASN_ASTER_PPCE__RSV2__M 		 0x3ffffff 
#define EV6__ASN_ASTER_PPCE__ASN__S 		 0x27 
#define EV6__ASN_ASTER_PPCE__ASN__V 		 0x8 
#define EV6__ASN_ASTER_PPCE__ASN__M 		 0xff 
#define EV6__ASN_ASTER_PPCE__RSV3__S 		 0x2f 
#define EV6__ASN_ASTER_PPCE__RSV3__V 		 0x11 
#define EV6__ASN_ASTER_PPCE__RSV3__M 		 0x1ffff 

#define EV6__ASN_ASTER_FPE__RSV0__S 		 0x0 
#define EV6__ASN_ASTER_FPE__RSV0__V 		 0x1 
#define EV6__ASN_ASTER_FPE__RSV0__M 		 0x1 
#define EV6__ASN_ASTER_FPE__PPCE__S 		 0x1 
#define EV6__ASN_ASTER_FPE__PPCE__V 		 0x1 
#define EV6__ASN_ASTER_FPE__PPCE__M 		 0x1 
#define EV6__ASN_ASTER_FPE__FPE__S 		 0x2 
#define EV6__ASN_ASTER_FPE__FPE__V 		 0x1 
#define EV6__ASN_ASTER_FPE__FPE__M 		 0x1 
#define EV6__ASN_ASTER_FPE__RSV1__S 		 0x3 
#define EV6__ASN_ASTER_FPE__RSV1__V 		 0x2 
#define EV6__ASN_ASTER_FPE__RSV1__M 		 0x3 
#define EV6__ASN_ASTER_FPE__ASTER__S 		 0x5 
#define EV6__ASN_ASTER_FPE__ASTER__V 		 0x4 
#define EV6__ASN_ASTER_FPE__ASTER__M 		 0xf 
#define EV6__ASN_ASTER_FPE__ASTRR__S 		 0x9 
#define EV6__ASN_ASTER_FPE__ASTRR__V 		 0x4 
#define EV6__ASN_ASTER_FPE__ASTRR__M 		 0xf 
#define EV6__ASN_ASTER_FPE__RSV2__S 		 0xd 
#define EV6__ASN_ASTER_FPE__RSV2__V 		 0x1a 
#define EV6__ASN_ASTER_FPE__RSV2__M 		 0x3ffffff 
#define EV6__ASN_ASTER_FPE__ASN__S 		 0x27 
#define EV6__ASN_ASTER_FPE__ASN__V 		 0x8 
#define EV6__ASN_ASTER_FPE__ASN__M 		 0xff 
#define EV6__ASN_ASTER_FPE__RSV3__S 		 0x2f 
#define EV6__ASN_ASTER_FPE__RSV3__V 		 0x11 
#define EV6__ASN_ASTER_FPE__RSV3__M 		 0x1ffff 

#define EV6__ASN_ASTRR_PPCE__RSV0__S 		 0x0 
#define EV6__ASN_ASTRR_PPCE__RSV0__V 		 0x1 
#define EV6__ASN_ASTRR_PPCE__RSV0__M 		 0x1 
#define EV6__ASN_ASTRR_PPCE__PPCE__S 		 0x1 
#define EV6__ASN_ASTRR_PPCE__PPCE__V 		 0x1 
#define EV6__ASN_ASTRR_PPCE__PPCE__M 		 0x1 
#define EV6__ASN_ASTRR_PPCE__FPE__S 		 0x2 
#define EV6__ASN_ASTRR_PPCE__FPE__V 		 0x1 
#define EV6__ASN_ASTRR_PPCE__FPE__M 		 0x1 
#define EV6__ASN_ASTRR_PPCE__RSV1__S 		 0x3 
#define EV6__ASN_ASTRR_PPCE__RSV1__V 		 0x2 
#define EV6__ASN_ASTRR_PPCE__RSV1__M 		 0x3 
#define EV6__ASN_ASTRR_PPCE__ASTER__S 		 0x5 
#define EV6__ASN_ASTRR_PPCE__ASTER__V 		 0x4 
#define EV6__ASN_ASTRR_PPCE__ASTER__M 		 0xf 
#define EV6__ASN_ASTRR_PPCE__ASTRR__S 		 0x9 
#define EV6__ASN_ASTRR_PPCE__ASTRR__V 		 0x4 
#define EV6__ASN_ASTRR_PPCE__ASTRR__M 		 0xf 
#define EV6__ASN_ASTRR_PPCE__RSV2__S 		 0xd 
#define EV6__ASN_ASTRR_PPCE__RSV2__V 		 0x1a 
#define EV6__ASN_ASTRR_PPCE__RSV2__M 		 0x3ffffff 
#define EV6__ASN_ASTRR_PPCE__ASN__S 		 0x27 
#define EV6__ASN_ASTRR_PPCE__ASN__V 		 0x8 
#define EV6__ASN_ASTRR_PPCE__ASN__M 		 0xff 
#define EV6__ASN_ASTRR_PPCE__RSV3__S 		 0x2f 
#define EV6__ASN_ASTRR_PPCE__RSV3__V 		 0x11 
#define EV6__ASN_ASTRR_PPCE__RSV3__M 		 0x1ffff 

#define EV6__ASN_ASTRR_FPE__RSV0__S 		 0x0 
#define EV6__ASN_ASTRR_FPE__RSV0__V 		 0x1 
#define EV6__ASN_ASTRR_FPE__RSV0__M 		 0x1 
#define EV6__ASN_ASTRR_FPE__PPCE__S 		 0x1 
#define EV6__ASN_ASTRR_FPE__PPCE__V 		 0x1 
#define EV6__ASN_ASTRR_FPE__PPCE__M 		 0x1 
#define EV6__ASN_ASTRR_FPE__FPE__S 		 0x2 
#define EV6__ASN_ASTRR_FPE__FPE__V 		 0x1 
#define EV6__ASN_ASTRR_FPE__FPE__M 		 0x1 
#define EV6__ASN_ASTRR_FPE__RSV1__S 		 0x3 
#define EV6__ASN_ASTRR_FPE__RSV1__V 		 0x2 
#define EV6__ASN_ASTRR_FPE__RSV1__M 		 0x3 
#define EV6__ASN_ASTRR_FPE__ASTER__S 		 0x5 
#define EV6__ASN_ASTRR_FPE__ASTER__V 		 0x4 
#define EV6__ASN_ASTRR_FPE__ASTER__M 		 0xf 
#define EV6__ASN_ASTRR_FPE__ASTRR__S 		 0x9 
#define EV6__ASN_ASTRR_FPE__ASTRR__V 		 0x4 
#define EV6__ASN_ASTRR_FPE__ASTRR__M 		 0xf 
#define EV6__ASN_ASTRR_FPE__RSV2__S 		 0xd 
#define EV6__ASN_ASTRR_FPE__RSV2__V 		 0x1a 
#define EV6__ASN_ASTRR_FPE__RSV2__M 		 0x3ffffff 
#define EV6__ASN_ASTRR_FPE__ASN__S 		 0x27 
#define EV6__ASN_ASTRR_FPE__ASN__V 		 0x8 
#define EV6__ASN_ASTRR_FPE__ASN__M 		 0xff 
#define EV6__ASN_ASTRR_FPE__RSV3__S 		 0x2f 
#define EV6__ASN_ASTRR_FPE__RSV3__V 		 0x11 
#define EV6__ASN_ASTRR_FPE__RSV3__M 		 0x1ffff 

#define EV6__ASN_PPCE_FPE__RSV0__S 		 0x0 
#define EV6__ASN_PPCE_FPE__RSV0__V 		 0x1 
#define EV6__ASN_PPCE_FPE__RSV0__M 		 0x1 
#define EV6__ASN_PPCE_FPE__PPCE__S 		 0x1 
#define EV6__ASN_PPCE_FPE__PPCE__V 		 0x1 
#define EV6__ASN_PPCE_FPE__PPCE__M 		 0x1 
#define EV6__ASN_PPCE_FPE__FPE__S 		 0x2 
#define EV6__ASN_PPCE_FPE__FPE__V 		 0x1 
#define EV6__ASN_PPCE_FPE__FPE__M 		 0x1 
#define EV6__ASN_PPCE_FPE__RSV1__S 		 0x3 
#define EV6__ASN_PPCE_FPE__RSV1__V 		 0x2 
#define EV6__ASN_PPCE_FPE__RSV1__M 		 0x3 
#define EV6__ASN_PPCE_FPE__ASTER__S 		 0x5 
#define EV6__ASN_PPCE_FPE__ASTER__V 		 0x4 
#define EV6__ASN_PPCE_FPE__ASTER__M 		 0xf 
#define EV6__ASN_PPCE_FPE__ASTRR__S 		 0x9 
#define EV6__ASN_PPCE_FPE__ASTRR__V 		 0x4 
#define EV6__ASN_PPCE_FPE__ASTRR__M 		 0xf 
#define EV6__ASN_PPCE_FPE__RSV2__S 		 0xd 
#define EV6__ASN_PPCE_FPE__RSV2__V 		 0x1a 
#define EV6__ASN_PPCE_FPE__RSV2__M 		 0x3ffffff 
#define EV6__ASN_PPCE_FPE__ASN__S 		 0x27 
#define EV6__ASN_PPCE_FPE__ASN__V 		 0x8 
#define EV6__ASN_PPCE_FPE__ASN__M 		 0xff 
#define EV6__ASN_PPCE_FPE__RSV3__S 		 0x2f 
#define EV6__ASN_PPCE_FPE__RSV3__V 		 0x11 
#define EV6__ASN_PPCE_FPE__RSV3__M 		 0x1ffff 

#define EV6__ASTER_ASTRR_PPCE__RSV0__S 		 0x0 
#define EV6__ASTER_ASTRR_PPCE__RSV0__V 		 0x1 
#define EV6__ASTER_ASTRR_PPCE__RSV0__M 		 0x1 
#define EV6__ASTER_ASTRR_PPCE__PPCE__S 		 0x1 
#define EV6__ASTER_ASTRR_PPCE__PPCE__V 		 0x1 
#define EV6__ASTER_ASTRR_PPCE__PPCE__M 		 0x1 
#define EV6__ASTER_ASTRR_PPCE__FPE__S 		 0x2 
#define EV6__ASTER_ASTRR_PPCE__FPE__V 		 0x1 
#define EV6__ASTER_ASTRR_PPCE__FPE__M 		 0x1 
#define EV6__ASTER_ASTRR_PPCE__RSV1__S 		 0x3 
#define EV6__ASTER_ASTRR_PPCE__RSV1__V 		 0x2 
#define EV6__ASTER_ASTRR_PPCE__RSV1__M 		 0x3 
#define EV6__ASTER_ASTRR_PPCE__ASTER__S 		 0x5 
#define EV6__ASTER_ASTRR_PPCE__ASTER__V 		 0x4 
#define EV6__ASTER_ASTRR_PPCE__ASTER__M 		 0xf 
#define EV6__ASTER_ASTRR_PPCE__ASTRR__S 		 0x9 
#define EV6__ASTER_ASTRR_PPCE__ASTRR__V 		 0x4 
#define EV6__ASTER_ASTRR_PPCE__ASTRR__M 		 0xf 
#define EV6__ASTER_ASTRR_PPCE__RSV2__S 		 0xd 
#define EV6__ASTER_ASTRR_PPCE__RSV2__V 		 0x1a 
#define EV6__ASTER_ASTRR_PPCE__RSV2__M 		 0x3ffffff 
#define EV6__ASTER_ASTRR_PPCE__ASN__S 		 0x27 
#define EV6__ASTER_ASTRR_PPCE__ASN__V 		 0x8 
#define EV6__ASTER_ASTRR_PPCE__ASN__M 		 0xff 
#define EV6__ASTER_ASTRR_PPCE__RSV3__S 		 0x2f 
#define EV6__ASTER_ASTRR_PPCE__RSV3__V 		 0x11 
#define EV6__ASTER_ASTRR_PPCE__RSV3__M 		 0x1ffff 

#define EV6__ASTER_ASTRR_FPE__RSV0__S 		 0x0 
#define EV6__ASTER_ASTRR_FPE__RSV0__V 		 0x1 
#define EV6__ASTER_ASTRR_FPE__RSV0__M 		 0x1 
#define EV6__ASTER_ASTRR_FPE__PPCE__S 		 0x1 
#define EV6__ASTER_ASTRR_FPE__PPCE__V 		 0x1 
#define EV6__ASTER_ASTRR_FPE__PPCE__M 		 0x1 
#define EV6__ASTER_ASTRR_FPE__FPE__S 		 0x2 
#define EV6__ASTER_ASTRR_FPE__FPE__V 		 0x1 
#define EV6__ASTER_ASTRR_FPE__FPE__M 		 0x1 
#define EV6__ASTER_ASTRR_FPE__RSV1__S 		 0x3 
#define EV6__ASTER_ASTRR_FPE__RSV1__V 		 0x2 
#define EV6__ASTER_ASTRR_FPE__RSV1__M 		 0x3 
#define EV6__ASTER_ASTRR_FPE__ASTER__S 		 0x5 
#define EV6__ASTER_ASTRR_FPE__ASTER__V 		 0x4 
#define EV6__ASTER_ASTRR_FPE__ASTER__M 		 0xf 
#define EV6__ASTER_ASTRR_FPE__ASTRR__S 		 0x9 
#define EV6__ASTER_ASTRR_FPE__ASTRR__V 		 0x4 
#define EV6__ASTER_ASTRR_FPE__ASTRR__M 		 0xf 
#define EV6__ASTER_ASTRR_FPE__RSV2__S 		 0xd 
#define EV6__ASTER_ASTRR_FPE__RSV2__V 		 0x1a 
#define EV6__ASTER_ASTRR_FPE__RSV2__M 		 0x3ffffff 
#define EV6__ASTER_ASTRR_FPE__ASN__S 		 0x27 
#define EV6__ASTER_ASTRR_FPE__ASN__V 		 0x8 
#define EV6__ASTER_ASTRR_FPE__ASN__M 		 0xff 
#define EV6__ASTER_ASTRR_FPE__RSV3__S 		 0x2f 
#define EV6__ASTER_ASTRR_FPE__RSV3__V 		 0x11 
#define EV6__ASTER_ASTRR_FPE__RSV3__M 		 0x1ffff 

#define EV6__ASTER_PPCE_FPE__RSV0__S 		 0x0 
#define EV6__ASTER_PPCE_FPE__RSV0__V 		 0x1 
#define EV6__ASTER_PPCE_FPE__RSV0__M 		 0x1 
#define EV6__ASTER_PPCE_FPE__PPCE__S 		 0x1 
#define EV6__ASTER_PPCE_FPE__PPCE__V 		 0x1 
#define EV6__ASTER_PPCE_FPE__PPCE__M 		 0x1 
#define EV6__ASTER_PPCE_FPE__FPE__S 		 0x2 
#define EV6__ASTER_PPCE_FPE__FPE__V 		 0x1 
#define EV6__ASTER_PPCE_FPE__FPE__M 		 0x1 
#define EV6__ASTER_PPCE_FPE__RSV1__S 		 0x3 
#define EV6__ASTER_PPCE_FPE__RSV1__V 		 0x2 
#define EV6__ASTER_PPCE_FPE__RSV1__M 		 0x3 
#define EV6__ASTER_PPCE_FPE__ASTER__S 		 0x5 
#define EV6__ASTER_PPCE_FPE__ASTER__V 		 0x4 
#define EV6__ASTER_PPCE_FPE__ASTER__M 		 0xf 
#define EV6__ASTER_PPCE_FPE__ASTRR__S 		 0x9 
#define EV6__ASTER_PPCE_FPE__ASTRR__V 		 0x4 
#define EV6__ASTER_PPCE_FPE__ASTRR__M 		 0xf 
#define EV6__ASTER_PPCE_FPE__RSV2__S 		 0xd 
#define EV6__ASTER_PPCE_FPE__RSV2__V 		 0x1a 
#define EV6__ASTER_PPCE_FPE__RSV2__M 		 0x3ffffff 
#define EV6__ASTER_PPCE_FPE__ASN__S 		 0x27 
#define EV6__ASTER_PPCE_FPE__ASN__V 		 0x8 
#define EV6__ASTER_PPCE_FPE__ASN__M 		 0xff 
#define EV6__ASTER_PPCE_FPE__RSV3__S 		 0x2f 
#define EV6__ASTER_PPCE_FPE__RSV3__V 		 0x11 
#define EV6__ASTER_PPCE_FPE__RSV3__M 		 0x1ffff 

#define EV6__ASTRR_PPCE_FPE__RSV0__S 		 0x0 
#define EV6__ASTRR_PPCE_FPE__RSV0__V 		 0x1 
#define EV6__ASTRR_PPCE_FPE__RSV0__M 		 0x1 
#define EV6__ASTRR_PPCE_FPE__PPCE__S 		 0x1 
#define EV6__ASTRR_PPCE_FPE__PPCE__V 		 0x1 
#define EV6__ASTRR_PPCE_FPE__PPCE__M 		 0x1 
#define EV6__ASTRR_PPCE_FPE__FPE__S 		 0x2 
#define EV6__ASTRR_PPCE_FPE__FPE__V 		 0x1 
#define EV6__ASTRR_PPCE_FPE__FPE__M 		 0x1 
#define EV6__ASTRR_PPCE_FPE__RSV1__S 		 0x3 
#define EV6__ASTRR_PPCE_FPE__RSV1__V 		 0x2 
#define EV6__ASTRR_PPCE_FPE__RSV1__M 		 0x3 
#define EV6__ASTRR_PPCE_FPE__ASTER__S 		 0x5 
#define EV6__ASTRR_PPCE_FPE__ASTER__V 		 0x4 
#define EV6__ASTRR_PPCE_FPE__ASTER__M 		 0xf 
#define EV6__ASTRR_PPCE_FPE__ASTRR__S 		 0x9 
#define EV6__ASTRR_PPCE_FPE__ASTRR__V 		 0x4 
#define EV6__ASTRR_PPCE_FPE__ASTRR__M 		 0xf 
#define EV6__ASTRR_PPCE_FPE__RSV2__S 		 0xd 
#define EV6__ASTRR_PPCE_FPE__RSV2__V 		 0x1a 
#define EV6__ASTRR_PPCE_FPE__RSV2__M 		 0x3ffffff 
#define EV6__ASTRR_PPCE_FPE__ASN__S 		 0x27 
#define EV6__ASTRR_PPCE_FPE__ASN__V 		 0x8 
#define EV6__ASTRR_PPCE_FPE__ASN__M 		 0xff 
#define EV6__ASTRR_PPCE_FPE__RSV3__S 		 0x2f 
#define EV6__ASTRR_PPCE_FPE__RSV3__V 		 0x11 
#define EV6__ASTRR_PPCE_FPE__RSV3__M 		 0x1ffff 

#define EV6__ASN_ASTER_ASTRR_PPCE__RSV0__S 		 0x0 
#define EV6__ASN_ASTER_ASTRR_PPCE__RSV0__V 		 0x1 
#define EV6__ASN_ASTER_ASTRR_PPCE__RSV0__M 		 0x1 
#define EV6__ASN_ASTER_ASTRR_PPCE__PPCE__S 		 0x1 
#define EV6__ASN_ASTER_ASTRR_PPCE__PPCE__V 		 0x1 
#define EV6__ASN_ASTER_ASTRR_PPCE__PPCE__M 		 0x1 
#define EV6__ASN_ASTER_ASTRR_PPCE__FPE__S 		 0x2 
#define EV6__ASN_ASTER_ASTRR_PPCE__FPE__V 		 0x1 
#define EV6__ASN_ASTER_ASTRR_PPCE__FPE__M 		 0x1 
#define EV6__ASN_ASTER_ASTRR_PPCE__RSV1__S 		 0x3 
#define EV6__ASN_ASTER_ASTRR_PPCE__RSV1__V 		 0x2 
#define EV6__ASN_ASTER_ASTRR_PPCE__RSV1__M 		 0x3 
#define EV6__ASN_ASTER_ASTRR_PPCE__ASTER__S 		 0x5 
#define EV6__ASN_ASTER_ASTRR_PPCE__ASTER__V 		 0x4 
#define EV6__ASN_ASTER_ASTRR_PPCE__ASTER__M 		 0xf 
#define EV6__ASN_ASTER_ASTRR_PPCE__ASTRR__S 		 0x9 
#define EV6__ASN_ASTER_ASTRR_PPCE__ASTRR__V 		 0x4 
#define EV6__ASN_ASTER_ASTRR_PPCE__ASTRR__M 		 0xf 
#define EV6__ASN_ASTER_ASTRR_PPCE__RSV2__S 		 0xd 
#define EV6__ASN_ASTER_ASTRR_PPCE__RSV2__V 		 0x1a 
#define EV6__ASN_ASTER_ASTRR_PPCE__RSV2__M 		 0x3ffffff 
#define EV6__ASN_ASTER_ASTRR_PPCE__ASN__S 		 0x27 
#define EV6__ASN_ASTER_ASTRR_PPCE__ASN__V 		 0x8 
#define EV6__ASN_ASTER_ASTRR_PPCE__ASN__M 		 0xff 
#define EV6__ASN_ASTER_ASTRR_PPCE__RSV3__S 		 0x2f 
#define EV6__ASN_ASTER_ASTRR_PPCE__RSV3__V 		 0x11 
#define EV6__ASN_ASTER_ASTRR_PPCE__RSV3__M 		 0x1ffff 

#define EV6__ASN_ASTER_ASTRR_FPE__RSV0__S 		 0x0 
#define EV6__ASN_ASTER_ASTRR_FPE__RSV0__V 		 0x1 
#define EV6__ASN_ASTER_ASTRR_FPE__RSV0__M 		 0x1 
#define EV6__ASN_ASTER_ASTRR_FPE__PPCE__S 		 0x1 
#define EV6__ASN_ASTER_ASTRR_FPE__PPCE__V 		 0x1 
#define EV6__ASN_ASTER_ASTRR_FPE__PPCE__M 		 0x1 
#define EV6__ASN_ASTER_ASTRR_FPE__FPE__S 		 0x2 
#define EV6__ASN_ASTER_ASTRR_FPE__FPE__V 		 0x1 
#define EV6__ASN_ASTER_ASTRR_FPE__FPE__M 		 0x1 
#define EV6__ASN_ASTER_ASTRR_FPE__RSV1__S 		 0x3 
#define EV6__ASN_ASTER_ASTRR_FPE__RSV1__V 		 0x2 
#define EV6__ASN_ASTER_ASTRR_FPE__RSV1__M 		 0x3 
#define EV6__ASN_ASTER_ASTRR_FPE__ASTER__S 		 0x5 
#define EV6__ASN_ASTER_ASTRR_FPE__ASTER__V 		 0x4 
#define EV6__ASN_ASTER_ASTRR_FPE__ASTER__M 		 0xf 
#define EV6__ASN_ASTER_ASTRR_FPE__ASTRR__S 		 0x9 
#define EV6__ASN_ASTER_ASTRR_FPE__ASTRR__V 		 0x4 
#define EV6__ASN_ASTER_ASTRR_FPE__ASTRR__M 		 0xf 
#define EV6__ASN_ASTER_ASTRR_FPE__RSV2__S 		 0xd 
#define EV6__ASN_ASTER_ASTRR_FPE__RSV2__V 		 0x1a 
#define EV6__ASN_ASTER_ASTRR_FPE__RSV2__M 		 0x3ffffff 
#define EV6__ASN_ASTER_ASTRR_FPE__ASN__S 		 0x27 
#define EV6__ASN_ASTER_ASTRR_FPE__ASN__V 		 0x8 
#define EV6__ASN_ASTER_ASTRR_FPE__ASN__M 		 0xff 
#define EV6__ASN_ASTER_ASTRR_FPE__RSV3__S 		 0x2f 
#define EV6__ASN_ASTER_ASTRR_FPE__RSV3__V 		 0x11 
#define EV6__ASN_ASTER_ASTRR_FPE__RSV3__M 		 0x1ffff 

#define EV6__ASN_ASTER_PPCE_FPE__RSV0__S 		 0x0 
#define EV6__ASN_ASTER_PPCE_FPE__RSV0__V 		 0x1 
#define EV6__ASN_ASTER_PPCE_FPE__RSV0__M 		 0x1 
#define EV6__ASN_ASTER_PPCE_FPE__PPCE__S 		 0x1 
#define EV6__ASN_ASTER_PPCE_FPE__PPCE__V 		 0x1 
#define EV6__ASN_ASTER_PPCE_FPE__PPCE__M 		 0x1 
#define EV6__ASN_ASTER_PPCE_FPE__FPE__S 		 0x2 
#define EV6__ASN_ASTER_PPCE_FPE__FPE__V 		 0x1 
#define EV6__ASN_ASTER_PPCE_FPE__FPE__M 		 0x1 
#define EV6__ASN_ASTER_PPCE_FPE__RSV1__S 		 0x3 
#define EV6__ASN_ASTER_PPCE_FPE__RSV1__V 		 0x2 
#define EV6__ASN_ASTER_PPCE_FPE__RSV1__M 		 0x3 
#define EV6__ASN_ASTER_PPCE_FPE__ASTER__S 		 0x5 
#define EV6__ASN_ASTER_PPCE_FPE__ASTER__V 		 0x4 
#define EV6__ASN_ASTER_PPCE_FPE__ASTER__M 		 0xf 
#define EV6__ASN_ASTER_PPCE_FPE__ASTRR__S 		 0x9 
#define EV6__ASN_ASTER_PPCE_FPE__ASTRR__V 		 0x4 
#define EV6__ASN_ASTER_PPCE_FPE__ASTRR__M 		 0xf 
#define EV6__ASN_ASTER_PPCE_FPE__RSV2__S 		 0xd 
#define EV6__ASN_ASTER_PPCE_FPE__RSV2__V 		 0x1a 
#define EV6__ASN_ASTER_PPCE_FPE__RSV2__M 		 0x3ffffff 
#define EV6__ASN_ASTER_PPCE_FPE__ASN__S 		 0x27 
#define EV6__ASN_ASTER_PPCE_FPE__ASN__V 		 0x8 
#define EV6__ASN_ASTER_PPCE_FPE__ASN__M 		 0xff 
#define EV6__ASN_ASTER_PPCE_FPE__RSV3__S 		 0x2f 
#define EV6__ASN_ASTER_PPCE_FPE__RSV3__V 		 0x11 
#define EV6__ASN_ASTER_PPCE_FPE__RSV3__M 		 0x1ffff 

#define EV6__ASN_ASTRR_PPCE_FPE__RSV0__S 		 0x0 
#define EV6__ASN_ASTRR_PPCE_FPE__RSV0__V 		 0x1 
#define EV6__ASN_ASTRR_PPCE_FPE__RSV0__M 		 0x1 
#define EV6__ASN_ASTRR_PPCE_FPE__PPCE__S 		 0x1 
#define EV6__ASN_ASTRR_PPCE_FPE__PPCE__V 		 0x1 
#define EV6__ASN_ASTRR_PPCE_FPE__PPCE__M 		 0x1 
#define EV6__ASN_ASTRR_PPCE_FPE__FPE__S 		 0x2 
#define EV6__ASN_ASTRR_PPCE_FPE__FPE__V 		 0x1 
#define EV6__ASN_ASTRR_PPCE_FPE__FPE__M 		 0x1 
#define EV6__ASN_ASTRR_PPCE_FPE__RSV1__S 		 0x3 
#define EV6__ASN_ASTRR_PPCE_FPE__RSV1__V 		 0x2 
#define EV6__ASN_ASTRR_PPCE_FPE__RSV1__M 		 0x3 
#define EV6__ASN_ASTRR_PPCE_FPE__ASTER__S 		 0x5 
#define EV6__ASN_ASTRR_PPCE_FPE__ASTER__V 		 0x4 
#define EV6__ASN_ASTRR_PPCE_FPE__ASTER__M 		 0xf 
#define EV6__ASN_ASTRR_PPCE_FPE__ASTRR__S 		 0x9 
#define EV6__ASN_ASTRR_PPCE_FPE__ASTRR__V 		 0x4 
#define EV6__ASN_ASTRR_PPCE_FPE__ASTRR__M 		 0xf 
#define EV6__ASN_ASTRR_PPCE_FPE__RSV2__S 		 0xd 
#define EV6__ASN_ASTRR_PPCE_FPE__RSV2__V 		 0x1a 
#define EV6__ASN_ASTRR_PPCE_FPE__RSV2__M 		 0x3ffffff 
#define EV6__ASN_ASTRR_PPCE_FPE__ASN__S 		 0x27 
#define EV6__ASN_ASTRR_PPCE_FPE__ASN__V 		 0x8 
#define EV6__ASN_ASTRR_PPCE_FPE__ASN__M 		 0xff 
#define EV6__ASN_ASTRR_PPCE_FPE__RSV3__S 		 0x2f 
#define EV6__ASN_ASTRR_PPCE_FPE__RSV3__V 		 0x11 
#define EV6__ASN_ASTRR_PPCE_FPE__RSV3__M 		 0x1ffff 

#define EV6__ASTER_ASTRR_PPCE_FPE__RSV0__S 		 0x0 
#define EV6__ASTER_ASTRR_PPCE_FPE__RSV0__V 		 0x1 
#define EV6__ASTER_ASTRR_PPCE_FPE__RSV0__M 		 0x1 
#define EV6__ASTER_ASTRR_PPCE_FPE__PPCE__S 		 0x1 
#define EV6__ASTER_ASTRR_PPCE_FPE__PPCE__V 		 0x1 
#define EV6__ASTER_ASTRR_PPCE_FPE__PPCE__M 		 0x1 
#define EV6__ASTER_ASTRR_PPCE_FPE__FPE__S 		 0x2 
#define EV6__ASTER_ASTRR_PPCE_FPE__FPE__V 		 0x1 
#define EV6__ASTER_ASTRR_PPCE_FPE__FPE__M 		 0x1 
#define EV6__ASTER_ASTRR_PPCE_FPE__RSV1__S 		 0x3 
#define EV6__ASTER_ASTRR_PPCE_FPE__RSV1__V 		 0x2 
#define EV6__ASTER_ASTRR_PPCE_FPE__RSV1__M 		 0x3 
#define EV6__ASTER_ASTRR_PPCE_FPE__ASTER__S 		 0x5 
#define EV6__ASTER_ASTRR_PPCE_FPE__ASTER__V 		 0x4 
#define EV6__ASTER_ASTRR_PPCE_FPE__ASTER__M 		 0xf 
#define EV6__ASTER_ASTRR_PPCE_FPE__ASTRR__S 		 0x9 
#define EV6__ASTER_ASTRR_PPCE_FPE__ASTRR__V 		 0x4 
#define EV6__ASTER_ASTRR_PPCE_FPE__ASTRR__M 		 0xf 
#define EV6__ASTER_ASTRR_PPCE_FPE__RSV2__S 		 0xd 
#define EV6__ASTER_ASTRR_PPCE_FPE__RSV2__V 		 0x1a 
#define EV6__ASTER_ASTRR_PPCE_FPE__RSV2__M 		 0x3ffffff 
#define EV6__ASTER_ASTRR_PPCE_FPE__ASN__S 		 0x27 
#define EV6__ASTER_ASTRR_PPCE_FPE__ASN__V 		 0x8 
#define EV6__ASTER_ASTRR_PPCE_FPE__ASN__M 		 0xff 
#define EV6__ASTER_ASTRR_PPCE_FPE__RSV3__S 		 0x2f 
#define EV6__ASTER_ASTRR_PPCE_FPE__RSV3__V 		 0x11 
#define EV6__ASTER_ASTRR_PPCE_FPE__RSV3__M 		 0x1ffff 

#define EV6__PROCESS_CONTEXT__RSV0__S 		 0x0 
#define EV6__PROCESS_CONTEXT__RSV0__V 		 0x1 
#define EV6__PROCESS_CONTEXT__RSV0__M 		 0x1 
#define EV6__PROCESS_CONTEXT__PPCE__S 		 0x1 
#define EV6__PROCESS_CONTEXT__PPCE__V 		 0x1 
#define EV6__PROCESS_CONTEXT__PPCE__M 		 0x1 
#define EV6__PROCESS_CONTEXT__FPE__S 		 0x2 
#define EV6__PROCESS_CONTEXT__FPE__V 		 0x1 
#define EV6__PROCESS_CONTEXT__FPE__M 		 0x1 
#define EV6__PROCESS_CONTEXT__RSV1__S 		 0x3 
#define EV6__PROCESS_CONTEXT__RSV1__V 		 0x2 
#define EV6__PROCESS_CONTEXT__RSV1__M 		 0x3 
#define EV6__PROCESS_CONTEXT__ASTER__S 		 0x5 
#define EV6__PROCESS_CONTEXT__ASTER__V 		 0x4 
#define EV6__PROCESS_CONTEXT__ASTER__M 		 0xf 
#define EV6__PROCESS_CONTEXT__ASTRR__S 		 0x9 
#define EV6__PROCESS_CONTEXT__ASTRR__V 		 0x4 
#define EV6__PROCESS_CONTEXT__ASTRR__M 		 0xf 
#define EV6__PROCESS_CONTEXT__RSV2__S 		 0xd 
#define EV6__PROCESS_CONTEXT__RSV2__V 		 0x1a 
#define EV6__PROCESS_CONTEXT__RSV2__M 		 0x3ffffff 
#define EV6__PROCESS_CONTEXT__ASN__S 		 0x27 
#define EV6__PROCESS_CONTEXT__ASN__V 		 0x8 
#define EV6__PROCESS_CONTEXT__ASN__M 		 0xff 
#define EV6__PROCESS_CONTEXT__RSV3__S 		 0x2f 
#define EV6__PROCESS_CONTEXT__RSV3__V 		 0x11 
#define EV6__PROCESS_CONTEXT__RSV3__M 		 0x1ffff 

#define EV6__DTB_TAG0__RSV1__S 		 0x0 
#define EV6__DTB_TAG0__RSV1__V 		 0xd 
#define EV6__DTB_TAG0__RSV1__M 		 0x1fff 
#define EV6__DTB_TAG0__VA__S 		 0xd 
#define EV6__DTB_TAG0__VA__V 		 0x23 
#define EV6__DTB_TAG0__VA__M 		 0x7ffffffff 
#define EV6__DTB_TAG0__RSV2__S 		 0x30 
#define EV6__DTB_TAG0__RSV2__V 		 0x10 
#define EV6__DTB_TAG0__RSV2__M 		 0xffff 

#define EV6__DTB_TAG1__RSV1__S 		 0x0 
#define EV6__DTB_TAG1__RSV1__V 		 0xd 
#define EV6__DTB_TAG1__RSV1__M 		 0x1fff 
#define EV6__DTB_TAG1__VA__S 		 0xd 
#define EV6__DTB_TAG1__VA__V 		 0x23 
#define EV6__DTB_TAG1__VA__M 		 0x7ffffffff 
#define EV6__DTB_TAG1__RSV2__S 		 0x30 
#define EV6__DTB_TAG1__RSV2__V 		 0x10 
#define EV6__DTB_TAG1__RSV2__M 		 0xffff 

#define EV6__DTB_PTE0__RSV1__S 		 0x0 
#define EV6__DTB_PTE0__RSV1__V 		 0x1 
#define EV6__DTB_PTE0__RSV1__M 		 0x1 
#define EV6__DTB_PTE0__FOR__S 		 0x1 
#define EV6__DTB_PTE0__FOR__V 		 0x1 
#define EV6__DTB_PTE0__FOR__M 		 0x1 
#define EV6__DTB_PTE0__FOW__S 		 0x2 
#define EV6__DTB_PTE0__FOW__V 		 0x1 
#define EV6__DTB_PTE0__FOW__M 		 0x1 
#define EV6__DTB_PTE0__RSV2__S 		 0x3 
#define EV6__DTB_PTE0__RSV2__V 		 0x1 
#define EV6__DTB_PTE0__RSV2__M 		 0x1 
#define EV6__DTB_PTE0__ASM__S 		 0x4 
#define EV6__DTB_PTE0__ASM__V 		 0x1 
#define EV6__DTB_PTE0__ASM__M 		 0x1 
#define EV6__DTB_PTE0__GH__S 		 0x5 
#define EV6__DTB_PTE0__GH__V 		 0x2 
#define EV6__DTB_PTE0__GH__M 		 0x3 
#define EV6__DTB_PTE0__RSV3__S 		 0x7 
#define EV6__DTB_PTE0__RSV3__V 		 0x1 
#define EV6__DTB_PTE0__RSV3__M 		 0x1 
#define EV6__DTB_PTE0__KRE__S 		 0x8 
#define EV6__DTB_PTE0__KRE__V 		 0x1 
#define EV6__DTB_PTE0__KRE__M 		 0x1 
#define EV6__DTB_PTE0__ERE__S 		 0x9 
#define EV6__DTB_PTE0__ERE__V 		 0x1 
#define EV6__DTB_PTE0__ERE__M 		 0x1 
#define EV6__DTB_PTE0__SRE__S 		 0xa 
#define EV6__DTB_PTE0__SRE__V 		 0x1 
#define EV6__DTB_PTE0__SRE__M 		 0x1 
#define EV6__DTB_PTE0__URE__S 		 0xb 
#define EV6__DTB_PTE0__URE__V 		 0x1 
#define EV6__DTB_PTE0__URE__M 		 0x1 
#define EV6__DTB_PTE0__KWE__S 		 0xc 
#define EV6__DTB_PTE0__KWE__V 		 0x1 
#define EV6__DTB_PTE0__KWE__M 		 0x1 
#define EV6__DTB_PTE0__EWE__S 		 0xd 
#define EV6__DTB_PTE0__EWE__V 		 0x1 
#define EV6__DTB_PTE0__EWE__M 		 0x1 
#define EV6__DTB_PTE0__SWE__S 		 0xe 
#define EV6__DTB_PTE0__SWE__V 		 0x1 
#define EV6__DTB_PTE0__SWE__M 		 0x1 
#define EV6__DTB_PTE0__UWE__S 		 0xf 
#define EV6__DTB_PTE0__UWE__V 		 0x1 
#define EV6__DTB_PTE0__UWE__M 		 0x1 
#define EV6__DTB_PTE0__RSV4__S 		 0x10 
#define EV6__DTB_PTE0__RSV4__V 		 0x10 
#define EV6__DTB_PTE0__RSV4__M 		 0xffff 
#define EV6__DTB_PTE0__PFN__S 		 0x20 
#define EV6__DTB_PTE0__PFN__V 		 0x1f 
#define EV6__DTB_PTE0__PFN__M 		 0x7fffffff 
#define EV6__DTB_PTE0__RSV5__S 		 0x3f 
#define EV6__DTB_PTE0__RSV5__V 		 0x1 
#define EV6__DTB_PTE0__RSV5__M 		 0x1 

#define EV6__DTB_PTE1__RSV1__S 		 0x0 
#define EV6__DTB_PTE1__RSV1__V 		 0x1 
#define EV6__DTB_PTE1__RSV1__M 		 0x1 
#define EV6__DTB_PTE1__FOR__S 		 0x1 
#define EV6__DTB_PTE1__FOR__V 		 0x1 
#define EV6__DTB_PTE1__FOR__M 		 0x1 
#define EV6__DTB_PTE1__FOW__S 		 0x2 
#define EV6__DTB_PTE1__FOW__V 		 0x1 
#define EV6__DTB_PTE1__FOW__M 		 0x1 
#define EV6__DTB_PTE1__RSV2__S 		 0x3 
#define EV6__DTB_PTE1__RSV2__V 		 0x1 
#define EV6__DTB_PTE1__RSV2__M 		 0x1 
#define EV6__DTB_PTE1__ASM__S 		 0x4 
#define EV6__DTB_PTE1__ASM__V 		 0x1 
#define EV6__DTB_PTE1__ASM__M 		 0x1 
#define EV6__DTB_PTE1__GH__S 		 0x5 
#define EV6__DTB_PTE1__GH__V 		 0x2 
#define EV6__DTB_PTE1__GH__M 		 0x3 
#define EV6__DTB_PTE1__RSV3__S 		 0x7 
#define EV6__DTB_PTE1__RSV3__V 		 0x1 
#define EV6__DTB_PTE1__RSV3__M 		 0x1 
#define EV6__DTB_PTE1__KRE__S 		 0x8 
#define EV6__DTB_PTE1__KRE__V 		 0x1 
#define EV6__DTB_PTE1__KRE__M 		 0x1 
#define EV6__DTB_PTE1__ERE__S 		 0x9 
#define EV6__DTB_PTE1__ERE__V 		 0x1 
#define EV6__DTB_PTE1__ERE__M 		 0x1 
#define EV6__DTB_PTE1__SRE__S 		 0xa 
#define EV6__DTB_PTE1__SRE__V 		 0x1 
#define EV6__DTB_PTE1__SRE__M 		 0x1 
#define EV6__DTB_PTE1__URE__S 		 0xb 
#define EV6__DTB_PTE1__URE__V 		 0x1 
#define EV6__DTB_PTE1__URE__M 		 0x1 
#define EV6__DTB_PTE1__KWE__S 		 0xc 
#define EV6__DTB_PTE1__KWE__V 		 0x1 
#define EV6__DTB_PTE1__KWE__M 		 0x1 
#define EV6__DTB_PTE1__EWE__S 		 0xd 
#define EV6__DTB_PTE1__EWE__V 		 0x1 
#define EV6__DTB_PTE1__EWE__M 		 0x1 
#define EV6__DTB_PTE1__SWE__S 		 0xe 
#define EV6__DTB_PTE1__SWE__V 		 0x1 
#define EV6__DTB_PTE1__SWE__M 		 0x1 
#define EV6__DTB_PTE1__UWE__S 		 0xf 
#define EV6__DTB_PTE1__UWE__V 		 0x1 
#define EV6__DTB_PTE1__UWE__M 		 0x1 
#define EV6__DTB_PTE1__RSV4__S 		 0x10 
#define EV6__DTB_PTE1__RSV4__V 		 0x10 
#define EV6__DTB_PTE1__RSV4__M 		 0xffff 
#define EV6__DTB_PTE1__PFN__S 		 0x20 
#define EV6__DTB_PTE1__PFN__V 		 0x1f 
#define EV6__DTB_PTE1__PFN__M 		 0x7fffffff 
#define EV6__DTB_PTE1__RSV5__S 		 0x3f 
#define EV6__DTB_PTE1__RSV5__V 		 0x1 
#define EV6__DTB_PTE1__RSV5__M 		 0x1 

#define EV6__DTB_IAP__RSV__S 		 0x0 
#define EV6__DTB_IAP__RSV__V 		 0x40 
#define EV6__DTB_IAP__RSV__M 		 0xffffffffffffffff 

#define EV6__DTB_IA__RSV__S 		 0x0 
#define EV6__DTB_IA__RSV__V 		 0x40 
#define EV6__DTB_IA__RSV__M 		 0xffffffffffffffff 

#define EV6__DTB_IS0__RSV1__S 		 0x0 
#define EV6__DTB_IS0__RSV1__V 		 0xd 
#define EV6__DTB_IS0__RSV1__M 		 0x1fff 
#define EV6__DTB_IS0__VA__S 		 0xd 
#define EV6__DTB_IS0__VA__V 		 0x23 
#define EV6__DTB_IS0__VA__M 		 0x7ffffffff 
#define EV6__DTB_IS0__RSV2__S 		 0x30 
#define EV6__DTB_IS0__RSV2__V 		 0x10 
#define EV6__DTB_IS0__RSV2__M 		 0xffff 

#define EV6__DTB_IS1__RSV1__S 		 0x0 
#define EV6__DTB_IS1__RSV1__V 		 0xd 
#define EV6__DTB_IS1__RSV1__M 		 0x1fff 
#define EV6__DTB_IS1__VA__S 		 0xd 
#define EV6__DTB_IS1__VA__V 		 0x23 
#define EV6__DTB_IS1__VA__M 		 0x7ffffffff 
#define EV6__DTB_IS1__RSV2__S 		 0x30 
#define EV6__DTB_IS1__RSV2__V 		 0x10 
#define EV6__DTB_IS1__RSV2__M 		 0xffff 

#define EV6__DTB_ASN0__RSV__S 		 0x0 
#define EV6__DTB_ASN0__RSV__V 		 0x38 
#define EV6__DTB_ASN0__RSV__M 		 0xffffffffffffff 
#define EV6__DTB_ASN0__ASN__S 		 0x38 
#define EV6__DTB_ASN0__ASN__V 		 0x8 
#define EV6__DTB_ASN0__ASN__M 		 0xff 

#define EV6__DTB_ASN1__RSV__S 		 0x0 
#define EV6__DTB_ASN1__RSV__V 		 0x38 
#define EV6__DTB_ASN1__RSV__M 		 0xffffffffffffff 
#define EV6__DTB_ASN1__ASN__S 		 0x38 
#define EV6__DTB_ASN1__ASN__V 		 0x8 
#define EV6__DTB_ASN1__ASN__M 		 0xff 

#define EV6__DTB_ALT_MODE__MODE__S 		 0x0 
#define EV6__DTB_ALT_MODE__MODE__V 		 0x2 
#define EV6__DTB_ALT_MODE__MODE__M 		 0x3 
#define EV6__DTB_ALT_MODE__RSV1__S 		 0x2 
#define EV6__DTB_ALT_MODE__RSV1__V 		 0x3e 
#define EV6__DTB_ALT_MODE__RSV1__M 		 0x3fffffffffffffff 

#define EV6__MM_STAT__WR__S 		 0x0 
#define EV6__MM_STAT__WR__V 		 0x1 
#define EV6__MM_STAT__WR__M 		 0x1 
#define EV6__MM_STAT__ACV__S 		 0x1 
#define EV6__MM_STAT__ACV__V 		 0x1 
#define EV6__MM_STAT__ACV__M 		 0x1 
#define EV6__MM_STAT__FOR__S 		 0x2 
#define EV6__MM_STAT__FOR__V 		 0x1 
#define EV6__MM_STAT__FOR__M 		 0x1 
#define EV6__MM_STAT__FOW__S 		 0x3 
#define EV6__MM_STAT__FOW__V 		 0x1 
#define EV6__MM_STAT__FOW__M 		 0x1 
#define EV6__MM_STAT__OPCODE__S 		 0x4 
#define EV6__MM_STAT__OPCODE__V 		 0x6 
#define EV6__MM_STAT__OPCODE__M 		 0x3f 
#define EV6__MM_STAT__DC_TAG_PERR__S 		 0xa 
#define EV6__MM_STAT__DC_TAG_PERR__V 		 0x1 
#define EV6__MM_STAT__DC_TAG_PERR__M 		 0x1 
#define EV6__MM_STAT__RSV1__S 		 0xb 
#define EV6__MM_STAT__RSV1__V 		 0x35 
#define EV6__MM_STAT__RSV1__M 		 0x1fffffffffffff 

#define EV6__M_CTL__RSV1__S 		 0x0 
#define EV6__M_CTL__RSV1__V 		 0x1 
#define EV6__M_CTL__RSV1__M 		 0x1 
#define EV6__M_CTL__SPE__S 		 0x1 
#define EV6__M_CTL__SPE__V 		 0x3 
#define EV6__M_CTL__SPE__M 		 0x7 
#define EV6__M_CTL__RSV2__S 		 0x4 
#define EV6__M_CTL__RSV2__V 		 0x3c 
#define EV6__M_CTL__RSV2__M 		 0xfffffffffffffff 

#define EV6__DC_CTL__SET_EN__S 		 0x0 
#define EV6__DC_CTL__SET_EN__V 		 0x2 
#define EV6__DC_CTL__SET_EN__M 		 0x3 
#define EV6__DC_CTL__F_HIT__S 		 0x2 
#define EV6__DC_CTL__F_HIT__V 		 0x1 
#define EV6__DC_CTL__F_HIT__M 		 0x1 
#define EV6__DC_CTL__FLUSH__S 		 0x3 
#define EV6__DC_CTL__FLUSH__V 		 0x1 
#define EV6__DC_CTL__FLUSH__M 		 0x1 
#define EV6__DC_CTL__F_BAD_TPAR__S 		 0x4 
#define EV6__DC_CTL__F_BAD_TPAR__V 		 0x1 
#define EV6__DC_CTL__F_BAD_TPAR__M 		 0x1 
#define EV6__DC_CTL__F_BAD_DECC__S 		 0x5 
#define EV6__DC_CTL__F_BAD_DECC__V 		 0x1 
#define EV6__DC_CTL__F_BAD_DECC__M 		 0x1 
#define EV6__DC_CTL__DCTAG_PAR_EN__S 		 0x6 
#define EV6__DC_CTL__DCTAG_PAR_EN__V 		 0x1 
#define EV6__DC_CTL__DCTAG_PAR_EN__M 		 0x1 
#define EV6__DC_CTL__DCDAT_ERR_EN__S 		 0x7 
#define EV6__DC_CTL__DCDAT_ERR_EN__V 		 0x1 
#define EV6__DC_CTL__DCDAT_ERR_EN__M 		 0x1 
#define EV6__DC_CTL__RSV1__S 		 0x8 
#define EV6__DC_CTL__RSV1__V 		 0x38 
#define EV6__DC_CTL__RSV1__M 		 0xffffffffffffff 

#define EV6__DC_STAT__TPERR_P0__S 		 0x0 
#define EV6__DC_STAT__TPERR_P0__V 		 0x1 
#define EV6__DC_STAT__TPERR_P0__M 		 0x1 
#define EV6__DC_STAT__TPERR_P1__S 		 0x1 
#define EV6__DC_STAT__TPERR_P1__V 		 0x1 
#define EV6__DC_STAT__TPERR_P1__M 		 0x1 
#define EV6__DC_STAT__DECC_ERR__S 		 0x2 
#define EV6__DC_STAT__DECC_ERR__V 		 0x1 
#define EV6__DC_STAT__DECC_ERR__M 		 0x1 
#define EV6__DC_STAT__DECC_COR__S 		 0x3 
#define EV6__DC_STAT__DECC_COR__V 		 0x1 
#define EV6__DC_STAT__DECC_COR__M 		 0x1 
#define EV6__DC_STAT__SEO__S 		 0x4 
#define EV6__DC_STAT__SEO__V 		 0x1 
#define EV6__DC_STAT__SEO__M 		 0x1 
#define EV6__DC_STAT__RSV1__S 		 0x5 
#define EV6__DC_STAT__RSV1__V 		 0x3b 
#define EV6__DC_STAT__RSV1__M 		 0x7ffffffffffffff 

#define EV6__DATA__C_DATA__S 		 0x0 
#define EV6__DATA__C_DATA__V 		 0x6 
#define EV6__DATA__C_DATA__M 		 0x3f 
#define EV6__DATA__RSV1__S 		 0x6 
#define EV6__DATA__RSV1__V 		 0x3a 
#define EV6__DATA__RSV1__M 		 0x3ffffffffffffff 

#define EV6__SHIFT_CONTROL__C_SHIFT__S 		 0x0 
#define EV6__SHIFT_CONTROL__C_SHIFT__V 		 0x1 
#define EV6__SHIFT_CONTROL__C_SHIFT__M 		 0x1 
#define EV6__SHIFT_CONTROL__RSV1__S 		 0x1 
#define EV6__SHIFT_CONTROL__RSV1__V 		 0x3f 
#define EV6__SHIFT_CONTROL__RSV1__M 		 0x7fffffffffffffff 

#define EV6__FEN 		 0x10000 
#define EV6__FEN__FEN__S 		 0x0 
#define EV6__FEN__FEN__V 		 0x1 
#define EV6__FEN__FEN__M 		 0x1 
#define EV6__FEN__RSV2__S 		 0x1 
#define EV6__FEN__RSV2__V 		 0x3f 
#define EV6__FEN__RSV2__M 		 0x7fffffffffffffff 

#define EV6__FPCR 		 0x10001 
#define EV6__FPCR__RSV1__S 		 0x0 
#define EV6__FPCR__RSV1__V 		 0x30 
#define EV6__FPCR__RSV1__M 		 0xffffffffffff 
#define EV6__FPCR__DNZ__S 		 0x30 
#define EV6__FPCR__DNZ__V 		 0x1 
#define EV6__FPCR__DNZ__M 		 0x1 
#define EV6__FPCR__INVD__S 		 0x31 
#define EV6__FPCR__INVD__V 		 0x1 
#define EV6__FPCR__INVD__M 		 0x1 
#define EV6__FPCR__DZED__S 		 0x32 
#define EV6__FPCR__DZED__V 		 0x1 
#define EV6__FPCR__DZED__M 		 0x1 
#define EV6__FPCR__OVFD__S 		 0x33 
#define EV6__FPCR__OVFD__V 		 0x1 
#define EV6__FPCR__OVFD__M 		 0x1 
#define EV6__FPCR__INV__S 		 0x34 
#define EV6__FPCR__INV__V 		 0x1 
#define EV6__FPCR__INV__M 		 0x1 
#define EV6__FPCR__DZE__S 		 0x35 
#define EV6__FPCR__DZE__V 		 0x1 
#define EV6__FPCR__DZE__M 		 0x1 
#define EV6__FPCR__OVF__S 		 0x36 
#define EV6__FPCR__OVF__V 		 0x1 
#define EV6__FPCR__OVF__M 		 0x1 
#define EV6__FPCR__UNF__S 		 0x37 
#define EV6__FPCR__UNF__V 		 0x1 
#define EV6__FPCR__UNF__M 		 0x1 
#define EV6__FPCR__INE__S 		 0x38 
#define EV6__FPCR__INE__V 		 0x1 
#define EV6__FPCR__INE__M 		 0x1 
#define EV6__FPCR__IOV__S 		 0x39 
#define EV6__FPCR__IOV__V 		 0x1 
#define EV6__FPCR__IOV__M 		 0x1 
#define EV6__FPCR__DYN__S 		 0x3a 
#define EV6__FPCR__DYN__V 		 0x2 
#define EV6__FPCR__DYN__M 		 0x3 
#define EV6__FPCR__UNDZ__S 		 0x3c 
#define EV6__FPCR__UNDZ__V 		 0x1 
#define EV6__FPCR__UNDZ__M 		 0x1 
#define EV6__FPCR__UNFD__S 		 0x3d 
#define EV6__FPCR__UNFD__V 		 0x1 
#define EV6__FPCR__UNFD__M 		 0x1 
#define EV6__FPCR__INED__S 		 0x3e 
#define EV6__FPCR__INED__V 		 0x1 
#define EV6__FPCR__INED__M 		 0x1 
#define EV6__FPCR__SUM__S 		 0x3f 
#define EV6__FPCR__SUM__V 		 0x1 
#define EV6__FPCR__SUM__M 		 0x1 

#define EV6__HW_INT_REG 		 0x1000e 
#define EV6__HW_INT_REG__RSV1__S 		 0x0 
#define EV6__HW_INT_REG__RSV1__V 		 0x1a 
#define EV6__HW_INT_REG__RSV1__M 		 0x3ffffff 
#define EV6__HW_INT_REG__FBTP__S 		 0x1a 
#define EV6__HW_INT_REG__FBTP__V 		 0x1 
#define EV6__HW_INT_REG__FBTP__M 		 0x1 
#define EV6__HW_INT_REG__FBDP__S 		 0x1b 
#define EV6__HW_INT_REG__FBDP__V 		 0x1 
#define EV6__HW_INT_REG__FBDP__M 		 0x1 
#define EV6__HW_INT_REG__MCHK_D__S 		 0x1c 
#define EV6__HW_INT_REG__MCHK_D__V 		 0x1 
#define EV6__HW_INT_REG__MCHK_D__M 		 0x1 
#define EV6__HW_INT_REG__PC__S 		 0x1d 
#define EV6__HW_INT_REG__PC__V 		 0x2 
#define EV6__HW_INT_REG__PC__M 		 0x3 
#define EV6__HW_INT_REG__CR__S 		 0x1f 
#define EV6__HW_INT_REG__CR__V 		 0x1 
#define EV6__HW_INT_REG__CR__M 		 0x1 
#define EV6__HW_INT_REG__SL__S 		 0x20 
#define EV6__HW_INT_REG__SL__V 		 0x1 
#define EV6__HW_INT_REG__SL__M 		 0x1 
#define EV6__HW_INT_REG__RSV2__S 		 0x21 
#define EV6__HW_INT_REG__RSV2__V 		 0x1f 
#define EV6__HW_INT_REG__RSV2__M 		 0x7fffffff 

#define EV6_SCB__CC 		 0x20 
#define EV6_SCB__CC_CTL 		 0x20 
#define EV6_SCB__VA 		 0xf0 
#define EV6_SCB__VA_FORM 		 0xf0 
#define EV6_SCB__VA_CTL 		 0x20 
#define EV6_SCB__ITB_TAG 		 0x40 
#define EV6_SCB__ITB_PTE 		 0x11 
#define EV6_SCB__ITB_IAP 		 0x10 
#define EV6_SCB__ITB_IA 		 0x10 
#define EV6_SCB__ITB_IS 		 0x50 
#define EV6_SCB__EXC_ADDR 		 0x0 
#define EV6_SCB__IVA_FORM 		 0x0 
#define EV6_SCB__PS 		 0x10 
#define EV6_SCB__IER 		 0x10 
#define EV6_SCB__IER_CM 		 0x10 
#define EV6_SCB__SIRR 		 0x10 
#define EV6_SCB__ISUM 		 0x0 
#define EV6_SCB__HW_INT_CLR 		 0x10 
#define EV6_SCB__EXC_SUM 		 0x0 
#define EV6_SCB__PAL_BASE 		 0x10 
#define EV6_SCB__I_CTL 		 0x10 
#define EV6_SCB__IC_FLUSH 		 0x10 
#define EV6_SCB__IC_FLUSH_ASM 		 0x10 
#define EV6_SCB__PCTR_CTL 		 0x10 
#define EV6_SCB__CLR_MAP 		 0xf0 
#define EV6_SCB__I_STAT 		 0x10 
#define EV6_SCB__ASN 		 0x10 
#define EV6_SCB__ASTER 		 0x10 
#define EV6_SCB__ASTRR 		 0x10 
#define EV6_SCB__PPCE 		 0x10 
#define EV6_SCB__FPE 		 0x10 
#define EV6_SCB__PROCESS_CONTEXT 		 0x10 
#define EV6_SCB__DTB_TAG0 		 0x44 
#define EV6_SCB__DTB_TAG1 		 0x22 
#define EV6_SCB__DTB_PTE0 		 0x11 
#define EV6_SCB__DTB_PTE1 		 0x88 
#define EV6_SCB__DTB_IAP 		 0x80 
#define EV6_SCB__DTB_IA 		 0x80 
#define EV6_SCB__DTB_IS0 		 0x40 
#define EV6_SCB__DTB_IS1 		 0x80 
#define EV6_SCB__DTB_ASN0 		 0x10 
#define EV6_SCB__DTB_ASN1 		 0x80 
#define EV6_SCB__DTB_ALT_MODE 		 0x40 
#define EV6_SCB__MM_STAT 		 0x0 
#define EV6_SCB__M_CTL 		 0x40 
#define EV6_SCB__DC_CTL 		 0x40 
#define EV6_SCB__DC_STAT 		 0x40 
#define EV6_SCB__DATA 		 0x40 
#define EV6_SCB__SHIFT_CONTROL 		 0x40 



/* here are the actual ev6 IPRs */

/* ev6 ev6 ev6 ev6 ev6 ev6 ev6 ev6 ev6 ev6 */

/* I had to shift the ipr and then or in the proper scoreboard bits */
/* redefine stuff for handling the scoreboard bits */
 
/* Ebox iprs */

#define EV6__CC 		(( 0xC0 << 8) | EV6_SCB__CC)

#define EV6__CC_CTL 		(( 0xC1 << 8) | EV6_SCB__CC_CTL)
#define EV6__VA 		(( 0xC2 << 8) | EV6_SCB__VA)
#define EV6__VA_FORM 		(( 0xC3 << 8) | EV6_SCB__VA_FORM)
#define EV6__VA_FORM_48 	(( 0xC3 << 8) | EV6_SCB__VA_FORM)
#define EV6__VA_FORM_32 	(( 0xC3 << 8) | EV6_SCB__VA_FORM)
#define EV6__VA_CTL 		(( 0xC4 << 8) | EV6_SCB__VA_CTL)

/* Ibox iprs */

#define EV6__ITB_TAG 		(( 0x00 << 8) | EV6_SCB__ITB_TAG)
#define EV6__ITB_PTE 		(( 0x01 << 8) | EV6_SCB__ITB_PTE)
#define EV6__ITB_IAP 		(( 0x02 << 8) | EV6_SCB__ITB_IAP)
#define EV6__ITB_IA 		(( 0x03 << 8) | EV6_SCB__ITB_IA)
#define EV6__ITB_IS 		(( 0x04 << 8) | EV6_SCB__ITB_IS)

#define EV6__EXC_ADDR 		(( 0x06 << 8) | EV6_SCB__EXC_ADDR)
#define EV6__IVA_FORM 		(( 0x07 << 8) | EV6_SCB__IVA_FORM)
#define EV6__IVA_FORM_48 	(( 0x07 << 8) | EV6_SCB__IVA_FORM)
#define EV6__IVA_FORM_32 	(( 0x07 << 8) | EV6_SCB__IVA_FORM)
#define EV6__PS 		(( 0x09 << 8) | EV6_SCB__PS)
#define EV6__IER 		(( 0x0A << 8) | EV6_SCB__IER)
#define EV6__IER_CM 		(( 0x0B << 8) | EV6_SCB__IER_CM)

#define EV6__SIRR 		(( 0x0C << 8) | EV6_SCB__SIRR)
#define EV6__ISUM 		(( 0x0D << 8) | EV6_SCB__ISUM)
#define EV6__HW_INT_CLR 	(( 0x0E << 8) | EV6_SCB__HW_INT_CLR)
#define EV6__EXC_SUM 		(( 0x0F << 8) | EV6_SCB__EXC_SUM)
#define EV6__PAL_BASE 		(( 0x10 << 8) | EV6_SCB__PAL_BASE)
#define EV6__I_CTL 		(( 0x11 << 8) | EV6_SCB__I_CTL)


#define EV6__IC_FLUSH_ASM 	(( 0x12 << 8) | EV6_SCB__IC_FLUSH_ASM)
#define EV6__IC_FLUSH 		(( 0x13 << 8) | EV6_SCB__IC_FLUSH)
#define EV6__PCTR_CTL 		(( 0x14 << 8) | EV6_SCB__PCTR_CTL)
#define EV6__CLR_MAP 		(( 0x15 << 8) | EV6_SCB__CLR_MAP)
#define EV6__I_STAT 		(( 0x16 << 8) | EV6_SCB__I_STAT)
/* sleep does have scoreboard bits in it */
#define EV6__SLEEP              (( 0x17 << 8) | 0xF0

#define EV6__ASN 		(( 0x41 << 8) | EV6_SCB__ASN)
#define EV6__ASTER 		(( 0x42 << 8) | EV6_SCB__ASTER)
#define EV6__ASTRR 		(( 0x44 << 8) | EV6_SCB__ASTRR)
#define EV6__PPCE 		(( 0x48 << 8) | EV6_SCB__PPCE)
#define EV6__FPE 		(( 0x50 << 8) | EV6_SCB__FPE)
#define EV6__ASN_ASTER 		(( 0x43 << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASN_ASTRR 		(( 0x45 << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASN_PPCE 		(( 0x49 << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASN_FPE 		(( 0x51 << 8) | EV6_SCB__PROCESS_CONTEXT)

#define EV6__ASTER_ASTRR 	(( 0x46 << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASTER_PPCE 	(( 0x4A << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASTER_FPE 		(( 0x52 << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASTRR_PPCE 	(( 0x4C << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASTRR_FPE  	(( 0x54 << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__PPCE_FPE 		(( 0x58 << 8) | EV6_SCB__PROCESS_CONTEXT)

#define EV6__ASN_ASTER_ASTRR 	(( 0x47 << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASN_ASTER_PPCE 	(( 0x4B << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASN_ASTER_FPE 	(( 0x53 << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASN_ASTRR_PPCE 	(( 0x4D << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASN_ASTRR_FPE 	(( 0x55 << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASN_PPCE_FPE  	(( 0x59 << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASTER_ASTRR_PPCE 	(( 0x4E << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASTER_ASTRR_FPE 	(( 0x56 << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASTER_PPCE_FPE  	(( 0x5A << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASTRR_PPCE_FPE 	(( 0x5C << 8) | EV6_SCB__PROCESS_CONTEXT)

#define EV6__ASN_ASTER_ASTRR_PPCE	(( 0x4F << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASN_ASTER_ASTRR_FPE 	(( 0x57 << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASN_ASTER_PPCE_FPE 	(( 0x5B << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASN_ASTRR_PPCE_FPE 	(( 0x5D << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__ASTER_ASTRR_PPCE_FPE 	(( 0x5E << 8) | EV6_SCB__PROCESS_CONTEXT)
#define EV6__PROCESS_CONTEXT 		(( 0x5F << 8) | EV6_SCB__PROCESS_CONTEXT)

/* Mbox iprs */

#define EV6__DTB_TAG0 		(( 0x20 << 8) | EV6_SCB__DTB_TAG0)
#define EV6__DTB_TAG1 		(( 0xA0 << 8) | EV6_SCB__DTB_TAG1)
#define EV6__DTB_PTE0 		(( 0x21 << 8) | EV6_SCB__DTB_PTE0)
#define EV6__DTB_PTE1 		(( 0xA1 << 8) | EV6_SCB__DTB_PTE1)

#define EV6__DTB_IAP 		(( 0xA2 << 8) | EV6_SCB__DTB_IAP)
#define EV6__DTB_IA 		(( 0xA3 << 8) | EV6_SCB__DTB_IA)
#define EV6__DTB_IS0 		(( 0x24 << 8) | EV6_SCB__DTB_IS0)
#define EV6__DTB_IS1 		(( 0xA4 << 8) | EV6_SCB__DTB_IS1)
#define EV6__DTB_ASN0 		(( 0x25 << 8) | EV6_SCB__DTB_ASN0)
#define EV6__DTB_ASN1 		(( 0xA5 << 8) | EV6_SCB__DTB_ASN1)

#define EV6__DTB_ALT_MODE 	(( 0x26 << 8) | EV6_SCB__DTB_ALT_MODE)
#define EV6__MM_STAT 		(( 0x27 << 8) | EV6_SCB__MM_STAT)
#define EV6__M_CTL 		(( 0x28 << 8) | EV6_SCB__M_CTL)
#define EV6__DC_CTL 		(( 0x29 << 8) | EV6_SCB__DC_CTL)
#define EV6__DC_STAT 		(( 0x2A << 8) | EV6_SCB__DC_STAT)

/* Cbox iprs */

#define EV6__DATA 		(( 0x2B << 8) | EV6_SCB__DATA)
#define EV6__SHIFT_CONTROL 	(( 0x2C << 8) | EV6_SCB__SHIFT_CONTROL)

/* ev6 ev6 ev6 ev6 ev6 ev6 ev6 ev6 ev6 ev6 */


/* old ev5 stuff begins here */

/*
**
**  Ibox IPR Definitions:
**
*/

#define isr		0x100	/* RO - Interrupt Summary */
#define itbTag		0x101	/* WO - ITB Tag */
#define	itbPte		0x102	/* RW - ITB Page Table Entry */
#define itbAsn		0x103	/* RW - ITB Address Space Number */
#define itbPteTemp	0x104	/* RO - ITB Page Table Entry Temporary */
#define	itbIa		0x105	/* WO - ITB Invalidate All */
#define itbIap		0x106	/* WO - ITB Invalidate All Process */
#define itbIs		0x107	/* WO - ITB Invalidate Single */
#define sirr		0x108	/* RW - Software Interrupt Request */
#define astrr		0x109	/* RW - Async. System Trap Request */
#define aster		0x10A	/* RW - Async. System Trap Enable */
#define excAddr		0x10B	/* RW - Exception Address */
#define excSum		0x10C	/* RW - Exception Summary */
#define excMask		0x10D	/* RO - Exception Mask */
#define palBase		0x10E	/* RW - PAL Base */
#define ips		0x10F	/* RW - Processor Status */
#define ipl		0x110	/* RW - Interrupt Priority Level */
#define intId		0x111	/* RO - Interrupt ID */
#define iFaultVaForm	0x112	/* RO - Formatted Faulting VA */
#define iVptBr		0x113	/* RW - I-Stream Virtual Page Table Base */
#define hwIntClr	0x115	/* WO - Hardware Interrupt Clear */
#define slXmit		0x116	/* WO - Serial Line Transmit */
#define slRcv		0x117	/* RO - Serial Line Receive */
#define icsr		0x118	/* RW - Ibox Control/Status */
#define icFlush		0x119	/* WO - I-Cache Flush Control */
#define flushIc         0x119   /* WO - I-Cache Flush Control (DC21064 Symbol) */
#define icPerr		0x11A	/* RW - I-Cache Parity Error Status */
#define PmCtr		0x11C	/* RW - Performance Counter */

/*
**
**  Ibox Control/Status Register (ICSR) Bit Summary
**
**	Extent	Size	Name	Type	Function
**	------	----	----	----	------------------------------------
**	 <39>	 1	TST	RW,0	Assert Test Status
**	 <38>	 1	ISTA	RO	I-Cache BIST Status
**	 <37>	 1	DBS	RW,1	Debug Port Select
**	 <36>	 1	FBD	RW,0	Force Bad I-Cache Data Parity
**	 <35>	 1	FBT	RW,0	Force Bad I-Cache Tag Parity
**	 <34>	 1	FMS	RW,0	Force I-Cache Miss
**	 <33>	 1	SLE	RW,0	Enable Serial Line Interrupts
**	 <32>	 1	CRDE	RW,0	Enable Correctable Error Interrupts
**	 <30>	 1	SDE	RW,0	Enable PAL Shadow Registers
**	<29:28>	 2	SPE	RW,0	Enable I-Stream Super Page Mode
**	 <27>	 1	HWE	RW,0	Enable PALRES Instrs in Kernel Mode
**	 <26>	 1	FPE	RW,0	Enable Floating Point Instructions
**	 <25>	 1	TMD	RW,0	Disable Ibox Timeout Counter
**	 <24>	 1	TMM	RW,0	Timeout Counter Mode
**
*/

#define ICSR_V_TST	39
#define ICSR_M_TST	(1<<ICSR_V_TST)
#define ICSR_V_ISTA	38
#define ICSR_M_ISTA	(1<<ICSR_V_ISTA)
#define ICSR_V_DBS	37
#define ICSR_M_DBS	(1<<ICSR_V_DBS)
#define ICSR_V_FBD	36
#define ICSR_M_FBD	(1<<ICSR_V_FBD)
#define ICSR_V_FBT	35
#define	ICSR_M_FBT	(1<<ICSR_V_FBT)
#define ICSR_V_FMS	34
#define ICSR_M_FMS	(1<<ICSR_V_FMS)
#define	ICSR_V_SLE	33
#define ICSR_M_SLE	(1<<ICSR_V_SLE)
#define ICSR_V_CRDE	32
#define ICSR_M_CRDE	(1<<ICSR_V_CRDE)
#define ICSR_V_SDE	30
#define ICSR_M_SDE	(1<<ICSR_V_SDE)
#define ICSR_V_SPE	28
#define ICSR_M_SPE	(3<<ICSR_V_SPE)
#define ICSR_V_HWE	27
#define ICSR_M_HWE	(1<<ICSR_V_HWE)
#define ICSR_V_FPE	26
#define ICSR_M_FPE	(1<<ICSR_V_FPE)
#define ICSR_V_TMD	25
#define ICSR_M_TMD	(1<<ICSR_V_TMD)
#define ICSR_V_TMM	24
#define ICSR_M_TMM	(1<<ICSR_V_TMM)

/*
**
**  Serial Line Tranmit Register (SL_XMIT)
**
**	Extent	Size	Name	Type	Function
**	------	----	----	----	------------------------------------
**	 <7>	 1	TMT	WO,1	Serial line transmit data
**
*/

#define	SLXMIT_V_TMT   	7
#define SLXMIT_M_TMT	(1<<SLXMIT_V_TMT)

/*
**
**  Serial Line Receive Register (SL_RCV)
**
**	Extent	Size	Name	Type	Function
**	------	----	----	----	------------------------------------
**	 <6>	 1	RCV	RO	Serial line receive data
**
*/

#define	SLRCV_V_RCV   	6
#define SLRCV_M_RCV	(1<<SLRCV_V_RCV)

/*
**
**  Icache Parity Error Status Register (ICPERR) Bit Summary
**
**	Extent	Size	Name	Type	Function
**	------	----	----	----	------------------------------------
**	 <13>	 1	TMR	W1C	Timeout reset error
**	 <12>	 1	TPE	W1C	Tag parity error
**	 <11>	 1	DPE	W1C	Data parity error
**
*/

#define	ICPERR_V_TMR   	13
#define ICPERR_M_TMR	(1<<ICPERR_V_TMR)
#define ICPERR_V_TPE	12
#define ICPERR_M_TPE	(1<<ICPERR_V_TPE)
#define ICPERR_V_DPE	11
#define ICPERR_M_DPE	(1<<ICPERR_V_DPE)

#define ICPERR_M_ALL	(ICPERR_M_TMR | ICPERR_M_TPE | ICPERR_M_DPE)

/*
**
**  Exception Summary Register (EXC_SUM) Bit Summary
**
**	Extent	Size	Name	Type	Function
**	------	----	----	----	------------------------------------
**	 <16>	 1	IOV	 WA	Integer overflow
**	 <15>	 1	INE	 WA	Inexact result
**	 <14>	 1	UNF	 WA	Underflow
**	 <13>	 1	FOV	 WA	Overflow
**	 <12>	 1	DZE	 WA	Division by zero
**	 <11>	 1	INV	 WA	Invalid operation
**	 <10>	 1	SWC	 WA	Software completion
**
*/

#define EXC_V_IOV	16
#define EXC_M_IOV	(1<<EXC_V_IOV)
#define EXC_V_INE	15
#define EXC_M_INE	(1<<EXC_V_INE)
#define EXC_V_UNF	14
#define EXC_M_UNF	(1<<EXC_V_UNF)
#define EXC_V_FOV	13
#define EXC_M_FOV	(1<<EXC_V_FOV)
#define EXC_V_DZE	12
#define	EXC_M_DZE	(1<<EXC_V_DZE)
#define EXC_V_INV	11
#define EXC_M_INV	(1<<EXC_V_INV)
#define	EXC_V_SWC	10
#define EXC_M_SWC	(1<<EXC_V_SWC)

/*
**
**  Hardware Interrupt Clear Register (HWINT_CLR) Bit Summary
**
**	 Extent	Size	Name	Type	Function
**	 ------	----	----	----	---------------------------------
**	  <33>	  1	SLC	W1C	Clear Serial Line interrupt
**	  <32>	  1	CRDC	W1C	Clear Correctable Read Data interrupt
**	  <29>	  1	PC2C	W1C	Clear Performance Counter 2 interrupt
**	  <28>	  1	PC1C	W1C	Clear Performance Counter 1 interrupt
**	  <27>	  1	PC0C    W1C	Clear Performance Counter 0 interrupt
**
*/

#define HWINT_V_SLC	33
#define HWINT_M_SLC	(1<<HWINT_V_SLC)
#define HWINT_V_CRDC	32
#define HWINT_M_CRDC	(1<<HWINT_V_CRDC)
#define HWINT_V_PC2C	29
#define HWINT_M_PC2C	(1<<HWINT_V_PC2C)
#define HWINT_V_PC1C	28
#define HWINT_M_PC1C	(1<<HWINT_V_PC1C)
#define HWINT_V_PC0C	27
#define HWINT_M_PC0C	(1<<HWINT_V_PC0C)

/*
**
**  Interrupt Summary Register (ISR) Bit Summary
**
**	 Extent	Size	Name	Type	Function
**	 ------	----	----	----	---------------------------------
**	  <34>	  1	HLT    	RO	External Halt interrupt
**	  <33>	  1	SLI	RO	Serial Line interrupt
**	  <32>	  1	CRD	RO	Correctable ECC errors
**	  <31>	  1	MCK	RO	System Machine Check
**	  <30>	  1	PFL	RO	Power Fail
**	  <29>	  1	PC2	RO	Performance Counter 2 interrupt
**	  <28>	  1	PC1	RO	Performance Counter 1 interrupt
**	  <27>	  1	PC0	RO	Performance Counter 0 interrupt
**	  <23>	  1	I23	RO	External Hardware interrupt
**	  <22>	  1	I22	RO	External Hardware interrupt
**	  <21>	  1	I21	RO	External Hardware interrupt
**	  <20>	  1	I20	RO	External Hardware interrupt
**	  <19>	  1	ATR	RO	Async. System Trap request
**	 <18:4>	 15	SIRR	RO,0	Software Interrupt request
**	  <3:0>	  4	ASTRR	RO	Async. System Trap request (USEK)
**
**/

#define ISR_V_HLT	34
#define ISR_M_HLT	(1<<ISR_V_HLT)
#define ISR_V_SLI	33
#define ISR_M_SLI	(1<<ISR_V_SLI)
#define ISR_V_CRD	32
#define ISR_M_CRD	(1<<ISR_V_CRD)
#define ISR_V_MCK	31
#define ISR_M_MCK	(1<<ISR_V_MCK)
#define ISR_V_PFL	30
#define ISR_M_PFL	(1<<ISR_V_PFL)
#define ISR_V_PC2	29
#define ISR_M_PC2	(1<<ISR_V_PC2)
#define ISR_V_PC1	28
#define ISR_M_PC1	(1<<ISR_V_PC1)
#define ISR_V_PC0	27
#define ISR_M_PC0	(1<<ISR_V_PC0)
#define ISR_V_I23	23
#define ISR_M_I23	(1<<ISR_V_I23)
#define ISR_V_I22	22
#define ISR_M_I22	(1<<ISR_V_I22)
#define ISR_V_I21	21
#define ISR_M_I21	(1<<ISR_V_I21)
#define ISR_V_I20	20
#define ISR_M_I20	(1<<ISR_V_I20)
#define ISR_V_ATR	19
#define ISR_M_ATR	(1<<ISR_V_ATR)
#define ISR_V_SIRR	4
#define ISR_M_SIRR	(0x7FFF<<ISR_V_SIRR)
#define ISR_V_ASTRR	0
#define ISR_M_ASTRR	(0xF<<ISR_V_ASTRR)

/*
**
**  Mbox and D-Cache IPR Definitions:
**
*/

#define dtbAsn		0x200	/* WO - DTB Address Space Number */
#define dtbCm		0x201	/* WO - DTB Current Mode */
#define dtbTag		0x202	/* WO - DTB Tag */
#define dtbPte		0x203	/* RW - DTB Page Table Entry */
#define dtbPteTemp	0x204	/* RO - DTB Page Table Entry Temporary */
#define mmStat		0x205	/* RO - D-Stream MM Fault Status */
#define va		0x206	/* RO - Faulting Virtual Address */
#define vaForm		0x207	/* RO - Formatted Virtual Address */
#define mVptBr		0x208	/* WO - Mbox Virtual Page Table Base */
#define dtbIap		0x209	/* WO - DTB Invalidate All Process */
#define dtbIa		0x20A	/* WO - DTB Invalidate All */
#define dtbIs		0x20B	/* WO - DTB Invalidate Single */
#define altMode		0x20C	/* WO - Alternate Mode */
#define cc		0x20D	/* WO - Cycle Counter */
#define ccCtl		0x20E	/* WO - Cycle Counter Control */
#define mcsr		0x20F	/* RW - Mbox Control Register */
#define dcFlush		0x210	/* WO - Dcache Flush */
#define dcPerr	        0x212	/* RW - Dcache Parity Error Status */
#define dcTestCtl	0x213	/* RW - Dcache Test Tag Control */
#define dcTestTag	0x214	/* RW - Dcache Test Tag */
#define dcTestTagTemp	0x215	/* RW - Dcache Test Tag Temporary */
#define dcMode		0x216	/* RW - Dcache Mode */
#define mafMode		0x217	/* RW - Miss Address File Mode */

/*
**
**  D-Stream MM Fault Status Register (MM_STAT) Bit Summary
**
**	 Extent	Size	Name	  Type	Function
**	 ------	----	----	  ----	---------------------------------
**	<16:11>	  6	OPCODE 	  RO	Opcode of faulting instruction
**	<10:06>	  5	RA	  RO	Ra field of faulting instruction
**          <5>	  1	BAD_VA	  RO	Bad virtual address
**	    <4>	  1	DTB_MISS  RO	Reference resulted in DTB miss
**	    <3>	  1	FOW	  RO	Fault on write
**	    <2>	  1	FOR	  RO	Fault on read
**	    <1>   1     ACV	  RO	Access violation
**          <0>	  1	WR	  RO	Reference type
**
*/

#define	MMSTAT_V_OPC		11
#define MMSTAT_M_OPC		(0x3F<<MMSTAT_V_OPC)
#define MMSTAT_V_RA		6
#define MMSTAT_M_RA		(0x1F<<MMSTAT_V_RA)
#define MMSTAT_V_BAD_VA		5
#define MMSTAT_M_BAD_VA		(1<<MMSTAT_V_BAD_VA)
#define MMSTAT_V_DTB_MISS	4
#define MMSTAT_M_DTB_MISS	(1<<MMSTAT_V_DTB_MISS)
#define MMSTAT_V_FOW		3
#define MMSTAT_M_FOW		(1<<MMSTAT_V_FOW)
#define MMSTAT_V_FOR		2
#define MMSTAT_M_FOR		(1<<MMSTAT_V_FOR)
#define MMSTAT_V_ACV		1
#define MMSTAT_M_ACV		(1<<MMSTAT_V_ACV)
#define MMSTAT_V_WR		0
#define MMSTAT_M_WR		(1<<MMSTAT_V_WR)


/*
**
** Mbox Control Register (MCSR) Bit Summary
**
**	 Extent	Size	Name	Type	Function
**	 ------	----	----	----	---------------------------------
**	   <5>	  1	DBG1	RW,0   	Mbox Debug Packet Select
**	   <4>	  1	E_BE	RW,0	Ebox Big Endian mode enable
**	   <3>	  1	DBG0	RW,0	Debug Test Select
**	  <2:1>	  2	SP	RW,0   	Superpage mode enable
**	   <0>	  1	M_BE	RW,0    Mbox Big Endian mode enable
**
*/

#define MCSR_V_DBG1	5
#define MCSR_M_DBG1	(1<<MCSR_V_DBG1)
#define MCSR_V_E_BE	4
#define MCSR_M_E_BE	(1<<MCSR_V_E_BE)
#define MCSR_V_DBG0	3
#define MCSR_M_DBG0	(1<<MCSR_V_DBG0)
#define MCSR_V_SP	1
#define MCSR_M_SP	(3<<MCSR_V_SP)
#define MCSR_V_M_BE	0
#define MCSR_M_M_BE	(1<<MCSR_V_M_BE)

/*
**
**  Dcache Parity Error Status Register (DCPERR) Bit Summary
**
**	Extent	Size	Name	Type	Function
**	------	----	----	----	------------------------------------
**	 <5>	 1	TP1	RO	Dcache bank 1 tag parity error
**	 <4>	 1	TP0	RO	Dcache bank 0 tag parity error
**	 <3>	 1	DP1	RO	Dcache bank 1 data parity error
**	 <2>	 1	DP0	RO	Dcache bank 0 data parity error
**	 <1>	 1	LOCK	W1C	Locks/clears bits <5:2>
**	 <0>	 1	SEO	W1C	Second Dcache parity error occurred
**
*/

#define DCPERR_V_TP1	5
#define DCPERR_M_TP1	(1<<DCPERR_V_TP1)
#define	DCPERR_V_TP0   	4
#define DCPERR_M_TP0	(1<<DCPERR_V_TP0)
#define DCPERR_V_DP1	3
#define DCPERR_M_DP1	(1<<DCPERR_V_DP1)
#define DCPERR_V_DP0    2
#define DCPERR_M_DP0	(1<<DCPERR_V_DP0)
#define DCPERR_V_LOCK	1
#define DCPERR_M_LOCK	(1<<DCPERR_V_LOCK)
#define DCPERR_V_SEO	0
#define DCPERR_M_SEO	(1<<DCPERR_V_SEO)

#define DCPERR_M_ALL	(DCPERR_M_LOCK | DCPERR_M_SEO)

/*
**
**  Dcache Mode Register (DC_MODE) Bit Summary
**
**	 Extent	Size	Name	  Type	Function
**	 ------	----	----	  ----	---------------------------------
**	   <4>	  1	DOA	  RO    Hardware Dcache Disable
**	   <3>	  1	PERR_DIS  RW,0	Disable Dcache Parity Error reporting
**	   <2>	  1	BAD_DP	  RW,0	Force Dcache data bad parity
**	   <1>	  1	FHIT	  RW,0	Force Dcache hit
**	   <0>	  1	ENA 	  RW,0	Software Dcache Enable
**
*/

#define	DC_V_DOA	4
#define DC_M_DOA        (1<<DC_V_DOA)
#define DC_V_PERR_DIS	3
#define DC_M_PERR_DIS	(1<<DC_V_PERR_DIS)
#define DC_V_BAD_DP	2
#define DC_M_BAD_DP	(1<<DC_V_BAD_DP)
#define DC_V_FHIT	1
#define DC_M_FHIT	(1<<DC_V_FHIT)
#define DC_V_ENA	0
#define DC_M_ENA	(1<<DC_V_ENA)

/*
**
**  Miss Address File Mode Register (MAF_MODE) Bit Summay
**
**	 Extent	Size	Name	  Type	Function
**	 ------	----	----	  ----	---------------------------------
**         <7>    1     WB        RO,0  If set, pending WB request
**	   <6>	  1	DREAD	  RO,0  If set, pending D-read request
**
*/

#define MAF_V_WB_PENDING        7
#define MAF_M_WB_PENDING        (1<<MAF_V_WB_PENDING)
#define MAF_V_DREAD_PENDING     6
#define MAF_M_DREAD_PENDING     (1<<MAF_V_DREAD_PENDING)

/*
**
**  Cbox IPR Definitions:
**
*/

#define scCtl		0x0A8	/* RW - Scache Control */
#define scStat		0x0E8	/* RO - Scache Error Status */
#define scAddr		0x188	/* RO - Scache Error Address */
#define	bcCtl		0x128	/* WO - Bcache/System Interface Control */
#define bcCfg		0x1C8	/* WO - Bcache Configuration Parameters */
#define bcTagAddr	0x108	/* RO - Bcache Tag */
#define eiStat		0x168	/* RO - Bcache/System Error Status */
#define eiAddr		0x148	/* RO - Bcache/System Error Address */
#define fillSyn		0x068	/* RO - Fill Syndrome */
#define ldLock		0x1E8	/* RO - LDx_L Address */

/*
**
**  Scache Control Register (SC_CTL) Bit Summary
**
**	 Extent	Size	Name	  Type	Function
**	 ------	----	----	  ----	---------------------------------
**	 <15:13>  3	SET_EN	  RW,1  Set enable
**	    <12>  1	BLK_SIZE  RW,1	Scache/Bcache block size select
**	 <11:08>  4	FB_DP	  RW,0	Force bad data parity
**	 <07:02>  6	TAG_STAT  RW	Tag status and parity
**	     <1>  1	FLUSH	  RW,0	If set, clear all tag valid bits
**	     <0>  1     FHIT	  RW,0  Force hits
**
*/

#define	SC_V_SET_EN	13
#define SC_M_SET_EN	(7<<SC_V_SET_EN)
#define SC_V_BLK_SIZE	12
#define SC_M_BLK_SIZE	(1<<SC_V_BLK_SIZE)
#define SC_V_FB_DP	8
#define SC_M_FB_DP	(0xF<<SC_V_FB_DP)
#define SC_V_TAG_STAT	2
#define SC_M_TAG_STAT	(0x3F<<SC_V_TAG_STAT)
#define SC_V_FLUSH	1
#define SC_M_FLUSH	(1<<SC_V_FLUSH)
#define SC_V_FHIT	0
#define SC_M_FHIT	(1<<SC_V_FHIT)

/*
**
**  Scache Status Register (SC_STAT) Bit Summary
**
**	 Extent	Size	Name	  Type	Function
**	 ------	----	----	  ----	---------------------------------
**	    <16>  1	SCND_ERR  RO	Second SCache parity Error.
**	 <15:11>  5	CMD	  RO	SCache transaction causing error.
**	  <10:3>  8	DPERR	  RO	Data Parity Error
**	   <2:0>  3	TPERR	  RO	Tag Parity Error
**
*/

#define SC_V_SCND_ERR	16
#define SC_V_CMD	11
#define SC_M_CMD	(0x1F<<SC_V_CMD)
#define SC_V_DPERR	3
#define SC_M_DPERR	(0xFF<<SC_V_DPERR)
#define SC_V_TPERR	0
#define SC_M_TPERR	(7<<SC_V_TPERR)


/*
**
**  External Interface Status Register (EI_STAT) Bit Summary
**
**	 Extent	Size	Name	  	Type	Function
**	 ------	----	----	  	----	---------------------------------
**	    <35>  1	SEO_HRD_ERR	RO
**	    <34>  1	FIL_IRD		RO
**	    <33>  1	EI_PAR_ERR	RO
**	    <32>  1	UNC_ECC_ERR	RO
**	    <31>  1	COR_ECC_ERR	RO
**	    <30>  1	EI_ES		RO
**	    <29>  1	BC_TC_PERR	RO
**	    <28>  1	BC_TPERR	RO
**	 <27:24>  4	CHIP_ID		RO
**
*/

#define EI_V_SEO_HRD_ERR	35
#define EI_V_FIL_IRD		34
#define EI_V_EI_PAR_ERR		33
#define EI_V_UNC_ECC_ERR	32
#define EI_V_COR_ECC_ERR	31
#define EI_V_EI_ES		30
#define EI_V_BC_TC_PERR		29
#define EI_V_BC_TPERR		28
#define EI_V_CHIP_ID		24
#define EI_M_CHIP_ID		(0xF<<EI_V_CHIP_ID)

/*
**
**  Bcache Control Register (BC_CTL) Bit Summary
**
**	 Extent	Size  Name	    Type  Function
**	 ------	----  ----	    ----  ---------------------------------
**	    <27>  1   DIS_VIC_BUF   WO,0  Disable Scache victim buffer
**	    <26>  1   DIS_BAF_BYP   WO,0  Disable speculative Bcache reads
**	    <25>  1   DBG_MUX_SEL   WO,0  Debug MUX select
**	 <24:19>  6   PM_MUX_SEL    WO,0  Performance counter MUX select
**       <18:17>  2   BC_WAVE       WO,0  Number of cycles of wave pipelining
**	    <16>  1   TL_PIPE_LATCH WO,0  Pipe system control pins
**	    <15>  1   EI_DIS_ERR    WO,1  Disable ECC (parity) error
**       <14:13>  2   BC_BAD_DAT    WO,0  Force bad data
**       <12:08>  5   BC_TAG_STAT   WO    Bcache tag status and parity
**           <7>  1   BC_FHIT       WO,0  Bcache force hit
**           <6>  1   EI_ECC        WO,1  ECC or byte parity mode
**           <5>  1   VTM_FIRST     WO,1  Drive out victim block address first
**           <4>  1   CORR_FILL_DAT WO,1  Correct fill data
**           <3>  1   EI_CMD_GRP3   WO,0  Drive MB command to external pins
**           <2>  1   EI_CMD_GRP2   WO,0  Drive LOCK & SET_DIRTY to ext. pins
**           <1>  1   ALLOC_CYC     WO,0  Allocate cycle for non-cached LDs.
**           <0>  1   BC_ENA        W0,0  Bcache enable
**
*/
#define BC_V_DIS_SC_VIC_BUF	27
#define BC_M_DIS_SC_VIC_BUF	(1<<BC_V_DIS_SC_VIC_BUF)
#define BC_V_DIS_BAF_BYP	26
#define BC_M_DIS_BAF_BYP	(1<<BC_V_DIS_BAF_BYP)
#define BC_V_DBG_MUX_SEL	25
#define BC_M_DBG_MUX_SEL	(1<<BC_V_DBG_MUX_SEL)
#define BC_V_PM_MUX_SEL		19
#define BC_M_PM_MUX_SEL		(0x3F<<BC_V_PM_MUX_SEL)
#define BC_V_BC_WAVE		17
#define BC_M_BC_WAVE		(3<<BC_V_BC_WAVE)
#define BC_V_TL_PIPE_LATCH	16
#define BC_M_TL_PIPE_LATCH	(1<<BC_V_TL_PIPE_LATCH)
#define BC_V_EI_DIS_ERR		15
#define BC_M_EI_DIS_ERR		(1<<BC_V_EI_DIS_ERR)
#define BC_V_BC_BAD_DAT		13
#define BC_M_BC_BAD_DAT		(3<<BC_V_BC_BAD_DAT)
#define BC_V_BC_TAG_STAT	8
#define BC_M_BC_TAG_STAT	(0x1F<<BC_V_BC_TAG_STAT)
#define BC_V_BC_FHIT		7
#define BC_M_BC_FHIT		(1<<BC_V_BC_FHIT)
#define BC_V_EI_ECC_OR_PARITY	6
#define BC_M_EI_ECC_OR_PARITY	(1<<BC_V_EI_ECC_OR_PARITY)
#define BC_V_VTM_FIRST		5
#define BC_M_VTM_FIRST		(1<<BC_V_VTM_FIRST)
#define BC_V_CORR_FILL_DAT	4
#define BC_M_CORR_FILL_DAT	(1<<BC_V_CORR_FILL_DAT)
#define BC_V_EI_CMD_GRP3	3
#define BC_M_EI_CMD_GRP3	(1<<BC_V_EI_CMD_GRP3)
#define BC_V_EI_CMD_GRP2	2
#define BC_M_EI_CMD_GRP2	(1<<BC_V_EI_CMD_GRP2)
#define BC_V_ALLOC_CYC		1
#define BC_M_ALLOC_CYC		(1<<BC_V_ALLOC_CYC)
#define BC_V_BC_ENA		0
#define BC_M_BC_ENA		(1<<BC_V_BC_ENA)

#define BC_K_MCHECK \
         ((BC_M_EI_DIS_ERR)       | \
	  (BC_M_CORR_FILL_DAT))

#define BC_K_DFAULT \
	(((BC_M_EI_DIS_ERR)       | \
	  (BC_M_EI_ECC_OR_PARITY) | \
          (BC_M_VTM_FIRST)        | \
	  (BC_M_CORR_FILL_DAT))>>1)
/*
**
**  Bcache Configuration Register (BC_CONFIG) Bit Summary
**
**	 Extent	Size  Name	    Type  Function
**	 ------	----  ----	    ----  ---------------------------------
**	<35:29>   7   RSVD	    WO    Reserved - Must Be Zero
**	<28:20>   9   WE_CTL        WO,0  Bcache write enable control
**	<19:19>   1   RSVD	    WO,0  Reserved - Must Be Zero
**	<18:16>   3   WE_OFF        WO,1  Bcache fill write enable pulse offset
**	<15:15>   1   RSVD          WO,0  Reserved - Must Be Zero
**	<14:12>   3   RD_WR_SPC     WO,7  Bcache private read/write spacing
**	<11:08>   4   WR_SPD        WO,4  Bcache write speed in CPU cycles
**	<07:04>   4   RD_SPD	    WO,4  Bcache read speed in CPU cycles
**	<03:03>   1   RSVD	    WO,0  Reserved - Must Be Zero
**	<02:00>   3   SIZE	    WO,1  Bcache size
*/
#define	BC_V_WE_CTL	20
#define BC_M_WE_CTL	(0x1FF<<BC_V_WE_CTL)
#define BC_V_WE_OFF	16
#define BC_M_WE_OFF	(0x7<<BC_V_WE_OFF)
#define BC_V_RD_WR_SPC	12
#define BC_M_RD_WR_SPC	(0x7<<BC_V_RD_WR_SPC)
#define BC_V_WR_SPD	8
#define BC_M_WR_SPD	(0xF<<BC_V_WR_SPD)
#define BC_V_RD_SPD	4
#define BC_M_RD_SPD	(0xF<<BC_V_RD_SPD)
#define BC_V_SIZE	0
#define BC_M_SIZE	(0x7<<BC_V_SIZE)

#define BC_K_CONFIG \
	((0x1<<BC_V_WE_OFF)    | \
	 (0x7<<BC_V_RD_WR_SPC) | \
	 (0x4<<BC_V_WR_SPD)    | \
	 (0x4<<BC_V_RD_SPD)    | \
	 (0x1<<BC_V_SIZE))

/*
**
**  DECchip 21164 Privileged Architecture Library Entry Offsets:
**
**	Entry Name	    Offset (Hex)
**
**	RESET			0000
**	IACCVIO			0080
**	INTERRUPT	       	0100
**	ITB_MISS		0180
**	DTB_MISS (Single)       0200
**	DTB_MISS (Double)       0280
**	UNALIGN			0300
**	D_FAULT			0380
**	MCHK			0400
**	OPCDEC			0480
**	ARITH			0500
**	FEN			0580
**	CALL_PAL (Privileged)	2000
**	CALL_PAL (Unprivileged)	3000
**
*/

#define PAL_RESET_ENTRY		    0x0000
#define PAL_IACCVIO_ENTRY	    0x0080
#define PAL_INTERRUPT_ENTRY	    0x0100
#define PAL_ITB_MISS_ENTRY	    0x0180
#define PAL_DTB_MISS_ENTRY	    0x0200
#define PAL_DOUBLE_MISS_ENTRY	    0x0280
#define PAL_UNALIGN_ENTRY	    0x0300
#define PAL_D_FAULT_ENTRY	    0x0380
#define PAL_MCHK_ENTRY		    0x0400
#define PAL_OPCDEC_ENTRY	    0x0480
#define PAL_ARITH_ENTRY	    	    0x0500
#define PAL_FEN_ENTRY		    0x0580
#define PAL_CALL_PAL_PRIV_ENTRY	    0x2000
#define PAL_CALL_PAL_UNPRIV_ENTRY   0x3000

/*
**
** Architecturally Reserved Opcode (PALRES) Definitions:
**
*/

#define	mtpr	    hw_mtpr
#define	mfpr	    hw_mfpr

#define	ldl_a	    hw_ldl/a
#define ldq_a	    hw_ldq/a
#define stq_a	    hw_stq/a
#define stl_a	    hw_stl/a

#define ldl_p	    hw_ldl/p
#define ldq_p	    hw_ldq/p
#define stl_p	    hw_stl/p
#define stq_p	    hw_stq/p

/*
** Virtual PTE fetch variants of HW_LD.
*/
#define ld_vpte     hw_ldq/v

/*
** Physical mode load-lock and store-conditional variants of
** HW_LD and HW_ST.
*/

#define ldq_lp	    hw_ldq/pl
#define stq_cp	    hw_stq/pc

/*
**
**  General Purpose Register Definitions:
**
*/

#define	r0		$0
#define r1		$1
#define r2		$2
#define r3		$3
#define r4		$4
#define r5		$5
#define r6		$6
#define r7		$7
#define r8		$8
#define r9		$9
#define r10		$10
#define r11		$11
#define r12		$12
#define r13		$13
#define r14		$14
#define	r15		$15
#define	r16		$16
#define	r17		$17
#define	r18		$18
#define	r19		$19
#define	r20		$20
#define	r21		$21
#define r22		$22
#define r23		$23
#define r24		$24
#define r25		$25
#define r26		$26
#define r27		$27
#define r28		$28
#define r29		$29
#define r30		$30
#define r31		$31

/*
**
** Floating Point Register Definitions:
**
*/

#define	f0		$f0
#define f1		$f1
#define f2		$f2
#define f3		$f3
#define f4		$f4
#define f5		$f5
#define f6		$f6
#define f7		$f7
#define f8		$f8
#define f9		$f9
#define f10		$f10
#define f11		$f11
#define f12		$f12
#define f13		$f13
#define f14		$f14
#define	f15		$f15
#define	f16		$f16
#define	f17		$f17
#define	f18		$f18
#define	f19		$f19
#define	f20		$f20
#define	f21		$f21
#define f22		$f22
#define f23		$f23
#define f24		$f24
#define f25		$f25
#define f26		$f26
#define f27		$f27
#define f28		$f28
#define f29		$f29
#define f30		$f30
#define f31		$f31

/*
**
**  PAL Temporary Register Definitions:
**
*/

#define	pt0		0x140
#define	pt1		0x141
#define	pt2		0x142
#define	pt3		0x143
#define	pt4		0x144
#define	pt5		0x145
#define	pt6		0x146
#define	pt7		0x147
#define	pt8		0x148
#define	pt9		0x149
#define	pt10		0x14A
#define	pt11		0x14B
#define	pt12		0x14C
#define	pt13		0x14D
#define	pt14		0x14E
#define	pt15		0x14F
#define	pt16		0x150
#define	pt17		0x151
#define	pt18		0x152
#define	pt19		0x153
#define	pt20		0x154
#define	pt21		0x155
#define	pt22		0x156
#define	pt23		0x157


/*
**  PAL Shadow Registers:
**
**  The DECchip 21264 shadows r4-r7 and r20-r23 when in PALmode and
**  ICSR<SDE> = 1.
*/

#define	p0		r4
#define p1		r5
#define p2		r6
#define p3		r7
#define p4		r20
#define p5		r21
#define p6		r22
#define p7		r23


/*
** SRM Defined State Definitions:
*/

/*
**  This table is an accounting of the DECchip 21164 storage used to
**  implement the SRM defined state for OSF/1.
**
** 	IPR Name			Internal Storage
**      --------                        ----------------
**	Processor Status		ps, dtbCm, ipl, r11
**	Program Counter			Ibox
**	Interrupt Entry			ptEntInt
**	Arith Trap Entry		ptEntArith
**	MM Fault Entry			ptEntMM
**	Unaligned Access Entry		ptEntUna
**	Instruction Fault Entry		ptEntIF
**	Call System Entry		ptEntSys
**	User Stack Pointer		ptUsp
**	Kernel Stack Pointer		ptKsp
**	Kernel Global Pointer		ptKgp
**	System Value			ptSysVal
**	Page Table Base Register	ptPtbr
**	Virtual Page Table Base		iVptBr, mVptBr
**	Process Control Block Base	ptPcbb
**	Address Space Number		itbAsn, dtbAsn
**	Cycle Counter			cc, ccCtl
**	Float Point Enable		icsr
**	Lock Flag			Cbox/System
**	Unique				PCB
**	Who-Am-I			ptWhami
*/

#define ptEntUna	pt2	/* Unaligned Access Dispatch Entry */
#define ptImpure	pt3	/* Pointer To PAL Scratch Area */
#define ptEntIF		pt7	/* Instruction Fault Dispatch Entry */
#define ptIntMask	pt8	/* Interrupt Enable Mask */
#define ptEntSys	pt9	/* Call System Dispatch Entry */
#define ptTrap          pt11
#define ptEntInt	pt11	/* Hardware Interrupt Dispatch Entry */
#define ptEntArith	pt12	/* Arithmetic Trap Dispatch Entry */
#if defined(KDEBUG)
#define ptEntDbg	pt13	/* Kernel Debugger Dispatch Entry */
#endif /* KDEBUG */
#define ptMisc          pt16    /* Miscellaneous Flags */
#define ptWhami		pt16	/* Who-Am-I Register Pt16<15:8> */
#define ptMces		pt16	/* Machine Check Error Summary Pt16<4:0> */
#define ptSysVal	pt17	/* Per-Processor System Value */
#define ptUsp		pt18	/* User Stack Pointer */
#define ptKsp		pt19	/* Kernel Stack Pointer */
#define ptPtbr		pt20	/* Page Table Base Register */
#define ptEntMM		pt21	/* MM Fault Dispatch Entry */
#define ptKgp		pt22	/* Kernel Global Pointer */
#define ptPcbb		pt23	/* Process Control Block Base */

/*
**
**   Miscellaneous PAL State Flags (ptMisc) Bit Summary
**
**	 Extent	Size  Name	Function
**	 ------	----  ----	---------------------------------
**	 <55:48>  8   SWAP      Swap PALcode flag -- character 'S'
**	 <47:32> 16   MCHK      Machine Check Error code
**	 <31:16> 16   SCB       System Control Block vector
**	 <15:08>  8   WHAMI     Who-Am-I identifier
**       <04:00>  5   MCES      Machine Check Error Summary bits
**
*/

#define PT16_V_MCES	0
#define PT16_V_WHAMI	8
#define PT16_V_SCB	16
#define PT16_V_MCHK	32
#define PT16_V_SWAP	48


/*
**	CPU specific STALL definition.
*/

#define	STALL	bis    r31, r31, r31

#endif /* __DC21264_LOADED */
