
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
.asciz "$Id: asmstuff.s,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $"
#else
.asciiz "$Id: asmstuff.s,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $"
#endif
.text
#endif

/*
 * $Log: asmstuff.s,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.26  1998/10/08  13:57:48  gries
 * added mb to fix c_chip bugs
 *
 * Revision 1.25  1997/07/09  18:03:37  pbell
 * Eliminated ReadByte and added a conditional to include ReadFlashByte
 * for SX164 builds only.
 *
 * Revision 1.24  1997/06/10  18:21:35  fdh
 * Added imb().
 *
 * Revision 1.23  1997/05/02  20:23:32  fdh
 * Added the _WIN32 condition around directives used with that assembler.
 *
 * Revision 1.22  1997/05/02  18:08:37  pbell
 * Added Flash byte read and wirte functions for the SX.
 *
 * Revision 1.21  1997/04/10  17:05:30  fdh
 * Added the .asciz directive for linux.
 *
 * Revision 1.20  1997/03/26  15:04:19  pbell
 * Added Read/Write functions.
 *
 * Revision 1.19  1997/03/14  18:36:13  fdh
 * Removed include for asm.h.
 *
 * Revision 1.18  1997/02/21  03:28:28  fdh
 * Added functions for accessing unaligned data.
 *
 * Revision 1.17  1996/05/22  21:11:44  fdh
 * Added wrmces and rdmces interfaces to the callpal functions.
 *
 * Revision 1.16  1995/10/12  21:27:57  fdh
 * Added mask operation to wait_cycles function.
 *
 * Revision 1.15  1995/08/25  19:47:20  fdh
 * Added rpcc function.
 *
 * Revision 1.14  1995/02/24  16:06:47  fdh
 * Modified to use the regdefs.h and paldefs.h include files.
 *
 * Revision 1.13  1994/11/22  23:46:38  fdh
 * Don't include PALcode include files for make depend.
 *
 * Revision 1.12  1994/11/04  16:17:48  rusling
 * Added in __main() routine which does nothing as all main()
 * routines compiled by GNU CC have a call to it...
 *
 * Revision 1.11  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.10  1994/07/21  18:03:52  fdh
 * Implemented CleanBCache: to flush any dirty data
 * from the Backup Cache.
 *
 * Revision 1.9  1994/07/13  14:19:39  fdh
 * Changed uswait to wait_cycles which now waits for a
 * specified number of CPU cycles.
 *
 * Revision 1.8  1994/06/20  18:00:11  fdh
 * Modify to use new ANSI compliant PALcode definitions.
 *
 * Revision 1.7  1994/04/06  04:30:52  fdh
 * Added wrfen function.
 * Modified for updated cserve.h.
 *
 * Revision 1.6  1994/03/13  14:38:31  fdh
 * Added GetSP() function which returns a resonably
 * current Stack Pointer value.
 *
 * Revision 1.5  1994/03/11  02:51:51  fdh
 * Included cserve.h
 *
 * Revision 1.4  1994/01/19  10:49:19  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.3  1993/10/02  23:46:40  berent
 * Merge in development by Anthony Berent
 *
 *>> Revision 1.3  1993/08/06  14:42:38  berent
 *>> Added C callable swpipl function
 *
 * Revision 1.2  1993/06/17  19:10:34  fdh
 * Added halt and swppal functions to take advantage
 * of those facilities in PALcode...
 *
 * Revision 1.1  1993/06/08  19:56:19  fdh
 * Initial revision
 *
 */

#ifdef _WIN32
/*
 * Tell assembler that byte instructions are OK
 */
	.arch   ev56
	.tune   ev56
#endif /* _WIN32 */

#include "palosf.h"
#include "regdefs.h"
#include "paldefs.h"
#include "system.h"	


/*
 *  GNU CC demands that __main exists.
 */
	.text
	.align 4
	.globl __main
	.ent __main
__main:
	ret	zero, (ra)
	.end __main

	.text
	.align 4
	.globl	mb
	.ent	mb 2
mb:
	.option	O1
	.frame	sp, 0, ra
	mb
	ret	zero, (ra)
	.end	mb

	.text
	.align 4
	.globl imb
	.ent imb
imb:
	/*
	 * Instruction barrier interface;
	 * simply call the PAL code instruction.
	 */
	.frame sp, 0, ra
	call_pal PAL_IMB_ENTRY
	ret zero, (ra)
	.end imb

	.text
	.align 4
	.globl	rpcc
	.ent	rpcc 2
