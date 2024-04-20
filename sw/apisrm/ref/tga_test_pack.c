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
 *      1.1     Add stipple test for video ram screening.
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
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"

#define TEST_CASES 3

#define SIZE_DMA_BUFFER    (16384+32)
#define LW_SIZE_DMA_BUFFER (SIZE_DMA_BUFFER/4)

#define CHECK_CONFIG() {if (data_ptr->sfbp$l_module == hx_8_plane)return(1);}

int	StipPattern;


/*+
* ===========================================================================
* = stip$$stipple_8_pack_vis_test - Stipple 8 bit packed Visual Test = 
* ===========================================================================
*
* OVERVIEW:
*       8 bit unpacked visual 
* 
* FORM OF CALL:
*       stip$$stipple_8_pack_vis_test ( argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       8 bit packed visual for stipple mode
--*/
int     stip$$stipple_8_pack_vis_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             i,s=1,num_bytes;
register        unsigned int    expect,actual,*vram;
register        int             offset;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;
register	char		*pptr;

StipPattern=0;
for (i=0,pptr=argv[i];i<argc;i++,pptr=argv[i])    
        if (*pptr++=='-'&& *pptr++== 'p')
            {StipPattern=strtol (pptr,0,16);break;}

offset         = data_ptr->sfbp$l_console_offset;
vram           = (unsigned int *)offset;
expect         = StipPattern ? StipPattern : sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;

CHECK_CONFIG();

stip.src_address = (int)vram;
stip.size        = num_bytes;
stip.mode        = GXcopy;
stip.stipple     = transparent_stipple;

color.fg_value   = expect;

visual.depth     = packed_8_packed;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.boolean   = stip.mode;

(*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

/* mask out everything except the byte which we wanted  */
/* which is least significant byte in this test         */

for(i=0;s&1&&i<num_bytes/4;i++,vram++)
 {
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  if (actual !=expect || data_ptr->sfbp$l_test_error )                                
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;                                     
      param[1] = expect ;                                       
      param[2] = actual;                                        
      s=0;                                                      
      }  
 }
return (s);
}


/*+
* ===========================================================================
* = stip$$stipple_8_unpack_vis_test - Stipple 8 bit unpacked Visual Test = 
* ===========================================================================
*
* OVERVIEW:
*       8 bit unpacked visual 
* 
* FORM OF CALL:
*       stip$$stipple_8_unpack_vis_test ( argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       8 bit unpacked destination visual for stipple mode.
*
--*/
int     stip$$stipple_8_unpack_vis_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int    i,s=1,bytemask,num_bytes;
register        int    expect,actual,*vram;
register        int    offset;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;
register	char		*pptr;

StipPattern=0;
for (i=0,pptr=argv[i];i<argc;i++,pptr=argv[i])    
        if (*pptr++=='-'&& *pptr++== 'p')
            {StipPattern=strtol (pptr,0,16);break;}

offset         = data_ptr->sfbp$l_console_offset;
vram           = (int *)offset;
expect         = StipPattern ? StipPattern : sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;

CHECK_CONFIG();

stip.src_address = (int)vram;
stip.size        = num_bytes;
stip.mode        = GXcopy;
stip.stipple     = transparent_stipple;

color.fg_value   = expect;

/* 8 bit unpacked mode, which means we get 1 byte per longword  */
/* We can test this in onscreen memory, but only in color red   */
/* if we do no rotation on the data.                            */
/* see the other tests for rotation.                            */

visual.depth     = packed_8_unpacked;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.boolean   = stip.mode;

(*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

for(i=0;s&1&&i<num_bytes/4;i++,vram++)
 {
  bytemask = (i%4);
  expect   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;
  expect >>= (bytemask<<3);
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);

  if ((actual&0xff) != (expect&0xff) || data_ptr->sfbp$l_test_error )                              
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;                                     
      param[1] = expect&0xff ;                                       
      param[2] = actual&0xff ;                                        
      s=0;                                                      
      }  
 }
return (s);
}



/*+
* ===========================================================================
* = stip$$stipple_8_unpack_rot_test - Stipple 8 bit packed rotate Test = 
* ===========================================================================
*
* OVERVIEW:
*       8 bit unpacked visual rotate test
* 
* FORM OF CALL:
*       stip$$stipple_8_unpack_rot_test ( argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Rotate unpacked visualks for stipple mode. 
*
--*/
int     stip$$stipple_8_unpack_rot_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int    i,j,s=1,bytemask,num_bytes;
register        int    expect,actual,*vram;
register        int    offset,rotate_mask;
ROTATE_DEST     rot[4];
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;
register	char		*pptr;

StipPattern=0;
for (i=0,pptr=argv[i];i<argc;i++,pptr=argv[i])    
        if (*pptr++=='-'&& *pptr++== 'p')
            {StipPattern=strtol (pptr,0,16);break;}

expect         = StipPattern ? StipPattern : sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
rot[0]         = rotate_dest_0;
rot[1]         = rotate_dest_1;
rot[2]         = rotate_dest_2;
rot[3]         = rotate_dest_3;

CHECK_CONFIG();

stip.size        = num_bytes;
stip.mode        = GXcopy;
stip.stipple     = transparent_stipple;
color.fg_value   = expect;

/* 8 bit unpacked mode, which means we get 1 byte per longword  */

