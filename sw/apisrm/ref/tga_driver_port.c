/*
 * COPYRIGHT (c) 1991,1990 BY
 * DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.
 * ALL RIGHTS RESERVED.
 *                                     
 * THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED
 * ONLY  IN  ACCORDANCE  OF  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE
 * INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER
 * COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY
 * OTHER PERSON.  NO TITLE TO AND  OWNERSHIP OF THE  SOFTWARE IS  HEREBY
 * TRANSFERRED.
 * 
 * THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE
 * AND  SHOULD  NOT  BE  CONSTRUED  AS A COMMITMENT BY DIGITAL EQUIPMENT
 * CORPORATION.
 * 
 * DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS
 * SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.
 */

/*
 *++
 *  FACILITY:
 *
 *      sfbp console port driver 
 *
 *  ABSTRACT:
 *
 *      This module builds the sfbp port driver function table.
 *
 *  AUTHORS:
 *                     
 *      James Peacock 
 *
 *  CREATION DATE:      01-28-91
 *
 *  MODIFICATION HISTORY:
 *
 *      ska	11-Nov-1998	Conditionalize for Yukona
 *
 *	jje	03-Apr-1996	Conditionalize for the Cortex
 *
 *	rbb	13-Mar-1995	Conditionalize for the EB164
 *
 *--
*/


/*
**
**  INCLUDE FILES
**
**/
#include        "cp$src:display.h"
#include   	"cp$src:platform.h"
#include	"cp$src:ansi_def.h"
#include	"cp$src:alpha_arc.h"
#include	"cp$src:chstate.h"
#include 	"cp$src:tga_sfbp_callbacks.h"
#include 	"cp$src:tga_sfbp_prom_defines.h"
#include 	"cp$src:tga_sfbp_def.h"
#include 	"cp$src:tga_sfbp_protos.h"
#include 	"cp$src:tga_sfbp_463.h"
#include 	"cp$src:tga_sfbp_485.h"
#include 	"cp$src:tga_sfbp_ext.h"
#include 	"cp$src:tga_sfbp_crb_def.h"
#include 	"cp$src:tga_sfbp_ctb_def.h"
#include	"cp$src:tga_sfbp_driver_crystal.h"
#include        "cp$src:tga_sfbp_pci_def.h"
#include 	"cp$src:kernel_def.h"
#include 	"cp$src:dynamic_def.h"
#include        "cp$src:stddef.h"
#include 	"cp$src:prdef.h"
#include        "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include 	"cp$src:pb_def.h"
#include        "cp$src:eisa.h"   
#include	"cp$inc:platform_io.h"
#include	"cp$src:vgapb.h"

#define column8 1
#define OneRow  1
#define TwoRows 2
#define TGA_VENDOR_ID 0x00041011
/*#define REVERSE_VIDEO 7*/
    
#define   SCROLL_SIZE	      (DMA_BYTES)
#define   SIZE_DMA_BUFFER     (SCROLL_SIZE+64)
#define   LW_SIZE_DMA_BUFFER  (SIZE_DMA_BUFFER/4)
                        
extern    int TGA_mem_base[];
    
#define   PCI_byte_mask 0
#define   PCI_word_mask 1
#define   PCI_long_mask 3

void     pci_outmem(unsigned __int64 address, unsigned int value, int length);
unsigned int pci_inmem(unsigned __int64 address, int length);

extern   int ScrollDmaPtr[];
                          
/* Set to 1 to force module type to 24	*/

#if SABLE
#define WindowBase io_get_window_base_rt(TGA_pb[data_ptr->sfbp$l_slot_number])
#else
#define WindowBase io_get_window_base(TGA_pb[data_ptr->sfbp$l_slot_number])
#endif

#define PROMPT() {if (data_ptr->sfbp$l_alt_console){printf("Continue ? ");getchar();printf("\n");}else getchar();} 
#define REPEAT(index,count) for( index=0;index<count;index++)
#define CLEAR(i,var,size) for(i=0;i<size;i++)var[i]=0;

#define  ConfigPlir 	0x3C
#define  ConfigPvrr	0x40
#define  ConfigPaer	0x44

extern   int TgaVector[];
extern struct CH_STATE *tga_ch_state;
extern   struct   pb *TGA_pb[];

#if BACKING_STORE
extern char *tga_console_buffer;                          
#endif
                        
#define  PciSlot1	11
#define  PciSlot2	12
#define  PciSlot3	13
#define  PciSlot4	14

extern   int tga_int_handler ();                         
                                                         
                                                         
#define PalleteSnoop 0x40
#define TGA_DEBUG 0

int	rtc_read (int offset);
#define TgaToyOffset 0x16
void 	sys_pci_enable_interrupt  (int vector);
void 	sys_pci_disable_interrupt (int vector);

extern  unsigned char char_lookup_table[];



/*+
 * ===================================================
 * = sfbp$init_driver - sfbp init driver function      =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine will initialize the sfbp driver data area,
 *     and return pointers to the functions that it supports. The
 *     init_driver routine is called by the console subsystem
 *     so it can fill in its port function block, required to
 *     do character input and output. This is the major functions
 *     needed by the console.
 *    
 * FORM OF CALL:
 *  
 *     sfbp$init_driver(sfbp_ioseg_ptr,
 *                     data_ptr,
 *                     cp_func_ptr_desc,
 *                     scia_ptr)
 *
 * ARGUMENTS:
 *      sfbp_ioseg_ptr 	  - pointer to the sfbp iosegment table
 *      data_ptr 	  - pointer to the sfbp driver data area 
 *	cp_func_ptr_desc  - pointer to the Console function block
 *	scia_ptr          - pointer to the scia 
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *     This routine will initialize the sfbp driver data area,
 *     and return pointers to the functions that it supports. 
 * 
 * ALGORITHM:
 *      - Get the address of port driver function block.
 *      - Fill in the port driver functions.
 *
--*/
int	sfbp$init_driver( struct sfbp_ioseg *sfbp_ioseg_ptr,SFBP *data_ptr,
                        struct function_block_desc *cp_func_ptr_desc,SCIA *scia_ptr)

{
register int     s;
register CTB     *ctb_ptr;
register struct  drv_desc        *drv;
register IOSEG	 *ioseg_ptr;

drv  		= (struct drv_desc *)&scia_ptr->gfx_drv_desc;
ioseg_ptr	= (IOSEG *)drv->ioseg_ptr;
drv_desc_ptr    = (struct drv_desc   *)&scia_ptr->gfx_drv_desc;
sfbp_ioseg_ptr  = (IOSEG *)drv_desc_ptr->ioseg_ptr ;
control         = (struct sfbp_control *)ioseg_ptr->sfbp$a_reg_space;
gfx_func_ptr    = (struct gfx_func_block *)&data_ptr->sfbp$r_graphics_func;
bt_func_ptr     = (struct bt_func_block *)&data_ptr->sfbp$r_bt_func;
cp_func_ptr     = (struct cp_func_block *)&scia_ptr->console_func;

cp_func_ptr->pv_putchar 	= sfbp$putchar;
cp_func_ptr->pv_init_output 	= sfbp$init_output;
cp_func_ptr->pv_reset_output 	= sfbp$reset_output;

sfbp$init_gfx_driver (scia_ptr,data_ptr);

return(1);
}
              


/*+
 * ===================================================
 * = sfbp$init_gfx_driver - sfbp GFX init driver function  =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine will initialize the sfbp driver data area,
 *     and return pointers to the functions that it supports. The
 *     init_driver routine is called by the console subsystem
 *     so it can fill in its port function block, required to
 *     do character input and output. This is the major functions
 *     needed by the console.
 *    
 * FORM OF CALL:
 *  
 *     sfbp$init_gfx_driver(scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr          - pointer to the scia 
 *      data_ptr 	  - pointer to the sfbp driver data area 
 *
 * FUNCTIONAL DESCRIPTION:
 *     This routine will initialize the low level graphics functions.
 *
--*/
int	sfbp$init_gfx_driver(SCIA *scia_ptr,SFBP *data_ptr)

{

gfx_func_ptr->pv_init_param         	= sfbp$$init_param ;
gfx_func_ptr->pv_fill_vram          	= sfbp$$fill_video_memory;
gfx_func_ptr->pv_configure_video    	= sfbp$$configure_video;
gfx_func_ptr->pv_sfbp_write_reg     	= sfbp$$sfbp_write_reg;
gfx_func_ptr->pv_sfbp_read_reg      	= sfbp$$sfbp_read_reg;
gfx_func_ptr->pv_sfbp_write_vram    	= sfbp$$sfbp_write_vram;
gfx_func_ptr->pv_sfbp_read_vram     	= sfbp$$sfbp_read_vram;
gfx_func_ptr->pv_sfbp_wait_csr_ready	= sfbp$$sfbp_wait_csr_ready;
gfx_func_ptr->pv_sfbp_stipple       	= sfbp$$sfbp_stipple;
gfx_func_ptr->pv_sfbp_read_pci_config  	= sfbp$$sfbp_read_pci_config;
gfx_func_ptr->pv_sfbp_write_pci_config 	= sfbp$$sfbp_write_pci_config;
gfx_func_ptr->pv_sfbp_read_rom      	= sfbp$$sfbp_read_rom;
gfx_func_ptr->pv_test_int           	= sfbp$$test_int;
gfx_func_ptr->pv_clear_int_req	    	= sfbp$$clear_int_req;
#if FULL_TGA
gfx_func_ptr->pv_disable_interrupt  	= sfbp$$disable_interrupt;
gfx_func_ptr->pv_enable_interrupt   	= sfbp$$enable_interrupt;
gfx_func_ptr->pv_wait_retrace	    	= sfbp$$wait_retrace;
gfx_func_ptr->pv_sfbp_error         	= sfbp$$sfbp_error_handler;
gfx_func_ptr->pv_sfbp_aligned_copy  	= sfbp$$sfbp_aligned_copy;
gfx_func_ptr->pv_sfbp_unaligned_copy	= sfbp$$sfbp_unaligned_copy;
gfx_func_ptr->pv_sfbp_drawline      	= sfbp$$sfbp_draw_line;
gfx_func_ptr->pv_get_expect         	= sfbp$$get_expect;
gfx_func_ptr->pv_sfbp_write_rom     	= sfbp$$sfbp_write_rom;
#endif

return(SS$K_SUCCESS);
}

/*+
 * ===================================================
 * = sfbp$reset_output - reset the output device =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine will reset the console output device.
 *  
 * FORM OF CALL:
 *  
 *     sfbp$reset_output(scia_ptr,data_ptr)
 *  
 * ARGUMENTS:
 *	scia_ptr - pointer to the shared console interface area
 *      data_ptr - driver data area pointer 
 *	 
 * FUNCTIONAL DESCRIPTION:
 *
 *	This routine will reinitialize the driver's data,
 *	clear the screen, and then reset/enable	the video.
 *
--*/

int	sfbp$reset_output(SCIA *scia_ptr,SFBP *data_ptr)


