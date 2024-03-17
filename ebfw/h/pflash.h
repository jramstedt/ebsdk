#ifndef _PFLASH_H_
#define _PFLASH_H_

/*****************************************************************************

Copyright © 1994, Digital Equipment Corporation, Maynard, Massachusetts. 

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

#define FLASH_H_RCSID "$Id: pflash.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $"

/*
 * $Log: pflash.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/05/01  20:04:52  pbell
 * Initial revision
 *
 */

//
// The AMD is a full megabyte broken into the following sectors:
//
// 0x00000 .. 0x03fff       -- > SROM    (not writable)
// 0x04000 .. 0x05fff       -- > First   8K 'nvram'
// 0x06000 .. 0x07fff       -- > Second  8K 'nvram'
// 0x08000 .. 0x0ffff       -- > DROM    (not writable)
// 0x10000 .. 0x1ffff       -- > First   64K of ARC firmware
// 0x20000 .. 0x2ffff       -- > Second  64K of ARC firmware
// 0x30000 .. 0x3ffff       -- > Third   64K of ARC firmware
// 0x40000 .. 0x4ffff       -- > Fourth  64K of ARC firmware
// 0x50000 .. 0x5ffff       -- > Fifth   64K of ARC firmware
// 0x60000 .. 0x6ffff       -- > Sixth   64K of ARC firmware
// 0x70000 .. 0x7ffff       -- > Seventh 64K of ARC firmware

// 0x80000 .. 0xFffff       -- > 8 x 64KB sectors, for SRM

//
// Name the blocks
//

#include "am29v800.h"

#define SROM_BASE           SECTOR_1_BASE
#define NVRAM1_BASE         SECTOR_2_BASE
#define NVRAM2_BASE         SECTOR_3_BASE
#define DROM_BASE           SECTOR_4_BASE
#define ARC_ROM_BASE        SECTOR_5_BASE
#define SRM_ROM_BASE        SECTOR_12_BASE

#define SROM_SECTOR_SIZE    SECTOR_1_SIZE
#define NVRAM1_SECTOR_SIZE  SECTOR_2_SIZE
#define NVRAM2_SECTOR_SIZE  SECTOR_3_SIZE
#define DROM_SECTOR_SIZE    SECTOR_4_SIZE
#define ARC_SECTOR_SIZE     (SECTOR_12_BASE - SECTOR_5_BASE)
#define SRM_SECTOR_SIZE     (Am29LV800_DEVICE_SIZE - SECTOR_12_BASE)

#endif // _PFLASH_H_
