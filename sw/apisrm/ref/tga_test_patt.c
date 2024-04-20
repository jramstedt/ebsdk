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
 *      Color Frame Buffer Test Routines 
 *
 *  ABSTRACT:
 *
 *      BT459 RAMDAC test .
  *
 *  AUTHORS:
 *
 *      James Peacock
 *
 *
 *  CREATION DATE:      02-11-91
 *                    
 *  MODIFICATION HISTORY:
 *      Initial Modification	
 *
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

#define   CHECK_DITHER() {if (data_ptr->sfbp$l_module == hx_32_plane_z  || \
                              data_ptr->sfbp$l_module == hx_32_plane_no_z )line.line = cint_nd_line_mode;}

struct span_test_type { COLOR_TYPES color; } ;

typedef  struct span_test_type SPAN_TEST;

SPAN_TEST span_table [] = 
        {
        red,
        green,
        blue,
        red_green,
        red_blue,
        green_blue,
        red_green_blue,
        };

SPAN_TEST shade_table [] = 
        {
        red_green_blue,
        red_blue,
        red_green,
        red_blue,
        green_blue,
        blue,
        };

struct XorCheckSumType 
  {
  int frequency;
  int checksum8;
  int checksum32;
  };
  
typedef struct XorCheckSumType XOR;

int     patt$$dma_verify_pattern (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,
        int param [], XOR XorCheckSumTable [] ,int tablesize);
        


/*+
* ===========================================================================
* = patt$$fill_vram - Fill video ram =
* ===========================================================================
*
* OVERVIEW:
*       Fill video ram
* 
* FORM OF CALL:
*       patt$$fill_vram (argc,argv,vector,scia_ptr,data_ptr,param )
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       vector      - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*       Fill video ram with noted color bar pixel refs.
--*/
int     patt$$fill_vram (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int   startx,starty,endx,endy;
register int   i,j,k,x,y,s,StatusReg;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;
#define  num_bars 8
register int      vscans,vram,size_bar;
static   int      bar_values[num_bars]=
         {
         sfbp$k_def_lw_black,
         sfbp$k_def_lw_blue,
         sfbp$k_def_lw_red,
         sfbp$k_def_lw_purple,
         sfbp$k_def_lw_green,
         sfbp$k_def_lw_yellow,
         sfbp$k_def_lw_red|sfbp$k_def_lw_green,
         sfbp$k_def_lw_white,
         };

 static struct signature_type sfbp$r_patt32_test = 
  {
  0,
  0x36,0x36,0x13,0x13,                            
  0x54,0x54,0xe3,0xe3,
  0x2d,0x2d,0xda,0xda,
  };

static XOR XorCheckSumTable [] = 
   {
   130  ,0x00000000, 0x00000000,
   119  ,0x00000000, 0x00000000,
   108  ,0x00000000, 0x00000000,
   104  ,0x00000000, 0x00000000,
   92   ,0x00000000, 0x00000000,
   75   ,0x00000000, 0x00000000,
   74   ,0x00000000, 0x00000000,
   70   ,0x00000000, 0x00000000,
   69   ,0x00000000, 0x00000000,
   65   ,0x00000000, 0x00000000,
   50   ,0xc0c0c0c0, 0x00000000,
   40   ,0xc0c0c0c0, 0x00000000,
   32   ,0xadadadad, 0x00000000,
   25   ,0xadadadad, 0x00000000,
  };                 
                     
size_bar         = data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/num_bars;
line.draw_method = line_address;
line.line        = transparent_line_mode,
line.speed       = fast;
line.boolean_op  = GXcopy;
line.nogo        = FALSE;
line.verify      = TRUE;
line.verify_z    = FALSE;
line.span_line   = FALSE;
line.window_type = Xwindow;
 
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;

color.method     = foreground;
line.ya          = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines;
line.yb          = 0;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
color.fg_value   = 0;
line.color_change= TRUE;

for (s=1,x=0;s&1 && x<data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;x++)
         {
         line.xa = line.xb = x;
         if (color.fg_value != bar_values[x/size_bar])
                line.color_change= TRUE; 
         color.fg_value = bar_values[x/size_bar] & data_ptr->sfbp$l_pixel_mask ;
         s=  (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
         s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
	 s = data_ptr->sfbp$l_conte ? 1 : s ;
	 line.polyline = data_ptr->sfbp$l_verify_lines ? FALSE : TRUE;         
         }
         
 line.xa = line.xb = x-1;
 s = (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
 s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);

 if (s&1)
  {
  if (data_ptr->sfbp$l_ramdac==bt485)  
  	s = patt$$dma_verify_pattern (argc,argv,ccv,scia_ptr,data_ptr,param, 
			XorCheckSumTable ,sizeof (XorCheckSumTable));

  if ((s&1) || data_ptr->sfbp$l_conte )
   {
   if (data_ptr->sfbp$l_ramdac == bt463 )
	s=(*parse_func_ptr->pv_signature_mode_test) (argc,argv,ccv,scia_ptr,data_ptr,param,
 	         (struct signature_type *)&sfbp$r_patt32_test);
   }
 }

return (s);
}




/*+
* ===========================================================================
* = patt$$red_test - The red test =
* ===========================================================================
*
* OVERVIEW:
*       Red Screen 
* 
* FORM OF CALL:
*       patt$$red_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Red screen test. On a TGA module we will check the SENSE bit for
*       proper polarity.
*
--*/
int     patt$$red_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int     size,s=1;

size = data_ptr->sfbp$l_vram_size;
if (data_ptr->sfbp$l_module==hx_32_plane_z)size/=2;

size -= data_ptr->sfbp$l_console_offset;

(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,data_ptr->sfbp$l_console_offset,
        sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ,size);                      

return (1);
}


