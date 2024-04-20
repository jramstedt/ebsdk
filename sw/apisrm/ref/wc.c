/* file:	wc.c
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
 *      WC, a program that counts bytes, words and lines in an input
 *	stream.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *	ajb	07-May-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	29-Jan-1991	print out total only when more than one
 *				file is scanned.
 *
 *	ajb	18-Dec-1990	Read the input stream in big blocks for
 *				performance.
 *
 *	cfc	04-Dec-1990	-f qualifier which takes a string value
 *				for a file name (just to test out qualifier
 *				values, not really useful) wc -f test
 *				and -n which takes a count to
 *				overide argc on wild cards (wc -n 2 test*);
 *
 *	pel	28-Nov-1990	access stdin differently.
 *
 *	cfc	06-Sep-1990	Add wild card exp via callback rd_scan
 *				(located in rd_driver).  Used by qscan.
 *
 *	cfc	06-Sep-1990	Qscan calling sequence changed,added callbk
 *
 *	ajb	18-Jul-1990	Qscan calling sequence changed
 *
 *	ajb	28-Jun-1990	Read from stdin when no files present
 *
 *	ajb	07-May-1990	miscellaneous work.
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:parse_def.h"
#include "cp$src:version.h"
#include "cp$src:ctype.h"
#include "cp$src:stddef.h"
#include "cp$src:prdef.h"
#include "cp$src:msg_def.h"

struct STAT {
    int lines;
    int words;
    int bytes;
};

#include "cp$inc:prototypes.h"

#define QL	0			/* indecies into qualifier structures
					 */
#define QW	1
#define QC	2
#define QMAX	3

/*+
 * ============================================================================
 * = wc - Count bytes, words and lines and report totals.                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	WC counts bytes, words and lines in files and reports the totals on
 *	the standard output.
 *  
 *   COMMAND FMT:	wc 0 Z 0 wc
 *
 *   COMMAND FORM:
 *  
 *	wc ( [-{l|w|c}] [<file>...] )
 *  
 *   COMMAND TAG:	wc 0 RXBZ wc
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<file>... - Specifies the file(s) on which to perform the counts.
 *
 *   COMMAND OPTION(S):
 *
 *	-l - Specifies to count lines and display the number of lines.
 *	-w - Specifies to count words and display the number of words.
 *	-c - Specifies to count bytes and display the number of characters.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>cat foo
 *      Good luck to you all!
 *      >>>wc foo
 *              1        5       22 foo
 *      >>>wc -l foo
 *              1 foo
 *      >>>wc -w foo
 *              5 foo
 *      >>>wc -c foo
 *              22 foo
 *      >>>wc foo foo
 *              1        5       22 foo
 *              1        5       22 foo
 *              2       10       44 total
 *~
 *   COMMAND REFERENCES:
 *
 *	cat, ls
 *
 * FORM OF CALL:
 *  
 *	wc (int argc, char *argv [])
 *  
 * RETURN CODES:
 *
 *	msg_success - normal completion
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
int wc(int argc, char *argv[])
{
    struct FILE *fin;
    int status, argix;
    struct STAT local;
    struct STAT total;
    struct PCB *pcb;
    struct QSTRUCT qual[QMAX];
    int files;

    memset(&total, 0, sizeof(struct STAT));

    /* Decode the flags (argc,argv are adjusted and flags culled out). */

    files = 0;
    status = qscan(&argc, argv, "-", "l w c", qual);
    if (status != msg_success) {
	printf(status);
	return status;
    }
    if ((qual[QL].present + qual[QW].present + qual[QC].present) == 0) {
	qual[QL].present = 1;
	qual[QW].present = 1;
	qual[QC].present = 1;
    }

    /* Read from standard input if no files were specified on the command line.
     */

    if (argc == 1) {
	pcb = getpcb();
	wc_count(pcb->pcb$a_stdin, &local, &total);
	files++;
	wc_render(&local, qual[QL].present, qual[QW].present, qual[QC].present,
	  "");

	/*
	 * Visit each file specification on the command line
	 */
    } else {
	for (argix = 1; argix < argc; argix++) {
	    if (killpending())
		break;
	    fin = (struct FILE *) fopen(argv[argix], "r");
	    if (fin == NUL) {
		printf("wc: can't open %s\n", argv[argix]);
		continue;
	    }
	    wc_count(fin, &local, &total);
	    files++;
	    fclose(fin);		/* release as soon as possible */
	    wc_render(&local, qual[QL].present, qual[QW].present,
	      qual[QC].present, argv[argix]);
	}
    }

    /* Output grand total */

    if (files > 1) {
	wc_render(&total, qual[QL].present, qual[QW].present, qual[QC].present,
	  "total");
    }

    return msg_success;
}

/*----------*/
void wc_render(struct STAT *s, int fl, int fw, int fc, char *name)
{

    if (fl)
	printf("%8d ", s->lines);
    if (fw)
	printf("%8d ", s->words);
    if (fc)
	printf("%8d ", s->bytes);
    printf("%s\n", name);
}

/*----------*/
void wc_count(struct FILE *fp, struct STAT *l, struct STAT *t)
{
    int ch, inword;
    char buf[512], *s;
    int len;

    l->lines = 0;
    l->words = 0;
    l->bytes = 0;
    inword = 0;

    while (len = fread(buf, 1, sizeof(buf), fp)) {
	if (killpending())
	    break;
	s = buf;
	l->bytes += len;
	while (len--) {
	    if (isspace(*s)) {
		inword = 0;
		l->lines += (*s == '\n');
	    } else if (!inword) {
		inword++;
		l->words++;
	    }
	    s++;
	}
    }

    t->lines += l->lines;
    t->words += l->words;
    t->bytes += l->bytes;
}
/* comment this out
#if EGRESS_SCRIPT && 0
{terminator >>>}
#
# WC
#
sho version
echo this is foo > foo
echo this is bar > bar
cat foo
wc foo bar
ec -l foo bar
wc -c foo bar
wc -w foo bar
ls -l | wc
ls barfo
wc barfo
wc -t foo
wc foo barfo foo
ls -l > foo
wc foo foo foo foo foo foo foo foo foo foo

#endif EGRESS_SCRIPT
*/
