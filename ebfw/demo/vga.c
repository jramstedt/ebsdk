
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
__attribute__((unused)) static const char *rcsid = "$Id: vga.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: vga.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1995/08/25  20:27:27  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.2  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:54  fdh
 * Initial revision
 *
 */

#include "lib.h"

/*#define DONT_GO_HIREZ  /**/
#define DEBUG
#define TRITON
#define CLRSCN
#define NEWFONTS
/*#define TESTPATTERN*/

#define OFFSCREEN 768
unsigned int use_blt=1;
/*
 * Simple VGA routines.
 *
 * All modern VGA's are build using SuperVGA chips,
 * and all SuperVGA chips are different. This means that this
 * code is chip specific. I have made an attempt to mark all of the
 * code that is SuperVGA chip specific with conditionals.
 * This particular version of the code is for the MCT-VGA-1024,
 * which uses the WD90C90 chip.
 *
 * This code is written for the Theta-II/Jensen address scheme. 
 *
 * This code also assumes that the monitor is a VGA (analog)
 * monitor. If we cared, we could change the entries in the palette
 * RAM so that things would work on an EGA (ttl) monitor.
 *
 * modified to run the COMPAQ Qvision EISA VGA in 1024x768, ni, 60Hz mode.
 *  (added mode calls, etc, to make that work right..)
 */

#define NROW    25
#define NCOL    80



char scroll[]="Now Scrolling.. \n";


/*
 * ISA cycle type masks.
 * These get shifted by 4 in the read/write routines.. 
 */

#define LOBYTE   (0x00000000)
#define HIBYTE   (0x00000008)

#define WWORD     (0x00000002)
#define ENC_BIT2 (0x00000010) /*huh?*/

#define B_EN1    (0x0000000)


#include "palette8.h"


/*
 * Configuration.
 */

#define WD90C00 1                       /* WD90C00 SuperVGA chip.       */
#define VGA8BIT 1                       /* VGA is 8 bit.                */
#define NEWVGA                          .* stuff for the cheapo VGA on T2 */

#define LOADGR  0                       /* Load glyphs for GR.          */


/*
 * VGA memory and I/O addresses. Any addresses
 * that depend on the value of the IOA bit in the
 * miscellaneous output register have the appropriate value
 * for IOA=1; that is, they have the 0x03Dx form.
 */

#define VGAGSEG (0xA000<<7) /* trust me. This makes vgastl() work right. */
#define VGAASEG (0xB800<<7)
#define REALBASE 0x20000000
#define VGABASE (REALBASE+(VGAASEG))


#define VGAMORW 0x03C2
#define VGAMORR 0x03CC
#define VGAFCRW 0x03DA
#define VGAFCRR 0x03CA
#define VGAIS0R 0x03C2
#define VGAIS1R 0x03DA



#define VGASEQA 0x03C4
#define VGASEQD 0x03C5
#define VGACRTA 0x03D4
#define VGACRTD 0x03D5
#define VGAGFXA 0x03CE
#define VGAGFXD 0x03CF
#define VGAATRA 0x03C0
#define VGAATRD 0x03C0
#define VGADACW 0x03C8
#define VGADACR 0x03C7
#define VGADACD 0x03C9
#define VGADACM 0x03C6



/*
 * Colours.
 */

#define BGROUND 0x09                    /* Bright blue.                 */
#define FGROUND 0x0F                    /* White.                       */
#define BORDER  0x0C                    /* Bright red.                  */



/*
 * Initialization tables.
 */

#define NSINIT  4
#define NCINIT  25
#define NGINIT  9
#define NAINIT  21

char    vgasinit[NSINIT] = {    /* sequencer registers */
	/* 0x09,*/
	0x01,   /* 01: clk mode. screen on, every clock, 8-dot. */
	0x03,   /* 02: mapmask. enable maps 1:0. */
	0x00,   /* 03: charmap sel. 1st 8k for a & b. */
	0x02    /* 04: mem mode. Mem >64k. */
};



char    vgacinit[NCINIT] = {    /* CRTC registers */
	0x5F,   0x4F,   0x50,   0x82,   0x55,
	0x81,   0xBF,   0x1F,   0x00,   0x4F,
	0x0E,   0x0F,   0x00,   0x00,   0x00,
	0x00,   0x9C,   0x8E,   0x8F,   0x28,
	0x1F,   0x96,   0xB9,   0xA3,   0xFF
};



char    vgaginit[NGINIT] = {    /* graphic ctl regs */
	0x00,   0x00,   0x00,   0x00,   0x00,
	0x10,   0x0E,   0x00,   0xFF
};



char    vgaainit[NAINIT] = {
	0x00,   0x01,   0x02,   0x03,   0x04,
	0x05,   0x06,   0x07,   0x08,   0x09,
	0x0A,   0x0B,   0x0C,   0x0D,   0x0E,
	0x0F,   0x00,   BORDER, 0x0F,   0x00,
	0x00
};

#define CHARHEIGHT 16
#define CHARWIDTH 8
extern unsigned char    vga8x16xx[];
extern unsigned char    vga8x16gl[];


unsigned char basefont8x16[] = {
#include "san28x16.h"
};

#if LOADGR
extern  unsigned char   vga8x16gr[];
#endif



/*
 * Colour map.
 */

struct  {
  char  r;
  char  g;
  char  b;

}       vgadac[16] = {
	{ 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x7f },
	{ 0x00, 0x7f, 0x00 },
	{ 0x00, 0x7f, 0x7f },
	{ 0x7f, 0x00, 0x00 },
	{ 0x7f, 0x00, 0x7f },
	{ 0x7f, 0x7f, 0x00 },
	{ 0x7f, 0x7f, 0x7f },
	{ 0x1f, 0x1f, 0x1f },
	{ 0x00, 0x00, 0xff },
	{ 0x00, 0xff, 0x00 },
	{ 0x00, 0xff, 0xff },
	{ 0xff, 0x00, 0x00 },
	{ 0xff, 0x00, 0xff },
	{ 0xff, 0xff, 0x00 },
	{ 0xff, 0xff, 0xff }
};



int     vgarow;
int     vgacol;
long    vgaloc;



extern  void    vgainit();
extern  void    vgaerase();
extern  void    vgalcgen();
extern  void    vgasetloc();
extern  void    vgaputc();
extern  void    vgastl();
extern  int     vgaldl();


void q_setcolor(unsigned int, unsigned int);
void q_setmode(unsigned int);
void q_clearscreen();
void q_loadchars();
void writexy();

/* Standard types */
typedef unsigned char        BOOL;
typedef unsigned char        BYTE;
typedef unsigned int         WORD;
typedef unsigned long        DWORD;
typedef unsigned char      * PBYTE;
typedef unsigned int       * PWORD;
typedef unsigned long      * PDWORD;
typedef          char        CHAR;
typedef          int         INT;
typedef          int         SHORT;
typedef          long int    LONG;
typedef          void        VOID;
typedef          char      * PCHAR;
typedef          int       * PINT;
typedef          int       * PSHORT;
typedef          long int  * PLONG;
typedef          void      * PVOID;
typedef          char        SCHAR;
typedef          int         SSHORT;
typedef          long int    SLONG;
typedef          char      * PSCHAR;
typedef          int       * PSSHORT;
typedef          long int  * PSLONG;
typedef unsigned char        UCHAR;
typedef unsigned int         USHORT;
typedef unsigned long int    ULONG;
typedef unsigned char      * PUCHAR;
typedef unsigned int       * PUSHORT;
typedef unsigned long int   * PULONG;
typedef float	REAL;
typedef double DOUBLE;

/*************************************************************************
 * FUNCTION PROTOTYPES
 *************************************************************************/
USHORT SetPackedRead( VOID);
USHORT SetPackedWrite( VOID);
USHORT SetPlanarRead( BYTE);
USHORT SetPlanarWrite( BYTE);
USHORT Set_FB_CE_Write( BYTE, BYTE);
USHORT Set_FT_CE_Write( BYTE);
USHORT Set_FO_CE_Write( BYTE, BYTE, BYTE);
USHORT SetBitBLT( BYTE, BYTE, BYTE, BYTE);
USHORT SetLine( BYTE, BYTE);

/*************************************************************************
 * FUNCTION PROTOTYPES
 *************************************************************************/
USHORT Blt_SS_Pk( USHORT, USHORT, USHORT, USHORT, USHORT, USHORT);
USHORT Blt_SS_CE_lin(ULONG, ULONG , USHORT, USHORT, USHORT, USHORT, BYTE, BYTE);
USHORT Blt_CS_CE( BYTE, USHORT, USHORT, USHORT, USHORT, BYTE, BYTE, BYTE *);
USHORT Blt_PS_Pk( USHORT, USHORT, USHORT, USHORT, BYTE *);
USHORT Blt_PS_CE( USHORT, USHORT, USHORT, USHORT, BYTE, BYTE, BYTE *);
USHORT Blt_Nx8_PS_Pk( ULONG, USHORT, USHORT, USHORT, BYTE *);
USHORT Blt_NxM_PS_Pk( ULONG, USHORT, USHORT, USHORT, USHORT, BYTE *);
USHORT Blt_Buff_SS_Pk( USHORT, USHORT *, USHORT *, USHORT *, 
			USHORT *, USHORT *, USHORT *);
USHORT SetExtMode(BYTE, BYTE);
USHORT ClearScreen(VOID);

/* defines from compaq disk.. */

/***************************************************************************
 * Defines
 ***************************************************************************/

#define GC_INDEX            0x3CE      /* Index and Data Registers */
#define GC_DATA             0x3CF
#define SEQ_INDEX           0x3C4
#define SEQ_DATA            0x3C5
#define CRTC_INDEX          0x3D4
#define CRTC_DATA           0x3D5
#define ATTR_INDEX          0x3C0
#define ATTR_DATA           0x3C0
#define MISC_OUTPUT         0x3C2

#define CTRL_REG_1         0x63CA      /* Datapath Registers */
#define DATAPATH_CTRL        0x5A
#define GC_FG_COLOR          0x43
#define GC_BG_COLOR          0x44
#define SEQ_PIXEL_WR_MSK     0x02
#define GC_PLANE_WR_MSK      0x08
#define ROP_A              0x33C7
#define ROP_0              0x33C5
#define ROP_1              0x33C4
#define ROP_2              0x33C3
#define ROP_3              0x33C2
#define DATA_ROTATE          0x03
#define READ_CTRL            0x41

#define X0_SRC_ADDR_LO     0x63C0      /* BitBLT Registers */
#define Y0_SRC_ADDR_HI     0x63C2
#define DEST_ADDR_LO       0x63CC
#define DEST_ADDR_HI       0x63CE
#define BITMAP_WIDTH       0x23C2
#define BITMAP_HEIGHT      0x23C4
#define SRC_PITCH          0x23CA
#define DEST_PITCH         0x23CE
#define BLT_CMD_0          0x33CE
#define BLT_CMD_1          0x33CF
#define PREG_0             0x33CA
#define PREG_1             0x33CB
#define PREG_2             0x33CC
#define PREG_3             0x33CD
#define PREG_4             0x33CA
#define PREG_5             0x33CB
#define PREG_6             0x33CC
#define PREG_7             0x33CD

#define X1                 0x83CC      /* Line Draw Registers */
#define Y1                 0x83CE
#define LINE_PATTERN       0x83C0
#define PATTERN_END          0x62
#define LINE_CMD             0x60
#define LINE_PIX_CNT         0x64
#define LINE_ERR_TERM        0x66
#define SIGN_CODES           0x63
#define K1_CONST             0x68
#define K2_CONST             0x6A

#define PALETTE_WRITE       0x3C8      /* DAC registers */
#define PALETTE_READ        0x3C7
#define PALETTE_DATA        0x3C9
#define CO_COLOR_WRITE     0x83C8
#define CO_COLOR_DATA      0x83C9
#define DAC_CMD_0          0x83C6
#define DAC_CMD_1          0x13C8
#define DAC_CMD_2          0x13C9

#define PAGE_REG_0           0x45      /* Control Registers */
#define PAGE_REG_1           0x46
#define HI_ADDR_MAP          0x48
#define ENV_REG_1            0x50
#define VIRT_CTRLR_SEL     0x83C4

#define PACKED_PIXEL_VIEW    0x00      /* CTRL_REG_1 values */
#define PLANAR_VIEW          0x08
#define EXPAND_TO_FG         0x10
#define EXPAND_TO_BG         0x18
#define BITS_PER_PIX_4       0x00
#define BITS_PER_PIX_8       0x02
#define BITS_PER_PIX_16      0x04
#define BITS_PER_PIX_32      0x06
#define ENAB_TRITON_MODE     0x01

#define ROPSELECT_NO_ROPS              0x00      /* DATAPATH_CTRL values */
#define ROPSELECT_PRIMARY_ONLY         0x40
#define ROPSELECT_ALL_EXCPT_PRIMARY    0x80
#define ROPSELECT_ALL                  0xc0
#define PIXELMASK_ONLY                 0x00
#define PIXELMASK_AND_SRC_DATA         0x10
#define PIXELMASK_AND_CPU_DATA         0x20
#define PIXELMASK_AND_SCRN_LATCHES     0x30
#define PLANARMASK_ONLY                0x00
#define PLANARMASK_NONE_0XFF           0x04
#define PLANARMASK_AND_CPU_DATA        0x08
#define PLANARMASK_AND_SCRN_LATCHES    0x0c
#define SRC_IS_CPU_DATA                0x00
#define SRC_IS_SCRN_LATCHES            0x01
#define SRC_IS_PATTERN_REGS            0x02
#define SRC_IS_LINE_PATTERN            0x03

#define SOURCE_DATA         0x0C       /* ROP values */
#define DEST_DATA           0x0A

#define START_BLT            0x01      /* BLT_CMD_0 values */
#define NO_BYTE_SWAP         0x00
#define BYTE_SWAP            0x20
#define FORWARD              0x00
#define BACKWARD             0x40
#define WRAP                 0x00
#define NO_WRAP              0x80

#define LIN_SRC_ADDR         0x00      /* BLT_CMD_1 values */
#define XY_SRC_ADDR          0x40
#define LIN_DEST_ADDR        0x00
#define XY_DEST_ADDR         0x80

#define START_LINE           0x01      /* LINE_CMD values */
#define NO_CALC_ONLY         0x00
#define CALC_ONLY            0x02
#define NO_KEEP_X0_Y0        0x00
#define KEEP_X0_Y0           0x08
#define LINE_RESET           0x80

#define BUFFER_BUSY_BIT      0x80      /* CTRL_REG_1 bit */
#define GLOBAL_BUSY_BIT      0x40

#define SS_BIT               0x01      /* BLT_CMD_0 bit */

#define START_BIT            0x01      /* LINE_CMD bit */

#define TRUE                    1      /* Misc. */
#define FALSE                   0
#define NO_ROTATE            0x00
#define NO_MASK              0xFF
#define MAX_SCANLINE_DWORDS   256

#define TESTS_PASSED            0      /* TritonPOST() defines */
#define ASIC_FAILURE            1
#define SETMODE_FAILURE         2
#define MEMORY_FAILURE          3
#define DAC_FAILURE             4

#define MODE_32                 0      /* SetExtMode() defines */
#define MODE_37                 1
#define MODE_38                 2
#define MODE_3B                 3
#define MODE_3C                 4
#define MODE_3E                 5
#define MODE_4D                 6
#define MODE_4E                 7
#define MON_CLASS_CNT           4
#define MODE_CNT                8
#define SEQ_CNT                 5
#define CRTC_CNT               25
#define ATTR_CNT               20
#define GRFX_CNT                9

#define MDA                  0x00      /* GetAdapterInfo() defines */
#define CGA                  0x01
#define VGA                  0x02
#define ACCELERATED_VGA      0x03
#define ACCELERATED_VGA_132  0x04
#define ADVANCED_VGA         0x05
#define PORTABLE_ADV_VGA     0x06
#define TRITON_ISA           0x07
#define TRITON_EISA          0x08
#define NONE               0xFFFF

/* end defines from compaq disk.. */


unsigned int v_rows, v_cols;
unsigned int q_curcol, q_currow;
unsigned char bg_color, fg_color;


unsigned char hex[]={'0','1','2','3','4','5','6','7',
			'8','9','a','b','c','d','e','f'};
