/* file:	lx164_io.c
 *
 * Copyright (C) 1996, 1997, 1998 by
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
 *  FACILITY:
 *
 *      Alpha SRM Console Firmware
 *
 *  MODULE DESCRIPTION:     
 *
 *	Platform specific I/O routines for Pyxis-based motherboards
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
 *	ER	16-Feb-1998	Fix I/O abort timeout problem with probes to
 *				SWXCR RAID controller.
 *	ER	18-Feb-1997	Added PROBE_IO intefaces.
 *	ER	05-Dec-1996	Added support for linear (byte/word architecture 
 *				extensions) addressing.
 *  
 *--
 */                                                

/* $INCLUDE_OPTIONS$ */
/* $INCLUDE_OPTIONS_END$ */
#include        "cp$src:stddef.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:pb_def.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:probe_io_def.h"
#include	"cp$src:pyxis_main_csr_def.h"
#include	"cp$src:pyxis_addr_trans_def.h"
/* Platform specific include files */
#include	"cp$inc:platform_io.h"
#include        "cp$src:platform.h"
#include 	"cp$src:platform_cpu.h"
#include	"cp$src:i82378_def.h"

/*
** External Data Declarations
*/
extern unsigned char ETAPciMaxBusNumber;
extern int manual_config;
extern int pci_bus;
extern unsigned char pci_irq_table[ ];
extern unsigned char vector_allocated[ ];
extern int ExpectMachineCheck;
extern int ioa_timeout_flag;

/*
** External Function Prototype Declarations
*/

/* From kernel_alpha.mar */
#if BWX
extern void stb( unsigned __int64 address, unsigned char data );
extern void stw( unsigned __int64 address, unsigned short data );
extern unsigned char ldb( unsigned __int64 address );
extern unsigned short ldw( unsigned __int64 address );
#endif
extern void stl( unsigned __int64 address, unsigned int data );
extern void stq( unsigned __int64 address, unsigned __int64 data );
extern unsigned int ldl( unsigned __int64 address );
extern unsigned __int64 ldq( unsigned __int64 address );

/* From probe_io.c */
extern void pci_add_region( unsigned int base, unsigned int size, int type );

/* From printf.c */
extern int err_printf( char *format, ... );

/* From lx164_init.c */
extern int SMCinit( void );

static int sio_state_saved = 0;
char sio_state_buffer[0x80];

void setup_io( void )
{
    unsigned __int64 BaseCSR = pyxis_k_main_csr_base;
    unsigned int pyxis_ctrl_bits;
    unsigned int pyxis_err_mask_bits;
    unsigned int i;
    struct pb SIO_pb;
    unsigned char temp;

/*
** Initialize Pyxis Control Register 1 (PYXIS_CTRL1)
*/
    pyxis_ctrl_bits = (
	  pyxis_ctrl1_m_ioa_ben		    /* Enable byte/word support */
    );
    WRITE_IO_CSR( CSR_PYXIS_CTRL1, pyxis_ctrl_bits );
    mb( );
/*
** Initialize the Pyxis Control Register (PYXIS_CTRL)
*/
    pyxis_ctrl_bits = (
	  pyxis_ctrl_m_pci_en			/* Pyxis does NOT assert reset to the PCI */
	| pyxis_ctrl_m_pci_mst_en		/* Pyxis will initiate PCI transactions */
	| pyxis_ctrl_m_pci_mem_en		/* Pyxis will respond to PCI transactions */
	| pyxis_ctrl_m_pci_req64_en		/* Pyxis will request 64 bit PCI data transactions */
	| pyxis_ctrl_m_pci_ack64_en		/* Pyxis will accept 64 bit PCI data transactions */
	| pyxis_ctrl_m_fillerr_en		/* Pyxis will assert FILL_ERROR on read misses */
	| pyxis_ctrl_m_mchkerr_en		/* Pyxis will assert ERROR to report system MCHK conditions */
	| pyxis_ctrl_m_ecc_chk_en		/* Pyxis will check IOD data */
	| (pyxis_ctrl_m_rd_type & 0x00000000)	/* PCI Read prefetch algorithm = No prefetch */
	| (pyxis_ctrl_m_rl_type & 0x00000000)	/* PCI Read Line prefetch algorithm = No prefetch */
	| (pyxis_ctrl_m_rm_type & 0x00000000)	/* PCI Read Multiple prefetch algorithm = No prefetch */
    );
    WRITE_IO_CSR( CSR_PYXIS_CTRL, pyxis_ctrl_bits );
    mb( );
/*
** Initialize the Pyxis Error Mask Register (PYXIS_ERR_MASK)
*/
    pyxis_err_mask_bits = (
	  pyxis_err_mask_m_cor_err	    /* Enable reporting for correctable ECC errors */
	| pyxis_err_mask_m_un_cor_err	    /* Enable reporting for uncorrectable ECC errors */
	| pyxis_err_mask_m_mem_nem	    /* Enable reporting for Non-Existent Memory */
	| pyxis_err_mask_m_pci_serr	    /* Enable reporting for PCI SERR errors */
	| pyxis_err_mask_m_rcvd_mas_abt	    /* Enable reporting for PCI Master Abort errors */
	| pyxis_err_mask_m_rcvd_tar_abt	    /* Enable reporting for PCI Target Abort errors */
	| pyxis_err_mask_m_pa_pte_inv	    /* Enable reporting for invalid scatter/gather PTE errors */
	| pyxis_err_mask_m_ioa_timeout	    /* Enable reporting for I/O time-out errors */
    );
    WRITE_IO_CSR( CSR_PYXIS_ERR_MASK, pyxis_err_mask_bits );
    mb( );
/* 
** Dismiss any pending errors 
*/
    WRITE_IO_CSR( CSR_PYXIS_ERR, 0xFFFF );
    mb( );
/* 
**  Set up PCI latency to be full latency
*/
    WRITE_IO_CSR( CSR_PCI_LAT, 0xFF00 );
    mb( );
/* 
**  Make sparse memory regions 1,2 contiguous starting at address 2 gigabyte. 
*/
    WRITE_IO_CSR( CSR_HAE_MEM, 0x8000A000 );
    mb( );
/* 
**  Make sparse I/O regions A,B contiguous in the lower 64 MB of PCI I/O space.
*/
    WRITE_IO_CSR( CSR_HAE_IO, 0x2000000 );
    mb( );
/*
**  Invalidate all TB entries
*/
    BaseCSR = pyxis_a_addr_trans;

    WRITE_IO_CSR( CSR_TBIA, 3 );
    mb( );
/*
**  PCI Target Window 0 will be disabled
*/
    WRITE_IO_CSR( CSR_W0_BASE, 0 );
    mb( );
/*
**  Initialize PCI Target Window 1:
**
**	PCI Address 0x40000000 (1 gigabyte)
**	Size of 1 GByte
**	point to memory address 0
*/
    WRITE_IO_CSR( CSR_W1_MASK, wmask_k_sz1gb );
    WRITE_IO_CSR( CSR_T1_BASE, 0x0 );
    WRITE_IO_CSR( CSR_W1_BASE, 0x40000000 | wbase_m_w_en );
    mb( );
/*
**  PCI Target Windows 2 and 3 will be disabled 
*/
    WRITE_IO_CSR( CSR_W2_BASE, 0 );
    WRITE_IO_CSR( CSR_W3_BASE, 0 );
    mb( );
/* 
**  Initialize the Intel 82378IB PCI-to-ISA Bridge ...
*/
    SIO_pb.hose = 0;
    SIO_pb.bus = 0;
    SIO_pb.slot = 8;
    SIO_pb.function = 0;
/*
** If the state of the South bridge has been saved, then restore
** the initial state.
*/
    if ( sio_state_saved ) {
    	for ( i = 0;  i < sizeof( sio_state_buffer );  i++ ) {
	    outcfgb( &SIO_pb, i, sio_state_buffer[i] );
	}
    }
    else {
/*
**  PAC - PCI Arbiter Control Register
*/
	temp = incfgb( &SIO_pb, 0x41 );
	outcfgb( &SIO_pb, 0x41, temp & 0xFE ); /* Turn off GAT mode */
/*
**  IADCON - ISA Address Decoder Control Register
**
**	IADCON<7:4> - Top of Memory = 16 MBytes
**	IADCON<3:0> - ISA and DMA Memory Cycle to PCI Bus Enables:
**		      <0:0> -   0-512 KByte Memory Block = DISABLED
**		      <1:1> - 512-640 KByte Memory Block = DISABLED
**		      <2:2> - 640-768 KByte Memory Block = DISABLED
**		      <3:3> - 768-960 KByte Memory Block = DISABLED
*/
	outcfgb( &SIO_pb, 0x48, 0xF0 );
/*
** Capture the state of the South bridge
*/    	
	for ( i = 0;  i < sizeof( sio_state_buffer );  i++ ) {
	    sio_state_buffer[i] = incfgb( &SIO_pb, i );
	}
	sio_state_saved = 1;
    }
/*
** Initialize the SMC FDC37C93X Ultra I/O Controller
*/
    ( void )( SMCinit( ) );
/*
** Initialize the slave 82C59A ISA interrupt controller
**
**  ICW1 - ICW4 write required
**  ICW2 - Vector table base address = 8h
**  ICW3 - Slave identification code = 2h
**  ICW4 - 80x86 mode, normal EOI
**  OCW1 - IRQ0 - 7 disabled
*/
    outportb( 0, I82378_C2_ICW1, I82378_ICWSEL | I82378_IC4 );
    outportb( 0, I82378_C2_ICW2, I82378_VECTOR_BASE_1 );
    outportb( 0, I82378_C2_ICW3, I82378_C2_SLAVEID );
    outportb( 0, I82378_C2_ICW4, I82378_MM );
    outportb( 0, I82378_C2_OCW1, I82378_IRQALL );
/*
** Initialize the master 82C59A ISA interrupt controller
**
**  ICW1 - ICW4 write required
**  ICW2 - Vector table base address = 0h
**  ICW3 - Slave controller connected to IRQ2
**  ICW4 - 80x86 mode, normal EOI
**  OCW1 - IRQ2 enabaled, IRQ0, IRQ1, IRQ3-7 disabled
*/
    outportb( 0, I82378_C1_ICW1, I82378_ICWSEL | I82378_IC4 );
    outportb( 0, I82378_C1_ICW2, I82378_VECTOR_BASE_0 );
    outportb( 0, I82378_C1_ICW3, I82378_C1_CICIRQ );
    outportb( 0, I82378_C1_ICW4, I82378_MM );
    outportb( 0, I82378_C1_OCW1, I82378_IRQALL & ~I82378_IRQ2 );
/*
** Send a non-specific EOI command to clear any pending ISA interrupts
*/
    for ( i = 0;  i < 8;  i++ ) {
	outportb( 0, I82378_C2_OCW2, I82378_EOI_NS | i );
	outportb( 0, I82378_C1_OCW2, I82378_EOI_NS | i );
    }
}