/*+
* ===========================================================================
* = patt$$blue_test - The blue test =
* ===========================================================================
*
* OVERVIEW:
*       Blue Screen Test 
* 
* FORM OF CALL:
*       patt$$blue_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Blue Screen test
*
--*/
int     patt$$blue_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int     size,s=1;

 size = data_ptr->sfbp$l_vram_size;
 if (data_ptr->sfbp$l_module==hx_32_plane_z)size/=2;

 size -= data_ptr->sfbp$l_console_offset;

 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,data_ptr->sfbp$l_console_offset,
        sfbp$k_def_lw_blue & data_ptr->sfbp$l_pixel_mask ,size);                      
}


/*+
* ===========================================================================
* = patt$$green_test - The green test =
* ===========================================================================
*
* OVERVIEW:
*       Green Screen Test 
* 
* FORM OF CALL:
*       patt$$green_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Green Screen Test 
*
--*/
int     patt$$green_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int     size,s=1;

 size = data_ptr->sfbp$l_vram_size;
 if (data_ptr->sfbp$l_module==hx_32_plane_z)size/=2;
 
 size -= data_ptr->sfbp$l_console_offset;

 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,data_ptr->sfbp$l_console_offset,
        sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ,size);                      

 return (s);
}


/*+
* ===========================================================================
* = patt$$white_test - The white test =
* ===========================================================================
*
* OVERVIEW:
*       White Screen Test 
* 
* FORM OF CALL:
*       patt$$white_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* FUNCTIONAL DESCRIPTION:
*       White Test. 
*
--*/
int     patt$$white_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int     size,s=1;

 size = data_ptr->sfbp$l_vram_size;
 if (data_ptr->sfbp$l_module==hx_32_plane_z)size/=2;
 
 size -= data_ptr->sfbp$l_console_offset;

 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,data_ptr->sfbp$l_console_offset,
        sfbp$k_def_lw_white & data_ptr->sfbp$l_pixel_mask ,size);                      

 return (s);

 }