{
register  int offset;

 sfbp$init_output  (scia_ptr,data_ptr);
            
 /* Just Clear Out frame buffer                  */
 /* I do not care about Z buffer area            */
 if (data_ptr->sfbp$l_module==hx_8_plane )                                                     
        sfbp$$fill_video_memory(scia_ptr,data_ptr,(char *)0,data_ptr->sfbp$l_backgroundvalue,data_ptr->sfbp$l_vram_size);      
                                                                                               
 else if (data_ptr->sfbp$l_module==hx_32_plane_no_z)                                           
        sfbp$$fill_video_memory(scia_ptr,data_ptr,(char *)1024,data_ptr->sfbp$l_backgroundvalue,data_ptr->sfbp$l_vram_size-1024);
                                                                                                  
 else sfbp$$fill_video_memory(scia_ptr,data_ptr,(char *)1024,data_ptr->sfbp$l_backgroundvalue,(data_ptr->sfbp$l_vram_size/2)-1024);
          
return(1);
}         
          
         
/*+       
 * ===================================================
 * = sfbp$init_output - initialize the console output device =
 * ===================================================
 *        
 * OVERVIEW:
 *        
 *     This routine will initialize the console output device
 *        
 * FORM OF CALL:
 *        
 *     sfbp$init_output(scia_ptr,data_ptr)
 *        
 *        
 * ARGUMENTS:
 *	scia_ptr - pointer to the shared console interface area
 *      data_ptr - driver data area pointer
 *	  
 * FUNCTIONAL DESCRIPTION:
 *	This routine will initialize the module.
 *        
 * ALGORITHM:
 *      TGA reset if on a TGA module
 *      Configure the video (module type,oscillator type, and connect to ramdac driver)
 *      Initialize Screen Parameters
 *      IF not a console driver                                               
 *        THEN
 *              Initialize asic (all registers including horiz and vertical)
 *              Initialize the ramdac now that we know what kind it is.
 *              Allocate our color map
 *              Initialize our color map
 *             
--*/

int	sfbp$init_output(SCIA *scia_ptr,SFBP *data_ptr)

{
register int    s=1;

/* For the console driver...device already initialized  */
/* So lets just connect up to drivers                   */
/* So our screen does not go boink                      */
/* Console Driver Just to Character Output              */
/* And no other Ramdac Stuff                            */
                                                   
                                                   
  sfbp$$tga_reset (scia_ptr,data_ptr);             
                                                   
  sfbp$$configure_video(scia_ptr,data_ptr);        
                                                   
  sfbp$$init_param (scia_ptr,data_ptr);            
                                                   
  sfbp$$asic_reset (scia_ptr,data_ptr);            
                                                   
  (*bt_func_ptr->pv_bt_init) (scia_ptr,data_ptr);  
                                                   
  (*bt_func_ptr->pv_bt_load_332_color) (scia_ptr,data_ptr);
                                                   
  /* Need to Clear the FG register also          */
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_foreground,0,LW);

return (1);
}



/*+
 * ===================================================
 * = sfbp$$init_param - Initialize the Parameter Block =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine will initialize system parameters
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$init_param (scia_ptr,data_ptr)
 *  
 *
 * ARGUMENTS:
 *	scia_ptr - pointer to the shared console interface area
 *      data_ptr - driver data area pointer 
 *	 
 * FUNCTIONAL DESCRIPTION:
 *      Initialize the system parameters to customize the screen. I use
 *      this to set the max row and column. This sets up the console 
 *      window area. This is really only used for VMS most likely, but has
 *      been included for completeness.
 *
--*/
int	sfbp$$init_param (SCIA *scia_ptr,SFBP *data_ptr)
                                               
                 
{                
data_ptr->sfbp$l_cursor_max_row    = (data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/scia_ptr->font_height);
data_ptr->sfbp$l_cursor_max_column = (data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels/scia_ptr->font_width);
#if SABLE || MIKASA
data_ptr->sfbp$l_cursor_row        = 0;
#else
data_ptr->sfbp$l_cursor_row        = data_ptr->sfbp$l_cursor_max_row-1;
#endif
data_ptr->sfbp$l_cursor_column     = 0;


return (1);
}


/*+
 * ===================================================
 * = sfbp$putchar - the sfbp_putchar routine           =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine will output a string of characters to the
 *     console window of the video screen 
 *    
 * FORM OF CALL:
 *  
 *     sfbp$putchar(scia_ptr,buffer_desc_ptr)
 *
 * RETURNS:
 *	integer = number of characters printed 
 *
 * ARGUMENTS:                                                 
 *	scia_ptr          - pointer to the scia               
 *	buffer_desc_ptr   - pointer to a buffer descriptor for the string of
 *                          characters to print               
 *                                                            
 * FUNCTIONAL DESCRIPTION:                                    
 *                                                            
 *     This routine will output a string of characters to the 
 *     console window of the video screen. This output normal 
 *     characters,cr,lf,tab and . This routine is normally    
 *     called by the console class driver. When this routine is
 *     called from a turbo channel environment by a putc routine,
 *     then character size is only 1 in the descriptor.       
 *                                                            
 * CALLS:                                                     
 *	- sfbp$$display_character()                           
 *      - sfbp$$delete_character()                            
 *      - sfbp$$tab_character()                               
 *      - sfbp$$cr_return()                                   
 *      - sfbp$$line_feed()                                   
 *                                                            
 * ALGORITHM:                                                 
 *      - all input parameters are checked                    
 *      - check to make sure that the cursor has been allocated..
 *      - while there are characters to output                
 *         - choose between the usfont or mcsfont ptr         
 *         - if a displayable character call 
 *                - sfbp$$display_character()
 *         - else if a tab   
 *               - sfbp$$tab_character
 *         - else if a backspace or delete call 
 *               - sfbp$$delete_character
 *         - else if a carriage return
 *               - sfbp$$cr_return()
 *         - else if a line feed
 *               - sfbp$$line_feed()                     
 *                                      
--*/  
sfbp$putchar  (struct CH_STATE *chs,char ch,unsigned char attribute,
    							int xpos,int ypos)  
{   
register int   mcs=0,i;
register char *font_ptr;
register char  out_char;
unsigned int   char_index = (unsigned int)ch;
    
#if BACKING_STORE
  if (ch)
      tga_console_buffer [xpos + ypos * data_ptr->sfbp$l_cursor_max_column] = ch;
#endif

  char_index &= 0xff;

  data_ptr->sfbp$l_cursor_column = xpos;    
  data_ptr->sfbp$l_cursor_row = ypos;       
  font_ptr    =  scia_ptr->usfont_table_ptr;
  (*bt_func_ptr->pv_bt_pos_cursor)(scia_ptr,data_ptr,
                data_ptr->sfbp$l_cursor_row,
                data_ptr->sfbp$l_cursor_column,FALSE);
              
  if (char_index > 128) 
      {                                            
      char_index  =  char_lookup_table[char_index - 128];  
      mcs  	  = 1;                             
      }

  out_char    =  char_index;		                                        
  out_char   &=  127;                                                           
                                                                                
  if ((out_char>=' '&& out_char <= '~') || mcs )	                        
    sfbp$$display_a_character
       (scia_ptr, char_index, font_ptr, NoEraseChar, attribute);        
#if FULL_TGA
  else switch (out_char)		                        
   {                                        
   case sfbp$k_delete	      :             
   case sfbp$k_backspace      : sfbp$$delete_character (scia_ptr,data_ptr);break;
   case sfbp$k_tab            : sfbp$$tab_character    (scia_ptr,data_ptr);break;
   case sfbp$k_carriage_return: sfbp$$move_cursor (scia_ptr,data_ptr,sfbp$k_begin_line);break;
   case sfbp$k_line_feed      : sfbp$$line_feed   (scia_ptr,data_ptr);break;
   } 					                        
#endif

}



/*+
 * ===================================================
 * = sfbp$$display_character - dispays a character to the screen =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *    This routine will display one character to the screen. 
 *
 * FORM OF CALL:
 *  
 *     sfbp$$display_character(scia_ptr,
 *                            char_index,
 *                            font_ptr);
 *
 * ARGUMENTS:
 *	scia_ptr   - pointer to the scia
 *	char_index - index of character to display
 *      font_ptr   - pointer to the font table 	 
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *   This routine displays one character to the screen and then
 *   updates the console cursor position.
 *
 * CALLS:
 *
 *      sfbp$$display_a_character()
 *	sfbp$$move_cursor()
 *	sfbp$$scroll()
 *
 *
 * ALGORITHM:
 *
 *     - call sfbp_display_one_character
 *     - if cursor at end of row 
 *         - if cursor at end of screen size
 *             - call sfbp_scroll 
 *             - move cursor to beginning of current line
 *          else
 *             - move cursor to beginning of next line 
 *       else 
 *         - move_cursor forward on cell
 *            
--*/          
int	sfbp$$display_character(SCIA *scia_ptr,int char_index,char *font_ptr)
{           
                        
 sfbp$$display_a_character
   (scia_ptr,char_index,font_ptr,NoEraseChar, 0 /*attribute*/);
#if FULL_TGA
 if (data_ptr->sfbp$l_cursor_column == (data_ptr->sfbp$l_cursor_max_column - 1) )
   {                        
    if (data_ptr->sfbp$l_cursor_row == data_ptr->sfbp$l_cursor_max_row - 1)
     {                      
      sfbp$$scroll(scia_ptr,data_ptr);
      sfbp$$move_cursor(scia_ptr,data_ptr,sfbp$k_begin_line);
     } else sfbp$$move_cursor(scia_ptr,data_ptr,sfbp$k_begin_next_line);	
   }else sfbp$$move_cursor(scia_ptr,data_ptr,sfbp$k_forward);			
#endif
                            
return(1);                        
}                           
                            
                           
/*+                         
 * ===================================================
 * = sfbp$$display_a_character - dispays one character to the screen =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *    This routine will calculate the position of the next
 *    character on the screen and the address of the character
 *    to copy. 
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$display_a_character(scia_ptr,char_index,font_ptr,erase);
 *
 * ARGUMENTS:
 *	scia_ptr - pointer to the scia
 *	char_index - the index into the font table
 *      font_ptr - pointer to the font table
 *      erase    - this is true for erase (backspace ops)
 *
 * FUNCTIONAL DESCRIPTION:
 *      This routine uses opaque stipple mode to stipple a character 
 *      to the screen. We do 1 scanline at a time for the height of
 *      the character. Each scanline is 12 pixels wide(big font). 
 *      We load the background register with 0xffffffff so that pixels
 *      that should be on are filled with the pixel index. We then
 *      set the pixel mask to the value read from the font table. 
 *      However, since this is opaque stipple mode (<32 pixels), we
 *      have to compliment that value.  Note, I use the persistent  
 *      pixel mask register so I do not have to write that each time.
 *                                                                  
--*/                                                                
int	
sfbp$$display_a_character(SCIA *scia_ptr, int char_index,
                  char *font_ptr,int erase, int attribute)
{                                                                                                            
register int   EndScanLine,NumberScanLines,WriteMask,lws,*vptr;                                              
register char  *byte_cell_font_ptr;                                                                          
register char  *current_ptr;                                                                                 
register int   i,j,bits,BitsTemp,BitMask,AddressMask;                                                        
                                                                                                             
byte_cell_font_ptr =  font_ptr+ data_ptr->sfbp$l_cell_byte_offset * char_index;                              
current_ptr        =  calc$$character_position(scia_ptr,data_ptr);                                           
vptr               =  (int *)current_ptr;                                                                    
lws                =  data_ptr->sfbp$l_bytes_per_scanline/4;                                                 
NumberScanLines    =  scia_ptr->font_height;                                                                 
EndScanLine        =  (int)current_ptr + (data_ptr->sfbp$l_bytes_per_scanline*NumberScanLines);              
                                                                                                             
(*gfx_func_ptr->pv_sfbp_write_reg)
    (scia_ptr,sfbp$k_foreground,data_ptr->sfbp$l_backgroundvalue,LW);                   
        
/*attribute = REVERSE_VIDEO ;*/
if (data_ptr->sfbp$l_planes==8)                                                                     
   {                                                                                                
   if (attribute==REVERSE_VIDEO) {

     (*gfx_func_ptr->pv_sfbp_write_reg)
           (scia_ptr,sfbp$k_foreground,data_ptr->sfbp$l_foregroundvalue,LW);                   
     (*gfx_func_ptr->pv_sfbp_write_reg)                     
           (scia_ptr,sfbp$k_background,data_ptr->sfbp$l_backgroundvalue,LW);      
   }                                                        
   else if (erase==EraseChar)                                                                            
        (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_background,0,LW);                        
   else                                                                                             
     (*gfx_func_ptr->pv_sfbp_write_reg)
           (scia_ptr,sfbp$k_background,data_ptr->sfbp$l_foregroundvalue,LW);      
     (*gfx_func_ptr->pv_sfbp_write_reg)
           (scia_ptr,sfbp$k_mode,sfbp$k_opaque_stipple,LW);         
     AddressMask = 0xfffffffc;                                                                  
     BitMask     =  (int)vptr % 4;                                                              
   }                                                                                          
else                                                                                          
   {
   if (attribute==REVERSE_VIDEO) {

     (*gfx_func_ptr->pv_sfbp_write_reg)
           (scia_ptr,sfbp$k_foreground,data_ptr->sfbp$l_foregroundvalue,LW);                   
     (*gfx_func_ptr->pv_sfbp_write_reg)                     
           (scia_ptr,sfbp$k_background,data_ptr->sfbp$l_backgroundvalue,LW);      
   }
   else if (erase==EraseChar)                                                                      
        (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_background,data_ptr->sfbp$l_foregroundvalue,LW); 
   else (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_background,                  
          (data_ptr->sfbp$l_foregroundvalue & 0x00ffffff),LW);                            
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,sfbp$k_opaque_stipple|(source_24_dest<<8),LW);
   AddressMask = 0xfffffff0;                                                            
   BitMask     =  (int)vptr % 16;                                                       
   BitMask    /=  4;                                                                    
   }                                                                                    
                                                                                        
 WriteMask    = 0x1ff;                                                                  
 WriteMask  <<= BitMask;                                                                
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_persistent_pmask,WriteMask,LW);     
                                                                                        
 do                                                                                     
    {                                                                                   
    bits = (int)(*byte_cell_font_ptr++);                                        
    /* VGA Table Bits are in big endian */                              
    /* Have to change to little endian  */                              
    for (i=0,j=7,BitsTemp=0;i<8;i++,j--)                                
    	BitsTemp |= (((bits&(128>>i))>>j)<<i);                          
    bits = BitsTemp;                                                    
                                                                        
    /* Pattern Must be aligned 4 pixels */                              
    bits <<= BitMask;                                                   
                                                                        
    /* compliment for opaque stipple	*/
    bits = (~bits);

    /* Write Address			*/
    (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,(int)vptr & AddressMask,bits,LW);

    vptr  += lws;
    } while ((int)vptr<EndScanLine);

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 return(1);
}           


