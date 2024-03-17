/*****************************************************************************

Copyright © 1994, Digital Equipment Corporation, Maynard, Massachusetts.

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

#define SERIAL_IO_S_RCSID "$Id: serialio.s,v 1.1.1.1 1998/12/29 21:36:25 paradis Exp $"

/*
 * --------------------------------------------------------------------
 * Code for doing serial I/O thru SROM port on EV5.
 *
 *	Author: Rogelio R. Cruz, Digital Equipment Corporation
 *	Date: 6-Sept-1994
 *      Origins: Toni Camuso (Alcor)
 * $Log: serialio.s,v $
 * Revision 1.1.1.1  1998/12/29 21:36:25  paradis
 * Initial CVS checkin
 *
# Revision 1.19  1997/12/04  21:39:39  pbell
# Merged 21264 changes.
#
# Revision 1.18  1997/05/30  02:11:10  fdh
# Support cpu variations with the cpu.h file.
#
# Revision 1.17  1996/05/15  21:33:21  cruz
# Moved some routines (getChar, putChar, keypressed, auto_sync) to
# a new file called sromio.s so they can be used by other programs
# including the Debug Monitor.
#
# Revision 1.16  1996/05/06  21:19:42  cruz
# Added an additional 4-bit delay after sending a character to prevent
# overrunning the receiving terminal's buffer.
#
# Revision 1.15  1996/04/25  18:41:23  cruz
# Undo change from 1.13.  I had moved the control-c abort from getString
# to getChar, but then realized that this was incorrect since it would
# affect the "xm" command.  Because the data read during the "xm" command
# is binary, it may contain the code for control-c, causing a cancellation
# of the "xm" command before it gets done.  I fixed the problem by moving
# the control-c abort code back to getString.  It also means that the "xm"
# command cannot be aborted.
#
# Revision 1.14  1996/04/23  22:57:02  cruz
# Changed the autobaud algorithm to detect the rate with any 7-bit
# ascii character.
#
# Revision 1.13  1996/04/08  19:25:44  cruz
# Moved the check for control-c from getString to getChar.
# Pressing control-c while getting a character aborts the
# input operation, returning to the main loop in mini_dbg.s
#
# Revision 1.12  1996/04/02  16:50:46  cruz
# A little cleanup of comments.
#
# Revision 1.11  1996/04/02  15:44:37  cruz
# Combined putQuad into putReg to save some instructions.
#
# Revision 1.10  1996/04/02  02:14:06  cruz
# Allow backspace to delete a character.
#
# Revision 1.9  1996/04/02  01:44:42  cruz
# Modified putChar so it makes sure that the transmit line
# is high before it begins to send out a character.  This
# fixes the problem where the first character printed
# after a poweron was lost.
#
# Revision 1.8  1996/04/01  20:53:41  cruz
# Got rid of pt13 usage for storing flags and autobaud info.
#
# Revision 1.7  1996/04/01  16:49:58  cruz
# echo flag is now bigger than 256, therefore, we can't use it
# as an 8-bit literal in instructions.
#
# Revision 1.6  1996/03/27  16:40:26  cruz
# Updated getString to allow 64-bit numbers to be entered.
# Modified getLong to return 64-bit numbers instead.
#
# Revision 1.5  1995/05/19  20:05:08  cruz
# On abort, branch to endcase label instead of printPrompt.
#
# Revision 1.4  1995/05/04  21:21:37  cruz
# Added routine "putQuad" to print a quadword value without
# a CR/LF.
#
# Revision 1.3  1995/04/19  19:54:23  cruz
# Corrected definitions.
#
# Revision 1.2  1995/04/19  19:50:45  cruz
# Added definitions SL_RCV_V_RCV and SL_TXMIT_V_TMT.
#
# Revision 1.1  1995/04/12  23:17:40  cruz
# Initial revision
#
# Revision 1.4  1995/01/24  19:23:43  cruz
# Removed some EV5 definitions already defined in dc21164.h
#
# Revision 1.3  1995/01/21  13:44:02  fdh
# Added SL_RCV_V_RCV and SL_TXMIT_V_TMT definitions.
#
# Revision 1.2  1994/12/19  18:38:09  cruz
# Updated EV5 specific constants to match those in dc21164.h
#
# Revision 1.1  1994/12/13  16:36:54  cruz
# Initial revision
#
 * --------------------------------------------------------------------*/

