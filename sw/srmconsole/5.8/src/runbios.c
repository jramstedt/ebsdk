/* file:	runbios.c
 *
 * Copyright (C) 1993 by
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
 *      Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      Runbios, execute BIOS extension.
 *
 *  AUTHORS:
 *
 *      K. LeMieux
 *
 *  CREATION DATE:
 *
 *  MODIFICATION HISTORY:
 *
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:parse_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:tt_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:vgapb.h"
#include "cp$src:probe_io_def.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#define DEBUG 0

extern struct  VGA_PB *vpb;
extern struct TTPB *console_ttpb;
extern int spl_kernel;
extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
extern unsigned int max_hose;
extern struct ZONE *memzone;

#if SABLE || RAWHIDE || TURBO || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER
extern bios_mchk_handler( unsigned int *flag, int *frame );
extern struct HQE mchq_660;		     	/*MCHECK handler queue*/
extern int MachineCheckFlag;			/* flag for bios emulator */
#if MODULAR
extern struct HQE *mchq = &mchq_660;		/* help with compiler bug */
#endif
#else
extern int ExpectMachineCheck;
#endif
#define BOOTBIOS_OPTION 1
#if BOOTBIOS
extern struct pb *bootbios_pb;
extern int bios_restart_flag;
#endif
extern int ETABiosResident;

#define VGA_CLASS(x) ((x == 0x000100) || (x == 0x030000))

int kbd_not_ready( int dummy );
#if BOOTBIOS
int open_bios ( struct pb *pb );
int open_bios_sub ( struct pb *pb );
#endif
int run_bios( char *name );
show_bios( int argc, char *argv [] );
int bios_rom_info( int hose, char *buf );
exec_bios( struct pb *pb, int perm );
int close_bios ( struct pb *pb );
void run_bios_init();
int bios_target_valid( struct pb *fpb );
void set_bios_target( struct pb *pb );
void clear_bios_target();

extern null_procedure();

extern unsigned __int64 *sg_map;

/* Table of options for which we support booting through the BIOS */
U32 bios_rom_id_table[] = {
    0xB0600E11,		/* Compaq SmartArray ( Maxwell ) */
    0x0
};

/*
 * Holds the target pb and it's parent. Used to hide sister controllers
 * of the target controller.
 */

struct target_bios {
    struct pb *pb;
    struct pb *parent_pb;
} bios_target;

/*+
 *
============================================================================
 * = run_bios_init - init for runbios
=
 *
============================================================================
 *
 * OVERVIEW:
 *
 *  Init various things for run bios. This routine is called by 
 *  by startbiosemulator which can be called on a powerup, reset or "init"
 *  command, or due a forced PCI reset ( for example, a stop -driver
 *  start -driver sequence ).
 * 
 * FORM OF CALL:
 *
 *	run_bios_init()
 *
 * RETURNS:
 *  
 *	none
 *
 * ARGUMENTS:
 *  
 *	none
 *   
 * SIDE EFFECTS:
 *
 *
-*/
void run_bios_init() {
    
#if BOOTBIOS

    /*
     * If a PCI reset has occurred ( reset_hardware ) then close the bootbios.
     */

    if ( bios_restart_flag ) {
	if ( bootbios_pb ) {
	    close_bios( bootbios_pb );
	}
    }
#endif

}


/*+
 *
============================================================================
 * = bios_rom_id_valid - validate bios rom id
=
 *
============================================================================
 *
 * OVERVIEW:
 *
 *  Determine whether this is a BIOS ROM that we can execute. 
 * 
 * FORM OF CALL:
 *
 *	bios_rom_id_valid( id )
 *
 * RETURNS:
 *
 *      TRUE, FALSE
 *
 * ARGUMENTS:
 *  
 *	U32 id - PCI id
 *
 *   
 * SIDE EFFECTS:
 *
 *
-*/
int bios_rom_id_valid( U32 id ) {

    int i;
    int match = FALSE;

    for ( i = 0; ( bios_rom_id_table[i] != 0 ); i++ ) {
	if ( id  == bios_rom_id_table[i] ) {
	    match = TRUE;
	    break;
	}
    }
    return ( match );
}

