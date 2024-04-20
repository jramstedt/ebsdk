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

#define CHECK_CONFIG() {if (data_ptr->sfbp$l_module != hx_32_plane_z  && \
                            data_ptr->sfbp$l_module != hx_32_plane_no_z )return(1);}
                            

/* These are constants used by the cfg_read/cfg_write code      */
#define   PCI_byte_mask 0
#define   PCI_word_mask 1
#define   PCI_long_mask 3

void     pci_outmem(unsigned __int64 address, unsigned int value, int length);
unsigned int pci_inmem_byte (unsigned __int64 address, int length);
unsigned int pci_inmem      (unsigned __int64 address, int length );


/*+
* ===========================================================================
* = stip$$stipple_fg_test - Stipple Foreground Color Test Routine =
* ===========================================================================
*
* OVERVIEW:
*       Stipple Foreground Color Test 
* 
* FORM OF CALL:
*       stip$$stipple_fg_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Transparent stipple test.
--*/
int     stip$$stipple_fg_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             i,s=1,num_bytes;
register        unsigned int    expect,actual,*vram;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;

vram           = (unsigned int *)data_ptr->sfbp$l_console_offset;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
expect         = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;

stip.src_address = (int)vram;
stip.size        = num_bytes;
stip.mode        = GXcopy;
stip.stipple     = transparent_stipple;
color.fg_value   = expect;

visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.line_type = 0;
visual.boolean   = stip.mode;

(*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

for(i=0;s&1&&i<num_bytes/4;i++,vram++)
 {
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  if (data_ptr->sfbp$l_print) printf ("STIP 0x%x = 0x%x\n",vram,actual);
  if ( (actual !=expect) || data_ptr->sfbp$l_test_error )                                
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;                                     
      param[1] = expect ;                                       
      param[2] = actual;                                        
      s= data_ptr->sfbp$l_conte ? 1 : 0;                                                      
      }  
 }
return (s);
}




/*+
* ===========================================================================
* = stip$$stipple_bg_test - Stipple Background Color Test Routine =
* ===========================================================================
*
* OVERVIEW:
*       Stipple Background Color Test 
* 
* FORM OF CALL:
*       stip$$stipple_bg_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Backgrount Stipple mode test. 
*
--*/
int     stip$$stipple_bg_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             dst,i,s=1,num_bytes;
register        unsigned int    expect,actual,*vram;
register        unsigned char   *src;
STIP            stip;
COLOR           color;
VISUAL          visual;
Z               z;

vram           = (unsigned int *)data_ptr->sfbp$l_console_offset;
expect         = sfbp$k_def_lw_blue & data_ptr->sfbp$l_pixel_mask ;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;

stip.src_address  = (int)vram;
stip.size         = num_bytes;
stip.mode         = GXcopy;
stip.stipple      = opaque_stipple;

color.bg_value    = expect;

visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.line_type = 0;
visual.boolean   = stip.mode;

(*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

for(i=0;s&1&&i<num_bytes/4;i++,vram++ )        
 {
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  if (data_ptr->sfbp$l_print) printf ("STIP 0x%x = 0x%x\n",vram,actual);
  if ( (actual !=expect) || data_ptr->sfbp$l_test_error )                                
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;                                     
      param[1] = (int)expect ;                                       
      param[2] = actual;                                        
      s= data_ptr->sfbp$l_conte ? 1: 0;                                                      
      }  
 }

return (s);
}


/*+
* ===========================================================================
* = stip$$stipple_inc_test - Stipple Incremental Test Routine =
* ===========================================================================
*
* OVERVIEW:
*       Stipple Incremental Test 
* 
* FORM OF CALL:
*       stip$$stipple_inc_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Opaque stipple for 1 to 32 pixels.
*
--*/
int     stip$$stipple_inc_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int i,j,s=1,bytemask,num_bytes,*vram,expect;
unsigned int    actual;
unsigned char   *start;
STIP            stip;
COLOR           color;
VISUAL          visual;
Z               z;

expect          = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;
num_bytes       = data_ptr->sfbp$l_bytes_per_scanline;                      

