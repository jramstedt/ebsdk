/*****************************************************************************

       Copyright 1995 Digital Equipment Corporation,
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
__attribute__((unused)) static const char *rcsid = "$Id: floppy.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 *      Floppy Driver:  Low level driver for a floppy disk.
 *      Author:  Rogelio. R. Cruz  Based on driver from Carey McMaster.
 *
 *  $Log: floppy.c,v $
 *  Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 *  Initial CVS checkin
 *
 * Revision 1.27  1997/10/10  18:03:45  fdh
 * Check the DEBUG_FLOPPY_DRIVER conditional.
 *
 * Revision 1.26  1997/07/10  00:21:01  fdh
 * Added DEBUG code.
 * Removed some unnecessary deposits while
 * polling the interrupt controller.
 *
 * Revision 1.25  1997/05/02  18:11:16  fdh
 * Added the Tracks per side entry to the FLOPPY_DEVICE structure.
 *
 * Revision 1.24  1997/05/01  15:07:40  fdh
 * Condensed debug tracing a little.
 *
 * Revision 1.23  1997/02/21  03:30:04  fdh
 * Added appropriate delays on retries.
 *
 * Revision 1.22  1996/08/20  02:50:07  fdh
 * Changed a typedef name to avoid a conflict.
 *
 * Revision 1.21  1996/05/16  14:44:37  fdh
 * Removed use of inportb alias.
 *
 * Revision 1.20  1996/05/07  23:53:28  fdh
 * Replaced WaitUs with usleep.
 *
 * Revision 1.19  1996/02/22  13:28:13  cruz
 * Added comments to init_pic().
 *
 * Revision 1.18  1995/12/23  21:09:01  fdh
 * Included nttypes.h.
 *
 * Revision 1.17  1995/11/01  16:32:31  cruz
 * Added compile conditional NEEDFLOPPY around code.
 *
 * Revision 1.16  1995/10/26  23:45:05  cruz
 * Removed unused argument to sensedriveinterrupts().
 *
 * Revision 1.15  1995/10/03  02:35:48  fdh
 * Made a couple of internal functions global.
 *
 * Revision 1.14  1995/10/02  22:11:42  cruz
 * Reduced waiting time after issuing commands.
 *
 * Revision 1.13  1995/09/27  20:05:25  cruz
 *  Since the floppy controller can only keep one drive
 * spinning, I changed the code so that it always spins up
 * the drive it's about to access IF the last drive accessed
 * was not the one we're about to access.
 *
 * Revision 1.12  1995/09/15  19:27:19  cruz
 * New version of the floppy driver
 *
 *
 */

#ifdef DEBUG_FLOPPY_DRIVER
#define DEBUG
#endif

#include "system.h"

#ifdef NEEDFLOPPY
#include "lib.h"
#include "nttypes.h"
#include "errno.h"
#include "floppy.h"
#include "ledcodes.h"


/* ======================================================================
 * =                        TYPES DEFINITIONS                           =
 * ======================================================================
 */
#define DRIVE_NUMBER            (DriveInfo->DriveNumber)
#define CURRENT_DRIVE_TYPE      (DriveInfo->CurrentDriveType)
#define NO_DMA                  (DriveInfo->DoNotUseDMAAccess)



#define FLOPPY_CONTROLLER_TIMEOUT       100000  /* Time out in microsecs*/
#define TO_RECEIVE              0x0     /* Controller will receive.     */
#define TO_SEND                 0x40    /* Controller will send data.   */
#define TO_SEND_OR_RECEIVE      0x1     /* " ready to send or receive.  */

#define READ_FLOPPY             0       /* Operation is READ.           */
#define WRITE_FLOPPY            1       /* Operation is WRITE.          */


/*
 *  Floppy Controller Registers
 */
#define FDC_DIGITAL_OUTPUT_REGISTER     0x3F2
#define FDC_MAIN_STATUS_REGISTER        0x3F4
#define FDC_DATA_REGISTER               0x3F5
#define FDC_DIGITAL_INPUT_REGISTER      0x3F7
#define FDC_DATA_RATE_REGISTER          0x3F7

/* msr */
#define	MSRDIO	                        0x40
#define	MSRRQM	                        0x80


#define ST0_INTERRUPT_CODE      0xC0            /* Interrupt code.              */
#define IC_NORMAL_TERMINATION   0x00            /* IC code for normal termination*/

/*
 *  Floppy Controller Commands
 */
#define FDC_SPECIFY_CMD                 0x3
#define FDC_RECALIBRATE_CMD             0x7
#define FDC_SENSE_INTERRUPTS_CMD        0x8
#define FDC_SEEK_CMD                    0xF
#define	FDC_READ_CMD            	0x66
#define	FDC_WRITE_CMD           	0x45

#define READ_WRITE_RETRIES	10
#define RECALIBRATE_RETRIES	2

#define MAX_FDC_CMD_LENGTH      9               /* Maximum number of bytes    */
#define MAX_FDC_RESULT_LENGTH   7               /* Maximum number of bytes    */


typedef struct {
    UCHAR DensityName[12];              /* Disk density string.         */
    UCHAR SectorCode;                   /* Sector code.                 */
    UCHAR EndOfTrack;                   /* End of track sector number.  */
    UCHAR SectorGap;                    /* Sector gap.                  */
    UCHAR FormatGap3;                   /* Format Gap 3.                */
} FLOPPY_TYPE;

typedef struct {
    UCHAR DeviceType[3];                /* Device type string.          */
    ui    SectorsPerTrack;              /* Sectors per track.           */
    ui    TracksPerSide;                /* Tracks per side.             */
    UCHAR NumberOfHeads;                /* Number of heads.             */
    UCHAR MotorOnTimer;                 /* Motor on timer.              */
    UCHAR MotorOffTimer;                /* Motor off timer.             */
    UCHAR StepRate;                     /* Step Rate Interval.          */
    UCHAR DataRate;                     /* Data rate register value.    */
} FLOPPY_DEVICE;


