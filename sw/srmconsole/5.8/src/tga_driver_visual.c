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

int	        sfbp$$sfbp_write_reg  (SCIA *scia_ptr,int address,int data,int mask );
int             sfbp$$sfbp_cint_setup (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z);
int             sfbp$$sfbp_cint_setup (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z);

#define PIXEL_MASK         0xFFFF
#define LINE_LENGTH        16
#define COLOR_VALUE_MASK   0xF80FFFFF           /* 20 bits color        */
#define COLOR_INCR_MASK    0x000FFFFF           /* 8 + 12 bits fraction */


/*+
* ===========================================================================
* = sfbp$$load_block_color - Load block color =
* ===========================================================================
*
* OVERVIEW:
*       Load the block color registers 
* 
* FORM OF CALL:
*       sfbp$$load_block_color (scia_ptr,data_ptr,value)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*       value           - value to load into block color
*
* FUNCTIONAL DESCRIPTION:
*       Load the block color registers. The diagnostic loads the same value to
*       all registers. Value must be a longword value populated across all 32
*       bits. Block color registers are actually holding registers within 
*       the context of video ram and are used in block fill and block stipple
*       operations. The 8 plane system requires only 2 register be written,
*       while the 32 plane requires 8.
*
--*/
int     sfbp$$load_block_color (SCIA *scia_ptr,SFBP *data_ptr,int value )

{
int      i,count,s=1;

count           = (data_ptr->sfbp$l_planes==8) ? 2 : 8;

for (i=0;i<count;i++)
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_block_color_0+(i<<2),value,LW);
  
return (s);
}



/*+
* ===========================================================================
* = sfbp$$load_visual - Load visual =
* ===========================================================================
*
* OVERVIEW:
*       Load the visual type 
* 
* FORM OF CALL:
*       sfbp$$load_visual (scia_ptr,data_ptr,visual,z)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*       visual          - visual type to load
*       z               - z type
*
* FUNCTIONAL DESCRIPTION:
*       This will load the source and destination visuals. The console driver
*       always uses the default mode and visuals. The diagnostics use something
*       as defined by the diagnostics. The source visual gets loaded in the
*       mode register and the destination visual gets loaded in the raster op
*       register. The stencil mode register gets loaded with the current stencil.
*
--*/
int     sfbp$$load_visual (SCIA *scia_ptr,SFBP *data_ptr,VISUAL *visual, Z *z)

{
int      i,count,s=1;
union    control_reg mode;
union    boolean_reg *bool;
int      bool_value=0;
int      SourceVisual,DestVisual;

 mode.sfbp$l_whole = 0;
 mode.sfbp$l_whole = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);

 switch (visual->depth)
  {
  case packed_8_packed:       
        SourceVisual = SourceVisualPacked;                 
        DestVisual   = DestVisualPacked;
        break;
  case source_24_dest:        
        SourceVisual = SourceVisualTrue;                                  
        DestVisual   = DestVisualTrue;
        break;
  case packed_8_unpacked:     
        SourceVisual = SourceVisualPacked;                 
        DestVisual   = DestVisualUnPacked;
        break;
  case unpacked_8_packed:   
        SourceVisual = SourceVisualUnPacked;                                  
        DestVisual   = DestVisualPacked;
        break;
  case unpacked_8_unpacked:   
        SourceVisual = SourceVisualUnPacked;                                  
        DestVisual   = DestVisualUnPacked;
        break;
  case source_12_low_dest:    
        SourceVisual = SourceVisualLow;                                   
        DestVisual   = DestVisual12bit;
        break;
  case source_12_hi_dest:     
        SourceVisual = SourceVisualHi;
        DestVisual   = DestVisual12bit;
        break;
  }
  
 /* Set up the source Visual First      */
 mode.sfbp$r_control_bits.sfbp$v_con_depth = SourceVisual;
 mode.sfbp$r_control_bits.sfbp$v_con_rotate= visual->rotation>>2;
 mode.sfbp$r_control_bits.sfbp$v_con_zsize = 0;
 mode.sfbp$r_control_bits.sfbp$v_con_cap   = visual->capend;
 mode.sfbp$r_control_bits.sfbp$v_linet     = visual->line_type;

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,mode.sfbp$l_whole,LW);

 /* Raster Op Register Has the Destination Visual       */
 /* Nice Place for it I guess after we move it          */

 bool = (union boolean_reg *)&bool_value;
 bool->sfbp$r_boolean_bits.sfbp$v_bool_code   = visual->boolean;
 bool->sfbp$r_boolean_bits.sfbp$v_bool_rotate = visual->rotation&3;
 bool->sfbp$r_boolean_bits.sfbp$v_bool_visual = DestVisual;

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,bool_value,LW);

 sfbp$$load_stype  (scia_ptr,data_ptr, visual , z ) ;

