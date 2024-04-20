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
**	EBxx Software Tools - sysgen
** 
** FUNCTIONAL DESCRIPTION:
** 
**	Create a system image from a HWRPB, the PALcode and
**	the system image (Debug Monitor, O/S or Firmware).
** 
**
**	[ image			]	Wherever it's based (eg 40000).	
**
**	[ padded		]	to 0x20000
**	[ l3 PT 		]	 0x2000
**	[ l2 PT for image	]	 0x2000
**	[ 1st free PFN (argv)	]	
**	[ image entry address   ]
**	[ PAL OSF		]
**
**	[ HWRPB			]	0x20000, padded
**
**	[ image header 	]		512 bytes (to be read by disk boot)
**
** CALLING ENVIRONMENT: 
**
**	user mode
** 
** AUTHOR: David A Rusling
**
** CREATION-DATE: 25-Jan-94
** 
** MODIFIED BY: 
** 
**      Who	When		What
**	-----	-----------	---------------------------------------------
**	DAR	   Jan-1994	First version.
**      KMC     18-Aug-1994     fixed new offset calculation
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "a_out.h"
#include "c_32_64.h"

/*
 *  Macros
 */
#define TRUE 1
#define FALSE 0

#define ROMMAX	0x100000
#define PALMAX	0x020000
#define RPBMAX	0x020000
#define PGSIZE	0x002000

typedef struct {
    int16 f_magic;
    int16 f_nscns;
    int32 f_timdat;
    int64 f_symptr;
    int32 f_nsyms;
    int16 f_opthdr;
    int16 f_flags;
} FHDR;

#define NFHDR	(2+2+4+8+4+2+2)
#define FMAGIC	0603

typedef struct {
    int16 a_magic;
    int16 a_vstamp;
    int32 a_pad0;
    int64 a_tsize;
    int64 a_dsize;
    int64 a_bsize;
    int64 a_entry;
    int64 a_tbase;
    int64 a_dbase;
    int64 a_bbase;
    int32 a_gmask;
    int32 a_fmask;
    int64 a_gpvalue;
} AHDR;

#define NAHDR	(2+2+4+8+8+8+8+8+8+8+4+4+8)
#define AMAGIC	0407
#define ASTAMP	23

typedef struct {
    char s_name[8];
    int64 s_paddr;
    int64 s_vaddr;
    int64 s_size;
    int64 s_scnptr;
    int64 s_relptr;
    int64 s_lnnoptr;
    int16 s_nreloc;
    int16 s_nlnno;
    int32 s_flags;
} SHDR;

#define NSHDR	(8+8+8+8+8+8+8+2+2+4)
#define SROUND	16

#if 0
#define _error(string1,string2)\
     {	\
	fprintf(stderr, "%s %s\n", string1, string2);	\
	exit(1);    \
     }
#endif

void _error(const char *string1, const char *string2)
{
	fprintf(stderr, "%s %s\n", string1, string2);
	exit(1);
}

#define _SEEN(o) seen[o-'a']
#define _OPTION(o) (_SEEN(o) == TRUE)


/*
 *  Global data
 */
#define SEEN_SIZE 100
char seen[SEEN_SIZE];			/* upper and lower case */
/*
 *  Describe the files that we have as input.
 */
#define MAX_FILES 	10

#define aout		0
#define coff		1
#define stripped 	2
#define output		3
#define unknown		4

typedef struct fileinfo {
    char *file;
    int type;
    FILE *fp;
    int64 entry;			/* entry point for this file */
    int64 newentry;			/* entry point specified by user */
    int64 tbase;
    int64 dbase;
    int64 dsize;
    int64 bsize;
    int64 tsize;
    int64 size;				/* total bytes read from this file */
    int valid : 1;
    int entrypoint_given : 1;
} fileinfo_t;
fileinfo_t files[MAX_FILES];

int pad_char = 0x00;
unsigned long datacount;
unsigned long sumcheck;
#define _fputc(a,b) \
{ \
  fputc((a),(b)); \
  datacount++; \
  sumcheck = (sumcheck + (unsigned long) a) \
    & (unsigned long) 0xffffffff; \
}

FILE *output_fp;
/*
 *  Forward routine descriptions.
 */
int main(int argc, char **argv);
void usage();
void print_filetype(int type);
FILE *parse_filehead(int m_index);
void output_file(int m_index);
void put_padding(unsigned long padding);
int64 fsize(FILE *fp);

/*
 *  External routines.
 */

