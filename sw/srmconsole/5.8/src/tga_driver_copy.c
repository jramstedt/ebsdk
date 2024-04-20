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
 *      Driver sfbp ASIC Routines
 *
 *  ABSTRACT:
 *      This provides common ASIC routines required for graphics.
 *
 *  AUTHORS:
 *
 *      James Peacock
 *
 *
 *  CREATION DATE:      03-25-91 
 *
 *  MODIFICATION HISTORY:
 *
 *	dtr 16-mar-1995	Added definition for tga_pb
 *
 *
 *--
*/
#include "cp$src:platform.h"
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
#include 	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include 	"cp$src:prdef.h"
#include	"cp$inc:platform_io.h"

extern          int sim$g_print;

#define         DMA_READ_WORD_SIZE  512
#define         DMA_WRITE_WORD_SIZE 512
#define         DMA_READ_BYTES_COPY  DMA_READ_WORD_SIZE*4
#define         DMA_WRITE_BYTES_COPY DMA_WRITE_WORD_SIZE*8
#define         COPY_MASK           0xfffffff8

/* Add 4096 to copy64 register aliases  */
#define         RegisterAlias           0x1000
#define         RegisterAliasInt        RegisterAlias/4
#define         AliasEntries            64

/* These are constants used by the cfg_read/cfg_write code      */
#define   PCI_byte_mask 0
#define   PCI_word_mask 1
#define   PCI_long_mask 3

void     pci_outmem(unsigned __int64 address, unsigned int value, int length);
unsigned int pci_inmem(unsigned __int64 address, int length);

#define  DMA_SPIN_TIME 50000

extern struct   pb   *TGA_pb[];
#define WindowBase io_get_window_base(TGA_pb[data_ptr->sfbp$l_slot_number])

