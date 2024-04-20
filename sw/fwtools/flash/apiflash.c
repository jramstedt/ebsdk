#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef _MSC_VER						// Basically on a pc, under VC++.
#include <io.h>
#else
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif
 
#include "apiflash.h"

static struct {						// Do this to ensure _headerStruct and _buffer are contiguous in memory.
	HEADERSTRUCT _headerStruct;
	char _buffer[BUFFERSIZE];
} dataBlock;

#define headerStruct	(dataBlock._headerStruct)
#define buffer			(dataBlock._buffer)

static unsigned long CalcChecksum(
char *dataBuffer,
long length)
/*++

Routine Description:

    Quick and dirty checksum of first the n (Size) bytes of Buffer.

Arguments:

    Buffer - Data to checksum

    Size - Number of bytes to checksum

Return Value:

    The calculated checksum

--*/
{
	unsigned long checksum = 0;
	unsigned long highBit;
	long count = 0;

	for (count = 0; count < length; count++)
	{
		highBit = (checksum & 0x80000000) >> 31;
		checksum = (checksum << 1) + highBit;			// now the low_bit
		checksum += *dataBuffer++;
	}

	return(checksum);
}

static struct stat *filestat(
int handle)
{
	static struct stat _filestat;

	fstat(handle,&_filestat);

	return(&_filestat);
}

static BOOL ResetOutputFile(
char *exeName)
{
	int inHandle, outHandle;
	BOOL result = TRUE;


	outHandle = open (OUTPUTEXENAME, O_WRONLY|O_CREAT|O_TRUNC|O_SYNC, S_IREAD|S_IWRITE|S_IEXEC);
	inHandle = open(exeName,O_RDONLY|O_SYNC);

	if (outHandle == -1)
	{
		printf("Apiflash: Unable to create new upgrade file "OUTPUTEXENAME"\n");
		result = FALSE;
	}
	
	if (inHandle == -1)
	{
		printf("Apiflash: Unable to open %s as input file\n",exeName);
		result = FALSE;
	}

	if (
	    (inHandle != -1) &&
	    (outHandle != -1)
	   )
	{
		write(outHandle,buffer,read(inHandle,buffer,min(INITIALPADSIZE,BUFFERSIZE)));

		// Now pad out.
		memset(buffer,'\0',sizeof(buffer));
		write(outHandle,buffer,INITIALPADSIZE-filestat(outHandle)->st_size);

		result = (INITIALPADSIZE == filestat(outHandle)->st_size);
	}

	if (inHandle != -1)
		close(inHandle);

	if (outHandle != -1)
		close(outHandle);

	printf("Apiflash: Created new upgrade file "OUTPUTEXENAME"\n");

	return(result);
}

static BOOL AddToOutputFile(
char *exeName,
char *addFile,
long FlashAddress,
char *whatitis)
{
	int inHandle, outHandle;
	BOOL result = TRUE;

	if (whatitis == NULL)
	{
		printf("Apiflash: Can't add %s need rom description\n",addFile);
		printf("\t: i.e. ApiFlash -addFileUp10Srm.Rom -address90000 -descriptionSRMConsole\n");
		printf("\t: or ApiFlash -addFileUp10Srm.Rom -address90000 -description\"SRM Console update\"\n");
		return(FALSE);
	}

	outHandle = open (OUTPUTEXENAME, O_WRONLY|O_SYNC|O_APPEND, S_IREAD|S_IWRITE|S_IEXEC);
	inHandle = open (addFile, O_RDONLY|O_SYNC);

	if (outHandle == -1)
		{
		printf("Apiflash: Unable to find upgrade file "OUTPUTEXENAME" try -reset to create file\n");
		return (FALSE);
		}
	
	if (inHandle == -1)
		{
		printf("Apiflash: Unable to open %s as input file\n",addFile);
		return (FALSE);
		}

	// Know input and output files are valid
		// Copy, forcing termination in case of a length overrun
	strncpy(headerStruct.description, whatitis, sizeof(headerStruct.description));
	headerStruct.description[sizeof(headerStruct.description)-1] = '\0';

		// Copy, forcing termination in case of a length overrun
	strncpy(headerStruct.filename, addFile, sizeof(headerStruct.filename));
	headerStruct.filename[sizeof(headerStruct.filename)-1] = '\0';

	headerStruct.romAddress = FlashAddress;
	headerStruct.filelength = filestat(inHandle)->st_size;
		
	if (read(inHandle,buffer,BUFFERSIZE) != headerStruct.filelength)
		{
		printf("Apiflash: Unable to read %s as input file\n",addFile);
		result = FALSE;
		}
	else
		{
		headerStruct.checksum = CHECKSUMDATABLOCK;
		write(outHandle,&headerStruct,sizeof(headerStruct));
		write(outHandle,buffer,headerStruct.filelength);
		}

	close(inHandle);
	close(outHandle);

	if (result)
		printf("Apiflash: Appended %s to upgrade file "OUTPUTEXENAME"\n",addFile);

	return(result);
}

