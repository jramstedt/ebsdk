/* file:	uniq.c
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
 *      UNIQ, a proper subset of the U*x uniq program.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *	ajb	06-Feb-1991
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	12-Feb-1991	Remove field qualifer (use tr command to
 *				eliminate whitespace)
 *
 *
 *--
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:version.h"
#include	"cp$src:ctype.h"
#include	"cp$src:stddef.h"
#include	"cp$src:prdef.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"


#define	QC	0	/* indecies into qualifier structures */
#define	QMAX	1

/*+
 * ============================================================================
 * = uniq - Remove duplicate lines in a file.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Uniq culls out duplicate lines in a file.  U*x versions of uniq also
 *	have a qualifier that matches on fields.  Similar funcationality can
 *	be obtained by passing the input stream through a filter that removes
 *	whitespace (see the command tr).	
 *  
 *   COMMAND FMT: uniq 4 Z 0 uniq
 *
 *   COMMAND FORM:
 *  
 *	uniq  ( [-<n>] [<file>...] )
 *  
 *   COMMAND TAG: uniq 0 RXBZ uniq
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<file>... - Specifies the file(s) to searched for duplicate lines.
 *		    If no file is specifies, input is taken from stdin.
 *
 *   COMMAND OPTION(S):
 *
 *	-<n>	- Specifies to skip n characters.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>hd dua0 | uniq -1 | more
 *~
 *   COMMAND REFERENCES:
 *
 *	tr
 *
 * FORM OF CALL:
 *  
 *	uniq (int argc, char *argv [])
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int uniq (int argc, char *argv []) {
	struct FILE	*fin;
	int		status,argix;
	struct PCB 	*pcb;
	struct QSTRUCT	qual [QMAX];
	int	skip_characters;

	/*
	 * Decode the flags (argc,argv are adjusted and flags culled out).
	 */
	status = qscan (&argc, argv,"-", "%dc", qual);
	if (status != msg_success) {
	    printf (status);
	    return status;
	}
	skip_characters = 0;
	if (qual [QC].present) {
	    skip_characters = qual [QC].value.integer;
	}

	/*
	 * Read from standard input if no files were specified on the
	 * command line.
	 */	
	if (argc == 1) {
	    pcb = getpcb();
	    uniq_helper (pcb->pcb$a_stdin, skip_characters);

	/*
	 * Visit each file specification on the command line
	 */
	} else {
	    for (argix=1; argix<argc; argix++) {
		if (killpending ()) break;
		fin = (struct FILE *) fopen (argv [argix], "r");
		if (fin == NUL) {
		    printf (msg_nofile);
		    continue;
		}
		uniq_helper (fin, skip_characters);
		fclose (fin); /* release as soon as possible */
	    }
	}

	return msg_success;
}

/*----------*/
void uniq_helper (struct FILE *fp, int skip_characters) {
	struct BUF {
	    char 	buf [512];
	} b0, b1, *previous, *current, *hold;
	int	result;

	previous = &b0;
	current = &b1;

	memset (&b0, 0, sizeof (struct BUF));
	memset (&b1, 0, sizeof (struct BUF));

	while (fgets (current->buf, sizeof (current->buf), fp)) {
	    if (killpending ()) break;

	    result = strcmp (
		current->buf + skip_characters,
		previous->buf + skip_characters
	    );

	    /*
	     * lines identical, don't print
	     */
	    if (result == 0) {

	    /*
	     * lines are different, print and swap
	     */
	    } else {
		if (previous->buf [0]) {
		    printf ("%s", previous->buf);
		}
		hold = previous;
		previous = current;
		current = hold;
	    }

	}

	if (previous->buf [0]) {
	    printf ("%s", previous->buf);
	}
}
