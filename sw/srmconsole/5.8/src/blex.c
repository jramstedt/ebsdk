/* file:	blex.c
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
 *      Lexical  analyzer/parser  for a Bourne shell.
 *
 * We've had to rearrange left recursive
 * rules using the following technique (page 48 in the third edition of the
 * dragon book)
 *		A : A a | b
 *	becomes:
 *		A : b R
 *		R : a R | epsilon 
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      21-Mar-1991
 *
 *  MODIFICATION HISTORY:
 *
 *	jrk	14-Sep-1993	Overlay integration
 *
 *	pel	21-Jul-1992	remove check of qscan status from sh routine so
 *				that scripts will be able to have qualifiers.
 *
 *	ajb	21-Jul-1992	Remove most shell qualifiers so that they don't
 *				conflict with qualifiers used by scripts.
 *
 *	pel	06-Mar-1992	add arg to satisfy new read_with_prompt functn.
 *
 *	ajb	17-jan-1992	change ntoi calling sequence
 *
 *	kp	09-Jan-1991	Remove CB_PROTO usage.
 *
 *	phk	13-Nov-1991	modify fbe prompt
 *
 *	ajb	18-oct-1991	Remove trailing newlines on command
 *				substitution (I thought I fixed this long
 *				ago!)
 *
 *	ajb	10-Sep-1991	Add stderr redirection using the construct
 *				2>filename.  Only stderr is recognized in this
 *				context.
 *
 *	ajb	09-Sep-1991	perform parameter substitution before creating
 *				background process so that sequences such as
 *				for i in * ; do echo $i ; done 
 *				get the correct values if $i.  Add -c qualifier
 *				to suppress parameter substitution.
 *
 *	phk	05-Sep-1991	add support for fbe prompt
 *
 *	jad	05-Aug-1991	add support for cb_proto
 *
 *	ajb	17-Jul-1991	add routine headers
 *
 *	ini	dd-mmm-yyyy	{@tbs@}
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:regexp_def.h"
#include	"cp$src:ev_def.h"
#include	"cp$src:tt_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:parser.h"
#include	"cp$src:blex.h"
#include	"cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#if MODULAR
#define sh_routine adv_sh_routine
#endif

#define DEBUG 0
#define SHELL_DEBUG 0

#if __DECC_VER > 60000000
#define C57_COMPILER 0		/* fix for loop iteration problem */
#else
#define C57_COMPILER 1		/* fix for loop iteration problem */
#endif

#if IMP
extern int nothing_but_cpu;
#endif

#if DEBUG
int blex_debug = 0;
#define malloc(size) \
  dyn$_malloc (size, DYN$K_SYNC|DYN$K_FLOOD, 0, 0, 0);
#define realloc(p, new_size) \
  dyn$_realloc (p, new_size, DYN$K_SYNC|DYN$K_FLOOD, 0);
#define free(p) \
  dyn$_free (p, DYN$K_SYNC|DYN$K_FLOOD);
#endif

extern int cpu_mask;
extern int pipe_flag;

#if SECURE
extern int secure;
#endif

extern int shell_startup();
extern struct LOCK spl_kernel;
extern struct DDB *pi_ddb_ptr;
extern int shell_stack;
extern unsigned char idset[];

#if TURBO || RAWHIDE
extern int console_dlb_flag;
#endif
/*
 * Max number of words in a wordlist
 * AFTER all expansions and substitutions
 */
int max_shell_words = 400;

/*
 * The following structure defines all the tokens that the lexical analyzer
 * recognizes.  Some of the fields in the structure are only used for
 * debugging.
 */
struct {
	char 	value;
	char	keyword_precursor;
	char	pad1;
	char	pad2;
	char	*name;
	char	*literal;	/* what is printed when pretty printing	*/
} tlist [] = {
	{t_null,	0,	0, 0,	"null",		""},
	{t_space,	0,	0, 0,	"space",	" "},
	{t_eof,		0,	0, 0,	"t_eof",	""},
	{t_in,		0,	0, 0,	"t_in",		"<"},
	{t_in_here,	0,	0, 0,	"t_in_here",	"<<"},
	{t_out,		0,	0, 0,	"t_out",	">"},
	{t_out_append,	0,	0, 0,	"t_out_append",	">>"},
	{t_err,		0,	0, 0,	"t_err",	"2>"},
	{t_err_append,	0,	0, 0,	"t_err_append",	"2>>"},
	{t_cpuspec,	0,	0, 0,	"t_cpuspec",	"&p"},
	{t_word,	0,	0, 0,	"t_word",	"word"},
	{t_ampersand,	1,	0, 0,	"t_ampersand",	"&"},
	{t_and,		1,	0, 0,	"t_and",	"&&"},
	{t_pipe,	1,	0, 0,	"t_pipe",	"|"},
	{t_or,		1,	0, 0,	"t_or",		"||"},
	{t_semicolon,	1,	0, 0,	"t_semicolon",	";"},
	{t_caseterm,	1,	0, 0,	"t_caseterm",	";;"},
	{t_lparen,	1,	0, 0,	"t_lparen",	"("},
	{t_rparen,	1,	0, 0,	"t_rparen",	")"},
	{t_comment,	0,	0, 0,	"t_comment",	"comment"},
	{t_universal,	0,	0, 0,	"t_universal",	"???"},
	{t_ret,		1,	0, 0,	"t_ret",	"\n"},

/* all following tokens are keywords and only keywords.  No keywords above this
 * line either.  Keep the "if" keyword first (otherwise change the word
 * recognizer).
 */
	{t_if,		1,	0, 0,	"t_if",		"if"},
	{t_then,	1,	0, 0,	"t_then",	"then"},
	{t_else,	1,	0, 0,	"t_else",	"else"},
	{t_elif,	1,	0, 0,	"t_elif",	"elif"},
	{t_fi,		1,	0, 0,	"t_fi",		"fi"},
	{t_case,	0,	0, 0,	"t_case",	"case"},
	{t_esac,	1,	0, 0,	"t_esac",	"esac"},
	{t_for,		0,	0, 0,	"t_for",	"for"},
	{t_while,	1,	0, 0,	"t_while",	"while"},
	{t_until,	1,	0, 0,	"t_until",	"until"},
	{t_do,		1,	0, 0,	"t_do",		"do"},
	{t_done,	1,	0, 0,	"t_done",	"done"},
	{t_lbrace,	1,	0, 0,	"t_lbrace",	"{"},
	{t_rbrace,	1,	0, 0,	"t_rbrace",	"}"},
	{t_inkey,	0,	0, 0,	"t_inkey",	"in"},
	{-1,		0,	0, 0,	0,		0}
};

#if SHELL_DEBUG
/*
 * Verify that the token list is consistent, ie. that the "value" field has the
 * same value as the index into tlist.
 *
 */
void tlist_verify () {
	int	i;
        struct PCB *pcb;

        pcb = getpcb ();

	for (i=0; tlist [i].value != -1; i++) {
	    if (tlist [i].value != i) {
		err_printf ("tlist [%d] is broken\n", i);
	    }
	}
}
#endif

/*
 * Initialize a dynamic string descriptor, and allocate some memory.
 */
void str_dyninit (struct STRDYN *d, int alloc) {
	d->alloc = alloc;
	d->ptr = malloc (d->alloc);
	d->len = 0;
	d->ptr [0] = 0;
}

/*
 * Free all the storage associated with a dynamic string.  Leave the
 * structure in a state such that a subsequent call to str_dyncat will
 * malloc (instead of realloc) some more memory.
 */
#if 0
void str_dynrelease (struct STRDYN *d) {
	if (d->ptr) free (d->ptr);
	d->ptr = 0;
	d->alloc = 0;
	d->len = 0;
}
#endif

/*
 * Concatenate a string onto a dynamic string.  Handle the case where
 * no space is currently allocated in the string (the alloc field will
 * be 0).
 */
void str_dyncat (struct STRDYN *d, char *s) {

	while (*s) {

	    /*
	     * Make more room if needed.  As a heuristic, over allocate
	     * to avoid a potential future reallocation.  Allow for a null
	     * terminator.  Don't reallocate if there isn't anything there.
	     */
	    if (d->len >= (d->alloc - 2)) {
		d->alloc += strlen (s) + 32;
		if (d->ptr) {
		    d->ptr = realloc (d->ptr, d->alloc);
		} else {
		    d->ptr = malloc (d->alloc);
		}
	    }

	    d->ptr [d->len++] = *s++;
	}
	   
	/* keep the string null terminated */
	d->ptr [d->len] = 0;
}

/*+
 * ============================================================================
 * = yy_lex - Lexical analyzer for a Bourne shell.			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine performs the lexical analysis for a Bourne shell.  It
 *	reads the input stream until a token is found.  If the input stream is
 * 	an interactive device, then this routine also handles the prompting.
 *
 *	In order for this routine to be reentrant, all data is kept in a
 *	context block whose address is passed to this routine.
 *  
 * FORM OF CALL:
 *  
 *	yy_lex ( struct YYCTX context )
 *  
 * RETURNS:
 *
 *	x - token number, including t_eof on EOF
 *       
 * ARGUMENTS:
 *
 * 	struct YYCTX *context	- pointer to context block for this instance of
 *				the lexical anlayzer.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int yy_lex (struct YYCTX *yyl) {
	int	c, cc;
	int	status;
	int	i;


	c = yy_get (yyl, 0);
	yyl->yyleng = 0;
	status = t_null;

	/*
	 * Eat leading whitespace
	 */
	if (c == ' ' || c == '\t') {
	    while (c = yy_get (yyl, 0)) {
		if (c != ' ' && c != '\t') break;
	    }
	    install_history (yyl, t_space);
	}

	/* end of file */
	if (c == EOF) {
	    status = t_eof;

	/* newline */
	} else if (c == '\n') {
	    yy_put (yyl, c);
	    status = t_ret;

	/*
	 * Gobble up comments and drop them so that a large comment
	 * doesn't overflow the token buffer.  Change them into a
	 * return token.
	 */
	} else if (c == '#') {		/* comment		*/
	    yy_put (yyl, c);
	    do {
		c = yy_get (yyl, 0);
	    } while ((c != EOF) && (c != '\n'));
	    if (c == '\n') {
		status = t_ret;
	    } else {
		status = t_eof;
	    }

	/* left parenthesis */
	} else if (c == '(') {
	    yy_put (yyl, c);
	    status = t_lparen;

	/* right parenthesis */
	} else if (c == ')') {
	    yy_put (yyl, c);
	    status = t_rparen;

	/* &, && or &p */
	} else if (c == '&') {
	    yy_put (yyl, c);
	    c = yy_get (yyl, 0);
	    switch (c) {
	    case '&':
		yy_put (yyl, c);
		status = t_and;
		break;
	    case 'p':
	    case 'P':
		yy_put (yyl, c);
		status = t_cpuspec;
		break;
	    default:
		status = t_ampersand;
		yy_unget (yyl);
	    }

	} else if (status = yy_sdt (yyl, c, '<', t_in, t_in_here)) {

	} else if (status = yy_sdt (yyl, c, '>', t_out, t_out_append)) {

	} else if (status = yy_sdt (yyl, c, '|', t_pipe, t_or)) {

	} else if (status = yy_sdt (yyl, c, ';', t_semicolon, t_caseterm)) {

	/* redirection of stderr, hardcoded to "2>" and "2>>" */
	} else if (c == '2') {
	    yy_put (yyl, '2');
	    cc = yy_get (yyl, 0);
	    status = yy_sdt (yyl, cc, '>', t_err, t_err_append);
	    if (status == t_null) {
		yy_unget (yyl);	/* release < */
		yyl->yyleng = 0;
	    }
	}

	/*
	 * If we haven't found anything, assume a start of a word
	 */
	if (status == t_null) {
	    status = yy_word (yyl, c);
	}

#if SHELL_DEBUG
	/*
	 * When debugging, show the token recognized
	 */
	if (yyl->debug) {
	    err_printf ("%25.25s	%2d %s\n",
		tlist [status].name,
		yyl->yyleng,
		(status == t_ret) ? "newline" : yyl->yytext
	    );
	}