/*+
 *
============================================================================
 * = kbd_not_ready - noop the console's keyboard driver while BIOS is
running =
 *
============================================================================
 *
 * OVERVIEW:
 *
 * Routine used to keep console's graphics keyboard driver 
 * out of the picture while the BIOS is running.
 * 
 * FORM OF CALL:
 *
 *	kbd_not_ready( dummy )
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *  
 *	int dummy - param to be compatible with actual ready routine
 *
 *   
 * SIDE EFFECTS:
 *
 *
-*/
int kbd_not_ready(int dummy) {
    return (0);
}

#if BOOTBIOS

/*+
 *
============================================================================
 * = open_bios - Open BIOS extension ROM
=
 *
============================================================================
 *
 * OVERVIEW:
 *          
 *      Open BIOS extension ROM to access BIOS services. Create a separate 
 *	process; a large stack is required.
 *
 * FORM OF CALL:
 *  
 *	open_bios( pb )
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 *	struct pb *pb - port block of target adapter
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int open_bios( struct pb *pb ) {

    struct SEMAPHORE complete;

    krn$_seminit( &complete, 0, "open_bios" );
    krn$_create( open_bios_sub, null_procedure, &complete,
		   3, 1 << whoami(), 8192, "open_bios", 0, 0, 0, 0, 0, 0, pb
);
    krn$_wait( &complete );
    krn$_semrelease( &complete );
    return 0;
}

/*+
 *
============================================================================
 * = open_bios_sub - Open BIOS extension ROM
=
 *
============================================================================
 *
 * OVERVIEW:
 *          
 *      Open BIOS extension ROM to access BIOS services.
 *
 * FORM OF CALL:
 *  
 *	open_bios_sub ( pb )
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 *	struct pb *pb - port block of target adapter
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int open_bios_sub ( struct pb *pb ) {

    int save_rxready;
    int status;
    int bios_num;
    int i;
    int match = FALSE;
    unsigned int id;
    int class;
    char display_msg[128];

    if ( pb != NULL ) {


	SetupBiosGraphics();

	/*
	 * Replace graphics keyboard rxready routine so that 
	 * BIOS can have full control of graphics keyboard.
	 */

	spinlock( &spl_kernel );
	save_rxready = vpb->ttpb->rxready;
	vpb->ttpb->rxready = kbd_not_ready;
	spinunlock( &spl_kernel );


	sprintf(display_msg, "\nwaiting for %s to start...\n", pb->name );
	pprintf( display_msg );

	/*
	 * Reset the VGA mode.
	 */

	VGASetMode();

	/*
	 * Get a message onto the graphics display before we disable 
	 * video.
	 */

	EBDisplayString( display_msg );

	/*
	 * Disable video so that BIOS does not prompt to enter configuration
	 * utility.
	 */

	VGADisable( 1 );

	/*
	 * Execute the BIOS.
	 */

	exec_bios( pb, TRUE );

	/*
	 * Enable video.
	 */

	VGADisable( 0 );

	/*
	 * Replace the rxready routine for console use of graphics keyboard.
	 */

	spinlock( &spl_kernel );
	vpb->ttpb->rxready = save_rxready;
	spinunlock( &spl_kernel );

	SetupBiosGraphics();

    }

    return ( msg_success );
}
#endif


