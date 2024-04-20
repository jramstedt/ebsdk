/*
 * file:    dp264_fru.h
 *
 * Copyright (C) 1998 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
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
 *      This module contains global platform-specific definitions
 *	used by the JEDEC-like FRU IIC EEPROM access routines. 
 *                                        
 *  AUTHOR:
 *
 *      Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *      30-Nov-1998
 *
 *  MODIFICATION HISTORY:
 *
 *	ER	30-Nov-1998	Adapted from CLIPPER_FRU.H
 *
 *--
 */

#include "cp$src:common.h"

typedef struct _fru_name_list {
  unsigned __int64      fru_id;
           char         name[15];
           int          iic_support;
} FRU_NAME_LIST;


#if NAUTILUS || TINOSA || GLXYTRAIN
static FRU_NAME_LIST fru_names[] = {
 { 0x0000ff0000ff00ff, "SFM", 0},
 { 0x00A2ff0000ff00ff, "SMB", 1},
 { 0x00A4010000ff00ff, "SMB.CPU0", 1},
 { 0x0008000000bb00ff, "SMB.PCI0", 0},
 { 0x0009000000bb00ff, "SMB.PCI1", 0},
 { 0x000a000000bb00ff, "SMB.PCI2", 0},
 { 0x000b000000bb00ff, "SMB.PCI3", 0},
 { 0x0005000000bb00ff, "SMB.AGP0", 0},
 { 0x0000ff00000000ff, "SMB.DIMM0", 0},
 { 0x0001ff00000100ff, "SMB.DIMM1", 0},
 { 0x0002ff00000200ff, "SMB.DIMM2", 0},
 {0}
};
#else
typedef struct smb_platform {
    unsigned __int64 tig_rev;
    unsigned __int64 cchip_misc;
    unsigned __int64 dchip_rev;
    unsigned __int64 pchip0_ctrl;
    unsigned __int64 pchip1_ctrl;
} SMB_PLATFORM;
#define PLATFORM_SMB_EXT SMB_PLATFORM smb_platform;

typedef struct iop_pchip {
    unsigned __int64 pchip_pctl; 
    unsigned __int64 pchip_err_mask; 
    unsigned __int64 pchip_wsba0; 
    unsigned __int64 pchip_wsba1; 
    unsigned __int64 pchip_wsba2; 
    unsigned __int64 pchip_wsba3; 
    unsigned __int64 pchip_wsm0; 
    unsigned __int64 pchip_wsm1; 
    unsigned __int64 pchip_wsm2; 
    unsigned __int64 pchip_wsm3; 
    unsigned __int64 pchip_tba0; 
    unsigned __int64 pchip_tba1; 
    unsigned __int64 pchip_tba2; 
    unsigned __int64 pchip_tba3; 
} IOP_PCHIP;
#define PLATFORM_IO_EXTENSION IOP_PCHIP iop_pchip;

typedef struct mmb_ctrl {
    unsigned __int64 cchip_csc;
    unsigned __int64 cchip_aar;
    unsigned int fru_count;
    unsigned int fru;
} MMB_CTRL;
#define PLATFORM_MEM_FRU_EXT MMB_CTRL mmb_ctrl;

static FRU_NAME_LIST fru_names[] = {
 { 0x0000ff0000ff00ff, "SFM", 0},
 { 0x00000A0000ff00ff, "PWR0", 0},
 { 0x00010A0000ff00ff, "PWR1", 0},
 { 0x00020A0000ff00ff, "PWR2", 0},
 { 0x0001ff0000ff00ff, "FAN1", 0},
 { 0x0002ff0000ff00ff, "FAN2", 0},
 { 0x00A2ff0000ff00ff, "SMB0", 1},
 { 0x00A4010000ff00ff, "SMB0.CPU0", 1},
 { 0x00000B0000ff00ff, "SMB0.CPU0.FAN", 0},
 { 0x00AC020000ff00ff, "SMB0.CPU1", 1},
 { 0x00010B0000ff00ff, "SMB0.CPU1.FAN", 0},
 { 0x0007000000bb00ff, "SMB0.PCI0", 0},
 { 0x0008000000bb00ff, "SMB0.PCI1", 0},
 { 0x0009000000bb00ff, "SMB0.PCI2", 0},
 { 0x0007000000b800ff, "SMB0.PCI3", 0},
 { 0x0008000000b800ff, "SMB0.PCI4", 0},
 { 0x0009000000b800ff, "SMB0.PCI5", 0},
 { 0x000Bff00000000ff, "SMB0.DIMM0", 0},
 { 0x000Eff00000100ff, "SMB0.DIMM1", 0},
 { 0x001Aff00000200ff, "SMB0.DIMM2", 0},
 { 0x001Cff00000300ff, "SMB0.DIMM3", 0},
 { 0x0001ff00001000ff, "SMB0.DIMM4", 0},
 { 0x0006ff00001100ff, "SMB0.DIMM5", 0},
 { 0x001Eff00001200ff, "SMB0.DIMM6", 0},
 { 0x0020ff00001300ff, "SMB0.DIMM7", 0},
 { 0x000Dff00002000ff, "SMB0.DIMM8", 0},
 { 0x0010ff00002100ff, "SMB0.DIMM9", 0},
 { 0x0019ff00002200ff, "SMB0.DIMM10", 0},
 { 0x001Bff00002300ff, "SMB0.DIMM11", 0},
 { 0x0005ff00003000ff, "SMB0.DIMM12", 0},
 { 0x0009ff00003100ff, "SMB0.DIMM13", 0},
 { 0x001Dff00003200ff, "SMB0.DIMM14", 0},
 { 0x001Fff00003300ff, "SMB0.DIMM15", 0},
 {0}       
};
#endif