/*
 *  Error codes for the possible errors that may be reported
 *  by this stage.
 */
enum FP_OP_ERRORS {
    FP_OP_START                 = (0 << ERROR4_START_BIT),
    FP_OP_FDC_TIMEOUT           = (1 << ERROR4_START_BIT),
    FP_OP_RESULT_OVERRUN        = (2 << ERROR4_START_BIT),
    FP_OP_CANT_RECALIBRATE      = (3 << ERROR4_START_BIT),
    FP_OP_CANT_MOVE_HEAD        = (4 << ERROR4_START_BIT),
    FP_OP_IO_FAILED             = (5 << ERROR4_START_BIT)
};



/* ======================================================================
 * =                LOCAL-GLOBAL VARIABLE DEFINITIONS                   =
 * ======================================================================
 */

/*
 *  Corresponding string for the codes defined above.
 */
char *Error4Strings[] = {
    "No Errors!",
    "The Floppy Controller timed out waiting to become ready",
    "The result phase produced more bytes than allowed",
    "Recalibrate command failed",
    "Failure in moving head to desired track",
    "I/O command failed"
};

FLOPPY_TYPE FloppyDiskTypes[] =
{
    {"720 KB",                          /* Density.                     */
       2,                               /* Sector code.                 */
       0x9,                             /* End of Track sector.         */
       0x1B,                            /* Sector gap.                  */
       0x50                             /* Format gap 3.                */
    },
    {"1.44 MB",                         /* Density.                     */
       2,                               /* Sector code.                 */
       0x12,                            /* End of Track sector.         */
       0x1B,                            /* Sector gap.                  */
       0x6C                             /* Format gap 3.                */
    },
    {"2.88 MB",                         /* Density.                     */
       2,                               /* Sector code.                 */
       0x24,                            /* End of Track sector.         */
       0x1B,                            /* Sector gap.                  */
       0x53                             /* Format gap 3.                */
    }
};

FLOPPY_DEVICE  FloppyDriveTypes[] = 
{
    {"DD",                              /* Double Density disk.         */
       9,                               /* 9 Sectors per track.         */ 
       80,                              /* Tracks per side              */
       2,                               /* 2 Heads.                     */
       1,                               /* 4 ms Motor on timer.         */
       8,                               /* 256 ms motor off timer.      */
       0xE,                             /* 4 ms. step rate interval.    */
       2                                /* 250 kb/s. data rate.         */
    },
    {"HD",                              /* High Density disk.           */
       18,                              /* 18 Sectors per track.        */ 
       80,                              /* Tracks per side              */
       2,                               /* 2 Heads.                     */
       1,                               /* 1 ms Motor on timer.         */
       0xF,                             /* 240 ms motor off  timer.     */
       0xC,                             /* 4 ms. step rate interval     */
       0                                /* 500 kb/s. data rate.         */
    },
    {"ED",                              /* Enhanced Density disk.       */
       36,                              /* 36 Sectors per track.        */ 
       80,                              /* Tracks per side              */
       2,                               /* 2 Heads.                     */
       29,                              /* 29 ms Motor on timer.        */
       0xE,                             /* 112 ms motor off  timer.     */
       0x9,                             /* 3.5 ms. step rate interval   */
       3                                /* 1 Mb/s. data rate.           */
    }
};

static int LastDriveType;


/* ======================================================================
 * =                   INTERNAL FUNCTION PROTOTYPES                     =
 * ======================================================================
 */
static
DBM_STATUS
WaitForFloppyControllerReady (
                              IN ui Direction,
                              OUT ui *Status
                              );
static
DBM_STATUS
SendCommandToFloppyController (
                               UCHAR Command[],
                               ui CommandCount
                               );
static
DBM_STATUS
SenseDriveInterrupts (
                      VOID
                      );

static
DBM_STATUS
ReadResultPhase (
                 OUT UCHAR Result[],
                 OUT ui *ByteCount
                 );

static
BOOLEAN
CommandTerminatedSuccessfully (
                               VOID
                               );

static
DBM_STATUS
RecalibrateDrive (
                 IN UCHAR DriveNumber
                 );
static
DBM_STATUS
MoveDriveHead (
               IN UCHAR DriveNumber,
               IN ui Head,
               IN ui Track
               );
static
DBM_STATUS
FloppyReadWrite (
                 IN FLOPPY_DRIVE_INFO *DriveInfo,
                 IN UCHAR *Buffer,
                 IN ui LogicalSector,
                 IN ui DiskType,
                 IN ui Operation
                 );

static
VOID
SpinUpFloppyDrive (
                   IN FLOPPY_DRIVE_INFO *DriveInfo 
                   );

#ifdef NOT_IN_USE
static
VOID 
FloppyControllerReset (
                       VOID
                       );

static
BOOLEAN
DiskHasChanged (
                VOID
                );
#endif

static void fdawait(void);
static void setdma(const char *const buf, int type);



/* ======================================================================
 * =  WaitForFloppyControllerReady - Waits for controller to be ready.  =
 * ======================================================================
 *  OVERVIEW
 *      This routine waits until the floppy controller indicates that it
 *      ready to receive or send data.  If it does not become ready within
 *      FLOPPY_CONTROLLER_TIMEOUT microseconds, then the routine will
 *      time out and return STATUS_FAILURE.
 *
 *  FORM OF CALL
 * 	WaitForFloppyControllerReady (Direction, &Status);
 * 
 *  INPUTS
 *      Direction:  If equals to TO_RECEIVE, then it waits for the
 *               controller to be ready for receive.  If equals to
 *               TO_SEND, then it waits for the controller to be
 *               ready to send.
 *      Status:  The address of a location where the last controller 
 *               status byte read in will be placed.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation was successful.
 *      STATUS_FAILURE if the operation failed (timed out).
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 8/31/95
 */
