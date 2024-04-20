/* file:        boot.c
 *
 * Copyright (C) 1990, 1993 by
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
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      Boot code
 *
 * Author:
 *
 *     Stephen F. Shirron
 *
 * CREATION DATE:
 *
 *     05-Jul-1990
 *     
 * MODIFICATION HISTORY:
 *
 *	tna     11-Nov-1998     Conditionalized for YukonA
 *
 *	noh	07-Nov-1995	BURNS behavior is now like AVMIN.
 *				Added BURNS conditional.
 *
 *      rhd     27-Oct-1995     Fixed the strncpy for quoted file name, was
 *                              dropping last char.
 *
 *      swt     29-Jun-1995     add Noritake platform.
 *
 *      noh     19-Jun-1995     AVMIN: Removed temporary buffer used by
 *				boot_disk routine. Fixed "quoted strings"
 *				problems.
 *
 *      noh      8-Jun-1995     AVMIN: Reduced size of malloc block used by
 *				boot_disk routine (32K to 1K).
 *
 *      dpm      7-Jun-1995     MIKASA don't allow boots if the console was
 *                              was entered via a control p or halt. Print
 *				a message that the system must be reinitilaized
 *
 *      noh      1-Jun-1995     AVMIN: Changed way filename is handle in MOP
 *                              boots.  Case is not changed for quoted strings,
 *				all others are converted to uppercase.
 *
 *      jje      3-May-1995     Fixed boot_flash() for M224 by fetching flash
 *                              size from the inode rather than assuming 512KB. 
 *
 *	dwb	12-Oct-1994	Add a success return value to boot_sub to
 *				correct a problem with multiple boot devices
 *
 *	bobf	 6-Jul-1994	Medulla change only.  The Module Display
 *				Register is 'blanked' before the boot occurs.
 *
 *      dch     24-May-1994     Medulla change only.  Added count of attempts
 *                              to boot without the P2 connector installed.
 *                              The count is stored in NVRAM.
 *
 *	mdr	11-Apr-1994	Medulla change only.  Check to make sure that
 *				Medulla's P2 connector is installed.
 *
 *	dwr	12-Oct-1993	ECO 6.15: For turbo mp boots, the hwrpb slot
 *				ISN'T emulated. Therefore, don't use id from
 *				whoami().
 *
 *	kl	01-Oct-1993	Call create_device in boot_sub (driver shutdown
 *				support).
 *
 *	kl	10-Sep-1993	OSF boot
 *
 *	kl	19-Aug-1993	merge
 *
 *	phs	22-Jun-1993	Allow pmem boot.
 *
 *      dda     02-Jun-93       Documentation/header work
 *
 *      hcb     31-mar-1993     use rebuild_memdsc and rebuild_crb
 *
 *      cb      05-mar-1993     modify to removed noarg variable
 *     
 *      cbf     05-jan-1993     add secure features
 *
 *	pel	03-Nov-1992	boot command; stall until fru table built which
 *				will be done after all storage buses polled.
 *
 *	kp	27-Jul-1992	Remove KDM70 references in VAX build.
 *
 *	ajb	13-Jul-1992	Don't boot if main memory zone is in use
 *
 *      cjd     10-Jun-1992     Integrated inet (bootp/tftp) boots
 *
 *	sfs	07-May-1992	Allow a list of boot devices.
 *
 *	sfs	25-Mar-1992	Use 'boot_file' if set on input, and set
 *				'booted_file' before exiting
 *
 *	ajb	08-jan-1992	Add -stack qualifier to allow for
 *				variable number of stack pages at boot
 *				time.  Force a default 2 two stack pages.
 *				Always map at least 256kb for the image
 *				that is read in.
 *
 *	ajb	09-Dec-1991	Reflect changes in the slot back into
 *				pal impure area.
 *
 *	ajb	26-Nov-1991	Put stack at end of booted image
 *
 *	phk	20-Nov-1991	Modify boot_network for the new MOPDL driver 
 *				implementation
 *
 *	sfs	14-Nov-1991	Merge in changes for console ECO.  Add Cobra
 *				fixes.
 *
 *	jds	16-Aug-1991	Added XNA as a boot device.  Commented out
 *				pprintfs which occur after tt_driver is disabled.
 *
 *	dtm	 6-Aug-1991	Add PTE for UART address.  Add support
 *				for pmem booting of ram disks.
 *
 *	kl	27-Jun-1991	Cold/warm boots.
 *
 *	kl	7-Jun-1991	Add boot_reset support.
 *
 *	djm	05-Jun-1991	Did some enhancements in the VAX area.
 *				More to come.
 *
 *	dtm	29-May-1991	Add booted_dev support.
 *
 *	ajb	14-may-1991	Unconditionally turn off MMG when booting on VAX
 *
 *	jad 	25-Mar-1991	Allow for control C during boot.
 *
 *	dtm	14-Mar-1991	Boot calls krn$_remove_node to exit 
 *				gracefully.
 *
 *	dtm	 5-Feb-1991	Remove creation of HWRPB, and use the HWRPB
 *				that already exists.  Added -flags qualifier.
 *
 *	sfs	14-Jan-1990	Fix up tape boots and network boots for VAX.
 *
 *	sfs	11-Jan-1990	Make this work again on a VAX.  It will read
 *				VMB from a disk (using the boot block format)
 *				and jump to it.  VMB then does the real work.
 *
 *	sfs	05-Jul-1990	Initial entry.
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:prdef.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:ctype.h"
#include "cp$src:hwrpb_def.h"
#include "cp$src:parse_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:btddef.h"
#include "cp$src:mem_def.h"
#include "cp$src:aprdef.h"
#include "cp$src:impure_def.h"
#include "cp$src:pal_def.h"
#include "cp$inc:platform_io.h"
#include "cp$src:platform_cpu.h"
#include "cp$src:get_console_base.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#if MODULAR
#include "cp$src:stddef.h"
#include "cp$src:ctype.h"
extern int ovly_image_address;
extern int print_debug_flag;
extern lfu_address;
#endif

#if EV6
extern int page_table_levels;
#else
#define page_table_levels 3
#endif

#if SECURE
/* for secure */
extern int secure;
extern void set_secure();
#endif

extern int cb_ref;
extern int boot_cpu_slot;
extern int boot_retry_counter;
extern struct LOCK spl_kernel;
extern int primary_cpu;
extern int dispatch[];
extern int _dispatch_();
extern int fixup[];
extern int _fixup_();
extern struct EVNODE *evmem;
extern char ENV_VAR_TABLE[];
extern char ENV_VAR_TABLE_END[];
#if MEDULLA
#define         MOD_CONFIG_REG          0x2800
#define		MOD_DISPLAY_REG		0x2400
extern struct DDB medullaflash_ddb;
extern struct DDB m224flash_ddb;
extern unsigned char inportb(struct pb *pb, int offset);
char msg_nop2 [] = "\tNo P2 Connector Installed - May Cause Hardware Damage\n";
#endif

extern int force_failover_auto;
extern int force_failover_next;

#if CORTEX || YUKONA
extern struct DDB cortexflash_ddb;
#endif

extern struct DDB *pmem_ddb_ptr;

#if MODULAR
static char *get_raw_input_buffer(struct PCB *pcb);
extern int force_boot_reset;
int dispatch_adr = &dispatch;
int fixup_adr = &fixup;
#endif

#if NT_ONLY_SUPPORT
int poison_hwrpb;
#endif

#if MIKASA
extern int glb_hlt;
extern int force_boot_reset;
#endif

#if TURBO || RAWHIDE 
extern int ERR_LOG_FLAG;
extern int cpu_mask;
void display_boot_info(char *device);
typedef __int64 INT;
typedef unsigned __int64 UINT;

#endif
#if TURBO
extern int flatmem_check_flag;
extern int mixed_cpus_flag;
extern int dont_boot;		/* Global flag defined in 'turbo.c' */
#endif
#if RAWHIDE
extern UINT memory_test_desc_table;
extern int console_dlb_flag;
extern int global_halt_status;
#endif
#if GALAXY
extern int all_cpu_mask;
extern int galaxy_partitions;
#if TURBO
extern int lp_hard_partition;
#endif
#endif

/*
 *  External routine references
 */
extern int (*fe_callback) ();
extern int krn$_callback ();
extern int configure_io_adapters();

extern struct device* find_topology(struct window_head *w,
                             int hose, int slot, int channel, int node);
extern void krn$_init(void);

extern struct ZONE *memzone;

/*                                           
 * Topology array offsets
 */
#define TOP$K_NODE     0
#define TOP$K_CHANNEL  1
#define TOP$K_SLOT     2
#define TOP$K_HOSE     3
#define	TOP$K_MAX      4

extern struct HWRPB *hwrpb;
extern int HWRPB_ADDR[];
extern int _align (LONGWORD) robust_mode;
#if TURBO || RAWHIDE
extern struct window_head config;
#endif

volatile struct bb {
    unsigned short int skip;
    unsigned char n;
    unsigned char key;
    unsigned short int high_lbn;
    unsigned short int low_lbn;
    char fill[472];
    unsigned long int size;
    unsigned long int size_h;
    unsigned long int lbn;
    unsigned long int lbn_h;
    unsigned long int flags;
    unsigned long int flags_h;
    unsigned long int check;
    unsigned long int check_h;
    } ;

