/* file:	nautilus_io.c
 * 
 * Copyright (C) 1996,1997,1998 by
 * Digital Equipment Corporation,Maynard,Massachusetts.
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++                                                                     
 *  FACILITY:
 *
 *      Alpha Firmware for Nautilus
 *                                            
 *  MODULE DESCRIPTION:     
 *
 *	PC264 platform specific io routines
 *
 *  AUTHORS:
 *
 *	Eric Goehl
 *
 *  CREATION DATE:
 *  
 *      13-Nov-1996
 *
 *  MODIFICATION HISTORY:
 *
 *	er	17-Nov-1998	Adding get_com1_baud() and set_com1_baud().
 *	er	13-Mar-1998	Moved interrupt handling support,
 *				probe_local(),PowerUpProgress(),
 *				and use_eisa_ecu_data() stub from 
 *				PC264.C to here.
 *	er	24-Aug-1997	Fixed TIG bus xflash().
 *	er	28-Jul-1997	Fixed offset mask in xcfgx.
 *	er	14-Jul-1997	Added probe_pci_setup routine,
 *				cleaned up includes,external defs.
 *     	egg	13-Nov-1996	Initial port from eb164_io.c
 *
 *--
 */                                                

extern struct pb *pb;

#include	"cp$src:platform.h"
#include	"cp$inc:platform_io.h"
#include	"cp$src:stddef.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:pb_def.h"
#include 	"cp$src:common.h"	/* uint64 */
#include	"cp$inc:prototypes.h"
#include	"cp$src:probe_io_def.h"	/* TYPE_PCI,etc. */
#include	"cp$src:common_def.h"	/* msg_failure, msg_success */
#include	"cp$src:dynamic_def.h"	/* DYN$K_... constants. */
#include	"cp$src:eisa_def.h"
#include	"cp$src:ctype.h"	/* toupper */

/* Platform specific include files */
#include 	"cp$src:platform_cpu.h"
#include	"cp$src:m1543c_sbridge.h"
#include	"cp$src:m1543c_def.h"
#include	"cp$src:pic8259.h"

#include	"cp$src:nt_types.h"

#define DEBUG	1

#if M1543C
#define SYSTEM_FLASH_OFFSET		0xFFFC0000
#define FlashAddr(x)			(SYSTEM_FLASH_OFFSET + (x))
#define SYSTEM_FLASH_WINDOW		0x00040000
#define OFFSETTOGPIO(offset)	(((offset / SYSTEM_FLASH_WINDOW) & 0x03) | (((offset / SYSTEM_FLASH_WINDOW) & 0x04) << 4))
#define GPOREG					DOGPIO
#define GPIOBITS				(0x43)
#endif
#if M1535C
#define SYSTEM_FLASH_OFFSET		0xFFF80000
#define FlashAddr(x)			(SYSTEM_FLASH_OFFSET + (x))
#define SYSTEM_FLASH_WINDOW		0x00080000
#define OFFSETTOGPIO(offset)	(((offset / SYSTEM_FLASH_WINDOW) & 0x02) << (1-1))
#define GPOREG					GPO_BLK1
#define GPIOBITS				(0x02)
#define OFFSETTOGPIO2(offset)	(((offset / SYSTEM_FLASH_WINDOW) & 0x01) << 5)
#define GPOREG2					GPO_BLK4
#define GPIOBITS2				(0x20)
#endif

