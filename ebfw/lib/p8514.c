
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
static char *rcsid = "$Id: p8514.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: p8514.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.8  1996/05/16  14:52:45  fdh
 * Removed use inportb alias.
 *
 * Revision 1.7  1995/10/26  23:43:21  cruz
 * added casting and fixed up prototypes.
 *
 * Revision 1.6  1995/08/25  19:47:20  fdh
 * Removed unnecessary variable.
 *
 * Revision 1.5  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.4  1994/06/21  15:37:54  rusling
 * fixed up WNT compilation warnings.
 *
 * Revision 1.3  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.2  1994/01/19  10:40:08  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.1  1993/06/08  19:56:24  fdh
 * Initial revision
 *
 */


#include "system.h"
#include "wga.h"
#include "lib.h"

#define GXclear 0
#define GXcopy 3
/*
 * Translation table between X ALU and 8514/A MERGE
 */
ub mergexlate[16] = {
  0x1, /* GXclear        0 */
  0xC, /* GXand          src AND dst */
  0xD, /* GXandReverse   src AND NOT dst */
  0x7, /* GXcopy         src */
  0xE, /* GXandInverted  (NOT src) AND dst */
  0x3, /* GXnoop         dst */
  0x5, /* GXxor          src XOR dst */
  0xB, /* GXor           src OR dst */
  0xF, /* GXnor          (NOT src) AND (NOT dst) */
  0x6, /* GXequiv        (NOT src) XOR dst */
  0x0, /* GXinvert       NOT dst */
  0xA, /* GXorReverse    src OR (NOT dst) */
  0x4, /* GXcopyInverted NOT src */
  0x9, /* GXorInverted   (NOT src) OR dst */
  0x8, /* GXnand         (NOT src) OR (NOT dst) */
  0x2  /* GXset          1 */
};


ub lutData[48] =
 {
 0x00, 0x00, 0x00,  /* black */
 0x00, 0x00, 0x7f,  /* dark blue */
 0x00, 0x7f, 0x00,  /* dark green */
 0x00, 0x7f, 0x7f,  /* dark cyan */
 0x7f, 0x00, 0x00,  /* dark red */
 0x7f, 0x00, 0x7f,  /* dark magenta */
 0x7f, 0x7f, 0x00,  /* dark yellow */
 0x7f, 0x7f, 0x7f,  /* gray */
 0x1f, 0x1f, 0x1f,  /* dark gray */
 0x00, 0x00, 0xff,  /* bright blue */
 0x00, 0xff, 0x00,  /* bright green */
 0x00, 0xff, 0xff,  /* bright cyan */
 0xff, 0x00, 0x00,  /* bright red */
 0xff, 0x00, 0xff,  /* bright magenta */
 0xff, 0xff, 0x00,  /* bright yellow */
 0xff, 0xff, 0xff   /* white */
 };


void byteoutport(int p, int d)
{
  outportb(p, d);
  outportb(p+1, d >> 8);
}

volatile int volx;
void short_delay(void)
{
  int i;
  for (i = 0; i < 1000; i += 1) volx = i;
}

void InitLUT(void)
{
  ui i, j;
/*  ui want, got, ok; */
  outportb(PALMASK, 0xff);
  outportb(PALWRITE_ADDR, 0);
  j = 0;
  for (i = 0; i < 768; i += 1)
  {
    outportb(PALDATA, lutData[j] >> 2);
    j += 1;
    if (j >= 48) j = 0;
    short_delay();
  }
/*
  outportb(PALREAD_ADDR, 0);
  j = 0;
  ok = TRUE;
  for (i = 0; i < 768; i += 1)
  {
    got = inportb(PALDATA);
    want = lutData[j] >> 2;
    if (want != got)
    {
      printf("lut read %d was %02x want %02x\n", i, got, want);
      ok = FALSE;
    }
    j += 1;
    if (j >= 48) j = 0;
  }
  if (ok) printf("LUT readback OK\n");
 */
}

void pwgaFillSolid(ui fg, ui alu, 
    ui planemask, int xDst, int yDst, int wDst, int hDst)
{
  while (inportw(GP_STAT) & (0x0100 >> 8))
    short_delay();
  byteoutport(FG_COLOR, fg);
  byteoutport(FG_MIX, (0x0020 | mergexlate[alu]));
  byteoutport(WR_MASK, planemask);
  byteoutport(CUR_X, xDst);
  byteoutport(CUR_Y, yDst);
  byteoutport(MAJ_AXIS_PCNT, wDst-1);
  byteoutport(MULTIFUNC_CNTL, hDst-1);
  byteoutport(CMD, CMD_XRECT | CMD_INCY | CMD_INCX
             | CMD_DRAW | CMD_ACROSS | CMD_WRITE);
}

int pwgaExists(void)
{
  byteoutport(SUBSYS_CNTL, 0x8000);
  byteoutport(SUBSYS_CNTL, 0x4000);
  byteoutport(ERR_TERM, 0xa5a5);
  return (inportw(ERR_TERM) == 0xa5a5);
}

