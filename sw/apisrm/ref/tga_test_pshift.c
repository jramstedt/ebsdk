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
#include "cp$src:tga_sfbp_test_pshift.h"




/*+
* ===========================================================================
* = pshift$$pos_shift_test - Pixel Shift Mode Write - Read Test =
* ===========================================================================
*
* OVERVIEW:
*       Pixel Shift Mode write read test
* 
* FORM OF CALL:
*       pshift$$pos_shift_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This is similar to the copy mode test. It will copy from the 1 quadword to the next quadword 
*       of video ram, except this time it will use the pixel shift register for data as it is copied from
*       video ram. 
*
	The Pixel Shift Register specifies the number of bytes to shift
	pixel data before the data is written into the copy buffer.  The
	value is interpreted as a four bit signed value allowing either
	positive or negative shifts.  The Pixel Shift value only effects
	data in copy and dma modes; it is ignored in all other modes.

	As a side effect of writing to the Pixel Shift Register, the
	Copy mode state is reset.  The next write to the Frame buffer
	will be interpreted as the starting source address.

	31                                                     4 3     0
	+-------------------------------------------------------+------+
	|                         ignore                        | Shift|
	+-------------------------------------------------------+------+

	Value at init: 0
*
--*/
int     pshift$$pos_shift_test (int argc,char *argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int      i,j,k,s=1;
int      *cpy,expect,actual,copymask,copydata,num_bytes,*dst;
char      *svram,*src,*start;
SCOPY     copy;
VISUAL    visual;
Z         z;

svram          = (char *)data_ptr->sfbp$l_console_offset;
copymask       = M1;
num_bytes      = 32;

for(i=0,s=1;s&1&&i*sizeof(struct pshift_type)<sizeof(pshift_table);i++)
 {
 src   =  svram ;                                                                 
 start = (char *)src + num_bytes;
 dst   = (int *)start; 
 cpy   = (int *)src;

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LWMASK);
 (*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,dst,0,num_bytes);                              /* Clear destination     */
 for (j=0;j<32;j+=4)
        (*gfx_func_ptr->pv_sfbp_write_reg) (scia_ptr,(sfbp$k_copy_reg_0+j),0,LW);

 for (j=0;j<8;j++,cpy++)
         (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,cpy,copybuffer[j],LWMASK);

 copy.src_address = (int)src;
 copy.dst_address = (int)dst;
 copy.size        = num_bytes;
 copy.copy        = simple;
 copy.copymask    = copymask;
 copy.boolean_op  = GXcopy;
 copy.pixel_shift = pshift_table[i].pshift;

 visual.depth     = packed_8_packed;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;
 
(*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 for (j=0;s&1&&j<num_bytes/4;j++,dst++)                                                         
  {
  actual= (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,dst);
  if (actual != pshift_table[i].expect[j] || data_ptr->sfbp$l_test_error )         
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)dst;
      param[1] = (int)pshift_table[i].expect[j];                                 
      param[2] = actual;                                                       
      s=0;                                                                     
      }  
   }
 }                                                                             

 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

return (s);
}



/*+
* ===========================================================================
* = pshift$$neg_shift_test - Pixel Shift Mode Write - Read Test =
* ===========================================================================
*
* OVERVIEW:
*       Pixel Shift Mode write read test
* 
* FORM OF CALL:
*       pshift$$neg_shift_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* RETURNS:
*       1
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
*       Negative address pixel shift test.
*
--*/
int     pshift$$neg_shift_test(argc,argv,ccv,scia_ptr, data_ptr,param)