/*
// Pin To Line Tables
*/
unsigned char pci_irq_table[1][PCI_MAX_SLOT-PCI_MIN_SLOT+1][4] = {{
/*    INT A INT B INT C INT D
      ----- ----- ----- ----- */
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 0  = PCI_AD[11] TINOSA_PCI_DEVICE_ID		*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 1  = PCI_AD[12] TINOSA_AGP_DEVICE_ID Agp	*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 2  = PCI_AD[13]								*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 3  = PCI_AD[14]								*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 4  = PCI_AD[15]								*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 5  = PCI_AD[16] TINOSA_AGP_SLOT1_DEVICE_ID	*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 6  = PCI_AD[17]								*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 7  = PCI_AD[18] South Bridge				*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 8  = PCI_AD[19] TINOSA_PCI_SLOT0_DEVICE_ID	*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 9  = PCI_AD[20] TINOSA_PCI_SLOT1_DEVICE_ID	*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 10 = PCI_AD[21] TINOSA_PCI_SLOT2_DEVICE_ID	*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 11 = PCI_AD[22] TINOSA_PCI_SLOT3_DEVICE_ID	*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 12 = PCI_AD[23]								*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 13 = PCI_AD[24]								*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 14 = PCI_AD[25]								*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 15 = PCI_AD[26]								*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 16 = PCI_AD[27] TINOSA_IDE_DEVICE_ID		*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 17 = PCI_AD[28] TINOSA_PMU_DEVICE_ID		*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 18 = PCI_AD[29]								*/
    { 0xff, 0xff, 0xff, 0xff }, /* Virtual Slot 19 = PCI_AD[30]								*/
    { 0xff, 0xff, 0xff, 0xff },	/* Virtual Slot 20 = PCI_AD[31] TINOSA_USB_DEVICE_ID		*/
}};

/*
** External Data Declarations
*/
extern struct LOCK spl_kernel;                  
extern int primary_cpu;
extern struct set_param_table baud_table[];
extern int cbip;

/*            
** External Function Prototype Declarations     
*/                       
                       
                                              
/* From kernel_alpha.mar */
extern unsigned __int64 ldq(unsigned __int64 address);
extern void stq(unsigned __int64 address,unsigned __int64 data);
extern spinlock(struct LOCK *spl);
extern spinunlock(struct LOCK *spl);
extern int do_bpt();

/* From printf.c */
extern int qprintf(char *format,...);
extern int err_printf(char *format,...);

/* From pc264.c */
extern int whoami(void);

/* From nautilus_init.c */
extern void AcerM1543C_init(void);

void setup_io(void )
{
    unsigned __int64 pctl_bits = 0;
    unsigned int perrmask_bits = 0;
    unsigned __int64 temp;
    unsigned int i;

	GobyInitialize();					/* Init northbridge. */
	M1543CPciIsaInitialize();			/* Init southbridge. */
	M7101PmuInitialize();				/* Init PM/. */
	AcerM1543C_init();					/* Initialize the M1543C Super I/O Controller */
	/*IrqVectorSetup();					/* Setup IRQ sharing stuff. */

	/*
	** Initialize the slave ISA interrupt controller
	**   
	**  ICW1 - ICW4 write required
	**  ICW2 - Vector table base address = 8h
	**  ICW3 - Slave identification code = 2h
	**  ICW4 - x86 mode,normal EOI
	**  OCW1 - IRQ8 - IRQ15 disabled
	*/
    outportb(NULL,&pEisaControlBase->Interrupt2ControlPort0,PIC8259_IntC2_DICW1);
    outportb(NULL,&pEisaControlBase->Interrupt2ControlPort1,PIC8259_IntC2_DICW2);
    outportb(NULL,&pEisaControlBase->Interrupt2ControlPort1,PIC8259_IntC2_DICW3);
    outportb(NULL,&pEisaControlBase->Interrupt2ControlPort1,PIC8259_IntC2_DICW4);
    outportb(NULL,&pEisaControlBase->Interrupt2ControlPort1,PIC8259_IntC2_DOCW1);

	/*
	** Initialize the master ISA interrupt controller
	**
	**  ICW1 - ICW4 write required
	**  ICW2 - Vector table base address = 0h
	**  ICW3 - Slave controller connected to IRQ2
	**  ICW4 - x86 mode,normal EOI
	**  OCW1 - IRQ2 enabaled,IRQ0,IRQ1,IRQ3-IRQ7 disabled
	*/
    outportb(NULL,&pEisaControlBase->Interrupt1ControlPort0,PIC8259_IntC1_DICW1);
    outportb(NULL,&pEisaControlBase->Interrupt1ControlPort1,PIC8259_IntC1_DICW2);
    outportb(NULL,&pEisaControlBase->Interrupt1ControlPort1,PIC8259_IntC1_DICW3);
    outportb(NULL,&pEisaControlBase->Interrupt1ControlPort1,PIC8259_IntC1_DICW4);
    outportb(NULL,&pEisaControlBase->Interrupt1ControlPort1,PIC8259_IntC1_DOCW1);

	/*
	** Send a non-specific EOI command to clear any pending ISA interrupts
	*/
    for (i = 0;  i < 8;  i++)
	{
		outportb(NULL,&pEisaControlBase->Interrupt2ControlPort0,PIC8259_EOI|i);
		outportb(NULL,&pEisaControlBase->Interrupt1ControlPort0,PIC8259_EOI|i);
    }
}
                                                                          
