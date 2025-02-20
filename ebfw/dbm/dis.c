
/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
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
__attribute__((unused)) static const char *rcsid = "$Id: dis.c,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $";
#endif

/*
 *      An instruction disassembler for alpha.
 *
 * $Log: dis.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.16  1997/09/17  20:03:37  pbell
 * Added support for load/store byte/word instructions.
 *
 * Revision 1.15  1995/12/05  23:51:42  cruz
 * Fixed bug in printing the register names.  Can't call reg()
 * more than once in the same statement because of the static
 * string variable.
 *
 * Revision 1.14  1995/12/01  22:22:19  cruz
 * Fixed code so it could print the correct name for floating point
 * registers.  Removed unnecessary code to reduce space.
 *
 * Revision 1.13  1995/11/22  21:25:59  cruz
 * Removed static initialization of regmode.
 *
 * Revision 1.12  1995/10/26  21:31:37  cruz
 * Clean up code.  Made local variables static.
 *
 * Revision 1.11  1995/10/18  12:51:10  fdh
 * Modified to remove unused flags arguments.
 *
 * Revision 1.10  1994/12/07  19:14:10  cruz
 * Changed instruction names "MT" and "MF" to "MTPR" and "MFPR"
 *
 * Revision 1.9  1994/08/05  20:18:01  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.8  1994/06/29  14:09:55  rusling
 * Added floating point disassembly.
 *
 * Revision 1.7  1994/06/23  13:43:51  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.6  1994/06/19  15:44:07  fdh
 * Correct cast for mask so that data is not printed
 * signed extended.
 *
 * Revision 1.5  1994/06/17  19:36:54  fdh
 * Clean-up...
 *
 * Revision 1.4  1994/04/01  22:44:38  fdh
 * Modified data type for DisStartAddress.
 *
 * Revision 1.3  1994/03/25  04:23:37  fdh
 * Correct a couple of register names.
 *
 * Revision 1.2  1994/01/19  10:50:12  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.1  1993/06/08  19:56:40  fdh
 * Initial revision
 *
 */

#include "lib.h"
#include "mon.h"
#include "dis.h"

#define DEFAULT_NUMBER_OF_INSTRUCTIONS 8


/*
 * Variable for specifying the type of the register names to 
 * be used: software (1) or hardware names (0).
 */
ui regmode;

/*
 *  Address where dissassembly of code should start if no 
 *  address is specified with "dis" cmd.
 */
ul DisStartAddress;


/*
 * disassemble range of instructions
 *
 *   first = address of first instruction to disassemble
 *   last  = address of last instruction to disassemble
 */
void dis( int argc, ul first, ul last )
{
  ul i;
  ui  instr;

  /*
   *  set values for first and noi as they are optional parameters
   */
  if( argc < 2 ) first = DisStartAddress;

  /* make sure address is long-aligned  */
  first &= (ul)(-4);  /* And with F...FFC */

  if( argc < 3 ) last = first - 4 + DEFAULT_NUMBER_OF_INSTRUCTIONS*4;

  /*
   * set DisStartAddress to new value
   */
  DisStartAddress = last + 4;
  for( i=first; i <= last; i+=4 ){
     printf( "%08x:  ", i );
     instr = ReadL( i );
     printf( "%08x  ", instr );
     dis_instruct( instr, i );
     printf( "\n" );
  }
}

#define BR_OPC_LOW   0x30
#define BR_OPC_HI    0x3f
#define MEM_OPC_LOW  0x20
#define MEM_OPC_HI   0x2f
#define MEM_OPC_FLT_LOW 0x20
#define MEM_OPC_FLT_HI  0x27
#define OPR_OPC_LOW  0x10
#define OPR_OPC_HI   0x13
#define FLOAT_OPC    0x17
#define FIEEE_OPC    0x16
#define FVAX_OPC     0x15
#define PAL_OPC      0x0

#define PAL_MT       0x1d
#define PAL_MF       0x19
#define PAL_LD       0x1b
#define PAL_ST       0x1f
#define PAL_REI      0x1e