#include "ascitab.h"
#include "mini_dbg.h"

/* ======================================================================
** = PVC Labels.  							=
** ======================================================================
**	Label					Last X
**	-----					------
**	pvc$serial_putx_X$4003			8
**	pvc$serial_putx_ret$4003.X		1
**	pvc$serial_getstring_X$4004		1
**	pvc$serial_getstring_ret$4004.X		1
**	pvc$serial_cvthex_X$4005		2
**	pvc$serial_cvthex_ret$4005.X		1
**	pvc$serial_getlong_ret$4006.X		1
**	pvc$serial_putreg_ret$4008.X		1
*/

/* ======================================================================
** = putQuad - Prints a 64 bit value as HHHHHHHH.LLLLLLLL.		=
** ======================================================================
** OVERVIEW:
**	Prints a 64-bit value as HHHHHHHH.LLLLLLLL.  No cr+lf is printed.
**
** FORM OF CALL:
**	bsr r23, putQuad 
**  
** RETURNS:
**	none
**
** ARGUMENTS:
**	r18 - value to output
*/
putQuad:
	bis	r23, 1, r23
	br	r31, putRegVal


/* ======================================================================
** = putReg - ouptut the contents of a 64 bit reg as HHHHHHHH.LLLLLLLL	=
** ======================================================================
** FORM OF CALL:
**	bsr	r23, putReg	- Prints reg name and whole quadword
**	bsr	r23, putRegL	- Prints reg name and low longword
**  	bsr	r23, putRegVal	- Prints out the whole quadword only
**  	bsr	r23, putRegValL	- Prints out the lower LW only
**  
** RETURNS:
**	none
**
** ARGUMENTS:
**	r16 - upto 8 ascii characters (register name)
**	r18 - contents of the register to output
*/
putReg:
pvc$serial_putx_1$4003:
	bsr	r21, putString		/* write the register name	*/
	lda	r17, (a_ht)(r31)	/* send a tab			*/
pvc$serial_putchar_11$4001:
	bsr	r22, putChar

putRegVal:
	srl	r18,  32, r17		/* send the high Longword	*/
pvc$serial_putx_2$4003:
	bsr	r21, putLong		/* :				*/
	lda	r17, (a_period)(r31)	/* send a decimal point		*/
pvc$serial_putchar_12$4001:
	bsr	r22, putChar

	br	r31, putRegValL		/* send the low longword 	*/
putRegL:
pvc$serial_putx_3$4003:
	bsr	r21, putString		/* write the register name	*/
	lda	r17, (a_ht)(r31)	/* send a tab			*/
pvc$serial_putchar_13$4001:
	bsr	r22, putChar


putRegValL:
	zapnot	r18, 0x0f, r17		/* send the low longword	*/
pvc$serial_putx_4$4003:
	bsr	r21, putLong		/* :				*/

	blbs	r23, EndPutReg		/* Print a new line feed and CR	*/
	lda	r16, ( (a_nl << 8) | (a_cr << 0)) (r31)
pvc$serial_putx_5$4003:
	bsr	r21, putString

EndPutReg:
	bic	r23, 1, r23
pvc$serial_putreg_ret$4008.1:
	ret	r31, (r23)

