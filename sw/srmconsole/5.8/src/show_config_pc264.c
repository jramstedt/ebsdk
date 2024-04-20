/* file:    show_config_pc264.c
 *
 * Copyright (C) 1998 by
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
 *	Alpha SRM Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      Platform specific system configuration screen(s).
 *
 *  AUTHORS:
 *
 *      Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *	24-Mar-1998
 *
 *  MODIFICATION HISTORY:
 *
 *	er	09-Sep-1999	Fix processor revision display in show_core()
 *	er	03-Jun-1999	Fix show_core() to recognize EV67 processor type.
 *	jwj	13-May-1999	Fix show_core() for new srom revision format (increase srom_rev[])
 *	er	03-Sep-1998	Display TIG and Arbiter PLD revision information.
 *	er	16-Jun-1998	Use get_srom_revision() external routine
 *				to display SROM revision information.
 *				Removed show_srom_version().
 *
 *--
 */
/* $INCLUDE_OPTIONS$ */
#include 	"cp$inc:platform_io.h"
/* $INCLUDE_OPTIONS_END$ */
#include	"cp$src:kernel_def.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:ev_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:impure_def.h"
#include	"cp$src:version.h"
#include 	"cp$src:pb_def.h"
#include	"cp$src:probe_io_def.h"
#include 	"cp$src:ub_def.h"
#include	"cp$src:hwrpb_def.h"
/* Platform specific include files */
#include	"cp$src:platform.h"
#include	"cp$src:platform_cpu.h"
#include	"cp$src:tsunami.h"

/*
** External variable declarations
*/
extern struct SEMAPHORE ubs_lock;
extern unsigned int num_ubs;
extern struct ub **ubs;
extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
extern unsigned int max_hose;
extern int primary_cpu;
extern int cpu_mask;

/*
** External function prototype declarations
*/
extern int printf( );
extern strcmp_ub( struct ub *ub1, struct ub *ub2 );
extern int show_isa_config( );	/* isacfg.c */
extern showmem( int argc, char *argv[] );
extern int krn$_timer_get_cycle_time( int id );
extern void get_srom_revision( char *srev, int id );

/*
** Local function prototype declarations
*/
void show_firmware( void );
void show_core_system( void );
void show_pci_config( unsigned int hose );
void show_ubs( int hose, int bus, int slot, int func );
void show_bcache( int id );

