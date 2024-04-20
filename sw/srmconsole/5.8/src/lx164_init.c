/* file:	lx164_init.c
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
 *	Platform specific initialization routines.
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
 *	ER  27-Mar-1998	    Add init_serial_port routine.
 *	ER  06-Nov-1997	    Remove link_en and mwen_en from PYXIS_CTRL1 init.
 *	ER  18-Feb-1997	    Remove references to io_device_tbl.
 *	ER  11-Dec-1996	    Disabled prefetching on PCI memory read, read
 *			    line, and read multiple commands in PYXIS_CTRL
 *			    register.  This will be fixed in the next pass
 *			    of Pyxis.
 *--
 */                                                

/* $INCLUDE_OPTIONS$ */
/* $INCLUDE_OPTIONS_END$ */
#include        "cp$src:kernel_def.h"
#include	"cp$src:pb_def.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:alphascb_def.h"
#include 	"cp$src:ev_def.h"
#include 	"cp$src:alpha_def.h"
#include 	"cp$src:aprdef.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:eisa.h"
#include	"cp$src:tt_def.h"
#include	"cp$src:probe_io_def.h"
#include	"cp$src:combo_def.h"

/* Platform specific definition files */
#include 	"cp$src:platform_cpu.h"
#include	"cp$inc:platform_io.h"
#include	"cp$src:platform.h"
#include	"cp$src:apc.h"
#include 	"cp$src:pyxis_main_csr_def.h"
#include	"cp$src:pyxis_addr_trans_def.h"
#include	"cp$src:impure_def.h"
#include	"cp$src:smc_def.h"
#include	"cp$src:i82378_def.h"

/*
 NOTE !!!!!

  The undefine CC must be done to get a clean build

  The file hwrpb_def.h defines the symbol CC which 
  also gets defined by ev5_defs.sdl

  We are in a catch 22, we can't change the ev5_defs
  without affecting PALcode and we can't change hwrpb_def
  without affecting the console source pool
*/
#undef  CC
#include	"cp$src:ev5_defs.h"

/* external data needed */
extern int do_bpt( void );

extern int manual_config;
extern int keybd_present;
extern int graphics_console;
extern int CTB$MAX_CTBS;
extern int CTB$MAX_SIZE;
extern struct TTPB serial_ttpb;
extern struct TTPB null_ttpb;
extern struct TTPB *console_ttpb;	/* pointer to primary console port */
extern struct HQE mchq_660;			/*MCHECK handler queue*/
extern int HWRPB;
extern struct LOCK spl_kernel;
extern struct SEMAPHORE pbs_lock;
extern unsigned int num_pbs;
extern struct pb **pbs;

/* external function prototypes */
extern int mcheck_handler_620( int vector );
extern int mcheck_handler_630( int vector );
extern int mcheck_handler( int vector );
extern void lx164_unexpected_int( void );
extern void setup_io( void );
extern int spinlock( struct LOCK *spl_kernel );
extern int spinunlock( struct LOCK *spl_kernel );
extern void krn$_reset_toy( void );
extern void io_enable_interrupts( struct pb *pb, unsigned int vector );
extern int read_pci_id( int hose, int bus, int slot, int function );
extern void get_pci_csr_vector( struct pb *pb );
extern void sys_irq_init( void );

int io_bus_reset;

struct set_param_table term_name[] = {
	{"com1",	0},
	{"com2",	1},
	{0}
    };                                             

int CTB_units[] = {
	CTB$K_DZ_WS
	};

struct CTB_VT_table CTB_VT_tab[] = {
	{0, 0, 0, 0, 0}
	};

struct CTB_WS_table CTB_WS_tab[] = {
	{0, 0, 21, 0, 0, CTB$K_NONE}
	};


#define SMC_DEBUG 0

unsigned __int64 SMCConfigState( unsigned __int64 baseAddr )
{
    unsigned char devId;
    unsigned char devRev;

    unsigned __int64 configPort;
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    configPort = indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )configPort + 1 );

    outportb( 0, configPort, CONFIG_ON_KEY );
    outportb( 0, configPort, CONFIG_ON_KEY );
    outportb( 0, indexPort, DEVICE_ID );
    devId = inportb( 0, dataPort );
    if ( devId == VALID_DEVICE_ID ) {
	outportb( 0, indexPort, DEVICE_REV );
	devRev = inportb( 0, dataPort );
    }
    else {
	baseAddr = 0;
    }
    return( baseAddr );
}

void SMCrunState( unsigned __int64 baseAddr )
{
    outportb( 0, baseAddr, CONFIG_OFF_KEY );
}

