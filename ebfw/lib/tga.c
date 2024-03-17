
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
static char *rcsid = "$Id: tga.c,v 1.1.1.1 1998/12/29 21:36:12 paradis Exp $";
#endif

/*
 * $Log: tga.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:12  paradis
 * Initial CVS checkin
 *
 * Revision 1.7  1996/06/05  21:09:25  fdh
 * Corrected bug where FwColumn was not updated properly.
 *
 * Revision 1.6  1996/05/15  20:38:00  fdh
 * Modified tgaputc() to output TABs.
 *
 * Revision 1.5  1995/10/31  22:52:42  cruz
 * Added compile conditional NEEDTGA around code.
 *
 * Revision 1.4  1995/10/31  16:47:19  cruz
 * Commented out some unused routines.
 *
 * Revision 1.3  1995/10/26  23:35:46  cruz
 * Added casting.  Fixed precedence bug (x & 1 == 1).
 *
 * Revision 1.2  1995/10/03  03:17:58  fdh
 * Replaced references to demse memory access routines with
 * XXXX_REGISTER_ULONG() routines.
 *
 * Revision 1.1  1995/08/25  20:32:50  fdh
 * Initial revision
 *
 */
/*++

Copyright (c) 1993,1994  Digital Equipment Corporation

Module Name:

    tga.c

Abstract:

    This module implements firmware TGA intialization.

Author:


Environment:

    Kernel mode.


Revision History:

--*/

#include "system.h"
#ifdef NEEDTGA

#if 0
#include "ntos.h"           /* Used To Define Basic Windows NT Types, E.G. UCHAR*/
                            /* Windows NT 3.5 DDK File \Ddk\Inc\Miniport.H Will*/
                            /* also suffice.*/
#else
#include "nttypes.h"
#include "halp.h"
#endif

#include "tga.h"            // Used to define TGA-specific constants

//
// Define colors, HI = High Intensity
//
// The palette registers are simply 1:1 indirections to the color registers.
//
//

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


//
// PCI board definitions.
//

//
// Definitions needed for Frame Buffer style graphics cards (including TGA)
//

const UCHAR VGADefaultPalette[16][3] = {

    {0x00, 0x00, 0x00},         // Black
    {0xAA, 0x00, 0x00},         // Red
    {0x00, 0xAA, 0x00},         // Green
    {0xAA, 0xAA, 0x00},         // Yellow

    {0x00, 0x00, 0xAA},         // Blue
    {0xAA, 0x00, 0xAA},         // Magenta
    {0x00, 0xAA, 0xAA},         // Cyan
    {0xAA, 0xAA, 0xAA},         // White

    {0x00, 0x00, 0x00},         // Hi Black
    {0xFF, 0x00, 0x00},         // Hi Red
    {0x00, 0xFF, 0x00},         // Hi Green
    {0xFF, 0xFF, 0x00},         // Hi Yellow

    {0x00, 0x00, 0xFF},         // Hi Blue
    {0xFF, 0x00, 0xFF},         // Hi Magenta
    {0x00, 0xFF, 0xFF},         // Hi Cyan
    {0xFF, 0xFF, 0xFF}          // Hi White
};

//
// Table to convert a packed nibble of pixel data into a longword mask used
// to determine foreground vs. background color.  Used for 8bpp frame buffers.
//

const ULONG FBPixelDataConvert[16] = {
    0x00000000,
    0xFF000000,
    0x00FF0000,
    0xFFFF0000,

    0x0000FF00,
    0xFF00FF00,
    0x00FFFF00,
    0xFFFFFF00,

    0x000000FF,
    0xFF0000FF,
    0x00FF00FF,
    0xFFFF00FF,

    0x0000FFFF,
    0xFF00FFFF,
    0x00FFFFFF,
    0xFFFFFFFF
};

// 
// External references to VGA 8x16 Font Table 
//
// VGA8x16Chars[] represents normal ASCII characters 0x20 -- 0x7f
// (space -- del).  Each byte is one scan line.
// e.g., for lower-case "a" charcter (ASCII 0x61), the entry would be:

