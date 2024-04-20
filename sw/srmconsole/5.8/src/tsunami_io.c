/*
 * Copyright (C) 1997 by
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Tsunami I/O functions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	gdm	04-Feb-1999	Conditionalise for Eiger. Since we do not have
 *				a Pchip1 we must avoid all writes there. Added
 *				to webbrick conditions.
 *
 *	dtr	24-Nov-1997	Added function call to determine if ppb is a 
 *				primary bridge on a system that doens't get
 *				routed - fixed.
 *
 *	er	30-Sep-1997	Added io_get_window_base_rt routine.
 *	er	24-Sep-1997	Moved PCI read/write driver functions to
 *				pci_driver.c.
 *
 *	hcb	22-Sep-1997	Initial entry.
 */

#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:pb_def.h"
#include	"cp$src:probe_io_def.h"
#include	"cp$src:common.h"
#include	"cp$src:msg_def.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:platform_cpu.h"
#include	"cp$inc:platform_io.h"
#include	"cp$src:tsunami.h"

#define DEBUG 0

unsigned int max_hose;

#if GALAXY
extern int galaxy_partitions;
extern int galaxy_io_mask;
#endif

extern struct HQE mchq_660;
extern unsigned char ETAPciMaxBusNumber;
extern int pci_bus;
extern int manual_config;

extern unsigned byte ldb( );
extern unsigned word ldw( );
extern unsigned long ldl( );
extern unsigned quad ldq( );
extern void stb( );
extern void stw( );
extern void stl( );
extern void stq( );

unsigned long xportx( struct pb *pb, unsigned quad offset, unsigned long data,
	unsigned long (*ld_or_st)( ) )
    {
	UINT o;
    UINT p;

    if( pb && ( pb->hose > max_hose ) )
	return( -1 );

    o = 32*1024*1024 - 1;
    p = PCI0_IO_BASE;
    if( pb )
	p += pb->hose * 0x200; /* 8 GB per hose */
    p = ( p << 24 ) | ( offset & o );
    data = ld_or_st( p, data );
    return( data );
    }

unsigned byte inportb( struct pb *pb, unsigned quad offset )
    {
    return( xportx( pb, offset, 0, ldb ) );
    }

unsigned word inportw( struct pb *pb, unsigned quad offset )
    {
    return( xportx( pb, offset, 0, ldw ) );
    }

unsigned long inportl( struct pb *pb, unsigned quad offset )
    {
    return( xportx( pb, offset, 0, ldl ) );
    }

void outportb( struct pb *pb, unsigned quad offset, unsigned byte data )
    {
    xportx( pb, offset, data, stb );
    }

void outportw( struct pb *pb, unsigned quad offset, unsigned word data )
    {
    xportx( pb, offset, data, stw );
    }

void outportl( struct pb *pb, unsigned quad offset, unsigned long data )
    {
    xportx( pb, offset, data, stl );
    }

unsigned quad xmemx( struct pb *pb, unsigned quad offset, unsigned quad data,
	unsigned quad (*ld_or_st)( ) )
    {
    data = ld_or_st( xdmem( pb, offset ), data );
    return( data );
    }

unsigned quad xdmem( struct pb *pb, unsigned quad offset )
    {
    UINT o;
    UINT p;

    if( pb && ( pb->hose > max_hose ) )
	return( -1 );
    o = (UINT)4*1024*1024*1024 - 1;
    p = PCI0_MEM_BASE;
    if( pb )
	p += pb->hose * 0x200; /* 8 GB per hose */
    p = ( p << 24 ) | ( offset & o );
    return( p );
    }

unsigned byte inmemb( struct pb *pb, unsigned quad offset )
    {
    return( xmemx( pb, offset, 0, ldb ) );
    }

unsigned word inmemw( struct pb *pb, unsigned quad offset )
    {
    return( xmemx( pb, offset, 0, ldw ) );
    }

unsigned long inmeml( struct pb *pb, unsigned quad offset )
    {
    return( xmemx( pb, offset, 0, ldl ) );
    }

unsigned quad inmemq( struct pb *pb, unsigned quad offset )
    {
    return( xmemx( pb, offset, 0, ldq ) );
    }