/*+
 * ===========================================================================
 * = xportx - Read or write PCI I/O Address Space.			     =
 * ===========================================================================
 *
 * OVERVIEW:
 *  
 *	This function will perform the specified read/write from/to PCI 
 *	I/O address space.
 *
 *	If using PCI sparse address mapping, the HAE_IO register must be
 *	initialized to 2000.0000 to make Region A (32 MB) and Region B
 *	(32 MB) contiguous in the lower 64 MB of PCI I/O space.  All ISA
 *	I/O reads and writes must go through Region A (lower 32 MB) in
 *	order to support legacy addressing of the lower 64 KB of I/O
 *	address space.
 *  
 *	If using PCI linear address mapping, the BWX option (byte/word
 *	architectural extensions) must be specified in the platform build
 *	description file.
 *
 * FORM OF CALL:
 *  
 *	xportx( pb, offset, data, ld_or_st( ) );
 *
 * RETURNS:
 *
 *	On a read, returns a longword of data.  The calling function must 
 *	truncate the data to the proper length.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned __int64 offset - quadword offset in PCI I/O address space.
 *       
 *	unsigned int data - data to write (write transfers only)      
 *
 *	unsigned int *ld_or_st( ) - address of load or store function.
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *	This function will perform a read or write transfer to PCI I/O address 
 *	space.  No error checking or validation of the address is performed.
 *
 * RETURN CODES:
 *
 *	None
 *
--*/

unsigned int xportx( 
    struct pb *pb, 
    unsigned __int64 offset, 
    unsigned int data,
    unsigned int ( *ld_or_st )( ) )
{
/*
**
** CPU to PCI Sparse I/O Address Mapping
** =====================================
**
**       3  3     3  3   3   3   3   3  2
**       9  8     5  4   3   2   1   0  9                     7 6 5 4 3 2  1  0
**  +--+---+-------+---+---+---+---+---+-----------------------+---+---+-------+
**  |  | 1 |  SBZ  | 1 | 0 | 1 | 1 | 0 |   PCI Offset <24:2>   |   |   | 0 0 0 |
**  +--+---+-------+---+---+---+---+---+-----------------------+---+---+-------+
**                                                |              |   |
**                                                |              |   V
**                                                |              V   Size
**                                                |              Byte Offset
**                        3           2 2         |
**                        1           5 4         V           2 1   0
**                       +-------------+-----------------------+-----+
**         PCI Address:  |    HAE_IO   |  Quadword  Address    | 0 0 |
**                       +-------------+-----------------------+-----+
**
**
** CPU to PCI Linear I/O Address Mapping
** =====================================
**
**       3  3    3  3   3   3   3   3  3
**       9  8    7  6   5   4   3   2  1                               2 1   0
**  +--+---+------+---+---+---+---+---+---------------------------------+-----+
**  |  | 1 | Size | X | 1 | 0 | 0 | 1 |        PCI Offset <31:2>        | 0 0 |
**  +--+---+------+---+---+---+---+---+---------------------------------+-----+
**                                                   |
**                                                   |
**                             3                     | 
**                             1                     V                 2 1   0
**                            +-----------------------------------------+-----+
**              PCI Address:  |             Quadword Address            | 0 0 |
**                            +-----------------------------------------+-----+
**
**  Size field is added by hardware:
**	00  Quadword (8 bytes)
**	01  Longword (4 bytes)
**	10  Word (2 bytes)
**	11  Byte
**
*/
    unsigned __int64 BaseIO;
    unsigned __int64 addr;

#if BWX
    BaseIO = PCI_IO_BW;
    addr = ( unsigned __int64 )( 
	      ( BaseIO << PCI_BASE_ADDR_SHIFT )
	    | ( offset & 0xFFFFFFFF ) 
    ); 
#else
    BaseIO = PCI_IO_A;
    addr = ( unsigned __int64 )( 
	      ( BaseIO << PCI_BASE_ADDR_SHIFT ) 
	    | ( ( offset & 0x1FFFFFF ) << PCI_BYTE_OFFSET_SHIFT ) 
	    | ( size << PCI_XFER_LENGTH_SHIFT ) 
    );
#endif
    data = ld_or_st( addr, data );
    return( data );
}