SetFlashGpioBits(
IN ULONG FlashOffset)
/*++

Routine Description:

    Set up the GPIO bits to enable access to FlashOffset

Arguments:

    FlashOffset - offset within the flash ROM.

Return Value:

    Returns offset into visible flash area.

--*/
{
	UCHAR gpioBits;

#if M1535C
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,PCSDS),M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,PCSDS),sizeof(UCHAR))&~0x08,sizeof(UCHAR));	/* Set PCS2J/GPO29 to GPO29. */
	M7101PmuWriteConfig(OFFSETOF(M7101_PMU_CONFIG,MULTI_BLK1),M7101PmuReadConfig(OFFSETOF(M7101_PMU_CONFIG,MULTI_BLK1),sizeof(UCHAR))|0x02,sizeof(UCHAR));		/* Set GPI32/GPO39 to GPO39. */
#endif

	gpioBits = M7101PmuReadConfig(OFFSETOF(M7101_PMU_CONFIG,GPOREG),sizeof(gpioBits));
	gpioBits = (gpioBits & ~GPIOBITS) | OFFSETTOGPIO(FlashOffset);
	M7101PmuWriteConfig(OFFSETOF(M7101_PMU_CONFIG,GPOREG),gpioBits,sizeof(gpioBits));	/* Page in the correct page. OPTIMIZE SO WE ONLY DO WHEN IT CHANGES. */

#ifdef OFFSETTOGPIO2
	gpioBits = M7101PmuReadConfig(OFFSETOF(M7101_PMU_CONFIG,GPOREG2),sizeof(gpioBits));
	gpioBits = (gpioBits & ~GPIOBITS2) | OFFSETTOGPIO2(FlashOffset);
	M7101PmuWriteConfig(OFFSETOF(M7101_PMU_CONFIG,GPOREG2),gpioBits,sizeof(gpioBits));	/* Page in the correct page. OPTIMIZE SO WE ONLY DO WHEN IT CHANGES. */
#endif

	return(FlashAddr(FlashOffset & (SYSTEM_FLASH_WINDOW-1)));			/* Now return the correct offset. */
}

