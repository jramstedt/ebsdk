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
#include "cp$src:tga_sfbp_dithermatrix.h"

#define   CHECK_DITHER() {if (data_ptr->sfbp$l_module == hx_32_plane_z  || \
                              data_ptr->sfbp$l_module == hx_32_plane_no_z )return (1);}


/*+
* ===========================================================================
* = reg$$write_read_test - Register Write Read Test =
* ===========================================================================
*
* OVERVIEW:
*       Verify registers can be written and read
* 
* FORM OF CALL:
*       reg$$write_read_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Register Test Write and Read. This will write all registers
*       with a data pattern and read it back. 
*

====================
= SFB+ Address Map =
====================

	The SFB+ address space is divided into three major regions:

	1) ROM space occupies the first 1MB.  Reads to consecutive words in
	   this region return consecutive bytes in the option rom.  Writes
	   to ROM space are interpreted as aliases of the address and continue
	   registers.

	2) REG space occupies the next 1MB. The SFB+ only decodes the
	   low ten bits of addresses in this region allowing software
	   to create virtual aliases of the SFB+ registers within this region.

	3) FB space is programmable in size and depth through the deep
	   register.  Supported configurations are as follows:



	For 8 plane systems the deep bit of the deep register must be set to
	zero.  The mask field of the deep register determines the size of the
	and interpretation of the frame buffer address region.

	deep.deep = 0  (8 plane 2M pixel systems)

	                deep.mask = 0   deep.mask = 1   deep.mask = 3
        base + 000.0000 +-----------+   +-----------+   +-----------+
                        |    ROM    |   |    ROM    |   |    ROM    |
        base + 010.0000 +-----------+   +-----------+   +-----------+
                        |    REG    |   |    REG    |   |    REG    |
        base + 020.0000 +-----------+   +-----------+   +-----------+
                        |   Frame   |   |    XXX    |   |           |
                        |  Buffer 0 |   |           |   |           |
        base + 040.0000 +-----------+   +-----------+   |           |
                                        |   Frame   |   |    XXX    |
                                        |  Buffer 0 |   |           |
                                        +------------   |           |
                                        |   Back    |   |           |
                                        |  Buffer 0 |   |           |
        base + 080.0000                 +-----------+   +-----------+
                                                        |   Frame   |
                                                        |  Buffer 0 |
                                                        +-----------+
                                                        |   Back    |
                                                        |  Buffer 0 |
        base + 0c0.0000                                 +-----------+
                                                        |   Frame   |
                                                        |  Buffer 1 |
                                                        +-----------+
                                                        |   Back    |
                                                        |  Buffer 1 |
                                                        +-----------+

	For 32 plane systems the deep bit of the deep register must be set to
	one.  The mask field of the deep register determines the size of the
	and interpretation of the frame buffer address region.
	

	deep.deep = 1  (32 plane 2M pixel systems)

	                deep.mask = 3   deep.mask = 7
        base + 000.0000 +-----------+   +-----------+
                        |    ROM    |   |    ROM    |
        base + 010.0000 +-----------+   +-----------+
                        |    REG    |   |    REG    |
        base + 020.0000 +-----------+   +-----------+
                        |           |   |           |
                        |           |   |           |
                        |    XXX    |   |           |
                        |           |   |           |
                        |           |   |           |
                        |           |   |           |
        base + 080.0000 +-----------+   |    XXX    |
                        |           |   |           |
                        |           |   |           |
                        |           |   |           |
                        |   Frame   |   |           |
                        |  Buffer 0 |   |           |
                        |           |   |           |
                        |           |   |           |
                        |           |   |           |
        base + 100.0000 +-----------+   +-----------+
                                        |           |
                                        |           |
                                        |           |
                                        |   Frame   |
                                        |  Buffer 0 |
                                        |           |
                                        |           |
                                        |           |
        base + 180.0000                 +-----------+
                                        |           |
                                        |           |
                                        |           |
                                        |   Back    |
                                        |  Buffer 0 |
                                        |           |
                                        |           |
                                        |           |
                                        +-----------+

=========================
= Rom space Address Map =
=========================

      ========================================================================
          Address              Size            Definition  (Read)
      ========================================================================
      base + 00.0000           8 bits       Rom byte 0                   R
      base + 00.0004           8 bits       Rom byte 1                   R
          ...
      base + 0f.fffc           8 bits       Rom byte 256K-1              R


      ========================================================================
         Address               Size            Definition  (Write)
      ========================================================================
      base + 00.xxx0          24 bits       Address                      W
      base + 00.xxx4          32 bits       Continue                     W
      base + 08.xxxx          32 bits       Continue                     W


========================
= Register Address Map =
========================

      ========================================================================
         Address               Size            Definition
      ========================================================================
      base + 10.0000          32 bits       Copy Buffer Register0        RW
      base + 10.0004          32 bits       Copy Buffer Register1        RW
      base + 10.0008          32 bits       Copy Buffer Register2        RW
      base + 10.000C          32 bits       Copy Buffer Register3        RW
      base + 10.0010          32 bits       Copy Buffer Register4        RW
      base + 10.0014          32 bits       Copy Buffer Register5        RW
      base + 10.0018          32 bits       Copy Buffer Register6        RW
      base + 10.001C          32 bits       Copy Buffer Register7        RW

      base + 10.0020          32 bits       Foreground                   RW
      base + 10.0024          32 bits       Background                   RW
      base + 10.0028          32 bits       PlaneMask                    W
      base + 10.002C          32 bits       OneShot PixelMask            RW
      base + 10.0030          15 bits       Mode                         RW *
      base + 10.0034           4 bits       Raster Op                    RW
      base + 10.0038           4 bits       PixelShift                   RW
      base + 10.003C          24 bits       Address                      RW

      base + 10.0040          32 bits       Bresenham 1                  RW
      base + 10.0044          32 bits       Bresenham 2                  RW
      base + 10.0048          21 bits       Bresenham 3                  RW *
      base + 10.004C          32 bits       Continue / BresZ (read)      RW *
      base + 10.0050          15 bits       Deep                         RW
      base + 10.0054          32 bits       Start / Revision (read)      RW *
      base + 10.0058          32 bits       Stencil Mode                 RW
      base + 10.005c          32 bits       Persistent PixelMask         W

      base + 10.0060          10 bits       Cursor Base Address          RW
      base + 10.0064          29 bits       Horizontal Control           RW
      base + 10.0068          28 bits       Vertical Control             RW
      base + 10.006c           9 bits       Video Base Address           W
      base + 10.0070           2 bits       Video Valid                  RW
      base + 10.0074          25 bits       Cursor XY                    RW
      base + 10.0078          18 bits       Video Shift Address          RW *
      base + 10.007c          10 bits       Interrupt Status             RW *

      base + 10.0080          32 bits       Data                         RW
      base + 10.0084          20 bits       Red   Increment              W
      base + 10.0088          20 bits       Green Increment              W
      base + 10.008c          20 bits       Blue  Increment              W
      base + 10.0090          32 bits       Z Increment (low)            RW
      base + 10.0094           4 bits       Z Increment (high)           RW
      base + 10.0098          32 bits       DMA Base Address             RW
      base + 10.009c          32 bits       Bresenham Width		 RW

      base + 10.00a0          32 bits       Z value (low)                RW
      base + 10.00a4           4 bits       Z value (high)               RW *
      base + 10.00a8          24 bits       Z Base Address               W
      base + 10.00ac          24 bits       Address                      W
      base + 10.00b0          25 bits       Red   Value                  RW *
      base + 10.00b4          25 bits       Green Value                  RW *
      base + 10.00b8          20 bits       Blue  Value                  RW
      base + 10.00bc          32 bits       Span Width / Dither (read)   RW *

      base + 10.00c0          32 bits       Ramdac Setup                 W

      base + 10.0100          32 bits       SlopeNoGo (|-dx| < |-dy|)    W  **
      base + 10.0104          32 bits       SlopeNoGo (|-dx| < |+dy|)    W  **
      base + 10.0108          32 bits       SlopeNoGo (|+dx| < |-dy|)    W  **
      base + 10.010c          32 bits       SlopeNoGo (|+dx| < |+dy|)    W  **
      base + 10.0110          32 bits       SlopeNoGo (|-dx| > |-dy|)    W  **
      base + 10.0114          32 bits       SlopeNoGo (|-dx| > |+dy|)    W  **
      base + 10.0118          32 bits       SlopeNoGo (|+dx| > |-dy|)    W  **
      base + 10.011c          32 bits       Slope
      base + 10.0120          32 bits       Slope     (|-dx| < |-dy|)    W
      base + 10.0124          32 bits       Slope     (|-dx| < |+dy|)    W
      base + 10.0128          32 bits       Slope     (|+dx| < |-dy|)    W
      base + 10.012c          32 bits       Slope     (|+dx| < |+dy|)    W
      base + 10.0130          32 bits       Slope     (|-dx| > |-dy|)    W
      base + 10.0134          32 bits       Slope     (|-dx| > |+dy|)    W
      base + 10.0138          32 bits       Slope     (|+dx| > |-dy|)    W
      base + 10.013c          32 bits       Slope     (|+dx| > |+dy|)    W

      base + 10.0140          32 bits       Block mode color 0           W
      base + 10.0144          32 bits       Block mode color 1           W
      base + 10.0148          32 bits       Block mode color 2           W
      base + 10.014c          32 bits       Block mode color 3           W
      base + 10.0150          32 bits       Block mode color 4           W
      base + 10.0154          32 bits       Block mode color 5           W
      base + 10.0158          32 bits       Block mode color 6           W
      base + 10.015c          32 bits       Block mode color 7           W

      base + 10.0160          24 bits       Copy64 Source Register       W
      base + 10.0164          24 bits       Copy64 Destination Register  W
      base + 10.0168          24 bits       Copy64 Source Reg. (alias)   W
      base + 10.016c          24 bits       Copy64 Dest Reg. (alias)     W

      base + 10.01e0          26 bits       EPROM Write                  W
      base + 10.01e4                        RESERVED
      base + 10.01e8           2 bits       Clock                        W
      base + 10.01ec                        RESERVED
      base + 10.01f0           8 bits       Ramdac Interface             RW *
      base + 10.01f4                        RESERVED
      base + 10.01f8           1 bit        Command Status               RW *
      base + 10.01fc           1 bit        Command Status               RW *

      *  - Registers that do not read exactly as written, see spec for details.

      ** - Reads to the SlopeNoGo registers will return data from the second
	   half of the CopyBuffer.
*
--*/
int     reg$$write_read_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
#define  REG$$WR 0                      /* read should = write  */
#define  REG$$WO 1                      /* Write only           */
#define  REG$$RO 2                      /* Read only            */
#define  REG$$NA 3                      /* dont touch it        */
#define  REG$$DD 4                      /* Read Not Equal Write */

