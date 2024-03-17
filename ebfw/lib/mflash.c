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
static char *rcsid = "$Id: mflash.c,v 1.4 1999/02/03 16:14:22 gries Exp $";
#endif

/*
 * Controls the reading and writing of the flash device.  
 * Author: Rogelio Cruz
 *
 * $Log: mflash.c,v $
 * Revision 1.4  1999/02/03 16:14:22  gries
 * add include system.h such that dp264.h would be included
 *
 * Revision 1.3  1999/01/25 22:16:04  gries
 * Remove the joke son
 *
 * Revision 1.2  1999/01/21 19:05:08  gries
 * First Release to cvs gries
 *
 * Revision 1.19  1997/12/15  20:44:05  pbell
 * Updated for dp264
 *
 * Revision 1.18  1997/08/18  15:59:47  fdh
 * Added call to AmdFlash_Initialize.
 *
 * Revision 1.17  1997/05/01  20:52:56  pbell
 * Added support for the Am29LV800.
 *
 * Revision 1.16  1996/05/22  20:59:25  fdh
 * Removed some obsoleted routines.  Added code to
 * handle headers with the experimental signature.
 * Minor modifications around default offset definitions.
 *
 * Revision 1.15  1996/02/15  11:18:10  cruz
 * Check image checksum and flag if different from that in the header.
 *
 * Revision 1.14  1996/02/06  14:08:13  cruz
 * Rewrote top level driver to use the algorithm:
 * read, modify, write, verify block.
 * This approach is more effecient than the previous one.
 * Also added was the ability to erase sections of the
 * flash.
 *
 * Revision 1.13  1995/12/12  21:28:53  cruz
 * Removed two unused variables.
 *
 * Revision 1.12  1995/11/22  15:18:05  cruz
 * Rewrote code to prevent flashing when image extends beyond the
 * available flash space.
 *
 * Revision 1.12  1995/11/22  15:18:05  cruz
 * Rewrote code to prevent flashing when image extends beyond the
 * available flash space.
 *
 * Revision 1.11  1995/11/20  22:39:26  cruz
 * Rearrange the code so that it asks for the flash offset
 * AFTER it has printed out the image header (if it exists).
 *
 * Revision 1.10  1995/11/20  21:55:15  cruz
 * Added the ability to write flash at any offset for any number
 * of specified bytes.
 *
 * Revision 1.9  1995/11/17  16:47:20  cruz
 * Added code for asking user to confirm the destination offset.
 *
 * Revision 1.8  1995/11/17  00:00:04  cruz
 * Modified flash_main to look ask for a default destination offset if
 * one is not provided in the command line.  A value for the default
 * offset will be computed from the header if possible.
 *
 * Revision 1.7  1995/10/31  22:50:49  cruz
 * Added compile conditional NEEDFLASHMEMORY around code.
 *
 * Revision 1.6  1995/10/26  23:39:11  cruz
 * Added casting and fixed unsigned comparisons.
 *
 * Revision 1.5  1995/10/03  20:24:52  cruz
 * Made some local functions static.
 * Removed old code.
 *
 * Revision 1.4  1995/10/03  03:20:02  fdh
 * Moved nt HAL I/O routines to halp.c.
 *
 * Revision 1.3  1995/09/02  06:22:49  fdh
 * Included time.h
 *
 * Revision 1.2  1995/07/18  20:10:24  cruz
 * Added code for handling of errors.
 *
 * Revision 1.1  1995/07/05  18:18:50  cruz
 * Initial revision
 *
 *
 */

#include "system.h"

#ifdef NEEDFLASHMEMORY
#include "lib.h"
#include "ledcodes.h"
#include "romhead.h"
#include "rom.h"
#include "fwup.h"
#include "time.h"
#include "console.h"

static
ARC_STATUS
FwupTranslateBusAddresses (
    VOID
    );

static
ARC_STATUS
FwUpdateFlashImage (
    IN ULONG ImageAddress,
    IN ULONG ImageSize,
    IN ULONG FlashOffset
    );

static
ULONG
CalculateBlockNumber (
    ULONG FlashOffset
    );

static
VOID
DecodeErrorMessage (VOID);

