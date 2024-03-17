/*
 *      VID: [2.0] PT: [Thu Aug 21 16:25:43 1997] SF: [impure.h]
 *       TI: [/sae_share/apps/bin/vice -iplatform.s -l// -p# -DEB164 -DLX164 -h -m -Xlint -XDEBUG -XPAL_K_REV -XKDEBUG -XDISABLE_CRD -XDISABLE_MACHINE_CHECKS -XSROM_SERIAL_PORT -XCONSOLE_ENTRY -alx164 ]
 */
#define	__IMPURE_LOADED	    1
/*
*****************************************************************************
**                                                                          *
**  Copyright © 1994							    *
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
**	DECchip 21164 PALcode
**
**  MODULE:
**
**	impure.h
**
**  MODULE DESCRIPTION:
**
**      PAL impure scratch and logout area data structure 
**	definitions
**
**  AUTHOR: ER
**
**  CREATION DATE:  17-Jan-1994
**
**  $Id: impure.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $
**
**  MODIFICATION HISTORY:
**
**  $Log: impure.h,v $
**  Revision 1.1.1.1  1998/12/29 21:36:07  paradis
**  Initial CVS checkin
**
**  Revision 1.15  1997/08/18  18:30:01  fdh
**  Merged in dc21164pc support.
**
**  Revision 1.14  1996/06/24 15:18:17  fdh
**  Added the STORE_IPR macro for storing IPR's without
**  the need for compensating for offset/base address
**  combinations.
**
**  Revision 1.13  1996/06/17  15:57:41  fdh
**  Corrected a few definitions associated with the Logout
**  Frame.
**
**  Added additional error status definitions.
**
**  Revision 1.12  1995/11/07  09:54:58  cruz
**  Added macros SAVE_GPRS and RESTORE_GPRS
**
**  Revision 1.11  1995/04/03  17:30:05  samberg
**  Add bc_cfg_off location
**
**  Revision 1.10  1994/11/30  15:57:17  samberg
**  Use c-style comments for c compiler use
**
**  Revision 1.9  1994/11/22  19:03:05  samberg
**  Make CNS_Q_BASE = 0 for ev4 backward compatibility
**
**  Revision 1.8  1994/07/08  17:02:39  samberg
**  Changes to support platform specific additions
**
**  Revision 1.7  1994/05/25  22:16:02  ericr
**  Added offsets for CBOX IPRs
**
**  Revision 1.6  1994/05/20  19:23:08  ericr
**  Moved SAVE_xPR & RESTORE_xPR macros from osfpal.s to here
**
**  Revision 1.5  1994/05/20  18:08:01  ericr
**  Changed line comments to C++ style comment character
**
**  Revision 1.4  1994/01/26  22:26:53  ericr
**  Added macros to compute base addresses of logout frames
**
**  Revision 1.3  1994/01/26  15:06:36  ericr
**  Combine impure scratch and logout area definitions
**
**  Revision 1.2  1994/01/25  20:33:07  ericr
**  Make shadow register save area relative to a single base offset
**
**  Revision 1.1  1994/01/17  19:35:31  ericr
**  Initial revision
**
**
**--
*/

/*
** Commmon area Offset Definitions:
*/

#define CNS_Q_RCS_ID    0x0
#define CNS_Q_SCRATCH   0xA0

/*
** Processor Saved State Area Offset Definitions:
**
** These offsets are relative to the base of the impure area.
*/

#define CNS_Q_BASE      0x000   /* Define base for debug monitor compatibility */
#define CNS_Q_FLAG	0x100
#define CNS_Q_HALT	0x108
#define CNS_Q_GPR	0x110	/* Offset to base of saved GPR area */
#define CNS_Q_FPR	0x210	/* Offset to base of saved FPR area */
#define CNS_Q_MCHK	0x310
#define CNS_Q_PT	0x318	/* Offset to base of saved PALtemp area */
#define CNS_Q_SHADOW	0x3D8	/* Offset to base of saved PALshadow area */
#define CNS_Q_IPR	0x418	/* Offset to base of saved IPR area */

/*
** Offsets to saved internal processor registers
*/

#define CNS_Q_EXC_ADDR		0x418
#define CNS_Q_PAL_BASE		0x420
#define CNS_Q_MM_STAT		0x428
#define CNS_Q_VA		0x430
#define CNS_Q_ICSR		0x438
#define CNS_Q_IPL		0x440
#define CNS_Q_IPS		0x448
#define CNS_Q_ITB_ASN		0x450
#define CNS_Q_ASTER		0x458
#define CNS_Q_ASTRR		0x460
#define CNS_Q_ISR		0x468
#define CNS_Q_IVPTBR		0x470
#define CNS_Q_MCSR		0x478
#define CNS_Q_DC_MODE		0x480
#define CNS_Q_MAF_MODE		0x488
#define CNS_Q_SIRR		0x490
#define CNS_Q_FPCSR		0x498
#define CNS_Q_ICPERR_STAT	0x4A0
#define CNS_Q_PM_CTR		0x4A8
#define CNS_Q_EXC_SUM		0x4B0
#define CNS_Q_EXC_MASK		0x4B8
#define CNS_Q_INT_ID		0x4C0
#define CNS_Q_DCPERR_STAT	0x4C8
#define CNS_Q_SC_STAT		0x4D0
#define CNS_Q_SC_ADDR		0x4D8
#define CNS_Q_SC_CTL		0x4E0
#define CNS_Q_BC_TAG_ADDR	0x4E8

