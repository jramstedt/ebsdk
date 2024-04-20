/* file:    show_config_lx164.c
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
 *	20-Nov-1996
 *
 *  MODIFICATION HISTORY:
 *
 *	ER	18-Feb-1997	Changed show_pci_config() and show_ubs() for 
 *				PROBE_IO interface
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
#include	"cp$src:apc.h"

/*
** External variable declarations
*/
extern struct SEMAPHORE ubs_lock;
extern unsigned int num_ubs;
extern struct ub **ubs;
extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;

/*
** External function prototype declarations
*/
extern int printf( );
extern strcmp_ub( struct ub *ub1, struct ub *ub2 );

extern int show_isa_config( );	/* isacfg.c */
extern showmem( int argc, char *argv[] );
extern unsigned int io_get_asic_revision( void );
extern void show_srom_version( void );

/*
** Local function prototype declarations
*/
void show_firmware( void );
void show_core_system( void );
void show_pci_config( unsigned int hose );
void show_ubs( int hose, int bus, int slot, int func );

/*+
 * ============================================================================
 * = show config - Display the system configuration		              =
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
    /* Display the PAL and SRM firmware revisions */
    show_firmware( );

    /* Display the CPU and core logic information */
    show_core_system( );

    /* Display the memory configuration	*/
    printf( "\nMEMORY\n" );
    showmem( argc, argv );

    printf( "\n" );

    /* Display the PCI configuration */
    show_pci_config( 0 );

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
    struct HWRPB *hwrpb_ptr = ( struct HWRPB * )PAL$HWRPB_BASE;
    struct DSRDB *dsrdb_ptr;

    dsrdb_ptr = ( int )hwrpb_ptr + hwrpb_ptr->DSRDB_OFFSET[0];
    evp = &evn;
    ev_read( "oem_string", &evp, 0 );
    if ( strlen( evp->value.string ) != 0 )
	printf( "%24s%s\n", "", evp->value.string );
    else
	printf( "%24s%s\n", "", ( int * )( ( int )dsrdb_ptr + dsrdb_ptr->NAME_OFFSET[0] + 8 ) );

    evp = &evn;
    ev_read( "pal", &evp, EV$K_SYSTEM );
    printf( "\nSRM Console %c%d.%d-%-10d\t%s\n",
	    v_variant,
	    v_major,
	    v_minor,
	    v_sequence,
	    evp->value.string
    );
    show_srom_version( );
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

    int type, rev;
    char chip_name[8];

    rev = ( ( int * )( PAL$IMPURE_BASE + IMPURE$CPU_TYPE_OFFSET ) )[0];
    type = ( ( int * )( PAL$IMPURE_BASE + IMPURE$CPU_TYPE_OFFSET + 4 ) )[0];
/* 
** If the revision level isn't legal set it to "?" ('0' + 0xf) 
*/
    if ( rev <= 0 || rev > 6 ) {
	    rev = 0xf;
    } 
    switch ( type ) {
	    case 2:
		strcpy( chip_name, "21064" );
		break;	
	    case 4:
		switch ( rev ) {
		    case 0:
		    	strcpy( chip_name, "?????" );
			break;
		    case 1:
		    case 2:
		    	strcpy( chip_name, "21066" );
			break;
		    case 3:
		    case 4:
		    	strcpy( chip_name, "21068" );
			break;
		    case 5:
		    	strcpy( chip_name, "21066A" );
			break;
		    case 6:
		    	strcpy( chip_name, "21068A" );
			break;
		}
		break;
	    case 5:
		strcpy( chip_name, "21164" );
		break;
	    case 6:
		strcpy( chip_name, "21064A" );
		break;
	    case 7:
		strcpy( chip_name, "21164A" );
		break;
	    case 8:
		strcpy( chip_name, "21264" );
		break;
	    case 9:
		strcpy( chip_name, "21164PC" );
		break;
	    default:
		strcpy( chip_name, "?????" );
	}

    printf( "Alpha %s-%c\n", chip_name, '0' + rev );
#if LX164
    printf( "Pyxis ASIC Pass %d\n", io_get_asic_revision( ) );
#endif
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
 *	None
 *
 * ARGUMENTS:
 *
 *   	unsigned int hose - Hose currently being sniffed
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

    if ( hose > MAX_HOSE ) {
    	return;
    }
    max_bus = 0;
    for ( bus = 0;  bus <= max_bus;  bus++ ) {
	if ( bus != 1 ) { /* Bus 1 is reserved for EISA/ISA */
	    printf( "\nPCI Bus \n" );
	    for ( slot = 0;  slot <= 31 ;  slot++ ) {
		max_func = 0;
		for ( func = 0;  func <= 7;  func++ ) {
		    pb = get_matching_pb( hose, bus, slot, func, 0 );
		    if ( pb ) {
			if ( pb->type == TYPE_PCI ) {
			    if ( func == 0 ) {
				if ( incfgb( pb, 0x0E ) & 0x80 ) {
				    max_func = 7;
				}
			    }
			    if ( max_func ) {
				printf( "     Bus %02d  Slot %02d/%d: ", bus, slot, func );
			    }
			    else {
				printf( "     Bus %02d  Slot %02d: ", bus, slot );
			    }
			    printf( "%s\n", pb->hardware_name );
			    if ( incfgw( pb, 0x0A ) == 0x0604 ) {
				pdm = pb->pdm;
				pbm = pdm->pbm;
				if ( max_bus < pbm->sub_bus ) {
				    max_bus = pbm->sub_bus;
				}
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