/*+
 * ============================================================================
 * = show config - Display the system configuration.		              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This routine displays the system configuration information.
 *	Console and PAL revisions for Digital UNIX and OpenVMS are 
 *	displayed.  All core system, PCI, and [E]ISA modules are listed.  
 *	Information on controllers and their devices is also displayed.  
 *
 *   COMMAND FORM:
 *  
 *   	show config ( )
 *  
 *   COMMAND ARGUMENT(S):
 *          
 *	None
 *
 *   COMMAND OPTION(S):
 *
 *	None                                 
 *
 *   COMMAND EXAMPLE(S):
 *
 *	In the following example, a system's configuration is displayed. 
 *~
 *	>>>show config
 *                      Alpha 21164 Evaluation Board 266 MHz
 *
 *      SRM Console X4.5-381     VMS PALcode V1.17-0, OSF PALcode V1.21-0
 *	DECchip (tm) 21164-5
 *	DECchip (tm) 21171 CIA ASIC Pass 2
 *
 *	MEMORY
 *	64 Meg of system memory
 *
 *
 *	PCI Bus
 *	    Bus 00  Slot 05: Digital ZLXp Graphics Controller
 *
 *	    Bus 00  Slot 07: DECchip 21040 Network Controller
 *					  ewa0.0.0.7.0          08-00-2B-E2-B1-08
 *	    Bus 00  Slot 08: Intel SIO 82378
 *	    Bus 00  Slot 09: NCR 53C810 Scsi Controller
 *					  pka0.7.0.9.0          SCSI Bus ID 7
 *					  dka0.0.0.9.0           SEAGATE ST31230N
 *					  dka400.4.0.9.0         RRD43
 *
 *	ISA
 *	Slot    Device  Name            Type         Enabled  BaseAddr  IRQ     DMA
 *	0
 *		0       MOUSE           Embedded        Yes     60      12
 *		1       KBD             Embedded        Yes     60      1
 *		2       COM1            Embedded        Yes     3f8     4
 *		3       COM2            Embedded        Yes     2f8     3
 *		4       LPT1            Embedded        Yes     3bc     7
 *		5       FLOPPY          Embedded        Yes     3f0     6       2
 *
 *~
 *   COMMAND REFERENCES:
 *
 *	show memory, show fru
 *
 * FORM OF CALL:
 *
 *	showconfig( argc, *argv[] )
 *
 * RETURN CODES:
 *
 *   	msg_success  - completed successfully
 *
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *  	None
 *
-*/
int showconfig( int argc,char *argv[] ) 
{
    struct EVNODE evn, *evp;
    struct HWRPB *hwrpb = ( struct HWRPB * )PAL$HWRPB_BASE;
    struct DSRDB *dsrdb;
    int hose;

    dsrdb = ( int )hwrpb + hwrpb->DSRDB_OFFSET[0];
    evp = &evn;
    ev_read( "oem_string", &evp, 0 );
    if ( strlen( evp->value.string ) != 0 )
	printf( "%24s%s\n", "", evp->value.string );
    else
	printf( "%24s%s\n", "", ( int * )( ( int )dsrdb + dsrdb->NAME_OFFSET[0] + 8 ) );

    /* Display the PAL and SRM firmware revisions */
    show_firmware( );

    /* Display the CPU and core logic information */
    show_core_system( );

    /* Display the memory configuration	*/
    printf( "\nMEMORY\n" );
    showmem( argc, argv );

    printf( "\n" );

    /* Display the PCI configuration */
    for ( hose = 0;  hose <= max_hose;  hose++ ) {
    	show_pci_config( hose );
    }
    printf( "\n" );

    /* Display the ISA configuration */
    show_isa_config( );

    printf( "\n" );

    return( msg_success );
}

/*+
 * ============================================================================
 * = show_firmware - Display the system firmware and PALcode revisions        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This routine displays the system firmware and PALcode versions.
 *
 * FORM OF CALL:
 *
 *      show_firmware( );
 *
 * RETURN CODES:
 *
 *   	None
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *  	None
 *
-*/
void show_firmware( void ) 
{
    struct EVNODE evn, *evp;

    printf( "\nSRM Console:\t%c%d.%d-%d"OEM_VERSION_SUB"\n",
	    v_variant,
	    v_major,
	    v_minor,
	    v_sequence
    );
    evp = &evn;
    ev_read( "pal", &evp, EV$K_SYSTEM );
    printf( "PALcode:\t%s\n", evp->value.string );
}

