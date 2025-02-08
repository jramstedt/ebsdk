/*****************************************************************************

Copyright 1994, 1995 Digital Equipment Corporation, Maynard, Massachusetts. 

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
__attribute__((unused)) static const char *rcsid = "$Id: 29f040.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*++

Purpose: Firmware/HAL driver for AMD Am29F040 512kB flash device.
    
--*/

#include "system.h"
#ifdef NEEDFLASHMEMORY
#include "halp.h"
#include "arccodes.h"
#include "flash8k.h"

#define Am29F040_DEVICE_SIZE        0x80000
#define Am29F080_DEVICE_SIZE        0x100000

#define Am29F040_DEVICE_NAME        "AMD Am29F040"
#define Am29F080_DEVICE_NAME        "AMD Am29F080"

#define COMMAND_OFFSET              0x5555

#define COMMAND_PREFIX_OFFSET1      (COMMAND_OFFSET)
#define COMMAND_PREFIX_COMMAND1     0xaa

#define COMMAND_PREFIX_OFFSET2      0x2aaa
#define COMMAND_PREFIX_COMMAND2     0x55

#define SET_READ_ARRAY      0xf0    // write to COMMAND_ADDRESS
#define SET_BYTE_WRITE      0xa0    // write to COMMAND_ADDRESS
#define SET_ERASE           0x80    // write to COMMAND_ADDRESS
#define CONFIRM_ERASE_BLOCK 0x30    // write to the base address of the block
#define ID_REQUEST          0x90    // write to COMMAND_ADDRESS

#define MANUFACTURER_ID     0x01
#define Am29F040_DEVICE_ID  0xa4
#define Am29F080_DEVICE_ID  0xd5

#define BLOCK_SIZE          0x10000
#define BLANK_DATA          0xff

#define STATUS_TIMEOUT      5000000

#define COMMAND_PREFIX(address) {                                              \
    PUCHAR PrefixAddress1;                                                     \
    PUCHAR PrefixAddress2;                                                     \
    PrefixAddress1 = PrefixAddress2 = Am29F040_BlockAlign(address);            \
    PrefixAddress1 = (PUCHAR)((ULONG)PrefixAddress1 + COMMAND_PREFIX_OFFSET1); \
    PrefixAddress2 = (PUCHAR)((ULONG)PrefixAddress2 + COMMAND_PREFIX_OFFSET2); \
    WRITE_CONFIG_RAM_DATA(PrefixAddress1, COMMAND_PREFIX_COMMAND1);            \
    HalpMb();                                                                  \
    WRITE_CONFIG_RAM_DATA(PrefixAddress2, COMMAND_PREFIX_COMMAND2);            \
    HalpMb();                                                                  \
    }

#define COMMAND_ADDRESS(address) \
    ((PUCHAR)(((ULONG)(BLOCK_ALIGN(address)) + (COMMAND_OFFSET))))

#define BLOCK_ALIGN(address) \
    ((PUCHAR)((ULONG)(address) & ~(BLOCK_SIZE-1)))

//
// Local function prototypes
//
static
ARC_STATUS
Am29F040_SetReadMode(
    IN PUCHAR Address
    );

static
ARC_STATUS
Am29F040_WriteByte(
    IN PUCHAR Address,
    IN UCHAR Data
    );

static
ARC_STATUS
Am29F040_EraseBlock(
    IN PUCHAR Address
    );

static
ARC_STATUS
Am29F040_CheckStatus(
    IN PUCHAR Address,
    IN UCHAR Data,
    IN FLASH_OPERATIONS Operation
    );

static
PUCHAR
Am29F040_BlockAlign(
    IN PUCHAR Address
    );

static
UCHAR
Am29F040_ReadByte(
    IN PUCHAR Address
    );

static
BOOLEAN
Am29F040_OverwriteCheck(
    IN UCHAR OldData,
    IN UCHAR NewData
    );

static
ULONG
Am29F040_BlockSize(
    IN PUCHAR Address
    );

static
ULONG
Am29F040_GetLastError(
    VOID
    );

static
VOID
Am29F040_SetLastError(
    ULONG FlashStatus
    );

FLASH_DRIVER Am29F040_DriverInformation = {
    Am29F040_DEVICE_NAME,
    Am29F040_SetReadMode,      // SetReadModeFunction
    Am29F040_WriteByte,        // WriteByteFunction
    Am29F040_EraseBlock,       // EraseBlockFunction
    Am29F040_BlockAlign,       // AlignBlockFunction
    Am29F040_ReadByte,         // ReadByteFunction
    Am29F040_OverwriteCheck,   // OverwriteCheckFunction
    Am29F040_BlockSize,        // BlockSizeFunction
    Am29F040_GetLastError,     // GetLastErrorFunction
    Am29F040_DEVICE_SIZE,      // DeviceSize
    BLANK_DATA                 // ErasedData
    };