#if 0
	0x00,  // 00000000
	0x00,  // 00000000
	0x00,  // 00000000
	0x00,  // 00000000
	0x00,  // 00000000
	0x78,  // 01111000
	0x0C,  // 00001100
	0x7C,  // 01111100
	0xCC,  // 11001100
	0xCC,  // 11001100
	0xCC,  // 11001100
	0x76,  // 01110110
	0x00,  // 00000000
	0x00,  // 00000000
	0x00,  // 00000000
	0x00,  // 00000000

#endif

// 
// VGA8x16Undef[] represents a single 8x16 undefined character.
// (This is the character to be displayed for ASCII values for
// which there is no definition in VGA8x16Chars or VGA8x16AuxChars.)
//
// VGA8x16Aux[] represents ASCII characters 0x80 -- 0xef.  
//


#define VGA8x16CharStart	0x20
#define VGA8x16CharEnd		0x7F
extern UCHAR vga8x16gl[];

extern UCHAR vga8x16xx[];

#define VGA8x16AuxCharStart	0x80
#define VGA8x16AuxCharEnd	0xEF
extern UCHAR vga8x16gr[];


//
// external references
//

//
// VideoDisplayString is used to pass Identifier for the 
// the DisplayControler type of ControllerClass in the ARC 
// configuration tree.
//
// This string is used by Text Mode Setup (during Windows NT
// installation) to find the proper video miniport driver
// to load.
//

#if 0
extern PUCHAR VideoDisplayString;
#endif

#if 0
//
// HalpMb() is used on Alpha AXP to order (serialize) reads and writes.
// The implmentation of this function is the single Alpha instruction "mb".
// 

extern VOID
  HalpMb(
      VOID
      );
#endif


//
// Static data.
//

LONG FwColumn;			// Column of character to be displayed
LONG FwRow;			// Row of character to be displayed
BOOLEAN FwHighIntensity;	// TRUE, if char to be displayed in bold
BOOLEAN FwReverseVideo;		// TRUE, if char to be displayed in rev video
ULONG FwForegroundColor;	// Forground color of character
ULONG FwBackgroundColor;	// Background color of character
ULONG DisplayWidth;		// Width of display, in characters
#ifdef NOT_IN_USE
ULONG DisplayHeight;		// Height of display, in characters
#endif
PUCHAR VideoMemoryBase;		// Pointer to frame buffer pixel 0
ULONG CharHeightInPixels;	
ULONG CharWidthInPixels;
ULONG BytesPerScanLine;
ULONG BytesPerPixel;

//
// Graphics board identifier.  This is intialized to UNINITIALIZED_GRAPHICS,
// indicating unknown graphcis card type.  When the system first inits, it 
// will determine the graphics card type and note it in this variable.
// (For example, FwIntializeDigitalTGA, returns a value of this type appropriate 
// for TGA.)  Subsequently, GraphicsCardType determines which frame buffer
// output method (i.e., 8 Bpp vs. 24 Bpp) to use in FwOutputCharacter.
//

GRAPHICS_TYPE GraphicsCardType = UNINITIALIZED_GRAPHICS;



//
// local function prototypes
//

GRAPHICS_TYPE
FwInitializeDigitalTGA(
        PHYSICAL_ADDRESS BaseAddress
        );

VOID
FwOutputCharacter (
    IN UCHAR Character
    );


GRAPHICS_TYPE
FwInitializeDigitalTGA(
    PHYSICAL_ADDRESS BaseAddress
    )

/*++

Routine Description:

    This routine initializes the DECchip 21030 (Digital TGA) and the
    associated RAMDAC on the currently supported module types.

Arguments:

    BaseAddress - BaseAddress field of TGA PCI Config Space
                  (where TGA is mapped to)

Return Value:

    FRAME_BUFFER_8BPP_GRAPHICS for 8 plane TGA card
    FRAME_BUFFER_32BPP_GRAPHICS for a 24 plane TGA card
    UNINITIALIZED_GRAPHICS if the module or DAC type is unknown

--*/