/*+
 * ============================================================================
 * = inportx - Read I/O port in PCI I/O address space.			      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	These routines will read a byte, word, or longword from PCI I/O
 *	address space. 
 *
 * FORM OF CALL:
 *
 *	data = inportb( pb, offset );
 *
 *	data = inportw( pb, offset );
 *
 *	data = inportl( pb, offset );
 *                    
 * RETURNS:
 *
 *	unsigned [char][short][int] - Data from read returned in low 
 *				      order position.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned __int64 offset - Quadword offset in PCI I/O address
 *				  space.
 *
-*/

unsigned char inportb( struct pb *pb, unsigned __int64 offset ) 
{    
#if BWX
    return( xportx( pb, offset, 0, ldb ) );
#else
    return( xportx( pb, offset, 0, ldl ) >> ( ( offset & 3 ) * 8 ) );
#endif
}

unsigned short inportw( struct pb *pb, unsigned __int64 offset )
{
#if BWX
    return( xportx( pb, offset, 0, ldw ) );
#else
    return( xportx( pb, offset, 0, ldl ) >> ( ( offset & 2 ) * 8 ) );
#endif
}

unsigned int inportl( struct pb *pb, unsigned __int64 offset )
{
    return( xportx( pb, offset, 0, ldl ) );
}


/*+
 * ============================================================================
 * = outportx - Write I/O port in PCI I/O address space.		      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	These routines will write a byte, word, or longword to PCI I/O
 *	address space. 
 *
 * FORM OF CALL:
 *
 *	outportb( pb, offset, data ); 
 *
 *	outportw( pb, offset, data );
 *
 *	outportl( pb, offset, data );
 *                    
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned __int64 offset - Quadword offset in PCI I/O address
 *				  space.
 *
 *	unsigned [char][short][int] data - Data to be written.
 *
-*/

void outportb( struct pb *pb, unsigned __int64 offset, unsigned char data )
{        
#if BWX
    ( void )xportx( pb, offset, data, stb );
#else
    ( void )xportx( pb, offset, data << ( ( offset & 3 ) * 8 ), stl );
#endif
}

void outportw( struct pb *pb, unsigned __int64 offset, unsigned short data )
{       
#if BWX
    ( void )xportx( pb, offset, data, stw );
#else
    ( void )xportx( pb, offset, data << ( ( offset & 2 ) * 8 ), stl );
#endif
}

void outportl( struct pb *pb, unsigned __int64 offset, unsigned int data )
{       
    ( void )xportx( pb, offset, data, stl );
}


/*+
 * ===========================================================================
 * = xmemx - Read or write PCI Memory Address Space.			     =
 * ===========================================================================
 *
 * OVERVIEW:
 *  
 *	This function will perform the specified read/write from/to PCI
 *	Memory address space.
 *  
 *	If using PCI sparse address mapping, the HAE_MEM register must be
 *	initialized to 8000.A000 to make Region 0 (512 MB) and Region 1
 *	(128 MB) contiguous starting at PCI address 2 GB.  Region 2 (64 MB)
 *	is used to address the lower 16 MB of PCI sparse memory space 
 *	starting at PCI address 0.
 *
 *	If using PCI linear address mapping, the BWX option (byte/word
 *	architectural extensions) must be specified in the platform build
 *	description file.
 *
 * FORM OF CALL:
 *  
 *	xmemx( pb, offset, data, ld_or_st( ) );
 *
 * RETURNS:
 *
 *	On a read, returns either a longword or quadword of data.  The 
 *	calling function must truncate the data to the proper length.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned __int64 offset - quadword offset in PCI memory address
 *				  space.
 *       
 *	unsigned __int64 data - data to write (write transfers only)
 *       
 *	unsigned __int64 *ld_or_st( ) - address of load or store function.
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *	This function will perform a read or write transfer to PCI memory 
 *	address space.  No error checking or validation of the address is 
 *	performed.
 *
 * RETURN CODES:
 *
 *	None
 *
--*/

unsigned int xmemx( 
    struct pb *pb, 
    unsigned __int64 offset, 
    unsigned __int64 data,
    unsigned __int64 ( *ld_or_st )( ) )
{
/*
**
** CPU to PCI Sparse Memory Region 0 Address Mapping
** =================================================
**
**       3  3     3  3  3
**       9  8     5  4  3                                     7 6 5 4 3 2  1  0
**  +--+---+-------+---+---------------------------------------+---+---+-------+
**  |  | 1 |  SBZ  | 0 |           PCI Offset <28:2>           |   |   | 0 0 0 |
**  +--+---+-------+---+---------------------------------------+---+---+-------+
**                                          |                    |   |
**                                          |                    |   V
**                                          |                    V   Size
**                                          |                    Byte Offset
**               3             2 2          |
**               1             9 8          V                 2 1   0
**              +---------------+------------------------------+-----+
** PCI Address: | HAE_MEM<31:29>|      Quadword  Address       | 0 0 |
**              +---------------+------------------------------+-----+
**
**
** CPU to PCI Linear Memory Address Mapping
** ========================================
**
**       3  3    3  3   3   3   3   3  3
**       9  8    7  6   5   4   3   2  1                               2 1   0
**  +--+---+------+---+---+---+---+---+---------------------------------+-----+
**  |  | 1 | Size | X | 1 | 0 | 0 | 0 |        PCI Offset <31:2>        | 0 0 |
**  +--+---+------+---+---+---+---+---+---------------------------------+-----+
**                                                   |
**                                                   |
**                             3                     | 
**                             1                     V                 2 1   0
**                            +-----------------------------------------+-----+
**              PCI Address:  |             Quadword Address            | 0 0 |
**                            +-----------------------------------------+-----+
**
**  Size field is added by hardware:
**	00  Quadword (8 bytes)
**	01  Longword (4 bytes)
**	10  Word (2 bytes)
**	11  Byte
**
*/
    unsigned __int64 BaseMem;
    unsigned __int64 addr;

#if BWX
    BaseMem = PCI_MEM_BW;
    addr = ( unsigned __int64 )( 
	      ( BaseMem << PCI_BASE_ADDR_SHIFT ) 
	    | ( offset & 0xFFFFFFFF ) 
    );
#else
    if ( offset < ( 16 * 1024 * 1024 ) ) {
	BaseMem = PCI_MEM_2;
	addr = ( unsigned __int64 )( 
		  ( BaseMem << PCI_BASE_ADDR_SHIFT ) 
		| ( ( offset & 0x3FFFFFF ) << PCI_BYTE_OFFSET_SHIFT ) 
		| ( size << PCI_XFER_LENGTH_SHIFT ) 
        );
    }
    else {
	BaseMem = PCI_MEM_0;
	addr = ( unsigned __int64 )( 
		      ( BaseMem << PCI_BASE_ADDR_SHIFT ) 
		    | ( ( offset & 0x1FFFFFFF ) << PCI_BYTE_OFFSET_SHIFT ) 
		    | ( size << PCI_XFER_LENGTH_SHIFT ) 
	);
    }
#endif
    data = ld_or_st( addr, data );
    return( data );
}

