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
__attribute__((unused)) static const char *rcsid = "$Id: 28f008sa.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

#include "system.h"
#ifdef NEEDFLASHMEMORY
#include "halp.h"
#include "arccodes.h"
#include "flash8k.h"

#define I28F008SA_DEVICE_SIZE   0x100000

#define SET_READ_ARRAY      0xff
#define SET_BYTE_WRITE      0x40
#define SET_ERASE_BLOCK     0x20
#define CONFIRM_ERASE_BLOCK 0xd0
#define READ_STATUS         0x70
#define RESET_STATUS        0x50
#define ID_REQUEST          0x90

#define MANUFACTURER_ID     0x89
#define DEVICE_ID           0xa2

#define BLOCK_SIZE          0x10000
#define BLANK_DATA          0xff

#define STATUS_TIMEOUT      5000000

#define STATUS_READY        0x80
#define STATUS_ERASE_SUSP   0x40
#define STATUS_ERASE_ERR    0x20
#define STATUS_WRITE_ERR    0x10
#define STATUS_VPP_LOW      0x08
#define STATUS_CMD_SEQ_ERR  (STATUS_WRITE_ERR | STATUS_READ_ERR)

//
// Local function prototypes
//

static 
ARC_STATUS
I28F008SA_SetReadMode(
    IN PUCHAR Address
    );

static 
ARC_STATUS
I28F008SA_WriteByte(
    IN PUCHAR Address,
    IN UCHAR Data
    );

static 
ARC_STATUS
I28F008SA_EraseBlock(
    IN PUCHAR Address
    );

static 
ARC_STATUS
I28F008SA_CheckStatus(
    IN PUCHAR Address,
    FLASH_OPERATIONS Operation
    );

static 
PUCHAR
I28F008SA_BlockAlign(
    IN PUCHAR Address
    );

static 
UCHAR
I28F008SA_ReadByte(
    IN PUCHAR Address
    );

static 
BOOLEAN
I28F008SA_OverwriteCheck(
    IN UCHAR OldData,
    IN UCHAR NewData
    );

static 
ULONG
I28F008SA_BlockSize(
    IN PUCHAR Address
    );

static 
ULONG
I28F008SA_GetLastError(
    VOID
    );

static 
VOID
I28F008SA_SetLastError(
    ULONG FlashStatus
    );

FLASH_DRIVER I28F008SA_DriverInformation = {
    "Intel 28F008SA",
    I28F008SA_SetReadMode,      // SetReadModeFunction
    I28F008SA_WriteByte,        // WriteByteFunction
    I28F008SA_EraseBlock,       // EraseBlockFunction
    I28F008SA_BlockAlign,       // AlignBlockFunction
    I28F008SA_ReadByte,         // ReadByteFunction
    I28F008SA_OverwriteCheck,   // OverwriteCheckFunction
    I28F008SA_BlockSize,        // BlockSizeFunction
    I28F008SA_GetLastError,     // GetLastErrorFunction
    I28F008SA_DEVICE_SIZE,      // DeviceSize
    BLANK_DATA                  // ErasedData
    };

static ULONG I28F008SA_LastError;

PFLASH_DRIVER
I28F008SA_Initialize(
    IN PUCHAR NvRamPtr
    )
{
    PFLASH_DRIVER ReturnDriver = NULL;
    UCHAR ManufacturerID;
    UCHAR DeviceID;

    NvRamPtr = I28F008SA_BlockAlign(NvRamPtr);

    WRITE_CONFIG_RAM_DATA(NvRamPtr, ID_REQUEST);

    ManufacturerID = READ_CONFIG_RAM_DATA(NvRamPtr);
    DeviceID = READ_CONFIG_RAM_DATA((PUCHAR)((uintptr_t)NvRamPtr + 1));

    if ((ManufacturerID == MANUFACTURER_ID) && (DeviceID == DEVICE_ID)) {
        I28F008SA_LastError = 0;
        I28F008SA_SetReadMode(NvRamPtr);
        ReturnDriver = &I28F008SA_DriverInformation;
    }

    return ReturnDriver;
}

static 
ARC_STATUS
I28F008SA_SetReadMode(
    IN PUCHAR NvRamPtr
    )
{
    WRITE_CONFIG_RAM_DATA(NvRamPtr, SET_READ_ARRAY);
    HalpMb();

    return ESUCCESS;
}

static 
ARC_STATUS
I28F008SA_WriteByte(
    IN PUCHAR NvRamPtr, 
    IN UCHAR Data
    )
{
    ARC_STATUS ReturnStatus;

    I28F008SA_SetReadMode(NvRamPtr);

    WRITE_CONFIG_RAM_DATA(NvRamPtr, SET_BYTE_WRITE);
    WRITE_CONFIG_RAM_DATA(NvRamPtr, Data);

    ReturnStatus = I28F008SA_CheckStatus(NvRamPtr, FlashByteWrite);

    I28F008SA_SetReadMode(NvRamPtr);

    return ReturnStatus;
}

