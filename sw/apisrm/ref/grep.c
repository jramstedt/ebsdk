/* file:	grep.c
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
 *
 * Abstract:	Globally search for Regular Expressions and Print; our
 *		favorite command :-)
 *
 * Author:	AJ Beaverson
 *
 * Restrictions:
 *
 *	These routines are line oriented, and don't handle lines with
 *	embedded nulls very well.
 *
 * Modifications:
 *
 *
 *
 *	mar     02-Aug-1994     Fix -c switch to print only the number of matches
 *
 *	ajb	10-Jun-1993	Broken out from regexp.c
 *
 *	ajb	18-Dec-1991	Return correct values in grep
 *
 *	ajb	02-May-1991	make regexp_shell more robust
 *
 *	ajb	19-Apr-1991	When compiling shell regular expressions,
 *				provide no special treatment for metacharacters
 *				that are quoted.  Remove the expr field in the
 *				regular expression.  Remove character classes
 *				and make grep take expressions that are
 *				identical to U*x.
 *
 *	ajb	17-Apr-1991	Added mayberegular
 *
 *	ajb	15-jan-1991	Add qualifiers to grep command
 *
 *	pel	28-Nov-1990	access stdin/out/err differently.
 *
 *	cfc	07-Nov-1990	Make grep recognize stdin for pipes.
 *
 *	ajb	02-Nov-1990	Add simple grep
 *
 *	ajb	01-Nov-1990	Remove parameters to regexp_validate.
 *
 *	ajb	19-oct-1990	Not clearing enuogh memory in character class
 *				definition.
 *
 *	ajb	16-Oct-1990	Rename getfield/setfield to getbit/testbit
 *
 *	ajb	01-Oct-1990	Conditional compilation of validation code.
 *
 *	ajb	06-Sep-1990	Put setfield/getfield instructions in C.
 *
 *	kp 	24-Jun-1990	Autodoc fixes and msg defs.
 *
 *	ajb	08-Jun-1990	Add regexp_extract
 *
 *	ajb	04-June-1990	Create a routine that parses character classification
 *				constructs.  This is also used by scanf.
 *
 *	ajb	30-May-1990	Add compilation for shell regular expressions.
 *
 *	ajb	08-May-1990	Initial entry.
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:common.h"
#include	"cp$src:ctype.h"
#include	"cp$src:regexp_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:parse_def.h"
#include "cp$inc:prototypes.h"

/*+
 * ============================================================================
 * = grep - Globally search for regular expressions and print matches.        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Globally search for regular expressions and print any lines
 *      containing occurrences of the regular expression.
 *      Since grep is line oriented, it only works on ASCII files.
 *
 *      A regular expression is a shorthand way of specifying a 'wildcard'
 *      type of string comparison.  Regular expressions may need to be
 *	enclosed by quotes to prevent metacharacters from being interpreted
 *	by the shell.  Grep supports the following metacharacters:
 *~      
 *         ^   matches the beginning of line.
 *         $   matches end of line.
 *         .   matches any single character.
 *         [ ] set of characters; [ABC] matches either 'A' or 'B' or 'C'; a
 *               dash (other than first or last of the set) denotes a range
 *               of characters: [A-Z] matches any upper case letter; if the
 *               first character of the set is '^', then the sense of match
 *               is reversed: [^0-9] matches any non-digit; several
 *               characters need to be quoted with backslash (\) if they
 *               occur in a set:  '\', ']', '-', and '^'
 *         *   repeated matching; when placed after a pattern, indicates that
 *               the pattern should match any number of times.  For example,
 *               '[a-z][0-9]*' matches a lower case letter followed by zero or
 *               more digits.
 *         +   repeated matching; when placed after a pattern, indicates that
 *               the pattern should match one or more times '[0-9]+' matches
 *               any non-empty sequence of digits.
 *         ?   optional matching; indicates that the pattern can match zero or
 *               one times.  '[a-z][0-9]?' matches lower case letter alone or
 *               followed by a single digit.
 *         \   quote character; prevent the character which follows from
 *		having special meaning.
 *~
 *   COMMAND FMT: grep 0 Z 0 grep
 *  
 *   COMMAND FORM:
 *  
 *	grep ( [-{c|i|n|v}] [-f <file>] [<expression>] [<file>...] )
 *
 *   COMMAND TAG: grep 0 RXBZ grep
 *  
 *   COMMAND ARGUMENT(S):
 *
 *	<expression> - Specifies the target regular expression.
 *		If any regular expression metacharacters are present,
 *		the expression should be enclosed with quotes to avoid
 *		interpretation by the shell.
 *
 *	<file>... - Specifies the file(s) to be searched.
 *		If none are present, then stdin is searched.
 *
 *   COMMAND OPTION(S):
 *
 *	-c - Print only the number of lines matched.
 *	-i - Ignore case in the search.  By default grep is case sensitive.
 *	-n - Print the line numbers of the matching lines.
 *	-v - Print all lines that don't contain the expression.
 *	-f <file> - Specifies to take regular expressions from a file,
 *		instead of command.
 *
 *   COMMAND EXAMPLE(S):
 *
 *	In the following example, the output of the ps command (stdin)
 *	is searched for lines containing 'eza0'.
 *~
 *      >>>ps | grep eza0
 *      0000001f 0019e220 3          2 ffffffff 0   mopcn_eza0 waiting on mop_eza0_cnw
 *      00000019 0018e220 2          1 ffffffff 0   mopid_eza0 waiting on tqe
 *      00000018 0018f900 3          3 ffffffff 0   mopdl_eza0 waiting on mop_eza0_dlw
 *      00000015 0019c320 5          0 ffffffff 0      tx_eza0 waiting on eza0_isr_tx
 *      00000013 001a2ce0 5          2 ffffffff 0      rx_eza0 waiting on eza0_isr_rx
 *~
 *	In the next example, grep is used to search for all quadwords in a
 *	range of memory which are non-zero.
 *~
 *      >>>alloc 20
 *      00FFFFE0
 *      >>>deposit -q pmem:fffff0 0
 *      >>>e -n 3 ffffe0
 *      pmem:           FFFFE0 EFEFEFEFEFEFEFEF
 *      pmem:           FFFFE8 EFEFEFEFEFEFEFEF
 *      pmem:           FFFFF0 0000000000000000
 *      pmem:           FFFFF8 EFEFEFEFEFEFEFEF
 *      >>>e -n 3 ffffe0 | grep -v 0000000000000000
 *      pmem:           FFFFE0 EFEFEFEFEFEFEFEF
 *      pmem:           FFFFE8 EFEFEFEFEFEFEFEF
 *      pmem:           FFFFF8 EFEFEFEFEFEFEFEF
 *      >>>free ffffe0
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *  
 *	None  
 *  
 * FORM OF CALL:
 *  
 *	grep( argc, *argv[] )
 *  
 * RETURNS CODES:
 *
 *	0	- at least one match found
 *	1	- no matches found
 *	2	- all other errors
 *       
 * ARGUMENTS:
 *
 *      int argc - number of command line arguments passed by the shell
 *      char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
#define	QV	0
#define	QC	1
#define	QF	2
#define	QN	3
#define	QI	4
#define	QMAX	5
#define QSTRING "v c %sf n i"
#define	MAX_RE	128	/* max number of REs when obtained from a file */