{

    PUCHAR PciVideoRegisterBase = NULL;
    ULONG i, j;
    ULONG Temp;
    ULONG Config;
    GRAPHICS_TYPE FbType;
    const UCHAR PLLbits[7] = { 0x80, 0x04, 0x00, 0x24, 0x44, 0x80, 0xB8 }; //25.175Mhz x 4 (640x480 @ 60Hz)
#if 0
    ULONG AddressSpace = 0;
    PHYSICAL_ADDRESS TranslatedAddress;
#endif

    //
    // Initialize static variables
    //

    FwColumn = 0;
    FwRow = 0;
    FwHighIntensity = TRUE;
    FwReverseVideo = FALSE;
    FwForegroundColor = FW_COLOR_WHITE;
    FwBackgroundColor = FW_COLOR_BLUE;

    DisplayWidth = 80;
#ifdef NOT_IN_USE
    DisplayHeight = 30;            // 640x480 graphics mode ==> 30 lines
#endif
    CharWidthInPixels = 8;        // Must be 8 for font used in FwOutputCharacter
    CharHeightInPixels = 16;
    BytesPerPixel = 1;
    BytesPerScanLine = DisplayWidth * CharWidthInPixels * BytesPerPixel;



    //
    // Determine the base addresses that were previously initialized
    // and then enable TGA memory space access and bus mastering (for DMA).
    //


#if 0
    HalTranslateBusAddress(
	PCIBus,
	0,                    // ecrfix - Bus 0 only for now.
	BaseAddress,
	&AddressSpace,
	&TranslatedAddress
	);
#endif

#if 0
    PciVideoRegisterBase = (PUCHAR)(TranslatedAddress.LowPart);
#else
    PciVideoRegisterBase = (PUCHAR) BaseAddress;
#endif

    //
    // Determine module type:
    //        "8 plane" = 2MB frame buffer, 8 bits per pixel
    //        "24 plane" = 8MB frame buffer, 32 bits per pixel
    //        "24 plane Z" = 8MB FB, 32 bpp + 8MB Z-buffer / offscreen memory, 32bpp
    //


    Temp = READ_REGISTER_ULONG (TGA_ALT_ROM_BASE);
    Config = (Temp >> 12) & 0x0F;

    mb();
    while ((READ_REGISTER_ULONG (TGA_COMMAND_STATUS) & 1) == 1) {
        ;                // Wait for not busy BEFORE writing Deep reg.
    }
    mb();

    switch (Config) {

      case TGA_8PLANE:
        WRITE_REGISTER_ULONG (TGA_DEEP, 0x00014000);
        FbType = FRAME_BUFFER_8BPP_GRAPHICS;
	printf("\n\t%s\n", "Initializing TGA 8 Plane");
        GraphicsCardType = FRAME_BUFFER_8BPP_GRAPHICS;
        break;

      case TGA_24PLANE:
        WRITE_REGISTER_ULONG (TGA_DEEP, 0x0001440D);
        FbType = FRAME_BUFFER_32BPP_GRAPHICS;
	printf("\n\t%s\n", "Initializing TGA 24 Plane");
        GraphicsCardType = FRAME_BUFFER_32BPP_GRAPHICS;
        break;

      case TGA_24PLANE_Z_BUF:
        WRITE_REGISTER_ULONG (TGA_DEEP, 0x0001441D);
        FbType = FRAME_BUFFER_32BPP_GRAPHICS;
	printf("\n\t%s\n", "Initializing TGA 24 Plane Z-buffer");
        GraphicsCardType = FRAME_BUFFER_32BPP_GRAPHICS;
        break;

      default:
        return UNINITIALIZED_GRAPHICS;                // Unknown module type
    }

    mb();
    while ((READ_REGISTER_ULONG (TGA_COMMAND_STATUS) & 1) == 1) {
        ;                // Wait for not busy AFTER writing Deep reg.
    }
    mb();

    switch (Config) {
      case TGA_8PLANE:
        VideoMemoryBase = PciVideoRegisterBase + 0x00200000;        // 2Mb offset
        WRITE_REGISTER_ULONG (TGA_RASTEROP, 0x0003);        // Src->Dst, 8bpp
        WRITE_REGISTER_ULONG (TGA_MODE, 0x00002000);        // Simple mode, 8bpp
        WRITE_REGISTER_ULONG (TGA_VIDEO_BASE_ADDRESS, 0x00000000);
        break;

      case TGA_24PLANE:
        VideoMemoryBase = PciVideoRegisterBase + 0x00804000;        // 8Mb offset
        WRITE_REGISTER_ULONG (TGA_RASTEROP, 0x0303);        // Src->Dst, 32bpp
        WRITE_REGISTER_ULONG (TGA_MODE, 0x00002300);        // Simple mode, 32bpp
        WRITE_REGISTER_ULONG (TGA_VIDEO_BASE_ADDRESS, 0x00000001);
        break;

      case TGA_24PLANE_Z_BUF:
        VideoMemoryBase = PciVideoRegisterBase + 0x01004000;        // 16Mb offset
        WRITE_REGISTER_ULONG (TGA_RASTEROP, 0x0303);        // Src->Dst, 32bpp
        WRITE_REGISTER_ULONG (TGA_MODE, 0x00002300);        // Simple mode, 32bpp
        WRITE_REGISTER_ULONG (TGA_VIDEO_BASE_ADDRESS, 0x00000001);
        break;
    }

    //
    // Write the PLL for 640x480 @ 72Hz
    //

    for (i = 0;  i <= 6;  i++) {

        for (j = 0;  j <= 7;  j++) {
            Temp = (PLLbits[i] >> (7-j)) & 1;
            if (i == 6  &&  j == 7)
                Temp |= 2;                      // Set ~HOLD bit on last write
            WRITE_REGISTER_ULONG (TGA_CLOCK, Temp);
        }
    }

    WRITE_REGISTER_ULONG (TGA_PLANEMASK, 0xFFFFFFFF);
    WRITE_REGISTER_ULONG (TGA_PIXELMASK, 0xFFFFFFFF);
    WRITE_REGISTER_ULONG (TGA_BLOCK_COLOR_0, 0x12345678);
    WRITE_REGISTER_ULONG (TGA_BLOCK_COLOR_1, 0x12345678);

    //
    // Init. video timing registers for 640x480 resolution
    //

    WRITE_REGISTER_ULONG (TGA_HORIZONTAL_CONTROL, TGA_HORZ_640_X_480_60HZ);
    WRITE_REGISTER_ULONG (TGA_VERTICAL_CONTROL,   TGA_VERT_640_X_480_60HZ);

    switch (Config) {
      case TGA_8PLANE:
        //
        // Initialize Brooktree Bt485 registers
        //

        //
        // For the following #if :
        //   0 = pass 1 modules that do not have the clock modification
        //   1 = pass 2 modules or pass 1 modules w/ clock modification
        //
        // NOTE: It is planned that all pass 1 modules will be modified
        //    so that the #else clause will be unnecessary in the future...
        //


        TGA_BT485_WRITE(BT485_COMMAND_REG_0, 0xA2);  // Enable Cmd_reg_3 access
        TGA_BT485_WRITE(BT485_PALETTE_CURSOR_WRITE_ADDR, 0x01);  // Cmd_reg_3
        TGA_BT485_WRITE(BT485_COMMAND_REG_3, 0x10);  // Enable differential clk

        TGA_BT485_WRITE(BT485_COMMAND_REG_1, 0x40);
        TGA_BT485_WRITE(BT485_COMMAND_REG_2, 0x20);

        TGA_BT485_WRITE(BT485_PIXEL_MASK_REG, 0xFF);

        //
        // Fill palette
        //
	  
        TGA_BT485_WRITE(BT485_PALETTE_CURSOR_WRITE_ADDR, 0x00);   // Addr 0

        WRITE_REGISTER_ULONG (TGA_RAMDAC_SETUP, (BT485_PALETTE_DATA)<<1);
        for (i = 0;  i < 16;  i++) {
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT485_PALETTE_DATA<<9)|
                                (ULONG)VGADefaultPalette[i][0]);    // Red
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT485_PALETTE_DATA<<9)|
                                (ULONG)VGADefaultPalette[i][1]);    // Green
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT485_PALETTE_DATA<<9)|
                                (ULONG)VGADefaultPalette[i][2]);    // Blue
        }

        //
        // Should have something better for rest of palette, but for now:
        //   Fill rest of palette with alternating red, green and blue colors.
        // It is really a don't care since these entries should never be used.
        //

        for (i = 0;  i < 240*3;  i += 4) {
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT485_PALETTE_DATA<<9) | 0x55);
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT485_PALETTE_DATA<<9));
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT485_PALETTE_DATA<<9));
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT485_PALETTE_DATA<<9));
        }

        break;

      case TGA_24PLANE:
      case TGA_24PLANE_Z_BUF:

        //
        // Static variable changes for 24 Plane cards
        //

        BytesPerPixel = 4;
        BytesPerScanLine = DisplayWidth * CharWidthInPixels * BytesPerPixel;

        //
        // Make sure RAMDAC is a Brooktree Bt463 then initialize registers
        //
        TGA_BT463_READ(BT463_REG_ACC, BT463_ID_REG, Temp);
        if (Temp != 0x2A) {
            return UNINITIALIZED_GRAPHICS;                // Unknown module type
        }

        WRITE_REGISTER_ULONG (TGA_VIDEO_VALID, 0x01);    // Enable video timing

        TGA_BT463_WRITE(BT463_REG_ACC, BT463_COMMAND_REG_0, 0x40);
        TGA_BT463_WRITE(BT463_REG_ACC, BT463_COMMAND_REG_1, 0x08);
        TGA_BT463_WRITE(BT463_REG_ACC, BT463_COMMAND_REG_2, 0x40);

        TGA_BT463_WRITE(BT463_REG_ACC, BT463_READ_MASK_P0_P7,   0xFF);
        TGA_BT463_WRITE(BT463_REG_ACC, BT463_READ_MASK_P8_P15,  0xFF);
        TGA_BT463_WRITE(BT463_REG_ACC, BT463_READ_MASK_P16_P23, 0xFF);
        TGA_BT463_WRITE(BT463_REG_ACC, BT463_READ_MASK_P24_P31, 0x0F);

        TGA_BT463_WRITE(BT463_REG_ACC, BT463_BLINK_MASK_P0_P7,   0x00);
        TGA_BT463_WRITE(BT463_REG_ACC, BT463_BLINK_MASK_P8_P15,  0x00);
        TGA_BT463_WRITE(BT463_REG_ACC, BT463_BLINK_MASK_P16_P23, 0x00);
        TGA_BT463_WRITE(BT463_REG_ACC, BT463_BLINK_MASK_P24_P31, 0x00);

        //
        // Fill palette
        //

        TGA_BT463_LOAD_ADDR(0x0000);   // Palette Address 0x0000

        WRITE_REGISTER_ULONG (TGA_RAMDAC_SETUP, (BT463_PALETTE)<<2);

        for (i = 0;  i < 16;  i++) {
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT463_PALETTE<<10) |
                                (ULONG)VGADefaultPalette[i][0]);    // Red
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT463_PALETTE<<10) |
                                (ULONG)VGADefaultPalette[i][1]);    // Green
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT463_PALETTE<<10) |
                                (ULONG)VGADefaultPalette[i][2]);    // Blue
        }

        //
        // Should have something better for rest of palette, but for now:
        //   Fill rest of palette with alternating red, green and blue colors.
        // It is really a don't care since these entries should never be used.
        //
        for (i = 0;  i < 512*3;  i += 4) {
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT463_PALETTE<<10) | 0x55);
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT463_PALETTE<<10));
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT463_PALETTE<<10));
            WRITE_REGISTER_ULONG (TGA_RAMDAC_INTERFACE, (BT463_PALETTE<<10));
        }

        //
        // Fill window type table after making sure the vertical retrace
        // is just beginning.
        //

        while ((READ_REGISTER_ULONG(TGA_INTERRUPT_STATUS) & TGA_FRAME_END_PEND) == 0) {
            ;                        // Wait for end of frame
        }
        WRITE_REGISTER_ULONG(TGA_INTERRUPT_STATUS, TGA_FRAME_END_PEND);

        mb();
        while ((READ_REGISTER_ULONG(TGA_INTERRUPT_STATUS) & TGA_FRAME_END_PEND) == 0) {
            ;                        // Wait for end of frame
        }
        WRITE_REGISTER_ULONG(TGA_INTERRUPT_STATUS, TGA_FRAME_END_PEND);

        TGA_BT463_LOAD_ADDR(BT463_WINDOW_TYPE_BASE);

        WRITE_REGISTER_ULONG (TGA_RAMDAC_SETUP, (BT463_REG_ACC)<<2);

        for (i = 0;  i < 16;  i++) {
            WRITE_REGISTER_ULONG(TGA_RAMDAC_INTERFACE,(BT463_REG_ACC<<10)|0x00);
            WRITE_REGISTER_ULONG(TGA_RAMDAC_INTERFACE,(BT463_REG_ACC<<10)|0x01);
            WRITE_REGISTER_ULONG(TGA_RAMDAC_INTERFACE,(BT463_REG_ACC<<10)|0x80);
        }

        break;

    }        // switch (Config)

    WRITE_REGISTER_ULONG (TGA_VIDEO_VALID, 0x01);    // Enable video timing

    return FbType;
}