int      r,w,temp;
int      expect,pattern,stuck=0,i,j,s=1;
static   char *state[2]={"Write ","Read "};
static   struct reg_type {
        int     index;                                  
        int     access;                                 
        int     pattern;                                
        int     expect;                                 
        char    *text;
        } reg_table [] = {                              
         0,REG$$WO,0x55555555,0x55555555,      "0x100000 copy register 0      ",	 
         1,REG$$WO,0x55555555,0x55555555,      "0x100004 copy register 1      ",	 
         2,REG$$WO,0x55555555,0x55555555,      "0x100008 copy register 2      ",	 
         3,REG$$WO,0x55555555,0x55555555,      "0x10000C copy register 3      ",	 
         4,REG$$WO,0x55555555,0x55555555,      "0x100010 copy register 4      ",	 
         5,REG$$WO,0x55555555,0x55555555,      "0x100014 copy register 5      ",	 
         6,REG$$WO,0x55555555,0x55555555,      "0x100018 copy register 6      ",	 
         7,REG$$WO,0x55555555,0x55555555,      "0x10001C copy register 7      ",	 
         8,REG$$WR,0x55555555,0x55555555,      "0x100020 foreground register  ",	 
         9,REG$$WR,0xAAAAAAAA,0xAAAAAAAA,      "0x100024 background register  ",	 
        10,REG$$WO,0xffffffff,0xffffffff,      "0x100028 plane mask           ",	 
        11,REG$$WR,0xffffffff,0xffffffff,      "0x10002C one shot pixel mask  ",	 
        12,REG$$DD,0x00000000,0x00000000,      "0x100030 sfbp mode            ",	 
        13,REG$$WR,0x00000003,0x00000003,      "0x100034 boolean operation    ",	 
        14,REG$$WR,0x00000005,0x00000005,      "0x100038 pixel shift          ",	 
        15,REG$$WO,0x00000000,0x00000000,      "0x10003C line address         ",	 
        16,REG$$WO,0x00000000,0xffffffff,      "0x100040 bresh 1              ",	 
        17,REG$$WO,0x00000000,0xffffffff,      "0x100044 bresh 2              ",	 
        18,REG$$DD,0x00000000,0xffffffff,      "0x100048 bresh 3              ",	 
        19,REG$$DD,0x00000000,0x00000000,      "0x10004C bresh continue/zinc1 ",	 
        20,REG$$NA,0x00000000,0x0000000f,      "0x100050 deep(do not touch)   ",	 
        21,REG$$DD,0x00000000,0x00000000,      "0x100054 start /rev           ",	 
        22,REG$$WR,0x00000000,0x00000000,      "0x100058 Stencil Mode         ",	 
        23,REG$$WO,0x55555555,0x55555555,      "0x10005C Persis Pixelmask     ",         
        24,REG$$WR,0x000003ff,0x000003f0,      "0x100060 Cursor Base Address  ",	 
        25,REG$$WR,0x03A80700,0x03A80700,      "0x100064 video horiz(130)     ",	 
        26,REG$$WR,0x88830360,0x88830360,      "0x100068 video vertical(130)  ",	 
        27,REG$$WO,0x55555555,0x55555555,      "0x10006C refresh base         ",	 
        28,REG$$WR,0x55555555,0x00000005,      "0x100070 video valid          ",	 
        29,REG$$WR,0x00000000,0x00000000,      "0x100074 Cursor XY            ",	 
        30,REG$$DD,0x55555555,0x55555555,      "0x100078 Video Shift Address  ",	 
        31,REG$$DD,0x0000000f,0x0000000f,      "0x10007C Interrupt Status     ",         
        32,REG$$WO,0x55555555,0x55555555,      "0x100080 Data Register        ",         
        33,REG$$WO,0x55555555,0x55555555,      "0x100084 Red Increment        ",         
        34,REG$$WO,0x55555555,0x55555555,      "0x100088 Green Increment      ",         
        35,REG$$WO,0x55555555,0x55555555,      "0x10008C Blue Increment       ",         
        36,REG$$WR,0x55555555,0x55555555,      "0x100090 Z increment-fraction ",         
        37,REG$$WR,0x55555555,0x00000005,      "0x100094 Z increment-whole    ",         
        38,REG$$WO,0x55555555,0x55555555,      "0x100098 DMA base Address     ",         
        39,REG$$WR,0x55555555,0x55555555,      "0x10009C Breshenham Width     ",         
        40,REG$$WR,0x55555555,0x55555555,      "0x1000A0 Z value - fraction   ",         
        41,REG$$DD,0x55555555,0x55555555,      "0x1000A4 Z value - whole      ",         
        42,REG$$WO,0x55555555,0x55555555,      "0x1000A8 Z base Address       ",         
        43,REG$$WO,0x00000000,0x00000000,      "0x1000AC Line Address         ",         
        44,REG$$DD,0x55555555,0x55555555,      "0x1000B0 Red Value            ",         
        45,REG$$DD,0x55555555,0x55555555,      "0x1000B4 Green Value          ",         
        46,REG$$WR,0x55555555,0x00055555,      "0x1000B8 Blue Value           ",         
        47,REG$$DD,0x00000000,0x00000000,      "0x1000BC Span Width/Dither    ",         
        48,REG$$WO,0x55555555,0x55555555,      "0x1000C0 Ramdac Setup         ",         
        64,REG$$WO,0x55555555,0x55555555,      "0x100100 Slope NoGo 0         ",         
        65,REG$$WO,0x55555555,0x55555555,      "0x100104 Slope NoGo 1         ",         
        66,REG$$WO,0x55555555,0x55555555,      "0x100108 Slope NoGo 2         ",         
        67,REG$$WO,0x55555555,0x55555555,      "0x10010C Slope NoGo 3         ",         
        68,REG$$WO,0x55555555,0x55555555,      "0x100110 Slope NoGo 4         ",         
        69,REG$$WO,0x55555555,0x55555555,      "0x100114 Slope NoGo 5         ",         
        70,REG$$WO,0x55555555,0x55555555,      "0x100118 Slope NoGo 6         ",         
        71,REG$$WO,0x55555555,0x55555555,      "0x10011C Slope NoGo 7         ",         
        72,REG$$DD,0x55555555,0x55555555,      "0x100120 Slope   Go 0         ",         
        73,REG$$DD,0x55555555,0x55555555,      "0x100124 Slope   Go 1         ",         
        74,REG$$DD,0x55555555,0x55555555,      "0x100128 Slope   Go 2         ",         
        75,REG$$DD,0x55555555,0x55555555,      "0x10012C Slope   Go 3         ",         
        76,REG$$DD,0x55555555,0x55555555,      "0x100130 Slope   Go 4         ",         
        77,REG$$DD,0x55555555,0x55555555,      "0x100134 Slope   Go 5         ",         
        78,REG$$DD,0x55555555,0x55555555,      "0x100138 Slope   Go 6         ",         
        79,REG$$DD,0x55555555,0x55555555,      "0x10013C Slope   Go 7         ",         
        80,REG$$WO,0x55555555,0x55555555,      "0x100140 Block Color 0        ",         
        81,REG$$WO,0x55555555,0x55555555,      "0x100144 Block Color 1        ",         
        82,REG$$WO,0x55555555,0x55555555,      "0x100148 Block Color 2        ",         
        83,REG$$WO,0x55555555,0x55555555,      "0x10014C Block Color 3        ",         
        84,REG$$WO,0x55555555,0x55555555,      "0x100150 Block Color 4        ",         
        85,REG$$WO,0x55555555,0x55555555,      "0x100154 Block Color 5        ",         
        86,REG$$WO,0x55555555,0x55555555,      "0x100158 Block Color 6        ",         
        87,REG$$WO,0x55555555,0x55555555,      "0x10015C Block Color 7        ",         
        88,REG$$WO,0x00000000,0x00000000,      "0x100160 Copy 64 Source       ",         
        89,REG$$WO,0x00000000,0x00000000,      "0x100164 Copy 64 Dest         ",         
        90,REG$$WO,0x00000000,0x00000000,      "0x100168 Copy 64 Source Alias ",         
        91,REG$$WO,0x00000000,0x00000000,      "0x10016C Copy 64 Dest Alias   ",         
        };
        