rpcc:
	.option	O1
	.frame	sp, 0, ra
	rpcc	v0
	ret	zero, (ra)
	.end	rpcc

	.text
	.align 4
	.globl	getsp
	.ent	getsp 2
getsp:
	.option	O1
	.frame	sp, 0, ra
	bis	sp, zero, v0
	ret	zero, (ra)
	.end	getsp

	.text
	.align 4
	.globl	cServe
	.ent	cServe 2
cServe:
	.option	O1
	.frame	sp, 0, ra
	call_pal PAL_CSERVE_ENTRY
	ret	zero, (ra)
	.end	cServe

	.text
	.align 4
	.globl	wrfen
	.ent	wrfen 2
wrfen:
	.option	O1
	.frame	sp, 0, ra
	call_pal PAL_WRFEN_ENTRY
	ret	zero, (ra)
	.end	wrfen

	.text
	.align 4
	.globl	swppal
	.ent	swppal 2
swppal:
	.option	O1
	.frame	sp, 0, ra
	call_pal PAL_SWPPAL_ENTRY
	ret	zero, (ra)
	.end	swppal

	.text
	.align 4
	.globl	halt
	.ent	halt 2
halt:
	.option	O1
	.frame	sp, 0, ra
	call_pal PAL_HALT_ENTRY
	ret	zero, (ra)
	.end	halt

	.text
	.align 4
	.globl	wait_cycles
	.ent	wait_cycles 2
wait_cycles:
	.option O1
	.frame	sp, 0, ra
/*
 *	a0 contains the number of wait cycles.
 */
	rpcc	t0			/*  read cycle counter */
	zap	a0, 0xf0, a0		/* mask off upper longword */
again:	rpcc	t2			/*  read cycle counter again */
	subl	t2, t0, t2		/* check for wrapping */
	bge 	t2, noshft		/* check to see if negative */

	lda	t3, 1(zero)             /* we need to add 2^32 */
	sll	t3, 32, t3		/* shift the add amount */
	addq	t3, t2, t2		/* add 0x100000000 to it for wrapping */

noshft:	cmplt	t2, a0, t2		/* compare these for usec timer */
	bne	t2, again		/* stay in... */
	ret	zero, (ra)
	.end	wait_cycles

	.text
	.align 4
	.globl swpipl
	.ent swpipl
swpipl:
	/* C access to swap IPL pal routine */
	.frame sp,0,ra
	call_pal PAL_SWPIPL_ENTRY
	ret zero,(ra)
	.end swpipl	

	.text
	.align 4
	.globl wrmces
	.ent wrmces
wrmces: 
	/* Write the machine check error status register */
	.frame sp,0,ra
	call_pal PAL_WRMCES_ENTRY
	ret zero,(ra)
	.end wrmces


	.text
	.align 4
	.globl rdmces
	.ent rdmces
rdmces: 
	/* Read the machine check error status register */
	.frame sp,0,ra
	call_pal PAL_RDMCES_ENTRY
	ret zero,(ra)
	.end rdmces

	.text
	.align 4
	.globl	CleanBCache
	.ent	CleanBCache 2
CleanBCache:
	.option O1
	.frame	sp, 0, ra
/*
 *	Make sure that the cache is not dirty by reading
 *	the contents of memory twice the size of the BCache.
 *
 *	a0 must contain the memory size (2xBCache) to sweep.
 */
	mb	/* Flush the Write Buffer first */
Sweep0:
	ldq	t0, 0xFFF&-32(a0)	/* Load  next cache block.	*/
	lda     a0, -32(a0)		/* Decrement pointer		*/
	bgt     a0, Sweep0		/* Loop until done		*/
	ret	zero, (ra)
	.end	CleanBCache

#ifdef _WIN32
/*
 * Where compilers cannot dereference quadword addresses
 * (ie NT), then we must have a assembler routines for this.
 */

#ifdef CPU_SUPPORTS_BYTE_WORD

	.align 4
LEAF(ReadB)
	ldbu	v0, 0(a0)
	ret	zero, (ra)
END(ReadB)

	.align 4
LEAF(ReadW)
	ldwu	v0, 0(a0)
	ret	zero, (ra)
END(ReadW)

	.align 4
LEAF(WriteB)
	stb	a1, 0(a0)
	mb
	mb
	ret	zero, (ra)
