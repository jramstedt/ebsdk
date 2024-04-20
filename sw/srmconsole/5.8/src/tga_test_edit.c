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
 *      Color Frame Buffer TEST,INIT and CNFG dispatcher
 *
 *  ABSTRACT:
 *
 *      This is used to dispatch to the test object,cnfg object and
 *      init object in the sfbpurbo option rom.
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
 *--*/

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


/*+===========================================================================
* = test_edit - Editor =
* +===========================================================================
*
* OVERVIEW:
*       Register Editor
*
* FORM OF CALL:
*       test_edit ()
*
* RETURNS:
*       1 for success, else 0 for failure
*
* ARGUMENTS:
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*       This is used for examining and reading locations.
*
* CALLS:
*       none
*
--*/

int test_edit (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr, int param [])

{
int        done,i,len,count;
char       *ptr,temp[80],temp2[80];
int        offset,bytemask,address,data,data2,w,r;

#if TGA_24 
 	offset =  0x800000;
#else
if (data_ptr->sfbp$l_module == hx_8_plane )
	offset =  0x200000 ;
else if ( data_ptr->sfbp$l_module == hx_32_plane_no_z)
	offset =  0x800000 ;
else if ( data_ptr->sfbp$l_module == hx_32_plane_z)
	offset = 0x1000000 ;
#endif


for (done=FALSE;done==FALSE;)
 {
 ptr           = temp;
 count         = 1;
 bytemask      = 0xf;
 
 for (;done==FALSE;)
  {
  for (i=0;i<sizeof(temp);i++)temp[i]=0;
  printf ("ZLXpEdit>> ");
  gets (temp);
  if ((len=strlen(temp))>0)break;
  }

 if (strncmp(temp,"quit",1)==0)done=TRUE;
 else if (temp[0]=='e')
  {
  for (i=0;i<len&&*ptr++!=' ';i++);
  address = strtol (ptr,0,16);
  for (;i<len&&*ptr++!=' ';i++);
  count   = strtol (ptr,0,16);
  for (;i<len&&*ptr++!=' ';i++);
  
  if (count<1)count=1;

  for (i=0,w=address;i<count;i++,w+=4)
    {
    if (w>=0 && w<0x100000)
        data = (*gfx_func_ptr->pv_sfbp_read_rom)(scia_ptr,w);
    else if (w>=0x100000 && w<offset)
        data = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,w-0x100000);
    else data = (*gfx_func_ptr->pv_sfbp_read_vram)(scia_ptr,w-offset);
    printf ("0x%x= %x\n",w,data);
    }
  }

 else if (temp[0]=='d')
  {
  for (i=0;i<len&&*ptr++!=' ';i++);
  address = strtol (ptr,0,16);
  for (;i<len&&*ptr++!=' ';i++);
  data    = strtol (ptr,0,16);
  for (;i<len&&*ptr++!=' ';i++);
  count   = strtol (ptr,0,16);
  for (;i<len&&*ptr++!=' ';i++);
  bytemask= 0xf;

  if (count<1)count=1;
  for (i=0,w=address;i<count;i++,w+=4)
        {
        if (w>=0x100000 && w<offset)
                (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,w-0x100000,data,LW);
        else (*gfx_func_ptr->pv_sfbp_write_vram)(scia_ptr,w-offset,data,bytemask);
        }
  }
 else
   {
   printf ("e    address [count] \n");
   printf ("d    address data [count] \n");
   }
 }
return (1);
}
