/*++

Copyright (c) 1997  Digital Equipment Corporation

Module Name:

    SBridge.c

Abstract:

    Functions for configuring Acer Labs M1543C Southbridge in a Nautilus Motherboard.

Author:

    Chris Gearing  01 Feb 1999

Environment:

    AlphaBIOS

Revision History:

--*/

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:platform_cpu.h"
#include "cp$src:pb_def.h"
#include "cp$src:common.h"	/* uint64 */
#include "cp$src:isacfg.h"
#include "cp$src:parse_def.h"
#include "cp$src:m1543c_sbridge.h"
#include "cp$src:eisa_def.h"
#include "cp$src:nt_types.h"
#include "cp$src:m1543c_def.h"
#include "cp$src:combo_def.h"
#include "cp$src:iic_devices.h"
#include "cp$inc:platform_io.h"

#if NAUTILUS
#include "cp$src:irongate.h"
#endif

#define M1543C_DEBUG 0

void AllocateRouteTables(void);
char CsIrqReserveTable[ISA_IRQS+1] = IRQRESERVEDEFAULT;
char CsSteerableIrqTable[STEERABLE_IRQS+1] = STEERABLEIRQIRQDEFAULT;

/*  P R I V A T E   V A R I A B L E S */
static UCHAR routeValues[16] = {0x00,0x08,0x00,0x02,0x04,0x05,0x07,0x06,0x00,0x01,0x03,0x09,0x0b,0x00,0x0d,0x0f};

/*  P U B L I C   F U N C T I O N   D E F I N I T I O N S */

STEERABLEIRQ steerableIrqSetup[STEERABLE_IRQS] = {
	{M1543C_PMU_DEVICE_ID,		OFFSETOF(M1543_PCI_ISA_CONFIG,SCIIR),	0,	0,	"PWR_MANAGEMENT"},
	{M1543C_USB_DEVICE_ID,		OFFSETOF(M1543_PCI_ISA_CONFIG,USBIR),	0,	0,	"USB"},
#if PCISLOTIRQ_THRU_SBRIDGE
	{PCI_SLOT0_DEVICE_ID,		OFFSETOF(M1543_PCI_ISA_CONFIG,PIRTI),	0,	0,	NULL},
	{PCI_SLOT1_DEVICE_ID,		OFFSETOF(M1543_PCI_ISA_CONFIG,PIRTI),	4,	0,	NULL},
	{PCI_SLOT2_DEVICE_ID,		OFFSETOF(M1543_PCI_ISA_CONFIG,PIRTII),	0,	0,	NULL},
	{PCI_SLOT3_DEVICE_ID,		OFFSETOF(M1543_PCI_ISA_CONFIG,PIRTII),	4,	0,	NULL},
#endif
};

extern unsigned char pci_irq_table[][PCI_MAX_SLOT-PCI_MIN_SLOT+1][4];
extern unsigned int max_hose;

#define DEBUG_SBRIDGE 0
#if DEBUG_SBRIDGE
#define	debug(x)		qprintf x
#else
#define	debug(x)
#endif

VOID M1543CPciIsaInitialize(VOID)
/*++

Routine Description:

    It sets up some stuff specific for the Nautilus Motherboard.
    This function is called to configure the built-in Acer Labs M1543C PCI-ISA interface 
    
    It sets up some stuff specific for the Nautilus Motherboard.

Arguments:

Return Value:

    None.

--*/

{
	ASSERT(sizeof(M1543_PCI_ISA_CONFIG) == 0x100);	/* Assert structure filled correctly. */
	ASSERT(sizeof(M7101_PMU_CONFIG) == 0x100);		/* Assert structure filled correctly. */

	/* Set up some south bridge stuff. Should form this into a data list. */
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,ISACI), M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,ISACI),sizeof(UCHAR))|0x40, sizeof(UCHAR));	/* Enable 32Bit DMA. */
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,ISACII), M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,ISACII),sizeof(UCHAR))|0x80, sizeof(UCHAR));	/* Enable Port 92h. */
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,PIPM), M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,PIPM),sizeof(UCHAR))|0x01, sizeof(UCHAR));		/* Enable SERR Routing to CPU. */
#if !SHARK
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,BCSC), M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,BCSC),sizeof(UCHAR))|0x06, sizeof(UCHAR));		/* Enable 256K Windows at FFFC0000 */
#endif
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,USBIR), M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,USBIR),sizeof(UCHAR))|0x80, sizeof(UCHAR));	/* IOCHRDY will not be driven during DMA cycle. */

