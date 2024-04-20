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
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"

#define SIZE_DMA_BUFFER    (16384+32)
#define LW_SIZE_DMA_BUFFER (SIZE_DMA_BUFFER/4)

#define CHECK_CONFIG() {if (data_ptr->sfbp$l_module != hx_32_plane_z  && \
                            data_ptr->sfbp$l_module != hx_32_plane_no_z )return(1);}

static  int      byte_sizes[]= {1,2,3,4,5,6,7,8,9,13,17,25,31,33};

struct  dma_read_test_type 
        {
        int     mode;
        int     size;
        int     depth;
        int     rotation;
        int     left1;
        int     left2;
        int     right1;
        int     right2;
        };      

struct  dma_write_test_type 
        {
        int     mode;
        int     size;
        int     depth;
        int     rotation;
        int     left1;
        int     right1;
        };      
                
typedef struct dma_read_test_type DMA_READ_TEST;
typedef struct dma_write_test_type DMA_WRITE_TEST;

#define MAX_DMA_WORDS 512
#define MAX_DMA_BYTES 2048

static   DMA_READ_TEST dma_read_table[] = 
 {                                                                              /* left 1 left2 right1 right2 */
 dma_read_non_dithered,   1,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0x0,0x0,0x1,0x0, 
 dma_read_non_dithered,   2,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0x0,0x0,0x3,0x0, 
 dma_read_non_dithered,   3,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0x0,0x0,0x7,0x0, 
 dma_read_non_dithered,   4,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0x0,0x0,0xf,0x0, 
 dma_read_non_dithered,   5,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0x0,0x1,0x0, 
 dma_read_non_dithered,   6,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0x0,0x3,0x0, 
 dma_read_non_dithered,   7,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0x0,0x7,0x0, 
 dma_read_non_dithered,   8,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0x0,0xf,0x0, 
 dma_read_non_dithered,   9,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0x1,0x0, 
 dma_read_non_dithered,  10,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0x3,0x0, 
 dma_read_non_dithered,  11,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0x7,0x0, 
 dma_read_non_dithered,  12,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0xf,0x0, 
 dma_read_non_dithered,  32,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0xf,0x0, 
 dma_read_non_dithered,  64,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0xf,0x0, 
 dma_read_non_dithered, 128,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0xf,0x0, 
 dma_read_non_dithered, 256,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0xf,0x0, 
 dma_read_non_dithered, 512,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0xf,0x0, 
 dma_read_non_dithered, 768,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0xf,0x0, 
 dma_read_non_dithered,1024,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0xf,0x0, 
 dma_read_non_dithered,1536,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0xf,0x0, 
 dma_read_non_dithered,2044,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0xf,0x0, 
 dma_read_non_dithered,2048,packed_8_packed,(rotate_source_0<<2)|(rotate_dest_0),0xf,0xf,0xf,0x0, 
 };

static   DMA_WRITE_TEST dma_write_table[] = 
        {                                                                    /* left right      */

/* Pass 1 Asic used to fail from 1 to 16 bytes	*/

        dma_write,   1,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x00,0x01, 
        dma_write,   2,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x00,0x03, 
        dma_write,   3,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x00,0x07, 
        dma_write,   4,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x00,0x0f, 
        dma_write,   5,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x00,0x1f, 
        dma_write,   6,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x00,0x3f, 
        dma_write,   7,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x00,0x7f, 
        dma_write,   8,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x00,0xff, 
        dma_write,   9,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0x01, 
        dma_write,  10,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0x03, 
        dma_write,  11,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0x07, 
        dma_write,  12,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0x0f, 
        dma_write,  13,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0x1f, 
        dma_write,  14,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0x3f, 
        dma_write,  15,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0x7f, 
        dma_write,  16,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0xff, 

/* Pass 1 Asic used to fail from 1 to 16 bytes	*/

        dma_write,  32,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0xff, 
        dma_write,  64,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0xff, 
        dma_write, 128,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0xff, 
        dma_write, 256,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0xff, 
        dma_write, 512,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0xff, 
        dma_write, 768,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0xff, 
        dma_write,1024,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0xff, 
        dma_write,1536,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0xff, 
        dma_write,2040,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0xff, 
        dma_write,2048,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0xff,0xff, 
        };

