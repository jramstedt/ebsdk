/* file:	strfunc.c
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
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      Miscellaneous string functions.
 *
 *  AUTHORS:
 *
 *      Chris Bord (from VAC C runtime group)
 *
 *  CREATION DATE:
 *  
 *      01-Jan-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	jrk	 4-Jan-1996	Added strwcmp_nocase
 *
 *	ajb	28-Apr-1992	Added strunion
 *
 *	ajb	25-Jul-1991	use memset to clear character arrays
 *
 *	dtm	29-May-1991	add strrchr
 *
 *	dtm	 7-Feb-1991	_nocase strcmp functions fixed for cases
 *				where strings do not match.
 *
 *	dtm	14-Jan-1991	remove abbrev
 *
 *	pel	10-Jan-1991	add substring, substring_nocase.
 *
 *	dtm	06-Nov-1990	Rename strcmp_case/strncmp_case to _nocase
 *
 *	dtm	01-Nov-1990	Add abbrev (from ev_driver.c)
 *
 *	ajb	26-Sep-1990	Fix non initialized arrays, redundant clearing
 *
 *	ajb	25-Sep-1990	Add strelement
 *
 *	dtm	18-Sep-1990	Add strcmp_case and strncmp_case
 *
 *	ajb	08-Aug-1990	Bump characters arrays to 256 bytes
 *
 *	ajb	17-Jul-1990	Add strncmp
 *
 *	ajb	07-Jul-1990	Autodoc updates
 *
 *	ajb	30-May-1990	Added strcount.
 *
 *	ajb	10-May-1990	Adapted from C run time library.
 *--
 */


#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:ctype.h"

/*+
 * ============================================================================
 * = strlen - return the length of a string                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This function calculates the length of the string specified in arg 1
 *	by scanning the string looking for a null byte terminator.
 *  
 * FORM OF CALL:
 *  
 *	strlen (s)
 *  
 * RETURNS:
 *
 *	x - length of string which does not inlcude terminating null
 *       
 * ARGUMENTS:
 *
 * 	char *s - address of string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
size_t strlen(char *s)    
{                               
    char *tmpstr = s;
        
    while (*tmpstr++ != '\0');
           
    return(tmpstr-s-1);         
}                                       

/*+
 * ============================================================================
 * = strcpy - copy strings                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This functions copies the string specified by the second argument
 *	to the area whose address is given in the first arg.
 *  
 * FORM OF CALL:
 *  
 *	strcpy (s1, s2)
 *  
 * RETURNS:
 *
 *	x - address of first string
 *       
 * ARGUMENTS:
 *
 *      char *s1	- address of first string
 *	char *s2	- address of second string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
char *strcpy(char *s1, char *s2)
{ 
    char *tmpstr = s2;
    char *res = s1;
  
    while (*s1++ = *tmpstr++);

    return(res);
} 
  
/*+
 * ============================================================================
 * = strncpy - counted string copy                                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This function copies the string specified by the second argument to 
 *	address specified in the first argument.  The number of characters
 *	copied is at most that specified by the third argument.
 *	If the source string is longer than the maximum copy length,
 *	the destination string will not be terminated by a null byte.
 *	If the source string is shorter than the specified maximum,
 *	then the destination is padded with null characters
 *	up to the maximum.
 *
 *	The destination string will not necessarily reflect the source string
 *	if the two strings overlap.
 *  
 * FORM OF CALL:
 *  
 *	strncpy (s1, s2, n)
 *  
 * RETURNS:
 *
 *	x - address of first string
 *       
 * ARGUMENTS:
 *
 *      char *s1	- address of first string
 *	cons char *s2	- address of second string
 *	int n		- number of bytes
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
char *strncpy(char *s1, char *s2, size_t n)
{ 
    char *tmpstr = s2;
    char *s = s1;
    int cnt = 0;
                
    while ((cnt++ < n) && (*s++ = *tmpstr++));
    while (cnt++ < n)
  	*s++ = '\0';
            
    return(s1);
}               

            
/*+
 * ============================================================================
 * = strcat - concatenate strings                                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This function catenates the two specified strings, by copying the string
 *	specified by the second argument to the end of the string specified by
 *	the first argument.
 *  
 * FORM OF CALL:
 *  
 *	strcat (s1, s2)
 *  
 * RETURNS:
 *
 *      x - address of first string
 *       
 * ARGUMENTS:
 *
 *      char *s1 -	    address of first string
 *	char *s2 -    address of second string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
char *strcat(char *s1, char *s2)
{               
    char *tmpstr = s2;
    char *s = s1;
            
    while (*s != '\0')
	s++;
    while ((*s++ = *tmpstr++) != '\0');
            
    return(s1);
}           

/*+
 * ============================================================================
 * = strncat - counted string concatenation                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This function catenates the two specified strings, by copying the string
 *	specified by the second argument to the end of the string specified by
 *	the first argument.  The number of bytes copied from the second string
 *	is at most that specified by the third argument.
 *  
 * FORM OF CALL:
 *  
 *	strncat(s1, s2, n)
 *  
 * RETURNS:
 *
 *      x - address of first string
 *       
 * ARGUMENTS:
 *
 *	char *s1 -		address of first string
 *	char *s2 -	address of second string
 *	int n -			number of bytes
 *
 * SIDE EFFECTS:
 *
 *	None
-*/
char *strncat(char *s1, char *s2, size_t n)
{           
    char *tmpstr = s2;
    char *res = s1;
    int cnt;
            
    while (*s1 != '\0')
	s1++;
    cnt = 0;
    while ((cnt++ < n) && ((*s1++ = *tmpstr++) != '\0'));
    if (cnt >= n && (*(s1-1) != '\0'))	    /* [001] */
	*s1 = '\0';

    return(res);
}   
  