END(WriteB)

	.align 4
LEAF(WriteW)
	stw	a1, 0(a0)
	mb
	mb
	ret	zero, (ra)
END(WriteW)

#else
/*
** Byte/Word operations are not supported so operations must be mapped to
** long word
*/
	.align 4
LEAF(ReadB)
	ldq_u	t0, (a0)
	extbl	t0, a0, v0
	ret	ra
END(ReadB)

	.align 4
LEAF(ReadW)
	bic	a0, 2, t1
	and	a0, 2, t0
	ldl	t1, (t1)
	extwl	t1, t0, v0
	ret	ra
END(ReadW)

	.align 4
LEAF(WriteB)
	bic	a0, 3, t1
	and	a0, 3, t4
	ldl	t2, (t1)
	insbl	a1, t4, t3
	mskbl	t2, t4, t2
	bis	t2, t3, t2
	stl	t2, (t1)
	ret	ra
END(WriteB)

	.align 4
LEAF(WriteW)
	bic	a0, 2, t1
	and	a0, 2, t2
	ldl	t3, (t1)
	inswl	a1, t2, t4
	mskwl	t3, t2, t3
	bis	t3, t4, t3
	stl	t3, (t1)
	ret	ra
END(WriteW)

#endif

	.align 4
LEAF(ReadL)
	ldl	v0, 0(a0)
	ret	zero, (ra)
END(ReadL)

	.align 4
LEAF(ReadQ)
	ldq	v0, (a0)
	ret	ra
END(ReadQ)

	.align 4
LEAF(WriteL)
	stl	a1, 0(a0)
	ret	zero, (ra)
END(WriteL)

	.align 4
LEAF(WriteQ)
	stq	a1, 0(a0)
	mb
	mb
	ret	ra
END(WriteQ)

	.align 4
LEAF(ReadFloat)
	lds	$f0, 0(a0)
	ret	ra
END(ReadFloat)

	.align 4
LEAF(ReadDouble)
	ldt	$f0, 0(a0)
	ret	ra
END(ReadDouble)

	.align 4
LEAF(WriteFloat)
	sts	$f17, 0(a0)
	ret	ra
END(WriteFloat)

	.align 4
LEAF(WriteDouble)
	stt	$f17, 0(a0)
	ret	ra
END(WriteDouble)

#endif

#ifndef DISABLE_UNALIGNED_LOAD_STORE
	.align 4
LEAF(UnalignedReadB)
	ldq_u	v0, 0(a0)
	extbl	v0, a0, v0
	ret	zero, (ra)
END(UnalignedReadB)

	.align 4
LEAF(UnalignedReadW)
	ldq_u	v0, 0(a0)
	ldq_u	t0, 1(a0)
	extwl	v0, a0, v0
	extwh	t0, a0, t0
	bis	v0, t0, v0
	ret	zero, (ra)
END(UnalignedReadW)

	.align 4
LEAF(UnalignedReadL)
	ldq_u	v0, 0(a0)
	ldq_u	t0, 3(a0)
	extll	v0, a0, v0
	extlh	t0, a0, t0
	bis	v0, t0, v0
	addl	v0, 0, v0
	ret	zero, (ra)
END(UnalignedReadL)
	
	.align 4
LEAF(UnalignedReadQ)
	ldq_u	v0, 0(a0)
	ldq_u	t0, 7(a0)
	extql	v0, a0, v0
	extqh	t0, a0, t0
	bis	v0, t0, v0
	ret	zero, (ra)
END(UnalignedReadQ)

	.align 4
LEAF(UnalignedWriteB)
	lda	sp, -48(sp)
	stq	a1, 8(sp)
	ldq	t0, 8(sp)
	and	t0, 0xff, t0
	ldq_u	t2, 0(a0)
	insbl	t0, a0, t1
	mskbl	t2, a0, t2
	bis	t2, t1, t2
	stq_u	t2, 0(a0)
	lda	sp, 48(sp)
	ret	zero, (ra)
END(UnalignedWriteB)
	
	.align 4
LEAF(UnalignedWriteW)
	lda	sp, -48(sp)
	stq	a1, 8(sp)
	ldq	t0, 8(sp)
	extwl	t0, 0, t0
	ldq_u	t1, 0(a0)
	ldq_u	t3, 1(a0)
	inswh	t0, a0, t2
	mskwh	t3, a0, t3
	bis	t3, t2, t3
	stq_u	t3, 1(a0)
	inswl	t0, a0, t2
	mskwl	t1, a0, t1
	bis	t1, t2, t1
	stq_u	t1, 0(a0)
	lda	sp, 48(sp)
	ret	zero, (ra)
