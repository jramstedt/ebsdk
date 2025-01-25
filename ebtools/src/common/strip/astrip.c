/*****************************************************************************

Copyright � 1994, Digital Equipment Corporation, Maynard, Massachusetts. 

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
**	Evaluation Board Software Tools - alist
** 
** FUNCTIONAL DESCRIPTION:
** 
**      Generate a list file from an a.out image file.  This
**      module also acts as an image post processor for input
**      into the Palcode Violation Checker.
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
*/
/*
 * $Log: astrip.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:24  paradis
 * Initial CVS checkin
 *
 * Revision 4.3  1997/03/20  23:22:15  fdh
 * Prints out output file size.
 *
 * Revision 4.2  1994/07/11  14:52:06  rusling
 * Fixed problems were tstart and/or astart are not defined.
 *
 */
#ifndef lint
static char *RCSid = "$Id: astrip.c,v 1.1.1.1 1998/12/29 21:36:24 paradis Exp $";
#endif



#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "stdc.h"
#include "aout.h"
/*
 *  Macros
 */
#define TRUE 1
#define FALSE 0
#define BLOCK_SIZE 1000


#define _SEEN(o) seen[o-'a']
#define _OPTION(o) (_SEEN(o) == TRUE)


/*
 *  Global data (all names preceded by 'strip_g'.
 */
char *strip_g_buffer = NULL;
unsigned int strip_g_buffer_size = 0;
unsigned int strip_n = 0;
#define SEEN_SIZE 100
char seen[SEEN_SIZE];			/* upper and lower case */
/*
 *  Forward routine descriptions.
 */
int main(int argc, char **argv);
void usage();
void read_file(char *filename);
void write_file(char *filename);
/*
 *  External routines.
 */

