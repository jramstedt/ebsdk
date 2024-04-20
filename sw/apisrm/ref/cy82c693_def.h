/* File:	cy82c693_def.h
 *
 * Copyright (C) 1997 by
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
 * Abstract:	
 *
 *	This file contains register and content definitions for the 
 *	Cypress CY82C693 chip.
 *
 *	The CY82C693 is a highly integrated peripheral controller 
 *	for PCI-based systems.  The CY82C693 contains a PCI-to-ISA
 *	bridge, a PCI IDE controller, DMA controllers, Interrupt
 *	controllers, a Real Time Clock, keyboard/mouse controller,
 *	and an Open HCI compliant USB host controller.
 *
 * Author:	
 *
 *	Eric Rasmussen
 *
 * Modification History:
 *
 *	er	09-Sep-1997	Added DMA controller and PCI configuration 
 *				register definitions
 *
 *	er	30-Jan-1997	Initial Entry
 */

#ifndef __CY82C693_H
#define __CY82C693_H

/*
** CY82C693 Control Register Index Definitions
*/
#define CY82C693_CR01_INDEX	    0x01
#define CY82C693_CR02_INDEX	    0x02
#define CY82C693_CR03_INDEX	    0x03
#define CY82C693_CR04_INDEX	    0x04
#define CY82C693_CR05_INDEX	    0x05

/*
** Control Register Alias Definitions
*/
#define CY82C693_CFG_1_INDEX	    CY82C693_CR01_INDEX
#define CY82C693_CFG_2_INDEX	    CY82C693_CR02_INDEX
#define CY82C693_ELCR_1_INDEX	    CY82C693_CR03_INDEX
#define CY82C693_ELCR_2_INDEX	    CY82C693_CR04_INDEX
#define CY82C693_RTC_CFG_INDEX	    CY82C693_CR05_INDEX

/*
** CY82C693 Control Register Definition
**
** The control registers are accessed through I/O ports
** 22h and 23h (PCI I/O reads or writes to offsets 22h
** and 23h).  To access each control register, the user
** must first write an index number into port 22h to
** point to the selected register.  Data can then be
** accessed by reading/writing from/to port 23h.
*/
typedef struct _CY82C693_CONTROL_REGS {
    unsigned char	index;	/* Write only */
    unsigned char	data;	/* Read/Write */
} CY82C693_CONTROL_REGS;


/*
** CY82C693 Interrupt Controller Definitions
*/

/* 
** Initialization Command Word Register Offset Definitions 
*/
#define CY82C693_IntC1_ICW1	0x20
#define CY82C693_IntC1_ICW2	0x21
#define CY82C693_IntC1_ICW3	0x21
#define CY82C693_IntC1_ICW4	0x21

#define CY82C693_IntC2_ICW1	0xA0
#define CY82C693_IntC2_ICW2	0xA1
#define CY82C693_IntC2_ICW3	0xA1
#define CY82C693_IntC2_ICW4	0xA1

/*
** Initialization Command Word 1 Default Programming Values
**
**	ICW1<7:5>   Reserved (MBZ)
**	ICW1<4>	    Controller Initialization Control:
**			0   - Operational Command Word Write
**			1   - Initialization Command Word Write (D)
**	ICW1<3>	    Controller Interrupt Request Control:
**			0   - Edge-Triggered (D)
**			1   - Level-Sensitive
**	ICW1<2>	    Don't care
**	ICW1<1>	    Controller Cascade Control:
**			0   - Cascade Mode (D)
**			1   - Single Mode (should NEVER be used)
**	ICW1<0>	    ICW4 Write Status:
**			0   - ICW4 write no required
**			1   - ICW4 write required (D)
**	
*/
#define CY82C693_IntC1_DICW1	0x11
#define CY82C693_IntC2_DICW1	0x11

/*
** Initialization Command Word 2 Default Programming Values
**
**	ICW2<7:3>   Upper 5 bits of interrupt vector
**	ICW2<2:0>   Interrupt Request Level:
**			- Must be programmed to 000b
*/
#define CY82C693_IntC1_DICW2	0x00
#define CY82C693_IntC2_DICW2	0x08

