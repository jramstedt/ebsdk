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
**      ED64 Software Tools - cstrip
** 
** FUNCTIONAL DESCRIPTION:
** 
**      Strip out the text portion of an coff format object
**      file.
** 
** CALLING ENVIRONMENT: 
**
**      user mode
** 
** AUTHOR: David A Rusling
**
** CREATION-DATE: 05-NOV-1992
** 
** MODIFIED BY: 
** 
**      Who     When            What
**      -----   -----------     ---------------------------------------------
**      DAR        Nov-1992     First version.
*/
#ifndef LINT
static char *rcsid = "$Id: cstrip.c,v 1.2 1999/04/09 17:39:29 paradis Exp $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef _WIN32
#include <string.h>
typedef unsigned long  vm_offset_t;
#else
#include <strings.h>
#endif
#include "coff.h"
#include "syms.h"
#include "reloc.h"
/*
 *  Macros
 */
#define TRUE 1
#define FALSE 0
#define BLOCK_SIZE 1000

#define _SEEN(o) (seen[o-'a'])

#define _error(string1,string2)\
     {  \
	printf("error: %s %s\n", string1, string2);     \
	exit(1);    \
     }

/*
 *  Get as much from a file as you want from where you want.
 */
void FGET(FILE *file, int offset, void *buffer, int size)
{
    if (fseek(file, (long) (offset), SEEK_SET) != 0) {
	fprintf(stderr, "ERROR: positioning file too %d\n", (offset));
	exit(0);
    }
    if (fread((char *) buffer, sizeof(char) , size, file) == NULL) {
	fprintf(stderr, "ERROR: reading file at position %d\n", offset);
	exit(0);
    }
}

unsigned long fsize(FILE *fp)
{
    unsigned long size;

    size = 0;
    fseek(fp, 0, 0);
    while (getc(fp) != EOF)
	size++;
    fseek(fp, 0, 0);
    return size;
}

/*
 *  Global data (all names preceded by 'coff_g'.
 */
unsigned int strip_n;
#define SEEN_SIZE ('z' - 'a' + 10)
char seen[SEEN_SIZE];                   /* upper and lower case */
/*
 *  Forward routine descriptions.
 */
main(int argc, char **argv);
void usage();
void write_file(FILE *in, char *filename);
unsigned int write_text_section(FILE *in, FILE *out);
unsigned int write_sections(FILE *in, FILE *out);
unsigned int write_a_section(FILE *in, FILE *out, struct scnhdr *shdr);
void copy_to_file(FILE *in, FILE *out, int offset, int size);

static char *Revision = "$Revision: 1.2 $";
static char *DateStamp = __DATE__;
static char *TimeStamp = __TIME__;

main(int argc, char **argv)
{
    char *in_file_name = NULL;
    char *out_file_name = NULL;
    char *arg, option;
    int i;
    FILE *in;

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
	    arg++;                      /* skip the '-' */
	    while (option = *arg++) {   /* until we reach the '0' string
					 * terminator */
		switch (option) {
		    case 'h':           /* help */
		    case 'H': 
			usage();
			exit(1);
		    case 'v':           /* verbose */
		    case 'V': 
			printf("cstrip - version (%s)\n", rcsid);

		    case 'r':           /* round to 8 bytes in size */
		    case 'R': 
		    case 'a':           /* strip out all sections */
		    case 'A': 
			_SEEN(tolower(option)) = TRUE;
			break;
		    case 'n':           /* strip of the first n bytes */
		    case 'N': 
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
	    if (in_file_name == NULL) {
		in_file_name = arg;
	    } else {
		out_file_name = arg;
	    }
	}
    }
/*
 *  If verbose has been selected reveal the arguments.
 */
    if (_SEEN('v')) {
	char c;
	fprintf(stderr, "%s - generate an Intel hex32 file [%s %s %s]\n",
		argv[0], Revision, DateStamp, TimeStamp);

	fprintf(stderr, "arguments : ");
	for (c = 'a'; c < 'z'; c++)
	    if _SEEN(c)
		fprintf(stderr, "%c", c);
	fprintf(stderr, "\n\n");
    }
/*
 *  Try and open the input file.
 */
    in = fopen(in_file_name, "rb");
    if (in == NULL) {
	fprintf(stderr, "ERROR: unable to open file (%s)\n", in_file_name);
	exit(0);
    }
/*
 *  Now go and analyse the file contents.
 */
    write_file(in, out_file_name);
 return 0;
}                                       /* end of main() */

