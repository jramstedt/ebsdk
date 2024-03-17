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
**
** FACILITY:	
**
**	ED64 Software Tools - srom
** 
** FUNCTIONAL DESCRIPTION:
** 
**      Given an object file, generate output in the form
**      required for inclusion in an srom.
** 
** CALLING ENVIRONMENT: 
**
**	user mode
** 
** AUTHOR: Anonymous
**
** CREATION-DATE: 1992
** 
** MODIFIED BY: 
** 
**      Who	When		What
**	-----	-----------	---------------------------------------------
**      DAR     09-Mar-1993     Added test for code size in readCode()
**   	DAR	09-Jul-1993	modified to build on 32 bit unix systems
*
* $Log: srom.c,v $
* Revision 1.1.1.1  1998/12/29 21:36:24  paradis
* Initial CVS checkin
*
 * Revision 4.16  1997/06/10  18:15:41  bissen
 * Modified the code to limit the size of the Icache on a 21164PC
 * to 8KBytes.  This is the limit that the 21164PC (PCA56) can
 * load from SROM.
 *
 * Revision 4.14  1997/02/21  20:47:12  fdh
 * Always pad 21164 SROMs to fill up icache.
 * Corrected seen indexing.
 * Report version for verbose mode.
 *
 * Revision 4.13  1996/04/04  21:02:40  cruz
 * Changed the readCode routine to compute the number of
 * instructions to use for padding for EV5 instead of
 * always assuming 44.
 * Also, rewrote the loop that read the code in to correct
 * a possible bug that would have occurred when the input
 * data was larger than the allocated array.
 *
 * Revision 4.12  1995/11/14  23:18:48  cruz
 * Undo the last change.
 *
 * Revision 4.10  1995/11/14  18:24:58  cruz
 * Cleaned up code with lint.
 *
 * Revision 4.9  1995/07/10  18:23:48  cruz
 * Disabled the advanced options and padding.
 *
 * Revision 4.8  1995/04/21  14:35:44  cruz
 * Fixed a bug in the part of the code that figures out what
 * the output filename should be.  This was a bug that was
 * there from the beginning and just showed up now after
 * another malloc was done under NT.
 *
*/
#ifndef LINT
static char *rcsid = "$Id: srom.c,v 1.1.1.1 1998/12/29 21:36:24 paradis Exp $";
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define IC_MAX_SIZE_BY (16 * 1024)    /* The largest ICache so far is for EV45 */
#define IC_MAX_SIZE_LW (IC_MAX_SIZE_BY / 4)
 
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

char cpu[15];			        /* Identifies target cpu. */
unsigned int icache_size;               /* ICache size for selected CPU */
unsigned int maxIcPtr;                  /* Maximum ptr for input buffer */
int *icCode;	                	/* Pointer to input buffer. */
char *srom;				/* Formatted srom bit image */
unsigned int sromPtr;			/* ptr into srom */
unsigned int ic_fmt_size_by;            /* # of bytes needed for storing */
    					/* formatted srom image.	*/
int bytes_per_rec;                      /* Number of bytes per hex record */
int padinstr;
int verbose;
int overflow;
int padsrom, padsrom_by;
int	tagvalfillbits;
int	tagfillmapbits;
int	tvalids;

/************************************************************************
 *      	        EV5 SPECIFIC VARIABLES    			*
 ************************************************************************/
int tphysical, bht, base, asm, asn, tag;

/* fillmap [0 - 127] maps data 127:0, etc. */
/* fillmap[n] is bit position in output vector.  bit 0 of this vector is first-in;
   bit 199 is last */

int dfillmap [128] = {
                                      /* data 0:127 -- fillmap[0:127]*/
  42,44,46,48,50,52,54,56,            /* 0:7 */
  58,60,62,64,66,68,70,72,            /* 8:15 */
  74,76,78,80,82,84,86,88,            /* 16:23 */
  90,92,94,96,98,100,102,104,         /* 24:31 */
  43,45,47,49,51,53,55,57,            /* 32:39 */
  59,61,63,65,67,69,71,73,            /* 40:47 */
  75,77,79,81,83,85,87,89,            /* 48:55 */
  91,93,95,97,99,101,103,105,         /* 56:63 */
  128,130,132,134,136,138,140,142,    /* 64:71 */
  144,146,148,150,152,154,156,158,    /* 72:79 */
  160,162,164,166,168,170,172,174,    /* 80:87 */
  176,178,180,182,184,186,188,190,    /* 88:95 */
  129,131,133,135,137,139,141,143,    /* 96:103 */
  145,147,149,151,153,155,157,159,    /* 104:111 */
  161,163,165,167,169,171,173,175,    /* 112:119 */
  177,179,181,183,185,187,189,191     /* 120:127 */
  };

int BHTfillmap[8] = {           /* BHT vector 0:7 -- BHTfillmap[0:7] */
  199,198,197,196,195,194,193,192     /* 0:7 */
  };

