/*
 *        Copyright (c) Digital Equipment Corporation, 1990
 *        All Rights Reserved.  Unpublished-rights reserved
 *        under the copyright laws of the United States.
 *
 *        The software contained on this media is proprietary to"
 *        and embodies confidential technology of Digital.  Possession,
 *        use, duplication or dissemination of the software and media
 *        is authorized only pursuant to a valid written license from
 *        Digital.
 *
 *        RESTRICTED RIGHTS LEGEND  Use, duplication, or disclosure"
 *        by the U.S. Government is subject to restrictions as set 
 *        forth in subparagraph (c)(1)(ii) of DFARS 252.227-7013, or 
 *        in FAR 52.227-19, as applicable.
 */



/*
 *++
 *  FACILITY:
 *
 *      Driver sfbp ASIC Routines
 *
 *  ABSTRACT:
 *      This provides common ASIC routines required for graphics.
 *
 *  AUTHORS:
 *
 *      James Peacock
 *
 *
 *  CREATION DATE:      03-25-91 
 *
 *  MODIFICATION HISTORY:
 *
 *
 *--
*/
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:tga_sfbp_include.h"
#include "cp$src:tga_sfbp_callbacks.h"
#include "cp$src:tga_sfbp_prom_defines.h"
#include "cp$src:tga_sfbp_def.h"
#include "cp$src:tga_sfbp_protos.h"
#include "cp$src:tga_sfbp_463.h"
#include "cp$src:tga_sfbp_485.h"
#include "cp$src:tga_sfbp_ext.h"
#include "cp$src:tga_sfbp_crb_def.h"
#include "cp$src:tga_sfbp_ctb_def.h"

/* These are constants used by the cfg_read/cfg_write code      */
#define   PCI_byte_mask 0
#define   PCI_word_mask 1
#define   PCI_long_mask 3

void     pci_outmem(unsigned __int64 address, unsigned int value, int length);
unsigned int pci_inmem(unsigned __int64 address, int length);

#define  TGA_DEBUG 0


