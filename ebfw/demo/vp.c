
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
__attribute__((unused)) static const char *rcsid = "$Id: vp.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: vp.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:55  fdh
 * Initial revision
 *
 */



/* a procedure to translate alpha virtual addresses */

/* jn  20-Nov-1991 */


/* 
 * inputs:
 *
 *   ptbr  - page table base register, physical page frame of segment 1
 *		page table
 *
 *   va  - virtual address to translate
 */
struct pte_ctl_struct{
	unsigned val	: 1;
	unsigned fonr	: 1;
	unsigned fonw	: 1;
	unsigned fone	: 1;
	unsigned asm	: 1;
	unsigned gh	: 2;
	unsigned rsv	: 1;
	unsigned kre	: 1;
	unsigned ere	: 1;
	unsigned sre	: 1;
	unsigned ure	: 1;
	unsigned kwe	: 1;
	unsigned ewe	: 1;
	unsigned swe	: 1;
	unsigned uwe	: 1;
	unsigned sfw	: 16;
};
struct pte_struct{
	struct pte_ctl_struct ctl;
	unsigned int pfn;
};
/*typedef struct pte_struct pte_type;*/

#define PAGESIZE 13
#define SEGSIZE  10
#define _8K	 0x2000
#define PGSIZ    _8K

void
vp( long ptbr, long va )
{
  struct pte_struct  pte_s1, pte_s2, pte;
  long   seg1, seg2, seg3, offset;
  long mask=0;
  int  i;
  long tmpmask;
  long pa;
  long pte_addr;
  
  for( i=0; i<PAGESIZE; i++ )
	  mask = (mask<<1) | 1;
  offset = (va & mask);

  mask = 0;
  for( i=0; i<SEGSIZE; i++ )
	  mask = (mask<<1) | 1;

  tmpmask = (mask << PAGESIZE);
  seg3 = (  (va & tmpmask) >> PAGESIZE );

  tmpmask = (mask << (PAGESIZE + SEGSIZE) );
  seg2 = (  (va & tmpmask) >> (PAGESIZE + SEGSIZE) );

  tmpmask = (mask << (PAGESIZE + SEGSIZE *2) );
  seg1 = (  (va & tmpmask) >> (PAGESIZE + SEGSIZE *2) );

/*
 *  printf( "seg1 = %x  seg2 = %x  seg3 = %x  off = %x\n",
 *	 seg1, seg2, seg3, offset );
 */


  /* do the translation */
  pte_addr = (ptbr * PGSIZ) + (seg1 * 8);
/*  printf( "pte1 addr = %x\n", pte_addr ); */
  pte_s1 = *(struct pte_struct *)pte_addr;
/*  printf( "pte_s1 pfn = %x\n", pte_s1.pfn ); */

  pte_addr = (pte_s1.pfn * PGSIZ) + (seg2 * 8);
/*  printf( "pte2 addr = %x\n", pte_addr ); */
  pte_s2 = *(struct pte_struct *)pte_addr;
/*  printf( "pte_s2 pfn = %x\n", pte_s2.pfn ); */

  pte_addr = (pte_s2.pfn * PGSIZ) + (seg3 * 8);
/*  printf( "pte3 addr = %x\n", pte_addr ); */
  pte = *(struct pte_struct *)pte_addr;
/*  printf( "pte pfn = %x\n", pte.pfn ); */

  pa = (pte.pfn * PGSIZ) | offset;
  printf( "pa = %x", pa );

  if( pte.ctl.val == 1 ) printf( " V" );
  if( pte.ctl.fonr == 1 ) printf( " FOR" );
  if( pte.ctl.fonw == 1 ) printf( " FOW" );
  if( pte.ctl.fone == 1 ) printf( " FOE" );
  if( pte.ctl.asm == 1 ) printf( " ASM" );
  printf( "  r: " );
  if( pte.ctl.kre == 1 ) printf( "K" );
  if( pte.ctl.ere == 1 ) printf( "E" );
  if( pte.ctl.sre == 1 ) printf( "S" );
  if( pte.ctl.ure == 1 ) printf( "U" );
  printf( "  w: " );
  if( pte.ctl.kwe == 1 ) printf( "K" );
  if( pte.ctl.ewe == 1 ) printf( "E" );
  if( pte.ctl.swe == 1 ) printf( "S" );
  if( pte.ctl.uwe == 1 ) printf( "U" );
  printf( "  gh = %d", pte.ctl.gh);
  printf( "  sfw = %x\n", pte.ctl.sfw );

}
