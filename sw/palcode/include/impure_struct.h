#ifndef	IMPURE_STRUCT_H__LOADED 
#define IMPURE_STRUCT_H__LOADED 
/*----------------------------------------------------------------------
 *       Copyright (c) 1999, Alpha Processor, Inc.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the copyright notice and this permission notice appear
 * in all copies of software and supporting documentation, and that the
 * name of Alpha Processor not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written
 * prior permission. Alpha Processor grants this permission provided that
 * you prominently mark, as not part of the original, any
 * modifications made to this software or documentation.
 *
 * Alpha Processor disclaims all warranties and/or guarantees with regard
 * to this software, including all implied warranties of fitness for a
 * particular purpose and merchantability, and makes no representations
 * regarding the use of, or the results of the use of, the software and
 * documentation in terms of correctness, accuracy, reliability,
 * currentness or otherwise; and you rely on the software, documentation
 * and results solely at your own risk.
 *
 *----------------------------------------------------------------------*/

typedef struct {
    unsigned long FLAG;			/* 0x0 */ 
    unsigned long HALT;			/* 0x8 */ 
    unsigned long R0;			/* 0x10 */ 
    unsigned long R1;			/* 0x18 */ 
    unsigned long R2;			/* 0x20 */ 
    unsigned long R3;			/* 0x28 */ 
    unsigned long R4;			/* 0x30 */ 
    unsigned long R5;			/* 0x38 */ 
    unsigned long R6;			/* 0x40 */ 
    unsigned long R7;			/* 0x48 */ 
    unsigned long R8;			/* 0x50 */ 
    unsigned long R9;			/* 0x58 */ 
    unsigned long R10;			/* 0x60 */ 
    unsigned long R11;			/* 0x68 */ 
    unsigned long R12;			/* 0x70 */ 
    unsigned long R13;			/* 0x78 */ 
    unsigned long R14;			/* 0x80 */ 
    unsigned long R15;			/* 0x88 */ 
    unsigned long R16;			/* 0x90 */ 
    unsigned long R17;			/* 0x98 */ 
    unsigned long R18;			/* 0xa0 */ 
    unsigned long R19;			/* 0xa8 */ 
    unsigned long R20;			/* 0xb0 */ 
    unsigned long R21;			/* 0xb8 */ 
    unsigned long R22;			/* 0xc0 */ 
    unsigned long R23;			/* 0xc8 */ 
    unsigned long R24;			/* 0xd0 */ 
    unsigned long R25;			/* 0xd8 */ 
    unsigned long R26;			/* 0xe0 */ 
    unsigned long R27;			/* 0xe8 */ 
    unsigned long R28;			/* 0xf0 */ 
    unsigned long R29;			/* 0xf8 */ 
    unsigned long R30;			/* 0x100 */ 
    unsigned long R31;			/* 0x108 */ 
    unsigned long F0;			/* 0x110 */ 
    unsigned long F1;			/* 0x118 */ 
    unsigned long F2;			/* 0x120 */ 
    unsigned long F3;			/* 0x128 */ 
    unsigned long F4;			/* 0x130 */ 
    unsigned long F5;			/* 0x138 */ 
    unsigned long F6;			/* 0x140 */ 
    unsigned long F7;			/* 0x148 */ 
    unsigned long F8;			/* 0x150 */ 
    unsigned long F9;			/* 0x158 */ 
    unsigned long F10;			/* 0x160 */ 
    unsigned long F11;			/* 0x168 */ 
    unsigned long F12;			/* 0x170 */ 
    unsigned long F13;			/* 0x178 */ 
    unsigned long F14;			/* 0x180 */ 
    unsigned long F15;			/* 0x188 */ 
    unsigned long F16;			/* 0x190 */ 
    unsigned long F17;			/* 0x198 */ 
    unsigned long F18;			/* 0x1a0 */ 
    unsigned long F19;			/* 0x1a8 */ 
    unsigned long F20;			/* 0x1b0 */ 
    unsigned long F21;			/* 0x1b8 */ 
    unsigned long F22;			/* 0x1c0 */ 
    unsigned long F23;			/* 0x1c8 */ 
    unsigned long F24;			/* 0x1d0 */ 
    unsigned long F25;			/* 0x1d8 */ 
    unsigned long F26;			/* 0x1e0 */ 
    unsigned long F27;			/* 0x1e8 */ 
    unsigned long F28;			/* 0x1f0 */ 
    unsigned long F29;			/* 0x1f8 */ 
    unsigned long F30;			/* 0x200 */ 
    unsigned long F31;			/* 0x208 */ 
    unsigned long MCHKFLAG;		/* 0x210 */ 
    unsigned long IMPURE;		/* 0x218 */ 
    unsigned long WHAMI;		/* 0x220 */ 
    unsigned long SCC;			/* 0x228 */ 
    unsigned long PRBR;			/* 0x230 */ 
    unsigned long PTBR;			/* 0x238 */ 
    unsigned long TRAP;			/* 0x240 */ 
    unsigned long HALT_CODE;		/* 0x248 */ 
    unsigned long KSP;			/* 0x250 */ 
    unsigned long SCBB;			/* 0x258 */ 
    unsigned long PCBB;			/* 0x260 */ 
    unsigned long VPTB;			/* 0x268 */ 
    unsigned long SROM_REV;		/* 0x270 */ 
    unsigned long PROC_ID;		/* 0x278 */ 
    unsigned long MEM_SIZE;		/* 0x280 */ 
    unsigned long CYCLE_CNT;		/* 0x288 */ 
    unsigned long SIGNATURE;		/* 0x290 */ 
    unsigned long PROC_MASK;		/* 0x298 */ 
    unsigned long SYSCTX;		/* 0x2a0 */ 
    unsigned long PLACE_HOLDER18;	/* 0x2a8 */ 
    unsigned long PLACE_HOLDER19;	/* 0x2b0 */ 
    unsigned long PLACE_HOLDER20;	/* 0x2b8 */ 
    unsigned long PLACE_HOLDER21;	/* 0x2c0 */ 
    unsigned long PLACE_HOLDER22;	/* 0x2c8 */ 
    unsigned long PLACE_HOLDER23;	/* 0x2d0 */ 
    unsigned long P4;			/* 0x2d8 */ 
    unsigned long P5;			/* 0x2e0 */ 
    unsigned long P6;			/* 0x2e8 */ 
    unsigned long P7;			/* 0x2f0 */ 
    unsigned long P20;			/* 0x2f8 */ 
    unsigned long P_TEMP;		/* 0x300 */ 
    unsigned long P_MISC;		/* 0x308 */ 
    unsigned long P23;			/* 0x310 */ 
    unsigned long FPCR;			/* 0x318 */ 
    unsigned long VA;			/* 0x320 */ 
    unsigned long VA_CTL;		/* 0x328 */ 
    unsigned long EXC_ADDR;		/* 0x330 */ 
    unsigned long IER_CM;		/* 0x338 */ 
    unsigned long SIRR;			/* 0x340 */ 
    unsigned long ISUM;			/* 0x348 */ 
    unsigned long EXC_SUM;		/* 0x350 */ 
    unsigned long PAL_BASE;		/* 0x358 */ 
    unsigned long I_CTL;		/* 0x360 */ 
    unsigned long PCTR_CTL;		/* 0x368 */ 
    unsigned long PROCESS_CONTEXT;	/* 0x370 */ 
    unsigned long I_STAT;		/* 0x378 */ 
    unsigned long DTB_ALT_MODE;		/* 0x380 */ 
    unsigned long MM_STAT;		/* 0x388 */ 
    unsigned long M_CTL;		/* 0x390 */ 
    unsigned long DC_CTL;		/* 0x398 */ 
    unsigned long DC_STAT;		/* 0x3a0 */ 
    unsigned long WRITE_MANY;		/* 0x3a8 */ 
    unsigned long AUTOBAUD;		/* 0x3b0 */ 
    unsigned long BEHAV_DATA;		/* 0x3b8 */ 
} ImpureRgn_t;

#endif 		/* IMPURE_STRUCT_H__LOADED */