#endif
	/*
	 * Install the token in the history buffer.
	 */
	yyl->current = status;
	install_history (yyl, status);

	return status;
}
/*
 * Install the current token into the token history.  Allow the token history to
 * grow dynamically, up to some (large) predetermined maximum. The history
 * buffer exists so that we may regenerate commands for constructs such
 * as the & operator that need to run in separate shells.
 *
 * We count on the initial allocation of the parse structure to be filled in
 * with 0 so that the yyl->tka field (number of tokens allocated) is 0.
 */
void install_history (struct YYCTX *yyl, int t) {


	/*
	 * Increase the size of the token history if necessary.
	 * Complain, but still work, if the number of tokens
	 * exceeds 2048
	 */
	if (yyl->tkc >= yyl->tka) {
	    if (yyl->tka >= 2048) {
		err_printf ("exceeding %d tokens, continuing\n", yyl->tka);
	    }

	    /* initial allocation */
	    if (yyl->tka == 0) {
		yyl->tka = 64;
		yyl->tkv = dyn$_malloc (yyl->tka * sizeof (char *), DYN$K_SYNC|DYN$K_NOOWN);

	    /* increase current space */
	    } else {
		yyl->tka += 32;
		yyl->tkv = dyn$_realloc (yyl->tkv, yyl->tka * sizeof (char *), DYN$K_SYNC|DYN$K_NOOWN);
	    }
	}

	/*
	 * Word tokens are allocated dynamically, all other tokens
	 * are put directly into the history buffer.
	 */
	if (t == t_word) {
	    yyl->tkv [yyl->tkc] = malloc (yyl->yyleng + 1);
	    strcpy (yyl->tkv [yyl->tkc], yyl->yytext);
	} else {
	    yyl->tkv [yyl->tkc] = t;
	}

	yyl->tkc++;
}
/*
 * Reset the lexical analyzer after errors, commands, etc.
 */         
void yy_reset (struct YYCTX *yyl)
{
        struct  EVNODE evn, *evp;

	yyl->input_buf [0] = 0;
	yyl->inp = yyl->input_buf;
	yyl->current = t_ret;
	yyl->px = 0;
	yyl->tkc = 0;
	yyl->yyleng = 0;

/* Update the primary prompt */

#if MAX_PROCESSOR_ID > 1
#if TURBO || RAWHIDE
        evp = &evn;
        if ( ev_read("prompt", &evp, 0) == msg_success ) {
	    if (console_dlb_flag)
	        sprintf(yyl->prompts[0], "LP%02d%s", whoami(), evp->value.string );
	    else
	        sprintf(yyl->prompts[0], "P%02d%s", whoami(), evp->value.string );
	} else {
	    if (console_dlb_flag)
	        sprintf (yyl->prompts [0], "LP%02d>>>", whoami ());
	    else
	        sprintf (yyl->prompts [0], "P%02d>>>", whoami ());
	}
#else
	    sprintf (yyl->prompts [0], "P%02d>>>", whoami ());
#endif
#else
	strcpy (yyl->prompts [0], ">>>");
#endif

/* Update the secondary prompt */

	strcpy (yyl->prompts [1], "_>");
}

/*
 * Character set that terminates a word.
 */
char wordterm [] = ";&|<>()	 \n";

/*
 * Characters that must be quoted within double quotes
 */
char dq_chars [] = "$`\\\"";

/*
 * Treat the current character as the start of a word.  Eat everything until
 * we see a word terminator.  Handle the various quoting mechanisms as follows:
 *
 *	Single quoted strings gobble up everything until the next single quote.
 *	No exceptions and no escapes.
 *
 *	Backquoted strings gobble up everything until the next backquote.
 *	Backquotes themselves are allowed to be escaped with a backslash.
 *
 *	Double quoted strings eat everything, except that escaped newlines are
 *	dropped.  Otherwise the routine checks for real terminating double
 *	quotes as opposed to escaped double quotes (which aren't real).
 */
int yy_word (struct YYCTX *yyl, int current) {
	int	status;
	int	i;
	int	c;
	char	term;

	c = current;
	status = t_word;

	/*
	 * read until a terminator
	 */
	do {
	    yy_put (yyl, c);

	    switch (c) {
	    /* eat everything up to and including next single quote */
	    case SINGLEQUOTE:
		while ((c = yy_get (yyl, 1)) != EOF) {
		    yy_put (yyl, c);
		    if (c == SINGLEQUOTE) break;
		}
		break;

	    /* eat everything up to and including next double/back quote */
	    case BACKQUOTE:
	    case DOUBLEQUOTE:
		term = c;
		while ((c = yy_get (yyl, 0)) != EOF) {
		    yy_put (yyl, c);
		    if (c == term) {
			if ((yy_backslash_count (yyl) & 1) == 0) {
			    break;
			}
		    }
		}
		break;

	    /* backslash, next character is eaten unconditionally */
	    case BACKSLASH:
		c = yy_get (yyl, 1);
		yy_put (yyl, c);
		break;
	    }

	    c = yy_get (yyl, 0);
	    if (c == EOF) {
		status = t_eof;
		break;
	    }
	} while (!strchr (wordterm, c));

	yy_unget (yyl); 

	/*
	 * If the previous token triggers keyword recognition, then
	 * check for a keyword.
	 */
	if (tlist [yyl->current].keyword_precursor) {
	    for (i = t_if; tlist [i].value != -1; i++) {
		if (strcmp (yyl->yytext, tlist [i].literal) == 0) {
		    status = tlist [i].value;
		    break;
		}
	    }
	}

	return status;
}

/*
 * Walk backward in the token buffer from the current character and count the
 * number of consecutive backslashes.  If the number is odd, then the current
 * character is escaped.  If even, then all the backslashes cancel each other
 * out.
 */
int yy_backslash_count (struct YYCTX *yyl) {
	int	n;
	int	index;

	index = yyl->yyleng - 2;
	n = 0;
	while (index > 0) {
	    if (yyl->yytext [index] == BACKSLASH) {
		n++;
	    } else {
		break;
	    }
	    index--;
	}
	return n;
}
/*
 * Return the next character from the input stream.  Various quoting mechanisms
 * require different treatment of escaped characters (a character preceeded by 
 * a backslash).  To handle these vagaries, there are two modes to this
 * routine:
 *
 *	In raw mode, no interpretation is done for any character sequence.
 * This mode is intended for use within single quotes.
 *
 *	In cooked mode, escaped newlines are eaten by this routine.  In other
 * words a newline preceeded by a backslash never makes it out of this routine.
 *
 * All other escaped characters are returned as backslash character with
 * nothing elided.
 */
int yy_get (struct YYCTX *yyl, int rawmode) {
	int	c;
	int	n;
	int	more;
	int	status;
        struct PCB *pcb;

        pcb = getpcb ();

	/*
	 * Loop until we have a character
	 */
	do {
	    more = 0;

	    /*
	     * If nothing is in the input buffer we have to read
	     * in another line.
	     */
	    if (*yyl->inp == 0) {
		if (yyl->tty) {
		    status = read_with_prompt (
			yyl->prompts [yyl->px],
			sizeof (yyl->input_buf),
			yyl->input_buf,
			0,
			0,
			1
		    );
		} else {
		    status = fgets (yyl->input_buf, sizeof (yyl->input_buf), yyl->yyin);
		    n = strlen (yyl->input_buf);
		    if (strcmp (&yyl->input_buf [n-2], "\r\n") == 0) {
			strcpy (&yyl->input_buf [n-2], "\n");
		    }
		}
		if (status == 0) return EOF;

		if (status == 1) {
		    c = yyl->input_buf [0];
		    if (c == 3) {	/* ^C, just reprompt */
			more = 1;
			pcb->pcb$l_killpending &= ~SIGNAL_CONTROLC;
		        yyl->input_buf [0] = 0;
			continue;
		    }
		}

		if (yyl->px == 0) yyl->px = 1; /* advance to secondary prompt */
		if (yyl->verify) {
		    printf ("%s", yyl->input_buf);
		}
		yyl->lineno++;
		yyl->inp = yyl->input_buf;
	    }

	    /* fetch the next character */
	    c = *yyl->inp++;

	    /*
	     * If we see a backslash and we're not in raw mode, then 
	     * we eat the next newline if present.
	     */
	    if ((c == '\\')  && (!rawmode)  &&  (*yyl->inp == '\n')) {
		yyl->inp++; /* skip over newline */
		more = 1;
	    }

	} while (more);

	return c;
}

/*
 * Push the curent character back into the input stream
 */
void yy_unget (struct YYCTX *yyl) {

	if (yyl->inp > yyl->input_buf) {
	    yyl->inp--;
	} else {
	    err_printf ("yy_lex pushback error\n");
	}
}
/*
 * Install the next character in the token buffer, checking for overflow.
 * Leave room for a terminating null.
 */
void yy_put (struct YYCTX *yyl, int c) {
	if (yyl->yyleng <= sizeof (yyl->yytext) - 1) {
	    yyl->yytext [yyl->yyleng++] = c;
	}
	yyl->yytext [yyl->yyleng] = 0;
}

/*
 * Certain tokens are either one or two instances of the same character.
 * This routine checks for such cases given the character (single/double token)
 *
 */
int yy_sdt (struct YYCTX *yyl, int c, int t, int r_single, int r_double) {
	if (c == t) {
	    yy_put (yyl, c);
	    c = yy_get (yyl, 0);
	    if (c == t) {
		yy_put (yyl, c);
		return r_double;
	    } else {
		yy_unget (yyl);
		return r_single;
	    }
	}
	return t_null;
}
/*======================================================================*/
/* parser starts here  */


#if SHELL_DEBUG
#define showrule(x,y) _showrule(x,y)
#else
#define showrule(x,y)
#endif

extern int exec_andor ();
extern int exec_case ();
extern int exec_command_list ();
extern int exec_command_sequence ();
extern int exec_for ();
extern int exec_if ();
extern int exec_null ();
extern int exec_pipe ();
extern int exec_simple ();
extern int exec_loop ();
extern int exec_word ();

/*
 * This list ties node types with names and action routines.
 */
struct NLIST nlist [] = {
	{n_andor,		"andor",		exec_andor},
	{n_case,		"case",			exec_case},
	{n_command_list,	"command_list",		exec_command_list},
	{n_command_sequence,	"command_sequence",	exec_command_sequence},
	{n_for,			"for",			exec_for},
	{n_if,			"if",			exec_if},
	{n_null,		"null",			exec_null},
	{n_pipe,		"pipe",			exec_pipe},
	{n_simple,		"simple",		exec_simple},
	{n_loop,		"loop",			exec_loop},
	{n_word,		"word",			exec_word},
	{-1,			"",			0},
};
#if SHELL_DEBUG
/*----------*/
void nlist_verify () {
	int	i;
        struct PCB *pcb;

        pcb = getpcb ();

	for (i=0; nlist [i].value != -1; i++) {
	    if (nlist [i].value != i) {
		err_printf ("nlist %d not valid (%d)\n", i, nlist [i].value);
	    }
	}
}
#endif

#define	DEF_KIDS	8		/* default number of kids	*/

/*
 * Clone some data and create a pointer to that data.
 */
void node_add (struct PARSE *p, struct NODE *np, void *data, int length) {
	void	*cp;

	node_make_room (p, np);
	np->kids [np->nkids++] = cp = malloc (length);
	memcpy (cp, data, length);
}

/*
 * Add a kid to the list, where the kid is just an integer
 */
void node_datum (struct PARSE *p, struct NODE *np, int pointer) {
	node_make_room (p, np);
	np->kids [np->nkids++] = pointer;
}

/*
 * make room for another kid
 */
void node_make_room (struct PARSE *p, struct NODE *np) {
	if (np->nkids >= np->slots) {
	    np->slots += DEF_KIDS;
	    np->kids = realloc (np->kids, sizeof (void *) * np->slots);
	}
}

/*
 * Create a node.
 */
int node_create (struct PARSE *p, int id) {
	struct NODE	*np;
	int	i;

	np = malloc (sizeof (struct NODE));
	np->nkids = 0;
	np->id = id;
	np->slots = DEF_KIDS;

	np->kids = malloc (sizeof (void *) * DEF_KIDS);

	np->flink = & np->flink;
	np->blink = & np->flink;

	/* curent token marks begining of this node */
	np->tstart = p->yyctx.tkc - 1;
	return np;
}
/*
 * Regenerate text from the parse tree.  In certain contexts, parameter
 * substitution is performed.
 */
