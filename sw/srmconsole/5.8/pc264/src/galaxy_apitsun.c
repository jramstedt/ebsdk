/*
 *
 * file:	galaxy_pc264.c
 *
 * Copyright (C) 1998 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * Compaq Computer Corporation, Houston, Texas.
 * All rights reserved.
 *                                                                                               
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */
/*
 *++
 *  FACILITY:
 *                                                
 *      Alpha SRM Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	PC264 Specific Galaxy Configuration Tree
 *
 *  AUTHOR:
 *
 *      Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *      21-Dec-1998
 *
 *  MODIFICATION HISTORY:
 *	
 *	er	21-Dec-1998    Initial port from GALAXY_CLIPPER.C
 *
 *
 *--
 */
#include "cp$src:platform.h"
#include "cp$src:ctype.h"
#include "cp$src:parse_def.h"
#include "cp$src:msg_def.h"
#include "cp$inc:kernel_entry.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:console_hal_macros.h"
#include "cp$src:ev_def.h"
#include "cp$src:impure_def.h"
#include "cp$src:pal_def.h"
#include "cp$src:hwrpb_def.h"
#include "cp$src:version.h"
#include "cp$src:pb_def.h"
#include "cp$src:probe_io_def.h"
#include "cp$src:platform_fru.h"
#include "cp$src:gct.h"
#include "cp$src:gct_nodeid.h"
#include "cp$src:gct_tree.h"
#include "cp$src:gct_tree_init.h"
#include "cp$src:galaxy_def_configs.h"
#include "cp$src:irongate.h"
#include "cp$src:platform_cpu.h"
#include "cp$src:jedec_def.h"
#include "cp$inc:platform_io.h"

#include	"cp$src:nt_types.h"

#define DEBUG	0
#define JedecSerialNum(pJedec, b)		strcpy(b,"Unknown");

extern void get_srom_revision(char *srev, int id);

extern uint64 get_array_size(int array);
extern uint64 get_array_base(int array);

extern int cpu_mask;
extern unsigned __int64 mem_size;
extern int in_console;
extern int primary_cpu;
extern struct HWRPB *hwrpb;
extern gct_root;
char fw_rev[10] = { 0, 0 };
#define DIMMS_PER_ARRAY 1


unsigned __int64 pci_cfg_base(int hose, int bus, int slot, int function)
{
    UINT p = PCI_CONFIG_BASE;
    UINT q = 0;

    p += hose * 0x200; /* 8 GB per hose */
    q = (bus << 16) + (slot << 11) + (function << 8);
    p = (p << 24) | q;
    return(p);
}

/*+
 * ============================================================================
 * = gct_init$pc264_hw - Initialize the Galaxy Configuration Tree structure   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Initialize the root and child/sibling nodes of the hardware, software, 
 *	and FRU branches of the configuration tree data structure.
 *
 * FORM OF CALL:
 *
 *	gct_init$pc264_hw(); 
 *
 * RETURNS:
 *
 *      Success
 *                          
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int gct_init$pc264_hw() 
{
    GCT_HANDLE hw_root = 0, fru_root = 0;
    int i, j, k, status;
    unsigned int num_dimms = 0;
    struct pb *pb;

/* 
 ** Get the firmware revision.  This information is needed for TLV 
 ** (tag, length, value) format subpacket fields when constructing 
 ** nodes on the FRU branch of the configuration tree.
 */
    sprintf(fw_rev, "%c%d.%d-%d"OEM_VERSION_SUB, v_variant, v_major, v_minor, v_sequence);
/*
 ** Build the hardware resource branch of the configuration tree.  Start by
 ** getting a handle to the hardware root node.
 */
    hw_root = gct__lookup_node(0, 0, FIND_BY_TYPE, NODE_HW_ROOT, 0, 0, 0, 0);
#if DEBUG
    printf("Building hardware branch of configuration tree starting at root %x\n", hw_root);
#endif
/* 
 ** Build a hardware node for the system mother board.
 */
    if ((status = build_smb_hw(hw_root)) != SUCCESS)
	return(status);

/* 
 ** Build hardware nodes for each CPU in the system 
 */
    for (i = 0; i < MAX_PROCESSOR_ID; i++) {
	if (cpu_mask & (1 << i)) {
	    if ((status = build_cpu_hw(hw_root, i)) != SUCCESS) {
		return(status);
	    }
	}
    }


/* 
 ** Build a hardware node for each I/O processor in the system
 */
    for (i = 0;  i < 1;  i++) {
	if ((status = build_iop_hw(hw_root, i)) != SUCCESS)
	    return(status);
    }

/* 
 ** Build a hardware node for the Memory Subsystem
 */
    if ((status = build_memory_sub_hw(hw_root)) != SUCCESS)
	return(status);
/*
 ** Build a hardware node for the Power and Environmental montoring
 **
 ** The Member ID field from the system variation offset in the HWRPB 
 ** is used to determine the system type.  If Member ID is a '1' 
 ** (AlphaPC 264DP), do not build a HW node for Power/Environmental 
 ** monitoring.
 */
    if ((hwrpb->SYSVAR[0] >> 10) != 1) {
	if ((status = build_power_hw(hw_root)) != SUCCESS)
	    return(status);
    }
/* 
 ** Next, build the FRU branch of the configuration tree
 */
    if ((status = build_fru_root(hw_root)) != SUCCESS)
	return(status);
/*
 ** Get a handle to the FRU root node.
 */
    fru_root = gct__lookup_node(0, 0, FIND_BY_TYPE, NODE_FRU_ROOT, 0, 0, 0, 0);
/* 
 ** Build an FRU node for the system mother board.
 */
    if ((status = build_smb_fru(hw_root, fru_root)) != SUCCESS)
	return(status);
/* 
 ** Build an FRU node for the Power/Environmental monitoring.  
 **
 ** The Member ID field from the system variation offset in the HWRPB 
 ** is used to determine the system type.  If Member ID is a '1'
 ** (AlphaPC 264DP), do not build a FRU node for Power/Environmental 
 ** monitoring.
 */
    if ((hwrpb->SYSVAR[0] >> 10) != 1) {
	if ((status = build_power_fru(hw_root, fru_root)) != SUCCESS) {
	    return(status);
	}                                                                                 
    }

    return(SUCCESS);
}


int gct_init$pc264_sw() 
{
    GCT_HANDLE sw_root = 0;

    sw_root = gct__lookup_node(0, 0, FIND_BY_TYPE, NODE_SW_ROOT, 0, 0, 0, 0);
    build_sw_root(sw_root);
    return(SUCCESS);
}


/*+
 * ============================================================================
 * = build_smb_hw - Construct a node for the System Motherboard hardware      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_smb_hw(hw_root); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *      hw_root - a handle to the root node of the hardware branch
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_smb_hw(GCT_HANDLE hw_root) 
{
    GCT_HANDLE hwnode = 0, node = 0;
    GCT_HW_ROOT_NODE *pHW_root_node;
    GCT_SMB_NODE *pSmb_node;
    GCT_NODE_ID_HW_ROOT *pHw_root_id;
    GCT_NODE_ID_SMB *pHw_smb_id;

/* 
 ** Backfill some data for hardware root node 
 */
    pHW_root_node = (GCT_HW_ROOT_NODE *)_GCT_MAKE_ADDRESS(hw_root);
    pHW_root_node->hd_hw_root.depth = 1;
    pHW_root_node->hd_hw_root.node_flags |= NODE_HARDWARE;
    pHW_root_node->hd_hw_root.config = hw_root;
    pHW_root_node->hd_hw_root.hd_extension = offsetof(struct _gct_hw_root_node, ext_hroot);
    pHW_root_node->ext_hroot.fru_count = 1;

    pHw_root_id = &pHW_root_node->hd_hw_root.id;
    pHw_root_id->node_id_hw_root_rsrvd_08_15 = 0xFF;
    pHw_root_id->node_id_hw_root_rsrvd_16_31 = 0xFFFF;
    pHw_root_id->node_id_hw_root_rsrvd_32_63 = 0xFFFFFFFF; 
/* 
 ** Create a handle for the System Motherboard and insert it into the hardware
 ** branch of the configuration tree as the last sibling of the hardware root's
 ** children.
 */
    node = (gct_node_info[NODE_SMB].create_node)(NODE_SMB, 0, 0, 0, 0);
    gct__insert_node(hw_root, node, GCT__INSERT_CHILD_LAST_SIB);
#if DEBUG
    printf("Creating SMB HW child %x for parent %x\n", node, hw_root);
#endif
/* 
 ** Make a pointer to the new SMB node from the handle and initialize it.
 */
    pSmb_node = (GCT_SMB_NODE *)_GCT_MAKE_ADDRESS(node);
    pSmb_node->hd_smb.hd_extension = offsetof(struct _gct_smb_node, ext_smb);
    pSmb_node->hd_smb.depth = 2;
    pSmb_node->hd_smb.node_flags |= NODE_HARDWARE;
/*
 ** Initialize this node's ID field
 */
    pHw_smb_id = &pSmb_node->hd_smb.id;
    pHw_smb_id->node_id_smb_rsrvd_0_31 = 0xFFFFFFFF;
    pHw_smb_id->node_id_smb_hsbb = -1;
    pHw_smb_id->node_id_smb_rsrvd_56_63 = 0xFF;
/* 
 ** Initialize the System Mother Board (SMB) system resource configuration subpacket 
 */
    pSmb_node->smb_res.smb_sub.subpack_length = sizeof(SUBPKT_SMB_FRU5);
    pSmb_node->smb_res.smb_sub.subpack_class = CLASS_REGATTA; /* erfix */
    pSmb_node->smb_res.smb_sub.subpack_type = SUBPACK_SMB;
    pSmb_node->smb_res.smb_sub.subpack_rev = 1;

/* 
 ** Fill in the header extension information
 */
    pSmb_node->ext_smb.fru_count = 1;
    pSmb_node->ext_smb.subpkt_count = 1;
    pSmb_node->ext_smb.subpkt_offset = offsetof(struct _gct_smb_node, smb_res);

    return(SUCCESS);
}


/*+
 * ============================================================================
 * = build_cpu_hw - Construct a node for a CPU hardware resource              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_cpu_hw(hw_root, cpu_id); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *           
 *      hw_root - a handle to the root node of the hardware branch
 *	cpu_id  - a unique CPU identifier
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_cpu_hw(GCT_HANDLE hw_root, int cpu_id) 
{
    GCT_ROOT_NODE *pRoot;
    GCT_HANDLE hwnode = 0, node = 0;
    GCT_CPU_NODE *pCpu_node;
    GCT_NODE_ID_CPU *pNodeid_cpu;
    GCT_NODE_ID_SMB *pNodeid_smb;
    unsigned __int64 tmp = 0;
    GCT_HD_EXT *pHd_ext;
    GCT_TLV *pTLV;
    SUBPKT_CACHE_FRU5 *pCache;
    SUBPKT_SENSOR_FRU5 *pSensor;
    int status, orig_size, sptr, eptr;
    unsigned char *pValue;
    struct PALVERSION *pal_version;
    struct PALVERSION *osfpal_version;
    struct SLOT *slot;
    struct FILE *fp;
    char buffer[sizeof(JEDEC)];
    JEDEC *pJedec;
    char device[32], b[10];    

/* 
 ** Get a pointer to the root of the configuration tree
 */
    pRoot = (GCT_ROOT_NODE *)_GCT_MAKE_ADDRESS(0);
/* 
 ** Get a pointer to the SMB node
 */
    pNodeid_smb = &tmp;
    pNodeid_smb->node_id_smb_rsrvd_0_31 = 0xFFFFFFFF;
    pNodeid_smb->node_id_smb_hsbb = -1;
    pNodeid_smb->node_id_smb_rsrvd_56_63 = 0xFF;
    if ((status = gct$find_node(0, &hwnode, (FIND_BY_TYPE | FIND_BY_ID), NODE_SMB,  0, tmp, 0, 0)) != SUCCESS)
	return(status);
/* 
 ** Create a handle for this CPU node and insert it into the hardware branch of the
 ** configuration tree as the last sibling of the SMB hardware's children.
 */
    node = (gct_node_info[NODE_CPU].create_node)(NODE_CPU, 0, 0, 0, 0);
    gct__insert_node(hwnode, node, GCT__INSERT_CHILD_LAST_SIB);
#if DEBUG
    printf("Creating CPU %d HW child %x for parent %x\n", cpu_id, node, hwnode);
#endif
/* 
 ** Make a pointer to the new CPU node from the handle and initialize it.
 */
    pCpu_node = (GCT_CPU_NODE *)_GCT_MAKE_ADDRESS(node);
#if DEBUG
    printf("CPU %d node at %x, handle %x\n", cpu_id, pCpu_node, node);
#endif
    pCpu_node->hd_cpu.depth = 3;
    pCpu_node->hd_cpu.node_flags |= NODE_HARDWARE;

    if (cpu_id != primary_cpu)
	pCpu_node->hd_cpu.subtype = SNODE_CPU_NOPRIMARY;
/*
 ** Initialize this node's ID field
 */
    pNodeid_cpu = &pCpu_node->hd_cpu.id;
    pNodeid_cpu->node_id_cpu_cpu = cpu_id;
    pNodeid_cpu->node_id_cpu_rsrvd_16_23 = 0xFF;
    pNodeid_cpu->node_id_cpu_hsbb = -1;
    pNodeid_cpu->node_id_cpu_rsrvd_56_63 = 0xFF;
/* 
 ** Save original size before adding any subpackets.
 */
    orig_size = pCpu_node->hd_cpu.size;
    pRoot->available += pCpu_node->hd_cpu.size;
    pRoot->first_free -= pCpu_node->hd_cpu.size;
/*
 ** Initialize the Processor resource configuration subpacket 
 */
    pCpu_node->cpu_res.cpu_sub.subpack_length = sizeof(SUBPKT_CPU_FRU5) - 8;
    pCpu_node->cpu_res.cpu_sub.subpack_class = CLASS1;
    pCpu_node->cpu_res.cpu_sub.subpack_type = SUBPACK_PROCESSOR;
    pCpu_node->cpu_res.cpu_sub.subpack_rev = 5;
/* 
 ** Get processor and PALcode revision information from the per-CPU slot of
 ** the HWRPB data structure.
 */
    slot = (void *)((int)hwrpb+ *(UINT *)hwrpb->SLOT_OFFSET +
		(cpu_id * sizeof(struct SLOT)));
    pCpu_node->cpu_res.processor_id = cpu_id;
    pCpu_node->cpu_res.processor_family = 0x30;
    pCpu_node->cpu_res.cpu_state = *(__int64 *)&slot->STATE;
    pCpu_node->cpu_res.ovms_palcode_revision = *(__int64 *)&slot->PAL_REV_AVAIL[0][2];
    pCpu_node->cpu_res.dunix_palcode_revision = *(__int64 *)&slot->PAL_REV_AVAIL[0][4];
    pCpu_node->cpu_res.wnt_palcode_revision = 0;
    pCpu_node->cpu_res.alpha_type = *(__int64 *)&slot->CPU_TYPE;
    pCpu_node->cpu_res.processor_variation = *(__int64 *)&slot->CPU_VAR;
