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
 *      Color Frame Buffer Test Routine 
 *
 *  ABSTRACT:
 *
 *      Test Description:              
  *
 *  AUTHORS:
 *
 *      James Peacock
 *
 *
 *  CREATION DATE:      02-28-91
 *
 *  MODIFICATION HISTORY:
 *
 *--
*/

/**
** INCLUDES
**/

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
#include "cp$src:tga_sfbp_dithermatrix.h"

#define         ZPATT                   0x55555555
#define         FPATT                   0xAAAAAAAA
#define         DEPTH_INCREMENT         256
#define         FRACTION_INCREMENT      1

/* Use bottom depth when drawing on bottom of   screen  */
/* and use top depth when drawing on bottom             */

#define PROMPT() {if (data_ptr->sfbp$l_alt_console){printf("Continue ? ");getchar();printf("\n");}else getchar();} 

#define CHECK_CONFIG() {if (data_ptr->sfbp$l_module != hx_32_plane_z )\
                {printf ("ZLINE: 32 Plane Z Buffer Not Implemented \n");return(0);} \
         }

struct  line_modes_type
        {
        int     line_mode_test;
        int     line_mode;
        BEE     speed;
        SLOPE   nogo;
        FILL    method; 
        METHOD  line_address;         
        int     doz; 
        int     rgb_format;
        int     DitherFlag;    
        char    *text;
        };

typedef struct line_modes_type LINE_TEST;


static LINE_TEST line_modes[] = 
        {
        1,transparent_line_mode, slow,nogo,foreground,line_address, FALSE,     TRUE,  FALSE, "transparent slow ",
        2,transparent_line_mode, fast,go,foreground,line_address, FALSE,       TRUE,  FALSE, "transparent fast go ",    
        3,transparent_line_mode, fast,nogo,foreground,line_address, FALSE,     TRUE,  FALSE, "transparent fast nogo ",    

        4,opaque_line_mode, slow,nogo,background,line_address, FALSE,          TRUE,  FALSE, "opaque slow ",
        5,opaque_line_mode, fast,go,background,line_address, FALSE,            TRUE,  FALSE, "opaque fast go ", 	
        6,opaque_line_mode, fast,nogo,background,line_address, FALSE,          TRUE,  FALSE, "opaque fast nogo ", 	

        7,cint_nd_line_mode,fast,go,foreground,line_address, FALSE,            TRUE,  FALSE, "cint nd go", 	
        8,cint_nd_trans_line_mode_red,fast,go,foreground,line_address, FALSE,  TRUE,  FALSE, "cint nd trans data seq", 	 	
        9,cint_d_line_mode,fast,go,foreground,line_address, FALSE,             TRUE,  TRUE,  "cint d go", 	 	
       10,z_transparent_line_mode,fast,go,foreground,line_address, TRUE,       TRUE,  FALSE, "z trans ", 		
       11,z_opaque_line_mode,fast,go,background,line_address, TRUE,            TRUE,  FALSE, "z opaq  ",	
       12,z_cint_trans_nd_line_mode,fast,go,foreground,line_address, TRUE,     TRUE,  FALSE, "z cint trans nd ",
       13,z_cint_trans_d_line_mode,fast,go,foreground,line_address, TRUE,      TRUE,  TRUE,  "z cint trans d ",	
       14,z_cint_opaq_nd_line_mode,fast,go,foreground,line_address, TRUE,      TRUE,  FALSE, "z cint opaque nd ",	
       15,z_cint_opaq_d_line_mode,fast,go,foreground,line_address, TRUE,       TRUE,  TRUE,  "z cint opaque d ",	
       16,z_cint_trans_nd_line_mode_red,fast,go,foreground,line_address,TRUE,  TRUE,  FALSE, "z cint transparent nd seq",	
       17,z_cint_opaq_nd_line_mode_red,fast,go,foreground,line_address, TRUE,  TRUE,  FALSE, "z cint opaque nd seq",	
      };



#define MAX_LINES 4
#define ZBASE_TEST_OFFSET_8  (-sfbp$k_one_mill  -sfbp$k_one_mill/4) 
#define ZBASE_TEST_OFFSET_32 (-sfbp$k_one_mill  -sfbp$k_one_mill/4) 

struct XorCheckSumType 
  {
  int frequency;
  int checksum8;
  int checksum32;
  };
  
typedef struct XorCheckSumType XOR;

int     line$$dma_verify_pattern (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,
        int param [], XOR XorCheckSumTable [] ,int TableSize);

#define END_PIXEL 64

int     YLimit,XLimit;


