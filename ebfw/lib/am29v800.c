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

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: am29v800.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 * $Log: am29v800.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1997/05/02  20:18:53  fdh
 * Moved the prototype for the init function.
 *
 * Revision 1.1  1997/05/01  20:41:34  pbell
 * Initial revision
 *
 */

#include "system.h"
#ifdef NEEDFLASHMEMORY
#include "halp.h"
#include "arccodes.h"
#include "flash8k.h"
#include "am29v800.h"

//
// External prototypes
//

VOID pWriteFlashByte(
    IN ULONG FlashOffset,
    IN UCHAR Data
    );

UCHAR pReadFlashByte(
    IN ULONG FlashOffset
    );

//
// Local function prototypes
//

static 
ARC_STATUS
Am29LV800_SetReadMode(
    IN PUCHAR FlashOffset
    );

static 
ARC_STATUS
Am29LV800_WriteByte(
    IN PUCHAR FlashOffset,
    IN UCHAR Data
    );

static 
ARC_STATUS
Am29LV800_EraseSector(
    IN PUCHAR FlashOffset
    );

static 
ARC_STATUS
Am29LV800_CheckStatus(
    IN PUCHAR FlashOffset,
    IN FLASH_OPERATIONS Operation,
    IN UCHAR OptionalData
    );

static 
PUCHAR
Am29LV800_SectorAlign(
    IN PUCHAR FlashOffset
    );

static 
UCHAR
Am29LV800_ReadByte(
    IN PUCHAR FlashOffset
    );

static 
BOOLEAN
Am29LV800_OverwriteCheck(
    IN UCHAR OldData,
    IN UCHAR NewData
    );

static 
ULONG
Am29LV800_SectorSize(
    IN PUCHAR FlashOffset
    );

static 
ULONG
Am29LV800_GetLastError(
    VOID
    );

FLASH_DRIVER Am29LV800_DriverInformation = {
    "AMD Am29LV800",
    Am29LV800_SetReadMode,       // SetReadModeFunction
    Am29LV800_WriteByte,         // WriteByteFunction
    Am29LV800_EraseSector,       // EraseBlockFunction
    Am29LV800_SectorAlign,       // BlockAlignFunction
    Am29LV800_ReadByte,          // ReadByteFunction
    Am29LV800_OverwriteCheck,    // OverwriteCheckFunction
    Am29LV800_SectorSize,        // BlockSizeFunction
    Am29LV800_GetLastError,      // Get last error
    (Am29LV800_DEVICE_SIZE - 1), // DeviceSize (max address)
    Am29LV800_ERASED_DATA        // What an erased data looks like
    };

//
// Keep track of the last error
//
static ULONG Am29LV800_LastError;


static UCHAR am29LV800_GetSectorNumber(
    IN PUCHAR FlashOffset
    )
/*++

Routine Description:

    This routine figures out which sector number we are in (for use in 
    clearing the sector...)

Arguments:

    FlashOffset   - offset within the flash ROM.

Return Value:

    The sector number within the range of the offset.

--*/
{
    uintptr_t Offset = (uintptr_t)FlashOffset;

    if (Offset < SECTOR_2_BASE) {
        return 0;
    } else if (Offset < SECTOR_3_BASE) {
        return 1;
    } else if (Offset < SECTOR_4_BASE) {
        return 2;
    } else if (Offset < SECTOR_5_BASE) {
        return 3;
    }

    //
    // All other sectors are 0x10000, 0x20000, etc, so shift right by 64K,
    // Then add the preceding section offset's
    //
    
    return (UCHAR)((Offset >> 16) + 3); 
}


PFLASH_DRIVER
Am29LV800_Initialize(
    IN PUCHAR FlashOffset
    )