/*+
* ===========================================================================
* = patt$$black_test - The black test =
* ===========================================================================
*
* OVERVIEW:
*       black Screen Test 
* 
* FORM OF CALL:
*       patt$$black_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* FUNCTIONAL DESCRIPTION:
*       black Test. 
*
--*/
int     patt$$black_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int     size,s=1;

 size = data_ptr->sfbp$l_vram_size;
 if (data_ptr->sfbp$l_module==hx_32_plane_z)size/=2;
 
 size -= data_ptr->sfbp$l_console_offset;

 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,data_ptr->sfbp$l_console_offset,0,size);                      

 return (s);

}



/*+
* ===========================================================================
* = patt$$fill_gray - Fill video ram gray =
* ===========================================================================
*
* OVERVIEW:
*       Fill video ram gray scale
* 
* FORM OF CALL:
*       patt$$fill_gray (argc,argv,vector,scia_ptr,data_ptr,param )
*
* FUNCTIONAL DESCRIPTION:
*       Gray scale fill
--*/
int     patt$$fill_gray (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int   startx,starty,endx,endy;
register int   i,j,k,x,y,s;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;
#define  num_bars 8
register int      vscans,vram,size_bar;
static   int      gray_color[3]={1,1,1};
int      red=0,green=0,blue=0;
static   int      bar_values[num_bars]=
         {
         0x00000000, 
         0x20202020, 
         0x40404040, 
         0x60606060, 
         0x80808080, 
         0xA0A0A0A0, 
         0xC0C0C0C0, 
         0xFFFFFFFF, 
         };
         
char    temp[20],*pptr;
         
for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])
  if (*pptr++=='-'&&*pptr++== 'r')red=1;
for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])
  if (*pptr++=='-'&&*pptr++== 'g')green=1;
for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])
  if (*pptr++=='-'&&*pptr++== 'b')blue=1;
  
if (red|green|blue)
        {gray_color[0]=red;gray_color[1]=green;gray_color[2]=blue;}
        
(*bt_func_ptr->pv_bt_load_888_color)(scia_ptr,data_ptr,gray_color);
                                                                              
size_bar         = data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/num_bars;

line.draw_method = line_address;
line.line        = transparent_line_mode,
line.speed       = fast;
line.boolean_op  = GXcopy;
line.nogo        = FALSE;
line.verify      = TRUE;
line.verify_z    = FALSE;
line.span_line   = FALSE;
line.window_type = data_ptr->sfbp$l_line_type_mode;
 
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;

color.method     = foreground;
line.ya          = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines;
line.yb          = 0;
line.polyline    = FALSE;
line.wait_buffer = data_ptr->sfbp$l_verify_lines ? TRUE : FALSE;
line.color_change= TRUE; 
color.fg_value   = 0;

for (s=1,x=0;s&1 && x<data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;x++)
         {
         line.xa = line.xb = x;
         if (color.fg_value != bar_values[x/size_bar])
                line.color_change= TRUE; 
         color.fg_value = bar_values[x/size_bar] & data_ptr->sfbp$l_pixel_mask ;
         s=(*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
         s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
         line.polyline = data_ptr->sfbp$l_verify_lines ? FALSE : TRUE;
	 s = data_ptr->sfbp$l_conte ? 1 : s ;
         }
         
 line.xa = line.xb = x-1;
 line.wait_buffer = data_ptr->sfbp$l_verify_lines ? TRUE : FALSE;
 s= (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
 s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);

 /* Restore the color map        */
 if (data_ptr->sfbp$l_verbose)
         {
         if (data_ptr->sfbp$l_alt_console)
                 printf ("Gray Scale Continue ? ");
         getchar();
	 printf ("\n");
         }

(*bt_func_ptr->pv_bt_load_332_color)(scia_ptr,data_ptr);

return (s);

}



