/******************************************************************************/
/**                                                                          **/
/**  Copyright (c) 1998                                                      **/
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
* Author: Nigel Croxon
*
*   Modified by:
*
*	X-1	Nigel Croxon				16-Jun-1998
**/

 
/*
 *  This is the internal max fragments supported
 */
#define MAX_FRAGMENTS 64

typedef struct _gct_mem_frag_defs {
  unsigned __int64	type;
  unsigned __int64	id;
  unsigned __int64	flags;
  unsigned __int64	pa;
  unsigned __int64	size;
} GCT_MEM_FRAG_DEFS;

/*
 *   DEBUG flags
 */
#define GCT__DEBUG_ALLOCATE_NODE		0x001
#define GCT__DEBUG_INIT				0x002
#define GCT__DEBUG_LOOKUP_NODE			0x004
#define GCT__DEBUG_CREATE_GALAXY		0x008
#define GCT__DEBUG_ASSIGN_HW_COMMUNITY		0x010
#define GCT__DEBUG_ASSIGN_HW_PARTITION		0x020
#define GCT__DEBUG_INSERT_NODE			0x040
#define GCT__DEBUG_CREATE_PARTITION_ID		0x080
#define GCT__DEBUG_CREATE_COMMUNITY_ID		0x100
#define GCT__DEBUG_BIND_NODE			0x200
#define GCT__DEBUG_INIT_FRAGS			0x400
#define GCT__DEBUG_DEASSIGN_HW			0x800
#define GCT__DEBUG_ASSIGN_MEM_FRAG		0x1000
#define GCT__DEBUG_DEASSIGN_MEM_FRAG		0x2000
#define GCT__DEBUG_GET_MAX_PARTITIONS		0x4000
#define GCT__DEBUG_VALIDATE_PARTITIONS		0x8000

#ifndef GCT__DEBUG_FLAGS
#define GCT__DEBUG_FLAGS 0
#endif


/*
 *   Extern data
 */
#ifndef GCT_POINTER_TO_ROOT
#define GCT_POINTER_TO_ROOT gct_root
extern GCT_NODE *gct_root;  
#endif

/*
 *  Node creation routines
 */

GCT_HANDLE gct__alloc_node(char nodeType, char nodeSubType, unsigned __int64 flags, int extra_size, void *type_specific);
GCT_HANDLE gct__create_mem_desc(char nodeType, char nodeSubType, unsigned __int64 flags, int extra_size, void *type_specific);
GCT_HANDLE gct__create_mem_sub(char nodeType, char nodeSubType, unsigned __int64 flags, int extra_size, void *type_specific);
GCT_HANDLE gct__create_iop(char nodeType, char nodeSubType, unsigned __int64 flags, int extra_size, void *type_specific);


/*
 *  Node deletion routines
 */

void gct__delete_node(GCT_HANDLE node);
void gct__cpu_init(GCT_HANDLE node, GCT_HANDLE community, GCT_HANDLE partiton, void *type_specific);


/*
 *  Node initialization routines
 */

int      gct__partition_init(GCT_HANDLE node, GCT_HANDLE community, GCT_HANDLE partition, void *type_specific);
int      gct__mem_frag_init(GCT_HANDLE node, GCT_HANDLE community, GCT_HANDLE partition, void *type_specific);
int      gct__cpu_init(GCT_HANDLE node, GCT_HANDLE community, GCT_HANDLE partition, void *type_specific);


extern void       gct__insert_node(GCT_HANDLE current, GCT_HANDLE newNode, int ins_type);

typedef struct _gct_node_info {
  int		size;
  GCT_HANDLE	(*create_node)(char nodeType, char nodeSubType, __int64 flags, int extra_size, void *type_specific);
  void		(*delete_node)(GCT_HANDLE handle);
  int		(*galaxy_init)(GCT_HANDLE node, GCT_HANDLE community, GCT_HANDLE partition, void *type_specific);
} GCT_NODE_INFO;

/*
 *  This array is used to create, init, and delete nodes, it is indexed by
 *  the node type.  The first entry is the base size of the node.  The next
 *  is the routine used to create the node.  Then the routine to delete the
 *  node, and finally a Galaxy initialization routine.
 *
 *  The normal node creation is gct__alloc_node, but if there is special
 *  processing - like some initialization data from the HW definition, or
 *  the size needs to be dynamically set - then a private creation routine
 *  can be implemented (they will typically call gct__alloc_node to do the
 *  actual creation).  Deletion is the same.  If special things need to be
 *  done as part of the deletion, this is where to hook it.
 *
 *  The galaxy initialization routine is a type specific initialization that
 *  will be done when the galaxy software configuration is completed.
 *
 */
