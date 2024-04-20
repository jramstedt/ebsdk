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
 *      Color Frame Buffer CNFG ROM object
 *
 *  ABSTRACT:
 *
 *      Print configuration information.
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

extern   char rcsid[];
extern   char baselevel[];



/*+
* ===========================================================================
* = test_cnfg - sfbp Configuration ROM Object =
* ===========================================================================
*
* OVERVIEW:
*       This module is used to report information about the sfbp
*       color frame buffer.
* 
* FORM OF CALL:
*       cnfg (scia_ptr,data_ptr,argc,argv,slot,vector)
*
*
* ARGUMENTS:
*       scia_ptr - SCIA Pointer
*       data_ptr - Driver data area pointer
*       argc     - argument count which can be 1 or 2
*       argv     - argv[0]="cnfg",argv[1]=module number if argc =2.
*       slot     - this is the current slot the sfbp is plugged into.
*       vector   - this is the callback vector.
*
* FUNCTIONAL DESCRIPTION:
*       This routine is used to report configuration information about
*       the sfb+ module. Configuration information is reported with the
*       console "cnfg" command.  When the argument count is 2, then the
*       device can report more extended information. Extended information
*       includes specifics about the monitor. The firmware is included
*       in the extended configuration information.
*               In addition I report the asic chip revision information
*       on the BL line.
*
--*/
int test_cnfg (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])
{
int     i;
static  char    chip_rev[2]={0,0};

chip_rev[0] = 'A' + (data_ptr->sfbp$l_etch_revision-1);

#if MEDULLA
if ( data_ptr->sfbp$l_module == hx_8_plane)           {printf("  (TGA8M  %s-%s %d Mhz)",
        &baselevel[0],&chip_rev[0],data_ptr->sfbp$r_mon_data.sfbp$l_sold_freq);}
else if ( data_ptr->sfbp$l_module == hx_32_plane_no_z){printf("  (TGA24M %s-%s %d Mhz)",
        &baselevel[0],&chip_rev[0],data_ptr->sfbp$r_mon_data.sfbp$l_sold_freq);}
else if ( data_ptr->sfbp$l_module == hx_32_plane_z)   {printf("  (TGA24M with Z buffer %s-%s %d Mhz)",
        &baselevel[0],&chip_rev[0],data_ptr->sfbp$r_mon_data.sfbp$l_sold_freq);}
#else                                                                      
if ( data_ptr->sfbp$l_module == hx_8_plane)           {printf("  (ZLXp-E1  %s-%s %d Mhz)",
        &baselevel[0],&chip_rev[0],data_ptr->sfbp$r_mon_data.sfbp$l_sold_freq);}
else if ( data_ptr->sfbp$l_module == hx_32_plane_no_z){printf("  (ZLXp-E2 %s-%s %d Mhz)",
        &baselevel[0],&chip_rev[0],data_ptr->sfbp$r_mon_data.sfbp$l_sold_freq);}
else if ( data_ptr->sfbp$l_module == hx_32_plane_z)   {printf("  (ZLXp-E3 %s-%s %d Mhz)",
        &baselevel[0],&chip_rev[0],data_ptr->sfbp$r_mon_data.sfbp$l_sold_freq);}
#endif

sfbp$$print_video_info (scia_ptr,data_ptr);

return(SS$K_SUCCESS);
}


/*+
 * ===================================================
 * = sfbp$$print_video_info - Print Video Configuration =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *    This routine will print video information
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$print_video_info (scia_ptr,data_ptr)
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver pointer
 *      data_ptr - driver data area pointer	 
 *
 * FUNCTIONAL DESCRIPTION:
 *      Do a print of all video information. This is for display purposes.
 *
 *
 *
--*/
int     sfbp$$print_video_info (SCIA *scia_ptr,SFBP *data_ptr)

{

 printf ("\n");
 printf("  ---------------------------\n");
 if (data_ptr->sfbp$l_module==hx_8_plane)
        printf("  2 Mbytes @8 Planes \n");
 else   printf("  %d Mbytes @24 Planes \n",data_ptr->sfbp$l_vram_size/(1024*1024));
 printf("  %d x %d @%d Hz \n",data_ptr->sfbp$r_mon_data.sfbp$l_d_pixels,
                              data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines,
                              data_ptr->sfbp$r_mon_data.sfbp$l_refresh_rate);
 printf("  ---------------------------\n");


return (SS$K_SUCCESS);
}
