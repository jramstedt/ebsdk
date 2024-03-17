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
static char *rcsid = "$Id: chain2cbox.c,v 1.2 1999/01/21 19:06:23 gries Exp $";
#endif

/*
 * Author: Franklin Hooker, Digital Equipment Corporation, July 1997
 *
 * $Log: chain2cbox.c,v $
 * Revision 1.2  1999/01/21 19:06:23  gries
 * First Release to cvs gries
 *
 * Revision 1.5  1997/12/22  18:19:21  quinn
 * changes for PASS-2 Cbox write_once chain
 *
 * Revision 1.4  1997/07/07  17:55:56  fdh
 * Added global definitions for command line parsing.
 *
 * Revision 1.3  1997/07/07  15:01:59  fdh
 * Comments...
 *
 * Revision 1.2  1997/07/07  01:24:28  fdh
 * Options are now case-sensitive.
 *
 * Revision 1.1  1997/07/07  00:50:05  fdh
 * Initial revision
 *
 */

#include "ic4mat.h"

char seen[SEEN_SIZE];

char * getopt_option_b = NULL;
char * getopt_option_m = NULL;
char * getopt_option_p = NULL;
char * cputype_option = NULL;

file_data_t infile;
file_data_t dxefile;
file_data_t outfile;
file_data_t lisfile;


static char *Revision = "$Revision: 1.2 $";
static char *DateStamp = __DATE__;
static char *TimeStamp = __TIME__;

int errors, warnings, ev6_passnum;

static void process_data(void);
static void usage(char **argv);

static char *banner_string = "Generates ic4mat input Cbox file";

