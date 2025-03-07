/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
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

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: pci.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * PCI specific routines.
 *
 * $Log: pci.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.42  1998/06/26  18:48:31  gries
 * start work on second pci
 *
 * Revision 1.41  1997/06/09  15:43:54  fdh
 * Corrected a few unsigned comparisons.
 *
 * Revision 1.40  1997/06/02  04:34:53  fdh
 * Modified to handle "expected machine checks" during
 * PCI bus configuration.  No longer requires raising
 * IPL to avoid the machine checks.
 *
 * Revision 1.39  1997/05/28  15:19:12  fdh
 * Corrected data type assignments.
 *
 * Revision 1.38  1997/05/16  02:07:06  fdh
 * Modified to map multi-function devices.
 *
 * Revision 1.37  1996/08/19  04:14:04  fdh
 * Fix when bridges don't require and address space recources.
 * Cleaned up PCIShow output.
 *
 * Revision 1.36  1996/08/16  15:39:45  fdh
 * Modified to map extended PCI ROM base addresses.
 * Enable VGA compatible address support for bridges
 * in front of the VGA card.
 * Fixed the PCIDevicePresent() and PCIMultiFunction()
 * functions to work better with bridges.
 * Added PCIGetNumberOfBusses() function.
 *
 * Revision 1.35  1996/08/11  12:01:32  fdh
 * Enable Target Abort reporting for pci-pci bridges.
 * Enable PCI system error reporting for pci-pci bridges
 * and primary bus.
 *
 * Revision 1.34  1996/08/11  10:29:31  fdh
 * Corrected args passed to outcfgl when writing the
 * subordinate bus number to pci-pci bridge devices.
 * The function and register arguments were swapped.
 *
 * Revision 1.33  1996/08/10  01:46:37  fdh
 * Insure that any no device errors are cleared
 * before probing for the next device.
 *
 * Revision 1.32  1996/08/09  04:49:44  fdh
 * Added mask when reading secondary status register.
 *
 * Revision 1.31  1996/08/07  18:31:08  fdh
 * Added PCIDumpDevice().
 *
 * Revision 1.30  1996/06/14  18:06:24  fdh
 * Removed unreferenced local variables.
 *
 * Revision 1.29  1996/06/13  03:26:43  fdh
 * Added additional debug tracing.
 *
 * Revision 1.28  1996/06/03  17:47:03  fdh
 * Modified to ignore base address registers that don't
 * report the space allocation.  i.e. what could happen
 * when a device is not in PCI native mode when this code
 * is reached.
 *
 * Revision 1.27  1996/05/31  20:01:30  fdh
 * Init command variable with current setting.
 *
 * Revision 1.26  1996/05/31  13:06:48  fdh
 * Added some additional DEBUG tracing.
 *
 * Revision 1.25  1996/05/26  20:05:15  fdh
 * Modified a couple of variable names for clarity.
 *
 * Revision 1.24  1996/05/25  20:35:50  fdh
 * Renamed a conflicting variable.
 *
 * Revision 1.23  1996/05/24  20:06:08  fdh
 * Modified to map all Base Addresses requested by a device.
 * This code originally mapped only one I/O area and one Memory
 * area.
 *
 * Revision 1.22  1996/05/22  20:24:26  fdh
 * Added programmable definitions for PCI probe limits.
 * Disabled interrupts only during each device probe.
 * Enable only one VGA BIOS... The first one found wins.
 * Read device revision.  Begin PCI memory base allocation
 * at 16MB.  Cleaned up PCIshow output and added more device
 * class definitions.  Changed logic used in PCIDevicePresent().
 * Corrected mask used in InPciCfgW.
 *
 * Revision 1.21  1995/11/22  20:54:08  cruz
 * Moved all initialization of static variables to PCIInit().
 * This will cause the variables to be reinitialized if the
 * debug monitor is reinitialized.
 *
 * Revision 1.20  1995/10/26  23:37:11  cruz
 * Moved code from stub_pci.c to here. Cleaned up code by
 * doing casting and fixing up prototypes.
 *
 * Revision 1.19  1995/09/02  03:44:27  fdh
 * Implemented PCIDevicePresent routine to prevent
 * invalid PCI Config space accesses for the generic
 * PCI Config access space routines.
 *
 * Revision 1.18  1995/08/25  19:47:20  fdh
 * Added special conditions to support TGA and VGA.
 * Added more PCI class ID messages.
 * Implemented PCIMultiFunction().
 * Implemented generic PCI Config space access routines which
 * perform additional diagnostics.
 *
 * Revision 1.17  1995/03/07  20:58:23  cruz
 * Fixed another problem in the PCI device memory allocation
 * algorithm.  It now aligns bases to the maximum of the
 * requested size and the minimum base value.
 *
 * Revision 1.16  1995/02/23  20:41:15  cruz
 * Fixed PCI mem and IO memory allocation algorithm to align
 * mem and IO bases on a 4K and 1K byte boundary, respectively.
 * The way it was before prevented addresses in the range 256
 * to 1K bytes within each 1K chunk from being forwarded to the
 * devices on the other side of the bridge because the ISA mode
 * enable bit was set.
 *
 * Revision 1.15  1994/11/23  20:02:13  cruz
 * Modified to support two additional PCI windows.
 *
 * Revision 1.14  1994/11/18  19:09:47  fdh
 * Disable and enable interrupts when sniffing PCI slots.
 *
 * Revision 1.13  1994/11/01  11:32:28  rusling
 * Added support for PCI-PCI bridges.
 *
 * Revision 1.8  1994/06/21  10:43:53  rusling
 * Fixed WNT compile warning.
 *
 * Revision 1.7  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.6  1994/01/27  20:34:43  fdh
 * Modified PCIShow() to show true PCI target window
 * settings for and EB64+.  Registers are "write only"
 * on the EB66.
 *
 * Revision 1.5  1994/01/19  10:40:08  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.4  1993/11/29  14:58:11  rusling
 * Added PCI address checking and conversion routines,
 * PCIMapAddress() and PCIValidAddress().
 *
 * Revision 1.3  1993/11/24  15:22:51  rusling
 * Moved to using IOPCIClearNODEV() when looking to see what
 * PCI slots are filled.  This makes pci.c totally system
 * independent.
 *
 * Revision 1.2  1993/11/22  13:20:18  rusling
 * PCI support code, initial merged version.
 *
 * Revision 1.1  1993/11/22  12:18:22  rusling
 * Initial revision
 *
 *  
 */