#define OPR_MEM_DISP 0x18
#define OPR_MEM_JMP  0x1a
#define OPR_LDA      0x8
#define OPR_LDAH     0x9
#define OPR_LDBU     0xa
#define OPR_LDQ_U    0xb
#define OPR_LDWU     0xc
#define OPR_STW	     0xd
#define OPR_STB	     0xe
#define OPR_STQ_U    0xf

static void dis_instruct( ui instr, ul addr )
{
   union instruction i;
   register int op;

   i.full = instr;
   op = i.g.opcode;

   if( (op >= BR_OPC_LOW) && (op <= BR_OPC_HI) ) {
     dis_branch( i, addr );
     return;
   }

   if( (op >= MEM_OPC_LOW) && (op <= MEM_OPC_HI) ) {
     dis_mem( i );
     return;
   }

   if( (op >= OPR_OPC_LOW) && (op <= OPR_OPC_HI) ) {
     dis_opr( i );
     return;
   }

   if( op == FLOAT_OPC ) {
     dis_float(i );
     return;
   }

   if( op == FIEEE_OPC ) {
     dis_ieee( i );
     return;
   }

   if( op == FVAX_OPC ) {
     dis_vax( i );
     return;
   }

   if( op == PAL_OPC ) {
     dis_pal( i );
     return;
   }

   switch( op ){
   case OPR_MEM_DISP:
     dis_memdisp( i );
     return;
   case OPR_LDA:
   case OPR_LDAH:
   case OPR_LDQ_U:
   case OPR_STQ_U:
   case OPR_LDBU:
   case OPR_LDWU:
   case OPR_STB:
   case OPR_STW:
     dis_memsp( i );
     return;
   case OPR_MEM_JMP:
     dis_memjmp( i );
     return;
   case PAL_MT:
   case PAL_MF:
   case PAL_LD:
   case PAL_ST:
   case PAL_REI:
     dis_palopc( i );
     return;
   default:
     unknwn( i );
     return;
   }

 }

static char *br_ops[]={
                  "br",
		  "fbeq",
		  "fblt",
		  "fble",
		  "bsr",
		  "fbne",
		  "fbge",
		  "fbgt",
		  "blbc",
		  "beq",
		  "blt",
		  "ble",
		  "blbs",
		  "bne",
		  "bge",
		  "bgt"
		  };
		    
static void dis_branch( union instruction i, ul addr )
  {
    int disp = (int)i.b.br_disp;
    int reg_type;

    disp +=1;   /* displacement is relative to updated pc */
    disp *=4;   /* displacement is a longword offset */

    addr += disp;

    if (*br_ops[i.b.opcode - BR_OPC_LOW] == 'f') 
      reg_type = FP_REG;
    else
      reg_type = INT_REG;

    printf("%10s    %2s, %x", br_ops[i.b.opcode - BR_OPC_LOW],
           reg(i.b.Ra, reg_type), addr);
  }

static char *mem_ops[]={
  "ldf",
  "ldg",
  "lds",
  "ldt",
  "stf",
  "stg",
  "sts",
  "stt",
  "ldl",
  "ldq",
  "ldl_l",
  "ldq_l",
  "stl",
  "stq",
  "stl_c",
  "stq_c"
  };

static void dis_mem( union instruction i )
{
  char *opc;
  opc =  mem_ops[i.m.opcode - MEM_OPC_LOW]; 
  
  /* If floating instruction, print fp registers. */
  if( (i.m.opcode >= MEM_OPC_FLT_LOW) && (i.m.opcode <= MEM_OPC_FLT_HI) )
    fmt_mem(opc, i, FP_REG);
  else 
    fmt_mem(opc, i, INT_REG);
}

static void dis_memsp( union instruction i )
{
   char *opc;

   switch( i.m.opcode )
   {
   case OPR_LDA:    opc = "lda";    break;
   case OPR_LDAH:   opc = "ldah";   break;
   case OPR_LDQ_U:  opc = "ldq_u";  break;
   case OPR_STQ_U:  opc = "stq_u";  break;
   case OPR_LDBU:   opc = "ldbu";   break;
   case OPR_LDWU:   opc = "ldwu";   break;
   case OPR_STB:    opc = "stb";    break;
   case OPR_STW:    opc = "stw";    break;

   default:	    unknwn( i );    return;
   }

   fmt_mem(opc, i, INT_REG);
}