#if NAUTILUS || TINOSA
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,PILET), 0,sizeof(UCHAR));																	/* Clear back to default, Diags may set this. */
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,GPOS), M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,GPOS),sizeof(UCHAR))|0x03, sizeof(UCHAR));		/* Set GPIO[0:1] Pins as outputs. */
#endif

#if SHARK
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,OPTIONREGISTER[0]), 0x04,sizeof(UCHAR));														/* UDMA66 cable select enable */
	M5229IdeWriteConfig(OFFSETOF(M5229_IDE_PCI_CONFIG,UltraDmaSet[0]), 0xca, sizeof(UCHAR));															/* UDMA66 Setting for primary channel */
	M5229IdeWriteConfig(OFFSETOF(M5229_IDE_PCI_CONFIG,ChannelCfg[0]), 00310001, sizeof(ULONG));														/* Some other settings */
#endif
}

void SouthBridgeDeviceInit()
{
	int steerableIrq;
	UCHAR cEnable;
	USHORT sEnable, irqLevel;
	int irq;
	char irqList[16] = {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0};	/* 1 is IRQ always allocated to ISA */
	struct TABLE_ENTRY *pIsaDevice;

	debug(("Southbridge DeviceInit Starting\n"));

#if SHARK
	MB8574AndOr(~MB8574_SERIALPORTSELECT_RW,0);
#endif

	ASSERT(ARRAYSIZE(irqList) == ARRAYSIZE(CsIrqReserveTable));
	/*
	* Make sure that the ISA configuration data table has been 
	* initialized before probing.
	*/
	isacfg( 0, 0 );

	isacfg_get_irqs(irqList);					/* Used IRQs go to !0 */

	for (irq = 0; irq < ARRAYSIZE(irqList); irq++)
	{
		if (irqList[irq] == 0)					/* I.e. not used by ISA. */
			CsIrqReserveTable[irq] = IRQFORPNPCHAR;
	}

	for (irq = 0; irq < ARRAYSIZE(steerableIrqSetup); irq++)
	{
		
		if (
		    (steerableIrqSetup[irq].handle) &&
			(pIsaDevice = IsaDeviceInfo(steerableIrqSetup[irq].handle))					/* Get Ptr to device info */
		   )
		{
			debug(("ISA Device %s %d %x\n",steerableIrqSetup[irq].handle,pIsaDevice->device_enable,pIsaDevice->irq0_assignment));

			if (pIsaDevice->device_enable == 0)
			{
				CsSteerableIrqTable[irq] = STEERABLEIRQDISABLEDCHAR;
			}
			else
			{
				if (pIsaDevice->irq0_assignment == 0x80000000)
				{
					CsSteerableIrqTable[irq] = STEERABLEIRQAUTOCHAR;
				}
				else
				{
					if (pIsaDevice->irq0_assignment > 0x9)
						CsSteerableIrqTable[irq] = 'A'+(pIsaDevice->irq0_assignment-10);
					else
						CsSteerableIrqTable[irq] = '0'+pIsaDevice->irq0_assignment;
				}
			}
		}
	}

	/*
	// Now set the bits in the PCI-IRQ routing table to IRQX.
	*/
	AllocateRouteTables();						/* First off allocate them. */
	
	/* Now Set up IRQ routing */
	for (steerableIrq = 0; steerableIrq < STEERABLE_IRQS; steerableIrq++)
	{
		cEnable = (UCHAR)(M1543PciIsaReadConfig(steerableIrqSetup[steerableIrq].regOffset, sizeof(cEnable)) & (steerableIrqSetup[steerableIrq].regShift?0x0f:0xf0));
		
		if (pci_irq_table[0][steerableIrqSetup[steerableIrq].deviceId-PCI_MIN_SLOT][steerableIrqSetup[steerableIrq].deviceLine] != 0xff)
			cEnable |= routeValues[pci_irq_table[0][steerableIrqSetup[steerableIrq-PCI_MIN_SLOT].deviceId][steerableIrqSetup[steerableIrq].deviceLine]] << steerableIrqSetup[steerableIrq].regShift;
		M1543PciIsaWriteConfig(steerableIrqSetup[steerableIrq].regOffset, cEnable, sizeof(cEnable));
		debug(("Reg %x = %x\n",steerableIrqSetup[steerableIrq].regOffset,cEnable));
	}

	/* Now set IRQ levels. */
	irqLevel = 0;
#if PCISLOTIRQ_THRU_SBRIDGE
	irqLevel |= (1 << pci_irq_table[0][PCI_SLOT0_DEVICE_ID-PCI_MIN_SLOT][0]);
	irqLevel |= (1 << pci_irq_table[0][PCI_SLOT0_DEVICE_ID-PCI_MIN_SLOT][1]);
	irqLevel |= (1 << pci_irq_table[0][PCI_SLOT0_DEVICE_ID-PCI_MIN_SLOT][2]);
	irqLevel |= (1 << pci_irq_table[0][PCI_SLOT0_DEVICE_ID-PCI_MIN_SLOT][3]);
#endif
	irqLevel |= (1 << pci_irq_table[0][M1543C_PMU_DEVICE_ID-PCI_MIN_SLOT][0]);
	irqLevel |= (1 << pci_irq_table[0][M1543C_USB_DEVICE_ID-PCI_MIN_SLOT][0]);
	outportb( NULL, &pEisaControlBase->Interrupt1EdgeLevel,irqLevel & 0x0ff);
	outportb( NULL, &pEisaControlBase->Interrupt2EdgeLevel,irqLevel / 0x100);
	debug(("IRQ Level %x\n",irqLevel));

	/* Enable/Disable PS2 Mouse */
	cEnable = (UCHAR)(M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,IORC),sizeof(UCHAR)) & ~0xc0);		/* Clear PS2 keyboard/mouse bits. */
	if (IRQFORSYS(CsIrqReserveTable,PS2_INTERRUPT))
		cEnable |= 0x40;																						/* Enable PS2 Mouse */
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,IORC), cEnable, sizeof(UCHAR));						/* 0x80 Enables PS2-KB bit. */
	
	/* Enable/Disable PMU */
	cEnable = (UCHAR)(M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,SMCCII),sizeof(cEnable)) & ~0x04);	/* Enable PMU controller. */
	if (STEERABLEIRQDISABLED(CsSteerableIrqTable,STEERABLE_PMU_A_INDEX))
		cEnable |= 0x04;																						/* Disable PMU Controller. */
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,SMCCII), cEnable, sizeof(cEnable));	
	debug(("PMU Enable Value %x %x\n",cEnable,M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,SMCCII), sizeof(cEnable))));

	/*
	// Enable/Disable USB and setup for HCT.
	*/
	sEnable = (USHORT)(M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,IOPWII),sizeof(USHORT)) & ~0x4000);	/* Enable USB controller. */
	if (STEERABLEIRQDISABLED(CsSteerableIrqTable,STEERABLE_USB_A_INDEX))
		sEnable |= 0x4000;					/* Disable USB controller. */
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,IOPWII), sEnable, sizeof(sEnable));