#include "system.h"
#include "lib.h"

#ifdef NEEDPCI
/*
 * Odd definitions.
 */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef NULL
#define NULL 0
#endif

#define DIGITAL_TGA_PCI_DEVID   0x00041011   // Digital TGA

/*
 * Include files.
 */
#include "pci.h"
#include "mcheck.h"

/*
 * The root of pci devices found and the unallocated PCI devices/buses.
 */
static PCIBus_t *root = NULL;			/* tree of devices and buses  */
static PCIDevice_t *devices;		        /* linked list of all devices */
static PCIDevice_t freedevices[(PCI_T0_LAST_IDSEL-PCI_T0_FIRST_IDSEL+1)
			       + (PCI_T1_LAST_DEVICE-PCI_T1_FIRST_DEVICE+1) * PCI_MAX_BRIDGES];
static PCIBus_t  freebuses[PCI_MAX_BRIDGES];
static PCIDevice_t *allocatedevices;
static PCIBus_t *allocatebuses;
static ui PCIIOBase;
static ui PCIMemBase;

static int VgaDeviceDetected;
int DigitalTGADetected;
ul  DigitalTGABase;

/*
 * Any routines local to this module.
 */
static PCIDevice_t *PCI_allocate_device(void);
static ui PCI_slot_filled(int bus, int slot, int function);
static char *DeviceClassString(ui class);

/*
 * Any macros that are local to this module.
 * PCI_MASK_UNIT is defined in the system specific module
 * as it varies between PCI chipsets.
 */
#define _PCIMaskToSize(m) \
  ( ((m >> 20) + 1) * PCI_MASK_UNIT )

#define MAX(val1, val2) ( ((val1) > (val2)) ? val1 : val2)
#define align(value,alignment) \
	( (value & (alignment - 1)) == 0 ? value : value + (alignment - (value & (alignment - 1))) )
#define _Align(value,alignment) \
	((((value) & ~(alignment)) == 0) ? (value) : ((value) & (alignment)) + ~(alignment) + 1)
#define BAR_OFFSET(idx) ((idx<MAX_BASE_ADDRESS_REGISTERS) ? \
			 ((idx*4)+0x10) : 0x30)	/* Base addresses including ROM */

/**************************************************************************
 * PCI_slot_filled()                                                      *
 **************************************************************************/
/*
 * Check to see if a PCI slot has a device, how we do this depends on the
 * evaluation board.
 */
static ui PCI_slot_filled(int bus, int slot, int function)
{
    ui l;

/*
 *  First, clear any pending NODEV errors.
 */

    IOPCIClearNODEV();
    MachineCheckExpected = TRUE;

#if 0
/*
 *    Make sure interrupts are disabled
 *    before probing the slots
 *    because they can be expected to
 *    cause interrupts (Machine Checks).
 *    This precludes setting breakpoints
 *    in this code under the remote debugger
 *    until interrupts are re-enabled.
 */
    int save_ipl = swpipl(7);
#endif

/*  
 *  Read the vendor and device ids (first longword).
 */
    l = incfgl(bus, slot, function, PCI_CFG_REG_VENDOR_DEVICE);

/*
 *  Give the expected machine check a chance to happen
 */
    usleep(1000);

#if 0
/*
 *     Re-enable the interrupts at the previous level
 *     so that messages can reach the debug server.
 */
    swpipl(save_ipl);
#endif

/*
 *  Clear any pending PCI NODEV error and
 *  return if slot is unoccupied.
 */
    if (IOPCIClearNODEV()) return 0;

    MachineCheckExpected = FALSE;
    PRTRACE2("ExpectedMachineCheckTaken = 0x%lx\n", ExpectedMachineCheckTaken);
    if (ExpectedMachineCheckTaken) {
      PRTRACE1("Expected Machine Check Taken\n");
      ExpectedMachineCheckTaken = FALSE;
      return 0;
    }

/*
 *  Check for all 1s as the vendor id (this also means
 *  that the slot doesn't contain a device).
 */
    if ((l & 0xffff) == 0xffff) return 0;

/*
 *  The slot is occupied.
 */
    return l;
}

/**************************************************************************
 * PCISetDeviceConfig()                                                   *
 **************************************************************************/
