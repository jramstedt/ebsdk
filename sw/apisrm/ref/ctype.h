/*	CTYPE - V3.0 - Character Type Classification Macros	*/

#ifndef __CTYPE_LOADED
#define __CTYPE_LOADED	1

#define _U	0x1
#define _L	0x2
#define _D	0x4
#define _S	0x8
#define _P	0x10
#define _C	0x20
#define _X	0x40
#define _B	0x80


extern	char _ctype_ [];
static char *_ctype_ptr[] = &_ctype_;
 
#define isupper(c)	((*_ctype_ptr)[(c) & 0xFF] & _U)
#define islower(c)	((*_ctype_ptr)[(c) & 0xFF] & _L)
#define isalpha(c)	((*_ctype_ptr)[(c) & 0xFF] & (_U | _L))
#define isdigit(c)	((*_ctype_ptr)[(c) & 0xFF] & _D)
#define isalnum(c)	((*_ctype_ptr)[(c) & 0xFF] & (_U | _L | _D))
#define isxdigit(c)	((*_ctype_ptr)[(c) & 0xFF] & _X)
#define isspace(c)	((*_ctype_ptr)[(c) & 0xFF] & _S)
#define ispunct(c)	((*_ctype_ptr)[(c) & 0xFF] & _P)
#define isgraph(c)	((*_ctype_ptr)[(c) & 0xFF] & (_P | _U | _L | _D))
#define isprint(c)	((*_ctype_ptr)[(c) & 0xFF] & (_P | _U | _L | _D | _B))
#define iscntrl(c)	((*_ctype_ptr)[(c) & 0xFF] & _C)
#define isascii(c)	((unsigned)(c) <= 0x7F)

#define isodigit(c)	(('0' <= (c))  && ((c) <= '7'))

#define toupper(c)	(islower (c) ? (c) - 0x20 : (c))
#define tolower(c)	(isupper (c) ? (c) + 0x20 : (c))

#define toascii(c)	((c) & 0x7F)

#endif				/* __CTYPE_LOADED */
