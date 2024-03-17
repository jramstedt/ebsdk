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
#ifndef LINT
static char *rcsid = "$Id: bit.c,v 1.1.1.1 1998/12/29 21:36:20 paradis Exp $";
#endif

/*
 * Author: Franklin Hooker, Digital Equipment Corporation, July 1997
 * $Log: bit.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:20  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/11/11  17:03:12  pbell
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "ebtools.h"

char seen[SEEN_SIZE];

int errors, warnings;

#define BITWIDTH 8

file_data_t infile[BITWIDTH];
file_data_t outfile;

static char *Revision = "$Revision: 1.1.1.1 $";
static char *DateStamp = __DATE__;
static char *TimeStamp = __TIME__;

static void process_data(void);
static void usage(char **argv);

static char *banner_string = "Generates a bit-sliced image file";

int main(int argc, char **argv)
{
    int i;
    int option;

    errors = warnings = 0;

    for (i=0; i<BITWIDTH; ++i) {
      memset(&infile[i], 0, sizeof(file_data_t));
    }
    memset(&outfile, 0, sizeof(file_data_t));

    for (i=0; i<BITWIDTH; ++i) {
      infile[i].fname = NULL;
      infile[i].eof = TRUE;
    }
    outfile.fname = NULL;

    for (i=0; i<BITWIDTH; ++i) {
      infile[i].file = NULL;
    }
    outfile.file = NULL;

    for (i = 0; i < SEEN_SIZE; i++)
	seen[i] = FALSE;

    while ((option = getopt (argc, argv, "hvb:")) != EOF) {
      switch (option) {
      case 'v':	/* verbose */
	_SEEN(option) = TRUE;
	break;

      case 'h': 
	usage(argv);
	exit(EXIT_SUCCESS);
      }
    }

    i = 0;
    for ( ; optind < argc; optind++) {
      if (*argv[optind] == '-') {
	while ((option = getopt (argc, argv, "o")) != EOF) {
	  switch (option) {
	  case 'o':	/* output file */
	    _SEEN(option) = TRUE;
	    outfile.fname = argv[optind];
	    printf("Output file: %s\n", outfile.fname);
	    break;
	  }
	}
      } else {
	if (i >= BITWIDTH) {
	  fprintf(stderr, "Error: Too many input files specified\n");
	  exit(EXIT_FAILURE);
	}

	if (infile[i].fname == NULL) {
	  infile[i].eof = FALSE;
	  infile[i].fname = argv[optind];
	  printf("Input file %d: %s\n", i, infile[i].fname);
	  ++i;
	  continue;
	}
      }
    }


    if (infile[0].fname == NULL) {
      fprintf(stderr, "Error: No input file name specified\n");
      usage(argv);
      exit(EXIT_FAILURE);
    }

    if (outfile.fname == NULL) {
      fprintf(stderr, "Error: No output file name specified\n");
      usage(argv);
      exit(EXIT_FAILURE);
    }


    if (_SEEN('v')) {
      fprintf(stderr, "%s - %s [%s %s %s]\n\n",
	      argv[0], banner_string, Revision, DateStamp, TimeStamp);
    }

/*
 *  If neccessary, open the files
 */

    for (i=0; i<BITWIDTH; ++i) {
      if (infile[i].fname != NULL) {
	if (_SEEN('v'))
	  fprintf(stderr, "...Input file %d is %s\n", i, infile[i].fname);

#ifdef _WIN32
	infile[i].file = fopen(infile[i].fname, "rb");
#else
	infile[i].file = fopen(infile[i].fname, "r");
#endif

	if (infile[i].file == NULL) {
	  fprintf(stderr, "ERROR, failed to open input file %s\n", infile[i].fname );
	  exit(EXIT_FAILURE);
	}
      }
    }

    if (outfile.fname != NULL) {
      if (_SEEN('v'))
	fprintf(stderr, "...Output file is %s\n", outfile.fname);

#ifdef _WIN32
      outfile.file = fopen(outfile.fname, "wb");
#else
      outfile.file = fopen(outfile.fname, "w");
#endif

      if (outfile.file == NULL) {
	fprintf(stderr, "ERROR, failed to open input file %s\n", outfile.fname );
	exit(EXIT_FAILURE);
      }
    }

    process_data();

/*
 *  Close any opened files.
 */
    for (i=0; i<BITWIDTH; ++i) {
      if (infile[i].file != NULL)
	fclose(infile[i].file);
    }
    if (outfile.file != NULL)
	fclose(outfile.file);

    exit((errors == 0) ? EXIT_SUCCESS : EXIT_FAILURE);
}

void process_data(void)
{
  int c[BITWIDTH];
  int data;
  int i, j;
  int end_of_all_files = FALSE;

  while (end_of_all_files == FALSE) {
    end_of_all_files = TRUE;
    for (i=0; i<BITWIDTH; ++i) {
      c[i] = 0;
    }
    for (i=0; i<BITWIDTH; ++i) {
      if (infile[i].eof == TRUE) continue;

      if ((c[i] = getc(infile[i].file)) == EOF) {
	infile[i].eof = TRUE;
	continue;
      }

      end_of_all_files = FALSE;
    }

    if (end_of_all_files == TRUE)
      break;

    for (j=0; j<BITWIDTH; ++j) {
      data = 0;
      for (i=0; i<BITWIDTH; ++i) {
	data |= (((c[i] >> j) & 1) << i);
      }
      fputc(data, outfile.file);
    }

  }
}

static void usage(char **argv)
{
    fprintf(stderr, "\n%s - %s [%s %s %s]\n\n",
	    argv[0], banner_string, Revision, DateStamp, TimeStamp);
    fprintf(stderr, "> %s [options] input-file[s...] -o output-file\n\n",
	    argv[0]);

    fprintf(stderr, "\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "\t-v                Produce verbose output.\n");
    fprintf(stderr, "\t-h                Prints this help text.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "example:\n");
    fprintf(stderr, "\t%s -v srom.img srom.bit\n\n", argv[0]);
}