END(UnalignedWriteW)
	
	.align 4
LEAF(UnalignedWriteL)
	addl	a1, 0, a1
	ldq_u	t0, 0(a0)
	ldq_u	t2, 3(a0)
	inslh	a1, a0, t1
	msklh	t2, a0, t2
	bis	t2, t1, t2
	stq_u	t2, 3(a0)
	insll	a1, a0, t1
	mskll	t0, a0, t0
	bis	t0, t1, t0
	stq_u	t0, 0(a0)
	ret	zero, (ra)
END(UnalignedWriteL)
	
	.align 4
LEAF(UnalignedWriteQ)
	ldq_u	t0, 0(a0)
	ldq_u	t2, 7(a0)
	insqh	a1, a0, t1
	mskqh	t2, a0, t2
	bis	t2, t1, t2
	stq_u	t2, 7(a0)
	insql	a1, a0, t1
	mskql	t0, a0, t0
	bis	t0, t1, t0
	stq_u	t0, 0(a0)
	ret	zero, (ra)
END(UnalignedWriteQ)
	
	.align 4
LEAF(UnalignedReadFloat)
	lda	sp, -16(sp)
	ldq_u	t0, 0(a0)
	ldq_u	t1, 3(a0)
	extll	t0, a0, t0
	extlh	t1, a0, t1
	bis	t0, t1, t0
	addl	t0, 0, t0
	stl	t0, 0(sp)
	lds	$f0, 0(sp)
	lda	sp, 16(sp)
	ret	zero, (ra)
END(UnalignedReadFloat)
	
	.align 4
LEAF(UnalignedReadDouble)
	lda	sp, -16(sp)
	ldq_u	t0, 0(a0)
	ldq_u	t1, 7(a0)
	extql	t0, a0, t0
	extqh	t1, a0, t1
	bis	t0, t1, t0
	stq	t0, 0(sp)
	ldt	$f0, 0(sp)
	lda	sp, 16(sp)
	ret	zero, (ra)
END(UnalignedReadDouble)
	
	.align 4
LEAF(UnalignedWriteFloat)
	lda	sp, -16(sp)
	sts	$f17, 0(sp)
	ldl	t0, 0(sp)
	ldq_u	t1, 0(a0)
	ldq_u	t3, 3(a0)
	inslh	t0, a0, t2
	msklh	t3, a0, t3
	bis	t3, t2, t3
	stq_u	t3, 3(a0)
	insll	t0, a0, t2
	mskll	t1, a0, t1
	bis	t1, t2, t1
	stq_u	t1, 0(a0)
	lda	sp, 16(sp)
	ret	zero, (ra)
END(UnalignedWriteFloat)
	
	.align 4
LEAF(UnalignedWriteDouble)
	lda	sp, -16(sp)
	stt	$f17, 0(sp)
	ldq	t0, 0(sp)
	ldq_u	t1, 0(a0)
	ldq_u	t3, 7(a0)
	insqh	t0, a0, t2
	mskqh	t3, a0, t3
	bis	t3, t2, t3
	stq_u	t3, 7(a0)
	insql	t0, a0, t2
	mskql	t1, a0, t1
	bis	t1, t2, t1
	stq_u	t1, 0(a0)
	lda	sp, 16(sp)
	ret	zero, (ra)
END(UnalignedWriteDouble)
#endif /* DISABLE_UNALIGNED_LOAD_STORE */

#ifdef SX164 /* only used for SX164 flash update */

	.text
	.align 4
	.globl  WriteFlash
	.ent    WriteFlash 2
WriteFlash:
	.option O1
	.frame  sp, 0, ra
	br		t0, WriteFlash1
	.long	0
WriteFlash1:
	lda		t1, 10(zero)
WriteFlashLoop1:
	subq	t1, 1, t1
	cmoveq	t1, a0, t0
	stb		a1, 0(t0)
	mb
	mb
	lda		t2, 200(zero)
WriteFlashLoop2:
	subq	t2, 1, t2
	bgt		t2, WriteFlashLoop2
	bgt		t1, WriteFlashLoop1

	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	bis		zero, zero, zero
	ret		zero, (ra)
	.end    WriteFlash

#endif /* SX164 */