void usage()
{
    printf("cstrip <options> <infile> [<outfile>]\n");
    printf("\tStip the header from a coff format object file\n\toptions:\n");
    printf("\tNOTE: Upper and Lower case options are treated equally\n");
    printf("\tv,V = turn on verbose mode\n");
    printf("\th,H = print this help file\n");
    printf("\tn,N<number> = strip this number of bytes off the file\n");
    printf("\tr,R = round the file size to an 8 byte boundary\n");
    printf("\ta,A = extract all the sections from the coff file\n");
}

void write_file(FILE *in, char *filename)
{
    FILE *out;
    unsigned int size;

    if ((out = fopen(filename, "w")) == NULL) {
	printf("ERROR: cannot open file - %s", filename);
	exit(1);
    }
/*
 *  figure out how much text is in the file and
 *  where it starts.  For -n<size> we don't need to 
 *  work that out.
 */
    if _SEEN('n') {
	size = fsize(in) - strip_n;
	if _SEEN('v') {
	    printf("...text size is %d\n", size);
	    printf("...at %d bytes into the input file\n", strip_n);
	}
/*
 *  Write the file into memory, block by block.
 */
	copy_to_file(in, out, strip_n, size);
    } else {
/*
 *  Write out one or more of the sections.
 */
	if _SEEN('a')
	    size = write_sections(in, out);
	else
	    size = write_text_section(in, out);
    }


/*
 *  Pad it out to modulus 8 size.
 */
    if (((size & 0x7) != 0) && (_SEEN('r'))) {
	int extra;

	printf("WARNING: text size not modulus 8 - rounding\n");
	extra = 8 - (size & 0x7);
	size += extra;
	while (extra--)
	    fputc(0, out);
    }

    printf("File %s written %d (0x%x) Bytes\n\n",
	   filename, size, size);
/*
 *  Now close the file.
 */
    fclose(out);
}

unsigned int write_text_section(FILE *in, FILE *out)
{
    unsigned long int used, i;
    struct filehdr fhdr;
    struct scnhdr shdr;
    struct aouthdr ohdr;

    FGET(in, 0, (void *) &fhdr, sizeof(fhdr));
    FGET(in, sizeof(fhdr), (void *) &ohdr, sizeof(ohdr));
    used = sizeof(struct filehdr) + fhdr.f_opthdr;
/*
 *  Look for the text section.
 */
    for (i = 0; i < fhdr.f_nscns; i++) {
	FGET(in, (int) used, (void *) &shdr, sizeof(shdr));
	if ((shdr.s_size != 0) && (shdr.s_scnptr != 0)) {
/*
 *  We're only dumping the .text section, so check for this.
 */
	    if (strcmp(shdr.s_name, ".text") == 0)
		return (write_a_section(in, out, &shdr));
	    used = used + sizeof(struct scnhdr);
	}
    }
    return 0;
}

typedef struct section {
    struct section *next;
    unsigned long int physical;
    unsigned long int size;
    unsigned long int scnhdr;
} section_t;
section_t *list = NULL;

static void InsertSection(section_t *new)
{
    if (list == NULL) {
	new->next = list;
	list = new;
	return;
    } else {
	if (new->physical < list->physical) {
	    new->next = list;
	    list = new;
	    return;
	} else {
	    section_t *last, *next;

	    last = list;
	    next = last->next;
	    while (next != NULL) {
		if (next->physical > new->physical) {
		    new->next = last->next;
		    last->next = new;
		    return;
		}
		last = next;
		next = next->next;
	    }
	    last->next = new;
	    new->next = NULL;
	    return;
	}
    }
}