for (rotate_mask=0xff,s=1,j=0;s&1 && j<4;j++,rotate_mask<<=8)
{
 /* rotate the destination for the desired byte, which */
 /* would be byte 0,1,2 or 3 for packed mode           */

 offset           = data_ptr->sfbp$l_console_offset;
 vram             = (int *)offset;
 stip.src_address = (int)vram;

 expect           = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;
 visual.depth     = packed_8_unpacked;
 visual.rotation  = (rotate_source_0<<2) | rot[j];
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = stip.mode;

 if (data_ptr->sfbp$l_print)
        printf ("rotation %d expect 0x%x mask 0x%x \n",j,expect&rotate_mask,rotate_mask);

 /* Make Sure All Bytes Are Zero First  */ 
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,vram,0,num_bytes);                      

 /* Now Stipple with rotate function    */
 (*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

 for(i=0;s&1&&i<num_bytes/4;i++,vram++)
 {
  /* First get the expect data from source      */
  bytemask = (i%4);
  expect = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;
  expect >>= (bytemask<<3);

  /* and then rotate it right as necessary            */
  expect <<= (j<<3);
  
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  if ((actual&rotate_mask) != (expect&rotate_mask) || data_ptr->sfbp$l_test_error )
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;                                     
      param[1] = expect&rotate_mask ;                                       
      param[2] = actual&rotate_mask ;                                        
      s=0;                                                      
      }  
  }
 }

return (s);
}



/*+
* ===========================================================================
* = stip$$stipple_12_lo_vis_test - Stipple 12 bit packed Visual Test = 
* ===========================================================================
*                                  
* OVERVIEW:                        
*       12 bit lo visual           
*                                  
* FORM OF CALL:                    
*       stip$$stipple_12_lo_vis_test ( argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Verify 12 bit visual.      
*                                  
--*/                               
int     stip$$stipple_12_lo_vis_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])
                                   
{                                  
register        int             i,s=1,num_bytes;
register        unsigned int    expect,actual,*vram;
register        int             offset,mask_lo;
COLOR           color;             
STIP            stip;              
VISUAL          visual;            
Z               z;                 
                                   
offset         = data_ptr->sfbp$l_console_offset;
vram           = (unsigned int *)offset;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
                                   
CHECK_CONFIG();                    
                                   
stip.src_address = (int)vram;                    
stip.size        = num_bytes;                 
stip.mode        = GXcopy;                    
stip.stipple     = transparent_stipple;       
                                              
/* set the visual to 12 bit low, which would be the low 4 bits  */
/* of the foreground register for each respective register      */
                                              
visual.depth     = source_12_low_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.boolean   = stip.mode;

/* Pass 3 12 bit visuals fixed   */
/* Data must be in low 12 bits   */

expect           = data_ptr->sfbp$l_etch_revision < TgaPass3 ? sfbp$k_def_lw_green : 0xCCC;
expect          &= data_ptr->sfbp$l_pixel_mask;             
color.fg_value   = expect;                                  
mask_lo          = data_ptr->sfbp$l_etch_revision < TgaPass3 ? 0xff0f0f0f : 0xfff;

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,mask_lo,LW);

(*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);
           
for(i=0;s&1&&i<num_bytes/4;i++,vram++)
 {         
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  if ((actual&mask_lo) != (expect&mask_lo) )                                
      {    
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;                                     
      param[1] = expect&mask_lo;                                       
      param[2] = actual&mask_lo;                                        
      s=0;                                                      
      }    
 }         
           
return (s);
}



/*+
* ===========================================================================
* = stip$$stipple_12_hi_vis_test - Stipple 8 bit packed Visual Test = 
* ===========================================================================
*
* OVERVIEW:
*       12 bit hi visual 
* 
* FORM OF CALL:
*       stip$$stipple_12_hi_vis_test ( argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       12 bit hi visual
*
--*/
int     stip$$stipple_12_hi_vis_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             i,s=1,num_bytes;
register        unsigned int    expect,actual,*vram;
register        int             offset,mask_hi;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;

offset         = data_ptr->sfbp$l_console_offset;
vram           = (unsigned int *)offset;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;

CHECK_CONFIG();

stip.src_address = (int)vram;
stip.size        = num_bytes;
stip.mode        = GXcopy;
stip.stipple     = transparent_stipple;
                                       
visual.depth     = source_12_hi_dest;  
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;              
visual.doz       = FALSE;
visual.boolean   = stip.mode;

/* Pass 3 12 bit visuals fixed   */
expect           = data_ptr->sfbp$l_etch_revision < TgaPass3 ? sfbp$k_def_lw_green : 0xCCC;
expect          &= data_ptr->sfbp$l_pixel_mask;                 
color.fg_value   = expect;                           
mask_hi          = data_ptr->sfbp$l_etch_revision < TgaPass3 ? 0xfff0f0f0 : 0xfff000;
                                                            
(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,mask_hi,LW);

(*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

expect           = data_ptr->sfbp$l_etch_revision < TgaPass3 ? sfbp$k_def_lw_green : 0xCCC000;
expect          &= data_ptr->sfbp$l_pixel_mask;
for(i=0;s&1&&i<num_bytes/4;i++,vram++)
 {
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  if ((actual&mask_hi) !=(expect&mask_hi) )
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;                                     
      param[1] = expect&mask_hi ;                                       
      param[2] = actual&mask_hi;                                        
      s=0;                                                      
      }  
 }
