/* file:	setshow.c
 *
 * Copyright (C) 1990 by
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
 *      This module implements the console SET, SHOW, and CLEAR commands.
 *
 *  AUTHORS:
 *
 *      Dave Mayo
 *
 *  CREATION DATE:
 *  
 *      30-Jul-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	noh	07-Nov-1995	Added conditional for BURNS on show_iobq 
 *				command.
 *
 *	rhd    	23-Jun-1995	init var    status = msg_cnt
 *
 *	rhd    	23-Jun-1995	Reduce code complexity / size
 *
 *	noh	30-Mar-1995	Removed show_iobq command for AVMIN.
 *
 *	cto	27-Jun-1994	Make IOBQ part of show command
 *
 *	mdr	17-Jan-1994	added set mode fastboot/nofastboot and
 *				show mode for medulla.
 *
 *	jrk	 3-Sep-1993	Overlay integration
 *
 *	kl	19-Aug-1993	merge
 *
 *	hcb	14-Jul-1993	Move showmem to memconfig_platform
 *
 *	hcb	16-Jun-1993	include csm_csr_def.h
 *
 *	hcb	23-Apr-1993	Change num_bnk_pair to num_banks to support
 *				one bank modules
 *
 *      joes    15-Apr-1993     Conditionalise for Medulla, no show mem, no
 *								cluster support.
 *
 *      hcb     07-Apr-1993     Handle unconfigured presto module in show mem
 *
 *      pel     08-Mar-1993     Implement show memory for Morgan.
 *
 *      hcb     03-Feb-1993     add PrestoServe NVRAM failure to show memory
 *
 *      cbf     22-Jan-1993     modify set password
 *
 *	pel	15-Jan-1993	Conditionalize for morgan
 *
 *      hcb     31-Dec-1992     add PrestoServe NVRAM to show memory
 *
 *      cbf     23-Dec-1992     add secure features
 *
 *      pel     17-Nov-1992     add EV$K_USER option when calling ev_write from
 *                              set command w/ argc == 2 and not -d qualifier
 *
 *	hcb	21-Sep-1992	add pscreen.h and use defines
 *
 *	db	05-Aug-1992	add warnings to show boot and show bflags
 *
 *	ajb	22-Jul-1992	Conditionalize show cluster for jensen
 *
 *	ajb	14-Jul-1992	Put in JENSEN conditionals
 *
 *	hcb	13-Jul-1992	Fix 4-way interleave display in show memory
 *
 *	hcb	24-Jun-1992	add not configured message to show memory
 *
 *	hcb	04-Jun-1992	only show total bad pages not per board
 *
 *	hcb	27-May-1992	include eerom_def
 *
 *	ajb	11-May-1992	Remove ev_scan
 *
 *	db	11-May-1992	Fix the Pele build
 *
 *	db	08-May-1992	add show fru and show error
 *
 *	hcb	16-Apr-1992	Add show memory for Cobra
 *
 *	kp	09-Jan-1992	Remove CB_PROTO usage, take out DUP if MINIMAL.
 *
 *	kl	18-Mar-1991	Replace setcpu command with environment variables
 *				and validation/action routines.
 *
 *	kl	12-Feb-1991	Switch rx in setcpu
 *
 *	dtm	20-Dec-1990	add set -d support.  fix printf messages.
 *
 *	dtm	11-Dec-1990	Add SHOW CONFIG.
 *
 *	sfs	07-Nov-1990	Add SHOW DEVICE.
 *
 *	dtm	29-Oct-1990	call qscan prior to argc conditional in
 *				set/show.
 *
 *	kl	18-Oct-1990	modify setcpu,showcpu,sethost
 *
 *	dtm	15-Oct-1990	remove local environment variables. 
 *				add ev_def.h
 *
 *	dtm	05-Oct-1990	remove set language
 *
 *	kl	05-Oct-1990	Setcpu,showcpu,sethost
 *
 *	dtm	14-Sep-1990	EVNODE value field changed to union.
 *				Caller now allocates storage for ev_read data.
 *				other modifications based on techinical review.
 *
 *	dtm	11-Sep-1990	Changes for wildcard expansion of SET command.
 *				Cleanup from CC/STANDARD=PORTABLE.
 *
 *	cfc	06-Sep-1990	add wild card expansion to qscan call.
 *	dtm			Add environment variable wildcard expansion
 *				for SHOW and CLEAR.
 *
 *	dtm	30-Jul-1990	Initial entry.
 *--
 */

#include        "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:parse_def.h"
#include "cp$src:parser.h"
#include "cp$src:version.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ctype.h"
#include "cp$src:stddef.h"
#include "cp$src:prdef.h"
#include "cp$src:msg_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:pb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:cb_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:tt_def.h"
#include "cp$src:regexp_def.h"
#include "cp$inc:kernel_entry.h"

#define malloc(x) dyn$_malloc(x,DYN$K_SYNC)
#define free(x) dyn$_free(x,DYN$K_SYNC)
#define TRUE 1
#define FALSE 0

#if MEDULLA
extern struct pb *siopb;
extern unsigned char inportb(struct pb *pb, int offset);
extern void outportb(struct pb *pb, int offset, unsigned char data) ;

#if MEDULLA_PASS_NUMBER == 1 
extern xlate_rtc_addr(unsigned long offset);
#endif
#endif

