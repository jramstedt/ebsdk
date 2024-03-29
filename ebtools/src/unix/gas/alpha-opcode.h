/* Table of opcodes for the alpha.
   Copyright (C) 1989 Free Software Foundation, Inc.

This file is part of GAS, the GNU Assembler, and GDB, the GNU disassembler.

GAS/GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GAS/GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GAS or GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* $Id: alpha-opcode.h,v 1.1.1.1 1998/12/29 21:36:27 paradis Exp $
 * $Log: alpha-opcode.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:27  paradis
 * Initial CVS checkin
 *
 * Revision 4.18  1998/01/07  15:02:55  gries
 * added newinstructions
 *
 * Revision 4.17  1998/01/07  14:29:58  gries
 * added new instructions
 *
 * Revision 4.16  1997/07/31  19:27:05  fdh
 * Corrected syntax on ev6 hw_ret instructions.
 *
 * Revision 4.15  1997/07/31  18:31:30  fdh
 * Corrected the hw_ret opcodes for ev6.
 *
 * Revision 4.14  1997/07/27  17:53:01  fdh
 * Corrected ev6 hw_mtpr and hw_mfpr instructions.  The unused
 * register field must specify r31.
 *
 * Revision 4.13  1997/07/21  22:31:19  fdh
 * Added whint and ecb instructions.
 *
 * Revision 4.12  1997/07/10  11:34:38  fdh
 * Added 21264 PAL instructions pal_21264_opcodes[]
 *
 * Revision 4.11  1997/05/22  17:58:12  fdh
 * Oops... type on ldbu instruction.
 *
 * Revision 4.10  1997/05/08  18:17:48  fdh
 * Added load/store byte/word instructions.  Extended cpu option switchs.
 *
 * Revision 4.9  1995/04/20  15:58:28  cruz
 * Fixed bugs in hw_st/paq and hw_st/pa for dc21064 version
 * Added more combinations of switches to dc21164 version.
 * (hw_ld and hw_st switches, that is).
 *
 *
 */

#if !defined(__STDC__) && !defined(const)
#define const
#endif

/*
 * Structure of an opcode table entry.
 */
struct alpha_opcode
{
    const char *name;
    const unsigned int match;	/* fixed encoding part of instruction */
    const int isa_float;
    const char *args;
};

/*
   All alpha opcodes are 32 bits, except for the `set' instruction (really
   a macro), which is 64 bits.  It is handled as a special case.

   The match component is a mask saying which bits must match a
   particular opcode in order for an instruction to be an instance
   of that opcode.

   The args component is a string containing one character
   for each operand of the instruction.

Kinds of operands:
   1    ra register
   2    rb register
   3	rc register
   r	same register for ra and rc
   R	same register for ra and rb
   e    fa floating point register.
   f    fb floating point register.
   g    fc floating point register.
   I	26 bit immediate
   l	16 low bits of immediate
   h	16 high(er) bits of immediate		why ?
   L	22 bit PC relative immediate.
   i	14 bit PC relative immediate.
   b	8 bit literal
   G	Base-register GET at address, needs macro-expansion
   P	Base-register PUT at address, needs macro-expansion
   Bn	builtin macro
   t	twelve bit displacement
   T    ten bit displacement
   8	eight bit index

Syntactic elements
   (
   )	base register in "offset(base)" addressing
   ,	separator

Builtin functions (look like macros to the programmer)
   %br		the current base register
   la	%r,A	load the (64bit) address in register %r
   li	%r,N	load the constant N in register %r
   lo(A)	low 16 bits of Address (relocatable)
   uml(A)	med-low 16 bits, unchanged
   umh(A)	med-high 16 bits, unchanged
   uhi(A)	high 16 bits, unchanged
   ml(A)	med-low, adjusted viz sign of lo(A)
   mh(A)	med-high, adjusted viz sign of ml(A)
   hi(A)	high, adjusted viz sign of mh(A)
   
*/

/* The order of the opcodes in this table is significant:
   
   * The assembler requires that all instances of the same mnemonic must be
   consecutive.  If they aren't, the assembler will bomb at runtime.

   * The disassembler should not care about the order of the opcodes.  */