void node_render (struct PARSE *p, struct NODE *np, struct FILE *fout, int parsub) {
	int	i;
	int	pingc, pongc;
	char	*pingv [1], *pongv [1];


	if (np == 0) return;
	pingc = 1;

	for (i=np->tstart; i<np->tstop; i++) {
	    if (p->yyctx.tkv [i] < t_last) {
		fprintf (fout, "%s", tlist [(int) p->yyctx.tkv [i]].literal);
	    } else {
		if (parsub) {
		    pingv [0] = p->yyctx.tkv [i];
		    sh_parsub (p, &pingc, &pingv, &pongc, &pongv, SUB_PARSUB);
		    fprintf (fout, "%s", pongv [0]);
		    if (pingv [0]) free (pongv [0]);
		} else {
		    fprintf (fout, "%s", p->yyctx.tkv [i]);
		}
	    }
	}
}

/*----------*/
void free_ptrs (int argc, char **argv) {
	int	i;
	for (i=0; i < argc; i++) {
	    if (argv [i]) free (argv [i]);
	    argv [i] = 0;
	}
}

/*----------*/
void free_argcv (int argc, char **argv) {
	free_ptrs (argc, argv);
	if (argv) free (argv);
}

/*+
 * ============================================================================
 * = shell - Create a new shell process.                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	The shell command creates another shell process.  Each shell process
 *	implements most of the functionality of the Bourne shell.
 *  
 * FORM OF CALL:
 *  
 *	sh ( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - success
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
#define QDELSTDIN	0
#define QVERIFY		1
#define QECHO		2
#define	QMAX		3
#define QSTRING		"d v x"

#if MODULAR
int adv_sh_routine (int argc, char *argv [])
#else
int sh_routine (int argc, char *argv [])
#endif
{
	int		i;
	struct PARSE	*p;	/* this is a BIG structure */
	struct YYCTX	*yyl;
	struct NODE	*np, *parent, parent_node;
	int		status;
	struct PCB	*pcb;
	struct FILE	*fp;
	struct INODE	*ip;
	char		*cp;
	struct QSTRUCT qual [QMAX];

	/*
	 * perform a quick and simple internal consistency check
	 */
#if SHELL_DEBUG
	tlist_verify ();
	nlist_verify ();
#endif

	pcb = getpcb ();
	p = malloc (sizeof (struct PARSE));
	pcb->pcb$l_shellp = p;

	/*
	 * Decode and set qualifiers.  Scripts can take qualifiers, and we
	 * don't want to have a lot of shell qualifiers colliding with them,
	 * so we lump all shell qualifiers as a parameter to "shellquals".
	 */
	qscan (&argc, argv, "-", QSTRING, qual);
	p->delstdin	= qual [QDELSTDIN].present;
	p->verify	= qual [QVERIFY].present;
	p->echo		= qual [QECHO].present;
	p->rules	= 0;
	p->ltoken	= 0;
	p->show_tree	= 0;


	/*
	 * Clone the shell's argc argv into the parse structure
	 */
	p->argc = argc;
	p->argv = argv;


	/*
	 * Always use stdin as the command stream.  This differs from the real
	 * Bourne shell in that it doesn't treat argv [1] as a command file.
	 */
	yyl = &p->yyctx;
	yyl->tty = isatty (pcb->pcb$a_stdin);
	yyl->yyin = pcb->pcb$a_stdin;
	yyl->filename = pcb->pcb$a_stdin_name;

	/*
	 * Fill in the pointer to the tty port block if stdin is a tty
	 * Register ourselves with the control c handler if we're a
	 * foreground shell and we haven't already been registered
	 * (this later case can occur if the shell is started by
	 * another shell).
	 */
	fp = pcb->pcb$a_stdin;
	if (isatty (fp)) {
	    p->ttpb = fp->ip->misc;
	    spinlock (&spl_kernel);
	    if (pcb->pcb$r_ctrlcq.flink == & pcb->pcb$r_ctrlcq.flink) {
		insq (&pcb->pcb$r_ctrlcq, p->ttpb->ctrlch.blink);
	    }
	    spinunlock (&spl_kernel);
	} else {
	    p->ttpb = 0;
	}

	/*
	 * initialize the lexical analyzer
	 */
	yy_reset (yyl);
	yyl->lineno = 1;
	yyl->debug = p->ltoken;
	yyl->verify = p->verify;

 	/*
	 * Set up a dummy parse node that describes the parent's i/o.  This
	 * allows all subsequent parse nodes to inherit the parent's  i/o.
	 */
	parent = & parent_node;
	parent->header = parent;
	parent->stdin_name = mk_dynamic (pcb->pcb$a_stdin_name);
	parent->stdout_name = mk_dynamic (pcb->pcb$a_stdout_name);
	parent->stderr_name = mk_dynamic (pcb->pcb$a_stderr_name);

	/*
	 * By default, we append to the parent's output
	 */
	strcpy (parent->stdin_mode, pcb->pcb$b_stdin_mode);
	strcpy (parent->stdout_mode, "a");
	strcpy (parent->stderr_mode, "a");


	/*
	 * Set up an exception handler for syntax errors.  Syntax errors in
	 * non foreground shells exit the shell.
	 */
	np = 0;
	if (status = setjmp (&p->jump_env)) {

	    yy_reset (yyl);
	    sh_flush (p);

	    if (!p->ttpb) {
		p->laststatus = msg_failure;
		p->request_exit = 1;
	    }
	}

	/*
	 * Tag all dynamic memory blocks that must be preserved when we flush.
	 */
	sh_tag (p);

	/*
	 * Read in commands until eof or someone kills us.
	 */
	do {
	    if (p->request_exit)
		break;

	    if (pcb->pcb$l_cmd_switch == 1)
		break;

	    /*
	     * Recognize and consume ^C.
	     */
	    if (pcb->pcb$l_killpending & SIGNAL_CONTROLC) {
		printf ("^C\n");
		pcb->pcb$l_killpending &= ~SIGNAL_CONTROLC;

		/*
		 * Turn ^C into a kill command if we're anywhere other
		 * than the foreground.
		 */
		if (!isatty (pcb->pcb$a_stdin)) {
		    pcb->pcb$l_killpending |= SIGNAL_CONTROLC | SIGNAL_KILL;
		}
	    }

	    /*
	     * If someone is trying to kill us (as with a kill command),
	     * we exit.
	     */
	    if (pcb->pcb$l_killpending & SIGNAL_KILL) {
		p->laststatus = msg_success;
		break;
	    }

	    /*
	     * reset the lexical analyzer
	     */
	    yy_reset (yyl);
	    p->lookahead = yy_lex (&p->yyctx);
	    p->level = 0;;

	    /* scan until we have a comamnd */
	    np = rule_top (p);

	    /*
	     * execute the command.
	     */
	    status = msg_success;
	    inherit (p, parent, np); /* pass stdio on to kids */
	    p->level = 0;
	    p->breakc = 0;
	    status = (*nlist [np->id].exec) (p, np);

	    /*
	     * release the parse tree and all the dynamic memory associated
	     * with it.
	     */
 	    sh_flush (p);

	} while (p->lookahead != t_eof);


	/*
	 * Close and delete stdin if the -d qualifier is present.  This feature
	 * is used by background and command substitution shells to
	 * automatically clean up scratch files before they exit.
	 */
	if (p->delstdin) {
	    fclose (pcb->pcb$a_stdin);
	    remove (pcb->pcb$a_stdin_name);
	}

	/* release the token history */
	free (yyl->tkv);

	/* return status of last command */
	status = p->laststatus;
	free(p);
	return status;
}

/*
 * Make a list in the parse structure of all the dynamic memory blocks
 * that are currently assigned to this shell process.  When we get errors,
 * all blocks except these blocks will be flushed.
 */
void sh_tag (struct PARSE *p) {
	struct DYNAMIC	*dp;
        struct QUEUE	*qe;
	struct PCB	*pcb;
	int	misses;

	pcb = getpcb ();

	p->prec = 0;
	qe = pcb->pcb$r_dq.flink;
	misses = 0;

	while (qe != &pcb->pcb$r_dq.flink) {
	    dp = (void *) ((int) qe - (offsetof (struct DYNAMIC, free_fl)));
	    qe = qe->flink;
#if DEBUG
	if (blex_debug)
	    printf("sh_tag: prec %x dp %x\n", p->prec, dp);
#endif
	    p->preowned [p->prec++] = dp;
	    if (p->prec >= sizeof (p->preowned) / sizeof (p->preowned [0])) {
		misses++;
	    }
	}
	if (misses) {
	    err_printf ("shell couldn't tag %d memory blocks\n", misses);
#if 0
	    sysfault (100);
#endif
	}

}

/*
 * Flush any memory allocated since we started parsing.  Exclude
 * memory that has already been tagged in the ownership list in 
 * the parse data structure.
 */
void sh_flush (struct PARSE *p) {
	int	i;
	struct PCB	*pcb;
	struct DYNAMIC	*dp;
        struct QUEUE	*qe;
	int		found;

	pcb = getpcb ();

	qe = pcb->pcb$r_dq.flink;
	while (qe != &pcb->pcb$r_dq.flink) {
	    dp = (void *) ((int) qe - (offsetof (struct DYNAMIC, free_fl)));
	    qe = qe->flink;
   	    found = 0;
	    for (i=0; !found && i<p->prec; i++) {
		if (p->preowned [i] == dp) found = 1;
	    }
	    if (!found) {
		dp->backlink = dp;
#if DEBUG
	if (blex_debug)
	    printf("sh_flush: dp %x\n", dp);
#endif
		free ((int) dp + sizeof (struct DYNAMIC));
	    }
	}
}
/*
 * Accept the current token without advancing to the next one.
 */
void sh_accept (struct PARSE *p, int t) {

	if (p->lookahead == t) {
	    ;

	} else {
	    syntax_error (p);
	}
}

/*
 * Accept the current token and fetch the next one.
 */
void sh_match (struct PARSE *p, int t) {

	/*
	 * accept current token and get the next one
	 */
	if (p->lookahead == t) {
	    p->lookahead = yy_lex (&p->yyctx);

	/*
	 * Complain if the current token isn't what we expected.
	 */
	} else {
	    syntax_error (p);
	}
}

/*
 * Verify that the current token is the keyword requested
 */
void match_keyword (struct PARSE *p, int keywordix) {

	/*
	 * accept current token and get the next one
	 */
	if (strcmp_nocase (tlist [keywordix].literal, p->yyctx.yytext) == 0) {
	    p->lookahead = yy_lex (&p->yyctx);

	/*
	 * Complain if the current token isn't what we expected.
	 */
	} else {
	    syntax_error (p);
	}
}

/*----------*/
void syntax_error (struct PARSE *p) {

	err_printf ("syntax error: '%s' unexpected on line %d, file %s\n",
	    (p->yyctx.current == t_ret ) ? "newline" : p->yyctx.yytext,
	    p->yyctx.lineno - 1,
	    p->yyctx.filename
	);

	longjmp (&p->jump_env, 2);
}


/*----------*/
int rule_top (struct PARSE *p) {
	struct NODE *np;

	showrule (p, "top");

	/* top : t_eof */
	if (p->lookahead == t_eof) {
	    np = node_create (p, n_null);

	/* top	: t_ret */
	} else if (p->lookahead == t_ret) {
	    np = node_create (p, n_null);
	    sh_accept (p, t_ret);

	/* top : command_list t_ret */
	} else {
	    np = rule_command_list (p);
	    sh_accept (p, t_ret);
	}

	np->tstop = p->yyctx.tkc;
	p->level--;
	return np;
}

static char andor_firsts [] = {
	t_lparen,		/* command firsts		*/
	t_lbrace,
	t_for,
	t_while,
	t_until,
	t_case,
	t_if,
	t_word,			/* item firsts			*/
	t_out,
	t_in,
	t_in_here,
	t_out_append,
	t_err,
	t_err_append,
	t_cpuspec,
	0
};


/*----------*/
int rule_command_sequence  (struct PARSE *p) {
	struct NODE	*np;
	int		x;

	showrule (p, "command_sequence");
	np = node_create (p, n_command_sequence);

	/* command_sequence	: command_sequence command_list */
	/* command_sequence	: command_list */
	while (strchr (andor_firsts, p->lookahead) || (p->lookahead == t_ret)) {
	    if (p->lookahead == t_ret)  {
		sh_match (p, t_ret);
	    } else {
		x = rule_command_list (p);
		node_datum (p, np, x);
	    }
	}

	np->tstop = p->yyctx.tkc;
	p->level--;
	return np;
}
/*
 * The children are structured as follows:
 *
 *	kid n	is a pointer to a command list
 *	kid n+1	is how the command was terminated
 *		(t_ret, t_ampersand, t_semicolon)
 */