#if SECURE
extern int secure;
extern void set_secure();
#endif

extern strcmp_pb(struct pb *pb1, struct pb *pb2);
extern strcmp_sb(struct sb *sb1, struct sb *sb2);
extern strcmp_ub(struct ub *ub1, struct ub *ub2);

extern struct QUEUE pcbq;
extern int spl_kernel;
extern int in_console;
extern int cpu_mask;
extern int primary_cpu;
extern int null_procedure();
extern int show_fru(int argc, char *argv[]);
extern int show_error(int argc, char *argv[]);
extern int show_iobq(int argc, char *argv[]);
extern void show_power( int argc, char *argv[] );
extern void show_cpu( int argc, char *argv[] );

extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
extern unsigned int num_pbs_all;
extern struct SEMAPHORE sbs_lock;
extern struct sb **sbs;
extern unsigned int num_sbs;
extern unsigned int num_sbs_all;
extern struct SEMAPHORE ubs_lock;
extern struct ub **ubs;
extern unsigned int num_ubs;
extern unsigned int num_ubs_all;

extern struct SEMAPHORE evlock;
extern struct EVNODE evlist;
extern struct TTPB *console_ttpb;
extern int timer_cpu;

/*+
 * ============================================================================
 * = set - Set or modify the value of an environment variable.                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Sets or modifies the value of an environment variable. 
 *	Environment variables are used to pass configuration information
 *	between the console and the operating system.
 *  
 *   COMMAND FMT: set 2 H 0 set
 *
 *   COMMAND FORM:
 *
 *	set ( <envar> <value>
 *            [-integer] [-string]
 *            where
 *            <envar>={auto_action,bootdef_dev,boot_file,boot_osflags,...} )
 *  
 *   COMMAND TAG: set 0 RXBZ set se
 *
 *   COMMAND ARGUMENT(S):
 *
 *	<envar> - The environment variable to be assigned a new value.
 *		  Refer to the list of commonly used environment variables
 *		  below.
 *
 * 	<value> - The value that is assigned to the environment variable.
 *		  Either a numeric value or an ASCII string.
 *
 *   COMMAND OPTION(S):
 *
 *	-integer - Creates an environment variable as an integer. 
 *	-string  - Creates an environment variable as a string. 
 *
 *   ENVIRONMENT VARIABLE(S):
 *
 *	auto_action   - Sets the console action following an error, halt,  
 *			or power-up, to halt, boot, or restart. 
 *			The default setting is halt. 
 *
 *	bootdef_dev   - Sets the default device or device list from which the
 *			system attempts to boot.  For systems which ship with 
 *			factory installed software (FIS), the default device is
 *			preset at the factory to the device that contains FIS.
 *			For systems which do not ship with FIS, the default
 *			setting is null.
 *
 *	boot_file     - Sets the file name to be used when a bootstrap
 *                      requires a file name. The default setting is null. 
 *
 *	boot_osflags  - Sets additional parameters to be passed to
 *                      system software. When using VMS software, these 
 *			parameters are the system root number and boot flags. 
 *			The default setting is 0,0. 
 *
 *	tta*_baud     - Sets the baud rate for the auxilliary serial port.
 *			Possible settings are 600, 1200, 2400, 4800, 
 *			9600, or 19,200. The default setting is 1200. 
 *
 *	tta*_halts    - Specifies halt characters recognized on the console
 *			serial ports, tta0 and tta1.  The value is an integer
 *			bitmap, where:
 *#o
 *			o bit 0 : Enables(1) or disables(0) CTRL-P to init
 *				from the console.
 *			o bit 1 : Enables(1) or disables(0) CTRL-P halts
 *				from the operating system.
 *			o bit 2 : Enables(1) or disables(0) BREAK halts
 *				from the operating system.  Note, since tta1
 *				is intended for modem support, this bit is
 *				ignored on tta1 (i.e. BREAKs are not permitted
 *				on the auxiliary port).
 *#
 *			The default for tta0 is a 2, enabling CTRL-P halts
 *			from the operating system.
 *			The default for tta1 is a 0, disabling halts
 *			from the operating system.
 *
 *   COMMAND EXAMPLE(S):
 *
 *	In the following example, the default device from which the 
 *	system attempts to boot is set to eza0.
 *~
 *	>>>set bootdef_dev eza0
 *~
 *	In the next example, the system's default console action following 
 *	error, halt, or power-up is set to boot. 
 *~
 *	>>>set auto_action boot 
 *~
 *	In the next example, the file name to be used when the system's boot 
 *	requires a file name is set to vax_4000.sys.  
 *~
 *	>>>set boot_file vax_4000.sys 
 *~
 *	In the next example, the system's default boot flags are set to 0,1. 
 *~ 
 *	>>>set boot_osflags 0,1
 *~
 *	In the next example, the Baud rate of the auxillary console serial
 *	port, tta1, is set to 1200.
 *~ 
 *	>>>set tta1_baud 1200 
 *~
 *	In the next example, an environment variable called foo is created
 *	and given a value of 5.
 *~
 *	>>>set foo 5
 *~ 
 *
 *   COMMAND REFERENCES:
 *
 *      clear, set host, show
 *
 * FORM OF CALL:
 *  
 *	set( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

#define QD	0			/* qualifier -d */
#define QI	1			/* force env to be an integer */
#define QS	2			/* force env to be a string */
#define QMAX	3
#define QSTRING "default integer string"