/* ======================================================================
** = putLong (putByte) - cvt LW (BYTE) to 8 (2) ASCII bytes out to srom port =
** ======================================================================
** OVERVIEW:
**	Converts the 4-Byte longword (byte) in r17 to an 8-Byte (2-Byte) 
**	ASCII string in r16.  Falls through to putString after the 
**	conversion, so the return call through r21 is made from putString.
**
** FORM OF CALL:
**	R17	<- longword (byte) to send to srom port
**	bsr	r21, putLong
**  
** RETURNS:
**	none
**
** ARGUMENTS:
**	R17 - longword (byte)to convert to ascii and print to srom port
**	r21 - return address
**
** CALLS:
**	************* Falls through to putString **************
**
** REGISTERS:
**	R0  - loop count
**	R1  - scratch
**	R2  - scratch
**	R17 - value to print
**	R16 - build string and pass to putString
**
**	14 Instructions.  Adheres to calling std v0.0
**/
putByte:
	lda	r0,  2(r31)		/* 2 ASCII bytes for 1 hex byte*/
	br	r31, putContinue
putLong:
	lda	r0, 8(r31)		/* 8 ASCII bytes for 4 hex bytes*/
putContinue:
	bis	r31, r31, r16		/* initialize the string	*/
ps10:					/* REPEAT			*/
	sll	r16,  8, r16		/* : get result to next byte pos*/
	and	r17, 0x0f, r1		/* : get current nibble 	*/
	addq	r1,  0x30, r1		/* : convert to ascii		*/
	cmplt	r1,  0x3a, r2		/* : IF the character is A-F	*/
	bne	r2,  ps20		/* : THEN			*/
	addq	r1,  0x27, r1		/* : : add 7 to get the number	*/
ps20:					/* : ENDIF			*/
	bis	r1,  r16, r16		/* : put it into the longword	*/
	srl	r17,  4,  r17		/* : Get next nibble in low byte*/
	subq	r0,   1,  r0		/* : Decrement the loop count	*/
	bne	r0,  ps10		/* UNTIL we get all the chars	*/
					/* Fall through to putString, below*/

/* ======================================================================
** = putString - output upto 8-Bytes ASCII string to the SROM port	=
** ======================================================================
** FORM OF CALL:
**	r16	<- ascii string
**	bsr	r21, putString
**  
** RETURNS:
**	none
**
** ARGUMENTS:
**	r16 - string (up to 8 characters)
**	r21 - return address
**
** CALLS:
**	putChar (RA = R22).  
**
**	5 Instructions.  Adheres to calling std v0.0
*/
putString:				/* REPEAT			*/
	zapnot	r16, 1, r17		/* : just want the low byte	*/
pvc$serial_putchar_4$4001:
	bsr	r22, putChar		/* : output the character	*/	
	srl	r16, 8, r16		/* : next char into low byte 	*/
	bne	r16, putString		/* UNTIL we output all the chars*/
pvc$serial_putx_ret$4003.1:
	ret	r31, (r21)		

	
/* ======================================================================
** = cvtHex - convert an ASCII string to a hexadecimal number		=
** ======================================================================
** OVERVIEW:
**	This routine expects UPPER case characters!  The getString routine
**	returns a-z characters as A-Z.  getString also returns the string 
**	in both r0 AND r16, which is convenient, as this routine expects 
**	the data in r16.
**	
**	Converts the 8 ascii characters in r16 to a longword integer.  
**	Lowercase and uppercase are both converted.  Characters outside 
**	the ranges 0-9, and A-Z will wrap.  For example a 'G' becomes 
**	a 0, 'H' becomes 1, etc.
**  
** FORM OF CALL:
**	bsr	r21, cvtHex
**  
** ARGUMENTS:
**	R0 - Quadword containing data to be converted.
**	R16 - Quadword containing data to be converted.
**
** RETURNS:
**	R0 - longword integer represented by the 8 ascii characters in r16
**
** REGISTERS
**	R1  - number of ascii bytes to convert
**	R2  - conversion scratchpad
**	R3  - branch test
**
**	15 Instructions.  Adheres to calling std v0.0
*/
cvtHex:
	bis	r31, r31, r0	/* Initialize result.			*/
	lda	r1, 8(r31)	/* get the byte count			*/
