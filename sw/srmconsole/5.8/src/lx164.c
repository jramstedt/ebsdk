/* file:	lx164.c
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
 */                  
                     
/*                   
 *++                 
 *  FACILITY:        
 *                                                    
 *      Alpha SRM Console Firmware
 *                                                                   
 *  MODULE DESCRIPTION:                                              
 *                                                                   
 *	Platform specific miscelleneous routines.
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
 *	ER	18-Feb-1997	Move PCI interrupt routines from pci_size_config_lx164.c
 *				to here.  Now using PROBE_IO interface.
 *
 *--
 */                                                

/* $INCLUDE_OPTIONS$ */
#include	"cp$inc:platform_io.h"
/* $INCLUDE_OPTIONS_END$ */
#include	"cp$src:platform.h"	
#include        "cp$src:kernel_def.h"
#include	"cp$src:pb_def.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:alphascb_def.h"
#include	"cp$src:dynamic_def.h"
#include 	"cp$src:ev_def.h"
#include 	"cp$src:alpha_def.h"
#include 	"cp$src:aprdef.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:nvram_def.h"
/* Platform specific definition files */
#include 	"cp$src:platform_cpu.h"    /* HWRPB$_ definitions */
#include 	"cp$src:apc.h"
#include 	"cp$src:isacfg.h"
#include	"cp$src:impure_def.h"
#include	"cp$src:i82378_def.h"
#include	"cp$src:pyxis_main_csr_def.h"
#include	"cp$src:romhead.h"
/*
** NOTE !!!!!
**
**  The undefine CC must be done to get a clean build
**
**  The file hwrpb_def.h defines the symbol CC which 
**  also gets defined by ev5_defs.sdl
**
**  We are in a catch 22, we can't change the ev5_defs
**  without affecting PALcode and we can't change hwrpb_def
**  without affecting the console source pool
*/
#undef  CC
#include	"cp$src:ev5_defs.h"

#define DEBUG	0

/*
** LX164 PCI Interrupt Control
** ===========================
**
** PCI interrupt control logic is implemented as 3 8-bit I/O slaves,
** on the ISA bus at addresses 804h, 805h, and 806h, by decoding the
** three ISA address bits SA<2:0> and the three ECAS_ADDR<2:0> bits.
**
** Each PCI interrupt can be individually masked by setting the
** appropriate bit in the mask register.  An interrupt is disabled
** by writing a '1' to the desired bit position in the mask register.
** An interrupt is enabled by writing a '0'.  The interrupt mask
** register is WRITE-ONLY. 
**
** An I/O read at ISA addresses 804h, 805h, and 806h returns the 
** current state of the UNMASKED 19 PCI interrupt sources.  On reads,
** a '1' indicates that the source has an interrupt request pending.
**
** Interrupt Request and Mask Register 1 (ISA Address 804h)
**
**     7       6       5       4       3       2       1       0
** +-------+-------+-------+-------+-------+-------+-------+-------+
** | INTB0 |  USB  |  IDE  |  SIO  | INTA3 | INTA2 | INTA1 | INTA0 |
** +-------+-------+-------+-------+-------+-------+-------+-------+
**
** Interrupt Request and Mask Register 2 (ISA Address 805h)
**
**    15      14      13      12      11      10       9       8
** +-------+-------+-------+-------+-------+-------+-------+-------+
** | INTD0 | INTC3 | INTC2 | INTC1 | INTC0 | INTB3 | INTB2 | INTB1 |
** +-------+-------+-------+-------+-------+-------+-------+-------+
**
** Interrupt Request and Mask Register 3 (ISA Address 806h)
**
**    23      22      21      20      19      18      17      16
** +-------+-------+-------+-------+-------+-------+-------+-------+
** |  RAZ  |  RAZ  |  RAZ  |  RAZ  |  RAZ  | INTD3 | INTD2 | INTD1 |
** +-------+-------+-------+-------+-------+-------+-------+-------+
**  RAZ = Read-As-Zero (Read Only)
**
** PCI device interrupt vectors = 90h + (bit position).
**
** LX164 Primary PCI Bus IDSEL and INT_PIN to INT_LINE Mappings
** -------------------------------------------------------------------------
** IDSEL                 PCI          Physical         INT_PIN/INT_LINE
** Device            Address Bit      Address       IntA  IntB  IntC  IntD
** -------------------------------------------------------------------------
** PCI option 2       PCI_AD<16>    87.0005.0000      2     9    13    17
** PCI option 0       PCI_AD<17>    87.0006.0000      0     7    11    15
** PCI option 1       PCI_AD<18>    87.0007.0000      1     8    12    16
** PCI-to-ISA Bridge  PCI_AD<19>    87.0008.0000     --    --    --    --
** PCI option 3       PCI_AD<20>    87.0009.0000      3    10    14    18
** USB                PCI_AD<21>    87.000A.0000      6     6     6     6
** IDE                PCI_AD<22>    87.000B.0000      5     5     5     5
**
*/
#define MAX_VECTORS	    28

