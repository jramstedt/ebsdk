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

#define PIXEL_MASK         0xFFFF
#define LINE_LENGTH        16
#define R       0
#define G       1
#define B       2

static          char *w_type[2]={"X","NT"};

/* Add 4096 to bresh continue register aliases  */
#define         RegisterAlias           0x1000
#define         RegisterAliasInt        RegisterAlias/4
#define         AliasEntries            64


/* These are constants used by the cfg_read/cfg_write code      */
#define   PCI_byte_mask 0
#define   PCI_word_mask 1
#define   PCI_long_mask 3

void     pci_outmem(unsigned __int64 address, unsigned int value, int length);
unsigned int pci_inmem(unsigned __int64 address, int length);

int 	 PrintVerify=0;


/*+
* ===========================================================================
* = sfbp$$sfbp_draw_line - Draw a Line =
* ===========================================================================
*
* OVERVIEW:
*       Draw a line 
* 
* FORM OF CALL:
*       sfbp$$sfbp_draw_line (SCIA *scia_ptr,SFBP *data_ptr,LINE *line ,COLOR *color, VISUAL *visual,Z *z)
*
* ARGUMENTS:
*       scia_ptr - shared driver pointer
*       data_ptr - driver data area pointer
*       line     - line type structure
*       color    - color information
*       visual   - visual information
*       z        - zbuffer information
*
* FUNCTIONAL DESCRIPTION:
*       This routine will use Breshenham calculations for drawing lines the 
*       old way or the new way. The parameters are calculated using the
*       equations from the sfb+ spec. 
*
	In the original SFB, software was required to compute and write the
	Bresenham parameters to the SFB.  This method is still available in the
	SFB+, but a streamlined interface is also provided.  In the new
	interface, a single write to one of the Slope registers will cause
	the Bresenham registers to be loaded and the first portion (up to
	sixteen pixels) to be drawn, using data from the Data Register.

	Z-buffering and color interpolation for lines are independently
	selected by additional bits in the Mode Register.  The Z-buffer
	location is determined by the Z Base Address register.  There are
	additional registers for specifying the starting values of R, G, B,
	and Z as well as for the incremental values of R, G, B, and Z.

	To correctly initialize the color interpolation logic or the Z-buffer
	logic, software MUST use one of the Slope registers.  The older
	interface does not have provisions for properly initializing the values
	used to step through the dither matrix, nor to step through the Z
	buffer.

	In line mode the data is interpreted as a bit pattern,	where 1's
	are expanded into foreground pixels and 0's are either expanded
	into background pixels (opaque line draw mode) or are no-op's
	(transparent line draw mode).  Since writes to the Frame Buffer
	are to word addresses, in 8 bit mode, the low two bits defining
	the byte/pixel address is supplied in <17:16> of the data field.

	31                        18  16 15                            0
	+---------------------------+---+------------------------------+
	|           ignore          |adr|          Line Data           |
	+---------------------------+---+------------------------------+

	When painting with the color interpolation logic, the line logic uses
	the destination depth (see the Raster-op Register below) to determine
	how to compress and replicate the extended RGB values.  8-bit visuals
	are reduced to 3-3-2 RGB, and then replicated 4 times across the 32-bit
	word. 12-bit visuals are reduced to 4-4-4 RGB, and then replicated
	twice across both sets of RGB values.  (See the picture of pixel
	formats in simple mode above.)

	When painting using sequential interpolated lines, 8-bit visuals are
        produced by replicating the 8-bit whole portion of the Red Value
	Register 4 times across the 32-bit word. Unlike color interpolation
	where 3-3-2 RGB is produced by coalescing data from all the color
	registers, sequentially ordered interpolation through a full 8-bit
	range is enabled when using the sequential interpolation logic.
	Sequential interpolated lines can only be used for 8-bit visuals.

	The sfb+ will always leave the line hardware in a nice state for
	Polyline-like drawing, for drawing contiguous lines end-to-end.
	Software may load in a new foreground color or new Z and RGB
	increments for the new line, but the hardware will always leave RGB
	Values, Z Value, Z Address, x and y offsets into the dither matrix,
	and frame buffer Address in the right state.

(Software notes)

	To paint solid lines, initialize the Data register to all 1's at the
	top of the routine.  To paint connected lines, also initialize the
	Address register to the first pixel of the first line.  Then for each
	line write to the appropriate slope register (and then to the BresCont
	register as needed for the length of the line).  The Address register
	will be left with the starting address of the next line.

	To paint solid unconnected lines, for each line first write to the
	Address register, then to the appropriate Slope register (and the
	BresCont register as needed).

	In the very fastest code sequences for lines (which deals with a single
	clip rectangle), the code will probably end up branching into 8
	different cases, and write the appropriate Slope register directly.

	For clipped lines, write the SlopeNoGo register with the appropriate
	data.  Then for each rectangle that intersects the line, write the
	Bres3 register, the Address register, and then the BresCont register as
	many times as needed.  (On Alpha systems, you can write the first 16
	pixels by loading Bres3, then writing data to a frame buffer address.
	You could do this on a MIPS machine as well, but benchmarks have shown
	that this involves too many translation lookaside buffer faults, and
	so it's faster to avoid writing to the frame buffer.)

	In clipped line code that deals with multiple clip rectangles, I
	suspect that the best thing to do is to get a pointer to the base of
	the SlopeNoGo registers, and then OR in bits to the address as the
	conditionals dx < 0, dy < 0, and abs(dx) < abs(dy) are determined.

	If multiple rectangle clipped line code has already written to the
	SlopeNoGo register, then discovers that the line is completely visible,
	it is fastest to paint to the appropriate Slope register rather than to
	Bres3 and then Address/BresCont or the frame buffer.

	To paint unclipped dashed lines, follow the directions for solid lines,
	but remember to load the dash pattern into Data before writing to the
	appropriate Slope register.  Alternatively, write to the SlopeNoGo
	register, then write dash data into the BresCont register.  Clipped
	dashed lines use the same ordering of commands as clipped solid lines.

	If you want lines to include the second endpoint, set capEnds in the
	Mode register to 1.  If you want to exclude the second endpoint, set
	capEnds to 0.
*
* Algorithm:
*
*       Calculate DX and DY and line length
*       IF slow speed (old way)
*          BEGIN
*             IF not a polyline 
*                        THEN compute breshenham parameters
*             Call Slow Calculation Routine
*          END  
*          ELSE call fast calculation routine
*
*       If waiting on line done(asic done)
*               THEN wait for csr bit clear.
*
--*/
int     sfbp$$sfbp_draw_line (SCIA *scia_ptr,SFBP *data_ptr,LINE *line ,COLOR *color, VISUAL *visual,Z *z)

