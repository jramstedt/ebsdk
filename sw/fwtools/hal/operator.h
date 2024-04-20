#ifndef __OPERATOR_H__
#define __OPERATOR_H__
/*
	Copyright (c) 1994
	Digital Equipment Corporation, Maynard, Massachusetts 01754

	The HAL Alpha Assembler is free software; you can redistribute 
	it and/or modify it with the sole restriction that this copyright 
	notice be attached.

	The information in this software is subject to change without notice 
	and should not be construed as a commitment by Digital Equipment
	Corporation.

	The author disclaims all warranties with regard to this software,
	including all implied warranties of merchantability and fitness, in
	no event shall he be liable for any special, indirect or consequential
	damages or any damages whatsoever resulting from loss of use, data, 
	or profits, whether in an action of contract, negligence or other
	tortious action, arising out of or in connection with the use or
	performance of this software.

*/

#define ID_NULL		(-1)

typedef int OPID;

typedef struct operator_ OPERATOR;

struct operator_ {
    char* name;

    /* Legal types:
     *
     * INS_PAL
     * INS_MEM
     * INS_HWM		HW_LD, HW_ST
     * INS_BRN
     * INS_FOP
     * INS_FP2		Floating convert 
     * INS_IOP
     * INS_JSR
     * INS_MIS		Misc.(No operand)MB, Draint etc...,HW_REI 
     * INS_M1A		Misc.(with Ra)RPCC 
     * INS_M1B		Misc.(with Rb)FETCH, FETCH_M 
     * MACRO_CALL
     */
    int type;

    /* Value is one of two things:
     *
     * 1. for INS_* it is the opcode value
     * 2. for MACRO_CALL it is the OPID
     */
    int value;

    OPID next;
};

/* supporting structures for operator field parsing */
extern OPERATOR* operator;		/* array of defined operators... includes macros */
extern MACRODEF** macro_defn;		/* array of macro definitions */
extern int macronum;

OPID create_macro_defn PROTO((MACRODEF* mtmp));
OPID find_operator PROTO((char* opname, int return_name));
void delete_operator PROTO((OPID oid));
OPID create_operator PROTO((char* name, int type, int value));
void insert_operator PROTO((OPID nop));

#endif
