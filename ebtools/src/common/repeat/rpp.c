/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/
/*
**
** FACILITY:	
**
**	ED64 Software Tools - repeat pre-processor.
** 
** FUNCTIONAL DESCRIPTION:
** 
**      Takes a file as input and outputs it to either the same file
**      or to a different file if named.  In between it processes all 
**      the repeat directives.
** 
** CALLING ENVIRONMENT: 
**
**	user mode
** 
** AUTHOR: David A Rusling
**
** CREATION-DATE: 05-NOV-1992
** 
** MODIFIED BY: 
** 
**      Who	When		What
**	-----	-----------	---------------------------------------------
**      DAR        Aug-1993     Modified so that repeat and endrepeat directives
**                              could be on the same line.
**	DAR	   Jul-1993	First version.
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/*
 *  Macros
 */
#define TRUE 1
#define FALSE 0
#define BLOCK_SIZE 1000

#define _SEEN(o) seen[o-'a']
#define _OPTION(o) (_SEEN(o) == TRUE)
/*****************************************************************
 *  Global data
 *****************************************************************/
/*
 *  Keep some counters.
 */
struct {
    unsigned int lines;
    unsigned int fragments;
    unsigned int memory;
} counters = {0, 0, 0};
/*
 *  Somewhere to keep the options.
 */
#define SEEN_SIZE 100
char seen[SEEN_SIZE];			/* upper and lower case */

/*
 *  What state are we in?
 */
#define K_STATE_NORMAL   42
#define K_STATE_REPEAT   43
/*
 *  Somewhere to keep the state.
 */
unsigned int gl_state = K_STATE_NORMAL;
/*
 *  Somewhere to record each repeat (they may be nested).  You should
 *  note that repeats points at the innermost repeat block (it's a 
 *  singly linked list).
 */
#define MIN_REPEAT 80

typedef struct head {
    struct frag *first;
    struct frag *last;
} headT;

typedef struct frag {
    struct frag *next;			/* next  repeatfrag in the chain */
    struct frag *parent;		/* the parent fragment */
    char variant;                       /* which variant of the directive is this? */
    char *start;		        /* start of buffer containing repeat
					 * text */
    unsigned int size;			/* size of text in buffer */
    unsigned int repeat;		/* number of times to rpeat the block
					 */
    unsigned int bufl;                  /* buffer length */
    headT subs;				/* sub-fragments */
} fragT;

struct {
    fragT *first;
    fragT *current;
} base = {NULL, NULL};

/*
 *  Arrays describing what a begin and end repeat directive looks like.
 *  (We allow different flavours).
 */
struct directive {
#ifdef ultrix
    char * (*action)();
#else
    char * (*action)(char *buf, char variant, FILE *file);
#endif
					/* action routine to process the event
					 */
    unsigned int length;		/* length of the keyword */
    char *key;				/* pointer to the keyword */
    char variant;		        /* which variant of the key is it? */
};

/*
 *  Forward routine descriptions.  These have to come before the directives
 *  structure definition.
 */
int main(int argc, char **argv);
void usage();
void process_file(char *ofname, char *ifname);
void save_repeat(char c);
char *p_repeat(char *buf, char variant, FILE *ofile);
char *p_endrepeat(char *buf, char variant, FILE *ofile);
void output_fragment_tree(fragT *walker, FILE *ofile);
void free_fragment_tree(fragT *walker);
char *find_and_action_directive(char *buf, FILE *ofile);
fragT *create_fragment(unsigned int count, unsigned int size);

#define DIRECTIVES 6         		/* don't forget to change this! */
struct directive directives[DIRECTIVES] = {
    {p_repeat, 7, ".repeat", 0}, 
    {p_endrepeat, 10, ".endrepeat", 0}, 
    {p_repeat, 7, "#repeat", 1}, 
    {p_endrepeat, 10, "#endrepeat", 1},
    {p_repeat, 5, ".rept", 2}, 
    {p_endrepeat, 5, ".endr", 2}
};