return (s);
}

                       
                       
                      
/*+                    
* ===========================================================================
* = copy$$pack_8_unpack_test - packed to unpacked test =
* ===========================================================================
*                      
* OVERVIEW:            
*       packed to unpacked copy 8 bit visual
*                      
* FORM OF CALL:        
*       copy$$pack_8_unpack_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       packed to unpacked copy 8 bit visual
*
--*/
int     copy$$pack_8_unpack_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  bytemask,i,j,s=1,num_bytes;
register int  expect,actual,shift,*vram;
register char *src,*dst;
SCOPY   copy;
VISUAL   visual;
Z        z;

CHECK_CONFIG();

expect         = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
shift          = 0;
src            = (char *)data_ptr->sfbp$l_console_offset;
dst            = (char *)(src+(data_ptr->sfbp$l_bytes_per_row*(data_ptr->sfbp$l_cursor_max_row/2)));
vram           = (int *)dst;

(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,expect,num_bytes);                      
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,dst,0,num_bytes);                      

copy.src_address        = (int)src;
copy.dst_address        = (int)dst;
copy.size               = num_bytes;
copy.copy               = simple;
copy.boolean_op         = GXcopy;
copy.copymask           = M1;
copy.pixel_shift        = 0;

/* Set up the visual for 8 bit packed which is 8 bytes per longword     */
/* typically this is done in offscreen memory where we do not worry     */
/* about the window types. But I will set up the foreground             */
/* with a window type of zero and overlay zero (topy byte = 0)          */
/* The destination will be unpacked format, which is 1 byte per longword*/
/* which means we have to take that into account when setting up the    */
/* pointers for checking the data ( ACTION ITEM)                        */

visual.depth     = packed_8_unpacked;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.boolean   = copy.boolean_op;

(*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

for(i=j=0,s=1;s&1&&i<num_bytes;i+=4,vram++,j++)
 {
  bytemask = (j%4);
  expect   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;
  expect >>= (bytemask<<3);
  
  actual  = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  if ((actual&0xff) != (expect&0xff) || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;              
      param[1] = expect &0xff;               
      param[2] = actual &0xff;                
      s=0;                              
      }  
  }
return (s);
}



/*+
* ===========================================================================
* = copy$$unpack_8_pack_test - unpacked to packed test =
* ===========================================================================
*
* OVERVIEW:
*       unpacked to packed copy 8 bit visual
* 
* FORM OF CALL:
*       copy$$unpack_8_pack_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       unpacked to packed copy 8 bit visual
*
--*/
int     copy$$unpack_8_pack_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int      i,j,s=1,num_bytes;
register int  expect,actual,shift,*vram;
register char *src,*dst;
SCOPY   copy;
VISUAL   visual;
STIP     stip;
Z        z;
COLOR    color;

CHECK_CONFIG();

num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
expect         = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;
shift          = 0;
src            = (char *)data_ptr->sfbp$l_console_offset;
dst            = (char *)(src+(data_ptr->sfbp$l_bytes_per_row*(data_ptr->sfbp$l_cursor_max_row/2)));
vram           = (int *)dst;

stip.src_address = (int)src;
stip.size        = num_bytes;
stip.mode        = GXcopy;
stip.stipple     = transparent_stipple;

color.fg_value   = expect;

visual.depth     = packed_8_unpacked;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.boolean   = stip.mode;

 /* Make Sure All Bytes Are Zero First  */ 
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,0,num_bytes);                      

(*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

/* Clear out the destination also                                       */
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,dst,0,num_bytes);                      

copy.src_address        = (int)src;
copy.dst_address        = (int)dst;
copy.size               = num_bytes;
copy.copy               = simple;
copy.boolean_op         = GXcopy;
copy.copymask           = M1;
copy.pixel_shift        = 0;

/* Set up the visual for 8 bit packed which is 8 bytes per longword     */
/* typically this is done in offscreen memory where we do not worry     */
/* about the window types. But I will set up the foreground             */
/* with a window type of zero and overlay zero (topy byte = 0)          */
/* The destination will be unpacked format, which is 1 byte per longword*/
/* which means we have to take that into account when setting up the    */
/* pointers for checking the data ( ACTION ITEM)                        */

visual.depth     = unpacked_8_packed;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.boolean   = copy.boolean_op;

(*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

/* The unpacked data should have been repacked                          */
/* into our desired format now, which would be my original data         */

for(i=0,s=1;s&1&&i<num_bytes;i+=4,vram++)
 {
  actual  = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  actual &= data_ptr->sfbp$l_pixel_mask;
  
  if (actual != expect || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;              
      param[1] = expect ;               
      param[2] = actual;                
      s=0;                              
      }  
  }
return (s);
}



/*+
* ===========================================================================
* = copy$$unpack_8_unpack_test - packed to unpacked test =
* ===========================================================================
*
* OVERVIEW:
*       unpacked to unpacked copy 8 bit visual
* 
* FORM OF CALL:
*       copy$$unpack_8_unpack_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       unpacked to unpacked copy 8 bit visual
*
--*/
int     copy$$unpack_8_unpack_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  bytemask,i,j,s=1,num_bytes;
register int  expect,actual,shift,*vram;
register char *src,*dst;
SCOPY   copy;
VISUAL   visual;
Z        z;
STIP     stip;
COLOR    color;

CHECK_CONFIG();