int predfillmap[20] = {         /* predecodes 0:19 -- predfillmap[0:19] */
  106,108,110,112,114,                /* 0:4 */
  107,109,111,113,115,                /* 5:9 */
  118,120,122,124,126,                /* 10:14 */
  119,121,123,125,127                 /* 15:19 */
  };

int octawpfillmap =             /* octaword parity */
  117;

int predpfillmap =              /* predecode parity */
  116;

int tagfillmap[30] = {          /* tag bits 13:42 -- tagfillmap[0:29] */
  29,28,27,26,25,24,23,22,21,20,      /* 13:22 */
  19,18,17,16,15,14,13,12,11,10,      /* 23:32 */
   9, 8, 7, 6, 5, 4, 3, 2, 1, 0       /* 33:42 */
  };

int asnfillmap[7] = {           /* asn 0:6 -- asnfillmap[0:6] */
  37,36,35,34,33,32,31                /* 0:6 */
  };

int asmfillmap =                /* asm -- asmfillmap */
  30;

int tagphysfillmap =            /* tagphysical address -- tagphysfillmap */
  38;

int tagvalfillmap[4] = {        /* tag valid bits 0:3 -- tagvalfillmap */
  40,39,38,37                   /* 0:3 */
  };
  
int tagparfillmap =             /* tag parity -- tagparfillmap */
  41;                                 



/************************************************************************
 *      	        EV5 SPECIFIC ROUTINES    			*
 ************************************************************************/