void q_putchar();
void q_scroll();

unsigned  char readreg();
void writereg();

#define BAILOUT (*(int(*)())0)()
void main()
{
	vgainit();
}



/*
 * Initialize the VGA. When all of this
 * is completed the VGA is in 80x25 alphanumeric mode,
 * using an 8x16 character cell, the screen is blank (with the
 * cursor at (0, 0), and a font has been loaded into
 * VRAM map 2.
 */

void vgainit()
{

  register int  i;
  int j;


#if WD90C00
  outportb(0x46E8, 0x10);               /* Global enable.               */
  outportb(0x0102, 0x01);
  outportb(0x46E8, 0x08);
#endif



#ifdef NEWVGA

/* new stuff. */

  outportb(VGAGFXA, 0x0d);              /* PR3 */
  outportb(VGAGFXD, 0x02);              /* unlock everything */
  outportb(VGAGFXA, 0x0f);              /* PR5 */
  outportb(VGAGFXD, 0x05);              /* unlock PR0-PR4 */

  writereg(0x3d4,0x29,0x85);            /* PR11: unlock PR10-PR17 */
  writereg(0x3d4,0x2e,0x20);            /* PR15: enable vclk0,vclk1 */
  writereg(0x3c4,0x6,0x48);             /* PR20: unlock extd 3c4 regs */

/* end new stuff. */
#endif

  outportb(VGAMORW, 0x63);              /* misc output reg */
					/* ..+vsync, -hsync,,vclk0,
					  vmem access enabled, CRTC@3Dx */

  /* Sequencer */
  outportb(VGASEQA, 0x00);              /* seq reset */
  outportb(VGASEQD, 0x01);              /* .. sync reset */

  for (i=0; i<NSINIT; ++i) {
    outportb(VGASEQA, 0x01+i);          /* see comments in the table.. */
    outportb(VGASEQD, vgasinit[i]);
  }

  outportb(VGASEQA, 0x00);              /* seq reset */
  outportb(VGASEQD, 0x03);              /* run mode. */

  /* crtc */
  for (i=0; i<NCINIT; ++i) {
    outportb(VGACRTA, i);               /* see table. */
    outportb(VGACRTD, vgacinit[i]);
  }

  /* graphics */

  for (i=0; i<NGINIT; ++i) {
    outportb(VGAGFXA, i);
    outportb(VGAGFXD, vgaginit[i]);
  }

#if WD90C00

  outportb(VGAGFXA, 0x09);      /* PR0A                         */
  outportb(VGAGFXD, 0x00);
  outportb(VGAGFXA, 0x0B);      /* PR1                          */
  outportb(VGAGFXD, 0x06);

#endif

#ifdef NEWVGA

  outportb(VGASEQA, 0x6);       /* PR20 */
  outportb(VGASEQD, 0x48);      /* unlock extd 0x3c4 addresses */
  outportb(VGASEQA, 0x12);      /* PR32 */
  outportb(VGASEQD, 0x04);      /* magic bits for the clock generator */
  outportb(VGAGFXA, 0x0c);      /* PR2 */
  outportb(VGAGFXD, 0x02);      /* clock generator bit */
  outportb(0x3d4, 0x2e);        /* PR15 */
  outportb(0x3d5, 0x20);        /* enable vclk0,vclk1 outputs */
#endif


  /* attributes */
  (void) inportbxt(VGAIS1R);
  for (i=0; i<NAINIT; ++i) {
    outportb(VGAATRA, i);
    outportb(VGAATRD, vgaainit[i]);
  }
  outportb(VGAATRA, 0x20);

  /* dacs */

  outportb(VGADACM, 0xFF);

  outportb(VGADACW, 0x00);

  for (i=0; i<16; ++i) {
    outportb(VGADACD, vgadac[i].r >> 2);
    outportb(VGADACD, vgadac[i].g >> 2);
    outportb(VGADACD, vgadac[i].b >> 2);
  }

  vgalcgen();
  vgaerase();

  vgarow = 0;
  vgacol = 0;

  vgasetloc();

  writereg(0x3d4,0x29,0x85);    /* PR10: unlock PR11-17 */
  outportb(VGACRTA,0x2e);       /* PR15 */
  outportb(VGACRTD,0x20);       /* enable vclk0,vclk1 (again?) */

  j = 0x01;
  for (i=0;i<2000;i++)
	{
	    vgaputc((j++)&0x7f);
	}
printf("\nchars written ");

 /* hang loose for a bit, then scroll.. */
 for (i=0;i<5;i++)
	{
	volatile unsigned int j;
	for (j=0;j<0x3f;j++) 
		mb();
	for (j=0;scroll[j]!='\0';j++)
		vgaputc(scroll[j]);
	}

#ifdef DONT_GO_HIREZ
BAILOUT;
#endif

printf("\ninto SETMODE ");
 /* off into never-never land.. */
 SetExtMode(2,2);
 v_cols=1024;
 v_rows=768;
 SetPackedRead();
 SetPackedWrite();
 q_clearscreen();
 q_setcolor(0xf,0x0);

printf("\n returned from CLEARSCREEN ");



 /* initialize the putc variables */
 q_curcol=0;
 q_currow=0;
while (1)
 { /* try to put up a character.. */
 if ((j&3)==0)
   {
   q_setcolor((j>>2), 0x0);
   q_putchar('\n'); q_putchar(hex[(j>>6)&0xf]);q_putchar(hex[(j>>2)&0x7]);
   }
 j++;
 for (i=0;i<=127;i++)
	{
#ifdef TESTPATTERN
	q_putchar('E');
#else
	if (i != '\n')
	  q_putchar(i);
#endif /*TESTPATTERN*/
	}

 q_putchar('\n');
 /* now see if we can write characters.. */
}
 (*(int(*)())0)();


}



void vgalcgen()
{
  register long a;
  register int  i;
  register int  j;
  register unsigned char  *cp;

#ifdef DEBUG
  printf("vgalcgen\n");
  for (i=0;i<0xfffff;i++) ;
#endif


  outportb(VGASEQA, 0x02);
  outportb(VGASEQD, 0x04);
  outportb(VGASEQA, 0x04);
  outportb(VGASEQD, 0x06);
  outportb(VGAGFXA, 0x05);
  outportb(VGAGFXD, 0x00);
  outportb(VGAGFXA, 0x06);
  outportb(VGAGFXD, 0x05);

  a = (REALBASE+(VGAGSEG));

  for (i=0; i<256; ++i) {
    cp = &vga8x16xx[0];
#ifdef NEWFONTS
    if (i<128)
       cp = &basefont8x16[CHARHEIGHT*i];
#else
    if (i>=0x20 && i<=0x7F)
      cp = &vga8x16gl[16 * (i-0x20)];
#endif

#if LOADGR
    else if (i>=0xA0 && i<=0xFF)
      cp = &vga8x16gr[16 * (i-0xA0)];
#endif
    /* 16 lines */
    for (j=0; j<16; j+=2) {
      /* 8 bits */
      vgastl(a|LOBYTE, cp[j+0]);
      vgastl(a|HIBYTE, cp[j+1]);
      a += (ENC_BIT2);
    }

    /* 16 unused */

    for (j=0; j<16; j+=2) {
      /* byte cycles */
      vgastl(a|LOBYTE, 0xFF);
      vgastl(a|HIBYTE, 0xFF);
      a += (ENC_BIT2);
    }

  }                             

#ifdef DEBUG
  printf ("Char Gen Loaded ");
#endif


  outportb(VGASEQA, 0x02);      /* EGA SEQ char map sel */
  outportb(VGASEQD, 0x03);      /* ..a:1st 8k, b:4th 8k */
  outportb(VGASEQA, 0x04);      /* EGA Mem Mode Reg */
  outportb(VGASEQD, 0x03);      /* ..alpha mode, seq access, >64kB */
  outportb(VGAGFXA, 0x05);      /* Graphics mode reg */
  outportb(VGAGFXD, 0x10);      /* ..CGA compatible odd-even mode */
  outportb(VGAGFXA, 0x06);      /* Misc reg */
  outportb(VGAGFXD, 0x0E);      /* memory @B800-BFFF, A0 odd-even */

}



void vgaerase()
{
  register long a;
  register int  i;

#ifdef DEBUG
  printf("vgaerase\n");
  for (i=0;i<0xfffff;i++); 
#endif


  a = VGABASE;
  for (i=0; i<NROW*NCOL; ++i) {
    vgastl(a|LOBYTE, ' ');
    vgastl(a|HIBYTE, (BGROUND<<4)|(FGROUND<<0));
    a += (ENC_BIT2);
  }

}



void vgaputc(c)
register int    c;
{

  register int  i;
  register int  j;
  register long a1;
  register long a2;

  /* backspaces; won't work with proportional fonts without some work.. */
  if (c == '\b') {
    if (vgacol != 0) {
      --vgacol;
      vgasetloc();
    }

  } else if (c == '\n') {
    vgacol = 0;
    if (vgarow < NROW-1)
      ++vgarow;
    else {
      a1 = VGABASE;
      /* (NCOL%8) == 0 */
      a2 = VGABASE + ((NCOL<<3)<<1);
      for (i=0; i<NROW-1; ++i) {
	for (j=0; j<NCOL; ++j) {
#if VGA8BIT
	  vgastl(a1|HIBYTE, vgaldl(a2|HIBYTE)&0xFF);
	  vgastl(a1|LOBYTE, vgaldl(a2|LOBYTE)&0xFF);
#else
	  vgastl(a1|WWORD, vgaldl(a2|WWORD)&0xFFFF);
#endif
	  a1 += (ENC_BIT2);
	  a2 += (ENC_BIT2);
	}

      }

      for (j=0; j<NCOL; ++j) {
#if VGA8BIT
	vgastl(a1|LOBYTE, ' ');
	vgastl(a1|HIBYTE, (BGROUND<<4)|(FGROUND<<0));
#else
	vgastl(a1|WWORD, (BGROUND<<12)|(FGROUND<<8)|' ');
#endif
	a1 += (ENC_BIT2);
      }

    }

    vgasetloc();

  } else if ((c>=0x20 && c<=0x7F) || (c>=0xA0 && c<=0xFF)) {
    if (vgacol >= NCOL)
      vgaputc('\n');
#if VGA8BIT
    vgastl(vgaloc|LOBYTE, c&0xFF);
    vgastl(vgaloc|HIBYTE, (BGROUND<<4)|(FGROUND<<0));
#else
    vgastl(vgaloc|WWORD, (BGROUND<<12)|(FGROUND<<8)|(c&0xFF));
#endif

    if (vgacol < NCOL-1) {
      ++vgacol;
      vgasetloc();
    } else {
      vgasetloc();
      ++vgacol;
    }

  }

}



void vgasetloc()
{

  register int  offs,i;

  offs = (NCOL*vgarow) + vgacol;
  vgaloc = VGABASE + (offs<<4); /* ends up being <<8, which is every other */
				/* location (skipping the attr bytes..) */

  outportb(VGACRTA, 0x0E);
  outportb(VGACRTD, (offs>>8)&0xFF);
  outportb(VGACRTA, 0x0F);
  outportb(VGACRTD, (offs>>0)&0xFF);
}



#define xBYTEMASK 0x6000000

int vgaldl(a, d)
long    a;
int     d;
{
  unsigned int data,shift;
  data = (*((int *) (a << 4)));
  shift = 8*(((a)>>3)&3); 
  return (data>>shift);
}



void vgastl(a, d)
long    a;
int     d;
{

volatile int i;
unsigned int id,shift;

  shift = 8*( (a>>3)&3);
  id = d<<shift;
  *((int *) (a << 4)) = id;mb();
}



/* read the config of the VGA card.. */
/* this is a debug-only routine... */
#define CRTC 0x3d4

char pr_3cf[][6] = {
	"PR0A","PR0B","PR1","PR2","PR3","PR4","PR5"
		};

char pr_3b5[][6] = {
	"PR10","PR11","PR12","PR13","PR14","PR15","PR16","PR17"
	};

char pr_3c5[][6] = {
	"PR20","PR21"
	};

char pr_3c5a[][6] = {
	"PR30","PR31","PR32"
	};

void
dumpvga()
{
	unsigned int i;

	/* first, unlock all the registers */
	writereg(0x3ce,0xd,0x2); /* PR3 */
	writereg(0x3ce,0xf,0x5); /* PR5 */
	writereg(0x3d4,0x29,0x85); /* PR10 */

	for (i=0;i<0x19;i++)
		{
		printf("CRTC_x%02x:%02x\t",i,readreg(CRTC,i));
		if ((i&3)==3) printf("\n");
		}

	printf("MiscOut:%02x\t",inportb(0x3cc));
	printf("InSt0:%02x\t",inportb(0x3c2));
	printf("InSt1:%02x\t",inportb(0x3da));
	printf("FeaCtl:%02x\n",inportb(0x3ca));

	for (i=1;i<5;i++)
		{
		printf("SEQ_%02x:%02x\t",i,readreg(0x3c4,i));
		}

	printf("\n");

	for (i=0;i<9;i++)
		{
		printf("GCTL_%02x:%02x\t",i,readreg(0x3ce,i));
		if ((i&3)==3) printf("\n");
		}

	printf("CRTstat:%02x\n",inportb(0x3da));

	/* PR registers.. */
	for (i=9;i<0x10;i++)
		{
		printf("%s:%02x\t",pr_3cf[i-9],readreg(0x3ce,i));
		if(((i-9)&3)==3) printf("\n");
		}

	printf("\n");

	for (i=0x29;i<0x31;i++)
		{
		printf("%s:%02x\t",pr_3b5[i-0x29],readreg(0x3d4,i));
		if(((i-0x29)&3)==3) printf("\n");
		}

	printf("PR32:%02x\n",readreg(0x3c4,0x12));
}



/* read a register whose data register immediately follows the address
  register */

unsigned char readreg(sel,off)
unsigned int sel;
unsigned int off;
{

	outportb(sel,off);
	return((unsigned char)inportb(sel+1));
}



/* write a register whose data register immediately follows the address
  register */

void writereg(sel,off,dat)
unsigned int sel;
unsigned int off;
unsigned char dat;
{

	outportb(sel,off);
	outportb(sel+1,dat);
	return;
}


 /* new stuff for the Compaq card and hi-res modes goes here.. */

#define NUM_SLOTS 6
#define MODE_640 0              /* 640x480x8 */
#define MODE_1024_60 1          /* 1024x768x8 */
#define MODE_1024_72 2          /* 1024x768x8 */

#define NUM_MODES 3
#define Q_SEQCNT 5
#define Q_ATTRCNT 20
#define Q_GRFXCNT 9
#define Q_CRTCNT 25
/* defines to make the compaq stuff compatible */
#define inp inportb
#define outp outportb
#define outpw outportw

/* handy routines from the Compaq disk.. with their data tables.. */
/*************************************************************************
 * SetMode register values
 *
 * The following data tables are used by the SetMode function to load
 * the appropriate register values for a video mode switch.  Each table
 * contains 8 entries which can be either a single value or another table.
 * These entries correspond to register values for one of the eight new
 * Triton video modes as follows:
 *
 *            Video Mode       Mode Type         Table Index
 *            ----------       ----------	     -----------
 *                32            640x480x8             0
 *                37           1024x768x4             1
 *                38           1024x768x8             2
 *                3B           512x480x16             3
 *                3C           640x400x16             4
 *                3E           640x480x16             5
 *                4D           512x480x32             6
 *                4E           640x400x32             7
 *
 *
 * The first group of tables below contain one set of values which apply
 * to all classes of monitors.  For these tables, the first index selects
 * the video mode and the second (if applicable) selects the port index.
 * The second group of tables contain a set of values for each monitor
 * class.  For these tables, the first index selects the monitor class,
 * the second selects the video mode, and the third (if applicable) selects
 * the port index.  Four monitor classes are currently defined (see the
 * SetMode header) but more may be added by simply appending a new set of
 * register values for the new monitor class to the appropriate data tables
 * below and incrementing the #define constant, MON_CLASS_CNT.   Note that
 * monitor class 3 is a special case in that its true register values are
 * loaded at run time.
 *
 *************************************************************************/


/*************************************************************************
 * The following tables have only one set of values which apply to all
 * monitor classes.
 *************************************************************************/

