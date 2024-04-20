#ifndef _common_
#define _common_

/* file:	common.h
 *
 * Copyright (C) 1990,2000 by
 * Compaq Computer Corporation, Marlboro, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed   as  a commitment by Compaq Computer
 * Corporation.
 *
 * Compaq assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Compaq.
 *
 * Abstract:	C macros that are of general utility for Console Firmware.
 *
 * Author:	AJ Beaverson
 *
 * Modifications:
 *
 *	phk	14-Jun-1990	Move getpcb,mfpr,mtpr,insq & remq
 *				subroutines declarations in 
 *                              kernel.c
 *
 *	phk	13-Jun-1990	Remove the getpcb() macro and
 *                              define getpcb() as a static subroutine
 *                              inlined by the compiler (for VAX target)
 *
 *                              define mtpr, mfpr, insq & remq  
 *				as static subroutines inlined by the 
 *				compiler (for VAX target)
 *
 *
 *	phk	07-Jun-1990	Define getpcb() macro 
 *                              #pragma builtins can  not be used
 *                              whitin a macro. 
 *                              #pragma builtins must be follow by a <cr> 
 *				but there is no way to include a <cr> 
 *				in a macro expansion.
 *                              Therefore #pragma builtins is placed
 *                              outside the macro and the entire source
 *                              file is compiled with builtins enabled.    
 *
 *                              These macros needs some definitions
 *				from STDEF.H & PRDEF.H
 *
 *	jds	04-Apr-1990	Added SEXT macro.
 *
 *	jad	28-Mar-1990	Add bitset and bittst macros.
 *
 *	jad	01-Feb-1990	Added trace macro.
 *
 *	ajb	13-Dec-1990	Remove isatty macro
 *
 *	pel	28-Nov-1990	delete stdin, stdout, stderr definitions
 *
 *	jad	08-Nov-1990	Add integer definitions
 *
 *	ajb	12-Oct-1990	Remove export/import
 *
 *	pel	04-Oct-1990	Add fseek constants, remove ftell.
 *
 *	jad	27-Aug-1990	Add LCG
 *
 *	ajb	02-May-1990	Add isatty, isapipe macros.
 *
 *	ajb	30-Apr-1990	add stdin, stdout, stderr definitions
 *
 *	ajb	23-Apr-1990	Initial entry.
 *
 */

/*
 * Dummy macros for specifying routine parameters that produce
 * the ellipsis notation (...) in the function prototype and
 * for controlling prototype generation.
 */
#define protoarg
#define protoargs
#define protonoarg
#define protonoargs
#define protoif(x)
#define protoendif(x)

#define umin(a,b) (((unsigned int) (a)) < ((unsigned int) (b)) ? ((unsigned int) (a)) : ((unsigned int) (b)))
#define umax(a,b) (((unsigned int) (a)) > ((unsigned int) (b)) ? ((unsigned int) (a)) : ((unsigned int) (b)))
#define min(a,b) (((int) (a)) < ((int) (b)) ? ((int) (a)) : ((int) (b)))
#define max(a,b) (((int) (a)) > ((int) (b)) ? ((int) (a)) : ((int) (b)))
#define abs(x) (((x) < 0) ? -(x) : (x))

#define offsetof(type, identifier) ((unsigned int)(&((type*)NULL)->identifier))

#define	NUL 0
#define NULL	(void *) 0
#define	EOF (-1)
#define	TRUE 1
#define	FALSE 0

#define	SEEK_SET	0	/* from beginning	 */
#define	SEEK_CUR	1	/* from current position */
#define	SEEK_END	2	/* from end of file	 */

#define INTERRUPT_MODE	0
#define POLLED_MODE	1

/* Special GALAXY definitions */

#if !GCT6
#define GCT_HANDLE int
#else
#define GCT_HANDLE __int64
#endif

/*Integer definitions*/

typedef struct {long l; long h;} INT_64;/*64 Bit width*/
typedef long INT_32;			/*32 Bit width*/
typedef short INT_16; 			/*16 Bit width*/
typedef char INT_8; 			/*8 Bit width*/
typedef struct {unsigned long l; unsigned long h;} U_INT_64; 
					/*64 Bit width unsigned*/
typedef unsigned long U_INT_32;		/*32 Bit width unsigned*/
typedef unsigned long U32;		/*32 Bit width unsigned*/
typedef unsigned __int64 u__int64;	/* 64 bit unsigned int64 */
typedef unsigned short U_INT_16;	/*16 Bit width unsigned*/
typedef unsigned char U_INT_8; 		/*8 Bit width unsigned*/

typedef __int8 int8;
typedef unsigned char uint8;
typedef unsigned char u_char;

typedef __int16	int16;
typedef unsigned __int16 uint16;
typedef unsigned short	u_short;

typedef __int32	int32;
typedef unsigned __int32 uint32;
typedef unsigned int	uint;
typedef unsigned int	u_int;
typedef unsigned long	u_long;

typedef __int64	int64;
typedef __int64	INT;
typedef unsigned __int64 uint64;
typedef unsigned __int64 u_int64;
typedef unsigned __int64 UINT;

typedef unsigned int size_t;

#define byte char
#define word short
#define quad __int64
#define ubyte unsigned char
#define uword unsigned short
#define uquad unsigned __int64

#define SEXT(adr) ((adr & 0x80000000) ? 0xffffffff : 0)

/*+
 * ============================================================================
 * = trace - Traces program flow.                                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is used primararly for debug. It can be used to
 *	trace program flow. When the enable flag is not 0 the string
 *	passed is displayed.
 *
 * FORM OF CALL:
 *
 *	trace (str_ptr,enable); 
 *
 * RETURNS:
 *
 *	None   
 *
 * ARGUMENTS:
 *
 *	char *str_ptr - String to be displayed.
 *	int enable - Should display be enabled flag.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

#define trace(a,b) ((b) ? printf(a) : 1)


/*+
 * ============================================================================
 * = bitset - Sets a bit.                                                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This macro sets or clears a bit at a given offset from a
 *	memory location.
 *
 * FORM OF CALL:
 *
 *	 bitset(b,o,v) ; 
 *
 * RETURNS:
 *
 *	None   
 *
 * ARGUMENTS:
 *
 *	unsigned char *b - Base pointer to a memory location.
 *	unsigned int o - Offset form the base memory pointer.
 *	unsigned char v - The value 0 or 1. 0 clears the bit
 *			  and 1 sets it.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

#define bitset(m,o,v) \
	        (*(unsigned char*)((unsigned int)(m)+((o)/8))|=((v)<<((o)%8)))	


/*+
 * ============================================================================
 * = bittst - Tests a bit.                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This macro returns a bit value at a location. 
 *
 * FORM OF CALL:
 *
 *	 bittst(b,o); 
 *
 * RETURNS:
 *
 *	unsigned char v - 0 or 1 Returns the value of the bit at that location. 
 *
 * ARGUMENTS:
 *
 *	unsigned char *b - Base pointer to a memory location.
 *	unsigned int o - Offset form the base memory pointer.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

#define bittst(m,o) \
		(((*(unsigned char*)((unsigned int)(m)+((o)/8)))>>((o)%8))&1)


/*
 * Define a macro that returns true iff  lo <= x < hi
 */
#define	inrange(x,lo,hi) (((unsigned int) (lo) <= (unsigned int) (x))\
		 && ((unsigned int) (x) < (unsigned int) (hi)))


#define INODE_LOCK(x) ilist_lock ()
#define INODE_UNLOCK(x) ilist_unlock ()


#endif