/*+
 * ===================================================
 * = calc$$screen_bytes - Calculate screen bytes = 
 * ===================================================
 *   
 * OVERVIEW:
 *     Calculate bytes per scanline,row and column.
 *  
 * FORM OF CALL:
 *     calc$$screen_bytes (scia_ptr,data_ptr)
 *
 * FUNCTIONAL DESCRIPTION:
 *	Compute bytes per scanline,row and column. This calculation is
 *	dependent on the number of planes, character font height and
 *	width. This routine is called once during system video configuration.
 *      The end parameters are then used in character positioning calculations
 *      when the port is called.
 *
--*/
int calc$$screen_bytes ( SCIA *scia_ptr, SFBP *data_ptr )

{

data_ptr->sfbp$l_cursor_max_row       = (data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/scia_ptr->font_height); 
data_ptr->sfbp$l_cursor_max_column    = (data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels/scia_ptr->font_width);
                                            
data_ptr->sfbp$l_bytes_per_scanline   = (data_ptr->sfbp$l_planes*data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels)/8;

data_ptr->sfbp$l_bytes_per_row        = (data_ptr->sfbp$l_bytes_per_scanline * (scia_ptr->font_height));
data_ptr->sfbp$l_bytes_per_column     = (data_ptr->sfbp$l_planes*(scia_ptr->font_width))/8;
data_ptr->sfbp$l_bytes_per_pixel      =  data_ptr->sfbp$l_planes==8 ? 1 : 4;


return (SS$K_SUCCESS);                                                       
}

/*+
 * ===================================================
 * = calc$$character_position - Compute character position =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Compute the current character position.
 *  
 * FORM OF CALL:
 *  
 *     calc$$character_position  ( scia_ptr, data_ptr )
 *
 * ARGUMENTS:
 *	scia_ptr	- shared driver interface
 *	data_ptr	- data area pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	Compute the offset into the video ram to update the character. 
 *	The character position is displayed starting at the first scanline
 *	for the character. The character size is determined by the font
 *	size, which is located in the driver data area structure.
 *
 * ALGORITHM:
 *	- Get the current console offset into video ram
 *	- Add to offset (current cursor row * bytes per row )
 *	- Add to offset (current column * bytes per column )
 *	- Add to offset the start of video ram
--*/

char  *calc$$character_position ( SCIA *scia_ptr, SFBP *data_ptr)

{
char  *vram;

/* The ATI font is 9x16 	*/

vram  = (char *)((data_ptr->sfbp$l_cursor_row * data_ptr->sfbp$l_bytes_per_row ) +
	(data_ptr->sfbp$l_cursor_column * data_ptr->sfbp$l_bytes_per_column));

/* Refresh Address Is at 4096   */
vram += (data_ptr->sfbp$l_console_offset);

return (vram);
}

                                                  

/*+
 * ===================================================
 * = calc$$cell_font_size - Calculate the font size =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Compute the offset into the font table for a scanline.
 *  
 * FORM OF CALL:
 *
 * 	calc$$cell_font_size (scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr	- SCIA pointer
 *	data_ptr	- Driver data area pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	This routine is used to compute the offset of character into font table.
 *	This results in an offset to the font table with the pixel illumination 
 *	data. This routine is called once during video configuration.
 *
 * ALGORITHM:
 *	- Compute the number of pixels wide character.
 *	- Compute the vertical offset. What is different height between characters
--*/

int	calc$$cell_font_size ( SCIA *scia_ptr, SFBP *data_ptr )

                      
{                     
/* The ATI font table is 32 bytes of which we use only the */
data_ptr->sfbp$l_cell_byte_offset  =  32;

return (1);
}


#if FULL_TGA
/*+
 * ===================================================
 * = sfbp$$scroll - scroll the display up by one line =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *    This routine is called to scroll the display up by one
 *    line. The entire display region is moved 22 scanlines lines up 
 *    for the scroll operation. The line that used to be on top
 *    is lost and the bottom line is cleared. This routine is called
 *    by display_character prior to displaying the current scanline,
 *    so we just push the old and bring in the new.
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$scroll(scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *
 *	scia_ptr        - pointer to the scia
 *      data_ptr        - driver data pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	We have to move the contents of video RAM from one location to another
 *	for the sole purpose of moving everything up 1 row of text. So, what we
 *	will do is copy 1 row at a time for the maximum number of rows in our 
 *	current console window.
 *              When using the sfbp for scrolling, it would be faster to just
 *      copy everythinbg from second row to end of screen to the first row, thus
 *      scrolling faster.
 *
 *
 * CALLS:
 *	sfbp$copy_video_memory - copy sections of video memory 
 *
 * ALGORITHM:
 *	- Get the pointer to the first row of video ram.
 *	- Get the pointer to the second row of video ram.
 *	- Repeat for maximum number of rows-1 
 *		copy next row to previous row .
 *	- End Repeat
 *	- Clear the Last row to make room for new line of text
 *
--*/
int	sfbp$$scroll ( SCIA *scia_ptr, SFBP *data_ptr )
           
{
register int num_bytes;
register char *last_row_ptr,*first_row_ptr,*second_row_ptr;

 {
 num_bytes     = (data_ptr->sfbp$l_cursor_max_row-1)*data_ptr->sfbp$l_bytes_per_row;
 first_row_ptr = (char *)data_ptr->sfbp$l_console_offset;
 second_row_ptr= (char *)first_row_ptr + data_ptr->sfbp$l_bytes_per_row;
 }     
                                              
 sfbp$$copy_video_memory (scia_ptr,data_ptr,second_row_ptr,first_row_ptr,num_bytes);
                                    
 /* When Scrolling we need to clear the extra at bottom of screen        */
                                    
 sfbp$$zero_line (scia_ptr,data_ptr,TwoRows);

return(SS$K_SUCCESS);
}           

                            

/*+
 * ===================================================
 * = sfbp$$zero_line - scroll the display up by one line =
 * ===================================================
 *
 * OVERVIEW:
 *      Zero N character rows.
 *  
--*/    
int	sfbp$$zero_line ( SCIA *scia_ptr, SFBP *data_ptr , int NumRows )

{

register char *current_ptr;
           
current_ptr   = (char *)(data_ptr->sfbp$l_console_offset +
                        (data_ptr->sfbp$l_cursor_row * data_ptr->sfbp$l_bytes_per_row));
           
sfbp$$fill_video_memory(scia_ptr,data_ptr,current_ptr,
            data_ptr->sfbp$l_backgroundvalue,data_ptr->sfbp$l_bytes_per_row * NumRows );
 
return (1);
}
#endif

/*+
 * ===================================================
 * = sfbp$$copy_video_memory - Copy Video Memory =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This is used to copy video ram .
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$copy_video_memory (scia_ptr,data_ptr,source,pattern,size)
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	data_ptr - driver data area pointer
 *	source	 - source pointer (copy from here )
 *	dest     - Destination Pointer (copy to here )
 *	size	 - This many bytes 
 *
 * FUNCTIONAL DESCRIPTION:
 *      This routine will set up to do an aligned copy of
 *      video ram. Console Driver will use copy64 for 
 *      maximum speed. Since the console driver is using copy64 
 *      and we need speed, we use register aliasing so that
 *      we do not have to flush the write buffer. I will 
 *      increment the register address by 0x1000 for the copy
 *      64 registers, for example 0x1160,0x2160,0x3160..up
 *      to 12 entries. 
 *              
 *      Note, that if the active pixel count is set to 1284 or
 *      some unaligned pixel boundary, we can not use copy 64, 
 *      which will be the case since lines are more important
 *      than copies I guess.
 *         
 * ALGORITHM:
 *      Initialize Copy Mode Structure type
 *      Set to copy 64 type 
 *      Set copymask to 0xffffffff
 *      Set default visuals 
 *      Call Aligned copy routine in driver_copy.c
 *
--*/         
int sfbp$$copy_video_memory ( SCIA *scia_ptr, SFBP *data_ptr ,
        char *source,char *dest,unsigned int size)
          