unsigned char pci_irq_table[ MAX_VECTORS ] = {
     2,   9,  13,  17,	/* slot 5  - PCI option 2 */
     0,   7,  11,  15,	/* slot 6  - PCI option 0 */
     1,   8,  12,  16,	/* slot 7  - PCI option 1 */
   255, 255, 255, 255,	/* slot 8  - PCI-to-ISA bridge */
     3,  10,  14,  18,	/* slot 9  - PCI option 3 */
     6,   6,   6,   6,	/* slot 10 - USB */
     5,   5,   5,   5	/* slot 11 - IDE */
};

unsigned char vector_allocated[ MAX_VECTORS ];

/*
** External Data Declarations
*/
extern struct LOCK spl_kernel;

/*
** External Function Prototype Declarations
*/

/* From printf.c */
extern int err_printf( char *format, ... );
extern int qprintf( char *format, ... );
extern int pprintf( char *format, ... );
extern int printf( char *format, ... );
extern int strncmp( const char *__s1, const char *__s2, size_t __n );

/* From kernel_alpha.mar */
extern unsigned __int64 mfpr_whami( void );
extern void *cserve( int operation, ... );
extern void sysfault( int fault_code );
extern spinlock( struct LOCK *spl );
extern spinunlock( struct LOCK *spl );
extern void *cserve( int operation, ... );

/* From timer.c */
extern int krn$_timer_get_cycle_time( int id );

/* From filesys.c */
extern struct FILE *fopen( const char *__filename, const char *__mode, ... );
extern size_t fread( void *__ptr, size_t __size, size_t __nmemb, struct FILE *__stream );
extern int fseek( struct FILE *__stream, __int64 __offset, int __whence );
extern int fclose( struct FILE *__stream );
extern int sprintf( char *__s, const char *__format, ... );

/* From startstop.c */
extern void start_drivers( void );
extern void stop_drivers( void );

/* From toy_driver.c */
extern void rtc_write( int offset, int data );

/* From ev_driver.c *
extern int ev_read( char *name, struct EVNODE **evptr, int option );

/* From isacfg.c */
extern unsigned long isacfg_get_device( char *handle, int instance, struct TABLE_ENTRY *ptr );

#if XDELTA_ON
extern void *memset( void *__s, int __c, size_t __n );
extern int *getpd( int i );
extern void *dyn$_malloc( unsigned int size, unsigned int option, unsigned int modulus, 
			  unsigned int remainder, struct ZONE *zone );
#else
extern int int_vector_set ( int int_vector, void( *handler )(), void *p0, void *p1);
#endif

/*
** Local Function Prototype Declarations
*/
int cpu_speed( void );
int get_sysvar( void );

/*
** Definitions needed for clean build of console
*/
int n810_speed_ptr;
int isa_count = 1;

