/* file:	rm.c
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
 *	rm - remove filename
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
 *	ajb	30-jan-1992	Broken out from shell_commands
 *--
 */
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"

/*+
 * ============================================================================
 * = rm - Remove files from the file system.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Remove the specified file or files from the file system.
 *	Any allocated memory is returned to the heap.
 *  
 *   COMMAND FMT: rm 0 NZ 0 rm
 *
 *   COMMAND FORM:
 *  
 *	rm ( <file>... )
 *  
 *   COMMAND TAG: rm 0 RXBZ rm
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<file>... - Specifies the file or files to be deleted.
 *
 *   COMMAND OPTION(S):
 *
 * 	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>ls foo
 *      foo
 *      >>>rm foo
 *      >>>ls foo
 *      foo no such file
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *
 *	cat, ls
 *
 * FORM OF CALL:
 *  
 *	rm( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Success, all files specified removed.
 *	msg_fatal - file(s) not removed
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *	Files are removed and memory is returned to the heap.
 *
-*/
int rm (int argc, char *argv []) {
	int	argix;
	int	not_removed, status;
                           
	not_removed = 0;
	status = msg_success;
	for (argix=1; argix<argc; argix++) {
	    if (killpending ()) {
		return msg_success;
	    }
	    status = remove (argv [argix]);
	    if ( status != msg_success) {
		not_removed++;
		err_printf (msg_not_removed, argv [argix]);
	    }
	}
	return not_removed ? msg_fatal : msg_success;
}
