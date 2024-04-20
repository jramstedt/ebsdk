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
 *	These are routines to set up the BT485 RAMDAC.
 *
 *  AUTHORS:
 *
 *      James Peacock
 *
 *
 *  CREATION DATE:      12-13-90 
 *
 *  MODIFICATION HISTORY:
 *
 *	Eric Goehl	15-Sep-94	Turned off 2x clock doubler. This is just for clarity because this bit is ignored
 *					when running in Differential ECL clock mode is selected (cr34 set).
 *
 *	James Peacock   01-31-94	sync on green frea > 75 Mhz
 *	
 *	James Peacock	12-13-90	Port from LCG
 *
 *
 *--
*/

#include  "cp$src:platform.h"
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
#include  "cp$src:kernel_def.h"
#include  "cp$src:dynamic_def.h"
#include  "cp$src:ev_def.h"
#include  "cp$src:msg_def.h"


#define   SIZE_DMA_BUFFER     (DMA_BYTES+64)
#define   LW_SIZE_DMA_BUFFER  (SIZE_DMA_BUFFER/4)
#define   MSWAIT 100000

void      pci_outmem    (unsigned __int64 address, unsigned int value, int length);
unsigned  int pci_inmem_byte (unsigned __int64 address, int length);
unsigned  int pci_inmem      (unsigned __int64 address, int length);

#define   PCI_byte_mask 0
#define   PCI_word_mask 1
#define   PCI_long_mask 3

#define   sfbp$k_rs_cmdreg3 sfbp$k_rs_status
#define   TgaToyOffset 0x16

extern	  int TestDmaPtr;
int	  rtc_read (int offset);

enum      on_types
         {
	 Enabled,
	 Disabled,
	 };


/*+
 * ===================================================
 * = sfbp$init_bt_485_driver - sfbp ramdac hookup =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine will connect ramdac routines.
  *    
 * FORM OF CALL:
 *  
 *     sfbp$init_bt_485_driver(scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr          - pointer to the scia 
 *      data_ptr 	  - pointer to the sfbp driver data area 
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *     This routine will initialize the sfbp driver data area,
 *     and return pointers to the functions that it supports. 
 * 
--*/
int	sfbp$init_bt_485_driver (SCIA *ptr,CCV *ccv_vector)

{
bt_func_ptr->pv_bt_init            = sfbp$$bt_485_init ;
bt_func_ptr->pv_bt_write_color     = sfbp$$bt_485_write_color;
bt_func_ptr->pv_bt_write_reg	   = sfbp$$bt_485_write_reg;
bt_func_ptr->pv_bt_load_332_color  = sfbp$$bt_485_load_332_color;
bt_func_ptr->pv_bt_pos_cursor      = sfbp$$bt_485_pos_cursor;
bt_func_ptr->pv_bt_cursor_ram      = sfbp$$bt_485_cursor_ram;
bt_func_ptr->pv_bt_wait_interrupt  = sfbp$$bt_wait_485_interrupt;
#if FULL_TGA
bt_func_ptr->pv_bt_write_cur_color = sfbp$$bt_485_write_cur_color;
bt_func_ptr->pv_bt_alloc_color     = sfbp$$bt_485_alloc_color;
bt_func_ptr->pv_bt_free_color      = sfbp$$bt_485_free_color;
bt_func_ptr->pv_bt_free_all_color  = sfbp$$bt_485_free_all_color;
bt_func_ptr->pv_bt_read_reg	   = sfbp$$bt_485_read_reg;
bt_func_ptr->pv_bt_read_color      = sfbp$$bt_485_read_color;
bt_func_ptr->pv_bt_read_cur_color  = sfbp$$bt_485_read_cur_color;
bt_func_ptr->pv_bt_load_888_color  = sfbp$$bt_485_load_888_color;
bt_func_ptr->pv_bt_dma_checksum    = sfbp$$bt_485_dma_checksum;
#endif

return(SS$K_SUCCESS);
}



