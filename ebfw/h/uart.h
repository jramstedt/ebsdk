#ifndef __UART_H_LOADED
#define __UART_H_LOADED
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
 *  $Id: uart.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 */

/*
 * $Log: uart.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.7  1997/04/30  21:19:32  pbell
 * Added CommPortToNumber and CommPortFromNumber definitions so
 * that the enum conversions are only done in one place.
 *
 * Revision 1.6  1997/04/23  17:41:19  pbell
 * Changed definitions to be independent of specific Comm ports.
 *
 * Revision 1.5  1995/09/05  18:44:35  fdh
 * Removed Port_t definitions
 *
 * Revision 1.4  1995/08/25  19:37:47  fdh
 * Added definition for TGA and changed them all to enum types.
 *
 * Revision 1.3  1994/08/05  20:13:47  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.2  1994/06/20  14:18:59  fdh
 * Fixup header file preprocessor #include conditionals.
 *
 * Revision 1.1  1993/06/08  19:56:16  fdh
 * Initial revision
 *
 */

#include "kbdscan.h"

/* utility functions */

BOOLEAN UartInitLine	( int PortBase, int baud );
BOOLEAN UartCharAv	( int PortBase );
char UartGetChar	( int PortBase );
void UartPutChar	( int PortBase, char c );
void UartFlushReadBuffer( int PortBase );
char UartGetNextChar	( int PortBase, char * C, int WaitLimit );

/* defines for old port specific versions of these functions */

#define uart_init_line( line, baud )	UartInitLine( CommPortBaseL( line ), baud )
#define uart_charav( port )		UartCharAv( CommPortBase( port ) )
#define uart_getchar( port )		UartGetChar( CommPortBase( port ) )
#define uart_putchar( port, c )		UartPutChar( CommPortBase( port ), c )

/* base addresses for comm ports */

extern const int UartPortBaseTable[];

/* CommPortTo/FromNumber rely on the Port_t enum to place COM1 .. COM4
 * in sequence.  If this changes then this must be updated */
#define CommPortFromNumber( number ) ((Port_t)( ((int)COM1) + number ))
#define CommPortToNumber( port )     ( ((int)port) - ((int)COM1) )
#define CommPortBase( port )	     ( UartPortBaseTable[CommPortToNumber( port )] )
#define CommPortBaseL( line )	     ( UartPortBaseTable[line - 1] )

/* service offsets for use with comm port base addresses */

#define com_Rbr	    0	/* Receive Buffer Register Offset */  
#define com_Thr	    0	/* Xmit Holding Register Offset */    
#define com_Ier	    1	/* Interrupt Enable Register Offset */
#define com_Iir	    2	/* Interrupt ID Register Offset */    
#define com_Lcr	    3	/* Line Control Register Offset */    
#define com_Mcr	    4	/* Modem Control Register Offset */   
#define com_Lsr	    5	/* Line Status Register Offset */     
#define com_Msr	    6	/* Modem Status Register Offset */    
#define com_Scr	    7	/* Scratch Register Offset */         
#define com_Dll	    0	/* Divisor Latch (LS) Offset */       
#define com_Dlm	    1	/* Divisor Latch (MS) Offset */       

/* modem control bits */
#define com_DTR	    0x01
#define com_RTS	    0x02

/* line control bits */
#define com_WLS	    0x03
#define com_5BW	    0x00
#define com_6BW	    0x01
#define com_7BW	    0x02
#define com_8BW	    0x03

#define com_STB	    0x04
#define com_1SB	    0x04
#define com_2SB	    0x00

#define com_PEN	    0x08    /* parity enable */
#define com_EPE	    0x10    /* even parity enable (otherwise odd) */


#define com_DLAB    0x80    /* Divisor latch enable bit */

/* LPT port functions and defines */

void putcLpt(int c);

#define	lptDr	0x3bc
#define	lptSr	0x3bd
#define	lptCr	0x3be


#define	lptSTB	0x01
#define	lptAFD	0x02
#define	lptnInit 0x04
#define	lptSlct	0x08
#define	lptIrq	0x10
#define	lptDir	0x20

#define	lptDly	100000

#endif /* __UART_H_LOADED */