static void fmt_mem( char *opc, union instruction i, int reg_type )
{
    printf( "%10s    %2s, ", opc, reg(i.m.Ra, reg_type));
    printf( "%d(%2s)", (uw)i.m.mem_disp, reg(i.m.Rb, INT_REG));
}



#define OPR_MB      0x4000
#define OPR_FETCH   0x8000
#define OPR_RS      0xf000
#define OPR_DRAINT  0x0000
#define OPR_FETCH_M 0xa000
#define OPR_RPCC    0xc000
#define OPR_RC      0xe000


static void dis_memdisp( union instruction i )
{
  char *opc;

  switch( i.m.mem_disp ){
  case OPR_MB:
    opc = "mb";
    break;
  case OPR_FETCH:
    opc = "fetch";
    break;
  case OPR_RS:
    opc = "rs";
    break;
  case OPR_DRAINT:
    opc = "draint";
    break;
  case OPR_FETCH_M:
    opc = "fetch_m";
    break;
  case OPR_RPCC:
    opc = "rpcc";
    break;
  case OPR_RC:
    opc = "rc";
    break;
  default:
    unknwn( i );
    return;
  }

  printf( "%10s    ", opc );

}

static char *jmp_ops[]={
                 "jmp",
		 "jsr",
		 "ret",
		 "jsr_coroutine" };

static void dis_memjmp( union instruction i )
{
  char *opc;

  opc = jmp_ops[ ( i.m.mem_disp >> 14 )];

  printf( "%10s    %2s, ", opc, reg( i.m.Ra, INT_REG ));
  printf( "%2s", reg( i.m.Rb, INT_REG) );
}

#define OPR10 0x10
#define OPR11 0x11
#define OPR12 0x12
#define OPR13 0x13

static void dis_opr( union instruction i )
  {

    switch( i.g.opcode ){
    case OPR10:
      dis_opr10( i );
      return;
    case OPR11:
      dis_opr11( i );
      return;
    case OPR12:
      dis_opr12( i );
      return;
    case OPR13:
      dis_opr13( i );
      return;
    default:
      unknwn( i );
      return;
    }
  }


static char *add_ops[]={
                  "addl",
		  "addq",
		  "addl/v",
		  "addq/v" };
static char *sub_ops[] = {
                   "subl",
		   "subq",
		   "subl/v",
		   "subq/v" };

static char *cmp_ops[]={
                  "UNKNWN",
                  "cmpult",
		  "cmpeq",
		  "cmpule",
		  "cmplt",
		  "UNKNWN",
		  "cmple" };

static char *sadd_ops[]={
                  "s4addl",
		  "s8addl",
		  "s4addq",
		  "s8addq" };
static char *ssub_ops[]={
                  "s4subl",
		  "s8subl",
		  "s4subq",
		  "s8subq" };

#define OPR_ADD  0x0
#define OPR_SUB  0x9
#define OPR_CMP  0xd
#define OPR_SADD 0x2
#define OPR_SSUB 0xb

static void dis_opr10( union instruction i )
{
  char *op;

  switch( (i.o.func & 0xf) ){
  case OPR_ADD:
    op = add_ops[ (i.o.func >> 5) ];
    break;
  case OPR_SUB:
    op = sub_ops[ (i.o.func >> 5) ];
    break;
  case OPR_CMP:
    op = cmp_ops[ (i.o.func >> 4) ];
    break;
  case OPR_SADD:
    op = sadd_ops[ (i.o.func >> 4) ];
    break;
  case OPR_SSUB:
    op = ssub_ops[ (i.o.func >> 4) ];
    break;
  default:
    unknwn( i );
    return;
  }

  opr_fmt( op, i );
}


static char *and_ops[]={
                 "and",
		 "bis",
		 "xor" };

static char *bic_ops[]={
                 "bic",
		 "ornot",
		 "eqv" };

static char *cmoveq_ops[]={
                  "UNKNWN",
		  "cmovlbs",
		  "cmoveq",
		  "UNKNWN",
		  "cmovlt",
		  "UNKNWN",
		  "cmovle" };

