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
 *      sfbp Driver support routines
 *
 *  ABSTRACT:
 *
 *      This module provides support routines for the sfbp console drivers. 
 *	These are routines to set up the BT463 RAMDAC.
 *
 *  AUTHORS:
 *
 *      James Peacock
 *
 *
 *  CREATION DATE:      12-13-90 
 *
 *  MODIFICATION HISTORY:
 *	James Peacock	12-13-90	Port from LCG
 *
 *
 *--                      
*/

#include   "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include  "cp$src:tga_sfbp_include.h"
#include "cp$src:tga_sfbp_callbacks.h"
#include "cp$src:tga_sfbp_prom_defines.h"
#include "cp$src:tga_sfbp_def.h"
#include "cp$src:tga_sfbp_protos.h"
#include "cp$src:tga_sfbp_463.h"
#include "cp$src:tga_sfbp_485.h"
#include "cp$src:tga_sfbp_ext.h"
#include "cp$src:tga_sfbp_crb_def.h"
#include "cp$src:tga_sfbp_ctb_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:ev_def.h"


#define   SIZE_DMA_BUFFER    DMA_BYTES
#define   LW_SIZE_DMA_BUFFER (DMA_BYTES/4)
#define   MSWAIT 1000000

int       sfbp$$463_signature_sync (SCIA *scia_ptr, SFBP *data_ptr,int pixel,int input );

void      pci_outmem         (unsigned __int64 address, unsigned int value, int length);
unsigned  int pci_inmem_byte (unsigned __int64 address, int length);
unsigned  int pci_inmem      (unsigned __int64 address, int length);

#define   PCI_byte_mask 0
#define   PCI_word_mask 1
#define   PCI_long_mask 3

int	sfbp$$bt_463_read_window (SCIA *scia_ptr,int address,int *value );
int	rtc_read (int offset);
#define TgaToyOffset 0x16


/*+
 * ===================================================
 * = sfbp$init_bt_463_driver - sfbp ramdac hookup =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine will connect ramdac routines.
  *    
 * FORM OF CALL:
 *  
 *     sfbp$init_bt_463_driver(scia_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr          - pointer to the scia 
 *
 * FUNCTIONAL DESCRIPTION:
 *     Connect the 463 port driver with class driver.
 *
--*/
int	sfbp$init_bt_463_driver (SCIA *scia_ptr,CCV *ccv_vector)

{
bt_func_ptr->pv_bt_init            = sfbp$$bt_463_init ;
bt_func_ptr->pv_bt_write_color     = sfbp$$bt_463_write_color;
bt_func_ptr->pv_bt_write_reg	   = sfbp$$bt_463_write_reg;
bt_func_ptr->pv_bt_load_332_color  = sfbp$$bt_463_load_332_color;
bt_func_ptr->pv_bt_pos_cursor      = sfbp$$bt_463_pos_cursor;
bt_func_ptr->pv_bt_cursor_ram      = sfbp$$bt_463_cursor_ram;
bt_func_ptr->pv_bt_wait_interrupt  = sfbp$$bt_wait_463_interrupt;
#if FULL_TGA
bt_func_ptr->pv_bt_write_cur_color = sfbp$$bt_463_write_cur_color;
bt_func_ptr->pv_bt_load_address    = sfbp$$bt_463_load_address;
bt_func_ptr->pv_bt_write_ovl_color = sfbp$$bt_463_write_ovl_color;
bt_func_ptr->pv_bt_alloc_color     = sfbp$$bt_463_alloc_color;
bt_func_ptr->pv_bt_free_color      = sfbp$$bt_463_free_color;
bt_func_ptr->pv_bt_free_all_color  = sfbp$$bt_463_free_all_color;
bt_func_ptr->pv_bt_read_reg	   = sfbp$$bt_463_read_reg;
bt_func_ptr->pv_bt_load_888_color  = sfbp$$bt_463_load_888_color;
bt_func_ptr->pv_bt_read_cur_color  = sfbp$$bt_463_read_cur_color;
bt_func_ptr->pv_bt_read_lo         = sfbp$$bt_463_read_lo;
bt_func_ptr->pv_bt_read_hi         = sfbp$$bt_463_read_hi;
bt_func_ptr->pv_bt_read_ovl_color  = sfbp$$bt_463_read_ovl_color;
bt_func_ptr->pv_bt_read_color      = sfbp$$bt_463_read_color;
bt_func_ptr->pv_bt_dma_checksum    = sfbp$$bt_463_dma_checksum;
#endif

return(SS$K_SUCCESS);
}


/*+
 * ===================================================
 * = sfbp$$bt_463_init - BT463 RAMDAC Initialization =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  	Initialize the BT463 RAMDAC for driver operation.
 *  
 * FORM OF CALL:
 *     sfbp$$bt_463_init (scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr	- shared console driver ram pointer
 *	data_ptr	- driver data area pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	Initialize the 463
 *
--*/
int	sfbp$$bt_463_init (SCIA *scia_ptr,SFBP *data_ptr)

{
register int	 i,s=1;

static struct   bt_463_init_type {
        int	(*rtn)();				
 	}bt_463_table[]= 
	{
         sfbp$$bt_463_set_cmd_reg0,
         sfbp$$bt_463_color       ,
         sfbp$$bt_463_cursor_ram  ,
         sfbp$$bt_463_window      ,
	};

 for (i=0;i*sizeof(struct bt_463_init_type )<sizeof (bt_463_table);i++)
	(*bt_463_table[i].rtn)(scia_ptr,data_ptr);

 sfbp$$bt_463_pos_cursor (scia_ptr,data_ptr,
        data_ptr->sfbp$l_cursor_row,
        data_ptr->sfbp$l_cursor_column,FALSE);

 return (s);
}



/*+
 * ===================================================
 * = sfbp$$bt_463_set_cmd_reg0 - Set up Command Register 0 =
 * ===================================================
 *
 * OVERVIEW:
 *     set up command register 0
 *  
 * FORM OF CALL:
 *  	sfbp$$bt_463_set_cmd_reg0 (scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr	- scia location pointer
 *	data_ptr	- driver data area pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	This is used to load all of the command registers.
 *
 *
--*/
int	sfbp$$bt_463_set_cmd_reg0(SCIA *scia_ptr,SFBP *data_ptr)

