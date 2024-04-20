/* file:	qscan.c
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
 *      These routines facilitate the parsing of Unix style qualifiers from
 *	an argc, argv construct.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *	jds - Jim Sawin
 *
 *  CREATION DATE:
 *  
 *      29-Jun-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	jrk	20-May-1995	add qscan help
 *
 *	ajb	30-Dec-1991	Don't recognize qualifier introducers in
 *				the middle of a string when verifying that
 *				all qualifiers have been found.
 *
 *	phk	13-Aug-1991	add FBE_STANDALONE switch for the standalone
 *				FBE DMA_GEN version
 *
 *	twp	07-Aug-1991	Fixed case of unexpected qualifiers on command
 *				line
 *
 *	jds	20-Jun-1991	Allow '-' as a valid argument for examine/deposit.
 *
 *	ajb	06-Mar-1991	Don't copy qualifer strings into structure,
 *				copy just pointers.  Hard code qualifier
 *				introducer to - for performance.
 *
 *	ajb	25-Feb-1991	Remove wildcarding in qscan.  Remove feature
 *				where single character qualifiers could be 
 *				concatenated.
 *
 *	dtm	 8-Feb-1991	Change atoi to xtoi for hex qualifier values.
 *				Check for qualifiers on case-insensitive basis.
 *	cfc	13-Dec-1990	Fixed test for present in second argv loop.
 *
 *	cfc	04-Dec-1990	Qualifiers now return values when a ":"
 *				is appended to a legal qualifier.
 *				Fixed bug in loop along qual string:
 *				Step with var i;  was a "1".
 *	dtm	25-Sep-1990	Add message codes when calling callbk routine
 *	cfc	12-Sep-1990	Compile portable.
 *	cfc	12-Sep-1990	Retain original parameters in ARGV after wild card expansion.
 *	cfc	30-Jul-1990	Add wild card expansion.
 *	ajb	18-Jul-1990	Parameterize the character set that starts
 *				qualifiers and codify the messages returned.
 *--
 */

#include "cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:stddef.h"
#include "cp$inc:prototypes.h"

/*
 * Specifier types.  Some qualifiers take parameters.  We allow
 * numeric (decimal and hex) and string parameters.
 */
#define SPEC_SIMPLE	0
#define SPEC_DECIMAL	1
#define SPEC_HEX   	2
#define SPEC_CSTRING  	3
#define SPEC_ERROR	4

#define HELP_KEY	0x8		/* ^h */

