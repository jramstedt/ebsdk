#ifndef H_VGAREG
#define H_VGAREG
/*#define DEBUG*/

/*
 * ISA cycle type masks.
 * All of these have been converted so that the BUS driver will handle
 * the appropriate shifting of data.  
 */

#define LOBYTE   (0x00000000)


#define HIBYTE   (0x00000001)
#define WWORD     (0x00000000)
#define ENC_BIT2 (0x00000002) /*huh?*/


/*
 * Configuration.
 */
/*	 
**  In order to support all of the board properly, extended mode 3 has been
**  disabled for the time being.  If time allows, ATI extended mode 3 will be
**  worked on later.  Mode 3+ requires VGACLKBITS to be 5
*/
#define	VGACLKBITS 1

#define VGAGSEG (0xA0000)    /*base address of the font tables on the vga option*/
#define VGAASEG (0xB8000)    /*base address of the visible screen memory */
#define VGABASE VGAASEG

#define GENMO_W 0x03C2      /* misc output write */
#define GENMO_R 0x03CC      /* misc output read */

#define	GENS0	0x3c2
#define	GENS1	0x3da

#define	VGAMORW	0x03C2
#define	VGAMORR	0x03CC
#define	VGAFCRW	0x03DA
#define	VGAFCRR	0x03CA
#define	VGAIS0R	0x03C2
#define	VGAIS1R	0x03DA

#define	VGASEQA	0x03C4
#define	VGASEQD	0x03C5
#define	VGACRTA	0x03D4
#define	VGACRTD	0x03D5
#define	VGAGFXA	0x03CE
#define	VGAGFXD	0x03CF
#define	VGAATRA	0x03C0
#define	VGAATRD	0x03C0
#define	VGADACW	0x03C8
#define	VGADACR	0x03C7
#define	VGADACD	0x03C9
#define	VGADACM	0x03C6

#define GENENA	0x46e8
#define GENVS	0x0102

#define	MEM_CFG	0x5eee
#define	MISC_OPTIONS	0x36ee

/*
 * Colours.
 */
#define	BGROUND	0x09			/* Bright blue.			*/
#define	FGROUND	0x0F			/* White.			*/
#define	BORDER	0x0C			/* Bright red.			*/

#define xBYTEMASK 0x6000000

#define ui unsigned int

/*
 * Initialization tables.
 */
/*
 * Initialization tables.
 */

/* CRT controller settings for LCD setup */
#define	LCD_NCINIT 13    /* 13 entries in the arrays below */
#define CRTC_11_INDX 9   /* the 9th, (1st=0), must be CRTC_11 */

/* since CRTC 7..0 can only be written if CRTC_11.7=1,  CRTC_11  */
/*    (with bit 7=1) must follow CRTC 7..0 in the lists below    */

char lcd_vgacindx[LCD_NCINIT] =
   {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x10,0x11,0x12,0x15,0x16};

/* Shadow CRT controller register settings for the TFT */
#ifdef R640
char lcd_vgacinit[LCD_NCINIT] = {
    0x5F,	/* Horizontal total */
    0x4F,	/* Horizontal display enable end */
    0x50,	/* Start horizontal blanking */
    0x82,	/* End horizontal blanking */
    0x55,	/* Start horizontal retrace */
    0x80,	/* End horizontal retrace */
    0x0B,	/* Vertical total (0x20b) */
    0x3E,	/* Vertical overflow	V retrace start:9 = 0
					V disp en end:9 = 0
					V total:9 = 1
					Line compare:8 = 1
					Start V blank:8 = 1
					V retrace start:8 = 1
					V disp en end:8 = 1
					V total:8 = 0 */
    0xEA,	/* Vertical retrace start (0x1ea) */
    0x0E,	/* Vertical retrace end */
    0xDF,	/* Vertical display enable end (0x1df) */
    0xE7,	/* Start vertical blank (0x1e7) */
    0x04	/* End vertical blank */
};
   /* {0x5F,0x4F,0x50,0x82,0x55,0x81,0x0f,0x3E,0xEC,0x8E,0xDF,0xE6,0xb9}; */
   /* CSM values for shadow CRTC for DUAL mode 3, derived from pc-3 DUAL */