/*+
 * ============================================================================
 * = boot - Bootstrap the system.                                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Initializes the processor, loads a program image from the 
 *	specified boot device, and transfers control to that image. If you 
 *	do not specify a boot device in the command line, the default boot 
 *	device is used. The default boot device is determined by the 
 *	value of the 'bootdef_dev' environment variable.  
 *
 *	If you specify a list of devices, a bootstrap is attempted from each
 *	device in order. Then control passes to the first successfully
 *	booted image.  In a list, always enter network devices last,
 *	since network bootstraps only terminate if a fatal error occurs
 *	or an image is successfully loaded.
 *
 *	The -flags option can pass additional information to the operating  
 *	system about the boot that you are requesting. On a VMS system, 
 *	the -flags option specifies the system root number and boot flags.
 *	If you do not specify a boot flag qualifier, then the default boot 
 *	flags value specified by the 'boot_osflags' environment variable 
 *	is used.
 *
 *	The -protocol option allows selection of either the DECNET MOP or
 *	the TCP/IP BOOTP network bootstraps.   The keywords 'mop' or 'bootp'
 *	are valid arguments for this option.  Note, it is possible to set
 *	the default protocol for a port by setting the environment variable,
 *	'eza0_protocols' or 'ezb0_protocols' to the appropriate protocol.
 *
 *	Explicitly stating the boot flags or the boot device
 *	overrides the current default value for the current boot request,
 *	but does not change the corresponding environment variable.
 *
 *   COMMAND FMT: boot_support 1 SB 8 boot
 *  
 *   COMMAND FORM:
 *  
 *	boot ( [-file <filename>] [-flags <longword>[,<longword>]]
 *             [-protocols <enet_protocol>] [-halt]
 *             [<boot_device>][,<boot_device>] )
 *  
 *   COMMAND TAG: boot 6*1024 RXBZP boot b
 *  
 *   COMMAND ARGUMENT(S):
 *
 * 	<boot_device> - A device path or list of devices from which the firmware
 *		attempts to boot, or a saved boot specification in the form of
 *		an environment variable. Use the set bootdef_dev command 
 *		to define the default boot device. 
 *
 *   COMMAND OPTION(S):
 *
 *	-file <filename> - Specifies the name of a file to load into the 
 *		system.  For booting from Ethernet, this name 
 *		is limited by the MOP V3 load protocol to 15 characters.
 *		Use the set boot_file command to specify a default 
 *		boot file. 
 *
 * 	-flags <text>[,<text>] - Provides additional operating system-specific
 *		boot information. In the case of VMS, specifies system root 
 *		number and boot flags. These values are passed to the operating
 *		system for interpretation.  Preset default boot flag 
 *		values are 0,0. Use the set boot_osflags command 
 *		to change the default boot flag values. 
 *
 *	-protocols <enet_protocol> - Specifies the Ethernet protocol(s) to
 *		to be used for the network boot.  Either the keyword 'mop'
 *		or 'bootp' may be specified.  If both are specified, each
 *		protocol is attempted to solicit a boot server.
 *
 *	-halt   - Forces the bootstrap operation to halt and invoke the 
 *		console program once the image is loaded and page tables
 *		and other data structures are set up.  Console device drivers
 *		are not shut down when this qualifier is present. Transfer 
 *		control to the image by entering the continue command. 
 *
 * <condition>(INTERNAL)
 *	-bss <pages> - The number of additional pages for basic static storage
 *		to be mapped beyond the image loaded in.
 *		By default 256 KB is mapped.  This value may
 *		only be increased with this qualifier.
 *		This is primarily a debug option.
 *
 *	-stack <pages> - A decimal value that ensures that at least this number
 *		of pages will be mapped for stack usage.  The stack size
 *		is never less that 2 pages.  Using this qualifier can only
 *		increase stack usage, not decrease it. 
 *		This is primarily a debug option.
 *
 *	-skip <bytes> - The number of bytes of an image file to skip or ignore
 *		during a boot load.  This is primarily a debug option.
 *
 *	-start <offset> - The byte offset into the image file at which to start
 *		execution.  This is primarily a debug option.
 * <endcondition>
 *
 *   COMMAND EXAMPLE(S):
 *
 *	In the following example, the system boots from the default 
 *	boot device. The console program returns an error message if a 
 *	default boot device has not been set. 
 *~
 *	>>>boot  
 *~
 *	In the next example, the system boots from the Ethernet port, eza0. 
 *~
 *	>>>boot eza0
 *~
 *	In the next example, the system boots the file named 'dec_4000.sys'
 *	from device mke0.
 *~
 *	>>>boot -file dec_4000.sys mke0 
 *~
 *	In the next example, the system attempts a TCP/IP BOOTP network boot
 *	from Ethernet port eza0.
 *~
 *	>>>boot -protocol bootp eza0
 *~
 *	In the next example, the system boots from the default boot 
 *	device using boot flag settings 0,1. 
 *~
 *	>>>boot -flags 0,1
 *~
 *      In the next example, the system loads the operating system from 
 *	the SCSI disk, dka0, but remains in console mode.  Subsequently,
 *	you can enter the continue command to transfer control to
 *	the operating system. 
 *~
 *	>>>boot -halt dka0 
 *~	
 *	
 *   COMMAND REFERENCES:
 *
 *	set, show
 *
 * FORM OF CALL:
 *  
 *	boot( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Indicates normal completion.
 *       
 * ARGUMENTS:
 *
 *	int argc - The number of command line arguments passed by the shell. 
 *	char *argv [] - Array of pointers to arguments. 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

#if !MODULAR
int boot( int argc, char *argv[] )
{
    return boot_support(argc, argv);
}
#endif

#define QFLAG 0
#define QHALT 1
#define	QSTACK 2
#define	QBSS 3
#define QFILE 4
#define QSKIP 5
#define QSTART 6
#define QPROTOCOLS 7
#define QLENGTH 8
#define QMAX 9
#define QSTRING "%sflags halt %dstack %dbss %sfile %xskip %xstart " \
		"%sprotocols %xlength"

int boot_support( int argc, char *argv[] )
    {
    char *d;
    char *device;
    char *file;
    char *flags;
    int i;
    int j;
    int status;
    int autoboot;
    int autofail;
    int repeat;
    struct EVNODE *evp_device;
    struct EVNODE *evp_file;
    struct EVNODE *evp_flags;
    struct EVNODE *evp_reset;
    struct QSTRUCT *qual;
#if AVMIN
    char	*tmpptr1;
    char	*tmpptr2;
    char	*tfile;
    int fchanged;
#endif
#if TURBO
    int test_block = 0;
    int pdebug_save;
    struct EVNODE evn,*evp=&evn;
#endif

#if MEDULLA
    unsigned short toy_reg, p2_cnt;
    char med_ev2 [] = "p2_cnt";
    extern int dont_boot;	/* Global flag defined in 'timer.c' */
    extern int blink_led;	/* Global flag defined in powerup_medulla.c */
    struct CONSOLE_NVRAM *nv_ptr = CONSOLE_NVRAM_BASE;
#endif

#if RAWHIDE
    int halt_status;
#endif

#if MODULAR
    ovly_load("BOOT");		/* Lock overlay in memory */
#endif

#if MIKASA
#if !CORELLE
    halt_switch_in( );
    if (glb_hlt==1 || cb_ref)
     {
        force_boot_reset = 1;
        printf("\nUnknown system state.\n");
        printf("System must be reset prior to boot.\n\n");
     }
#endif
#endif

#if TURBO || RAWHIDE
    if ( cb_ref ) {
        force_boot_reset = 1;
        printf("\nInconsistent boot driver state.\n");
#if GALAXY
	/* we can't do a successful boot reset if we're in a galaxy */
	if (galaxy_partitions > 1) {
	    printf("System is configured with multiple partitions.\n");
	    printf("A complete INIT must be performed before rebooting.\n");
	    return (msg_failure);
	}
#endif
        printf("System will be reset prior to boot.\n\n");
    }

#if GALAXY && (TURBO || RAWHIDE)
    /* check to make sure that the galaxy command has been issued */
    if ((galaxy_partitions > 1) && (cpu_mask == all_cpu_mask)) {
	printf("Partitions defined, but not started. Starting partitions now.\n");
	if (galaxy_shared_adr == 0)
	    ovly_load("GALAXY");
	if (galaxy_shared_adr) {
	    status = galaxy(0,0);
	    if (status != msg_success)
		return (msg_failure);
	} else {
	    return (msg_failure);
	}
    }
#endif

/* If this is a 'boot'd' console -- reset before boot'ing again */
#if RAWHIDE
    if (console_dlb_flag)
	force_boot_reset = 1;
#endif
#endif

