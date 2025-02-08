/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
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
**	ED64 Software Tools - clist
** 
** FUNCTIONAL DESCRIPTION:
** 
**      Generate a list file from a coff image file.  This
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
**      Who	When		What
**	-----	-----------	---------------------------------------------
**	DAR	   Nov-1992	First version.
*/
#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: ccheck.c,v 1.1.1.1 1998/12/29 21:36:23 paradis Exp $";
#endif
/*
typedef long vm_offset_t;
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#include <windef.h>

/* Convert the NT types to the names used in coff.h */

#define ALPHAMAGIC IMAGE_FILE_MACHINE_ALPHA

typedef IMAGE_FILE_HEADER filehdr_t;
#define f_magic Machine
#define f_nscns NumberOfSections
#define f_opthdr SizeOfOptionalHeader

typedef IMAGE_OPTIONAL_HEADER aouthdr_t;
#define magic Magic
#define tsize SizeOfCode
#define dsize SizeOfInitializedData
#define bsize SizeOfUninitializedData
#define entry AddressOfEntryPoint
#define text_start BaseOfCode
#define data_start BaseOfData

typedef IMAGE_SECTION_HEADER scnhdr_t;
#define s_name Name
#define s_scnptr PointerToRawData
#define s_size SizeOfRawData	
#define s_paddr Misc.PhysicalAddress
#define s_vaddr VirtualAddress

#define SHDR_SIZE IMAGE_SIZEOF_SECTION_HEADER

/* It looks as if all sizes on NT are rounded to a multiple of 512 bytes */

#define N_TXTOFF(f, a) ((sizeof(filehdr_t) + (f).f_opthdr + (f).f_nscns * SHDR_SIZE + 511) & ~511)
#else

#include "stdc.h"
#include "coff.h"
#include "syms.h"
#include "reloc.h"

typedef struct aouthdr aouthdr_t;
typedef struct filehdr filehdr_t;
typedef struct scnhdr scnhdr_t;

#define SHDR_SIZE (sizeof(scnhdr_t)) 

#endif

/*
 *  Macros
 */
#define TRUE 1
#define FALSE 0
#define BLOCK_SIZE 1000

#define _debug(name,string)\
     {if (_SEEN('d')) \
	printf("debug: %s %s\n", name, string);	\
     }

#define _ddebug(name,string,number)\
     {if (_SEEN('d')) \
	printf("debug: %s %s %d\n", name, string,number);	\
     }

#define _error(string1,string2)\
     {	\
	printf("error: %s %s\n", string1, string2);	\
	exit(1);    \
     }

#define _SEEN(o) (seen[o-'a'])
/*
 *  Global data (all names preceded by 'coff_g'.
 */
FILE *coff_g_infile;
char *coff_g_buffer = NULL;
unsigned int coff_g_buffer_size = 0;
#define SEEN_SIZE ('z' - 'a' + 10)
char seen[SEEN_SIZE];			/* upper and lower case */
/*
 *  Forward routine descriptions.
 */
void usage();
void read_file(char *filename);
void analyse_coff_file();