static   DMA_WRITE_TEST dma_left_write_table[] = 
        {                                                                    /* left right      */
        dma_write,  17,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x01,0x00, 
        dma_write,  18,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x03,0x00, 
        dma_write,  19,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x07,0x00, 
        dma_write,  20,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x0f,0x00, 
        dma_write,  21,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x1f,0x00, 
        dma_write,  22,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x3f,0x00, 
        dma_write,  23,packed_8_packed,(rotate_source_0<<2) | (rotate_dest_0),0x7f,0x00, 
        };

#define         color$m_bits 12

extern  int TestDmaPtr[];


/*+
* ===========================================================================
* = copy$$row_write_read_test - Row Copy Mode Write - Read Test =
* ===========================================================================
*
* OVERVIEW:
*       Row copy Mode write read test
* 
* FORM OF CALL:
*       copy$$row_write_read_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This will do an aligned copy of 1 scanline from the beginning
*       of video ram to N rows past the beginning. 
*
--*/
int     copy$$row_write_read_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int      i,s=1,num_bytes;
register int  expect,actual,shift,*vram;
register char *src,*dst;
SCOPY    copy;
VISUAL   visual;
Z        z;

 expect         = 0x55555555 & data_ptr->sfbp$l_pixel_mask; 
 num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
 shift          = 0;
 src            = (char *)data_ptr->sfbp$l_console_offset;
 dst            = (char *)(src+(data_ptr->sfbp$l_bytes_per_row*(data_ptr->sfbp$l_cursor_max_row/2)));
 vram           = (int *)dst;
 
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,expect,num_bytes);                      
 
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,dst,0,num_bytes);                      
 
 copy.src_address = (int)src;
 copy.dst_address = (int)dst;
 copy.size        = num_bytes;
 copy.copy        = simple;
 copy.boolean_op  = GXcopy;
 copy.copymask    = M1;
 copy.pixel_shift = PIXEL_BIAS;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;
 
 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

  for(i=0,s=1;s&1&&i<num_bytes/4;i++,vram++)
  {
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
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
* = copy$$src_unaligned_test - Source Unaligned Copy Test =
* ===========================================================================
*
* OVERVIEW:
*       Source Unaligned Copy Test 
* 
* FORM OF CALL:
*       copy$$src_unaligned_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This will do unaligned source address copies from video ram.
*
--*/
int     copy$$src_unaligned_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int   i,j,k,x,count,s=1,num_bytes=0;
register int   bytemask,expect,actual,shift;
register char  *src,*dst,*vram,*m;
char     buffer[20],*ptr;
SCOPY    copy;
VISUAL   visual;
Z        z;

for (i=0,m=(char *)buffer,ptr=buffer;i<sizeof(buffer);i++)*m++=0;
if ((*parse_func_ptr->pv_parse_check)(argv,argc,"b",&ptr))num_bytes=strtol(ptr,0,10);

count          = num_bytes ? 1 : sizeof(byte_sizes)/4;
src            = (char *)data_ptr->sfbp$l_console_offset;
dst            = (char *)(src+(3*data_ptr->sfbp$l_bytes_per_scanline));
expect         = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;              

(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,expect,data_ptr->sfbp$l_bytes_per_scanline);                      

for (k=0,s=1;s&1 && k<count;k++)
 {
 num_bytes      = (count==1) ? num_bytes : byte_sizes[k];
 for (x=0;s&1 && x<8;x++)
 {
 src            = (char *)data_ptr->sfbp$l_console_offset;
 dst            = (char *)(src+(3*data_ptr->sfbp$l_bytes_per_scanline));
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,dst,0,32);
 src           += x;
  
 copy.src_address = (int)src;
 copy.dst_address = (int)dst;
 copy.size        = num_bytes;
 copy.copy        = simple;
 copy.boolean_op  = GXcopy;
 copy.copymask    = 0;
 copy.pixel_shift = 0;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;
 
 (*gfx_func_ptr->pv_sfbp_unaligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 for(i=0;s&1 && i<num_bytes;i++,src++,dst++)
  {
  bytemask  = (int)dst %4;
  actual    = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,dst);
  if (expect&0xff != (actual >>(bytemask<<3)) || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)dst;              
      param[1] = expect &0xff ;               
      param[2] = (actual >>(bytemask<<3));                
      s=0;                              
      }  
  }       

 }
}


return (s);
}


