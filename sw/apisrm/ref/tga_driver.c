#define USENEW      1
#define ATI9X16     1
#define USEATIFONTS 1
#define DEC_TGA	0x00041011

/* file:	tga_driver.c
 *
 * Copyright (C) 1993 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.           
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *               
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Avanti Firmware
 *
 *  ABSTRACT: TGA Video Driver
 *
 *	Video driver for the TGA graphics accelerator (DECchip 21030),
 *	a PCI-based 2D and 3D graphics rendering chip.
 *
 *  AUTHORS:
 *
 *	Jim Peacock
 *
 *  CREATION DATE:
 *  
 *	25-feb-1993
 *
 *  BUGS:
 *
 *  MODIFICATION HISTORY:
 *
 *	jje	03-Apr-1996	Conditionalize for Cortex.
 *	
 *	dtr	24-jul-1995	merged console selection into one
 *	
 *	swt	28-Jun-1995	Add Noritake platform.
 *
 *	rbb	13-Mar-1995	Conditionalize for the EB164
 *
 * 	egg	04-Oct-1994	Modifying tga_scroll() to scroll a little at
 *				a time.
 *	egg	14-Sep-1994	tga_reinit'ing all TGA's present in the system
 *                              then restablish console tga connection.
 *	dwb	08-Aug-1994	Change tga_reinit to optionaly clear the 
 *				screen
 *
 *--
 */

/* $INCLUDE_OPTIONS$ */
#include	"cp$src:platform.h"
/* $INCLUDE_OPTIONS_END$ */
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include 	"cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:tt_def.h"
#include 	"cp$src:ev_def.h"
#include 	"cp$src:time.h"		
#include 	"cp$src:pb_def.h"
#include        "cp$src:eisa.h"   
#include	"cp$inc:platform_io.h"
#include	"cp$src:display.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:vgafont.h"
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
#include 	"cp$src:tga_sfbp_data_ptr.h"
#include 	"cp$src:tga_sfbp_data.h"
#if MEDULLA
#include        "cp$src:8530_def.h"
#endif

extern char ATI_FONT_TABLE[];   
extern unsigned __int64 cb_time_limit;
extern int cbip;
extern struct pb *console_pb;
extern struct TTPB null_ttpb;
extern struct TTPB *console_ttpb;
extern int graphics_console;

#include "cp$src:ansi_def.h"
#include "cp$src:alpha_arc.h"
#include "cp$src:chstate.h"
#include "cp$src:vgapb.h"
 
int  tga_read      (struct FILE *fp, int size, int number, unsigned char *s );
int  tga_open      (struct FILE *fp, char *info, char *next, char *mode );
int  tga_setmode   (int mode);
int  tga_close     (struct FILE *fp );
int  tga_txoff 	   ( int dummy ) ;
int  tga_txon 	   ( struct VGA_PB *tpb);
int tga_txready (struct VGA_PB *tpb);
void tga_scroll (struct CH_STATE *chs);
void  tga_txsend   (struct VGA_PB *tpb, char c) ;
void tga_software_init (struct CH_STATE *chs,int row,int col);
int  null_procedure ();
                                                       
#if AVANTI
#include    	"cp$src:pci_size_config.h"
#endif
                  
extern 	int spl_kernel;

#define NROW    25
#define NCOL    80
#define ROM_STRIDE 4
#define RAM_STRIDE 1

int	sim$g_bus,sim$g_print,sim$g_stdout;
int	sim$g_trace,sim$g_fileio;


int   tga_read_linear  (unsigned __int64 linear);
void  tga_write_linear (unsigned __int64 linear,unsigned int value);

/* These are constants used by the cfg_read/cfg_write code      */
#define   PCI_byte_mask 0
#define   PCI_word_mask 1
#define   PCI_long_mask 3

void     pci_outmem (unsigned __int64 address, unsigned int value, int length);
int      pci_inmem  (unsigned __int64 address, int length);

#if MEDULLA
#define EV_CONSOLE_TOY_OFFSET 0x17
int tga24m = 0;  
int TGA_rom_base[MaxTgaCount];
void check_valid_tga ();
int check_tga_int();
int vga_reinit(){}; /* dummy routine for kernel.c */
#endif 

struct   pb   *TGA_pb[MaxTgaCount];
SCIA     *TGA_scia_ptr[MaxTgaCount];
SFBP     *TGA_sfbp_ptr[MaxTgaCount];
IOSEG    *TGA_ioseg_ptr[MaxTgaCount];
int      TGA_mem_base[MaxTgaCount];
int	 ScrollDmaPtr[MaxTgaCount];
int	 TestDmaPtr[MaxTgaCount];
int      TGAInterruptCount[3]={0,0,0};
int      TgaVector[3]={0,0,0};
                
struct CH_STATE *tga_ch_state;

#if BACKING_STORE
int tga_console_size;
char *tga_console_buffer;
#endif

#define   SCROLL_SIZE	      (DMA_BYTES)
#define   SIZE_DMA_BUFFER     (SCROLL_SIZE+64)
#define   LW_SIZE_DMA_BUFFER  (SIZE_DMA_BUFFER/4)

#define	  SparseOffset	5
#define	  SparseStart	0x20000

int dyn$_malloc (int size, int option, int modulus, int remainder, struct ZONE *zone);

#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_FLOOD|DYN$K_NOOWN,0,0,0)