num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
shift          = 0;
src            = (char *)data_ptr->sfbp$l_console_offset;
dst            = (char *)(src+(data_ptr->sfbp$l_bytes_per_row*(data_ptr->sfbp$l_cursor_max_row/2)));
vram           = (int *)dst;
expect         = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;

stip.src_address = (int)src;
stip.size        = num_bytes;
stip.mode        = GXcopy;
stip.stipple     = transparent_stipple;

color.fg_value   = expect;

visual.depth     = packed_8_unpacked;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.boolean   = stip.mode;

 /* Make Sure All Bytes Are Zero First  */ 
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,0,num_bytes);                      

/* Fill up vram with unpacked data now using unpacked destination       */
(*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

/* Clear out destination                                                */
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,dst,0,num_bytes);                      

copy.src_address        = (int)src;
copy.dst_address        = (int)dst;
copy.size               = num_bytes;
copy.copy               = simple;
copy.boolean_op         = GXcopy;
copy.copymask           = M1;
copy.pixel_shift        = 0;

/* lets go unpacked to unpacked now     */
visual.depth            = unpacked_8_unpacked;
visual.rotation         = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend           = FALSE;
visual.doz              = FALSE;
visual.boolean          = copy.boolean_op;

(*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

/* If we are unpacked, then we only get 1 byte per longword     */
for(i=j=0,s=1;s&1&&i<num_bytes;i+=4,j++,vram++)
 {
  bytemask = (j%4);
  expect   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;
  expect >>= (bytemask<<3);
  actual  = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  actual &= data_ptr->sfbp$l_pixel_mask;

  if ((actual&0xff) != (expect&0xff) || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;              
      param[1] = expect & 0xff ;               
      param[2] = actual & 0xff;                
      s=0;                              
      }  
  }
return (s);
}


/*+
* ===========================================================================
* = copy$$pack_8_dst_rot_test - packed to unpacked test =
* ===========================================================================
*
* OVERVIEW:
*       packed to unpacked copy wit rotate 8 bit visual
* 
* FORM OF CALL:
*       copy$$pack_8_dst_rot_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       packed to unpacked copy wit rotate 8 bit visual
*
--*/
int     copy$$pack_8_dst_rot_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  bytemask,i,j,s=1,num_bytes,rotate_mask;
register int  expect,actual,shift,*vram;
register char *src,*dst;
SCOPY   copy;
VISUAL   visual;
Z        z;
STIP     stip;
ROTATE_DEST   rot[4];

CHECK_CONFIG();

drv_desc_ptr   = (struct drv_desc  *)&scia_ptr->gfx_drv_desc;
sfbp_ioseg_ptr = (struct sfbp_ioseg *)drv_desc_ptr->ioseg_ptr;
gfx_func_ptr   = (struct gfx_func_block *)&data_ptr->sfbp$r_graphics_func;
control        = (struct sfbp_control *)sfbp_ioseg_ptr->sfbp$a_reg_space;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
shift          = 0;
rot[0]         = rotate_dest_0;
rot[1]         = rotate_dest_1;
rot[2]         = rotate_dest_2;
rot[3]         = rotate_dest_3;

rotate_mask      = data_ptr->sfbp$l_rotate_dst_mask ;

for (j=0;j<4;j++,rotate_mask <<= 8)
 {
 if (data_ptr->sfbp$l_print)printf ("rotate %d\n",j);
 src  = (char *)data_ptr->sfbp$l_console_offset;
 dst  = (char *)(src+(data_ptr->sfbp$l_bytes_per_row*(data_ptr->sfbp$l_cursor_max_row/2)));
 vram = (int *)dst;

 expect  = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,expect,num_bytes);                      
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,dst,0,num_bytes);                      

 copy.src_address        = (int)src;
 copy.dst_address        = (int)dst;
 vram                    = (int *)dst;

 copy.size               = num_bytes;
 copy.copy               = simple;
 copy.boolean_op         = GXcopy;
 copy.copymask           = M1;
 copy.pixel_shift        = 0;
 
 /* rotate the destination for the desired byte, which */
 /* would be byte 0,1,2 or 3 for packed mode           */
 
 visual.depth     = unpacked_8_unpacked;
 visual.rotation  = (rotate_source_0<<2) | rot[j];
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);
 
 for(i=0,s=1;s&1&&i<num_bytes;i+=4,vram++)
  {
  /* First get the expected byte        */
  bytemask = (i%4);
  expect   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;
  expect >>= (bytemask<<3);

  /* And then rotate                    */
  expect <<= (j<<3);

  actual  = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,dst);
  if ((actual&rotate_mask) != (expect&rotate_mask) || data_ptr->sfbp$l_test_error )
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;              
      param[1] = expect&rotate_mask;               
      param[2] = actual&rotate_mask;                
      s=0;                              
      }  
  }                                     /* End checking data loop       */

 }                                      /* End rotate loop              */
 
return (s);
}



