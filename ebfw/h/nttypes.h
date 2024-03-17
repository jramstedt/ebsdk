#ifndef __NTTYPES_H_LOADED
#define __NTTYPES_H_LOADED
/*****************************************************************************

       Copyright 1995 Digital Equipment Corporation,
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
 *  $Id: nttypes.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $
 *
 * Defines some of the data types used by the NT firmare.
 * These are defined in the context of the Debug Monitor
 * environment established in "lib.h".
 *
 * $Log: nttypes.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.5  1997/02/21  04:20:54  fdh
 * Removed VOID definition.
 *
 * Revision 1.4  1996/05/22  21:34:03  fdh
 * Added the SHORT definition.
 *
 * Revision 1.3  1995/10/03  11:57:15  fdh
 * Merged redundant definitions with lib.h.
 *
 * Revision 1.2  1995/08/25  19:37:47  fdh
 * Imported more definitions.
 *
 * Revision 1.1  1995/07/05  17:59:12  cruz
 * Initial revision
 *
 *
 */

#include "lib.h"

#undef IN
#define IN
#undef OUT
#define OUT

typedef ub		UCHAR;
typedef char	      * PCHAR;
typedef ub	      * PUCHAR;
typedef ui		ULONG;
typedef ui	      * PULONG;
typedef signed int      LONG;
/* typedef void            VOID; */
#define VOID		void
typedef void          * PVOID;
typedef uw		USHORT;
typedef sw		SHORT;

#ifdef _WIN32
typedef __int64         LONGLONG;
typedef ul		ULONGLONG;
#else
typedef long            LONGLONG;
typedef ul		ULONGLONG;
#endif

typedef LONGLONG PHYSICAL_ADDRESS;
#endif /* __NTTYPES_H_LOADED */