/*+
* ===========================================================================
* = copy$$dst_unaligned_test - Destination Unaligned Copy Test =
* ===========================================================================
*
* OVERVIEW:
*       Source Unaligned Copy Test 
* 
* FORM OF CALL:
*       copy$$dst_unaligned_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This will do unaligned destination address copies from video ram.
*
--*/
int     copy$$dst_unaligned_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  i,j,k,x,count,s=1,num_bytes=0;
register int  bytemask,expect,actual,shift;
register char *src,*dst,*vram,*m;
char     buffer[20],*ptr;
SCOPY    copy;
VISUAL   visual;
Z        z;

for (i=0,m=(char *)buffer,ptr=buffer;i<sizeof(buffer);i++)*m++=0;
if ((*parse_func_ptr->pv_parse_check)(argv,argc,"b",&ptr))num_bytes=strtol(ptr,0,10);

src            = (char *)data_ptr->sfbp$l_console_offset;
dst            = (char *)(src+(3*data_ptr->sfbp$l_bytes_per_scanline));
expect         = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;              

(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,expect,data_ptr->sfbp$l_bytes_per_scanline);                      

count          = num_bytes ? 1 : sizeof(byte_sizes)/4;

for (k=0,s=1;s&1 && k<count;k++)
 {
 num_bytes      = (count==1) ? num_bytes : byte_sizes[k];
 for (x=0;s&1 && x<8;x++)
 {
 src            = (char *)data_ptr->sfbp$l_console_offset;
 dst            = (char *)(src+(3*data_ptr->sfbp$l_bytes_per_scanline));
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,dst,0,32);

 dst           += x;
  
 copy.src_address = (int)src;
 copy.dst_address = (int)dst;
 copy.size        = num_bytes;
 copy.copy        = simple;
 copy.boolean_op  = GXcopy;
 copy.copymask    = 0;
 copy.pixel_shift = 0;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

 (*gfx_func_ptr->pv_sfbp_unaligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 for(i=0;s&1 && i<num_bytes;i++,src++,dst++)
  {
  bytemask  = (int)dst%4;
  actual    = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,dst) ;
  expect    = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;              
  if (expect&0xff != (actual >>(bytemask<<3)) || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)dst;              
      param[1] = expect &0xff ;               
      param[2] = (actual >>(bytemask<<3));                
      s=0;                              
      }  
  }       

 }
}

return (s);
}


/*+
* ===========================================================================
* = copy$$src_dst_unaligned_test - Destination Unaligned Copy Test =
* ===========================================================================
*
* OVERVIEW:
*       Source Unaligned Copy Test 
* 
* FORM OF CALL:
*       copy$$src_dst_unaligned_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* FUNCTIONAL DESCRIPTION:
*       This will do unaligned source and destination copies of video ram.
*
--*/
int     copy$$src_dst_unaligned_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  i,j,k,x,count,s=1,num_bytes=0;
register int  bytemask,expect,actual,shift;
register char *m,*src,*dst,*vram;
char     buffer[20],*ptr;
SCOPY    copy;
VISUAL   visual;
Z        z;

for (i=0,m=(char *)buffer,ptr=buffer;i<sizeof(buffer);i++)*m++=0;
if ((*parse_func_ptr->pv_parse_check)(argv,argc,"b",&ptr))num_bytes=strtol(ptr,0,10);

src            = (char *)data_ptr->sfbp$l_console_offset;
dst            = (char *)(src+(3*data_ptr->sfbp$l_bytes_per_scanline));
expect         = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask;              
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,expect,data_ptr->sfbp$l_bytes_per_scanline);                      

count          = num_bytes ? 1 : sizeof(byte_sizes)/4;

for (k=0,s=1;s&1 && k<count;k++)
 {
 num_bytes      = (count==1) ? num_bytes : byte_sizes[k];
 for (x=0;s&1 && x<8;x++)
 {
 src            = (char *)data_ptr->sfbp$l_console_offset;
 dst            = (char *)(src+(3*data_ptr->sfbp$l_bytes_per_scanline));
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,dst,0,32);

 src           += x;
 dst           += x;
  
 copy.src_address = (int)src;
 copy.dst_address = (int)dst;
 copy.size        = num_bytes;
 copy.copy        = simple;
 copy.boolean_op  = GXcopy;
 copy.copymask    = 0;
 copy.pixel_shift = 0;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

 (*gfx_func_ptr->pv_sfbp_unaligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 for(i=0;s&1 && i<num_bytes;i++,src++,dst++)
  {
  bytemask  = (int)dst%4;
  actual    = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,dst) ;
  if (expect&0xff != (actual >>(bytemask<<3)) || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)dst;              
      param[1] = expect &0xff ;               
      param[2] = (actual >>(bytemask<<3));                
      s=0;                              
      }  
  }       


 }
}
 