/*+
 * ===================================================
 * = sfbp$$bt_485_init - BT485 RAMDAC Initialization =
 * ===================================================
 *
 *   
 * OVERVIEW:                                          
 *  	Initialize the BT485 RAMDAC for driver operation.
 *  
 * FORM OF CALL:
 *     sfbp$$bt_485_init (scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr	- shared console driver ram pointer
 *	data_ptr	- driver data area pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	This is used to initialize the brooktree 485 RAMDAC, with 
 *	the exception of the color palette ram which is done during
 *	the vertical retrace period. I separated the two modules
 *	because we really only need to load color data during 
 *	the vertical retrace.
 *
 *      Note that the BT485 automatically increments it's address
 *      register when a read or write it done to one of the 
 *      command registers (c0-c1: 10). So, all of these routines
 *      were designed to achieve some type of autonomy and functionality
 *      that could be shared among routines. So the specific routines
 *      in all cases where there is bit modification do a RMW to a
 *      local variable, which is then written to the command register
 *      address previously loaded into the address register.
 *
--*/
int	sfbp$$bt_485_init (SCIA *scia_ptr,SFBP *data_ptr)

{
int	 i,s=1;
static   struct   bt_485_init_type {
        int	(*rtn)();				
 	}bt_485_table[]= 
	{
        sfbp$$bt_485_set_cmd_reg0    	      ,
        sfbp$$bt_485_color                    ,
        sfbp$$bt_485_cursor_ram               ,
	};

 for (i=0;i*sizeof(struct bt_485_init_type )<sizeof (bt_485_table);i++)
        (*bt_485_table[i].rtn)(scia_ptr,data_ptr);

 sfbp$$bt_485_pos_cursor (scia_ptr,data_ptr,
        data_ptr->sfbp$l_cursor_row,
        data_ptr->sfbp$l_cursor_column,FALSE);

return (s);
}


/*+
 * ===================================================
 * = sfbp$$bt_485_set_cmd_reg0 - Set up Command Register 0 =
 * ===================================================
 *
 * OVERVIEW:
 *     set up command register 0
 *  
 * FORM OF CALL:
 *  	sfbp$$bt_485_set_cmd_reg0 (scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr	- scia location pointer
 *	data_ptr	- driver data area pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	This is used to load command register zero. 
 *
--*/
int	sfbp$$bt_485_set_cmd_reg0(SCIA *scia_ptr,SFBP *data_ptr)

{
#define GreenSyncEnable 8
int	data,csr; 
             
 /* Access CR3,IRE, and 8 bit operation		*/
             
  csr = 0xA2;
             
 /* Check bit for sync on green			*/
 /* little endian bit for this			*/             
 /* If bit on---then sync on			*/

  data   = rtc_read (TgaToyOffset);
  data  &= (1<<data_ptr->sfbp$l_slot_number);
  csr   |= data ? GreenSyncEnable : 0;
  
  sfbp$$bt_485_write_reg(scia_ptr,sfbp$k_rs_cmd_reg0,csr);

/* 4:1 multiplexing				*/
  sfbp$$bt_485_write_reg(scia_ptr,sfbp$k_rs_cmd_reg1,0x40);
                                               
/* Non-masked input, contiguous, 2 color/Highlight cursor , pclk1 selected.*/
  sfbp$$bt_485_write_reg(scia_ptr,sfbp$k_rs_cmd_reg2,0x35);
                                               
/* Enable all Planes			       	*/
  sfbp$$bt_485_write_reg(scia_ptr,sfbp$k_rs_pixel_mask,0xff);
                                               
return (SS$K_SUCCESS);                         
}                                              

/*+                                           
 * ===================================================
 * = sfbp$$bt_485_color - sets up the sfbp color data =
 * ===================================================
 *                                             
 * OVERVIEW:                                   
 *                                             
 *     This routine initializes the BT485 color palette.
 *                                             
 * FORM OF CALL:                               
 *                                             
 *     sfbp$$bt_485_color (scia_ptr,data_ptr); 
 *                                             
 * ARGUMENTS:                                  
 *	scia_ptr   - pointer to the sfbp shared area
 *      data_ptr   - pointer to the sfbp driver data area
 *                                             
 * FUNCTIONAL DESCRIPTION:                     
 *                                             
--*/                                           
                                               