int main(int argc, char **argv)
{
    int i;
    int option;

    errors = warnings = 0;

    ev6_passnum = DEFAULT_EV6_PASSNUM;

    memset(&infile, 0, sizeof(file_data_t));
    memset(&outfile, 0, sizeof(file_data_t));
    memset(&lisfile, 0, sizeof(file_data_t));

    infile.fname = NULL;
    outfile.fname = NULL;
    lisfile.fname = NULL;

    infile.file = NULL;
    outfile.file = NULL;
    lisfile.file = NULL;

    for (i = 0; i < SEEN_SIZE; i++)
	seen[i] = FALSE;

    while ((option = getopt (argc, argv, "vl:a:p:h")) != EOF) {
      switch (option) {
      case 'v':	/* verbose */
	_SEEN(option) = TRUE;
	break;

      case 'l':	/* Produce listing file */
	_SEEN(option) = TRUE;
	lisfile.fname = optarg;
	break;

      case 'a':	/* Use alias CSR names */
	_SEEN(option) = TRUE;
	break;

      case 'p':	/* EV6 Pass Number */
	_SEEN(option) = TRUE;
	getopt_option_p = optarg;
        ev6_passnum = strtol(getopt_option_p, (char**)NULL, 10);
	break;

      case 'h': 
	usage(argv);
	exit(EXIT_SUCCESS);
      }
    }

    for ( ; optind < argc; optind++) {
      if (infile.fname == NULL) {
	infile.fname = argv[optind];
	continue;
      }

      if (outfile.fname == NULL) {
	outfile.fname = argv[optind];
	continue;
      }
    }

    if (infile.fname == NULL) {
      fprintf(stderr, "Error: No CHAIN file name specified\n");
      usage(argv);
      exit(EXIT_FAILURE);
    }

    if (outfile.fname == NULL) {
      fprintf(stderr, "Error: No output file name specified\n");
      usage(argv);
      exit(EXIT_FAILURE);
    }


    if (_SEEN('v')) {
      fprintf(stderr, "%s - %s[%s %s %s]\n\n",
	      argv[0], banner_string, Revision, DateStamp, TimeStamp);
      fprintf(stderr, "EV6 Pass = %d\n\n", ev6_passnum);
    }

    if (_SEEN('v'))
      fprintf(stderr, "Opening files...\n\n");

/*
 *  If neccessary, open the files
 */

    if (infile.fname != NULL) {
	if (_SEEN('v'))
	  fprintf(stderr, "...CHAIN file is %s\n", infile.fname);

	infile.file = fopen(infile.fname, "r");

	if (infile.file == NULL) {
	  fprintf(stderr, "ERROR, failed to open CHAIN file %s\n", infile.fname );
	  exit(EXIT_FAILURE);
	}
    }

    if (lisfile.fname != NULL) {
	if (_SEEN('v'))
	  fprintf(stderr, "...List file is %s\n", lisfile.fname);

	lisfile.file = fopen(lisfile.fname, "w");

	if (lisfile.file == NULL) {
	  fprintf(stderr, "ERROR, failed to open file %s\n", lisfile.fname );
	  exit(EXIT_FAILURE);
	}

	fprintf(lisfile.file, "/*\n** %s - %s [%s %s %s]\n\n",
		argv[0], banner_string, Revision, DateStamp, TimeStamp);
        fprintf(lisfile.file, "EV6 Pass = %d\n*/\n", ev6_passnum);
    }

    if (outfile.fname != NULL) {
	if (_SEEN('v'))
	  fprintf(stderr, "...Output file is %s\n", outfile.fname);

	outfile.file = fopen(outfile.fname, "w");

	if (outfile.file == NULL) {
	  fprintf(stderr, "ERROR, failed to open file %s\n", outfile.fname );
	  exit(EXIT_FAILURE);
	}
    }

    if (_SEEN('v'))
      fprintf(stderr, "Processing data...\n\n");

    process_data();

    if (_SEEN('v'))
      fprintf(stderr, "Closing files...\n\n");

/*
 *  Close any opened files.
 */
    if (infile.file != NULL)
	fclose(infile.file);
    if (outfile.file != NULL)
	fclose(outfile.file);
    if (lisfile.file != NULL)
	fclose(lisfile.file);

    if (_SEEN('v'))
      fprintf(stderr, "Errors:%d  Warnings:%d \n\n", errors, warnings);

    exit((errors == 0) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void process_data(void)
{
  convert_ev6_chain_to_cbox();
}

void report_error(char *trailing_comment, char *message,  char *fmt, ...)
{
  va_list args;

  fprintf(stderr, "%s %s, line %d:",
	  infile.fname, message, infile.linenum);

  va_start(args, fmt);
  vfprintf(stderr,	fmt, args);
  if (_SEEN('l')) {
    vfprintf(lisfile.file,	fmt, args);
  }
  va_end(args);

  fprintf(stderr, "%s", infile.line);
  if (trailing_comment != NULL)
    fprintf(stderr, ";%s", trailing_comment+1);
  fprintf(stderr, "\n\n");
}

static void usage(char **argv)
{
    fprintf(stderr, "\n%s - %s [%s %s %s]\n\n",
	    argv[0], banner_string, Revision, DateStamp, TimeStamp);
    fprintf(stderr, "> %s [options] chain-file output-file\n\n",
	    argv[0]);

    fprintf(stderr, "Converts a text file containing an EV6 write-once\n");
    fprintf(stderr, "chain value into an ic4mat input Cbox file.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "\t-v                Produce verbose output.\n");
    fprintf(stderr, "\t-l list-file      (21264 only) Generates a list-file based\n");
    fprintf(stderr, "\t                  on the CBOX file read.\n");
    fprintf(stderr, "\t-a                (21264 only) Places CBOX register alias\n");
    fprintf(stderr, "\t                  names into the list-file\n");
    fprintf(stderr, "\t                  instead of the standard register names.\n");
    fprintf(stderr, "\t-p cpu-pass       (21264 only) EV6 Pass number (default=%d).\n",DEFAULT_EV6_PASSNUM);
    fprintf(stderr, "\t-h                Prints this help text.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "example:\n");
    fprintf(stderr, "\t%s -v -l test.lis test.chain test.cbox\n\n", argv[0]);
}