static struct alpha_opcode alpha_opcodes[] =
{

{ "ldgp",       0x00000000, 0, "3,l(1)Ba" },	/* builtin */
{ "setgp",      0x00000000, 0, "0(1)Bb" },	/* builtin */

{ "reml",       0x00000000, 0, "1,2,3B0" },	/* builtin */
{ "divl",       0x00000000, 0, "1,2,3B1" },	/* builtin */
{ "remq",       0x00000000, 0, "1,2,3B2" },	/* builtin */
{ "divq",       0x00000000, 0, "1,2,3B3" },	/* builtin */
{ "remlu",      0x00000000, 0, "1,2,3B4" },	/* builtin */
{ "divlu",      0x00000000, 0, "1,2,3B5" },	/* builtin */
{ "remqu",      0x00000000, 0, "1,2,3B6" },	/* builtin */
{ "divqu",      0x00000000, 0, "1,2,3B7" },	/* builtin */

{ "lda",        0x20000000, 0, "1,l(2)" },	/* 6o+5a+5b+16d */
{ "lda",        0x20000000, 0, "1,G" },		/* regbase macro */
{ "ldi", 	0x201F0000, 0, "1,l"},		/* ldi ra,lit == lda ra,lit(r31) */
{ "ldah",       0x24000000, 0, "1,l(2)" },
{ "ldah",       0x24000000, 0, "1,G" },		/* regbase macro */
{ "lui", 	0x241F0000, 0, "1,l"},		/* lui ra,lit == ldah ra,lit(r31) */
{ "ldil",       0x20000000, 0, "1,G" },		/* macro */
{ "ldiq",       0x20000000, 0, "1,G" },		/* (broken) macro */

{ "ldbu",       0x28000000, 0, "1,l(2)" },
{ "ldbu",       0x28000000, 0, "1,G" },		/* regbase macro */
{ "ldwu",       0x30000000, 0, "1,l(2)" },
{ "ldwu",       0x30000000, 0, "1,G" },		/* regbase macro */
{ "ldl",        0xa0000000, 0, "1,l(2)" },
{ "ldl",        0xa0000000, 0, "1,G" },		/* regbase macro */
{ "ldl_l",      0xa8000000, 0, "1,l(2)" },
{ "ldl_l",      0xa8000000, 0, "1,G" },		/* regbase macro */
{ "ldq",        0xa4000000, 0, "1,l(2)" },
{ "ldq",        0xa4000000, 0, "1,G" },		/* regbase macro */
{ "ldq_u",      0x2c000000, 0, "1,l(2)" },
{ "ldq_u",      0x2c000000, 0, "1,G" },		/* regbase macro */
{ "ldq_l",      0xac000000, 0, "1,l(2)" },
{ "ldq_l",      0xac000000, 0, "1,G" },		/* regbase macro */

{ "stb",        0x38000000, 0, "1,l(2)" },
{ "stb",        0x38000000, 0, "1,P" },		/* regbase macro */
{ "stw",        0x34000000, 0, "1,l(2)" },
{ "stw",        0x34000000, 0, "1,P" },		/* regbase macro */
{ "stl",        0xb0000000, 0, "1,l(2)" },
{ "stl",        0xb0000000, 0, "1,P" },		/* regbase macro */
{ "stl_c",      0xb8000000, 0, "1,l(2)" },
{ "stl_c",      0xb8000000, 0, "1,P" },		/* regbase macro */
{ "stq",        0xb4000000, 0, "1,l(2)" },
{ "stq",        0xb4000000, 0, "1,P" },		/* regbase macro */
{ "stq_u",      0x3c000000, 0, "1,l(2)" },
{ "stq_u",      0x3c000000, 0, "1,P" },		/* regbase macro */
{ "stq_c",      0xbc000000, 0, "1,l(2)" },
{ "stq_c",      0xbc000000, 0, "1,P" },		/* regbase macro */

{ "beq",        0xe4000000, 0, "1,L" },		/* 6o+5a+21d */
{ "bne",        0xf4000000, 0, "1,L" },
{ "blt",        0xe8000000, 0, "1,L" },
{ "ble",        0xec000000, 0, "1,L" },
{ "bgt",        0xfc000000, 0, "1,L" },
{ "bge",        0xf8000000, 0, "1,L" },
{ "blbc",       0xe0000000, 0, "1,L" },
{ "blbs",       0xf0000000, 0, "1,L" },

{ "br",         0xc0000000, 0, "1,L" },
{ "br",         0xc3e00000, 0, "L" },		/* macro: br zero,disp */
{ "bsr",        0xd0000000, 0, "1,L" },
{ "bsr",        0xd3500000, 0, "L" },		/* macro: bsr $ra,L */

{ "jmp",        0x68000000, 0, "1,(2),i" },	/* 6o+5a+5b+2A+14d */
{ "jmp",        0x68000000, 0, "1,(2)" },
{ "jsr",        0x68004000, 0, "1,(2),i" },
{ "jsr",        0x68004000, 0, "1,(2)" },
{ "jsr",        0x68004000, 0, "1,Bc" },		/* macro: lda $pv,L;jsr .. */

{ "ret",        0x68008000, 0, "1,(2),i" },
{ "ret",        0x68008000, 0, "1,(2)" },
{ "ret",        0x6b5a8000, 0, "" },		/* macro: ret ra,(ra) */
{ "ret",        0x6be08000, 0, "(2)" },		/* macro: ret zero,(2) */
{ "ret",        0x681a8000, 0, "1" },		/* macro: ret 1,(ra) */
{ "jcr",        0x6800c000, 0, "1,(2)" },
{ "jsr_coroutine",        0x6800c000, 0, "1,(2)" },

{ "addl",       0x40000000, 0, "1,2,3" },		/* 6o+5a+5b+4z+7f+5c */
{ "addl",       0x40001000, 0, "1,b,3" },		/* 6o+5a+8n+1+7f+5c */
{ "addl/v",     0x40000800, 0, "1,2,3" },
{ "addl/v",     0x40001800, 0, "1,b,3" },
{ "s4addl",     0x40000040, 0, "1,2,3" },
{ "s4addl",     0x40001040, 0, "1,b,3" },
{ "s8addl",     0x40000240, 0, "1,2,3" },
{ "s8addl",     0x40001240, 0, "1,b,3" },
{ "addq",       0x40000400, 0, "1,2,3" },
{ "addq",       0x40001400, 0, "1,b,3" },
{ "addq/v",     0x40000c00, 0, "1,2,3" },

{ "addq/v",     0x40001c00, 0, "1,b,3" },
{ "s4addq",     0x40000440, 0, "1,2,3" },
{ "s4addq",     0x40001440, 0, "1,b,3" },
{ "s8addq",     0x40000640, 0, "1,2,3" },
{ "s8addq",     0x40001640, 0, "1,b,3" },
{ "cmpeq",      0x400005a0, 0, "1,2,3" },
{ "cmpeq",      0x400015a0, 0, "1,b,3" },
{ "cmplt",      0x400009a0, 0, "1,2,3" },
{ "cmplt",      0x400019a0, 0, "1,b,3" },
{ "cmple",      0x40000da0, 0, "1,2,3" },
{ "cmple",      0x40001da0, 0, "1,b,3" },
{ "cmpult",     0x400003a0, 0, "1,2,3" },
{ "cmpult",     0x400013a0, 0, "1,b,3" },
{ "cmpule",     0x400007a0, 0, "1,2,3" },
{ "cmpule",     0x400017a0, 0, "1,b,3" },
{ "subl",       0x40000120, 0, "1,2,3" },
{ "subl",       0x40001120, 0, "1,b,3" },
{ "subl/v",     0x40000920, 0, "1,2,3" },
{ "subl/v",     0x40001920, 0, "1,b,3" },
{ "s4subl",     0x40000160, 0, "1,2,3" },
{ "s4subl",     0x40001160, 0, "1,b,3" },
{ "s8subl",     0x40000360, 0, "1,2,3" },
{ "s8subl",     0x40001360, 0, "1,b,3" },
{ "subq",       0x40000520, 0, "1,2,3" },
{ "subq",       0x40001520, 0, "1,b,3" },
{ "subq/v",     0x40000d20, 0, "1,2,3" },
{ "subq/v",     0x40001d20, 0, "1,b,3" },
{ "s4subq",     0x40000560, 0, "1,2,3" },
{ "s4subq",     0x40001560, 0, "1,b,3" },
{ "s8subq",     0x40000760, 0, "1,2,3" },
{ "s8subq",     0x40001760, 0, "1,b,3" },
{ "cmpbge",     0x400001e0, 0, "1,2,3" },
{ "cmpbge",     0x400011e0, 0, "1,b,3" },

{ "mull",       0x4c000000, 0, "1,2,3" },
{ "mull",       0x4c001000, 0, "1,b,3" },
{ "mull/v",     0x4c000800, 0, "1,2,3" },
{ "mull/v",     0x4c001800, 0, "1,b,3" },
{ "mulq",       0x4c000400, 0, "1,2,3" },
{ "mulq",       0x4c001400, 0, "1,b,3" },
{ "mulq/v",     0x4c000c00, 0, "1,2,3" },
{ "mulq/v",     0x4c001c00, 0, "1,b,3" },
{ "umulh",      0x4c000600, 0, "1,2,3" },
{ "umulh",      0x4c001600, 0, "1,b,3" },

{ "clr",        0x47ff0400, 0, "3" },		/* macro: or zero,zero,rc */
{ "negl",       0x43e00120, 0, "2,3" },		/* macro: subl zero,rb,rc */
{ "negl_v",     0x43e00920, 0, "2,3" },		/* macro: subl_v zero,rb,rc */
{ "negq",       0x43e00520, 0, "2,3" },		/* macro: subq zero,rb,rc */
{ "negq_v",     0x43e00d20, 0, "2,3" },		/* macro: subq_v zero,rb,rc */
{ "sextl",      0x43e00000, 0, "2,3" },		/* macro: addl zero,rb,rc */

{ "and",        0x44000000, 0, "1,2,3" },
{ "and",        0x44001000, 0, "1,b,3" },
{ "and",        0x44000000, 0, "r,2" },		/* macro: and ra,rb,ra */
{ "and",        0x44001000, 0, "r,b" },		/* macro: and ra,#,ra  */
{ "or",         0x44000400, 0, "1,2,3" },
{ "or",         0x44001400, 0, "1,b,3" },
{ "or",         0x44000400, 0, "r,2" },		/* macro: or ra,rb,ra */
{ "or",         0x44001400, 0, "r,b" },		/* macro: or ra,#,ra  */
{ "bis",        0x44000400, 0, "1,2,3" },
{ "bis",        0x44001400, 0, "1,b,3" },
{ "bis",        0x44000400, 0, "r,2" },		/* macro: or ra,rb,ra */
{ "bis",        0x44001400, 0, "r,b" },		/* macro: or ra,#,ra  */
{ "movi",	0x47E01400, 0, "b,3"},		/* movi lit,rc == bis r31,lit,rc */
{ "xor",        0x44000800, 0, "1,2,3" },
{ "xor",        0x44001800, 0, "1,b,3" },
{ "xor",        0x44000800, 0, "r,2" },		/* macro: ra,rb,ra */
{ "xor",        0x44001800, 0, "r,b" },		/* macro: ra,#,ra */
{ "andnot",     0x44000100, 0, "1,2,3" },
{ "andnot",     0x44001100, 0, "1,b,3" },
{ "andnot",     0x44000100, 0, "r,2" },		/* macro: ra,#,ra */
{ "andnot",     0x44001100, 0, "r,b" },		/* macro: ra,#,ra */
{ "bic",        0x44000100, 0, "1,2,3" },
{ "bic",        0x44001100, 0, "1,b,3" },
{ "bic",        0x44000100, 0, "r,2" },		/* macro: ra,#,ra */
{ "bic",        0x44001100, 0, "r,b" },		/* macro: ra,#,ra */
{ "ornot",      0x44000500, 0, "1,2,3" },
{ "ornot",      0x44001500, 0, "1,b,3" },
{ "ornot",      0x44000500, 0, "r,2" },		/* macro: ra,#,ra */
{ "ornot",      0x44001500, 0, "r,b" },		/* macro: ra,#,ra */
{ "not",        0x47e00500, 0, "2,3" },		/* macro: ornot zero,.. */
{ "not",        0x47e01500, 0, "b,3" },
{ "xornot",     0x44000900, 0, "1,2,3" },
{ "xornot",     0x44001900, 0, "1,b,3" },
{ "xornot",     0x44000900, 0, "r,2" },		/* macro: ra,#,ra */
{ "xornot",     0x44001900, 0, "r,b" },		/* macro: ra,#,ra */
{ "eqv",        0x44000900, 0, "1,2,3" },
{ "eqv",        0x44001900, 0, "1,b,3" },
{ "eqv",        0x44000900, 0, "r,2" },		/* macro: ra,#,ra */
{ "eqv",        0x44001900, 0, "r,b" },		/* macro: ra,#,ra */

{ "cmoveq",     0x44000480, 0, "1,2,3" },
{ "cmoveq",     0x44001480, 0, "1,b,3" },
{ "cmovne",     0x440004c0, 0, "1,2,3" },
{ "cmovne",     0x440014c0, 0, "1,b,3" },
{ "cmovlt",     0x44000880, 0, "1,2,3" },
{ "cmovlt",     0x44001880, 0, "1,b,3" },
{ "cmovle",     0x44000c80, 0, "1,2,3" },
{ "cmovle",     0x44001c80, 0, "1,b,3" },
{ "cmovgt",     0x44000cc0, 0, "1,2,3" },
{ "cmovgt",     0x44001cc0, 0, "1,b,3" },
{ "cmovge",     0x440008c0, 0, "1,2,3" },
{ "cmovge",     0x440018c0, 0, "1,b,3" },
{ "cmovlbc",    0x440002c0, 0, "1,2,3" },
{ "cmovlbc",    0x440012c0, 0, "1,b,3" },
{ "cmovlbs",    0x44000280, 0, "1,2,3" },
{ "cmovlbs",    0x44001280, 0, "1,b,3" },

{ "sll",        0x48000720, 0, "1,2,3" },
{ "sll",        0x48001720, 0, "1,b,3" },
{ "srl",        0x48000680, 0, "1,2,3" },
{ "srl",        0x48001680, 0, "1,b,3" },
{ "sra",        0x48000780, 0, "1,2,3" },
{ "sra",        0x48001780, 0, "1,b,3" },

{ "extbl",      0x480000c0, 0, "1,2,3" },
{ "extbl",      0x480010c0, 0, "1,b,3" },
{ "extwl",      0x480002c0, 0, "1,2,3" },
{ "extwl",      0x480012c0, 0, "1,b,3" },
{ "extll",      0x480004c0, 0, "1,2,3" },
{ "extll",      0x480014c0, 0, "1,b,3" },
{ "extql",      0x480006c0, 0, "1,2,3" },
{ "extql",      0x480016c0, 0, "1,b,3" },
{ "extwh",      0x48000b40, 0, "1,2,3" },
{ "extwh",      0x48001b40, 0, "1,b,3" },
{ "extlh",      0x48000d40, 0, "1,2,3" },
{ "extlh",      0x48001d40, 0, "1,b,3" },
{ "extqh",      0x48000f40, 0, "1,2,3" },
{ "extqh",      0x48001f40, 0, "1,b,3" },
{ "insbl",      0x48000160, 0, "1,2,3" },
{ "insbl",      0x48001160, 0, "1,b,3" },
{ "inswl",      0x48000360, 0, "1,2,3" },
{ "inswl",      0x48001360, 0, "1,b,3" },
{ "insll",      0x48000560, 0, "1,2,3" },
{ "insll",      0x48001560, 0, "1,b,3" },
{ "insql",      0x48000760, 0, "1,2,3" },
{ "insql",      0x48001760, 0, "1,b,3" },
{ "inswh",      0x48000ae0, 0, "1,2,3" },
{ "inswh",      0x48001ae0, 0, "1,b,3" },
{ "inslh",      0x48000ce0, 0, "1,2,3" },
{ "inslh",      0x48001ce0, 0, "1,b,3" },
{ "insqh",      0x48000ee0, 0, "1,2,3" },
{ "insqh",      0x48001ee0, 0, "1,b,3" },
{ "mskbl",      0x48000040, 0, "1,2,3" },
{ "mskbl",      0x48001040, 0, "1,b,3" },
{ "mskwl",      0x48000240, 0, "1,2,3" },
{ "mskwl",      0x48001240, 0, "1,b,3" },
{ "mskll",      0x48000440, 0, "1,2,3" },
{ "mskll",      0x48001440, 0, "1,b,3" },
{ "mskql",      0x48000640, 0, "1,2,3" },
{ "mskql",      0x48001640, 0, "1,b,3" },
{ "mskwh",      0x48000a40, 0, "1,2,3" },
{ "mskwh",      0x48001a40, 0, "1,b,3" },
{ "msklh",      0x48000c40, 0, "1,2,3" },
{ "msklh",      0x48001c40, 0, "1,b,3" },
{ "mskqh",      0x48000e40, 0, "1,2,3" },
{ "mskqh",      0x48001e40, 0, "1,b,3" },
{ "zap",        0x48000600, 0, "1,2,3" },
{ "zap",        0x48001600, 0, "1,b,3" },
{ "zapnot",     0x48000620, 0, "1,2,3" },
{ "zapnot",     0x48001620, 0, "1,b,3" },

/*
 * Floating point instructions
 */
{ "ldf",        0x80000000, 1, "e,l(2)" },	/* 6o+5a+5b+16d */
{ "ldf",        0x80000000, 1, "e,G" },		/* regbase macro */
{ "ldg",        0x84000000, 1, "e,l(2)" },
{ "ldg",        0x84000000, 1, "e,G" },		/* regbase macro */
{ "lds",        0x88000000, 1, "e,l(2)" },
{ "lds",        0x88000000, 1, "e,G" },		/* regbase macro */
{ "ldt",        0x8c000000, 1, "e,l(2)" },
{ "ldt",        0x8c000000, 1, "e,G" },		/* regbase macro */
{ "stf",        0x90000000, 1, "e,l(2)" },
{ "stf",        0x90000000, 1, "e,P" },		/* regbase macro */
{ "stg",        0x94000000, 1, "e,l(2)" },
{ "stg",        0x94000000, 1, "e,P" },		/* regbase macro */
{ "sts",        0x98000000, 1, "e,l(2)" },
{ "sts",        0x98000000, 1, "e,P" },		/* regbase macro */
{ "stt",        0x9c000000, 1, "e,l(2)" },
{ "stt",        0x9c000000, 1, "e,P" },		/* regbase macro */

{ "fbeq",       0xc4000000, 1, "e,L" },		/* 6o+5a+21d */
{ "fbne",       0xd4000000, 1, "e,L" },
{ "fblt",       0xc8000000, 1, "e,L" },
{ "fble",       0xcc000000, 1, "e,L" },
{ "fbgt",       0xdc000000, 1, "e,L" },
{ "fbge",       0xd8000000, 1, "e,L" },

/* All subsets (opcode 0x17) */
{ "cpys",       0x5c000400, 1, "e,f,g" },		/* 6o+5a+5b+11f+5c */
{ "cpysn",      0x5c000420, 1, "e,f,g" },
{ "cpyse",      0x5c000440, 1, "e,f,g" },

{ "cvtlq",      0x5fe00200, 1, "f,g" },
{ "cvtql",      0x5fe00600, 1, "f,g" },
{ "cvtql/v",    0x5fe02600, 1, "f,g" },
{ "cvtql/sv",   0x5fe06600, 1, "f,g" },

{ "fcmoveq",    0x5c000540, 1, "e,f,g" },
{ "fcmovne",    0x5c000560, 1, "e,f,g" },
{ "fcmovlt",    0x5c000580, 1, "e,f,g" },
{ "fcmovle",    0x5c0005c0, 1, "e,f,g" },
{ "fcmovgt",    0x5c0005e0, 1, "e,f,g" },
{ "fcmovge",    0x5c0005a0, 1, "e,f,g" },

{ "mf_fpcr",    0x5c0004a0, 1, "E" },
{ "mt_fpcr",    0x5c000480, 1, "E" },

/* Vax subset (opcode 0x15) */
{ "addf",       0x54001000, 1, "e,f,g" },
{ "addf/c",     0x54000000, 1, "e,f,g" },
{ "addf/u",     0x54003000, 1, "e,f,g" },
{ "addf/uc",    0x54002000, 1, "e,f,g" },
{ "addf/s",     0x54009000, 1, "e,f,g" },
{ "addf/sc",    0x54008000, 1, "e,f,g" },
{ "addf/su",    0x5400b000, 1, "e,f,g" },
{ "addf/suc",   0x5400a000, 1, "e,f,g" },
{ "addg",       0x54001400, 1, "e,f,g" },
{ "addg/c",     0x54000400, 1, "e,f,g" },
{ "addg/u",     0x54003400, 1, "e,f,g" },
{ "addg/uc",    0x54002400, 1, "e,f,g" },
{ "addg/s",     0x54009400, 1, "e,f,g" },
{ "addg/sc",    0x54008400, 1, "e,f,g" },
{ "addg/su",    0x5400b400, 1, "e,f,g" },
{ "addg/suc",   0x5400a400, 1, "e,f,g" },
{ "subf",       0x54001020, 1, "e,f,g" },
{ "subf/c",     0x54000020, 1, "e,f,g" },
{ "subf/u",     0x54003020, 1, "e,f,g" },
{ "subf/uc",    0x54002020, 1, "e,f,g" },
{ "subf/s",     0x54009020, 1, "e,f,g" },
{ "subf/sc",    0x54008020, 1, "e,f,g" },
{ "subf/su",    0x5400b020, 1, "e,f,g" },
{ "subf/suc",   0x5400a020, 1, "e,f,g" },
{ "subg",       0x54001420, 1, "e,f,g" },
{ "subg/c",     0x54000420, 1, "e,f,g" },
{ "subg/u",     0x54003420, 1, "e,f,g" },
{ "subg/uc",    0x54002420, 1, "e,f,g" },
{ "subg/s",     0x54009420, 1, "e,f,g" },
{ "subg/sc",    0x54008420, 1, "e,f,g" },
{ "subg/su",    0x5400b420, 1, "e,f,g" },
{ "subg/suc",   0x5400a420, 1, "e,f,g" },

{ "cmpgeq",     0x540014a0, 1, "e,f,g" },
{ "cmpgeq/s",   0x540094a0, 1, "e,f,g" },
{ "cmpglt",     0x540014c0, 1, "e,f,g" },
{ "cmpglt/s",   0x540094c0, 1, "e,f,g" },
{ "cmpgle",     0x540014e0, 1, "e,f,g" },
{ "cmpgle/s",   0x540094e0, 1, "e,f,g" },

{ "cvtgq",      0x57e015e0, 1, "f,g" },
{ "cvtgq/c",    0x57e005e0, 1, "f,g" },
{ "cvtgq/v",    0x57e035e0, 1, "f,g" },
{ "cvtgq/vc",   0x57e025e0, 1, "f,g" },
{ "cvtgq/s",    0x57e095e0, 1, "f,g" },
{ "cvtgq/sc",   0x57e085e0, 1, "f,g" },
{ "cvtgq/sv",   0x57e0b5e0, 1, "f,g" },
{ "cvtgq/svc",  0x57e0a5e0, 1, "f,g" },
{ "cvtqf",      0x57e01780, 1, "f,g" },
{ "cvtqf/c",    0x57e00780, 1, "f,g" },
{ "cvtqf/s",    0x57e09780, 1, "f,g" },
{ "cvtqf/sc",   0x57e08780, 1, "f,g" },
{ "cvtqg",      0x57e017c0, 1, "f,g" },
{ "cvtqg/c",    0x57e007c0, 1, "f,g" },
{ "cvtqg/s",    0x57e097c0, 1, "f,g" },
{ "cvtqg/sc",   0x57e087c0, 1, "f,g" },
{ "cvtdg",      0x57e013c0, 1, "f,g" },
{ "cvtdg/c",    0x57e003c0, 1, "f,g" },
{ "cvtdg/u",    0x57e033c0, 1, "f,g" },
{ "cvtdg/uc",   0x57e023c0, 1, "f,g" },
{ "cvtdg/s",    0x57e093c0, 1, "f,g" },
{ "cvtdg/sc",   0x57e083c0, 1, "f,g" },
{ "cvtdg/su",   0x57e0b3c0, 1, "f,g" },
{ "cvtdg/suc",  0x57e0a3c0, 1, "f,g" },
{ "cvtgd",      0x57e015a0, 1, "f,g" },
{ "cvtgd/c",    0x57e005a0, 1, "f,g" },
{ "cvtgd/u",    0x57e035a0, 1, "f,g" },
{ "cvtgd/uc",   0x57e025a0, 1, "f,g" },
{ "cvtgd/s",    0x57e095a0, 1, "f,g" },
{ "cvtgd/sc",   0x57e085a0, 1, "f,g" },
{ "cvtgd/su",   0x57e0b5a0, 1, "f,g" },
{ "cvtgd/suc",  0x57e0a5a0, 1, "f,g" },
{ "cvtgf",      0x57e01580, 1, "f,g" },
{ "cvtgf/c",    0x57e00580, 1, "f,g" },
{ "cvtgf/u",    0x57e03580, 1, "f,g" },
{ "cvtgf/uc",   0x57e02580, 1, "f,g" },
{ "cvtgf/s",    0x57e09580, 1, "f,g" },
{ "cvtgf/sc",   0x57e08580, 1, "f,g" },
{ "cvtgf/su",   0x57e0b580, 1, "f,g" },
{ "cvtgf/suc",  0x57e0a580, 1, "f,g" },

{ "divf",       0x54001060, 1, "e,f,g" },
{ "divf/c",     0x54000060, 1, "e,f,g" },
{ "divf/u",     0x54003060, 1, "e,f,g" },
{ "divf/uc",    0x54002060, 1, "e,f,g" },
{ "divf/s",     0x54009060, 1, "e,f,g" },
{ "divf/sc",    0x54008060, 1, "e,f,g" },
{ "divf/su",    0x5400b060, 1, "e,f,g" },
{ "divf/suc",   0x5400a060, 1, "e,f,g" },
{ "divg",       0x54001460, 1, "e,f,g" },
{ "divg/c",     0x54000460, 1, "e,f,g" },
{ "divg/u",     0x54003460, 1, "e,f,g" },
{ "divg/uc",    0x54002460, 1, "e,f,g" },
{ "divg/s",     0x54009460, 1, "e,f,g" },
{ "divg/sc",    0x54008460, 1, "e,f,g" },
{ "divg/su",    0x5400b460, 1, "e,f,g" },
{ "divg/suc",   0x5400a460, 1, "e,f,g" },
{ "mulf",       0x54001040, 1, "e,f,g" },
{ "mulf/c",     0x54000040, 1, "e,f,g" },
{ "mulf/u",     0x54003040, 1, "e,f,g" },
{ "mulf/uc",    0x54002040, 1, "e,f,g" },
{ "mulf/s",     0x54009040, 1, "e,f,g" },
{ "mulf/sc",    0x54008040, 1, "e,f,g" },
{ "mulf/su",    0x5400b040, 1, "e,f,g" },
{ "mulf/suc",   0x5400a040, 1, "e,f,g" },
{ "mulg",       0x54001440, 1, "e,f,g" },
{ "mulg/c",     0x54000440, 1, "e,f,g" },
{ "mulg/u",     0x54003440, 1, "e,f,g" },
{ "mulg/uc",    0x54002440, 1, "e,f,g" },
{ "mulg/s",     0x54009440, 1, "e,f,g" },
{ "mulg/sc",    0x54008440, 1, "e,f,g" },
{ "mulg/su",    0x5400b440, 1, "e,f,g" },
{ "mulg/suc",   0x5400a440, 1, "e,f,g" },

/* IEEE subset (opcode 0x16) */
{ "adds",       0x58001000, 1, "e,f,g" },
{ "adds/c",     0x58000000, 1, "e,f,g" },
{ "adds/m",     0x58000800, 1, "e,f,g" },
{ "adds/d",     0x58001800, 1, "e,f,g" },
{ "adds/u",     0x58003000, 1, "e,f,g" },
{ "adds/uc",    0x58002000, 1, "e,f,g" },
{ "adds/um",    0x58002800, 1, "e,f,g" },
{ "adds/ud",    0x58003800, 1, "e,f,g" },
{ "adds/su",    0x5800b000, 1, "e,f,g" },
{ "adds/suc",   0x5800a000, 1, "e,f,g" },
{ "adds/sum",   0x5800a800, 1, "e,f,g" },
{ "adds/sud",   0x5800b800, 1, "e,f,g" },
{ "adds/sui",   0x5800f000, 1, "e,f,g" },
{ "adds/suic",  0x5800e000, 1, "e,f,g" },
{ "adds/suim",  0x5800e800, 1, "e,f,g" },
{ "adds/suid",  0x5800f800, 1, "e,f,g" },
{ "addt",       0x58001400, 1, "e,f,g" },
{ "addt/c",     0x58000400, 1, "e,f,g" },
{ "addt/m",     0x58000c00, 1, "e,f,g" },
{ "addt/d",     0x58001c00, 1, "e,f,g" },
{ "addt/u",     0x58003400, 1, "e,f,g" },
{ "addt/uc",    0x58002400, 1, "e,f,g" },
{ "addt/um",    0x58002c00, 1, "e,f,g" },
{ "addt/ud",    0x58003c00, 1, "e,f,g" },
{ "addt/su",    0x5800b400, 1, "e,f,g" },
{ "addt/suc",   0x5800a400, 1, "e,f,g" },
{ "addt/sum",   0x5800ac00, 1, "e,f,g" },
{ "addt/sud",   0x5800bc00, 1, "e,f,g" },
{ "addt/sui",   0x5800f400, 1, "e,f,g" },
{ "addt/suic",  0x5800e400, 1, "e,f,g" },
{ "addt/suim",  0x5800ec00, 1, "e,f,g" },
{ "addt/suid",  0x5800fc00, 1, "e,f,g" },
{ "subs",       0x58001020, 1, "e,f,g" },
{ "subs/c",     0x58000020, 1, "e,f,g" },
{ "subs/m",     0x58000820, 1, "e,f,g" },
{ "subs/d",     0x58001820, 1, "e,f,g" },
{ "subs/u",     0x58003020, 1, "e,f,g" },
{ "subs/uc",    0x58002020, 1, "e,f,g" },
{ "subs/um",    0x58002820, 1, "e,f,g" },
{ "subs/ud",    0x58003820, 1, "e,f,g" },
{ "subs/su",    0x5800b020, 1, "e,f,g" },
{ "subs/suc",   0x5800a020, 1, "e,f,g" },
{ "subs/sum",   0x5800a820, 1, "e,f,g" },
{ "subs/sud",   0x5800b820, 1, "e,f,g" },
{ "subs/sui",   0x5800f020, 1, "e,f,g" },
{ "subs/suic",  0x5800e020, 1, "e,f,g" },
{ "subs/suim",  0x5800e820, 1, "e,f,g" },
{ "subs/suid",  0x5800f820, 1, "e,f,g" },
{ "subt",       0x58001420, 1, "e,f,g" },
{ "subt/c",     0x58000420, 1, "e,f,g" },
{ "subt/m",     0x58000c20, 1, "e,f,g" },
{ "subt/d",     0x58001c20, 1, "e,f,g" },
{ "subt/u",     0x58003420, 1, "e,f,g" },
{ "subt/uc",    0x58002420, 1, "e,f,g" },
{ "subt/um",    0x58002c20, 1, "e,f,g" },
{ "subt/ud",    0x58003c20, 1, "e,f,g" },
{ "subt/su",    0x5800b420, 1, "e,f,g" },
{ "subt/suc",   0x5800a420, 1, "e,f,g" },
{ "subt/sum",   0x5800ac20, 1, "e,f,g" },
{ "subt/sud",   0x5800bc20, 1, "e,f,g" },
{ "subt/sui",   0x5800f420, 1, "e,f,g" },
{ "subt/suic",  0x5800e420, 1, "e,f,g" },
{ "subt/suim",  0x5800ec20, 1, "e,f,g" },
{ "subt/suid",  0x5800fc20, 1, "e,f,g" },

{ "cmpteq",     0x580014a0, 1, "e,f,g" },
{ "cmpteq/su",  0x5800b4a0, 1, "e,f,g" },
{ "cmptlt",     0x580014c0, 1, "e,f,g" },
{ "cmptlt/su",  0x5800b4c0, 1, "e,f,g" },
{ "cmptle",     0x580014e0, 1, "e,f,g" },
{ "cmptle/su",  0x5800b4e0, 1, "e,f,g" },
{ "cmptun",     0x58001480, 1, "e,f,g" },
{ "cmptun/su",  0x5800b480, 1, "e,f,g" },

{ "cvttq",      0x5be015e0, 1, "f,g" },
{ "cvttq/c",    0x5be005e0, 1, "f,g" },
{ "cvttq/v",    0x5be035e0, 1, "f,g" },
{ "cvttq/vc",   0x5be025e0, 1, "f,g" },
{ "cvttq/sv",   0x5be0b5e0, 1, "f,g" },
{ "cvttq/svc",  0x5be0a5e0, 1, "f,g" },
{ "cvttq/svi",  0x5be0f5e0, 1, "f,g" },
{ "cvttq/svic", 0x5be0e5e0, 1, "f,g" },
{ "cvtqs",      0x5be01780, 1, "f,g" },
{ "cvtqs/c",    0x5be00780, 1, "f,g" },
{ "cvtqs/m",    0x5be00f80, 1, "f,g" },
{ "cvtqs/d",    0x5be01f80, 1, "f,g" },
{ "cvtqs/sui",  0x5be0f780, 1, "f,g" },
{ "cvtqs/suic", 0x5be0e780, 1, "f,g" },
{ "cvtqs/suim", 0x5be0ef80, 1, "f,g" },
{ "cvtqs/suid", 0x5be0ff80, 1, "f,g" },
{ "cvtqt",      0x5be017c0, 1, "f,g" },
{ "cvtqt/c",    0x5be007c0, 1, "f,g" },
{ "cvtqt/m",    0x5be00fc0, 1, "f,g" },
{ "cvtqt/d",    0x5be01fc0, 1, "f,g" },
{ "cvtqt/sui",  0x5be0f7c0, 1, "f,g" },
{ "cvtqt/suic", 0x5be0e7c0, 1, "f,g" },
{ "cvtqt/suim", 0x5be0efc0, 1, "f,g" },
{ "cvtqt/suid", 0x5be0ffc0, 1, "f,g" },
{ "cvtts",      0x5be01580, 1, "f,g" },
{ "cvtts/c",    0x5be00580, 1, "f,g" },
{ "cvtts/m",    0x5be00d80, 1, "f,g" },
{ "cvtts/d",    0x5be01d80, 1, "f,g" },
{ "cvtts/u",    0x5be03580, 1, "f,g" },
{ "cvtts/uc",   0x5be02580, 1, "f,g" },
{ "cvtts/um",   0x5be02d80, 1, "f,g" },
{ "cvtts/ud",   0x5be03d80, 1, "f,g" },
{ "cvtts/su",   0x5be0b580, 1, "f,g" },
{ "cvtts/suc",  0x5be0a580, 1, "f,g" },
{ "cvtts/sum",  0x5be0ad80, 1, "f,g" },
{ "cvtts/sud",  0x5be0bd80, 1, "f,g" },
{ "cvtts/sui",  0x5be0f580, 1, "f,g" },
{ "cvtts/suic", 0x5be0e580, 1, "f,g" },
{ "cvtts/suim", 0x5be0ed80, 1, "f,g" },
{ "cvtts/suid", 0x5be0fd80, 1, "f,g" },

{ "divs",       0x58001060, 1, "e,f,g" },
{ "divs/c",     0x58000060, 1, "e,f,g" },
{ "divs/m",     0x58000860, 1, "e,f,g" },
{ "divs/d",     0x58001860, 1, "e,f,g" },
{ "divs/u",     0x58003060, 1, "e,f,g" },
{ "divs/uc",    0x58002060, 1, "e,f,g" },
{ "divs/um",    0x58002860, 1, "e,f,g" },
{ "divs/ud",    0x58003860, 1, "e,f,g" },
{ "divs/su",    0x5800b060, 1, "e,f,g" },
{ "divs/suc",   0x5800a060, 1, "e,f,g" },
{ "divs/sum",   0x5800a860, 1, "e,f,g" },
{ "divs/sud",   0x5800b860, 1, "e,f,g" },
{ "divs/sui",   0x5800f060, 1, "e,f,g" },
{ "divs/suic",  0x5800e060, 1, "e,f,g" },
{ "divs/suim",  0x5800e860, 1, "e,f,g" },
{ "divs/suid",  0x5800f860, 1, "e,f,g" },
{ "divt",       0x58001460, 1, "e,f,g" },
{ "divt/c",     0x58000460, 1, "e,f,g" },
{ "divt/m",     0x58000c60, 1, "e,f,g" },
{ "divt/d",     0x58001c60, 1, "e,f,g" },
{ "divt/u",     0x58003460, 1, "e,f,g" },
{ "divt/uc",    0x58002460, 1, "e,f,g" },
{ "divt/um",    0x58002c60, 1, "e,f,g" },
{ "divt/ud",    0x58003c60, 1, "e,f,g" },
{ "divt/su",    0x5800b460, 1, "e,f,g" },
{ "divt/suc",   0x5800a460, 1, "e,f,g" },
{ "divt/sum",   0x5800ac60, 1, "e,f,g" },
{ "divt/sud",   0x5800bc60, 1, "e,f,g" },
{ "divt/sui",   0x5800f460, 1, "e,f,g" },
{ "divt/suic",  0x5800e460, 1, "e,f,g" },
{ "divt/suim",  0x5800ec60, 1, "e,f,g" },
{ "divt/suid",  0x5800fc60, 1, "e,f,g" },
{ "muls",       0x58001040, 1, "e,f,g" },
{ "muls/c",     0x58000040, 1, "e,f,g" },
{ "muls/m",     0x58000840, 1, "e,f,g" },
{ "muls/d",     0x58001840, 1, "e,f,g" },
{ "muls/u",     0x58003040, 1, "e,f,g" },
{ "muls/uc",    0x58002040, 1, "e,f,g" },
{ "muls/um",    0x58002840, 1, "e,f,g" },
{ "muls/ud",    0x58003840, 1, "e,f,g" },
{ "muls/su",    0x5800b040, 1, "e,f,g" },
{ "muls/suc",   0x5800a040, 1, "e,f,g" },
{ "muls/sum",   0x5800a840, 1, "e,f,g" },
{ "muls/sud",   0x5800b840, 1, "e,f,g" },
{ "muls/sui",   0x5800f040, 1, "e,f,g" },
{ "muls/suic",  0x5800e040, 1, "e,f,g" },
{ "muls/suim",  0x5800e840, 1, "e,f,g" },
{ "muls/suid",  0x5800f840, 1, "e,f,g" },
{ "mult",       0x58001440, 1, "e,f,g" },
{ "mult/c",     0x58000440, 1, "e,f,g" },
{ "mult/m",     0x58000c40, 1, "e,f,g" },
{ "mult/d",     0x58001c40, 1, "e,f,g" },
{ "mult/u",     0x58003440, 1, "e,f,g" },
{ "mult/uc",    0x58002440, 1, "e,f,g" },
{ "mult/um",    0x58002c40, 1, "e,f,g" },
{ "mult/ud",    0x58003c40, 1, "e,f,g" },
{ "mult/su",    0x5800b440, 1, "e,f,g" },
{ "mult/suc",   0x5800a440, 1, "e,f,g" },
{ "mult/sum",   0x5800ac40, 1, "e,f,g" },
{ "mult/sud",   0x5800bc40, 1, "e,f,g" },
{ "mult/sui",   0x5800f440, 1, "e,f,g" },
{ "mult/suic",  0x5800e440, 1, "e,f,g" },
{ "mult/suim",  0x5800ec40, 1, "e,f,g" },
{ "mult/suid",  0x5800fc40, 1, "e,f,g" },
{ "sqrts",      0x53e01160, 1, "f,g" },
{ "sqrts/c",    0x53e00160, 1, "f,g" },
{ "sqrts/m",    0x53e00960, 1, "f,g" },
{ "sqrts/d",    0x53e01960, 1, "f,g" },
{ "sqrts/u",    0x53e03160, 1, "f,g" },
{ "sqrts/uc",   0x53e02160, 1, "f,g" },
{ "sqrts/um",   0x53e02960, 1, "f,g" },
{ "sqrts/ud",   0x53e03960, 1, "f,g" },
{ "sqrts/su",   0x53e0b160, 1, "f,g" },
{ "sqrts/suc",  0x53e0a160, 1, "f,g" },
{ "sqrts/sum",  0x53e0a960, 1, "f,g" },
{ "sqrts/sud",  0x53e0b960, 1, "f,g" },
{ "sqrts/sui",  0x53e0f160, 1, "f,g" },
{ "sqrts/suic", 0x53e0e160, 1, "f,g" },
{ "sqrts/suim", 0x53e0e960, 1, "f,g" },
{ "sqrts/suid", 0x53e0f960, 1, "f,g" },
{ "sqrtt",      0x53e01560, 1, "f,g" },
{ "sqrtt/c",    0x53e00560, 1, "f,g" },
{ "sqrtt/m",    0x53e00d60, 1, "f,g" },
{ "sqrtt/d",    0x53e01d60, 1, "f,g" },
{ "sqrtt/u",    0x53e03560, 1, "f,g" },
{ "sqrtt/uc",   0x53e02560, 1, "f,g" },
{ "sqrtt/um",   0x53e02d60, 1, "f,g" },
{ "sqrtt/ud",   0x53e03d60, 1, "f,g" },
{ "sqrtt/su",   0x53e0b560, 1, "f,g" },
{ "sqrtt/suc",  0x53e0a560, 1, "f,g" },
{ "sqrtt/sum",  0x53e0ad60, 1, "f,g" },
{ "sqrtt/sud",  0x53e0bd60, 1, "f,g" },
{ "sqrtt/sui",  0x53e0f560, 1, "f,g" },
{ "sqrtt/suic", 0x53e0e560, 1, "f,g" },
{ "sqrtt/suim", 0x53e0ed60, 1, "f,g" },
{ "sqrtt/suid", 0x53e0fd60, 1, "f,g" },
{ "sqrtf",      0x53e01140, 1, "f,g" },
{ "sqrtf/c",    0x53e00140, 1, "f,g" },
{ "sqrtf/m",    0x53e00940, 1, "f,g" },
{ "sqrtf/d",    0x53e01940, 1, "f,g" },
{ "sqrtf/u",    0x53e03140, 1, "f,g" },
{ "sqrtf/uc",   0x53e02140, 1, "f,g" },
{ "sqrtf/um",   0x53e02940, 1, "f,g" },
{ "sqrtf/ud",   0x53e03940, 1, "f,g" },
{ "sqrtf/su",   0x53e0b140, 1, "f,g" },
{ "sqrtf/suc",  0x53e0a140, 1, "f,g" },
{ "sqrtf/sum",  0x53e0a940, 1, "f,g" },
{ "sqrtf/sud",  0x53e0b940, 1, "f,g" },
{ "sqrtf/sui",  0x53e0f140, 1, "f,g" },
{ "sqrtf/suic", 0x53e0e140, 1, "f,g" },
{ "sqrtf/suim", 0x53e0e940, 1, "f,g" },
{ "sqrtf/suid", 0x53e0f940, 1, "f,g" },
{ "sqrtg",      0x53e01540, 1, "f,g" },
{ "sqrtg/c",    0x53e00540, 1, "f,g" },
{ "sqrtg/m",    0x53e00d40, 1, "f,g" },
{ "sqrtg/d",    0x53e01d40, 1, "f,g" },
{ "sqrtg/u",    0x53e03540, 1, "f,g" },
{ "sqrtg/uc",   0x53e02540, 1, "f,g" },
{ "sqrtg/um",   0x53e02d40, 1, "f,g" },
{ "sqrtg/ud",   0x53e03d40, 1, "f,g" },
{ "sqrtg/su",   0x53e0b540, 1, "f,g" },
{ "sqrtg/suc",  0x53e0a540, 1, "f,g" },
{ "sqrtg/sum",  0x53e0ad40, 1, "f,g" },
{ "sqrtg/sud",  0x53e0bd40, 1, "f,g" },
{ "sqrtg/sui",  0x53e0f540, 1, "f,g" },
{ "sqrtg/suic", 0x53e0e540, 1, "f,g" },
{ "sqrtg/suim", 0x53e0ed40, 1, "f,g" },
{ "sqrtg/suid", 0x53e0fd40, 1, "f,g" },
{ "itoft",      0x501f0480, 1, "1,g" },
{ "itofs",      0x501f0080, 1, "1,g" },
{ "itoff",      0x501f0280, 1, "1,g" },
{ "ftoit",      0x701f0e00, 1, "e,3" },
{ "ftois",      0x701f0f00, 1, "e,3" },
{ "minub8",     0x70000740, 0, "1,2,3" },
{ "minub8",     0x70000740, 0, "1,b,3" },
{ "minsb8",     0x70000700, 0, "1,2,3" },
{ "minsb8",     0x70000700, 0, "1,b,3" },
{ "minuw4",     0x70000760, 0, "1,2,3" },
{ "minuw4",     0x70000760, 0, "1,b,3" },
{ "minsw4",     0x70000720, 0, "1,2,3" },
{ "minsw4",     0x70000720, 0, "1,b,3" },
{ "maxub8",     0x70000780, 0, "1,2,3" },
{ "maxub8",     0x70000780, 0, "1,b,3" },
{ "maxsb8",     0x700007c0, 0, "1,2,3" },
{ "maxsb8",     0x700007c0, 0, "1,b,3" },
{ "maxuw4",     0x700007a0, 0, "1,2,3" },
{ "maxuw4",     0x700007a0, 0, "1,b,3" },
{ "maxsw4",     0x700007e0, 0, "1,2,3" },
{ "maxsw4",     0x700007e0, 0, "1,b,3" },
{ "perr",     	0x70000620, 0, "1,2,3" },
{ "pklb",     	0x701f06e0, 0, "2,3" },
{ "pkwb",     	0x701f06c0, 0, "2,3" },
{ "unpklb",     0x701f06a0, 0, "2,3" },
{ "unpkwb",     0x701f0680, 0, "2,3" },

/*
 * Miscellaneous
 */
{ "pal",        0x00000000, 0, "I" },		/* 6o+26f */
{ "call_pal",   0x00000000, 0, "I" },		/* alias */
{ "bpt",        0x00000080, 0, "" },
{ "chmk",       0x00000083, 0, "" },
{ "imb",        0x00000086, 0, "" },

{ "draint",     0x60000000, 0, "" },		/* 6o+5a+5b+16d */
{ "trapb",      0x60000000, 0, "" },		/* 6o+5a+5b+16d */
{ "fetch",      0x60008000, 0, "0(2)" },
{ "fetch_m",    0x6000a000, 0, "0(2)" },
{ "mb",         0x60004000, 0, "" },
{ "wmb",        0x60004400, 0, "" },
{ "rpcc",       0x6000c000, 0, "1" },
{ "rc",         0x6000e000, 0, "1" },
{ "rs",         0x6000f000, 0, "1" },
/*
 * More macros
 */
{ "nop",         0x47ff041f, 0, "" },		/* or zero,zero,zero */
{ "mov",         0x47e00400, 0, "2,3" },		/* or zero,r2,r3 */
};

