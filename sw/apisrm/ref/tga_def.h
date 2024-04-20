/*
 * tga_def.h
 *
 * Copyright (C) 1995 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 * Abstract:	Definitions for TGA Graphics Adapter
 *
 * Author:	Console Firmware Team
 *
 */

#define TGAMaxHorizontalPixels	1280
#define TGAMaxVerticalPixels	1024

#define TGACharWidthInPixels	8
#define TGACharHeightInPixels	16

#define TGAMaxDisplayWidth	(TGAMaxHorizontalPixels / TGACharWidthInPixels)
#define TGAMaxDisplayHeight	(TGAMaxVerticalPixels / TGACharHeightInPixels)

#define TGA8x16CharStart	0x20
#define TGA8x16CharEnd		0x7F

#define TGA8x16AuxCharStart	0x80
#define TGA8x16AuxCharEnd	0xF8

/* Define types of Graphics cards. */

#define UNINITIALIZED_GRAPHICS		0
#define FRAME_BUFFER_8BPP_GRAPHICS	1
#define FRAME_BUFFER_32BPP_GRAPHICS	2

/* Different TGA module types */

#define TGA_8PLANE	  0	/* T8-02: 2Mb, 8 bit per pixel */
#define TGA_32PLANE_NO_Z  1	/* T32-08: 8Mb, 32 bit per pixel */
#define TGA_32PLANE	  2	/* T32-08: 8Mb, 32 bit per pixel */
#define TGA_32PLANE_Z_BUF 3	/* T32-88: 16Mb, 32 bpp (8Mb FB+8Mb Z) */

/* Definitions for the Digital TGA boards  (DECchip 21030) */

#define TGA_CONFIG_CMD		(0x4)
#define TGA_LATENCY_TIMER	(0xc)
#define TGA_LINE_INTERRUPT	(0x3c)
#define TGA_PALLETE_SNOOP	(0x40)
#define TGA_ADDRESS_EXT		(0x44)

#define TGA_ALT_ROM_BASE	(0x0)
#define TGA_DEEP                (0x100050)
#define TGA_PLANEMASK           (0x100028)
#define TGA_PIXELMASK           (0x10005C)
#define TGA_RASTEROP            (0x100034)
#define TGA_BLOCK_COLOR_0       (0x100140)
#define TGA_BLOCK_COLOR_1       (0x100144)
#define TGA_HORIZONTAL_CONTROL  (0x100064)
#define TGA_VERTICAL_CONTROL    (0x100068)
#define TGA_MODE                (0x100030)
#define TGA_VIDEO_BASE_ADDRESS  (0x10006C)
#define TGA_COMMAND_STATUS      (0x1001F8)
#define TGA_CLOCK               (0x1001E8)
#define TGA_RAMDAC_SETUP        (0x1000C0)
#define TGA_RAMDAC_INTERFACE    (0x1001F0)
#define TGA_VIDEO_VALID         (0x100070)
#define TGA_EPROM_WRITE         (0x1001E0)
#define TGA_INTERRUPT_STATUS    (0x10007C)
#define TGA_PIXEL_SHIFT		(0x100038)
#define TGA_DATA_REGISTER	(0x100080)
#define TGA_DMA_BASE_ADDRESS	(0x100098)
#define TGA_BOOLEAN_OP		(0x100034)

#define TGA_FRAME_END_PEND	0x01

#define TGA_VRAM_MEM_8		0x0200000
#define TGA_VRAM_MEM_32_NOZ	0x0800000
#define TGA_VRAM_MEM_32_Z	0x1000000

#define DMA_READ_COPY_NON_DITHERED       	0x17
#define DMA_READ_COPY_DITHERED           	0x37
#define DMA_WRITE_COPY                   	0x1F

/* Brooktree BT485 used on the 8 plane TGA module (2MB frame buffer, 8 bpp) */

#define TGA_BT485_READ(addr,data)		\
    wpci(TGA_RAMDAC_SETUP, (addr)<<1); 		\
    data = rpci(TGA_RAMDAC_INTERFACE);		\
    data = (data >> 16) & 0xFF;

#define TGA_BT485_WRITE(addr,data)		\
    wpci(TGA_RAMDAC_SETUP, (addr)<<1);		\
    wpci(TGA_RAMDAC_INTERFACE, ((addr)<<9) | (data)&0xFF);

#define BT485_PALETTE_CURSOR_WRITE_ADDR		0x0
#define BT485_PALETTE_DATA			0x1
#define BT485_PIXEL_MASK_REG			0x2
#define BT485_PALETTE_CURSOR_READ_ADDR		0x3
#define BT485_CURSOR_OVERSCAN_COLOR_WRITE_ADDR	0x4
#define BT485_CURSOR_OVERSCAN_COLOR_DATA	0x5
#define BT485_COMMAND_REG_0			0x6
#define BT485_CURSOR_OVERSCAN_COLOR_READ_ADDR	0x7
#define BT485_COMMAND_REG_1			0x8
#define BT485_COMMAND_REG_2			0x9
#define BT485_STATUS_REG			0xA
#define BT485_COMMAND_REG_3			0xA
					/* See Bt485 Spec. */
#define BT485_CURSOR_ARRAY_DATA			0xB
#define BT485_CURSOR_X_LOW			0xC
#define BT485_CURSOR_X_HIGH			0xD
#define BT485_CURSOR_Y_LOW			0xE
#define BT485_CURSOR_Y_HIGH			0xF

