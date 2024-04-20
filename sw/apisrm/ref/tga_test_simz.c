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
 *      Color Frame Buffer Test Routine 
 *
 *  ABSTRACT:
 *
 *      Test Description:
  *
 *  AUTHORS:
 *
 *      James Peacock
 *
 *
 *  CREATION DATE:      02-28-91
 *
 *  MODIFICATION HISTORY:
 *
 *      1.1     Add stipple test for video ram screening.
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


#define         IPATT           0xAA000000
#define         SPATT           0x55000000
#define         ZPATT           0x00555555
#define        ZSPATT           0x55555555
#define         FPATT           0xAAAAAAAA

#define CHECK_CONFIG() {if (data_ptr->sfbp$l_module == hx_8_plane )return(1);}

#define IBM_TOP 1


/*+
* ===========================================================================
* = simz$$simple_test - Simple Mode Z buffer test =
* ===========================================================================
*
* OVERVIEW:
*       Simple Mode Z Buffer Test 
* 
* FORM OF CALL:
*       simz$$simple_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*               This will initialize a portion of the Z buffer with
*       known values. It will then write z values to the z buffer 
*       address. The second write is the pixel value written to 
*       the frame buffer, which is only done if the comparison
*       succeeds.
*
	Simple Z mode is like simple mode, but performs a Z value
	comparison to see if the pixel data should actually be written.
	Simple Z mode requires the Z Base Address and Z value registers
	to be set-up prior to the write to the frame buffer.  The Stencil
	mode register specifies the actions to take, just like in Z buffered
	line modes the pixel is only written if the Z and Stencil tests pass,
	the Z and Stencil values will have the appropriate operation applied.

	Simple Z mode uses the bottom 4 bits of the Pixel Mask Register to
	mask off bytes that shouldn't be written.
*
--*/
int     simz$$simple_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             zfill,i,j,k,u,w,y,s=1,num_bytes;
register        int             expect,zdata,mode_reg,actual,zvalue;
int             zram,fram;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;
union           stencil_reg stencil;
int             fraction=0,whole=0;
union           zfraction_reg   *zf;
union           zvalue_reg      *zv;

static          struct  zop_table_type 
         {
         int    zop;
         } zop_table [] =
         {
/*       keep,
*/       replace
         };
         

static          struct  z_table_type 
         {
         int    testno;                         /* Case number                                    */
         char   *text;                          /* text for what the inequality is                */
         ZTEST  ztest;                          /* inequality to check for                        */
         int    zvalue;                         /* Z value to write                               */
         int    fvalue;                         /* frame buffer value to write if ztest true      */
         int    expect;                         /* expected frame buffer value                    */
         } z_table [] =
         {
         1, ">=",          geq,           ZPATT,                  FPATT,          FPATT,  /* true...      write fpatt   */
         2, ">=",          geq,           ZPATT+1,                FPATT,          FPATT,  /* true...      write fpatt   */
         3, ">=",          geq,           ZPATT-1,                FPATT,          0,      /* false...don't write fpatt  */
         4, "always",      always,        ZPATT,                  FPATT,          FPATT,  /* true...      write  fpatt  */
         5, "never",       never,         ZPATT,                  FPATT,          0,      /* false...don't write fpatt  */
         6, "<",           less,          ZPATT-1,                FPATT,          FPATT,  /* true...      write fpatt   */
         7, "<",           less,          ZPATT,                  FPATT,          0,      /* false...dont write fpatt   */
         8, "==",          equal,         ZPATT,                  FPATT,          FPATT,  /* true...      write fpatt   */
         9 ,"==",          equal,         ZPATT-1,                FPATT,          0,      /* false...dont write fpatt   */
        10, "<=",          leq,           ZPATT,                  FPATT,          FPATT,  /* true...      write fpatt   */
        11, "<=",          leq,           ZPATT-1,                FPATT,          FPATT,  /* true...      write fpatt   */
        12, "<=",          leq,           ZPATT+1,                FPATT,          0,      /* false ...    write fpatt   */
        13, ">",           greater,       ZPATT+1,                FPATT,          FPATT,  /* true...      write fpatt   */
        14, ">",           greater,       ZPATT,                  FPATT,          0,      /* false...dont write fpatt   */
        15, "!=",          notequal,      ZPATT-1,                FPATT,          FPATT,  /* true...      write fpatt   */
        16, "!=",          notequal,      ZPATT,                  FPATT,          0,      /* false...dont write fpatt   */
         };

