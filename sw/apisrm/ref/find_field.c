/* file:	find_field.c
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
 *	find_field command
 *
 *  AUTHORS:
 *
 *      Paul Larochelle
 *
 *  CREATION DATE:
 *  
 *      30-jan-1992
 *
 *  MODIFICATION HISTORY:
 *
 *	cto	28-Jun-1994	Add qualifiable delimiter
 *
 *	ajb	19-Feb-1992	Pick off multiple fields and print
 *				in the order they were specified.
 *
 *	ajb	30-jan-1992	Broken out from shell_commands.
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"


/*+
 * ============================================================================
 * = find_field - Extract a field from each input line and write it.          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This command may be replaced by a non-standard grep option sometime
 *	in the future.  Look for the new grep option if this command vaporizes.
 *	
 *	Extract one specified field from each string in stdin.  Output these
 *	fields as lines (with linefeeds) to stdout.
 *
 *	Maximum string length supported is 133 characters.
 *	Strings larger than the max supported size will be broken up into 
 *	multiple strings before field extraction.
 *
 *   COMMAND FMT: find_field 4 0 0 find_field
 *
 *   COMMAND FORM:
 *  
 *	find_field ( <field_number> [-d <delimiter>])
 *
 *   COMMAND TAG: find_field 0 RXBZ find_field
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<field_number> - Specifies a number from 0 to the number of fields on a
 *		line minus 1.  Larger numbers will not produce a field match.
 *
 *   COMMAND OPTION(S):
 *
 * 	-d <delimiter> - Specifies delimiter string
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>cat foo
 *      this is a sample file                                                  
 *      to show how find_field works.
 *      It should pick off the word in 
 *      specified field on each line.
 *      >>>find_field 0 < foo
 *      this 
 *      to 
 *      It 
 *      specified 
 *      >>>find_field 1 3 < foo
 *      is sample 
 *      show find_field 
 *      should off 
 *      field each 
 *      >>>
 *      >>>  show device | grep dua | find 0 -d "."
 *      dua1
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *
 *	grep
 *
 * FORM OF CALL:
 *  
 *	find_field( argc, argv);
 *  
 * RETURNS:
 *
 *  msg_success
 *       
 *  msg_insuff_params if less than one command line argument
 *       
 * ARGUMENTS:
 *
 *  int argc        - number of arguments; must be 0 or 1
 *  char *argv []   - array of pointers to arguments
-*/

#define	QD	0
#define	ELEMENT_QMAX	1

int find_field (int argc, char *argv[]) {

#define MAX_LINE 256
    struct QSTRUCT	qual [ELEMENT_QMAX];
    struct PCB		*pcb;
    unsigned char	buf   [MAX_LINE + 2];
    unsigned char	newbuf [MAX_LINE + 2], *np;
    char  		*delimiter;
    int			i;
    int			status;
    /*
     * parse command qualifiers
     */
    status = qscan (&argc, argv, "-", "%sd", qual);
    if (status != msg_success) {
	err_printf (status);
	return status;
    }

    if (argc < 2) {
	err_printf( msg_insuff_params);
	return msg_insuff_params;
    }

    pcb = getpcb();

    for (i=1; i<argc; i++) {
	argv [i] = atoi (argv [i]);
    }

    /*
     * Get delimiter
     */
    if (qual [QD].present)
	delimiter = qual [QD].value.string;
    else
	delimiter = " \t\n";

    while (fgets( buf, MAX_LINE + 1, pcb->pcb$a_stdin) != NULL) {
	np = newbuf;
	*np = 0;
	for (i=1; i<argc; i++) {
	    if (strelement( buf, argv [i], delimiter, np)) {
		np += strlen (np);
		*np++ = ' ';
		*np = 0;
	    }
	}
	printf ("%s\n", newbuf);
    }

    return msg_success;
}
