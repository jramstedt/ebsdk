/*** MODULE GCT_ROUTINES ***/
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
*	X-3	Andy Kuehnel				18-Dec-1997
*		From Fred: Add gct$update_galaxy_id.
*
*	X-2	Andy Kuehnel				13-Nov-1997
*		Add standard header.
*/
#ifndef _GCT_ROUTINES_H_
#define _GCT_ROUTINES_H_

extern int  gct$read_lock( GCT_ID partition_id, unsigned __int64 retry);
extern int  gct$read_unlock( GCT_ID partition_id);
extern int  gct$make_hw_active( GCT_HANDLE sw_node, GCT_HANDLE hw_node);
extern int  gct$make_hw_inactive( GCT_HANDLE sw_node, GCT_HANDLE hw_node);
extern int  gct$get_update_level( unsigned __int64 *updateLevel);
extern int  gct$assign_hw( GCT_HANDLE sw_node, GCT_HANDLE hw_node, unsigned int active, unsigned __int64 incarnation);
extern int  gct$deassign_hw( GCT_HANDLE sw_entity, GCT_HANDLE hw_node, unsigned int active);
extern int  gct$create_partition( GCT_HANDLE *partition, GCT_ID *partition_id, GCT_HANDLE community, int os_type);
extern int  gct$create_community( GCT_HANDLE *community, GCT_ID *community_id);
extern int  gct$delete_partition( GCT_HANDLE partition, unsigned int active);
extern int  gct$delete_community( GCT_HANDLE community, unsigned int active) ;
extern int  gct$assign_mem_fragment( GCT_HANDLE sw_owner, unsigned int flags, unsigned __int64 PA, unsigned __int64 size, unsigned int active);
extern int  gct$deassign_mem_fragment( GCT_HANDLE sw_owner, unsigned __int64 PA, unsigned int active);
extern int  gct$find_node( GCT_HANDLE root, GCT_HANDLE *current, int search_criteria, char search_type, char search_subType, GCT_ID search_id, GCT_HANDLE search_owner, int *depth);
extern int  gct$find_partition( unsigned __int64 HWRPB, GCT_HANDLE *partition, GCT_ID *partition_id, GCT_HANDLE *community, GCT_ID *community_id);
extern int  gct$get_text(unsigned __int64 what, int names, char *prefix, char *retname, int maxlen);
extern int  gct$validate_partition(GCT_HANDLE partition);
extern int  gct$update_gmdb( GCT_HANDLE community, int offset, unsigned __int64 new_data, unsigned __int64 old_data);
extern int  gct$get_max_partition();
extern int  gct$update_galaxy_id( char *new_data, char *old_data);

#endif
