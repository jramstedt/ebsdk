/* file:	ps.c
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
 *	ps (process status) command
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      30-Jan-1992
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	30-jan-1992	Broken out from shell_commands.
 *--
 */
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"

/*+
 * ============================================================================
 * = ps - Print process status and statistics.                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      The ps command displays the system state in the form of
 *	process status and statistics.  
 *  
 *   COMMAND FMT: ps_command 0 NZ 0 ps
 *
 *   COMMAND FORM:
 *  
 *	ps ( )
 *  
 *   COMMAND TAG: ps_command 0 RXBZ ps
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
 *~
 *      >>>ps
 *       ID       PCB     Pri CPU Time Affinity CPU  Program    State
 *      -------- -------- --- -------- -------- --- ---------- ------------------------
 *      0000008f 0010e8a0 3          0 00000001 0           ps running
 *      00000020 00110160 1          0 ffffffff 0     puc_poll waiting on tqe
 *      0000001f 0013cb60 6          0 ffffffff 0  puc_receive waiting on puu_receive
 *      0000001c 0013ed00 1          0 ffffffff 0     pub_poll waiting on tqe
 *      0000001b 0014fc00 6          0 ffffffff 0  pub_receive waiting on puu_receive
 *      0000001a 00111a20 3          0 00000001 0           sh ready
 *      00000015 001176a0 2          0 ffffffff 0   mopcn_eza0 waiting on mop_eza0_cnw
 *      00000014 00119140 2          0 ffffffff 0   mopid_eza0 waiting on tqe
 *      00000013 0011ac20 2          0 ffffffff 0   mopdl_eza0 waiting on mop_eza0_dlw
 *      00000012 0011f6a0 6          0 ffffffff 0      tx_eza0 waiting on eza0_isr_tx
 *      00000011 00121140 6          0 ffffffff 0      rx_eza0 waiting on eza0_isr_rx
 *      00000010 00122ac0 1          0 ffffffff 0     pua_poll waiting on tqe
 *      0000000f 001244e0 6          0 ffffffff 0  pua_receive waiting on pua_receive
 *      00000009 00147460 5          0 ffffffff 0     lad_poll waiting on tqe
 *      00000008 00148f00 5          0 ffffffff 0     dup_poll waiting on tqe
 *      00000007 0014a9a0 5          0 ffffffff 0    mscp_poll waiting on tqe
 *      00000006 0014e1a0 5          0 00000001 0     entry_00 waiting on entry_00
 *      00000004 001516e0 2          0 ffffffff 0   dead_eater waiting on dead_pcb
 *      00000003 00153140 7   11759330 ffffffff 0        timer waiting on timer
 *      00000002 00158740 6          0 ffffffff 0   tt_control waiting on tt_control
 *      00000001 0005cfd8 0          0 00000001 0         idle ready
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *
 * 	show_status, sa, sp
 *
 * FORM OF CALL:
 *  
 *	ps_command ( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Success
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *      	   (should always be 1)
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int ps_command (int argc, char *argv[]) {
	krn$_walkpcb ();
	return (int) msg_success;
}

/*+
 * ============================================================================
 * = sa - Set process affinity.                                               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Set affinity, 'sa', is used to change the affinity mask of a process.
 *	The process may then execute on any processors specified by the mask. 
 *  
 *   COMMAND FMT: set_affinity 0 NZ 0 sa
 *
 *   COMMAND FORM:
 *  
 *   	sa ( <process_id> <affinity_mask> )
 *  
 *   COMMAND TAG: set_affinity 0 RXBZ sa
 *
 *   COMMAND ARGUMENT(S):
 *
 *	<process_id> - Specifies the PID of the process to be modified.
 *
 *	<affinity_mask> - Specifies the new affinity mask which indicates
 *		which processors the process may run on.   Bits 0 and 1
 *		of the mask correspond to processors 0 and 1, respectively.
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>memtest -p 0 &
 *	>>>ps | grep  memtest
 *	00000025 001a9700 2      23691 00000001 0      memtest ready
 *	>>>sa 25 2
 *	>>>ps | grep  memtest
 *	00000025 001a9700 2     125955 00000002 1      memtest running
 * 	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	ps, sp
 *
 * FORM OF CALL:
 *  
 *	set_affinity( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - success
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
int set_affinity (int argc, char *argv[]) {
	krn$_setaffinity(xtoi(argv[1]),xtoi(argv[2]));
        return (int) msg_success;
}

/*+
 * ============================================================================
 * = sp - Set process priority.                                               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Set priority, 'sp', is used to modify the priority of a process.
 *	Changing the priority of process will impact the behaviour of
 *	the process and the rest system.
 *  
 *   COMMAND FMT: set_priority 0 NZ 0 sp
 *
 *   COMMAND FORM:
 *  
 *   	sp ( <process_id> <new_priority> )
 *  
 *   COMMAND TAG: set_priority 0 RXBZ sp
 *
 *   COMMAND ARGUMENT(S):
 *
 *	<process_id> - Specifies the PID of the process to be modified.
 *
 *	<new_priority> - Specifies the new priority for the process.
 *		Priority values range from 0 lowest to 7 highest.
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>memtest -p 0 &
 *	>>>ps | grep  memtest
 *	00000025 001a9700 2      23691 00000001 0      memtest ready
 *	>>>sp 25 3
 *	>>>ps | grep  memtest
 *	00000025 001a9700 3     125955 00000001 0      memtest ready
 * 	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	ps, sa
 *
 * FORM OF CALL:
 *  
 *	set_priority( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - success
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
int set_priority (int argc, char *argv[]) {
 	krn$_setpriority(xtoi(argv[1]),xtoi(argv[2]));
        return (int) msg_success;
}