/*+
* ===========================================================================
* = patt$$color_test - Line Test =
* ===========================================================================
*
* OVERVIEW:
*       Color Interpolation Span Pattern
* 
* FORM OF CALL:
*       patt$$color_test  ( argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Color Interpolation Span Pattern
*
--*/
int  patt$$color_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      i,j,s=1,fg=TRUE;
int      dx,dy,dxGEdy,method,startx,starty,endx,endy,cnt ;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;
union    color_value_reg INC,RED,GRN,BLU;
int      section,fractional_intensity,whole_intensity;
static   int gray_color[3]={1,1,1};

static   struct signature_type sfbp$r_span32_test = 
 {
 0,
 0x65,0x36,0x5e,0xc4,                            
 0x14,0x17,0xac,0xf1,
 0x12,0x21,0x2d,0x34,
 };

static XOR XorCheckSumTable [] = 
   {
   130  ,0x20800060, 0x00000000,
   119  ,0x20800060, 0x00000000,
   108  ,0x20800060, 0x00000000,
   104  ,0x28bc8354, 0x00000000,
   92   ,0x28bc8354, 0x00000000,
   75   ,0x7894803c, 0x00000000,
   74   ,0x7894803c, 0x00000000,
   70   ,0x129e9e51, 0x00000000,
   69   ,0x129e9e51, 0x00000000,
   65   ,0x7894803c, 0x00000000,
   50   ,0x3927757a, 0x00000000,
   40   ,0x3927757a, 0x00000000,
   32   ,0xe894de4e, 0x00000000,
   25   ,0xe894de4e, 0x00000000,
  };                 
                     
 int     size;       
                     
 size = data_ptr->sfbp$l_vram_size;
 if (data_ptr->sfbp$l_module==hx_32_plane_z)size/=2;
 size -= data_ptr->sfbp$l_console_offset;
                     
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,
                data_ptr->sfbp$l_console_offset,0,size);

startx           =  0;
endx             =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;
cnt              =  (data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/8);

line.draw_method = line_address;
line.speed       = fast;
line.boolean_op  = GXcopy;
line.nogo        = FALSE;
line.span_line   = TRUE;
line.verify      = TRUE;
line.window_type = Xwindow;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
line.color_change= TRUE;

visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.boolean   = line.boolean_op;

color.fg_value   = 0;

RED.sfbp$l_whole = GRN.sfbp$l_whole = BLU.sfbp$l_whole = INC.sfbp$l_whole =0;

/* 24 plane use a graduated color map   */
if ( data_ptr->sfbp$l_module != hx_8_plane ) 
	(*bt_func_ptr->pv_bt_load_888_color)(scia_ptr,data_ptr,gray_color);

fractional_intensity = (4096 * 256) / endx; 
whole_intensity      = 0;

line.line = data_ptr->sfbp$l_planes == 8 ? cint_d_line_mode : cint_nd_line_mode;
 
INC.sfbp$r_color_value_bits.sfbp$v_fraction = fractional_intensity;
INC.sfbp$r_color_value_bits.sfbp$v_value    = whole_intensity;

RED.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
RED.sfbp$r_color_value_bits.sfbp$v_value    = 0;
GRN.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
GRN.sfbp$r_color_value_bits.sfbp$v_value    = 0;
BLU.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
BLU.sfbp$r_color_value_bits.sfbp$v_value    = 0;