/*+
* ===========================================================================
* = line$$xa_lt_xb_m_lt_1_test - Line Test =
* ===========================================================================
*
* OVERVIEW:
*       XA < XB and Slope < 1
* 
* FORM OF CALL:
*       line$$xa_lt_xb_m_lt_1_test  ( argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
*       XA < XB and Slope < 1
*
--*/
int  line$$xa_lt_xb_m_lt_1_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{         
int      i,d,f,k,s=1,fg=TRUE;
int      method,startx,starty,endx,endy,step;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;
union    color_value_reg INC,RED,GRN,BLU;
int      len,j,depth,num_bytes,fractional_intensity,whole_intensity;
int      dx,dy,dxGEdy,expect_depth,zoffset,fraction;

starty =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
step   =  (data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2)/data_ptr->sfbp$l_test_lines;
YLimit =  data_ptr->sfbp$l_test_zbuffer ? data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/4  : 0;
 
if (step ==0) step = 1;

line.verify      = TRUE;
line.verify_z    = TRUE;
line.boolean_op  = GXcopy;
line.span_line   = FALSE;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
line.color_change= TRUE;

visual.capend    = FALSE;
visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;

color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

RED.sfbp$l_whole = GRN.sfbp$l_whole = BLU.sfbp$l_whole = INC.sfbp$l_whole =0;

for (k=Xwindow;k<=NTwindow;k++) 
 {
 for (d=1,f=0,endy=starty;s&1 && endy>YLimit;endy-=step,
        d+=DEPTH_INCREMENT,f+=FRACTION_INCREMENT)
  {
     for (j=0;s&1 && j*sizeof(LINE_TEST)<sizeof(line_modes);j++)
        {
         /* -l1 transparent, -l2 opaque, -l3 interpolated, -l4 all z modes  */
         if (data_ptr->sfbp$l_line_test_mode == 0 ||
             line_modes[j].line_mode_test == data_ptr->sfbp$l_line_test_mode )

         {
         color.method     = line_modes[j].method;
         line.line        = line_modes[j].line_mode;
         line.nogo        = line_modes[j].nogo;
         line.speed       = line_modes[j].speed;
         line.draw_method = line_modes[j].line_address;
         line.window_type = k;
         visual.doz       = line_modes[j].doz;
         data_ptr->sfbp$l_rgb_format = line_modes[j].rgb_format;

         startx    = data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
         endx      = data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;

         line.xa          = startx;
         line.xb          = endx;
         line.ya          = starty;
         line.yb          = endy;

         z.value_fraction       = 0;
         z.value_whole          = 0;
         z.increment_fraction   = f;
         z.increment_whole      = d;
         z.stencil_value        = 0;
         z.rmask                = 0xff;
         z.wmask                = 0xff;
         z.stest                = always;
         z.sfail                = replace;
         z.zfail                = replace;
         z.zpass                = replace;
         z.ztest                = always;
         z.replace              = replace;
         z.zwidth               = data_ptr->sfbp$l_module == hx_8_plane ? 4 : 1;


         if (data_ptr->sfbp$l_module == hx_8_plane ||
             data_ptr->sfbp$l_module == hx_32_plane_no_z )
                {
                z.offset   = data_ptr->sfbp$l_module == hx_8_plane ? ZBASE_TEST_OFFSET_8 : ZBASE_TEST_OFFSET_32;
                z.base_addr= z.offset + 
                                (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                 line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
                                                
                }
               
         else if (data_ptr->sfbp$l_module == hx_32_plane_z)
              {
              z.offset        = 0;
              z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 + z.offset +
                                         (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                          line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
              }                                                  

         RED.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         RED.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         RED.sfbp$r_color_value_bits.sfbp$v_dither   = starty&31;
        
         GRN.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         GRN.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         GRN.sfbp$r_color_value_bits.sfbp$v_dither   = startx&31;
        
         BLU.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         BLU.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         
         color.red_value   = RED.sfbp$l_whole;
         color.green_value = GRN.sfbp$l_whole;
         color.blue_value  = BLU.sfbp$l_whole;

         dx          = endx-startx;
         dy          = endy-starty;
         dxGEdy      = lineabs(dx) >= lineabs(dy) ? TRUE : FALSE;
         fractional_intensity = (4096 * 256) / (dxGEdy ? lineabs(endx-startx) : lineabs (endy-starty));
         whole_intensity      = 0;

         INC.sfbp$r_color_value_bits.sfbp$v_fraction = fractional_intensity;
         INC.sfbp$r_color_value_bits.sfbp$v_value    = whole_intensity;
 
         color.red_increment   = INC.sfbp$l_whole;
         color.green_increment = 0;
         color.blue_increment  = 0;

         if (data_ptr->sfbp$l_print) {printf ("%s\n",line_modes[j].text);}

         /* Dither Mode Tests Are Only For 8 Plane Systems      */
         if (data_ptr->sfbp$l_planes == 8 || 
            (data_ptr->sfbp$l_planes != 8 && !line_modes[j].DitherFlag ))
            {
            /* Skip the Z buffer Test Unless Requested to(8 and 32 no z) */
            if (!line_modes[j].doz || (data_ptr->sfbp$l_test_zbuffer && line_modes[j].doz))
             {
             s= (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
             s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
             }
            }
         }
     }
  }
}
 
return (s);
}



/*+
* ===========================================================================
* = line$$xa_gt_xb_m_lt_1_test - Line Test =
* ===========================================================================
*
* OVERVIEW:
*       XA > XB  Slope < 1
* 
* FORM OF CALL:
*       line$$xa_gt_xb_m_lt_1_test  ( argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int  line$$xa_gt_xb_m_lt_1_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      i,d,f,k,s=1,fg=TRUE;
int      method,startx,starty,endx,endy,step;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;
union    color_value_reg INC,RED,GRN,BLU;
int      j,depth,num_bytes,fractional_intensity,whole_intensity;
int      dx,dy,dxGEdy,expect_depth,zoffset,fraction;


starty  =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
step    =  (data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2)/data_ptr->sfbp$l_test_lines;
YLimit  =  data_ptr->sfbp$l_test_zbuffer ? data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/4  : 0;

if (step ==0) step = 1;

line.verify      = TRUE;
line.verify_z    = TRUE;
line.boolean_op  = GXcopy;
line.span_line   = FALSE;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
line.color_change= TRUE;

visual.capend    = FALSE;
visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;

color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

RED.sfbp$l_whole = GRN.sfbp$l_whole = BLU.sfbp$l_whole = INC.sfbp$l_whole =0;

for (k=Xwindow;k<=NTwindow;k++)
 {
 for (d=1,f=0,s=1,endy=starty;s&1 && endy>YLimit;endy-=step,
        d+=DEPTH_INCREMENT,f+=FRACTION_INCREMENT)
 {
     for (j=0;s&1 && j*sizeof(LINE_TEST)<sizeof(line_modes);j++)
        {

         /* -l1 transparent, -l2 opaque, -l3 interpolated, -l4 all z modes  */
         if (data_ptr->sfbp$l_line_test_mode == 0 ||
             line_modes[j].line_mode_test == data_ptr->sfbp$l_line_test_mode )

         {
         color.method     = line_modes[j].method;
         line.line        = line_modes[j].line_mode;
         line.nogo        = line_modes[j].nogo;
         line.speed       = line_modes[j].speed;
         line.draw_method = line_modes[j].line_address;
         line.window_type = k;
         visual.doz       = line_modes[j].doz;
         data_ptr->sfbp$l_rgb_format = line_modes[j].rgb_format;

         startx           = data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;  
         endx             = 0;

         line.xa          = startx;
         line.xb          = endx;
         line.ya          = starty;
         line.yb          = endy;

         z.value_fraction       = 0;
         z.value_whole          = 0;
         z.increment_fraction   = f;
         z.increment_whole      = d;
         z.stencil_value        = 0;
         z.rmask                = 0xff;
         z.wmask                = 0xff;
         z.stest                = always;
         z.sfail                = replace;
         z.zfail                = replace;
         z.zpass                = replace;
         z.ztest                = always;
         z.replace              = replace;
         z.zwidth               = data_ptr->sfbp$l_module == hx_8_plane ? 4 : 1;

         if (data_ptr->sfbp$l_module == hx_8_plane ||
             data_ptr->sfbp$l_module == hx_32_plane_no_z )
                {
                z.offset   = data_ptr->sfbp$l_module == hx_8_plane ? ZBASE_TEST_OFFSET_8 : ZBASE_TEST_OFFSET_32;
                z.base_addr= z.offset + 
                                (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                 line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
                }
               
         else if (data_ptr->sfbp$l_module == hx_32_plane_z)
              {
              z.offset        = 0;
              z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 + z.offset +
                                         (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                          line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
              }                                                  

         RED.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         RED.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         RED.sfbp$r_color_value_bits.sfbp$v_dither   = starty&31;
        
         GRN.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         GRN.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         GRN.sfbp$r_color_value_bits.sfbp$v_dither   = startx&31;
        
         BLU.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         BLU.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         
         color.red_value   = RED.sfbp$l_whole;
         color.green_value = GRN.sfbp$l_whole;
         color.blue_value  = BLU.sfbp$l_whole;

         dx          = endx-startx;
         dy          = endy-starty;
         dxGEdy      = lineabs(dx) >= lineabs(dy) ? TRUE : FALSE;
         fractional_intensity = (4096 * 256) / (dxGEdy ? lineabs(endx-startx) : lineabs (endy-starty));
         whole_intensity      = 0;

         INC.sfbp$r_color_value_bits.sfbp$v_fraction = fractional_intensity;
         INC.sfbp$r_color_value_bits.sfbp$v_value    = whole_intensity;
 
         color.red_increment   = 0;
         color.green_increment = INC.sfbp$l_whole; 
         color.blue_increment  = 0;

         if (data_ptr->sfbp$l_print) {printf ("%s\n",line_modes[j].text);}

         /* Dither Mode Tests Are Only For 8 Plane Systems      */
         if (data_ptr->sfbp$l_planes == 8 || 
            (data_ptr->sfbp$l_planes != 8 && !line_modes[j].DitherFlag ))
            {
            if (!line_modes[j].doz || (data_ptr->sfbp$l_test_zbuffer && line_modes[j].doz))
             {
             s= (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
             s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
             }
            }
         }

    }
  }
}
             
return (s);
}


/*+
* ===========================================================================
* = line$$xa_lt_xb_m_gt_1_test - Line Test =
* ===========================================================================
*
* OVERVIEW:
*       Xa < Xb and slope > 1
* 
* FORM OF CALL:
*       line$$xa_lt_xb_m_gt_1_test  ( argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int  line$$xa_lt_xb_m_gt_1_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      i,d,f,k,s=1,fg=TRUE;
int	 method,startx,starty,endx,endy,step;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;
union    color_value_reg INC,RED,GRN,BLU;
int      j,depth,num_bytes,fractional_intensity,whole_intensity;
int      dx,dy,dxGEdy,expect_depth,zoffset,fraction;

startx        =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
step          =  (data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2)/data_ptr->sfbp$l_test_lines;

if (step ==0) step = 1;

line.verify      = TRUE;
line.verify_z    = TRUE;
line.boolean_op  = GXcopy;
line.span_line   = FALSE;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
line.color_change= TRUE;

visual.capend    = FALSE;
visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;

color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

RED.sfbp$l_whole = GRN.sfbp$l_whole = BLU.sfbp$l_whole = INC.sfbp$l_whole =0;

for (k=Xwindow;k<=NTwindow;k++)
 {
 for (d=1,f=0,s=1,endx=startx;s&1 && endx<data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;endx+=step,
                d+=DEPTH_INCREMENT,f+=FRACTION_INCREMENT)
 {
     for (j=0;s&1 && j*sizeof(LINE_TEST)<sizeof(line_modes);j++)
        {

         /* -l1 transparent, -l2 opaque, -l3 interpolated, -l4 all z modes  */
         if (data_ptr->sfbp$l_line_test_mode == 0 ||
             line_modes[j].line_mode_test == data_ptr->sfbp$l_line_test_mode )

          {
          color.method     = line_modes[j].method;
          line.line        = line_modes[j].line_mode;
          line.nogo        = line_modes[j].nogo;
          line.speed       = line_modes[j].speed;
          line.draw_method = line_modes[j].line_address;
          line.window_type = k;
          visual.doz       = line_modes[j].doz;
          data_ptr->sfbp$l_rgb_format = line_modes[j].rgb_format;

          starty           = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;

          if (data_ptr->sfbp$l_test_zbuffer)endy = starty-END_PIXEL;
          else endy  = 0;

          line.ya          = starty;
          line.yb          = endy;
          line.xb          = endx;
          line.xa          = startx;
          
          z.value_fraction       = 0;
          z.value_whole          = 0;
          z.increment_fraction   = f;
          z.increment_whole      = d;
          z.stencil_value        = 0;
          z.rmask                = 0xff;
          z.wmask                = 0xff;
          z.stest                = always;
          z.sfail                = replace;
          z.zfail                = replace;
          z.zpass                = replace;
          z.ztest                = always;
          z.replace              = replace;
          z.zwidth               = data_ptr->sfbp$l_module == hx_8_plane ? 4 : 1;

          if (data_ptr->sfbp$l_module == hx_8_plane ||
              data_ptr->sfbp$l_module == hx_32_plane_no_z )
                {
                z.offset   = data_ptr->sfbp$l_module == hx_8_plane ? ZBASE_TEST_OFFSET_8 : ZBASE_TEST_OFFSET_32;
                z.base_addr= z.offset + 
                                (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                 line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
                }
               
          else if (data_ptr->sfbp$l_module == hx_32_plane_z)
              {
              z.offset        = 0;
              z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 + z.offset +
                                         (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                          line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
              }                                                  

          RED.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
          RED.sfbp$r_color_value_bits.sfbp$v_value    = 0;
          RED.sfbp$r_color_value_bits.sfbp$v_dither   = starty&31;
        
          GRN.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
          GRN.sfbp$r_color_value_bits.sfbp$v_value    = 0;
          GRN.sfbp$r_color_value_bits.sfbp$v_dither   = startx&31;
        
          BLU.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
          BLU.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         
          color.red_value   = RED.sfbp$l_whole;
          color.green_value = GRN.sfbp$l_whole;
          color.blue_value  = BLU.sfbp$l_whole;

          dx          = endx-startx;
          dy          = endy-starty;
          dxGEdy      = lineabs(dx) >= lineabs(dy) ? TRUE : FALSE;
          fractional_intensity = (4096 * 256) / (dxGEdy ? lineabs(endx-startx) : lineabs (endy-starty));
          whole_intensity      = 0;

          INC.sfbp$r_color_value_bits.sfbp$v_fraction = fractional_intensity;
          INC.sfbp$r_color_value_bits.sfbp$v_value    = whole_intensity;
 
          color.red_increment   = 0;
          color.green_increment = 0;
          color.blue_increment  = INC.sfbp$l_whole;

          if (data_ptr->sfbp$l_print) {printf ("%s\n",line_modes[j].text);}
          /* Dither Mode Tests Are Only For 8 Plane Systems      */
          if (data_ptr->sfbp$l_planes == 8 || 
            (data_ptr->sfbp$l_planes != 8 && !line_modes[j].DitherFlag ))
            {
            if (!line_modes[j].doz || (data_ptr->sfbp$l_test_zbuffer && line_modes[j].doz))
             {
             s= (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
             s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
             }
            }
          }

        }
    }        
  }
 return (s);
}


/*+
* ===========================================================================
* = line$$xa_gt_xb_m_gt_1_test - Line Test =
* ===========================================================================
*
* OVERVIEW:
*       Xa > Xb Slope > 1
* 
* FORM OF CALL:
*       line$$xa_gt_xb_m_gt_1_test  ( argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int  line$$xa_gt_xb_m_gt_1_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      i,d,f,k,s=1,fg=TRUE;
int      method,startx,starty,endx,endy,step;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;
union    color_value_reg INC,RED,GRN,BLU;
int      j,depth,num_bytes,fractional_intensity,whole_intensity;
int      dx,dy,dxGEdy,expect_depth,zoffset,fraction;

startx        =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
step          =  (data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2)/data_ptr->sfbp$l_test_lines;

if (step ==0) step = 1;

line.verify      = TRUE;
line.verify_z    = TRUE;
line.boolean_op  = GXcopy;
line.span_line   = FALSE;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
line.color_change= TRUE;


visual.capend    = FALSE;
visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;

color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

RED.sfbp$l_whole = GRN.sfbp$l_whole = BLU.sfbp$l_whole = INC.sfbp$l_whole =0;

for (k=Xwindow;k<=NTwindow;k++)
 {
 for (d=1,f=0,s=1,endx=startx;s&1 && endx>0;endx-=step,
                d+=DEPTH_INCREMENT,f+=FRACTION_INCREMENT)

 {
     for (j=0;s&1 && j*sizeof(LINE_TEST)<sizeof(line_modes);j++)
        {

         /* -l1 transparent, -l2 opaque, -l3 interpolated, -l4 all z modes  */
         if (data_ptr->sfbp$l_line_test_mode == 0 ||
             line_modes[j].line_mode_test == data_ptr->sfbp$l_line_test_mode )

         {
         color.method     = line_modes[j].method;
         line.line        = line_modes[j].line_mode;
         line.nogo        = line_modes[j].nogo;
         line.speed       = line_modes[j].speed;
         line.draw_method = line_modes[j].line_address;
         line.window_type = k;
         visual.doz       = line_modes[j].doz;
         data_ptr->sfbp$l_rgb_format = line_modes[j].rgb_format;

         starty           = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;

         if (data_ptr->sfbp$l_test_zbuffer)endy = starty-END_PIXEL;
         else endy  = 0;

         line.ya          = starty;
         line.yb          = endy;
         line.xb          = endx;
         line.xa          = startx;

         z.value_fraction       = 0;
         z.value_whole          = 0;
         z.increment_fraction   = f;
         z.increment_whole      = d;
         z.stencil_value        = 0;
         z.rmask                = 0xff;
         z.wmask                = 0xff;
         z.stest                = always;
         z.sfail                = replace;
         z.zfail                = replace;
         z.zpass                = replace;
         z.ztest                = always;
         z.replace              = replace;
         z.zwidth               = data_ptr->sfbp$l_module == hx_8_plane ? 4 : 1;

         if (data_ptr->sfbp$l_module == hx_8_plane ||
             data_ptr->sfbp$l_module == hx_32_plane_no_z )
                {
                z.offset   = data_ptr->sfbp$l_module == hx_8_plane ? ZBASE_TEST_OFFSET_8 : ZBASE_TEST_OFFSET_32;
                z.base_addr= z.offset + 
                                (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                 line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
                }
               
         else if (data_ptr->sfbp$l_module == hx_32_plane_z)
              {
              z.offset        = 0;
              z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 + z.offset +
                                         (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                          line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
              }                                                  

         RED.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         RED.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         RED.sfbp$r_color_value_bits.sfbp$v_dither   = starty&31;
        
         GRN.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         GRN.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         GRN.sfbp$r_color_value_bits.sfbp$v_dither   = startx&31;
        
         BLU.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         BLU.sfbp$r_color_value_bits.sfbp$v_value    = 0;
        
         color.red_value   = RED.sfbp$l_whole;
         color.green_value = GRN.sfbp$l_whole;
         color.blue_value  = BLU.sfbp$l_whole;

         whole_intensity      = 0;
         INC.sfbp$r_color_value_bits.sfbp$v_value    = whole_intensity;

         dx          = endx-startx;
         dy          = endy-starty;
         dxGEdy      = lineabs(dx) >= lineabs(dy) ? TRUE : FALSE;
         fractional_intensity = (4096 * 256) / (dxGEdy ? lineabs(endx-startx) : lineabs (endy-starty));

         INC.sfbp$r_color_value_bits.sfbp$v_fraction = fractional_intensity;

         color.red_increment   = INC.sfbp$l_whole;
         color.green_increment = 0;
         color.blue_increment  = 0;

         if (data_ptr->sfbp$l_print) {printf ("%s\n",line_modes[j].text);}
         /* Dither Mode Tests Are Only For 8 Plane Systems      */
         if (data_ptr->sfbp$l_planes == 8 || 
            (data_ptr->sfbp$l_planes != 8 && !line_modes[j].DitherFlag ))
            {
            if (!line_modes[j].doz || (data_ptr->sfbp$l_test_zbuffer && line_modes[j].doz))
             {
             s= (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
             s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
             }
            }
         }
     }
   }
 } 
return (s);
}