FLASH_DRIVER Am29F080_DriverInformation = {
    Am29F080_DEVICE_NAME,
    Am29F040_SetReadMode,      // SetReadModeFunction
    Am29F040_WriteByte,        // WriteByteFunction
    Am29F040_EraseBlock,       // EraseBlockFunction
    Am29F040_BlockAlign,       // AlignBlockFunction
    Am29F040_ReadByte,         // ReadByteFunction
    Am29F040_OverwriteCheck,   // OverwriteCheckFunction
    Am29F040_BlockSize,        // BlockSizeFunction
    Am29F040_GetLastError,     // GetLastErrorFunction
    Am29F080_DEVICE_SIZE,      // DeviceSize
    BLANK_DATA                 // ErasedData
    };


static ULONG Am29F040_LastError;

PFLASH_DRIVER
Am29F040_Initialize(
    IN PUCHAR NvRamPtr
    )
{
    PFLASH_DRIVER ReturnDriver = NULL;
    UCHAR ManufacturerID;
    UCHAR DeviceID;

    NvRamPtr = Am29F040_BlockAlign(NvRamPtr);

    COMMAND_PREFIX(NvRamPtr);
    WRITE_CONFIG_RAM_DATA(COMMAND_ADDRESS(NvRamPtr), ID_REQUEST);

    ManufacturerID = READ_CONFIG_RAM_DATA(NvRamPtr);
    DeviceID = READ_CONFIG_RAM_DATA((PUCHAR)((ULONG)NvRamPtr + 1));

    if ((ManufacturerID == MANUFACTURER_ID) && 
        (DeviceID == Am29F040_DEVICE_ID)) {

        Am29F040_LastError = 0;
        Am29F040_SetReadMode(NvRamPtr);
        ReturnDriver = &Am29F040_DriverInformation;
    }

    return ReturnDriver;
}

PFLASH_DRIVER
Am29F080_Initialize(
    IN PUCHAR NvRamPtr
    )
{
    PFLASH_DRIVER ReturnDriver = NULL;
    UCHAR ManufacturerID;
    UCHAR DeviceID;

    NvRamPtr = Am29F040_BlockAlign(NvRamPtr);

    COMMAND_PREFIX(NvRamPtr);
    WRITE_CONFIG_RAM_DATA(COMMAND_ADDRESS(NvRamPtr), ID_REQUEST);

    ManufacturerID = READ_CONFIG_RAM_DATA(NvRamPtr);
    DeviceID = READ_CONFIG_RAM_DATA((PUCHAR)((ULONG)NvRamPtr + 1));

    if ((ManufacturerID == MANUFACTURER_ID) && 
        (DeviceID == Am29F080_DEVICE_ID)) {

        Am29F040_LastError = 0;
        Am29F040_SetReadMode(NvRamPtr);
        ReturnDriver = &Am29F080_DriverInformation;
    }

    return ReturnDriver;
}

static
ARC_STATUS
Am29F040_SetReadMode(
    IN PUCHAR NvRamPtr
    )
{
    COMMAND_PREFIX(NvRamPtr);
    WRITE_CONFIG_RAM_DATA(COMMAND_ADDRESS(NvRamPtr), SET_READ_ARRAY);

    return ESUCCESS;
}

static
ARC_STATUS
Am29F040_WriteByte(
    IN PUCHAR NvRamPtr, 
    IN UCHAR Data
    )
{
    ARC_STATUS ReturnStatus;

    Am29F040_SetReadMode(NvRamPtr);

    COMMAND_PREFIX(NvRamPtr);
    WRITE_CONFIG_RAM_DATA(COMMAND_ADDRESS(NvRamPtr), SET_BYTE_WRITE);
    WRITE_CONFIG_RAM_DATA(NvRamPtr, Data);

    ReturnStatus = Am29F040_CheckStatus(NvRamPtr, Data, FlashByteWrite);

    Am29F040_SetReadMode(NvRamPtr);

    return ReturnStatus;
}

