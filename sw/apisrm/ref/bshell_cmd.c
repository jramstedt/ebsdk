/* file:	bshell_cmd.c
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
 *      Bourne shell comamnds.
 *
 *  AUTHORS:
 *
 *      J. Kirchoff
 *
 *  CREATION DATE:
 *  
 *      29-Sep-1993
 *
 *--
 */


#include "cp$src:platform.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:regexp_def.h"
#include	"cp$src:ev_def.h"
#include	"cp$src:tt_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:parser.h"
#include	"cp$src:blex.h"

#define SHELL_DEBUG 0

#include "cp$inc:kernel_entry.h"

extern struct LOCK spl_kernel;

/*+
 * ============================================================================
 * = shell - Create a new shell process.                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	The shell command creates another shell process.  Each shell process
 *	implements most of the functionality of the Bourne shell.
 *  
 *   COMMAND FMT: sh_cmd 3 0 12 shell
 *
 *   COMMAND FORM:
 *  
 *	shell ( [-{v|x}] [<arg>...] )
 *  
 *   COMMAND TAG: sh_cmd 12*1024 RXBZ shell
 *
 *   COMMAND ARGUMENT(S):
 *
 *	<arg>... - Specifies any text string(s) terminated with whitespace
 *		to be passed to the shell process.
 *
 *   COMMAND OPTION(S):
 *
 *	-v    - Specifies to print lines as they are read from the input file.
 *	-x    - Specifies to show commands just before they are executed.
 *
 *	Internal only
 *	-d    - Specifies to delete stdin when shell is done.
 *
 *	Debug only
 *	-l    - Specifies to trace the lexical analyzer by showing tokens
 *		as they are recognized.
 *	-r    - Specifies to trace the parser by showing rules
 *		as they are executed.
 *	-p    - Specifies to trace the execution engine by showing routines
 *		as they are called.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>echo > foo 'help examine'
 *      >>>cat foo
 *      help examine
 *      >>>shell < foo
 *      examine
 *              Displays data at a specified address.
 *        syntax:
 *              examine ( [-{b,w,l,q,o,h,d}][-n val] [-s val] [device:]address )
 *      
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *
 *	exit
 *
 * FORM OF CALL:
 *  
 *	sh_cmd ( argc, *argv[] )
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

int sh_cmd (int argc, char *argv [])
{
    int status;

    status = sh_routine(argc, argv);

    return status;
}

/*+
 * ============================================================================
 * = kill - Stop and delete a process.                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Kill the processes listed on the command line.  Processes are killed
 *	by making a kernel call with the process id (PID) as the argument.
 *  
 *   COMMAND FMT: kill 0 N 0 kill
 *
 *   COMMAND FORM:
 *  
 *	kill ( [<pid>...] )
 *  
 *   COMMAND TAG: kill 0 RXBZ kill
 *
 *   COMMAND ARGUMENT(S):
 *
 *	<pid>... - Specifies the PID(s) as shown by the 'ps' command
 *		   of the process(es) to be killed.
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>memtest -p 0 &
 *      >>>ps | grep memtest
 *      000000f1 00217920 2       9357 ffffffff 0      memtest ready
 *      >>>kill f1
 *      >>>ps | grep memtest
 *~
 *   COMMAND REFERENCES:
 *
 *	ps
 *
 * FORM OF CALL:
 *  
 *	kill ( argc, *argv[] )
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments,
 *			the command arguments are process IDs.
 *
 * SIDE EFFECTS:
 *
 *      Specified processes are deleted.
 *
-*/
void kill (int argc, char *argv []) {
	int	argix;
	int	pid;

	/*
	 * Iterate over each argument.  Arguments are in hex.
	 */
	for (argix=1; argix<argc;argix++) {

	    /*
	     * We simply pass the PID to the kill command
	     * and let it decide if the PID is valid or not.
	     */
	    pid = xtoi (argv [argix]);
	    krn$_delete (pid);
	}
}

/*+
 * ============================================================================
 * = exit - Exit the current shell. 					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Exit the current shell with a status or return the status of the last
 *	command.
 *  
 *   COMMAND FMT: exit_cmd 0 N 0 exit
 *
 *   COMMAND FORM:
 *  
 *	exit ( <exit_value> )
 *  
 *   COMMAND TAG: exit_cmd 0 RXBZ exit
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<exit_value> - Specifies the status code to be returned by the shell.
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>exit			# exits with status of previous command
 *	>>>exit 0		# exits with success
 *	>>>test || exit 	# runs test and exits if an error
 *~
 *   COMMAND REFERENCES:
 *
 *	None
 *
 * FORM OF CALL:
 *  
 *	exit ( argc, argv )
 *  
 * RETURNS:
 *
 *	last status or value on command line
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int exit_cmd (int argc, char *argv[]) {
	struct PCB 	*parent, *pcb;
	struct PARSE	*p;
	int		status;

	status = msg_success;
	if (argc > 1) {
	    status = atoi (argv [1]);
	}

	/*
	 * Find the parent shell
	 */
	pcb = getpcb ();
	if (parent = krn$_findpcb (pcb->pcb$l_parentpid)) {
	    if (p = parent->pcb$l_shellp) {
		if (argc == 1) {
		    status = p->laststatus;
		}
		sh_request_exit (p);
	    }     
	    spinunlock (&spl_kernel);
	}

	return status;
}

/*+
 * ============================================================================
 * = break - Break out of a for, while, or until loop.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Exit the current shell with a status or return the status of the last
 *	command.
 *  
 *   COMMAND FMT: break_cmd 3 N 0 break
 *
 *   COMMAND FORM:
 *  
 *	break ( <break_level> )
 *  
 *   COMMAND TAG: break_cmd 0 RXBZ break
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<break_level> - Specifies the status code to be returned by the shell.
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>> for i in 1 2 3 4 5 ; do  echo $i ; break ; done
 *	1
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	None
 *
 * FORM OF CALL:
 *  
 *	break_cmd ( argc, argv )
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int break_cmd (int argc, char *argv[]) {
	struct PCB 	*parent, *pcb;
	struct PARSE	*p;
	int		level;

	level = 1;
	if (argc > 1) {
	    level = atoi (argv [1]);
	}

	/*
	 * Find the parent shell
	 */
	pcb = getpcb ();
	if (parent = krn$_findpcb (pcb->pcb$l_parentpid)) {
	    if (p = parent->pcb$l_shellp) {
		p->breakc = level;
	    }     
	    spinunlock (&spl_kernel);
	}

	return msg_success;
}
