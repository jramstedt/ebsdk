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
__attribute__((unused)) static const char *rcsid = "$Id: rcsv.c,v 1.2 1999/02/09 19:56:07 gries Exp $";
#endif

/*
**
** FACILITY:	
**
**	ED64 Software Tools - Encodes RCS revision string into 64bits.
** 
** FUNCTIONAL DESCRIPTION:
** 
**      Takes a file as input and and searches for RCS identifiers
**      to determine the revision of the file. Then, writes out an
**      include file which contains a 64 bit encoding of the RCS revision.
** 
** CALLING ENVIRONMENT: 
**
**	user mode
** 
** AUTHOR: Franklin Hooker
**
 * $Log: rcsv.c,v $
 * Revision 1.2  1999/02/09 19:56:07  gries
 * -x to give a xx.xxx version number
 *
 * Revision 1.1.1.1  1998/12/29 21:36:24  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1997/11/05  15:55:52  pbell
 * Changed the Experimental/Released flags to X and V
 *
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

char *strRCS(char *input, char *id);

/*
 *  Macros
 */
#define TRUE 1
#define FALSE 0

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

#define RCS_LOCK_SIGNATURE 0x58	/* Ascii X - Experimental */
#define RCS_UNLOCK_SIGNATURE 0x56 /* Ascii V - Released */
#define RCS_REV_LEVELS 8
char rev[RCS_REV_LEVELS];

/*
 *  Forward routine descriptions.  These have to come before the directives
 *  structure definition.
 */
int main(int argc, char **argv);
void usage();
void process_file(char *ifname, char *ofname);
void encode_rev(char * v);

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
	    while ((option = *arg++)) {	/* until we reach the '0' string
					 * terminator */
		option = tolower(option);
		switch (option) {
		    case 'h': 
			usage();
			exit(1);
		    case 'v':
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
		    case 'x': 
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
	fprintf(stderr, "rcsv - Encode files' RCS revision [V1.1]\n");
	fprintf(stderr, "Input file = %s\nOutput file = %s\n", ifile_name, ofile_name);
    }

/*
 *  Now go and play with the file.
 */
    process_file(ifile_name, ofile_name);

    fprintf(stderr, "%s written\n", ofile_name);
    return(EXIT_SUCCESS);
}					/* end of main() */

void usage()
{
}

void process_file(char *ifname, char *ofname)
{
  FILE *ofile, *ifile;

#define MAX_READ 120
  char ibuf[MAX_READ];
  char *p;
  unsigned int rcs_encoded_rev_h;
  unsigned int rcs_encoded_rev_l;
  int i;
  
  /*
   *  First, open the files.
   */
  
  if ((ifile = fopen(ifname, "r")) == NULL) {
    fprintf(stderr, "ERROR: cannot open input file - %s\n", ifname);
    exit(1);
  }
  
    if (ofname == NULL)
	ofile = stdout;
    else {
	if ((ofile = fopen(ofname, "w")) == NULL) {
	    fprintf(stderr, "ERROR: cannot open output file - %s\n", ofname);
	    exit(1);
	}
    }

  while (fgets(ibuf, MAX_READ, ifile) != 0) {
    counters.lines++;
    
    /*
     * Search string for a valid RCS Identifier.
     */
    if ((p=strRCS(ibuf, "$Id:")) != NULL)
      break;
    if ((p=strRCS(ibuf, "$Header:")) != NULL)
      break;
  }
  
  if (p == NULL) {
    fprintf(stderr, "No valid RCS Identifier was found\n");
    fprintf(ofile, "#define RCS_ENCODED_REV_H 0x00000000\n");
    fprintf(ofile, "#define RCS_ENCODED_REV_L 0x00000000\n");
    return;
  }
  
  rcs_encoded_rev_l = 0;
  rcs_encoded_rev_h = 0;

  assert((RCS_REV_LEVELS >> 1) <= sizeof(rcs_encoded_rev_l));

  for (i = 0; i < (RCS_REV_LEVELS >> 1); ++i) {
    rcs_encoded_rev_l |= rev[i] << (CHAR_BIT * i);
    rcs_encoded_rev_h |= rev[i + (RCS_REV_LEVELS >> 1)] << (CHAR_BIT * i);
  }

  if (_OPTION('v'))
    fprintf(stderr, "RCS Encoded revision: (high) %08x, (low) %08x\n",
	    rcs_encoded_rev_h, rcs_encoded_rev_l);

  fprintf(ofile, "#define RCS_ENCODED_REV_H 0x%08X\n", rcs_encoded_rev_h);
  fprintf(ofile, "#define RCS_ENCODED_REV_L 0x%08X\n", rcs_encoded_rev_l);
}

char *strRCS(char *input, char *id)
{
  int status;
  char *p;
  char *c;
  char revstr[MAX_READ];
  
  if ((p = strstr(input, id)) != NULL) {
    p = p+strlen(id);
    if ((c = strchr((p), '$')) != NULL)
      *c = 0;

    if (c == p)
      return(NULL);
    
    status = sscanf(p, "%*s %s", revstr);
    if (status != EOF) {
      if (_OPTION('v'))
	fprintf(stderr, "RCS revision: %s\n", revstr);
      encode_rev(revstr);
    }
    else
      return(NULL);

    status = sscanf(p, "%*s %*s %*s %*s %*s %*s %s", revstr);
    if (_OPTION('x')) 
      rev[RCS_REV_LEVELS-1] = RCS_LOCK_SIGNATURE;
    else
      rev[RCS_REV_LEVELS-1] = RCS_UNLOCK_SIGNATURE;
  }
  return(p);
}

void encode_rev(char * v)
{
  int i;
  char *p;
  char *end;

  
  for (i = 0; i < RCS_REV_LEVELS; i++)
    rev[i] = 0;

  end = v + strlen(v);
  p = v;

  i = 0;
  while (p<end) {
    ++p;
    if ((*p == '.') || (*p == 0)) {
      *p = 0;
      rev[i] = atoi(v) & 0xff;
      ++i;
      v = p + 1;
    }
  }
}
