
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
__attribute__((unused)) static const char *rcsid = "$Id: sromport.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * $Log: sromport.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1997/08/14  20:50:09  pbell
 * Moved the display message into SromPortInit.
 *
 * Revision 1.3  1997/08/12  02:34:11  fdh
 * Output LED code before and after initializing SROM
 * serial port speed.
 *
 * Revision 1.2  1997/02/21  03:15:52  fdh
 * Modified to save line status in a global variable
 * instead of a PALtemp.
 *
 * Revision 1.1  1996/05/22  21:54:58  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "mon.h"
#include "lib.h"
#include "palcsrv.h"
#include "ledcodes.h"

BOOLEAN SromPortInitialized;
static ul LineStatus;

int SromCharav(void)
{
  return TRUE;
}

int SromGetChar(void)
{
  return ((int) cServe(0, 0, CSERVE_K_SROM_GETC, LineStatus));
}

void  SromPutChar(char c)
{ 
  cServe(0, (ul) c, CSERVE_K_SROM_PUTC, LineStatus);
  if (c == '\n') SromPutChar((char) '\r');
}

int SromPortInit( BOOLEAN PromptUser )
{
  if( PromptUser )
  {
    printf("Initializing SROM Serial Port.\n");
    printf("\tType an uppercase U into SROM Serial Port to enable it\n");
  }
  outLed(led_k_autobaud);
  LineStatus = cServe(0, 0, CSERVE_K_SROM_INIT);
  outLed(led_k_autobaud_done);
  SromPortInitialized = TRUE;
  return TRUE;
}