return (s);
}


#if FULL_TGA
/*+
* ===========================================================================
* = sfbp$$mode_setup - Setup the mode register =
* ===========================================================================
*
* OVERVIEW:
*       Make A line 
* 
* FORM OF CALL:
*       sfbp$$mode_setup (scia_ptr,data_ptr,line,color,visual,z)
*
* RETURNS:
*       1 for success else 0 for failure
*
* ARGUMENTS:
*       scia_ptr - shared driver pointer
*       data_ptr - driver data area pointer
*       line     - line descriptor
*       color    - what color
*       visual   - what visual 
*       z        - z type
*
* FUNCTIONAL DESCRIPTION:
*       Set up mode register based on the request. This function looks at the
*       current mode and sets up the mode register. This routine has to 
*       set up the data register also for the capend functionality.
*
*
	The Mode register controls the action of the SFB+ asic when it
	receives a write to the frame buffer. The actions taken and the
	interpretation of the data are described elsewhere. Mode is defined
	as follows:

        3                              1 1 1 1 1 1 1
	1                              6 5 4 3 2 1 0   8 7             0
        +-------------------------------+-+-+-+---+-----+--------------+
        |           MUST BE ZERO        |C|Z|L|rot|visul|     Mode     |
        +-------------------------------+-+-+-+---+-----+--------------+

	Mode : 00000000	simple mode
	       00010000	z-buffered simple mode.

	       00000001	opaque stipple mode
	       00100001 opaque fill    mode
	       00000101 transparent stipple mode
	       00100101 transparent fill    mode
	       00001101 transparent block stipple mode
	       00101101 transparent block fill    mode

	       00000010	opaque line mode
	       00000110	transparent line mode
	       00001110	color interpolated transparent non-dithered line mode
	       00101110	color interpolated transparent dithered     line mode
	       01001110	color interpolated trans, sequential interp line mode
	       00010010	z-buffered, opaque, line mode
	       00010110	z-buffered, trans,  line mode
	       00011010	z-buffered, opaque, color interp non-dithered line mode
	       01011010	z-buffered, opaque, sequential interp non-dithered
	       00111010	z-buffered, opaque, color interp dithered     line mode
	       00011110	z-buffered, trans,  color interp non-dithered line mode
	       01011110	z-buffered, trans,  sequential interp non-dithered
	       00111110	z-buffered, trans,  color interp dithered     line mode

	       00000111	copy mode
	       00010111	dma-read  copy mode, non-dithered
	       00110111	dma-read  copy mode, dithered
	       00011111	dma-write copy mode

	The Visual depth field of the mode register defines the type of source
	visual on 32 plane systems.  Depth must be zero on 8 plane systems.

	Visual:  000   8  bit   packed source
		 001   8  bit unpacked source
		 010   12 bit source (low)
		 110   12 bit source (high)
		 011   24 bit source

	The Rotate field defines the byte position within the word of the
	source visual.  This is only useful in conjunction with 8 bit unpacked
	visuals on 32 plane systems and is ignored otherwise.

	Rotate:  00    rotate source 0 bytes
		 01    rotate source 1 byte
		 10    rotate source 2 bytes
		 11    rotate source 3 bytes

	The NT Lines bit when set will enable setting of the
	initial Bresenham error term so as to paint lines in the
	manner specified by the Microsoft Windows GDI.  When not set,
	lines are painted the way DEC's X servers have historically
	painted them.

	L:	 0	historical X server lines
		 1	NT Windows compliant lines

	The z16 bit of the mode register controls the size of a Z value.
	When set, the a Z buffer value is 16 bits where the low byte of
	the 24 bit Z value is neither compared or stored. When the z16
	bit is not set, Z buffer values are a full 24 bits.
	
	z16:	    1	Z values are 16 bits
	            0   Z values are 24 bits

	The capEnds bit adjusts the line length during Bresenham setup.
	See description of Bresenham parameter derivation in the section
	on "Slope Registers".

	CapEnds:    1   cap lines
		    0   do not cap lines.

	The "MUST BE ZERO" fields are reserved for future use.

	When the mode register is read, the state of the Pixel Mask Register
	is returned in mode<19>.

	Value at init: 0

(Software notes)

	In 8-plane systems, the sfb+ uses a Depth of 000: all sources and
	destinations are packed 8-bit pixels.

	In 32-bit system, the sfb+ can be set to any of the modes specified.
	The sfb+ can paint to different depth visuals: 8, 12, and 24 bits.  The
	color interpolated line logic, for example, needs to know how it should
	compress the extended RGB values down to the visual depth, and how it
	should replicate this information across a 32-bit pixel.

	Further, in order to avoid a 4X slowdown (compared to an 8-bit system)
	when painting to 8-bit visuals, the sfb+ can paint to both packed and
	unpacked 8-bit visuals.  Packed 8-bit visuals are used for off-screen
	pixmaps, and are stored 4 8-bit pixels per 32-bit word.  Unpacked 8-bit
	visuals are used for on-screen windows, and are stored one 8-bit pixel
	per 32-bit word.  The sfb+ chip includes logic to paint 8 unpacked
	8-bit pixels at a time, by writing one byte to each of 8 different
	32-bit words.  When performing copies, the sfb+ also includes logic to
	read eight 8-bit pixels at a time, by reading one byte from each of
	eight different pixels.  Thus, with the proper selection of source and
	destination depths, it's possible not only to deal with painting to
	packed and unpacked visuals, but to copy between any combination as
	well.

	Due to deficiencies in the Brooktree 463, we can't deal with 12-bit
	packed and unpacked visuals.  So off-screen 12-bit pixmaps look just
	like on-screen windows: one pixel per 32-bit word.  In order to copy
	pixmaps to either set of RGB fields on the screen, and vice-versa, the
	sfb+ hardware can replicate either the high (buffer 1) or low (buffer
	0) bits into the other 12 bits.

	The main-memory format for 12-bit visuals will look like:

	31    28 27   24 23   20 19   16 15   12 11    8 7     4 3     0
	+-------+-------+-------+-------+-------+-------+-------+-------+
	|  0000 |  0000 |  Blue |  0000 | Green |  0000 |  Red  |  0000 |
	+-------+-------+-------+-------+-------+-------+-------+-------+

	When using DMA to copy a drawable to main memory, the sfb+ hardware
	will put 0000 into the appropriate fields.

	If you want to copy all 32 bits in each pixel verbatim (for backing
	store, say), use a 24-bit source with a 24-bit destination.  You might
	use this mode even for 12-bit double-buffered images, and so save both
	the front and back buffers simultaneously.

	In order to allow double and triple buffering of 8-bit images, the sfb+
	allows the specification of source and destination rotation.  Without
	going into detail on how words are physically laid out in memory,
	think of the source and destination rotation fields as specifying which
	byte in an unpacked 8-bit window that you're interested in.  00 means
	byte 0, 01 means byte 1, etc.

	Only the source depth and rotation are stored in the mode register, see
	the Raster Op Register for a description of the fields defining the
	destination depth and rotation.
*
*
--*/
int     sfbp$$mode_setup (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z)