CHECK_CONFIG();

zf = (union zfraction_reg   *)&fraction;
zv = (union zvalue_reg      *)&whole;

fram           = data_ptr->sfbp$l_console_offset;
if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
     zram           =  sfbp$k_a_sfbp_zbuf_mem_32 - data_ptr->sfbp$l_vram_base;
else zram      =  fram + 10*data_ptr->sfbp$l_bytes_per_scanline;

expect         = sfbp$k_def_lw_red;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
zfill          = 0x00555555;

(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,fram,0,num_bytes);                      
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,zram,zfill,num_bytes);                      

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = 0;
 visual.boolean   = GXcopy;

 if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
        z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 ;       
 else z.base_addr       = (int)zram;

/* This test is to verify z buffering only primarily and keep the       */
/* stencil logic the same                                               */

for (s=1,y=0;s&1 && y*sizeof (struct zop_table_type)<sizeof(zop_table);y++)
{
 
 for (i=0;s&1 && i*sizeof (struct z_table_type)<sizeof(z_table);i++)
 {   
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
  (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

  (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,0,LW);
  (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,zram,ZPATT,LW);
 
  stencil.sfbp$l_whole = 0;
  stencil.sfbp$r_stencil_bits.sfbp$v_stencil_wmask = 0xff;
  stencil.sfbp$r_stencil_bits.sfbp$v_stencil_rmask = 0xff;
  stencil.sfbp$r_stencil_bits.sfbp$v_stencil_stest = always;
  stencil.sfbp$r_stencil_bits.sfbp$v_stencil_sfail = keep;
  stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zfail = keep;
  stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zpass = keep;
  stencil.sfbp$r_stencil_bits.sfbp$v_stencil_ztest = z_table[i].ztest;
  stencil.sfbp$r_stencil_bits.sfbp$v_stencil_z     = zop_table[y].zop;

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,stencil.sfbp$l_whole,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 
  mode_reg  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);
  mode_reg |= sfbp$k_simple_z;
  
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,mode_reg,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

 if (data_ptr->sfbp$l_print) 
	{printf ("test %d %s zvalue 0x%x\n",z_table[i].testno,z_table[i].text,z_table[i].zvalue);}

 fraction = whole = 0;
 zf->sfbp$r_zfraction_bits.sfbp$v_fraction = 0;
 zf->sfbp$r_zfraction_bits.sfbp$v_whole    = z_table[i].zvalue;
 zv->sfbp$r_zvalue_bits.sfbp$v_whole       = z_table[i].zvalue>>20;  

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_base_addr,z.base_addr,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_fraction,fraction,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_whole,whole,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

 (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,z_table[i].fvalue,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

 actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,fram);
 if (data_ptr->sfbp$l_print) {printf ("Read Pixel Address 0x%x value 0x%x\n",fram,actual);}

 /* First check frame buffer and see if it is what is expected  */
 if (actual != z_table[i].expect || data_ptr->sfbp$l_test_error )
      { 
      param[0] = data_ptr->sfbp$l_vram_base | (int)fram;                                     
      param[1] = z_table[i].expect;                                       
      param[2] = actual;                                        
      s=0;                                                      
      if (data_ptr->sfbp$l_supplemental)
	printf ("SIMZ 1 FAIL test %d %s zvalue 0x%x\n",
		z_table[i].testno,z_table[i].text,z_table[i].zvalue);
      }  
  (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
  (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
  }
}

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);
(*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

return (s);
}





/*+
* ===========================================================================
* = simz$$stest_test - stencil Mode Z buffer test =
* ===========================================================================
*
* OVERVIEW:
*       stencil Mode Z Buffer Test 
* 
* FORM OF CALL:
*       simz$$stest_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
*
* FUNCTIONAL DESCRIPTION:
*               Stencil verification.
--*/
int     simz$$stest_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             zfill,i,j,k,y,u,w,f,s=1,num_bytes;
register        int             expect,sdata,zdata,mode_reg,actual,zvalue;
int             zram,fram;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;
union           stencil_reg stencil;
int             fraction=0,whole=0;
union           zfraction_reg   *zf;
union           zvalue_reg      *zv;


static          struct  zop_table_type 
         {
         int    zop;
         } zop_table [] =
         {
/*       keep,
*/       replace
         };
         
static          struct  stest_table_type 
         {
         int    testno;  
         char   *text;   
         STEST  stest;   
         int    svalue;  
         int    fvalue;  
         int    expect; 
         } stest_table [] =
         {
         1, ">=",          geq,           SPATT,                  FPATT,          FPATT,  
         2, ">=",          geq,           SPATT+0x01000000,       FPATT,          FPATT,  
         3, ">=",          geq,           SPATT-0x01000000,       FPATT,          0,      
         4, "always",      always,        SPATT,                  FPATT,          FPATT,  
         5, "never",       never,         SPATT,                  FPATT,          0,      
         6, "<",           less,          SPATT-0x01000000,       FPATT,          FPATT,  
         7, "<",           less,          SPATT,                  FPATT,          0,      
         8, "==",          equal,         SPATT,                  FPATT,          FPATT,  
         9 ,"==",          equal,         SPATT-0x01000000,       FPATT,          0,      
        10, "<=",          leq,           SPATT,                  FPATT,          FPATT,  
        11, "<=",          leq,           SPATT-0x01000000,       FPATT,          FPATT,  
        12, "<=",          leq,           SPATT+0x01000000,       FPATT,          0,      
        13, ">",           greater,       SPATT+0x01000000,       FPATT,          FPATT,  
        14, ">",           greater,       SPATT,                  FPATT,          0,      
        15, "!=",          notequal,      SPATT-0x01000000,       FPATT,          FPATT,  
        16, "!=",          notequal,      SPATT,                  FPATT,          0,      
         };

CHECK_CONFIG();

zf = (union zfraction_reg   *)&fraction;
zv = (union zvalue_reg      *)&whole;

fram           = data_ptr->sfbp$l_console_offset;
if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
     zram           =  sfbp$k_a_sfbp_zbuf_mem_32 - data_ptr->sfbp$l_vram_base;
else zram      =  fram + 10*data_ptr->sfbp$l_bytes_per_scanline;

expect         = sfbp$k_def_lw_red;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
zfill          = 0x55000000;

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,fram,0,num_bytes);                      
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,zram,zfill,num_bytes);                      

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = 0;
 visual.boolean   = GXcopy;

 if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
        z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 ;       
 else z.base_addr       = (int)zram;

 for (s=1,k=0;s&1 && k*sizeof (struct stest_table_type)<sizeof(stest_table);k++)
    {
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,zram,SPATT,LW);
 
          stencil.sfbp$l_whole = 0;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_wmask = 0xff;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_rmask = 0xff;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_stest = stest_table[k].stest;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_sfail = replace;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zfail = replace;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zpass = replace;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_ztest = always;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_z     = replace;

          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,stencil.sfbp$l_whole,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         
          mode_reg  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);
          mode_reg |= sfbp$k_simple_z;
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,mode_reg,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
          
          if (data_ptr->sfbp$l_print) {printf ("test %d %s svalue 0x%x\n",stest_table[k].testno,
                        stest_table[k].text,stest_table[k].svalue);}

          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_base_addr,z.base_addr,LW);
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_fraction,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_whole,stest_table[k].svalue,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,stest_table[k].fvalue,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

          actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,fram);
          if (data_ptr->sfbp$l_print) {printf ("Read Pixel Address 0x%x value 0x%x\n",fram,actual);}

          if (actual != stest_table[k].expect || data_ptr->sfbp$l_test_error )
              { 
              param[0] = data_ptr->sfbp$l_vram_base | (int)fram;                                     
              param[1] = stest_table[k].expect;                                       
              param[2] = actual;                                        
              s=0; 
              if (data_ptr->sfbp$l_supplemental)
		  printf ("STEST 2 Failed test %d %s svalue 0x%x\n",stest_table[k].testno,
                        stest_table[k].text,stest_table[k].svalue);
	      }
         (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
         (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         }

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);
(*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

return (s);
}