#if (!WILDFIRE)
static GCT_NODE_INFO gct_node_info[NODE_LAST+1] = {
  {0,                              0,               0,                 0},	/* N/A  */
  {sizeof(GCT_ROOT_NODE),          0,               0,                 0},	/* Galaxy Root  */
  {sizeof(GCT_HW_ROOT_NODE),       gct__alloc_node, gct__delete_node,  0},	/* Hardware Root  */
  {sizeof(GCT_SW_ROOT_NODE),       gct__alloc_node, gct__delete_node,  0},	/* Software Root  */
  {sizeof(GCT_TEMPLATE_ROOT_NODE), gct__alloc_node, gct__delete_node,  0},	/* Temlplate Root  */
  {sizeof(GCT_COMMUNITY_NODE),     gct__alloc_node, gct__delete_node,  0},	/* Community  */
  {sizeof(GCT_PARTITION_NODE),     gct__alloc_node, gct__delete_node,  gct__partition_init}, /* Partition  */
  {sizeof(GCT_SBB_NODE),           gct__alloc_node, gct__delete_node,  0},	/* System Building Block  */
  {sizeof(GCT_PSEUDO_NODE),        gct__alloc_node, gct__delete_node,  0},	/* Pseudo  */
  {sizeof(GCT_CPU_NODE),           gct__alloc_node, gct__delete_node,  gct__cpu_init}, /* CPU  */
  {sizeof(GCT_MEMORY_SUB_NODE),    gct__create_mem_sub, gct__delete_node, 0},	/* Memory Subsystem  */
  {sizeof(GCT_MEM_DESC_NODE),      gct__create_mem_desc,gct__delete_node, gct__mem_frag_init}, /* Memory Description  */
  {sizeof(GCT_MEMORY_CTRL_NODE),   gct__alloc_node, gct__delete_node,  0},	/* Memory Control  */
  {sizeof(GCT_IOP_NODE),           gct__create_iop, gct__delete_node,  0},	/* IO Processor  */
  {sizeof(GCT_HOSE_NODE),          gct__alloc_node, gct__delete_node,  0},	/* Hose  */
  {sizeof(GCT_BUS_NODE),           gct__alloc_node, gct__delete_node,  0},	/* Bus  */
  {sizeof(GCT_IO_CTRL_NODE),       gct__alloc_node, gct__delete_node,  0},	/* IO Controller  */
  {sizeof(GCT_SLOT_NODE),          gct__alloc_node, gct__delete_node,  0},	/* Slot  */
  {sizeof(GCT_CPU_MODULE_NODE),    gct__alloc_node, gct__delete_node,  0},	/* CPU Module Card  */
  {sizeof(GCT_POWER_ENVIR_NODE),   gct__alloc_node, gct__delete_node,  0},	/* Power Environment  */
  {sizeof(GCT_FRU_ROOT_NODE),      gct__alloc_node, gct__delete_node,  0},	/* FRU Root  */
  {sizeof(GCT_FRU_DESC_NODE),      gct__alloc_node, gct__delete_node,  0},	/* FRU DESC  */
  {sizeof(GCT_SMB_NODE),           gct__alloc_node, gct__delete_node,  0},	/* SMB  */
  {sizeof(GCT_CAB_NODE),           gct__alloc_node, gct__delete_node,  0},	/* CAB  */
  {sizeof(GCT_CHASSIS_NODE),       gct__alloc_node, gct__delete_node,  0},	/* Sys Chassis */
  {sizeof(GCT_EXP_CHASSIS_NODE),   gct__alloc_node, gct__delete_node,  0},	/* Expand Chassis */
  {sizeof(GCT_SYS_INTER_SWITCH_NODE),gct__alloc_node, gct__delete_node,  0},	/* Sys InterConn Switch */
  {sizeof(GCT_HARD_PARTITION_NODE),gct__alloc_node, gct__delete_node,  0},	/* Hard Partition */
  {0,                              gct__alloc_node, gct__delete_node,  0}	/* NODE_LAST  */
};
#else
static GCT_NODE_INFO gct_node_info[NODE_LAST+1] = {
  {0,                              0,               0,                 0},	/* N/A  */
  {sizeof(GCT_ROOT_NODE),          0,               0,                 0},	/* Galaxy Root  */
  {sizeof(GCT_HW_ROOT_NODE),       gct__alloc_node, gct__delete_node,  0},	/* Hardware Root  */
  {sizeof(GCT_SW_ROOT_NODE),       gct__alloc_node, gct__delete_node,  0},	/* Software Root  */
  {sizeof(GCT_TEMPLATE_ROOT_NODE), gct__alloc_node, gct__delete_node,  0},	/* Temlplate Root  */
  {sizeof(GCT_COMMUNITY_NODE),     gct__alloc_node, gct__delete_node,  0},	/* Community  */
  {sizeof(GCT_PARTITION_NODE),     gct__alloc_node, gct__delete_node,  gct__partition_init}, /* Partition  */
  {sizeof(GCT_SBB_NODE),           gct__alloc_node, gct__delete_node,  0},	/* System Building Block  */
  {sizeof(GCT_PSEUDO_NODE),        gct__alloc_node, gct__delete_node,  0},	/* Pseudo  */
  {sizeof(GCT_CPU_NODE),           gct__alloc_node, gct__delete_node,  gct__cpu_init}, /* CPU  */
  {sizeof(GCT_MEMORY_SUB_NODE),    gct__create_mem_sub, gct__delete_node, 0},	/* Memory Subsystem  */
  {sizeof(GCT_MEM_DESC_NODE),      gct__create_mem_desc,gct__delete_node, gct__mem_frag_init}, /* Memory Description  */
  {sizeof(GCT_WF_MEMORY_CTRL_NODE),gct__alloc_node, gct__delete_node,  0},	/* Memory Control  */
  {sizeof(GCT_WF_IOP_NODE),        gct__create_iop, gct__delete_node,  0},	/* IO Processor  */
  {sizeof(GCT_WF_HOSE_NODE),       gct__alloc_node, gct__delete_node,  0},	/* Hose  */
  {sizeof(GCT_WF_BUS_NODE),        gct__alloc_node, gct__delete_node,  0},	/* Bus  */
  {sizeof(GCT_WF_IO_CTRL_NODE),    gct__alloc_node, gct__delete_node,  0},	/* IO Controller  */
  {sizeof(GCT_SLOT_NODE),          gct__alloc_node, gct__delete_node,  0},	/* Slot  */
  {sizeof(GCT_CPU_MODULE_NODE),    gct__alloc_node, gct__delete_node,  0},	/* CPU Module Card  */
  {sizeof(GCT_POWER_ENVIR_NODE),   gct__alloc_node, gct__delete_node,  0},	/* Power Environment  */
  {sizeof(GCT_FRU_ROOT_NODE),      gct__alloc_node, gct__delete_node,  0},	/* FRU Root  */
  {sizeof(GCT_FRU_DESC_NODE),      gct__alloc_node, gct__delete_node,  0},	/* FRU DESC  */
  {sizeof(GCT_SMB_NODE),           gct__alloc_node, gct__delete_node,  0},	/* SMB  */
  {sizeof(GCT_CAB_NODE),           gct__alloc_node, gct__delete_node,  0},	/* CAB  */
  {sizeof(GCT_CHASSIS_NODE),       gct__alloc_node, gct__delete_node,  0},	/* Sys Chassis */
  {sizeof(GCT_EXP_CHASSIS_NODE),   gct__alloc_node, gct__delete_node,  0},	/* Expand Chassis */
  {sizeof(GCT_WF_SYS_INTER_SWITCH_NODE),gct__alloc_node, gct__delete_node,  0},	/* Sys InterConn Switch */
  {sizeof(GCT_HARD_PARTITION_NODE),gct__alloc_node, gct__delete_node,  0},	/* Hard Partition */
  {0,                              gct__alloc_node, gct__delete_node,  0}	/* NODE_LAST  */
};
#endif

/*
 *   The node insertion can be sibling (next) or child
 */
#define GCT__INSERT_NEXT_SIBLING    1
#define GCT__INSERT_CHILD           2
#define GCT__INSERT_CHILD_LAST_SIB  3
#define GCT__INSERT_LAST_SIBLING    4