static BOOL ShowInfo(
char *exeName)
{
	int inHandle;
	BOOL result = TRUE;

	inHandle = open (exeName, O_RDONLY|O_SYNC);

	if (inHandle == -1)
	{
		printf("Apiflash: Unable to open %s as input file\n",exeName);
		return(FALSE);
	}

	// Seek to first.
	lseek(inHandle,INITIALPADSIZE,SEEK_SET);

	while((lseek(inHandle,0,SEEK_CUR) < filestat(inHandle)->st_size) && (result == TRUE))
	{
		if (
		    (read(inHandle,&headerStruct,sizeof(headerStruct)) != sizeof(headerStruct)) ||
		    (read(inHandle,buffer,headerStruct.filelength) != headerStruct.filelength) ||
		    (headerStruct.checksum != CHECKSUMDATABLOCK)
		   )
			result = FALSE;

		switch (headerStruct.romAddress)
		{
			case MA_FILE:
				printf("File: %s (%s) length %ld checksum %s\n",
					  headerStruct.filename,
					  headerStruct.description,
					  headerStruct.filelength,
					  (result?"GOOD":"BAD"));
				break;

			case MA_SCRIPT:
				printf("Script: %s length %ld checksum %s\n",
					  headerStruct.description,
					  headerStruct.filelength,
					  (result?"GOOD":"BAD"));
				break;

			default:
				printf("ROM image: %s (%s) length %ld position 0x%lx checksum %s\n",
					  headerStruct.filename,
					  headerStruct.description,
					  headerStruct.filelength,
					  headerStruct.romAddress,
					  (result?"GOOD":"BAD"));
				break;
		}
	}

	close(inHandle);

	return(result);
}

static int WriteBufferOut (unsigned char *my_buff, long buff_len,
		int FileHandle, char *outfilename, char *descr,
		long seekaddress)
{
	int onePercent, percentDone;
	long bytesWritten;
	char	*writePtr;

	if (FileHandle == -1)
		{
		printf("Apiflash: Unable to open %s for output\n", outfilename);
		return FALSE;
		}

	lseek (FileHandle, seekaddress, SEEK_SET);
	if (lseek (FileHandle, 0, SEEK_CUR) != seekaddress)
		{
		printf("Error writing to %s, seek failed\n",  outfilename);
		return FALSE;
		}

	// Ok - lots of up-front verification, now do the file writes
	// and record our progress
	onePercent = max(1, buff_len / 100);

	for (writePtr = my_buff, percentDone = 0, bytesWritten = 0;
			bytesWritten < buff_len; percentDone++)
		{
		onePercent = min (onePercent, buff_len - bytesWritten);

		if (write (FileHandle, writePtr, onePercent) != onePercent)
			{
			printf ("\nError: my_buff == 0x%lx, writePtr == 0x%lx, onePercent == 0x%lx\n",
							(long) my_buff, (long)writePtr, (long) onePercent);
			return FALSE;
			}
		else
			{
			bytesWritten += onePercent;
			writePtr += onePercent;
			printf("\rWriting: %s (%s) %d%%", outfilename, descr,
					min(100,percentDone));
			fflush(stdout);
			}
		}

	if (bytesWritten != buff_len)
		{
		printf("\nError writing to %s, write failed %ld %ld\n",
				outfilename, bytesWritten, buff_len);
		return FALSE;
		}
	else
		printf("\nSuccessful\n");

	return TRUE;
}

