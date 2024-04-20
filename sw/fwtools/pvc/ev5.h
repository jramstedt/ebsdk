/* ev5.h */

/*
 *	
 *	Copyright (c) 1992
 *	Digital Equipment Corporation, Maynard, Massachusetts 01754
 *	
 *	This software is furnished under a license for use only on a single computer
 *	system  and  may be  copied  only  with the inclusion of the above copyright
 *	notice.  This software, or  any other copies thereof, may not be provided or
 *	otherwise made  available  to any other person except for use on such system
 *	and to one who agrees to these license terms.  Title to and ownership of the
 *	software shall at all times remain in DEC.
 *	
 *	The information  in  this software  is  subject to change without notice and
 *	should not be construed as a commitment by Digital Equipment Corporation.
 *	
 *	DEC  assumes no responsibility for the use or reliability of its software on
 *	equipment which is not supplied by DEC.
 *	
 *	
 *	REVISION HISTORY:		
 *	
 *	Who	When		What		
 *	----------------------------------------------------------------------
 *	JEM	02-Sep-1993	Change some ipr decodes
 *	PIR	16-Dec-1992	Updated to reflect EV5
 *	GPC	29-Aug-1992	Fitted to PVC, deleted unnecessary defines
 *	HA	29-Jun-1992	VA_BITS, PA_BITS
 *	HA	06-May-1992	More IPR's
 *	HA	02-Apr-1992	EV5
 *	HA	20-Nov-1991	IPR's moved from ISPDEF.H to this file	
 *	HA    	06-Nov-1991     First pass
 */

/*
 * define the IPR's
 */

#define MAX_ADU_IPR		(128)

/*EV5*/


#define	IPR_ISR			(0x100)
#define	IPR_ITB_TAG		(0x101)
#define	IPR_ITB_PTE		(0x102)
#define	IPR_ITB_ASN		(0x103)
#define	IPR_ITB_PTE_TEMP	(0x104)
#define	IPR_ITBIA		(0x105)
#define	IPR_ITBIAP		(0x106)
#define	IPR_ITBIS		(0x107)
#define	IPR_SIRR		(0x108)
#define	IPR_ASTRR		(0x109)
#define	IPR_ASTER		(0x10A)
#define	IPR_EXC_ADDR		(0x10B)
#define	IPR_EXC_SUM		(0x10C)
#define	IPR_EXC_MASK		(0x10D)
#define	IPR_PAL_BASE		(0x10E)
#define	IPR_PS			(0x10F)
#define	IPR_IPL			(0x110)
#define	IPR_INTID		(0x111)
#define	IPR_IFAULT_VA_FORM	(0x112)
#define	IPR_IVPTBR		(0x113)
#define	IPR_HWINT_CLR		(0x115)
#define	IPR_SL_XMIT		(0x116)
#define	IPR_SL_RCV		(0x117)
#define	IPR_ICSR		(0x118)
#define	IPR_IC_FLUSH		(0x119)
#define	IPR_IC_PERR_STAT	(0x11A)
#define	IPR_PMCTR		(0x11C)
#define IPR_PALTEMP0		(0x140)
#define IPR_PALTEMP1		(0x141)
#define IPR_PALTEMP2		(0x142)
#define IPR_PALTEMP3		(0x143)
#define IPR_PALTEMP4		(0x144)
#define IPR_PALTEMP5		(0x145)
#define IPR_PALTEMP6		(0x146)
#define IPR_PALTEMP7		(0x147)
#define IPR_PALTEMP8		(0x148)
#define IPR_PALTEMP9		(0x149)
#define IPR_PALTEMPA		(0x14A)
#define IPR_PALTEMPB		(0x14B)
#define IPR_PALTEMPC		(0x14C)
#define IPR_PALTEMPD		(0x14D)
#define IPR_PALTEMPE		(0x14E)
#define IPR_PALTEMPF		(0x14F)
#define IPR_PALTEMP10		(0x150)
#define IPR_PALTEMP11		(0x151)
#define IPR_PALTEMP12		(0x152)
#define IPR_PALTEMP13		(0x153)
#define IPR_PALTEMP14		(0x154)
#define IPR_PALTEMP15		(0x155)
#define IPR_PALTEMP16		(0x156)
#define IPR_PALTEMP17		(0x157)