ui pwgaHWInit(int mode1024)
{
  ub nPlanes;
/*  ub headType; */
  int width, height;
  if (!pwgaExists()) return(FALSE);
  nPlanes = (inportw(SUBSYS_STAT) & 0x0080) ? 8 : 4;
/*  headType = (ub)((inportw(SUBSYS_STAT) & 0x0070) >> 4); */
  outportb( PALMASK, (1 << nPlanes) - 1 ) ;
/*  printf("pwgaHWInit: nplanes %d, headtype %d\n", nPlanes, headType);*/
  byteoutport(DISP_CNTL, 0x0040); /* Disable Hsync, Vsync, Blank */
  /*
   * These timing parameters courtesy Western Digital data sheet
   */
  if (mode1024) {
    width = 1024;
    height = 768;
    byteoutport(H_TOTAL, 0x009D);
    byteoutport(H_DISP, 0x007F);
    byteoutport(H_SYNC_START, 0x0081);
    byteoutport(H_SYNC_WID, 0x0016); /* Positive HS */
    byteoutport(V_TOTAL, 0x0660);
    byteoutport(V_DISP, 0x05FB);
    byteoutport(V_SYNC_START, 0x0600);
    byteoutport(V_SYNC_WID, 0x0008); /* Positive VS */
    byteoutport(ADVFUNC_CNTL, 0x0007); /* 1024x768 clock, no pass through */
    byteoutport(DISP_CNTL, 0x0033); /* Interlace, enable Hsync, Vsync, Blank */
  }
  else {
    width = 640;
    height = 480;
    byteoutport(H_TOTAL, 0x0063);
    byteoutport(H_DISP, 0x004F);
    byteoutport(H_SYNC_START, 0x0052);
    byteoutport(H_SYNC_WID, 0x002C); /* Negative HS */
    byteoutport(V_TOTAL, 0x0418);
    byteoutport(V_DISP, 0x03BB);
    byteoutport(V_SYNC_START, 0x03D2);
    byteoutport(V_SYNC_WID, 0x0022); /* Negative VS */
    byteoutport(ADVFUNC_CNTL, 0x0003); /* 640x480 clock, no pass through */
    byteoutport(DISP_CNTL, 0x0023); /* No interlace, enable Hsync, Vsync, Blank */
  }
  byteoutport(SUBSYS_CNTL, 0x9000); /* Reset the Graphics Subsystem */
  short_delay();
  byteoutport(MULTIFUNC_CNTL, 0x5006); /* Set memory configuration */
  short_delay();
  byteoutport(SUBSYS_CNTL, 0x400F); /* Enable Graphics Subsystem, Clear intrpts */
  short_delay();
  while (inportw(GP_STAT) & (0x0100 >> 7))
    short_delay();
  byteoutport(MULTIFUNC_CNTL, 0x1000); /* Top */
  byteoutport(MULTIFUNC_CNTL, 0x2000); /* Left */
  byteoutport(MULTIFUNC_CNTL, 0x3000 | 1023); /* Bottom */
  byteoutport(MULTIFUNC_CNTL, 0x4000 | 1023); /* Right */
  byteoutport(MULTIFUNC_CNTL, 0xA000);
  byteoutport(RD_MASK, 0x00FF);
  byteoutport(BG_MIX, 3);
  pwgaFillSolid(0, GXcopy, (1<<nPlanes)-1, 0, 0, width, height);
  /*           (fg, alu, planemask, xDst, yDst, wDst, hDst) */
  return nPlanes;
}

void outwords(short int *wSrc, int wcount)
{
  while (wcount >= 8)
  {
    while (inportw(GP_STAT) & (0x0100 >> 8)) short_delay();
    outportb(PIX_TRANS+0, *wSrc);
    outportb(PIX_TRANS+1, *wSrc>>8);
    ++wSrc;
    outportb(PIX_TRANS+0, *wSrc);
    outportb(PIX_TRANS+1, *wSrc>>8);
    ++wSrc;
    outportb(PIX_TRANS+0, *wSrc);
    outportb(PIX_TRANS+1, *wSrc>>8);
    ++wSrc;
    outportb(PIX_TRANS+0, *wSrc);
    outportb(PIX_TRANS+1, *wSrc>>8);
    ++wSrc;
    outportb(PIX_TRANS+0, *wSrc);
    outportb(PIX_TRANS+1, *wSrc>>8);
    ++wSrc;
    outportb(PIX_TRANS+0, *wSrc);
    outportb(PIX_TRANS+1, *wSrc>>8);
    ++wSrc;
    outportb(PIX_TRANS+0, *wSrc);
    outportb(PIX_TRANS+1, *wSrc>>8);
    ++wSrc;
    outportb(PIX_TRANS+0, *wSrc);
    outportb(PIX_TRANS+1, *wSrc>>8);
    ++wSrc;
/*
    outportb(PIX_TRANS, *wSrc++);
    outportb(PIX_TRANS, *wSrc++);
    outportb(PIX_TRANS, *wSrc++);
    outportb(PIX_TRANS, *wSrc++);
    outportb(PIX_TRANS, *wSrc++);
    outportb(PIX_TRANS, *wSrc++);
    outportb(PIX_TRANS, *wSrc++);
    outportb(PIX_TRANS, *wSrc++);
*/
    wcount -= 8;
  }
  if (wcount > 0)
    while (inportw(GP_STAT) & (0x0100 >> wcount)) short_delay();
  while (wcount > 0)
  {
    byteoutport(PIX_TRANS, *wSrc++);
    wcount -= 1;
  }
}