/*+
 * ============================================================================
 * = show_core_system - Display the core system information		      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This routine displays the CPU and core logic type and revision 
 *	information.
 *
 * FORM OF CALL:
 *
 *      show_core_system( );
 *
 * RETURN CODES:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *  	None
 *
-*/
void show_core_system( void ) {

    int i, type, rev, speed;
    char chip_name[8];
    char srom_rev[10];

    printf( "\nProcessors\n" );
    for ( i = 0;  i < MAX_PROCESSOR_ID;  i++ ) {
    	if ( cpu_mask & ( 1 << i ) ) {
	    type = ( ( int * )( PAL$IMPURE_BASE + IMPURE$CPU_TYPE_OFFSET + 4 ) )[i*2];
	    if ( type == 8 )
		strcpy( chip_name, "21264" );
	    else if ( type == 11 )
		strcpy( chip_name, "21264A" );
	    else if ( type == 13 )
		strcpy( chip_name, "21264B" );
	    else
		sprintf(chip_name, "Type %d",type);
/* 
** If the revision level isn't legal set it to "?" ('0' + 0xf) 
*/
	    rev = ( ( int * )( PAL$IMPURE_BASE + IMPURE$CPU_TYPE_OFFSET ) )[i*2];
	    if ( ( type == 8 ) && ( rev < 0 || rev > 8 ) )
		rev = 0xf;
	    else if ( ( type == 11 ) && ( rev < 2 || rev > 10 ) )
		rev = 0xf;
	    speed = krn$_timer_get_cycle_time( i ) / 1000000;     
	    printf( "CPU %d\t\tAlpha %s-%c %3d MHz\t", i, chip_name, '0' + rev, speed );
	    get_srom_revision( &srom_rev, i );
	    printf( "SROM Revision: %5s\n", srom_rev );
	    show_bcache( i );
	}
    }
    printf( "Core Logic\n" );
    printf( "Cchip\t\tDECchip 21272-CA Rev %d.1\n", ( ( ReadTsunamiCSR( CSR_MISC ) >> 32 ) & 0xFFL ) + 1 ); 
    printf( "Dchip\t\tDECchip 21272-DA Rev %d.0\n", ( ( ReadTsunamiCSR( CSR_DREV ) & 0x0FL ) ) + 1 );
    printf( "Pchip 0\t\tDECchip 21272-EA Rev %d", ( ( ReadTsunamiCSR( PCHIP0_PCTL ) >> 24 ) & 0x03L ) + 1 );
    if ( rev = ( ReadTsunamiCSR( PCHIP0_PMONCTL ) & 0x01L ) )
	printf( ".%d\n", rev + 1 );
    else
	printf( "\n" );
    printf( "Pchip 1\t\tDECchip 21272-EA Rev %d", ( ( ReadTsunamiCSR( PCHIP1_PCTL ) >> 24 ) & 0x03L ) + 1 );
    if ( rev = ( ReadTsunamiCSR( PCHIP1_PMONCTL ) & 0x01L ) )
	printf( ".%d\n", rev + 1 );
    else
	printf( "\n" );
/*
** Display TIG and Arbiter PLD revision information
*/
    printf( "\n" );
    printf( "TIG\t\tRev %d.%d\n", 
	    ( intig( NULL, 0xE00006 ) >> 5 ) & 0x7, intig( NULL, 0xE00006 ) & 0x1F );
    printf( "Arbiter\t\tRev %d.%d (0x%x)\n",
	    ( intig( NULL, 0xE00005 ) >> 5 ) & 0x7, intig( NULL, 0xE00005 ) & 0x1F,
	      intig( NULL, 0xE00007 ) & 0xFF );
}

/*+
 * ============================================================================
 * = show_pci_config - Display the system PCI bus configuration		     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This routine displays the system PCI configuration and devices
 *	on the current hose/bus being sniffed.
 *
 * FORM OF CALL:
 *
 *      show_pci_config( hose );
 *
 * RETURN CODES:
 *
 *   	None
 *
 * ARGUMENTS:
 *
 *	unsigned int hose - Hose currently being sniffed
 *
 * SIDE EFFECTS:
 *
 *  	None
 *
-*/
void show_pci_config( unsigned int hose ) {

    struct pb *pb;
    struct pci_device_misc *pdm;
    struct pci_bridge_misc *pbm;
    unsigned int i, bus, max_bus, slot, func, max_func;

    if ( hose > max_hose ) {
    	return;
    }
    max_bus = 0;
    printf( "\nPCI Hose %02d\n", hose );
    for ( bus = 0;  bus <= max_bus;  bus++ ) {
	if ( bus != 1 ) { /* Bus 1 is reserved for EISA/ISA */
	    for ( slot = 0;  slot <= 31 ;  slot++ ) {
		max_func = 0;
		for ( func = 0;  func <= 7;  func++ ) {
		    pb = get_matching_pb( hose, bus, slot, func, 0 );
		    if ( pb ) {
			if ( pb->type == TYPE_PCI ) {
			    if ( func == 0 )
				if ( incfgb( pb, 0x0E ) & 0x80 )
				    max_func = 7;
			    if ( max_func )
				printf( "     Bus %02d  Slot %02d/%d: ", bus, slot, func );
			    else
				printf( "     Bus %02d  Slot %02d: ", bus, slot );

			    printf( "%s\n", pb->hardware_name );
			    if ( incfgw( pb, 0x0A ) == 0x0604 ) {
				pdm = pb->pdm;
				pbm = pdm->pbm;
				if ( max_bus < pbm->sub_bus )
				    max_bus = pbm->sub_bus;
			    }
			    printf( "%35s%-20.20s%  %-20.20s\n","",
				    pb->name, pb->info );
			    show_ubs( hose, bus, slot, func );
			}
		    }
		}
	    }
	}
    }
}

