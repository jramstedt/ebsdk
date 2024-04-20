/* file:	startstop.c
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
 *	start, stop, init and continue commands
 *
 *  AUTHORS:
 *
 *      Kevin Lemieux
 *
 *  CREATION DATE:
 *  
 *      30-jan-1992
 *
 *	2-Feb-1999	"Add sys_init for YUKONA"
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$src:prdef.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:version.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:ev_def.h"
#include	"cp$src:tt_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$inc:kernel_entry.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:impure_def.h"
#include	"cp$src:aprdef.h"

#if TURBO
#include	"cp$src:mem_def.h"
#endif

unsigned int sys_init = 0;
                     
extern struct QUEUE pcbq;
extern int kbd_output(unsigned char c, int port);
extern int 	primary_cpu;
extern int	spl_kernel;	
extern int 	in_console;
extern		console_exit ();
extern		null_procedure ();
extern int 	hwrpb;
extern int	cpu_mask;
#if GALAXY
extern int	all_cpu_mask;
#endif
extern int	drivers_running;
extern int	cb_ref;
extern int	cb_fp[];
extern		console_restart ();

/*+
 * ============================================================================
 * = crash - crash the system.						      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Crash the system.  Used to instigate a hung operating system to
 *      crash in order to capture a crash dump.
 *
 *  
 *   COMMAND FMT: stop 3 BZ 8 crash
 *
 *   COMMAND FORM:
 *  
 *	crash ( ) 
 *  
 *   COMMAND TAG: stop 0 RXBZ crash
 *
 *   COMMAND ARGUMENT(S):
 *
 *      None
 * 
 *   COMMAND OPTION(S):
 *
 *      None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>crash
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	continue, init, start, stop
 *
 * FORM OF CALL:
 *  
 *	stop( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Normal operation.
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

/*+
 * ============================================================================
 * = halt - halt the specified processor or device.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Halts the specified processor or device.
 *	Equivelant to the STOP command.
 *  
 *   COMMAND FMT: stop 2 BZ 8 halt
 *
 *   COMMAND FORM:
 *  
 *	halt ( [-drivers [<device_prefix>]] [<processor_num>] )
 *  
 *   COMMAND TAG: stop 0 RXBZ halt h
 *
 *   COMMAND ARGUMENT(S):
 *
 *      <processor_num> - Specifies the number of the processor to stop.
 *
 *   COMMAND OPTION(S):
 *
 *      -drivers [<device_prefix>] - Specifies the name of the device or
 *		device class to stop.  If no device prefix is specified,
 *		then all drivers are stopped.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>halt
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	continue, init, start, stop
 *
 * FORM OF CALL:
 *  
 *	stop( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Normal operation.
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

/*+
 * ============================================================================
 * = stop - Stop the specified processor or device.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Stops the specified processor or device.
 *  
 *   COMMAND FMT: stop 3 BZ 8 stop
 *
 *   COMMAND FORM:
 *  
 *	stop ( [-drivers [<device_prefix>]] [<processor_num>] )
 *  
 *   COMMAND TAG: stop 0 RXBZ stop
 *
 *   COMMAND ARGUMENT(S):
 *
 *      <processor_num> - Specifies the number of the processor to stop.
 *
 *   COMMAND OPTION(S):
 *
 *      -drivers [<device_prefix>] - Specifies the name of the device or
 *		device class to stop.  If no device prefix is specified,
 *		then all drivers are stopped.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>stop 2 
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	continue, crash, init, start
 *
 * FORM OF CALL:
 *  
 *	stop( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Normal operation.
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
int stop (int argc, char *argv[]) {
        int id, status;
	struct QSTRUCT qual[2];
	struct PCB *pcb;

	status = qscan (&argc, argv, "-", "drivers crash", qual);
	if (status != msg_success) {
	    err_printf (status);
	    return status;
	}
	if (strncmp_nocase (argv[0], "c", 1) == 0) {
	    qual[1].present = 1;
	}
	if (qual[0].present && qual[1].present) {
	    err_printf (msg_qual_conflict);
	    return msg_qual_conflict;
	}
	if (qual[0].present) {
	    stop_drivers ();
	    return msg_success;
	}

	if (argc == 2) {
	    status = common_convert (argv[1], 10, &id, 4);
	    if (status) {
		err_printf(status);
		return status;
	    }
	} else {
	    id = whoami ();
	}

	if (id >= MAX_PROCESSOR_ID) {
	    err_printf("invalid processor ID\n");
	    return msg_failure;
	}

#if GALAXY
	if (!(all_cpu_mask & (1 << id))) {
#else
	if (!(cpu_mask & (1 << id))) {
#endif
	    err_printf("invalid processor ID\n");
	    return msg_failure;
	}

	if (qual[1].present) {
	    if (!in_console & (1 << id)) {
		node_halt (id);
	    }
	    pcb = getpcb ();
	    pcb->pcb$l_affinity = (1 << id);
	    reschedule (1);

	    restart_cpu (1);
	    return msg_success;
	}

	if (in_console & (1 << id)) {
	    err_printf("CPU %d is halted\n", id);
	    return msg_success;
	}

	node_halt (id);
	return msg_success;
}

#if !( STARTSHUT || DRIVERSHUT )
void start_drivers () {
	int i;

	if (drivers_running) {
	    driver_setmode (DDB$K_READY, 1);
	} else {
	    for (i = 0; i < cb_ref; i++)
		fclose (cb_fp[i]);
	    cb_ref = 0;
	    driver_setmode (DDB$K_STOP, 1);
	    reset_hardware (1);
	    krn$_set_console_mode (INTERRUPT_MODE);
	    driver_setmode (DDB$K_START, 1);
	    drivers_running = 1;
	}
}

void stop_drivers () {
	int i;

	if (drivers_running) {
	    for (i = 0; i < cb_ref; i++)
		fclose (cb_fp[i]);
	    cb_ref = 0;
	    driver_setmode (DDB$K_STOP, 1);
	    drivers_running = 0;
	    krn$_set_console_mode (POLLED_MODE);
	}
}
#endif

/*+
 * ============================================================================
 * = start - Start a processor at the specified address or drivers.           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Starts program execution on a processor at the specified address
 *	or start drivers.
 *  
 *   COMMAND FMT: start_cmd 3 BZS 8 start
 *
 *   COMMAND FORM:
 *  
 *	start ( [-drivers [<device_prefix>]] [<address>] )
 *  
 *   COMMAND TAG: start_cmd 0 RXBZP start
 *
 *   COMMAND ARGUMENT(S):
 *
 *	<address> - Specifies the PC address at which to start execution.
 *
 *   COMMAND OPTION(S):
 *
 *      -drivers [<device_prefix>] - Specifies the name of the device or
 *		device class to start.  If no device prefix is specified,
 *		then all drivers are started.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>start 400 	        # Start the primary at address 400.
 *	>>>start 100000 &p 2    # Start processor 2 at address 100000.
 *	>>>start -drivers       # Start the drivers in the system.
 *~
 *   COMMAND REFERENCES:
 *
 *	continue, init, stop
 *
 * FORM OF CALL:
 *  
 *	start_cmd( argc, *argv[] )
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

#define QDRIVERS	0

#define start_qstring "drivers" 

int start_cmd (int argc, char *argv[]) {
	int id, i, address[2],status;
	int ps[2];
	struct QSTRUCT qual[1];
	struct PCB *pcb;

	address[0] = 0;
	address[1] = 0;

	status = qscan (&argc, argv, "-", start_qstring, qual);
	if (status != msg_success) {
	    err_printf (status);
	    return status;
	}

	if (qual[QDRIVERS].present) {
	    start_drivers ();
	    return msg_success;
	}

	id = whoami ();

#if !MODULAR
	if (argc == 2) {
	    status = common_convert (argv[1], 10, &id, 4);
	    if (status) {
		err_printf(status);
		return status;
	    }
	}
#endif
#if MODULAR
	if (argc == 2) {
	    status = common_convert (argv[1], 16, &address[0], 4);
	    if (status) {
		err_printf(status);
		return status;
	    }

	    ps[1] = 0;
	    ps[0] = 0;
	    write_ipr( APR$K_PS, ps );
	    ps[0] = 0x1f;
	    write_ipr( APR$K_IPL, ps );
	    write_pc( address );
	    console_exit( );
	    return msg_success;
	}
#endif

	if (id >= MAX_PROCESSOR_ID) {
	    err_printf("invalid processor ID\n");
	    return msg_failure;
	}

	if (!(cpu_mask & (1 << id))) {
	    err_printf("invalid processor ID\n");
	    return msg_success;
	}

	if (!in_console & (1 << id)) {
	    err_printf("CPU %d is not halted\n", id);
	    return msg_success;
	}

	pcb = getpcb ();
	pcb->pcb$l_affinity = (1 << id);
	reschedule (1);

	secondary_start ();
	return msg_success;
}

/*+
 * ============================================================================
 * = continue - Resume program execution on the specified processor.          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Continues execution on the specified processor, or the primary 
 *	processor if a processor is not specified. The processor begins 
 *	executing instructions at the address currently contained in the 
 *      program counter. The processor is not initialized. 
 *
 *	The continue command is only valid if execution has been halted via
 *	the 'boot -halt' command, or if an operator has halted the 
 *	system by one of two methods: either by pressing the Halt button on 
 *	the control panel or by entering Ctrl/P on the console terminal. 
 *
 *	Note that some console commands, for example, test and init, may alter 
 *	the machine state so that program mode cannot be successfully resumed. 
 *  
 *   COMMAND FMT: continue_cmd 1 BNZS 8 continue
 *
 *   COMMAND FORM:
 *  
 *	continue ( )
 *  
 *   COMMAND TAG: continue_cmd 0 RXBZP continue c
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	None
 *
 *   COMMAND OPTION(S):
 *
 * 	None
 *
 *   COMMAND EXAMPLE(S):
 *
 *	In the following example, the primary processor resumes
 *	operating system mode. 
 *~
 *	>>>continue
 *~
 *	In the next example, a system's second processor is commanded to 
 *	resume operating system mode. 
 *~
 *	>>>continue &p 2
 *~
 *   COMMAND REFERENCES:
 *
 *	start, stop
 *
 * FORM OF CALL:
 *  
 *	continue_cmd( argc, *argv[] )
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
int continue_cmd (int argc, char *argv[]) {
        int id;
	int status;
	struct PCB *pcb;
	struct SLOT *slot;
	struct HWRPB *hwrpb_ptr;

	if (argc == 2) {
	    status = common_convert (argv[1], 10, &id, 4);
	    if (status) {
		err_printf(status);
		return status;
	    }
	} else {
	    id = whoami ();
	}

	if (id >= MAX_PROCESSOR_ID) {
	    err_printf("invalid processor ID\n");
	    return msg_failure;
	}

	if (!(cpu_mask & (1 << id))) {
	    printf("invalid processor ID\n");
	    return msg_success;
	}

	if (!(in_console & (1 << id))) {
	    printf("CPU %d is not halted\n", id);
	    return msg_success;
	}

	/*
	 * Complain if the context in the per cpu slot is not valid
	 */
	hwrpb_ptr = hwrpb;
