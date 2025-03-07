
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
__attribute__((unused)) static const char *rcsid = "$Id: xmodem.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * $Log: xmodem.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1997/08/12  16:26:22  fdh
 * Modified the wait delay for the srom serial port.
 * Added a delay before displaying trasnfer statistics.
 *
 * Revision 1.2  1997/07/25  18:32:24  pbell
 * Updated to allow operation over the SROM serial port.
 *
 * Revision 1.1  1997/04/29  21:51:57  pbell
 * Initial revision
 *
 */

#include "lib.h"
#include "mon.h"

/*--------------------------------------------------------------------------
 *
 * Xmodem/Ymodem protocal support
 *
 *-------------------------------------------------------------------------*/

#define CTRL_SOH    0x01
#define CTRL_STX    0x02
#define CTRL_EOT    0x04
#define CTRL_ACK    0x06
#define CTRL_NAK    0x15
#define CTRL_CAN    0x18

/* all wait values are in miliseconds. */

#define READWAIT_INIT		10000
#define READWAIT_ACK		 1000
#define READWAIT_BLOCK		10000
#define READWAIT_BLOCKCHARS	 1000

#define MAX_LEADING_GARBAGE	30
#define MAX_BLOCK_RETRIES	 5
#define MAX_ACH_CHARS		10
#define NUM_CAN_CHARS_TO_CAN	 8
#define MAX_CONSEC_ERRORS	10
#define NUM_TRIES_ON_START	10

#define RTN_TIME_OUT		-1
#define RTN_EOT			-2
#define RTN_DATA_ERROR		-3
#define RTN_CANCELED		-4
#define RTN_DUP_BLOCK		-5

#define SMALL_BLOCK_SIZE  128
#define LARGE_BLOCK_SIZE 1024

typedef struct CONTROLDATA
{
    BOOLEAN Started;
    BOOLEAN FullCRC;
    int	    NextBlockNumber;
    int	    BlockSize;
    int (* CRCFun)( int, int );
} CONTROLDATA;

/* functions mapped to external functions */

/* void TransmitCharacter( char Value ); */
#define TransmitCharacter	    PutChar

/* internal functions */

static void	CancelTransfer	    ( CONTROLDATA * CD );
static void	CONTROLDATA_Init    ( CONTROLDATA * CD );
static void	FlushReceive	    ( void );
static int	GetFirstReceiveChar ( CONTROLDATA * CD );
static int	ReceiveBlock	    ( CONTROLDATA * CD, ul Data );
static int	ReceiveCharacter    ( int MaxWait );


/*--------------------------------------------------------------------------
 * Add one character to an 8 bit checksum
 *-------------------------------------------------------------------------*/

static int UpdateChecksum( int Check, int C )
{
    return( (Check + C) & 0x0FF );
}

/*--------------------------------------------------------------------------
 * Add one character to a 16 bit CRC
 *-------------------------------------------------------------------------*/

static int UpdateCRC( int CRC, int C )
{
    int i;
    unsigned short Value = CRC ^ C<<8;
    for( i=8; --i>=0; )
    {
	if( Value & 0x8000 )
	    Value = (Value << 1) ^ 0x1021;
	else
	    Value <<= 1;
    }
    return( Value & 0x0FFFF );
}


/*--------------------------------------------------------------------------
 * Cancel the transfer
 *-------------------------------------------------------------------------*/

static void CancelTransfer( CONTROLDATA * CD )
{
    int i;
    FlushReceive();
    for( i=NUM_CAN_CHARS_TO_CAN; i>0; i-- )
	TransmitCharacter( CTRL_CAN );
    FlushReceive();
}


/*--------------------------------------------------------------------------
 * Fill the CONTROLDATA structure with defaults
 *-------------------------------------------------------------------------*/

static void CONTROLDATA_Init( CONTROLDATA * CD )
{
    CD->Started = FALSE;
    CD->FullCRC = FALSE;
    CD->NextBlockNumber = 1;
    CD->BlockSize = SMALL_BLOCK_SIZE;
    CD->CRCFun = UpdateChecksum;
}