int rule_command_list (struct PARSE *p) {
	struct NODE	*np;
	int		x;

	showrule (p, "command_list");
	np = node_create (p, n_command_list);

	/* command_list : andor */
	/* command_list : command_list ";" */
	/* command_list : command_list "&" */
	/* command_list : command_list ";" andor */
	/* command_list : command_list "&" andor */
	while (strchr (andor_firsts, p->lookahead)) {
	    x = rule_andor (p);
	    node_datum (p, np, x);
	    if ((p->lookahead == t_ampersand) ||
		(p->lookahead == t_semicolon)) {
		node_datum (p, np, p->lookahead);	/* termination type */
		sh_match (p, p->lookahead);
		if (strchr (andor_firsts, p->lookahead)) {
		    ;
		} else {
		    np->tstop = p->yyctx.tkc;
		    p->level--;
		    return np;
		}
	    } else {
		node_datum (p, np, t_ret);	/* termination type */
		np->tstop = p->yyctx.tkc;
		p->level--;
		return np;
	    }
	}

	syntax_error (p);
}

/*
 * The children are structured as follows:
 *
 *	kid n	is a pointer to a pipeline
 *	kin n+1	is how the command was terminated (t_ret, t_and, t_or)
 *
 * The last node is NULL indicating the end of the andor chain.
 */
int rule_andor (struct PARSE *p) {
	struct NODE	*np;
	int		x;

	showrule (p, "andor");
	np = node_create (p, n_andor);
	
	/* andor : pipeline */
	/* andor : pipeline "&&" andor */
	/* andor : pipeline "||" andor */
	while (strchr (andor_firsts, p->lookahead)) {
	    x = rule_pipe (p);
	    node_datum (p, np, x);
	    if ((p->lookahead == t_and) ||
		(p->lookahead == t_or)) {
		node_datum (p, np, p->lookahead);
		sh_match (p, p->lookahead);
	    } else {
		node_datum (p, np, t_null);
		np->tstop = p->yyctx.tkc;
		p->level--;
		return np;
	    }
	}

	syntax_error (p);
}
/*
 * The children are structured as follows:
 *
 *	kid n	is a pointer to a command
 *
 * pipeline firsts are the same as andor firsts
 */
int rule_pipe (struct PARSE *p) {
	struct NODE *np;
	int	x;

	showrule (p, "pipeline");
	np = node_create (p, n_pipe);

	/* pipeline : command | pipeline "|" command */
	while (strchr (andor_firsts, p->lookahead)) {
	    x = rule_command (p);
	    node_datum (p, np, x);
	    if (p->lookahead == t_pipe) {
		sh_match (p, t_pipe);
	    } else {
		np->tstop = p->yyctx.tkc;
		p->level--;
		return np;
	    }
	}

	syntax_error (p);
}
static char term_firsts [] = {t_ret, t_semicolon, 0};

/*----------*/
int rule_command (struct PARSE *p) {
	struct NODE	*np;
	int		i;

	showrule (p, "command");

	/* command	: "(" command_sequence ")" */
	if (p->lookahead == t_lparen) {
	    sh_match (p, t_lparen);
	    np = rule_command_sequence (p);
	    sh_match (p, t_rparen);

	/* command	: "{" command_sequence "}" */
	} else if (p->lookahead == t_lbrace) {
	    sh_match (p, t_lbrace);
	    np = rule_command_sequence (p);
	    sh_match (p, t_rbrace);

	/* kid 0	name rule	*/
	/* kid 1	word list or null */
	/* kid 2	command sequence */
	/* command	: "for" name "in" word_list "do" command_sequence "done" */
	/* command	: "for" name "do" command_sequence "done" */
	/* in the above case, substitute $* */
	} else if (p->lookahead == t_for) {
	    np = node_create (p, n_for);
	    sh_match (p, t_for);
	    np->kids [0] = rule_name (p);
	    if (!strchr (term_firsts, p->lookahead)) {
		match_keyword (p, t_inkey);
		np->kids [1] = rule_word_list (p);

	    /* no "in" keyword, so generate $* */
	    } else {
		np->kids [1] = node_create (p, n_word);
	        for (i=1; i<p->argc; i++) {
		    node_add (p, np->kids [1], p->argv [i], strlen (p->argv [i]) + 1);
		}
	    }

	    rule_term (p);
	    sh_match (p, t_do);
	    np->kids [2] = rule_command_sequence (p);
	    rule_term (p);
	    sh_match (p, t_done);
	    np->nkids = 3;
	
	/* kid 0	t_until, t_while */
	/* kid 1	conditional	*/
	/* kid 2	loop body	*/
	/*				*/
	/* command	: "while" command_sequence "do" command_sequence "done" */
	/* command	: "until" command_sequence "do" command_sequence "done" */
	/* command	: "while" command_sequence "done" */
	/* command	: "until" command_sequence "done" */
	} else if ((p->lookahead == t_until) || (p->lookahead == t_while)) {
	    np = node_create (p, n_loop);
	    np->kids [0] = p->lookahead;
	    sh_match (p, p->lookahead);
	    np->kids [1] = rule_command_sequence (p);
	    if (p->lookahead == t_do) {
		sh_match (p, t_do);
		np->kids [2] = rule_command_sequence (p);
	    } else {
		np->kids [2] = node_create (p, n_null);
	    }
	    sh_match (p, t_done);
	    np->nkids = 3;

	/* command	: "case" word "in" case_sequence "esac" */
	} else if (p->lookahead == t_case) {
	    np = rule_case (p);

	/* kid 0	boolean condition	*/
	/* kid 1	true part		*/
	/* kid 2	false part		*/
	/* command	: "if" command_sequence "then" command_sequence else_part "fi" */
	} else if (p->lookahead == t_if) {
	    np = node_create (p, n_if);
	    sh_match (p, t_if);
	    np->kids [0] = rule_command_sequence (p);
	    sh_match (p, t_then);
	    np->kids [1] = rule_command_sequence (p);
	    np->kids [2] = rule_else_part (p);
	    sh_match (p, t_fi);
	    np->nkids = 3;

	/* command	: simple_command */
	} else {
	    np = rule_simple_command (p);
	}

	np->tstop = p->yyctx.tkc;
	p->level--;
	return np;
}
char item_firsts [] = {
	t_word,
	t_in,
	t_in_here,
	t_out_append,
	t_out,
	t_err,
	t_err_append,
	t_cpuspec,
	0
};

/*
 * Structure of a simple command is:
 *
 *	kid 0	NULL or t_in wordlist 
 *	kid 1	NULL or t_in_here wordlist
 *	kid 2	NULL or t_out wordlist
 *	kid 3	NULL or t_out_append wordlist
 *	kid 4	NULL or t_err wordlist
 *	kid 5	NULL or t_err_append wordlist
 *	kid 6	NULL or affinity mask wordlist
 *	kid 7	pointer to wordlist rule
 *
 */
int rule_simple_command (struct PARSE *p) {
	struct NODE	*np;
	int		lookahead, y;

	showrule (p, "simple_command");
	np = node_create (p, n_simple);

	/* simple_command : item */
	/* simple_command : simple_command item */
	/* item : t_word  		*/
	/* item : t_cpuspec t_word 	*/
	/* item : input_output 		*/
	/* input_output : t_in t_word	*/
	/* input_output : t_in_here t_word */
	/* input_output : t_out t_word	*/
	/* input_output : t_out_append t_word */
	/* input_output : t_err t_word */
	/* input_output : t_err_append t_word */
	np->nkids = 8;
#if (8 > DEF_KIDS)
#error "8 > DEF_KIDS"
#endif
	while (strchr (item_firsts, p->lookahead)) {
	    lookahead = p->lookahead;

	    /*
	     * Create the node if it doesn't exist.  This has to happen
	     * before we match on the token so that the token window is
	     * accurate.
	     */
	    if (np->kids [lookahead - t_in] == 0) {
		np->kids [lookahead - t_in] = node_create (p, n_word);
	    }

	    switch (lookahead) {

	    case t_in:
	    case t_in_here:
	    case t_out:
	    case t_out_append:
	    case t_err:
	    case t_err_append:
	    case t_cpuspec:
		sh_match (p, p->lookahead);
		y = rule_word (p);
		node_datum (p, np->kids [lookahead - t_in], y);
		break;

	    case t_word:
		y = rule_word (p);
		node_datum (p, np->kids [lookahead - t_in], y);
        	break;
	    }
	}

	np->tstop = p->yyctx.tkc;
	p->level--;
	return np;
}
static char case_part_firsts [] = {t_word, 0};

/*
 * This case rule extends the standard Bourne shell by allowing an unlimited
 * number of patterns per case, eg:
 *
 *		case i in
 *			p0 | p1 | p2 ) echo " multiple pattern example" ;;
 *		esac
 *
 * Structure of the kids is:
 *	0	word
 *
 *	for every case:
 *
 *	1 + 2*n	pattern
 *	2 + 2*n	command sequence
 *
 */
int rule_case (struct PARSE *p) {
	struct NODE *np;
	struct NODE *wp;
	struct NODE *pp;	/* pattern pointer */
	int	x;

	/* case_sequence : case_part */
	/* case_sequence : case_sequence case_part */
	/* case_part : pattern_list ")" command_sequence ";;" */
	/* pattern_list : pattern */
	/* pattern_list : pattern | pattern_list */
	showrule (p, "case");

	/*
	 * Create the root node
	 */
	np = node_create (p, n_case);
	wp = node_create (p, n_word);
	sh_match (p, t_case);
	np->kids [0] = wp;
	wp->kids [0] = rule_word (p);
	wp->nkids = 1;
	match_keyword (p, t_inkey);
	rule_term (p);		/* eat terminator that postceeds "in" */
	np->nkids = 1;

	/*
	 * Process all the kids
	 */
	while (strchr (case_part_firsts, p->lookahead)) {

	    /* capture all the patterns for this case */
	    pp = node_create (p, n_word);
	    node_datum (p, np, pp);
	    while (1) {
		x = rule_pattern (p);		/* pattern_list		*/
		node_datum (p, pp, x);
		if (p->lookahead == t_pipe) {
		    sh_match (p, t_pipe);
		} else {
		    break;
		}
	    }

	    sh_match (p, t_rparen);		/* ) 		*/
	    x = rule_command_sequence (p);	/* sequence 	*/
	    node_datum (p, np, x);
	    sh_match (p, t_caseterm);		/* ;;		*/

	    /* eat optional case part terminator */
	    while (p->lookahead == t_ret) {
		sh_match (p, t_ret);
	    }
		
	}

	/*
	 * eat the case terminator
	 */
	sh_match (p, t_esac);

	np->tstop = p->yyctx.tkc;
	p->level--;
	return np;
}
/* Structure of the kids is:
 *
 *	kid 1	boolean command_sequence
 * 	kid 2	true part command sequence (elif only)
 *	kid 3	else part command sequence (elif only) 
 */
int rule_else_part (struct PARSE *p) {
	struct NODE *np;

	showrule (p, "else_part");
	
	/* else_part	: "elif" command_sequence "then" command_sequence else_part */
	if (p->lookahead == t_elif) {
	    np = node_create (p, n_if);
	    sh_match (p, t_elif);
	    np->kids [np->nkids++] = rule_command_sequence (p);
	    sh_match (p, t_then);
	    np->kids [np->nkids++] = rule_command_sequence (p);
	    np->kids [np->nkids++] = rule_else_part (p);

	/* else_part	: "else" command_sequence */
	} else if (p->lookahead == t_else) {
	    sh_match (p, t_else);
	    np = rule_command_sequence (p);

	/* else_part	: ; */
	} else {
	    np = node_create (p, n_null);
	    ; /* nothing */
	}

	np->tstop = p->yyctx.tkc;
	p->level--;
	return np;
}
/*
 * Return a pointer to the word.  Allocate some memory and copy the current
 * word into that memory
 */
int rule_word (struct PARSE *p) {
	char	*wp;

	showrule (p, "word" );

	/* word : */
	wp = mk_dynamic (&p->yyctx.yytext);
	sh_match (p, t_word);

	p->level--;
	return wp;
}