#define WRITEIT 0 
#define READIT  1


        
  for (i=0;s&1&&i*sizeof(struct reg_type)<sizeof(reg_table);i++)
  {
   w = (reg_table[i].index<<2);
   pattern = reg_table[i].pattern ;
   expect  = reg_table[i].expect  ;
   if (reg_table[i].access!=REG$$RO && 
       reg_table[i].access!=REG$$NA)
     {
     if (data_ptr->sfbp$l_print ){printf ("Write 0x%x %s \n",w,reg_table[i].text);}
     (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,w,pattern,LWMASK);
     }

     if (data_ptr->sfbp$l_print ){printf ("Read 0x%x %s \n",w,reg_table[i].text);}
     temp = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,w);                                             
     if (reg_table[i].access!=REG$$WO &&
         reg_table[i].access!=REG$$NA &&
         reg_table[i].access!=REG$$DD  )
     {
     if (temp  != expect || data_ptr->sfbp$l_test_error )
         {
         param[0] = sfbp$k_a_sfbp_reg_space | ((int)w);                                      
         param[1] = (int)expect;
         param[2] = (int)temp;                                   
         s = 0;
         if (data_ptr->sfbp$l_print )
                {printf ("Error 0x%x %s \n",w,reg_table[i].text);}
         }
      }
   }                        
 