#if M1535C
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,PCSDS), M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,PCSDS),sizeof(UCHAR))&~0x08, sizeof(UCHAR));	/* Set PCS2J/GPO29 to GPO29. */
#endif	
#if M1543C
	/*
	// Enable/Disable SMB Interrupt and setup for HCT.
	*/
	cEnable = (UCHAR)M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,SMBIR),sizeof(cEnable)) & ~0xe0;		/* Clear out enable bits. */
	if (IRQFORSYS(CsIrqReserveTable,SMB_INTERRUPT))
		cEnable |= 0x11;																						/* Enable SMB Interrupt. */
	M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,SMBIR), cEnable, sizeof(cEnable));	
#endif
	/*
	// Enable/Disable IDE and setup for HCT.
	*/
	cEnable = (UCHAR)(M1543PciIsaReadConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,IDEIC),sizeof(cEnable))) & ~0x4d;		/* Clear all bits for IDE. */

	if (
	    (IRQFORSYS(CsIrqReserveTable,IDE0_INTERRUPT)) ||
	    (IRQFORSYS(CsIrqReserveTable,IDE1_INTERRUPT))
	   )
	{
		cEnable |= 0x41;					/* Enable the IDE Controller state. */

		if (IRQFORSYS(CsIrqReserveTable,IDE0_INTERRUPT))
			cEnable |= 0x04;
		if (IRQFORSYS(CsIrqReserveTable,IDE1_INTERRUPT))
			cEnable |= 0x08;

		debug(("IDE Enable Reg %x\n",cEnable));
		M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,IDEIC), cEnable, sizeof(cEnable));					/* Set the IDE Controller state. */
	}
	else
	{
		M1543PciIsaWriteConfig(OFFSETOF(M1543_PCI_ISA_CONFIG,IDEIC), cEnable, sizeof(cEnable));					/* Set the IDE Controller state. */
	}

	debug(("Southbridge DeviceInit Done\n"));
}