/*+
 * ============================================================================
 * = whoami - Identify on which processor the process is running              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Return a small (less that 63) integer indicating which CPU the caller
 *	is running on.  Note that by the time the caller gets around to using
 *	the information, he may have been moved to another CPU.
 *
 * FORM OF CALL:
 *
 *	whoami( ); 
 *
 * RETURNS:
 *
 *      CPU number on which current process is running
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
int whoami( void ) 
{
    return( mfpr_whami( ) );
}


/*+                         
 * ============================================================================
 * = primary - Determine if the process is running on the primary processor.  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Return a value of 1 if the caller is running on the primary processor.
 *	Return a value of 0 is the caller is running on a secondary processor.
 *
 * FORM OF CALL:
 *
 *	primary( ); 
 *
 * RETURN CODES:
 *                          
 *      0 - current process is running on a secondary processor.
 *      1 - current process is running on the primary processor.
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
int primary( void ) 
{
    return( whoami( ) == LX164$PRIMARY_CPU );
}                           


/*+
 * ============================================================================
 * = node_halt - node halt the processor                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Node halts the given processor, this is currently a NOOP on 
 *      AlphaPC systems. This routine will need to contain useful 
 *	code if AlphaPC systems decide to go to SMP.
 *
 * FORM OF CALL:
 *
 *	node_halt( who ); 
 *
 * RETURNS:                 
 *
 *      None                                              
 *
 * ARGUMENTS:
 *
 *      int who - processor to halt
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void node_halt( int who ) 
{
}
                            

/*+
 * ============================================================================
 * = start_secondary - Start a secondary processor at the specified address.  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine first forces a secondary processor into Console mode, then
 *      uses the CCA to start a secondary processor. This is basically a NOOP on 
 *      AlphaPC systems until we decide to go to SMP.
 *
 * FORM OF CALL:
 *                          
 *	start_secondary( id, address ); 
 *
 * RETURN CODES:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *      int id	- node id of processor to start
 *	int address - physical address of routine to be started.
 *
 * SIDE EFFECTS:
 *
 *      None                
 *
-*/

void start_secondaries( void )
{
}

void start_secondary( int id, int address )
{
}


/*+
 * ============================================================================
 * = console_restart - Restart the system.                                    = 
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Platform specific code for restarting the system. This code currently 
 *      jumps to the PALcode reset entry point to restart the console.
 *
 * FORM OF CALL:
 *
 *	console_restart( ); 
 *
 * RETURNS:
 *
 *      None
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

void console_restart( void )
{
    cserve( CSERVE$MTPR_EXC_ADDR, PAL$PAL_BASE + 1 );
}

                            
/*+     
 * ============================================================================
 * = build_dsrdb - Build the dynamic system recognition data block            =
 * ============================================================================
 *      
 * OVERVIEW:
 *      
 *	Platform specific code for building the dynamic system recognition
 *      data block.
 *      
 * FORM OF CALL:
 *      
 *	build_dsrdb( hwrpb, offset ); 
 *      
 * RETURNS:
 *      
 *      Size of DSRDB
 *      
 * ARGUMENTS:
 *
 *      struct HWRPB *hwrpb - pointer to the HWRPB data structure
 *      int offset - offset from HWRPB to where DSRDB will reside
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/     

/*
** Global data for Dynamic System Recognition Data Block (DSRDB)
*/      
       
#define LURT_COLS 9
#define NAME_SIZE 80
       
/*                                      
** AlphaPC 164LX SMM values:                    
**                                      
**  1694 - AlphaPC 164LX (EV56/400 MHz)                     
**  1695 - AlphaPC 164LX (EV56/466 MHz)                     
**  1696 - AlphaPC 164LX (EV56/533 MHz)
**  1697 - AlphaPC 164LX (EV56/600 MHz)
**                                                        
*/                                                        
                                                          
const int lurt_info[LURT_COLS] = { 12,-1,-1,-1,-1,-1,-1,1050,1050 };
    
int build_dsrdb( struct HWRPB *hwrpb, int offset )                     
{                                                              
    int size;                                                      
    int i;
    char t[NAME_SIZE];
    unsigned __int64 *lurt;
    unsigned __int64 *name;                                                     
    struct DSRDB *dsrdb;                                           
                                                                   
    hwrpb->DSRDB_OFFSET[0] = offset;
/*                       
** Allocate the DSRDB ...
*/                       
    dsrdb = ( int )hwrpb + offset;                  
    size = sizeof( *dsrdb ) + 8 + LURT_COLS * 8 + 8 + NAME_SIZE;
    memset( dsrdb, 0, size );       
/* 
** AlphaPC 164 has different SMM's based on processor speed.
*/                                                               
    switch ( cpu_speed( ) ) {
	case 400:
	    dsrdb->SMM[0] = 1694;
	    break;
	case 466:
	    dsrdb->SMM[0] = 1695;
	    break;
	case 533:
	    dsrdb->SMM[0] = 1696;
	    break;
	case 600:
	    dsrdb->SMM[0] = 1697;
	    break;
	default:
#if DEBUG
	    err_printf( "CPU speed error = %d MHz, in build_dsrdb().\n", cpu_speed( ) );
#endif
	    dsrdb->SMM[0] = 1694;
	    break;
    }
    sprintf( t, "Digital AlphaPC 164LX %3d MHz", cpu_speed( ) );
/*                                         
** Set up LURT and name offsets ...
*/                                         
    dsrdb->LURT_OFFSET[0] = sizeof( *dsrdb );
    dsrdb->NAME_OFFSET[0] = sizeof( *dsrdb ) + 8 + LURT_COLS*8;
/*                                         
** Set up the LURT information             
*/                                         
    lurt = ( int )dsrdb + dsrdb->LURT_OFFSET[0];
    *lurt++ = LURT_COLS;                   
    for( i = 0; i < LURT_COLS; i++ )       
	*lurt++ = lurt_info[i];
/*                                         
** Save the system name string             
*/                                         
    name = ( int )dsrdb + dsrdb->NAME_OFFSET[0];
    *name++ = strlen( t );                 
    strcpy( name, t );                     

    return( size );                        
}                                      
                                           