unsigned __int64 SMCdetectUltraIO( )
{
    unsigned __int64 baseAddr;

    baseAddr = 0x3F0;
    if ( ( baseAddr = SMCConfigState( baseAddr ) ) == 0x3F0 ) {
	return( baseAddr );
    }
    baseAddr = 0x370;
    if ( ( baseAddr = SMCConfigState( baseAddr ) ) == 0x370 ) {
	return( baseAddr );
    }
    return( ( unsigned __int64 )0 );
}

void SMCEnableSER1( unsigned __int64 baseAddr )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, LOGICAL_DEVICE_NUMBER );
    outportb( 0, dataPort, SER1 );

    outportb( 0, indexPort, ADDR_LO );
    outportb( 0, dataPort, ( COM1_BASE & 0xFF ) );

    outportb( 0, indexPort, ADDR_HI );
    outportb( 0, dataPort, ( ( COM1_BASE >> 8 ) & 0xFF ) );

    outportb( 0, indexPort, INTERRUPT_SEL );
    outportb( 0, dataPort, COM1_INTERRUPT );

    outportb( 0, indexPort, ACTIVATE );
    outportb( 0, dataPort, DEVICE_ON );
}

void SMCDisableSER1( unsigned __int64 baseAddr )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, LOGICAL_DEVICE_NUMBER );
    outportb( 0, dataPort, SER1 );

    outportb( 0, indexPort, ACTIVATE );
    outportb( 0, dataPort, DEVICE_OFF );
}

void SMCEnableSER2( unsigned __int64 baseAddr )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, LOGICAL_DEVICE_NUMBER );
    outportb( 0, dataPort, SER2 );

    outportb( 0, indexPort, ADDR_LO );
    outportb( 0, dataPort, ( COM2_BASE & 0xFF ) );

    outportb( 0, indexPort, ADDR_HI );
    outportb( 0, dataPort, ( ( COM2_BASE >> 8 ) & 0xFF ) );

    outportb( 0, indexPort, INTERRUPT_SEL );
    outportb( 0, dataPort, COM2_INTERRUPT );

    outportb( 0, indexPort, ACTIVATE );
    outportb( 0, dataPort, DEVICE_ON );
}

void SMCDisableSER2( unsigned __int64 baseAddr )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, LOGICAL_DEVICE_NUMBER );
    outportb( 0, dataPort, SER2 );

    outportb( 0, indexPort, ACTIVATE );
    outportb( 0, dataPort, DEVICE_OFF );
}

void SMCEnablePARP( unsigned __int64 baseAddr )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, LOGICAL_DEVICE_NUMBER );
    outportb( 0, dataPort, PARP );

    outportb( 0, indexPort, ADDR_LO );
    outportb( 0, dataPort, ( PARP_BASE & 0xFF ) );

    outportb( 0, indexPort, ADDR_HI );
    outportb( 0, dataPort, ( ( PARP_BASE >> 8 ) & 0xFF ) );

    outportb( 0, indexPort, INTERRUPT_SEL );
    outportb( 0, dataPort, PARP_INTERRUPT );

    outportb( 0, indexPort, ACTIVATE );
    outportb( 0, dataPort, DEVICE_ON );
}

void SMCDisablePARP( unsigned __int64 baseAddr )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, LOGICAL_DEVICE_NUMBER );
    outportb( 0, dataPort, PARP );

    outportb( 0, indexPort, ACTIVATE );
    outportb( 0, dataPort, DEVICE_OFF );
}

void SMCEnableIDE( unsigned __int64 baseAddr, unsigned char ide_id )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, LOGICAL_DEVICE_NUMBER );
    outportb( 0, dataPort, ide_id );

    outportb( 0, indexPort, ACTIVATE );
    outportb( 0, dataPort, DEVICE_ON );
}

void SMCDisableIDE( unsigned __int64 baseAddr, unsigned char ide_id )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, LOGICAL_DEVICE_NUMBER );
    outportb( 0, dataPort, ide_id );

    outportb( 0, indexPort, ACTIVATE );
    outportb( 0, dataPort, DEVICE_OFF );
}

void SMCEnableRTCL( unsigned __int64 baseAddr )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, LOGICAL_DEVICE_NUMBER );
    outportb( 0, dataPort, RTCL );

    outportb( 0, indexPort, ACTIVATE );
    outportb( 0, dataPort, DEVICE_ON );
}

