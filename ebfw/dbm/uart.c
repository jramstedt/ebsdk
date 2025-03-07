
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
__attribute__((unused)) static const char *rcsid = "$Id: uart.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * $Log: uart.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.17  1997/08/14  14:34:36  pbell
 * Added support for 115200 and 57600 baud rates.
 *
 * Revision 1.16  1997/04/29  21:50:58  pbell
 * Changed UartGetChar to use 8 bits not 7
 *
 * Revision 1.15  1997/04/23  17:56:18  pbell
 * Changed all functions to be independent of the
 * actual port that they are operating on.  The new
 * Uart.......( int PortBase ... functions will
 * work with any STD uart at a given address.
 *
 * Revision 1.14  1997/02/21  03:12:59  fdh
 * Added a 50usec delay on retries to output characters.
 *
 * Revision 1.13  1996/05/16  14:56:34  fdh
 * Removed use of inportb and outportb aliases.
 *
 * Revision 1.12  1995/10/26  21:31:18  cruz
 * Added casting for a couple of function invocations.
 *
 * Revision 1.11  1995/09/05  18:28:32  fdh
 * Moved vga, etc. out of this module.  Now supports only
 * serial ports and parallel port.
 *
 * Revision 1.10  1995/08/25  20:13:23  fdh
 * Added support for Digital TGA, and VGA BIOS Emulation.
 * If a keyboard is detected and initialized the Graphics
 * is initialized.
 *
 * Revision 1.9  1994/08/07  06:44:52  fdh
 * Flush the receive buffers when initing the com ports.
 *
 * Revision 1.8  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.7  1994/07/28  17:14:53  fdh
 * Removed unused definitions.
 *
 * Revision 1.6  1994/06/21  14:29:39  rusling
 * fixed up WNT compile warnings.
 *
 * Revision 1.5  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.4  1994/01/19  10:40:08  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.3  1993/11/22  16:02:20  rusling
 * Moved the IO Bus init call out of uart_init() and
 * explicitly into main() (in /rom/cmd.c).
 *
 * Revision 1.2  1993/11/19  17:03:16  fdh
 * Fixup include declarations.
 *
 * Revision 1.1  1993/06/08  19:56:26  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "mon.h"
#include "lib.h"
#include "uart.h"

const int UartPortBaseTable[] = { 0x3f8, 0x2f8, 0x3e8, 0x2e8 };

/*--------------------------------------------------------------------------
 * Initialize the Comm port at the specified address and baud rate.
 * Returns FALSE if the port does not exist else TRUE
 *-------------------------------------------------------------------------*/

BOOLEAN UartInitLine(int PortBase, int baud)
{
    int i;
    int baudconst;

    switch (baud)
    {
    case 115200:baudconst =   1; break;
    case 57600: baudconst =   2; break;
    case 38400: baudconst =   3; break;
    case 19200: baudconst =   6; break;
    default:
    case 9600:	baudconst =  12; break;
    case 4800:	baudconst =  24; break;
    case 2400:	baudconst =  48; break;
    case 1200:	baudconst =  96; break;
    case 300:	baudconst = 384; break;
    case 150:	baudconst = 768; break;
    }

    outportb(PortBase+com_Lcr, 0x87);
    if( inportb(PortBase+com_Lcr) != 0x87 )
	return( FALSE );
    outportb(PortBase+com_Dlm, 0);
    outportb(PortBase+com_Dll, baudconst);
    outportb(PortBase+com_Lcr, 0x07);
    if( inportb(PortBase+com_Lcr) != 0x07 )
	return( FALSE );
    outportb(PortBase+com_Mcr, 0x0F);

    for(i=10;i>0 && inportb(PortBase+com_Lsr) != (ui)0;i--)
	inportb(PortBase+com_Rbr);
    return( TRUE );
}


BOOLEAN UartCharAv( int PortBase )
{
    return((inportb(PortBase+com_Lsr) & 1) != 0);
}


char UartGetChar( int PortBase )
{
    return((char)inportb(PortBase+com_Rbr) & 0x0FF);
}


/*--------------------------------------------------------------------------
 * Read the next character from the port waiting if necessary.
 * Returns: 1 if a character is read, 0 if it timed out.
 *-------------------------------------------------------------------------*/

char UartGetNextChar(
    int	    PortBase,	/* IO Port address */
    char *  c,		/* Address to store the character read */
    int	    WaitLimit)	/* Wait limit in miliseconds */
{
    long Waited = 0;
    long FullWaitLimit = 1000 * WaitLimit;
    do
    {
	if( UartCharAv( PortBase ) )
	{
	    *c = UartGetChar( PortBase );
	    return( 1 );
	}

	if( Waited >= FullWaitLimit )
	    return( 0 );

	usleep(80);	/* at 115.2K baud chars are ~86us apart */
	Waited += 80;
    }
    while( 1 );
}


void UartPutChar( int PortBase, char c )
{ 
    while ((inportb(PortBase + com_Lsr) & 0x20) == 0)
        usleep(50);
    outportb(PortBase + com_Thr,(ui)c);
    if (c == '\n')
	UartPutChar( PortBase, (char) '\r');
}


/*--------------------------------------------------------------------------
 * Read all available characters from the port
 *-------------------------------------------------------------------------*/

void UartFlushReadBuffer( int PortBase )
{
    while( UartCharAv( PortBase ) )
	UartGetChar( PortBase );
}

void putcLpt(int c)
{
   int i;
   while(!(inportb(lptSr) & 0x80))
	;
   for(i=0;i<lptDly;++i)
	;
   outportb(lptDr,    c);
   for(i=0;i<lptDly;++i)
	;
   outportb(lptCr,   lptnInit|lptSTB|lptAFD);
   for(i=0;i<lptDly;++i)
	;
   outportb(lptCr,   lptnInit|lptAFD);
   for(i=0;i<lptDly;++i)
	;
   (void)inportb(lptSr);
}