int instrpredecode(int inst)
{

int result;
int opcode;
int func;
int jsr_type;
int ra;

int out0;
int out1;
int out2;
int out3;
int out4;
int e0_only;
int  e1_only;
int  ee;
int lnoop;
int  fadd;
int  fmul;
int  fe;
int  br_type;
int  ld;
int  store;
int  br;
int  call_pal;
int  bsr;
int  ret_rei;
int  jmp;
int  jsr_cor;
int  jsr;
int  cond_br;

#define EXT(data, bit)\
    (((data) & ((unsigned) 1 << (bit))) != 0)
#define EXTV(data, hbit, lbit)\
    (((data) >> (lbit)) & \
     ((((hbit) - (lbit) + 1) == 32) ? ((unsigned)0xffffffff) : (~((unsigned)0xffffffff << ((hbit) - (lbit) + 1)))))
#define INS(name, bit, data)\
  (name) = (((name) & ~((unsigned) 1 << (bit))) | \
	    (((unsigned) (data) << (bit)) & ((unsigned) 1 << (bit))))


 opcode = EXTV(inst, 31, 26 );
 func   = EXTV(inst, 12, 5);
 jsr_type = EXTV(inst, 15,14); 
 ra = EXTV(inst,25,21);

e0_only  = (opcode == 0x24) ||		    /* STF */
	   (opcode == 0x25) ||		    /* STG */
	   (opcode == 0x26) ||		    /* STS */
	   (opcode == 0x27) ||		    /* STT */
	   (opcode == 0x0F) ||		    /* STQ_U */
	   (opcode == 0x2A) ||		    /* LDL_L */
	   (opcode == 0x2B) ||		    /* LDQ_L */
	   (opcode == 0x2C) ||		    /* STL */
	   (opcode == 0x2D) ||		    /* STQ */
	   (opcode == 0x2E) ||		    /* STL_C */
	   (opcode == 0x2F) ||		    /* STQ_C */
	   (opcode == 0x1F) ||		    /* HW_ST*/
	   (opcode == 0x18) ||		    /* MISC mem format: FETCH/_M, RS, RC, RPCC, TRAPB, MB) */
	   (opcode == 0x12) ||		    /* EXT,MSK,INS,SRX,SLX,ZAP*/
	   (opcode == 0x13) ||		    /* MULX */
	   ((opcode == 0x1D) && (EXT(inst,8) == 0)) || /* MBOX HW_MTPR */
	   ((opcode == 0x19) && (EXT(inst,8) == 0)) || /* MBOX HW_MFPR */	
	   (opcode == 0x01) ||              /* VR::: might change this later RESDEC's */
	   (opcode == 0x02) ||              /* RESDEC's */
	   (opcode == 0x03) ||              /* RESDEC's */
	   (opcode == 0x04) ||              /* RESDEC's */
	   (opcode == 0x05) ||              /* RESDEC's */
	   (opcode == 0x06) ||              /* RESDEC's */
	   (opcode == 0x07) ||              /* RESDEC's */
	   (opcode == 0x0a) ||              /* RESDEC's */
	   (opcode == 0x0c) ||              /* RESDEC's */
	   (opcode == 0x0d) ||		    /* RESDEC's */
	   (opcode == 0x0e) ||		    /* RESDEC's */
	   (opcode == 0x14) ||		    /* RESDEC's */
	   (opcode == 0x1c);		    /* RESDEC's */

e1_only =  (opcode == 0x30) ||		    /* BR */
	   (opcode == 0x34) ||		    /* BSR */
	   (opcode == 0x38) ||		    /* BLBC */
	   (opcode == 0x39) ||		    /* BEQ */
	   (opcode == 0x3A) ||		    /* BLT */
	   (opcode == 0x3B) ||		    /* BLE */
	   (opcode == 0x3C) ||		    /* BLBS */
	   (opcode == 0x3D) ||		    /* BNE */
	   (opcode == 0x3E) ||		    /* BGE */
	   (opcode == 0x3F) ||		    /* BGT */
	   (opcode == 0x1A) ||		    /* JMP,JSR,RET,JSR_COROT */
	   (opcode == 0x1E) ||		    /* HW_REI */
	   (opcode == 0x00) ||		    /* CALL_PAL */
	   ((opcode == 0x1D) && (EXT(inst,8) == 1)) || /* IBOX HW_MTPR */
	   ((opcode == 0x19) && (EXT(inst,8) == 1));   /* IBOX HW_MTPR */	

ee =       (opcode == 0x10) ||		    /* ADD, SUB, CMP */
	   (opcode == 0x11) ||		    /* AND, BIC etc. logicals */
	   (opcode == 0x28) ||		    /* LDL */
	   (opcode == 0x29) ||		    /* LDQ */
	   (opcode == 0x0B)&(ra != 0x1F) ||  /* LDQ_U */
	   (opcode == 0x08) ||		    /* LDA */
	   (opcode == 0x09) ||		    /* LDAH */
	   (opcode == 0x20) ||		    /* LDF */
	   (opcode == 0x21) ||		    /* LDG */
	   (opcode == 0x22) ||		    /* LDS */
	   (opcode == 0x23) ||		    /* LDT */
	   (opcode == 0x1B);		    /* HW_LD */


lnoop =	   (opcode == 0x0B)&(ra == 0x1F);    /* LDQ_U R31, x(y) - NOOP*/

fadd =	   ((opcode == 0x17) && (func != 0x20)) ||	    /* Flt, datatype indep excl CPYS */
	   ((opcode == 0x15) && ((func & 0xf) != 0x2)) ||   /* VAX excl MUL's */
	   ((opcode == 0x16) && ((func & 0xf) != 0x2)) ||   /* IEEE excl MUL's */
	   (opcode == 0x31) ||		    /* FBEQ */
	   (opcode == 0x32) ||		    /* FBLT */
	   (opcode == 0x33) ||		    /* FBLE */
	   (opcode == 0x35) ||		    /* FBNE */
	   (opcode == 0x36) ||		    /* FBGE */
	   (opcode == 0x37);		    /* FBGT */
	   
fmul = 	   ((opcode == 0x15) && ((func & 0xf) == 0x2)) ||   /* VAX MUL's */
	   ((opcode == 0x16) && ((func & 0xf) == 0x2));	    /* IEEE MUL's */

fe =       ((opcode == 0x17) && (func == 0x20));    /*  CPYS */

br_type =  ((opcode & 0x30) == 0x30) ||   /* all branches */
	   (opcode == 0x1A) ||		  /* JMP's */
	   (opcode == 0x00) ||		  /* CALL PAL */
	   (opcode == 0x1E);		  /* HW_REI */

ld =	   (opcode == 0x28) ||		  /* LDL */
	   (opcode == 0x29) ||		  /* LDQ */
/*	   (opcode == 0x2A) ||		     LDL_L */
/*	   (opcode == 0x2B) ||		     LDQ_L */
	   (opcode == 0x0B) ||		  /* LDQ_U */
	   (opcode == 0x20) ||		  /* LDF */
	   (opcode == 0x21) ||		  /* LDG */
	   (opcode == 0x22) ||		  /* LDS */
	   (opcode == 0x23) ||		  /* LDT */
	   (opcode == 0x1B);		  /* HW_LD */

store =	   (opcode == 0x24) ||		  /* STF */
	   (opcode == 0x25) ||		  /* STG */
	   (opcode == 0x26) ||		  /* STS */
	   (opcode == 0x27) ||		  /* STT */
	   (opcode == 0x0F) ||		  /* STQ_U */
	   (opcode == 0x2C) ||		  /* STL */
	   (opcode == 0x2D) ||		  /* STQ */
	   (opcode == 0x2E) ||		  /* STL_C */
	   (opcode == 0x2F) ||		  /* STQ_C */
	   (opcode == 0x18) ||		  /* Misc: TRAPB, MB, RS, RC, RPCC etc. */
           (opcode == 0x1F) ||		  /* HW_ST */
	   (opcode == 0x2A) ||		  /* LDL_L */
	   (opcode == 0x2B);		  /* LDQ_L */


br =	   (opcode == 0x30);		  /* all branches */

call_pal = (opcode == 0x00);		  /* call PAL */
    
bsr =	   (opcode == 0x34);

ret_rei =  ((opcode == 0x1A) && (jsr_type == 0x2)) ||
	   ((opcode == 0x1E) && (jsr_type != 0x3));

jmp = ((opcode == 0x1A) && (jsr_type == 0x0));

jsr_cor = ((opcode == 0x1A) && (jsr_type == 0x3));

jsr = ((opcode == 0x1A) && (jsr_type == 0x1));

cond_br = (opcode == 0x31) ||
	  (opcode == 0x32) ||
	  (opcode == 0x33) ||
	  (opcode == 0x35) ||
	  (opcode == 0x36) ||
	  (opcode == 0x37) ||
	  (opcode == 0x38) ||
	  (opcode == 0x39) ||
	  (opcode == 0x3A) ||
	  (opcode == 0x3B) ||
	  (opcode == 0x3C) ||
	  (opcode == 0x3D) ||
	  (opcode == 0x3E) ||
	  (opcode == 0x3F);


out0 = br || bsr || jmp || jsr || (ee && !ld) || (e0_only && !store);
out1 = ret_rei ||(e1_only && !br_type)|| jmp ||jsr_cor|| jsr || lnoop || (fadd && !br_type) || fe;; 
out2 = call_pal || bsr || jsr_cor || e0_only ||jsr ||fmul || fe;
out3 = (e1_only && cond_br) || (e1_only && !br_type) || fadd || fmul || fe;
out4 = ee || lnoop || e0_only || fadd || fmul || fe;

 result = 0;
 INS( result, 0, out0 );
 INS( result, 1, out1 );
 INS( result, 2, out2 ); 
 INS( result, 3, out3 );
 INS( result, 4, out4 );

return (result);
}