(*cp_func_ptr->pv_init_output)(scia_ptr,data_ptr);


return (s);
}


/*+
* ===========================================================================
* = reg$$read_slope_nogo_test - Register Read SlopeNoGo Registers Test =
* ===========================================================================
*
* OVERVIEW:
*       Read the slope no go registers to get to copy64 upper 32 bytes.
* 
* FORM OF CALL:
*       reg$$read_slope_nogo_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       The upper 32 bytes of the 64 byte copy buffer are read by reading
*       the slope no go registers. The test is almost identical to the 
*       copy64 copy mode test, except we read the copy buffer. This is
*	how we read the upper 32 of copy buffer, read the slope nogo
*	registers, cute huh ?
*
--*/
int     reg$$read_slope_nogo_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,
	 SFBP *data_ptr,int param[])

{
register int      i,s=1,num_bytes;
register int  expect,actual,shift,*vram;
register char *src,*dst;
SCOPY   copy;
VISUAL   visual;
Z        z;

 /* Clear the nogo registers    */
 for (i=0;i<32;i+=4)
	(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_slope_nogo_0+i,0,LW);

 /* Do at least 8 scanlines for aliasing        */
 expect         = sfbp$k_def_lw_purple;
 num_bytes      = 8*data_ptr->sfbp$l_bytes_per_scanline;
 shift          = 0;
 src            = (char *)data_ptr->sfbp$l_console_offset;
 dst            = (char *)(src+(data_ptr->sfbp$l_bytes_per_row*(data_ptr->sfbp$l_cursor_max_row/2)));
 vram           = (int *)dst;

 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,src,expect,num_bytes);                      
 (*gfx_func_ptr->pv_fill_vram) (scia_ptr,data_ptr,dst,0,num_bytes);                      
 
 copy.src_address = data_ptr->sfbp$l_planes==8 ? (int)src : (int)(src)/4;
 copy.dst_address = data_ptr->sfbp$l_planes==8 ? (int)dst : (int)(dst)/4;
 copy.size        = num_bytes;
 copy.copy        = copy64;
 copy.boolean_op  = GXcopy;
 copy.copymask    = M1;
 copy.pixel_shift = 0;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = copy.boolean_op;

 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 /* Fill up the copy buffer     */
 (*gfx_func_ptr->pv_sfbp_aligned_copy) (scia_ptr,data_ptr,&copy,&visual,&z);

 /* And now read the nogo registers and verify it */
 /* It should be our data in the copy buffer      */
 for (i=0;s&1 && i<32;i+=4)
     {
     actual = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_slope_nogo_0+i);
     if (actual != expect )
        {
        param[0] = sfbp$k_a_sfbp_reg_space | (sfbp$k_slope_nogo_0+i);
        param[1] = expect ;               
        param[2] = actual;                
        s = 0;
        }
     }
     
