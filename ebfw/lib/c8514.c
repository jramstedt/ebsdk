
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
__attribute__((unused)) static const char *rcsid = "$Id: c8514.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 * $Log: c8514.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.5  1995/10/26  23:48:15  cruz
 * Fixed up prototypes.
 *
 * Revision 1.4  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.2  1994/01/19  10:40:08  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.1  1993/06/08  19:56:19  fdh
 * Initial revision
 *
 */



/* 8514 console driver */
/* Wed Sep 4 18:27:53 EDT L.S. */

#include "system.h"
#include "lib.h"

#define	NROW	24
#define	NCOL	80

#define	BGROUND	0x09			/* Bright blue.			*/
#define	FGROUND	0x0F			/* White.			*/
#define	BORDER	0x0C			/* Bright red.			*/

#define	RTOY(r)	(20*(r))
#define	CTOX(c)	((c)<<3)

extern	ub	vga8x16xx[];
extern	ub	vga8x16gl[];
int	c8514row;
int	c8514col;

void c8514show(void);
void c8514hide(void);

void c8514insfontchar(int code, ub *s)
{
  ub cbuf[128];
  int ch, bit, chp;
  chp = 0;
  for (ch = 0; ch < 16; ch += 1)
    for (bit = 7; bit >= 0; bit -= 1)
    {
      if (s[ch] & (1 << bit)) cbuf[chp] = FGROUND;
      else cbuf[chp] = BGROUND;
      chp += 1;
    }
  pwgaDrawColorImage(code << 3, 768, 8, 16, &cbuf[0], 8, 3, 0xff);
}

void initfont(void)
{
  int i;
  /* move font into offscreen memory */
  for (i = 0; i < 32; i += 1)
  {
    c8514insfontchar(i, &vga8x16xx[0]);
  }
  for (i = 32; i < 128; i += 1)
  {
    c8514insfontchar(i, &vga8x16gl[(i - 32) << 4]);
  }
}

int c8514init(void)
{
  /*    Init 8514 */
  if (!DisplayOpen(0)) return(FALSE);
  c8514erase();
  initfont();
  c8514show();
  return(TRUE);
}

void c8514erase(void)
{
  pwgaFillSolid(BGROUND, 3, 0xff, 0, 0, 1024, 768);
  c8514col = 0;
  c8514row = 0;
}

void c8514putc(char c)
{
  c8514hide();

  if (c == '\f') {
    c8514erase();
  }
  else if (c == '\b') {
    if (c8514col > 0) --c8514col;
  }
  else if (c == '\r') {
    c8514col = 0;
  }
  else if (c == '\n')
  {
    c8514col = 0;
    if (c8514row < NROW-1) ++c8514row;
    else
    {
      /* scroll up one line */
      pwgaBlit(0, 20, 640, 480, 0, 0, 3, 0xff);
    }
  }
  else
  {
    if ((c>=0x20 && c<=0x7F))
    {
      if (c8514col >= NCOL) {
        c8514putc((char)'\n');
        c8514hide();
      }
      /* paint char */
      pwgaBlit((((int) c) & 0xff) << 3, 768, 8, 16, c8514col << 3,
        2 + (c8514row * 20), (ui) 3, (ui) 0xff);
      if (c8514col < NCOL) ++c8514col;
    }
  }
  c8514show();

}



void
c8514hide(void)
{
	register int	col;

	if ((col = c8514col) == NCOL)
		col = NCOL-1;
	pwgaFillSolid(BGROUND, 3, 0xFF, CTOX(col), RTOY(c8514row+1)-2, 8, 2);
}

void
c8514show(void)
{
	register int	col;

	if ((col = c8514col) == NCOL)
		col = NCOL-1;
	pwgaFillSolid(FGROUND, 3, 0xFF, CTOX(col), RTOY(c8514row+1)-2, 8, 2);
}