int sfbp$$bt_485_color (SCIA *scia_ptr,SFBP *data_ptr)
{                                              
register int	i;                             
static char   colors[2][3]=
        {       
        0x00,0x00,0x00,                                 /* Black        */
        0xff,0xff,0xff,                                 /* White        */
        };

for (i=0;i<256;i++)
        sfbp$$bt_485_write_color  (scia_ptr,i,colors[0]);                   

return (SS$K_SUCCESS);
}

/*+
 * ===================================================
 * = sfbp$$bt_485_cursor_ram - Set up the BT485 cursor =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine initializes the BT485 cursor.
  *    
 * FORM OF CALL:    
 *  
 *     sfbp$$bt_485_cursor_ram (scia_ptr,data_ptr);
 *
 * ARGUMENTS:
 *	scia_ptr   - pointer to the sfbp shared area
 *      data_ptr   - pointer to the sfbp driver data area
 *
 * FUNCTIONAL DESCRIPTION:
 *	Fill in the 485 cursor ram
--*/

int sfbp$$bt_485_cursor_ram (SCIA *scia_ptr,SFBP *data_ptr)

{                 
register int      i,j,k,plane,cmdreg3,msb,temp1,temp2;
register int      ecl;
                  
static char   colors[2][3]=
        {         
        0x00,0x00,0x00,                                                         /* Black        */
        0xff,0xff,0xff,                                                         /* White        */
        };        
                  
 /* Indirect Access to Command Reg 3		*/
 sfbp$$bt_485_write_reg (scia_ptr,sfbp$k_rs_addr_pallete_write,1);
                  
 /* 485 has a reserved bit for ecl clock	*/
 /* By Default it is on				*/
                  
  ecl  = 0x10;    

 cmdreg3 = ecl | 0x4;
 sfbp$$bt_485_write_reg(scia_ptr,sfbp$k_rs_cmdreg3,cmdreg3);
                  
 /* Initialize 10 bit counter to zero			*/ 
 (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_addr_pallete_write,0);

 /* Clear all to zero					*/
 for (i=0;i<sfbp$K_SIZE_CURSOR_RAM;i++)                                         
	sfbp$$bt_485_write_reg (scia_ptr,sfbp$k_rs_cursor_ram,0x00);

 /* Initialize 10 bit counter to zero			*/ 
 (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_addr_pallete_write,1);
 (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_cmdreg3,cmdreg3);
 (*bt_func_ptr->pv_bt_write_reg) (scia_ptr,sfbp$k_rs_addr_pallete_write,0);

 /* 00 is pallette location 0 which is black		*/
 /* 11 is cursor color 3 				*/
 /* cursor ram is 64x8 bytes for each plane		*/
 /* 1 bit per pixel so 1 byte wide for font		*/ 
 for (plane=0;plane<2;plane++)
  for (i=0;i<64;i++)
   for (j=0;j<8;j++)                                		                 	
     {       
     if ((i>13 && i<16 && j==0))
 	  sfbp$$bt_485_write_reg (scia_ptr,sfbp$k_rs_cursor_ram,0xff);
     else sfbp$$bt_485_write_reg (scia_ptr,sfbp$k_rs_cursor_ram,0x00);
     }

 sfbp$$bt_485_write_cur_color  (scia_ptr,1,colors[0]);           	/* 0101 55 = color 1 = black	*/
 sfbp$$bt_485_write_cur_color  (scia_ptr,2,colors[0]);                  /* 1010 AA = color 2 = black	*/
 sfbp$$bt_485_write_cur_color  (scia_ptr,3,colors[1]);                  /* 1111 FF = color 3 = white	*/ 

 return (SS$K_SUCCESS);
}