/* Sequencer register values (3C5.00 - 3C5.04) */
BYTE abSeq[MODE_CNT][SEQ_CNT] = {
   { 0x01, 0x01, 0xff, 0x00, 0x0e },
   { 0x01, 0x01, 0xff, 0x00, 0x0e },
   { 0x01, 0x01, 0xff, 0x00, 0x0e },
   { 0x01, 0x01, 0xff, 0x00, 0x0e },
   { 0x01, 0x01, 0xff, 0x00, 0x0e },
   { 0x01, 0x01, 0xff, 0x00, 0x0e },
   { 0x01, 0x01, 0xff, 0x00, 0x0e },
   { 0x01, 0x01, 0xff, 0x00, 0x0e }  };

/* Attribute Controller register values (3C0.00 - 3C0.13h) */
BYTE abAttr[MODE_CNT][ATTR_CNT] = {
   { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
     0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x41, 0x00, 0x0f, 0x00 },
   { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
     0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x41, 0x00, 0x0f, 0x00 },
   { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
     0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x41, 0x00, 0x0f, 0x00 },
   { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
     0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x41, 0x00, 0x0f, 0x00 },
   { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
     0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x41, 0x00, 0x0f, 0x00 },
   { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
     0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x41, 0x00, 0x0f, 0x00 },
   { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
     0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x41, 0x00, 0x0f, 0x00 },
   { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
     0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x41, 0x00, 0x0f, 0x00 } };

/* Graphics Controller register values (3CF.00 - 3CF.08) */
BYTE abGraphics[MODE_CNT][GRFX_CNT] = {
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff } };

/* Control Register 1 values (63CA) */
BYTE abCtrlReg1[MODE_CNT] =
   { 0x03, 0x01, 0x03, 0x05, 0x05, 0x05, 0x07, 0x07 };

/* DAC Command Register 1 values (13C8) */
BYTE abDacCmd1[MODE_CNT] =
   { 0x40, 0x60, 0x40, 0x20, 0x20, 0x20, 0x00, 0x00 };

/* Overflow Register 1 values (3CF.42h) */
BYTE abOverflow1[MODE_CNT] =
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01 };


/*************************************************************************
 * The following tables have a different set of values for each monitor
 * class.
 *************************************************************************/

/* CRTC register values (3D5.00 - 3D5.18h) */
BYTE abCrtc[MON_CLASS_CNT][MODE_CNT][CRTC_CNT] = {
	{
   /* CRTC register values for monitor class 0 */
   { 0x5f, 0x4f, 0x50, 0x81, 0x53, 0x9f, 0x0b, 0x3e, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xea, 0x8c, 0xdf, 0x80, 0x00, 0xe5,
     0x03, 0Xe3, 0xff },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00 },
   { 0x4a, 0x3f, 0x3f, 0x8d, 0x42, 0x0c, 0x0e, 0x3e, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xec, 0x8e, 0xdf, 0x80, 0x00, 0xe5,
     0x06, 0xe3, 0xff },
   { 0x5f, 0x4f, 0x50, 0x81, 0x53, 0x9f, 0x0b, 0x3e, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0x84, 0x8f, 0xa0, 0x00, 0x95,
     0x03, 0xe3, 0xff },
   { 0x5f, 0x4f, 0x50, 0x81, 0x53, 0x9f, 0x0b, 0x3e, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xea, 0x8c, 0xdf, 0xa0, 0x00, 0xe5,
     0x03, 0xe3, 0xff },
   { 0x4a, 0x3f, 0x3f, 0x8d, 0x42, 0x0c, 0x0e, 0x3e, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xec, 0x8e, 0xdf, 0x00, 0x00, 0xe5,
     0x06, 0xe3, 0xff },
   { 0x5f, 0x4f, 0x50, 0x81, 0x53, 0x9f, 0x0b, 0x3e, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0x84, 0x8f, 0x40, 0x00, 0x95,
     0x03, 0xe3, 0xff }
   },
	{
   /* CRTC register values for monitor class 1 */
   { 0x5f, 0x4f, 0x50, 0x81, 0x53, 0x9f, 0x0b, 0x3e, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xea, 0x8c, 0xdf, 0x80, 0x00, 0xe5,
     0x03, 0xe3, 0xff },
#ifdef OLDTABLE /* this looks wrong.. */
   { 0xa1, 0x7f, 0x7f, 0x84, 0x87, 0x9d, 0x2e, 0xf5, 0x00, 0x60, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x8b, 0xff, 0x40, 0x00, 0xff,
     0x2e, 0xe3, 0xff },
#else /*OLDTABLE*/
   { 0x9b, 0x7f, 0x7f, 0x9e, 0x87, 0x17, 0x31, 0xf5, 0x00, 0x61, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x94, 0xff, 0x80, 0x00, 0xff,
     0x31, 0xe3, 0xff },
#endif /*OLDTABLE*/
   { 0xa1, 0x7f, 0x7f, 0x84, 0x85, 0x9b, 0x2e, 0xf5, 0x00, 0x60, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x8b, 0xff, 0x80, 0x00, 0xff,
     0x2e, 0xe3, 0xff },
   { 0x4a, 0x3f, 0x3f, 0x8d, 0x42, 0x0c, 0x0e, 0x3e, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xec, 0x8e, 0xdf, 0x80, 0x00, 0xe5,
     0x06, 0xe3, 0xff },
   { 0x5f, 0x4f, 0x50, 0x81, 0x53, 0x9f, 0x0b, 0x3e, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0x84, 0x8f, 0xa0, 0x00, 0x95,
     0x03, 0xe3, 0xff },
   { 0x5f, 0x4f, 0x50, 0x81, 0x53, 0x9f, 0x0b, 0x3e, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xea, 0x8c, 0xdf, 0xa0, 0x00, 0xe5,
     0x03, 0xe3, 0xff },
   { 0x4a, 0x3f, 0x3f, 0x8d, 0x42, 0x0c, 0x0e, 0x3e, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xec, 0x8e, 0xdf, 0x00, 0x00, 0xe5,
     0x06, 0xe3, 0xff },
   { 0x5f, 0x4f, 0x50, 0x81, 0x53, 0x9f, 0x0b, 0x3e, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0x84, 0x8f, 0x40, 0x00, 0x95,
     0x03, 0xe3, 0xff }
   },
   {
   /* CRTC register values for monitor class 2 */
   { 0x69, 0x4f, 0x55, 0x86, 0x58, 0x80, 0x56, 0xb2, 0x00, 0x60, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x83, 0xdf, 0x80, 0x00, 0x01,
     0x32, 0xe3, 0xff },
   { 0x9e, 0x7f, 0x7f, 0x81, 0x83, 0x93, 0x1e, 0xf1, 0x00, 0x60, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x83, 0xff, 0x40, 0x00, 0xff,
     0x1e, 0xe3, 0xff },
   { 0x9e, 0x7f, 0x7f, 0x81, 0x83, 0x93, 0x1e, 0xf1, 0x00, 0x60, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x83, 0xff, 0x80, 0x00, 0xff,
     0x1e, 0xe3, 0xff },
   { 0x54, 0x3f, 0x44, 0x92, 0x47, 0x0d, 0x56, 0xb2, 0x00, 0x60, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x83, 0xdf, 0x80, 0x00, 0x01,
     0x32, 0xe3, 0xff },
   { 0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x1f, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x9c, 0x8e, 0x8f, 0x50, 0x00, 0x96,
     0xb9, 0xe3, 0xff },
   { 0x69, 0x4f, 0x55, 0x86, 0x58, 0x80, 0x56, 0xb2, 0x00, 0x60, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x83, 0xdf, 0xa0, 0x00, 0x01,
     0x32, 0xe3, 0xff },
   { 0x54, 0x3f, 0x44, 0x92, 0x47, 0x0d, 0x56, 0xb2, 0x00, 0x60, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x83, 0xdf, 0x00, 0x00, 0x01,
     0x32, 0xe3, 0xff },
   { 0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x1f, 0x00, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x9c, 0x8e, 0x8f, 0x50, 0x00, 0x96,
     0xb9, 0xe3, 0xff }
   },
   {
   /* CRTC register array (loaded later) for monitor class 3 (3rd party) */
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00 }
   } };

/* Miscellaneous Output Reg. values (3C2) */
BYTE abMiscOut[MON_CLASS_CNT][MODE_CNT] =
   {
   /* Monitor class 0 */
   { 0xef, 0x00, 0x00, 0xf7, 0x6f, 0xef, 0xf7, 0x6f },

   /* Monitor class 1 */
   { 0xef, 0x27, 0x03, 0xf7, 0x6f, 0xef, 0xf7, 0x6f },

   /* Monitor class 2 */
   { 0xf3, 0x2b, 0x2b, 0xfb, 0x6f, 0xf3, 0xfb, 0x6f },

   /* Monitor class 3 (loaded later) */
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
   };

/* Overflow Register 2 values (3CF.51h) */
BYTE abOverflow2[MON_CLASS_CNT][MODE_CNT] =
   {
   /* Monitor class 0 */
   { 0x08, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08 },

   /* Monitor class 1 */
   { 0x08, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08 },

   /* Monitor class 2 */
   { 0x28, 0x00, 0x00, 0x08, 0x28, 0x28, 0x08, 0x28 },

   /* Monitor class 3 (loaded later) */
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
   };



/************************************************************************
 * Triton Default Palette & Gamma Correction Values
 ************************************************************************/

BYTE abDefaultPalette[256][3] = {
   { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0xa8 }, { 0x00, 0xa8, 0x00 },
   { 0x00, 0xa8, 0xa8 }, { 0xa8, 0x00, 0x00 }, { 0xa8, 0x00, 0xa8 },
   { 0xa8, 0x54, 0x00 }, { 0xa8, 0xa8, 0xa8 }, { 0x54, 0x54, 0x54 },
   { 0x54, 0x54, 0xfc }, { 0x54, 0xfc, 0x54 }, { 0x54, 0xfc, 0xfc },
   { 0xfc, 0x54, 0x54 }, { 0xfc, 0x54, 0xfc }, { 0xfc, 0xfc, 0x54 },
   { 0xfc, 0xfc, 0xfc }, { 0x00, 0x00, 0x00 }, { 0x14, 0x14, 0x14 },
   { 0x20, 0x20, 0x20 }, { 0x2c, 0x2c, 0x2c }, { 0x38, 0x38, 0x38 },
   { 0x44, 0x44, 0x44 }, { 0x50, 0x50, 0x50 }, { 0x60, 0x60, 0x60 },
   { 0x70, 0x70, 0x70 }, { 0x80, 0x80, 0x80 }, { 0x90, 0x90, 0x90 },
   { 0xa0, 0xa0, 0xa0 }, { 0xb4, 0xb4, 0xb4 }, { 0xc8, 0xc8, 0xc8 },
   { 0xe0, 0xe0, 0xe0 }, { 0xfc, 0xfc, 0xfc }, { 0x00, 0x00, 0xfc },
   { 0x40, 0x00, 0xfc }, { 0x7c, 0x00, 0xfc }, { 0xbc, 0x00, 0xfc },
   { 0xfc, 0x00, 0xfc }, { 0xfc, 0x00, 0xbc }, { 0xfc, 0x00, 0x7c },
   { 0xfc, 0x00, 0x40 }, { 0xfc, 0x00, 0x00 }, { 0xfc, 0x40, 0x00 },
   { 0xfc, 0x7c, 0x00 }, { 0xfc, 0xbc, 0x00 }, { 0xfc, 0xfc, 0x00 },
   { 0xbc, 0xfc, 0x00 }, { 0x7c, 0xfc, 0x00 }, { 0x40, 0xfc, 0x00 },
   { 0x00, 0xfc, 0x00 }, { 0x00, 0xfc, 0x40 }, { 0x00, 0xfc, 0x7c },
   { 0x00, 0xfc, 0xbc }, { 0x00, 0xfc, 0xfc }, { 0x00, 0xbc, 0xfc },
   { 0x00, 0x7c, 0xfc }, { 0x00, 0x40, 0xfc }, { 0x7c, 0x7c, 0xfc },
   { 0x9c, 0x7c, 0xfc }, { 0xbc, 0x7c, 0xfc }, { 0xdc, 0x7c, 0xfc },
   { 0xfc, 0x7c, 0xfc }, { 0xfc, 0x7c, 0xdc }, { 0xfc, 0x7c, 0xbc },
   { 0xfc, 0x7c, 0x9c }, { 0xfc, 0x7c, 0x7c }, { 0xfc, 0x9c, 0x7c },
   { 0xfc, 0xbc, 0x7c }, { 0xfc, 0xdc, 0x7c }, { 0xfc, 0xfc, 0x7c },
   { 0xdc, 0xfc, 0x7c }, { 0xbc, 0xfc, 0x7c }, { 0x9c, 0xfc, 0x7c },
   { 0x7c, 0xfc, 0x7c }, { 0x7c, 0xfc, 0x9c }, { 0x7c, 0xfc, 0xbc },
   { 0x7c, 0xfc, 0xdc }, { 0x7c, 0xfc, 0xfc }, { 0x7c, 0xdc, 0xfc },
   { 0x7c, 0xbc, 0xfc }, { 0x7c, 0x9c, 0xfc }, { 0xb4, 0xb4, 0xfc },
   { 0xc4, 0xb4, 0xfc }, { 0xd8, 0xb4, 0xfc }, { 0xe8, 0xb4, 0xfc },
   { 0xfc, 0xb4, 0xfc }, { 0xfc, 0xb4, 0xe8 }, { 0xfc, 0xb4, 0xd8 },
   { 0xfc, 0xb4, 0xc4 }, { 0xfc, 0xb4, 0xb4 }, { 0xfc, 0xc4, 0xb4 },
   { 0xfc, 0xd8, 0xb4 }, { 0xfc, 0xe8, 0xb4 }, { 0xfc, 0xfc, 0xb4 },
   { 0xe8, 0xfc, 0xb4 }, { 0xd8, 0xfc, 0xb4 }, { 0xc4, 0xfc, 0xb4 },
   { 0xb4, 0xfc, 0xb4 }, { 0xb4, 0xfc, 0xc4 }, { 0xb4, 0xfc, 0xd8 },
   { 0xb4, 0xfc, 0xe8 }, { 0xb4, 0xfc, 0xfc }, { 0xb4, 0xe8, 0xfc },
   { 0xb4, 0xd8, 0xfc }, { 0xb4, 0xc4, 0xfc }, { 0x00, 0x00, 0x70 },
   { 0x1c, 0x00, 0x70 }, { 0x38, 0x00, 0x70 }, { 0x54, 0x00, 0x70 },
   { 0x70, 0x00, 0x70 }, { 0x70, 0x00, 0x54 }, { 0x70, 0x00, 0x38 },
   { 0x70, 0x00, 0x1c }, { 0x70, 0x00, 0x00 }, { 0x70, 0x1c, 0x00 },
   { 0x70, 0x38, 0x00 }, { 0x70, 0x54, 0x00 }, { 0x70, 0x70, 0x00 },
   { 0x54, 0x70, 0x00 }, { 0x38, 0x70, 0x00 }, { 0x1c, 0x70, 0x00 },
   { 0x00, 0x70, 0x00 }, { 0x00, 0x70, 0x1c }, { 0x00, 0x70, 0x38 },
   { 0x00, 0x70, 0x54 }, { 0x00, 0x70, 0x70 }, { 0x00, 0x54, 0x70 },
   { 0x00, 0x38, 0x70 }, { 0x00, 0x1c, 0x70 }, { 0x38, 0x38, 0x70 },
   { 0x44, 0x38, 0x70 }, { 0x54, 0x38, 0x70 }, { 0x60, 0x38, 0x70 },
   { 0x70, 0x38, 0x70 }, { 0x70, 0x38, 0x60 }, { 0x70, 0x38, 0x54 },
   { 0x70, 0x38, 0x44 }, { 0x70, 0x38, 0x38 }, { 0x70, 0x44, 0x38 },
   { 0x70, 0x54, 0x38 }, { 0x70, 0x60, 0x38 }, { 0x70, 0x70, 0x38 },
   { 0x60, 0x70, 0x38 }, { 0x54, 0x70, 0x38 }, { 0x44, 0x70, 0x38 },
   { 0x38, 0x70, 0x38 }, { 0x38, 0x70, 0x44 }, { 0x38, 0x70, 0x54 },
   { 0x38, 0x70, 0x60 }, { 0x38, 0x70, 0x70 }, { 0x38, 0x60, 0x70 },
   { 0x38, 0x54, 0x70 }, { 0x38, 0x44, 0x70 }, { 0x50, 0x50, 0x70 },
   { 0x58, 0x50, 0x70 }, { 0x60, 0x50, 0x70 }, { 0x68, 0x50, 0x70 },
   { 0x70, 0x50, 0x70 }, { 0x70, 0x50, 0x68 }, { 0x70, 0x50, 0x60 },
   { 0x70, 0x50, 0x58 }, { 0x70, 0x50, 0x50 }, { 0x70, 0x58, 0x50 },
   { 0x70, 0x60, 0x50 }, { 0x70, 0x68, 0x50 }, { 0x70, 0x70, 0x50 },
   { 0x68, 0x70, 0x50 }, { 0x60, 0x70, 0x50 }, { 0x58, 0x70, 0x50 },
   { 0x50, 0x70, 0x50 }, { 0x50, 0x70, 0x58 }, { 0x50, 0x70, 0x60 },
   { 0x50, 0x70, 0x68 }, { 0x50, 0x70, 0x70 }, { 0x50, 0x68, 0x70 },
   { 0x50, 0x60, 0x70 }, { 0x50, 0x58, 0x70 }, { 0x00, 0x00, 0x40 },
   { 0x10, 0x00, 0x40 }, { 0x20, 0x00, 0x40 }, { 0x30, 0x00, 0x40 },
   { 0x40, 0x00, 0x40 }, { 0x40, 0x00, 0x30 }, { 0x40, 0x00, 0x20 },
   { 0x40, 0x00, 0x10 }, { 0x40, 0x00, 0x00 }, { 0x40, 0x10, 0x00 },
   { 0x40, 0x20, 0x00 }, { 0x40, 0x30, 0x00 }, { 0x40, 0x40, 0x00 },
   { 0x30, 0x40, 0x00 }, { 0x20, 0x40, 0x00 }, { 0x10, 0x40, 0x00 },
   { 0x00, 0x40, 0x00 }, { 0x00, 0x40, 0x10 }, { 0x00, 0x40, 0x20 },
   { 0x00, 0x40, 0x30 }, { 0x00, 0x40, 0x40 }, { 0x00, 0x30, 0x40 },
   { 0x00, 0x20, 0x40 }, { 0x00, 0x10, 0x40 }, { 0x20, 0x20, 0x40 },
   { 0x28, 0x20, 0x40 }, { 0x30, 0x20, 0x40 }, { 0x38, 0x20, 0x40 },
   { 0x40, 0x20, 0x40 }, { 0x40, 0x20, 0x38 }, { 0x40, 0x20, 0x30 },
   { 0x40, 0x20, 0x28 }, { 0x40, 0x20, 0x20 }, { 0x40, 0x28, 0x20 },
   { 0x40, 0x30, 0x20 }, { 0x40, 0x38, 0x20 }, { 0x40, 0x40, 0x20 },
   { 0x38, 0x40, 0x20 }, { 0x30, 0x40, 0x20 }, { 0x28, 0x40, 0x20 },
   { 0x20, 0x40, 0x20 }, { 0x20, 0x40, 0x28 }, { 0x20, 0x40, 0x30 },
   { 0x20, 0x40, 0x38 }, { 0x20, 0x40, 0x40 }, { 0x20, 0x38, 0x40 },
   { 0x20, 0x30, 0x40 }, { 0x20, 0x28, 0x40 }, { 0x2c, 0x2c, 0x40 },
   { 0x30, 0x2c, 0x40 }, { 0x34, 0x2c, 0x40 }, { 0x3c, 0x2c, 0x40 },
   { 0x40, 0x2c, 0x40 }, { 0x40, 0x2c, 0x3c }, { 0x40, 0x2c, 0x34 },
   { 0x40, 0x2c, 0x30 }, { 0x40, 0x2c, 0x2c }, { 0x40, 0x30, 0x2c },
   { 0x40, 0x34, 0x2c }, { 0x40, 0x3c, 0x2c }, { 0x40, 0x40, 0x2c },
   { 0x3c, 0x40, 0x2c }, { 0x34, 0x40, 0x2c }, { 0x30, 0x40, 0x2c },
   { 0x2c, 0x40, 0x2c }, { 0x2c, 0x40, 0x30 }, { 0x2c, 0x40, 0x34 },
   { 0x2c, 0x40, 0x3c }, { 0x2c, 0x40, 0x40 }, { 0x2c, 0x3c, 0x40 },
   { 0x2c, 0x34, 0x40 }, { 0x2c, 0x30, 0x40 }, { 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00 } };

