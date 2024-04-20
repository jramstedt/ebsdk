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

#include	"cp$src:platform.h"
#include	"cp$inc:platform_io.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:common_def.h"
#include	"cp$src:pb_def.h"
#include	"cp$src:common.h"	/* uint64 */
#include	"cp$src:tinosa_nbridge.h"
#include	"cp$src:m1543c_sbridge.h"
#include	"cp$src:goby.h"

#include	"cp$src:nt_types.h"

#define GARTTABLESIZE	GARTPAGESIZE+sizeof(GARTPDT)+(sizeof(GARTPT)*(DRAM_PHYSICAL_LENGTH/(GARTPAGESIZE*ARRAYSIZE(pGartPT->pde))))

ULONG
READ_GART_ULONG(
    PULONG Register
    );

/*  P R I V A T E   V A R I A B L E S
//
//  P U B L I C   F U N C T I O N   D E F I N I T I O N S
//
*/

VOID GobyInitialize(void)

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
	GOBY_COMMAND command;
	GOBY_PCIARBITRATION pciArbitration;
	GOBY_BIUCONTROLSTATUS biuControlStatus;
	GOBY_MROCONTROL mroControl;

	DbgPrint("Goby Configure...\n");

	/* Enable Serr Reporting. */
	command.AsUSHORT = (USHORT)NBridgeReadPciConfig(GOBY_PCI_DEVICE_ID,OFFSETOF(GOBY_CONFIG,Command),sizeof(command.AsUSHORT));
	command.u.serrEnable = 1;
	NBridgeWritePciConfig(GOBY_PCI_DEVICE_ID,OFFSETOF(GOBY_CONFIG,Command),command.AsUSHORT,sizeof(command.AsUSHORT));

	/* Enable power management, make EV6 Mode etc. */
	pciArbitration.AsULONG = NBridgeReadPciConfig(GOBY_PCI_DEVICE_ID,OFFSETOF(GOBY_CONFIG,PciArbitrationControl),sizeof(pciArbitration.AsULONG));
	pciArbitration.u.ev6Mode = 1;							/* EV6 Mode (RW). When set, this bit indicates that the PCI interface decodes memory hits in the EV6 mode. There are no memory holes and DMA can be performed on any address that lies within the SDRAM map. 0 =x86 mode (default) 1 =EV6 mode */
	NBridgeWritePciConfig(GOBY_PCI_DEVICE_ID,OFFSETOF(GOBY_CONFIG,PciArbitrationControl),pciArbitration.AsULONG,sizeof(pciArbitration.AsULONG));

	/* Set probe limit. */
	biuControlStatus.AsULONG = NBridgeReadPciConfig(GOBY_PCI_DEVICE_ID,OFFSETOF(GOBY_CONFIG,BIUControlStatus),sizeof(biuControlStatus.AsULONG));
	biuControlStatus.u.reserved2 = 0xb4;					/* Kyuwons Special Values. */
	biuControlStatus.u.probeEnable = 1;						/* Enable probe. */
	NBridgeWritePciConfig(GOBY_PCI_DEVICE_ID,OFFSETOF(GOBY_CONFIG,BIUControlStatus),biuControlStatus.AsULONG,sizeof(biuControlStatus.AsULONG));

#ifdef GARTENABLED
	uLongVal = NBridgeReadPciConfig(GOBY_PCI_DEVICE_ID,GOBY_0_AGPVAS,sizeof(uLongVal));
	NBridgeWritePciConfig(GOBY_PCI_DEVICE_ID,GOBY_0_AGPVAS,uLongVal|0x1|(TINOSA_VASVALUE<<1),sizeof(uLongVal));	/* Set AGP window to TINOSA_GART_SIZE. */
#endif

	mroControl.AsUSHORT = (USHORT)NBridgeReadPciConfig(GOBY_PCI_DEVICE_ID,OFFSETOF(GOBY_CONFIG,MROControl),sizeof(mroControl.AsUSHORT));
	mroControl.u.reorderDisable = 0;						/* Bit 0 Reorder Disable (RW) 0 = The MRO (Memory Reorder Buffer) reorders memory requests to optimize memory performance 1 = The MRO does not reorder memory requests (default) */
	mroControl.u.pciBlockWriteEnable = 1;					/* Bit 9 PCI Block Write Enable (RW) */
	mroControl.u.pipelineEnable = 1;						/* Bit 10 Pipeline Enable (RW). Pipelined probes to same cacheline as address. */
	NBridgeWritePciConfig(GOBY_PCI_DEVICE_ID,OFFSETOF(GOBY_CONFIG,MROControl),mroControl.AsUSHORT,sizeof(mroControl.AsUSHORT));	

	DbgPrint("Done\n");
}

