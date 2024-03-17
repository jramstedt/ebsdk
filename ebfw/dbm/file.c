
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

#ifndef LINT
static char *rcsid = "$Id: file.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * $Log: file.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.10  1997/06/09  15:42:19  fdh
 * Modified arguments to FileWriteRange() to accept a
 * size in bytes rather than the end address.
 *
 * Revision 1.9  1997/05/02  18:24:08  fdh
 * Moved ReadWriteSectors() to another module.
 * It works with disk sectors not files.
 *
 * Revision 1.8  1996/08/20  02:50:57  fdh
 * Changed a typedef name to avoid a conflict.
 *
 * Revision 1.7  1995/12/23  21:10:24  fdh
 * Included nttypes.h.
 *
 * Revision 1.6  1995/12/08  03:21:08  cruz
 * Cosmetic change to error message printing.
 *
 * Revision 1.5  1995/11/01  16:49:04  cruz
 * Placed compile conditional NEEDFLOPPY around code.
 *
 * Revision 1.4  1995/11/01  16:00:17  cruz
 * Changed FileWriteRange() so that it also writes the last byte of
 * the range specified.
 *
 * Revision 1.3  1995/10/26  22:28:50  cruz
 * Removed unused variable.  Included "fat.h"
 *
 * Revision 1.2  1995/10/10  14:23:15  fdh
 * Removed some unnecessary includes.
 *
 * Revision 1.1  1995/10/04  23:38:15  fdh
 * Initial revision
 *
 */

#include "system.h"

#ifdef NEEDFLOPPY
#include "lib.h"
#include "nttypes.h"
#include "mon.h"
#include "fat.h"

/* ======================================================================
 * =  LoadAFile - Loads a file in its entirety to memory.               =
 * ======================================================================
 *  OVERVIEW
 *      This function will load a file into memory.
 *
 *  FORM OF CALL
 * 	LoadAFile(SourceFile, Destination);
 * 
 *  INPUTS
 *      SourceFile:  Filename of the source file.
 *      Destination:  Address where data should be loaded to.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if operation was successful; 
 *      STATUS_FAILURE, otherwise.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/11/95
 */
DBM_STATUS
LoadAFile (
           char * SourceFile,
           char * Destination
           )
{
    FILE * FilePtr;
    size_t BytesRead;
    DBM_STATUS Status = STATUS_SUCCESS;
    
    /*
     *  First try to open the file for reading.  If not found
     *  or some type of error is encountered about the operation.
     */
    if ((FilePtr = fopen(SourceFile, "r")) == NULL) {
      printf("Can't read \"%s\"\n", SourceFile);
      perror("ERROR");
        return STATUS_FAILURE;
    }

    /*
     *  Print out some info.
     */
    printf("Loading %s at address 0x%X\n", SourceFile, Destination);

    /*
     *  Read the file and check for errors.
     */
    if ((BytesRead = fload(FilePtr, Destination)) == 0) {
        perror("ERROR:  File load failed");
        Status = STATUS_FAILURE;
    }
        
    printf("Done... %d (0x%X) Bytes read.\n", BytesRead, BytesRead);
    if (fclose(FilePtr) == EOF) {
        perror("ERROR:  Unexpected error while closing file");
        Status = STATUS_FAILURE;
    }
    return Status;
}



/* ======================================================================
 * =  FileWriteRange - Writes a range of memory to a file.              =
 * ======================================================================
 *  OVERVIEW
 *      This function will write to a file data from a specified memory 
 *      range.
 *
 *  FORM OF CALL
 * 	FileWriteRange(Filename, StartAddress, Size);
 * 
 *  INPUTS
 *      Filename:  Filename of file to be written.
 *      StartAddress:  First byte of range to be copied.
 *      Size:  Number of bytes to copy.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if operation was successful; 
 *      STATUS_FAILURE, otherwise.
 *
 */