BYTE abDefaultGamma[256][3] = {
   { 0x00, 0x00, 0x00 }, { 0x1c, 0x17, 0x17 }, { 0x25, 0x1f, 0x1f },
   { 0x2b, 0x25, 0x25 }, { 0x30, 0x2a, 0x2a }, { 0x35, 0x2e, 0x2e },
   { 0x39, 0x32, 0x32 }, { 0x3d, 0x35, 0x35 }, { 0x40, 0x39, 0x39 },
   { 0x43, 0x3c, 0x3c }, { 0x46, 0x3e, 0x3e }, { 0x49, 0x41, 0x41 },
   { 0x4b, 0x44, 0x44 }, { 0x4e, 0x46, 0x46 }, { 0x50, 0x48, 0x48 },
   { 0x52, 0x4a, 0x4a }, { 0x54, 0x4d, 0x4d }, { 0x56, 0x4f, 0x4f },
   { 0x58, 0x51, 0x51 }, { 0x5a, 0x52, 0x52 }, { 0x5c, 0x54, 0x54 },
   { 0x5e, 0x56, 0x56 }, { 0x60, 0x58, 0x58 }, { 0x61, 0x5a, 0x5a },
   { 0x63, 0x5b, 0x5b }, { 0x65, 0x5d, 0x5d }, { 0x66, 0x5e, 0x5e },
   { 0x68, 0x60, 0x60 }, { 0x69, 0x62, 0x62 }, { 0x6b, 0x63, 0x63 },
   { 0x6c, 0x65, 0x65 }, { 0x6e, 0x66, 0x66 }, { 0x6f, 0x67, 0x67 },
   { 0x71, 0x69, 0x69 }, { 0x72, 0x6a, 0x6a }, { 0x73, 0x6c, 0x6c },
   { 0x75, 0x6d, 0x6d }, { 0x76, 0x6e, 0x6e }, { 0x77, 0x6f, 0x6f },
   { 0x78, 0x71, 0x71 }, { 0x7a, 0x72, 0x72 }, { 0x7b, 0x73, 0x73 },
   { 0x7c, 0x74, 0x74 }, { 0x7d, 0x76, 0x76 }, { 0x7e, 0x77, 0x77 },
   { 0x7f, 0x78, 0x78 }, { 0x81, 0x79, 0x79 }, { 0x82, 0x7a, 0x7a },
   { 0x83, 0x7b, 0x7b }, { 0x84, 0x7c, 0x7c }, { 0x85, 0x7e, 0x7e },
   { 0x86, 0x7f, 0x7f }, { 0x87, 0x80, 0x80 }, { 0x88, 0x81, 0x81 },
   { 0x89, 0x82, 0x82 }, { 0x8a, 0x83, 0x83 }, { 0x8b, 0x84, 0x84 },
   { 0x8c, 0x85, 0x85 }, { 0x8d, 0x86, 0x86 }, { 0x8e, 0x87, 0x87 },
   { 0x8f, 0x88, 0x88 }, { 0x90, 0x89, 0x89 }, { 0x91, 0x8a, 0x8a },
   { 0x92, 0x8b, 0x8b }, { 0x93, 0x8c, 0x8c }, { 0x94, 0x8d, 0x8d },
   { 0x95, 0x8e, 0x8e }, { 0x95, 0x8f, 0x8f }, { 0x96, 0x90, 0x90 },
   { 0x97, 0x90, 0x90 }, { 0x98, 0x91, 0x91 }, { 0x99, 0x92, 0x92 },
   { 0x9a, 0x93, 0x93 }, { 0x9b, 0x94, 0x94 }, { 0x9b, 0x95, 0x95 },
   { 0x9c, 0x96, 0x96 }, { 0x9d, 0x97, 0x97 }, { 0x9e, 0x98, 0x98 },
   { 0x9f, 0x98, 0x98 }, { 0xa0, 0x99, 0x99 }, { 0xa0, 0x9a, 0x9a },
   { 0xa1, 0x9b, 0x9b }, { 0xa2, 0x9c, 0x9c }, { 0xa3, 0x9d, 0x9d },
   { 0xa4, 0x9d, 0x9d }, { 0xa4, 0x9e, 0x9e }, { 0xa5, 0x9f, 0x9f },
   { 0xa6, 0xa0, 0xa0 }, { 0xa7, 0xa1, 0xa1 }, { 0xa7, 0xa1, 0xa1 },
   { 0xa8, 0xa2, 0xa2 }, { 0xa9, 0xa3, 0xa3 }, { 0xaa, 0xa4, 0xa4 },
   { 0xaa, 0xa4, 0xa4 }, { 0xab, 0xa5, 0xa5 }, { 0xac, 0xa6, 0xa6 },
   { 0xad, 0xa7, 0xa7 }, { 0xad, 0xa8, 0xa8 }, { 0xae, 0xa8, 0xa8 },
   { 0xaf, 0xa9, 0xa9 }, { 0xaf, 0xaa, 0xaa }, { 0xb0, 0xaa, 0xaa },
   { 0xb1, 0xab, 0xab }, { 0xb1, 0xac, 0xac }, { 0xb2, 0xad, 0xad },
   { 0xb3, 0xad, 0xad }, { 0xb3, 0xae, 0xae }, { 0xb4, 0xaf, 0xaf },
   { 0xb5, 0xb0, 0xb0 }, { 0xb6, 0xb0, 0xb0 }, { 0xb6, 0xb1, 0xb1 },
   { 0xb7, 0xb2, 0xb2 }, { 0xb7, 0xb2, 0xb2 }, { 0xb8, 0xb3, 0xb3 },
   { 0xb9, 0xb4, 0xb4 }, { 0xb9, 0xb4, 0xb4 }, { 0xba, 0xb5, 0xb5 },
   { 0xbb, 0xb6, 0xb6 }, { 0xbb, 0xb6, 0xb6 }, { 0xbc, 0xb7, 0xb7 },
   { 0xbd, 0xb8, 0xb8 }, { 0xbd, 0xb8, 0xb8 }, { 0xbe, 0xb9, 0xb9 },
   { 0xbe, 0xba, 0xba }, { 0xbf, 0xba, 0xba }, { 0xc0, 0xbb, 0xbb },
   { 0xc0, 0xbc, 0xbc }, { 0xc1, 0xbc, 0xbc }, { 0xc2, 0xbd, 0xbd },
   { 0xc2, 0xbe, 0xbe }, { 0xc3, 0xbe, 0xbe }, { 0xc3, 0xbf, 0xbf },
   { 0xc4, 0xc0, 0xc0 }, { 0xc5, 0xc0, 0xc0 }, { 0xc5, 0xc1, 0xc1 },
   { 0xc6, 0xc1, 0xc1 }, { 0xc6, 0xc2, 0xc2 }, { 0xc7, 0xc3, 0xc3 },
   { 0xc7, 0xc3, 0xc3 }, { 0xc8, 0xc4, 0xc4 }, { 0xc9, 0xc4, 0xc4 },
   { 0xc9, 0xc5, 0xc5 }, { 0xca, 0xc6, 0xc6 }, { 0xca, 0xc6, 0xc6 },
   { 0xcb, 0xc7, 0xc7 }, { 0xcb, 0xc8, 0xc8 }, { 0xcc, 0xc8, 0xc8 },
   { 0xcd, 0xc9, 0xc9 }, { 0xcd, 0xc9, 0xc9 }, { 0xce, 0xca, 0xca },
   { 0xce, 0xca, 0xca }, { 0xcf, 0xcb, 0xcb }, { 0xcf, 0xcc, 0xcc },
   { 0xd0, 0xcc, 0xcc }, { 0xd0, 0xcd, 0xcd }, { 0xd1, 0xcd, 0xcd },
   { 0xd1, 0xce, 0xce }, { 0xd2, 0xcf, 0xcf }, { 0xd3, 0xcf, 0xcf },
   { 0xd3, 0xd0, 0xd0 }, { 0xd4, 0xd0, 0xd0 }, { 0xd4, 0xd1, 0xd1 },
   { 0xd5, 0xd1, 0xd1 }, { 0xd5, 0xd2, 0xd2 }, { 0xd6, 0xd2, 0xd2 },
   { 0xd6, 0xd3, 0xd3 }, { 0xd7, 0xd4, 0xd4 }, { 0xd7, 0xd4, 0xd4 },
   { 0xd8, 0xd5, 0xd5 }, { 0xd8, 0xd5, 0xd5 }, { 0xd9, 0xd6, 0xd6 },
   { 0xd9, 0xd6, 0xd6 }, { 0xda, 0xd7, 0xd7 }, { 0xda, 0xd7, 0xd7 },
   { 0xdb, 0xd8, 0xd8 }, { 0xdb, 0xd8, 0xd8 }, { 0xdc, 0xd9, 0xd9 },
   { 0xdc, 0xda, 0xda }, { 0xdd, 0xda, 0xda }, { 0xdd, 0xdb, 0xdb },
   { 0xde, 0xdb, 0xdb }, { 0xde, 0xdc, 0xdc }, { 0xdf, 0xdc, 0xdc },
   { 0xdf, 0xdd, 0xdd }, { 0xe0, 0xdd, 0xdd }, { 0xe0, 0xde, 0xde },
   { 0xe1, 0xde, 0xde }, { 0xe1, 0xdf, 0xdf }, { 0xe2, 0xdf, 0xdf },
   { 0xe2, 0xe0, 0xe0 }, { 0xe3, 0xe0, 0xe0 }, { 0xe3, 0xe1, 0xe1 },
   { 0xe4, 0xe1, 0xe1 }, { 0xe4, 0xe2, 0xe2 }, { 0xe5, 0xe2, 0xe2 },
   { 0xe5, 0xe3, 0xe3 }, { 0xe6, 0xe3, 0xe3 }, { 0xe6, 0xe4, 0xe4 },
   { 0xe6, 0xe4, 0xe4 }, { 0xe7, 0xe5, 0xe5 }, { 0xe7, 0xe5, 0xe5 },
   { 0xe8, 0xe6, 0xe6 }, { 0xe8, 0xe6, 0xe6 }, { 0xe9, 0xe7, 0xe7 },
   { 0xe9, 0xe7, 0xe7 }, { 0xea, 0xe8, 0xe8 }, { 0xea, 0xe8, 0xe8 },
   { 0xeb, 0xe9, 0xe9 }, { 0xeb, 0xe9, 0xe9 }, { 0xeb, 0xea, 0xea },
   { 0xec, 0xea, 0xea }, { 0xec, 0xeb, 0xeb }, { 0xed, 0xeb, 0xeb },
   { 0xed, 0xec, 0xec }, { 0xee, 0xec, 0xec }, { 0xee, 0xed, 0xed },
   { 0xef, 0xed, 0xed }, { 0xef, 0xee, 0xee }, { 0xf0, 0xee, 0xee },
   { 0xf0, 0xef, 0xef }, { 0xf0, 0xef, 0xef }, { 0xf1, 0xf0, 0xf0 },
   { 0xf1, 0xf0, 0xf0 }, { 0xf2, 0xf1, 0xf1 }, { 0xf2, 0xf1, 0xf1 },
   { 0xf3, 0xf1, 0xf1 }, { 0xf3, 0xf2, 0xf2 }, { 0xf3, 0xf2, 0xf2 },
   { 0xf4, 0xf3, 0xf3 }, { 0xf4, 0xf3, 0xf3 }, { 0xf5, 0xf4, 0xf4 },
   { 0xf5, 0xf4, 0xf4 }, { 0xf6, 0xf5, 0xf5 }, { 0xf6, 0xf5, 0xf5 },
   { 0xf6, 0xf6, 0xf6 }, { 0xf7, 0xf6, 0xf6 }, { 0xf7, 0xf7, 0xf7 },
   { 0xf8, 0xf7, 0xf7 }, { 0xf8, 0xf7, 0xf7 }, { 0xf8, 0xf8, 0xf8 },
   { 0xf9, 0xf8, 0xf8 }, { 0xf9, 0xf9, 0xf9 }, { 0xfa, 0xf9, 0xf9 },
   { 0xfa, 0xfa, 0xfa }, { 0xfb, 0xfa, 0xfa }, { 0xfb, 0xfb, 0xfb },
   { 0xfb, 0xfb, 0xfb }, { 0xfc, 0xfb, 0xfb }, { 0xfc, 0xfc, 0xfc },
   { 0xfd, 0xfc, 0xfc }, { 0xfd, 0xfd, 0xfd }, { 0xfd, 0xfd, 0xfd },
   { 0xfe, 0xfe, 0xfe }, { 0xfe, 0xfe, 0xfe }, { 0xff, 0xff, 0xff },
   { 0xff, 0xff, 0xff } };