set(int argc, char *argv[])
{
    char *arg;
    int i, status, rstat, new, len;
    struct QSTRUCT qual[QMAX];
    struct EVNODE *evp, *temp_evp, evn;

    evp = &evn;

    if (argc <= 1) {
	printf(msg_insuff_params);
	return msg_failure;
    }

    status = msg_cnt;

#if MEDULLA
/* set mode ... */

    if (substr_nocase(argv[1], "mode") > 0) {
	setmode(argc, argv);
	return msg_success;
    }
#endif

/* set host ... */

    if (substr_nocase(argv[1], "host") > 0) {
	sethost(argc, argv);
	status = msg_success;
    }

/* set boot ... */

    else if (strcmp_nocase(argv[1], "boot") == 0) {
	err_printf("Use bootdef_dev instead.\n");
	status =  msg_success;
    }

/* set bflag ... */

    else if (strncmp_nocase(argv[1], "bfl", 3) == 0) {
	err_printf("Use boot_osflags instead.\n");
	status =  msg_success;
    }

#if SECURE
/* set password ... */

    else if (substr_nocase(argv[1], "password") > 0) {

	/* do not except a password as a parameter */
	if (argc > 2) {
	    err_printf(msg_cmd_noparams);
	    return msg_success;
	}

/* get the old password and pass as parameter */

	set_secure(0);
	ev_read("password", &evp, EV$K_SYSTEM);
	ev_write(argv[1], evp->value.integer, EV$K_INTEGER | EV$K_USER);
	status = msg_success;
    }

/* set secure */

    else if (substr_nocase(argv[1], "secure") > 0) {

	/* if there's a valid password, set secure */
	set_secure(1);
	if (secure)
	    err_printf(msg_cnsl_secure);
	else
	    err_printf(msg_no_vldpsswd);
	status = msg_success;
    }
#endif

    if ( status != msg_cnt )
	return (status);

/* set <evname> <value> */

    qscan(&argc, argv, "-", QSTRING, qual);

    if (qual[QD].present) {
	err_printf("-d obsolete\n");
	return msg_qual_conflict;
    }

    if (argc > 3) {
	if (qual[QI].present) {
	    err_printf(msg_extra_params);
	    return msg_extra_params;
	}
	len = 0;
	for (i = 2; i < argc; i++) {
	    len += strlen(argv[i]) + 1;
	}
	arg = malloc(len);
	arg[0] = 0;
	for (i = 2; i < argc; i++) {
	    if (i>2) strcat( arg, " ");
	    strcat(arg, argv[i]);
	}
	argc = 3;
	argv[2] = arg;	
    }

    if (argc == 3) {
	krn$_wait(&evlock);
	rstat = ev_locate(&evlist, argv[1], &temp_evp);
	krn$_post(&evlock);
	new = ((rstat == msg_noev) ? 1 : 0);

	if (qual[QI].present) {
	    rstat = ev_write(argv[1], xtoi(argv[2]), EV$K_INTEGER | EV$K_USER);
	} else {
	    rstat = ev_write(argv[1], argv[2], EV$K_STRING | EV$K_USER);
	}
	err_printf(rstat, argv[1]);
	if (new && (ev_read(argv[1], &evp, 0) == msg_success)) {
	    err_printf("environment variable %s created\n", evp->name);
	}
	return msg_success;
    }

/* set <evname> */

    if (argc == 2) {
	if (qual[QI].present) {
	    err_printf(ev_write(argv[1], 0, EV$K_INTEGER | EV$K_USER), argv[1]);
	} else {
	    err_printf(ev_write(argv[1], "", EV$K_STRING | EV$K_USER), argv[1]);
	}
	return msg_success;
    }
    return msg_failure;
}
#undef QD
#undef QI
#undef QS
#undef QMAX
#undef QSTRING

create()
{
}