DBM_STATUS
FileWriteRange (
                char *Filename,
                ul StartAddress,
                size_t Size
                )
{
    FILE * FilePointer;
    size_t BytesWritten;
    DBM_STATUS Status = STATUS_SUCCESS;

    /*
     *  First try to open the file for writing.  If some type of
     *  error is encountered, about the operation.
     */
    if ((FilePointer = fopen(Filename, "w")) == NULL) {
        perror("ERROR:  Can't open file for writing");
        return STATUS_FAILURE;
    }

    /*
     *  Print out some info.
     */
    printf("Saving range 0x%X to 0x%X to file  %s\n", StartAddress,
           StartAddress+Size-1, Filename);

    BytesWritten = fwrite((void *) StartAddress, 1, Size, FilePointer);

    /*
     *  Check for errors and report any.
     */
    if (BytesWritten < Size) {
        perror("ERROR:  Couldn't write some or all the data");
        Status = STATUS_FAILURE;
    }
        
    if (fclose(FilePointer) == EOF) {
        perror("ERROR:  Unexpected error while closing file");
        Status = STATUS_FAILURE;
    }
    return Status;
}



/* ======================================================================
 * =  FileDirectory - Prints the files in a specified directory.        =
 * ======================================================================
 *  OVERVIEW
 *      This function print out the files in a directory.
 *
 *  FORM OF CALL
 * 	FileDirectory(Directory);
 * 
 *  INPUTS
 *      Directory:  String specifying directory.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if operation was successful; 
 *      STATUS_FAILURE, otherwise.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/11/95
 */
DBM_STATUS
FileDirectory (
               char *Directory
               )
{
    FILE * FilePointer;
    DBM_STATUS Status = STATUS_SUCCESS;
    char * string;

    /*
     *  First try to open the file for reading.  If not found
     *  or some type of error is encountered about the operation.
     */
    if ((FilePointer = fopen(Directory, "d")) == NULL) {
        perror("ERROR:  Can't open source file for reading");
        return STATUS_FAILURE;
    }

    string = fdir(FilePointer, NULL);
    while (string != NULL) {
        printf ("%s\n", string);
        string = fdir(NULL, NULL);
    }

    /*
     *  If there are any errors, print them.
     */
    if (errno != NO_ERRORS) {
        perror("ERROR:  Something happened while getting a directory entry");
        Status = STATUS_FAILURE;
    }

    if (fclose(FilePointer) == EOF) {
        perror("ERROR:  Unexpected error while closing file");
        Status = STATUS_FAILURE;
    }
    return Status;
}



/* ======================================================================
 * =  FileCopy - Copies data from source to destination file.           =
 * ======================================================================
 *  OVERVIEW
 *      This function will copy the contents of one file to another.
 *
 *  FORM OF CALL
 * 	FileCopy(SourceFile, DestinationFile);
 * 
 *  INPUTS
 *      SourceFile:  Filename of the source file.
 *      DestinationFile:  Filename of the destination file.
 *
 *  RETURNS
 * 	STATUS_SUCCESS if operation was successful; 
 *      STATUS_FAILURE, otherwise.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/11/95
 */
DBM_STATUS
FileCopy (
          char *SourceFile,
          char *DestinationFile
          )
{
    FILE        *SourcePtr, *DestPtr;
    BOOLEAN     NoErrors;
    ub          Data[512];
    ui          ObjectsRead, ObjectsWritten;


    /*
     *  Open the files and check for errors.
     */
    if ((SourcePtr = fopen(SourceFile, "r")) == NULL) {
        perror("ERROR:  Can't open source file for reading");
        return STATUS_FAILURE;
    }

    if ((DestPtr = fopen(DestinationFile, "w")) == NULL) {
        perror("ERROR:  Can't open destination file for writing");
        fclose(SourcePtr);
        return STATUS_FAILURE;
    }
    printf("Copying files...\n");
    /*
     *  Copy from source to destination as long as the end-of-file
     *  for the source has not been reached and an error has not been
     *  encountered.
     */
    NoErrors = TRUE;
    while ( (!feof(SourcePtr)) && NoErrors && (errno == NO_ERRORS)) {
        ObjectsRead = fread(Data, 1, 512, SourcePtr);
        ObjectsWritten = fwrite(Data, 1, ObjectsRead, DestPtr);
        if (ObjectsWritten < ObjectsRead) {
            NoErrors = FALSE;
        }
    } /* while */

    if (errno != NO_ERRORS) {
        perror("ERROR:  Something happened while copying the files");
        fclose(DestPtr);
        fclose(SourcePtr);
        return STATUS_FAILURE;
    }

    fclose(DestPtr);
    fclose(SourcePtr);
    printf("Done...\n");
    return STATUS_SUCCESS;
}

#endif /* NEEDFLOPPY */
