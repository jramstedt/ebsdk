/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
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

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: amdflash.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
Module Name:

    amdflash.c

Abstract:

    This file contains the driver for the AMD flash devices.
    Currently the driver supports the Am29f080 and Am29f016
    devices.

Author:

    Jeff Wiedemeier   09-Dec-1996
   
    Adapted from 29f040.c

 * $Log: amdflash.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/05/01  20:41:53  pbell
 * Initial revision
 *
 */

#include "halp.h"
#include "arccodes.h"
#include "flash8k.h"
#include "amdflash.h"

//
// External prototypes
//
VOID
pWriteFlashByte(
    IN ULONG FlashOffset,
    IN UCHAR Data
    );

UCHAR
pReadFlashByte(
    IN ULONG FlashOffset
    );

//++
// Local function prototypes
//--
//
// Init function
//
PFLASH_DRIVER
AmdFlash_Initialize(
    IN PUCHAR FlashOffset
    );

//
// Helper functions
//
static
VOID
AmdFlashCommandSequence(
    PAMD_FLASH_COMMAND CommandSequence
    );

static
VOID
AmdFlashSetLastError(
    ULONG Error
    );

static
ARC_STATUS
AmdFlash_CheckStatus(
    IN PUCHAR FlashOffset,
    IN FLASH_OPERATIONS Operation,
    IN UCHAR Data
    );

//
// API functions
//
ARC_STATUS
AmdFlash_SetReadMode(
    IN PUCHAR FlashOffset
    );

ARC_STATUS
AmdFlash_WriteByte(
    IN PUCHAR FlashOffset,
    IN UCHAR Data
    );

ARC_STATUS
AmdFlash_EraseSector(
    IN PUCHAR FlashOffset
    );

PUCHAR
AmdFlash_SectorAlign(
    IN PUCHAR FlashOffset
    );

UCHAR
AmdFlash_ReadByte(
    IN PUCHAR FlashOffset
    );

BOOLEAN
AmdFlash_OverwriteCheck(
    IN UCHAR OldData,
    IN UCHAR NewData
    );

ULONG
AmdFlash_SectorSize(
    IN PUCHAR FlashOffset
    );

ULONG
AmdFlash_GetLastError(
    VOID
    );

//
// Command Sequences
//
static AMD_FLASH_COMMAND AmdFlashCommandAutoselect[] = {
  {0x5555, 0xaa},
  {0x2aaa, 0x55},
  {0x5555, 0x90},
  AMD_COMMAND_END_OF_SEQUENCE
};

static AMD_FLASH_COMMAND AmdFlashCommandResetRead[] = {
  {0x5555, 0xaa},
  {0x2aaa, 0x55},
  {0x5555, 0xf0},
  AMD_COMMAND_END_OF_SEQUENCE
};

static AMD_FLASH_COMMAND AmdFlashCommandByteProgram[] = {
  {0x5555, 0xaa},
  {0x2aaa, 0x55},
  {0x5555, 0xa0},
  AMD_COMMAND_END_OF_SEQUENCE
};

static AMD_FLASH_COMMAND AmdFlashCommandSectorErase[] = {
  {0x5555, 0xaa},
  {0x2aaa, 0x55},
  {0x5555, 0x80},
  {0x5555, 0xaa},
  {0x2aaa, 0x55},
  AMD_COMMAND_END_OF_SEQUENCE
};

//++
// AMD Am29v800T / Am29v800B 1MB flash ROM - BYTE MODE ONLY
//--
//
// Command sequences
//
static AMD_FLASH_COMMAND Am29v800CommandAutoselect[] = {
  {0xaaaa, 0xaa},
  {0x5555, 0x55},
  {0xaaaa, 0x90},
  AMD_COMMAND_END_OF_SEQUENCE
};

static AMD_FLASH_COMMAND Am29v800CommandResetRead[] = {
  {0xaaaa, 0xf0},
  AMD_COMMAND_END_OF_SEQUENCE
};

static AMD_FLASH_COMMAND Am29v800CommandByteProgram[] = {
  {0xaaaa, 0xaa},
  {0x5555, 0x55},
  {0xaaaa, 0xa0},
  AMD_COMMAND_END_OF_SEQUENCE
};