int main(int argc, char **argv)
{
    char *in_file_name = "a.out";	/* default .o file */
    char *arg, option;
    int i;

    for (i = 0; i < SEEN_SIZE; i++)
	seen[i] = FALSE;
/*
 * Parse arguments, but we are only interested in flags.
 * Skip argv[0].
 */
    for (i = 1; i < argc; i++) {
	_debug("main()", "argument loop");
	arg = argv[i];
	if (*arg == '-') {
/*
 * This is a -xyz style options list.  Work out the options specified.
 */
	    arg++;			/* skip the '-' */
	    while (option = *arg++) {	/* until we reach the '0' string
					 * terminator */
		_debug("main()", "option loop");
		switch (option) {
		    case 'v': 		/* verbose */
		    case 'V': 
		  printf("ccheck - version (%s)\n", rcsid);

		    case 'e': 		/* entry points (for PVC) */
		    case 'E': 
		    case 'm': 		/* map (pvc) symbols (for PVC) */
		    case 'M': 
		    case 'f': 		/* full, don't skip zero locations */
		    case 'F': 
		    case 'b': 		/* just print out the bare bones */
		    case 'B': 
			_SEEN(tolower(option)) = TRUE;
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
	    in_file_name = arg;
	    _debug("main(), filename is ", in_file_name);
	}
    }
/*
 *  If verbose has been selected reveal the arguments.
 */
    if _SEEN('v') {
	char c;

	printf("arguments : ");
	for (c = 'a'; c < 'z'; c++)
	    if _SEEN(c)
		printf("%c", c);
	printf("\n");
    }
/*
 *  Read in the coff file and save it in memory before formatting.
 */
    read_file(in_file_name);
    if (!_SEEN('e') && !_SEEN('m')) {
	printf("Dump of coff file %s of size %d bytes\n", in_file_name, 
	  coff_g_buffer_size);
    }

/*
 *  Now go and analyse the file contents.
 */
    analyse_coff_file();
}					/* end of main() */

void usage()
{
    printf("clist - description\n");
    printf("\tDump the contents of a coff format object file\n");
    printf("clist - usage : alist -dDvVeEmMhH <filename>\n");
    printf("\tNOTE: Upper and Lower case options are treated equally\n");
    printf("\td,D = turn on debug mode\n");
    printf("\tv,V = turn on verbose mode\n");
    printf("\te,E = print entry points (for PVC)\n");
    printf("\tm,M = print map information (for PVC)\n");
    printf("\tf,F = print full information (no skipping zero locations)\n");
    printf("\th,H = print this help file\n");
}

void read_file(char *filename)
{
    char *newbuffer, *p;
    unsigned int size;

    if ((coff_g_infile = fopen(filename, "rb")) == NULL)
	_error("cannot open file ", filename);
/*
 *  Read the file into memory, block by block, allocating as we
 *  go along.
 */
    while (TRUE) {
	newbuffer = (char *) malloc(coff_g_buffer_size + BLOCK_SIZE);
	if (coff_g_buffer_size != 0) {
	    memcpy(newbuffer, coff_g_buffer, coff_g_buffer_size);
	    free(coff_g_buffer);
	}
	size = fread(newbuffer + coff_g_buffer_size, sizeof(char) , 
	  BLOCK_SIZE, coff_g_infile);
	coff_g_buffer = newbuffer;
	if (size == 0)
	    return;			/* we've finished */
	else {
	    coff_g_buffer_size = coff_g_buffer_size + size;
	}
    }
}
void analyse_coff_file()
{
    filehdr_t *fhdr;
    aouthdr_t *ohdr;
    char section_name[9];
    int i;
    int used = 0;
    int textoff;
    int dataoff;
    int dataendoff;

/*
 *  first, print out the file header
 */

    fhdr = (filehdr_t *) coff_g_buffer;
    printf("File Header (filehdr), after %d bytes\n", used);
    printf("\tf_magic \t= 0%o (octal)\n", fhdr->f_magic);
    if(fhdr->f_magic != ALPHAMAGIC)
      printf("******** Not an Alpha object file ***********\n");
    printf("\tf_nscns \t= %d (number of sections)\n", fhdr->f_nscns);
    printf("\tf_opthdr \t= 0x%X (%d)(size of optional header)\n", 
	   fhdr->f_opthdr, fhdr->f_opthdr);

    used = sizeof(filehdr_t);
/*
 *  Now print out the optional header.  Save the used offset so that the 
 *  verbose dump can happen.
 */
    ohdr = (aouthdr_t *) (coff_g_buffer + used);
    printf("Optional Header (aouthdr), after %d bytes\n", used);
    printf("\tmagic \t\t= 0%o (octal)", ohdr->magic);
#ifndef _WIN32
    switch(ohdr->magic) {
    case OMAGIC:
	printf("OMAGIC file\n");
	break;
    case NMAGIC:
	printf("NMAGIC file\n");
	break;
    case ZMAGIC:
	printf("ZMAGIC file\n");
	break;
    default:
	printf("**** Unknown optional header magic number\n ****");
    }
#else
    printf("\n");
#endif
    printf("\ttsize \t\t= %d ( 0x%x ) (text size in bytes, padded)\n", 
	   ohdr->tsize,ohdr->tsize);
    printf("\tdsize \t\t= %d  ( 0x%x ) (initialized data size in bytes, padded)\n", 
	   ohdr->dsize,ohdr->dsize);
    printf("\tbsize \t\t= %d  ( 0x%x ) (uninitialized data size in bytes, padded)\n", 
	   ohdr->bsize,ohdr->bsize);
    printf("\tentry \t\t= 0x%X (entry point)\n", ohdr->entry);
    printf("\ttext_start \t\t= 0x%X = %d\n", ohdr->text_start, ohdr->text_start);
    printf("\tdata_start \t\t= 0x%X = %d\n", ohdr->data_start, ohdr->data_start);


    used = used+fhdr->f_opthdr;

    /* Now do some calculations based on this data */
    printf("\nCalculated file offsets:\n");

    textoff = N_TXTOFF(*fhdr, *ohdr);
    dataoff = textoff + ohdr->tsize;
    dataendoff = dataoff+ohdr->dsize;

    printf("\tExpected text SEGMENT offset = %d, ( = 0%o, 0x%x)\n", textoff, textoff, textoff);
    printf("\tExpected data SEGMENT offset = %d, ( = 0%o, 0x%x)\n", dataoff, dataoff, dataoff);
    printf("\tExpected data SEGMENT end = %d, ( = 0%o, 0x%x)\n", dataendoff, dataendoff, dataendoff);
    
    /* Print out the section headers */
    printf("\n Section Headers \n");

    for(i=0; i<fhdr->f_nscns; i++) {
	scnhdr_t * shdr = (scnhdr_t *) (coff_g_buffer + used);

	printf("\nSection number %d at offset %d \n",i, used);
	memcpy(section_name,  shdr -> s_name, 8);
	section_name[8] = 0;
	printf("Name = %s\n", section_name);
	printf("\nFile data offset %d ( 0x%x )\n", shdr -> s_scnptr, shdr -> s_scnptr);
	printf("File data size %d ( 0x%x )\n", shdr -> s_size, shdr -> s_size);
	printf("File data end %d ( 0x%x)\n", shdr -> s_size + shdr -> s_scnptr, shdr -> s_size + shdr -> s_scnptr);
	printf("Physical address %x\n", shdr -> s_paddr);
	printf("Virtual address %x\n", shdr -> s_vaddr);
	if(shdr -> s_scnptr == 0) {
	    printf("No data in file for segment\n");
	}
	else {
	    
	    if(shdr -> s_scnptr < textoff) 
	      printf("******** Starts before start of text segment **********\n");
	    else if(shdr -> s_scnptr < dataoff) 
	      printf("Starts in text segment\n");
	    else if(shdr -> s_scnptr < dataendoff)
	      printf("Starts in data segment\n");
	    else
	      printf("******** Starts after end of data segment **********\n");
	    
	    if(shdr -> s_scnptr + shdr -> s_size <= textoff) 
	      printf("******** Ends before start of text segment **********\n");
	    else if(shdr -> s_scnptr + shdr -> s_size <= dataoff) 
	      printf("Ends in text segment\n");
	    else if(shdr -> s_scnptr + shdr -> s_size <= dataendoff)
	      printf("Ends in data segment\n");
	    else
	      printf("******** Ends after end of data segment **********\n");
	    
	    if((shdr -> s_scnptr < dataoff) && (shdr -> s_scnptr + shdr ->s_size > dataoff))
	      printf("******Starts and ends in different segments*********\n");
	}
	used += SHDR_SIZE;
    }

}