/*+
 * ============================================================================
 * = strcspn -  name span a string with a character set                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This function matches characters from the string specified by first
 *	argument against the set of characters given by the string specified
 * 	by the second argument.  It returns the number of characters
 *	that precede the first matched character, i.e the first character
 *	in the string that is in the given character set.
 *  
 * FORM OF CALL:
 *  
 *	strcspn (s1, s2)
 *  
 * RETURNS:
 *
 *	x - indicates the length of the segment
 *       
 * ARGUMENTS:
 *
 *	char *s1 -    address of first string
 *	char *s2 -    address of second string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
size_t strcspn(char *s1, char *s2)
{ 
    char *str = s1;
    char *charset = s2;
    size_t res;
    char inlist[256];
    int i;

    memset (inlist, 0, sizeof (inlist));
    while (*charset != '\0') inlist [*charset++] = 1;

    res = 0;
    while ((*str != '\0') && (!inlist [*str++])) 
  	res++;

    return(res);
} 

/*+
 * ============================================================================
 * = strspn - match characters from a string                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This function matches characters from the string specified by first
 *	argument against the set of characters given by the string specified
 *	by the second argument.  It returns the number of characters that
 *	precede the first mismatched character, ie.e the first character
 *	in the string that is not in the given character set.
 *  
 * FORM OF CALL:
 *  
 *	strspn(s1, s2)
 *  
 * RETURNS:
 *
 *	x - indicates the length of the segment
 *       
 * ARGUMENTS:
 *
 *	char *s1 -    address of first string
 *	char *s2 -    address of second string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
size_t strspn(char *s1, char *s2)
{ 
    char *str = s1;
    char *charset = s2;
    size_t res;
    char inlist[256];
    int i;
  
    memset (inlist, 0, sizeof (inlist));
    while (*charset != '\0') inlist [*charset++] = 1;

    res = 0;
    while ((*str != '\0') && inlist [*str++]) 
  	res++;

    return(res);
}

/*+
 * ============================================================================
 * = strpbrk - search a string for specified set of characters                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	The strpbrk functions scans the characters in the string, stops
 *	when it encounters a character found in charset, and returns
 *	the address of the first character in the string that appears
 *	in the character set.
 *  
 * FORM OF CALL:
 *  
 *	strpbrk(s1, s2)                                  
 *  
 * RETURNS:
 *
 *	x - indicates the address of the first character in the string that is
 *	    in the set
 *       
 * ARGUMENTS:
 *
 *	char *s1 -    address of first string 
 *	char *s2 -    address of second string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
char *strpbrk(char *s1, char *s2)                                  
{                                                                              
    char *str = s1;                                                            
    char *charset = s2;                                                        
    char inlist[256];                                                          
    int i;

    memset (inlist, 0, sizeof (inlist));
    while (*charset != '\0') inlist [*charset++] = 1;

    while ((*str != '\0') && !inlist [*str])                          
	str++;                                                                 

    return(*str == '\0' ? NULL : str);                                         
}                                                                              
                                                                               
                                                                               
/*+
 * ============================================================================
 * = strstr - substring search function                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	The substring search routine.  Locate the first occurrence in the
 *	string pointed to by s1 of the sequence of characters in the string
 *	pointed to by s2.  
 *
 *	The declaration of strstr differs from that in the header file string.h.
 *	ANSI states that 'unsigned' is the default type for 'char'.  This is
 *	not the case in VAX C, therefore, the variables of strstr are
 *	explicitly defined as 'unsigned' char to compensate for the above
 *	difference.
 *
 *  
 * FORM OF CALL:
 *  
 *	strstr(s1, s2)                 
 *  
 * RETURNS:
 *
 *	x - pointer to the located string, or null if not found
 *       
 * ARGUMENTS:
 *
 *	unsigned char *s1 -   address of first string
 *	unsigned char *s2	-   address of second string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
char *strstr(unsigned char *s1, unsigned char *s2)                 
{                                                                              
                                                                               
    register unsigned char *target;                                            
    register unsigned char *tmpsrc;                                            
    register unsigned char *cursrc = s1;                                       
                                                                               
    if (*s2 == '\0')		/* '\0' matches everything */                  
	return((char *) cursrc);
                                                                               
    while (*cursrc != '\0') {                                                  
	if (*cursrc == *s2) {   /* if 1st chars match, then check the rest */  
	    target = s2;                                                       
	    tmpsrc = cursrc;                                                   
	    while (*target++ == *tmpsrc++) {                                   
		if (*(target) == '\0')    /* match is found. return it */      
		    return((char *) cursrc); 
		if (*(tmpsrc) == '\0')    /* no match exists */                
		    return(NULL);                                              
	    }
	}
	cursrc++;
    }
    return(NULL);	/* no match exists */
}	