/*+
* ===========================================================================
* = sfbp$$sfbp_stipple - Stipple Video Ram =
* ===========================================================================
*
* OVERVIEW:
*       Stipple Video RAM
* 
* FORM OF CALL:
*       sfbp$$sfbp_stipple (scia_ptr,data_ptr,stip,color,visual,z)
*
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*       stip            - stipple data structure
*       color           - color type
*       visual          - what visual for the stipple
*       z               - what depth 
*
* FUNCTIONAL DESCRIPTION
*       This driver call is used to stipple video ram. The console driver
*       and diagnostics invoke this function. The console driver always
*       does aligned block fills for performance.
*
	Stipple modes are functionally unchanged from the original SFB.
	In both stipple modes data written to the frame buffer is interpreted
	as a bit pattern, where 1's are expanded into foreground pixels and 0's
	are either expanded into background pixels (opaque stipple mode) or
	are no-op's (transparent stipple mode).  In opaque stipple mode an
	additional mask may be written in order to write fewer than 32 pixels.
	SFB+ stipple performance will be higher than the original SFB because
	the memory timing will be fixed at about 30ns rather than being tied
	to the TURBOchannel clock (40ns or 80ns, depending on the system's
	TURBOchannel clock speed).

	The stipple address for 8-bit visuals needs 4 pixel alignment.
	For an 8-bit packed visual, 4 pixels are 4 bytes.  For an 8-bit
	unpacked visual, 4 pixels are 16 bytes.  All other visuals (12-bit
	and 24-bit) must align the stipple address to 8 bytes.

	31                                                             0
	+--------------------------------------------------------------+
	|                         Stipple Pattern                      |
	+--------------------------------------------------------------+


Foreground Register
-------------------

	In stipple and line modes ones in the data pattern are expanded into
	foreground pixels.  The Foreground register defines the data pattern or
	color of a Foreground pixel.  In 8 plane mode, the foreground color
	must be replicated across all 32 bits of the Foreground Register.  In
	12-plane mode, the foreground color needs to be replicated across both
	sets of RGB values (see the picture of pixel formats described in
	simple mode).

	31                                                             0
	+--------------------------------------------------------------+
	|                         Foreground                           |
	+--------------------------------------------------------------+

	Value at init: 0

	Side effects: Writes to the Red or Blue Increment Register will
		      alter the contents of the Foreground register.

Background Register
-------------------

	In opaque stipple and opaque line modes zeros in the data pattern are
	expanded into background pixels.  The Background register defines the
	data pattern or color of a background pixel. In 8 plane mode, the
	background color must be replicated across all 32 bits of the
	Background register.  In 12-plane mode, the background color needs to
	be replicated across both sets of RGB values (see the picture of pixel
	formats described in simple mode).


	31                                                             0
	+--------------------------------------------------------------+
	|                         Background                           |
	+--------------------------------------------------------------+

	Value at init: 0

	Side effects: Writes to the Green or Blue Increment Register will
		      alter the contents of the Background register.

Block stipple mode
------------------

	Block stipple mode is similar to transparent stipple mode. In Block
	Stipple mode, the data specifies a stipple pattern indicating which
	of the 32 pixels is to be written.  Pixels corresponding to bits set
	in the block stipple pattern are written with the Block mode color,
	pixels corresponding to bits not set in the block stipple pattern are
	unchanged.  Because Block Stipple Mode uses a special block write
	memory cycle, raster-ops cannot be supported.  Block stipples will
	operate as if the raster-op were set to GXcopy no matter what the
	raster-op is actually programmed to.

	Block stipple allows alignment to 1 pixel.  Since block stipple uses
	a full 32-bit data word, there is no room in the data word to put the
	bottom 2 bits of a byte address.  Thus, packed 8-bit visuals can align
	to 1 pixel only by using the Address register; when writing to the
	frame buffer they must align to 4 bytes.

	When painting on an 8-bit system, or when painting 8-plane unpacked on
	a 32-bit system, a block stipple to an address that is aligned to 32
	PIXELS (not bytes!) will paint up to 32 pixels in 60 nsec.  If the
	address is not aligned to 32 pixels, the hardware will automatically
	break the request into two operations, each aligned to 32 pixels, and
	will require 120 nsec to paint the pixels.

	When painting to 24-plane or 12-plane visuals, block stipple to an
	address that is aligned to 32 pixels will paint 32 pixels in 240
	nsec.  If the address is not aligned to 32 pixels, the hardware
	requires 480 nsec to paint the pixels.

	Block stipple simply doesn't work when painting to an 8-plane packed
	visual on a 32-bit system.

	In order to save gates, software must rotate the stipple
	pattern so that it is 4-pixel aligned even though the address
	only requires 1-pixel alignment.  A 32-bit barrel-shifter on the
	chip would have been too gate intensive.

	31                                                             0
	+--------------------------------------------------------------+
        |            Block Stipple Pattern (possibly rotated)          |
	+--------------------------------------------------------------+


(Software notes)

	Every place the current sfb code uses transparent stipple mode, a
	destination-independent raster op (GXclear, GXcopy, GXcopyInverted, or
	GXset), and is painting more than 1 bit at a time, the sfb+ code should
	probably use block stipple mode.  (If painting a single bit at a time,
	there's no advantage to special-casing the destination-independent
	raster ops in order to use block stipple mode.)

	Note that block stipple and block fill modes use the Color registers,
	not the Foreground register.  You have to load up 8 pixels.

	On 8-bit systems, data can't be shipped to the sfb+ faster than 32
	bits every 120 nsec on the TURBOchannel, there's no reason to align
	to 32-pixel boundaries.  Aligning to 4-byte boundaries is sufficient,
	and may even be faster than aligning to 32-pixel boundaries.  With
	4-byte alignment, chances are increased that you will need only one
	32-bit word to describe a narrow area.

	The PCI bus can transfer a block stipple pattern in 60 nsec.  In this
	case, spans that cross at least 2 32-pixel boundaries will benefit
	from aligning to 32-pixel addresses.  But remember, if you have a
	screen or pixmap width mod 32 != 0 (in order for lines to go fast),
	then you must recompute alignment data on each scanline.

	On 32-bit systems, aligning to 32 pixels is almost mandatory.  This
	means that you probably want drawable widths to be a multiple of 128
	bytes.

	If you are painting large enough areas using opaque stipple and a
	destination-independent raster op, AND you are being hampered by memory
	bandwidth, you'll get better performance by first using Block fill mode
	(described immediately below) to paint the background color, then using
	block stipple mode to paint the foreground bits on top of it.  I
	suspect that CopyPlane will improve this way.


Fill modes
----------

	In Transparent and Opaque Fill modes, writes to the Frame Buffer
	define the starting address and the data is interpreted as a
	pixel count. Up to 2K pixels can be filled in a single operation.
	The Data register defines a 32 pixel pattern for the fill.  The
	address can be aligned to a 1-pixel boundary (1 byte on 8-bit
	systems, 4 bytes on 32-bit systems).  The pattern must be rotated
	to a 4-pixel alignment.

	For 8 plane frame buffers the low two bits of the byte/pixel address
	are extracted from bits <17:16> of the data, unless the address
	is written into the Address register and the data is written into
	the Continue register.

	Pattern bits that are 1's are expanded into foreground pixels,
	0's are either expanded into background pixels (opaque fill mode)
	or are no-op's (transparent fill and block fill modes).

	Raster-ops are supported in normal fill modes.

	31                        18  16 15     11 10                  0
	+---------------------------+---+---------+--------------------+
	|           ignore          |adr|  ignore |    Pixel count-1   |
	+---------------------------+---+---------+--------------------+

(Software notes)

	Software is responsible for limiting the pixel count in order to
	avoid bus timeouts.  This maximum count is affected by the number of
	bits/pixel and whether the raster op directly writes or does a
	read-modify-write.

	Transparent and Opaque Fill can be used to provide all of the
	functions described below for Block Fill, but can deal with
	destination-dependent raster-ops.  In addition, Opaque fill
	should probably be used for small areas of opaque stipple; the
	exact trade-off between this and using Block Fill/Block Stipple will
	be determined by performance tuning.

	In most cases, each routine that wants to use one of the Fill
	modes can be compiled once or twice, and make a few on-the-fly
	decisions at run time.  At the top of each routine, you have to
	determine which of the three Fill modes to use, and so how to load up
	the foreground/background/color registers.  (Some routines, like
	rectangles, may choose between Opaque Fill and Block Fill depending
	upon the width of the rectangle.)  You also need to initialize a
	variable containing the maximum pixel count that can be used.  After
	that, everything should be the same.


Block Fill mode
---------------

	Block Fill Mode uses a special block write memory cycle with
	performance four times the normal fill modes, but opaque modes and
	raster-ops cannot be supported.  Block fills will operate as if the
	raster-op were set to GXcopy no matter what the raster-op is actually
	programmed to.  Rather than using the Foreground register, 1 bits in
	the pattern are expanded into the corresponding Block color pixel; 0
	bits in the pattern leave the destination unchanged.

	Block Fill Mode paints 32 pixels/60 nsec in 8-bit systems.  On 32-bit
	systems, if the Data Register is all 1's and the visual type is
	24-plane or 12-plane, Block Fill Mode paints 32 pixels/60 nsec on the
	interior, but takes 4 60 nsec cycles to paint unaligned left and
	right edges.  If the data register is not all 1's, Block Fill Mode
	paints 32 pixels/240 nsec.

	Block Fill Mode does not work for packed 8-plane visuals on 32-bit
	systems.

(Software notes)

	For destination-independent raster ops, block fill mode is useful not
	just for solid fills, but for some transparent stipple fills, and even
	for some opaque stipple and tile fills as well.

	Solid fills are simple.  Write the foreground pixel into the Color
	registers, write all 1's into the Data register, then start writing
	block fill data words into the frame buffer.

	Transparent stipple fills that are powers of 2 <= 32 bits are almost as
	simple.  Assume that the pattern has been expanded to be 32 bits wide,
	if necessary.  For each scanline, you have to write the stipple pattern
	into the Data register.	Then just write a block fill data word to the
	frame buffer to paint the entire scanline.

	Opaque stipple fills can use block fill to paint a solid background,
	then proceed like transparent stipples.  If the area to paint is large
	enough (where ``large enough'' will have to be determined once we get
	the real chips back), and the stipple pattern is a power of 2 <= 8 bits
	it will be better to use code similar to the existing sfb 4-byte tiling
	code.  The basic idea is to rotate the 8 bits appropriately to meet the
	alignment constraints (in this case 8 pixels), expand the 8 bits
	into 8 pixels, load these pixels into the Color registers, and then
	write a block fill data word to the frame buffer to paint the scan
	line.  (The Data register should be loaded with 1's at the top of the
	routine.)

	Tiles that are powers of 2 <= 8 pixels are handled much the same way,
	except you don't need to expand the tile, you just need to rotate each
	scanline of the tile appropriately, write the pattern to the Color
	registers, then write a word to the frame buffer to paint the scanline.

	Block fill mode is also used to simultaneously swap buffers in 12/12
	double buffering or 8/8/8 triple buffering, and clear the new back
	buffer to the background color.  The control bits are set so as to
	effect a buffer swap, the background color is set in the appropriate
	pixel bits, and then the planemask is initialized to allow the write to
	affect just the control bits and the buffer being cleared.

	Finally, block fill mode should be used for clearing the Z
	buffer/stencil bits.

* 
*
* ALGORITHM:
*       Calculate the maximum bytes and stipple increment based on visual
*       Calculate the end address for stipple
*
*       IF transparent block fill mode
*               THEN 
*                       load block color registers
*                       set data register to 0xffffffff
*                       set pixel mask register to 0xffffffff
*                       set mode register to transparent block fill (visual set later)
*                       calculate the block fill count = stipple size / fill byte max 
*                       calculate the repeat ops = stipple size / max stipple bytes
*                       calculate the residual bytes = stipple size MOD fill byte max
*
*       ELSE IF transparent stipple mode
*               THEN
*                       set foreground to color->fg_value
*                       set mode register to transparent stipple
*                       set pixel mask to 0xffffffff
*       
*       ELSE IF Opaque Stipple Mode 
*               THEN
*                       set background to color->bg_value
*                       set mode register to opaque stipple
*                       set pixel mask to 0xffffffff
*
*       ELSE IF Transparent Block Stipple
*               THEN
*                       load block color registers
*                       set pixel mask register to 0xffffffff
*                       set mode register to transparent block stipple  (visual set later)
*                       calculate the repeat ops = stipple size / max stipple bytes
*                       calculate the residual bytes = stipple size MOD max bytes
*
*       ELSE IF transparent fill ( repeated transparent stipple)
*               THEN
*                       set pixel mask register to 0xffffffff
*                       set the foreground register to color->fg_value
*                       set mode register to transparent fill (visual set later)
*                       calculate the block fill count = stipple size / fill byte max 
*                       calculate the repeat ops = stipple size / max stipple bytes
*                       calculate the residual bytes = stipple size MOD fill byte max
*
*       ELSE IF opaque fill ( repeated opaque stipple)
*               THEN
*                       set pixel mask register to 0xffffffff
*                       set the background register to color->bg_value
*                       set mode register to transparent fill (visual set later)
*                       calculate the block fill count = stipple size / fill byte max 
*                       calculate the repeat ops = stipple size / max stipple bytes
*                       calculate the residual bytes = stipple size MOD fill byte max
*
*
*       Load the Source and Destination Visuals 
*       IF residual bytes 
*               THEN 
*                       IF transparent block fill or transparent fill 
*                               THEN pmask = M1
*                       ELSE if opaque fill THEN pmask = ~M1
*                       ELSE pmask =residual bytes ( 1 in each bit)
*                      
*                       IF opaque stipple THNE pmask = ~ pmask
*                       Write address register with stipple address (vram)
*                       Write start register with pmask value (mask of pixels to stipple)       
*                       Increment video ram address by residual bytes stippled
*
*       DO
*               Write stipple mask (0xffffffff) to video ram address
*               Increment Video Address by stipple increment
*       WHILE video address < end address
*
*       Flush the Write Buffer
*       Write Mode Register with Default Mode Value
*       Write Raster Op Register with Default rop value
*       Wait for command status register bit to clear.
*
*                                      
--*/



