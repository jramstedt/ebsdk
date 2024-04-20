/* file:	memset.c
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
 * Abstract:	Block memory utilities.
 *
 * Author:	AJ Beaverson
 *
 * Modifications:
 *
 *	phk     14-May-1991	optimize memset & memcpy for VAX targets
 *
 *	ajb	28-Feb-1991	change memset so that the resulting buffer
 *				has the same contents regardless of alignment.
 *
 *	ajb	25-Jan-1991	Add routine that swaps contents of two
 *				buffers (used by sort routines)
 *
 *	sfs	31-Oct-1990	Optimize memset (do long operations where
 *				possible).
 *
 *	jad	28-Sep-1990	Add memcmp
 *
 *	ajb	16-Aug-1990	Fix GNUC complaints
 *
 *	sfs	14-Aug-1990	Fix return values for memset/memcpy.
 *
 *	ajb	27-Mar-1990	Initial entry.
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"

#define   k63   ( (64 * 1024) - 4)


/*+
 * ============================================================================
 * = memset - set a range of memory to a value.                               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Memset sets a arbitrary length of contiguous memory to a value.  The
 *	order in which the memory is set, and the access mechansim (byte, word
 *	longword writes) is UNPREDICTABLE.
 *  
 * FORM OF CALL:
 *  
 *	memset (void *address, unsigned char value, int length)
 *  
 * RETURNS:
 *
 *	address - starting address of block.
 *       
 * ARGUMENTS:
 *
 * 	void *address	- starting address of block
 *	int value	- value to be stored
 *	const int size	- length in bytes
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
char *memset (void *address, unsigned char value, int length) {

	if ((((int) address&3) == 0) && ((length&3) == 0)) {
	    int *a;
	    int v;
	    int l;

	    a = address;
	    v = (value << 24) | (value << 16) | (value << 8) | value;
	    l = length/4;
	    while (--l >= 0) *a++ = v;
	} else {
	    char *a;
	    char v;
	    int	l;
	
	    a = address;
	    v = value;
	    l = length;
	    while (--l >= 0) *a++ = v;
	}
	return address;
}

/*+
 * ============================================================================
 * = memcpy - copy a block of memory                                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Copy a block of memory from one area to another area.  Insure that the
 *	destination area accurately reflects the source, even if the source
 *	area is overwritten.
 *  
 * FORM OF CALL:
 *  
 *	memcpy (char *destination, char *source, int length)
 *  
 * RETURNS:
 *
 *	dst - destination address
 *       
 * ARGUMENTS:
 *
 * 	void *destination   - destination address
 *	void *source	    - source address
 *	int length	    - size in bytes
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
char *memcpy (char *destination, char *source, int length) {

 	char	*s, *d;
	int 	l;

	s = source;
	d = destination;
	l = length;

	if (s > d) {
	    while (--l >= 0) *d++ = *s++;
	} else {
	    s += l;
	    d += l;
	    while (--l >= 0) *--d = *--s;
	}

	return destination;
}


/*+
 * ============================================================================
 * = memcmp - compare a block of memory                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Compare two blocks of memory lexicographically.
 *  
 * FORM OF CALL:
 *  
 *	memcmp (s0,s1,len)
 *  
 * RETURNS:
 *
 *	minus 1		- s1 < s0
 *	0		- s1 == s0
 * 	1		- s1 > s0
 *       
 * ARGUMENTS:
 *
 *      char unsigned *s0	- address of first null terminated string
 *      char unsigned *s1	- address of second null terminated string
 *	int len - number of characters to compare
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int memcmp (char unsigned *s0, char unsigned *s1, int len) {
	char unsigned *t0, *t1;
	int	l;

	t0 = (char unsigned *) s0;
	t1 = (char unsigned *) s1;
	l = len;

	while ((--l >= 0) && (*t0 == *t1)) t0++, t1++;
	if (l < 0) return 0;
	if (*t0 == *t1) return 0;
	if (*t0 > *t1) return 1;
	return -1;
}


/*+
 * ============================================================================
 * = memswp - swap contents of two blocks of memory =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Swap the contents of two memory blocks.
 *  
 * FORM OF CALL:
 *  
 *	memswap (s0,s1,len)
 *  
 * RETURNS:
 *
 *	len
 *       
 * ARGUMENTS:
 *
 *      char unsigned *s0	- address of first block
 *      char unsigned *s1	- address of second block
 *	int len 		- number of characters to compare
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
char *memswp (unsigned char *dst, unsigned char *src, int len) {
 	unsigned char	*s, *d, hold;
	int	l;

	s = src;
	d = dst;
	l = len;

	if (s > d) {
	    while (--l >= 0) {
		hold = *d;
		*d = *s;
		*s = hold;
		d++;
		s++;
	    }
	} else {
	    s += l;
	    d += l;
	    while (--l >= 0) {
		d--;
		s--;
		hold = *d;
		*d = *s;
		*s = hold;
	    }
	}
	return dst;
}