static AMD_FLASH_COMMAND Am29v800CommandSectorErase[] = {
  {0xaaaa, 0xaa},
  {0x5555, 0x55},
  {0xaaaa, 0x80},
  {0xaaaa, 0xaa},
  {0x5555, 0x55},
  AMD_COMMAND_END_OF_SEQUENCE
};

//
// Am29v800T Layout
//
//    Sector   Start    End     Sector
//    Number   Addr     Addr     Size
//    ------  -------  -------  ------
//       0    0x00000  0x0ffff  64 kB
//       1    0x10000  0x1ffff  64 kB
//       2    0x20000  0x2ffff  64 kB
//       3    0x30000  0x3ffff  64 kB
//       4    0x40000  0x4ffff  64 kB
//       5    0x50000  0x5ffff  64 kB
//       6    0x60000  0x6ffff  64 kB
//       7    0x70000  0x7ffff  64 kB
//       8    0x80000  0x8ffff  64 kB
//       9    0x90000  0x9ffff  64 kB
//      10    0xa0000  0xaffff  64 kB
//      11    0xb0000  0xbffff  64 kB
//      12    0xc0000  0xcffff  64 kB
//      13    0xd0000  0xdffff  64 kB
//      14    0xe0000  0xeffff  64 kB
//      15    0xf0000  0xf7fff  32 kB
//      16    0xf8000  0xf9fff   8 kB
//      17    0xfa000  0xfbfff   8 kB
//      18    0xfc000  0xfffff  16 kB
//
static AMD_SECTOR_INFORMATION Am29v800TLayout[] = {
  {0x00000, 0x10000},  // Sector 0
  {0x10000, 0x10000},  // Sector 1
  {0x20000, 0x10000},  // Sector 2
  {0x30000, 0x10000},  // Sector 3
  {0x40000, 0x10000},  // Sector 4
  {0x50000, 0x10000},  // Sector 5
  {0x60000, 0x10000},  // Sector 6
  {0x70000, 0x10000},  // Sector 7
  {0x80000, 0x10000},  // Sector 8
  {0x90000, 0x10000},  // Sector 9
  {0xa0000, 0x10000},  // Sector 10
  {0xb0000, 0x10000},  // Sector 11
  {0xc0000, 0x10000},  // Sector 12
  {0xd0000, 0x10000},  // Sector 13
  {0xe0000, 0x10000},  // Sector 14
  {0xf0000, 0x08000},  // Sector 15
  {0xf8000, 0x02000},  // Sector 16
  {0xfa000, 0x02000},  // Sector 17
  {0xfc000, 0x04000},  // Sector 18
  {0x00000, 0x00000}
};

//
// Am29v800B Layout
//
//    Sector   Start    End     Sector
//    Number   Addr     Addr     Size
//    ------  -------  -------  ------
//       0    0x00000  0x03fff  16 kB
//       1    0x04000  0x05fff   8 kB
//       2    0x06000  0x07fff   8 kB
//       3    0x08000  0x0ffff  32 kB
//       4    0x10000  0x1ffff  64 kB
//       5    0x20000  0x2ffff  64 kB
//       6    0x30000  0x3ffff  64 kB
//       7    0x40000  0x4ffff  64 kB
//       8    0x50000  0x5ffff  64 kB
//       9    0x60000  0x6ffff  64 kB
//      10    0x70000  0x7ffff  64 kB
//      11    0x80000  0x8ffff  64 kB
//      12    0x90000  0x9ffff  64 kB
//      13    0xa0000  0xaffff  64 kB
//      14    0xb0000  0xbffff  64 kB
//      15    0xc0000  0xcffff  64 kB
//      16    0xd0000  0xdffff  64 kB
//      17    0xe0000  0xeffff  64 kB
//      18    0xf0000  0xfffff  64 kB
//
static AMD_SECTOR_INFORMATION Am29v800BLayout[] = {
  {0x00000, 0x10000},  // Sector 0
  {0x10000, 0x10000},  // Sector 1
  {0x20000, 0x10000},  // Sector 2
  {0x30000, 0x10000},  // Sector 3
  {0x40000, 0x10000},  // Sector 4
  {0x50000, 0x10000},  // Sector 5
  {0x60000, 0x10000},  // Sector 6
  {0x70000, 0x10000},  // Sector 7
  {0x80000, 0x10000},  // Sector 8
  {0x90000, 0x10000},  // Sector 9
  {0xa0000, 0x10000},  // Sector 10
  {0xb0000, 0x10000},  // Sector 11
  {0xc0000, 0x10000},  // Sector 12
  {0xd0000, 0x10000},  // Sector 13
  {0xe0000, 0x10000},  // Sector 14
  {0xf0000, 0x08000},  // Sector 15
  {0xf8000, 0x02000},  // Sector 16
  {0xfa000, 0x02000},  // Sector 17
  {0xfc000, 0x04000},  // Sector 18
  {0x00000, 0x00000}
};

