#ifndef __VARARGS_LOADED
#define __VARARGS_LOADED	1

/*	VARARGS - V3.0	*/

/*
 * Definitions for access to variable length argument lists
 */

#ifdef __STDARG_LOADED
#undef __STDARG_LOADED
#undef va_start
#undef va_arg
#undef va_end
#else
typedef char *	va_list;
#endif

#define va_dcl			int	va_alist;

#define va_count(count)		(count = __VA_COUNT_BUILTIN)
#define va_start_1(ap, offset)	ap = (va_list) ((int) __VA_START_BUILTIN(va_alist) + (offset))

#define va_start(ap)		ap = (va_list) __VA_START_BUILTIN(va_alist)
#define va_arg(ap, type)	(ap = (va_list) ((int) ap + 8), \
				 sizeof (type) > 8 ? \
				 (* (type *)(* (int *) ((int) ap - 8))) : \
				 (* (type *)((int) ap - 8)))

#define va_end(ap)		ap = (va_list) 0

#endif					/* __VARARGS_LOADED */