VOID
FwOutputCharacter (
    IN UCHAR Character
    )

/*++

Routine Description:

    This routine displays a single character on the video screen at the current
    cursor location with the current color and video attributes.  It assumes
    the character locations are word aligned.

Arguments:

    Character - Supplies the character to be displayed in the video
                cards memory space.

Return Value:

    None.

--*/

{
    ULONG FGColor;
    ULONG BGColor;
    PUCHAR Destination;
    ULONG I, J, Data, Mask;
    PUCHAR FontPtr;

    //
    // Map ASCII code 7 (BELL) to bullet
    //
    if (Character == 7) {
            Character = '~' + 1;
    }

    if (FwReverseVideo) {
        FGColor = FwBackgroundColor;
        BGColor = FwForegroundColor + (FwHighIntensity ? 0x08 : 0 );
    } else {
        FGColor = FwForegroundColor + (FwHighIntensity ? 0x08 : 0 );
        BGColor = FwBackgroundColor;
    }
    FGColor &= 0x0F;
    BGColor &= 0x0F;

    Character &= 0xFF;
    if ((Character >= VGA8x16CharStart) && (Character <= VGA8x16CharEnd)) {
        // Normal printing characters
        FontPtr = &vga8x16gl[16 * (Character - VGA8x16CharStart)];
    } else if ((Character >= VGA8x16AuxCharStart) && (Character <= VGA8x16AuxCharEnd)) {
        // Line drawing characters
        FontPtr = &vga8x16gr[16 * (Character - VGA8x16AuxCharStart)];
    } else {
        // Undefined characters
        FontPtr = &vga8x16xx[0];
    }

    Destination = (PUCHAR)(VideoMemoryBase +
                    FwRow * CharHeightInPixels * BytesPerScanLine +
                    FwColumn * CharWidthInPixels * BytesPerPixel
                    );

    switch (GraphicsCardType) {

      case FRAME_BUFFER_8BPP_GRAPHICS:

        FGColor = FGColor << 8 | FGColor;
        FGColor = FGColor << 16 | FGColor;
        BGColor = BGColor << 8 | BGColor;
        BGColor = BGColor << 16 | BGColor;

        for (I = 0; I < CharHeightInPixels; I++) {
            Data = FBPixelDataConvert[FontPtr[I] >> 4];
            Data = (Data & FGColor) | (~Data & BGColor);
            WRITE_REGISTER_ULONG((PULONG)(Destination), Data);

            Data = FBPixelDataConvert[FontPtr[I] & 0x0F];
            Data = (Data & FGColor) | (~Data & BGColor);
            WRITE_REGISTER_ULONG((PULONG)(Destination+4), Data);

            Destination += BytesPerScanLine;
        }

        break;

      case FRAME_BUFFER_32BPP_GRAPHICS:
        FGColor = VGADefaultPalette[FGColor][0] << 16 |                // Red
                  VGADefaultPalette[FGColor][1] << 8 |                // Green
                  VGADefaultPalette[FGColor][2];                // Blue
        BGColor = VGADefaultPalette[BGColor][0] << 16 |                // Red
                  VGADefaultPalette[BGColor][1] << 8 |                // Green
                  VGADefaultPalette[BGColor][2];                // Blue

        for (I = 0;  I < CharHeightInPixels;  I++) {
            Data = FontPtr[I];
            Mask = 1 << (CharWidthInPixels-1);

            for (J = 0;  J < CharWidthInPixels;  J++) {
                //
                // Scan the Data bits from MSB to LSB and display FGColor
                //  if the bit is set or BGColor if the bit is cleared.
                //
                if ((Data & Mask) != 0) {
                        WRITE_REGISTER_ULONG((PULONG)(Destination + J*4), FGColor);
                } else {
                        WRITE_REGISTER_ULONG((PULONG)(Destination + J*4), BGColor);
                }

                Mask = Mask >> 1;
            }

            Destination += BytesPerScanLine;
        }

        break;

      default:
        break;

    }
}