/*XDELTA routines*/
#if XDELTA_ON

extern int scb;
extern char BPT_SYNC[];
extern char EXCEPT_SYNC[];
int exe$gl_scb = 0;
int xdt$gl_scb = 0;
int mmg$gl_sptbase = 0;
int mmg$gl_va_to_vpn = -HWRPB$_PAGEBITS;
int xdt$gl_owner_id = -1;
int xdt$gl_benign_cpus = 0;
int xdt$gl_interlock = 0;
int smp$gl_primid = LX164$PRIMARY_CPU;
int smp$gl_cpuconf = (1<<HWRPB$_NPROC)-1;
int exe$gl_state = 0;
int exe$gl_time_control = 0;

void smp$intall_bit_acq( void )
{
}

void ini$writable( void )
{
}

void ini$rdonly( void )
{
}

void xdelta( void )
{
    int *pd;
    int do_bpt( );

    memset( BPT_SYNC, 0, 8 );
    memset( EXCEPT_SYNC, 0, 8 );
    exe$gl_scb = scb;
    xdt$gl_scb = ( int )dyn$_malloc( SCB_ENTRIES * SCB_ENTRY_SIZE,
	    DYN$K_SYNC|DYN$K_ALIGN, HWRPB$_PAGESIZE, 0, (struct ZONE *)0 );
    pd = getpd( do_bpt );
    xdt$init( pd[2] );
}

con$save_cty( void )
{
}

con$restore_cty( void )
{
}

con$getchar( void )
{
    return( jxgetc( ) );
}

con$putchar( char c )
{
    jputc( c );
}
#else
void fake_bpt( void )
{
    struct PCB *pcb;
    struct ALPHA_CTX *acx;

    pcb = getpcb( );
    acx = pcb->pcb$a_acp;
    pprintf( "breakpoint at PC %x desired, XDELTA not loaded\n", acx->acx$q_pc[0] );
}

void xdelta( void )
{
/*
** Remove printf to cleanup power on display
**    qprintf("XDELTA not enabled.\n");
*/
    ( void )int_vector_set( SCB$Q_BREAKPOINT, fake_bpt, 0, 0 );
}

#endif

void sys$native_to_translated( void )
{
    sysfault( 9992 );
}


/* arc load support routines	*/

void jump_to_arc( int p )
{
    cserve( CSERVE$JUMP_TO_ARC, p | 1 );
}

/*
 * ============================================================================
 *   load_arc_fw - Load the NT firmware from flash or a floppy diskette	      =
 * ============================================================================
 */
