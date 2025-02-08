
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
__attribute__((unused)) static const char *rcsid = "$Id: loops1.c,v 1.2 1999/04/09 17:39:27 paradis Exp $";
#endif

/*
 * $Log: loops1.c,v $
 * Revision 1.2  1999/04/09 17:39:27  paradis
 * Added "regtest" to default build for system exerciser
 * Fixed compiler incompatibility in loops1.c
 * Made cstrip a bit more verbose
 *
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:48  fdh
 * Initial revision
 *
 */



#include "stdio.h"

struct s
{
    int si;
    long sj;
    struct s* spp;
};

int func1(int, struct s *);
int func3(void);

extern int func2(int);

void main()
{
    int i = 1;
    int j = 2;
    struct s dataarea;
    static struct s *sp ;
    char * c ="abc";

    sp = &dataarea;
    printf("Test starting \n");
    j += 1;
    func3();
    func3();
    i = func1(func2(j), sp);

    printf("Test run, i = %i, c = %s\n", i , c );
}

int func1(int k, struct s *sp)
{
    int i=0;
    struct s * sp2 = sp;
    static struct s dataarea[5];

    while(i < 5 ) {
	sp2 -> si = i;
	sp2 -> sj = k;
	sp2 -> spp = &(dataarea[i]);
	sp2 = sp2 -> spp;
    }
    return 0;
}

int func3()
{
    printf("In func3\n");
    return 0;
}










