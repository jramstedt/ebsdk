
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

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: flash.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: flash.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.6  1995/08/25  20:27:27  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.5  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.4  1994/08/03  13:44:48  fdh
 * Remove old comment...
 *
 * Revision 1.3  1994/06/28  20:12:40  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.2  1994/06/27  18:15:25  fdh
 * Merged flashmain.c into flash.c.
 *
 * Revision 1.1  1993/06/08  19:56:47  fdh
 * Initial revision
 *
 */




/*      modified on Thu Sep  3 16:58:33 1987 by jdd */

#include "lib.h"

typedef struct {
  int west, east, north, south;
  } Rectangle;
typedef struct {
  Rectangle r;
  int vx, vy;
  } MovingRect;
typedef struct {
  MovingRect from;
  Rectangle to;
} View;

Rectangle domain;
int wavy;
MovingRect center;
int borderSize;
int tint;

extern void Rcts();
#define cup_width 32
#define cup_height 32
static char cup_bits[] = {
   0x00, 0xe0, 0x0f, 0x00, 0x00, 0x1c, 0x70, 0x00, 0x00, 0x03, 0x80, 0x01,
   0x80, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x0c,
   0x10, 0x00, 0x00, 0x12, 0x08, 0x00, 0x00, 0x21, 0x08, 0x00, 0x80, 0x20,
   0xe4, 0xee, 0xea, 0x4e, 0xa4, 0x2a, 0x6a, 0x44, 0xe4, 0x2a, 0x76, 0x44,
   0xa2, 0x2a, 0x2a, 0x84, 0xa2, 0xee, 0xee, 0x84, 0x02, 0x00, 0x02, 0x80,
   0x02, 0x00, 0x01, 0x80, 0x02, 0x80, 0x00, 0x80, 0xba, 0xcb, 0xa5, 0xbb,
   0x8a, 0x68, 0xac, 0x88, 0xbc, 0xd8, 0xb4, 0x58, 0xa4, 0x48, 0xb4, 0x48,
   0xbc, 0xcb, 0xa5, 0x7b, 0x08, 0x02, 0x00, 0x20, 0x08, 0x01, 0x00, 0x20,
   0x90, 0x00, 0x00, 0x10, 0x60, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x04,
   0x80, 0x00, 0x00, 0x02, 0x00, 0x03, 0x80, 0x01, 0x00, 0x1c, 0x70, 0x00,
   0x00, 0xe0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00};


unsigned char cup[1024];

extern void Rcts(void);
extern int abs(int x);
extern int random(void);
extern void BuildCup(void);
extern void flash(void);
extern void PaintCenter(Rectangle r);
extern void MoveRect(MovingRect * m);
extern void DoInverseBitBlt(int fromx , int fromy , Rectangle to);
extern void DoBitBlt(int fromx , int fromy , Rectangle to);
extern void DoView(View v);
extern void main(void);

void main()
{
  flash();
}

int abs(int x)
{
  if (x < 0) x = -x;
  return(x);
}

#define RandomSeed 1354117939
#define RandomMultiplier 1327217885
unsigned int lastRand = RandomSeed;

int random()
{
  lastRand *= RandomMultiplier;
  if (lastRand & 0x80000000)
  {
    lastRand &= 0x7fffffff;
    lastRand += 1;
    if (lastRand & 0x80000000)
    {
      lastRand &= 0x7fffffff;
      lastRand += 1;  /* cannot carry a second time */
    }
  }
  return(lastRand);
}

void BuildCup()
{
  int row, col, byte, bit;
  char * cp;
  cp = (char *) &cup[0];
  for (row = 0; row < cup_height; row += 1)
  {
    for (col = 0; col < cup_width; col += 1)
    {
      byte = col >> 3;
      bit = col & 7;
      if (cup_bits[byte + (row * (cup_width >> 3))] & (1 << bit))
	*cp++ = 0xff;
      else
        *cp++ = 0x00;
    }
  }
}

void flash()
{
  int i;
  BuildCup();
  i = DisplayOpen(1);
/*  printf ("DisplayOpen returns %d\n", i); */
  wavy = TRUE;
  /* read cup file */
  /* otherwise read decsrc and wavy = FALSE; */
  domain.west = 0;
  domain.east = 1024;
  domain.north = 0;
  domain.south = 768;
  borderSize = 4;
  center.r.west = domain.west + 48;
  center.r.east =
    center.r.west + cup_width + (2 * borderSize);
  center.r.north = domain.north + 48;
  center.r.south =
    center.r.north + cup_height + (2 * borderSize);
  center.vx = 1;
  center.vy = 1;
  Rcts();
  DisplayClose();
}

void PaintCenter(Rectangle r)
{
  int i;
/*
  for (i = 0; i < 24; i += 1)
    pwgaFillSolid(i, 3, 0xff, 0, i * 20, 1024, 20);
  printf("hit return\n");
  GetChar();
*/
  pwgaFillSolid(0, 3, 0xff, domain.west, domain.north,
    domain.east - domain.west, domain.south - domain.north);
  
  pwgaDrawColorImage(r.west + borderSize, r.north + borderSize,
     cup_width, cup_height, cup, cup_width, 3, 0xff);
}