/*+
* ===========================================================================
* = simz$$sfail_test - stencil Mode Z buffer test =
* ===========================================================================
*
* OVERVIEW:
*       stencil Mode Z Buffer Test 
* 
* FORM OF CALL:
*       simz$$sfail_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*               Stencil Fail test
*
--*/
int     simz$$sfail_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             zfill,i,j,k,y,u,w,f,s=1,num_bytes;
register        int             expect,sdata,zdata,mode_reg,actual,zvalue;
int             zram,fram;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;
union           stencil_reg stencil;
int             fraction=0,whole=0;
union           zfraction_reg   *zf;
union           zvalue_reg      *zv;

static          struct  stest_table_type 
         {
         int    testno;  
         char   *text;   
         STEST  stest;   
         SFAIL  sfail;                            
         int    svalue;  
         int    fvalue;  
         int    expect; 
         } stest_table [] =
         {
         1, ">=",          geq,         keep,      SPATT-0x01000000,       FPATT,          SPATT,      
         2, ">=",          geq,         zero,      SPATT-0x01000000,       FPATT,          0,      
         3, ">=",          geq,         replace,   SPATT-0x01000000,       FPATT,          SPATT-0x01000000,      
         4, ">=",          geq,         incr,      SPATT-0x01000000,       FPATT,          SPATT+0x01000000,      
         5, ">=",          geq,         decr,      SPATT-0x01000000,       FPATT,          SPATT-0x01000000,      
         6, ">=",          geq,         invert,    SPATT-0x01000000,       FPATT,          IPATT,
         };