static char *cmovne_ops[]={
                  "UNKNWN",
		  "cmovlbc",
		  "cmovne",
		  "UNKNWN",
		  "cmovge",
		  "UNKNWN",
		  "cmovgt" };


#define OPR_AND  0x0
#define OPR_BIC  0x8
#define OPR_CMOVEQ 0x4
#define OPR_CMOVNE 0x6

static void dis_opr11( union instruction i )
{
  char *op;

  switch( (i.o.func & 0xf) ){
  case OPR_AND:
    op = and_ops[ (i.o.func >> 5) ];
    break;
  case OPR_BIC:
    op = bic_ops[ (i.o.func >> 5) ];
    break;
  case OPR_CMOVEQ:
    op = cmoveq_ops[ (i.o.func >> 4) ];
    break;
  case OPR_CMOVNE:
    op = cmovne_ops[ (i.o.func >> 4) ];
    break;
  default:
    unknwn( i );
    return;
  }

  opr_fmt( op, i );
}

static char *extl_ops[]={
                  "extbl",
		  "extwl",
		  "extll",
		  "extql" };

static char *exth_ops[]={
                  "extwh",
		  "extlh",
		  "extqh" };

static char *insl_ops[]={
                  "insbl",
		  "inswl",
		  "insll",
		  "insql" };

static char *insh_ops[]={
                  "inswh",
		  "inslh",
		  "insqh" };


static char *msk_ops[]={
                 "mskbl",
		 "mskwl",
		 "mskll",
		 "mskql",
		 "UNKNWN",
		 "mskwh",
		 "msklh",
		 "mskqh" };

#define OPR_EXTL 0x6
#define OPR_EXTH 0xa
#define OPR_INSL 0xb
#define OPR_INSH 0x7
#define OPR_MSK  0x2
#define OPR_SLL 0x39
#define OPR_SRA 0x3c
#define OPR_SRL 0x34
#define OPR_ZAP 0x30
#define OPR_ZAPNOT 0x31

static void dis_opr12( union instruction i )
{
  char *op;

  switch( (i.o.func & 0xf) ){
  case OPR_EXTL:
    op = extl_ops[ (i.o.func >> 4) ];
    break;
  case OPR_EXTH:
    op = exth_ops[ (i.o.func >> 4) - 5 ];
    break;
  case OPR_INSL:
    op = insl_ops[ (i.o.func >> 4) ];
    break;
  case OPR_INSH:
    op = insh_ops[ (i.o.func >> 4) - 5 ];
    break;
  case OPR_MSK:
    op = msk_ops[ (i.o.func >> 4) ];
    break;
  default:
    switch( i.o.func ){
    case OPR_SLL:
      op = "sll";
      break;
    case OPR_SRA:
      op = "sra";
      break;
    case OPR_SRL:
      op = "srl";
      break;
    case OPR_ZAP:
      op = "zap";
      break;
    case OPR_ZAPNOT:
      op = "zapnot";
      break;
    default:
      unknwn( i );
      return;
    }
  }

  opr_fmt( op, i );
}

static char *mul_ops[]={
                  "mull",
		  "UNKNWN",
		  "mulq",
		  "umulh",
		  "mull/v",
		  "UNKNWN",
		  "mulq/v" };

static void dis_opr13( union instruction i )
{
  char *op;

  op = mul_ops[ (i.o.func >> 4) ];

  opr_fmt( op, i );
}

#define OP_LIT 1

static void opr_fmt( char *opc, union instruction i )
{

   printf( "%10s    %2s, ", opc, reg(i.o.Ra, INT_REG));
   if( i.o.op_ty == OP_LIT ){
     printf( "0x%x, ", i.o.op_var );
   } else
     printf( "%2s, ", reg( (i.o.op_var >> 3), INT_REG) );

   printf( "%2s", reg(i.o.Rc, INT_REG) );
}

typedef struct {
    unsigned short op;
   char *name;
} TFloatOp;