#if TURBO
    /*  See if we have enough heap to decompress overlays. */
    /*  If not, we'll do a boot_reset before continuing.   */

    /* malloc 150Kb */

    test_block = dyn$_malloc((150*1024), DYN$K_SYNC|DYN$K_NOWAIT);

    if ( test_block == 0 ) {   		/* malloc failed */
extern struct ZONE defzone;
        force_boot_reset = 1;
        pprintf("Insufficient Heap for overlay decompression.\n");
	pprintf("System will be reset prior to boot.\n");

	pdebug_save = print_debug_flag;
	print_debug_flag = 1;
	pprintf ("\n");
	dyn$_check();		/* show dynamic memory usage */
	dyn$_procstat (&defzone, 1);
	print_debug_flag = pdebug_save;
    }
    else
        free (test_block);
#endif

#if TURBO || RAWHIDE || MIKASA
#if TURBO
    if (!lfu_address)
#endif
    {
    if (force_boot_reset)
	boot_reset( argc, argv );	/* force a reset */

    if( argc && ( ( argv[0][0] == 'b' ) || ( argv[0][0] == 'B' ) ) )
	{
	evp_reset = malloc( sizeof( *evp_reset ) );
	if ( ev_read( "boot_reset", &evp_reset, EV$K_SYSTEM ) == msg_success )
	    if ( !( strcmp( evp_reset->value.string, "ON" ) ) )
		boot_reset( argc, argv );
	}
    else
	{
	if ( argv[0][0] ==  'r')
	    strcpy( argv[0], "b" );
	}
    }
#endif

#if SECURE
    /*
     *  If console secure and the user specified any arguments, 
     *  do not allow them to boot.
     */
    if( ( argc > 1 ) &&  secure  )
	{
        err_printf( msg_noboot_params );
        return( msg_noboot_params );
	}
#endif

#if TL6
/*
 * If the "dont_boot" flag was set, do not boot the OS. Return failure.
 */
    if (dont_boot) {
	ev_read("sys_model_num", &evp, 0);
	pprintf("\n* * * WARNING * * *\n");
	pprintf("boot disallowed:\n");
	pprintf("System model number \"%s\" does not agree with hardware configuration.\n\n", evp->value.string);
        return msg_failure;
    }
#endif

#if MEDULLA
    /*
     * If the "dont_boot" flag was set, do not boot the OS. Return failure.
     */
    if (dont_boot)
    {
	pprintf("\nboot disallowed: ");
	if(dont_boot & 1)
	    err_printf("CPU Speed error\n");

	if(dont_boot & 2)
	    err_printf("Unknown Board Type\n");

        if(dont_boot > 3)
	    err_printf("%xh\n",dont_boot);	

        return msg_failure;
    }
    /*
     *  Make sure P2 connector is attached before booting.  If it
     *  is not present it will compromise the reliability of the
     *  module.
     *
     *  NOTE:  Should the user continue to boot w/o the P2 connector,
     *	       it should be logged by the error logger.
     */
    if( !(inportb(NULL, (int)MOD_CONFIG_REG) & 0x20) )
	{
        if (!update_confirmed (msg_nop2)) return msg_failure;
	
#if MEDULLA_PASS_NUMBER == 1
	toy_reg = xlate_rtc_addr (&nv_ptr->p2_count);
#else
	toy_reg = &nv_ptr->p2_count;
#endif
    	p2_cnt = inportb(NULL, toy_reg); /* Read value of NVRAM p2_cnt */
	p2_cnt += 1;  /* increment value to log amount of boot attempts */
	write_nvram(NULL, &med_ev2, p2_cnt); /* Store new count in NVRAM */
	pprintf ("\n\tAttempts to boot without P2 installed = %d\n\n",p2_cnt);
	}

    /*
     * Blank the LED Display Register just before booting
     */
     blink_led = 0;
     outportb(NULL, MOD_DISPLAY_REG, 0);
#endif

    fe_callback = krn$_callback;

    /*
     *  Restart any stopped drivers.
     */
    start_drivers( );

#if RAWHIDE
    /*
     *  Check the halt switch, and obey it. Global status may get cleared
     *  by call to halt_switch_in().
     */
    if( halt_switch_in( TRUE ) || global_halt_status )
        {
        global_halt_status = FALSE;
        printf("\nHalt is or has been asserted, BOOT NOT POSSIBLE\n\n");
        return( msg_no_boot );
        }
#else
    /*
     *  Check the halt switch, and obey it.
     */
    if( halt_switch_in( ) )
        {
        printf( msg_no_boot );
        return( msg_no_boot );
        }
#endif
    
    /*
     *  We will be reading in an image into the area covered by the main memory
     *  heap.  Verify that this heap is free.
     */
    if (memzone) {
#if TURBO
	if (!lfu_address)
#endif
	{
#if MODULAR
	if (ovly_image_address != 0)
	    free(ovly_image_address);
	ovly_image_address = 0;
#endif
	if( memzone->allocated ) {
	    err_printf( "warning -- main memory zone is not free\n" );
	    return( msg_failure );
	}
	}
/* eliminate memzone, it gets wiped out during booting */

    dyn$_zfree(memzone);
    free(memzone);
    memzone = 0;
    }

#if TURBO
    lfu_address = 0;		/* clear this for boot of lfu */
#endif

/* Parse out qualifiers. */

    qual = malloc( sizeof( *qual ) * QMAX );
    status = qscan( &argc, argv, "-", QSTRING, qual );
    if( status != msg_success )	{
	err_printf( status );
	return( status );
    }

    if( argc && ( ( argv[0][0] == 'b' ) || ( argv[0][0] == 'B' ) ) )
	{
	autoboot = 0;
        if( argc > 1 ) 
 	    {
	    j = 0;
	    for( i = 1; i < argc; i++ )
		j += strlen( argv[i] );
	    d = malloc( j + argc );
	    device = malloc( j + argc );
	    device[0] = 0;
	    for( i = 1; i < argc; i++ )
		for( j = 0; strelement( argv[i], j, " ,", d ); j++ )
		    {
		    strcat( device, d );
		    strcat( device, " " );
		    }
	    device[strlen( device ) - 1] = 0;
	    ev_write( "boot_dev", device, EV$K_SYSTEM|EV$K_STRING );
	    free( d );
	    free( device );
	    }
	else
	    {
	    evp_device = malloc( sizeof( *evp_device ) );
	    if( ev_read( "bootdef_dev", &evp_device, EV$K_SYSTEM ) != msg_success )
		strcpy( evp_device->value.string, "" );
	    ev_write( "boot_dev", evp_device->value.string, EV$K_SYSTEM|EV$K_STRING );
	    free( evp_device );
	    }
	}
    else
	autoboot = 1;

    /*
     *  Fetch the environment variables we'll need for later.
     */
    evp_device = malloc( sizeof( *evp_device ) );
    if( ev_read( "boot_dev", &evp_device, EV$K_SYSTEM ) != msg_success )
	strcpy( evp_device->value.string, "" );


    evp_file = malloc( sizeof( *evp_file ) );
    if( ev_read( "boot_file", &evp_file, EV$K_SYSTEM ) != msg_success )
	strcpy( evp_file->value.string, "" );
    evp_flags = malloc( sizeof( *evp_flags ) );
    if( ev_read( "boot_osflags", &evp_flags, EV$K_SYSTEM ) != msg_success )
	strcpy( evp_flags->value.string, "0" );

    /*
     *  If no boot device is available, just quit.
     */
    if( !strlen( evp_device->value.string ) )
	{
	err_printf( "usage is boot <device> [-file <file>] [-flags <flags>]\n" );
	return( msg_failure );
	}

    /*
     *  Check each argument for a possible list of boot devices to try.  The
     *  elements of the list may be separated by spaces or commas.
     */
    device = malloc( strlen( evp_device->value.string ) );
    if( qual[QFILE].present )
	file = qual[QFILE].value.string;
    else
	file = evp_file->value.string;
#if AVMIN
    tfile = malloc(strlen(file)+1);
    fchanged = 0;
    if ((tmpptr1 = strchr(file, '"')) != NULL) {
	if ((tmpptr2 = strrchr(file, '"')) != NULL) {
	    *tmpptr2 = '\0';  
	    strncpy(tfile, tmpptr1+1, strlen(tmpptr1+1));
	    fchanged = 1;
	}
    }
    if (fchanged)
	strcpy(file, tfile);
    free(tfile);
#endif

    if( qual[QFLAG].present )
	flags = qual[QFLAG].value.string;
    else
	flags = evp_flags->value.string;

    autofail = force_failover_auto;
REPEAT:
    if ( ( autoboot ) && ( autofail == 1 ) && ( repeat == 1) )
      {
      autofail = 0; /* only retry once */
      force_failover_next = 1;
      }
    repeat = 0;
    for( j = 0; strelement( evp_device->value.string, j, " ,", device ); j++ )
	{
	if( killpending( ) )
	    return( msg_ctrlc );
	status = boot_sub( device, file, flags, qual );
	if( ( status == msg_success ) || ( status == msg_ctrlc ) )
	    return( status );
	if( status == msg_mop_inf_retry_net )
	    repeat = 1;
	}
    if( autoboot )
	{
	for( i = 0; i < 30; i++ )
	    {
	    if( killpending( ) )
		return( msg_ctrlc );
	    krn$_sleep( 500 );
	    }
	repeat = 1;
	}
    if( repeat )
	{
	err_printf( "\nRetrying, type ^C to abort...\n\n" );
	goto REPEAT;
	}
    }



