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
 *	These are routines to do write buffer flush,wait retrace,etc.
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

#include   	"cp$src:platform.h"
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

/* These are constants used by the cfg_read/cfg_write code      */
#define   PCI_byte_mask 0
#define   PCI_word_mask 1
#define   PCI_long_mask 3

void     pci_outmem    (unsigned __int64 address, unsigned int value, int length);
unsigned int pci_inmem (unsigned __int64 address, int length);

extern   int TGA_mem_base[];

#define  TGA_DEBUG 0


/*+
 * ===================================================
 * = sfbp$$sfbp_write_reg - Write data to ASIC register =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Load data into ASIC register
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$sfbp_write_reg (scia_ptr,address,data ,mask);
 *
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver pointer
 *	address  - ASIC register
 *	data	 - data to load into register
 *      mask     - byte mask
 *
 * FUNCTIONAL DESCRIPTION:
 *      Load ASIC register with value
 *
--*/
int	sfbp$$sfbp_write_reg (SCIA *scia_ptr,int address,int data,int mask )

{
char    *reg;

reg  = (char *)control;
reg += (address );

sfbp$$bus_write      (reg,data,mask);
wbflush ();                                                       


return (SS$K_SUCCESS );

}

/*+
 * ===================================================
 * = sfbp$$sfbp_read_reg - Read data from ASIC register =
 * ===================================================
 *
 * OVERVIEW:
 *     Read data from into ASIC register
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$sfbp_read_reg (scia_ptr,address )
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver pointer
 *	address  - ASIC register
 *
 * FUNCTIONAL DESCRIPTION:
 *      Read ASIC register with value
 *
--*/
int	sfbp$$sfbp_read_reg(SCIA *scia_ptr,int address )

{
register int    data;
register char   *reg;

reg  =  (char *)control;
reg += (address );
data = sfbp$$bus_read (reg);

return (data);
}



/*+
 * ===================================================
 * = sfbp$$sfbp_read_rom - Read data from ASIC register =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     Read rom space
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$sfbp_read_rom (scia_ptr,address )
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver pointer
 *	address  - ASIC register
 *
 * FUNCTIONAL DESCRIPTION:
 *      Read rom space
 *
--*/
int	sfbp$$sfbp_read_rom(SCIA *scia_ptr,int address )

{
int	data;

#if TGA_DEBUG
pprintf ("mem base 0x%x address 0x%x\n",
	TGA_mem_base[data_ptr->sfbp$l_slot_number],
	TGA_mem_base[data_ptr->sfbp$l_slot_number]+address);
#endif
	
data = pci_inmem ( TGA_mem_base[data_ptr->sfbp$l_slot_number]+address,PCI_long_mask);

return (data);

}


/*+
 * ===================================================
 * = sfbp$$sfbp_write_vram - Write data vram =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Load data into video ram
  *  
 * FORM OF CALL:
 *  
 *     sfbp$$sfbp_write_vram (scia_ptr,address,data ,mask);
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver pointer
 *	address  - ASIC register
 *	data	 - data to load into register
 *      mask     - byte mask
 *
 * FUNCTIONAL DESCRIPTION:
 *      Load video ram with value
  *
 --*/
int	sfbp$$sfbp_write_vram (SCIA *scia_ptr,int address,int data,int mask )

{
register char   *vram;
register struct  drv_desc        *drv;
register IOSEG  *ioseg_ptr;

drv  = (struct drv_desc *)&scia_ptr->gfx_drv_desc;
ioseg_ptr = (IOSEG *)drv->ioseg_ptr;
vram = (char *)ioseg_ptr->sfbp$a_vram_mem_ptr;

vram += (address );

sfbp$$bus_write      (vram,data,mask);

return (SS$K_SUCCESS );

}


