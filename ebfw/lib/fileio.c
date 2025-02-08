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
__attribute__((unused)) static const char *rcsid = "$Id: fileio.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 *   This file implements the file I/O for the debug monitor.
 *
 * $Log: fileio.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.17  1997/05/30  18:58:27  pbell
 * Added new functions fgetc, fputc, fungetc.
 *
 * Revision 1.16  1997/05/23  15:47:01  pbell
 * Removed drive shutdown calls and memory resets now that the
 * driver behaves as it should.
 *
 * Revision 1.15  1997/05/02  18:19:02  fdh
 * Made MapLogicalToPhysicalDevice a global function.
 *
 * Revision 1.14  1996/08/20  02:50:07  fdh
 * Changed a typedef name to avoid a conflict.
 *
 * Revision 1.13  1996/01/18  12:21:06  cruz
 * If DISABLE_ED_DRIVE is defined, then assume that drives are High Density drives.
 *
 * Revision 1.12  1995/12/23  21:09:20  fdh
 * Included nttypes.h.
 *
 * Revision 1.11  1995/12/08  03:21:44  cruz
 * Cosmetic change to error message printing.
 *
 * Revision 1.10  1995/11/22  21:38:34  cruz
 * Removed static initialization of variables.
 *
 * Revision 1.9  1995/11/09  21:37:53  cruz
 * Changed type of errno.
 *
 * Revision 1.8  1995/11/01  16:49:22  cruz
 * Placed compile conditional NEEDFLOPPY around code.
 *
 * Revision 1.7  1995/10/26  23:35:19  cruz
 * Cleaned up code by fixing prototypes and casting.
 *
 * Revision 1.6  1995/10/10  15:08:01  cruz
 * Changed type of "DriveNumber" to int from ui.
 *
 * Revision 1.5  1995/09/27  19:58:15  cruz
 * Added support for a second disk drive.
 *
 * Revision 1.4  1995/09/22  14:51:06  cruz
 * Fixed a bug that wouldn't skip the drive specification.
 *
 * Revision 1.3  1995/09/21  20:56:32  cruz
 * Added code for implementing fcd and fdr which change the
 * current working directory and default drive respectively.
 *
 * Revision 1.2  1995/09/21  16:01:09  cruz
 * Changed the way the code handled an error for an fopen.
 * The new code calls the Shutdown routine for the drive
 * which spins down the disk and resets some variables
 * internal to the fat driver module.
 *
 * Revision 1.1  1995/09/15  19:26:32  cruz
 * Initial revision
 *
 */

#include "system.h"

#ifdef NEEDFLOPPY
#include "lib.h"
#include "nttypes.h"
#include "fat.h"

/* ======================================================================
 * =                        TYPES DEFINITIONS                           =
 * ======================================================================
 */
/*
 *  Error codes for the possible errors that may be reported
 *  by this stage.
 */
enum FILE_OP_ERRORS {
    FILE_OP_START                       = (0 << ERROR1_START_BIT),
    FILE_OP_INVALID_ARGUMENTS           = (1 << ERROR1_START_BIT),
    FILE_OP_MAX_OPENED_FILES            = (2 << ERROR1_START_BIT),
    FILE_OP_INVALID_LOGICAL_DRIVE       = (3 << ERROR1_START_BIT),
    FILE_OP_DEVICE_MAP_FAILED           = (4 << ERROR1_START_BIT)
};

/* ======================================================================
 * =                    GLOBAL VARIABLE DEFINITIONS                     =
 * ======================================================================
 */
DRIVE_DISPATCH_TABLE LogicalDrives[MAXIMUM_LOGICAL_DRIVE_COUNT];
si      errno;

/* ======================================================================
 * =                LOCAL-GLOBAL VARIABLE DEFINITIONS                   =
 * ======================================================================
 */

/*
 *  Corresponding string for the codes defined above.
 */
static char *Error1Strings[] = {
    "No errors",
    "One or more arguments to the function are invalid",
    "The maximum number of opened files exceed",
    "Invalid logical drive specified",
    "Failed to map logical to physical drive"
};

static char *Error2Strings[1];
/*
 *  Keeps count of the total number of opened files.
 */
static ui NumberOfOpenedFiles;

static int DefaultDrive;             /* Default drive when absent.   */


/* ======================================================================
 * =                    EXTERNAL VARIABLE DEFINITIONS                   =
 * ======================================================================
 */
extern char *Error3Strings[];
extern char *Error4Strings[];


