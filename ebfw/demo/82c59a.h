
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
 *  $Id: 82c59a.h,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $;
 */

/*
 * $Log: 82c59a.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:44  fdh
 * Initial revision
 *
 */




/*--------------------------------------------------------------------------*/
/*  header file: this file is the declarations for the 82C59 in the 82357   */
/*                                                                          */
/*  cjm: created this file on 4/21/92                                       */
/*--------------------------------------------------------------------------*/

#define CNTRL1_ICW1   0x20
#define CNTRL1_ICW2   0x21
#define CNTRL1_ICW3   0x21
#define CNTRL1_ICW4   0x21
#define CNTRL1_MASK   0x21
#define CNTRL1_OCW1   0x21
#define CNTRL1_OCW2   0x20
#define CNTRL1_OCW3   0x20
#define CNTRL1_STATUS 0x20
#define CNTRL1_ELCR   0x4d0

#define CNTRL2_ICW1   0xA0
#define CNTRL2_ICW2   0xA1
#define CNTRL2_ICW3   0xA1
#define CNTRL2_ICW4   0xA1
#define CNTRL2_OCW1   0xa1
#define CNTRL2_OCW2   0xa0
#define CNTRL2_OCW3   0xa0
#define CNTRL2_MASK   0xa1
#define CNTRL2_STATUS 0xa0
#define CNTRL2_ELCR   0x4d1

#define NON_SPEC_EOI  0x20
#define SPEC_EOI      0x60
#define ROTATE_NS     0xa0
#define ROTATE_AUTO_S 0x80
#define ROTATE_AUTO_C 0x0
#define ROTATE_SPEC   0xe0
#define SET_PRIOR     0xc0
#define NOP           0x40
#define OWC2          0x0

/* OWC3 definitions */
#define OWC3          0x08
#define READ_IR       0x02
#define READ_IS       0x03
#define RESET_SPMASK  0x40
#define SET_SPMASK    0x60

/* mask definitions */
#define CASCADE_C      0x2
#define CASCADE_B      0x2





