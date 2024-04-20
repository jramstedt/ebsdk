/* file:	regexp.c
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
 *
 *
 * Abstract:	Routines that deal with regular expressions for
 *		EVAX/Cobra firmware.
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
#include	"cp$src:common.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:version.h"
#include	"cp$src:stddef.h"
#include	"cp$src:ctype.h"
#include	"cp$src:limits.h"
#include	"cp$src:regexp_def.h"
#include	"cp$src:ansi_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:parse_def.h"
#include "cp$inc:prototypes.h"

/*
 * The universal_class is the set of all possible characters.  Null and delete
 * are not members of this set.
 */
unsigned int universal_class [] = {
 	0xfffffffe,
	0xffffffff,
	0xffffffff,	/* ascii lower case	*/
	0x7fffffff,  	/* ascii upper case	*/
	0xffffffff,
	0xffffffff,
	0xffffffff,	/* iso lower	*/
	0x7fffffff	/* iso upper	*/
};

#define	ASCII_MASK	0x07fffffe /* bitmasks for picking off */
#define	ISO_MASK	0x7fffffff /* upper and lower case	*/



#define singlequote '\''
#define doublequote '"'
#define backquote   '`'
#define backslash   '\\'

/*+
 * ============================================================================
 * = regexp_extract - extract fields from a regular expression                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Extract the tokens from a regular expression that has been compared.
 *	The expression and string compared against are used to extract the
 *	appropriate characters from the string.
 *
 *	Correct results are guarantteed if the source and destination
 *	strings are the same.
 *  
 * FORM OF CALL:
 *  
 *	regexp_extract (regexp, mask, string, symbol)
 *  
 * RETURN CODES:
 *
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 * 	struct REGEXP *regexp	- address of regular expression
 *	char *mask		- ascii string consisting of "0" and "1"
 *				  indicating which tokens to extract.
 *	char *src		- source string
 *	char *dst		- where extracted characters are stored
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int regexp_extract (struct REGEXP *regexp, char *mask, char *src, char *dst) {
	struct TOKEN	*tp;
	int		tx, j;

	tp = regexp->tlist;

	for (tx=0; *src  && *mask  && tx<regexp->tcount; tx++, tp++, mask++) {
	    for (j=0; j<tp->cur; j++) {
		if (*mask == '1') {
		    *dst++ = *src;
		}
		src++;
	    }
	}

	*dst = 0;
	return (int) msg_success;
}


/*+
 * ============================================================================
 * = regexp_compile - convert a regular expression into an internal form      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Compile a regular expression into an internal form, and return a
 *	pointer to this internal form.  The space for the internal form
 *	is allocated from the heap.
 *  
 * FORM OF CALL:
 *  
 *	regexp_compile (expression, foldcase)
 *  
 * RETURN CODES:
 *
 *	0 - error
 *	x - address of compiled regular expression
 *      
 * ARGUMENTS:
 *
 * 	char *expression	- address of null terminated string defining
 *				  regular expression
 *	int foldcase		- flags word that controls case sensitivity
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
struct REGEXP *regexp_compile (char *expression, int foldcase) {
	struct TOKEN	*tp;
	struct REGEXP	*re;
	int		explength;
	unsigned int	c;
	int		i, j;

	/*
	 * Allocate and initialize the tokens.  For an initial allocation,
	 * use the size of the string.  The number of tokens will always
	 * be less than this number.
	 */
	re = malloc (sizeof (struct REGEXP));
	explength = strlen (expression);

	re->tlist = malloc (explength * sizeof (struct TOKEN));
	memset (re->tlist, 0, explength * sizeof (struct TOKEN));
           
	/*
	 * Visit each character in the regular expression
	 */
	tp = re->tlist;

	while (c = *expression++) {
	    tp->minimum = 1;
	    tp->maximum = 1;

	    switch (c) {

	    /*
	     * Tag to begining of line.  The ^ character only has this
	     * significance if it is the first character in the expression.
	     */
	    case '^':
		if (tp == re->tlist) {
		    tp--;
		    re->tagbol = 1;
		} else {
		    setbit (1, c, tp->chmask);
		}		
		break;


	    /*
	     * Tag to end of line.  The $ character only has this significance
	     * if it is the last character in the expression.
	     */
	    case '$':
		if (*expression == 0) {
		    tp--;
		    re->tageol = 1;
		} else {
		    setbit (1, c, tp->chmask);
		}		
		break;

	    /*
	     * wildcard character
	     */
	    case '.':
		memcpy (tp->chmask, universal_class, sizeof (universal_class));
		break;

	    /*
	     * 0 or 1 occurences of the previous token.
	     */
	    case '?':
		/* ? at the begining of the line represents itself */
		if (tp == re->tlist) {
		    setbit (1, c, tp->chmask);
		} else {
		    tp--;
		    tp->minimum = 0;
		    tp->maximum = 1;
		}
		break;

	    /*
	     * 1 or more occurences of the previous token.
	     */
	    case '+':
		/* + at the begining of the line represents itself */
		if (tp == re->tlist) {
		    setbit (1, c, tp->chmask);
		} else {
		    tp--;
		    tp->minimum = 1;
		    tp->maximum = INT_MAX;
		}
		break;

	    /*
	     * Kleene closure.  0 or more occurences of the previous token.
	     * If this is the first character, treat it as a normal character.
	     */
	    case '*':
		/* * at the begining of the line represents itself */
		if (tp == re->tlist) {
		    setbit (1, c, tp->chmask);
		} else {
		    tp--;
		    tp->minimum = 0;
		    tp->maximum = INT_MAX;
		}
		break;

	    case '[':
		if (compile_class (&expression, tp->chmask)) {
		    regexp_release (re);
		    return 0;
		}
		break;


	    /*
	     * Just received a BACKSLASH, so we treat the next character as
	     * representing itself.
	     */
	    case backslash:
		c = *expression++;

	    /*
	     * None of the special characters were flagged, so we treat
	     * the character as representing itself.
	     */
	    default:
		setbit (1, c, tp->chmask);
	    	break;
	    }

	    /* advance to the next token */
	    tp++;
	}
        re->tcount = tp - re->tlist;
	

	/*
	 * revisit each token in the list and apply the value of the 
	 * foldcase switch.
	 */
	if (foldcase) {
	    for (tp=re->tlist, i=0; i<re->tcount; i++, tp++) {
		regexp_foldcase (tp->chmask);
	    }
	}

	return re;
}

