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



/*+
* ===========================================================================
* = plane$$write_read_test - Pixel Plane Mode Write - Read Test =
* ===========================================================================
*
* OVERVIEW:
*       Plane Mask Register write read test
* 
* FORM OF CALL:
*       plane$$write_read_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Test each bit of plane mask register. Write to the location in vram
*       with plane on and then disable the plane and try to write to it.
*

	The Plane Mask register specifies which bits within a pixel will
	be affected by a write operation to the Frame Buffer. If the
	bit in the Plane Mask register is set, the corresponding bit in
	the pixel will be modified by writes to the Frame Buffer.  If the
	bit in the Plane Mask register is not set, the corresponding bit
	within the pixel will be unchanged. In 8 plane mode, the mask
	must be replicated across all 32 bits of the Plane Mask register.

	Since this register is implemented inside the VRAM's, its state
	is UNDEFINED at initialization.

	31                                                             0
	+--------------------------------------------------------------+
	|                         Plane Mask                           |
	+--------------------------------------------------------------+

	Value at init: UNDEFINED

*
--*/
int     plane$$write_read_test (int argc,char *argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register volatile int      i,j,s=1;
volatile unsigned int       actual,*src,*dst,initdata;
static   struct plane_8_type {
        unsigned int  plane;
        unsigned int  expect;
        char         *text;
        } plane_8_table [] =
        {
        0x00000000, 0xffffffff,   "No Change ",
        0x01010101, 0xfefefefe,   "Bit 0 on ",
        0x02020202, 0xfdfdfdfd,   "Bit 1 on ", 
        0x04040404, 0xfbfbfbfb,   "Bit 2 on ",
        0x08080808, 0xf7f7f7f7,   "Bit 3 on ",
        0x10101010, 0xefefefef,   "Bit 4 on ",
        0x20202020, 0xdfdfdfdf,   "Bit 5 on ",
        0x40404040, 0xbfbfbfbf,   "Bit 6 on ",
        0x80808080, 0x7f7f7f7f,   "Bit 7 on ",
        0xffffffff, 0x00000000,   "All   on ",
        };

static   struct plane_32_type {
        unsigned int  plane;
        unsigned int  expect;
        char         *text;
        } plane_32_table [] =
        {
        0x00000000, 0xffffffff,   "No Change ",
        0x00000001, 0xfffffffe,   "Bit 0  on ",
        0x00000002, 0xfffffffd,   "Bit 1  on ", 
        0x00000004, 0xfffffffb,   "Bit 2  on ",
        0x00000008, 0xfffffff7,   "Bit 3  on ",
        0x00000010, 0xffffffef,   "Bit 4  on ",
        0x00000020, 0xffffffdf,   "Bit 5  on ",
        0x00000040, 0xffffffbf,   "Bit 6  on ",
        0x00000080, 0xffffff7f,   "Bit 7  on ",
        0x00000100, 0xfffffeff,   "Bit 8  on ",
        0x00000200, 0xfffffdff,   "Bit 9  on ",
        0x00000400, 0xfffffbff,   "Bit 10 on ",
        0x00000800, 0xfffff7ff,   "Bit 11 on ",
        0x00001000, 0xffffefff,   "Bit 12 on ",
        0x00002000, 0xffffdfff,   "Bit 13 on ",
        0x00004000, 0xffffbfff,   "Bit 14 on ",
        0x00008000, 0xffff7fff,   "Bit 15 on ",
        0x00010000, 0xfffeffff,   "Bit 16 on ",
        0x00020000, 0xfffdffff,   "Bit 17 on ",
        0x00040000, 0xfffbffff,   "Bit 18 on ",
        0x00080000, 0xfff7ffff,   "Bit 19 on ",
        0x00100000, 0xffefffff,   "Bit 20 on ",
        0x00200000, 0xffdfffff,   "Bit 21 on ",
        0x00400000, 0xffbfffff,   "Bit 22 on ",
        0x00800000, 0xff7fffff,   "Bit 23 on ",
        0x01000000, 0xfeffffff,   "Bit 24 on ",
        0x02000000, 0xfdffffff,   "Bit 25 on ",
        0x04000000, 0xfbffffff,   "Bit 26 on ",
        0x08000000, 0xf7ffffff,   "Bit 27 on ",
        0x10000000, 0xefffffff,   "Bit 28 on ",
        0x20000000, 0xdfffffff,   "Bit 29 on ",
        0x40000000, 0xbfffffff,   "Bit 30 on ",
        0x80000000, 0x7fffffff,   "Bit 31 on ",
        0xffffffff, 0x00000000,   "All   on ",
        };


src             = (unsigned int *)data_ptr->sfbp$l_console_offset;
initdata        = 0xffffffff;

/* Fill vram with all planes on first   */
(*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,src,initdata,32*8);

if (data_ptr->sfbp$l_module == hx_8_plane )
 {
 for(i=0,s=1;s&1&&i*sizeof(struct plane_8_type)<sizeof(plane_8_table);i++,src++)
 {
  /* And then Enable 1 plane at a time  */
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,
                 plane_8_table[i].plane,LWMASK);

  /* write that plane and check that only that one is on */
  (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,0,LWMASK);
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,src);
  if (data_ptr->sfbp$l_print){printf ("planemask 0x%x %s actual 0x%x\n",
                plane_8_table[i].plane,plane_8_table[i].text,actual);}
  if (actual != plane_8_table[i].expect || data_ptr->sfbp$l_test_error )   
   { 
   param[0] = data_ptr->sfbp$l_vram_base | (int)src;                                                  
   param[1] = (int)plane_8_table[i].expect ;                                                      
   param[2] = actual;                    
   printf ("planemask 0x%x expect 0x%x actual 0x%x\n",
        plane_8_table[i].plane,param[1],param[2]);
   s=data_ptr->sfbp$l_conte ? 1 : 0;                                  
   }  
  }
 }
 else 
 {
  for(i=0,s=1;s&1&&i*sizeof(struct plane_32_type)<sizeof(plane_32_table);i++,src++)
  {
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,
                 plane_32_table[i].plane,LWMASK);
   (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,0,LWMASK);
   actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,src);
   if (data_ptr->sfbp$l_print){printf ("planemask 0x%x %s actual 0x%x\n",
                plane_32_table[i].plane,plane_32_table[i].text,actual);}
   if (actual != plane_32_table[i].expect || data_ptr->sfbp$l_test_error)   
    { 
    param[0] = data_ptr->sfbp$l_vram_base | (int)src;                                                  
    param[1] = (int)plane_32_table[i].expect ;                                                      
    param[2] = actual;                    
    s=data_ptr->sfbp$l_conte ? 1 : 0;                                  
    }  
   }
 }
 
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_mask,M1,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);

 return (s);
}