/*
 ** Fill in the processor manufacturer TLV field
 */
    pTLV = &pCpu_node->cpu_res.processor_manufacturer.tlv_tag;

#ifdef FIX_LATER
/*
 ** Open the EEPROM on the CPU daughter card and read 256
 ** bytes of JEDEC-like data.
 */
    sprintf(device, "iic_cpu%d", cpu_id);
    if ((fp = fopen(device, "sr")) == 0) {
	err_printf(msg_bad_open, device);
	return(0);
    }
    else {
	fread(buffer, sizeof(JEDEC), 1, fp);
	pJedec = &buffer;
	fclose(fp);
    }
    pTLV->tlv_tag = TLV_TAG_ISOLATIN1;
    pTLV->tlv_length = 10;
    pValue = (unsigned char *)&pTLV->tlv_value;
    strncpy((char *)pValue, pJedec->jedec_b_manuf_spec_model, pTLV->tlv_length);
    pCpu_node->hd_cpu.size = (pCpu_node->hd_cpu.size + _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4);
    pCpu_node->cpu_res.cpu_sub.subpack_length += _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4;
    _NEXT_TLV(pTLV);
#endif

/*
 ** Fill in the processor serial number TLV field
 */
    pTLV->tlv_tag = TLV_TAG_ISOLATIN1;
    pTLV->tlv_length = 10;
    pValue = (unsigned char *)&pTLV->tlv_value;
    JedecSerialNum(pJedec, b);
    strncpy((char *)pValue, b, pTLV->tlv_length);
    pCpu_node->hd_cpu.size = (pCpu_node->hd_cpu.size + _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4);
    pCpu_node->cpu_res.cpu_sub.subpack_length += _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4;
    _NEXT_TLV(pTLV);
/*
 ** Fill in the processor revision level TLV field
 */
    pTLV->tlv_tag = TLV_TAG_ISOLATIN1;
    pTLV->tlv_length = strlen("00");
    pValue = (unsigned char *)&pTLV->tlv_value;
    strncpy((char *)pValue, "00", pTLV->tlv_length);
    pCpu_node->hd_cpu.size = (pCpu_node->hd_cpu.size + _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4);
    pCpu_node->cpu_res.cpu_sub.subpack_length += _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4;
    _NEXT_TLV(pTLV);
/*
 ** Initialize the Cache configuration subpacket 
 */
    pCache = (int)pTLV;
#if DEBUG
    printf("CPU %d cache subpacket at %x\n", cpu_id, pCache);
#endif
    pCache->cache_sub.subpack_length = sizeof(SUBPKT_CACHE_FRU5);
    pCache->cache_sub.subpack_class = CLASS1;
    pCache->cache_sub.subpack_type = SUBPACK_CACHE;
    pCache->cache_sub.subpack_rev = 3;

    pCache->cache_level = CACHE_LVL_SECONDARY;
    pCache->cache_size = pJedec->jedec_b_fru_spec[2] * 1024;
    pCache->cache_speed = 200;
    pCache->size_avail = pJedec->jedec_b_fru_spec[2] * 1024;
    pCache->cache_wp = CACHE_WP_LATEWRITE;
    pCache->cache_ec = CACHE_EC_SINGLEBITECC;
    pCache->cache_type = CACHE_TYPE_UNIFIED;
    pCache->cache_state = CACHE_STAT_ENABLED;
/* 
 ** Add the cache configuration subpacket size to the CPU node size
 */
    pCpu_node->hd_cpu.size += sizeof(SUBPKT_CACHE_FRU5);
/*
 ** Initialize a Sensor resource configuration subpacket for CPU Fan
 */
    pSensor = (int)pCache + sizeof(SUBPKT_CACHE_FRU5);
#if DEBUG
    printf("CPU %d sensor subpacket at %x\n", cpu_id, pSensor);
#endif

    pSensor->sensor_sub.subpack_length = sizeof(SUBPKT_SENSOR_FRU5);
    pSensor->sensor_sub.subpack_class = CLASS1;
    pSensor->sensor_sub.subpack_type = SUBPACK_SENSOR;
    pSensor->sensor_sub.subpack_rev = 2;
    pSensor->console_id = cpu_id;
    pSensor->sensor_class = SENSOR_CLASS_FAN;
    pSensor->sensor_properties = SENSOR_PROP_STATUS;
/* 
 ** Add the sensor resource subpacket size to the CPU node size
 */
    pCpu_node->hd_cpu.size += sizeof(SUBPKT_SENSOR_FRU5);
/* 
 ** Fill in the header extension information
 */
    pHd_ext = (int)pSensor + sizeof(SUBPKT_SENSOR_FRU5);
    pHd_ext->fru_count = 1;
    pHd_ext->subpkt_count = 3;
    pHd_ext->subpkt_offset = offsetof(struct _gct_cpu_node, cpu_res);
#if DEBUG
    printf("CPU %d header extension at %x\n", cpu_id, pHd_ext);
    printf("       ->fru_count     = %d\n", pHd_ext->fru_count);
    printf("       ->subpkt_count  = %d\n", pHd_ext->subpkt_count);
    printf("       ->subpkt_offset = %x\n", pHd_ext->subpkt_offset);
#endif
/* 
 ** Add Header Extension size to node size
 */
    pCpu_node->hd_cpu.size += sizeof(GCT_HD_EXT);
    eptr = &pHd_ext->fru_count;
    sptr = &pCpu_node->hd_cpu.type;
    pCpu_node->hd_cpu.hd_extension = eptr - sptr;
#if DEBUG
    printf("CPU %d node end at %x\n", cpu_id, eptr);
    printf("CPU %d node start at %x\n", cpu_id, sptr);
    printf("CPU %d header extension at %x\n", cpu_id, pCpu_node->hd_cpu.hd_extension);
#endif
/*
 ** Quadword align
 */
    pCpu_node->hd_cpu.size = ((pCpu_node->hd_cpu.size + 63) & ~63); 
#if DEBUG
    printf("CPU %d node size = %x\n", cpu_id, pCpu_node->hd_cpu.size);
#endif
/*    pCpu_node->hd_cpu.size -= gct_head; */
/* 
 ** Update fields in header so next CPU node has proper offset 
 */
    pRoot->available -= pCpu_node->hd_cpu.size;
    pRoot->first_free += pCpu_node->hd_cpu.size;
#if DEBUG
    printf("First free node at %x\n", pRoot->first_free);
    printf("%d bytes available in free pool\n\n", pRoot->available);
#endif

    return(SUCCESS);
}


/*+
 ** ============================================================================
 * = build_iop_hw - Construct a node for an I/O Processor hardware resource   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_iop_hw(hw_root, iop_id); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *      hw_root - a handle to the root node of the hardware branch
 *	iop_id  - a unique I/O Processor identifier
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_iop_hw(GCT_HANDLE hw_root, int iop_id) 
{
    int status;
    GCT_HANDLE hwnode = 0, node = 0;
    GCT_IOP_NODE *pIop_node;
    GCT_NODE_ID_IOP *pNodeid_iop;
    GCT_NODE_ID_SMB *pNodeid_smb;
    unsigned __int64 tmp = 0;
/* 
 ** Get a pointer to the SMB node
 */
    pNodeid_smb = &tmp;
    pNodeid_smb->node_id_smb_rsrvd_0_31 = 0xFFFFFFFF;
    pNodeid_smb->node_id_smb_hsbb = -1;
    pNodeid_smb->node_id_smb_rsrvd_56_63 = 0xFF;
    if ((status = gct$find_node(0, &hwnode, (FIND_BY_TYPE | FIND_BY_ID), NODE_SMB,  0, tmp, 0, 0)) != SUCCESS)
	return(status);
/* 
 ** Create a handle for this IOP node and insert it into the hardware branch of the
 ** configuration tree as the last sibling of the SMB hardware's children.
 */
    node = (gct_node_info[NODE_IOP].create_node)(NODE_IOP, 0, 0, 0, 0);
    gct__insert_node(hwnode, node, GCT__INSERT_CHILD_LAST_SIB);
#if DEBUG
    printf("Creating IOP %d HW child %x for parent %x\n", iop_id, node, hwnode);
#endif
/* 
 ** Make a pointer to the new IOP node from the handle and initialize it.
 */
    pIop_node = (GCT_IOP_NODE *)_GCT_MAKE_ADDRESS(node);
    pIop_node->hd_iop.hd_extension = offsetof(struct _gct_iop_node, ext_iop);
    pIop_node->hd_iop.depth = 3;
    pIop_node->hd_iop.node_flags |= NODE_HARDWARE;
    pIop_node->min_io_pa = 0;
    pIop_node->max_io_pa = mem_size - 1;
/*
 ** Initialize this node's ID field
 */
    pNodeid_iop = &pIop_node->hd_iop.id;
    pNodeid_iop->node_id_iop_iop = iop_id;
    pNodeid_iop->node_id_iop_rsrvd_8_31 = -1;
    pNodeid_iop->node_id_iop_hsbb = -1;
    pNodeid_iop->node_id_iop_rsrvd_56_63 = 0xFF;
/*
 ** Initialize the System Bus Bridge resource configuration subpacket 
 */
    pIop_node->iop_busbridge.busbridge_sub.subpack_length = sizeof(SUBPKT_BUSBRIDGE_FRU5);
    pIop_node->iop_busbridge.busbridge_sub.subpack_class = CLASS_REGATTA; /* erfix */
    pIop_node->iop_busbridge.busbridge_sub.subpack_type = SUBPACK_SYS_BUS_BRIDGE;                            
    pIop_node->iop_busbridge.busbridge_sub.subpack_rev = 3;                                                 
                                                                                                              
    pIop_node->iop_busbridge.bridge_level = 1;                                                 
    pIop_node->iop_busbridge.bridge_type = SNODE_PCI;                                         
    pIop_node->iop_busbridge.register_count = 0xE;                                               

/* 
 ** Fill in the header extension information
 */
    pIop_node->ext_iop.subpkt_count  = 1;                                                                     
    pIop_node->ext_iop.subpkt_offset = offsetof(struct _gct_iop_node, iop_busbridge);
/*
 ** Build a hose hardware node for this IOP
 */
    if ((status = build_hose_hw(iop_id, iop_id)) != SUCCESS)
	return(status);  

    return(SUCCESS);
}


/*+
 ** ============================================================================
 * = build_hose_hw - Construct a node for an I/O hose hardware resource       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_hose_hw(iop_id, hose_id); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *	iop_id  - I/O processor number
 *      hose_id - I/O hose number
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_hose_hw(int iop_id, int hose_id) 
{
    int status, bus;
    GCT_HANDLE hwnode = 0, node = 0;
    GCT_HOSE_NODE *pHose_node;
    GCT_NODE_ID_HOSE *pNodeid_hose;
    GCT_NODE_ID_IOP *pNodeid_iop;
    unsigned __int64 tmp = 0;

/* 
 ** Find the right IOP
 */
    pNodeid_iop = &tmp;
    pNodeid_iop->node_id_iop_iop = iop_id;
    pNodeid_iop->node_id_iop_rsrvd_8_31 = -1;
    pNodeid_iop->node_id_iop_hsbb = -1;
    pNodeid_iop->node_id_iop_rsrvd_56_63 = 0xFF;
    if ((status = gct$find_node(0, &hwnode, (FIND_BY_TYPE | FIND_BY_ID), NODE_IOP, 0, tmp, 0, 0)) != SUCCESS)
	return(status);
/* 
 ** Create a handle for this hose node and insert it into the hardware branch of the
 ** configuration tree as the last sibling of the IOP hardware's children.
 */
    node = (gct_node_info[NODE_HOSE].create_node)(NODE_HOSE, 0, 0, 0, 0);
    gct__insert_node(hwnode, node, GCT__INSERT_CHILD_LAST_SIB);
#if DEBUG
    printf("Creating hose %d HW child %x for parent %x\n", hose_id, node, hwnode);
#endif
/* 
 ** Make a pointer to the new hose node from the handle and initialize it.
 */
    pHose_node = (GCT_HOSE_NODE *)_GCT_MAKE_ADDRESS(node);
    pHose_node->hd_hose.depth = 4;
    pHose_node->hd_hose.node_flags |= NODE_HARDWARE;
/*
 ** Initialize this node's ID field
 */        
    pNodeid_hose = &pHose_node->hd_hose.id;
    pNodeid_hose->node_id_hose_iop = iop_id;
    pNodeid_hose->node_id_hose_hose = hose_id;
    pNodeid_hose->node_id_hose_rsrvd_16_31 = -1;
    pNodeid_hose->node_id_hose_hsbb = -1;
    pNodeid_hose->node_id_hose_rsrvd_56_63 = 0xFF;
/*
 ** Build a hardware node for pci and pci buses .
 */
	if ((status = build_bus_hw(iop_id, hose_id, 0)) != SUCCESS)
		return(status);  

	if ((status = build_bus_hw(iop_id, hose_id, 1)) != SUCCESS)
		return(status);

    return(SUCCESS);
}


/*+
 * ============================================================================
 * = build_bus_hw - Construct a node for an I/O bus hardware resource         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_bus_hw(iop_id, hose_id, bus_id); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *	iop_id  - I/O processor number
 *      hose_id - I/O hose number
 *	bus_id  - I/O bus number
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_bus_hw(int iop_id, int hose_id, int bus_id) 
{
    GCT_HANDLE hwnode = 0, node = 0;
    GCT_BUS_NODE *pBus_node;
    GCT_NODE_ID_HOSE *pNodeid_hose;
    GCT_NODE_ID_BUS *pNodeid_bus;
    int status, slot, function;
    unsigned __int64 tmp = 0;
    struct pb *pb;

/* 
 ** Find the right hose
 */
    pNodeid_hose = &tmp;
    pNodeid_hose->node_id_hose_iop  = iop_id;
    pNodeid_hose->node_id_hose_hose = hose_id;
    pNodeid_hose->node_id_hose_rsrvd_16_31 = -1;
    pNodeid_hose->node_id_hose_hsbb = -1;
    pNodeid_hose->node_id_hose_rsrvd_56_63 = 0xFF;
    if ((status = gct$find_node(0, &hwnode, (FIND_BY_TYPE | FIND_BY_ID), NODE_HOSE, 0, tmp, 0, 0)) != SUCCESS)
	return(status);