static TFloatOp float_ops[] = {
{0x020, "cpys"}, {0x021, "cpyse"}, {0x022, "cpyse"},

{0x025, "mf_fpcr"}, {0x024, "mt_fpcr"}, 

{0x530, "cvtql/sv"}, {0x010, "cvtq"}, {0x030, "cvtql"}, {0x130, "cvtql/v"},

{0x02a, "fcmoveq"}, {0x02c, "fcmovlt"}, {0x02e, "fcmovle"}, 
{0x02b, "fcmovne"}, {0x02d, "fcmovge"}, {0x02f, "fcmovgt"},
 
};

static void dis_float( union instruction i )
{
  unsigned long j;

/* implements table C-6 */

   for (j = 0; j < sizeof(float_ops) / sizeof(TFloatOp); j++) {
       if (float_ops[j].op == i.f.func) {
	   printf("%10s    ", float_ops[j].name );
	   printf("%2s,", reg(i.f.Fa, FP_REG));
	   printf("%2s,", reg(i.f.Fb, FP_REG));
	   printf("%2s", reg(i.f.Fc, FP_REG));
	   return;
       }
   }
   printf( "unknown (float)" );
}


static TFloatOp ieee_ops[] = {
{0x080, "adds"}, {0x000, "adds/c"}, {0x040, "adds/m"}, {0x0c0, "adds/d"},
{0x180, "adds/u"}, {0x100, "adds/uc"}, {0x140, "adds/um"}, {0x1c0, "adds/ud"},
{0x580, "adds/su"}, {0x500, "adds/suc"}, {0x540, "adds/sum"}, {0x5c0, "adds/sud"},
{0x780, "adds/sui"}, {0x700, "adds/suic"}, {0x740, "adds/suim"}, {0x7c0, "adds/suid"},

{0x0a0, "addt"}, {0x020, "addt/c"}, {0x060, "addt/m"}, {0x0e0, "addt/d"},
{0x1a0, "addt/u"}, {0x120, "addt/uc"}, {0x160, "addt/um"}, {0x1e0, "addt/ud"},
{0x5a0, "addt/su"}, {0x520, "addt/suc"}, {0x560, "addt/sum"}, {0x5e0, "addt/sud"},
{0x7a0, "addt/sui"}, {0x720, "addt/suic"}, {0x760, "addt/suim"}, {0x7e0, "addt/suid"},

{0x0a5, "cmpteq"}, {0x5a5, "cmpteq/su"},

{0x0a6, "cmptlt"}, {0x5a6, "cmptlt/su"},

{0x0a7, "cmptle"}, {0x5a7, "cmptle/su"},

{0x0a4, "cmptun"}, {0x5a4, "cmptun/su"},

{0x0bc, "cvtqs"}, {0x03c, "cvtqs/c"}, {0x07c, "cvtqs/m"}, {0x0fc, "cvtqs/d"},
{0x7bc, "cvtqs/sui"}, {0x73c, "cvtqs/suic"}, {0x77c, "cvtqs/suim"}, {0x7fc, "cvtqs/suid"},

{0x0be, "cvtqt"}, {0x03e, "cvtqt/c"}, {0x07e, "cvtqt/m"}, {0x0fe, "cvtqt/d"},
{0x7be, "cvtqt/sui"}, {0x73e, "cvtqt/suic"}, {0x77e, "cvtqt/suim"}, {0x7fe, "cvtqt/suid"},

{0x0ac, "cvtts"}, {0x02c, "cvtts/c"}, {0x06c, "cvtts/m"}, {0x0ec, "cvtts/d"},
{0x1ac, "cvtts/u"}, {0x12c, "cvtts/uc"}, {0x16c, "cvtts/um"}, {0x1ec, "cvtts/ud"},
{0x5ac, "cvtts/su"}, {0x52c, "cvtts/suc"}, {0x56c, "cvtts/sum"}, {0x5ec, "cvtts/sud"},
{0x7ac, "cvtts/sui"}, {0x72c, "cvtts/suic"}, {0x76c, "cvtts/suim"}, {0x7ec, "cvtts/suid"},

{0x083, "divs"}, {0x003, "divs/c"}, {0x043, "divs/m"}, {0x0c3, "divs/d"},
{0x183, "divs/u"}, {0x103, "divs/uc"}, {0x143, "divs/um"}, {0x1c3, "divs/ud"},
{0x583, "divs/su"}, {0x503, "divs/suc"}, {0x543, "divs/sum"}, {0x5c3, "divs/sud"},
{0x783, "divs/sui"}, {0x703, "divs/suic"}, {0x743, "divs/suim"}, {0x7c3, "divs/suid"},

{0x0a3, "divt"}, {0x023, "divt/c"}, {0x063, "divt/m"}, {0x0e3, "divt/m"},
{0x1a3, "divt/u"}, {0x123, "divt/uc"}, {0x163, "divt/um"}, {0x1e3, "divt/um"},
{0x5a3, "divt/su"}, {0x523, "divt/suc"}, {0x563, "divt/sum"}, {0x5e3, "divt/sum"},
{0x7a3, "divt/sui"}, {0x723, "divt/suic"}, {0x763, "divt/suim"}, {0x7e3, "divt/suim"},

{0x082, "muls"}, {0x002, "muls/c"}, {0x042, "muls/m"}, {0x0c2, "muls/d"},
{0x182, "muls/u"}, {0x102, "muls/uc"}, {0x142, "muls/um"}, {0x1c2, "muls/ud"},
{0x582, "muls/su"}, {0x502, "muls/suc"}, {0x542, "muls/sum"}, {0x5c2, "muls/sud"},
{0x782, "muls/sui"}, {0x702, "muls/suic"}, {0x742, "muls/suim"}, {0x7c2, "muls/suid"},

{0x0a2, "mult"}, {0x022, "mult/c"}, {0x062, "mult/m"}, {0x0e2, "mult/d"},
{0x1a2, "mult/u"}, {0x122, "mult/uc"}, {0x162, "mult/um"}, {0x1e2, "mult/ud"},
{0x5a2, "mult/su"}, {0x522, "mult/suc"}, {0x562, "mult/sum"}, {0x5e2, "mult/sud"},
{0x7a2, "mult/sui"}, {0x722, "mult/suic"}, {0x762, "mult/suim"}, {0x7e2, "mult/suid"},

{0x081, "subs"}, {0x001, "subs/c"}, {0x041, "subs/m"}, {0x0c1, "subs/d"},
{0x181, "subs/u"}, {0x101, "subs/uc"}, {0x141, "subs/um"}, {0x1c1, "subs/ud"},
{0x581, "subs/su"}, {0x501, "subs/suc"}, {0x541, "subs/sum"}, {0x5c1, "subs/sud"},
{0x781, "subs/sui"}, {0x701, "subs/suic"}, {0x741, "subs/suim"}, {0x7c1, "subs/suid"},

{0x0a1, "subt"}, {0x021, "subt/c"}, {0x061, "subt/c"}, {0x0e1, "subt/d"},
{0x1a1, "subt/u"}, {0x121, "subt/uc"}, {0x161, "subt/uc"}, {0x1e1, "subt/ud"},
{0x5a1, "subt/su"}, {0x521, "subt/suc"}, {0x561, "subt/suc"}, {0x5e1, "subt/sud"},
{0x7a1, "subt/sui"}, {0x721, "subt/suic"}, {0x761, "subt/suic"}, {0x7e1, "subt/suid"},

{0x0af, "cvttq"}, {0x02f, "cvttq/c"}, {0x1af, "cvttq/v"}, {0x12f, "cvttq/vc"},
{0x5af, "cvttq/sv"}, {0x52f, "cvttq/svc"}, {0x7af, "cvttq/svi"}, {0x72f, "cvttq/svic"},
{0x0ef, "cvttq/d"}, {0x1ef, "cvttq/vd"}, {0x5ef, "cvttq/svd"}, {0x7ef, "cvttq/svid"},
{0x06f, "cvttq/m"}, {0x16f, "cvttq/vm"}, {0x56f, "cvttq/svm"}, {0x76f, "cvttq/svim"},
};

