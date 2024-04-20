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
 *      BT485 RAMDAC test .
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
 * 	Eric Goehl 	14-Sep-1994	Added command register 3 to the register test - it requires different
 *					access cycles to reach then the other registers.
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

#ifdef TGA
#define CHECK_RAMDAC() {if  (data_ptr->sfbp$l_ramdac != bt485  )\
                            {printf ("TFL TGA RAMDAC is not BT485 \n"); \
                             return (0);}}
#else
#define CHECK_RAMDAC() {if  (data_ptr->sfbp$l_ramdac != bt485  )\
                            {printf ("TFL PMAG-?? RAMDAC is not BT485 \n"); \
                             return (0);}}
#endif

#define sfbp$k_rs_cmdreg3 sfbp$k_rs_status


/*+
* ===========================================================================
* = vdac_485$$fill_vram - Fill video ram =
* ===========================================================================
*
* OVERVIEW:
*       Fill video ram
* 
* FORM OF CALL:
*       vdac_485$$fill_vram (argc,argv,vector,scia_ptr,data_ptr,param )
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
*       Fill video ram. This will fill video ram with a incrementing
*       pattern from 00 to 256 to represent the 256 possible 
*       colors from the RAMDAC. Each longword of video ram is filled 
*       with the next incrementing pattern. So the first 256 longwords
*       is filled with the address modulo 256. This is repeated for the
*       amount of memory within the system. This is used to populate the
*       video rams for the signature tests.
*
--*/
int     vdac_485$$fill_vram (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])


{
register int      s=1;
register int      *vid,num_bytes;

CHECK_RAMDAC();

vid            = (int *)0;
num_bytes      = data_ptr->sfbp$l_vram_size; 
                     
s=(*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,vid,0x55555555,num_bytes);

return (s);
}



/*+
* ===========================================================================
* = vdac_485$$cmd_reg_test - VDAC command Register Test =
* ===========================================================================
*
* OVERVIEW:
*       Load the VDAC command registers and verify them.
* 
* FORM OF CALL:
*       vdac_485$$cmd_reg_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       This test loads the BT485 command registers and reads them back to
*       check for integrity. The BT485 automatically increments its address
*       register after each write cycle. So we load the address register 
*       with the initial value and then read a table of values to load the
*       registers. We then reload the address register to read the values.
*               I do not test the test register or the signature register
*       because these values change during active and non active video.
*
--*/
int     vdac_485$$cmd_reg_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int     i,s=1;
int      temp1,temp2;
unsigned char temp;
static   struct  vdac_reg_type {
                int     btreg;
                unsigned char    value;
        } vdac_cmd_regs[] = 
        {
	sfbp$k_rs_cmd_reg0,      0xA2,                 
	sfbp$k_rs_cmd_reg1,      0x40,                 
  	sfbp$k_rs_cmd_reg2,      0x25,                 
  	sfbp$k_rs_cmdreg3,       0x14,   /* Special access required for this register. */              
        sfbp$k_rs_pixel_mask,    0xff,   
        };                           
                                     
register struct   bt_func_block *bt; 
                                     
CHECK_RAMDAC();                      
                                                                                      