/* 
 ** Create a handle for this bus node and insert it into the hardware branch of the
 ** configuration tree as the last sibling of the hose hardware's children.
 */
    node = (gct_node_info[NODE_BUS].create_node)(NODE_BUS, 0, 0, 0, 0);
    gct__insert_node(hwnode, node, GCT__INSERT_CHILD_LAST_SIB);
#if DEBUG
    printf("Creating bus %d HW child %x for parent %x\n", bus_id, node, hwnode);
#endif
/* 
 ** Make a pointer to the new bus node from the handle and initialize it.
 */
    pBus_node = (GCT_BUS_NODE *)_GCT_MAKE_ADDRESS(node);
    pBus_node->hd_bus.subtype = SNODE_PCI;
    pBus_node->hd_bus.hd_extension = offsetof(struct _gct_bus_node,ext_bus);
    pBus_node->hd_bus.depth = 5;
    pBus_node->hd_bus.node_flags |= NODE_HARDWARE;
/*
 ** Initialize this node's ID field
 */
    pNodeid_bus = &pBus_node->hd_bus.id;
    pNodeid_bus->node_id_bus_iop = iop_id;
    pNodeid_bus->node_id_bus_hose = hose_id;
    pNodeid_bus->node_id_bus_bus = bus_id;
    pNodeid_bus->node_id_bus_rsrvd_24_31 = -1;
    pNodeid_bus->node_id_bus_hsbb = -1;
    pNodeid_bus->node_id_bus_rsrvd_56_63 = 0xFF;
/* 
 ** Fill in the header extension information
 */
    pBus_node->ext_bus.subpkt_count = 1;
    pBus_node->ext_bus.subpkt_offset = offsetof(struct _gct_bus_node, bus_sub);
/*
 ** Initialize the Bus resource configuration subpacket 
 */
    pBus_node->bus_sub.bus_sub.subpack_length = sizeof(SUBPKT_BUS_FRU5);
    pBus_node->bus_sub.bus_sub.subpack_class = CLASS1;
    pBus_node->bus_sub.bus_sub.subpack_type = SUBPACK_BUS;
    pBus_node->bus_sub.bus_sub.subpack_rev = 1;
/*
 ** Build hardware nodes for each I/O controller attached to this hose/bus.
 */
    for (slot = 0; slot <= 31; slot++) {
	for (function = 0; function <= 7; function++) {
	    if ((pb = get_matching_pb(hose_id, bus_id, slot, function, 0)) != (struct pb *)0) {
		if (pb->type == TYPE_PCI) {
		    if (pb->hose == 0 && pb->bus == 0 && pb->slot == 5) {
		    	if (pb->function == 0)
			    if ((status = build_isa_ioctrl_hw(iop_id, hose_id, bus_id)) != SUCCESS)
				return(status);
		    }
		    else {
                        if ((status = build_ioctrl_hw(iop_id, hose_id, bus_id, slot, function)) != SUCCESS)
			    return(status);
#if DEBUG
			printf("hose: %d bus: %d slot: %2d\t %-20.20s\n", 
				hose_id, bus_id, slot, pb->hardware_name);
#endif
		    }
		}
	    }
	}
    }

    return(SUCCESS);
}


/*+
 * ============================================================================
 * = build_isa_ioctrl_hw - Construct a node for an ISA I/O controller resource=
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_isa_ioctrl_hw(iop_id, hose_id, bus_id); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *	iop_id  - I/O processor number
 *	hose_id - hose number
 *	bus_id	- bus number
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_isa_ioctrl_hw(int iop_id, int hose_id, int bus_id) 
{
    int status;
    GCT_HANDLE hwnode = 0, node = 0;
    GCT_NODE *pHW_node;
    GCT_IOCTRL_ISA_NODE *pIoctrl_node;
    GCT_NODE_ID_IO_CTRL *pNodeid_ioctrl;
    GCT_NODE_ID_HOSE *pNodeid_hose;
    unsigned __int64 tmp = 0;

/* 
 ** Find the right hose
 */
    pNodeid_hose = &tmp;
    pNodeid_hose->node_id_hose_iop = iop_id;
    pNodeid_hose->node_id_hose_hose = hose_id;
    pNodeid_hose->node_id_hose_rsrvd_16_31 = -1;
    pNodeid_hose->node_id_hose_hsbb = -1;
    pNodeid_hose->node_id_hose_rsrvd_56_63 = 0xFF;
    if ((status = gct$find_node(0, &hwnode, (FIND_BY_TYPE | FIND_BY_ID), NODE_HOSE,  0, tmp, 0, 0)) != SUCCESS)
	return(status);
/* 
 ** Make a pointer to the hose node from the handle.
 */
    pHW_node = _GCT_MAKE_ADDRESS(hwnode);
    hwnode = pHW_node->child;
    pHW_node = _GCT_MAKE_ADDRESS(hwnode);
/* 
 ** Create a handle for this I/O controller node and insert it into the hardware 
 ** branch of the configuration tree as the last sibling of the hose hardware's 
 ** children.
 */
    node = (gct_node_info[NODE_IO_CTRL].create_node)(NODE_IO_CTRL, 0, 0, 0, 0);
    gct__insert_node(hwnode, node, GCT__INSERT_CHILD_LAST_SIB);
#if DEBUG
    printf("Creating ISA I/O Ctlr HW child %x for parent %x\n", node, hwnode);
#endif
/* 
 ** Make a pointer to the new I/O controller node from the handle and initialize it.
 */
    pIoctrl_node = (GCT_IOCTRL_ISA_NODE *)_GCT_MAKE_ADDRESS(node);
    pIoctrl_node->hd_ioctrl_isa.hd_extension = offsetof(struct _gct_ioctrl_isa_node, ext_ioctrl_isa);
    pIoctrl_node->hd_ioctrl_isa.depth = 6;
    pIoctrl_node->hd_ioctrl_isa.subtype = SNODE_ISA;
    pIoctrl_node->hd_ioctrl_isa.node_flags |= NODE_HARDWARE;
/*
 ** Initialize this node's ID field
 */
    pNodeid_ioctrl = &pIoctrl_node->hd_ioctrl_isa.id;
    pNodeid_ioctrl->node_id_io_ctrl_iop	= iop_id;
    pNodeid_ioctrl->node_id_io_ctrl_hose = hose_id;
    pNodeid_ioctrl->node_id_io_ctrl_bus	= bus_id;
    pNodeid_ioctrl->node_id_io_ctrl_ctrlr = 5;
    pNodeid_ioctrl->node_id_io_ctrl_hsbb = -1;
    pNodeid_ioctrl->node_id_io_ctrl_rsrvd_56_63 = 0xFF;
/* 
 ** Fill in the header extension information
 */
    pIoctrl_node->ext_ioctrl_isa.subpkt_count = 1;
    pIoctrl_node->ext_ioctrl_isa.subpkt_offset = offsetof(struct _gct_ioctrl_isa_node, isa_busbridge);
/*
 ** Initialize the ISA Bus Bridge resource configuration subpacket 
 */
    pIoctrl_node->isa_busbridge.busbridge_sub.subpack_length = sizeof(SUBPKT_BUSBRIDGE_FRU5);
    pIoctrl_node->isa_busbridge.busbridge_sub.subpack_class = CLASS1;
    pIoctrl_node->isa_busbridge.busbridge_sub.subpack_type = 4;
    pIoctrl_node->isa_busbridge.busbridge_sub.subpack_rev = 3;
    pIoctrl_node->isa_busbridge.bridge_level = 2;
    pIoctrl_node->isa_busbridge.bridge_type = SNODE_ISA;
    pIoctrl_node->isa_busbridge.register_count = 0;
    pIoctrl_node->isa_busbridge.physical_address = 0;

    return (SUCCESS);
}


/*+
 * ============================================================================
 * = build_ioctrl_hw - Construct a node for an I/O controller resource        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_ioctrl_hw(iop_id, hose_id, bus_id, slot_id, func_id); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *	iop_id  - I/O processor number
 *	hose_id - hose number
 *	bus_id	- bus number
 *	slot_id - slot number
 *	func_id - function number
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_ioctrl_hw(int iop_id, int hose_id, int bus_id, int slot_id, int func_id) 
{
    int status;
    GCT_HANDLE hwnode = 0, node = 0;
    GCT_NODE *pHW_node;
    GCT_IO_CTRL_NODE *pIoctrl_node;
    GCT_NODE_ID_IO_CTRL *pNodeid_ioctrl;
    GCT_NODE_ID_HOSE *pNodeid_hose;
    unsigned __int64 tmp = 0;
    struct pb pb;
    int i, *ptr, offset = 0;

/* 
 ** Find the right hose
 */
    pNodeid_hose = &tmp;
    pNodeid_hose->node_id_hose_iop = iop_id;
    pNodeid_hose->node_id_hose_hose = hose_id;
    pNodeid_hose->node_id_hose_rsrvd_16_31 = -1;
    pNodeid_hose->node_id_hose_hsbb = -1;
    pNodeid_hose->node_id_hose_rsrvd_56_63 = 0xFF;  
    if ((status = gct$find_node(0, &hwnode, (FIND_BY_TYPE | FIND_BY_ID), NODE_HOSE, 0, tmp, 0, 0)) != SUCCESS)
	return(status);
/* 
 ** Make a pointer to the hose node from the handle.
 */
    pHW_node = _GCT_MAKE_ADDRESS(hwnode);
    hwnode = pHW_node->child;
    pHW_node = _GCT_MAKE_ADDRESS(hwnode);
/* 
 ** Create a handle for this I/O controller node and insert it into the hardware 
 ** branch of the configuration tree as the last sibling of the hose hardware's 
 ** children.
 */
    node = (gct_node_info[NODE_IO_CTRL].create_node)(NODE_IO_CTRL, 0, 0, 0, 0);
    gct__insert_node(hwnode, node, GCT__INSERT_CHILD_LAST_SIB);
#if DEBUG
    printf("Creating I/O Ctlr HW child %x for parent %x ", node, hwnode);
#endif
/* 
 ** Make a pointer to the new I/O controller node from the handle and initialize it.
 */
    pIoctrl_node = (GCT_IO_CTRL_NODE *)_GCT_MAKE_ADDRESS(node);
    pIoctrl_node->hd_io_ctrl.hd_extension = offsetof(struct _gct_io_ctrl_node, ext_io_ctrl);
    pIoctrl_node->hd_io_ctrl.depth = 6;
    pIoctrl_node->hd_io_ctrl.subtype = SNODE_PCI;
    pIoctrl_node->hd_io_ctrl.node_flags |= NODE_HARDWARE;
/*
 ** Initialize this node's ID field
 */                                                                                        
    pNodeid_ioctrl = &pIoctrl_node->hd_io_ctrl.id;                                         
    pNodeid_ioctrl->node_id_io_ctrl_iop = iop_id;                                          
    pNodeid_ioctrl->node_id_io_ctrl_hose = hose_id;                                        
    pNodeid_ioctrl->node_id_io_ctrl_bus = bus_id;                                          
    pNodeid_ioctrl->node_id_io_ctrl_ctrlr = slot_id | (func_id << 5);               
    pNodeid_ioctrl->node_id_io_ctrl_hsbb = -1;
    pNodeid_ioctrl->node_id_io_ctrl_rsrvd_56_63 = 0xFF;
/*
 ** Initialize the PCI device resource configuration subpacket 
 */
    pIoctrl_node->pci_sub.pci_sub.subpack_length = sizeof(SUBPKT_PCI_FRU5);
    pIoctrl_node->pci_sub.pci_sub.subpack_class = CLASS1;
    pIoctrl_node->pci_sub.pci_sub.subpack_type = 5;
    pIoctrl_node->pci_sub.pci_sub.subpack_rev = 2;

    pIoctrl_node->pci_sub.pci_config_addr = pci_cfg_base(hose_id, bus_id, slot_id, func_id);
    pb.hose = hose_id;
    pb.bus = bus_id;
    pb.slot = slot_id;
    pb.function = func_id;
    ptr = &pIoctrl_node->pci_sub.pci_head.pci_space0;
    for (i = 0;  i < 16;  i++) {
    	*ptr = incfgl(&pb, offset);
	ptr++;
	offset += 4;
    }
/*
 ** Initialize the PCI VPD (Vital Product Data) resource configuration subpacket 
 */
    pIoctrl_node->pci_vpd.pci_vpd_sub.subpack_length = sizeof(SUBPKT_PCI_VPD_FRU5);
    pIoctrl_node->pci_vpd.pci_vpd_sub.subpack_class = CLASS1;
    pIoctrl_node->pci_vpd.pci_vpd_sub.subpack_type = 30;
    pIoctrl_node->pci_vpd.pci_vpd_sub.subpack_rev = 1;
/* 
 ** Fill in the header extension information
 */
    pIoctrl_node->ext_io_ctrl.fru_count = 1;
    pIoctrl_node->ext_io_ctrl.subpkt_count = 2;
    pIoctrl_node->ext_io_ctrl.subpkt_offset = offsetof(struct _gct_io_ctrl_node, pci_sub);

    return (SUCCESS);
}


/*+
 * ============================================================================
 * = build_memory_sub_hw - Construct a node for the Memory subsystem resource =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_memory_sub_hw(hw_root); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *      hw_root - a handle to the root node of the hardware branch
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_memory_sub_hw(GCT_HANDLE hw_root) 
{
    int status;
    GCT_HANDLE hwnode = 0, node = 0;
    GCT_MEMORY_SUB_NODE *pMemsub_node;
    GCT_NODE_ID_MEM_SUB *pNodeid_mem_sub;
    GCT_NODE_ID_SMB *pNodeid_smb;
    unsigned __int64 tmp = 0;
/* 
 ** Get a pointer to the SMB node
 */
    pNodeid_smb = &tmp;
    pNodeid_smb->node_id_smb_rsrvd_0_31 = -1;
    pNodeid_smb->node_id_smb_hsbb = -1;
    pNodeid_smb->node_id_smb_rsrvd_56_63 = 0xFF;
    if ((status = gct$find_node(0, &hwnode, (FIND_BY_TYPE | FIND_BY_ID), NODE_SMB,  0, tmp, 0, 0)) != SUCCESS)
	return(status);
/* 
 ** Create a handle for the Memory Subsystem node and insert it into the hardware 
 ** branch of the configuration tree as the last sibling of the SMB hardware's children.
 */
    node = (gct_node_info[NODE_MEMORY_SUB].create_node)(NODE_MEMORY_SUB, 0, 0, 0, 0);
    gct__insert_node(hwnode, node, GCT__INSERT_CHILD_LAST_SIB);