void outwblock(ub *pSrc, int w, int h, int widthSrc)
{
  int hcount, wcount;
  short int word;
  if (!(w & 1))  /* width is even */
  {
    for (hcount = 0; hcount < h; hcount += 1)
    {
      outwords((short int *) pSrc, w >> 1);
      pSrc += widthSrc;
    }
  }
  else
  {
    for (hcount = 0; hcount < (h >> 1); hcount += 1)
    {
      wcount = w >> 1;
      outwords((short int *) pSrc, wcount);
      word = pSrc[wcount << 1];   /* last pixel on line */
      pSrc += widthSrc;
      word = (word & 0xff) + ((*pSrc << 8) & 0xff00);
      outwords(&word, 1);
      outwords((short int *) (&pSrc[1]), wcount);
      pSrc += widthSrc;
    }
    if (h & 1)
    {
      wcount = w >> 1;
      outwords((short int *) pSrc, wcount + 1);
    }
  }
}

void pwgaDrawColorImage(int xDst, int yDst, int wDst, int hDst, 
    ub *pSrc, int widthSrc, ui alu, 
    ui planemask)
{
  while (inportw(GP_STAT) & (0x0100 >> 7)) short_delay();
  byteoutport(FG_MIX, (0x0040 | mergexlate[alu]));
  byteoutport(WR_MASK, planemask);
  byteoutport(CUR_X, xDst);
  byteoutport(CUR_Y, yDst);
  byteoutport(MAJ_AXIS_PCNT, wDst - 1);
  byteoutport(MULTIFUNC_CNTL, hDst - 1);
  if (wDst > 1) {
    byteoutport(CMD, CMD_XRECT | CMD_WORDBUS | CMD_LSBFIRST | CMD_VDATA
               | CMD_INCY | CMD_INCX
               | CMD_DRAW | CMD_THROUGH | CMD_WRITE);
    outwblock(pSrc, wDst, hDst, widthSrc);
  }
  else
  {
    byteoutport(CMD, CMD_XRECT | CMD_BYTEBUS | CMD_VDATA
               | CMD_INCY | CMD_INCX
               | CMD_DRAW | CMD_THROUGH | CMD_WRITE);
    while (hDst--)
    {
      while (inportw(GP_STAT) & (0x0100 >> 1)) short_delay();
      byteoutport(PIX_TRANS, *pSrc);
      pSrc += widthSrc;
    }
  }
}

void pwgaBlit(int xSrc, int ySrc, int wSrc, int hSrc,
    int xDst, int yDst, ui alu, ui planemask)
{
  ui  command;
  command = CMD_COPYRECT | CMD_INCY | CMD_INCX
            | CMD_DRAW | CMD_ACROSS | CMD_WRITE;
  wSrc--;
  hSrc--;
  if (xSrc < xDst) {
    xSrc += wSrc;
    xDst += wSrc;
    command &= ~CMD_INCX;
  }
  if (ySrc < yDst) {
    ySrc += hSrc;
    yDst += hSrc;
    command &= ~CMD_INCY;
  }
  while (inportw(GP_STAT) & (0x0100 >> 4)) short_delay();
  byteoutport(FG_MIX, (0x0060 | mergexlate[alu]));
  byteoutport(WR_MASK, planemask);
  byteoutport(CUR_X, xSrc);
  byteoutport(CUR_Y, ySrc);
  while (inportw(GP_STAT) & (0x0100 >> 5)) short_delay();
  byteoutport(DESTX_DIASTP, xDst);
  byteoutport(DESTY_AXSTP, yDst);
  byteoutport(MAJ_AXIS_PCNT, wSrc);
  byteoutport(MULTIFUNC_CNTL, hSrc);
  byteoutport(CMD, command);
}

int DisplayOpen(int mode1024)
{
  int i;
  i = pwgaHWInit(mode1024);
  if (i) InitLUT();
  return(i);
}

void DisplayClose(void)
{
  byteoutport(DISP_CNTL, 0x0040);    /* Disable Hsync, Vsync, Blank */
  byteoutport(ADVFUNC_CNTL, 0x0000); /* Pass Through */
  /*
   * Now the 8514/A DAC will mirror changes made to the VGA DAC
   */
}