{
register int      i,ops,res_bytes,data;

 ops         = line->len/LINE_LENGTH;
 res_bytes   = line->len%LINE_LENGTH;
 data        = PIXEL_MASK;

 if (res_bytes)for (i=0,data=0;i<res_bytes+visual->capend;i++)data |= (1<<i);

 switch (line->line)
  {
  case cint_nd_line_mode:
  case cint_nd_trans_line_mode_red:
  case cint_nd_opaque_line_mode_red:
  case cint_d_line_mode:
        sfbp$$sfbp_cint_setup  (scia_ptr,data_ptr,line,color,visual,z);
        break;

  case z_cint_trans_nd_line_mode:
  case z_cint_trans_d_line_mode:
  case z_cint_opaq_nd_line_mode:
  case z_cint_opaq_d_line_mode:
  case z_cint_trans_nd_line_mode_red:
  case z_cint_opaq_nd_line_mode_red:
        sfbp$$sfbp_cint_setup  (scia_ptr,data_ptr,line,color,visual,z);
        sfbp$$sfbp_zbuff_setup (scia_ptr,data_ptr,line,color,visual,z);
        break;
        
  case z_transparent_line_mode:
  case z_opaque_line_mode:
        sfbp$$sfbp_zbuff_setup (scia_ptr,data_ptr,line,color,visual,z);
        break;
  };

if (!line->polyline)
 {
 if (line->line == transparent_line_mode )
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,
        sfbp$k_transparent_line,LW);

 else if (line->line == opaque_line_mode )
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,
        sfbp$k_opaque_line,LW);

 else if (line->line == cint_nd_line_mode )
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,
        sfbp$k_cint_trans_non_dither_line, LW);

 else if (line->line == cint_nd_trans_line_mode_red )
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,
        sfbp$k_cint_trans_non_dither_line_red, LW);

 else if (line->line == cint_d_line_mode )
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,
        sfbp$k_cint_trans_dither_line, LW);

 else if (line->line == z_cint_trans_nd_line_mode )
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,
        sfbp$k_zbuff_cint_trans_non_dither_line , LW);

 else if (line->line == z_cint_trans_d_line_mode )
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,
        sfbp$k_zbuff_cint_trans_dither_line, LW);

 else if (line->line == z_cint_opaq_nd_line_mode )
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,
        sfbp$k_zbuff_cint_opaq_non_dither_line , LW);

 else if (line->line == z_cint_opaq_d_line_mode )
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,
        sfbp$k_zbuff_cint_opaq_dither_line, LW);

 else if (line->line == z_transparent_line_mode )
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,
        sfbp$k_zbuff_transparent_line,LW);

 else if (line->line == z_opaque_line_mode )
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,
        sfbp$k_zbuff_opaque_line, LW);

 else if (line->line == z_cint_trans_nd_line_mode_red )
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,
        sfbp$k_zbuff_cint_trans_non_dither_line_red, LW);

 else if (line->line == z_cint_opaq_nd_line_mode_red )
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,
        sfbp$k_zbuff_cint_opaque_non_dither_line_red, LW);

 if (line->line == z_opaque_line_mode || 
     line->line == z_cint_opaq_d_line_mode ||
     line->line == z_cint_opaq_nd_line_mode ||
     line->line == z_cint_opaq_nd_line_mode_red ||
     line->line == opaque_line_mode ){data = (~data)&PIXEL_MASK;}

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_data_register, data ,LW );
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,line->boolean_op,LW);

 if (data_ptr->sfbp$l_print)
  {
  printf ("Mode Register 0x%x \n",
	 (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode));
  printf ("Rop  Register 0x%x \n",
 	 (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_boolean_op));
  printf ("Data Register 0x%x \n",
  	 (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_data_register));
  }

 }


