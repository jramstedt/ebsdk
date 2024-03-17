/* ev4.h */


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
 *	Rev	Who	When		What		
 *	----------------------------------------------------------------------
 *	  0     gpc     AUG-92          First Created
 *	
 */

#ifndef EV4_H
#define EV4_H 1

/* ev4 iprs */
#define	EVX$IPR_IBOX		(1<<5)
#define	EVX$IPR_ABOX		(1<<6)
#define	EVX$IPR_PAL		(1<<7)

#define	EVX$IPR_DTB_CTL		(EVX$IPR_ABOX | 0)
#define	EVX$IPR_DTB_PTE		(EVX$IPR_ABOX | 2)
#define	EVX$IPR_DPTE_TEMP	(EVX$IPR_ABOX | 3)
#define	EVX$IPR_MM_CSR		(EVX$IPR_ABOX | 4)
#define	EVX$IPR_VA		(EVX$IPR_ABOX | 5)

#define	EVX$IPR_ITBZAP		(EVX$IPR_IBOX | 6)
#define	EVX$IPR_DTBZAP		(EVX$IPR_ABOX | 6)
#define	EVX$IPR_ITBASM		(EVX$IPR_IBOX | 7)
#define	EVX$IPR_DTBASM		(EVX$IPR_ABOX | 7)
#define	EVX$IPR_ITBIS		(EVX$IPR_IBOX | 8)
#define	EVX$IPR_DTBIS		(EVX$IPR_ABOX | 8)

#define	EVX$IPR_BIU_ADDR	(EVX$IPR_ABOX | 9)
#define	EVX$IPR_BIU_STAT	(EVX$IPR_ABOX | 10)
#define	EVX$IPR_DC_ADDR		(EVX$IPR_ABOX | 11)
#define	EVX$IPR_DC_STAT		(EVX$IPR_ABOX | 12)
#define	EVX$IPR_FILL_ADDR	(EVX$IPR_ABOX | 13)	
#define	EVX$IPR_ABOX_CTL	(EVX$IPR_ABOX | 14)
#define	EVX$IPR_ALT_MODE	(EVX$IPR_ABOX | 15)
#define	EVX$IPR_CC		(EVX$IPR_ABOX | 16)
#define	EVX$IPR_CC_CTL		(EVX$IPR_ABOX | 17)
#define	EVX$IPR_BIU_CTL		(EVX$IPR_ABOX | 18)
#define	EVX$IPR_FILL_SYNDROME	(EVX$IPR_ABOX | 19)
#define	EVX$IPR_BC_TAG		(EVX$IPR_ABOX | 20)
#define	EVX$IPR_FLUSH_IC	(EVX$IPR_ABOX | 21)
#define	EVX$IPR_FLUSH_IC_ASM	(EVX$IPR_ABOX | 23)

#define	EVX$IPR_INTR_FLAG	(EVX$IPR_ABOX | 22)
#define	EVX$IPR_LOCK_FLAG	(EVX$IPR_ABOX | 24)

#define	EVX$IPR_TB_TAG		(EVX$IPR_IBOX | 0)
#define	EVX$IPR_ITB_PTE		(EVX$IPR_IBOX | 1)
#define	EVX$IPR_ICCSR		(EVX$IPR_IBOX | 2)
#define	EVX$IPR_IPTE_TEMP	(EVX$IPR_IBOX | 3)
#define	EVX$IPR_EXC_ADDR	(EVX$IPR_IBOX | 4)
#define	EVX$IPR_SL_RCV		(EVX$IPR_IBOX | 5)