/*+
 * ============================================================================
 * = boot_sub - Perform supplemental boot work                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Based upon device type, this routine calls a specific boot routine
 *      such as disk, tape or Ethernet.  It then initializes and maps things
 *      such as the HWRPB, pages tables, virtual regions, all of firmware,
 *      the PAL and the heap.  Finally, the routine either returns to the 
 *      console if the HALT flag was set, or jumps to the bootstrap code.
 *
 * FORM OF CALL:
 *  
 *	boot_sub (device, file, flags, qual)
 *  
 * RETURN CODES:
 *
 *     msg_success - successful operation
 *     msg_failure - failure
 *     msg_ctrlc   - aborted by control_C handler
 *       
 * ARGUMENTS:
 *
 *      char *device  - Pointer to boot device name
 *      char *file    - Pointer to boot file name 
 *      char *flags   - Pointer to boot flags
 *      QSTRUCT *qual - Pointer to boot commands arguments
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int boot_sub( char *device, char *file, char *flags, struct QSTRUCT *qual )
    {
    int status;
    char dname[EV_VALUE_LEN+1];
    int id;

    int real_id;

    int i;
    int j;
    int *p;
    int image_start;
    int image_bytes;
    int image_pages;
    int pfn, ptx;
    int t;
    struct PCB *pcb;
    struct MEMDSC *memdsc;
    struct SLOT *slot;
    UINT pt;
    UINT l0pt;
    UINT l1pt;
    UINT l2pt;
    UINT l3pt0;
    UINT l3pt1;
    unsigned char *base;
    uint base_id;
    UINT pt_pfn;
    UINT l0pt_pfn;
    UINT l1pt_pfn;
    UINT l2pt_pfn;
    UINT l3pt0_pfn;
    UINT l3pt1_pfn;
    UINT base_pfn;
    UINT page_table;
    int skip;
    struct FILE *fp;
    struct DDB *dva;	/* driver type */
    struct EVNODE *evp, evn;
    int protocol;	/* for devices with multiple protocols (like EZ) */
    int stack_pages;
    int bss_pages;
    id = whoami();

#if TURBO
    if (flatmem_check_flag != 0) {
       printf("Illegal memory configuration - booting disabled.\n");
       return msg_failure;
    }

    if (mixed_cpus_flag != 0) {
       printf("Illegal KN7CD/E configuration - booting disabled.\n");
       return msg_failure;
    }
#endif

#if STARTSHUT || DRIVERSHUT
    status = create_device(device);
    if (status != msg_success)
	return status;
#endif

/* Display boot information such as device name, file name, flags */

    printf( "(boot %s%s%s%s%s)\n", device,
	    strlen( file ) ? " -file " : "", file,
	    strlen( flags ) ? " -flags " : "", flags );

#if STARTSHUT || DRIVERSHUT

    boot_retry_counter = 4;
    for( i = 0; i < boot_retry_counter; i++ )
	{
	if( killpending( ) )
	    return( msg_ctrlc );
	fp = fopen( device, "sr" );
	if( fp )
	    break;
	krn$_sleep( 500 );
	}
#if TURBO || RAWHIDE
    display_boot_info(device);
#endif
#else
    fp = fopen( device, "sr" );
#endif
    if( !fp )
	{
	err_printf( "failed to open %s\n", device );
	return( msg_failure );
	}

#if MODULAR
/* Build the HWRPB */   

    if ( !hwrpb ) {
        hwrpb = hwrpb_base_address();
        build_hwrpb(hwrpb);
    }
#endif

    init_cpu_state_all_cpus( );

#if NT_ONLY_SUPPORT
/* Note: on NT-only machines, we won't poison the HWRPB if overridden by */
/* the user (SRM_BOOT=ON) or we're building the HWRPB for AlphaBIOS/NT.  */
/* If srm_boot is set to on the fixup the hwrpb family type */

    evp = &evn;
    if( poison_hwrpb )
	{
	if( ( ev_read( "srm_boot", &evp, 0 ) != msg_success ) ||
	    ( strcmp_nocase( evp->value.string, "ON" ) ) )
	    *(UINT *)hwrpb->SYSTYPE = -(INT)HWRPB$_SYSTYPE;
	}
#endif

/* build_fru_table used to be here.  Moved to after bootstrap loaded */

    memdsc = (int)hwrpb + *hwrpb->MEM_OFFSET;

    j = *memdsc->CLUSTER_COUNT - 1;
    base_pfn = *memdsc->CLUSTER[1].START_PFN;
#if HIGH_BOOT_PTEPAGES
#if MAX_MEMORY_CHUNK > 1
    j = 2;
#endif
    l0pt_pfn = *memdsc->CLUSTER[j].START_PFN;
#else
    l0pt_pfn = *memdsc->CLUSTER[1].START_PFN - BOOT_PTEPAGES;
#endif
    l1pt_pfn = l0pt_pfn + BOOT_L0PAGES;
    l2pt_pfn = l1pt_pfn + BOOT_L1PAGES;
    l3pt0_pfn = l2pt_pfn + BOOT_L2PAGES;
    l3pt1_pfn = l3pt0_pfn + BOOT_L30PAGES;

    base = phys_to_virt( base_pfn * HWRPB$_PAGESIZE );
    l0pt = phys_to_virt( l0pt_pfn * HWRPB$_PAGESIZE );
    l1pt = phys_to_virt( l1pt_pfn * HWRPB$_PAGESIZE );
    l2pt = phys_to_virt( l2pt_pfn * HWRPB$_PAGESIZE );
    l3pt0 = phys_to_virt( l3pt0_pfn * HWRPB$_PAGESIZE );
    l3pt1 = phys_to_virt( l3pt1_pfn * HWRPB$_PAGESIZE );

    page_table = l0pt;

    if (page_table_levels == 4) {
	pt_pfn = l0pt_pfn;
    } else {
	pt_pfn = l1pt_pfn;
    }
    pt = pt_pfn * HWRPB$_PAGESIZE;

    /*
     * Figure out how many pages are being requested for
     * the stack.
     */
    stack_pages = BOOT_STACKPAGES;
    if( qual[QSTACK].present )
	stack_pages = max( qual[QSTACK].value.integer, BOOT_STACKPAGES );

    protocol = 0;

    /*
     * Initialize image bytes.  Currently only boot_pmem uses the initial
     * value, which can be set with the -length qualifier.
     */
    image_bytes = 0;
    if( qual[QLENGTH].present )
	image_bytes = qual[QLENGTH].value.integer;

    /* 
     * djm - Sept 9, 93
     * With DEBUG = 1 , these lines can be uncommented for more info.
     * dump_fp(fp);
     * dump_inode(fp->ip);
     * dump_ddb(fp->ip->dva);
     */

    /* device is a block oriented device */
    if( fp->ip->dva->block ) {
	t = boot_disk( device, file, fp, base, &image_start, &image_bytes );

    /* device is a tape device */
    } else if( fp->ip->dva->sequential && !fp->ip->dva->net_device ) {
	t = boot_tape( device, file, fp, base, &image_start, &image_bytes );

    /* device is physical memory */
    } else if( fp->ip->dva == pmem_ddb_ptr) {
       t = boot_pmem( device, file, fp, &base, &image_start, &image_bytes, &base_pfn );

#if MEDULLA
    /* device is flash memory */
    } else if( ( fp->ip->dva == &medullaflash_ddb ) ||
               ( fp->ip->dva == &m224flash_ddb ) ) {
       t = boot_flash( device, file, fp, base, &image_start, &image_bytes );
#endif
#if CORTEX || YUKONA
    /* device is flash memory */
    } else if ( fp->ip->dva == &cortexflash_ddb )
	{
       t = boot_flash( device, file, fp, base, &image_start, &image_bytes );
#endif
     /* network type device */
    } else if( fp->ip->dva->net_device ) {
	struct EVNODE *evp;
	int trymop = TRUE;
	int trybootp = TRUE;
	char varname[32];
	char *protocols;
	  
	/*
	 *  Decide whether to try MOP, BOOTP (TFTP), or both.
	 */
	evp = malloc( sizeof( struct EVNODE ) );
	sprintf( varname, "%.4s_protocols", device );
	if( qual[QPROTOCOLS].present )
	    protocols = qual[QPROTOCOLS].value.string;
	else if( ev_read( varname, &evp, 0 ) == msg_success )
	    protocols = evp->value.string;
	else
	    protocols = "";
	if( strlen( protocols ) )
	    {
	    if( !strstr( protocols, "MOP" )
		    && !strstr( protocols, "mop") )
		trymop = FALSE;

	    if( !strstr( protocols, "BOOTP" )
		    && !strstr( protocols, "bootp" ) )
		trybootp = FALSE;
	        }
	t = msg_failure;
	if( trymop && !killpending( ) )
            {
	    protocol = 0;
            printf( "\nTrying MOP boot.\n" );
	        t = boot_network( device, file, fp, base, &image_start,
			&image_bytes, "mopdl", 1 );
            }

	if( trybootp && ( t != msg_success ) && !killpending( ) )
            {
	    protocol = 1;
            printf( "\nTrying BOOTP boot.\n" );
	        t = boot_network( device, file, fp, base, &image_start,
			&image_bytes, "tftp:verbose", 0 );
            }

	free( evp );

	} else {
	    printf( "unrecognized boot device type\n" );
	    return( msg_failure );
	}

    if( killpending( ) )
	return( msg_ctrlc );

    if( qual[QSKIP].present )
	{
	skip = qual[QSKIP].value.integer;
	if( skip )
	    {
	    memcpy( base, base + skip, image_bytes );
	    image_bytes -= skip;
	    }
	}
    image_pages = ( image_bytes + 8191 ) / HWRPB$_PAGESIZE;

    /*
     *  Save the driver type for use later when we have to
     *  determine the boot device type.
     */
    dva = fp->ip->dva;

    file2dev( fp->ip->name, dname, protocol );

    fclose( fp );
    if( t != msg_success )
	{
	err_printf( "bootstrap failure\n" );
	return( t );
	}
    printf( "bootstrap code read in\n" );

