/* file:	lx164_io.h
 *
 * Copyright (C) 1996, 1997 by
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
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha SRM Console Firmware
 *
 *  MODULE DESCRIPTION:     
 *
 *	Platform specific I/O definitions for Pyxis-based motherboards
 *
 *  AUTHORS:
 *
 *	Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *      20-Nov-1996
 *
 *  MODIFICATION HISTORY:
 *  
 *	ER	18-Feb-1997	Added PROBE_IO interfaces
 *	ER	03-Dec-1996	Removed duplicate PCI configuration space header 
 *				definitions included by pci_def.h.
 *				
 *--
 */                                                
#ifndef __PLATFORM_IO_H
#define __PLATFORM_IO_H

#include    "cp$src:pci_def.h"

#define MIN_HOSE    0
#define MAX_HOSE    1

#define PROBE_PCI   1
#define PROBE_ISA   1

#define PCI_MIN_SLOT		5
#define PCI_MAX_SLOT		11

#define PCI_BASE_ADDR_SHIFT	28
#define PCI_BYTE_OFFSET_SHIFT	5
#define PCI_XFER_LENGTH_SHIFT	3

/* 
** The following constants are referenced by the driver used for accessing
** PCI configuration space, pcicfg_driver.c
*/
#define MIN_ALIGN_MEM		0x1000	/* 4k bytes is the minimum MEM page alignment size */
#define MIN_ALIGN_IO		0x100	/* 256 bytes is the minimum IO page alignment size */
#define MIN_ALIGN_IO_MASK	0xff	/* mask for doing page aligns */
#define ONE_MEG			0x100000

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
#define PCI_MEM_0		0x800	/* PCI Sparse Memory Region 0, 512 MB */
#define PCI_MEM_1		0x840	/* PCI Sparse Memory Region 1, 128 MB */
#define PCI_MEM_2		0x850	/* PCI Sparse Memory Region 2,  64 MB */
#define PCI_IO_A		0x858	/* PCI Sparse I/O Region A, 32 MB */
#define PCI_IO_B		0x85C	/* PCI Sparse I/O Region B, 32 MB */
#define PCI_DMEM		0x860	/* PCI Dense Memory Space */
#define PCI_CFG			0x870	/* PCI Sparse Configuration Space */
#define PCI_INTACK		0x872	/* PCI Special Cycle/Interrupt Acknowledge */
#define PCI_MEM_BW		0x880	/* PCI Linear (byte/word) Memory Space */
#define PCI_IO_BW		0x890	/* PCI Linear (byte/word) I/O Space */
#define PCI_CFG0_BW		0x8A0	/* PCI Linear (byte/word) Configuration Space, type 0 */
#define PCI_CFG1_BW		0x8B0	/* PCI Linear (byte/word) Configuration Space, type 1 */

/*
** PCI Host Expansion Bus [E]ISA Definitions
*/
#define EISA_BUSES		1	/* Number of [E]ISA expansion buses */
#define EISA_HOSE		0
#define ISA_MAX_SLOT		3	/* Number of ISA expansion slots */

#define ARC_NVRAM_OFFSET	0x0
#define ARC_NVRAM_LENGTH	0x8000
#define ISACFG_NVRAM_OFFSET	0x8000
#define ISACFG_NVRAM_LENGTH	0xC00

/*
** Specific addressing for Host Bridge CSR read and write operations 
*/
#define Local_CSR		( BaseCSR << 28 )
#define CSR_L( x )		( Local_CSR | ( ( x ) & 0x3FFFFFFF ) )

#define READ_IO_CSR( c )	read_pyxis_csr( CSR_L( c ) )
#define WRITE_IO_CSR( c, v )	write_pyxis_csr( CSR_L( c ), v )
#define READ_MEM_CSR( b, c )	read_pyxis_csr( CSR_L( c ) | ( b * 0x40 ) )