static
ARC_STATUS
FlashInit (VOID);

#define MAX_RETRIES   3         /* Number of retries. */

PUCHAR FlashBaseAddressQva =     NULL;
PUCHAR FlashDenseBaseQva =       NULL;
ULONG FlashWindowSize =         0;
ULONG FlashAccessHaeValue =     0;
PUCHAR FlashAddress19PortQva =   NULL;

//
// Variable for holding the current Flash driver.
//
PFLASH_DRIVER FwUpdateFlashDriver = NULL;

//
// Variable containing list of supported Flash drivers.
//
PFLASH_DRIVER (*FwUpdateFlashDriverList[])(PUCHAR) = {
#ifdef AMD_FLASH
  AmdFlash_Initialize,
#endif
#ifdef INTEL_28F008SA
    I28F008SA_Initialize,
#endif
#ifdef AMD_29F040
    Am29F040_Initialize,
#endif
#ifdef AMD_29F080
    Am29F080_Initialize,
#endif
#ifdef AMD_29LV800
    Am29LV800_Initialize,
#endif
    NULL
    };


static BOOLEAN FlashDriverInitialized = FALSE;

//
// Init code.
//
static
ARC_STATUS
FlashInit (
    VOID
    )
{

  ARC_STATUS status = ESUCCESS;

  if (!FlashDriverInitialized) {
      FwupTranslateBusAddresses();     /* Initialize the translated bus addresses. */
      if ((status = FindFlashDevice()) != ESUCCESS) {
          VenPrint(" ERROR: Could not find flash device.\r\n\n");
      }
      else {
          FlashDriverInitialized = TRUE;
      }
  }
  return (status);
}