/*+
 * ============================================================================
 * = show - Display an environment variable value or other information.       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Displays the current value (or setting) for an environment 
 *	variable that you specify. 
 *
 * 	Also displays other information about the system, according to the 
 *	arguments that you enter on the command line. For example,  
 *	you can display the system configuration by entering 'show config'. 
 * 
 *   COMMAND FMT: show 3 H 0 show
 *
 *   COMMAND FORM:
 *
 *	show ( [{config,device,hwrpb,memory,pal,version,...}]
 *             [<envar>]
 *             where:
 *             <envar>={auto_action,bootdef_dev,boot_file,boot_osflags,...} )
 *  
 *   COMMAND TAG: show 0 RXB show sho sh
 *
 *   COMMAND ARGUMENT(S):
 *
 *	cluster - Displays open virtual circuits.
 *	config  - Displays the current system configuration.
 *	device  - Displays devices and controllers in the system.
 *	error   - Displays error log information.
 *	fbus    - Displays Futurebus+ devices.
 *	fru     - Displays system FRU information.
 *	hwrpb   - Displays the Alpha HWRPB.
 *	map     - Displays system virtual memory map.
 *	memory  - Displays the memory module configuration.
 *	overlay - Displays the overlay configuration.
 *	pal     - Displays the versions of VMS and OSF PALcode.
 *	version - Displays the verison of the console firmware.
 *	memory  - Displays the memory module configuration.
 *	<envar> - Displays the value of the environment variable specified.
 *		  Refer to the list of commonly used environment variables
 *		  below.
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   ENVIRONMENT VARIABLE(S):
 *
 *	auto_action   - Displays the console action following an error halt 
 *			or power-up: either halt, boot, or restart. 
 *
 *	baud_modem    - Displays the baud rate for the auxilliary console port.
 *
 *	bootdef_dev   - Displays the device or device list from which 
 *			bootstrapping is attempted.
 *
 *	boot_file     - Displays the file name to be used when a bootstrap
 *                      requires a file name.
 *
 *	boot_osflags  - Displays the additional parameters to be passed to
 *                      system software.
 *
 *	eza0_rm_boot, ezb0_rm_boot - Displays the state of MOP remote boot
 *			enable.  A 0 indicates enabled, a 1 indicates disabled.
 *
 *	language      - Displays the language in which system software and 
 *			layered products are displayed. 
 *
 *	password      - Displays whether a password is required to gain 
 *			access to privileged console commands. 	
 * 
 *   COMMAND EXAMPLE(S):
 *
 *	In the following example, the version of firmware on a 
 *	system is displayed. The firmware version is X1.4-2129. 
 *~
 *	>>>show version
 *	version                 V3.0-1 Sep 20 1992 00:28:54
 *	>>>
 *~	
 *	In the next example, the default system powerup action is displayed.
 *~
 *	>>>show auto_action
 *	boot
 *	>>>
 *~
 *	In the next example, a system's default boot device is displayed. 
 *	The default boot device in the example is eza0. 
 *~
 *	>>>show bootdef_dev
 *	eza0
 *	>>>
 *~
 *	In the next example, the baud rate for a system's auxillary 
 *	console port is displayed. The baud rate in the example is 1200. 
 *~
 *	>>>show baud_modem 
 *	1200
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	clear, set, show cluster, show config, show device, show error
 *	show fru, show fbus, show hwrpb, show map, show memory
 *   
 * FORM OF CALL:
 *  
 *	show( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Indicates normal completion.
 *       
 * ARGUMENTS:
 *
 *	int argc - The number of command line arguments passed by the shell
 *	char *argv [] - Array of pointers to arguments. 
 *
 * SIDE EFFECTS:
 *
 *      None
 *   
-*/

#define QL	0			/* qualifier -l */
#define QMAX	1

show(int argc, char *argv[])
{
    int i, status;
    struct EVNODE *evp, *evp_copy;
    struct REGEXP **r;
    struct QSTRUCT qual[QMAX];
    extern int HWRPB;

    if (argc != 1) {

#if MEDULLA
/* show mode ... */

    if (substr_nocase(argv[1], "mode") > 0) {
	showmode(argc, argv);
	return msg_success;
    }
#endif

    status = msg_cnt;

    if (substr_nocase(argv[1], "config") > 0) {
	showconfig(argc, argv);
	status = msg_success;
    }
    else if (strcmp_nocase(argv[1], "boot") == 0) {
	err_printf("Use bootdef_dev instead.\n");
	status = msg_failure;
    }
    else if (strncmp_nocase(argv[1], "bfl", 3) == 0) {
	err_printf("Use boot_osflags instead.\n");
	status = msg_failure;
    }
    else if (substr_nocase(argv[1], "device") > 0) {
	showdevice(argc, argv);
	status = msg_success;
    }
    else if (substr_nocase(argv[1], "raid") > 0) {
	showraid(argc, argv);
	status = msg_success;
    }
    else if (substr_nocase(argv[1], "memory") > 0) {
	showmem(argc, argv);
	status = msg_success;
    }
    else if (substr_nocase(argv[1], "hwrpb") > 0) {
	showhwrpb();
	status = msg_success;
    }
    else if (substr_nocase(argv[1], "overlay") > 0) {
	err_printf("not implemented\n");
	status = msg_failure;
    }
    else if (substr_nocase(argv[1], "map") > 0) {
	showmap(argc, argv);
	status = msg_success;
    }
    else if (substr_nocase(argv[1], "cluster") > 0) {
#if MSCP
	showcluster(argc, argv);
	status = msg_success;
#else
	err_printf("not implemented\n");
	status = msg_failure;
#endif
    }
    else if (substr_nocase(argv[1], "fru") > 0) {
	show_fru(argc, argv);
	status = msg_success;
    }
    else if (substr_nocase(argv[1], "error") > 0) {
	show_error(argc, argv);
	status = msg_success;
    }
    else if (substr_nocase(argv[1], "power") > 0) {
	show_power(argc, argv);
	status = msg_success;
     }
    else if (substr_nocase(argv[1], "cpu") > 0) {
	show_cpu(argc, argv);
	status = msg_success;
     }
#if !AVMIN
    else if (substr_nocase(argv[1], "iobq") > 0) {
	show_iobq(argc, argv);
	status = msg_success;
    }
#endif

#if TAKARA || K2 || EIGER
    else if (substr_nocase(argv[1], "power") > 0) {
	srvmgr(argc,argv);
	status = msg_success;
     }
    else if (substr_nocase(argv[1], "srvmgr") > 0) {
	srvmgr(argc,argv);
	status = msg_success;
     }
    else if (substr_nocase(argv[1], "pci") > 0) {
	show_pci_config(0);
	status = msg_success;
     }
    else if (substr_nocase(argv[1], "isa") > 0) {
	show_isa_config();
	status = msg_success;
     }
    else if (substr_nocase(argv[1], "srom") > 0) {
	show_srom_version();
	status = msg_success;
     }
#endif  /* takara/k2 */

    if ( status != msg_cnt )
	return (status);

/* must be envar */

    }

    qscan(&argc, argv, "-", "l", qual);

    if (argc == 1) {
	argc = 2;
	argv[1] = "*";
    }
    r = malloc(sizeof(struct REGEXP *) *argc);
    for (i = 1; i < argc; i++) {
	r[i] = regexp_shell(argv[i], 1);
    }
    evp_copy = malloc(sizeof(struct EVNODE));

    krn$_wait(&evlock);
    evp = evlist.flink;

    while (evp != &evlist) {
	if (((evp->attributes & EV$M_UNUSED) == EV$K_IN_USE) &&
	  ((evp->attributes & EV$M_READ) == EV$K_READ)) {
	    for (i = 1; i < argc; i++) {
		if (regexp_match(r[i], evp->name)) {
		    memcpy(evp_copy, evp, sizeof(struct EVNODE));
		    if (evp->rd_action)
			evp->rd_action(evp->name, evp, evp_copy);
		    ev_render(evp_copy, qual[QL].present);
		}
	    }
	}
	evp = evp->flink;
    }
    krn$_post(&evlock);
    for (i = 1; i < argc; i++) {
	regexp_release(r[i]);
    }
    free(r);
    free(evp_copy);
    return msg_success;
}
#undef QL
#undef QMAX

