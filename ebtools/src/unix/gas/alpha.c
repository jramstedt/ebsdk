/* alpha.c -- Assemble for the ALPHA
   Copyright (C) 1989 Free Software Foundation, Inc.

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


#if __osf__
#include <mach/boolean.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <stab.h>

#include "alpha-opcode.h"
#include "as.h"
#include "frags.h"
#include "struc-symbol.h"
#include "flonum.h"
#include "expr.h"
#include "hash.h"
#include "md.h"
#include "write.h"
#include "read.h"
#include "symbols.h"

#include "../../common/include/stdc.h"

void md_begin();
void md_end();
void md_number_to_chars();
void md_assemble();
char *md_atof();
void md_convert_frag();
void md_create_short_jump();
void md_create_long_jump();
int md_estimate_size_before_relax();
void md_number_to_imm();
void md_number_to_disp();
void md_number_to_field();
void md_ri_to_chars();
void emit_relocations();
static int alpha_ip();

relax_typeS md_relax_table[] = {0};

void s_load_PALcode_Instructions();

/* handle of the OPCODE hash table */
static struct hash_control *op_hash = NULL;

static void s_common(), s_skip(), s_frame(), s_base();
extern void s_globl(), s_long(), s_short(), s_space(), cons(), s_proc(), 
  s_text(), s_data(), float_cons();
extern void list_add(unsigned long opcode);

pseudo_typeS md_pseudo_table[] = {
    {"common", s_common, 0}, 
    {"rdata", s_text, 0}, 
    {"sdata", s_data, 0}, 
    {"gprel32", cons, 4}, 		/* XXX fixme */
    {"global", s_globl, 0}, 
    {"t_floating", float_cons, 'd'}, 
    {"s_floating", float_cons, 'f'}, 
    {"f_floating", float_cons, 'F'}, 
    {"g_floating", float_cons, 'G'}, 
    {"d_floating", float_cons, 'D'}, 

    {"proc", s_proc, 0}, 
    {"aproc", s_proc, 1}, 
    {"frame", s_frame, 0}, 
    {"reguse", s_skip, 0}, 
    {"livereg", s_skip, 0}, 
    {"mask", s_skip, 0}, 
    {"fmask", s_skip, 0}, 
    {"end", s_skip, 0}, 
    {"extern", s_skip, 0}, 		/*??*/
    {"base", s_base, 0}, 		/*??*/
#if __osf__
      {"option", s_skip, 0}, {"prologue", s_skip, 0}, 
#endif

      {"verstamp", s_skip, 0}, {"ent", s_skip, 0}, {"aent", s_skip, 0}, 
      {"loc", s_skip, 0}, 

      {NULL, 0, 0},};

#define SA	21			/* shift for register Ra */
#define SB	16			/* shift for register Ra */
#define SC	0			/* shift for register Ra */
#define SN	13			/* shift for 8 bit immediate # */

#define T9	23
#define T10	24
#define T11	25
#define RA	26
#define PV	27
#define AT	28
#define GP	29
#define SP	30
#define ZERO	31

#ifndef FIRST_32BIT_QUADRANT
#define FIRST_32BIT_QUADRANT FALSE
#endif

#if FIRST_32BIT_QUADRANT
    boolean_t first_32bit_quadrant = TRUE;
    int base_register = ZERO;		/* GP if !first_32bit_quadrant */
#else
    boolean_t first_32bit_quadrant = FALSE;
    int base_register = GP;		/* GP if !first_32bit_quadrant */
#endif
    boolean_t no_mixed_code = FALSE;
    boolean_t use_large_offsets = FALSE;
    boolean_t nofloats = FALSE;

    integer_t omagic;			/* Magic number for header */

    int md_short_jump_size = 4;
    int md_long_jump_size = 4;

/* This array holds the chars that always start a comment.  If the
    pre-processor is disabled, these aren't very useful */
    char comment_chars[] = "#";		/* JF removed '|' from comment_chars */

/* This array holds the chars that only start a comment at the beginning of
   a line.  If the line seems to have the form '# 123 filename'
   .line and .file directives will appear in the pre-processed output */
/* Note that input_file.c hand checks for '#' at the beginning of the
   first line of the input file.  This is because the compiler outputs
   #NO_APP at the beginning of its output. */
/* Also note that '/*' will always start a comment */
    char line_comment_chars[] = "#";

/* Chars that can be used to separate mant from exp in floating point nums */
    char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant */
/* As in 0f12.456 */
/* or    0d1.2345e12 */
    char FLT_CHARS[] = "rRsSfFdDxXpP";

/* Also be aware that MAXIMUM_NUMBER_OF_CHARS_FOR_FLOAT may have to be
   changed in read.c .  Ideally it shouldn't have to know about it at all,
   but nothing is ideal around here.
 */
    int size_reloc_info = sizeof(struct relocation_info);

    static unsigned char octal[256];
#define isoctal(c)  octal[c]
    static unsigned char toHex[256];

    struct alpha_it {
	char *error;
	unsigned long opcode;
	struct nlist *nlistp;
	expressionS exp;
	int pcrel;
	enum reloc_type reloc;
    };

#ifdef __STDC__
    static void print_insn(struct alpha_it *insn);
    static int getExpression(char *str, struct alpha_it *insn);
#else
    static void print_insn();
    static int getExpression();
#endif
    static char *expr_end;

/*
 *  Which particular implementation of the alpha architecture are we talking
 *  about here?
 */
#define _type_21064       0
#define _type_21164       1
#define _type_21264       2
#define _type_21066       _type_21064
#define _type_21068       _type_21064

static int machine_type = _type_21064;
static int PALcodeInstructionsLoaded = FALSE;


    static void s_common()
{
    register char *name;
    register char c;
    register char *p;
    register int temp;
    register symbolS *symbolP;

    name = input_line_pointer;
    c = get_symbol_end();

    /* just after name is now '\0' */
    p = input_line_pointer;
    *p = c;
    SKIP_WHITESPACE();
    if (*input_line_pointer != ',') {
	as_warn("Expected comma after symbol-name");
	ignore_rest_of_line();
	return;
    }
    input_line_pointer++;		/* skip ',' */
    if ((temp = get_absolute_expression()) < 0) {
	as_warn(".COMMon length (%d.) <0! Ignored.", temp);
	ignore_rest_of_line();
	return;
    }
    *p = 0;
    symbolP = symbol_find_or_make(name);
    *p = c;
    if ((symbolP->sy_type & N_TYPE) != N_UNDF || symbolP->sy_other != 0 || 
      symbolP->sy_desc != 0) {
	as_warn("Ignoring attempt to re-define symbol");
	ignore_rest_of_line();
	return;
    }
    if (neq64(symbolP->sy_value, zero)) {
	if (low32(symbolP->sy_value) != temp) {
	    as_warn("Length of .comm \"%s\" is already %d. Not changed to %d.", 
	      symbolP->sy_name, low32(symbolP->sy_value), temp);
	}
    } else {
	symbolP->sy_value = plus_a_32(zero, temp);
	symbolP->sy_type |= N_EXT;
    }
    know(symbolP->sy_frag == &zero_address_frag);
    if (strncmp(input_line_pointer, ",\"bss\"", 6) != 0) {
	p = input_line_pointer;
	while (*p && *p != '\n')
	    p++;
	c = *p;
	*p = '\0';
	as_warn("bad .common segment: `%s'", input_line_pointer);
	*p = c;
	return;
    }
    input_line_pointer += 6;
    demand_empty_rest_of_line();
    return;
}