#else		/* else 800x600 mode */
char lcd_vgacinit[LCD_NCINIT] = {
/*
index	value		description */
/*00*/	0x7b,		/* Horizontal total = chars + retrace - 5 = 123, chars=100 ... retrace = 28 */
	0x63,		/* Horizontal display enable end = chars - 1 = 99 ... 100 characters/line */
	0x64,		/* H blanking begins at: char 100 */
	0x9e,		/* H blanking ends at: char 30(1e)+32(idx 05)=62, top bit undefined ? */
/*04*/	0x69,		/* H retrace begins at: char 105 */
	0x92,		/* H retrace ends at char 18(12), (bit 7 is MSB of H blank end reg) */
	0x6f,		/* V t - raster lines -2 = (02)(6f) */
	0xf0,		/* V retr beg[9]=1, V ee[9]=1, V t[9]=1, line comp[8]=1 */
				/* 	V retrace start:9 = 1
					V disp en end:9 = 1
					V total:9 = 1
					Line compare:8 = 1
					Start V blank:8 = 0
					V retrace start:8 = 0
					V disp en end:8 = 0
					V total:8 = 0 */
/* note index discontinuity! */
/*10*/	0x58,		/* V ret beg - 1 = (02)(58) = */
/*11*/	0x0a,		/* V ret end=scan line 10 */
/*12*/	0x57,		/* Vertical display enable end; V ee - 1 = (02)(57) = */
/* note index discontinuity! */
/*15*/	0x58,		/* V bl beg -1 = (02)(58) = */
/*16*/	0x65,		/* V bl end -1 = (6f) = 111 */
};
#endif

/*Graphics mode 37*/
/*Graphics constants*/
#define SEQ_PIXEL_WR_MASK 0x02
#define DAC_CMD_0 0x83c6
#define DAC_CMD_1 0x13c8
#define DAC_CMD_2 0x13c9
#define CO_COLOR_WRITE 0x83c8
#define CO_COLOR_DATA  0x83c9

/* General registers */
#define MO_WRITE_DATA_37 0x2b

/* Extended reg */
#define EXTENDED_GR40_A 0x40
#define EXTENDED_GR40_D 0x01
#define EXTENDED_GR42_A 0x42
#define EXTENDED_GR42_D 0x00
#define EXTENDED_GR51_A 0x51
#define EXTENDED_GR51_D 0x00

#define EXTENDED_63CA_A 0x63ca
#define EXTENDED_63CA_D 0x01
#define EXTENDED_DAC_CMD1_D 0x60

#define EXTENDED_CRTC51_A 0x51
#define EXTENDED_CRTC51_D 0x00

/*
 * VGA memory and I/O addresses. Any addresses
 * that depend on the value of the IOA bit in the
 * miscellaneous output register have the appropriate value
 * for IOA=1; that is, they have the 0x03Dx form.
 */
/* VGA memory address used when writing font definitions (this is
   the VGA graphics mode segment in a PC) */
#define VGA_GSEG		((byte_address_t) 0xA0000)
/* VGA memory address used when writing text characters (this is
   the VGA alphanumeric mode  segment in a PC) */
#define VGA_ASEG		((byte_address_t) 0xB8000)

#define VGA_MORW		0x03C2	/* Misc output register (write) */
#define VGA_MORR		0x03CC	/* Misc output register (read) */
#define VGA_FCRW		0x03DA	/* Feature control register (write) */
#define VGA_FCRR		0x03CA	/* Feature control register (read) */
#define VGA_ISR0		0x03C2	/* Input status register 0 (read) */
#define VGA_ISR1		0x03DA	/* Input status register 1 (read) */

#define VGA_SEQA		0x03C4	/* Sequencer index register */
#define VGA_SEQD		0x03C5	/* Sequencer data register */
#define VGA_CRTA		0x03D4	/* CRT controller index register */
#define VGA_CRTD		0x03D5	/* CRT controller data register */
#define VGA_GFXA		0x03CE	/* Graphics controller index register */
#define VGA_GFXD		0x03CF	/* Graphics controller data register */
#define VGA_ATRA		0x03C0	/* Attribute controller index register */
#define VGA_ATRD		0x03C0	/* Attribute controller data register */
#define VGA_DACW		0x03C8	/* RAMDAC write address */
#define VGA_DACR		0x03C7	/* RAMDAC read address */
#define VGA_DACD		0x03C9	/* RAMDAC pixel data */
#define VGA_DACM		0x03C6	/* RAMDAC pixel mask */

