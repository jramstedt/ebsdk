#ifndef __FATDRV_H_LOADED
#define __FATDRV_H_LOADED
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

/*
 *  $Id: fatdrv.h,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $
 */

/*
 * $Log: fatdrv.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.5  1997/05/02  18:33:25  fdh
 * Added IsBootSectorValid().
 *
 * Revision 1.4  1996/08/20  02:50:07  fdh
 * Changed a typedef name to avoid a conflict.
 *
 * Revision 1.3  1995/11/09  21:38:56  cruz
 * Changed return type of CloseFile.
 *
 * Revision 1.2  1995/10/26  23:34:37  cruz
 * Fixed prototypes.
 *
 * Revision 1.1  1995/10/03  20:26:21  cruz
 * Initial revision
 *
 *
 *  Static prototypes definitions for fatdrv.c
 */

static
char *
SetCurrentDirectory (
                     IN ui DeviceNumber,
                     IN char *DirectoryString
                     );
static
FILE * 
OpenFatFile (
             IN char  FileSpec[],
             IN UCHAR Mode[],
             IN ui    DriveNumber,
             IN VOID *DeviceData
             );
static
DISK_INFO *
GetDiskInfo (
             IN ui    DriveNumber,
             IN VOID *DeviceData
             );
static
DBM_STATUS
DetermineDiskDensity (
                      IN FLOPPY_DRIVE_INFO *DriveInfo,
                      OUT ui    *DiskType
                      );
static
DBM_STATUS
ReadBootSector (
                IN DISK_INFO *DiskInfo
                );
static
VOID
ConvertBootSector (
                   IN BOOT_SECTOR_B *BS,
                   IN BOOT_SECTOR_INFO *ConvertedBS
                   );
static
BOOLEAN
IsBootSectorValid (
                   IN BOOT_SECTOR_INFO *BootSector
                   );
static
ui   
BytesToInteger (
                IN UCHAR ArrayOfBytes[],
                IN ui    NumberOfBytes
                );
static
VOID
IntegerToBytes (
                IN ui    Value,
                IN ui    NumberOfBytes,
                OUT UCHAR ArrayOfBytes[]
                );
static
DBM_STATUS
ReadFatArea (
             IN DISK_INFO * DiskInfo
             );

#ifdef PTRTRACE
static
VOID
PrintFatArea (
             IN DISK_INFO * DiskInfo
             );
#endif

static
ui
GetFreeSpace (
             IN DISK_INFO * DiskInfo
             );
static
ui   
GetNextCluster (
                IN DISK_INFO * DiskInfo,
                IN ui    FatEntryNumber
                );
static
char *
GetPath (
         char FileSpec[],
         char CurrentDirectory[]
         );

static
char *
GetFilename (
             char FileSpec[]
             );

static
DBM_STATUS
FindDirectory (
              IN FILE * FilePointer,
              IN char * Path
              );
static
DIRECTORY_ENTRY_B *
NextDirectoryEntry (
                    IN FILE *FilePointer,
                    IN BOOLEAN Restart,
                    IN BOOLEAN (*MatchFunction)(DIRECTORY_ENTRY_B *, VOID *),
                    IN VOID * Argument,
                    OUT ui    *FoundInSector,
                    OUT ui    *FoundInEntryNumber
                    );

static
ui ClusterToLogicalSector (
                           IN DISK_INFO * DiskInfo,
                           IN ui    Cluster
                           );
static
BOOLEAN
MatchOnFilename (
               IN DIRECTORY_ENTRY_B *DirectoryEntry,
               IN UCHAR Filename[]
               );
static
BOOLEAN
MatchOnDirectory (
                  IN DIRECTORY_ENTRY_B *DirectoryEntry,
                  IN UCHAR Filename[]
                  );
static
BOOLEAN
MatchOnStandardDirectory (
                          IN DIRECTORY_ENTRY_B *DirectoryEntry,
                          IN VOID *NotUsed
                          );
static
BOOLEAN
DirectoryEntryEmpty (
                     IN DIRECTORY_ENTRY_B *DirectoryEntry
                     );
static
BOOLEAN
DirectoryEntryDeleted (
                       IN DIRECTORY_ENTRY_B *DirectoryEntry
                       );

static
BOOLEAN
DirectoryEntryIsSubdirectory (
                              IN DIRECTORY_ENTRY_B *DirectoryEntry
                              );
static
BOOLEAN
DirectoryEntryIsVolumeName (
                            IN DIRECTORY_ENTRY_B *DirectoryEntry
                            );
#if 0
static
BOOLEAN
DirectoryEntryIsHidden (
                        IN DIRECTORY_ENTRY_B *DirectoryEntry
                        );
#endif
static
BOOLEAN
DirectoryEntryIsValid (
                       IN DIRECTORY_ENTRY_B *DirectoryEntry
                       );
static
ui   
GetFilenameWithExtension (
                          IN UCHAR Filename[],
                          IN UCHAR Extension[],
                          OUT UCHAR FilenameWithExtension[]
                          );
static
ui   
DoToChainOfClusters (
                     IN DISK_INFO * DiskInfo,
                     IN ui    StartingCluster,
                     IN ui    Action,
                     IN UCHAR *LoadAddress 
                     );
static
VOID
ChangeFatEntry (
                IN DISK_INFO * DiskInfo,
                IN ui    FatEntryNumber,
                IN ui    Data
                );
static
VOID
FillInFilename (
                IN DIRECTORY_ENTRY_B *DirectoryEntry,
                IN UCHAR Filename[]
                );
static
VOID
FillInDateAndTime (
                   IN DIRECTORY_ENTRY_B *DirectoryEntry
                   );
static
DBM_STATUS
UpdateDirectoryEntry (
                      IN FILE *FilePointer
                      );
static
ui    
ReadFatFile (
             IN FILE *FilePointer,
             IN UCHAR *Destination,
             IN ui    Size
             );

static
ui    
ReadFromFile (
             IN FILE * FilePointer
             );

static
ui    
WriteFatFile (
              IN FILE *FilePointer,
              IN UCHAR *Source,
              IN ui    Size
              );

static
ui    
WriteToFile (
             IN FILE * FilePointer
             );

static
ui   
GetAvailableCluster (
             IN DISK_INFO * DiskInfo
             );

static
ui   
UpdateFatArea (
               DISK_INFO *DiskInfo
               );

static
int    
CloseFatFile (
          IN FILE *FilePointer
          );
static
char *
GetFatDirectory (
                 IN FILE * FilePointer,
                 IN UCHAR * FormatString
                 );
static
char *
PrintDirectoryEntry (
                     IN DIRECTORY_ENTRY_B * DirectoryEntry,
                     IN ui    Flags 
                     );

static
ui    
ReadAFatFile (
              IN FILE * FilePointer,
              UCHAR   *Destination
              );

#endif /* __FATDRV_H_LOADED */


