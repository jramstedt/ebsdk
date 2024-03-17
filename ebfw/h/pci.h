#ifndef __PCI_H_LOADED
#define __PCI_H_LOADED
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

/*
 *  $Id: pci.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 *
 *  This file contains all of the PCI specific definitions required for
 *  the PCI bus support within the debug monitor. 
 */

/*
 * $Log: pci.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.23  1997/05/16  02:08:17  fdh
 * Modified to map multi-function devices.
 *
 * Revision 1.22  1996/08/20  02:32:06  fdh
 * Made PCIGetNumberOfBusses() visible to non-PCI platform builds.
 *
 * Revision 1.21  1996/08/19  04:11:20  fdh
 * Removed unnecessary elements from the PCIDevice structure.
 * Added the bus pointer to PCIBus structure for bridge devices.
 *
 * Revision 1.20  1996/08/16  17:22:10  fdh
 * Added definitions required to map extended PCI ROM base addresses.
 * Added PCIGetNumberOfBusses() function.
 *
 * Revision 1.19  1996/08/07  18:35:00  fdh
 * Added PCIDumpDevice().
 *
 * Revision 1.18  1996/06/03  14:49:23  fdh
 * Removed Ethernet device and vendor id definitions.
 *
 * Revision 1.17  1996/05/26  20:06:16  fdh
 * Modified a couple of variable names for clarity.
 * Updated some macros in accordance with previous changes.
 *
 * Revision 1.16  1996/05/24  20:08:26  fdh
 * Modified the PCI device data structure PCIDevice to support
 * mapping all base addresses rather than only one I/O and one
 * Memory.
 *
 * Revision 1.15  1996/05/22  21:23:05  fdh
 * Defined programmable limits for the PCI bus probing.
 *
 * Revision 1.14  1995/10/31  22:48:06  cruz
 * Moved some prototypes outside the NEEDPCI area.
 *
 * Revision 1.13  1995/09/02  03:51:53  fdh
 * Added PCIDevicePresent declaration.
 *
 * Revision 1.12  1995/08/25  19:37:47  fdh
 * Added generic PCI config space access routines.
 * These routines call system specific routines.
 *
 * Revision 1.11  1994/11/01  11:30:18  rusling
 * Added PCI-PCI bridge support.
 *
 * Revision 1.10  1994/08/05  20:13:47  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * $Log: pci.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.23  1997/05/16  02:08:17  fdh
 * Modified to map multi-function devices.
 *
 * Revision 1.22  1996/08/20  02:32:06  fdh
 * Made PCIGetNumberOfBusses() visible to non-PCI platform builds.
 *
 * Revision 1.21  1996/08/19  04:11:20  fdh
 * Removed unnecessary elements from the PCIDevice structure.
 * Added the bus pointer to PCIBus structure for bridge devices.
 *
 * Revision 1.20  1996/08/16  17:22:10  fdh
 * Added definitions required to map extended PCI ROM base addresses.
 * Added PCIGetNumberOfBusses() function.
 *
 * Revision 1.19  1996/08/07  18:35:00  fdh
 * Added PCIDumpDevice().
 *
 * Revision 1.18  1996/06/03  14:49:23  fdh
 * Removed Ethernet device and vendor id definitions.
 *
 * Revision 1.17  1996/05/26  20:06:16  fdh
 * Modified a couple of variable names for clarity.
 * Updated some macros in accordance with previous changes.
 *
 * Revision 1.16  1996/05/24  20:08:26  fdh
 * Modified the PCI device data structure PCIDevice to support
 * mapping all base addresses rather than only one I/O and one
 * Memory.
 *
 * Revision 1.15  1996/05/22  21:23:05  fdh
 * Defined programmable limits for the PCI bus probing.
 *
 * Revision 1.14  1995/10/31  22:48:06  cruz
 * Moved some prototypes outside the NEEDPCI area.
 *
 * Revision 1.13  1995/09/02  03:51:53  fdh
 * Added PCIDevicePresent declaration.
 *
 * Revision 1.12  1995/08/25  19:37:47  fdh
 * Added generic PCI config space access routines.
 * These routines call system specific routines.
 *
 * Revision 1.11  1994/11/01  11:30:18  rusling
 * Added PCI-PCI bridge support.
 *
 * Revision 1.9  1994/06/20  14:18:59  fdh
 * Fixup header file preprocessor #include conditionals.
 *
 * Revision 1.8  1994/06/19  15:48:04  fdh
 * Changed PCI_H to __PCI_H_LOADED
 *
 * Revision 1.7  1994/06/17  19:34:01  fdh
 * Clean-up...
 *
 * Revision 1.6  1994/01/19  10:22:28  rusling
 * Ported to ALpha Windows NT.
 *
 * Revision 1.5  1993/11/29  14:58:42  rusling
 * Added PCI address checking and conversion routines,
 * PCIMapAddress() and PCIValidAddress().
 *
 * Revision 1.4  1993/11/24  15:23:29  rusling
 * Added PCI_CFG_REG_VENDOR_DEVICE for offset to
 * first longword of the PCI configuration header.
 *
 * Revision 1.3  1993/11/23  10:43:50  rusling
 * Remove per system information.
 *
 * Revision 1.2  1993/11/22  13:17:13  rusling
 * Merged with PCI/21040 changes.
 *
 * Revision 1.1  1993/11/22  12:16:56  rusling
 * Initial revision
 *
 *
 */

#ifdef NEEDPCI