static BOOL FlashUpgrade(
char *exeName)
{
	int inHandle, outHandle;
	BOOL result = TRUE;
	char *outFile;
	long writeAddress;

	inHandle = open (exeName, O_RDONLY|O_SYNC);

	if (inHandle == -1)
	{
		printf("Apiflash: Unable to open %s as input file\n",exeName);
		return(FALSE);
	}

	// Seek to first.
	lseek(inHandle,INITIALPADSIZE,SEEK_SET);

	while((lseek(inHandle,0,SEEK_CUR) < filestat(inHandle)->st_size) && (result == TRUE))
	{
		if (
		    (read(inHandle,&headerStruct,sizeof(headerStruct)) != sizeof(headerStruct)) ||
		    (read(inHandle,buffer,headerStruct.filelength) != headerStruct.filelength) ||
		    (headerStruct.checksum != CHECKSUMDATABLOCK)
		   )
			result = FALSE;

		if (result == FALSE)
			{
			printf("Entry: %s (%s) is BAD upgrade aborted\n",
						headerStruct.filename, headerStruct.description);
			continue;
			}

		outFile = headerStruct.filename;
		writeAddress = 0;
		unlink(outFile);
		outHandle = open (outFile, O_RDWR|O_CREAT|O_TRUNC|O_SYNC, S_IREAD|S_IWRITE|S_IEXEC);

		result = WriteBufferOut (buffer, headerStruct.filelength, outHandle,
					outFile, headerStruct.description, writeAddress);

		if (result == FALSE)
			continue;

		close (outHandle);
		sleep (1);

		// Now, perform any post processing necessary (executing scripts, copying
		// of images into flash addresses, etc.)
		switch (headerStruct.romAddress)
			{
			case MA_FILE:
				break;

			case MA_SCRIPT:
				printf ("Executing script (%s)\n", headerStruct.description);
				system(headerStruct.description);
				// Should this be waited on?
				break;

			default:
				{
				// This is a rom image.  Now that we've written the physical file to disk,
				// we'll reopen it, and then write those contents to the flash.
				int	RomHandle, FlashHandle;
				char	*FlashFile;
				char	*MemRomImage;

				RomHandle = open (outFile, O_RDONLY|O_SYNC, S_IREAD|S_IWRITE|S_IEXEC);
#ifdef DEVEL_DEBUG
				FlashFile = "test_flash";
				FlashHandle = open (FlashFile, O_RDWR|O_SYNC, S_IREAD|S_IWRITE|S_IEXEC);
#else
				FlashFile = "/dev/flash";
				FlashHandle = open (FlashFile, O_RDWR|O_SYNC|O_CREAT|O_TRUNC);
#endif
				if (FlashHandle == -1)
					{
					printf ("Cannot open flash file %s: errno == %d\n", FlashFile, errno);
					result = FALSE;
					break;
					}
				writeAddress = headerStruct.romAddress;
				MemRomImage = malloc (headerStruct.filelength);
		    		if (read (RomHandle, MemRomImage, headerStruct.filelength) != headerStruct.filelength)
					{
					printf ("Unable to read %s (%s)\n", headerStruct.filename,
								headerStruct.description);
					result = FALSE;
					break;
					}
				close (RomHandle);
				result = WriteBufferOut (MemRomImage, headerStruct.filelength, FlashHandle,
								FlashFile, headerStruct.description, writeAddress);
				close (FlashHandle);
				}	// Rom image processing
			}	// switch for kind of post-processing

		}	// While unpacking the input file contents

	close(inHandle);
	return(result);
}