/* ======================================================================
 * =                       LOCAL FUNCTION PROTOTYPES                    =
 * ======================================================================
 */
static
int
GetLogicalDriveNumber (
                       IN char Filename[],
                       OUT char *NewFileSpec[]
                       );



/* ======================================================================
 * =  fopen - Opens a file for reading or writing.                      =
 * ======================================================================
 *  OVERVIEW
 *      This function implents the standard C fopen() function.
 *      Note that some modes may not be implemented for some devices.
 *
 *  FORM OF CALL
 * 	fopen(Filename, Mode)
 * 
 *  INPUTS
 * 	Filename:  A file specification to be opened for read/write.
 *      Mode:      Specifies operation to be performed on file.
 *
 *  RETURNS
 * 	A file pointer to opened file or NULL if not successful.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 8/31/95
 */
FILE *
fopen (
       char * Filename,
       char * Mode
       )
{
    int          DriveNumber;
    FILE        *FilePointer;

    /*
     *  Assume no errors.
     */
    errno = NO_ERRORS;
    
    /*
     *  Make sure the inputs are not NULL
     */
    if ((Filename == NULL) || (Mode == NULL)) {
        errno = FILE_OP_INVALID_ARGUMENTS ;
        return NULL;
    }


    if (NumberOfOpenedFiles >= FOPEN_MAX) {
        errno = FILE_OP_MAX_OPENED_FILES;
        return NULL;
    }

    if ((DriveNumber = GetLogicalDriveNumber(Filename, &Filename)) < 0) {
        return NULL;
    }

    /*
     *  Map the logical device number to a physical device.  This
     *  routine will call initialize the device and set up the
     *  dispatch table.
     */
    if (MapLogicalToPhysicalDevice(DriveNumber) == STATUS_FAILURE) {
        errno |= FILE_OP_DEVICE_MAP_FAILED;
        return NULL;
    }

    /*
     *  Call this drive's open file function.
     */
    FilePointer = LogicalDrives[DriveNumber].OpenFile(Filename, Mode, DriveNumber,
                                           (void *)LogicalDrives[DriveNumber].DeviceData);

    if (FilePointer != NULL)
        NumberOfOpenedFiles++;

    return FilePointer;
}       


/* ======================================================================
 * =  fgetc - Reads the next character from the stream
 * ======================================================================
 *  OVERVIEW
 *      This function implents the standard C fgetc() function.
 *
 *  INPUTS
 *      stream:  File pointer to opened file.
 *
 *  RETURNS
 * 	The character read or EOF if at the end of file.
 *
 *  AUTHOR
 *      Peter J. Bell, Digital Equipment Corporation, 5/29/97
 */
int fgetc( FILE * stream )
{
    if( stream->BufferIndex < stream->BufferEnd )
	return( stream->Buffer[stream->BufferIndex++] );
    else
    {
        ub Buffer[2];

	if( LogicalDrives[stream->DriveNumber].ReadData( stream, Buffer, 1 ) )
	    return( Buffer[0] );
	else
	    return( EOF );
    }
}


/* ======================================================================
 * =  fungetc - Puts one character back onto the input stream
 * ======================================================================
 *  OVERVIEW
 *      This function implents the standard C fungetc() function.
 *
 *  INPUTS
 *      stream:  File pointer to opened file.
 *
 *  RETURNS
 * 	The character read or EOF if it could not be returned.
 *
 *  AUTHOR
 *      Peter J. Bell, Digital Equipment Corporation, 5/29/97
 */
int fungetc( int c, FILE * stream )
{
    if( stream->BufferIndex > -4 )
    {
	stream->Buffer[--stream->BufferIndex] = c;
	return( c );
    }
    else
	return( EOF );
}


/* ======================================================================
 * =  fread - Reads N objects from opened file.                         =
 * ======================================================================
 *  OVERVIEW
 *      This function implents the standard C fread() function.
 *
 *  FORM OF CALL
 * 	fread(ptr, size, nobj, stream);
 * 
 *  INPUTS
 *      ptr:    Pointer to area where data will be read to.
 *      size:   Size in bytes of an object.
 *      nobj:   Number of objects to read.
 *      stream:  File pointer to opened file.
 *
 *  RETURNS
 * 	The number of objects read in.  If less than nobj, then an error
 *      may have occurred.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/11/95
 */