#define MAX_BASE_ADDRESS_REGISTERS	6

/*
 *  Define the PCI device data structure.
 */
typedef struct PCIDevice {
    struct PCIBus *parent;	/* parent bus */
    struct PCIBus *bus;		/* bus if bridge device */
    struct PCIDevice *next;	/* in device chain */
    struct PCIDevice *sibling;	/* for this bus */

    struct {
      ui Reg;
      ui Base;
      ui Size;
      int Type;			/* True = I/O, False = Mem */
    } PCI_Bar [MAX_BASE_ADDRESS_REGISTERS+1]; /* plus 1 includes the ROM Base */

    ui slot;
    ui function;
    ui class;
    uw vendor_id;
    uw device_id;
    uw command;
    ub revision;

} PCIDevice_t;

typedef struct PCIBus {
    struct PCIBus *parent;
    struct PCIBus *next;
    struct PCIDevice *bridge;
    struct PCIDevice *devices;
    struct PCIBus *children;

    ui PCI_IO_Reg;
    ui PCI_IO_Base;
    ui PCI_IO_Size;
    ui PCI_Mem_Reg;
    ui PCI_Mem_Base;
    ui PCI_Mem_Size;

    unsigned char number;
    unsigned char primary;
    unsigned char secondary;
    unsigned char subordinate;
} PCIBus_t;


/*
 *  Define some macros for getting at fields in the
 *  PCIDevice_t and PCIBus_t typedefs.
 */
#define _PCI_Bar_Reg(device,idx)	(device)->PCI_Bar[idx].Reg
#define _PCI_Bar_Base(device,idx)	(device)->PCI_Bar[idx].Base
#define _PCI_Bar_Size(device,idx)	(device)->PCI_Bar[idx].Size
#define _PCI_Slot(device)		(device)->slot
#define _PCI_Class(device)		(device)->class
#define _PCI_VendorID(device)		(device)->vendor_id
#define _PCI_DeviceID(device)		(device)->device_id
#define _PCI_Command(device)		(device)->command
#define _PCI_Revision(device)		(device)->revision
#define _PCI_Print(device)		(device)->print
#define _PCI_Bus(device)		(device)->parent->number

/*
 *  Declare some limits.
 */

/*
 *  Defines the limits for the id_sel lines to probe.
 */
#ifndef PCI_T0_FIRST_IDSEL
#define PCI_T0_FIRST_IDSEL	11 /* bits 31:11 of type 0 */
#endif
#ifndef PCI_T0_LAST_IDSEL
#define PCI_T0_LAST_IDSEL	31 /* bits 31:11 of type 0 */
#endif

/*
 *  Defines the limits for the devices to probe across a PCI bridge.
 */
#ifndef PCI_T1_FIRST_DEVICE
#define PCI_T1_FIRST_DEVICE	 0 /* bits 14:11 of type 1 */
#endif
#ifndef PCI_T1_LAST_DEVICE
#define PCI_T1_LAST_DEVICE	15 /* bits 14:11 of type 1 */
#endif

#define PCI_MAX_BRIDGES	   	4

/*
 *  The PCI configuration registers.
 */
#define PCI_CFG_REG_VENDOR_DEVICE         0x0
#define PCI_CFG_REG_STATUS_COMMAND        0x4
					/* LONG */
#define PCI_CFG_REG_REVISION_ID           0x8
					/* BYTE */
#define PCI_CFG_REG_CLASS_CODE		  0x9
#define PCI_CFG_REG_LATENCY_TIMER         0xD
					/* BYTE */
#define PCI_CFG_REG_BAR0                  0x10
					/* Long */
#define PCI_CFG_REG_BRIDGE_PRIMARY	  0x18
#define PCI_CFG_REG_BRIDGE_SECONDARY	  0x19
#define PCI_CFG_REG_BRIDGE_SUBORDINATE	  0x1a

#define PCI_CFG_REG_EROM_BASE             0x30
					/* Long */

#define PCI_BRIDGE_CLASS		       0x060400
/*
 *  Declare all of the externally visible routines in pci.c
 */
extern void PCIShow(void);
extern ui PCIValidAddress(ub * address);
extern ui PCIMapAddress(ub * address);
extern void PCIInit(void);
extern PCIDevice_t *PCIDeviceFind(uw vendor, uw device);
extern PCIDevice_t *PCIDeviceFindNext(PCIDevice_t *device);
extern void PCISetDeviceConfig(PCIDevice_t *device);
extern int PCIDevicePresent(ui busnumber, ui devicenumber, ui function);
extern void PCIDumpDevice(ui bus, ui dev, ui function);
#endif /* NEEDPCI */

/*
 *  These are placed outside the #ifdef NEEDPCI because we need to
 *  provide prototypes for the stub (dummy) routines in pci.c which are
 *  needed for bios emulation even if no pci is present on the board
*/
extern ub InPciCfgB(ui bus, ui dev, ui func, ui reg);
extern uw InPciCfgW(ui bus, ui dev, ui func, ui reg);
extern ui InPciCfgL(ui bus, ui dev, ui func, ui reg);
extern void OutPciCfgB(ui bus, ui dev, ui func, ui reg, ub data);
extern void OutPciCfgW(ui bus, ui dev, ui func, ui reg, uw data);
extern void OutPciCfgL(ui bus, ui dev, ui func, ui reg, ui data);
extern ui PCIGetNumberOfBusses(void);

#endif /* __PCI_H_LOADED */