return  (SS$K_SUCCESS);
}




/*+
* ===========================================================================
* = sfbp$$sfbp_cint_setup - Color Interpolation Setup =
* ===========================================================================
*
* OVERVIEW:
*       Set up the color interpolation for drawing color interpolated lines
* 
* FORM OF CALL:
*       sfbp$$sfbp_cint_setup (scia_ptr,data_ptr,line,color,visual,z)
*
* RETURNS:
*       1 for success else 0 for failure
*
* ARGUMENTS:
*       scia_ptr - shared driver pointer
*       data_ptr - driver data area pointer
*       line     - line descriptor
*       color    - what color
*       visual   - what visual 
*       z        - z buffer if we need it 
*
* FUNCTIONAL DESCRIPTION:
*       Set up the color interpolation registers using the values my diagnostics have
*       already set up.
*
*
*Red Value Register
------------------

	The Red Value Register contains the Red color value of the first
	pixel in a line or span.  The Red value is a 20 bit value
	containing 12 bits of fractional precision. The starting dither
	matrix row pointer is stored in the high bits of the register.


	31      27 26         20 19           12 11                    0
	+---------+-------------+---------------+----------------------+
	|DitherRow|    ignore   |   Red Value   |  Red Value Fraction  |
	+---------+-------------+---------------+----------------------+

	When read, the DitherRow value is not returned. The DitherRow value
	can be read from the SpanWidth Register.

	Value at init: 0

(Software notes)

	DitherRow and DitherCol should be computed by taking the bottom 5 bits
	of the y and x pixel offsets of the beginning of the line or span
	within the drawable.


Green Value Register
--------------------

	The Green Value Register contains the Green color value of the first
	pixel in a line or span.  The Green value is a 20 bit value
	containing 12 bits of fractional precision. The starting dither
	matrix column pointer is stored in the high bits of the register.

	31      27 26         20 19           12 11                    0
	+---------+-------------+---------------+----------------------+
	|DitherCol|    ignore   |  Green Value  | Green Value Fraction |
	+---------+-------------+---------------+----------------------+

	When read, the DitherCol value is not returned. The DitherCol value
	can be read from the SpanWidth Register.

	Value at init: 0


Blue Value Register
-------------------

	The Blue Value Register contains the Blue color value of the first
	pixel in a line or span.  The Blue value is a 20 bit value
	containing 12 bits of fractional precision.

        31                    20 19           12 11                    0
	+-----------------------+---------------+----------------------+
	|        ignore         |  Blue Value   | Blue Value Fraction  |
	+-----------------------+---------------+----------------------+

	Value at init: 0
Red Increment Register
----------------------

	The Red Increment register contains the incremental red value to
	be added to each consecutive pixel.  The Red Increment is a 20
	bit value containing 12 bits of fractional precision.

        31                    20 19           12 11                    0
	+-----------------------+---------------+----------------------+
	|        ignore         | Red Increment |Red Increment Fraction|
	+-----------------------+---------------+----------------------+

	Value at init: 0

	Side Effects:  Writes to the Foreground register will alter the
	               contents of the Red Increment register.

Green Increment Register
------------------------

	The Green Increment register contains the incremental Green
	value to be added to each consecutive pixel.  The Green
	Increment is a 20 bit value containing 12 bits of fractional
	precision.

        31                    20 19           12 11                    0
	+-----------------------+---------------+----------------------+
	|        ignore         |Green Increment|Green Incrmnt Fraction|
	+-----------------------+---------------+----------------------+

	Value at init: 0

	Side Effects:  Writes to the Background register will alter the
	               contents of the Green Increment register.

Blue Increment Register
-----------------------

	The Blue Increment register contains the incremental Blue value
	to be added to each consecutive pixel.  The Blue Increment is a
	20 bit value containing 12 bits of fractional precision.

        31                    20 19           12 11                    0
	+-----------------------+---------------+----------------------+
	|        ignore         | Blue Increment|Blue Incrment Fraction|
	+-----------------------+---------------+----------------------+

	Value at init: 0

	Side Effects:  Writes to the Foreground or Background register will
	               alter the contents of the Blue Increment register.

*
--*/
int     sfbp$$sfbp_cint_setup (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z)