void PCISetDeviceConfig(PCIDevice_t *device)
{
    PCIBus_t *parent;
    ui command;
    ui busno = (ui)device->parent->number;
    ui alignment;
    int idx;

    command = incfgl(busno, device->slot, device->function, PCI_CFG_REG_STATUS_COMMAND);

/*
 *  Tell the device where the bases are (and enable use of them).
 */
#ifdef PRTRACE
    printf("  ConfigDevice, Bus:%d Slot:%d Func:%d\n",
	     busno, device->slot, device->function);
#endif
    for (idx=0; idx < MAX_BASE_ADDRESS_REGISTERS+1; ++idx) {
      if (device->PCI_Bar[idx].Size == 0) continue;
      outcfgl(busno, device->slot, device->function,
	      device->PCI_Bar[idx].Reg, device->PCI_Bar[idx].Base | 0x1); 
#ifdef PRTRACE
      printf("\t%s Bar:%x, %x\n", device->PCI_Bar[idx].Type ? "I/O" : "Mem",
	       BAR_OFFSET(idx), device->PCI_Bar[idx].Base);
#endif
      if (device->PCI_Bar[idx].Type == TRUE)
	command |= 0x1;		/* Enable in I/O Space */
      else {
	if (idx < MAX_BASE_ADDRESS_REGISTERS)
	  command |= 0x2;	/* Enable in Memory Space */
	else
	  command |= 0x1;	/* Enable ROM Base */
	if ((device->vendor_id == (DIGITAL_TGA_PCI_DEVID & 0xFFFF))
	    && (device->device_id == (DIGITAL_TGA_PCI_DEVID >> 16))
	    && !DigitalTGADetected) {
	  DigitalTGABase = device->PCI_Bar[idx].Base;
	  DigitalTGADetected = TRUE;
	}
      }
    }

    if (!VgaDeviceDetected) {
      /* Enable I/O registers for only the first VGA device detected. */
      if(((device->class == 0x000100)
	  |(device->class == 0x030000)
	  |(device->class == 0x030100)) && !VgaDeviceDetected) {
	VgaDeviceDetected = TRUE;
	command |= 0x1;

/*
 *  Enable the BIOS relocating it to 0xc0000 if it is on the primary bus.
 */
	if (busno == 0) {
	  outcfgl(busno, device->slot, device->function, PCI_CFG_REG_EROM_BASE, 0xfffff000);
	  alignment = incfgl(busno, device->slot, device->function, PCI_CFG_REG_EROM_BASE);
	  outcfgl(busno, device->slot, device->function, PCI_CFG_REG_EROM_BASE,
		  _Align(0xc0000, alignment)|1);
	  device->PCI_Bar[MAX_BASE_ADDRESS_REGISTERS].Base = 0xc0000;
	}

/*
 *  Walk back up the tree, enabling VGA compatible address
 *  support for each of the bridges along the way.
 */
	parent = device->parent;
	while (parent->bridge != NULL ) {
	  outcfgl(parent->parent->number, parent->bridge->slot, parent->bridge->function, 0x3c,
		  incfgl(parent->parent->number, parent->bridge->slot, parent->bridge->function, 0x3c) | 0x80000);
	  parent = parent->parent;
	}
      }
    }
    PRTRACE2("\tCommand:%x\n", command | 4);
    outcfgl(busno, device->slot, device->function, PCI_CFG_REG_STATUS_COMMAND, command | 0x4);
}

/**************************************************************************
 * PCI_allocate_device()                                                  *
 **************************************************************************/
PCIDevice_t *PCI_allocate_device(void)
{
    PCIDevice_t *device = allocatedevices;
/*
 *  Allocate one.
 */

    allocatedevices++;
    memset((char *)device, 0, sizeof(PCIDevice_t));
    return device;
}

/**************************************************************************
 * PCIQueryDevice()                                                       *
 **************************************************************************/
void PCIQueryDevice(PCIDevice_t *device)
{
  ui bar;
  PCIBus_t *bus;
  int idx;

/*
 *  Figure out how much memory (and what sort) the given device needs.
 */
  bus = device->parent;

#ifdef PRTRACE
  printf("  QueryDevice, Bus:%d Slot:%d Func:%d\n",
	   bus->number, device->slot, device->function);
#endif
  for (idx=0; idx < MAX_BASE_ADDRESS_REGISTERS+1; ++idx) {
    outcfgl(bus->number, device->slot, device->function, BAR_OFFSET(idx), 0xffffffff);
    bar = incfgl(bus->number, device->slot, device->function, BAR_OFFSET(idx));
    if (bar == 0) continue;
    if ((int) bar > 0) continue;
    PRTRACE3("\tBar:%x, %x\n", BAR_OFFSET(idx), bar);
/*
 *  We've read the base address register back after writing all ones
 *  and so now we must decode it.
 */
    if ((bar & 0x1) && (idx < MAX_BASE_ADDRESS_REGISTERS)) {

/*
 *  I/O space address register.
 */
      device->PCI_Bar[idx].Type = TRUE;
      bar = bar & 0xFFFFFFFE;
      device->PCI_Bar[idx].Size = ~bar+1;
      device->PCI_Bar[idx].Reg = BAR_OFFSET(idx); 
      PRTRACE3("\tI/O Reg:%x, Size:%x\n",
	       device->PCI_Bar[idx].Reg, device->PCI_Bar[idx].Size);
    } else {
      ui i;
/*
 *  Memory space address register.
 */
      device->PCI_Bar[idx].Type = FALSE;
      i = (bar >> 1) & 0x3;
      if ((i == 0) | (i == 1)) {
/*
 *  32 bit wide memory address.
 */
	bar = bar & 0xfffffff0;
	device->PCI_Bar[idx].Size = ~bar+1;
	device->PCI_Bar[idx].Reg = BAR_OFFSET(idx);
	PRTRACE3("\tMem Reg:%x, Size:%x\n",
		 device->PCI_Bar[idx].Reg, device->PCI_Bar[idx].Size);
      } else {
/*
 *  64 bit wide memory address, ignore for now.
 */
      }
    }
  }
}

/**************************************************************************
 * PCI_allocate_bus()                                                     *
 **************************************************************************/