/*++

Routine Description:

    This routine spanks the FLASH ROM with the auto select command, which
    allows us to make sure that the device is what we think it is. 

Arguments:

    FlashOffset   - offset within the flash ROM.

Return Value:

    NULL for no driver, or the address of the FLASH_DRIVER structure for
    this driver.

--*/
{
    PFLASH_DRIVER ReturnDriver = NULL;
    UCHAR ManufacturerID;
    UCHAR DeviceID;

    Am29LV800_SetReadMode((PUCHAR)SECTOR_2_BASE); // first 8K section (NVRAM)
 
    KeStallExecutionProcessor(50); // wkc -- GACK! what a bogus part

    pWriteFlashByte(COMMAND_ADDR1, COMMAND_DATA1);
    pWriteFlashByte(COMMAND_ADDR2, COMMAND_DATA2);
    pWriteFlashByte(COMMAND_ADDR3, COMMAND_DATA3_AUTOSELECT);

    //
    // Get manufacturer and device ID. Note spec says device ID lives
    // at byte 1, but empirical evidence says it is at byte 2.
    //
    //[wem] The 0x02 offset for device ID is true on some platforms.
    // On Miata, the reason provided by Jim Delmonico is as follows:
    //
    //      We are using the part in byte mode rather than
    //      word mode. Therefore we are connecting
    //      <A18...A2, A1, A0, *&* A-1).  The 'A-1' gives us 
    //      the extra address bit to get byte mode from the 
    //      device.  That means all the autoselect addresses
    //      need to be shifted one bit left to be valid.
    //      The autoselect mode is the only mode affected by this. 
    //      Program/erase cycles are not affected.
    //

    ManufacturerID = Am29LV800_ReadByte((PUCHAR)0x00);
    DeviceID = Am29LV800_ReadByte((PUCHAR)0x02);  // extra shift of address needed


    if ((ManufacturerID == MANUFACTURER_ID) && (DeviceID == DEVICE_ID)) {
        Am29LV800_SetReadMode(0x0);
        Am29LV800_SetReadMode((PUCHAR)SECTOR_2_BASE);

        ReturnDriver = &Am29LV800_DriverInformation;
    }

    return ReturnDriver;
}


static 
ARC_STATUS
Am29LV800_SetReadMode(
    IN PUCHAR FlashOffset
    )
/*++

Routine Description:

    This routine spanks the FLASH ROM with the read reset routine.

Arguments:

    FlashOffset   - offset within the flash ROM -- but not used.

Return Value:

    The value of the check status routine (EIO or ESUCCESS)

--*/
{

    pWriteFlashByte((uintptr_t)FlashOffset, COMMAND_READ_RESET);
    return ESUCCESS;
}


static 
ARC_STATUS
Am29LV800_WriteByte(
    IN PUCHAR FlashOffset, 
    IN UCHAR Data
    )
/*++

Routine Description:

    This routine spanks the FLASH ROM with the program command, then calls
    the check status routine -- then resets the device to read.

Arguments:

    FlashOffset   - offset within the flash ROM.
    Data          - the data byte to write.

Return Value:

    The value of the check status routine (EIO or ESUCCESS)

--*/
{
    ARC_STATUS ReturnStatus;

    pWriteFlashByte(COMMAND_ADDR1, COMMAND_DATA1);
    pWriteFlashByte(COMMAND_ADDR2, COMMAND_DATA2);
    pWriteFlashByte(COMMAND_ADDR3, COMMAND_DATA3_PROGRAM);

    pWriteFlashByte((uintptr_t)FlashOffset, Data);

    ReturnStatus = Am29LV800_CheckStatus(FlashOffset, FlashByteWrite, Data);

    Am29LV800_SetReadMode(FlashOffset);

    return ReturnStatus;
}


static 
ARC_STATUS
Am29LV800_EraseSector(
    IN PUCHAR FlashOffset
    )
/*++

Routine Description:

    This routine spanks the FLASH ROM with the erase sector command, then calls
    the check status routine

Arguments:

    FlashOffset   - offset within the flash ROM.

Return Value:

    The value of the check status routine (EIO or ESUCCESS)

--*/
{
    ARC_STATUS ReturnStatus;
    ULONG Count =  MAX_FLASH_READ_ATTEMPTS;

    Am29LV800_SetReadMode(FlashOffset);

    pWriteFlashByte(COMMAND_ADDR1, COMMAND_DATA1);
    pWriteFlashByte(COMMAND_ADDR2, COMMAND_DATA2);
    pWriteFlashByte(COMMAND_ADDR3, COMMAND_DATA3);
    pWriteFlashByte(COMMAND_ADDR4, COMMAND_DATA4);
    pWriteFlashByte(COMMAND_ADDR5, COMMAND_DATA5);
    pWriteFlashByte((uintptr_t)FlashOffset, COMMAND_DATA6_SECTOR_ERASE);

    KeStallExecutionProcessor(80); // short stall for erase command to take

    ReturnStatus = Am29LV800_CheckStatus(FlashOffset, 
                                        FlashEraseBlock, 
                                        0xff);
   
    Am29LV800_SetReadMode(FlashOffset);

    return ReturnStatus;

}


static 
ARC_STATUS
Am29LV800_CheckStatus (
    IN PUCHAR FlashOffset,
    IN FLASH_OPERATIONS Operation,
    IN UCHAR OptionalData
    )