/*+                                                                        
 * ============================================================================
 * = in_flash - Read data from flash ROM address space.                 =  
 * ============================================================================
 *                                                                         
 * OVERVIEW:                                                               
 *                                                                         
 *   	This routine will read a byte from the flash ROM address space.    
 *                                                                         
 * FORM OF CALL:                                                           
 *                                                                         
 *   	data = in_flash(pb,offset);                                     
 *                                                                         
 * RETURNS:      
 *   
 *   	unsigned char - Data from read.
 *   
 * ARGUMENTS:
 *   
 *	struct pb *pb - pointer to a device port block data structure.
 *	                See pb_def.h for more info.
 *
 *	unsigned int offset - Longword offset in flash ROM region to read.
 *
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

unsigned char in_flash(struct pb *pb,unsigned int offset ) 
{
	UINT FlashAddress;
	UCHAR Data;
	UCHAR pmuEnable;

	pmuEnable = M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,SMCCII),sizeof(pmuEnable));		/* Get current state. */
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,SMCCII),pmuEnable & ~0x04,sizeof(pmuEnable));/* Enable. */

	FlashAddress = SetFlashGpioBits(offset);

	Data = READ_REGISTER_UCHAR(FlashAddress);

	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,SMCCII),pmuEnable,sizeof(pmuEnable));		/* Replace state. */

	return(Data);
}                              
                               
                              
/*+                            
 * ============================================================================
 * = out_flash - Write data to flash ROM address space.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will write a byte to the flash ROM address space.
 *
 * FORM OF CALL:
 *
 *	out_flash(pb,offset,data);
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.                   
 *                                                                    
 *	unsigned int offset - Longword offset in flash ROM region to write.
 *
 *	unsigned __int64 - Data to write,passed by value.
 *
 * SIDE EFFECTS:
 *                                        
 *      None
 *
-*/

void out_flash(struct pb *pb,unsigned int offset,unsigned __int64 data ) 
{
	UINT FlashAddress;
	UCHAR pmuEnable;

	pmuEnable = M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,SMCCII),sizeof(pmuEnable));		/* Get current state. */
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,SMCCII),pmuEnable & ~0x04,sizeof(pmuEnable));/* Enable. */

	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,BCSC),M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,BCSC),sizeof(UCHAR))|0x40,sizeof(UCHAR));	/* Turn off flash write protect. */

	FlashAddress = SetFlashGpioBits(offset);

	WRITE_REGISTER_UCHAR(FlashAddress,data);

	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,BCSC),M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,BCSC),sizeof(UCHAR))&~0x40,sizeof(UCHAR));/* Turn on flash write protect. */
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,SMCCII),pmuEnable,sizeof(pmuEnable));		/* Replace state. */
}

                                                   
void PowerUpProgress (unsigned char value,char *string,int a,int b,int c,int d )
{
#if DEBUG
	static qp = FALSE;

    if (value < 0xf7)		/* Make sure we don't try before RS232 initialized. */
		qp = TRUE;

	if (qp)
	{
		qprintf("CPU %d: %2x.",whoami(),value);                        
		qprintf(string,a,b,c,d);
	}
#endif
    outportb(NULL,0x80,value);
}


probe_local()
{
    goby_probe_local();
}

                                                   
void use_eisa_ecu_data(struct pb *pb )             
{                                                   
}                                                   

unsigned char vector_allocated[ 64 ];

/*+
 * ============================================================================
 * = sys_irq_init - Initialize PCI irq table				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Initialize the PCI irq allocation table entries to default values.
 *      Called from POWERUP.C.
 *                        
 * FORM OF CALL: 
 *               
 *	sys_irq_init()
 *                    
 * FUNCTIONAL DESCRIPTION 
 *               
 *	Clear the PCI irq allocation entries so drivers can allocate interrupt 
 *	vectors.
 *
-*/
void sys_irq_init(void )
{                                        
    int i;                               
                                         
    for (i = 0;  i < sizeof(vector_allocated);  i++ ) {
    	vector_allocated[i] = 0;
    }
}