int eparity(int x) 
{
  x = x ^ (x >> 16);
  x = x ^ (x >> 8);
  x = x ^ (x >> 4);
  x = x ^ (x >> 2);
  x = x ^ (x >> 1);
  return (x&1);
}

void build_vector(int instr[], int outvector[])
{
  int j, k, t;
  int tparity, pdparity, predecodes, owparity;

  tparity = eparity(tag) ^ eparity(tphysical) ^ eparity(asn);
  predecodes=0;
  owparity = 0;

  for (j=0;j<8;j++) outvector[j]=0;

  for (j=0;j<4;j++) {
    predecodes |= (4 ^ instrpredecode(instr[j])) << (j*5);
    owparity ^= eparity(instr[j]);  /* invert bit 2 to match fill scan chain attribute */
  }

  pdparity = eparity(predecodes);

  /* bht */
  for (j=0;j<7;j++) {
    t = BHTfillmap[j];
    outvector[t>>5] |= ((bht >> j) & 1) << (t&0x1f);
  }

  /* instructions */
  for (k=0;k<4;k++) {
    for (j=0;j<32;j++) {
      t = dfillmap[j+k*32];
      outvector[t>>5] |= ((instr[k] >> j) & 1) << (t&0x1f);
    }
  }

  /* predecodes */
  for (j=0;j<20;j++) {
    t = predfillmap[j];
    outvector[t>>5] |= ((predecodes >> j) & 1) << (t&0x1f);
  }

  /* owparity */
  outvector[octawpfillmap>>5] |= owparity << ((octawpfillmap)&0x1f);
  
  /* pdparity */
  outvector[predpfillmap>>5] |= pdparity << ((predpfillmap)&0x1f);

  /* tparity */
  outvector[tagparfillmap>>5] |= tparity << ((tagparfillmap)&0x1f);

  /* tvalids */
  for (j=0;j<tagvalfillbits;j++){
    t =tagvalfillmap[j];
    outvector[t>>5] |= ((tvalids >> j) & 1) << (t&0x1f);
  }

  /* tphysical */
  outvector[tagphysfillmap>>5] |=  tphysical << ((tagphysfillmap)&0x1f);

  /* asn */
  for (j=0;j<7;j++){
    t = asnfillmap[j];
    outvector[t>>5] |= ((asn >> j) & 1) << (t&0x1f);
  }
      
  /* asm */
  outvector[asmfillmap>>5] |=  asm << ((asmfillmap)&0x1f);

  /* tag */
  for (j=0;j<tagfillmapbits;j++) {
    t = tagfillmap[j];
    outvector[t>>5] |= ((tag >> j) & 1) << (t&0x1f);
  }
}

void codeToEv5(unsigned int insCnt)
{
    unsigned int cacheLines, i, j;
    int *src, outvector[8]; 
    char *out;

    cacheLines = (insCnt+3) / 4;	/* instruction to cache lines */
    for (i = 0; i < cacheLines; i++) { 
      src = (int *) &icCode [i*4];     /* Format four instrs at a time */
      build_vector (src, outvector);
      out = (char *) outvector;
      for (j = 0; j < (unsigned int)bytes_per_rec; j++)  /* copy translation to output buffer */
        srom[sromPtr++] = out[j];
    }
    sromPtr *= 8;                       /* Compute number of bits.  */
}

/************************************************************************
 *      	        EV4 SPECIFIC ROUTINES    			*
 ************************************************************************/
void putbit(int v, int c)
{
    int i, b;

    if (v == 0) {
	sromPtr += c;
    } else {
	for (i = 0; i < c; i++) {
	    b = v & 1;
	    b <<= (sromPtr & 7);
	    srom[sromPtr >> 3] |= b;
	    sromPtr++;
	    v >>= 1;
	}
    }
}