void outmemb( struct pb *pb, unsigned quad offset, unsigned byte data )
    {
    xmemx( pb, offset, data, stb );
    }

void outmemw( struct pb *pb, unsigned quad offset, unsigned word data )
    {
    xmemx( pb, offset, data, stw );
    }

void outmeml( struct pb *pb, unsigned quad offset, unsigned long data )
    {
    xmemx( pb, offset, data, stl );
    }

void outmemq( struct pb *pb, unsigned quad offset, unsigned quad data )
    {
    xmemx( pb, offset, data, stq );
    }

unsigned long indmeml( struct pb *pb, unsigned quad offset )
    {
    return( inmeml( pb, offset ) );
    }

unsigned quad indmemq( struct pb *pb, unsigned quad offset )
    {
    return( inmemq( pb, offset ) );
    }

void outdmeml( struct pb *pb, unsigned quad offset, unsigned long data )
    {
    outmeml( pb, offset, data );
    }

void outdmemq( struct pb *pb, unsigned quad offset, unsigned quad data )
    {
    outmemq( pb, offset, data );
    }

unsigned long xcfgx( struct pb *pb, unsigned quad offset, unsigned long data,
	unsigned long (*ld_or_st)( ) )
    {
    int bus;
    int slot;
    int function;
    UINT o;
    UINT p;
    UINT q;

    if( pb->hose > max_hose )
	return( -1 );
    o = 256 - 1;
    p = PCI0_CONFIG_BASE;
    p += pb->hose * 0x200; /* 8 GB per hose */
    bus = pb->bus;
    slot = pb->slot;
    function = pb->function;
    if( ( slot > ( bus ? 31 : PCI_MAX_SLOT ) ) || ( function > 7 ) )
	{
	data = 0xffffffff;
	}
    else
	{
	q = ( bus << 16 ) + ( slot << 11 ) + ( function << 8 ) + ( offset & o );
	p = ( p << 24 ) | q;
	data = ld_or_st( p, data );
	}
    return( data );
    }

unsigned byte incfgb( struct pb *pb, unsigned quad offset )
    {
    return( xcfgx( pb, offset, 0, ldb ) );
    }

unsigned word incfgw( struct pb *pb, unsigned quad offset )
    {
    return( xcfgx( pb, offset, 0, ldw ) );
    }

unsigned long incfgl( struct pb *pb, unsigned quad offset )
    {
    return( xcfgx( pb, offset, 0, ldl ) );
    }

void outcfgb( struct pb *pb, unsigned quad offset, unsigned byte data )
    {
    xcfgx( pb, offset, data, stb );
    }

void outcfgw( struct pb *pb, unsigned quad offset, unsigned word data )
    {
    xcfgx( pb, offset, data, stw );
    }

void outcfgl( struct pb *pb, unsigned quad offset, unsigned long data )
    {
    xcfgx( pb, offset, data, stl );
    }

unsigned byte xtig( struct pb *pb, unsigned quad offset, unsigned byte data,
	unsigned quad (*ld_or_st)( ) )
    {                                         
    UINT o;
    UINT p;
    UINT addr;                                            
                                          
    o = 16*1024*1024 - 1;
    p = TIG_BASE;
    p = ( p << 24 ) | ( ( offset & o ) << 6 );
    data = ld_or_st( p, data );
#if DEBUG
    pprintf( "Tsunami TIG bus address %x, data %x\n", p, data );
#endif
    return( data );
    }                                                                          
                                                                           
unsigned byte intig( struct pb *pb, unsigned quad offset )
    {
    return( xtig( pb, offset, 0, ldq ) );
    }

void outtig( struct pb *pb, unsigned quad offset, unsigned byte data )
    {
    xtig( pb, offset, data, stq );
    }

#if CAUSE_ABORT