/*
 * Fold cases on a bitvector that represents the ISO-Latin character set.
 */
void regexp_foldcase (int *class) {

	/* ascii */
	class [2] |= class [3] & ASCII_MASK;	/* fold upper into lower */
	class [3] |= class [2] & ASCII_MASK;	/* fold lower into upper */

	/* iso latin supplemental graphic characters */
	class [6] |= class [7] & ISO_MASK;	/* fold upper into lower */
	class [7] |= class [6] & ISO_MASK;	/* fold lower into upper */
}


/*+
 * ============================================================================ 
 * = regexp_release - release storage for regular expression                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Release the storage associated with the context for a regular
 *	expression.
 *
 * FORM OF CALL:
 *  
 *      regexp_release (struct REGEXP *re) 
 *  
 * RETURN CODES:
 *
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 *	struct REGEXP *re - pointer to storage to be freed 
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void regexp_release (struct REGEXP *re) {
	free (re->tlist);
	free (re);
}

/*+
 * ============================================================================
 * = compile_class - compile a class construct for regular expressions        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Compile a character class construct for regular expressions, scanf
 *	character delimiters etc.
 *	Character classes are delimited by square brackets.  
 *	Characters inside the brackets are give no special interpretation
 *	except for '\', '-', '[', and ']'.
 *	Ranges of characters may be specified, such as 0-9, A-Z etc.
 *	On errors, the value of the input pointer is undefined.
 *  
 * FORM OF CALL:
 *  
 *      compile_class (s, class)	
 *  
 * RETURN CODES:
 *
 *      msg_success		- Normal completion.
 *      msg_lt_brckt_expected	- Sequence not starting with '['.
 *      msg_null_terminated	- Sequence prematurely terminated with NULL.
 *      msg_hyphen_prefix	- Hyphen encountered at begining of sequence.
 *      msg_hyphen_suffix	- Hyphen encountered at end of sequence.
 *
 * ARGUMENTS:
 *
 *      char **s      -	address of pointer to string
 *      int *class    -	bitmask of characters in class
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int compile_class (char **s, int *class) {
	int	c, low, high;
	int	inversion;
	int	i;
	struct TOKEN 	*t;

	/*
	 * clear class
	 */
	memset (class, 0, sizeof (t->chmask));

	/*
	 * Inversion character only has meaning if
	 * it is the first.
	 */
	inversion = 0;
	low = 0;
	while (c = *(*s)++) {
	    switch (c) {
	    case '~':
		if (low == 0) {
		    inversion = 1;
		} else {
		    class [c / 32] |= 1 << (c % 32);
		}
		break;
		    
	    case 0:	  /* Null */
		return (int) msg_null_terminated;
		break;

	    case '-':
		if (low == 0) return (int) msg_hyphen_prefix;
		high = *(*s)++;
		if (high == 0) return (int) msg_hyphen_suffix;
		for (c=low; c<=high; c++) {
		    class [c / 32] |= 1 << (c % 32);
		}
		break;

		
	    case ']':
		if (inversion) {
    		    for (i=0; i<8; i++) {
			class [i]  ^= -1;
			class [i] &= universal_class [i];
		    }
		}
		return (int) msg_success;

	    case backslash:
		c = *(*s)++;
	    default:
		class [c / 32] |= 1 << (c % 32);
		break;
	    }
	    low = c;
	}
	return (int) msg_success;
}

