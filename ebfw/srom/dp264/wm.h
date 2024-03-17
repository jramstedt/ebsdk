#ifndef __WM_H_LOADED
#define __WM_H_LOADED
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
 * $Id: wm.h,v 1.2 1999/01/21 19:06:08 gries Exp $;
 * Author: Franklin Hooker, Digital Equipment Corporation, July 1997
 *
 * $Log: wm.h,v $
 * Revision 1.2  1999/01/21 19:06:08  gries
 * First Release to cvs gries
 *
 * Revision 1.3  1997/09/22  18:14:59  fdh
 * Corrected masks...
 *
 * Revision 1.2  1997/08/13  15:29:07  fdh
 * Added the definition for BC_ENABLE_I_MODE_MASK.
 *
 * Revision 1.1  1997/07/22  13:56:16  fdh
 * Initial revision
 *
 */

/*
** The write_many_a value is based on the table in the spec, with
** bc_enable<0> being the LSB, and with the duplicate bits as shown
** in the table.
**
** When we shift the bits in, they are sampled from the MSB.
**
** We transform the write_many chain to a value we can shift in by
** doing a write to EV6_DATA, shifting our value right 6 bits, doing a
** write to EV6_DATA, etc.
**
** So the following transformation is done on the write_many_a value:
**
** 	<35:30>|<29:24>|<23:18>|<17:12>|<11:06>|<05:00> =>
** 	<05:00>|<11:06>|<17;12>|<23:18>|<29:24>|<35:30>
**
**	eg.
**	c_reg_ipr->write_many_a = 0x043bef8c5
**	value in data register  = 0x163bef0c1
**
**---------------------------------------------------------------
** <30>	pad_raw_bc_size_a<1>			<00> 1	1
** <31>	init_mode_2_a<0>			<01> 0
** <32>	bc_wrt_sts_a<3>				<02> 0
** <33>	bc_wrt_sts_a<2>				<03> 0
**---------------------------------------------------------------
** <34>	bc_wrt_sts_a<1>				<04> 0	C
** <35>	bc_wrt_sts_a<0>				<05> 0
** <24>	set_dirty_enable_a<1>			<06> 1
** <25>	set_dirty_enable_a<2>			<07> 1
**---------------------------------------------------------------
** <26>	bc_bank_enable_a			<08> 0	0
** <27>	pad_raw_bc_size_a<4>			<09> 0
** <28>	pad_raw_bc_size_a<3>			<10> 0
** <29>	pad_raw_bc_size_a<2>			<11> 0
**---------------------------------------------------------------
** <18>	zeroblk_enable_a<0>			<12> 1	F
** <19>	bc_enable_a				<13> 1
** <20>	bc_enable_a				<14> 1
** <21>	bc_enable_a				<15> 1
**---------------------------------------------------------------
** <22>	set_dirty_enable_a<0>			<16> 0	E
** <23>	zeroblk_enable_a<0>			<17> 1
** <12>	bc_enable_a				<18> 1
** <13>	bc_enable_a				<19> 1
**---------------------------------------------------------------
** <14>	bc_enable_a				<20> 1	B
** <15>	zeroblk_enable_a<1>			<21> 1
** <16>	enable_evict_a				<22> 0
** <17>	bc_enable_a				<23> 1
**---------------------------------------------------------------
** <6>	bc_enable_a				<24> 1	3
** <7>	bc_enable_a				<25> 1
** <8>	raw_bc_size_a<4>			<26> 0
** <9>	raw_bc_size_a<3>			<27> 0
**---------------------------------------------------------------
** <10>	raw_bc_size_a<2>			<28> 0	6
** <11>	raw_bc_size_a<1>			<29> 1
** <0>	bc_enable_a				<30> 1
** <1>	init_mode_a				<31> 0
**---------------------------------------------------------------
** <2>	raw_bc_size_a<1>			<32> 1	1
** <3>	raw_bc_size_a<2>			<33> 0
** <4>	raw_bc_size_a<3>			<34> 0
** <5>	raw_bc_size_a<4>			<35> 0
**---------------------------------------------------------------
**
**                                      |3 3 3 3|3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1                     
**                                      |5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0|
**                                      +=======+=======+=======+=======+=======+=======+=======+=======+=======+
** BC_ENABLE_MASK           0.439ce000  | | | | | |1| | | | |1|1|1| | |1|1|1| | |1|1|1| | | | | | | | | | | | | |
** INIT_MODE_MASK           0.80000002  | | | | |1| | | | | | | | | | | | | | | | | | | | | | | | | | | | | |1| |
** BC_SIZE1_MASK            1.20000001  | | | |1| | |1| | | | | | | | | | | | | | | | | | | | | | | | | | | | |1|
** BC_SIZE2_MASK            2.10000800  | | |1| | | | |1| | | | | | | | | | | | | | | | |1| | | | | | | | | | | |
** BC_SIZE3_MASK            4.08000400  | |1| | | | | | |1| | | | | | | | | | | | | | | | |1| | | | | | | | | | |
** BC_SIZE4_MASK            8.04000200  |1| | | | | | | | |1| | | | | | | | | | | | | | | | |1| | | | | | | | | |
** ZEROBLK_ENABLE0_MASK     0.00021000  | | | | | | | | | | | | | | | | | | |1| | | | |1| | | | | | | | | | | | |
** ZEROBLK_ENABLE1_MASK     0.00200000  | | | | | | | | | | | | | | |1| | | | | | | | | | | | | | | | | | | | | |
** ENABLE_EVICT_MASK        0.00400000  | | | | | | | | | | | | | |1| | | | | | | | | | | | | | | | | | | | | | |
** SET_DIRTY_ENABLE0_MASK   0.00010000  | | | | | | | | | | | | | | | | | | | |1| | | | | | | | | | | | | | | | |
** SET_DIRTY_ENABLE1_MASK   0.00000040  | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |1| | | | | | |
** SET_DIRTY_ENABLE2_MASK   0.00000080  | | | | | | | | | | | | | | | | | | | | | | | | | | | | |1| | | | | | | |
** BC_BANK_ENABLE_MASK      0.00000100  | | | | | | | | | | | | | | | | | | | | | | | | | | | |1| | | | | | | | |
** BC_WRT_STS0_MASK         0.00000020  | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |1| | | | | |
** BC_WRT_STS1_MASK         0.00000010  | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |1| | | | |
** BC_WRT_STS2_MASK         0.00000008  | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |1| | | |
** BC_WRT_STS3_MASK         0.00000004  | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |1| | |
**                                      +=======+=======+=======+=======+=======+=======+=======+=======+=======+
** BC_ENABLE_I_MODE_MASK    0.001C0000  | | | | | | | | | | | | | | | |1|1|1| | | | | | | | | | | | | | | | | | |
*/