void AllocateRouteTables(void)
{
	static char pciInterrupts[] = {0x01,0x03,0x04,0x05,0x06,0x07,0x09,0x0a,0x0b,0x0c,0x0e,0x0f};
	int irqEntry, irqNumber;
	int line, device, steerableIrq;
	PCHAR VarString;

	debug(("AllocateRouteTables %s %s\n",CsIrqReserveTable,CsSteerableIrqTable));

	for (line = 0; line < ARRAYSIZE(pciInterrupts); line++)							/* Check one of our PCI router interrupts is available. */
	{
		if (IRQFORPNP(CsIrqReserveTable,pciInterrupts[line]))
			break;
	}

	if (line < ARRAYSIZE(pciInterrupts))											/* Something free so allocate. */
	{
		/* So set up line zero for each device. */
		for (irqEntry = 0, steerableIrq = 0; steerableIrq < STEERABLE_IRQS; steerableIrq++)
		{
			if (STEERABLEIRQAUTO(CsSteerableIrqTable,steerableIrq))
			{
				while (!IRQFORPNP(CsIrqReserveTable,pciInterrupts[irqEntry]))		/* This will eventually come true, because we tested earlier. */
					irqEntry = (irqEntry+1) % ARRAYSIZE(pciInterrupts);				/* Look for the next one. */

				irqNumber = pciInterrupts[irqEntry];
				irqEntry = (irqEntry+1) % ARRAYSIZE(pciInterrupts);					/* Move to the next one. */
			}
			else
			{
				if (STEERABLEIRQDISABLED(CsSteerableIrqTable,steerableIrq))
					irqNumber = 0xff;
				else
					irqNumber = STEERABLEIRQVALUE(CsSteerableIrqTable,steerableIrq);
			}

			pci_irq_table[0][steerableIrqSetup[steerableIrq].deviceId-PCI_MIN_SLOT][steerableIrqSetup[steerableIrq].deviceLine] = irqNumber;
		}

#if PCISLOTIRQ_THRU_SBRIDGE
		/* Now for the 'Slots' copy into the other entrys.*/
		for (device = PCI_SLOT0_DEVICE_ID; device <= PCI_SLOT3_DEVICE_ID; device++)
		{
			for (line = ('B'-'A'); line <= ('D'-'A'); line++)						/* Thats self documenting if ever I saw a bit of code. */
				pci_irq_table[0][device-PCI_MIN_SLOT][line] = pci_irq_table[0][device+line-(((device+line)>PCI_SLOT3_DEVICE_ID)?4:0-PCI_MIN_SLOT)][0];
		}
	
#if NAUTILUS
		/* And a little fix up for the AGP lines. */
		pci_irq_table[0][NAUTILUS_AGP_SLOT0_DEVICE_ID-PCI_MIN_SLOT][0] = pci_irq_table[0][PCI_SLOT0_DEVICE_ID-PCI_MIN_SLOT][0];
		pci_irq_table[0][NAUTILUS_AGP_SLOT0_DEVICE_ID-PCI_MIN_SLOT][1] = pci_irq_table[0][PCI_SLOT0_DEVICE_ID-PCI_MIN_SLOT][1];
#endif
#endif
	}
	else
	{
		debug(("No IRQs for PCI slots.\n"));
	}

#if DEBUG_SBRIDGE
	for (device = PCI_MIN_SLOT; device <= PCI_MAX_SLOT; device++)
		debug(("Device %d uses IRQ [%x][%x][%x][%x]\n",device,pci_irq_table[0][device-PCI_MIN_SLOT][0],pci_irq_table[0][device-PCI_MIN_SLOT][1],pci_irq_table[0][device-PCI_MIN_SLOT][2],pci_irq_table[0][device-PCI_MIN_SLOT][3]));
#endif
}