for (i=0,s=1;s&1&&i*sizeof(struct vdac_reg_type)<sizeof(vdac_cmd_regs);i++)           
 {                                                                                    
  if (vdac_cmd_regs[i].btreg==sfbp$k_rs_cmdreg3){                                    
    (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_addr_pallete_write,1);        
    (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_cmdreg3,vdac_cmd_regs[i].value);      
    temp = (*bt_func_ptr->pv_bt_read_reg)(scia_ptr,vdac_cmd_regs[i].btreg);
    (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_addr_pallete_write,0);            
  }                                                                                       
  else {                                                                                  
    (*bt_func_ptr->pv_bt_write_reg)(scia_ptr,vdac_cmd_regs[i].btreg,vdac_cmd_regs[i].value);
    temp = (*bt_func_ptr->pv_bt_read_reg)(scia_ptr,vdac_cmd_regs[i].btreg);
  }
  if ( temp != vdac_cmd_regs[i].value || data_ptr->sfbp$l_test_error )
    {                                 
     param[0] = vdac_cmd_regs[i].btreg;
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
* = vdac_485$$col_ram_test - VDAC Color RAM Test =
* ===========================================================================
*
* OVERVIEW:
*       Load the VDAC color ram and verify it.
* 
* FORM OF CALL:
*       vdac_485$$col_ram_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       The BT485 has a 256x24 bit color ram. This routine 
*       will test the color ram with incrementing and decrementing
*       data patterns. 
*
--*/
int     vdac_485$$col_ram_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int     i,j,k,s=1;
int      temp1,temp2;
unsigned char temp[3];

CHECK_RAMDAC();

for (i=0,s=1;s&1&&i<256;i++)                                    
 {
   temp[0]=temp[1]=temp[2]=i;
   (*bt_func_ptr->pv_bt_write_color)(scia_ptr,i,temp);
   (*bt_func_ptr->pv_bt_read_color)(scia_ptr,i,temp);
   for (j=0;s&1&&j<3;j++)
   if (temp[j] != i || data_ptr->sfbp$l_test_error )
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
* = vdac_485$$cur_ram_test - VDAC Cursor RAM Test =
* ===========================================================================
*
* OVERVIEW:
*       Load the VDAC cursor ram and verify it.
* 
* FORM OF CALL:
*       vdac_485$$cur_ram_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       The BT485 has a 64x64x2 bit plane cursor ram. This routine 
*       will test the cursor ram with incrementing and decrementing
*       data patterns. The size of cursor ram is 8192 bits which is
*       1024 bytes of addressable memory. When ever we load cursor
*       ram, we should wait for vertical retrace to occur so we 
*       load during the BLANK level.
*		The 485 has 2 bits A8:9 which must be set to get to
*	the larger array, in addition to the CR32 bit.
--*/
int     vdac_485$$cur_ram_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int   patt,i,j,k,s=1,retries;
int      addr,cmdreg0,cmdreg3,msb,temp1,temp2;
unsigned char temp;
static   int      pattern[4]={0xff,0x55,0xAA,0x00};

CHECK_RAMDAC();

 /* Position my cursor in middle of screen		*/
 (*bt_func_ptr->pv_bt_pos_cursor)(scia_ptr,data_ptr,
 	data_ptr->sfbp$l_cursor_max_row/2,                                                            
        data_ptr->sfbp$l_cursor_max_column/2,FALSE);                                                  
                                                                                                      
 cmdreg0  = (*bt_func_ptr->pv_bt_read_reg) (scia_ptr,sfbp$k_rs_cmd_reg0);                             
 cmdreg3  = ((*bt_func_ptr->pv_bt_read_reg) (scia_ptr,sfbp$k_rs_cmdreg3))&0x1c;
 cmdreg3 |= 0x10;      /* Enable Differential ECL clock. */                 
                                                                            
 for (j=0,s=1;s&1&&j<4;j++)                                                 
 {                                                                          
 patt = pattern[j]&0xff;                                                    
 /* Initialize 10 bit counter to zero		     	*/                  
 (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_addr_pallete_write,1); 
 (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_cmdreg3,cmdreg3);      
 (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_addr_pallete_write,0); 
                                                                            
 /* Now just cycle thru auto incrementing each time	*/                  
 for (i=0;s&1&&i<sfbp$K_SIZE_CURSOR_RAM;i++)                                         
	(*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_cursor_ram,patt);
                                                                            
 /* Initialize 10 bit counter to zero			*/                  
 (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_addr_pallete_write,1); 
 (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_cmdreg3,cmdreg3);      
 (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_addr_pallete_write,0); 
                                                                            
 /* Now just cycle thru auto incrementing each time	*/                  
 for (i=0;s&1&&i<sfbp$K_SIZE_CURSOR_RAM;i++)                                         
   {                                                                        
    temp = ((*bt_func_ptr->pv_bt_read_reg)  (scia_ptr,sfbp$k_rs_cursor_ram))&0xff;
    if (temp != patt || data_ptr->sfbp$l_test_error )                     
      {                                                                   
      param[0] = i;                                                       
      param[1] = patt;                                                    
      param[2] = temp;                                                    
      s=0;                                                                
      }
    }
  } 
 
return (s);
}


/*+
* ===========================================================================
* = vdac_485$$cur_col_test - VDAC Cursor Color Register Test =
* ===========================================================================
*
* OVERVIEW:
*       Load the VDAC cursor color registers and verify them.
* 
* FORM OF CALL:
*       vdac_485$$cur_col_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*       The BT485 has 3 registers for the color of the cursor. The pixel 
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
int     vdac_485$$cur_col_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int     i,j,k,s=1,pix,retries;
int      cmdreg3,msb,temp1,temp2;
unsigned char  temp[3];
static   int   pixel[4]= {0xff,0x55,0xAA,0xff};                                         /* 00,01,10,11   */
static   unsigned char color[3][3] =
	{
        0xff,0x00,0x00,                                                                 /* red     01    */
        0x00,0xff,0x00,                                                                 /* green   10    */
        0x00,0x00,0xff
	};                                                                		/* blue    11    */

 CHECK_RAMDAC();

 for (i=0;i<3;i++)                                                                      /* Colors       */
    (*bt_func_ptr->pv_bt_write_cur_color)(scia_ptr,1+i,color[i]);			/* 01,10,11	*/
 
 cmdreg3  =((*bt_func_ptr->pv_bt_read_reg) (scia_ptr,sfbp$k_rs_cmdreg3))&0x1c;
 cmdreg3 |= 0x10;  /* Enable Differential ECL clock. */                     

 if (data_ptr->sfbp$l_print)
   printf("Bt485 RAMDAC cmdreg3 = %x.\n",cmdreg3);
 /* Initialize 10 bit counter to zero			*/ 
 (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_addr_pallete_write,1);
 (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_cmdreg3,cmdreg3);
 (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_addr_pallete_write,0);

  for (i=k=0;i<4;i++)    
    for (j=0;j<sfbp$K_SIZE_CURSOR_RAM/4;j++,k++)                                          
        (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_cursor_ram,pixel[i]);
   
 for (i=0,s=1;s&1&&i<3;i++)                                                              
  {
   (*bt_func_ptr->pv_bt_read_cur_color) (scia_ptr,1+i,temp);
   if (data_ptr->sfbp$l_print)
      printf("Color register %x read as %x.\n",1+i,temp[i]);
   for (j=0,s=1;s&1&&j<3;j++)
     {
     if (temp[j] != color[i][j] || data_ptr->sfbp$l_test_error  )                     
       {
       param[0] = 1+i;
       param[1] = color[i][j]&0xff;
       param[2] = temp[j]&0xff; 
       s=0;                
       break;                                              
       }                                                   
    }
  }

return (s);
}