void codeToEv3(unsigned int insCnt)
{
    unsigned int qw, ip, i, j, d0, d1, t0, t1;

    qw = (insCnt + 1) >> 1;		/* instruction to cache lines */
    ip = 0;

    for (i = 0; i < qw; i++) {
	t0 = t1 = 0;
	d0 = icCode[ip + 0];
	d1 = icCode[ip + 1];
	for (j = 0; j < 32; j++) {
	    t0 = t0 + t0 + (d0 & 1);
	    d0 >>= 1;
	    t1 = t1 + t1 + (d1 & 1);
	    d1 >>= 1;
	}
	;
	putbit(t1, 32);
	putbit(t0, 32);
	ip += 2;
    }
}

void codeToEv4(unsigned int insCnt)
{
    unsigned int cacheLines, ip, i;

    cacheLines = (insCnt + 7) >> 3;	/* instruction to cache lines */
    ip = 0;
    if (verbose) printf("Code takes up %d cache lines\n", cacheLines);

    for (i = 0; i < cacheLines; i++) {
	putbit(icCode[ip + 0], 32);
	putbit(icCode[ip + 2], 32);
	putbit(icCode[ip + 4], 32);
	putbit(icCode[ip + 6], 32);
	putbit(tag, 21);       		/* the tag */
	putbit(asn, 6);			/* the asn */
	putbit(asm, 1);			/* the asm */
	putbit(1, 1);			/* the valid bit */
	putbit(icCode[ip + 1], 32);
	putbit(icCode[ip + 3], 32);
	putbit(icCode[ip + 5], 32);
	putbit(icCode[ip + 7], 32);
	putbit(bht, 8);		/* put the bht bits */
	ip += 8;
    }
}

/************************************************************************
 *      	            COMMON CODE 	   			*
 ************************************************************************/
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int writehex(FILE *hf, int addr, int max_addr)
{
  int  j, checkSum, bytes;
  
  while (addr < max_addr) {
    bytes = MIN(bytes_per_rec,  max_addr - addr);
    checkSum = bytes + 0x00 + (addr & 0xff) + ((addr >> 8) & 0xff);
    fprintf(hf, ":%02X", bytes);
    fprintf(hf, "%02X", (addr >> 8) & 0xff);
    fprintf(hf, "%02X", addr & 0xff);
    fprintf(hf, "00");
    for (j = 0; j < bytes; j++) {
      fprintf(hf, "%02X", srom[addr] & 0xff);
      checkSum += srom[addr++];
    }
    fprintf(hf, "%02X", (0 - checkSum) & 0xff);
    fprintf(hf, "\n");
  }
 return (addr);
}

void binToHex(FILE *hf)
{
    int addr, max_addr;

    addr = 0;
    max_addr = (sromPtr >> 3);               /* Convert bits to bytes */
    if (padsrom) max_addr = MAX(max_addr, padsrom);
    addr = writehex(hf, addr, max_addr);
}
unsigned int fill_icache(unsigned int cs)
{
  unsigned int i;
  for (i = cs; i < maxIcPtr; i++)
    icCode[i] = padinstr;
  return (i-cs);  /* Number of LWs padded */
}

unsigned int readCode(FILE *ef)
{
  unsigned int cnt = 0;
  unsigned int leftover, ev5pad;
  int   data;

  while (TRUE) {
    data = (int) getw(ef);  /* Read data in.    */
    if (feof(ef)) break;    /* Abort on EOF.    */

    if (cnt >= maxIcPtr ) {
      overflow = TRUE;
      maxIcPtr += 16;		/* Increase 16 instructs at a time. */
      if ( (icCode = (int *) realloc (icCode, maxIcPtr * 4)) == NULL ) {
        fprintf(stderr, "ERROR: Cannot re-allocate space for input buffer.\n");
        exit(1);
      }
    }
    icCode[cnt++] = data;
  }

  fill_icache(cnt);    /* Pad the extra space */
  
  if (!overflow & (strncmp (cpu, "21164", 5) == 0)) {
     leftover = maxIcPtr - cnt;   /* Number of instructions that would fit */
                          /* Need 44 NOPs for adding + 4 for aligning to INT16 */
     ev5pad = (leftover < 48) ? leftover : 48;
     cnt += ev5pad;
     printf("Extra padding added: %d instrs\n", ev5pad);
  }

  return (cnt);   /* Return number of longwords read. */
}


void strtoupper(char *str)
{
  while (*str != '\0') {
    *str = (char) toupper ((int) *str);
    str++;
  }
}