/*+
* ===========================================================================
* = plane$$pixel_mask_test - Pixel Mask Register Test  =
* ===========================================================================
*
* OVERVIEW:
*       Test the One Shot Pixel Mask Register
* 
* FORM OF CALL:
*       plane$$pixel_mask_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This test will test the one shot pixel mask register. This register
*       is used to mask pixels when written to the frame buffer. I use this
*       register in the opaque stipple operations bus since a persistent
*       pixel mask register (Not one shot) I added a test. The state of the
*       pixel mask register is returned in bit 23 of the mode register. We 
*       check that the state changes after the first frame buffer bit.
*
	The Pixel Mask register is used to mask pixels in Opaque stipple
	and fill modes.  Only pixels whose corresponding bit is set in
	the Pixel Mask register will be modified.  In Opaque Stipple mode the
	Pixel Mask register must be used to write fewer than 32 pixels. In
	Opaque fill mode, the PixelMask register can be used as a pattern
        mask, but must be 32 pixel aligned.

	The Pixel Mask register can also be used to mask pixels in Simple
	and Simple Z modes.  In this case only the bottom 4 bits are used,
	and mask the corresponding bytes off of the next write to the frame
	buffer.  A '1' bit enables writing of the corresponding byte and
	a '0' bit disables writing of the corresponding byte.

        The Pixel Mask register	is not explicitly used other modes, but
	writing to it while in other modes may yield unexpected results.

	The Pixel Mask register can be modified by writing either the
	One-shot or Persistent Pixel Mask registers. When set using the
	One-shot register, the Pixel Mask register is reinitialized to
	all 1's after each frame buffer operation.  When set using the
	Persistent register, the Pixel Mask register retains its value
	until next written.

	31                                                             0
	+--------------------------------------------------------------+
	|                         Pixel Mask                           |
	+--------------------------------------------------------------+

	The state of the pixelMask (hard or OneShot) can be read from the
	Mode register.

	Value at init: ffffffff
*
--*/
int     plane$$pixel_mask_test (int argc,char *argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int     mode1,mode2,mask,data=M1,actual,expect=M1,i,j,k,s=1;
union   control_reg *MODE;

/* Initialize Output	*/

(*cp_func_ptr->pv_init_output)(scia_ptr,data_ptr);

/* Initialize the Planemask for all bits        */
(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);

/* We have already tested the pixel mask OPQ ST */
/* Lets just verify it returns to -1            */

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_mask,0xf,LW);
(*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

/* Read the state of Pixel Mask Register        */
 mode1 = ((*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode)>>23);

/* A frame buffer write should return it to -1  */
(*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,0,data,LW);
(*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 
/* Read the state of Pixel Mask Register        */
/* Should be zero                               */
mode2  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);
MODE   = (union control_reg *)&mode2;

actual = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_pixel_mask);
if (actual != expect || data_ptr->sfbp$l_test_error)
  {
  param[0] = sfbp$k_a_sfbp_reg_space | sfbp$k_pixel_mask;
  param[1] = expect;
  param[2] = actual;                    
  s=0;
  }