return (s);
}



/*+
* ===========================================================================
* = copy$$inc_write_read_test - Incremental Copy Mode Write - Read Test =
* ===========================================================================
*
* OVERVIEW:
*       Incremental copy Mode write read test
* 
* FORM OF CALL:
*       copy$$inc_write_read_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This will do incremental copies of 0 to 32 bytes. The sfbp is capable of doing
*       copies up to 32 pixels. Well...it should be able to 0 to 32 also without
*       hanging. This test will verify that functionality. Again, I will use the 
*       first row as my source and the second row as the destination.
*
--*/
int     copy$$inc_write_read_test (int argc,char *argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int      i,j,s=1,bytemask,num_bytes,num_lws;
int  color,expect,copymask,actual,shift,*dst;
char *start,*src;
SCOPY   copy;
VISUAL   visual;
Z        z;

color          = 0xAAAAAAAA;
num_bytes      = 64;
num_lws        = num_bytes/4;
shift          = 0;

for (i=1,s=1,copymask=0;s&1&&i<=32;i++)                                            
 {
 src   = (char *)data_ptr->sfbp$l_console_offset;
 src  +=  data_ptr->sfbp$l_bytes_per_row*(data_ptr->sfbp$l_cursor_max_row/4);
 start =  (char *)(src+(data_ptr->sfbp$l_bytes_per_row*(data_ptr->sfbp$l_cursor_max_row/2)));
 dst   =  (int *)start;

 for (j=0;j<32;j+=4)
        (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,(sfbp$k_copy_reg_0+j),0,LWMASK);

 (*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,src,color,num_bytes);                      
 (*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,dst,0,num_bytes);                      
 copymask |= (1<<(i-1));

 copy.src_address = (int)src;
 copy.dst_address = (int)dst;
 copy.size        = i;
 copy.copy        = simple;
 copy.boolean_op  = GXcopy;
 copy.copymask    = copymask;
 copy.pixel_shift = 0;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 for(j=0;s&1&&j<i;j++)                                                 
   {
   actual   = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,dst);
   bytemask = j%4;
   if (expect&0xff != (actual >>(bytemask<<3)) || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)dst;              
      param[1] = expect &0xff ;               
      param[2] = (actual >>(bytemask<<3));                
      s=0;                              
      }  
   if (bytemask==3)dst++;
   }
}
return (s);
}



/*+
* ===========================================================================
* = copy$$c64_write_read_test - Row Copy Mode Write - Read Test =
* ===========================================================================
*
* OVERVIEW:
*       Row copy Mode write read test
* 
* FORM OF CALL:
*       copy$$c64_write_read_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This will do an aligned copy using the copy 64 registers, which
*       is 64 byte copies thru the copy buffer.
*
--*/
int     copy$$c64_write_read_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int      i,s=1,num_bytes;
register int  expect,actual,shift,*vram;
register char *src,*dst;
SCOPY   copy;
VISUAL   visual;
Z        z;

 /* Do at least 8 scanlines for aliasing        */
 expect         = sfbp$k_def_lw_purple;
 num_bytes      = 8*data_ptr->sfbp$l_bytes_per_scanline;
 shift          = 0;
 src            = (char *)data_ptr->sfbp$l_console_offset;
 dst            = (char *)(src+(data_ptr->sfbp$l_bytes_per_row*(data_ptr->sfbp$l_cursor_max_row/2)));
 vram           = (int *)dst;

 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,expect,num_bytes);                      
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,dst,0,num_bytes);                      
 
 copy.src_address = data_ptr->sfbp$l_planes==8 ? (int)src : (int)(src)/4;
 copy.dst_address = data_ptr->sfbp$l_planes==8 ? (int)dst : (int)(dst)/4;
 copy.size        = num_bytes;
 copy.copy        = copy64;
 copy.boolean_op  = GXcopy;
 copy.copymask    = M1;
 copy.pixel_shift = 0;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

