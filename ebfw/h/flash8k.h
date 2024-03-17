// flash8k.h
#ifndef _FLASH8K_H_
#define _FLASH8K_H_
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

#ifdef FLASH8K_INCLUDE_FILES
#include "halp.h"
#include "arccodes.h"
#endif //FLASH8K_DONT_INCLUDE_FILES

//
//	The value of HalpCMOSRamBase must be set at initialization
//

typedef enum _FLASH_OPERATIONS {
  FlashByteWrite,
  FlashEraseBlock 
} FLASH_OPERATIONS, *PFLASH_OPERATIONS;

typedef struct _FLASH_DRIVER {
  PCHAR      DeviceName;
  ARC_STATUS (*SetReadModeFunction)(PUCHAR address);
  ARC_STATUS (*WriteByteFunction)(PUCHAR address, UCHAR data);
  ARC_STATUS (*EraseBlockFunction)(PUCHAR address);
  PUCHAR     (*BlockAlignFunction)(PUCHAR address);
  UCHAR      (*ReadByteFunction)(PUCHAR address);
  BOOLEAN    (*OverwriteCheckFunction)(UCHAR olddata, UCHAR newdata);
  ULONG      (*BlockSizeFunction)(PUCHAR address);
  ULONG      (*GetLastErrorFunction)(VOID);
  ULONG      DeviceSize;
  UCHAR      ErasedData;
} FLASH_DRIVER, *PFLASH_DRIVER;

extern PFLASH_DRIVER HalpFlashDriver;

#if (defined(DP264) || defined(CLUBS))
#define WRITE_CONFIG_RAM_DATA(boffset,data) \
	WRITE_FLASH_UCHAR((ul)boffset, (ub)data)
#define READ_CONFIG_RAM_DATA(boffset) \
	READ_FLASH_UCHAR ( (ul)boffset )
#else
#define WRITE_CONFIG_RAM_DATA(boffset,data) \
        WRITE_REGISTER_UCHAR((PUCHAR)(boffset),(UCHAR)((data) & 0xff))

#define READ_CONFIG_RAM_DATA(boffset) \
        READ_REGISTER_UCHAR((PUCHAR)(boffset))
#endif

//
// The following macros define the HalpFlash8k*() functions in terms
// of the FlashDriver variable.  FlashDriver points to a structure 
// containing information about the currently active flash driver.
// Information in the structure pointed to by FlashDriver includes
// functions within the driver to perform all device-dependent operations.
//

#define HalpFlash8kSetReadMode(boffset)             \
        HalpFlashDriver->SetReadModeFunction(       \
                (PUCHAR)((ULONG)(HalpCMOSRamBase) | \
                (ULONG)(boffset)))

#define HalpFlash8kReadByte(boffset)                \
        HalpFlashDriver->ReadByteFunction(          \
                (PUCHAR)((ULONG)(HalpCMOSRamBase) | \
                (ULONG)(boffset)))

#define HalpFlash8kWriteByte(boffset, data)         \
        HalpFlashDriver->WriteByteFunction(         \
                (PUCHAR)((ULONG)(HalpCMOSRamBase) | \
                (ULONG)(boffset)), ((data) & 0xff))

#define HalpFlash8kOverwriteCheck(olddata, newdata) \
        HalpFlashDriver->OverwriteCheckFunction(    \
                (olddata) & 0xff, (newdata) & 0xff)

#define HalpFlash8kEraseBlock(boffset)              \
        HalpFlashDriver->EraseBlockFunction(        \
                (PUCHAR)((ULONG)(HalpCMOSRamBase) | \
                (ULONG)(boffset)))

#define HalpFlash8kBlockAlign(boffset)              \
        HalpFlashDriver->BlockAlignFunction(        \
                (PUCHAR)((ULONG)(HalpCMOSRamBase) | \
                (ULONG)(boffset)))

#define HalpFlash8kBlockSize(boffset)               \
        HalpFlashDriver->BlockSizeFunction(         \
                (PUCHAR)((ULONG)(HalpCMOSRamBase) | \
                (ULONG)(boffset)))

#define HalpFlash8kCheckStatus(boffset, operation)  \
        HalpFlashDriver->CheckStatusFunction(       \
                (PUCHAR)((ULONG)(HalpCMOSRamBase) | \
                (ULONG)(boffset)), (operation))

#define HalpFlash8kGetLastError()                   \
        HalpFlashDriver->GetLastErrorFunction()

//
// Error codes for GetLastError()
//
#define ERROR_VPP_LOW       1
#define ERROR_ERASE_ERROR   2
#define ERROR_WRITE_ERROR   3
#define ERROR_TIMEOUT       4
#define ERROR_UNKNOWN       127

//
// External function prototypes
//
/*
// Supported Flash drivers init routines.
*/
extern PFLASH_DRIVER AmdFlash_Initialize(PUCHAR FlashOffset);
extern PFLASH_DRIVER I28F008SA_Initialize(PUCHAR NvRamPtr);
extern PFLASH_DRIVER Am29F040_Initialize(PUCHAR NvRamPtr);
extern PFLASH_DRIVER Am29F080_Initialize(PUCHAR NvRamPtr);
extern PFLASH_DRIVER Am29F400_Initialize(ULONG FlashOffset);
extern PFLASH_DRIVER Am29LV800_Initialize(PUCHAR FlashOffset);

#endif // _FLASH8K_H_