{
register  int         s=1;

/* If no color value change     */
/* Then do not load values      */

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_red_value,color->red_value &COLOR_VALUE_MASK,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_green_value,color->green_value &COLOR_VALUE_MASK,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_blue_value,color->blue_value &COLOR_VALUE_MASK,LW);
 
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_red_increment,color->red_increment &COLOR_INCR_MASK,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_green_increment,color->green_increment &COLOR_INCR_MASK,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_blue_increment,color->blue_increment &COLOR_INCR_MASK,LW);
  
return (1);

}




/*+
* ===========================================================================
* = sfbp$$sfbp_zbuff_setup - Zbuffer Setup =
* ===========================================================================
*
* OVERVIEW:
*       Set up the zbuffer logic for drawing color interpolated lines
* 
* FORM OF CALL:
*       sfbp$$sfbp_zbuff_setup (scia_ptr,data_ptr,line,color,visual,z)
*
* ARGUMENTS:
*       scia_ptr - shared driver pointer
*       data_ptr - driver data area pointer
*       line     - line descriptor
*       color    - what color
*       visual   - what visual 
*       z        - z value 
*
* FUNCTIONAL DESCRIPTION:
*       Set up the zbuffer logic, which is the zbase address and z value
*       register. These values are set up in my diagnostics which are 
*       passed to this routine.
*
	The Z Value register contains the Z value of the first pixel in
	a line or span.  The Z value is a 36 bit value containing 12
	bits of fractional precision.  The high byte of the Z value register
	contains the stencil value used in stencil buffer operations.

	31                                   12 11                     0
	+--------------------------------------+-----------------------+
	|               Z Value                |    Z Value Fraction   |
	+--------------------------------------+-----------------------+
	63            56 55                                36 35      32
	+---------------+------------------------------------+---------+
	| Stencil value |              ignore                | Z Value |
	+---------------+----------------------------------------------+

	When read, the Stencil value is not returned.

	Value at init: 0
*
	The Z Increment register contains the incremental Z value to be
	added to each consecutive pixel.  The Z Increment is a 36 bit
	value containing 12 bits of fractional precision.

	31                                   12 11                     0
	+--------------------------------------+-----------------------+
	|         Z Increment Value            | Z Increment Fraction  |
	+--------------------------------------+-----------------------+
	63                                                 36 35      32
	+----------------------------------------------------+---------+
	|                    ignore                          |Z Inc Val|
	+--------------------------------------------------------------+


	Value at init: 0
*
*
--*/
int     sfbp$$sfbp_zbuff_setup (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z)