{
register  int  i,x_inc,y_inc,s=1;
register  int  dmajor,dminor,amajor,aminor;
register  int  dxGEdy,dxGE0,dyGE0,majorGE0,minorGE0;
register  int  e,e1,e2,dx,dy,e_inc;

line->dx = dx = (line->xb-line->xa)&0xffffffff;
line->dy = dy = (line->yb-line->ya)&0xffffffff;
dxGEdy   = drvabs(dx) >= drvabs(dy) ? TRUE : FALSE;
line->len= dxGEdy ? drvabs(dx) : drvabs (dy);  

if (line->speed==slow)
   {
    if (!line->polyline)
    {
    dxGEdy = drvabs(dx)>drvabs(dy) ? TRUE: FALSE;
    dxGE0  = dx>=0 ? TRUE : FALSE;
    dyGE0  = dy>=0 ? TRUE : FALSE;

    dmajor = dxGEdy ? drvabs(dx) : drvabs(dy) ;
    dminor = dxGEdy ? drvabs(dy) : drvabs(dx) ;

    majorGE0 = dxGEdy ? dxGE0 : dyGE0;
    minorGE0 = dxGEdy ? dyGE0 : dxGE0;
   
    e1 = dminor;
    e2 = dmajor +  ~dminor + 1; 
    if (line->window_type==NTwindow){
      if (data_ptr->sfbp$l_etch_revision > TgaPass2Plus)
        e_inc = dxGEdy ? !dyGE0 : !dxGE0 ;            
      else e_inc = dxGEdy ? dyGE0 : !dxGE0 ;
    }
    else e_inc = majorGE0;
    e = ((dminor<<1) + (~dmajor) + e_inc) >> 1;

    line->e1 = drvabs (e1);
    line->e2 = drvabs (e2);
    line->e  = e;
  
    if (dxGEdy)
      {
      line->a1=((dx>0) ? 1 : -1 );                                     
      line->a2=line->a1+((dy>0) ? data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels: -data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels);
      }  
    else
      {
      line->a1 = (dy>0)? data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels: -data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels;
      line->a2 = line->a1+ ((dx>0) ? 1 : -1) ;                          
      }
    } 
    sfbp$$calc_slow_line (scia_ptr,data_ptr,line,color,visual,z);
    } else sfbp$$calc_fast_line (scia_ptr,data_ptr,line,color,visual,z);


if (line->wait_buffer) 
 {
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 }

 
return (s);
}