{
int	 data,csr;
#define  GreenSyncEnable 0x80

  sfbp$$bt_463_write_reg(scia_ptr,bt463$k_cmd_reg_0,0x40);
  sfbp$$bt_463_write_reg(scia_ptr,bt463$k_cmd_reg_1,0x08);       

  csr     = 0x40;
  data    = rtc_read (TgaToyOffset);
  data   &= (1<<data_ptr->sfbp$l_slot_number);
  csr    |= data ? GreenSyncEnable : 0;

  sfbp$$bt_463_write_reg(scia_ptr,bt463$k_cmd_reg_2,csr);

  sfbp$$bt_463_write_reg(scia_ptr,bt463$k_pixel_mask_reg_p0_p7,0xff);
  sfbp$$bt_463_write_reg(scia_ptr,bt463$k_pixel_mask_reg_p8_p15,0xff);
  sfbp$$bt_463_write_reg(scia_ptr,bt463$k_pixel_mask_reg_p16_p23,0xff);
  sfbp$$bt_463_write_reg(scia_ptr,bt463$k_pixel_mask_reg_p24_p27,0x0f);
  sfbp$$bt_463_write_reg(scia_ptr,bt463$k_blink_mask_reg_p0_p7,0);
  sfbp$$bt_463_write_reg(scia_ptr,bt463$k_blink_mask_reg_p8_p15,0);
  sfbp$$bt_463_write_reg(scia_ptr,bt463$k_blink_mask_reg_p16_p23,0);
  sfbp$$bt_463_write_reg(scia_ptr,bt463$k_blink_mask_reg_p24_p27,0);

return (SS$K_SUCCESS);
}


/*+
 * ===================================================
 * = sfbp$$bt_463_color - sets up the sfbp color data =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine initializes the BT463 color palette.
 *    
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_color (scia_ptr,data_ptr);
 *
 * ARGUMENTS:
 *	scia_ptr   - pointer to the sfbp shared area
 *      data_ptr   - pointer to the sfbp driver data area
 *
 * FUNCTIONAL DESCRIPTION:
 *      Clear all of color ram to zero.
 *
 *
--*/

int sfbp$$bt_463_color (SCIA *scia_ptr,SFBP *data_ptr)


{
register int	i;
static char   colors[3]={0x00,0x00,0x00};
        
for (i=0;i<sfbp$K_SIZE_BT463_COLOR_RAM;i++)
        sfbp$$bt_463_write_color(scia_ptr,i,colors);

return (SS$K_SUCCESS);
}


/*+
 * ===================================================
 * = sfbp$$bt_463_window - sets up the sfbp color data =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine initializes the BT463 window table.
 *    
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_window (scia_ptr,data_ptr);
 *
 * ARGUMENTS:
 *	scia_ptr   - pointer to the sfbp shared area
 *      data_ptr   - pointer to the sfbp driver data area
 *
 * FUNCTIONAL DESCRIPTION:
 *      Initialize the window table entries. I set all the window
 *      type entries to be the same. However, since the upper 2
 *      entries (14 and 15) are for the cursor, these 2 entries
 *      are only planes in depth and will differ from the rest. 
 *      Updating the window type entries requires that we wait
 *      for a vertical retrace.
 *
--*/

int sfbp$$bt_463_window (SCIA *scia_ptr,SFBP *data_ptr)


{
union    window_463 *w;
register int	i;
int      s=1,EntryCount,local=0,local1;

w              = (union window_463 *)&local;

for (i=0;s&1 && i<16;i++)
        {
        w->sfbp$r_window_463_bits.sfbp$v_window_463_planes = i<14 ? 8:2;            
        s= sfbp$$bt_463_write_window (scia_ptr,bt463$k_window_type_table+i,local);
        }

return (1);
}


/*+
 * ===================================================
 * = sfbp$$bt_463_cursor_ram - Set up the BT463 cursor =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine initializes the BT463 cursor.
  *    
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_cursor_ram (scia_ptr,data_ptr);
 *
 * ARGUMENTS:
 *	scia_ptr   - pointer to the sfbp shared area
 *      data_ptr   - pointer to the sfbp driver data area
 *
 * FUNCTIONAL DESCRIPTION:
 *	This routine is used to set up the cursor ram in the BT463.
 *      The cursor ram is 64 x 64 pixels with 2 bits of information
 *      per pixel.  This translates to 8192 bits (1024 bytes) of pixel 
 *      information that can be addressed by the cpu. In our case, our 
 *      cursor pattern is a single scanline and we are setting that up
 *      to be using the color from cursor color register 3, which means
 *      both bits are on. The first 2 bytes are written with 0xFF and 
 *      the rest of RAM is written with zero for no pattern. This should
 *      result in an underscore or topscore at the specified position 
 *      on the physical display.
 *              After cursor ram is initialized with the lut value, then
 *      the 3 color registers are loaded with the color white.
 *              The cursor ram is organized as 64 pixels across which
 *      is 16 bytes and 64 pixels down which is also 16 bytes. So we
 *      need to make the cursor illuminated for the size of a character
 *      block which is our font height (15 pixels) and font width which
 *      is 8 pixels. So we will illuminate the upper left box of the 
 *      available cursor ram. 
 *              There is a trick in loading the cursor ram, you have to make
 *      sure to reload the address register when going to the next row.
 *              The 463 gets it's cursor ram data from video ram as define
 *      by the cursor base address register. I put this at address 0 in the
 *      frame buffer which is for the cursor.
 *              There is a trick in loading the cursor ram, you have to make
 *      sure to reload the address register when going to the next row. For the
 *      24 plane system, we can get 16 pixels (font width) into 1 longword.
 *
--*/

int sfbp$$bt_463_cursor_ram (SCIA *scia_ptr,SFBP *data_ptr)

{
register int  Height,Width,CursorData,i;
static char   colors[3]={0xff,0xff,0xff};                                                         
        
for (i=0;i<1024;i+=4)
        (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,i,0,LW);   

CursorData = 0xffff;								/* 8 pix=2bytes */
Height     = scia_ptr->font_height;                                             /* And Height   */

for (i=0;i<Height;i++)                                                          
  (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,i*16,CursorData,LW);                                   

sfbp$$bt_463_write_cur_color (scia_ptr,bt463$k_cur_color_0,colors);          
sfbp$$bt_463_write_cur_color (scia_ptr,bt463$k_cur_color_1,colors);          

return (SS$K_SUCCESS);
}