PCIBus_t *PCI_allocate_bus(void)
{
    PCIBus_t *bus = allocatebuses;
/*
 *  Allocate one.
 */
    allocatebuses++;
    memset((char *)bus, 0, sizeof(PCIBus_t));
    return bus;
}

/**************************************************************************
 * PCIInsertDevice()                                                      *
 **************************************************************************/
/*
 *  Insert the PCI device into the queue of devices.
 */
static void PCIInsertDevice(PCIBus_t *bus, PCIDevice_t *device)
{
#ifdef PRTRACE
  printf("  Inserting... bus:%d, device:%d, func:%d\n",
	   bus->number, device->slot, device->function);
#endif

  if (bus->devices != NULL) {
    PCIDevice_t *devp = bus->devices;
    while (devp->next != NULL)
      devp = devp->next;

    devp->next = device;
  }
  else {
/*
 *  The list is empty, so make this the first entry.
 */
    bus->devices = device;
  }

  device->next = NULL;
}

/**************************************************************************
 * PCIAllocateSpace()                                                     *
 **************************************************************************/
void PCIAllocateSpace(PCIBus_t *bus)
{
    ui tio, bio, tmem, bmem, l, alignto;
    PCIBus_t *child;
    PCIDevice_t *device;
    int idx;
/*
 *  Don't do anything unless this bus has any devices/bridges.
 */
    if ((bus->devices != NULL) || (bus->children != NULL)) {
/*
 *  Align the current bases on appropriate boundaries (4K for IO and
 *  1Mb for memory).
 */
	bio = PCIIOBase = align(PCIIOBase,0x1000);
	bmem = PCIMemBase = align(PCIMemBase, 0x100000);
/*
 *  Allocate space to each device.
 */
	device = bus->devices;
	while (device != NULL) {
	  if (device->bus == NULL) { /* If this is not a bridge? */
#ifdef PRTRACE
	    printf("  AllocateDevice, Bus:%d Slot:%d Func:%d\n",
		     bus->number, device->slot, device->function);
#endif
/*
 *  This is a normal device, allocate it some space.
 */
	    for (idx=0; idx < MAX_BASE_ADDRESS_REGISTERS+1; ++idx) {
	      if (device->PCI_Bar[idx].Size != 0) {
		if (device->PCI_Bar[idx].Type) {
		  alignto = MAX(0x400, device->PCI_Bar[idx].Size); /* Align to multiple of size or minimum base */
		  PCIIOBase = device->PCI_Bar[idx].Base = 
		    align(PCIIOBase, alignto);
		  PCIIOBase += alignto;	    
		}
		else {
		  alignto = MAX(0x1000, device->PCI_Bar[idx].Size); /* Align to multiple of size or minimum base */   
		  PCIMemBase = device->PCI_Bar[idx].Base = 
		    align(PCIMemBase, alignto);
		  PCIMemBase += alignto;
		}
	      }
	    }
/*
 *  Tell the device where the bases are.
 */
	    PCISetDeviceConfig(device);
	  }
	  device = device -> next;
	}
/*
 *  Allocate space for all of the sub-buses.
 */
    	child = bus->children;
    	while (child != NULL) {
	    PCIAllocateSpace(child);
	    child = child->next;
        }
/*
 *  Align the current bases on 4K and 1MByte boundaries.
 */
	tio = PCIIOBase = align(PCIIOBase,0x1000);
	tmem = PCIMemBase = align(PCIMemBase, 0x100000);
/*
 *  set up this bus's windows.
 */
	bus->PCI_IO_Size = tio - bio;
	if (bus->PCI_IO_Size != 0)
	  bus->PCI_IO_Base = bio;

	bus->PCI_Mem_Size = tmem - bmem;
	if (bus->PCI_Mem_Size != 0)
	  bus->PCI_Mem_Base = bmem;

	if (bus->bridge != NULL) {
	    PCIDevice_t *bridge = bus->bridge;
/*
 *  Set up the top and bottom of the I/O memory range for this bus.
 */
	    if (bus->PCI_IO_Size != 0) {
	      l = incfgl(bridge->parent->number, bridge->slot, bridge->function, 0x1c);
	      l = (l & 0xffff0000) | (bio >> 8) | ((tio - 1) & 0xf000);
	      outcfgl(bridge->parent->number, bridge->slot, bridge->function, 0x1c, l);
	    }

	    if (bus->PCI_Mem_Size != 0) {
	      l = ((bmem & 0xfff00000) >> 16) | ((tmem - 1) & 0xfff00000) ;
	      outcfgl(bridge->parent->number, bridge->slot, bridge->function, 0x20, l);
	    }
/*
 *  clear status bits, enable I/O (for downstream I/O), turn on master
 *  enable (for upstream I/O), turn on memory enable (for downstream 
 *  memory), turn on master enable (for upstream memory and I/O),
 *  enable PCI system error reporting.
 */
	    outcfgl(bridge->parent->number, bridge->slot, bridge->function, 0x4, 0xffff0107);
	  }
      }
}
/**************************************************************************
 * PCIScanBus()                                                           *
 **************************************************************************/