extern struct CTB_WS_table CTB_WS_tab[];

#define set_lin(addr, lin) \
    lin = SparseStart;  \
    lin = ( (lin << 16) | ( (addr<<SparseOffset) & 0xfffffffc ) );

#if MEDULLA
void tga_scc_interrupt (int slot);
#else
int tga_int_handler (int slot);
#endif
enum pci_slot_types 
	{
	PciSlot1,
	PciSlot2,
	PciSlot3,
	};
         
int	TgaCount=0;

void     TgaConnectKeyboard (struct CH_STATE *chs, int slot, int dev_slot );

#define DmaMallocSize (16384+1024)

                                                    
/*+                                                  
* ===========================================================================
* = tga_init -  Make TGA visible as a file unit =
* ===========================================================================
*
* OVERVIEW:
* 
* FORM OF CALL:
*       tga_init ()
*
* RETURNS:
*       none
*
* ARGUMENTS:
*
* GLOBALS:
*
* FUNCTIONAL DESCRIPTION:
*	This is the initial entry point into the driver. The kernel
*	will call this routine to initialize the path to the driver
*	by setting up the INODE for the device. We also have to
*	connect to the keyboard driver by setting up a function 
*   	block with the appropriate function addresses so the keyboard
*   	driver can dump out characters to the graphics head.
*   
* RETURN CODES:
*       none
*   
--*/ 
    
tga_init( )
    {
    int tga_init_pb( );

    TgaCount = 0;
    find_pb( "tga", 0, tga_init_pb );
    if( TgaCount )
    	{                 
    	TgaConsoleSlot = 0;
    	ConnectToTga( TgaConsoleSlot );

#if MEDULLA
        check_valid_tga ();
#endif 
    	cp_func_ptr->pv_init_output( scia_ptr, data_ptr );

    	tga_ch_state = malloc( sizeof( struct CH_STATE ) );
#if BACKING_STORE
	tga_console_size =
		data_ptr->sfbp$l_cursor_max_row *
		data_ptr->sfbp$l_cursor_max_column;
	tga_console_buffer = malloc( tga_console_size );
#endif                           
    	TgaConnectKeyboard( tga_ch_state,
    		TgaConsoleSlot, TGA_pb[TgaConsoleSlot]->slot );
           
    	tga_software_init( tga_ch_state, 0, 0 );
    	}

    return( msg_success );
    }
    
tga_init_pb( struct pb *pb )
    {
    void *p;
                 
    if( TgaCount < MaxTgaCount )
    	{
	if( TgaCount )
	    p = ScrollDmaPtr[0];
	else
	    p = malloc( DmaMallocSize );
    	TGA_pb[TgaCount] = pb;
    	TGA_sfbp_ptr[TgaCount] = malloc( sizeof( SFBP ) );
    	TGA_scia_ptr[TgaCount] = malloc( sizeof( SCIA ) );
    	TGA_ioseg_ptr[TgaCount] = malloc( sizeof( IOSEG ) );
    	ScrollDmaPtr[TgaCount] = p;
#if FULL_TGA
    	TestDmaPtr[TgaCount] = malloc( DmaMallocSize );
#endif
    	TgaVector[TgaCount] = pb->vector;
#if MEDULLA
        int_vector_set( pb->vector, tga_scc_interrupt, TgaCount );
#else
	int_vector_set( pb->vector, tga_int_handler, TgaCount );
#endif
	TGA_mem_base[TgaCount] = incfgl( pb, 0x10 ) & ~15;
	ConnectToTga( TgaCount );
	cp_func_ptr->pv_reset_output( scia_ptr, data_ptr );  
	TgaCount++;
	}
    }                                 
/*+                                                  
* ===========================================================================
* = tga_reinit -  Reinit. TGA hardware after returning from an OS.
* ===========================================================================
*   
* OVERVIEW: This routine puts the TGA module into a known state after
*   	returning from the OS (Shutdown).
*   
* FORM OF CALL:
*       tga_reinit ()
*             
* RETURNS:    
*       none  
*             
* ARGUMENTS:  
*   	erase_screen - true is desire is to erase the screen as well as
*   	reinit the hardware.
*           
* GLOBALS:  
*           
* FUNCTIONAL DESCRIPTION:
*	Its possible the OS  munged the TGA module or some associated pci
*	interrupt.  In any case, it is desirable to put the hardware into a 
* 	known state after the OS has had control.  This routine is being put
* 	in with the hope that a hang on init problem (after return from the OS)
*	will be corrected.
--*/                    
                        