/*+
* ===========================================================================
* = copy$$src_lo_12_test - packed to unpacked test =
* ===========================================================================
*
* OVERVIEW:
*       12 bit visual lo copy
* 
* FORM OF CALL:
*       copy$$src_lo_12_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       12 bit visual lo copy
*
--*/
int     copy$$src_lo_12_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  mask_lo,i,s=1,num_bytes;
register int  expect,actual,shift,*vram;
register char *src,*dst;
STIP     stip;
SCOPY    copy;
VISUAL   visual;
COLOR    color;
Z        z;

 CHECK_CONFIG();

 expect         = 0x11223344 ;
 num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
 shift          = 0;
 src            = (char *)data_ptr->sfbp$l_console_offset;
 dst            = (char *)(src+(data_ptr->sfbp$l_bytes_per_row*(data_ptr->sfbp$l_cursor_max_row/2)));
 vram           = (int  *)dst;
 
 stip.src_address = (int)src;
 stip.size        = num_bytes;
 stip.mode        = GXcopy;
 stip.stipple     = transparent_stipple;
  
 color.fg_value   = expect;
 
 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = stip.mode;
 
 mask_lo          = 0xff0f0f0f;
 
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,mask_lo,LW);

 /* Fill in the 12 bit source   */
 /* Upper Nibble is Zero        */
 (*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1 ,LW);

 /* Clear out the destination   */
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,dst,0,num_bytes);                      
 
 copy.src_address        = (int)src;
 copy.dst_address        = (int)dst;
 copy.size               = num_bytes;
 copy.copy               = simple;
 copy.boolean_op         = GXcopy;
 copy.copymask           = M1;
 copy.pixel_shift        = 0;
 
 visual.depth     = source_12_low_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

 /* The Upper Nibble should be replicated       */
 /* So just check for expect                    */
 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);
 
 for(i=0,s=1;s&1&&i<num_bytes;i+=4,vram++)
 {
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,dst);
  if (actual != expect || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;              
      param[1] = expect;               
      param[2] = actual;                
      s=0;                              
      }  
  }

return (s);
}


/*+
* ===========================================================================
* = copy$$src_hi_12_test - packed to unpacked test =
* ===========================================================================
*
* OVERVIEW:
*       12 bit visual hi copy
* 
* FORM OF CALL:
*       copy$$src_hi_12_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       12 bit visual hi copy
*
--*/
int     copy$$src_hi_12_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  mask_hi,i,s=1,num_bytes;
register int  expect,actual,shift,*vram;
register char *src,*dst;
STIP     stip;
SCOPY    copy;
VISUAL   visual;
COLOR    color;
Z        z;

 CHECK_CONFIG();

 expect         = 0x11223344;
 num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
 shift          = 0;
 src            = (char *)data_ptr->sfbp$l_console_offset;
 dst            = (char *)(src+(data_ptr->sfbp$l_bytes_per_row*(data_ptr->sfbp$l_cursor_max_row/2)));
 vram           = (int *)dst;
 
 stip.src_address = (int)src;
 stip.size        = num_bytes;
 stip.mode        = GXcopy;
 stip.stipple     = transparent_stipple;
  
 color.fg_value   = expect;
 
 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = stip.mode;
 
 mask_hi          = 0xfff0f0f0;
 
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,mask_hi,LW);

 /* Fill in the 12 bit source   */
 (*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1 ,LW);
 /* Clear out the destination   */
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,dst,0,num_bytes);                      
 
 copy.src_address        = (int)src;
 copy.dst_address        = (int)dst;
 copy.size               = num_bytes;
 copy.copy               = simple;
 copy.boolean_op         = GXcopy;
 copy.copymask           = M1;
 copy.pixel_shift        = 0;
 visual.boolean          = copy.boolean_op;
 
 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);
 
 for(i=0,s=1;s&1&&i<num_bytes;i+=4,vram++)
 {
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,dst);
  if (actual != expect || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;              
      param[1] = expect;
      param[2] = actual;
      s=0;                              
      }  
  }

return (s);
}




/*+
* ===========================================================================
* = copy$$dmar_pack_8_pack_test - dma packed to unpacked test =
* ===========================================================================
*
* OVERVIEW:
*       8 bit visual packed dma read
* 
* FORM OF CALL:
*       copy$$dmar_pack_8_pack_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       8 bit visual packed dma read
*
--*/
int     copy$$dmar_pack_8_pack_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  *DmaBufferPtr,*ptr,bytemask,i,j,s=1,num_bytes;
register int  expect,actual,shift,*vram;
register char *src,*dst;
SCOPY   copy;
VISUAL   visual;
Z        z;

CHECK_CONFIG();

expect         = 0x11223344;
num_bytes      = SIZE_DMA_BUFFER-8;
shift          = 0;
src            = (char *)data_ptr->sfbp$l_console_offset;
vram           = (int *)src;

DmaBufferPtr   = (int *)malloc(SIZE_DMA_BUFFER);

/* Fill the source buffer now   */
/* Source Always Packed         */
for (i=0,ptr=DmaBufferPtr;i<LW_SIZE_DMA_BUFFER;i++)*ptr++ =expect;

/* Clear the destination buffer */
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,0,num_bytes);                      

/* Data DMA from copy.src_address to copy.system_address        */

copy.src_address        = (int)src;
copy.size               = num_bytes;
copy.boolean_op         = GXcopy;
copy.copymask           = M1;
copy.pixel_shift        = 0;
copy.copy               = dma_read_non_dithered;
copy.left1              = 0xf;
copy.left2              = 0xf;
copy.right1             = 0xf;
copy.right2             = 0;
copy.system_address     = (int)DmaBufferPtr;

/* Set up the visual for 8 bit packed which is 8 bytes per longword     */
/* typically this is done in offscreen memory where we do not worry     */
/* about the window types. But I will set up the foreground             */
/* with a window type of zero and overlay zero (topy byte = 0)          */
/* The destination will be unpacked format, which is 1 byte per longword*/
/* which means we have to take that into account when setting up the    */
/* pointers for checking the data ( ACTION ITEM)                        */