/*+
 * ============================================================================
 * = mayberegular - check a string for regular expression metacharacters      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Determine if a string could possibly be a candidate for a regular
 *      expression expansion.  The routine may occassionaly return true when
 *      in fact no expansion is possible, as in the case where the
 *      metacharacters have been protected by quoting.
 *  
 * FORM OF CALL:
 *  
 *	mayberegular (char *s)
 *  
 * RETURNS:
 *
 *	0 - string has no regular expression metacharacters
 *	1 - string may have regular expression metacharacters
 *       
 * ARGUMENTS:
 *
 * 	char *s	- address of string
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int mayberegular (char *s) {
	return strpbrk (s, "*?[]");
}

/*+
 * ============================================================================
 * = regexp_shell - compile a shell regular expression                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Compile a shell regular expression into an internal form, and return
 *	a pointer to this internal form.  The space for the internal form
 *	is allocated from the heap.  Shell regular expressions are simpler
 *	than the generic regular expressions allowed in other contexts.
 *	Uses static built-in class tables.
 *
 * FORM OF CALL:
 *  
 *	regexp_shell (shell_expr, foldcase)
 *  
 * RETURNS:
 *
 *      x - address of regular expression data structure REGEXP structure.
 *
 * ARGUMENTS:
 *
 *      char *shell_expr - address of null terminated string defining regular
 *			   expression in shell terms
 *	int foldcase	 - flags word that controls case sensitivity
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
struct REGEXP *regexp_shell (char *expression, int foldcase) {
	struct TOKEN	*tp;
	struct REGEXP	*re;
	int		explength;
	unsigned int	c, term;
	int		i;

	/*
	 * Allocate and initialize the tokens.  For an initial allocation,
	 * use the size of the string.  The number of tokens will always
	 * be less than this number.
	 */
	re = (void *) malloc (sizeof (struct REGEXP));
	explength = strlen (expression);

	re->tlist = (void *) malloc (explength * sizeof (struct TOKEN));
	memset (re->tlist, 0, explength * sizeof (struct TOKEN));
           
	/*
	 * Visit each character in the regular expression
	 */
	tp = re->tlist;

	while (c = *expression++) {
            tp->minimum = 1;
	    tp->maximum = 1;
	
	    switch (c) {

	    /*
	     * Nothing is translated inside single quotes
	     */
	    case singlequote:
		term = c;
		while (*expression  && ((c = *expression++) != term)) {
		    setbit (1, c, tp->chmask);
		    tp++;
		    tp->minimum = 1;
		    tp->maximum = 1;
		}
		break;

	    /*
	     * backslashes are translated inside double quotes
	     */
	    case doublequote:
		term = c;
		while (*expression && ((c = *expression++) != term)) {
		    if (c == backslash) {
			c = *expression++;
		    }
		    setbit (1, c, tp->chmask);
		    tp++;
		    tp->minimum = 1;
		    tp->maximum = 1;
		}
		break;

	    /*
	     * Wildcard characters.
	     */
	    case '%':
	    case '?':
		memcpy (tp->chmask, universal_class, sizeof (universal_class));
		break;

	    /*
	     * Kleene closure on a wildcard.  0 or more occurances of anything.
	     */
	    case '*':
		memcpy (tp->chmask, universal_class, sizeof (universal_class));
		tp->minimum = 0;
		tp->maximum = INT_MAX;
		break;

	    case '[':
		if (compile_class (&expression, tp->chmask)) {
		    regexp_release (re);
		    return 0;
		}
		break;

	    /* next character is taken literally, fall through to default */
	    case backslash:
		c = *expression++;

	    /*
	     * None of the special cases were flagged, so we treat
	     * the character as representing itself.
	     */
	    default:
		setbit (1, c, tp->chmask);
	    	break;
	    }

	    /* advance to the next token */
	    tp++;
	}
	re->tcount = tp - re->tlist;


	/*
	 * revisit each token in the list and apply the value of the 
	 * foldcase switch.
	 */
	if (foldcase) {
	    for (tp=re->tlist, i=0; i<re->tcount; i++, tp++) {
		regexp_foldcase (tp->chmask);
	    }
	}

	return re;
}