/*+
 * ============================================================================
 * = clear - Clear an option or an environment variable.                       =
 * ============================================================================
 *              
 * OVERVIEW:
 *  
 *	Clear an option or
 *	Deletes an environment variable from the name space.
 *
 *   	Note that some environment variables, such as bootdef_dev, are 
 *	permanent and cannot be deleted.
 *  
 *   COMMAND FMT: clear 2 H 0 clear
 *
 *   COMMAND FORM:
 *  
 *	clear ( [option] or envar )
 *  
 *   COMMAND TAG: clear 0 RXBZ clear
 *
 *   COMMAND ARGUMENT(S):
 *
 *	option - name of the option to be cleared.
 * 	<envar> - Specifies the name of the environment variable to be cleared.
 *
 *   COMMAND OPTION(S):
 *
 * 	None
 *
 *   COMMAND EXAMPLE(S):
 *
 *	In the following example, an environment variable, 'foo', is deleted. 
 *~
 *	>>>clear foo
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	set, show
 *
 * FORM OF CALL:
 *  
 *	clear( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *   
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

clear(int argc, char *argv[])
{
    int i, status, ev_cnt = 0;
    struct EVNODE *evp, evn;

    evp = &evn;

    if (argc < 2) {
	printf(msg_insuff_params);
	return msg_failure;
    }

/* visit each variable to be cleared */

    for (i = 1; i < argc; i++) {

#if SECURE
/* password needs to be special cased */
	if (!strcmp_nocase(argv[i], "password")) {
	    clear_password();
	} else {
	    printf(ev_delete(argv[i], EV$K_USER), argv[i]);
	}
#else
	printf(ev_delete(argv[i], EV$K_USER), argv[i]);
#endif
    }
}

/*+
 * ============================================================================
 * = ev_render - Output contents of an environment variable to STDOUT.       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Outputs the contents of a specified environment variable in readable 
 * 	form to STDOUT.
 *   
 * FORM OF CALL:
 *  
 *	ev_render ( evp, longformat )
 *  
 * RETURN CODES:
 *
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 *	struct EVNODE *evp  - pointer to environment variable node
 * 	int longformat      - 0=abbreviated;  non-zero=long format
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

ev_render(struct EVNODE *evp, int longformat)
{

    if (longformat) {
	printf("%c", evp->attributes & EV$M_READ ? '-' : 'r');
	printf("%c", evp->attributes & EV$M_WRITE ? '-' : 'w');
	printf("%c", evp->attributes & EV$M_DELETE ? '-' : 'd');
	printf(" ");
	printf("%c", evp->attributes & EV$M_VOLATILE ? 'n' : 'v');
	printf("%c", evp->attributes & EV$M_CLASS ? 'g' : 'l');
	printf("%c", evp->attributes & EV$M_TYPE ? 'i' : 's');
	printf("  ");
    }
    switch (evp->attributes & EV$M_TYPE) {
	case EV$K_STRING: 
	    printf("%-20s\t%-16s\n", evp->name, evp->value.string);
	    break;
	case EV$K_INTEGER: 
	    printf("%-20s\t%-16x\n", evp->name, evp->value.integer);
	    break;
    }
    return msg_success;
}

#if MEDULLA
setmode(int argc, char *argv[])
{
    unsigned int nv_reg;

    if (argc < 3) {
	printf(msg_insuff_params);
	return msg_failure;
    }

    if (argc > 3){
       printf(msg_extra_params);
       return msg_failure;
    }

#if MEDULLA_PASS_NUMBER == 1 
	nv_reg = xlate_rtc_addr ((unsigned long)0x8010);
#else
	nv_reg = 0x8010;
#endif
    if ( strcmp_nocase("fastboot", argv[2]) == 0 )
       outportb(siopb, (int)nv_reg, (unsigned char)0x1);

    else if ( strcmp_nocase("nofastboot", argv[2]) == 0 )
            outportb(siopb, (int)nv_reg, (unsigned char)0x0);

    else{
	printf("invalid mode %s\n", argv[2]);
	return msg_failure;
    }

    return msg_success;
}

showmode(int argc, char *argv[])
{
    unsigned int nv_reg;

    if (argc != 2) {
	printf(msg_invalid_cmd);
	return msg_failure;
    }

#if MEDULLA_PASS_NUMBER == 1 
	nv_reg = xlate_rtc_addr ((unsigned long)0x8010);
#else
	nv_reg = 0x8010;
#endif
	
    printf("Console is in ");

	if(inportb(siopb, (int)nv_reg))
	  printf("fastboot");
	else
	  printf("nofastboot");

    printf(" mode\n");

    return msg_success;
}
#endif

/*+
 * ============================================================================
 * = switch_primary - switch primary					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Switch the console's receiver.
 *  
 * FORM OF CALL:
 *  
 *	switch_primary ( id )
 *  
 * RETURN CODES:
 *   
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 * 	int id - new primary
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
switch_primary(int id)
{

    struct PCB *pcb, *parent;

    /* turn on the new primary's receiver */

    pcb = getpcb();

    krn$_create(console_ttpb->rxon, 	/* address of process           */
      null_procedure, 			/* startup routine              */
      0, 				/* completion semaphore         */
      6, 				/* process priority             */
      1 << id, 				/* cpu affinity mask            */
      1024 * 8, 			/* stack size                   */
      "rxon", 				/* process name                 */
      "tt", 				/* stdin                        */
      "r", "tt", 			/* stdout                       */
      "w", "tt", 			/* stderr                       */
      "w", 0, 0, 0, 0);

    console_ttpb->rxoff();
    parent = krn$_findpcb(pcb->pcb$l_parentpid);
    parent->pcb$l_affinity = 1 << id;
    primary_cpu = id;
    timer_cpu = id;
    spinunlock(&spl_kernel);
    return msg_success;
}