/*+
 *
============================================================================
 * = run_bios - Run BIOS extension ROM =
 *
============================================================================
 *
 * OVERVIEW:
 *          
 *	Run BIOS extension ROM
 *
 * FORM OF CALL:
 *  
 *	run_bios ( name )
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 *	char *name - name of controller
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int run_bios( char *name ) {

    int save_rxready;
    int status;
    int bios_num;
    int i;
    int match = FALSE;
    struct HQE hqe;
    int class;
    struct pb *pb;
    struct pb *fpb;
    int hose;

    /*
     * Must have memzone for X86 emulator memory requirements.
     */

    if ( !memzone ) {
	printf("Runbios available only prior to booting.  Reinit system and try again\n");
	return msg_failure;
    }

    pb = pb_name_match( name );

    if ( pb != NULL ) {

#if DEBUG
	pprintf("running bios pb %x\n", pb );
#endif

	/*
	 * Make sure this is not a VGA BIOS. VGA BIOS has already run 
	 * during powerup.
	 */
    
	class = incfgl( pb, 8);
	class >>= 8;
	if (!( VGA_CLASS( class ))) { 

	    stop_drivers();


#if BOOTBIOS

	    /*
	     * If a BIOS is resident then we may not be able to access it's 
	     * extension ROM. For example, when the the DPT BIOS executes, it 
	     * writes it's extension ROM base address register making it 
	     * impossible to read the extension ROM. So not only do we 
	     * have to stop the drivers, we also have to call reset_hardware
	     * which will result in reconfiguration of the controller's PCI 
	     * space. Also the BIOS may make assumptions about being run only
	     * once after the PCI is reset and may write the BIOS data area.
	     */

	    if ( bootbios_pb ) {
		hose = bootbios_pb->hose;
		close_bios( bootbios_pb );
		if ( hose == pb->hose )
		    reset_hardware( 1 );
	    }
#endif

	    /*
	     * Replace graphics keyboard rxready routine so that 
	     * BIOS can have full control of graphics keyboard.
	     */

	    spinlock( &spl_kernel );
	    save_rxready = vpb->ttpb->rxready;
	    vpb->ttpb->rxready = kbd_not_ready;
	    spinunlock( &spl_kernel );

	    /*
	     * Reset the VGA mode.
	     */

	    VGASetMode();

	    /*
	     * Initialize the graphics keyboard for use by the BIOS.
	     */

	    ETAinit_kbd();

	    /*
	     * We need the floppy driver to do firmware updates,
	     * so start that one.
	     */
		
	    krn$_set_console_mode(INTERRUPT_MODE);  /* stop raises IPL */

	    /* restart floppy */

	    fpb = pb_name_match( "dva0" );

	    if ( fpb ) {
/*		pprintf("Restart %s, %s\n", fpb->name, fpb->device); */
		driver_setmode(DDB$K_START, 6, fpb);
	    }
	}

	if ( exec_bios( pb, FALSE ) == msg_success ) 
	    close_bios( pb );
	
	/*
	 * Restart the drivers.
	 * Have to do this because the BIOS may write config space,
	 * may write BIOS data area ( it makes assumptions that the
	 * BIOS only runs once ), and have to restart the drivers 
	 * for the targeted controllers.
	 */

	krn$_set_console_mode( POLLED_MODE );
	start_drivers();

	/*
	 * Replace the rxready routine for console use of graphics keyboard.
	 */

	spinlock( &spl_kernel );
	vpb->ttpb->rxready = save_rxready;
	spinunlock( &spl_kernel );

	/*
	 * Need this to get graphics head to a full blue screen.
	 */

	pprintf("\n");

    }
    return ( msg_success );
}

/*+
 *
============================================================================
 * = exec_bios - Execute BIOS extension ROM
=
 *
============================================================================
 *
 * OVERVIEW:
 *          
 *      Execute BIOS extension ROM.
 *
 * FORM OF CALL:
 *  
 *	exec_bios ( pb, perm )
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 *	struct pb *pb - port block of target adapter
	int perm - allocate permanent - true/false
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int exec_bios( struct pb *pb, int perm ) {

    struct HQE hqe;

    /* 
     * May see machine checks during execution of BIOS. Dismiss them 
     * during the BIOS execution.
     */

#if SABLE || RAWHIDE || TURBO || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER
    hqe.handler = bios_mchk_handler;
#if MODULAR
    insq_lock( &hqe, mchq->blink );
#else
    insq_lock( &hqe, mchq_660.blink );
#endif
#else
    ExpectMachineCheck = 1;
#endif

    /*
     * Execute the BIOS.
     */

    set_bios_target( pb );

    ETAExecBios( pb->hose, pb->bus, pb->slot, pb->function, perm );

    clear_bios_target();

    /*
     * BIOS complete. No longer dismiss machine checks.
     */

#if SABLE || RAWHIDE || TURBO || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER
    remq_lock( &hqe );
#else
    ExpectMachineCheck = 0;
#endif
    return 0;
}


