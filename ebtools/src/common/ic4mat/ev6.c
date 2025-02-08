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
__attribute__((unused)) static const char *rcsid = "$Id: ev6.c,v 1.2 1999/01/21 19:06:24 gries Exp $";
#endif

/*
 * Author: Franklin Hooker, Digital Equipment Corporation, July 1997
 *
 * $Log: ev6.c,v $
 * Revision 1.2  1999/01/21 19:06:24  gries
 * First Release to cvs gries
 *
 * Revision 1.3  1997/07/08  15:25:32  fdh
 * Added a missing return.
 *
 * Revision 1.2  1997/07/07  14:55:44  fdh
 * Added write_chain prototype.
 *
 */

#include "ic4mat.h"

static void read_ev6_cbox(void);
static int write_chain(bit_t *write_once_a, datum_t *chain_buffer, int length);
static int read_chain(bit_t *write_once_a);
static int read_line(void);

void format_ev6_icache(void)
{
  int length;
  bit_t *write_once_a;
  int leader = 16;
  datum_t chain_buffer[MAX_CHAIN/DATUMSIZE] = {0};

  write_once_a = calloc((MAX_CHAIN+leader), sizeof(bit_t));

  read_ev6_cbox();
  puts("Loading up Write Once Chain from CBOX");
  length = setup_chain_data(write_once_a,
			    FORWARD, /* Load up chain */
			    leader);
  
  printf("Write Once Chain size %d",length);
  write_chain(write_once_a, chain_buffer, length); /*  */
  process_ev6_srom(chain_buffer);
}


static void read_ev6_cbox (void)
{
  char * trailing_comment;
  string csr_name;
  csr_datum_t csr_value;
  csr_t *csr_p;
  char buffer[MAX_FILELINE+1];

  /*
   * identify input file as source in list file
   */
  if (_SEEN('l'))
    fprintf (lisfile.file, "\n/*** Input file: %s ***/\n\n", infile.fname);

  /*
   * initialize in structure
   */
  infile.eof = FALSE;
  infile.linenum = 0;

  while (read_line()) {
    trailing_comment = strchr(infile.line, ';');
    if (trailing_comment != NULL) *trailing_comment = '\0';

    if (infile.line[infile.start] == '\0') { /* Anything to do? */
      if (_SEEN('l')) {
	if (trailing_comment == NULL) {
	  fprintf(lisfile.file, "%s\n", infile.line);
	}
	else {
	  fprintf(lisfile.file, "%s;%s\n", infile.line, trailing_comment+1);
	}
      }
      continue;			/* Nothing to do */
    }

    if (sscanf(infile.line, "%s = %x", csr_name, &csr_value) != 2) {
      report_error(trailing_comment,
		   "Error:",
		   "Syntax error\n");
      ++errors;
      continue;
    }

    csr_p = lookup_csr(csr_name);
    if (csr_p == NULL) {
      report_error(trailing_comment,
		   "Error:",
		   "Unknown CSR\n");
      ++errors;
      continue;
    }

    csr_p->value = csr_value;
    if (csr_p->initialized == TRUE) {
      report_error(trailing_comment,
		   "Warning:",
		   "CSR already initialized... Overriding\n");
      ++warnings;
    }

    csr_p->initialized = TRUE;

    if (_SEEN('l')) {
      sprintf(buffer, "%-30s = 0x%-8x",
	      _SEEN('a') ? csr_p->alias : csr_p->name,
	      csr_p->value);

      if (trailing_comment == NULL) {
	fprintf(lisfile.file, "%s\n", buffer);
	continue;
      }

      fprintf(lisfile.file, "%-*s;%s\n",
	      (int) (trailing_comment - infile.line),
	      buffer,
	      trailing_comment+1);
    }
  }
}


/*
 * read_line  -  read next line from file
 *
 * Reads next line from infile.file that contains text into infile.line.
 * Returns TRUE if a line returned. Length of line is returned in
 * infile.linelength.
 * 
 * At end of file: sets infile.line to a null string, sets infile.eof TRUE,
 * and returns FALSE.
 * 
 * Actual line number in file is returned in infile.linenum.
 * 
 */