int tga_reinit(int erase_screen )  
{                                       
    int cmr;
    int cmc;
    struct CH_STATE *chs;
    char *buffer;
    int r;
    int c;
    char ch;
    int i;                                  

    for (i=0; i<TgaCount; i++) {   /* ReInit all TGA's present. */           
	if ((incfgl (TGA_pb[i], 0x04) & 6) != 6)
	     continue;
	ConnectToTga (i);
#if BACKING_STORE
	(*cp_func_ptr->pv_reset_output) (scia_ptr, data_ptr);       
	if (i != TgaConsoleSlot)
	    continue;
	cmr = data_ptr->sfbp$l_cursor_max_row;
	cmc = data_ptr->sfbp$l_cursor_max_column;
	chs = tga_ch_state;       
	buffer = tga_console_buffer;
	for (r=0; r<cmr; r++) {   
	    for (c=0; c<cmc; c++) {
		ch = *buffer++;   
		if (ch && ch != ' ')
		    sfbp$putchar (chs, ch, 0, c, r);
	    }                     
	}                         
	sfbp$putchar (chs, 0, 0,  
		chs->ds.CursorXPosition, chs->ds.CursorYPosition);
#else                             
	if (0/*erase_screen*/) {                  
	    (*cp_func_ptr->pv_reset_output)(scia_ptr,data_ptr);
	} else {                              
	    (*cp_func_ptr->pv_init_output)(scia_ptr,data_ptr);       
	}                                                          
        tga_ch_state->ds.CursorXPosition = 0;
        tga_ch_state->ds.CursorYPosition = data_ptr->sfbp$l_cursor_max_row-1;
#endif                            
    }                                                            
    if (TgaCount)     /* Reconnect to console Tga if present. */
  	ConnectToTga(TgaConsoleSlot);
    return (0);
}                     
/*+                   
* ===========================================================================
* = ConnectToTga -  Connect the Tga in UnitNumber to the Tga Driver  =
* ===========================================================================
*                                                                      
* OVERVIEW:           
*                   
* FORM OF CALL: 
* PARAMETERS:   
*   	UnitNumber - zero based index for the TGA to be connected.  
*               
--*/                                                           
void ConnectToTga(int UnitNumber)
{                                
    
    data_ptr        = (SFBP  *)TGA_sfbp_ptr[UnitNumber];
    data_ptr->sfbp$l_foregroundvalue = sfbp$k_def_lw_white;   
/* This determines  the color of the cleared screen. */
    data_ptr->sfbp$l_backgroundvalue = sfbp$k_console_blue;   
    scia_ptr        = (SCIA  *)TGA_scia_ptr[UnitNumber];
    sfbp_ioseg_ptr  = (IOSEG *)TGA_ioseg_ptr[UnitNumber];
    cp_func_ptr     = (struct cp_func_block *)&scia_ptr->console_func;
    drv_desc_ptr    = (struct drv_desc   *)&scia_ptr->gfx_drv_desc;
    drv_desc_ptr->data_ptr   = (unsigned int *)data_ptr;
    drv_desc_ptr->ioseg_ptr  = (unsigned int *)sfbp_ioseg_ptr;
    gfx_func_ptr    = (struct gfx_func_block *)&data_ptr->sfbp$r_graphics_func;
    bt_func_ptr     = (struct bt_func_block *)&data_ptr->sfbp$r_bt_func;
    parse_func_ptr  = (struct parse_func_block *)&data_ptr->sfbp$r_parse_func;
                             
    tga_driver_init (scia_ptr,data_ptr,sfbp_ioseg_ptr,
    		&cp_function_block[UnitNumber],UnitNumber,ccv);           
                             
    control = (struct sfbp_control *)sfbp_ioseg_ptr->sfbp$a_reg_space;

}   
                     
/*+                   
* ===========================================================================
* = TgaConnectKeyBoard -  Connect to keyboard =
* ===========================================================================
*                     
* OVERVIEW:           
*   
* FORM OF CALL:
*   	TgaConnectKeyboard ()
*   
--*/                                                           
void TgaConnectKeyboard (struct CH_STATE *chs, int slot, int dev_slot )                  
 {                                                             
  int 	 i,argc;                                                 
  char **argv;                                                 
  char   device_name[10];
  struct TTPB *ttpb;
  struct VGA_PB *tpb;

  for (i=0;i<10;i++)device_name[i]=0;
                  
	/*Allocate the port block */
  tpb = (struct VGA_PB*)malloc(sizeof(struct VGA_PB));
                                                         
  /*Save the pointer to the state structure*/             
	tpb->chs = chs;                                 

  /* tga0,tgb0,tgc0....			   */                                                        
 
  sprintf( device_name, "tg%c0", slot + 'a' );

  /* Tell the keyboard driver how to dump characters	*/
  ttpb 	     	= tt_init_port(device_name);    
             
  ttpb->rxoff 	= null_procedure;
  ttpb->rxon 	= null_procedure;
  ttpb->rxready = null_procedure;
  ttpb->rxread 	= null_procedure;
                  
  ttpb->txoff 	= tga_txoff;
  ttpb->txon 	= tga_txon;
  ttpb->txready = tga_txready;
  ttpb->txsend 	= tga_txsend;
  ttpb->mode 	= DDB$K_INTERRUPT; 
  ttpb->port  	= tpb;
  tpb->ttpb     = ttpb;
                                      
  /* Release lock acquired by tt_init_port */
  spinunlock(&spl_kernel);
  
  if (graphics_console && (console_ttpb == &null_ttpb))
    {
    console_pb = TGA_pb[slot];
    console_ttpb = ttpb;
    fill_in_ctb_ws_slotinfo (PCI_BUS, TGA_pb[slot]->hose, TGA_pb[slot]->bus, TGA_pb[slot]->slot);
    }
}  

/*+
 * ============================================================================
 * = tga_software_init - Initialize the vga software.                         =
 * ============================================================================
 * 
 *  OVERVIEW:
 *                                                 
 *	This routine initializes the tga software data structures.
 *                                
 *  FORM OF CALL:                 
 *                                
 *	tga_software_init (chs, row, col );  
 *                                
 *  RETURNS:                      
 *                                
 *	None.                     
 *
 *  ARGUMENTS:
 *
 *	struct CH_STATE *chs - Character state structure.
 *	row		     - starting y 
 *   	col		     - starting x
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *      
 *--
 */

