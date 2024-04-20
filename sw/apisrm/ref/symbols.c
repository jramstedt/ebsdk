/* file:	symbols.c -- simple symbol table routines
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
 *      Simple symbol table package that facilitates implementation of
 *	environment variables.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      25-May-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	24-Sep-1990	Rearrange structures
 *
 *	ajb	11-Jul-1990	Autodoc updates
 *--
 */
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:parse_def.h"

/*+
 * ============================================================================
 * = sym_find_name - look up a symbol by name                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Look up a symbol by name in a symbol table, and return the address of
 *	the symbol table entry.  Don't assume any ordering in the table. 
 *	Assume the table is terminated with a symbol whose string length is 0,
 *	and whose value is 0.
 *  
 * FORM OF CALL:
 *  
 *	sym_find_name (struct SYMBOL *symtab, char *s)
 *  
 * RETURNS:
 *
 *	x - pointer to resulting symbol, or 0 if not found
 *       
 * ARGUMENTS:
 *
 *      struct SYMBOL *symtab	- pointer to symbol table
 *	char *s			- name to search for
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
struct SYMBOL *sym_find_name (struct SYMBOL *symtab, char *s) {
	struct SYMBOL *sp;

	for (sp=symtab; *sp->mne; sp++) {
	    if (strcmp (sp->mne, s) == 0) return sp;
	}
	return 0;
}


#if 0
/*
 * Dump the contents of a symbol table to the standard output.
 */
void sym_dump (struct SYMBOL *sp) {
	while (*sp->mne) {
	    printf ("%08X %08X    %s\n", sp->v0, sp->v1, sp->mne);
	    sp++;
	}
}

#include "cp$src:parse_def.h"
#include stdio	/* exclude */

struct SYMBOL shell_keywords [] = {
	{"if",		TOK$K_IF,	0},
	{"then",	TOK$K_THEN,	0},
	{"else",	TOK$K_ELSE,	0},
	{"elif",	TOK$K_ELIF,	0},
	{"fi",		TOK$K_FI,	0},
	{"in",		TOK$K_IN,	0},
	{"for",		TOK$K_FOR,	0},
	{"while",	TOK$K_WHILE,	0},
	{"until",	TOK$K_UNTIL,	0},
	{"do",		TOK$K_DO,	0},
	{"done",	TOK$K_DONE,	0},
	{"",		0,		0}
};

/*----------*/
void main (int argc, char *argv []) {
	struct SYMBOL *sp;
	int	argix;

	sym_dump (&shell_keywords);

	printf ("\n");
	for (argix=0; argix<argc; argix++) {
	    sp = sym_find_name (&shell_keywords, argv [argix]);
            if (sp) {
		printf ("%08X    %s\n", sp->value, sp->mne);
	    }
	}
}
#endif	