int main(int argc, char **argv)
{
    char *ifname = NULL;	/* default .o file */
    char *ofname = "a.out.strip";
    char *arg, option;
    int i;

    for (i=0;i<SEEN_SIZE;i++) seen[i] = FALSE;
/*
 * Parse arguments, but we are only interested in flags.
 * Skip argv[0].
 */
    for (i=1;i<argc;i++) {
	arg = argv[i] ;
	if (*arg == '-') {
/*
 * This is a -xyz style options list.  Work out the options specified.
 */
	    arg++;			/* skip the '-' */	    
	    while (option = *arg++) {	/* until we reach the '0' string terminator */
		switch (option) {
		    case 'h':
		    case 'H':
			usage();
			exit(1);
		    case 'r':		/* round to an 8 byte boundary */
		    case 'R':
		    case 'v':		/* verbose, allow upper and lower case */
		    case 'V':
		    case 'a':           /* strip *all* components (text and data) */
		    case 'A':
			_SEEN(tolower(option)) = TRUE;
			break;
		    case 'N':
		    case 'n':
/*
 *  This is a special one, the -n option is followed (immediately)
 *  by a number, the number of bytes to break off the file.
 */
			_SEEN(tolower(option)) = TRUE;
			sscanf(arg, "%d", &strip_n);
			arg = arg + strlen(arg);
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
	    if (ifname == NULL)
		ifname = arg;
	    else
		ofname = arg;
	}
    }

    if _OPTION('v')
	fprintf(stderr, "a.out file dumper [V1.0]\n");
/*
 *  Read in the coff file and save it in memory before formatting.
 */
    read_file(ifname);
    if _OPTION('v') {
	fprintf(stderr, "Strip of a.out object file %s of size %d bytes\n", ifname, 
	  strip_g_buffer_size);	
	if _OPTION('n') 
	    fprintf(stderr, "Strip %d bytes\n", strip_n);
	fprintf(stderr, "Output file is %s\n", ofname);
    }
 
/*
 *  Now go and analyse the file contents.
 */
    write_file(ofname);

    return EXIT_SUCCESS;
}					/* end of main() */

void usage()
{
    printf("astrip <options> <infile> [<outfile>]\n");
    printf("\tStip the header from an a.out format object file\n\toptions:\n");
    printf("\tNOTE: Upper and Lower case options are treated equally\n");
    printf("\tv,V = turn on verbose mode\n");
    printf("\th,H = print this help file\n");
    printf("\tn,N<number> = strip this number of bytes off the file\n");
    printf("\tr,R = round the file size to an 8 byte boundary\n");
    printf("\ta,A = strip all sections (text and data)\n");
}

void read_file(char *filename)
{
    FILE *ifile;
    char *newbuffer;
    unsigned int size;

    if ((ifile = fopen(filename,"rb")) == NULL) {
	fprintf(stderr, "ERROR: cannot open file - %s",filename);
	exit(1) ;
    }
/*
 *  Read the file into memory, block by block, allocating as we
 *  go along.
 */
    while(TRUE) {
	newbuffer = (char *)malloc(strip_g_buffer_size + BLOCK_SIZE);
	if (strip_g_buffer_size != 0) {
	    memcpy(newbuffer, strip_g_buffer, strip_g_buffer_size);
	    free(strip_g_buffer);
	    }
	size = fread(newbuffer+strip_g_buffer_size, sizeof(char), 
	    BLOCK_SIZE, ifile);
	strip_g_buffer = newbuffer;
	if (size == 0) {
	    fclose(ifile);
	    return;		    /* we've finished */
	} else {
	    strip_g_buffer_size = strip_g_buffer_size + size;
	}
    }
}

void write_file(char *filename)
{
    FILE *ofile;
    char *p;
    unsigned int size, i, offset, extra;
    struct exec *head;

    head = (struct exec *) strip_g_buffer;

    if ((ofile = fopen(filename,"wb")) == NULL) {
	fprintf(stderr, "ERROR: cannot open file - %s",filename);
	exit(1) ;
    }
/*
 *  figure out how much text is in the file and
 *  where it starts.  For -n<size> we don't need to 
 *  work that out.
 */
    if _OPTION('n') {
	size = strip_g_buffer_size - strip_n;
	offset = strip_n;
	p = (char *)(strip_g_buffer + strip_n);
    } else {
#if CROSS_COMPILE_32_TO_64_BITS
	size = low32(head->a_text);
#else
	size = head->a_text;
#endif
	offset = N_TXTOFF(*head);
	p = (char *)(strip_g_buffer + offset); 
	if _OPTION('a') {
	    /* The size is not simply a_text + a_data because we need to
	     * include anything in between for the offsets to be right 
	     */
#if CROSS_COMPILE_32_TO_64_BITS
	    if ((low32(head->a_dstart) == 0) || (low32(head->a_tstart) == 0))
	    	size = low32(head->a_data) + low32(head->a_text);
	    else
		size = low32(head->a_data) + 
	      (low32(head->a_dstart) - low32(head->a_tstart) 
		- low32(head->a_text));
#else
	    if ((head->a_dstart == 0) || (head->a_tstart == 0))
	        size = head->a_data + head->a_text;
	    else
		size = head->a_data + 
		    (head->a_dstart - head->a_tstart - head->a_text);
#endif
	}
    }

    if _OPTION('v') {
    	fprintf(stderr, "...strip size is %d\n", size);
    	fprintf(stderr, "...at %d bytes into the input file\n", offset);
    }
/*
 *  Write the file into memory, block by block.
 */
    i = 0;
    while(i < size) {
	fputc(*p, ofile);
	p++;
	i++;
    }
/*
 *  Pad it out to modulus 8 size.
 */
   if ( ((size & 0x7) != 0) && (_OPTION('r')) ) {
	fprintf(stderr, "WARNING: text size not modulus 8 - rounding\n");
	extra = 8 - (size & 0x7) ;
	size += extra;
    	i = 0;
    	while(i < extra) {
	    fputc(0, ofile);
	    i++;
	}
   }

    printf("File %s written %d (0x%x) Bytes\n\n",
	   filename, size, size);

/*
 *  Now close the file.
 */
    fclose(ofile);
}

