/* file:	pci_size_config.h
 *
 * Copyright (C) 1994 by
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
*++
*  FACILITY:  ALCOR SRM console
*
*  MODULE DESCRIPTION:
*
*      This module contains all common definitions required for PCI bus
*      sizing and configuration
*
*  AUTHORS:
*
*      Jim Hamel
*
*  CREATION DATE:  March 10, 1994 
*
*  DESIGN ISSUES:
*
*      All common definitions specific to the PCI bus sizing and 
*      Configuration should be put here ..
*
*
*  MODIFICATION HISTORY:
*
*	dtr - 1-aug-1996  - Added secondary id field to tables.
(
*	noh - 15-Feb-1995 - added module definition PCI_SIZE_CONFIG_DEF.
*
*	dtr - 7-Jun-1995  - Added third addess space for prefetchable memory
*			    which is required to support tga devices behind
*			    bridges.
*			    
*	DTR - 18-aug-1994 - added the sys_pci* prototypes.
*
*    
*--
*/

#ifndef PCI_SIZE_CONFIG_DEF
#define PCI_SIZE_CONFIG_DEF 1

#include "cp$inc:platform_io.h"		/* PCI_MAX_DEV */
/*
  Definitions used for PCI bus sizing ......
*/

#define	PCI_MEM_BASE 0x100000
#define PCI_IO_BASE  0x40000   
#define PCI_DEVICES 16
#define PCI_BARS  9		/* includes expansion rom */
#define IDX_IDS 6
#define PRIMARY_BUS 0
#define IO_SPACE 1
#define MEM_SPACE 0
#define	PREFETCHABLE_SPACE 2
#define MAX_SPACE 3
#define MAX_CASCADE_LEVELS 5
#define CFGADDR_MASK 0xfffffff0


/*
  PCI-PCI Bridge Definitions
*/
#define BUS_NBR_OFS	0x18
#define IO_BAR_OFS	0x1c
#define MEM_BAR_OFS	0x20
#define PREMEM_BAR_OFS	0x24
#define SUB_VENDOR_ID		0x2c
#define BRG_CNTL_OFS	0x3c

/*
  Minumum alignment for the various bridges base addresses 
*/
#define MEM_BAR_MIN	0x100000
#define IO_BAR_MIN	4096

#define ENABLE_MST_MEM_IO 0x7
#define ENABLE_IO 0x1
#define ENABLE_MEM 0x2
#define ENABLE_MST 0x4

/*
 PCI Class/Sub Class definitions
*/
#define CLASS_before 0
#define SUBCLASS_vga 0
#define SUBCLASS_vga_comp 1

#define CLASS_mass_storage 1
#define SUBCLASS_scsi 0
#define SUBCLASS_ide 1
#define SUBCLASS_floppy 2
#define SUBCLASS_ipi 3
#define SUBCLASS_other 0x80

#define CLASS_network 2
#define SUBCLASS_ethernet 0
#define SUBCLASS_token_ring 1
#define SUBCLASS_fddi 2

#define CLASS_display 3
#define SUBCLASS_vga 0
#define SUBCLASS_xga 1

#define CLASS_multimedia 4
#define SUBCLASS_video 0
#define SUBCLASS_audio 1

#define CLASS_memory 5
#define SUBCLASS_ram 0
#define SUBCLASS_flash 1

#define CLASS_bridge 6
#define SUBCLASS_host 0
#define SUBCLASS_isa 0
#define SUBCLASS_eisa 0
#define SUBCLASS_mc 0
#define SUBCLASS_pci 0
#define SUBCLASS_pcmcia 0

#define CLASS_none 0xff

#define class_pos  24
#define subclass_pos 16

#define PCI_PCI_BRIDGE  0x06040000
#define PCI_EISA_BRIDGE  0x06020000
#define BRIDGE_CLASS 0x06000000


/*
 Structure to contain supported bridge types

 vid_did	- contains the combined vendor/device ID
 cascade_level  - contains how many levels you can cascade the bridge
 setup_bridge_cfg  - points to bridge specific routine to setup for config
 setup_bridge_mio  - points to bridge specific routine to setup for memory and I/O
*/
#ifndef __cplusplus
  typedef int (*PSetupBridgeFcn)();
#else            
  typedef int (*PSetupBridgeFcn)(...);
#endif                                              
struct supported_bridge         
    {                           
       int vid_did;	        
       int sub_id;                                                                                         
       int cascade_level;                                                                                  
       PSetupBridgeFcn setup_bridge_cfg;
       PSetupBridgeFcn setup_bridge_mio;
    }; 

/*
ALCOR PCI Bus sizing element

   next_bus_elem    - will point to any subsequent bus elements in the system
   prev_bus_elem    - will point to any previous  bus elements in the system
   sorted_mem_bar   - will point to a list of sorted memory space address regions
   sorted_io_bar    - will point to a list of sorted io space address regions
   dev_list	    - will point to a list of devices found on the bus
   bus		    - will contain the bus number
   space_req	    - will contain the total memory/io  space needed on the bus 
   alignment	    - will contain the natural alignment beneath the bridge
   flags	    - will contain device specific information. VGA currently                     
*/                                                                                                
                                                             