size_t
fread (
       void *ptr, 
       size_t size, 
       size_t nobj,
       FILE * stream
       )
{
    size_t TotalSizeToRead;
    size_t BytesRead;

    /*
     *  XXX We should check here that the file was opened for reading.
     */

    TotalSizeToRead = size * nobj;
    /*
     *  Call the drive's ReadData function.
     */
    BytesRead = LogicalDrives[stream->DriveNumber].ReadData(stream,
                                                            (unsigned char *)ptr, 
                                                            (ui)TotalSizeToRead);
    return (BytesRead / size);
}


/* ======================================================================
 * =  fputc - Puts one character onto the output stream
 * ======================================================================
 *  OVERVIEW
 *      This function implents the standard C fputc() function.
 *
 *  INPUTS
 *      stream:  File pointer to opened file.
 *
 *  RETURNS
 * 	The character read or EOF if it could not be returned.
 *
 *  AUTHOR
 *      Peter J. Bell, Digital Equipment Corporation, 5/29/97
 */
int fputc( int c, FILE * stream )
{
    if( stream->BufferIndex < (stream->BufferSize-1) )
    {
	stream->Buffer[stream->BufferIndex++] = c;
	return( c );
    }
    else
    {
        ub Buffer[2];

	Buffer[0] = c;
	Buffer[1] = 0;
	if( LogicalDrives[stream->DriveNumber].WriteData(stream, Buffer, 1 ) != 1 )
	    return( EOF );
	else
	    return( c );
    }
}


/* ======================================================================
 * =  fwrite - Writes N objects to opened file.                         =
 * ======================================================================
 *  OVERVIEW
 *      This function implents the standard C fwrite() function.
 *
 *  FORM OF CALL
 * 	fwrite(ptr, size, nobj, stream);
 * 
 *  INPUTS
 *      ptr:    Pointer to area where data will be read from.
 *      size:   Size in bytes of an object.
 *      nobj:   Number of objects to write.
 *      stream:  File pointer to opened file.
 *
 *  RETURNS
 * 	The number of objects written.  If less than nobj, then an error
 *      may have occurred.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 8/31/95
 */
size_t
fwrite (
        const void *ptr, 
        size_t size, 
        size_t nobj,
        FILE * stream
        )
{
    size_t TotalSizeToWrite;
    size_t BytesWritten;

    /*
     *  XXX We should check here that the file was opened for writing.
     */

    TotalSizeToWrite = size * nobj;
    /*
     *  Call the drive's WriteData function.
     */
    BytesWritten = LogicalDrives[stream->DriveNumber].WriteData(stream,
                                                                (unsigned char *)ptr,    
                                                                (ui)TotalSizeToWrite);
    return (BytesWritten / size);
}


/* ======================================================================
 * =  fclose - Closes an opened file.                                   =
 * ======================================================================
 *  OVERVIEW
 *      This function implents the standard C fclose() function.
 *
 *  FORM OF CALL
 * 	fclose(stream);
 * 
 *  INPUTS
 *      stream:  File pointer to opened file.
 *
 *  RETURNS
 * 	EOF if any errors occurred; zero, otherwise.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 8/31/95
 */
int
fclose (
        const FILE * stream
        )
{
    /*
     *  Decrement the number of opened files.
     */
    NumberOfOpenedFiles--;
    /*
     *  Call the drive's CloseFile function.
     */
    return( LogicalDrives[stream->DriveNumber].CloseFile(stream) );
}



/* ======================================================================
 * =  feof - Returns true if end-of-file has been seen.                 =
 * ======================================================================
 *  OVERVIEW
 *      This function implents the standard C feof() function.
 *
 *  FORM OF CALL
 * 	feof(stream);
 * 
 *  INPUTS
 *      stream:  File pointer to opened file.
 *
 *  RETURNS
 * 	Non-zero if the end of file has been seen.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/11/95
 */
int
feof (
      FILE * stream
      )
{
    return (stream->EndOfFile);
}




/* ======================================================================
 * =  perror - Prints Returns true if end-of-file has been seen.        =
 * ======================================================================
 *  OVERVIEW
 *      This function implents the standard C ferror() function.
 *
 *  FORM OF CALL
 * 	ferror(s);
 * 
 *  INPUTS
 *      s:  User supplied error message.
 *
 *  RETURNS
 * 	None.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/11/95
 */
