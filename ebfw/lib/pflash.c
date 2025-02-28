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
__attribute__((unused)) static const char *rcsid = "$Id: pflash.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: pflash.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/05/01  20:55:26  pbell
 * Initial revision
 *
 */

#include "system.h"
#ifdef NEEDFLASHMEMORY
#include "halp.h"
#include "arccodes.h"
#include <flash8k.h>

//
// external declarations
//

UCHAR
READ_FLASH_UCHAR(
    IN ULONG Offset
    );

VOID
WRITE_FLASH_UCHAR(
    IN ULONG Offset,
    IN UCHAR Value
    );

//
// forward declarations
//

UCHAR pReadFlashByte(
    IN uintptr_t FlashOffset
    );

//
// public interface
//

ULONG
PlatformFlashEnableAccess(
    VOID
    )
{
    // nothing required

    return 0;
}

VOID
PlatformFlashDisableAccess(
    IN ULONG OldState
    )
{
    // nothing required
}

UCHAR
PlatformFlashReadByte(
    IN ULONG FlashOffset
    )
{
    return pReadFlashByte(FlashOffset);
}



//
// semi-private interface
//

VOID pWriteFlashByte(
    IN uintptr_t FlashOffset,
    IN UCHAR Data
    )
/*++

Routine Description:

    Write a single byte to flash.

Arguments:

    FlashOffset - offset within the flash ROM.
    Data - data to write

Return Value:

    None.

Notes:

    Assumes that the write will succeed (i.e.,
    that the value already at the offset is erased
    or can be overwritten).

--*/
{

    WRITE_FLASH_UCHAR ( FlashOffset, Data );

}


UCHAR pReadFlashByte(
    IN uintptr_t FlashOffset
    )
/*++

Routine Description:

    Read a single byte from flash.

Arguments:

    FlashOffset   - offset within the flash ROM.

Return Value:

    The value of the flash at the given location.

--*/
{
    UCHAR ReturnVal;

    
    ReturnVal = READ_FLASH_UCHAR ( FlashOffset );

    return ReturnVal;
}

#endif /* NEEDFLASHMEMORY */

