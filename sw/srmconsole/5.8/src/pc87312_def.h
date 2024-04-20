/* File:	pc87312_def.h
 *
 * Copyright (C) 1993 by
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
 * Abstract:	pc87312 COMBO CHIP header definitions 
 *
 * Author:	D.W. Neale
 *
 * Modifications:
 *
 *	dwn	16-June-1993	Initial Entry
 */

/* LOCAL SYMBOLS and Macro definitions
*/



/* COMBO CONFIGURATION definitions
/*
/* 	Register offsets 
*/
#define COMBO_INDEX	0x398	/* Configuration Index    Base Address */
#define COMBO_DATA	0x399	/* Configuration Data     Base Address */
#define COMBO_NumReg	3	/* */

#define COMBO_FER	0x00	/* Function Enable  Register */
#define COMBO_FAR	0x01	/* Function Address Register */
#define COMBO_PTR	0x02	/* Power and Test   Register */

#define COMBO_LOCKCONFIG 0x40	/* PTR[6] Lock Configuration Registers */

/* 	Mask Definitions
/*    		RW   bits have an associated 1
/*    		RO   bits have an associated 0
/*    		rsvd bits have an associated 0
*/
#define COMBO_MFER	0xFF
#define COMBO_MFAR	0xFF
#define COMBO_MPTR	0x3F

/* 	Init Values 
*/
#define COMBO_IFER	0x4F	/* FDC, UART1, UART2, PORT */
#define COMBO_IFAR	0x11	/* */
#define COMBO_IPTR	0x00	/* Power Clock Option         */

/* 	Default Program values 
*/
#define COMBO_DFER	0x0F	/* FDC, UART1, UART2, PORT */
#define COMBO_DFAR	0x11	/* PORT=0x3Bc, UART1=0x3F8, UART2=0x2F8, */
#define COMBO_DPTR	0x00	/* */



/* SERIAL PORT definitions
/*
/* 	Register offsets 
*/
#define COMBO_UART0	0x3F8	/* Serial Port 0 Base Address */
#define COMBO_UART1 	0x2F8	/* Serial Port 1 Base Address */
#define COMBO_UART_INC 	0x100	/* */

#define UART_RBR	0x00	/* READ  Receive Buffer */
#define UART_THR	0x00	/* WRITE Transmit Holding */
#define UART_IER	0x01	/* WR    Interrupt Enable */
#define UART_IIR	0x02	/* READ  Interrupt Identification */
#define UART_FCR	0x02	/* WRITE FIFO Control */
#define UART_LCR	0x03	/* WR    Line Control */
#define UART_MCR	0x04	/* WR    MODEM Control */
#define UART_LSR	0x05	/* RO    Line Status */
#define UART_MSR	0x06	/* RO    Modem Status */
#define UART_SCR	0x07	/* WR    Scratch */

/* Programmable 16 bit Baud Rate Generator via
/*	Divisor Latch lsb Reg, DLL
/*	Divisor Latch msb Reg, DLM
/*
/* The Divisor Latch Registers are accessed by:
/*	1. First settting LCR<7>
/*	2. Addressing offset 0 for DLL
/*	3. Addressing offset 1 for DLH
/*
*/
#define UART_DLL	0x00	/* WR Divisor Latch LSB */
#define UART_DLM	0x01	/* WR Divisor Latch MSB */


/* 	Default Program values 
*/
#define UART_DIER	0x00	/* Disable Interrupts */
#define UART_DFCR	0x07	/* FIFO Control             WRITE */
#define UART_DLCR	0x1F	/* Line Control */
#define UART_DMCR	0x00	/* MODEM Status */

/* Programmable 16 bit Baud Rate Generator default 
/*	DLM==15:8,7:0==DLL
/*
/* 	Buad Rates range from ie:
/*	1. 56000 <DLM,DLL> = 2
/*	2. 19200 <DLM,DLL> = 6
/*	3. 110   <DLM,DLL> = 1047d, 417h
/*	4. 50    <DLM,DLL> = 2304d, 900h
/*
*/
#define UART_DDLL	0x0C	/* 9600 Baud */
#define UART_DDLM	0x00	/*           */


/* Serial Port Program Masks and Vectors
*/
#define	UART_TO		1024*8	/* Averaged ~400 * 4 == 1600 */