static int read_line (void)
{
  char * newline;
  int i;

  /*
   * check if already at end of file
   */
  if (infile.eof) return FALSE;

  do {
    ++infile.linenum;
    if (!fgets(infile.line, MAX_FILELINE, infile.file)) {
      if (ferror (infile.file)) {	/* exit if read error */
	report_error(NULL,
		     "Error:",
		     "File read error\n");
	++errors;
	exit(EXIT_FAILURE);
      }
      else {
	infile.eof = TRUE;		/* record end of file */
	infile.line[0] = '\0';
	infile.linelength = 0;
	infile.start = infile.position = infile.end = 0;
	return FALSE;
      }
    }

    newline = strchr(infile.line, '\n');
    if (newline != NULL) {
      *newline = '\0';
    }
    else {
      report_error(NULL, "Error:",
		   "Line too long. Line longer than %d characters\n",
		   MAX_FILELINE);
      ++errors;
    }

    /*
     * Replace "C" style comments with white-space.
     */
    for (i = 0; infile.line[i] != '\0'; ++i) {
      /* Check for beginning of "C" style comment */
      if ((infile.line[i] == '/') && (infile.line[i+1] == '*')) {
	if (infile.comment) {
	  report_error(NULL,
		       "Error:",
		       "Illegally nested comment.\n");
	  ++errors;
	}
	infile.comment = TRUE;
	infile.line[i] = infile.line[i+1] = ' ';
	i += 2;
      }
      else {
	/* Check for beginning of "C" style comment */
	if ((infile.line[i] == '*') && (infile.line[i+1] == '/')) {
	  infile.comment = FALSE;
	  infile.line[i] = infile.line[i+1] = ' ';
	  i += 2;
	}
	else {
	  /* And all the comment characters in between */
	  if (infile.comment) infile.line[i] = ' ';
	}
      }
    }


    infile.linelength = strlen(infile.line); /* get length of line */
    infile.start = 0;
    infile.end = infile.linelength;

    while (isspace (infile.line[infile.start])) ++infile.start;  /* ignore leading blanks */
    if (infile.start == infile.end) continue;	/* read again if blank line */
    while (isspace (infile.line[infile.end - 1])) --infile.end; /* ignore trailing blanks */
  } while (infile.start == infile.end); /* read again if blank line */

  infile.position = infile.start;
  return TRUE;
}


static int write_chain(bit_t *write_once_a, datum_t *chain_buffer, int length)
{
  int i;

  puts("Moving Write Once data into temporary chain buffer");
  for (i=0; i < length; ++i) {
    chain_buffer[i/DATUMSIZE] |= (datum_t) write_once_a[i] << (i % DATUMSIZE);
  }

  puts("Printing out Write Once chain");
  printf("CBOX_SHIFT_DATA(%d,0) = 0x", length - 1);

  for (i=length/DATUMSIZE; i >= 0 ; --i) {
    printf("%0*lX", sizeof(datum_t)*2, chain_buffer[i]);
  }
  printf("\n\n");
  fflush(stdout);
  return(length);
}


void convert_ev6_chain_to_cbox(void)
{
  bit_t *write_once_a;
  int leader = 16;

  write_once_a = calloc((MAX_CHAIN+leader), sizeof(bit_t));
  read_chain(write_once_a);

  puts("Loading up CBOX from Write Once Chain");
  setup_chain_data(write_once_a,
		   REVERSE, /* Load up chain */
		   leader);
  
  dump_cbox(outfile.file);
}

static int read_chain(bit_t *write_once_a)
{
  int i;
  int j;
  bit_t nibble;
  int length;
  int example_chain_length;
  char example_chain[MAX_FILELINE];

  example_chain_length = 0;
  while (read_line()) {
    if (sscanf(infile.line, "%s", example_chain) == 1) {
      puts("Write Once Chain...");
      printf("%s\n\n", example_chain);
      fprintf(outfile.file, "/*\n", example_chain);
      fprintf(outfile.file, "** %s\n", example_chain);
      fprintf(outfile.file, "*/\n", example_chain);

      example_chain_length = strlen(example_chain);
      length = example_chain_length * 4;

      break;
    }
    else {
      report_error(NULL,
		   "Error:",
		   "Couldn't find Write Once Chain.\n");
      ++errors;
    }
  }

  if (example_chain_length == 0) {
    exit(EXIT_FAILURE);
  }

  puts("Scanning example chain into write once array");
  for (j=0; j < example_chain_length; ++j) {
    if (isdigit(example_chain[(example_chain_length-1-j)])) {
      nibble = (bit_t) example_chain[(example_chain_length-1-j)] - '0';
    }
    else if (isxdigit(example_chain[(example_chain_length-1-j)])) {
      nibble = (bit_t) tolower(example_chain[(example_chain_length-1-j)]) - 'a' + 10;
    }
    else {
      break;
    }

    printf("'%X' ", nibble);

    for (i=0; i < 4; ++i) {
      write_once_a[i+(j*4)] = (nibble >> i) & 1;
      printf("[%d]=%x,", i+(j*4), write_once_a[i+(j*4)]);
      fflush(stdout);
    }
    printf("\n");
  }
  printf("\n");

  return(length);
}