ui PCIScanBus(PCIBus_t *bus, ui min_dev, ui max_dev)
{
    ui i;
    ui l;
    ui max;
    int function;
    int bridge_flag;
    PCIDevice_t *device;
    PCIBus_t *child;

    max = bus->secondary;
    for (i = min_dev; i <= max_dev; i++) {
     for (function=0; function < 8; function++) {

	l = PCI_slot_filled(bus->number, i, function);	

	if (l != 0) {
#ifdef PRTRACE
	  printf("!!! Device detected:0x%x, Bus:%d, Slot:%d Func:%d !!!\n",
		   l, bus->number, i, function);
#endif
/*
 *  This slot (i) is filled.
 */
	    device = PCI_allocate_device();
	    device->parent = bus;
/*
 *  Put it into the simple chain of devices that is used to find 
 *  devices once everything is set up.
 */
	    device->sibling = devices;
	    devices = device;

	    device->slot = i;
	    device->function = function;
	    device->vendor_id = (uw)(l & 0xFFFF);
	    device->device_id = (uw)(l >> 16);
	    bridge_flag = FALSE;

/*
 *  Check to see if this device is a PCI-PCI bridge.
 */
	    l = incfgl(bus->number, i, function, PCI_CFG_REG_REVISION_ID);
	    device->revision = (ub) (l & 0xff);
	    l = l >> 8;			/* upper 3 bytes */
	    device->class = l;
	    bridge_flag = (l == PCI_BRIDGE_CLASS);
/*
 *  Figure out just how much I/O and memory space this device needs.
 *  Note that we must have set its parent up by this time.  We don't
 *  do this for bridges.
 */
	    if (!bridge_flag)
		PCIQueryDevice(device);
/*
 *  Now insert it into the list of devices held by the parent bus.
 *  Note that we can only do this when we know how much memory and
 *  I/O it requires as the device list is in ascending memory order.
 */
	    PCIInsertDevice(bus, device);
/*
 *  Check to see if this device is a PCI-PCI bridge.
 */
 	    if (bridge_flag) {
		ui buses;
/*
 *  Insert it into the tree of buses.
 */
		child = PCI_allocate_bus();
		child->next = bus->children;
		bus->children = child;
		child->bridge = device;
		child->parent = bus;
		device->bus = child;
/*
 *  Set up the primary, secondary and subordinate bus numbers.
 */
		max = max + 1;
		child->number = (unsigned char)max;
		child->primary = bus->secondary;
		child->secondary = (unsigned char)max;
		child->subordinate = 0xFF;
/*
 *  Init address ranges putting things out of the way temporarily.
 */
		outcfgl(bus->number, i, function, 0x1c, 0x000000f0);
		outcfgl(bus->number, i, function, 0x20, 0x0000fff0);
/*
 *  Turn off downstream PF memory address range.
 */
		outcfgl(bus->number, i, function, 0x24, 0x0000fff0);
/*
 *  Enable Target Abort and PCI system error reporting.
 */
		outcfgl(bus->number, i, function, 0x3c, 0x00220000);
/*
 *  clear all status bits and turn off memory, I/O and master enables.
 */
		outcfgl(bus->number, i, function, 0x04, 0xffff0000);
/*
 *  configure the bus numbers for this bridge.
 */
		buses = incfgl(bus->number, i, function, 0x18);
		buses = buses & 0xff000000;
		buses = buses | (ui)(child->primary) | 
			((ui)(child->secondary) << 8) | 
			((ui)(child->subordinate) << 16);
		outcfgl(bus->number, i, function, 0x18, buses);
/*
 *  scan all subordinate buses.
 */
		max = PCIScanBus(child, PCI_T1_FIRST_DEVICE, PCI_T1_LAST_DEVICE);
/*
 *  Set the subordinate bus number to it's real value.
 */
		child->subordinate = (unsigned char)max;
		buses = (buses & 0xff00ffff) | ((ui)(child->subordinate) << 16);
		outcfgl(bus->number, i, function, 0x18, buses);
	      }
/*
 *  If this is not a multi-function device move on.
 */
	  if ( !(incfgl(bus->number, i, 0, 0x0C) & 0x800000) )
	    break;
	}
	else {
#ifdef PRTRACE
	  printf("!!! Empty slot, Bus:%d, Slot:%d Func:%d !!!\n",
		   bus->number, i, function);
#endif
	  break;
	}
      }
    }
/*
 *  We've scanned the bus and so we know all about what's on the other
 *  side of any bridges that may be on this bus plus any devices.  
 *
 *  Return how far we've got finding sub-buses.
 */
    return max;
}
/**************************************************************************
 * PCIInit()                                                              *
 **************************************************************************/
/*
 * NOTE: we cannot do any printf() calls as we cannot be sure that
 * the uart has been initialised.
 */
void PCIInit(void)
{
/*
 *  Init the device allocation pointers
 *  incase the monitor is restarted.
 */
    devices = NULL;
    allocatedevices = freedevices;
    allocatebuses = freebuses;

/*
 *  Set initial values of variables.
 */
    PCIIOBase = 0x1000;
    PCIMemBase = 0x1000000;

    VgaDeviceDetected = FALSE;
    DigitalTGADetected = FALSE;

/*
 * Allocate the first bus (there's at least one!)
 */
    root = PCI_allocate_bus();
    root->number = root->primary = root->secondary = 0;
/*
 *  And then scan it and all it's sub-buses.
 */
    root->subordinate =
      (unsigned char)PCIScanBus(root, PCI_T0_FIRST_IDSEL, PCI_T0_LAST_IDSEL);

/*
 *  Now scan the tree allocating PCI memory and I/O space.
 */
    PCIAllocateSpace(root);
}

/**************************************************************************
 * PCIDeviceFind()                                                        *
 **************************************************************************/
PCIDevice_t *PCIDeviceFind(uw vendor, uw device)
{
    PCIDevice_t *PCIDevice = devices;

    while (PCIDevice != NULL) {
	if ((PCIDevice->vendor_id == vendor) && (PCIDevice->device_id == device))
	    return PCIDevice;
	PCIDevice = PCIDevice->sibling;
    }
    return NULL;
}
/**************************************************************************
 * PCIDeviceFindNext()                                                    *
 **************************************************************************/