CHECK_CONFIG();

fram           = data_ptr->sfbp$l_console_offset;
zf = (union zfraction_reg   *)&fraction;
zv = (union zvalue_reg      *)&whole;

if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
     zram           =  sfbp$k_a_sfbp_zbuf_mem_32 - data_ptr->sfbp$l_vram_base;
else zram      =  fram + 10*data_ptr->sfbp$l_bytes_per_scanline;

expect         = sfbp$k_def_lw_red;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
zfill          = 0x55000000;

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,fram,0,num_bytes);                      
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,zram,zfill,num_bytes);                      

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = 0;
 visual.boolean   = GXcopy;
 
 if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
        z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 ;       
 else z.base_addr       = (int)zram;

 for (s=1,k=0;s&1 && k*sizeof (struct stest_table_type)<sizeof(stest_table);k++)
    {
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,zram,SPATT,LW);
 
          stencil.sfbp$l_whole = 0;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_wmask = 0xff;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_rmask = 0xff;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_stest = stest_table[k].stest;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_sfail = stest_table[k].sfail;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zfail = replace;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zpass = replace;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_ztest = always;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_z     = replace;

          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,stencil.sfbp$l_whole,LW);
         
          mode_reg  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);
          mode_reg |= sfbp$k_simple_z;
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,mode_reg,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         
          if (data_ptr->sfbp$l_print) {printf ("test %d %s svalue 0x%x\n",stest_table[k].testno,
                        stest_table[k].text,stest_table[k].svalue);}

          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_base_addr,z.base_addr,LW);
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_fraction,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_whole,stest_table[k].svalue,LW);


          /* 1.1 JMP add planemask write...should have no effect        */
          /* Except if should fail on IBM parts.                        */
         
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,stest_table[k].fvalue,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

          actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,zram);
          if (data_ptr->sfbp$l_print) {printf ("Read Z Address 0x%x value 0x%x\n",zram,actual);}

          if (actual != stest_table[k].expect )
              { 
              param[0] = data_ptr->sfbp$l_vram_base | (int)fram;                                     
              param[1] = stest_table[k].expect;                                       
              param[2] = actual;                                        
              s= 0;
              }  

         (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
         (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);
         }

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);