/*
 * construct a word list
 */
int rule_word_list (struct PARSE *p) {
	struct NODE	*np;
	int		x;

	showrule (p, "word_list");
	np = node_create (p, n_word);

	/* word_list : word */
	/* word_list : word_list word */
	while  (p->lookahead == t_word) {
	    x = rule_word (p);
	    node_datum (p, np, x);
	}

	np->tstop = p->yyctx.tkc;
	p->level--;
	return np;
}
/*----------*/
int rule_name (struct PARSE *p) {
	char	*wp;

	/* name : letter (letter | digit | _ )+ */
	showrule (p, "name");
	
	/*
	 * Verify that the token is an identifier
	 */
	if (p->lookahead == t_word) {
	    if (strspn (p->yyctx.yytext, idset) == p->yyctx.yyleng) {
		if (isalpha (p->yyctx.yytext [0])) {
		    wp = mk_dynamic (&p->yyctx.yytext);
		    sh_match (p, t_word);
		    p->level--;
		    return wp;
		}
	    }
	}

	syntax_error (p);
}

/*----------*/
int rule_pattern (struct PARSE *p) {
	struct NODE *np;

	/* pattern : word */
	showrule (p, "pattern");

	np = rule_word (p);
	p->level--;
	return np;
}

/*
 * Eat terminators
 */
void rule_term (struct PARSE *p) {
	showrule (p, "term");

	/* term : t_semicolon */
	/* term : t_ret */

	while ((p->lookahead == t_ret) || (p->lookahead == t_semicolon)) {
	    sh_match (p, p->lookahead);
	}

	p->level--;
}


#if SHELL_DEBUG

/*----------*/
void _showrule (struct PARSE *p, char *s) {
	int	i;
	if (p->rules) {
	    for (i=0; i <= p->level; i++) err_printf (" ");
	    err_printf ("rule_%s\n", s);
	}
	p->level++;
}
#endif

/*============================================================================*/
/* execution engine starts here */

#if SHELL_DEBUG
#define showexec(x,y) _showexec(x,y)
#else
#define showexec(x,y)
#endif

/*----------*/
int exec_command_sequence (struct PARSE *p, struct NODE *np) {
	struct NODE *kid;
	int	i;
	int	status;

	showexec (p, np);
	status = msg_success;

	for (i=0; (status == msg_success)  &&  (i<np->nkids); i++) {
	    kid = np->kids [i];
	    inherit (p, np, kid);
	    status = (*nlist [kid->id].exec) (p, kid);
	}

	p->level--;
	return status;
}
/*----------*/
int exec_command_list (struct PARSE *p, struct NODE *np) {
	struct NODE	*kid;
	struct FILE	*fout;
	struct PCB	*pcb;
	char		bgname [32];
        int		i;
	int		status;
	int		second_status;
	int		argc;
	char		**argv;
	int		priority;
	int		who;

	showexec (p, np);
	pcb = getpcb ();

	status = msg_success;
	i = 0;
	while ((status == msg_success)  && (i <np->nkids)) {
	    kid = np->kids [i++];
	    inherit (p, np, kid);
	    kid->header = np;

	    switch ((int) np->kids [i++]) {

	    /* don't wait for completion, spawn off another shell */
	    case t_ampersand:

		/* write out the parse tree that will become the commands
		 * to the background shell.
		 */	 


#if SECURE
                if ( isatty(pcb->pcb$a_stdin) && secure )
		{
                     err_printf(msg_cnsl_secure);
                     return msg_cnsl_secure;
		}
#endif

		krn$_unique ("bg_", bgname);
		fout = fopen (bgname, "w");
		kid->tstop--;	/* don't print out ampersand */
		node_render (p, kid, fout, 1);
		kid->tstop++;
		fputc ('\n', fout);
		fclose (fout);

		/*
		 * Background priority is one less than current priority,
		 * as long as we don't underflow into the idle process' 
		 * priority.
		 */
		priority = pcb->pcb$l_pri - 1;
		if (priority < 1) priority = 1;
		who = pcb->pcb$l_affinity;

		/*
		 * Create a new shell.  Give a -d qualifier so that it
		 * deletes the scratch input file once its done (this shell
		 * may not be around to delete it). Environment variables 
		 * have already been translated by this shell, so we tell the
		 * background shell to not handle $xxxx.
		 */


		sh_makesubshell (p, &argc, &argv);
		p->lastbgpid = krn$_create (
		    sh_routine,			/* procedure		*/
		    shell_startup,		/* startup routine	*/
		    0,				/* completion semaphore	*/
		    priority,			/* priority		*/
		    who, 			/* affinity		*/
		    shell_stack,		/* size			*/
		    "sh_bg",			/* process name		*/
		    bgname, "r",		/* stdin		*/
		    0, 0,			/* stdout		*/
		    0, 0,			/* parent's stderr	*/
		    argc, argv
		);
		break;

	    /* invoke the children synchronously */
	    case t_semicolon:
	    case t_ret:
		status = (*nlist [kid->id].exec) (p, kid);
		second_status = exec_wait_kids (p, np);
		if (second_status != msg_success) {
		    status = second_status;
		}
		break;

	    default:
		err_printf ("command list default rule taken\n");
		status = msg_failure;
	    }
	}
	
	p->level--;
	return status;
}
/*
 * Wait for all the children to complete that are on a queue of active
 * children.  Once complete, we remove them from the active list and release
 * their completion semaphores.  The kids are added on to the list
 * in the order that they were started, so that the status returned is
 * the status of the last kid.
 *
 * When we are woken up from waiting on a completion semaphore, if our
 * killpending bit is set, we have to broadcast that to all remaining children.
 *
 * If we're not talking to a tty type device, then the port block pointer is
 * null.
 */
int exec_wait_kids (struct PARSE *p, struct NODE *np) {
	volatile struct NODE	*kid;
	struct PCB	*pcb;

	pcb = getpcb ();
	kid = np->flink;
	while (kid != &np->flink) {

	    /*
	     * If any of our killpending bits are set, we broadcast
	     * the signal to all remaining children.
	     */
	    if (pcb->pcb$l_killpending) {
		exec_broadcast (p, np, SIGNAL_KILL | SIGNAL_CONTROLC);
	    }

	    /* Wait for the completion semaphore to be posted and then
	     * release the semaphore
	     */
	    p->laststatus = krn$_wait (&kid->done);
	    krn$_semrelease (&kid->done);

	    /* Advance to the next process on the list */
	    kid = kid->flink;
	    remq (kid->blink);
	}

	/*
	 * Return an error code if we've been aborted.
	 */
	if (pcb->pcb$l_killpending) {
	    return msg_ctrlc;
	} else {
	    return msg_success;
	}
}
/*
 * Set the signal mask in all executing children of this node.
 */
void exec_broadcast (struct PARSE *p, struct NODE *np, int sigmask) {
	struct NODE	*kid;
	struct PCB	*pcb;

	kid = np->flink;
	while (kid != &np->flink) {

	    /* if the PID is still active, set the killpending bit. */
	    pcb = krn$_findpcb (kid->pid);
	    if (pcb) {
		pcb->pcb$l_killpending |= sigmask;
		spinunlock (&spl_kernel);
	    }

	    /* advance to the next process */
	    kid = kid->flink;
	}
}

/*----------*/
int exec_andor (struct PARSE *p, struct NODE *np) {
	struct NODE	*kid;
        int		i;
	int	      	status;

	showexec (p, np);

	i = 0;
	while (i < np->nkids) {
	    kid = np->kids [i++];
	    inherit (p, np, kid);
	    if (status = (*nlist [kid->id].exec) (p, kid)) break;

	    switch ((int) np->kids [i++]) {

	    /* execute following statements only if previous was true
	     * (note shell's inversion of true and false)
	     */
	    case t_and:
		if (p->laststatus) {
		    p->level--;
		    return status;
		}
		break;

	    /* execute following statements only if previous was false
	     * (note shell's inversion of true and false)
	     */
	    case t_or:
		if (!p->laststatus) {
		    p->level--;
		    return status;
		}
		break;
		
	    /* no conditional, all done */
	    case t_null:
		p->level--;
		return status;
		break;

	    default:
		err_printf ("andor default rule taken\n");
		status = msg_failure;
	    }
	}

	return status;

}
/*
 * Every pattern is a list of regular expressions that are applied to the 
 * word.
 */
int exec_case (struct PARSE *p, struct NODE *np) {
	struct NODE *kid;
	int	status;
        struct REGEXP	*re;
	int	argc;
	char	**argv;
	int	pc;	/* number of patterns in a case	*/
	char	**pv;	/* list of patterns		*/
	int	i, px;
	int	found;
	
	showexec (p, np);

	exec_word (p, np->kids [0], &argc, &argv, SUB_PARSUB | SUB_CMDSUB);

	i = 1;
	status = msg_success;
	re = 0;
	found = 0;
	while (i < np->nkids) {

	    /*
	     * Expand the pattern list
	     */
	    exec_word (p, np->kids [i], &pc, &pv, SUB_DEFAULTS);
	    found = 0;
	    for (px=0; !found && px<pc; px++) {
		re = regexp_shell (pv [px], 0);
		if (re == 0) {
		    err_printf ("Poorly formed pattern in case statement");
		    return msg_failure;
		}

		found = regexp_match (re, argv [0]);
		regexp_release (re); re = 0;
	    }
	    free_argcv (pc, pv);
	    if (found) break;

	    i += 2;
	}

	if (found) {
	    kid = np->kids [i+1];
	    inherit (p, np, kid);
	    status = (*nlist [kid->id].exec) (p, kid);
	}

	free_argcv (argc, argv);
	p->level--;
	return status;
}
/*----------*/
int exec_for (struct PARSE *p, struct NODE *np) {
	int		i;
	int		status;
	struct NODE	*kid;
        int		start [2], stop [2];
	int		argc;
	char		**argv;

	showexec (p, np);

	/*
	 * Build the word list
	 */
	exec_word (p, np->kids [1], &argc, &argv, SUB_DEFAULTS | SUB_EXPAND);

	/*
	 * Iterate over the word list
	 */
	status = msg_success;
	kid = np->kids [2];
	for (i=0; i < argc && !p->request_exit && !p->breakc; i++) {
	    inherit (p, np, kid);
	    ev_write (np->kids [0], argv [i], EV$K_STRING | EV$K_USER);
	    if (status = (*nlist [kid->id].exec) (p, kid)) break;
	}
	if (p->breakc) p->breakc--;

	free_argcv (argc, argv);
	p->level--;
	return status;
}
/*----------*/
int exec_if (struct PARSE *p, struct NODE *np) {
	struct NODE *boolean, *truepart, *falsepart;
	int	status;

	showexec (p, np);

	boolean = np->kids [0];
	truepart = np->kids [1];
	falsepart = np->kids [2];

	inherit (p, np, boolean);
	if (status = (*nlist [boolean->id].exec) (p, boolean)) return status;

	if (p->laststatus == 0) {
	    inherit (p, np, truepart);
	    status = (*nlist [truepart->id].exec) (p, truepart);
	} else {
	    inherit (p, np, falsepart);
	    status = (*nlist [falsepart->id].exec) (p, falsepart);
	}

	p->level--;
	return status;
}

