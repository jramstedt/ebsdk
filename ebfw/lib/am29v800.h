#ifndef _AM29V800_H_
#define _AM29V800_H_
/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

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

/*
 *  $Id: am29v800.h,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $
 */

/*
 * $Log: am29v800.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1997/05/02  19:59:51  fdh
 * Corrected the RCS directive.
 *
 * Revision 1.1  1997/05/01  20:41:28  pbell
 * Initial revision
 *
 */

//
// The AMD 800 is a full 1 megabyte broken into the following sectors:
//
// 0x00000 .. 0x03fff       -- >  Section 1         // SROM 
// 0x04000 .. 0x05fff       -- >  Section 2         // NVRAM_0
// 0x06000 .. 0x07fff       -- >  Section 3         // NVRAM_1
// 0x08000 .. 0x0ffff       -- >  Section 4         // DROM
// 0x10000 .. 0x1ffff       -- >  Section 5         // AlphaBIOS starts here
// 0x20000 .. 0x2ffff       -- >  Section 6         //  448 KB
// 0x30000 .. 0x3ffff       -- >  Section 7
// 0x40000 .. 0x4ffff       -- >  Section 8
// 0x50000 .. 0x5ffff       -- >  Section 9
// 0x60000 .. 0x6ffff       -- >  Section 10
// 0x70000 .. 0x7ffff       -- >  Section 11

// 0x80000 .. 0x8ffff       -- >  Section 12        // SRM Console starts here
// 0x90000 .. 0x9ffff       -- >  Section 13        //  512 KB
// 0xA0000 .. 0xAffff       -- >  Section 14
// 0xB0000 .. 0xBffff       -- >  Section 15
// 0xC0000 .. 0xCffff       -- >  Section 16
// 0xD0000 .. 0xDffff       -- >  Section 17
// 0xE0000 .. 0xEffff       -- >  Section 18
// 0xF0000 .. 0xFffff       -- >  Section 19

//
// Name the blocks
//

#define SECTOR_1_BASE       0x0
#define SECTOR_2_BASE       0x4000
#define SECTOR_3_BASE       0x6000
#define SECTOR_4_BASE       0x8000
#define SECTOR_5_BASE       0x10000
#define SECTOR_6_BASE       0x20000
// ...
#define SECTOR_12_BASE      0x80000

#define SECTOR_1_SIZE       (SECTOR_2_BASE - SECTOR_1_BASE)
#define SECTOR_2_SIZE       (SECTOR_3_BASE - SECTOR_2_BASE)
#define SECTOR_3_SIZE       (SECTOR_4_BASE - SECTOR_3_BASE)
#define SECTOR_4_SIZE       (SECTOR_5_BASE - SECTOR_4_BASE)
#define SECTOR_5_SIZE       (SECTOR_6_BASE - SECTOR_5_BASE)

//
// The largest addressable offset
//

#define Am29LV800_DEVICE_SIZE 0x100000

//
// What an erased data byte looks like
//

#define Am29LV800_ERASED_DATA 0xff

//
// The command writing sequence for read/auto select/write
// Valid for byte mode only.
//

#define COMMAND_READ_RESET  0xf0

#define COMMAND_ADDR1       0xaaaa
#define COMMAND_DATA1       0xaa

#define COMMAND_ADDR2       0x5555
#define COMMAND_DATA2       0x55

#define COMMAND_ADDR3       0xaaaa
#define COMMAND_DATA3_READ          0xf0        // then {addr,data}
#define COMMAND_DATA3_AUTOSELECT    0x90
#define COMMAND_DATA3_PROGRAM       0xA0        // then {addr,data}
#define COMMAND_DATA3       0x80                // for the additional ops

// 
// and additional bus cyles for chip and sector erase
//

#define COMMAND_ADDR4       0xaaaa
#define COMMAND_DATA4       0xaa

#define COMMAND_ADDR5       0x5555
#define COMMAND_DATA5       0x55

// for SECTOR ERASE, the addr is the sector number
#define COMMAND_DATA6_SECTOR_ERASE  0x30

#define COMMAND_ADDR6       0xaaaa
#define COMMAND_DATA6_CHIP_ERASE    0x10

//
// This is the Am29LV800 made by AMD
// 
// The Device ID given here assumes byte 
// mode and bottom boot block. If the 
// device is wired differently on some
// future platform, this will have to
// be made platform-specific.
//

#define MANUFACTURER_ID     0x01
#define DEVICE_ID           0x5b

//
// Max attempts
//

#define MAX_FLASH_READ_ATTEMPTS 0x800000

#include "pflash.h"

#endif // _AM29V800_H_
