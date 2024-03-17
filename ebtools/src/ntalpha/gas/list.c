/* list.c -- List file generator
   Copyright (C) 1989 Free Software Foundation, Inc.

This file is part of GAS, the GNU Assembler.

GAS is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GAS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GAS; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


#if __osf__
#include <mach/boolean.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <stab.h>
#include "as.h"
#include "frags.h"
#include "struc_sy.h"
#include "flonum.h"
#include "expr.h"
#include "hash.h"
#include "md.h"
#include "write.h"
#include "read.h"
#include "symbols.h"

/* local data structures */

/* describes a line of the file associated with some code
   fragment */
typedef struct line {
    struct line *next;
    unsigned int pnumber;
    unsigned int lnumber;
    unsigned long opcode;
} lineS;

/* describes an input file */
typedef struct lfile {
    struct lfile *next;
    lineS *lines;
    char *name;
} lfileS;

static lfileS *root = NULL;

/* local routines */
void list_begin(char *file);
void list_add(unsigned long opcode);
void list_symbols();
void list_end();

/* external routines */

unsigned int get_physical_input_line();
unsigned int get_logical_input_line();
void md_print_instruction(int instruction, FILE* stream);

/* this routine performs all of the startup required by the list functionality
   of the GNU assembler */

void list_begin(char *file)
{
    lfileS *f;

/* don't bother if a list file is not wanted */
    if (!flagseen['l'])
	return;

    f = (lfileS *)xmalloc(sizeof(lfileS));
    if (f == NULL)
	return;		/* make this an error, one day */

    f->lines = NULL;
    f->name = file;	/* pointer to the file's name */

    if (root != NULL)
	root->next = f;
    root = f;
}

/* add details about the current file, and its code fragments into the
   a new list data structure */
void list_add(unsigned long opcode)
{
    lineS *l,*s;

/* don't bother if a list file is not wanted */
    if (!flagseen['l'])
	return;

/*  allocate a line entry */

    l = (lineS *)xmalloc(sizeof(lineS));
    if (l == NULL)
	return;  	/* make this an error, one day */

    l->next = NULL;	
    l->pnumber = get_physical_input_line();
    l->lnumber = get_logical_input_line() - 1;
    l->opcode = opcode;

/*  queue it on the file entry associated with it, always pointed
    at by root. */
  
    if (root->lines == NULL)
	root->lines = l;
    else {
	s = root->lines;
	while (s->next != NULL) s = s->next;
	s->next = l;
    }
}
/* this routine lists information about symbols found during assembly */
    
void list_symbols()
{
    symbolS *symbolP;

    printf("\nSymbolic Information:\n\n");
    for (symbolP = symbol_rootP; symbolP; symbolP = symbolP->sy_next) {
    	printf("\t%s (type = ", symbolP->sy_nlist.n_un.n_name);
	switch (symbolP->sy_type) {
	    case N_UNDF:
		printf("Undefined (External)");
		break;
	    case N_ABS:
		printf("Absolute");
		break;
	    case N_TEXT:
		printf("Text");
		break;
	    case N_DATA:
		printf("Data");
		break;
	    case N_BSS:
		printf("BSS");
		break;
	    case N_COMM:
		printf("Common");
		break;
	    case N_FN:
		printf("FN");
		break;
	    case N_EXT:
		printf("External (Undefined)");
		break;
	    case N_TYPE:
		printf("Type");
		break;
	    case N_STAB:
		printf("STAB");
		break;
	    default:
	 	printf("UNKNOWN");
	}
	printf(")\n");
    }
    printf("\n");
}

/* this routine is called when a complete listing file is required
   to be output, so dump the file out and throw away all the 
   appropriate data structures */
void list_end()
{
    lfileS *f;
    FILE *source;
    lineS *l;
#define MAXLINE 80
    char *c, buffer[MAXLINE];
    unsigned int i;


/* don't bother if a list file is not wanted */
    if (!flagseen['l'])
	return;
/* 
 * for every file...
 */
    f = root;
    while (f) {
	printf("LISTING %s\n", f->name);
/*
 *  Read the file and print it out, interspersing the opcodes generated.
 */
	source = fopen(f->name, "r");
	if (source ==(FILE *)0) {
	    as_perror ("Can't open source file for listing", f->name);
	    return;
	}

/*
 *  Read lines until the end of the file.  Print out associated instructions
 *  after the corresponding line (note that there may be more than one
 *  instruction.
 */
	i = 1;
	l = f->lines;
	while (fgets(buffer, MAXLINE, source) != NULL) {
	    printf("[%08d] %s", i, buffer);
	    while (l) {
		if (i == l->lnumber) {
		    printf("\t\t%0X  ", l->opcode);
		    md_print_instruction(l->opcode, stdout);
		    printf("\n");
		    l = l->next;
		} else 
		    break;
	    }
	    i++;
	}
/*
 *  Print out any instructions that are left.
 */
	while (l) {
	    printf("\t\t\t%0X\n", l->opcode);
	    l = l->next;
	}
	f = f->next;
    }
/*
 *  Dump the symbolic information
 */
    list_symbols();

    return;
}