/*----------*/
int exec_null (struct PARSE *p, struct NODE *np) {
	showexec (p, np);
	p->level--;
	return msg_success; /* always returns true */
}
/*----------*/
int exec_pipe (struct PARSE *p, struct NODE *np) {
	struct NODE *kid, *left, *right;
	int	i;
	int	status;

	showexec (p, np);

	/*
	 * Default inheritence
	 */
	status = msg_success;
	for (i=0; i<np->nkids; i++) {
	    right = np->kids [i];
	    right->misc = 0;
	    inherit (p, np, np->kids [i]);
	}
	
	/*
	 * Overlay inherited stdin, stdout with pipe names
	 */
	for (i=0; (status == msg_success)  &&  i < (np->nkids-1); i++) {

	    /* set up pipe writer */
	    left = np->kids [i];
	    strcpy (left->stdout_mode, "w");
	    left->stdout_name = realloc (left->stdout_name, 32);
	    krn$_unique ("pipe", left->stdout_name);

	    /* set up pipe reader */
	    right = np->kids [i+1];
	    strcpy (right->stdin_mode, "r");
	    right->stdin_name = realloc (right->stdin_name, 32);
	    strcpy (right->stdin_name, left->stdout_name);

	    /*
	     * create the pipe so that the ref count is never 0 until
	     * both the reader and the writer have opened it
	     */
	    if (create_noopen (right->stdin_name, pi_ddb_ptr, 0, 0) != msg_success) {
		err_printf ("can't create a pipe");
		status = msg_failure;
	    }
	    if ((right->misc = fopen (right->stdin_name, "w")) == 0) {
		err_printf ("Can't open pipe %s\n", right->stdin_name);
		status = msg_failure;
	    }
	}

	pipe_flag = 1;			/* indicate piped operation */

	/*
	 * Invoke all the children
	 */
	for (i = 0; i<np->nkids; i++) {
	    kid = np->kids [i];
	    status = (*nlist [kid->id].exec) (p, kid);
	    if (status != msg_success) {
		if (i>0) {
		    right = np->kids [i];
		    if (right->misc) {
			fclose (right->misc);
			right->misc = fopen (right->stdin_name, "r");
		    }
		}
		break;
	    }
	}

	/*
	 * Close out all the open pipes.  All the children have started up
	 * by now.
	 */
	for (i=0; i < (np->nkids-1); i++) {
	    right = np->kids [i+1];
	    if (right->misc) fclose (right->misc);
	}	

	pipe_flag = 0;			/* clear piped operation */
	p->level--;
	return status;
}

/*----------*/
int exec_simple (struct PARSE *p, struct NODE *np) {
	struct NODE	*kid, wp;
	int		i;
	int		status;
	int		argc;
	char		**argv;
	char		*cmdname;
	int		(*routine) ();
	int		who;
	char 		*stdin_name,  *stdin_mode;
	char 		*stdout_name, *stdout_mode;
	char 		*stderr_name, *stderr_mode;
	struct PCB	*pcb, *kidpcb;
	struct FILE	*fin;
	struct INODE	*ip;
	char		*firsts [8];
	int		stacksize;
	struct cmd_s_table *ct;

	showexec (p, np);

	/*
	 * Default the standard i/o
	 */
	stdin_name = np->stdin_name;
	stdin_mode = np->stdin_mode;

	stdout_name = np->stdout_name;
	stdout_mode = np->stdout_mode;

	stderr_name = np->stderr_name;
	stderr_mode = np->stderr_mode;

	/*
	 * Perform all the substitutions on the word lists
	 * This happens after the i/o defaults are applied so that
	 * processes within a loop can redirect their output and yet still
	 * have the entire loop piped into something.
	 */
	argc = 0;
	argv = 0;
	for (i=0; i<8; i++) {
	    free_argcv (argc, argv); argc = 0; argv = 0;
	    firsts [i] = 0;
	    if (np->kids [i] == 0) continue;
	    exec_word (p, np->kids [i], &argc, &argv, SUB_DEFAULTS | SUB_EXPAND);
	    firsts [i] = mk_dynamic (argv [0]);
	}

	if (!p->request_exit) {
	    /* stdin redefined with t_in */
	    if (np->kids [0]) {
		stdin_name = firsts [0];
	    }

	    /* stdout redefined with t_out */
	    if (np->kids [2]) {
		stdout_name = firsts [2];
		stdout_mode = "w";
	    }

	    /* stdout redefined with t_out_append */
	    if (np->kids [3]) {
		stdout_name = firsts [3];
		stdout_mode = "a";
	    }

	    /* stderr redefined with t_err */
	    if (np->kids [4]) {
		stderr_name = firsts [4];
		stderr_mode = "w";
	    }

	    /* stderr redefined with t_err_append */
	    if (np->kids [5]) {
		stderr_name = firsts [5];
		stderr_mode = "a";
	    }

	    /*
	     *  show the command about to be be executed
	     */
	    if (p->echo) {
		for (i=0; i<argc; i++) {
		    err_printf ("%s ", argv [i]);
		}
		err_printf ("<%s >%s 2>&%s\n",
		    stdin_name, stdout_name, stderr_name
		);
	    }

	    pcb = getpcb ();

#if IMP
	    if (nothing_but_cpu) {
		if ( ( strcmp((argv[0]) , "cpu" ) ) )
		    return msg_shell_nocmd;
	    }
#endif

	    if (*(argv[0]) == 0)
		return msg_shell_nocmd;

#if MODULAR
	    ct = cmd_table_lookup(argc, argv, 1);
	    if (ct == 1)
		return msg_success;		/* help given, just return */
	    if (ct) {
		cmdname = ct->ct_string;
		routine = ct->ct_routine;
		stacksize = ct->ct_misc * 1024;
#if SECURE
        	if ( (isatty(pcb->pcb$a_stdin)) && secure && 
		    (!(ct->ct_flags & parse$m_secure)) ) {
		    err_printf(msg_cnsl_secure);
		    return msg_cnsl_secure;
		}
#endif
	    } else {
#endif
	    /*
	     * Look up argv [0] in the file system.
	     */
       
	    cmdname = argv[0];

	    if ((fin = fopen (argv [0], "rs")) == NULL) {
#if !IMP
		err_printf ("%s: No such command\n", argv [0]);
#endif
		free_ptrs (sizeof (firsts) / sizeof (char *), firsts);	
		p->level--;
		return msg_shell_nocmd;
	    }

	    ip = fin->ip;

	    /*
	     * found the command - now see if we can run it in this state
	     * if not, close the file in ptr and return.
	     */
#if SECURE
            if ( (isatty(pcb->pcb$a_stdin)) && secure && 
                                      ( !(ip->attr & ATTR$M_SECURE) ) )  
            {
		fclose (fin);
        	err_printf(msg_cnsl_secure);
        	return msg_cnsl_secure;
	    }
#endif

	    /* File is a binary */
	    if ((ip->attr & ATTR$M_EXECUTE) &&
		(ip->attr & ATTR$M_BINARY)) {
		routine = (void *) ip->loc;
		stacksize = ip->misc;

	    /* File is a script.  Rename stdin to the file name */
	    } else {
		if ((ip->attr & ATTR$M_EXECUTE) == 0) {
		    err_printf ("%s is not executable\n", argv [0]);
		    free_ptrs (sizeof (firsts) / sizeof (char *), firsts);	
	    	    p->level--;
		    fclose (fin);
		    return msg_shell_noex;
		}
		routine = (void *) sh_routine;
		stdin_name = argv [0];
		stdin_mode = "r";
		stacksize = shell_stack;
    /* ajbfix: inherit current shell qualifiers */
	    }
	    fclose (fin);
#if MODULAR
	    }
#endif
	    /*
	     * Register this process with the list of active processes
	     * Set up a completion semaphore.
	     */
	    insq (np, np->header->blink);
	    krn$_seminit (&np->done, 0, "child_done");

	    /*
	     * affinity mask
	     */
	    if (np->kids [6]) {
		who = 1 << atoi (firsts [6]);
	    } else {
		who = pcb->pcb$l_affinity;
	    }

	    np->pid =
	    p->lastpid =
	    krn$_create (
		routine,		/* address of process		*/
		shell_startup,	/* user defined startup		*/
		&np->done,		/* completion semaphore		*/
		pcb->pcb$l_pri,	/* process priority		*/
		who,		/* cpu affinity mask		*/
		stacksize,		/* default stack size		*/
		cmdname,		/* process name			*/
		stdin_name,		/* stdin			*/
		stdin_mode,
		stdout_name,	/* stdout			*/
		stdout_mode,
		stderr_name,	/* stderr			*/
		stderr_mode,
		argc,		/* process specific arguments	*/
		argv
	    );

	    /*
	     * Register this process with the control C handler.
	     */
	    sh_register (p, np->pid);
	}

	free_ptrs (sizeof (firsts) / sizeof (char *), firsts);	
	p->level--;
	return msg_success;
}
/*----------*/
int exec_loop (struct PARSE *p, struct NODE *np) {
	struct NODE	*boolean, *body;
	int		status;

	showexec (p, np);

	boolean = np->kids [1];
	body = np->kids [2];

	status = msg_success;
	while (!status && !p->request_exit  && !p->breakc) {
	    inherit (p, np, boolean);
	    if (status = (*nlist [boolean->id].exec) (p, boolean)) break;

	    if ((np->kids [0] == t_while)  && p->laststatus) break;
	    if ((np->kids [0] == t_until) && !p->laststatus) break;

	    inherit (p, np, body);
	    status = (*nlist [body->id].exec) (p, body);
	}
	if (p->breakc) p->breakc--;

	p->level--;
	return status;
}
/*
 * Perform various substitutions on a word list. As a result of the
 * substitutions, the word list can grow.  This routine can return a fairly
 * long list of values depending on what substitutions happened.  In the
 * following order, it:
 *
 *		1) performs parameter substitution 
 *			$ digits
 *			$ characters
 *			$ { characters }
 *		2) performs command substitution
 *		3) blank interpretation
 *		4) filename generation (i.e. expansion of wildcards)
 *		5) quote removal and backslash interpretation
 *
 * While performing these operations it maintains an argc/argv array.  Always
 * insure that the argv array is terminated by a null pointer.
 */
int exec_word (struct PARSE *p, struct NODE *np, int *argc, int *argv, int submask) {
	int	i, j;
	int	pingc, pongc;
	char	**pingv, **pongv;
	int	temp;
	int	mask;
	int	sh_parsub ();
	int	sh_cmdsub ();
	int	sh_blank ();
	int	sh_genfiles ();
	int	sh_quote ();
	static  (*sublist []) () = {
		sh_parsub,
		sh_cmdsub,
		sh_blank,
		sh_genfiles,
		sh_quote
	};


	showexec (p, np);

	pingv = malloc ((max_shell_words + 1) * sizeof (char *));
	pongv = malloc ((max_shell_words + 1) * sizeof (char *));

	/*
	 * Copy the words from the wordlist into a copy local to the node.
	 * The original wordlist has to be preserved as
	 * it may be used multiple times.
	 */
	pongc = 0;
	pingc = np->nkids;
	for (i=0; i<np->nkids; i++) {
	    pingv [i] = mk_dynamic (np->kids [i]);
	}

	/*
	 * perform all the substitutions requested.  The submask indicates
	 * substitutions to perform
	 */
	mask = 1;
	for (i=0; i<sizeof (sublist) / sizeof (sublist [0]); i++) {
	    if (submask & mask) {
		(*sublist [i]) (p, &pingc, pingv, &pongc, pongv, submask);
		free_ptrs (pingc, pingv);
		pingc = pongc;
		temp = pingv;
		pingv = pongv;
		pongv = temp;
	    }
	    mask <<= 1;
#if !C57_COMPILER
#if SHELL_DEBUG
            if (submask & SUB_RENDER) {
                show_expanded_args(i, pingc, pingv)
	    }
#endif
#endif

#if C57_COMPILER
#if (SHELL_DEBUG || C57_COMPILER)
	    if (submask & SUB_RENDER) {
		pprintf ("%d:", i);
		for (j=0; j<pingc; j++) {
		    pprintf (" /%s/", pingv [j]);
		}
		pprintf ("\n");
	    }
#endif
#endif
	}
	free_argcv (pongc, pongv);

	*argc = pingc;
	*argv = pingv;

	/*
	 * Terminate the array with a null pointer
	 */
	pingv [pingc] = 0;

	p->level--;
	return msg_success;
}

#if SHELL_DEBUG
/*----------*/
void _showexec (struct PARSE *p, struct NODE *np) {
	int	i;

	if (p->show_tree) {
	    for (i=0; i<p->level; i++) err_printf (" ");
	    err_printf ("exec_%s", nlist [np->id].name);

	    for (i=0; i<np->nkids; i++) {
		err_printf (" %08X", (int) np->kids [i]);
	    }
	    err_printf ("\n");
	}
	p->level++;
}
#endif


/*
 * Let a child inherit the context from a parent.
 */
void inherit (struct PARSE *p, struct NODE *parent, struct NODE *kid) {

	kid->header = parent->header;

	/*
	 * Free the pointers to the names if already allocated
	 */
	if (kid->stdin_name) free (kid->stdin_name);
	if (kid->stdout_name) free (kid->stdout_name);
	if (kid->stderr_name) free (kid->stderr_name);

	/*
	 * inherit the names
	 */
	kid->stdin_name = mk_dynamic (parent->stdin_name);
	kid->stdout_name = mk_dynamic (parent->stdout_name);
	kid->stderr_name = mk_dynamic (parent->stderr_name);

	/*
	 * inherit the access modes
	 */
	strcpy (kid->stdin_mode, parent->stdin_mode);
	strcpy (kid->stdout_mode, parent->stdout_mode);
	strcpy (kid->stderr_mode, parent->stderr_mode);
}