extern int null_procedure( );
/*
** CauseMasterAbort is used  only for a Pass 1 tsunami bug. The pci retry counter
** wasn't getting reset after normal transactions. A master abort every so often
** causes the counter to reset.
*/ 
void CauseMasterAbort(void)
{                    
  struct pb pb = { 0 };
                      
  while (1)           
  {                    
      /* hose 0 */    
    pb.hose = 0;    
    outmeml(&pb, 0x40000000-4, 0);	/* Cause a master abort */
#if CLEAR_ABORT
    WriteTsunamiCSR(PCHIP0_PERROR, ReadTsunamiCSR(PCHIP0_PERROR));
#endif                                                                
      /* hose 1 */                                              
    pb.hose = 1;                                              
    outmeml(&pb, 0x40000000-4, 0);	/* Cause a master abort */
#if CLEAR_ABORT
    WriteTsunamiCSR(PCHIP1_PERROR, ReadTsunamiCSR(PCHIP1_PERROR));
#endif                
    krn$_sleep(1000);	/* pause for 1 second */                  
  }      
} 
#endif

void tsunami_probe_local( )
    {
    struct pb *pb;

    max_hose = io_get_max_hose( );

#if CAUSE_ABORT
/*
 * Master abort process. This is a fix for the tsunami p1 bug that doesn't 
 * reset the pci timeout after normal transactions. The master abort every
 * now and then causes the retry counter to be reset.
 */                        
    krn$_create(CauseMasterAbort,	/* address of process           */
      null_procedure, 	   	 	/* startup routine              */
      0, 	     	   	 	/* completion semaphore         */
      7, 	     	   	 	/* process priority             */
      -1, 	     	   	 	/* cpu affinity mask            */
      0,   	     	   		/* stack size                   */
      "RetryReset", 	   		/* process name                 */
      "nl", "r", "nl", "w", "nl", "w");
#endif
    }

void probe_pci_setup( int hose )
    {
    unsigned int base;
    unsigned int size;

    /*	setup the PCI IO space
     *			start Address		size
     *	EISA legacy 	00001000(4KB)		64KB - 4KB = 60KB	
     *	PCI devices	00010000(64KB)		32MB-64KB
     *	PCI uncacheable	01000000(16MB)  	512MB - 16MB = 496MB
     *	PCI cacheable	40000000(1GB)		1GB
     */
#ifdef EISA_MAX_SLOT
    base = ( EISA_MAX_SLOT + 1 ) * 0x1000;
    size = 0x10000 - ( EISA_MAX_SLOT + 1 ) * 0x1000;
#else
    base = 0x1000;
    size = 0x10000 - 0x1000;
#endif
    pci_add_region( base, size, SIZE_GRAN_BASE_IO_16_BIT );
    base = 0x10000;
    size = 0x1ff0000;
    pci_add_region( base, size, SIZE_GRAN_BASE_IO );
    base = 0x1000000;
    size = 0x1f000000;
    pci_add_region( base, size, SIZE_GRAN_BASE_SPARSE );
    base = 0x40000000;
    size = 0x40000000;
    pci_add_region( base, size, SIZE_GRAN_BASE_DENSE );
    }

void probe_pci_cleanup( int hose )
    {
    if( hose == 0 )
	{
	manual_config = 0;
	ETAPciMaxBusNumber = pci_bus;
	}
    }

int read_pci_id_handler( unsigned int *flag, void *frame )
    {
    machine_handle_nxm( frame );
    clear_cpu_errors( whoami( ) );
    *flag = 1;
    return( msg_success );
    }

int read_pci_id( int hose, int bus, int slot, int function )
    {
    int flag;
    unsigned int id;
    struct HQE hqe;
    struct pb pb;

    pb.hose = hose;
    pb.bus = bus;
    pb.slot = slot;
    pb.function = function;
    flag = 0;
    hqe.handler = read_pci_id_handler;
    hqe.param = &flag;
    insq_lock( &hqe, mchq_660.blink );

    id = incfgl( &pb, 0x00 );
    
    remq_lock( &hqe );
    if( flag || ( id == (u_int)0xffffffff ) )
	id = 0;
    return( id );
    }

int get_hose_type( int hose )
    {
    if( hose <= max_hose )
	{
#if GALAXY
	if( galaxy_partitions )
	    if( !( ( galaxy_io_mask >> hose ) & 1 ) )
		return( TYPE_UNKNOWN );
#endif
	return( TYPE_PCI );
	}
    else
	return( TYPE_UNKNOWN );
    }

