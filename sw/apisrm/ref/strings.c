/* file:	strings.c
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
 *      Search for contiguous ascii strings in a byte stream.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *	ajb	27-Sep-1991
 *
 *  MODIFICATION HISTORY:
 *
 *--
 */
#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:ctype.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:parse_def.h"
#include "cp$src:msg_def.h"

extern struct PCB *getpcb ();
#define	stdin  (getpcb()->pcb$a_stdin)
#define stdout (getpcb()->pcb$a_stdout)
#define stderr (getpcb()->pcb$a_stderr)


#define QNUMBER 0
#define QOFFSET 1
#define QMAX 2
#define QSTRING "%d offset"

/*+
 * ============================================================================
 * = strings - Search for printable ASCII strings in a byte stream.           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Strings searchs files, or stdin if no files are present,
 *	for printable ASCII strings.
 *  
 *   COMMAND FMT: strings 3 Z 0 strings
 *
 *   COMMAND FORM:
 *  
 *	strings ( [-<n>] [-offset] [<file>...] )
 *  
 *   COMMAND TAG: strings 0 RXBZ strings
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<file>... - Specifies the name of file(s) to search.
 *
 *   COMMAND OPTION(S):
 *
 *	-<n>	- Specifies the minimum length <n> of the strings.
 *	-offset	- print offset in hex
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>strings pmem
 *	>>>strings -8 dka200
 *~
 *   COMMAND REFERENCES:
 *
 *	None
 *
 * FORM OF CALL:
 *  
 *	strings ( int argc, char *argv [] )
 *  
 * RETURN CODES:
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
strings (int argc, char **argv) {
	int	argix;
	int	consec;		/* number of consecutive ascii chars */
        char	*buf;
	int	print_offset;
	struct FILE	*fp;
	struct QSTRUCT qual [QMAX];

	/*
	 * Determine the minimum string length
	 */
	consec = 4;
	qscan (&argc, argv, "-", QSTRING, qual);
	if (qual [QNUMBER].present) {
	    consec = qual [QNUMBER].value.integer;
	}

	print_offset = qual [QOFFSET].present;
	buf = (void *) malloc (consec);

	/*
	 * Visit each file, or use stdin
	 */
	if (argc == 1) {
	    strings_helper (stdin, buf, consec, print_offset);
	} else {
	    for (argix=1; argix < argc; argix++) {
		fp = fopen (argv [argix], "r");
		if (fp) {
		    strings_helper (fp, buf, consec, print_offset);
		}
		fclose (fp);
	    }
	}
	    
	free (buf);
	return msg_success;
}


/*
 * print out strings that are at least CONSEC consecutive printing characters
 * that are terminated by a newline or a null.
 */
strings_helper (struct FILE *fp, char *buf, int consec, int print_offset) {
	char	*bp, *s;
	int	c;
	int	seqn;	/* number of characters in current sequence */
	int	n;

	bp = buf;
	seqn = 0;
	n = 0;

	while ((c = fgetc (fp)) != EOF) {
	    c &= 0x7f;	/* ignore 8 bit code */

	    /* if not printable, we're in "hunt" mode */
	    if (!isprint (c)) {
		if (seqn >= consec) putchar ('\n');
		bp = buf;
		seqn = 0;

	    /* character is printable. We remember it because we don't
	     * have enough, or we already have enough and keep printing
	     */
	    } else {
		seqn++;
		if (seqn < consec) {
		    *bp++ = c;
		} else {
		    if (print_offset && (bp != buf)) {
			printf ("%08X ", n-consec + 1);
		    }
   		    for (s=buf; s<bp; s++) putchar (*s);
		    bp = buf;
		    putchar (c);
		}
	    }

	    n++;
	    if ((n & 0xfff) == 0) {
		if (killpending()) break;
	    }
	}
}