/*+
 * ===================================================
 * = sfbp$$sfbp_read_vram - Read video ram =
 * ===================================================
 *
 * OVERVIEW:
 *     Read video ram
  *  
 * FORM OF CALL:
 *  
 *     sfbp$$sfbp_read_vram (scia_ptr,address )
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver pointer
 *	address  - ASIC register
 *
 * FUNCTIONAL DESCRIPTION:
 *      Read ASIC register with value
 *
--*/
int	sfbp$$sfbp_read_vram(SCIA *scia_ptr,int address )

{
register int    data;
register char   *vram;
register struct  drv_desc        *drv;
register IOSEG  *ioseg_ptr;

drv  = (struct drv_desc *)&scia_ptr->gfx_drv_desc;
ioseg_ptr = (IOSEG *)drv->ioseg_ptr;
vram = (char *)ioseg_ptr->sfbp$a_vram_mem_ptr;

vram += (address );

/* Only Read Longwords		*/
data = sfbp$$bus_read ((int)vram&(~3));

return (data);
}


/*+
* ===========================================================================
* = sfbp$$test_int - Test for Interrupt =
* ===========================================================================
*
* OVERVIEW:
*       Test for interrupt
* 
* FORM OF CALL:
*       sfbp$$test_int (scia_ptr,data_ptr,value)
*
* ARGUMENTS:
*       scia_ptr        - driver area
*       data_ptr        - driver data 
*       value           - interrupt to test for
*
* FUNCTIONAL DESCRIPTION:
*       Read Interrupt Status Bit , I use this for polling and console check
*
--*/
int     sfbp$$test_int (SCIA *scia_ptr, SFBP *data_ptr ,INT_TYPE value)
{
register int     bits,s=1;

if (data_ptr->sfbp$l_int_poll)
  {
  bits = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_int_status);
  s = (bits & (1<<value)) ? 1 : 0;
  }
else s = testintr (data_ptr->sfbp$l_slot_number);
   
return (s);
}

/*+
* ===========================================================================
* = sfbp$$clear_int_req - Interrupt Clear Request Test =
* ===========================================================================
*
* OVERVIEW:
*       Clear any interrupts posted.
* 
* FORM OF CALL:
*       sfbp$$clear_int_req (scia_ptr,data_ptr,INT_TYPE int)
*
* ARGUMENTS:
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       int         - type of interrupt to clear 
*
* FUNCTIONAL DESCRIPTION:
*       Clear specific bit from controller.
*
--*/
int     sfbp$$clear_int_req (SCIA *scia_ptr, SFBP *data_ptr,INT_TYPE value)

{
int     temp,j;


  temp  =  (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_int_status);
  temp &=  0x1f001f;
  temp |=  (1<<value);

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_int_status,temp,LW);   

 /* Wait for Chip Idle after clearing   */
 sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);

return (1);
}




#if FULL_TGA
/*+
* ===========================================================================
* = sfbp$$get_expect - Check data with boolean =
* ===========================================================================
*
* OVERVIEW:
*       Get expected and actual data
* 
* FORM OF CALL:
*       sfbp$$get_expect (expect,actual,mode)
*
* RETURNS:
*       The data I expect in video ram the next time I check it.
*
* ARGUMENTS:
*       expect          - pointer to expected data
*       actual          - pointer to actual data
*       mode            - boolean mode  0 to 15
*
* FUNCTIONAL DESCRIPTION:
*       Read the boolean mode and see if the data is correct. We  need
*       to call this routine to find out what our actual data will be
*       when written to vram so that we can check it. The boolean
*       function determines the destination. The src is our expected
*       data that we write and dst is the original destination.       
*
--*/
int sfbp$$get_expect (int *src,int *dst,int mode)