int grep (int argc, char *argv []) {
	struct QSTRUCT	qual [QMAX];
	struct REGEXP	*re [MAX_RE];
	int		re_len;
        struct FILE	*fin;
	char		*line;
  	int		i;
	int		argix;
	struct PCB	*pcb;
	int		total_matches;


	/*
	 * parse command qualifiers
	 */
	if (qscan (&argc, argv, "-", QSTRING, qual)) {
	    return 2;
	}

	/*
	 * Regular expressions are obtained from a file
	 */
	if (qual [QF].present) {
	    line = malloc (512);
	    fin = fopen (qual [QF].value.string, "r");
	    if (fin == NULL) {
		return msg_nofile;
	    }
	    re_len = 0;
	    while ((re_len < MAX_RE)  &&  fgets (line, 512, fin) != NULL) {
		line [strlen (line) - 1] = 0; /* chop off newline */
		re [re_len] = regexp_compile (line, qual [QI].present);
		if (re [re_len] == 0) {
		    err_printf ("not compiled: %s\n", line);
		} else {
		    re_len++;
		}
	    }
	    fclose (fin);
	    argix = 1;
	    free (line);


	/*
	 * Regular expression is obtained from the command line
	 */
	} else {
	    if (argc >= 2) {
		re [0] = regexp_compile (argv [1], qual [QI].present);
		re_len = 1;
		if (re [0] == 0) {
		    return 2;
		}
		argix = 2;
	    } else {
		return 2;
	    }
	}


	/*
	 * Apply the regular expressions to stdin
	 */
	total_matches = 0;
	if (argix >= argc) {
	    pcb = getpcb();
	    total_matches += grep_helper (pcb->pcb$a_stdin_name, pcb->pcb$a_stdin, qual, re, re_len, qual [QC].present);

	/*
	 * Apply regular expressions to file list
	 */
	} else {
	    for (; argix<argc; argix++) {
		fin = fopen (argv [argix], "r");
		if (fin == NULL) {
		    continue;
		}
		total_matches += grep_helper (argv [argix], fin, qual, re, re_len, qual [QC].present);
		fclose (fin);
		if (killpending ()) break;
	    }
	}

	if (total_matches) {
	    return 0;
	} else {
	    return 1;
	}
}

/*----------*/
int grep_helper (
char	*name,
struct FILE *fin,
struct QSTRUCT *qual,
struct REGEXP *re [],
int re_len,
int	report_matches
) {
	int	i;
 	int	line_number;
	int	lines_matched;
	int	match;
	char	line [256];

 
	line_number = -1;
	lines_matched = 0;

	while (fgets (line, sizeof (line), fin) != NULL) {
	    i = strlen (line);
	    if (line[i-1] == '\n') {
		line[i-1] = '\0';
	    }
	    line_number++;
	    if (killpending ()) break;

	    /*
	     * apply each regular expression to this line
	     */
	    match = 0;
	    for (i=0; i<re_len; i++) {
		match |= regexp_subset (re [i], line);
	    }
	    match ^= qual [QV].present;
	    if (match && !report_matches) {
		if (qual [QN].present) {
		    printf ("%4d: %s\n", line_number, line);
		} else {
		    printf ("%s\n", line);
		}
	    }
	    lines_matched += match;
	}

	if (report_matches) {
	    printf ("%d\n", lines_matched);
	}

	return lines_matched;
}