static void dis_ieee( union instruction i )
{
    unsigned long j;

/* implements table C-7 */

   for (j = 0; j < sizeof(ieee_ops) / sizeof(TFloatOp); j++) {
       if (ieee_ops[j].op == i.f.func) {
	   printf("%10s    ", ieee_ops[j].name );
	   printf("%2s,", reg(i.f.Fa, FP_REG));
	   printf("%2s,", reg(i.f.Fb, FP_REG));
	   printf("%2s", reg(i.f.Fc, FP_REG));
	   return;
       }
   }
   printf( "unknown (ieee)" );
}

static TFloatOp vax_ops[] = {
{0x080, "addf"}, {0x000, "addf/c"}, {0x180, "addf/u"}, {0x100, "addf/uc"},
{0x480, "addf/s"}, {0x400, "addf/sc"}, {0x580, "addf/su"}, {0x500, "addf/suc"},

{0x09e, "cvtdg"}, {0x01e, "cvtdg/c"}, {0x19e, "cvtdg/u"}, {0x11e, "cvtdb/uc"},
{0x49e, "cvtdg/s"}, {0x41e, "cvtdg/sc"}, {0x59e, "cvtdg/su"}, {0x51e, "cvtdb/suc"},

{0x0a0, "addg"}, {0x020, "addg/c"}, {0x1a0, "addg/u"}, {0x120, "addg/uc"},
{0x4a0, "addg/s"}, {0x420, "addg/sc"}, {0x5a0, "addg/su"}, {0x520, "addg/suc"},

{0x0a5, "cmpgeq"}, {0x4a5, "cmpgeq/s"},

{0x0a6, "cmpglt"}, {0x4a6, "cmpglt/s"},

{0x0a7, "cmpgle"}, {0x4a7, "cmpgle/s"},

{0x0ac, "cvtgf"}, {0x02c, "cvtgf/c"}, {0x1ac, "cvtgf/u"}, {0x12c, "cvtgf/uc"},
{0x4ac, "cvtgf/s"}, {0x42c, "cvtgf/sc"}, {0x5ac, "cvtgf/su"}, {0x52c, "cvtgf/suc"},

{0x0ad, "cvtgd"}, {0x02d, "cvtgd/c"}, {0x1ad, "cvtgd/u"}, {0x12d, "cvtgd/uc"},
{0x4ad, "cvtgd/s"}, {0x42d, "cvtgd/sc"}, {0x5ad, "cvtgd/su"}, {0x52d, "cvtgd/suc"},

{0x0bc, "cvtqf"}, {0x03c, "cvtqf/c"},

{0x0be, "cvtqg"}, {0x03e, "cvtqg/c"},

{0x083, "divf"}, {0x003, "divf/c"}, {0x183, "divf/u"}, {0x103, "divf/uc"},
{0x483, "divf/s"}, {0x403, "divf/sc"}, {0x583, "divf/su"}, {0x503, "divf/suc"},

{0x0a3, "divg"}, {0x023, "divg/c"}, {0x1a3, "divg/u"}, {0x123, "divg/uc"},
{0x4a3, "divg/s"}, {0x423, "divg/sc"}, {0x5a3, "divg/su"}, {0x523, "divg/suc"},

{0x082, "mulf"}, {0x002, "mulf/c"}, {0x182, "mulf/u"}, {0x102, "mulf/uc"},
{0x482, "mulf/s"}, {0x402, "mulf/sc"}, {0x582, "mulf/su"}, {0x502, "mulf/suc"},

{0x0a2, "mulg"}, {0x022, "mulg/c"}, {0x1a2, "mulg/u"}, {0x122, "mulg/uc"},
{0x4a2, "mulg/s"}, {0x422, "mulg/sc"}, {0x5a2, "mulg/su"}, {0x522, "mulg/suc"},

{0x081, "subf"}, {0x001, "subf/c"}, {0x181, "subf/u"}, {0x101, "subf/uc"},
{0x481, "subf/s"}, {0x401, "subf/sc"}, {0x581, "subf/su"}, {0x501, "subf/suc"},

{0x0a1, "subg"}, {0x021, "subg/c"}, {0x1a1, "subg/u"}, {0x121, "subg/uc"},
{0x4a1, "subg/s"}, {0x421, "subg/sc"}, {0x5a1, "subg/su"}, {0x521, "subg/suc"},

};