static 
ARC_STATUS
I28F008SA_EraseBlock (
    IN PUCHAR NvRamPtr
    )
{
    ARC_STATUS ReturnStatus;

    WRITE_CONFIG_RAM_DATA(NvRamPtr, SET_ERASE_BLOCK);
    WRITE_CONFIG_RAM_DATA(NvRamPtr, CONFIRM_ERASE_BLOCK);

    ReturnStatus = I28F008SA_CheckStatus(NvRamPtr, FlashEraseBlock);
   
    I28F008SA_SetReadMode(NvRamPtr);

    return ReturnStatus;
}

static 
ARC_STATUS
I28F008SA_CheckStatus (
    IN PUCHAR NvRamPtr,
    IN FLASH_OPERATIONS Operation
    )
{
    ARC_STATUS ReturnStatus = EIO;
    ULONG Timeout;
    UCHAR FlashStatus;

    //
    // Keep reading the status until the device is done with its
    // current operation.
    //
    Timeout = STATUS_TIMEOUT;
    do {
        WRITE_CONFIG_RAM_DATA(NvRamPtr, READ_STATUS);
        FlashStatus = READ_CONFIG_RAM_DATA(NvRamPtr);
        KeStallExecutionProcessor(1);
        Timeout--;
    } while (((FlashStatus & 0x80) == 0) && (Timeout != 0));

    //
    // Check the status for the operation requested.
    //
    switch(Operation) {
    case FlashByteWrite:
        if ((FlashStatus & 0x18) == 0) {
            ReturnStatus = ESUCCESS;
        } else {
            I28F008SA_SetLastError(FlashStatus & 0x18);
        }
        break;
    case FlashEraseBlock:
        if (((FlashStatus & 0x28) == 0) && ((FlashStatus & 0x30) != 0x30)) {
            ReturnStatus = ESUCCESS;
        } else {
            I28F008SA_SetLastError(FlashStatus & 0x28);
        }
        break;
    }
    if ((FlashStatus & 0x80) == 0) {
        ReturnStatus = EIO;
        I28F008SA_SetLastError(0);
    }

    //
    // Clear the flash status register
    // This is a silent operation.  The status that gets returned is the
    // status of the real operation as determined above.
    //
    WRITE_CONFIG_RAM_DATA(NvRamPtr, RESET_STATUS);
    Timeout = STATUS_TIMEOUT;
    do {
        WRITE_CONFIG_RAM_DATA(NvRamPtr, READ_STATUS);
        FlashStatus = READ_CONFIG_RAM_DATA(NvRamPtr);
        KeStallExecutionProcessor(1);
        Timeout--;
    } while (((FlashStatus & 0x80) == 0) && (Timeout != 0));

    return ReturnStatus;
}

static 
PUCHAR
I28F008SA_BlockAlign(
    IN PUCHAR Address
    )
{
    return (PUCHAR)((uintptr_t)Address & ~(BLOCK_SIZE-1));
}

static 
UCHAR
I28F008SA_ReadByte(
    IN PUCHAR Address
    )
{
    return READ_CONFIG_RAM_DATA(Address);
}

static 
BOOLEAN
I28F008SA_OverwriteCheck(
    IN UCHAR OldData,
    IN UCHAR NewData
    )
/*++

Return Value:

    Zero if OldData can be overwritten with NewData.
    Non-zero if device must be erased to write NewData.

--*/
{
    return (~OldData & NewData) ? FALSE : TRUE;
}

static 
ULONG
I28F008SA_BlockSize(
    IN PUCHAR Address
    )
/*++

Return Value:

    The block size of the device.  This is a constant because all
    blocks in the 28f008sa are the same size.

--*/
{
    return BLOCK_SIZE;
}

static 
ULONG
I28F008SA_GetLastError(
    VOID
    )
{
    return I28F008SA_LastError;
}

static 
VOID
I28F008SA_SetLastError(
    ULONG FlashStatus
    )
{
    I28F008SA_LastError = ERROR_UNKNOWN;
    if (FlashStatus == 0) 
        I28F008SA_LastError = ERROR_TIMEOUT;
    if (FlashStatus & STATUS_WRITE_ERR)
        I28F008SA_LastError = ERROR_WRITE_ERROR;
    if (FlashStatus & STATUS_ERASE_ERR)
        I28F008SA_LastError = ERROR_ERASE_ERROR;
    if (FlashStatus & STATUS_VPP_LOW)
        I28F008SA_LastError = ERROR_VPP_LOW;
}

#endif /* NEEDFLASHMEMORY */
