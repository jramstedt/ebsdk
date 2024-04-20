/* file:	sort.c
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
 *      Cobra Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      A crude sort subroutine which should be enhanced and made into a shell
 *      command.  See the routine header documentation of sort for more info.
 *
 *  AUTHORS:
 *
 *      Paul LaRochelle
 *
 *  CREATION DATE:
 *  
 *      05-Oct-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	14-Feb-1991	Make it a proper subset of U*x sort command
 *
 *      pel	17-Oct-1990	return err if fopen's fail.
 *--
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:version.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:stddef.h"
#include	"cp$src:prdef.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"

/*
 * Maximum amount of space SORT will take from the heap before complaining.
 */
#define	MAX_SPACE	(1024 * 64)
#define MAX_LINES	(1024 * 4)

extern strcmp ();

/*+
 * ============================================================================
 * = sort - Sort file(s) and write the sorted data into another file.         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Sort file(s) in lexicographic order and write the results onto
 *	stdout.  Sort has limitations on the size of files that it can
 *	sort, due to the finite amount of memory.
 *      <note> Sort is NOT available in alpha1000 and alpha800. <endnote>
 *
 *   COMMAND FMT: sort 3 NZ 0 sort
 *
 *   COMMAND FORM:
 *  
 *	sort ( <file...> )
 *  
 *   COMMAND TAG: sort 0 RXBZ sort
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<file...> - Specifies the (unsorted) file(s) to be sorted.
 *
 *   COMMAND OPTION(S):
 *
 * 	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>cat foo
 *      banana is yellow
 *      pear is green
 *      apple is red
 *      orange is orange
 *      >>>sort foo
 *      apple is red
 *      banana is yellow
 *      orange is orange
 *      pear is green
 *      >>>cat foo2
 *      a - excellent
 *      b - good
 *      c - satisfaction
 *      >>>sort foo foo2
 *      a - excellent
 *      apple is red
 *      b - good
 *      banana is yellow
 *      c - satisfaction
 *      orange is orange
 *      pear is green
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *  
 *      None
 *
 * FORM OF CALL:
 *  
 *	sort ( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Success
 *	msg_nofile   - if a fopen fails
 *	msg_sort_scratch - if exceeds maximum lines
 *       
 * ARGUMENTS:
 *
 *	argc - argument count
 *	char *argv[] - argument array
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int sort (int argc, char *argv []) {
	char	**lines;	/* array of pointers to lines	*/
	int	lc;		/* number of lines		*/
	int	argix;
	struct FILE *fin;
	int	i;
	int	status;
	struct PCB	*pcb;

	/*
	 * Read in the input files
	 */
	lines = malloc (sizeof (char *) * MAX_LINES);
	lc = 0;
	for (argix=1; argix<argc; argix++) {
	    fin = fopen (argv [argix], "r");
	    if (fin == NULL) {
		return msg_nofile;
	    }
	    status = sort_helper (lines, MAX_LINES, &lc, fin);
	    if (status != msg_success) {
		return status;
	    }
	    fclose (fin);
	}
	if (argc == 1) {
	    pcb = getpcb();
	    fin = pcb->pcb$a_stdin;
	    status = sort_helper (lines, MAX_LINES, &lc, fin);
	    if (status != msg_success) {
		return status;
	    }
	}

	/*
	 * Sort the lines
	 */
	shellsort (lines, lc, strcmp);

	/*
	 * Write the lines out
	 */
	for (i=0; i<lc; i++) {
	    printf ("%s\n", lines [i]);
	}

	return msg_success;
}

/*----------*/
int sort_helper (char **lines, int maxlines, int *lc, struct FILE *fin) {
	char	buf [256];
	int	x;

	while (fgets (buf, sizeof (buf), fin)) {
	    if (killpending ()) break;
	    x = strlen (buf);
	    buf [x-1] = 0;	/* trim trailing newline */
	    lines [*lc] = malloc (x);
	    strcpy (lines [*lc], buf);
	    (*lc)++;
	    if (*lc >= maxlines) {
		err_printf (msg_sort_scratch);
		return msg_sort_scratch;
	    }
	}

	return msg_success;
}