#define	UART$M_LPBCK	0x10	/* MCR<4> */
#define	UART$M_IRQ	0x08	/* MCR<3> */
#define	UART$M_OUT1	0x04	/* MCR<2> */
#define	UART$M_RTS	0x02	/* MCR<1> */
#define	UART$M_DTR	0x01	/* MCR<0> */

#define	UART$M_DCD	0x80	/* MSR<7> */
#define	UART$M_RI	0x40	/* MSR<6> */
#define	UART$M_DSR	0x20	/* MSR<5> */
#define	UART$M_CTS	0x10	/* MSR<4> */

#define	UART$M_DDCD	0x08	/* MSR<3> */
#define	UART$M_TERI	0x04	/* MSR<2> */
#define	UART$M_DDSR	0x02	/* MSR<1> */
#define	UART$M_DCTS	0x01	/* MSR<0> */

/* See PC87312 Spec Table 6-10, pg 58 for details
*/
#define UART$V_NoInt	0x01	/* IIR<3:0> No Interrupt pending */
#define UART$V_LSR	0x06	/* IIR<3:0> Receiver Line Status */
#define UART$V_RcvDat	0x04	/* IIR<3:0> Receive Data Available */
#define UART$V_CharTo	0x0C	/* IIR<3:0> Charater Timeout */
#define UART$V_XmtEmpty	0x02	/* IIR<3:0> Transmitter Holding Reg empty */
#define UART$V_MSR	0x00	/* IIR<3:0> Modem Status Register Interrupt */

#define UART$M_DR	0x01	/* LSR <0> Receive Data Ready */
#define UART$M_OE	0x02	/* LSR <1> Overrun Error */
#define UART$M_PE	0x04	/* LSR <2> Parity Error */
#define UART$M_FE	0x08	/* LSR <3> Framing Error */
#define UART$M_BI	0x10	/* LSR <4> Break Interrupt */
#define UART$M_THRE	0x20	/* LSR <5> Transmitter Holding Ref Empty */
#define UART$M_TEMT	0x40	/* LSR <6> Transmitter Empty */
#define UART$M_FIFO	0x80	/* LSR <7> FIFO PE, FE, BI */


/* Printer Port Configuration definitions
/*
/* 	Register offsets 
*/
#define COMBO_PNTR 	0x3BC		/* Printer Port Base Address */

#define PAR_TO		1024*8	/* Averaged ~400 * 4 == 1600 */

#define PNTR_DTR  (COMBO_PNTR+0x00)	/* Data    Register */
#define PNTR_STR  (COMBO_PNTR+0x01)	/* Status  Register */
#define PNTR_CTR  (COMBO_PNTR+0x02)	/* Control Register */

/* 	Mask Definitions
/*    		RW   bits have an associated 1
/*    		RO   bits have an associated 0
/*    		rsvd bits have an associated 0
*/
#define PNTR_MDTR	 0xFF
#define PNTR_MSTR	 0x00	/* RO */
#define PNTR_MCTR	 0x1F

/* 	Init Values 
*/
#define PNTR_IDTR	 0x00
#define PNTR_ISTR	 0xFF
#define PNTR_ICTR	 0xE0

/* 	Default Program values 
*/
#define PNTR_DDTR	 0x00
#define PNTR_DSTR	 0x00	/* RO */
#define PNTR_DCTR	 0x00


/* Printer Port Masks and Vectors
*/
#define	PNTR$M_STB	0x01	/* CTR<0> Data Strobe */
#define	PNTR$M_AFD 	0x02	/* CTR<1> Auto Feed */
#define	PNTR$M_INIT 	0x04	/* CTR<2> Initialize */
#define	PNTR$M_SLIN 	0x08	/* CTR<3> Select In */
#define	PNTR$M_IE	0x10	/* CTR<4> Interrupts Enable IRQ5-7 */
#define	PNTR$M_POE 	0x20	/* CTR<5> Printer Port Output Enable */

