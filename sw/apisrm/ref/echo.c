/* file:	echo.c
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
 *	echo command
 *
 *  AUTHORS:
 *
 *      numerous
 *
 *  CREATION DATE:
 *  
 *      30-Jan-1992
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	30-Jan-1992	Broken out from shell_commands
 *--
 */
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:msg_def.h"


/*+
 * ============================================================================
 * = echo - Echo the command line.                                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Echoes the text entered on the command line. The echo command 
 *	separates arguments in the line to be echoed by blanks.  The end 
 *	of the echoed line is signified by a new line on the standard output. 
 *
 *	Whenever the input is more than one line, be explicit by 
 *	enclosing the text within single quotes. 
 *  
 *   COMMAND FMT: echo 2 Z 0 echo
 *
 *   COMMAND FORM:
 *  
 *	echo ( [-n] <args>... )
 *  
 *   COMMAND TAG: echo 0 RXBZ echo
 *
 *   COMMAND ARGUMENT(S):
 *
 *	<args>... - Specifies any arbitrary set(s) of character strings.
 *
 *   COMMAND OPTION(S):
 *
 *	-n - Suppress newlines from output.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>echo this is a test.
 *      this is a test.
 *      >>>echo -n this is a test.
 *      this is a test.>>>
 *      >>>echo this is a test > foo
 *      >>>cat foo
 *      this is a test
 *      >>>echo > foo 'this is the simplest way
 *      _>to create a long file. All characters will be echoed
 *      _>file foo until the closing single quote.'
 *      >>>cat foo
 *      this is the simplest way
 *      to create a long file. All characters will be echoed
 *      file foo until the closing single quote.
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *
 *	cat
 *
 * FORM OF CALL:
 *  
 *	echo( argc, *argv[] )
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
int echo (int argc, char **argv) {
	int	status;
        int	argix;
	struct QSTRUCT qv;

/*
 * Check for a -n qualifier
 * let errors except for msg_failure (help_key) pass
 * (maybe putting qualifiers in a file)
 */
	status = qscan (&argc, argv, "-", "n", &qv);
	if (status == msg_failure) {
	    err_printf (status);
	    return status;
	}
	while (--argc > 0) {
	    printf ("%s%s", *++argv, (argc > 1) ? " " : "");
	}

	if (!qv.present) printf ("\n");
	return (int) msg_success;
}