{         
SCOPY    copy;
VISUAL   visual;
Z        z;
       
 copy.src_address = (int)source;
 copy.dst_address = (int)dest;
       
 copy$$dma_scroll (scia_ptr,data_ptr,&copy,&visual,&z,size); 
       
 return (1);

}
           

/*+
 * ===================================================
 * = sfbp$$fill_video_memory - Data Fill Video Memory =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This is used to fill video ram with a pattern.
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$fill_video_memory (scia_ptr,data_ptr,source,pattern,size)
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	data_ptr - driver data area pointer
 *	source	 - source pointer (copy from here )
 *	pattern  - Data Fill Pattern 
 *	size	 - This many bytes 
 *
 * FUNCTIONAL DESCRIPTION:
 *      Set up the stipple type to tile video ram. We use 
 *      transparent block fill for maximum speed.
 *
 *
 * ALGORITHM:
 *      Initialize Stipple Data Structure type
 *      Set to transparent block fill mode
 *      Set default visuals
 *      Call stipple driver to stipple size and pattern
 *           
 *      
  --*/
int sfbp$$fill_video_memory (SCIA *scia_ptr,SFBP *data_ptr,char *source,int pattern,int size)

{
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;

 stip.src_address = (int)source;
 stip.size        = size;
 stip.mode        = GXcopy;
 stip.stipple     = transparent_block_fill;
 
 color.fg_value   = pattern;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = Xwindow;
 visual.boolean   = stip.mode;

 (*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

return (SS$K_SUCCESS);
}
                


#if FULL_TGA
/*+
 * ===================================================
 * = sfbp$$move_cursor - This routine will move the cursor position =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine will update the driver_data area cursor position
 *     and display the cursor in this new position
 *    
 * FORM OF CALL:
 *  
 *     sfbp$$move_cursor(scia_ptr,data_ptr,direction);
 *
 * ARGUMENTS:
 *
 *	scia_ptr - pointer to the scia
 *      data_ptr - driver data area pointer
 *      direction - direction to move cursor 
 *	 
 * FUNCTIONAL DESCRIPTION:
 *
 *     This routine moves the cursor position in the driver data area 
 *     directions allowed are:
 *
 *
 * ALGORITHM:
 *     - Decode the cursor move operation.
 *       - sfbp$START_SCREEN    - places cursor at top left cell of console screen
 *       - sfbp$BEGIN_LINE      - places cursor at column zero of current line
 *       - sfbp$BEGIN_NEXT_LINE - places cursor at column zero of the next line
 *       - sfbp$NEXT_LINE       - places cursor at same column on next line
 *       - sfbp$FORWARD         - places cursor at the next cell address (same line)
 *       - sfbp$BACKWARD        - places cursor at the last cell address (same line)
 *       - sfbp$END_LAST_LINE   - places cursor at the last cell of last line
 *       - sfbp$SAME            - re writes the cursor registers 
 *     - Call update cursor routine to update RAMDAC cursor.
--*/

int	sfbp$$move_cursor ( SCIA *scia_ptr, SFBP *data_ptr ,int direction)

{                                                          
register unsigned int i,s=SS$K_SUCCESS;

switch(direction)
      {
       case sfbp$k_begin_line:     
         {
         data_ptr->sfbp$l_cursor_column = 0;
         break;
         }
       case sfbp$k_begin_next_line:
         {
         data_ptr->sfbp$l_cursor_row++;
         data_ptr->sfbp$l_cursor_column = 0;
         s=sfbp$$zero_line (scia_ptr, data_ptr , OneRow );
         break;
         }
       case sfbp$k_next_line:      {data_ptr->sfbp$l_cursor_row++;break;}
       case sfbp$k_forward:        {data_ptr->sfbp$l_cursor_column++;break;}
       case sfbp$k_backward:       {data_ptr->sfbp$l_cursor_column--;break;}
       case sfbp$k_end_last_line:  {data_ptr->sfbp$l_cursor_row--;
                                   data_ptr->sfbp$l_cursor_column = data_ptr->sfbp$l_cursor_max_column;break;}
       case sfbp$k_same:break; 
       default : s=SS$K_ERROR;
      }

if (data_ptr->sfbp$l_cursor_update)
        (*bt_func_ptr->pv_bt_pos_cursor)(scia_ptr,data_ptr,
                data_ptr->sfbp$l_cursor_row,
                data_ptr->sfbp$l_cursor_column,FALSE);


return(s);
}


/*+
 * ===================================================
 * = sfbp$$delete_character - deletes a character on the screen =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *    This routine will delete the last character put to the screen
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$delete_character(scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr - pointer to the scia
 *      data_ptr - driver data pointer
 *	 
 * FUNCTIONAL DESCRIPTION:
 *
 *   - if cursor is at beginning of a line
 *      - move the cursor back one row and at last column
 *     else 
 *      - move the cursor back one character cell
 *   - call sfbp_display_a_character to display a space
 *    
 *     I need to tell the character driver I am erasing a character
 *     so it can set up the pixel mask correctly.
 *      
 * ALGORITHM:
 *	- check for character delete outside the console window.
 *	- Move the cursor back 1 position (maybe to the last line).
 *      - Then put a space where the current character is
 *      - Then update the cursor.
--*/

int	sfbp$$delete_character ( SCIA *scia_ptr, SFBP *data_ptr )
 
{

if(data_ptr->sfbp$l_cursor_column != 0 || data_ptr->sfbp$l_cursor_row != 0)
 {
 if(data_ptr->sfbp$l_cursor_column == 0)
        sfbp$$move_cursor(scia_ptr,data_ptr,sfbp$k_end_last_line);
 else sfbp$$move_cursor(scia_ptr,data_ptr,sfbp$k_backward);
 sfbp$$display_a_character(scia_ptr,sfbp$k_space_index,
           scia_ptr->usfont_table_ptr,EraseChar, 0/*attribute*/);
 }

return (1);
}


/*+
 * ===================================================
 * = sfbp$$tab_character - Tab character on the screen =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *    This routine will move over 8 spaces.
  *  
 * FORM OF CALL:
 *  
 *     sfbp$$tab_character(scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr - pointer to the scia
 *      data_ptr - driver data pointer
 *	 
 * FUNCTIONAL DESCRIPTION:
 *      - move over 8 characters
 *      
 * ALGORITHM:
 *	- check for character delete outside the console window.
 *	- Move the cursor back 1 position (maybe to the last line).
 *      - Then put a space where the current character is
 *      - Then update the cursor.
--*/

int	sfbp$$tab_character ( SCIA *scia_ptr, SFBP *data_ptr )
 
{
register        int     i;

for (i=0;i<8;i++)
 sfbp$$display_character(scia_ptr,sfbp$k_space_index,
                scia_ptr->usfont_table_ptr);

return (1);
}



/*+
 * ===================================================
 * = sfbp$$line_feed - handles the line feed character =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine will update the cursor position registers 
 *     on the sfbp.
 *    
 * FORM OF CALL:
 *  
 *     sfbp$$line_feed(scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr - pointer to the scia
 *      data_ptr - data area pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *      Line feed character
 *
 * CALLS:
 *      sfbp$$move_cursor();
 *      sfbp$display_cursor_line();
 *      sfbp$$scroll();
 *
 * ALGORITHM:
 *	- turn off cursor at current position
 *	- if row max instance: scroll everthing up 1 line 
 *		 move cursor to same position in video ram
 *	- else move cursor to next line
--*/

int	sfbp$$line_feed ( SCIA *scia_ptr, SFBP *data_ptr )
{

if ( data_ptr->sfbp$l_cursor_row == (data_ptr->sfbp$l_cursor_max_row-1) )
 {
 sfbp$$scroll(scia_ptr,data_ptr);
 sfbp$$move_cursor(scia_ptr,data_ptr,sfbp$k_same);
 sfbp$$zero_line(scia_ptr,data_ptr,OneRow );
 }
else sfbp$$move_cursor(scia_ptr,data_ptr,sfbp$k_next_line);

return(1);
}
#endif

/*+
 * ===================================================
 * = sfbp$$configure_video - setup the video specific parameters =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine determines the video monitor and sets
 *     up the vram and monitor specific parameters. This is
 *     the majority of video subsystem register initialization.
 *
 *    
 * FORM OF CALL:
 *     sfbp$$configure_video(scia_ptr,data_ptr);
 *
 * RETURNS:
 *
 *	integer - SS$K_SUCCESS when it is successful
 *              - SS$K_ERROR   when it fails
 *
 * ARGUMENTS:
 *	scia_ptr   - shared driver interface pointer
 *      data_ptr   - pointer to the sfbp driver data area
 *
 * GLOBALS:
 *     none
 *
 * FUNCTIONAL DESCRIPTION:
 *      This routine is used to get the module type and oscillator type. This information
 *      is then used for asic and ramdac initialization. If the console driver 
 *      is the current thread, then I skip the ics load because the module
 *      diagnostics were already executed.
 *
 * ALGORITHM:
 *      Get the Module type ( 8 plane,24 plane and ramdac type)
 *      IF not the console driver
 *      THEN 
 *              Load the ICS part with oscillator data
 *
 *      Get the video ram type                
 *      Connect Up with Ramdac Driver 
 *      Calculate the screen bytes
 *      Calculate the font table cell offset
 *      Calculate the cursor x and y bias
 *      
 *         
--*/
int sfbp$$configure_video ( SCIA *scia_ptr, SFBP *data_ptr )

{
register struct  sfbp_registers *register_ptr;
register int     done,i,s=1;
struct           monitor_data *dst,*src;
int	fx,fy; /* Cursor Position offsets */ 
#define          CURSOR_CENTER 31

 /* Determine the Module Type    */
 sfbp$$get_module_type (scia_ptr,data_ptr);

 /* Load up the ICS part         */
 sfbp$$load_ics (scia_ptr,data_ptr,
        &data_ptr->sfbp$r_mon_data.sfbp$r_isc_data);

 /* Figure out how to set up vram*/
 sfbp$$get_vram_type (scia_ptr,data_ptr);

 /* Connect Ramdac Driver        */
 sfbp$$connect_ramdac_type (scia_ptr,data_ptr);

 /* Calculate Screen Dimensions  */
 calc$$screen_bytes   (scia_ptr,data_ptr);

 /* Calculate font size          */
 calc$$cell_font_size (scia_ptr,data_ptr);

 /* Adjust Cursor Bias - 485 	 */
 if (data_ptr->sfbp$l_ramdac==bt485)         
  {                                
  data_ptr->sfbp$r_mon_data.sfbp$l_cursor_x = 64;
  data_ptr->sfbp$r_mon_data.sfbp$l_cursor_y = 64;
  }                                             
 else                                           
  {                                             
  data_ptr->sfbp$r_mon_data.sfbp$l_cursor_x = data_ptr->sfbp$r_mon_data.sfbp$l_h_back_porch +
                                              data_ptr->sfbp$r_mon_data.sfbp$l_h_sync -3 - 52  + CURSOR_CENTER + 23;
  data_ptr->sfbp$r_mon_data.sfbp$l_cursor_y = data_ptr->sfbp$r_mon_data.sfbp$l_v_back_porch +    
                                              data_ptr->sfbp$r_mon_data.sfbp$l_v_front_porch +   
                                              data_ptr->sfbp$r_mon_data.sfbp$l_v_sync - 1 - 32 + CURSOR_CENTER;
  fx=fy=0;                                                   
  switch   (data_ptr->sfbp$r_mon_data.sfbp$l_sold_freq )   { 
    case 104:
    case 69:                                    
    case 65:  /* Switch Pos. 8 */
         fy=-5;      /* negative fy values move cursor up */   
         break;      /* negative fx values move cursor left */
    case 25:  /* 12 */         
    case 32:  /* 11 */                                   
         fy=-7;      
         break;      
    case 50:  /* 9 */         
         fy = -30;                               
         break;                                  
    case 40: /* 10 */      
         fy -22;                   
         break; 
  }                                        
  data_ptr->sfbp$r_mon_data.sfbp$l_cursor_y += fy;                                                              
  data_ptr->sfbp$r_mon_data.sfbp$l_cursor_x += fx;      
  }                                
 /* Set console_offset via module type. */
 data_ptr->sfbp$l_console_offset = data_ptr->sfbp$l_module == hx_8_plane ? CONSOLE_OFFSET : CONSOLE_OFFSET*4;
 
return(SS$K_SUCCESS);
}


/*+
 * ===================================================
 * = sfbp$$get_module_type - get monitor type =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *     Get the module type
  *  
 * FORM OF CALL:
 *     sfbp$$get_module_type (scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr	- shared driver interface pointer
 *      data_ptr        - driver data area pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	This routine does a rom read to get the module and osc 
 *      switch bits. These are located in the upper byte of
 *      the data returned. I read back a module number from 0
 *      to N. I then have this in my table for each oscillator
 *      entry. 
 *
 * ALGORITHM:
 *      Config = Read location 0 of rom space
 *      Module = Config 31:28   (0=8p,1=24p,2=24p with z,3=dual head)
 *      Switch = Config 28:24   (0 to 16 with 1 off)
 *      Etch   = Read the start register for the asic revision
 *      
 *      Print out the rom data if doing debug 
 *     
 *      SCAN my crystal table to find option that matches
 *      IF I do not find the option in table THEN use 130 Mhz
 *                                    
 *      Load the ICS part with the found option.
 *                                    
 *      Here is the enumerated structure that I use.
 *                                    
 *       enum option_types            
                {                     
                OPTION_130,                     0
                OPTION_119,                     1                                                 
                OPTION_108,                     2
                OPTION_104,                     3
                OPTION_92,                      4
                OPTION_75,                      5        
                OPTION_74,                      6        
                OPTION_69,                      7        
                OPTION_65,                      8
                OPTION_50,                      9 
                OPTION_40,                     10
                OPTION_32,                     11 
                OPTION_25,                     12 
        };   
 *
 *
--*/

int	sfbp$$get_module_type( SCIA *scia_ptr, SFBP *data_ptr)

{
struct   monitor_data *dst,*src;
volatile register int  oscill,data,vcc,i,j,done=FALSE,count;
volatile register char *isc_ptr;
int      config;
register ROM_CONFIG    *rom_ptr;

 config   = (*gfx_func_ptr->pv_sfbp_read_rom)(scia_ptr,0);
 config  &= 0x3fff;     
 rom_ptr  = (ROM_CONFIG *)&config;
 oscill   = data_ptr->sfbp$l_override ? data_ptr->sfbp$l_override : rom_ptr->sfbp$r_rom_config_bits.sfbp$v_switch;

 /* If this an 8 plane module...it is possible that 	*/
 /* we will read a value of 2 for the module type	*/
 /* Because they only had 1 bit for module type		*/
 /* If the module low bit is set to 0...then its 8	*/
                        
 /* Save the Module Type for ASIC Reset */
 data_ptr->sfbp$l_module = rom_ptr->sfbp$r_rom_config_bits.sfbp$v_module;
 /* 2 head not supported...make it 1    */
 if (data_ptr->sfbp$l_module == hx_8_plane_dual_head)
	data_ptr->sfbp$l_module = hx_8_plane;
	                                                                        
 data_ptr->sfbp$l_col    =  Column256;
 data_ptr->sfbp$l_etch_revision =  
        (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_start);
 
 for (i=done=0;!done &&i*sizeof(struct monitor_data)<sizeof(crystal_table);i++)            
   if (oscill==crystal_table[i].sfbp$l_option)              
        {                                                   
        copy_crystal_data (data_ptr,crystal_table[i].sfbp$l_sold_freq,oscill);
        done=1;
        }      
           
 if (!done)copy_crystal_data (data_ptr,130,oscill);
           
 if ( (data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels%8)==0 )
         data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels += OddPixels;
           
 return(1);
           
}          
           
           
          
/*+        
 * ===================================================
 * = sfbp$$load_ics - Load ICS =
 * ===================================================
 *         
 *         
 * OVERVIEW:
 *     Load ICS1562 
 *         
 * FORM OF CALL:
 *     sfbp$$load_ics (scia_ptr,data_ptr,m)
 *         
 * ARGUMENTS:
 *	scia_ptr	- shared driver interface pointer
 *      data_ptr        - driver data area pointer
 *      m               - pointer to 56 byte stream
 *         
 * FUNCTIONAL DESCRIPTION:
 *	The ICS part has 56 bits which are loaded serially. I load 
 *      55 bits and then latch on bit 56 being loaded. The ICS 
 *      data is located in the driver crystal table. The ICS data
 *      is predetermined for all of our crystals and is located
 *      in the driver crystal table. 
 *         
	This register is used to program the ICS1562 Video Dot Clock
	Generator. Programming the ICS1562 requires 56 consecutive writes
	to this register supplying one data bit each write.  On the last
	write, the ~hold bit should be deasserted (set to a one) causing
	the data bits to be transfered from the holding register.

	This is a write only register.

	31                                                       2 1   0
	+---------------------------------------------------------+----+
	|                                ignore                   |~h d|
	+---------------------------------------------------------+----+

	bit 0  data
	bit 1 ~hold
 *
 *
--*/
int	sfbp$$load_ics ( SCIA *scia_ptr, SFBP *data_ptr,char *ics_ptr)

{
int     i,j,data;
enum    clock_types {Hold_L,Hold_H};
char    *NewIcsPtr;
       
/* We need to do this twice for RAMdac reset    */
/* which is bit 6 set to 1 the first time       */
/* And zero the second                          */
/* The table has zero in it                     */

for (j=1;j>=0;j--)
 {
  NewIcsPtr = ics_ptr;
  /* Load 55 bits with Hold Low and then assert on last bit 56      */
  for (i=0;i<sizeof(ISC);i++)
        {
        data  = (int)*NewIcsPtr++;
     	/* If freq > 70 Mhz then post scale it			    */
     	/* This is only on 485 though				    */
#if  (0)   /* Carryover from TGA pass 1 boards.  Soon going away. */
      	if (data_ptr->sfbp$l_module == hx_8_plane &&
	    data_ptr->sfbp$r_mon_data.sfbp$l_sold_freq > 70
            && i==12) data= 1;               
#endif      
        if (i==6)data = j;         
        if (i==sizeof(ISC)-1) data |= (Hold_H << 1);
        (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_ics_1562,data&0xff,LW);
        }
  }

return (1);
}



/*+
 * ===================================================
 * = sfbp$$get_vram_type - get monitor type =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *     Get the vram type
  *  
 * FORM OF CALL:
 *     sfbp$$get_vram_type (scia_ptr,data_ptr)
  *
 * ARGUMENTS:
 *	scia_ptr	- shared driver interface pointer
 *      data_ptr        - driver data area pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	This routine gets the module type and then sets up all of
 *      the values in driver data area that are required to initialize
 *      the deep register in The asic reset routine.  
 *
--*/

int	sfbp$$get_vram_type( SCIA *scia_ptr, SFBP *data_ptr)

{
register int      i,vram_type;
register char     *vram_mem_ptr;
register struct   drv_desc        *drv;
register IOSEG   *ioseg_ptr;

 drv  = (struct drv_desc *)&scia_ptr->gfx_drv_desc;
 ioseg_ptr  = (IOSEG *)drv_desc_ptr->ioseg_ptr ;

 for (i=0;i<3;i++)data_ptr->sfbp$l_block[i]=0;

 data_ptr->sfbp$l_heads          = 1;
 vram_mem_ptr                    = (char *)TGA_mem_base[data_ptr->sfbp$l_slot_number];
 data_ptr->sfbp$l_cursor_offchip = FALSE;

 if (data_ptr->sfbp$l_module == hx_8_plane )
        {
        data_ptr->sfbp$l_ramdac         = bt485;
        data_ptr->sfbp$l_planes         = 8 ;
        data_ptr->sfbp$l_vram_size      = 2* sfbp$k_one_mill ;
        vram_mem_ptr                   += (sfbp$k_a_sfbp_vram_mem_8 );
        data_ptr->sfbp$l_vram_base      = sfbp$k_a_sfbp_vram_mem_8;
        data_ptr->sfbp$l_sam            = sam_512;
        data_ptr->sfbp$l_mask           = mask_4MB;
        data_ptr->sfbp$l_default_mode   = sfbp$k_simple ;       
        data_ptr->sfbp$l_default_rop    = GXcopy|(DestVisualPacked<<8);
        }
        
else if (data_ptr->sfbp$l_module == hx_32_plane_no_z )
        {
        /* 8 Mbytes     video ram       */
        data_ptr->sfbp$l_ramdac         = bt463;
        data_ptr->sfbp$l_planes         =  32 ;
        data_ptr->sfbp$l_vram_size      = 8 * sfbp$k_one_mill ;
        data_ptr->sfbp$l_sam            = sam_256;
        data_ptr->sfbp$l_mask           = mask_16MB;
        data_ptr->sfbp$l_default_mode   = sfbp$k_simple | (SourceVisualTrue<<8) ;       
        data_ptr->sfbp$l_default_rop    = GXcopy|(DestVisualTrue<<8);
        data_ptr->sfbp$l_cursor_offchip = TRUE;
        vram_mem_ptr                   += (sfbp$k_a_sfbp_vram_mem_32_noz );
        data_ptr->sfbp$l_vram_base      = sfbp$k_a_sfbp_vram_mem_32_noz;
        }
        
else if (data_ptr->sfbp$l_module == hx_32_plane_z )
        {
        /* 16 Mbytes     video ram       */
        data_ptr->sfbp$l_ramdac         = bt463;
        data_ptr->sfbp$l_planes         = 32 ;
        data_ptr->sfbp$l_vram_size      = 16 * sfbp$k_one_mill ;
        data_ptr->sfbp$l_sam            = sam_256;
        data_ptr->sfbp$l_mask           = mask_32MB;
        data_ptr->sfbp$l_default_mode   = sfbp$k_simple | (SourceVisualTrue<<8) ;       
        data_ptr->sfbp$l_default_rop    = GXcopy|(DestVisualTrue<<8);
        data_ptr->sfbp$l_cursor_offchip = TRUE;
        vram_mem_ptr                   += (sfbp$k_a_sfbp_vram_mem_32_z );
        data_ptr->sfbp$l_vram_base      = sfbp$k_a_sfbp_vram_mem_32_z;
        }

 ioseg_ptr->sfbp$a_vram_mem_ptr = (char *)vram_mem_ptr;
    
 return(1);
}



/*+
 * ===================================================
 * = sfbp$$connect_ramdac_type - get monitor type =
 * ===================================================
 * OVERVIEW:
 *     Get the ramdac type
 *  
 * FORM OF CALL:
 *     sfbp$$connect_ramdac_type (scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr	- shared driver interface pointer
 *      data_ptr        - driver data area pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	Knowing the ramdac type, I connect to my ramdac port
 *      driver routines. The ramdac driver can be a console
 *      or diagnostic version since they are linked at 2
 *      different addresses. The console one is linked to 
 *      fit within the crt region and diagnostic one fits
 *      within the T region. This is done because we do not
 *      have fixup or pic code.
 *
 * ALGORITHM:
 *      If console driver mode THEN ramdac_name = "crt"
 *      ELSE ramdac_name = "tci"
 *
 *      IF ramdac driver not loaded
 *        THEN
 *              IF ramdac is 463 then strcat (ramdac_name,463")
 *              ELSE IF ramdac is 459 then strcat (ramdac_name,459")
 *              ELSE IF ramdac is 485 then strcat (ramdac_name,485")
 *              Load ramdac_name rom object in ramdac region 
 *
 *      Connect to driver initialization (init_driver at ramdac_region)
 *
 --*/

int	sfbp$$connect_ramdac_type( SCIA *scia_ptr, SFBP *data_ptr)

{
register long     (*driver_init)();

 if ( data_ptr->sfbp$l_ramdac == bt485)
        driver_init = (long (*) () )sfbp$init_bt_485_driver;

 else if ( data_ptr->sfbp$l_ramdac == bt463)
	driver_init = (long (*) () )sfbp$init_bt_463_driver;

 (*driver_init)(scia_ptr,ccv);

 return(1);
}


/*+
 * ===================================================
 * = copy_crystal_data - copy the crystal data =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *     Copy the crystal data
 *  
 * FORM OF CALL:
 *     copy_crystal_data (data_ptr,frequency,oscill)
 *
--*/
int     copy_crystal_data (SFBP *data_ptr,int frequency,int oscill)

{
register  int   i,done=0;
register  struct monitor_data *src,*dst;

 for (i=0;!done && i*sizeof(struct monitor_data)<sizeof(crystal_table);i++)   
  	if (crystal_table[i].sfbp$l_sold_freq == frequency )                                
                 {
                 src  = (struct monitor_data *)&crystal_table[i];           
                 dst  = (struct monitor_data *)&data_ptr->sfbp$r_mon_data;  
                 *dst = *src;
                 done = 1;
                 }

return (1);
}




/*+
 * ===================================================
 * = sfbp$$asic_reset - sets up the sfbp video hardware =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine initializes the sfbp video registers to correspond
 *     to the values currently in the driver data area. The driver
 *     data areas were established during configuration or a 
 *     parameter modification.
 *    
 * FORM OF CALL:
 *  
 *     sfbp$$asic_reset(scia_ptr,data_ptr);
 *
 * ARGUMENTS:
 *
 *	scia_ptr   - shared driver interface area
 *      data_ptr   - pointer to the sfbp driver data area
 *
 * FUNCTIONAL DESCRIPTION:
 *	Initialize the video subsystem. This routine is called after
 *	the driver data area has been established and the cursor
 *	context is in the upper left hand corner of the screen.
 *      The driver data area has all the implementation specific
 *      parameters associated with it, such as monitor type and
 *      resolution. The timing parameters associated with the 
 *      system should be established prior to initializing the
 *      RAMDAC. These are the timing registers on the sfbp ASIC.
 *              We have to determine how to set up video ram 
 *      types, so we do a block operation to video ram and read
 *      back the data, if it is wrong, then we toggle state of
 *      the blk_col bits. On 32 plane systems with z buffer, we
 *      need to do this to primary and secondary modules.
 *
 *
	The Deep Register specifies the type and configuration of the frame
	buffer.  The SFB+ will support 8 and 32 plane frame buffers comprised
	of many different types of video rams.  Since the deep register
	describes the type of rams PHYSICALLY connected to the SFB+, it
	should be set by the initialization sequence only.

                                       1 1 1 1 1 1 1
	31                             6 5 4 3 2 1 0 9 8     5 4   2 1 0
	+-----------------------------+-+-+-+-+-+-+-+-+-------+-----+-+-+
	|           MUST BE ZERO      |H|D|B|R|W|P|S|C|  Blk  | Mask|0|D|
	+-----------------------------+-+-+-+-+-+-+-+-+-------+-----+-+-+

	The Deep field of the Deep Register specifies the physical depth of
	of the frame buffer.  See the Mode Register description for how to
	paint 8-bit visuals on a 32-bit frame-buffer.

	Deep:	0      8 Plane
		1     32 Plane

	The Mask field of the Deep Register enables decoding of bits <24:22>
	of system addresses.  The mask field is set as follows:

	Mask:   000	4MB
		001     8MB
		011    16MB
		111    32MB

	The Block Type field of the Deep Register defines the format of block
	mode data for each bank of video ram.  See the specific video ram spec
	for a more detailed description of the block transfer operation.

	Blk[0]: 0     4 column decode bank 0
		1     8 column decode bank 0

	Blk[1]: 0     4 column decode bank 1
		1     8 column decode bank 1

	Blk[2]: 0     4 column decode bank 2
		1     8 column decode bank 2

	Blk[3]: 0     4 column decode bank 3
		1     8 column decode bank 3

	The Column Size Field of the Deep Register defines the column size of
	of the video rams.  Frame buffers comprised of 128kx video rams have
	8 bits of column address, 256kx video rams have 9 bits of address.

	C:	 0	9 bits (256kx rams)
		 1	8 bits (128kx rams)

	The SAM size field of the Deep Register defines the size of the serial
	shift register in the video rams.

	S:	 0	512 entries
		 1	256 entries

	The Parity defines the type of word parity to expect/generate on the
	bus.  The Parity bit can be reversed by diagnostics to test the parity
	checking logic within the chip.  Since the TURBOchannel does not
	currently define a method to test parity generation logic, the SFB+
	will always drive odd parity.

	P:	 0	odd parity
		 1	even parity.

	The Flash ROM Write enable bit when set will enable writes to the
	Flash ROM.  When not set, writes to the Flash ROM register will not
	effect the ROM, but the cycle will be externally visible and could
	be used to implement a write-only parallel port.

	W:	 0	Writes are disabled.
		 1	enable writes to the Flash ROM

	The Ready bit defines the space required in the command buffer
	before the chip will accept a write transaction.  By default the
	chip requires at least eight available entries, on systems that
	do not support block transfers, the ready bit can be set and the
	chip will issue ready when only two available entries exist.

	R:	 0	8 available entries required
		 1	2 available entries required

	The slowDac bit of the deep register causes the SFB+ to stall after
	each ramdac operation.  This is required by most PC ramdacs which
	cannot accept consecutive operations very quickly.

	B:	 0	no automatic stalls
		 1      stalls inserted by hardware.

	The DMA size bit determines the maximum length DMA burst the SFB+
	will attempt.

	D:	 0	 64 word max transfers.
		 1	128 word max transfers.


	The hSync bit of the deep register controls the style of video sync
	pulse generated.  When set the SFB+ will generate separate vertical
	and horizontal sync pulses, by default the SFB+ generates a composite
	sync signal a stereo goggle control signal.

	H:	 0	Composite sync on vSync pin, stereo control  on hSync pin
		 1	Vertical  sync on vSync pin, horizontal sync on hSync pin


	The remaining bits within the Deep Register must be set to zero, some
	of these bits are used to control internal testability features.
 *    
	The Video Base Address register contains the starting row address
	within the Frame Buffer of the visible portion of video memory.
	Software must insure that sufficient video memory exists beyond
	the Video Base Address to display the entire screen. The Video Base
	register is a Write Only register.

	31                                           9 8               0
	+---------------------------------------------+----------------+
	|                 ignore                      |   Video Base   |
	+---------------------------------------------+----------------+

	Value at init: 0


Video Valid
-----------

	The Video Valid Register contains bits to enable portions of the
	video system. Setting the valid bit indicates the monitor timing
	control registers have been properly set-up, the Blank bit is 
	used to disable output after the registers have been set-up.

	BV:	00 - No video signal generated.
		01 - Active display
		10 - No video signal generated.
		11 - Sync pulses only, no active video.

	C:	 0 - disable Cursor display
		 1 - enable  Cursor display

	31                                                      3 2 1 0
	+--------------------------------------------------------+-+-+-+
	|                           ignore                       |C|B|V|
	+--------------------------------------------------------+-+-+-+

	Value at init: 0
 *
 *  ALGORITHM:
 *      Wait for Chip Idle
 *      Write Deep Register with depth,column,block,and sam initial values.
 *      Wait for Chip Idle
 *      Write Planemask to 0xffffffff
 *      Write Pixelmask to 0xffffffff
 *      Write Raster Op With Default Raster Op
 *      Calculate the source and dest visuals for module
 *      Get the turbo channel information for DMA word size and Block Mode
 *      IF word size is 128 words THEN set deep word size to 1(default)
 *      IF word size is  64 words THEN set deep entry size to 1 ( 2 entries only (pelican))
 *              NOTE: Pelican is only system that is 64 words ( 3 MIN ? )
 *      Write SFB+ mode register to transparent block stipple | visual << 8
 *      IF this is an 8 plane system
 *        THEN    
 *              Write block color 0 and 1 with 12345678
 *              Write frame buffer location 0 0xffffffff (stipple op)
 *              Read Location 0 
 *              If location 0 NOT equal to 12345678 
 *              THEN toggle block[0] bits
 *
 *      ELSE IF this is a 32 plane system with no z buffer
 *        THEN    
 *              Write block color 7:0 with 12345678
 *              Write frame buffer location 0 0xffffffff (stipple op)
 *              Read Location 0 
 *              If location 0 NOT equal to 12345678 
 *              THEN toggle block[0] bits
 *
 *      ELSE IF this is a 32 plane system with z buffer
 *        THEN   
 *              do the bottom module first
 *              Write block color 7:0 with 12345678
 *              Write frame buffer location 0 0xffffffff (stipple op)
 *              Read Location 0 
 *              If location 0 NOT equal to 12345678 
 *              THEN toggle block[0] bits
 *              Now do the top module
 *
 *              Write frame buffer location 0 relative to z start (stipple op)
 *              Read Location 0 relative to z start 
 *              If location 0 relative to z start NOT equal to 12345678 
 *              THEN toggle block[1] bits
 *
 *    Wait for Chip Idle
 *    Write the DEEP register with new value
 *    Wait for Chip Idle
 *    Set Horizontal and vertical timing values
 *    Set Video Valid to Active Video
 *    Set Bool Op to Default rop
 *    Set Mode to Default Mode
 *    Set Video Refresh base to 4096 (4k)
 *    Get the mips system id (getsysid) 
 *    IF the system ID TCFx is TCF1 (3)
 *      THEN 
 *         enable block mode writes to current slot 
 *         enable parity to current slot 
 *      
 --*/   
int sfbp$$asic_reset (SCIA *scia_ptr,SFBP *data_ptr)
        
{                               
                                
int     i,data,load_count,j,s=1;
union   deep_reg *deep;         
int     mode,expect,actual,deep_value;
int     VramOffset,SourceVisual,DestVisual;
int 	BottomOffset,A2offset;  
 deep           = (union deep_reg *)&deep_value;
 deep_value     = 0;            
                                
 deep->sfbp$r_deep_bits.sfbp$v_deep      = data_ptr->sfbp$l_module == hx_8_plane ? depth_8 : depth_32;
 deep->sfbp$r_deep_bits.sfbp$v_mask      = data_ptr->sfbp$l_mask;
 deep->sfbp$r_deep_bits.sfbp$v_blk0      = data_ptr->sfbp$l_block[0];
 deep->sfbp$r_deep_bits.sfbp$v_blk1      = data_ptr->sfbp$l_block[1];
 deep->sfbp$r_deep_bits.sfbp$v_blk2      = data_ptr->sfbp$l_block[2];
 deep->sfbp$r_deep_bits.sfbp$v_blk3      = data_ptr->sfbp$l_block[3];
 deep->sfbp$r_deep_bits.sfbp$v_col       = data_ptr->sfbp$l_col;
 deep->sfbp$r_deep_bits.sfbp$v_sam       = data_ptr->sfbp$l_sam;
 deep->sfbp$r_deep_bits.sfbp$v_parityf   = disabled;
#if MEDULLA
 deep->sfbp$r_deep_bits.sfbp$v_flashe    = enabled;
#else
 deep->sfbp$r_deep_bits.sfbp$v_flashe    = disabled;
#endif
 deep->sfbp$r_deep_bits.sfbp$v_dma_size  = dma_128;
 deep->sfbp$r_deep_bits.sfbp$v_block     = 0;

 if ( data_ptr->sfbp$l_ramdac == bt463)
   deep->sfbp$r_deep_bits.sfbp$v_slodac  = disabled;
 else  
   deep->sfbp$r_deep_bits.sfbp$v_slodac  = enabled;

 /* Composite Sync if sync on green true for slot	*/
 data    = rtc_read (TgaToyOffset);
 data   &= (1<<data_ptr->sfbp$l_slot_number);
 deep->sfbp$r_deep_bits.sfbp$v_hsync = data ? composite_sync : vertical_sync;

 data_ptr->sfbp$l_alpha_platform         = TRUE;
 deep->sfbp$r_deep_bits.sfbp$v_dma_size  = 1;
 deep->sfbp$r_deep_bits.sfbp$v_block     = 0;

 sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_deep,deep->sfbp$l_whole,LW);
 sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_mask,M1,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);
 data_ptr->sfbp$l_pixel_mask      = 0xFFFFFFFF;

 SourceVisual = data_ptr->sfbp$l_module == hx_8_plane ? SourceVisualPacked : SourceVisualTrue;
 DestVisual   = data_ptr->sfbp$l_module == hx_8_plane ? DestVisualPacked   : DestVisualTrue;
 mode = sfbp$k_transparent_block_stipple|(SourceVisual<<8);
 (*gfx_func_ptr->pv_sfbp_write_reg) (scia_ptr,sfbp$k_mode,mode,LW);

 expect       = 0x12345678;
 VramOffset   = 1024;
 BottomOffset = 0x800000;
   
 /* Start of IBM Ram Part code */
 A2offset = data_ptr->sfbp$l_module == hx_8_plane ? 0x20 : 0x80;

 (*gfx_func_ptr->pv_sfbp_write_reg) (scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg) (scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 for (i=VramOffset; i<(VramOffset+2*A2offset); i+=4)
    (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,i,0,LW);

 /* There is a bug in this code	*/
 if (data_ptr->sfbp$l_module == hx_32_plane_z)
    {
    for (i=BottomOffset+VramOffset; i<(VramOffset+BottomOffset+2*A2offset); i+=4)
      (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,i,0,LW);
    }
  
 SourceVisual = data_ptr->sfbp$l_module == hx_8_plane ? SourceVisualPacked : SourceVisualTrue;
 DestVisual   = data_ptr->sfbp$l_module == hx_8_plane ? DestVisualPacked   : DestVisualTrue;
 mode = sfbp$k_transparent_block_stipple|(SourceVisual<<8);
 (*gfx_func_ptr->pv_sfbp_write_reg) (scia_ptr,sfbp$k_mode,mode,LW);
 sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);
 /* End of IBM Ram Part code */

 if (data_ptr->sfbp$l_module == hx_8_plane )
  {
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_block_color_0,expect,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_block_color_1,expect,LW);
  (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,VramOffset,M1,LW);
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,VramOffset);
  if (actual!=expect)
   {
   data_ptr->sfbp$l_block[0] = ~data_ptr->sfbp$l_block[0];
   deep->sfbp$r_deep_bits.sfbp$v_blk0 = data_ptr->sfbp$l_block[0];
   }

  data_ptr->sfbp$l_pixel_mask      = 0xFFFFFFFF;
  data_ptr->sfbp$l_rotate_src_mask = 0x000000FF;
  data_ptr->sfbp$l_rotate_dst_mask = 0x000000FF;
  }
 
 if (data_ptr->sfbp$l_module == hx_32_plane_no_z)
  {
  for (i=0;i<32;i+=4)
    (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_block_color_0+i,expect,LW);

  (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,VramOffset,M1,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg) (scia_ptr,sfbp$k_mode,sfbp$k_simple,LW);
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,VramOffset);
  if (actual!=expect)
        {
        deep->sfbp$r_deep_bits.sfbp$v_blk0 = column8;
        deep->sfbp$r_deep_bits.sfbp$v_sam  = sam_512;
        }

  data_ptr->sfbp$l_pixel_mask      = 0x00FFFFFF;
  data_ptr->sfbp$l_rotate_src_mask = 0x000000FF;
  data_ptr->sfbp$l_rotate_dst_mask = 0x000000FF;
  }
  
 else if (data_ptr->sfbp$l_module == hx_32_plane_z)
  {
  for (i=0;i<32;i+=4)
    (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_block_color_0+i,expect,LW);

  /* Do the Top Module First */
  /* Which is Frame Buffer   */
  /* Do not write cursor ram */
  /* CORR: 8 column = 512 sam*/
  (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,VramOffset,M1,LW);
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,VramOffset);
  if (actual!=expect)
        {
        deep->sfbp$r_deep_bits.sfbp$v_blk0 = column8;
        deep->sfbp$r_deep_bits.sfbp$v_sam  = sam_512;
        }
        
  /* Now the Bottom           */
  (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,VramOffset,M1,LW);
  actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,VramOffset);
  if (actual!=expect)
        deep->sfbp$r_deep_bits.sfbp$v_blk1 = column8;
        
  data_ptr->sfbp$l_pixel_mask      = 0x00FFFFFF;
  data_ptr->sfbp$l_rotate_src_mask = 0x000000FF;
  data_ptr->sfbp$l_rotate_dst_mask = 0x000000FF;
  }

 sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_deep,deep->sfbp$l_whole,LW);
 sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);

 sfbp$$vt_set_horizontal       (scia_ptr,data_ptr);
 sfbp$$vt_set_vertical         (scia_ptr,data_ptr);

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);

  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_refresh_addr,1);

  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_int_status,0x1f,LW);
  sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);

  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);

  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_video_valid,ActiveVideo,LW);