#if DEBUG
    printf("Creating Memory Subsystem HW child %x for parent %x\n", node, hwnode);
#endif
/* 
 ** Make a pointer to the new Memory Subsystem node from the handle and initialize it.
 */
    pMemsub_node = (GCT_MEMORY_SUB_NODE *)_GCT_MAKE_ADDRESS(node);
    pMemsub_node->hd_memory_sub.depth = 3;
    pMemsub_node->hd_memory_sub.node_flags |= NODE_HARDWARE;
    pMemsub_node->min_pa = 0;
    pMemsub_node->max_pa = mem_size - 1;
/*
 ** Initialize this node's ID field
 */
    pNodeid_mem_sub = &pMemsub_node->hd_memory_sub.id;
    pNodeid_mem_sub->node_id_mem_sub_memsub = 0;
    pNodeid_mem_sub->node_id_mem_sub_rsrvd_8_31 = -1;
    pNodeid_mem_sub->node_id_mem_sub_hsbb = -1;
    pNodeid_mem_sub->node_id_mem_sub_rsrvd_56_63 = 0xFF;
/*
 ** Build a single Memory Controller node 
 */
    if ((status = build_memory_ctrl_hw(0)) != SUCCESS)
	return(status);

    return(SUCCESS);
}


/*+
 * ============================================================================
 * = build_memory_ctrl_hw - Construct a node for a Memory controller resource =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_memory_ctrl_hw(ctrl_id); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *      ctrl_id - memory controller number
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_memory_ctrl_hw(int ctrl_id) 
{
    int i, status, orig_size, num_subpkts = 0;
    GCT_ROOT_NODE *pRoot;
    GCT_HANDLE hwnode = 0, node = 0;
    GCT_MEMORY_CTRL_NODE *pMemctrl_node;
    GCT_NODE_ID_MEM_CTRL *pNodeid_mem_ctrl;
    GCT_NODE_ID_MEM_SUB *pNodeid_mem_sub;
    SUBPKT_MEMORY_FRU5 *pSubpkt;
    GCT_HD_EXT *pHd_ext;
    unsigned __int64 tmp = 0;

/* 
 ** Get a pointer to the root of the configuration tree
 */
    pRoot = (GCT_ROOT_NODE *)_GCT_MAKE_ADDRESS(0);
/* 
 ** Get a pointer to the Memory Subsystem node
 */
    pNodeid_mem_sub = &tmp;
    pNodeid_mem_sub->node_id_mem_sub_rsrvd_8_31 = -1;
    pNodeid_mem_sub->node_id_mem_sub_hsbb = -1;
    pNodeid_mem_sub->node_id_mem_sub_rsrvd_56_63 = 0xFF;
    if ((status = gct$find_node(0, &hwnode, (FIND_BY_TYPE | FIND_BY_ID), NODE_MEMORY_SUB,  0, tmp, 0, 0)) != SUCCESS)
	return(status);
/* 
 ** Create a handle for the Memory Controller node and insert it into the hardware 
 ** branch of the configuration tree as the last sibling of the memory subsystem's children.
 */
    node = (gct_node_info[NODE_MEMORY_CTRL].create_node)(NODE_MEMORY_CTRL, 0, 0, 0, 0);
    gct__insert_node(hwnode, node, GCT__INSERT_CHILD_LAST_SIB);
/* 
 ** Make a pointer to the new Memory Controller node from the handle and initialize it.
 */
    pMemctrl_node = (GCT_MEMORY_CTRL_NODE *)_GCT_MAKE_ADDRESS(node);
    pMemctrl_node->hd_memory_ctrl.depth = 4;
    pMemctrl_node->hd_memory_ctrl.node_flags |= NODE_HARDWARE;
/*
 ** Initialize this node's ID field
 */
    pNodeid_mem_ctrl = &pMemctrl_node->hd_memory_ctrl.id;
    pNodeid_mem_ctrl->node_id_mem_ctrl_memctrl = ctrl_id;
    pNodeid_mem_ctrl->node_id_mem_ctrl_rsrvd_24_31 = -1;
    pNodeid_mem_ctrl->node_id_mem_ctrl_hsbb = -1;
    pNodeid_mem_ctrl->node_id_mem_ctrl_rsrvd_56_63 = 0xFF;
/* 
 ** Save original size before adding any subpackets.
 */
    orig_size = pMemctrl_node->hd_memory_ctrl.size;
    pRoot->available += pMemctrl_node->hd_memory_ctrl.size;
    pRoot->first_free -= pMemctrl_node->hd_memory_ctrl.size;

    pMemctrl_node->hd_memory_ctrl.size = sizeof(GCT_NODE);
    pSubpkt = &pMemctrl_node->hd_memory_ctrl.magic + 1;

/* 
 ** Initialize a Memory Controller resource configuration subpacket for
 ** each memory array configured in the system.
 */
    for (i = 0;  i < MAX_MEMORY_ARRAY;  i++)
	{
		if (get_array_size(i) != 0)
		{
			pSubpkt->memory_sub.subpack_length = sizeof(SUBPKT_MEMORY_FRU5);
			pSubpkt->memory_sub.subpack_class = CLASS_REGATTA; /* erfix */
			pSubpkt->memory_sub.subpack_type = SUBPACK_MEMORY;
			pSubpkt->memory_sub.subpack_rev = 3;

			pSubpkt->mem_id = i;
			pSubpkt->register_count = 2;

			pMemctrl_node->hd_memory_ctrl.size += sizeof(SUBPKT_MEMORY_FRU5);
			num_subpkts++;
			pSubpkt++;
		}
    }
/* 
 ** Fill in the header extension information
 */
    pHd_ext = (GCT_HD_EXT *)pSubpkt;
    pMemctrl_node->hd_memory_ctrl.hd_extension = pMemctrl_node->hd_memory_ctrl.size;
    pHd_ext->fru_count = 0;
    pHd_ext->subpkt_count = num_subpkts;
    pHd_ext->subpkt_offset = offsetof(struct _gct_memory_ctrl_node, mem_ctrl_sub);
    pMemctrl_node->hd_memory_ctrl.size += sizeof(GCT_HD_EXT);
/* 
 ** Quadword align 
 */
    pMemctrl_node->hd_memory_ctrl.size = ((pMemctrl_node->hd_memory_ctrl.size + 63) & ~63);
/* 
 ** Update fields in header so next node has proper offset
 */
    pRoot->available -= pMemctrl_node->hd_memory_ctrl.size;
    pRoot->first_free += pMemctrl_node->hd_memory_ctrl.size;
/*
 ** Build a Memory Descriptor node
 */
    if ((status = build_memory_desc_hw()) != SUCCESS)
	return(status);

    return(SUCCESS);
}


/*+
 * ============================================================================
 * = build_memory_desc_hw - Construct a node for a Memory descriptor resource =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_memory_desc_hw(); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_memory_desc_hw(void) 
{
    int status;
    GCT_HANDLE hwnode = 0, node = 0;
    GCT_MEM_DESC_NODE *pMemdesc_node;
    GCT_NODE_ID_MEM_DESC *pNodeid_mem_desc;
    GCT_NODE_ID_MEM_SUB *pNodeid_mem_sub;
    unsigned __int64 tmp = 0;

/* 
 ** Get a pointer to the Memory Subsystem node
 */
    pNodeid_mem_sub = &tmp;
    pNodeid_mem_sub->node_id_mem_sub_rsrvd_8_31 = -1;
    pNodeid_mem_sub->node_id_mem_sub_hsbb = -1;
    pNodeid_mem_sub->node_id_mem_sub_rsrvd_56_63 = 0xFF;
    if ((status = gct$find_node(0, &hwnode, (FIND_BY_TYPE | FIND_BY_ID), NODE_MEMORY_SUB,  0, tmp, 0, 0)) != SUCCESS)
	return(status);
/* 
 ** Create a handle for the Memory Descriptor node and insert it into the hardware 
 ** branch of the configuration tree as the last sibling of the memory subsystem's children.
 */
    node = (gct_node_info[NODE_MEMORY_DESC].create_node)(NODE_MEMORY_DESC, 0, 0, 0, 0);
    gct__insert_node(hwnode, node, GCT__INSERT_CHILD_LAST_SIB);
#if DEBUG
    printf("Creating Memory Descriptor HW child %x for parent %x\n", node, hwnode);
#endif
/* 
 ** Make a pointer to the new Memory Descriptor node from the handle and initialize it.
 */
    pMemdesc_node = (GCT_MEM_DESC_NODE *)_GCT_MAKE_ADDRESS(node);
    pMemdesc_node->hd_mem_desc.hd_extension = offsetof(struct _gct_mem_desc_node, ext_mem_desc);
    pMemdesc_node->hd_mem_desc.depth = 4;
    pMemdesc_node->hd_mem_desc.node_flags |= NODE_HARDWARE;
/* 
 ** Fill in the header extension information
 */
    pMemdesc_node->ext_mem_desc.fru_count = 0;
/*
 ** Initialize this node's ID field
 */
    pNodeid_mem_desc = &pMemdesc_node->hd_mem_desc.id;
    pNodeid_mem_desc->node_id_mem_desc_memdesc = 0;
    pNodeid_mem_desc->node_id_mem_desc_rsrvd_24_31 = -1;
    pNodeid_mem_desc->node_id_mem_desc_hsbb = -1;
    pNodeid_mem_desc->node_id_mem_desc_rsrvd_56_63 = 0xFF;
/*
 ** Initialize the Memory Descriptor's configuration node
 */
    pMemdesc_node->mem_info.base_pa = 0;
    pMemdesc_node->mem_info.base_size = mem_size - 1;
    pMemdesc_node->mem_info.desc_count = 0;
    pMemdesc_node->mem_frag  = 0;
    pMemdesc_node->bitmap_pa = 0;

    return(SUCCESS);
}


/*+
 * ============================================================================
 * = build_power_hw - Construct a node for the Power/Environmental resource   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_power_hw(hw_root); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *      hw_root - a handle to the root node of the hardware branch
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_power_hw(GCT_HANDLE hw_root) 
{
    int i, status, orig_size;
    GCT_ROOT_NODE *pRoot;
    GCT_HANDLE node = 0;
    GCT_POWER_ENVIR_NODE *pPower_node;
    GCT_NODE_ID_POWER_ENVIR5 *pNodeid_power;
    unsigned __int64 tmp = 0;
    GCT_HD_EXT *pHd_ext;
    SUBPKT_SENSOR_FRU5 *pSensor;
    struct FILE *fp;
    int num_subpkts = 0;
    unsigned char fail_register;
    unsigned char func_register;
    unsigned char ps_present[ ] = { 7, 4, 6};
    unsigned char ps_ok[ ] = { 5, 6, 7 };     
    unsigned char fan_ok[ ] = { 5, 1 };       
    int goldrack = 0;
        
    if (fp = fopen("iic_8574_ocp", "sr+")) {
       goldrack = 1;
       fclose(fp);                           
    }   
/*                   
 ** Get a pointer to the root of the configuration tree
 */                  
    pRoot = (GCT_ROOT_NODE *)_GCT_MAKE_ADDRESS(0);
/*                   
 ** Create a handle for the Power/Environmental node and insert it into the hardware 
 ** branch of the configuration tree as the last sibling of the hardware root's children.
 */                  
    node = (gct_node_info[NODE_POWER_ENVIR].create_node)(NODE_POWER_ENVIR, 0, 0, 0, 0);
    gct__insert_node(hw_root, node, GCT__INSERT_CHILD_LAST_SIB);
#if DEBUG            
    printf("Creating Power/Environmental HW child %x for parent %x\n", node, hw_root);
#endif                               
/*                                   
 ** Make a pointer to the new Power/Environmental node from the handle and initialize it.
 */                                  
    pPower_node = (GCT_POWER_ENVIR_NODE *)_GCT_MAKE_ADDRESS(node);
    pPower_node->hd_power_envir.depth = 2;
    pPower_node->hd_power_envir.node_flags |= NODE_HARDWARE;
/*                                   
 ** Initialize this node's ID field   
 */                                  
    pNodeid_power = &pPower_node->hd_power_envir.id;
    pNodeid_power->node_id_power_envir_pe_num = 0;
    pNodeid_power->node_id_power_rsrvd_16_23 = -1;
    pNodeid_power->node_id_power_envir_hsbb = -1;
/*                                   
 ** Save original size before adding any subpackets.
 */                                  
    orig_size = pPower_node->hd_power_envir.size;
    pRoot->available += pPower_node->hd_power_envir.size;
    pRoot->first_free -= pPower_node->hd_power_envir.size;
/*                                   
 ** Initialize the Sensor resource configuration subpackets
 */                                  
    pPower_node->hd_power_envir.size = sizeof(GCT_POWER_ENVIR_NODE);
    pSensor = &pPower_node->hd_power_envir.magic + 1;
                                     
    if ((fp = fopen("iic_system0", "sr")) != 0) {
	status = fread(&fail_register, 1, 1, fp);
	fclose(fp);                                                                             
    }                                                                                             
    if ((fp = fopen("iic_system1", "sr")) != 0) {                                           
	status = fread(&func_register, 1, 1, fp);                                               
	fclose(fp);                                                   
    }                                                                   
    if (!status)                                                      
	return(status);                                               
                                                                        
                                                                        
    for (i = 0;  (i == 0) || ((i < 3) && goldrack);  i++)            
    {                                                                   
    	if ((!(fail_register & (1 << ps_present[i]))) &&         
	    	(!(func_register & (1 << ps_ok[i]))))  
        {                                 
    	  	/*                                                                      
	  	 ** DC Power subpacket                     
	  	 */                      
                                         
	    pSensor->sensor_sub.subpack_length = sizeof(SUBPKT_SENSOR_FRU5);
	    pSensor->sensor_sub.subpack_class = CLASS1;
    	    pSensor->sensor_sub.subpack_type = SUBPACK_SENSOR;
    	    pSensor->sensor_sub.subpack_rev = 2;   
    	    pSensor->console_id = i;               
    	    pSensor->sensor_class = SENSOR_CLASS_DCPWR;
    	    pSensor->sensor_properties  = SENSOR_PROP_STATUS | SENSOR_PROP_UPPERLONG_BITSUPPORT;
    	    pPower_node->hd_power_envir.size += sizeof(SUBPKT_SENSOR_FRU5);
    	    pSensor = &pSensor->sensor_properties + 1;
    	    num_subpkts++;               
    	}                                
       	if ((i < 2) && ((func_register & (1 << 1)) || (fail_register & (1 << fan_ok[i])))) 
        {                                
/*                                                                                                 
 ** System Fan subpacket                  
 */                                      
	    pSensor->sensor_sub.subpack_length = sizeof(SUBPKT_SENSOR_FRU5);
	    pSensor->sensor_sub.subpack_class = CLASS1;
	    pSensor->sensor_sub.subpack_type = SUBPACK_SENSOR;
	    pSensor->sensor_sub.subpack_rev = 2;
	    pSensor->console_id = i + 1;
	    pSensor->sensor_class = SENSOR_CLASS_FAN;
	    pSensor->sensor_properties = SENSOR_PROP_VALUE;
	    pPower_node->hd_power_envir.size += sizeof(SUBPKT_SENSOR_FRU5);
	    pSensor = &pSensor->sensor_properties + 1;
	    num_subpkts++;
	}           
    }               
    if (fail_register & (1 << 0)) {
/*                  
 ** Temperature sensor subpacket
 */
	pSensor->sensor_sub.subpack_length = sizeof(SUBPKT_SENSOR_FRU5);
	pSensor->sensor_sub.subpack_class = CLASS1;
	pSensor->sensor_sub.subpack_type = SUBPACK_SENSOR;
	pSensor->sensor_sub.subpack_rev = 2;
	pSensor->console_id = 0x9E;
	pSensor->sensor_class = SENSOR_CLASS_TEMP;
	pSensor->sensor_properties = SENSOR_PROP_VALUE;
        pPower_node->hd_power_envir.size += sizeof(SUBPKT_SENSOR_FRU5);
	pSensor = &pSensor->sensor_properties + 1;
	num_subpkts++;
    }