#define CNS_Q_EI_STAT		0x4F0
#define CNS_Q_EI_ADDR		0x4F8
#define CNS_Q_FILL_SYN		0x500
#define CNS_Q_LD_LOCK		0x508

#define CNS_Q_BC_CTL		0x510
#define CNS_Q_BC_CFG            0x518
#define CNS_Q_PM_CTL		0x520	/* Performance monitor counter */

#define CNS_Q_SROM_REV          0x528
#define CNS_Q_PROC_ID           0x530
#define CNS_Q_MEM_SIZE          0x538
#define CNS_Q_CYCLE_CNT         0x540
#define CNS_Q_SIGNATURE         0x548
#define CNS_Q_PROC_MASK         0x550
#define CNS_Q_SYSCTX            0x558

#define CNS_Q_BC_CFG_OFF        0x560

#define CNS_K_SIZE		0x568

/*
** Macros for saving/restoring data to/from the PAL impure scratch 
** area.  
**
** The console save state area is larger than the addressibility
** of the HW_LD/ST instructions (10-bit signed byte displacement), 
** so some adjustments to the base offsets, as well as the offsets 
** within each base region, are necessary.  
**
** The console save state area is divided into two segments; the 
** CPU-specific segment and the platform-specific segment.  The 
** state that is saved in the CPU-specific segment includes GPRs, 
** FPRs, IPRs, halt code, MCHK flag, etc.  All other state is saved 
** in the platform-specific segment.
**
** The impure pointer will need to be adjusted by a different offset
** value for each region within a given segment.  The SAVE and RESTORE 
** macros will auto-magically adjust the offsets accordingly.
**
*/

#define SAVE_GPR(reg,offset,base) \
 	stq_p	reg, ((offset-0x200)&0x3FF)(base)

#define RESTORE_GPR(reg,offset,base) \
	ldq_p	reg, ((offset-0x200)&0x3FF)(base)

#define SAVE_FPR(reg,offset,base) \
	stt	reg, ((offset-0x200)&0x3FF)(base)

#define RESTORE_FPR(reg,offset,base) \
	ldt	reg, ((offset-0x200)&0x3FF)(base)

#define SAVE_IPR(reg,offset,base) \
	mfpr	v0, reg;	  \
  	stq_p	v0, ((offset-CNS_Q_IPR)&0x3FF)(base)

#define RESTORE_IPR(reg,offset,base) \
  	ldq_p	v0, ((offset-CNS_Q_IPR)&0x3FF)(base); \
	mtpr	v0, reg

#define SAVE_SHADOW(reg,offset,base) \
  	stq_p	reg, ((offset-CNS_Q_IPR)&0x3FF)(base)

#define	RESTORE_SHADOW(reg,offset,base)\
	ldq_p	reg, ((offset-CNS_Q_IPR)&0x3FF)(base)

/*
 *  STORE_IPR doesn't compensate for weird
 *  offset/base combinations.
 */
#define STORE_IPR(reg,offset,base) \
	mfpr	v0, reg;	  \
  	stq_p	v0, ((offset)&0x3FF)(base)