//
// Am29v800 commands
//
static AMD_FLASH_COMMANDS Am29v800Commands = {
  Am29v800CommandResetRead,
  Am29v800CommandByteProgram,
  Am29v800CommandSectorErase
};

//
// Am29v800T FLASH_DEVICE structure
//
static AMD_FLASH_DEVICE Am29v800TDeviceInfo = {
  "AMD Am29v800T",      // Device Name
  0xda,                 // Device Id
  0x100000,             // Device Size (1 MB)
  0xff,                 // Erased data
  0x30,                 // SectorEraseConfirm
  Am29v800TLayout,      // Device Layout
  &Am29v800Commands     // Commands
};

//
// Am29v800B FLASH_DEVICE structure
//
static AMD_FLASH_DEVICE Am29v800BDeviceInfo = {
  "AMD Am29v800B",      // Device Name
  0x5b,                 // Device Id
  0x100000,             // Device Size (1 MB)
  0xff,                 // Erased data
  0x30,                 // SectorEraseConfirm
  Am29v800BLayout,      // Device Layout
  &Am29v800Commands     // Commands
};

//++
// AMD Am29f080 1MB flash ROM
//--
//
// Layout
//
//    Sector   Start    End     Sector
//    Number   Addr     Addr     Size
//    ------  -------  -------  ------
//       0    0x00000  0x0ffff  64 kB
//       1    0x10000  0x1ffff  64 kB
//       2    0x20000  0x2ffff  64 kB
//       3    0x30000  0x3ffff  64 kB
//       4    0x40000  0x4ffff  64 kB
//       5    0x50000  0x5ffff  64 kB
//       6    0x60000  0x6ffff  64 kB
//       7    0x70000  0x7ffff  64 kB
//       8    0x80000  0x8ffff  64 kB
//       9    0x90000  0x9ffff  64 kB
//      10    0xa0000  0xaffff  64 kB
//      11    0xb0000  0xbffff  64 kB
//      12    0xc0000  0xcffff  64 kB
//      13    0xd0000  0xdffff  64 kB
//      14    0xe0000  0xeffff  64 kB
//      15    0xf0000  0xfffff  64 kB
//
static AMD_SECTOR_INFORMATION Am29f080Layout[] = {
  {0x00000, 0x10000},  // Sector 0
  {0x10000, 0x10000},  // Sector 1
  {0x20000, 0x10000},  // Sector 2
  {0x30000, 0x10000},  // Sector 3
  {0x40000, 0x10000},  // Sector 4
  {0x50000, 0x10000},  // Sector 5
  {0x60000, 0x10000},  // Sector 6
  {0x70000, 0x10000},  // Sector 7
  {0x80000, 0x10000},  // Sector 8
  {0x90000, 0x10000},  // Sector 9
  {0xa0000, 0x10000},  // Sector 10
  {0xb0000, 0x10000},  // Sector 11
  {0xc0000, 0x10000},  // Sector 12
  {0xd0000, 0x10000},  // Sector 13
  {0xe0000, 0x10000},  // Sector 14
  {0xf0000, 0x10000},  // Sector 15
  {0x00000, 0x00000}
};

//
// The Am29f080 commands
//
static AMD_FLASH_COMMANDS Am29f080Commands = {
  AmdFlashCommandResetRead,
  AmdFlashCommandByteProgram,
  AmdFlashCommandSectorErase
};

//
// The Am29f080 FLASH_DEVICE structure
//
static AMD_FLASH_DEVICE Am29f080DeviceInfo = {
  "AMD Am29f080",       // Device Name
  0xd5,                 // Device Id
  0x100000,             // Device Size (1 MB)
  0xff,                 // Erased data
  0x30,                 // SectorEraseConfirm
  Am29f080Layout,       // Device Layout
  &Am29f080Commands     // Commands
};