{
int      s=1;
unsigned int iexpect;                                  

switch  (mode)
 {
 case 0x0: iexpect= 0;break;                                           /* 0                     */
 case 0x1: iexpect= (*src)&(sfbp$$bus_read(dst));break;                 /* src AND dst           */
 case 0x2: iexpect= (*src)&(~(sfbp$$bus_read(dst)));break;              /* src AND NOT dst       */
 case 0x3: iexpect= *src;break;                                        /* src                   */
 case 0x4: iexpect= (~(*src)&(sfbp$$bus_read(dst)));break;              /* NOT (src) AND dst     */
 case 0x5: iexpect= sfbp$$bus_read(dst);break;                          /* dst                   */
 case 0x6: iexpect= *src^sfbp$$bus_read(dst);break;                     /* src XOR dst           */
 case 0x7: iexpect= *src|sfbp$$bus_read(dst);break;                     /* src OR dst            */
 case 0x8: iexpect= ~(*src)&~(sfbp$$bus_read(dst));break;               /* NOT(src) AND NOT(dst) */
 case 0x9: iexpect= ~(*src)^(sfbp$$bus_read(dst));break;                /* NOT(src) XOR (dst)    */
 case 0xa: iexpect= ~(sfbp$$bus_read(dst));break;                       /* NOT dst               */
 case 0xb: iexpect= *src|~(sfbp$$bus_read(dst));break;                  /* src OR (NOT dst)      */
 case 0xc: iexpect= ~(*src);break;                                     /* NOT src               */
 case 0xd: iexpect= ~(*src)|sfbp$$bus_read(dst);break;                  /* (NOT src) OR dst      */ 
 case 0xe: iexpect= ~(*src)|~(sfbp$$bus_read(dst));break;               /* (NOT src) OR (NOT dst)*/
 case 0xf: iexpect= 0xffffffff;break;                                  /* 1                     */
 }

return (iexpect);
}


/*+
* ===========================================================================
* = sfbp$$sfbp_error_handler - Common Error Handler =
* ===========================================================================
*
* OVERVIEW:
*       This will print the test arguments on the command line.
* 
* FORM OF CALL:
*       sfbp$$sfbp_error_handler (scia_ptr,data_ptr,slot,string,param,fault_class,fault_code,fault8,fault32)
*
*
* ARGUMENTS:
*       scia_ptr        - shared driver area pointer
*       data_ptr        - driver data area pointer
*       status          - failure status code
*       slot            - failing slot
*       string          - argument pointer
*       param           - error parameter array [3]
*       fault_class     - class of fault
*       fault_code      - fault code (hex digit)
*
* FUNCTIONAL DESCRIPTION:
*       This will use the callback vector to print error information. When
*       We get an error, we don't know what the state of the display is so
*       may have to put the machine in a reset state so we can print out
*       errors and the such. We need to make the screen black.
*
--*/
int     sfbp$$sfbp_error_handler (SCIA *scia_ptr,SFBP *data_ptr,int slot,char *string,int param[],
        int fault_class, int fault_code,CHIP8 chip8_table[] ,CHIP32 chip32_table[],CHIP32Z chip32z_table[])

{
int	 i;
unsigned __int64 FailingAddress;
static  struct error_class_table_type 
        {
        int   error_type;
        char  *text;
        } error_type_table [] =
        {
        RegisterTestError,      "Reg",
        InterruptTestError,     "Int",
        PlaneMaskTestError,     "PMask",
        PixelShiftTestError,    "PShift",
        VramTestError,          "Vram",
        SimzTestError,          "Simz",
        StippleTestError,       "Stip",
        CopyTestError,          "Copy",
        BoolTestError,          "Rop",
        LineTestError,          "Line",
        LineColorCompareError,  "Line Color",
        LineZCompareError,      "Line Z",
        PackTestError,          "Pack",
        BoxTestError,           "Grid",
        FontTestError,          "Font",
        VdacTestError,          "RAMdac",
        PatternTestError,       "Patt",
        FlashError,             "Flash",
        ChairError,             "Chair ",
        ICSError,               "Osc",
        InitError,              "Init",
        CnfgError,              "Conf",
        EditError,              "edit",
        PixelMaskTestError,     "PixMask",
        };
        

 if (!data_ptr->sfbp$l_alt_console)
        (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_foreground,0,LW);

 FailingAddress   = 2;
 FailingAddress <<= 32;

 FailingAddress |= param[0];
 if (data_ptr->sfbp$l_error_type != VdacTestError )
     FailingAddress |= TGA_mem_base[data_ptr->sfbp$l_slot_number];                         

if (fault_class==fault_checksum)
  printf ("TFL #%d ZLXp =0X%x %s\n\tExpect=0x%x,Actual=0x%x,XOR=0x%x\n",
        slot,fault_code,string,
        param[1],                                       /* Expected             */
        param[2],                                       /* Actual               */
        param[1]^param[2]);                             /* XOR combination      */
 else                  
  printf ("TFL #%d ZLXp =0X%x %s\n\tAddress=0x%x,Expect=0x%x,Actual=0x%x,XOR=0x%x\n",
        slot,fault_code,string,
        FailingAddress,                                 /* Failing Address      */
        param[1],                                       /* Expected             */
        param[2],                                       /* Actual               */
        param[1]^param[2]);                      

 for (i=0;i*sizeof(struct error_class_table_type)<sizeof (error_type_table);i++)
        if (data_ptr->sfbp$l_error_type == error_type_table[i].error_type)
                {printf ("\t%s Class \n",error_type_table[i].text);break;}

 sfbp$$sfbp_fault_handler (scia_ptr,data_ptr,slot,string,param,fault_class,fault_code,
        chip8_table,chip32_table,chip32z_table);
 
 return (data_ptr->sfbp$l_conte);
}