return (1);
}


 
/*+
 * ===================================================
 * = sfbp$$vt_set_horizontal - Set video horizontal timing =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     Set the video horizontal timing
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$vt_set_horizontal (scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr	- shared driver interface pointer
 *	data_ptr	- data pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	This sets the horizontal timing parameters. We need to divide by 
 *      4 because of 4:1 mux.
 *
	The Horizontal Control Register contains the horizontal timing
	parameters specific to the monitor in use.  The counts are in
	4 pixel units. The horizontal control parameters must be
	defined prior to enabling video.  The ODD bit enables/disables
	the display of the last 4 pixels on systems that have an odd
	multiple of 4 pixels per active scan line.

	31 30 28 27         21 20         14 13      9 8               0
	+-+-----+-------------+-------------+---------+----------------+
	|O| ign | Back Porch  | Horiz Sync  | Front P |     active     |
	+-+-----+-------------+-------------+---------+----------------+

	Value at init: 0

                | Back Porch  | Horiz Sync  | Front P |     active     |
                |-------------+-------------+---------+----------------+
	    min |     2       |     2       |    2    |       2*       |
                |-------------+-------------+---------+----------------+


       * Note:  Programming for the active field is as follows:

				active
			value	pixels
			-----   ------
			 002	    8
			 003	   12
			  .
			  .
			  .
			 1ff	 2044
			 000	 2048
			 001	 2052

 *
 *
--*/