/*--------------------------------------------------------------------------
 * Flush all characters from the receiver
 *-------------------------------------------------------------------------*/

static void FlushReceive( void )
{
    /* wait for a timeout reading characters to end the flush */
    while( ReceiveCharacter( READWAIT_BLOCKCHARS ) != -1 )
	;
}


/*--------------------------------------------------------------------------
 * Receive File support routines
 *-------------------------------------------------------------------------*/

static int GetFirstReceiveChar( CONTROLDATA * CD )
{
    int i,C;
    for( i=NUM_TRIES_ON_START*2; i>0; i-- )
    {
	TransmitCharacter( i>NUM_TRIES_ON_START ? 'C' : CTRL_NAK );
	C = ReceiveCharacter( READWAIT_INIT );
	if( C == CTRL_SOH || C == CTRL_STX )
	{
	    if( i > NUM_TRIES_ON_START )
	    {
	        CD->CRCFun = UpdateCRC;
		CD->FullCRC = TRUE;
	    }
	    return( C );
	}
	if( C == CTRL_CAN )
	{
	    C = ReceiveCharacter( READWAIT_BLOCKCHARS );
	    if( C == CTRL_CAN )
		return( -1 );
	}
	FlushReceive();
    }
    return( -1 );
}


/*--------------------------------------------------------------------------
 * Wait for init character to be sent
 * Returns BlockSize received or an error code
 * RTN_TIME_OUT, RTN_EOT, RTN_DATA_ERROR, RTN_CANCELED
 *-------------------------------------------------------------------------*/

static int ReceiveBlock( CONTROLDATA * CD, ul Data )
{
    int CharsRemaining;
    int C;
    int CRC;
    int CRCSent;
    int SentBlockNumber;
    int Result;
    ul  OrigData = Data;

    if( !CD->Started )
    {
	C = GetFirstReceiveChar( CD );
	if( C == -1 )
	    return( RTN_CANCELED );
	CD->Started = TRUE;
    }
    else
	C = ReceiveCharacter( READWAIT_BLOCK );

    /* check the initial characters and exit on errors */

    do
    {
	switch( C )
	{
	case CTRL_STX:	Result = CD->BlockSize = 1024;    break;

	case CTRL_SOH:	Result = CD->BlockSize =  128;    break;

	case CTRL_EOT:	TransmitCharacter( CTRL_NAK );
			C = ReceiveCharacter( READWAIT_BLOCK );
			if( C != CTRL_EOT )
			    continue;
		        TransmitCharacter( CTRL_ACK );
			return( RTN_EOT );

	case CTRL_CAN:	C = ReceiveCharacter( READWAIT_BLOCK );
			if( C == CTRL_CAN )
			    return( RTN_CANCELED );
			continue;

	case -1:	return( RTN_TIME_OUT );

	default:	return( RTN_DATA_ERROR );
	}
	break;
    } while( TRUE );

    /* read in the block */

    SentBlockNumber = ReceiveCharacter( READWAIT_BLOCKCHARS );
    C = ReceiveCharacter( READWAIT_BLOCKCHARS );
    if( SentBlockNumber != (255-C) || SentBlockNumber == -1 ) 
	return( RTN_DATA_ERROR );
    if( SentBlockNumber == ((CD->NextBlockNumber - 1) & 0x0FF) )
	Result = RTN_DUP_BLOCK;
    else if( SentBlockNumber != CD->NextBlockNumber )
	return( RTN_DATA_ERROR );

    for( CharsRemaining = CD->BlockSize; CharsRemaining > 0; CharsRemaining--,Data++ )
    {
        C = ReceiveCharacter( READWAIT_BLOCKCHARS );
        if( C < 0 )
	    return( RTN_TIME_OUT );
	WriteB( Data, C );
    }

    if( CD->FullCRC )
    {
        C = ReceiveCharacter( READWAIT_BLOCKCHARS );
        if( C < 0 )
	    return( RTN_TIME_OUT );
	CRCSent = (C<<8);
    }
    else
	CRCSent = 0;

    C = ReceiveCharacter( READWAIT_BLOCKCHARS );
    if( C < 0 )
	return( RTN_TIME_OUT );

    CRCSent += C;

    for( CRC = 0, CharsRemaining = CD->BlockSize, Data = OrigData;
	 CharsRemaining > 0;
	 CharsRemaining--,Data++ )
    {
        C = ReadB( Data );
	CRC = CD->CRCFun( CRC, C );
    }

    if( CRC != CRCSent )
	return( RTN_DATA_ERROR );

    if( Result != RTN_DUP_BLOCK )
	CD->NextBlockNumber = (CD->NextBlockNumber + 1) & 0x0FF;  /* update NBN */

    TransmitCharacter( CTRL_ACK );
    return( Result );
}


