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
.data
#ifdef __linux__
.asciz "$Id: setjmp.s,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $"
#else
.asciiz "$Id: setjmp.s,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $"
#endif
.text
#endif

/*
 * $Log: setjmp.s,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
# Revision 1.2  1997/04/10  17:06:43  fdh
# Added the .asciz directive for linux.
#
 * Revision 1.1  1997/02/21 03:27:19  fdh
 * Initial revision
 *
 */

#include "regdefs.h"
	
	.text
	.align 4
	.globl setjmp
	.ent setjmp
setjmp:

	.frame sp, 0, ra
	.prologue 0	

	stq	s0, 0*8(a0)
	stq	s1, 1*8(a0)
	stq	s2, 2*8(a0)
	stq	s3, 3*8(a0)
	stq	s4, 4*8(a0)
	stq	s5, 5*8(a0)
	stq	s6, 6*8(a0)
	stq	ra, 7*8(a0)
	stq	gp, 8*8(a0)
	stq	sp, 9*8(a0)

	/* Now do the floating point registers */
	stt	$f2, 10*8(a0)
	stt	$f3, 11*8(a0)
	stt	$f4, 12*8(a0)
	stt	$f5, 13*8(a0)
	stt	$f6, 14*8(a0)
	stt	$f7, 15*8(a0)
	stt	$f8, 16*8(a0)
	stt	$f9, 17*8(a0)

	bis	zero, zero, v0
	ret	zero, (ra)
	.end setjmp	

	
	.text
	.align 4
	.globl longjmp
	.ent longjmp
longjmp:

	.frame sp, 0, ra
	.prologue 0	

	ldq	s0, 0*8(a0)
	ldq	s1, 1*8(a0)
	ldq	s2, 2*8(a0)
	ldq	s3, 3*8(a0)
	ldq	s4, 4*8(a0)
	ldq	s5, 5*8(a0)
	ldq	s6, 6*8(a0)
	ldq	ra, 7*8(a0)
	ldq	gp, 8*8(a0)
	ldq	sp, 9*8(a0)

	/* Now do the floating point registers */
	ldt	$f2, 10*8(a0)
	ldt	$f3, 11*8(a0)
	ldt	$f4, 12*8(a0)
	ldt	$f5, 13*8(a0)
	ldt	$f6, 14*8(a0)
	ldt	$f7, 15*8(a0)
	ldt	$f8, 16*8(a0)
	ldt	$f9, 17*8(a0)

	bis	a1, zero, v0
	cmoveq	a1, 1, v0
	ret	zero, (ra)
	.end longjmp	
