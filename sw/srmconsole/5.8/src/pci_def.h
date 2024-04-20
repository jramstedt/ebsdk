/* file:	pci_def.h
 *
 * Copyright (C) 1993 by
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
 *
 *
 * Abstract:	platform-specific PCI definitions.
 *
 * Author:    	Doug Neale
 *		Carl Furbeck
 *		Judith Gold
 *
 * Modifications:
 *         	dwn	11-may-1993	Added some constants
 *         	jeg	15-Apr-1993	Initial entry.
 */
#define VEND_DEV_ID		0x0
#define COM_STAT		0x4
#define REV_ID			0x8
#define CACHE_L_SIZ		0xC
#define BASE_ADDR0		0x10
#define BASE_ADDR1		0x14
#define BASE_ADDR2		0x18
#define BASE_ADDR3		0x1C
#define BASE_ADDR4		0x20
#define BASE_ADDR5		0x24
#define RESERVED0		0x28
#define RESERVED1		0x2C
#define EXP_ROM_BASE		0x30
#define RESERVED2		0x34
#define RESERVED3		0x38
#define INT_LINE		0x3C