for (i=1,s=1;s&1&&i<=32;i++)
 {
 start            = (unsigned char *)data_ptr->sfbp$l_console_offset;
 vram             = (int *)start;
 stip.src_address = (int)vram;
 stip.size        = i;
 stip.mode        = GXcopy;
 stip.stipple     = opaque_stipple;
 
 color.bg_value   = expect;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = 0;
 visual.boolean   = stip.mode;
 
(*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

 for(j=0;s&1&&j<i;j++)        
  {
  actual   = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  bytemask = j%4;
  actual  &= 0xff;
  expect  &= 0xff;
  if ( (actual!=expect) || data_ptr->sfbp$l_test_error )                                
       { 
       param[0] = data_ptr->sfbp$l_vram_base | (int)vram;                                     
       param[1] = expect ;                                       
       param[2] = actual;                                        
       s=0;                                                      
       }  
  if (bytemask==3)vram++;
  }
 }
return (s);
}



/*+
* ===========================================================================
* = stip$$stipple_block_test - Stipple Foreground Color Test Routine =
* ===========================================================================
*
* OVERVIEW:
*       Stipple block test
* 
* FORM OF CALL:
*       stip$$stipple_block_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Stipple mode test. This will use block mode stipple which is 
*       similar to transparent stipple mode,except the block color
*       registers are set up. These are registers in the vram and 
*       it's much faster to use block stipple.
*
--*/
int     stip$$stipple_block_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             i,s=1,num_bytes;
register        unsigned int    expect,actual,*vram;
register        int             offset;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;

offset         = data_ptr->sfbp$l_console_offset;
offset        += 5*data_ptr->sfbp$l_bytes_per_row;
vram           = (unsigned int *)offset;
expect         = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;

stip.src_address = (int)vram;
stip.size        = num_bytes;
stip.mode        = GXcopy;
stip.stipple     = transparent_block_stipple;

color.fg_value   = expect;

visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.boolean   = stip.mode;