static
DBM_STATUS
WaitForFloppyControllerReady (
                              IN ui Direction,
                              OUT ui *Status
                              )
{
    ui TimeOutTimer;
    ui TestBits;

#if 0
    PRTRACE1("Waiting for controller to be ready.\n");
#endif

    TimeOutTimer = FLOPPY_CONTROLLER_TIMEOUT;

    if (Direction == TO_SEND_OR_RECEIVE) {
        TestBits = MSRRQM;
        Direction = 0;
    }
    else {
        TestBits = MSRRQM | MSRDIO;
    }

    PRTRACE1(" MSR:");

    /*
     *  Wait for the controller to be ready to receive or to send.
     */
    do {
        /*
         *         Main Status Register (MSR)
         *   -----------------------------------------
         *   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
         *   -----------------------------------------
         *   |REQ |DATA|NON-|CMD |DRV3|DRV2|DRV1|DRV0|
         *   |MAST|I/O |DMA |PROG|BUSY|BUSY|BUSY|BUSY|
         *   -----------------------------------------
         */
        *Status = (ui) inportb(FDC_MAIN_STATUS_REGISTER);

        PRTRACE2(" 0x%X", *Status);

        if ((*Status & TestBits) == (MSRRQM | Direction)) {
	  PRTRACE1("\n");
	  return STATUS_SUCCESS;
        }

        /*
         *  If not ready, then wait 24 microseconds before
         *  trying again.
         */
        usleep(24);

    } while (TimeOutTimer--);

    PRTRACE1("\nTimed out while Waiting for controller to be ready.\n");
    errno = FP_OP_FDC_TIMEOUT;
    return STATUS_FAILURE;
}



/* ======================================================================
 * =  SendCommandToFloppyController - Sends a command to the controller.=
 * ======================================================================
 *  OVERVIEW
 *      Sends a command to the controller when ready.  If it doesn't
 *      become ready within a specified time, then it times out.
 *
 *  FORM OF CALL
 * 	SendCommandToFloppyController (Command, CommandCount)
 * 
 *  INPUTS
 *      Command:  Array of bytes containing the command to be send out.
 *      CommandCount:  Number of bytes that make up the command.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation was successful.
 *      STATUS_FAILURE if the operation failed (timed out).
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 8/31/95
 */
static
DBM_STATUS
SendCommandToFloppyController (
                               UCHAR Command[],
                               ui CommandCount
                               )
{
    ui Index, Status;

    PRTRACE1("Sending command to floppy controller.\n");

    /*
     *  Send each byte of the command out but before sending
     *  make sure the controller is ready to receive a command.
     */
    for (Index = 0; Index < CommandCount; Index++) {

        if (WaitForFloppyControllerReady(TO_RECEIVE, &Status) ==
            STATUS_FAILURE) {
            PRTRACE1("ERROR: Timed out while sending command.\n");
            return STATUS_FAILURE;
        }

        /*
         *  Send the command out to the controller.
         */
        outportb(FDC_DATA_REGISTER, Command[Index]);

    } /* for */

    return STATUS_SUCCESS;
}



/* ======================================================================
 * =  SenseDriveInterrupts - Send out the sense interrupts command.     =
 * ======================================================================
 *  OVERVIEW
 *      Sends out the sense interrupts command which clears
 *      any pending interrupts as a side effect.
 *
 *  FORM OF CALL
 * 	SenseDriveInterrupts ();
 * 
 *  INPUTS
 *      None
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation was successful.
 *      STATUS_FAILURE if the operation failed (timed out).
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 8/31/95
 */
static
DBM_STATUS
SenseDriveInterrupts (
                      VOID
                      )
{
    UCHAR       Command[MAX_FDC_CMD_LENGTH];

    /*
     *                Sense Interrupt Command
     *          ---------------------------------
     *          | 0 | 0 | 0 | 0 | 1 | 0 | 0 | 0 |
     *          ---------------------------------
     */
    PRTRACE1("Sending SENSE INTERRUPT command.\n");
    Command[0] = FDC_SENSE_INTERRUPTS_CMD;
    return (SendCommandToFloppyController(Command, 1));
}



/* ======================================================================
 * =  ReadResultPhase - Reads the result phase of a command.            =
 * ======================================================================
 *  OVERVIEW
 *      Reads the result phase of a command.
 *
 *  FORM OF CALL
 * 	ReadResultPhase (Result, &ByteCount);
 * 
 *  OUTPUTS
 *      Result:  Array of bytes where result bytes will be placed.
 *      ByteCount:  Number of bytes that make up the result.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation was successful.
 *      STATUS_FAILURE if the operation failed (timed out).
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 8/31/95
 */
static
DBM_STATUS
ReadResultPhase (
                 OUT UCHAR Result[],
                 OUT ui *ByteCount
                 )
{
    ui       Status;

    PRTRACE1("Reading Result Phase of a command.\n");

    *ByteCount = 0;

    do {
        /*
         *  Waits for the controller to be ready to send or receive
         *  a command.
         */
        if (WaitForFloppyControllerReady(TO_SEND_OR_RECEIVE, &Status) ==
            STATUS_FAILURE) {
            return(STATUS_FAILURE);
        }

        /*
         *  If the controller is ready to receive, then it means that
         *  the result phase is over.  
         */
        if ((Status & MSRDIO) == TO_RECEIVE) {
            return STATUS_SUCCESS;
        }

        /*
         *  The controller is ready to send a result byte.  Read it.
         */
        Result[*ByteCount] = inportb(FDC_DATA_REGISTER);
        (*ByteCount)++;
    } while (*ByteCount <  MAX_FDC_RESULT_LENGTH + 1);

    errno = FP_OP_RESULT_OVERRUN;
    return STATUS_FAILURE;
}