/*************************************************************************
 * CONSTANT DEFINITIONS
 *************************************************************************/

/*************************************************************************
 * EXTERNAL REFERENCES
 *************************************************************************/

/*************************************************************************
 * GLOBAL VARIABLES
 *************************************************************************/

/*************************************************************************
 * FUNCTION PROTOTYPES
 *************************************************************************/



/*************************************************************************
 * SetExtMode()
 *
 * DESCRIPTION:
 *   This function sets Triton to any of the extended video modes without
 *   using BIOS calls.  The video mode to be set is selected by passing in
 *   an index used to access the proper register values in setmode.h.  A
 *   monitor class must also be passed in which indexes the appropriate
 *   timing parameters in setmode.h for the desired monitor.  Two classes
 *   of monitors are currently supported for each extended video mode.
 *   The mode indices and supported monitor classes for each video mode are
 *   shown in the following table.
 *
 *         Video Mode     Mode Index      Monitor Classes Supported
 *        ----------------------------------------------------------
 *            32h             0                      0, 2
 *            37h             1                      1, 2
 *            38h             2                      1, 2
 *            3Bh             3                      0, 2
 *            3Ch             4                      0, 2
 *            3Eh             5                      0, 2
 *            4Dh             6                      0, 2
 *            4Eh             7                      0, 2
 *								
	        
 *   The monitor class is defined by the horizontal and vertical refresh 
 *   rates supported by a monitor for a given video mode.  The vertical
 *   refresh rates for classes 0 - 3 are shown below.  The timing values
 *   for monitor class 3 are loaded by another function which must be
 *   called before class 3 becomes a valid monitor class.
 *
 *    Monitor    Monitor       Vertical Refresh Rate (in Hz) for Mode:
 *     Class      Type      32h   37h   38h   3Bh   3Ch   3Eh   4Dh   4Eh
 *   ---------  ---------  -----------------------------------------------
 *       0         VGA      60    --    --    60    60    60    60    60
 *       1        AG1024    60    66    66    60    60    60    60    60
 *       2       New 1024   75    72    72    75    70    75    75    70
 *       3       3rd Prty    ?     ?     ?     ?     ?     ?     ?     ?
 *
 *   New monitor classes may be added by appending new register values to 
 *   the data tables in setmode.h.  Passing in an unknown monitor class 
 *   will cause SetMode to return an error.
 *
 * INPUT:
 *   ibMode:      mode index into header file register tables (0..7)
 *   ibMonClass:  monitor class (another index into reg. tables) (0..2)
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   0 = set mode performed
 *   1 = illegal mode or monitor class, set mode aborted
 *
 *************************************************************************/

USHORT SetExtMode( BYTE ibMode, 
                   BYTE ibMonClass )
{
   USHORT iusLoop;

   /* check for valid mode and monitor class */
   outpw( GC_INDEX, 0x050F);      /* unlock extended graphics regs */
   outp( GC_INDEX, ENV_REG_1);
   if (((ibMode == MODE_37) || (ibMode == MODE_38)) && (ibMonClass == 0))
      return( 1);                                          /* error */
#ifdef FOOFOO /*...tek*/
   if (((inp(GC_DATA) & 0x80) == 0) && (ibMonClass == 3))
      return( 1);                                          /* error */
#endif /*FOOFOO*/
   if ((ibMode >= MODE_CNT) || (ibMonClass >= MON_CLASS_CNT))
      return( 1);                                          /* error */

   /* turn video off */
   inp( 0x3da);                   /* reset latch */
   outp( ATTR_INDEX, 0x00);

   /* set the sequencer */
   for (iusLoop = 0; iusLoop < SEQ_CNT; iusLoop++)
      {                           /* synchronous seq reset, load seq regs */
      outp( SEQ_INDEX, iusLoop);
      outp( SEQ_DATA, abSeq[ibMode][iusLoop]);
      };
   outp( SEQ_INDEX, 0x00);
   outp( SEQ_DATA, 0x03);         /* restart the sequencer */

   /* unlock extended graphics registers */
   outp( GC_INDEX, 0x0f);
   outp( GC_DATA, 0x05);

   /* set Adv VGA mode (set bit 0 of Ctrl Reg 0) */
   outp( GC_INDEX, 0x40);
   outp( GC_DATA, 0x01);

   /* fix sequencer pixel mask for 8 bits */
   outp( SEQ_INDEX, SEQ_PIXEL_WR_MSK);
   outp( SEQ_DATA, 0xff);

   /* set BitBLT enable (unlocks other Triton extended registers) */
   outp( GC_INDEX, 0x10);
   outp( GC_DATA, 0x08);

   /* set Triton mode, set bits per pixel */
   outp( CTRL_REG_1, abCtrlReg1[ibMode]);

   /* load Misc Output reg */
   outp( MISC_OUTPUT, abMiscOut[ibMonClass][ibMode]);

   /* load DAC Cmd regs */
   outp( DAC_CMD_0, 0x02);                  /* 8-bit DAC */
   outp( DAC_CMD_1, abDacCmd1[ibMode]);     /* bits per pixel */
   outp( DAC_CMD_2, 0x20);                  /* set PortSel mask */

   /* load CRTC parameters */
   outp( CRTC_INDEX, 0x11);
   outp( CRTC_DATA, 0x00);        /* unlock CRTC regs 0-7 */
   for (iusLoop = 0; iusLoop < CRTC_CNT; iusLoop++)
      {
      outp( CRTC_INDEX, iusLoop);
      outp( CRTC_DATA, abCrtc[ibMonClass][ibMode][iusLoop]);
      }
   outp( GC_INDEX, 0x42);
   outp( GC_DATA, abOverflow1[ibMode]);
   outp( GC_INDEX, 0x51);
   outp( GC_DATA, abOverflow2[ibMonClass][ibMode]);

   /* load overscan color (black) */
   outp( CO_COLOR_WRITE, 0x00);
   outp( CO_COLOR_DATA, 0x00);    /* red component */
   outp( CO_COLOR_DATA, 0x00);    /* green component */
   outp( CO_COLOR_DATA, 0x00);    /* blue component */

   /* load attribute regs */
   inp( 0x3da);                   /* reset latch */
   for (iusLoop = 0; iusLoop < ATTR_CNT; iusLoop++)
      {
      outp( ATTR_INDEX, iusLoop);
      outp( ATTR_DATA, abAttr[ibMode][iusLoop]);
      }

   /* load graphics regs */
   for (iusLoop = 0; iusLoop < GRFX_CNT; iusLoop++)
      {
      outp( GC_INDEX, iusLoop);
      outp( GC_DATA, abGraphics[ibMode][iusLoop]);
      }

   if (ibMode < MODE_3B)     /* true color mode? */
      {
      /* 4 or 8 bpp mode --> load default palette */
      outp( PALETTE_WRITE, 0x00);
      for (iusLoop = 0; iusLoop < 256; iusLoop++)
         {
         outp( PALETTE_DATA, abDefaultPalette[iusLoop][0]);     /* red */
         outp( PALETTE_DATA, abDefaultPalette[iusLoop][1]);     /* green */
         outp( PALETTE_DATA, abDefaultPalette[iusLoop][2]);     /* blue */
         }
      }
   else
      {
      /* true color mode --> load default gamma correction table */
      outp( PALETTE_WRITE, 0x00);
      for (iusLoop = 0; iusLoop < 256; iusLoop++)
         {
         outp( PALETTE_DATA, abDefaultGamma[iusLoop][0]);       /* red */
         outp( PALETTE_DATA, abDefaultGamma[iusLoop][1]);       /* green */
         outp( PALETTE_DATA, abDefaultGamma[iusLoop][2]);       /* blue */
         }
      }

   /* clear frame buffer */
   ClearScreen();

   /* turn video on */
   inp( 0x3da);                   /* reset latch */
   outp( ATTR_INDEX, 0x20);

   return( 0);                    /* no error */
}  /* SetExtMode() */


/*************************************************************************
 * ClearScreen()
 *
 * DESCRIPTION:
 *   This function uses a pattern-to-screen BitBLT operation to clear the
 *   video screen.  The BitBLT engine is reset after the operation.
 *
 * INPUT:
 *   none
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT ClearScreen( VOID)
{
   unsigned char bHoldReg;

   /* set datapath source for pattern-to-screen BitBLT */
   outp( GC_INDEX, DATAPATH_CTRL);
   bHoldReg = inp( GC_DATA );            /* save old Datapath Control reg */
   outp( GC_DATA, ROPSELECT_NO_ROPS |
                  PIXELMASK_ONLY |
                  PLANARMASK_NONE_0XFF |
                  SRC_IS_PATTERN_REGS );

   /* set pattern register for blank pattern */
   outp( PREG_4, 0x00);
   outp( PREG_5, 0x00);
   outp( PREG_6, 0x00);
   outp( PREG_7, 0x00);
   outp( PREG_0, 0x00);
   outp( PREG_1, 0x00);
   outp( PREG_2, 0x00);
   outp( PREG_3, 0x00);

   /* set up BitBLT registers and start engine */
   outpw( DEST_ADDR_LO, 0x00);
   outpw( DEST_ADDR_HI, 0x00);
   outpw( BITMAP_WIDTH, 1024);
   outpw( BITMAP_HEIGHT, 1023);
   outp( BLT_CMD_1, XY_SRC_ADDR |
                    XY_DEST_ADDR );
   outp( BLT_CMD_0, FORWARD |
                    START_BLT );

   /* wait for idle h/w */
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT);

   /* reset the BLT Engine to default state */
   outp( GC_INDEX, 0x10);
   outp( GC_DATA,0x48);
   outp( GC_DATA,0x08);

   /* restore Datapath Control Reg */
   outp( GC_INDEX, DATAPATH_CTRL);
   outp( GC_DATA, bHoldReg);
}  /* ClearScreen() */

/*************************************************************************
 * ClearScreen()
 *
 * DESCRIPTION:
 *   This function uses a pattern-to-screen BitBLT operation to clear the
 *   video screen.  The BitBLT engine is reset after the operation.
 *
 * INPUT:
 *   none
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT ClearLastLine( VOID)
{
   unsigned char bHoldReg;
   while(inportb(CTRL_REG_1) & GLOBAL_BUSY_BIT)
	; /* wait for the chips to cool down.. */
   /* set datapath source for pattern-to-screen BitBLT */
   outp( GC_INDEX, DATAPATH_CTRL);
   bHoldReg = inp( GC_DATA);            /* save old Datapath Control reg */
   outp( GC_DATA, ROPSELECT_NO_ROPS |
                  PIXELMASK_ONLY |
                  PLANARMASK_NONE_0XFF |
                  SRC_IS_PATTERN_REGS );
   /* set pattern register for blank pattern */
   outp( PREG_4, 0x00);
   outp( PREG_5, 0x00);
   outp( PREG_6, 0x00);
   outp( PREG_7, 0x00);
   outp( PREG_0, 0x00);
   outp( PREG_1, 0x00);
   outp( PREG_2, 0x00);
   outp( PREG_3, 0x00);

   /* set up BitBLT registers and start engine */
   outpw( DEST_ADDR_LO, 0x00);
   outpw( DEST_ADDR_HI, v_rows-CHARHEIGHT);
   outpw( BITMAP_WIDTH, 1024);
   outpw( BITMAP_HEIGHT, CHARHEIGHT);
   outp( BLT_CMD_1, XY_SRC_ADDR |
                    XY_DEST_ADDR );
   outp( BLT_CMD_0, FORWARD |
                    START_BLT );

   /* wait for idle h/w */
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT);

   /* reset the BLT Engine to default state */
   outp( GC_INDEX, 0x10);
   outp( GC_DATA,0x48);
   outp( GC_DATA,0x08);

   /* restore Datapath Control Reg */
   outp( GC_INDEX, DATAPATH_CTRL);
   outp( GC_DATA, bHoldReg);
}  /* ClearLastLine() */




int qvisionPresent= -1;

unsigned char q_seq[NUM_MODES][Q_SEQCNT] = {
	{ 0x03, 0x01, 0xff, 0x00, 0x0e },  
	{ 0x03, 0x01, 0xff, 0x00, 0x0e },
	{ 0x03, 0x01, 0xff, 0x00, 0x0e }    };



unsigned char q_attr[NUM_MODES][Q_ATTRCNT] = {
	{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	 0x41, 0x00, 0x0f, 0x00 },

	{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	 0x41, 0x00, 0x0f, 0x00 },

	{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	 0x41, 0x00, 0x0f, 0x00 }  };


unsigned char q_grfx[NUM_MODES][Q_GRFXCNT] = {
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff} ,
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff} , 
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff} };


unsigned char q_ctlreg1[NUM_MODES] = 
	{0x03, 0x03, 0x03};


unsigned char q_cmdreg1[NUM_MODES] = 
	{0x40, 0x40, 0x40};


unsigned char q_over1[NUM_MODES] = 
	{0x0, 0x0, 0x0};


unsigned char q_crtc[NUM_MODES][Q_CRTCNT] = {
	{0x5f, 0x4f, 0x50, 0x81, 0x53, 0x9f, 0x0b, 0x3e, 0x00, 0x40, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0xea, 0x8c, 0xdf, 0x80, 0x00, 0xe5,
	 0x03, 0xe3, 0xff}, /* 640x480, std VGA monitor */

	{0x9b, 0x7f, 0x7f, 0x9e, 0x87, 0x17, 0x31, 0xf5, 0x00, 0x60, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x94, 0xff, 0x80, 0x00, 0xff,
	 0x31, 0xe3, 0xff}, /* 1024x768, 60Hz vertical */

	{0x9e, 0x7f, 0x7f, 0x81, 0x83, 0x93, 0x1e, 0xf1, 0x00, 0x60, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x83, 0xff, 0x80, 0x00, 0xff,
	 0x1e, 0xe3, 0xff}  /* 1024x768, 72Hz vertical */
	    };


unsigned char q_miscout[NUM_MODES] = 
	{ 0xef, 0xe3, 0x2b };


unsigned char q_over2[NUM_MODES] = 
	{ 0x08, 0x00, 0x00 };