/*+
* ===========================================================================
* = sfbp$$sfbp_fault_handler - Common Error Handler =
* ===========================================================================
*
* OVERVIEW:
*       This will handle faults
* 
* FORM OF CALL:
*       sfbp$$sfbp_fault_handler (scia_ptr,data_ptr,slot,string,param,fault_class,fault_code)
*
* ARGUMENTS:
*       scia_ptr        - shared driver area pointer
*       data_ptr        - driver data area pointer
*       status          - failure status code
*       slot            - failing slot
*       string          - argument pointer
*       param           - error parameter array [3]
*       fault_class     - class of fault
*       fault_code      - fault code (hex digit)
*
* FUNCTIONAL DESCRIPTION:
*       This will determine how to handle faults.
*
--*/
int     sfbp$$sfbp_fault_handler (SCIA *scia_ptr,SFBP *data_ptr,int slot,char *string,int param[],
        int fault_class,int fault_code,CHIP8 chip8_table[] ,CHIP32 chip32_table[],CHIP32Z chip32z_table[])

{

switch  (fault_class)
 {
 case   nofault: 
        return (1);        

 case   fault_asic:
        sfbp$$sfbp_fault_asic      (scia_ptr,data_ptr,slot,string,param,fault_class,fault_code,
                                                chip8_table,chip32_table,chip32z_table);
        return (1);        

 case   fault_interrupt:
        sfbp$$sfbp_fault_interrupt (scia_ptr,data_ptr,slot,string,param,fault_class,fault_code,
                                                chip8_table,chip32_table,chip32z_table);
        return (1);        

 case   fault_ramdac:
        sfbp$$sfbp_fault_ramdac (scia_ptr,data_ptr,slot,string,param,fault_class,fault_code,
                                                chip8_table,chip32_table,chip32z_table);
        return (1);        

 case   fault_vram:
        sfbp$$sfbp_fault_vram   (scia_ptr,data_ptr,slot,string,param,fault_class,fault_code,
                                                chip8_table,chip32_table,chip32z_table);
        return (1);        
 } 
 
return (1);
}



/*+
* ===========================================================================
* = sfbp$$sfbp_fault_vram - Vram Isolation =
* ===========================================================================
*
* OVERVIEW:
*       Isolate Vram Faults
* 
* FORM OF CALL:
*       sfbp$$sfbp_fault_vram (scia_ptr,data_ptr,slot,string,param,fault_class,fault_code)
*
*
* ARGUMENTS:
*       scia_ptr        - shared driver area pointer
*       data_ptr        - driver data area pointer
*       status          - failure status code
*       slot            - failing slot
*       string          - argument pointer
*       param           - error parameter array [3]
*       fault_class     - class of fault
*       fault_code      - fault code (hex digit)
*
* FUNCTIONAL DESCRIPTION:
*       This will present video ram faults.
*
--*/
int     sfbp$$sfbp_fault_vram (SCIA *scia_ptr,SFBP *data_ptr,int slot,char *string,int param[],
        int fault_class,int fault_code,
        CHIP8  chip8_table[],
        CHIP32 chip32_table[],
        CHIP32Z chip32z_table[])