/* 
 ** Fill in the header extension information
 */
    pHd_ext = (GCT_HD_EXT *)pSensor;
    pPower_node->hd_power_envir.hd_extension = pPower_node->hd_power_envir.size;
    pHd_ext->fru_count = 1;
    pHd_ext->subpkt_count = num_subpkts;
    pHd_ext->subpkt_offset = sizeof(GCT_NODE);
    pPower_node->hd_power_envir.size += sizeof(GCT_HD_EXT);
/* 
 ** Quadword align 
 */
    pPower_node->hd_power_envir.size = ((pPower_node->hd_power_envir.size + 63) & ~63); 
/* 
 ** Update fields in header so next node has proper offset
 */
    pRoot->available -= pPower_node->hd_power_envir.size;
    pRoot->first_free += pPower_node->hd_power_envir.size;

    return(SUCCESS);
}


/*+
 * ============================================================================
 * = build_fru_root - Construct the root node on the FRU branch of the tree   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_fru_root(hw_root); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *      hw_root - a handle to the root node of the hardware branch
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_fru_root(GCT_HANDLE hw_root) 
{
    unsigned int status, orig_size;
    GCT_ROOT_NODE *pRoot;
    GCT_HW_ROOT_NODE *pHWroot_node;
    GCT_HANDLE node = 0, fru_root = 0;
    GCT_FRU_ROOT_NODE *pFru_root_node;
    GCT_FRU_DESC_NODE *pFru_desc_node;
    GCT_NODE_ID_FRU_DESC *pFru_root_id;
    GCT_TLV *pTLV;
    unsigned char *pValue;
    struct DSRDB *dsrdb;
    unsigned __int64 *name;
    struct EVNODE evnode, *evp;
    char buffer[32];

#if DEBUG
    printf("Building FRU root node for %x\n", hw_root);
#endif
/* 
 ** Get a pointer to the root of the configuration tree
 */
    pRoot = (GCT_ROOT_NODE *)_GCT_MAKE_ADDRESS(0);
/* 
 ** Get a pointer to the root of the hardware branch of the configuration tree
 */
    pHWroot_node = _GCT_MAKE_ADDRESS(hw_root);
/* 
 ** Create a handle for the FRU root and insert it into the hardware branch of the
 ** configuration tree as the last sibling of the hardware root.
 */
    node = (gct_node_info[NODE_FRU_ROOT].create_node)(NODE_FRU_ROOT, 0, 0, 0, 0);
    gct__insert_node(hw_root, node, GCT__INSERT_LAST_SIBLING);
/*
 ** Grab a handle to the FRU root out of the configuration tree.
 */
    if ((status = gct$find_node(0, &fru_root, FIND_BY_TYPE, NODE_FRU_ROOT, 0, 0, 0, 0)) != SUCCESS)
	return(status);
/* 
 ** Update the hardware root to point to the FRU root 
 */
    pHWroot_node->ext_hroot.fru = fru_root;
/* 
 ** Make a pointer to the FRU root node from the handle and initialize it.
 */
    pFru_root_node = (GCT_FRU_ROOT_NODE *)_GCT_MAKE_ADDRESS(fru_root);
    pFru_root_node->hd_fru_root.depth = 1;
/* 
 ** Make a pointer to a FRU descriptor from the root handle and initialize it.
 */
    pFru_desc_node = (GCT_FRU_DESC_NODE *)_GCT_MAKE_ADDRESS(fru_root);
/*
 ** Initialize this node's ID field
 */
    pFru_root_id = &pFru_root_node->hd_fru_root.id;
    pFru_root_id->node_id_fru_desc_site_loc    =  0;
    pFru_root_id->node_id_fru_desc_cab_id      = -1;
    pFru_root_id->node_id_fru_desc_position    = -1;
    pFru_root_id->node_id_fru_desc_chassis     = -1;
    pFru_root_id->node_id_fru_desc_subassembly = -1;
    pFru_root_id->node_id_fru_desc_assembly    = -1;
    pFru_root_id->node_id_fru_desc_slot        = -1;
/* 
 ** Save original size before adding to the descriptor
 */
    orig_size = pFru_desc_node->hd_fru_desc.size;
/* 
 ** Get the starting address of the first TLV 
 */
    pTLV = &pFru_root_node->root_fru_info.fru_info.tlv_tag;
/*
 ** Fill in the manufacturer TLV field
 */
    build_tlv(pTLV, 1, "Compaq", strlen("Compaq"), pFru_desc_node);
    _NEXT_TLV(pTLV);
/*
 ** Fill in the model number TLV field
 */
    dsrdb = (int)hwrpb + hwrpb->DSRDB_OFFSET[0];
    name = (int)dsrdb + dsrdb->NAME_OFFSET[0];
    build_tlv(pTLV, 1, name + 1, strlen(name + 1), pFru_desc_node);
    _NEXT_TLV(pTLV);
/*
 ** Fill in the part number TLV field
 */
    build_tlv(pTLV, 1, " ", 1, pFru_desc_node);
    _NEXT_TLV(pTLV);
/*
 ** Fill in the system serial number TLV field
 */
    evp = &evnode;
    ev_read("sys_serial_num", &evp, EV$K_SYSTEM);
    strcpy(buffer, evp->value.string);
    build_tlv(pTLV, 1, buffer, strlen(buffer), pFru_desc_node);
    _NEXT_TLV(pTLV);
/*
 ** Fill in the firmware revision TLV field
 */
    build_tlv(pTLV, 1, fw_rev, 9, pFru_desc_node);
/* 
 ** Pad the size of the descriptor to the nearest quadword 
 */
    pFru_desc_node->hd_fru_desc.size = ((pFru_desc_node->hd_fru_desc.size + 63) & ~63);
/* 
 ** Update fields in configuration tree header so that the next create has 
 ** the proper offset.
 */
    pRoot->available -= pFru_desc_node->hd_fru_desc.size;
    pRoot->first_free = (pRoot->first_free - orig_size) + pFru_desc_node->hd_fru_desc.size;

    return(SUCCESS);
}


/*+
 * ============================================================================
 * = build_smb_fru - Build a FRU descriptor for the System Motherboard        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_smb_fru(hw_root, fru_root); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *      hw_root  - a handle to the root node of the hardware branch
 *	fru_root - a handle to the root node of the FRU branch
 *
 * SIDE EFFECTS:
 *                                           
 *      None
 *   
-*/
int build_smb_fru(GCT_HANDLE hw_root, GCT_HANDLE fru_root)
{
	unsigned int status, orig_size;
	GCT_ROOT_NODE *pRoot;
	GCT_HANDLE node = 0, hw_node = 0;
	GCT_SMB_NODE *pSmb_node;
	GCT_FRU_DESC_NODE *pFru_desc_node;
	GCT_NODE_ID_FRU_DESC *pNodeid_fru_desc;
	GCT_NODE_ID_SMB *pNodeid_smb;
	unsigned __int64 tmp = 0;
	GCT_TLV *pTLV;
	struct FILE *fp;
	char buffer[sizeof(JEDEC)];
	JEDEC *pJedec;
	char piecebuffer[16], b[10];
	int h, i, j, k;
	struct pb *pb;

	/* 
	** Get a pointer to the root of the configuration tree
	*/
	pRoot = (GCT_ROOT_NODE *)_GCT_MAKE_ADDRESS(0);

	/* 
	** Create a handle for the SMB FRU and insert it into the FRU branch of the
	** configuration tree as the last sibling of the FRU root's children.
	*/
	node = (gct_node_info[NODE_FRU_DESC].create_node)(NODE_FRU_DESC, 0, 0, 0, 0);
	gct__insert_node(fru_root, node, GCT__INSERT_CHILD_LAST_SIB);

	/* 
	** Get a handle to the SMB node on the hardware branch of the configuration tree
	*/
	pNodeid_smb = &tmp;
	pNodeid_smb->node_id_smb_rsrvd_0_31 = -1;
	pNodeid_smb->node_id_smb_hsbb = -1;
	pNodeid_smb->node_id_smb_rsrvd_56_63 = -1;
	
	if ((status = gct$find_node(0, &hw_node, (FIND_BY_TYPE | FIND_BY_ID), NODE_SMB, 0, tmp, 0, 0)) != SUCCESS)
		return(status);

#if DEBUG
    printf("Creating SMB FRU child %x for HW node %x\n", node, hw_node);
#endif

	/* 
	** Make a pointer to the SMB HW node from the handle and link it to the FRU node.
	*/
    pSmb_node = (GCT_SMB_NODE *)_GCT_MAKE_ADDRESS(hw_node);
    pSmb_node->ext_smb.fru = node;

	/* 
	** Make a pointer to a FRU descriptor from the handle and initialize it.
	*/
	pFru_desc_node = (GCT_FRU_DESC_NODE *)_GCT_MAKE_ADDRESS(node);
	pFru_desc_node->hd_fru_desc.depth = 2;

	/*
	** Fill in the FRU descriptor node id which acts as a physical locator.
	*/
	pNodeid_fru_desc = &pFru_desc_node->hd_fru_desc.id;
	pNodeid_fru_desc->node_id_fru_desc_site_loc = 0xff;
	pNodeid_fru_desc->node_id_fru_desc_cab_id = 0x00;
	pNodeid_fru_desc->node_id_fru_desc_position = 0xff;
	pNodeid_fru_desc->node_id_fru_desc_chassis = 0x00;
	pNodeid_fru_desc->node_id_fru_desc_assembly = 0x00;
	pNodeid_fru_desc->node_id_fru_desc_subassembly = 0xff;

	/*
	** Assign a platform-specific identifier code to provide the final level of 
	** granularity required to resolve this individual FRU's physical location in 
	** the system.
	**
	** For the System Motherboard, this is the IIC offset where the FRU EEPROM
	** is located.
	*/
	pNodeid_fru_desc->node_id_fru_desc_slot = 0x00A2;

	/* 
	** Save the original size before adding to the descriptor 
	*/
	orig_size = pFru_desc_node->hd_fru_desc.size;

	/* 
	** Get the starting address of the first TLV 
	*/
	pTLV = &pFru_desc_node->fru_desc_info.fru_info.tlv_tag;

#ifdef FIX_LATER
	/*
	** Open the FRU EEPROM on the motherboard and read 256
	** bytes of JEDEC-like data.
	*/
    if ((fp = fopen("iic_smb0", "sr")) != 0)
	{
		fread(buffer, sizeof(JEDEC), 1, fp);
		fclose(fp);
		pJedec = &buffer;

		/*
		** Fill in the manufacturer TLV field
		*/
		memcpy(piecebuffer, pJedec->jedec_b_manuf_spec_alias, 10);
		build_tlv(pTLV, 1, piecebuffer, 10, pFru_desc_node);
		_NEXT_TLV(pTLV);

		/*
		** Fill in the model number TLV field
		*/
		memcpy(piecebuffer, pJedec->jedec_b_manuf_spec_model, 10);
		build_tlv(pTLV, 1, piecebuffer, 10, pFru_desc_node);
		_NEXT_TLV(pTLV);

		/*
		** Fill in the part number TLV field
		*/
		memcpy(piecebuffer, pJedec->jedec_b_manuf_part_class, 18);
		build_tlv(pTLV, 1, piecebuffer, 18, pFru_desc_node);
		_NEXT_TLV(pTLV);

		/*
		** Fill in the serial number TLV field
		*/
		JedecSerialNum(pJedec, b);
		strncpy(piecebuffer, b, 10);
		build_tlv(pTLV, 1, piecebuffer, 10, pFru_desc_node);

		_NEXT_TLV(pTLV);

		/*
		** Fill in the firmware revision TLV field
		*/
		build_tlv(pTLV, 1, fw_rev, 9, pFru_desc_node);
	}
#endif

	/* 
	** Pad the size of the descriptor to the nearest quadword 
	*/
    pFru_desc_node->hd_fru_desc.size = ((pFru_desc_node->hd_fru_desc.size + 63) & ~63);

	/* 
	** Update fields in configuration tree header so that the next create has 
	** the proper offset.
	*/
	pRoot->available -= pFru_desc_node->hd_fru_desc.size;
	pRoot->first_free = (pRoot->first_free - orig_size) + pFru_desc_node->hd_fru_desc.size;


	/* 
	** Build an FRU node for each CPU in the system 
	*/
	for (i = 0; i < MAX_PROCESSOR_ID; i++)
	{
		if (cpu_mask & (1 << i))
		{
			if ((status = build_cpu_fru(hw_root, node, i)) != SUCCESS)
				return(status);
		}
	}

	/* 
	** Build an FRU node for each PCI option in the system 
	*/
    for (h = 0; h <= io_get_max_hose(); h++)
	{
		for (i = 0;  i < 31; i++)
		{
			for (j = 0;  j <= 31;  j++)
			{
				for (k = 0;  k <= 7;  k++)
				{
	    			if ((pb = get_matching_pb(h, i, j, k, 0)) != (struct pb *)0)
					{
						if (pb->type == TYPE_PCI)
						{
							/*
							** Do not build a FRU node for the ISA I/O controller etc on the motherboard.
							** the on-board SCSI controller on hose 0, bus 0, slot 6.
							*/
							if (
								(pb->bus != 0) ||
								(pb->slot >= PCI_SLOT0_DEVICE_ID) &&
								(pb->slot <= PCI_SLOT3_DEVICE_ID)
							   )
							{
								if ((status = build_pci_fru(hw_root, node, pb)) != SUCCESS)
									return(status);
							}
						}
					}
				}
			}
		}
    }

	/*
	** Build a FRU node for each dimm
	*/
    if ((status = build_memory_fru(hw_root, node)) != SUCCESS)
		return(status);

    return(SUCCESS);
}