/*+
 * ===================================================
 * = sfbp$$bt_485_pos_cursor - pos the BT485 cursor =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine poss the BT485 cursor.
  *    
 * FORM OF CALL:
 *  
 *     sfbp$$bt_485_pos_cursor (scia_ptr,data_ptr);
 *
 * ARGUMENTS:
 *	scia_ptr   - pointer to the sfbp shared area
 *      data_ptr   - pointer to the sfbp driver data area
 *      x          - use this position
 *      y          - use this position
 *
 * FUNCTIONAL DESCRIPTION:
 *	This routine is used to set pos the cursor in the BT485.
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
--*/

int sfbp$$bt_485_pos_cursor (SCIA *scia_ptr,SFBP *data_ptr,int row,int column,int bypixel)

{
register   int  xpos,ypos;

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

if (bypixel)printf ("cursor x %d xpos %d cursor y %d ypos %d \n",
		data_ptr->sfbp$r_mon_data.sfbp$l_cursor_x,xpos,
		data_ptr->sfbp$r_mon_data.sfbp$l_cursor_y,ypos);

sfbp$$bt_485_write_reg (scia_ptr,sfbp$k_rs_cursor_x_lo ,xpos&0xff);
sfbp$$bt_485_write_reg (scia_ptr,sfbp$k_rs_cursor_x_hi ,xpos>>8);				
sfbp$$bt_485_write_reg (scia_ptr,sfbp$k_rs_cursor_y_lo ,ypos&0xff);
sfbp$$bt_485_write_reg (scia_ptr,sfbp$k_rs_cursor_y_hi ,ypos>>8);				

return (SS$K_SUCCESS);

}


/*+
 * ===================================================
 * = sfbp$$bt_485_write_reg - Write data to BT485 register =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Load data into BT485 register
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_485_write_reg (scia_ptr,address,data );
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver pointer
 *	address  - BT485 register
 *	data	 - data to load into register
 *
 * FUNCTIONAL DESCRIPTION:
 *	This will load the address register with the BT485 register.
 *	It will then write the data to the register. The BT485 device
 *	requires a 16 bit address register to be loaded with the address
 *	of the register to update. After this is done, then the next 
 *	write operation is data for the register. The sfbp decodes the
 *	addresses to determine if the sfbp address register is being written,
 *	internal register or color palette location.
 *
 *
--*/

int	sfbp$$bt_485_write_reg (SCIA *scia_ptr,int address,int data )

{
volatile register char	 *reg;
register int Setup,DacData;

/* Write  the control setup first       */
reg  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
reg += (sfbp$k_ramdac_setup );

/* Set up register gets RS3:RS0 data    */
/* Which is our address in this case    */

Setup = (address<<1)|RamdacWriteAccess;

sfbp$$bus_write (reg,Setup,LWMASK);

wbflush();

/* And then write the data              */
/* Must include the control in 15:8     */
/* This is TGA specific                 */

reg  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;

DacData = (((address<<1)|RamdacWriteAccess)<<8)|data;

sfbp$$bus_write (reg,DacData,LWMASK);

wbflush();

return (SS$K_SUCCESS );

}



/*+
 * ===================================================
 * = sfbp$$bt_485_write_color - Write BT485 Color Palette =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Write Read Color Palette RAM in BT485
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_485_write_color (scia_ptr,address,color)
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address  - relative address of color palette ram location (0 to 256).
 *	color    - rgb array
 *
 * FUNCTIONAL DESCRIPTION:
 *	The color palette RAM is 256 x 24 bits deep and represents
 *	256 possible colors that the BT485 can produce on the video 
 *	monitor. Each entry is 24 bits wide, 8 bits for RED,8 bits for
 *	GREEN and 8 Bits for Blue. To write to color palette RAM,
 *	all we need to do is write to location 0000-00FF with C1:C0 set
 *	to 11, which is done by the sfbp decode logic. The address register
 *	is loaded with the color palette ram location. The BT485 then expects
 *	three successive write cycles 8 bits RED, 8 Bits GREEN and then 8 bits
 *	of BLUE. The address register is automatically incremented to the 
 *	next color palette location following the blue write cycle. A color is
 *	maximum intensity when set to FF and minimum intensity (black) when set
 *	to 0.
 *              This routine is fine to call for a few entries. If you are 
 *      loading the entire color ram, then use a separate routine.
 *
--*/