/*+
 * ============================================================================
 * = qscan - Pick off qualifiers and their values from a command line.        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine will extract Unix style qualifiers from a command line,
 *	delivered as an argc, argv construct, and fill in a structure 
 *	indicating the presence/absence of those qualifiers.  A list of valid
 *	qualifers is passed to this routine so that it knows what to look for.
 *
 *	Unix has several, sometimes conflicting, conventions for handling
 *	qualifiers.  This routine attempts to handle some of these conventions
 *	in a consistent fashion.
 *
 *	The word "qualifier" is used instead of "flag" to emphasize that
 *	qscan handles true multicharacter qualifiers, such as "longword",
 *	and not single letters ("l").
 *
 *	If the qualifiers take arguments, then those also will be converted
 *	and placed into the structure. Various scanf style conversion rules
 *	are available.  If a qualifer is tagged as having an argument, an
 *	argument will always be required, as there is no way at run time to
 *	indicate if the next parameter on the command line belongs to the
 *	qualifier or should be left alone.  For example, if the "-n" qualifier
 *	were to have an optional value on the examine command, then the
 *	command "e -n 1234 " is ambiguous; qscan would not know if the 1234
 *	is to be taken as an argument to the -n or to be left alone in the
 *	argv array.
 *
 *	The qualifiers presented to qscan must include all qualifiers the
 *	command expects.  Qscan complains if it finds a qualifier in the
 *	argv array that wasn't in the valid qualifier list.
 *
 *	Qualifiers may be single characters ("flags") or multiple characters.
 *	Single character qualifiers may be concatenated together, as long as
 *	the concatenation doesn't conflict with a multicharacter qualifier.
 *  
 *	The model presented by this routine is that all qualifiers are global,
 *	i.e. they can appear anywhere on the command line.  This a more
 * 	restrictive model than some U*x commands have.  Qscan will not be
 *	of much help to these commands.
 *
 *	This routine is not intended to be a general purpose command line
 *	syntax/qualifier evaluation routine, but a routine with a relatively
 *	simple interface that gracefully removes the complications that
 *	global qualifiers present.
 *
 *	Qscan does not attempt to validate combinations of qualifiers.  It
 *	does leave around a structure to facilitate validation by the
 *	caller.
 *
 *	There is no way to specify a list of arguments to a qualifer.
 *
 *	Several U*x commands take a numeric value with no flag introducer,
 *	for example "head -56".  To accomodate this model, qscan will
 *	recognize one and only such conversion opn the command line.  In the
 *	call to qscan, specifiy a "%d" or "%x" without the identfying string.
 *	If the number is found, it will be put into the appropriate index into
 *	the qstruct. 
 *
 *	To indicate that a qualifier takes a value, preceed the qualifier 
 *	with one of the following.
 *
 *~
 *	%x	treat parameter as a hex number
 *	%d	treat parameter as a signed decimal number
 *	%s	treat parameter as a string.
 *~
 * FORM OF CALL:
 *  
 *	qscan ( argc, argv, qlist, flist, qvb )
 *  
 * RETURN CODES:
 *
 *	msg_success		- success
 *	msg_failure		- failure, may be help, no printable status
 *	msg_qscan_noqual	- qualifiers found on command line that weren't in the list
 *	msg_qscan_conv		- conversion error on one or more qualifiers
 *	msg_qscan_allocerr	- failure in malloc to define storage for array tmp_argv, arg_element_ptr.
 *	msg_qscan_misc		- other miscellaneous errors
 *      
 * ARGUMENTS:
 *
 * 	int *argc		- address of callers argc
 *	char *argv []		- address of array of pointers to strings
 *	char *qintro		- character set denoting qualifier introducers
 *	const char *flist	- address of string indicating legal qualifiers
 *	struct QSTRUCT *qvb	- address of data structure that gets filled in
 *				  indicating presence/absence of qualifiers
 *                                If NUL, then no qualifiers are searched.
 *
 * SIDE EFFECTS:
 *
 *	Caller's ARGC, ARGV array is modified.  Specifically, all qualifiers in 
 *	the format list are magically removed.
 *
-*/
int qscan (
int		*argc,
char		*argv [],
const char	*qintro,
const char 	*flist,
struct QSTRUCT	*qvb
) {
	char	qvc;		/* number of qualifiers		*/
	struct PCB *pcb;
	struct QSTRUCT *qvs;
	int	val;
	int	argix;		/* iterates over argv	*/
	int	i, j;
	int	status;
	char	*qlist;
	int	matches;
	int	last_found, found, exact;


	/*
	 * Register with the PCB
	 */
	pcb = getpcb();
	pcb->pcb$a_qualp = (int *) qvb;

	/*
	 * There's nothing to do if no qualifiers were passed.
	 */
	if (qvb == 0) return msg_success;

	/*
	 * Transcribe the qualifiers from the argument list to
	 * the structure and figure out if they have required
	 * parameters.
	 */
	qvc = 0;
	qvs = qvb;
	qlist = flist;
	while (next_element (&qlist, "	 ", qvs->id)) {
	    qvs->present = 0;
	    qvs->value.integer = 0;
	    qvs->val_type = classify_specifier (qvs->id);
	    if (qvs->val_type == SPEC_ERROR) return msg_qscan_valerr;
	    qvc++;
	    qvs++;
	}

	/*
	 * The first pass over the argument list looks for exact matches,
	 * or proper substrings that are unique.
	 */
	for (argix=0; argix < *argc; argix++) {

	    /* Is this a qualifier introducer ? */

	    if (!strchr (qintro, argv [argix] [0])) continue;

#if !MODULAR
	    /*
	     * Nothing to do if this isn't a real qualifier (let
	     * a raw '-' pass through).
	     */
	    if (argv [argix] [1] == 0) continue;
#endif
#if MODULAR
	    /*
	     * If qualifier introducer by itself, is next help ?
	     * No, Nothing to do if this isn't a real qualifier (let
	     * a raw '-' pass through).
	     */
	    if (argv [argix] [1] == 0) {
		if (*argv[argix+1] == HELP_KEY) {
		    printf("  qualifiers are:\n");
		    for (qvs=qvb, i=0; i<qvc; qvs++, i++) {
			printf("\t%s", qvs->id);		    
			switch (qvs->val_type){
			    case SPEC_DECIMAL:
				printf(" <decimal number>");
				break;
			    case SPEC_CSTRING:
				printf(" <string>");
				break;
			    case SPEC_HEX:
				printf(" <hex number>");
				break;
			}
			pcrlf();
		    }
		    return msg_failure;			/* for now */
		} else {
		    continue;
		}
	    }
#endif
	    /*
	     * Apply this argument to all the qualifiers
	     * present to find a unique match (exact or proper substring)
	     */
	    exact = 0;
	    found = 0;
	    last_found = -1;
	    for (qvs=qvb, i=0; !exact && i<qvc; qvs++, i++) {
		switch (substr_nocase (& argv [argix] [1], qvs->id)) {
		case 0:		/* no match */
		    break;
		case 1:		/* exact match */
		    found = 1;
		    last_found = i;
		    exact = 1;
		    break;
		case 2:		/* abbreviation */
		    found++;
		    last_found = i;
		}
	    }

	    /*
	     * Check for unambiguous match
	     */
	    if (found != 1) continue;
    
	    /*
	     * parse the qualifiers
	     */
	    qvs = & qvb [last_found];
	    qvs->present++;
	    val = 0;
	    switch (qvs->val_type){
	    case SPEC_DECIMAL:
		if ((argix+1) >= *argc) return msg_qscan_misc;
		status = common_convert (argv [argix+1], 10, &qvs->value.integer, sizeof qvs->value.integer);
		if (status) return status;
		val++;
		break;

	    case SPEC_CSTRING:
		if ((argix+1) >= *argc) return msg_qscan_misc;
		qvs->value.string = argv [argix+1];
		val++;
		break;
	    case SPEC_HEX:
		if ((argix+1) >= *argc) return msg_qscan_misc;
		status = common_convert (argv [argix+1], 16, &qvs->value.integer, sizeof qvs->value.integer);
		if (status) return status;
		val++;
		break;
	    }

	    /*
	     * Remove the parameters from the command line.  The variable FOUND
	     * indicates how many are to be removed.
	     */
	    *argc = *argc - found - val;
	    for (i=argix; found && i<*argc; i++) {
		argv [i] = argv [i+found+val];
	    }
	    argix--; /*argix will be bumped by loop ctr*/
	}

	/*
	 * For the second pass over the command line, assume that single
	 * character qualifiers can be grouped togther.
	 */
	for (argix=1; argix < *argc; argix++) {

	    /*
	     * Nothing to do if this isn't a real qualifier (let
	     * a raw '-' pass through).
	     */
	    if (!strchr (qintro, argv [argix] [0])) continue;
	    if (argv [argix] [1] == 0) continue;

	    val = 0;
	    found = 0;
	    for (qvs=qvb, i=0; i<qvc; qvs++, i++) {
		if (qvs->id [1]) continue; /* must be 1 character */
		if (qvs->present) continue; /* must be free */
		if (strpbrk (& argv [argix] [1], & qvs->id [0])) {
		    found++;
		    if (qvs->val_type != SPEC_SIMPLE)
			val++;
		}
	    }

	    /* allow processing of one non-simple single digit qualifier. */
	    if (found != strlen (& argv [argix] [1]) || val > 1) continue;

	    for (qvs=qvb, i=0; i<qvc; qvs++, i++) {
		if (qvs->id [1]) continue; /* must be 1 character */
		if (qvs->present) continue; /* must be free */
		if (strpbrk (& argv [argix] [1], & qvs->id [0])) {
		    if (qvs->val_type != SPEC_SIMPLE ) {
			if ((argix+1) >= *argc) return msg_qscan_misc;
			switch (qvs->val_type){
			case SPEC_DECIMAL:
			    status = common_convert (argv [argix+1], 10,
			      &qvs->value.integer, sizeof qvs->value.integer);
			    if (status) return status;
			    break;
			case SPEC_CSTRING:
			    qvs->value.string = argv [argix+1];
			    break;
			case SPEC_HEX:
			    status = common_convert (argv [argix+1], 16,
			      &qvs->value.integer, sizeof qvs->value.integer);
			    if (status) return status;
			    break;
		        }
		    }
		    qvs->present = 1;
		}
	    }

	    /*
	     * remove from the argv array if found
	     */
	    if (found) {
		*argc = *argc - 1 - val;
		for (i=argix; i < *argc; i++) {
		    argv [i] = argv [i+1+val];
		}
		argix--;
	    }
   	}


	/*
	 * The third pass over the argument list looks for raw numeric
	 * qualifiers (eq: more -56 foo).  These show up in the qualifier
	 * specification list as "%d%, "%x" etc.  Only the first one of
	 * these is treated as a numeric qualifier, and then only if it is
	 * decimal.
	 */
	found = 0;
	for (argix=1; !found && argix < *argc; argix++) {

	    /*
	     * Nothing to do if this isn't a real qualifier (let
	     * a raw '-' pass through).
	     */
	    if (!strchr (qintro, argv [argix] [0])) continue;
	    if (argv [argix] [1] == 0) continue;

	    for (qvs=qvb, i=0; i<qvc; qvs++, i++) {
		if ((qvs->val_type == SPEC_DECIMAL) && (qvs->id [0] == 0)) {
		    status = common_convert (
			&argv [argix] [1],
			10,
			&qvs->value.integer,
			sizeof qvs->value.integer
		    );
		    if (status) return status;
		    qvs->present = 1;

		    /* remove it from the argument list */
		    (*argc)--;		
		    for (j=argix; j < *argc; j++) {
			argv [j] = argv [j+1];
		    }
		    argix--;
		    found = 1;
		    break;
		}
	    }
	}

	/*
	 * Walk the command line for the fourth time.  All qualifiers should
	 * have been "eaten" by now.  We verify this by insuring that all
	 * arguments left over don't start with a qualifier introducer.
	 * If they do, and there are characters following the introducer,
	 * we complain.  Thus, "foo -" passes.
	 */
 	for (argix=1; argix < *argc; argix++) {
	    if (strchr (qintro, argv [argix] [0])) {
		if (argv [argix] [1]) {
		    return msg_qscan_noqual;
		}
	    }
	}

	/*
	 * Terminate the arglist with a null argument
	 */
	argv [*argc] = NULL;
	return msg_success;
}