#define	PNTR$M_IRQ 	0x04	/* STR<2> "IRQ5-5 Status" or "POE" */
#define	PNTR$M_ERR 	0x80	/* STR<3> Printer Error signal */
#define	PNTR$M_SLCT 	0x10	/* STR<4> Printer selected */
#define	PNTR$M_PE 	0x20	/* STR<5> Printer out of paper */
#define	PNTR$M_ACK 	0x40	/* STR<6> Data Received */
#define	PNTR$M_BUSY 	0x80	/* STR<7> Can't accept data */


/* FDC Floppy Disk Controller definitions
/*	Sable's FDC will be configured in PC-AT Mode, 
/*	ie IDENT=1 MFM=NC, see COMBO spec pg9.
/*
/* 	Register offsets 
*/
#define COMBO_FDC	0x3F0	/* Floppy Disk Controller Base Address */
#define FDC_VERSION 	0x10	/* Version Command Results byte */

/* Sable IO configures the FDC in PC-AT Mode making
/* the SRA and SRB registers undefined but are included
/* here for completeness.
*/
#define FDC_SRA  (COMBO_FDC+0x00)	/* Status Register A */
#define FDC_SRB	 (COMBO_FDC+0x01)	/* Status Register B */

#define FDC_DOR	 (COMBO_FDC+0x02)	/* Digital Output Register */
#define FDC_TDR	 (COMBO_FDC+0x03)	/* Tape Drive Register */
#define FDC_MSR	 (COMBO_FDC+0x04)	/* Main Status REgister */
#define FDC_DSR	 (COMBO_FDC+0x04)	/* Data Rate Select Register */
#define FDC_FIFO (COMBO_FDC+0x05)	/* Data Register FIFO */
#define FDC_RSVD (COMBO_FDC+0x06)	/* rsvd */
#define FDC_DIR	 (COMBO_FDC+0x07)	/* Digital Input REgister */
#define FDC_CCR	 (COMBO_FDC+0x07)	/* Configuration Control Register */

/* 	Register Bit Mask Definitions
/*    		RW   bits have an associated 1
/*    		RO   bits have an associated 0
/*    		rsvd bits have an associated 0
*/
#define FDC_MSRA	0x00	/* not present in PC-AT mode */
#define FDC_MSRB	0x00	/* not present in PC-AT mode */

#define FDC_MDOR	0xFF
#define FDC_MTDR	0x03
#define FDC_MMSR	0x00	/* RO */
#define FDC_MDSR	0x00	/* WO */
#define FDC_MFIFO 	0x00
#define FDC_MDIR	0x00	/* RO */
#define FDC_MCCR	0x00	/* WO */

/* 	Init Values 
*/
#define FDC_ISRA	0x00	/* not present in PC-AT mode */
#define FDC_ISRB	0x00	/* not present in PC-AT mode */
#define FDC_IDOR	0x00
#define FDC_ITDR	0x00
#define FDC_IMSR	0x00
#define FDC_IDSR	0x00	/* WO */
#define FDC_IFIFO 	0x00
#define FDC_IDIR	0x00
#define FDC_ICCR	0x00	/* WO */

/* 	Default Program values 
*/
#define FDC_DSRA	0x00	/* not present in PC-AT mode */
#define FDC_DSRB	0x00	/* not present in PC-AT mode */
#define FDC_DDOR	0x04
#define FDC_DTDR	0x00
#define FDC_DMSR	0x00	/* RO */
#define FDC_DDSR	0x02	/* */
#define FDC_DFIFO 	0x00
#define FDC_DDIR	0x00	/* RO */
#define FDC_DCCR	0x02


/* FDC Command Opcode Masks
*/
#define FDC$M_MT	0x80	/* Command Opcode bit 7 */
#define FDC$M_LOCK	0x80	/* Command Opcode bit 7 */
#define FDC$M_MFM	0x40	/* Command Opcode bit 6 */
#define FDC$M_DIR	0x40	/* Command Opcode bit 6 */
#define FDC$M_WNR	0x40	/* Command Opcode bit 6 */
#define FDC$M_SK	0x20	/* Command Opcode bit 4 */

