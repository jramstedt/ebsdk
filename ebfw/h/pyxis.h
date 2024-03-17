#ifndef __PYXIS_H_LOADED
#define __PYXIS_H_LOADED
/*****************************************************************************

       Copyright 1993, 1994, 1995 Digital Equipment Corporation,
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
 *  $Id: pyxis.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 */

/*
 * $Log: pyxis.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1997/09/22  21:08:07  pbell
 * Added definitions for the power managment status register.
 *
 * Revision 1.2  1997/05/01  20:05:49  pbell
 * *** empty log message ***
 *
 * Initial revision
 *
 *
 */

/*
 ******************************** WARNING *******************************
 *	BE EXTREMELY CAREFUL WHEN USING OFFSETS LARGER THAN 0x8000  WITH
 *	AN "LDA" INSTRUCTION BECAUSE THEY WILL CAUSE AN UNWANTED SIGN 
 *	EXTENSION.
 ******************************** WARNING *******************************
*/

/* ======================================================================
 * =			BASE ADDRESSES					=
 * ======================================================================
 *
 *	Define the base address for registers accessing the memory 
 *	controller
*/
#define MC_GCR_BASE			0x874	/* General Control Register	*/
#define MC_GCR_BASE_SHIFT 	28		/* Shift base by this # of bits */
#define MC_DIAG_BASE		0x874	/* Diagnostic Registers		*/
#define MC_DIAG_BASE_SHIFT 	28	
#define MC_PERFMON_BASE		0x874	/* Performance monitoring regs 	*/
#define MC_PERFMON_BASE_SHIFT 	28	
#define MC_ERROR_BASE		0x874	/* Error Registers		*/
#define MC_ERROR_BASE_SHIFT 28	
#define MC_MCR_BASE			0x875	/* Memory Configuration Registers*/
#define MC_MCR_BASE_SHIFT 	28	
#define MC_PCI_BASE			0x876	/* PCI Configuration Registers	*/
#define MC_PCI_BASE_SHIFT 	28	
#define MC_PM_BASE			0x879	/* Power Management Registers	*/
#define MC_PM_BASE_SHIFT 	28	
#define MC_ICR_BASE			0x87A	/* INT Controller Registers	*/
#define MC_ICR_BASE_SHIFT 	28	


/* ======================================================================
 * =	  OFFSETS FOR THE GENERAL CONTROL REGISTERS (87.4000.0000)	=
 * ======================================================================
*/
#define MC_PYXIS_REV		0x80	/* Revision of PYXIS ASIC.	*/
#define MC_PCI_LAT			0xC0	/* PCI Master Latency Timer.	*/
#define MC_PYXIS_CTRL		0x100	/* PYXIS Control Register.	*/
#define MC_PYXIS_CTRL1		0x140	/* PYXIS Control Register.	*/
#define MC_FLASH_CTRL		0x200	/* PYXIS Control Register.	*/
#define MC_HAE_MEM		0x400   /* HW Address Extension (Sparse)*/
#define MC_HAE_IO		0x440	/* HW Addr. Ext. for sparse IO.	*/ 
#define MC_CFG			0x480	/* Configuration reg for bridges*/


/* ======================================================================
 * =	    OFFSETS FOR THE DIAGNOSTIC REGISTERS (87.4000.0000)		=
 * ======================================================================
*/
#define MC_PYXIS_DIAG		0x2000	/* Diagnostic control register	*/
#define MC_DIAG_CHECK		0x3000	/* Diagnostic check register	*/
#define MC_PYXIS_PM_OFFSET	0x5000	/* Power Management registers   */


/* ======================================================================
 * =   OFFSETS FOR THE PERFORMANCE MONITORING REGISTERS (87.4000.0000)	=
 * ======================================================================
*/
#define MC_PERF_MONITOR		0x4000	/* Performance monitor register	*/
#define MC_PERF_CONTROL		0x4040	/* Perf. Mon. Control register	*/


