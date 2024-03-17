/*****************************************************************************

Copyright 1996, Digital Equipment Corporation, Maynard, Massachusetts.

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

#define SROMIO_S_RCSID "$Id: sromio.s,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $"

/*
 * --------------------------------------------------------------------
 * Code for writing and reading a character from the SROM port.
 *
 *	Author: Rogelio R. Cruz, Digital Equipment Corporation
 *	Date: 6-Sept-1994
 *
 * $Log: sromio.s,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
# Revision 1.4  1997/12/04  21:39:38  pbell
# Merged 21264 changes.
#
# Revision 1.3  1997/05/02  13:46:36  pbell
# Updated to use the cpu.h include instead of all the
# cpu type switches.
#
# Revision 1.2  1996/05/16  19:07:56  fdh
# Modified to use local definition for ascii X.
#
# Revision 1.1  1996/05/15  21:36:01  cruz
# Initial revision
#
*/

#include "mini_dbg.h"

#define ascii_X         0x58

.global keypressed
.global getChar
.global putChar
.global autobaud_sync

/* ======================================================================
** = PVC Labels.  							=
** ======================================================================
**	Label					Last X
**	-----					------
**	pvc$serial_keypressed_X$4009		2
**	pvc$serial_keypressed_ret$4009.X	1
**	pvc$serial_putchar_X$4001		14
**	pvc$serial_putchar$4001.X		1
**	pvc$serial_getchar_X$4002		1
**	pvc$serial_getchar_ret$4002.X		1
**	pvc$serial_waitbit_X$4000		4
**	pvc$serial_waitbit_ret$4000.X		1
**	pvc$serial_autosync_ret$4010.X		1
*/

	
/* ======================================================================
** = keypressed - detect if user pressed a key				=
** ======================================================================
** OVERVIEW:
**	Returns a 1 if a key has been pressed, 0 if not.
**	check the serial line interrupt bit, if it's set, clear it
**	and return 1, otherwise, return 0
**
** FORM_OF_CALL:
**	bsr r20, keypressed
**  
** RETURNS:
**	R0  - 1 if keypressed, else 0
** 
** SIDE EFFECTS:
**	The Serial Line Interrupt bit is cleared.
**
** ARGUMENTS:
**	none
**
** REGISTERS:
**	R0  - scratch & return value
**	R20 - return address 
**	
** CALLS:
**	none
**
*/
keypressed:
#if defined(DC21264)
	mfpr	r0, EV6__ISUM		/* read Interrupt Summary Register*/
	srl	r0, EV6__ISUM__SL__S, r0 /* r0<0> <- Serial Line Intrpt bit*/
	and	r0, EV6__ISUM__SL__M, r0 /* :				*/
	sll	r0, EV6__HW_INT_CLR__SL__S, r0 /* put into position	*/
	mtpr	r0, EV6__HW_INT_CLR	/* Clear any serial line intrpt*/
	cmovne	r0, 1, r0		/* Return 1 if a key was pressed*/ 
#elif defined(DC21164) || defined(DC21164PC)
	mfpr	r0, isr 		/* read Interrupt Summary Register*/
	srl	r0, ISR_V_SLI, r0	/* r0<0> <- Serial Line Intrpt bit*/
	and	r0, 1, r0		/* :				*/
	sll	r0, HWINT_V_SLC, r0 	/* put into position		*/
	mtpr	r0, hwIntClr		/* Clear any serial line intrpt*/
	cmovne	r0, 1, r0		/* Return 1 if a key was pressed*/ 
#else
	mfpr	r0, hirr 		/* read Interrupt Request Register*/
	srl	r0, HIRR_V_SLR, r0	/* r0<0> <- Serial Line Intrpt bit*/
	and	r0, 1, r0		/* :				*/
	mtpr 	r31, slClr		/* Clear serial line interrupts	*/
#endif
pvc$serial_keypressed_ret$4009.1:
	ret	r31, (r20)