/* placeholders, compat, unimplemented features */
static void s_skip()
{
    extern char is_end_of_line[];

    while (!is_end_of_line[*input_line_pointer]) {
	++input_line_pointer;
    }
    ++input_line_pointer;
    return;
}

/* ".frame framereg, framesize, pc_register" */
static void s_frame()
{
    register char *name;
    register char c;
    register char *p;
    register symbolS *symbolP;
    register int fs, framereg, pcreg, im, fm;

    fs = im = fm = 0;
    pcreg = 26;

#ifdef CROSS_COMPILE_32_TO_64_BITS
#define encode_regmask(v, i,f)	{ (v).low = (f); (v).high = (i); }
#else
#define encode_regmask(v,i,f)	{ (v) = (((i)<<32)|(f)); }
#endif

    name = symbol_lastP->sy_name;
    symbolP = symbol_new(name, N_FRAME, 0, 0, 0, &zero_address_frag);

    framereg = SP;
    if (*input_line_pointer == '$') {
	input_line_pointer++;
	if ((input_line_pointer[0] == 's') && (input_line_pointer[1] == 'p')) {
	    framereg = SP;
	    input_line_pointer += 2;
	} else {
	    framereg = get_absolute_expression();
	}
	framereg &= 31;			/*zonk*/
    } else
	as_warn("framereg expected, using $%d.", framereg);

    while ((*input_line_pointer == ',') || (*input_line_pointer == ' '))
	input_line_pointer++;

    fs = get_absolute_expression();

    while ((*input_line_pointer == ',') || (*input_line_pointer == ' '))
	input_line_pointer++;

    if (*input_line_pointer == '$')
	input_line_pointer++;
    pcreg = get_absolute_expression();
    if (pcreg < 0 || pcreg > 30) {
	pcreg = 26;
	as_warn("Bad pcreg, using $26.");
    }

    if (*input_line_pointer == ',') {
	input_line_pointer++;
	im = get_absolute_expression();
    }
    if (*input_line_pointer == ',') {
	input_line_pointer++;
	fm = get_absolute_expression();
    }
complete: 
    symbolP->sy_other = pcreg;
    symbolP->sy_desc = fs;
    encode_regmask(symbolP->sy_value, im, fm);
    demand_empty_rest_of_line();
}

static void s_base()
{
    if (first_32bit_quadrant) {		/* not fatal, but it might not work in 
					 * the end */
	as_warn("File overrides no-base-register option.");
	first_32bit_quadrant = FALSE;
    }

    SKIP_WHITESPACE();
    if (*input_line_pointer == '$') {	/* $rNN form */
	input_line_pointer++;
	if (*input_line_pointer == 'r')
	    input_line_pointer++;
    }

    base_register = get_absolute_expression();
    if (base_register < 0 || base_register > 31) {
	base_register = GP;
	as_warn("Bad base register, using $r.", base_register);
    }
    demand_empty_rest_of_line();
}


/* This function is called once, at assembler startup time.  It should
   set up all the tables, etc. that the MD part of the assembler will need.  */
void md_begin()
{
    register char *retval = NULL;
    int lose = 0;
    register unsigned int i = 0;

    omagic = plus_a_32(zero, OMAGIC);	/* Magic number for header */

    op_hash = hash_new();
    if (op_hash == NULL)
	as_fatal("Virtual memory exhausted");
/*
 *  Add in the standard opcodes.
 */
    while (i < NUMOPCODES) {
	const char *name = alpha_opcodes[i].name;
	retval = hash_insert(op_hash, name, &alpha_opcodes[i]);
	if (retval != NULL && *retval != '\0') {
	    fprintf(stderr, "internal error: can't hash `%s': %s\n", 
	      alpha_opcodes[i].name, retval);
	    lose = 1;
	}
	do {
	    ++i;
	} while (i < NUMOPCODES && !strcmp(alpha_opcodes[i].name, name));
    }



    if (lose)
	as_fatal("Broken assembler.  No assembly attempted.");

    for (i = '0'; i < '8'; ++i)
	octal[i] = 1;
    for (i = '0'; i <= '9'; ++i)
	toHex[i] = i - '0';
    for (i = 'a'; i <= 'f'; ++i)
	toHex[i] = i + 10 - 'a';
    for (i = 'A'; i <= 'F'; ++i)
	toHex[i] = i + 10 - 'A';

}

void md_end()
{
    return;
}

int optnum = 1;

void md_assemble(str)
    char *str;
{
    char *toP;
    int i, count;

#define MAX_INSNS	5
    struct alpha_it insns[MAX_INSNS];

/*
 *  If we haven't loaded the PALcode instructions
 *  for this architecture then do so.
 */
    if (!PALcodeInstructionsLoaded)
      s_load_PALcode_Instructions();

    assert(str);
    count = alpha_ip(str, insns);
    if (count <= 0)
	return;

    for (i = 0; i < count; i++) {

	toP = frag_more(4);

	/* put out the opcode */
	md_number_to_chars(toP, insns[i].opcode, 4);

	list_add(insns[i].opcode);

	/* put out the symbol-dependent stuff */
	if (insns[i].reloc != NO_RELOC)
	    fix_new(frag_now, 		/* which frag */
	      (toP - frag_now->fr_literal), 
					/* where */
	      4, 			/* size */
	      insns[i].exp.X_add_symbol, insns[i].exp.X_subtract_symbol, 
	      insns[i].exp.X_add_number, insns[0].pcrel, insns[i].reloc);
    }
}

static int alpha_ip(str, insns)
    char *str;
    struct alpha_it insns[];
{
    char *s;

    const char *args;
    char c;
    unsigned long i;
    struct alpha_opcode *pattern;
    char *argsStart;
    unsigned int opcode;
    unsigned int mask;
    int match = FALSE, num_gen = 1;
    int comma = 0;


