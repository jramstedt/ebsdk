#ifndef __HALP_H_LOADED
#define __HALP_H_LOADED
/*****************************************************************************

       Copyright 1993, 1994, 1995 Digital Equipment Corporation,
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
 *  $Id: halp.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $
 */

/*
 * $Log: halp.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1995/10/03  03:24:44  fdh
 * Removed mb() declaration and corrected a couple of prototypes.
 *
 * Revision 1.2  1995/09/02  07:03:27  fdh
 * Removed references to WaitUs.
 *
 * Revision 1.1  1995/07/05  17:57:28  cruz
 * Initial revision
 *
 *
 */
#include "nttypes.h"

#define HalpMb mb
#define KeStallExecutionProcessor usleep

extern VOID
WRITE_PORT_UCHAR (
                       IN PUCHAR AddressP, 
                       IN UCHAR Value
                       );

extern VOID 
WRITE_REGISTER_UCHAR (
                      IN PUCHAR AddressP,
                      IN UCHAR Data
                      );

extern UCHAR
READ_REGISTER_UCHAR (
                      IN PUCHAR AddressP
                      );

extern VOID 
WRITE_REGISTER_ULONG (
                      IN PULONG AddressP,
                      IN ULONG Data
                      );

extern ULONG
READ_REGISTER_ULONG (
		     IN PULONG AddressP
                      );


#endif /* __HALP_H_LOADED */