/* ======================================================================
 * =	      OFFSETS FOR THE ERROR REGISTERS (87.4000.0000)		=
 * ======================================================================
*/
#define MC_PYXIS_ERR	0x8200	/* PYXIS Error Register.		*/
#define MC_PYXIS_STAT	0x8240	/* PYXIS Status Register.		*/
#define MC_ERR_MASK		0x8280	/* PYXIS Error Mask Register.	*/
#define MC_PYXIS_SYN	0x8300	/* PYXIS Syndrome Register.	*/
#define MC_PYXIS_ECSR	0x8308	/* PYXIS Error Data CSR	*/
#define MC_PYXIS_MEAR	0x8400	/* PYXIS Memory Address Register */
#define MC_PYXIS_MESR	0x8440	/* PYXIS Memory Status Register */
#define MC_PCI_ERR0		0x8800	/* PCI Error Status Register 0.	*/
#define MC_PCI_ERR1		0x8840	/* PCI Error Status Register 1.	*/
#define MC_PCI_ERR2		0x8880	/* PCI Error Status Register 2.	*/


/* ======================================================================
 * =   OFFSETS FOR THE MEMORY CONFIGURATION REGISTERS (87.5000.0000)	=
 * ======================================================================
*/
#define MC_MCR			0x000	/* Memory Control Register */
#define MC_MCMR			0x040	/* Memory Clock Mask Register */
#define MC_GTR			0x200	/* Global Timing Register */
#define MC_RTR			0x300	/* Refresh Timing Register */
#define MC_RHPR			0x400	/* Row History Policy Register */
#define MC_MDR1			0x500	/* Memory Controller Debug Register 1 */
#define MC_MDR2			0x540	/* Memory Controller Debug Register 2 */

#define MC_BBA0			0x600	/* Bank Base Address Register 0	*/
#define MC_BBA1			0x640	/* Bank Base Address Register 1	*/
#define MC_BBA2			0x680	/* Bank Base Address Register 2	*/
#define MC_BBA3			0x6C0	/* Bank Base Address Register 3	*/
#define MC_BBA4			0x700	/* Bank Base Address Register 4	*/
#define MC_BBA5			0x740	/* Bank Base Address Register 5	*/
#define MC_BBA6			0x780	/* Bank Base Address Register 6	*/
#define MC_BBA7			0x7C0	/* Bank Base Address Register 7	*/

#define MC_BCR0			0x800	/* Bank Configuration Register 0	*/
#define MC_BCR1			0x840	/* Bank Configuration Register 1	*/
#define MC_BCR2			0x880	/* Bank Configuration Register 2	*/
#define MC_BCR3			0x8C0	/* Bank Configuration Register 3	*/
#define MC_BCR4			0x900	/* Bank Configuration Register 4	*/
#define MC_BCR5			0x940	/* Bank Configuration Register 5	*/
#define MC_BCR6			0x980	/* Bank Configuration Register 6	*/
#define MC_BCR7			0x9C0	/* Bank Configuration Register 7	*/

#define MC_BTR0			0xA00	/* Bank Timing Register 0	*/
#define MC_BTR1			0xA40	/* Bank Timing Register 1	*/
#define MC_BTR2			0xA80	/* Bank Timing Register 2	*/
#define MC_BTR3			0xAC0	/* Bank Timing Register 3	*/
#define MC_BTR4			0xB00	/* Bank Timing Register 4	*/
#define MC_BTR5			0xB40	/* Bank Timing Register 5	*/
#define MC_BTR6			0xB80	/* Bank Timing Register 6	*/
#define MC_BTR7			0xBC0	/* Bank Timing Register 7	*/

#define MC_CVM			0xC00	/* Cache Valid Map Register */


