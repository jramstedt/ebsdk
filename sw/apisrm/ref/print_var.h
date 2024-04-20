/*
 * print_var.h 
 *
 * Copyright (C) 1992 by
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Variable number of argument macros
 *
 * Author:	John R. Kirchoff
 *
 * Modifications:
 *
 *	jrk	12-May-1992	Initial entry.
 */

/*
 * macros with variable number of arguments
 * These are used to trick C into having macros with optional arguments.
 */

#define p_args0 0
#define p_args1(arg1) \
	arg1
#define p_args2(arg1, arg2) \
	arg1, arg2
#define p_args3(arg1, arg2, arg3) \
	arg1, arg2, arg3
#define p_args4(arg1, arg2, arg3, arg4) \
	arg1, arg2, arg3, arg4
#define p_args5(arg1, arg2, arg3, arg4, arg5) \
	arg1, arg2, arg3, arg4, arg5
#define p_args6(arg1, arg2, arg3, arg4, arg5, arg6) \
	arg1, arg2, arg3, arg4, arg5, arg6
#define p_args7(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
	arg1, arg2, arg3, arg4, arg5, arg6, arg7
#define p_args8(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
	arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8
#define p_args9(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
	arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9
#define p_args10(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) \
    arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10

#if 0

/* Example usage: */

#define DEBUG 1

#if DEBUG
#define d_pnt(fmt, args)	\
    d_printf(fmt, args)
#else
#define d_pnt(fmt, args)
#endif

main()
{
    d_pnt("test\n", p_args0);
    d_pnt("test 1, %x\n", p_args1(1));
    d_pnt("test 2, %x %x\n", p_args2(1, 2));
    d_pnt("test 3, %s %x %s\n", p_args3("first", 2, "last"));
}

d_printf(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10)
{
    printf(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}
#endif
