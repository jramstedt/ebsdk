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
 *      BT459 RAMDAC Crosshair test.
  *
 *  AUTHORS:
 *
 *      James Peacock
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
* = chair$$cur_chair_test - Cursor Crosshair Test =
* ===========================================================================
*
* OVERVIEW:
*       Verify Crosshair 
* 
* FORM OF CALL:
*       chair$$cur_chair_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       scia_ptr    - shared driver pointer
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*       This sets up the cursor crosshair for visual indication. We make
*       a full screen crosshair out of it being 7 pixels in width. We do
*       not use the crosshair, but I have included it for display.
*
--*/
int     test_chair (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
if (data_ptr->sfbp$l_test_offchip || data_ptr->sfbp$l_ramdac == bt463)
        chair$$cur_offchip_test (argc,argv,ccv,scia_ptr,data_ptr,param);
else if (data_ptr->sfbp$l_ramdac == bt485)
            chair$$cur_485_test (argc,argv,ccv,scia_ptr,data_ptr,param);


return (1);
}


/*+
* ===========================================================================
* = chair$$cur_485_test - Cursor Crosshair Test =
* ===========================================================================
*
* OVERVIEW:
*       Verify Crosshair 
* 
* FORM OF CALL:
*       chair$$cur_485_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       scia_ptr    - shared driver pointer
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*       This sets up the cursor crosshair for visual indication. We make
*       a full screen crosshair out of it being 7 pixels in width. We do
*       not use the crosshair, but I have included it for display.
*
--*/
int     chair$$cur_485_test (int argc,char *argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
unsigned  char      buffer[20],local=0;
register  int       done,i,xbias,ybias;

        {
        xbias          = data_ptr->sfbp$r_mon_data.sfbp$l_cursor_x;
        ybias          = data_ptr->sfbp$r_mon_data.sfbp$l_cursor_y;

        for (done=0;!done;)
         {

         /* Move the cursor based on bias       */
         
         (*bt_func_ptr->pv_bt_write_reg)(scia_ptr,sfbp$k_rs_cursor_x_lo ,xbias&0xff);
         (*bt_func_ptr->pv_bt_write_reg)(scia_ptr,sfbp$k_rs_cursor_x_hi ,xbias>>8);				
         (*bt_func_ptr->pv_bt_write_reg)(scia_ptr,sfbp$k_rs_cursor_y_lo ,ybias&0xff);
         (*bt_func_ptr->pv_bt_write_reg)(scia_ptr,sfbp$k_rs_cursor_y_hi ,ybias>>8);				
        
         CLEAR(i,buffer,sizeof(buffer));
         buffer[1]=getchar();

         /* Use the arrow keys at alt console   */
         switch (buffer[1])
          {
          case 'A':case '3':ybias--;break;
          case 'B':case '4':ybias++;break;
          case 'C':case '2':xbias++;break;
          case 'D':case '1':xbias--;break;
          case 'q':
          case 'Q':
          case 13 :
          case 10 : done=TRUE;
          }
         }
        } 
        
return (SS$K_SUCCESS);
}



/*+
* ===========================================================================
* = chair$$cur_offchip_test - Cursor Crosshair OffChip =
* ===========================================================================
*
* OVERVIEW:
*       Verify Crosshair 
* 
* FORM OF CALL:
*       chair$$cur_offchip_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       scia_ptr    - shared driver pointer
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*       This sets up the cursor crosshair for visual indication. We make
*       a full screen crosshair out of it being 7 pixels in width. We do
*       not use the crosshair, but I have included it for display.
*
* RETURN CODES:
*       param[0] = value;                                  P1 = value              
*       param[1] = vdac_cmd_regs[i]&0xff;                  P2 = addr low           
*       param[2] = vdac_cmd_regs[i]>>8;                    P3 = addr High          
*
* ALGORITHM:
*       - Set the window x and y registers to zero
*       - Set the window width and height registers to 0xFFF
*       - Load the cursor command register again and set crosshair up.
*
--*/
int     chair$$cur_offchip_test (int argc,char *argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register  int  i,rows,bypixel=TRUE,done,base,row=0,col=0;
char      inchar;

        {
        row = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
        col = data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels/2;

	for (i=0;i<1024;i+=4)
	   (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,i,0xffffffff,LW);   

	rows   = 63;
	base   = rows<<10;
	(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_cursor_base,base,LW);

        for (done=0;!done;)
         {
         (*bt_func_ptr->pv_bt_pos_cursor)(scia_ptr,data_ptr,row,col,bypixel);
         inchar = getchar ();
         switch (inchar)
          {
          case 'A':case '3':row--;break;
          case 'B':case '4':row++;break;
          case 'C':case '2':col++;break;
          case 'D':case '1':col--;break;
          case 'q':
          case 'Q':
          case 13 :
          case 10 : done=TRUE;
          }
         }
        } 

 (*bt_func_ptr->pv_bt_cursor_ram)(scia_ptr,data_ptr);
        
return (1);
}

