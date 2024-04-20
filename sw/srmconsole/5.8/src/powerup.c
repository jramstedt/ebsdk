/* file:	powerup.c
 *
 * Copyright (C) 1991 by
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
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      Powerup
 *
 *  AUTHORS:
 *
 *      Kevin LeMieux
 *
 *  CREATION DATE:
 *  
 *      20-May-1991
 *
 *  MODIFICATION HISTORY:
 *
 * 	mr   	11-oct-1995	mod #if to not go to ARC on a firmware update
 * 	pas  	15-sep-1995	fix #if for sbmin blds
 * 	pas	13-sep-1995	remove KBD code and move to specific files
 *	mc	01-Mar-1993	add OCP message for SABLE
 *
 *	pel	15-Jan-1993	Conditionalize for morgan
 *
 *      cbf     30-Dec-1992     add conditional compiles around secure features
 *
 *      cbf     23-Dec-1992     add secure features
 *
 *	hcb	08-Dec-1992	Force mem_size to 8MB in robust mode.
 *
 *	pel	01-Dec-1992	Don't build FRU table if in robust mode.
 *
 *	pel	30-Nov-1992	Let build_fru_table routine do some of the
 *				checking to see if fru table should be built.
 *
 *	pel	03-Nov-1992	call build_fru_table if all nodes on all active
 *				storage buses have been polled and fru table
 *				not already built.  boot can occur after this.
 *				This is necessary in case of robust mode where
 *				pkt/put_poll may not build the fru table.
 *
 *	jad	09-Sep-1992	Move switch_to_tty to tt_driver.
 *
 *	ajb	14-Jul-1992	Add JENSEN support
 *
 *	pel	24-Jun-1992	Moved set of screen_mode via read TOY:10 to
 *				krn$_idle; was in powerup procedure.
 *				
 *	pel	24-Apr-1992	move reset_io_buses from here to krn$_idle
 *
 *	pel	19-Mar-1992	Change stdin/out/err channels of powerup
 *				process from nl to tt after tt driver inited.
 *				
 *
 *	ajb	28-Feb-1992	Major overhaul to migrate functionality out of
 *				idle process into here.
 *
 *	pel	17-Dec-1991	reset io buses in powerup procedure so that 
 *				the reset signal can be held for the correct
 *				amount of time.
 *
 *	pel	27-Nov-1991	add call to calib_micro_delay.
 *
 *	pel	30-Oct-1991	chg powerup to not wait for lbus reset & do
 *				tt_init. tt_init done from filesys_init1 now.
 *
 *	pel	30-Oct-1991	chg powerup stdout/err to tt instead of nl
 *				after tt_init if primary. Replace stdin/out/err
 *				tta0 channels w/ tt for spawned processes.
 *	pel	25-Oct-1991	do tt_init only for cobra, cb_proto & pele
 *				otherwise do from krn$_idle. Seems to cure
 *				tt_tx_interrupt crash on 1st char typed.
 *	pel	25-Oct-1991	do tt_init only if primary cpu.
 *	pel	24-Oct-1991	wait on bus_reset sem if cobra/cb_proto/pele
 *	phk	25-Sep-1991	add powerup script exit status check for FBE 
 *	phk	20-Sep-1991	add compilation switches for FBE target
 *	pel	17-Sep-1991	remove powerup script call for ADU target
 *	phk	13-Sep-1991	add powerup script for FBE target
 *	pel	22-Aug-1991	for PELE, COBRA, ADU, run powerup script
 *
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$src:prdef.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:tt_def.h"
#include	"cp$src:ev_def.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:mem_def.h"
#include	"cp$src:impure_def.h"
#include	"cp$src:alphascb_def.h"
#include	"cp$inc:kernel_entry.h"
#include	"cp$src:platform_cpu.h"
#include	"cp$src:pb_def.h"

#if SABLE
#include	"cp$src:sable_ocp.h"
#include	"cp$src:t2_def.h"
#endif

#if PC264
#include	"cp$src:arc_scb_def.h"
#if !WEBBRICK && !API_PLATFORM
#include	"cp$src:sable_ocp.h"
#endif
#endif

/*Constants*/
#define pwrup_script "pwrup"

#if SECURE
/* --- for secure */
extern  int secure;
extern  int set_secure();
#endif
  
extern	unsigned __int64 mem_size;
extern	int cpip;	/* cpu powerup in progress */
extern	int shell_stack;
extern  unsigned int		*scb;
extern	show_version ();
extern	system_reset_or_error ();
extern 	sh();
extern	shell_startup ();
extern 	entry();
extern 	struct LOCK spl_kernel;
extern 	null_procedure();
extern  rxready ();
extern  struct window_head config;
extern int 	hwrpb;