int main(int argc, char **argv)
{
    char *ifile_name = NULL, *ofile_name = NULL;
    char *arg, option;
    int i;

    for (i = 0; i < SEEN_SIZE; i++)
	seen[i] = FALSE;
/*
 * Parse arguments, but we are only interested in flags.
 * Skip argv[0].
 */
    for (i = 1; i < argc; i++) {
	arg = argv[i];
	if (*arg == '-') {
/*
 * This is a -xyz style options list.  Work out the options specified.
 */
	    arg++;			/* skip the '-' */
	    while (option = *arg++) {	/* until we reach the '0' string
					 * terminator */
		option = tolower(option);
		switch (option) {
		    case 'h': 
			usage();
			exit(1);
		    case 'v': 		/* verbose, allow upper and lower case
					 */
			_SEEN(option) = TRUE;
			break;
		    case 'o': 
			ofile_name = ++arg;
			arg = arg + strlen(arg);
			_SEEN(option) = TRUE;
			break;
		    case 'i': 
			ifile_name = ++arg;
			arg = arg + strlen(arg);
			_SEEN(option) = TRUE;
			break;
		    default: 
			usage();
			exit(0);
			break;
		}
	    }
	} else {
/*
 *  This is a filename, ignore multiple filenames, just take the 
 *  last one specified.  This is not a rolls-royce interface.
 */
	    if (ifile_name == NULL)
		ifile_name = arg;
	    else {
		if (ofile_name == NULL)
		    ofile_name = arg;
		else {
		    fprintf(stderr, "ERROR: too many file names given\n");
		    exit(0);
		}
	    }
	}
    }					/* end of for arguments */

/*
 *  If no input file name was given, complain.
 *  If no output file name was given then use stdout
 */
    if (ifile_name == NULL) {
	fprintf(stderr, "ERROR: no input file name given\n");
	exit(1);
    }
    if (_OPTION('v')) {
	fprintf(stderr, "Repeat pre-processor [V1.1]\n");
	fprintf(stderr, "Input file = %s\nOutput file = %s\n", ifile_name, ofile_name);
    }
/*
 *  Now go and play with the file.
 */
    process_file(ifile_name, ofile_name);

    return EXIT_SUCCESS;
}					/* end of main() */

void usage()
{
    printf("rpp - description\n");
    printf("\tRepeat pre-processor\n");
    printf("rpp - usage : rpp -h | rpp [-v] <input file> [<output file>] \n");
    printf("\tNOTE: Upper and Lower case options are treated equally\n");
    printf("\t      output file defaults to stdout\n");
    printf("\t-v,V = turn on verbose mode\n");
    printf("\t-h,H = print this help file\n");
}

void process_file(char *ifname, char *ofname)
{
    FILE *ofile, *ifile;

#define MAX_READ 120
    char ibuf[MAX_READ];

    char *p;
/*
 *  First, open the files.
 */

    if ((ifile = fopen(ifname, "r")) == NULL) {
	fprintf(stderr, "ERROR: cannot open file - %s\n", ifname);
	exit(1);
    }

    if (ofname == NULL)
	ofile = stdout;
    else {
	if ((ofile = fopen(ofname, "w")) == NULL) {
	    fprintf(stderr, "ERROR: cannot open file - %s\n", ofname);
	    exit(1);
	}
    }
/*
 * Read the input file looking for start and end repeat directives.
 * Repeat directives may be nested and each repeat block contains
 * a block of memory with the repeat chunks in it.
 */

    while (fgets(ibuf, MAX_READ, ifile) != 0) {
	counters.lines++;
/*
 *  Search for a directive within the line and whilst we are 
 *  not inside a directive save the input or output it 
 *  directly.
 */
	p = ibuf;
	while (p < (ibuf + strlen(ibuf))) {
/*
 *  Try and find a match for the directive.  If it does, p gets
 *  moved on past it (so we have to check that we're not at the
 *  end of the buffer).
 */
	  p = find_and_action_directive(p, ofile);
	  if (p < (ibuf + strlen(ibuf))) {
	  if (gl_state == K_STATE_REPEAT)
	    save_repeat(*p++);
	  else
	    fputc(*p++, ofile);
	}
	}
    }
/*
 *  We've exhausted the input file, are there any repeat blocks
 *  lying around.  If there are then we have an error.
 */
    if (base.first != NULL) {
	fprintf(stderr, "ERROR: one or more repeat directives have missing\n");
	fprintf(stderr, "\tend repeat directives\n");
	exit(1);
    }
/*
 *  If the caller wants details, then print out the counters.
 */
    if _OPTION('v') {
	fprintf(stderr, "\nStatistics:\n\n");
	fprintf(stderr, "\tlines read = %d\n", counters.lines);
	fprintf(stderr, "\tbytes allocated = %d\n", counters.memory);
	fprintf(stderr, "\tfragment blocks allocated = %d\n", counters.fragments);
    }
}