VOID M7101PmuInitialize(void)
/*++

Routine Description:

    This function is called to configure the built-in Acer Labs M1543C PMU interface 
    
    It sets up some stuff specific for the Nautilus Motherboard.

Arguments:


Return Value:

    None.

--*/

{
	ASSERT(sizeof(M7101_PMU_CONFIG) == 0x100);	/* Assert structure filled correctly. */

#if !SHARK
	/*
	// Set up the Acer M1543C interface to enable GPIO[1:0] as outputs..
	*/
#if M1543C
	M7101PmuWriteConfig(OFFSETOF(M7101_PMU_CONFIG,DCGPIO), M7101PmuReadConfig(OFFSETOF(M7101_PMU_CONFIG,DCGPIO), sizeof(UCHAR))|0x43, sizeof(UCHAR));			/* Set GPIO[0:1] Pins as outputs. */
#endif
#if M1535C
	M7101PmuWriteConfig(OFFSETOF(M7101_PMU_CONFIG,MULTI_BLK1), M7101PmuReadConfig(OFFSETOF(M7101_PMU_CONFIG,MULTI_BLK1),sizeof(UCHAR))|0x02, sizeof(UCHAR));	/* Set GPI32/GPO39 to GPO39. */
#endif
#endif
}

void SBridgeWritePciConfig(
IN ULONG deviceNumber,
IN ULONG offset,
IN ULONG uLongVal,
IN ULONG size)
{
	struct pb pb;

	pb.hose = 0;
	pb.bus = 0;
	pb.slot = deviceNumber;
	pb.function = 0;

	switch (size)
	{
		case sizeof(UCHAR):
			outcfgb(&pb,offset,uLongVal);
			break;

		case sizeof(USHORT):
			outcfgw(&pb,offset,uLongVal);
			break;

		case sizeof(ULONG):
			outcfgl(&pb,offset,uLongVal);
			break;
	}
}

ULONG SBridgeReadPciConfig(
IN ULONG deviceNumber,
IN ULONG offset,
IN ULONG size)
{
	ULONG result;
	struct pb pb;

	pb.hose = 0;
	pb.bus = 0;
	pb.slot = deviceNumber;
	pb.function = 0;

	switch (size)
	{
		case sizeof(UCHAR):
			result = incfgb(&pb,offset);
			break;

		case sizeof(USHORT):
			result = incfgw(&pb,offset);
			break;

		case sizeof(ULONG):
			result = incfgl(&pb,offset);
			break;
	}

	return(result);
}


/*+
 * ============================================================================
 * = AcerM1543C_init - M1543C Super I/O controller initialization                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine configures and enables device functions on the
 *	M1543c Super I/O controller.
 *
 * FORM OF CALL:
 *
 *	AcerM1543C_init(); 
 *
 * RETURNS:                          
 *
 *      Nothing
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
void AcerM1543C_init (void)
{
	if (M1543c_detect() != NULL)
	{
		M1543cDisableDevice(SERIAL_0);
		M1543cConfigureDevice(SERIAL_0,
							  COM1_BASE,
							  COM1_IRQ,
							  -1);
		M1543cEnableDevice(SERIAL_0);

		M1543cDisableDevice(SERIAL_1);
		M1543cConfigureDevice(SERIAL_1,
							  COM2_BASE,
							  COM2_IRQ,
							  -1);
		M1543cEnableDevice(SERIAL_1);

		M1543cDisableDevice(PARALLEL_0);
		M1543cConfigureDevice(PARALLEL_0,
							  PARP_BASE,
							  PARP_IRQ,
							  PARP_DRQ);
		M1543cEnableDevice(PARALLEL_0);

		M1543cDisableDevice(FLOPPY_0);
#if !SHARK
		M1543cConfigureDevice(FLOPPY_0,
							  FDC_BASE,
							  FDC_IRQ,
							  FDC_DRQ);
		M1543cEnableDevice(FLOPPY_0);
#endif

		M1543cDisableDevice(KBD_CTRL_0);
/*#if !SHARK*/
		M1543cConfigureDevice(KBD_CTRL_0,
							  UNUSED_OPTION,
							  KBD_IRQ,
							  UNUSED_OPTION);

		M1543cSetDeviceOption(KBD_CTRL_0,
							  M1543c_SECOND_INTERRUPT_INDEX,
							  PS2_IRQ);
		M1543cEnableDevice(KBD_CTRL_0);