PCIDevice_t *PCIDeviceFindNext(PCIDevice_t *PCIDevice)
{
    if (PCIDevice != NULL) {
	uw vendor, device;

	vendor = PCIDevice->vendor_id;
	device = PCIDevice->device_id;

	PCIDevice = PCIDevice->sibling;
	while (PCIDevice != NULL) {
	    if ((PCIDevice->vendor_id == vendor) && (PCIDevice->device_id == device))
		return PCIDevice;
	    PCIDevice = PCIDevice->sibling;
	}
    }
    return NULL;
}



/**************************************************************************
 * PCIShowDevice()                                                        *
 **************************************************************************/
void PCIShowDevice(PCIBus_t *bus, PCIDevice_t *device)
{
  char buffer[20];
  int IoX = 0;
  int MemX = 0;
  int LinePrinted = FALSE;

#ifdef PCIDEBUG
  printf("\tat address %X\n", device);
  printf("\tparent = %X, sibling = %X, next = %X\n", device->parent,
	 device->sibling, device->next);
#endif

  while (TRUE) {
    for (IoX=IoX; IoX < MAX_BASE_ADDRESS_REGISTERS+1; ++IoX) {
      if ((device->PCI_Bar[IoX].Type) && (device->PCI_Bar[IoX].Size != 0)) {
	break;
      }
    }

    for (MemX=MemX; MemX < MAX_BASE_ADDRESS_REGISTERS+1; ++MemX) {
      if ((!device->PCI_Bar[MemX].Type) && (device->PCI_Bar[MemX].Size != 0)) {
	break;
      }
    }

    if (LinePrinted
	&& ((IoX > MAX_BASE_ADDRESS_REGISTERS)
	    && (MemX > MAX_BASE_ADDRESS_REGISTERS))) break;

    if (!LinePrinted) {
      if (device->function == 0) {
	sprintf(buffer, "%3d  ", device->slot);
      }
      else {
	sprintf(buffer, "%2d/%1d ", device->slot, device->function);
      }
      printf("%s %04X   %04X   %02X %04X ",
	     buffer, device->vendor_id, device->device_id, device->revision,
	     incfgw(bus->number, device->slot, device->function, PCI_CFG_REG_STATUS_COMMAND));
    }
    else
      printf("%3s   %4s   %4s   %2s %4s ", "", "", "", "", "");

    if (device->bus == NULL ) {
      /* Non-Bridge Device */
      if (IoX < MAX_BASE_ADDRESS_REGISTERS+1)
	printf("%8X ", device->PCI_Bar[IoX].Base);
      else
	printf("%8s ", "");

      if (MemX < MAX_BASE_ADDRESS_REGISTERS+1)
	printf("%8X%c",  device->PCI_Bar[MemX].Base,
	       (MemX == MAX_BASE_ADDRESS_REGISTERS) ? '<' : ' ');
      else
	printf("%8s ", "");
    }
    else {
      /* Bridge Device */
      if (device->bus->PCI_IO_Base != 0)
	printf("%8X ", device->bus->PCI_IO_Base);
      else
	printf("%8s ", "");

      if (device->bus->PCI_Mem_Base != 0)
	printf("%8X ", device->bus->PCI_Mem_Base);
      else
	printf("%8s ", "");
    }

    if (!LinePrinted) {
      printf("%6X %-27s\n",
	     device->class, DeviceClassString(device->class));
      LinePrinted = TRUE;
    }
    else printf("\n");

#ifdef PRTRACE
    if ((IoX < MAX_BASE_ADDRESS_REGISTERS+1) || (MemX < MAX_BASE_ADDRESS_REGISTERS+1)) {
      printf("%3s   %4s   %4s   %2s %4s ", "", "", "", "", "");

      if (IoX < MAX_BASE_ADDRESS_REGISTERS+1)
	printf("%8X ", device->PCI_Bar[IoX].Size);
      else
	printf("%8s ", "");

      if (MemX < MAX_BASE_ADDRESS_REGISTERS+1)
	printf("%8X ",  device->PCI_Bar[MemX].Size);
      else
	printf("%8s ", "");
      printf("\n");
    }
#endif /* PRTRACE */

    ++IoX;
    ++MemX;
  }

#ifdef PCIDEBUG
  PCIDumpDevice((ui) bus->number, (ui) device->slot,0);
#endif

}

/**************************************************************************
 * PCIShowBus()                                                           *
 **************************************************************************/
void PCIShowBus(PCIBus_t *bus)
{
    PCIDevice_t *device;
    PCIBus_t *child;

    printf("\nBus %d:", bus->number);

#ifdef PCIDEBUG
    printf("\tprimary:%d, secondary:%d, subordinate:%d\n", 
	bus->primary, bus->secondary, bus->subordinate);
    printf("\tI/O base:%X, Mem base:%X\n", bus->PCI_IO_Base, bus->PCI_Mem_Base);
    printf("\tI/O size:%X, Mem size:%X\n", bus->PCI_IO_Size, bus->PCI_Mem_Size);
    printf("Bus = %d (address = %X)\n", bus->number, bus);
    printf("\tbridge = %X\n", bus->bridge);
    printf("\tchildren = %X, next = %X\n", bus->children, bus->next);
#endif

    printf("\n");

/*
 *  Show all the devices on this bus.
 */
    printf("Slot Vendor Device Rev Cmnd I/O Base Mem Base  Class  Description\n");
    printf("==== ====== ====== === ==== ======== ========  ===== ===============\n");
    device = bus->devices;
    while (device != NULL) {
	PCIShowDevice(bus, device);
	device = device->next;
    }
/*
 *  Show all of the sub-buses.
 */
    child = bus->children;
    while (child != NULL) {
	PCIShowBus(child);
	child = child->next;
    }
}