/*
 * Brooktree BT463 used on the 32 plane TGA module (8MB frame buffer, 32 bpp)
 */
#define TGA_BT463_LOAD_ADDR(addr)					\
    wpci(TGA_RAMDAC_SETUP, (BT463_ADDR_LOW)<<2);			\
    wpci(TGA_RAMDAC_INTERFACE, ((BT463_ADDR_LOW)<<10) | ((addr)&0xFF));	\
    wpci(TGA_RAMDAC_SETUP, (BT463_ADDR_HIGH)<<2);			\
    wpci(TGA_RAMDAC_INTERFACE, ((BT463_ADDR_HIGH)<<10) | (((addr)>>8)&0xFF));

/*
 * For the TGA_BT463_READ and TGA_BT463_WRITE macros, "mode" is either:
 *	BT463_REG_ACC for normal register access, or
 *	BT463_PALETTE for palette access
 */

#define TGA_BT463_WRITE_BIT	0x0
#define TGA_BT463_READ_BIT	0x2

#define TGA_BT463_READ(mode,addr,data)				\
    TGA_BT463_LOAD_ADDR(addr)					\
    wpci(TGA_RAMDAC_SETUP, (mode)<<2 | TGA_BT463_READ_BIT);	\
    data = rpci(TGA_RAMDAC_INTERFACE);				\
    data = (data >> 16) & 0xFF;

#define TGA_BT463_WRITE(mode,addr,data)				\
    TGA_BT463_LOAD_ADDR(addr)					\
    wpci(TGA_RAMDAC_SETUP, (mode)<<2 | TGA_BT463_WRITE_BIT); 	\
    wpci(TGA_RAMDAC_INTERFACE,					\
	((((mode)<<2) | TGA_BT463_WRITE_BIT)<<8) | (data)&0xFF );

#define BT463_ADDR_LOW			0x0
#define BT463_ADDR_HIGH			0x1
#define BT463_REG_ACC			0x2
#define BT463_PALETTE			0x3

#define BT463_CURSOR_COLOR_0		0x0100
#define BT463_CURSOR_COLOR_1		0x0101

#define BT463_ID_REG			0x0200
					/*  value = 0x2A */
#define BT463_COMMAND_REG_0		0x0201
#define BT463_COMMAND_REG_1		0x0202
#define BT463_COMMAND_REG_2		0x0203

#define BT463_READ_MASK_P0_P7		0x0205
#define BT463_READ_MASK_P8_P15		0x0206
#define BT463_READ_MASK_P16_P23		0x0207
#define BT463_READ_MASK_P24_P31		0x0208

#define BT463_BLINK_MASK_P0_P7		0x0209
#define BT463_BLINK_MASK_P8_P15		0x020A
#define BT463_BLINK_MASK_P16_P23	0x020B
#define BT463_BLINK_MASK_P24_P31	0x020C

#define BT463_REVISION			0x0220

#define BT463_WINDOW_TYPE_BASE		0x0300	/* 0x0300 to 0x030F */

#define mode_simple			0x00

#define SourceVisualPacked    0
#define SourceVisualUnPacked  1
#define SourceVisualLow       2
#define SourceVisualHi        6
#define SourceVisualTrue      3

#define DestVisualPacked      0
#define DestVisualUnPacked    1
#define DestVisual12bit       2
#define DestVisualTrue        3

#define DmaMallocSize	(2 * 1024)
#define CONSOLE_OFFSET	0x0

enum  rasterops
{
        GXclear,
        GXand,
        GXandReverse,
        GXcopy,
        GXandInverted,
        GXnoop,
        GXxor,
        GXor,
        GXnor,
        GXequiv,
        GXinvert,
        GXorReverse,
        GXcopyInverted,
        GXxorcopyInverted,
        GXnand,
        GXset
};

/*
 * Define colors, HI = High Intensity
 *
 * The palette registers are simply 1:1 indirections to the color registers.
 *
 */

#define FW_COLOR_BLACK      0x00
#define FW_COLOR_RED        0x01
#define FW_COLOR_GREEN      0x02
#define FW_COLOR_YELLOW     0x03
#define FW_COLOR_BLUE       0x04
#define FW_COLOR_MAGENTA    0x05
#define FW_COLOR_CYAN       0x06
#define FW_COLOR_WHITE      0x07
#define FW_COLOR_HI_BLACK   0x08
#define FW_COLOR_HI_RED     0x09
#define FW_COLOR_HI_GREEN   0x0A
#define FW_COLOR_HI_YELLOW  0x0B
#define FW_COLOR_HI_BLUE    0x0C
#define FW_COLOR_HI_MAGENTA 0x0D
#define FW_COLOR_HI_CYAN    0x0E
#define FW_COLOR_HI_WHITE   0x0F

#define DecodeMask	0xFE000000
#define PCI_BUS		3

#define CONTROL_SEQUENCE_MAX_PARAMETER 10
#define IDLE_WAIT	100000

/*
 * This is used as a bit table for serial load ISC 1562
 * Each byte is 8 bits when written
 * so the table size is 7 bytes for 56 bits
 * with 1 byte pad for alignment
 */

struct ics_data 
{                      
    char     data[8];
};
        
typedef struct ics_data ICS;

struct monitor_data
{
    int monitor_columns;            
    int monitor_rows;               
    int sold_freq;                  
    int refresh_rate;               
    int vert;
    int horiz;
    ICS ics_data;
};
         