void q_setmode(mode)
unsigned int mode;
{

int     i;



/* first, we need to figure out if there is a qvision card in the machine
 * or not. we do this by trolling the slot-specific addresses xC80h for
 * the EISA-Triton specific value 0x1130110e. Remeber the slot in the 
 * qvisionPresent variable. */

	for (i=1;i<NUM_SLOTS+1;i++)
		{
		unsigned int j;
		j = inportl((i*0x1000)+0xc80);
		if ( (j&0x0fffffff) == 0x130110e)
			{
			qvisionPresent = i;
			printf("\nTriton found in slot %d",i);
			break;
			}
		}
	if (qvisionPresent < 1)
		{
		printf("Triton VGA not found. Mode not changed.\n");
		return;
		}
	/* start slamming around registers to set the mode */
	/* unlock all registers.. */
	outportw(GC_INDEX, 0X050F);
	/* video off */
	inportb(0x3da);
	outportb(ATTR_INDEX, 0x00);
	/* load sequencer */
	for (i=0;i<Q_SEQCNT;i++)
		{
		outportb(SEQ_INDEX,i);
		outportb(SEQ_DATA,q_seq[mode][i]);
		}

	/* unlock extended registers */
	outportb(GC_INDEX, 0x0f);
	outportb(GC_DATA, 0x05);
	/* set advanced VGA mode, packed view. */
	writereg(GC_INDEX, 0x40,0x03);

	/* set seq pixel mask to 8 bits */
	writereg(SEQ_INDEX,0x02,0xff);

	/* set bitblt enable, unlock Trition registers */
	writereg(GC_INDEX,0x10,0x08);
	/* set Triton mode */
	outportb(CTRL_REG_1,q_ctlreg1[mode]);
	/* load misc out reg */
	outportb(MISC_OUTPUT, q_miscout[mode]);
	/* DAC registers.. */
	outportb(DAC_CMD_0,0x02);	/* 8-bit */
	outportb(DAC_CMD_1,q_cmdreg1[mode]);
	outportb(DAC_CMD_2,0x20);
	/* load the CRTC registers.. */
	writereg(CRTC_INDEX, 0x11, 0x00); /* unlock */
	for (i=0;i<Q_CRTCNT;i++)
		writereg(CRTC_INDEX,i,q_crtc[mode][i]);

	writereg(CRTC_INDEX, 0x42, q_over1[mode]);

	writereg(CRTC_INDEX, 0x51, q_over2[mode]);

	/* set border (overscan) colors */
	outportb(CO_COLOR_WRITE, 0x00);
	outportb(CO_COLOR_DATA, 0x0f);	/* red */
	outportb(CO_COLOR_DATA, 0x0f);  /* green */
	outportb(CO_COLOR_DATA, 0x04);  /* blue */

	/* attribure regs.. */
	inportb(0x3da);		/* reset latch */
	for (i=0;i<Q_ATTRCNT; i++)
		writereg(ATTR_INDEX, i, q_attr[mode][i]);
	/* graphics regs.. */
	for (i=0;i<Q_GRFXCNT;i++)
		writereg(GC_INDEX,i,q_grfx[mode][i]);
	/* load default palette */
	outportb(PALETTE_WRITE, 0x00);
	for (i=0;i<256;i++)
		{
		outportb(PALETTE_DATA, palette8[i][0]);
		outportb(PALETTE_DATA, palette8[i][1]);
		outportb(PALETTE_DATA, palette8[i][2]);
		}
	/* should clear the screen in here.. */

	/* video on.. */
	inportb(0x3da);
	outportb(ATTR_INDEX,0x20);

	writereg(0x3cf,0x5a, 0x04);
	/* remember the mode we're in.. */
	switch (mode)
	    {
	    case 0:
		v_rows = 480;
		v_cols = 640;
	      break;
	    case 1:
		v_rows = 768;
		v_cols = 1024;
	      break;
	    case 2:
		v_rows = 768;
		v_cols = 1024;
	      break;
	    }
	return;
} /* q_setmode */

void q_clearscreen()
{
 unsigned char bHoldReg;

 /* clear the screen in the graphics modes. */
 int x,y;
#ifdef CLRSCN
 bHoldReg = readreg(GC_INDEX,DATAPATH_CTRL);
 writereg (GC_INDEX,DATAPATH_CTRL,0x6);
 outportb(PREG_4,0x00);
 outportb(PREG_5,0x00);
 outportb(PREG_6,0x00);
 outportb(PREG_7,0x00);
 outportb(PREG_0,0x00);
 outportb(PREG_1,0x00);
 outportb(PREG_2,0x00);
 outportb(PREG_3,0x00);

 /* run the bitblt.. */
 outportw(DEST_ADDR_LO,0x00);
 outportw(DEST_ADDR_HI,0x00);
 outportw(BITMAP_WIDTH,1024);
 outportw(BITMAP_HEIGHT,1024);
 outportb(BLT_CMD_1, XY_SRC_ADDR|XY_DEST_ADDR);
 outportb(BLT_CMD_0, FORWARD|START_BLT);

 /* wait for done.. */
 while (inportb(CTRL_REG_1) & GLOBAL_BUSY_BIT)  ;

 /* reset the bitblt engine */
 outportb(GC_INDEX,0x10);
 outportb(GC_DATA, 0x48);
 outportb(GC_DATA,0x08);

 /* restore datapath.. */
 writereg(GC_INDEX,DATAPATH_CTRL, bHoldReg);
#endif /*CLRSCN*/
 /* we restored the datapath mode above, but here we set it to 
  * what we really want.. */
 if (v_rows == 0)
	{
	printf("\n Not in graphics mode? ");
	return;	/* not in a graphics mode?? */
	}
}

void
eisawriteb(addr,val)
unsigned int addr;
unsigned char val;
{
	unsigned int *ptr, dat;
	ptr =  (unsigned int *)( ((0x80000000)+(addr<<5))<<2);
	dat = val << (8*(addr&3));
	*ptr = dat;
	mb();

}

void
eisawritel(addr,val)
unsigned int addr;
unsigned int val;
{
	unsigned int *ptr;
	ptr = (unsigned int *)( ((0x80000000)+(addr<<5))<<2 | 0x60);
	*ptr = val; mb();
}

void
eisawritew(addr,val)
unsigned int addr;
unsigned short int val;
{
	unsigned int *ptr, dat;
	ptr = (unsigned int *)( ((0x80000000)+(addr<<5))<<2 | 0x20);
	dat = val << (8*(addr&3));
	*ptr = dat;
	mb();
}

void writexy(x,y,val)
int x;
int y;
unsigned char val;
{
	unsigned int addr;
	/* writes a pixel at (x,y)*/
	/* assumes we're in 32k map mode; we only use the first */
	/* mapping register.. */
	   writereg(GC_INDEX,0x45,((y*1024+x)>>12)); /* set the map */
	addr = 0xA0000 + (((y*1024)+x)&0xFFF);
	eisawriteb(addr, val);
} 

volatile unsigned int tom;

void snooze()
{
	int i;
	for (i=0;i<0xffffff;i++) tom++;
}

void q_putchar(cin)
unsigned char cin;
{
  /* put up the character.. */

  unsigned int i,j;
  unsigned char *cp;
	/* copy the rows from the font to the screen. */
	/* assumes 8bit/pixel packed mode.. */

	/* Is it a backspace? */
	if (cin == '\b')
		{
		if (q_curcol >= CHARWIDTH) q_curcol -= CHARWIDTH;
		return;
		}
	/* newline? */
	if (cin == '\n')
		{
		if (q_currow <= (v_rows-CHARHEIGHT*2))
			{
			/* easy, just munge pointers.. */
			q_currow += CHARHEIGHT;
			q_curcol = 0;
			return;
			}
		   else 
			{
			/* oog, gotta scroll.. */
			q_scroll();
			q_curcol = 0;
			return;
			}
		}
    if (use_blt==0)
	{
#ifdef NEWFONTS
	cp = &basefont8x16[CHARHEIGHT*cin];
#else
	cp = &vga8x16xx[0];	/* blank default */
	if (cin>=0x20 && cin<=0x7f)
		cp = &vga8x16gl[CHARHEIGHT*(cin-0x20)];
#endif
	for (i=0;i<CHARHEIGHT;i++)
		{
		for (j=0;j<CHARWIDTH;j++)
		    {
		    unsigned char color;
		    if (cp[i]&(1<<(CHARWIDTH-1-j))) 
			color=fg_color;
		      else
			color=bg_color;
		    writexy(q_curcol+j,q_currow+i,color);
		    }
		}
	}
      else
	{
	Blt_SS_Pk(
		((cin%64)*CHARWIDTH),		/* x addr */
		OFFSCREEN+(cin/64)*CHARHEIGHT,	/* y addr */
		q_curcol,			/* x dest */
		q_currow,			/* y dest */
		CHARWIDTH,
		CHARHEIGHT);
	}
	
	q_curcol += CHARWIDTH;
	/* check for scroll.. */
	if (q_curcol >= v_cols)
		{
		q_putchar('\n');
		}
}
BYTE bg[8] = {0,0,0,0,0,0,0,0};

void
q_setcolor(fg,bg)
unsigned int fg, bg;
{
    fg_color=fg;
    bg_color=bg;
    q_loadchars();
}
void q_loadchars()
{
/* load the font into off-screen video memory. 
 * the font is loaded as two rows of 64 characters each.
 */
    unsigned int cin, row,col, j,i,color;
    unsigned char *cp;
    row=OFFSCREEN;
    col=0;
    /* load up the first off-screen line with characters.. */
    for (cin=0;cin<128;cin++)
	{
#ifdef NEWFONTS
	cp = &basefont8x16[CHARHEIGHT*cin];
#else
	cp = &vga8x16xx[0];	/* blank default */
	if (cin>=0x20 && cin<=0x7f)
		cp = &vga8x16gl[CHARHEIGHT*(cin-0x20)];
#endif
	for (i=0;i<CHARHEIGHT;i++)
		{
		for (j=0;j<CHARWIDTH;j++)
		    {
		    if (cp[i]&(1<<(CHARWIDTH-1-j)))
			color=fg_color;
		      else
			color=bg_color;
		    writexy(col+j,row+i,color);
		    }
		}
	col += CHARWIDTH;
	if (cin == 63)
		{
		row += CHARHEIGHT;
		col=0;
		}
	}

}


void q_scroll()
{
	int i;
	/* load the background color.. */
	for (i=0;i<8;i++)
		bg[8]=i;

	/* scroll the screen up one character height, using the bitblt. */
	Blt_SS_Pk(0,CHARHEIGHT,0,0,1024,768-CHARHEIGHT);
	ClearLastLine();
	/* hang loose and see if it worked.. */
	for (i=0;i<0xFFFF;i++)
		mb();
	/* that probably botched the read/write modes, so.. */
	SetPackedRead();
	SetPackedWrite();
		  

}

/* from the compaq disk.. */
/*************************************************************************
 *
 *   All warranties including any warranties of merchantability and
 *   fitness for use are expressly disclaimed.  Use of this software
 *   constitutes acceptance of the foregoing.
 *
 *   Compaq Computer Corporation 1991
 *
 *************************************************************************
 *
 * Module : D_PATH.C - Datapath configuration functions.
 *
 * Version: V 1.00
 *
 * Date   : 9/20/91
 *
 * Author : KBV
 *
 *************************************************************************
 *
 * Change Log:
 *
 * Who  Code  Date      Revision  Description
 * ---  ----  --------  --------  -----------
 * KBV        09/20/91  V 1.00    Original
 *
 *
 *
 *
 *
 *************************************************************************
 *
 * Functional Description
 * ----------------------
 *
 *   SetPackedRead() - configures datapath for packed pixel reads
 *   SetPackedWrite() - configures datapath for packed pixel writes
 *   SetPlanarRead() - configures datapath for planar reads
 *   SetPlanarWrite() - configures datapath for planar writes
 *   Set_FB_CE_Write() - foreground/background color-expand writes
 *   Set_FT_CE_Write() - foreground/transparent color-expand writes
 *   Set_FO_CE_Write() - foreground/opaque color-expand writes
 *   SetBitBLT() - configures the datapath for the BitBLT engine
 *   SetLine() - configures the datapath for the Line Draw engine
 *
 *************************************************************************/

/*************************************************************************
 *
 * DATA TYPE PREFIX (Hungarian notation)
 *
 *   f  : BOOL (flag)
 *   b  : BYTE (unsigned 8 bit)
 *   us : USHORT (unsigned 16 bit)
 *   ul : ULONG (unsigned 32 bit)
 *   x  : x coordinate
 *   y  : y coordinate
 *   p  : pointer
 *   a  : array (ab = array of bytes, aus = ... )
 *   i  : index to array (ib, ius, ... )
 *   c  : count (cb, cus, ... )
 *
 *************************************************************************/

/*************************************************************************
 * CONSTANT DEFINITIONS
 *************************************************************************/

/*************************************************************************
 * EXTERNAL REFERENCES
 *************************************************************************/

/*************************************************************************
 * GLOBAL VARIABLES
 *************************************************************************/

/*************************************************************************
 * SetPackedRead()
 *
 * DESCRIPTION:
 *   This function configures the datapath for packed pixel reads.
 *
 * INPUT:
 *   none
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT SetPackedRead( VOID )
{

   /* Wait for idle hardware.  Only useful if using BitBLT or Line *
    * Draw Engine before calling this function.                    *
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT); */

   /* Set datapath.  Preserve the number of bits per pixel. */
   outp( CTRL_REG_1, PACKED_PIXEL_VIEW |
                     (inp( CTRL_REG_1) & 0x07));

}  /* SetPackedRead() */


/*************************************************************************
 * SetPackedWrite()
 *
 * DESCRIPTION:
 *   This function configures the datapath for packed pixel writes
 *   in the simplest case of no rotate, no ROPs, and no masking. 
 *
 * INPUT:
 *   none
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT SetPackedWrite( VOID)
{

   /* Wait for idle hardware.  Only useful if using BitBLT or Line *
    * Draw Engine before calling this function.                    *
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT); */

   /* Set datapath.  Preserve the number of bits per pixel. */
   outp( CTRL_REG_1, PACKED_PIXEL_VIEW |
                     (inp( CTRL_REG_1) & 0x07));
   outp( GC_INDEX, DATAPATH_CTRL);
   outp( GC_DATA, ROPSELECT_NO_ROPS |
                  PIXELMASK_ONLY |
                  PLANARMASK_NONE_0XFF |
                  SRC_IS_CPU_DATA        );
   outp( GC_INDEX, DATA_ROTATE);
   outp( GC_DATA, NO_ROTATE);
   outp( SEQ_INDEX, SEQ_PIXEL_WR_MSK);
   outp( SEQ_DATA, NO_MASK);

}  /* SetPackedWrite() */


/*************************************************************************
 * SetPlanarRead()
 *
 * DESCRIPTION:
 *   This function configures the datapath for planar reads from a 
 *   given bit plane.
 *
 * INPUT:
 *   bBitPlane:  bit plane to read (0..7)
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT SetPlanarRead( BYTE bBitPlane )
{

   /* Wait for idle hardware.  Only useful if using BitBLT or Line *
    * Draw Engine before calling this function.                    *
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT); */

   /* Set datapath.  Preserve the number of bits per pixel. */
   outp( CTRL_REG_1, PLANAR_VIEW |
                     (inp( CTRL_REG_1) & 0x07));
   outp( GC_INDEX, READ_CTRL);
   outp( GC_DATA, bBitPlane);

}  /* SetPlanarRead() */


/*************************************************************************
 * SetPlanarWrite()
 *
 * DESCRIPTION:
 *   This function configures the datapath for simple planar writes 
 *   (no color-expand) in the simplest case of no rotate, no ROPs, and no
 *   pixel masking.  A mask is passed in for the Color Plane Write Mask 
 *   to determine which bit planes are written and which are left as they 
 *   are.
 *
 * INPUT:
 *   bBitPlaneMask:  1 in mask writes that bit plane, 0 leaves as is
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT SetPlanarWrite( BYTE bBitPlaneMask )
{

   /* Wait for idle hardware.  Only useful if using BitBLT or Line *
    * Draw Engine before calling this function.                    *
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT); */

   /* Set datapath.  Preserve the number of bits per pixel. */
   outp( CTRL_REG_1, PLANAR_VIEW |
                     (inp( CTRL_REG_1) & 0x07));
   outp( GC_INDEX, DATAPATH_CTRL);
   outp( GC_DATA, ROPSELECT_NO_ROPS |
                  PIXELMASK_ONLY |
                  PLANARMASK_ONLY |
                  SRC_IS_CPU_DATA     );
   outp( GC_INDEX, DATA_ROTATE);
   outp( GC_DATA, NO_ROTATE);
   outp( SEQ_INDEX, SEQ_PIXEL_WR_MSK);
   outp( SEQ_DATA, NO_MASK);
   outp( GC_INDEX, GC_PLANE_WR_MSK);
   outp( GC_DATA, bBitPlaneMask);

}  /* SetPlanarWrite() */