#define NUMOPCODES ((sizeof alpha_opcodes)/(sizeof alpha_opcodes[0]))

/*
 * 21064 (and 21066/68) specific PAL instructions.
 */
static struct alpha_opcode pal_21064_opcodes[] =
{

/*
 * PAL instructions
 */
{ "hw_ld",	0x6c000000, 0, "1,t(2)" },
{ "hw_ld/p",	0x6c008000, 0, "1,t(2)" },
{ "hw_ld/a",	0x6c004000, 0, "1,t(2)" },
{ "hw_ld/r",	0x6c002000, 0, "1,t(2)" },
{ "hw_ld/q",	0x6c001000, 0, "1,t(2)" },
{ "hw_ld/pa",	0x6c00C000, 0, "1,t(2)" },
{ "hw_ld/pr",	0x6c00A000, 0, "1,t(2)" },
{ "hw_ld/pq",	0x6c009000, 0, "1,t(2)" },
{ "hw_ld/ar",	0x6c006000, 0, "1,t(2)" },
{ "hw_ld/aq",	0x6c005000, 0, "1,t(2)" },
{ "hw_ld/rq",	0x6c003000, 0, "1,t(2)" },
{ "hw_ld/par",	0x6c00e000, 0, "1,t(2)" },
{ "hw_ld/paq",	0x6c00d000, 0, "1,t(2)" },
{ "hw_ld/prq",	0x6c00b000, 0, "1,t(2)" },
{ "hw_ld/arq",	0x6c007000, 0, "1,t(2)" },
{ "hw_ld/parq",	0x6c00f000, 0, "1,t(2)" },

{ "hw_ldq",	0x6c001000, 0, "1,t(2)" },		/* ldq/ldl variants for Eric */
{ "hw_ldq/p",	0x6c009000, 0, "1,t(2)" },
{ "hw_ldq/a",	0x6c005000, 0, "1,t(2)" },
{ "hw_ldq/r",	0x6c003000, 0, "1,t(2)" },
{ "hw_ldq/pa",	0x6c00d000, 0, "1,t(2)" },
{ "hw_ldq/pr",	0x6c00b000, 0, "1,t(2)" },
{ "hw_ldq/ar",	0x6c007000, 0, "1,t(2)" },
{ "hw_ldq/par",	0x6c00f000, 0, "1,t(2)" },
{ "hw_ldl",	0x6c000000, 0, "1,t(2)" },
{ "hw_ldl/p",	0x6c008000, 0, "1,t(2)" },
{ "hw_ldl/a",	0x6c004000, 0, "1,t(2)" },
{ "hw_ldl/r",	0x6c002000, 0, "1,t(2)" },
{ "hw_ldl/pa",	0x6c00C000, 0, "1,t(2)" },
{ "hw_ldl/pr",	0x6c00A000, 0, "1,t(2)" },
{ "hw_ldl/ar",	0x6c006000, 0, "1,t(2)" },
{ "hw_ldl/par",	0x6c00e000, 0, "1,t(2)" },

{ "hw_st/paq",	0x7c00d000, 0, "1,t(2)" },  /* Fixed by RRC */
{ "hw_st/pa",	0x7c00c000, 0, "1,t(2)" },  /* Fixed by RRC */
{ "hw_st/pq",	0x7c009000, 0, "1,t(2)" },
{ "hw_st/aq",	0x7c005000, 0, "1,t(2)" },
{ "hw_st/p",	0x7c008000, 0, "1,t(2)" },
{ "hw_st/a",	0x7c004000, 0, "1,t(2)" },
{ "hw_st/q",	0x7c001000, 0, "1,t(2)" },
{ "hw_st",	0x7c000000, 0, "1,t(2)" },

{ "hw_stq/pa",	0x7c00d000, 0, "1,t(2)" }, /* Fixed by RRC */		/* stq/stl variants for Eric */
{ "hw_stq/p",	0x7c009000, 0, "1,t(2)" },
{ "hw_stq",	0x7c001000, 0, "1,t(2)" },
{ "hw_stq/a",	0x7c005000, 0, "1,t(2)" },
{ "hw_stl/pa",	0x7c00c000, 0, "1,t(2)" }, /* Fixed by RRC */
{ "hw_stl/p",	0x7c008000, 0, "1,t(2)" },
{ "hw_stl/a",	0x7c004000, 0, "1,t(2)" },
{ "hw_stl",	0x7c000000, 0, "1,t(2)" },

{ "hw_mfpr/p",	0x64000080, 0, "R,3" },
{ "hw_mfpr/a",	0x64000040, 0, "R,3" },
{ "hw_mfpr/i",	0x64000020, 0, "R,3" },
{ "hw_mfpr/pa",	0x640000c0, 0, "R,3" },
{ "hw_mfpr/pi",	0x640000a0, 0, "R,3" },
{ "hw_mfpr/ai",	0x64000060, 0, "R,3" },
{ "hw_mfpr/pai",0x640000e0, 0, "R,3" },
{ "hw_mfpr",	0x64000000, 0, "R,8" },

{ "hw_mtpr/p", 	0x74000080, 0, "R,3" },
{ "hw_mtpr/a", 	0x74000040, 0, "R,3" },
{ "hw_mtpr/i", 	0x74000020, 0, "R,3" },
{ "hw_mtpr/pa", 0x740000c0, 0, "R,3" },
{ "hw_mtpr/pi", 0x740000a0, 0, "R,3" },
{ "hw_mtpr/ai", 0x74000060, 0, "R,3" },
{ "hw_mtpr/pai",0x740000e0, 0, "R,3" },
{ "hw_mtpr", 	0x74000000, 0, "R,8" },

{ "hw_rei",	0x7bff8000, 0, "" },
};