/*+
* ===========================================================================
* = sfbp$$calc_slow_line - Make A Line using Hardware Registers =
* ===========================================================================
*
* OVERVIEW:
*       Make A line 
* 
* FORM OF CALL:
*       sfbp$$calc_slow_line scia_ptr,data_ptr,line,color,visual,z)
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
*       This routine sets up the breshenham registers using our already
*       calculated parameters.
*

Address Register
----------------

	The Address register contains the Frame Buffer byte address to be used
	by the next write to the Start register or the next write to the Slope
	register in line mode.  Since the Address register can reference the
	full 16MB Frame Buffer address space, it can be used to indirectly
	reference Frame Buffer locations not directly addressable due to
	slot size limitations.  Location 0 in the Address Register corresponds
	to the first memory location in VRAM, and NOT the bottom of the sfb+
	chip address space.  Thus the Address Register cannot be used to
	address sfb+ registers, or the EEPROM.

	31            24 23                                            0
	+---------------+----------------------------------------------+
	|   ignore      |                 Address                      |
	+---------------+----------------------------------------------+

	Value at init: 0


Slope Registers
---------------

	The Slope registers provide a simplified interface to the
	Bresenham engine, eliminating the need to write to multiple registers
	and to the Frame Buffer to draw a line.  A write to any of the slope
	registers specifies the absolute value of the horizontal and
	verti
cal deltas of the line.  The signs and relative magnitudes of
	the deltas are inferred from the specific slope register that is
	written.  There are eight slope registers, one for each
	combination of sign and magnitude of the dx and dy values, writing
	to any of these registers will cause the Bresenham registers to be
	computed, and the initial portion (up to sixteen pixels) of the
	line to be drawn.  There are also eight SlopeNoGo registers which
	cause the Bresenham parameters to be computed just like the Slope
	registers, but do not initiate the drawing of the line.
                                   
	Results are undefined if you write dx = 0, dy = 0 to any of the
	Slope or SlopeNoGo registers.  That is, software must filter out
	0-length lines.

	The derivation of the Bresenham parameters is as follows:

	    dmajor = (dxGEdy ? dx : dy);
	    dminor = (dxGEdy ? dy : dx);
                      
	    majorGE0 = (dxGEdy ? dxGE0 : dyGE0);
	    minorGE0 = (dxGEdy ? dyGE0 : dxGE0);
                      
	    amajor = (dxGEdy ? SFBPIXELBYTES : NLWIDTH);
	    aminor = (dxGEdy ? NLWIDTH : SFBPIXELBYTES);
                      
	    b.len = dmajor + capEnds;
	    b.e1 = dminor;
	    b.e2 = dmajor + ~dminor + 1;
                      
	    if (NT_compliant_lines){   
              if (data_ptr->revision > TgaPass2Plus)
                errorInc = (dxGEdy ? !dyGE0 : !dxGE0);
              else
	     	errorInc = (dxGEdy ? dyGE0 : !dxGE0);
            }
	    else                   
		errorInc = majorGE0;
                                   
	    b.e = ((dminor<<1) + ~dmajor + errorInc) >> 1;
	    b.a1 = (majorGE0 ? amajor : ~amajor + 1);
	    b.a2 = (minorGE0 ? aminor : ~aminor + 1) + b.a1;
                                   
	    b.x2 = (dxGE0 ? 1 : -1);
	    b.y2 = (dyGE0 ? 1 : -1);
	    b.x1 = (dxGEdy ? b.x2 : 0);
	    b.y1 = (dxGEdy ? 0 : b.y2);

	A Bresenham step looks like:

	    if (data & 0) *address = pixel; 
            data >>= 1;
            if (b.e < 0) 
                {
                address += b.a1;
                b.e += b.e1;
                } else 
                {
                address += b.a2;
                b.e -= be2;
                }

	31                            16 15                            0
	+-------------------------------+------------------------------+
	|              |dy|             |             |dx|             |
	+-------------------------------+------------------------------+


Span Width Register
-------------------

	The Span Width Register is an alias for the Slope Register |+dx| >
	|+dy|.  Normally this register is used to paint horizontal spans,
	and so will have some positive dx, and dy = 0.

	When read, the Span Width register returns the register values
	pertaining to dithering.

	31                                 13 12      8 7       3 2 1 0
	+------------------------------------+---------+---------+-+-+-+
	|                 ignore             |   row   |   col   |g|x|y|
	+--------------------------------------------------------------+

	row: row value last written to the Red   Value register
	col: col value last written to the Green Value register
	g:   dxGEdy from last slope write
	x:   dxGE0  from last slope write
	y:   dyGE0  from last slope write


Start Register
--------------

	Writes to the Start Register are interpreted exactly the same as
	writes to the Frame buffer, except that the Address register is
	used as the Frame Buffer address for the write.  The data value
	written to the start register is interpreted based on the mode
	in the same fashion as a direct write to the Frame Buffer.

	The Start Register can be used to write to Frame Buffer addresses
	which lie outside the slot Address space defined by the system.

	Reading the start register returns the current SFB+ asic revision.

Bresenham Register 1
--------------------

	The Bresenham 1 register contains the address increment and
	error increment to be used when the cumulative error value is
	negative.  The error increment is a positive value that is added
	to the cumulative error.

	31                            16 15                            0
	+-------------------------------+------------------------------+
	|     Address Increment 1       |      Error Increment 1       |
	+-------------------------------+------------------------------+

	Value at init: 0


Bresenham Register 2
--------------------

	The Bresenham 2 register contains the address increment and
	error increment to be used when the cumulative error value is
	positive.  The error increment is a positive value that is
	subtracted from the cumulative error.

	31                            16 15                            0
	+-------------------------------+------------------------------+
	|     Address Increment 2       |      Error Increment 2       |
	+-------------------------------+------------------------------+

	Value at init: 0


Bresenham Register 3
--------------------

	This Bresenham 3 register contains the initial error value to be
	used by the Bresenham line engine and the line length of the
	initial segment. The Initial Error value is a signed 17 bit
	value, the length is a four bit value specifying from 1 to 16
	pixels as the initial segment length (0 = 16 pixels). After each
	operation the length value is reinitialized so subsequent writes
	to the continue register will extend the line by 16 pixels.

	31                              15 14                  4 3     0
	+---------------------------------+---------------------+------+
	|       Initial Error Value       |       ignore        |Length|
	+---------------------------------+---------------------+------+

	Value at init: 0


Bresenham Width register
------------------------

	The Bresenham Width register defines the width in pixels of the
	drawable and Z buffer.  These widths define the NLWIDTH parameter
	used when computing Bresenham parameters from slopes and must be
	defined prior to using the slope registers.

	31                            16 15                            0
	+-------------------------------+------------------------------+
	|       Z buffer width          |        Drawable width        |
	+-------------------------------+------------------------------+

	Value at init: 0


Line Continue Register
----------------------

	The Line Continue Register when written will continue a line for an
	additional 16 pixels.  The line data is interpreted as a bit pattern
	where 1's are expanded into foreground pixels and 0's are either
	expanded into background pixels (opaque line mode) or are no-op's
	(transparent line draw mode).

	If the Address Register has been updated since the last frame
	buffer operation, the continue operation will use the address
	register as the starting address, otherwise the chip will continue
	from where the previous operation left off.

	Writing to the continue register without updating the address register
	in modes other than line mode may yield unexpected continuation
	addresses.

	31                            16 15                            0
	+-------------------------------+------------------------------+
	|           ignore              |          Line Data           |
	+-------------------------------+------------------------------+

	Reads to the line continue register will return the internal bresenham
	parameters associated with Z address increments set-up by the last
	slope register write.

	31                            16 15                            0
	+-------------------------------+------------------------------+
	|     Z Address Increment 2     |     Z Address Increment 1    |
	+-------------------------------+------------------------------+
*
*
Data Register
-------------

	In Line mode, this register defines the pattern of pixels to be
	written when the slope register is written.  Bits that are set are
	expanded into Foreground pixels, bits that are clear are no-ops in
	Transparent line mode or expanded into Background pixels in Opaque
	line mode.  Note that this register only provides data for the
	beginning portion of a line, i.e. those pixels which are written
	as a result of writing a slope register.  Data for subsequent
	pixels are provided when the Continue Register is written.

	31                            16 15                            0
	+-------------------------------+------------------------------+
	|            ignore             |          Line Data           |
	+-------------------------------+------------------------------+

	In Fill modes, the Data register defines a 32-bit repeating pattern
	aligned to 4 pixels, which is used as a transparent stipple.

	31                                                             0
	+--------------------------------------------------------------+
	|                       Block Fill Data                        |
	+--------------------------------------------------------------+

	In Dma Write mode, the Data register defines a word mask to be
	anded with the data before it is written to memory.  The Data
	register is not hardware interlocked so software must insure that
	the value of the data register does not change while a Dma Write
	is in progress.

	31                                                             0
	+--------------------------------------------------------------+
	|                       Dma Write Mask                         |
	+--------------------------------------------------------------+

	Value at init: ffffffff
*
*       The driver will use rom space aliasing for the continue register
*       so that I do not have to flush the write buffer. What this does
*       is pack the write buffer with data and with block mode enabled
*       should make lines scream.
*
* ALGORITHM:
*
*       IF NOT polyline THEN
*               Write b1,b2, and b3 with calculated values from draw_line
*               Set up the Mode register based on line type
*               Load the line visual
*               IF color change THEN load fg and bg values 
*               IF one of opaque line modes then data = 0 ELSE data = 0xffff
*               Calculates the bytes per pixel and pixels per z based on visual depth.
*               Calculate the video ram offset from Xa and Ya coordinates.
*               Add console offset to video ram offset ( first 4K void)
*               Write the bresenham width register
*               IF line draw method is address-data 
*                       THEN 
*                           IF Residual pixels
*                              THEN Write to Video Ram Address pixel Mask with bytemask in 17:16
*                                  INCREMENT the video ram  by residual bytes
*                           REPEAT for number of 16 pixel operations
*                               Write address/data pairs with bytemask in 17:16
*               ELSE 
*                       Write Line Address Register
*                       IF residual pixels
*                               THEN Write Continue register with residual pixel mask
*                       REPEAT for number of 16 pixel operations
*                               Write continue register (flush also on alpha)                
*
--*/
int     sfbp$$calc_slow_line (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z)

