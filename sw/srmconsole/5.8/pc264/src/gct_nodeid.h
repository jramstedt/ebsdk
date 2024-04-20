/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:05:08 by OpenVMS SDL EV1-31     */
/* Source:   8-NOV-1999 13:52:19 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]GCT_NODEID.S */
/********************************************************************************************************************************/
/* file:	gct_nodeid.sdl                                              */
/*                                                                          */
/* Copyright (C) 1998, by                                                   */
/* Digital Equipment Corporation, Maynard, Massachusetts.                   */
/* All rights reserved.                                                     */
/*                                                                          */
/* This software is furnished under a license and may be used and copied    */
/* only  in  accordance  of  the  terms  of  such  license  and with the    */
/* inclusion of the above copyright notice. This software or  any  other    */
/* copies thereof may not be provided or otherwise made available to any    */
/* other person.  No title to and  ownership of the  software is  hereby    */
/* transferred.                                                             */
/*                                                                          */
/* The information in this software is  subject to change without notice    */
/* and  should  not  be  construed  as a commitment by digital equipment    */
/* corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by digital.                  */
/*                                                                          */
/*                                                                          */
/* Abstract:	Galaxy Node ID specific definitions for Alpha firmware.     */
/*                                                                          */
/* Author:	Nigel J. Croxon                                             */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*   12-Nov-98  NJC - Flip all hardware ID 180 Degrees                      */
/*                                                                          */
/*** MODULE $gct_nodeid ***/
typedef struct _GCT_NODE_ID_ROOT {
    unsigned node_id_root : 8;
    unsigned node_id_root_rsrvd_08_15 : 8;
    unsigned node_id_root_rsrvd_16_31 : 16;
    unsigned node_id_root_rsrvd_32_63 : 32;
    } GCT_NODE_ID_ROOT;
typedef struct _GCT_NODE_ID_HW_ROOT {
    unsigned node_id_hw_root : 8;
    unsigned node_id_hw_root_rsrvd_08_15 : 8;
    unsigned node_id_hw_root_rsrvd_16_31 : 16;
    unsigned node_id_hw_root_rsrvd_32_63 : 32;
    } GCT_NODE_ID_HW_ROOT;
typedef struct _GCT_NODE_ID_SW_ROOT {
    unsigned node_id_sw_root : 8;
    unsigned node_id_sw_root_rsrvd_08_15 : 8;
    unsigned node_id_sw_root_rsrvd_16_31 : 16;
    unsigned node_id_sw_root_rsrvd_32_63 : 32;
    } GCT_NODE_ID_SW_ROOT;
typedef struct _GCT_NODE_ID_TEMPLATE_ROOT {
    unsigned node_id_template_root : 8;
    unsigned node_id_template_root_rsrvd_08_15 : 8;
    unsigned node_id_template_root_rsrvd_16_31 : 16;
    unsigned node_id_template_root_rsrvd_32_63 : 32;
    } GCT_NODE_ID_TEMPLATE_ROOT;
typedef struct _GCT_NODE_ID_SBB {
    unsigned node_id_sbb_rsrvd_0_31 : 32;
    unsigned node_id_sbb_hsbb : 8;
    unsigned node_id_sbb_rsrvd_40_47 : 8;
    unsigned node_id_sbb_sbb : 8;
    unsigned node_id_sbb_rsrvd_56_63 : 8;
    } GCT_NODE_ID_SBB;
typedef struct _GCT_NODE_ID_SMB {
    unsigned node_id_smb_rsrvd_0_31 : 32;
    unsigned node_id_smb_hsbb : 8;
    unsigned node_id_smb_smb : 8;
    unsigned node_id_smb_sbb : 8;
    unsigned node_id_smb_rsrvd_56_63 : 8;
    } GCT_NODE_ID_SMB;
typedef struct _GCT_NODE_ID_CPU {
    unsigned node_id_cpu_cpu : 16;
    unsigned node_id_cpu_rsrvd_16_23 : 8;
    unsigned node_id_cpu_revcnt : 8;
    unsigned node_id_cpu_hsbb : 8;
    unsigned node_id_cpu_smb : 8;
    unsigned node_id_cpu_sbb : 8;
    unsigned node_id_cpu_rsrvd_56_63 : 8;
    } GCT_NODE_ID_CPU;
