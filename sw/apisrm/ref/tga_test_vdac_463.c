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
 *      BT463 RAMDAC test .
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



/*+
* ===========================================================================
* = vdac_463$$fill_vram - Fill video ram =
* ===========================================================================
*
* OVERVIEW:
*       Fill video ram
* 
* FORM OF CALL:
*       vdac_463$$fill_vram (argc,argv,vector,scia_ptr,data_ptr,param )
*
* RETURNS:
*
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       vector      - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*       Fill video ram. This will fill video ram with a incrementing
*       pattern from 00 to 256 to represent the 256 possible 
*       colors from the RAMDAC. Each longword of video ram is filled 
*       with the next incrementing pattern. So the first 256 longwords
*       is filled with the address modulo 256. This is repeated for the
*       amount of memory within the system. This is used to populate the
*       video rams for the signature tests.
*
--*/
int     vdac_463$$fill_vram (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int      s=1;
register int      *vid,num_bytes;

vid            = (int *)data_ptr->sfbp$l_console_offset;
num_bytes      = data_ptr->sfbp$l_vram_size; 
num_bytes     -= data_ptr->sfbp$l_console_offset;

if (data_ptr->sfbp$l_print) pprintf ("num_bytes =  0x%x \n",num_bytes);
                 
s=(*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,vid,0x55555555,num_bytes);

return (s);
}



/*+
* ===========================================================================
* = vdac_463$$cmd_reg_test - VDAC command Register Test =
* ===========================================================================
*
* OVERVIEW:
*       Load the VDAC command registers and verify them.
* 
* FORM OF CALL:
*       vdac_463$$cmd_reg_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* RETURNS:
*       status
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* GLOBALS:
*       none       
*
* FUNCTIONAL DESCRIPTION:
*       This test loads the BT463 command registers and reads them back to
*       check for integrity. The BT463 automatically increments its address
*       register after each write cycle. So we load the address register 
*       with the initial value and then read a table of values to load the
*       registers. We then reload the address register to read the values.
*               I do not test the test register or the signature register
*       because these values change during active and non active video.
*
--*/
int     vdac_463$$cmd_reg_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int     i,s=1;
int      temp1,temp2;
unsigned char temp;

static struct  vdac_reg_type {
        unsigned char    value;
	int	 index;
        } vdac_cmd_regs[] = {
	0x40,           1,	/* 4:1 mux                  */
	0x08,           2,	/* 2 plane cursor           */
	0xc0,           3,	/* sync, 7.5 ire,           */
	0xff,           5,	/* pixel read mask p7:p0    */
	0xff,           6,	/* pixel read mask p15:p8   */
	0xff,           7,	/* pixel read mask p23:p16  */
	0x0f,           8,	/* pixel read mask p27:p24  */
	0x00,           9,	/* pixel blink mask p7:p0   */
	0x00,           10,	/* pixel blink mask p15:p8  */
	0x00,           11,	/* pixel blink mask p23:p16 */
	0x00,           12,	/* pixel blink mask p27:p24 */
        };

for (i=0,s=1;s&1&&i*sizeof(struct vdac_reg_type)<sizeof(vdac_cmd_regs);i++)
  {
  (*bt_func_ptr->pv_bt_write_reg)(scia_ptr,bt463$k_id_reg+vdac_cmd_regs[i].index,vdac_cmd_regs[i].value);
  }
   
for (i=0,s=1;s&1&&i*sizeof(struct vdac_reg_type)<sizeof(vdac_cmd_regs);i++)
 {
  temp = (*bt_func_ptr->pv_bt_read_reg)(scia_ptr,bt463$k_id_reg+vdac_cmd_regs[i].index);
  if ( temp != vdac_cmd_regs[i].value || data_ptr->sfbp$l_test_error )
    {
     temp1 = (*bt_func_ptr->pv_bt_read_lo)(scia_ptr);
     temp2 = (*bt_func_ptr->pv_bt_read_hi)(scia_ptr);
     param[0] = (temp1|(temp2<<8))-1;
     param[1] = vdac_cmd_regs[i].value&0xff;                        
     param[2] = temp&0xff;
     s=0;
     break;                                             
     }
 }   
return (s);
}