{
int     pixel=0,data_byte=0,bank=0,chip=0;
int     FailingByte= -1,found=0,i,done=0,offset,ebyte,abyte,address,expect,actual;
CHIP32Z *Z32table;
static  char *board_text[2] ={"Frame Buffer","ZBuffer"};
static  struct fault_vram_type 
        {
        int     number;
        char    *fault_vram_string;
        } fault_vram_table [] = 
        {    
        1, "E15 AD",  
        2, "E3,E6,E9 ABT",  
        };          
                                                                          
typedef struct fault_vram32_type                                          
        {                                                                 
        int     number;                                                   
        char    *fault_vram_string;
        }vram32_table[3];             
                                                                    
struct fault_vram32_type fault_vram_table32[][3]  =                  
        {                                                           
         {    /* VRAM buffer */                                     
         1, "E36 AD",                                               
         2, "E13/E14/E17/E18 ABT",                          
         3, "E6/E24 CAS/RAS",                               
         },                                                 
         {    /* Z-buffer */                                
         1, "E36 AD",                                       
         2, "E11/E12/E15/E16 ABT",                          
         3, "E5/E23 CAS/RAS",                               
         }                                                  
        };                                                  
                                                            
address = param[0];                                         
expect  = param[1];                                         
actual  = param[2];                                         
Z32table= (CHIP32Z *)chip32z_table;                         
                                                            
 /* Line Verify Checks 1 byte at a time */                 
 if (data_ptr->sfbp$l_error_type == LineColorCompareError ||
     data_ptr->sfbp$l_error_type == LineZCompareError)     
        FailingByte = (address&7);                         
                                                           
 if (data_ptr->sfbp$l_module == hx_8_plane )               
 {                                                         
 bank = 0;                                                 
 /* Check for line mode failure        */                  
 if (FailingByte<0)                                        
  {                                                        
  /* Find the first offending byte     */                  
  /* Assume Address is Longword Align  */                  
  for (i=0;i<4;i++)
        {
        ebyte = (expect>>(i<<3))&0xff;
        abyte = (actual>>(i<<3))&0xff;
        if (ebyte!=abyte)
                {
                data_byte=i;
                if ((address&7)>3)data_byte+=4;
                found = 1;
                break;
                }
        }
   }else {found=1;data_byte=FailingByte;}
   
 if (found)
        {
        printf ("\tVRAM Bank %d Pix %d Byte %d Exx--> %s \n",
                bank,
                chip8_table[data_byte].pixel,
                chip8_table[data_byte].data_byte,
                chip8_table[data_byte].text);

        if (data_ptr->sfbp$l_module==hx_8_plane)
                for (i=0;i*sizeof(struct fault_vram_type)<sizeof (fault_vram_table);i++)
                        printf ("\tVerify %s\n",fault_vram_table[i].fault_vram_string);
        }
 }
 else if (data_ptr->sfbp$l_module == hx_32_plane_no_z || 
          data_ptr->sfbp$l_module == hx_32_plane_z )
 {
  /* Find the first offending byte     */
     
  for (i=0;i<4;i++)
        {
        ebyte = (expect>>(i<<3))&0xff;
        abyte = (actual>>(i<<3))&0xff;
        if (ebyte!=abyte)
                {
                data_byte  = i;
                pixel = (address&0x1f)/4;
                found = 1;
                break;
                }        
        }

  for (i=0;found && i<32;i++,Z32table++)
   {
    if ( chip32z_table[i].pixel == pixel && 
         chip32z_table[i].data_byte  == data_byte  ) 

    {
    /* Check for Z buffer Failure on the Bottom Module   */
    /* Which we can get from the frame buffer address    */
    /* Z buffer is at top of memory                      */
    /* frame buffer from 0100.0000 to 017ff.ffff         */
    /* Z space from 0180.0000 to 01ff.ffff               */
    /* Bank 0 is frame buffer                            */
    /* Bank 1 is Z buffer                                */
                                               
    if (data_ptr->sfbp$l_module == hx_32_plane_z  && 
         (address >= sfbp$k_a_sfbp_zbuf_mem_32 ))bank=1;
    else bank=0;                               
                                               
    /* If Z buffered data...then at end of table       */
    if (bank==1)Z32table += 32;                
                                               
    if (data_ptr->sfbp$l_module == hx_32_plane_z )
      {                                        
                                               
      printf ("\tVRAM Bank %d [%s] Pix %d Byte %d Exx-> %s \n", 
                bank,                          
                board_text[bank],              
                Z32table->pixel,               
                Z32table->data_byte,                
                Z32table->text);               
      }                                        
   else                                        
      {                                        
      printf ("\tVRAM Bank %d [%s] Pix %d Byte %d Exx-> %s \n",
                bank,                          
                board_text[bank],              
                Z32table->pixel,               
                Z32table->data_byte,                
                Z32table->text);               
      }                                        
                                               
   for (i=0;i*sizeof(struct fault_vram32_type)<sizeof (vram32_table);i++)
        printf ("\tVerify %s\n",fault_vram_table32[bank][i].fault_vram_string);
   break;                                        
   }                                             
  }                                              
 }

   
return (1);
}