int cpu_mask;
int main_shell_pid;

#if PC264
extern int arc_to_srm( void );
#endif

#if GALAXY
extern int galaxy_partitions;
extern int galaxy_io_mask;
extern int galaxy_node_number;
#endif

/*
 * Robust mode is a cell that determines if the console
 * is run in a robust mode or not.
 */
int	_align (LONGWORD) robust_mode = 0;	/* init to non robust mode */

/*
 * Screen mode is a cell that determines if the powerup screen
 * is run at powerup time.
 */
#if SABLE
#else
int	screen_mode = 0;	/* init to no screen mode */
#endif

/*
   Powerup will vary from platform to platform. On Laser, we come up with each
   CPU running independently out of Bcache. In this mode, they essentially are all
   primaries. Various diagnostics are run in this mode including CPU selftest,
   memory interaction tests and multiprocessor tests. The file system must be
   initialized in stages to accomodate the various diagnostic stages. After the
   Bcache mode diagnostics have run, all CPU's then begin executing SMP mode from
   a common image in memory. At this point various SMP mode diagnostics run.

   Many details surrounding powerup have yet to be worked out and this will no
   doubt change considerably.

   The Laser Bcache powerup mode is simulated (somewhat anyway) by 
   having two CPU's execute independent images in memory.
*/

/*+
 * ============================================================================
 * = Powerup - processor specific powerup process                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Powerup process.  This process does a lot of processor specific startup
 *	and controls the powerup screen.
 *  
 * FORM OF CALL:
 *  
 *	powerup ()
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
void powerup() {
    struct SEMAPHORE sem;
#if SABLE
    extern int dsr_check[3];
    extern struct QUEUE tt_pbs;
    struct TTPB *ttpb;
#endif
#if SABLE && INTERACTIVE_SCREEN
    int found = 0;
    extern pscreen ();
#endif
    struct EVNODE ev, *evp;
    struct PCB *pcb;
    int id;
    int i;
    char name[16];
    int status;
    char **shv;
    struct SEMAPHORE pscreen_done;
    struct FILE *iic_file;
    unsigned char ocp2_val;
    int ev5p2, ev5p4, ev5pother, ev56, *p;
    struct SLOT *slot;
    struct HWRPB *hwrpb_ptr;
#if GALAXY
    char ev_string[EV$K_LENGTH];
#endif

#if SABLE
    extern int sable_ocp_write (struct FILE *fp, int size, int number, unsigned char *buf);
    char ocp_string[16];
    struct FILE *fp;
    unsigned __int64 p_io_csr;
    unsigned __int64 two = 2;
    unsigned __int64 three = 3;
    int t3_installed;
#endif

#if (PC264 && (!WEBBRICK || !API_PLATFORM))
    extern int sable_ocp_write (struct FILE *fp, int size, int number, unsigned char *buf);
    char ocp_string[16];
    struct FILE *fp = 0;
#endif

	id = whoami();
	if (primary ()) {

	    /*
	     * start any drivers needed to probe
	     */
	    ddb_startup (1);

	    /*
	     * memory data structures that must be defined before the powerup
	     * screen
	     */
	    mem_size = 0;

	    /*  
	     * next phase of driver initialization
	     */
	    ddb_startup (2); /* iic driver */
	 