/* ======================================================================
** = getChar -- routine to receive a character from the serial line	=
** ======================================================================
** OVERVIEW:
**	Once the start bit of '0' is detected we wait 1 1/2 bit times 
**	before reading the first data bit.  The data bit is packed into a 
**	register and shifted left.  After the first bit we only wait 
**	whole bit times.  After 8 bits have been read the routine is exited.
**
** FORM OF CALL:
**	bsr	r22, getChar
** 
** RETURNS:
**	R0  - Character returned after user input
**	
** CALLS:
**	waitBitHalf
**	waitBitTime
**       
** REGISTERS:
**	R1  - Scratch.
**	R2  - Bit counter AND character acumulator.
**	      Bits <0:7> is the bit counter (with an extra value of 8)
**	      Bits <8:15> is the character typed in.
**	R20 - return address for the call to waitBitTime()
**
*/
getChar:
	lda	r2, (0+8)(r31)		/* Set the bit count to 0.	*/

	/* Wait for the start bits (sl_rcv bit will go low)		*/
char_start:
#ifdef DC21264
	mfpr	r0, EV6__I_CTL		/* Poll the serial line		*/
        srl     r0, EV6__I_CTL__SL_RCV__S, r0 /* : r0<0> <- current data bit	*/
	blbs	r0, char_start          /* until low -> start bit is '0'*/
#else
	mfpr	r0, slRcv               /* Poll the serial line		*/
	bne	r0, char_start          /* until low -> start bit is '0'*/
#endif

pvc$serial_waitbit_1$4000:
	bsr	r20, waitBitHalf	/* Wait for a half bit time	*/

get_bits:				/* DO				*/
pvc$serial_waitbit_2$4000:
	bsr	r20, waitBitTime	/* : wait for one bit time	*/
#ifdef DC21264
	mfpr	r0, EV6__I_CTL		/* : Get the current data bit.	*/
	srl 	r0, EV6__I_CTL__SL_RCV__S, r0 /* : r0<0> <- current data bit	*/
        and     r0, 1, r0		/* : clear the rest of the bits	*/
#else
	mfpr	r0,  slRcv		/* : Get the current data bit.	*/
	srl 	r0,  SLRCV_V_RCV, r0	/* : r0<0> <- current data bit	*/
#endif
	sll	r0,  r2, r0		/* : r0<r2> <- current data bit */
	bis	r0,  r2, r2		/* : r2 <- r2 .bit_or. r0	*/
	lda	r2, 1(r2)		/* : Increment bit countr.	*/
	and	r2, 0xFF, r0		/* : Ignore character in comp.	*/
	cmpult	r0, (8+8), r1		/* : Do we have a byte now?	*/
	bne	r1, get_bits		/* WHILE bit count .NE. 8	*/

stop_get:
pvc$serial_waitbit_3$4000:
	bsr	r20, waitBitTime	/* Wait for the middle of the 	*/
					/* stop bit.			*/
#ifndef DC21264
        mfpr	r1, slRcv               /* We're in the middle.		*/
#endif
pvc$serial_keypressed_1$4009:
	bsr	r20, keypressed		/* Clear serial line interrupts	*/

	extbl	r2, 1, r0		/* Place character in R0.	*/
	srl	r15, mc_v_echo, r1	/* IF echo_flag set		*/
	blbc	r1, exit_getchar	/* THEN				*/
	xor	r0, ascii_X, r1		/* : IF user sent 'X'		*/
	bne	r1, exit_getchar	/* : THEN			*/
	lda	r1, mc_m_echo(r31)
	bic	r15, r1, r15		/* : : clear the echo flag	*/
					/* : ENDIF			*/
exit_getchar:				/* ENDIF			*/
pvc$serial_getchar_ret$4002.1:
	ret	r31, (r22)		/* return to caller		*/


/* ======================================================================
** = putChar - routine to send a character over the serial line		=
** ======================================================================
** OVERVIEW:
**  	This routine is used to transmit a character over the serial line.  
** 	First a start bit of '0' is sent.  This is followed by eight data 
**	bits.  The data bits are passed to the routine through a gpr.  The 
** 	data is transmitted one bit at a time and the gpr is shifted once 
**	to the right after each bit.  After the last data bit is transmitted
**	a stop bit of '1' is added to the end of the character.  Extra
**      stop bits are added to make sure that we don't overflow the
**      receiving terminal's buffer. 
**	We make sure that the transmit is initially high so that the
**	receiver can detect the start bit.
**
** FORM OF CALL:
**	R17	<- the character to be sent. 
**	bsr	r22, putChar
**	
** RETURNS:
**      none
**	
** ARGUMENTS:
**	R17 - the character which is to be sent to the SROM port.
**
** REGISTERS:
**	r2  - scratch, bit count
**	r17 - character to output through the SROM port
**	r20 - return address for the call to waitBitTime()
**
*/
putChar:
	srl	r15, mc_v_echo, r2	/* IF echo flag is clear THEN	*/
	blbc	r2, exit_putChar	/* Output is disabled, exit	*/

	/* Initialize routine variables	*/
	lda	r2, 16(r31)		/* 1 high + 1 start + 8 data + 	*/
					/* 6 stop (5 extra for delay).  */
	lda	r17, 0x3F00(r17)	/* add stop bits at the end and	*/
	s4addq	r17, 1, r17		/* then add start and high bit	*/