/*+
 * ============================================================================
 * = inmemx - Read data from PCI Memory address space.			      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	These routines will read a byte, word, longword, or quadword from 
 *	PCI Memory address space. 
 *
 * FORM OF CALL:
 *
 *	data = inmemb( pb, offset );
 *
 *	data = inmemw( pb, offset );
 *
 *	data = inmeml( pb, offset );
 *
 *	data = inmemq( pb, offset );
 *                    
 * RETURNS:
 *
 *	unsigned [char][short][int][__int64] - Data from read returned in 
 *					       low order position.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned __int64 offset - Quadword offset in PCI Memory address 
 *				  space.
 *
-*/

unsigned char inmemb( struct pb *pb, unsigned __int64 offset )
{
#if BWX
    return( xmemx( pb, offset, 0, ldb ) );
#else
    return( xmemx( pb, offset, 0, ldl ) >> ( ( offset & 3 ) * 8 ) );
#endif
}

unsigned short inmemw( struct pb *pb, unsigned __int64 offset )
{
#if BWX
    return( xmemx( pb, offset, 0, ldw ) );
#else
    return( xmemx( pb, offset, 0, ldl ) >> ( ( offset & 2 ) * 8 ) );
#endif
}

unsigned long inmeml( struct pb *pb, unsigned __int64 offset )
{
    return( xmemx( pb, offset, 0, ldl ) );
}

unsigned __int64 inmemq( struct pb *pb, unsigned __int64 offset )
{
    return( xmemx( pb, offset, 0, ldq ) );
}


/*+
 * ============================================================================
 * = outmemx - Write data to PCI Memory address space.			      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	These routines will write a byte, word, longword, or quadword to 
 *	PCI Memory address space. 
 *
 * FORM OF CALL:
 *
 *	outmemb( pb, offset, data ); 
 *
 *	outmemw( pb, offset, data );
 *
 *	outmeml( pb, offset, data );
 *
 *	outmemq( pb, offset, data );
 *                    
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned __int64 offset - Quadword offset in PCI Memory address
 *				  space.
 * 
 *	unsigned [char][short][int][__int64] data - Data to be written.
 *
-*/

void outmemb( struct pb *pb, unsigned __int64 offset, unsigned char data )
{
#if BWX
    ( void )xmemx( pb, offset, data, stb );
#else
    ( void )xmemx( pb, offset, data << ( ( offset & 3 ) * 8 ), stl );
#endif
}

void outmemw( struct pb *pb, unsigned __int64 offset, unsigned short data )
{
#if BWX
    ( void )xmemx( pb, offset, data, stw );
#else
    ( void )xmemx( pb, offset, data << ( ( offset & 2 ) * 8 ), stl );
#endif
}

void outmeml( struct pb *pb, unsigned __int64 offset, unsigned int data )
{
    ( void )xmemx( pb, offset, data, stl );
}

void outmemq( struct pb *pb, unsigned __int64 offset, unsigned  __int64 data )
{
    ( void )xmemx( pb, offset, data, stq );
}


/*+
 * ============================================================================
 * = xdmem - Generate a PCI Dense Memory space address.                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will generate an address in the 4 GB PCI dense memory 
 *	address space. 
 *
 * FORM OF CALL:
 *
 *	xdmem( pb, offset ); 
 *                    
 * RETURNS:
 *
 *	unsigned __int64 - PCI dense memory space address
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned __int64 offset - Quadword offset in PCI dense memory 
 *			          address space.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
unsigned __int64 xdmem( struct pb *pb, unsigned __int64 offset )
{
/*
**
** CPU to PCI Dense Memory Address Mapping
** =======================================
**
**       3  3     3  3   3   3  3
**       9  8     5  4   3   2  1                                       2 1   0
**  +--+---+-------+---+---+---+-----------------------------------------+-----+
**  |  | 1 |       | 1 | 1 | 0 |         PCI Offset <31:2>               | 0 0 |
**  +--+---+-------+---+---+---+-----------------------------------------+-----+
**                                                   |
**                                                   |
**                              3                    |
**                              1                    V                  2 1   0
**                             +-----------------------------------------+-----+
**               PCI Address:  |             Quadword Address            | 0 0 |
**                             +-----------------------------------------+-----+
**
*/
    unsigned __int64 BaseMem;
    unsigned __int64 addr;

#if BWX
    BaseMem = PCI_MEM_BW;
#else
    BaseMem = PCI_DMEM;
#endif
    addr = ( unsigned __int64 )( 
	      ( BaseMem << PCI_BASE_ADDR_SHIFT ) 
	    | ( offset & 0xFFFFFFFF ) 
    );
    return( addr );
}


/*+
 * ============================================================================
 * = indmemx - Read data from PCI dense memory address space.      	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	These routines will read a longword or quadword from the 4 GB PCI 
 *	dense memory address space. 
 *
 * FORM OF CALL:
 *
 *	data = indmeml( pb, offset ); 
 *
 *	data = indmemq( pb, offset );
 *                    
 * RETURNS:
 *
 *	unsigned [int][__int64] - Data from read.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned __int64 offset - Quadword offset in PCI dense memory 
 *				  address space.
 *
-*/

unsigned int indmeml( struct pb *pb, unsigned __int64 offset )
{
    return( ldl( xdmem( pb, offset ) ) );
}

unsigned __int64 indmemq( struct pb *pb, unsigned __int64 offset )
{
    return( ldq( xdmem( pb, offset ) ) );
}


/*+
 * ============================================================================
 * = outdmemx - Write data to PCI dense memory address space.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	These routines will write a longword or quadword to the 4 GB PCI 
 *	dense memory address space. 
 *
 * FORM OF CALL:
 *
 *	outdmeml( pb, offset, data ); 
 *
 *	outdmemq( pb, offset, data );
 *                    
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned __int64 offset - Quadword offset in PCI dense memory 
 *				  address space.
 *
 *	unsigned [int][__int64] data - Data to be written.
 *
-*/

void outdmeml( struct pb *pb, unsigned __int64 offset, unsigned int data )
{
    stl( xdmem( pb, offset ), data );
}

void outdmemq( struct pb *pb, unsigned __int64 offset, unsigned __int64 data )
{
    stq( xdmem( pb, offset ), data );
}