{
register int bytemask;
register int i,j,res_data,res_bytes,ops,addr,base,data;
union    bresh_reg1 *b1;
union    bresh_reg2 *b2;
union    bresh_reg3 *b3;
int      bpp,ppz,local1=0,local2=0,local3=0;
volatile register int   count,*cptr;
volatile register char  *reg;

 reg        = (char *)sfbp_ioseg_ptr->sfbp$a_rom_mem_ptr;
 reg       += (0x80000);
 cptr       = (int *)reg;
 
 ops        = line->len/LINE_LENGTH;
 res_bytes  = line->len%LINE_LENGTH;
 data       = PIXEL_MASK;

if (!line->polyline)
 {
 b1         = (union bresh_reg1 *)&local1;
 b2         = (union bresh_reg2 *)&local2;
 b3         = (union bresh_reg3 *)&local3;
 b1->sfbp$r_bresh_reg1_bits.sfbp$v_bresh_err_inc = line->e1;  
 b1->sfbp$r_bresh_reg1_bits.sfbp$v_bresh_add_inc = line->a1;  
 b2->sfbp$r_bresh_reg2_bits.sfbp$v_bresh_err_inc = line->e2;
 b2->sfbp$r_bresh_reg2_bits.sfbp$v_bresh_add_inc = line->a2;
 b3->sfbp$r_bresh_reg3_bits.sfbp$v_bresh_ierror  = line->e;               
 b3->sfbp$r_bresh_reg3_bits.sfbp$v_bresh_length  = res_bytes ? res_bytes : 0;

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_bresh_reg1,b1->sfbp$l_whole,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_bresh_reg2,b2->sfbp$l_whole,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_bresh_reg3,b3->sfbp$l_whole,LW);

 sfbp$$mode_setup  (scia_ptr,data_ptr,line,color, visual, z);

 visual->line_type = line->window_type;

 sfbp$$load_visual (scia_ptr,data_ptr,visual,z ) ;
 
  if (data_ptr->sfbp$l_module==hx_8_plane)
	 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);
 else (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,0x00ffffff,LW);

 }