/*+
* ===========================================================================
* = line$$xa_lt_xb_m_lt_m1_test - Line Test =
* ===========================================================================
*
* OVERVIEW:
*       Xa < Xb and slope < -1
* 
* FORM OF CALL:
*       line$$xa_lt_xb_m_lt_m1_test  ( argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int  line$$xa_lt_xb_m_lt_m1_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      i,d,f,k,s=1,fg=FALSE;
int      method,startx,starty,endx,endy,step;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;
union    color_value_reg INC,RED,GRN,BLU;
int      j,depth,num_bytes,fractional_intensity,whole_intensity;
int      dx,dy,dxGEdy,expect_depth,zoffset,fraction;

starty        =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
step          = (data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2)/data_ptr->sfbp$l_test_lines;

if (data_ptr->sfbp$l_test_zbuffer)
 YLimit = starty + data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/8;
else
 YLimit = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines;
        
if (step ==0) step = 1;

line.verify      = TRUE;
line.verify_z    = TRUE;
line.boolean_op  = GXcopy;
line.span_line   = FALSE;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
line.color_change= TRUE;

visual.capend    = FALSE;
visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;

color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

RED.sfbp$l_whole = GRN.sfbp$l_whole = BLU.sfbp$l_whole = INC.sfbp$l_whole =0;


for (k=Xwindow;k<=NTwindow;k++)
 {
 for (s=1,d=1,f=0,endy=starty;s&1 && endy<YLimit;endy+=step,
        d+=DEPTH_INCREMENT,f+=FRACTION_INCREMENT)
 {
     for (j=0;s&1 && j*sizeof(LINE_TEST)<sizeof(line_modes);j++)
        {

         /* -l1 transparent, -l2 opaque, -l3 interpolated, -l4 all z modes  */
         if (data_ptr->sfbp$l_line_test_mode == 0 ||
             line_modes[j].line_mode_test == data_ptr->sfbp$l_line_test_mode )

         {
         color.method     = line_modes[j].method;
         line.line        = line_modes[j].line_mode;
         line.nogo        = line_modes[j].nogo;
         line.speed       = line_modes[j].speed;
         line.draw_method = line_modes[j].line_address;
         line.window_type = k;
         visual.doz       = line_modes[j].doz;
         data_ptr->sfbp$l_rgb_format = line_modes[j].rgb_format;

         startx  = data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
         endx    = data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;

         line.xa          = startx;
         line.xb          = endx;
         line.yb          = endy;
         line.ya          = starty;

         z.value_fraction       = 0;
         z.value_whole          = 0;
         z.increment_fraction   = f;
         z.increment_whole      = d;
         z.stencil_value        = 0;
         z.rmask                = 0xff;
         z.wmask                = 0xff;
         z.stest                = always;
         z.sfail                = replace;
         z.zfail                = replace;
         z.zpass                = replace;
         z.ztest                = always;
         z.replace              = replace;
         z.zwidth               = data_ptr->sfbp$l_module == hx_8_plane ? 4 : 1;

         if (data_ptr->sfbp$l_module == hx_8_plane ||
             data_ptr->sfbp$l_module == hx_32_plane_no_z )
                {
                z.offset   = data_ptr->sfbp$l_module == hx_8_plane ? ZBASE_TEST_OFFSET_8 : ZBASE_TEST_OFFSET_32;
                z.base_addr= z.offset + 
                                (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                 line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
                }
               
         else if (data_ptr->sfbp$l_module == hx_32_plane_z)
              {
              z.offset        = 0;
              z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 + z.offset +
                                         (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                          line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
              }                                                  

         RED.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         RED.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         RED.sfbp$r_color_value_bits.sfbp$v_dither   = starty&31;
        
         GRN.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         GRN.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         GRN.sfbp$r_color_value_bits.sfbp$v_dither   = startx&31;
        
         BLU.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         BLU.sfbp$r_color_value_bits.sfbp$v_value    = 0;
        
         color.red_value   = RED.sfbp$l_whole;
         color.green_value = GRN.sfbp$l_whole;
         color.blue_value  = BLU.sfbp$l_whole;

         whole_intensity      = 0;
         INC.sfbp$r_color_value_bits.sfbp$v_value    = whole_intensity;

         dx          = endx-startx;
         dy          = endy-starty;
         dxGEdy      = lineabs(dx) >= lineabs(dy) ? TRUE : FALSE;
         fractional_intensity = (4096 * 256) / (dxGEdy ? lineabs(endx-startx) : lineabs (endy-starty));

         INC.sfbp$r_color_value_bits.sfbp$v_fraction = fractional_intensity;
         color.red_increment   = INC.sfbp$l_whole;
         color.green_increment = 0;
         color.blue_increment  = 0;

         if (data_ptr->sfbp$l_print) {printf ("%s\n",line_modes[j].text);}
         /* Dither Mode Tests Are Only For 8 Plane Systems      */
         if (data_ptr->sfbp$l_planes == 8 || 
            (data_ptr->sfbp$l_planes != 8 && !line_modes[j].DitherFlag ))
            {
            if (!line_modes[j].doz || (data_ptr->sfbp$l_test_zbuffer && line_modes[j].doz))
             {
             s= (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
             s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
             }
            }
         }
      }
    }
 }   