void tga_software_init (struct CH_STATE *chs,int x, int y)
{                     
                      
/*Initialize the display state*/
	memset (chs, 0, sizeof (struct CH_STATE));
	chs->ds.CursorXPosition = y;
	chs->ds.CursorYPosition = x;
	chs->ds.CursorMaxXPosition = data_ptr->sfbp$l_cursor_max_column -1;
	chs->ds.CursorMaxYPosition = data_ptr->sfbp$l_cursor_max_row -1;
/* Can't do this because chs stores only a uchar, we require 32 bits. */
	chs->ds.ForegroundColor = sfbp$k_def_lw_white;                  
	chs->ds.BackgroundColor = sfbp$k_console_blue;                   
/* Override the above 2 statements with the following. */
	data_ptr->sfbp$l_foregroundvalue = sfbp$k_def_lw_white;                   
	data_ptr->sfbp$l_backgroundvalue = sfbp$k_console_blue;   
	chs->ds.HighIntensity = OFF;
	chs->ds.UnderScored = OFF;
	chs->ds.ReverseVideo = OFF;
	chs->CurrentAttribute = NORMAL_VIDEO;
  	chs->putc = sfbp$putchar;
	chs->getc = null_procedure;
	chs->ioctl = null_procedure;
        chs->scroll_in_prog = 0;     
#if SABLE || MIKASA
	chs->scroll_jump = (chs->ds.CursorMaxYPosition + 1) / 8 - 1;
#endif
        chs->row = 0;     
        chs->col = 0;     

} 
                     

/*+  
* ===========================================================================
* = tga_txoff -  Transmit Off =
* ===========================================================================
*
* OVERVIEW:
* 
* FORM OF CALL:
*       tga_txoff ()
*
* RETURNS:
*       none
*
* ARGUMENTS:
*
* GLOBALS:
*
* FUNCTIONAL DESCRIPTION:
*
* RETURN CODES:
*       none
*
--*/ 

int tga_txoff ( int dummy ) 
{
return (1);
}
                  


/*+
* ===========================================================================
* = tga_txon -  TGA On =
* ===========================================================================
*
* OVERVIEW:
* 
* FORM OF CALL:
*       tga_txoff ()
*
* RETURNS:
*       none
*
* ARGUMENTS:
*
* GLOBALS:
*        
* FUNCTIONAL DESCRIPTION:
*	Character output must be interrupt driven from keyboard driver.
*        
* RETURN CODES:
*       none
*        
--*/     
         
int tga_txon ( struct VGA_PB *tpb) 
 {                               
 return (tt_tx_interrupt (tpb->ttpb));
 }


/*+
* ===========================================================================
* = tga_txready -  TGA Ready =
* ===========================================================================
*  
* OVERVIEW:  This function is supposed to get called before any character is
*	output to the tga.  It is responsible for calling the tga_scroll  
* 	routine -  that scrolls a line then returns - until its finished with
*	a screenful of data. 
*	The tga_scroll only does a little at a time to avoid the problem  
*	of the OS missing clock ticks during raised IPL lengthy callbacks.
*  
* FORM OF CALL:
*       tga_txready ()
*  
* RETURNS: 0 - Not ready to transmit more data - Still scrolling.
*          1 - Ready to transmit more data - no scrolling in progress. 
*                              
*                          
--*/              
int tga_txready (struct VGA_PB *tpb) {                        
       	if (tpb->chs->scroll_in_prog) {
	    tga_scroll (tpb->chs);
	    return 0;
	} else {
	    return 1;
	}
}


/*+
* ===========================================================================
* = tga_txsend -  Send =
* ===========================================================================
*   
* OVERVIEW:
*                             
* FORM OF CALL:
*       tga_txsend ()
*   
* RETURNS:
*       none
*   
* ARGUMENTS:
*   
* GLOBALS:  
*           
* FUNCTIONAL DESCRIPTION:
*   	Send one character.  Parse with Ansi parser, which in turn calls
*	tbp->chs->putc (which was set to sfbp$putchar).
*                                                          
* RETURN CODES:
*       none
*       
--*/    
void tga_txsend (struct VGA_PB *tpb, char c) 
                          
 {                
/*Put the character*/
	aputchar (tpb->chs,c); 
 }                          