#if RAWHIDE | MIKASA | TURBO
    if ( !(hwrpb->FRU_OFFSET[0]) ) {
#if TURBO | RAWHIDE
	base_id = *((int *)(base + 4));
	if ((base_id != 'LFU0') && (base_id != 'TLR0')) {
	    ovly_load("FRU");
	    build_fru_table();
	    ovly_remove("FRU");
	}
#else
        build_fru_table();
#endif
    }
#endif

#if SABLE
    strcpy( PAL$IMPURE_BASE + IMPURE$PLATFORM_SCRATCH_OFFSET, dname);
#endif
    ev_write( "booted_dev", dname, EV$K_STRING );
    ev_write( "booted_file", file, EV$K_STRING );
    ev_write( "booted_osflags", flags, EV$K_STRING );
    printf( "base = %x, image_start = %x, image_bytes = %x\n", base, image_start, image_bytes );
    if( qual[QSTART].present )
	{
	image_start = qual[QSTART].value.integer;
	printf( "changing image_start to %x\n", image_start );
	}

    printf( "initializing HWRPB at %x\n", hwrpb );
#if TURBO
    /*
     * EMULATION HACK! For Turbo MP, the hwrbp slot for the 'real' CPU is used
     */
    real_id = real_slot(id);
    slot = (int)hwrpb + *hwrpb->SLOT_OFFSET + (real_id * sizeof(struct SLOT));
#else
    slot = (int)hwrpb + *hwrpb->SLOT_OFFSET + (id * sizeof(struct SLOT));
#endif
    *(INT *)hwrpb->TXRDY = 0;
    *(INT *)hwrpb->RXRDY = 0;    
    *(INT *)hwrpb->SAVE_TERM = 0;
    *(INT *)hwrpb->SAVE_TERM_VALUE = 0;
    *(INT *)hwrpb->RESTORE_TERM = 0;
    *(INT *)hwrpb->RESTORE_TERM_VALUE = 0;
    *(INT *)hwrpb->RESTART = 0;
    *(INT *)hwrpb->RESTART_VALUE = 0;
    *(INT *)slot->HWPCB.VMS_HWPCB.PTBR = pt_pfn;
    *(INT *)slot->HWPCB.VMS_HWPCB.ASN = 0;
    *(INT *)slot->HWPCB.VMS_HWPCB.ASTEN_SR = 0;
    *(INT *)slot->HWPCB.VMS_HWPCB.FEN = 0;
    *(INT *)slot->HWPCB.VMS_HWPCB.CC = 0;
#if TURBO
    *(INT *)hwrpb->PRIMARY = real_id;
#else
    *(INT *)hwrpb->PRIMARY = id;
#endif

#if GALAXY
    /*
     * Write the Galaxy Configuration Tree pointer
     */
    if (galaxy_partitions) {
#if RAWHIDE || TURBO
	*(UINT *)hwrpb->RSVD_HW = *(UINT *)0x80;
#endif
	printf("GCT base = %x\n",*(uint64 *)hwrpb->RSVD_HW);
	galaxy_increment_incarnation();	    /* this partition is booting */
    } else {
	*(INT *)hwrpb->RSVD_HW = 0;
    }
#else
    *(INT *)hwrpb->RSVD_HW = 0;
#endif

    *(INT *)hwrpb->RSVD_SW = 0;
    *(INT *)slot->HALT_PC = 0x20000000 + image_start;
    slot->STATE.SLOT$V_CV = 1;
    slot->STATE.SLOT$V_BIP = 1;
    boot_cpu_slot = slot;

    printf( "initializing page table at %x\n", pt );
    quadset( page_table, 0, BOOT_PTEPAGES * HWRPB$_PAGESIZE / 8 );

    if( page_table_levels == 4 )
	{
	/*
	 * map the page tables themselves, except for the level 0 and level 1
	 * page tables
	 */
	((int *)l0pt)[0] = 0x1101;
	((int *)l0pt)[1] = l1pt_pfn;
	((int *)l0pt)[2] = 0x1101;
	((int *)l0pt)[3] = l0pt_pfn;
	((int *)l1pt)[0] = 0x1101;
	((int *)l1pt)[1] = l2pt_pfn;
	}
    else
	{
	/*
	 * map the page tables themselves, except for the level 1
	 * page table
	 */
	((int *)l1pt)[0] = 0x1101;
	((int *)l1pt)[1] = l2pt_pfn;
	((int *)l1pt)[2] = 0x1101;
	((int *)l1pt)[3] = l1pt_pfn;
	}

    /* map first virtual region (0x10000000) in level 2 tables */
    ((int *)l2pt)[64] = 0x1101;
    ((int *)l2pt)[65] = l3pt0_pfn;

    /* map second virtual region (0x20000000) in level 2 tables */
    for( i = 0; i < BOOT_L31PAGES; i++ )
	{
	((int *)l2pt)[128 + i*2] = 0x1101;
	((int *)l2pt)[129 + i*2] = l3pt1_pfn + i;
	}

    /* map third virtual region (0x40000000) in level 2 tables */
    ((int *)l2pt)[256] = 0x1101;
    ((int *)l2pt)[257] = l2pt_pfn;

    /*
     * Map all of the firmware, HWRPB, PALcode, and heap
     */
    for( ptx = 0, i = 1, pfn = *memdsc->CLUSTER[0].START_PFN + 1;
	    i < *memdsc->CLUSTER[0].PFN_COUNT; i++ )
	{
	((int *)l3pt0)[ptx++] = 0x1101;
	((int *)l3pt0)[ptx++] = pfn++;
	}

#if RAWHIDE
    /*
     * This is temporary and will be removed once we use the real bitmap
     * unconditionally.
     */
    if ( memory_test_desc_table )
#endif

    for( j; j < *memdsc->CLUSTER_COUNT; j++ )
	if( *memdsc->CLUSTER[j].USAGE )
	    for( i = 0, pfn = *memdsc->CLUSTER[j].START_PFN;
		    i < *memdsc->CLUSTER[j].PFN_COUNT; i++ )
		{
		((int *)l3pt0)[ptx++] = 0x1101;
		((int *)l3pt0)[ptx++] = pfn++;
		}
#if WILDFIRE
	else
	    {
	    uint64 bitmap_pa;
	    uint32 bitmap_size;
	    uint32 start_pfn;
	    uint32 pfn_count;

	    bitmap_pa = *(uint64 *)memdsc->CLUSTER[j].BITMAP_PA;
	    bitmap_size = ( *memdsc->CLUSTER[j].PFN_COUNT + 8 * HWRPB$_PAGESIZE - 1 ) / 8; 
	    start_pfn = bitmap_pa / HWRPB$_PAGESIZE;
	    pfn_count = ( ( bitmap_pa & ( HWRPB$_PAGESIZE - 1 ) ) + bitmap_size + HWRPB$_PAGESIZE - 1 )  / HWRPB$_PAGESIZE;
	    for( i = 0, pfn = start_pfn; i < pfn_count; i++ )
		{
		((int *)l3pt0)[ptx++] = 0x1101;
		((int *)l3pt0)[ptx++] = pfn++;
		}
	    }
#endif

    /*
     * Map the system software address space
     */
    for( ptx = i = 0, pfn = base_pfn; i < image_pages; i++ )
	{
	((int *)l3pt1)[ptx++] = 0x1101;
	((int *)l3pt1)[ptx++] = pfn++;
	}

    /*
     * Map in the BSS region.  This region may be increased with the bss
     * qualifier.
     */
    bss_pages = BOOT_BSSPAGES;
    if( qual[QBSS].present )
	bss_pages = max( BOOT_BSSPAGES, qual[QBSS].value.integer );
    for( i = 0; i < bss_pages; i++ )
	{
	((int *)l3pt1)[ptx++] = 0x1101;
	((int *)l3pt1)[ptx++] = pfn++;
	}

    /*
     * Map in one guard page (assume a PTE of 0 makes for a good
     * guard page, and that we have 1 guard page)
     */
    ptx += BOOT_GUARDPAGES * 2;

    /* map in the stack */
    for( i = 0; i < stack_pages; i++ )
	{
	((int *)l3pt1)[ptx++] = 0x1101;
	((int *)l3pt1)[ptx++] = pfn++;
	}

    /* We allocated only enough level 3 pages tables to accomodate
     * BOOT_L31PAGES * HWRPB$_PAGESIZE * HWRPB$_PAGESIZE / 8 bytes of memory.
     * Verify that we haven't run out of PTEs.
     */
    if (ptx >= (BOOT_L31PAGES * HWRPB$_PAGESIZE / 8 * 2)) 
	{
	printf ("Exceeded the number of allocated PTEs, continuing anyway.\n");
	}

    slot->HWPCB.VMS_HWPCB.KSP[0] = 0x20000000 + (ptx/2) * HWRPB$_PAGESIZE;
    slot->HWPCB.VMS_HWPCB.KSP[1] = 0;