for (s=1,j=0;j*sizeof(SPAN_TEST)<sizeof(span_table);j++)     
 {
 line.xa    = startx;
 line.xb    = endx;
 starty     = j* cnt ;
 line.ya    = starty;
 for (i=0;s&1 && i< cnt/2 ;i++,starty++)
        {
         endy      = starty;
         line.ya   = starty;
         line.yb   = endy;

         RED.sfbp$r_color_value_bits.sfbp$v_dither   = line.ya&31;
         GRN.sfbp$r_color_value_bits.sfbp$v_dither   = line.xa&31;
        
         color.red_value   = RED.sfbp$l_whole;
         color.green_value = GRN.sfbp$l_whole;
         color.blue_value  = BLU.sfbp$l_whole;

         color.red_increment   = color.green_increment  = color.blue_increment = 0;
         switch (span_table[j].color)
           {
           case red:
           case red_blue:
           case red_green:
           case red_green_blue:
                  color.red_increment   = INC.sfbp$l_whole;
           }                  

         switch (span_table[j].color)
           {
           case green:
           case red_green:
           case green_blue:
           case red_green_blue:
                  color.green_increment = INC.sfbp$l_whole;
           }                            

         switch (span_table[j].color)
           {
            case blue:
            case red_blue:
            case green_blue:
            case red_green_blue:
                  color.blue_increment  = INC.sfbp$l_whole;
            } 
        s=(*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
        s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
        line.polyline    = FALSE; 
	s = data_ptr->sfbp$l_conte ? 1 : s ;
        }
   }
  if (s&1)
  {
   if (data_ptr->sfbp$l_ramdac==bt485)  
	s = patt$$dma_verify_pattern (argc,argv,ccv,scia_ptr,data_ptr,param, XorCheckSumTable ,sizeof (XorCheckSumTable));
   if ((s&1) || data_ptr->sfbp$l_conte )
     {
     if(data_ptr->sfbp$l_ramdac == bt463 )
	s=(*parse_func_ptr->pv_signature_mode_test) (argc,argv,ccv,scia_ptr,data_ptr,param,
		(struct signature_type *)&sfbp$r_span32_test);
     }
  }
   
 /* 24 plane use a graduated color map   */
 if ( data_ptr->sfbp$l_module != hx_8_plane ) 
  {
  /* Restore the color map        */
  if (data_ptr->sfbp$l_verbose)
          {
          if (data_ptr->sfbp$l_alt_console)
                 printf ("Color Span Test Continue ?");
          getchar();
	  printf ("\n");
          }
   }

return (s);
}



/*+
* ===========================================================================
* = patt$$shade_test - Shade Test =
* ===========================================================================
*
* OVERVIEW:
*       Color Interpolation Shade Pattern
* 
* FORM OF CALL:
*       patt$$shade_test  ( argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Color Interpolation from top to bottom with all colors.
*
--*/
int  patt$$shade_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      i,j,s=1,fg=TRUE;
int      dx,dy,dxGEdy,method,startx,starty,endx,endy,cnt ;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;
union    color_value_reg INC,NINC,RED,GRN,BLU;
int      lineinc,section,fractional_intensity,whole_intensity;
static   int gray_color[3]={1,1,1};

static   struct signature_type sfbp$r_span32_test = 
 {              
 0,             
 0x74,0x74,0xd7,0xd7,                            
 0xa4,0xa4,0x22,0x22,
 0x94,0x94,0x51,0x51,
 };

static XOR XorCheckSumTable [] = 
   {
   130  ,0x1828e010, 0x00000000,
   119  ,0x1828e010, 0x00000000,
   108  ,0x1828e010, 0x00000000,
   104  ,0x5cf0d4c8, 0x00000000,
   92   ,0x5cf0d4c8, 0x00000000,
   75   ,0xa0a0c080, 0x00000000,
   74   ,0xa0a0c080, 0x00000000,
   70   ,0xc0c060e0, 0x00000000,
   69   ,0xc0c060e0, 0x00000000,
   65   ,0xa0a0c080, 0x00000000,
   50   ,0x0693d798, 0x00000000,
   40   ,0x0693d798, 0x00000000,
   32   ,0x5b705c64, 0x00000000,
   25   ,0x5b705c64, 0x00000000,
  };
 int     size;

 size = data_ptr->sfbp$l_vram_size;
 if (data_ptr->sfbp$l_module==hx_32_plane_z)size/=2;
 size -= data_ptr->sfbp$l_console_offset;

 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,
                data_ptr->sfbp$l_console_offset,0,size);

startx           =  0;
endx             =  0;
starty           =  0;
endy             =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines;