#if GALAXY && REGATTA
	    if (get_console_base_pa()) {
		int i,j;
		char s[80];
		INT data,addr;

		/* Copy the configuration information from the primary instance */
		for (addr=0x40; addr<=0x80; addr+=8) {
		    ldqp(&addr,&data);
		    *(INT *)addr = data;
		}

		addr = 0x40;
		for (galaxy_node_number=0;
			galaxy_node_number<4;
			galaxy_node_number++) {
		    ldqp(&addr,&data);
		    if (data == get_console_base_pa()) 
			break;
		    addr+=8;
		}
     
		addr+=8;
		ldqp(&addr,&data);
		mem_size = data - get_console_base_pa();

		cpu_mask |= (1 << whoami());
	    }
	    if (get_console_base_pa()) {    /* We're "the" secondary instance */
		galaxy_partitions = 2;      /* Clipper is bounded */

	    } else {
		/* check for galaxy on the primary */
		if (nvram_read_sev("lp_count", ev_string) != msg_failure) {
		    galaxy_partitions = atoi(ev_string);
		} else {
		    galaxy_partitions = 0;
		}
	    }

	    if (get_console_base_pa() == 0) {   /* primary instance */
		if (galaxy_partitions > 0) {
		    char *s;

		    /* figure out our iod mask */
		    if (get_console_base_pa() == 0) {       
			s = "lp_io_mask0";      /* first instance */
		    } else {
			s = "lp_io_mask1";      /* second one */
		    }
		    if (nvram_read_sev(s, ev_string) != msg_failure) {
			galaxy_io_mask = xtoi(ev_string);
			if (galaxy_io_mask == 0)
			    galaxy_io_mask = 0xff;
		    } else {
			galaxy_io_mask = 0xff;
		    }

		} else {
		    galaxy_io_mask = 0xff;
		}
		*(UINT *)(PAL$IRQ_FWD_FLAG) = 0;/* clear the */
						/* "forward interrupt" flag */
		*(UINT *)(PAL$IRQ_VECT) = 0;    /* clear the vector */

	    } else {    /* secondary instance */
		UINT io_mask_addr, temp;
		int inst0_galaxy_io_mask;
		uint64 addr, data;

		addr = PAL$IRQ_FWD_FLAG;
		data = 1;		/* set the "forward interrupt" flag */
		stqp(&addr, &data);
		addr = PAL$IRQ_VECT;
		data = 0;		/* clear the vector */
		stqp(&addr, &data);	

		io_mask_addr = &galaxy_io_mask;
		ldqp_u(io_mask_addr, &temp);
		inst0_galaxy_io_mask = temp;
		
		galaxy_io_mask = (~inst0_galaxy_io_mask);
		pprintf("galaxy_io_mask: %08x\n", galaxy_io_mask);
	    }

#endif

	    memconfig();	/* size system, run diagnostics */

#if SABLE || (PC264 && (!WEBBRICK || !API_PLATFORM))
	strcpy(ocp_string, "Probe I/O subsys");
	sable_ocp_write(fp, strlen(ocp_string), 1, ocp_string);
#endif
	    /*
	     * probe the system I/O
	     */
	    PowerUpProgress(0xcf, "probe I/O subsystem\n");
#if M1535C || M1543C
		SouthBridgeDeviceInit();
#if NAUTILUS
		NBridgeSetGartSize();
#endif
#endif

	    probe_io ();

#if CPQPHPC
	    /*
	    ** Cause a system reset if the PCI hot-plug bus(s) need it.
	    */
	    cpqphpc_sys_reset();
#endif
	    /*  
	     * next phase of driver initialization
	     */
	    PowerUpProgress(0xce, "starting drivers\n");
	    ddb_startup (3);	/* tt_init */
	    null_flush ();

	    /* change stdin/out/err channels from nl to tt */
	    switch_to_tty ();

	    /*
	     * report errors detected by micro controllers or SROM
	     * during powerup
	     */
	    report_powerup_errors( );
#if SABLE
	    /*  
	     * Stop a Sable image from running on a Lynx and vice versa
	     */
	    p_io_csr = SIO_CSR_BASE;
	    t3_installed = 0;
	    if (((_READ_IO_CSR( IOCSR_ADDR ) >> 33) & 7) >= 4 )
		t3_installed = 1;
#if LYNX
	    if (!t3_installed) {
		err_printf("\n*** Error, incorrect console for AlphaServer 2100\n");
		while (1);
	    }
#else
	    if (t3_installed) {
		err_printf("\n*** Error, incorrect console for AlphaServer 2100A\n");
		while (1);
	    }
#endif
#endif
	    /*  
	     * next phase of driver initialization
	     */
	    ddb_startup (4);

	    /*
	     * start secondaries
	     */
	    if (!robust_mode)
		start_secondaries ();
	}

      /* Com flow error messages printed to OCP and event log */

#if SABLE
        i=0; 
	ttpb = tt_pbs.flink;
	  while (ttpb != &tt_pbs.flink && i<2) {
	    dsr_check[i]=0;
	    ttpb->txready(i);
	    if( dsr_check[i] == 1) {
	      qprintf("Warning: COM%d device not found\n",i+1);
	      sprintf(ocp_string, "COM%d Dev Err",i+1); 	
	      sable_ocp_write(fp, strlen(ocp_string), 1, ocp_string);
	    }
	    i++;
	    ttpb = ttpb->ttpb.flink;
	  }
#endif

	/*
	 * Build this processor's slot in the hwrpb
	 */
	spinlock (&spl_kernel);
	cpu_mask |= (1 << id);
	spinunlock (&spl_kernel);
	build_slot (hwrpb, id);