/* ======================================================================
 * =  CommandTerminatedSuccessfully - Returns TRUE if successful.       =
 * ======================================================================
 *  OVERVIEW
 *      Returns true if the command terminated  successfully
 *
 *  FORM OF CALL
 *      CommandTerminatedSuccessfully()
 * 
 *  INPUTS
 *      None.
 *
 *  RETURNS
 * 	TRUE if the operation was successful.
 *      FALSE otherwise.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 8/31/95
 */
static
BOOLEAN
CommandTerminatedSuccessfully (
                               VOID
                               )
{
    UCHAR  Result[MAX_FDC_RESULT_LENGTH];
    ui  ByteCount;

    PRTRACE1("Checking if command terminated successfully.\n");
    if (ReadResultPhase(Result, &ByteCount) == STATUS_FAILURE) {
        return FALSE;
    }

    /*
     *  Check that interrupt code bits which indicate whether the
     *  command completed normally or not.
     *
     *              Status Register 0
     *   -----------------------------------------
     *   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
     *   -----------------------------------------
     *   |INT |INT |SEEK|EQMT| 0  |HEAD|DRV1|DRV0|
     *   |CODE|CODE|END |CHCK|    |SLCT|    |    |
     *   -----------------------------------------
     */
    if (ByteCount == 0 || (Result[0] & ST0_INTERRUPT_CODE) != IC_NORMAL_TERMINATION) {
        return FALSE;
    }
    return TRUE;
}



/* ======================================================================
 * =  RecalibrateDrive - Returns head to track 0.                       =
 * ======================================================================
 *  OVERVIEW
 *      Returns the head to track 0.
 *
 *  FORM OF CALL
 *      RecalibrateDrive(DriveNumber);
 * 
 *  INPUTS
 *      DriveNumber: Drive to be recalibrated.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation was successful.
 *      STATUS_FAILURE if the operation failed.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 8/31/95
 */
static
DBM_STATUS
RecalibrateDrive (
                 IN UCHAR DriveNumber
                 )
{
    ui       Retries;
    UCHAR       Command[MAX_FDC_CMD_LENGTH];

    Retries = RECALIBRATE_RETRIES;

    /*
     *  We'll try sending the recalibrate command twice
     *  because some drives cannot back step more than
     *  77 tracks.  This means that if we have an 80 track
     *  drive, we'll need to do the recalibration twice
     *  so we can get to track 0.
     */

    do {
        /*
         *    ---------------------------------
         *    | 0 | 0 | 0 | 0 | 0 | 1 | 1 | 1 |
         *    ---------------------------------
         *    | 0 | 0 | 0 | 0 | 0 | 0 |DR1|DR0|
         *    ---------------------------------
         */
        PRTRACE1("Sending RECALIBRATE command.\n");
        Command[0] = FDC_RECALIBRATE_CMD;
        Command[1] = DriveNumber;
        if (SendCommandToFloppyController(Command, 2) == STATUS_FAILURE) {
            return STATUS_FAILURE;
        }

        /*
         *  Wait for controller to be ready.
         */
        fdawait();

        /*
         *  Sense and clear all interrupts that may be pending.
         *  Used for determining if track 0 signal failed.
         */
        if (SenseDriveInterrupts() == STATUS_FAILURE) {
            return STATUS_FAILURE;
        }

        /*
         *  Read the status register to determine if there were
         *  any problems.
         */
        if (CommandTerminatedSuccessfully()) {
            /*
             *  Wait a few milliseconds before continuing.
             */
            usleep(10);
            return STATUS_SUCCESS;
        }

    } while (Retries--);

    errno = FP_OP_CANT_RECALIBRATE;
    return STATUS_FAILURE;
}



/* ======================================================================
 * =  MoveDriveHead - Moves the drive head to specified location.       =
 * ======================================================================
 *  OVERVIEW
 *      Moves the specified head to the specified location.
 *
 *  FORM OF CALL
 *      MoveDriveHead(DriveNumber, Head, Track);
 * 
 *  INPUTS
 *      DriveNumber: Drive whose head should be moved.
 *      Head:  Head to be moved.
 *      Track:  Track that the head should be moved to.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation was successful.
 *      STATUS_FAILURE if the operation failed.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/1/95
 */
static
DBM_STATUS
MoveDriveHead (
               IN UCHAR DriveNumber,
               IN ui Head,
               IN ui Track
               )
{
    UCHAR       Command[MAX_FDC_CMD_LENGTH];

    PRTRACE3("Moving head for drive %d to track %d\n", DriveNumber, Track);

    /*
     *        Seek Command
     *    ---------------------------------
     *    | 0 | 0 | 0 | 0 | 1 | 1 | 1 | 1 |
     *    ---------------------------------
     *    | 0 | 0 | 0 | 0 | 0 |HD |DR1|DR0|
     *    ---------------------------------
     *    |      New track number.        |
     *    ---------------------------------
     *    |MSN of track # | 0 | 0 | 0 | 0 |
     *    ---------------------------------
     *    Last byte of command is required only if
     *    ETR is set in Mode Command.
     */
    Command[0] = FDC_SEEK_CMD;
    Command[1] = (UCHAR) (((Head & 1) << 2) | DriveNumber);
    Command[2] = (UCHAR) Track;
    if (SendCommandToFloppyController(Command, 3) == STATUS_FAILURE) {
        return STATUS_FAILURE;
    }

    /*
     *  Wait for controller to be ready.
     */
    fdawait();

    /*
     *  Sense and clear all interrupts that may be pending.
     *  Used for determining if seek is done.
     */
    if (SenseDriveInterrupts() == STATUS_FAILURE) {
        return STATUS_FAILURE;
    }

    /*
     *  Read the status register to determine if there were
     *  any problems.
     */
    if (CommandTerminatedSuccessfully()) {
        /*
         *  Wait a few milliseconds before continuing.
         */
        usleep(10);
        return STATUS_SUCCESS;
    }

    errno = FP_OP_CANT_MOVE_HEAD;
    return STATUS_FAILURE;
}



