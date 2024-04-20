/* xlate.h
 *
 * Copyright (C) 1992, 1993, 1994 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	Translation tables definitions.
 *
 *  AUTHORS:
 *
 *	Judith Gold
 *
 *  CREATION DATE:
 *  
 *	02-May-1994	
 *
 *
 *  MODIFICATION HISTORY:
 *
 *	6-sep-1995  al	    Add alt_diffs structure to support ALT keys.
 *
 *	13-Mar-1995 rbb	    Conditionalize for EB164
 *
 *	27-Dec-1994 er	    Conditionalize for EB66
 *
 *	10-Nov-1994 jrk	    Fix for Turbo
 *
 *	15-Aug-1994 er	    Conditionalize for SD164
 *
 *	23-Jun-1994 er	    Conditionalize for EB64+
 *
 *	9-Ma-1994   dtr	    Added in mustang and the rest of the workstations to
 *			    the conditionals
 *
 *--
 */

#ifndef _H_XLATE
#define _H_XLATE

#define MCTRL(x) ((x)&0x1f)

#define ASCII		0
#define ALPHA_ASCII	0
#define SHIFT_ASCII	0
#define NON_ASCII	0
#define SCROLL		1
#define CAPS		2
#define LSHIFT		3
#define LCTRL		4
#define RSHIFT		5
#define RCTRL		6
#define IGNORE		7
#define	ALT		8
#define	FUNCTION	9
#define	FUNCTION_KEY	9
#define	NUMLOCK		10
#define	NUMPAD		11
#define	NUMERIC_PAD	11
#define	UNDEFINED	12
#define	UNKNOWN		12

#define MAX_CODE	0x85
#define MAX_KBD_TYPE	16
#define NUM_KEYS	0x7f    /* number of keys in keyboard */

typedef struct {
	unsigned char	*unshifted;
	unsigned char	*shifted;
	unsigned char	char_type;
	unsigned char   u_length;
	unsigned char	pad0;
	unsigned char	pad1;
} XLATE_T;

typedef struct {
	unsigned char keycode;
	unsigned char unshifted[2];
	unsigned char shifted[2];
} XLATE_DIFF;

struct alt_diffs {		/*Exists to service ALT selections*/
   unsigned char scancode;
   unsigned char value;
};

#endif