/*+
 * ============================================================================
 * = strchr - find first occurance of a character in a string                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Find the first occurance of a character in a string and return the 
 *	address of that character.
 *  
 * FORM OF CALL:
 *  
 *	strchr (s0, c)
 *  
 * RETURNS:
 *
 *	x - address of found character or 0
 *       
 * ARGUMENTS:
 *
 *      char *s0 - string to search
 *	char c   - character to search for
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
char *strchr (char *s0, char c) {
	char	*s;

	s = s0;
	while (*s) {
	    if (*s == c) return s;
	    s++;
	}
	return 0;
}

/*+
 * ============================================================================
 * = strrchr - find last occurance of a character in a string                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Find the last occurance of a character in a string and return the 
 *	address of that character.
 *  
 * FORM OF CALL:
 *  
 *	strrchr (s0, c)
 *  
 * RETURNS:
 *
 *	x - address of found character or 0
 *       
 * ARGUMENTS:
 *
 *      char *s0 - string to search
 *	char c   - character to search for
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
char *strrchr (char *s0, char c) {
	char	*s,*p;
	int	found;

	found = 0;
	s = s0;
	while (*s) {
	    if (*s == c) {
		p = s;
		found = 1;
	    }
	    s++;
	}
	if (found) return p;
	else return 0;
}

/*+
 * ============================================================================
 * = strcount - count the number of occurances of a character in a string     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Count the number of occurences of a character class in a string.
 *	The character class is specified as a string.
 *  
 * FORM OF CALL:
 *  
 *	strcount (s, class)
 *  
 * RETURNS:
 *
 *	x - Number of characters in s that are also in class
 *       
 * ARGUMENTS:
 *
 *      char *s		- string to search
 *	char *class	- character class to count
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int strcount (char *s, char *class) {
    char *str = s;
    char *charset = class;
    char inlist[256];
    int res;
    int i;

    memset (inlist, 0, sizeof (inlist));
    while (*charset != '\0') inlist [*charset++] = 1;

    res = 0;
    while (*str != '\0') res += inlist [*str++];

    return(res);
}
	  
/*+
 * ============================================================================
 * = strcmp - compare two strings                                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Compare two strings lexicographically.
 *  
 * FORM OF CALL:
 *  
 *	strcmp (s0, s1)
 *  
 * RETURNS:
 *
 *	minus 1		- s1 < s0
 *	0		- s1 == s0
 * 	1		- s1 > s0
 *       
 * ARGUMENTS:
 *
 *      char unsigned *s0 -	address of first null terminated string
 *      char unsigned *s1 -	address of second null terminated string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int strcmp (char unsigned *s0, char unsigned *s1) {
	char *t0, *t1;

	t0 = (char *) s0;
	t1 = (char *) s1;

	while (*t0  && (*t0 == *t1)) t0++, t1++;
	if (*t0 == *t1) return 0;
	if (*t0 > *t1) return 1;
	return -1;
}

/*+
 * ============================================================================
 * = strncmp - compare two strings up to a predetermined length               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Compare two strings lexicographically up to a predtermined length.
 *  
 * FORM OF CALL:
 *  
 *	strncmp (s0, s1, length)
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
 *	int length - maximum number of characters to check	
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int strncmp (char unsigned *s0, char unsigned *s1, int length) {
	char *t0, *t1;
	int	l;

	t0 = (char *) s0;
	t1 = (char *) s1;
	l = length;

	while ((--l >= 0)  &&  *t0  && (*t0 == *t1)) t0++, t1++;
	if (l < 0) return 0;
	if (*t0 == *t1) return 0;
	if (*t0 > *t1) return 1;
	return -1;
}
/*+
 * ============================================================================
 * = strcmp_nocase - compare two strings on a case-insensitive basis          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Compare two strings lexicographically.  Letters represented in 
 *	different case are treated as matching (a == A).
 *  
 * FORM OF CALL:
 *  
 *	strcmp_nocase (s0, s1)
 *  
 * RETURNS:
 *
 *	minus 1		- s1 < s0
 *	0		- s1 == s0
 * 	1		- s1 > s0
 *       
 * ARGUMENTS:
 *
 *      char unsigned *s0 -	address of first null terminated string
 *      char unsigned *s1 -	address of second null terminated string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int strcmp_nocase (char unsigned *s0, char unsigned *s1) {
	char *t0, *t1;

	t0 = (char *) s0;
	t1 = (char *) s1;

	while (*t0  && (tolower(*t0) == tolower(*t1))) t0++, t1++;
	if (tolower(*t0) == tolower(*t1)) return 0;
	if (tolower(*t0) > tolower(*t1)) return 1;
	return -1;
}

/*+
 * ============================================================================
 * = strwcmp_nocase - compare two strings, case-insensitive, wild chars	      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Compare two strings lexicographically.  Letters represented in 
 *	different case are treated as matching (a == A). Allow wild card (?)
 *	match.
 *  
 * FORM OF CALL:
 *  
 *	strwcmp_nocase (s0, s1)
 *  
 * RETURNS:
 *
 *	minus 1		- s1 < s0
 *	0		- s1 == s0
 * 	1		- s1 > s0
 *       
 * ARGUMENTS:
 *
 *      char unsigned *s0 -	address of first null terminated string
 *      char unsigned *s1 -	address of second null terminated string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int strwcmp_nocase(char unsigned *s0, char unsigned *s1)
{
    char *t0, *t1;

    t0 = (char *) s0;
    t1 = (char *) s1;

    while (*t0 && ((tolower(*t0) == tolower(*t1)) || (*t0 == '?') || (*t1 == '?')))
	t0++, t1++;
    if (tolower(*t0) == tolower(*t1))
	return 0;
    if (tolower(*t0) > tolower(*t1))
	return 1;
    return -1;
}

/*+
 * ============================================================================
 * = strncmp_nocase - compare two strings on case-insensitive basis to length =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Compare two strings lexicographically up to a predtermined length.
 *	Letters represented in different case are treated as matching (a == A).
 *  
 * FORM OF CALL:
 *  
 *	strncmp_nocase (s0, s1, length)
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
 *	int length - maximum number of characters to check	
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int strncmp_nocase (char unsigned *s0, char unsigned *s1, int length) {
	char *t0, *t1;
	int	l;

	t0 = (char *) s0;
	t1 = (char *) s1;
	l = length;

	while ((--l >= 0)  &&  *t0  && (tolower(*t0) == tolower(*t1))) 
	  t0++, t1++;
	if (l < 0) return 0;
	if (tolower(*t0) == tolower(*t1)) return 0;
	if (tolower(*t0) > tolower(*t1)) return 1;
	return -1;
}

/*+
 * ============================================================================
 * = substr - see if first string is a substring of a second string	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Compare two strings for whether the first is an abbreviation for the
 *	second, or exactly = the second or neither.  Case sensitive.
 *
 * FORM OF CALL:
 *
 *	substr (s, t)
 *
 * RETURN CODES:
 *
 *      0       - no match
 *      1       - exact match
 *      2       - substring
 *
 * ARGUMENTS:
 *
 *	char *s - string representing the possible substring
 *	char *t - string representing the full word
 *
 * SIDE EFFECTS:
 *
 *	none
 *
-*/
int substr (char *s, char *t)
{
        while (1) {
            if (*s==0) {
                return (*t==0) ? 1 :2;
            }
            if (*t==0) return (0);
            if ( *s != *t ) return (0);
            s++;
            t++;
        }
}