void save_repeat(char c)
{
    fragT *frag = base.current;

    if (frag == NULL) {
	fprintf(stderr, "ERROR: (internal) saving repeat blocks\n");
	fprintf(stderr, "\twithout a repeat directive first being found\n");
	exit(1);
    }
/*
 *  If the current fragment has children, then this is text that
 *  comes between nested sub-repeat blocks.  Otherwise it is text
 *  inside a leaf fragment.
 */
    if (frag->subs.first != NULL) {
	fragT *new;
/*
 *  Non-leaf fragment.  Create a new child fragment with a repeat
 *  count of 1 and put it at the end of frag's sub-fragments.  Do
 *  Not make the new fragment the current fragment.  This means that
 *  each line of text between sub-fragments generates a new fragment
 *  block.  That's ok as we allocate as little memory as possible knowing
 *  that it will never be added to.
 */

	new = create_fragment(1, 10);
	frag->subs.last->next = new;
	frag->subs.last = new;
	new->parent = frag;
	frag = new;
    }
/*
 *  Save the text in the fragment.
 */
    if (frag->size < frag->bufl) {
	frag->start[frag->size] = c;
	frag->size++;
    } else {
        char *buffer;
	unsigned int size, i;
/*
 *  Try and allocate a bigger buffer.
 */
	size = frag->bufl + 10;
	buffer = (char *)malloc(size);
	counters.memory+= size;
	if (buffer == NULL) {
	  fprintf(stderr, "ERROR: failed to allocate a fragment buffer\n");
	  exit(1);
	}
	memset(buffer, '\0', size);
	for (i=0; i<frag->size; i++) buffer[i] = frag->start[i];
	buffer[frag->size + 1] = c;
	frag->size++;
/*
 *  Throw away the old buffer and fix up the new one.
 */
	free(frag->start);
	frag->start = buffer;
	frag->bufl = size;
    }
}

char *p_repeat(char *buf, char variant, FILE *ofile)
{
    unsigned int count;
    fragT *frag;
    char *end = buf;

/*
 *  Work out the count of repeats (buf points
 *  past the directive itself).
 */
    sscanf(buf, "%d", &count);
    frag = create_fragment(count, MIN_REPEAT);
/*
 *  If the world wants to know, then tell it.
 */
    if _OPTION('v') 
      fprintf(stderr, "\trepeat directive on line %d, repeat %d times\n",
	    counters.lines, count);
/*
 *  Work out where the end of this directive is.
 *  One or more spaces followed by some digits.
 */
    while (!(isspace(*end++)));
    while (isdigit(*end++));
/*
 *  Set the variant of the repeat directive (we check it on
 *  the end repeat).
 */
    frag->variant = variant;
/*
 *  Put it into the tree of fragments.
 */
    if (base.first == NULL) {
/*
 *  Empty, this is the first repeating fragment so make the
 *  the base point to it.
 */
	base.first = frag;
	base.current = frag;
    } else {
/*
 *  base.current points at the current node in the tree.
 *  This repeat must be a sub-repeat of that node (ie below
 *  it in the tree).  There are two possibilities, the first
 *  is that the node has no subs yet and the second that it 
 *  has subs already.  Either way, base.current should end
 *  up pointing the the new fragment.
 */
	fragT *current = base.current;

	if (current->subs.first == NULL) {
	    current->subs.first = frag;
	    current->subs.last = frag;
	} else {
	    current->subs.last->next = frag;
	    current->subs.last = frag;
	}
	frag->parent = base.current;
	base.current = frag;
    }
/*
 *  Tell the caller that we're in (one or more) repeat
 *  directives.
 */
    gl_state = K_STATE_REPEAT;
    return(end);
}