int flash_main(
    IN ULONG ImageAddress,
    IN LONG DestinationOffset, 
    IN ULONG UserSpecifiedSize
    )
{

  ULONG FlashStartOffset, EndOffset;
  ULONG ImageSize, ImageEnd, ImageStart;
  ULONG FlashSize;
  ULONG DestinationBlock, EndBlock;
  ARC_STATUS ReturnStatus;
  romheader_t * RomImageHeader;
  fw_id_t *FwidData;
  int c,its_nt;
  BOOLEAN EraseOnly = FALSE;
  BOOLEAN HeaderOK;
  BOOLEAN HeaderFound = FALSE;
  ULONG ImageCheckSum;

  outLed (led_k_flash_entered);

  FlashWriteEnable(TRUE);

    if (FlashInit() != ESUCCESS) {
      FlashWriteEnable(FALSE);
      return (0);
    }

  if (ImageAddress == (ULONG) -1) {
      EraseOnly = TRUE;
  }

  FlashStartOffset = (ULONG) DestinationOffset;
  RomImageHeader = (romheader_t * ) ImageAddress;

  if (!EraseOnly) {
      printf("\nImage source address : 0x%X\n", RomImageHeader);
      if (UserSpecifiedSize == 0) {	     /* Treat source as raw data if a size is specified. */
          printf("Searching for Standard ROM image header: ");
	  /*
	   *  Scan the next 1K bytes for the header to accomodate
	   *  other arbitrary headers that might precede the ROM header.
	   */
	  for (RomImageHeader = (romheader_t * ) ImageAddress;
	       RomImageHeader < (romheader_t * )(ImageAddress+1024);
	       RomImageHeader = (romheader_t * )((unsigned int *)RomImageHeader+1)) {
	    if (HasRomSignature(RomImageHeader)) {
              printf ("Found.\n");
	      HeaderFound = TRUE;
              HeaderOK = dumpHeader(RomImageHeader);
	      its_nt= (((int) RomImageHeader->romh.V1.fw_id)==1);
              if (ROMH_VERSION(RomImageHeader) != 0) {
                  ImageStart = (ULONG)RomImageHeader + (ULONG)RomImageHeader->romh.V0.hsize;
                  ImageEnd =  ImageStart + RomImageHeader->romh.V1.rimage_size - 1;
                  ImageCheckSum = ChecksumMem(ImageStart, ImageEnd);
                  if (!HeaderOK || (ImageCheckSum != RomImageHeader->romh.V0.checksum)) {
                      printf("Checksum Error: Image may not load properly.\n");
                  }
              }
	      break;
	    }
	  }
	  if (!HeaderFound) {	
	    printf ("NOT found!!!\n\n");
	    FlashWriteEnable(FALSE);
	    return(0);
          }
      }
  }

  /* If the user did not specify an offset (DestinationOffset < 0), then 
   * figure out a default offset and ask the user to confirm.  Algorithm:
   *
   *  1) Assume offset is FW_DEF_OFFSET;
   *  2) If there's a ROM signature, then we must have a header.  
   *  3) If the header is V2 or higher, then there may be a default offset.  Check
   *     to see if it was specified and use it if it was.
   *  4) If #3 is not satisfied, then if the header version is 1 or higher, then
   *     there must be a firmware id type, use it to figure out a default.
   */

  printf("FlashStartOffset %lx\n",FlashStartOffset);
  if (DestinationOffset < 0) {
      FlashStartOffset = FW_DEF_OFFSET;
      if (HeaderFound) {
          if ((ROMH_VERSION(RomImageHeader) > 1) &&
              (RomImageHeader->romh.V2.rom_offset & 1)) {
              FlashStartOffset = RomImageHeader->romh.V2.rom_offset & ~3;
            printf("HeaderFound FlashStartOffset %lx\n",FlashStartOffset);
          }
          else {
              if (ROMH_VERSION(RomImageHeader) >= 1) {
		FwidData = fwid_match_i((int) RomImageHeader->romh.V1.fw_id);
		if (FwidData != NULL)
		  FlashStartOffset = FwidData->default_fw_offset;
            printf("FwidData %lx FlashStartOffset %lx\n",FwidData,FlashStartOffset);
             }
          }
      }
      printf("\nEnter destination offset or press RETURN for default [%X]: ", FlashStartOffset);
      get_command();
      if (argc != 0) {
          FlashStartOffset = (ULONG)hexarg[0];
      }
  } /* DestinationOffset < 0 */

  FlashSize = FlashGetDeviceSize();

  printf("\nFlash offset         : 0x%X", FlashStartOffset);
  if (!EraseOnly) {
      if ((FlashStartOffset & 3) && HeaderFound) {
          printf(" *** Warning:  Should be LW aligned. ***\n");
      }
      else {
          printf("\n");
      }

      if (UserSpecifiedSize == 0) {	/* Use image size from header if available */
          if (HeaderFound) {
              ImageSize = romimage_size(RomImageHeader);
              printf("Image size w/ header : %d ", ImageSize);
          }
          else {	
              ImageSize = FlashSize - FlashStartOffset;
              printf("Assumed image size   : %d ", ImageSize);
          }
      }
      else {
          ImageSize = UserSpecifiedSize;
          printf("Data image size      : %d", ImageSize);
      }
  }
  else {
      if (UserSpecifiedSize == 0) {
          ImageSize = FlashSize - FlashStartOffset;
      }
      else {
          ImageSize = UserSpecifiedSize;
      }
      printf("\nBytes to be erased   : %d", ImageSize);
  }

  EndOffset = FlashStartOffset+ImageSize-1;

  if (EndOffset >= FlashSize) {
      VenPrint1("\nError: Image extends beyond the available space by %d bytes.\n\n",
      EndOffset - FlashSize + 1); 
      FlashWriteEnable(FALSE);
      return (0);
  }

  DestinationBlock = CalculateBlockNumber(FlashStartOffset);
  EndBlock = CalculateBlockNumber(EndOffset);
  printf(" (Block %d to %d inclusive).\n", DestinationBlock, EndBlock);

  printf("\n\t!!!!! Warning: About to overwrite flash memory !!!!!\n");
  printf("\t     Press Y to proceed, any other key to abort.\n\n");
  c = GetChar ();
  if ( (c != 'y') && (c != 'Y'))
     {	
      printf("Update cancelled by user.  \n\n");
      FlashWriteEnable(FALSE);
      return (EIO);
    }

#ifdef a_joke_son
  if(its_nt)
  {
    printf("\n\tDo you really want to overwrite flash memory?\n");
    printf("\t     Press Y to proceed, any other key to abort.\n\n");
    c = GetChar ();
    if ( (c != 'y') && (c != 'Y'))
     {	
      printf("Update cancelled by user.  \n\n");
      FlashWriteEnable(FALSE);
      return (EIO);
    }
    printf("\n\tDo you have a note from your mother?\n");
    printf("\t     Press Y to proceed, any other key to abort.\n\n");
    c = GetChar ();
    if ( (c != 'y') && (c != 'Y'))
     {	
      printf("Update cancelled by user.  \n\n");
      FlashWriteEnable(FALSE);
      return (EIO);
    }
  }
#endif

  outLed (led_k_write_flash);
  ReturnStatus = FwUpdateFlashImage(ImageAddress, ImageSize, FlashStartOffset);
  if (ReturnStatus != ESUCCESS) {
      printf("\nUpdate failed.\n");
  }
  printf("\n");
  outLed (led_k_flash_exit);
  FlashWriteEnable(FALSE);
  return (ReturnStatus);
}