int load_arc_fw( void ) 
{
    int i, j;
    int dest;
    struct FILE *fp;
    romheader_t hdr;
    unsigned int checksum;
    char *srcname[] = { "fat:lx164nt.rom/dva0", NULL };

    start_drivers( );     
    for ( i = 0; srcname[i] != NULL; i++ ) {
	err_printf( "Loading AlphaBIOS from file %s\n", srcname[i] );
	if ( ( fp = fopen( srcname[i], "r" ) ) == NULL ) continue;
	do {              
	    /* Read the ROM header */
	    if ( fread( &hdr, sizeof( hdr ), 1, fp ) != sizeof( hdr ) ) {
		err_printf ( "Error reading ROM header\n" );
		break;    
	    }
	    /* Validate the ROM header */
	    if ( ( hdr.romh.V0.signature != ROM_H_SIGNATURE ) || ( hdr.romh.V0.csignature != ~ROM_H_SIGNATURE ) ) {
		err_printf ( "ROM header signature does not match expected pattern %08X %08X\n",
			    ROM_H_SIGNATURE, ~ROM_H_SIGNATURE );
		break;
	    }
	    /* Read the ROM image */
	    fseek( fp, hdr.romh.V0.hsize, SEEK_SET );
	    dest = hdr.romh.V0.destination.low;
	    if ( fread( dest, hdr.romh.V0.size, 1, fp ) != hdr.romh.V0.size ) {
		err_printf ( "Error reading ROM image\n" );
		break;
	    }
	    /* Validate the ROM image checksum */
	    checksum = 0;
	    for ( j = 0; j < hdr.romh.V0.size; j++ )
		COMPUTE_CHECKSUM( *( unsigned char * )( dest + j ), checksum );
	    if ( checksum != hdr.romh.V0.checksum ) {
		err_printf ( "Computed ROM image checksum 0x%04X does not match header checksum 0x%04X\n",
			    checksum, hdr.romh.V0.checksum );
		break;
	    }
	    /* Jump and execute the image ... */
	    fclose( fp );
	    stop_drivers( );
	    jump_to_arc( dest );
	} while ( FALSE );
	fclose ( fp );
    }
    return ( msg_failure );
}

/*
 * ============================================================================
 *   arc - Transfer control to the ARC console				      =
 * ============================================================================
 *
 * COMMAND TAG: arc 0 RXBZ arc nt
 */
int arc( int argc, char *argv[] ) 
{
    return( load_arc_fw( ) );
}

/*
 * COMMAND TAG: jtopal 0 RXBZ jtopal
 */
void jtopal( int argc, char *argv[] )
{
    if( argc == 2 )
	jump_to_arc( xtoi( argv[1] ) );
}


/*+
 * ============================================================================
 * = config_get_device - Return whether device enabled                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	The calling routine passes in the name of the device, and this
 *	platform specific code will search for that device and return
 *      1 if it is found and 0 if it is disabled or unavailable.
 *
 * FORM OF CALL:
 *
 *	found = config_get_device("devname"); 
 *
 * RETURNS:                 
 *
 *      found   -  1 if enabled
 *                 0 if disabled or unavailable
 *
 * ARGUMENTS:
 *
 *      char *devname - device for which to search 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
unsigned int config_get_device( char *devname )
{
    struct TABLE_ENTRY temp;
    int found;

    found = isacfg_get_device( devname, 1, &temp );
    if ( ( found ) && ( temp.device_enable == 1 ) )
	return 1;
    else
	return 0;
}

                                                   
#define MAX_REV_LEVELS 8
#define RCS_SIGNATURE 0x55

void show_srom_version( void )
{
    struct impure *impure_ptr;
    unsigned int signature;
    int i, rev;

    impure_ptr = ( struct impure * )( PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE );
    signature = *( unsigned __int64 * )( impure_ptr->cns$srom_signature );
    if ( ( signature >> 16 ) == 0xDECB ) {
	printf( "\nSROM Revision: " );
	rev = ( int )( ( * ( unsigned __int64 * )( impure_ptr->cns$srom_srom_rev ) ) >> ( 8 * ( MAX_REV_LEVELS - 1 ) ) ) & 0xFF;
	if ( ( rev != ( int )( RCS_SIGNATURE & 0xFF ) ) && ( rev != ( int )( ~RCS_SIGNATURE & 0xFF ) ) ) {
	    printf( "UNKNOWN" );
	}
	else {
	    for ( i = 0; i < MAX_REV_LEVELS; i++ ) {
		rev = ( int )( ( * ( unsigned __int64 * )( impure_ptr->cns$srom_srom_rev ) ) >> ( 8 * i ) ) & 0xFF;
		if ( rev == 0 ) break;
		printf( "%s%d", ( i == 0 ) ? "" : ".", rev );
	    }
	}
	printf( "\n" );
    }
}                                                   
                                                    

int cpu_speed( void )
{
    int speed;
    struct impure *impure_ptr;
    unsigned int signature;

    impure_ptr = ( struct impure * )( PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE );
    signature = *( unsigned __int64 * )( impure_ptr->cns$srom_signature );
    if ( ( signature >> 16 ) == 0xDECB ) {
	speed = ( int )( 1000000 / 
	    *( unsigned __int64 * )( impure_ptr->cns$srom_cycle_cnt ) );
    }
    else {
	speed = ( int )( krn$_timer_get_cycle_time( whoami( ) ) / 1000000 );     
    }
    return( speed );
}


int get_sysvar( void )
{
    int sysvar = HWRPB$_SYSVAR;

    switch ( cpu_speed( ) ) {
	case 400:
	    sysvar += ( 8 << 10 );
	    break;
	case 466:
	    sysvar += ( 9 << 10 );
	    break;
	case 533:
	    sysvar += ( 10 << 10 );
	    break;
	case 600:
	    sysvar += ( 11 << 10 );
	    break;
	default:
#if DEBUG
	    err_printf( "CPU speed error = %d MHz, in get_sysvar().\n", cpu_speed( ) );
#endif
	    sysvar += ( 8 << 10 );
	    break;
    }
    return( sysvar );
}
                                                   
void PowerUpProgress ( unsigned char value, char *string, int a, int b, int c, int d )
{                                                   
    qprintf( "%2x.", value );
#if 0
    qprintf( string, a, b, c, d );
#endif
    outportb( NULL, 0x80, value );
}


/*+
 * ============================================================================
 * = sys_pci_enable_interrupt - Enable a PCI interrupt                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will enable a PCI interrupt by clearing the appropriate 
 *	bit in the PCI interrupt mask register.
 *
 * FORM OF CALL:
 *
 *	sys_pci_enable_interrupt( vector )
 * 
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *      int vector - a PCI device interrupt vector
 *
-*/
void sys_pci_enable_interrupt( int vector )
{
    unsigned int irq, mask;

    irq = vector - PCI_BASE_VECTOR;
    mask = cserve( CSERVE$PCI_INTR_ENABLE, irq );

#if DEBUG
    qprintf( "Enabed PCI interrupt %d for vector %04x, mask = %08x\n",
	      irq, vector, mask );
#endif
}