/* ======================================================================
 * =  SpinUpFloppyDrive - Turns on the motor for the specified drive.   =
 * ======================================================================
 *  OVERVIEW
 *       Turns on the motor for the specified drive.
 *
 *  FORM OF CALL
 *      SpinUpFloppyDrive(DriveInfo);
 * 
 *  INPUTS
 *      DriveInfo:  Pointer to structure containing info about the drive.
 *
 *  RETURNS
 * 	None.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 8/31/95
 */
static
VOID
SpinUpFloppyDrive(
                  IN FLOPPY_DRIVE_INFO *DriveInfo
                  )
{
    UCHAR       Value;

    PRTRACE2("Spinning up drive %d\n", DRIVE_NUMBER);

    /*
     *  Enable drive and get the motor going.
     *
     *   Digital Output Register (DOR)
     *   -----------------------------------------
     *   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
     *   -----------------------------------------
     *   |MTR3|MTR2|MTR1|MTR0|DMA |RST |DRV1|DRV0|
     *   -----------------------------------------
     */

    /*
     *  1) Turn on the motor for current drive.
     *  2) Enable DMA signals if using DMA access.
     *  3) Bring controller out of reset by writing a 1.
     *  4) Set the drive select.
     */
      Value = (1 << (4 + DRIVE_NUMBER)) | 
              ((!NO_DMA) << 3) |
              (1 << 2) | DRIVE_NUMBER;

    PRTRACE2("Writing 0x%X to DOR reg.\n", Value);
    outportb(FDC_DIGITAL_OUTPUT_REGISTER, Value);

    usleep(10);                    /* Wait 100 microseconds.          */
}



/* ======================================================================
 * =  SpinDownFloppyDrive - Turns off the motor for the specified drive.=
 * ======================================================================
 *  OVERVIEW
 *       Turns off the motor for the specified drive.
 *
 *  FORM OF CALL
 *      SpinDownFloppyDrive(DriveInfo)
 * 
 *  INPUTS
 *      DriveNumber: Drive to be turned off.
 *      DoNotUseDMAAccess:  If True, the drive will be set in non-DMA mode.
 *
 *  RETURNS
 * 	None.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 8/31/95
 */
VOID
SpinDownFloppyDrive(
                    IN FLOPPY_DRIVE_INFO *DriveInfo
                    )
{
    UCHAR    Value;

    PRTRACE2("Spinning down drive %d\n", DRIVE_NUMBER);
    /*
     *   Spin down the motor.
     *
     *   Digital Output Register (DOR)
     *   -----------------------------------------
     *   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
     *   -----------------------------------------
     *   |MTR3|MTR2|MTR1|MTR0|DMA |RST |DRV1|DRV0|
     *   -----------------------------------------
     */

    /*
     *  1) Turn off all motors
     *  2) Leave DMA signals enabled if using DMA access.
     *  3) Do not reset controller.
     *  4) Select drive.
     */
      Value = ((!NO_DMA) << 3) | (1 << 2) | DRIVE_NUMBER;

    PRTRACE2("Writing 0x%X to DOR reg.\n", Value);
    outportb(FDC_DIGITAL_OUTPUT_REGISTER, Value);
}


#ifdef NOT_IN_USE
/* ======================================================================
 * =  FloppyControllerReset - Resets the floppy controller.             =
 * ======================================================================
 *  OVERVIEW
 *       Resets the floppy controller.  All motors will be turned off,
 *      DMA will be disabled and drive #0 will be selected.
 *
 *  FORM OF CALL
 *      FloppyControllerReset()
 * 
 *  INPUTS
 *      None.
 *
 *  RETURNS
 * 	None.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/8/95
 */
static
VOID 
FloppyControllerReset (
                       VOID
                       )
{
    PRTRACE1("Resetting floppy controller.\n");
    /*
     *   Digital Output Register (DOR)
     *   -----------------------------------------
     *   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
     *   -----------------------------------------
     *   |MTR3|MTR2|MTR1|MTR0|DMA |RST |DRV1|DRV0|
     *   -----------------------------------------
     */

    /*
     *  1) Turn off all motors
     *  2) Disable DMA signals.
     *  3) Reset controller.
     */
    outportb(FDC_DIGITAL_OUTPUT_REGISTER, 0x0);
    usleep(1);
    outportb(FDC_DIGITAL_OUTPUT_REGISTER, (1<<2));
}



/* ======================================================================
 * =  DiskHasChanged - Returns TRUE if disk has benn changed.           =
 * ======================================================================
 *  OVERVIEW
 *      Returns TRUE if the disk has been changed for the current drive.
 *      NOTE:  I never got this function to work for our drives.
 *
 *  FORM OF CALL
 *      DiskHasChanged()
 * 
 *  INPUTS
 *      None.
 *
 *  RETURNS
 * 	TRUE if disk has been changed, FALSE otherwise.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/8/95
 */
static
BOOLEAN
DiskHasChanged (
                VOID
                )
{
    /*
     *   Digital Input Register (DIR)
     *   -----------------------------------------
     *   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
     *   -----------------------------------------
     *   |DSK | X  |  X |  X | X  | X  | X  | X  |
     *   |CHNG|    |    |    |    |    |    |    |
     *   -----------------------------------------
     */

    return ((inportb(FDC_DIGITAL_INPUT_REGISTER) & (1<<7)) == (1<<7));
}
#endif


/* ======================================================================
 * =  FloppyReadWrite - Performs a read/write operation from/to disk.   =
 * ======================================================================
 *  OVERVIEW
 *      Performs a read or write of specified sector.
 *
 *  FORM OF CALL
 *      FloppyReadWrite(DriveInfo, Buffer, Sector, Track, Head, Operation);
 * 
 *  INPUTS
 *      DriveInfo:  Pointer to structure containing info about the drive.
 *      Buffer:  Pointer to area where data should be read from/written to.
 *      LogicalSector:  Logical sector to read/write.
 *      DiskType:  Index into the DiskTypes Table.
 *      Operation:  READ_FLOPPY or WRITE_FLOPPY
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation was successful.
 *      STATUS_FAILURE if the operation failed.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/1/95
 */