/*+
* ===========================================================================
* = sfbp$$sfbp_fault_asic - ASIC Isolation =
* ===========================================================================
*
* OVERVIEW:
*       Isolate ASIC Faults
* 
* FORM OF CALL:
*       sfbp$$sfbp_fault_asic (scia_ptr,data_ptr,slot,string,param,fault_class,fault_code)
*
* ARGUMENTS:
*       scia_ptr        - shared driver area pointer
*       data_ptr        - driver data area pointer
*       status          - failure status code
*       slot            - failing slot
*       string          - argument pointer
*       param           - error parameter array [3]
*       fault_class     - class of fault
*       fault_code      - fault code (hex digit)
*
* FUNCTIONAL DESCRIPTION:
*       This will isolate asic faults
*
--*/
int     sfbp$$sfbp_fault_asic (SCIA *scia_ptr,SFBP *data_ptr,int slot,char *string,int param[],
        int fault_class,int fault_code,CHIP8 chip8_table[] ,CHIP32 chip32_table[],CHIP32Z chip32z_table[])
{
register int i;
static struct fault_asic_type 
        {
        int     number;
        char    *fault_asic_string;
        } fault_asic_table [] = 
        {
        1, "Y1  FB Xtal",
        2, "E02 FB Clk Div",
        };

for (i=0;i*sizeof(struct fault_asic_type)<sizeof (fault_asic_table);i++)
        printf ("\tVerify %s\n",fault_asic_table[i].fault_asic_string);

return (1);
}



/*+
* ===========================================================================
* = sfbp$$sfbp_fault_interrupt - interrupt Isolation =
* ===========================================================================
*
* OVERVIEW:
*       Isolate interrupt Faults
* 
* FORM OF CALL:
*       sfbp$$sfbp_fault_interrupt (scia_ptr,data_ptr,slot,string,param,fault_class,fault_code)
*
* ARGUMENTS:
*       scia_ptr        - shared driver area pointer
*       data_ptr        - driver data area pointer
*       status          - failure status code
*       slot            - failing slot
*       string          - argument pointer
*       param           - error parameter array [3]
*       fault_class     - class of fault
*       fault_code      - fault code (hex digit)
*
* FUNCTIONAL DESCRIPTION:
*       This will isolate interrupt faults
*
--*/
int     sfbp$$sfbp_fault_interrupt (SCIA *scia_ptr,SFBP *data_ptr,int slot,char *string,int param[],
        int fault_class,int fault_code,CHIP8 chip8_table[] ,CHIP32 chip32_table[],CHIP32Z chip32z_table[])

