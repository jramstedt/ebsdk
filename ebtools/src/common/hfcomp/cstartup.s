
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
.asciz "$Id: cstartup.s,v 1.1.1.1 1998/12/29 21:36:25 paradis Exp $"
#else
.asciiz "$Id: cstartup.s,v 1.1.1.1 1998/12/29 21:36:25 paradis Exp $"
#endif
.text
#endif

/*
 * $Log: cstartup.s,v $
 * Revision 1.1.1.1  1998/12/29 21:36:25  paradis
 * Initial CVS checkin
 *
 * Revision 1.15  1998/10/08  13:52:23  gries
 * added #ifdef ISP
 *         lda     pv, main
 *         jsr     ra, (pv)
 * #endif
 * for quick isp debugging
 *
 * Revision 1.14  1997/05/28  20:42:53  fdh
 * Corrected problem where return address was not restored
 * when program is terminated by means of exit().
 *
 * Revision 1.13  1997/04/10  17:06:09  fdh
 * Added the .asciz directive for linux.
 *
 * Revision 1.12  1997/03/17 10:16:01  fdh
 * Added call to setjmp to setup a call to exit().
 * The NO_EXITDATA definition bypasses this feature.
 *
 * Revision 1.11  1995/08/31  00:22:28  fdh
 * Pass argument registers a0-a5 to User program.
 *
 * Revision 1.10  1995/08/30  22:54:48  fdh
 * Modified to pass arguments into User programs.
 *
 * Revision 1.9  1995/08/25  19:47:20  fdh
 * Pass process mode (User/DBM) argument into initdata.
 *
 * Revision 1.8  1995/02/24  16:07:34  fdh
 * Modified to use the regdefs.h and paldefs.h include files.
 *
 * Revision 1.7  1995/02/01  15:18:26  fdh
 * Added conditional to disable doinitdata().
 *
 * Revision 1.6  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.5  1994/01/19  10:49:19  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.4  1993/08/26  17:37:05  fdh
 * Cleaned up a bit.  Don't bother with argument passing for now.
 *
 * Revision 1.3  1993/06/18  20:30:03  fdh
 * Created _exit function to make debugger happy...
 *
 * Revision 1.2  1993/06/18  16:50:02  fdh
 * Init unitialized data during C runtime startup...
 *
 * Revision 1.1  1993/06/08  19:56:20  fdh
 * Initial revision
 *
 */



/*
 * Run-time start up for C executables
 */

#include "regdefs.h"
#include "paldefs.h"

	.text

STARTFRM = 64           # return address, arguments and padding to octaword align

        .globl  __start
        .ent    __start, 0
__start:


	lda     sp, -STARTFRM(sp)	# Create a stack frame
	stq     ra,  0(sp)		# Place return address on the stack
	stq	a0,  8(sp)
	stq	a1, 16(sp)
	stq	a2, 24(sp)
	stq	a3, 32(sp)
	stq	a4, 40(sp)
	stq	a5, 48(sp)

	.mask   0x843f0000, -56
	.frame  sp, STARTFRM, ra

	br      t0, 2f			# get the current PC
2:	ldgp    gp, 0(t0)               # init gp

#ifdef ISP
        lda     pv, main
        jsr     ra, (pv)
#endif
#ifndef NO_INITDATA
	lda	a0, 1			# 0=DBM Mode, 1=User Mode
	lda	pv, initdata
	jsr	ra, (pv)
	ldgp	gp, 0(ra)
#endif /* NO_INITDATA */
	
#ifndef NO_EXITDATA
	lda	a0, ExitData
	lda	pv, setjmp
	jsr	ra, (pv)
	bne	v0, _exit
#endif /* NO_EXITDATA */

	/* Restore arguments */
	ldq	a0,  8(sp)
	ldq	a1, 16(sp)
	ldq	a2, 24(sp)
	ldq	a3, 32(sp)
	ldq	a4, 40(sp)
	ldq	a5, 48(sp)

	/* Every good C program has a main() */
	lda	pv, main
	jsr	ra, (pv)
	ldgp	gp, 0(ra)

 	bsr	zero, _exit
.end	__start


        .globl  _exit
        .ent    _exit, 0
_exit:
	ldq     ra, 0(sp)		# restore return address
	lda	sp, STARTFRM(sp)	# prune back the stack

	ret	zero, (ra)		# Back from whence we came
.end	_exit