#define ref_quad(x) (&(__int64)(x))

    if( killpending( ) )
	return( msg_ctrlc );
    printf( "initializing machine state\n" );

    printf( "setting affinity to the primary CPU\n" );
    pcb = getpcb( );
    pcb->pcb$l_affinity = 1 << primary_cpu;
    reschedule( 1 );

    hwrpb->VPTBR[0] = 0;
    if (page_table_levels == 4) {
	hwrpb->VPTBR[1] = 1 << (13 + 10 + 10 + 10 - 32);
    } else {
	hwrpb->VPTBR[1] = 1 << (13 + 10 + 10 - 32);
    }
    write_ipr( APR$K_KSP, slot->HWPCB.VMS_HWPCB.KSP );
    write_ipr( APR$K_PTBR, slot->HWPCB.VMS_HWPCB.PTBR );
    write_ipr( APR$K_VPTB, hwrpb->VPTBR );
    write_pc( slot->HALT_PC );
    map_via_page_table( l3pt0, (UINT)hwrpb, (UINT)HWRPB_ADDR );
#if !MODULAR
    map_via_page_table( l3pt0, (UINT)_dispatch_, (UINT)(dispatch + 2) );
    map_via_page_table( l3pt0, (UINT)_fixup_, (UINT)(fixup + 2) );
#else
    map_via_page_table( l3pt0, (UINT)_dispatch_, (UINT)(dispatch_adr + (2*4)) );
    map_via_page_table( l3pt0, (UINT)_fixup_, (UINT)(fixup_adr + (2*4)) );
#endif
    p = (int)&ENV_VAR_TABLE;
    j = ((int)&ENV_VAR_TABLE_END - (int)p) / 16;
    for( i = 0; i < j; i++ )
	map_via_page_table( l3pt0, (UINT)(evmem + i), (UINT)(p + i*4) );
    rebuild_memdsc( hwrpb, l3pt0 );
    rebuild_crb( hwrpb, l3pt0 );
    twos_checksum( hwrpb, offsetof( struct HWRPB, CHKSUM ) / 4, hwrpb->CHKSUM );
    checksum_config( hwrpb );

#if TURBO
#define CALLBACK_BUF_SIZE 3*1024
    /* Allocate some space for EEPROM callback use */
    p = &ERR_LOG_FLAG;
    for (j=cpu_mask,i=0;i<MAX_PROCESSOR_ID;j = j>>1,i++) {
	if ((j&1) && (!(p[2*i])))
	    p[2*i] = dyn$_malloc(CALLBACK_BUF_SIZE,DYN$K_SYNC|DYN$K_NOOWN);
    }

    /* Set impure OSF PAL base for PAL - need for swppal */
    *(int *) 0x4180 = 0x18000;

#endif    

#if STARTSHUT || DRIVERSHUT
    configure_io_adapters( TRUE );
#endif


#if RAWHIDE
    execute_nvram_script( NVRAM_BOOT );
#endif
#if TURBO
    execute_script("ee_boot");
    if (lp_hard_partition) {
	struct PCB *pcb = getpcb();

	/* Phase 0 - everyone flush their BCache */
	krn$_mp_action(
	  galaxy_isolate_partition,
	  pcb->pcb$l_pri + 1,
	  cpu_mask,
	  0,
	  "galaxy_isolate",
	  "nl", "r",
	  "tt", "w",
	  "tt", "w",
	  0, 0, 0, 0);

	/* Phase 1 - everyone clear the TLMMR registers */
	krn$_mp_action(
	  galaxy_isolate_partition,
	  pcb->pcb$l_pri + 1,
	  cpu_mask,
	  0,
	  "galaxy_isolate",
	  "nl", "r",
	  "tt", "w",
	  "tt", "w",
	  1, 0, 0, 0);
	}
#endif

    /*
     *  Return if the halt flag was set.
     */
    if( qual[QHALT].present )
	return( msg_success );

    if( killpending( ) )
	return( msg_ctrlc );
    printf( "jumping to bootstrap code\n" );

    /*
     *  Exit the console.
     */
#if MODULAR
    ovly_symbol_flush();		/* free symbol memory */
#endif
    console_exit_use_tt( );
    return( msg_success );
    }


/*+
 * ============================================================================
 * = boot_disk - Bootstrap routine for disks                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine is called to bootstrap a disk (block oriented device).
 *      It checks for a valid boot block and reads the boot block
 *      starting from the base address. 
 *  
 * FORM OF CALL:
 *  
 *      boot_disk (device, file, fp, base, start, length)
 *  
 * RETURN CODES:
 *
 *     msg_success - successful operation
 *     msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *      char *device  - Pointer to boot device name
 *      char *file    - Pointer to boot file name 
 *      FILE *fp      - Pointer to FILE structure
 *      unsigned char *base - Pointer to base address 
 *      int *start - Pointer to offset address from base
 *      int *length - Pointer to size of image
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int boot_disk( char *device, char *file, struct FILE *fp,
	unsigned char *base, int *start , int *length )
    {
    unsigned char *copy;
    unsigned char buffer[512];
    int i;
    int j;
    int k;
    int l;
    struct bb *bb;

    if( fread( buffer, 512, 1, fp ) != 512 )
	{
	printf( "failed to read %s\n", device );
	return( msg_failure );
	}
    if( boot_check_boot_block( buffer ) != msg_success )
	{
	printf( "block 0 of %s is not a valid boot block\n", device );
	return( msg_failure );
	}
    printf( "block 0 of %s is a valid boot block\n", device );
    *start = 0;
    bb = &buffer[0];
    fseek( fp, (INT)bb->lbn * 512, 0 );
    i = bb->size;
    j = (int)base;
    printf( "reading %d blocks from %s\n", i, device );
    *length = i * 512;
#if AVMIN
    while( i > 0 )
	{
	k = min( i, 64 );
	l = k * 512;
	if( fread( j, 512, k, fp ) != l )
	    {
	    printf( "failed to read %s\n", device );
	    return( msg_failure );
	    }
	i -= k;
	j += l;
	}
#else
    copy = malloc( 64 * 512 );
    while( i > 0 )
	{
	k = min( i, 64 );
	l = k * 512;
	if( fread( copy, 512, k, fp ) != l )
	    {
	    printf( "failed to read %s\n", device );
	    free( copy );
	    return( msg_failure );
	    }
	memcpy( j, copy, l );                    
	i -= k;
	j += l;
	}
    free( copy );
#endif
    return( msg_success );
    }


/*+
 * ============================================================================
 * = boot_tape - Bootstrap routine for tapes                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine is called to bootstrap a tape device. 
 *  
 * FORM OF CALL:
 *  
 *      boot_disk (device, file, fp, base, start, length)
 *  
 * RETURN CODES:
 *
 *     msg_success - successful operation
 *     msg_failure - failure
 *     t           -  Contains result of boot_get_filename function
 *     t           -  Contains result of boot_skip_file function
 *       
 * ARGUMENTS:
 *
 *      char *device  - Pointer to boot device name
 *      char *file    - Pointer to boot file name 
 *      FILE *fp      - Pointer to FILE structure
 *      unsigned char *base - Pointer to base address 
 *      int *start - Pointer to offset address from base
 *      int *length - Pointer to size of image
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int boot_tape( char *device, char *file, struct FILE *fp,
	unsigned char *base, int *start, int *length )
    {
    char filename[80];
    unsigned char buffer[512];
    int n;
    int t;
    int j;
    int l=0;
    struct bb *bb;

    n = fread( buffer, 512, 1, fp );
    if( n == 0 )
	{
	printf( "failed to read %s\n", device );
	return( msg_failure );
	}
    else if( n == 80 )
	{
	bb = 0;
	if( strncmp( buffer, "VOL1", 4 ) != 0 )
	    {
	    printf( "%s is not an ANSI labeled tape\n", device );
	    return( msg_failure );
	    }
	while( 1 )
	    {
	    t = boot_get_filename( fp, filename, buffer );
	    if( t != msg_success )
		return( t );
	    if( ( strlen( file ) == 0 )
		    || ( strcmp_nocase( filename, file ) == 0 ) )
		break;
	    t = boot_skip_file( fp, buffer, 2 );
	    if( t != msg_success )
		return( t );
	    }
	printf( "reading file %s from %s\n", filename, device );
	n = 0;
	*start = 0;
	if( strcmp( &filename[strlen(filename)-4], ".SYS" ) == 0 )
	    *start = 512;
	}
    else if( n == 512 )
	{
	printf( "boot block format for %s\n", device );
	if( boot_check_boot_block( buffer) != msg_success )
	    {
	    printf( "record 0 of %s is not a valid boot block, must be data\n", device );
	    bb = 0;
	    memcpy( base, buffer, 512 );
	    n = 1;
	    base += 512;
	    }
	else
	    {
	    printf( "record 0 of %s is a valid boot block\n", device );
	    bb = &buffer[0];
	    n = 0;
	    }
	*start = 0;
	}
    else
	{
	printf( "unrecognized tape format\n" );
	return( msg_failure );
	}
    j = (int)base;
    while( 1 )
	{
	fread( buffer, 512, 1, fp );
	memcpy(j, buffer, 512);
	j += 512;
	if( fp->status == msg_success )
	    {
	    n++;
	    if( bb && ( bb->size == n ) )
		break;
	    }
#if MODULAR
	else if( !strcmp(fp->status,msg_eof) )
	    break;
	else if( ( !strcmp(fp->status,msg_rec_underflow) )
		|| ( !strcmp(fp->status,msg_rec_overflow) ))
#else
	else if( fp->status == msg_eof )
	    break;
	else if( ( fp->status == msg_rec_underflow )
		|| ( fp->status == msg_rec_overflow ) )
#endif
	    {
	    printf( fp->status, 512 );
	    return( msg_failure );
	    }
	else
	    return( msg_failure );
	}
    printf( "%d records read\n", n );
    *length = n * 512;
    return( msg_success );
    }

/* Skip a tape file */
int boot_skip_file( struct FILE *fp, unsigned char *buffer, int count )
    {
    if( count <= 0 )
	return( msg_success );
    while( 1 )
	{
	fread( buffer, 512, 1, fp );
#if MODULAR
	if (fp->status == msg_success ||
	    !strcmp(fp->status,msg_rec_underflow) ||
	    !strcmp(fp->status,msg_rec_overflow))
#else
	if (fp->status == msg_success ||
	    fp->status == msg_rec_underflow ||
	    fp->status == msg_rec_overflow)
#endif
	    {
	    ;
	    }

#if MODULAR
	else if ( !strcmp(fp->status,msg_eof) )
#else
	else if (fp->status == msg_eof)
#endif
	    {
	    if( --count == 0 )
		return( msg_success );
	    }
	else
	    {
	    return( fp->status );
	    }
	}
    }