/* 	FDC Command Opcodes
*/
#define FDC$M_Config		0x13	/* Configure */
#define FDC$M_DumpReg		0x0E	/* Dump REgisters */
#define FDC$M_FmtTrk		0x0D	/* Format Track */
#define FDC$M_Lock		0x14	/* Lock */
#define FDC$M_Mode		0x01	/* Mode */
#define FDC$M_Nsc		0x18	/* NSC */
#define FDC$M_PrdicMode		0x12	/* Perpendicular Mode */
#define FDC$M_Read		0x06	/* Read Data */
#define FDC$M_RdDel		0x0C	/* Read Deleted Data */
#define FDC$M_RdID		0x0A	/* Read ID */
#define FDC$M_RdTrk		0x02	/* Read A Track */
#define FDC$M_Recal		0x07	/* Recalibrate */
#define FDC$M_RelSk		0x8F	/* Relative Seek */
#define FDC$M_ScnEQ		0x11	/* Scan Equal */
#define FDC$M_ScnHEQ		0x1D	/* Scan High or Equal */
#define FDC$M_ScnLEQ		0x19	/* Scan Low or Equal */
#define FDC$M_Seek		0x0F	/* Seek */
#define FDC$M_SnsDrvStat	0x04	/* Sense Drive Status */
#define FDC$M_SnsInter		0x08	/* Sense Interrupt */
#define FDC$M_SetTrk		0x21	/* Set Track */
#define FDC$M_Specify		0x03	/* Specify */
#define FDC$M_Verify		0x16	/* Verify */
#define FDC$M_Version		0x10	/* Version */
#define FDC$M_Write		0x05	/* Write Data */
#define FDC$M_WtDel		0x09	/* Write Deleted Data */

/* Command Sizes
*/
#define FDC$V_Config		4	/* Configure */
#define FDC$V_DumpReg		1	/* Dump REgisters */
#define FDC$V_FmtTrk		6	/* Format Track */
#define FDC$V_Lock		1	/* Lock */
#define FDC$V_Mode		5	/* Mode */
#define FDC$V_Nsc		1	/* NSC */
#define FDC$V_PrdicMode		2	/* Perpendicular Mode */
#define FDC$V_Read		9	/* Read Data */
#define FDC$V_RdDel		9	/* Read Deleted Data */
#define FDC$V_RdID		2	/* Read ID */
#define FDC$V_RdTrk		9	/* Read A Track */
#define FDC$V_Recal		2	/* Recalibrate */
#define FDC$V_RelSk		2	/* Relative Seek */
#define FDC$V_ScnEQ		9	/* Scan Equal */
#define FDC$V_ScnHEQ		9	/* Scan High or Equal */
#define FDC$V_ScnLEQ		9	/* Scan Low or Equal */
#define FDC$V_Seek		4	/* Seek */
#define FDC$V_SnsDrvStat	2	/* Sense Drive Status */
#define FDC$V_SnsInter		1	/* Sense Interrupt */
#define FDC$V_SetTrk		3	/* Set Track */
#define FDC$V_Specify		3	/* Specify */
#define FDC$V_Verify		9	/* Verify */
#define FDC$V_Version		1	/* Version */
#define FDC$V_Write		9	/* Write Data */
#define FDC$V_WtDel		9	/* Write Deleted Data */

/* MSR, Phase Definitions
*/
#define FDC$M_IDLE	0x90	/* FDC ready to Receive FIFO Writes */
#define FDC$M_CMD	0xD0	/* FDC Rdy FIFO Writes, Cmd in progress  */
#define FDC$M_RESULTS	0x80	/* FDC Rdy FIFO Reads,  Cmd in progress */


/* 	MSR, MAIN STATUS REGISTER BIT DEFINITIONS
*/
#define FDC$M_RQM	0x80	/* Request for Master */
#define FDC$M_DIO	0x40	/* Data I/O Direction */
#define FDC$M_NDMA	0x20	/* Non-DMA Execution */
#define FDC$M_CPROG	0x10	/* Command In Progress */
#define FDC$M_BUSY3	0x08	/* Drive 3 Busy */
#define FDC$M_BUSY2	0x04	/* Drive 2 Busy */
#define FDC$M_BUSY1	0x02	/* Drive 1 Busy */
#define FDC$M_BUSY0	0x01	/* Drive 0 Busy */

#define FDC$V_BUSY3	0x03	/* */
#define FDC$V_BUSY2	0x02	/* */
#define FDC$V_BUSY1	0x01	/* */
#define FDC$V_BUSY0	0x00	/* */