(*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

for(i=0,s=1;s&1&&i<num_bytes/4;i++,vram++)
 {
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
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
* = copy$$dma_read_test - packed to unpacked test =
* ===========================================================================
*
* OVERVIEW:
*       Row copy Mode dma read test
* 
* FORM OF CALL:
*       copy$$dma_read_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This will do dma read copies from host memory to video ram.
*
--*/
int     copy$$dma_read_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  i,j,s=1,num_bytes;
register int  UserCount=0,overlap=32,bytemask,expect,actual,shift;
char     *src,*dst,*ptr,*vram,*m,*DmaBufferPtr;
char     buffer[20],temp[40];
SCOPY    copy;
VISUAL   visual;
Z        z;

 {
 for (i=0,num_bytes=0,m=(char *)buffer,ptr=buffer;i<sizeof(buffer);i++)*m++=0;
        if ((*parse_func_ptr->pv_parse_check)(argv,argc,"n",&ptr))
                {UserCount=TRUE;num_bytes=strtol(ptr,0,10);}
 
 if (UserCount && num_bytes>SIZE_DMA_BUFFER)num_bytes=SIZE_DMA_BUFFER;
 DmaBufferPtr  = (char *)TestDmaPtr[data_ptr->sfbp$l_slot_number];

 do
 {
 for (s=1,j=0;s&1 && j*sizeof(DMA_READ_TEST)<sizeof(dma_read_table);j++)
 {
 if (!UserCount)num_bytes = dma_read_table[j].size;
 expect    = sfbp$k_def_lw_green;
 src       = (char *)data_ptr->sfbp$l_console_offset;
 
 ptr	   = DmaBufferPtr;
 for (i=0;i<SIZE_DMA_BUFFER;i++)*ptr++= (char)expect;

 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,0,num_bytes);                      

 copy.src_address        = (int)src;
 copy.size               = num_bytes;
 copy.copy               = dma_read_table[j].mode;
 copy.boolean_op         = GXcopy;
 copy.copymask           = M1;
 copy.pixel_shift        = 0;
 
 copy.left1              = UserCount ? 0xf : dma_read_table[j].left1;
 copy.left2              = UserCount ? 0xf : dma_read_table[j].left2;
 copy.right1             = UserCount ? 0xf : dma_read_table[j].right1;
 copy.right2             = UserCount ? 0x0 : dma_read_table[j].right2;

 ptr		  	 = DmaBufferPtr;

 copy.system_address     = (int)ptr;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

 if (data_ptr->sfbp$l_supplemental)
	printf ("DMA Read %d bytes \n",num_bytes);

 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 vram = (char *)src;
 for(i=0,s=1;s&1&&i<num_bytes;i++,vram++)
  {
   bytemask = i%4;
   actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
   if (i==num_bytes)expect=0;
   if (data_ptr->sfbp$l_supplemental) printf ("0x%x = 0x%x\n",vram,  (actual >>(bytemask<<3)) );
   if (expect&0xff != (actual >>(bytemask<<3)) || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)vram;              
      param[1] = expect &0xff ;               
      param[2] = (actual >>(bytemask<<3));                
      s=0;                              
      }  
    }
   if (UserCount)break; 
   }
  } while (data_ptr->sfbp$l_loope);
 } 

 return (s);
}