/*+
 * ============================================================================
 * = assign_pci_vector - Allocate a PCI device interrupt routing assignment   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will allocate an interrupt routing assignment for a
 *	PCI device base on the specified slot number and interrupt pin.
 *
 * FORM OF CALL:
 *
 *	assign_pci_vector(pb,pin,base_pin,slot);
 *                    
 * RETURNS:
 *
 *   	Interrupt routing assignment,otherwise,-1.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	      int pin - interrupt pin register in PCI config space.
 *	     int slot - PCI slot number
 *
 * SIDE EFFECTS:
 *
 *      An interrupt vector may also be assigned.
 *
-*/
int assign_pci_vector(struct pb *pb,int pin,int base_pin,int slot )
{
	int irq;
	unsigned short class;
	unsigned int id;

	irq = pci_irq_table[pb->hose][slot - PCI_MIN_SLOT][pin-1];

	/*
	** Special case handling for PCI devices routed through ISA IRQs
	*/
	id = incfgl(pb,0);

	if (id == MAKEVENID(PCI_VENDORID_ACER,PCI_DEVICEID_ACER_IDE))
	{
		class = incfgw(pb,0x0A);

		/*
		** IDE interrupts are routed through ISA IRQ 14 & 15
		*/
		if (class == 0x0101 )
		{
			if (!vector_allocated[IDE0_INTERRUPT])
				irq = IDE0_INTERRUPT;
			else
				irq = IDE1_INTERRUPT;
		}
	}

	if (irq == 0xFF )
	{
		pb->vector = 0;
	}
	else 
	{
		vector_allocated[irq] = 1;
		pb->vector = irq + ISA_BASE_VECTOR;
	}

#if ((IPL_RUN) && (IPL_RUN > 19))
	pb->vector = 0;
#endif      
	return(irq);
}

/*+
 * ============================================================================
 * = io_get_vector - Allocate an ISA device interrupt vector                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will allocate an ISA device interrupt vector for the 
 *	specified IRQ channel.
 *
 *	The following table shows the allocation of ISA device interrupt 
 *	vectors:
 *
 *	IRQ	Vector	       Typical Interrupt Source
 *	====	======	========================================
 *	irq0	 0x80	Reserved - Interval timer 1 counter 0 output
 *	irq1	 0x81	Keyboard controller
 *	irq2	 0x82	Reserved - INTR from slave controller
 *	irq3	 0x83	COM2 serial port
 *	irq4	 0x84	COM1 serial port
 *	irq5	 0x85	Available
 *	irq6	 0x86	Floppy controller
 *	irq7	 0x87	Parallel port 1
 *	irq8	 0x88	Reserved
 *	irq9	 0x89	Available
 *	irq10	 0x8A	Universal Serial Bus controller
 *	irq11	 0x8B	Available
 *	irq12	 0x8C	Mouse controller
 *	irq13	 0x8D	Available
 *	irq14    0x8E	IDE controller
 *	irq15	 0x8F	IDE controller
 *
 * FORM OF CALL:
 *
 *	io_get_vector(pb,irq);
 *                    
 * RETURNS:
 *
 *   	Interrupt vector allocated (base vector + (IRQ from 0 to 15) ),
 *	otherwise,zero if vector not allocated.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned int irq - An ISA IRQ channel number.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
unsigned int io_get_vector(struct pb *pb,unsigned int irq )
{
    unsigned int vector;

    vector = ISA_BASE_VECTOR + irq;

#if ((IPL_RUN) && (IPL_RUN > 19))
    vector = 0;
#endif

    return(vector);
}

/*+
 * ============================================================================
 * = io_issue_eoi - Issue an End-Of-Interrupt command.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will issue an End-Of-Interrupt (EOI) command to
 *	clear the specified interrupt request.
 *
 * FORM OF CALL:
 *
 *	io_issue_eoi(pb,vector);
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *                                                       
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned int vector - A device interrupt vector.
 *
-*/
void io_issue_eoi(struct pb *pb,unsigned int vector )
{
	vector -= ISA_BASE_VECTOR;

	/*
	** Issue a Specific EOI command to clear the interrupt request
	** corresponding to IRQ0 - IRQ7 by writing the Operation Control
	** Word 2 (OCW) register on interrupt controller 1.
	*/                             
	spinlock(&spl_kernel);

	/*
	//  Issue a non-specific EOI to the appropriate control port 
	//  depending on the interrupt being ack'd
	//  The master (Controller1) for IRQs 0-7 or the slave (Controller2)
	//  for IRQs 8-f
	*/
	if (vector < 8)
	{
		WRITE_PORT_UCHAR(&pEisaControlBase->Interrupt1ControlPort0,PIC8259_EOI|vector);
	}
	else
	{
		WRITE_PORT_UCHAR(&pEisaControlBase->Interrupt2ControlPort0,PIC8259_EOI|(vector - 8));
		WRITE_PORT_UCHAR(&pEisaControlBase->Interrupt1ControlPort0,PIC8259_EOI|2);
	}

	spinunlock(&spl_kernel);
}                              
                              