int     sfbp$$sfbp_stipple (SCIA *scia_ptr,SFBP * data_ptr, STIP *stip , COLOR *color, VISUAL *visual, Z *z)

{
register int           i,j,k,increment,s=1;
register int           unaligned_count,data,pmask;
register int           stip_inc,bytemask,block_fill_cnt=1,max_bytes,ops,res_bytes;
register char          *vram;
volatile register      int *vsrc;
register int  end_address,fill_max;

 /* This calculation is used for writing to vram */
 vram            = (char *)sfbp_ioseg_ptr->sfbp$a_vram_mem_ptr+stip->src_address;
 vsrc            = (int  *)vram;
 vram            = (char *)stip->src_address;
 bytemask        = (int   )vram%4;

#if TGA_DEBUG
 pprintf ("stipple vsrc 0x%x 0x%x bytes\n",vsrc,stip->size);
#endif
 switch (visual->depth)
 {
 case packed_8_packed:                        
 case packed_8_unpacked:                      
 case unpacked_8_packed:                      
 case unpacked_8_unpacked:                    
        max_bytes = 32;
        fill_max  = 2048;
        stip_inc  = 8;
        break;
                
 case source_12_low_dest:                     
 case source_12_hi_dest:                      
        max_bytes = 32;
        fill_max  = 2048;
        stip_inc  = 32;
        break;
                
 case source_24_dest:                         
        max_bytes = 128;
        fill_max  = 2048;                                
        stip_inc  = 32;
        break;
 }
end_address = (int)vsrc+stip->size;

if (stip->stipple == transparent_block_fill)                                                                   
  {
#if TGA_DEBUG
  pprintf ("stipple block fill \n");
#endif
  /* Console Driver Only Does Transparent Block Fills */
  sfbp$$load_block_color (scia_ptr,data_ptr,color->fg_value);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,sfbp$k_transparent_block_fill,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_data_register,M1,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_mask,M1,LW);
  block_fill_cnt= stip->size/fill_max;
  if (block_fill_cnt)data=fill_max-1;
  else data = (stip->size%fill_max)-1;
  max_bytes = fill_max;
  ops       = stip->size/max_bytes;
  res_bytes = stip->size%fill_max;
#if TGA_DEBUG
  pprintf ("stipple block fill ops 0x%x res_bytes 0x%x\n",
	ops,res_bytes);
#endif
  }