line.draw_method = line_address;
line.speed       = fast;
line.boolean_op  = GXcopy;
line.nogo        = FALSE;
line.span_line   = FALSE;
line.verify      = TRUE;
line.window_type = Xwindow;
line.wait_buffer = data_ptr->sfbp$l_verify_lines ? TRUE : FALSE;
line.color_change= TRUE;
line.polyline    = FALSE;

visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.boolean   = line.boolean_op;

color.fg_value   = sfbp$k_def_lw_red;

RED.sfbp$l_whole = GRN.sfbp$l_whole = BLU.sfbp$l_whole = INC.sfbp$l_whole = NINC.sfbp$l_whole = 0;

/* 24 plane use a graduated color map   */
if ( data_ptr->sfbp$l_module != hx_8_plane ) 
	(*bt_func_ptr->pv_bt_load_888_color)(scia_ptr,data_ptr,gray_color);

lineinc              = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/6;
fractional_intensity = ((4096 * 256) / lineinc) - 4096;
whole_intensity      = 1;

RED.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
RED.sfbp$r_color_value_bits.sfbp$v_value    = 0;
GRN.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
GRN.sfbp$r_color_value_bits.sfbp$v_value    = 0;
BLU.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
BLU.sfbp$r_color_value_bits.sfbp$v_value    = 0;

INC.sfbp$r_color_value_bits.sfbp$v_fraction =  fractional_intensity;
INC.sfbp$r_color_value_bits.sfbp$v_value    =  whole_intensity;

NINC.sfbp$r_color_value_bits.sfbp$v_fraction = -fractional_intensity;
NINC.sfbp$r_color_value_bits.sfbp$v_value    = 0xfe;

line.line = data_ptr->sfbp$l_planes == 8 ? cint_d_line_mode : cint_nd_line_mode;

for (s=1,i=0;s&1 && i<data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;i++,endx++)     
 {
 line.xa =  line.xb = endx;
 line.ya =  starty; 
 line.yb =  line.ya + lineinc;
 
 for (j=0;s&1 && j<6;j++,line.ya += lineinc,line.yb += lineinc)
   {
         RED.sfbp$r_color_value_bits.sfbp$v_dither   = line.ya&31;
         GRN.sfbp$r_color_value_bits.sfbp$v_dither   = line.xa&31;
        
         color.red_value   = RED.sfbp$l_whole;
         color.green_value = GRN.sfbp$l_whole;
         color.blue_value  = BLU.sfbp$l_whole;

         color.red_increment   = color.green_increment  = color.blue_increment = 0;
         switch (j)         
          {
          case 0: 
                /* phase in red only    */
                color.red_value       = (line.ya&31)<<27;
                color.green_value     = (line.xa&31)<<27;
                color.blue_value      = 0;
                color.red_increment   = INC.sfbp$l_whole;
                color.green_increment = 0;
                color.blue_increment  = 0;
                break;

          case 1: 
                /* phase in green now   */
                /* leaving red at max   */
                color.red_value       = ((line.ya&31)<<27)|0xff000;
                color.green_value     =  (line.xa&31)<<27;
                color.blue_value      = 0;
                color.red_increment   = 0;
                color.green_increment = INC.sfbp$l_whole;
                color.blue_increment  = 0;
                break;

          case 2: 
                /* phase in blue now   */
                /* leaving rg at max   */
                color.red_value       = ((line.ya&31)<<27)|0xff000;
                color.green_value     = ((line.xa&31)<<27)|0xff000;
                color.blue_value      = 0;
                color.red_increment   = 0;
                color.green_increment = 0;
                color.blue_increment  = INC.sfbp$l_whole;
                break;

          case 3: 
                /* phase out green     */
                color.red_value       = ((line.ya&31)<<27)|0xff000;
                color.green_value     = ((line.xa&31)<<27)|0xff000;
                color.blue_value      = 0xff000;
                color.red_increment   = 0;
                color.green_increment = NINC.sfbp$l_whole;
                color.blue_increment  = 0;
                break;

          case 4: 
                /* phase out red       */
                color.red_value       = ((line.ya&31)<<27)|0xff000;
                color.green_value     = (line.xa&31)<<27;
                color.blue_value      = 0xff000;
                color.red_increment   = NINC.sfbp$l_whole;
                color.green_increment = 0;
                color.blue_increment  = 0;
                break;
                                                
          case 5: 
                /* phase out blue      */
                color.red_value       = (line.ya&31)<<27;
                color.green_value     = (line.xa&31)<<27;
                color.blue_value      = 0xff000;
                color.red_increment   = 0;
                color.green_increment = 0;
                color.blue_increment  = NINC.sfbp$l_whole;
                break;
          }
          
        s=(*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
        s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
	s = data_ptr->sfbp$l_conte ? 1 : s ;
        }
   }
  if (s&1)
  {
   if (data_ptr->sfbp$l_ramdac==bt485)  
	s = patt$$dma_verify_pattern (argc,argv,ccv,scia_ptr,data_ptr,param, XorCheckSumTable ,sizeof (XorCheckSumTable));
   if ((s&1) || data_ptr->sfbp$l_conte )
     { 
     if (data_ptr->sfbp$l_ramdac == bt463 )
	s=(*parse_func_ptr->pv_signature_mode_test) (argc,argv,ccv,scia_ptr,data_ptr,param,
		(struct signature_type *)&sfbp$r_span32_test);
     }
   }
 
 /* 24 plane use a graduated color map   */
 if ( data_ptr->sfbp$l_module != hx_8_plane ) 
  {
  /* Restore the color map        */
  if (data_ptr->sfbp$l_verbose)
          {
          if (data_ptr->sfbp$l_alt_console)
                 printf ("Color Span Test Continue ?");
          getchar();
	  printf ("\n");
          }
  }

 
return (s);
}