#if !SBMIN
	hwrpbtt_init (id);
#endif

	if (primary ()) {

	    /*Start level 5 drivers*/
	    if (!robust_mode) {
#if !SABLE
                build_fru_table ();
#endif

#if SABLE && !SBUPDATE 
		if (!halt_switch_in () && (rtc_read (0x3f) == 0x01)) {
		    evp = &ev;
		    ev_read ("full_powerup_diags", &evp, 0);
		    if (strcmp_nocase (evp->value.string, "OFF") == 0) {
			arc ();
		    }
		}
#endif

#if ISP_MODEL_SUPPORT
		if (platform() != ISP_MODEL)
#endif
		    ddb_startup (5);

		/*
		 * Run the powerup script by spawning a shell process
		 * which will take its stdin from a powerup script
		 */

		spinlock (&spl_kernel);
		cpip &= ~(1 << id);
		spinunlock (&spl_kernel);

		shv = malloc (2 * sizeof (char *));
		shv [0] = mk_dynamic ("shell");
		shv [1] = 0;
		krn$_seminit (&sem, 0, "pwrup_done");
		krn$_create (
		    sh,			/* address of process		*/
		    shell_startup,	/* startup routine		*/
		    &sem,		/* completion semaphore		*/
		    3,			/* process priority		*/
		    1<<id,		/* cpu affinity mask		*/
		    shell_stack,	/* stack size			*/
		    "pwrup_diag",	/* process name			*/
		    pwrup_script, "r",	/* stdin 			*/
		    "tt", "w",
		    "tt", "w",
		     1, shv
		);
		status = krn$_wait (&sem);
		krn$_semrelease (&sem);

		spinlock (&spl_kernel);
		cpip |= (1 << id);
		spinunlock (&spl_kernel);

#if SABLE
        	/*
        	 * Build the FRU table.
        	 *  For SABLE: (the Fru Table is build immediately and DOES NOT
        	 *  wait for IO bus devices to init.
		 *  For FRU V5 platforms this routine should call gct, update
		 *  the FRU and Reserved for Hardware offsets in the HWRPB,
		 *  and recalculate the HWRPB checksum.
        	 */
                build_fru_table ();
#endif
            }

#if GAMMA
	    ev5p2 = 0;
	    ev5p4 = 0;
	    ev5pother = 0;
	    ev56 = 0;

	    p = PAL$IMPURE_BASE + IMPURE$CPU_TYPE_OFFSET;

	    for( i = 0; i < MAX_PROCESSOR_ID; i++ )
		if( ( 1 << i ) & cpu_mask )
		    switch( p[i*2+1] )
			{
			case 5:
			    switch( p[i*2+0] )
				{
				case 1:
				    ev5p2 = 1;
				    break;

				case 5:
				    ev5p4 = 1;
				    break;

				default:
				    ev5pother = 1;
				    break;
				}
			    break;

			case 7:
			    ev56 = 1;
			    break;

			default:
			    ev5pother = 1;
			    break;
			}

	    /* if mixed ev5 pass 2 and ev5 pass 4 tell VMS pass 3 only */ 
	    if( ( ev5p2 && ev5p4 ) && !( ev5pother || ev56 ) )
		{
		evp = &ev;
		ev_read( "os_type", &evp, 0 );
		if( ( evp->value.string[1] == 'M' )
			|| ( evp->value.string[1] == 'p' ) )
		    {
		    hwrpb_ptr = hwrpb;
		    for( i = 0; i < MAX_PROCESSOR_ID; i++ )
			{
			slot = (int)hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET + 
				i * sizeof( struct SLOT );
			if( slot->CPU_TYPE[1] )
			    slot->CPU_TYPE[1] = 3;
			}
		    }
		}

	    /* if all ev5 pass 4 or ev56 do fixup */
	    if( ( ev5p4 || ev56 ) && !( ev5p2 || ev5pother ) )
		cserve( CSERVE$DO_PASS4_FIXUPS );
#endif

	    show_version ();
	}

	spinlock (&spl_kernel);
	cpip &= ~(1 << id);
	spinunlock (&spl_kernel);

	if (primary ()) {
	    if (halt_switch_in ()) {
		printf (msg_no_auto_action);
	    } else {
		if (!robust_mode) {
#if ( SABLE  && !SBUPDATE )
		    if (rtc_read (0x3f) == 0x01) {
			arc ();
		    }
#endif
#if GLXYTRAIN
		    /* check for fsb mode Simon Lee */
		    FsbClearPassword();
#endif
#if PC264 && !FWUPDATE
		    if ( !arc_to_srm( ) && ( rtc_read( 0x3f ) == 0x01 ) ) {
			arc( );
		    }
#endif
		}
		krn$_seminit (&sem, 0, "entry_done");
		krn$_create (
		    system_reset_or_error,
		    null_procedure,
		    &sem,
		    3,
		    1<<id,
		    6 * 1024,
		    "entry",
		    "tt", "r",
		    "tt", "w",
		    "tt", "w",
		    0, 0, 0
		);
		status = krn$_wait (&sem);
		krn$_semrelease (&sem);
	    }

#if SABLE && INTERACTIVE_SCREEN
	/*Look for a vga*/
	    spinlock (&spl_kernel);
	    ttpb = tt_pbs.flink;
	    spinunlock (&spl_kernel);
	    while (ttpb != &tt_pbs.flink) {
		if(!strcmp(ttpb->ip->name,"vga0")) {
		    found = 1;
		break;    
	    	}             
		ttpb=ttpb->ttpb.flink;   	  /* next queue entry */
	    }

	/*If we've found create a screen*/
	    if(found){
		shv = malloc (sizeof (char *) * 5);
		shv [0] = mk_dynamic ("pscreen");
		shv [1] = NULL;
		krn$_create (pscreen,      	/* address of process		*/
		    null_procedure,		/* startup routine		*/
		    0,		 		/* completion semaphore		*/
		    5,				/* process priority		*/
		    1<<id,			/* cpu affinity mask		*/
		    0,				/* stack size			*/
		    "pscreen",			/* process name			*/
		    "vga0", "r",
		    "vga0", "w",
		    "vga0", "w",
		    1, shv
		    );
	    }

	/*If we aren't the vga console start a shell*/
	    evp = &ev;
	    ev_read("console",&evp,0);
	    if(!strncmp_nocase(evp->value.string,"SERIAL",6)){
	        shv = malloc (2 * sizeof (char *));
	        shv [0] = mk_dynamic ("shell");
	        shv [1] = NULL;
	        main_shell_pid = krn$_create (
		    sh,			/* address of process		*/
		    shell_startup,	/* startup routine		*/
		    0,			/* completion semaphore		*/
		    3,			/* process priority		*/
		    1<<id,		/* cpu affinity mask		*/
		    shell_stack,	/* stack size			*/
		    shv [0],		/* process name			*/
		    "tt", "r",
		    "tt", "w",
		    "tt", "w",
		    1, shv);
	    }
#else

	/*Start the shell*/
	    shv = malloc (2 * sizeof (char *));
	    shv [0] = mk_dynamic ("shell");
	    shv [1] = NULL;
	    main_shell_pid = krn$_create (
		sh,			/* address of process		*/
		shell_startup,		/* startup routine		*/
		0,			/* completion semaphore		*/
		3,			/* process priority		*/
		1<<id,			/* cpu affinity mask		*/
		shell_stack,		/* stack size			*/
		shv [0],		/* process name			*/
		"tt", "r",
		"tt", "w",
		"tt", "w",
		1, shv
	    );
#endif
	}

