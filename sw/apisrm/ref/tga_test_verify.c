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
#include   "cp$src:platform.h"
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
#define ZMASK   0x00ffffff
#define COLOR_MASK 0x000ff000
#define COLOR_VALUE_MASK   0xF8FFFFFF0
#define COLOR_INCR_MASK    0x00FFFFFF0
#define INTERPMASK 0xfffffff0            /* to truncate interp bits */
#define DITHERMASK 0x000ff800            /* to truncate dither bits */
#define M 32    /* matrix size  */
#define N 32
#define pR 0    /* phase shifts */
#define pG 8
#define pB 16
#define D 7     /* matrix depth */

#define         color$m_bits    12

extern 	int 	PrintVerify;


/*+
* ===========================================================================
* = sfbp$$verify_draw_line - Draw a Line =
* ===========================================================================
*
* OVERVIEW:
*       Verify a line 
* 
* FORM OF CALL:
*       sfbp$$verify_draw_line (scia_ptr,data_ptr,line ,color, visual,z,param)
*
* ARGUMENTS:
*       scia_ptr        - shared console data ptr
*       data_ptr        - driver data area
*       line            - line type structure
*       color           - color type structure
*       visual          - visual type structure
*       z               - z type structure
*       param           - error parameters
*
* FUNCTIONAL DESCRIPTION:
*       This routine does a calculation of the breshenham algorithm and
*       figures out what the line address coordinates should be as 
*       it is stepped. It will check each pixel for the proper value. 
*
--*/
int     sfbp$$verify_draw_line (SCIA *scia_ptr,SFBP *data_ptr,LINE *line ,COLOR *color, 
                VISUAL *visual,Z *z,int param[] , int DROM[32][32] )

