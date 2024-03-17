#ifndef __IC4MAT_H_LOADED
#define __IC4MAT_H_LOADED
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
 *  $Id: ic4mat.h,v 1.1.1.1 1998/12/29 21:36:22 paradis Exp $;
 *  Author: Franklin Hooker, Digital Equipment Corporation, July 1997
 */

/*
 * $Log: ic4mat.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:22  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1997/12/22  17:18:33  gries
 * Quinn's changes from pass 2-3
 *
 * Revision 1.1  1997/11/12  18:56:16  pbell
 * Initial revision
 *
 * Revision 1.2  1997/07/07  15:00:43  fdh
 * Added declarations for global comman parsing options.
 *
 * Revision 1.1  1997/07/07  00:50:05  fdh
 * Initial revision
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "ebtools.h"
#include "ev6.h"

#define REVERSE TRUE
#define FORWARD FALSE

extern char * getopt_option_b;
extern char * getopt_option_m;
extern char * getopt_option_p;

extern file_data_t infile;
extern file_data_t dxefile;
extern file_data_t outfile;
extern file_data_t lisfile;

extern int errors, warnings, ev6_passnum;

extern int main(int argc, char * *argv);
extern void report_error(char *trailing_comment, char *message, char *fmt, ...);

#endif /* __IC4MAT_H_LOADED */