void NBridgeWritePciConfig(
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

ULONG NBridgeReadPciConfig(
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

#ifdef GARTDMAWINDOW
void SetUpGartDmaWindow(void)
{
	PGARTPDT pGartPDT = (PGARTPDT)FW_GART_BASE_PTR;
	ULONG uLongVal;
	GOBY_COMMAND command;
	PGOBY_GARTREG pGobyGartReg;
	GOBY_GARTREG gobyGartReg;
	PGARTPDE pGartPDE;
	PGARTPT pGartPT;
	PCHAR pMemPtr;
	int ptEntry;

	// Enable Response To Memory Accesses.
	command.AsUSHORT = (USHORT)NBridgeReadPciConfig(GOBY_PCI_DEVICE_ID,GOBY_0_COMMAND,sizeof(command.AsUSHORT));
	command.pciMemory = 1;
	NBridgeWritePciConfig(GOBY_PCI_DEVICE_ID,GOBY_0_COMMAND,command.AsUSHORT,sizeof(command.AsUSHORT));

	// Set AGP window to 1GB in size
	uLongVal = NBridgeReadPciConfig(GOBY_PCI_DEVICE_ID,GOBY_0_AGPVAS,sizeof(uLongVal));
	NBridgeWritePciConfig(GOBY_PCI_DEVICE_ID,GOBY_0_AGPVAS,uLongVal|0x1|(GARTVASVALUE<<1),sizeof(uLongVal));

	// Set AGP window to GARTWINDOWSTART base address.
	NBridgeWritePciConfig(GOBY_PCI_DEVICE_ID,GOBY_0_BAR0_AGP,GARTWINDOWSTART,sizeof(ULONG));

	// Get the GART registers.
	pGobyGartReg = (PGOBY_GARTREG)(NBridgeReadPciConfig(GOBY_PCI_DEVICE_ID,GOBY_0_BAR1_GART,sizeof(pGobyGartReg)) & ~0xf);
	DbgPrint("pGobyGartReg=%x\n",pGobyGartReg);

	pGartPT = (PGARTPT)(pGartPDT+1);

	ASSERT(GARTTABLESIZE <= (FW_GART_UPPER_BOUND-FW_GART_BASE));
	DbgPrint("pGartPDT=%x, pGartPT=%x, Allocated=%x\n",pGartPDT,pGartPT,GARTTABLESIZE);

	// Reset the directory to empty.
	memset(pGartPDT,'\0',sizeof(*pGartPDT));

	// Now Just Fill The Slots.
	for (pGartPDE = (((PGARTPDE)pGartPDT)+(GARTWINDOWSTART/(GARTPAGESIZE*ARRAYSIZE(pGartPT->pde)))), pMemPtr = 0;
		pMemPtr < (PCHAR)DRAM_PHYSICAL_LENGTH;
		pGartPT+=1,pGartPDE+=1)
	{
		pGartPDE->present = 1;
		pGartPDE->pageBaseAddress = (((ULONG)pGartPT & ~KSEG0_BASE)/GARTPAGESIZE);

		// Reset the table to empty.
		memset(pGartPT,'\0',sizeof(*pGartPT));

		// Now fill the page table entrys.
		for (ptEntry = 0; ptEntry < ARRAYSIZE(pGartPT->pde); ptEntry++,pMemPtr+=GARTPAGESIZE)
		{
			pGartPT->pde[ptEntry].present = 1;
			pGartPT->pde[ptEntry].pageBaseAddress = ((ULONG)pMemPtr/GARTPAGESIZE);
		}
	}

	// Now setup the Gart registers.
	WRITE_REGISTER_ULONG(&(pGobyGartReg->baseAddress),((ULONG)pGartPDT & ~KSEG0_BASE));

	gobyGartReg.enableStatus.AsUSHORT = READ_REGISTER_USHORT((PUSHORT)&(pGobyGartReg->enableStatus));
	gobyGartReg.enableStatus.gartCacheEnable = 1;
	WRITE_REGISTER_USHORT((PUSHORT)&(pGobyGartReg->enableStatus),gobyGartReg.enableStatus.AsUSHORT);

	gobyGartReg.cacheControl.cacheInvalidate = 1;
	WRITE_REGISTER_USHORT((PUSHORT)&(pGobyGartReg->cacheControl),gobyGartReg.cacheControl.AsUSHORT);

	// Dump Gart Registers.
	DbgPrint("Gart Registers\n");
	DbgPrint("capabilities=%x\n",READ_REGISTER_USHORT((PUSHORT)&(pGobyGartReg->capabilities)));
	DbgPrint("enableStatus=%x\n",READ_REGISTER_USHORT((PUSHORT)&(pGobyGartReg->enableStatus)));
	DbgPrint("baseAddress=%x\n",READ_REGISTER_ULONG((PULONG)&(pGobyGartReg->baseAddress)));
	DbgPrint("cacheSize=%x\n",READ_REGISTER_ULONG((PULONG)&(pGobyGartReg->cacheSize)));
	DbgPrint("cacheControl=%x\n",READ_REGISTER_ULONG((PULONG)&(pGobyGartReg->cacheControl)));
	DbgPrint("entryControl=%x\n",READ_REGISTER_ULONG((PULONG)&(pGobyGartReg->entryControl)));
	DbgPrint("pm2=%x\n",READ_REGISTER_ULONG((PULONG)&(pGobyGartReg->pm2)));
}
#endif // GARTDMAWINDOW