/*
** Initialization Command Word 3 (IntC1) Default Programming Values
**
**	ICW3<7>	    Slave Control IR7:
**			0   - No slave connected to IR7
**			1   - Slave connected to IR7
**	ICW3<6>	    Slave Control IR6:
**			0   - No slave connected to IR6
**			1   - Slave connected to IR6
**	ICW3<5>	    Slave Control IR5:
**			0   - No slave connected to IR5
**			1   - Slave connected to IR5
**	ICW3<4>	    Slave Control IR4:
**			0   - No slave connected to IR4
**			1   - Slave connected to IR4
**	ICW3<3>	    Slave Control IR3:
**			0   - No slave connected to IR3
**			1   - Slave connected to IR3
**	ICW3<2>	    Slave Control IR2:
**			0   - No slave connected to IR2
**			1   - Slave connected to IR2 (D)
**	ICW3<1>	    Slave Control IR1:
**			0   - No slave connected to IR1
**			1   - Slave connected to IR1
**	ICW3<0>	    Slave Control IR0:
**			0   - No slave connected to IR0
**			1   - Slave connected to IR0
*/
#define CY82C693_IntC1_DICW3	0x04

/*
** Initialization Command Word 3 (IntC2) Default Programming Values
**
**	ICW3<7:3>   Reserved (MBZ)
**	ICW3<2:0>   Slave mode address
**			- Should be programmed to 010b for 
**			  proper cascade mode operation with
**			  IntC1.
*/
#define CY82C693_IntC2_DICW3	0x02

/*
** Initialization Command Word 4 Default Programming Values
**
**	ICW4<7:5>   Reserved (MBZ)
**	ICW4<4>	    Multiple Interrupt Control:
**			0   - Disable (D)
**			1   - Enable
**	ICW4<3>	    Buffered Mode:
**			0   - Not Buffered (D)
**			1   - Buffered
**	ICW4<2>	    Master/Slave sotred in buffer:
**			    - Must be programmed to 0
**	ICW4<1>	    Automatic End-of-Interrupt (AEOI) Control:
**			0   - Disable AEOI (D)
**			1   - Enable AEOI
**	ICW4<0>	    Microprocessor Mode:
**			0   - Not x86
**			1   - x86 (D)
**	
*/
#define CY82C693_IntC1_DICW4	0x01
#define CY82C693_IntC2_DICW4	0x01

/*
** Operational Command Word Register Offset Definitions
*/
#define CY82C693_IntC1_OCW1	0x21
#define CY82C693_IntC1_OCW2	0x20
#define CY82C693_IntC1_OCW3	0x20

#define CY82C693_IntC2_OCW1	0xA1
#define CY82C693_IntC2_OCW2	0xA0
#define CY82C693_IntC2_OCW3	0xA0

/*
** Operational Command Word 1 Default Programming Values
**
**	OCW1<7>	    IR7 Mask Control:
**			0   - Not Masked
**			1   - Masked
**	OCW1<6>	    IR6 Mask Control:
**			0   - Not Masked
**			1   - Masked
**	OCW1<5>	    IR5 Mask Control:
**			0   - Not Masked
**			1   - Masked
**	OCW1<4>	    IR4 Mask Control:
**			0   - Not Masked
**			1   - Masked
**	OCW1<3>	    IR3 Mask Control:
**			0   - Not Masked
**			1   - Masked
**	OCW1<2>	    IR2 Mask Control:
**			0   - Not Masked
**			1   - Masked 
**	OCW1<1>	    IR1 Mask Control:
**			0   - Not Masked
**			1   - Masked
**	OCW1<0>	    IR0 Mask Control:
**			0   - Not Masked
**			1   - Masked 
*/
#define CY82C693_IntC1_DOCW1	0xFB
#define CY82C693_IntC2_DOCW1	0xFF

/*
** Operational Command Word 2 Default Programming Values
**
**	OCW2<7:5>   Command Control:
**			000 - Clear Rotate in Auto EOI (D)
**			001 - Non-specific EOI Command
**			010 - No Command
**			011 - Specific EOI Command
**			100 - Set Rotate on AEOI
**			101 - Rotate on Non-specific EOI
**			110 - Specific Rotate Command
**			111 - Rotate on Specific EOI
**	OCW2<4>	    Controller Initialization Control:
**			0   - Operational Command Word Write (D)
**			1   - Initialization Command Word Write
**	OCW2<3>	    Operational Command Word Selection Control:
**			0   - OCW2 write (D)
**			1   - OCW3 write
**	OCW2<2:0>   Interrupt Channel Select:
**			000 - Channel 0 (IRQ0/IRQ8) (D)
**			001 - Channel 1 (IRQ1/IRQ9)
**			010 - Channel 2 (IRQ2/IRQ10)
**			011 - Channel 3 (IRQ3/IRQ11)
**			100 - Channel 4 (IRQ4/IRQ12)
**			101 - Channel 5 (IRQ5/IRQ13)
**			110 - Channel 6 (IRQ6/IRQ14)
**			111 - Channel 7 (IRQ7/IRQ15)
*/
#define CY82C693_IntC1_DOCW2	0x00
#define CY82C693_IntC2_DOCW2	0x00

