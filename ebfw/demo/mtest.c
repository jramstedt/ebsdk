
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
__attribute__((unused)) static const char *rcsid = "$Id: mtest.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: mtest.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:50  fdh
 * Initial revision
 *
 */

#include "lib.h"

#define BASE      ( 0)
#define m16       ( 0x1000000/4)
#define m64       ( 0x4000000/4)
#define m128      ( 0x8000000/4)
#define m144      ( 0x9000000/4)
#define m192      ( 0xC000000/4)
#define FLUSH     ( 0xA000000/4)
#define PATTERN  (unsigned int)( 0x12345678)
#define FPATTERN (unsigned int)( 0x5a5a5a5a)
#define STANDALONE 1
#define	MEMNONE	0
#define	MEM8M	1
#define MEM16M  2
#define	MEM32M	3
#define ADDRESS (0x100000+0x40000) /* this is bootadr */

extern	void	memtest_all();
extern	void	memtest();
extern	void	memtest1();
extern	void	memtest2();
extern	void	memtest3();

void size_memory(int);

#define MASK_1 0xFFFFFFFF
int sizea,sizeb;

void main()
{
  int *mem=0,x;
  register unsigned int tmp,*flush=(unsigned int*) 0x140000;

  // uart_init();
  size_memory(0);

  for (x=(0x140004/4);x<(sizea+sizeb)/4;x++) {
    if (!(x%0x1000)) printf("testing memory %08x\r",&mem[x]);
    tmp = mem[x]&MASK_1;
    mem[x] = ~tmp&MASK_1; mb();
    flush[0] = 0x0; mb();
    if ((mem[x]&MASK_1) != (~tmp&MASK_1)) {
      printf("\n-memory test error! [addr:0x%08x, act:0x%08x, exp:0x%08x]\n",
	     &mem[x],mem[x]&MASK_1,~tmp&MASK_1);
      while(1);
      return;
    }
    mem[x] = tmp&MASK_1; mb();
    flush[0] = 0x0; mb();
    if ((mem[x]&MASK_1) != (tmp&MASK_1)) {
      printf("\n+memory test error! [addr:0x%08x, act:0x%08x, exp:0x%08x]\n",
	     &mem[x],mem[x]&MASK_1,tmp&MASK_1);
      while(1);
      return;
    }
  }
}

void size_memory(int verbose)
{
  unsigned int reg_4;
  register unsigned int save, *p,mem_conf=0,bankA=0,bankB=0,size;
  volatile unsigned int *sysctl = (unsigned int*) 0x1e0000000L;

  volatile unsigned int *mem = (unsigned int*) 0;
  volatile unsigned int *mem16 = (unsigned int*)  0x1000000;
  volatile unsigned int *mem64 = (unsigned int*)  0x4000000;
  volatile unsigned int *mem128 = (unsigned int*) 0x8000000;
  volatile unsigned int *mem144 = (unsigned int*) 0x9000000;
  volatile unsigned int *mem192 = (unsigned int*) 0xC000000;
  volatile unsigned int *flush = (unsigned int*)   0xA00000;
  
  if (verbose) printf("sizing system memory...\n");

  *sysctl = 0xF0;mb();
  reg_4 = 0x1f;				     /* set max memory */
  save = *mem;			             /* save base memory */
  *mem = PATTERN; mb();
  *flush = FPATTERN; mb();

  if (*mem16==PATTERN) reg_4 &= 0x1D;	     /* clear 16 meg SIMMS bit */

  *mem64 = PATTERN; mb();
  *flush = FPATTERN; mb();

  if (*mem64 != PATTERN) reg_4 &= 0x1E;	     /* clear double ras SIMMS bit */

  *mem128 = PATTERN; mb();
  *flush = FPATTERN; mb();

  if (*mem128 == PATTERN) {		     /* we found more memory */
    *mem128 = PATTERN; mb();
    *flush = FPATTERN; mb();
    
    if (*mem144 == PATTERN) {
      reg_4 &= 0x1A;   
    }
    *mem192 = PATTERN; mb();
    *flush = FPATTERN; mb();
    if (*mem192 != PATTERN) {
      reg_4 &= 0x17;			     /* clear double ras SIMMS bit */
    }
  }
  else reg_4 &= 0xF;			     /* otherwise we have no more memory */
  
  bankA = reg_4 & 3;
  sizea = (1<<bankA)*0x1000000;

  if (reg_4&0x10) {
    bankB = (reg_4 & 0xC) >> 2;
    sizeb = (1<<bankB)*0x1000000;
  }

  mem_conf = ((bankA|(bankB<<2))<<4)|(*sysctl&0xf);
  *sysctl = mem_conf; mb();
  size = sizea+sizeb;

  *mem = save; mb();
}