{
register int i;
static struct fault_interrupt_type 
        {
        int     number;
        char    *fault_interrupt_string;
        } fault_interrupt_table [] = 
        {
        1, "Y1  FB Xtal",
        2, "E02 FB Clk Div",
        3, "Y2  Xtal 14.318 Mhz",
        4, "E38 Dot Clk",  
        5, "E19 Video Clk",  
        };
        
for (i=0;i*sizeof(struct fault_interrupt_type)<sizeof (fault_interrupt_table);i++)
        printf ("\tVerify %s\n",fault_interrupt_table[i].fault_interrupt_string);

return (1);
}



/*+
* ===========================================================================
* = sfbp$$sfbp_fault_ramdac - RAMDAC Isolation =
* ===========================================================================
*
* OVERVIEW:
*       Isolate RAMDAC Faults
* 
* FORM OF CALL:
*       sfbp$$sfbp_fault_ramdac (scia_ptr,data_ptr,slot,string,param,fault_class,fault_code)
*
*
* ARGUMENTS:
*       scia_ptr        - shared driver area pointer
*       data_ptr        - driver data area pointer
*       status          - failure status code
*       slot            - failing slot
*       string          - argument pointer
*       param           - error parameter array [3]
*       fault_class     - class of fault
*       fault_code      - fault code (hex digit)

--*/
int     sfbp$$sfbp_fault_ramdac (SCIA *scia_ptr,SFBP *data_ptr,int slot,char *string,int param[],
        int fault_class,int fault_code,CHIP8 chip8_table[] ,CHIP32 chip32_table[],CHIP32Z chip32z_table[])

{
return (1);
}



/*+
 * ===================================================
 * = sfbp$$sfbp_write_rom - Write data vram =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Write rom space
  *  
 * FORM OF CALL:
 *  
 *     sfbp$$sfbp_write_rom (scia_ptr,address,data ,mask);
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver pointer
 *	address  - ASIC register
 *	data	 - data to load into register
 *      mask     - byte mask
 *
 * FUNCTIONAL DESCRIPTION:
 *      Write rom space (we can only write thru eeprom register) with
 *      address data pairs.
 *
EPROM Write Register (Write Only)
---------------------------------
	Writes to the EPROM take place by writing address/data pairs to this
	register. The EPROM Write Register contains an 18 bit address field
	and an 8 bit data field.


	31         25                                   8 7               0
	+---------+--------------------------------------+-----------------+
	| ignore  |            address                   |      Data       |
	+---------+--------------------------------------+-----------------+

	Reads from the EPROM address space, contain 8 bits of EPROM data, and
	8 bits of module configuration and monitor switch information.

	31    27   23                                   8 7               0
	+----+----+--------------------------------------+-----------------+
	|cnfg|swit|            ignore                    |      Data       |
	+----+----+--------------------------------------+-----------------+

	The configuration and switch fields contain the module and monitor types
	respectively.

	config field (bits 31-28)
	-------------------------
	0000 - 8 Plane
	0001 - 24 Plane
	0010 - 24 Plane w/Z
	0011 - Dual 8 Plane

	switch field
	------------
	monitor type - TBD

 *
 * 
 *
--*/
int	sfbp$$sfbp_write_rom (SCIA *scia_ptr,int address,int data,int mask )

{
volatile register char *rom;
volatile register int  *wrt;


/* TGA uses the indirect register              */
/* With data in low byte and address 25:8       */
rom  = (char *)(TGA_mem_base[data_ptr->sfbp$l_slot_number]+ sfbp$k_a_sfbp_reg_space + sfbp$k_eprom);
wrt  = (int *)rom;

/* TGA already takes into account the rom stride       */
pci_outmem (wrt,((address>>2)<<8)|(data&0xff),PCI_long_mask);

return (SS$K_SUCCESS );
}