/*+
 * ===================================================
 * = sfbp$$bt_463_pos_cursor - pos the BT463 cursor =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine poss the BT463 cursor.
  *    
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_pos_cursor (scia_ptr,data_ptr,row,column,bypixel);
 *
 * ARGUMENTS:
 *	scia_ptr   - pointer to the sfbp shared area
 *      data_ptr   - pointer to the sfbp driver data area
 *      row        - row
 *      column     - column
 *
 * FUNCTIONAL DESCRIPTION:
 *	This routine is used to set pos the cursor in the BT463.
 *      The cursor X and Y registers are loaded with row and 
 *      column information. The driver data area has the constants for
 *      the cursor constants needed for calculating the position. The
 *      current row and column are extracted from the driver data 
 *      area. The X and Y registers specify the center of the 
 *      cursor, we may have to position the cursor offset by half
 *      of a character width and height.
 *              I left a do while loop in here for empircal determination
 *      of the cursor position when we use a different monitor. 
 *
	The cursor XY register defines the position of the top left corner
	of the 64x64 pixel cursor region.  The cursor can be turned on and
	off using the enable bit in the Video Valid register.


	31            24 23                   12 11                    0
	+---------------+-----------------------+----------------------+
	|    ignore     |       Cursor Y        |       Cursor X       |
	+---------------+-----------------------+----------------------+

	Value at init: 0

        Cursor X min:
        ------------
        horizontal(sync + back porch)pixels - 63

        Cursor X max:
        ------------
        horizontal(sync + back porch + active)pixels - 1

        Cursor Y min:
        ------------
        vertical(sync + back porch)lines

        Cursor Y max:
        ------------
        vertical(sync + back porch + active)lines - 1


        RAMDACs without on-chip cursor
        ------------------------------
        For RAMDACs that do not have on-chip cursor hardware
        (i.e. BT458 and BT463):   The Cursor XY register defines the
        position of the top left corner	of the 64x64 cursor in pixels
        relative to start of the top row of the screen. (Start of the
        top row of the screen is defined by the falling edge of vertical
        sync followed by the first falling edge of horizontal sync).
        The cursor is not displayed unless the enable bit is set.

        BT458
        -----
        Cursor data is driven through the overlay port.
        The cursor color is changed via overlay color registers within
        the BT458.  Cursor pattern may be updated in video RAM, at any time,
        preferably during vertical retrace.  CR6 must be set (1) to
        enable the color palette RAM.


        BT463
        -----
        The cursor data will be muxed with the window type.  BT463 CR13
        must be set (1).  With CR13 set, window type
        0xE is defined as cursor color 0 and 0xF is cursor color 1.
        Cursor color is updated via the cursor color registers.

        Cursor pattern may be updated in video RAM, at any time,
        preferably during vertical retrace.


        RAMDACs with on-chip cursor
        ---------------------------
	Refer to the Brooktree Databook for RAMDACs with on-chip cursor
        hardware (i.e. BT459, BT460, and BT468.)


 *
 *
--*/

int sfbp$$bt_463_pos_cursor (SCIA *scia_ptr,SFBP *data_ptr,int row,int column,int bypixel)

{
register int base,rows=0,value,xpos,ypos,offset;

if (bypixel)
 {
 xpos =  column;
 ypos =  row;
 }
else
 {
  xpos =  column*scia_ptr->font_width;
  ypos =  row*scia_ptr->font_height;
 }

xpos += data_ptr->sfbp$r_mon_data.sfbp$l_cursor_x;
ypos += data_ptr->sfbp$r_mon_data.sfbp$l_cursor_y;

value  = (ypos<<12)|xpos;
offset = data_ptr->sfbp$l_cursor_offset;
rows   = scia_ptr->font_height-1;        
base   = (rows<<10)|(offset<<4);

if (!bypixel)
	(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_cursor_base,base,LW);

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_cursor_xy,value,LW);
(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_video_valid,ActiveVideo|CursorEnable,LW);


return (SS$K_SUCCESS);

}


/*+
 * ===================================================
 * = sfbp$$bt_463_write_reg - Write data to BT463 register =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Load data into BT463 register
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_write_reg (scia_ptr,address,data );
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver pointer
 *	address  - BT463 register
 *	data	 - data to load into register
 *
 * FUNCTIONAL DESCRIPTION:
 *	This will load the address register with the BT463 register.
 *	It will then write the data to the register. The BT463 device
 *	requires a 16 bit address register to be loaded with the address
 *	of the register to update. After this is done, then the next 
 *	write operation is data for the register. The sfbp decodes the
 *	addresses to determine if the sfbp address register is being written,
 *	internal register or color palette location.
 *
--*/

int	sfbp$$bt_463_write_reg (SCIA *scia_ptr,int address,int data )

{
register   char	 *reg;

sfbp$$bt_463_load_address (scia_ptr,address);

/* Write  the control setup first       */
reg  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
reg += (sfbp$k_ramdac_setup);

sfbp$$bus_write (reg,sfbp$k_bt_ctl_register_write ,LW);
wbflush();

/* And then write the data              */
reg  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;

sfbp$$bus_write (reg,(sfbp$k_bt_ctl_register_write<<8)|data,LW);

wbflush();

return (SS$K_SUCCESS );

}



/*+
 * ===================================================
 * = sfbp$$bt_463_load_address - Load BT463 address Register =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Load the BT463 Address register.
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_load_address	(scia,address)
 *
 * ARGUMENTS:
 *	scia	- shared driver interface pointer
 *	address	- register address to load into the address register
 *
 * FUNCTIONAL DESCRIPTION:
 *	This is used to load the address register of the BT463 with a
 *	relative address register. 
 *
--*/
int	sfbp$$bt_463_load_address (SCIA *scia_ptr,int address)