/*+
 * ============================================================================
 * = next_element - find next field in a string                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Extract the first field in string, where a field is delimited by a
 *	character set (the field separators).  The string address is passed
 *	by reference, and is updated by this routine so that successive calls
 *	will pick each field in the string.
 *
 *	The resulting string is not touched if no fields are found.
 *  
 * FORM OF CALL:
 *  
 *	next_element (char **s, char *sep, char *result)
 *  
 * RETURNS:
 *
 *	0 - no more fields
 *	1 - field is present
 *       
 * ARGUMENTS:
 *
 *      char **s - pointer to string address
 *	char *sep - string of characters defining field separators
 *	char *id - resultant null terminated field. Set to a null string if
 *	            np field was found.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int next_element (char **s, char *fieldsep, char *result) {
	char *id;

	id = result;

	/*
	 * Pick off leading field separators.
	 */
	while (strchr (fieldsep, **s)) (*s)++;

	/*
	 * Extract field
	 */
	while (**s && !strchr (fieldsep, **s)) {
	    *id++ = **s;
	    (*s)++;
	}

	/* return true if we found something, 0 otherwise */
	if (id == result) {
	    return 0;
	} else {
	    *id = 0;
	    return 1;
	}
}

/*
 * Determine what kind of qualifier a string is.  As a side effect, remove any
 * conversion specifications.
 *
 */
