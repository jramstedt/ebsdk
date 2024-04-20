/* file:	powerup_lx164.c
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
 *      Platform specific power up process.
 *
 *  AUTHORS:
 *
 *     Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *      20-Nov-1996
 *
 *  MODIFICATION HISTORY:
 *      
 *	ER	15-Sep-1998	Initialize HWRPB before probing I/O buses.
 *	ER	18-Feb-1997	Remove call to pci_size_config(), add call to probe_io().
 *--
 */
/* $INCLUDE_OPTIONS$ */
#include	"cp$inc:platform_io.h"
/* $INCLUDE_OPTIONS_END$ */
#include	"cp$src:kernel_def.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include 	"cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:prdef.h"
/* Platform specfic include files */
#include        "cp$src:platform.h"
#include	"cp$src:pyxis_main_csr_def.h"

/*Global variables*/
int _align (QUADWORD) cpu_mask;
int main_shell_pid; 
int screen_mode = 0;
int HWRPB;

/*External references*/
extern struct SEMAPHORE scs_lock;

extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
extern unsigned int num_pbs_all;

extern struct SEMAPHORE ubs_lock;
extern struct ub **ubs;
extern unsigned int num_ubs;
extern unsigned int num_ubs_all;

extern struct SEMAPHORE sbs_lock;
extern struct sb **sbs;
extern unsigned int num_sbs;
extern unsigned int num_sbs_all;

extern int cpip;			/* cpu powerup in progress */
extern spl_kernel;
extern int shell_stack;
extern null_procedure();
extern system_reset_or_error();
extern sh();
extern shell_startup();
extern struct TTPB *console_ttpb;	/* pointer to primary console port */
extern struct TTPB serial_ttpb;          

extern void probe_io( void );
extern int sys_environment_init();
extern void sys_irq_init( void );

/* Global variables needed only to build */
int _align (LONGWORD) robust_mode =  0;	/* init to robust mode */


/*+
 * ============================================================================
 * = Powerup - processor specific powerup process                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Powerup process.  This process is called created from krn$_idle.
 *	It is intended to be machine specific. The following are the
 *	requirements for the powerup process.
 *
 *	Startup driver phase 2
 *	Call routine memconfig() to configure memory.  
 *	Startup driver phase 3 tt_init should be included in phase 3
 *	Call routine switch_to_tty() to enable tt driver.  
 *	Startup driver phase 4
 *	Set up the cpu mask    
 *	Call routine build_slot () to build HWRPB slot
 *	Create entry process   
 *	Init the systems class drivers
 *	Startup driver phase 5 if not in robust mode
 *	Create the main shell  
 *	Post the entry process  
 *	Set cpip flag.  
 *  
 * FORM OF CALL:
 *  
 *	powerup()
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void powerup( void ) 
{
    struct SEMAPHORE sem;
    int status;
    char **shv;
    int  id;
    static int shc;
    char name[16];
    unsigned __int64 BaseCSR = pyxis_k_main_csr_base;

    id = whoami();

    krn$_seminit( &scs_lock, 1, "scs_lock" );
    krn$_seminit( &pbs_lock, 1, "pbs_lock" );
    krn$_seminit( &sbs_lock, 1, "sbs_lock" );
    krn$_seminit( &ubs_lock, 1, "ubs_lock" );
    num_pbs = 0;
    num_pbs_all = 32;
    pbs = dyn$_malloc( sizeof( struct pb * ) * 32, DYN$K_SYNC | DYN$K_NOOWN, 0, 0, ( struct ZONE * )0 );
    num_sbs = 0;
    num_sbs_all = 32;
    sbs = dyn$_malloc( sizeof( struct sb * ) * 32, DYN$K_SYNC | DYN$K_NOOWN, 0, 0, ( struct ZONE * )0 );
    num_ubs = 0;
    num_ubs_all = 32;
    ubs = dyn$_malloc( sizeof( struct ub * ) * 32, DYN$K_SYNC | DYN$K_NOOWN, 0, 0, ( struct ZONE * )0 );

    sys_irq_init( );
/*
** Kernel powerup sequence is 0xFF to 0xF0
** Driver powerup sequence is 0xEF to 0xE0
*/
    qprintf( "\n" );
    PowerUpProgress( 0xEF, "Configuring memory\n" );
    memconfig( );
/*
** Locate the HWRPB
*/   
    HWRPB = 0x2000;
    build_hwrpb( ( struct HWRPB *)HWRPB );
    build_bitmap( ( struct HWRPB *)HWRPB ); 
/*
** Dismiss any pending errors
*/
    WRITE_IO_CSR( CSR_PYXIS_ERR, 0xFFFF );
    mb( );

    PowerUpProgress( 0xEE, "Starting phase 1 drivers\n" );
    ddb_startup( 1 );	 

    PowerUpProgress( 0xED, "Configuring the I/O sub-system\n" );
    probe_io( );

/*  qprintf( "Startup driver phase 2\n" ); */
/*  ddb_startup( 2 ); */

    PowerUpProgress( 0xEC, "Starting phase 3 drivers\n" );
    ddb_startup( 3 );

    PowerUpProgress( 0xEB, "Switching stdin/out/err from nl to tt\n" );
    switch_to_tty( );

    PowerUpProgress( 0xEA, "Starting phase 4 drivers\n" );
    ddb_startup( 4 );

    PowerUpProgress( 0xE9, "Building this processor's slot in the HWRPB\n" );
    spinlock( &spl_kernel );
    cpu_mask |= ( 1 << id );
    spinunlock( &spl_kernel );
    build_slot( ( struct HWRPB * )HWRPB, id );

    hwrpbtt_init( id );

    sys_environment_init( );

    PowerUpProgress( 0xE8, "Initializing the SCSI class driver\n" );
    scsi_init( );

#if !( LXUPDATE )
    if ( !robust_mode )
#endif
    {
	PowerUpProgress( 0xE7, "Starting phase 5 drivers\n" );
	ddb_startup( 5 );
    }

    WRITE_IO_CSR( CSR_PYXIS_ERR, 0xFFFF );
    mb( );

    qprintf( "\n" );
    show_version( );
	
    krn$_seminit( &sem, 0, "entry_done" );
    krn$_create (
	system_reset_or_error,
	null_procedure,
	&sem,
	3,
	1 << id,
	0,
	"entry",
	"tt", "r",
	"tt", "w",
	"tt", "w",
	0
    );
    status = krn$_wait( &sem );
    krn$_semrelease( &sem );

    shv = malloc( 2 * sizeof ( char * ) );
    shv[0] = mk_dynamic( "shell" );
    shv[1] = NULL;
    main_shell_pid = krn$_create (
	sh,			/* address of process		*/
	shell_startup,		/* startup routine		*/
	0,			/* completion semaphore		*/
	3,			/* process priority		*/
	1 << id,		/* cpu affinity mask		*/
	shell_stack,		/* stack size			*/
	shv[0],			/* process name			*/
	"tt", "r",
	"tt", "w",
	"tt", "w",
	1, shv
    );

    spinlock( &spl_kernel );
    cpip &= ~( 1 << id );
    spinunlock( &spl_kernel );
}