int	sfbp$$bt_485_write_color (SCIA *scia_ptr,int address,char color[])

{
volatile register   char    *c;
register int i;

/* Write  the control setup first       */
c  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
c += (sfbp$k_ramdac_setup );
sfbp$$bus_write (c,(sfbp$k_rs_addr_pallete_write<<1)|RamdacWriteAccess,LWMASK);

wbflush();

/* And then write the palette location  into address register */
c  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
sfbp$$bus_write (c, (((sfbp$k_rs_addr_pallete_write<<1)|RamdacWriteAccess)<<8)|address,LWMASK);
 
wbflush();

/* Write  the control setup first       */
c  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
c += (sfbp$k_ramdac_setup );

/* Set up to do palette writes  */
sfbp$$bus_write (c,(sfbp$k_rs_pallete_ram<<1)|RamdacWriteAccess,LWMASK);

wbflush();

c  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
for (i=0;i<3;i++)
 {
 sfbp$$bus_write (c,(((sfbp$k_rs_pallete_ram<<1)|RamdacWriteAccess)<<8)|color[i]&0xff,LWMASK);
 wbflush();
 }

return (SS$K_SUCCESS);
}



/*+
 * ===================================================
 * = sfbp$$bt_485_write_cur_color - Write BT485 Cursor Color Palette =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Write Cursor Color Palette RAM in BT485
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_485_write_cur_color (scia_ptr,address,color)
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address  - relative address of color palette ram location (0 to 256).
 *	color    - rgb array
 *
 * FUNCTIONAL DESCRIPTION:
 *      Load cursor color register with specified rgb value
 *
--*/

int	sfbp$$bt_485_write_cur_color (SCIA *scia_ptr,int address,char color[])

{
volatile register   char	 *c;
register int  i;

/* Write  the control setup first       */
c  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
c += (sfbp$k_ramdac_setup );
sfbp$$bus_write (c,(sfbp$k_rs_addr_cur_color_write<<1)|RamdacWriteAccess,LWMASK);
wbflush();

/* And then write address of cursor color */
/* Which we want to write to              */
/* Into the address register              */
c  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
sfbp$$bus_write (c,(((sfbp$k_rs_addr_cur_color_write<<1)|RamdacWriteAccess)<<8)|address,LWMASK);
wbflush();

/* Write  the control setup first       */
c  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
c += (sfbp$k_ramdac_setup );
sfbp$$bus_write (c,(sfbp$k_rs_cursor_data<<1)|RamdacWriteAccess,LWMASK);

wbflush();

/* And then write the data              */
/* For the RGB cursor now               */
c  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;

for (i=0;i<3;i++)
 {
 sfbp$$bus_write (c,(((sfbp$k_rs_cursor_data<<1)|RamdacWriteAccess)<<8)|color[i]&0xff,LWMASK);
 wbflush();
 }

return (SS$K_SUCCESS);
}


#if FULL_TGA
/*+
* ===========================================================================
* = sfbp$$bt_485_alloc_color - Allocate a color =
* ===========================================================================
*
* OVERVIEW:
*       Allocate a color entry
* 
* FORM OF CALL:
*       sfbp$$bt_485_alloc_color (scia_ptr,data_ptr,color)
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
int     sfbp$$bt_485_alloc_color (SCIA *scia_ptr,SFBP *data_ptr,int *color)

{
int       s,done,i,j,k,index;
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
* = sfbp$$bt_485_free_all_color - Free all colors =
* ===========================================================================
*
* OVERVIEW:
*       Free a color entry
* 
* FORM OF CALL:
*       sfbp$$bt_485_free_all_color (scia_ptr,data_ptr)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*
* FUNCTIONAL DESCRIPTION:
*       Free all colors 
*
--*/
int     sfbp$$bt_485_free_all_color (SCIA *scia_ptr,SFBP *data_ptr)