#define	IPR_DTB_ASN		(0x200)
#define	IPR_DTB_CM		(0x201)
#define	IPR_DTB_TAG		(0x202)
#define	IPR_DTB_PTE		(0x203)
#define	IPR_DTB_PTE_TEMP	(0x204)
#define	IPR_MM_STAT		(0x205)
#define	IPR_VA			(0x206)
#define	IPR_VA_FORM		(0x207)
#define	IPR_MVPTBR		(0x208)
#define	IPR_DTBIAP		(0x209)
#define	IPR_DTBIA		(0x20A)
#define	IPR_DTBIS		(0x20B)
#define	IPR_ALT_MODE		(0x20C)
#define	IPR_CC			(0x20D)
#define	IPR_CC_CTL		(0x20E)
#define	IPR_MCSR		(0x20F)
#define	IPR_DC_FLUSH		(0x210)
#define	IPR_DC_PERR_STAT	(0x212)
#define	IPR_DC_TEST_CTL		(0x213)
#define	IPR_DC_TEST_TAG		(0x214)
#define	IPR_DC_TEST_TAG_TEMP	(0x215)
#define	IPR_DC_MODE		(0x216)
#define	IPR_MAF_MODE		(0x217)



/*REST*/
#define	EVX$IPR_IBOX		(1<<5)
#define	EVX$IPR_ABOX		(1<<6)

#define	EVX$IPR_DTB_CTL		(0x0)

#define	EVX$IPR_MM_CSR		(EVX$IPR_ABOX | 4)

#define	EVX$IPR_BIU_ADDR	(EVX$IPR_ABOX | 9)
#define	EVX$IPR_BIU_STAT	(EVX$IPR_ABOX | 10)
#define	EVX$IPR_DC_ADDR		(EVX$IPR_ABOX | 11)
#define	EVX$IPR_DC_STAT		(EVX$IPR_ABOX | 12)
#define	EVX$IPR_FILL_ADDR	(EVX$IPR_ABOX | 13)	
#define	EVX$IPR_ABOX_CTL	(EVX$IPR_ABOX | 14)
#define	EVX$IPR_BIU_CTL		(EVX$IPR_ABOX | 18)
#define	EVX$IPR_FILL_SYNDROME	(EVX$IPR_ABOX | 19)
#define	EVX$IPR_BC_TAG		(EVX$IPR_ABOX | 20)

#define	EVX$IPR_INTR_FLAG	(EVX$IPR_ABOX | 22)
#define	EVX$IPR_LOCK_FLAG	(EVX$IPR_ABOX | 23)

#define	EVX$IPR_HIRR		(EVX$IPR_IBOX | 12)
#define	EVX$IPR_ASTRR		(EVX$IPR_IBOX | 14)
#define	EVX$IPR_HIER		(EVX$IPR_IBOX | 16)
#define	EVX$IPR_SIER		(EVX$IPR_IBOX | 17)
#define	EVX$IPR_SL_CLR		(EVX$IPR_IBOX | 19)

#define	EVX$IPR_ISSUE_CHK	(EVX$IPR_IBOX | 29)
#define	EVX$IPR_SINGLE_ISSUE	(EVX$IPR_IBOX | 30)
#define	EVX$IPR_DUAL_ISSUE	(EVX$IPR_IBOX | 31)

/**PAL temps */
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

#define	EVX$IPR_FP_CTL		(MAX_IPR -1)

/*
 * define the Implementation specific code
 */

#undef EVX$OPC_HW_MTPR
#undef EVX$OPC_HW_MFPR
#undef EVX$OPC_HW_LD
#undef EVX$OPC_HW_ST
#undef EVX$OPC_HW_REI
#define EVX$OPC_HW_MTPR		(0x1D)
#define EVX$OPC_HW_MFPR		(0x19)
#define EVX$OPC_HW_LD		(0x1B)
#define EVX$OPC_HW_ST		(0x1F)
#define EVX$OPC_HW_REI		(0x1E)
