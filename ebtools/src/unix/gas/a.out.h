/*
 * HISTORY
 * $Log: a.out.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:27  paradis
 * Initial CVS checkin
 *
 * Revision 4.1  1995/02/16  15:26:25  samberg
 *  DC21164 hw_ld and hw_st changed from 12-bit to 10-bit displacement
 *
 * Revision 4.0  1993/10/18  16:33:02  rusling
 * initial version
 *
 */
#ifndef _A_OUT_H_
#define _A_OUT_H_ 1
/* This file describes the a.out file format
   Copyright (C) 1987 Free Software Foundation, Inc.

This file is part of GAS, the GNU Assembler.

GAS is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GAS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GAS; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


	/* Alpha's a.out. . . */

#ifdef CROSS_COMPILE_32_TO_64_BITS
#include "cross_32_to_64.h"
typedef int32 boolean_t;
#endif

#if __osf__
#include <mach/std_types.h>
typedef long integer_t;
#endif

#include "../../common/include/stdc.h"

struct exec {
	integer_t a_magic;	/* Use macros N_MAGIC, etc for access */
	vm_size_t a_text;	/* bytes of text in file */
	vm_size_t a_data;	/* bytes of data in file */
	vm_size_t a_bss;	/* bytes of auto-zeroed data */
	vm_size_t a_syms;	/* bytes of symbol table data in file */
	vm_offset_t a_entry;	/* start PC */
	vm_offset_t a_tstart;	/* text start, in memory */
	vm_offset_t a_dstart;	/* data start, in memory */
	vm_size_t a_trsize;	/* bytes of text-relocation info in file */
	vm_size_t a_drsize;	/* bytes of data-relocation info in file */
};

#define	__LDPGSZ	8192

#ifndef	OMAGIC
#define OMAGIC 0407
#define NMAGIC 0410
#define ZMAGIC 0413
#endif

#define N_BADMAG(x) \
 (((x).a_magic)!=OMAGIC && ((x).a_magic)!=NMAGIC && ((x).a_magic)!=ZMAGIC)

/* Address of the bottom of the text segment. */
#define N_TXTADDR(x) \
	((x).a_tstart)

/* Address of the bottom of the data segment. */
#define N_DATADDR(x) \
	((x).a_dstart)

/* Text segment offset. */
#define	N_TXTOFF(ex) \
	((ex).a_magic == ZMAGIC ? 0 : sizeof(struct exec))

/* Data segment offset. */
#define	N_DATOFF(ex) \
	((N_TXTOFF(ex)) + ((ex).a_magic != ZMAGIC ? (ex).a_text : \
	__LDPGSZ + ((ex).a_text - 1 & ~(__LDPGSZ - 1))))

/* Symbol table offset. */
#define N_SYMOFF(x) \
 (N_TXTOFF(x) + (x).a_text + (x).a_data + (x).a_trsize + (x).a_drsize)

#define N_STROFF(x) \
 (N_SYMOFF(x) + (x).a_syms)

struct nlist {
	union {
		char		*n_name;
		struct nlist	*n_next;
		long		 n_strx;
		vm_offset_t	 n_pad_cross_compile;
	} n_un;
	char		n_type;
	char		n_other;
	short		n_desc;
	int		n_pad;
	vm_offset_t 	n_value;
};

#define N_UNDF	0x0
#define N_ABS	0x2
#define N_TEXT	0x4
#define N_DATA	0x6
#define N_BSS	0x8
#define N_COMM	0x12
#define N_FN	0x1f
#define N_EXT	0x1
#define N_TYPE	0x1e
#define N_STAB	0xe0

#define	N_FORMAT	"%016lx"

enum reloc_type
{
    RELOC_0_7,      RELOC_0_15,      RELOC_0_31,     RELOC_0_63,
    RELOC_0_25,	    RELOC_16_31,     RELOC_WDISP21,  RELOC_BASE21,
    RELOC_WDISP14,  RELOC_BASE14,    RELOC_32_47,    RELOC_48_63,
    RELOC_U_16_31,  RELOC_U_32_47,   RELOC_U_48_63,  RELOC_0_12,
    RELOC_0_8, RELOC_0_10,

    NO_RELOC,

    BRELOC_0_15,    BRELOC_0_31,     BRELOC_16_31,
    BRSET_0_15,     BRSET_16_31
};

struct relocation_info {
	vm_offset_t	r_address;
	unsigned int	r_index;
	unsigned char	r_extern;
	enum reloc_type r_type 	   : 8;
	/* short	pad; */
	integer_t	r_addend;
};

#endif /* _A_OUT_H_ */