//++
// AMD Am29f016 2MB flash ROM
//--
//      
// Layout
//
//    Sector   Start      End     Sector
//    Number   Addr       Addr     Size
//    ------  --------  --------  ------
//       0    0x000000  0x00ffff  64 kB
//       1    0x010000  0x01ffff  64 kB
//       2    0x020000  0x02ffff  64 kB
//       3    0x030000  0x03ffff  64 kB
//       4    0x040000  0x04ffff  64 kB
//       5    0x050000  0x05ffff  64 kB
//       6    0x060000  0x06ffff  64 kB
//       7    0x070000  0x07ffff  64 kB
//       8    0x080000  0x08ffff  64 kB
//       9    0x090000  0x09ffff  64 kB
//      10    0x0a0000  0x0affff  64 kB
//      11    0x0b0000  0x0bffff  64 kB
//      12    0x0c0000  0x0cffff  64 kB
//      13    0x0d0000  0x0dffff  64 kB
//      14    0x0e0000  0x0effff  64 kB
//      15    0x0f0000  0x0fffff  64 kB
//      16    0x100000  0x10ffff  64 kB
//      17    0x110000  0x11ffff  64 kB
//      18    0x120000  0x12ffff  64 kB
//      19    0x130000  0x13ffff  64 kB
//      20    0x140000  0x14ffff  64 kB
//      21    0x150000  0x15ffff  64 kB
//      22    0x160000  0x16ffff  64 kB
//      23    0x170000  0x17ffff  64 kB
//      24    0x180000  0x18ffff  64 kB
//      25    0x190000  0x19ffff  64 kB
//      26    0x1a0000  0x1affff  64 kB
//      27    0x1b0000  0x1bffff  64 kB
//      28    0x1c0000  0x1cffff  64 kB
//      29    0x1d0000  0x1dffff  64 kB
//      30    0x1e0000  0x1effff  64 kB
//      31    0x1f0000  0x1fffff  64 kB
//
static AMD_SECTOR_INFORMATION Am29f016Layout[] = {
  {0x000000, 0x10000},  // Sector 0
  {0x010000, 0x10000},  // Sector 1
  {0x020000, 0x10000},  // Sector 2
  {0x030000, 0x10000},  // Sector 3
  {0x040000, 0x10000},  // Sector 4
  {0x050000, 0x10000},  // Sector 5
  {0x060000, 0x10000},  // Sector 6
  {0x070000, 0x10000},  // Sector 7
  {0x080000, 0x10000},  // Sector 8
  {0x090000, 0x10000},  // Sector 9
  {0x0a0000, 0x10000},  // Sector 10
  {0x0b0000, 0x10000},  // Sector 11
  {0x0c0000, 0x10000},  // Sector 12
  {0x0d0000, 0x10000},  // Sector 13
  {0x0e0000, 0x10000},  // Sector 14
  {0x0f0000, 0x10000},  // Sector 15
  {0x100000, 0x10000},  // Sector 16
  {0x110000, 0x10000},  // Sector 17
  {0x120000, 0x10000},  // Sector 18
  {0x130000, 0x10000},  // Sector 19
  {0x140000, 0x10000},  // Sector 20
  {0x150000, 0x10000},  // Sector 21
  {0x160000, 0x10000},  // Sector 22
  {0x170000, 0x10000},  // Sector 23
  {0x180000, 0x10000},  // Sector 24
  {0x190000, 0x10000},  // Sector 25
  {0x1a0000, 0x10000},  // Sector 26
  {0x1b0000, 0x10000},  // Sector 27
  {0x1c0000, 0x10000},  // Sector 28
  {0x1d0000, 0x10000},  // Sector 29
  {0x1e0000, 0x10000},  // Sector 30
  {0x1f0000, 0x10000},  // Sector 31
  {0x00000, 0x00000}
};

//
// The Am29f016 commands
//
static AMD_FLASH_COMMANDS Am29f016Commands = {
  AmdFlashCommandResetRead,
  AmdFlashCommandByteProgram,
  AmdFlashCommandSectorErase
};

//
// The Am29f016 FLASH_DEVICE structure
//
static AMD_FLASH_DEVICE Am29f016DeviceInfo = {
  "AMD Am29f016",       // Device Name
  0xad,                 // Device Id
  0x200000,             // Device Size (1 MB)
  0xff,                 // Erased data
  0x30,                 // SectorEraseConfirm
  Am29f016Layout,       // Device Layout
  &Am29f016Commands     // Commands
};