unsigned int write_sections(FILE *in, FILE *out)
{
/*
 *  Dump out all of the sections of the file, with padding
 *  between them.  Note that the sections may not be in ascending
 *  image address order.
 */
    unsigned long int used, i, size = 0, first = TRUE, physical = 0;
    struct filehdr fhdr;
    struct scnhdr shdr;
    struct aouthdr ohdr;
    section_t *s;

    FGET(in, 0, (void *) &fhdr, sizeof(fhdr));
    FGET(in, sizeof(fhdr), (void *) &ohdr, sizeof(ohdr));
/*
 *  First, build a list of section_t structures describing
 *  the sections in ascending physical order.
 */
    used = sizeof(struct filehdr) + fhdr.f_opthdr;
    for (i = 0; i < fhdr.f_nscns; i++) {
	FGET(in, (int) used, (void *) &shdr, sizeof(shdr));
/*
 *  Is this is a valid section header?   It must have a section
 *  pointer and some size at least.
 */
	if ((shdr.s_size != 0) && (shdr.s_scnptr != 0)) {
/*
 *  Furthermore, it should not be a .comment section (introduced in
 *  OSF V3.0).
 */
	    if (strncmp(shdr.s_name, ".comment", sizeof(".comment")) == 0) {
		if _SEEN('v') 
		    printf("skipping .comment section\n", size);
	    } else {
		section_t *new;
/*
 *  Allocate a new section struction and fill out its details.
 */
		new = (section_t *) malloc(sizeof(section_t));
		new->scnhdr = used;
		new->physical = shdr.s_paddr;
		new->size = shdr.s_size;
/*
 *  Insert it into the list in ascending physical order.
 */
		InsertSection(new);
	    }
	}
	used = used + sizeof(struct scnhdr);
    }
/*
 *  Dump out each section in turn, if we're only dumping text then
 *  check for that.
 */
    s = list;
    while (s != NULL) {
	FGET(in, (int)(s->scnhdr), (void *) &shdr, sizeof(shdr));
/*
 *  Put out any padding required.
 */
	if (first == TRUE)
	    first = FALSE;
	else {
	    if (physical < shdr.s_paddr) {
		unsigned int fill;
		int fill_c = '0';

		fill = shdr.s_paddr - physical;
		if _SEEN('v') {
		    printf(">>> Section %s, size %d, paddr 0x%x, offset %d\n",
			shdr.s_name, shdr.s_size, shdr.s_paddr, shdr.s_scnptr);
		    printf("    physical = 0x%x\n", physical);
		    printf("...padding out %d bytes\n", fill);
		}
		while (fill--)
		     fputc(fill_c, out);

		size = size + fill;
	    }
	}
/*
 *  Now output the section.
 */
	size = size + write_a_section(in, out, &shdr);
/*
 *  Update the physical address where we're at in the file
 *  (so that we can pad out the next section).
 */
	physical = shdr.s_paddr + shdr.s_size;
/*
 *  Move the section pointer on.
 */
	s = s->next;
    }

    return size;
}

unsigned int write_a_section(FILE *in, FILE *file, struct scnhdr *shdr)
{
    if _SEEN('v') {
	printf("...writing out %s section of size %d at 0x%08X\n", 
	  shdr->s_name, shdr->s_size, shdr->s_paddr);
	printf("   at offset %d\n", shdr->s_scnptr);
    }
    copy_to_file(in, file, (int)(shdr->s_scnptr), (int)(shdr->s_size));
    return (shdr->s_size);
}

void copy_to_file(FILE *in, FILE *out, int offset, int size)
{
    int c;

    fseek(in, offset, SEEK_SET);
    while (size--) {
	c = fgetc(in);
	if (c == EOF) {
	    fprintf(stderr, "ERROR: unexpected EOF\n");
	    exit(0);
	}
	fputc(c, out);
    }
}