#ifdef DC21264
data_bit:				/* REPEAT			*/
	mfpr	r0, EV6__I_CTL		/* get current I_CTL value	*/
	lda	r1, 0x1(r31)		/* r1 = mask for sl_xmit bit	*/
	sll	r1, EV6__I_CTL__SL_XMIT__S, r1 /* move to correct position */
	bic	r0, r1, r0		/* clear sl_xmit bit in old I_CTL value */
	blbc	r17, send_bit		/* check the new sl_xmit bit	*/
	bis	r0, r1, r0		/* set sl_xmit bit in old I_CTL value */
send_bit:				/* continue			*/
	mtpr	r0, EV6__I_CTL		/* EV6 Send the data bit.	*/
#else
	sll 	r17, (SLXMIT_V_TMT), r17 /* Put bits into position	*/

data_bit:				/* REPEAT			*/
	mtpr	r17, slXmit		/* EV5 Send the first bit.	*/ 
#endif

	srl	r17,  1, r17		/* Shift right, to the next bit.*/
	subq	r2,  1, r2		/* Decrement the bit count.	*/
pvc$serial_waitbit_4$4000:
	bsr	r20, waitBitTime	/* wait for a bit time	   	*/
	bne	r2, data_bit		/* UNTIL all data sent		*/

exit_putChar:
pvc$serial_putchar_ret$4001.1:
	ret	r31, (r22)		/* return to caller		*/


/* ======================================================================
** = waitBitTime - (waitBitHalf) - wait a bit time (wait 0.5 bit-time)	=
** ======================================================================
**
** OVERVIEW:
**	Waits a bit-time or half-bit time, depending on the entry point.
**
** FORM OF CALL:
**	bsr r20, waitBittime
**	bsr r20, waitBithalf
**	
** RETURNS:
**	none
**
** ARGUMENTS:
**	R15	- Upper 32 bits contain CPU cycles per bit.
**
** REGISTERS:
**	R0	- CPU cycle counts per bit
**	R1	- scratch
**
*/
waitBitHalf:
	srl	r15, (32+1), r0		/* Divide timing info by 2.	*/
	br	r31, waitBitCont
waitBitTime:
	srl	r15, 32, r0		/* Get the timing info.		*/
waitBitCont:

wait_init:
	lda	r1, 0x1(r31)		/*  				*/
	sll	r1, 32, r1		/* r1 = 1.0000.0000 		*/
#ifdef DC21264
	mtpr	r1, EV6__CC_CTL		/* clear cycle count 		*/
	addq	r31, r31, r31		/* nop				*/
	addq	r31, r31, r31		/* nop				*/
	addq	r31, r31, r31		/* nop				*/
        mtpr	r31, (EV6__EXC_ADDR+32)	/* dummy IPR write - sets SCBD bit 5 (to stall untill cc_ctl gets written) */
#else
	mtpr	r1, ccCtl		/* clear cycle count 		*/
	STALL				/* wait 3 cyc to avoid palcode 	*/
	STALL				/* violation 			*/
	STALL
	STALL
#endif
wait:					/* REPEAT 			*/
	rpcc	r1			/* : read the cycle count 	*/
	zapnot	r1, 0x0f, r1		/* : clear the high longword 	*/
	cmplt	r0, r1, r1		/* :  				*/
	beq	r1, wait		/* UNTIL we have waited time 	*/
					/* specified 			*/
pvc$serial_waitbit_ret$4000.1:
	ret	r31, (r20)		/* RETURN 			*/