#define CY82C693_EOI_NS		0x20	/* Non-specific EOI */
#define CY82C693_EOI		0x60	/* Specific EOI */

/*
** Operational Command Word 3 Default Programming Values
**
**	OCW3<7>	    Reserved (MBZ)
**	OCW3<6>	    Set/Reset Special Mask Mode Control:
**			0   - Disable (D)
**			1   - Enable
**	OCW3<5>	    Special Mask Mode Control:
**			0   - Disable (D)
**			1   - Enable
**	OCW3<4>	    Controller Initialization Control:
**			0   - Operational Command Word Write (D)
**			1   - Initialization Command Word Write
**	OCW3<3>	    Operational Command Word Selection Control:
**			0   - OCW2 write (D)
**			1   - OCW3 write
**	OCW3<2>	    Interrupt Polling Control:
**			0   - Disable Polling Cycle (D)
**			1   - Enable Polling Cycle
**	OCW3<1:0>   Status Control:
**			00  - Disable Status Read (D)
**			01  - Disable Status Read
**			10  - Contents of the IRR will be read
**			11  - Contents of the IMR will be read
*/
#define CY82C693_IntC1_DOCW3	0x00
#define CY82C693_IntC2_DOCW3	0x00


/*
** CY82C693 DMA Controller Definitions
*/

/*
** CY82C693 DMA Controller Register Address Definitions
**
** These registers are accessed by performing I/O reads and
** writes to addresses 000h through 00Fh (for DMAC1) and
** addresses 0C0h through 0DEh (for DMAC2).
*/

#define CY82C693_DMA11_ADDR	    0x00B
#define CY82C693_DMA13_ADDR	    0x00D

#define CY82C693_DMA27_ADDR	    0x0D6
#define CY82C693_DMA29_ADDR	    0x0DA

/*
** DMA Controller Register Alias Definitions
*/
#define CY82C693_DMAC1_MasterClear  CY82C693_DMA13_ADDR   /* Write Only */
#define CY82C693_DMAC2_MasterClear  CY82C693_DMA29_ADDR   /* Write Only */

#define CY82C693_DMAC1_Mode	    CY82C693_DMA11_ADDR   /* Write Only */
#define CY82C693_DMAC2_Mode	    CY82C693_DMA27_ADDR   /* Write Only */


/*
** CY82C693 PCI Configuration Definitions
*/

/*
** CY82C693 PCI Configuration Register Offset Definitions
*/              
#define CY82C693_PCI04_OFFSET	    0x04

#define CY82C693_PCI40_OFFSET	    0x40
#define CY82C693_PCI41_OFFSET	    0x41
#define CY82C693_PCI42_OFFSET	    0x42
#define CY82C693_PCI43_OFFSET	    0x43
#define CY82C693_PCI44_OFFSET	    0x44
#define CY82C693_PCI45_OFFSET	    0x45

#define	CY82C693_PCI47_OFFSET	    0x47
#define	CY82C693_PCI48_OFFSET	    0x48
#define	CY82C693_PCI49_OFFSET	    0x49
                      
#define CY82C693_PCI4B_OFFSET	    0x4B
#define CY82C693_PCI4C_OFFSET	    0x4C
#define CY82C693_PCI4D_OFFSET	    0x4D
                      
/*                    
** PCI Configuration Register Alias Definitions
*/
#define CY82C693_Command		CY82C693_PCI04_OFFSET

#define CY82C693_PCI_INTA_Routing	CY82C693_PCI40_OFFSET
#define CY82C693_PCI_INTB_Routing	CY82C693_PCI41_OFFSET
#define CY82C693_PCI_INTC_Routing	CY82C693_PCI42_OFFSET
#define CY82C693_PCI_INTD_Routing	CY82C693_PCI43_OFFSET
#define CY82C693_PCI_Control		CY82C693_PCI44_OFFSET
#define CY82C693_PCI_Error_Control	CY82C693_PCI45_OFFSET

#define CY82C693_PCI_BIOS_Control	CY82C693_PCI47_OFFSET
#define CY82C693_ISA_DMA_TopOfMemory	CY82C693_PCI48_OFFSET
#define CY82C693_AT_Control_1		CY82C693_PCI49_OFFSET
#define CY82C693_IDE_IRQ_0_Routing	CY82C693_PCI4B_OFFSET
#define CY82C693_IDE_IRQ_1_Routing	CY82C693_PCI4C_OFFSET
#define CY82C693_SA_USB_Control		CY82C693_PCI4D_OFFSET

#endif /* __CY82C693_H */
