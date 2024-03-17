#ifndef __FAT_H_LOADED
#define __FAT_H_LOADED
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
 *  $Id: fat.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $;
 */

/*
 * $Log: fat.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.7  1997/05/02  18:36:50  fdh
 * Added some structure definitions and the prototype for MapLogicalToPhysicalDevice().
 *
 * Revision 1.6  1996/08/20  02:48:43  fdh
 * Changed a typedef name to avoid a conflict.
 *
 * Revision 1.5  1995/10/03  20:28:03  cruz
 * Added some global function prototypes and removed old
 * definitions.
 *
 * Revision 1.4  1994/08/05  20:13:47  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/06/20  14:18:59  fdh
 * Fixup header file preprocessor #include conditionals.
 *
 * Revision 1.2  1994/06/17  19:34:01  fdh
 * Clean-up...
 *
 * Revision 1.1  1993/06/08  19:56:13  fdh
 * Initial revision
 *
 */

/* ======================================================================
 * =                        TYPES DEFINITIONS                           =
 * ======================================================================
 */

/*
 *  Definition for a DOS boot sector.
 */
typedef struct {
    UCHAR       JumpCode[3];            /* Jump instruction opcodes.    */
    UCHAR       ManufacturerCode[8];    /* Manufacture's identification.*/
    UCHAR       BytesPerSector[2];      /* Number of bytes per sector.  */
    UCHAR       SectorsPerCluster[1];   /* Number of sectors per cluster*/
    UCHAR       ReservedSectorsCount[2]; /* " of reserved sectors includ-*/
                                        /* ing the bootsector.          */
    UCHAR       NumberOfFats[1];        /* Number of FAT areas.         */
    UCHAR       EntriesInRootDirectory[2]; /* " of entries in root dir. */
    UCHAR       SectorCountInVolume[2]; /* Total # of sectors in volume.*/
    UCHAR       MediaDescriptor[1];     /* Describes type of media.     */
    UCHAR       SectorsPerFat[2];       /* Number of sectors per FAT.   */
    UCHAR       SectorsPerTrack[2];     /* Number of sectors per track. */
    UCHAR       ReadWriteHeadCount[2];  /* Number of Read/Write heads.  */
    UCHAR       HiddenSectorCount[4];   /* Number of hidden sectors.    */
    UCHAR       TotalSectInVolume[4];   /* If volume size > 32MB        */
    UCHAR       PhysicalDriveNumber[1]; /* Physical drive number?       */
    UCHAR       Reserved1[1];           /* Reserved.                    */
    UCHAR       ExtendedBootSignature[1]; /* 0x29 if present.           */
    UCHAR       VolumeID[4];            /* Volume ID.                   */
    UCHAR       VolumeLabel[11];        /* Volume lable.                */
    UCHAR       Reserved[8];            /* Reserved.                    */
    UCHAR       BootRoutine[512-0x3E];  /* The rest of the sector.      */
} BOOT_SECTOR_B;

/*
 *  A translated version of a DOS boot sector.
 */
typedef struct {
    UCHAR       ManufacturerCode[9];    /* Manufacture's identification.*/
    ui          BytesPerSector;         /* Number of bytes per sector.  */
    ui          SectorsPerCluster;      /* Number of sectors per cluster*/
    ui          ReservedSectorsCount;   /* " of reserved sectors includ-*/
                                        /* ing the bootsector.          */
    ui          NumberOfFats;           /* Number of FAT areas.         */
    ui          EntriesInRootDirectory; /* " of entries in root dir. */
    ui          SectorCountInVolume;    /* Total # of sectors in volume.*/
    ui          MediaDescriptor;        /* Describes type of media.     */
    ui          SectorsPerFat;          /* Number of sectors per FAT.   */
    ui          SectorsPerTrack;        /* Number of sectors per track. */
    ui          ReadWriteHeadCount;     /* Number of Read/Write heads.  */
    ui          HiddenSectorCount;      /* Number of hidden sectors.    */
    ui          VolumeID;               /* Volume ID.                   */
    UCHAR       VolumeLabel[12];        /* String representing label.   */
} BOOT_SECTOR_INFO;