static
DBM_STATUS
FloppyReadWrite (
                 IN FLOPPY_DRIVE_INFO *DriveInfo,
                 IN UCHAR *Buffer,
                 IN ui LogicalSector,
                 IN ui DiskType,
                 IN ui Operation
                 )
{
    UCHAR       Command[MAX_FDC_CMD_LENGTH];
    ui       PhysicalSector, PhysicalTrack, PhysicalHead;
    ui       SectorsPerTrack, NumberOfHeads;

    PRTRACE2("Performing I/O on drive %d\n", DRIVE_NUMBER);
    
    errno = NO_ERRORS;

    /*
     *  Make sure that the controller is configured for the
     *  right type of drive.
     */
    if (LastDriveType != CURRENT_DRIVE_TYPE) {
        if (InitializeFloppyDrive(DriveInfo) == STATUS_FAILURE) {
            return STATUS_FAILURE;
        }
    }

    SpinUpFloppyDrive(DriveInfo);

    /*
     *  If the current track is negative, then this means
     *  that we want to reset the drive (ie, step
     *  out the drive head to track 0).
     */
    if (DriveInfo->CurrentTrack < 0) {
        if (RecalibrateDrive(DRIVE_NUMBER) == STATUS_FAILURE) {
            return STATUS_FAILURE;
        }
        DriveInfo->CurrentTrack = 0;
    }


    /*
     *  Convert the logical sector to a physical location on the
     *  disk consisting of a track, sector and head number.
     */
    SectorsPerTrack = FloppyDriveTypes[CURRENT_DRIVE_TYPE].SectorsPerTrack;
    NumberOfHeads = FloppyDriveTypes[CURRENT_DRIVE_TYPE].NumberOfHeads;
    PhysicalHead = 0;
    PhysicalTrack = LogicalSector / (SectorsPerTrack * NumberOfHeads);
    PhysicalSector = LogicalSector % (SectorsPerTrack * NumberOfHeads); 
    if (PhysicalSector >= SectorsPerTrack) {
        PhysicalHead++;
        PhysicalSector -= SectorsPerTrack;
    }

    /*
     *  Determine if we need to move the head for the drive.
     *  If so, then move the head to the right track.
     */
    if ((ui)DriveInfo->CurrentTrack != PhysicalTrack) {
        if (MoveDriveHead(DRIVE_NUMBER, PhysicalHead, PhysicalTrack) == STATUS_FAILURE) {
            return STATUS_FAILURE;
        }
        DriveInfo->CurrentTrack = PhysicalTrack;
   }
        
    /*
     *  XXX TEMPORARY:  For now just set DMA channels.
     */
    setdma((char *)Buffer, (int)Operation);

    /*
     *   Read or Write a sector.
     *
     *   
     *   -----------------------------------------
     *   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
     *   -----------------------------------------
     *   | MT | MFM| SK | 0  | 0  | 1  |  1 |  0 |  Read Command
     *   -----------------------------------------
     *                    OR
     *   -----------------------------------------
     *   | MT | MFM| SK | 0  | 0  | 1  |  0 |  1 |  Write Command
     *   -----------------------------------------
     *   -----------------------------------------
     *   |IPS |  X | X  | X  | X  | HD | DR1| DR0| 
     *   -----------------------------------------
     *   |              Track Number.            |
     *   -----------------------------------------
     *   |              Drive Head Number.       |
     *   -----------------------------------------
     *   |             Sector Number.            |
     *   -----------------------------------------
     *   |           Bytes Per Sector Code.      |
     *   -----------------------------------------
     *   |         End Of Track Sector Number.   |
     *   -----------------------------------------
     *   |        Intersector Gap Length.        |
     *   -----------------------------------------
     *   |           Data Length.                |
     *   -----------------------------------------
     */
     Command[0] = (Operation == READ_FLOPPY) ? FDC_READ_CMD : 
                                               FDC_WRITE_CMD;
    Command[1] = (UCHAR)((PhysicalHead << 2)| DRIVE_NUMBER);
    Command[2] = (UCHAR) PhysicalTrack;
    Command[3] = (UCHAR) PhysicalHead;
    Command[4] = (UCHAR)(PhysicalSector+1);
    Command[5] = FloppyDiskTypes[DiskType].SectorCode;
    Command[6] = FloppyDiskTypes[DiskType].EndOfTrack;
    Command[7] = FloppyDiskTypes[DiskType].SectorGap;
    Command[8] = 0xFF;                  /* A Don't Care value.          */
    if (SendCommandToFloppyController(Command, 9) == STATUS_FAILURE) {
        return STATUS_FAILURE;
    }

    /*
     *  Wait for controller to be ready.
     */
    fdawait();

    /*
     *  Read the status register to determine if there were
     *  any problems.
     */
    if (CommandTerminatedSuccessfully()) {
        return STATUS_SUCCESS;
    }

    /*
     *  Reset the current track number.
     */
    DriveInfo->CurrentTrack = -1;
    errno = FP_OP_IO_FAILED;
    return STATUS_FAILURE;
}



/* ======================================================================
 * =  InitializeFloppyDrive - Initializes floppy drive.                 =
 * ======================================================================
 *  OVERVIEW
 *      Performs initialization of specified drive.
 *
 *  FORM OF CALL
 *      InitializeFloppyDrive(DriveInfo)
 * 
 *  INPUTS
 *      DriveInfo:  Pointer to structure containing info about the drive.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation was successful.
 *      STATUS_FAILURE if the operation failed.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/6/95
 */