#define NUM21064OPCODES ((sizeof pal_21064_opcodes)/(sizeof pal_21064_opcodes[0]))

/*
 * 21164 specific PAL instructions.
 */
static struct alpha_opcode pal_21164_opcodes[] =
{
{ "hw_ld",	0x6c000000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ld/p",	0x6c008000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ld/a",	0x6c004000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ld/r",	0x6c002000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ld/q",	0x6c001000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ld/v",	0x6c000800, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ld/l",	0x6c000400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_ld/pa",	0x6c00C000, 0, "1,T(2)" },
{ "hw_ld/pr",	0x6c00A000, 0, "1,T(2)" },
{ "hw_ld/pq",	0x6c009000, 0, "1,T(2)" },
{ "hw_ld/pv",	0x6c008800, 0, "1,T(2)" },
{ "hw_ld/pl",	0x6c008400, 0, "1,T(2)" },

{ "hw_ld/ar",	0x6c006000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ld/aq",	0x6c005000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ld/av",	0x6c004800, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ld/al",	0x6c004400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_ld/rq",	0x6c003000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ld/rv",	0x6c002800, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ld/rl",	0x6c002400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_ld/qv",	0x6c001800, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ld/ql",	0x6c001400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_ld/vl",	0x6c000C00, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */


{ "hw_ld/par",	0x6c00E000, 0, "1,T(2)" },
{ "hw_ld/paq",	0x6c00D000, 0, "1,T(2)" },
{ "hw_ld/pav",	0x6c00C800, 0, "1,T(2)" },
{ "hw_ld/pal",	0x6c00C400, 0, "1,T(2)" },

{ "hw_ld/prq",	0x6c00B000, 0, "1,T(2)" },
{ "hw_ld/prv",	0x6c00A800, 0, "1,T(2)" },
{ "hw_ld/prl",	0x6c00A400, 0, "1,T(2)" },

{ "hw_ld/pqv",	0x6c009800, 0, "1,T(2)" },
{ "hw_ld/pql",	0x6c009400, 0, "1,T(2)" },

{ "hw_ld/pvl",	0x6c008C00, 0, "1,T(2)" },

{ "hw_ld/arq",	0x6c007000, 0, "1,T(2)" },
{ "hw_ld/arv",	0x6c006800, 0, "1,T(2)" },
{ "hw_ld/arl",	0x6c006400, 0, "1,T(2)" },

{ "hw_ld/rqv",	0x6c003800, 0, "1,T(2)" },
{ "hw_ld/rql",	0x6c003400, 0, "1,T(2)" },

{ "hw_ld/qvl",	0x6c001C00, 0, "1,T(2)" },

{ "hw_ld/parq",	0x6c00F000, 0, "1,T(2)" },
{ "hw_ld/parv",	0x6c00E800, 0, "1,T(2)" },
{ "hw_ld/parl",	0x6c00E400, 0, "1,T(2)" },

{ "hw_ld/arqv",	0x6c007800, 0, "1,T(2)" },
{ "hw_ld/arql",	0x6c007400, 0, "1,T(2)" },
{ "hw_ld/rqvl",	0x6c003C00, 0, "1,T(2)" },

{ "hw_ld/parqv", 0x6c00F800, 0, "1,T(2)" },
{ "hw_ld/parql", 0x6c00F400, 0, "1,T(2)" },

{ "hw_ld/arqvl", 0x6c007C00, 0, "1,T(2)" },

{ "hw_ld/parqvl", 0x6c00FC00, 0, "1,T(2)" },

/* HW_LDQ */
{ "hw_ldq",	0x6c001000, 0, "1,T(2)" },
{ "hw_ldq/p",	0x6c009000, 0, "1,T(2)" },
{ "hw_ldq/a",	0x6c005000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ldq/r",	0x6c003000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ldq/v",	0x6c001800, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ldq/l",	0x6c001400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ldq/pa",	0x6c00D000, 0, "1,T(2)" },
{ "hw_ldq/pr",	0x6c00B000, 0, "1,T(2)" },
{ "hw_ldq/pv",	0x6c009800, 0, "1,T(2)" },
{ "hw_ldq/pl",	0x6c009400, 0, "1,T(2)" },
{ "hw_ldq/par",	0x6c00F000, 0, "1,T(2)" },
{ "hw_ldq/parv", 0x6c00F800, 0, "1,T(2)" },
{ "hw_ldq/parl", 0x6c00F400, 0, "1,T(2)" },
{ "hw_ldq/parvl", 0x6c00FC00, 0, "1,T(2)" },

{ "hw_ldq/ar",	0x6c007000, 0, "1,T(2)" },
{ "hw_ldq/rv",	0x6c003800, 0, "1,T(2)" },
{ "hw_ldq/rl",	0x6c003400, 0, "1,T(2)" },
{ "hw_ldq/vl",	0x6c001C00, 0, "1,T(2)" },

{ "hw_ldq/arv",	0x6c007800, 0, "1,T(2)" },
{ "hw_ldq/arl",	0x6c007400, 0, "1,T(2)" },
{ "hw_ldq/rvl",	0x6c003C00, 0, "1,T(2)" },
{ "hw_ldq/arvl", 0x6c007C00, 0, "1,T(2)" },

/* HW_LDL */
{ "hw_ldl",	0x6c000000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ldl/p",	0x6c008000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ldl/a",	0x6c004000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ldl/r",	0x6c002000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ldl/v",	0x6c000800, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ldl/l",	0x6c000400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_ldl/pa",	0x6c00C000, 0, "1,T(2)" },
{ "hw_ldl/pr",	0x6c00A000, 0, "1,T(2)" },
{ "hw_ldl/pv",	0x6c008800, 0, "1,T(2)" },
{ "hw_ldl/pl",	0x6c008400, 0, "1,T(2)" },

{ "hw_ldl/ar",	0x6c006000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ldl/av",	0x6c004800, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ldl/al",	0x6c004400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_ldl/rv",	0x6c002800, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ldl/rl",	0x6c002400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_ldl/vl",	0x6c000C00, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_ldl/par",	0x6c00E000, 0, "1,T(2)" },
{ "hw_ldl/pav",	0x6c00C800, 0, "1,T(2)" },
{ "hw_ldl/pal",	0x6c00C400, 0, "1,T(2)" },

{ "hw_ldl/prv",	0x6c00A800, 0, "1,T(2)" },
{ "hw_ldl/prl",	0x6c00A400, 0, "1,T(2)" },

{ "hw_ldl/pvl",	0x6c008C00, 0, "1,T(2)" },


{ "hw_ldl/arv",	0x6c006800, 0, "1,T(2)" },
{ "hw_ldl/arl",	0x6c006400, 0, "1,T(2)" },


{ "hw_ldl/parv",  0x6c00E800, 0, "1,T(2)" },
{ "hw_ldl/parl",  0x6c00E400, 0, "1,T(2)" },


/* HW_ST */
{ "hw_st",	0x7c000000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_st/p",	0x7c008000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_st/a",	0x7c004000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_st/q",	0x7c001000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_st/c",	0x7c000400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_st/pa",	0x7c00C000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_st/pq",	0x7c009000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_st/pc",	0x7c008400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_st/aq",	0x7c005000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_st/ac",	0x7c004400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_st/qc",	0x7c001400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_st/paq",	0x7c00D000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_st/pac",	0x7c00C400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_st/pqc",	0x7c009400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_st/aqc",	0x7c005400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */


{ "hw_st/paqc",	0x7c00D400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

/* HW_STL */
{ "hw_stl",	0x7c000000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_stl/p",	0x7c008000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_stl/a",	0x7c004000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_stl/c",	0x7c000400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_stl/pa",	0x7c00C000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_stl/pc",	0x7c008400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_stl/ac",	0x7c004400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_stl/pac",	0x7c00C400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

/* HW_STQ */
{ "hw_stq",	0x7c001000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_stq/p",	0x7c009000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_stq/a",	0x7c005000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_stq/c",	0x7c001400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_stq/pa",	0x7c00D000, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_stq/pc",	0x7c009400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */
{ "hw_stq/ac",	0x7c005400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */

{ "hw_stq/pac",	0x7c00D400, 0, "1,T(2)" },      /* RA, 10 bit displacement (RB) */


{ "hw_mfpr",	0x64000000, 0, "R,l" },         /* RA,RB,16 bits displacement */
{ "hw_mtpr", 	0x74000000, 0, "R,l" },         /* RA,RB,16 bits displacement */

{ "hw_rei",	0x7bff8000, 0, "" },

{ "hw_rei_stall",	0x7bffc000, 0, "" },
};

#define NUM21164OPCODES ((sizeof pal_21164_opcodes)/(sizeof pal_21164_opcodes[0]))

/*
 * 21264 specific PAL instructions.
 */
static struct alpha_opcode pal_21264_opcodes[] =
{
/* HW_LD */
/* there are 14 flavors of HW_LD: 7 longword and 7 quadword */

{ "hw_ldl/p",	0x6c000000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_ldl/pl",	0x6c002000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_ldl/vv",	0x6c004000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_ldl/v",	0x6c008000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_ldl/vw",	0x6c00A000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_ldl/a",	0x6c00C000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_ldl/vwa",	0x6c00E000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */

{ "hw_ldq/p",	0x6c001000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_ldq/pl",	0x6c003000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_ldq/vv",	0x6c005000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_ldq/v",	0x6c009000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_ldq/vw",	0x6c00B000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_ldq/a",	0x6c00D000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_ldq/vwa",	0x6c00F000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */


/* HW_ST */
/* there are 8 flavors of HW_LD: 4 longword and 4 quadword */

{ "hw_stl/p",	0x7c000000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_stl/pc",	0x7c002000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_stl/v",	0x7c004000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_stl/va",	0x7c00C000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */

{ "hw_stq/p",	0x7c001000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_stq/pc",	0x7c003000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_stq/v",	0x7c005000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */
{ "hw_stq/va",	0x7c00D000, 0, "1,t(2)" },      /* RA, 12 bit displacement (RB) */



/* HW_RET */
/* there are 8 flavors of HW_RET : 4 without stall , 4 with stall set*/
/* must choose r31 as ra */

 { "hw_ret/jmp",	0x7BE00000, 0, "(2)" },      /*  */
 { "hw_ret/jsr",	0x7BE04000, 0, "(2)" },      /*  */
 { "hw_ret/ret",	0x7BE08000, 0, "(2)" },      /*  */
 { "hw_ret/co" ,        0x7BE0C000, 0, "(2)" },      /*  */
					 	      
 { "hw_rets/jmp",	0x7BE02000, 0, "(2)" },      /*  */
 { "hw_rets/jsr",	0x7BE06000, 0, "(2)" },      /*  */
 { "hw_rets/ret",	0x7BE0A000, 0, "(2)" },      /*  */
 { "hw_rets/co" ,	0x7BE0E000, 0, "(2)" },      /*  */



/* MF and MT */
{ "hw_mfpr",	0x641F0000, 0, "R,l" },         /* RA,RB,16 bits displacement */
{ "hw_mtpr", 	0x77E00000, 0, "R,l" },         /* RA,RB,16 bits displacement */

{ "whint",       0x63E0F800, 0, "2" },
{ "ecb",         0x63E0E800, 0, "2" }
};

#define NUM21264OPCODES ((sizeof pal_21264_opcodes)/(sizeof pal_21264_opcodes[0]))