#define	NROW	30
#define	NCOL	80
#define tgadisplaycursor()

void tgaputc(register int c)
{
  register int	i;
  register int	j;
  register PUCHAR	a1;
  register PUCHAR	a2;

  switch (c) {
  case '\b':
    if (FwColumn != 0)
      {
	--FwColumn;
	tgadisplaycursor();
      }
    break;

  case '\r':
    FwColumn=0;
    tgadisplaycursor();
    break;

    case '\n':
      if (FwRow < NROW-1)
	{
	  ++FwRow;
	}
      else
	{
	  a1 = VideoMemoryBase;
	  a2 = a1 + (BytesPerScanLine * CharHeightInPixels);
	  j = (NROW-1) * BytesPerScanLine * CharHeightInPixels;
	  for (i=0; i<j; i +=4)
	    {
	      WRITE_REGISTER_ULONG((PULONG)a1, READ_REGISTER_ULONG((PULONG)a2));
	      a1 += 4;
	      a2 += 4;
	    }
	  FwRow = NROW-1;
	  for (FwColumn=0; FwColumn<NCOL; ++FwColumn)
	    {
	      FwOutputCharacter ((UCHAR)' ');
	    }
	}
      FwColumn = 0;
      tgadisplaycursor();
      break;

  case '\t':
    do {
      FwOutputCharacter ((UCHAR) ' ');
    } while ((++FwColumn%8) != 0);
    break;

  default:
    if (isprint(c)) {
      if (FwColumn >= NCOL)
	tgaputc('\n');
      FwOutputCharacter ((UCHAR)c);
      if (FwColumn < NCOL-1)
	{
	  ++FwColumn;
	  tgadisplaycursor();
	}
      else
	{
	  tgadisplaycursor();
	  ++FwColumn;
	}
    }
  }
}


void tgaerasedisplay(void)
{
  for (FwColumn=0; FwColumn<NCOL; ++FwColumn)
    {
      for (FwRow=0; FwRow<NROW; ++FwRow)
	FwOutputCharacter (' ');
    }
  FwRow = 0;
  FwColumn = 0;
}

GRAPHICS_TYPE tgasetup(void)
{
  GRAPHICS_TYPE FbType;
  FbType = FwInitializeDigitalTGA(DigitalTGABase);
  tgaerasedisplay();
  return (FbType);
}
#endif /* NEEDTGA */