static
ARC_STATUS
FwupTranslateBusAddresses (
    VOID
    )
{

    //
    // Get the flash window size and hae value
    //
    FlashWindowSize = FLASH_WINDOW_SIZE;
    FlashAccessHaeValue = FLASH_ACCESS_HAE_VALUE;

    //
    // Get the QVA for sparse access to the flash.  We get the
    // bus address to pass in by masking out the HAE bits from
    // the physical address of the flash.
    //
    FlashBaseAddressQva = (PUCHAR)((0xA << 28) | (FLASH_BASE_ADDRESS_PHYSICAL &
                              ~FLASH_ACCESS_HAE_MASK));
    //
    // Get the QVA for dense space access to the flash.
    //
    FlashDenseBaseQva = (PUCHAR)FLASH_BASE_ADDRESS_PHYSICAL;

    //
    // Get the QVA for the flash address 19 port.
    //
    FlashAddress19PortQva =  (PUCHAR)FLASH_ADDRESS19_PORT_PHYSICAL;

    PRTRACE2("FlashBaseAddressQva = %08x\r\n", FlashBaseAddressQva);
    PRTRACE2("FlashDenseBaseQva = %08x\r\n", FlashDenseBaseQva);
    PRTRACE2("FlashAddress19PortQva = %08x\r\n", FlashAddress19PortQva);
    return ESUCCESS;
}


