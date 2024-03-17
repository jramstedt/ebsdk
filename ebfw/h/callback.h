#ifndef __CALLBACK_H_LOADED
#define __CALLBACK_H_LOADED
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
 *  $Id: callback.h,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $;
 */

/*
 * $Log: callback.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1996/01/16  16:40:04  fdh
 * Renamed file...
 *
 * Revision 1.3  1995/09/12  21:27:33  fdh
 * Modified UserGetChar() to return an int
 * instead of a char.
 *
 * Revision 1.2  1995/09/02  06:28:28  fdh
 * Removed WaitUs() from callbacks.
 *
 * Revision 1.1  1995/08/25  20:52:56  fdh
 * Initial revision
 *
 */

/*::::::::::::::
callback.c
::::::::::::::*/
extern void (*UserPutChar)(char c);
extern int (*UserGetChar)(void);
extern int  (*UserCharAv)(void);
extern time_t (*Usertime)(time_t *out);
extern void (*Usermalloc_summary)(char *string);
extern void *(*Usermalloc)(size_t size);
extern void (*Userfree)(void *pointer);
extern void *(*Userrealloc)(void *pointer, size_t size);
extern void (*Userladbx_poll)(void);

#endif /* __CALLBACK_H_LOADED */
