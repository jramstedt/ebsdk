#ifndef __FILE_H_LOADED
#define __FILE_H_LOADED
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

/*
 *  $Id: file.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $
 */

/*
 *      This file contains the prototypes and definitions for the functions
 *      that do file i/o in C.
 *      Author:
 *          Rogelio R. Cruz, Digital Equipment Corporation, 9/11/95
 *
 *   $Log: file.h,v $
 *   Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 *   Initial CVS checkin
 *
 * Revision 1.7  1997/02/21  04:15:35  fdh
 * Added a few references.
 *
 * Revision 1.6  1995/11/09  21:50:51  cruz
 * Changed return type of CloseFile to int instead of "ui".
 *
 * Revision 1.5  1995/10/26  23:50:24  cruz
 * Added argument list to functions that makeup the dispatch table.
 *
 * Revision 1.4  1995/09/27  20:45:28  cruz
 * Added interface call "ReadRawSector"
 *
 * Revision 1.3  1995/09/27  19:59:44  cruz
 * Changed the maximum number of drives supported to 2.
 *
 * Revision 1.2  1995/09/21  20:54:31  cruz
 * Added prototypes for new routines fcd and fdr.
 *
 * Revision 1.1  1995/09/15  19:28:59  cruz
 * Initial revision
 *
 *
 */

#include <stddef.h>
#include "errno.h"

/* ======================================================================
 * =                        TYPES DEFINITION                            =
 * ======================================================================
 */
#define FOPEN_MAX               5       /* Max number of opened files.  */
#define MAXIMUM_LOGICAL_DRIVE_COUNT 2   /* Max number of logical drive. */

/*
 *  Definition of bits for the "Flags" field in the file structure.
 *  Bits 0-7 are used for generic flags.
 *  Bits 8-31 are device specific.
 */ 
#define READING_FILE      (1<<0)
#define WRITING_FILE      (1<<1)

/*
 *  Definition for a file structure.
 */
typedef struct {
    void        *DirectoryEntry;        /* Pointer to a directory entry.*/
    void        *DiskInfo;              /* Pointer to a DISK_INFO struct*/
    char        *Buffer;                /* Pointer to file buffer.      */
    ui          BufferSize;             /* Size of file buffer in bytes.*/
    ui          BufferIndex;            /* Pointer to next free location*/
    ui          BufferEnd;              /* End of buffer (read only)    */
    ui          DriveNumber;            /* Drive number associated with.*/
    ui          Flags;                  /* Flags                        */
    ui          DirectoryInSector;      /* Sector for directory entry   */
    ui          DirectoryIndex;         /* Index into directory sector. */
    ui          NextClusterToRead;      /* Next Cluster to read in.     */
    ui          EndOfFile;              /* TRUE if end of file.         */
} FILE;

/*
 *  Definition for the dispatch table of the logical drives.
 */
typedef struct {
    FILE * (*OpenFile)(char  * FileSpec,
                       char * Mode,
                       ui    DriveNumber,
                       void *DeviceData); /* Pointer to open file function */
    char * (*GetDirectory)(const FILE *, 
                           unsigned char *); /* Pointer to get directory fnc.*/
    ui    (*WriteData)(FILE *, unsigned char *,
                       ui);             /* Writes data to device.       */
    ui    (*ReadData)(FILE *, unsigned char *, ui); /* Reads data from device.*/
    int   (*CloseFile)(const FILE *);         /* Closes the file.             */
    ui    (*ReadAFile)(FILE *, unsigned char *); /* Reads a file into memory.*/
    char * (*SetDirectory)(ui, char *); /* Sets the current directory.  */
    void  *DeviceData;                  /* Device specific data.        */
    ui    Initialized;                  /* TRUE if device has been inited*/
    ui    InUse;                        /* Number of files using device */ 
} DRIVE_DISPATCH_TABLE;

#define EOF     (-1)

/* ======================================================================
 * =                      VARIABLE DECLARATIONS                         =
 * ======================================================================
 */
extern DRIVE_DISPATCH_TABLE LogicalDrives[MAXIMUM_LOGICAL_DRIVE_COUNT];

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

/* ======================================================================
 * =                      FUNCTION PROTOTYPES                           =
 * ======================================================================
 */
extern
FILE *
fopen (
       char * Filename,
       char * Mode
       );

extern
size_t
fread (
       void *ptr, 
       size_t size, 
       size_t nobj,
       FILE * stream
       );

extern
size_t
fwrite (
        const void *ptr, 
        size_t size, 
        size_t nobj,
        FILE * stream
        );

extern
int
fclose (
        const FILE * stream
        );

extern
int
feof (
      FILE * stream
      );

extern
void
perror (
        const char * s
        );

extern
char * 
fdir (
      const FILE * stream,
      const char * FormatString
      );

extern
size_t
fload (
       FILE * stream,
       char * Destination
       );

extern
char *
fcd (
     char * DirectoryString
     );

extern
char *
fdr (
     char * DriveString
     );

#endif /* __FILE_H_LOADED */


