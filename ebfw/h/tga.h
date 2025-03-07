
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
 * $Id: tga.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $(EB64 Debug Monitor);
 *
 * $Log: tga.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1995/10/05  01:05:24  fdh
 * Added function prototypes and included nttypes.h.
 *
 * Revision 1.2  1995/10/03  03:29:42  fdh
 * Replaced references to demse memory access routines with
 * XXXX_REGISTER_ULONG() routines.
 *
 * Revision 1.1  1995/08/25  20:59:32  fdh
 * Initial revision
 *
 */
/*++

Copyright (c) 1994  Digital Equipment Corporation
 
Module Name:

    tga.h

Abstract:

    This header file defines values for the TGA frame buffer (DECchip 21030).
    

Author:

    Eric Rehm 23-March-1994


Revision History:

--*/

#include "nttypes.h"

#define DIGITAL_TGA_PCI_DEVID   0x00041011   // Digital TGA

//
// Define types of Graphics cards.
//

typedef enum _GRAPHICS_TYPE {
    UNINITIALIZED_GRAPHICS,
    FRAME_BUFFER_8BPP_GRAPHICS,
    FRAME_BUFFER_32BPP_GRAPHICS
} GRAPHICS_TYPE;

//
// Definitions for the Digital TGA boards  (DECchip 21030)
//
#define TGA_ALT_ROM_BASE	(PULONG)(PciVideoRegisterBase)
#define TGA_DEEP                (PULONG)(PciVideoRegisterBase + 0x00100050)
#define TGA_PLANEMASK           (PULONG)(PciVideoRegisterBase + 0x00100028)
#define TGA_PIXELMASK           (PULONG)(PciVideoRegisterBase + 0x0010005C)
#define TGA_RASTEROP            (PULONG)(PciVideoRegisterBase + 0x00100034)
#define TGA_BLOCK_COLOR_0       (PULONG)(PciVideoRegisterBase + 0x00100140)
#define TGA_BLOCK_COLOR_1       (PULONG)(PciVideoRegisterBase + 0x00100144)
#define TGA_HORIZONTAL_CONTROL  (PULONG)(PciVideoRegisterBase + 0x00100064)
#define TGA_VERTICAL_CONTROL    (PULONG)(PciVideoRegisterBase + 0x00100068)
#define TGA_MODE                (PULONG)(PciVideoRegisterBase + 0x00100030)
#define TGA_VIDEO_BASE_ADDRESS  (PULONG)(PciVideoRegisterBase + 0x0010006C)
#define TGA_COMMAND_STATUS      (PULONG)(PciVideoRegisterBase + 0x001001F8)
#define TGA_CLOCK               (PULONG)(PciVideoRegisterBase + 0x001001E8)
#define TGA_RAMDAC_SETUP        (PULONG)(PciVideoRegisterBase + 0x001000C0)
#define TGA_RAMDAC_INTERFACE    (PULONG)(PciVideoRegisterBase + 0x001001F0)
#define TGA_VIDEO_VALID         (PULONG)(PciVideoRegisterBase + 0x00100070)
#define TGA_EPROM_WRITE         (PULONG)(PciVideoRegisterBase + 0x001001E0)
#define TGA_INTERRUPT_STATUS    (PULONG)(PciVideoRegisterBase + 0x0010007C)
#define TGA_FRAME_END_PEND	0x01

//
// Different module types that Digital is building
//
#define TGA_8PLANE		0	// T8-02: 2Mb, 8 bit per pixel
#define TGA_24PLANE		1	// T32-08: 8Mb, 32 bit per pixel
//#define TGA_???		2	//  (undefined as of now)
#define TGA_24PLANE_Z_BUF	3	// T32-88: 16Mb, 32 bpp (8Mb FB+8Mb Z)

//
// Brooktree BT485 used on the 8 plane TGA module (2MB frame buffer, 8 bpp)
//
#define TGA_BT485_READ(addr,data)	\
    WRITE_REGISTER_ULONG (TGA_RAMDAC_SETUP, (addr)<<1); 	\
    data = READ_REGISTER_ULONG (TGA_RAMDAC_INTERFACE);		\
    data = (data >> 16) & 0xFF;

#define TGA_BT485_WRITE(addr,data)	\
    WRITE_REGISTER_ULONG (TGA_RAMDAC_SETUP, (addr)<<1);		\
    WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, ((addr)<<9) | ((data)&0xFF));


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
#define BT485_COMMAND_REG_3			0xA   // See Bt485 Spec. to R/W
#define BT485_CURSOR_ARRAY_DATA			0xB
#define BT485_CURSOR_X_LOW			0xC
#define BT485_CURSOR_X_HIGH			0xD
#define BT485_CURSOR_Y_LOW			0xE
#define BT485_CURSOR_Y_HIGH			0xF

//
// Brooktree BT463 used on the 24 plane TGA module (8MB frame buffer, 32 bpp)
//
#define TGA_BT463_LOAD_ADDR(addr)		\
    WRITE_REGISTER_ULONG (TGA_RAMDAC_SETUP, (BT463_ADDR_LOW)<<2);	\
    WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE,				\
    			   ((BT463_ADDR_LOW)<<10) | ((addr)&0xFF));	\
    WRITE_REGISTER_ULONG (TGA_RAMDAC_SETUP, (BT463_ADDR_HIGH)<<2);	\
    WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE,				\
    			   ((BT463_ADDR_HIGH)<<10) | (((addr)>>8)&0xFF));

//
// For the TGA_BT463_READ and TGA_BT463_WRITE macros, "mode" is either:
//	BT463_REG_ACC for normal register access, or
//	BT463_PALETTE for palette access
//
#define TGA_BT463_WRITE_BIT	0x0
#define TGA_BT463_READ_BIT	0x2

#define TGA_BT463_READ(mode,addr,data)		\
    TGA_BT463_LOAD_ADDR(addr)			\
    WRITE_REGISTER_ULONG (TGA_RAMDAC_SETUP, (mode)<<2 | TGA_BT463_READ_BIT);  \
    data = READ_REGISTER_ULONG (TGA_RAMDAC_INTERFACE);			      \
    data = (data >> 16) & 0xFF;

#define TGA_BT463_WRITE(mode,addr,data)		\
    TGA_BT463_LOAD_ADDR(addr)			\
    WRITE_REGISTER_ULONG (TGA_RAMDAC_SETUP, (mode)<<2 | TGA_BT463_WRITE_BIT); \
    WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE,				      \
    		((((mode)<<2) | TGA_BT463_WRITE_BIT)<<8) | ((data)&0xFF) );


#define BT463_ADDR_LOW			0x0
#define BT463_ADDR_HIGH			0x1
#define BT463_REG_ACC			0x2
#define BT463_PALETTE			0x3


#define BT463_CURSOR_COLOR_0		0x0100
#define BT463_CURSOR_COLOR_1		0x0101

#define BT463_ID_REG			0x0200		// value = 0x2A
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

#define BT463_WINDOW_TYPE_BASE		0x0300		// 0x0300 to 0x030F

//
// Video timing register paramteters for 640x480 resolution @ 60 Hz Vert. refresh
//

#define TGA_HORZ_640_X_480_60HZ         0x018608A0
#define TGA_VERT_640_X_480_60HZ         0x084251E0


/* Function Prototypes */
extern GRAPHICS_TYPE FwInitializeDigitalTGA(PHYSICAL_ADDRESS BaseAddress);
extern VOID FwOutputCharacter(UCHAR Character);
extern void tgaputc(register int c);
extern void tgaerasedisplay(void);
extern GRAPHICS_TYPE tgasetup(void);