return (s);
}


/*+
* ===========================================================================
* = reg$$read_span_test - Register Read Span Registers Test =
* ===========================================================================
*
* OVERVIEW:
*       Read the Span Register bits 
* 
* FORM OF CALL:
*       reg$$read_span_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Read the span register bits to verify. This test will draw a 
*       dithered line for Xa < Xb slope < 1 and verify the values
*       red are proper.
*
	The Span Width Register is an alias for the Slope Register |+dx| >
	|+dy|.  Normally this register is used to paint horizontal spans,
	and so will have some positive dx, and dy = 0.

	When read, the Span Width register returns the register values
	pertaining to dithering.

	31                                 13 12      8 7       3 2 1 0
	+------------------------------------+---------+---------+-+-+-+
	|                 ignore             |   row   |   col   |g|x|y|
	+--------------------------------------------------------------+

	row: row value last written to the Red   Value register
	col: col value last written to the Green Value register
	g:   dxGEdy from last slope write
	x:   dxGE0  from last slope write
	y:   dyGE0  from last slope write

*
--*/
int     reg$$read_span_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{


int       temp,i,j,s=1,fg=TRUE;
int       dx,dy,dxGEdy,method,startx,starty,endx,endy,cnt ;
LINE      line;
COLOR     color;
VISUAL    visual;
Z         z;
union     color_value_reg INC,RED,GRN,BLU;
int       section,fractional_intensity,whole_intensity;
SPAN_BITS *span;


 CHECK_DITHER();

 startx    =  0;
 endx      =  data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels;
 starty    = 0;
 endy      = starty;

 line.draw_method = line_address;
 line.speed       = fast;
 line.boolean_op  = GXcopy;
 line.nogo        = FALSE;
 line.span_line   = TRUE;
 line.verify      = TRUE;
 line.window_type = Xwindow;
 line.polyline    = FALSE;
 line.wait_buffer = TRUE;
 line.color_change= TRUE;

 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.capend    = FALSE;
 visual.doz       = FALSE;
 visual.boolean   = line.boolean_op;
 
 color.fg_value   = 0;
 
 RED.sfbp$l_whole = GRN.sfbp$l_whole = BLU.sfbp$l_whole = INC.sfbp$l_whole =0;
 
 fractional_intensity = (4096 * 256) / endx; 
 whole_intensity      = 0;
 
 line.xa   = startx;
 line.ya   = starty;
 line.xb   = endx;
 line.yb   = endy;

 line.line = data_ptr->sfbp$l_planes == 8 ? cint_d_line_mode : cint_nd_line_mode;
  
 INC.sfbp$r_color_value_bits.sfbp$v_fraction = fractional_intensity;
 INC.sfbp$r_color_value_bits.sfbp$v_value    = whole_intensity;
 
 RED.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
 RED.sfbp$r_color_value_bits.sfbp$v_value    = 0;
 RED.sfbp$r_color_value_bits.sfbp$v_dither   = line.ya&31;
         
 GRN.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
 GRN.sfbp$r_color_value_bits.sfbp$v_value    = 0;
 GRN.sfbp$r_color_value_bits.sfbp$v_dither   = line.xa&31;
        
 BLU.sfbp$r_color_value_bits.sfbp$v_fraction = 0;
 BLU.sfbp$r_color_value_bits.sfbp$v_value    = 0;
 
 color.red_value   = RED.sfbp$l_whole;
 color.green_value = GRN.sfbp$l_whole;
 color.blue_value  = BLU.sfbp$l_whole;

 color.red_increment   = INC.sfbp$l_whole;
 color.green_increment = INC.sfbp$l_whole;
 color.blue_increment  = INC.sfbp$l_whole;
 
 s = (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
 s = (*parse_func_ptr->pv_verify_line ) (scia_ptr,data_ptr,&line,&color,&visual,&z,param,DROM);

 temp = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_span_width);                                             
 span = (SPAN_BITS *)&temp;
 
 /* Check the bits    */
 /* horizontal line   */

 if (data_ptr->sfbp$l_print) 
        printf ("Span Read 0x%x\n",temp);
        
 if (( span->sfbp$r_span_reg_bits.sfbp$v_dyge0) && 
     ( span->sfbp$r_span_reg_bits.sfbp$v_dxge0) && 
     ( span->sfbp$r_span_reg_bits.sfbp$v_dxgedy) && 
     ( span->sfbp$r_span_reg_bits.sfbp$v_dcol==0) && 
     ( span->sfbp$r_span_reg_bits.sfbp$v_drow==0))s=1;
 else
     {
     param[0] = sfbp$k_a_sfbp_reg_space | (sfbp$k_span_width );
     param[1] = 7 ;               
     param[2] = temp;
     s = 0;
     }
     

return (s);
}