/*+
* ===========================================================================
* = tga_scroll -  =
* ===========================================================================
*                 
* OVERVIEW: Scrolls the tga display up one row.     
*                                                                 
* FORM OF CALL:           
*       tga_scroll (struct CH_STATE *chs)    
*                                                                  
* RETURNS:                             
*       none                           
*                                      
* FUNCTIONAL DESCRIPTION:              
*  This routine has a slowest case scenario of only moving one scanline worth
*  of bitmapped during each call.  The general idea is that when a Callback
*  (from the Operating System) is in progress, we need to minimize the time
*  spent here so that not timer ticks are missed by the OS. The kernel manages
*  setting cb_time_limit and the cbip (callback in progress) flag.
*
*  If cbip is set, this routine checks the time against the cb_time_limit after
*  moving one scanline worth of data.  It returns time > cp_time_limit.
*
*  The OS must call txready before outputting characters. txready calls
*  this routine if the chs->scroll_in_prog flag is set. It is cleared when
*  a screenfull of data has been scrolled up one (character-size row).
*                                                                 
*                                                                 
--*/                                                              
void tga_scroll (struct CH_STATE *chs)                            
{                                                                   
int r,c,i,j;
char ch;                                                          
unsigned char attr;                                               
unsigned __int64 time;                                                        
                                                                  
/*Copy the screen up 1 row*/                                      
                                                                  
    	/*Initialize*/                                            
    	if (chs->scroll_in_prog) {                                
    	    r = chs->row;                                         
	    chs->scroll_in_prog = 0;
    	} else {                                                  
	    r = 0;                                                
	}                                                         
	i = 0;                                                    
	j = chs->scroll_jump + 1;
                                                                  
	/*Scroll one line*/                                       
	for (; r<data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines; r++) {
	  if (i && cbip) {   	/*Quit if we've done enough*/     
	    rscc (&time);                                         
	    if (time > cb_time_limit) {                           
	      chs->row = r;                                       
	      chs->scroll_in_prog = 1;                              
	      return;                                             
	    }                                          
	  }                                            
	  tga$$copyScanline(r,j);    /*Copy up one scanline of data*/ 
	  i++;    /*Show we've made progress*/                
	}                                                     

#if BACKING_STORE
	i = j * data_ptr->sfbp$l_cursor_max_column;
	memcpy (tga_console_buffer, tga_console_buffer + i, tga_console_size - i);
	memset (tga_console_buffer + tga_console_size - i, 0, i);
#endif
}                        
                         
/*+                      
 *===========================================================================
 *= tga$$copyScanline  -                                            
 *===========================================================================
 *                                                                  
-*/                                                                 
int	tga$$copyScanline ( int sourceScanline, int rowJump )
{                                                                   
register int num_bytes;                                             
register char *last_row_ptr,*first_row_ptr,*second_row_ptr;         
int bpr,bps,spr; /* bytes per row, bytes per scanline, scanlines per row */

 bpr = data_ptr->sfbp$l_bytes_per_row;
 bps = data_ptr->sfbp$l_bytes_per_scanline;
 spr = scia_ptr->font_height /* = bpr/bps; */;
 num_bytes     = bps;
 first_row_ptr = (char *)data_ptr->sfbp$l_console_offset;           
 first_row_ptr += sourceScanline*bps;   
 second_row_ptr= (char *)first_row_ptr + bpr*rowJump;
                                                                  
 if (sourceScanline<(data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines-spr))
   sfbp$$copy_video_memory (scia_ptr,data_ptr,second_row_ptr,
	    first_row_ptr,num_bytes);
 else                                   
   sfbp$$fill_video_memory (scia_ptr,data_ptr,first_row_ptr,    
            data_ptr->sfbp$l_backgroundvalue,num_bytes);
 return(SS$K_SUCCESS);                        
}                                             
                                   
                                  
#if FULL_TGA
/*+                                
 *===========================================================================
 *= gets  - Sub-Standard version of C library function
 *===========================================================================
 *            
-*/

char *gets (char *s) {     
	struct PCB *pcb;   
        int status;
                           
	pcb = getpcb();    
                           
        if (isatty (pcb->pcb$a_stdin))           
       	    status = read_with_prompt("",80, s, pcb->pcb$a_stdin,
      				      pcb->pcb$a_stdin, 1);
    	else                                  
            status = fgets(s, 80, pcb->pcb$a_stdin); 
        return status;
}                                            


/*+
 *===========================================================================
 *= GetTgaConsole - Get TGA Console Type 
 *===========================================================================
 *            
-*/

void GetTgaConsole (SFBP *data_ptr)
     {     
	struct PCB *pcb;   
                           
	pcb = getpcb();    
                           
        if (strcmp (pcb->pcb$a_stdin_name,"tga0")==0)           
       	    {
 	    data_ptr->sfbp$l_console_driver = TRUE;
 	    data_ptr->sfbp$l_alt_console    = FALSE;
	    }
       	 else
  	    {
 	    data_ptr->sfbp$l_console_driver = FALSE;
 	    data_ptr->sfbp$l_alt_console    = TRUE;
	    }

      }


/*+
 *===========================================================================
 *= time  - returns 8 bits worth of seconds.  Used for random seed.
 *===========================================================================
 *                   
-*/                  
                     
time_t time (time_t *tp) 
{                    
  struct toy_date_time p;
                     
  get_date_time(&p); 
  
  if (tp) *tp = p.seconds;
  return(p.seconds);          
  
}          
  

/*+                
 * ============================================================================
 * = tga_open - Connect with the TGA driver =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Open a connection to the TGA.
 *
 * FORM OF CALL:
 *
 *	tga_open (); 
 *
 * RETURNS:
 *
 *	None
 *   
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int tga_open( struct FILE *fp, char *info, char *next, char *mode )
 {
 int	i,argc,n[2];
 char	number [32];
 char	**argv;                                        
 
 /* Connect Up With Current COnsole Slot	*/
 tga_driver_init (scia_ptr,data_ptr,sfbp_ioseg_ptr,
 	&cp_function_block[TgaConsoleSlot],TgaConsoleSlot,ccv);
                      
 control = (struct sfbp_control *)sfbp_ioseg_ptr->sfbp$a_reg_space;
 
 (*cp_func_ptr->pv_reset_output)(scia_ptr,data_ptr);  
 
 if (info && *info) 
  {
  for (i=0; i<2; i++) 
   {
   strelement (info, i, "	 \r\n", number);
   n [i] = atoi (number);
   }
  
  /* Set up the file parameter information	*/
  *fp->offset = n [0] + n [1] * NCOL;
  
  /* Let the port driver know where cursor is	*/
  if (n[1] == 0){
    data_ptr->sfbp$l_cursor_row        = n[0];
    data_ptr->sfbp$l_cursor_column     = n[1];
  } 
  else {
    data_ptr->sfbp$l_cursor_row        = 0;
    data_ptr->sfbp$l_cursor_column     = 0;
  } 
  }  
  return (1);
 }