static
ARC_STATUS
Am29F040_EraseBlock (
    IN PUCHAR NvRamPtr
    )
{
    ARC_STATUS ReturnStatus;
    
    NvRamPtr = Am29F040_BlockAlign(NvRamPtr);

    COMMAND_PREFIX(NvRamPtr);
    WRITE_CONFIG_RAM_DATA(COMMAND_ADDRESS(NvRamPtr), SET_ERASE);
    COMMAND_PREFIX(NvRamPtr);
    WRITE_CONFIG_RAM_DATA(COMMAND_ADDRESS(NvRamPtr), CONFIRM_ERASE_BLOCK);

    ReturnStatus = Am29F040_CheckStatus(NvRamPtr, (UCHAR) 0xff, FlashEraseBlock);
   
    Am29F040_SetReadMode(NvRamPtr);

    return ReturnStatus;
}

static
ARC_STATUS
Am29F040_CheckStatus (
    IN PUCHAR NvRamPtr,
    IN UCHAR Data,
    IN FLASH_OPERATIONS Operation
    )
{
    ARC_STATUS ReturnStatus = EIO;
    UCHAR FlashStatus;
    ULONG Timeout;

#if 0
    BOOLEAN StopReading = FALSE;
    //
    // Keep reading the status until the device is done with its
    // current operation.
    //
    do {
        FlashStatus = READ_CONFIG_RAM_DATA(NvRamPtr);
        if (FlashStatus & 0x20) {
            StopReading = TRUE;
            FlashStatus = READ_CONFIG_RAM_DATA(NvRamPtr);
        }
    } while (!StopReading && (((FlashStatus ^ Data) & 0x80) != 0));

    if ((FlashStatus ^ Data) != 0) {
        ReturnStatus = ESUCCESS;
    } else {
        ReturnStatus = EIO;
    }

    return ReturnStatus;
#else
    //
    // If we are doing an erase command, check whether the erase command
    // was accepted.  This is supposed to happen within the first 100 usec.
    //

    if (Operation == FlashEraseBlock) {
        Timeout = STATUS_TIMEOUT;
    
        while (((READ_CONFIG_RAM_DATA(NvRamPtr) & 0x08) != 0x08) && 
               (Timeout != 0)) {
            KeStallExecutionProcessor(1);
            Timeout--;
        }
    
        if (Timeout == 0) {
            ReturnStatus = EIO;
            Am29F040_SetLastError(0);
            return ReturnStatus;
        }
    }
    
    //
    // Now wait for the actual status
    //
    Timeout = STATUS_TIMEOUT;
    do {
        FlashStatus = READ_CONFIG_RAM_DATA(NvRamPtr);

        if (((FlashStatus ^ Data) & 0x80) == 0) {
            ReturnStatus = ESUCCESS;
            break;
        }

        if (FlashStatus & 0x20) {
            if (((READ_CONFIG_RAM_DATA(NvRamPtr) ^ Data) & 0x80) == 0) {
                ReturnStatus = ESUCCESS;
                break;
            } else {
                ReturnStatus = EIO;
                Am29F040_SetLastError(Operation | 0x80000000);
                break;
            }
        }
        KeStallExecutionProcessor(1);
        Timeout--;
    } while (Timeout != 0);

    if (Timeout == 0) {
        ReturnStatus = EIO;
        Am29F040_SetLastError(0);
    }

    return ReturnStatus;
#endif
}

static
PUCHAR
Am29F040_BlockAlign(
    IN PUCHAR Address
    )
{
    return BLOCK_ALIGN(Address);
}

static
UCHAR
Am29F040_ReadByte(
    IN PUCHAR Address
    )
{
    return READ_CONFIG_RAM_DATA(Address);
}

static
BOOLEAN
Am29F040_OverwriteCheck(
    IN UCHAR OldData,
    IN UCHAR NewData
    )
/*++

Return Value:

    TRUE if OldData can be overwritten with NewData.
    FALSE if OldData must be erased before writing NewData.

--*/
{
    return (~OldData & NewData) ? FALSE : TRUE;
}

static
ULONG 
Am29F040_BlockSize(
    IN PUCHAR Address
    )
{
    return BLOCK_SIZE;
}

static
ULONG
Am29F040_GetLastError(
    VOID
    )
{
    return Am29F040_LastError;
}

static
VOID
Am29F040_SetLastError(
    ULONG FlashStatus
    )
{
    Am29F040_LastError = ERROR_UNKNOWN;
    if (FlashStatus == 0) {
        Am29F040_LastError = ERROR_TIMEOUT;
    } else {
        switch(FlashStatus & ~0x80000000) {
        case FlashByteWrite:
            Am29F040_LastError = ERROR_WRITE_ERROR;
            break;
        case FlashEraseBlock:
            Am29F040_LastError = ERROR_ERASE_ERROR;
            break;
        }
    }
}

#endif /* NEEDFLASHMEMORY */