return (s);
}


/*+
* ===========================================================================
* = line$$xa_gt_xb_m_lt_m1_test - Line Test =
* ===========================================================================
*
* OVERVIEW:
*       Xa > Xb and slope < -1 
* 
* FORM OF CALL:
*       line$$xa_gt_xb_m_lt_m1_test  ( argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int  line$$xa_gt_xb_m_lt_m1_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      i,d,f,k,s=1,fg=FALSE;
int      method,startx,starty,endx,endy,step;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;
union    color_value_reg INC,RED,GRN,BLU;
int      j,depth,num_bytes,fractional_intensity,whole_intensity;
int      dx,dy,dxGEdy,expect_depth,zoffset,fraction;

if (step ==0) step = 1;

starty  =   data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2 ;
step    =  (data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2)/data_ptr->sfbp$l_test_lines;

if (data_ptr->sfbp$l_test_zbuffer)
 YLimit = starty + data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/8;
else
 YLimit = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines;

if (step ==0) step = 1;
line.verify      = TRUE;
line.verify_z    = TRUE;
line.boolean_op  = GXcopy;
line.span_line   = FALSE;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
line.color_change= TRUE;

visual.capend    = FALSE;
visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;

color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

RED.sfbp$l_whole = GRN.sfbp$l_whole = BLU.sfbp$l_whole = INC.sfbp$l_whole =0;

for (k=Xwindow;k<=NTwindow;k++)
 {
 for (s=1,d=1,f=0,endy=starty;s&1 && endy<YLimit;endy+=step,
        d+=DEPTH_INCREMENT,f+=FRACTION_INCREMENT)
 {
     for (j=0;s&1 && j*sizeof(LINE_TEST)<sizeof(line_modes);j++)
        {

         /* -l1 transparent, -l2 opaque, -l3 interpolated, -l4 all z modes  */
         if (data_ptr->sfbp$l_line_test_mode == 0 ||
             line_modes[j].line_mode_test == data_ptr->sfbp$l_line_test_mode )

         {
         color.method     = line_modes[j].method;
         line.line        = line_modes[j].line_mode;
         line.nogo        = line_modes[j].nogo;
         line.speed       = line_modes[j].speed;
         line.draw_method = line_modes[j].line_address;
         line.window_type = k;
         visual.doz       = line_modes[j].doz;
         data_ptr->sfbp$l_rgb_format = line_modes[j].rgb_format;

         startx           = data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
         endx             = 0;

         line.xa          = startx;
         line.xb          = endx;
         line.yb          = endy;
         line.ya          = starty;

         z.value_fraction       = 0;
         z.value_whole          = 0;
         z.increment_fraction   = f;
         z.increment_whole      = d;
         z.stencil_value        = 0;
         z.rmask                = 0xff;
         z.wmask                = 0xff;
         z.stest                = always;
         z.sfail                = replace;
         z.zfail                = replace;
         z.zpass                = replace;
         z.ztest                = always;
         z.replace              = replace;
         z.zwidth               = data_ptr->sfbp$l_module == hx_8_plane ? 4 : 1;

         if (data_ptr->sfbp$l_module == hx_8_plane ||
             data_ptr->sfbp$l_module == hx_32_plane_no_z )
                {
                z.offset   = data_ptr->sfbp$l_module == hx_8_plane ? ZBASE_TEST_OFFSET_8 : ZBASE_TEST_OFFSET_32;
                z.base_addr= z.offset + 
                                (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                 line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
                }
               
         else if (data_ptr->sfbp$l_module == hx_32_plane_z)
              {
              z.offset        = 0;
              z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 + z.offset +
                                         (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                          line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
              }                                                  

         RED.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         RED.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         RED.sfbp$r_color_value_bits.sfbp$v_dither   = starty&31;
        
         GRN.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         GRN.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         GRN.sfbp$r_color_value_bits.sfbp$v_dither   = startx&31;
        
         BLU.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         BLU.sfbp$r_color_value_bits.sfbp$v_value    = 0;
        
         color.red_value   = RED.sfbp$l_whole;
         color.green_value = GRN.sfbp$l_whole;
         color.blue_value  = BLU.sfbp$l_whole;

         whole_intensity      = 0;
         INC.sfbp$r_color_value_bits.sfbp$v_value    = whole_intensity;

         color.red_increment   = 0;

         dx          = endx-startx;
         dy          = endy-starty;
         dxGEdy      = lineabs(dx) >= lineabs(dy) ? TRUE : FALSE;
         fractional_intensity = (4096 * 256) / (dxGEdy ? lineabs(endx-startx) : lineabs (endy-starty));

         INC.sfbp$r_color_value_bits.sfbp$v_fraction = fractional_intensity;
         color.green_increment = INC.sfbp$l_whole;
         color.blue_increment  = 0;


         if (data_ptr->sfbp$l_print) {printf ("%s\n",line_modes[j].text);}
         /* Dither Mode Tests Are Only For 8 Plane Systems      */
         if (data_ptr->sfbp$l_planes == 8 || 
            (data_ptr->sfbp$l_planes != 8 && !line_modes[j].DitherFlag ))
            {
            if (!line_modes[j].doz || (data_ptr->sfbp$l_test_zbuffer && line_modes[j].doz))
             {
             s= (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
             s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
             }
            }
         }
      }
    }
 }   
  
