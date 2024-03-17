#ifndef __SETJMP_H_LOADED
#define __SETJMP_H_LOADED
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
 *  $Id: setjmp.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 */

/*
 * $Log: setjmp.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1997/06/10  18:39:34  pbell
 * Changed jmp_buf base type from long to ul for NT compatibility
 *
 * Revision 1.1  1997/02/21  04:16:35  fdh
 * Initial revision
 *
 */

#define __JMP_BUF_SIZE (18)
typedef ul jmp_buf[__JMP_BUF_SIZE];

extern void longjmp(jmp_buf env, int value);
extern int  setjmp(jmp_buf env);
extern int  _setjmp(jmp_buf env);
extern void _longjmp(jmp_buf env, int value);

#endif /* __SETJMP_H_LOADED */