/*
** Macro to save the internal state of the general purpose
** register file.  Note that it switches out of shadow mode
** to save the registers real registers hidden behind the
** shadow registers.
**
** Register Usage Conventions:
**
**      pt0 - Saved v0 (r0)
**      pt4 - Saved t0 (r1)
**      t0 - Base address of the save state area.
**      v0 - scratch.  Will be trashed.
*/
#define SAVE_GPRS                               \
	lda	t0, 0x200(t0);                  \
	mfpr	v0, pt0;                        \
	SAVE_GPR(v0,CNS_Q_GPR+0x00,t0);         \
	mfpr	v0, pt4;                        \
	SAVE_GPR(v0,CNS_Q_GPR+0x08,t0);         \
	SAVE_GPR(r2,CNS_Q_GPR+0x10,t0);         \
	SAVE_GPR(r3,CNS_Q_GPR+0x18,t0);         \
	SAVE_GPR(r4,CNS_Q_GPR+0x20,t0);         \
	SAVE_GPR(r5,CNS_Q_GPR+0x28,t0);         \
 	mfpr	r5, icsr;                       \
	ldah	r4, (1<<(ICSR_V_SDE-16))(zero); \
	bic	r5, r4, r4;                     \
	mtpr	r4, icsr;                       \
	STALL;                                  \
	STALL;                                  \
	STALL;                                  \
	NOP;                                    \
	SAVE_GPR(r6,CNS_Q_GPR+0x30,t0);       \
	SAVE_GPR(r7,CNS_Q_GPR+0x38,t0);       \
	SAVE_GPR(r8,CNS_Q_GPR+0x40,t0);       \
	SAVE_GPR(r9,CNS_Q_GPR+0x48,t0);       \
	SAVE_GPR(r10,CNS_Q_GPR+0x50,t0);      \
	SAVE_GPR(r11,CNS_Q_GPR+0x58,t0);      \
	SAVE_GPR(r12,CNS_Q_GPR+0x60,t0);      \
	SAVE_GPR(r13,CNS_Q_GPR+0x68,t0);      \
	SAVE_GPR(r14,CNS_Q_GPR+0x70,t0);      \
	SAVE_GPR(r15,CNS_Q_GPR+0x78,t0);      \
	SAVE_GPR(r16,CNS_Q_GPR+0x80,t0);      \
	SAVE_GPR(r17,CNS_Q_GPR+0x88,t0);      \
	SAVE_GPR(r18,CNS_Q_GPR+0x90,t0);      \
	SAVE_GPR(r19,CNS_Q_GPR+0x98,t0);      \
	SAVE_GPR(r20,CNS_Q_GPR+0xA0,t0);      \
	SAVE_GPR(r21,CNS_Q_GPR+0xA8,t0);      \
	SAVE_GPR(r22,CNS_Q_GPR+0xB0,t0);      \
	SAVE_GPR(r23,CNS_Q_GPR+0xB8,t0);      \
	SAVE_GPR(r24,CNS_Q_GPR+0xC0,t0);      \
	SAVE_GPR(r25,CNS_Q_GPR+0xC8,t0);      \
	SAVE_GPR(r26,CNS_Q_GPR+0xD0,t0);      \
	SAVE_GPR(r27,CNS_Q_GPR+0xD8,t0);      \
	SAVE_GPR(r28,CNS_Q_GPR+0xE0,t0);      \
	SAVE_GPR(r29,CNS_Q_GPR+0xE8,t0);      \
	SAVE_GPR(r30,CNS_Q_GPR+0xF0,t0);      \
	SAVE_GPR(r31,CNS_Q_GPR+0xF8,t0);      \
	STALL;                                  \
	STALL;                                  \
	mtpr	r5, icsr;                       \
	STALL;                                  \
	STALL;                                  \
	STALL;                                  \
	NOP;                                    \
	lda	t0, -0x200(t0)

/*
** Macro to restore the internal state of the general purpose
** register file.  Note that it switches out of shadow mode
** to save the registers real registers hidden behind the
** shadow registers.
**
** Register Usage Conventions:
**
**      t0 - Base address of the save state area.
**      v0 (r0) and t1 (r2) will be used as scratch. 
**      Warning: Make sure that the base register t0
**      is not restored before we are done using it.
*/
#define RESTORE_GPRS                            \
	lda	t0, 0x200(t0);                  \
	mfpr	v0, icsr;                       \
	ldah	t1, (1<<(ICSR_V_SDE-16))(zero); \
	bic	v0, t1, t1;                     \
	mtpr	t1, icsr;                       \
	STALL;                                  \
	STALL;                                  \
	STALL;                                  \
	NOP;                                    \
	RESTORE_GPR(r2,CNS_Q_GPR+0x10,t0);       \
	RESTORE_GPR(r3,CNS_Q_GPR+0x18,t0);       \
	RESTORE_GPR(r4,CNS_Q_GPR+0x20,t0);       \
	RESTORE_GPR(r5,CNS_Q_GPR+0x28,t0);       \
	RESTORE_GPR(r6,CNS_Q_GPR+0x30,t0);       \
	RESTORE_GPR(r7,CNS_Q_GPR+0x38,t0);       \
	RESTORE_GPR(r8,CNS_Q_GPR+0x40,t0);       \
	RESTORE_GPR(r9,CNS_Q_GPR+0x48,t0);       \
	RESTORE_GPR(r10,CNS_Q_GPR+0x50,t0);      \
	RESTORE_GPR(r11,CNS_Q_GPR+0x58,t0);      \
	RESTORE_GPR(r12,CNS_Q_GPR+0x60,t0);      \
	RESTORE_GPR(r13,CNS_Q_GPR+0x68,t0);      \
	RESTORE_GPR(r14,CNS_Q_GPR+0x70,t0);      \
	RESTORE_GPR(r15,CNS_Q_GPR+0x78,t0);      \
	RESTORE_GPR(r16,CNS_Q_GPR+0x80,t0);      \
	RESTORE_GPR(r17,CNS_Q_GPR+0x88,t0);      \
	RESTORE_GPR(r18,CNS_Q_GPR+0x90,t0);      \
	RESTORE_GPR(r19,CNS_Q_GPR+0x98,t0);      \
	RESTORE_GPR(r20,CNS_Q_GPR+0xA0,t0);      \
	RESTORE_GPR(r21,CNS_Q_GPR+0xA8,t0);      \
	RESTORE_GPR(r22,CNS_Q_GPR+0xB0,t0);      \
	RESTORE_GPR(r23,CNS_Q_GPR+0xB8,t0);      \
	RESTORE_GPR(r24,CNS_Q_GPR+0xC0,t0);      \
	RESTORE_GPR(r25,CNS_Q_GPR+0xC8,t0);      \
	RESTORE_GPR(r26,CNS_Q_GPR+0xD0,t0);      \
	RESTORE_GPR(r27,CNS_Q_GPR+0xD8,t0);      \
	RESTORE_GPR(r28,CNS_Q_GPR+0xE0,t0);      \
	RESTORE_GPR(r29,CNS_Q_GPR+0xE8,t0);      \
	RESTORE_GPR(r30,CNS_Q_GPR+0xF0,t0);      \
	RESTORE_GPR(r31,CNS_Q_GPR+0xF8,t0);      \
	STALL;                                   \
	STALL;                                   \
	mtpr	v0, icsr;                        \
	STALL;                                   \
	STALL;                                   \
	STALL;                                   \
	NOP;                                     \
	RESTORE_GPR(r0,CNS_Q_GPR+0x00,t0);       \
	RESTORE_GPR(r1,CNS_Q_GPR+0x08,t0);