return (s);
}


/*+
* ===========================================================================
* = line$$xa_lt_xb_m_gt_m1_test - Line Test =
* ===========================================================================
*
* OVERVIEW:
*       Xa < Xb and slope > -1 
* 
* FORM OF CALL:
*       line$$xa_lt_xb_m_gt_m1_test  ( argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int  line$$xa_lt_xb_m_gt_m1_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      i,d,f,k,s=1,fg=FALSE;
int      method,startx,starty,endx,endy,step;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;
union    color_value_reg INC,RED,GRN,BLU;
int      j,depth,num_bytes,fractional_intensity,whole_intensity;
int      dx,dy,dxGEdy,expect_depth,zoffset,fraction;


startx        =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
step          =  (data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2)/data_ptr->sfbp$l_test_lines;
if (step ==0) step = 1;

line.verify      = TRUE;
line.verify_z    = TRUE;
line.boolean_op  = GXcopy;
line.span_line   = FALSE;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
line.color_change= TRUE;

visual.capend    = FALSE;
visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;

color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

RED.sfbp$l_whole = GRN.sfbp$l_whole = BLU.sfbp$l_whole = INC.sfbp$l_whole =0;

for (k=Xwindow;k<=NTwindow;k++)
 {
 for (s=1,d=1,f=0,endx=startx;s&1 && endx<data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;endx+=step,
        d+=DEPTH_INCREMENT,f+=FRACTION_INCREMENT)
 {
     for (j=0;s&1 && j*sizeof(LINE_TEST)<sizeof(line_modes);j++)
        {

         /* -l1 transparent, -l2 opaque, -l3 interpolated, -l4 all z modes  */
          if (data_ptr->sfbp$l_line_test_mode == 0 ||
             line_modes[j].line_mode_test == data_ptr->sfbp$l_line_test_mode )

          {
          color.method     = line_modes[j].method;
          line.line        = line_modes[j].line_mode;
          line.nogo        = line_modes[j].nogo;
          line.speed       = line_modes[j].speed;
          line.draw_method = line_modes[j].line_address;
          line.window_type = k;
          visual.doz       = line_modes[j].doz;
          data_ptr->sfbp$l_rgb_format = line_modes[j].rgb_format;

          starty   = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2 ;
          endy     = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines   ;

          if (data_ptr->sfbp$l_test_zbuffer)endy = starty+END_PIXEL;
          else endy     = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines;

          line.ya          = starty;
          line.yb          = endy;
          line.xb          = endx;
          line.xa          = startx;

          z.value_fraction       = 0;
          z.value_whole          = 0;
          z.increment_fraction   = f;
          z.increment_whole      = d;
          z.stencil_value        = 0;
          z.rmask                = 0xff;
          z.wmask                = 0xff;
          z.stest                = always;
          z.sfail                = replace;
          z.zfail                = replace;
          z.zpass                = replace;
          z.ztest                = always;
          z.replace              = replace;
          z.zwidth               = data_ptr->sfbp$l_module == hx_8_plane ? 4 : 1;

          if (data_ptr->sfbp$l_module == hx_8_plane ||
              data_ptr->sfbp$l_module == hx_32_plane_no_z )
                {
                z.offset   = data_ptr->sfbp$l_module == hx_8_plane ? ZBASE_TEST_OFFSET_8 : ZBASE_TEST_OFFSET_32;
                z.base_addr= z.offset + 
                                (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                 line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
                }
               
          else if (data_ptr->sfbp$l_module == hx_32_plane_z)
              {
              z.offset        = 0;
              z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 + z.offset +
                                         (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                          line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
              }                                                  

          RED.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
          RED.sfbp$r_color_value_bits.sfbp$v_value    = 0;
          RED.sfbp$r_color_value_bits.sfbp$v_dither   = starty&31;
         
          GRN.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
          GRN.sfbp$r_color_value_bits.sfbp$v_value    = 0;
          GRN.sfbp$r_color_value_bits.sfbp$v_dither   = startx&31;
        
          BLU.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
          BLU.sfbp$r_color_value_bits.sfbp$v_value    = 0;
        
          color.red_value   = RED.sfbp$l_whole;
          color.green_value = GRN.sfbp$l_whole;
          color.blue_value  = BLU.sfbp$l_whole;

          whole_intensity      = 0;
          INC.sfbp$r_color_value_bits.sfbp$v_value    = whole_intensity;
 
          dx          = endx-startx;
          dy          = endy-starty;
          dxGEdy      = lineabs(dx) >= lineabs(dy) ? TRUE : FALSE;
          fractional_intensity = (4096 * 256) / (dxGEdy ? lineabs(endx-startx) : lineabs (endy-starty));

          INC.sfbp$r_color_value_bits.sfbp$v_fraction = fractional_intensity;

          color.blue_increment   = INC.sfbp$l_whole;
          color.red_increment    = 0;
          color.green_increment  = 0;

          if (data_ptr->sfbp$l_print) {printf ("%s\n",line_modes[j].text);}
         /* Dither Mode Tests Are Only For 8 Plane Systems      */
         if (data_ptr->sfbp$l_planes == 8 || 
            (data_ptr->sfbp$l_planes != 8 && !line_modes[j].DitherFlag ))
            {
            if (!line_modes[j].doz || (data_ptr->sfbp$l_test_zbuffer && line_modes[j].doz))
             {
             s= (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
             s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
             }
            }
          }
         }
       }
  }      
