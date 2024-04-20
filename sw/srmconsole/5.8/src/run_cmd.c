/* file:	run_cmd.c
 *
 * Copyright (C) 1993 by
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
 *
 *++
 *  FACILITY:
 *
 *      Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      Run a utility etc..
 *
 *  AUTHORS:
 *
 *      K. LeMieux
 *
 *  CREATION DATE:
 *
 *  MODIFICATION HISTORY:
 *
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:parse_def.h"
#include "cp$src:msg_def.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

extern struct ZONE *memzone;

/*+
 *
============================================================================
 * = run - run something
=
 *
============================================================================
 *
 * OVERVIEW:
 *
 *	Run - run something
 * 
 *   COMMAND FMT: run 3 H 3 run
 *
 *   COMMAND FORM:
 *
 *   	run bios [controller name]
 *  
 *   COMMAND TAG: run 8*1024 RXB run
 *
 *   COMMAND ARGUMENT(S):
 *
 *	bios    - Run BIOS extension ROMs
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>run bios pya0
 *   
 * FORM OF CALL:
 *  
 *	run( argc, *argv[] )
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
int run(int argc, char *argv[]) {

    int status = msg_success;
 
    if ( argc > 1 ) {
	if (( substr_nocase( argv[1], "bios" ) > 0 )) {
	    if ( argc < 3 ) {
	     err_printf( msg_insuff_params );
	     status = msg_failure;	    
	    }
	    if ( argc > 3 ) {
		err_printf( msg_extra_params );
		status = msg_failure;	    
	    }
	    run_bios( argv[2] );
	}
    }
    return ( status );
}