/* ======================================================================
 * =     OFFSETS FOR THE PCI CONFIGURATION REGISTERS (87.6000.0000)	=
 * ======================================================================
*/
#define MC_TBIA			0x100	/* S/G Translation Buffer Inval.*/
#define MC_W0_BASE		0x400	/* Window Base 0.		*/
#define MC_W0_MASK		0x440	/* Window Mask 0.		*/
#define MC_T0_BASE		0x480	/* Translated Base 0.		*/
#define MC_W1_BASE		0x500	/* Window Base 1.		*/
#define MC_W1_MASK		0x540	/* Window Mask 1.		*/
#define MC_T1_BASE		0x580	/* Translated Base 1.		*/
#define MC_W2_BASE		0x600	/* Window Base 2.		*/
#define MC_W2_MASK		0x640	/* Window Mask 2.		*/
#define MC_T2_BASE		0x680	/* Translated Base 2.		*/
#define MC_W3_BASE		0x700	/* Window Base 3.		*/
#define MC_W3_MASK		0x740	/* Window Mask 3.		*/
#define MC_T3_BASE		0x780	/* Translated Base 3.		*/
#define MC_DAC			0x7C0	/* Window DAC Base.		*/
#define MC_LTB_TAG0		0x800	/* Lockable Translation Buffer.	*/
#define MC_LTB_TAG1		0x840	/* Lockable Translation Buffer.	*/
#define MC_LTB_TAG2		0x880	/* Lockable Translation Buffer.	*/
#define MC_LTB_TAG3		0x8C0	/* Lockable Translation Buffer.	*/
#define MC_TB_TAG0		0x900	/* Translation Buffer Tag.	*/
#define MC_TB_TAG1		0x940	/* Translation Buffer Tag.	*/
#define MC_TB_TAG2		0x980	/* Translation Buffer Tag.	*/
#define MC_TB_TAG3		0x9C0	/* Translation Buffer Tag.	*/
#define MC_TB0_PAGE0		0x1000	/* Translation Buffer 0 Page.	*/
#define MC_TB0_PAGE1		0x1040	/* Translation Buffer 0 Page.	*/
#define MC_TB0_PAGE2		0x1080	/* Translation Buffer 0 Page.	*/
#define MC_TB0_PAGE3		0x10C0	/* Translation Buffer 0 Page.	*/
#define MC_TB1_PAGE0		0x1100	/* Translation Buffer 1 Page.	*/
#define MC_TB1_PAGE1		0x1140	/* Translation Buffer 1 Page.	*/
#define MC_TB1_PAGE2		0x1180	/* Translation Buffer 1 Page.	*/
#define MC_TB1_PAGE3		0x11C0	/* Translation Buffer 1 Page.	*/
#define MC_TB2_PAGE0		0x1200	/* Translation Buffer 2 Page.	*/
#define MC_TB2_PAGE1		0x1240	/* Translation Buffer 2 Page.	*/
#define MC_TB2_PAGE2		0x1280	/* Translation Buffer 2 Page.	*/
#define MC_TB2_PAGE3		0x12C0	/* Translation Buffer 2 Page.	*/
#define MC_TB3_PAGE0		0x1300	/* Translation Buffer 3 Page.	*/
#define MC_TB3_PAGE1		0x1340	/* Translation Buffer 3 Page.	*/
#define MC_TB3_PAGE2		0x1380	/* Translation Buffer 3 Page.	*/
#define MC_TB3_PAGE3		0x13C0	/* Translation Buffer 3 Page.	*/
#define MC_TB4_PAGE0		0x1400	/* Translation Buffer 4 Page.	*/
#define MC_TB4_PAGE1		0x1440	/* Translation Buffer 4 Page.	*/
#define MC_TB4_PAGE2		0x1480	/* Translation Buffer 4 Page.	*/
#define MC_TB4_PAGE3		0x14C0	/* Translation Buffer 4 Page.	*/
#define MC_TB5_PAGE0		0x1500	/* Translation Buffer 5 Page.	*/
#define MC_TB5_PAGE1		0x1540	/* Translation Buffer 5 Page.	*/
#define MC_TB5_PAGE2		0x1580	/* Translation Buffer 5 Page.	*/
#define MC_TB5_PAGE3		0x15C0	/* Translation Buffer 5 Page.	*/
#define MC_TB6_PAGE0		0x1600	/* Translation Buffer 6 Page.	*/
#define MC_TB6_PAGE1		0x1640	/* Translation Buffer 6 Page.	*/
#define MC_TB6_PAGE2		0x1680	/* Translation Buffer 6 Page.	*/
#define MC_TB6_PAGE3		0x16C0	/* Translation Buffer 6 Page.	*/
#define MC_TB7_PAGE0		0x1700	/* Translation Buffer 7 Page.	*/
#define MC_TB7_PAGE1		0x1740	/* Translation Buffer 7 Page.	*/
#define MC_TB7_PAGE2		0x1780	/* Translation Buffer 7 Page.	*/
#define MC_TB7_PAGE3		0x17C0	/* Translation Buffer 7 Page.	*/