{
register        int     count,i,s=1;
register        int     e_inc,e,e1,e2,x_inc,y_inc,dx,dy;
register        int     x,y, zoffset,offset,expect,actual,bytemask;
register        int     startx,starty,endx,endy;
register        char    *vram;
int             pack_value[3],value[3];
int             zactual,zvalue,zfraction;
int             dxGEdy,xx,yy,dither_rom[3],dith_value[3];
int             bpp,ppz,dmajor,dminor,amajor,aminor;
int             *ptr,dxGE0,dyGE0,majorGE0,minorGE0;
int             rotate_mask;
                     
data_ptr->sfbp$l_error_type=CompareOk;
                     
if (data_ptr->sfbp$l_verify_lines && 
        (line->verify || line->verify_z ))
 {                   
  vram    = (char *)sfbp_ioseg_ptr->sfbp$a_vram_mem_ptr;
  dx      = line->xb-line->xa;
  dy      = line->yb-line->ya;
                     
  dxGEdy = myiabs(dx)>myiabs(dy) ? TRUE: FALSE;
  dxGE0  = dx>=0 ? TRUE : FALSE;
  dyGE0  = dy>=0 ? TRUE : FALSE;
                     
  dmajor = dxGEdy ? myiabs(dx) : myiabs(dy) ;
  dminor = dxGEdy ? myiabs(dy) : myiabs(dx) ;
                     
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
  e  = ((dminor<<1) + (~dmajor) + e_inc) >> 1;
    
  x_inc   = dx < 0 ? -1 : 1; 
  y_inc   = dy <= 0 ? -1 : 1;
  starty  = line->ya;endy=line->yb;
  startx  = line->xa;endx=line->xb;

  /* If doing DMA read dither...then we get the data from my initial set up */
  /* value and then I increment it accordingly                              */
  /* I am using dither row 0 and dither column ?                            */
    
  if (data_ptr->sfbp$l_dma_read_dither)
    {
    value[R]  = (data_ptr->sfbp$l_dma_read_initial<<color$m_bits)&COLOR_MASK;
    value[G]  = (data_ptr->sfbp$l_dma_read_initial<<color$m_bits)&COLOR_MASK;
    value[B]  = (data_ptr->sfbp$l_dma_read_initial<<color$m_bits)&COLOR_MASK;
    }
  else
  {
    value[R]  = color->red_value;
    value[G]  = color->green_value;
    value[B]  = color->blue_value;
    }
    
  zvalue    = z->value_whole;
  zfraction = z->value_fraction;

  for (i=0,s=1,y=starty,x=startx;s&1 && i<myiabs(dmajor)+visual->capend;i++)  
        {
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
        /* Bytes Per Scanline Already has the Depth Included    */        
        offset = (x*bpp+y*data_ptr->sfbp$l_bytes_per_scanline); 

        /* If the visual is an unpacked visual, we are always   */
        /* On a longword boundary                               */

        if (visual->depth==packed_8_unpacked || 
            visual->depth==unpacked_8_unpacked ) 
                offset = (x*4+y*data_ptr->sfbp$l_bytes_per_scanline); 

        switch (line->line)
          { 
          case transparent_line_mode:
          case opaque_line_mode:
          case z_transparent_line_mode:
          case z_opaque_line_mode:
                 expect  = (color->method == foreground) ? color->fg_value : color->bg_value;
                 break;

          case cint_nd_line_mode:
          case cint_nd_trans_line_mode_red:
          case cint_nd_opaque_line_mode_red:
          case cint_d_line_mode:
          case z_cint_trans_nd_line_mode:
          case z_cint_trans_d_line_mode:
          case z_cint_opaq_nd_line_mode:
          case z_cint_opaq_d_line_mode:
          case z_cint_trans_nd_line_mode_red:
          case z_cint_opaq_nd_line_mode_red:
                 /* No dither check in 24 plane mode    */
                 if (visual->depth == source_24_dest)
                        {
                         if (line->line == cint_nd_trans_line_mode_red   ||
                             line->line == cint_nd_opaque_line_mode_red  ||
                             line->line == z_cint_trans_nd_line_mode_red ||
                             line->line == z_cint_opaq_nd_line_mode_red)
                                {
                                /* Sequential    interpolation in 32 plane */
                                pack_value[R] = (value[R]>>color$m_bits)&0xff;
                                expect = (pack_value[R]<<16)&0x00ff0000;
                                }
                         else
                                {
                                /* Non-Sequential interpolation in 32 plane */
                                pack_value[R] = (value[R]>>color$m_bits)&0xff;
                                pack_value[G] = (value[G]>>color$m_bits)&0xff;
                                pack_value[B] = (value[B]>>color$m_bits)&0xff;
                                expect        = (pack_value[R]<<16)|(pack_value[G]<<8)|(pack_value[B]<<0);
                                }
                        }
                 else
                 {
                   /* Check dither in 8 plane mode      */
                   if ( line->line == cint_d_line_mode || 
                        line->line == z_cint_trans_d_line_mode ||
                        line->line == z_cint_opaq_d_line_mode )
                         {
                         /* Adjust for dithering        */
                         /* My color value is in bits 19:12     */
                         /* drop in dither bits 1 bit below the number of color bits */
                         /* which is bit 16 for red and green and bit 15 for blue    */
                         /* This is the dither bits shifted left by 10 or 11 for blue*/
                         /* Since there are 9 bits of adder in hardware, we need to  */
                         /* truncate the result.                                     */

                         xx = x%M; yy = y % N;
                         dither_rom[R] = DROM[yy][(xx+pR)%M]<<10;
                         dither_rom[G] = DROM[yy][(xx+pG)%M]<<10;
                         dither_rom[B] = DROM[yy][(xx+pB)%M]<<11;
                
                         dith_value[R] = (((value[R] & DITHERMASK) - ((value[R] >> 3) & DITHERMASK)) 
                        		   & DITHERMASK) + (dither_rom[R] & DITHERMASK); 
                         dith_value[G] = (((value[G] & DITHERMASK) - ((value[G] >> 3) & DITHERMASK)) 
                        		   & DITHERMASK) + (dither_rom[G] & DITHERMASK); 
                         dith_value[B] = (((value[B] & DITHERMASK) - ((value[B] >> 2) & DITHERMASK)) 
                                           & DITHERMASK) + (dither_rom[B] & DITHERMASK);  

                         /* Get the lower 3 bits of color (or 2 for blue)
                         /* This will change for 12 bit visual  */
                         pack_value[R]   = (dith_value[R]>>color$m_bits)>>5;
                         pack_value[G]   = (dith_value[G]>>color$m_bits)>>5;
                         pack_value[B]   = (dith_value[B]>>color$m_bits)>>6;
                         expect          = ((pack_value[R]&7)<<5) | ((pack_value[G]&7)<<2) | ((pack_value[B]&3)<<0);
                         expect         |= (expect<<8) | (expect<<16) | (expect<<24);
                         }
                        else
                         {
                         /* No dither...just interpolation      */
                         /* Get the lower 3 bits of color (or 2 for blue)       */
                         /* This will change for 12 bit visual  */

                         /* Non-dithered line mode red is 8 bits of interpolated data   */
                         /* Rather than the 3 bits used for the rgb format              */
                         /* So we don't mess with it                                    */
                         if (line->line == cint_nd_trans_line_mode_red   ||
                             line->line == cint_nd_opaque_line_mode_red  ||
                             line->line == z_cint_trans_nd_line_mode_red ||
                             line->line == z_cint_opaq_nd_line_mode_red)
                                {
                                pack_value[R] = (value[R]>>color$m_bits)>>(0);
                                expect = pack_value[R]&0xff;
                                }
                         else 
                                {
                                pack_value[R] = (value[R]>>color$m_bits)>>5;
                                pack_value[G] = (value[G]>>color$m_bits)>>5;
                                pack_value[B] = (value[B]>>color$m_bits)>>6;
                                expect        = ((pack_value[R]&7)<<5) | ((pack_value[G]&7)<<2) | ((pack_value[B]&3)<<0);
                                expect       |= (expect<<8) | (expect<<16) | (expect<<24);
                                }
                         }
                }
                 break;                
          }

        if (line->verify)
         {
         offset += data_ptr->sfbp$l_console_offset;
         actual  = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,offset);

         /* Unpacked visuals are always the low byte    */
         /* Unless I rotate the destination byte        */
         /* I have to get byte from expect data 3:0     */
         
         if (visual->depth==packed_8_unpacked || 
             visual->depth==unpacked_8_unpacked )
                {
                bytemask  = (int)i%4;
                expect = expect >> (bytemask<<3);
                rotate_mask = 0xff << (visual->rotation&3);
                expect <<= (visual->rotation & 3);
                expect &= rotate_mask;
                actual <<= (visual->rotation & 3);
                actual &= rotate_mask;
                }
         else
                {
                 if (visual->depth ==  source_24_dest)bytemask=0;
                 else bytemask  = (int)offset%4;
                 if (line->line == cint_nd_trans_line_mode_red   ||
                     line->line == cint_nd_opaque_line_mode_red  ||
                     line->line == z_cint_trans_nd_line_mode_red ||
                     line->line == z_cint_opaq_nd_line_mode_red)
                           {
                           if (visual->depth==source_24_dest)
                                        actual &= 0x00ff0000;
                           else  actual = (actual>>(bytemask<<3))&0xff;
                           }
                 else 
                           {
                           expect = (expect>>(bytemask<<3))&0xff;
                           actual = (actual>>(bytemask<<3))&0xff;
                           }
                }
         if ( expect != actual || data_ptr->sfbp$l_test_error )
                {
                /* Report Longword Boundary     */
                param[0] = data_ptr->sfbp$l_vram_base + offset;
                param[1] = expect;
                param[2] = actual;
                s = 0;
                data_ptr->sfbp$l_error_type=LineColorCompareError;
                if (data_ptr->sfbp$l_supplemental)
                        printf ("Line Error 0x%x expect 0x%x actual 0x%x [%d,%d] [%d,%d]MOD32 \n",
                                        param[0],expect,actual,x,y,x%32,y%32);
                }else if (PrintVerify)
			   printf ("OK Offset 0x%x expect 0x%x actual 0x%x [%d,%d] [%d,%d]MOD32 \n",
	                                        offset,expect,actual,x,y,x%32,y%32);
         }

        if (s&1 && line->verify_z )
         {
         switch (line->line)
          {
          case z_transparent_line_mode:
          case z_opaque_line_mode:
          case z_cint_trans_nd_line_mode:
          case z_cint_trans_d_line_mode:
          case z_cint_opaq_nd_line_mode:
          case z_cint_opaq_d_line_mode:
          case z_cint_trans_nd_line_mode_red:
          case z_cint_opaq_nd_line_mode_red:
                 {
                 zoffset   = z->offset+  (x*data_ptr->sfbp$l_bytes_per_pixel + 
                                y*data_ptr->sfbp$l_bytes_per_scanline) * z->zwidth;
                
                 zactual   = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,zoffset);

                 if ( (zactual & ZMASK) != zvalue || data_ptr->sfbp$l_test_error )
                     {
                      /* Report Longword Boundary     */
                     param[0] = data_ptr->sfbp$l_vram_base + zoffset;
                     param[1] = zvalue;
                     param[2] = zactual & ZMASK ;
                     data_ptr->sfbp$l_error_type=LineZCompareError;
                     s = 0;
                     } 
                 }
           }
         }
         
        if (e<0)e+=e1;
        else
                {
                if (dxGEdy)y += y_inc ;else x += x_inc;
                e -= e2;
                } 
        if (dxGEdy)x += x_inc; else y += y_inc;


        value[R]  += color->red_increment;
        value[G]  += color->green_increment;
        value[B]  += color->blue_increment;
        
        value[R]  &= 0xfffff;
        value[G]  &= 0xfffff;
        value[B]  &= 0xfffff;
                        
        zvalue    += z->increment_whole;
        zfraction += z->increment_fraction;

    }
  }


return (s);
}


/*+
* ===========================================================================
* = sfbp$$rand - Get a random number =
* ===========================================================================
*
* OVERVIEW:
*       Get a random number
*       x = (x*a+c) MOD m
*
* FORM OF CALL:
*       sfbp$$rand (seed,low,high)
*
* ARGUMENTS:
*       seed     - seed to use for random number
*       low      - low value
*       high     - high value
* 
--*/
int sfbp$$rand (int *seed,int low,int high)

{
int     x,a,c;

#ifdef SIMULATION
*seed = (rand()) % (high-low);
#else
a = 314159;
c = 453816;
x = (*seed*a)+c+time(0);

*seed = x % (high-low);

#endif

return (*seed);
}