//++
// Known autoselect sequences
//--
static PAMD_FLASH_COMMAND AmdFlashAutoselectSequences[] = {
  AmdFlashCommandAutoselect,
  Am29v800CommandAutoselect,
  NULL
};

//++
// List of support devices
//--
static PAMD_FLASH_DEVICE SupportedDevices[] = {
  &Am29v800TDeviceInfo,
  &Am29v800BDeviceInfo,
  &Am29f080DeviceInfo,
  &Am29f016DeviceInfo,
  NULL
};

//++
// The flash driver structure
//--
FLASH_DRIVER AmdFlashFlashDriver = {
  NULL, // DeviceName - will be filled in by Initialize
  AmdFlash_SetReadMode,      // SetReadModeFunction
  AmdFlash_WriteByte,        // WriteByteFunction
  AmdFlash_EraseSector,      // EraseBlockFunction
  AmdFlash_SectorAlign,      // AlignBlockFunction
  AmdFlash_ReadByte,         // ReadByteFunction
  AmdFlash_OverwriteCheck,   // OverwriteCheckFunction
  AmdFlash_SectorSize,       // BlockSizeFunction
  AmdFlash_GetLastError,     // GetLastErrorFunction
  0,    // DeviceSize - will be filled in by initialize
  0     // ErasedData - will be filled in by initialize
};

//++
// Driver information
//--
static struct _AMDFLASH_DRIVER_INFORMATION {
  PFLASH_DRIVER pFlashDriver;
  PAMD_FLASH_DEVICE pFlashDevice;
  ULONG LastError;
} DriverInformation;

//++
// Initialization function
//--

PFLASH_DRIVER
AmdFlash_Initialize(
    IN PUCHAR FlashOffset
    )
/*++

Routine Description:

    This function...

Arguments:

    FlashOffset - offset within the flash rom

Return Value:

    NULL for no driver or the address of the FLASH_DRIVER structure for
    this driver.

--*/
{
  PFLASH_DRIVER ReturnDriver = NULL;
  UCHAR ManufacturerId;
  UCHAR DeviceId;
  int sequence;

  //
  // Try the known autoselect sequences until a device is found.
  //
  for(sequence=0; 
      (AmdFlashAutoselectSequences[sequence] != NULL) &&
        (ReturnDriver == NULL);
      sequence++) {

    //
    // Send the autoselect sequence and read the manufacturer and
    // device id.
    //
    AmdFlashCommandSequence(AmdFlashAutoselectSequences[sequence]);
    ManufacturerId = pReadFlashByte(0);
    DeviceId = pReadFlashByte(1);

    if (ManufacturerId == AMD_MANUFACTURER_ID) {
      int i;

      //
      // It's and AMD part - loop through the list
      // of supported devices to try to find a match
      // for the device id.
      //
      for(i = 0; SupportedDevices[i] != NULL; i++) {
        if (DeviceId == SupportedDevices[i]->DeviceId) {
          //
          // We found a supported device - fill in the 
          // FlashDriver structure
          //
          AmdFlashFlashDriver.DeviceName = SupportedDevices[i]->DeviceName;
          AmdFlashFlashDriver.DeviceSize = SupportedDevices[i]->DeviceSize;
          AmdFlashFlashDriver.ErasedData = SupportedDevices[i]->ErasedData;

          //
          // Now fill out the driver information structure
          //
          DriverInformation.pFlashDriver = &AmdFlashFlashDriver;
          DriverInformation.pFlashDevice = SupportedDevices[i];
          DriverInformation.LastError = 0;

          //
          // Remember the flash driver structure
          //
          ReturnDriver = DriverInformation.pFlashDriver;

          //
          // Set the device to read mode
          //
          DriverInformation.pFlashDriver->SetReadModeFunction(FlashOffset);

          //
          // And break out of this loop
          //
          break;
        }
      }
    }
  }

  return ReturnDriver;
}

//++
// Helper functions
//--

static
VOID
AmdFlashCommandSequence(
    PAMD_FLASH_COMMAND CommandSequence
    )
{
  int i;

  for(i = 0; 
      (CommandSequence[i].Offset != 0) || (CommandSequence[i].Data != 0);
      i++) {

    pWriteFlashByte(CommandSequence[i].Offset,
                    CommandSequence[i].Data);
  }
}

static
VOID
AmdFlashSetLastError(
    ULONG Error
    )
