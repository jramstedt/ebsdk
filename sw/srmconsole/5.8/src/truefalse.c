/* file:	truefalse.c
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
 *	true and false commands
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
 * = true - Return success status, always.                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Always return success status. 
 *	This shell command is useful for creating infinite loops in scripts.
 *  
 *   COMMAND FMT: true_cmd 4 NZ 0 true
 *
 *   COMMAND FORM:
 *  
 *	true ( )
 *  
 *   COMMAND TAG: true_cmd 0 RXBZ true
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
 *~
 *      >>>while true ; do echo foo; done
 *	foo
 *	foo
 *	.
 *	.
 *	.
 *~
 * FORM OF CALL:
 *  
 *	true_cmd ( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - for true command
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
int true_cmd (int argc, char *argv []) {
	return msg_success;
}

/*+
 * ============================================================================
 * = false - Return failure status, always.                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Always return failure status. 
 *  
 *   COMMAND FMT: false_cmd 2 NZ 0 false
 *
 *   COMMAND FORM:
 *  
 *	false ( )
 *  
 *   COMMAND TAG: false_cmd 0 RXBZ false
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
 *~
 *      >>>while false ; do echo foo; done
 *	>>>
 *~
 * FORM OF CALL:
 *  
 *	false_cmd ( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_failure - for false command
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
int false_cmd (int argc, char *argv []) {
	return msg_failure;
}