/*+
* ===========================================================================
* = sfbp$$sfbp_aligned_copy - Copy Video Ram =
* ===========================================================================
*
* OVERVIEW:
*       Copy Video RAM
* 
* FORM OF CALL:
*       sfbp$$sfbp_aligned_copy (scia_ptr,data_ptr,copy,visual,z)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*       copy            - copy type data structure
*       visual          - visual type data structure
*       z               - z type data structure
*       
* FUNCTIONAL DESCRIPTION:
*       This function does aligned copies using the mode in copy data structure.
*       The console driver always uses copy64 mode for performance. The function
*       is conditionally compiled for the console driver. Note, that when running
*       on an Alpha machine, the mips emulator does not flush the write buffer
*       for performance reasons, so it is up to me do optimize the code to
*       prevent write buffer merges.
*              
*              
	For increased performance, the copy buffer has been expanded to allow
	data to be copied in 64 byte blocks instead of 32 byte blocks.  Since
	the copy mask is only 32 pixels, 8-plane systems can still only specify
	32 byte masked copies, but 32-plane systems can specify a 16 pixel
	mask. Unmasked 64 byte copies on 8-plane systems can be performed
	through writes to the Copy64 Source/Destination registers.  For all
	visuals except 8-bit unpacked, Copy mode requires 8-byte alignment of
	both source and destination addresses.  For 8-bit unpacked visuals,
	addresses must be aligned to 32 bytes.
               
	8 plane Frame Buffer
               
	31                                                             0
	+--------------------------------------------------------------+
	|                     32 Pixel Copy Mask                       |
	+--------------------------------------------------------------+
               
	32 plane Frame Buffer
               
	31                            16 15                            0
	+-------------------------------+------------------------------+
	|             ignore            |      16 Pixel Copy Mask      |
	+-------------------------------+------------------------------+

(Software notes)

	Eeeeeyuck.

	First, let's talk about 32-plane systems.  No problem.  The code looks
	the same, but since we can copy 64 bytes of data at a time the data
	word uses 16 bits, as opposed to only 8 on the current sfb.  This is
	easy to deal with in the sfbparams.h file.  Also, all scanlines and
	pixmaps in 32-plane systems will be padded out to an 8-byte boundary,
	so the existing code should work just fine.

	Onto 8-bit systems.  Two problems, one is that a 32-bit data word can't
	tell you about 64 pixels, and the second is the (n*8)+4 bytes scanline
	width that the sfb+ prefers in order to make lines go really fast.
                              
	It's easy to deal with the 64-byte copy buffer.  The existing code's
	inner loop write all 1's to the source to read up 32 bytes, then writes
	all 1's to the destination to write back 32 bytes.  The new 8-bit pixel
	code will have an inner loop that writes the source address to the
	Copy64 register to read up 64 bytes, then write the destination address
	to the Copy64 register to write back 64 bytes.  It will then have
	anywhere from 0 to 63 bytes left to copy, which it can do with the
	normal style.  Note that the existing code only deals with an ending
	case of 1 to 32 bytes left to copy, so that code has to change
	somewhat.             
                              
   	The sfb+ hardware allows you to paint stipple patterns at an
   	alignment of 4 bytes for most filling operations, but copy mode won't
   	work if you do this.  You need to use an alignment of 8 bytes, which
   	means that all even scanlines use one set of masks, and all odd
   	scanlines use another.  Rather than flip between these masks EACH
   	scanline, I suspect that the best way to write the code is to compute
   	the masks for the first scanline, then skip down copying the third,
   	fifth, etc.  Then come back up and recompute the masks for the second
   	scanline, and skip down copying the forth, sixth, etc. scanlines.
   
   
DMA copy modes
--------------
   	The SFB+ includes support for DMA copies between the Frame Buffer
   	and system memory.  DMA copies use the same byte shifter as
   	ordinary copies.  The shift amount is specified by writing the
   	Pixel Shift Register.  In the DMA Copy modes, the DMA Address
   	register defines the system address of the pixmap, writes to the
	Frame buffer initiate the transfer and specify the Frame buffer
	address of the image. The data written to the Frame Buffer is
	interpreted as 16 bits of mask and a count.

	The depth fields in the Raster Op and Mode registers affects DMA
	copies, so it is possible, for example, to copy packed 8-bit images
	to a 32-bit screen at full DMA speeds.

	(Software notes)

	See the sample C routines for DMA code.


DMA read mode
-------------
	In DMA read mode, the count specifies the number of words which are
	To be read from system memory and the 16 bits of mask are
	interpreted as four 4-bit masks.  For all visuals except 8-bit
	unpacked, the framebuffer address needs	to be 4-byte aligned.
	For 8-bit unpacked visuals, the framebuffer address must be
	16-byte aligned.

	31                            16 15   12 11    8 7     4 3      0
	+-------------------------------+-------+-------+-------+-------+
	|        I/O bus read count-1   |right2 |right1 | left2 | left1 |
	+-------------------------------+-------+-------+-------+-------+

	Two of these masks are used at the left edge of the span being
	copied, the other two are used at the right edge.  The reason for
	two masks at each edge is to allow software control of the residue
	register.  At the left edge, the two 4-bit masks specify the
	framebuffer writemasks to be used for each of the first two words
	of data from system memory.  If the desired effect of the first
	read from memory is just to prime the residue register, then the
	first 4-bit mask will be set to 0.
 
	At the right edge, the first 4-bit mask specifies the framebuffer
	writemask to be used on the last read from system memory.  The
	second 4-bit mask specifies the framebuffer writemask to be used
	for the byte shifter residue register *after* the last read from
	system memory.  If the data remaining in the byte shifter residue
	register after the last read is not to be used, the second 4-bit
	mask will be set to 0.

	For DMA reads consisting of fewer than 3 reads, one or both of the
	left edge masks won't be used:

	# reads           masks used
	-------           ----------
	3 or more reads   left1  left2  right1 right2
	2 reads           left1  right1 right2
	1 read            right1 right2


DMA write mode
--------------
	In DMA write mode, the count specifies the number of 64-bit words
	which are to be read from the framebuffer memory and the 16 bits of
	mask are interpreted as two 8-bit masks.  For all visuals except 8-bit
	unpacked, the framebuffer address needs	to be 8-byte aligned.
	For 8-bit unpacked visuals, the framebuffer address must be
	32-byte aligned.


	31                            16 15            8 7              0
	+-------------------------------+---------------+---------------+
	|    framebuffer read count-1   |  right mask   |  left mask    |
	+-------------------------------+---------------+---------------+

	(Note that even though byte masks are specified for the left and
	right edges, because the TURBOchannel doesn't support masked writes
	for DMA, only full word (32-bit) transfers are supported.  Software
	is responsible for dealing with any partial word writes on the left
	and right edges in system memory.)

	In order to allow software control of the byte shifter residue
	register for DMA writes, the first framebuffer read is *always*
	used to prime the residue register.  Note that this first
	framebuffer read *is* included in the count.  The left mask is used
	with the *second* framebuffer read and the right mask is used with
	the last framebuffer read.  On the right edge, software forces data
	out of the residue register by reading an extra word if necessary.

	For operations consisting of 2 or fewer framebuffer reads, one or
	both of the masks may not be used:

	# reads           masks used
	-------           ----------
	3 or more reads   left   right
	2 reads           right
	1 read            (none)

	Since DMA write mode may be used to fetch data as part of a GetImage
	operation, in which the data goes directly back to the application,
	DMA write mode logically and's the Data Register with each word from
	the frame buffer before writing it to main memory.  This allows
	software to zero out the bottom nibbles of RGB in 12-bit visuals, to
	zero out the top nibble of window id/overlay so as to just get the
	overlay index, and to zero out the top byte in 12-bit and 24-bit
	visuals.

	THE DATA REGISTER IS NOT INTERLOCKED IN HARDWARE.  This means that
	you cannot write a new value to the Data register after a DMA write
	operation until the Command Status Register becomes 0.
*
*
* ALGORITHM:
*       IF CONSOLE DRIVER 
*                Write Mode Register with copy mode
*                Write Pixel shift register with 0 to clear state
*                Calculate the copy64 source and destination values
*                Calculate the end address 
*                DO 
*                       Write copy64 source with source
*                       Write copy64 dest with dest
*                       Increment the source and dest +64
*                       Write copy64 alias source with source
*                       Write copy64 alias dest with dest
*                       Increment the source and dest +64
*                       Flush The Write Buffer to Prevent a Merge
*                WHILE source < End Address
*               
*       ELSE ( for diagnostics)
*       Clear the copy buffer
*       Calculate the max bytes and copy incrment based on the visual depth
*       IF simple copy or copy64 THEN write mode with copy mode
*       ELSE if dma read non dithered
*               THEN 
*                       max bytes = 2048 bytes
*                       Write mode register dma read copy non dithered
*                       Write DMA base address register with system address      
*
*       ELSE if dma read dithered
*               THEN 
*                       max bytes = 2048 bytes ( 32 bit operations)
*                       Write mode register dma read copy non dithered
*                       Write DMA base address register with system address      
*
*       ELSE if dma write 
*               THEN 
*                       max bytes = 4096 bytes (64 bit operations)
*                       Write mode register dma write copy 
*                       Write DMA base address register with system address
*                       Write data register with 0xffffffff
*                      
*       Load the Visual
*       Calculate the repeat operations and residual bytes
*       Write the Pixel Shift Register with copy->pixel_shift
*       REPEAT for number of operations
*               Write to video ram address ( value depends on copy mode)
*               Incrment video ram address by copy increment
*       END REPEAT     
*                      
*       IF residual bytes
*               IF DMA mode then use left and right masks to copy
*               ELSE use pixel mask to copy remaining pixels
*                      
*       Flush the Write Buffer
*       Write the Mode Register with Default Mode
*       Write the Raster Op Register With Default Raster Op
*       Wait for Command CSR ready bit to clear
*                      
--*/                   
int     sfbp$$sfbp_aligned_copy (SCIA *scia_ptr,SFBP *data_ptr,SCOPY *copy, VISUAL *visual,Z *z)
                       