/*+
 * ============================================================================
 * = sys_pci_disable_interrupt - Disable a PCI interrupt                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine will disable a PCI interrupt by setting the appropriate
 *	bit in the PCI interrupt mask register.
 *
 * FORM OF CALL:
 *
 *	sys_pci_disable_interrupt( vector )
 * 
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *      int vector - a PCI device interrupt vector
 *
-*/
void sys_pci_disable_interrupt( int vector )
{
    unsigned int irq, mask;

    irq = vector - PCI_BASE_VECTOR;
    mask = cserve( CSERVE$PCI_INTR_DISABLE, irq );

#if DEBUG
    qprintf( "Disabled PCI interrupt %d for vector %04x, mask = %08x\n",
	      irq, vector, mask );
#endif
}


/*+
 * ============================================================================
 * = io_get_vector - Allocate an ISA device interrupt vector                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will allocate an ISA device interrupt vector for the 
 *	specified IRQ channel.
 *
 *	The following table shows the allocation of ISA device interrupt 
 *	vectors:
 *
 *	IRQ	Vector	       Typical Interrupt Source
 *	====	======	========================================
 *	irq0	 0x80	Reserved - Interval timer 1 counter 0 output
 *	irq1	 0x81	Keyboard controller
 *	irq2	 0x82	Reserved - INTR from slave 82C59 controller
 *	irq3	 0x83	COM2 serial port
 *	irq4	 0x84	COM1 serial port
 *	irq5	 0x85	Available
 *	irq6	 0x86	Floppy controller
 *	irq7	 0x87	Parallel port 1
 *	irq8	 0x88	Reserved - Real time clock in 82378IB
 *	irq9	 0x89	Available
 *	irq10	 0x8A	Available
 *	irq11	 0x8B	Available
 *	irq12	 0x8C	Mouse controller
 *	irq13	 0x8D	Available
 *	irq14    0x8E	IDE controller
 *	irq15	 0x8F	IDE controller
 *
 * FORM OF CALL:
 *
 *	io_get_vector( pb, irq );
 *                    
 * RETURNS:
 *
 *	Interrupt vector allocated (base vector + (IRQ from 0 to 15) ),
 *	otherwise, zero if vector not allocated.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned int irq - An ISA IRQ channel number.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
unsigned int io_get_vector( struct pb *pb, unsigned int irq )
{
    unsigned int vector;

    vector = ISA_BASE_VECTOR + irq;

#if ((IPL_RUN) && (IPL_RUN > 19))
    vector = 0;
#endif

    return( vector );
}


/*+
 * ============================================================================
 * = io_issue_eoi - Issue an End-Of-Interrupt command.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will issue an End-Of-Interrupt (EOI) command to
 *	clear the specified interrupt request.
 *
 * FORM OF CALL:
 *
 *	io_issue_eoi( pb, vector );
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
 *	unsigned int vector - A device interrupt vector.
 *
-*/
void io_issue_eoi( struct pb *pb, unsigned int vector )
{
    unsigned int irq;
/*
** Is this an ISA device interrupt vector?
*/
    if ( ( vector & 0xF0 ) == ISA_BASE_VECTOR ) {
	irq = vector & 0x0F;
/*
** Issue a Specific EOI command to clear the interrupt request
** corresponding to IRQ0 - IRQ7 by writing the Operation Control
** Word 2 (OCW) register on interrupt controller 1.
*/
	spinlock( &spl_kernel );
	if ( irq < 8 ) {
	    outportb( pb, I82378_C1_OCW2, I82378_EOI_S + ( irq & 7 ) );
/*
** Issue a Specific EOI command to clear the interrupt request
** corresponding to IRQ8 - IRQ15 by writing the Operation Control
** Word 2 (OCW) register on interrupt controller 2.
**
** Cascaded interrupt controller configurations must also issue
** a second Specific EOI command to clear the slave input (IRQ2) 
** on the master controller.
*/
	} else {
	    outportb( pb, I82378_C2_OCW2, I82378_EOI_S + ( irq & 7 ) );
	    outportb( pb, I82378_C1_OCW2, I82378_EOI_S + 2 );
	}
	spinunlock( &spl_kernel );
    }
}