void MoveRect(MovingRect *m)
{
  int x;
  m->r.west  += m->vx;
  m->r.east  += m->vx;
  m->r.north += m->vy;
  m->r.south += m->vy;
  x = m->r.west - domain.west;
  if (x < 0)
  {
    m->vx = abs(m->vx);
    m->r.west -= x;
    m->r.east -= x;
  }
  else
  {
    x = domain.east - m->r.east;
    if (x < 0)
    {
      m->vx = -abs(m->vx);
      m->r.west += x;
      m->r.east += x;
    }
  }
  x = m->r.north - domain.north;
  if (x < 0)
  {
    m->vy = abs(m->vy);
    m->r.north -= x;
    m->r.south -= x;
  }
  else
  {
    x = domain.south - m->r.south;
    if (x < 0)
    {
      m->vy = -abs(m->vy);
      m->r.north += x;
      m->r.south += x;
    }
  }
}

void DoInverseBitBlt(int fromx, int fromy, Rectangle to)
{
  if ((to.east - to.west) <= 0) return;
  if ((to.south - to.north) <= 0) return;
  pwgaBlit(fromx, fromy, to.east - to.west, to.south - to.north,
    to.west, to.north, 12, 0xff);
}

void DoBitBlt(int fromx, int fromy, Rectangle to)
{
  if ((to.east - to.west) <= 0) return;
  if ((to.south - to.north) <= 0) return;
  pwgaBlit(fromx, fromy, to.east - to.west, to.south - to.north,
    to.west, to.north, 3, 0xff);
}

void DoView(View v)
{
  DoInverseBitBlt(v.from.r.west, v.from.r.north, v.to);
}

void Rcts()
{
  Rectangle oldCenter;
  int dx, dy;
  View r[4];
  int j;
  PaintCenter(center.r);

  r[0].from.r.west  = domain.west;
  r[0].from.r.east  = center.r.east;
  r[0].from.r.north = domain.north;
  r[0].from.r.south = center.r.north;
  r[0].from.vx      = 1;
  r[0].from.vy      = 1;
  r[0].to           = r[0].from.r;
  for (j = 1; j <= 16; j += 1) MoveRect(&r[0].from);

  r[1].from.r.west  = domain.west;
  r[1].from.r.east  = center.r.west;
  r[1].from.r.north = center.r.north;
  r[1].from.r.south = domain.south;
  r[1].from.vx      = 1;
  r[1].from.vy      = -1;
  r[1].to           = r[1].from.r;
  for (j = 1; j <= 16; j += 1) MoveRect(&r[1].from);

  r[2].from.r.west  = center.r.west;
  r[2].from.r.east  = domain.east;
  r[2].from.r.north = center.r.south;
  r[2].from.r.south = domain.south;
  r[2].from.vx      = -1;
  r[2].from.vy      = -1;
  r[2].to           = r[2].from.r;
  for (j = 1; j <= 16; j += 1) MoveRect(&r[2].from);

  r[3].from.r.west  = center.r.east;
  r[3].from.r.east  = domain.east;
  r[3].from.r.north = domain.north;
  r[3].from.r.south = center.r.south;
  r[3].from.vx      = -1;
  r[3].from.vy      = 1;
  r[3].to           = r[3].from.r;
  for (j = 1; j <= 16; j += 1) MoveRect(&r[3].from);
  while (1) /* was !kbhit() */
  {
    for (j = 0; j < 4; j += 1) DoView(r[j]);
    oldCenter = center.r;
    MoveRect(&center);
    DoBitBlt(oldCenter.west, oldCenter.north, center.r);
    dx = center.r.west - oldCenter.west;
    dy = center.r.north - oldCenter.north;

    r[0].from.r.east += dx;
    r[0].from.r.south += dy;
    r[0].to.east += dx;
    r[0].to.south += dy;
    if (wavy && ((random() & 0xf) == 0))
    {
      if (r[0].from.r.east < ((center.r.west + center.r.east) / 2))
	r[0].from.vx = 1;
      else
	r[0].from.vx = -1;
      if (r[0].from.r.south < ((center.r.north + center.r.south) / 2))
	r[0].from.vy = 1;
      else
	r[0].from.vy = -1;
    }
    MoveRect(&r[0].from);

    r[1].from.r.east += dx;
    r[1].from.r.north += dy;
    r[1].to.east += dx;
    r[1].to.north += dy;
    if (wavy && ((random() & 0xf) == 0))
    {
      if (r[1].from.r.east < ((center.r.west + center.r.east) / 2))
	r[1].from.vx = 1;
      else
	r[1].from.vx = -1;
      if (r[1].from.r.north < ((center.r.north + center.r.south) / 2))
	r[1].from.vy = 1;
      else
	r[1].from.vy = -1;
    }
    MoveRect(&r[1].from);

    r[2].from.r.west += dx;
    r[2].from.r.north += dy;
    r[2].to.west += dx;
    r[2].to.north += dy;
    if (wavy && ((random() & 0xf) == 0))
    {
      if (r[2].from.r.west < ((center.r.west + center.r.east) / 2))
	r[2].from.vx = 1;
      else
	r[2].from.vx = -1;
      if (r[2].from.r.north < ((center.r.north + center.r.south) / 2))
	r[2].from.vy = 1;
      else
	r[2].from.vy = -1;
    }
    MoveRect(&r[2].from);

    r[3].from.r.west += dx;
    r[3].from.r.south += dy;
    r[3].to.west += dx;
    r[3].to.south += dy;
    if (wavy && ((random() & 0xf) == 0))
    {
      if (r[3].from.r.west < ((center.r.west + center.r.east) / 2))
	r[3].from.vx = 1;
      else
	r[3].from.vx = -1;
      if (r[3].from.r.south < ((center.r.north + center.r.south) / 2))
	r[3].from.vy = 1;
      else
	r[3].from.vy = -1;
    }
    MoveRect(&r[3].from);
  }
}