/*+
* ===========================================================================
* = patt$$grayline_test - Line Test =
* ===========================================================================
*
* OVERVIEW:
*       Color Interpolation Span Pattern
* 
* FORM OF CALL:
*       patt$$grayline_test  ( argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Color Interpolation Span Pattern
*
--*/
int  patt$$grayline_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,int param [] )

{
int      size,i,j,s=1,fg=TRUE;
int      dx,dy,dxGEdy,method,startx,starty,endx,endy,cnt ;
union    color_value_reg INC,RED,GRN,BLU;
int      fractional_intensity,whole_intensity,red=0,green=0,blue=0;
static   int   gray_color[3]={1,1,1};
register char *pptr;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;

static   struct signature_type sfbp$r_span32_test = 
 {
 0,
 0x3f,0x3f,0x24,0x24,                            
 0xca,0xca,0x9a,0x9a,
 0x3b,0x3b,0xe4,0xe4,
 };

static   XOR XorCheckSumTable [] = 
   {
   130  ,0xe8a80010, 0x00000000,
   119  ,0xe8a80010, 0x00000000,
   108  ,0xe8a80010, 0x00000000,
   104  ,0xcc4cf0a4, 0x00000000,
   92   ,0xcc4cf0a4, 0x00000000,
   75   ,0x80a0c000, 0x00000000,
   74   ,0x80a0c000, 0x00000000,
   69   ,0x008000a0, 0x00000000,
   65   ,0x80a0c000, 0x00000000,
   50   ,0x89da4a05, 0x00000000,
   40   ,0x89da4a05, 0x00000000,
   32   ,0x5b705c64, 0x00000000,
   25   ,0x5b705c64, 0x00000000,
  };

 for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])
   if (*pptr++=='-'&&*pptr++== 'r')red=1;
 for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])
   if (*pptr++=='-'&&*pptr++== 'g')green=1;
 for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])
   if (*pptr++=='-'&&*pptr++== 'b')blue=1;

 if (red||green||blue)
        {gray_color[0]=red;gray_color[1]=green;gray_color[2]=blue;}

 size = data_ptr->sfbp$l_vram_size;
 if (data_ptr->sfbp$l_module==hx_32_plane_z)size/=2;
 size -= data_ptr->sfbp$l_console_offset;

 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,data_ptr->sfbp$l_console_offset,0,size);