void SMCEnableKYBD( unsigned __int64 baseAddr )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, LOGICAL_DEVICE_NUMBER );
    outportb( 0, dataPort, KYBD );

    outportb( 0, indexPort, INTERRUPT_SEL ); /* Primary interrupt select */
    outportb( 0, dataPort, KYBD_INTERRUPT );

    outportb( 0, indexPort, 0x72 );	    /* Secondary interrupt select */
    outportb( 0, dataPort, 0x0C );

    outportb( 0, indexPort, ACTIVATE );
    outportb( 0, dataPort, DEVICE_ON );
}

void SMCDisableKYBD( unsigned __int64 baseAddr )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, LOGICAL_DEVICE_NUMBER );
    outportb( 0, dataPort, KYBD );

    outportb( 0, indexPort, ACTIVATE );
    outportb( 0, dataPort, DEVICE_OFF );
}

void SMCEnableFDC( unsigned __int64 baseAddr )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    unsigned char oldValue;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, LOGICAL_DEVICE_NUMBER );
    outportb( 0, dataPort, FDC );

    outportb( 0, indexPort, FDD_MODE_REGISTER );
    oldValue = inportb( 0, dataPort );

    oldValue |= 0x0E;			/* Enable burst mode */
    outportb( 0, dataPort, oldValue );

    outportb( 0, indexPort, INTERRUPT_SEL ); /* Primary interrupt select */
    outportb( 0, dataPort, 0x06 );

    outportb( 0, indexPort, DMA_CHANNEL_SEL );	/* DMA channel select */
    outportb( 0, dataPort, 0x02 );

    outportb( 0, indexPort, ACTIVATE );
    outportb( 0, dataPort, DEVICE_ON );
}

void SMCDisableFDC( unsigned __int64 baseAddr )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, LOGICAL_DEVICE_NUMBER );
    outportb( 0, dataPort, FDC );

    outportb( 0, indexPort, ACTIVATE );
    outportb( 0, dataPort, DEVICE_OFF );
}

#if SMC_DEBUG
void SMCReportDeviceStatus( unsigned __int64 baseAddr )
{
    unsigned __int64 indexPort;
    unsigned __int64 dataPort;
    
    unsigned char currentControl;
    unsigned char fer;

    indexPort = baseAddr;
    dataPort = ( unsigned __int64 )( ( char * )baseAddr + 1 );

    outportb( 0, indexPort, POWER_CONTROL );
    currentControl = inportb( 0, dataPort );

    if ( currentControl & ( 1 << FDC ) )
	qprintf( "\t+FDC Enabled\n" );
    else
	qprintf( "\t-FDC Disabled\n" );

    if ( currentControl & ( 1 << IDE1 ) )
	qprintf( "\t+IDE1 Enabled\n" );
    else
	qprintf( "\t-IDE1 Disabled\n" );

    if ( currentControl & ( 1 << IDE2 ) )
	qprintf( "\t+IDE2 Enabled\n" );
    else
	qprintf( "\t-IDE2 Disabled\n" );

    if ( currentControl & ( 1 << PARP ) )
	qprintf( "\t+PARP Enabled\n" );
    else
	qprintf( "\t-PARP Disabled\n" );

    if ( currentControl & ( 1 << SER1 ) )
	qprintf( "\t+SER1 Enabled\n" );
    else
	qprintf( "\t-SER1 Disabled\n" );

    if ( currentControl & ( 1 << SER2 ) )
	qprintf( "\t+SER2 Enabled\n" );
    else
	qprintf( "\t-SER2 Disabled\n" );

    qprintf( "\n" );
}
#endif

