#ifndef __CTYPES_H_LOADED
#define __CTYPES_H_LOADED
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
 *  $Id: ctype.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $;
 *
 * $Log: ctype.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.5  1997/02/21  04:12:27  fdh
 * Corrected a definition.
 *
 * Revision 1.4  1995/10/11  21:40:55  fdh
 * Added check for range.
 *
 * Revision 1.3  1995/09/02  03:49:24  fdh
 * Moved array initialization to ctype.c
 *
 */

#define _ISMACRO(c,x) ((c <= 127) && ((int) (ctypeArray[c] & (x))))

extern const unsigned short ctypeArray[];

#define _isalnum(c)	_ISMACRO(c,_ISALNUM_)
#define _isalpha(c)	_ISMACRO(c,_ISALPHA_)
#define _isascii(c)	_ISMACRO(c,_ISASCII_)
#define _iscntrl(c)	_ISMACRO(c,_ISCNTRL_)
#define _isdigit(c)	_ISMACRO(c,_ISDIGIT_)
#define _isgraph(c)	_ISMACRO(c,_ISGRAPH_)
#define _islower(c)	_ISMACRO(c,_ISLOWER_)
#define _isprint(c)	_ISMACRO(c,_ISPRINT_)
#define _ispunct(c)	_ISMACRO(c,_ISPUNCT_)
#define _isspace(c)	_ISMACRO(c,_ISSPACE_)
#define _isupper(c)	_ISMACRO(c,_ISUPPER_)
#define _isxdigit(c)	_ISMACRO(c,_ISXDIGIT_)

#define _ISALNUM_  0x0001
#define _ISALPHA_  0x0002
#define _ISASCII_  0x0004
#define _ISCNTRL_  0x0008
#define _ISDIGIT_  0x0010
#define _ISGRAPH_  0x0020
#define _ISLOWER_  0x0040
#define _ISPRINT_  0x0080
#define _ISPUNCT_  0x0100
#define _ISSPACE_  0x0200
#define _ISUPPER_  0x0400
#define _ISXDIGIT_ 0x0800

#endif /* __CTYPES_H_LOADED */
