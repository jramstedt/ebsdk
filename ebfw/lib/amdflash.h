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
 *  $Id: amdflash.h,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $;
 */

/*
Module Name:

    amdflash.h

Abstract:

    This file contains definitions for the AMD flash driver.

Author:

    Jeff Wiedemeier   09-Dec-1995

 * $Log: amdflash.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1997/08/18  15:47:39  fdh
 * Corrected the RCS directive.
 *
 * Revision 1.1  1997/05/01  20:41:48  pbell
 * Initial revision
 *
 */

#ifndef __AMDFLASH_H__
#define __AMDFLASH_H__

#define AMD_MANUFACTURER_ID     (0x01)

#define AMD_TIMEOUT_VALUE       (5000000)
#define AMD_COMMAND_END_OF_SEQUENCE {0x0000, 0x00}

typedef struct _AMD_FLASH_COMMAND {
  ULONG Offset;
  UCHAR Data;
} AMD_FLASH_COMMAND, *PAMD_FLASH_COMMAND;

typedef struct _AMD_FLASH_COMMANDS {
  PAMD_FLASH_COMMAND ResetReadCommand;
  PAMD_FLASH_COMMAND ByteProgramCommand;
  PAMD_FLASH_COMMAND SectorEraseCommand;
} AMD_FLASH_COMMANDS, *PAMD_FLASH_COMMANDS;

typedef struct _AMD_SECTOR_INFORMATION {
  ULONG Offset;
  ULONG Size;
} AMD_SECTOR_INFORMATION, *PAMD_SECTOR_INFORMATION;

typedef struct _AMD_FLASH_DEVICE {
  PCHAR DeviceName;
  UCHAR DeviceId;
  ULONG DeviceSize;
  UCHAR ErasedData;
  UCHAR SectorEraseConfirm;
  PAMD_SECTOR_INFORMATION Layout;
  PAMD_FLASH_COMMANDS Commands;
} AMD_FLASH_DEVICE, *PAMD_FLASH_DEVICE;

#endif // !__AMDFLASH_H__