if (line->color_change)
  {
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_foreground,color->fg_value,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_background,color->bg_value,LW);
  }
 
if (line->line == z_opaque_line_mode || 
         line->line == opaque_line_mode ) data = 0;

switch (visual->depth)
 {
 case packed_8_packed:                        
 case packed_8_unpacked:                      
 case unpacked_8_packed:                      
 case unpacked_8_unpacked:                    
        /* Bytes per Pixel and Pixels Per Z value       */
        bpp   = 1;      
        ppz   = 4;
        break;
                
 case source_12_low_dest:                     
 case source_12_hi_dest:                      
        bpp   = 4;
        ppz   = 1;
        break;
                
 case source_24_dest:                         
        bpp   = 4;
        ppz   = 1; 
        break;
 }
 if (visual->depth==packed_8_unpacked || visual->depth==unpacked_8_unpacked ) 
        addr  = (line->xa*4 + line->ya*data_ptr->sfbp$l_bytes_per_scanline); 
 else addr = (line->xa*bpp + line->ya*data_ptr->sfbp$l_bytes_per_scanline); 

 addr += data_ptr->sfbp$l_console_offset;

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_bresh_width,
        ((ppz*data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels)<<16)|
              data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels,LW);

 if (data_ptr->sfbp$l_print) 
        printf ("[%d,%d] to [%d,%d] addr 0x%x ops %d \n",
                        line->xa,line->ya,line->xb,line->yb,addr,ops);
 
 if (line->draw_method==address_data)                                                                             
  {
   /* Get the residual bytes first      */
   if (res_bytes)
    {
    bytemask = (int)addr%4;
    for (j=0,res_data=0;j<res_bytes;j++)res_data |= (1<<j);
    res_data  |= (bytemask<<16);
    if (line->line == opaque_line_mode )res_data = (~res_data)&0x3FFFF;
    (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,(addr&(~3)),res_data,LW);
    addr += res_bytes;
    }
   /* And then the remainder            */
   for (i=0;i<ops;i++,addr+=LINE_LENGTH) 
    {
    data     = PIXEL_MASK;
    bytemask = (int)addr%4;
    data    |= (bytemask<<16);
    if (line->line == opaque_line_mode )data = (~data)&0x3FFFF;
    (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,(addr&(~3)),data,LW);
    }
  }
  else 
   {
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_line_address,addr,LW);

   /* Do the residual first               */
   /* Because thats the way it works      */
  if (res_bytes)
   {

   for (i=0,res_data=0;i<res_bytes;i++)res_data |= (1<<i);
   if (line->line == opaque_line_mode )res_data = (~res_data)&PIXEL_MASK;
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_bresh_continue,res_data,LW);
   }

 for (i=count=0;i<ops;i++,cptr++)
         pci_outmem (cptr,data,PCI_long_mask);
  }