void
perror (
        const char * s
        )
{
    ui Error1, Error2, Error3, Error4;

    /*
     *  Print the string only if there's an error.
     */
    if (errno != 0) {
        Error1 = (errno >> ERROR1_START_BIT) & ERROR1_MASK;
        Error2 = (errno >> ERROR2_START_BIT) & ERROR2_MASK;
        Error3 = (errno >> ERROR3_START_BIT) & ERROR3_MASK;
        Error4 = (errno >> ERROR4_START_BIT) & ERROR4_MASK;

        printf("%s: ", s);
        if (Error1) {
            printf("L1 - %s\n", Error1Strings[Error1]);
        }
        if (Error2) {
            printf("L2 - %s\n", Error2Strings[Error2]);
        }
        if (Error3) {
            printf("L3 - %s\n", Error3Strings[Error3]);
        }
        if (Error4) {
            printf("L4 - %s\n", Error4Strings[Error4]);
        }
        printf("\n");
    }
}



/* ======================================================================
 * =  fdir - Gets the next directory entry.                             =
 * ======================================================================
 *  OVERVIEW
 *      This function returns a string containing the next directory
 *      entry.
 *
 *  FORM OF CALL
 * 	fdir(stream, FormatString);
 * 
 *  INPUTS
 *      stream:  File pointer to opened file.  If non-null, the first
 *               entry is returned.  If NULL, then the next entry after
 *               the last one is returned.
 *      FormatString:  A user supplied string specifying how the returned
 *                     string should be constructed.
 *                     Note that currently this is not implemented.
 *
 *  RETURNS
 * 	Pointer to character string containing the formatted 
 *      directory entry.  If no more entries are available or an
 *      error was encountered, NULL is returned.  Look at errno to
 *      determine which.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 8/31/95
 */
char * 
fdir (
      const FILE * stream,
      const char * FormatString
      )
{
    static ui DriveNumber = 0;
    if (stream != NULL) {
        DriveNumber = stream->DriveNumber;
    }

    /*
     *  XXX We should check here that the file was opened for reading.
     */

    /*
     *  Call the drive's GetDirectory function.
     */
    return ((char *) LogicalDrives[DriveNumber].GetDirectory(stream, (unsigned char *)FormatString));
}



/* ======================================================================
 * =  fload - Reads in a file.                                          =
 * ======================================================================
 *  OVERVIEW
 *      This function reads in a file from start to end.  It is similar
 *      to fread() but there's no need to specify how many bytes to read.
 *
 *  FORM OF CALL
 * 	fload(stream, Destination);
 * 
 *  INPUTS
 *      stream:  File pointer to opened file.
 *      Destination:  Address where data will be read to.
 *
 *  RETURNS
 * 	The number of bytes read in; zero if there was a problem.  
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/11/95
 */
size_t
fload (
       FILE * stream,
       char * Destination
       )
{
    size_t BytesRead;

    /*
     *  XXX We should check here that the file was opened for reading.
     */


    /*
     *  Call the drive's ReadAFile function.
     */
    BytesRead = LogicalDrives[stream->DriveNumber].ReadAFile(stream,
                                                          (unsigned char *)Destination);
    return (BytesRead);
}



/* ======================================================================
 * =  fcd - Sets the current directory                                  =
 * ======================================================================
 *  OVERVIEW
 *      This function sets the current working directory for the default
 *      or specified drive.
 *
 *  FORM OF CALL
 * 	fcd (DirectoryString)
 * 
 *  INPUTS
 * 	DirectoryString:  String containing the new directory path or
 *                 the new default drive.  If the string is empty, then
 *                 it returns the default path.
 *
 *  RETURNS
 * 	A string containing the current path.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/21/95
 */
char *
fcd (
     char * DirectoryString
     )
{
    int          DriveNumber;

    errno = NO_ERRORS;
    if ((DriveNumber = GetLogicalDriveNumber(DirectoryString, &DirectoryString)) < 0) {
        return NULL;
    }

    MapLogicalToPhysicalDevice(DriveNumber);
    DefaultDrive = DriveNumber;
    /*
     *  Call this drive's change directory function.
     */
    return (LogicalDrives[DriveNumber].SetDirectory(DriveNumber, 
                                                    DirectoryString));
}       



/* ======================================================================
 * =  fdr - Sets the default drive.                                     =
 * ======================================================================
 *  OVERVIEW
 *      This function sets the default drive.
 *
 *  FORM OF CALL
 * 	fcd (DriveString)
 * 
 *  INPUTS
 * 	DriveString:  String containing the new default drive spec.
 *                    In should be in the form "driveletter:", ie "a:"
 *
 *  RETURNS
 * 	A pointer to character containing the letter signifying current drv.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/21/95
 */