/* ======================================================================
 * =	  OFFSETS FOR THE MISCELLANEOUS REGISTERS (87.8000.0000)	=
 * ======================================================================
*/
#define MC_CCR			0x000	/* Clock Control Register	*/
#define MC_CLK_STAT		0x100	/* Clock Status Register	*/
#define MC_RESET		0x900	/* Software Reset Register	*/


/* ======================================================================
 * =   		BIT EXTENT FOR PYXIS_CTRL REGISTER (87.4000.0100)		=
 * ======================================================================
*/
#define MC_PYXIS_CTRL_V_PCI_EN		0
#define MC_PYXIS_CTRL_M_PCI_EN 		(1 << MC_PYXIS_CTRL_V_PCI_EN)
#define MC_PYXIS_CTRL_V_PCI_LOOP_EN	2
#define MC_PYXIS_CTRL_M_PCI_LOOP_EN 	(1 << MC_PYXIS_CTRL_V_PCI_LOOP_EN)
#define MC_PYXIS_CTRL_V_FST_BB_EN		3
#define MC_PYXIS_CTRL_M_FST_BB_EN 	(1 << MC_PYXIS_CTRL_V_FST_BB_EN)
#define MC_PYXIS_CTRL_V_PCI_MST_EN	4
#define MC_PYXIS_CTRL_M_PCI_MST_EN 	(1 << MC_PYXIS_CTRL_V_PCI_MST_EN)
#define MC_PYXIS_CTRL_V_PCI_MEM_EN	5
#define MC_PYXIS_CTRL_M_PCI_MEM_EN 	(1 << MC_PYXIS_CTRL_V_PCI_MEM_EN)
#define MC_PYXIS_CTRL_V_PCI_REQ64_EN	6 
#define MC_PYXIS_CTRL_M_PCI_REQ64_EN 	(1 << MC_PYXIS_CTRL_V_PCI_REQ64_EN)
#define MC_PYXIS_CTRL_V_PCI_ACK64_EN	7 
#define MC_PYXIS_CTRL_M_PCI_ACK64_EN 	(1 << MC_PYXIS_CTRL_V_PCI_ACK64_EN)
#define MC_PYXIS_CTRL_V_ADDR_PE_EN	8 
#define MC_PYXIS_CTRL_M_ADDR_PE_EN 	(1 << MC_PYXIS_CTRL_V_ADDR_PE_EN)
#define MC_PYXIS_CTRL_V_PERR_EN		9 
#define MC_PYXIS_CTRL_M_PERR_EN 		(1 << MC_PYXIS_CTRL_V_PERR_EN)
#define MC_PYXIS_CTRL_V_FILLERR_EN	10
#define MC_PYXIS_CTRL_M_FILLERR_EN 	(1 << MC_PYXIS_CTRL_V_FILLERR_EN)
#define MC_PYXIS_CTRL_V_MCHKERR_EN	11
#define MC_PYXIS_CTRL_M_MCHKERR_EN 	(1 << MC_PYXIS_CTRL_V_MCHKERR_EN)
#define MC_PYXIS_CTRL_V_ECC_CHK_EN	12
#define MC_PYXIS_CTRL_M_ECC_CHK_EN 	(1 << MC_PYXIS_CTRL_V_ECC_CHK_EN)
#define MC_PYXIS_CTRL_V_ASSERT_IDLE_BC	13
#define MC_PYXIS_CTRL_M_ASSERT_IDLE_BC 	(1 << MC_PYXIS_CTRL_V_ASSERT_IDLE_BC)
#define MC_PYXIS_CTRL_V_RD_TYPE		20
#define MC_PYXIS_CTRL_M_RD_TYPE 		(3 << MC_PYXIS_CTRL_V_RD_TYPE)
#define MC_PYXIS_CTRL_V_RL_TYPE		24
#define MC_PYXIS_CTRL_M_RL_TYPE 		(3 << MC_PYXIS_CTRL_V_RL_TYPE)
#define MC_PYXIS_CTRL_V_RM_TYPE		28
#define MC_PYXIS_CTRL_M_RM_TYPE 		(3 << MC_PYXIS_CTRL_V_RM_TYPE)