/*+
* ===========================================================================
* = copy$$dma_write_test - Row Copy Mode Write - Read Test =
* ===========================================================================
*
* OVERVIEW:
*       Row copy Mode write read test
* 
* FORM OF CALL:
*       copy$$dma_write_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This will do dma write copies from video ram to host memory.
*
--*/
int     copy$$dma_write_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  i,overlap=32,j,s=1,num_bytes;
register int  UserCount=0,*dma,expect,actual,shift,*vram;
char     *src,*dst,*ptr,*m,*DmaBufferPtr;
char     buffer[20];
char     temp[40];
SCOPY    copy;
VISUAL   visual;
Z        z;

 {
 for (i=0,num_bytes=0,m=(char *)buffer,ptr=buffer;i<sizeof(buffer);i++)*m++=0;
        if ((*parse_func_ptr->pv_parse_check)(argv,argc,"n",&ptr))
                {UserCount=TRUE;num_bytes=strtol(ptr,0,10);}
                 
 if (UserCount && num_bytes>SIZE_DMA_BUFFER)num_bytes=SIZE_DMA_BUFFER;
 DmaBufferPtr  = (char *)TestDmaPtr[data_ptr->sfbp$l_slot_number];

 do
 {
 for (s=1,j=0;s&1 && j*sizeof(DMA_WRITE_TEST)<sizeof(dma_write_table);j++)
 {
 /* pass 1 asic used to fail with 1 to 16 byte dma transfers	*/
 /* If pass 2...do all transfers				*/
 /* If pass 1...do all > 16 byte transfers			*/
 expect         = sfbp$k_def_lw_yellow ;   
 if (!UserCount)num_bytes  = dma_write_table[j].size;
 src            = (char *)data_ptr->sfbp$l_console_offset;
                                           
 if ( data_ptr->sfbp$l_etch_revision > TgaPass1 ||  num_bytes > 16 )
 {
 
 ptr = DmaBufferPtr;
 for (i=0;i<SIZE_DMA_BUFFER;i++)*ptr++=0;
 
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,expect,num_bytes);                      

 copy.src_address = (int)src;
 copy.size        = num_bytes;
 copy.copy        = dma_write;
 copy.boolean_op  = GXcopy;
 copy.copymask    = M1;
 copy.pixel_shift = 0;
 copy.left1       = UserCount ? 0xff : dma_write_table[j].left1;
 copy.right1      = UserCount ? 0xff : dma_write_table[j].right1;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

 ptr 		  = DmaBufferPtr;
 copy.system_address     = (int)ptr;

 if (data_ptr->sfbp$l_supplemental)
	printf ("DMA Write %d bytes \n",num_bytes);

 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 ptr 	 = DmaBufferPtr;
 for(i=0,s=1;s&1&&i<num_bytes-8;i++,ptr++)
 {
  actual = (*ptr);
  if (data_ptr->sfbp$l_supplemental) printf ("0x%x = 0x%x\n",ptr,actual);
  if ((actual&0xff) != (expect&0xff) || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = (int)ptr;
      param[1] = expect & 0xff ;               
      param[2] = actual & 0xff ;                
      s= data_ptr->sfbp$l_conte ? 1 : 0;
      }
   }
  if (UserCount)break;
  }
  }
 } while (data_ptr->sfbp$l_loope);
}

return (s);

}



/*+
* ===========================================================================
* = copy$$dma_left_write_test - Row Copy Mode Write - Read Test =
* ===========================================================================
*
* OVERVIEW:
*       Row copy Mode write read test
* 
* FORM OF CALL:
*       copy$$dma_left_write_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This will do dma write copy from video vram to host memory.
*       This test is specifically for the left edge mask.
*
--*/
int     copy$$dma_left_write_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  i,overlap=32,j,s=1,num_bytes;
register int  CheckBytes,*dma,expect,actual,shift,*vram;
register char *src,*dst,*ptr,*DmaBufferPtr;
char     temp[40];
SCOPY    copy;
VISUAL   visual;
Z        z;

 {
 DmaBufferPtr  = (char *)TestDmaPtr[data_ptr->sfbp$l_slot_number];

/* TGA pass 2 asic can hang on edge masking.  Skip this test. */
 if ( data_ptr->sfbp$l_etch_revision == TgaPass2 ||
      data_ptr->sfbp$l_etch_revision == TgaPass2Plus ) return(1);
                                        
 do
 {
 for (s=1,j=0;s&1 && j*sizeof(DMA_WRITE_TEST)<sizeof(dma_left_write_table);j++)
 {            
 expect         = sfbp$k_def_lw_yellow;
 num_bytes      = dma_left_write_table[j].size;
 src            = (char *)data_ptr->sfbp$l_console_offset;
 
 ptr 		= DmaBufferPtr;
 copy.src_address = (int)src;
 copy.size        = num_bytes;
 copy.copy        = dma_write;
 copy.boolean_op  = GXcopy;
 copy.copymask    = M1;
 copy.pixel_shift = 0;
 copy.left1       = dma_left_write_table[j].left1;
 copy.right1      = dma_left_write_table[j].right1;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

 ptr 		  = DmaBufferPtr;
 copy.system_address     = (int)ptr;

 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,expect,num_bytes);                      

 if (data_ptr->sfbp$l_supplemental)
	printf ("DMA Write %d bytes \n",num_bytes);

 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 /* I am only looking at left edge characteristics      */
 /* Which is the second longword                        */
 /* We must read enough so that left mask is used       */
 /* and not the right mask, which is why I read like    */
 /* 17 bytes and the such                               */
 
 expect &=0xff;
 CheckBytes = (dma_left_write_table[j].size % 8);
 ptr    = DmaBufferPtr;
 for(i=0,s=1;s&1&&i<CheckBytes;i++,ptr++)
 {
  actual = (*ptr)&0xff;
  if (data_ptr->sfbp$l_supplemental) printf ("0x%x = 0x%x\n",ptr,actual);
  if (data_ptr->sfbp$l_print)
  if (actual != expect || data_ptr->sfbp$l_test_error )                 
      { 
      param[0] = (int)ptr;
      param[1] = expect ;               
      param[2] = actual;                
      s=0;
      }
    }
   }
  } while (data_ptr->sfbp$l_loope);
 }

 return (s);
}