int SMCinit( )
{
    unsigned __int64 SMCUltraBase;

    if ( ( SMCUltraBase = SMCdetectUltraIO( ) ) != ( unsigned __int64 )0 ) {
#if SMC_DEBUG
	qprintf( "SMC FDC37C93X Ultra I/O Controller found @ %x\n", SMCUltraBase );
	SMCReportDeviceStatus( SMCUltraBase );
#endif
	SMCEnableSER1( SMCUltraBase );
	SMCEnableSER2( SMCUltraBase );
	SMCEnablePARP( SMCUltraBase );
	SMCEnableKYBD( SMCUltraBase );
	SMCEnableFDC( SMCUltraBase );
#if SMC_DEBUG
	SMCReportDeviceStatus( SMCUltraBase );
#endif
	SMCrunState( SMCUltraBase );
	return( msg_success );
    }
    else {
#if SMC_DEBUG
	qprintf( "No SMC FDC37C93X Ultra I/O Controller found\n" );
#endif
	return( msg_failure );
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
 *	init_serial_port( pb, port, baud ); 
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
void init_serial_port( struct pb *pb, int port, int baud )
{
    char temp;
/*
** Access divisor latches
*/
    outportb( pb, port + LCR, LCR$M_DLA );
/*
** Set the line speed
*/
    if ( baud == 0 ) {
    	baud = BAUD_9600;
    }
    outportb( pb, port + DLL, baud >> 0 );
    outportb( pb, port + DLM, baud >> 8 );
/*
** Set line control to 8 bit, 1 stop bit, no parity
*/
    outportb( pb, port + LCR, CHAR_8BIT | LCR$M_SBS );
/*
** Set modem control to DTR, RTS active, OUT1 low, OUT2 enabled
*/
    outportb( pb, port + MCR, MCR$M_DTR | MCR$M_RTS | MCR$M_OUT1 | MCR$M_OUT2 ); 
/*
**  Turn FIFOs on
*/
    outportb( pb, port + FCR, FCR$M_FE | FCR$M_RFR | FCR$M_TFR );
/*
**  Flush the receive buffer
*/
    while ( inportb( pb, port + LSR ) & LSR$M_DR ) 
	temp = inportb( pb, port + RBR );                                                          
}


void pb_reinit( struct pb *pb )
{
    switch ( pb-> type ) {
        case TYPE_PCI:
            read_pci_id( pb->hose, pb->bus, pb->slot, pb->function );
            get_pci_csr_vector( pb );
            break;
    }
}


/*+
 * ============================================================================
 * = platform_init1 - First platform specific initialization                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Perform console initialization that is required by the platform.
 *	This routine should do any specific initialization for setting
 *	up the console data structures.
 *
 *	This routine is called as the first thing in KRN$_IDLE.
 *
 * FORM OF CALL:
 *
 *	platform_init1( ); 
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
void platform_init1( void )
{
    setup_io( );
}


/*+
 * ============================================================================
 * = platform_init2 - Second platform specific initialization                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Perform console initialization that is required by the platform.
 *	This routine should do any specific initialization for setting
 *	up the console data structures.
 *
 *	This routine is called in KRN$_IDLE after some memory/variable 
 *      initialization has occured. 
 *
 * FORM OF CALL:
 *
 *	platform_init2( ); 
 *
 * RETURNS:
 *
 *      Nothing
 *                          
 * ARGUMENTS:
 *
 *      None
-*/
void platform_init2( void ) {

    struct pb SIO_pb;

/*
** Force automatic configuration and tell the BIOS emulator that the
** PCI bus is configured.  This prevents the BIOS from re-configuring
** the device.
*/
    manual_config = 0;
/*
** Initialize the serial port(s) ...
*/
    SIO_pb.hose = 0;
    SIO_pb.bus = 0;
    SIO_pb.slot = 8;
    SIO_pb.function = 0;
    init_serial_port( &SIO_pb, COM1, 0 );
    init_serial_port( &SIO_pb, COM2, 0 );
/*
** Initialize Console Terminal Block (CTB) table
*/
    CTB_units[0] = CTB$K_DZ_WS;
    CTB_WS_tab[0].TERM_OUT_LOC = ISA_BUS << 16;
/* 
** Set graphics_console global variable according to TOY nvram value
** to reflect graphics or serial console mode. 
*/
    ev_console_init( 0, 0 );	

    keybd_present = kbd_attatched( );   /* probe the controller */
/*
** If we know we'll be using the serial port as primary console then
** setup for using those I/O routines now so we can display ASAP.
** Otherwise use null I/O routines until a graphics driver instantiates
** itself as the primary console and initializes its hardware.
*/
    CTB$MAX_CTBS = CTB$MAX_UNITS;
    CTB$MAX_SIZE = sizeof( struct CTB_WS );
    if ( graphics_console && keybd_present ) {
	console_ttpb = &null_ttpb;
	CTB_WS_tab[0].TYPE = CTB$K_DZ_WS;
    }
    else {
	console_ttpb = &serial_ttpb;
	CTB_WS_tab[0].TYPE = CTB$K_DZ_VT;
    }
}


/*+
 * ============================================================================
 * = initialize_hardware - initialize hardware                                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Perform platform specific hardware initialization.
 *
 *	This routine is called in KRN$_IDLE after the dynamic memory data  
 *      structure initialization has occured. The routine pci_size_config.c
 *      uses malloc and free so this initialization needs to occur 
 *
 * FORM OF CALL:
 *
 *	initialize_hardware( ); 
 *
 * RETURNS:
 *
 *      Nothing
 *                          
 * ARGUMENTS:
 *
 *      None
 *
-*/
void initialize_hardware( void )
{
/*
**  Setup interrupt handlers for the system ....
*/
    int_vector_set( SCB$Q_SLE, ( void (*)( ) )do_bpt, 0, 0 );             
    int_vector_set( SCB$Q_SYS_CORR_ERR, mcheck_handler_620, ( void * )SCB$Q_SYS_CORR_ERR, 0 );
    int_vector_set( SCB$Q_PR_CORR_ERR, mcheck_handler_630, ( void * )SCB$Q_PR_CORR_ERR, 0 );
    int_vector_set( SCB$Q_SYS_MCHK, mcheck_handler, ( void * )SCB$Q_SYS_MCHK, 0 );
    int_vector_set( SCB$Q_PR_MCHK, mcheck_handler, ( void * )SCB$Q_PR_MCHK, 0 );
    int_vector_set( SCB$Q_6F0, lx164_unexpected_int, 0, 0 );
    mchq_660.flink = &mchq_660;
    mchq_660.blink = &mchq_660;
}

/*+
 * ============================================================================
 * = reset_hardware - reset hardware					      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Perform a hardware reset
 *
 * FORM OF CALL:
 *
 *	reset_hardware( log ); 
 *
 * RETURNS:
 *
 *      Nothing
 *                          
 * ARGUMENTS:
 *
 *      int log - ???
 *
-*/
void reset_hardware( int log ) 
{
    struct HWRPB *hwrpb;
    struct SLOT *slot;
    unsigned int i;
    unsigned __int64 BaseCSR = pyxis_a_addr_trans;
    union PYXIS_CTRL pyxis_csr;
    struct pb pb;
    int temp;

    hwrpb = HWRPB;
    slot = ( int )hwrpb + *hwrpb->SLOT_OFFSET;
    if ( !slot->STATE.SLOT$V_BIP 
	    || ( READ_IO_CSR( CSR_W0_BASE ) & 1 ) 
	    || ( READ_IO_CSR( CSR_W2_BASE ) & 1 ) )
    {
    	spinlock( &spl_kernel );
/*
** Reset the I/O busses ...
*/
	io_bus_reset = 1;
	if ( log ) {
	    pprintf( "Resetting I/O buses...\n" );
	}
/*
** Disable PCI Target Windows
*/
	WRITE_IO_CSR( CSR_W0_BASE, 0 );
	WRITE_IO_CSR( CSR_W1_BASE, 0 );
	WRITE_IO_CSR( CSR_W2_BASE, 0 );
	WRITE_IO_CSR( CSR_W3_BASE, 0 );
	mb( );
/*
** Wait for all transactions to complete
*/
	krn$_micro_delay( 50000 );
/*
** Reset the PCI bus...
*/
	BaseCSR = pyxis_k_main_csr_base;

	temp = pyxis_csr.pyxis_ctrl_l_whole = ( int )READ_IO_CSR( CSR_PYXIS_CTRL );

	pyxis_csr.pyxis_ctrl_r_field.pyxis_ctrl_v_pci_req64_en = ~pyxis_csr.pyxis_ctrl_r_field.pyxis_ctrl_v_pci_req64_en;
	pyxis_csr.pyxis_ctrl_r_field.pyxis_ctrl_v_pci_en = 0;
	WRITE_IO_CSR( CSR_PYXIS_CTRL, pyxis_csr.pyxis_ctrl_l_whole );
	mb( );

	krn$_micro_delay( 10000 );

	WRITE_IO_CSR( CSR_PYXIS_CTRL, temp );
	mb( );

	krn$_micro_delay( 50000 );

	setup_io( );
/*
** Initialize the serial port(s) ...
*/
	pb.hose = 0;
	pb.bus = 0;
	pb.slot = 8;
	pb.function = 0;

	init_serial_port( &pb, COM1, 0 );
	init_serial_port( &pb, COM2, 0 );

	sys_irq_init( );

	spinunlock( &spl_kernel );

#if ((IPL_RUN) && (IPL_RUN > 19))
        io_enable_interrupts( &pb, 0x86 );
        io_enable_interrupts( &pb, COM2_VECTOR );
        io_enable_interrupts( &pb, COM1_VECTOR );
#endif

        krn$_reset_toy( );

        krn$_wait( &pbs_lock );
        for ( i = 0; i < num_pbs; i++ )
            pb_reinit( pbs[i] );
        krn$_post( &pbs_lock );

#if TGA
	tga_reinit( 0 ); /* Reinit the TGA but do not clear the screen */
#endif
	vga_reinit( );

	kbd_reinit( );
    }
}
