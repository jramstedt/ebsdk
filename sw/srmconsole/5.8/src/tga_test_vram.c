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
 *      BT459 RAMDAC test .
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
 *	dtr 16-mar-1995	- added external definition of tga_pb
 *
 *--
*/

/**
** INCLUDES
**/

#include   "cp$src:platform.h"
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
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:stddef.h"
#include "cp$src:prdef.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$inc:platform_io.h"

#define   VRAM_DMA_BYTES      (16384)
#define   SIZE_DMA_BUFFER     (VRAM_DMA_BYTES+64)
#define   LW_SIZE_DMA_BUFFER  (SIZE_DMA_BUFFER/4)
#define   StipplePattern      0x55555555
#define   CStipplePattern     0xAAAAAAAA

void      pci_outmem         (unsigned __int64 address, unsigned int value, int length);
unsigned  int pci_inmem_byte (unsigned __int64 address, int length);
unsigned  int pci_inmem      (unsigned __int64 address, int length);

#define   PCI_byte_mask 0
#define   PCI_word_mask 1
#define   PCI_long_mask 3

extern struct   pb   *TGA_pb[];
#define WindowBase io_get_window_base(TGA_pb[data_ptr->sfbp$l_slot_number])

#define ClockIpl  21
#define DeviceIpl 16
#define HaltIpl	  31

extern  int TestDmaPtr[];


/*+
* ===========================================================================
* = vram$$data_path_test - Data Path Test =
* ===========================================================================
*
* OVERVIEW:
*       Video Ram Data Path Test 
* 
* FORM OF CALL:
*       vram$$data_path_test (argc,argv,vector,scia_ptr,data_ptr,param )
*
* RETURNS:
*       1 for success
*       0 for failure      
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
*       This routine is used to verify the data  path from the turbo
*       channel to the video ram. I use location zero as my 
*       consistent address to write to. I then write 1,2,4,8...up
*       to bit 31 to that location and read it back for integrity.
*
--*/
int     vram$$data_path_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int     WriteData,PatternCnt,j,count,actual,i,data,vram,s=1;

/* If 24 plane Z module..then touch both banks  */
count = data_ptr->sfbp$l_module == hx_32_plane_z ? 2 : 1; 

for (s=1,vram=0,j=0;s&1 && j<count;j++)
 {
 for (PatternCnt=0;s&1 && PatternCnt<2;PatternCnt++)
  {
  for (i=0,data=1;s&1 && i<32;i++,data<<=1)
   {
   WriteData = PatternCnt == 0 ? data : ~data;
   if (data_ptr->sfbp$l_print) printf ("vram 0x%x data 0x%x\n",vram,WriteData);
   (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,vram,WriteData,LW);
   actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
   if (actual != WriteData )
       {                                                                                  
       param[0] = data_ptr->sfbp$l_vram_base | vram;
       param[1] = WriteData;
       param[2] = actual ;                                                               
       s =0;
       }
     }
   }
  vram += sfbp$k_a_sfbp_vram_mem_32_noz;
  }
}


/*+
* ===========================================================================
* = vram$$address_path_test - Address Path Test =
* ===========================================================================
*
* OVERVIEW:
*       Video Ram Write Test 
* 
* FORM OF CALL:
*       vram$$address_path_test (argc,argv,vector,scia_ptr,data_ptr,param )
*
* RETURNS:
*       1 for success
*       0 for failure      
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
*       This will verify the address path to the video ram cells. It will
*       start with the lowest address in video march and then go to 
*       binary increments of the address, such as 0,1,2,4,8 up to the
*       maximum address.       
*
--*/
int     vram$$address_path_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
int     bytemask,actual,i,j,data,vram,s=1;

for (j=1,s=1,vram=0;s&1 && vram <data_ptr->sfbp$l_vram_size;j<<=1)
 {
 for (i=0,data=0x55555555;s&1 && i<2;i++,data = 0xAAAAAAAA)
  {
   bytemask = (int)(1<<(vram%4));
   if (data_ptr->sfbp$l_print) {printf ("write 0x%x 0x%x bytemask %d\n",vram,data,bytemask);}
   (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,vram,data,bytemask);
   actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,vram);
   actual >>= ((vram%4)<<3);
   if ((actual&0xff) != (data&0xff) || data_ptr->sfbp$l_test_error )
       {                                                                                 
       param[0] = data_ptr->sfbp$l_vram_base | vram;
       param[1] = data&0xff;
       param[2] = actual&0xff;                                                               
       s = data_ptr->sfbp$l_conte ? 1 : 0;
       }
   }
 vram = j;
 }

return (s);        
}



/*+
* ===========================================================================
* = vram$$write_test - Write Test =
* ===========================================================================
*
* OVERVIEW:
*       Video Ram Write Test 
* 
* FORM OF CALL:
*       vram$$write_test (argc,argv,vector,scia_ptr,data_ptr,param )
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
*       This will stipple all of video ram with the pattern 0x55555555.
*       
* ALGORITHM:
*       IF using an offchip cursor (463) 24 plane
*               Then disable cursor operation
*       Call Fill VRAM routine to block fill vram.
*       
--*/
int     vram$$write_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int VramData;

 /* Stipple all of vram...vram size determined during configuration                      */
 /* time which gets the configuration type and sets vram size to 2,6,8 or 16 Mbytes      */
 
 /* This is the only case where we stipple all of video ram                              */
 /* We will not see the cursor in this case so we should disable at this point           */
 /* Otherwise We get Garbage on the Tube                                                 */
 if (data_ptr->sfbp$l_cursor_offchip )
        (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_video_valid,ActiveVideo,LW);

 /* Look for Parameter   */
 /* t 1/vram 55555555    */

  VramData = StipplePattern;
                 
 (*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,0,VramData,data_ptr->sfbp$l_vram_size);

 /* Should have a restored cursor at this point          */
 /* Which is important at console                        */

 return (1);
}