{
register    char	 *lo,*hi;

  
/* Write  the control setup first       */
lo  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
lo += (sfbp$k_ramdac_setup );
sfbp$$bus_write (lo,sfbp$k_bt_ctl_address_lo_write ,LW);
wbflush();

/* And then write the data              */
lo  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
sfbp$$bus_write (lo,(sfbp$k_bt_ctl_address_lo_write<<8)|address&0xff,LW);
wbflush();

/* Write  the control setup first       */
hi  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
hi += (sfbp$k_ramdac_setup );
sfbp$$bus_write (hi,sfbp$k_bt_ctl_address_hi_write ,LW);
wbflush();

/* And then write the data              */
hi = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
sfbp$$bus_write (hi,(sfbp$k_bt_ctl_address_hi_write<<8)|((address>>8)&0xff),LW);
wbflush();

return (SS$K_SUCCESS);
}


/*+
 * ===================================================
 * = sfbp$$bt_463_write_color - Write BT463 Color Palette =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Write Read Color Palette RAM in BT463
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_write_color (scia_ptr,address,color)
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address  - relative address of color palette ram location (0 to 512).
 *	color    - rgb array
 *
 * FUNCTIONAL DESCRIPTION:
 *	The color palette RAM is 512 x 24 bits deep and represents
 *	512 possible colors that the BT463 can produce on the video 
 *	monitor. Each entry is 24 bits wide, 8 bits for RED,8 bits for
 *	GREEN and 8 Bits for Blue. To write to color palette RAM,
 *	all we need to do is write to location 0000-00FF with C1:C0 set
 *	to 11, which is done by the sfbp decode logic. The address register
 *	is loaded with the color palette ram location. The BT463 then expects
 *	three successive write cycles 8 bits RED, 8 Bits GREEN and then 8 bits
 *	of BLUE. The address register is automatically incremented to the 
 *	next color palette location following the blue write cycle. A color is
 *	maximum intensity when set to FF and minimum intensity (black) when set
 *	to 0.
 *              This routine is fine to call for a few entries. If you are 
 *      loading the entire color ram, then use a separate routine.
 *
 *	
--*/

int	sfbp$$bt_463_write_color (SCIA *scia_ptr,int address,char color[])

{
register   char    *c;
register   int i;

sfbp$$bt_463_load_address (scia_ptr,address);		

/* Write  the control setup first       */
c  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
c += (sfbp$k_ramdac_setup );
sfbp$$bus_write (c,sfbp$k_bt_ctl_color_write ,LW);
wbflush();

/* And then write the data              */
c  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;

for (i=0;i<3;i++)
 {
 sfbp$$bus_write (c,(sfbp$k_bt_ctl_color_write<<8)|(color[i]&0xff),LW);
 wbflush();
 }

return (SS$K_SUCCESS);
}



/*+
 * ===================================================
 * = sfbp$$bt_463_write_window - Write BT463 Color Palette =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Write Read Color Palette RAM in BT463
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_write_window (scia_ptr,address,value )
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address  - relative address of color palette ram location (0 to 512).
 *	value    - value to load into entry
 *
 * FUNCTIONAL DESCRIPTION:
 *      Fill in window type entry . Return 0 if interrupts not working
 *
 *	
--*/

int	sfbp$$bt_463_write_window (SCIA *scia_ptr,int address,int value )

{
register char    *c;
register int i,s=1;
int	 b0,b8,b16;

 data_ptr->sfbp$l_int_poll = 1;

 sfbp$$bt_463_load_address (scia_ptr,address);		

 /* Wait for Vertical Retrace twice to make sure */
 for (i=0;s&1 && i<2;i++)
  {
  (*gfx_func_ptr->pv_clear_int_req)   (scia_ptr,data_ptr,end_of_frame);
  s=(*bt_func_ptr->pv_bt_wait_interrupt)(scia_ptr,data_ptr,FALSE,0,end_of_frame,FALSE);
  }

 /* Write  the control setup first       */
 c  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
 c += (sfbp$k_ramdac_setup );
 
 sfbp$$bus_write (c,sfbp$k_bt_ctl_register_write ,LW);
 wbflush();

 /* And then write the data              */
 c  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
 
 b0  = (value>>0)&0xff;
 b8  = (value>>8)&0xff;
 b16 = (value>>16)&0xff;
 
 sfbp$$bus_write (c,(sfbp$k_bt_ctl_register_write<<8)|b0,LW); 
 wbflush();

 sfbp$$bus_write (c,(sfbp$k_bt_ctl_register_write<<8)|b8,LW); 
 wbflush();
 
 sfbp$$bus_write (c,(sfbp$k_bt_ctl_register_write<<8)|b16,LW); 
 wbflush();
 
 data_ptr->sfbp$l_int_poll = 0;

 return (s);
}

/*+
 * ===================================================
 * = sfbp$$bt_463_write_cur_color - Write BT463 Cursor Color Palette =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Write Cursor Color Palette RAM in BT463
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_write_cur_color (scia_ptr,address,color)
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address  - relative address of color palette ram location (0 to 512).
 *	color    - rgb array
 *
 * FUNCTIONAL DESCRIPTION:
 *      Load cursor color register with specified rgb value
 *
 *	
 * ALGORITHM:
 *	- Load Address register with color palette ram entry 
 *	- write color data to that address.
--*/

int	sfbp$$bt_463_write_cur_color (SCIA *scia_ptr,int address,char color[])

{
register   char	 *c;
register int  i;

sfbp$$bt_463_load_address (scia_ptr,address);		

/* Write  the control setup first       */
c  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
c += (sfbp$k_ramdac_setup );
sfbp$$bus_write (c,sfbp$k_bt_ctl_register_write ,LW);
wbflush();

/* And then write the data              */
c  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;

for (i=0;i<3;i++)
 {
 sfbp$$bus_write (c,(sfbp$k_bt_ctl_register_write<<8)|(color[i]&0xff),LW);
 wbflush();
 }

return (SS$K_SUCCESS);
}



/*+
* ===========================================================================
* = sfbp$$bt_463_load_332_color - Load Default Color Map =
* ===========================================================================
*
* OVERVIEW:
*       Load the default Color Map Entry 
* 
* FORM OF CALL:
*       sfbp$$bt_463_load_332_color (scia_ptr,data_ptr)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*
* FUNCTIONAL DESCRIPTION:
*       Load a default color map with the following entries. That way we can
*       easily restore the color map. The test routines can then use this
*       during testing without having to regenerate code every time. I will
*       most likely put additions into this for certain colors. I am only writing
*       about 4 entries so I will call the write color routine each time.
*
*       00      - Black
*       01      - White
*       02      - Red
*       03      - Green
*       04      - Blue
*       05      - yellow
*       06      - brown 
*       07      - purple
*      
--*/
int     sfbp$$bt_463_load_332_color (SCIA *scia_ptr,SFBP *data_ptr)

