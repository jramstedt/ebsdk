/*****************************************************************************

Copyright 1994, 1995 Digital Equipment Corporation, Maynard, Massachusetts. 

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

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: halp.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: halp.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1995/10/03  03:20:41  fdh
 * Initial revision
 *
 */

#include "halp.h"


VOID
WRITE_PORT_UCHAR (
                       IN PUCHAR AddressP, 
                       IN UCHAR Value
                       )
{
    outportb( (ui)AddressP, (ui)Value);
}



VOID 
WRITE_REGISTER_UCHAR (
                      IN PUCHAR AddressP,
                      IN UCHAR Data
                      )
{
ULONG Address = (ULONG)AddressP;

   if (((Address >> 28) & 0xE) == 0xA) {  // Is it spare space?
       Address &= ~(0xA << 28);
       outmemb( (ui)Address, (ui)Data);
   }
   else {
       outdmemb( (ui)Address, (ui)Data);
   }
}

UCHAR
READ_REGISTER_UCHAR (
                      IN PUCHAR AddressP
                      )
{
ULONG Address = (ULONG)AddressP;

   if (((Address >> 28) & 0xE) == 0xA) { // Is it spare space?
       Address &= ~(0xA << 28);
       return (UCHAR) inmemb((ui)Address);
   }
   else {
       return (UCHAR) indmemb((ui)Address);
   }
}


VOID 
WRITE_REGISTER_ULONG (
                      IN PULONG AddressP,
                      IN ULONG Data
                      )
{
ULONG Address = (ULONG)AddressP;

   if (((Address >> 28) & 0xE) == 0xA) {  // Is it spare space?
       Address &= ~(0xA << 28);
       outmeml( (ui)Address, (ui)Data);
   }
   else {
       outdmeml( (ui)Address, (ui)Data);
   }
}

ULONG
READ_REGISTER_ULONG (
                      IN PULONG AddressP
                      )
{
ULONG Address = (ULONG)AddressP;

   if (((Address >> 28) & 0xE) == 0xA) { // Is it spare space?
       Address &= ~(0xA << 28);
       return (ULONG) inmeml((ui)Address);
   }
   else {
       return (ULONG) indmeml((ui)Address);
   }
}