/*+
 * ===========================================================================
 * = xcfgx - Read or write PCI Configuration Address Space.		     =
 * ===========================================================================
 *
 * OVERVIEW:
 *  
 *	This function takes device specific information, in the form of a 
 *	port block data structure, and uses it to access a 256 byte per
 *	device region in PCI Configuration address space.
 *  
 *	If using PCI linear address mapping, the BWX option (byte/word
 *	architectural extensions) must be specified in the platform build
 *	description file.
 *
 * FORM OF CALL:
 *  
 *	xmemx( pb, offset, data, ld_or_st( ) );
 *
 * RETURNS:
 *
 *	On a read, returns a longword of data.  The calling 
 *	function must truncate the data to the proper length.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned __int64 offset - quadword offset in PCI configuration 
 *				  address space.
 *       
 *	unsigned int data - data to write (write transfers only)
 *       
 *	unsigned int *ld_or_st( ) - address of load or store function.
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *	This function will perform a read or write transfer to PCI 
 *	Configuration address space.  No error checking or validation
 *	of the address is performed.
 *
 * RETURN CODES:
 *
 *	None
 *
--*/

unsigned int xcfgx( 
    struct pb *pb, 
    unsigned __int64 offset, 
    unsigned int data,
    unsigned int ( *ld_or_st )( ) )
{
    unsigned int bus = pb->bus;
    unsigned int slot = pb->slot;
    unsigned int func = pb->function;
    unsigned int cfg_offset;
    unsigned __int64 BaseCSR = pyxis_k_main_csr_base;
    unsigned __int64 BaseIO;
    unsigned __int64 addr;
/*
**
** CPU to PCI Sparse Configuration Address Mapping (Type 0)
** ========================================================
**
**      3 3   3 3 3 3 3 3 2 2     2 2     1 1    1 1
**      9 8   5 4 3 2 1 0 9 8     1 0     6 5    3 2        7 6 5 4 3 2  1  0
**  +--+-+-----+-+-+-+-+-+-+-------+-------+------+----------+---+---+-------+
**  |  |1| MBZ |1|1|1|0|0|0|       |       |      |          |   |   | 0 0 0 |
**  +--+-+-----+-+-+-+-+-+-+-------+-------+------+----------+---+---+-------+
**                                /        |      |          | |   |
**                               /         |      |          | |   V
**                              /          |      |          | V   Size
**                             /           |      |          | Byte Offset
**                            /            |      |          |
**                           /             |      |          |
**                          /              |      |          |
**                       3 /              1|1     |          |
**                       1/               1|0    8|7        2|1   0
**                      +------------------+------+----------+-----+
**        PCI Address:  |       IDSEL      | FUNC |    REG   | 0 0 |
**                      +------------------+------+----------+-----+
**                                                              ^
**                                                              |
**                                                              CSR_CFG<1:0>
**
** CPU to PCI Linear Configuration Address Mapping (Type 0)
** ========================================================
**
**      3 3  3 3 3 3 3 3 3 3 2 2      2 2     1 1    1 1
**      9 8  7 6 5 4 3 2 1 0 9 8      1 0     6 5    3 2        7 6    2 1   0
**  +--+-+----+-+-+-+-+-+-+-+-+--------+-------+------+----------+------+-----+
**  |  |1|Size|X|1|0|1|0|0|0|0|        |       |      |          |      | 0 0 |
**  +--+-+----+-+-+-+-+-+-+-+-+--------+-------+------+----------+------+-----+
**                                    /        |      |          |
**                                   /         |      |          |
**                                  /          |      |          |
**                                 /           |      |          |
**                                /            |      |          |
**                               /             |      |          |
**                              /              |      |          |
**                           3 /              1|1     |          |
**                           1/               1|0    8|7        2|1   0
**                          +------------------+------+----------+-----+
**            PCI Address:  |       IDSEL      | FUNC |    REG   | 0 0 |
**                          +------------------+------+----------+-----+
**                                                                  ^
**                                                                  |
**                                                                  CSR_CFG<1:0>
**  Size field is added by hardware:
**	00  Quadword (8 bytes)
**	01  Longword (4 bytes)
**	10  Word (2 bytes)
**	11  Byte
**
** CPU to PCI Sparse Configuration Address Mapping (Type 1)
** ========================================================
**
**      3 3   3 3 3 3 3 3 2 2     2 2     1 1    1 1
**      9 8   5 4 3 2 1 0 9 8     1 0     6 5    3 2        7 6 5 4 3 2  1  0
**  +--+-+-----+-+-+-+-+-+-+-------+-------+------+----------+---+---+-------+
**  |  |1| MBZ |1|1|1|0|0|0|       |       |      |          |   |   | 0 0 0 |
**  +--+-+-----+-+-+-+-+-+-+-------+-------+------+----------+---+---+-------+
**                         |       |       |      |          | |   |
**                         |       |       |      |          | |   V
**                         |       |       |      |          | V   Size
**                         |       |       |      |          | Byte Offset
**                         |       |       |      |          |
**             3    2 2   2|2     1|1     1|1     |          |
**             1    7 6   4|3     6|5     1|0    8|7        2|1   0
**            +------+-----+-------+-------+------+----------+-----+
**  PCI Addr: | 0000 | 000 |  BUS  |  DEV  | FUNC |    REG   | 0 1 |
**            +------+-----+-------+-------+------+----------+-----+
**                                                              ^
**                                                              |
**                                                              CSR_CFG<1:0>
**
** CPU to PCI Linear Configuration Address Mapping (Type 1)
** ========================================================
**
**      3 3  3 3 3 3 3 3 3 3 2 2     2 2     1 1    1 1
**      9 8  7 6 5 4 3 2 1 0 9 8     1 0     6 5    3 2        7 6    2 1   0
**  +--+-+----+-+-+-+-+-+-+-+-+-------+-------+------+----------+------+-----+
**  |  |1|Size|X|1|0|1|1|0|0|0|       |       |      |          |      | 0 0 |
**  +--+-+----+-+-+-+-+-+-+-+-+-------+-------+------+----------+------+-----+
**                            |       |       |      |          |
**                            |       |       |      |          |
**                            |       |       |      |          |
**                            |       |       |      |          |
**                            |       |       |      |          |
**                3    2 2   2|2     1|1     1|1     |          |
**                1    7 6   4|3     6|5     1|0    8|7        2|1   0
**               +------+-----+-------+-------+------+----------+-----+
**     PCI Addr: | 0000 | 000 |  BUS  |  DEV  | FUNC |    REG   | 0 1 |
**               +------+-----+-------+-------+------+----------+-----+
**                                                                 ^
**                                                                 |
**                                                                 CSR_CFG<1:0>
**  Size field is added by hardware:
**	00  Quadword (8 bytes)
**	01  Longword (4 bytes)
**	10  Word (2 bytes)
**	11  Byte
*/
    if ( ( slot > ( bus ? 31 : PCI_MAX_SLOT ) ) || ( func > 7 ) ) {
	return( 0xFFFFFFFF );
    }
    if ( bus ) {
#if BWX
	BaseIO = PCI_CFG1_BW;
#else
	BaseIO = PCI_CFG;
#endif
	WRITE_IO_CSR( CSR_CFG, pci_k_cfgtype1 );
	mb( );
	cfg_offset = ( 
	      ( bus << 16 ) 
	    + ( slot << 11 ) 
	    + ( func << 8 ) 
	    + ( offset & 0xFF ) 
	);
    }
    else {
#if BWX
	BaseIO = PCI_CFG0_BW;
#else
	BaseIO = PCI_CFG;
#endif
	WRITE_IO_CSR( CSR_CFG, pci_k_cfgtype0 );
	mb( );
	cfg_offset = ( 
	      ( slot << 11 ) 
	    + ( func << 8 ) 
	    + ( offset & 0xFF ) 
	);
    }
#if BWX
    addr = ( unsigned __int64 )( 
	      ( BaseIO << PCI_BASE_ADDR_SHIFT ) 
	    | ( cfg_offset ) 
    );
#else
    addr = ( unsigned __int64 )( 
	      ( BaseIO << PCI_BASE_ADDR_SHIFT ) 
	    | ( cfg_offset << PCI_BYTE_OFFSET_SHIFT ) 
	    | ( size << PCI_XFER_LENGTH_SHIFT ) 
    );
#endif
    data = ld_or_st( addr, data );
    WRITE_IO_CSR( CSR_CFG, pci_k_cfgtype0 );
    mb( );
    return( data );
}


