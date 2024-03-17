
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
.asciz "$Id: ladbxusr.s,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $"
#else
.asciiz "$Id: ladbxusr.s,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $"
#endif
.text
#endif

/* 
 * $Log: ladbxusr.s,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
# Revision 1.8  1997/08/17  11:24:13  fdh
# Included palosf.h instead of osf.h.
#
# Revision 1.7  1997/04/10  17:16:12  fdh
# Added the .asciz directive for linux.
#
 * Revision 1.6  1995/11/16 14:30:53  fdh
 * Fixed up copyright header and RCS string.
 *
# Revision 1.5  1995/08/30  23:10:43  fdh
# Removed USE_MONITOR_DEBUG conditional and cleaned
# up register name usage.
#
# Revision 1.4  1995/08/25  20:09:01  fdh
# Modified ladbx_poll() to call app_poll() directly rather than
# through an "indirect" pointer.  ladbx_poll() is now called
# via a standard Debug Monitor callback.
#
# Revision 1.3  1994/08/05  20:16:23  fdh
# Updated Copyright header and RCS $Id: identifier.
#
# Revision 1.2  1994/06/28  20:11:24  fdh
# Modified filenames and build precedure to fit into a FAT filesystem.
#
# Revision 1.1  1994/03/10  16:44:19  berent
# Initial revision
#
# Revision 1.1  1993/10/01  15:50:19  berent
# Initial revision
#
 */

#include "palosf.h"
#include "paldefs.h"
#include "regdefs.h"
#include "fregs.h"

	.ugen	
	.verstamp	3 11
	.data	
	.align	3
	.align	0
$$3:
	.quad	$$4
	.data	
	.align	3
	.align	0
$$4:

/* ladbx_poll - tells the ladbx server to check its ethernet device for messages of siginificance
 * to remote debug. 
 *
 * C declaration:
 *	extern void ladbx_poll(void);
 */
	.text	
	.align	4
	.globl	ladbx_poll
	.ent	ladbx_poll 2

ladbx_poll:
	ldgp	gp, 0(pv)
	lda	sp, -16(sp)
	stq	ra, 0(sp)
	.mask	0x04000000, -16
	.frame	sp, 16, ra, 0
	.prologue	1
	/*
	 * Call the monitor function. The monitor function takes
	 * 1 argument which is :
         * 1) The address at which the user program should be stopped if a STOP message is
         *    received.
         */

	/* Set the stop point to the return address of this call */
	mov 	ra, a0
	.livereg	0x0001C002,0x00000000
	lda pv,app_poll
	jsr ra,(t12),app_poll
	ldgp	gp, 0(ra)
	.livereg	0x007F0002,0x3FC00000
	ldq	ra, 0(sp)
	lda	sp, 16(sp)
	ret	zero, (ra), 1
	.end	ladbx_poll