/*+
 * ============================================================================
 * = tga_read - read characters from the character cell map                   =
 * ============================================================================
 *
 * OVERVIEW:
 *                         
 *	This reads characters from the character cell map (REAL usefull).  A
 *	read starts at the current cursor position, and terminates when the
 *	end of the screen is encountered.
 *
 *	The cursor position is kept local to this file descriptor (implying
 *	simultaneous reads by different processes will not collide with each
 *	other).
 *
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should
 *			correspond to the TGA device.
 *	int size	size of item in bytes
 *	int number 	number of items.
 *	char *buf	Address to write the data to.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes read, or 0 on error.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *
 *--
 */

int tga_read( struct FILE *fp, int size, int number, unsigned char *s )

{
int	i,len;
int	data,video_addr;
	
 len = size * number;

 for (i=0;i<len;i++) 
  {
  if (!inrange (*fp->offset, 0, fp->ip->len[0]))break;
 
  video_addr  = sfbp_ioseg_ptr->sfbp$a_vram_mem_ptr ;

  video_addr += (*fp->offset);
 
  *s++ = pci_inmem ( video_addr , PCI_byte_mask);
 
  *fp->offset += 1;	   
  }

 return (i);
	
}              
               


/*+
 * ============================================================================
 * = tga_close - Close Connection with TGA driver =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Close a connection to the TGA.
 *
 * FORM OF CALL:
 *
 *	tga_close (); 
 *
 * RETURNS:
 *
 *	None
 *   
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int tga_close( struct FILE *fp )
 {
 return (1);
 }
#endif

/*+
* ===========================================================================
* = tga_driver_init - sfbp console port driver initialization. =
* ===========================================================================
*
* OVERVIEW:
*       Initialize the sfbp for use by the console port driver.
* 
* FORM OF CALL:
*
*       tga_driver_init (scia_ptr,data_ptr,ioseg_ptr,cp_function,slot, vector)
*
*
* RETURNS:
*       0  : If successfull
*       -1 : if unsucessfull
*
* ARGUMENTS:
*       scia_ptr     - scia pointer
*       data_ptr     - driver data area pointer
*       ioseg_ptr    - IO Segment pointer
*       cp_function  - Console Program Function Block Pointer
*       slot         - this is the current slot the sfbp is plugged into.
*       vector       - this is the callback vector.
*   
* GLOBALS:
*       none.
*   
* FUNCTIONAL DESCRIPTION:
*   
* RETURN CODES:
*       0 for success.
*   
* CALLS:
*   
* SIDE EFFECTS:
*       video subsystem is initialized.
*   
* ALGORITHM:
*       
--*/
int tga_driver_init (SCIA *scia_ptr,SFBP *data_ptr,IOSEG *sfbp_ioseg_ptr,
    	struct function_block_desc *cp_function_ptr,
    	int slot, CCV *ccv)          

{         
int      (*driver_init)();
register int      length,s=1;
          
tga_init_scia_table  (scia_ptr,data_ptr,sfbp_ioseg_ptr,cp_function_ptr,slot,ccv); 
tga_init_data_table  (data_ptr,slot, ccv );                                  
tga_init_ioseg_table (data_ptr,sfbp_ioseg_ptr ,slot,ccv );                     

cp_func_ptr = (struct cp_func_block *)cp_function_ptr->function_block_address; 

driver_init = (int (*) () )sfbp$init_driver;
(*driver_init)(sfbp_ioseg_ptr,data_ptr,cp_function_ptr,scia_ptr);                    

return (1);                                
}


/*                            
* ===========================================================================
* = tga_init_scia_table - Initialize the SCIA table =
* ===========================================================================
*
* OVERVIEW:
*       Initialize the SCIA table used by the crt driver.
* 
* FORM OF CALL:
*       tga_init_scia_table (scia_ptr , data_ptr, ioseg_ptr, cp_function, slot , ccv )
*
* RETURNS:
*       1
*
* ARGUMENTS:
*       scia_ptr        - Pointer to SCIA table
*       data_ptr        - driver data pointer
*       sfbp_ioseg_ptr   - IO segment table pointer
*       cp_function     - Console Driver function block pointer
*       slot            - what slot we are in       
*       ccv             - callback vector pointer
*
* GLOBALS:
*       scia_block - This is the SCIA data block
*
* FUNCTIONAL DESCRIPTION:
*       The SCIA table is a shared driver table. It is used to identify the
*       port driver functions so that an upper level class driver can
*       call those functions. The console configuration process normally
*       sets this up, but we are setting up for this environment to maintain
*       some form of compatibility. For this particular application, I
*       am only initializing the graphics port driver descriptor to 
*       point to our driver application. This also initializes the 
*       console program function block which has pointers to the
*       port driver function block we will be using. Since I do not get a 
*       pointer to the font tables, I also require a pointer to be set up
*       for the fonts. 
*
* RETURN CODES:
*       none
*
* CALLS:
*       none
*
* SIDE EFFECTS:
*       port functions initialized
*
* ALGORITHM:
*       - Initialize the pointer to the us font table.
*       - Initialize the pointer to the mcs font table.
*       - Initialize driver descriptor.
*       - Initialize the console  function descriptor block.
*       - Initialize the graphics function descriptor block.
*       - Initialize the font width and height parameters.
--*/
int tga_init_scia_table (SCIA *scia_ptr, SFBP *data_ptr, IOSEG *sfbp_ioseg_ptr, 
	struct function_block_desc *cp_function, int slot , CCV *ccv)