#if 0
struct ARGCVDYN {
	int	slots;		/* number of pointers currently allocated*/
	int	maxslots;	/* max number of pointers allowed	*/
	int	argc;		/* number of strings defined		*/
	char	**argv;		/* pointer to array of pointers		*/
};

int argcv_init (int initslots, int maxslots) {
	struct ARGCVDYN *ap;

	ap = malloc (sizeof (struct ARGCVDYN));
	ap->argc = 0;
        ap->maxslots = maxslots;
	ap->slots = initslots;
	ap->argv = malloc (sizeof (char *) * ap->slots);
	return ap;
}

void argcv_freeall (struct ARGCVDYN *ap) {
	argcv_freeptrs (ap);
	free (ap->argv);
	free (ap);
}

void argcv_freeptrs (struct ARGCVDYN *ap) {
	int	i;

	for (i=0; i<ap->argc; i++) {
	    if (ap->argv [i]) free (ap->argv [i]);
	}
}

/*
 * Install a string into the array. Assume that the string to be installed is
 * allocated from the heap, and we therfore can simply point to it.
 */
int argcv_install (struct ARGCVDYN *ap, char *s) {

	/*
	 * Make some room in the list
	 */
	if (ap->argc >= ap->slots) {
	    if (ap->slots >= ap->maxslots) {
		return msg_extra_params;
	    }

	    /*
	     * double the number of slots, not to exceed the maximum allowed.
	     */
	    ap->slots *= 2;
	    ap->slots = min (ap->slots, ap->maxslots);
	    ap->argv = realloc (ap->argv, sizeof (char *) * ap->slots);
	}

	ap->argv [ap->argc++] = s;
	return msg_success;
}
#endif

/*
 * Substitute shell parameters in a string.  Constructs supported are:
 *
 *	$*		all shell parameters
 *	$#		number of shell parameters in decimal
 *	$!		pid of the last background process
 *	$$		pid of this shell
 *	$p		pid of parent
 *	$gp		pid of grandparent
 *	$?		status of the last executed command
 *	$digits		a shell parameter
 *	$id		shell variable
 *	${...}		any of the above constructs may use braces as
 *			delimiters
 *
 * Parameter substitution does not occur inside single quotes.
 *
 * Since in general it is not known how big the output string will be, this
 * routine mallocs and reallocs memory as needed to handle the result.  It
 * returns the address of the memory.
 */
char *sh_par_xlate (struct PARSE *p, char *string) {
	struct STRDYN	xd;
	int	i;
	int	pid;
	struct PCB	*pcb;
	struct EVNODE	*evp, evnode;
	int		evstat;

	str_dyninit (&xd, 128); /* make this big enough to hold the easy stuff */

	/* all shell parameters */
     	if (strcmp (string, "*") == 0) {
	    for (i=1; i<p->argc; i++) {
		str_dyncat (&xd, p->argv [i]);
		if (i != (p->argc-1)) {
		    str_dyncat (&xd, " ");
		}
	    }

     	/* number of shell parameters */
	} else if (strcmp (string, "#") == 0) {
	    sprintf (xd.ptr, "%d", p->argc-1);
	    
	/* last background pid */
	} else if (strcmp (string, "!") == 0) {
	    sprintf (xd.ptr, "%08X", p->lastbgpid);

	/* pid of this shell */
	} else if (strcmp (string, "$") == 0) {
	    pcb = getpcb ();
	    pid = pcb->pcb$l_pid;
     	    sprintf (xd.ptr, "%08X", pid);

	/* pid of this shell's parent */
	} else if (strcmp (string, "p") == 0) {
	    pcb = getpcb ();
	    pid = pcb->pcb$l_parentpid;
	    sprintf (xd.ptr, "%08X", pid);


	/* pid of this shell's grandparent.  If we can't find the grandparent
	 * then we use the parent.
         */
	} else if (strcmp (string, "gp") == 0) {
	    pcb = getpcb ();
	    pid = pcb->pcb$l_parentpid;
	    if (pcb = krn$_findpcb (pid)) {
		pid = pcb->pcb$l_parentpid;
		spinunlock (&spl_kernel);
	    }
	    sprintf (xd.ptr, "%08X", pid);
	

	/* status of last executed command */
	} else if (strcmp (string, "?") == 0) {
	    sprintf (xd.ptr, "%08X", p->laststatus);

     	/* particular shell parameter */
	} else if (isdigit (string [0])) {
	    i = atoi (string);
	    if (i < p->argc) {
		str_dyncat (&xd, p->argv [i]);
	    }

	/* environment variable */
	} else {
	    evp = &evnode;
	    evstat = ev_read (string, &evp, EV$K_SYSTEM);
	    if (evstat == msg_success) {
     		if ((evp->attributes & EV$M_TYPE) == EV$K_INTEGER) {
		    sprintf (xd.ptr, "%d", evp->value.integer);
		} else if ((evp->attributes & EV$M_TYPE) == EV$K_STRING) {
		    strcpy (xd.ptr, evp->value.string);
		}
	    }
	}

	return xd.ptr;
}
/*+
 * ============================================================================
 * = sh_parsub - Perform parameter substitution on a word list.               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Perform parameter substitution on a word list.  Text inside single and
 * back quotes is not looked at.  
 *
 * FORM OF CALL:
 *  
 *	sh_parsub ( p, pingc, pingv, pongc, pongv, submask )
 *  
 * RETURN CODES:
 *
 *	msg_success - normal completion
 *       
 * ARGUMENTS:
 *
 *	struct PARSE *p	- pointer to parse structure for this shell
 *	int pingc	- number of strings in source wordlist
 *	char **pingv	- array of pointers to source strings
 *	int pongc	- number of strings in destination wordlist
 *	char **pongv	- array of pointers to strings for destination
 *	int submask	- control flags
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int sh_parsub (struct PARSE *p, int *pingc, char **pingv,  int *pongc, char **pongv, int submask) {
	int	argix;
	char	term;
	char	*s;
	char	*ps, *parstring;
	char	*xlate;
	struct STRDYN	os;	/* output string */
	char	simple [2];
	int	within_double;


	*pongc = 0;
	simple [0] = simple [1] = 0;

	/* walk down the argument list */
	for (argix=0; argix < *pingc; argix++) {
	    s = pingv [argix];
	    if (*pongc >= max_shell_words) break;

	    /*
	     * no substitutions to perform if no $ can be found
	     */
	    if (!strchr (s, '$')) {
		pongv [*pongc] = pingv [argix];
		pingv [argix] = 0;
		*pongc += 1;
		continue;
	    }

	    /* set up a dynamic string descriptor */
	    str_dyninit (&os, 256);

	    /* walk down the word */
	    within_double = 0;
	  
	    while (*s) {
		switch (term = *s) {

		case BACKSLASH:
		    simple [0] = *s++;
		    str_dyncat (&os, simple);
		    if (*s) {	/* don't eat nulls */
			simple [0] = *s++;
			str_dyncat (&os, simple);
		    }
		    break;

	        /*
	         * nothing happens inside single/back quotes, so advance
		 * to the next single/back quote.
	         */
		case BACKQUOTE:
	        case SINGLEQUOTE:
		    if (within_double) {
			simple [0] = *s++;
			str_dyncat (&os, simple);
		    } else {
			simple [0] = *s++;
			str_dyncat (&os, simple);
			do {
			    simple [0] = *s;
			    str_dyncat (&os, simple);
			} while (*s && (*s++ != term));
		    }
		    break;

	        case DOUBLEQUOTE:
		    within_double ^= 1;
		    simple [0] = *s++;
		    str_dyncat (&os, simple);
		    break;
	
		case '$':
		    parstring = malloc (strlen (s) + 1);
		    ps = parstring;
		    s++;

		    /* eat until right brace */
		    if (*s == '{') {
			s++;
			while (*s && (*s != '}')) *ps++ = *s++;
			s++;
		    } else {
			if (isalpha (*s)) {
			    while (*s && strchr (idset, *s)) *ps++ = *s++;
			} else if (isdigit (*s)) {
			    while (*s && isdigit (*s)) *ps++ = *s++;
			} else if ((*s == NULL)
				|| (*s == SINGLEQUOTE) || (*s == BACKQUOTE)
				|| (*s == DOUBLEQUOTE) || (*s == BACKSLASH)) {
			    simple [0] = term;
			    str_dyncat (&os, simple);
			} else {
			    *ps++ = *s++;
			}
		    }
		    *ps = 0;

		    /* translate field, and insure that target string has enough
		     * room for the translation
		     */
		    if (*parstring) {
			xlate = sh_par_xlate (p, parstring);
			str_dyncat (&os, xlate);
			free (xlate);
		    }
		    free (parstring);
		    break;

		default:
		    simple [0] = *s++;
		    str_dyncat (&os, simple);
		    break;
		}
	    }

	    /*
	     * Install the dynamic string into the results word list
	     */
	    if (strlen (os.ptr)) {
		pongv [*pongc] = os.ptr;
		*pongc += 1;
	    }
	}

	return msg_success;
}
/*+
 * ============================================================================
 * = sh_cmdsub - Perform command substitution on a word list.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Only text within backquotes is eligible for command substitution.  Backquotes
 * are reconginzed by themselves and within double quotes.
 *
 * FORM OF CALL:
 *  
 *	sh_cmdsub ( p, pingc, pingv, pongc, pongv, int submask )
 *  
 * RETURN CODES:
 *
 *	msg_success - normal completion
 *       
 * ARGUMENTS:
 *
 *	struct PARSE *p	- pointer to parse structure for this shell
 *	int pingc	- number of strings in source wordlist
 *	char **pingv	- array of pointers to source strings
 *	int pongc	- number of strings in destination wordlist
 *	char **pongv	- array of pointers to strings for destination
 *	int submask	- control flags
 *            
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int sh_cmdsub (struct PARSE *p, int *pingc, char **pingv, int *pongc, char **pongv, int submask) {
	struct FILE *fin, *fout;
	struct SEMAPHORE cmd_done;
	struct PCB 	*pcb;
	char	output_name [MAX_NAME];
	char	input_name [MAX_NAME];
	char	*s;
	struct STRDYN	os;
	char	simple [2];
	char	term;
	int	status;
	int	argc;
	char	**argv;
	int	argix;
	int	pid;
	int	who;
	int	c;

	*pongc = 0;
	simple [0] = simple [1] = 0;
	pcb = getpcb ();

	/* walk the word list */
	for (argix=0; argix < *pingc; argix++) {
	    s = pingv [argix];
	    if (*pongc >= max_shell_words) break;

	    /*
	     * no substitutions to perform if no backquotes can be found
	     */
	    if (!strchr (s, '`')) {
		pongv [*pongc] = pingv [argix];
		pingv [argix] = 0;
		*pongc += 1;
		continue;
	    }

	    /* set up a dynamic string descriptor */
	    str_dyninit (&os, 2048);

	    /* the string has potential command substitution (the backquotes
	     * detected may be protected by single quotes).
	     */
	    while (*s) {
		switch (*s) {

		case BACKSLASH:
		    simple [0] = *s++;
		    str_dyncat (&os, simple);
		    if (*s) {	/* don't eat nulls */
		        simple [0] = *s++;
		        str_dyncat (&os, simple);
		    }
		    break;

	        /*
	         * nothing happens inside single/double quotes, so advance
		 * to the next quote. (ajbfix:  this is wrong!)
	         */
	        case SINGLEQUOTE:
	        case DOUBLEQUOTE:
		    term = *s;
		    simple [0] = *s++;
		    str_dyncat (&os, simple);
		    do {
			simple [0] = *s;
			str_dyncat (&os, simple);
		    } while (*s && (*s++ != term));
		    break;
		
		case BACKQUOTE:

#if SECURE
		    /* if secure and foreground mode, do not execute */
                    if ( isatty(pcb->pcb$a_stdin) && secure )
                    {
                         err_printf(msg_cnsl_secure);
                         return msg_cnsl_secure;
   	            }
#endif

		    krn$_unique ("sh_in_", input_name);
		    krn$_unique ("sh_out_", output_name);

		    /* capture the backquoted string */
		    fout = fopen (input_name, "w");
		    if (fout == NULL) {
			err_printf ("shell can't open %s\n", input_name);
		    }
		    s++;

		    /* while capturing the backquoted string, we have to
		     * allow for quoted stuff.
		     */
		    while (*s && (*s != BACKQUOTE)) {
			if (*s == BACKSLASH) s++;
			fputc (*s++, fout);
		    }
		    fputc ('\n', fout);
		    s++; /* skip over backquote */
		    fclose (fout);

		    pcb = getpcb ();
		    who = pcb->pcb$l_affinity;

		    /* execute the shell command */
		    krn$_seminit (&cmd_done, 0, "cmd_sub");
		    sh_makesubshell (p, &argc, &argv);
		    pid = krn$_create (
			sh_routine,		/* procedure		*/
			shell_startup,		/* startup routine	*/
			&cmd_done,		/* completion semaphore	*/
			pcb->pcb$l_pri,		/* priority		*/
			who,			/* affinity		*/
			shell_stack,		/* size			*/
			"sh_cmdsub",		/* process name		*/
			input_name, "r",	/* stdin		*/
			output_name, "a",	/* stdout		*/
			0, 0,			/* parent's stderr	*/
			argc, argv
		    );
		    sh_register (p, pid);
		    status = krn$_wait (&cmd_done);
		    krn$_semrelease (&cmd_done);

		    /* Copy the results into the string */
		    fin = fopen (output_name, "r");
		    if (fin == NULL) {
			err_printf ("shell can't open output file %s from command substitution\n", output_name);
		    }
		    while ((c = fgetc (fin)) != EOF) {
			simple [0] = c;
			str_dyncat (&os, simple);
		    }
		    fclose (fin);

		    /* clean up temporary files */
		    remove (output_name);
		    break;

		default:
		    simple [0] = *s++;
		    str_dyncat (&os, simple);
		    break;
		}
	    }

	    /*
	     * Install the dynamic string into the results word list
	     */
	    if (strlen (os.ptr)) {
		pongv [*pongc] = os.ptr;
		*pongc += 1;
	    }
	}
	return msg_success;
}
/*
 * Create a command line for a subshell.  Set up the qualifiers such that the
 * standard input is deleted when the subshell exits.
 */