    for (s = str;
      islower(*s) || *s == '_' || *s == '/' || *s == '4' || *s == '8'; ++s)
	;
    switch (*s) {

	case '\0': 
	    break;

	case ',': 
	    comma = 1;

	/*FALLTHROUGH*/

	case ' ': 
	    *s++ = '\0';
	    break;

	default: 
	    as_warn("Unknown opcode: `%s'", str);
	    exit(1);
    }
    if ((pattern = (struct alpha_opcode *) hash_find(op_hash, str)) == NULL) {
	as_warn("Unknown opcode: `%s'", str);
	return -1;
    }
    if (comma) {
	*--s = ',';
    }
    argsStart = s;
    for (;;) {
	opcode = pattern->match;
	num_gen = 1;
	bzero(insns, sizeof(*insns));
	insns[0].reloc = NO_RELOC;

	/* Build the opcode, checking as we go to make sure that the operands
	 * match */
	for (args = pattern->args;; ++args) {
	    switch (*args) {

		case '\0': 		/* end of args */
		    if (*s == '\0') {
			match = TRUE;
		    }
		    break;

		case '+': 
		    if (*s == '+') {
			++s;
			continue;
		    }
		    if (*s == '-') {
			continue;
		    }
		    break;

		case '(': 		/* these must match exactly */
		case ')': 
		case ',': 
		case ' ': 
		case '0': 
		    if (*s++ == *args)
			continue;
		    break;

		case '1': 		/* next operand must be a register */
		case '2': 
		case '3': 
		case 'r': 
		case 'R': 
		    if (*s++ == '$') {
			switch (c = *s++) {

			    case 'a': 	/* $at: as temporary */
				if (*s++ != 't')
				    goto error;
				mask = AT;
				break;

			    case 'g': 	/* $gp: base register */
				if (*s++ != 'p')
				    goto error;
				mask = base_register;
				break;

			    case 's': 	/* $sp: stack pointer */
				if (*s++ != 'p')
				    goto error;
				mask = SP;
				break;


			    case 'r': 	/* any register */
				if (!isdigit(c = *s++)) {
				    goto error;
				}

			    /* FALLTHROUGH */
			    case '0': 
			    case '1': 
			    case '2': 
			    case '3': 
			    case '4': 
			    case '5': 
			    case '6': 
			    case '7': 
			    case '8': 
			    case '9': 
				if (isdigit(*s)) {
				    if ((c = 10 * (c - '0') + (*s++ - '0')) >= 
				      32) {
					goto error;
				    }
				} else {
				    c -= '0';
				}
				if ((c == GP) && first_32bit_quadrant)
				    c = ZERO;

				mask = c;
				break;

			    default: 
				goto error;
			}

			/* Got the register, now figure out where it goes in
			 * the opcode. */
doregister: 		switch (*args) {

			    case '1': 
			    case 'e': 
				opcode |= mask << SA;
				continue;

			    case '2': 
			    case 'f': 
				opcode |= mask << SB;
				continue;

			    case '3': 
			    case 'g': 
				opcode |= mask;
				continue;

			    case 'r': 
				opcode |= (mask << SA) | mask;
				continue;

			    case 'R': 	/* ra and rb are the same */
				opcode |= (mask << SA) | (mask << SB);
				continue;

			    case 'E': 
				opcode |= (mask << SA) | (mask << SB) | (mask);
				continue;
			}
		    }
		    break;

		case 'e': 		/* next operand is a floating point
					 * register */
		case 'f': 
		case 'g': 
		case 'E': 
		    if (*s++ == '$' && *s++ == 'f' && isdigit(*s)) {
			mask = *s++;
			if (isdigit(*s)) {
			    mask = 10 * (mask - '0') + (*s++ - '0');
			    if (mask >= 32) {
				break;
			    }
			} else {
			    mask -= '0';
			}

			/* same encoding as gp registers */
			goto doregister;
		    }
		    break;

		case 'h': 		/* bits 16..31 */
		    insns[0].reloc = RELOC_16_31;
		    goto immediate;

		case 'l': 		/* bits 0..15 */
		    insns[0].reloc = RELOC_0_15;
		    goto immediate;

		case 'L': 		/* 21 bit PC relative immediate */
		    insns[0].reloc = RELOC_WDISP21;
		    insns[0].pcrel = 1;
		    goto immediate;

		case 'i': 		/* 14 bit immediate */
		    insns[0].reloc = RELOC_WDISP14;
		    insns[0].pcrel = 1;
		    goto immediate;

		case 'b': 		/* 8 bit immediate */
		    insns[0].reloc = RELOC_0_7;
		    goto immediate;

		case 't': 		/* 12 bit 0...11 */
		    insns[0].reloc = RELOC_0_12;
		    goto immediate;

		case 'T': 		/* 10 bit 0...9 */
		    insns[0].reloc = RELOC_0_10;
		    goto immediate;

		case '8': 		/* 8 bit 0...7 */
		    insns[0].reloc = RELOC_0_8;
		    goto immediate;

		case 'I': 		/* 26 bit immediate */
		    insns[0].reloc = RELOC_0_25;

		    /*FALLTHROUGH*/

immediate: 	    if (*s == ' ')
			s++;
		    (void) getExpression(s, &insns[0]);
		    s = expr_end;
		    continue;

		/* The following two.. take advantage of the fact that opcode
		 * already contains most of what we need to know.  We just
		 * prepend to the instr an "ldah $r,%ml(expr)($base)" and turn 
		 * this one (done later after we return) into something like
		 * "stq $r,%lo(expr)(at)" or "ldq $r,%lo(expr)($r)". NOTE: This
		 * can fail later on at link time if the offset from $base
		 * actually turns out to be more than 2**31 or 2**47 if
		 * use_large_offsets is set. */
		case 'P': 		/* Addressing macros: PUT */
		    mask = AT;		/* register 'at' */

		/* fall through */

		case 'G': 		/* Addressing macros: GET */
get_macro: 

		    /* all it is missing is the expression, which is what we
		     * will get now */

		    if (*s == ' ')
			s++;
		    (void) getExpression(s, &insns[0]);
		    s = expr_end;

		    /* Must check for "lda ..,number" too */
		    if (insns[0].exp.X_seg == SEG_BIG) {
			as_warn("Sorry, not yet. Put bignums in .data section.");
			return -1;
		    }
		    if (insns[0].exp.X_seg == SEG_ABSOLUTE) {

			/* This only handles 32bit numbers */
			register int val = insns[0].exp.X_add_number;
			register short sval;

			insns[0].reloc = NO_RELOC;
			insns[1].reloc = NO_RELOC;

			sval = val;
			if (0)
			    fprintf(stderr, "val %lx sval %lx\n", val, sval);
			if ((sval != val) && (val & 0x8000)) {
			    val += 0x10000;
			    sval = val;
			}

			if (optnum && (sval == val)) {

			    /* optimize away the ldah */
			    num_gen = 1;
			    opcode |= (ZERO << SB) | (val & 0xffff);
			} else {
			    num_gen = 2;
			    insns[1].opcode = 
			      opcode | (mask << SB) | (val & 0xffff);
			    opcode = 0x24000000

			      /*ldah*/
			      |mask << SA | (ZERO << SB) | 
			      ((val >> 16) & 0xffff);
			}

		    } else {

			/* Not a number */
			if (use_large_offsets) {
			    num_gen = 5;
			    insns[4].exp = insns[3].exp = insns[2].exp = 
			      insns[1].exp = insns[0].exp;

			    /* Generate:      ldah    at,x2(zero)     insn[0]  
			     *    ldah    REG,x1(GP)      insn[1]      sll     
			     * at,16,at        insn[2]      addq    REG,at,REG 
			     *     insn[3]      OP      ?,x0(REG)       insn[4]
			     */


			    insns[4].reloc = RELOC_0_15;
			    insns[4].opcode = opcode | mask << SB;

			    insns[3].reloc = NO_RELOC;
			    insns[3].opcode = 0x40000400

			      /*addq*/
			      |mask << SA | AT << SB | mask;

			    insns[2].reloc = NO_RELOC;
			    insns[2].opcode = 0x48001720

			      /*sll ?,#,?*/
			      |16 << SN | AT << SA | AT;

			    insns[1].reloc = RELOC_16_31;
			    insns[1].opcode = 0x24000000

			      /*ldah*/
			      |mask << SA | (base_register << SB);

			    insns[0].reloc = RELOC_32_47;
			    opcode = 0x24000000

			      /*ldah*/
			      |AT << SA | ZERO << SB;

			} else {
			    num_gen = 2;
			    insns[1].exp = insns[0].exp;

			    /* Generate: ldah REG,x1(GP); OP ?,x0(REG) */

			    insns[1].reloc = RELOC_0_15;
			    insns[1].opcode = opcode | mask << SB;

			    insns[0].reloc = RELOC_16_31;
			    opcode = 0x24000000

			      /*ldah*/
			      |mask << SA | (base_register << SB);
			}
		    }

		    continue;

		/* Same failure modes as above, actually most of the same code 
		 * shared. */
		case 'B': 		/* Builtins */
		    args++;
		    switch (*args) {

			case 'a': 	/* ldgp */

			    if (first_32bit_quadrant || no_mixed_code)
				return -1;
			    insns[0].reloc = NO_RELOC;

			    /* generate "zap %r,0xf,%r" to take high 32 bits */
			    opcode |= 0x48001600

			      /* zap ?,#,?*/
			      |(0xf << SN);
			    continue;


			case 'b': 	/* setgp */

			    /* generate "zap %r,0xf,$gp" to take high 32 bits
			     */
			    opcode |= 0x48001600

			      /* zap ?,#,?*/
			      |(0xf << SN) | (base_register);
			    continue;

			case 'c': 	/* jsr $r,foo  becomes      lda $27,foo
					 *      jsr $r,($27),foo */
			    {
				char expansion[128], *func;
				int len;

				insns[0].reloc = RELOC_WDISP14;
				insns[0].pcrel = 1;
				opcode |= PV << SB;

				/* We still have to parse the function name */
				if (*s == ' ')
				    s++;
				func = s;
				(void) getExpression(s, &insns[0]);
				s = expr_end;

				/* Do the expansion */
				sprintf(expansion, "lda $%d,", PV);
				len = strlen(expansion);
				bcopy(func, &expansion[len], s - func);
				len += s - func;
				expansion[len] = 0;

				/* Assemble this extra (a macro!) now */
				md_assemble(expansion);
			    }
			    continue;

			/* DIVISION and MODULUS. Yech */
			case '0': 	/* reml */
			case '1': 	/* divl */
			case '2': 	/* remq */
			case '3': 	/* divq */
			case '4': 	/* remlu */
			case '5': 	/* divlu */
			case '6': 	/* remqu */
			case '7': 	/* divqu */
			    {
				static char func[8][6] = {"reml", "divl", 
				  "remq", "divq", "remlu", "divlu", "remqu", 
				  "divqu"};
				char expansion[64];
				int reg;

				/* All regs parsed, in opcode */

				/* Do the expansions, one instr at a time */

				if ((reg = (opcode >> SA) & 31) != T10) {
				    sprintf(expansion, "mov $%d,$%d", reg, 
				      T10);

				    /* x->t10 */
				    md_assemble(expansion);
				}
				if ((reg = (opcode >> SB) & 31) != T11) {
				    sprintf(expansion, "mov $%d,$%d", reg, 
				      T11);

				    /* y->t11 */
				    md_assemble(expansion);
				}
				sprintf(expansion, "lda $%d,__%s", PV, 
				  func[*args - '0']);
				md_assemble(expansion);
				sprintf(expansion, "jsr $%d,($%d),__%s", T9, 
				  PV, func[*args - '0']);
				md_assemble(expansion);
				if (!first_32bit_quadrant) {
				    sprintf(expansion, "zap $%d,0xf,$%d", T9, 
				      base_register);
				    md_assemble(expansion);
				}

				/* Use insns[0] to get at the result */
				if ((reg = (opcode & 31)) != PV) {
				    opcode = 0x47e00400

				      /* or zero,zero,zero */
				      |(PV << SB) | reg

				      /* Rc */
				      ;	/* pv->z */
				} else
				    num_gen = 0;
			    }
			    continue;
		    }

		/* fall through */
		default: 
		    abort();
	    }
	    break;
	}
error: 	if (match == FALSE) {

	    /* Args don't match.  */
	    if (&pattern[1] - alpha_opcodes < NUMOPCODES && 
	      !strcmp(pattern->name, pattern[1].name)) {
		++pattern;
		s = argsStart;
		continue;
	    } else {
		as_warn("Illegal operands");
		return -1;
	    }
	} else {

	    /* Args match, see if a float instructions and -nofloats */
	    if (nofloats && pattern->isa_float)
		return -1;
	}
	break;
    }

