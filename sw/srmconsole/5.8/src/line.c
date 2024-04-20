/* file:	line.c
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
 *	Unix style line command.
 *
 *  AUTHORS:
 *
 *      Paul LaRochelle
 *
 *  CREATION DATE:
 *  
 *      06-Mar-1992
 *
 *  MODIFICATION HISTORY:
 *
 *      cbf     27-Jan-1993     add arg to read_with_prompt for echo functn.
 *
 *	hcb	08-May-1992	use read_with_prompt for tty only
 *
 *	pel	08-May-1992	echo line output to stdin & use fputs to filter
 *				out backspaces and replaced characters.
 *
 *	pel	06-Mar-1992	Initial entry
 *--
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"

extern struct LOCK _align (QUADWORD) spl_kernel;

/*+
 * ============================================================================
 * = line - Read a line from standard input and write it to standard output.  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Copy one line (up to a new-line) from the standard input channel of
 *	the current process to the standard output channel of the current
 *	process.  Always outputs at least a new-line.
 *
 *	It is often used in scripts to read from the user's terminal, or to
 * 	read lines from a pipeline while in a for/while/until loop.
 *  
 *   COMMAND FMT: line 0 N 0 line
 *
 *   COMMAND FORM:
 *  
 *	line ( )
 *         
 *   COMMAND TAG: line 0 RXBZ line
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
 *	Use interactively as follows:
 *~
 *      >>>line
 *	type a line of input followed by carriage return
 *	type a line of input followed by carriage return
 *      >>>line >nl
 *	type a line of input followed by carriage return
 *	>>>
 *~
 *	Use w/in a script as follows:
 *~
 *      >>>echo -n 'continue [Y, (N)]? '
 *      >>>line <tt >tee:foo/nl
 *	>>>if grep <foo '[yY]' >nl; then echo yes; else echo no; fi
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	None
 *
 * FORM OF CALL:
 *  
 *	line ( )
 *  
 * RETURN CODES:
 *         
 *	msg_success - success
 *
 *	1 - error or EOF
 *
 *	msg_extra_params - error, one or more arguments were given
 *       
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int line (int argc, char *argv[]) {
	struct PCB	*pcb;
	char		buf[256];
	int		status;
           
    if (argc > 1) {
	err_printf( msg_extra_params);
	return msg_extra_params;
    }
    else {
	pcb = getpcb();
        if (isatty (pcb->pcb$a_stdin))           
	    status = read_with_prompt("", sizeof(buf), buf, pcb->pcb$a_stdin,
							    pcb->pcb$a_stdin,
                                                            1 );
	else
            status = fgets(buf, sizeof (buf), pcb->pcb$a_stdin); 
	if ( (status == EOF) || (status == 0) ) {
	     return msg_failure;
	} else {
	    fputs( buf, pcb->pcb$a_stdout);
	    return msg_success;
	}
    }
}
