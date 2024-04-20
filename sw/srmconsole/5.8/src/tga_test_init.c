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
#include "cp$src:tga_sfbp_463.h"
#include "cp$src:tga_sfbp_485.h"
#include "cp$src:tga_sfbp_ext.h"
#include "cp$src:tga_sfbp_crb_def.h"
#include "cp$src:tga_sfbp_ctb_def.h"



/*+
* ===========================================================================
* = test_init - sfbp Console Port driver initialization =
* ===========================================================================
*
* OVERVIEW:
*       This is used for module initialization.
* 
* FORM OF CALL:
*       init(scia,data,argc, argv, slot, ccv)
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
*       This is used for module initialization. 
--*/

int test_init (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])
{
register int  help,Erase,i,j,done=0;
register char *pptr;
    
 /* Look for 8 bit visual       */
 for (j=data_ptr->sfbp$l_8bit_visual=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
        if (*pptr++=='-' && *pptr++== '8' ){data_ptr->sfbp$l_8bit_visual=1;break;}

 /* Erase the screen if 'erase' argument after init  */
 /* This is so we do not erase screen always         */
 /* t 1/init e                                       */
 
 for (Erase=j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
        if (*pptr++=='e'){Erase=1;break;}

 /* Look for Frequency Override Switch               */
 /* init -sF                                         */
 /* Value Must be From Table                         */
 /* Init Code Will Flag Error if Wrong               */

 for (data_ptr->sfbp$l_override=j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
    if (*pptr++=='-' && *pptr++=='s')
        data_ptr->sfbp$l_override=strtol (pptr,0,16);

 /* Look for Help                                    */
 /* t tc1 init ?                                     */
 for (j=help=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
    if (*pptr++=='?' ){help=1;break;}
    
 if (help || (data_ptr->sfbp$l_override<0 || data_ptr->sfbp$l_override>14) )
          {
          printf ("Allowed Switch Settings:\n");
          printf ("0= 130\n1= 119\n2= 108\n3= 104\n4= 92\n5= 75\n6= 74\n7= 69\n8= 65\n9= 50\nA= 40\nB= 32\nC= 25\n");
          }                
 
 if (Erase)(*cp_func_ptr->pv_reset_output)(scia_ptr,data_ptr);
 else (*cp_func_ptr->pv_init_output)(scia_ptr,data_ptr);


return(1);
}