{                      
register  int i,j,s=1,res_bytes=0,*cbptr;
register  int copy_inc,max_lws,count,data,copy_mask,dma_mode,max_bytes,aligned_ops;
register  char  *src,*dst,*vram_src,*vram_dst;
volatile  register  int *vsrc,*vdst;
register  int AliasInc,end_address;
volatile  register int   *c64src,*c64dst;
volatile  register char  *reg;
int       Alpha,base_address,systemid;
int       DeviceStatus=0;
union     DeviceRegType *device;
                       
                       
 src        = (char *)copy->src_address;
 dst        = (char *)copy->dst_address;
 vram_src   = (char *)sfbp_ioseg_ptr->sfbp$a_vram_mem_ptr;
 vram_dst   = (char *)sfbp_ioseg_ptr->sfbp$a_vram_mem_ptr;
 vram_src   += (int)(copy->src_address );
 vram_dst   += (int)(copy->dst_address );
 vsrc       = (int  *)vram_src;
 vdst       = (int  *)vram_dst;
 end_address= (int)vram_src + copy->size;
 reg        = (char *)control;                                 
 reg       += sfbp$k_copy64_src;
 c64src     = (int *)reg;
 c64dst     = (int *)c64src+1;

 for (j=0;j<32;j+=4)
         (*gfx_func_ptr->pv_sfbp_write_reg) (scia_ptr,(sfbp$k_copy_reg_0+j),0,LW);

 switch (visual->depth)
 {
 case packed_8_packed:                        
 case packed_8_unpacked:                      
 case unpacked_8_packed:                      
 case unpacked_8_unpacked:                    
        data      = M1;
        max_bytes = 32;
        copy_inc   = 8;
        break; 
                
 case source_12_low_dest:                     
 case source_12_hi_dest:                      
        data = 0xffff;
        max_bytes = 64;
        copy_inc  = 16;
        break;
                
 case source_24_dest:                         
        data = 0xffff;
        max_bytes = 64;
        copy_inc   =16 ;
        break;
 }
  
 if (copy->copy == copy64)max_bytes=64;
        
 if (copy->copy == simple || copy->copy == copy64 )
   (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,sfbp$k_copy,LW);
  
 base_address  = copy->system_address&(~3);
 if ( copy->copy == dma_read_non_dithered)
  {                     
  max_bytes   = DMA_READ_BYTES_COPY;
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,sfbp$k_dma_read_copy_non_dithered,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_dma_base_address,
  	base_address+WindowBase,LW);
  }
  
 else if ( copy->copy == dma_read_dithered )
  {
  max_bytes   = DMA_READ_BYTES_COPY;
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,sfbp$k_dma_read_copy_dithered,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_dma_base_address,
  	base_address+WindowBase,LW);
  }

 else if (copy->copy == dma_write )
  {
  max_bytes   = DMA_WRITE_BYTES_COPY;
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,sfbp$k_dma_write_copy,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_dma_base_address,
	base_address+WindowBase,LW);
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_data_register,M1,LW );
  }

 sfbp$$load_visual (scia_ptr,data_ptr,visual,z);

 /* Determine the number of ops required for copy */

 aligned_ops    = copy->size/max_bytes;                                               
 res_bytes      = copy->size%max_bytes;
 max_lws        = max_bytes/4;

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_shift,copy->pixel_shift,LW);

 /* Minimize the inner loop for speed   */ 
 if (copy->copy == copy64 )
         {
         /* Copy64 requires a pixel address     */
         /* 64 bytes = 64 pixels in  8 plane    */
         /* 64 bytes = 16 pixels in 32 plane    */
         if (data_ptr->sfbp$l_planes==32)max_bytes/=4;
         for(i=AliasInc=0;i<aligned_ops;i++)
                   {
                   pci_outmem  (c64src,src,PCI_long_mask);
                   pci_outmem  (c64dst,dst,PCI_long_mask);
                   src    += max_bytes;
                   dst    += max_bytes;
                   /* Increment by Alias Amount   */
                   c64src += RegisterAliasInt;
                   c64dst += RegisterAliasInt;
                   /* Check for Wrap              */   
                   if (++AliasInc==AliasEntries)
                          {
                          c64src    = (int *)reg;
                          c64dst    = (int *)c64src+1;
                          AliasInc  = 0;
                          }
                   }
         }

 else if (copy->copy == dma_read_dithered || copy->copy == dma_read_non_dithered )
         for(i=0;i<aligned_ops;i++,src+=max_bytes,dst+=max_bytes,
                        vsrc+=(copy_inc ),vdst+=(copy_inc ))
           {  
           data = ((DMA_READ_WORD_SIZE-1)<<16)|(copy->right2<<12)|(copy->right1<<8)|(copy->left2<<4)|(copy->left1);
           (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,data,LW);
	   /* HW Interlock	*/
           (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_command_status,0,LW); 
           }       
 else if (copy->copy == dma_write   )
         for(i=0;i<aligned_ops;i++,src+=max_bytes,dst+=max_bytes,
                        vsrc+=(copy_inc ),vdst+=(copy_inc ))
            {
            data = ((DMA_WRITE_WORD_SIZE-1)<<16)|(copy->right1<<8)|(copy->left1);
            (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,data,LW);
	    /* HW Interlock	*/
            (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_command_status,0,LW); 
            }
 else  
       do 
           {
	   pci_outmem (vsrc,data,PCI_long_mask);
	   pci_outmem (vdst,data,PCI_long_mask);
           vsrc += copy_inc;
           vdst += copy_inc;
           } while ((int)vsrc < end_address);
    
 if (res_bytes)
  {
   if (copy->copy == dma_read_dithered || copy->copy == dma_read_non_dithered )
    {
    count = res_bytes/4;
    if ((res_bytes%4)==0)count--;
    if (count<0)count=0;
    data = (count<<16)|(copy->right2<<12)|(copy->right1<<8)|(copy->left2<<4)|(copy->left1);
    if (data_ptr->sfbp$l_supplemental) printf("DMA Read 0x%x \n",data);
    (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,data,LW);
    /* HW Interlock	*/
    (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_command_status,0,LW); 
    }
   else if (copy->copy == dma_write )
    {
    count = res_bytes/8;
    if (count<=0)count=1; 
    else if ((res_bytes%8)!=0)count++;
    data = (count<<16)|(copy->right1<<8)|(copy->left1);
    if (data_ptr->sfbp$l_supplemental) printf("DMA Write 0x%x \n",data);
    (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,data,LW);
    /* HW Interlock	*/
    (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_command_status,0,LW); 
    }
   else
    {
    for (i=copy_mask=0;i<res_bytes;i++)copy_mask   |= (1<<i);
    (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,copy_mask,LW );
    (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,dst,copy_mask,LW );
    }
 }
  
 wbflush();
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