#define RESET 0x00

/* Index values for Graphics controller registers */
#define VGA_GC_SET_RESET	0x00	/* Set/reset register */
#define VGA_GC_EN_SET_RESET	0x01	/* Enable set/reset register */
#define VGA_GC_COLOUR_CMP	0x02	/* Colour compare register */
#define VGA_GC_DATA_ROTATE	0x03	/* Data rotate register */
#define VGA_GC_READ_MAP_SEL	0x04	/* Read map select register */
#define VGA_GC_GMODE		0x05	/* Graphics mode */
#define VGA_GC_MISC		0x06	/* Miscellaneous register */
#define VGA_GC_COLOUR_DONT_CARE	0x07	/* Colour don't care register */
#define VGA_GC_BIT_MASK		0x08	/* Bit mask register */

/* Index values for the CRT controller registers */
#define VGA_CRT_CURSOR_HIGH	0x0e	/* Cursor location high byte */
#define VGA_CRT_CURSOR_LOW	0x0f	/* Cursor location low byte */

/* Index values for Sequencer registers */
#define VGA_SEQ_RESET		0x00	/* Reset */
#define VGA_SEQ_CLOCK		0x01	/* Clocking mode */
#define VGA_SEQ_MAP_MASK	0x02	/* Map mask */
#define VGA_SEQ_CHMAP_SEL	0x03	/* Character map select */
#define VGA_SEQ_MEM_MODE	0x04	/* Memory mode */

/* Index values for Paradise registers */
#define VGA_PR0A		0x09	/* Address offset register A */
#define VGA_PR0B		0x0a	/* Address offset register B */
#define VGA_PR1			0x0b	/* Memory size */
#define VGA_PR1A		0x33
#define VGA_PR1B		0x34
#define VGA_PR2			0x0c	/* Video select register */
#define VGA_PR3			0x0d	/* CRT lock control register */
#define VGA_PR4 		0x0E
#define VGA_PR5			0x0f	/* Gen purpose status/unlock PR0-PR4 */
#define VGA_PR10		0x29	/* Unlock PR11 - PR17 */
#define VGA_PR11		0x2a	/* EGA switches */
#define VGA_PR15		0x2e	/* Miscellaneous control */
#define VGA_PR18		0x31
#define VGA_PR19		0x32
#define VGA_PR20		0x06	/* Unlock Paradise extended registers */
#define VGA_PR30		0x35
#define VGA_PR32		0x12	/* Miscellaneous control 4 */
#define VGA_PR33		0x38
#define VGA_PR34		0x39
#define VGA_PR34A		0x14	/* via VGA_SEQA */
#define VGA_PR35		0x3A
#define VGA_PR36		0x3B
#define VGA_PR37		0x3C
#define VGA_PR39		0x3E
#define VGA_PR41		0x37
#define VGA_PR44		0x3F
#define VGA_PR57		0x19
#define VGA_PR59		0x21
#define VGA_PR62		0x24
#define VGA_PR66		0x29	/* VGA_SEQA */
#define VGA_PR68		0x31	/* VGA_SEQA */
#define VGA_PR72		0x35	/* VGA_SEQA */

/*
 * Colours.
 */
#define	DEF_BACKGROUND	0x09		/* Bright blue */
#define	DEF_FOREGROUND	0x0F		/* White */
#define	DEF_BORDER	0x00		/* Black */

/* param for VGA_IOCTL_DEF_VMODE */
#define VMODE_VGA		0
#define VMODE_SVGA		1
#define VMODE_XVGA		2

/* Ioctl command definitions */
#define VGA_IOCTL_LOAD_FONTS	0x01	/* Load VGA fonts */
#define VGA_IOCTL_SET_FG	0x02	/* Set foreground colour */
#define VGA_IOCTL_SET_BG	0x03	/* Set background colour */
#define VGA_IOCTL_SET_BORDER	0x04	/* Set border colour */
#define VGA_IOCTL_SET_MOUSE_SCALE 0x05	/* Set mouse motion scale factor */
#define VGA_IOCTL_SET_LINEAR	0x06	/* Set linear addressing */
#define VGA_IOCTL_CLEAR_SCREEN	0x07
#define VGA_IOCTL_CURSOR_OFF	0x08
#define VGA_IOCTL_DEF_VMODE	0x09
#endif