cvtNext:			/* REPEAT				*/
	sll	r0,  4, r0	/* : get result to next byte position	*/
	subq	r1,  1, r1	/* : get the byte number to extract	*/
	extbl	r16, r1, r2	/* : get a byte from the 8-byte string	*/
	beq	r2, cvtNull	/* : IF byte is NOT null		*/
	subq	r2, 0x30, r2	/* : : Get rid of the ascii		*/
	cmplt	r2, 0x0A, r3	/* : : IF the character is NOT 0-9	*/
	bne	r3, cvtGood	/* : : THEN				*/
	subq	r2, 0x11, r2	/* : : : get the upper case index 	*/
	addq	r2, 0x0A, r2	/* : : : add 10 to get the number	*/
	and	r2, 0x0F, r2	/* : : : only want the 4 LSBits		*/
cvtGood:			/* : : ENDIF				*/
	bis	r2, r0, r0	/* : : put it into the longword		*/
				/* : ENDIF				*/
cvtNull:
	bne	r1, cvtNext	/* UNTIL We have converted all chars	*/
	zapnot	r0, 0x0f, r0	/* r0 <- only want the low longword	*/
pvc$serial_cvthex_ret$4005.1:
	ret	r31, (r21)	 

/* ======================================================================
** = getLong - Get upto 8-Bytes ASCII string and convert to longword	=
** ======================================================================
** FORM OF CALL:
**	bsr	r23, getLong
**
** RETURNS:
**	R0  - longword converted from ASCII quadword
**
** ARGUMENTS:
**	none
**
** CALLS:
**	getString
**	cvtHex
**
** REGISTERS:
**	R0:R3 - Used by called routines.
**	R16 - String returned from user.
**	R21 - return address for calls
**
*/
getLong:
pvc$serial_getstring_1$4004:
	bsr	r21, getString		/* r16 <- 8-byte ASCII string	*/
pvc$serial_cvthex_1$4005:
	bsr	r21, cvtHex		/* r0 <- 4-byte HEX number 	*/
	bis	r0, r0, r17		/* Save LW value.		*/
	bis	r18, r18, r16
pvc$serial_cvthex_2$4005:
	bsr	r21, cvtHex		/* r0 <- 4-byte HEX number 	*/
	sll	r0, 32, r0		/* Shift to upper Lw.		*/
	bis	r0, r17, r0
pvc$serial_getlong_ret$4006.1:
	ret	r31, (r23)

/* ======================================================================
** = getString - Get upto 8-Bytes ASCII string through SROM port   	=
** ======================================================================
** OVERVIEW:
**	Lower case characters will be changed to uppercase before they are
**	inserted in the return string (r0).  If the echo flag has been 
**	cleared by an external load request ("XM"), then no characters are 
**	echoed to the screen, else characters are echoed in the same case 
**	in which they are entered.
**
**	NOTE: The string will have the last entered character in the LSByte.
**
** FORM OF CALL:
**	bsr	r21, getString
**
** RETURNS:
**	R0  - Least significant longword string (up to 8 characters) = R16
**	R1  - Most significant longword string (up to 8 characters) = R18
**
** ARGUMENTS:
**	none
**
** CALLS:
**	getChar
**	putChar - checks for echo flag
**
** REGISTERS:
**	R0  - Character returned from getChar (= R16).
**	R0:R2 - Used by called routines.
**	R3  - Number of characters typed in.
**	R16 - Characters are accumulated in this register (lower lw).
**	R17 - Used for writing characters to the screen (putChar).
**	R18 - Characters are accumulated in this register (upper lw).
**
** RETURNS:
**	R0  - Character returned from getChar (= R16).
**	R3  - Number of characters typed in.
**
*/
getString:
	bis	r31, r31, r16		/* init the accumulator		*/
	bis	r31, r31, r18		/* init the accumulator		*/
	bis	r31, r31, r3		/* zero the character counter.	*/