/*++

Routine Description:

    This function sets the last error value for the driver.

Arguments:

    Error - value to set last error to.

Return Value:

    None

--*/
{
  DriverInformation.LastError = Error;
}

static
ARC_STATUS
AmdFlash_CheckStatus(
    IN PUCHAR FlashOffset,
    IN FLASH_OPERATIONS Operation,
    IN UCHAR Data
    )
/*++

Routine Description:

    This function checks the status of a flash operation.

Arguments:

    FlashOffset - offset within the flash
    Operation   - operation performed
    Data        - data value which may be needed for verification

Return Value:

    ESUCCESS for success, else EIO.

--*/
{
  ARC_STATUS ReturnStatus = EIO;
  UCHAR FlashRead;
  BOOLEAN DataOK = FALSE;
  ULONG Timeout;

  //
  // Follow the AMD /Data Polling Algorithm
  //
  //       +-------+
  //       | Start |
  //       +---+---+
  //           |
  //           V
  //     +-----------+
  // /-->| Read Byte |
  // |   +-----+-----+
  // |         |
  // |         V
  // |        / \
  // |   -----   -----  Yes
  // |  < DQ7 == Data >-------\
  // |   -----   -----        |
  // |        \ /             |
  // |         | No           |
  // |         V              |
  // |        / \             |
  // | No ----   ----         |
  // \---< DQ5 == 1  >        |
  //      ----   ----         |
  //          \ /             |
  //           | Yes          |
  //           V              |
  //     +-----------+        |
  //     | Read Byte |        |
  //     +-----+-----+        |
  //           |              |
  //           V              |
  //          / \             |
  //     -----   -----  Yes   |
  //    < DQ7 == Data >------>|
  //     -----   -----        |
  //          \ /             V
  //           | No       +-------+
  //           V          | Pass  |
  //       +-------+      +-------+
  //       | Fail  |
  //       +-------+
  //
  // We also add a timeout to avoid an infinite loop
  //
  for(Timeout = AMD_TIMEOUT_VALUE; 
      !DataOK && (Timeout > 0);
      Timeout--) {
    FlashRead = DriverInformation.pFlashDriver->ReadByteFunction(FlashOffset);
  
    if (((FlashRead ^ Data) & 0x80) == 0) {
      DataOK = TRUE;
    } else if (FlashRead & 0x20) {
      FlashRead = DriverInformation.pFlashDriver->ReadByteFunction(FlashOffset);
      if (((FlashRead ^ Data) & 0x80) == 0) {
        DataOK = TRUE;
      } else {
        //
        // This is the explicit failure condition
        //
        break;
      }
    }

    KeStallExecutionProcessor(1);
  }

  if (DataOK) {
    ReturnStatus = ESUCCESS;
  } else {
    //
    // There was an error - set the last error value.
    //
    AmdFlashSetLastError(Operation | 0x80000000);
  }

  return ReturnStatus;
}

//++
// Driver API functions
//--
ARC_STATUS
AmdFlash_SetReadMode(
    IN PUCHAR FlashOffset
    )
/*++

Routine Description:

    This function sets the flash device into read mode.

Arguments:

    FlashOffset - offset within the flash (not used)

Return Value:

    ESUCCESS

--*/
{
  AmdFlashCommandSequence(DriverInformation.pFlashDevice->Commands->ResetReadCommand);

  return ESUCCESS;
}

ARC_STATUS
AmdFlash_WriteByte(
    IN PUCHAR FlashOffset,
    IN UCHAR Data
    )
/*++

Routine Description:

    This function programs a byte into the flash.

Arguments:

    FlashOffset - offset within the flash
    Data        - the byte to write

Return Value:

    ESUCCESS on success, else EIO (return value determined by check
    status routine).

--*/
{
  ARC_STATUS ReturnStatus;

  AmdFlashCommandSequence(DriverInformation.pFlashDevice->Commands->ByteProgramCommand);
  pWriteFlashByte((uintptr_t)FlashOffset, Data);

  //
  // Get the status
  //
  ReturnStatus = AmdFlash_CheckStatus(FlashOffset, FlashByteWrite, Data);

  //
  // Return the device to read mode
  //
  DriverInformation.pFlashDriver->SetReadModeFunction(FlashOffset);

  return ReturnStatus;
}

ARC_STATUS
AmdFlash_EraseSector(
    IN PUCHAR FlashOffset
    )