/*+
 * ============================================================================
 * = set host - Connect the console to another console or server.            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Connects the console to another console or server. By default, 
 *	the set host command connects to another processor's console. 
 *	Using the -dup option invokes the DSSI DUP server on the 
 *	selected node. You can use the DUP protocol to examine and 
 *	modify parameters of a DSSI device. 
 *
 *	In the syntax, node is the node number of the processor or 
 *	device to attach the console. When you specify the dup options, 
 *	node must be in the range of 0 to 7. Task is the task to invoke 
 *	from the DUP driver. 
 *
 *	In a multiprocessor system, you can run the console on another 
 *	processor by entering 'set host n', where 'n' is the name of the 
 *	processor on which you want to run the console. The processor  
 *	affinity of the console shell will then switch to processer n. 
 *	Thereafter, all commands that you enter on the console will 
 *      execute on processor 'n'.
 *
 *   COMMAND FORM:
 *  
 *   	set host ( [-dup] [-task <task_name>] <target> )
 *  
 *   COMMAND ARGUMENT(S):
 *
 *	<target> - Specifies the target processor number or the
 *		 target device for the service connection.
 *
 *   COMMAND OPTION(S):
 *
 *	-dup -  Specifies connection to an MSCP DUP server.  The DUP service
 *		may be used to examine and modify parameters of a DSSI device.
 *
 *	-task <task_name> - Specifies which DUP service utility to invoke. 
 *		Refer to example below for a list of utilities.
 *		Note that in the absence of this qualifier, a directory 
 *		of utilities is displayed.
 *
 *   COMMAND EXAMPLE(S):
 *
 *	The following example shows how to change the console to run on another
 *	processor.
 *~
 *	>>> set host 1
 *	>>> 
 *~
 *	The next example shows how to connect to the MSCP DUP server
 *	on a disk.
 *~
 *      >>>show device du
 *      dud0.0.0.3.0               R2YQYA$DIA0                    RF72
 *      >>>set host -dup dud0
 *      starting DIRECT on pud0.0.0.3.0 (R2YQYA)
 *       
 *      Copyright (C) 1990  Digital Equipment Corporation
 *      PRFMON V1.0  D  2-NOV-1990 10:30:58
 *      DKCOPY V1.0  D  2-NOV-1990 10:30:58
 *      DRVEXR V2.0  D  2-NOV-1990 10:30:58
 *      DRVTST V2.0  D  2-NOV-1990 10:30:58
 *      HISTRY V1.1  D  2-NOV-1990 10:30:58
 *      DIRECT V1.0  D  2-NOV-1990 10:30:58
 *      ERASE  V2.0  D  2-NOV-1990 10:30:58
 *      VERIFY V1.0  D  2-NOV-1990 10:30:58
 *      DKUTIL V1.0  D  2-NOV-1990 10:30:58
 *      PARAMS V2.0  D  2-NOV-1990 10:30:58
 *      Total of 10 programs.
 *      Task?
 *      >>>set host -dup -task params dud0
 *      starting PARAMS on pud0.0.0.3.0 (R2YQYA)
 *      
 *      Copyright (C) 1990  Digital Equipment Corporation
 *      
 *      PARAMS> show allclass
 *      
 *      Parameter     Current           Default        Type     Radix
 *      --------- ----------------  ----------------  --------  -----
 *      ALLCLASS                 0                 0      Byte    Dec   B
 *      
 *      PARAMS> exit
 *      Exiting...
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *
 *	cdp
 *
 * FORM OF CALL:
 *  
 *	sethost( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Indicates success. 
 *       
 * ARGUMENTS:
 *
 *	int argc - The number of command line arguments passed by the shell. 
 *   	char *argv [] - Array of pointers to arguments.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
#define QDUP	0			/* qualifier -dup */