/* ======================================================================
 * =   		BIT EXTENT FOR PYXIS_CTRL1 REGISTER (87.4000.0140)		=
 * ======================================================================
*/
#define MC_PYXIS_CTRL1_V_IOA_BEN		0
#define MC_PYXIS_CTRL1_M_IOA_BEN		(1 << MC_PYXIS_CTRL1_V_IOA_BEN)
#define MC_PYXIS_CTRL1_V_PCI_MWIN_EN	4
#define MC_PYXIS_CTRL1_M_PCI_MWIN_EN	(1 << MC_PYXIS_CTRL1_V_PCI_MWIN_EN)
#define MC_PYXIS_CTRL1_V_PCI_LINK_EN	8
#define MC_PYXIS_CTRL1_M_PCI_LINK_EN	(1 << MC_PYXIS_CTRL1_V_PCI_LINK_EN)
#define MC_PYXIS_CTRL1_V_LW_PAR_MODE	12
#define MC_PYXIS_CTRL1_M_LW_PAR_MODE	(1 << MC_PYXIS_CTRL1_V_LW_PAR_MODE)

/* ======================================================================
 * =   		BIT EXTENT FOR PYXIS_FLASH_CTRL REGISTER (87.4000.0200)		=
 * ======================================================================
*/
#define PYXIS_FLASH_V_WP_WIDTH			0
#define PYXIS_FLASH_V_DISABLE_TIME		4
#define PYXIS_FLASH_V_ACCESS_TIME		8
#define PYXIS_FLASH_V_LOW_ENABLE		12
#define PYXIS_FLASH_V_HIGH_ENABLE		13

/* ======================================================================
 * =   		BIT EXTENT FOR MCR REGISTER (87.5000.0000)		=
 * ======================================================================
*/
#define MC_MCR_V_MODE_REQ			0	/* Mode Register Set command to DIMMS */
#define MC_MCR_M_MODE_REQ			1 << MC_MCR_V_MODE_REQ)
#define MC_MCR_V_SERVER_MODE		8	/* Workstation/Server Mode Status */
#define MC_MCR_M_SERVER_MODE		(1 << MC_MCR_V_SERVER_MODE)
#define MC_MCR_V_BCACHE_STAT		9	/* BCACHE Enable Status */
#define MC_MCR_M_BCACHE_STAT		(1 << MC_MCR_V_BCACHE_STAT)
#define MC_MCR_V_BCACHE_ENABLE		10	/* BCACHE Enable */
#define MC_MCR_M_BCACHE_ENABLE		(1 << MC_MCR_V_BCACHE_ENABLE)
#define MC_MCR_V_PIPELINE_CACHE		11	/* */
#define MC_MCR_M_PIPELINE_CACHE		(1 << MC_MCR_V_PIPELINE_CACHE)
#define MC_MCR_V_OVERLAP_DISABLE	12	/* */
#define MC_MCR_M_OVERLAP_DISABLE	(1 << MC_MCR_V_OVERLAP_DISABLE)
#define MC_MCR_V_SEQ_TRACE			13	/* */
#define MC_MCR_M_SEQ_TRACE			(1 << MC_MCR_V_SEQ_TRACE)
#define MC_MCR_V_CKE_AUTO			14  /* */
#define MC_MCR_M_CKE_AUTO			(1 << MC_MCR_V_CKE_AUTO)
#define MC_MCR_V_DRAM_CLK_AUTO		15	/* */
#define MC_MCR_M_DRAM_CLK_AUTO		(1 << MC_MCR_V_DRAM_CLK_AUTO)
#define MC_MCR_V_DRAM_M0DE			16	/* */
#define MC_MCR_M_DRAM_M0DE			(1 << MC_MCR_V_DRAM_M0DE)
#define MC_MCR_V_DRAM_M0DE_BL		16	/* DRAM Mode Burst Length */
#define MC_MCR_M_DRAM_M0DE_BL		(7 << MC_MCR_V_DRAM_M0DE_BL)
#define MC_MCR_V_DRAM_M0DE_WT		19	/* DRAM Mode Wrap Type */
#define MC_MCR_M_DRAM_M0DE_WT		(1 << MC_MCR_V_DRAM_M0DE_WT)
#define MC_MCR_V_DRAM_M0DE_LM		20	/* DRAM Mode Latency Mode */
#define MC_MCR_M_DRAM_M0DE_LM		(7 << MC_MCR_V_DRAM_M0DE_LM)