/*+
 * ============================================================================
 * = regexp_match - match regular expression with string                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Verify that a regular expression matches an entire string.
 *
 * FORM OF CALL:
 *  
 *      regexp_match (re, s)
 *  
 * RETURNS:
 *
 *       1 (match) or 0 (no match)
 *
 * ARGUMENTS:
 *
 *      struct REGEXP *re - address of internal form of regular expression
 *      char *s           - address of string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int regexp_match (struct REGEXP *re, char *s) {
	if (anchored_match (re, s, 0) == strlen (s)) {
	   return 1;
	} else {
	   return 0;
	}
}

/*+
 * ============================================================================
 * = regexp_subset - match regular expression with substring                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Verify that a regular expression matches at least some
 *	non empty substring in a string.
 *
 * FORM OF CALL:
 *  
 *      regexp_subset (struct REGEXP *re, char *s)
 *  
 * RETURNS:
 *
 *       1 (match) or 0 (no match)
 *
 * ARGUMENTS:
 *
 *      struct REGEXP *re - address of internal form of regular expression
 *      char *s           - address of string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int regexp_subset (struct REGEXP *re, char *s) {
	int	i;

	/*
	 * Special case null strings
	 */
	if (re->tcount == 0) return 1;

	for (i=0; s [i]; i++) {
	    if (anchored_match (re, s, i)) return 1;
	    if (i && re->tagbol) break;
	}
	return 0;
}