/*
** Short Corrected Error Logout Frame
*/

#define	LAS_Q_BASE		CNS_K_SIZE

#define LAS_L_SIZE		0x0000
#define LAS_L_FLAG		0x0004

#define LAS_Q_OFFSET_BASE	0x0008

#define LAS_L_CPU		0x0008
#define LAS_L_SYS		0x000C

#define LAS_Q_MCHK_CODE		0x0010

#define LAS_Q_CPU_BASE		0x0018

#define LAS_Q_EI_ADDR		0x0018
#define LAS_Q_FILL_SYNDROME	0x0020

#define LAS_Q_EI_STAT		0x0028
#define LAS_Q_ISR		0x0030

#define LAS_Q_SYS_BASE		0x0038

#define LAS_K_SIZE		0x0038

/*
** Long Machine Check Error Logout Frame
*/

#define LAF_Q_BASE		(LAS_Q_BASE+LAS_K_SIZE)

#define LAF_L_SIZE		0x0000
#define LAF_L_FLAG		0x0004

#define LAF_Q_OFFSET_BASE	0x0008

#define LAF_L_CPU		0x0008
#define LAF_L_SYS		0x000C

#define LAF_Q_MCHK_CODE		0x0010
#define LAF_Q_PT		0x0018

#define LAF_Q_CPU_BASE		0x00D8

#define LAF_Q_EXC_ADDR		0x00D8
#define LAF_Q_EXC_SUM		0x00E0
#define LAF_Q_EXC_MASK		0x00E8
#define LAF_Q_PAL_BASE		0x00F0
#define LAF_Q_ISR		0x00F8
#define LAF_Q_ICSR		0x0100
#define LAF_Q_ICPERR		0x0108
#define LAF_Q_DCPERR		0x0110
#define LAF_Q_VA		0x0118
#define LAF_Q_MM_STAT		0x0120
#define LAF_Q_SC_ADDR		0x0128
#define LAF_Q_SC_STAT		0x0130
#define LAF_Q_BC_TAG_ADDR	0x0138
#define LAF_Q_EI_ADDR		0x0140
#define LAF_Q_FILL_SYNDROME	0x0148
#define LAF_Q_EI_STAT		0x0150
#define LAF_Q_LD_LOCK		0x0158

#define LAF_Q_SYS_BASE		0x0160

#define LAF_Q_CPU_ERR0		0x160
#define LAF_Q_CPU_ERR1		0x168
#define LAF_Q_CIA_ERR		0x170
#define LAF_Q_CIA_STAT		0x178
#define LAF_Q_ERR_MASK		0x180
#define LAF_Q_CIA_SYN		0x188
#define LAF_Q_MEM_ERR0		0x190
#define LAF_Q_MEM_ERR1		0x198
#define LAF_Q_PCI_ERR0		0x1A0
#define LAF_Q_PCI_ERR1		0x1A8
#define LAF_Q_PCI_ERR2		0x1B0

#define LAF_K_SIZE		0x01B8