/* ======================================================================
 * =   		BIT EXTENT FOR GLOBAL TIMING REGISTER (87.5000.0200)	=
 * ======================================================================
*/
#define MC_GTR_V_MIN_RAS_PRECHARGE	0	/* Minimum RAS precharge */
#define MC_GTR_M_MIN_RAS_PRECHARGE	(7 << MC_GTR_V_MIN_RAS_PRECHARGE)
#define MC_GTR_V_CAS_LATENCY		4	/* CAS Latency */
#define MC_GTR_M_CAS_LATENCY		(3 << MC_GTR_V_CAS_LATENCY)
#define MC_GTR_V_IDLE_BC_WIDTH		8	/* Number of sysClks wait cycles */
#define MC_GTR_M_IDLE_BC_WIDTH		(7 << MC_GTR_V_IDLE_BC_WIDTH)

/* ======================================================================
 * =   		BIT EXTENT FOR REFRESH TIMING REGISTER (87.5000.0300)	=
 * ======================================================================
*/
#define MC_RTR_V_REFRESH_WIDTH		4	/* Refresh Width */
#define MC_RTR_M_REFRESH_WIDTH		(7 << MC_RTR_V_REFRESH_WIDTH)
#define MC_RTR_V_REF_INTERVAL		7	/* Refresh Interval */
#define MC_RTR_M_REF_INTERVAL		(0x3F << MC_RTR_V_REF_INTERVAL)
#define MC_RTR_V_FORCE_REF			15	/* Force Refresh */
#define MC_RTR_M_FORCE_REF			(1 << MC_RTR_V_FORCE_REF)

/* ======================================================================
 * =   		BIT EXTENT FOR BANK CONFIGURATION REGISTERS 			=
 * ======================================================================
*/
#define MC_BCR_V_BANK_ENABLE		0	/* Enables Bank */
#define MC_BCR_M_BANK_ENABLE		(1 << MC_BCR_V_BANK_ENABLE)
#define MC_BCR_V_BANK_SIZE			1	/* Bank Size */
#define MC_BCR_M_BANK_SIZE			(0xF << MC_BCR_V_BANK_SIZE)
#define MC_BCR_V_SUBBANK_ENA		5	/* Subbank Enable */
#define MC_BCR_M_SUBBANK_ENA		(1 << MC_BCR_V_SUBBANK_ENA)
#define MC_BCR_V_ROWSEL				6	/* 12/14 ROW Address Selects */
#define MC_BCR_M_ROWSEL				(1 << MC_BCR_V_ROWSEL)
#define MC_BCR_V_4BANK				7	/* Enable 4 Bank operation */
#define MC_BCR_M_4BANK				(1 << MC_BCR_V_4BANK)