/**************************************************************************
 * PCIShow()                                                              *
 **************************************************************************/
void PCIShow(void)
{

#ifdef PCIDEBUG
/*
 *  First print out the PCI to system address mapping windows.
 */
#ifdef EB64P
    ui pci_base_x;

    printf("\nPCI Address Mapping windows are:\n");
    pci_base_x = (ui)in_ioc_csr(EPIC_PCI_BASE_1);
    if (pci_base_x&EPIC_PCI_BASE_M_WENB) {
	printf("\t(1) PCI Base = 0x%08X, Size = 0x%08X\n",
	       pci_base_x&(~EPIC_PCI_BASE_M_WENB),
	       _PCIMaskToSize(in_ioc_csr(EPIC_PCI_MASK_1)));
	printf("\t\tTranslated Base = 0x%08X\n",
	       in_ioc_csr(EPIC_PCI_TBASE_1)<<1);
    }

    pci_base_x = (ui)in_ioc_csr(EPIC_PCI_BASE_2);
    if (pci_base_x&EPIC_PCI_BASE_M_WENB) {
	printf("\t(2) PCI Base = 0x%08X, Size = 0x%08X\n",
	       pci_base_x&(~EPIC_PCI_BASE_M_WENB),
	       _PCIMaskToSize(in_ioc_csr(EPIC_PCI_MASK_2)));
	printf("\t\tTranslated Base = 0x%08X\n",
	       in_ioc_csr(EPIC_PCI_TBASE_2)<<1);
    }
#else
    printf("\nPCI Address Mapping windows are:\n");
#if (PCI_BASE_1_USED)
    printf("\t(1) PCI Base = 0x%08X, Size = 0x%08X\n",
           PCI_BASE_1, _PCIMaskToSize(PCI_MASK_1));
    printf("\t\tTranslated Base = 0x%08X\n", PCI_TBASE_1);
#endif

#if (PCI_BASE_2_USED)
    printf("\t(2) PCI Base = 0x%08X, Size = 0x%08X\n",
           PCI_BASE_2, _PCIMaskToSize(PCI_MASK_2));
    printf("\t\tTranslated Base = 0x%08X\n", PCI_TBASE_2);
#endif

#ifdef EB164
#if (PCI_BASE_3_USED)
    printf("\t(3) PCI Base = 0x%08X, Size = 0x%08X\n",
           PCI_BASE_3, _PCIMaskToSize(PCI_MASK_3));
    printf("\t\tTranslated Base = 0x%08X\n", PCI_TBASE_3);
#endif

#if (PCI_BASE_4_USED)
    printf("\t(4) PCI Base = 0x%08X, Size = 0x%08X\n",
           PCI_BASE_4, _PCIMaskToSize(PCI_MASK_4));
    printf("\t\tTranslated Base = 0x%08X\n", PCI_TBASE_4);
#endif

#endif /* EB164 */
#endif
#endif /* PCIDEBUG */

    PCIShowBus(root);
    printf("\n");
}

/**************************************************************************
 * PCIValidAddress()                                                      *
 **************************************************************************/
/*
 *  Does the address passed fit into one of the two possible
 *  PCI to system space address mappings?
 */
ui PCIValidAddress(ub *address)
{
#if (PCI_BASE_1_USED)
    if (((uintptr_t) address > PCI_TBASE_1) &&
        ((uintptr_t) address < PCI_TBASE_1 + _PCIMaskToSize(PCI_MASK_1)))
      return TRUE;
#endif

#if (PCI_BASE_2_USED)
    if (((uintptr_t) address > PCI_TBASE_2) &&
        ((uintptr_t) address < PCI_TBASE_2 + _PCIMaskToSize(PCI_MASK_2)))
      return TRUE;
#endif

    return FALSE;
}
/**************************************************************************
 * PCIMapAddress()                                                        *
 **************************************************************************/
/*
 *  Map the address passed fit into one of the two possible
 *  PCI to system space address mappings.
 */
ui PCIMapAddress(ub *address)
{
#if (PCI_BASE_1_USED)
    {
	if (((uintptr_t) address > PCI_TBASE_1) &&
	  ((uintptr_t) address < PCI_TBASE_1 + _PCIMaskToSize(PCI_MASK_1)))
	    return ((uintptr_t) address | PCI_BASE_1);
    }
#endif

#if (PCI_BASE_2_USED) 
    {
	if (((uintptr_t) address > PCI_TBASE_2) &&
	  ((uintptr_t) address < PCI_TBASE_2 + _PCIMaskToSize(PCI_MASK_2)))
	    return ((uintptr_t) address | PCI_BASE_2);
    }
#endif
    return 0;
}

int PCIDevicePresent(ui busnumber, ui devicenumber, ui function)
{
  PCIDevice_t *PCIDevice = devices;

  while (PCIDevice != NULL) {
    if ((PCIDevice->parent->number == busnumber)
	&& (PCIDevice->slot == devicenumber)
	&& (PCIDevice->function == function))
      return TRUE;

    PCIDevice = PCIDevice->sibling;
  }
  return FALSE;
}