/*
** Hardware specific Host Bridge Register Definitions
**
** Note that these constants are base references that need to be shifted 
** into their proper bit positions to form addresses.
**
*/
#define GENERAL_BASE		0x874
#define MEMORY_BASE		0x875
#define ADDR_TRANS_BASE		0x876
#define MISC_BASE		0x878
#define POWER_MGMT_BASE		0x879
#define INTR_CTRLR_BASE		0x87A
#define FLASH_ROM_BASE		0x87C
        
/*      
** General Register Offset Definitions (Base = 87.4000.0000)
*/
#define CSR_PYXIS_REV		0x0080
#define CSR_PCI_LAT		0x00C0
#define CSR_PYXIS_CTRL		0x0100
#define CSR_PYXIS_CTRL1		0x0140
#define CSR_FLASH_CTRL		0x0200
#define CSR_HAE_MEM		0x0400
#define CSR_HAE_IO		0x0440
#define CSR_CFG			0x0480

/*
** Diagnostic Register Offset Definitions (Base = 87.4000.0000)
*/
#define CSR_PYXIS_DIAG		0x2000
#define CSR_DIAG_CHECK		0x3000

/*
** Performace Monitor Register Offset Definitions (Base = 87.4000.0000) 
*/
#define CSR_PERF_MONITOR	0x4000
#define CSR_PERF_CONTROL	0x4040

/*
** Error Register Offset Definitions (Base = 87.4000.0000)
*/
#define CSR_PYXIS_ERR		0x8200
#define CSR_PYXIS_STAT		0x8240
#define CSR_PYXIS_ERR_MASK	0x8280
#define CSR_PYXIS_SYN		0x8300
#define CSR_MEAR		0x8400
#define CSR_MESR		0x8440
#define CSR_PCI_ERR0		0x8800
#define CSR_PCI_ERR1		0x8840
#define CSR_PCI_ERR2		0x8880

/*    
** Memory Control Register Offset Definitions (Base = 87.5000.0000)
*/
#define CSR_MCR			0x0000
#define CSR_MCMR		0x0040
#define CSR_GTR			0x0200
#define CSR_RTR			0x0300
#define CSR_RHPR		0x0400
#define CSR_MDR			0x0500
#define CSR_BBAR		0x0600
#define CSR_BCR			0x0800
#define CSR_BTR			0x0A00
#define CSR_CVM			0x0C00

/*
** PCI Address Related Register Offset Definitions (Base = 87.6000.0000)
*/
#define CSR_TBIA		0x0100
#define CSR_W0_BASE		0x0400
#define CSR_W0_MASK		0x0440
#define CSR_T0_BASE		0x0480
#define CSR_W1_BASE		0x0500
#define CSR_W1_MASK		0x0540
#define CSR_T1_BASE		0x0580
#define CSR_W2_BASE		0x0600
#define CSR_W2_MASK		0x0640
#define CSR_T2_BASE		0x0680
#define CSR_W3_BASE		0x0700
#define CSR_W3_MASK		0x0740
#define CSR_T3_BASE		0x0780
#define CSR_DAC			0x07C0

/*
** Scatter Gather Address Translation Register Offset Definitions (Base = 87.6000.0000)
*/
#define CSR_LTB_TAG		0x0800
#define CSR_TB_TAG		0x0900
#define CSR_TB0_PAGE		0x1000
#define CSR_TB1_PAGE		0x1100
#define CSR_TB2_PAGE		0x1200
#define CSR_TB3_PAGE		0x1300
#define CSR_TB4_PAGE		0x1400
#define CSR_TB5_PAGE		0x1500
#define CSR_TB6_PAGE		0x1600
#define CSR_TB7_PAGE		0x1700

/*
** Miscellaneous Register Offset Definitions (Base = 87.8000.0000)
*/
#define CSR_CCR			0x0000
#define CSR_CLK_STAT		0x0200
#define CSR_RESET		0x0900

/*
** Power Management Register Offset Definitions (Base = 87.9000.0000)
*/
#define CSR_FAR			0x0000
#define CSR_FCR			0x0040
#define CSR_FTR			0x0080
#define CSR_PCR			0x00C0
#define CSR_PTR			0x0100
#define CSR_PSR			0x0140