int classify_specifier (char *id) {
	int	spec_type;
	char *start;

	switch (*id) {
	    case '%':
		start = id;
		id++;
		switch (*id) {
		    case 'x':
			spec_type = SPEC_HEX;
			id++;
			break;
		    case 'd':
			spec_type = SPEC_DECIMAL;
			id++;
			break;
		    case 's':
			spec_type = SPEC_CSTRING;
			id++;
			break;
		    default:
			spec_type = SPEC_ERROR;
		    }
		strcpy (start,id);
		return spec_type;
	    default: return SPEC_SIMPLE;
	    }	    
}

#if 0
/*+
 * ============================================================================
 * = qst  - Qscan test routine, used to provide coverage for qscan.           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	QST is a routine that provides coverage for qscan.  QST is to
 *	qualifiers as echo is to command line arguments.  Qualifiers to
 *	QST are chosen to test various paths through qscan.
 *  
 *   COMMAND FORM:
 *  
 *	qst ( [<file>...] )
 *  
 *   TAG: qst 0 RXBZ qst
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<file>... - Specifies the name of file(s) to search.
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *
 *	None
 *
 * FORM OF CALL:
 *  
 *	qst ( int argc, char *argv [] )
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
static char qstring [128] = "a %dd %xx %ss l o longword";
static int qmax = 7;
int qst (int argc, char *argv []) {
	struct QSTRUCT *qual;
	int status;
	int i;

	qual = dyn$_malloc (sizeof (struct QSTRUCT) * qmax, DYN$K_SYNC);
	status = qscan (&argc, argv, "-", qstring, qual);
	printf ("%08X\n", status);
	show_qual (qual, qmax);

	/*
	 * Echo out remaining arguments as verification that we didn't
	 * eat anything we shouldn't have.
	 */
	for (i=0; i<argc; i++) {
	    printf ("%2d: %s\n", i, argv [i]);
	}

	return status;
}