/*+
 * ============================================================================
 * = build_cpu_fru - Build a FRU descriptor for the specified CPU             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_cpu_fru(hw_root, fru_root, cpu_id); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *      hw_root  - a handle to the root node of the hardware branch
 *	fru_root - a handle to the root node of the FRU branch
 *	cpu_id   - a unique CPU identifier
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
*/
int build_cpu_fru(GCT_HANDLE hw_root, GCT_HANDLE fru_root, int cpu_id) 
{
	int i, orig_size;
	unsigned __int64 tmp = 0;
	GCT_ROOT_NODE *pRoot;
	GCT_NODE_ID_CPU *pNodeid_cpu;
	GCT_HANDLE node = 0, mnode = 0, hwnode = 0;
	GCT_CPU_NODE *pCpu_node;
	GCT_FRU_DESC_NODE *pFru_desc_node;
	GCT_HD_EXT *pHd_ext;
	GCT_TLV *pTLV;
	GCT_NODE_ID_FRU_DESC *pNodeid_fru_desc;
	char device[32];
	struct FILE	*fp = 0;
	JEDEC *pJedec;
	char buffer[sizeof(JEDEC)];
	char piecebuffer[16], b[10];
	GCT_SUBPACK *pSubpack;
	SUBPKT_SENSOR_FRU5 *pSensor;
	char sromrev[10];

	/* 
	** Get a pointer to the root of the configuration tree
	*/
	pRoot = (GCT_ROOT_NODE *)_GCT_MAKE_ADDRESS(0);

	/*
	** Create a temporary node ID for this CPU so we can locate its 
	** corresponding node in the hardware branch of the configuration tree.
	*/
    pNodeid_cpu = &tmp;
    pNodeid_cpu->node_id_cpu_cpu = cpu_id;
    pNodeid_cpu->node_id_cpu_rsrvd_16_23 = -1;
    pNodeid_cpu->node_id_cpu_hsbb = -1;
    pNodeid_cpu->node_id_cpu_rsrvd_56_63 = -1;

	/*
	 ** Find a handle to the specified CPU node in the hardware branch of the 
	 ** configuration tree and make a pointer to it.
	 */
    hwnode = gct__lookup_node(hw_root, hw_root, (FIND_BY_TYPE | FIND_BY_ID), NODE_CPU, 0, tmp, 0, 0);
    pCpu_node = _GCT_MAKE_ADDRESS(hwnode);

	/* 
	 ** Create a handle for this CPU's FRU descriptor and insert it into the FRU 
	 ** branch of the configuration tree as the last sibling of the FRU root's children. 
	 */
    mnode = (gct_node_info[NODE_FRU_DESC].create_node)(NODE_FRU_DESC, 0, 0, 0, 0);
    gct__insert_node(fru_root, mnode, GCT__INSERT_CHILD_LAST_SIB);
#if DEBUG
    printf("Creating CPU %d FRU child %x for HW node %x\n", cpu_id, mnode, hwnode);
#endif

	/* 
	** Make a pointer to the FRU descriptor from the handle and initialize it.
	*/
    pFru_desc_node = (GCT_FRU_DESC_NODE *)_GCT_MAKE_ADDRESS(mnode);
    pFru_desc_node->hd_fru_desc.depth = 3;

	/* 
	** Update the header extension of this CPU's hardware node to point to the 
	** to newly created FRU descriptor node. 
	*/
    pHd_ext = (int)pCpu_node + pCpu_node->hd_cpu.hd_extension;
    pHd_ext->fru = mnode;

	/*
	** Fill in the FRU descriptor node id which acts as a physical locator.
	*/
    pNodeid_fru_desc = &pFru_desc_node->hd_fru_desc.id;
    pNodeid_fru_desc->node_id_fru_desc_site_loc = 0xFF;
    pNodeid_fru_desc->node_id_fru_desc_cab_id = 0x00;
    pNodeid_fru_desc->node_id_fru_desc_position = 0xFF;
    pNodeid_fru_desc->node_id_fru_desc_chassis = 0x00;
    pNodeid_fru_desc->node_id_fru_desc_assembly = 0x00;
    pNodeid_fru_desc->node_id_fru_desc_subassembly = (cpu_id + 1);

	/*
	** Assign a platform-specific identifier code to provide the final level of 
	** granularity required to resolve this individual FRU's physical location in 
	** the system.
	**
	** For the CPU daughter cards, this is the IIC offset where the FRU EEPROM
	** is located.
	*/
    pNodeid_fru_desc->node_id_fru_desc_slot = 0x00A4 + (cpu_id * 8);

	/* 
	** Save the original size of the FRU descriptor before adding to it 
	*/
    orig_size = pFru_desc_node->hd_fru_desc.size;

	/* 
	** Get the starting address of the first TLV 
	*/
    pTLV = &pFru_desc_node->fru_desc_info.fru_info.tlv_tag;

	/*
	 ** Just fill in some nice values for the Jedec, one day we can read it for real.
	 */
    pJedec = &buffer;
	memset(buffer,'\0',sizeof(buffer));
	memcpy(pJedec->jedec_b_manuf_spec_alias, "Alpha Processor", sizeof(pJedec->jedec_b_manuf_spec_alias));
	memcpy(pJedec->jedec_b_manuf_spec_model, "Slot B Module", sizeof(pJedec->jedec_b_manuf_spec_model));
	memcpy(pJedec->jedec_b_manuf_part_class, "API Slot B Module", sizeof(pJedec->jedec_b_manuf_part_class));

	/*
	 ** Fill in the manufacturer TLV field
	 */
	memcpy(piecebuffer, pJedec->jedec_b_manuf_spec_alias, 10);
	build_tlv(pTLV, 1, piecebuffer, 10, pFru_desc_node);
	_NEXT_TLV(pTLV);

	/*
	 ** Fill in the model number TLV field
	 */
	memcpy(piecebuffer, pJedec->jedec_b_manuf_spec_model, 10);
	build_tlv(pTLV, 1, piecebuffer, 10, pFru_desc_node);
	_NEXT_TLV(pTLV);

	/*
	 ** Fill in the part number TLV field
	 */
	memcpy(piecebuffer, pJedec->jedec_b_manuf_part_class, 18);
	build_tlv(pTLV, 1, piecebuffer, 18, pFru_desc_node);
	_NEXT_TLV(pTLV);

	/*
	 ** Fill in the serial number TLV field
	 */
	JedecSerialNum(pJedec, b);
	strncpy(piecebuffer, b, 10);
	build_tlv(pTLV, 1, piecebuffer, 10, pFru_desc_node);
	_NEXT_TLV(pTLV);

	/*
	 ** Fill in the (SROM)firmware revision TLV field
	 */
	get_srom_revision(&sromrev, cpu_id);
	build_tlv(pTLV, 1, sromrev, 10, pFru_desc_node);

	/* 
	** Pad the size of the descriptor to the nearest quadword 
	*/
    pFru_desc_node->hd_fru_desc.size = ((pFru_desc_node->hd_fru_desc.size + 63) & ~63);

	/* pFru_desc_node->hd_fru_desc.size -= gct_head; */

	/* 
	** Update fields in configuration tree header so that the next create has 
	** the proper offset.
	*/
    pRoot->available -= pFru_desc_node->hd_fru_desc.size;
    pRoot->first_free = (pRoot->first_free - orig_size) + pFru_desc_node->hd_fru_desc.size;

	/*
	** Get a pointer to the CPU HW node's system configuration resource subpackets
	*/
    pSubpack = (int)pCpu_node + pHd_ext->subpkt_offset;

	/*
	** Update the fan sensor subpacket of this CPU's HW node to
	** point to the newly created FRU descriptor.
	*/
    for (i = 0; i < pHd_ext->subpkt_count;  i++)
	{
    	if (pSubpack->subpack_type == SUBPACK_SENSOR)
		{
			pSensor = (SUBPKT_SENSOR_FRU5 *)pSubpack;

		    if (pSensor->sensor_class == SENSOR_CLASS_FAN)
			{
				/* 
				** Create a handle for the subpacket FRU descriptor and insert it into the FRU 
				** branch of the configuration tree as the last sibling of the CPU's children. 
				*/
				node = (gct_node_info[NODE_FRU_DESC].create_node)(NODE_FRU_DESC, 0, 0, 0, 0);
				gct__insert_node(mnode, node, GCT__INSERT_CHILD_LAST_SIB);

				/* 
				** Make a pointer to the FRU descriptor from the handle and initialize it.
				*/
				pFru_desc_node = (GCT_FRU_DESC_NODE *)_GCT_MAKE_ADDRESS(node);
				pFru_desc_node->hd_fru_desc.depth = 4;

				/*
				** Update the sensor subpacket to point to the newly created FRU descriptor
				*/
				pSensor->fru_count = 1;
				pSensor->fru_offset = node;

				/* 
				** Save the original size of the FRU descriptor before adding to it 
				*/
				orig_size = pFru_desc_node->hd_fru_desc.size;

				/*
				** Fill in the FRU descriptor node id which acts as a physical locator.
				*/
				pNodeid_fru_desc = &pFru_desc_node->hd_fru_desc.id;
				pNodeid_fru_desc->node_id_fru_desc_site_loc = 0xFF;
				pNodeid_fru_desc->node_id_fru_desc_position = 0xFF;
				pNodeid_fru_desc->node_id_fru_desc_subassembly = 0x0B;
				pNodeid_fru_desc->node_id_fru_desc_slot = pSensor->console_id;

				/* 
				** Get the starting address of the first TLV 
				*/
				pTLV = &pFru_desc_node->fru_desc_info.fru_info.tlv_tag;

				/*
				** Fill in the manufacturer TLV field
				*/
				build_tlv(pTLV, 1, " ", 1, pFru_desc_node);
				_NEXT_TLV(pTLV);

				/*
				** Fill in the model number TLV field
				*/
				build_tlv(pTLV, 1, "CPU Fan", 7, pFru_desc_node);
				_NEXT_TLV(pTLV);

				/*
				** Fill in the part number TLV field
				*/
				build_tlv(pTLV, 1, " ", 1, pFru_desc_node);

				_NEXT_TLV(pTLV);
				/*
				** Fill in the serial number TLV field
				*/
				build_tlv(pTLV, 1, "-", 1, pFru_desc_node);

				/*
				** Fill in the firmware revision field
				*/
				_NEXT_TLV(pTLV);
				build_tlv(pTLV, 1, "-", 1, pFru_desc_node);

				/* 
				** Pad the size of the descriptor to the nearest quadword 
				*/
				pFru_desc_node->hd_fru_desc.size = ((pFru_desc_node->hd_fru_desc.size + 63) & ~63);

				/* 
				** Update fields in configuration tree header so that the next create has 
				** the proper offset.
				*/
				pRoot->available -= pFru_desc_node->hd_fru_desc.size;
				pRoot->first_free = (pRoot->first_free - orig_size) + pFru_desc_node->hd_fru_desc.size;
			}
		}

		/*
		** Update pointer to the next subpack 
		*/
		pSubpack = (char *)pSubpack + pSubpack->subpack_length;
    }

    return(SUCCESS);
}


