#ifndef __LIB_H__
#define __LIB_H__
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

#include "hal.h"
#include "operator.h"

extern MACRODEF** macro_defn;           /* array of macro definitions */
extern int macronum;                    /* number of macro definitions */
extern FILE *save_file;                 /* pointer to save file */                 
extern FILE *rest_file;                 /* pointer to restore file */                 

typedef struct macro_sav_ MACROSAV;
struct macro_sav_ {
    unsigned int namelen;
    unsigned int bodylen;
    int narg;
    int lineno;
    int var_arg;
    unsigned int skiparglen;
    unsigned int fnamelen;
};

#endif