int main(int argc, char **argv)
{
    char *arg, option;
    int i, findex = 0, flash_image= 0;
    long int entry = 0;
    unsigned long int padding, offset, first;
/*
 * Initialise
 */

    for (i = 0; i < SEEN_SIZE; i++)
	seen[i] = FALSE;

    for (i = 0; i < MAX_FILES; i++)
	 files[i].valid = FALSE;

/*
 * Parse arguments, but Skip argv[0].
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
		    case 'p': 		/* entry point given. */
			_SEEN(option) = TRUE;
			sscanf(arg, "%02X", &pad_char);
			arg = arg + strlen(arg);
			break;
		    case 'a': 
			_SEEN(option) = TRUE;
			files[findex].type = aout;
			break;
		    case 'c': 
			_SEEN(option) = TRUE;
			files[findex].type = coff;
			break;
		    case 'f': 
			_SEEN(option) = TRUE;
			flash_image= TRUE;
			break;
		    case 's': 
			_SEEN(option) = TRUE;
			files[findex].type = stripped;
			break;
		    case 'o': 
			_SEEN(option) = TRUE;
			files[findex].type = output;
			break;
		    case 'h': 
			usage();
			exit(1);
		    case 'v': 		/* verbose, allow upper and lower case
					 */
			_SEEN(option) = TRUE;
			break;
		    case 'e': 		/* entry point given. */
			_SEEN(option) = TRUE;
			sscanf(arg, "%X", &entry);
			files[findex].newentry = plus_a_32(zero, entry);
			files[findex].entrypoint_given = TRUE;
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
 *  Filename, keep these in an array and make sure that we do not see too
 *  many of them.
 */
	    if (findex < MAX_FILES) {
		files[findex].valid = TRUE;
		files[findex++].file = arg;
	    } else
		_error("ERROR:", "too many filenames given");
	}
    }
/*
 *  Announce ourselves.
 */
    fprintf(stderr, "sysgen, system builder [V3.1]\n");
/*
 *  Check for no files supplied.
 */
    if (findex == 0)
	_error("ERROR: no filenames given\n", "");
/*
 *  For each file, open it and work out where everything is.
 *  The side effect of this is to open the files for input.
 */
    for (i = 0; i < MAX_FILES; i++) {
	if ((files[i].type != output) && (files[i].valid))
	    files[i].fp = parse_filehead(i);
    }
/*
 *  Now, if one has been specified, open the output file.
 */
#ifdef _WIN32
    output_fp = NULL;
#else
    output_fp = stdout;
#endif
    for (i = 0; i < MAX_FILES; i++) {
	if ((files[i].type == output) && (files[i].valid)) {
	    if ((output_fp = 
	      fopen(files[i].file, "wb")) == NULL) {
		_error("ERROR: failed to open output file ", 
		  files[i].file);
	    }
	}
    }
#ifdef _WIN32
    if (output_fp == NULL)
	_error("ERROR:", " you must supply an output file name");
#endif
/*
 *  If the user asked for details, then give him/her details.
 */
    if (_OPTION('v')) {
	fprintf(stderr, "Padding byte: (0x%02x):\n", pad_char);
	fprintf(stderr, "Files are:\n");
	for (i = 0; i < MAX_FILES; i++) {
	    if (files[i].valid) {
		fprintf(stderr, "\t%s: (", files[i].file);
		print_filetype(files[i].type);
		fprintf(stderr, "), entry = 0x%08X,", low32(files[i].entry));
		fprintf(stderr, "%d text, %d data\n", 
		  low32(files[i].tsize), 
		  low32(files[i].dsize));
	    }
	}
	fprintf(stderr, "\n");
    }
/*
 *  Now concatenate each object file together to form the
 *  final image.  Between each file there is padding.  
 *  Note that we treat the first file differently just in
 *  case it's not based at zero.
 */
    padding = 0;
    offset = 0;
    first = TRUE;
    sumcheck = 0;
    datacount = 0;

    for (i = 0; i < MAX_FILES; i++) {
	if ((files[i].type != output) && (files[i].valid)) {
/*
 *  We have a valid file (image).  If it is not the first image
 *  work out how much  padding to put out before it.  We assume that 
 *  the entrypoint is at the base of the image and that if
 *  no entrypoint has been supplied, then we just concatenate
 *  the files.
 */
	    if (first) {
		offset = (unsigned int)low32(files[i].entry);
		if (flash_image)
	    	  put_padding(offset);
		padding = 0;
		first = FALSE;
	    } else {
		if (files[i].entrypoint_given) {
		    if ((unsigned int)low32(files[i].entry) < offset) {
			fprintf(stderr, "entry = 0x%X, offset = 0x%X\n", 
			  low32(files[i].entry), offset);
			_error("ERROR: image overlap", files[i].file);
		    }
		    padding = (unsigned int)low32(files[i].entry) - offset;
		} else {
		    if (files[i].type == aout)
			padding = 0;
		    else
			padding = low32(files[i].entry) - offset;
		}
	    }
/*
 *  Tell the world what we're doing.
 */
	    fprintf(stderr, "%08x %08x %08x\t%s pad, base/entry, size\n", 
	      padding, low32(files[i].entry), 
	      low32(files[i].size), files[i].file);

	    put_padding(padding);
	    output_file(i);
/*
 *  Now move offset on.  The new offset is comprised of the
 *  old offset, the file size and the padding.  (curewitz)
 */
	    offset += low32(files[i].size) + padding;
	}
    }

    printf(" ----- Data sum = %08X   Data size = %d (0x%X, %3.2f KB) -----\n\n", 
	   sumcheck, datacount, datacount, (float) datacount/1024);