/*+
* ===========================================================================
* = vdac_463$$col_ram_test - VDAC Color RAM Test =
* ===========================================================================
*
* OVERVIEW:
*       Load the VDAC color ram and verify it.
* 
* FORM OF CALL:
*       vdac_463$$col_ram_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       The BT463 has a 528x8 bit color ram. This routine 
*       will test the color ram with incrementing and decrementing
*       data patterns. 
*
--*/
int     vdac_463$$col_ram_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int     i,j,k,s=1;
int      temp1,temp2;
unsigned char temp[3];

for (i=0,s=1;s&1&&i<256;i++)                                    
 {
   temp[0]=temp[1]=temp[2]= i&0xff;
   (*bt_func_ptr->pv_bt_write_color)(scia_ptr,i,temp);
   (*bt_func_ptr->pv_bt_read_color)(scia_ptr,i,temp);
   for (j=0;s&1&&j<3;j++)
	   if ( (temp[j]&0xff) != (i&0xff) )
	     {
	     param[0] = i;
	     param[1] = i;
	     param[2] = temp[j]&0xff;
	     s=0;                                                
	     break;                                              
	     }                                                   
   }

return (s);
}

/*+
* ===========================================================================
* = vdac_463$$cur_ram_test - VDAC Cursor RAM Test =
* ===========================================================================
*
* OVERVIEW:
*       Load the VDAC cursor ram and verify it.
* 
* FORM OF CALL:
*       vdac_463$$cur_ram_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This duplicates the vram test and may not be needed.
*
--*/
int     vdac_463$$cur_ram_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int base,rows=0,value,xpos,ypos,offset;
register int temp,pattern,i,j,k,s=1;
static char   colors[4][3]=
        {       
        0x00,0x00,0x00,                                                         /* Black        */
        0xff,0x00,0x00,                                                         /* Red 	        */
        0x00,0x00,0xff,                                                         /* Blue 	*/
        0xff,0xff,0xff,                                                         /* White        */
        };
 
 (*bt_func_ptr->pv_bt_write_cur_color)(scia_ptr,bt463$k_cur_color_0,colors[1]); 
 (*bt_func_ptr->pv_bt_write_cur_color)(scia_ptr,bt463$k_cur_color_1,colors[2]); 

 (bt_func_ptr->pv_bt_pos_cursor )(scia_ptr,data_ptr,
       data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2,
       data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels/2,TRUE);

 rows   = 63;
 base   = rows<<10;
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_cursor_base,base,LW);

 for (s=1,j=0;s&1 && j<2;j++)
 {
 pattern  = j==0 ? 0x55555555 : 0xaaaaaaaa;

 for (i=0;i<1024;i+=4)
   (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,i,pattern,LW);   
 
 for (i=0;s&1 && i<1024;i+=4)
 {
  temp = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,i);
  if ( temp != pattern )
    {
     param[0] = i;
     param[1] = pattern;
     param[2] = temp;
     s=0;
     break;                                             
     }
  }
 }
  
 (*bt_func_ptr->pv_bt_write_cur_color)(scia_ptr,bt463$k_cur_color_0,colors[3]); 
 (*bt_func_ptr->pv_bt_write_cur_color)(scia_ptr,bt463$k_cur_color_1,colors[3]); 

 rows   = scia_ptr->font_height-1;        
 base   = (rows<<10)|(offset<<4);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_cursor_base,base,LW);

return (s);
}



/*+
* ===========================================================================
* = vdac_463$$cur_col_test - VDAC Cursor Color Register Test =
* ===========================================================================
*
* OVERVIEW:
*       Load the VDAC cursor color registers and verify them.
* 
* FORM OF CALL:
*       vdac_463$$cur_col_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       The BT463 has 3 registers for the color of the cursor. The pixel 
*       value in cursor ram selects which cursor register to use. The value
*       0x00 is undefined, 0x01 is cursor color register 1, 0x02 is color
*       register 2 and 0x11 is color 3. So we need simply to load 0x55 for
*       color 1, 0xAA for color 2 and 0xFF for color 3. And then we will
*       load the cursor color map with red,green and blue so that the
*       color of the cursor should be red,green and blue. To make the
*       cursor visible, we need to set up the cursor positioning registers
*       which is available from the driver data area.  
*               I check the cursor ram again after loading the colors.
*
--*/
int     vdac_463$$cur_col_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int     i,j,k,s=1,pix;
int      offset,temp1,temp2,temp3;
static   int      pixel[4]= {0xff,0x55,0xAA,0xff};                                      /* 00,01,10,11   */
unsigned char temp [2][3];
static   unsigned char color[2][3] =
        {
        0xff,0x00,0x00,                                                                 /* red     01    */
        0x00,0xff,0xff,                                                                 /* green-blue 10 */
        };