/*+
* ===========================================================================
* = reg$$read_mode_copy - Register Read Mode Copy Bit Test =
* ===========================================================================
*
* OVERVIEW:
*       Read the mode copy bit 
* 
* FORM OF CALL:
*       reg$$read_mode_copy_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       The mode register copy bit gets set on the first fill of the copy buffer
*       and clear on the drain. This test will verify that by doing a write
*       to the copy64 registers.
*
--*/
int     reg$$read_mode_copy_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int i,s=1;
register char *src,*dst;
union    control_reg *MODE;
int      mode;

src     = (char *)data_ptr->sfbp$l_console_offset;
dst     = src + 0x1000;

 /* Write Source         */
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_copy64_src,src,LW);

 mode  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);
 MODE  = (union control_reg *)&mode;

 if (MODE->sfbp$r_control_bits.sfbp$v_con_copy_flag==1)
  {
  param[0] = sfbp$k_a_sfbp_reg_space | sfbp$k_mode;
  param[1] = 1;
  param[2] = 0;
  s=0;
  if (data_ptr->sfbp$l_print)
        printf ("Mode<20> bit did not clear\n");
  }

 /* Write Destination   */
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_copy64_dst,dst,LW);

 mode  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);

 if (MODE->sfbp$r_control_bits.sfbp$v_con_copy_flag==0)
  {
  param[0] = sfbp$k_a_sfbp_reg_space | sfbp$k_mode;
  param[1] = 1;
  param[2] = 0;
  s=0;
  if (data_ptr->sfbp$l_print)
        printf ("Mode<20> bit did not set\n");
  }

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);

     
return (s);
}