/*+
 * ============================================================================
 * = anchored_match - match regular expression with substring                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Anchored match.  Given a pattern with possible closures and a string,
 *	see if the pattern matches the string, using as much of the string
 *	as possible.
 *
 * FORM OF CALL:
 *  
 *      anchored_match (re, s, index)
 *  
 * RETURNS:
 *
 *	Number of characters accepted before reaching a final state.
 *	This value returned will typically be 0 or the string length.
 *	A return value of 0 indicates no match.
 *
 * ARGUMENTS:
 *
 *      struct REGEXP *re - address of internal form of regular expression
 *      char *s           - address of string
 *      int index         - starting location in string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int anchored_match (struct REGEXP *re, char *s, int index) {
	int  		i;
	char		found;  
	struct TOKEN	*tp;
	unsigned int  	c;
	int		tc;
	int		shortened;
	int		chars_matched;

	/*
	 * can't possibly match if not at the begining of the line
	 */
	if (re->tagbol && index) return 0;

	found = 1;
	chars_matched = 0;
	tc = 0;
	tp = re->tlist;	
 
        while (found && tc<re->tcount) {

	    /*
	     * Eat as many characters as we can, up to the maximum allowed
	     * for this token, or until a mismatch (including an EOL).
	     */
	    tp->cur = 0;
	    while (found && tp->cur<tp->maximum) {
		c = (unsigned char) s [index + chars_matched];
		found = getbit (c, tp->chmask);
		chars_matched += found;
		tp->cur += found;
	    }

	    /*
	     * Insure that we are in range
	     */                
	    if ((tp->minimum <= tp->cur)  &&  (tp->cur <= tp->maximum)) {
		found = 1;

	    /*
	     * We are out of range, i.e. we have a mismatch at the current
	     * position.  If there is a previous closure, shorten
	     * it up by one and start over again from the token following
	     * the shortened one.  If we can't shorten any closures, then
	     * the match has failed.
	     */
	    } else {
		shortened = 0;
		while (tc >= 0) {
		    if (tp->cur > tp->minimum) {
			tp->cur--;
			chars_matched--;
			shortened = 1;
			found = 1;
			break;
		    } else {
			chars_matched -= tp->cur;
			tp->cur = 0;
			tc--;
			tp--;
		    }
		}

		/*
		 * the match failed if we didn't shorten anything
		 */
		if (!shortened) {
		    found = 0;
		}
	    }
		        
	    tc++;
	    tp++;
	}

	/*
	 * The match failed if we're not at the end of the string
	 */
	if (re->tageol) {
	    if (s [index + chars_matched] != 0) return 0;
	}

	return chars_matched;
}

#if VALIDATE

/*----------*/
void tdump (struct TOKEN *t) {
	int	i;

	printf ("	%4d %11d %4d ",
		t->minimum,
		t->maximum,
		t->cur
	);

	for (i=0; i<4; i++) {
	    printf (" %08X", t->chmask [i]);
	}
	printf ("\n");
}

/*----------*/
void regexp_render (struct REGEXP *re) {
	int		i;
	struct TOKEN	*t;

	
	printf ("	%4d	count %s %s\n",
	    re->tcount,
	    re->tagbol ? "(bol)" : "",
	    re->tageol ? "(eol)" : ""
	);

	t = re->tlist;
	for (i=0; i<re->tcount; i++) {
	    tdump (t);
	    t++;
	}
}

/*
 * The following data structure is used to test the regular expression
 * routines.
 */