{
int     i;

for (i=0;i<MAXCOLORS;i++)data_ptr->sfbp$l_color[i]=0;

return (SS$K_SUCCESS);
}



/*+
* ===========================================================================
* = sfbp$$bt_485_free_color - Free a color =
* ===========================================================================
*
* OVERVIEW:
*       Free a color entry
* 
* FORM OF CALL:
*       sfbp$$bt_485_free_color (scia_ptr,data_ptr,color)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*       color           - entry index pointer 
*
* FUNCTIONAL DESCRIPTION:
*       Free a previously allocated color map entry.
*
* ALGORITHM:
*       - Get the longword index
*       - Get the offset within longword 
*       - Clear that bit position.
*
--*/
int     sfbp$$bt_485_free_color (SCIA *scia_ptr,SFBP *data_ptr,int color)

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
* = sfbp$$bt_485_load_332_color - Load Default Color Map =
* ===========================================================================
*
* OVERVIEW:
*       Load the default Color Map Entry 
* 
* FORM OF CALL:
*       sfbp$$bt_485_load_332_color (scia_ptr,data_ptr)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*
* FUNCTIONAL DESCRIPTION:
*      
*
--*/
int     sfbp$$bt_485_load_332_color (SCIA *scia_ptr,SFBP *data_ptr)

{
int     i,j,k,cnt,s=1,ColorMapSize,color_index;
char    entry_color[3];

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
        
cnt            = 0;

for (i=0;i<256;i++)
     {
     /* RGB format with blue on low end and red up top  */
 
     entry_color[0] = (i&(7<<5)) ? red_table[((i&(7<<5))>>5)] : 0;
     entry_color[1] = (i&(7<<2)) ? green_table[((i&(7<<2))>>2)] : 0;
     entry_color[2] = (i&(3<<0)) ? blue_table [((i&(3<<0))>>0)] : 0;

     (*bt_func_ptr->pv_bt_write_color)(scia_ptr,i,entry_color);
     }

return (s);
}



#if FULL_TGA
/*+
 * ===================================================
 *  Read data from BT485 register =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Read data from BT485 register
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_485_read_reg (scia_ptr,address);
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address	 - (int *)BT485 register
 *
 * FUNCTIONAL DESCRIPTION:
 *	This will load the address register with the BT485 register.
 *	It will then write the data to the register.
 *
--*/
int	sfbp$$bt_485_read_reg (SCIA *scia_ptr,int address )

{
char     data;
volatile register   char	 *reg;
register int DacData,Setup;

/* Write  the control setup first       */
reg  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
reg += (sfbp$k_ramdac_setup );

Setup = (address<<1)|RamdacReadAccess;
sfbp$$bus_write (reg,Setup,LWMASK);

wbflush ();

/* And then read the data              */
reg  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;

data   = ((sfbp$$bus_read (reg))>>Dac0ReadData)&0xff;

return (data);
}
#endif

/*+
* ===========================================================================
* = sfbp$$bt_wait_485_interrupt - Test the interrupts to CPU =
* ===========================================================================
*
* OVERVIEW:
*       Test for a posted interrupt
* 
* FORM OF CALL:
*       sfbp$$bt_wait_485_nterrupt (scia_ptr, data_ptr,load,pixel,value)
*
* RETURNS:
*       1 for interrupt posted else 0 for no interrupt
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data pointer
*       load            - true if loading seed data for next sample
*       pixel           - which pixel to use (0,1,2, or 3)
*       value           - which interrupt bit to wait for
*
* FUNCTIONAL DESCRIPTION:
*       This routine is used to wait for an interrupt. If we are
*       not doing signature testing, then we wait for a single
*       interrupt. If we are doing signature testing, which is
*       timing sensitive, then we must wait for the end of frame
*       interrupt before reading the signature registers. I read
*       these registers and then save them in the driver data 
*       area.
*
--*/

