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
 *      Font scroller
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
* = font$$flood_screen - Flood the screen with character =
* ===========================================================================
*
* OVERVIEW:
*       Initialize the screen with specific character.
* 
* FORM OF CALL:
*       font$$flood_screen (argc,argv,vector,scia_ptr,data_ptr,param )
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       vector      - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
*
* FUNCTIONAL DESCRIPTION:
*       This is used to flood the screen with the appropriate character.
*       If no characters are specified on the command line, then the
*       font set from a space character to a tilda are put on the
*       screen. This character set is 94 characters in length and will 
*       repeat itself after that. When we get to the next row, then the
*       start character is incremented by 1 so the characters will be
*       shifted.
*
--*/
int     font$$flood_screen (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int  MaxCount,save_row,save_col;
register int  foreground,spaces,ColumnsToTest,Bright,mcs=0,Count,SaveFont,font,max_rows,max_cols,done,i,j,s=1;
struct        buffer_desc b;
char          *pptr,ic,c;
struct        FILE *tgafp;
char	      NewIntensity[3];
static char   colors[2][3]=
        {       
        0xff,0xff,0xff,                                 /* White        */
        0x00,0x00,0x00,                                 /* Black        */
        };

#define White 0
#define Black 1

max_rows = data_ptr->sfbp$l_cursor_max_row;
max_cols = data_ptr->sfbp$l_cursor_max_column;

data_ptr->sfbp$l_cursor_update = FALSE;

save_row = data_ptr->sfbp$l_cursor_row;
save_col = data_ptr->sfbp$l_cursor_column;

data_ptr->sfbp$l_cursor_column = 0;
data_ptr->sfbp$l_quick_verify  = FALSE;
data_ptr->sfbp$l_cursor_row    = 0;

for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
	if (*pptr++=='-'&& *pptr++== 'm' )
		{mcs = 1; break;}

/* Use this for intensity of the Character Painted	*/
for (Bright=j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
	if (*pptr++=='-'&& *pptr++== 'i' )
		{Bright = strtol (pptr,0,10); Bright &= 0xff; break;}

/* Use this for a space between each character		*/
for (ColumnsToTest=j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
	if (*pptr++=='-'&& *pptr++== 'c' )
		{
		ColumnsToTest = strtol (pptr,0,10);
	        if (ColumnsToTest>max_cols)ColumnsToTest = max_cols;
		break;
		}

/* Use this for a foreground pattern			*/
for (foreground=j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
	if (*pptr++=='-'&& *pptr++== 'p' )
		{
		foreground = strtol (pptr,0,16);
		break;
		}

/* Set up fg register first              */
if (foreground)(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_foreground,foreground,LW);
else (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_foreground,0,LW);

/* If black on white...0 = max and ff = min */
if (!data_ptr->sfbp$l_white_on_black)
 {
 (*bt_func_ptr->pv_bt_write_color)(scia_ptr,  0,colors[White]);
 (*bt_func_ptr->pv_bt_write_color)(scia_ptr,255,colors[Black]);
 }

if (Bright)
 {
 NewIntensity[0] = Bright;
 NewIntensity[1] = Bright;
 NewIntensity[2] = Bright;
 (*bt_func_ptr->pv_bt_write_color)(scia_ptr,255,NewIntensity);
 }

tgafp     = fopen ("tga0","w");	
MaxCount  = 2;

/* Set up Descriptor For Characters	*/
b.ADDRESS       = &c;
b.size		 = 1;
                     
for (Count=0,ic='!';(Count<MaxCount)&&!io_poll();Count++)
 {                                              
  for (i=0;i<max_rows&&!io_poll();i++)                      
   {                             
    for (j=0,c=ic;j<max_cols&&!io_poll();j++)
     {
      if (data_ptr->sfbp$l_font_char)
        	c=(unsigned char)data_ptr->sfbp$l_font_char;
      else if(c++ == '~')c=' ';      
      if (mcs) c|=0x80;              
      fprintf (tgafp,"%c",c);     	
      for (spaces=0;spaces<ColumnsToTest;spaces++)fprintf (tgafp," ");
      c &= 0x7f;      
     }
   if (ic++ == '~')ic=' ';
  }
 }
data_ptr->sfbp$l_cursor_row    = save_row;
data_ptr->sfbp$l_cursor_column = save_col;
data_ptr->sfbp$l_cursor_update = TRUE;                          
data_ptr->sfbp$l_quick_verify  = FALSE;
data_ptr->sfbp$l_cursor_max_column = max_cols;
  
  
if (!data_ptr->sfbp$l_white_on_black || Bright)
 {
 (*bt_func_ptr->pv_bt_write_color)(scia_ptr,  0,colors[Black]);
 (*bt_func_ptr->pv_bt_write_color)(scia_ptr,255,colors[White]);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_foreground,0,LW);
 }
  
  
 fclose (tgafp); 

return (SS$K_SUCCESS);
}