DBM_STATUS
InitializeFloppyDrive (
                       IN FLOPPY_DRIVE_INFO *DriveInfo
                       )
{
    UCHAR       Command[MAX_FDC_CMD_LENGTH];

    PRTRACE2("Initializing floppy drive #%d.\n", DRIVE_NUMBER);

     /*
     *  Initialize the current track variable.
     */
    DriveInfo->CurrentTrack = -1;

    /*
     *  Make sure all disks are spun down and the
     *  controller is out of reset.
     *  Wait 5 ms to give the controller a chance to
     *  come out of reset.
     */
    PRTRACE1("Turning off all motors.\n");
    SpinDownFloppyDrive(DriveInfo);
    usleep(10);

    /*
     *   Data Rate Select Register (DSR)
     *   -----------------------------------------
     *   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
     *   -----------------------------------------
     *   |S/W |LOW | 0  |PRE-|PRE-|PRE-|DATA|DATA|
     *   |RST |PWR |    |COMP|COMP|COMP|RATE|RATE|
     *   -----------------------------------------
     */
    PRTRACE1("Setting data transfer rate.\n");
    outportb(FDC_DATA_RATE_REGISTER, FloppyDriveTypes[CURRENT_DRIVE_TYPE].DataRate);

    /*
     *  Make sure that interrupts are cleared by reading the
     *  interrupt register which clears all.
     */
    PRTRACE1("Sending SENSE INTERRUPT command.\n");
    if (SenseDriveInterrupts() == STATUS_FAILURE) {
        return STATUS_FAILURE;
    }

    /*
     *  Read the status register to determine if there were
     *  any problems.  Note that the interrupt code will not
     *  be the normal termination code after a reset, therefore,
     *  I chose to ignore the return value of the next routine.
     *  We need to invoke it so that the result phase of the
     *  previous command is read.
     */
    CommandTerminatedSuccessfully();
    LastDriveType = CURRENT_DRIVE_TYPE;

    /*
     *  Set the initial values for the three internal timers of
     *  the floppy controller.
     *
     *          ---------------------------------
     *          | 0 | 0 | 0 | 0 | 0 | 0 | 1 | 1 |
     *          ---------------------------------
     *          |   Step Rate   |  Motor Off    |
     *          ---------------------------------
     *          |         Motor On Time     |DMA|
     *          ---------------------------------
     */
    PRTRACE1("Sending SPECIFY command.\n");
    Command[0] = FDC_SPECIFY_CMD;
    Command[1] = (FloppyDriveTypes[CURRENT_DRIVE_TYPE].StepRate << 4) |
                 (FloppyDriveTypes[CURRENT_DRIVE_TYPE].MotorOffTimer);
    Command[2] = (FloppyDriveTypes[CURRENT_DRIVE_TYPE].MotorOnTimer << 1) |
                  (NO_DMA & 1);
    return (SendCommandToFloppyController(Command, 3));
}



/* ======================================================================
 * =  ReadLogicalSector - Reads the specified logical sector into memory=
 * ======================================================================
 *  OVERVIEW
 *      Reads the specified sector into memory.
 *
 *  FORM OF CALL
 *      ReadLogicalSector(DriveInfo, DiskType, LogicalSector, Destination)
 * 
 *  INPUTS
 *      LogicalSector:  Logical sector number to read.
 *      DiskType: Pointer into DiskTypes table.
 *      Destination:  Address where data will be written to.
 *      DriveInfo:  Pointer to structure containing info about the drive.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation was successful.
 *      STATUS_FAILURE if the operation failed.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/6/95
 */
DBM_STATUS
ReadLogicalSector (
                   IN FLOPPY_DRIVE_INFO *DriveInfo,
                   IN ui DiskType,
                   IN ui LogicalSector,
                   IN UCHAR *Destination
                   )
{
    DBM_STATUS      Status;
    ui       Retries;

    PRTRACE3("Reading logical sector %d to 0x%X\n", LogicalSector, Destination);
    Retries = READ_WRITE_RETRIES;

    /*
     *  Repeat the reading of the logical sector until it is
     *  successful or we have tried reading it READ_WRITE_RETRIES times.
     */
    while ( Retries && 
           ((Status = FloppyReadWrite(DriveInfo, Destination, LogicalSector, 
                                      DiskType, READ_FLOPPY)) ==
            STATUS_FAILURE)) {
      --Retries;
    }

#ifdef DEBUG
    if (Retries != READ_WRITE_RETRIES) {
      outLed(led_k_floppy_read_error);
      printf("%d", READ_WRITE_RETRIES-Retries);
    }
    else {
      PutChar('.');
    }
#endif

    return Status;
}



/* ======================================================================
 * =  WriteLogicalSector - Writess the specified logical sector to disk =
 * ======================================================================
 *  OVERVIEW
 *      Writes the specified sector to disk.
 *
 *  FORM OF CALL
 *      WriteLogicalSector(DriveInfo, DiskType, LogicalSector, Source)
 * 
 *  INPUTS
 *      LogicalSector:  Logical sector number to read.
 *      DiskType: Pointer into DiskTypes table.
 *      DriveInfo:  Pointer to structure containing info about the drive.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation was successful.
 *      STATUS_FAILURE if the operation failed.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/6/95
 */
DBM_STATUS
WriteLogicalSector (
                    IN FLOPPY_DRIVE_INFO *DriveInfo,
                    IN ui DiskType,
                    IN ui LogicalSector,
                    IN UCHAR *Source
                   )
{
    DBM_STATUS      Status;
    ui       Retries;

    PRTRACE3("Writing logical sector %d from 0x%X\n", LogicalSector, Source);
    Retries = READ_WRITE_RETRIES;

    /*
     *  Repeat the writing of the logical sector until it is
     *  successful or we have tried writing it READ_WRITE_RETRIES times.
     */
    while ( Retries && 
           ((Status = FloppyReadWrite(DriveInfo, Source, LogicalSector, 
                                      DiskType, WRITE_FLOPPY)) ==
            STATUS_FAILURE)) {
      --Retries;
    }

#ifdef DEBUG
    if (Retries != READ_WRITE_RETRIES) {
      outLed(led_k_floppy_write_error);
      printf("%d", READ_WRITE_RETRIES-Retries);
    }
    else {
      PutChar('.');
    }
#endif

    return Status;
}



