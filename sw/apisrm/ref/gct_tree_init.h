/*** MODULE GCT_TREE_INIT ***/
/******************************************************************************/
/**                                                                          **/
/**  Copyright (c) 1997                                                      **/
/**  by DIGITAL Equipment Corporation, Maynard, Mass.                        **/
/**  All rights reserved.                                                    **/
/**                                                                          **/
/**  This software is furnished under a license and may be used and  copied  **/
/**  only  in  accordance  with  the  terms  of  such  license and with the  **/
/**  inclusion of the above copyright notice.  This software or  any  other  **/
/**  copies  thereof may not be provided or otherwise made available to any  **/
/**  other person.  No title to and ownership of  the  software  is  hereby  **/
/**  transferred.                                                            **/
/**                                                                          **/
/**  The information in this software is subject to change  without  notice  **/
/**  and  should  not  be  construed  as  a commitment by DIGITAL Equipment  **/
/**  Corporation.                                                            **/
/**                                                                          **/
/**  DIGITAL assumes no responsibility for the use or  reliability  of  its  **/
/**  software on equipment which is not supplied by DIGITAL.                 **/
/**                                                                          **/
/******************************************************************************/
/*
* Author: Fred Kleinsorge
*
*   Modified by:
*
* 
*	X-2	Andy Kuehnel				18-Dec-1997
*		From Fred: interface to gct_init$configure_galaxy
*		changed.
*
*	X-1	Andy Kuehnel				13-Nov-1997
*		Add standard header; initial checkin.
*/
#ifndef _GCT_TREE_INIT_H_
#define _GCT_TREE_INIT_H_

#define GIG		0x00040000000
#define ONE_GIG		0x00040000000
#define TWO_GIG		0x00080000000
#define FOUR_GIG	0x00100000000
#define EIGHT_GIG	0x00200000000
#define SIXTY4_GIG	0x01000000000
#define ONE28_GIG	0x02000000000
#define MEG		0x00000100000
#define ONE_MEG		0x00000100000
#define TWO_MEG		0x00000200000
#define FOUR_MEG	0x00000400000
#define EIGHT_MEG	0x00000800000
#define SIXTY4_MEG	0x00004000000
#define ONE28_MEG	0x00008000000
#define TWO56_MEG	0x00010000000

#define NO_INIT   0
#define INIT_ONLY 1
#define INIT      2


typedef struct _gct_platform_tree {
  __int64	depth;
  int		type;
  int		subtype;
  GCT_ID	id;
  unsigned __int64 flags;  
  unsigned __int64 private[8];
} GCT_PLATFORM_TREE;

/*
 *  Pseudo-NVRAM
 */
typedef struct _gct_galaxy_nvram {
  int		type;
  int		subtype;
  GCT_ID	id;
  unsigned __int64 private_init;
  unsigned __int64 private[8];
} GCT_GALAXY_NVRAM;

/*
 *  Built-in configurations
 */
typedef struct _gct_configurations {
  int               platform_type;
  GCT_PLATFORM_TREE *platform_tree;
  GCT_GALAXY_NVRAM  *galaxy_nvram;
  char		    *galaxy_id;
} GCT_CONFIGURATIONS;

typedef struct _gct_platform_data {
  unsigned __int64 cons_req;
  unsigned __int64 base_alloc;
  unsigned __int64 base_align;
  unsigned __int64 min_alloc;
  unsigned __int64 min_align;
  unsigned __int32 max_partitions;
  unsigned __int32 max_fragments;
  unsigned __int32 max_desc;
  char		*name;
  unsigned __int64 platform_type;
} GCT_PLATFORM_DATA;

#endif
