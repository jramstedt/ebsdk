/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
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
__attribute__((unused)) static const char *rcsid = "$Id: mouse.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * Abstract:	This file contains routines to probe for known mice
 *		on a given port.
 *
 * Author:	Peter J. Bell   1997/04/16
 *	        Derived from Jeff Wiedemeier's mouse.c
 *
 * Revision History:
 * $Log: mouse.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1997/04/30  21:14:44  pbell
 * Added static modifiers on all internal functions.
 *
 * Revision 1.1  1997/04/23  17:55:23  pbell
 * Initial revision
 *
 */

#include "lib.h"
#include "uart.h"

#define MSER_LINE_PROTOCAL	(com_7BW|com_1SB)
#define MSER_MIN_RESET          200
#define MSER_BAUDRATE           1200
#define MSER_MAX_RESET_BUFFER   8

#define CSER_POWER_UP   500
#define CSER_POWER_DOWN 500

typedef struct COMMSETTING
{
    int DLM;
    int DLL;
    int LineControl;
    int ModemControl;
} COMMSETTING;

static BOOLEAN MSerPowerUp    ( int PortBase );
static BOOLEAN MSerPowerDown  ( int PortBase );
static BOOLEAN MSerReset      ( int PortBase );
static BOOLEAN MSerDetect     ( int PortBase );
static void MSerGetCommSetting( int PortBase, COMMSETTING * Comm );
static void MSerRestoreCommSetting( int PortBase, COMMSETTING * Comm );

/*--------------------------------------------------------------------------
 * Read and save the current line settings for the specified Comm port
 *-------------------------------------------------------------------------*/

static void MSerGetCommSetting( int PortBase, COMMSETTING * Comm )
{
    Comm->LineControl	= inportb(PortBase+com_Lcr);
    Comm->ModemControl	= inportb(PortBase+com_Mcr);
    outportb(PortBase+com_Lcr, 0x87);
    Comm->DLM	    	= inportb(PortBase+com_Dlm);
    Comm->DLL	    	= inportb(PortBase+com_Dll);
    outportb(PortBase+com_Lcr, Comm->LineControl);
}

/*--------------------------------------------------------------------------
 * Reset the comm port to the saved settings
 *-------------------------------------------------------------------------*/

static void MSerRestoreCommSetting( int PortBase, COMMSETTING * Comm )
{
    int i;

    outportb(PortBase+com_Lcr, 0x87);
    outportb(PortBase+com_Dlm, Comm->DLM);
    outportb(PortBase+com_Dll, Comm->DLL);
    outportb(PortBase+com_Lcr, Comm->LineControl);
    outportb(PortBase+com_Mcr, Comm->ModemControl);

    for(i=10;i>0 && inportb(PortBase+com_Lsr) != (ui)0;i--)
	inportb(PortBase+com_Rbr);
}

/*--------------------------------------------------------------------------
 * Power up the mouse. Set RTS and DTR. Wait 10 ms.
 * Returns TRUE
 *-------------------------------------------------------------------------*/

static BOOLEAN MSerPowerUp( int PortBase )
{
    outportb( PortBase+com_Mcr, com_RTS | com_DTR );
    /*
     * Wait 10 ms.  The power-up response byte(s) should take at least
     * this long to get transmitted.
     */
    msleep(10);
    return TRUE;
}


/*--------------------------------------------------------------------------
 * Power down the mouse. Clear RTS, Set DTR. Wait for mouse to reset.
 * Returns TRUE
 *-------------------------------------------------------------------------*/

static BOOLEAN MSerPowerDown( int PortBase )
{
    ub LineCtrl = inportb(PortBase+com_Mcr);
    outportb( PortBase+com_Mcr, ( LineCtrl & ~com_RTS ) | com_DTR );
    /*
     * Keep RTS low for at least 150 ms, in order to correctly power
     * down older Microsoft serial mice.  Wait even longer to avoid
     * sending some Logitech CSeries mice into the floating point world...
     */
    msleep(CSER_POWER_DOWN);
    return TRUE;
}


/*--------------------------------------------------------------------------
 * Reset a microsoft serial mouse if one exists
 * Returns TRUE
 *-------------------------------------------------------------------------*/

static BOOLEAN MSerReset( int PortBase )
{
    /* Power down the mouse */
    MSerPowerDown( PortBase );
    /* Clean possible garbage in uart input buffer. */
    UartFlushReadBuffer( PortBase );
    /* Power up the mouse (reset). */
    MSerPowerUp( PortBase );

    return TRUE;
}


/*--------------------------------------------------------------------------
 * Detect a Microsoft serial protocal mouse
 * Returns 0 if none found, 1 if a mouse is detected.
 *-------------------------------------------------------------------------*/

static BOOLEAN MSerDetect( int PortBase )
{
    char ReceiveBuffer[MSER_MAX_RESET_BUFFER];
    size_t Count;
    size_t i;

    /* Set the baud rate for the mouse */
    UartInitLine( PortBase, MSER_BAUDRATE );
    /* set the line protocal */
    outportb(PortBase+com_Lcr, MSER_LINE_PROTOCAL);
    /* Reset the mouse */
    MSerReset(PortBase);

    /*
     * This code assumes that if a CSeries mouse is attached we will
     * respect its power up time - to accomplish this we use the
     * CSER_POWER_UP time as our timeout value in reading from the port.
     * To make sure this is safe we must guarantee that the CSER_POWER_UP
     * time is at least as long as the MSER_MIN_RESET time
     */
#if CSER_POWER_UP < MSER_MIN_RESET
#error "CSER_POWER_UP must be greator than MSER_MIN_RESET"
#endif

    /*
     * Get the possible first reset character ('M' or 'B'), followed
     * by any other characters the hardware happens to send back.
     *
     * Note: Typically, we expect to get just one character, perhaps
     * followed by and indication of the number of buttons.
     */

    Count = 0;
    if( UartGetNextChar( PortBase, &ReceiveBuffer[Count], CSER_POWER_UP) )
    {
	Count++;
	/* We got one - now eat any remaining characters */
	while(Count < (sizeof(ReceiveBuffer) - 1))
	{
	    if( UartGetNextChar( PortBase, &ReceiveBuffer[Count], 100) )
		Count++;
	    else
		break;
	}
    }
    ReceiveBuffer[Count] = '\0';

    /*
     * Analyze the possible mouse answer. Start at the beginning of the
     * "good" data in the receive buffer ignoring any extraneous 
     * characters that may have come in before the 'M' or 'B'.
     */
    for(i = 0; i < Count; i++)
	if( ReceiveBuffer[i] == 'M' || ReceiveBuffer[i] == 'B' )
	    return( 1 );    /* its a mouse! */

    /*
     * Special case: If another device is connected (CSeries, for example)
     * and this device sends a character, the minimum power up time might
     * not be respected. Take care of this case.
     */
    if( Count != 0 )
        msleep(CSER_POWER_UP);

    return( 0 );    /* mouse not found */
}


/*--------------------------------------------------------------------------
 * Check the specified port for a serial mouse.
 * Returns 0 if none found, 1 if a mouse is detected.
 *-------------------------------------------------------------------------*/

BOOLEAN IsMousePresent( int PortBase )
{
    BOOLEAN Result;
    COMMSETTING OrigCommSettings;

    MSerGetCommSetting( PortBase, &OrigCommSettings );
    Result = MSerDetect( PortBase );
    MSerRestoreCommSetting( PortBase, &OrigCommSettings );
    return( Result );
}