#if FULL_TGA
else if (stip->stipple == transparent_stipple)                                                                   
  {
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_foreground,color->fg_value,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,sfbp$k_transparent_stipple,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_mask,M1,LW);
  data          = M1;
  ops           = stip->size/max_bytes;
  res_bytes     = stip->size%max_bytes;
  }

else if (stip->stipple == opaque_stipple)                                                                   
  {
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_background,color->bg_value,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,sfbp$k_opaque_stipple,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_mask,M1,LW);
  data          = 0x0;
  ops           = stip->size/max_bytes;
  res_bytes     = stip->size%max_bytes;
  }

 else if (stip->stipple == transparent_block_stipple)                                                                   
  {
  sfbp$$load_block_color (scia_ptr,data_ptr,color->fg_value);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,sfbp$k_transparent_block_stipple,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_mask,M1,LW);
  data      = M1;
  ops       = stip->size/max_bytes;
  res_bytes = stip->size%max_bytes;
  }

 else if (stip->stipple == transparent_fill )                                                                   
  {
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_foreground,color->fg_value,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,sfbp$k_transparent_fill,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_mask,M1,LW);
  block_fill_cnt= stip->size/fill_max;
  if (block_fill_cnt)data=fill_max-1;
  else data = (stip->size%fill_max)-1;
  max_bytes = fill_max;
  ops       = stip->size/max_bytes;
  res_bytes = stip->size%fill_max;
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_data_register,M1,LW);
  }
 else if (stip->stipple == opaque_fill )                                                                   
  {
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_background,color->fg_value,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,sfbp$k_opaque_fill,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_mask,M1,LW);
  block_fill_cnt= stip->size/fill_max;
  if (block_fill_cnt)data=fill_max-1;
  else data = (stip->size%fill_max)-1;
  max_bytes = fill_max;
  ops       = stip->size/max_bytes;
  res_bytes = stip->size%fill_max;
  data      = ~data;
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_data_register,0,LW);
  }