    insns[0].opcode = opcode;
    return num_gen;
}

static int getExpression(str, this_insn)
    char *str;
    struct alpha_it *this_insn;
{
    char *save_in;
    segT seg;
    static struct am {
	char *name;
	enum reloc_type reloc;
    } macro[] = {
	{"hi", RELOC_48_63}, 
	{"lo", RELOC_0_15}, 
	{"ml", RELOC_16_31}, 
	{"mh", RELOC_32_47}, 
	{"uhi", RELOC_U_48_63}, 
	{"uml", RELOC_U_16_31}, 
	{"umh", RELOC_U_32_47}, 
	{0,}
    };

    /* Handle macros: "%macroname(expr)" */
    if (*str == '%') {
	struct am *m;
	char *p, *q;

	str++;
	m = &macro[0];
	while (q = m->name) {
	    p = str;
	    while (*q && *p == *q)
		p++, q++;
	    if (*q == 0)
		break;
	    m++;
	}
	if (q) {
	    str = p;			/* keep the '(' */
	    this_insn->reloc = m->reloc;
	}
    }

    save_in = input_line_pointer;
    input_line_pointer = str;

    switch (seg = expression(&this_insn->exp)) {

	case SEG_ABSOLUTE: 
	case SEG_TEXT: 
	case SEG_DATA: 
	case SEG_BSS: 
	case SEG_UNKNOWN: 
	case SEG_DIFFERENCE: 
	case SEG_BIG: 
	case SEG_NONE: 
	    break;

	default: 
	    this_insn->error = "bad segment";
	    expr_end = input_line_pointer;
	    input_line_pointer = save_in;
	    return 1;
    }
    expr_end = input_line_pointer;
    input_line_pointer = save_in;
    return 0;
}


/*
   Turn a string in input_line_pointer into a floating point constant of type
   type, and store the appropriate bytes in *litP.  The number of LITTLENUMS
   emitted is stored in *sizeP .  An error message is returned, or NULL on OK.
 */