/*+
 * ============================================================================
 * = show_ubs - Display the units for the port block			      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This routine displays the units attached to a controller.
 *	The port block is used to match the hose, bus, and slot 
 *	with the unit's hose, bus, and slot.
 *
 * FORM OF CALL:
 *
 *      show_ubs( hose, bus, slot, func );
 *
 * RETURN CODES:
 *
 *   	None
 *
 * ARGUMENTS:
 *
 *	int hose   - current hose being sniffed
 *
 *	int bus    - current bus being sniffed
 *
 *	int slot   - current slot being sniffed
 *
 *	int func   - current function being sniffed
 *	
 * SIDE EFFECTS:
 *
 *  	None
 *
-*/
void show_ubs( int hose, int bus, int slot, int func ) 
{
    struct ub *ub;
    struct pb *pb;
    int i; 

    krn$_wait( &ubs_lock );
    shellsort( ubs, num_ubs, strcmp_ub );
    for ( i = 0; i < num_ubs; i++ ) {
	ub = ubs[i];
	pb = ub->pb;
	if ( pb ) {
	    if ( ( pb->hose == hose ) && ( pb->bus == bus ) &&
		 ( pb->slot == slot ) && ( pb->function == func ) ) {
		printf( "%35s%-20.20s%   %-20.20s\n","",
			ub->inode->name, ub->info );
	    }
	}
    }
    krn$_post( &ubs_lock );
}


/*+
 * ============================================================================
 * = show_bcache - Display Bcache size information                            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will display the size of the Bcache.
 *
 * FORM OF CALL:
 *
 *	show_bcache( id );
 *                    
 * RETURNS:
 *         
 *	None.
 *
 * ARGUMENTS:
 *
 *	int id - Processor identifier
 *
 * SIDE EFFECTS:
 *
 *	None.
-*/
void show_bcache( int id )
{
    struct impure *impure_ptr;
    unsigned __int64 write_many_csr;
    int bc_enable;
    int bc_size = 0;

    impure_ptr = ( struct impure * )( PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE + ( id * PAL$IMPURE_SPECIFIC_SIZE ) );
    write_many_csr = *( unsigned __int64 * )( impure_ptr->cns$write_many );
    bc_enable = ( int )( ( write_many_csr >> 30 ) & 0x01 );
    if ( bc_enable ) {
	bc_size = ( int )( ( write_many_csr >> 32 ) & 0x0F );
	if ( bc_size == 1 )
	    bc_size = 2;
	else if ( bc_size == 3 )
	    bc_size = 4;
	else if ( bc_size == 7 )
	    bc_size = 8;
	else if ( bc_size == 8 )
	    bc_size = 16;
	else
	    bc_size = 0;
	printf( "\t\tBcache size: %d MB\n\n", bc_size );
    }
    else {
	printf( "\t\tBcache is disabled\n\n" );
    }
}