int main(
int argc,
char *argv[ ])
{
	static BOOL giveHelp;
	static BOOL reset;
	static BOOL info;
	static BOOL powerEnable;
	static BOOL upgrade;
	static char *image;
	static char *file;
	static char *script;
	static char *address;
	static char *description;
	static OPTIONS options[] = {
			{"-help",			FALSE,	&giveHelp,	NULL},
			{"-help",			TRUE,	&giveHelp,	NULL},
			{"-reset",		TRUE,	&reset,		NULL},
			{"-info",			FALSE,	&info,		NULL},
			{"-upgrade",		FALSE,	&upgrade,		NULL},
			{"-image",		TRUE,	NULL,		&image},
			{"-file",			TRUE,	NULL,		&file},
			{"-script",		TRUE,	NULL,		&script},
			{"-address",		TRUE,	NULL,		&address},
			{"-description",	TRUE,	NULL,		&description}
	};
	static int option, arg;
	static int inHandle;

	// Look at the length, if this is a raw file then we canadd things etc.
	inHandle = open(argv[0],O_RDONLY|O_SYNC);
	powerEnable = (filestat(inHandle)->st_size < INITIALPADSIZE);
	close(inHandle);

	for (arg = 1; arg < argc; arg++)
	{
		for (option = 0; option < ARRAYSIZE(options); option++)
		{
			if (
#ifndef _MSC_VER
			    (options[option].powerEnable == powerEnable) &&
#endif
			    (strstr(argv[arg],options[option].option) == argv[arg])
			   )
			{
				if (options[option].present)
					*options[option].present = TRUE;
				
				if (options[option].strData)
				{
				    char	*strhead, *strtail;
				    long	the_len;

					strhead = argv[arg]+strlen(options[option].option);

					// Strip off leading whitespace
					while (isspace(*strhead))
						strhead++;
					*options[option].strData = strhead;

					// Strip off trailing whitespace.  We have to do this
					// because of the way that our parts system works - it's kept
					// as a bunch of dos files, and some people edit scripts under
					// dos or (inadvertently) may have dos settings on their editor
					// In any case, this is to prevent a failure when processing a
					// trailing argument of <-fileREADME.txt\r\n>
					the_len = strlen (strhead) - 1;
					strtail = strhead + the_len;
					while (isspace(*strtail))
						{
						// Found one - convert it to a null
						strhead[the_len] = 0x0;
						the_len--;
						}
				}

				break;
			}
		}

		if (option == ARRAYSIZE(options))
		{
			giveHelp = 1;
			printf("Apiflash: Unknown option %s\n",argv[arg]);
		}
	}

	if (
	    (giveHelp) ||
	    (argc == 1)
	   )
	{
		if (powerEnable)
			printf("apiflash [-help] [-upgrade] [-reset] [-info] [-imageIMAGENAME] [-addressADDRESS] [-descriptionDESCRIPTION]\n");
		else
			printf("apiflash [-help] [-upgrade] [-info]\n");
		return(0);
	}

	if (reset)
	{
		if (ResetOutputFile(argv[0]) == FALSE)
			return(0);
	}

	if (image)
	{
		long romAddress;

		if (address == NULL)
		{
			printf("Apiflash: Can't add %s need rom hexAddress\n",image);
			printf("\t: i.e. ApiFlash -addFileUp10Srm.Rom -address90000 -descriptionSRMConsole\n");
			printf("\t: or ApiFlash -addFileUp10Srm.Rom -address90000 -description\"SRM Console update\"\n");
			return(FALSE);
		}

		if (sscanf(address,"%lx",&romAddress) != 1)
		{
			printf("Apiflash: Can't add %s need good rom hexAddress\n",image);
			return(FALSE);
		}

		if (AddToOutputFile(argv[0],image,romAddress,description) == FALSE)
			return(0);
	}

	if (file)
	{
		if (AddToOutputFile(argv[0],file,MA_FILE,file) == FALSE)
			return(0);
	}

	if (script)
	{
		if (AddToOutputFile(argv[0],script,MA_SCRIPT,script) == FALSE)
			return(0);
	}

#ifdef _MSC_VER
		argv[0] = OUTPUTEXENAME;
#endif

	if (
	    (upgrade) ||
	    (info)
	   )
	{
		if (ShowInfo(argv[0]) == FALSE)
			return(0);
	}

	if (upgrade)
	{
		if (FlashUpgrade(argv[0]) == FALSE)
			return(0);
	}

	return(1);	
}