return (s);
}





/*+
* ===========================================================================
* = simz$$zfail_test - stencil Mode Z buffer test =
* ===========================================================================
*
* OVERVIEW:
*       stencil Mode Z Buffer Test 
* 
* FORM OF CALL:
*       simz$$zfail_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*               Stencil Z depth fail test
--*/
int     simz$$zfail_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register        int             zfill,i,j,k,y,u,w,f,s=1,num_bytes;
register        int             expect,sdata,zdata,mode_reg,actual,zvalue;
int             zram,fram;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;
union           stencil_reg stencil;
int             fraction=0,whole=0;
union           zfraction_reg   *zf;
union           zvalue_reg      *zv;

/* Stencil Passes but Z fails   */
static          struct  stest_table_type 
         {
         int    testno;  
         char   *text;   
         STEST  ztest;   
         SFAIL  zfail;                            
         int    svalue;  
         int    fvalue;  
         int    expect; 
         } stest_table [] =
         {
         1, ">=",    geq,         keep,      ZSPATT-1,       FPATT,          SPATT,      
         2, ">=",    geq,         zero,      ZSPATT-1,       FPATT,          0,      
         3, ">=",    geq,         replace,   ZSPATT-1,       FPATT,          SPATT,
         4, ">=",    geq,         incr,      ZSPATT-1,       FPATT,          SPATT+0x01000000,
         5, ">=",    geq,         decr,      ZSPATT-1,       FPATT,          SPATT-0x01000000,
         6, ">=",    geq,         invert,    ZSPATT-1,       FPATT,          IPATT,
        };


CHECK_CONFIG();

fram  = data_ptr->sfbp$l_console_offset;
zf    = (union zfraction_reg   *)&fraction;
zv    = (union zvalue_reg      *)&whole;

if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
     zram           =  sfbp$k_a_sfbp_zbuf_mem_32 - data_ptr->sfbp$l_vram_base;
else zram      =  fram + 10*data_ptr->sfbp$l_bytes_per_scanline;

expect         = sfbp$k_def_lw_red;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
zfill          = ZSPATT;

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,fram,0,num_bytes);                      
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,zram,zfill,num_bytes);                      

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = 0;
 visual.boolean   = GXcopy;
 
 if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
        z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 ;       
 else z.base_addr       = (int)zram;

 for (s=1,k=0;s&1 && k*sizeof (struct stest_table_type)<sizeof(stest_table);k++)
    {
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,zram,zfill,LW);
 
          stencil.sfbp$l_whole = 0;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_wmask = 0xff;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_rmask = 0xff;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_stest = always;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_sfail = replace;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zfail = stest_table[k].zfail;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zpass = keep;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_ztest = never;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_z     = replace;

          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,stencil.sfbp$l_whole,LW);
         (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         
          mode_reg  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);
          mode_reg |= sfbp$k_simple_z;
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,mode_reg,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         
          if (data_ptr->sfbp$l_print) {printf ("test %d %s svalue 0x%x\n",stest_table[k].testno,
                        stest_table[k].text,stest_table[k].svalue);}

          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_base_addr,z.base_addr,LW);
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_fraction,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_whole,stest_table[k].svalue,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

          /* 1.1 JMP add planemask write...should have no effect        */
          /* Except if should fail on IBM parts. Since I have only      */
         
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,stest_table[k].fvalue,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

          actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,zram) & 0xff000000;
          if (data_ptr->sfbp$l_print) {printf ("Read Z Address 0x%x value 0x%x\n",zram,actual);}

          if (actual != stest_table[k].expect || data_ptr->sfbp$l_test_error )
              { 
              param[0] = data_ptr->sfbp$l_vram_base | zram;                                     
              param[1] = stest_table[k].expect;                                       
              param[2] = actual;                                        
              s=0;
             }  

         (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
         (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);
         }

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

return (s);
}