/* pic */
#define	PICBOCW	0x20
#define	PICBIRR	0x20
#define	READIRR	0x0A
#define	FDAMASK	0x40
static void fdawait(void)
{
  int	irr;

  outportb(PICBOCW, READIRR);
  while (TRUE) {
    irr = inportb(PICBIRR);
    if ((irr&FDAMASK) != 0)
      break;
    usleep(500);
  }
}



/* dma */
#define DMACMD03 0x08
#define DMACMD47 0xD0
#define DMAMODE03 0x0B
#define DMAMODE47 0xD6
#define DMAXMODE03 0x40B
#define DMAXMODE47 0x4D6
#define	DMAOFFS	0x04
#define	DMALPAG	0x81
#define DMAHPAG 0x481
#define	DMACNT	0x05
#define	DMAHCNT	0x405
#define	DMACBP	0x0C
#define DMAMASK03 0x0A
#define DMAMASK47 0xD4

static void setdma(const char *const buf, int type)
{
  
  /* control */
  outportb(DMACMD47, 0x00);		      /* 4, enable */
  outportb(DMACMD03, 0x00);		      /* 2, enable */
  outportb(DMAMODE47, 0xC0);		      /* 4, cascade */
  if (type == READ_FLOPPY) {		      /* if read from disk */
    outportb(DMAMODE03, 0x46);		      /* 2, single */
    outportb(DMAXMODE03, 0x02);
  }
  else {				      /* if write to disk */
    outportb(DMAMODE03, 0x4A);		      /* 2, single */
    outportb(DMAXMODE03, 0x02);
  }

  /* target address */
  outportb(DMACBP, 0x00);
  outportb(DMAOFFS, (uintptr_t)buf >>  0);
  outportb(DMAOFFS, (uintptr_t)buf >>  8);
  outportb(DMALPAG, (uintptr_t)buf >> 16);
  outportb(DMAHPAG, (uintptr_t)buf >> 24);
  /* count */
  outportb(DMACBP, 0x00);
  outportb(DMACNT, (512-1) >>  0);	
  outportb(DMACNT, (512-1) >>  8);	
  outportb(DMAHCNT, (512-1) >> 16);	
  /* enable */
  outportb(DMAMASK47, 0x00);		      /* un-mask 4, cascade */
  outportb(DMAMASK03, 0x02);		      /* un-mask 2, floppy */
}




#define ICW1_C1 0x20
#define ICW2_C1 0x21
#define ICW3_C1 0x21
#define ICW4_C1 0x21
#define MASK_C1 0x21
#define ELCR_C1 0x4D0
#define OCW1_C1 0x21
#define OCW2_C1 0x20
#define OCW3_C1 0x20

#define ICW1_C2 0xA0
#define ICW2_C2 0xA1
#define ICW3_C2 0xA1
#define ICW4_C2 0xA1
#define MASK_C2 0xA1
#define ELCR_C2 0x4D1
#define OCW1_C2 0xA1
#define OCW2_C2 0xA0
#define OCW3_C2 0xA0

/* ======================================================================
 * =  Init_PIC - Initializes the Programmable Interrupt Controllers     =
 * ======================================================================
 *  OVERVIEW
 *      This routine will initialize the interrupt controllers in the
 *      SIO.  The specs require that both controllers be initialized 
 *      before any function can be performed.  It also spells out the
 *      initialization sequence from which we may not diverge: ICW1,
 *      ICW2, ICW3 and ICW4 must be written in this order.  It is
 *      recommended that Controller 2 (slave) be initialized before
 *      Controller 1 (master).
 *
 *  FORM OF CALL
 * 	init_pic ();
 * 
 *  INPUTS
 *      None.
 *
 *  RETURNS
 * 	None.
 *
 *  AUTHOR
 *      Origins unknown.  Comments belong to Rogelio R. Cruz. 2/21/96
 */
VOID
init_pic (
          VOID
          )
{
    int x;

    /* 
     *  The following 4 I/O writes will set the slave controller as
     *  follow: 
     *        1) ICW4 write required, operate controllers in cascade mode, 
     *           interrupts are edge-triggered.
     *        2) Vector address for interrupt = 0x20 (not used).
     *        3) Must be programmed with 2 because controllers are 
     *           cascaded thru IRQ2.
     *        4) controller operates in 80x86 mode, normal EOI,
     *           buffered mode and special fully nested mode disabled.
     */
    outportb(ICW1_C2,0x11);
    outportb(ICW2_C2,0x08);
    outportb(ICW3_C2,0x02);
    outportb(ICW4_C2,0x01);

    outportb(MASK_C2,0x0);      /* Unmask all interrupts for slave controller */
    outportb(ELCR_C2,0);	/* select level sensitive */
  
    /* 
     *  The following 4 I/O writes will set the master controller as
     *  follow: 
     *        1) ICW4 write required, operate controllers in cascade mode, 
     *           interrupts are edge-triggered.
     *        2) Vector address for interrupt = 0x00 (not used).
     *        3) Must be programmed with 4 because controllers are 
     *           cascaded thru IRQ2.
     *        4) controller operates in 80x86 mode, normal EOI,
     *           buffered mode and special fully nested mode disabled.
     */
    outportb(ICW1_C1,0x11);
    outportb(ICW2_C1,0x00);
    outportb(ICW3_C1,0x04);
    outportb(ICW4_C1,0x01);

    outportb(MASK_C1,0xfb);     /* Enable IRQ2 input (coming from slave controller */
    outportb(ELCR_C1,0x4);

    for (x=0;x<8;x++) {		/* issue end of interrupt sequence */
        outportb(OCW2_C2,0x20|x);       /* Non-specific EOI command to each interrupt */
        outportb(OCW2_C1,0x20|x);
    }
}
#endif /* NEEDFLOPPY */
