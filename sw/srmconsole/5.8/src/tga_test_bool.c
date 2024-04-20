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
#include "cp$src:tga_sfbp_test_bool.h"



/*+
* ===========================================================================
* = bool$$sifb_mode_test - Boolean Simple Frame Buffer Mode Test =
* ===========================================================================
*
* OVERVIEW:
*       Boolean Simple Frame Buffer Mode Test 
* 
* FORM OF CALL:
*       bool$$sifb_mode_test (argc,argv,ccv,scia_ptr,data_ptra,param)
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
*               This test will verify each of the boolean operations
*       while in simple mode. 
*
	The Raster Op Register defines the operation to be performed on
	the pixel as it is written to the Frame Buffer.  Certain raster
	ops define logical functions of the old and new pixel data which
	require a read-modify-write operation to update the pixel.
	Raster-ops that require only a write take 60 nsec/8 bytes in page
	mode.  Raster-ops that require a read-modify-write take about 180
	nsec/8 bytes.

	31                                    12  10 9 8 7     4 3     0
	+---------------------------------------+---+---+-------+------+
	|                  ignore               |rot|vis|ignore | code |
	+---------------------------------------+---+---+-------+------+

            Function Name     HexCode           Function
          ==============================================================
            GXclear     	0		0
	    GXand       	1		src AND dst
	    GXandReverse	2		src AND NOT dst
	    GXcopy      	3		src
	    GXandInverted       4		(NOT src) AND dst
	    GXnoop      	5		dst
	    GXxor       	6		src XOR dst
	    GXor        	7		src OR dst
	    GXnor       	8		NOT(src) AND NOT(dst)
	    GXequiv     	9		NOT(src) XOR dst
	    GXinvert    	a		NOT dst
	    GXorReverse 	b		src OR (NOT dst)
	    GXcopyInverted      c		NOT src
	    GXorInverted	d		(NOT src) OR dst
	    GXnand      	e		(NOT src) OR (NOT dst)
	    GXset       	f		1

	The Visual depth field in the Raster Op register defines the type of
	destination visual on 32 plane systems. Depth must be zero on 8 plane
	systems.

	Visual:   00   8  bit   packed destination
		  01   8  bit unpacked destination
		  10   12 bit destination
		  11   24 bit destination

	The Rotate field of the Raster Op Register defines the byte position
	within the word of the destination visual.  This is only useful in
	conjunction with 8 bit unpacked visuals on 32 plane systems and must
	be zero otherwise.

	Rotate:  00    rotate destination 0 bytes
		 01    rotate destination 1 byte
		 10    rotate destination 2 bytes
		 11    rotate destination 3 bytes

	Value at init: 3
*
*
--*/
int     bool$$sifb_mode_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
volatile register int      i,s=1,num_bytes;
volatile unsigned int *src,*dst,actual;
register int rop;

dst  = (unsigned int *)data_ptr->sfbp$l_console_offset;

for (i=0,s=1;s&1&&i*sizeof(struct expect_type)<sizeof(expect_table);i++)
 {
  (*gfx_func_ptr->pv_sfbp_write_reg) (scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
  (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,dst,expect_table[i].initial,LW);
  rop = data_ptr->sfbp$l_module == hx_8_plane ? expect_table[i].code  : expect_table[i].code | (DestVisualPacked<<8);
  (*gfx_func_ptr->pv_sfbp_write_reg) (scia_ptr,sfbp$k_boolean_op,rop,LW);
  (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,dst,expect_table[i].src,LW);
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,dst,LW);
  if (actual!= expect_table[i].expect || data_ptr->sfbp$l_test_error )                       
       { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)dst;                                                      
      param[1] = expect_table[i].expect;
      param[2] = actual;                                                        
      s=0;                                                                      
      break; 
     }  
 }

 /* Restore the default raster ops and the such  */
 wbflush ();
 
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

return (s);                                             
}


/*+
* ===========================================================================
* = bool$$stip_mode_test - Boolean Stipple Mode Test =
* ===========================================================================
*
* OVERVIEW:
*       Boolean Stipple Mode Test 
* 
* FORM OF CALL:
*       bool$$stip_mode_test (argc,argv,ccv,scia_ptr,data_ptra,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
*
* FUNCTIONAL DESCRIPTION:
*       Stipple in GXxor and verify raster op.
*
--*/
int     bool$$stip_mode_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
volatile register  int      i,j,s=1,num_bytes,num_lws;
volatile char *vram,*src,*dst;
volatile int  actual,iexpect,expect;
STIP     stip;
COLOR    color;
VISUAL   visual;
Z        z;

 num_bytes     = 2048;
 num_lws       = num_bytes/4;

 dst = vram = (char *)data_ptr->sfbp$l_console_offset;
 
 /* Fill with our default pattern       */
 (*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,vram,0x55555555,num_bytes);
 
 /* Stipple the data in GXxor mode to compliment the data now     */
 /* Since that is the ultimate goal...compliment the data         */

 stip.src_address = (int)dst;
 stip.size        = num_bytes;
 stip.mode        = GXxor;
 stip.stipple     = transparent_stipple;
 color.fg_value   = M1;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = 0;
 visual.boolean   = stip.mode;
    
 /* Do it with XOR mode set             */
 (*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

  for(j=0;s&1&&j<num_lws;j++,vram+=4)                                                           
   {
   actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
   if (actual != 0xAAAAAAAA || data_ptr->sfbp$l_test_error )
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;                                     
      param[1] = 0xAAAAAAAA;
      param[2] = actual;                                        
      s=0;                                                      
      }  
   }


 return (s);                                             
}