#ifdef __cplusplus
 struct pci_baddr_elem; 	/* forward declaration */                                                 
 struct pci_dev_elem;    /* forward declaration */                                                 
#endif 
struct pci_bus_elem {                                                                             
	    struct pci_bus_elem *next_bus_elem;                                                   
	    struct pci_bus_elem *prev_bus_elem;                                                   
	    struct pci_baddr_elem *sorted_bar[MAX_SPACE];                                         
	    struct pci_dev_elem *dev_list;                                                        
	    int  bus;                                                                             
	    int space_req[MAX_SPACE];                                                             
	    int alignment[MAX_SPACE];
	    int	flags;
	    };


/*
 ALCOR PCI device element structure

 - This is used to save away device configuration information in the system
  
   where:

   next_dev_ptr	- points to the next device found on this bus 
   prev_dev_ptr	- points to the previous device found on this bus 
   device_id	- is the device id read from PCI config space
   vendor_id	- is the vendor ID read from PCI config space                                               
   sub_id	- subsystem vendor id
   revision_id	- is the vendor ID read from PCI config space 
   bus		- is the bus number the device was found on
   device	- is the device number on the bus
   vector	- is the interrupt vector allocated
   controller   - controller letter used by this device (depth first support )
   baddr_ptr	- will point to a singly linked list of all base addresses for that device
   sec_bus_ptr	- will point to a bus structure for a secondary bus
   bridge_dev_ptr - pointer to current bridge device element
   parent	- pointer to device element for parent bridge if this is a 
		  bridge device.
*/
struct pci_dev_elem {
	    struct pci_dev_elem *next_dev_ptr;
	    struct pci_dev_elem *prev_dev_ptr;
	    int vend_dev_id;
	    int revision_id;
	    int sub_id;	    /* subsystem vendor id */
	    int bus;
	    int slot;
	    int	int_line;
	    int	vector;
	    int controller;
	    struct pci_baddr_elem *baddr_ptr;
	    struct pci_bus_elem *sec_bus_ptr;
	    struct pci_dev_elem *bridge_dev_ptr;
	    struct pci_dev_elem *parent;
	    };

/*
 ALCOR PCI Base address structure 

 This is used by the PCI bus sizing routine .....

 where:

 next_bar_ptr	- points to the next element in the sorted list of base addresses
 prev_bar_ptr	- points to the previous element in the sorted list of base addresses
 dev_bar_ptr	- points to the next base address structure for this device
 dev_back_ptr   - points back to the device structure for this base address
 bar_nbr	- Base Address Number in config space (0..5)
 space_req	- is the memory required in bytes
 pci_addr	- is the PCI address that the base address is mapped to
 space		- contains 0 for memory, 1 for io and 2 for prefetchable
 alignment	- natural alignment of device - require by bridge support.
  
*/
           
struct pci_baddr_elem {
	struct pci_baddr_elem *next_bar_ptr;	    
	struct pci_baddr_elem *prev_bar_ptr;	    
	struct pci_baddr_elem *dev_bar_ptr;
	struct pci_dev_elem *dev_back_ptr;
	int bar_nbr;
	int space_req;
	int pci_addr;				    
	int space;
	int alignment;
	};
/*
PCI Device Table 

    hose    - hose number where device found 
    bus	    - bus number off of hose where device is found
    slot    - slot on bus where device is found
    int_line - interrupt lines from device
    vector  - interrupt vector allocated
    controller - controller designation for device

*/
struct pci_device_tbl   {
	int hose;
	int bus;
	int slot;
	int int_line;
	int vector;
	int controller;
	};


/*
 PCI Device Found table
 where:

    nbr_found	- number of devices found
    dev_found	- array of structures that contain info about where
                  the device was found.
*/

struct pci_devs_found {
    int	    nbr_found;
    struct  pci_device_tbl  dev_found[PCI_MAX_DEV];
    };

/*
 functions that can be used from the outside
*/

void pci_size_config(void);
struct pci_devs_found *pci_find(int vend_dev_id, int sub_id);
void sys_pci_enable_interrupt  (int vector);
void sys_pci_disable_interrupt (int vector);
int sys_pci_allocate_vector   ( struct pci_dev_elem *dev_ptr);
int sys_pci_deallocate_vector ( int bus ,int dev_slot, int dev_int_pin, int vector);
void sys_probe_setup (void);
int sys_probe_check (void);
void sys_pci_config_wr (int hose,
		   int bus,
		   int slot,
		   int addr,
		   int data);
int sys_pci_config_rd (int hose,
		   int bus,
		   int slot,
		   int addr,
		   int probe);
int sys_environment_init (void);
void sys_irq_init (void);

void pci_config_bus (int hose, struct pci_bus_elem *pci_bus_ptr, int space);

#endif /* PCI_SIZE_CONFIG_DEF */