/*+
* ===========================================================================
* = sfbp$$disable_interrupt - disable Interrupts to CPU =
* ===========================================================================
*
* OVERVIEW:
*       Disable interrupts
* 
* FORM OF CALL:
*       sfbp$$disable_interrupt (scia_ptr, data_ptr,int )
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data pointer
*       int             - interrupt value 
*
* FUNCTIONAL DESCRIPTION:
*       Read the interrupt status register, find the bit set
*       and clear it. We then disable interrupts by clearing 
*       the appropriate bit in the upper word.
*
--*/
int     sfbp$$disable_interrupt (SCIA *scia_ptr,SFBP *data_ptr, INT_TYPE value )

{
register int  temp;
union    deep_reg *deep;
int      deep_value;

 /* Clear bit in the upper longword     */
 temp   =  (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_int_status);
 temp  &= ~((1<<16)<<value);

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_int_status,temp,LW);   
 /* Wait for Chip Idle after clearing   */
 sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);

 sfbp$$clear_int_req (scia_ptr, data_ptr,value);

 disableintr (data_ptr->sfbp$l_slot_number);

 /* If disable parity...clear it in deep        */
 if (value == parity_error)
        {
        deep       = (union deep_reg *)&deep_value;
        deep_value = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_deep);
        deep->sfbp$r_deep_bits.sfbp$v_parityf = FALSE;

        sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);

        (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_deep,deep->sfbp$l_whole,LW);

        sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);
        }

return (SS$K_SUCCESS);
}



/*+
* ===========================================================================
* = sfbp$$enable_interrupt - Enable Interrupts to CPU =
* ===========================================================================
*
* OVERVIEW:
*       Enable interrupts
* 
* FORM OF CALL:
*       sfbp$$enable_interrupt (scia_ptr, data_ptr, ints )
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data pointer
*       ints            - which interrupt to enable
*
* FUNCTIONAL DESCRIPTION:
*       Set the enable bits in upper word. If we are testing
*       parity, then we must also set the parity enable in 
*       the deep register. Note, we must wait for deep operation
*       to complete.
*
--*/
int     sfbp$$enable_interrupt (SCIA *scia_ptr,SFBP *data_ptr, INT_TYPE value )

{
union    deep_reg *deep;
int      deep_value;
register int      temp;

enableintr (data_ptr->sfbp$l_slot_number);

 temp  =  (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_int_status);
 temp &=  0x1f001f;
 temp |=  ((1<<value)<<16);

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_int_status,temp,LW);   

 /* Wait for Chip Idle after clearing   */
 sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);

 if (value == parity_error)
        {
        deep       = (union deep_reg *)&deep_value;
        deep_value = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_deep);
        deep->sfbp$r_deep_bits.sfbp$v_parityf = TRUE;

        sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);

        (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_deep,deep->sfbp$l_whole,LW);

        sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);
        }
return (1);
}


/*+
* ===========================================================================
* = sfbp$$wait_retrace - Wait for 1 vertical retraces =
* ===========================================================================
*
* OVERVIEW:
*       Wait for variable number of vertical retraces.
* 
* FORM OF CALL:
*       sfbp$$wait_retrace (scia_ptr,data_ptr,load,pixel,input)
*
* ARGUMENTS:
*       scia_ptr        - driver area
*       data_ptr        - driver data 
*       load            - True if loading seed data
*       pixel           - What pixel to use for signature
*       input           - Input signature (463)
*
* FUNCTIONAL DESCRIPTION:
*       Wait for vertical retrace. We have to call the ramdac specific
*       routine to do this.
*
--*/
int     sfbp$$wait_retrace (SCIA *scia_ptr, SFBP *data_ptr ,int load,int pixel,INT_TYPE value,int input)
{
register int     i,j,s=1;

 sfbp$$enable_interrupt (scia_ptr,data_ptr,value );

 /* Wait for Chip Idle after clearing   */
 sfbp$$sfbp_wait_csr_ready (scia_ptr,data_ptr);

 s = (*bt_func_ptr->pv_bt_wait_interrupt) (scia_ptr,data_ptr,load,pixel,value,input );

return (s);
}
#endif