return (1);
}


/*+
* ===========================================================================
* = sfbp$$sfbp_unaligned_copy - Copy Video Ram =
* ===========================================================================
*
* OVERVIEW:
*       Copy Video RAM
* 
* FORM OF CALL:
*       sfbp$$sfbp_unaligned_copy (scia_ptr,data_ptr,copy,visual,z)
*
* RETURNS:
*       1 for success else 0 for failure
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data area pointer
*
* GLOBALS:
*       none       
*
* FUNCTIONAL DESCRIPTION:
*       Use the sfbp copy mode to copy video ram contents. When copying
*       from odd alignments, we need to specify the byte mask. I modified
*       to make copy as fast as possible by using register variables and
*       literals for the copymask. The write operation is done using an
*       integer pointer which is also a register. 
*
* RETURN CODES:
*       none
*
* CALLS:
*      none
*
* SIDE EFFECTS:
*       Video ram is modified.
*
* ALGORITHM:
*       - Set to boolean mode source copy from input data
*       - Set to COPY mode
*       - REPEAT for number of 32 byte copy blocks
*       -  Do longword writes to video ram, what is written does not matter.
*
--*/
int     sfbp$$sfbp_unaligned_copy (SCIA *scia_ptr,SFBP *data_ptr,SCOPY *copy, VISUAL *visual,Z *z)