int	sfbp$$vt_set_horizontal (SCIA *scia_ptr,SFBP *data_ptr)

{
volatile  int       hvalue;
volatile  union     video_htiming_reg *h;
register  int       i,ActivePixels,FrontPorch;
char      temp[20];

hvalue         = 0;
h              = (union video_htiming_reg *)&hvalue;

/* This Active Pixel Calculation assumes h_pixels is 1284,1028 */
/* Already, so the extra pixel load is already assumed         */
/* We just have to decrement front porch by 1 cycle            */
/* So that the timing remains the same for SYNC pulses         */

ActivePixels   = data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels/4;
FrontPorch     = data_ptr->sfbp$r_mon_data.sfbp$l_h_front_porch/4;

h->sfbp$r_video_h_timing_bits.sfbp$v_htime_pixels 	= ActivePixels;
h->sfbp$r_video_h_timing_bits.sfbp$v_htime_front_porch 	= FrontPorch;
h->sfbp$r_video_h_timing_bits.sfbp$v_htime_sync 	= data_ptr->sfbp$r_mon_data.sfbp$l_h_sync/4;
h->sfbp$r_video_h_timing_bits.sfbp$v_htime_back_porch   = data_ptr->sfbp$r_mon_data.sfbp$l_h_back_porch/4;
h->sfbp$r_video_h_timing_bits.sfbp$v_fill_0             = 0;
h->sfbp$r_video_h_timing_bits.sfbp$v_odd                = 0;

if (data_ptr->sfbp$r_mon_data.sfbp$l_h_pixels%8)
        {
        h->sfbp$r_video_h_timing_bits.sfbp$v_htime_front_porch = FrontPorch-1;
        /* if Odd bit set: do not display last 4 pixels */
        /* If Odd Bit Clear: then display the last 4    */
        h->sfbp$r_video_h_timing_bits.sfbp$v_odd        = 1;
        }
        
(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,
                sfbp$k_horizontal,h->sfbp$l_whole,LW);

return (SS$K_SUCCESS);
}


