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
#include	"cp$src:ev_def.h"
#include	"cp$src:common.h"	/* uint64 */
#include	"cp$src:nautilus_nbridge.h"
#include	"cp$src:m1543c_sbridge.h"
#include	"cp$src:irongate.h"
#include	"cp$src:nt_types.h"

#define GARTTABLESIZE	GARTPAGESIZE+sizeof(GARTPDT)+(sizeof(GARTPT)*(DRAM_PHYSICAL_LENGTH/(GARTPAGESIZE*ARRAYSIZE(pGartPT->pde))))
#define EV_GART_SIZE 0x18

extern struct set_param_table ev_gart_table[];

ULONG
READ_GART_ULONG(
    PULONG Register
    );

/*  P R I V A T E   V A R I A B L E S
//
//  P U B L I C   F U N C T I O N   D E F I N I T I O N S
//
*/

VOID IrongateInitialize(void)

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
	USHORT uShortVal;
	ULONG uLongVal;
	IRONGATE_COMMAND command;
	IRONGATE_PCIARBITRATION pciArbitration;
	IRONGATE_BIUCONTROLSTATUS biuControlStatus;

	/* Enable Serr Reporting. */
	command.AsUSHORT = (USHORT)NBridgeReadPciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_COMMAND,sizeof(command.AsUSHORT));
	command.u.serrEnable = 1;
	NBridgeWritePciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_COMMAND,command.AsUSHORT,sizeof(command.AsUSHORT));
	
	/* Enable power management, make EV6 Mode etc. */
	pciArbitration.AsULONG = NBridgeReadPciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_PCIARBITRATION,sizeof(pciArbitration.AsULONG));
	pciArbitration.u.ev6Mode = 1;							/* EV6 Mode (RW). When set, this bit indicates that the PCI interface decodes memory hits in the EV6 mode. There are no memory holes and DMA can be performed on any address that lies within the SDRAM map. 0 =x86 mode (default) 1 =EV6 mode */
	NBridgeWritePciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_PCIARBITRATION,pciArbitration.AsULONG,sizeof(pciArbitration.AsULONG));

	/* Set probe limit. */
	biuControlStatus.AsULONG = NBridgeReadPciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_BIUSTATUSCONTROL,sizeof(biuControlStatus.AsULONG));
	biuControlStatus.u.reserved2 = 0xb4;					/* Kyuwons Special Values. */
	biuControlStatus.u.probeEnable = 1;						/* Enable probe. */
	NBridgeWritePciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_BIUSTATUSCONTROL,biuControlStatus.AsULONG,sizeof(biuControlStatus.AsULONG));


	uShortVal = (USHORT)NBridgeReadPciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_MROCONTROL,sizeof(uShortVal));
	NBridgeWritePciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_MROCONTROL,uShortVal|0x0600,sizeof(uShortVal));	/* Set magic bits to TURN off AMD debug. */

	DbgPrint(("Done\n"));
}

int ev_gart_size(
char *name,
struct EVNODE *ev)
{
	int i;

	for (i=0; ev_gart_table[i].param != 0; i++)
	{
		if (!strcmp(ev->value.string, ev_gart_table[i].param))
		{
			rtc_write(EV_GART_SIZE, ev_gart_table[i].code);
			return(msg_success);
		}
	}

	return(msg_failure);	
}

ev_gart_size_init(
struct env_table *et,
char *value)
{
	char gart_size;
	int i;

	gart_size = rtc_read(EV_GART_SIZE);

	for (i=0; ev_gart_table[i].param != 0; i++)
	{
		if (gart_size == ev_gart_table[i].code)
		{
			return(ev_gart_table[i].param);
		}
	}

	return (ev_gart_table[0].param);
}

void NBridgeSetGartSize(void)
{
	ULONG uLongVal;
	struct EVNODE *evp_gartsize;
	ULONG nautilusVasValue = rtc_read(EV_GART_SIZE);

	nautilusVasValue = min(nautilusVasValue,5);

	DbgPrint(("nautilusVasValue = %x\n",nautilusVasValue));

	if (nautilusVasValue)
	{
		nautilusVasValue--;				/* Index from zero */
		uLongVal = NBridgeReadPciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_AGPVAS,sizeof(uLongVal));
		NBridgeWritePciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_AGPVAS,uLongVal|0x1|(nautilusVasValue<<1),sizeof(uLongVal));	/* Set AGP window to NAUTILUS_GART_SIZE. */
	}
}