static void dis_vax( union instruction i )
{
    unsigned long j;

/* implements table C-8 */

   for (j = 0; j < sizeof(vax_ops) / sizeof(TFloatOp); j++) {
       if (vax_ops[j].op == i.f.func) {
	   printf("%10s    ", vax_ops[j].name );
	   printf("%2s,", reg(i.f.Fa, FP_REG));
	   printf("%2s,", reg(i.f.Fb, FP_REG));
	   printf("%2s", reg(i.f.Fc, FP_REG));
	   return;
       }
   }
   printf( "unknown (vax)" );
}

static char *pal_priv[]={
  "halt","cflush","draina","","","","","","","cserve","","","","","","",
  "rdmces","wrmces","","","","","","","","","","","","","","",
  "","","","","","","","","","","","wrfen","","wrvptptr","","",
  "swpctx","wrval","rdval","tbi","wrent","swpipl","rdps","wrkgp","wrusp","","rdusp","","whami","retsys","","rti"
  };

static char *pal_unpriv[]={
  "bpt","bugchk","","callsys","","","imb","","","","","","","","","",
  "","","","","","","","","","","","","","","rdunique","wrunique",
  "","","","","","","","","","","gentrap","","","dbgstop","","",
  "","","","","ldqp","stqp","","","","","","","","","",""
  };