return (SS$K_SUCCESS);
}




/*+
* ===========================================================================
* = sfbp$$calc_fast_line - Make A Line using Hardware Registers =
* ===========================================================================
*
* OVERVIEW:
*       Make A line using the slope registers
* 
* FORM OF CALL:
*       sfbp$$calc_fast_line scia_ptr,data_ptr,line,color,visual,z)
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
*       This routine determines which slope register to write to
*       when drawing a line. Writing the slope register is then
*       used to initiate drawing a line. Horizontal lines 
*       require that I use the span register.
*
* ALGORITHM:
*
*       IF NOT polyline THEN
*          IF not span line THEN calculate which slope register to use
*               Set up the Mode register based on line type
*               Load the line visual
*               IF color change THEN load fg and bg values 
*               IF one of opaque line modes then data = 0 ELSE data = 0xffff
*               Calculates the bytes per pixel and pixels per z based on visual depth.
*               Calculate the video ram offset from Xa and Ya coordinates.
*               Add console offset to video ram offset ( first 4K void)
*               Write the bresenham width register
*               Write to slope register to initiate a line draw ( note: data register has mask)
*               
*               IF line draw method is address-data 
*                       THEN 
*                           IF Residual pixels
*                              THEN Write to Video Ram Address pixel Mask with bytemask in 17:16
*                                  INCREMENT the video ram  by residual bytes
*                           REPEAT for number of 16 pixel operations
*                               Write address/data pairs with bytemask in 17:16
*               ELSE 
*                       Write Line Address Register
*                       IF residual pixels and slope nogo mode
*                               THEN Write Continue register with residual pixel mask
*                       IF NOT residual pixels and NOT slope nogo THEN decrement count (slope already written)
*                               ELSE Write Continue register with residual pixel mask
*                       REPEAT for number of 16 pixel operations
*                               Write continue register (flush also on alpha)                
*
--*/
int     sfbp$$calc_fast_line (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z)