/*+
 * ============================================================================
 * = incfgx - Read data from PCI configuration address space.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	These routines will read a byte, word, or longword from a 256 byte
 *	per device region in PCI Configuration address space.
 *
 * FORM OF CALL:
 *
 *	data = incfgb( pb, offset );
 *                    
 *	data = incfgw( pb, offset );
 *                    
 *	data = incfgl( pb, offset );
 *                    
 * RETURNS:
 *
 *	unsigned [char][short][int] - Data from read returned in low
 *				      order position.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *	                See pb_def.h for more info.
 *
 *	unsigned __int64 offset - Quadword offset in region to read.
 *
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

unsigned char incfgb( struct pb *pb, unsigned  __int64 offset ) 
{
#if BWX
    return( xcfgx( pb, offset, 0, ldb ) );
#else
    return( xcfgx( pb, offset, 0, ldl ) >> ( ( offset & 3 ) * 8 ) );
#endif
}

unsigned short incfgw( struct pb *pb, unsigned  __int64 offset ) 
{
#if BWX
    return( xcfgx( pb, offset, 0, ldw ) );
#else
    return( xcfgx( pb, offset, 0, ldl ) >> ( ( offset & 2 ) * 8 ) );
#endif
}

unsigned int incfgl( struct pb *pb, unsigned  __int64 offset ) 
{
    return( xcfgx( pb, offset, 0, ldl ) );
}


/*+
 * ============================================================================
 * = outcfgx - Write data to PCI configuration address space.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	These routines will write a byte, word, or longword to a 256 byte per
 *	device region in PCI Configuration address space.
 *
 * FORM OF CALL:
 *
 *	outcfgb( pb, offset, data );
 *                    
 *	outcfgw( pb, offset, data );
 *                    
 *	outcfgl( pb, offset, data );
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
 *	unsigned __int64 offset - Quadword offset in region to write.
 *
 *	unsigned [char][short][int] - Data to write, passed by value.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void outcfgb( struct pb *pb, unsigned __int64 offset, unsigned char data ) 
{
#if BWX
    ( void )( xcfgx( pb, offset, data, stb ) );
#else
    ( void )( xcfgx( pb, offset, data << ( ( offset & 3 ) * 8 ), stl ) );
#endif
}

void outcfgw( struct pb *pb, unsigned __int64 offset, unsigned short data ) 
{
#if BWX
    ( void )( xcfgx( pb, offset, data, stw ) );
#else
    ( void )( xcfgx( pb, offset, data << ( ( offset & 2 ) * 8 ), stl ) );
#endif
}

void outcfgl( struct pb *pb, unsigned __int64 offset, unsigned int data ) 
{
    ( void )( xcfgx( pb, offset, data, stl ) );
}


/*+
 * ============================================================================
 * = read_pyxis_csr - Read a Pyxis CSR                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      This routine is used to read the Pyxis CSRs.
 *
 * FORM OF CALL:
 *
 *	data = read_pyxis_csr( address );
 *                    
 * RETURNS:
 *
 *	Quadword containing the data read
 *
 * ARGUMENTS:
 *
 *	unsigned __int64 address - Quadword address of CSR to read.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
unsigned __int64 read_pyxis_csr( unsigned __int64 address )
{
#pragma pointer_size save
#pragma pointer_size 64
    volatile unsigned __int64 *p;
#pragma pointer_size restore
    unsigned __int64 data;

    ( unsigned __int64 )p = address;
    mb( );
    data = *p;
    return( data );
}


/*+
 * ============================================================================
 * = write_pyxis_csr - Write a Pyxis CSR                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      This routine is used to write the Pyxis CSRs. 
 *
 * FORM OF CALL:
 *
 *	write_pyxis_csr( address, data );
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	unsigned __int64 address - Quadword address of CSR to write.
 *	unsigned __int64 data    - Quadword of data to be written.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void write_pyxis_csr( unsigned __int64 address, unsigned __int64 data )
{
#pragma pointer_size save
#pragma pointer_size 64
    volatile unsigned __int64 *p;
#pragma pointer_size restore

    ( unsigned __int64 )p = address;
    mb( );
    *p = data;
}


/*+
 * ============================================================================
 * = set_hae - Set the Host Address Extenstion register.                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will modify the appropriate bit field in the
 *	Pyxis HAE_MEM register corresponding to the high order 
 *	PCI address bits <31:29> in PCI sparse memory space.
 *
 *	The Host Address Extension register is used to generate 
 *	PCI address bits AD<31:26> during certain CPU initiated
 *	transactions that address PCI memory space.
 *
 * FORM OF CALL:
 *
 *	set_hae( value );
 *                    
 * RETURNS:
 *
 *	unsigned int - Previous value of HAE_MEM.
 *
 * ARGUMENTS:
 *
 *	unsigned int value - value to extract new HAE_MEM bits from
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

unsigned int set_hae( unsigned int value ) 
{
    unsigned __int64 BaseCSR = pyxis_k_main_csr_base;
    unsigned int hae_mem;

    hae_mem = ( unsigned int )READ_IO_CSR( CSR_HAE_MEM );
#if BWX
    return( hae_mem );
#else
    WRITE_IO_CSR( CSR_HAE_MEM, ( value & haemem_m_addr_ext_1 ) | ( hae_mem & ~haemem_m_addr_ext_1 ) );
    mb( );
    return( hae_mem );
#endif
}


/*+
 * ============================================================================
 * = io_get_window_base - return the current value of PCI window base         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will return the current value of the PCI base
 *	address for PCI target window 1.
 *
 * FORM OF CALL:
 *
 *	io_get_window_base( pb );
 *                    
 * RETURNS:
 *
 *	unsigned int - current PCI target window base address
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to port block data structure.
 *			This argument, although required, is only here 
 *			for compatibility with code that may really need it.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
unsigned int io_get_window_base( struct pb *pb )
{
    unsigned __int64 BaseCSR = pyxis_a_addr_trans;

    return( ( unsigned int )READ_IO_CSR( CSR_W1_BASE ) & wbase_m_w_base );
}


unsigned int io_get_asic_revision( void )
{
    unsigned int asic_revision;
    unsigned __int64 BaseCSR = pyxis_k_main_csr_base;

    asic_revision = ( READ_IO_CSR( CSR_PYXIS_REV ) & 0xFF );
    return( asic_revision + 1 );
}


/*+
 * ============================================================================
 * = probe_pci_setup - Define initial PCI memory map                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  	This routine defines the initial memory map for I/O, sparse, and
 *	dense memory space.  This information is used by the generic PCI
 *	sizing and configuring algorithm in PROBE_IO.C
 *
 * FORM OF CALL:
 *  
 *	probe_pci_setup( hose )
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *      hose - The hose number for the PCI bus (always zero for SX164).
 *              
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void probe_pci_setup( int hose )
{
    unsigned int base;
    unsigned int size;

    base = 0x1000;
    size = 0x10000 - 0x1000;
    pci_add_region( base, size, SIZE_GRAN_BASE_IO_16_BIT );
    base = 0x10000;
    size = 0xFF0000;
    pci_add_region( base, size, SIZE_GRAN_BASE_IO );
    base = 0x82000000;
    size = 0x6000000;
    pci_add_region( base, size, SIZE_GRAN_BASE_SPARSE );
    base = 0x88000000;
    size = 0x78000000;
    pci_add_region( base, size, SIZE_GRAN_BASE_DENSE );
}


void probe_pci_cleanup( int hose )
{
    if ( hose == 0 ) {
/*
** Force automatic configuration and tell the BIOS emulator that the
** PCI bus is configured.  This prevents the BIOS from re-configuring
** the device.
*/
	manual_config = 0;
	ETAPciMaxBusNumber = pci_bus;
    }
}


