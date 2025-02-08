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
__attribute__((unused)) static const char *rcsid = "$Id: flash.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*++

Description:

    This module implements primitives for flash access.

Revision History:

    $Log: flash.c,v $
    Revision 1.1.1.1  1998/12/29 21:36:10  paradis
    Initial CVS checkin

 * Revision 1.19  1997/05/01  20:45:24  pbell
 * *** empty log message ***
 *
 * Revision 1.18  1996/08/20  02:40:34  fdh
 * Added rcsid.
 *
 * Revision 1.17  1995/10/31  22:50:22  cruz
 * Added compile conditional NEEDFLASHMEMORY around code.
 *
 * Revision 1.16  1995/10/31  18:43:56  cruz
 * Commented out some unused routines.
 *
 * Revision 1.15  1995/10/26  23:46:36  cruz
 * Added casting and fixed up an argument type
 *
 * Revision 1.14  1995/10/03  20:24:17  cruz
 * Made a local function static.
 *
 * Revision 1.13  1995/10/03  03:18:49  fdh
 * Corrected data type mismatch.
 *
 * Revision 1.12  1995/07/05  18:17:03  cruz
 * This version of flash.c completely replaces the
 * previous versions.  It comes from the NT firmware kit and
 * should be kept identical to the one found there.
 *
 * Revision 1.1  1995/04/22  01:53:04  jeffw
 * Initial revision
 *
 * Revision 1.1  1995/03/20  14:31:41  jeffw
 * Initial revision
 *

--*/

#include "system.h"
#ifdef NEEDFLASHMEMORY
#include "fwup.h"

extern PFLASH_DRIVER (*FwUpdateFlashDriverList[])(PUCHAR);

static
ULONG
SetFlashAddress19 (
    IN ULONG Offset
    )
{

#ifndef	DISABLE_FLASH_ADDRESS19
    if (Offset >= FlashWindowSize) {   
        VenWritePortUCHAR(FlashAddress19PortQva, (UCHAR) 1);
        Offset -= FlashWindowSize;
    } else {
        VenWritePortUCHAR(FlashAddress19PortQva, (UCHAR) 0);
    }
#endif

    return Offset;
}

#ifdef NOT_IN_USE
ULONG
ReadFlashULONG (
    IN ULONG Offset
    )
{
    Offset = SetFlashAddress19(Offset);

    return VenReadRegisterULONG((PULONG)(FlashDenseBaseQva + Offset));
}
#endif

UCHAR
ReadFlashUCHAR (
    IN ULONG Offset
    )
{
#ifndef	NEEDPRIVATEFLASHBUS
    Offset = SetFlashAddress19(Offset);

    return VenReadRegisterUCHAR(FlashDenseBaseQva + Offset);
#else
    return pReadFlashByte(Offset);
#endif
}

VOID 
WriteFlashUCHAR (
    IN ULONG Offset,
    IN UCHAR Value
    )
{
#ifndef	NEEDPRIVATEFLASHBUS
    ULONG OldHae;

    Offset = SetFlashAddress19(Offset);
    OldHae = FwUpdateSetHAE(FlashAccessHaeValue);

    VenWriteRegisterUCHAR(FlashBaseAddressQva + Offset, Value);

    FwUpdateSetHAE(OldHae);
#else
    pWriteFlashByte(Offset, Value);
#endif
}

ARC_STATUS
WriteFlashByte (
    IN ULONG Offset,
    IN UCHAR Value
    )
{
    ULONG OldHae;
    ARC_STATUS ReturnStatus = EIO;

    Offset = SetFlashAddress19(Offset);
    OldHae = FwUpdateSetHAE(FlashAccessHaeValue);

    ReturnStatus = FlashWriteByte(FlashBaseAddressQva + Offset, Value);

    FwUpdateSetHAE(OldHae);
    return ReturnStatus;
}

ARC_STATUS
EraseFlashBlock (
    IN ULONG Offset
    )
{
    ULONG OldHae;
    ARC_STATUS ReturnStatus = EIO;

    Offset = SetFlashAddress19(Offset);
    OldHae = FwUpdateSetHAE(FlashAccessHaeValue);

    ReturnStatus = FlashEraseBlock(FlashBaseAddressQva + Offset);

    FwUpdateSetHAE(OldHae);
    return ReturnStatus;
}

ARC_STATUS
FindFlashDevice (
    VOID
    )
{
    ARC_STATUS ReturnStatus = ENODEV;
    ULONG DriverNumber;
    ULONG OldHaeValue;

    for(DriverNumber = 0;
        FwUpdateFlashDriverList[DriverNumber] != NULL;
        DriverNumber++) {

        OldHaeValue = FwUpdateSetHAE(FlashAccessHaeValue);

        FwUpdateFlashDriver =
            FwUpdateFlashDriverList[DriverNumber](FlashBaseAddressQva);

        FwUpdateSetHAE(OldHaeValue);

        if (FwUpdateFlashDriver != NULL) {
            ReturnStatus = ESUCCESS;
            break;
        }
    }

    return ReturnStatus;
}
#endif /* NEEDFLASHMEMORY */