#if TURBO
	slot = (int)hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET +
	  *hwrpb_ptr->SLOT_SIZE * (real_slot(id));
#else
	slot = (int)hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET +
	  *hwrpb_ptr->SLOT_SIZE * id;
#endif
	if (!slot->STATE.SLOT$V_CV) {
	    err_printf (msg_ctx_invalid);
	    return msg_ctx_invalid;
	}
	slot->STATE.SLOT$V_OH = 0;	    

	pprintf ("\ncontinuing CPU %d\n", id);
	if (id == primary_cpu) {
	    pcb = getpcb ();
	    pcb->pcb$l_affinity = (1 << id);
	    reschedule (1);
	    console_exit ();
	} else {
	    krn$_create (console_exit, null_procedure, 0, 6, 1<<id, 0,
		    argv[0], 0, 0, 0, 0, 0, 0);
	}
	return msg_success;
}


/*+
 * ============================================================================
 * = initialize - Initialize the specified drivers or driver phase.           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This command will initialize the system, or a specified device 
 *      driver or driver phase.  On some platforms, '>>>init' will
 *      cause a hardware system reset and reload the console image from 
 *      flash and restart it,.  However, on some platforms, an '>>>init' 
 *      will restart the current in-memory console image without a full
 *      hardware reset.
 *
 *   COMMAND FMT: init 1 BZ 0 initialize
 *  
 *   COMMAND FORM:
 *  
 *      init ( [-driver <device_or_phase>] )
 *  
 *   COMMAND TAG: init 0 RXBZ init
 *  
 *   COMMAND ARGUMENT(S):
 *
 *	None
 *
 *   COMMAND OPTION(S):
 *
 *	-driver <device_or_phase> - Initialize a driver or driver phase.
 *		The argument is either the driver phase number or
 *		a device name prefix, such as, 'ez'.  This option is
 *              a developer tool and seldom used in a customer context.
 *   
 *       NOTE: The -driver option is not supported on TurboLaser and
 *             Rawhide.
 *
 *
 *   COMMAND EXAMPLE(S):
 *
 *      Initialize the system
 *~
 *      >>>init 
 *~
 *	Initialize the ez drivers.
 *~
 *      >>>init -driver ez
 *~
 *	Initialize all the drivers in phase 5.
 *~
 *      >>>init -driver 5
 *~
 *   COMMAND REFERENCES:
 *
 *	start, stop
 *
 * FORM OF CALL:
 *
 *	init (argc,argv); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	int argc - Number of arguments specified.
 *	char *argv[] - List of arguments.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int init(int argc,char *argv[])
{
	int status;
	struct QSTRUCT qual[1];

/*Scan the command line*/
	status = qscan (&argc, argv, "-", "%sdriver", qual);
	if (status != msg_success) {
	    err_printf (status);
	    return status;
	}


	if (!qual[0].present) {
#if TURBO || RAWHIDE || MIKASA
            nvr_set_expected_init();
#endif

#if YUKONA
    	    sys_init = 1;
#endif

	    console_restart ();		/* Init the console */
	}

/*Handle the -driver qualifier*/
	if(qual[0].present)   
	/*Is it a driver phase or a single driver*/
	    if((qual[0].value.string[0] >= '0') &&
	       (qual[0].value.string[0] <= '9'))
	    {
	    /*Startup a driver phase*/
		ddb_startup(atoi(qual[0].value.string));
	    }
	    else
	    {
	    /*Startup a single driver*/
		ddb_startup_driver(qual[0].value.string);
	    }
}