visual.depth     = packed_8_packed;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.boolean   = copy.boolean_op;

(*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

for(i=j=0,s=1;s&1&&i<num_bytes;i+=4,j++,vram++)
 {
  bytemask = (j%4);
  expect   = 0x11223344;
  actual   = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);

  if (actual != expect || data_ptr->sfbp$l_test_error )
      {  
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;              
      param[1] = expect ;               
      param[2] = actual ;                
      s=0;                              
      }  
  }
free (DmaBufferPtr);

return (s);
}



/*+
* ===========================================================================
* = copy$$dmar_pack_8_unpack_test - unpacked to packed test =
* ===========================================================================
*
* OVERVIEW:
*       8 bit visual unpacked dma read
* 
* FORM OF CALL:
*       copy$$dmar_pack_8_unpack_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       8 bit visual unpacked dma read
*
--*/
int     copy$$dmar_pack_8_unpack_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  *DmaBufferPtr,*ptr,i,j,s=1,num_bytes;
register int  bytemask,data,expect,actual,shift,*vram;
register char *src,*dst;
SCOPY   copy;
VISUAL   visual;
STIP     stip;
Z        z;
COLOR    color;

CHECK_CONFIG();

num_bytes      = SIZE_DMA_BUFFER-8;
expect         = 0x11223344;
shift          = 0;
src            = (char *)data_ptr->sfbp$l_console_offset;
vram           = (int *)src;
DmaBufferPtr   = (int *)malloc(SIZE_DMA_BUFFER);

/* Fill the source buffer now   */
/* Source Always Packed         */
for (i=0,ptr=DmaBufferPtr;i<LW_SIZE_DMA_BUFFER;i++)*ptr++ =expect;

/* Clear out the destination also                                       */
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,0,num_bytes);                      

copy.src_address        = (int)src;
copy.size               = num_bytes;
copy.copy               = simple;
copy.boolean_op         = GXcopy;
copy.copymask           = M1;
copy.pixel_shift        = 0;
copy.copy               = dma_read_non_dithered;
copy.left1              = 0xf;
copy.left2              = 0xf;
copy.right1             = 0xf;
copy.right2             = 0x0;
copy.system_address     = (int)DmaBufferPtr;

visual.depth     = packed_8_unpacked;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.boolean   = copy.boolean_op;

(*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

for(i=j=0,s=1;s&1&&i<num_bytes;i+=4,j++,vram++)
 {
  bytemask = (j%4);
  expect   = 0x11223344;
  expect >>= (bytemask<<3);
  
  actual  = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  if ((actual&0xff) != (expect&0xff) || data_ptr->sfbp$l_test_error )                  
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;              
      param[1] = expect &0xff;               
      param[2] = actual &0xff;                
      s=0;                              
      }  
  }

free (DmaBufferPtr);
return (s);
}



/*+
* ===========================================================================
* = copy$$dmar_pack_8_unpack_rot_test - packed to unpacked rotate test =
* ===========================================================================
*
* OVERVIEW:
*       8 bit visual unpacked dma read with rotate
* 
* FORM OF CALL:
*       copy$$dmar_pack_8_unpack_rot_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       8 bit visual unpacked dma read with rotate
*
--*/
int     copy$$dmar_pack_8_unpack_rot_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  *DmaBufferPtr,*ptr,bytemask,i,j,k,s=1,num_bytes;
register int  rotate_mask,data,expect,actual,shift,*vram;
register char *src,*dst;
SCOPY   copy;
VISUAL   visual;
Z        z;
STIP     stip;
COLOR    color;
ROTATE_DEST   rot[4];

CHECK_CONFIG();

num_bytes      = SIZE_DMA_BUFFER-8;
shift          = 0;
src            = (char *)data_ptr->sfbp$l_console_offset;
vram           = (int *)src;
expect         = 0x11223344;
rot[0]         = rotate_dest_0;
rot[1]         = rotate_dest_1;
rot[2]         = rotate_dest_2;
rot[3]         = rotate_dest_3;

DmaBufferPtr   = (int *)malloc(SIZE_DMA_BUFFER);

/* Fill the source buffer now   */
/* Source Always Packed         */
for (i=0,ptr=DmaBufferPtr;i<LW_SIZE_DMA_BUFFER;i++)*ptr++ =expect;
        
rotate_mask      = data_ptr->sfbp$l_rotate_dst_mask ;