/*+
* ===========================================================================
* = reg$$read_mode_b3 - Register Read b3 Bit Test =
* ===========================================================================
*
* OVERVIEW:
*       Read the mode b3 change bit 
* 
* FORM OF CALL:
*       reg$$read_mode_b3_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Simply write bresh 3 and check mode bit to be set
*
--*/
int     reg$$read_mode_b3_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int i,s=1;
register char *src,*dst;
union    control_reg *MODE;
int      mode;
int      startx,starty,endx,endy;
LINE     line;
COLOR    color;
VISUAL   visual;
Z        z;


 startx           =  0;
 endx             =  15;
 starty           =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
 endy             =  data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines/2;
 
 line.line        = transparent_line_mode,
 line.span_line   = FALSE;
 line.speed       = fast;
 line.boolean_op  = GXcopy;
 line.verify      = TRUE;
 line.polyline    = FALSE;
 line.wait_buffer = TRUE;
 line.color_change= TRUE;
 line.draw_method = line_address;
 line.window_type = Xwindow;
 
 line.nogo        = go;

 line.xa          = startx;
 line.ya          = starty;
 line.xb          = endx;
 line.yb          = endy;
 
 visual.capend    = FALSE;
 visual.depth     = data_ptr->sfbp$l_module == hx_8_plane ? packed_8_packed : source_24_dest;
 visual.rotation  = (rotate_source_0<<2) | (rotate_dest_0);
 visual.boolean   = line.boolean_op;
 visual.doz       = FALSE;
 
 color.method     = foreground;
 color.fg_value   = sfbp$k_def_lw_red & data_ptr->sfbp$l_pixel_mask ;
 color.bg_value   = sfbp$k_def_lw_green & data_ptr->sfbp$l_pixel_mask ;
 
 (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);

 /* Write the B3 register                      */
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_bresh_reg3,0,LW);

 /* Mode<21> should be set now                 */
 mode  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);
 MODE  = (union control_reg *)&mode;

 if (MODE->sfbp$r_control_bits.sfbp$v_con_berror_flag==0)
  {
  param[0] = sfbp$k_a_sfbp_reg_space | sfbp$k_mode;
  param[1] = 1;
  param[2] = 0;
  s=0;
  if (data_ptr->sfbp$l_print)
        printf ("mode<21> did not set\n");
  }

 /* Draw Line and it should clear                */
 (*gfx_func_ptr->pv_sfbp_drawline)(scia_ptr,data_ptr,&line,&color,&visual,&z);
 mode  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);

 if (MODE->sfbp$r_control_bits.sfbp$v_con_berror_flag==1)
  {
  param[0] = sfbp$k_a_sfbp_reg_space | sfbp$k_mode;
  param[1] = 1;
  param[2] = 0;
  s=0;
  if (data_ptr->sfbp$l_print)
        printf ("mode<21> did not clear\n");
  }

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);

     
return (s);
}