{
int   zoffset,fraction=0,whole=0,s=1;
union zfraction_reg   *zf;
union zvalue_reg      *zv;


zf = (union zfraction_reg   *)&fraction;
zv = (union zvalue_reg      *)&whole;

fraction = whole = 0;
zf->sfbp$r_zfraction_bits.sfbp$v_fraction = (z->value_fraction&0xFFF);
zf->sfbp$r_zfraction_bits.sfbp$v_whole    = (z->value_whole);
zv->sfbp$r_zvalue_bits.sfbp$v_whole       = (z->value_whole>>20);                        /* 24:21 and dump in low 4      */
zv->sfbp$r_zvalue_bits.sfbp$v_stencil     = (z->stencil_value);

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_fraction,fraction,LW);
(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_whole,whole,LW);

fraction = whole = 0;
zf->sfbp$r_zfraction_bits.sfbp$v_fraction = (z->increment_fraction&0xFFF);
zf->sfbp$r_zfraction_bits.sfbp$v_whole    = (z->increment_whole);
zv->sfbp$r_zvalue_bits.sfbp$v_whole       = (z->value_whole>>20);

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_increment_fraction,fraction,LW);
(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_increment_whole,whole,LW);
(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_base_addr,z->base_addr,LW);


return (1);

}

#endif