unsigned int read_pci_id( int hose, int bus, int slot, int function )
{
    unsigned __int64 BaseCSR = pyxis_k_main_csr_base;
    unsigned int id, tmp;
    struct pb pb;

    pb.hose	= ( unsigned int )hose;
    pb.bus	= ( unsigned int )bus;
    pb.slot	= ( unsigned int )slot;
    pb.function = ( unsigned int )function;
/*
** Clear PYXIS_CTRL<FILL_ER_EN> when probing for PCI devices using 
** configuration space reads.  This will prevent the host bridge 
** from generating an ECC error if no device responds to the 
** configuration cycle.
*/
    tmp = READ_IO_CSR( CSR_PYXIS_CTRL );
    tmp &= ( ~pyxis_ctrl_m_fillerr_en );
    WRITE_IO_CSR( CSR_PYXIS_CTRL, tmp );
    mb( );
/*
** The following is to fix a problem with an I/O abort timeout error
** being signaled during a probe to the SWXCR RAID controller.
*/
    ExpectMachineCheck = 1; 

    do {
    	ioa_timeout_flag = 0;
	tmp = incfgl( &pb, VEND_DEV_ID );
	krn$_micro_delay( 10000 );
    } while ( ioa_timeout_flag );

    ExpectMachineCheck = 0;
/*
** Read the Vendor/Device ID 
*/
    id = incfgl( &pb, VEND_DEV_ID );
/* 
** Read the Pyxis Error register 
*/
    tmp = READ_IO_CSR( CSR_PYXIS_ERR );
    if ( tmp & 
	( pyxis_err_m_rcvd_mas_abt 
	| pyxis_err_m_rcvd_tar_abt 
	| pyxis_err_m_pci_serr 
	| pyxis_err_m_pci_perr 
	| pyxis_err_m_pci_addr_pe ) )
   {
/*
** Indicate probe failed
*/
	id = 0;
/* 
** Dismiss error(s) 
*/
	WRITE_IO_CSR( CSR_PYXIS_ERR, tmp & 0xFFFF );
	mb( );
/* 
** Make sure that the CSR write completes 
*/
	tmp = READ_IO_CSR( CSR_PYXIS_ERR );
    }
    if ( id == 0xFFFFFFFF )
	id = 0;
/* 
** Re-enable PYXIS_CTRL<FILL_ER_EN> 
*/
    tmp = READ_IO_CSR( CSR_PYXIS_CTRL );
    tmp |= pyxis_ctrl_m_fillerr_en;
    WRITE_IO_CSR( CSR_PYXIS_CTRL, tmp );
    mb( );
    return( id );
}


unsigned int get_hose_type( int hose )
{
    if ( hose == 0 )
	return( TYPE_PCI );
    else
	return( TYPE_UNKNOWN );
}


