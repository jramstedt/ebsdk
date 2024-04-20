/* file:	tr.c
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
 *	Shell TR command.  Translate characters.
 *#
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      14-Feb-1991
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	05-Dec-1991	fixed recognition of null characters
 *
 *	ajb	02-Aug-1991	Finished
 *--
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"


/*+
 * ============================================================================
 * = tr - Translate chararacters from stdin to stdout.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *    The tr command copies the standard input to the standard output with
 *    substitution or deletion of selected characters.  Input characters found
 *    in string1 are mapped into the corresponding characters of string2.
 *    When string2 is short it is padded to the length of string1 by duplicat-
 *    ing its last character.  Any combination of the options -cds may be
 *    used: -c complements the set of characters in string1 with respect to
 *    the universe of characters whose ASCII codes are 0 through 0377 octal;
 *    -d deletes all input characters in string1; -s squeezes all strings of
 *    repeated output characters that are in string2 to single characters.
 *
 *    In either string the notation a-b means a range of characters from a to
 *    b in increasing ASCII order.  The backslash character (\) followed by 1,
 *    2 or 3 octal digits stands for the character whose ASCII code is given
 *    by those digits.  A \ followed by any other character stands for that
 *    character.
 *
 *    TR is binary transparent; it's operation is not restricted to ascii
 *    strings.
 *
 *   COMMAND FMT: tr_cmd 2 Z 0 tr
 *
 *   COMMAND FORM:
 *  
 *	tr ( [-{c,d,s}] [<string1>] [<string2>] )
 *  
 *   COMMAND TAG: tr_cmd 0 RXBZ tr
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<string1> - Specifies the input characters to be translated.
 * 	<string2> - Specifies the output translated characters.
 *
 *   COMMAND OPTION(S):
 *
 * 	-c	- Complement characters in string1. 
 *	-d	- Delete all characters in string1 from output.
 *	-s	- Squeeze all repeated characters in string2 to one character.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>tr a-z A-Z <foo		# Translate lower case to upper case.
 *	>>>tr -s [\ \011\012] \012	# Break out words one per line.
 *	>>>tr -s [\ \011] [\ \011]	# Collapse multiple blanks and tabs.
 *	>>>tr -sd [ ]			# Remove blanks.
 *	>>>tr -sd '\000'		# Remove nulls from stdin.
 *~
 * FORM OF CALL:
 *  
 *	tr ( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Success
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
int tr_cmd (int argc, char **argv) {
	struct PCB	*pcb;
	struct FILE	*fin, *fout;
	int		i;
	unsigned char	string1 [256], *s1, inv_string1 [256];
	unsigned char	string2 [256], *s2, inv_string2 [256];
	unsigned char	xlate [256];
	unsigned int	s1_len, s2_len;
	int		complement, squeeze, delete;
	int		lastc, c;

	/*
	 * Pick off qualifiers
	 */
	complement = squeeze = delete = 0;
	argv++; argc--;
	if (argc  && (**argv == '-')) {
	    complement = strchr (*argv, 'c');
	    squeeze = strchr (*argv, 's');
	    delete = strchr (*argv, 'd');
	    argv++; argc--;
	}

	/* if first string is defined, read it in */
	s1_len = 0;
	if (argc) {
	    s1_len = expand_string (*argv, string1);
	    argv++; argc--;
	}

	/* if second string is also defined, read it in */
	s2_len = 0;
	if (argc) {
	    s2_len = expand_string (*argv, string2);
	    argv++; argc--;
	}

	/*
	 * Use files if present on command line
	 */
	pcb = getpcb ();
	fin = pcb->pcb$a_stdin;
	fout = pcb->pcb$a_stdout;

	/*
	 * If the -c qualifier is present, complement string1 with respect to
	 * the universe of characters (if string1 was not defined, then -c 
	 * will generate the entire character set).
	 */
	if (complement) {
	    invert_string (string1, s1_len, inv_string1);
	    s1 = string1;
	    for (i=0; i<256; i++) {
		if (!inv_string1 [i]) {
		    *s1++ = i;
		}
	    }
	    s1_len = s1 - string1;
	}

	/*
	 * Pad a short string2 to the length of string1 with the last character
	 * in string2.
	 */
	while (s2_len && (s2_len < s1_len)) {
	    string2 [s2_len] = string2 [s2_len-1];
	    s2_len++;
	}