for (i=0,s=1;s&1&&i<2;i++)                                                              
  {
   (*bt_func_ptr->pv_bt_write_cur_color)(scia_ptr,bt463$k_cur_color_0+i,color[i]);
   (*bt_func_ptr->pv_bt_read_cur_color) (scia_ptr,bt463$k_cur_color_0+i,temp[i]);
   for (j=0,s=1;s&1&&j<3;j++)
      if (temp[i][j] != color[i][j] )                     
       {
       temp1 = (*bt_func_ptr->pv_bt_read_lo)(scia_ptr);
       temp2 = (*bt_func_ptr->pv_bt_read_hi)(scia_ptr);
       param[0] = (temp1|temp2<<8)-1;
       param[1] = color[i][j]&0xff;
       param[2] = temp[i][j]&0xff; 
       s=0;                
       break;                                              
       }                                                   
    }

return (s);
}




/*+
* ===========================================================================
* = vdac_463$$cur_sig_test - VDAC Cursor Signature Register Test =
* ===========================================================================
*
* OVERVIEW:
*       Cursor Signature Test
* 
* FORM OF CALL:
*       vdac_463$$cur_sig_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* RETURNS:
*       1 for success else 0 for failure
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* GLOBALS:
*       
*
* FUNCTIONAL DESCRIPTION:
*       This test is already done when the pattern test is executed. The
*       goal is to move the cursor across the screen to present valid 
*       data to be compared by the analog comparators.
*               The signature registers are updated during the active
*       video time. The registers can be read during the blank interval
*       up to 15 clock pixels after blank is negated. This test is 
*       called last because cursor information is already on the screen.
*
--*/
int     vdac_463$$cur_sig_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int s=1;
static struct signature_type sfbp$r_463_test = 
 {
 0,
  0x4b,0x4b,0xf2,0xf2,                            
  0x8d,0x8d,0x50,0x50,
  0x1e,0x1e,0x31,0x31,
 };

 /* restore the default mode    */
 /* And wait for chip ]idle      */
 /* Before getting signatures   */
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

 s = (*parse_func_ptr->pv_signature_mode_test) (argc,argv,ccv,scia_ptr,data_ptr,param,
               (struct signature_type *)&sfbp$r_463_test);
       
return (s);
}


/*+
* ===========================================================================
* = vdac_463$$load_color_map - Load the color map =
* ===========================================================================
*
* OVERVIEW:
*       Load the copy color map.
* 
* FORM OF CALL:
*       vdac_463$$load_color_map (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* GLOBALS:
*       none       
*
* FUNCTIONAL DESCRIPTION:
*       The BT463 has a 256x24 bit color ram. This routine 
*       will load the color ram with black,blue and red. 
*       We need to set it up so that 0 is black, 1 is red 
*       2 is blue.
*
--*/
int     vdac_463$$load_color_map (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int     i,j,k,s;
int      temp1,temp2;
unsigned char temp[3];

REPEAT(i,256)
 {
 temp[0] = temp[1] = temp[2] = i;
  (*bt_func_ptr->pv_bt_write_color)(scia_ptr,bt463$k_color_ram+i,temp);
 }

return (SS$K_SUCCESS);
}



/*+
* ===========================================================================
* = vdac_463$$cur_onscreen - Put the cursor off the screen =
* ===========================================================================
*
* OVERVIEW:
*       Put the cursor in the middle of the screen
* 
* FORM OF CALL:
*       vdac_463$$cur_onscreen (scia_ptr,data_ptr)
*
* RETURNS:
*       1
* ARGUMENTS:
*       scia_ptr        - driver pointer
*       data_ptr        - driver data area pointer
*
* FUNCTIONAL DESCRIPTION:
*        Put the cursor in the middle of the screen.
*
--*/
int     vdac_463$$cur_onscreen (SCIA *scia_ptr,SFBP *data_ptr)