/*+                            
 * ============================================================================
 * = io_disable_interrupts - Disable a PCI or ISA device interrupt.           =
 * ============================================================================
 *                             
 * OVERVIEW:                   
 *                             
 *	This routine will disable a PCI or ISA device interrupt by setting
 *	the appropriate bit in a mask register.
 *
 * FORM OF CALL:
 *
 *	io_disable_interrupts(pb,vector);
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned int vector - An ISA or PCI device interrupt vector.
 *
-*/
void io_disable_interrupts(struct pb *pb,unsigned int vector )
{
	PVOID ControlPort;
	UCHAR MaskBit;

	vector -= ISA_BASE_VECTOR;

	/*
	** Disable the corresponding IRQ0 - IRQ7 channel in the Interrupt Mask 
	** Register (IMR) by writing the Operation Control Word 1 (OCW1) register 
	** on interrupt controller 1.
	*/
	spinlock(&spl_kernel);

	if (vector < 8 )
	{
		ControlPort = &pEisaControlBase->Interrupt1ControlPort1;
		MaskBit = (1 << vector);
	}
	else
	{
		ControlPort = &pEisaControlBase->Interrupt2ControlPort1;
		MaskBit = (1 << (vector-8));
	}

	/*
	//  Read the current mask value and set the requested mask bit
	*/
	WRITE_PORT_UCHAR(ControlPort,READ_PORT_UCHAR(ControlPort)|MaskBit);

	spinunlock(&spl_kernel);

	/*
	** Make sure the In Service (IS) bit for this interrupt is cleared.
	*/
	io_issue_eoi(pb,vector+ISA_BASE_VECTOR);
}

/*+
 * ============================================================================
 * = io_enable_interrupts - Enable a PCI or ISA device interrupt.             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will enable a PCI or ISA device interrupt by clearing
 *	the appropriate bit in a mask register.
 *
 * FORM OF CALL:
 *
 *	io_enable_interrupts(pb,vector);
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned int vector - An ISA or PCI device interrupt vector.
 *
-*/
void io_enable_interrupts(struct pb *pb,unsigned int vector )
{
	PVOID ControlPort;
	UCHAR MaskBit;

	vector -= ISA_BASE_VECTOR;

	/*
	** Enable the corresponding IRQ0 - IRQ7 channel in the Interrupt Mask 
	** Register (IMR) by writing the Operation Control Word 1 (OCW1) register 
	** on interrupt controller 1.
	*/
	spinlock(&spl_kernel);

	if (vector < 8 )
	{
		ControlPort = &pEisaControlBase->Interrupt1ControlPort1;
		MaskBit = (1 << vector);
	}
	else
	{
		ControlPort = &pEisaControlBase->Interrupt2ControlPort1;
		MaskBit = (1 << (vector-8));
	}

	/*
	//  Read the current value of the mask,and clear the mask bit for
	//  the requested Irq
	*/
	WRITE_PORT_UCHAR(ControlPort,READ_PORT_UCHAR(ControlPort) & ~MaskBit);

	spinunlock(&spl_kernel);

	/*
	** Make sure the In Service (IS) bit for this interrupt is cleared.
	*/
	io_issue_eoi(pb,vector+ISA_BASE_VECTOR);
}