void get_pci_csr_vector ( struct pb *pb )
{
    unsigned int i;
    unsigned int hose;
    unsigned int bus;
    unsigned int slot;
    struct pb *parent_pb;
    struct pci_device_misc *pdm;
    struct pci_bridge_misc *pbm;
    unsigned short command;
    unsigned char header_type;
    unsigned int base_int_pin;
    unsigned char int_pin;
    unsigned char irq;
    unsigned int base, size;
    unsigned short bridge_control;

    hose = pb->hose;
    bus  = pb->bus;
    slot = pb->slot;
    pdm  = pb->pdm;
    pbm  = pdm->pbm;
    parent_pb = pb->parent;
    if ( parent_pb ) {
	struct pci_device_misc *pdm;
	struct pci_bridge_misc *pbm;

	pdm = parent_pb->pdm;
	pbm = pdm->pbm;
	base_int_pin = pbm->base_int_pin;
    }
    else {
	base_int_pin = 0;
    }
/*
** Command Register Initialization:
**
**	- I/O Access Enable
**	    1 - Allow the device's I/O address decoder(s)
**		(if any are implemented) to respond to PCI
**		I/O accesses.
**
**	- Memory Access Enable
**	    1 - Allow the device to respond to PCI Memory
**		accesses (if it implements any memory address
**		decoders).
** 
**	- Master Enable
**	    1 - Allow device to behave as a bus master (if it
**		has bus master capability).
*/
    command = 0x0007;
/*
** If device does not respond to I/O space accesses,
** disable the device response.
*/
    if ( pdm->flags & PCI_FLAGS_NOT_ASSIGNED_IO )
	command &= ~0x0001;
/*
** If device does not respond to memory space accesses,
** disable the device response.
*/
    if ( pdm->flags & PCI_FLAGS_NOT_ASSIGNED_MEM )
	command &= ~0x0002;
/*
** The Header Type Register defines the layout of bytes 10h through 
** 3Fh in PCI configuration space and also whether or not the device
** contains multiple functions.  Bit 7 in the header type register
** is used to identify a multi-function device.  Bits 6 through
** 0 specify the layout of bytes 10h through 3Fh.
*/
    header_type = incfgb( pb, 0x0E ) & 0x7F;
    switch ( header_type ) {
/*
** PCI Device Configuration
*/
    	case 0:
	    pb->csr = pdm->base[1];
	    int_pin = incfgb( pb, 0x3D );
	    if ( int_pin ) {
		if ( base_int_pin ) {
		    int_pin = ( ( slot + base_int_pin - 1 + int_pin - 1 ) & 3 ) + 1;
		    slot = pdm->base_slot;
		}
		irq = pci_irq_table[ ( ( slot - PCI_MIN_SLOT ) * 4 ) + int_pin - 1 ];
		if ( irq != 0xFF ) {
		    if ( vector_allocated[ irq ] ) {
			if ( !( bus == 0 && slot == 11 && int_pin == 1 ) ) {
			    err_printf(
				"Vector allocation failed for hose %d, bus %d, slot %d, pin %d\n",
				hose, bus, slot, int_pin );
			    err_printf(
				"Vector %04x already allocated\n", PCI_BASE_VECTOR + irq );
			}
		    }
		    else {
			vector_allocated[ irq ] = 1;
			pb->vector = irq + PCI_BASE_VECTOR;
#if ((IPL_RUN) && (IPL_RUN > 19))
			pb->vector = 0;
#endif
		    }
		}
		outcfgb( pb, INT_LINE, irq );
	    }
	    for ( i = 0;  i < 6;  i++ ) {
	    	outcfgl( pb, BASE_ADDR0 + i * 4, pdm->base[i] );
	    }
	    outcfgl( pb, EXP_ROM_BASE, pdm->base[6] );
	    outcfgw( pb, COM_STAT, command );
	    break;
/*
** PCI-to-PCI Bridge Configuration
*/
	case 1:
	    if ( base_int_pin ) 
		pbm->base_int_pin = ( ( slot + base_int_pin - 1 ) & 3 ) + 1;
	    else
		pbm->base_int_pin = 1;
/*
** 0x10 - Base Address 0
** 0x14 - Base Address 1
** 0x38 - Expansion ROM Base Address
** 0x18 - Primary Bus Number
** 0x19 - Secondary Bus Number
** 0x1A - Subordinate Bus Number
** 0x1B - Secondary Latency TImer
** 0x1E - Secondary Status
*/
	    outcfgl( pb, 0x10, pdm->base[0] );
	    outcfgl( pb, 0x14, pdm->base[1] );
	    outcfgl( pb, 0x38, pdm->base[2] );
	    outcfgb( pb, 0x18, ( unsigned char )( pbm->pri_bus ) );
	    outcfgb( pb, 0x19, ( unsigned char )( pbm->sec_bus ) );
	    outcfgb( pb, 0x1A, ( unsigned char )( pbm->sub_bus ) );
	    outcfgb( pb, 0x1B, 0xFF );
	    outcfgw( pb, 0x1E, 0xFFFF );
/*
** 0x1C - I/O Base
** 0x1D - I/O Limit
** 0x30 - I/O Base upper 16 bits
** 0x32 - I/O Limit upper 16 bits
*/
	    if ( pbm->flags & PCI_FLAGS_IO_16_BIT ) {
		base = pbm->base[ SIZE_GRAN_BASE_IO_16_BIT ];
		size = pbm->size[ SIZE_GRAN_BASE_IO_16_BIT ];
	    }
	    else {
		base = pbm->base[ SIZE_GRAN_BASE_IO ];
		size = pbm->base[ SIZE_GRAN_BASE_IO ];
	    }
	    if ( size == 0 ) {
		base = -1;
		size = 2;
	    }
	    outcfgb( pb, 0x1C, ( unsigned char )( base >> 8 ) );
	    outcfgb( pb, 0x1D, ( unsigned char )( ( base + size - 1 ) >> 8 ) );
	    outcfgw( pb, 0x30, ( unsigned short )( base >> 16 ) );
	    outcfgw( pb, 0x32, ( unsigned short )( ( base + size - 1 ) >> 16 ) );
/*
** 0x20 - Memory Base
** 0x22 - Memory Limit
*/
	    base = pbm->base[ SIZE_GRAN_BASE_SPARSE ];
	    size = pbm->size[ SIZE_GRAN_BASE_SPARSE ];
	    if ( size == 0 ) {
	    	base = -1;
		size = 2;
	    }
	    outcfgw( pb, 0x20, ( unsigned short )( base >> 16 ) );
	    outcfgw( pb, 0x22, ( unsigned short )( ( base + size - 1 ) >> 16 ) );
/*
** 0x24 - Prefetchable Memory Base
** 0x26 - Prefetchable Memory Limit
*/
	    base = pbm->base[ SIZE_GRAN_BASE_DENSE ];
	    size = pbm->size[ SIZE_GRAN_BASE_DENSE ];
	    if ( size == 0 ) {
	    	base = -1;
		size = 2;
	    }
	    outcfgw( pb, 0x24, ( unsigned short )( base >> 16 ) );
	    outcfgw( pb, 0x26, ( unsigned short )( ( base + size - 1 ) >> 16 ) );
/*
** Bridge Control Initialization
**
**	- Parity Error Response
**	    1 - Enable parity error reporting and detection on
**	        the secondary interface.
**
**	- SERR# Enable
**	    1 - Enable the forwarding of secondary SERR# to 
**	        primary SERR#.
**
**	- Master Abort Mode 
**	    1 - Report master aborts by signaling target abort
**	        if possible, or by the assertion of SERR#.
*/
	    bridge_control = 0x0023;
/*
** Check if this bridge supports the forwarding of VGA compatible
** memory and I/O addresses from the primary to the secondary
** interface, and if so, set the VGA Enable bit.
*/
	    if ( pbm->flags & PCI_FLAGS_VGA ) {
	    	bridge_control |= 0x0008;
	    }
/*
** Check if this bridge should block forwarding of ISA I/O addresses
** in the address range defined by the I/O base and limit registers
** that are in the first 64 Kbytes of PCI I/O address space, and 
** if so, set the ISA Enable bit.
*/
	    if ( pbm->flags & PCI_FLAGS_ISA ) {
	    	bridge_control |= 0x0004;
	    }
	    outcfgw( pb, 0x3E, bridge_control );
/*
** Chip Control Initialization for DEC bridges ONLY.
**
** Set up the request timer mask field to avoid deadlocking when
** a device attempts to send data through the bridge, and the bridge
** is currently busy.  This is only needed on the older 2.0 bridges.
** This configuration space write will fall into the bit bucket on
** newer DEC bridges.
*/
	    if ( dec_pci_pci_bridge( pb ) ) {
	    	if ( parent_pb ) {
		    if ( dec_pci_pci_bridge( parent_pb ) ) {
/*
** Request mask timer = 16 PCI clocks
*/
		    	outcfgb( pb, 0x40, incfgb( parent_pb, 0x40 ) ^ 0x04 );
		    }
		    else {
/*
** Request mask timer = 32 PCI clocks
*/
			outcfgb( pb, 0x40, 0x08 );
		    }
		}
		else {
/*
** Request mask timer = 64 PCI clocks
*/
		    outcfgb( pb, 0x40, 0x0C );
		}
	    }
/*
** 0x0C - Cache Line Size
** 0x0D - Latency Timer
*/
	    outcfgw( pb, CACHE_L_SIZ, 0xFF08 );
/*
** 0x04 - Command
*/
	    outcfgw( pb, COM_STAT, command );
	    break;
    }
}

int pci_handler( struct FILE *fp, struct MCHK$LOGOUT *frame )
{
    machine_handle_nxm( frame );
    fp->status = msg_exdep_badadr;
    return( msg_success );
}