/*+
* ===========================================================================
* = simz$$zpass_test - stencil Mode Z buffer test =
* ===========================================================================
*
* OVERVIEW:
*       stencil Mode Z Buffer Test 
* 
* FORM OF CALL:
*       simz$$zpass_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*               Stencil Z pass test
--*/
int     simz$$zpass_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{

register        int             zfill,i,j,k,y,u,w,f,s=1,num_bytes;
register        int             expect,sdata,zdata,mode_reg,actual,zvalue;
int             zram,fram;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;
union           stencil_reg stencil;
int             fraction=0,whole=0;
union           zfraction_reg   *zf;
union           zvalue_reg      *zv;

/* Stencil Passes and Z Passes too */
static          struct  stest_table_type 
         {
         int    testno;  
         char   *text;   
         STEST  ztest;   
         SFAIL  zpass;                            
         int    svalue;  
         int    fvalue;  
         int    expect; 
         } stest_table [] =
         {
         1, ">=",    geq,         keep,      ZSPATT,       FPATT,          SPATT,      
         2, ">=",    geq,         zero,      ZSPATT,       FPATT,          0,      
         3, ">=",    geq,         replace,   ZSPATT,       FPATT,          SPATT,
         4, ">=",    geq,         incr,      ZSPATT,       FPATT,          SPATT+0x01000000,      
         5, ">=",    geq,         decr,      ZSPATT,       FPATT,          SPATT-0x01000000,      
         6, ">=",    geq,         invert,    ZSPATT,       FPATT,          IPATT,
         };


CHECK_CONFIG();

zf    = (union zfraction_reg   *)&fraction;
zv    = (union zvalue_reg      *)&whole;
fram  = data_ptr->sfbp$l_console_offset;

if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
     zram           =  sfbp$k_a_sfbp_zbuf_mem_32 - data_ptr->sfbp$l_vram_base;
else zram      =  fram + 10*data_ptr->sfbp$l_bytes_per_scanline;

expect         = sfbp$k_def_lw_red;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
zfill          = ZSPATT;

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,fram,0,num_bytes);                      
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,zram,zfill,num_bytes);                      

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = 0;
 visual.boolean   = GXcopy;

 if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
        z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 ;       
 else z.base_addr       = (int)zram;

 for (s=1,k=0;s&1 && k*sizeof (struct stest_table_type)<sizeof(stest_table);k++)
    {
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,zram,zfill,LW);
 
          stencil.sfbp$l_whole = 0;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_wmask = 0xff;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_rmask = 0xff;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_stest = always;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_sfail = replace;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zfail = replace;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zpass = stest_table[k].zpass;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_ztest = always;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_z     = replace;

          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,stencil.sfbp$l_whole,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         
          mode_reg  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);
          mode_reg |= sfbp$k_simple_z;
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,mode_reg,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         
          if (data_ptr->sfbp$l_print) {printf ("test %d %s svalue 0x%x\n",stest_table[k].testno,
                        stest_table[k].text,stest_table[k].svalue);}

          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_base_addr,z.base_addr,LW);
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_fraction,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_whole,stest_table[k].svalue,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

          /* 1.1 JMP add planemask write...should have no effect        */
          /* Except if should fail on IBM parts. Since I have only      */
         
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,0,LW);

          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,stest_table[k].fvalue,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

          actual = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,zram) & 0xff000000; 
          if (data_ptr->sfbp$l_print) {printf ("Read Z Address 0x%x value 0x%x\n",zram,actual);}

          if (actual != stest_table[k].expect || data_ptr->sfbp$l_test_error )
              { 
              param[0] = data_ptr->sfbp$l_vram_base | (int)zram;                                     
              param[1] = stest_table[k].expect;                                       
              param[2] = actual;                                        
              s =0;
              }  

         (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
         (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
          /* Restore plane mask for next loop   */
         (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);


         }

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

return (s);
}