struct {
	int	options;	/* what options to pass to expression compiler	*/
	char	*expr; 		/* regular expression				*/
	char	*s;		/* test string					*/
	int	match;		/* what the match routine should return		*/
	int	subset;		/* what the subset routine should return	*/
} tests [] = {
	{0,	"abcdefghijklmnopqrstuvwxyz",	"abcdefghijklmnopqrstuvwxyz",	1,	1},	/* automatic case conversion	*/
	{1,	"abcdefghijklmnopqrstuvwxyz",	"abcdefghijklmnopqrstuvwxyz",	1,	1},
	{0,	"abcdefghijklmnopqrstuvwxyz",	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",	0,	0},
	{1,	"abcdefghijklmnopqrstuvwxyz",	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",	1,	1},
	{0,	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",	"abcdefghijklmnopqrstuvwxyz",	0,	0},
	{1,	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",	"abcdefghijklmnopqrstuvwxyz",	1,	1},
	{0,	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",	1,	1},
	{1,	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",	1,	1},

	{0,	"*abcd",			"*abcd",		1,	1},	/* metacharacters at begining of line */
	{0,	".abcd",			".abcd",		1,	1},
	{0,	"?abcd",			"?abcd",		1,	1},
	{0,	"+abcd",			"+abcd",		1,	1},

	{0,	"^foobar",			"foobar",		1,	1},
	{0,	"^foobar",			" foobar",		0,	0},
	{0,	"^foobar",			"foobar ",		0,	1},
	{0,	"foobar$",			"foobar",		1,	1},
	{0,	"foobar$",			" foobar",		0,	1},
	{0,	"foobar$",			"foobar ",		0,	0},
	{0,	"^foobar$",			"foobar",		1,	1},
	{0,	"^foobar$",			" foobar",		0,	0},
	{0,	"^foobar$",			"foobar ",		0,	0},

	{0,	"[0-9]*[a-z]",			"0123456789z",		1,	1},	/* test character classification	*/
	{0,	"[0-9]+[a-z]",			"0123456789z",		1,	1},
	{0,	"[0-9]+[a-z]?",			"0123456789z",		1,	1},
	{0,	"[0-9]*[a-z]?",			"0123456789z",		1,	1},
	{0,	"[0123456789]*[a-z]",		"0123456789z",		1,	1},
	{0,	"[0-12-789]*[a-z]",		"0123456789z",		1,	1},

	{0,	"[\\0-9]*[a-z]",		"0123456789z",		1,	1},
	{0,	"[\\0-\\9]*[a-z]",		"0123456789z",		1,	1},


	{0,	"simple string",		"simple string",	1,	1},
	{0,	"",				"simple string",	0,	1},
	{0,	"",			"",				-1,	-1}	/* table terminator	*/
};

/*----------*/
void render_test (int testnum, int match_result, int subset_result) {
	printf ("%2d: %08X %d(%d) %d(%d) %20.20s %20.20s\n",
	    testnum,
	    tests [testnum].options,
	    tests [testnum].match, match_result,
	    tests [testnum].subset, subset_result,
	    tests [testnum].expr,
	    tests [testnum].s
	);
}

/*+
 * ============================================================================
 * = regexp_validate - run validation suite for regular expression matching   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Run the validation suite for regular expressions. Successful completion
 *      of this routing indicates a high probability of no errors in the 
 *	routines.
 *
 * FORM OF CALL:
 *  
 *      regexp_validate ()
 *  
 * RETURN CODES:
 *
 *	msg_success 		- Normal completion.
 *	msg_poorly_formed_exp	- Encountered poorly formed regular expression.
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
int regexp_validate () {
	int		i;
	int		argix;
	struct REGEXP	*re;
	int		match_result;
	int		subset_result;
	char		symbol [512];

	/*
	 * First execute the built in tests
	 */
	for (i=0; tests [i].match >= 0; i++) {
	    re = regexp_compile (tests [i].expr, tests [i].options);
	    if (re == 0) {
		printf ("Poorly formed expression: %s\n", tests [i].expr);
		continue;
	    }
	    match_result = regexp_match (re, tests [i].s);
	    subset_result = regexp_subset (re, tests [i].s);
	    if ((match_result != tests [i].match)  ||
		(subset_result != tests [i].subset)) {
		regexp_render (re);
		render_test (i, match_result, subset_result);
	    }
	    regexp_release (re);
	}

#if 0
	/*
	 * Treat the first argument as a regular expression, the second as an
	 * extraction mask, and all subsequent arguments
	 * as strings that the regular expression is matched against.
	 */
	if (argc < 4) return 0;
	if ((re = regexp_compile (argv [1], 0)) == 0) {
	    printf ("Poorly formed regular expression\n");
	    return (int) msg_poorly_formed_exp;
	}
	regexp_render (re);
	for (argix=3; argix<argc; argix++) {
	    match_result = regexp_match (re, argv [argix]);
	    subset_result = regexp_subset (re, argv [argix]);
	    regexp_extract (re, argv [2], argv [argix], symbol);
	    printf ("	%d %d |%s| %s\n", match_result, subset_result, symbol, argv [argix]);
	}	    
#endif

	return (int) msg_success;
}

#endif

#define	BITS_PER_INT (sizeof (int) * 8)

int getbit (unsigned int pos, unsigned int *base) {
	return base [pos / BITS_PER_INT] & (1 << (pos % BITS_PER_INT)) ? 1 : 0;
}

int getbit64(unsigned int pos, uint64 base)
{
    uint64 data;
    uint64 offset = pos / 64;
    uint64 addr = base + offset * 8;

    ldqp(&addr, &data);
    return (data >> (pos % 64)) & 1;
}

void setbit (unsigned int value, unsigned int pos, unsigned int *base) {
	if (value) {
	    base [pos / BITS_PER_INT] |= (1 << (pos % BITS_PER_INT));
	} else {
	    base [pos / BITS_PER_INT] &= ~(1 << (pos % BITS_PER_INT));
	}
}