void sh_makesubshell (struct PARSE *p, int *argc, int *arglist) {
	char	**argv;

	*argc = 2;

	argv = malloc (3 * sizeof (char *));
	argv [0] = mk_dynamic ("shell");
	argv [1] = mk_dynamic ("-d");
	argv [2] = 0;

	*arglist = argv;
}

/*+
 * ============================================================================
 * = sh_blank - Perform blank interpretation on a word list.                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Perform blank interpretation on a word  list.  Text inside
 * single and double quotes is not looked at.  Escaped characters (characters
 * preceeded by backslash) are also not looked at.
 *  
 *
 * FORM OF CALL:
 *  
 *	sh_blank ( p, pingc, pingv, pongc, pongv, submask )
 *  
 * RETURN CODES:
 *
 *	msg_success - normal completion
 *       
 * ARGUMENTS:
 *
 *	struct PARSE *p	- pointer to parse structure for this shell
 *	int pingc	- number of strings in source wordlist
 *	char **pingv	- array of pointers to source strings
 *	int pongc	- number of strings in destination wordlist
 *	char **pongv	- array of pointers to strings for destination
 *	int submask	- control flags
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int sh_blank (struct PARSE *p, int *pingc, char **pingv, int *pongc, char **pongv, int submask) {
	int	i;
	int	argix;
	char	*s, *sbase, *cp;
	int	inword;
	char	term;

	*pongc = 0;

	/*
	 * Walk down each argument
	 */
	for (argix=0; argix < *pingc; argix++) {
	    s = pingv [argix];

	    /* simply copy empty strings */
	    if (*s == '\0') {
		pongv [*pongc] = pingv [argix];
		pingv [argix] = 0;
		*pongc += 1;
		continue;
	    }

	    while (*s) {

		/* skip leading white space */
		while (*s && isspace (*s)) s++;
		if (*s == 0) break;

		/* mark begining of word */
		sbase = s;
	    
		/* span the word */
		inword = 1;
		while (*s && inword) {
		    switch (term = *s++) {

	            /* ignore escaped characters */
	            case BACKSLASH:
		        s++;
		        break;

		    /*
		     * nothing happens inside single quotes, so advance
		     * to the next quote.
		     */
		    case SINGLEQUOTE:
			while (*s && (*s++ != term));
			break;

		    /*
		     * advance to the next double quote while looking
		     * for escaped quotes
		     */
		    case DOUBLEQUOTE:
			skip_doublequote (&s);
			break;

		    /* whitespace */
		    case ' ':
		    case '	':
		    case '\n':
			s--;
			inword = 0;
			break;
		    }
		}

		/* word spans entire argument */
		if ((sbase == pingv [argix]) && (*s == 0)) {
		    pongv [*pongc] = pingv [argix];
		    pingv [argix] = 0;
		    *pongc += 1;
 
		/* multiple words in argument */
		} else {
		    cp = pongv [*pongc] = malloc (s - sbase + 1);
		    *pongc += 1;
		    while (sbase < s) *cp++ = *sbase++;
		    *cp++ = 0;
		}

		if (*pongc >= max_shell_words) break;
	    }
	}

	return msg_success;
}
/*
 * Given a pointer to a double quoted string, find the end of that string.
 * In doing so, we have to skip over escaped quote marks.
 */
void skip_doublequote (char **string) {
	char *s;
	char c;

	s = *string;
	while (c = *s++) {
	    if (c == DOUBLEQUOTE) break;
	    if (c == BACKSLASH) {
		s++;
	    }
	}
	*string = s;
}
/*
 * Install a string into a list of pointers to strings. 
 *
 * 	msg_success		- string installed
 *	msg_extra_params	- argument list overflow
 */
int sh_genfiles_installed (char *s, int *argc, char **argv) {

	/*
	 * check for argument list overflow
	 */
	if (*argc >= max_shell_words) {
	    return msg_extra_params;
	}
	argv [*argc] = mk_dynamic (s);
	*argc += 1;
	return msg_success;
}

/*+
 * ============================================================================
 * = sh_genfiles - Generate filenames from a list of words.                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * given a list of words, expand those words into file names if they have the
 * appropriate shell metacharacters.
 *
 * There is a bug in this code in that it may inhibit expansion when it
 * shouldn't, as in the case of:
 *	 for i in examine * ;do echo $i ; done
 *
 * FORM OF CALL:
 *  
 *	sh_genfiles ( p, pingc, pingv, pongc, pongv, submask ) 
 *  
 * RETURN CODES:
 *
 *	msg_success - normal completion
 *       
 * ARGUMENTS:
 *
 *	struct PARSE *p		- pointer to parse structure for this shell
 *	int pingc		- number of strings in source wordlist
 *	char **pingv		- array of pointers to source strings
 *	int pongc		- number of strings in destination wordlist
 *	char **pongv		- array of pointers to strings for destination
 *	int submask		- control flags
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int sh_genfiles (struct PARSE *p, int *pingc, char **pingv, int *pongc, char **pongv, int submask)
 {
	int	i;
	int	argix;
	int	status;
	int	allow_expansion;

	*pongc = 0;
	allow_expansion = submask & SUB_EXPAND;

	/*
	 * Treat each word as a regular expression 
	 */
	for (argix=0; argix < *pingc; argix++) {

	if (argix != 0) {		/* prevent 1st arg from expansion */

	    if (allow_expansion &&  mayberegular (pingv [argix])) {
		status = fexpand (
			pingv [argix],
			0,	/* short form, names only */
			sh_genfiles_installed,
			pongc,
			pongv );
	    } else {
		status = msg_failure;
	    }
	}
	    /*
	     * check for overflow
	     */
	    if (*pongc >= max_shell_words) {
		return msg_extra_params;
	    }

	    /*
	     * If no expansion happened, we preserve the original string
	     */
	    if (status != msg_success) {
		pongv [*pongc] = pingv [argix];
		pingv [argix] = 0;
		*pongc += 1;
	    }

	    /*
	     * If this is an internal file, then check to see if further
	     * expansions are disabled.
	     */
	    if (argix == 0) {
		struct INODE *ip;
#if MODULAR
		struct cmd_s_table *ct;

		ct = cmd_table_expand(pongv);

		if (ct) {			/* found */
			if (!(ct->ct_flags & parse$m_expand))
				allow_expansion = 0;
		} else {
#endif
		ip = finode (pongv [0], 0);
		if (ip && !(ip->attr & ATTR$M_EXPAND))
		    allow_expansion = 0;
#if MODULAR
		}
#endif
	    }
	}
	return msg_success;
}
/*+
 * ============================================================================
 * = sh_quote - Remove quotes and escaped characters in a word list.	     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Remove quoted substrings markers and resolve quoted charcters in a string.
 *
 * FORM OF CALL:
 *  
 *	sh_quote ( p, pingc, pingv, pongc, pongv, submask )
 *  
 * RETURN CODES:
 *
 *	msg_success - normal completion
 *       
 * ARGUMENTS:
 *
 *	struct PARSE *p	- pointer to parse structure for this shell
 *	int pingc	- number of strings in source wordlist
 *	char **pingv	- array of pointers to source strings
 *	int pongc	- number of strings in destination wordlist
 *	char **pongv	- array of pointers to strings for destination
 *	int submask		- control flags
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int sh_quote (struct PARSE *p, int *pingc, char **pingv, int *pongc, char **pongv, int submask) {
	char	*op;
	char	*s;
	char	term, c;
	int	argix;

	*pongc = *pingc;

	for (argix=0; argix < *pingc; argix++) {
	    pongv [argix] = pingv [argix];
	    pingv [argix] = 0;

	    op = s = pongv [argix];
	    while (*s) {
		switch (term = *s++) {

		case BACKSLASH:
		    *op++ = *s++;
		    break;

		/*
	         * nothing happens inside single quotes, so advance to the next
	         * quote.
	         */
		case SINGLEQUOTE:
		    while (*s && ((*op++ = *s++) != term));
		    op--;
		    break;

		/*
		 * Backslashes are translated inside double quotes
		 */	    
		case DOUBLEQUOTE:
		    while (*s && ((c = *s++) != term)) {
			if (c == BACKSLASH) {
			    c = *s++;
			}
			*op++ = c;
		    }
		    break;

		default:
		    *op++ = term;
		    break;
		}
	    }
	    *op = 0;
	}

	return msg_success;
}

/*
 * Register a process with the current port block if the port block is defined.
 *
 */
void sh_register (struct PARSE *p, int pid) {
	struct PCB *pcb;
	extern int krn$_pcount;

	if (p->ttpb) {
	    pcb = krn$_findpcb (pid);
	    if (pcb) {
#if 0
#if SHELL_DEBUG
		if (queue_validate (
		    &p->ttpb->ctrlch,
		    0,
		    -1,
		    0,
		    krn$_pcount-1
		)) do_bpt (&p->ttpb->ctrlch);
#endif
#endif
		/* Verify that the process hasn't inserted itself on the queue
		 * (interactive shells do this) first.
		 */
		if (pcb->pcb$r_ctrlcq.flink == & pcb->pcb$r_ctrlcq.flink) {
		    insq (&pcb->pcb$r_ctrlcq, p->ttpb->ctrlch.blink);
		}
		spinunlock (&spl_kernel);
	    }
	}
}

/*
 * Return the address of the lexical analyzer's input buffer.  This routine
 * should run under a spinlock to avoid the buffer from going away.
 *
 * The input buffer IS NOT always equivalent to the command line, especially
 * in the case of newlines.
 */
char * get_raw_input_buffer ( struct PCB *pcb ) {
        struct PARSE *p;

	p = (struct PARSE *) pcb->pcb$l_shellp;
	return &p->yyctx.input_buf [0];
}

#if !C57_COMPILER
#if SHELL_DEBUG
void show_expanded_args(int i, int pingc, char **pingv)
{
    pprintf ("%d:", i);
    for (j=0; j<pingc; j++) {
        pprintf (" /%s/", pingv [j]);
    }
    pprintf ("\n");
}
#endif
#endif