nxt_chr:				/* REPEAT			*/
pvc$serial_getchar_1$4002:
	bsr	r22, getChar		/* : r0 <- getChar		*/
	bis	r0,  r0, r17		/* : Save char in r17 to echo 	*/
	cmpeq	r0,  a_cr, r1		/* : IF char .NE. carriage_return*/
	bne	r1,  gets_done		/* : THEN			*/
	cmpeq	r0,  a_etx, r1		/* : IF char .EQ. ctrl-c	*/
	bne	r1,  endcase		/* : THEN quit			*/
	cmpeq	r0, a_bs, r1		/* Is it a backspace char?	*/
	bne	r1, del_char		/* If yes, then delete char.	*/
	cmple	r0, a_sp, r1		/* Is it a printable char?	*/
	bne	r1, nxt_chr		/* If yes, then skip it.	*/
	cmplt	r0, 0x80, r1		/* Is it a printable char?	*/
	beq	r1, nxt_chr		/* If yes, then skip it.	*/
	cmpeq	r0,  a_del, r1		/* : IF char .EQ. delete char	*/
	beq	r1,  not_del		/* : :				*/
del_char:
	beq	r3, nxt_chr		/* : .AND. there are some chars.*/
	bis	r31, a_bs, r17		/* : : send a backspace.	*/
pvc$serial_putchar_5$4001:
	bsr	r22, putChar		/* : : : 			*/
	bis	r31, a_sp, r17		/* : : send a space char.	*/
pvc$serial_putchar_6$4001:
	bsr	r22, putChar		/* : : : 			*/
	bis	r31, a_bs, r17		/* : : send another backspace.	*/
pvc$serial_putchar_7$4001:
	bsr	r22, putChar		/* : : :			*/
	srl	r16,  8, r16		/* Delete char from input string */
	extbl	r18, 0, r1		/* Shift R18 into R16.		*/
	srl	r18, 8, r18
	insbl	r1, 7, r1
	bis	r16, r1, r16

	subq	r3, 1, r3		/* Decrement the char. counter.	*/
	br	r31, nxt_chr		/* : : get the next character	*/
					/* : ENDIF			*/
not_del:
	subq	r0,  a_a, r1		/* : : IF char .GE. 'a' 	*/
	blt	r1,  not_low		/* : : THEN			*/
	cmple	r0,  a_z, r1		/* : : : IF char .LE. 'z'	*/
	beq	r1,  not_low		/* : : : THEN			*/
	subq	r0,  0x20, r0		/* : : : : convert to upper case*/
					/* : : : ENDIF			*/
not_low:				/* : : ENDIF			*/
	cmpeq	r3, 16, r1		/* Do we already have 16 chars?	*/
	bne	r1, nxt_chr		/* if yes, then don't add char.	*/
	addq	r3, 1, r3		/* Increment char counter.	*/
	extbl	r16, 7, r1		/* Shift into R18.		*/
	sll	r18, 8, r18
	bis	r18, r1, r18
	sll	r16,  8,  r16		/* : : shift byte up 		*/
	bis	r0,  r16, r16		/* : : write this character in 	*/
pvc$serial_putchar_8$4001:
	bsr	r22, putChar		/* : : echo char just entered	*/
					/* : ENDIF			*/
	br	r31, nxt_chr		/* UNTIL We see a <CR> input	*/

gets_done:
pvc$serial_putchar_9$4001:
	bsr	r22, putChar		/* output the CR		*/
	lda	r17, a_nl(r31)		/* output a LF			*/
pvc$serial_putchar_10$4001:
	bsr	r22, putChar
	bis	r16, r16, r0		/* put return value in r0	*/
	bis	r18, r18, r1		/* put return value in r1	*/
pvc$serial_getstring_ret$4004.1:
	ret	r31, (r21)		/* go home			*/