void usage()
{
  printf("\nUsage: srom [options] <input-file> [<output-file>]\n\n");
  printf("Enter all numbers as hex values.\n");
  printf("   -v            Verbose mode on\n");
  printf("   -h            Help, print this text\n");
  printf("   -21064        Generate code for DECChip 21064 (default).\n");
  printf("   -21164        Generate code for DECChip 21164\n");
  printf("   -21164PC      Generate code for DECChip 21164PC\n");
#ifdef SROM_ADVANCED_OPTIONS
  printf("   -append str   Appends string to end of hex file.\n");
  printf("   -icache size  Sets the ICache size.  If not specified, defaults to 8KB.\n");
  printf("   -padsrom size Image will be padded with NOPs to fill an SROM of specified size.\n");
  printf("   -padicache    Image will be padded with NOPs to fill the entire Icache, \n");
  printf("                 regardless of the total SROM size.\n");
  printf("   -padinstr ins Instruction to use if padding is selected.\n");


  printf("\n\t----------- Advanced Options ----------- \n");
  printf("   -base base    Sets base address for code.  The base is used to compute\n");
  printf("                 the TAG value for all instructions.\n");
  printf("   -bht history  Sets the branch history vector for all instructions.\n");
  printf("   -asn value    Sets the TAG ASN for all instructions.\n");
  printf("   -asm value    Sets the ASM bit for all instructions.\n");

  printf("\n\t----------- DC21164 specific ----------- \n");
  printf("   -t 1-bit      Sets the tag physical address bit for all instructions.  Enter as a single bit.\n");
#endif /* SROM_ADVANCED_OPTIONS */
  printf("\n");
  exit(1);
}

unsigned int log2(unsigned int x)
{ 
  unsigned int	y;
  unsigned int result;

  y = 1;
  result = 0;

  while (x > y) {
    result++;
    y = y + y;
  }
  return result;
}

int cvtstr (int argc, char *argv[], int i)
{
int value;
char *ptr;

  if (i >= argc) {
    fprintf (stderr, "ERROR: An argument is expected for option %s.\n", argv[i-1]);
    usage();
  }
  value = strtol(argv[i],&ptr,16);
  if (ptr != (argv[i]+strlen(argv[i]))) {
    fprintf (stderr, "ERROR: Can't convert string \"%s\" in option \"%s\" to a number.\n", 
             argv[i], argv[i-1]);
    usage ();
  }
  return (value);
}