/*
 *  exit normally.
 */
    fclose(output_fp);
   
    return EXIT_SUCCESS;

}					/* end of main() */

/****************************************************************
 * Put out some padding                                         *
 ****************************************************************/
void put_padding(unsigned long padding)
{
    while (padding--)
	_fputc(pad_char, output_fp);
}
/****************************************************************
 * Parse a file header (of whatever type)                       *
 ****************************************************************/
/*
 * Parse the header of the file given (via the files array).
 * and put the details in the files array.
 */
FILE *parse_filehead(int m_index)
{
    FILE *fp = NULL;

/*
 *  Initialise the information in the files vector.
 */

    assign64(files[m_index].tbase, zero);
    assign64(files[m_index].tsize, zero);
    assign64(files[m_index].dbase, zero);
    assign64(files[m_index].dsize, zero);
    assign64(files[m_index].bsize, zero);
    assign64(files[m_index].size, zero);
    assign64(files[m_index].entry, zero);

    switch (files[m_index].type) {
/*
 *  a.out file produced by GNU Assembler (GAS).
 */
	case aout: 
	    {
		struct exec *head;

		head = (struct exec *) malloc(sizeof(struct exec));
		if (head == 0)
		    _error("ERROR", 
		      "Unable to allocate space for a.out header\n");

		if ((fp = fopen(files[m_index].file, "rb")) == NULL)
		    _error("ERROR: failed to open file ", 
		      files[m_index].file);
		if (fread(head, sizeof(struct exec), 1, fp) != 1
		  || N_BADMAG(*head))
		    _error("ERROR: bad a.out header", files[m_index].file);
		files[m_index].tbase = plus_a_32(zero, N_TXTOFF(*head));
		assign64(files[m_index].tsize, head->a_text);
		files[m_index].dbase = plus_a_32(zero, N_DATOFF(*head));
		assign64(files[m_index].dsize, head->a_data);
		assign64(files[m_index].bsize, head->a_bss);
		if (files[m_index].entrypoint_given) {
		    assign64(files[m_index].entry, files[m_index].newentry);
		} else {
		    assign64(files[m_index].entry, head->a_entry);
		}
		files[m_index].size = 
		  plus(files[m_index].tsize, files[m_index].dsize);
		break;
	    }
/*
 *  File that has had any headers stripped and now contains just
 *  the image itself.
 */
	case stripped: 
	    {
		if ((fp = fopen(files[m_index].file, "rb")) == NULL)
		    _error("ERROR: failed to open file ", 
		      files[m_index].file);
		if (files[m_index].entrypoint_given) {
		    assign64(files[m_index].entry, files[m_index].newentry);
		} else
		    _error("ERROR:",
		      "no entrypoint given for a stripped file");
		assign64(files[m_index].size, fsize(fp));
		break;
	    }
	case coff: 
	    {
		FHDR *fhdr;
		AHDR *ahdr;

		fhdr = (FHDR *) malloc(sizeof(FHDR));
		ahdr = (AHDR *) malloc(sizeof(AHDR));
		if ((fhdr == NULL) || (ahdr == NULL))
		    _error("ERROR:", "failed to allocate memory");
		fp = fopen(files[m_index].file, "rb");
		if (fp == (FILE *) NULL)
		    _error("ERROR: failed to open file", 
		      files[m_index].file);
		if (fread((char *) fhdr, NFHDR, 1, fp) != 1 || 
		  fhdr->f_magic != FMAGIC)
		    _error("ERROR: bad file header", 
		      files[m_index].file);
		if (fread((char *) ahdr, NAHDR, 1, fp) != 1 || 
		  ahdr->a_magic != AMAGIC)
		    _error("ERROR: bad a.out header", 
		      files[m_index].file);
		if (ahdr->a_vstamp < ASTAMP)
		    _error("ERROR: ancient a.out version", 
		      files[m_index].file);
		files[m_index].tbase = plus_a_32(zero, 
		  ((NFHDR + NAHDR + fhdr->f_nscns * NSHDR
		  + (SROUND - 1)) & ~(SROUND - 1)));
		assign64(files[m_index].tsize, ahdr->a_tsize);
		files[m_index].dbase = plus(files[m_index].tbase, 
		  files[m_index].tsize);
		assign64(files[m_index].dsize, ahdr->a_dsize);
		assign64(files[m_index].bsize, ahdr->a_bsize);
		if (files[m_index].entrypoint_given) {
		    assign64(files[m_index].entry, files[m_index].newentry);
		} else {
		    assign64(files[m_index].entry, ahdr->a_entry);
		}
		files[m_index].size = 
		  plus(files[m_index].tsize, files[m_index].dsize);
		break;
	    }
	default: 
	    _error("ERROR: unknown file type", files[m_index].file);
    }
    return fp;
}