/*+
 * ============================================================================
 * = boot_get_filename - Get tape filename                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine reads the tape filename
 *  
 * FORM OF CALL:
 *  
 *      boot_get_filename (fp, filename, buffer )
 *  
 * RETURN CODES:
 *
 *     msg_failure -  failure (HDR1 record missing)
 *     fp->status  -  Contains fread status
 *     t           -  Contains result of boot_skip_file function
 *       
 * ARGUMENTS:
 *
 *      FILE *fp      - Pointer to FILE structure
 *      char *filename - Pointer to filename
 *      unsigned char *buffer - Address where stream will be read into
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int boot_get_filename( struct FILE *fp, char *filename, unsigned char *buffer )
    {
    char c;
    int i;
    int n;
    int t;

    fread( buffer, 80, 1, fp );
    if( fp->status != msg_success )
	return( fp->status );
    if( strncmp( buffer, "HDR1", 4 ) != 0 )
	{
	printf( "HDR1 record missing\n" );
	return( msg_failure );
	}
    for( i = 0; i < 17; i++ )
	{
	c = buffer[i+4];
	if( c == ' ' )
	    {
	    filename[i] = 0;
	    break;
	    }
	else
	    filename[i] = c;
	}
    if( i == 17 )
	{
	filename[17] = 0;
	while( 1 )
	    {
	    fread( buffer, 80, 1, fp );
#if MODULAR
	    if( !strcmp(fp->status,msg_eof) )
#else
	    if( fp->status == msg_eof )
#endif
		{
		n = 0;
		break;
		}
	    if( fp->status != msg_success )
		return( fp->status );
	    if( strncmp( buffer, "HDR4", 4 ) == 0 )
		{
		for( i = 0; i < 62; i++ )
		    {
		    c = buffer[i+5];
		    if( c == ' ' )
			{
			filename[i+17] = 0;
			break;
			}
		    else
			filename[i+17] = c;
		    }
		if( i == 62 )
		    filename[62+17] = 0;
		n = 1;
		break;
		}
	    }
	}
    else
	n = 1;
    t = boot_skip_file( fp, buffer, n );
    printf( "found file %s\n", filename );
    return( t );
    }


/*+
 * ============================================================================
 * = boot_pmem - Bootstrap routine for physical memory			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine is called to bootstrap a physical memory.
 *      It checks for a valid boot block and and gets the length from there.
 *	If a valid boot block is not found, 32Mb is assumed.
 *  
 * FORM OF CALL:
 *  
 *      boot_pmem (device, file, fp, base, start, length, base_pfn)
 *  
 * RETURN CODES:
 *
 *     msg_success - successful operation
 *     msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *      char *device  - Pointer to boot device name
 *      char *file    - Pointer to boot file name 
 *      FILE *fp      - Pointer to FILE structure
 *      unsigned char *base - Pointer to base address 
 *      int *start - Pointer to offset address from base
 *      int *length - Pointer to size of image
 *      unsigned int *base_pfn - Pointer to PFN of base address
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int boot_pmem( char *device, char *file, struct FILE *fp,
	   unsigned char **base, int *start , int *length, UINT *base_pfn )
    {
    unsigned char *image;

    /* 
     * The file pointer should already be pointing to the start of the
     * image to boot.  Do this by specifying an address on the device
     * name, i.e. pmem:24000.
     */
    image = ftell( fp );

    /*
     * If no -length qualifier was present, provide a default.
     */
    if (*length == 0) *length = 8 * 1024 * 1024;
    *start = (unsigned char *)0;
    *base = image;
    *base_pfn = virt_to_phys( (unsigned int)image ) >> HWRPB$_PAGEBITS;

    return( msg_success );
    }

/*+
 * ============================================================================
 * = boot_network - Bootstrap across network, MOP or BOOTP/TFTP               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine is called from boot_sub to perform a
 *      network bootstrap using MOP or BOOTP/TFTP protocols.
 *
 *	Upcase the filename iff 
 *		- our parent process is a shell
 *		- the filename hasn't been quoted, and is on the command line
 *  
 * FORM OF CALL:
 *  
 *	boot_network (device, file, fp, base, start, length, protocol, upcase)
 *  
 * RETURN CODES:
 *
 *     msg_success -  successful operation
 *     msg_ctrlc   - Null String if process to die by control_C handler
 *     msg_mop_inf_retry_net - Indicates Retry
 *       
 * ARGUMENTS:
 *
 *      char *device  - Pointer to boot device name
 *      char *file    - Pointer to boot file name 
 *      FILE *fp      - Pointer to FILE structure
 *      unsigned char *base - Pointer to base address 
 *      int *start - Pointer to offset address from base
 *      int *length - Pointer to size of image
 *      char *protocol - Pointer to desired protocol, mop or bootp/tftp
 *	int upcase - flag indicating to force boot filename to upper case
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int boot_network( char *device, char *file, struct FILE *fp,
	unsigned char *base, int *start, int *length, char *protocol,
	int upcase )
    {
    char *rebuf;
    char *modfile;
    char *newfile;
    int offset;
    int not_quoted;
    int t;
    int mark;
    struct PCB *pcb;
    struct PCB *parent;
    struct REGEXP *re;

    clear_progress();

    /*
     * Upcase the filename iff the parent is a shell, and the filename
     * is present on the command line, and hasn't been quoted, and the
     * caller wants the filename upcased (whew!).
     *
     * Since we want this code work with any shell, we have to let the
     * shell decide this (not all shells may have kept this information
     * around).
     */
    rebuf = malloc( strlen( file ) + 14 );
    sprintf( rebuf, "-fi*l*e*[ \t]+%s", file );
    re = regexp_compile( rebuf, 1 );
    free( rebuf );
    not_quoted = 0;
#if AVMIN
    if( re ) {
	pcb = getpcb( );
	parent = krn$_findpcb( pcb->pcb$l_parentpid );
    	if( parent ) {
	    if( parent->pcb$l_shellp ) {
	        if( regexp_subset( re, get_raw_input_buffer( parent ) ) )
		    not_quoted = 1;
	    }
	    spinunlock( &spl_kernel );
	} else {
	    if( pcb->pcb$l_shellp ) {
		if( regexp_subset(re, get_raw_input_buffer(pcb) ) )
		    not_quoted = 1;
	    }
	}
	regexp_release( re );
    }
    modfile = malloc( strlen( file ) + 1 );
    for( t = 0; file[t]; t++ )
        if( upcase && not_quoted )
            modfile[t] = toupper( file[t] );
        else
            modfile[t] = file[t];
#else
    if( re ) 
	{
	pcb = getpcb( );
	parent = krn$_findpcb( pcb->pcb$l_parentpid );
    	if( parent )
	    {
	    if( parent->pcb$l_shellp )
	        if( regexp_subset( re, get_raw_input_buffer( parent ) ) )
		    not_quoted = 1;
	    spinunlock( &spl_kernel );
	    }
	regexp_release( re );
	}
    modfile = malloc( strlen( file ) + 1 );
    for( t = 0; file[t]; t++ )
        if( upcase && not_quoted )
            modfile[t] = toupper( file[t] );
        else
            modfile[t] = file[t];