/* Equal to MAX_PRECISION in atof-ieee.c */
#define MAX_LITTLENUMS 6

char *md_atof(type, litP, sizeP)
    char type;
    char *litP;
    int *sizeP;
{
    int prec;
    LITTLENUM_TYPE words[MAX_LITTLENUMS];
    LITTLENUM_TYPE *wordP;
    char *t;
    char *atof_ieee(), *vax_md_atof();

    switch (type) {

	/* VAX floats */
	case 'F': 
	case 'D': 
	case 'G': 
	    return vax_md_atof(type, litP, sizeP);

	/* IEEE floats */
	case 'f': 
	    prec = 2;
	    break;

	case 'd': 
	    prec = 4;
	    break;

	case 'x': 
	case 'X': 
	    prec = 6;
	    break;

	case 'p': 
	case 'P': 
	    prec = 6;
	    break;

	default: 
	    *sizeP = 0;
	    return "Bad call to MD_ATOF()";
    }
    t = atof_ieee(input_line_pointer, type, words);
    if (t)
	input_line_pointer = t;
    *sizeP = prec * sizeof(LITTLENUM_TYPE);

    /* */
    for (wordP = words + prec - 1; prec--;) {
	md_number_to_chars(litP, (long) (*wordP--), sizeof(LITTLENUM_TYPE));
	litP += sizeof(LITTLENUM_TYPE);
    }

    return "";				/* Someone should teach Dean about null
					 * pointers */
}

/*
 * Write out little-endian.
 */
void md_number_to_chars(buf, val, n)
    char *buf;
    long val;
    int n;
{
    while (n--) {
	*buf++ = val & ((1 << BITS_PER_CHAR) - 1);
	val >>= BITS_PER_CHAR;
    }
}

void md_bignum_to_chars(buf, bignum, n)
    char *buf;
    LITTLENUM_TYPE *bignum;
    int n;
{
    while (n) {
	LITTLENUM_TYPE work = *bignum++;
	int nb = CHARS_PER_LITTLENUM;

	do {
	    *buf++ = work & ((1 << BITS_PER_CHAR) - 1);
	    if (n-- == 0)
		return;
	    work >>= BITS_PER_CHAR;
	} while (--nb);
    }
}

int logorrea = 0;


long computed_br_value;
extern struct exec the_exec;

void md_number_to_imm(buf, val, n, fixP, seg_type)
    char *buf;
    long val;
    int n;
    fixS *fixP;
    int seg_type;
{
    vm_offset_t v;

    if (seg_type != N_TEXT || fixP->fx_r_type == NO_RELOC) {
	md_number_to_chars(buf, val, n);
	return;
    }

    /* NOTE: We assume to be called *after* the size of       the text and data
     * segment have been computed. */

    /*  Set the initial value of the base register ifnotyet */
    if (computed_br_value == 0) {
	v = plus(the_exec.a_text, 	/* our (virt) data_start */
	  plus(the_exec.a_data, the_exec.a_bss));
	computed_br_value = low32(v) / 2;
	computed_br_value = (computed_br_value + 15) & ~15;
    }

    v = plus_a_32(zero, val);

    switch (fixP->fx_r_type) {

	case RELOC_0_7: 
            val &= 0xFF;    
	    buf[1] |= val << (SN - 8);
	    buf[2] |= (val >> (16 - SN)) & 0x1F;  /* mask off extra bits */
	    break;

	case BRELOC_0_15: 
#if 1
	    val = val - computed_br_value;
#endif
	case RELOC_0_15: 
	    buf[0] = val;
	    buf[1] = val >> 8;
	    break;

	case RELOC_0_8: 
	    buf[0] = val;
	    break;

	case RELOC_0_12: 
	    buf[0] = val;
	    buf[1] |= (val >> 8) & 0x0f;
	    break;

	case RELOC_0_10: 
	    buf[0] = val;
	    buf[1] |= (val >> 8) & 0x03;
	    break;

	case BRELOC_16_31: 
#if 1
	    val = val - computed_br_value;
#endif
	case RELOC_16_31: 
	    buf[0] = val >> 16;
	    buf[1] = val >> 24;
	    break;

	case RELOC_0_25: 
	    buf[0] = val;
	    buf[1] = val >> 8;
	    buf[2] = val >> 16;
	    buf[3] |= (val >> 24) & 0x3;
	    break;

	case RELOC_WDISP21: 
	    val >>= 2;
/*    case RELOC_BASE21: */
	    buf[0] = val;
	    buf[1] = val >> 8;
	    buf[2] |= (val >> 16) & 0x1f;
	    break;

	case RELOC_WDISP14: 
	    val >>= 2;
/*    case RELOC_BASE14: */
	    buf[0] = val;
	    buf[1] |= (val >> 8) & 0x3f;
	    break;

	case RELOC_0_63: 
	    if (logorrea)
		fprintf(stderr, "%lx[%d] = %lx. %d\n", buf, n, val, seg_type);
	    buf[0] = val;
	    buf[1] = val >> 8;
	    buf[2] = val >> 16;
	    buf[3] = val >> 24;
#ifdef CROSS_COMPILE_32_TO_64_BITS
	    buf[4] = buf[5] = buf[6] = buf[7] = 0;
#else
	    buf[4] = val >> 32;
	    buf[5] = val >> 40;
	    buf[6] = val >> 48;
	    buf[7] = val >> 56;
#endif
	    break;

	case RELOC_0_31: 
	    buf[0] = val;
	    buf[1] = val >> 8;
	    buf[2] = val >> 16;
	    buf[3] = val >> 24;
	    break;

	case BRSET_16_31: 
	case BRSET_0_15: 
	    val += computed_br_value;
	    if (val & 0x8000)
		val += 0x10000;		/* compensate for sign */
	    if (fixP->fx_r_type == BRSET_0_15) {
		buf[0] = val;
		buf[1] = val >> 8;
	    } else {
		buf[0] = val >> 16;
		buf[1] = val >> 24;
	    }
	    break;
	case NO_RELOC: 
	default: 
	    as_warn("bad relocation type: 0x%02x", fixP->fx_r_type);
	    break;
    }
    return;
}

/* should never be called for alpha */
void md_create_short_jump(ptr, from_addr, to_addr, frag, to_symbol)
    char *ptr;
    long from_addr, to_addr;
{
    fprintf(stderr, "alpha_create_short_jmp\n");
    abort();
}

/* should never be called for alpha */
void md_number_to_disp(buf, val, n)
    char *buf;
    long val;
{
    fprintf(stderr, "md_number_to_disp\n");
    abort();
}

/* should never be called for alpha */
void md_number_to_field(buf, val, fix)
    char *buf;
    long val;
    void *fix;
{
    fprintf(stderr, "alpha_number_to_field\n");
    abort();
}

/* the bit-field entries in the relocation_info struct plays hell 
   with the byte-order problems of cross-assembly.  So as a hack,
   I added this mach. dependent ri twiddler.  Ugly, but it gets
   you there. -KWK */
/* on sparc: first 4 bytes are normal unsigned long address, next three
   bytes are index, most sig. byte first.  Byte 7 is broken up with
   bit 7 as external, bits 6 & 5 unused, and the lower
   five bits as relocation type.  Next 4 bytes are long int addend. */