/*+
 * ===================================================
 * = sfbp$$vt_set_vertical - Set video vertical timing =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Set the video vertical timing
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$vt_set_vertical (scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr	- shared driver interface pointer
 *	data_ptr	- data pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	This sets the vertical timing parameters.
 *
	The Vertical Control Register contains the vertical timing parameters
	specific to the monitor in use.  The counts are units of scan lines.
	The vertical control parameters must be defined prior to enabling
	video. The StereoEn bit disables reloading of the video base after
	alternate vertical sync periods.  

	31    28 27       22 21       16 15     11 10                  0
	+-+-----+-----------+-----------+---------+--------------------+
	|S| ign |Back Porch | Vert Sync | Front P | Active Scan Lines  |
	+-+-----+-----------+-----------+---------+--------------------+

	Value at init: 0

                 Back Porch | Vert Sync | Front P | Active Scan Lines
                +-----------+-----------+---------+--------------------+
            min |    1      |     1     |    0    |       1            |
                +-----------+-----------+---------+--------------------+
 *
 *
--*/

int	sfbp$$vt_set_vertical (SCIA *scia_ptr,SFBP *data_ptr)

{
volatile  int       vvalue;
volatile  union     video_vtiming_reg *v;
char      temp[20];
register  int i;

vvalue          = 0;
v               = (union video_vtiming_reg *)&vvalue;

v->sfbp$r_video_v_timing_bits.sfbp$v_vtime_scanlines   = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines;
v->sfbp$r_video_v_timing_bits.sfbp$v_vtime_front_porch = data_ptr->sfbp$r_mon_data.sfbp$l_v_front_porch;
v->sfbp$r_video_v_timing_bits.sfbp$v_vtime_sync        = data_ptr->sfbp$r_mon_data.sfbp$l_v_sync;
v->sfbp$r_video_v_timing_bits.sfbp$v_vtime_back_porch  = data_ptr->sfbp$r_mon_data.sfbp$l_v_back_porch;
v->sfbp$r_video_v_timing_bits.sfbp$v_fill_1            = 0;
v->sfbp$r_video_v_timing_bits.sfbp$v_stereo            = 0;

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,
        sfbp$k_vertical,v->sfbp$l_whole,LW);