/*--------------------------------------------------------------------------
 * Read on character from the input device.
 * This version does not convert any of the characters read into other
 * characters or impose an endless wait for characters.
 *
 * The SROM port returns true for CharAv and a bit outside char for RawGetChar
 * if there is no character.  since there is a small wait loop in the SROM
 * version of RawGetChar the usleep call is not used when RawGetChar returns
 * no character.
 *-------------------------------------------------------------------------*/

static int ReceiveCharacter( int MaxWait )
{
    int Waited = 0;
    int FullWaitLimit = 1000 * MaxWait;
    do
    {
	ui c = 0;
	if( CharAv() )
	{
	    c = RawGetChar();
	    if( c <= 0x0FF )
		return( c );
	    /* add the wait used by the srom port */
	    Waited += 86*20;
	}

	if( Waited >= FullWaitLimit )
	    return( -1 );

	if( c == 0 )
	{
	    usleep(50);	/* at 115.2K baud chars are ~86us apart */
	    Waited += 50;
	}
    }
    while( 1 );
}


/*--------------------------------------------------------------------------
 * Receive a data stream and store it in memory at the address given.
 *-------------------------------------------------------------------------*/

BOOLEAN XReceive( ul Address )
{
    char * ResultText;
    BOOLEAN Result;
    CONTROLDATA CD;
    int ConsecErrors;
    int TotalConsecErrors;
    int TotalErrors;
    ul CurrentAddress = Address;
    int Rtn;

    printf(" Send File now...\n");

    CONTROLDATA_Init( &CD );

    TotalErrors = 0;
    TotalConsecErrors = 0;
    ConsecErrors = 0;
    ResultText = NULL;
    do
    {
	Rtn = ReceiveBlock( &CD, CurrentAddress );
	switch( Rtn )
	{
	case LARGE_BLOCK_SIZE:
	case SMALL_BLOCK_SIZE:
	    CurrentAddress += Rtn;
	    if( ConsecErrors > 0 )
	    {
		TotalErrors += ConsecErrors;
		if( ConsecErrors > 1 )
		    TotalConsecErrors++;
		ConsecErrors = 0;
	    }

	    break;

	case RTN_EOT:
	    ResultText = "Succeeded";
	    Result = TRUE;
	    break;

	case RTN_TIME_OUT:
	case RTN_DATA_ERROR:
	    if( ConsecErrors < MAX_CONSEC_ERRORS )
	    {
		FlushReceive();
		TransmitCharacter( CTRL_NAK );
		ConsecErrors++;
		break;
	    }
	    /* fallthrough */
	case RTN_CANCELED:
	    CD.BlockSize = 0;
	    CancelTransfer( &CD );
	    ResultText = "Canceled";
	    Result = FALSE;
	    break;
	}
    } while( ResultText == NULL );

    sleep(4);

    printf( "\nLoad %s after 0x%X (%d) bytes\n"
	    "Errors - Corrected %d, Consecutive %d, Uncorrected %d\n",
	    ResultText, (int)(CurrentAddress - Address),
	    (int)(CurrentAddress - Address), TotalErrors,
	    TotalConsecErrors, ConsecErrors );
    return( Result );
}