typedef struct _GCT_NODE_ID_CPU_MODULE {
    unsigned node_id_cpu_module : 16;
    unsigned node_id_cpu_module_rsrvd_16_31 : 16;
    unsigned node_id_cpu_module_hsbb : 8;
    unsigned node_id_cpu_module_smb : 8;
    unsigned node_id_cpu_module_sbb : 8;
    unsigned node_id_cpu_module_rsrvd_56_63 : 8;
    } GCT_NODE_ID_CPU_MODULE;
typedef struct _GCT_NODE_ID_MEM_SUB {
    unsigned node_id_mem_sub_memsub : 8;
    unsigned node_id_mem_sub_rsrvd_8_31 : 24;
    unsigned node_id_mem_sub_hsbb : 8;
    unsigned node_id_mem_sub_smb : 8;
    unsigned node_id_mem_sub_sbb : 8;
    unsigned node_id_mem_sub_rsrvd_56_63 : 8;
    } GCT_NODE_ID_MEM_SUB;
typedef struct _GCT_NODE_ID_MEM_DESC {
    unsigned node_id_mem_desc_memdesc : 16;
    unsigned node_id_mem_desc_memsub : 8;
    unsigned node_id_mem_desc_rsrvd_24_31 : 8;
    unsigned node_id_mem_desc_hsbb : 8;
    unsigned node_id_mem_desc_smb : 8;
    unsigned node_id_mem_desc_sbb : 8;
    unsigned node_id_mem_desc_rsrvd_56_63 : 8;
    } GCT_NODE_ID_MEM_DESC;
typedef struct _GCT_NODE_ID_MEM_CTRL {
    unsigned node_id_mem_ctrl_memctrl : 16;
    unsigned node_id_mem_ctrl_memsub : 8;
    unsigned node_id_mem_ctrl_rsrvd_24_31 : 8;
    unsigned node_id_mem_ctrl_hsbb : 8;
    unsigned node_id_mem_ctrl_smb : 8;
    unsigned node_id_mem_ctrl_sbb : 8;
    unsigned node_id_mem_ctrl_rsrvd_56_63 : 8;
    } GCT_NODE_ID_MEM_CTRL;
typedef struct _GCT_NODE_ID_IOP {
    unsigned node_id_iop_iop : 8;
    unsigned node_id_iop_rsrvd_8_31 : 24;
    unsigned node_id_iop_hsbb : 8;
    unsigned node_id_iop_smb : 8;
    unsigned node_id_iop_sbb : 8;
    unsigned node_id_iop_rsrvd_56_63 : 8;
    } GCT_NODE_ID_IOP;
typedef struct _GCT_NODE_ID_HOSE {
    unsigned node_id_hose_hose : 8;
    unsigned node_id_hose_iop : 8;
    unsigned node_id_hose_rsrvd_16_31 : 16;
    unsigned node_id_hose_hsbb : 8;
    unsigned node_id_hose_smb : 8;
    unsigned node_id_hose_sbb : 8;
    unsigned node_id_hose_rsrvd_56_63 : 8;
    } GCT_NODE_ID_HOSE;
typedef struct _GCT_NODE_ID_BUS {
    unsigned node_id_bus_bus : 8;
    unsigned node_id_bus_hose : 8;
    unsigned node_id_bus_iop : 8;
    unsigned node_id_bus_rsrvd_24_31 : 8;
    unsigned node_id_bus_hsbb : 8;
    unsigned node_id_bus_smb : 8;
    unsigned node_id_bus_sbb : 8;
    unsigned node_id_bus_rsrvd_56_63 : 8;
    } GCT_NODE_ID_BUS;
typedef struct _GCT_NODE_ID_SLOT {
    unsigned node_id_slot_slot : 8;
    unsigned node_id_slot_bus : 8;
    unsigned node_id_slot_hose : 8;
    unsigned node_id_slot_iop : 8;
    unsigned node_id_slot_hsbb : 8;
    unsigned node_id_slot_smb : 8;
    unsigned node_id_slot_sbb : 8;
    unsigned node_id_slot_rsrvd_56_63 : 8;
    } GCT_NODE_ID_SLOT;
typedef struct _GCT_NODE_ID_IO_CTRL {
    unsigned node_id_io_ctrl_ctrlr : 8;
    unsigned node_id_io_ctrl_bus : 8;
    unsigned node_id_io_ctrl_hose : 8;
    unsigned node_id_io_ctrl_iop : 8;
    unsigned node_id_io_ctrl_hsbb : 8;
    unsigned node_id_io_ctrl_smb : 8;
    unsigned node_id_io_ctrl_sbb : 8;
    unsigned node_id_io_ctrl_rsrvd_56_63 : 8;
    } GCT_NODE_ID_IO_CTRL;