#define	EVX$IPR_PS		(EVX$IPR_IBOX | 9)
#define	EVX$IPR_EXC_SUM		(EVX$IPR_IBOX | 10)
#define	EVX$IPR_PAL_BASE	(EVX$IPR_IBOX | 11)
#define	EVX$IPR_HIRR		(EVX$IPR_IBOX | 12)
#define	EVX$IPR_SIRR		(EVX$IPR_IBOX | 13)
#define	EVX$IPR_ASTRR		(EVX$IPR_IBOX | 14)
#define	EVX$IPR_HIER		(EVX$IPR_IBOX | 16)
#define	EVX$IPR_SIER		(EVX$IPR_IBOX | 17)
#define	EVX$IPR_ASTER		(EVX$IPR_IBOX | 18)
#define	EVX$IPR_SL_CLR		(EVX$IPR_IBOX | 19)
#define	EVX$IPR_SL_XMIT		(EVX$IPR_IBOX | 22)

#define	EVX$IPR_ISSUE_CHK	(EVX$IPR_IBOX | 29)
#define	EVX$IPR_SINGLE_ISSUE	(EVX$IPR_IBOX | 30)
#define	EVX$IPR_DUAL_ISSUE	(EVX$IPR_IBOX | 31)

#define	EVX$IPR_PAL_R0		(EVX$IPR_PAL  | 0)
#define	EVX$IPR_PAL_R1		(EVX$IPR_PAL  | 1)
#define	EVX$IPR_PAL_R2		(EVX$IPR_PAL  | 2)
#define	EVX$IPR_PAL_R3		(EVX$IPR_PAL  | 3)
#define	EVX$IPR_PAL_R4		(EVX$IPR_PAL  | 4)
#define	EVX$IPR_PAL_R5		(EVX$IPR_PAL  | 5)
#define	EVX$IPR_PAL_R6		(EVX$IPR_PAL  | 6)
#define	EVX$IPR_PAL_R7		(EVX$IPR_PAL  | 7)
#define	EVX$IPR_PAL_R8		(EVX$IPR_PAL  | 8)
#define	EVX$IPR_PAL_R9		(EVX$IPR_PAL  | 9)
#define	EVX$IPR_PAL_R10		(EVX$IPR_PAL  | 10)
#define	EVX$IPR_PAL_R11		(EVX$IPR_PAL  | 11)
#define	EVX$IPR_PAL_R12		(EVX$IPR_PAL  | 12)
#define	EVX$IPR_PAL_R13		(EVX$IPR_PAL  | 13)
#define	EVX$IPR_PAL_R14		(EVX$IPR_PAL  | 14)
#define	EVX$IPR_PAL_R15		(EVX$IPR_PAL  | 15)
#define	EVX$IPR_PAL_R16		(EVX$IPR_PAL  | 16)
#define	EVX$IPR_PAL_R17		(EVX$IPR_PAL  | 17)
#define	EVX$IPR_PAL_R18		(EVX$IPR_PAL  | 18)
#define	EVX$IPR_PAL_R19		(EVX$IPR_PAL  | 19)
#define	EVX$IPR_PAL_R20		(EVX$IPR_PAL  | 20)
#define	EVX$IPR_PAL_R21		(EVX$IPR_PAL  | 21)
#define	EVX$IPR_PAL_R22		(EVX$IPR_PAL  | 22)
#define	EVX$IPR_PAL_R23		(EVX$IPR_PAL  | 23)
#define	EVX$IPR_PAL_R24		(EVX$IPR_PAL  | 24)
#define	EVX$IPR_PAL_R25		(EVX$IPR_PAL  | 25)
#define	EVX$IPR_PAL_R26		(EVX$IPR_PAL  | 26)
#define	EVX$IPR_PAL_R27		(EVX$IPR_PAL  | 27)
#define	EVX$IPR_PAL_R28		(EVX$IPR_PAL  | 28)
#define	EVX$IPR_PAL_R29		(EVX$IPR_PAL  | 29)
#define	EVX$IPR_PAL_R30		(EVX$IPR_PAL  | 30)
#define	EVX$IPR_PAL_R31		(EVX$IPR_PAL  | 31)

#define	EVX$IPR_FP_CTL		(MAX_IPR -1)

#endif
