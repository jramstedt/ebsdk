/* file:	sleep.c
 *
 * Copyright (C) 1992 by
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
 *	Sleep command
 *
 *  AUTHORS:
 *
 *      Stephen Shirron
 *
 *  CREATION DATE:
 *  
 *      30-Jan-1992
 *
 *  MODIFICATION HISTORY:
 *             
 *	ajb	30-jan-1992	Broken out form shell_commands
 *--
 */
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:parse_def.h"    

/*+
 * ============================================================================
 * = sleep - Suspend execution for a time.                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      The sleep command suspends execution of the process
 *	for a specified number of seconds.  It temporarily wakes up
 *	every second to check for and kill pending bits.
 *  
 *   COMMAND FMT: sleep 3 0 0 sleep
 *
 *   COMMAND FORM:
 *  
 *	sleep ( [-v] <time_in_secs> )
 *  
 *   COMMAND TAG: sleep 0 RXBZ sleep
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<time_in_secs> - Specifies the number of seconds to sleep.
 *		The default is 1 second.
 *
 *   COMMAND OPTION(S):
 *             
 * 	-v - Specifies that the value supplied is in milliseconds.
 *		By default this is 1000 (1 second).
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>(sleep 10; echo hi there)&
 *	>>>    
 *	(10 seconds expire...)
 *	hi there
 *	>>> sleep -v 20  # sleep for 20 milliseconds
 *~
 *   COMMAND REFERENCES:
 *
 *	None
 *
 * FORM OF CALL:
 *  
 *	sleep( argc, *argv[] )
 *                               
 * RETURN CODES:
 *
 *	msg_success - Success
 *       
 * ARGUMENTS:
 *             
 *	int argc - number of command line arguments passed by the shell
 *      	   (should always be 2)
 *	char *argv [] - array of pointers to arguments
 *             (where argv [1] is the number of seconds to sleep)
 *
 * SIDE EFFECTS:
 *             
 *      None
 *
-*/
#define	QV	0
#define	QMAX	1
int sleep (int argc, char *argv[]) {
	struct QSTRUCT qual [QMAX];
	int	value;

	value = 1000;
	qscan (&argc, argv, "-", "%dv", qual);
	if (qual [QV].present) {
	    value = qual [QV].value.integer;
	}


	if (argc == 2) {
	    int i;

	    i = atoi (argv [1]);
	    while( --i >= 0 ) {
		if (killpending ()) {
		    return (int) msg_success;
		}
		krn$_sleep (value);
	    }
	}
	return (int) msg_success;
}