#define PAL_PRIV 0x0
#define PAL_PRIV_HI 0x3f
#define PAL_UNPRIV 0x80
#define PAL_UNPRIV_HI 0xbf

static void dis_pal( union instruction i )
{
   char *pal;

   printf( "" );

   if( (i.p.palcode >= PAL_UNPRIV) && (i.p.palcode <= PAL_UNPRIV_HI))
     pal = pal_unpriv[i.p.palcode - PAL_UNPRIV];
   else if(  ((int)i.p.palcode >= PAL_PRIV)  && (i.p.palcode <= PAL_PRIV_HI) ) 
     pal = pal_priv[i.p.palcode - PAL_PRIV];
   else {
     unknwn(i);
     return;
   }

   printf("  call_pal    %s", (strlen(pal) == 0) ? "opDec" : pal );

}

static void dis_palopc( union instruction i )
{

   switch( i.g.opcode ){
   case PAL_MT:
     printf( "%10s    ", "mtpr" );
     printf( "%2s", reg(i.hm.Ra, INT_REG) );
     printipr(i.hm.ipr);
     break;
   case PAL_MF:
     printf( "%10s    ", "mfpr" );
     printf( "%2s", reg(i.hm.Ra, INT_REG) );
     printipr(i.hm.ipr);
     break;
   case PAL_LD:
     printf( "%10s    ", "hwld" );
     printf( "%2s", reg(i.hls.Ra, INT_REG) );
     printf( ", %d", i.hls.disp );
     printf( "(%2s)", reg( i.hls.Rb, INT_REG) );
     break;
   case PAL_ST:
     printf( "%10s    ", "hwst" );
     printf( "%2s", reg(i.hls.Ra, INT_REG) );
     printf( ", %d", i.hls.disp );
     printf( "(%2s)", reg(i.hls.Rb, INT_REG ) );
     break;
   case PAL_REI:
     printf( "%10s    ", "hwrei" );
     break;
   default:
     unknwn( i );
     return;
   }

}

static char *software_name[]={
  "v0",
  "t0",
  "t1",
  "t2",

  "t3",
  "t4",
  "t5",
  "t6",

  "t7",
  "s0",
  "s1",
  "s2",

  "s3",
  "s4",
  "s5",
  "fp",

  "a0",
  "a1",
  "a2",
  "a3",

  "a4",
  "a5",
  "t8",
  "t9",

  "t10",
  "t11",
  "ra",
  "pv",

  "at",
  "gp",
  "sp",
  "zero",
};

static char regstr[6];
/* WARNING:  Don't call this routine more than once in the same statement! */
static char *reg(int r , int type)
{
  if (type == FP_REG) {
      sprintf(regstr, "f%d", r);
      return regstr;
  }

  if (regmode) return( software_name[r] );

  sprintf(regstr, "r%d", r);
  return regstr;
}

static void printipr( int idx )
{
  int i;
  for (i=0; ipr_array[i].idx > 0; i++)
    if (idx == ipr_array[i].idx) {
      printf( ", %s", ipr_array[i].name);
      return;
    }
  printf( ", %x", idx );
}