{
int     i,j,k,cnt,s=1,color_index,ColorMapSize;
char    read_color[3],entry_color[3];
static  char red_table [8]=
        {
        0x00, 
        0x20, 
        0x40, 
        0x60, 
        0x80, 
        0xA0, 
        0xC0, 
        0xFF, 
        };

static   char green_table [8] =
        {
        0x20,
        0x40,
        0x60,
        0x80,
        0xA0,
        0xC0,
        0xE0,
        0xFF,
        };

static   char blue_table [4] =
        {
        0x40, 
        0x80, 
        0xC0, 
        0xFF, 
        };
        
for (i=0;i<256;i++)
     {
     entry_color[0] = (i&(7<<5)) ? red_table[((i&(7<<5))>>5)] : 0;
     entry_color[1] = (i&(7<<2)) ? green_table[((i&(7<<2))>>2)] : 0;
     entry_color[2] = (i&(3<<0)) ? blue_table [((i&(3<<0))>>0)] : 0;

     (*bt_func_ptr->pv_bt_write_color)(scia_ptr,i,entry_color);
     }

return (s);
}



/*+
* ===========================================================================
* = sfbp$$bt_wait_463_interrupt - Test the interrupts to CPU =
* ===========================================================================
*
* OVERVIEW:
*       Test for a posted interrupt
* 
* FORM OF CALL:
*       sfbp$$bt_wait_463_nterrupt (scia_ptr, data_ptr,load,pixel,value)
*
* RETURNS:
*       1 for interrupt posted else 0 for no interrupt
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data pointer
*       load            - true if loading seed data for next sample
*       pixel           - which pixel to use (0,1,2, or 3)
*       input           - True if input signature
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*       Call the REX routine to check for the posted interrupt.  Interrupts
*       must be enabled prior to this routine. It is critical that we get
*       a snapshot of the signature registers and test register right 
*       after detecting the interrupt. This is because they are valid 
*       to read during the blank interval which only lasts for a period
*       equal to the vertical retrace time, so we save this information
*       in the driver data area for the signature. A test routine must
*       call the wait_retrace routine so that the information is
*       saved. We take the direct route when reading registers to get the
*       information as soon as possible.
*               When loading signature seed data, it is very important that
*       we do not delay at all, hence the reason for tight loops if the load
*       flag is set.
*
*
--*/
int     sfbp$$bt_wait_463_interrupt  (SCIA *scia_ptr,SFBP *data_ptr,int signature,
	int pixel,INT_TYPE value1,int input)

{
register int   i,j,s=1;
             
if (signature)sfbp$$463_signature_sync (scia_ptr,data_ptr,pixel,input);     
else         
       {     
       for (j=0;j<100000;j++)
                {   
	       	if ( (*gfx_func_ptr->pv_test_int) 
                                (scia_ptr,data_ptr,value1))break;
	       	krn$_micro_delay (10);                                                          
	       	}   
       s = (j==100000) ? 0 : 1;
       }
     
return (s);
}



#if FULL_TGA
/*+
 * ===================================================
 * = sfbp$$bt_463_read_window - Write BT463 Color Palette =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Write Read Color Palette RAM in BT463
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_read_window (scia_ptr,address,value )
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address  - relative address of color palette ram location (0 to 512).
 *	value    - value to load into entry
 *
 * FUNCTIONAL DESCRIPTION:
 *      Fill in window type entry 
 *
 *	
--*/

int	sfbp$$bt_463_read_window (SCIA *scia_ptr,int address,int *value )

{
register char    *c;
register int i;
int	 b0,b8,b16;

 data_ptr->sfbp$l_int_poll = 1;

 sfbp$$bt_463_load_address (scia_ptr,address);		

 /* Wait for Vertical Retrace twice to make sure */
 for (i=0;i<2;i++)
  {
  (*gfx_func_ptr->pv_clear_int_req)   (scia_ptr,data_ptr,end_of_frame);
  (*bt_func_ptr->pv_bt_wait_interrupt)(scia_ptr,data_ptr,FALSE,0,end_of_frame,FALSE);
  }

 /* Write  the control setup first       */
 c  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
 c += (sfbp$k_ramdac_setup );
 
 sfbp$$bus_write (c,sfbp$k_bt_ctl_register_read,LW);
 wbflush();

 /* And then read the data              */
 c  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
 
 b0  = (sfbp$$bus_read (c)>>Dac0ReadData)&0xff;
 b8  = (sfbp$$bus_read (c)>>Dac0ReadData)&0xff;
 b16 = (sfbp$$bus_read (c)>>Dac0ReadData)&0xff;
 
 *value = (b16<<16) | (b8<<8) | b0 ;
 
 return (1);

}



/*+
* ===========================================================================
* = sfbp$$bt_463_alloc_color - Allocate a color =
* ===========================================================================
*
* OVERVIEW:
*       Allocate a color entry
* 
* FORM OF CALL:
*       sfbp$$bt_463_alloc_color (scia_ptr,data_ptr,color)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*       color           - entry index pointer 
*
* FUNCTIONAL DESCRIPTION:
*       Allocate a color entry from 0 to 255 so I can use it. I 
*       walk the bitmap looking for the first zero entry. When
*       I find it, I return with the color index set to the
*       index into the color table. The user then writes this
*       entry in the color table with  the color that he or 
*       she desires and then continues. If I can not allocate
*       an entry, then a zero is returned. The color allocation
*       bitmap is cleared during driver initialization and should
*       also be cleared during a reset output function.
*
--*/
int     sfbp$$bt_463_alloc_color (SCIA *scia_ptr,SFBP *data_ptr,int *color)

{
int s,done,i,j,k,index;
#define   bits_per_longword 32

for (i=k=0,done=FALSE;i<MAXCOLORS&&!done;i++)
 for (j=0,index=1;j<bits_per_longword&&!done;j++,k++,index <<=1)
   if ((data_ptr->sfbp$l_color[i]&index)==FALSE)
      {
      *color=k;
      data_ptr->sfbp$l_color[i] |=index;
      done=TRUE;
      }

return (done);
}