/*+
* ===========================================================================
* = sfbp$$load_stype - Load stencil_type =
* ===========================================================================
*
* OVERVIEW:
*       Load the z type info 
* 
* FORM OF CALL:
*       sfbp$$load_stype (scia_ptr,data_ptr,visual, z)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*       visual          - visual type to load
*       z               - z type
*
* FUNCTIONAL DESCRIPTION:
*       Load the stencil type values into the mode register. The console driver
*       does no stencilling. The diagnostics are the only tests to use the
*       stencil.
*
	The Stencil Mode Register controls the behavior of the stencil
	buffer and z-buffer logic when performing a z-buffered operation
	(see the Mode Register below).


	31 30 28 27 25 24 22 21 19 18 16 15            8 7            0
	+-+-----+-----+-----+-----+-----+---------------+--------------+
	|Z|Ztest|dpass|dfail|sfail|Stest|    sRdMask    |    sWrMask   |
	+-+-----+-----+-----+-----+-----+---------------+--------------+

	The Z bit disables updating of the z-buffer.

	Z:	0 - replace
		1 - keep (don't update the z-buffer)

	The Ztest and Stest fields specify the comparison to be used for:

		Ztest: z-buffer buffer comparison
		Stest: stencil buffer comparison

	The comparison is "new <compareOp> memory".  For the stencil
	comparison, both the "new" and "memory" values are AND'ed with
	the sRdMask field *before* the comparison is performed.  All
	comparisons are unsigned.  The encoding of the comparisons is
	as follows:

		000	>= (gequal)
		001	Always
		010	Never
		011	<  (less)
		100	=  (equal)
	 	101	<= (lequal)
		110	>  (greater)
		111	#  (notequal)

	The sfail, dfail, and dpass fields define the operation to perform
	on the stencil buffer when:

		sfail: stencil test fails.
		dfail: stencil test passes but the depth test fails.
		dpass: stencil test passes and the depth test passes.

	The encoding for these fields is as follows:

	        000	keep
                001	zero
                010	replace
		011	increment
		100	decrement
		101	invert

	The sWrMask field is an 8-bit mask which specifies which planes
	of the stencil buffer may be modified.  A '1' in some bit position
	enables updating of that plane, whereas a '0' disables the updating
	of that plane.

	Value at init: 0
*
*
--*/
int     sfbp$$load_stype (SCIA *scia_ptr,SFBP *data_ptr,VISUAL *visual , Z *z)

{
int      i,count,s=1;
union    stencil_reg stencil;

if (visual->doz)
 {
 stencil.sfbp$r_stencil_bits.sfbp$v_stencil_wmask = z->wmask;
 stencil.sfbp$r_stencil_bits.sfbp$v_stencil_rmask = z->rmask;
 stencil.sfbp$r_stencil_bits.sfbp$v_stencil_stest = z->stest;
 stencil.sfbp$r_stencil_bits.sfbp$v_stencil_sfail = z->sfail;
 stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zfail = z->zfail;
 stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zpass = z->zpass;
 stencil.sfbp$r_stencil_bits.sfbp$v_stencil_ztest = z->ztest;
 stencil.sfbp$r_stencil_bits.sfbp$v_stencil_z     = z->replace;

 if (visual->doz)
        (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,stencil.sfbp$l_whole,LW);
 }

return (s);
}