#endif
 sfbp$$load_visual (scia_ptr,data_ptr, visual , z ) ;

 /* do the residual data first  */
 if (res_bytes)
        {
        if (stip->stipple == transparent_block_fill ||
            stip->stipple == transparent_fill )pmask=data;
        else if (stip->stipple == opaque_fill )pmask=~data;                                                                   

        else for (i=0,pmask=k=1;i<res_bytes;i++,k<<=1)pmask |= k; 

        if (stip->stipple == opaque_stipple)pmask=~(pmask);                                                                   

        /* Since we have residual...use the address/start register      */
        /* Which is equivalent to writing to frame buffer               */
        (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_line_address,vram,LW);
        (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_start,pmask,LW);
        vram+=res_bytes;
        }

#if TGA_DEBUG
   pprintf ("stipple block fill start vsrc 0x%x end 0x%x \n",vsrc,end_address);
#endif
   do
        {
        if(bytemask) 
                {
                /* Odd Aligned...so use the line address register              */
                (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_line_address,vram,LW);
                (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_start,data,LW);
                vram += max_bytes;
                vsrc += (max_bytes/4); 
                }
        else
                {
                /* Otherwise longword aligned and do a fb write                 */
                pci_outmem (vsrc,data,PCI_long_mask);
                vsrc  +=stip_inc;
                }
        }while ((int)vsrc<end_address);
                   
 wbflush ();
 
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 
#if TGA_DEBUG
   pprintf ("stipple operation done\n");
#endif

return (SS$K_SUCCESS);
}

