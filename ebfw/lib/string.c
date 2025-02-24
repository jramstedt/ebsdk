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
__attribute__((unused)) static const char *rcsid = "$Id: string.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: string.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.16  1998/10/08  14:08:58  gries
 * added newline to fix warning
 *
 * Revision 1.15  1998/08/19  17:13:28  jerzy
 * Removed (commented out) memcpy() to avoid link error.
 *
 * Revision 1.14  1997/05/21  12:58:47  fdh
 * Added RCS Id directive.
 *
 * Revision 1.13  1997/02/21  03:45:01  fdh
 * Corrected the strlen definition.
 *
 * Revision 1.12  1995/10/31  16:48:33  cruz
 * Commented out some unused routines.
 *
 * Revision 1.11  1995/10/18  23:05:29  cruz
 * Changed argument of memset from char to int.
 *
 * Revision 1.10  1995/09/22  13:10:12  fdh
 * Removed memmove() because it did not handle the case
 * for overlapping memory regions.  Therefore, memcpy()
 * supplants that memmove().
 *
 * Revision 1.9  1995/09/02  03:46:05  fdh
 * Now includes only routines normally defined in
 * the string.h header.
 *
 * Revision 1.8  1995/09/01  15:00:53  fdh
 * Removed ctype functions. Moved them to ctype.c
 *
 * Revision 1.7  1995/02/27  19:23:20  fdh
 * Added Ctype routines.
 *
 * Revision 1.6  1995/02/14  18:53:56  fdh
 * Implemented atoi() and various ctype Macros.
 *
 * Revision 1.5  1995/02/13  15:09:50  fdh
 * *** empty log message ***
 *
 * Revision 1.4  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.2  1994/06/14  10:05:45  rusling
 * Added memmove().
 *
 * Revision 1.1  1994/03/03  15:27:16  rusling
 * Initial revision
 *
 */
#include <stddef.h>
#include "lib.h"

#if 1
	// In the new libc.a memcopy, memmove, _OtsMove, etc
	// all reside in the same ots_move_alpha.o module.
	// We have to use memcpy from libca to avoid linker
	// error: duplicate definition of memcpy. 
/**************************************************************************
 * memcpy()                                                               *
 **************************************************************************/
void *memcpy(void *ps, const void *pct, size_t n)
{
  char * s = (char *)ps;
  char * ct = (char *)pct;

    char *saved = s;

    while (n--) *s++ = *ct++;
    return saved;
}
#endif

/**************************************************************************
 * memcmp()                                                               *
 **************************************************************************/
int memcmp(const void *pcs, const void *pct, size_t n)
{
  char * cs = (char *)pcs;
  char * ct = (char *)pct;

    while (n--) {
	if (*cs > *ct) return 1;
	if (*cs < *ct) return -1;
	cs++;
	ct++;
    }
    return 0;
}

/**************************************************************************
 * memset()                                                               *
 **************************************************************************/
void *memset(void *ps, int c, size_t n)
{
  char *s = (char *)ps;

    char *saved = s;

    while (n--) *s++ = (char)c;

    return saved;
}

/**************************************************************************
 * memchr()                                                               *
 **************************************************************************/
void* memchr(const void* ptr, int ch, size_t count)
{
	register const char *s = (const char *)ptr;
	while (count) {
		if (*s == (char)ch) {
			return (void *)s;
		}
		++s;
		--count;
	}

	return NULL;
}

size_t strlen(char *s)
{
  size_t len;
  len = 0;
  while (*s++ != '\0') ++len;
  return (len);
}

void bzero(char *s, int count)
{
  while (count--) *s++ = 0;
}

#ifdef NOT_IN_USE
void bcopy(char *from, char *to, int count)
{
  while (count--) *to++ = *from++;
}
#endif