for (k=0;k<4;k++,rotate_mask <<= 8)
 {
 vram     = (int *)src;
 expect   = 0x11223344;
  /* Clear out destination        */
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,0,num_bytes);                      

 /* DMA from from dma_buffer to copy.src_address */
 copy.src_address        = (int)src;
 copy.size               = num_bytes;
 copy.copy               = simple; 
 copy.boolean_op         = GXcopy;
 copy.copymask           = M1;
 copy.pixel_shift        = 0;
 copy.copy               = dma_read_non_dithered;
 copy.left1              = 0xf;
 copy.left2              = 0xf;
 copy.right1             = 0xf;
 copy.right2             = 0;
 copy.system_address     = (int)DmaBufferPtr;
 
 /* lets go unpacked to unpacked now     */

 visual.depth     = packed_8_unpacked;
 visual.rotation  = (rotate_source_0<<2) | rot[k];
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);
 
 if (data_ptr->sfbp$l_print)printf ("num_bytes %d \n",num_bytes);

 /* If we are unpacked, then we only get 1 byte per longword     */
 for(i=j=0,s=1;s&1&&i<num_bytes;i+=4,j++,vram++)
  {
   bytemask = (j%4);
   expect   = 0x11223344;
   expect >>= (bytemask<<3);
   expect <<= (k<<3);
   actual  = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
   if (data_ptr->sfbp$l_print)
	printf ("%d vram 0x%x  expect 0x%x actual 0x%x \n",j,
	vram, (actual&rotate_mask), (expect&rotate_mask) );

   if ((actual&rotate_mask) != (expect&rotate_mask) || data_ptr->sfbp$l_test_error )
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;              
      param[1] = expect&rotate_mask;               
      param[2] = actual&rotate_mask;                
      s=0;                              
      }  

   }
 }
 
free (DmaBufferPtr);

return (s);
}



/*+
* ===========================================================================
* = copy$$dmaw_pack_8_pack_test - dma packed to unpacked test =
* ===========================================================================
*
* OVERVIEW:
*       8 bit visual packed dma write
* 
* FORM OF CALL:
*       copy$$dmaw_pack_8_pack_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       8 bit visual packed dma write
*
--*/
int     copy$$dmaw_pack_8_pack_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  i,overlap=32,j,s=1,num_bytes;
register int  *DmaBufferPtr,*dma,expect,actual,shift,*vram;
register char *src,*dst,*ptr;
char     temp[40];
SCOPY    copy;
VISUAL   visual;
Z        z;

 CHECK_CONFIG();

 expect         = 0x11223344;
 num_bytes      = 1024;
 src            = (char *)data_ptr->sfbp$l_console_offset;
 
 DmaBufferPtr   = (int *)malloc(SIZE_DMA_BUFFER);
 
 copy.src_address = (int)src;
 copy.size        = num_bytes;
 copy.copy        = dma_write;
 copy.boolean_op  = GXcopy;
 copy.copymask    = M1;
 copy.pixel_shift = 0;
 copy.left1       = 0xff;
 copy.right1      = 0xff;

 visual.depth     = packed_8_packed ;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

 ptr              = (char *)DmaBufferPtr;
 copy.system_address     = (int)ptr;

 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,expect,num_bytes);                      
 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 dma  = DmaBufferPtr;
 for(i=0,s=1;s&1&&i<num_bytes/4;i++,dma++)
 {
  actual = *dma;
  if (data_ptr->sfbp$l_print) 
          printf ("dma_buffer[%d] expect 0x%x actual 0x%x\n",i,expect,actual);
  if (actual != expect || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = (int)dma;
      param[1] = expect ;               
      param[2] = actual;                
      s=0;
      }
  }
  
free (DmaBufferPtr);

return (s);
}



/*+
* ===========================================================================
* = copy$$dmaw_unpack_8_pack_test - unpacked to packed test =
* ===========================================================================
*
* OVERVIEW:
*       8 bit visual unpacked to packed dma write
* 
* FORM OF CALL:
*       copy$$dmaw_unpack_8_pack_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       8 bit visual unpacked to packed dma write
*
--*/
int     copy$$dmaw_unpack_8_pack_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  i,overlap=32,j,s=1,num_bytes;
register int  *DmaBufferPtr,*dma,expect,actual,shift,*vram;
register char *src,*dst,*ptr;
char     temp[40];
SCOPY    copy;
VISUAL   visual;
STIP     stip;
Z        z;
COLOR    color;

 CHECK_CONFIG();

 expect         = 0x11223344;
 num_bytes      = 1024;
 src            = (char *)data_ptr->sfbp$l_console_offset;
 
 DmaBufferPtr   = (int *)malloc(SIZE_DMA_BUFFER);
 
 stip.src_address = (int)src;
 stip.size        = num_bytes*4;
 stip.mode        = GXcopy;
 stip.stipple     = transparent_stipple;
 color.fg_value   = expect;

 visual.depth     = packed_8_unpacked ;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = stip.mode;

 /* Fill in some unpacked data first    */
 (*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

 /* And now copy it to packed world     */
 copy.src_address = (int)src;

 /* Size * 4 since unpacked to packed   */
 copy.size        = num_bytes*4;

 copy.copy        = dma_write;
 copy.boolean_op  = GXcopy;
 copy.copymask    = M1;
 copy.pixel_shift = 0;
 copy.left1       = 0xff;
 copy.right1      = 0xff;

 ptr              = (char *)DmaBufferPtr;
 copy.system_address     = (int)ptr;

 visual.depth     = unpacked_8_packed ;
 visual.boolean   = copy.boolean_op;

 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 dma = DmaBufferPtr;
 for(i=0,s=1;s&1&&i<num_bytes/4;i++,dma++)
 {
  actual = *dma;
  if (data_ptr->sfbp$l_print) 
          printf ("dma_buffer[%d] expect 0x%x actual 0x%x\n",i,expect,actual);
  if (actual != expect || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = (int)dma;
      param[1] = expect ;               
      param[2] = actual;                
      s=0;
      }
  }
  
 free (DmaBufferPtr);

 return (s);
}