/*+
 * ============================================================================
 * = substr_nocase - see if first string is a substring of a second string    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Compare two strings for whether the first is an abbreviation for the
 *	second, or exactly = the second or neither.  Case insensitive.
 *
 * FORM OF CALL:
 *
 *	substr_nocase (s, t)
 *
 * RETURN CODES:
 *
 *      0       - no match
 *      1       - exact match
 *      2       - substring
 *
 * ARGUMENTS:
 *
 *	char *s - string representing the possible substring
 *	char *t - string representing the full word
 *
 * SIDE EFFECTS:
 *
 *	none
 *
-*/
int substr_nocase(char *s, char *t)
{
        while (1) {
            if (*s==0) {
                return (*t==0) ? 1 :2;
            }
            if (*t==0) return (0);
            if ( tolower(*s) != tolower(*t) ) return (0);
            s++;
            t++;
        }
}

/*+
 * ============================================================================
 * = strelement - extract the nth element from a string                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Extract the Nth element (origin 0) from a string, where the elements
 *	are separated by a given character set.  Multiple occurances of the
 *	same separator are treated as one separator.  This function is similar
 *	in function to STRTOK, with the added advantage of being reentrant.
 *  
 * FORM OF CALL:
 *  
 *	strelement (string, n, charset, element)
 *  
 * RETURNS:
 *
 *	x - address of element
 *	0 - no element found
 *       
 * ARGUMENTS:
 *
 * 	char *string - input string
 *	int n - element number origin 0
 *	char *charset - set of character that define field separators
 *	char *element - extracted string
 *
 * SIDE EFFECTS:
 *
 *
-*/
int strelement (char *string, int n, char *set, char *element) {
	int	i;
	char	*s = string;
	char	*charset = set;
	char	*dst = element;
	char	inlist [256];

	memset (inlist, 0, sizeof (inlist));
	while (*charset != '\0') inlist [*charset++] = 1;

	while (1) {
	    while (*s && inlist [*s]) s++;	/* skip leading separators */
	    if (n == 0) break;
	    if (*s == 0) break;
	    while (*s && !inlist [*s]) s++;	/* skip field	*/
	    n--;
	}

	/*
	 * copy field
	 */
	while (*s && !inlist [*s]) *dst++ = *s++;
	*dst = '\0';
	return *element;
}
#if 0
/*+
 * ============================================================================
 * = strunion - form the union of two strings.                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Form the union of two strings.  The resulting string contains all the
 *	chararcters in the first string, and all the characters in the second
 *	string.  Characters that appear in both source strings appear only once
 *	in the resulting string.
 *
 *	The order of the characters in the resulting string is unpredicatable.
 *	The sources strings may not overlap the resulting string.
 *
 * FORM OF CALL:
 *  
 *	strunion (dst, s1, s2)
 *  
 * RETURNS:
 *
 *	n - length of resulting string
 *       
 * ARGUMENTS:
 *
 * 	char *dst - address of destination string
 *	char *s1 - address of first source string
 *	char *s2 - address of second source string
 *
 * SIDE EFFECTS:
 *
 *
-*/
int strunion (unsigned char *dst, unsigned char *s1, unsigned char *s2) {
	unsigned char *s;
	unsigned char charset [256];
	int	i;

	memset (charset, 0, sizeof (charset));

	s = s1; while (*s) charset [*s++] = 1;
	s = s2; while (*s) charset [*s++] = 1;

	s = dst;
	for (i=0; i<sizeof (charset)/sizeof (charset [0]); i++) {
	    if (charset [i]) *s++ = i;
	}
	*s = 0;
	return s - dst;
}
#endif

#define LOCAL_DEBUG 0
#if LOCAL_DEBUG

void main (int argc, char *argv []) {
	int	i;
	char	token [128];

	i = 0;
	while (strelement (argv [1], i, " ", token)) {
	    printf ("%d: |%s|\n", i, token);
	    i++;
	}
}
#endif

int strupper (unsigned char *s) {
    char *t;
        
    for (t = s; *t; t++) {
	*t = toupper (*t);
    }
           
    return s;
}

int strlower (unsigned char *s) {
    char *t;
        
    for (t = s; *t; t++) {
	*t = tolower (*t);
    }
           
    return s;
}
