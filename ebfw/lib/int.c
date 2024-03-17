
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

#ifndef LINT
static char *rcsid = "$Id: int.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: int.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1994/11/07  12:05:39  rusling
 * Now include system.h as well as lib.h
 *
 * Revision 1.3  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.2  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.1  1993/06/08  19:56:22  fdh
 * Initial revision
 *
 */



#include "system.h"
#include "lib.h"
#include "int.h"

void intr_enable(int int_level)
{
   int INT1mask, INT2mask;

   INT1mask = inportb( INT1_MASK );
   INT2mask = inportb( INT2_MASK );

  switch( int_level ){
  case 15:
  case 14:
  case 12:
  case 11:
  case 10:
  case 9:
     INT2mask &= ~(1<<(int_level-8));
   case 7:
   case 6:
   case 5:
   case 4:
   case 3:
     INT1mask &= ~( 1 << ((int_level>7)?2:int_level));
     break;
   default:
     printf( "undefined ISA interrupt level = %d\n", int_level );
   }

   INT2mask &= 0xff;
   INT1mask &= 0xff;

   outportb( INT2_MASK, INT2mask ); /* turn on requested interrupt level*/
   outportb( INT1_MASK, INT1mask ); /* pass through controller 1 */
}
 
