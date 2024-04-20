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
 *      Box for alignment patterns.
   *
 *  AUTHORS:
 *
 *      James Peacock
 *
 *
 *  CREATION DATE:      02-11-91
 *
 --*/

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



/*+
* ===========================================================================
* = box$$load_grid - Load the screen grid =
* ===========================================================================
*
* OVERVIEW:
*       Load the box screen grid.
* 
* FORM OF CALL:
*       box$$load_grid (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Use the line driver to draw a 32 line x  32 line grid.
*
--*/
int     box$$load_grid (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int   startx,starty,endx,endy;
register int   hlines=0,vlines=0,s,i,j,k,x,y;
register char *pptr;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;


 hlines=0;
 for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
        if (*pptr++=='-'&& *pptr++== 'h')
            {hlines=strtol (pptr,0,0);break;}

 vlines=0;
 for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
        if (*pptr++=='-'&& *pptr++== 'v' )
            {vlines=strtol (pptr,0,0);break;}

 line.draw_method = line_address;
 line.line        = transparent_line_mode,
 line.speed       = slow;
 line.boolean_op  = GXcopy;
 line.nogo        = FALSE;
 line.verify      = TRUE;
 line.verify_z    = FALSE;
 line.span_line   = FALSE;
  
 visual.doz       = FALSE;
 visual.capend    = FALSE;
 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.boolean   = line.boolean_op;
 
 color.method     = foreground;
 color.fg_value   = sfbp$k_def_lw_white & data_ptr->sfbp$l_pixel_mask ;
 
 line.xa          = 0;
 line.xb          = data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;;
 line.polyline    = FALSE;
 line.wait_buffer = data_ptr->sfbp$l_verify_lines ? TRUE :FALSE;
 line.color_change= TRUE;

 endx =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;
 endy =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines;

 /* draw horizontal lines first	*/ 
 for (i=0,s=1,y=0;s&1 && y<endy;y+=32)
         {
         line.yb = line.ya = y;
         (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
         s= (*parse_func_ptr->pv_verify_line )(scia_ptr,data_ptr,&line,&color,&visual,&z,param,0);
         line.polyline  = FALSE;
	 if ( (++i==hlines) && hlines )break;
         }
         
 if (s&1) 
  {
   /* one more to fill screen	*/
   if (!hlines)
   {
   line.ya = line.yb = endy-1;
   (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
   s= (*parse_func_ptr->pv_verify_line )(scia_ptr,data_ptr,&line,&color,&visual,&z,param,0);
   }
   
   line.ya          = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines;
   line.yb          = 0;
   line.polyline    = FALSE;
 
   /* do the vertical	*/
   for (i=0,x=0;s&1 && x<endx;x+=32)
         {
         line.xa = line.xb = x;
         (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
         s= (*parse_func_ptr->pv_verify_line )(scia_ptr,data_ptr,&line,&color,&visual,&z,param,0);
         line.polyline    = FALSE;
	 if ( (++i==vlines) && vlines )break;
         }
         
  if (s&1)
    {
    /* One more to fill screen	*/
    if (!vlines)
	{
	line.xa = line.xb = endx-1;
	(*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
	s= (*parse_func_ptr->pv_verify_line )(scia_ptr,data_ptr,&line,&color,&visual,&z,param,0);
	}
    }
  }
   
return (s);
}


/*+
* ===========================================================================
* = box$$load_circle - Load the circle on grid =
* ===========================================================================
*
* OVERVIEW:
*       Draw a circle on top of boxes.
* 
* FORM OF CALL:
*       box$$load_circle (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Draw a circle whose radius is the most left and right pixel of the
*       display. Since our display is not square, then the figure displayed
*       is actually an ellipse,isn't it. This program starts at a particular
*       point on the circle and draws a line segment, or chord. 
*
--*/
int     box$$load_circle (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int     i,j,k,s=1;
register int     d,radius,x,y;

radius        = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
d             = 3 - (2*radius);

for (x=0,y=radius;x<y;)
 {
 box$$draw_circle(scia_ptr,data_ptr,x, y);
 if(d<0)d+= 4*x+6;
 else 
  {
  d+= 4*(x-y)+10;
  y--;
  }
  x++;
 }
 if (x==y)box$$draw_circle (scia_ptr,data_ptr,x,y);

return (SS$K_SUCCESS);
}


/*+
* ===========================================================================
* = box$$draw_circle - Draw a circle chord =
* ===========================================================================
*
* OVERVIEW:
*       Draw a chord for a circle.
* 
* FORM OF CALL:
*       box$$draw_circle (scia_ptr,data_ptr,x, y)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data pointer
*       x               - tbd
*       y               - tbd
*
--*/
int box$$draw_circle(SCIA *scia_ptr,SFBP *data_ptr,int x,int y)

{
int      offset,mask,col,x0,y0,a, b,hpix,bytemask,bpix,temp;
unsigned char   *pixel;

pixel         = (unsigned char *)data_ptr->sfbp$l_console_offset;
bpix	      = (data_ptr->sfbp$l_module==hx_8_plane) ? 1: 4;
hpix          = (int)data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels*bpix;
x0            = data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels/2;   
y0            = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;   
col           = sfbp$k_def_lw_white & data_ptr->sfbp$l_pixel_mask;

a =  bpix * (x0 + x);
b =  hpix * (y0 - y);
offset   = pixel+a+b;
bytemask = offset%4;
mask   = data_ptr->sfbp$l_module==hx_8_plane ? 0xff : 0x00ffffff;
mask <<= (bytemask*8);
temp   = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,offset);
temp  |= (col & mask);
(*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,offset,temp,LW);

a = bpix * (x0 + y) ;
b = hpix * (y0 - x);
offset   = pixel+a+b;
bytemask = offset%4;
mask   = data_ptr->sfbp$l_module==hx_8_plane ? 0xff : 0x00ffffff;
mask <<= (bytemask*8);
temp   = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,offset);
temp  |= (col & mask);
(*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,offset,temp,LW);

a = bpix * (x0 + y);
b = hpix * (y0 + x);
offset   = pixel+a+b;
bytemask = offset%4;
mask   = data_ptr->sfbp$l_module==hx_8_plane ? 0xff : 0x00ffffff;
mask <<= (bytemask*8);
temp   = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,offset);
temp  |= (col & mask);
(*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,offset,temp,LW);

a = bpix * (x0 + x);
b = hpix * (y0 + y);
offset   = pixel+a+b;
bytemask = offset%4;
mask   = data_ptr->sfbp$l_module==hx_8_plane ? 0xff : 0x00ffffff;
mask <<= (bytemask*8);
temp   = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,offset);
temp  |= (col & mask);
(*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,offset,temp,LW);

a = bpix * (x0 - x);
b = hpix * (y0 + y);
offset   = pixel+a+b;
bytemask = offset%4;
mask   = data_ptr->sfbp$l_module==hx_8_plane ? 0xff : 0x00ffffff;
mask <<= (bytemask*8);
temp   = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,offset);
temp  |= (col & mask);
(*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,offset,temp,LW);

a = bpix * (x0 - y);
b = hpix * (y0 + x);
offset   = pixel+a+b;
bytemask = offset%4;
mask   = data_ptr->sfbp$l_module==hx_8_plane ? 0xff : 0x00ffffff;
mask <<= (bytemask*8);
temp   = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,offset);
temp  |= (col & mask);
(*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,offset,temp,LW);

a = bpix * (x0 - y);
b = hpix * (y0 - x);
offset   = pixel+a+b;
bytemask = offset%4;
mask   = data_ptr->sfbp$l_module==hx_8_plane ? 0xff : 0x00ffffff;
mask <<= (bytemask*8);
temp   = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,offset);
temp  |= (col & mask);
(*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,offset,temp,LW);

a = bpix * (x0 - x);
b = hpix * (y0 - y);
offset   = pixel+a+b;
bytemask = offset%4;
mask   = data_ptr->sfbp$l_module==hx_8_plane ? 0xff : 0x00ffffff;
mask <<= (bytemask*8);
temp   = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,offset);
temp  |= (col & mask);
(*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,offset,temp,LW);

return (SS$K_SUCCESS);
}