int     argc;
char    **argv;
CCV     *ccv;
struct  scia *scia_ptr;
struct  sfbp_data *data_ptr;
int     param[];
{
register int      i,j,k,s=1;
int      *cpy,expect,actual,copymask,copydata,num_bytes,*dst;
char     *svram,*src,*start,*copy_src,*copy_dst;
SCOPY     copy;
VISUAL    visual;
Z         z;

svram          = (char *)data_ptr->sfbp$l_console_offset;
copymask       = 0xffffffff;
num_bytes      = 32;

for(i=0,s=1;s&1&&i*sizeof(struct neg_pshift_type)<sizeof(neg_pshift_table);i++)
 {
 src   = svram ;                                                                 
 start = (char *)src + num_bytes+num_bytes;
 dst   = (int *)start;
 copy_src = src+num_bytes;
 copy_dst = (char *)dst+num_bytes;

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LWMASK);

 cpy      = (int *)src;

 for (j=0;j<32;j+=4)
        (*gfx_func_ptr->pv_sfbp_write_reg) (scia_ptr,(sfbp$k_copy_reg_0+j),0,LW);
 
 for (j=0;j<8;j++,cpy++)
         (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,cpy,copybuffer[j],LWMASK);

 (*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,dst,0,num_bytes);                              /* Clear destination     */

 copy.src_address = (int)copy_src;
 copy.dst_address = (int)copy_dst;
 copy.size        = num_bytes;
 copy.copy        = simple;
 copy.copymask    = copymask;
 copy.boolean_op  = 3;
 copy.pixel_shift = neg_pshift_table[i].pshift;

 visual.depth     = packed_8_packed;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

(*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 for (j=0;s&1&&j<num_bytes/4;j++,dst++)                                                         /* Check Each Long      */
  {
  actual =  (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,dst);
  if (actual != neg_pshift_table[i].expect[j] || data_ptr->sfbp$l_test_error )         
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)dst;
      param[1] = (int)neg_pshift_table[i].expect[j];                                 
      param[2] = actual;                                                       
      s=0;                                                                     
      }  
   }
 }                                                                             

 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

return (s);
}




/*+
* ===========================================================================
* = pshift$$neg_push_test - Pixel Shift Mode Write - Read Test =
* ===========================================================================
*
* OVERVIEW:
*       Pixel Shift Mode write read test
* 
* FORM OF CALL:
*       pshift$$neg_push_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* RETURNS:
*       1
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
*       Negative address pixel shift test.
*
--*/
int     pshift$$neg_push_test(argc,argv,ccv,scia_ptr, data_ptr,param)

int     argc;
char    **argv;
CCV     *ccv;
struct  scia *scia_ptr;
struct  sfbp_data *data_ptr;
int     param[];
{
register int      i,j,k,s=1;
int      expect,actual,copymask,copydata,num_bytes;
char     *src,*dst;
int      pattern,residue;
SCOPY    copy;
VISUAL   visual;
Z        z;

#ifdef DEBUG
 num_bytes      = 256;
 dst            = (char *)data_ptr->sfbp$l_console_offset;
 src            = (char *)(int)(4*data_ptr->sfbp$l_console_offset);

 for (j=0;j<32;j+=4)
         (*gfx_func_ptr->pv_sfbp_write_reg) (scia_ptr,(sfbp$k_copy_reg_0+j),0,LW);

 /* Initial Value to Mark the location  */
 (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,0x88888888,LW);

 /* Fill Pattern in Source Area */
 for (pattern=0x03020100,j=0;j<num_bytes/4;j++,src+=4,pattern+=0x01010101)
         (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src+4,pattern,LW);


 /* Clear the Destination Copy  */
 (*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,dst,0,num_bytes);                              
 
 src  = (char *)(int)(4*data_ptr->sfbp$l_console_offset);
 
 copy.copy        = simple;
 copy.copymask    = 0xffffffff;
 copy.boolean_op  = GXcopy;
 copy.pixel_shift = 0xc;

 visual.depth     = packed_8_packed;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

 copy.src_address = (int)src;
 copy.dst_address = (int)dst;
 copy.size        = num_bytes;

 printf ("src 0x%x dst 0x%x \n",copy.src_address,copy.dst_address);
 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 residue = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,copy.src_address+4);
 (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,copy.dst_address,residue,LW);
  
 for (pattern=0x03020100,j=0;s&1&&j<num_bytes/4;j++,dst+=4,pattern+=0x01010101)
  {
  actual =  (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,dst);
  if (actual != pattern || data_ptr->sfbp$l_test_error )         
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)dst;
      param[1] = pattern;
      param[2] = actual;                                                       
      s=0;                                                                      
      }  
   }
   
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

#endif
return (s);
}