int     sfbp$$bt_wait_485_interrupt  (SCIA *scia_ptr,SFBP *data_ptr,int signature,int pixel,INT_TYPE value1,int input)

{
register int   i,j,s=1;

       {
       for (j=0;j<10000;j++)
                {
		if ( (*gfx_func_ptr->pv_test_int) 
                                (scia_ptr,data_ptr,value1))break;
		krn$_micro_delay (10);                                                          
		}
       s = (j==10000) ? 0 : 1;
       }

return (s);

}




#if FULL_TGA
/*+
* ===========================================================================
* = sfbp$$bt_485_dma_checksum - Verify the data =
* ===========================================================================
*
* FUNCTIONAL DESCRIPTION:
*       DMA write the screen of information and then save it. I am just trying
*       to verify the screen contents and that is all. I will have to rely on
*       the signature testing for the ramdac later.
*
--*/
int sfbp$$bt_485_dma_checksum (SCIA *scia_ptr, SFBP *data_ptr )

{
register  int  src=0,i,j=0,s=1;
volatile  register  int *vsrc,*BufferPtr,*EndBufferPtr;
register  int bytes,EndAddress,SourceVisual,DestVisual;
register  int DmaBufferPtr,XorCheckSum,Data;
int       LwCount,ByteLane0=0,ByteLane1=0,ByteLane2=0,ByteLane3=0;

 bytes          = (1284*1024*data_ptr->sfbp$l_bytes_per_pixel),
 vsrc           = (int *)sfbp_ioseg_ptr->sfbp$a_vram_mem_ptr;
 vsrc          += (data_ptr->sfbp$l_console_offset/4);
 SourceVisual   = data_ptr->sfbp$l_module == hx_8_plane ? SourceVisualPacked: SourceVisualTrue;
 DestVisual     = data_ptr->sfbp$l_module == hx_8_plane ? DestVisualPacked  : DestVisualTrue;
 EndAddress     = bytes;

 DmaBufferPtr   = (int )TestDmaPtr;

 EndBufferPtr   = (int *)DmaBufferPtr;
 BufferPtr      = (int *)DmaBufferPtr;
 LwCount	= DMA_BYTES/4;
 EndBufferPtr  += LwCount;
 XorCheckSum    = 0;
 
 do  
 {
   BufferPtr  = (int *)DmaBufferPtr;
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

  return (XorCheckSum);

}



/*+
* ===========================================================================
* = sfbp$$bt_485_load_888_color - Load Default Color Map =
* ===========================================================================
*
* OVERVIEW:
*       Load the default Color Map Entry 
* 
* FORM OF CALL:
*       sfbp$$bt_485_load_888_color (scia_ptr,data_ptr)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*
* FUNCTIONAL DESCRIPTION:
*
--*/
int     sfbp$$bt_485_load_888_color (SCIA *scia_ptr,SFBP *data_ptr,int color[])

{
int      i,s=1,color_index;
 char entry_color[3];
        
/* color[0] is red, color[1] green and color[2] is blue */
/* this allows us to have either scale                  */

for (i=0;i<256;i++)
   {
    entry_color[0] = color[0] ? i : 0;
    entry_color[1] = color[1] ? i : 0;
    entry_color[2] = color[2] ? i : 0;
    (*bt_func_ptr->pv_bt_write_color)(scia_ptr,i,entry_color);
   }  
  
return (s);
}



/*+
 * ===================================================
 * = sfbp$$bt_485_read_ovl_color - Read BT485 overlay Color Palette =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Read overlay Color Palette RAM in BT485
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_485_read_ovl_color (scia_ptr,address,color)
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address  - address of color palette ram location.
 *	color    - red,green and blue array pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *      Read the overlay color registers. These are 24 bit registers.
 *
 *
--*/

void	sfbp$$bt_485_read_ovl_color (SCIA *scia_ptr,int address,char color[])

{
}



/*+
 * ===================================================
 * = sfbp$$bt_485_read_color - Read BT485 Color Palette =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Read Color Palette RAM in BT485
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_485_read_color (scia_ptr,address,color)
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver interface pointer
 *	address  - address of color palette ram location.
 *	color    - red,green and blue array pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *	The color palette RAM is 256 x 24 bits deep and represents
 *	256 possible colors that the BT485 can produce on the video 
 *	monitor. Each entry is 24 bits wide, 8 bits for RED,8 bits for
 *	GREEN and 8 Bits for Blue. To write to color palette RAM,
 *	all we need to do is write to location 0000-00FF with C1:C0 set
 *	to 11, which is done by the sfbp decode logic. The address register
 *	is loaded with the color palette ram location. The BT485 then expects
 *	three successive write cycles 8 bits RED, 8 Bits GREEN and then 8 bits
 *	of BLUE. The address register is automatically incremented to the 
 *	next color palette location following the blue write cycle. A color is
 *	maximum intensity when set to FF and minimum intensity (black) when set
 *	to 0.
 *
 --*/
 
int	sfbp$$bt_485_read_color (SCIA *scia_ptr,int address,char color[])

{
volatile register char	*r,*g,*b;

/* Write  the control setup first       */
r  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
r += (sfbp$k_ramdac_setup );
sfbp$$bus_write (r,(sfbp$k_rs_addr_pallete_read<<1)|RamdacWriteAccess,LWMASK);
wbflush();

/* And then write the palette location  into address register */
r  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
sfbp$$bus_write (r, (((sfbp$k_rs_addr_pallete_read<<1)|RamdacWriteAccess)<<8)|address,LWMASK);
wbflush();

/* Now set up for pallette read		*/
/* Write  the control setup first       */
r  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
r += (sfbp$k_ramdac_setup );

/* Set up to do palette reads 		*/
sfbp$$bus_write (r,(sfbp$k_rs_pallete_ram<<1)|RamdacReadAccess,LWMASK);
wbflush ();

/* And then read the data              */
r  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;

g = b = r;

color[0] = (sfbp$$bus_read (r)>>Dac0ReadData)&0xff;
color[1] = (sfbp$$bus_read (g)>>Dac0ReadData)&0xff;
color[2] = (sfbp$$bus_read (b)>>Dac0ReadData)&0xff;

return (SS$K_SUCCESS);
}


/*+
 * ===================================================
 * = sfbp$$bt_485_read_cur_color - Read BT485 Cursor Color Palette =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Read Cursor Color Palette RAM in BT485
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bt_485_read_cur_color (scia_ptr,address,color)
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

int	sfbp$$bt_485_read_cur_color (SCIA *scia_ptr,int address,char color[] )

{
volatile register   char	*r,*g,*b;

/* Write  the control setup first       */
r  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
r += (sfbp$k_ramdac_setup );
sfbp$$bus_write (r,(((sfbp$k_rs_addr_cur_color_read<<1)|RamdacWriteAccess)<<8),LWMASK);
wbflush ();

/* And then write address of cursor color */
/* Which we want to read  to              */
/* Into the address register              */
r  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;
sfbp$$bus_write (r,(((sfbp$k_rs_addr_cur_color_read<<1)|RamdacWriteAccess)<<8)|address,LWMASK);
wbflush();

/* Write  the control setup first       */
r  = (char *)sfbp_ioseg_ptr->sfbp$a_reg_space;
r += (sfbp$k_ramdac_setup );
sfbp$$bus_write (r,(sfbp$k_rs_cursor_data<<1)|RamdacReadAccess,LWMASK);
wbflush ();

/* And then read the data              */
g = b = r  = (char *)sfbp_ioseg_ptr->sfbp$a_bt_space;

color[0] = (sfbp$$bus_read (r)>>Dac0ReadData)&0xff;
color[1] = (sfbp$$bus_read (g)>>Dac0ReadData)&0xff;
color[2] = (sfbp$$bus_read (b)>>Dac0ReadData)&0xff;

return (SS$K_SUCCESS);
}
#endif