{
register int bytemask;
register int i,j,ops,res_bytes,addr,base,data;
register int res_data,slope_value,offset,slope_reg;
register int count,bpp,ppz,adx,ady,dxGEdy,dxGE0,dyGE0,majorGE0,minorGE0;
volatile register int   *cptr;
volatile register char  *reg;

reg        = (char *)sfbp_ioseg_ptr->sfbp$a_rom_mem_ptr;
reg       += (0x80000);
cptr       = (int *)reg;

slope_reg   = (line->nogo) ? sfbp$k_slope_nogo_0 : sfbp$k_slope_go_0 ;
slope_value = (drvabs (line->dy)<<16) | (drvabs (line->dx));
data        = PIXEL_MASK;;
dxGEdy      = drvabs(line->dx) >= drvabs(line->dy) ? TRUE : FALSE;
dxGE0       = line->dx>=0 ? TRUE : FALSE;
dyGE0       = line->dy>=0 ? TRUE : FALSE;

switch (visual->depth)
 {
 case packed_8_packed:                        
 case packed_8_unpacked:                      
 case unpacked_8_packed:                      
 case unpacked_8_unpacked:                    
        /* Bytes per Pixel and Pixels Per Z value       */
        bpp   = 1;      
        ppz   = 4;
        break;
                
 case source_12_low_dest:                     
 case source_12_hi_dest:                      
        bpp   = 4;
        ppz   = 1;
        break;
                
 case source_24_dest:                         
        bpp   = 4;
        ppz   = 1; 
        break;
 }
 if (visual->depth==packed_8_unpacked || visual->depth==unpacked_8_unpacked ) 
        addr  = (line->xa*4 + line->ya*data_ptr->sfbp$l_bytes_per_scanline); 
 else addr = (line->xa*bpp + line->ya*data_ptr->sfbp$l_bytes_per_scanline); 

 addr += data_ptr->sfbp$l_console_offset;
 
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_bresh_width,
        ((ppz*data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels)<<16)|
              data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels,LW);

ops         = line->len/LINE_LENGTH;
res_bytes   = line->len%LINE_LENGTH;