/****************************************************************
 * Output a file (of whatever type)                             *
 ****************************************************************/
void output_file(int m_index)
{
    int i;
    int c;
    int status;

    if (files[m_index].type != stripped) {
	status = fseek(files[m_index].fp, low32(files[m_index].tbase), 0);
	if (status != 0)
	    _error("ERROR: fseek() failure on file ", 
	      files[m_index].file);
	for (i = 0; i < low32(files[m_index].tsize); ++i) {
	    if ((c = getc(files[m_index].fp)) == EOF)
		abort();
	    _fputc(c, output_fp);
	}

	/* Data segment */
	status = fseek(files[m_index].fp, low32(files[m_index].dbase), 0);
	if (status != 0)
	    _error("ERROR: fseek() failure on file ", 
	      files[m_index].file);
	for (i = 0; i < low32(files[m_index].dsize); ++i) {
	    if ((c = getc(files[m_index].fp)) == EOF)
		abort();
	    _fputc(c, output_fp);
	}

    } else {
	while ((c = getc(files[m_index].fp)) != EOF)
	    _fputc(c, output_fp);
    }

    fclose(files[m_index].fp);
}
/****************************************************************
 * What sort of file is it?                                     *
 ****************************************************************/
void print_filetype(int type)
{
    switch (type) {
	case coff: 
	    fprintf(stderr, "coff");
	    break;
	case aout: 
	    fprintf(stderr, "a.out");
	    break;
	case stripped: 
	    fprintf(stderr, "stripped");
	    break;
	case unknown: 
	    fprintf(stderr, "unknown");
	    break;
	case output: 
	    fprintf(stderr, "output");
	    break;
	default: 
	    fprintf(stderr, "programming error! (%d)", type);
    }
}

/****************************************************************
 * How do you use this thing?                                   *
 ****************************************************************/
void usage()
{
    fprintf(stderr, "sysgen - generates system images\n\n");
    fprintf(stderr, "> sysgen [flags] [[fileflags] file] ...");
    fprintf(stderr, " [[fileflags] file]\n\n");
    fprintf(stderr, "\tWhere [fileflags] is one of -o, -e, -a, -c -p or -s:\n");
    fprintf(stderr, "\t\t-a: a.out file (produced by GAS)\n");
    fprintf(stderr, "\t\t-c: OSF/1 coff object file\n");
    fprintf(stderr, "\t\t-s: stripped file (no header)\n");
    fprintf(stderr, "\t\t-p: specifies padding byte\n");
    fprintf(stderr,
      "\t\t-eNNN: override/supply the entrypoint/base of an image\n");
    fprintf(stderr, "\t\t-o: file is the output file\n");
    fprintf(stderr, "\tThe files default to type -a\n\n");
#ifndef _WIN32
    fprintf(stderr, "\t\tIf output file is not supplied stdout is used\n");
#endif
    fprintf(stderr, "\tFlags: -hv\n");
    fprintf(stderr, "\t\t-h: print this help text\n");
    fprintf(stderr, "\t\t-v: verbose\n");
    fprintf(stderr, "Example\n\n");
    fprintf(stderr, "\tsysgen -v -a dbmpal -a -e40000 dbmhwrpb ");
    fprintf(stderr, "-s eb66_rom.nh -o eb66_rom.img\n\n");
    fprintf(stderr, "NOTE: the filenames may appear in any order.\n");
    fprintf(stderr,
      "      Stripped files must have an entrypoint/base specified\n\n");
}

int64 fsize(FILE *fp)
{

    int64 size;

    assign64(size, zero);
    fseek(fp, 0, 0);
    while (getc(fp) != EOF)
	 size = plus_a_32(size, 1);
    fseek(fp, 0, 0);
    return size;
}