/*+
* ===========================================================================
* = vram$$read_compliment_test - Read Ram Test =
* ===========================================================================
*
* OVERVIEW:
*       Video Ram Read Test 
* 
* FORM OF CALL:
*       vram$$read_compliment_test (argc,argv,vector,scia_ptr,data_ptr,param )
*
* RETURNS:
*       1 for success
*       0 for failure      
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
*       This routine will simply stipple video ram with the pattern 
*       0xFFFFFFFF with a GXxor raster op so that the prior contents
*       of vram should go from 0x55555555 to 0xAAAAAAAA. 
--*/
int     vram$$read_compliment_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register  int       i,j=0,s=1;
register  int       actual,bytes,lws,blocks,src_inc;
register  char      *src;
STIP      stip;
COLOR     color;
VISUAL    visual;
Z         z;

 bytes             = data_ptr->sfbp$l_vram_size; 
 src               = (char *)0;

 /* Stipple the data in GXxor mode to compliment the data now     */
 /* Since that is the ultimate goal...compliment the data         */

 stip.src_address = (int)src;
 stip.size        = bytes;
 stip.mode        = GXxor;
 stip.stipple     = transparent_stipple;
 color.fg_value   = M1;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = 0;
 visual.boolean   = stip.mode;
    
 /* Do it with XOR mode set             */
 (*gfx_func_ptr->pv_sfbp_stipple)(scia_ptr,data_ptr,&stip,&color,&visual,&z);

 return (1);
}


/*+
* ===========================================================================
* = vram$$compare_compliment_test - Read Ram Test =
* ===========================================================================
*
* OVERVIEW:
*       Video Ram Read Test 
* 
* FORM OF CALL:
*       vram$$compare_compliment_test (argc,argv,vector,scia_ptr,data_ptr,param )
*
* RETURNS:
*       1 for success
*       0 for failure      
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
*       This routine will check that the read compliment subtest
*       did a proper GXxor and that vram has 0xAAAAAAAA in it. 
*       We use DMA write copies of 2048 bytes and then check the
*       dma buffer for the pattern, thus improving the speed 
*       considerably. We should also invalidate cache and run
*       cached during the duration of the test to speed it up.
*
--*/
int     vram$$compare_compliment_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register  int  k=0,j=0,s=1;
register  int  ByteCount,LwCount,WriteMask,ReadCount,bytes,src=0;
volatile  register  int *vsrc,*BufferPtr,*EndBufferPtr;
register  int DmaBufferPtr,XfrCount,EndAddress,SourceVisual,DestVisual;
register  int ReadData,BufferIndex,CVramData,VramData;

 CVramData      = CStipplePattern;
 src		= 0;
 bytes          = data_ptr->sfbp$l_vram_size; 
 vsrc           = (int *)sfbp_ioseg_ptr->sfbp$a_vram_mem_ptr;
 SourceVisual   = data_ptr->sfbp$l_module == hx_8_plane ? SourceVisualPacked: SourceVisualTrue;
 DestVisual     = data_ptr->sfbp$l_module == hx_8_plane ? DestVisualPacked  : DestVisualTrue;
 EndAddress     = bytes;

 ByteCount      = VRAM_DMA_BYTES;
 LwCount        = ByteCount / 4 ; 
 ReadCount      = ByteCount / 8 ; 
 WriteMask      = ((ReadCount-1)<<16)|0xffff;

 BufferPtr = EndBufferPtr = (int *)TestDmaPtr[data_ptr->sfbp$l_slot_number];
 EndBufferPtr  += LwCount;

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_shift,0,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,sfbp$k_dma_write_copy|(SourceVisual<<8),LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_dma_base_address,
	(int)TestDmaPtr[data_ptr->sfbp$l_slot_number]+WindowBase,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_data_register,M1,LW );
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);
 EndBufferPtr  -= 2;             
                                 
 do                              
 {                               
  (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,WriteMask,LW);
  /* HW Interlock	*/
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_command_status,0,LW); 
  (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
                                 
  BufferPtr   = (int *)TestDmaPtr[data_ptr->sfbp$l_slot_number];
  BufferIndex = 0;
  do 
        {
        if ((ReadData= *BufferPtr) != CVramData )
           {
            param[0] = BufferPtr;
            param[1] = CVramData;
            param[2] = ReadData;
	    s 	     = 0;
            (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
            (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
            break;
            }else BufferIndex +=4;
        } while (++BufferPtr < EndBufferPtr);
  src += ByteCount;
  } while (src<EndAddress && (s&1) );  

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_shift,0,LW);

 /* If video ram test successfull...then scrub it        */
 if (s&1)(*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,0,0,data_ptr->sfbp$l_vram_size);

 if (data_ptr->sfbp$l_cursor_offchip )
   {
   (*bt_func_ptr->pv_bt_cursor_ram) (scia_ptr,data_ptr);
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_video_valid,CursorEnable|ActiveVideo,LW);
   }

 return (s);
}