void NBridgeWritePciConfig(
unsigned long deviceNumber,
unsigned long offset,
unsigned long uLongVal,
unsigned long size)
{
	struct pb pb;

	pb.hose = 0;
	pb.bus = (deviceNumber == NAUTILUS_AGP_SLOT0_DEVICE_ID)?2:0;
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
unsigned long deviceNumber,
unsigned long offset,
unsigned long size)
{
	ULONG result;
	struct pb pb;

	pb.hose = 0;
	pb.bus = (deviceNumber == NAUTILUS_AGP_SLOT0_DEVICE_ID)?2:0;
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
	IRONGATE_COMMAND command;
	PIRONGATE_GARTREG pIrongateGartReg;
	IRONGATE_GARTREG irongateGartReg;
	PGARTPDE pGartPDE;
	PGARTPT pGartPT;
	PCHAR pMemPtr;
	int ptEntry;

	// Enable Response To Memory Accesses.
	command.AsUSHORT = (USHORT)NBridgeReadPciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_COMMAND,sizeof(command.AsUSHORT));
	command.pciMemory = 1;
	NBridgeWritePciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_COMMAND,command.AsUSHORT,sizeof(command.AsUSHORT));

	// Set AGP window to 1GB in size
	uLongVal = NBridgeReadPciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_AGPVAS,sizeof(uLongVal));
	NBridgeWritePciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_AGPVAS,uLongVal|0x1|(GARTVASVALUE<<1),sizeof(uLongVal));

	// Set AGP window to GARTWINDOWSTART base address.
	NBridgeWritePciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_BAR0_AGP,GARTWINDOWSTART,sizeof(ULONG));

	// Get the GART registers.
	pIrongateGartReg = (PIRONGATE_GARTREG)(NBridgeReadPciConfig(NAUTILUS_PCI_DEVICE_ID,IRONGATE_0_BAR1_GART,sizeof(pIrongateGartReg)) & ~0xf);
	DbgPrint("pIrongateGartReg=%x\n",pIrongateGartReg);

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
	WRITE_REGISTER_ULONG(&(pIrongateGartReg->baseAddress),((ULONG)pGartPDT & ~KSEG0_BASE));

	irongateGartReg.enableStatus.AsUSHORT = READ_REGISTER_USHORT((PUSHORT)&(pIrongateGartReg->enableStatus));
	irongateGartReg.enableStatus.gartCacheEnable = 1;
	WRITE_REGISTER_USHORT((PUSHORT)&(pIrongateGartReg->enableStatus),irongateGartReg.enableStatus.AsUSHORT);

	irongateGartReg.cacheControl.cacheInvalidate = 1;
	WRITE_REGISTER_USHORT((PUSHORT)&(pIrongateGartReg->cacheControl),irongateGartReg.cacheControl.AsUSHORT);

	// Dump Gart Registers.
	DbgPrint("Gart Registers\n");
	DbgPrint("capabilities=%x\n",READ_REGISTER_USHORT((PUSHORT)&(pIrongateGartReg->capabilities)));
	DbgPrint("enableStatus=%x\n",READ_REGISTER_USHORT((PUSHORT)&(pIrongateGartReg->enableStatus)));
	DbgPrint("baseAddress=%x\n",READ_REGISTER_ULONG((PULONG)&(pIrongateGartReg->baseAddress)));
	DbgPrint("cacheSize=%x\n",READ_REGISTER_ULONG((PULONG)&(pIrongateGartReg->cacheSize)));
	DbgPrint("cacheControl=%x\n",READ_REGISTER_ULONG((PULONG)&(pIrongateGartReg->cacheControl)));
	DbgPrint("entryControl=%x\n",READ_REGISTER_ULONG((PULONG)&(pIrongateGartReg->entryControl)));
	DbgPrint("pm2=%x\n",READ_REGISTER_ULONG((PULONG)&(pIrongateGartReg->pm2)));
}
#endif // GARTDMAWINDOW