(*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

for(i=0;s&1&&i<num_bytes/4;i++,vram++)
 {
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  if ( (actual !=expect) || data_ptr->sfbp$l_test_error )                                
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
* = stip$$stipple_block_fill_test - Stipple Foreground Color Test Routine =
* ===========================================================================
*
* OVERVIEW:
*       Stipple Block Fill Test 
* 
* FORM OF CALL:
*       stip$$stipple_block_fill_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Stipple mode test. This will use block_fill mode stipple which is 
*       similar to transparent stipple mode,except the block_fill color
*       registers are set up. These are registers in the vram and 
*       it's much faster to use block_fill stipple. 
*
--*/
int     stip$$stipple_block_fill_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             offset,i,j,s=1,num_bytes;
register        unsigned int    expect,actual,*vram;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;


/* Lets do block fills of 1024 multiples to verify driver       */
/* Then I could use block fill for the console driver           */

for (s=1,j=1;s&1 && j<=4;j++)
 {
 offset         = data_ptr->sfbp$l_console_offset;
 offset        += 10*data_ptr->sfbp$l_bytes_per_row;
 vram           = (unsigned int *)offset;
 expect         = sfbp$k_def_lw_purple & data_ptr->sfbp$l_pixel_mask;
 num_bytes      = j*data_ptr->sfbp$l_bytes_per_scanline;

 stip.src_address = (int)vram;
 stip.size        = num_bytes;
 stip.mode        = GXcopy;
 stip.stipple     = transparent_block_fill;

 color.fg_value   = expect;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = 0;
 visual.boolean   = stip.mode;

 (*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

 for(i=0;s&1&&i<num_bytes/4;i++,vram++)
 {
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  if ((actual !=expect) || data_ptr->sfbp$l_test_error )                                
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;                                     
      param[1] = expect ;                                       
      param[2] = actual;                                        
      s=0;                                                      
      }  
 }
}

return (s);
}




/*+
* ===========================================================================
* = stip$$stipple_block_stip_align_test - Stipple Foreground Color Test Routine =
* ===========================================================================
*
* OVERVIEW:
*       Stipple block alignment test 
* 
* FORM OF CALL:
*       stip$$stipple_block_stip_align_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Stipple block alignment test 
*
--*/
int     stip$$stipple_block_stip_align_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             i,j,s=1,num_bytes;
register        unsigned int    expect,actual,*vram;
register        int             offset;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;

/* This test will do odd block stipple alignments       */

for (j=0;j<4;j++)
 {
 offset         = data_ptr->sfbp$l_console_offset;
 offset        += (5*data_ptr->sfbp$l_bytes_per_row)+j;
 vram           = (unsigned int *)offset;
 expect         = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;
 num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;

 stip.src_address = (int)vram;
 stip.size        = num_bytes;
 stip.mode        = GXcopy;
 stip.stipple     = transparent_block_stipple;
 
 color.fg_value   = expect;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = stip.mode;

 (*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

 for(i=0;s&1&&i<num_bytes/4;i++,vram++)
  {
   actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
   if ((actual !=expect) || data_ptr->sfbp$l_test_error )                                
       { 
       param[0] = data_ptr->sfbp$l_vram_base | (int)vram;                                     
       param[1] = expect ;                                       
       param[2] = actual;                                        
       s=0;                                                      
       }  
  }
 }

return (s);
}





/*+
* ===========================================================================
* = stip$$stipple_block_fill_align_test - Stipple Foreground Color Test Routine =
* ===========================================================================
*
* OVERVIEW:
*       Stipple block fill alignment test 
* 
* FORM OF CALL:
*       stip$$stipple_block_fill_align_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Stipple block alignment test 
*
--*/
int     stip$$stipple_block_fill_align_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             i,j,s=1,num_bytes;
register        unsigned int    expect,actual,*vram;
register        int             offset;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;

/* This test will do odd block stipple alignments       */

for (j=0;j<4;j++)
 {
 offset         = data_ptr->sfbp$l_console_offset;
 offset        += (5*data_ptr->sfbp$l_bytes_per_row)+j;
 vram           = (unsigned int *)offset;
 expect         = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;
 num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;

 stip.src_address = (int)vram;
 stip.size        = num_bytes;
 stip.mode        = GXcopy;
 stip.stipple     = transparent_block_fill;
 
 color.fg_value   = expect;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = stip.mode;

 (*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

 for(i=0;s&1&&i<num_bytes/4;i++,vram++)
  {
   actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
   if ((actual !=expect) || data_ptr->sfbp$l_test_error )                                
       { 
       param[0] = data_ptr->sfbp$l_vram_base | (int)vram;                                     
       param[1] = expect ;                                       
       param[2] = actual;                                        
       s=0;                                                      
       }  
  }
 }
  
return (s);
}





/*+
* ===========================================================================
* = stip$$transparent_fill_test - Stipple Foreground Color Test Routine =
* ===========================================================================
*
* OVERVIEW:
*       Stipple transparent fill test 
* 
* FORM OF CALL:
*       stip$$transparent_fill_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Stipple transparent fill test 
*
--*/
int     stip$$transparent_fill_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             offset,i,s=1,num_bytes;
register        unsigned int    expect,actual,*vram;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;

offset         = data_ptr->sfbp$l_console_offset;
offset        += 10*data_ptr->sfbp$l_bytes_per_row;
vram           = (unsigned int *)offset;
expect         = sfbp$k_def_lw_purple & data_ptr->sfbp$l_pixel_mask;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;

stip.src_address = (int)vram;
stip.size        = num_bytes;
stip.mode        = GXcopy;
stip.stipple     = transparent_fill;

color.fg_value   = expect;

visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.line_type = 0;
visual.boolean   = stip.mode;

(*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

for(i=0;s&1&&i<num_bytes/4;i++,vram++)
 {
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  if ((actual !=expect) || data_ptr->sfbp$l_test_error )                                
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
* = stip$$opaque_fill_test - Stipple Foreground Color Test Routine =
* ===========================================================================
*
* OVERVIEW:
*       Stipple opaque fill test 
* 
* FORM OF CALL:
*       stip$$opaque_fill_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Stipple opaque fill test 
*
--*/
int     stip$$opaque_fill_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             offset,i,s=1,num_bytes;
register        unsigned int    expect,actual,*vram;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;

offset         = data_ptr->sfbp$l_console_offset;
offset        += 10*data_ptr->sfbp$l_bytes_per_row;
vram           = (unsigned int *)offset;
expect         = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;

stip.src_address = (int)vram;
stip.size        = num_bytes;
stip.mode        = GXcopy;
stip.stipple     = opaque_fill;

color.fg_value   = expect;

visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
visual.capend    = FALSE;
visual.doz       = FALSE;
visual.line_type = 0;
visual.boolean   = stip.mode;

(*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

for(i=0;s&1&&i<num_bytes/4;i++,vram++)
 {
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
  if ((actual !=expect) || data_ptr->sfbp$l_test_error )                                
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
* = stip$$stipple_vram_test - Stipple VRAM Test =
* ===========================================================================
*             
* OVERVIEW:   
*       Stipple Video Ram Write Test 
*             
* FORM OF CALL:                                                               
*       stip$$stipple_vram_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*                                                             
* ARGUMENTS:                                                  
*       argc        - argument count                          
*       argv        - argument array                          
*       ccv         - callback vector                         
*       scia_ptr    - shared driver pointer                   
*       data_ptr    - driver data pointer                     
*       param       - parameter array for error information   
*                                                             
--*/                                                          
int     stip$$stipple_vram_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])
                                                              
{             
register  int       i,j,k,s=1;
register  int       *top,base,*vram,*lwsr,actual,data;
register  int       num_bytes,num_lws,num_segs;
register  int       SourceVisual,DestVisual,iteration_count,stipple_count;
                      
 /* This starts at bottom of memory and goes to the top	*/
 if (data_ptr->sfbp$l_manufacturing_mode)
 {            
 vram           = (int *)sfbp_ioseg_ptr->sfbp$a_vram_mem_ptr;
 num_bytes      = data_ptr->sfbp$l_vram_size;
 num_lws        = num_bytes/4;                                                     
 iteration_count= 33*3;
 stipple_count  = data_ptr->sfbp$l_module == hx_8_plane ? 8:32;
 data           = M1;                                    
 SourceVisual   = data_ptr->sfbp$l_module == hx_8_plane ? SourceVisualPacked: SourceVisualTrue;
 DestVisual     = data_ptr->sfbp$l_module == hx_8_plane ? DestVisualPacked  : DestVisualTrue;
                                                                                                
 (*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,0,0x55555555,num_bytes);                       
                                                                                                
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_foreground,M1,LW);                          
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,(SourceVisual<<8)|sfbp$k_transparent_stipple,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_mask,M1,LW);                          
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,(DestVisual<<8)|GXxor,LW);       
                            
 top = vram + num_lws;                                                   
 for (j=0;j<iteration_count&&!io_poll();j++)  
  for (k=0,base=0;k<(1024/stipple_count);k++,base+=stipple_count*13)
    for (lwsr=vram+(base&0x3ff);lwsr<top;lwsr += 1024)                      
       pci_outmem (lwsr,data,PCI_long_mask);        

 /* Restore to simple mode      */                                          
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);                
                                                                            
 if (j&1) data = 0xaaaaaaaa;                                                
 else data = 0x55555555;                                                    
 for (base=0;base<num_lws;base++,vram++)                                    
   if ((actual = pci_inmem (vram,PCI_long_mask) ) != data)                  
      {                                                                                 
      param[0] = data_ptr->sfbp$l_vram_base | (int) vram;                   
      param[1] = data;                                                      
      param[2] = actual;                                                    
      break;                                                                
      }                                                                     
  s = (base == num_lws) ? 1: 0;                                             
                                                                            
  if (data_ptr->sfbp$l_cursor_offchip )                                     
   {                                                                        
   (*bt_func_ptr->pv_bt_cursor_ram) (scia_ptr,data_ptr);                    
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_video_valid,CursorEnable|ActiveVideo,LW);
   }                                                                        
                                                                            
 }                                                                          
                                                                            
 return (s);                                                                
}                                                                           