#define BC_ENABLE_MASK           0x439ce000
#define INIT_MODE_MASK           0x80000002
#define BC_SIZE1_MASK            0x20000001
#define BC_SIZE2_MASK            0x10000800
#define BC_SIZE3_MASK            0x08000400
#define BC_SIZE4_MASK            0x04000200
#define ZEROBLK_ENABLE0_MASK     0x00021000
#define ZEROBLK_ENABLE1_MASK     0x00200000
#define ENABLE_EVICT_MASK        0x00400000
#define SET_DIRTY_ENABLE0_MASK   0x00010000
#define SET_DIRTY_ENABLE1_MASK   0x00000040
#define SET_DIRTY_ENABLE2_MASK   0x00000080
#define BC_BANK_ENABLE_MASK      0x00000100
#define BC_WRT_STS0_MASK         0x00000020
#define BC_WRT_STS1_MASK         0x00000010
#define BC_WRT_STS2_MASK         0x00000008
#define BC_WRT_STS3_MASK         0x00000004

#define BC_ENABLE_I_MODE_MASK	 0x001C0000

#define BC_SIZE1_MASK_H		 0x1
#define BC_SIZE2_MASK_H		 0x2
#define BC_SIZE3_MASK_H		 0x4
#define BC_SIZE4_MASK_H		 0x8

#define MERGE_BIT(mask, signal, offset) ((((signal)>>(offset)) & 1) * (mask))

#define WM_BC_ENABLE(Value) \
	MERGE_BIT(BC_ENABLE_MASK,		Value,	0)

#define WM_INIT_MODE(Value) \
	MERGE_BIT(INIT_MODE_MASK,		Value,	0)

#define WM_BC_SIZE1(Value) \
	MERGE_BIT(BC_SIZE1_MASK,		Value,	0)
#define WM_BC_SIZE2(Value) \
	MERGE_BIT(BC_SIZE2_MASK,		Value,	1)
