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
**	ED64 Software Tools - ntstrip
** 
** FUNCTIONAL DESCRIPTION:
** 
**      Strips everything off an NT ROM file that is not needed for download.
** 
** CALLING ENVIRONMENT: 
**
**	user mode
** 
** AUTHOR: Anthony Berent
**
** CREATION-DATE: 8-Sept-1993
** 
** MODIFIED BY: 
** 
**      Who	When		What
**	-----	-----------	---------------------------------------------------
**	ANB	8-Sept-1993	First version, partly derived from clist and cstrip.
*/
#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: newstrip.c,v 1.1.1.1 1998/12/29 21:36:23 paradis Exp $";
#endif
/*
typedef long vm_offset_t;
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
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

typedef IMAGE_SECTION_HEADER scnhdr_t;
#define s_name Name
#define s_scnptr PointerToRawData
#define s_size SizeOfRawData	
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
static unsigned int coff_g_buffer_size = 0;
#define SEEN_SIZE ('z' - 'a' + 10)
char seen[SEEN_SIZE];			/* upper and lower case */
/*
 *  Forward routine descriptions.
 */
void usage();
void read_file(char *filename);
void analyse_coff_file(char ** loadstart, unsigned long int * loadsize);
void write_stripped_file(char * filename, char * loadstart, unsigned long int loadsize);

int main(int argc, char **argv)
{
    char *ifname = NULL;
    char *ofname = "a_out.sti";
    char *arg, option;
    char * loadstart;
    unsigned long int loadsize;
    int i;

    for (i = 0; i < SEEN_SIZE; i++)
	seen[i] = FALSE;
/*
 * Parse arguments, but we are only interested in flags.
 * Skip argv[0].
 */
    for (i = 1; i < argc; i++) {
	arg = argv[i];
	_debug("main()", "argument loop");
	_debug("arg =", arg);
	if (*arg == '-') {
/*
 * This is a -xyz style options list.  Work out the options specified.
 */
	    arg++;			/* skip the '-' */
	    while (option = *arg++) {	/* until we reach the '0' string
					 * terminator */
		_debug("main()", "option loop");
		switch (option) {
		case 'd':               /* debug */
		case 'D':
		case 'v': 		/* verbose */
		case 'V': 
		    _SEEN(tolower(option)) = TRUE;
		    printf("newstrip - version (%s)\n", rcsid);
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
    _debug("main()", "arguments read");
    _debug("ifname =", ifname);
    _debug("ofname =", ofname);

/*
 *  Read in the coff file and save it in memory before formatting.
 */
    read_file(ifname);
    if (!_SEEN('e') && !_SEEN('m')) {
	printf("Strip of coff file %s of size %d bytes\n", ifname, 
	  coff_g_buffer_size);
    }

/*
 *  Now go and analyse the file contents.
 */
    analyse_coff_file(&loadstart, &loadsize);
    write_stripped_file(ofname,loadstart,loadsize);
}					/* end of main() */

void usage()
{
    printf("clist - description\n");
    printf("\tDump the contents of a coff format object file\n");
    printf("clist - usage : alist -dDvVeEmMhH <filename>\n");
    printf("\tNOTE: Upper and Lower case options are treated equally\n");
    printf("\tv,V = turn on verbose mode\n");
}

void read_file(char *filename)
{
    char *newbuffer, *p;
    unsigned int size;

    _debug("read_file()", "start"); 
    _debug("file name = ", filename );
    if ((coff_g_infile = fopen(filename, "rb")) == NULL)
	_error("cannot open file ", filename);
    _debug("read_file()", "file open");
/*
 *  Read the file into memory, block by block, allocating as we
 *  go along.
 */
    while (TRUE) {
	_debug("read_file()", "block read loop");
	newbuffer = (char *) malloc(coff_g_buffer_size + BLOCK_SIZE);
	if (coff_g_buffer_size != 0) {
	    memcpy(newbuffer, coff_g_buffer, coff_g_buffer_size);
	    free(coff_g_buffer);
	}
	size = fread(newbuffer + coff_g_buffer_size, 1 , 
	  BLOCK_SIZE, coff_g_infile);
	coff_g_buffer = newbuffer;
	if (size == 0)
	    return;			/* we've finished */
	else {
	    coff_g_buffer_size = coff_g_buffer_size + size;
	}
    }
    _debug("read_file()", "end");
}
void analyse_coff_file(char ** loadstart, unsigned long int * loadsize)
{
    filehdr_t *fhdr;
    aouthdr_t *ohdr;
    char section_name[9];
    int i;
    int used = 0;
    unsigned long int textoff;
    unsigned long int dataoff;
    unsigned long int dataendoff;
    unsigned long int loadaddr = ULONG_MAX;

/*
 *  first, check the file header
 */

    fhdr = (filehdr_t *) coff_g_buffer;
    if(fhdr->f_magic != ALPHAMAGIC)
      printf("******** Not a ROMable Alpha image file ***********\n");

    used = sizeof(filehdr_t);
/*
 *  Now find the optional header. 
 */
    ohdr = (aouthdr_t *) (coff_g_buffer + used);
    if _SEEN('v') {
	printf("Entry Point \t\t= 0x%X \n", ohdr->entry);
    }

    used = used+fhdr->f_opthdr;

    /* Now do some calculations based on this data */

    textoff = N_TXTOFF(*fhdr, *ohdr);
    dataoff = textoff + ohdr->tsize;
    dataendoff = dataoff+ohdr->dsize;

    /* Check out the section headers */

    for(i=0; i<fhdr->f_nscns; i++) {
	scnhdr_t * shdr = (scnhdr_t *) (coff_g_buffer + used);
	if(shdr -> s_scnptr != 0) {
	    memcpy(section_name,  shdr -> s_name, 8);
	    section_name[8] = 0;
	    if((shdr -> s_vaddr != 0) && (shdr -> s_vaddr < loadaddr)) loadaddr = shdr -> s_vaddr;
	    if(shdr -> s_scnptr < textoff) 
	      printf("******** Section %s starts before start of text segment **********\n", section_name);
	    if((shdr -> s_scnptr < dataoff) && (shdr -> s_scnptr + shdr ->s_size > dataoff) ||
	       (shdr -> s_scnptr < dataendoff) && (shdr -> s_scnptr + shdr ->s_size > dataendoff))
	      printf("******Section %s starts and ends in different segments*********\n", section_name);
	}
	used += SHDR_SIZE;
    }
    *loadstart = coff_g_buffer + textoff;
    *loadsize =  dataendoff - textoff;
/*
 *  If verbose has been selected print the size.
 */
    if _SEEN('v') {
	printf("Size of stripped file = %d (0x%x)\n", *loadsize, *loadsize);
	printf("Expected load address \t = 0x%X \n", loadaddr);
    }
}
void write_stripped_file(char *filename, char * loadstart, unsigned long int loadsize)
{
    FILE *ofile;

    if ((ofile = fopen(filename,"wb")) == NULL) {
	printf("ERROR: cannot open file - %s\n",filename);
	exit(1) ;
    }
    if(fwrite(loadstart, 1, loadsize, ofile) != loadsize) {
	printf("ERROR: failed to write stripped executable\n");
    }
}