{
register  int i,j,s=1,*cbptr,unaligned_ops;
register  char  *src,*dst;
register  int src_shift,dst_shift,backup_dst,res_bytes=0,src_mask,dst_mask;
register  int residual=0;

/* Flush the copy buffer        */
 for (j=0;j<32;j+=4)
         (*gfx_func_ptr->pv_sfbp_write_reg) (scia_ptr,(sfbp$k_copy_reg_0+j),0,LW);

src            = (char *)copy->src_address;
dst            = (char *)copy->dst_address;

/* Unaligned destination...back it up by 8 bytes */
/* because of priming residue or something       */
        
(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,sfbp$k_copy,LW);

sfbp$$load_visual (scia_ptr,data_ptr,visual,z);

unaligned_ops  = copy->size/8;
res_bytes      = copy->size%8;

/* Not too concerned with data coming into copy buffer  */
/* Just going out...so read in 8 bytes at a time        */
/* and then just mask out the ones I don't want on the  */
/* way out of the copy buffer                           */

src_mask  = 0xff;
dst_mask  = 0xff;
src_shift = 0;
dst_shift = 0;


if ((int)dst%8)
        {
        dst_mask  <<= ((int)dst%8);
        dst_mask  &= 0xff;
        dst_shift  = (int)dst%8;
        }

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_pixel_shift,copy->pixel_shift,LW);
 if (unaligned_ops)
  {
  for(i=0;i<unaligned_ops;i++,src+=8,dst+=8,dst_mask=0xff)
        {
        (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,src_mask,(1<<((int)src)%4));
        (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,dst,dst_mask,(1<<((int)dst)%4));
        }
  
  residual = dst_shift ? dst_shift : copy->size%8;
  if (residual)
        {
        for (i=src_mask=dst_mask=0;i<residual;i++)
                {src_mask |= (1<<i);dst_mask |= (1<<i);}

        (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,src_mask,(1<<((int)src)%4));
        (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,dst,dst_mask,(1<<((int)dst)%4));
        }
   
  }
  
 if (res_bytes)
        {
        for (i=src_mask=dst_mask=0;i<res_bytes;i++)
                {src_mask |= (1<<i);dst_mask |= (1<<i);}

        if ((int)dst%8)dst_mask <<= ((int)dst%8);     
        
        (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,src_mask&0xff,(1<<((int)src)%4));
        (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,dst,dst_mask&0xff,(1<<((int)dst)%4));
        if (dst_mask>0xff)
          {
          dst += (8 - ((int)dst%8) );
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,src,src_mask&0xff,(1<<((int)src)%4));
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,dst,(dst_mask>>8)&0xff,
                        (1<<((int)dst)%4));
          }
        

        }

 
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

return (1);
}