/*+
* ===========================================================================
* = copy$$dma_read_dither_test - packed to unpacked test =
* ===========================================================================
*
* OVERVIEW:
*       DMA Read Dither Test
* 
* FORM OF CALL:
*       copy$$dma_read_dither_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This test will do a dma read from host memory and then
*       verify that the hardware will dither the image. This
*       will only be run on 8 plane systems. Since the dither 
*       verification code is already in the line verify routine,
*       I will just call the line verify routine as if I was 
*       drawing a horizontal transparent line from location 0. The
*       dma read dither only works on 24 bit visual format, meaning
*       full longwords of data in rgb format with the upper byte
*       thrown away.  So, I fill up virtual memory with data such
*       XXRRGGBB and then increment the visual. The other trick is
*       that dither always operates on a 32x32 matrix and is expecting
*       the column and row bits in the upper byte.
*
--*/
int     copy$$dma_read_dither_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int i,s=1,red,green,blue,num_bytes,*DmaBufferPtr,*ptr;
register char *src;
SCOPY    copy;
VISUAL   visual;
Z        z;
COLOR    color;
LINE     line;

 {
 DmaBufferPtr  = (char *)TestDmaPtr[data_ptr->sfbp$l_slot_number];
 ptr	       = DmaBufferPtr;
 
 do
 {
 if (data_ptr->sfbp$l_planes == 8)
 {
 line.verify      = TRUE;
 line.verify_z    = FALSE;
 line.boolean_op  = GXcopy;
 line.span_line   = FALSE;
 line.polyline    = FALSE;
 line.wait_buffer = FALSE;
 line.color_change= TRUE;
 line.line        = cint_d_line_mode;
 line.nogo        = go;
 line.speed       = slow;
 line.draw_method = line_address;
 line.window_type = Xwindow;
 line.xa          = 0;
 line.ya          = 0;
 line.ya          = 0;
 line.xb          = data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels/4;
 
 visual.capend    = FALSE;
 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.boolean   = line.boolean_op;
 visual.doz       = FALSE;
 
 color.fg_value          = sfbp$k_def_lw_red   & data_ptr->sfbp$l_pixel_mask ;
 color.bg_value          = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;
 color.method            = foreground;
 color.red_increment     = 1<<color$m_bits;
 color.green_increment   = 1<<color$m_bits;
 color.blue_increment    = 1<<color$m_bits;
 
 /* Set up for a 1 scanline DMA        */
 num_bytes = data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;
 src              = (char *)data_ptr->sfbp$l_console_offset;

 /* Initialize DMA Buffer              */
 red = green = blue = 0;
 for (i=0;i<num_bytes/4;i++,red++,green++,blue++,ptr++)
        *ptr = ((red<<16)&0xff0000) | ((green << 8)&0xff00) | ((blue<<0)&0xff) ;

 /* Clear Destination Buffer           */ 
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,0,num_bytes);                      

 copy.src_address        = (int)src;
 copy.size               = num_bytes;
 copy.copy               = dma_read_dithered;
 copy.boolean_op         = GXcopy;
 copy.copymask           = M1;
 copy.pixel_shift        = 0;
 copy.left1              = 0xf;
 copy.left2              = 0xf;
 copy.right1             = 0xf;
 copy.right2             = 0xf;
 
 ptr                     = (int *)DmaBufferPtr;
 copy.system_address     = (int)ptr;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

 /* Do the DMA read from host memory       */
 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 /* Line Verify Code Has the Dither Matrix */
 data_ptr->sfbp$l_dma_read_dither =  TRUE;

 /* I need to tell my verify routine       */
 /* An Initial value and it increments it  */

 data_ptr->sfbp$l_dma_read_initial = 0;

 /* We faked it out to think it was a      */
 /* horizontal line                        */

 s=(*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);
  
  data_ptr->sfbp$l_dma_read_dither=FALSE;
  }
 } while (data_ptr->sfbp$l_loope);
}

return (s);
}