/*************************************************************************
 * Set_FB_CE_Write()
 *
 * DESCRIPTION:
 *   This function configures the datapath for a foreground/background 
 *   color-expand write in the simplest case of no rotate, no ROPs, and
 *   no masking.  This mode expands each bit of source data into either
 *   a foreground or background color pixel.  1's are expanded to 
 *   foreground color pixels and 0's are expanded to background color 
 *   pixels.
 *
 * INPUT:
 *   bForegroundColor:  4- or 8-bit color for Foreground Color Reg.
 *   bBackgroundColor:  4- or 8-bit color for Background Color Reg.
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT Set_FB_CE_Write( BYTE bForegroundColor,
                        BYTE bBackgroundColor  )
{

   /* Wait for idle hardware.  Only useful if using BitBLT or Line *
    * Draw Engine before calling this function.                    *
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT); */

   /* Set datapath.  Preserve the number of bits per pixel. */
   outp( CTRL_REG_1, EXPAND_TO_FG |
                     (inp( CTRL_REG_1) & 0x07));
   outp( GC_INDEX, DATAPATH_CTRL);
   outp( GC_DATA, ROPSELECT_NO_ROPS |
                  PIXELMASK_ONLY |
                  PLANARMASK_NONE_0XFF |
                  SRC_IS_CPU_DATA        );
   outp( GC_INDEX, DATA_ROTATE);
   outp( GC_DATA, NO_ROTATE);
   outp( SEQ_INDEX, SEQ_PIXEL_WR_MSK);
   outp( SEQ_DATA, NO_MASK);
   outp( GC_INDEX, GC_FG_COLOR);
   outp( GC_DATA, bForegroundColor);
   outp( GC_INDEX, GC_BG_COLOR);
   outp( GC_DATA, bBackgroundColor);

}  /* Set_FB_CE_Write() */


/*************************************************************************
 * Set_FT_CE_Write()
 *
 * DESCRIPTION:
 *   This function configures the datapath for a foreground/transparent 
 *   color-expand write in the simplest case of no rotate, no ROPs, no
 *   planar masking, and no additional pixel masking.  This mode expands
 *   each bit of the source data in either a foreground color pixel or
 *   or a pixel from the screen read latches.  1's are expanded to 
 *   foreground color pixels and 0's are expanded to pixels from the 
 *   screen read latches thus preserving those pixels.
 *
 * INPUT:
 *   bForegroundColor:  4- or 8-bit color for Foreground Color Reg.
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT Set_FT_CE_Write( BYTE bForegroundColor )
{

   /* Wait for idle hardware.  Only useful if using BitBLT or Line *
    * Draw Engine before calling this function.                    *
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT); */

   /* Set datapath.  Preserve the number of bits per pixel. */
   outp( CTRL_REG_1, EXPAND_TO_FG |
                     (inp( CTRL_REG_1) & 0x07));
   outp( GC_INDEX, DATAPATH_CTRL);
   outp( GC_DATA, ROPSELECT_NO_ROPS |
                  PIXELMASK_AND_CPU_DATA |
                  PLANARMASK_NONE_0XFF |
                  SRC_IS_CPU_DATA          );
   outp( GC_INDEX, DATA_ROTATE);
   outp( GC_DATA, NO_ROTATE);
   outp( SEQ_INDEX, SEQ_PIXEL_WR_MSK);
   outp( SEQ_DATA, NO_MASK);
   outp( GC_INDEX, GC_FG_COLOR);
   outp( GC_DATA, bForegroundColor);

}  /* Set_FT_CE_Write() */


/*************************************************************************
 * Set_FO_CE_Write()
 *
 * DESCRIPTION:
 *   This function configures the datapath for a foreground/opaque color-
 *   expand write in the case of no rotate and no masking.  This mode 
 *   expands each bit of the source data into foreground or background
 *   color pixels just as in Set_FB_CE_Write but ROPs in this mode are 
 *   applied only to foreground (primary) color pixels.  1's are expanded 
 *   into foreground color pixel and 0's are expanded into background color
 *   pixels.  
 *
 * INPUT:
 *   bForegroundColor:  8-bit color for Foreground Color Reg.
 *   bBackgroundColor:  8-bit color for Background Color Reg.
 *   bRasterOperation:  value (0..15) specifying 1 of the 16 ROPs
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT Set_FO_CE_Write( BYTE bForegroundColor,
                        BYTE bBackgroundColor,
                        BYTE bRasterOperation  )
{

   /* Wait for idle hardware.  Only useful if using BitBLT or Line *
    * Draw Engine before calling this function.                    *
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT); */

   /* Set datapath.  Preserve the number of bits per pixel. */
   outp( CTRL_REG_1, EXPAND_TO_FG |
                     (inp( CTRL_REG_1) & 0x07));
   outp( GC_INDEX, DATAPATH_CTRL);
   outp( GC_DATA, ROPSELECT_PRIMARY_ONLY |
                  PIXELMASK_ONLY |
                  PLANARMASK_NONE_0XFF |
                  SRC_IS_CPU_DATA          );
   outp( GC_INDEX, DATA_ROTATE);
   outp( GC_DATA, NO_ROTATE);
   outp( SEQ_INDEX, SEQ_PIXEL_WR_MSK);
   outp( SEQ_DATA, NO_MASK);
   outp( GC_INDEX, GC_FG_COLOR);
   outp( GC_DATA, bForegroundColor);
   outp( GC_INDEX, GC_BG_COLOR);
   outp( GC_DATA, bBackgroundColor);
   outp( ROP_A, bRasterOperation);

}  /* Set_FO_CE_Write() */


/*************************************************************************
 * SetBitBLT()
 *
 * DESCRIPTION:
 *   This function configures the datapath for any of the three types of
 *   BitBLT operations (screen-to-screen, cpu-to-screen, or pattern-to-
 *   screen) based on the data source specified.  Any packed-pixel or 
 *   planar views may also be specified.  The simple case of no ROPs and 
 *   no masking is assumed.
 *
 * INPUT:
 *   bDataSource:  selects type of BLT, should be one of the following:
 *                   SRC_IS_SCRN_LATHCES -> screen-to-screen BitBLTs
 *                   SRC_IS_CPU_DATA     -> cpu-to-screen BitBLTs
 *                   SRC_IS_PATTERN_REGS -> pattern-to-screen BitBLTs
 *   bExpandCtrl:  selects packed pixel, planar, or color-expand modes:
 *                   PACKED_PIXEL_VIEW
 *                   PLANAR_VIEW
 *                   EXPAND_TO_FG
 *                   EXPAND_TO_BG
 *   bForegroundColor:  foreground color used if color-expand selected
 *   bBackgroundColor:  background color used if color-expand selected
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT SetBitBLT( BYTE bDataSource,
                  BYTE bExpandCtrl,
                  BYTE bForegroundColor,
                  BYTE bBackgroundColor  )
{

   /* Assume waiting for idle hardware is handled elsewhere before *
    * this function is called.                                     */

   /* Set datapath.  Preserve the number of bits per pixel. */
   outp( CTRL_REG_1, bExpandCtrl |
                     (inp( CTRL_REG_1) & 0x07));
   outp( GC_INDEX, DATAPATH_CTRL);
   outp( GC_DATA, ROPSELECT_NO_ROPS |
                  PIXELMASK_ONLY |
                  PLANARMASK_NONE_0XFF |
                  bDataSource            );
   outp( SEQ_INDEX, SEQ_PIXEL_WR_MSK);
   outp( SEQ_DATA, NO_MASK);
   outp( GC_INDEX, GC_FG_COLOR);
   outp( GC_DATA, bForegroundColor);
   outp( GC_INDEX, GC_BG_COLOR);
   outp( GC_DATA, bBackgroundColor);

}  /* SetBitBLT() */


/*************************************************************************
 * SetLine()
 *
 * DESCRIPTION:
 *   This function configures the datapath for the Line Draw Engine in the
 *   simple case of no ROPs and no masking.
 *
 * INPUT:
 *   bLineColor:         color of the line to be drawn (foreground color)
 *   bBackgroundColor:   color of any gaps in the line pattern
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT SetLine( BYTE bLineColor,
                BYTE bBackgroundColor )
{

   /* Assume waiting for idle hardware is handled elsewhere before *
    * this function is called.                                     */

   /* Set datapath.  Preserve the number of bits per pixel. */
   outp( CTRL_REG_1, EXPAND_TO_FG |
                     (inp( CTRL_REG_1) & 0x07));
   outp( GC_INDEX, DATAPATH_CTRL);
   outp( GC_DATA, ROPSELECT_NO_ROPS |
                  PIXELMASK_ONLY |
                  PLANARMASK_NONE_0XFF |
                  SRC_IS_LINE_PATTERN    );
   outp( GC_INDEX, DATA_ROTATE);
   outp( GC_DATA, NO_ROTATE);
   outp( SEQ_INDEX, SEQ_PIXEL_WR_MSK);
   outp( SEQ_DATA, NO_MASK);
   outp( GC_INDEX, GC_FG_COLOR);
   outp( GC_DATA, bLineColor);
   outp( GC_INDEX, GC_BG_COLOR);
   outp( GC_DATA, bBackgroundColor);

}  /* SetLine() */

/*************************************************************************
 *
 *   All warranties including any warranties of merchantability and
 *   fitness for use are expressly disclaimed.  Use of this software
 *   constitutes acceptance of the foregoing.
 *
 *   Compaq Computer Corporation 1991
 *
 *************************************************************************
 *
 * Module : BLT_4_8.C - 4 and 8 bits per pixel BitBLT functions.
 *
 * Version: V 1.00
 *
 * Date   : 9/20/91
 *
 * Author : KBV
 *
 *************************************************************************
 *
 * Change Log:
 *
 * Who  Code  Date      Revision  Description
 * ---  ----  --------  --------  -----------
 * KBV        09/20/91  V 1.00    Original
 *
 *
 *
 *
 *
 *************************************************************************
 *
 * Functional Description
 * ----------------------
 *
 *   Blt_SS_Pk() - performs a packed pixel screen-to-screen BitBLT
 *   Blt_SS_CE_lin() - performs a color-expand screen-to-screen BitBLT
 *   Blt_CS_CE() - performs a color-expand cpu-to-screen BitBLT
 *   Blt_PS_Pk() - performs a packed pixel pattern-to-screen BitBLT
 *   Blt_PS_CE() - performs a color-expand pattern-to-screen BitBLT
 *   Blt_Nx8_PS_Pk() - Nx8 packed pixel pattern-to-screen BitBLT
 *   Blt_NxM_PS_Pk() - NxM packed pixel pattern-to-screen BitBLT
 *   Blt_Buff_SS_Pk() - multiple packed pixel screen-to-screen BitBLTs
 *
 *************************************************************************/

/*************************************************************************
 *
 * DATA TYPE PREFIX (Hungarian notation)
 *
 *   f  : BOOL (flag)
 *   b  : BYTE (unsigned 8 bit)
 *   us : USHORT (unsigned 16 bit)
 *   ul : ULONG (unsigned 32 bit)
 *   x  : x coordinate
 *   y  : y coordinate
 *   p  : pointer
 *   a  : array (ab = array of bytes, aus = ... )
 *   i  : index to array (ib, ius, ... )
 *   c  : count (cb, cus, ... )
 *
 *************************************************************************/


/*************************************************************************
 * CONSTANT DEFINITIONS
 *************************************************************************/

/*************************************************************************
 * EXTERNAL REFERENCES
 *************************************************************************/
extern USHORT SetBitBLT( BYTE, BYTE, BYTE, BYTE);

/*************************************************************************
 * GLOBAL VARIABLES
 *************************************************************************/

/************************************************************************* 
 * Blt_SS_Pk()
 *
 * DESCRIPTION:
 *   This function performs a single packed pixel screen-to-screen 
 *   BitBLT operation using (x,y) coordinate addressing for both the
 *   source and destination bitmaps.
 * 
 * INPUT:
 *   xSrc:      x coordinate of the source bitmap (0..1023)
 *   ySrc:      y coordinate of the source bitmap (0..1023)
 *   xDest:     x coordinate of the destination bitmap (-1024..1023)
 *   yDest:     y coordinate of the destination bitmap (-1024..1023)
 *   usWidth:   width of the source and destination bitmaps (0..1023)
 *   usHeight:  height of the source and destination bitmaps (0..1023)
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT Blt_SS_Pk( USHORT xSrc,
                  USHORT ySrc,
                  USHORT xDest,
                  USHORT yDest,
                  USHORT usWidth,
                  USHORT usHeight )
{
   BOOL fOverlap;

   /* if overlap is TRUE we must flip everything and BLT backwards. */
   if (fOverlap = (yDest > ySrc) || ((yDest == ySrc) && (xDest <= xSrc)))
      {
      xDest += usWidth - 1;
      yDest += usHeight - 1;
      xSrc += usWidth - 1;
      ySrc += usHeight - 1;
      }

   /* wait for idle hardware */
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT);

   /* Set the datapath.  Note that it is not necessary to do this *
    * for subsequent BitBLT operations of this type provided that *
    * the datapath configuration has not been altered.            */
   SetBitBLT( SRC_IS_SCRN_LATCHES, PACKED_PIXEL_VIEW, 0, 0);

   /* set BitBLT hardware registers and start engine */
   outpw( X0_SRC_ADDR_LO, xSrc);
   outpw( Y0_SRC_ADDR_HI, ySrc);
   outpw( DEST_ADDR_LO, xDest);
   outpw( DEST_ADDR_HI, yDest);
   outpw( BITMAP_WIDTH, usWidth);
   outpw( BITMAP_HEIGHT, usHeight);
   outp( BLT_CMD_1, XY_SRC_ADDR | 
                    XY_DEST_ADDR  ); 
   if (fOverlap)
     outp( BLT_CMD_0, BACKWARD | 
                      NO_BYTE_SWAP | 
                      WRAP |
                      START_BLT      );
   else
     outp( BLT_CMD_0, FORWARD | 
                      NO_BYTE_SWAP | 
                      WRAP |
                      START_BLT      );

}  /* Blt_SS_Pk() */


/************************************************************************* 
 * Blt_SS_CE_lin()
 *
 * DESCRIPTION:
 *   This function performs a single color-expand screen-to-screen 
 *   BitBLT operation using linear addressing for the source bitmap 
 *   and (x,y) coordinate addressing for the destination bitmap.
 * 
 * INPUT:
 *   xSrcAddr:     linear address of the source bitmap (0..8388607)
 *   ySrcAddr:     pitch in dwords of the source bitmap (0..65535)
 *   xDest:        x coordinate of the destination bitmap (-1024..1023)
 *   yDest:        y coordinate of the destination bitmap (-1024..1023)
 *   usWidth:      width of the destination bitmap (0..2047)
 *   usHeight:     height of the destination bitmap (0..1023)
 *   bFgndColor:   4- or 8-bit foreground color
 *   bBgndColor:   4- or 8-bit background color
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT Blt_SS_CE_lin( ULONG xSrcAddr,
                  ULONG ySrcAddr,
                  USHORT xDest,
                  USHORT yDest,
                  USHORT usWidth,
                  USHORT usHeight,
                  BYTE bFgndColor,
                  BYTE bBgndColor     )
{
   /* assume no overlap in this BitBLT */
printf("Blt_SS_CE_lin: x:%d, y=%d, xd=%d, yd=%d, w=%d,h=%d, bg=%d,fg=%d\n",
	xSrcAddr, ySrcAddr,xDest, yDest, usWidth, usHeight,
	bFgndColor,bBgndColor);
   /* wait for idle hardware */
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT);

   /* Set the datapath.  Note that it is not necessary to do this *
    * for subsequent BitBLT operations of this type provided that *
    * the datapath configuration has not been altered.            */
   SetBitBLT( SRC_IS_SCRN_LATCHES, EXPAND_TO_FG, bFgndColor, bBgndColor);

   /* set BitBLT hardware registers and start engine */
   outpw( X0_SRC_ADDR_LO, xSrcAddr );
   outpw( Y0_SRC_ADDR_HI, ySrcAddr);
   outpw( DEST_ADDR_LO, xDest);
   outpw( DEST_ADDR_HI, yDest);
   outpw( BITMAP_WIDTH, usWidth);
   outpw( BITMAP_HEIGHT, usHeight);
   outp( BLT_CMD_1, LIN_SRC_ADDR |
                    XY_DEST_ADDR |
		    XY_SRC_ADDR  );
   outp( BLT_CMD_0, FORWARD |
                    NO_BYTE_SWAP |
                    WRAP |
                    START_BLT      );

}  /* Blt_SS_CE_lin() */