int main(int argc, char *argv[])
{
    FILE *exef, *sromf;
    char *out_file_name = NULL;		/* default output file */
    char *in_file_name = NULL;
    char *arg, *append;
    int i, codeSize, padded;
    int padicache;
#ifdef SROM_ADVANCED_OPTIONS
    int OrigIcPtr;
#endif
    void *ptr;

/* Initialized variables needed. */
    icache_size = 8 * 1024;             /* Assume 8K icache.   */
    strcpy(cpu, "21064");		/* Assume EV4 cpu.	*/
    verbose = FALSE;
    bytes_per_rec = 16;              /* Print 16 data bytes in a rec */
	tagvalfillbits = 2;
	tagfillmapbits = 30;
	tvalids = 3;
    padicache = FALSE;                  /* Don't pad.          */
    padinstr = 0x00000000;              /* NOP instruction.    */
    padsrom = 0;
    asn = 0;			
    asm = 1;
    base = 0;
    bht = 0;
    tphysical = 1;
    append = NULL;
/*
 * Parse arguments, but we are only interested in flags.
 * Skip argv[0].
 */
    i = 1;
    while (i < argc) {
      arg = argv[i];
      if (*arg == '-') {
/*
 * This is a -xyz style options list.  Work out the options specified.
 */
        arg++;			/* skip the '-' */
        strtoupper (arg);           /* Convert to lower case. */
        switch (*arg) {
           case 'V': 		/* verbose mode one */
             verbose = TRUE;
	     printf("srom - version (%s)\n", rcsid);
             break;

#ifdef SROM_ADVANCED_OPTIONS
           case 'P':
             if (strncmp (arg, "PADICACHE", 9) == 0) {
               padicache = TRUE;
               padsrom = 0;
               break;
             }

             i++;
             if (strncmp (arg, "PADINSTR", 8) == 0) {
               padinstr = cvtstr (argc, argv, i);
               break;
             }

             if (strncmp (arg, "PADSROM", 7) == 0) {
               padsrom = cvtstr (argc, argv, i);
               padicache = FALSE;
               break;
             }
             fprintf(stderr, "Unknown option \"%s\"\n", arg);
             usage ();
             break;

  /* The following cases are advanced options */	  
           case 'A': /* asn or asm or append*/
             i++;
             if (strncmp (arg, "ASN", 3) == 0) {
               asn = cvtstr (argc, argv, i);
               break;
             }

             if (strncmp (arg, "ASM", 3) == 0) {
               asm = cvtstr (argc, argv, i);
               break;
             }
             /* Append string ":04200000FFFFFFFFE0" for Xylinx 8K-bytes parts */
             /* Append string ":04400000FFFFFFFFC0" for Xylinx 16K-bytes parts */
             if (strncmp (arg, "APPEND", 6) == 0) {
               if (i >= argc) {
                 fprintf (stderr, "ERROR: An argument is expected for option %s.\n", argv[i-1]);
                 usage();
               }
               append = argv[i];
               break;
             }

             fprintf(stderr, "Unknown option \"%s\"\n", arg);
             usage ();

             break;

             /* Icache Size */
           case 'I':  /* ICache Size */
             i++;

             if (strncmp (arg, "ICACHE", 6) == 0) {
               icache_size = cvtstr (argc, argv, i);
               break;
             }

             fprintf(stderr, "Unknown option \"%s\"\n", arg);
             usage ();
             break;


             /* base address specified (in bytes) or */
           case 'B':  /* branch history table. */
             i++;

             if (strncmp (arg, "BASE", 4) == 0) {
               base = cvtstr (argc, argv, i);
               break;
             }

             if (strncmp (arg, "BHT", 3) == 0) {
               bht = cvtstr (argc, argv, i);
               break;
             }

             fprintf(stderr, "Unknown option \"%s\"\n", arg);
             usage ();
             break;

  /* The following cases are specific to EV5 */             
           case 'T': /* tphysical */
             i++;
             tphysical = cvtstr (argc, argv, i) & 1;
             break;

#endif /* SROM_ADVANCED_OPTIONS */

           case '2':
             strncpy(cpu, arg, 14);
             break;
           default: 
             fprintf(stderr, "Unknown option \"%s\"\n", arg);
           case 'H': 
           case '?': 
             usage();
             break;
           } /* switch */
      } else {
/*
 *  This is the input filename, ignore multiple filenames, just take the 
 *  last one specified.  This is not a rolls-royce interface.
 */
	    if (in_file_name == NULL) {
		in_file_name = arg;

		if (out_file_name == NULL) {
		    char *dot;
		    unsigned int l;

		    dot = (char *) strchr(in_file_name, '.');
		    if (dot != NULL)
			l = (unsigned int) (dot - in_file_name);
		    else
			l = strlen(in_file_name);
#ifndef _WIN32
		    out_file_name = (char *) malloc(l + strlen(".srom")+2);
		    strncpy(out_file_name, in_file_name, l);
		    strcat(out_file_name, ".srom");
#else

		    /* NT and windows can only be guarenteed to cope with 3
		     * character extensions to file names */
		    out_file_name = (char *) malloc(l + strlen(".srm")+2);
		    strncpy(out_file_name, in_file_name, l);
		    strcat(out_file_name, ".srm");
#endif
		}
	    } else
	      out_file_name = arg;
	} 
	i++;
      }

    if (in_file_name == NULL) {
        printf("ERROR: Input filename not supplied.\n");
        usage();
    }

     if (strcmp (cpu, "21164PC") == 0) {
		icache_size = 8 * 1024;		/* Is really 16K Icache */
		asn &= 0x7f;  /* 7 bits for ASN */
		asm &= 0x1;   /* 1 bit for ASM */
		bht &= 0xFF;  /* 8 bits for BHT */
		tvalids = 0xF;
		tag = (base >> log2(icache_size)) & 0x3FFFFFFF;
		tagfillmapbits = 29;
		tagvalfillbits = 4;
		bytes_per_rec = 32;              /* Print 32 data bytes in a rec */
/* for PCA I added 55 bits of padding. One of those bits is reflected in the 
above numbers. */

		for (i=0; i<128; i++)                                                                     
			dfillmap[i]+=56;
		for (i=0; i<8; i++)
			BHTfillmap[i]+=56;
		for (i=0; i<20; i++)
			predfillmap[i]+=56;  
		octawpfillmap+=56;
		predpfillmap+=56;
		for (i=0; i<29; i++)
			tagfillmap[i]+=54;
		for (i=0; i<7; i++)
			asnfillmap[i]+=54;
		asmfillmap+=54;
		tagphysfillmap+=54;     
		for (i=0; i<4; i++)
			tagvalfillmap[i]+=56;
		tagparfillmap+=56;

     } else if (strcmp (cpu, "21164") == 0) {
       asn &= 0x7f;  /* 7 bits for ASN */
       asm &= 0x1;   /* 1 bit for ASM */
       bht &= 0xFF;  /* 8 bits for BHT */
       tag = (base >> log2(icache_size)) & 0x3FFFFFFF;
       bytes_per_rec = 25;              /* Print 25 data bytes in a rec */
     } else {
       asn &= 0x3f;  /* 6 bits for ASN */
       asm &= 0x1;   /* 1 bit for ASM */
       bht &= 0xFF;  /* 8 bits for BHT */
       tag = (base >> log2(icache_size)) & 0x1FFFFF;
     }

    if (verbose == TRUE) {
      printf("\nInput filename     : %s\n", in_file_name);
      printf("Hex output filename: %s\n", out_file_name);
      printf("CPU target         : DC%s\n", cpu);
      printf("   ICache size     : %d bytes (%d LWs)\n", icache_size, icache_size/4);
#ifdef SROM_ADVANCED_OPTIONS
      printf("   Base address    : 0x%08X\n", base);
      printf("   Tag value       : 0x%08X\n", tag);
      printf("   ASN value       : 0x%X\n", asn);
      printf("   ASM bit         : 0x%X\n", asm);
      printf("   Br. hist. table : 0x%X\n", bht);
      if (strcmp (cpu, "21164") == 0) {
        printf("   Tag Phys. Addr  : 0x%X\n", tphysical);
      }
#endif /* SROM_ADVANCED_OPTIONS */

      if (strcmp (cpu, "21164") == 0) {
		padicache = TRUE;	/* 21164 requires all of Icache to contain good parity */
      }

      if (strcmp (cpu, "21164PC") == 0) {
		padicache = TRUE;	/* 21164PC requires all of Icache to contain good parity */
      }

      if (padicache)
        printf("Image will be padded with instruction 0x%08X to fill entire ICache.\n", padinstr);
      if (padsrom)
        printf("Image will be padded with instruction 0x%08X to fill an SROM of size 0x%X bytes.\n", 
               padinstr, padsrom);
    }


    if ((exef = fopen(in_file_name, "rb")) == NULL) {
	fprintf(stderr, "ERROR: Cannot open input file %s\n", in_file_name);
	return (1);
    }

    if ((sromf = fopen(out_file_name, "wb")) == NULL) {
	fprintf(stderr, "ERROR: Cannot open hex output file %s\n", out_file_name);
	fclose(exef);
	return (1);
    }

    if (padsrom) { 
      if (strcmp (cpu, "21164PC") == 0) {
			padsrom_by = (padsrom * 8) / 256 * 16;
	  } else if (strncmp (cpu, "21164", 5) == 0) {
			padsrom_by = (padsrom * 8) / 200 * 16;
	  }	else {
			padsrom_by = (padsrom * 8) / 293 * 32;
	  }
	}

/* 
 *	Allocate space for reading the input file.  We guess the space needed will be
 *	equal to the icache_size unless we're padding the entire ROM in which case
 *      we use the ROM size.  However, we're allowing the code to continue executing
 *      if the boundaries are exceeded by increasing the buffer while reading in the file.
 */
    maxIcPtr = padsrom ? padsrom_by : icache_size;  
    maxIcPtr = (maxIcPtr + 3) / 4;   /* Compute max ptr in LWs */

#ifdef SROM_ADVANCED_OPTIONS
    OrigIcPtr = maxIcPtr;
#endif

    ptr = malloc (maxIcPtr * 4);
    icCode = (int *) ptr;
    if (icCode == NULL ) {
      fprintf(stderr, "ERROR: Cannot allocate space (%d bytes) for input buffer.\n", maxIcPtr*4);
      return (1);
    }

    overflow = FALSE;
    codeSize = readCode(exef);		/* read the exe into the code array */
    printf("\nRead %d LWs (%d bytes) of code\n", codeSize, codeSize*4);
#ifdef SROM_ADVANCED_OPTIONS
    printf("Space left in %s : %d LWs\n", 
           padsrom ? "SROM" : "ICache", (int)(OrigIcPtr - codeSize));
#endif /* SROM_ADVANCED_OPTIONS */
    if (overflow && (strcmp (cpu, "21164PC") != 0)) {         /* Was there an overflow condition? */
      fprintf(stderr, "WARNING: Input code size is larger than the available space in\n");
      fprintf(stderr, "         the %s.  The generated file will not fit in it.\n",
              padsrom ? "SROM" : "ICache");
    }

    padded = fill_icache(codeSize);     /* Pad with instr from end of code loaded to */
                                        /* end of input buffer.                      */

/*
 * 	If we user asked to perform padding AND there was no overflow, then change
 * 	code size to include the padding as well. 
 */
    if (!overflow && (padsrom || padicache)) {
      printf("Padding %d LWs with instruction 0x%08X\n", padded, padinstr);
      codeSize += padded;
    }

/* 
 *	Compute number of bytes needed for storing the formatted image.
 *	Algorithm:  Compute number of icache lines, multiply by the number
 *	of bits per line, round up to the next byte and divide by 8 to compute
 *	byte count.
 */
    i = maxIcPtr * 4;         /* How big should the output buffer be? */
    if (padsrom) i = MAX(i, padsrom); /* Let the output buffer be as large as srom */
	if (strcmp (cpu, "21164PC") == 0) {
		ic_fmt_size_by = (((i/16) * 256) + 7) / 8;
	} else if (strncmp (cpu, "21164", 5) == 0) {
		ic_fmt_size_by = (((i/16) * 200) + 7) / 8;
	} else {
		ic_fmt_size_by = (((i/32) * 293) + 7) / 8;
	}

    if ((srom = (char *) calloc (1, ic_fmt_size_by)) == NULL) {
      fprintf(stderr, "ERROR: Cannot allocate dynamic space for output buffer.\n");
      fclose(exef);
      fclose(sromf);
      return (1);
    }

    for (i = 0; (unsigned) i < ic_fmt_size_by; srom[i++] = 0)
	;				/* init srom to all zeros */
    sromPtr = 0;			/* Init pointer to array. */


    if (codeSize > 0) {
       if (strncmp (cpu, "21164", 5) == 0)
          codeToEv5(codeSize);          /* Format code to EV5's requirements. */
       else
          codeToEv4(codeSize);		/* swap the longwords around rest of
					 * bits */
       binToHex(sromf);		        /* write out the hex file */

       if (append != NULL) fprintf(sromf, "%s\n", append);
       fprintf(sromf, ":00000001FF\n");    /* End of file record. */

    }
    fclose(exef);
    fclose(sromf);
    return (0);
    }