char *
fdr (
     char * DriveString
     )
{
   static  char    Drive[3] = "a";
   int          DriveNumber;
   char        *NotUsed;    

   errno = NO_ERRORS;
   if ((DriveNumber = GetLogicalDriveNumber(DriveString, &NotUsed)) < 0) {
       return NULL;
   }   
   
   DefaultDrive = DriveNumber;
   Drive[0] = 'a' + DriveNumber;
   return Drive;
}       



/* ======================================================================
 * =  GetLogicalDriveNumber - Returns the logical drive number.         =
 * ======================================================================
 *  OVERVIEW
 *      Returns the default logical drive number or the logical number
 *      specified in the string
 *
 *  FORM OF CALL
 * 	GetLogicalDriveNumber (Filename)
 * 
 *  INPUTS
 * 	Filename:  String containing the specification from which the
 *                 drive number is to be extracted.  If drive is
 *                 given or the string is empty, then the default drive
 *                 is returned.
 *
 *  OUTPUTS
 *      NewFileSpec:  If the input string contains a drive spec, then
 *                 its pointer will be incremented by two to skip over it.
 *                 This pointer will be returned thru this string.
 *
 *  RETURNS
 * 	The logical drive number or -1 if an error occurred.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 9/21/95
 */
static
int
GetLogicalDriveNumber (
                       IN char Filename[],
                       OUT char *NewFileSpec[]
                       )
{
    int        DriveLetter;
    ui         DriveNumber;
    
    /*
     *  Check to see if the user is specifying a drive
     */
    if ((Filename[0] != '\0') && (Filename[1] == ':')) {
        /*
         *  If there's a drive specification, then convert it to
         *  a logical drive number and make sure it's valid.
         */
        DriveLetter = toupper((int)Filename[0]);
        DriveNumber = DriveLetter - 'A';

        /*
         *  If it's not a valid drive letter (A-Z) or it exceeds the
         *  maximum number of logical drives.
         */
        if ((!isupper(DriveLetter)) || 
            (DriveNumber >= MAXIMUM_LOGICAL_DRIVE_COUNT)) {
            errno = FILE_OP_INVALID_LOGICAL_DRIVE;
            return -1;
        }
        
        /*
         *  Skip over the drive letter and separator.
         */
        Filename += 2;
    } 
    else {
        DriveNumber = DefaultDrive;
    }

    *NewFileSpec = Filename;
    return DriveNumber;
}



/* ======================================================================
 * =  MapLogicalToPhysicalDevice - Maps logical device to a physical one=
 * ======================================================================
 *  OVERVIEW
 *      Map the logical device number to a physical device and initialize
 *      the dispatch table for the device.
 *
 *  FORM OF CALL
 * 	MapLogicalToPhysicalDevice(LogicalDeviceNumber);
 * 
 *  INPUTS
 * 	LogicalDeviceNumber:  Number of logical device (A-Z);
 *
 *  RETURNS
 * 	None.
 *
 *  AUTHOR
 *      Rogelio R. Cruz, Digital Equipment Corporation, 8/31/95
 */
DBM_STATUS
MapLogicalToPhysicalDevice(
                           IN int LogicalDeviceNumber
                           )
{
    ui DriveNumber, DriveType, DoNotUseDMA;

    /*
     *  First determine if this device has already been initialized.  
     *  If it has, then there's no need to do anything.
     */
    if (!LogicalDrives[LogicalDeviceNumber].Initialized) {

        /*
         *  XXX Here we are supposed to map the logical device number into
         *  a physical one.  This can be done by reading the toy ram
         *  where the mapping has been specified by the user.  For
         *  now we'll assume that all logical numbers map physical device
         *  and they are all floppies, FAT and 2.88Mb drives.
         */
        DriveNumber = LogicalDeviceNumber;
#ifdef DISABLE_ED_DRIVE
        DriveType = 1;
#else
        DriveType = 2;
#endif
        DoNotUseDMA = FALSE;

        if (AllocateFloppyDrive(LogicalDeviceNumber,
                                DriveNumber,
                                DriveType,
                                DoNotUseDMA) == STATUS_FAILURE) {
            return STATUS_FAILURE;
        }
        LogicalDrives[LogicalDeviceNumber].Initialized = TRUE;
    }

    return STATUS_SUCCESS;
}

#endif /* NEEDFLOPPY */