/*++

Routine Description:

    This routine checks the status of the flashbus operation. The operation
    may be specific, so different actions may be taken.

Arguments:

    FlashOffset   - offset within the flash ROM.
    Opertaion     - the operation performed on the FlashOffset.
    OptionalData  - an optional data value that may be needed for verification.

Return Value:

    EIO for faiure
    ESUCCESS for success

--*/
{
    ARC_STATUS ReturnStatus = EIO;
    UCHAR FlashRead;
    ULONG Count = 0;
    ULONG DataOK = FALSE;

#ifdef MISP
    DataOK = TRUE;          // simulator doesn't do status -- always succeed
#endif

    while ((DataOK == FALSE) && (Count < MAX_FLASH_READ_ATTEMPTS)) {

        FlashRead = Am29LV800_ReadByte(FlashOffset);

        //
        // Both FlashByteWrite & FlashEraseBlock checks use polling 
        // algorithm found on page 1-131 of the AMD part specification
        //

        if ((FlashRead & 0x80) == (OptionalData & 0x80)) {
            DataOK = TRUE;
        } else if (FlashRead & 0x20) {
            FlashRead = Am29LV800_ReadByte(FlashOffset);
            if ((FlashRead & 0x80) == (OptionalData & 0x80)) {
                DataOK = TRUE;
            } else {
                break;
            }
        } 
        Count++;
    } 

    if (DataOK == TRUE) {
        ReturnStatus = ESUCCESS;
    }

    return ReturnStatus;
}


static 
PUCHAR
Am29LV800_SectorAlign(
    IN PUCHAR FlashOffset
    )
/*++

Routine Description:

    This routine returns the base offset that the current offset within
    a sector of the flash ROM.

Arguments:

    FlashOffset   - offset within the flash ROM.

Return Value:

    The base offset of the current sector -- as defined by the FlashOffset.

Notes:

    Sector size varies across the Am29LV800's address space:

        Sector  1    - 16KB
        Sectors 2, 3 - 8KB
        Sector  4    - 32KB
        Sectors 5-19 - 64KB

--*/
{
    uintptr_t Offset = (uintptr_t)FlashOffset;

    if (Offset < SECTOR_2_BASE) {
        return (PUCHAR)SECTOR_1_BASE;
    } else if (Offset < SECTOR_3_BASE) {
        return (PUCHAR)SECTOR_2_BASE;
    } else if (Offset < SECTOR_4_BASE) {
        return (PUCHAR)SECTOR_3_BASE;
    } else if (Offset < SECTOR_5_BASE) {
        return (PUCHAR)SECTOR_4_BASE;
    }

    return (PUCHAR)(Offset & ~(SECTOR_5_SIZE-1));
}


static 
UCHAR
Am29LV800_ReadByte(
    IN PUCHAR FlashOffset
    )
/*++

Routine Description:

    This routine spanks the FLASH ROM with the read command, then calls
    the read flash bus function.

Arguments:

    FlashOffset   - offset within the flash ROM.


Return Value:

    The character at the appropriate location.

--*/
{
    UCHAR ReturnVal;

    // 
    // Assume Read mode is on
    //

    ReturnVal = pReadFlashByte((uintptr_t)FlashOffset);

    return ReturnVal;
}


static 
BOOLEAN
Am29LV800_OverwriteCheck(
    IN UCHAR OldData,
    IN UCHAR NewData
    )
/*++

Routine Description:

    This routine returns if we can safely overwrite an existing data
    with new data. Flash Rom's can go from 1-->1, 1-->0 and 0-->0, but
    cannot go from 0-->1.

Return Value:


    TRUE if we can safely overwrite
    FALSE if we cannot safely overwrite

--*/
{
    BOOLEAN OverwriteOk;

    OverwriteOk = ((NewData & ~OldData) == 0) ? TRUE: FALSE;

    return OverwriteOk;
}


static 
ULONG
Am29LV800_SectorSize(
    IN PUCHAR FlashOffset
    )
/*++

Routine Description:

    This routine returns the size of the sector that the offset is within.

Arguments:

    FlashOffset   - offset within the flash ROM.


Return Value:

    The block size of the sector.  

--*/
{
    if (FlashOffset < (PUCHAR)SECTOR_2_BASE) {
        return SECTOR_1_SIZE;
    } else if (FlashOffset < (PUCHAR)SECTOR_3_BASE) {
        return SECTOR_2_SIZE;
    } else if (FlashOffset < (PUCHAR)SECTOR_4_BASE) {
        return SECTOR_3_SIZE;
    } else if (FlashOffset < (PUCHAR)SECTOR_5_BASE) {
        return SECTOR_4_SIZE;
    }

    return SECTOR_5_SIZE;
}

static 
ULONG
Am29LV800_GetLastError(
    VOID
    )
{
  return ERROR_UNKNOWN;
}

#endif /* NEEDFLASHMEMORY */