{
             
scia_ptr->font_width                =  9;
scia_ptr->font_height               = 16;

/* Ascii font starts at begin	*/
scia_ptr->usfont_table_ptr 	    = (char *)ATI_FONT_TABLE;         
/* MCS   font starts in middle 	*/
scia_ptr->mcsfont_table_ptr 	    = (char *)scia_ptr->usfont_table_ptr + (128*32);

cp_function->function_block_address  = (unsigned int *)&scia_ptr->console_func;
cp_function->function_block_size     = sizeof(struct cp_func_block);


return (SS$K_SUCCESS);
}


/*+
* ===========================================================================
* = tga_init_data_table - Initialize the data table =
* ===========================================================================
*
* OVERVIEW:
*       This will initialize the driver data area.
* 
* FORM OF CALL:
*       tga_init_data_table (data_ptr , slot , ccv )
*
* RETURNS:
*       none
*
* ARGUMENTS:
*       data_ptr        - Pointer to driver data area
*       slot            - what slot we are in       
*       ccv             - callback vector pointer
*
* FUNCTIONAL DESCRIPTION:
*	
* RETURN CODES:
*       1
*
*
--*/ 
int tga_init_data_table (SFBP *data_ptr,int slot,CCV *ccv )

{

data_ptr->sfbp$l_ccv_vector     = (int)(ccv);
data_ptr->sfbp$l_slot_number    = (int)slot;
data_ptr->sfbp$l_cursor_update  = TRUE;                                        
data_ptr->sfbp$l_white_on_black = TRUE;                                        
data_ptr->sfbp$l_cursor_offset  = CURSOR_OFFSET ;
data_ptr->sfbp$l_hardware_assist= TRUE;                                        
data_ptr->sfbp$l_console_offset = data_ptr->sfbp$l_module == hx_8_plane ? CONSOLE_OFFSET : CONSOLE_OFFSET*4;
data_ptr->sfbp$l_display_clear	= TRUE;

return (1);
}



/*+
* ===========================================================================
* = tga_init_ioseg_table - Initialize the IOSEGMENT table =
* ===========================================================================
*
* OVERVIEW:
*       This will initialize the iosegment table used by the crt driver.
* 
* FORM OF CALL:
*       tga_init_ioseg_table (data_ptr,sfbp_ioseg_ptr , slot , ccv )
*
* RETURNS:
*       none
*
* ARGUMENTS:
*       data_ptr        - driver data pointer
*       sfbp_ioseg_ptr   - Pointer to IO segment table
*       slot            - what slot we are in       
*       ccv             - callback vector pointer
* GLOBALS:
*       sfbp_ioseg_block - This is the IO segmenty block in bss space
*
* FUNCTIONAL DESCRIPTION:
*
* RETURN CODES:
*       none
*
*
--*/ 
int tga_init_ioseg_table (SFBP *data_ptr,IOSEG *sfbp_ioseg_ptr,int slot,CCV *ccv )

{
data_ptr->sfbp$l_test_slot        	  = slot;                        
data_ptr->sfbp$l_rom_stride       	  = ROM_STRIDE;
data_ptr->sfbp$l_rom_size         	  = 64*1024;                            
                                                     
sfbp_ioseg_ptr->sfbp$a_rom_mem_ptr 	  = (char *)(TGA_mem_base[slot]+ (sfbp$k_a_sfbp_rom_space));
sfbp_ioseg_ptr->sfbp$a_reg_space   	  = (char *)(TGA_mem_base[slot]+ (sfbp$k_a_sfbp_reg_space));
sfbp_ioseg_ptr->sfbp$a_bt_space   	  = (char *)(TGA_mem_base[slot]+ (sfbp$k_a_sfbp_bt_space));
                                                     
return (1);
}                                                    
                                                     


 
#if FULL_TGA
/*+
 *===========================================================================
 *= strtol  - Converts ascii string of numerical digits to a long
 *===========================================================================
 *                   
-*/                  
                     
long strtol (const char *s, char **endp, int base)
{                                                                
  long i; 

  if (base == 0)base = 10;             
                       
  common_convert(s,base,&i,sizeof(i));
  return i;                                               
                        
}          
#endif

int pci_inmem(unsigned __int64 address, int length)
{        
 int value;
         
    value = indmeml( TGA_pb[data_ptr->sfbp$l_slot_number], address );
     return (value&0xffffffff);
 
 }
 

void pci_outmem(unsigned __int64 address, unsigned int value, int length)
 {     
    outdmeml( TGA_pb[data_ptr->sfbp$l_slot_number], address, value );
 }     
      
                                                                           