/* 	DSR,  DATA RATE SELECT REGISTER BIT DEFINITIONS
*/
#define FDC$M_SWRest	0x80	/* Software Reset */
#define FDC$M_LwPwr	0x40	/* Low Power */
#define FDC$M_PcSel2	0x10	/* PreCompensation Select 2 */
#define FDC$M_PcSel1	0x08	/* PreCompensation Select 1 */
#define FDC$M_PcSel0	0x04	/* PreCompensation Select 0 */
#define FDC$M_DRate1	0x02	/* Data Rate Select 1 */
#define FDC$M_DRate0	0x01	/* Data Rate Select 0 */

#define FDC$v_DRate3	0x03	/* */
#define FDC$v_DRate2	0x02	/* */
#define FDC$v_DRate1	0x01	/* */
#define FDC$V_DRate0	0x00	/* */

/* DOR, DIGITAL OUTPUT REGISTER
*/
#define FDC$M_MTR3	0x80	/* Moter Enable 3 */
#define FDC$M_MTR2	0x40	/* Moter Enable 2 */
#define FDC$M_MTR1	0x20	/* Moter Enable 1 */
#define FDC$M_MTR0	0x10	/* Moter Enable 0 */
#define FDC$M_DMAEN	0x08	/* DMA Enable */
#define FDC$M_RESET	0x04	/* Reset Controller */
#define FDC$M_DSel1	0x02	/* Drive Select 1 */
#define FDC$M_DSel0	0x01	/* Drive Select 0 */

#define FDC$V_DSel3	0x03	/* */
#define FDC$V_DSel2	0x02	/* */
#define FDC$V_DSel1	0x01	/* */
#define FDC$V_DSel0	0x00	/* */


/* IDE definitions
/*
/* 	The IDE Feature of the COMBO Chip is NOT enabled for Sable.
/*	However, IDE Register definitions are included for completness.
*/
#define COMBO_IDE 	0x1F0	/* Integrated Device Electronics Base Address */

#define IDE_DATA	(COMBO_IDE+0x00)
#define IDE_ERROR	(COMBO_IDE+0x01)
#define IDE_SECTORCNT	(COMBO_IDE+0x02)
#define IDE_SECTORNUM	(COMBO_IDE+0x03)
#define IDE_CYCLOW	(COMBO_IDE+0x04)
#define IDE_CYCHI	(COMBO_IDE+0x05)
#define IDE_DRVHEAD	(COMBO_IDE+0x06)
#define IDE_STATUS	(COMBO_IDE+0x07)
#define IDE_ALTERSTATUS	(COMBO_IDE+0x200+0x6)
#define IDE_DRVADR	(COMBO_IDE+0x200+0x7)

/* 	Mask Definitions
/*    		RW   bits have an associated 1
/*    		RO   bits have an associated 0
/*    		rsvd bits have an associated 0
*/
#define IDE_MDATA	0x00
#define IDE_MERROR	0x01
#define IDE_MSECTORCNT	0x02
#define IDE_MSECTORNUM	0x03
#define IDE_MCYCLOW	0x04
#define IDE_MCYCHI	0x05
#define IDE_MDRVHEAD	0x06
#define IDE_MSTATUS	0x07
#define IDE_MALTERSTATUS 0x20
#define IDE_MDRVADR	 0x20

/* 	Init Values 
*/
#define IDE_IDATA	0x00
#define IDE_IERROR	0x01
#define IDE_ISECTORCNT	0x02
#define IDE_ISECTORNUM	0x03
#define IDE_ICYCLOW	0x04
#define IDE_ICYCHI	0x05
#define IDE_IDRVHEAD	0x06
#define IDE_ISTATUS	0x07
#define IDE_IALTERSTATUS 0x20
#define IDE_IDRVADR	0x20

/* 	Default Program values 
*/
#define IDE_DDATA	0x00
#define IDE_DERROR	0x01
#define IDE_DSECTORCNT	0x02
#define IDE_DSECTORNUM	0x03
#define IDE_DCYCLOW	0x04
#define IDE_DCYCHI	0x05
#define IDE_DDRVHEAD	0x06
#define IDE_DSTATUS	0x07
#define IDE_DALTERSTATUS 0x20
#define IDE_DDRVADR	0x20