/*+
 * ============================================================================
 * = pc264_ipir - platform-specific interprocessor interrupt handler          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle all interprocessor interrupts
 *
 * FORM OF CALL:
 *
 *	pc264_ipir(); 
 *
 * RETURNS:
 *
 *      None
 *                                
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void pc264_ipir()
{
    pprintf("!!!!!!!!!!!!!! IP interrupt on CPU %d\n",whoami());
}

/*+
 * ============================================================================
 * = pc264_perfmon_isr - platform-specific perf montor interrupt handler      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle all performance monitor interrupts
 *
 * FORM OF CALL:
 *
 *	pc264_perfmon_isr(); 
 *
 * RETURNS:
 *
 *      None
 *                                
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void pc264_perfmon_isr(void )                  
{                                             
    pprintf("!!!!!!!!!!!!!! Performance monitor interrupt on CPU %d\n",whoami());
}                                             

/*+
 * ============================================================================
 * = pc264_unexpected_int - Handle all unexpected interrupts.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle all unexpected interrupts.
 *
 * FORM OF CALL:
 *
 *	pc264_unexpected_int(); 
 *
 * RETURNS:
 *
 *      None
 *                                
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void pc264_unexpected_int(void )
{
    pprintf("\n*** Unexpected interrupt .................\n");
    do_bpt();
}
/*+
 * ============================================================================
 * = mc_sg_map_on - Enable MC non 1 to 1 mapping
 * ============================================================================
 *     
 * OVERVIEW:
 *     	This routine and its companion mc_sg_map_off will enable anothor
 *     	location for the Memory channel adapter to point its PCT page too.
 *     	The location is currently 5mb 1 to 1; 1mb window.
 *
 *     	MC_CABLE expects to be able to get to address 524000 which is
 *     	the address pct 36 will write it data.
 *
 * RETURN: 
 *     	The pci window area that was remapped.
-*/        
unsigned int mc_sg_map_on(struct pb *pb)
{
#ifdef FIX_LATER
    if (pb->hose == 1 )
	{
	WriteTsunamiCSR(PCHIP1_WSBA2,WSBA2_VALUE | wsba_m_ena );
	WriteTsunamiCSR(PCHIP1_WSM2, 0);	/* 0 mask = 1 Meg Window */
	WriteTsunamiCSR(PCHIP1_TBA2, 0x00500000);
	}
	else
	{
	WriteTsunamiCSR(PCHIP0_WSBA2,WSBA2_VALUE | wsba_m_ena );
	WriteTsunamiCSR(PCHIP0_WSM2, 0);	/* 0 mask = 1 Meg Window */
	WriteTsunamiCSR(PCHIP0_TBA2, 0x00500000);
	}                                                    
    return WSBA2_VALUE; 	                      
#endif
	return(0);
}
/*
 * Turn off MC cable window.
*/
int mc_sg_map_off(struct pb *pb)                         
{                                                  
#ifdef FIX_LATER
    if (pb->hose == 1 )
	{
	WriteTsunamiCSR(PCHIP1_WSBA2,0);
	}
	else
	{
	WriteTsunamiCSR(PCHIP0_WSBA2,0);
	}
#endif
}                                                  


int get_com1_baud(void )
{
    if (rtc_read(0x11) == 69)
        return(rtc_read(0x12));
    else
        return(-1);
}

void set_com1_baud(
int baud)
{
    rtc_write(0x11,69);
    rtc_write(0x12,baud);
}

validate_com1_baud(char **value)                     
{                                                    
    int i;                                           
    char srev[8];                                    
    char buf[80];                                    
                                                     
    if (cbip)                                        
	return msg_success;                          
                                                     
    if ( ev_validate_table(value, baud_table) != msg_success )
	return msg_failure;                          
                                                     
    if( strcmp( *value, "9600" ) != 0 ) {            
	read_with_prompt("Embedded Remote Console only supports 9600 baud. Continue? (Y/[N]) ",sizeof(buf),buf,0,0,1);
	if (toupper(buf[0]) != 'Y')
	    return msg_failure;
    }
    return msg_success;
}