/* Thanx and a tip of the hat to Michael Bloom, mb@ttidca.tti.com */
/* on alpha: same, bitfields are just slightly different.
   Also, changed the implementation of this routine. */
void md_ri_to_chars(ri_p, ri)
    struct relocation_info *ri_p, ri;
{
    unsigned char the_bytes[sizeof(*ri_p)];
    struct relocation_info *bf = (struct relocation_info *) 0;

    md_number_to_chars(the_bytes + (int) &bf->r_address, ri.r_address, 
      sizeof(ri.r_address));
    md_number_to_chars(the_bytes + (int) &bf->r_index, ri.r_index, 
      sizeof(ri.r_index));
    md_number_to_chars(the_bytes + (int) &bf->r_extern, ri.r_extern, 
      sizeof(ri.r_extern));
/*  md_number_to_chars(the_bytes + (int)&bf->r_type,
  			ri.r_type, sizeof(ri.r_type));
   sigh. */
    the_bytes[(int) &bf->r_extern + 1] = ri.r_type;

    md_number_to_chars(the_bytes + (int) &bf->r_addend, ri.r_addend, 
      sizeof(ri.r_addend));

    /* now put it back where you found it, Junior... */
    bcopy(the_bytes, (char *) ri_p, sizeof(*ri_p));
}

/* should never be called for alpha */
void md_convert_frag(fragP)
    register fragS *fragP;
{
    fprintf(stderr, "alpha_convert_frag\n");
    abort();
}

/* should never be called for alpha */
void md_create_long_jump(ptr, from_addr, to_addr, frag, to_symbol)
    char *ptr;
    long from_addr, to_addr;
    fragS *frag;
    symbolS *to_symbol;
{
    fprintf(stderr, "alpha_create_long_jump\n");
    abort();
}

/* should never be called for alpha */
int md_estimate_size_before_relax(fragP, segtype)
    register fragS *fragP;
{
    fprintf(stderr, "alpha_estimate_size_before_relax\n");
    abort();
    return 0;
}

#if 0
/* for debugging only */
static void print_insn(insn)
    struct alpha_it *insn;
{
    char *Reloc[] = {"RELOC_0_7", "RELOC_0_15", "RELOC_0_31", "RELOC_0_63", 
      "RELOC_0_25", "RELOC_16_31", "RELOC_WDISP21", "RELOC_BASE21", 
      "RELOC_WDISP14", "RELOC_BASE14", "RELOC_32_47", "RELOC_48_63", 
      "RELOC_U_16_31", "RELOC_U_32_47", "RELOC_U_48_63", "NO_RELOC"};

    if (insn->error) {
	fprintf(stderr, "ERROR: %s\n");
    }
    fprintf(stderr, "opcode=0x%08x\n", insn->opcode);
    fprintf(stderr, "reloc = %s\n", Reloc[insn->reloc]);
    fprintf(stderr, "exp =  {\n");
    fprintf(stderr, "\t\tX_add_symbol = %s\n", 
      insn->exp.X_add_symbol ? (insn->exp.X_add_symbol->sy_name ? 
      insn->exp.X_add_symbol->sy_name : "???") : "0");
    fprintf(stderr, "\t\tX_sub_symbol = %s\n", 
      insn->exp.X_subtract_symbol ? (insn->exp.X_subtract_symbol->sy_name ? 
      insn->exp.X_subtract_symbol->sy_name : "???") : "0");
    fprintf(stderr, "\t\tX_add_number = %d\n", insn->exp.X_add_number);
    fprintf(stderr, "}\n");
    return;
}
#endif

/*
 * Alpha relocations are completely different, so it needs
 * this machine dependent routine to emit them.
 */
void emit_relocations(fixP, segment_address_in_file)
    register fixS *fixP;
    relax_addressT segment_address_in_file;
{
    struct relocation_info ri;
    register symbolS *symbolP;
    extern char *next_object_file_charP;
    long add_number;

    bzero((char *) &ri, sizeof(ri));
    for (; fixP; fixP = fixP->fx_next) {

	if (fixP->fx_r_type > BRSET_16_31) {
	    fprintf(stderr, "fixP->fx_r_type = %d\n", fixP->fx_r_type);
	    abort();
	}

	if ((symbolP = fixP->fx_addsy) != NULL) {
	    ri.r_address = plus_a_32(zero, fixP->fx_frag->fr_address + 
	      fixP->fx_where - segment_address_in_file);
	    if ((symbolP->sy_type & N_TYPE) == N_UNDF) {
		ri.r_extern = 1;
		ri.r_index = symbolP->sy_number;
	    } else {
		ri.r_extern = 0;
		ri.r_index = symbolP->sy_type & N_TYPE;
	    }
	    if (symbolP->sy_frag) {
		ri.r_addend = plus_a_32(zero, symbolP->sy_frag->fr_address);
		if (logorrea)
		    fprintf(stderr, " r_addend %x", low32(ri.r_addend));
	    }
	    ri.r_type = fixP->fx_r_type;
	    if (fixP->fx_pcrel) {
/*		ri.r_addend -= fixP->fx_where;          */
		ri.r_addend = minus(ri.r_addend, plus_a_32(ri.r_address, 4));
	    } else {

		/* a hack cuz linker makes RELOC_0_63 ADD_MEM for compat */
		if (ri.r_type == RELOC_0_63)
		    ri.r_addend = zero;
		else
		    ri.r_addend = plus_a_32(zero, fixP->fx_addnumber);
	    }
	    if (logorrea)
		fprintf(stderr, " %cr_addend %x @%x\n", 
		  fixP->fx_pcrel ? '-' : ' ', ri.r_addend, ri.r_address);

/*	    md_ri_to_chars((char *) &ri, ri);        */
	    append(&next_object_file_charP, (char *) &ri, sizeof(ri));
	}
    }
    return;
}

void s_load_PALcode_Instructions()
{
    register unsigned int i;
    register char *retval;
    register struct alpha_opcode *PALopcodes;
    register int PALsize = 0;
/*
 *  Add in the PALcode specific opcodes for this particular implementation
 *  of the alpha architecture.
 */

    switch (machine_type) {
    case _type_21064: 
	PALopcodes = pal_21064_opcodes;
	PALsize = NUM21064OPCODES;
	break;
    case _type_21164: 
	PALopcodes = pal_21164_opcodes;
	PALsize = NUM21164OPCODES;
	break;
    case _type_21264: 
	PALopcodes = pal_21264_opcodes;
	PALsize = NUM21264OPCODES;
	break;
    default: 
	as_fatal("Unknown machine type.  No assembly attempted.");
    }

    i = 0;
    while (i < PALsize) {
	const char *name = PALopcodes->name;
	retval = hash_insert(op_hash, name, PALopcodes);
	if (retval != NULL && *retval != '\0') {
	    fprintf(stderr, "internal error: can't hash `%s': %s\n", 
	     PALopcodes->name, retval);
	     as_fatal("Broken assembler.  No assembly attempted.");
	 }
	 do {
	    ++i;
	    PALopcodes++;
	 } while (i < PALsize && !strcmp(PALopcodes->name, name));
    }
    PALcodeInstructionsLoaded = TRUE;
    return;
}
int docpp = 0;
int machine_already_chosen = FALSE;