void get_pci_csr_vector( struct pb *pb )
    {
    int i;
    int header;
    int bridge_control;
    int command;
    int hose;
    int bus;
    int slot;
    int function;
    int base_slot;
    int int_pin;
    int base_int_pin;
    int int_line;
    int cache_line_size;
    unsigned int base;
    unsigned int size;
    struct pb *parent_pb;
    struct pci_device_misc *pdm;
    struct pci_bridge_misc *pbm;

    hose = pb->hose;
    bus = pb->bus;
    slot = pb->slot;
    function = pb->function;
    parent_pb = pb->parent;
    pdm = pb->pdm;
    pbm = pdm->pbm;
    header = incfgb( pb, 0x0e ) & 0x7f;
    if( parent_pb )
	{
	struct pci_device_misc *pdm;
	struct pci_bridge_misc *pbm;

	pdm = parent_pb->pdm;
	pbm = pdm->pbm;
	base_int_pin = pbm->base_int_pin;
	}
    else
	base_int_pin = 0;
    base_slot = pdm->base_slot;
    command = 0x0007;
    if( pdm->flags & PCI_FLAGS_NOT_ASSIGNED_IO )
	command &= ~0x0001;
    if( pdm->flags & PCI_FLAGS_NOT_ASSIGNED_MEM )
	command &= ~0x0002;
    cache_line_size = 64/4;
    switch( header )
	{
	case 0:
	    pb->csr = pdm->base[1];
	    int_pin = incfgb( pb, 0x3d );
	    if( int_pin )
		{
		if( base_int_pin )
		    int_pin = ( ( slot + base_int_pin - 1 + int_pin - 1 ) & 3 ) + 1;
		int_line = assign_pci_vector( pb, int_pin, base_int_pin, base_slot );
		}
	    else
		int_line = -1;
	    for( i = 0; i < 6; i++ )
		outcfgl( pb, 0x10 + i * 4, pdm->base[i] );
	    outcfgl( pb, 0x30, pdm->base[6] );
	    if( int_line < 0 )
		outcfgb( pb, 0x3c, 0 );
	    else
		outcfgb( pb, 0x3c, int_line );
	    /*
	     *  Hack for VGA 1280/P:  clear the QuickBlank bit.
	     */
	    if( incfgl( pb, 0x00 ) == 0x30320e11 )
		outcfgb( pb, 0x44, 0x01 );
	    /*
	     *  Hack for VIP:  disable write posting and retry holdoff in
	     *  PCI-to-PCI bridge above.
	     */
	    if( incfgl( pb, 0x00 ) == 0x00101011 )
		if( parent_pb )
		    if( dec_pci_pci_bridge( parent_pb ) )
			outcfgb( parent_pb, 0x40, 0x02 );
	    /*
	     *  Hack for OPPO:  disable retry holdoff in PCI-to-PCI bridge
	     *  above, but only if this OPPO is on the main PCI bus (i.e.,
	     *  is not already bridged).
	     */
	    if( incfgl( pb, 0x00 ) == 0x00161011 )
		if( parent_pb )
		    if( dec_pci_pci_bridge( parent_pb ) )
			if( !parent_pb->parent )
			    outcfgb( parent_pb, 0x40, 0x00 );

	    /*
	     *  Hack for Cypress USB:  set cache line size to 32 bytes.
	     */
	    if( ( incfgl( pb, 0x00 ) == (u_int)0xc6931080 ) && ( function == 3 ) )
		cache_line_size = 32/4;

	    /*
	     *  Hack for Acer Labs USB:  set cache line size to 32 bytes.
	     */
	    if( incfgl( pb, 0x00 ) == 0x523710b9 )
		cache_line_size = 32/4;

	    /*
	     *  Hack for Acer Labs IDE:  Turn back on access to the BARs
	     */
	    if( incfgl( pb, 0x00 ) == 0x522910b9 )
		outcfgb( pb, 0x53, 0x00 );

	    /*
	     * For DPT's Domino, set the second base register to 
	     * a base of 4GB. Set the bridge window up to map 512MB
	     * with a base of 4GB.
	     */

	    if( incfgl( pb, 0x00 ) == 0x10121044 ) {
		outcfgl( pb, 0x14, 0 );
		outcfgl( pb, 0x18, 1 );
		if( parent_pb ) {
		    outcfgw( parent_pb, 0x24, 0x00 );
		    outcfgw( parent_pb, 0x26, ( 0x20000000 - 1 ) >> 16 );
		    outcfgl( parent_pb, 0x28, 0x1 );
		    outcfgl( parent_pb, 0x2c, 0x1 );
		}
	    }

#if WFSTDIO
	    /*
	     *  Hack for WildFire Standard I/O module (behind a PPB).
	     */
	    if( incfgl( pb, 0x00 ) == 0x153310b9 )
		if( parent_pb )
		    {
		    outcfgb( parent_pb, 0x1c, 0x00 );
		    outcfgw( parent_pb, 0x20, 0x0010 );
		    outcfgw( parent_pb, 0x30, 0x0000 );
		    outcfgw( parent_pb, 0x3e, 0x0000 );
		    setup_wf_stdio( pb, 1 );
		    }
#endif

	    outcfgw( pb, 0x0c, 0xff00 | cache_line_size );
	    outcfgw( pb, 0x04, command );
	    break;

	case 1:
	    if( base_int_pin )
		pbm->base_int_pin = ( ( slot + base_int_pin - 1 ) & 3 ) + 1;
	    else if( pci_check_primary( hose, bus, slot ) )
		pbm->base_int_pin = 0;
	    else
		pbm->base_int_pin = 1;
	    outcfgl( pb, 0x10, pdm->base[0] );
	    outcfgl( pb, 0x14, pdm->base[1] );
	    outcfgl( pb, 0x38, pdm->base[2] );
	    outcfgb( pb, 0x18, pbm->pri_bus );
	    outcfgb( pb, 0x19, pbm->sec_bus );
	    outcfgb( pb, 0x1a, pbm->sub_bus );
	    outcfgb( pb, 0x1b, 0xff );
	    outcfgw( pb, 0x1e, 0xffff );
	    if( pbm->flags & PCI_FLAGS_IO_16_BIT )
		{
		base = pbm->base[SIZE_GRAN_BASE_IO_16_BIT];
		size = pbm->size[SIZE_GRAN_BASE_IO_16_BIT];
		}
	    else
		{
		base = pbm->base[SIZE_GRAN_BASE_IO];
		size = pbm->size[SIZE_GRAN_BASE_IO];
		}
	    if( size == 0 )
		{
		base = -1;
		size = 2;
		}
	    outcfgb( pb, 0x1c, base >> 8 );
	    outcfgb( pb, 0x1d, ( base + size - 1 ) >> 8 );
	    outcfgw( pb, 0x30, base >> 16 );
	    outcfgw( pb, 0x32, ( base + size - 1 ) >> 16 );
	    base = pbm->base[SIZE_GRAN_BASE_SPARSE];
	    size = pbm->size[SIZE_GRAN_BASE_SPARSE];
	    if( size == 0 )
		{
		base = -1;
		size = 2;
		}
	    outcfgw( pb, 0x20, base >> 16 );
	    outcfgw( pb, 0x22, ( base + size - 1 ) >> 16 );
	    base = pbm->base[SIZE_GRAN_BASE_DENSE];
	    size = pbm->size[SIZE_GRAN_BASE_DENSE];
	    if( size == 0 )
		{
		base = -1;
		size = 2;
		}
	    outcfgw( pb, 0x24, base >> 16 );
	    outcfgw( pb, 0x26, ( base + size - 1 ) >> 16 );
	    bridge_control = 0x0003;
	    if( pbm->flags & PCI_FLAGS_VGA )
		bridge_control |= 0x0008;
	    if( pbm->flags & PCI_FLAGS_ISA )
		bridge_control |= 0x0004;
	    outcfgw( pb, 0x3e, bridge_control );
	    if( dec_pci_pci_bridge( pb ) )
		if( parent_pb )
		    if( dec_pci_pci_bridge( parent_pb ) )
			outcfgb( pb, 0x40, incfgb( parent_pb, 0x40 ) ^ 0x04 );
		    else
			outcfgb( pb, 0x40, 0x08 );
		else
		    outcfgb( pb, 0x40, 0x0c );
	    outcfgw( pb, 0x0c, 0xff00 | cache_line_size );
	    outcfgw( pb, 0x04, command );
	    break;
	}
    }