typedef struct _GCT_NODE_ID_POWER_ENVIR5 {
    unsigned node_id_power_envir_pe_num : 16;
    unsigned node_id_power_rsrvd_16_23 : 8;
    unsigned node_id_power_envir_hsbb : 8;
    unsigned node_id_power_envir_smb : 8;
    unsigned node_id_power_envir_sbb : 8;
    unsigned node_id_power_envir_chassis : 8;
    unsigned node_id_power_envir_cab : 8;
    } GCT_NODE_ID_POWER_ENVIR5;
typedef struct _GCT_NODE_ID_POWER_ENVIR {
    unsigned node_id_power_envir_pe_num : 16;
    unsigned node_id_power_rsrvd_16_23 : 8;
    unsigned node_id_power_envir_cab : 8;
    unsigned node_id_power_envir_hsbb : 8;
    unsigned node_id_power_envir_smb : 8;
    unsigned node_id_power_envir_sbb : 8;
    unsigned node_id_power_envir_chassis : 8;
    } GCT_NODE_ID_POWER_ENVIR;
typedef struct _GCT_NODE_ID_PSEUDO {
    unsigned node_id_pseudo_num : 16;
    unsigned node_id_pseudo_rsrvd_16_31 : 16;
    unsigned node_id_pseudo_rsrvd_32_47 : 16;
    unsigned node_id_pseudo_chassis : 8;
    unsigned node_id_pseudo_cab : 8;
    } GCT_NODE_ID_PSEUDO;
typedef struct _GCT_NODE_ID_FRU_DESC {
    unsigned node_id_fru_desc_site_loc : 8;
    unsigned node_id_fru_desc_cab_id : 8;
    unsigned node_id_fru_desc_position : 8;
    unsigned node_id_fru_desc_chassis : 8;
    unsigned node_id_fru_desc_assembly : 8;
    unsigned node_id_fru_desc_subassembly : 8;
    unsigned node_id_fru_desc_slot : 16;
    } GCT_NODE_ID_FRU_DESC;
typedef struct _GCT_NODE_ID_SYS_INTER_SWITCH {
    unsigned node_id_switch_id : 16;
    unsigned node_id_switch_rsrvd_16_31 : 16;
    unsigned node_id_switch_hsbb : 8;
    unsigned node_id_switch_rsrvd_40_63 : 24;
    } GCT_NODE_ID_SYS_INTER_SWITCH;
typedef struct _GCT_NODE_ID_PARTITION {
    unsigned node_id_part_part : 16;
    unsigned node_id_part_16_31 : 16;
    unsigned node_id_part_32_63 : 32;
    } GCT_NODE_ID_PARTITION;
typedef struct _GCT_NODE_ID_COMMUNITY {
    unsigned node_id_comm_id : 16;
    unsigned node_id_comm_16_31 : 16;
    unsigned node_id_comm_32_63 : 32;
    } GCT_NODE_ID_COMMUNITY;
typedef struct _GCT_NODE_ID_CAB {
    unsigned node_id_cab : 8;
    unsigned node_id_cab_08_15 : 8;
    unsigned node_id_cab_16_31 : 16;
    unsigned node_id_cab_32_63 : 32;
    } GCT_NODE_ID_CAB;
typedef struct _GCT_NODE_ID_CHASSIS {
    unsigned node_id_chassis : 8;
    unsigned node_id_chassis_08_15 : 8;
    unsigned node_id_chassis_16_31 : 16;
    unsigned node_id_chassis_32_63 : 32;
    } GCT_NODE_ID_CHASSIS;
typedef struct _GCT_NODE_ID_HARD_PART {
    unsigned node_id_hard_part : 16;
    unsigned node_id_hard_part_16_31 : 16;
    unsigned node_id_hard_part_32_63 : 32;
    } GCT_NODE_ID_HARD_PART;
typedef struct _GCT_NODE_ID_RISER {
    unsigned node_id_riser_num : 8;
    unsigned node_id_riser_iop : 8;
    unsigned node_id_riser_rsrvd_16_31 : 16;
    unsigned node_id_riser_hsbb : 8;
    unsigned node_id_riser_smb : 8;
    unsigned node_id_riser_sbb : 8;
    unsigned node_id_riser_rsrvd_56_63 : 8;
    } GCT_NODE_ID_RISER;
 
