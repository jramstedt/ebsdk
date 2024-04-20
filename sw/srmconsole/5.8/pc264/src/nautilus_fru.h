/*
 * file:    nautilus_fru.h
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
 { 0x00A2ff0000ff00ff, "SMB", 1},
 { 0x00A4010000ff00ff, "SMB.CPU0", 1},
 { 0x0008000000bb00ff, "SMB.PCI0", 0},
 { 0x0009000000bb00ff, "SMB.PCI1", 0},
 { 0x000a000000bb00ff, "SMB.PCI2", 0},
 { 0x000b000000bb00ff, "SMB.PCI3", 0},
 { 0x0005000000bb00ff, "SMB.AGP0", 0},
 { 0x000Bff00000000ff, "SMB.DIMM0", 0},
 { 0x000Eff00000100ff, "SMB.DIMM1", 0},
 { 0x001Aff00000200ff, "SMB.DIMM2", 0},
 {0}
};
