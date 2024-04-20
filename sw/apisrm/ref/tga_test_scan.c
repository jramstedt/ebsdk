#include   "cp$src:platform.h"
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
 *      Scanline write routine 
 *
 *  AUTHORS:
 *
 *      James Peacock
 *
 --*/

/**
** INCLUDES
**/

#include "cp$src:tga_sfbp_include.h"
#include   "cp$src:platform.h"
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
* = sfbp$$scan_test - SCAN Utility =
* ===========================================================================
*
* OVERVIEW:
*       SCAN Utility
* 
* FORM OF CALL:
*       sfbp$$scan_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       scia_ptr    - shared driver pointer
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*       Fill 1 scanline from top to bottom. This is so I can test the front porch and
*       back porch stuff.
*
--*/
int     test_scan (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
register int    i,k,PixelCount,data,done,ScanCount;
register int    GoFlag=0,inchar,ScanOffset,LineOffset,RefreshBase;

ScanCount   = data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines;
PixelCount  = data_ptr->sfbp$l_bytes_per_scanline;
ScanOffset  = argc >=3  ? (strtol(argv[2],0,0)*PixelCount) : 0;
data        = argc ==4  ?  strtol(argv[3],0,0) : 0xffffffff;

RefreshBase = ((*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_refresh_addr)<<12);                                             
ScanOffset += RefreshBase;

for (k=done=0;!done;k++)
 {
 k = ScanOffset/PixelCount;
 if (data_ptr->sfbp$l_alt_console && !GoFlag)
        printf ("Start Scanline %d Offset %d (0x%x)\n",k,ScanOffset,ScanOffset);
 LineOffset = ScanOffset;
 for (i=0;i<PixelCount/4;i++,LineOffset+=4)
        (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,LineOffset,data,LW);
        
 if (!GoFlag)
  {
  if ((inchar=getchar())=='e' || inchar =='q')done=1;
  if (inchar =='c' || inchar =='g')GoFlag=1;
  }
  
 LineOffset = ScanOffset;
 ScanOffset  += data_ptr->sfbp$l_bytes_per_scanline;
 data   = ~data ;
 
 if (k>data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines)done=1;

 }
 
return (1);
}