/*
 * Redefine the qualifier string
 *    TAG: qdef 0 RXBZ qdef
 */
int qdef (int argc, char *argv []) {
	char *cp;
	int	i;
	char	element [64];


	cp = qstring;
	for (i=1; i<argc; i++) {
	    cp += sprintf (cp, "%s ", argv [i]);
	}

	for (qmax=0; strelement (qstring, qmax, " ", element); qmax++);
	printf ("%2d: %s\n", qmax, qstring);
	return msg_success;
}

char *spec_strings [] = {
	"simple  ",
	"decimal ",
	"hex     ",
	"string  ",
	"error   ",
};

/*----------*/
void show_qual (struct QSTRUCT *qual, int n) {
	int	i;

	for (i=0; i<n; i++) {
	    pprintf ("%2d: %10.10s %s %s",
		i,
		qual->id,
		spec_strings [qual->val_type],
		qual->present ? "    present " : "not present "
	    );                                   
	    if (qual->present) {
		switch (qual->val_type) {
		case SPEC_SIMPLE:
		    break;
		case SPEC_DECIMAL:
		    pprintf ("%d", qual->value.integer);
		    break;
		case SPEC_HEX:
		    pprintf ("%X", qual->value.integer);
		    break;
		case SPEC_CSTRING :
		    pprintf ("%s", qual->value.string);
		    break;
		default:
		    pprintf ("???");
		    break;
		}
	    }
	    pprintf ("\n");
	    qual++;
	}
}
#endif