#endif
    newfile = malloc( strlen( protocol ) + strlen( device ) + strlen( modfile ) + 5 );
    sprintf( newfile, "%s:\"%s\"/%s", protocol, modfile, device );
    free( modfile );
    fp = fopen( newfile, "gr" );
    if( fp )
	{
	offset = 0;
	mark = 0;
	while( 1 )
	    {
	    if( killpending( ) )
		{
		fclose( fp );
		free( newfile );
		return( msg_ctrlc );
		}
	    t = fread( base + offset, 2048, 1, fp );
	    if( t )
		offset += t;
	    else
		break;
	    if( offset >= mark )
		{
		print_progress( );
		mark += 65536;
		}
	    }
	err_printf("\n");
	if( fp->status == msg_success )
	    {
	    *start = 0;
	    *length = offset;
	    fclose( fp );
	    free( newfile );
	    return( msg_success );
	    }
	fclose( fp );
	}
    free( newfile );
    return( msg_mop_inf_retry_net );
    }

int boot_check_boot_block( unsigned char *buffer )
    {
    unsigned long int check;
    unsigned long int check_h;
    unsigned long int *p;
    int i;
    int j;
    int k;
    struct bb *bb;

    check = 0;
    check_h = 0;
    p = buffer;
    for( i = 0; i < 126; i += 2 )
	{
	j = check;
	k = p[i];
	check = j + k;
	if( ( check < j ) && ( check < k ) )
	    check_h++;
	check_h += p[i+1];
	}
    bb = &buffer[0];
    if( ( bb->flags == 0 ) && ( bb->flags_h == 0 )
	    && ( bb->size != 0 ) && ( bb->size_h == 0 ) && ( bb->lbn_h == 0 )
	    && ( bb->check == check ) && ( bb->check_h == check_h ) )
	return( msg_success );
    else
	return( msg_failure );
    }

#if MODULAR
/*
 * Return the address of the lexical analyzer's input buffer.  This routine
 * should run under a spinlock to avoid the buffer from going away.
 *
 * The input buffer IS NOT always equivalent to the command line, especially
 * in the case of newlines.
 *
 * This works for both ash and blex as their PARSE definitions both have the
 * input buffer first.
 */
static char *get_raw_input_buffer(struct PCB *pcb)
{
    struct PARSE *p;

    p = (struct PARSE *) pcb->pcb$l_shellp;
    return p;
}
#endif

#if DEBUG
void dump_fp(struct FILE *fp){
#define $x(y)  printf("      fp->%s : %x\n","y",fp->/**/y)
	printf("fp:%x\n",fp);
	$x(ip);   
	$x(local_offset[0]);
	$x(local_offset[1]);
	$x(offset);
	$x(mode);
	$x(rap);
	$x(status);
	$x(count);
	$x(misc);
/*
	printf("	fp->ip:%x\n",fp->ip);
        printf("	fp->local_offset[0]:%x fp->local_offset[1]:%x\n",
			  fp->local_offset[0], fp->local_offset[1]);
	printf("	fp->offset:%x \n", fp->offset);
	printf("	fp->mode: %x\n", fp->mode);
	printf("	fp->rap: %x\n", fp->rap);	
	printf("	fp->status: %x\n",fp->status);
	printf("	fp->count: %x\n",fp->count);
	printf("	fp->misc: %x\n", fp->misc);
*/
}

void dump_boot_block(unsigned char *buffer){
#define $x(y)  printf("     bb->%s : %x\n","y",bb->/**/y)
   struct bb *bb;
   bb = &buffer[0];
   printf("boot block : %08x\n",bb);
   $x(skip);	
   $x(n);
   $x(key);
   $x(high_lbn);
   $x(low_lbn);
   $x(size);
   $x(size_h);
   $x(lbn);
   $x(lbn_h);
   $x(flags);
   $x(flags_h);
   $x(check);
   $x(check_h);
}

void dump_inode( struct INODE *ip){
#define $x(y)  printf("      ip->%s : %x\n","y",ip->/**/y)
   printf("inode : %08x\n",ip);
   $x(dva);
   $x(inuse);
   $x(attr);
   $x(loc);
   $x(len); 
   $x(alloc);
   $x(ref);
   $x(append_offset);
   $x(misc);
   printf("      ip->%s : %s\n","name",ip->name);
   $x(bs);
}

void dump_ddb( struct DDB *ddb){
#define $x(y)  printf("      ddb->%s : %x\n","y",ddb->/**/y)
   printf("ddb : %08x\n",ddb);
   $x(name);
   printf("      ddb->%s : %s\n","name",ddb->name);
   $x(read);
   $x(write);
   $x(open);
   $x(close);
   $x(expand);
   $x(delete);
   $x(create);
   $x(setmode);
   $x(validation);
   $x(misc);
   $x(allow_info);
   $x(allow_next);
   $x(flash);
   $x(block);
   $x(sequential);
   $x(ethernet);
   $x(filesystem);
}
#endif

/*+
 * ============================================================================
 * = boot_flash - Bootstrap routine for flash memory			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine is called to bootstrap flash memory.
 *      It always assumes a size of 512Kb. It copies the flash contents
 *	to physical memory.
 *  
 * FORM OF CALL:
 *  
 *      boot_flash (device, file, fp, base, start, length)
 *  
 * RETURN CODES:
 *
 *     msg_success - successful operation
 *     msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *      char *device  - Pointer to boot device name
 *      char *file    - Pointer to boot file name 
 *      FILE *fp      - Pointer to FILE structure
 *      unsigned char *base - Pointer to base address 
 *      int *start - Pointer to offset address from base
 *      int *length - Pointer to size of image
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int boot_flash( char *device, char *file, struct FILE *fp,
	   unsigned char *base, int *start , int *length )
    {

    /*+
     * Copy the contents of flash to physical memory. 
    -*/

    *length = fread(base,fp->ip->len[0] ,1, fp);
    *start = 0;
    return( msg_success );
    }

#if TURBO || RAWHIDE
/*+
 * ============================================================================
 * = display_boot_info - dump to the audit trail some booting info            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *    Since engineering always gets back the log file of the clip of the boot
 *    failure, and often does not get back a "SHOW CONFIG" display, more info
 *    is automatically displayed at the time of boot.
 *
 *    This feature is enabled on TurboLaser and Rawhide only.
 *  
 *  
 * FORM OF CALL:
 *  
 *      display_boot_info (device)
 *  
 * RETURN CODES:
 *
 *       
 * ARGUMENTS:
 *
 *      char *device  - Pointer to boot device name
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void display_boot_info(char *device)
{
#if !RAWHIDE
#define EV_EXTENDED_VALUE_LEN 128
#define flatmem_check (*(ovly_rtn_list[6]))
     char dname[100];      
     char protocol[8];
     char controller[EV_EXTENDED_VALUE_LEN];
     int unit, node, channel, slot, hose, bus, function;
     int status;
     int (**ovly_rtn_list)();
     struct device *dev;
     struct device *t;      


#if 0
/*
This routine called setup_lmmrs and do_turbo_lmmrs and the reconfiguring of
memory cannot be done when running from memory.   It can be cleaned up
and would be a good idea to have it work.  So I'll leave this here as a 
reminder.
*/
     /*
      * Enforce flat memory restriction.
      * Remove this code after pass 4 EV5 is the only thing out there.
      */
     ovly_rtn_list = ovly_load("POWERUP");
     flatmem_check();
     ovly_remove("POWERUP");
#endif
     

     status=file2dev(device,&dname,0);
     if (status == msg_failure) return;
     printf("SRM boot identifier: %s\n",&dname);
     explode_devname(device, protocol, controller, &unit, &node, &channel, &slot,&hose);
     dev = find_controller(&config, protocol, controller_id_to_num(controller), &channel);
     printf("boot adapter: %s%d  rev %x in bus slot %d",
           dev->tbl->mnemonic,dev->unit,dev->rev,dev->slot);
     t = config.descr[log2phy_slot(dev->hose)];
     printf(" off of %s%d in TLSB slot %d\n",t->tbl->mnemonic,t->unit,t->slot);
#endif
}
#endif

int map_via_page_table( int *l3pt, uint64 pa, uint64 *va )
    {
    int i;
    uint32 pfn;

    pfn = (pa + get_console_base_pa()) / HWRPB$_PAGESIZE;
    for( i = 0; i < HWRPB$_PAGESIZE; i += 2 )
	{
	if( l3pt[i+1] == pfn )
	    {
	    *va = 0x10000000 + ( i / 2 * HWRPB$_PAGESIZE ) + ( pa % HWRPB$_PAGESIZE );
#if 0
	    pprintf( "map_via_page_table, pa = %x, pfn = %x, i = %x, va = %x\n",
		    pa, pfn, i, *va );
#endif
	    return( msg_success );
	    }
	}
    pprintf( "map_via_page_table, could not map pa = %x\n", pa );
    return( msg_failure );
    }