/*
 *  Definition of a FAT directory entry.
 */
typedef struct {
    UCHAR       Filename[8];            /* Filename padded with spaces  */
    UCHAR       FileExtension[3];       /* Extension padded with spaces.*/
    UCHAR       FileAttribute[1];       /* File Attribute.              */
    UCHAR       ReservedSpace[10];      /* Reserved space.              */
    UCHAR       TimeOfLastChange[2];    /* Time of last change.         */
    UCHAR       DateOfLastChange[2];    /* Date of last change.         */
    UCHAR       FirstCluster[2];        /* First Cluster of file.       */
    UCHAR       FileSize[4];            /* File size.                   */
} DIRECTORY_ENTRY_B;

/*
 *  File attribute bits.
 */
typedef struct {
    unsigned    WriteProtected:1;       /* 1 = File is write protected  */
    unsigned    HiddenFile:1;           /* 1 = File is hidden.          */
    unsigned    SystemFile:1;           /* 1 = File is system file.     */
    unsigned    VolumeName:1;           /* 1 = Entry has volume name.   */
    unsigned    Subdirectory:1;         /* 1 = Entry has subdir. name.  */
    unsigned    Archive:1;              /* 1 = File needs to be archived*/
    unsigned    Reserved:2;             /* Reserved bits.               */
} FILE_ATTRIBUTE;


  /*
 *  Structure use for keeping informatin about a disk together.
 */  
typedef struct {
    BOOT_SECTOR_INFO BootSector;        /* Boot Sector information.     */
    UCHAR       *IOBuffer;              /* Memory location to be used   */
                                        /* for all I/O operations.      */
    UCHAR       *Fat;                   /* Pointer to memory where FAT  */
                                        /* is loaded to.                */
                                        /* directory is loaded to.      */
    ui          FirstFatSector;         /* Sector where FAT area begins */
    ui          FirstDirectorySector;   /* Sector where directory begins*/
    ui          FirstDataSector;        /* Sector where data area begins*/
    ui          MaxClusterCount;        /* Maximum number of clusters.  */
    ui          BitsPerFatEntry;        /* Number of bits used by entry */
    ui          MaxEntriesInFat;        /* Max umber of entries per fat */
    ui          LastClusterMarker;      /* Identifies the last cluster. */
    ui          BytesPerCluster;        /* Number of bytes in a cluster */
    BOOLEAN     FatChanged;             /* If True FAT has been changed.*/
    ui          DriveNumber;            /* Logical Drive for this disk. */
    ui          DiskType;               /* One of ED, HD, or DD.        */
    VOID        *DeviceData;            /* Device specific data.        */
    ui          InUse;                  /* Number of users of this disk.*/
} DISK_INFO;

extern
ui    
ReadWriteRawSector (
                    IN BOOLEAN DoWrite,
                    IN DISK_INFO *DiskInfo,
                    IN ui  LogicalSector,
                    UCHAR   *DestSrcAddress
                    );
extern
DBM_STATUS
ReadWriteSectors (
                  ui DoWrite,
                  ui FirstSector,
                  ui BytesToTransfer,
                  char *DestSrcAddress,
                  ui Iterations,
                  ui DriveNumber
                  );
extern
VOID
Shutdown (
          IN ui DriveNumber
          );

extern
DBM_STATUS
AllocateFloppyDrive (
                     IN ui    LogicalDeviceNumber,
                     IN ui    DriveNumber,
                     IN ui    DriveType,
                     IN BOOLEAN DoNotUseDMA
                     );

extern
DBM_STATUS
MapLogicalToPhysicalDevice(
                           IN int LogicalDeviceNumber
                           );


#endif /* __FAT_H_LOADED */