return (SS$K_SUCCESS);
}


/*+
 * ===================================================
 * = sfbp$$tga_reset - Configure TGA Option =
  * ===================================================
 *
 * OVERVIEW:             
 *      Initialize the TGA configuration register   
 *    
 * FORM OF CALL:
 *    
 *     sfbp$$tga_reset(scia_ptr,data_ptr);
 *    
 * ARGUMENTS:
 *    
 *    	scia_ptr   - shared driver interface area
 *      data_ptr   - pointer to the sfbp driver data area
 *    
 * FUNCTIONAL DESCRIPTION:
 *      Write to the device status register to allow it to answer up
 *    
 --*/ 
      
int sfbp$$tga_reset (SCIA *scia_ptr,SFBP *data_ptr)
      
{     
union    DeviceRegType *device;
int      DeviceStatus=0;
union    CacheStuffType *cac;
int      mask,r,BaseAddress,CacheValue;
volatile register int  oscill,data,vcc,i,j,done=FALSE,count;
volatile register char *isc_ptr;
int      paer,config,module;
register ROM_CONFIG    *rom_ptr;
                         
 mask = PCI_long_mask;   
#if 0
/* config space is setup by pci_size_config.c or probe_io.c */
#if SABLE || MEDULLA || APC_PLATFORM 
#else
 device = (union DeviceRegType *)&DeviceStatus;
 device->DeviceBits.IOSpace     = 1;
 device->DeviceBits.MemorySpace = 1;
 device->DeviceBits.BusMaster   = 1;
 
 cac    = (union CacheStuffType *)&CacheValue;
 
 cac->CacheBits.CacheSize = 0x00;
 cac->CacheBits.Latency   = 0xff;
 cac->CacheBits.Header    = 0x00;
 cac->CacheBits.BIST      = 0x00;

 /* Device Status Register Set Up So Device Answers */
 (*gfx_func_ptr->pv_sfbp_write_pci_config)(scia_ptr,sfbp$k_a_sfbp_pci_config_cmd,DeviceStatus,mask);
 r = (*gfx_func_ptr->pv_sfbp_read_pci_config)(scia_ptr,sfbp$k_a_sfbp_pci_config_cmd);
 
 /* Latency Timer at Max                            */                           
 (*gfx_func_ptr->pv_sfbp_write_pci_config)(scia_ptr,sfbp$k_a_sfbp_pci_config_cac,CacheValue,mask);
 r = (*gfx_func_ptr->pv_sfbp_read_pci_config)(scia_ptr,sfbp$k_a_sfbp_pci_config_cac);
                                                                                 
 r = (*gfx_func_ptr->pv_sfbp_read_pci_config)(scia_ptr,sfbp$k_a_sfbp_pci_config_bas);
     
 /* Write the IRQ to the Config Register. Os can read this then to determine the interrupt	*/ 
 /* The proper thing to do here is read a configuration table for interrupts			*/

#if !(K2 || TAKARA)  /* Vector isn't 0x15 for DMCC products. */
 (*gfx_func_ptr->pv_sfbp_write_pci_config)(scia_ptr,ConfigPlir,0x15,mask);
#endif

#endif
#endif

 /* Now..also set up Address Extension Register    */
 /* I will get module type now so I can set this   */
     
 config   = (*gfx_func_ptr->pv_sfbp_read_rom)(scia_ptr,0);
 config  &= 0x3fff;     
 rom_ptr  = (ROM_CONFIG *)&config;
 oscill   = rom_ptr->sfbp$r_rom_config_bits.sfbp$v_switch;
 module   = rom_ptr->sfbp$r_rom_config_bits.sfbp$v_module;
 
 /* Shut off the pallete snoop					  */
 /* We are not in vga pass thru mode and also have no pals 	  */
 /* So just nail it						  */
 (*gfx_func_ptr->pv_sfbp_write_pci_config)(scia_ptr,
	PalleteSnoop,0,mask);

 /* If 24 plane...set address extension		*/
 /* Otherwise no extension			*/
 paer     = module == hx_8_plane ? 0 : 3;
 paer   <<= 16;

 (*gfx_func_ptr->pv_sfbp_write_pci_config)(scia_ptr,ConfigPaer,paer,mask);

 return (1);


}  

/*+
* ===========================================================================
* = copy$$dma_scroll - DMA scroll =
* ===========================================================================
*         
* OVERVIEW:
*       Scroll Using DMA copies 
*                 
* FORM OF CALL:   
*       copy$$dma_scroll (scia_ptr,data_ptr,copy,visual,z)
*                 
* FUNCTIONAL DESCRIPTION:
*       Use the DMA engine to dma video memory. We can dma 2048
*       bytes at a time so this results in less I/O writes to
*       do the copy so will be quicker than doing simple mode
*       copies.   
*                 
--*/              
int     copy$$dma_scroll (SCIA *scia_ptr,SFBP *data_ptr,SCOPY *copy, 
VISUAL *visual,Z *z,int size)
                              
{                             
register  int  j=0,k=0,s=1;   
volatile  register  int  ByteCount,WriteMask,ReadMask,EndAddress;
volatile  register  int  DmaBufferPtr,WrtDmaBase,RdDmaBase,RdDmaBase0,RdDmaBase1;
volatile  register  int  ReadCount,WriteWord,ReadWord,SourceVisual,DestVisual,src,dst;
volatile  register  char *reg;

 src            = copy->src_address;
 dst            = copy->dst_address;
 /*dst           += 8;    */
 src 	-= 8;
                                                          
 SourceVisual   = data_ptr->sfbp$l_module == hx_8_plane ? SourceVisualPacked: SourceVisualTrue;
 DestVisual     = data_ptr->sfbp$l_module == hx_8_plane ? DestVisualPacked  : DestVisualTrue;
 DmaBufferPtr   = ScrollDmaPtr[data_ptr->sfbp$l_slot_number];
                                   
 ByteCount      = (size < SCROLL_SIZE)?size:SCROLL_SIZE;                  
 WriteMask      = (  (ByteCount/8)<<16)|0xffff;        
                                   
 WrtDmaBase = (int  )DmaBufferPtr; 
 RdDmaBase0 = (int  )DmaBufferPtr; 
 RdDmaBase1 = (int )(DmaBufferPtr+4);
                    
 EndAddress = src + size;                    /* size is the byte count of the dma transaction. */
 WriteWord  = sfbp$k_dma_write_copy|(SourceVisual<<8);                 
 ReadWord   = sfbp$k_dma_read_copy_non_dithered|(SourceVisual<<8);     
                                                                       
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_shift,0x0,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_data_register,M1,LW );
          
#if (0)   
/* DMA write bug data misalignment bug, tga notesfile notes 39.1,39.4,39.5 */
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_dma_base_address,WrtDmaBase+WindowBase,LW);
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,WriteWord,LW);
/* Primer DMA write. */
   (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,(7<<16)|0xffff,LW);   
   /* HW Interlock	*/
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_command_status,0,LW);
   sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);
#endif          
          
  do      
   {      
   if ((int)src&7)
        { 
        /* NON 8 byte Transfer   */
        RdDmaBase  = RdDmaBase1;                                       
        ReadCount  = ByteCount-4;
        }                         
   else                           
        {                     
        /*  Byte transfer   */
        RdDmaBase  = RdDmaBase0;
        ReadCount  = ByteCount;
        }                        
   ReadMask   = (((ReadCount/4))<<16)|0xffff;
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_dma_base_address,
	  	WrtDmaBase+WindowBase,LW);
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,WriteWord,LW);
          
   (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,WriteMask,LW);
   /* HW Interlock	*/
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_command_status,0,LW);
   sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);

   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_dma_base_address,
		RdDmaBase+WindowBase,LW);
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,ReadWord,LW);

   (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,dst,ReadMask,LW);
   /* HW Interlock	*/
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_command_status,0,LW);
   sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);

   src += ReadCount;
   dst += ReadCount;
  
   } while (src<EndAddress);  

  /* Do any Required Cleanup    */
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_shift,0,LW);

 return (s);
}
  
/*+  
 * ===================================================
 * = SetUpTgaOption - Configure TGA Option =
  * ===================================================
 *         
 * OVERVIEW:
 *      Set up for Tga Operation
 *         
 * FORM OF CALL:
 *  	SetUpForTga (dev_fnd_ptr,index)
 *
 * ARGUMENTS
 *	dev_fnd - device found pointer 
 *	index	- my port block index
 *
 --*/
#if 0
int SetUpTgaOption (struct   pci_devs_found *dev_fnd_ptr,
		int index)
           
{                                                                                                                          
#if SABLE || MIKASA || MEDULLA || CORTEX || YUKONA || APC_PLATFORM
#else
#define  DecodeMask 0xFE000000
                        
 TGA_pb[index]->bus   = dev_fnd_ptr->dev_found[index].bus;
 TGA_pb[index]->slot  =  dev_fnd_ptr->dev_found[index].slot;
 TGA_pb[index]->hose  = 0;
 TGA_pb[index]->function = 0; 
 TGA_pb[index]->channel  = 0;
     
 /* This routine is called each time the  diags run	*/
 /* So we will check for a first time entry		*/
 if (TgaVector[index]==0)
  {  
     
     
  TgaVector[index] = dev_fnd_ptr->dev_found[index].vector;

  int_vector_set(TgaVector[index],tga_int_handler,index);
  TGA_mem_base[index] = incfgl (TGA_pb[index],sfbp$k_a_sfbp_pci_config_bas)& DecodeMask ;
  /* Enable the pic chip too...this guarantees I have   */
  /* the vector if a driver checks the pic chip		*/
#if AVANTI || K2 || MTU || TAKARA
  io_enable_interrupts (0, TgaVector[index]);
#endif

#if MIKASA 
  sys_pci_enable_interrupt  (TgaVector[index]);
#endif

  }

 return (1);
#endif
}
#endif
