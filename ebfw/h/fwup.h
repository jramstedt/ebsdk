#ifndef __FWUP_H_LOADED
#define __FWUP_H_LOADED
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

/*++

Module Name:

    $Id: fwup.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $

Description:

    Adapted version of fwup.h from NT firmware update tool.

Revision History:

    $Log: fwup.h,v $
    Revision 1.1.1.1  1998/12/29 21:36:06  paradis
    Initial CVS checkin

 * Revision 1.5  1995/10/26  23:49:49  cruz
 * Fixed up argument type to WriteFlashUCHAR and added casting
 * to a macro.
 *
 * Revision 1.4  1995/10/03  20:30:54  cruz
 * Added prototypes for global functions.
 *
 * Revision 1.3  1995/10/03  03:27:39  fdh
 * Replaced some prototypes with the appropriate include file.
 *
 * Revision 1.2  1995/07/18  20:11:28  cruz
 * Added macro definition for getting the last error.
 *
 * Revision 1.1  1995/07/05  17:58:38  cruz
 * Initial revision
 *

--*/

#include "nttypes.h"
#include "halp.h"
#include "arccodes.h"
#include "flash8k.h"

#define VenWritePortUCHAR WRITE_PORT_UCHAR
#define VenWriteRegisterUCHAR WRITE_REGISTER_UCHAR
#define VenReadRegisterUCHAR  READ_REGISTER_UCHAR
#define VenWriteRegisterULONG WRITE_REGISTER_ULONG
#define VenReadRegisterULONG  READ_REGISTER_ULONG

#define FwUpdateSetHAE SetHAE
#define VenPrint printf
#define VenPrint1 printf

#undef ASSERT
#ifdef FWUPDATE_DEBUG
#define ASSERT(exp)  {  \
    if (!(#exp)) {      \
        VenPrint2("ASSERT File: %s line: %d\r\n", __FILE__, __LINE__); \
        FwuWaitForKeypress(FALSE); \
    }}
#else // FWUPDATE_DEBUG
#define ASSERT(exp)
#endif // FWUPDATE_DEBUG

extern PFLASH_DRIVER FwUpdateFlashDriver;

#define FWUP_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define FWUP_MIN(a, b) (((a) > (b)) ? (b) : (a))


//
// The following macros define the Flash*() functions in terms
// of the FwUpdateFlashDriver variable.  FwUpdateFlashDriver points to 
// a structure containing information about the currently active flash driver.
// Information in the structure pointed to by FwUpdateFlashDriver includes
// functions within the driver to perform all device-dependent operations.
//

#define FlashSetReadMode(boffset)                       \
        FwUpdateFlashDriver->SetReadModeFunction(       \
                (PUCHAR)(boffset))

#define FlashReadByte(boffset)                          \
        FwUpdateFlashDriver->ReadByteFunction(          \
                (PUCHAR)(boffset))

#define FlashWriteByte(boffset, data)                   \
        FwUpdateFlashDriver->WriteByteFunction(         \
                (PUCHAR)(boffset), (UCHAR)((data) & 0xff))

#define FlashOverwriteCheck(olddata, newdata)           \
        FwUpdateFlashDriver->OverwriteCheckFunction(    \
                (olddata) & 0xff, (newdata) & 0xff)

#define FlashEraseBlock(boffset)                        \
        FwUpdateFlashDriver->EraseBlockFunction(        \
                (PUCHAR)(boffset))

#define FlashBlockAlign(boffset)                        \
        FwUpdateFlashDriver->BlockAlignFunction(        \
                (PUCHAR)(boffset))

#define FlashBlockSize(boffset)                         \
        FwUpdateFlashDriver->BlockSizeFunction(         \
                (PUCHAR)(boffset))

#define FlashCheckStatus(boffset, operation)            \
        FwUpdateFlashDriver->CheckStatusFunction(       \
                (PUCHAR)(boffset), (operation))

#define FlashGetDeviceSize()                            \
        FwUpdateFlashDriver->DeviceSize

#define FlashGetLastError()                            \
        FwUpdateFlashDriver->GetLastErrorFunction()

//
// global variables common to all platforms
//
extern PUCHAR FlashBaseAddressQva;
extern PUCHAR FlashDenseBaseQva;
extern ULONG FlashWindowSize;
extern ULONG FlashAccessHaeValue;
extern PUCHAR FlashAddress19PortQva;

//
// Functions which need to be defined for the platform
//
extern ULONG FwUpdateSetHAE(ULONG Value);

//
// External function prototypes.
//
extern ARC_STATUS FindFlashDevice(VOID);
extern ARC_STATUS EraseFlashBlock(IN ULONG Offset);
extern ARC_STATUS WriteFlashByte(IN ULONG Offset, IN UCHAR Value);
extern VOID WriteFlashUCHAR(IN ULONG Offset, IN UCHAR Value);
extern UCHAR ReadFlashUCHAR(IN ULONG Offset);

#endif /* __FWUP_H_LOADED */