char *p_endrepeat(char *buf, char variant, FILE *ofile)
{
    fragT *frag = base.current;
/*
 *  Look for errors.
 */
    if (frag == NULL) {
	fprintf(stderr, "ERROR: (line %d) you have entered an end repeat\n",
	  counters.lines);
	fprintf(stderr, "\tdirective without a matching repeat directive\n");
	exit(1);
    }

    if (frag->variant != variant) {
        fprintf(stderr, "ERROR: (line %d) end repeat directive does not\n",
		counters.lines);
	fprintf(stderr, "\tmatch repeat directive\n");
        exit(1);
    }
/*
 *  If the world wants to know, then tell it.
 */
    if _OPTION('v') 
      fprintf(stderr, "\tend repeat directive on line %d, repeat %d times\n",
	    counters.lines, frag->repeat);
/*
 *  If this is the last fragment, then just output it (and its sub-
 *  tree).  Otherwise, just move the current pointer up the tree.
 */
    if (frag->parent != NULL) {
	base.current = frag->parent;
	gl_state = K_STATE_REPEAT;
    } else {
/*
 *  Output the whole sub-tree and then free up the fragments.
 */
	output_fragment_tree(base.first, ofile);
	free_fragment_tree(base.first);
	base.first = base.current = NULL;
	gl_state = K_STATE_NORMAL;
    }
/*
 *  Return where in the buffer we are.
 */
    return(buf);
}

void output_fragment_tree(fragT *walker, FILE *ofile)
{
    unsigned int i,j;

    for (i = 0; i < walker->repeat; i++) {
        for (j=0; j<walker->size; j++) fputc(*(walker->start + j), ofile);
	if (walker->subs.first != NULL)
	    output_fragment_tree(walker->subs.first, ofile);
    }
    if (walker->next != NULL)
	output_fragment_tree(walker->next, ofile);
}

void free_fragment_tree(fragT *walker)
{
/*
 *  Recursively walk around the tree freeing off brothers and
 *  siblings and finally itself.
 */
    if (walker->subs.first != NULL)
	free_fragment_tree(walker->subs.first);
    else {
	if (walker->next != NULL)
	    free_fragment_tree(walker->next);
    }
    free(walker->start);
    free(walker);
}

char *find_and_action_directive(char *buf, FILE *ofile)
{
    unsigned int i;
    unsigned int matched;

    for (i = 0; i < DIRECTIVES; i++) {
        matched = strncmp(buf, directives[i].key, directives[i].length);
	if (matched == 0) {
/*
 *  We've found a matching directive, go and process it.
 */
	      return((*directives[i].action)(buf + directives[i].length, 
		   directives[i].variant, ofile));

	}				/* we've found a matching directive */
    }					/* end of for looking at directives */

    return (buf);
}

fragT *create_fragment(unsigned int count, unsigned int size)
{
    fragT *frag;
    char *buffer;
/*
 *  Allocate a frag block plus a buffer to keep the input in.
 */

    frag = (fragT *) malloc(sizeof(fragT));
    if (frag == NULL) {
	fprintf(stderr, "ERROR: cannot allocate a frag block\n");
	exit(1);
    }
    buffer = (char *)malloc(size);
    if (buffer == NULL) {
        fprintf(stderr, "ERROR: cannot allocate a fragment buffer\n");
	exit(1);
    }
/*
 *  Let's be paranoid.  Zero everything.
 */
    memset(frag, 0, sizeof(fragT));
    memset(buffer, '\0', size);
/*
 *  Bump the counters.
 */
    counters.memory += sizeof(fragT) + size;
    counters.fragments++;
/*
 *  Now fill it in.
 */
    frag->next = NULL;
    frag->parent = NULL;
    frag->start = buffer;
    frag->size = 0;
    frag->bufl = size;
    frag->repeat = count;
    return (frag);
}