/* ======================================================================
 * =   		BIT EXTENT FOR BANK TIMING REGISTERS 			=
 * ======================================================================
*/
#define MC_BTR_V_ROW_ADDR_HOLD		0	/* */
#define MC_BTR_M_ROW_ADDR_HOLD		(7 << MC_BTR_V_ROW_ADDR_HOLD)
#define MC_BTR_V_TOSHIBA			4	/* Toshiba SDRAMs */
#define MC_BTR_M_TOSHIBA			(1 << MC_BTR_V_TOSHIBA)
#define MC_BTR_V_SLOW_CHARGE		5	/* Slow Precharge */
#define MC_BTR_M_SLOW_CHARGE		(1 << MC_BTR_V_SLOW_CHARGE)


/* ======================================================================
 * =	  OFFSETS FOR POWER MANAGEMENT REGISTERS (87.9000.0000)	=
 * ======================================================================
 */

#define MC_PM_PSR			0x140	/* Power management register */

/* ======================================================================
 * =	  OFFSETS FOR THE INTERRUPT CONTROL REGISTERS (87.A000.0000)	=
 * ======================================================================
 */
#define MC_ICR_INT_REQ		0x00	/* Interrupt Request Register */
#define MC_ICR_INT_MASK		0x40	/* Interrupt Mask Register */
#define MC_ICR_INT_HILO		0xC0	/* Interrupt High/Low IRQ select Register */
#define MC_ICR_INT_ROUTE	0x140	/* Interupt Routing Register */
#define MC_ICR_GPO			0x180	/* General Purpose Register */
#define MC_ICR_INT_CNFG		0x1C0	/* Interrupt Configuration Register */
#define MC_ICR_RT_COUNT		0x200	/* Real Time Couinter */
#define MC_ICR_INT_TIME		0x240	/* Interrupt Time Register */
#define MC_ICR_IIC_CTRL		0x2C0	/* I2C Control Register */


/* ======================================================================
 * =   		BIT EXTENT FOR INTERRUPT ROUTING REGISTERS 			=
 * ======================================================================
*/
#define MC_INT_ROUTE_V_FAN		0	/* */
#define MC_INT_ROUTE_M_FAN		(1 << MC_INT_ROUTE_V_FAN)
#define MC_INT_ROUTE_V_NMI		1	/* */
#define MC_INT_ROUTE_M_NMI		(1 << MC_INT_ROUTE_V_NMI)
#define MC_INT_ROUTE_V_HALT		2	/* */
#define MC_INT_ROUTE_M_HALT		(1 << MC_INT_ROUTE_V_HALT)
#define MC_INT_ROUTE_V_IRQ2		6	/* */
#define MC_INT_ROUTE_M_IRQ2		(1 << MC_INT_ROUTE_V_IRQ2)


/* ======================================================================
 * =   		BIT EXTENT FOR INTERRUPT CONFIGURATION REGISTER 			=
 * ======================================================================
*/
#define INT_CNFG_V_CLOCK_DIVISOR		0	/* */
#define INT_CNFG_V_IRQ_COUNT			4	/* size of the external shift reg */
#define INT_CNFG_V_IRQ_CNFG				8	/* value to drive IRQs on reset   */
#define INT_CNFG_V_DRIVE_IRQ			16	/* use IRQ_CNFG on reset          */