/*+
* ===========================================================================
* = sfbp$$bt_463_free_all_color - Free all colors =
* ===========================================================================
*
* OVERVIEW:
*       Free a color entry
* 
* FORM OF CALL:
*       sfbp$$bt_463_free_all_color (scia_ptr,data_ptr)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*
* FUNCTIONAL DESCRIPTION:
*       Free all colors 
*
--*/
int     sfbp$$bt_463_free_all_color (SCIA *scia_ptr,SFBP *data_ptr)

{
int     i;

for (i=0;i<MAXCOLORS;i++)data_ptr->sfbp$l_color[i]=0;

return (SS$K_SUCCESS);
}



/*+
* ===========================================================================
* = sfbp$$bt_463_free_color - Free a color =
* ===========================================================================
*
* OVERVIEW:
*       Free a color entry
* 
* FORM OF CALL:
*       sfbp$$bt_463_free_color (scia_ptr,data_ptr,color)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*       color           - entry index pointer 
*
* FUNCTIONAL DESCRIPTION:
*       Free a previously allocated color map entry.
*
--*/
int     sfbp$$bt_463_free_color (SCIA *scia_ptr,SFBP *data_ptr,int color)

{
   int lw,bit;

lw  = color/MAXCOLORS;
bit = 1<<(color%32);
data_ptr->sfbp$l_color[lw] &= ~bit;

return (SS$K_SUCCESS);
}

#endif

/*+
* ===========================================================================
* = sfbp$$463_signature_sync - Sync with end of frame =
* ===========================================================================
*
* FUNCTIONAL DESCRIPTION:
*       Wait for End of frame. The SAR registers can only be accessed 
*       during the BLANK interval. We will wait for the first interrupt
*       and then the second to make sure we are really at EOF. And then
*       we load the signature registers with a seed, wait for the next
*       end of frame and then read the signature registers. Since the
*       SFB+ asic returns ready immediately on interrupt status, we must
*       wait for chip idle after the interrupt status register is clear.
*       Actually the ramdac spec says that it is safe to access SAR 
*       registers 15 pixel clock periods after BLANK. At 130 Mhz, this
*       would be about 115 nanoseconds or 4 instructions. This should
*       not be a problem at all.
*
--*/
int sfbp$$463_signature_sync (SCIA *scia_ptr, SFBP *data_ptr,int pixel,int input )

{
int       i,j,set,busy,sig[6];
volatile  register  char *c,*reg,*bt;
volatile  register  int *cptr,*setup,*face;
volatile  register *csrptr;
volatile  register *isrptr;
 
  /* We assume that when we come in here that    */
  /* Chip is idle and a frame buffer is valid    */

  /* This routine is optimized for speed and     */
  /* does not call generic routines for I/O      */
  
  /* Load Shift Address to point to last row     */
  /* Of Visible Screen                           */

  /* Get some of the initial pointers set up     */
  c  = (char *)control;
  c  += (sfbp$k_command_status);
  csrptr  = (int *)c;

  c  = (char *)control;
  c  += (sfbp$k_int_status);
  isrptr  = (int *)c;

  reg   = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
  reg  += (sfbp$k_ramdac_setup );
  setup = (int *)reg;

  bt   = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
  face = (int *)bt;

  /* Do test Register First             */
  /* Since Seed Can be Done After That  */

  pci_outmem (setup,sfbp$k_bt_ctl_address_lo_write ,PCI_long_mask);
  wbflush();
  pci_outmem (face,(sfbp$k_bt_ctl_address_lo_write<<8)|0xd,PCI_long_mask);
  wbflush();
  pci_outmem (setup,sfbp$k_bt_ctl_address_hi_write ,PCI_long_mask);
  wbflush();
  pci_outmem (face,(sfbp$k_bt_ctl_address_hi_write<<8)|0x2,PCI_long_mask);
  wbflush();
  pci_outmem (setup,sfbp$k_bt_ctl_register_write ,PCI_long_mask);
  wbflush();
  pci_outmem (face,(sfbp$k_bt_ctl_register_write<<8)|pixel,PCI_long_mask);
  wbflush();

  /* Wait for retrace, cancel pending first	*/
  for (i=0;i<2;i++)
  {
   for (j=set=0;!set && j<MSWAIT ;j++)
   	set = pci_inmem(isrptr,PCI_long_mask)&1;
   pci_outmem (isrptr,0x1f,PCI_long_mask);
   wbflush ();
   for (busy=1;busy;)
	busy = pci_inmem(csrptr,PCI_long_mask)&1;
  }

  /* Write seed value to input and output 	*/
  /* ASIC already setup for register writes	*/
  for (i=0;i<6;i++)
	{
	pci_outmem (face,(sfbp$k_bt_ctl_register_write<<8)|0xff,PCI_long_mask);
	wbflush ();
	}

  for (busy=1;busy;)
	busy = pci_inmem(csrptr,PCI_long_mask)&1;

  pci_outmem (setup,sfbp$k_bt_ctl_address_lo_write ,PCI_long_mask);
  wbflush();
  pci_outmem (face,(sfbp$k_bt_ctl_address_lo_write<<8)|0xe,PCI_long_mask);
  wbflush();
  pci_outmem (setup,sfbp$k_bt_ctl_address_hi_write ,PCI_long_mask);
  wbflush();
  pci_outmem (face,(sfbp$k_bt_ctl_address_hi_write<<8)|0x2,PCI_long_mask);
  wbflush();
  pci_outmem (setup,sfbp$k_bt_ctl_register_read,PCI_long_mask);
  wbflush();

  for (j=set=0;!set && j<MSWAIT ;j++)
   	set = pci_inmem(isrptr,PCI_long_mask)&1;

  /* And then read the data           */
  for (i=0;i<6;i++)
	sig[i] = pci_inmem (face,PCI_long_mask);

  for (i=0;i<6;i++)
	sig[i] = (sig[i]>>Dac0ReadData)&0xff;
 
  if (input==InPutSignature)
   {
   data_ptr->sfbp$r_snap.sfbp$r_sig[0].pixel[pixel]= sig[0];
   data_ptr->sfbp$r_snap.sfbp$r_sig[1].pixel[pixel]= sig[1];
   data_ptr->sfbp$r_snap.sfbp$r_sig[2].pixel[pixel]= sig[2];
   }
  else
   {
   data_ptr->sfbp$r_snap.sfbp$r_sig[0].pixel[pixel]= sig[3];
   data_ptr->sfbp$r_snap.sfbp$r_sig[1].pixel[pixel]= sig[4];
   data_ptr->sfbp$r_snap.sfbp$r_sig[2].pixel[pixel]= sig[5];
   }
      
return (1);
}