sethost(int argc, char *argv[])
{
    int id;
    struct QSTRUCT qual[1];
    struct PCB *pcb;

    qscan(&argc, argv, "-", "dup", qual);
    if (qual[QDUP].present) {
#if MSCP 
	return sethost_dup(argc, argv);
#else
	err_printf("not implemented\n");
	return msg_failure;
#endif
    }
    if (argc == 3) {
	id = xtoi(argv[2]);
    } else {
	printf("set host <CPU ID>\n");
	return (int) msg_success;
    }

    if (!(cpu_mask & (1 << id))) {
	printf("invalid CPU ID\n");
	return (int) msg_success;
    }

    if (!(in_console & (1 << id))) {
	printf("CPU %d is not halted\n", id);
	return (int) msg_success;
    }

    pcb = getpcb();
    pcb = krn$_findpcb(pcb->pcb$l_parentpid);
    pcb->pcb$l_affinity = 1 << id;
    spinunlock(&spl_kernel);
    reschedule(1);
    return (int) msg_success;
}

#define QTASK 0
#define QBG 1

#if MSCP

sethost_dup(int argc, char *argv[])
{
    char *task;
    int i;
    int j;
    int k;
    int t;
    struct QSTRUCT qual[2];
    struct sb *sb;
    struct ub *ub;

    qscan(&argc, argv, "-", "%stask bg", qual);
    if (qual[QTASK].present) {
	task = dyn$_malloc(strlen(qual[QTASK].value.string) + 1,
	  DYN$K_SYNC | DYN$K_NOOWN);
	strcpy(task, qual[QTASK].value.string);
    } else
	task = 0;
    if (argc < 3) {
	argc = 3;
	argv[2] = "";
    }
    krn$_wait(&sbs_lock);
    shellsort(sbs, num_sbs, strcmp_sb);
    for (i = 0; i < num_sbs; i++) {
	if (killpending())
	    break;
	sb = sbs[i];
	if (sb->state == sb_k_open) {
	    t = 0;
	    for (j = 2; j < argc; j++) {
		if (substr_nocase(argv[j], sb->string)) {
		    t = 1;
		    break;
		}
		if (strcmp(argv[j], sb->alias) == 0) {
		    t = 1;
		    break;
		}
		krn$_wait(&ubs_lock);
		for (k = 0; k < num_ubs; k++) {
		    ub = ubs[k];
		    if (substr_nocase(argv[j], ub->string) && (sb == ub->sb)) {
			t = 1;
			break;
		    }
		}
		krn$_post(&ubs_lock);
		if (t)
		    break;
	    }
	    if (t)
		sethost_dup_sub(sb, qual, task);
	}
    }
    krn$_post(&sbs_lock);
}

sethost_dup_sub(struct sb *sb, struct QSTRUCT qual[], char *task)
{
    int dup_begin();
    struct PCB *pcb;

    pcb = getpcb();
    if (qual[QBG].present) {
	krn$_create(dup_begin, null_procedure, 0, 5, -1, 4096, "dup_begin",
	  "nl", "r", pcb->pcb$a_stdout_name, "w", "nl", "w", sb, task, 1);
	krn$_sleep(1000);
    } else
	dup_begin(sb, task, 0);
}
#endif