return (s);
}


/*+
* ===========================================================================
* = line$$xa_gt_xb_m_gt_m1_test - Line Test =
* ===========================================================================
*
* OVERVIEW:
*       Xa > Xb and slope > -1
* 
* FORM OF CALL:
*       line$$xa_gt_xb_m_gt_m1_test  ( argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int  line$$xa_gt_xb_m_gt_m1_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      i,d,f,k,s=1,fg=FALSE;
int      method,startx,starty,endx,endy,step;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;
union    color_value_reg INC,RED,GRN,BLU;
int      j,depth,num_bytes,fractional_intensity,whole_intensity;
int      dx,dy,dxGEdy,expect_depth,zoffset,fraction;

startx        =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
step          =  (data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2)/data_ptr->sfbp$l_test_lines;

if (step ==0) step = 1;

line.verify      = TRUE;
line.verify_z    = TRUE;
line.boolean_op  = GXcopy;
line.span_line   = FALSE;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
line.color_change= TRUE;

visual.capend    = FALSE;
visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;

color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

RED.sfbp$l_whole = GRN.sfbp$l_whole = BLU.sfbp$l_whole = INC.sfbp$l_whole =0;

for (k=Xwindow;k<=NTwindow;k++)
 {
 for (s=1,d=1,f=0,endx=startx;s&1 && endx>0;endx-=step,
        d+=DEPTH_INCREMENT,f+=FRACTION_INCREMENT)
 {
     for (j=0;s&1 && j*sizeof(LINE_TEST)<sizeof(line_modes);j++)
        {

         /* -l1 transparent, -l2 opaque, -l3 interpolated, -l4 all z modes  */
         if (data_ptr->sfbp$l_line_test_mode == 0 ||
             line_modes[j].line_mode_test == data_ptr->sfbp$l_line_test_mode )

         {
         color.method     = line_modes[j].method;
         line.line        = line_modes[j].line_mode;
         line.nogo        = line_modes[j].nogo;
         line.speed       = line_modes[j].speed;
         line.draw_method = line_modes[j].line_address;
         line.window_type = k;
         visual.doz       = line_modes[j].doz;
         data_ptr->sfbp$l_rgb_format = line_modes[j].rgb_format;

         starty           = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2 ;
         endy             = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines   ;

         if (data_ptr->sfbp$l_test_zbuffer)endy = starty+END_PIXEL;
         else endy     = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines   ;

         line.ya          = starty;
         line.yb          = endy;
         line.xb          = endx;
         line.xa          = startx;

         z.value_fraction       = 0;
         z.value_whole          = 0;
         z.increment_fraction   = f;
         z.increment_whole      = d;
         z.stencil_value        = 0;
         z.rmask                = 0xff;
         z.wmask                = 0xff;
         z.stest                = always;
         z.sfail                = replace;
         z.zfail                = replace;
         z.zpass                = replace;
         z.ztest                = always;
         z.replace              = replace;
         z.zwidth               = data_ptr->sfbp$l_module == hx_8_plane ? 4 : 1;

         if (data_ptr->sfbp$l_module == hx_8_plane ||
              data_ptr->sfbp$l_module == hx_32_plane_no_z )
                {
                z.offset   = data_ptr->sfbp$l_module == hx_8_plane ? ZBASE_TEST_OFFSET_8 : ZBASE_TEST_OFFSET_32;
                z.base_addr= z.offset + 
                                (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                 line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
                }
               
         else if (data_ptr->sfbp$l_module == hx_32_plane_z)
              {
              z.offset        = 0;
              z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 + z.offset +
                                         (line.xa*data_ptr->sfbp$l_bytes_per_pixel + 
                                          line.ya*data_ptr->sfbp$l_bytes_per_scanline) * z.zwidth;
              }                                                  

         RED.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         RED.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         RED.sfbp$r_color_value_bits.sfbp$v_dither   = starty&31;
        
         GRN.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         GRN.sfbp$r_color_value_bits.sfbp$v_value    = 0;
         GRN.sfbp$r_color_value_bits.sfbp$v_dither   = startx&31;
        
         BLU.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
         BLU.sfbp$r_color_value_bits.sfbp$v_value    = 0;
        
         color.red_value   = RED.sfbp$l_whole;
         color.green_value = GRN.sfbp$l_whole;
         color.blue_value  = BLU.sfbp$l_whole;

         whole_intensity      = 0;
         INC.sfbp$r_color_value_bits.sfbp$v_value    = whole_intensity;

         dx          = endx-startx;
         dy          = endy-starty;
         dxGEdy      = lineabs(dx) >= lineabs(dy) ? TRUE : FALSE;
         fractional_intensity = (4096 * 256) / (dxGEdy ? lineabs(endx-startx) : lineabs (endy-starty));

         INC.sfbp$r_color_value_bits.sfbp$v_fraction = fractional_intensity;
         color.red_increment   = INC.sfbp$l_whole;
         color.green_increment = INC.sfbp$l_whole;
         color.blue_increment  = INC.sfbp$l_whole;

         if (data_ptr->sfbp$l_print) {printf ("%s\n",line_modes[j].text);}
         /* Dither Mode Tests Are Only For 8 Plane Systems      */
         if (data_ptr->sfbp$l_planes == 8 || 
            (data_ptr->sfbp$l_planes != 8 && !line_modes[j].DitherFlag ))
            {
            if (!line_modes[j].doz || (data_ptr->sfbp$l_test_zbuffer && line_modes[j].doz))
             {
             s= (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
             s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
             }
            }
         }
        }
       }
 }      