/* ======================================================================
 * =   		BIT EXTENT FOR CLOCK CONTROL REGISTERS 			=
 * ======================================================================
*/
#define MC_CCR_V_CLK_DIVIDE		0	/* */
#define MC_CCR_M_CLK_DIVIDE		(3 << MC_CCR_V_CLK_DIVIDE)
#define MC_CCR_V_PCLK_DIVIDE	4	/* */
#define MC_CCR_M_PCLK_DIVIDE	(7 << MC_CCR_V_PCLK_DIVIDE)
#define MC_CCR_V_PLL_RANGE		8	/* */
#define MC_CCR_M_PLL_RANGE		(3 << MC_CCR_V_PLL_RANGE)
#define MC_CCR_V_LONG_RESET		10	/* */
#define MC_CCR_M_LONG_RESET		(1 << MC_CCR_V_LONG_RESET)
#define MC_CCR_V_CONFIG_SRC		12	/* */
#define MC_CCR_M_CONFIG_SRC		(1 << MC_CCR_V_CONFIG_SRC)
#define MC_CCR_V_DCLK_INV		16	/* */
#define MC_CCR_M_DCLK_INV		(1 << MC_CCR_V_DCLK_INV)
#define MC_CCR_V_DCLK_FORCE		17	/* */
#define MC_CCR_M_DCLK_FORCE		(1 << MC_CCR_V_DCLK_FORCE)
#define MC_CCR_V_DCLK_PCSEL		18	/* */
#define MC_CCR_M_DCLK_PCSEL		(1 << MC_CCR_V_DCLK_PCSEL)
#define MC_CCR_V_DCLK_DELAY		24	/* */
#define MC_CCR_M_DCLK_DELAY		(0xFF << MC_CCR_V_DCLK_DELAY)


#define pyxis_k_main_csr_base 	0x0874	/* PYXIS General Control Register Base*/
#define pyxis_v_main_csr_base 	28	/* Shift base by this # of bits */

/* Offsets from PYXIS control base register. 				*/

#define hae_mem			0x0400    
#define hae_io			0x0440

#define pyxis_err			0x8200
#define pyxis_err_mask		0x8280
#define pyxis_err_v_cor_err	0
#define pyxis_err_v_un_corr_err	1
#define pyxis_err_v_cpu_pe	2
#define pyxis_err_v_mem_nem	3
#define pyxis_err_v_pci_serr	4
#define pyxis_err_v_perr		5
#define pyxis_err_v_pci_addr_pe	6
#define pyxis_err_v_rcvd_mas_abt	7
#define pyxis_err_v_rcvd_tar_abt	8
#define pyxis_err_v_pa_pte_inv	9
#define pyxis_err_v_ioa_timeout	11
#define pyxis_err_v_err_valid	31

/*
**	PYXIS Error Mask Register (ERR_MASK)
*/
#define pyxis_err_mask			0x8280
#define pyxis_err_mask_v_ioa_timeout	11
#define pyxis_err_mask_v_pa_pte_inv	9
#define pyxis_err_mask_v_tar_abt	8
#define pyxis_err_mask_v_mas_abt	7
#define pyxis_err_mask_v_pci_addr_pe	6
#define pyxis_err_mask_v_perr		5
#define pyxis_err_mask_v_pci_serr	4
#define pyxis_err_mask_v_mem_nem	3
#define pyxis_err_mask_v_cpu_pe		2
#define pyxis_err_mask_v_un_cor_err	1
#define pyxis_err_mask_v_cor_err	0

#define pyxis_k_addr_trans	0x876	/* PYXIS's PCI Address Translation Regs*/
#define pyxis_v_addr_trans	28	/* Shift base by this # of bits */

/* Offsets from PCI address translation base register.	Must be < 0x8000 */
/* if they are to be used with LDA instructions!			*/
#define w_base0			0x0400
#define w_mask0			0x0440
#define t_base0			0x0480
#define w_base1			0x0500 
#define w_mask1			0x0540
#define t_base1			0x0580
#define w_base2			0x0600 
#define w_mask2			0x0640
#define t_base2			0x0680
#define w_base3			0x0700 
#define w_mask3			0x0740 
#define t_base3			0x0780

#define w_v_en			0x0
#define w_m_en			(1 << w_v_en)



/* PYXIS Memory Control Register. */
#define mcr			0
#define mba			0x600
#define mba_v_disp		0x7 
#define mba_k_disp		0x80 	/* 1 << mba_v_disp */



/* The following constants define which bits are provided by the HAE_MEM*/
/* register for each of the three sparse regions.			*/
#define hae_sp1_bits		0xE0000000
#define hae_sp2_bits		0xF8000000
#define hae_sp3_bits		0xFC000000

#endif /* __PYXIS_H_LOADED */