/* ======================================================================
** = autobaud_sync -- routine used for synchronizing with the serial line=
** ======================================================================
** OVERVIEW:
**	The serial line is polled until the a start bit is seen.  We then
**	time from then until the stop bit is detected.  This happens when
**	the line goes high and stays high for longer than 8 times the 
**	previous time.
**
**      1->0  - is the start bit
**      0->1  - is bit 0 of "55"h
**      1->0  - is bit 1 of "55"h
**      0->1  - is bit 2 of "55"h
**      1->0  - is bit 3 of "55"h
**      0->1  - is bit 4 of "55"h
**      1->0  - is bit 5 of "55"h
**      0->1  - is bit 6 of "55"h
**      1->0  - is bit 7 of "55"h
**      0->1  - is the stop bit
**  
** FORM OF CALL:
**	bsr r21, autobaud_sync
**
** REGISTERS:
**	R0:R2 - Scratch.
**
** RETURNS:	
**	R15  - Timing info for using srom port is saved here.
**
*/
autobaud_sync:
	lda	r1,  1(r31)		/* r1 <- cc_ctl enable bit 	*/
	sll	r1,  32, r1
pvc$serial_keypressed_2$4009:
	bsr	r20, keypressed		/* Clear serial line interrupts	*/

        /* Wait for the start bit. Line will transition from high to low*/
poll_start:	                        /* DO 				*/
#ifdef DC21264
	mfpr	r0, EV6__I_CTL		/* : r0 <- sl_rcv 		*/
	srl 	r0, EV6__I_CTL__SL_RCV__S, r0 /* : r0<0> <- current data bit */
    	blbs	r0, poll_start		/* WHILE sl_rcv remains high 	*/
	mtpr	r1, EV6__CC_CTL 	/* Enable/clear the cycle counter. */
#else
	mfpr	r0, slRcv		/* : r0 <- sl_rcv 		*/
    	bne	r0, poll_start		/* WHILE sl_rcv remains high 	*/
	mtpr	r1,  ccCtl		/* Enable/clear the cycle counter. */
#endif

	/* Wait for a low to high transition (or the stop bit) */
poll_h:                   		/* : REPEAT 			*/
#ifdef DC21264
	mfpr	r0, EV6__I_CTL		/* : r0 <- sl_rcv 		*/
	srl 	r0, EV6__I_CTL__SL_RCV__S, r0 /* : r0<0> <- current data bit */
    	blbc	r0, poll_h		/* : UNTIL sl_rcv goes HI 	*/
#else
	mfpr	r0, slRcv		/* : : r0 <- sl_rcv 		*/
        beq	r0, poll_h		/* : UNTIL sl_rcv goes HI 	*/
#endif
	rpcc	r1			/* Record low-to-hi transition. */
	zap	r1, 0xF0, r1		/* blow off the high longword	*/
	
	/* Wait for a high to low transition or time out */
poll_l:					/* : REPEAT 			*/
	rpcc	r0
	zap	r0, 0xF0, r0		/* blow off the high longword	*/
	sll	r1, 3, r2		/* 8 times low-to-high time.	*/
	cmpult  r2, r0, r0		/* Have reached stop bit?	*/
	bne	r0, end_sync
#ifdef DC21264
	mfpr	r0, EV6__I_CTL		/* : r0 <- sl_rcv 		*/
	srl 	r0, EV6__I_CTL__SL_RCV__S, r0 /* : r0<0> <- current data bit */
    	blbs	r0, poll_l		/* : UNTIL sl_rcv goes LO 	*/
#else
	mfpr	r0, slRcv		/* : : r0 <- sl_rcv 		*/
        bne	r0, poll_l		/* : UNTIL sl_rcv goes LO 	*/
#endif
	br	r31, poll_h		/* Wait for next low-to-hi trans*/

end_sync:
	lda	r0, 0(r31)		/* Divide total cycle count (r1)*/
simple_div:
	mulq	r0, 9, r2		/* by 9 (8 data + 1 start bit)	*/
	cmpult	r2, r1, r2		/* This is a simple divide.	*/
	beq	r2, set_baud		/* R0 = R1/9			*/
	addq	r0, 1, r0
	br	r31, simple_div

set_baud:
	sll	r0,  32, r0		/* move to hi longword 		*/
	lda	r15, mc_m_echo(r0)	/* r15 <- cycles-per-bit  	*/
pvc$serial_autosync_ret$4010.1:
	ret	r31, (r21)
