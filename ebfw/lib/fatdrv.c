
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
__attribute__((unused)) static const char *rcsid = "$Id: fatdrv.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 *      FAT Driver:  Handles all operations that deal with a FAT disk.
 *      Author:  Rogelio. R. Cruz, Digital Equipment Corporation.
 *
 *  $Log: fatdrv.c,v $
 *  Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 *  Initial CVS checkin
 *
 * Revision 1.24  1997/10/10  18:13:25  pbell
 * Changed the ReadWriteSectors to use SpinDownFloppyDrive instead
 * of Shutdown because the drive is not actually open at that point.
 *
 * Revision 1.23  1997/07/10  00:23:28  fdh
 * Modified so that errors do not abort subsequent iterations.
 *
 * Revision 1.22  1997/06/03  13:00:46  pbell
 * Fixed a buffer address adjustment in a free call in CloseFatFile
 *
 * Revision 1.21  1997/05/30  18:58:27  pbell
 * modified buffer allocation so that there is always some room to hold
 * put back characters to support fungetc.
 *
 * Revision 1.20  1997/05/23  15:47:01  pbell
 * Added free calls to appropriately release allocated memory;
 * added drive shutdown commands for failure conditions.
 *
 * Revision 1.19  1997/05/02  18:32:49  fdh
 * Moved some structure definitions to an include file.
 * Added the IsBootSectorValid() to check boot sectors.
 * Removed FILE knowledge from ReadWriteRawSector().
 * Moved ReadWriteSectors() into this module where it
 * can better handle disk sector operations as opposed
 * to FILE operations.
 *
 * Revision 1.18  1997/02/21  03:32:52  fdh
 * Corrected "Last Cluster Marker" definitions and
 * and modified to not generate floating point instructions.
 *
 * Revision 1.17  1996/11/17  12:45:50  fdh
 * Corrected macro usage.
 *
 * Revision 1.16  1996/08/20  02:50:07  fdh
 * Changed a typedef name to avoid a conflict.
 *
 * Revision 1.15  1995/12/23  21:09:47  fdh
 * Included nttypes.h.
 *
 * Revision 1.14  1995/12/08  02:44:12  cruz
 * Fixed up in WriteFatFile in which I forgot to increment the
 * source address, thus causing all the clusters to have the
 * same data as the first cluster.
 *
 * Revision 1.13  1995/11/09  21:39:30  cruz
 * Changed return type of CloseFile.
 * Added some missing casting operators.
 *
 * Revision 1.12  1995/11/01  16:35:49  cruz
 * Added compile conditional NEEDFLOPPY around the code.
 *
 * Revision 1.11  1995/10/26  23:34:57  cruz
 * Cleaned up code by fixing prototypes and casting.
 *
 * Revision 1.10  1995/10/04  23:40:40  fdh
 * Moved carriage return.
 *
 * Revision 1.9  1995/10/04  18:24:05  cruz
 * Fixed a bug that prevented the detection of the end of file
 * correctly.
 *
 * Revision 1.8  1995/10/03  20:25:41  cruz
 * Made all local functions static.
 * Added a performance enhancement that tries to guess the next
 * free entry in the fat.
 *
 * Revision 1.7  1995/09/28  20:23:21  cruz
 * Added interface routine for reading and writing raw sectors.
 *
 * Revision 1.6  1995/09/27  19:44:37  cruz
 * Fixed a bug that corrupted the second copy of the FAT
 * Changed the cluster marker for an empty file from 0xFFF to 0x0.
 *
 * Revision 1.5  1995/09/21  20:55:34  cruz
 * Added code for allowing a current working directory (to be
 * changed with flcd command)
 *
 * Revision 1.4  1995/09/21  16:03:27  cruz
 * Added the ability to understand subdirectories.
 * Added a shutdown routine which is to be called
 * for shutting down the disk drive.
 *
 * Revision 1.2  1995/09/15  20:50:53  cruz
 * Fixed bug:  Set the DiskInfoTable entry back to NULL when
 *             all the files for a drive have been closed.
 *
 * Revision 1.1  1995/09/15  19:27:00  cruz
 * Initial revision
 *
 *
 */


#include "system.h"

#ifdef NEEDFLOPPY
#include "lib.h"
#include "nttypes.h"
#include "errno.h"
#include "floppy.h"
#include "fat.h"

#define FLOPPY_IO_BUFFER    (DMA_BUFF_BASE + 0x70000)
/*
 *  File attribute definitions.
 */
#define FILE_ATTRIBUTE_WRITE_PROTECTED  (1<<0)
#define FILE_ATTRIBUTE_HIDDEN_FILE      (1<<1)
#define FILE_ATTRIBUTE_SYSTEM_FILE      (1<<2)
#define FILE_ATTRIBUTE_VOLUME_NAME      (1<<3)
#define FILE_ATTRIBUTE_SUBDIRECTORY     (1<<4)
#define FILE_ATTRIBUTE_ARCHIVE          (1<<5)

/*
 *  Format for PrintDirectoryEntry
 */
#define DIR_DOS_FORMAT 1                /* DOS directory command format.*/
#define DIR_DBM_FORMAT 2                /* Debug monitor's own format.  */

/*
 *  Action to be performed on a cluster list.
 */
#define DELETE_CLUSTERS 0               /* Deletes clusters in list.    */
#define LOAD_CLUSTERS  1                /* Loads data from clusters.    */
#define LAST_CLUSTER   2                /* Returns the last cluster.    */

/*
 *  Definition of bits use in the field "Flags" of a file structure.
 */
#define IN_ROOT_DIRECTORY (1<<8)

/* 
 *  Define what the default file attribute should be.
 */
#define DEFAULT_FILE_ATTRIBUTE FILE_ATTRIBUTE_ARCHIVE  

/*
 *  Logical sector where boot sector resides.
 */
#define BOOT_SECTOR     0



/* ======================================================================
 * =                        MACRO DEFINITIONS                           =
 * ======================================================================
 */
#define BYTES_TO_INTEGER(x) (BytesToInteger((x), sizeof((x))))
#define INTEGER_TO_BYTES(x, y) (IntegerToBytes((x), sizeof((y)), y))
#define BYTES_TO_STRING(dest, src) strncpy ((char *)(dest), (char *)(src), \
                                 sizeof((src))); (dest)[sizeof((src))]='\0'
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


/*
 *  Error codes for the possible errors that may be reported
 *  by this stage.
 */
enum FAT_OP_ERRORS {
    FAT_OP_START                        = (0 << ERROR3_START_BIT),
    FAT_OP_HEAP_MALLOC_FAILED           = (1 << ERROR3_START_BIT),
    FAT_OP_MISSING_FILENAME             = (2 << ERROR3_START_BIT),
    FAT_OP_CANT_GET_DENSITY             = (3 << ERROR3_START_BIT),
    FAT_OP_BOOT_SECT_READ_FAILED        = (4 << ERROR3_START_BIT),
    FAT_OP_FAT_SECT_READ_FAILED         = (5 << ERROR3_START_BIT),
    FAT_OP_CANT_GET_NEXT_DIR_ENTRY      = (6 << ERROR3_START_BIT),
    FAT_OP_DIRECTORY_FULL               = (7 << ERROR3_START_BIT),
    FAT_OP_DIR_UPDATE_FAILED            = (8 << ERROR3_START_BIT),
    FAT_OP_FILE_NOT_FOUND               = (9 << ERROR3_START_BIT), 
    FAT_OP_FAILED_READ                  = (10 << ERROR3_START_BIT),
    FAT_OP_FAILED_WRITE                 = (11 << ERROR3_START_BIT),
    FAT_OP_DISK_FULL                    = (12 << ERROR3_START_BIT),
    FAT_OP_FAT_UPDATE_FAILED            = (13 << ERROR3_START_BIT),
    FAT_OP_OPERATION_ON_DIRECTORY       = (14 << ERROR3_START_BIT),
    FAT_OP_DEVICE_MAP_FAILED		= (15 << ERROR3_START_BIT)
};

/* ======================================================================
 * =                LOCAL-GLOBAL VARIABLE DEFINITIONS                   =
 * ======================================================================
 */

/*
 *  Corresponding string for the codes defined above.
 */
char *Error3Strings[] = {
    "No Errors!",
    "Cannot allocate memory from heap",
    "Missing filename",
    "Cannot determine disk density",
    "Failure detected while reading boot sector",
    "Failure detected while reading FAT sector",
    "Failure detected while reading a directory entry",
    "Directory is full",
    "Directory update failed",
    "File not found",
    "Failed to read a logical sector",
    "Failed to write a logical sector",
    "The disk is full",
    "FAT update failed",
    "Read/Write operation on directory entry not allowed"

};

/*
 *  Holds pointers to structures containing disk information.
 */
static  DISK_INFO DiskInfoTable[MAXIMUM_LOGICAL_DRIVE_COUNT];


/*
 *  Used for returning strings to a calling routine.
 */
#define MAXSTRING 128
static char OutputString[MAXSTRING+1];
#define MAXPATH 128                     /* Maximum path length allowed. */


/* ======================================================================
 * =                STATIC FUNCTION PROTOTYPES                          =
 * ======================================================================
 */
#include "fatdrv.h"

static BOOLEAN FinishOpeningFatFile( FILE * FilePointer, char * FileSpec,
				     UCHAR * Mode );


/* ======================================================================
 * =  AllocateFloppyDrive - Creates a device structure.                 =
 * ======================================================================
 *  OVERVIEW
 *      Allocates a new floppy device structure.
 *
 *  FORM OF CALL
 * 	AllocateFloppyDrive (LogicalDeviceNumber, DriveNumber, DriveType,
 *                           DoNotUseDMA);
 * 
 *  INPUTS
 *      LogicalDeviceNumber:  The logical device number of the device been
 *                            created.
 *      DriveNumber:          Physical drive number (0-3).
 *      DriveType:            Type of floppy drive: ED, HD, DD drive.
 *      DoNotUseDMA:          If TRUE, then all access to this device
 *                            will be done without DMA usage.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation succeeded
 *      STATUS_FAILURE if operation failed.
 */
DBM_STATUS
AllocateFloppyDrive (
                     IN ui    LogicalDeviceNumber,
                     IN ui    DriveNumber,
                     IN ui    DriveType,
                     IN BOOLEAN DoNotUseDMA
                     )
{
    FLOPPY_DRIVE_INFO *DriveInfo;
    static FLOPPY_DRIVE_INFO DriveInfoTable[MAXIMUM_LOGICAL_DRIVE_COUNT];

    DriveInfo = &DriveInfoTable[LogicalDeviceNumber];


    DriveInfo->DriveNumber = DriveNumber;  
    DriveInfo->DriveType = DriveType;
    DriveInfo->DoNotUseDMAAccess = DoNotUseDMA;
    LogicalDrives[LogicalDeviceNumber].DeviceData = (VOID *) DriveInfo;
    LogicalDrives[LogicalDeviceNumber].OpenFile = (FILE * (*)(char  * FileSpec,
                                                              char  * Mode,
                                                              ui    DriveNumber,
                                                              VOID *DeviceData))OpenFatFile;
    LogicalDrives[LogicalDeviceNumber].GetDirectory = (char * (*)(const FILE * FilePointer,
                                                                  UCHAR * FormatString))GetFatDirectory;
    LogicalDrives[LogicalDeviceNumber].WriteData = (ui (*)(FILE *FilePointer, 
                                                           UCHAR *Source,
                                                           ui Size))WriteFatFile;

    LogicalDrives[LogicalDeviceNumber].ReadData = (ui (*)(FILE * FilePointer, 
                                                          UCHAR *Destination,
                                                          ui    Size))ReadFatFile;
    LogicalDrives[LogicalDeviceNumber].CloseFile = (int (*)(const FILE *FilePointer))CloseFatFile;
    LogicalDrives[LogicalDeviceNumber].ReadAFile = (ui (*)(FILE * FilePointer,  
                                                           UCHAR   *Destination))ReadAFatFile;
    LogicalDrives[LogicalDeviceNumber].SetDirectory = (char * (*)(ui DeviceNumber,
                                                                  char *DirectoryString))SetCurrentDirectory;
    SetCurrentDirectory(LogicalDeviceNumber, "\\");
    init_pic();
    return STATUS_SUCCESS;
}



/* ======================================================================
 * =  SetCurrentDirectory - Sets the current directory for the drive.   =
 * ======================================================================
 *  OVERVIEW
 *      Sets the current directory for the specified drive.
 *
 *  FORM OF CALL
 * 	SetCurrentDirectory (DeviceNumber, DirectoryString)
 * 
 *  INPUTS
 *      DeviceNumber:     The logical device number.
 *      DirectoryString:  String containing the new directory, or an
 *                        empty string to obtain the current directory.
 *
 *  RETURNS
 * 	A string pointer to the current directory.
 */
static
char *
SetCurrentDirectory (
                     IN ui DeviceNumber,
                     IN char *DirectoryString
                     )
{
/*
 *  Array for holding the current directory
 */
    static char CurrentDirectory[MAXIMUM_LOGICAL_DRIVE_COUNT][MAXPATH];
    char        *Filename, *Path;

    if (strlen(DirectoryString)) {
        /*
         *  Convert path to the right form \dir1\dir2\...
         */
        Path = GetPath(DirectoryString, CurrentDirectory[DeviceNumber]);
        Filename = GetFilename(DirectoryString);
        if (Filename[0] != '\0') {
            strncat(Path, Filename, MAXPATH);
            strncat(Path, "\\", MAXPATH);
        }
        strncpy(CurrentDirectory[DeviceNumber], Path, MAXPATH);
    }
    return CurrentDirectory[DeviceNumber];
}


