
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
__attribute__((unused)) static const char *rcsid = "$Id: eb64mck.c,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $";
#endif

/*
 * $Log: eb64mck.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1996/05/22  22:08:45  fdh
 * Initial revision
 *
 */

#include "lib.h"
#include "mon.h"
#include "mcheck.h"

#ifndef MAKEDEPEND
#include "impure.h"		/* For logout area */
#include "cserve.h"
#include "dc21064.h"
#endif

void DumpLogout(LogoutFrame_t *Frame)
{
  Beep(100,1000);

  printf("Logout area address = 0x%x, size = 0x%x\n",
	 Frame, Frame->FrameSize);
  PrintMem(2, (ul) Frame,
	   (ul) (Frame+(Frame->FrameSize/4)), 0 ,0);

  printf("\n");
}

void DumpShortLogout(LogoutFrame_t *Frame)
{
  printf("Logout area address = 0x%x, size = 0x%x\n",
	 Frame, Frame->FrameSize);
  PrintMem(2, (ul) Frame,
	   (ul) (Frame+(Frame->FrameSize/4)), 0 ,0);
}


void ParseLogout(LogoutFrame_t *Frame)
{
}

void SetMcheckSystem(State_t state)
{

  switch(state) {
  case StateOn:
  case StateCPU:
    break;

  case StateOff:
    break;

  case StateSystem:
  default:
    break;
  }

  switch(state) {
  case StateOn:
  case StateSystem:
    break;

  case StateOff:
    break;

  case StateCPU:
  default:
    break;
  }
}