/*+
 * ============================================================================
 * = build_memory_fru - Build a FRU descriptor for the specified Memory array =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_memory_fru(hw_root, fru_root); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *      hw_root   - a handle to the root node of the hardware branch
 *	fru_root  - a handle to the root node of the FRU branch
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_memory_fru(GCT_HANDLE hw_root, GCT_HANDLE fru_root) 
{
	int i, j, orig_size, array_size, sptr, eptr;
	unsigned int first = 1;
	unsigned __int64 tmp = 0;
	GCT_ROOT_NODE *pRoot;
	GCT_NODE_ID_MEM_CTRL *pNodeid_mem_ctrl;
	GCT_HANDLE hwnode = 0, mnode = 0, node = 0;
	GCT_MEMORY_CTRL_NODE *pMemctrl_node;
	GCT_FRU_DESC_NODE *pFru_desc_node;
	GCT_NODE_ID_FRU_DESC *pNodeid_fru_desc;
	SUBPKT_MEMORY_FRU5 *pSubpkt;    
	GCT_TLV *pTLV;
	GCT_HD_EXT *pHd_ext, *prevHd_ext;
	unsigned char dimm_pos_num[MAX_MEMORY_ARRAY][DIMMS_PER_ARRAY] = {
	{0x00},{ 0x01},{ 0x02}
	};
	unsigned char dimm_slot_num[MAX_MEMORY_ARRAY][DIMMS_PER_ARRAY] = {
	{0x00},{ 0x01},{ 0x02}
	};
    char buffer[32];

	/* 
	 ** Get a pointer to the root of the configuration tree
	 */
	pRoot = (GCT_ROOT_NODE *)_GCT_MAKE_ADDRESS(0);

	/*
	** Create a temporary node ID for this Memory Controller so we can locate its 
	** corresponding node in the hardware branch of the configuration tree.
	*/
	pNodeid_mem_ctrl = &tmp;
	pNodeid_mem_ctrl->node_id_mem_ctrl_memctrl = 0;
	pNodeid_mem_ctrl->node_id_mem_ctrl_rsrvd_24_31 = -1;
	pNodeid_mem_ctrl->node_id_mem_ctrl_hsbb = -1;
	pNodeid_mem_ctrl->node_id_mem_ctrl_rsrvd_56_63 = -1;

	/*
	** Find a handle to the specified Memory Controller node in the hardware branch of the 
	** configuration tree and make a pointer to it.
	*/
	hwnode = gct__lookup_node(hw_root, hw_root, (FIND_BY_TYPE | FIND_BY_ID), NODE_MEMORY_CTRL, 0, tmp, 0, 0);
	pMemctrl_node = _GCT_MAKE_ADDRESS(hwnode);

    pSubpkt = &pMemctrl_node->hd_memory_ctrl.magic + 1;

    for (i = 0;  i < MAX_MEMORY_ARRAY;  i++)
	{
		prevHd_ext = NULL;
    	
		if ((array_size = (get_array_size(i) / (1024 * 1024))) != 0)
		{
			for (j = 0;  j < DIMMS_PER_ARRAY;  j++)
			{
				/* 
				** Create a handle for a FRU descriptor that describes the first DIMM in the memory subsystem
				** and insert it into the FRU branch of the configuration tree as the last sibling of the 
				** FRU root's children. 
				*/
		    	if (j == 0)
				{
					mnode = (gct_node_info[NODE_FRU_DESC].create_node)(NODE_FRU_DESC, 0, 0, 0, 0);
					gct__insert_node(fru_root, mnode, GCT__INSERT_CHILD_LAST_SIB);

					/* 
					** Make a pointer to the FRU descriptor from the handle and initialize it.
					*/
					pFru_desc_node = (GCT_FRU_DESC_NODE *)_GCT_MAKE_ADDRESS(mnode);
					pFru_desc_node->hd_fru_desc.depth = 3;

					/* 
					** Update this Memory Array's resource configuration subpacket to point to
					** the newly created FRU descriptor node. 
					*/
					pSubpkt++;
				}
				/* 
				** Create handles for the FRU descriptors that describe the remaining DIMMs in this Memory 
				** Array and insert it into the FRU branch of the configuration tree as the last sibling of
				** the first Memory DIMM in the Array. 
				*/
				else
				{
					node = (gct_node_info[NODE_FRU_DESC].create_node)(NODE_FRU_DESC, 0, 0, 0, 0);
					gct__insert_node(mnode, node, GCT__INSERT_LAST_SIBLING);

					/* 
					** Make a pointer to the FRU descriptor from the handle and initialize it.
					*/
					pFru_desc_node = (GCT_FRU_DESC_NODE *)_GCT_MAKE_ADDRESS(node);
					pFru_desc_node->hd_fru_desc.depth = 3;
				}

				/*
				** Fill in the FRU descriptor node id which acts as a physical locator.
				*/
				pNodeid_fru_desc = &pFru_desc_node->hd_fru_desc.id;
				pNodeid_fru_desc->node_id_fru_desc_site_loc = 0xff;
				pNodeid_fru_desc->node_id_fru_desc_position = dimm_pos_num[i][j];
				pNodeid_fru_desc->node_id_fru_desc_subassembly = 0xff;
				pNodeid_fru_desc->node_id_fru_desc_slot = dimm_slot_num[i][j];

				/* 
				** Save the original size of the FRU descriptor before adding to it 
				*/
				orig_size = pFru_desc_node->hd_fru_desc.size;

				/* 
				** Get the starting address of the first TLV 
				*/
				pTLV = &pFru_desc_node->fru_desc_info.fru_info.tlv_tag;

				/*
				** Fill in the manufacturer TLV field
				*/
				build_tlv(pTLV, 1, " ", 1, pFru_desc_node);
				_NEXT_TLV(pTLV);

				/*
				** Fill in the model number TLV field
				*/
				build_tlv(pTLV, 1, " ", 1, pFru_desc_node);
				_NEXT_TLV(pTLV);

				/*
				** Fill in the part number TLV field
				*/
				sprintf(buffer, "%dMB SDRAM", array_size);
				build_tlv(pTLV, 1, buffer, strlen(buffer), pFru_desc_node);
                _NEXT_TLV(pTLV);

				/*
				** Fill in the serial number TLV field
				*/
				build_tlv(pTLV, 1, " ", 1, pFru_desc_node);
				_NEXT_TLV(pTLV);

				/*
				** Fill in the firmware revision field
				*/
                build_tlv(pTLV, 1, "-", 1, pFru_desc_node);

                _NEXT_TLV(pTLV);

				if (prevHd_ext != NULL)
					prevHd_ext->fru = node;

				/*
				** Add a header extension to each memory DIMM FRU descriptor except the last one.
				** The code that walks the tree will know that it has reached a leaf node by the         
				** lack of a header extension.                                                           
				*/
				if (j < (DIMMS_PER_ARRAY-1))
				{
					pHd_ext = (int)pTLV + _ROUND_TLV_LENGTH(pTLV->tlv_length);
					pFru_desc_node->hd_fru_desc.size += sizeof(GCT_HD_EXT);

					pHd_ext->fru_count = (DIMMS_PER_ARRAY - j) - 1;

					eptr = &pHd_ext->fru_count;
					sptr = &pFru_desc_node->hd_fru_desc.type;
					pFru_desc_node->hd_fru_desc.hd_extension = eptr - sptr;
					prevHd_ext = pHd_ext;		
				}

				/* 
				** Pad the size of the descriptor to the nearest quadword 
				*/
				pFru_desc_node->hd_fru_desc.size = ((pFru_desc_node->hd_fru_desc.size + 63) & ~63);

				/*	pFru_desc_node->hd_fru_desc.size -= gct_head; */

				/* 
				** Update fields in configuration tree header so that the next create has 
				** the proper offset.
				*/
				pRoot->available -= pFru_desc_node->hd_fru_desc.size;
				pRoot->first_free = (pRoot->first_free - orig_size) + pFru_desc_node->hd_fru_desc.size;
		    }
		}
    }

    return(SUCCESS);
}


/*+
 * ============================================================================
 * = build_pci_fru - Build a FRU descriptor for the specified PCI option slot =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_pci_fru(hw_root, fru_root, pb); 
 *
 * RETURNS:
 *
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *      hw_root  - a handle to the root node of the hardware branch
 *	fru_root - a handle to the root node of the FRU branch
 *	pb       - a pointer to a port block data structure
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int build_pci_fru(GCT_HANDLE hw_root, GCT_HANDLE fru_root, struct pb *pb)
{
    unsigned int status, orig_size;
    GCT_ROOT_NODE *pRoot;
    unsigned __int64 tmp = 0;
    GCT_HANDLE node = 0, hwnode = 0;
    GCT_IO_CTRL_NODE *pIO_ctrl_node;
    GCT_FRU_DESC_NODE *pFru_desc_node;
    GCT_HD_EXT *pHd_ext;
    GCT_NODE_ID_IO_CTRL *pNodeid_ioctrl;
    GCT_NODE_ID_FRU_DESC *pNodeid_fru_desc;
    GCT_TLV *pTLV;
    char name[21] = {0,0};

	/* 
	** Get a pointer to the root of the configuration tree
	*/
	pRoot = (GCT_ROOT_NODE *)_GCT_MAKE_ADDRESS(0);

	/*
	** Create a temporary node ID for this I/O controller so we can locate 
	** its  corresponding node in the hardware branch of the configuration 
	** tree.
	*/
	pNodeid_ioctrl = &tmp;
	pNodeid_ioctrl->node_id_io_ctrl_ctrlr = pb->slot;
	pNodeid_ioctrl->node_id_io_ctrl_bus = pb->bus;
	pNodeid_ioctrl->node_id_io_ctrl_hose = pb->hose;
	pNodeid_ioctrl->node_id_io_ctrl_iop = pb->hose;
	pNodeid_ioctrl->node_id_io_ctrl_hsbb = -1;
	pNodeid_ioctrl->node_id_io_ctrl_rsrvd_56_63 = -1;

	/*
	** Find a handle to the specified I/O controller node in the hardware 
	** branch of the configuration tree and make a pointer to it.
	*/
	hwnode = gct__lookup_node(hw_root, hw_root, (FIND_BY_TYPE | FIND_BY_ID), NODE_IO_CTRL, 0, tmp, 0, 0);
	pIO_ctrl_node = _GCT_MAKE_ADDRESS(hwnode);

	/* 
	** Create a handle for this I/O controller's FRU descriptor and insert it into the FRU 
	** branch of the configuration tree as the last sibling of the FRU root's children. 
	*/
	node = (gct_node_info[NODE_FRU_DESC].create_node)(NODE_FRU_DESC, 0, 0, 0, 0);
	gct__insert_node(fru_root, node, GCT__INSERT_CHILD_LAST_SIB);

#if DEBUG
	printf("Creating hose %d, bus %d, slot %d, function %d %s PCI FRU child %x for HW node %x %lx\n", 
		   pb->hose, pb->bus, pb->slot, pb->function, pb->hardware_name, node, hwnode, tmp);
#endif

	/* 
	** Make a pointer to the FRU descriptor from the handle and initialize it.
	*/
	pFru_desc_node = (GCT_FRU_DESC_NODE *)_GCT_MAKE_ADDRESS(node);
	pFru_desc_node->hd_fru_desc.depth = 3;

	/* 
	** Update the header extension of this I/O controller's hardware node to 
	** point to the to newly created FRU descriptor node. 
	*/
	pHd_ext = (int)pIO_ctrl_node + pIO_ctrl_node->hd_io_ctrl.hd_extension;
	pHd_ext->fru = node;

	/*
	** Fill in the FRU descriptor node id which acts as a physical locator.
	*/
	pNodeid_fru_desc = &pFru_desc_node->hd_fru_desc.id;
	pNodeid_fru_desc->node_id_fru_desc_site_loc = 0xff;
	pNodeid_fru_desc->node_id_fru_desc_cab_id = 0;

    if (pb->hose)
	{
		pNodeid_fru_desc->node_id_fru_desc_position = 0xb8;
    }
    else
	{
		pNodeid_fru_desc->node_id_fru_desc_position = 0xbb;
    }

    pNodeid_fru_desc->node_id_fru_desc_chassis = 0;
    pNodeid_fru_desc->node_id_fru_desc_assembly = 0;
    pNodeid_fru_desc->node_id_fru_desc_subassembly = 0;
    pNodeid_fru_desc->node_id_fru_desc_slot = pb->slot;

	/* 
	** Save the original size of the FRU descriptor before adding to it 
	*/
	orig_size = pFru_desc_node->hd_fru_desc.size;

	/* 
	** Get the starting address of the first TLV 
	*/
	pTLV = &pFru_desc_node->fru_desc_info.fru_info.tlv_tag;

	/*
	** Fill in the manufacturer TLV field
	*/
	build_tlv(pTLV, 1, " ", 1, pFru_desc_node);
	_NEXT_TLV(pTLV);

	/*
	** Fill in the model number TLV field
	*/
	build_tlv(pTLV, 1, " ", 1, pFru_desc_node);
	_NEXT_TLV(pTLV);

	sprintf(name, "%s", pb->hardware_name);
	build_tlv(pTLV, 1, name, sizeof(name), pFru_desc_node);
	_NEXT_TLV(pTLV);

	/*
	** Fill in the part number TLV field
	*/
	build_tlv(pTLV, 1, " ", 1, pFru_desc_node);
	_NEXT_TLV(pTLV);

	/*
	** Fill in the serial number TLV field
	*/
	build_tlv(pTLV, 1, " ", 1, pFru_desc_node);

	/* 
	** Pad the size of the descriptor to the nearest quadword 
	*/
	pFru_desc_node->hd_fru_desc.size = ((pFru_desc_node->hd_fru_desc.size + 63) & ~63);

	/* 
	** Update fields in configuration tree header so that the next create has 
	** the proper offset.
	*/                           
	pRoot->available -= pFru_desc_node->hd_fru_desc.size;
	pRoot->first_free = (pRoot->first_free - orig_size) + pFru_desc_node->hd_fru_desc.size;

	return(SUCCESS);
}


