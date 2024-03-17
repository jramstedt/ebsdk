#ifndef __ASSERT_H_LOADED
#define __ASSERT_H_LOADED
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
 *  $Id: assert.h,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $;
 *
 * $Log: assert.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/02/21  04:11:09  fdh
 * Initial revision
 *
 */

#undef assert

#ifdef NDEBUG
#define assert(xxx) ((void)0)
#else
extern void __assert(char *message, char *file, int line);
#define assert(EX) (((int) (EX)) ? (void)0 : __assert(#EX, __FILE__, __LINE__))
#endif /* NDEBUG */

#endif /* __ASSERT_H_LOADED */
