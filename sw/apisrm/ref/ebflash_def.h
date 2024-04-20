#ifndef __EBFLASH_H_LOADED
#define __EBFLASH_H_LOADED
/*
 * Copyright (C) 1996 by
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Facility:	SRM Console Firmware for Alpha Microprocessor
 *		Evaluation/Motherboards.
 *
 * Abstract:	Flash ROM Definitions
 *
 * Author:	Eric Rasmussen
 *
 * Creation Date: 30-Dec-1997
 *
 * Modifications:
 *
 */
/* $INCLUDE_OPTIONS$ */
/* $INCLUDE_OPTIONS_END$ */

#define	FLASH_BYTE	1
#define FLASH_WORD	2
#define FLASH_LONG	4
#define FLASH_QUAD	8

/* 
** Number of programming attempts before giving up
*/
#define	FLASH_RETRIES	3

/*
** Intel 28F008SA state machine commands
*/				
#define	READ_RESET	0xff
#define IID		0x90
#define READ_STATUS	0x70
#define CLEAR_STATUS	0x50
#define ERASE_SETUP	0x20
#define ERASE_CONFIRM	0xd0
#define ERASE_SUSPEND	0xB0
#define BYTE_WRITE	0x40
#define ALT_WR_SET_WR	0x10

/*
** Intel 28F008SA state machine status bits
*/
#define	SR_READY	(1 << 7)
#define SR_ERASE_SUSP	(1 << 6)
#define SR_ERASE_ERR	(1 << 5)
#define SR_WRITE_ERR	(1 << 4)
#define SR_VPP_LOW	(1 << 3)

#define SR_CMD_SEQ_ERR	(SR_WRITE_ERR | SR_ERASE_ERR)

#define BX_T_M_CODE	0x89
#define BX_T_D_CODE	0xA2
#define DEVICE_NAME	"Intel 28F008SA"

#endif /* __EBFLASH_H_LOADED */
        