static
ARC_STATUS
FwUpdateFlashImage (
    IN ULONG ImageAddress,
    IN ULONG ImageSize,
    IN ULONG FlashOffset
    )
{
    UCHAR VerifyData;
    ULONG CurrentOffset;
    ULONG Retries, ByteRetries;
    ULONG Index, Mismatch;
    ULONG BlockSize;
    ULONG BlockBase;
    ULONG StartOffset, EndOffset;

    UCHAR Buffer[0x20000];
    BOOLEAN VerifyInProgress;
    ARC_STATUS ReturnStatus = ESUCCESS;

    if (FlashOffset + ImageSize  > FwUpdateFlashDriver->DeviceSize) {
        VenPrint(" ERROR: Image would extend beyond end of flash.\r\n");
        VenPrint(" Flash not written.\r\n");
        return ENOMEM;
    }

    VenPrint(" Writing Flash Block:");

    StartOffset = FlashOffset;
    EndOffset = FlashOffset + ImageSize - 1;

    /*
     *  For all the blocks making up this image, do the following:
     *      1) Read block into buffer.
     *      2) Write data into buffer.
     *      3) Erase block.
     *      4) Write data from buffer.
     *      5) Verify write.
     */
    CurrentOffset = (ULONG) FlashBlockAlign(StartOffset);
    while ((CurrentOffset <= EndOffset) && (ReturnStatus == ESUCCESS)) {
        BlockSize = FlashBlockSize(CurrentOffset);
        BlockBase = (ULONG) FlashBlockAlign(CurrentOffset);
        for (Index = 0; Index < BlockSize; Index++) {
            if ((CurrentOffset >= StartOffset) && (CurrentOffset <= EndOffset)) {
                if (ImageAddress == (ULONG) -1) {
                    Buffer[Index] = 0xFF;
                }
                else {
                    Buffer[Index] = *(PUCHAR)(ImageAddress + (CurrentOffset - StartOffset));
                }
            }
            else {
                Buffer[Index] = ReadFlashUCHAR(CurrentOffset);
            }
            CurrentOffset++;
        }

        VenPrint1(" %d ", CalculateBlockNumber(BlockBase));
        Retries = MAX_RETRIES;

        do {
            VenPrint1("\bE");
            if (CheckForChar(3)) {       /* IF Control-C was pressed, exit */
                return EIO;
            }

            /* 
             *  Erase one block.  
             */
            if ((ReturnStatus = EraseFlashBlock(BlockBase)) != ESUCCESS) {
                continue;
            }

            /* 
             *  Write one block of data.  Try to write a byte up to 2 times.
             */
            VenPrint("\bW");
            for (Index = 0; Index < BlockSize; Index++) {
                ByteRetries = 2;
                do {
                    ReturnStatus = WriteFlashByte(BlockBase + Index, Buffer[Index]);
		    VerifyData = ReadFlashUCHAR(BlockBase + Index);
                } while (--ByteRetries && (ReturnStatus != ESUCCESS));
		if(ReturnStatus != ESUCCESS) continue;
            }

            /* 
             *  Verify one block of data.  If there's mismatch, try sequence again.
             *  Don't print an error message until we have tried to rectify the
             *  problem MAX_RETRIES times.
             */
            VenPrint("\bV");
            Mismatch = 0;
            VerifyInProgress = FALSE;
            for (Index = 0; Index < BlockSize; Index++) {
                VerifyData = ReadFlashUCHAR(BlockBase + Index);
                if (VerifyData != Buffer[Index]) {
                    ReturnStatus = EIO;                    
                    if (Retries > 1) {
                        break;
                    }
                    VerifyInProgress = TRUE;
                    if ((Mismatch % 20) == 0) {
                        VenPrint("\r\nWrite failure(s) encountered:\n");
                        VenPrint("    Offset  Wrote  Read\n");
                        VenPrint("    ======  =====  ====\n");
                    }
                    VenPrint("    %06x    %02x    %02x\n", 
                             BlockBase + Index, Buffer[Index], VerifyData);
                    Mismatch++;
                    if (((Mismatch % 20) == 0) && !kbdcontinue()) {
                        break;
                    }
                }
            }
        } while((ReturnStatus != ESUCCESS) && (--Retries));

        /*
         *  If there still was an error, then don't try to continue.
         */
        if ((ReturnStatus != ESUCCESS) && !VerifyInProgress) {
            VenPrint("\n");
            DecodeErrorMessage();
        }
    }
    return ReturnStatus;
}

static
VOID
DecodeErrorMessage (VOID)
{
  ULONG LastError;
  
  LastError =  FlashGetLastError();

  switch (LastError) {
      case ERROR_VPP_LOW:
           VenPrint("ERROR: Cannot perform ERASE or WRITE operation because programming\n");
           VenPrint("       voltage is not adequate.\n");
           break;
      case ERROR_WRITE_ERROR:
           VenPrint("ERROR: Cannot perform WRITE operation.\n");
           break;
      case ERROR_ERASE_ERROR:
           VenPrint("ERROR: Cannot perform ERASE operation.\n");
           break;
      case ERROR_TIMEOUT:
           VenPrint("ERROR: A timeout occurred while performing operation.\n");
           break;
      default:
           VenPrint("ERROR: Cannot perform operation. Status = %d\n", LastError);
      }

}

static
ULONG
CalculateBlockNumber (
    ULONG FlashOffset
    )
{
    ULONG BlockSize;
    ULONG BlockNumber = 0;
    ULONG FlashPosition = 0;

    for(; FlashPosition < FlashOffset;) {
        BlockSize = FlashBlockSize(FlashPosition);
        if (FlashPosition + BlockSize <= FlashOffset) {
            FlashPosition += BlockSize;
            BlockNumber++;
        } else
            break;
    }

    return BlockNumber;
}

#endif /* NEEDFLASHMEMORY */