/*+
* ===========================================================================
* = simz$$rmask_test - stencil Mode Z buffer test =
* ===========================================================================
*
* OVERVIEW:
*       stencil Mode Z Buffer Test 
* 
* FORM OF CALL:
*       simz$$rmask_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*               Stencil read mask test
*
--*/
int     simz$$rmask_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{

register        int             zfill,i,j,k,y,u,w,f,s=1,num_bytes;
register        int             expect,sdata,zdata,mode_reg,actual,zvalue;
int             mask,zram,fram;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;
union           stencil_reg stencil;
int             fraction=0,whole=0;
union           zfraction_reg   *zf;
union           zvalue_reg      *zv;

/* Stencil Passes and Z Passes too */
static          struct  stest_table_type 
         {
         int    testno;  
         char   *text;   
         STEST  ztest;   
         SFAIL  zpass;                            
         int    svalue;  
         int    fvalue;  
         int    expect; 
         } stest_table [] =
         {
         1, ">=",    geq,         keep,      ZSPATT,       FPATT,          SPATT,      
         2, ">=",    geq,         zero,      ZSPATT,       FPATT,          0,      
         3, ">=",    geq,         replace,   ZSPATT,       FPATT,          SPATT,
         4, ">=",    geq,         incr,      ZSPATT,       FPATT,          SPATT+0x01000000,      
         5, ">=",    geq,         decr,      ZSPATT,       FPATT,          SPATT-0x01000000,      
         6, ">=",    geq,         invert,    ZSPATT,       FPATT,          IPATT,
         };

CHECK_CONFIG();

zf    = (union zfraction_reg   *)&fraction;
zv    = (union zvalue_reg      *)&whole;
fram  = data_ptr->sfbp$l_console_offset;

if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
     zram           =  sfbp$k_a_sfbp_zbuf_mem_32 - data_ptr->sfbp$l_vram_base;
else zram      =  fram + 10*data_ptr->sfbp$l_bytes_per_scanline;

expect         = sfbp$k_def_lw_red;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
zfill          = ZSPATT;

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,fram,0,num_bytes);                      
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,zram,zfill,num_bytes);                      

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = 0;
 visual.boolean   = GXcopy;

 if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
        z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 ;       
 else z.base_addr       = (int)zram;

 for (s=1,j=0,mask=1;s&1 && j<8;j++,mask<<=1)
 {
 for (k=0;s&1 && k*sizeof (struct stest_table_type)<sizeof(stest_table);k++)
    {
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,zram,zfill,LW);
 
          stencil.sfbp$l_whole = 0;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_wmask = 0xff;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_rmask = mask;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_stest = always;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_sfail = replace;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zfail = replace;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zpass = stest_table[k].zpass;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_ztest = always;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_z     = replace;

          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,stencil.sfbp$l_whole,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         
          mode_reg  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);
          mode_reg |= sfbp$k_simple_z;
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,mode_reg,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         
          if (data_ptr->sfbp$l_print) {printf ("test %d %s svalue 0x%x\n",stest_table[k].testno,
                        stest_table[k].text,stest_table[k].svalue);}

          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_base_addr,z.base_addr,LW);
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_fraction,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_whole,stest_table[k].svalue,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

          /* 1.1 JMP add planemask write...should have no effect        */
          /* Except if should fail on IBM parts. Since I have only      */
         
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,stest_table[k].fvalue,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 
          actual = ((*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,zram) & 0xff000000) &(mask<<24) ;
          if (data_ptr->sfbp$l_print) {printf ("Read Z Address 0x%x value 0x%x\n",zram,actual);}

          if (actual != (stest_table[k].expect & (mask<<24)) || data_ptr->sfbp$l_test_error )
              { 
              param[0] = data_ptr->sfbp$l_vram_base | (int)zram;                                     
              param[1] = stest_table[k].expect & (mask << 24);                                       
              param[2] = actual;                                        
              s=0;
              }  

         (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
         (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);
         }
     }
    
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 
return (s);
}