/* ======================================================================
 * =  OpenFatFile - Opens a file for reading/writing.                   =
 * ======================================================================
 *  OVERVIEW
 *      Opens a file for reading or writing.
 *
 *  FORM OF CALL
 * 	OpenFatFile (FileSpec, Mode)
 * 
 *  INPUTS
 *      DiskInfo:  Pointer to where disk information can be found.
 *      FileSpec:  File specification.
 *      Mode:   Operation to be performed.
 *              Note that only "w" and "r" are implemented for now.
 *              I have added "d" to indicate that a directory is being
 *              opened (for reading).
 *
 *  RETURNS
 * 	A pointer to a file descriptor or NULL if command not successful.
 */
static FILE * OpenFatFile (
             IN char  FileSpec[],
             IN UCHAR Mode[],
             IN ui    DriveNumber,
             IN VOID *DeviceData
             )
{
    FILE *      FilePointer;
    DISK_INFO * DiskInfo;

    PRTRACE2("Opening file %s\n", FileSpec);

    DiskInfo = GetDiskInfo(DriveNumber, DeviceData);
    if( DiskInfo == NULL )
	return( NULL );

    /*
     *  Allocate HEAP space for the file descriptor, file's data buffer 
     *  and directory entry.
     *  #NOTE the file buffer is extended by 8 characters and offset
     *  to leave room for unget functions.
     */
    if (((FilePointer = (FILE *) malloc(sizeof(FILE))) == NULL) ||
	((FilePointer->Buffer = (char *)malloc(DiskInfo->BytesPerCluster+8)) == NULL) ||
        ((FilePointer->DirectoryEntry = (DIRECTORY_ENTRY_B *) malloc(sizeof(DIRECTORY_ENTRY_B))) == NULL))
    {
	Shutdown( DriveNumber );
        errno = FAT_OP_HEAP_MALLOC_FAILED;
        return NULL;
    }

    /*
     *  Initialize the rest of the file structure 
     */
    FilePointer->Buffer += 8;
    FilePointer->BufferSize = DiskInfo->BytesPerCluster;
    FilePointer->DriveNumber = DriveNumber;
    FilePointer->DiskInfo = (VOID *) DiskInfo;
    FilePointer->Flags = 0;
    FilePointer->BufferIndex = 0;
    FilePointer->BufferEnd = 0;
    FilePointer->EndOfFile = FALSE;

    if( FinishOpeningFatFile( FilePointer, FileSpec, Mode ) )
	return( FilePointer );

    Shutdown( DriveNumber );
    free( FilePointer->DirectoryEntry );
    free( FilePointer->Buffer-8 );  /* the actual buffer is offset by 8 */
    free( FilePointer );
    return( NULL );
}


static BOOLEAN FinishOpeningFatFile (
    FILE *  FilePointer,
    char *  FileSpec,
    UCHAR * Mode)
{
    DIRECTORY_ENTRY_B * DirectoryEntry;
    ui			FirstClusterInFile;
    char *		Filename;
    char *		Path;

    /*
     *  Find the path spec and filename from the file spec.
     */
    Path = GetPath(FileSpec, SetCurrentDirectory(FilePointer->DriveNumber, ""));
    Filename = GetFilename(FileSpec);

    /*
     *  Check to see if we are opening a directory.  If we are, then things
     *  are handled slightly different.  
     */
    if (Mode[0] == 'd') {
        /*
         *  If there's a filename, then this means that the spec did not end
         *  with a "/" and the path is missing a part.  Fix it.
         */
        if (Filename[0] != '\0') {
            strncat(Path, Filename, MAXPATH);
            strncat(Path, "\\", MAXPATH);
        }

        /*
         *  Now we should have a directory path.  Do the search and leave the
         *  "DirectoryEntry" pointer in the File Pointer pointing to the entry
         *  corresponding to the path.  If the directory was not found, then
         *  return FALSE.
         */
        if (FindDirectory(FilePointer, Path) == STATUS_FAILURE) {
            return FALSE;
        }
        FilePointer->Flags |= READING_FILE;
        return TRUE;
    } /* if (Mode[0] == 'd') */


    /*
     *  If there isn't a filename, then abort open.
     */
    if (Filename[0] == '\0') {
        errno = FAT_OP_MISSING_FILENAME;
        return FALSE;
    }

    /*
     *  Now we should have a directory path.  Do the search and leave the
     *  "DirectoryEntry" pointer in the File Pointer pointing to the entry
     *  corresponding to the path.  If the directory was not found, then
     *  return FALSE.
     */
    if (FindDirectory(FilePointer, Path) == STATUS_FAILURE) {
        return FALSE;
    }
        
    PRTRACE2("Searching for file %s\n", Filename);


    /*
     *  Find out if there's a file in the specified directory that
     *  matches the filename for the requested operation.  At this
     *  point, the "Filename" variable should only hold the name
     *  and extension (ie, no path in the name).
     */
    DirectoryEntry = NextDirectoryEntry(FilePointer, TRUE,
                                        (BOOLEAN (*)(DIRECTORY_ENTRY_B *, VOID *))MatchOnFilename,
                                        Filename, 
                                        &FilePointer->DirectoryInSector,
                                        &FilePointer->DirectoryIndex);

    /*
     *  If a file exists, then make sure it's not a directory.
     */
    if ((DirectoryEntry != NULL) && 
        (DirectoryEntryIsSubdirectory(DirectoryEntry))) {
        errno = FAT_OP_OPERATION_ON_DIRECTORY;
        return FALSE;
    }

    /*
     *  If opening a file for writing, check if it already exists.
     */
    if (Mode[0] == 'w') {
        FilePointer->Flags |= WRITING_FILE;
        if (DirectoryEntry != NULL) {
            /*
             *  If it does exist, then we must then throw away the 
             *  file's previous contents.
             */
            FirstClusterInFile = BYTES_TO_INTEGER(DirectoryEntry->FirstCluster);
            DoToChainOfClusters(FilePointer->DiskInfo,
                                FirstClusterInFile,
                                DELETE_CLUSTERS,
                                NULL);
        }
        else {
            /*
             *  If file does not exist, then let's try to get an empty 
             *  directory entry.
             */
            if (errno != NO_ERRORS) {
                return FALSE;
            }
            PRTRACE1("Searching for an unused entry.\n");
            DirectoryEntry = NextDirectoryEntry(FilePointer, TRUE,
                                                (BOOLEAN (*)(DIRECTORY_ENTRY_B *, VOID *))DirectoryEntryEmpty, 
                                                NULL, 
                                                &FilePointer->DirectoryInSector,
                                                &FilePointer->DirectoryIndex);

            if (DirectoryEntry == NULL) {
                if (errno != NO_ERRORS) {
                    return FALSE;
                }
                /*
                 *  No more space is left for adding a new directory entry.
                 *  Let's search for a deleted entry and use that if
                 *  we find one.
                 */
                DirectoryEntry = NextDirectoryEntry(FilePointer, TRUE,
                                                    (BOOLEAN (*)(DIRECTORY_ENTRY_B *, VOID *))DirectoryEntryDeleted,
                                                    NULL, 
                                                    &FilePointer->DirectoryInSector,
                                                    &FilePointer->DirectoryIndex);


                if (DirectoryEntry == NULL) {
                    if (errno != NO_ERRORS) {
                        return FALSE;
                    }
                    errno = FAT_OP_DIRECTORY_FULL;
                    return FALSE;
                }
            }
        }

        /*
         *  Now that we have an entry, fill in the fields.
         *      1) Fill in the Filename and FileExtension fields.
         *      2) Set file attribute to a default value.
         *      3) Set the time and date of change to current time & date
         *      4) Set the first-cluster-in-file field to 0 (empty).
         *      5) Set file size to 0 bytes.
         */
        FillInFilename(DirectoryEntry, (UCHAR *)Filename);
        INTEGER_TO_BYTES(DEFAULT_FILE_ATTRIBUTE, DirectoryEntry->FileAttribute);
        FillInDateAndTime(DirectoryEntry);
#if 0
        INTEGER_TO_BYTES(FilePointer->DiskInfo->LastClusterMarker, DirectoryEntry->FirstCluster);
#endif
        INTEGER_TO_BYTES(0, DirectoryEntry->FirstCluster);
        INTEGER_TO_BYTES(0, DirectoryEntry->FileSize);


        *(DIRECTORY_ENTRY_B *)FilePointer->DirectoryEntry = *DirectoryEntry;

        PRTRACE2("Entry created for file %s\n", Filename);
        PRTRACE2("%s\n", PrintDirectoryEntry (FilePointer->DirectoryEntry, DIR_DOS_FORMAT));

        /*
         *  The fields have been filled in the directory entry.  Update the
         *  directory entry.
         */
        if (UpdateDirectoryEntry(FilePointer) == STATUS_FAILURE) {
            return FALSE;
        }
        return TRUE;
    } /*  if (Mode[0] == 'w') */


    /*
     *  If opening a file for reading, check if it exists.
     */
    if (Mode[0] == 'r') {
        FilePointer->Flags |= READING_FILE;
        if (DirectoryEntry == NULL) {
            if (errno != NO_ERRORS) {
                return FALSE;
            }
            errno = FAT_OP_FILE_NOT_FOUND;
            return FALSE;
        }

        /*
         *  If the file exists, copy its directory entry to the file
         *  descriptor so it can be used later.
         */
        *(DIRECTORY_ENTRY_B *)FilePointer->DirectoryEntry = *DirectoryEntry;

        FilePointer->NextClusterToRead = BYTES_TO_INTEGER(DirectoryEntry->FirstCluster);
        return TRUE;
    } /*  if (Mode[0] == 'r') */

    return FALSE;
}


/* ======================================================================
 * =  GetDiskInfo - Obtains all necessary information about current disk=
 * ======================================================================
 *  OVERVIEW
 *      Gets information about the current disk.
 *
 *  FORM OF CALL
 * 	GetDiskInfo (DriveNumber, DeviceData)
 * 
 *  INPUTS
 *      DriveNumber:  Logical drive number from which disk information
 *                    is to be obtained.
 *      DeviceData:  Pointer to data structure specific to device.
 *
 *  RETURNS
 * 	A pointer to a DISK_INFO structure or NULL if not successful.
 */
static
DISK_INFO *
GetDiskInfo (
             IN ui    DriveNumber,
             IN VOID *DeviceData
             )
{

    DISK_INFO *DiskInfo;

    /*
     *  Get pointer to table entry.
     */
    DiskInfo = &DiskInfoTable[DriveNumber];

    /*
     *  If none is using this disk, then initialize the table entry
     *  Increment the number of users of this disk.
     */
    if (DiskInfo->InUse++ == 0) {
        /*
         *  Remember the logical drive number of this disk.
         */
        DiskInfo->DriveNumber = DriveNumber;

        /*
         *  Allocate DMA space for the disk's I/O buffer.
         *  Reading the boot sector and fat area will be
         *  done thru this buffer. XXX
         */
        DiskInfo->IOBuffer = (UCHAR *)FLOPPY_IO_BUFFER;

        DiskInfo->DeviceData = DeviceData;

        /*
         *  Determine what type of floppy is present.
         */
        if (DetermineDiskDensity((FLOPPY_DRIVE_INFO *)DeviceData,
                                 &DiskInfo->DiskType) == STATUS_FAILURE) {
            errno |= FAT_OP_CANT_GET_DENSITY;
            return NULL;
        }

        /*
         *  Read the boot sector and FAT areas which contain 
         *  essential information about this disk.
         */
        if ((ReadBootSector(DiskInfo) == STATUS_FAILURE) ||
            (ReadFatArea(DiskInfo) == STATUS_FAILURE)) {
            return NULL;
        }
    }

    return DiskInfo;
}




/* ======================================================================
 * =  DetermineDiskDensity - Determines the density of the disk in drive=
 * ======================================================================
 *  OVERVIEW
 *      Determines the density of the disk in the current drive.
 *
 *  FORM OF CALL
 *      DetermineDiskDensity(DriveInfo, &DiskType)
 * 
 *  INPUTS
 *      DriveInfo:  Pointer to structure containing info about the drive.
 *
 *  OUTPUTS
 *      DiskType:  Type of disk will be written here.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation was successful.
 *      STATUS_FAILURE if the operation failed.
 */
static
DBM_STATUS
DetermineDiskDensity (
                      IN FLOPPY_DRIVE_INFO *DriveInfo,
                      OUT ui    *DiskType
                      )
{
    UCHAR       *TemporaryBuffer;
    DBM_STATUS      Status;

    PRTRACE1("Detemining the density of the disk.\n");

    /*
     *  XXX IO Buffer.
     */
    TemporaryBuffer = (UCHAR *)FLOPPY_IO_BUFFER;

    /*
     *  Determine the type of disk in the drive:  ED, HD or DD.
     *  We assumed the highest possible density and then try reading a
     *  sector.  If the read fails, then we try the next density until
     *  the lowest is reached.
     */
    *DiskType = DriveInfo->DriveType+1;   /* Assume density of drive. */

    do {
        (*DiskType)--;

        /*
         *  Let's initialize the drive.  Note that the current drive type
         *  will be dependent on the density of the disk.  This forces the
         *  disktype and drivetype indices to be the same.
         */
        DriveInfo->CurrentDriveType = *DiskType;
        if (InitializeFloppyDrive(DriveInfo) == STATUS_FAILURE) {
            return STATUS_FAILURE;
        }

	printf("\r");
        switch (*DiskType) {
        case ED_DISK_TYPE:
            printf("Extended Density selected  ");
            PRTRACE1("\n");
            break;
        case HD_DISK_TYPE:
            printf("High Density selected      ");
            PRTRACE1("\n");
            break;
        case DD_DISK_TYPE:
            printf("Low Density selected       ");
            PRTRACE1("\n");
            break;
        default:
            PRTRACE1("\nUnknown disk type.  Aborting.\n");
            return STATUS_FAILURE;
        }

        /*
         *  Read any sector and see if it succeeds at the current density.
         */
        Status = ReadLogicalSector(DriveInfo, *DiskType, 0x0, TemporaryBuffer);
    } while ((Status == STATUS_FAILURE) && (*DiskType != DD_DISK_TYPE));
    printf("\n");
    return Status;
}