#if MSCP
/*+
 * ============================================================================
 * = show cluster - Display open virtual circuits.                            =
 * ============================================================================
 *
 * OVERVIEW:
 *          
 *      Shows any open virtual circuits in the system on DSSI.
 *
 *   COMMAND FORM:
 *  
 *   	show cluster ( )
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
 *	In the following example, a system's cluster is displayed. 
 *~
 *	>>>show cluster
 *      pud0.0.0.3.0               R2YQYA               RF72/RFXX T251
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	show device
 *
 * FORM OF CALL:                             
 *  
 *	showcluster( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Indicates success. 
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
showcluster(int argc, char *argv[])
{
    int i;
    int j;
    struct sb *sb;

    if (argc < 3) {
	argc = 3;
	argv[2] = "";
    }
    krn$_wait(&sbs_lock);
    shellsort(sbs, num_sbs, strcmp_sb);
    for (i = 0; i < num_sbs; i++) {
	sb = sbs[i];
	if (sb->state == sb_k_open)
	    for (j = 2; j < argc; j++)
		if (substr_nocase(argv[j], sb->string)) {
		    if (*sb->string)
			printf("%s\n", sb->string);
		    break;
		}
    }
    krn$_post(&sbs_lock);
}

#endif

/*+         
 * ============================================================================
 * = show device - Display devices and controllers in the system.             = 
 * ============================================================================
 *
 * OVERVIEW:                                 
 *
 *	Shows the devices and controllers in the system.
 *	By default all devices and controllers which respond are shown.
 *	If you enter a device name or an abbreviation, show device displays 
 *	the device or devices that match the device name or abbreviation.  
 *
 *	The device naming convention is as follows.
 *~
 *      dka0.0.0.0.0
 *      | || | | | |
 *      | || | | | +--- Bus # : 0 LBus                     1 FutureBus+
 *      | || | | +---- Slot # : 0-4 DSSI/SCSI 6,7 Enet     2-13 Fbus Nodes
 *      | || | +--- Channel # : Used for multi-channel devices.
 *      | || +---- Bus Node # : Device's bus ID (i.e. DSSI node ID plug #).
 *      | |+----Device Unit # : Device's unique system unit number.
 *      | +---- Controller ID : One letter controller designator.
 *      +---------- Driver ID : Two letter port or class driver designator.
 *                              PU - DSSI port, DU - MSCP class
 *                              PK - SCSI port, DK - SCSI class
 *                              EZ - Ethernet Port 
 *~	
 *   cmd FORM:
 *  
 *   	show device ( [<device_name>] )
 *  
 *   cmd ARGUMENT(S):
 *          
 *	<device_name> - Specifies the device name or an abbreviation of a device
 *		name.  When an abbreviation or wildcarding is used, all devices
 *		which match the abbreviated name are shown.
 *
 *   cmd OPTION(S):
 *
 *	None                                 
 *
 *   cmd EXAMPLE(S):
 *
 *	The following example shows all devices and controllers in the system.
 *	Note that the controllers 'p_a0' and 'p_b0' are indeterminant, that is,
 *	neither SCSI nor DSSI.  This occurs when no devices or terminators are
 *	present. 
 *~
 *	>>>show device
 *      dkc0.0.0.2.0               DKC0                           RZ57
 *      dud0.0.0.3.0               R2YQYA$DIA0                    RF72
 *      mke0.0.0.4.0               MKE0                          TLZ04
 *      eza0.0.0.6.0               EZA0              08-00-2B-1D-27-AA
 *      ezb0.0.0.7.0               EZB0              08-00-2B-1D-27-AB
 *      p_a0.7.0.0.0                                          Bus ID 7
 *      p_b0.7.0.1.0                                          Bus ID 7
 *      pkc0.7.0.2.0               PKC0                  SCSI Bus ID 7
 *      pke0.7.0.4.0               PKE0                  SCSI Bus ID 7
 *      pud0.7.0.3.0               PID0                  DSSI Bus ID 7
 *      >>>
 *~
 *	In the next examples, specific devices or device classes are displayed. 
 *~
 *      >>>show device e           # Show Ethernet devices.
 *      eza0.0.0.6.0               EZA0              08-00-2B-1D-27-AA
 *      ezb0.0.0.7.0               EZB0              08-00-2B-1D-27-AB
 *      >>>show device eza0        # Show Ethernet port 0.
 *      eza0.0.0.6.0               EZA0              08-00-2B-1D-27-AA
 *      >>>show device du          # Show DSSI disks.
 *      dud0.0.0.3.0               R2YQYA$DIA0                    RF72
 *      >>>show device *k*         # Show SCSI devices.
 *      dkc0.0.0.2.0               DKC0                           RZ57
 *      mke0.0.0.4.0               MKE0                          TLZ04
 *      >>>show device dk          # Show SCSI disks.
 *      dkc0.0.0.2.0               DKC0                           RZ57
 *      >>>show device mk          # Show SCSI tape drives.
 *      mke0.0.0.4.0               MKE0                          TLZ04
 *      >>>
 *~
 *   cmd REFERENCES:
 *
 *	show cluster, show config
 *
 * FORM OF CALL:
 *  
 *	showdevice( argc, *argv[] )
 *          
 * RETURN CODES:
 *
 *	msg_success - Indicates success. 
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

#if DRIVERSHUT
#define IO_ADAPTER_POLL 1
showdevice(int argc, char *argv[]) {
    int i;

    if (argc == 2 ) {
	poll_units(FALSE, TRUE, IO_ADAPTER_POLL);	
    } else {
	if (argc > 2) {
	    for (i = 2; i < argc; i++) {
		poll_units(argv[i], TRUE, IO_ADAPTER_POLL);
	    }
	}
    }
}
#else
showdevice(int argc, char *argv[])
{
    int i;
    int j;
    struct pb *pb;
    struct ub *ub;

    start_drivers ();

    if (argc < 3) {
	argc = 3;
	argv[2] = "";
    }
    krn$_wait(&ubs_lock);
    shellsort(ubs, num_ubs, strcmp_ub);
    for (i = 0; i < num_ubs; i++) {
	ub = ubs[i];
	for (j = 2; j < argc; j++)
	    if (substr_nocase(argv[j], ub->string)) {
		if (*ub->string)
		    printf("%s\n", ub->string);
		break;
	    }
    }
    krn$_post(&ubs_lock);
    krn$_wait(&pbs_lock);
    shellsort(pbs, num_pbs, strcmp_pb);
    for (i = 0; i < num_pbs; i++) {
	pb = pbs[i];
	for (j = 2; j < argc; j++)
	    if (substr_nocase(argv[j], pb->string)) {
		if (*pb->string)
		    printf("%s\n", pb->string);
		break;
	    }
    }
    krn$_post(&pbs_lock);
}
#endif

showraid(int argc, char *argv[])
{
    dac960_show(argc, argv);
}

build_cmd( int argc, char *argv[] )
{
}

