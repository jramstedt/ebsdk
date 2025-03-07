#ifndef	__OLD_MINI_LOADED
#define	__OLD_MINI_LOADED	1
/*
*****************************************************************************
**                                                                          *
**  Copyright © 1994							    *
**  by Digital Equipment Corporation, Maynard, Massachusetts.		    *
**                                                                          *
**  All Rights Reserved							    *
**                                                                          *
**  Permission  is  hereby  granted  to  use, copy, modify and distribute   *
**  this  software  and  its  documentation,  in  both  source  code  and   *
**  object  code  form,  and without fee, for the purpose of distribution   *
**  of this software  or  modifications  of this software within products   *
**  incorporating  an  integrated   circuit  implementing  Digital's  AXP   *
**  architecture,  regardless  of the  source of such integrated circuit,   *
**  provided that the  above copyright  notice and this permission notice   *
**  appear  in  all copies,  and  that  the  name  of  Digital  Equipment   *
**  Corporation  not  be  used  in advertising or publicity pertaining to   *
**  distribution of the  document  or  software without specific, written   *
**  prior permission.							    *
**                                                                          *
**  Digital  Equipment  Corporation   disclaims  all   warranties  and/or   *
**  guarantees  with  regard  to  this  software,  including  all implied   *
**  warranties of fitness for  a  particular purpose and merchantability,   *
**  and makes  no  representations  regarding  the use of, or the results   *
**  of the use of, the software and documentation in terms of correctness,  *
**  accuracy,  reliability,  currentness  or  otherwise;  and you rely on   *
**  the software, documentation and results solely at your own risk.	    *
**                                                                          *
**  AXP is a trademark of Digital Equipment Corporation.		    *
**                                                                          *
*****************************************************************************
**
*/

/*
 * $Id: oldminid.h,v 1.1.1.1 1998/12/29 21:36:25 paradis Exp $
 * --------------------------------------------------------------------
 * Defines macros used in mini_dbg.s
 *
 *	Author: Rogelio R. Cruz, Digital Equipment Corporation
 *	Date: 6-Sept-1994
 *      Origins: Ken Curewitz, Tony Camuso
 *
 * $Log: oldminid.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:25  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/12/15  18:53:47  pbell
 * New
 *
 * Revision 1.9  1996/05/14  21:11:07  fdh
 * Modified to resolve conflict with predefined LDLI macro.
 *
 * Revision 1.8  1996/04/19  18:53:21  cruz
 * Replaced interface call MC_IO_AUTO_BAUD with
 * MC_IO_MINI_PROMPT which jumps to the SROM
 * prompt without doing an auto-baud call.
 *
 * Revision 1.7  1996/04/05  21:13:56  cruz
 * Updated with the recent changes to the mini_dbg.s file.
 * Flags were changed around.
 *
 * Revision 1.6  1996/03/01  23:57:46  cruz
 *  Added PRINT_BYTE to the serial I/O interface.
 *
 * Revision 1.5  1996/01/23  21:43:43  cruz
 * Added new flag definitions for writeaddr and notdata.
 *
 * Revision 1.4  1995/05/19  20:04:15  cruz
 * Added two more flags: one for the cp command and another for
 * requesting input of a 3rd address.
 *
 * Revision 1.3  1995/05/04  21:20:17  cruz
 * Added define for compare command
 *
 * Revision 1.2  1995/04/18  23:03:11  cruz
 * Changed definition of LDLQ
 *
 * Revision 1.1  1995/04/12  23:19:51  cruz
 * Initial revision
 *
 *
 * --------------------------------------------------------------------*/

#ifndef SEXT
#define SEXT(val) ((val ^ 0x8000) - 0x8000)
#endif

/*
 * the following bits definitions are flags for the mini-debugger
 */

#define mc_v_qw		0		/* qw output mode flag 			*/
#define mc_v_display	1		/* Display on/off flag.			*/
#define mc_v_loop     	2		/* infinite loop flag 			*/
#define mc_v_writeaddr 	3		/* A fill with address command.		*/
#define mc_v_notdata 	4		/* Negate data written.                 */
#define mc_v_base     	5		/* base address flag 			*/
#define mc_v_follow_w_rd 6		/* Follow command with a read.		*/
#define mc_v_follow_w_wr 7		/* Follow command with a write.		*/


#define mc_v_echo     	8		/* echo output flag 			*/
#define mc_v_write    	9		/* write/not_read flag 			*/
#define mc_v_xm 	10		/* An XM command has been executed.	*/
#define mc_v_block    	11		/* block read/write flag 		*/
#define mc_v_cmp 	12		/* A CoMpare command.			*/
#define mc_v_cp 	13		/* A Copy command.			*/
#define mc_v_mt 	14		/* A memory test command.		*/



