
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
 *  $Id: dis.h,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $;
 */

/*
 * $Log: dis.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.8  1995/12/01  22:23:41  cruz
 * Removed prototypes for delete functions.
 *
 * Revision 1.7  1995/10/26  21:32:26  cruz
 * Made local variable static
 *
 * Revision 1.6  1995/10/18  12:54:18  fdh
 * Removed unused flags arguments.
 *
 * Revision 1.5  1994/12/07  19:14:40  cruz
 * Added IPR names for DC21164
 *
 * Revision 1.4  1994/08/05  20:18:01  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/06/29  14:10:22  rusling
 * Added floating point disassembly.
 *
 * Revision 1.2  1994/06/23  13:45:15  rusling
 * Fixed up WNT compiler warnings.
 *
 * Revision 1.1  1994/06/17  19:40:17  fdh
 * Initial revision
 *
 */

#define INT_REG 0
#define FP_REG 1

struct op_lit{
  ui LIT: 8;
};

struct op_reg{
  ui SBZ: 3;
  ui Rb:  5;
};

union op_var{
  struct op_lit ol;
  struct op_reg or;
};

struct op_i{
  ui Rc:   5;
  ui func: 7;
  ui op_ty: 1;
  ui op_var: 8;
  ui Ra:   5;
  ui opcode: 6;
};

struct gen_i{
  ui junk: 26;
  ui opcode: 6;
};

struct branch_i{
  int br_disp: 21;
  ui Ra:      5;
  ui opcode:  6;
};

struct mem_i{
  ui mem_disp: 16;
  ui Rb:       5;
  ui Ra:       5;
  ui opcode:   6;
};
#ifdef DC21164
struct hwm_i{
  ui ipr:    16;
  ui Rb:     5;
  ui Ra:     5;
  ui opcode: 6;
};
struct hwls_i{
  ui disp:   10;
  ui cond:    1;  
  ui mbz2:    1;  
  ui qw:      1;
  ui mbz1:    1;  
  ui alt:     1;
  ui phy:     1;
  ui Rb:      5;
  ui Ra:      5;
  ui opcode:  6;
};
#else
struct hwm_i{
  ui ipr:    8;
  ui fill1:  8;
  ui Rb:     5;
  ui Ra:     5;
  ui opcode: 6;
};
struct hwls_i{
  ui disp:   12;
  ui qw:      1;
  ui rwc:     1;
  ui alt:     1;
  ui phy:     1;
  ui Rb:      5;
  ui Ra:      5;
  ui opcode:  6;
};
#endif
struct pal_i{
  ui palcode: 26;
  ui opcode:   6;
};

struct float_i{
  ui Fc:      5;
  ui func:    11;
  ui Fb:      5;
  ui Fa:      5;
  ui opcode:  6;
};

union instruction{
  ul full;
  struct gen_i    g;
  struct branch_i b;
  struct op_i     o;
  struct mem_i    m;
  struct float_i  f;
  struct pal_i    p;
  struct hwm_i    hm;
  struct hwls_i   hls;
  char   ascii[4];
};

#ifdef DC21164
static struct ipr_struct{
  int idx;
  char *name;
} ipr_array[] = {
  {0x100,		"isr"},
  {0x101,		"itbTag"},
  {0x102,		"itbPte"},
  {0x103,		"itbAsn"},
  {0x104,		"itbPteTemp"},
  {0x105,		"itbIa"},
  {0x106,		"itbIap"},
  {0x107,		"itbIs"},
  {0x108,		"sirr"},
  {0x109,		"astrr"},
  {0x10A,		"aster"},
  {0x10B,		"excAddr"},
  {0x10C,		"excSum"},
  {0x10D,		"excMask"},
  {0x10E,		"palBase"},
  {0x10F,		"ps"},
  {0x110,		"ipl"},
  {0x111,		"intid"},
  {0x112,		"ifaultVaForm"},
  {0x113,		"ivptbr"},
  {0x115,		"hwIntClr"},
  {0x116,		"slXmit"},
  {0x117,		"slRcv"},
  {0x118,		"icsr"},
  {0x119,		"icFlushCtl"},
  {0x11A,		"icPerrStat"},
  {0x11C,		"pmctr"},
  {0x140,		"pt0"},
  {0x141,		"pt1"},
  {0x142,		"pt2"},
  {0x143,		"pt3"},
  {0x144,		"pt4"},
  {0x145,		"pt5"},
  {0x146,		"pt6"},
  {0x147,		"pt7"},
  {0x148,		"pt8"},
  {0x149,		"pt9"},
  {0x14A,		"pt10"},
  {0x14B,		"pt11"},
  {0x14C,		"pt12"},
  {0x14D,		"pt13"},
  {0x14E,		"pt14"},
  {0x14F,		"pt15"},
  {0x150,		"pt16"},
  {0x151,		"pt17"},
  {0x152,		"pt18"},
  {0x153,		"pt19"},
  {0x154,		"pt20"},
  {0x155,		"pt21"},
  {0x156,		"pt22"},
  {0x157,		"pt23"},
  {0x200,		"dtbAsn"},
  {0x201,		"dtbCm"},
  {0x202,		"dtbTag"},
  {0x203,		"dtbPte"},
  {0x204,		"dtbPteTemp"},
  {0x205,		"mmStat"},
  {0x206,		"va"},
  {0x207,		"vaForm"},
  {0x208,		"mvptbr"},
  {0x209,		"dtbIap"},
  {0x20A,		"dtbIa"},
  {0x20B,		"dtbIs"},
  {0x20C,		"altMode"},
  {0x20D,		"cc"},
  {0x20E,		"ccCtl"},
  {0x20F,		"mcsr"},
  {0x210,		"dcFlush"},
  {0x212,		"dcPerrStat"},
  {0x213,		"dcTestCtl"},
  {0x214,		"dcTestTag"},
  {0x215,		"dcTestTagTemp"},
  {0x216,		"dcMode"},
  {0x217,		"mafMode"},
  {-1, ""}
  };