int md_parse_option(argP, cntP, vecP)
    char **argP;
    int *cntP;
    char *** vecP;
{
    int machine_chosen = FALSE;

    if (**argP == 'F') {
	nofloats = TRUE;
	return 1;
    }

    /* Asks for a preprocessor pass */
    if (**argP == 'P') {
	docpp++;
	return 1;
    }

    /* asks for list files */
    if (**argP == 'L') {
	return 1;
    }

    /* Use base-register addressing, e.g. PIC code */
    if (**argP == 'B') {
	if (first_32bit_quadrant) {
	    first_32bit_quadrant = FALSE;
	    base_register = GP;
	} else {
	    first_32bit_quadrant = TRUE;
	    base_register = ZERO;
	}
	if (argP[0][1] == 'k')
	    no_mixed_code = TRUE;
	argP[0][1] = 0;
	return 1;
    }
/*
 *  The user may be passing the type of machine
 *  (default is 21064).
 */
    if (strncmp("21064", *argP, 5) == 0) {
	if (machine_already_chosen)
	    as_warn("Attempt to define the machine type twice");
	else {
	    machine_type = _type_21064;
	    argP[0][1] = 0;
	    machine_chosen = machine_already_chosen = TRUE;
	}
    }
    if (strncmp("21066", *argP, 5) == 0) {
	if (machine_already_chosen)
	    as_warn("Attempt to define the machine type twice");
	else {
	    machine_type = _type_21066;
	    argP[0][1] = 0;
	    machine_chosen = machine_already_chosen = TRUE;
	}
    }
    if (strncmp("21164", *argP, 5) == 0) {
	if (machine_already_chosen)
	    as_warn("Attempt to define the machine type twice");
	else {
	    machine_type = _type_21164;
	    argP[0][1] = 0;
	    machine_chosen = machine_already_chosen = TRUE;
	}
    }
    if (strncmp("21264", *argP, 5) == 0) {
	if (machine_already_chosen)
	    as_warn("Attempt to define the machine type twice");
	else {
	    machine_type = _type_21264;
	    argP[0][1] = 0;
	    machine_chosen = machine_already_chosen = TRUE;
	}
    }
/*
 *  If we've just selected a machine type, then
 *  add the machine specific PALcode ops into the
 *  instruction hash table.
 */
    if (machine_chosen) {
        s_load_PALcode_Instructions();
	return 1;
    }
/*
 *  We don't know this argument, so return an error.
 */
    return 0;
}


md_s_set(name)
    char *name;
{
    if (strcmp(name, "at") == 0)
	return 1;
    if (strcmp(name, "noat") == 0)
	return 1;
    if (strcmp(name, "reorder") == 0)
	return 1;
    if (strcmp(name, "noreorder") == 0)
	return 1;
    return 0;
}

/*	-----------	Disassembler for list output	------------	*/
/*	-----------	Courtesy of David Rusling	------------	*/
/*	-----------	rusling@ricks.enet.dec.com	------------	*/

/*=============================================*/
/* Masks for Disassembly of instruction fields */
/*=============================================*/

/* Masks used to recognise the instruction */

#define OPCODE_MASK 0xFC000000
#define IMMEDIATE_MASK 0x00001000
#define JUMP_FUNCTION_MASK 0x0000C000
#define MEMORY_FUNCTION_MASK 0x0000FFFF
#define INTEGER_OPERATE_FUNCTION_MASK 0x00000FE0
#define FPOINT_OPERATE_FUNCTION_MASK 0x0000FFE0
#define HW_MODE_BITS_MASK	     0x0000F000
#define HW_REG_SET_MASK		     0x000000E0

/* Masks used to find arguments */
#define RA_MASK 0x03E00000
#define RB_MASK 0x001F0000
#define RC_MASK 0x0000001F
#define MEM_DISP_MASK 0x0000FFFF
#define HW_MEM_DISP_MASK 0x00000FFF
#define HW_MEM_DISP_MASK_10 0x000003FF
#define JUMP_HINT_MASK 0x00003FFF
#define BRANCH_DISP_MASK 0x001FFFFF
#define LITERAL_MASK 0x001FE000
#define PALCODE_FUNCTION_MASK 0x03FFFFFF
#define INTEGER_OPERATE_SBZ_MASK 0x0000E000
#define MXPR_DISPLACEMENT_MASK 0x0000000F

/* Shifts for arguments */

#define RA_SHIFT 21
#define RB_SHIFT 16
#define RC_SHIFT 0
#define MEM_DISP_SHIFT 0
#define HW_MEM_DISP_SHIFT 0
#define JUMP_HINT_SHIFT 0
#define BRANCH_DISP_SHIFT 0
#define LITERAL_SHIFT 13
#define PALCODE_FUNCTION_SHIFT 0

/* Function to search for a instruction in the opcode table */
struct alpha_opcode *find_opcode(int instruction)
{
    struct alpha_opcode *opcodedata = 0;
    int i;