int pci_handler( struct FILE *fp, struct MCHK$LOGOUT *frame )
    {
    machine_handle_nxm( frame );
    fp->status = msg_exdep_badadr;
    return( msg_success );
    }


#if MONET
#define parbits	(perror_m_rdpe | perror_m_perr | perror_m_ape)
#else
#define parbits	perror_m_rdpe
#endif

void set_pci_parity( UINT parity )
    {
    UINT temp;

    temp = ReadTsunamiCSR( PCHIP0_PERRMASK );

    if( parity )
	temp |= parbits;
    else
	temp &= ~parbits;

    WriteTsunamiCSR( PCHIP0_PERRMASK, temp );
#if WEBBRICK || EIGER
    /* Do not try to write to Pchip1 for us */
#else
    temp = ReadTsunamiCSR( PCHIP1_PERRMASK );

    if( parity )
	temp |= parbits;
    else
	temp &= ~parbits;

    WriteTsunamiCSR( PCHIP1_PERRMASK, temp );
#endif
    }

/*
** ============================================================================
** = ReadTsunamiCSR - Tsunami CSR read routine.				      =
** ============================================================================
**
** OVERVIEW:
**
**      This routine is used to read the Tsunami CSRs. Its primary 
**   	function is to hide all the translations needed to read the CSRs.
**
**   	The CSR's are quadword, so the data returned is also a quadword.
**
** FORM OF CALL:
**
**   	data = ReadTsunamiCSR (offset);
**                           
** RETURNS:
**
**	quadword containing the data read
**
** ARGUMENTS:
**
**	offset - register to read from.
**
** SIDE EFFECTS:
**
**	None
**
*/
unsigned __int64 ReadTsunamiCSR( unsigned __int64 offset )
    {
    return ldq(offset);
    }

