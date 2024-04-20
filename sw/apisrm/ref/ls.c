/* file:	ls.c
 *
 * Copyright (C) 1993, 1994 by
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
 *      ConsoleFirmware
 *                                            
 *  MODULE DESCRIPTION:
 *
 *	ls command.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 * 
 *      25-Jan-1993
 *
 *  MODIFICATION HISTORY:
 *
 *	jrk	10-Aug-1994	used column_print
 *--
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:parse_def.h"

/*+
 * ============================================================================
 * = ls - List files or inodes in file system.                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	List files or inodes in the system.  
 *	Inodes are RAM disk files, open channels and some drivers.
 *	RAM disk files include script files, diagnostics and executable
 *	shell commands.
 *
 *	If no strings are present on the command line, then list all files
 *	or inodes in the system.
 *
 *   COMMAND FMT: ls 0 0 0 ls
 *
 *   COMMAND FORM:
 *  
 *	ls ( [-l] [<file>...] )
 *
 *   COMMAND TAG: ls 0 RXB ls
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<file>... - Specifies the file(s) or inode(s) to be listed.
 *
 *   COMMAND OPTION(S):
 *
 * 	-l - Specifies to list in long format.  Each file or inode is listed
 *		on a line with additional information.
 *		By default just file names are listed.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>ls examine
 *      examine      
 *      >>>ls -l examine
 *      r-xb  rd                   0/0                f7cc8     0    examine
 *      >>>ls d*
 *      d            date         debug1       debug2       decode       deposit
 *      dg_pidlist   dka0.0.0.0.0              dke100.1.0.4.0            
 *      dub0.0.0.1.0              dynamic      
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *
 * 	None
 *
 * FORM OF CALL:
 *  
 *	ls( argc, argv);
 *  
 * RETURN CODES:
 *
 *  msg_success - normal completion
 *  msg_failure - if the inode does not exist
 *    
 * ARGUMENTS:
 *
 *  int argc        - number of arguments; must be 0 or 1
 *  char *argv []   - array of pointers to arguments
 *		      argv[0] if present = "1" long format, gives everything.
 *                    Otherwise, just filenames are shown.
 *
 * SIDE EFFECTS:
 *
 * 	None
 *
-*/

#define	QL	0	/* qualifier -l */
#define QMAX	1

int ls (int argc, char **argv) {
    int column;
    int status = msg_success;
    int	ls_cb ();
    struct QSTRUCT qual [QMAX];
    
    status = qscan (&argc, argv,"-", "l", qual);
    if (status != msg_success) {
	err_printf (status);
	return status;
    }
    column = 0;
    
/* No arguments, match everything */

    if (argc == 1) {
	argc = 2;
	argv [1] = "*";
    }
    
/* List files requested */

    argv++;
    while (*argv) {
	if (killpending ()) {
	    break;
	}

	status = fexpand (*argv, qual [QL].present, ls_cb, qual [QL].present, &column);
	if (status != msg_success) {
	    if (column) {
		printf ("\n");
		column = 0;
	    }
	    if (status != msg_ctrlc) {
		printf ("%s %m", *argv, status);
	    }
	}
	argv++;
    }
    
/* Clean up dangling CR */

    if (column) {
	printf ("\n");
    }
    return status;
}

int ls_cb (char *fname, int longformat, unsigned int *column) {
    unsigned int len;
    unsigned int padding;
    
/* in long format, we always print one per line */

    if (longformat) {
	printf ("%s\n", fname);
	
    } else {

/* otherwise we try to align stuff up */

	column_print(fname, column);
    }
    return msg_success;
}