/*#endif*/

#if M1543C_DEBUG
		qprintf("M1543c Super I/O Controller found @ %x\n", M1543c_detect());
#endif
	}
	else
	{
#if M1543C_DEBUG
		qprintf("No M1543c Super I/O Controller found\n");
#endif
	}
}


/*+
 * ============================================================================
 * = init_serial_port - Initialize a serial port                              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine initializes a serial port.
 *
 * FORM OF CALL:
 *
 *	init_serial_port(pb, port, baud); 
 *
 * RETURNS:                          
 *
 *      Nothing
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
void init_serial_port(
struct pb *pb,
int port,
int baud)
{
	char temp;

	/* Access divisor latches */
	outportb(pb, port + LCR, LCR$M_DLA);

	/* Set the line speed */
	if (baud == 0)
		baud = (SimpleComDebug()?BAUD_115200:BAUD_9600);

#if TINOSA
	baud = BAUD_7200;	/* Fix for early Tinosa. */
#endif

	outportb(pb, port + DLL, baud >> 0);
	outportb(pb, port + DLM, baud >> 8);

	/* Set line control to 8 bit, 1 stop bit, no parity */
	outportb(pb, port + LCR, CHAR_8BIT | LCR$M_SBS);

	/* Set modem control to DTR, RTS active, OUT1 low, OUT2 enabled */
	outportb(pb, port + MCR, MCR$M_DTR | MCR$M_RTS | MCR$M_OUT1 | MCR$M_OUT2); 

	/* Turn FIFOs on */
	outportb(pb, port + FCR, FCR$M_FE | FCR$M_RFR | FCR$M_TFR);

	/* Flush the receive buffer */
	while (inportb(pb, port + LSR) & LSR$M_DR) 
		temp = inportb(pb, port + RBR);                                                          
}

#if DEBUG_SBRIDGE
void DumpRegistersSBridge(void)
{
	int device;

	for (device = 0; device < ARRAYSIZE(pci_irq_table[0]); device++)
		qprintf("Device %d uses IRQ [%x][%x][%x][%x]\n",device,pci_irq_table[0][device-PCI_MIN_SLOT][0],pci_irq_table[0][device-PCI_MIN_SLOT][1],pci_irq_table[0][device-PCI_MIN_SLOT][2],pci_irq_table[0][device-PCI_MIN_SLOT][3]);
}
#endif

static struct TABLE_ENTRY *IsaDeviceInfo(
char *handle)
{
	static struct TABLE_ENTRY isaDevice;

	isacfg_get_device(handle,1,&isaDevice);								/* Get Ptr to device info */

	return(&isaDevice);
}

#if M1535C
VOID M5451AudioInitialize(VOID)
{
	ULONG dwValue;
	PAUDREG pAudioReg;

	/* qprintf("PCI-Audio Galaxy Configure (ACER M1535D)\r\n");  */

	/* Set up the Acer M1543C interface to enable sound device.. */
    	dwValue = M5451AudioReadConfig(0x44, sizeof(ULONG));
	dwValue = (dwValue & 0xff00ff00 ) | 0x000800aa;
	M5451AudioWriteConfig(0x44, dwValue, sizeof(ULONG));

	pAudioReg = 
	(PAUDREG) (M5451AudioReadConfig(0x10, sizeof(ULONG)) &0xfffffffc);
		/*  Get Audio Base IO */

	dwValue = inportl(NULL, &(pAudioReg->GC_CIR));
	dwValue = ( dwValue & 0xfffe00ff ) | 0x000100100;
	outportl(NULL, &(pAudioReg->GC_CIR), dwValue);	/* 0xA0 */

         /* Disable INT which were used by 32 channels, 0xA4 */
	outportl(NULL, &(pAudioReg->AINTEN), 0x00000000);

	/* Clear INT of 32 channels, 0x98 */
	outportl(NULL, &(pAudioReg->AINT), 0xffffffff);
	/* Connect MPUR0 to MIDI-OUT FIFO, 0x22 */
	outportb(NULL, &(pAudioReg->MPUR2), 0x10); 
}
#endif