#define mc_m_qw	      (1 << mc_v_qw)
#define mc_m_display  (1 << mc_v_display)
#define mc_m_loop     (1 << mc_v_loop)
#define mc_m_writeaddr 	(1 << mc_v_writeaddr)
#define mc_m_notdata 	(1 << mc_v_notdata)  
#define mc_m_base     (1 << mc_v_base)	
#define mc_m_follow_w_rd (1 << mc_v_follow_w_rd)
#define mc_m_follow_w_wr (1 << mc_v_follow_w_wr)

#define mc_m_echo     (1 << mc_v_echo)
#define mc_m_write    (1 << mc_v_write)
#define mc_m_xm       (1 << mc_v_xm)
#define mc_m_block    (1 << mc_v_block)
#define mc_m_cmp 	(1 << mc_v_cmp)
#define mc_m_cp 	(1 << mc_v_cp)
#define mc_m_mt 	(1 << mc_v_mt)





#define MC_IO_MINIDEBUGGER 	0
#define MC_IO_PRINT_STRING	1
#define MC_IO_PRINT_LONG	2
#define MC_IO_PRINT_CHAR	3
#define MC_IO_READ_CHAR		4
#define MC_IO_SET_BAUD		5
#define MC_IO_MINI_PROMPT	6
#define MC_IO_PRINT_BYTE	7

#ifndef LDLI
#define LDLI(reg,val) \
lda reg, SEXT(val&0xffff)(r31)		; /* Mask off upper word	*/ \
ldah reg, SEXT((val+0x8000)>>16)(reg)	; /* Mask off lower word and	*/
					  /* remove its sign-extension	*/
#endif

#define LDLQ(reg, val_h, val_l) \
LDLI(reg, val_h); \
sll  reg, 32, reg; \
lda  reg, SEXT(val_l&0xffff)(reg); \
ldah reg, SEXT((val_l+0x8000)>>16)(reg);


/*--------------------------------------------------------------------------
 *
 *	"b4_to_l" macro takes 4 characters (bytes) and puts them into
 *	a longword.  The first argument is shifted into the high byte
 *	the last into the lowest byte.
 *--------------------------------------------------------------------------*/
#define b4_to_l(c1, c2, c3, c4)			\
	( (c4 << 24) | (c3 << 16) | (c2 << 8) | (c1 << 0) )


#define GET_LONG()	       			\
	bsr	r23, getLong;

#define PRINT_REG() \
	bsr	r23, putReg;

	/* Prints upto 4 chars and the value of an IPR */
#define LOAD_IPR4(c1, c2, c3, c4, ipr)	\
	LDLI(r16, (b4_to_l(c1,c2,c3,c4)));	\
	mfpr	r18, ipr;


	/* Prints upto 8 chars and the value of an IPR */
#define LOAD_IPR8(c1, c2, c3, c4, c5, c6, c7, c8, ipr)	\
	LDLQ(r16, b4_to_l(c5,c6,c7,c8), b4_to_l(c1,c2,c3,c4));	\
	mfpr	r18, ipr;

	/* Prints upto 4 chars and the value of an GPR */
#define LOAD_REG4(c1, c2, c3, c4, reg)		\
	LDLI(r16, (b4_to_l(c1,c2,c3,c4)));	\
	bis  reg, reg, r18;


	/* Prints upto 8 chars and the value of an GPR */
#define LOAD_REG8(c1, c2, c3, c4, c5, c6, c7, c8, reg)	\
	LDLQ(r16, b4_to_l(c5,c6,c7,c8), b4_to_l(c1,c2,c3,c4)); \
	bis  reg, reg, r18;	

	/* Prints upto 8 chars to the serial rom port.	*/
#define PRINT_STR()  \
	bsr	r21, putString ;

	/* Prints upto 1 chars to the serial rom port.	*/
#define PRINT_CHAR()				\
	bsr	r22, putChar;

	/* Loads 1 char for printinting to the serial rom port.	*/
#define LOAD_CHAR(c1)				\
	lda	r17, c1(r31);

#if 0
	/* Prints upto 2 chars to the serial rom port.	*/
#define PRINT_STR2(c1, c2)			\
	lda r16, ((c1 << 0) | (c2 << 8))(r31);	\
	bsr	r21, putString ;

	/* Prints upto 4 chars to the serial rom port.	*/
#define PRINT_STR4(c1, c2, c3, c4)		\
	LDLI(r16, (b4_to_l(c1,c2,c3,c4)));	\
	bsr	r21, putString ;

	/* Prints upto 8 chars to the serial rom port.	*/
#define PRINT_STR8(c1, c2, c3, c4, c5, c6, c7, c8)	\
	LDLQ(r16, b4_to_l(c5,c6,c7,c8), b4_to_l(c1,c2,c3,c4)); \
	bsr	r21, putString ;
#endif

	/* Loads 1 character into the argument reg for putString */
#define LOAD_STR1(c1)				\
	lda	r16, c1(r31);

	/* Loads upto 2 characters into the argument reg for putString */