else if (MODE->sfbp$r_control_bits.sfbp$v_con_persistent_flag==1)
  {
  param[0] = sfbp$k_a_sfbp_reg_space | sfbp$k_pixel_mask;
  param[1] = 0;
  param[2] = 0;
  s=0;
  }


return (s);
  
}


/*+
* ===========================================================================
* = plane$$pers_pixel_mask_test - Persistent Pixel Mask Register Test  =
* ===========================================================================
*
* OVERVIEW:
*       Test the One Shot Pixel Mask Register
* 
* FORM OF CALL:
*       plane$$pers_pixel_mask_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This test will test the one shot pixel mask register. This register
*       is used to mask pixels when written to the frame buffer. I use this
*       register in the opaque stipple operations bus since a persistent
*       pixel mask register (Not one shot) I added a test.
*
--*/
int     plane$$pers_pixel_mask_test (int argc,char *argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int     mode2,actual,mask,data=M1,expect=0x12345678,i,j,k,s=1;
union   control_reg *MODE;

/* Initialize the Planemask for all bits        */
(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);

/* We have already tested the pixel mask OPQ ST */
/* Lets just verify it returns to -1            */

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_persistent_pmask,expect,LW);

/* A frame buffer write should keep mask same   */
(*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,0,data,LW);
 
actual = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_pixel_mask);
mode2  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);
MODE   = (union control_reg *)&mode2;

if (actual != expect || data_ptr->sfbp$l_test_error)
  {
  param[0] = sfbp$k_a_sfbp_reg_space | sfbp$k_pixel_mask ;
  param[1] = expect;
  param[2] = actual;                    
  s=0;
  }

else if (MODE->sfbp$r_control_bits.sfbp$v_con_persistent_flag==0)
  {
  param[0] = sfbp$k_a_sfbp_reg_space | sfbp$k_pixel_mask;
  param[1] = ~MODE->sfbp$r_control_bits.sfbp$v_con_persistent_flag;
  param[2] = 0;
  s=0;
  }

/* 24 plane I had to do this...when I figure out..I will */
/* remove						 */

if (data_ptr->sfbp$l_module != hx_8_plane)
	(*cp_func_ptr->pv_init_output)(scia_ptr,data_ptr);


return (s);
  
}