uint64 read_io_csr( uint64 addr )
    {
    return( ldq( addr ) );
    }

/*
** ============================================================================
** = WriteTsunamiCSR - Tsunami CSR write routine.  			      =
** ============================================================================
**
** OVERVIEW:
**
**      This routine is used to write the Tsunami CSRs. Its primary 
**   	function is to hide all the translations needed to write the CSRs.
**
** FORM OF CALL:
**
**   	WriteTsunamiCSR (offset, data);
**
** ARGUMENTS:
**
**   	offset - register to read from.
**   	data   - the data to write to a CSR.
**
** SIDE EFFECTS:
**
**   	None
**
*/
void WriteTsunamiCSR( unsigned __int64 offset, unsigned __int64 data )
    {
    stq(offset, data);
    }

void write_io_csr( uint64 addr, uint64 data )
    {
    stq( addr, data );
    }

/*+                      
 * ============================================================================
 * = io_get_window_base_rt - return the current value of PCI window base      =
 * ============================================================================
 *   
 * OVERVIEW:
 *   
 *   	This routine will return the current value of the PCI base
 *   	address for PCI target window 1.
 *   
 * FORM OF CALL:
 *           
 *   	io_get_window_base_rt( pb );
 *                    
 * RETURNS:  
 *           
 *   	unsigned int - current PCI target window base address
 *           
 * ARGUMENTS:
 *           
 *   	struct pb *pb - pointer to port block data structure.
 *   	     		This argument, although required, is only here 
 *   	     		for compatibility with code that may really need it.
 *           
 * SIDE EFFECTS:
 *           
 *      None 
 *           
-*/          
unsigned int io_get_window_base_rt( struct pb *pb )
{                      
#if WEBBRICK || EIGER
  return( ReadTsunamiCSR( PCHIP0_WSBA1 ) & 0xFFF00000 );
#else
  return( ReadTsunamiCSR( !pb || ( pb->hose == 0 ) ? PCHIP0_WSBA1 : PCHIP1_WSBA1 ) & 0xFFF00000 );
#endif
}                                                      