/*+
 *
============================================================================
 * = close_bios - Close BIOS extension ROM
=
 *
============================================================================
 *
 * OVERVIEW:
 *          
 *      Close BIOS extension ROM.
 *
 * FORM OF CALL:
 *  
 *	close_bios ( pb )
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 *	struct pb *pb - port block of target adapter
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int close_bios ( struct pb *pb ) {
#if DEBUG
    pprintf("closing bios pb %x\n", pb );
#endif
    ETACloseBios( pb->hose, pb->bus, pb->slot, pb->function );
    pb = 0;
    return 0;
}

/*+
 *
============================================================================
 * = show bios - Display location of BIOS extension ROM's.
=
 *
============================================================================
 *
 * OVERVIEW:
 *          
 *      Display location of BIOS extension ROM's. Buffer the information and
 *	print only after VGA display reset has completed so the information
 *	remains on the screen.
 *
 *   COMMAND FORM:
 *  
 *   	show bios ()
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
 *~
 * P00>>>show bios
 * resetting all I/O buses
 * 
 * BIOS ROM's on hose 0
 * 
 * vga0.0.0.1.0 - DEC PowerStorm
 * pka0.7.0.2.0 - NCR 53C895
 * pya0.0.0.4.0 - CPQ SmartArray
 * 
 *~
 * FORM OF CALL:                             
 *  
 *	show_bios ()
 *  
 * RETURN CODES:
 *
 *	None.
 *       
 * ARGUMENTS:
 *
 *	none
 *
 * SIDE EFFECTS:
 *
 *      None                                 
 *
-*/
#define LINE_LENGTH 80
int show_bios( int argc, char *argv [] ) {

    int hose = 0;
    int status;
    int i;
    int pb_count = 0;
    char *buf, *sbuf;
    struct pb *pb;
    int bios_rom;

    if ( argc > 2 ) {
	status = common_convert( argv[2], 10, &hose, 4 );
	if ( status ) {
	    err_printf ( status );
	    return status;
	}
    }

    if ( hose <= max_hose ) {
	stop_drivers();

	krn$_wait( &pbs_lock );
	for( i = 0; i < num_pbs; i++ ) {
	    pb = pbs[i];
	    if (( pb->hose == hose ) && ( pb->type == TYPE_PCI ))
		pb_count++;
	}
	krn$_post( &pbs_lock );
	buf = malloc( pb_count * LINE_LENGTH );
	sbuf = buf;
	bios_rom = bios_rom_info( hose, buf );
	start_drivers();
	pprintf("\nBIOS ROM's on hose %d\n\n", hose );
	for ( i = 0; i < bios_rom; i++ ) {
	    printf("%s", &sbuf[ i * LINE_LENGTH ]);
	}
	free( sbuf );
    }
    return 0;
}


/*+
 *
============================================================================
 * = bios_rom_info - gather information for controllers on a given hose
=
 *
============================================================================
 *
 * OVERVIEW:
 *
 *	Gather information for controllers on a given hose.
 * 
 *
 * FORM OF CALL:
 *
 *	bios_rom_info( hose, buf )
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *	int hose - hose number
 *	char *buf - information buffer
 *   
 * SIDE EFFECTS:
 *
 *
-*/
int bios_rom_info( int hose, char *buf ) {

    int i;
    int exp_rom = 0;
    struct pb *pb;
    int base;
    int header;
    int rom_sig = 0;
    int bios_rom = 0;

    krn$_wait( &pbs_lock );
    for( i = 0; i < num_pbs; i++ ) {
	pb = pbs[i];
	if (( pb->hose == hose ) && ( pb->type == TYPE_PCI )) {
	    header = incfgb( pb, 0x0e );
	    if (( header & 0x7f ) == 0 )
		exp_rom = 0x30;
	    if (( header & 0x7f ) == 1 )
		exp_rom = 0x38;
	    if ( exp_rom ) {
		base = incfgl( pb, exp_rom ) & ~1;
		if (( base != 0 ) && ( base != 0xffffffff )) {
		    outcfgl( pb, exp_rom, base | 1 );
		    rom_sig = inmemw( pb, base );
		    outcfgl( pb, exp_rom, base );
		    if ( rom_sig == 0xAA55 ) {
			bios_rom++;
			sprintf( buf, "%s - %s\n", pb->name, pb->hardware_name );
			buf = buf + LINE_LENGTH;
		    }
		}
	    }
	}
    }
    krn$_post( &pbs_lock );
    return ( bios_rom );
}