/*+
 * ============================================================================
 * = io_disable_interrupts - Disable a PCI or ISA device interrupt.           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will disable a PCI or ISA device interrupt by setting
 *	the appropriate bit in a mask register.
 *
 * FORM OF CALL:
 *
 *	io_disable_interrupts( pb, vector );
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
 *	unsigned int vector - An ISA or PCI device interrupt vector.
 *
-*/
void io_disable_interrupts( struct pb *pb, unsigned int vector )
{
    unsigned int irq;
/*
** Is this an ISA device interrupt vector?
*/
    if ( ( vector & 0xF0 ) == ISA_BASE_VECTOR ) {
	irq = vector & 0x0F;
/*
** Disable the corresponding IRQ0 - IRQ7 channel in the Interrupt Mask 
** Register (IMR) by writing the Operation Control Word 1 (OCW1) register 
** on interrupt controller 1.
*/
	spinlock( &spl_kernel );
	if ( irq < 8 ) {
	    outportb( pb, I82378_C1_OCW1, inportb ( pb, I82378_C1_OCW1 ) | ( 1 << ( irq & 7 ) ) );
/*
** Disable the corresponding IRQ8 - IRQ15 channel in the Interrupt Mask 
** Register (IMR) by writing the Operation Control Word 1 (OCW1) register 
** on interrupt controller 2.
*/
	} else {
	    outportb( pb, I82378_C2_OCW1, inportb ( pb, I82378_C2_OCW1 ) | ( 1 << ( irq & 7 ) ) );
	}
	spinunlock( &spl_kernel );
/*
** Make sure the In Service (IS) bit for this interrupt is cleared.
*/
	io_issue_eoi( pb, vector );
    }
/*
** Not ISA, must be a PCI device interrupt vector.
*/
    else {
	sys_pci_disable_interrupt( vector );
    }
}


