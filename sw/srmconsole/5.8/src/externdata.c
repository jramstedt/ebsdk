/* file:	externdata.c
 *
 * Copyright (C) 1990 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 *
 *
 * Abstract:	Required character array for CTYPE.H support on EVAX/Cobra.
 *
 * Author:	VAX C RTL
 *
 * Modifications:
 *
 *	ajb	07-Nov-1990	Add 8 bit support
 *
 *	ajb	09-Aug-1990	Remove readonly attribute
 *
 *	ajb	11-May-1990	Add in place holders for 8 bit characters.
 *
 *	ajb	27-Mar-1990	Initial entry.
 */


#include "cp$src:platform.h"

/*
 * Character type declaration
 */
#define	U	0x01	/* Uppercase letters	*/
#define	L	0x02	/* Lowercase letters	*/
#define	D	0x04	/* Decimal digits	*/
#define	S	0x08	/* White space		*/
#define	P	0x10	/* Punctuation marks	*/
#define	C	0x20	/* Control characters	*/
#define	X	0x40	/* Hexadecimal digits	*/
#define	B	0x80	/* Blank character only	*/


unsigned char _ctype_ [] = {
	C,   C,   C,   C,   C,   C,   C,   C,   	/* 00 - 07 */
	C,   S|C, S|C, S|C, S|C, S|C, C,   C,   	/* 08 - 0f */
	C,   C,   C,   C,   C,   C,   C,   C,   	/* 10 - 17 */
	C,   C,   C,   C,   C,   C,   C,   C,   	/* 18 - 1f */
	B|S, P,   P,   P,   P,   P,   P,   P,		/* 20 - 27 */
	P,   P,   P,   P,   P,   P,   P,   P,   	/* 28 - 2f */
	X|D, X|D, X|D, X|D, X|D, X|D, X|D, X|D, 	/* 30 - 37 */
	X|D, X|D, P,   P,   P,   P,   P,   P,   	/* 38 - 3f */
	P,   X|U, X|U, X|U, X|U, X|U, X|U, U,   	/* 40 - 47 */
	U,   U,   U,   U,   U,   U,   U,   U,		/* 48 - 4f */
	U,   U,   U,   U,   U,   U,   U,   U,   	/* 50 - 57 */
	U,   U,   U,   P,   P,   P,   P,   P,   	/* 58 - 5f */
	P,   X|L, X|L, X|L, X|L, X|L, X|L, L,   	/* 60 - 67 */
	L,   L,   L,   L,   L,   L,   L,   L,   	/* 68 - 6f */
	L,   L,   L,   L,   L,   L,   L,   L,		/* 70 - 77 */
	L,   L,   L,   P,   P,   P,   P,   C,		/* 78 - 7f */
	

/* The following table is for DEC Multinational Character Set (MCS) */

	0,   0,   0,   0,   C,   C,   C,   C,   	/* 80 - 87 */
	C,   C,   C,   C,   C,   C,   C,   C,   	/* 88 - 8f */
	C,   C,   C,   C,   C,   C,   C,   C,		/* 90 - 97 */
	0,   0,   0,   C,   C,   C,   C,   C,		/* 98 - 9f */
	P,   P,   P,   P,   0,   P,   0,   P,		/* a0 - a7 */
	P,   P,   P,   P,   0,   0,   0,   0,		/* a8 - af */
	P,   P,   P,   P,   0,   P,   P,   P,		/* b0 - b7 */
	0,   P,   P,   P,   P,   P,   0,   P,		/* b8 - bf */
	U,   U,   U,   U,   U,   U,   U,   U,		/* c0 - c7 */
	U,   U,   U,   U,   U,   U,   U,   U,		/* c8 - cf */
	0,   U,   U,   U,   U,   U,   U,   U,		/* d0 - d7 */
	U,   U,   U,   U,   U,   U,   0,   P,		/* d8 - df */
	L,   L,   L,   L,   L,   L,   L,   L,		/* e0 - e7 */
	L,   L,   L,   L,   L,   L,   L,   L,		/* e8 - ef */
	0,   L,   L,   L,   L,   L,   L,   L,		/* f0 - f7 */
	L,   L,   L,   L,   L,   L,   0,   0,		/* f8 - ff */

#if 0
/* The following table is for ISO Latin 1 */

	0,   0,   0,   0,   C,   C,   C,   C,   	/* 80 - 87 */
	C,   C,   C,   C,   C,   C,   C,   C,   	/* 88 - 8f */
	C,   C,   C,   C,   C,   C,   C,   C,		/* 90 - 97 */
	0,   0,   0,   C,   C,   C,   C,   C,		/* 98 - 9f */
	P,   P,   P,   P,   P,   P,   P,   P,		/* a0 - a7 */
	P,   P,   P,   P,   P,   P,   P,   P,		/* a8 - af */
	P,   P,   P,   P,   P,   P,   P,   P,		/* b0 - b7 */
	P,   P,   P,   P,   P,   P,   P,   P,		/* b8 - bf */
	U,   U,   U,   U,   U,   U,   U,   U,		/* c0 - c7 */
	U,   U,   U,   U,   U,   U,   U,   U,		/* c8 - cf */
	U,   U,   U,   U,   U,   U,   U,   U,		/* d0 - d7 */
	U,   U,   U,   U,   U,   U,   U,   P,		/* d8 - df */
	L,   L,   L,   L,   L,   L,   L,   L,		/* e0 - e7 */
	L,   L,   L,   L,   L,   L,   L,   L,		/* e8 - ef */
	L,   L,   L,   L,   L,   L,   L,   L,		/* f0 - f7 */
	L,   L,   L,   L,   L,   L,   L,   P,		/* f8 - ff */
#endif
};