#if FULL_TGA
/*+
 * ===================================================
 * = sfbp$$bt_463_write_ovl_color - Write BT463 Overlay Color Palette =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Write Overlay Color Palette RAM in BT463
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_write_ovl_color (scia_ptr,address,color)
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address  - relative address of color palette ram location (0 to 256).
 *	color    - rgb array
 *
 * FUNCTIONAL DESCRIPTION:
 *      Load overlay color register with specified rgb value
 *
--*/

int	sfbp$$bt_463_write_ovl_color (SCIA *scia_ptr,int address,char color [])

{
register   char	 *c;
register int i;

sfbp$$bt_463_load_address (scia_ptr,address);		

/* Write  the control setup first       */
c  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
c += (sfbp$k_ramdac_setup );
sfbp$$bus_write (c,sfbp$k_bt_ctl_register_write ,LW);
wbflush();

/* And then write the data              */
c  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;

for (i=0;i<3;i++)
 {
 sfbp$$bus_write (c,(sfbp$k_bt_ctl_register_write<<8)|color[i],LW);
 wbflush();
 }

return (SS$K_SUCCESS);
}



/*+
 * ===================================================
 *  Read data from BT463 register =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Read data from BT463 register
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_read_reg (scia_ptr,address);
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address	 - (int *)BT463 register
 *
 * FUNCTIONAL DESCRIPTION:
 *	This will load the address register with the BT463 register.
 *	It will then write the data to the register.
 *
 *
--*/
int	sfbp$$bt_463_read_reg (SCIA *scia_ptr,int address )

{
char     data;
register   char	 *reg;

sfbp$$bt_463_load_address (scia_ptr,address);

/* Write  the control setup first       */
reg  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
reg += (sfbp$k_ramdac_setup );
sfbp$$bus_write (reg,sfbp$k_bt_ctl_register_read ,LW);
wbflush();

/* And then write the data              */
reg  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
data = (sfbp$$bus_read (reg)>>Dac0ReadData)&0xff;

return (data);
}



/*+
 * ===================================================
 * = sfbp$$bt_463_set_test_reg - Set up test register =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *     set up test register
 *  
 * FORM OF CALL:
 *  	sfbp$$bt_463_set_test_reg (scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr	- scia location pointer
 *	data_ptr	- driver data area pointer
 *
--*/
int	sfbp$$bt_463_set_test_reg (SCIA *scia_ptr,SFBP *data_ptr)

{

sfbp$$bt_463_write_reg(scia_ptr,bt463$k_test_reg,0);

return (SS$K_SUCCESS);
}



/*+
 * ===================================================
 * = sfbp$$bt_463_read_ovl_color - Read BT463 overlay Color Palette =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Read overlay Color Palette RAM in BT463
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_read_ovl_color (scia_ptr,address,color)
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address  - address of color palette ram location.
 *	color    - red,green and blue array pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *      Read the overlay color registers. These are 24 bit registers.
 *
--*/

int	sfbp$$bt_463_read_ovl_color (SCIA *scia_ptr,int address,char color[])

{
register   char	*r,*g,*b;

/* This would only used if overlay ram was available in         */
/* ramdac. In our case, overlay ram is in video ram , so we     */
/* really do not need this                                      */

sfbp$$bt_463_load_address (scia_ptr,address);		

/* Write  the control setup first       */
r  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
r += (sfbp$k_ramdac_setup );
sfbp$$bus_write (r,sfbp$k_bt_ctl_register_read ,LW);
wbflush();

/* And then write the data              */
r  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
g = b = r;

color[0] = (sfbp$$bus_read (r)>>Dac0ReadData)&0xff;
color[1] = (sfbp$$bus_read (g)>>Dac0ReadData)&0xff;
color[2] = (sfbp$$bus_read (b)>>Dac0ReadData)&0xff;

return (SS$K_SUCCESS);
}



/*+
 * ===================================================
 * = sfbp$$bt_463_read_lo - Read data from BT463 Address Lo =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Read data from BT463 register Address Lo
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_read_lo (scia_ptr,data )
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	data	 - (int *)pointer to store register value
 *
--*/
int	sfbp$$bt_463_read_lo (SCIA *scia_ptr)

{
register int    data;
register char	 *reg;

/* Write  the control setup first       */
reg  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
reg += (sfbp$k_ramdac_setup );
sfbp$$bus_write (reg,sfbp$k_bt_ctl_address_lo_read ,LW);
wbflush ();

reg  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
data     = (sfbp$$bus_read(reg)>>Dac0ReadData)&0xff;

return (data);
}


/*+
 * ===================================================
 * = sfbp$$bt_463_read_hi - Read data from BT463 Address hi =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Read data from BT463 register Address Hi 
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_read_hi (scia_ptr,data );
 *
--*/
int	sfbp$$bt_463_read_hi(SCIA *scia_ptr)

{
register          int     temp;
register   char	 *reg;

/* Write  the control setup first       */
reg  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
reg += (sfbp$k_ramdac_setup );
sfbp$$bus_write (reg,sfbp$k_bt_ctl_address_hi_read,LW);
wbflush ();

/* And then write the data              */
reg  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
temp = (sfbp$$bus_read(reg)>>Dac0ReadData)&0xff;

return (temp);
}


/*+
* ===========================================================================
* = sfbp$$bt_463_load_888_color - Load Default Color Map =
* ===========================================================================
*
* OVERVIEW:
*       Load the default Color Map Entry 
* 
* FORM OF CALL:
*       sfbp$$bt_463_load_888_color (scia_ptr,data_ptr,color)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*       
--*/
int     sfbp$$bt_463_load_888_color (SCIA *scia_ptr,SFBP *data_ptr,int color[])

