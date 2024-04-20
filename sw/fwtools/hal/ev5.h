#ifndef __EV5_H__
#define __EV5_H__
/*
	Copyright (c) 1994
	Digital Equipment Corporation, Maynard, Massachusetts 01754

	The HAL Alpha Assembler is free software; you can redistribute 
	it and/or modify it with the sole restriction that this copyright 
	notice be attached.

	The information in this software is subject to change without notice 
	and should not be construed as a commitment by Digital Equipment
	Corporation.

	The author disclaims all warranties with regard to this software,
	including all implied warranties of merchantability and fitness, in
	no event shall he be liable for any special, indirect or consequential
	damages or any damages whatsoever resulting from loss of use, data, 
	or profits, whether in an action of contract, negligence or other
	tortious action, arising out of or in connection with the use or
	performance of this software.

*/

/*****************************************************************************/
/* define the IPR's							     */
/*****************************************************************************/
#define MAX_IPR			(4096*2)    /* 12bit valid index for IPR's   */

/* EV4 holdovers */
#define	IPR_ABOX		(1<<6)
#define	IPR_DTB_CTL		(IPR_ABOX | 0)
#define	IPR_MM_CSR		(IPR_ABOX | 4)
#define	IPR_DTBZAP		(IPR_ABOX | 6)
#define	IPR_DTBASM		(IPR_ABOX | 7)
#define	IPR_DC_ADDR		(IPR_ABOX | 11)
#define	IPR_DC_STAT		(IPR_ABOX | 12)
#define	IPR_BIU_STAT		(IPR_ABOX | 10)
#define	IPR_FILL_ADDR		(IPR_ABOX | 13)	
#define	IPR_ABOX_CTL		(IPR_ABOX | 14)
#define	IPR_FILL_SYNDROME	(IPR_ABOX | 19)
#define	IPR_BC_TAG		(IPR_ABOX | 20)
#define	IPR_BIU_CTL		(IPR_ABOX | 18)
#define	IPR_INTR_FLAG		(IPR_ABOX | 22)
#define	IPR_LOCK_FLAG		(IPR_ABOX | 23)

#define	IPR_IBOX		(1<<5)
#define	IPR_ITBZAP		(IPR_IBOX | 6)
#define	IPR_ITBASM		(IPR_IBOX | 7)
#define	IPR_TB_TAG		(IPR_IBOX | 0)
#define	IPR_ICCSR		(IPR_IBOX | 2)
#define	IPR_FLUSH_IC		(IPR_ABOX | 21)
#define	IPR_HIRR		(IPR_IBOX | 12)
#define	IPR_HIER		(IPR_IBOX | 16)
#define	IPR_SIER		(IPR_IBOX | 17)
#define	IPR_SL_CLR		(IPR_IBOX | 19)
#define	IPR_ISSUE_CHK		(IPR_IBOX | 29)
#define	IPR_SINGLE_ISSUE	(IPR_IBOX | 30)
#define	IPR_DUAL_ISSUE		(IPR_IBOX | 31)

/* EV5 Iprs */
#define	IPR_DTB_CM		(0x201)
#define	IPR_DTB_TAG		(0x202)
#define	IPR_DTB_PTE		(0x203)
#define	IPR_DPTE_TEMP		(0x204)
#define	IPR_MM_STAT		(0x205)
#define	IPR_VA 			(0x206)
#define	IPR_DTBIA		(0x20A)
#define	IPR_DTBIS		(0x20B)
#define	IPR_ALT_MODE		(0x20C)
#define	IPR_CC			(0x20D)
#define	IPR_CC_CTL		(0x20E)

#define	IPR_ITB_TAG		(0x101)
#define	IPR_ITB_PTE		(0x102)
#define	IPR_IPTE_TEMP		(0x104)
#define	IPR_ITBIA		(0x105)
#define	IPR_ITBIS		(0x107)
#define	IPR_SIRR		(0x108)
#define	IPR_ASTRR		(0x109)
#define	IPR_ASTER		(0x10A)
#define	IPR_EXC_ADDR		(0x10B)
#define	IPR_EXC_SUM		(0x10C)
#define	IPR_EXC_MASK		(0x10D)
#define	IPR_PAL_BASE		(0x10E)
#define	IPR_PS			(0x10F)
#define IPR_HWINT_CLR		(0x115)
#define	IPR_SL_XMIT		(0x116)
#define	IPR_SL_RCV		(0x117)
#define	IPR_ICSR		(0x118)

/* PAL temps */
#define	IPR_PAL			(0x140)

#define	IPR_PAL_R0		(IPR_PAL  | 0)
#define	IPR_PAL_R1		(IPR_PAL  | 1)
#define	IPR_PAL_R2		(IPR_PAL  | 2)
#define	IPR_PAL_R3		(IPR_PAL  | 3)
#define	IPR_PAL_R4		(IPR_PAL  | 4)
#define	IPR_PAL_R5		(IPR_PAL  | 5)
#define	IPR_PAL_R6		(IPR_PAL  | 6)
#define	IPR_PAL_R7		(IPR_PAL  | 7)
#define	IPR_PAL_R8		(IPR_PAL  | 8)
#define	IPR_PAL_R9		(IPR_PAL  | 9)
#define	IPR_PAL_R10		(IPR_PAL  | 10)
#define	IPR_PAL_R11		(IPR_PAL  | 11)
#define	IPR_PAL_R12		(IPR_PAL  | 12)
#define	IPR_PAL_R13		(IPR_PAL  | 13)
#define	IPR_PAL_R14		(IPR_PAL  | 14)
#define	IPR_PAL_R15		(IPR_PAL  | 15)
#define	IPR_PAL_R16		(IPR_PAL  | 16)
#define	IPR_PAL_R17		(IPR_PAL  | 17)
#define	IPR_PAL_R18		(IPR_PAL  | 18)
#define	IPR_PAL_R19		(IPR_PAL  | 19)
#define	IPR_PAL_R20		(IPR_PAL  | 20)
#define	IPR_PAL_R21		(IPR_PAL  | 21)
#define	IPR_PAL_R22		(IPR_PAL  | 22)
#define	IPR_PAL_R23		(IPR_PAL  | 23)
#define	IPR_PAL_R24		(IPR_PAL  | 24)
#define	IPR_PAL_R25		(IPR_PAL  | 25)
#define	IPR_PAL_R26		(IPR_PAL  | 26)
#define	IPR_PAL_R27		(IPR_PAL  | 27)
#define	IPR_PAL_R28		(IPR_PAL  | 28)
#define	IPR_PAL_R29		(IPR_PAL  | 29)
#define	IPR_PAL_R30		(IPR_PAL  | 30)
#define	IPR_PAL_R31		(IPR_PAL  | 31)

#endif			

