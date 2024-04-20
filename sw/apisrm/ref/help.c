/* file:	help.c
 *
 * Copyright (C) 1990,1995 by
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
 *      Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      A simple help program for the Common console.
 *	The model supported is similar to the man command in Ultrix.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson, Kevin Peterson
 *
 *  CREATION DATE:
 *  
 *	ajb	11-Dec-1991
 *
 *  MODIFICATION HISTORY:
 *
 *	jrk	26-May-1995	Modular console
 *	jrk	23-Sep-1993	Overlays
 *	ajb	11-Dec-1991	Initial entry.
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parser.h"
#include	"cp$src:regexp_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:tt_def.h"
#include	"cp$inc:kernel_entry.h"
#if !MODULAR
#include	"cp$src:help_text.h"
#endif

#include "cp$inc:prototypes.h"

#if MODULAR
extern int cmd_ovly_name;
extern int advanced_mode_flag;
extern int diagnostic_mode_flag;

char advhlp[] = {"\
\t\tAdvanced Command synopsis:\n\
\t\t>file   Re-direct output(create).\n\
\t\t>>file  Re-direct output(append).\n\
\t\t<file   Read input.\n\
\t\t| Pipe (cmd1 | cmd2).\n\
\t\t& Background (cmd1 &).\n\
\t\t; Sequence (cmd1 ; cmd2)\n\
\t\t` Substitution (cmd `cmd`).\n\
\t\tfor          (for var [ in word... ]\n\
\t\t              do cmd_list\n\
\t\t              done).\n\
\t\twhile        (while cmd_list\n\
\t\t              [ do cmd_list ]\n\
\t\t              done).\n\
\t\tcase         (case word in\n\
\t\t              [ pattern [ | pattern ] ...) cmd_list ;; ] ...\n\
\t\t              esac).\n\
\t\tif-then-elif (if cmd_list then cmd_list\n\
\t\t              [ elif cmd_list then cmd_list ] ...\n\
\t\t              [ else cmd_list ]\n\
\t\t              fi).\n\n\
"};
#endif

/*+
 * ============================================================================
 * = man - Display information about console commands.			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to the HELP command.
 *
 *   COMMAND FMT: help_cmd 0 BN 0 man
 *
 *   COMMAND FORM:
 *  
 *	man ( [<command>...]
 *		Command synopsis conventions:
 *		item	Implies a placeholder for user specified item.
 *              item...	Implies an item or list of items.
 *              []	Implies optional keyword or item.
 *              {a,b,c}	Implies any one of a, b, c.
 *              {a|b|c}	Implies any combination of a, b, c. )
 *  
 *   COMMAND TAG: help_cmd 0 RXB man
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<command>... - Specifies the command(s) or topic(s) for which help
 *		       is requested.
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>man
 *	>>>man show
 *~
 *   COMMAND REFERENCES:
 *
 *	help
 *
 * FORM OF CALL:
 *  
 *	help_cmd (int argc, char *argv [])
 *  
 * RETURNS:
 *
 *	0 - Indicates success. 
 *       
 * ARGUMENTS:
 *
 * 	argc	- The number of words present on command line. 
 *	argv	- Array of pointers to words on command line. 
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

/*+
 * ============================================================================
 * = help - Display information about console commands.			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Defines and shows the syntax for each command that you specify on the 
 *	command line. If you do not specify a command, displays information 
 *	about the help command and lists the commands for which additional 
 *	information is available. 
 *  
 *	For each argument (or command) on the command line, help tries to 
 * 	find all topics that match that argument. For example, if there are  
 *	topics on 'exit', 'examine' and 'entry', the command 'help ex' would 
 *	generate the help text for both 'exit' and 'examine'.
 *
 *	Wildcards are supported, so that 'help *' generates the expected
 *	behavior.  Topics are treated as regular expressions that have
 *	the same rules as regular expressions for the shell.  Help topics are
 *	case sensitive.
 *
 *	In describing command syntax, the following conventions are used.
 *#o
 *	o <literal>(<item>) Angle brackets enclose a placeholder, for which the user must
 *	  specify a value.
 *	o [<literal>(<item>)] Square brackets enclose optional parameters, qualifiers,
 *	  or values.
 *	o {a,b,c} Braces enclosing items separated by commas, imply
 *	  mutually exclusive items.  Choose any one of a, b, c.
 *	o {a|b|c} Braces enclosing items separated by vertical bars, imply
 *	  combinatorial items.  Choose any combination of a, b, c.
 *#
 *	The commands help and man can be used interchangeably. 
 *
 *   COMMAND FMT: help_cmd 1 BN 0 help
 *
 *   COMMAND FORM:
 *  
 *	help ( [<command>...]
 *		Command synopsis conventions:
 *		<item>    Implies a placeholder for user specified item.
 *              <item>... Implies an item or list of items.
 *              []        Implies optional keyword or item.
 *              {a,b,c}   Implies any one of a, b, c.
 *              {a|b|c}   Implies any combination of a, b, c. )
 *  
 *   COMMAND TAG: help_cmd 0 RXB help
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<command>... - Specifies the command(s) or topic(s) for which help
 *		       is requested.
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *
 *	In the following example, a list of topics for which help is 
 *	available is requested. 
 *~
 *	>>>help            # List all topics.
 *~
 *	In the next example, help is requested on all topics. 
 *~
 *	>>>help *          # List all topics and associated text.
 *~
 *	In the next example, help is requested on all commands that 
 *	begin with 'ex'.
 *~
 *	>>>help ex         
 *~
 *	In the next example, help is requested on the boot command. 
 *~
 *	>>>help boot 
 *~
 *   COMMAND REFERENCES:
 *
 *	man
 *
 * FORM OF CALL:
 *  
 *	help_cmd (int argc, char *argv [])
 *  
 * RETURNS:
 *
 *	0 - Indicates success. 
 *       
 * ARGUMENTS:
 *
 * 	argc	- The number of words present on command line. 
 *	argv	- Array of pointers to words on command line. 
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int help_cmd (int argc, char *argv [])
{
    int	i;
    char *s;
    int save_name;

#if MODULAR
    save_name = cmd_ovly_name;
    cmd_ovly_name = 0;
#endif
/*
 * If no topics are present, give help on help, and then list
 * all available topics
 */
	if (argc == 1) {

	    help_helper ("help", 0);
	
#if MODULAR
	    if (advanced_mode_flag)
		printf("%s", advhlp);
#endif
	    printf ("The following help topics are available:\n\n");

#if MODULAR
	    *argv[0] = HELP_KEY;
	    cmd_table_lookup(1, argv, 0);
#endif

#if !MODULAR
	    for (i = 0; *helpbuf [i].topic; i++) {
		printf ("%-15.15s ", helpbuf [i].topic);
		if ((i % 5) == 4) printf ("\n");
	    }
	    printf ("\n");
#endif
/*
 * Give help on each topic requested
 */
	} else {

	    for (i=1; i<argc; i++)
		help_helper (argv [i], 0);

	}
