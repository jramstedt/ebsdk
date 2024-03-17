#ifndef __PRTRACE_H_LOADED
#define __PRTRACE_H_LOADED
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
 * $Id: prtrace.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $
 */

/*
 * MODULE DESCRIPTION:
 * 
 *     Tracing macros for EB64 monitor
 *
 * HISTORY:
 *     $Log: prtrace.h,v $
 *     Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 *     Initial CVS checkin
 *
 * Revision 1.6  1995/10/13  19:45:51  cruz
 * Added definitions for new debugging macros which allow the user
 * to control what debugging information gets printed out.
 *
 * Revision 1.5  1994/08/08  00:27:52  fdh
 * Added TRACE_ENABLE conditional.
 *
 * Revision 1.4  1994/08/05  20:13:47  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/06/20  14:18:59  fdh
 * Fixup header file preprocessor #include conditionals.
 *
 * Revision 1.2  1994/03/09  12:47:12  berent
 * Added PRINTERR macros
 *
 * Revision 1.1  1993/06/22  11:19:52  berent
 * Tracing macros implemementing PRTRACE compile time
 * option.
 *
 */

/* The following macros are used for printing out status information and internal error */
/* messages; such messages go to standard error in environments in which there is a */
/* standard error file */
#define PRINTERR1(x) printf(x)
#define PRINTERR2(x,y) printf(x,y)
#define PRINTERR3(x,y,z) printf(x,y,z)


/*
 * Tracing with debug prints using the PRTRACE
 * macros (see prtrace.h) can be performed by
 * two different means. (1) Defining PRTRACE
 * will build in the tracing at compile time.
 * (2) Defining TRACE_ENABLE will also build
 * in tracing at compile time.  However, most
 * of it can be enabled or disabled at run
 * time with the debug monitor prtrace command.
 */

#ifdef TRACE_ENABLE
#define PRTRACE

extern int prtrace_enable;
#define _PRTRACE_PRINTF if (prtrace_enable) printf
#else
#define _PRTRACE_PRINTF printf
#endif

#ifdef PRTRACE
#define PRTRACE1(x) _PRTRACE_PRINTF(x)
#define PRTRACE2(x,y) _PRTRACE_PRINTF(x,y)
#define PRTRACE3(x,y,z) _PRTRACE_PRINTF(x,y,z)
#else
#define PRTRACE1(x)
#define PRTRACE2(x,y)
#define PRTRACE3(x,y,z)
#endif

#ifdef PRTRACE
typedef struct {
    ui   value;
    char *valuename;
} VARIABLE_VALUES;

#define DEBUG_PRINT(var, cond, str) if (var & (cond)) printf str
#define DEBUG_DO(var, cond, str) if (var & (cond)) {str}
#else
#define DEBUG_PRINT(var, cond, str) 
#define DEBUG_DO(var, cond, str) 
#endif /* PRTRACE */

#endif /* __PRTRACE_H_LOADED */