/*+
 *
============================================================================
 * = bios_support - determine whether this platform supports a BIOS feature
=
 *
============================================================================
 *
 * OVERVIEW:
 *
 *	The X86 object library is compiled without knowledge of platform 
 *	conditionals. This routine provides a test of platform support for
 *	a particular BIOS feature.
 *
 * FORM OF CALL:
 *
 *	bios_support()
 *
 * RETURNS:
 *
 *      int support - TRUE, FALSE
 *
 * ARGUMENTS:
 *
 *	None
 *   
 * SIDE EFFECTS:
 *
 *
-*/
int bios_support( int option ) {
    int support = FALSE;

    if ( option == BOOTBIOS_OPTION ) {
#if BOOTBIOS
	support = TRUE;
#endif
    }
    return ( support );
}
    

/*+
 *
============================================================================
 * = bios_target_valid - determine whether this is a valid PCI config target
=
 *
============================================================================
 *
 * OVERVIEW:
 *
 *	When BIOS ROM's are run they typically attempt to run the BIOS on 
 *	each of their sister controllers. When we execute a BIOS we want to 
 *	run the BIOS only on the target controller. It takes too much time
 *	run the BIOS on all the sister controllers. This routine effectively
 *	hides the sister controllers of the target controller. It allows
 *	PCI configuration cycle access only to the target controller and the
 *	PCI bridge that the target controller may or may not be behind. It 
 *	alllows a special case access for the DPT ( Dorado ) controller.
 *	This controller is unusual in that it is composed of a bridge that 
 *	hosts two PCI slots, one of which is the RAID controller, the other 
 *	a memory controller to serve the RAID controller. We allow the RAID 
 *	controller PCI configuration access to the memory controller.
 *	
 *
 * FORM OF CALL:
 *
 *	bios_target_valid()
 *
 * RETURNS:
 *
 *      int valid - TRUE, FALSE
 *
 * ARGUMENTS:
 *
 *	struct pb *fpb - target pb
 *   
 * SIDE EFFECTS:
 *
 *
-*/
int bios_target_valid( struct pb *pb ) {
    int target_match = FALSE;
    int parent_match = FALSE;
    int dpt_memory_ctrl_match = FALSE;
    int valid = TRUE;

    if ( bios_target.pb ) {
	if ( pb ) {
	    if (( pb->hose == bios_target.pb->hose ) && 
		( pb->bus == bios_target.pb->bus ) &&
		( pb->slot == bios_target.pb->slot ) &&
		( pb->function == bios_target.pb->function )) {
		    target_match = TRUE;
	    }
	    if ( bios_target.parent_pb ) {
		if (( pb->hose == bios_target.parent_pb->hose ) && 
		    ( pb->bus == bios_target.parent_pb->bus ) &&
		    ( pb->slot == bios_target.parent_pb->slot ) &&
		    ( pb->function == bios_target.parent_pb->function )) {
			parent_match = TRUE;
#if 0
			pprintf("parent match h %x b %x s %x f %x\n", 
				 pb->hose, pb->bus, pb->slot,
				 pb->function );
#endif
		}
	    }
	    if (( pb->hose == bios_target.pb->hose ) && 
		( pb->bus == bios_target.pb->bus )) {
		    if ( incfgl( bios_target.pb, 0 ) == 0xA5011044 ) {
			if ( incfgl( pb, 0 ) == 0x10121044 ) {
			    dpt_memory_ctrl_match = TRUE;
#if 0
			    pprintf("dpt mem match h %x b %x s %x f %x\n", 
				     pb->hose, pb->bus, pb->slot,
				     pb->function );
#endif
			}
		    }
	    }
	    if (!( target_match || parent_match || dpt_memory_ctrl_match ))
		valid = FALSE;
	}
    }
    return ( valid );
}

void set_bios_target( struct pb *pb ) {

    bios_target.pb = pb;
    if ( pb->bus ) {
	if ( pb->parent ) {
	    bios_target.parent_pb = pb->parent;
	}
    }
}

void clear_bios_target() {
    bios_target.pb = 0;
    bios_target.parent_pb = 0;
}
