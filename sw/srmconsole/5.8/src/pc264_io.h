/* file:	pc264_io.h
 *                   
 * Copyright (C) 1996 by
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
 *
 *
 * Abstract:	Pci definitions.
 *
 * Author: Eric Goehl
 *
 * Modifications:
 *
 *	er	24-Sep-1997	Added io_get_max_hose() macro. Cleaned up
 *			        for use with tsunami_io.c
 *	er	25-Jul-1997	Changed hose probe order via HOSE macro.
 *	er	14-Jul-1997	Cleaned up, fixed PCI_MIN_SLOT and
 *				PCI_MAX_SLOT constants.
 *	egg	13-Nov-1996	Initial port of eb164_io.h
 *
 */
#ifndef pc264_io_h		/* Prevent multiple includes. */
#define pc264_io_h

#include    "cp$src:platform.h"
#include    "cp$src:pci_def.h"
#include    "cp$src:pci_vendor_ids.h"
                             
#if NAUTILUS || TINOSA || GLXYTRAIN

enum {ISP_MODEL = 1, REAL_HW};

#define PCI_K_MAX_SLOTS		16
#define MAX_HOSE	       	0
#define MIN_HOSE			0
#define HOSE(x)				(x)
#define PROBE_LOCAL			1
#define PROBE_PCI 			1
#define PROBE_ISA 			1
#define KGPSA_POLLED		1

#define io_get_max_hose( )	MAX_HOSE

/*
// PCI Device IDs
*/
#define PCI_SLOT0_DEVICE_ID	0x08
#define PCI_SLOT1_DEVICE_ID	0x09
#define PCI_SLOT2_DEVICE_ID	0x0a
#define PCI_SLOT3_DEVICE_ID	0x0b

#define PCI_MIN_SLOT		0
#define PCI_MAX_SLOT		20

/*                     
** PCI Host Expansion Bus [E]ISA Definitions
*/             
#define EISA_BUSES		1	/* Number of [E]ISA expansion buses */
#define EISA_HOSE		0
#define ISA_MAX_SLOT		2	/* Number of [E]ISA expansion slots */

#define ARC_NVRAM_OFFSET	0x0
#define ARC_NVRAM_LENGTH	0x8000
#define ISACFG_NVRAM_OFFSET	0x8000
#define ISACFG_NVRAM_LENGTH	0xC00

/*
** Specific addressing for Host Bridge CSR read and write operations 
*/
#define cchip_offset( x, y )	( ( ( x & 1 ) ? 0x40 : 0 ) + ( ( x & 2 ) ? 0x400 : 0 ) + y )

#define io_get_window_base( pb )    io_get_window_base_rt( pb )

unsigned char in_flash( struct pb *pb, unsigned int offset );

void out_flash( struct pb *pb, unsigned int offset, unsigned char data );

unsigned int io_get_window_base( struct pb *pb );

#else
  /* Enables the pci retry reset process. Tsunami p1 bug. */
#define CAUSE_ABORT 0        
  /* Enables clearing the status of the target abort used to */
  /* reset the pci retry reset process. Tsunami p1 bug.  */
#define CLEAR_ABORT 0

enum {ISP_MODEL = 1, REAL_HW};

#define PCI_K_MAX_SLOTS		16
#define MAX_HOSE	       	1
#define MIN_HOSE		0
#define HOSE(x)			( x ^ 1 )
#define PROBE_LOCAL		1
#define PROBE_PCI 		1
#define PROBE_ISA 		1
#define KGPSA_POLLED		1

#define io_get_max_hose( )	MAX_HOSE

#if SHARK
#define PCI_MIN_SLOT		3
#define PCI_MAX_SLOT		20

/*
// PCI Device IDs
*/
#define PCI_SLOT0_DEVICE_ID	0x0a
#define PCI_SLOT3_DEVICE_ID	0x0a			/* Not a real slot so same as SLOT0 */

#else
static enum  TPCHIP0_SLOT_IDS{             
        SIO_SLOT=5,             /* PCI0_AD<16> */
	SCSI_SLOT, 		/* PCI0_AD<17> */                 
	PCIO_SLOT0,      	/* PCI0_AD<18> */
	PCIO_SLOT1,             /* PCI0_AD<19> */	        
	PCIO_SLOT2              /* PCI0_AD<20> */
} PChip0Ids;                                      
static enum  TPCHIP1_SLOT_IDS{             
	PCI1_SLOT0=7,           /* PCI1_AD<18> */        
	PCI1_SLOT1,             /* PCI1_AD<19> */        
	PCI1_SLOT2,             /* PCI1_AD<20> */        
	PCI1_SLOT3              /* PCI1_AD<21> */        
} PChip1Ids;                                      

#define PCI_MIN_SLOT		5
#define PCI_MAX_SLOT		10

/*
// PCI Bus 0 Device IDs
*/
#define PCI_SLOT0_DEVICE_ID	0x07
#define PCI_SLOT1_DEVICE_ID	0x08
#define PCI_SLOT2_DEVICE_ID	0x09
#define PCI_SLOT3_DEVICE_ID	0x0a

#endif
/*
** 
** Host Bridge System Addressing Definitions
**
** The following constants define base address values that describe the
** mapping of the 40 bit processor physical address space into memory
** and I/O space.
**
** Note that these constants are base references that need to be shifted 
** into their proper bit positions to form addresses.
**                                           
*/
#define PCI0_IO_BASE		0x801FC /* PCI 0 I/O Space */
#define PCI1_IO_BASE		0x803FC /* PCI 1 I/O Space */
#define PCI0_MEM_BASE		0x80000 /* PCI 0 Memory Space */
#define PCI1_MEM_BASE		0x80200	/* PCI 1 Memory Space */
#define PCI0_CONFIG_BASE	0x801FE /* PCI 0 Configuration Space */
#define PCI1_CONFIG_BASE	0x803FE	/* PCI 1 Configuration Space */
#define PCI0_INTACK		0x801F8	/* PCI 0 Special Cycle/Interrupt Acknowledge */
#define PCI1_INTACK		0x803F8	/* PCI 1 Special Cycle/Interrupt Acknowledge */
#define TIG_BASE   		0x80100 /* Tsunami TIG bus base address */

/*                     
** PCI Host Expansion Bus [E]ISA Definitions
*/             
#define EISA_BUSES		1	/* Number of [E]ISA expansion buses */
#define EISA_HOSE		0
#define ISA_MAX_SLOT		2	/* Number of [E]ISA expansion slots */

#define ARC_NVRAM_OFFSET	0x0
#define ARC_NVRAM_LENGTH	0x8000
#define ISACFG_NVRAM_OFFSET	0x8000
#define ISACFG_NVRAM_LENGTH	0xC00

/*
** Specific addressing for Host Bridge CSR read and write operations 
*/
#define cchip_offset( x, y )	( ( ( x & 1 ) ? 0x40 : 0 ) + ( ( x & 2 ) ? 0x400 : 0 ) + y )

#define io_get_window_base( pb )    io_get_window_base_rt( pb )

unsigned char in_flash( struct pb *pb, unsigned int offset );

void out_flash( struct pb *pb, unsigned int offset, unsigned char data );

unsigned int io_get_window_base( struct pb *pb );

#endif
#endif	/* Include file sentry */