#if FULL_TGA
/*                                                                          
* ===========================================================================
* = tga_driver stubs -
* ===========================================================================
*
-*/
io_poll ()    /* Returns TRUE if a key is pressed; on the main keyboard.  
                Doesn't check the serial port keyboard. control c received.*/ 
{                    
 return (kbd_get()<0?0:1);
}                    
                     
                                                                           
/*                                                                          
* ===========================================================================
* = disableintr - This will clear the interrupt and condition
* ===========================================================================
*
-*/
disableintr (slot)   
{                    
io_disable_interrupts (0, TgaVector[slot]);
return(1);
}


                                                                           
/*                                                                          
* ===========================================================================
* = enableintr - This will enable the pic chip =
* ===========================================================================
*
-*/
enableintr  (slot) 
{
io_enable_interrupts (0, TgaVector[slot]);
return(1);
}
msdelay     (delay) {krn$_sleep(delay);}                      
usdelay     (delay) {krn$_micro_delay (delay);}                      
#endif
testintr    (slot) {return(TGAInterruptCount[data_ptr->sfbp$l_slot_number]);}
wbflush     () {mb();}     

#if !MEDULLA

/*++
* ===========================================================================
* = tga_int_handler - =
* ===========================================================================
--*/                                                   
                                                       
void tga_int_handler (int normalized_slot)              
{                                                      
#if 0
register int temp;                   

 /* Make sure Driver Init'd first	*/
 /* this in case I get spurious		*/
 if (gfx_func_ptr)
  {
  /* First thing, clear the pending interrupt(s). Level sensative. */
  temp  =  (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_int_status);
  temp &=  0x1f001f;                                                 
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_int_status,temp,LW);   
  }
  /* Increment the count per normalized slot	*/
  TGAInterruptCount[normalized_slot]++;
 
  /* Issue an EOI to the device slot		*/
  io_issue_eoi (0, TgaVector[normalized_slot]);
#else
    struct pb *pb;

    pb = TGA_pb[normalized_slot];
    if( pb )
	io_disable_interrupts( pb, pb->vector );
#endif
 }
#endif

#if MEDULLA
/*++
* ===========================================================================
* = tga_scc_interrupt - =
* ===========================================================================
--*/
void tga_scc_interrupt(int slot)
{
  unsigned char status;
  int value;

  if(status=check_scc_int()){
    if (status&RR3$M_ARXI) {
      keyboard_interrupt(slot);
    } else if (status&RR3$M_BRXI) {
      mouse_interrupt(slot);
    } else if (status&RR3$M_ATXI) {
      keyboard_interrupt_tx(slot);
    } else if (status&RR3$M_BTXI) {
      mouse_interrupt_tx(slot);
    }
  } else if(value=check_tga_int()){
    (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_int_status,value);
   TGAInterruptCount[0]++;
  }
}

int check_tga_int(){
 int      value;

 value = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_int_status);
 value &= 0x1f001f;
 return(value);
}
#endif     
    
/*+  
 * ============================================================================
 * = vga_setmode - Set mode                                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Set mode.
 *  
 * FORM OF CALL:
 *  
 *	vga_setmode ()
 *  
 * RETURNS:
 *
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 *	int mode - Mode the driver is to be changed to.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int tga_setmode (int mode) 
 {
 return (1);
 }
#if AVANTI
/*+  
 * ============================================================================
 * = CheckForTga - Check for Tga =
 * ============================================================================
 *
 * OVERVIEW:
 *	This will see if the option is a TGA
 *
 * FORM OF CALL:
 *
 *	CheckForTga (int slot );
 *                    
 * RETURNS:
 *	1 if TGA , else 0
 *
-*/
struct  pci_devs_found *CheckForTga (int slot)
{                
int	i;       
struct   pci_devs_found *dev_fnd_ptr;
                 
 dev_fnd_ptr = pci_find(DEC_TGA, 0);
 for (i = 0; i < dev_fnd_ptr->nbr_found;i++ ) 
 	if (slot == dev_fnd_ptr->dev_found[i].slot)
 		{
 		return (dev_fnd_ptr);
 		}
return (0);
}
#endif

#if MEDULLA
/*+
 * ============================================================================
 * = check_valid_tga - Check for valid TGA option.                            =
 * ============================================================================
 *
 * OVERVIEW:
 *      This will check if the option is a valid TGA option for PMC TGA options.
 *      It will set the NVRAM location for graphics if valid.
 *
 * FORM OF CALL:
 *
 *      check_valid_tga ();
 *
 * RETURNS:
 *      none
 *
-*/
void check_valid_tga () {
 int      config,module;
 register ROM_CONFIG    *rom_ptr;

  config   = (*gfx_func_ptr->pv_sfbp_read_rom)(scia_ptr,0);
  config  &= 0xffff;
  rom_ptr  = (ROM_CONFIG *)&config;
  module = (int)rom_ptr->sfbp$r_rom_config_bits.sfbp$v_module;

  if (module == 1)
        tga24m = 1;  /* used to flag keyboard driver for special addressing */

#if 0
  if ((module != 4) && (module != 1)) {
     qprintf("Unrecognized graphic module ID (0x%x)\n",module);
     qprintf("Default to serial console\n");
     graphics_console=0;
     rtc_write(EV_CONSOLE_TOY_OFFSET,graphics_console);
  } else {
     graphics_console=1;
     rtc_write(EV_CONSOLE_TOY_OFFSET,graphics_console);
  }
#endif
}

#endif