#else
static struct ipr_struct{
  int idx;
  char *name;
} ipr_array[] = {
  {0x20+0x0,		"tbTag"},
  {0x20+0x1,		"itbPte"},
  {0x20+0x2,		"iccsr"},
  {0x20+0x3,		"itbPteTemp"},
  {0x20+0x4,		"excAddr"},
  {0x20+0x5,		"slRcv"},
  {0x20+0x6,		"itbZap"},
  {0x20+0x7,		"itbAsm"},
  {0x20+0x8,		"itbIs"},
  {0x20+0x9,		"ps"},
  {0x20+0xA,		"excSum"},
  {0x20+0xB,		"palBase"},
  {0x20+0xC,		"hirr"},
  {0x20+0xD,		"sirr"},
  {0x20+0xE,		"astrr"},
  {0x20+0x10,		"hier"},
  {0x20+0x11,		"sier"},
  {0x20+0x12,		"aster"},
  {0x20+0x13,		"slClr"},
  {0x20+0x16,		"slXmit"},
  {0x40+0x0,		"dtbCtl"},
  {0x40+0x0,		"tbCtl"},
  {0x40+0x2,		"dtbPte"},
  {0x40+0x3,		"dtbPteTemp"},
  {0x40+0x4,		"mmcsr"},
  {0x40+0x5,		"va"},
  {0x40+0x6,		"dtbZap"},
  {0x40+0x7,		"dtbAsm"},
  {0x40+0x8,		"dtbIs"},
  {0x40+0x9,		"biuAddr"},
  {0x40+0xA,		"biuStat"},
  {0x40+0xB,		"dcAddr"},
  {0x40+0xC,		"dcStat"},
  {0x40+0xD,		"fillAddr"},
  {0x40+0xE,		"aboxCtl"},
  {0x40+0xF,		"altMode"},
  {0x40+0x10,		"cc"},
  {0x40+0x11,		"ccCtl"},
  {0x40+0x12,		"biuCtl"},
  {0x40+0x13,		"fillSyndrome"},
  {0x40+0x14,		"bcTag"},
  {0x40+0x15,		"flushIc"},
  {0x40+0x17,		"flushIcAsm"},
  {0x40+0x20+0x6,	"xtbZap"},
  {0x40+0x20+0x7,	"xtbAsm"},
  {0x80+0x0,		"pt0"},
  {0x80+0x1,		"pt1"},
  {0x80+0x2,		"pt2"},
  {0x80+0x3,		"pt3"},
  {0x80+0x4,		"pt4"},
  {0x80+0x5,		"pt5"},
  {0x80+0x6,		"pt6"},
  {0x80+0x7,		"pt7"},
  {0x80+0x8,		"pt8"},
  {0x80+0x9,		"pt9"},
  {0x80+0xA,		"pt10"},
  {0x80+0xB,		"pt11"},
  {0x80+0xC,		"pt12"},
  {0x80+0xD,		"pt13"},
  {0x80+0XE,		"pt14"},
  {0x80+0xF,		"pt15"},
  {0x80+0x10,		"pt16"},
  {0x80+0x11,		"pt17"},
  {0x80+0x12,		"pt18"},
  {0x80+0x13,		"pt19"},
  {0x80+0x14,		"pt20"},
  {0x80+0x15,		"pt21"},
  {0x80+0x16,		"pt22"},
  {0x80+0x17,		"pt23"},
  {0x80+0x18,		"pt24"},
  {0x80+0x19,		"pt25"},
  {0x80+0x1A,		"pt26"},
  {0x80+0x1B,		"pt27"},
  {0x80+0x1C,		"pt28"},
  {0x80+0x1D,		"pt29"},
  {0x80+0x1E,		"pt30"},
  {0x80+0x1F,		"pt31"},
  {-1, ""}
  };
#endif

#define unknwn(i)   printf( "    UNKNWN    " );

static void printipr(int idx);
static char *reg(int r , int type);
static void dis_instruct(ui instr , ul addr);
static void dis_branch(union instruction i , ul addr);
static void dis_mem(union instruction i);
static void dis_opr(union instruction i);
static void dis_float(union instruction i);
static void dis_ieee(union instruction i);
static void dis_vax(union instruction i);
static void dis_pal(union instruction i);
static void dis_memdisp(union instruction i);
static void dis_memsp(union instruction i);
static void dis_memjmp(union instruction i);
static void dis_palopc(union instruction i);
static void fmt_mem(char * opc , union instruction i, int reg_type );
static void dis_opr10(union instruction i);
static void dis_opr11(union instruction i);
static void dis_opr12(union instruction i);
static void dis_opr13(union instruction i);
static void opr_fmt(char * opc , union instruction i);