#if LOCAL_DEBUG
	show_charset ("string1", string1, s1_len);
	show_charset ("string2", string2, s2_len);
#endif
	
	/*
	 * Build the translation vector
	 */
	for (i=0; i<256; i++) xlate [i] = i;
	for (i=0; i<s1_len; i++) {
	    xlate [string1 [i]] = string2 [i];
	}

	/*
	 * invert the strings.  When inverted, using the character as an index
	 * determines if the character is present in the original string.
	 */
	invert_string (string1, s1_len, inv_string1);
	invert_string (string2, s2_len, inv_string2);

	/*
	 * Transcribe from stdin to stdout.  If no strings have been
	 * specified, this degenerates to a simple copy.
	 */
	lastc = -1;
	while ((c = fgetc (fin)) != EOF) {

	    /* delete all characters that are in string1 */
	    if (delete) {
		if (inv_string1 [c]) continue;
	    }

	    /* translate character */
	    c = xlate [c];

	    /* squeeze all repeated characters in string 2 */
	    if (squeeze) {
		if ((c == lastc) &&  inv_string2 [c]) continue;
	    }

	    /* write out the character */
	    fputc (c, fout);
	    lastc = c;
	}

	return msg_success;
}


/*----------*/
void invert_string (unsigned char *s, int len, unsigned char *inv) {
	memset (inv, 0, 256);
	while (len--) inv [*s++] = 1;
}

#define isoctal(x) (('0' <= (x)) && ((x) <= '7'))


int tr_next (unsigned char **s, int *quoted) {
	int		i;
	int	number;
	unsigned char	c;


	/* not quoted */
	*quoted = 1;
	c = **s;
	(*s)++;
	if (c != '\\') {
	    *quoted = 0;
	    if (c == 0) {
		return -1;
	    } else {
		return c;
	    }
	}

	/* backslash at end of string */
	if (**s == 0) {
	    return '\\';

	/*
	 * octal escape sequence.
	 */
	} else if (isoctal (**s)) {
	    i = 0;
	    number = 0;
	    while ((i<3)  &&  **s  &&  isoctal (**s)) {
		number *= 8;
		number += **s - '0';
		(*s)++;
	    }
	    return number;


	/* quoted character */
	} else {
	    c = **s;
	    (*s)++;
	    return c;
	}
}

	
/*
 * Expand a string
 */
int expand_string (unsigned char *input, unsigned char *output) {
	unsigned char	*in;
	unsigned char	*out;
	unsigned char	lower, upper;
	int		c;
	int		quoted;

	in = input;
	out = output;

	while ((c = tr_next (&in, &quoted)) != -1) {

	    /*
	     * Character range.
	     */
	    if ((c == '-')  && !quoted) {

		/* if at the begining or end, no special significance */
		if ((in == (input+1)) || (*in == 0)) {
		    *out++ = '-';

		/* calculate range */
		} else {
		    out--;
		    lower = *out;
		    upper = tr_next (&in, &quoted);
		    for (c = lower; c <= upper; c++) {
			*out++ = c;
		    }
		}

	    } else {
		*out++ = c;
	    }
	}

	*out++ = 0;
	return (out - output - 1);
}

#if LOCAL_DEBUG

void main (int argc, char *argv []) {
	tr_cmd (argc, argv);
}

void show_charset (char *header, unsigned char *set, int n) {
	unsigned char	invert [256];
 	char	 	ascii [17];
	int		row, col, c;

	invert_string (set, n, invert);

	printf ("\n%s %d\n", header, n);
	for (row=0; row < 256; row += 16) {
	    printf ("%02X: ", row);
	    for (col=0; col<16; col++) {
		c = row + col;
		printf ("%1d ", invert [c]);
		if (invert [c]  && isprint (c)) {
		    ascii [col] = c;
		} else {
		    ascii [col] = ' ';
		}
	    }
	    ascii [16] = 0;
	    printf (" %s\n", ascii);
	}
}
#endif
