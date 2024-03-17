#ifndef __EBTOOLS_H_LOADED
#define __EBTOOLS_H_LOADED
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
 *  $Id: ebtools.h,v 1.1.1.1 1998/12/29 21:36:20 paradis Exp $;
 *
 *  Author: Franklin Hooker, Digital Equipment Corporation, July 1997
 */

/*
 * $Log: ebtools.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:20  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/11/11  17:03:12  pbell
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include "getopt.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define FILENAME_SIZE 256
#define MAX_STRING 256 /* Size of strings */
#define MAX_FILELINE 512 /* Size of file lines */
typedef char string[MAX_STRING+1], filename[FILENAME_SIZE+1], fileline[MAX_FILELINE+1];

#define _SEEN(o) (seen[o-'a'])
#define SEEN_SIZE ('z' - 'a' + 10)
extern char seen[];

/* information for reading an input file */
typedef struct file_data {
  char *fname;			/* name of file */
  FILE *file;			/* file pointer */
  int fsize;			/* File size */
  int linenum;			/* current line number */
  fileline line;		/* full current line */
  int linelength;		/* current line length */
  int position;			/* current position in line */
  int start;			/* start position */
  int end;			/* end position */
  int eof;			/* end of file flag */
  int comment;			/* comment flag */
}  file_data_t;

#endif /* __EBTOOLS_H_LOADED */