static char *DeviceClassString(ui class)
{
  switch (class>>16) {

  case 0x00:			/* Built before code definitions */
    switch (class) {
    case 0x000100:
      return("VGA compatible device");
    default:
      return("Non-VGA compatible device");
    }

  case 0x01:			/* Mass Storage Controller */
    switch (class>>8) {
    case 0x0100:
      return("SCSI bus controller");
    case 0x0101:
      return("IDE controller");
    case 0x0102:
      return("Floppy disk controller");
    case 0x0103:
      return("IPI bus controller");
    default:
      return("Mass storage controller");
    }

  case 0x02:			/* Network Controller */
    switch (class>>8) {
    case 0x0200:
      return("Ethernet controller");
    case 0x0201:
      return("Token Ring controller");
    case 0x0202:
      return("FDDI controller");
    default:
      return("Network controller");
    }

  case 0x03:			/* Display Controller */
    switch (class) {
    case 0x030000:
      return("VGA compatible controller");
    case 0x030001:
      return("8514-compatible controller");
    case 0x030100:
      return("XGA controller");
    default:
      return("Display controller");
    }

  case 0x04:			/* Multimedia Device */
    return("Multimedia Device");

  case 0x05:			/* Memory Controller */
    return("Memory Controller");

  case 0x06:			/* Bridge Device */
    switch (class) {
    case 0x060100:
      return("ISA bridge");
    case 0x060400:
      return("PCI-PCI bridge");
    default:
      return("Bridge Device");
    }

  case 0x07:			/* Simple Communication Controller */
    return("Communication Controller");

  case 0x08:			/* Base System Peripheral */
    return("Base System Peripheral");

  case 0x09:			/* Input Device */
    return("Input Device");

  case 0x0A:			/* Docking Station */
    return("docking Station");

  case 0x0B:			/* Processor */
    return("processor");

  case 0x0C:			/* Serial Bus Controller */
    return("Serial Bus controller");

  default:
    return("Unknown");
  }
}


/**************************************************************************
 * PCIDumpDevice()                                                        *
 **************************************************************************/
void PCIDumpDevice(ui bus, ui dev, ui function)
{
  int i;
  printf("\n\tPCI Configuration Space: bus %d, Slot %d\n", bus, dev);
  for(i=0; i<0x10; i+=0x4) {
    printf("\t%02x:%08x  %02x:%08x  %02x:%08x  %02x:%08x\n",
	   i, incfgl(bus, dev, function, i),
	   i+0x10, incfgl(bus, dev, function, i+0x10),
	   i+0x20, incfgl(bus, dev, function, i+0x20),
	   i+0x30, incfgl(bus, dev, function, i+0x30));
  }
  printf("\n");
}

/**************************************************************************
 * PCIGetNumberOfBusses()                                                 *
 **************************************************************************/
ui PCIGetNumberOfBusses(void)
{
  return ((ui)root->subordinate);
}

ub InPciCfgB(ui bus, ui dev, ui func, ui reg)
{
  ub data = (ub) -1;
  if (PCIDevicePresent(bus, dev, func))
    data = (ub) incfgb (bus, dev, func, reg);
#ifdef DEBUG_GETPCI
  printf(" prb: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
  return data;
}

uw InPciCfgW(ui bus, ui dev, ui func, ui reg)
{
  uw data = (uw) -1;
  if (PCIDevicePresent(bus, dev, func))
    data = (uw) incfgw (bus, dev, func, reg);
#ifdef DEBUG_GETPCI
  printf(" prw: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
  return data;
}

ui InPciCfgL(ui bus, ui dev, ui func, ui reg)
{
  ui data = (ui) -1;
  if (PCIDevicePresent(bus, dev, func))
    data = (ui) incfgl (bus, dev, func, reg);
#ifdef DEBUG_GETPCI
  printf(" prl: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
  return data;
}

void OutPciCfgB(ui bus, ui dev, ui func, ui reg, ub data)
{
#ifdef DEBUG_PUTPCI
  printf(" pwb: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
  if (PCIDevicePresent(bus, dev, func))
      outcfgb (bus, dev, func, reg, data);
}

void OutPciCfgW(ui bus, ui dev, ui func, ui reg, uw data)
{
#ifdef DEBUG_PUTPCI
  printf(" pww: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
  if (PCIDevicePresent(bus, dev, func))
      outcfgw (bus, dev, func, reg, data);
}

void OutPciCfgL(ui bus, ui dev, ui func, ui reg, ui data)
{
#ifdef DEBUG_PUTPCI
  printf(" pwl: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
  if (PCIDevicePresent(bus, dev, func))
      outcfgl (bus, dev, func, reg, data);
}
#else  /* !NEEDPCI */

ui PCIGetNumberOfBusses(void)
{
  return ((ui)0);
}

ub InPciCfgB(ui bus, ui dev, ui func, ui reg)
{
  ub data = (ub) -1;
#ifdef DEBUG_GETPCI
  printf(" prb: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
  return data;
}

uw InPciCfgW(ui bus, ui dev, ui func, ui reg)
{
  uw data = (uw) -1;
#ifdef DEBUG_GETPCI
  printf(" prw: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
  return data;
}

ui InPciCfgL(ui bus, ui dev, ui func, ui reg)
{
  ui data = (ui) -1;
#ifdef DEBUG_GETPCI
  printf(" prl: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
  return data;
}

void OutPciCfgB(ui bus, ui dev, ui func, ui reg, ub data)
{
#ifdef DEBUG_PUTPCI
  printf(" pwb: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
}

void OutPciCfgW(ui bus, ui dev, ui func, ui reg, uw data)
{
#ifdef DEBUG_PUTPCI
  printf(" pww: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
}

void OutPciCfgL(ui bus, ui dev, ui func, ui reg, ui data)
{
#ifdef DEBUG_PUTPCI
  printf(" pwl: %d %d %x %x %x", bus, dev, func, reg, data);
#endif
}

#endif /* NEEDPCI */
