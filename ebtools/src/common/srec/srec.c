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
#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: srec.c,v 1.1.1.1 1998/12/29 21:36:24 paradis Exp $";
#endif
/*
======================
srec.c  - generate a Moto file on stdout
======================
*/
#ifdef VMS
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stat.h>
#include "a_out.h"
#include "c_32_64.h"
#else
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "stdc.h"
#include "aout.h"
#endif


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define BLKSIZE	25

char buf[BLKSIZE];
/* struct exec e;*/

#ifndef min
#define min(a,b)                 (a>b ? b : a)
#endif

#define _SEEN(o) (seen[o-'a'])
#define SEEN_SIZE ('z' - 'a' + 10)
char seen[SEEN_SIZE];			/* upper and lower case */
/*
 *  Declare the routines in this module.
 */
int main(int argc, char **argv);
void generate_motorola_s3(unsigned int offset, FILE *in, FILE *out);


int main(int argc, char **argv)
{
    char *infile = NULL, *outfile = NULL;
    FILE *ifile = NULL, *ofile = NULL;

    unsigned int offset = 0;
    int i;
    char option, *arg;
/*
 * Parse arguments, but we are only interested in flags.
 * Skip argv[0].
 */

    for (i = 0; i < SEEN_SIZE; i++)
	seen[i] = FALSE;

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
		    case 'v': 		/* verbose */
		  printf("srec - version (%s)\n", rcsid);

		    case 'i': 		/* input file is image */
		    case 'a': 		/* input file is a.out format */
			_SEEN(option) = TRUE;
			break;
/*
 *  This is a special one, the -o option is followed (immediately)
 *  by a number, the offset of the code into the ROM.
 */
		    case 'o': 
			_SEEN(tolower(option)) = TRUE;
			sscanf(arg, "%X", &offset);
			arg = arg + strlen(arg);
			break;
		    case 'h': 
		    default: 
			printf("srec - generate Motorola S records\n");
			printf("usage: srec -v <input-file> <output-file>\n");
			printf("\t-vV = verbose, print more information than usual\n");
			printf("\t-iI = input file is an image file (no header)\n");
			printf("\t-aA = input file is a.out format\n");
			printf("\t-oO<number> = place object at this offset in the output file\n");
			printf("If input and output files are not\n");
			printf("specified, then stdin and stdout are used\n");
			exit(0);
			break;
		}
	    }
	} else {
/*
 *  This is a filename, ignore multiple filenames
 */
	    if (infile == NULL)
		infile = arg;
	    else {
		if (outfile != NULL) {
		    fprintf(stderr, "ERROR, too many files supplied\n");
		    exit(1);
		}
		outfile = arg;
	    }
	}
    }
/*
 *  Check that the combinations of arguments is valid.
 *  We allow:
 *        [-v] -a -o
 *        [-v] -i -o
 *  But not:
 *        [-v] -a -i
 */
    if (_SEEN('i') && _SEEN('a')) {
	fprintf(stderr, "ERROR: illegal combination of options\n");
	exit(1);
    }
/*
 *  If verbose has been selected reveal the arguments.
 */
    if (_SEEN('v')) {
	printf("srec - generate Motorola S Records [V1.2]\n");
	printf("...input file format is ");
	if _SEEN('a')
	    printf("a.out");
	if _SEEN('i')
	    printf("image");
	printf("\n");
    }

/*
 *  If neccessary, open the files (they may be stdin and stdout)
 */

    if (infile != NULL) {
	if (_SEEN('v'))
	    printf("...input file is %s\n", infile);
#ifdef _WIN32
	ifile = fopen(infile, "rb");
#else
	ifile = fopen(infile, "r");
#endif
	if (ifile == NULL) {
	    fprintf(stderr, "ERROR, failed to open input file %s\n", infile);
	    exit(0);
	}
    }
    if (outfile != NULL) {
	if (_SEEN('v'))
	    printf("...output file is %s\n", outfile);
	ofile = fopen(outfile, "w");
	if (ofile == NULL) {
	    fprintf(stderr, "ERROR, failed to open output file %s\n", outfile);
	    exit(0);
	}
    }


/*
 *  Now generate the motorola s3 records.
 */
    generate_motorola_s3(offset, (ifile == NULL) ? stdin : ifile, 
      (ofile == NULL) ? stdout : ofile);
/*
 *  Close any opened files.
 */
    if (ifile != NULL)
	fclose(ifile);
    if (ofile != NULL)
	fclose(ofile);
/*
 *  Exit with a good status.
 */
    return EXIT_SUCCESS;
}

void generate_motorola_s3(unsigned int offset, FILE *in, FILE *out)
{
    unsigned int b;
    unsigned int addr;
    unsigned char check;
    unsigned int i, len;

    if _SEEN('a') {			/* a.out header format */
	struct exec e;

	fread(&e, sizeof(struct exec), 1, in);

/*
 *  if the -o option was supplied, then the user wants
 *  the code based somewhere particular, otherwise use
 *  the information in the a.out header.
 */
	if _SEEN('o')
	    addr = offset;
	else
	    addr = low32(e.a_entry);

	len = low32(e.a_text) + low32(e.a_data);
    } else {
	len = BLKSIZE;
	addr = offset;			/* where offset is zero or set by the
					 * caller */
    }

    if _SEEN('v')
	printf("...placing object at offset %08X\n", addr);


    while (1) {
	if ((b = fread(buf, 1, min(len, BLKSIZE), in)) == 0)
	    break;			/* eof */

	check = 0;

	fprintf(out, "S2%02X%06X", b + 4, addr);
	check += b + 4;
	check += (addr >> 16) & 0xff;
	check += (addr >> 8) & 0xff;
	check += addr & 0xff;

/*
	for (i = 0; i < b; i += 2) {
	    fprintf(out, "%02X%02X", buf[i + 1] & 0xff, buf[i] & 0xff);
	    check += buf[i];
	    check += buf[i + 1];
*/

	for (i = 0; i < b; i += 1) {
	    fprintf(out, "%02X", buf[i] & 0xff);
	    check += buf[i];

	}
	fprintf(out, "%02X\n", ~check & 0xff);
	addr += b;

	if _SEEN('a') {
	    len -= b;
	    if (len == 0)
		break;
	}
    }
    fprintf(out, "S9030000%02X\n", ~3 & 0xff);
}