/*++

Routine Description:

    This function erases a sector within the flash.

Arguments:

    FlashOffset - offset within the flash

Return Value:

    ESUCCESS or EIO (determined by check status routine).

--*/
{
  ARC_STATUS ReturnStatus;

  AmdFlashCommandSequence(DriverInformation.pFlashDevice->Commands->SectorEraseCommand);

  //
  // Finish the sequence...
  //
  pWriteFlashByte((uintptr_t)FlashOffset, 
                  DriverInformation.pFlashDevice->SectorEraseConfirm);

  //
  // Check the status
  //
  ReturnStatus = AmdFlash_CheckStatus(FlashOffset,
                                      FlashEraseBlock,
                                      DriverInformation.pFlashDevice->ErasedData);
  //
  // Return the device to read mode
  //
  DriverInformation.pFlashDriver->SetReadModeFunction(FlashOffset);
  
  return ReturnStatus;
}

PUCHAR
AmdFlash_SectorAlign(
    IN PUCHAR FlashOffset
    )
/*++

Routine Description:

    This function returns the base offset of the sector in which
    FlashOffset lies.

Arguments:

    FlashOffset - offset within the flash

Return Value:

    The base offset of the specified sector - 0 if FlashOffset is
    out of range.

--*/
{
  PAMD_SECTOR_INFORMATION pLayout;
  uintptr_t BaseOffset = 0;
  int i;

  //
  // Get the sector information
  //
  pLayout = DriverInformation.pFlashDevice->Layout;

  //
  // Loop throught the layout structure and find the 
  // desired sector
  //
  for(i = 0; pLayout[i].Size != 0; i++) {
    if (((uintptr_t)FlashOffset >= pLayout[i].Offset) &&
        ((uintptr_t)FlashOffset < (pLayout[i].Offset + pLayout[i].Size))) {
      //
      // FlashOffset falls in the current sector, set BaseOffset
      //
      BaseOffset = pLayout[i].Offset;
      break;
    }
  }

  return (PUCHAR)BaseOffset;
}

UCHAR
AmdFlash_ReadByte(
    IN PUCHAR FlashOffset
    )
/*++

Routine Description:

    This function reads a byte from the flash. It assumes that
    the flash device is in read mode.

Arguments:

    FlashOffset - offset within the flash

Return Value:

    Returns the byte read.

--*/
{
  return pReadFlashByte((uintptr_t)FlashOffset);
}

BOOLEAN
AmdFlash_OverwriteCheck(
    IN UCHAR OldData,
    IN UCHAR NewData
    )
/*++

Routine Description:

    This routine returns if we can safely overwrite an existing data
    with new data. Flash Rom's can go from 1-->1, 1-->0 and 0-->0, but
    cannot go from 0-->1.

Arguments:

    OldData - the data currently in the flash
    NewData - the data to be written

Return Value:

    TRUE if we can safely overwrite
    FALSE if we cannot safely overwrite

--*/
{
    return ((NewData & ~OldData) == 0) ? TRUE: FALSE;
}

ULONG
AmdFlash_SectorSize(
    IN PUCHAR FlashOffset
    )
/*++

Routine Description:

    This function returns the size of the sector in which
    FlashOffset lies.

Arguments:

    FlashOffset - offset within the flash

Return Value:

    The size of the specified sector - 0 if FlashOffset is
    out of range.

--*/
{
  PAMD_SECTOR_INFORMATION pLayout;
  ULONG Size = 0;
  int i;

  //
  // Get the sector information
  //
  pLayout = DriverInformation.pFlashDevice->Layout;

  //
  // Loop throught the layout structure and find the 
  // desired sector
  //
  for(i = 0; pLayout[i].Size != 0; i++) {
    if (((uintptr_t)FlashOffset >= pLayout[i].Offset) &&
        ((uintptr_t)FlashOffset < (pLayout[i].Offset + pLayout[i].Size))) {
      //
      // FlashOffset falls in the current sector, remember the size
      //
      Size = pLayout[i].Size;
      break;
    }
  }

  return Size;
}

ULONG
AmdFlash_GetLastError(
    VOID
    )
/*++

Routine Description:

    This function returns the last driver-defined error 
    condition encountered by the driver.

Arguments:

    None

Return Value:

    None

--*/
{
  return DriverInformation.LastError;
}