{
int     i,s=1,color_index;
char    entry_color[3],read_color[3];

for (i=0;i<256;i++)
    {
    entry_color[0] = color[0] ? i&0xff : 0;
    entry_color[1] = color[1] ? i&0xff : 0;
    entry_color[2] = color[2] ? i&0xff : 0;
    (*bt_func_ptr->pv_bt_write_color)(scia_ptr,i,entry_color);
    }  

return (s);
}


/*+
 * ===================================================
 * = sfbp$$bt_463_read_cur_color - Read BT463 Cursor Color Palette =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Read Cursor Color Palette RAM in BT463
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_read_cur_color (scia_ptr,address,color)
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address  - address of color palette ram location.
 *	color    - red,green and blue array pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *      Read the cursor color registers. These are 24 bit registers.
 *
 *
--*/

int	sfbp$$bt_463_read_cur_color (SCIA *scia_ptr,int address,char color[] )

{
register   char	*r;

sfbp$$bt_463_load_address (scia_ptr,address);		

/* Write  the control setup first       */
r  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
r += (sfbp$k_ramdac_setup );
sfbp$$bus_write (r,sfbp$k_bt_ctl_register_read ,LW);
wbflush ();

/* And then write the data              */
r  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;

color[0] = (sfbp$$bus_read (r)>>Dac0ReadData)&0xff;
color[1] = (sfbp$$bus_read (r)>>Dac0ReadData)&0xff;
color[2] = (sfbp$$bus_read (r)>>Dac0ReadData)&0xff;

return (SS$K_SUCCESS);
}


/*+
 * ===================================================
 * = sfbp$$bt_463_read_color - Read BT463 Color Palette =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Read Color Palette RAM in BT463
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_463_read_color (scia_ptr,address,color)
 *
 * RETURNS:
 *	none
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address  - address of color palette ram location.
 *	color    - red,green and blue array pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	The color palette RAM is 512 x 24 bits deep and represents
 *	512 possible colors that the BT463 can produce on the video 
 *	monitor. Each entry is 24 bits wide, 8 bits for RED,8 bits for
 *	GREEN and 8 Bits for Blue. To write to color palette RAM,
 *	all we need to do is write to location 0000-00FF with C1:C0 set
 *	to 11, which is done by the sfbp decode logic. The address register
 *	is loaded with the color palette ram location. The BT463 then expects
 *	three successive write cycles 8 bits RED, 8 Bits GREEN and then 8 bits
 *	of BLUE. The address register is automatically incremented to the 
 *	next color palette location following the blue write cycle. A color is
 *	maximum intensity when set to FF and minimum intensity (black) when set
 *	to 0.
 *
--*/
int	sfbp$$bt_463_read_color (SCIA *scia_ptr,int address,char color[])

{
register char	*r,*g,*b;

sfbp$$bt_463_load_address (scia_ptr,address);		

/* Write  the control setup first       */
r  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
r += (sfbp$k_ramdac_setup );
sfbp$$bus_write (r,sfbp$k_bt_ctl_color_read ,LW);
wbflush ();

/* And then write the data              */
g = b = r  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;

color[0] = (sfbp$$bus_read (r)>>Dac0ReadData)&0xff;
color[1] = (sfbp$$bus_read (g)>>Dac0ReadData)&0xff;
color[2] = (sfbp$$bus_read (b)>>Dac0ReadData)&0xff;

return (SS$K_SUCCESS);
}




/*+
* ===========================================================================
* = sfbp$$bt_463_dma_checksum - Verify the data =
* ===========================================================================
*
* FUNCTIONAL DESCRIPTION:
*       DMA write the screen of information and then save it. I am just trying
*       to verify the screen contents and that is all. I will have to rely on
*       the signature testing for the ramdac later.
*
--*/
int sfbp$$bt_463_dma_checksum (SCIA *scia_ptr, SFBP *data_ptr )

{
register  int  src=0,i,j=0,s=1;
volatile  register  int *vsrc,*BufferPtr,*EndBufferPtr;
register  int bytes,EndAddress,SourceVisual,DestVisual;
register  int DmaBufferPtr,XorCheckSum,Data;
int       WriteMask,WriteWord,LwCount,ByteLane0=0,ByteLane1=0,ByteLane2=0,ByteLane3=0;

 bytes          = (1284*1024*data_ptr->sfbp$l_bytes_per_pixel),
 vsrc           = (int *)sfbp_ioseg_ptr->sfbp$a_vram_mem_ptr;
 vsrc          += (data_ptr->sfbp$l_console_offset/4);
 src		= data_ptr->sfbp$l_console_offset;
 SourceVisual   = data_ptr->sfbp$l_module == hx_8_plane ? SourceVisualPacked: SourceVisualTrue;
 DestVisual     = data_ptr->sfbp$l_module == hx_8_plane ? DestVisualPacked  : DestVisualTrue;
 EndAddress     = bytes;
 WriteWord      = sfbp$k_dma_write_copy|(SourceVisual<<8);
 WriteMask	= ((DMA_BYTES/8)<<16)|0xffff;

 DmaBufferPtr   = (int )malloc (SIZE_DMA_BUFFER);
 EndBufferPtr   = (int *)DmaBufferPtr;
 LwCount	= DMA_BYTES/4;
 EndBufferPtr  += LwCount;
 XorCheckSum    = 0;
 
 do  
 {
   BufferPtr      = (int *)DmaBufferPtr;
   for (j=0;j<LwCount;j++,vsrc++)
	*BufferPtr++ = pci_inmem (vsrc,PCI_long_mask);

   BufferPtr   = (int *)DmaBufferPtr;
    do 
      { 
       Data         = *BufferPtr;
       ByteLane0   += (Data&0x000000ff);
       ByteLane1   += (Data&0x0000ff00);
       ByteLane2   += (Data&0x00ff0000);
       ByteLane3   += (Data&0xff000000);
       XorCheckSum  = ((ByteLane0 & 0x000000ff) |
                       (ByteLane1 & 0x0000ff00) |
                       (ByteLane2 & 0x00ff0000) |
                       (ByteLane3 & 0xff000000));
      } while (++BufferPtr < EndBufferPtr);

   src += DMA_BYTES;
  } while (src<EndAddress );  

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_shift,0,LW);

 free(DmaBufferPtr);

 return (XorCheckSum);
}
#endif
