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
 *  $Id: getopt.c,v 1.1.1.1 1998/12/29 21:36:20 paradis Exp $;
 *
 *  Author: Franklin Hooker, Digital Equipment Corporation, July 1997
 */

/*
 * $Log: getopt.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:20  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/11/11  17:03:12  pbell
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include "getopt.h"

#define ERR(s, c) \
    if (opterr) {\
        char errbuf[2];\
        errbuf[0] = (char) c; errbuf[1] = '\n';\
        (void) fwrite (argv[0], strlen (argv[0]), 1, stderr);\
        (void) fwrite (s, strlen (s), 1, stderr);\
        (void) fwrite (errbuf, 2, 1, stderr);\
    }

int opterr = 1;
int optind = 1;
int optopt;
char *optarg;

int getopt (int argc, char **argv, const char *opts)
{
    static int sp = 1;
    int c;
    char *cp;

    if (sp == 1)
        if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
            return (EOF);
        else if (strcmp (argv[optind], "--") == 0) {
            optind++;
            return (EOF);
        }
    optopt = c = argv[optind][sp];
    if (c == ':' || (cp = strchr (opts, c)) == NULL) {
        ERR (": illegal option -- ", c);
        if (argv[optind][++sp] == '\0') {
            optind++;
            sp = 1;
        }
        return ('?');
    }
    if (*++cp == ':') {
        if (argv[optind][sp+1] != '\0')
            optarg = &argv[optind++][sp+1];
        else if (++optind >= argc) {
            ERR (": option requires an argument -- ", c);
            sp = 1;
            return ('?');
        } else
            optarg = argv[optind++];
        sp = 1;
    } else {
        if (argv[optind][++sp] == '\0') {
            sp = 1;
            optind++;
        }
        optarg = NULL;
    }

    return (c);

}