#define LOAD_STR2(c1, c2)			\
	lda r16, ((c1 << 0) | (c2 << 8))(r31);

	/* Loads upto 4 characters into the argument reg for putString */
#define LOAD_STR4(c1, c2, c3, c4)		\
	LDLI(r16, (b4_to_l(c1,c2,c3,c4)));

	/* Loads upto 8 characters into the argument reg for putString */
#define LOAD_STR8(c1, c2, c3, c4, c5, c6, c7, c8)	\
	LDLQ(r16, b4_to_l(c5,c6,c7,c8), b4_to_l(c1,c2,c3,c4)); \


#define CASE_ON(c1, c2, label)		\
	lda r2, (c1<<8 | c2)(r31);	\
	xor r0, r2, r2;			\
	bne r2, label

#define CASE_ON_FLAG(c1, c2, flag)	\
	lda r2, (c1<<8 | c2)(r31);	\
	xor r0, r2, r2;			\
	cmoveq r2, flag, r4

#if 0
#define loads(reg, lw_lo, lw_hi)		\
	LDLQ (reg, ((lw_hi << 32) | (lw_lo)));	

#define loads8(lw_lo, lw_hi)	\
	LDLQ (r16, ((lw_hi << 32) | (lw_lo)));	

#define loads4(lw_lo)	\
	LDLI (r16, lw_lo);	


/*--------------------------------------------------------------------------
 *
 * putc macro - put a character
 *
 * FORM OF MACRO:
 *
 *	putc( c )		    * sends "c" to srom port
 *
 * REGISTERS DESTROYED:
 *
 *	r17: value to print
 *	r22: return address
 *
 *-------------------------------------------------------------------------*/
#define putc(c)			\
	lda	r17, c(r31);			\
	bsr	r22, putChar;

/*--------------------------------------------------------------------------
 *
 * putl macro - put a long register
 *
 * FORM OF MACRO:
 *
 *	putr( r*? )		    * sends val of r?* to srom port
 *
 * REGISTERS DESTROYED:
 *
 *	r17: value to print
 *	r29: return address
 *
 *-------------------------------------------------------------------------*/
#define putl(reg)				\
	bis	reg, reg, r17;			\
	bsr	r21, putLong;


/*
 *============================================================================
 * getc macro - get a character from the user through the srom port
 *============================================================================
 *
 * OVERVIEW:
 *
 *	Acquire a character from the user through the srom port using the jump
 *	table.
 *
 * FORM OF MACRO:
 *
 *	getc				* r0 <- character from srom port
 *
 * REGISTERS DESTROYED:
 *
 *	r0			character from srom port in byte 0
 *	r22			bsr return address
 *
 */
#define	getc			\
	bsr	r22, getChar

/*============================================================================
 * gets macro - get a string from the user through the srom port
 *============================================================================
 */
#define GETS()	\
	bsr	r21, getString

#define gets	\
	bsr	r21, getString
#endif

#define SAVE_GPR_STATE() 	\
	mtpr	r0, pt0;	\
	mtpr	r1, pt1;	\
	mtpr	r2, pt2;	\
	mtpr	r3, pt3;	\
	mtpr	r4, pt4;	\
	mtpr	r5, pt5;	\
	mtpr	r6, pt6;	\
	mtpr	r7, pt7;	\
	mtpr	r8, pt8;	\
	mtpr	r9, pt9;	\
	mtpr	r10, pt10;	\
	mtpr	r11, pt11;	\
	mtpr	r12, pt12;	\
	mtpr	r14, pt14;	\
	mtpr	r15, pt15;	\
	mtpr	r16, pt16;	\
	mtpr	r17, pt17;	\
	mtpr	r18, pt18;	\
	mtpr	r19, pt19;	\
	mtpr	r20, pt20;	\
	mtpr	r21, pt21;	\
        mtpr	r22, pt22;	\
	mtpr	r23, pt23;


#define RESTORE_GPR_STATE() 	\
	mfpr	r1, pt1;	\
	mfpr	r2, pt2;	\
	mfpr	r3, pt3;	\
	mfpr	r4, pt4;	\
	mfpr	r5, pt5;	\
	mfpr	r6, pt6;	\
	mfpr	r7, pt7;	\
	mfpr	r8, pt8;	\
	mfpr	r9, pt9;	\
	mfpr	r10, pt10;	\
	mfpr	r11, pt11;	\
	mfpr	r12, pt12;	\
	mfpr	r14, pt14;	\
	mfpr	r15, pt15;	\
	mfpr	r16, pt16;	\
	mfpr	r17, pt17;	\
	mfpr	r18, pt18;	\
	mfpr	r19, pt19;	\
	mfpr	r20, pt20;	\
	mfpr	r21, pt21;	\
	mfpr	r22, pt22;	\
	mfpr	r23, pt23;	\
	mfpr	r0, pt0;       /* We do this one last to avoid possible PALcode violations */

#endif				/* __OLD_MINI_LOADED */