return (s);
}


/*+
* ===========================================================================
* = line$$h_cap_test - Horizontal Line Test =
* ===========================================================================
*
* OVERVIEW:
*       Write a horizontal line to the first scanline in video ram.
* 
* FORM OF CALL:
*       line$$h_cap_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int     line$$h_cap_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      s=1,fg=TRUE;
int      method,startx,starty,endx,endy;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;

startx           =  0;
endx             =  15;
starty           =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
endy             =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;

line.line        = transparent_line_mode,
line.span_line   = FALSE;
line.speed       = fast;
line.boolean_op  = GXcopy;
line.verify      = TRUE;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
line.color_change= TRUE;
line.draw_method = line_address;
line.window_type = Xwindow;

line.nogo        = go;

line.xa          = startx;
line.ya          = starty;
line.xb          = endx;
line.yb          = endy;

visual.capend    = TRUE;
visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;
visual.doz       = FALSE;

color.method     = foreground;
color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

s = (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);

return (s);
}



/*+
* ===========================================================================
* = line$$h_span_test - Horizontal Line Span Test =
* ===========================================================================
*
* OVERVIEW:
*       Span alias register test
* 
* FORM OF CALL:
*       line$$h_span_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int     line$$h_span_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      s=1,fg=TRUE;
int      method,startx,starty,endx,endy;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;

startx           =  0;
endx             =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;
starty           =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
endy             =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;

line.line        = transparent_line_mode,
line.span_line   = TRUE;
line.speed       = fast;
line.boolean_op  = GXcopy;
line.verify      = TRUE;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
line.color_change= TRUE;

line.nogo        = go;
line.draw_method = line_address;
line.window_type = NTwindow;

line.xa          = startx;
line.ya          = starty;
line.xb          = endx;
line.yb          = endy;

visual.capend    = FALSE;
visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;
visual.doz       = FALSE;

color.method     = foreground;
color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

s = (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);


return (s);
}


/*+
* ===========================================================================
* = line$$h_align_test - Horizontal Line Align Test =
* ===========================================================================
*
* OVERVIEW:
*       Line Address Alignment 
* 
* FORM OF CALL:
*       line$$h_align_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int     line$$h_align_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      i,s=1,fg=TRUE;
int      method,startx,starty,endx,endy;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;

/* Start line on a 1 pixel boundary using address register      */
/* Continue Register Used                                       */
for (s=1,i=1;s&1 && i<4;i++)
 {
 startx           =  i;
 endx             =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
 starty           =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
 endy             =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
 
 line.line        = transparent_line_mode,
 line.span_line   = FALSE;
 line.speed       = fast;
 line.boolean_op  = GXcopy;
 line.verify      = TRUE;
 line.polyline    = FALSE;
 line.wait_buffer = FALSE;
 line.color_change= TRUE;

 line.nogo        = go;
 line.draw_method = line_address;
 line.window_type = NTwindow;

 line.xa          = startx;
 line.ya          = starty;
 line.xb          = endx;
 line.yb          = endy;
 
 visual.capend    = FALSE;
 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.boolean   = line.boolean_op;
 visual.doz       = FALSE; 

 color.method     = foreground;
 color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
 color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

 s = (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
 s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
 }
return (s);
}


/*+
* ===========================================================================
* = line$$h_address_data_test - Horizontal Line Align Test =
* ===========================================================================
*
* OVERVIEW:
*       Address data pairs
* 
* FORM OF CALL:
*       line$$h_address_data_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int     line$$h_address_data_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      i,j,s=1,fg=TRUE;
int      method,startx,starty,endx,endy;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;

for (j=0;j<2;j++) 
 {
 /* Start line on a 1 pixel boundary using address/data  */
 for (s=1,i=1;s&1 && i<4;i++)
 {
 startx           =  i;
 endx             =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
 starty           =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
 endy             =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
 
 line.line        = transparent_line_mode,
 line.span_line   = FALSE;
 
 /* first time we use the traditional line draw method using b1,b2, and b3 */
 line.speed       = j;
 line.boolean_op  = GXcopy;
 line.verify      = TRUE;
 line.polyline    = FALSE;
 line.wait_buffer = FALSE;
 line.color_change= TRUE;

 /* When using the fast way, we use the slope nogo and then use the address */
 /* for doing frame buffer writes, since the slope already calculated       */

 line.nogo        = nogo;
 line.draw_method = address_data;
 line.window_type = NTwindow;

 line.xa          = startx;
 line.ya          = starty;
 line.xb          = endx;
 line.yb          = endy;
  
 visual.capend    = FALSE;
 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.boolean   = line.boolean_op;
 visual.doz       = FALSE; 

 color.method     = foreground;
 color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
 color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

 s = (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
 s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
 }
}
 
return (s);
}



/*+
* ===========================================================================
* = line$$output_signature_mode_test - The signature mode analysis test =
* ===========================================================================
*
* OVERVIEW:
*       Signature Analysis Test 
* 
* FORM OF CALL:
*       line$$output_signature_mode_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*                           
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*                        
* FUNCTIONAL DESCRIPTION:
*                        
--*/                     
int     line$$output_signature_mode_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )
                         
