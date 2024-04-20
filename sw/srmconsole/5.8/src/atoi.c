/* file:	atoi.c
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
 */

/*
 *++
 *  FACILITY:
 *
 *      Cobra Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	ATOI utility.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      19-Apr-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	24-mar-1992	Check for and report overflow on
 *				conversions.
 *
 *	ajb	03-Feb-1992	Change common_convert so that a leading 00
 *				allows the default radix to be specified.
 *
 *	ajb	17-jan-1992	enhance ntoi
 *
 *	ajb	25-Feb-1991	use 0x, 0d, 0b for radix overrides
 *
 *	ajb	19-Apr-1990	Initial entry.
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:ctype.h"
#include	"cp$src:msg_def.h"

/*+
 * ============================================================================
 * = atoi - Convert ASCII to integer                                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Convert from ASCII to binary integer.  Eat any prefix whitespace.
 *	Accept an optional sign (-), and optional radix specifiers of the
 *	form %x, %d, %o, or %b representing hex, decimal, octal, or binary,
 *	respectively.
 *  
 * FORM OF CALL:
 *  
 *	atoi( s )
 *  
 * RETURNS:
 *
 *	n - integer represented by ASCII string or 0 for invalid strings
 *       
 * ARGUMENTS:
 *
 *      const char *s	- address of null terminated string to be converted
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
INT atoi (const char *s) {
	INT	result;

	common_convert (s, 10, &result, sizeof (result));
	return result;
}

/*+
 * ============================================================================
 * = xtoi - Convert hex ASCII to integer                                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Convert from ASCII to binary integer.  Eat any prefix whitespace.
 *	Accept an optional sign (-).  Assume a radix of hex.
 *  
 * FORM OF CALL:
 *  
 *	xtoi( s )
 *  
 * RETURNS:
 *
 *	n - integer represented by ASCII string or 0 for invalid strings
 *       
 * ARGUMENTS:
 *
 *      const char *s	- address of null terminated string to be converted
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
INT xtoi (const char *s) {
	INT	number;

	common_convert (s, 16, &number, sizeof (number));
	return number;
}

/*+
 * ============================================================================
 * = common_convert - Convert an ascii string to binary                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Convert an ASCII string to binary.  Eat any prefix whitespace.
 *	Accept an optional sign (-), and optional radix specifiers of the
 *	form %x, %d, %o, or %b representing hex, decimal, octal, or binary,
 *	respectively.  Pass a default radix.
 *
 *	Pass the size of the result so that arbitrarily long strings may
 *	be accomodated.
 *
 *	Check for illegal digits and overflows.
 *  
 * FORM OF CALL:
 *  
 *	common_convert (s, radix, result, length)
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_radix - number isn't in the radix specified
 *       
 * ARGUMENTS:
 *
 *      const char *s		- address of null terminated string to be converted
 *	int radix	- radix to use for conversion
 *	unsigned char *result	- address of resulting number
 *	int length	- size of number in bytes
 *   
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int common_convert (const char *s, int radix, unsigned char *result, int length) {
	int	c;
	int	digit;
	int	i, carry;
	int	x;
	unsigned char *cp;
	int	negate;

	/* skip optional whitespace */
     	while (isspace (*s)) s++;

	/* accept optional sign */
	negate = 0;
	if (*s == '-') {
	    negate = 1;
	    s++;
	}

	/*
	 * Eat optional radix specifier
	 */
	if (*s == '%') {
	    s++;
	    switch (*s) {
	    case 'B':
	    case 'b': radix = 2;  s++; break;
	    case 'D':
	    case 'd': radix = 10; s++; break;
	    case 'X':
	    case 'x': radix = 16; s++; break;
	    }
	}


	/*
	 * convert number
	 */
	memset (result, 0, length);
	while (*s) {
	    c = toupper (*s);
	    if (!isxdigit (c)) {
		return msg_radix;
	    }
	    digit = c - '0';
	    if (c >= 'A') {
		digit -= 7;
	    }
	    if (digit >= radix) return msg_radix;
	    carry = digit;
	    for (cp=result, i=0; i<length; i++, cp++) {
		x = *cp * radix + carry;
		*cp = x;
		carry = (x >> 8) & 0xff;
	    }
	    if (carry) return msg_overflow;
	    s++;
	}

	if (negate) {
	    /* complement */
	    for (cp=result, i=0; i<length; i++, cp++) *cp = ~ *cp;

	    /* add 1 */
	    carry = 1;
	    for (cp=result, i=0; i<length; i++, cp++) {
		x = *cp + carry;
		*cp = x;
		carry = x >> 8;
	    }
	}

	return msg_success;
}

#if 0

void main (int argc, char *argv []) {
	int	argix;
	for (argix=1; argix<argc; argix++) {
	    printf ("%08X %s\n", atoi_test (argv [argix]), argv [argix]);
	}
}
#endif