#if MODULAR
	if (cmd_ovly_name)	/* if subject required an overlay, remove it */
	    ovly_remove(cmd_ovly_name);
	cmd_ovly_name = save_name;
#endif
	return msg_success;
}

/*
 * treat the string as a regular expression and print out all help that
 * the string is a proper subset of.
 */
int help_helper (char *s, int fonly)
{
	struct REGEXP *re;
	int	i;
	int	found;

	re = regexp_shell (s, 1);	/* fold case */
	if (re == 0) {
	    no_help (s);
	    return msg_failure;
	}

/* Visit each topic */

	found = 0;
#if MODULAR
	found = ovly_help_helper(re, fonly);
#endif
#if !MODULAR
	for (i = 0; *helpbuf [i].topic; i++) {
	    if (anchored_match (re, helpbuf [i].topic, 0) == 0) continue;

	    /* text found, print it out */
	    printf ("NAME    \n\t %s\n",   helpbuf [i].topic);
	    printf ("FUNCTION\n\t %s\n",   helpbuf [i].text);
	    printf ("SYNOPSIS\n\t %s\n\n", helpbuf [i].syn);
	    found++;
	}
#endif
	if (!found) {
	    no_help (s);
	}

	regexp_release (re);
}

/*----------*/
void no_help (char *s) {
    printf("%s doesn't match any topic. Type just \"help\" for a list of topics.\n", s);
}