/*************************************************************************
 * Blt_PS_Pk()
 *
 * DESCRIPTION:
 *   This function performs a packed pixel pattern-to-screen BitBLT 
 *   operation using (x,y) destination addressing.  The source address
 *   is 0 which causes the pattern to start in the first PReg (PReg4).
 *
 * INPUT:
 *   xDest:        x coordinate of the destination bitmap (-1024..1023)
 *   yDest:        y coordinate of the destination bitmap (-1024..1023)
 *   usWidth:      width of the destination bitmap (0..2047)
 *   usHeight:     height of the destination bitmap (0..1023)
 *   abPattern[]:  array of 8 bytes for pattern registers
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT Blt_PS_Pk( USHORT xDest,
                  USHORT yDest,
                  USHORT usWidth,
                  USHORT usHeight,
                  BYTE *abPattern   )
{
   /* wait for idle hardware */
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT);

   /* Set the datapath.  Note that it is not necessary to do this *
    * for subsequent BitBLT operations of this type provided that *
    * the datapath configuration has not been altered.            */
   SetBitBLT( SRC_IS_PATTERN_REGS, PACKED_PIXEL_VIEW, 0, 0);
   /* Set pattern registers.  Note that it is not necessary to do *
    * this for subsequent pattern-to-screen BitBLT operations if  *
    * the pattern does not change.                                */
   outp( PREG_4, abPattern[0]);
   outp( PREG_5, abPattern[1]);
   outp( PREG_6, abPattern[2]);
   outp( PREG_7, abPattern[3]);
   outp( PREG_0, abPattern[4]);
   outp( PREG_1, abPattern[5]);
   outp( PREG_2, abPattern[6]);
   outp( PREG_3, abPattern[7]);

   /* set BitBLT hardware registers and start engine */
   outpw( X0_SRC_ADDR_LO, 0);               /* pattern starts in PReg4 */
   outpw( SRC_PITCH, MAX_SCANLINE_DWORDS);
   outpw( DEST_ADDR_LO, xDest);
   outpw( DEST_ADDR_HI, yDest);
   outpw( BITMAP_WIDTH, usWidth);
   outpw( BITMAP_HEIGHT, usHeight);
   outp( BLT_CMD_1, XY_SRC_ADDR |
                    XY_DEST_ADDR   );
   outp( BLT_CMD_0, FORWARD |
                    NO_BYTE_SWAP |
                    WRAP |
                    START_BLT      );

}  /* Blt_PS_Pk() */


/*************************************************************************
 * Blt_PS_CE()
 *
 * DESCRIPTION:
 *   This function performs a color-expand pattern-to-screen BitBLT
 *   operation using (x,y) destination addressing.  The source address
 *   is 0 which causes the pattern to start with bit 7 of PReg4.
 *
 * INPUT:
 *   xDest:        x coordinate of the destination bitmap (-1024..1023)
 *   yDest:        y coordinate of the destination bitmap (-1024..1023)
 *   usWidth:      width of the destination bitmap (0..2047)
 *   usHeight:     height of the destination bitmap (0..1023)
 *   bFgndColor:   4- or 8-bit foreground color
 *   bBgndColor:   4- or 8-bit background color
 *   abPattern[]:  array of 8 bytes for pattern registers
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *************************************************************************/

USHORT Blt_PS_CE( USHORT xDest,
                  USHORT yDest,
                  USHORT usWidth,
                  USHORT usHeight,
                  BYTE bFgndColor,
                  BYTE bBgndColor,
                  BYTE *abPattern  )
{
   /* wait for idle hardware */
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT);

   /* Set the datapath.  Note that it is not necessary to do this *
    * for subsequent BitBLT operations of this type provided that *
    * the datapath configuration has not been altered.            */
   SetBitBLT( SRC_IS_PATTERN_REGS, EXPAND_TO_FG, bFgndColor, bBgndColor);

   /* Set pattern registers.  Note that it is not necessary to do *
    * this for subsequent pattern-to-screen BitBLT operations if  *
    * the pattern does not change.                                */
   outp( PREG_4, abPattern[0]);
   outp( PREG_5, abPattern[1]);
   outp( PREG_6, abPattern[2]);
   outp( PREG_7, abPattern[3]);
   outp( PREG_0, abPattern[4]);
   outp( PREG_1, abPattern[5]);
   outp( PREG_2, abPattern[6]);
   outp( PREG_3, abPattern[7]);

   /* set BitBLT hardware registers and start engine */
   outpw( X0_SRC_ADDR_LO, 0);               /* pattern starts in PReg4 */
   outpw( SRC_PITCH, MAX_SCANLINE_DWORDS);
   outpw( DEST_ADDR_LO, xDest);
   outpw( DEST_ADDR_HI, yDest);
   outpw( BITMAP_WIDTH, usWidth);
   outpw( BITMAP_HEIGHT, usHeight);
   outp( BLT_CMD_1, LIN_SRC_ADDR |
                    XY_DEST_ADDR   );
   outp( BLT_CMD_0, FORWARD |
                    NO_BYTE_SWAP |
                    WRAP |
                    START_BLT      );

}  /* Blt_PS_CE() */


/*************************************************************************
 * Blt_Nx8_PS_Pk()
 *
 * DESCRIPTION:
 *   This function performs several simple 8 bpp packed pixel pattern-to-
 *   screen BitBLT operations to transfer an Nx8 packed pixel pattern to 
 *   the screen.  Linear destination addressing must be used.  The source
 *   address is 0 which causes the pattern to start in PReg4.
 *
 * INPUT:
 *   ulDestLinAddr:  linear address of the destination bitmap (0..8388607)
 *   usWidth:        width of the destination bitmap (0..1023)
 *   usHeight:       height of the destination bitmap (0..1023)
 *   usN:            height of the Nx8 pattern
 *   abNx8Pattern[]: array of (N * 8) bytes for the pattern registers
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT Blt_Nx8_PS_Pk( ULONG ulDestLinAddr,
                      USHORT usWidth,
                      USHORT usHeight,
                      USHORT usN,
                      BYTE *abNx8Pattern   )
{
   USHORT cusPatternLine;

   /* wait for idle hardware */
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT);

   /* Set the datapath.  Note that it is not necessary to do this *
    * for subsequent BitBLT operations of this type provided that *
    * the datapath configuration has not been altered.            */
   SetBitBLT( SRC_IS_PATTERN_REGS, PACKED_PIXEL_VIEW, 0, 0);

   /* set BitBLT hardware registers */
   outpw( X0_SRC_ADDR_LO, 0);
   outpw( SRC_PITCH, MAX_SCANLINE_DWORDS);
   outpw( DEST_PITCH, usN * MAX_SCANLINE_DWORDS);  /* set (pitch * n) */ 
   outpw( BITMAP_WIDTH, usWidth);
   outpw( BITMAP_HEIGHT, usHeight / usN);          /* set (height / n) */
   outp( BLT_CMD_1, LIN_SRC_ADDR |
                    LIN_DEST_ADDR  );

   for (cusPatternLine = 0; cusPatternLine < usN; cusPatternLine++)
      {
      /* wait for idle hardware */
      while (inp( BLT_CMD_0) & SS_BIT);

      /* load line of pattern */
      outp( PREG_4, abNx8Pattern[cusPatternLine * 8 + 0]);
      outp( PREG_5, abNx8Pattern[cusPatternLine * 8 + 1]);
      outp( PREG_6, abNx8Pattern[cusPatternLine * 8 + 2]);
      outp( PREG_7, abNx8Pattern[cusPatternLine * 8 + 3]);
      outp( PREG_0, abNx8Pattern[cusPatternLine * 8 + 4]);
      outp( PREG_1, abNx8Pattern[cusPatternLine * 8 + 5]);
      outp( PREG_2, abNx8Pattern[cusPatternLine * 8 + 6]);
      outp( PREG_3, abNx8Pattern[cusPatternLine * 8 + 7]);

      /* set destination address */
      outpw( DEST_ADDR_LO, ulDestLinAddr & 0xFFFF);
      outpw( DEST_ADDR_HI, ulDestLinAddr >> 16);

      /* start the BitBLT Engine */
      outp( BLT_CMD_0, FORWARD |
                       NO_BYTE_SWAP |
                       WRAP |
                       START_BLT      );

      /* increment destination address for next line of pattern */
      ulDestLinAddr += MAX_SCANLINE_DWORDS * 32;
      }

}  /* Blt_Nx8_PS_Pk() */


/*************************************************************************
 * Blt_NxM_PS_Pk()
 *
 * DESCRIPTION:
 *   This function performs several simple 8 bpp packed pixel pattern-to-
 *   screen BitBLT operations to transfer an NxM packed pixel pattern to 
 *   the screen.  To simplify the process, the bitmap width is assumed to 
 *   be a multiple of M and the bitmap height is assumed to be a multiple 
 *   of N.  Linear destination addressing must be used.  The source address
 *   is set to 0.
 *
 * INPUT:
 *   ulDestLinAddr:  linear address of the destination bitmap (0..8388607)
 *   usWidth:        width of the destination bitmap (0..1023)
 *   usHeight:       height of the destination bitmap (0..1023)
 *   usN:            height of the NxM pattern
 *   usM:            width of the NxM pattern
 *   abNxMPattern[]: array of (N * M) bytes for the pattern registers
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 *
 *************************************************************************/

USHORT Blt_NxM_PS_Pk( ULONG ulDestLinAddr,
                      USHORT usWidth,
                      USHORT usHeight,
                      USHORT usN,
                      USHORT usM,
                      BYTE *abNxMPattern   )
{
   USHORT cusPatternLine;
   USHORT cusLineSeg;
   USHORT cusSegColumn;

   /* wait for idle hardware */
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT);

   /* Set the datapath.  Note that it is not necessary to do this *
    * for subsequent BitBLT operations of this type provided that *
    * the datapath configuration has not been altered.            */
   SetBitBLT( SRC_IS_PATTERN_REGS, PACKED_PIXEL_VIEW, 0, 0);

   /* set BitBLT hardware registers */
   outpw( X0_SRC_ADDR_LO, 0);
   outpw( SRC_PITCH, MAX_SCANLINE_DWORDS);
   outpw( DEST_PITCH, usN * MAX_SCANLINE_DWORDS);  /* set (pitch * n) */ 
   outpw( BITMAP_HEIGHT, usHeight / usN);          /* set (height / n) */
   outp( BLT_CMD_1, LIN_SRC_ADDR |
                    LIN_DEST_ADDR  );

   /* transfer pattern to screen */
   for (cusPatternLine = 0; cusPatternLine < usN; cusPatternLine++)
      {
      for (cusLineSeg = 0; cusLineSeg < (usM - 1) / 8 + 1; cusLineSeg++)
         {
         /* set current segment width */
         if (usM - (cusLineSeg * 8) >= 8)
            outpw( BITMAP_WIDTH, 8);
         else
            outpw( BITMAP_WIDTH, usM - (cusLineSeg * 8));

         /* load line of pattern */
         outp( PREG_4, abNxMPattern[usM*cusPatternLine + cusLineSeg*8 + 0]);
         outp( PREG_5, abNxMPattern[usM*cusPatternLine + cusLineSeg*8 + 1]);
         outp( PREG_6, abNxMPattern[usM*cusPatternLine + cusLineSeg*8 + 2]);
         outp( PREG_7, abNxMPattern[usM*cusPatternLine + cusLineSeg*8 + 3]);
         outp( PREG_0, abNxMPattern[usM*cusPatternLine + cusLineSeg*8 + 4]);
         outp( PREG_1, abNxMPattern[usM*cusPatternLine + cusLineSeg*8 + 5]);
         outp( PREG_2, abNxMPattern[usM*cusPatternLine + cusLineSeg*8 + 6]);
         outp( PREG_3, abNxMPattern[usM*cusPatternLine + cusLineSeg*8 + 7]);

         for (cusSegColumn = 0; 
              cusSegColumn < (usWidth / usM); 
              cusSegColumn++                  )
            {
            /* set destination address */
            outpw( DEST_ADDR_LO, (ulDestLinAddr + 8 * (cusLineSeg * 8
                                         + cusSegColumn * usM)) & 0xFFFF);
            outpw( DEST_ADDR_HI, (ulDestLinAddr + 8 * (cusLineSeg * 8
                                         + cusSegColumn * usM)) >> 16);

            /* start the BitBLT Engine */
            outp( BLT_CMD_0, FORWARD |
                             NO_BYTE_SWAP |
                             WRAP |
                             START_BLT      );

            /* wait for idle hardware before starting next BitBLT */
            while (inp( BLT_CMD_0) & SS_BIT);
            }
         }

      /* increment destination address for next line of pattern */
      ulDestLinAddr += MAX_SCANLINE_DWORDS * 32;
      }

}  /* Blt_NxM_PS_Pk() */


/*************************************************************************
 * Blt_Buff_SS_Pk()
 *
 * DESCRIPTION:
 *   This function performs multiple packed pixel screen-to-screen 
 *   BitBLT operations using buffering.  (x,y) format is used for both 
 *   source and destination addressing.
 *
 * INPUT:
 *   usNumOfBLTs:  the number of BitBLT operations to be performed
 *   axSrc[]:      array of source bitmap x coordinates (0..1023)
 *   aySrc[]:      array of source bitmap y coordinates (0..1023)
 *   axDest[]:     array of destination bitmap x coordinates (-1024..1023)
 *   ayDest[]:     array of destination bitmap y coordinates (-1024..1023)
 *   ausWidth[]:   array of bitmap widths (0..1023)
 *   ausHeight[]:  array of bitmap heights (0..1023)
 *
 * OUTPUT:
 *   none
 *
 * RETURN:
 *   none
 * 
 *************************************************************************/

USHORT Blt_Buff_SS_Pk( USHORT usNumOfBLTs,
                       USHORT *axSrc,
                       USHORT *aySrc,
                       USHORT *axDest,
                       USHORT *ayDest,
                       USHORT *ausWidth,
                       USHORT *ausHeight    )
{
   USHORT cusBLTCnt;
   BOOL fOverlap;

   /* wait for idle hardware */
   while (inp( CTRL_REG_1) & GLOBAL_BUSY_BIT);

   /* Set the datapath.  Note that it is not necessary to do this *
    * for subsequent BitBLT operations of this type provided that *
    * the datapath configuration has not been altered.            */
   SetBitBLT( SRC_IS_SCRN_LATCHES, PACKED_PIXEL_VIEW, 0, 0);

   for (cusBLTCnt = 0; cusBLTCnt < usNumOfBLTs; cusBLTCnt++)
      {
      /* if overlap is TRUE we must flip everything and BLT backwards. */
      if (fOverlap = (ayDest[cusBLTCnt] > aySrc[cusBLTCnt]) || 
                        ((ayDest[cusBLTCnt] == aySrc[cusBLTCnt]) && 
                           (axDest[cusBLTCnt] <= axSrc[cusBLTCnt])))
         {
         axDest[cusBLTCnt] += ausWidth[cusBLTCnt] - 1;
         ayDest[cusBLTCnt] += ausHeight[cusBLTCnt] - 1;
         axSrc[cusBLTCnt] += ausWidth[cusBLTCnt] - 1;
         aySrc[cusBLTCnt] += ausHeight[cusBLTCnt] - 1;
         }

      /* wait until buffer not busy */
      while (inp( CTRL_REG_1) & BUFFER_BUSY_BIT);

      /* set buffered BitBLT hardware registers and start engine */
      outpw( X0_SRC_ADDR_LO, axSrc[cusBLTCnt]);
      outpw( Y0_SRC_ADDR_HI, aySrc[cusBLTCnt]);
      outpw( DEST_ADDR_LO, axDest[cusBLTCnt]);
      outpw( DEST_ADDR_HI, ayDest[cusBLTCnt]);
      outpw( BITMAP_WIDTH, ausWidth[cusBLTCnt]);
      outpw( BITMAP_HEIGHT, ausHeight[cusBLTCnt]);
      outp( BLT_CMD_1, XY_SRC_ADDR |
                       XY_DEST_ADDR  );
      if (fOverlap)
         outp( BLT_CMD_0, BACKWARD |
                          NO_BYTE_SWAP |
                          WRAP |
                          START_BLT      );
      else
         outp( BLT_CMD_0, FORWARD |
                          NO_BYTE_SWAP |
                          WRAP |
                          START_BLT      );
      }

}  /* Blt_Buff_SS_Pk() */