if (!line->polyline)
 {
 if (!line->span_line || (line->span_line && line->dx <0 ) )
  {
  if    (  (line->dx <0 && line->dy <0) &&  (drvabs(-line->dx)) <= (drvabs (-line->dy)))offset = 0;
  else if ((line->dx <0 && line->dy >0) &&  (drvabs(-line->dx)) <= (drvabs ( line->dy)))offset = 4;
  else if ((line->dx >0 && line->dy <0) &&  (drvabs( line->dx)) <= (drvabs (-line->dy)))offset = 8;
  else if ((line->dx >0 && line->dy >0) &&  (drvabs( line->dx)) <= (drvabs ( line->dy)))offset = 12;
  else if ((line->dx <0 && line->dy <0) &&  (drvabs(-line->dx)) >= (drvabs (-line->dy)))offset = 16; 
  else if ((line->dx <0 && line->dy >0) &&  (drvabs(-line->dx)) >= (drvabs ( line->dy)))offset = 20;
  else if ((line->dx >0 && line->dy <0) &&  (drvabs( line->dx)) >= (drvabs (-line->dy)))offset = 24;
  else if ((line->dx >0 && line->dy >0) &&  (drvabs( line->dx)) >= (drvabs ( line->dy)))offset = 28;
  else 
   {
   if      (line->dx >0  && line->dy==0)offset= 28;
   else if (line->dx <0  && line->dy==0)offset= 20;
   else if (line->dy >0  && line->dx==0)offset= 12;
   else if (line->dy <0  && line->dx==0)offset=  8;
   }               
 slope_reg    += offset;
 } else slope_reg = sfbp$k_span_width;  

}
 sfbp$$mode_setup  (scia_ptr,data_ptr,line,color, visual, z);

 if (!line->polyline)
        {
        visual->line_type = line->window_type;
        sfbp$$load_visual (scia_ptr,data_ptr, visual ,z) ;
        }
         
 if (data_ptr->sfbp$l_module==hx_8_plane)
	 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);
 else (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,0x00ffffff,LW);

/* NOTE: Do not Write these registers if not in Interpolated MODE       */
/* Because they share latches with the increment registers              */

if ( (line->line == transparent_line_mode || 
      line->line == opaque_line_mode ||
      line->line == z_transparent_line_mode || 
      line->line == z_opaque_line_mode)  &&
      line->color_change)
          {
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_foreground,color->fg_value,LW);
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_background,color->bg_value,LW);
          }
         
/* Load an Address into the Engine      */
if (line->draw_method==line_address)
        (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_line_address,addr,LW);

/* Initiate a Slope Write               */
if (!line->polyline)
        (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,slope_reg,slope_value,LW);

if (line->line == z_opaque_line_mode || 
         line->line == opaque_line_mode ) data = 0;

/* If using frame buffer writes...then get bytemask info        */
/* Slope already calculated                                     */
/* I only include this so I can test for it here also           */

 if (data_ptr->sfbp$l_print) 
        printf ("[%d,%d] to [%d,%d] addr 0x%x ops %d \n",
                        line->xa,line->ya,line->xb,line->yb,addr,ops);

if (line->draw_method==address_data)
 {
   if (res_bytes)
    {
    bytemask = (int)addr%4;
    for (j=0,res_data=0;j<res_bytes;j++)res_data |= (1<<j);
    res_data  |= (bytemask<<16);
    if (line->line == opaque_line_mode )res_data = (~res_data)&0x3FFFF;
    (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,(addr&(~3)),res_data,LW);
    addr += res_bytes;
    }
   /* And then the remainder            */
   for (i=0;i<ops;i++,addr+=LINE_LENGTH) 
    {
    data     = PIXEL_MASK;
    bytemask = (int)addr%4;
    data    |= (bytemask<<16);
    if (line->line == opaque_line_mode )data = (~data)&0x3FFFF;
    (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,(addr&(~3)),data,LW);
    }
 }
 else
  {
  /* If in NOGO mode, then we need to write continue register, otherwise  */
  /* GO mode uses the data register for the initial residual write        */
  if (res_bytes && line->nogo )
   {
   for (i=0,res_data=0;i<res_bytes;i++)res_data |= (1<<i);
   if (line->line == opaque_line_mode )res_data = (~res_data)&PIXEL_MASK;
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_bresh_continue,res_data,LW);
   }

  /* If in slope GO mode, then the initial line draw was already done     */
  /* If we had residual bytes though...then don't decrement the count     */
  if (!res_bytes && !line->nogo)ops--;                                                   

  for (i=count=0;ops>0 && i<ops;i++,cptr++)
         pci_outmem (cptr,data,PCI_long_mask);
  }

return (SS$K_SUCCESS);
}

int drvabs(int x){return  ( (x<0) ? -x : x);}
