#ifndef __REGDEFS_H_LOADED
#define __REGDEFS_H_LOADED
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
 *  $Id: regdefs.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 */

/*
 * $Log: regdefs.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1997/03/14  19:17:29  fdh
 * Added definitions for the hardware register names.
 * Added LEAF and END macro definitions.
 *
 * Revision 1.2  1995/02/24  16:00:18  fdh
 * Conditional around #define AT.
 *
 * Revision 1.1  1995/02/24  15:54:26  fdh
 * Initial revision
 *
 */

#define LEAF(x)                                         \
        .globl  x;                                      \
        .ent    x,0;                                    \
x:;                                                     \
        .frame  sp,0,ra

#define	END(proc)					\
	.end	proc

#define v0	$0
#define t0	$1
#define t1	$2
#define t2	$3
#define t3	$4
#define t4	$5
#define t5	$6
#define t6	$7
#define t7	$8
#define s0	$9
#define s1	$10
#define s2	$11
#define s3	$12
#define s4	$13
#define s5	$14
#define s6	$15
#define fp	$15	/* fp & s6 are the same	*/
#define a0	$16
#define a1	$17
#define a2	$18
#define a3	$19
#define a4	$20
#define a5	$21
#define t8	$22
#define t9	$23
#define t10	$24
#define t11	$25
#define ra	$26
#define pv	$27	/* pv and t5 are the same */
#define t12	$27
#ifndef AT
#define AT	$at
#endif
#define gp	$29
#define	sp	$30
#define zero	$31

#define r0 $0
#define r1 $1
#define r2 $2
#define r3 $3
#define r4 $4
#define r5 $5
#define r6 $6
#define r7 $7
#define r8 $8
#define r9 $9
#define r10 $10
#define r11 $11
#define r12 $12
#define r13 $13
#define r14 $14
#define r15 $15
#define r16 $16
#define r17 $17
#define r18 $18
#define r19 $19
#define r20 $20
#define r21 $21
#define r22 $22
#define r23 $23
#define r24 $24
#define r25 $25
#define r26 $26
#define r27 $27
#define r28 $28
#define r29 $29
#define r30 $30
#define r31 $31

#endif /* __REGDEFS_H_LOADED */