/*+
* ===========================================================================
* = reg$$read_mode_addr - Register Read addr Bit Test =
* ===========================================================================
*
* OVERVIEW:
*       Read the mode addr change bit 
* 
* FORM OF CALL:
*       reg$$read_mode_addr_test (argc,argv,ccv,scia_ptr,data_ptr,param)
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
*       Simply write bresh 3 and check mode bit to be set
*
--*/
int     reg$$read_mode_addr_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int i,s=1;
register char *src,*dst;
union    control_reg *MODE;
int      mode;


/* Write to line address register to set b3 bit in mode	reg	*/
(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_line_address,0,LW);

/* Wait for chip idle 	*/
(*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

/* Read the addr bit...it should be set         */ 
 mode  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);
 MODE  = (union control_reg *)&mode;

if (MODE->sfbp$r_control_bits.sfbp$v_con_addr_flag==0)
  {
  param[0] = sfbp$k_a_sfbp_reg_space | sfbp$k_mode;
  param[1] = 1;
  param[2] = 0;
  s=0;
  if (data_ptr->sfbp$l_print)
        printf ("Mode<22> did not set\n");
  }

 /* Frame Buffer Write Clears it         */
 /* Do it and check for clear		 */
 (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,0,0,LW);

 /* Wait for chip idle 	*/
(*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
 mode  = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_mode);
 if (MODE->sfbp$r_control_bits.sfbp$v_con_addr_flag==1)
  {
  param[0] = sfbp$k_a_sfbp_reg_space | sfbp$k_mode;
  param[1] = 0;
  param[2] = 1;
  s=0;
  if (data_ptr->sfbp$l_print)
        printf ("Mode<22> did not clear\n");
  }

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);

     
return (s);
}