#define WM_BC_SIZE3(Value) \
	MERGE_BIT(BC_SIZE3_MASK,		Value,	2)
#define WM_BC_SIZE4(Value) \
	MERGE_BIT(BC_SIZE4_MASK,		Value,	3)

#define WM_BC_SIZE(Value) \
	WM_BC_SIZE1(Value) | \
	WM_BC_SIZE2(Value) | \
	WM_BC_SIZE3(Value) | \
	WM_BC_SIZE4(Value)

#define WM_BC_SIZE1_H(Value) \
	MERGE_BIT(BC_SIZE1_MASK_H,		Value,	0)
#define WM_BC_SIZE2_H(Value) \
	MERGE_BIT(BC_SIZE2_MASK_H,		Value,	1)
#define WM_BC_SIZE3_H(Value) \
	MERGE_BIT(BC_SIZE3_MASK_H,		Value,	2)
#define WM_BC_SIZE4_H(Value) \
	MERGE_BIT(BC_SIZE4_MASK_H,		Value,	3)

#define WM_BC_SIZE_H(Value) \
	WM_BC_SIZE1_H(Value) | \
	WM_BC_SIZE2_H(Value) | \
	WM_BC_SIZE3_H(Value) | \
	WM_BC_SIZE4_H(Value)

#define WM_ZEROBLK_ENABLE0(Value) \
	MERGE_BIT(ZEROBLK_ENABLE0_MASK,		Value,	0)
#define WM_ZEROBLK_ENABLE1(Value) \
	MERGE_BIT(ZEROBLK_ENABLE1_MASK,		Value,	1)

#define WM_ZEROBLK_ENABLE(Value) \
	WM_ZEROBLK_ENABLE0(Value) | \
	WM_ZEROBLK_ENABLE1(Value)

#define WM_ENABLE_EVICT(Value) \
	MERGE_BIT(ENABLE_EVICT_MASK,		Value,	0)

#define WM_SET_DIRTY_ENABLE0(Value) \
	MERGE_BIT(SET_DIRTY_ENABLE0_MASK,	Value,	0)
#define WM_SET_DIRTY_ENABLE1(Value) \
	MERGE_BIT(SET_DIRTY_ENABLE1_MASK,	Value,	1)
#define WM_SET_DIRTY_ENABLE2(Value) \
	MERGE_BIT(SET_DIRTY_ENABLE2_MASK,	Value,	2)

#define WM_SET_DIRTY_ENABLE(Value) \
	WM_SET_DIRTY_ENABLE0(Value) | \
	WM_SET_DIRTY_ENABLE1(Value) | \
	WM_SET_DIRTY_ENABLE2(Value)

#define WM_BC_BANK_ENABLE(Value) \
	MERGE_BIT(BC_BANK_ENABLE_MASK,		Value,	0)

#define WM_BC_WRT_STS0(Value) \
	MERGE_BIT(BC_WRT_STS0_MASK,		Value,	0)
#define WM_BC_WRT_STS1(Value) \
	MERGE_BIT(BC_WRT_STS1_MASK,		Value,	1)
#define WM_BC_WRT_STS2(Value) \
	MERGE_BIT(BC_WRT_STS2_MASK,		Value,	2)
#define WM_BC_WRT_STS3(Value) \
	MERGE_BIT(BC_WRT_STS3_MASK,		Value,	3)

#define WM_BC_WRT_STS(Value) \
	WM_BC_WRT_STS0(Value) | \
	WM_BC_WRT_STS1(Value) | \
	WM_BC_WRT_STS2(Value) | \
	WM_BC_WRT_STS3(Value)

#define WRITE_MANY_CHAIN_L	\
	WM_BC_ENABLE(bc_enable_a)		| \
	WM_INIT_MODE(init_mode_a)		| \
	WM_BC_SIZE(bc_size_a)			| \
	WM_ZEROBLK_ENABLE(zeroblk_enable_a)	| \
	WM_ENABLE_EVICT(enable_evict_a)		| \
	WM_SET_DIRTY_ENABLE(set_dirty_enable_a)	| \
	WM_BC_BANK_ENABLE(bc_bank_enable_a)	| \
	WM_BC_WRT_STS(bc_wrt_sts_a)


#define WRITE_MANY_CHAIN_H \
	WM_BC_SIZE_H(bc_size_a)

#endif /* __WM_H_LOADED */