/*+
 * ============================================================================
 * = io_enable_interrupts - Enable a PCI or ISA device interrupt.             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will enable a PCI or ISA device interrupt by clearing
 *	the appropriate bit in a mask register.
 *
 * FORM OF CALL:
 *
 *	io_enable_interrupts( pb, vector );
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
 *	unsigned int vector - An ISA or PCI device interrupt vector.
 *
-*/
void io_enable_interrupts( struct pb *pb, unsigned int vector )
{
    unsigned int irq;
/*
** Is this an ISA device interrupt vector?
*/
    if ( ( vector & 0xF0 ) == ISA_BASE_VECTOR ) {
	irq = vector & 0x0F;
/*
** Enable the corresponding IRQ0 - IRQ7 channel in the Interrupt Mask 
** Register (IMR) by writing the Operation Control Word 1 (OCW1) register 
** on interrupt controller 1.
*/
	spinlock( &spl_kernel );
	if ( irq < 8 ) {
	    outportb( pb, I82378_C1_OCW1, inportb ( pb, I82378_C1_OCW1 ) & ~( 1 << ( irq & 7 ) ) );
/*
** Enable the corresponding IRQ8 - IRQ15 channel in the Interrupt Mask 
** Register (IMR) by writing the Operation Control Word 1 (OCW1) register 
** on interrupt controller 2.
*/
	} else {
	    outportb( pb, I82378_C2_OCW1, inportb ( pb, I82378_C2_OCW1 ) & ~( 1 << ( irq & 7 ) ) );
	}
	spinunlock( &spl_kernel );
/*
** Make sure the In Service (IS) bit for this interrupt is cleared.
*/
	io_issue_eoi( pb, vector );
    }
/*
** Not ISA, must be a PCI device interrupt vector.
*/
    else {
	sys_pci_enable_interrupt( vector );
    }
}


/*+
 * ============================================================================
 * = sys_environment_init - Initialize system specific environment variables  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will initialize any system specific environment
 *	variables.
 *
 * FORM OF CALL:
 *
 *	sys_environment_init( );
 *                    
 * RETURNS:
 *
 *	1 if successful, otherwise, zero on failure.
 *
 * ARGUMENTS:
 *
 *	None.
 *
 * SIDE EFFECTS:
 *
 *	This routine is called before phase 5 driver start up and requires
 *	that the console's ev_driver be running.
 *
-*/
enum pci_parity_types { off, on, sniff };

int sys_environment_init( void )
{
    struct EVNODE evn, *evp;
    enum pci_parity_types parity;
    unsigned __int64 BaseCSR = pyxis_k_main_csr_base;
    unsigned int ctrl_bits, mask_bits;
 
    evp = &evn;
/* 
** Check for PCI parity enable
*/
    if ( ev_read( "pci_parity", &evp, 0 ) == msg_success ) {
	if ( strncmp( evp->value.string, "off", 3 ) == 0 ) 
	    parity = off;
	if ( strncmp( evp->value.string, "on", 2 ) == 0 ) 
	    parity = on;
	if ( strncmp( evp->value.string, "sniff", 3 ) == 0 ) 
	    parity = sniff;
    }
#if DEBUG
    qprintf( "Environment init: pci_parity = %s\n", evp->value.string );
#endif

    ctrl_bits = READ_IO_CSR( CSR_PYXIS_CTRL );
    mask_bits = READ_IO_CSR( CSR_PYXIS_ERR_MASK );

    switch ( parity ) {
	case on:
	    ctrl_bits |= ( pyxis_ctrl_m_perr_en | 
			   pyxis_ctrl_m_addr_pe_en );
	    mask_bits |= ( pyxis_err_mask_m_perr | 
			   pyxis_err_mask_m_pci_addr_pe );
	    break;
	case off:
	    ctrl_bits &= ~( pyxis_ctrl_m_perr_en | 
			    pyxis_ctrl_m_addr_pe_en );
	    mask_bits &= ~( pyxis_err_mask_m_perr | 
			    pyxis_err_mask_m_pci_addr_pe );
	    break;
    	case sniff:
	    break;
    }

    WRITE_IO_CSR( CSR_PYXIS_CTRL, ctrl_bits );
    mb( );
    WRITE_IO_CSR( CSR_PYXIS_ERR_MASK, mask_bits );
    mb( );

    return (1);
}


/*+
 * ============================================================================
 * = sys_irq_init - Initialize PCI irq table				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Initialize the PCI irq allocation table entries to default values.
 *
 * FORM OF CALL:
 *
 *	sys_irq_init( )
 *                    
 * FUNCTIONAL DESCRIPTION 
 *
 *	Clear the PCI irq allocation entries so drivers can allocate interrupt 
 *	vectors.
 *
-*/
void sys_irq_init( void )
{
    int i;

    for ( i = 0;  i < MAX_VECTORS;  i++ ) {
    	vector_allocated[ i ] = 0;
    }
}