{
register  int  i,save_row,save_col;

save_row = data_ptr->sfbp$l_cursor_row;
save_col = data_ptr->sfbp$l_cursor_column;

data_ptr->sfbp$l_cursor_row    = 0;
data_ptr->sfbp$l_cursor_column = 0;

(*bt_func_ptr->pv_bt_pos_cursor)(scia_ptr,data_ptr,
                data_ptr->sfbp$l_cursor_row,
                data_ptr->sfbp$l_cursor_column,FALSE);
 
data_ptr->sfbp$l_cursor_row    = save_row;
data_ptr->sfbp$l_cursor_column = save_col;

return (SS$K_SUCCESS);
}


/*+
* ===========================================================================
* = vdac_463$$cur_offscreen - Put the cursor off the screen =
* ===========================================================================
*
* OVERVIEW:
*       Put the cursor in the middle of the screen
* 
* FORM OF CALL:
*       vdac_463$$cur_offscreen (scia_ptr,data_ptr)
*
* ARGUMENTS:
*       scia_ptr        - driver pointer
*       data_ptr        - driver data area pointer
*
* FUNCTIONAL DESCRIPTION:
*        Put the cursor in the middle of the screen.
*
--*/
int     vdac_463$$cur_offscreen (SCIA *scia_ptr,SFBP *data_ptr)

{
register  int       i;

return (SS$K_SUCCESS);
}


/*+
* ===========================================================================
* = vdac_463$$control_plane_test - VDAC Control And Overlay RAM Test =
* ==========================================================================
*
* OVERVIEW:
*       Load the video ram control and overlay planes and verify it.
* 
* FORM OF CALL:
*       vdac_463$$control_plane_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       The purpose of this test is to test the control and overlay planes 
*       that are on the modules that support this. The 32 plane configuration
*       is somewhat different in that all the overlay and control information
*       is located within the 32 bit longword in frame buffer address space and
*       this is seen as one contiguous set of memory. We have already done a
*       cell integrity test, so the point of this test is to verify the control
*       and overlay signal lines from the video ram chips to the ramdac. There
*       are 4 control lines which are the window type field. There are 4 overlay
*       inputs which are then used by the 463 to display overlay colors. The
*       463 has overlay ram (16 colors) which are located from 200 to 20F in
*       each respective red,green and blue color map. We will load up the 
*       color map, and then test out each overlay color. To test the control
*       inputs, I already have set up all window types to be the same, so 
*       I am not sure specifically how I will test the window type fields. 
*
--*/
int     vdac_463$$control_plane_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  i,temp,s=1;
char     *vram;

/* Just 16 unique colors for 16 unique overlay inputs   */
static   unsigned char overlay_table [16][3] =
        {
        0x00,0x00,0x00, 
        0x00,0x00,0xff, 
        0x00,0xff,0x00, 
        0xff,0x00,0x00, 
        0x00,0xff,0xff, 
        0xff,0xff,0x00, 
        0xff,0x00,0xff, 
        0xff,0xff,0xff, 
        0x80,0x80,0x80, 
        0x00,0x00,0x80, 
        0x00,0x80,0x00, 
        0x80,0x00,0x00, 
        0x00,0x80,0x80, 
        0x80,0x80,0x00, 
        0x80,0x00,0x80, 
        0xC0,0xC0,0xC0, 
        };

static struct signature_type sfbp$r_463_control_test = 
 {
 0,
 0xb8,0xb8,0xb8,0x93,                            
 0xc8,0xc8,0xc8,0xfe,
 0xd5,0xd5,0xd5,0xce,
 };

#ifdef DOUBLE
 /* First update the overlay color map for my colors      */

 for (i=0;i<16;i++)                                       
    (*bt_func_ptr->pv_bt_write_ovl_color)(scia_ptr,bt463$k_overlay_color+i,overlay_table[i]);
  
 /* Just use the first 16 lines of video ram memory      */
 vram = (char *)0;

 /* The screen should have 16 lines of overlay color     */
 /* Use window type of zero for now                      */
 for (i=0;i<16;i++,vram+=data_ptr->sfbp$l_bytes_per_scanline)                                       
        (*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,
                        vram,(i<<24),data_ptr->sfbp$l_bytes_per_scanline);

if (data_ptr->sfbp$l_signature_test)
         s = (*parse_func_ptr->pv_signature_mode_test) (argc,argv,ccv,scia_ptr,data_ptr,param,
               (struct signature_type *)&sfbp$r_463_control_test);
#endif

return (s);
}


