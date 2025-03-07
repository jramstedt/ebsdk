
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
__attribute__((unused)) static const char *rcsid = "$Id: callback.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 * $Log: callback.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.6  1997/04/30  21:15:25  pbell
 * Added UngetChar and updated GetChar and CharAv to support it.
 *
 * Revision 1.5  1996/01/16  16:42:40  fdh
 * Renamed file.
 *
 * Revision 1.4  1995/10/26  23:33:34  cruz
 * Fixed prototypes
 *
 * Revision 1.3  1995/09/12  21:32:25  fdh
 * Modified GetChar to return an int instead of a char.
 *
 * Revision 1.2  1995/09/02  06:29:01  fdh
 * Removed WaitUs() from callbacks.
 *
 * Revision 1.1  1995/08/25  20:34:42  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "lib.h"
#include "callback.h"
#include "ladbxusr.h"

void PutChar(char c)
{
  (*UserPutChar)(c);
}

unsigned CharsSaved = 0;
char SavedChars[4];

int GetChar(void)
{
    if( CharsSaved )
	return( SavedChars[--CharsSaved] );
    return( UserGetChar() );
}

void UngetChar( char c )
{
    if( CharsSaved < 4 )
	SavedChars[CharsSaved++] = c;
}

int CharAv(void)
{
    if( CharsSaved )
	return( TRUE );
    return(*UserCharAv)();
}

time_t time(time_t *out)
{
  return (*Usertime)(out);
}

void malloc_summary(char *string)
{
  (*Usermalloc_summary)(string);
}

void *malloc(size_t size)
{
  return(*Usermalloc)(size);
}

void free(void *pointer)
{
  (*Userfree)(pointer);
}

void *realloc(void *pointer, size_t size)
{
  return(*Userrealloc)(pointer, size);
}

void ladbx_poll(void)
{
  (*Userladbx_poll)();
}