/*+
* ===========================================================================
* = bool$$copy_mode_test - Boolean Copy Mode Test =
* ===========================================================================
*
* OVERVIEW:
*       Boolean Copy Mode Test 
* 
* FORM OF CALL:
*       bool$$copy_mode_test (argc,argv,ccv,scia_ptr,data_ptra,param)
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
*       GXxor raster op in copy mode.
*
*
--*/
int     bool$$copy_mode_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int      i,j,s=1,num_bytes,num_lws,shift;
char *src,*dst;
int copymask,actual,iexpect,expect;
SCOPY    copy;
VISUAL   visual;
Z        z;

 num_bytes       = 2048;
 num_lws         = num_bytes/4;
 shift           = 0;
 copymask        = 0xffffffff;

 src = (char *)(data_ptr->sfbp$l_console_offset);
 dst = src+(10*num_bytes);

 /* Fill source with 0xFFFFFFFF        */
 (*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,src,0xFFFFFFFF,num_bytes);

 /* Fill destination with 0x55555555   */
 (*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,dst,0x55555555,num_bytes);

 copy.src_address = (int)src;
 copy.dst_address = (int)dst;
 copy.size        = num_bytes;
 copy.copy        = simple;
 copy.copymask    = copymask;
 copy.boolean_op  = GXxor;
 copy.pixel_shift = shift;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = 0;
 visual.boolean   = copy.boolean_op;

(*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

for(s=1,j=0;s&1&&j<num_lws;j++,dst+=4)                      
   {
   actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,dst);
   if (actual != 0xAAAAAAAA || data_ptr->sfbp$l_test_error )
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)dst;                                     
      param[1] = 0xAAAAAAAA;
      param[2] = actual;                                        
      s=0;                                                      
      }  
   }

return (s);
}

/*+
* ===========================================================================
* = bool$$line_mode_test - Boolean Line Mode Test =
* ===========================================================================
*
* OVERVIEW:
*       Boolean Line Mode Test 
* 
* FORM OF CALL:
*       bool$$line_mode_test (argc,argv,ccv,scia_ptr,data_ptra,param)
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
*       Draw a horizontal centered line in GXxor mode. The line verify routine needs
*       to be modified to verify the line draw operation.
*
--*/
int     bool$$line_mode_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int      i,j,s=1;
register char     *src,*dst;
register int      num_bytes,num_lws,lines,actual,startx,starty,endx,endy,method;
register int 	  expect;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;

 src            = (char *)data_ptr->sfbp$l_console_offset;
 num_bytes      = 64;
 num_lws        = num_bytes/4;
 
 (*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,src,0x55555555,num_bytes);

 startx         =  0;
 endx           =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;
 starty         =  0;
 endy           =  0;

 /* Draw a transparent line now         */
 line.draw_method = line_address;
 line.line        = transparent_line_mode,
 line.speed       = slow;
 line.boolean_op  = GXxor;
 line.nogo        = FALSE;
 line.polyline    = FALSE;
 line.wait_buffer = TRUE;
 line.color_change= TRUE;
 
 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = 0;
 visual.boolean   = line.boolean_op;
 
 color.method     = foreground;

 /* Load with 0xffffffff so line will be complimented   */
 /* Since raster op is XOR                              */
 color.fg_value   = 0xFFFFFFFF;

 line.xa          = startx;
 line.ya          = starty;
 line.xb          = endx;
 line.yb          = endy;
 line.window_type = Xwindow;

 (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);

 expect  = 0xAAAAAAAA & data_ptr->sfbp$l_pixel_mask;

 for(j=0;s&1&&j<num_lws;j++,src+=4)                      
   {
   actual  = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,src);
   actual &= data_ptr->sfbp$l_pixel_mask;

   if (actual != expect || data_ptr->sfbp$l_test_error )
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)src;                                     
      param[1] = expect;
      param[2] = actual;                                        
      s=0;                                                      
      }  
   }

return (s);                                             
}