/*+
 * ============================================================================
 * = build_power_fru - Build a FRU descriptor for the Power/Environ. monitor  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *	build_power_fru(hw_root, fru_root); 
 *                                                                 
 * RETURNS:
 *                                                                 
 *      1 = SUCCESS
 *	0 = FAILED
 *                          
 * ARGUMENTS:
 *
 *      hw_root  - a handle to the root node of the hardware branch
 *	fru_root - a handle to the root node of the FRU branch
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *                                                          
-*/
int build_power_fru(GCT_HANDLE hw_root, GCT_HANDLE fru_root)    
{
    int i, orig_size;
    unsigned __int64 tmp = 0;
    GCT_ROOT_NODE *pRoot;
    GCT_NODE_ID_POWER_ENVIR5 *pNodeid_power;
    GCT_HANDLE hwnode = 0, node = 0;
    GCT_POWER_ENVIR_NODE *pPower_node;
    GCT_FRU_DESC_NODE *pFru_desc_node;
    GCT_NODE_ID_FRU_DESC *pNodeid_fru_desc;
    GCT_HD_EXT *pHD_ext;
    GCT_SUBPACK *pSubpack;
    SUBPKT_SENSOR_FRU5 *pSensor;
    GCT_TLV *pTLV;
    struct FILE *fp;

/* 
 ** Get a pointer to the root of the configuration tree
 */
    pRoot = (GCT_ROOT_NODE *)_GCT_MAKE_ADDRESS(0);
/*
 ** Create a temporary node ID for the Power/Environmental monitor so we can locate its 
 ** corresponding node in the hardware branch of the configuration tree.
 */
    pNodeid_power = &tmp;
    pNodeid_power->node_id_power_envir_pe_num = 0;
    pNodeid_power->node_id_power_rsrvd_16_23 = -1;
    pNodeid_power->node_id_power_envir_hsbb = -1;
/*
 ** Find a handle to the specified Power/Environmental node in the hardware branch of the 
 ** configuration tree and make a pointer to it.
 */
    hwnode = gct__lookup_node(hw_root, hw_root, (FIND_BY_TYPE | FIND_BY_ID), NODE_POWER_ENVIR, 0, tmp, 0, 0);
    pPower_node = _GCT_MAKE_ADDRESS(hwnode);
/* 
 ** Create a handle for the Server Features FRU descriptor and insert it into the FRU 
 ** branch of the configuration tree as the last sibling of the FRU root's children. 
 */
    node = (gct_node_info[NODE_FRU_DESC].create_node)(NODE_FRU_DESC, 0, 0, 0, 0);
    gct__insert_node(fru_root, node, GCT__INSERT_CHILD_LAST_SIB);
/* 
 ** Make a pointer to the FRU descriptor from the handle and initialize it.
 */
    pFru_desc_node = (GCT_FRU_DESC_NODE *)_GCT_MAKE_ADDRESS(node);
    pFru_desc_node->hd_fru_desc.depth = 2;
/* 
 ** Update the header extension of the Power/Environmental hardware node to point
 ** to the newly created FRU descriptor.
 */
    pHD_ext = (int)pPower_node + pPower_node->hd_power_envir.hd_extension;
    pHD_ext->fru = node;
/*
 ** Fill in the FRU descriptor node id which acts as a physical locator.
 */
    pNodeid_fru_desc = &pFru_desc_node->hd_fru_desc.id;
    pNodeid_fru_desc->node_id_fru_desc_site_loc = 0xFF;
    pNodeid_fru_desc->node_id_fru_desc_position = 0xFF;
    pNodeid_fru_desc->node_id_fru_desc_subassembly = 0xFF;
/*
 ** Assign a platform-specific identifier code to provide the final level of 
 ** granularity required to resolve this individual FRU's physical location in 
 ** the system.
 */
    pNodeid_fru_desc->node_id_fru_desc_slot = 0x00;
/* 
 ** Save the original size of the FRU descriptor before adding to it 
 */
    orig_size = pFru_desc_node->hd_fru_desc.size;
/* 
 ** Get the starting address of the first TLV 
 */
    pTLV = &pFru_desc_node->fru_desc_info.fru_info.tlv_tag;
/*
 ** Fill in the manufacturer TLV field
 */                                    
    build_tlv(pTLV, 1, "Compaq", 6, pFru_desc_node);
    _NEXT_TLV(pTLV);
/*
 ** Fill in the model number TLV field
 */
    build_tlv(pTLV, 1, "SvrMgmt", 7, pFru_desc_node);
    _NEXT_TLV(pTLV);
/*
 ** Fill in the part number TLV field
 */
    if (fp = fopen("iic_8574_ocp", "sr+")) /* Goldrack */ 
       build_tlv(pTLV, 1, "54-30358-01", 11, pFru_desc_node);
    else  /* Goldrush */                                     
       build_tlv(pTLV, 1, "54-25580-01", 11, pFru_desc_node);
    fclose(fp);                                                            

    _NEXT_TLV(pTLV);
/*
 ** Fill in the serial number TLV field
 */
    build_tlv(pTLV, 1, "-", 1, pFru_desc_node);

/*
 ** Fill in the firmware revision field
 */
    _NEXT_TLV(pTLV);
    build_tlv(pTLV, 1, "-", 1, pFru_desc_node);

/* 
 ** Pad the size of the descriptor to the nearest quadword 
 */
    pFru_desc_node->hd_fru_desc.size = ((pFru_desc_node->hd_fru_desc.size + 63) & ~63);

/*    pFru_desc_node->hd_fru_desc.size -= gct_head;*/

/* 
 ** Update fields in configuration tree header so that the next create has 
 ** the proper offset.
 */
    pRoot->available -= pFru_desc_node->hd_fru_desc.size;
    pRoot->first_free = (pRoot->first_free - orig_size) + pFru_desc_node->hd_fru_desc.size;
/*
 ** Get a pointer to the Power/Environmental HW node's system configuration resource
 ** subpackets                                                      
 */
    pSubpack = &pPower_node->hd_power_envir.type + pHD_ext->subpkt_offset;
                                               
    for (i = 0;  i < pHD_ext->subpkt_count;  i++) {
	pSensor = (SUBPKT_SENSOR_FRU5 *)pSubpack;
/*
 ** Do not create a FRU descriptor for the temperature sensor
 */
	if (pSensor->sensor_class == SENSOR_CLASS_TEMP)
	    continue;
/* 
 ** Create a handle for the subpacket FRU descriptor and insert it into the FRU 
 ** branch of the configuration tree as the last sibling of the FRU root's children. 
 */
	node = (gct_node_info[NODE_FRU_DESC].create_node)(NODE_FRU_DESC, 0, 0, 0, 0);
	gct__insert_node(fru_root, node, GCT__INSERT_CHILD_LAST_SIB);
/* 
 ** Make a pointer to the FRU descriptor from the handle and initialize it.
 */
	pFru_desc_node = (GCT_FRU_DESC_NODE *)_GCT_MAKE_ADDRESS(node);
	pFru_desc_node->hd_fru_desc.depth = 2;
/*
 ** Update the sensor subpacket to point to the newly created FRU descriptor
 */
	pSensor->fru_count = 1;
	pSensor->fru_offset = node;
/* 
 ** Save the original size of the FRU descriptor before adding to it 
 */
	orig_size = pFru_desc_node->hd_fru_desc.size;

	switch (pSensor->sensor_class) {
	    case SENSOR_CLASS_FAN:
/*
 ** Fill in the FRU descriptor node id which acts as a physical locator.
 */
		pNodeid_fru_desc = &pFru_desc_node->hd_fru_desc.id;
		pNodeid_fru_desc->node_id_fru_desc_site_loc = 0xFF;
		pNodeid_fru_desc->node_id_fru_desc_position = 0xFF;
		pNodeid_fru_desc->node_id_fru_desc_subassembly = 0xFF;
		pNodeid_fru_desc->node_id_fru_desc_slot = pSensor->console_id;
/* 
 ** Get the starting address of the first TLV 
 */
		pTLV = &pFru_desc_node->fru_desc_info.fru_info.tlv_tag;
/*
 ** Fill in the manufacturer TLV field
 */
		build_tlv(pTLV, 1, "Compaq", 6, pFru_desc_node);
		_NEXT_TLV(pTLV);
/*
 ** Fill in the model number TLV field
 */
		build_tlv(pTLV, 1, "System Fan", 10, pFru_desc_node);
		_NEXT_TLV(pTLV);
/*                                                                        
 ** Fill in the part number TLV field
 */                              
		switch (pSensor->console_id) {
		    case 1:                                        
                        if (fp = fopen("iic_8574_ocp", "sr+")) /* Goldrack */ 
 	 	           build_tlv(pTLV, 1, "70-40142-01", 11, pFru_desc_node);
                        else  /* Goldrush */                                     
      			   build_tlv(pTLV, 1, "70-31351-01", 11, pFru_desc_node);            
                        fclose(fp);                                                            
      	                break;                                                                 
      		    case 2:                                                                    
                        if (fp = fopen("iic_8574_ocp", "sr+")) /* Goldrack */ 
      	 	           build_tlv(pTLV, 1, "70-40142-01", 11, pFru_desc_node);
                        else  /* Goldrush */                                     
      			   build_tlv(pTLV, 1, "70-31351-02", 11, pFru_desc_node);
                        fclose(fp);
      			break;   
		    default:
			build_tlv(pTLV, 1, " ", 1, pFru_desc_node);
			break;
		}
		_NEXT_TLV(pTLV);
/*
 ** Fill in the serial number TLV field
 */
		build_tlv(pTLV, 1, "-", 1, pFru_desc_node);

/*
 ** Fill in the firmware revision field
 */
               _NEXT_TLV(pTLV);
                build_tlv(pTLV, 1, "-", 1, pFru_desc_node);

	    	break;

	    case SENSOR_CLASS_DCPWR:
/*
 ** Fill in the FRU descriptor node id which acts as a physical locator.
 */
		pNodeid_fru_desc = &pFru_desc_node->hd_fru_desc.id;
		pNodeid_fru_desc->node_id_fru_desc_site_loc = 0xFF;
		pNodeid_fru_desc->node_id_fru_desc_position = 0xFF;
		pNodeid_fru_desc->node_id_fru_desc_subassembly = 0x0A;
		pNodeid_fru_desc->node_id_fru_desc_slot = pSensor->console_id;
/* 
 ** Get the starting address of the first TLV 
 */
		pTLV = &pFru_desc_node->fru_desc_info.fru_info.tlv_tag;
/*
 ** Fill in the manufacturer TLV field
 */
		build_tlv(pTLV, 1, "Compaq", 6, pFru_desc_node);
		_NEXT_TLV(pTLV); 
/*
 ** Fill in the model number TLV field
 */
		build_tlv(pTLV, 1, "Power", 5, pFru_desc_node);
		_NEXT_TLV(pTLV);
/*
 ** Fill in the part number TLV field
 */
                if (fp = fopen("iic_8574_ocp", "sr+")) /* Goldrack */
	 	   build_tlv(pTLV, 1, "30-50662-01", 11, pFru_desc_node);
                else  /* Goldrush */                                     
	 	   build_tlv(pTLV, 1, "H7899-AA", 8, pFru_desc_node);
                fclose(fp);                                

		_NEXT_TLV(pTLV);
/*
 ** Fill in the serial number TLV field
 */
		build_tlv(pTLV, 1, "-", 1, pFru_desc_node);

/*
 ** Fill in the firmware revision field
 */
               _NEXT_TLV(pTLV);
                build_tlv(pTLV, 1, "-", 1, pFru_desc_node);

	    	break;
	}
/* 
 ** Pad the size of the descriptor to the nearest quadword 
 */
	pFru_desc_node->hd_fru_desc.size = ((pFru_desc_node->hd_fru_desc.size + 63) & ~63);
/* 
 ** Update fields in configuration tree header so that the next create has 
 ** the proper offset.
 */
	pRoot->available -= pFru_desc_node->hd_fru_desc.size;
	pRoot->first_free = (pRoot->first_free - orig_size) + pFru_desc_node->hd_fru_desc.size;
/*
 ** Update pointer to the next subpack 
 */
	pSubpack = (char *)pSubpack + pSubpack->subpack_length;
    }
    return(SUCCESS);
}


/*
 ** BUILD_SW_ROOT
 */                             
int build_sw_root(GCT_HANDLE sw_root)
{
    GCT_ROOT_NODE *pRoot;
    GCT_HANDLE node = 0, hwnode = 0;
    GCT_SW_ROOT_NODE *pSW_root;
    GCT_NODE_ID_SW_ROOT *pSw_root_id;
    GCT_COMMUNITY_NODE *pCommunity;  
    GCT_PARTITION_NODE *pPartition;
    GCT_TLV *pTLV;
    GCT_HD_EXT *pHd_ext;
    int sptr, eptr, orig_size;
    unsigned char *pValue;

/* 
 ** Get a pointer to the root of the configuration tree
 */
    pRoot = (GCT_ROOT_NODE *)_GCT_MAKE_ADDRESS(0);
/* 
 ** Get a pointer to the root of the software branch of the configuration tree
 */
    pSW_root = (GCT_SW_ROOT_NODE *)_GCT_MAKE_ADDRESS(sw_root);
/*
 ** Fix up software root depth
 */
    pSW_root->hd_sw_root.depth = 1;
/*
 ** Initialize this node's ID field
 */
    pSw_root_id = &pSW_root->hd_sw_root.id;
    pSw_root_id->node_id_sw_root_rsrvd_08_15 = 0xFF;
    pSw_root_id->node_id_sw_root_rsrvd_16_31 = 0xFFFF;
    pSw_root_id->node_id_sw_root_rsrvd_32_63 = 0xFFFFFFFF; 

    hwnode = gct__lookup_node(sw_root, sw_root, FIND_BY_TYPE, NODE_COMMUNITY, 0, 0, 0, 0);
/* 
 ** Make a pointer to the community node from the handle and initialize it.
 */
    pCommunity = _GCT_MAKE_ADDRESS(hwnode);
    pCommunity->hd_community.depth = 2;
    pCommunity->hd_community.owner = sw_root;
    pCommunity->hd_community.current_owner = sw_root;
    pCommunity->hd_community.config = sw_root;
    pCommunity->hd_community.affinity = sw_root;
/* 
 ** Create a handle for a software partition and insert it into the software
 ** branch of the configuration tree as the child of the community.
 */
    node = (gct_node_info[NODE_PARTITION].create_node)(NODE_PARTITION, 0, 0, 0, 0);
    gct__insert_node(hwnode, node, GCT__INSERT_CHILD);
/* 
 ** Make a pointer to the partition from the handle and initialize it.
 */
    pPartition = _GCT_MAKE_ADDRESS(node);
    pPartition->hd_partition.depth = 3;
/* 
 ** Save original partition header size before adding to it 
 */
    orig_size = pPartition->hd_partition.size;
/* 
 ** Initialize the console configuration subpacket 
 */
    pPartition->os_type = OS_VMS;
    pPartition->csl_res.csl_sub.subpack_length = sizeof(SUBPKT_CSL_FRU5) - 8;
    pPartition->csl_res.csl_sub.subpack_class = CLASS1;
    pPartition->csl_res.csl_sub.subpack_type = SUBPACK_CONSOLE;
    pPartition->csl_res.csl_sub.subpack_rev = 2;
    pPartition->csl_res.reset_reason = CSL_RESET_REASON_HARD_RESET;
    pPartition->csl_res.ev_count  = 0;
/*
 ** Fill in the console part number.
 */
    pTLV = &pPartition->csl_res.srm_console_part_number;
    pTLV->tlv_tag = TLV_TAG_ISOLATIN1;
    pTLV->tlv_length = strlen("SRM");
    pValue = (unsigned char *)&pTLV->tlv_value;
    strncpy((char *)pValue, "SRM", pTLV->tlv_length);
    pPartition->hd_partition.size = (pPartition->hd_partition.size + _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4);
    pPartition->csl_res.csl_sub.subpack_length += _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4;
    _NEXT_TLV(pTLV);
/*
 ** Fill in the console type revision string.
 */
    pTLV->tlv_tag = TLV_TAG_ISOLATIN1;
    pTLV->tlv_length = 9;
    pValue = (unsigned char *)&pTLV->tlv_value;
    strncpy((char *)pValue, fw_rev, pTLV->tlv_length);
    pPartition->hd_partition.size = (pPartition->hd_partition.size + _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4);
    pPartition->csl_res.csl_sub.subpack_length += _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4;
    _NEXT_TLV(pTLV);
/*
 ** Fill in the AlphaBIOS part number.
 */
    pTLV->tlv_tag = TLV_TAG_ISOLATIN1;
    pTLV->tlv_length = strlen("AlphaBIOS");
    pValue = (unsigned char *) &pTLV->tlv_value;
    strncpy((char *)pValue, "AlphaBIOS", pTLV->tlv_length);
    pPartition->hd_partition.size = (pPartition->hd_partition.size + _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4);
    pPartition->csl_res.csl_sub.subpack_length += _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4;
    _NEXT_TLV(pTLV);         
/*
 ** Leave AlphaBIOS field blank
 */
    pTLV->tlv_tag = TLV_TAG_ISOLATIN1;
    pTLV->tlv_length = strlen("arc_ver:xx.xx.xx.xxx");
    pValue = (unsigned char *)&pTLV->tlv_value;
    strncpy((char *)pValue, "arc_ver:xx.xx.xx.xxx", pTLV->tlv_length);
    pPartition->hd_partition.size = (pPartition->hd_partition.size + _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4);
    pPartition->csl_res.csl_sub.subpack_length += _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4;
    _NEXT_TLV(pTLV);
/* 
 ** Fill in the header extension information
 */
    pHd_ext = (int)pTLV;
    pHd_ext->subpkt_count  = 1;
    pHd_ext->subpkt_offset = offsetof(struct _gct_partition_node, csl_res);
/* 
 ** Add in the header extension size
 */
    eptr = &pHd_ext->fru_count;
    sptr = &pPartition->hd_partition.type;
    pPartition->hd_partition.hd_extension = eptr - sptr;
/* 
 ** Pad the header size to the nearest quadword 
 */
    pPartition->hd_partition.size = ((pPartition->hd_partition.size + 63) & ~63);
/* 
 ** Update fields in header so next node has proper offset 
 */
    pRoot->available -= pPartition->hd_partition.size;
    pRoot->first_free = (pRoot->first_free - orig_size) + pPartition->hd_partition.size;

    return(SUCCESS);
}