/*+
* ===========================================================================
* = simz$$wmask_test - stencil Mode Z buffer test =
* ===========================================================================
*
* OVERVIEW:
*       stencil Mode Z Buffer Test 
* 
* FORM OF CALL:
*       simz$$wmask_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
*
* FUNCTIONAL DESCRIPTION:
*               Stencil write mask test
*
--*/
int     simz$$wmask_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{

register        int             zfill,i,j,k,y,u,w,f,s=1,num_bytes;
register        int             expect,sdata,zdata,mode_reg,actual,zvalue;
int             mask,zram,fram;
COLOR           color;
STIP            stip;
VISUAL          visual;
Z               z;
union           stencil_reg stencil;
int             fraction=0,whole=0;
union           zfraction_reg   *zf;
union           zvalue_reg      *zv;

/* Stencil Passes and Z Passes too */
static          struct  stest_table_type 
         {
         int    testno;  
         char   *text;   
         STEST  ztest;   
         SFAIL  zpass;                            
         int    svalue;  
         int    fvalue;  
         int    expect; 
         } stest_table [] =
         {
         1, ">=",    geq,         keep,      ZSPATT,       FPATT,          SPATT,      
         2, ">=",    geq,         zero,      ZSPATT,       FPATT,          0,      
         3, ">=",    geq,         replace,   ZSPATT,       FPATT,          SPATT,
         4, ">=",    geq,         incr,      ZSPATT,       FPATT,          SPATT+0x01000000,      
         5, ">=",    geq,         decr,      ZSPATT,       FPATT,          SPATT-0x01000000,      
         6, ">=",    geq,         invert,    ZSPATT,       FPATT,          IPATT,
         };

CHECK_CONFIG();

zf    = (union zfraction_reg   *)&fraction;
zv    = (union zvalue_reg      *)&whole;
fram  = data_ptr->sfbp$l_console_offset;

if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
     zram           =  sfbp$k_a_sfbp_zbuf_mem_32 - data_ptr->sfbp$l_vram_base;
else zram      =  fram + 10*data_ptr->sfbp$l_bytes_per_scanline;

expect         = sfbp$k_def_lw_red;
num_bytes      = data_ptr->sfbp$l_bytes_per_scanline;
zfill          = ZSPATT;

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,fram,0,num_bytes);                      
(*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,zram,zfill,num_bytes);                      

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.line_type = 0;
 visual.boolean   = GXcopy;

 if (data_ptr->sfbp$l_vram_size/sfbp$k_one_mill  == 16 )
        z.base_addr     = sfbp$k_a_sfbp_zbuf_mem_32 ;       
 else z.base_addr       = (int)zram;

for (s=1,j=0,mask=1;s&1 && j<8;j++,mask<<=1)
 {
 for (k=0;s&1 && k*sizeof (struct stest_table_type)<sizeof(stest_table);k++)
    {
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,zram,zfill,LW);
 
          stencil.sfbp$l_whole = 0;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_wmask = mask;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_rmask = 0xff;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_stest = always;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_sfail = replace;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zfail = replace;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_zpass = stest_table[k].zpass;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_ztest = always;
          stencil.sfbp$r_stencil_bits.sfbp$v_stencil_z     = replace;

          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,stencil.sfbp$l_whole,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         
          mode_reg  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);
          mode_reg |= sfbp$k_simple_z;

          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,mode_reg,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         
          if (data_ptr->sfbp$l_print) {printf ("test %d %s svalue 0x%x\n",stest_table[k].testno,
                        stest_table[k].text,stest_table[k].svalue);}

          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_base_addr,z.base_addr,LW);
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_fraction,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_z_value_whole,stest_table[k].svalue,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

          /* 1.1 JMP add planemask write...should have no effect        */
          /* Except if should fail on IBM parts. Since I have only      */
         
          (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,0,LW);
          (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,fram,stest_table[k].fvalue,LW);
          (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

          actual = ((*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,zram) & 0xff000000) &(mask<<24) ;
          if (data_ptr->sfbp$l_print) {printf ("Read Z Address 0x%x value 0x%x\n",zram,actual);}

          if (actual != (stest_table[k].expect & (mask<<24)) || data_ptr->sfbp$l_test_error )
              {  
              param[0] = data_ptr->sfbp$l_vram_base | (int)zram;                                     
              param[1] = stest_table[k].expect & (mask << 24);                                       
              param[2] = actual;                                        
              s=0;
              }  

         (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
         (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
         (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);
         }
     }

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_stencil_mode,0,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

return (s);
}