/*+
* ===========================================================================
* = line$$line_8_pack_vis_test - line 8 bit packed Visual Test = 
* ===========================================================================
*
* OVERVIEW:
*       8 bit visual line draw
* 
* FORM OF CALL:
*       line$$line_8_pack_vis_test ( argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       8 bit visual line draw
*
--*/
int     line$$line_8_pack_vis_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int      s=1,fg=TRUE;
int      method,startx,starty,endx,endy;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;

CHECK_CONFIG();

startx           =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
endx             =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;
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
line.window_type = Xwindow;
line.nogo        = go;
line.draw_method = line_address;

line.xa          = startx;
line.ya          = starty;
line.xb          = endx;
line.yb          = endy;

visual.capend    = FALSE;
visual.depth     = packed_8_packed ; 
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;
visual.doz       = FALSE;

color.method     = foreground;
color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

(*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,0);

return (s);
}




/*+
* ===========================================================================
* = line$$line_8_unpack_vis_test - line 8 bit packed Visual Test = 
* ===========================================================================
*
* OVERVIEW:
*       8 bit visual unpacked line draw
* 
* FORM OF CALL:
*       line$$line_8_unpack_vis_test ( argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       8 bit visual unpacked line draw
*
--*/
int     line$$line_8_unpack_vis_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int      s=1,fg=TRUE;
int      method,startx,starty,endx,endy;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;

CHECK_CONFIG();

startx           =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
endx             =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;
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
line.window_type = Xwindow;
line.nogo        = go;
line.draw_method = line_address;

line.xa          = startx;
line.ya          = starty;
line.xb          = endx;
line.yb          = endy;

visual.capend    = FALSE;
visual.depth     = packed_8_unpacked ; 
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;
visual.doz       = FALSE;

color.method     = foreground;
color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

(*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,0);

return (s);
}



/*+
* ===========================================================================
* = line$$line_8_unpack_rot_test - line 8 bit packed Visual Test = 
* ===========================================================================
*
* OVERVIEW:
*       8 bit visual unpacked line draw with rotate
* 
* FORM OF CALL:
*       line$$line_8_unpack_rot_test ( argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       8 bit visual unpacked line draw with rotate
*
--*/
int     line$$line_8_unpack_rot_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int      s=1,i,j,fg=TRUE;
int      method,startx,starty,endx,endy;
ROTATE_DEST     rot[4];
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;

CHECK_CONFIG();

startx           =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
endx             =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;
starty           =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
endy             =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;

rot[0]           = rotate_dest_0;
rot[1]           = rotate_dest_1;
rot[2]           = rotate_dest_2;
rot[3]           = rotate_dest_3;

line.line        = transparent_line_mode,
line.span_line   = FALSE;
line.speed       = fast;
line.boolean_op  = GXcopy;
line.verify      = TRUE;
line.polyline    = FALSE;
line.wait_buffer = FALSE;
line.color_change= TRUE;
line.window_type = Xwindow;
line.draw_method = line_address;

line.nogo        = go;

line.xa          = startx;
line.ya          = starty;
line.xb          = endx;
line.yb          = endy;

for (j=0,s=1;s&1 && j<4;j++)
 {
 if (data_ptr->sfbp$l_print) printf ("line rotate %d\n",j);
 visual.capend    = FALSE;
 visual.depth     = packed_8_unpacked ; 
 visual.rotation  = (rotate_source_0<<2) | rot[j];
 visual.boolean   = line.boolean_op;
 visual.doz       = FALSE;
 
 color.method     = foreground;
 color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
 color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;
 
 (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
 s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,0);
 }
  
return (s);
}



/*+
* ===========================================================================
* = line$$line_12_packlo_vis_test - line 12 bit packed Visual Test = 
* ===========================================================================
*
* OVERVIEW:
*       12 bit visual lo line
* 
* FORM OF CALL:
*       line$$line_12_packlo_vis_test ( argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       12 bit visual lo line
*
--*/
int     line$$line_12_packlo_vis_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int      s=1,fg=TRUE;
int      method,startx,starty,endx,endy;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;

CHECK_CONFIG();

startx           =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
endx             =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;
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
line.window_type = Xwindow;
line.nogo        = go;
line.draw_method = line_address;

line.xa          = startx;
line.ya          = starty;
line.xb          = endx;
line.yb          = endy;

visual.capend    = FALSE;
visual.depth     = source_12_low_dest; 
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;
visual.doz       = FALSE;

color.method     = foreground;
color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

(*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,0);

return (s);
}




/*+
* ===========================================================================
* = line$$line_12_packhi_vis_test - line 12 bit packed Visual Test = 
* ===========================================================================
*
* OVERVIEW:
*       12 bit visual lo line
* 
* FORM OF CALL:
*       line$$line_12_packhi_vis_test ( argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       12 bit visual hi line
*
--*/
int     line$$line_12_packhi_vis_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int      s=1,fg=TRUE;
int      method,startx,starty,endx,endy;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;

CHECK_CONFIG();

startx           =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/2;
endx             =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;
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
line.window_type = Xwindow;
line.nogo        = go;
line.draw_method = line_address;

line.xa          = startx;
line.ya          = starty;
line.xb          = endx;
line.yb          = endy;

visual.capend    = FALSE;
visual.depth     = source_12_hi_dest; 
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.boolean   = line.boolean_op;
visual.doz       = FALSE;

color.method     = foreground;
color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;

(*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,0);

return (s);
}