{                        
int    i,s=1;            
static XOR XorCheckSumTable [] = 
  {                      
   130  ,0x9e3c6836, 0x00000000,
   119  ,0x9e3c6836, 0x00000000,
   108  ,0x9e3c6836, 0x00000000,
   104  ,0x012b851c, 0x00000000,
   92   ,0x012b851c, 0x00000000,
   75   ,0x76550644, 0x00000000,
   74   ,0x76550644, 0x00000000,
   70   ,0x2be0b1b2, 0x00000000,
   69   ,0x2be0b1b2, 0x00000000,
   65   ,0x76550644, 0x00000000,
   50   ,0x1dfdcccf, 0x00000000,
   40   ,0x1dfdcccf, 0x00000000,
   32   ,0xcb3e9659, 0x00000000,
   25   ,0xcb3e9659, 0x00000000,
  };

static XOR P3XorCheckSumTable [] = 
  {                      
   130  ,0x9e3c6836, 0x00000000,
   119  ,0x9e3c6836, 0x00000000,
   108  ,0x9e3c6836, 0x00000000,
   104  ,0x61536d1c, 0x00000000,
   92   ,0x61536d1c, 0x00000000,
   75   ,0x7655069c, 0x00000000,
   74   ,0x7655069c, 0x00000000,
   70   ,0x2be0b1d2, 0x00000000,
   69   ,0x2be0b1d2, 0x00000000,
   65   ,0x7655069c, 0x00000000,
   50   ,0x1dfdcc03, 0x00000000,
   40   ,0x1dfdcc03, 0x00000000,
   32   ,0xcb3e9659, 0x00000000,
   25   ,0xcb3e9659, 0x00000000,
  };

static XOR XorCheckSumTableZ [] = 
  {
   130  ,0xaaead58d, 0x00000000,
   119  ,0xaaead58d, 0x00000000,
   108  ,0xaaead58d, 0x00000000,
   104  ,0xc18a4ef4, 0x00000000,
   92   ,0xc18a4ef4, 0x00000000,
   75   ,0x07f3aeb3, 0x00000000,
   74   ,0x07f3aeb3, 0x00000000,
   70   ,0x66ec7ac1, 0x00000000,
   69   ,0x66ec7ac1, 0x00000000,
   65   ,0x07f3aeb3, 0x00000000,
   50   ,0xe970ecf6, 0x00000000,
   40   ,0xe970ecf6, 0x00000000,
   32   ,0x285eaac3, 0x00000000,
   25   ,0x285eaac3, 0x00000000,
  };

 static struct signature_type sfbp$r_line32_test = 
  {
  0,
  0xe5,0xfa,0x41,0x90,                               
  0xd1,0x8d,0xe8,0xe2,
  0x7b,0xea,0x21,0xbf,
  };

 static struct signature_type sfbp$r_line32z_test = 
  {
  0,
  0x8c,0x6f,0xd4,0x98,                            
  0x43,0x33,0x5c,0xd3,
  0x0a,0xbe,0xcb,0xa6,
  }; 

 if (data_ptr->sfbp$l_test_lines == MAX_LINES)
 {            
 if (data_ptr->sfbp$l_ramdac==bt485) 
  {
   if (data_ptr->sfbp$l_test_zbuffer)
	s=line$$dma_verify_pattern(argc,argv,ccv,scia_ptr,data_ptr,param,
		XorCheckSumTableZ ,sizeof (XorCheckSumTableZ));
   else  
     if (data_ptr->sfbp$l_etch_revision >= TgaPass3)
        s = line$$dma_verify_pattern (argc,argv,ccv,scia_ptr,data_ptr,param,
	P3XorCheckSumTable ,sizeof (XorCheckSumTable));
     else
        s = line$$dma_verify_pattern (argc,argv,ccv,scia_ptr,data_ptr,param,
	XorCheckSumTable ,sizeof (XorCheckSumTable));
   }

  if (s&1 || data_ptr->sfbp$l_conte )
  {
  if (data_ptr->sfbp$l_ramdac==bt463)
	{
	if (data_ptr->sfbp$l_test_zbuffer)
		s=(*parse_func_ptr->pv_signature_mode_test) (argc,argv,ccv,scia_ptr,data_ptr,param,
	          	(struct signature_type *)&sfbp$r_line32z_test);
	else s=(*parse_func_ptr->pv_signature_mode_test) (argc,argv,ccv,scia_ptr,data_ptr,param,
	          	(struct signature_type *)&sfbp$r_line32_test);
	}
  }
 }

return (s);
}
                                                       

/*+
* ===========================================================================
* = line$$input_signature_mode_test - The signature mode analysis test =
* ===========================================================================
*
* OVERVIEW:
*       Signature Analysis Test 
* 
* FORM OF CALL:
*       line$$input_signature_mode_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int     line$$input_signature_mode_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int    i,s=1;

 static struct signature_type sfbp$r_lower_line32_test = 
  {
  0,
  0xe5,0xfa,0x41,0x90,                            
  0xd1,0x8d,0xe8,0xe2,
  0x7b,0xea,0x21,0xbf,
  };

 static struct signature_type sfbp$r_lower_line32z_test = 
  {
  0,
  0xe5,0xfa,0x41,0x90,                            
  0xd1,0x8d,0xe8,0xe2,
  0x7b,0xea,0x21,0xbf,
  };

 static struct signature_type sfbp$r_upper_line32_test = 
  {
  0,
  0xe5,0xfa,0x41,0x90,                            
  0xd1,0x8d,0xe8,0xe2,
  0x7b,0xea,0x21,0xbf,
  };

 static struct signature_type sfbp$r_upper_line32z_test = 
  {
  0,
  0xe5,0xfa,0x41,0x90,                            
  0xd1,0x8d,0xe8,0xe2,
  0x7b,0xea,0x21,0xbf,
  };

 #ifdef INPUT_SIGNS
 if (data_ptr->sfbp$l_test_lines == MAX_LINES)
 { 
  if (data_ptr->sfbp$l_ramdac==bt463)
	{
	if (data_ptr->sfbp$l_test_zbuffer)
	     {
	     printf ("z lower signature\n");
	     (*bt_func_ptr->pv_bt_write_reg)(scia_ptr,bt463$k_cmd_reg_2,0xc0);
	     s=(*parse_func_ptr->pv_input_signature_mode_test) (argc,argv,ccv,scia_ptr,data_ptr,param,
	          	(struct signature_type *)&sfbp$r_lower_line32z_test);
	     if (s&1)
	     	{
	        printf ("z upper signature\n");
	        (*bt_func_ptr->pv_bt_write_reg)(scia_ptr,bt463$k_cmd_reg_2,0xc4);
		s=(*parse_func_ptr->pv_input_signature_mode_test) (argc,argv,ccv,scia_ptr,data_ptr,param,
		          	(struct signature_type *)&sfbp$r_upper_line32z_test);
		}

	     }
	else 
	     {
	     printf ("lower signature\n");
	     (*bt_func_ptr->pv_bt_write_reg)(scia_ptr,bt463$k_cmd_reg_2,0xc0);
	     s=(*parse_func_ptr->pv_input_signature_mode_test) (argc,argv,ccv,scia_ptr,data_ptr,param,
	          	(struct signature_type *)&sfbp$r_lower_line32_test);
	     if (s&1)
		{
	        printf ("upper signature\n");
	        (*bt_func_ptr->pv_bt_write_reg)(scia_ptr,bt463$k_cmd_reg_2,0xc4);
		s=(*parse_func_ptr->pv_input_signature_mode_test) (argc,argv,ccv,scia_ptr,data_ptr,param,
		          	(struct signature_type *)&sfbp$r_upper_line32_test);
		}
	     }
	}
 }

(*bt_func_ptr->pv_bt_write_reg)(scia_ptr,bt463$k_cmd_reg_2,0xc0);
#endif

return (s);
}




/*+
* ===========================================================================
* = line$$dma_verify_pattern - Verify pattern =
* ===========================================================================
*
* OVERVIEW:
*       Verify Pattern 
* 
* FORM OF CALL:
*       line$$dma_verify_pattern (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int     line$$dma_verify_pattern (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,
        int param [], XOR XorCheckSumTable [] ,int TableSize)
              
{             
int XorCheckSum,i,s=1;
              
  XorCheckSum = (*bt_func_ptr->pv_bt_dma_checksum)(scia_ptr,data_ptr);
              
  /* Ignore the Stereo Monitor  */
  if (data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines>512)
   {          
   for (s=1,i=0;s&1 && i*sizeof (struct XorCheckSumType)<TableSize;i++)
        {                        
        if (XorCheckSumTable[i].frequency == data_ptr->sfbp$r_mon_data.sfbp$l_sold_freq)
                {                                      
                 if (data_ptr->sfbp$l_supplemental)    
                        printf ("%d Mhz Xor Check 0x%x\n",XorCheckSumTable[i].frequency,XorCheckSum);
                                                       
                 if (data_ptr->sfbp$l_module == hx_8_plane)                 
                        {                                                   
                        if (XorCheckSumTable[i].checksum8!= XorCheckSum)    
                             {                                              
                             param[0]  = data_ptr->sfbp$l_vram_base ;       
                             param[1]  = XorCheckSumTable[i].checksum8;
                             param[2]  = XorCheckSum;
                             s = 0;
                             }   
                        }        
                else             
                        {
                        if (XorCheckSumTable[i].checksum32!= XorCheckSum)
                             {
                             param[0]  = data_ptr->sfbp$l_vram_base ;
                             param[1]  = XorCheckSum;
                             param[2]  = XorCheckSumTable[i].checksum32;
                             s = 0;
                             }
                        }
                }
        }
    }
  if (!(s&1))
   {
   data_ptr->sfbp$l_fault_class = fault_checksum;
   data_ptr->sfbp$l_error_type  = VramTestError;
   }   
 
 return (s);
 }

int lineabs(int x){return  ( (x<0) ? -x : x);}