    /* Search the opcode table */
    for (i = 0; i < NUMOPCODES; i++) {

	/* Check if the function code matches */
	if ((alpha_opcodes[i].match & OPCODE_MASK) == 
	  (instruction & OPCODE_MASK)) {

	    /* opcode matches so this is a candidate; check what arguments it  
	     * takes to work out whether a function code has to match */
	    if (strcmp(alpha_opcodes[i].args, "1,l(2)") == 0 || 
	      strcmp(alpha_opcodes[i].args, "e,l(2)") == 0 || 
	      strcmp(alpha_opcodes[i].args, "1,L") == 0) {

		/* Memory or branch format without function code */
		opcodedata = &alpha_opcodes[i];
		break;
	    } else if (strcmp(alpha_opcodes[i].args, "1,(2),i") == 0) {

		/* Jump (Memory format branch) format; check the jump function 
		 * mask */
		if ((alpha_opcodes[i].match & JUMP_FUNCTION_MASK) == 
		  (instruction & JUMP_FUNCTION_MASK)) {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    } else if (strcmp(alpha_opcodes[i].args, "1,t(2)") == 0) {

		/* EV4 hardware memory access format format; check the mode
		 * bits */
		if ((alpha_opcodes[i].match & HW_MODE_BITS_MASK) == 
		  (instruction & HW_MODE_BITS_MASK)) {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    } else if (strcmp(alpha_opcodes[i].args, "R,3") == 0) {

		/* EV4 internal register access format format; check the
		 * register set mask */
		if ((alpha_opcodes[i].match & HW_REG_SET_MASK) == 
		  (instruction & HW_REG_SET_MASK)) {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    } else if (strcmp(alpha_opcodes[i].args, "R,8") == 0) {

		/* EV4 internal register access format format; check the
		 * register set mask */
		if ((alpha_opcodes[i].match & HW_REG_SET_MASK) == 
		  (instruction & HW_REG_SET_MASK)) {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    } else if (strcmp(alpha_opcodes[i].args, "1,2,3") == 0 || 
	      strcmp(alpha_opcodes[i].args, "1,b,3") == 0) {

		/* Operate format; check immediate bit and function code */
		if ((alpha_opcodes[i].match & IMMEDIATE_MASK) == 
		  (instruction & IMMEDIATE_MASK) && 
		  (alpha_opcodes[i].match & INTEGER_OPERATE_FUNCTION_MASK) == 
		  (instruction & INTEGER_OPERATE_FUNCTION_MASK)) {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    } else if (strcmp(alpha_opcodes[i].args, "e,f,g") == 0 || 
	      strcmp(alpha_opcodes[i].args, "f,g") == 0) {

		/* Operate format; check immediate bit and function mask */
		if ((alpha_opcodes[i].match & FPOINT_OPERATE_FUNCTION_MASK) == 
		  (instruction & FPOINT_OPERATE_FUNCTION_MASK)) {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    } else if (strcmp(alpha_opcodes[i].args, "I") == 0) {

		/* We have found a PAL code call instruction - there may a more
		 * precise opcode defined */
		/* so only use this code if we have not already found a match
		 */

		if (opcodedata == 0)
		    opcodedata = &alpha_opcodes[i];

	    /* Don't exit the loop; there may be a more precise match yet */
	    } else if (strcmp(alpha_opcodes[i].args, "0(2)") == 0) {

		/* Miscellaneous instructions - Memory format with function
		 * code*/
		if ((alpha_opcodes[i].match & MEMORY_FUNCTION_MASK) == 
		  (instruction & MEMORY_FUNCTION_MASK)) {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    } else if (strcmp(alpha_opcodes[i].args, "1") == 0) {

		/* Miscellaneous instructions - Memory format with function
		 * code but different arguments */
		if ((alpha_opcodes[i].match & MEMORY_FUNCTION_MASK) == 
		  (instruction & MEMORY_FUNCTION_MASK)) {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    }

	      else if (strcmp(alpha_opcodes[i].args, "") == 0) {

		/* Miscellaneous instructions with no arguments; should be
		 * exact match */
		if (alpha_opcodes[i].match == instruction) {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    }

	/* All other cases are only used for predefined macros and
	 * pseudo-instructions; ignore them */
	}
    }

    /* Return the opcode data; it will be null if a match hasn't been found */
    return opcodedata;
}

/* Function to print an instruction and arguments */
void md_print_instruction(int instruction, FILE *stream)
{
    struct alpha_opcode *opcodedata;
    char *argptr;
    int duplicate_reg_bad = FALSE;
    int bad_reg_id;
    char bad_reg_name;


    /* Try to find it in the opcode table */
    opcodedata = find_opcode(instruction);
    if (opcodedata == 0) {
	fprintf(stream, "\t# BAD INSTRUCTION 0x%08x", instruction);
	return;
    }

    /* Print the instruction name */
    fprintf(stream, "%s\t", opcodedata->name);

    /* Now scan its arguments */
    argptr = opcodedata->args;

    while (*argptr != 0) {
	switch (*argptr) {

	    /* Real argument types */
	    case '1': 
	    case 'e': 
		fprintf(stream, "$%i", (instruction & RA_MASK) >> RA_SHIFT);
		break;
	    case '2': 
	    case 'f': 
		fprintf(stream, "$%i", (instruction & RB_MASK) >> RB_SHIFT);
		break;
	    case '3': 
	    case 'g': 
		fprintf(stream, "$%i", (instruction & RC_MASK) >> RC_SHIFT);
		break;
	    case 'r': 
		fprintf(stream, "$%i", (instruction & RA_MASK) >> RA_SHIFT);
		if (((instruction & RC_MASK) >> RC_SHIFT) != 
		  ((instruction & RA_MASK) >> RA_SHIFT)) {
		    duplicate_reg_bad = TRUE;
		    bad_reg_name = 'C';
		    bad_reg_id = (instruction & RC_MASK) >> RC_SHIFT;
		}
		break;
	    case 'R': 
		fprintf(stream, "$%i", (instruction & RA_MASK) >> RA_SHIFT);
		if (((instruction & RB_MASK) >> RB_SHIFT) != 
		  ((instruction & RA_MASK) >> RA_SHIFT)) {
		    duplicate_reg_bad = TRUE;
		    bad_reg_name = 'B';
		    bad_reg_id = (instruction & RB_MASK) >> RB_SHIFT;
		}
		break;
	    case 'I': 
		fprintf(stream, "0x%07x", (instruction & 
		  PALCODE_FUNCTION_MASK) >> PALCODE_FUNCTION_SHIFT);
		break;
	    case 'l': 
		fprintf(stream, "0x%04x", 
		  (instruction & MEM_DISP_MASK) >> MEM_DISP_SHIFT);
		break;
	    case 'h': 
		fprintf(stream, "0x%04x", 
		  ((instruction & MEM_DISP_MASK) >> MEM_DISP_SHIFT) << 16);
		break;
	    case 't': 
		/* Special for EV4 hardware load/store instructions */
		fprintf(stream, "0x%04x", 
		  ((instruction & HW_MEM_DISP_MASK) >> HW_MEM_DISP_SHIFT));
		break;
	    case 'T': 
		/* Special for EV5 hardware load/store instructions */
		fprintf(stream, "0x%04x", 
		  ((instruction & HW_MEM_DISP_MASK_10) >> HW_MEM_DISP_SHIFT));
		break;
	    case 'L': 
		fprintf(stream, ".+0x%08x", 
		  ((instruction & BRANCH_DISP_MASK) >> BRANCH_DISP_SHIFT) * 4);
		break;
	    case 'i': 
		fprintf(stream, ".+0x%06x", 
		  ((instruction & JUMP_HINT_MASK) >> JUMP_HINT_SHIFT) * 4);
		break;
	    case 'b': 
		fprintf(stream, "0x%02x", 
		  (instruction & LITERAL_MASK) >> LITERAL_SHIFT);
		break;
	    case '8': 
		fprintf(stream, "0x%02x", 
		  (instruction & MXPR_DISPLACEMENT_MASK));
		break;

	    /* All other possible characters are punctuation; print what we are
	     * given */
	    default: 
		fprintf(stream, "%c", *argptr);
	}
	argptr++;
    }

    /* Check for some special cases where fields must be zero etc. */
    if (strcmp(opcodedata->args, "1,2,3") == 0) {
	if ((instruction & INTEGER_OPERATE_SBZ_MASK) != 0) {
	    fprintf(stream, " WARNING: SBZ field not zero");
	}
    } else if (strcmp(opcodedata->args, "f,g") == 0) {
	if ((instruction & RA_MASK) != RA_MASK) {
	    fprintf(stream, " WARNING: RA field not $31");
	}
    }

    if (duplicate_reg_bad) {
	fprintf(stream, 
	  " WARNING: R%c register field does not match RA - value %i", 
	  bad_reg_name, bad_reg_id);
    }
}

