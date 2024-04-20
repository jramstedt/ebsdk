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
 *      Color Frame Buffer INIT object
 *
 *  ABSTRACT:
 *
 *      INIT object
 *
 *  AUTHORS:
 *
 *      James Peacock
 *
 *
 *  CREATION DATE:      02-11-91
 *
 --*/

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
#include "cp$src:tga_sfbp_ext.h"
#include "cp$src:tga_sfbp_crb_def.h"
#include "cp$src:tga_sfbp_ctb_def.h"



/*+
* ===========================================================================
* = test_stereo - Stereo Test =                                          
* ===========================================================================
*
* OVERVIEW:
*       This is used for stereo test
* 
* FORM OF CALL:
*       test_stereo (scia,data,argc, argv, slot, ccv)
*
* ARGUMENTS:
*       scia    - shared driver pointer
*       data    - driver data pointer
*       argc    - argument count >=1
*       argv    - argv [0]="init", argv[1]=module number if argc >1.
*       slot    - the slot number
*       ccv     - callback vector array 
*
* FUNCTIONAL DESCRIPTION:
*       This is used for stereo test. 
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
 *
--*/

int test_stereo (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, 
SFBP *data_ptr,int param[])
{
volatile  int       vvalue;
volatile  union     video_vtiming_reg *v;
char      temp[20];
register  int i;

vvalue = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_vertical);
v      = (union video_vtiming_reg *)&vvalue;

v->sfbp$r_video_v_timing_bits.sfbp$v_stereo    = 1;

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_vertical,v->sfbp$l_whole,LW);

if (data_ptr->sfbp$l_verbose)
 {
 printf ("Stereo Continue ?\n");
 getchar ();
 }else msdelay (1000);
 
v->sfbp$r_video_v_timing_bits.sfbp$v_stereo    = 0;

(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_vertical,v->sfbp$l_whole,LW);

return(1);
}