/* ======================================================================
 * =  ReadBootSector - Reads the boot sector of disk in current drive.  =
 * ======================================================================
 *  OVERVIEW
 *      Reads the boot sector of disk in current drive and fills in 
 *      fields in the DISK_INFO structure such as sectors where FAT,
 *      Root Directory and Data area begin.
 *
 *  FORM OF CALL
 * 	ReadBootSector (BootSector)
 * 
 *  INPUTS
 *      DiskInfo:  Pointer to structure to store disk information.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation was successful.
 *      STATUS_FAILURE if the operation failed.
 */
static
DBM_STATUS
ReadBootSector (
                IN DISK_INFO *DiskInfo
                )
{
    BOOT_SECTOR_B *BootSector;

    BootSector = (BOOT_SECTOR_B *)DiskInfo->IOBuffer;
    if (ReadLogicalSector((FLOPPY_DRIVE_INFO *)DiskInfo->DeviceData, 
                          DiskInfo->DiskType, 
                          BOOT_SECTOR, 
                          (UCHAR *)BootSector) == STATUS_FAILURE) {
        errno |= FAT_OP_BOOT_SECT_READ_FAILED;
        return STATUS_FAILURE;
    }

    ConvertBootSector(BootSector, &DiskInfo->BootSector);

    if (!IsBootSectorValid(&DiskInfo->BootSector)) {
        errno |= FAT_OP_BOOT_SECT_READ_FAILED;
        return STATUS_FAILURE;
    }

    /*
     * Compute where each area of a DOS disk begins.  DOS disks have the
     * Reserved Sectors (which include the Boot Sector) followed by one 
     * or more FAT areas, followed by the root directory area and 
     * finally the data area.  We assumed that each directory entry
     * is 32 bytes long in figuring where the Data Area begins.
     */
    DiskInfo->FirstFatSector = DiskInfo->BootSector.ReservedSectorsCount;
    DiskInfo->FirstDirectorySector = DiskInfo->FirstFatSector + 
                              (DiskInfo->BootSector.NumberOfFats *
                               DiskInfo->BootSector.SectorsPerFat);
    DiskInfo->FirstDataSector = DiskInfo->FirstDirectorySector + 
                              ((DiskInfo->BootSector.EntriesInRootDirectory * 32) /
                               DiskInfo->BootSector.BytesPerSector);

    /*
     *  The number of clusters in this disk is found by taking the total
     *  number of sectors in the disk and dividing by the number of
     *  sectors per cluster.  This number is used to determine if FAT
     *  entries are 12 or 16 bits long.
     */
    DiskInfo->MaxClusterCount = DiskInfo->BootSector.SectorCountInVolume /
                                DiskInfo->BootSector.SectorsPerCluster;
    DiskInfo->BitsPerFatEntry = (DiskInfo->MaxClusterCount < 4096) ? 12 : 16;
    DiskInfo->LastClusterMarker = (1 << DiskInfo->BitsPerFatEntry) - 8;


    /* 
     *  Determine the maximum number of entries in the FAT.  We take into 
     *  account the fact that some sectors are occupied by the FAT and
     *  directory area and also that the first two entries in the FAT
     *  are not used for denoting clusters.
     */
    DiskInfo->MaxEntriesInFat = ((DiskInfo->BootSector.SectorCountInVolume -
                                DiskInfo->FirstDataSector) / 
                                DiskInfo->BootSector.SectorsPerCluster) + 2;

    DiskInfo->BytesPerCluster = DiskInfo->BootSector.SectorsPerCluster *
                                DiskInfo->BootSector.BytesPerSector;

#ifdef PRTRACE
    {
    UCHAR *Array;
    ui    Index;

    Array = (UCHAR *) BootSector;
    PRTRACE2("       Boot Sector located at sector number %d.\n", BOOT_SECTOR);
    PRTRACE1("Adr.  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
    for (Index=0; Index < 512; Index++) {
        if (Index % 16 == 0) {
            PRTRACE2("\n%03X  ", Index);
        }
        PRTRACE2(" %02X", Array[Index]);
    }

    PRTRACE2("\n\nVolume label...............%s\n", 
           DiskInfo->BootSector.VolumeLabel);
    PRTRACE2("Volume ID .................%08X\n", 
           DiskInfo->BootSector.VolumeID);
    PRTRACE2("Manufacturer's ID..........%s\n", 
           DiskInfo->BootSector.ManufacturerCode);
    PRTRACE2("Bytes Per Sector...........%d\n", 
           DiskInfo->BootSector.BytesPerSector);
    PRTRACE2("Sectors Per Cluster........%d\n", 
           DiskInfo->BootSector.SectorsPerCluster);
    PRTRACE2("Reserved Sectors...........%d\n",
           DiskInfo->BootSector.ReservedSectorsCount);
    PRTRACE2("Number of FAT Areas........%d\n", 
           DiskInfo->BootSector.NumberOfFats);
    PRTRACE2("Max. Entries in Root Dir...%d\n", 
           DiskInfo->BootSector.EntriesInRootDirectory);
    PRTRACE2("Sector Count in Volume.....%d\n", 
           DiskInfo->BootSector.SectorCountInVolume);
    PRTRACE2("Media Descriptor...........0x%02X\n", 
           DiskInfo->BootSector.MediaDescriptor);
    PRTRACE2("Sectors Per Fat............%d\n", 
           DiskInfo->BootSector.SectorsPerFat);
    PRTRACE2("Sectors Per Track..........%d\n", 
           DiskInfo->BootSector.SectorsPerTrack);
    PRTRACE2("Read/Write Head Count......%d\n", 
           DiskInfo->BootSector.ReadWriteHeadCount);
    PRTRACE2("Hidden Sector Count........%d\n", 
           DiskInfo->BootSector.HiddenSectorCount);

    PRTRACE2("\nFAT area beings at sector..%d\n", DiskInfo->FirstFatSector);
    PRTRACE2("Directory area beings at ..%d\n", DiskInfo->FirstDirectorySector);
    PRTRACE2("Data area beings at........%d\n", DiskInfo->FirstDataSector);
    PRTRACE2("Max number of Clusters.....%d\n", DiskInfo->MaxClusterCount);
    PRTRACE2("Bits Per Fat Entry.........%d\n\n", DiskInfo->BitsPerFatEntry);
    }
#endif

    return STATUS_SUCCESS;
}



/* ======================================================================
 * =  ConvertBootSector - Converts fields in the boot sector.           =
 * ======================================================================
 *  OVERVIEW
 *      Converts fields in the boot sector to a more manageable form.
 *      For example, bytes representing number are converted to an
 *      integer and bytes representing a string are converted to a NULL
 *      terminated string.
 *
 *  FORM OF CALL
 * 	ConvertBootSector (BS, ConvertedBS)
 * 
 *  INPUTS
 *      BS:  Pointer to the boot sector to be converted.
 *      ConvertedBS:  Pointer to where the converted boot sector 
 *      will be stored.
 *
 *  RETURNS
 * 	Nothing.
 */
static
VOID
ConvertBootSector (
                   IN BOOT_SECTOR_B *BS,
                   IN BOOT_SECTOR_INFO *ConvertedBS
                   )
{
    ConvertedBS->BytesPerSector = BYTES_TO_INTEGER(BS->BytesPerSector);
    ConvertedBS->SectorsPerCluster = BYTES_TO_INTEGER(BS->SectorsPerCluster);
    ConvertedBS->ReservedSectorsCount = BYTES_TO_INTEGER(BS->ReservedSectorsCount);
    ConvertedBS->NumberOfFats = BYTES_TO_INTEGER(BS->NumberOfFats);
    ConvertedBS->EntriesInRootDirectory = BYTES_TO_INTEGER(BS->EntriesInRootDirectory);
    ConvertedBS->SectorCountInVolume = BYTES_TO_INTEGER(BS->SectorCountInVolume);
    ConvertedBS->MediaDescriptor = BYTES_TO_INTEGER(BS->MediaDescriptor);
    ConvertedBS->SectorsPerFat = BYTES_TO_INTEGER(BS->SectorsPerFat);
    ConvertedBS->SectorsPerTrack = BYTES_TO_INTEGER(BS->SectorsPerTrack);
    ConvertedBS->ReadWriteHeadCount = BYTES_TO_INTEGER(BS->ReadWriteHeadCount);
    ConvertedBS->HiddenSectorCount = BYTES_TO_INTEGER(BS->HiddenSectorCount);
    BYTES_TO_STRING(ConvertedBS->ManufacturerCode, BS->ManufacturerCode);
    if (BS->ExtendedBootSignature[0] == 0x29) {
        ConvertedBS->VolumeID = BYTES_TO_INTEGER(BS->VolumeID);        
        BYTES_TO_STRING(ConvertedBS->VolumeLabel, BS->VolumeLabel);
    }
    else {
        ConvertedBS->VolumeID = 0;
        ConvertedBS->VolumeLabel[0] = '\0';
    }
}



/* ======================================================================
 * =  IsBootSectorValid - Cursory check of boot sector integrity.       =
 * ======================================================================
 *  OVERVIEW
 *      Check a few things that "might" indicate that a valid boot sector
 *      is present.
 *
 *  FORM OF CALL
 * 	IsBootSectorValid (BootSector)
 * 
 *  INPUTS
 *      BootSector:  Pointer to the boot sector.
 *
 *  RETURNS
 * 	TRUE if the boot sector is valid.
 */
static
BOOLEAN
IsBootSectorValid (
                   IN BOOT_SECTOR_INFO *BootSector
                   )
{
    if (!BootSector->BytesPerSector) return FALSE;
    if (!BootSector->SectorsPerCluster)  return FALSE;
    if (!BootSector->NumberOfFats)  return FALSE;
    if (!BootSector->SectorsPerFat)  return FALSE;
    return TRUE;
}



/* ======================================================================
 * =  BytesToInteger - Converts bytes representing a number to an int.  =
 * ======================================================================
 *  OVERVIEW
 *      Converts the number of bytes specified into a number.
 *
 *  FORM OF CALL
 * 	BytesToInteger (ArrayofBytes, NumberOfBytes)
 * 
 *  INPUTS
 *      ArrayOfBytes: The array of bytes to be converted to an integer.
 *      NumberOfBytes:  Number of bytes to convert from the array.
 *
 *  RETURNS
 * 	The number created from converting the bytes.
 */
static
ui   
BytesToInteger (
                IN UCHAR ArrayOfBytes[],
                IN ui    NumberOfBytes
                )
{
    ui    Index;
    ui    Number = 0;
    
    for (Index = 0; Index < NumberOfBytes; Index++) {
        Number |= ArrayOfBytes[Index] << (8 * Index);
    }
    return Number;
}



/* ======================================================================
 * =  IntegerToBytes -  Stores an integer into an array of bytes.       =
 * ======================================================================
 *  OVERVIEW
 *      Stores an integer of specified bytes into an array of bytes.
 *
 *  FORM OF CALL
 * 	IntegerToBytes (Value, NumberOfBytes, ArrayofBytes)
 * 
 *  INPUTS
 *      Value:  Value to be written.
 *      NumberOfBytes:  Number of bytes to write to array.
 *
 *  OUTPUTS
 *      ArrayOfBytes: The array of bytes to where the value will be saved.
 *      
 *  RETURNS
 * 	None.
 */
static
VOID
IntegerToBytes (
                IN ui    Value,
                IN ui    NumberOfBytes,
                OUT UCHAR ArrayOfBytes[]
                )
{
    ui    Index;

    for (Index = 0; Index < NumberOfBytes; Index++) {
        ArrayOfBytes[Index] = (UCHAR) Value & 0xFF;
        Value >>= 8;
    }
}



/* ======================================================================
 * =  ReadFatArea - Reads the FAT sectors into memory.                  =
 * ======================================================================
 *  OVERVIEW
 *      Reads the entire FAT area into memory.
 *
 *  FORM OF CALL
 * 	ReadFatArea (DiskInfo)
 * 
 *  INPUTS
 *      DiskInfo:  Pointer to structure to store disk information.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the operation succeeds 
 *      STATUS_FAILURE if the operation fails.
 */
static
DBM_STATUS
ReadFatArea (
             IN DISK_INFO * DiskInfo
             )
{
    ui    Sector, FatSize;
    UCHAR *LoadAddress;

    /*
     *  Allocate HEAP space for the FAT Area.
     */
    FatSize = (DiskInfo->FirstDirectorySector - DiskInfo->FirstFatSector) *
               DiskInfo->BootSector.BytesPerSector;
    if ((DiskInfo->Fat = (UCHAR *) malloc(FatSize)) == NULL) {
        errno = FAT_OP_HEAP_MALLOC_FAILED;
        return STATUS_FAILURE;
    }

    /*
     *  Read ALL the FAT areas into memory where it can be easily 
     *  accessed.  We know that the FAT areas encompass all the
     *  sectors between FirstFatSector and FirstDirectorySector.
     */
    LoadAddress = DiskInfo->Fat;
    for (Sector = DiskInfo->FirstFatSector; Sector < DiskInfo->FirstDirectorySector; 
         Sector++) {
        if (ReadLogicalSector((FLOPPY_DRIVE_INFO *)DiskInfo->DeviceData, 
                              DiskInfo->DiskType, 
                              Sector,  
                              DiskInfo->IOBuffer) == STATUS_FAILURE) {
            errno |= FAT_OP_FAT_SECT_READ_FAILED;
            return STATUS_FAILURE;
        }
        memcpy(LoadAddress, DiskInfo->IOBuffer, 
               DiskInfo->BootSector.BytesPerSector);
        LoadAddress += DiskInfo->BootSector.BytesPerSector;
    }

    DiskInfo->FatChanged = FALSE;
    return STATUS_SUCCESS;
}



#ifdef PRTRACE
/* ======================================================================
 * =  PrintFatArea - Prints the first FAT in a disk.                    =
 * ======================================================================
 *  OVERVIEW
 *      Prints the first FAT area in a disk.
 *
 *  FORM OF CALL
 * 	PrintFatArea (DiskInfo)
 * 
 *  INPUTS
 *      DiskInfo:  Pointer to structure containing disk information.
 *
 *  RETURNS
 * 	None.
 */
static
VOID
PrintFatArea (
             IN DISK_INFO * DiskInfo
             )
{
    ui    Data;
    ui    FatEntry;

    /* 
     * Print out the FAT.
     */
    printf("Printing out FAT...\n");
    for (FatEntry = 0; FatEntry < DiskInfo->MaxEntriesInFat; FatEntry++) {
        Data = GetNextCluster(DiskInfo, FatEntry);
        if (FatEntry % 16 == 0) {
            printf("\n%03X | ", FatEntry);
        }
        if (DiskInfo->BitsPerFatEntry == 16) {
            printf("%04X ", Data);
        }
        else {
            printf("%03X ", Data);
        }
    } /* for */
}
#endif



/* ======================================================================
 * =  GetFreeSpace - Returns the number of bytes free in the disk.      =
 * ======================================================================
 *  OVERVIEW
 *      Returns the number of free bytes.
 *
 *  FORM OF CALL
 * 	GetFreeSpace (DiskInfo)
 * 
 *  INPUTS
 *      DiskInfo:  Pointer to structure to store disk information.
 *
 *  RETURNS
 * 	The number of free bytes.
 */
static
ui
GetFreeSpace (
             IN DISK_INFO * DiskInfo
             )
{
    ui    FatEntry;
    ui    FreeSpace = 0;

    for (FatEntry = 0; FatEntry < DiskInfo->MaxEntriesInFat; FatEntry++) {
        if (GetNextCluster(DiskInfo, FatEntry) == 0) {
            FreeSpace += DiskInfo->BytesPerCluster;
        }
    }
    return FreeSpace;
}



/* ======================================================================
 * =  GetNextCluster - Gets the next cluster from FAT                   =
 * ======================================================================
 *  OVERVIEW
 *      Reads the FAT to obtain the next cluster to be used.
 *
 *  FORM OF CALL
 * 	GetNextCluster (DiskInfo, FatEntryNumber)
 * 
 *  INPUTS
 *      DiskInfo:  Pointer to structure to store disk information.
 *      FatEntryNumber:  Index into the FAT table to obtain the next cluster.
 *              Note that a cluster number can be used as an entry number.
 *
 *  RETURNS
 * 	The contents of the FAT at the specified entry.
 */
static
ui   
GetNextCluster (
                IN DISK_INFO * DiskInfo,
                IN ui    FatEntryNumber
                )
{
    ui    Index;
    UCHAR * Fat = DiskInfo->Fat;

    /*
     *  A FAT entry can be 12 or 16 bits.  Depending on this size
     *  access the table (the FAT) slightly different.  For 16 bits,
     *  the access is simple, simply take the entry # and multiply by 
     *  2 bytes (16 bits).
     */
    if (DiskInfo->BitsPerFatEntry == 16) {
        Index = FatEntryNumber * 2;
        return (BytesToInteger(&Fat [Index], 2) & 0xFFFF);
    }

    /* 
     *  If a Fat entry is 12 bits long, then we need to multiply the 
     *  entry number by 1.5 bytes (12 bits).  If the result is a whole
     *  number, then we can use it as a direct offset and use only the
     *  lower 12 bits. (The result will be a whole number when the 
     *  entry is an odd number).  If not a whole number, then we
     *  use the result as a direct offset and throw away the lower 4
     *  bits (which are not part of the entry).
     */
    Index = (FatEntryNumber * 3) / 2; /* Use integer math */
    if (FatEntryNumber & 1) {
        return ((BytesToInteger(&Fat [Index], 2) >> 4) & 0xFFF);
    }
    else {
        return (BytesToInteger(&Fat [Index], 2) & 0xFFF);
    }
}



/* ======================================================================
 * =  Shutdown - Performs a shutdown of the drive.                      =
 * ======================================================================
 *  OVERVIEW
 *      Performs a shutdown (spin off) of the drive specified.
 *
 *  FORM OF CALL
 * 	Shutdown (DriveNumber)
 * 
 *  INPUTS
 *      DriveNumber:  Logical device number of drive to shutdown.
 *
 *  RETURNS
 *      None.
 */
VOID
Shutdown (
          IN ui DriveNumber
          )
{

    /*
     *  If the disk is in use, then decrement counter.
     *  If it reaches 0, then spin down the disk.
     */
    if (DiskInfoTable[DriveNumber].InUse) {
        DiskInfoTable[DriveNumber].InUse--;
        if (!DiskInfoTable[DriveNumber].InUse) {
            SpinDownFloppyDrive((FLOPPY_DRIVE_INFO *)DiskInfoTable[DriveNumber].DeviceData);
	    if( DiskInfoTable[DriveNumber].Fat != NULL )
		free( DiskInfoTable[DriveNumber].Fat );
	    DiskInfoTable[DriveNumber].Fat = NULL;
        }
    }
}



/* ======================================================================
 * =  GetPath - Gets the path out of a file spec.                       =
 * ======================================================================
 *  OVERVIEW
 *      Gets the path part of a file spec.
 *
 *  FORM OF CALL
 * 	GetPath (FileSpec, CurrentDirectory)
 * 
 *  INPUTS
 *      FileSpec:  String pointer to file spec.
 *                 The file spec can have an absolute or relative path.
 *                 The directory separators can be either UNIX or DOS
 *                 style.  I did this so that I wouldn't have to remember 
 *                 which style DOS uses.  Note, however, that this code
 *                 will convert UNIX to DOS style which is expected thru
 *                 out the rest of this file.
 *      CurrentDirectory:  String pointer to spec. for current directory.
 *
 *  RETURNS
 * 	Pointer to path spec of the form:  \sub1\sub2\...\subN\filename
 */
static
char *
GetPath (
         IN char FileSpec[],
         IN char CurrentDirectory[]
         )
{
    static char Path[MAXPATH+1];
    int         Index;
    int         Separator = -1;

    /*
     *  Check to see if an absolute path has been specified.  If
     *  not, then it must be a relative path.  Prepend the current
     *  directory.
     */
    if ((FileSpec[0] == '/') || (FileSpec[0] == '\\')) {
        Path[0] = '\0';
    }
    else {
        strncpy(Path, CurrentDirectory, MAXPATH);
        Path[MAXPATH] = '\0';
    }

    strncat(Path, FileSpec, MAXPATH);

    /*
     *  Convert it to lowercase & remember the last subdirectory separator.
     */
    Index = 0;
    while (Path[Index] != '\0') {
        if ((Path[Index] = tolower((int)Path[Index])) == '/') {
            Path[Index] = '\\';
        }
        if (Path[Index] == '\\') {
            Separator = Index;
        }
        Index++;
    }

    if (Separator >= 0) {
        Path[++Separator] = '\0';
    }
    return Path;
}



/* ======================================================================
 * =  GetFilename - Gets the filename part of a file spec.              =
 * ======================================================================
 *  OVERVIEW
 *      Gets the filename part of a file spec.
 *
 *  FORM OF CALL
 * 	GetFilename (FileSpec)
 * 
 *  INPUTS
 *      FileSpec:  String pointer to file spec.
 *
 *  RETURNS
 * 	Pointer to static space where filename is copied to.
 */

static
char *
GetFilename (
             IN char FileSpec[]
             )
{
    char        *Pointer;
    static char Filename[15];
    ui          Index;

    if (((Pointer = (char *) strrchr(FileSpec, '\\')) == NULL) &&
        ((Pointer = (char *) strrchr(FileSpec, '/')) == NULL)) {
        Pointer = FileSpec;
    }
    else {
        Pointer++;                      /* Skip over the separator.     */
    }
    Index = 0;
    while ((Index < 15) && (*Pointer != '\0')) {
        Filename[Index++] = tolower((int) *Pointer);
        Pointer++;
    }
    Filename[Index] = '\0';
    return Filename;
}



/* ======================================================================
 * =  FindDirectory - Parses the path spec until it reaches the last sub=
 * ======================================================================
 *  OVERVIEW
 *      Parses the path spec until it reaches the last subdirectory in the
 *      path.  Then, the file pointer is set to point to that entry.
 *
 *  FORM OF CALL
 * 	FindDirectory(FilePointer, Path)
 * 
 *  INPUTS
 *      FilePointer:  Pointer to file structure.
 *      Path:  Path specification to be parsed.  It's expected that the
 *             path will have the form of: \sub1\sub2\...\subN\
 *
 *  RETURNS
 * 	STATUS_SUCCESS if operation is successful.
 *      STATUS_FAILURE otherwise
 */
static
DBM_STATUS
FindDirectory (
              IN FILE * FilePointer,
              IN char * Path
              )
{
    char        LocalCopy[MAXSTRING+1];
    char        *Pointer;
    char        *CurrentChar;


    DIRECTORY_ENTRY_B *DirectoryEntry;

    strncpy(LocalCopy, Path, MAXSTRING);
    LocalCopy[MAXSTRING] = '\0';
    
    /*
     *  Start all searches from the root directory.
     */

    FilePointer->Flags |= IN_ROOT_DIRECTORY;

    /*
     *  Search the local copy of the path for separators.  First, skip
     *  over the first character which is the root separator since we
     *  assume that all paths at this point are absolute.
     */
    Pointer = &LocalCopy[1]; 
    for (CurrentChar = Pointer; *CurrentChar != '\0'; CurrentChar++) {
        /*
         *  If we find another separator, then we have a new subdirectory
         *  name.  Find out if it's present on the disk.
         */
        if (*CurrentChar == '\\') {
            *CurrentChar++ = '\0';
            DirectoryEntry = NextDirectoryEntry(FilePointer, TRUE, 
                                                (BOOLEAN (*)(DIRECTORY_ENTRY_B *, VOID *))MatchOnDirectory, 
                                                Pointer, 
                                                &FilePointer->DirectoryInSector,
                                                &FilePointer->DirectoryIndex);
            if (DirectoryEntry == NULL) {
                if (errno == NO_ERRORS) {
                    errno = FAT_OP_FILE_NOT_FOUND;
                }
                return STATUS_FAILURE;
            } 

            /*
             *  We found a directory that matched. Clear the in_root_dir flag.
             */
            FilePointer->Flags &= ~IN_ROOT_DIRECTORY;
            *(DIRECTORY_ENTRY_B *)FilePointer->DirectoryEntry = *DirectoryEntry;
            Pointer = CurrentChar;              /* Move on to the next part */

            /*
             *  If the directory entry that matched has a start cluster of 0,
             *  then this means that it must be a subdirectory of the root directory.
             *  We'll need to handle things differently.
             */
            if (!BYTES_TO_INTEGER(DirectoryEntry->FirstCluster)) {
                /*
                 *  Indicate that we are at the root directory now.
                 */
                FilePointer->Flags |= IN_ROOT_DIRECTORY;                    
            }
        } 
            
    } /* for */

    return STATUS_SUCCESS;
}



/* ======================================================================
 * =  NextDirectoryEntry - Returns the next dir. entry that matches.    =
 * ======================================================================
 *  OVERVIEW
 *      Returns the next directory entry that matches a specified condition.
 *      The caller must pass in a function for determining when there's
 *      a match.
 *
 *  FORM OF CALL
 *      NextDirectoryEntry(FilePointer, Restart, MatchFunction, Argument)
 * 
 *  INPUTS
 * 	FilePointer:  Pointer to File descriptor.
 *      Restart:  A Boolean value specifying whether a search should
 *                be restarted or the next entry should be returned.
 *      MatchFunction:  Function that determines whether there's a
 *                      match between the current directory entry
 *                      and the search condition.
 *      Argument:       Argument to be passed along to the function.
 *
 *  OUTPUTS
 *      These two output parameters are only valid when the returned
 *      value is non-NULL.
 *      FoundInSector:  The logical sector number in which the match
 *                      was found.
 *      FoundInEntryNumber:  Entry index where match was found.
 *
 *  RETURNS
 * 	NULL if no entry that matches is found or a pointer to
 *      the entry that matched.  Note that the pointer is into the
 *      disk's I/O Buffer.  This means that if any other disk operations
 *      are performed, the data pointed to by the pointer will be 
 *      wiped out.  Therefore, the caller should copy the data to
 *      another area if it will needed at a future time.
 */
static
DIRECTORY_ENTRY_B *
NextDirectoryEntry (
                    IN FILE *FilePointer,
                    IN BOOLEAN Restart,
                    IN BOOLEAN (*MatchFunction)(DIRECTORY_ENTRY_B *, VOID *),
                    IN VOID * Argument,
                    OUT ui    *FoundInSector,
                    OUT ui    *FoundInEntryNumber
                    )
{
    static ui    NextSectorToRead;
    static ui    LastSectorToRead;
    static ui    CurrentCluster;
    static ui    CurrentEntryIndex;

    ui          MaxEntriesPerSector;
    ui          Index;
    DISK_INFO   *DiskInfo;
    DIRECTORY_ENTRY_B *DirectoryEntry;
    DIRECTORY_ENTRY_B *DirectorySector;
    BOOLEAN Match;

    DiskInfo = (DISK_INFO *)FilePointer->DiskInfo;

    /*
     *  File I/O is always done thru this buffer.
     */
    DirectorySector = (DIRECTORY_ENTRY_B *) DiskInfo->IOBuffer;

    /*
     *  Compute the number of directory entries in one sector.
     */
    MaxEntriesPerSector = DiskInfo->BootSector.BytesPerSector / 32;

    /*
     *  If we are restarting the search, then reset the
     *  static variables that are kept between calls.
     *  Different conditions exist if we are searching the
     *  root directory.
     */
    if (Restart) {
        /*
         *  If we are looking at the root directory, then the
         *  first and last sector to read are known in advanced.
         */
        if (FilePointer->Flags & IN_ROOT_DIRECTORY) {
            NextSectorToRead = DiskInfo->FirstDirectorySector;
            LastSectorToRead = DiskInfo->FirstDataSector;
        }
        else {
            /*
             *  If we are looking at a subdirectory, then get the
             *  first cluster pointing to where the entries are.
             */
            DirectoryEntry = (DIRECTORY_ENTRY_B *) FilePointer->DirectoryEntry;
            CurrentCluster = BYTES_TO_INTEGER(DirectoryEntry->FirstCluster);
            NextSectorToRead = ClusterToLogicalSector(DiskInfo, CurrentCluster);
            LastSectorToRead = NextSectorToRead + DiskInfo->BootSector.SectorsPerCluster;
            CurrentCluster = GetNextCluster(DiskInfo, CurrentCluster);
        }
        
        CurrentEntryIndex = 0;
    } /* if (Restart) */

    do {
        /*
         *  While there are sectors to search do the following.
         */
        while (NextSectorToRead < LastSectorToRead) {

            /*
             *  If the current entry index is 0, this tells us that
             *  we need to read the next sector.
             */
            if ((CurrentEntryIndex == 0) &&
                (ReadLogicalSector((FLOPPY_DRIVE_INFO *)DiskInfo->DeviceData, 
                                  DiskInfo->DiskType, 
                                  NextSectorToRead, 
                                  (UCHAR *)DirectorySector) == STATUS_FAILURE)) {
                PRTRACE2("Failure reading sector %d\n", NextSectorToRead);
                errno |= FAT_OP_CANT_GET_NEXT_DIR_ENTRY;
                return NULL;
            }

            for (Index = CurrentEntryIndex; 
                 Index < MaxEntriesPerSector;
                 Index++) {
                /*
                 *  Search from the current entry to the last entry in
                 *  the current sector.  If a match is found, then we
                 *  store the index at which the search should be 
                 *  restarted the next time around.
                 */
                DirectoryEntry = &DirectorySector[Index];
                Match = MatchFunction(DirectoryEntry, Argument);

                /*
                 *  Output the sector and index in which the current 
                 *  directory entry resides.
                 */
                *FoundInSector = NextSectorToRead;
                *FoundInEntryNumber = Index;

                /*
                 *  Increment the current entry counter.  If it has
                 *  reached the maximum number, then reset the entry
                 *  counter and increment the sector counter so the 
                 *  next time around it reads a new sector.
                 */
                CurrentEntryIndex++;
                if (CurrentEntryIndex == MaxEntriesPerSector) {
                    CurrentEntryIndex = 0;
                    NextSectorToRead++;
                }

                if (Match) {
                    return DirectoryEntry;
                }
                
                /*
                 *  Abort the search if the current entry is the last directory entry.
                 */
                if (DirectoryEntryEmpty(DirectoryEntry)) {
                    return NULL;
                }
            } /* for */
        } /* while (NextSectorToRead < LastSectorToRead) */

        if (FilePointer->Flags & IN_ROOT_DIRECTORY) {
            /*
             *  If we are looking at the root directory, then 
             *  we are done searching the directory at this
             *  point and no match has been found, so return NULL.
             */
            break;
        }
        else {
            /*
             *  We are looking at a subdirectory.  Get the next cluster
             *  that makes up the subdirectory.
             */
            NextSectorToRead = ClusterToLogicalSector(DiskInfo, CurrentCluster);
            LastSectorToRead = NextSectorToRead + DiskInfo->BootSector.SectorsPerCluster;
            CurrentCluster = GetNextCluster(DiskInfo, CurrentCluster);
        }
    } while (CurrentCluster < DiskInfo->LastClusterMarker);
    return NULL;
}



/* ======================================================================
 * =  ClusterToLogicalSector - Converts a cluster to a logical sector # =
 * ======================================================================
 *  OVERVIEW
 *      Converts a cluster number to a logical sector number.
 *
 *  FORM OF CALL
 * 	ClusterToLogicalSector (DiskInfo, Cluster)
 * 
 *  INPUTS
 *      DiskInfo:  Pointer to structure to store disk information.
 *      Cluster:   Cluster number to be converted to sector number.
 *
 *  RETURNS
 * 	The logical sector number corresponding to the cluster number.
 */
static
ui    
ClusterToLogicalSector (
                        IN DISK_INFO * DiskInfo,
                        IN ui    Cluster
                        )
{
    ui    Sector;

    /*
     *  Because the first two FAT entries contain the media descriptor,
     *  the actual zero cluster on the volume is FAT entry 2.
     */  
    Cluster -= 2;

    Sector = Cluster * DiskInfo->BootSector.SectorsPerCluster;

    /*
     *  Sector now points to the sector number relative to the beginning
     *  of the DATA range.  We need to add the sector number where the
     *  DATA section begins to obtain the sector number relative to
     *  the first sector of the disk.
     */
    Sector += DiskInfo->FirstDataSector;

    return Sector;
}



/* ======================================================================
 * =  MatchOnFilename - Returns true if filenames match.                =
 * ======================================================================
 *  OVERVIEW
 *      Returns true if the filename passed in matches the filename in
 *      the directory entry passed in.
 *
 *  FORM OF CALL
 * 	MatchOnFilename (DirectoryEntry, Filename)
 * 
 *  INPUTS
 *      DirectoryEntry:  Directory entry containing filename.
 *      Filename:  Filename that we want to match on.
 *
 *  RETURNS
 * 	True if there's a match in the filenames.
 */
static
BOOLEAN
MatchOnFilename (
               IN DIRECTORY_ENTRY_B *DirectoryEntry,
               IN UCHAR Filename[]
               )
{
    UCHAR  FilenameWithExtension[13];

    /*
     *  If the entry is not valid, then return FALSE.
     */
    if (!DirectoryEntryIsValid(DirectoryEntry)) {
        return FALSE;
    }

    /*
     *  If first character is 0x05, then replace it by the real ASCII
     *  code = 0xE5 before doing the comparison.
     */
    if (DirectoryEntry->Filename[0] == 0x05) {
        DirectoryEntry->Filename[0] = 0xE5;
    }

    GetFilenameWithExtension(DirectoryEntry->Filename,
                             DirectoryEntry->FileExtension,
                             FilenameWithExtension);
    return (strncmp((char *)Filename, (char *)FilenameWithExtension, 12) == 0);
}



/* ======================================================================
 * =  MatchOnDirectory - Returns true if entry matches and it's a dir.  =
 * ======================================================================
 *  OVERVIEW
 *      Returns true if the filename passed in matches the filename in
 *      the directory entry passed in AND the entry is a directory.
 *
 *  FORM OF CALL
 * 	MatchOnDirectory (DirectoryEntry, Filename)
 * 
 *  INPUTS
 *      DirectoryEntry:  Directory entry containing filename.
 *      Filename:  Filename that we want to match on.
 *
 *  RETURNS
 * 	True if there's a match in the filenames.
 */
static
BOOLEAN
MatchOnDirectory (
                  IN DIRECTORY_ENTRY_B *DirectoryEntry,
                  IN UCHAR Filename[]
                  )
{
    return (DirectoryEntryIsSubdirectory(DirectoryEntry) &&
            MatchOnFilename(DirectoryEntry, Filename));
            
}



/* ======================================================================
 * =  MatchOnStandardDirectory - Returns true if entry is std. dir entry=
 * ======================================================================
 *  OVERVIEW
 *      Returns true if the directory entry is valid and not a volume 
 *      name.
 *
 *  FORM OF CALL
 * 	MatchOnStandardDirectory (DirectoryEntry, NotUsed)
 * 
 *  INPUTS
 *      DirectoryEntry:  Directory entry to compare.
 *      NotUsed:
 *
 *  RETURNS
 * 	True if there's a match.  False otherwise
 */
static
BOOLEAN
MatchOnStandardDirectory (
                          IN DIRECTORY_ENTRY_B *DirectoryEntry,
                          IN VOID *NotUsed
                          )
{
    return (DirectoryEntryIsValid(DirectoryEntry) && 
            (!DirectoryEntryIsVolumeName(DirectoryEntry)));
}

#if 0
/* ======================================================================
 * =  DirectoryEntryIsHidden - Returns TRUE if entry is a hidden file.  =
 * ======================================================================
 *  OVERVIEW
 *      Returns true if the entry is marked hidden.
 *
 *  FORM OF CALL
 * 	DirectoryEntryIsHidden (DirectoryEntry)
 * 
 *  INPUTS
 *      DirectoryEntry:  Pointer to directory entry.
 *
 *  RETURNS
 * 	TRUE if entry is has volume name.  FALSE, otherwise
 */
static
BOOLEAN
DirectoryEntryIsHidden (
                        IN DIRECTORY_ENTRY_B *DirectoryEntry
                        )
{
    ui    Attribute;
    Attribute = BYTES_TO_INTEGER(DirectoryEntry->FileAttribute);
    return (Attribute & FILE_ATTRIBUTE_HIDDEN_FILE);
}



/* ======================================================================
 * =  MatchOnStandardDirectory - Returns true if entry is std. dir entry=
 * ======================================================================
 *  OVERVIEW
 *      Returns true if the directory entry is not a hidden file or a
 *      volume name.  That is, it will return true on regalar files and
 *      subdirectory entries.
 *
 *  FORM OF CALL
 * 	MatchOnStandardDirectory (DirectoryEntry, NotUsed)
 * 
 *  INPUTS
 *      DirectoryEntry:  Directory entry to compare.
 *      NotUsed:
 *
 *  RETURNS
 * 	True if there's a match.  False otherwise
 */
static
BOOLEAN
MatchOnStandardDirectory (
                          IN DIRECTORY_ENTRY_B *DirectoryEntry,
                          IN VOID *NotUsed
                          )
{
    return (DirectoryEntryIsValid(DirectoryEntry) && 
            (!DirectoryEntryIsVolumeName(DirectoryEntry)) &&
            (!DirectoryEntryIsHidden(DirectoryEntry)));
}
#endif



/* ======================================================================
 * =  DirectoryEntryEmpty - Returns true if directory entry is empty.   =
 * ======================================================================
 *  OVERVIEW
 *      Returns true if the directory entry is empty.
 *
 *  FORM OF CALL
 * 	DirectoryEntryEmpty (DirectoryEntry)
 * 
 *  INPUTS
 *      DirectoryEntry:  Pointer to directory entry.
 *
 *  RETURNS
 * 	TRUE if entry is empty. FALSE, otherwise
 */
static
BOOLEAN
DirectoryEntryEmpty (
                     IN DIRECTORY_ENTRY_B *DirectoryEntry
                     )
{
    return (DirectoryEntry->Filename[0] == 0);
}



/* ======================================================================
 * =  DirectoryEntryDeleted - Returns TRUE if entry has been deleted.   =
 * ======================================================================
 *  OVERVIEW
 *      Returns true if the directory entry is marked deleted.
 *
 *  FORM OF CALL
 * 	DirectoryEntryDeleted (DirectoryEntry)
 * 
 *  INPUTS
 *      DirectoryEntry:  Pointer to directory entry.
 *
 *  RETURNS
 * 	TRUE if entry is marked deleted.  FALSE, otherwise
 */
static
BOOLEAN
DirectoryEntryDeleted (
                       IN DIRECTORY_ENTRY_B *DirectoryEntry
                       )
{
    return (DirectoryEntry->Filename[0] == 0xE5);
}



/* ======================================================================
 * =  DirectoryEntryIsSubdirectory - Returns TRUE if entry is a sub-dir.=
 * ======================================================================
 *  OVERVIEW
 *      Returns true if the entry is a subdirectory.
 *
 *  FORM OF CALL
 * 	DirectoryEntryIsSubdirectory (DirectoryEntry)
 * 
 *  INPUTS
 *      DirectoryEntry:  Pointer to directory entry.
 *
 *  RETURNS
 * 	TRUE if entry is a subdirectory entry.  FALSE, otherwise
 */
static
BOOLEAN
DirectoryEntryIsSubdirectory (
                              IN DIRECTORY_ENTRY_B *DirectoryEntry
                              )
{
    ui    Attribute;
    Attribute = BYTES_TO_INTEGER(DirectoryEntry->FileAttribute);
    return (Attribute & FILE_ATTRIBUTE_SUBDIRECTORY);
}



/* ======================================================================
 * =  DirectoryEntryIsVolumeName - Returns TRUE if entry is a volume name=
 * ======================================================================
 *  OVERVIEW
 *      Returns true if the entry is the volume name.
 *
 *  FORM OF CALL
 * 	DirectoryEntryIsVolumeName (DirectoryEntry)
 * 
 *  INPUTS
 *      DirectoryEntry:  Pointer to directory entry.
 *
 *  RETURNS
 * 	TRUE if entry is volume name.  FALSE, otherwise
 */
static
BOOLEAN
DirectoryEntryIsVolumeName (
                            IN DIRECTORY_ENTRY_B *DirectoryEntry
                            )
{
    ui    Attribute;
    Attribute = BYTES_TO_INTEGER(DirectoryEntry->FileAttribute);
    return (Attribute & FILE_ATTRIBUTE_VOLUME_NAME);
}




/* ======================================================================
 * =  DirectoryEntryIsValid - Returns true if directory entry is valid. =
 * ======================================================================
 *  OVERVIEW
 *      Returns true if the directory entry is not empty or marked deleted.
 *
 *  FORM OF CALL
 * 	DirectoryEntryIsValid (DirectoryEntry)
 * 
 *  INPUTS
 *      DirectoryEntry:  Pointer to directory entry.
 *
 *  RETURNS
 * 	TRUE if entry is not empty or marked deleted.  FALSE, otherwise
 */
static
BOOLEAN
DirectoryEntryIsValid (
                       IN DIRECTORY_ENTRY_B *DirectoryEntry
                       )
{
    return (!DirectoryEntryEmpty(DirectoryEntry) &&
            !DirectoryEntryDeleted(DirectoryEntry));
}



/* ======================================================================
 * =  GetFilenameWithExtension - Returns filename.ext.                  =
 * ======================================================================
 *  OVERVIEW
 *      Creates the string consisting of filename followed by extension.
 *
 *  FORM OF CALL
 * 	GetFilenameWithExtension (Filename, Extension, FilenameWithExtension)
 * 
 *  INPUTS
 *      Filename:  Pointer to an 8-byte string containing the filename.
 *      Extension: Pointer to a 3-byte string containing the extension.
 *
 *  OUTPUTS
 *      FilenameWithExtension: Pointer to space where resulting string
 *                 will be saved.  The filename will have the form of
 *                 name.ext or name if not file extension exists.
 *
 *  RETURNS
 * 	The length of the resulting string.
 */
static
ui   
GetFilenameWithExtension (
                          IN UCHAR Filename[],
                          IN UCHAR Extension[],
                          OUT UCHAR FilenameWithExtension[]
                          )
{
    ui     Index, Index2;

    /*
     *  Copy the filename to the output string up to 8 characters or
     *  up to the first padding space.
     */
    for (Index = 0;
         (Index < 8) && (Filename[Index] != ' ');
         Index++) {
        FilenameWithExtension[Index] = tolower(Filename[Index]);
    }
    
   for (Index2 = 0;
         (Index2 < 3) && (Extension[Index2] != ' ');
         Index2++) {

        if (Index2 == 0) {
            FilenameWithExtension[Index++] = '.';
        }

        FilenameWithExtension[Index++] = tolower(Extension[Index2]);
    }
    
    FilenameWithExtension[Index] = '\0';
    return Index;
}



/* ======================================================================
 * =  DoToChainOfClusters - Performs an action on chain of clusters.    =
 * ======================================================================
 *  OVERVIEW
 *      Goes thru the list of clusters that makes up a file and performs
 *      a specified function.
 *
 *  FORM OF CALL
 * 	DoToChainOfClusters(DiskInfo, StartingCluster, Action, LoadAddress)
 * 
 *  INPUTS
 *      DiskInfo:  Pointer to structure to store disk information.
 *      StartingCluster:  First cluster in the list.
 *      Action:  Input controlling function to be performed on chain
 *               of clusters.
 *               LAST_CLUSTER    - Returns the last cluster in chain.
 *               DELETE_CLUSTERS - Marks all clusters in the chain as
 *                                 available.
 *               LOAD_CLUSTERS   - Loads the data the clusters point to.
 *      LoadAddrss:  Address where data from clusters will be loaded to.
 *
 *  RETURNS
 * 	The last cluster in the chain IF the action is LAST_CLUSTER.
 *      STATUS_SUCCESS for any other action if the action succeeded.
 *      STATUS_FAILURE if it didn't.
 */
static
ui   
DoToChainOfClusters (
                     IN DISK_INFO * DiskInfo,
                     IN ui    StartingCluster,
                     IN ui    Action,
                     IN UCHAR *LoadAddress 
                     )
{
    ui    CurrentCluster, NextCluster;
    ui    StartingSector, Sector;

    PRTRACE3("Last Cluster Marker: 0x%x (%d)\n",
	     DiskInfo->LastClusterMarker,
	     DiskInfo->LastClusterMarker);

    /*
     *  Go thru the chain of clusters.
     */
    CurrentCluster = StartingCluster;
    while ((CurrentCluster < DiskInfo->LastClusterMarker) &&
           (CurrentCluster >= 2)) {
	PRTRACE2("=%d", CurrentCluster);
        NextCluster = GetNextCluster(DiskInfo, CurrentCluster);

        switch (Action) {
        case LAST_CLUSTER:
            if (NextCluster >= DiskInfo->LastClusterMarker) {
                return CurrentCluster;
            }
            break;

        case DELETE_CLUSTERS:
            /*
             *  To delete a cluster, just change its FAT entry to 0x000.
             */
            ChangeFatEntry(DiskInfo, CurrentCluster, 0x0);
            break;

        case LOAD_CLUSTERS:
            /*
             *  To load a cluster into memory, find out which logical
             *  sector it maps to.  Then load all the sectors that
             *  make up the cluster.
             */
            StartingSector = ClusterToLogicalSector(DiskInfo, CurrentCluster);
            for (Sector = StartingSector;
                 Sector < StartingSector + DiskInfo->BootSector.SectorsPerCluster;
                 Sector++) {
                if (ReadLogicalSector((FLOPPY_DRIVE_INFO *)DiskInfo->DeviceData, 
                                      DiskInfo->DiskType, 
                                      Sector, 
                                      DiskInfo->IOBuffer) == STATUS_FAILURE) {
                    return STATUS_FAILURE;
                }
                memcpy(LoadAddress, DiskInfo->IOBuffer, 
                       DiskInfo->BootSector.BytesPerSector);
                LoadAddress += DiskInfo->BootSector.BytesPerSector;
            }
            break;
        }

        CurrentCluster = NextCluster;
    }

    PRTRACE3("\nLast Cluster Marker Encountered: 0x%x (%d)\n",
	     CurrentCluster,
	     CurrentCluster);

    return STATUS_SUCCESS;
}



/* ======================================================================
 * =  ChangeFatEntry - Changes FAT entry to specified value.            =
 * ======================================================================
 *  OVERVIEW
 *      Changes the specified FAT entry in memory.
 *
 *  FORM OF CALL
 * 	ChangeFatEntry (DiskInfo, FatEntryNumber, Data)
 * 
 *  INPUTS
 *      DiskInfo:  Pointer to structure to store disk information.
 *      FatEntryNumber:  Entry in FAT table to be changed.
 *      Data:  Data to be written to entry.
 *
 *  RETURNS
 * 	None.
 */
static
VOID
ChangeFatEntry (
                IN DISK_INFO * DiskInfo,
                IN ui    FatEntryNumber,
                IN ui    Data
                )
{
    ui    Index;
    ui    CurrentData;

    UCHAR * Fat = DiskInfo->Fat;
    ui    NumberOfFats = DiskInfo->BootSector.NumberOfFats;
    ui    NewData;

    while (NumberOfFats != 0) {
        /*
         *  A FAT entry can be 12 or 16 bits.  Depending on this size
         *  access the table (the FAT) slightly different.  For 16 bits,
         *  the access is simple, simply take the entry # and multiply by 
         *  2 bytes (16 bits).
         */
        if (DiskInfo->BitsPerFatEntry == 16) {
            Index = FatEntryNumber * 2;
            Fat [Index] = Data & 0xFFFF;
        }
        else {
            /* 
             *  If a Fat entry is 12 bits long, then we need to multiply the 
             *  entry number by 1.5 bytes (12 bits).  Because we can only do
             *  accesses in a byte granularity (8 bits), we need to make sure 
             *  that when we write the data to the fat we preserve the 4 bits
             *  that are not part of our current entry.  We do this by reading
             *  first and ORing our data in before writing it out.
             */
            Index = (FatEntryNumber * 3) / 2; /* Use integer math */
            CurrentData = BytesToInteger(&Fat [Index], 2);

            /*  
             *  If the Index computed above is not a whole number then we
             *  must preserve the lower 4 bits of the data read which 
             *  correspond to the entry preceeding the one we're changing.
             *  If the Index is a whole number then we must preserve the 
             *  upper 4 bits of the data read which correspond to the
             *  entry following the one we're changing.
             */
            if (FatEntryNumber & 1) {
                NewData = (CurrentData & 0x000F) | ((Data << 4) & 0xFFF0);
            }   
            else {
                NewData = (CurrentData & 0xF000) | (Data & 0x0FFF);
            }

            IntegerToBytes(NewData, 2, &Fat [Index]);

            /*
             * Make sure that the change occurs in all the FATs present.
             */
            NumberOfFats--;
            Fat += DiskInfo->BootSector.SectorsPerFat * DiskInfo->BootSector.BytesPerSector;
            DiskInfo->FatChanged = TRUE;
        }
    } /* while (NumberOfFats != 0) */
}



/* ======================================================================
 * =  FillInFilename - Fill in fields in directory entry with filename. =
 * ======================================================================
 *  OVERVIEW
 *      Fills in the fields in a directory entry that correspond to the
 *      the filename.
 *
 *  FORM OF CALL
 * 	FillInFilename(DirectoryEntry, Filename)
 * 
 *  INPUTS
 *      DirectoryEntry:  Pointer to directory entry to be filled in.
 *      Filename:  File name to be broken up into name.ext.
 *
 *  RETURNS
 * 	None.
 */
static
VOID
FillInFilename (
                IN DIRECTORY_ENTRY_B *DirectoryEntry,
                IN UCHAR Filename[]
                )
{
    ui     Index;
    /*
     *  Pad the Filename and FileExtension fields with spaces.
     */
    for (Index = 0; Index < 8; Index++) {
        DirectoryEntry->Filename[Index] = ' ';
        DirectoryEntry->FileExtension[Index % 3 ] = ' ';
     } 
  
    /*
     *  Copy the filename to the Filename field.  We are done when one
     *  of the following occurs: 
     *          1) 8 legal characters have been copied.
     *          2) The end of the string is encountered.
     *          3) A period is encountered
     */
    Index = 0;
    while ( (Index < 8) && (Filename[Index] != '\0') &&
                           (Filename[Index] != '.')) {
        DirectoryEntry->Filename[Index] = toupper(Filename[Index]);
        Index++;
    }
    
    /*
     *  See if filename has an extension.  If it does, then fill in the
     *  extension field.
     */
    Filename = (UCHAR *) strchr((char *)Filename, '.');
    if (Filename != NULL) {
        Filename++;                     /* Skip the period.             */
        Index = 0;
        while ( (Index < 3) && (Filename[Index] != '\0')) {
            DirectoryEntry->FileExtension[Index] = toupper(Filename[Index]);
            Index++;
        }
    }
}



/* ======================================================================
 * =  FillDateAndTime - Fill in date and time of last change fields.    =
 * ======================================================================
 *  OVERVIEW
 *      Fills in the date and time of last change in a directory entry.
 *
 *  FORM OF CALL
 * 	FillInDateAndTime(DirectoryEntry)
 * 
 *  INPUTS
 *      DirectoryEntry:  Pointer to directory entry to be filled in.
 *
 *  RETURNS
 * 	None.
 */
static
VOID
FillInDateAndTime (
                   IN DIRECTORY_ENTRY_B *DirectoryEntry
                   )
{
    time_t  CurrentTime;
    struct tm *TimeStructure;
    ui    ChangeTime, ChangeDate;

    /*
     *  Get the current time and date.
     */
    CurrentTime = time(NULL);
    TimeStructure = localtime(&CurrentTime);

    /*
     *  Convert time to DOS representation.
     */
    ChangeTime = (ui)((TimeStructure->tm_sec & 0x1f) >> 1);
    ChangeTime |= (TimeStructure->tm_min & 0x3F) << 5;
    ChangeTime |= (TimeStructure->tm_hour & 0x1F) << 11;
    INTEGER_TO_BYTES(ChangeTime, DirectoryEntry->TimeOfLastChange);

    /*
     *  Convert date to DOS representation.
     */
    ChangeDate = (ui)(TimeStructure->tm_mday & 0x1F);
    ChangeDate |= (TimeStructure->tm_mon & 0xF) << 5;
    ChangeDate |= ((TimeStructure->tm_year - 80) & 0x7F) << 9;
    INTEGER_TO_BYTES(ChangeDate, DirectoryEntry->DateOfLastChange);
}



/* ======================================================================
 * =  UpdateDirectoryEntry - Updates directory entry for file.          =
 * ======================================================================
 *  OVERVIEW
 *      Updates the directory entry for the file specified.
 *
 *  FORM OF CALL
 *      UpdateDirectoryEntry(FilePointer)
 * 
 *  INPUTS
 * 	FilePointer:  Pointer to File descriptor 
 *
 *  RETURNS
 * 	STATUS_SUCCESS if operation is successful.
 *      STATUS_FAILURE otherwise
 */
static
DBM_STATUS
UpdateDirectoryEntry (
                      IN FILE *FilePointer
                      )
{
    DISK_INFO *DiskInfo;
    DIRECTORY_ENTRY_B *DirectoryArea;

    DiskInfo = (DISK_INFO *) FilePointer->DiskInfo;
    DirectoryArea =  (DIRECTORY_ENTRY_B *)DiskInfo->IOBuffer;
    /*
     *  Read in the directory sector for the entry we want to update.
     */
    if (ReadLogicalSector((FLOPPY_DRIVE_INFO *)DiskInfo->DeviceData, 
                          DiskInfo->DiskType, 
                          FilePointer->DirectoryInSector, 
                          (UCHAR *)DirectoryArea) == STATUS_FAILURE) {
        errno |= FAT_OP_DIR_UPDATE_FAILED;
        return STATUS_FAILURE;
    }

    /*
     *  Copy the directory entry from the file pointer to the directory
     *  area.
     */
    DirectoryArea[FilePointer->DirectoryIndex] = * (DIRECTORY_ENTRY_B *)
                                                FilePointer->DirectoryEntry;
    /*
     *  Write out the update sector.
     */
    if (WriteLogicalSector((FLOPPY_DRIVE_INFO *)DiskInfo->DeviceData, 
                           DiskInfo->DiskType, 
                           FilePointer->DirectoryInSector, 
                           (UCHAR *)DirectoryArea) == STATUS_FAILURE) {
        errno |= FAT_OP_DIR_UPDATE_FAILED;
        return STATUS_FAILURE;
    }

    return STATUS_SUCCESS;
}



/* ======================================================================
 * =  ReadFatFile - Reads data from file buffer to user's area.         =
 * ======================================================================
 *  OVERVIEW
 *      Copies data from file's buffer to the user's area .  If buffer is 
 *      empty, then data is read from the disk.
 *
 *  FORM OF CALL
 * 	ReadFatFile (FilePointer, Destination, Size)
 * 
 *  INPUTS
 *      FilePointer:  Pointer to file descriptor
 *      Destination:  Area to copy to.
 *      Size:  Number of bytes to copy to Destination.
 *
 *  RETURNS
 * 	The number of bytes read (copied to user's buffer from
 *      file's buffer.  Less than requested size if there was a problem.
 */
static
ui    
ReadFatFile (
             IN FILE *FilePointer,
             IN UCHAR *Destination,
             IN ui    Size
             )
{
    ui    BytesToCopy, BytesRead = 0;

    while ((int)Size > 0) {
        /*
         *  If the file buffer is empty, then read in new data and
         *  reset the index.
         */
        if (FilePointer->BufferIndex == FilePointer->BufferEnd) {
            FilePointer->BufferIndex = 0;
            if ((FilePointer->BufferEnd = ReadFromFile(FilePointer)) == 0) {
                break;
            }
        }

        /*
         *  Number of bytes to copy is the smallest of the requested size
         *  or the available buffer space.
         */
        BytesToCopy = MIN(Size, FilePointer->BufferEnd - FilePointer->BufferIndex);
        memcpy(Destination, &FilePointer->Buffer[FilePointer->BufferIndex], 
               BytesToCopy);
        FilePointer->BufferIndex += BytesToCopy;
        BytesRead += BytesToCopy;
        Destination += BytesToCopy;
        Size -= BytesToCopy;
    } /* while */

    return BytesRead;
}



/* ======================================================================
 * =  ReadFromFile - Reads from a Fat file.                             =
 * ======================================================================
 *  OVERVIEW
 *      Reads the next cluster from the opened file.
 *
 *  FORM OF CALL
 * 	ReadFromFile (FilePointer);
 * 
 *  INPUTS
 *      FilePointer:  Points to the file structure which contains all the
 *                    information we need to read from the disk.
 *
 *  RETURNS
 * 	The number of bytes read.  If end of file, then it returns 0.
 *      If an error is detected, it sets the errno variable and
 *      returns 0. 
 */
static
ui    
ReadFromFile (
             IN FILE * FilePointer
             )
{
    DISK_INFO *DiskInfo;
    ui      NextClusterToRead;
    ui      StartingSector, Sector;
    UCHAR   *DestinationAddress;
    ui      BytesRemaining, BytesRead = 0;
    ui      FileSize;

    DiskInfo = (DISK_INFO *) FilePointer->DiskInfo;

    /*
     *  Find out the next cluster to be read in.
     */
    NextClusterToRead = FilePointer->NextClusterToRead;
    if ((NextClusterToRead >= DiskInfo->LastClusterMarker) ||
        NextClusterToRead < 2) {
        /*
         *  We must have reached the end of the file.
         */
        FilePointer->EndOfFile = TRUE;
        return 0;
    }

    /*
     *  Find the logical sector of that cluster and read in the data.
     */
    StartingSector = ClusterToLogicalSector(DiskInfo, NextClusterToRead);
    DestinationAddress = (UCHAR *) FilePointer->Buffer;
    for (Sector = StartingSector;
         Sector < StartingSector + DiskInfo->BootSector.SectorsPerCluster;
         Sector++) {
        if (ReadLogicalSector((FLOPPY_DRIVE_INFO *)DiskInfo->DeviceData, 
                              DiskInfo->DiskType, 
                              Sector, 
                              DiskInfo->IOBuffer) == STATUS_FAILURE) {
            errno |= FAT_OP_FAILED_READ;
            return 0;
        }
        memcpy(DestinationAddress, DiskInfo->IOBuffer, 
               DiskInfo->BootSector.BytesPerSector);
        DestinationAddress += DiskInfo->BootSector.BytesPerSector;
        BytesRead += DiskInfo->BootSector.BytesPerSector;
    }

    /*
     *  Get the next cluster to read from this file.
     */
    FilePointer->NextClusterToRead = GetNextCluster(DiskInfo, NextClusterToRead);

    if (FilePointer->NextClusterToRead >= DiskInfo->LastClusterMarker) {
        /*
         *  If we have reached the end of the file, then compute how
         *  many bytes from this cluster are part of the file.
         */
        FileSize = BYTES_TO_INTEGER(((DIRECTORY_ENTRY_B *)FilePointer->DirectoryEntry)->FileSize);
        BytesRemaining = FileSize % DiskInfo->BytesPerCluster;
        BytesRead = (BytesRemaining) ? BytesRemaining : BytesRead;
    }

    return BytesRead;
}



/* ======================================================================
 * =  WriteFatFile - Writes data to disk when buffer is full.           =
 * ======================================================================
 *  OVERVIEW
 *      Copies data from user area to file's buffer.  If buffer is full,
 *      then it will write out the buffer to disk.
 *
 *  FORM OF CALL
 * 	WriteFatFile (FilePointer, Source, Size)
 * 
 *  INPUTS
 *      FilePointer:  Pointer to file descriptor
 *      Source:  Area to copy from.
 *      Size:  Number of bytes to copy from Source
 *
 *  RETURNS
 * 	The number of bytes written (copied from user's buffer to
 *      file's buffer.  0 if there was a problem.
 */
static
ui    
WriteFatFile (
              IN FILE *FilePointer,
              IN UCHAR *Source,
              IN ui    Size
              )
{
    ui    BytesToCopy;
    DIRECTORY_ENTRY_B *DirectoryEntry;
    ui    FileSize;
    ui    BytesWritten = 0;
    ui    BytesWrittenToDisk = 0;

    while ((int)Size > 0) {
        /*
         *  Number of bytes to copy is the smallest of the requested size
         *  or the available buffer space.
         */
        BytesToCopy = MIN(Size, FilePointer->BufferSize - FilePointer->BufferIndex);
        memcpy(&FilePointer->Buffer[FilePointer->BufferIndex], Source,
               BytesToCopy);
        FilePointer->BufferIndex += BytesToCopy;
        BytesWritten += BytesToCopy;
        Source += BytesToCopy;
        Size -= BytesToCopy;

        /*
         *  If the file buffer is full, then write out to the device and
         *  reset the index.
         */
        if (FilePointer->BufferIndex == FilePointer->BufferSize) {
            if (WriteToFile(FilePointer) == STATUS_FAILURE) {
                break;
            }
            BytesWrittenToDisk += FilePointer->BufferIndex;
            FilePointer->BufferIndex = 0;
        }
    } /* while */

    if (BytesWrittenToDisk) {
        /*
         *  Set the file size in the directory entry.
         */
        DirectoryEntry = (DIRECTORY_ENTRY_B *)FilePointer->DirectoryEntry;
        FileSize = BYTES_TO_INTEGER(DirectoryEntry->FileSize) + BytesWrittenToDisk;
        INTEGER_TO_BYTES(FileSize, DirectoryEntry->FileSize); 
#if 0
        if ((UpdateDirectoryEntry(FilePointer) == STATUS_FAILURE) ||
            (UpdateFatArea((DISK_INFO *)FilePointer->DiskInfo) == STATUS_FAILURE)) {
            return 0;
        }
#endif
    }
    return BytesWritten;
}



/* ======================================================================
 * =  WriteToFile - Writes data to the disk.                            =
 * ======================================================================
 *  OVERVIEW
 *      Writes 1 cluster of data from memory to disk.
 *
 *  FORM OF CALL
 * 	WriteToFile (FilePointer);
 * 
 *  INPUTS
 *      FilePointer:  Points to the file structure which contains all the
 *                    information we need to write to the disk.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if the command succeeded
 *      STATUS_FAILURE if command failed.
 */
static
ui    
WriteToFile (
             IN FILE * FilePointer
             )
{
    DISK_INFO *DiskInfo;
    DIRECTORY_ENTRY_B *DirectoryEntry;
    ui      FirstCluster, LastCluster, AvailableCluster;
    ui      StartingSector, Sector;
    UCHAR   *SourceAddress;

    DiskInfo = (DISK_INFO *) FilePointer->DiskInfo;
    DirectoryEntry = (DIRECTORY_ENTRY_B *) FilePointer->DirectoryEntry;

    /*
     *  Find a cluster that is not being used.
     */
    if ((AvailableCluster = GetAvailableCluster(DiskInfo)) == 0) {
        PRTRACE1("Can not obtain a free cluster for writing data.\n");
        errno = FAT_OP_DISK_FULL;
        return STATUS_FAILURE;
    }

    /*
     *  Find the logical sector of that cluster and write out the data.
     */
    StartingSector = ClusterToLogicalSector(DiskInfo, AvailableCluster);
    SourceAddress = (UCHAR *) FilePointer->Buffer;
    for (Sector = StartingSector;
         Sector < StartingSector + DiskInfo->BootSector.SectorsPerCluster;
         Sector++) {
        memcpy(DiskInfo->IOBuffer, SourceAddress, 
               DiskInfo->BootSector.BytesPerSector);
        if (WriteLogicalSector((FLOPPY_DRIVE_INFO *)DiskInfo->DeviceData, 
                               DiskInfo->DiskType, 
                               Sector, 
                               DiskInfo->IOBuffer) == STATUS_FAILURE) {
            errno |= FAT_OP_FAILED_WRITE;
            return STATUS_FAILURE;
        }                               
        SourceAddress += DiskInfo->BootSector.BytesPerSector;
    }
    /*
     *  Now that we're done writing out one cluster's worth of data,
     *  mark the cluster that we wrote to as the last cluster of the file
     *  and add it to the chain of clusters that makes up the file.
     */
    ChangeFatEntry(DiskInfo, AvailableCluster, DiskInfo->LastClusterMarker);

    /*
     *  Let's find the last cluster for this file.  We start with the
     *  first cluster which is stored in the directory entry.  If the
     *  file is empty (ie, the first cluster is 0), 
     *  then there's no need to search.  We'll just change the entry to
     *  point to the new cluster.
     */
    FirstCluster = BYTES_TO_INTEGER(DirectoryEntry->FirstCluster);
    if (FirstCluster < 2) {
        INTEGER_TO_BYTES(AvailableCluster, DirectoryEntry->FirstCluster);
        return (STATUS_SUCCESS);
    }
    /*
     *  If the file is not empty, then we'll follow the chain of clusters
     *  until we reach the last one.
     */
    LastCluster = DoToChainOfClusters(DiskInfo, FirstCluster, LAST_CLUSTER, NULL); 

    /*
     *  We can then add the new cluster to the last cluster, thus extending the
     *  chain (file).
     */
    ChangeFatEntry(DiskInfo, LastCluster, AvailableCluster);
    return (STATUS_SUCCESS);
}



/* ======================================================================
 * =  GetAvailableCluster - Returns the next free cluster in the FAT.   =
 * ======================================================================
 *  OVERVIEW
 *      Returns the next free cluster in the FAT.
 *
 *  FORM OF CALL
 * 	GetAvailableCluster (DiskInfo)
 * 
 *  INPUTS
 *      DiskInfo:  Pointer to structure containing disk information.
 *
 *  RETURNS
 * 	The next available cluster or if none is found 0.
 *      We can return 0 because clusters 0 & 1 are reserved!
 */
static
ui   
GetAvailableCluster (
                     IN DISK_INFO * DiskInfo
                     )
{
    ui    FatEntry;
    ui    Data;
static ui GuessEntry = 0;

    /*
     *  As a performance enhancement, check the guess first
     */
    if ((GuessEntry < DiskInfo->MaxEntriesInFat) &&
        !GetNextCluster(DiskInfo, GuessEntry)) {
        return GuessEntry++;
    }
    /* 
     * Go thru the FAT and stop when we find the first available cluster.
     */
    for (FatEntry = 2; FatEntry < DiskInfo->MaxEntriesInFat; FatEntry++) {
        Data = GetNextCluster(DiskInfo, FatEntry);
        if (Data == 0) {
            GuessEntry = FatEntry+1;
            return FatEntry;
        }
    }
    return 0;
}



/* ======================================================================
 * =  UpdateFatArea - Writes out the modified FAT area.                 =
 * ======================================================================
 *  OVERVIEW
 *      Writes out the modified FAT area.
 *
 *  FORM OF CALL
 * 	UpdateFatArea (DiskInfo)
 * 
 *  INPUTS
 *      DiskInfo:  Pointer to structure containing disk information.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if operation is successful.
 *      STATUS_FAILURE otherwise
 */
static
ui   
UpdateFatArea (
               DISK_INFO *DiskInfo
               )
{
    ui    Sector;
    UCHAR *SourceAddress;

    if (DiskInfo->FatChanged) {
        /*
         *  Write out ALL the FAT areas to disk.
         */
        SourceAddress = DiskInfo->Fat;
        for (Sector = DiskInfo->FirstFatSector; Sector < DiskInfo->FirstDirectorySector; 
             Sector++) {
            memcpy(DiskInfo->IOBuffer, SourceAddress, 
                   DiskInfo->BootSector.BytesPerSector);
            if (WriteLogicalSector((FLOPPY_DRIVE_INFO *)DiskInfo->DeviceData, 
                                  DiskInfo->DiskType, 
                                  Sector, 
                                  DiskInfo->IOBuffer) == STATUS_FAILURE) {
                errno |= FAT_OP_FAT_UPDATE_FAILED;
                return STATUS_FAILURE;
            }
            SourceAddress += DiskInfo->BootSector.BytesPerSector;
        } /* for */
        DiskInfo->FatChanged = FALSE;
    } /* if */

    return STATUS_SUCCESS;
}




/* ======================================================================
 * =  CloseFatFile - Implements fclose().                               =
 * ======================================================================
 *  OVERVIEW
 *      Closes an opened file, writing out any left over data.
 *
 *  FORM OF CALL
 * 	CloseFatFile (FilePointer)
 * 
 *  INPUTS
 *      DiskInfo:  Pointer to where disk information can be found.
 *
 *  RETURNS
 * 	EOF if any errors occurred and zero otherwise.
 */
static
int    
CloseFatFile (
          IN FILE *FilePointer
          )
{
    DIRECTORY_ENTRY_B *DirectoryEntry;
    ui    FileSize;
    DBM_STATUS Status = STATUS_SUCCESS;

    if (FilePointer->Flags & WRITING_FILE) {
        /*
         *  Flush out the remainder of the file's buffer.
         */
        if (FilePointer->BufferIndex) {
            Status = WriteToFile(FilePointer);
    
            /*
             *  Set the file size in the directory entry.
             */
            DirectoryEntry = (DIRECTORY_ENTRY_B *)FilePointer->DirectoryEntry;
            FileSize = BYTES_TO_INTEGER(DirectoryEntry->FileSize) + FilePointer->BufferIndex; 
            INTEGER_TO_BYTES(FileSize, DirectoryEntry->FileSize); 
        }

        if ((Status != STATUS_SUCCESS) ||
            (UpdateDirectoryEntry(FilePointer) == STATUS_FAILURE) ||
            (UpdateFatArea((DISK_INFO *)FilePointer->DiskInfo) == STATUS_FAILURE)) {
            Status = STATUS_FAILURE;
        }
    }

    Shutdown(FilePointer->DriveNumber);
    free( FilePointer->DirectoryEntry );
    free( FilePointer->Buffer-8 );  /* the actual buffer is offset by 8 */
    free( FilePointer );

    if (Status != STATUS_SUCCESS)
        return EOF;
    else
        return 0;
}



/* ======================================================================
 * =  GetFatDirectory - Gets the directory of a FAT disk.               =
 * ======================================================================
 *  OVERVIEW
 *      Returns a string containing one entry of the FAT directory
 *      specified in the FILE descriptor.
 *
 *  FORM OF CALL
 * 	GetFatDirectory (FilePointer, FormatString);
 * 
 *  INPUTS
 *      FilePointer:  Points to the file structure which contains all the
 *                    information we need to know about the disk.
 *                    Pass in NULL to obtain the next directory entry.
 *      FormatString: A format string which describes how the string
 *                    to be returned is to be built.  If NULL, then
 *                    we use a default format.
 *
 *  RETURNS
 * 	A string containing the next directory entry in the specified
 *      format.  NULL if there are no more entries or an error occurred.
 */
static
char *
GetFatDirectory (
                 IN FILE * FilePointer,
                 IN UCHAR * FormatString
                 )
{
    static FILE *LastFilePointer = NULL;
    static BOOLEAN LastEntry = FALSE;

    ui    FoundInSector, FoundInEntryNumber;
    BOOLEAN     Restart;
    DIRECTORY_ENTRY_B *DirectoryEntry;

    if (FilePointer == NULL) {
        FilePointer = LastFilePointer;
        Restart = FALSE;
    }
    else {
        LastFilePointer = FilePointer;
        Restart = TRUE;
        LastEntry = FALSE;
    }

    DirectoryEntry = NextDirectoryEntry(FilePointer, Restart,
                                        MatchOnStandardDirectory,
                                        NULL, &FoundInSector,
                                        &FoundInEntryNumber);

    if ((DirectoryEntry == NULL) && (LastEntry == FALSE)) {
        LastEntry = TRUE;
        sprintf(OutputString, "%39d bytes free\n",
                GetFreeSpace(FilePointer->DiskInfo));
        return OutputString;
    }
    return (PrintDirectoryEntry (DirectoryEntry, DIR_DOS_FORMAT));
}



/* ======================================================================
 * =  PrintDirectoryEntry - Prints information from a directory entry.  =
 * ======================================================================
 *  OVERVIEW
 *      Prints information from a directory entry to a string.
 *
 *  FORM OF CALL
 * 	PrintDirectoryEntry (DirectoryEntry, Flags)
 * 
 *  INPUTS
 *      DirectoryEntry:  Pointer to a standard directory entry.
 *      Flags:  Indicates what information from entry should be printed.
 *              DIR_DOS_FORMAT - Simulates DOS directory command format.
 *              DIR_DBM_FORMAT - Debug monitor's own directory format.
 *
 *  RETURNS
 * 	Returns a pointer to the string containing the formated output.
 */
static
char *
PrintDirectoryEntry (
                     IN DIRECTORY_ENTRY_B * DirectoryEntry,
                     IN ui    Flags 
                     )

{

    ui     LastChangeTime, Seconds, Minutes, Hours;
    ui     LastChangeDate, Day, Month, Year;
    ui     FileSize;
    UCHAR  Filename[13];

    if (DirectoryEntry == NULL) {
        return NULL;
    }

    LastChangeTime = BYTES_TO_INTEGER(DirectoryEntry->TimeOfLastChange);
    Seconds = (LastChangeTime & 0x1F) * 2;  /* Extract the seconds.      */
    Minutes = (LastChangeTime >> 5) & 0x3F; /* Extract the minutes.      */
    Hours = (LastChangeTime >> 11) & 0x1F;  /* Extract the hours.        */

    LastChangeDate = BYTES_TO_INTEGER(DirectoryEntry->DateOfLastChange);
    Day = LastChangeDate & 0x1F;        /* Extract the day of the month */
    Month = (LastChangeDate >> 5) & 0xF;   /* Extract the month         */
    Year = ((LastChangeDate >> 9) & 0x7F) + 1980;  /* Extract the year  */

    FileSize = BYTES_TO_INTEGER(DirectoryEntry->FileSize);

    GetFilenameWithExtension(DirectoryEntry->Filename,
                             DirectoryEntry->FileExtension,
                             Filename);

    switch (Flags) {
    case DIR_DOS_FORMAT:
        if (DirectoryEntryIsSubdirectory (DirectoryEntry)) {
            sprintf(OutputString, "%02d/%02d/%d  %02d:%02d%c%17s%7s%s",
                    Month, Day, Year-1900, (Hours>12) ? Hours-12 : Hours,
                    Minutes, (Hours>=12) ? 'p' : 'a', "<DIR>", " ", Filename);
        }
        else {
            sprintf(OutputString, "%02d/%02d/%d  %02d:%02d%c   %20d %s",
                    Month, Day, Year-1900, (Hours>12) ? Hours-12 : Hours,
                    Minutes, (Hours>=12) ? 'p' : 'a', FileSize,
                    Filename);
        }
        break;

    case DIR_DBM_FORMAT:
        sprintf(OutputString, "%-13s   %7d bytes  %02d/%02d/%d %02d:%02d:%02d",
               Filename, FileSize, Month, Day, Year, Hours,
               Minutes, Seconds);
        break;
    }
    return OutputString;
}



/* ======================================================================
 * =  ReadAFatFile - Reads a file into specified destination.           =
 * ======================================================================
 *  OVERVIEW
 *      Reads an entire file into memory.
 *
 *  FORM OF CALL
 * 	ReadAFatFile (FilePointer, Destination);
 * 
 *  INPUTS
 *      FilePointer:  Points to the file structure which contains all the
 *                    information we need to read from the disk.
 *      Destination:  Address where file should be loaded to.
 *
 *  RETURNS
 *      Returns the number of bytes read in or 0 if an error occured.
 */
static
ui    
ReadAFatFile (
              IN FILE * FilePointer,
              UCHAR   *Destination
              )
{
    DIRECTORY_ENTRY_B *DirectoryEntry;
    ui          FirstCluster;

    DirectoryEntry = (DIRECTORY_ENTRY_B *) FilePointer->DirectoryEntry;
    FirstCluster = BYTES_TO_INTEGER(DirectoryEntry->FirstCluster);
    if (DoToChainOfClusters(FilePointer->DiskInfo, 
                            FirstCluster, 
                            LOAD_CLUSTERS, 
                            Destination) != STATUS_SUCCESS) {
        return 0;
    }
    return (BYTES_TO_INTEGER(DirectoryEntry->FileSize));
}



/* ======================================================================
 * =  ReadWriteRawSector - Reads/Writes a specified logical sector.     =
 * ======================================================================
 *  OVERVIEW
 *      Reads/Writes a specified logical sector into memory.
 *
 *  FORM OF CALL
 * 	ReadWriteRawSector (DoWrite, FilePointer, LogicalSector, 
 *                          DestinationAddress);
 * 
 *  INPUTS
 *      DoWrite:  If 1, then Write.  If 0, then Read.
 *      FilePointer:  Points to the file structure which contains all the
 *                    information we need to read from the disk.
 *      LogicalSector: Sector to read in.
 *      Destination:  Address where data should be loaded to.
 *
 *  RETURNS
 *      Returns the number of bytes read/written or 0 if an error occurred.
 */
ui    
ReadWriteRawSector (
                    IN BOOLEAN DoWrite,
                    IN DISK_INFO *DiskInfo,
                    IN ui  LogicalSector,
                    UCHAR   *DestSrcAddress
                    )
{
    if (DoWrite) {
        memcpy(DiskInfo->IOBuffer, DestSrcAddress, 
               DiskInfo->BootSector.BytesPerSector);
        if (WriteLogicalSector((FLOPPY_DRIVE_INFO *)DiskInfo->DeviceData, 
                               DiskInfo->DiskType, 
                               LogicalSector, 
                               DiskInfo->IOBuffer) == STATUS_FAILURE) {
            errno |= FAT_OP_FAILED_WRITE;
            return 0;
        }
    }
    else {
        if (ReadLogicalSector((FLOPPY_DRIVE_INFO *)DiskInfo->DeviceData, 
                              DiskInfo->DiskType, 
                              LogicalSector, 
                              DiskInfo->IOBuffer) == STATUS_FAILURE) {
            errno |= FAT_OP_FAILED_READ;
            return 0;
        }
        memcpy(DestSrcAddress, DiskInfo->IOBuffer, 
               DiskInfo->BootSector.BytesPerSector);
    }
    return DiskInfo->BootSector.BytesPerSector;
}



/* ======================================================================
 * =  ReadWriteSectors - Reads/Writes logical sectors specified         =
 * ======================================================================
 *  OVERVIEW
 *      Reads the range of logical sectors specified into memory, N times.
 *
 *  FORM OF CALL
 * 	ReadWriteSectors (DoWrite, FirstSector, LastSector, 
 *                        DestSrcAddress, Repeat)
 * 
 *  INPUTS
 *      DoWrite:  If 1, then Write.  If 0, then Read.
 *      FirstSector:  FirstSector to be read
 *      LastSector:  Last Sector to be read.
 *      DestSrcAddress:  Address where sectors should be read/written.
 *      Repeat:  Number of times to repeat operation
 *
 *  RETURNS
 * 	STATUS_SUCCESS if operation was successful; 
 *      STATUS_FAILURE, otherwise.
 */
DBM_STATUS
ReadWriteSectors (
                  ui DoWrite,
                  ui FirstSector,
                  ui BytesToTransfer,
                  char *DestSrcAddress,
                  ui Iterations,
                  ui DriveNumber
                  )
{
    DISK_INFO   *DiskInfo;
    ui          Sector, Bytes, TotalBytes;

    DiskInfo = &DiskInfoTable[DriveNumber];

    /*
     *  Map the logical device number to a physical device.  This
     *  routine will call initialize the device and set up the
     *  dispatch table.
     */
    if (MapLogicalToPhysicalDevice(DriveNumber) == STATUS_FAILURE) {
        errno |= FAT_OP_DEVICE_MAP_FAILED;
        return STATUS_FAILURE;
    }

    DiskInfo->DeviceData = (void *)LogicalDrives[DriveNumber].DeviceData;

    /*
     *  Allocate DMA space for the disk's I/O buffer. XXX
     */
    DiskInfo->IOBuffer = (UCHAR *)FLOPPY_IO_BUFFER;

    /*
     *  Determine what type of floppy is present.
     */
    if (DetermineDiskDensity((FLOPPY_DRIVE_INFO *)DiskInfo->DeviceData,
			     &DiskInfo->DiskType) == STATUS_FAILURE) {
      errno |= FAT_OP_CANT_GET_DENSITY;
      return STATUS_FAILURE;
    }
    
    if (DoWrite) {
        printf("Writing %d bytes from 0x%X starting at sector %d.\n",
	       BytesToTransfer, DestSrcAddress, FirstSector);
    }
    else {
        printf("Reading %d bytes to 0x%X starting at sector %d.\n",
	       BytesToTransfer, DestSrcAddress, FirstSector);
    }

    /*
     * This shouldn't be necessary...  XXX
     * ReadWriteRawSector needs this info but it shouldn't
     * take it from the BootSector structure because that
     * assumes that the floppy contains a FAT format.
     * This should work as long as the floppy has been low-level
     * formatted.   The proper solution is to remove IOBuffer from the
     * DISK_INFO structure and put that knowledge into the low-level
     * driver.  The low-level driver already knows the characteristics
     * of a sector.  At that point we can probably do away with
     * ReadWriteRawSector completely.
     */
    DiskInfo->BootSector.BytesPerSector = 512;

    /*
     *  Repeat the operation on the range 
     */
    while (Iterations--) {
        Sector = FirstSector;
	TotalBytes = 0;
        do {
            Bytes = ReadWriteRawSector(DoWrite, DiskInfo, Sector,
				       (ub *)(DestSrcAddress + TotalBytes));
            if (!Bytes) {
                perror("ERROR:  Something happened while reading/writing sector");
            }
	    ++Sector;
	    TotalBytes += Bytes;
        } while (TotalBytes < BytesToTransfer);
    printf("Done... %d (0x%X) bytes transferred\n", TotalBytes, TotalBytes);
    if (Iterations != 0)
      printf("\t %d iterations remaining\n", Iterations);
    }

    SpinDownFloppyDrive((FLOPPY_DRIVE_INFO *)DiskInfo->DeviceData);

    return STATUS_SUCCESS;
}

#endif /* NEEDFLOPPY */