#if NT_ONLY_SUPPORT
	ev_delete("srm_boot", 0);
#endif

#if GALAXY
    if (!primary() && (galaxy_partitions > 0)) {

        /* Check if we're init'ing into a booted instance */
        galaxy_check_boot();

        /* Set the run bit and current owner appropriately */
        galaxy_set_run_bit(0);
    }
#endif

#if SECURE
        /*
        ** NOTE: THIS CODE MUST BE EXECUTED LAST  **
        **
        ** if robust_mode, set secure off
        */
	if (primary ()) {
            if (robust_mode) {
               set_secure (0); 
            }
	}
#endif

}

#if GLXYTRAIN
void FsbClearPassword ()
{
	struct pb pb;
	struct EVNODE *evp;

/*	EnableAliPMU ();  */

	pb.hose = 0;
	pb.bus = 0;
	pb.slot = 0x11; /* Pmu ID */
	pb.function = 0;
				
	if ( (incfgb(&pb, 0x97) & 0x3) == 0x3 )   /* control for GPI 0-1 */
	{
		evp = (void *) malloc(sizeof(struct EVNODE));
		ev_read("password", &evp, EV$K_SYSTEM);

		fsb_clear_password();
		set_secure(0);
	}
/*	DisableAliPMU (); */
}
#endif