/*
** Interrupt Controller Register Offset Definitions (Base = 87.A000.0000)
*/
#define CSR_INT_REQ		0x0000
#define CSR_INT_MASK		0x0040
#define CSR_INT_HILO		0x00C0
#define CSR_INT_ROUTE		0x0140
#define CSR_GPO			0x0180
#define CSR_INT_CNFG		0x01C0
#define CSR_RT_COUNT		0x0200
#define CSR_INT_TIME		0x0240
#define CSR_IIC_CTRL		0x02C0
            
/*
** Flash ROM definitions required for from_driver.c
*/

#define SEGSIZE		(64 * 1024)
#define NUMSEGS		16

#define FLASH_BASE	0xFFF80000

#define FLASH_ROM_SIZE	0x100000


#define FSROM_BASE	0
#define FSROM_NUMSEGS	1
#define FSROM_SIZE	(FSROM_NUMSEGS * SEGSIZE)

#define FWROM_BASE	(FSROM_BASE + FSROM_SIZE)
#define FWROM_NUMSEGS	14
#define FWROM_SIZE	(FWROM_NUMSEGS * SEGSIZE)


/*          
** I/O Function Prototype Declarations
*/          
unsigned char inmemb( struct pb *pb, unsigned __int64 offset );
unsigned short inmemw( struct pb *pb, unsigned __int64 offset );
unsigned int inmeml( struct pb *pb, unsigned __int64 offset );
unsigned __int64 inmemq( struct pb *pb, unsigned __int64 offset );
            
void outmemb( struct pb *pb, unsigned __int64 offset, unsigned char data );
void outmemw( struct pb *pb, unsigned __int64 offset, unsigned short data );
void outmeml( struct pb *pb, unsigned __int64 offset, unsigned int data );
void outmemq( struct pb *pb, unsigned __int64 offset, unsigned __int64 data );
            
unsigned char inportb( struct pb *pb, unsigned __int64 offset );
unsigned short inportw( struct pb *pb, unsigned __int64 offset );
unsigned int inportl( struct pb *pb, unsigned __int64 offset );
            
void outportb( struct pb *pb, unsigned __int64 offset, unsigned char data );
void outportw( struct pb *pb, unsigned __int64 offset, unsigned short data );
void outportl( struct pb *pb, unsigned __int64 offset, unsigned int data );

unsigned int indmeml( struct pb *pb, unsigned __int64 offset );
unsigned __int64 indmemq( struct pb *pb, unsigned __int64 offset );

void outdmeml( struct pb *pb, unsigned __int64 offset, unsigned int data );
void outdmemq( struct pb *pb, unsigned __int64 offset, unsigned __int64 data );

unsigned char incfgb( struct pb *pb, unsigned __int64 offset );
unsigned short incfgw( struct pb *pb, unsigned __int64 offset );
unsigned int incfgl( struct pb *pb, unsigned __int64 offset );

void outcfgb( struct pb *pb, unsigned __int64 offset, unsigned char data );
void outcfgw( struct pb *pb, unsigned __int64 offset, unsigned short data );
void outcfgl( struct pb *pb, unsigned __int64 offset, unsigned int data );

unsigned __int64 read_pyxis_csr( unsigned __int64 address );
void write_pyxis_csr( unsigned __int64 address, unsigned __int64 data );

unsigned int set_hae( unsigned int value );

unsigned int io_get_window_base( struct pb *pb );

unsigned int io_get_asic_revision( void );

unsigned int get_hose_type( int hose );

void probe_pci_setup( int hose );

void probe_pci_cleanup( int hose );

unsigned int read_pci_id( int hose, int bus, int slot, int function );

void get_pci_csr_vector( struct pb *pb );

void eisa_size_config( int hose, int bus );

int use_eisa_ecu_data( struct pb *pb );

#endif	/* __PLATFORM_IO_H */