line.draw_method = line_address;
line.speed       = fast;
line.boolean_op  = GXcopy;
line.nogo        = FALSE;
line.span_line   = TRUE;
line.verify      = TRUE;
line.window_type = Xwindow;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
line.color_change= TRUE;

visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.boolean   = line.boolean_op;
color.method     = foreground;
color.fg_value   = sfbp$k_def_lw_red;

RED.sfbp$l_whole = GRN.sfbp$l_whole = BLU.sfbp$l_whole = INC.sfbp$l_whole =0;

fractional_intensity = 0;
whole_intensity      = 0;

line.line = data_ptr->sfbp$l_planes == 8 ? cint_d_line_mode : cint_nd_line_mode;
 
INC.sfbp$r_color_value_bits.sfbp$v_fraction = fractional_intensity;
INC.sfbp$r_color_value_bits.sfbp$v_value    = whole_intensity;

startx           =  0;
endx             =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;
starty           =  0;
cnt              =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines;

RED.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
GRN.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
BLU.sfbp$r_color_value_bits.sfbp$v_fraction = 0;

line.xa   = startx;
line.xb   = endx;

/* 24 plane use a graduated color map   */
if ( data_ptr->sfbp$l_module != hx_8_plane ) 
	(*bt_func_ptr->pv_bt_load_888_color)(scia_ptr,data_ptr,gray_color);

for (s=1,j=0;s&1 && j<cnt ;j++,starty++)     
 {
 line.ya   = starty;
 line.yb   = starty;

 RED.sfbp$r_color_value_bits.sfbp$v_value    = gray_color[0] ? j/4 : 0;
 GRN.sfbp$r_color_value_bits.sfbp$v_value    = gray_color[1] ? j/4 : 0;
 BLU.sfbp$r_color_value_bits.sfbp$v_value    = gray_color[2] ? j/4 : 0;

 RED.sfbp$r_color_value_bits.sfbp$v_dither   = line.ya&31;
 GRN.sfbp$r_color_value_bits.sfbp$v_dither   = line.xa&31;
        
 color.red_value   = RED.sfbp$l_whole;
 color.green_value = GRN.sfbp$l_whole;
 color.blue_value  = BLU.sfbp$l_whole;

 color.red_increment   = color.green_increment  = color.blue_increment = 0;

 s = (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
 s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
 s = data_ptr->sfbp$l_conte ? 1 : s ;
 line.polyline    = FALSE; 
 }

 if (s&1 && !(red|green|blue) )
    {
    if (data_ptr->sfbp$l_ramdac==bt485)  
	s = patt$$dma_verify_pattern (argc,argv,ccv,scia_ptr,data_ptr,param, 
                        XorCheckSumTable ,sizeof (XorCheckSumTable));
    if ( (s&1) || data_ptr->sfbp$l_conte)
     {
     if (data_ptr->sfbp$l_ramdac == bt463 )
	s=(*parse_func_ptr->pv_signature_mode_test) (argc,argv,ccv,scia_ptr,data_ptr,param,
		(struct signature_type *)&sfbp$r_span32_test);
     }
 
    }

return (s);
}


/*+
* ===========================================================================
* = patt$$dma_verify_pattern - Verify pattern =
* ===========================================================================
*
* OVERVIEW:
*       Verify Pattern 
* 
* FORM OF CALL:
*       patt$$dma_verify_pattern (argc,argv,ccv,scia_ptr,data_ptr,param)
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
int     patt$$dma_verify_pattern (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,SFBP * data_ptr,
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
                             param[1]  = XorCheckSumTable[i].checksum32;
                             param[2]  = XorCheckSum; 
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
