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
 *
 * FACILITY:    
 *
 *      EB6x Software Tools - uload
 * 
 * FUNCTIONAL DESCRIPTION:
 * 
 *      Used by the SROM mini-debugger when loading images via
 *      the serial line.
 * 
 * CALLING ENVIRONMENT: 
 *
 *      user mode
 * 
 * AUTHOR: David A Rusling
 *
 * CREATION-DATE: 22-Mar-1994
 * 
 * MODIFIED BY: 
 * 
 * $Log: uload.c,v $
 * Revision 1.2  1999/03/21 23:35:32  paradis
 * Removed old "xload" sources
 * Added new version of "uload"
 * Added binaries of "uload.exe" and "xload.exe" to NT build
 *
 * Revision 1.1.1.1  1999/01/04 17:29:06  paradis
 * Initial checkin
 *
 * Revision 1.1.1.1  1998/12/29 21:36:24  paradis
 * Initial CVS checkin
 *
 * Revision 1.14  1997/11/04  15:54:21  pbell
 * Fixed bug and added appropriate delays.  tcdrain is now called
 * less frequently due to problems on some platforms.
 *
 * Revision 1.13  1997/08/11  21:01:40  fdh
 * Added 57600 and 115200 speeds.
 *
 * Revision 1.12  1997/08/09  06:21:23  fdh
 * Added 38400 speed.
 *
 * Revision 1.11  1997/08/08  10:54:50  fdh
 * Modified to print transfer status about 4 times
 * per second instead of printing for each quadword
 * transferred.
 *
 * Revision 1.10  1996/05/22  00:06:32  cruz
 * Added tcdrain() call to the writeserial routine to
 * make sure characters are sent out.
 *
 * Revision 1.9  1996/04/30  20:38:32  cruz
 * Got rid of unused variables to make lint happy.
 *
 * Revision 1.8  1996/04/30  01:29:12  cruz
 * Inserted some 1 sec delays after each character in "XB"
 * command.
 *
 * Revision 1.7  1996/04/29  23:51:30  cruz
 * Added code to pad image size to a multiple of 4.
 *
 * Revision 1.6  1996/04/29  22:27:56  cruz
 * Changed the load_file routine which didn't compute the
 * correct image size for file sizes not a multiple of 4.
 *
 * Revision 1.5  1996/04/27  00:08:07  cruz
 * Fixed lots of bugs that broke uload.  Most of them had to
 * do with not waiting for the I/O buffers to have space
 * before writing the output data and some timing problems.
 * I added a timer feature and the ability to do with
 * autobaud and xb command.
 *
 * Revision 1.4  1995/11/14  16:24:20  cruz
 * Clean up code using lint.
 *
 * Revision 1.3  1994/08/06  00:01:00  fdh
 * Updated Copyright header
 *
 * Revision 1.2  1994/03/22  16:36:36  rusling
 * Some small fixups.
 *
 * Revision 1.1  1994/03/22  16:35:29  rusling
 * Initial revision
 *
*/
#ifndef lint
static char *RCSid = "$Id: uload.c,v 1.2 1999/03/21 23:35:32 paradis Exp $";
#endif



#if defined(__osf__) && defined(__alpha)
#include <stdio.h>
#include <strings.h>
#include <sgtty.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#endif

#ifdef __linux__
#include <stdio.h>
#include <strings.h>
#include <termios.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#endif


#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <winuser.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif



#define INPUT_BUFFER_SIZE       256
#define OUTPUT_BUFFER_SIZE      256
#define PROMPT_STRING           "SROM> "

char    InputBuffer [INPUT_BUFFER_SIZE];
char    OutputBuffer [OUTPUT_BUFFER_SIZE];

static void WaitForPrompt(char *Prompt);
static void Help(void);
static void Error(char * ErrorMessage);
static void SendFile(char *Source, int ImageSize);
static void SendUploadCommand(int DestinationAddress, int ImageSize);
static void RemoteTerminalSync(void);
static int ReadSerialPort(void);
static void WriteSerialPortBinary(char *OutputString, int BytesToWrite, int WaitForReply);
static void WriteSerialPortSlow(char *OutputString, int WaitForReply);
static void SetupSerialPort(char * name, int baudrate);
static void CloseSerialPort();
static void VerifyFile(char *Source, int DestinationAddress, int ImageSize);
static int ReadSerialPortFixed(char * buffer, int readsize);

int             image_length;
unsigned char   *image;
int             BaudRate;

#ifdef VERBOSE
int             transferring_file=0;
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#define IMAGE_PAD 8

char    carriage_return [] = {0x0d};
char    checksum_error [] = {"?2A"};

static void load_file (char *name);

#ifdef _WIN32
#define strcasecmp _stricmp
#define sleep(x) Sleep((x)*1000)
#endif

int main(int argc, char *argv[])
{
	char *          ExeFile;
	int             LoadAddress;
	char *          SerialPort;
	int             BootAfter = 0;
	int		VerifyAfter = 0;

	printf("\n        ****  ULOAD V3.0 [%s]  ****\n", __DATE__);
	printf("        ****  SROM Binary Load Utility  ****\n\n");
	fflush(stdout);

	/* Parse Arguments */
	if ((argc < 4) || (argc > 6)) {
	    Help();
	    exit (1);
	}

	ExeFile = argv[1];
	sscanf(argv[2], "%x", &LoadAddress);
	SerialPort = argv[3];
	BaudRate=9600;  /* default */
	if(argc > 4) {
	    if(strcasecmp(argv[4], "xb") == 0) {
		BootAfter = 1;
	    }
	    else if(strcasecmp(argv[4], "fast") == 0) {
		/* For compatibility with xload */
		BaudRate=19200;
	    }
	    else if(strcasecmp(argv[4], "verify") == 0) {
		VerifyAfter = 1;
	    }
	    else {
		sscanf(argv[4], "%d", &BaudRate);
	    }
	}
	if(argc > 5) {
	    if(strcasecmp(argv[5], "xb") == 0) {
		BootAfter = 1;
	    }
	    else if(strcasecmp(argv[5], "verify") == 0) {
		VerifyAfter = 1;
	    }
	    else {
		printf("Invalid fifth argument: %s\n", argv[5]);
		fflush(stdout);
		Help();
		exit(-1);
	    }
	}

	/* load in the file  */
	load_file (ExeFile);

	/* Open and set up the serial port */

	SetupSerialPort(SerialPort, BaudRate);

	printf("\07Connecting to serial port: %s\n", argv[3]);
	fflush(stdout);

	RemoteTerminalSync();

	sscanf (argv [2], "%x", &LoadAddress);
	SendUploadCommand(LoadAddress,image_length);

	SendFile((char *)image, image_length);

	 /* Now wait for the prompt */
	WaitForPrompt(PROMPT_STRING);

	if (VerifyAfter) {
	    printf("Verifying image\n");
	    fflush(stdout);
	    VerifyFile((char *)image, LoadAddress, image_length);
	}
	if (BootAfter) {
	    printf("\n\nSending XB command...\n");
	    fflush(stdout);
	    WriteSerialPortSlow("xb\r", FALSE);
	}
	printf("\n\n");
	fflush(stdout);
	sleep(2);

	CloseSerialPort();
	return 0;
}    /* end of ULOAD   */
      
 /*--------------------------------------------------------*/
static void load_file (char *name)
{                                                   
    int		fd;
    int         pad;
	int		nread;
#ifdef _WIN32
	struct _stat st;
#else
    struct stat	st;
#endif

    printf ("Retrieving image to upload: %s\n", name);
    fflush(stdout);

#ifdef _WIN32
	if(_stat(name, &st) < 0) {
		perror("name");
		exit(1);
	}

	fd = _open(name, _O_BINARY | O_RDONLY);
#else
	fd = open(name, O_RDONLY);
#endif
    if (fd < 0) {
		perror(name);
		printf ("%%ULOAD-E-FILOPN, can't open %s\n", name);
		fflush(stdout);
		exit (1);
    }

#ifndef _WIN32
    if(fstat(fd, &st) < 0) {
		perror("fstat");
		exit(1);
    }
#endif
    image_length = st.st_size;
    pad = IMAGE_PAD - (image_length % IMAGE_PAD);
    if (pad != IMAGE_PAD) {
		image_length += pad;
		printf("%d bytes added for padding\n", pad);
    }
    printf("Size of file 0x%X (%d) bytes\n", image_length, image_length);

    image = malloc(image_length);
    if(image == NULL) {
		perror("malloc");
		exit(1);
    }

#ifdef _WIN32
	nread = _read(fd, image, image_length-pad);
#else
	nread = read(fd, image, image_length-pad);
#endif
    if(nread != (image_length-pad)) {
		perror("image read");
		exit(1);
    }
    fflush(stdout);
#ifdef _WIN32
	_close(fd);
#else
    close(fd);
#endif

}

static void Help(void)
{
    printf("   usage :  uload exe_file load_address serial_port [baud_rate] [xb]\n\n");
    printf("Parameters:\n");
    printf("   exe_file         This is the name of the image file to be loaded.\n");
    printf("                    No extentions are implied.\n");
    printf("   load_address     This is the HEX physical address in the target\n");
    printf("                    memory, where the image will be loaded.\n");
    printf("   serial_port      This is the name of the serial line/port to which\n");
    printf("                    the remote terminal is connected.\n");
    printf("   baud_rate        OPTIONAL.  Supported baud rates:\n");
    printf("                        300, 1200, 2400, 4800, 9600\n");
    printf("                        19200, 38400, 57600, 115200\n");
    printf("                    If not specified, 9600 is the default\n");
    printf("   xb               OPTIONAL.  If \"xb\" is provided, then the XB command\n");
    printf("                    will be executed after loading the image.\n\n");
    printf("Example:\n");
    printf("    uload pc164fsb.cmp 300000 /dev/tty00 9600\n\n");
    fflush(stdout);
}

static void Error(char * ErrorMessage)
{
    perror(ErrorMessage);
    exit (0);
}

static void SendFile(char *Source, int ImageSize)
{
    int TransferSize;
    int TimeLeft;
    int modulus;

#ifdef VERBOSE
    transferring_file = TRUE;
#endif

    modulus = BaudRate / (4 * 10);  /* 4 writes * (8 bits of data  + 1 start + 1 stop) */
    sleep(1);  /* Wait before transmission */
    printf("\n  Bytes   Time\n----------------\n");
    fflush(stdout);

    while (ImageSize) {
	TimeLeft = (ImageSize * 10)/BaudRate;  /* 8 bits of data  + 1 start + 1 stop */
	TransferSize = min (ImageSize, 8);
	WriteSerialPortBinary(Source, TransferSize, FALSE);
	ImageSize -= TransferSize;
	Source += TransferSize;
	if (((ImageSize  & -8ul) % modulus) == 0) {
	  printf("%7d   %02d:%02d\r", ImageSize, TimeLeft/60, TimeLeft % 60 );
	  fflush(stdout);
	}
    }  
    printf("\n");
    fflush(stdout);
    WriteSerialPortBinary("\0\0\0\0\0\0\0\0\r", 9, FALSE);  /* Complete a QW */

#ifdef VERBOSE
    transferring_file = FALSE;
#endif
}

static void VerifyFile(char *Source, int DestinationAddress, int ImageSize)
{
    char	buf[64];
    unsigned int	*iptr;
    unsigned int	ah, al, dh, dl;
    int	offset;
    int			errors = 0;

    /* Verify the bits that were sent down */
    printf("Verifying image...\n");

    /* Send a "BM" (dump memory) command */
    WriteSerialPortSlow("BM\r", FALSE);
    WaitForPrompt("A> ");
    sprintf(OutputBuffer, "%08x\r", DestinationAddress);
    WriteSerialPortSlow(OutputBuffer, FALSE);
    WaitForPrompt("A> ");
    sprintf(OutputBuffer, "%08x\r", DestinationAddress+ImageSize-1);
    WriteSerialPortSlow(OutputBuffer, FALSE);

    /* Chew up the address we just sent plus the trailing CR/LF */
    ReadSerialPortFixed(buf, 10);


    /* Read lines and parse them */
    printf("\n\n");
    iptr = (unsigned int *)Source;
    for(offset = 0; offset < ImageSize; offset += 8) {
	if((offset % 0x100) == 0) {
		printf("Verifying 0x%08x...\r", DestinationAddress+offset);
		fflush(stdout);
	}
	ReadSerialPortFixed(buf, 38);
	if(buf[8] != '.' || buf[17] != ':' || buf[18] != ' '
	  || buf[27] != '.') {
	    printf("\nBad input line: %s\n", buf);
	    return;
	}
	sscanf(buf, "%08x.%08x: %08x.%08x\r\n", &ah, &al, &dh, &dl);
	if(dl != *iptr) {
	    errors++;
	    printf("\nData mismatch, address 0x%08x expect 0x%08x got 0x%08x\n",
		DestinationAddress + offset, *iptr, dl);
	}
	iptr++;
	if(dh != *iptr) {
	    errors++;
	    printf("\nData mismatch, address 0x%08x expect 0x%08x got 0x%08x\n",
		DestinationAddress + offset + 4, *iptr, dh);
	}
	iptr++;
    }

    printf("\n%d errors found\n", errors);
}

    

static void SendUploadCommand(int DestinationAddress, int ImageSize)
{
    printf("\nSending XM command.\nDestination = 0x%X, Size = 0x%X (%d) bytes\n",
	    DestinationAddress, ImageSize, ImageSize);
    fflush(stdout);
    sprintf (OutputBuffer, "XM\r%X\r%X\r", DestinationAddress, ImageSize);
    WriteSerialPortSlow(OutputBuffer, FALSE);
}

static void RemoteTerminalSync(void)
{
  
    printf("Syncing with remote terminal...\n");
    fflush(stdout);
#ifdef VERBOSE
    printf("Writing autobaud character\n");
#endif
    WriteSerialPortSlow("U", FALSE); /* Shift-U for autobaud */
    sleep(1);
#ifdef VERBOSE
    printf("Writing carriage-return\n");
#endif
    WriteSerialPortSlow("\r", TRUE);  /* Print a carriage return */
#ifdef VERBOSE
    printf("Waiting for prompt...\n");
#endif
    WaitForPrompt(PROMPT_STRING);
}

#ifdef OLD
static void WaitForPrompt(char *Prompt)
{
    char PromptArray[256];
    char *ptr;
    int Index;

    sleep(1);
    PromptArray[0]='\0';

    while (TRUE) {
	ReadSerialPort();
	printf("%s", InputBuffer);
	fflush(stdout);
	strncat(PromptArray, InputBuffer, 255);
	ptr = PromptArray;
	while (*ptr != '\0') {
	    if (strncmp(ptr, Prompt, strlen(Prompt)) == 0)  /* if prompt is detected, then return. */
	      return;
	    ptr++;
	}
	if (strlen(PromptArray) == 255) {
	    for (Index = 0; Index < 255;  Index++)
	      PromptArray[Index] = PromptArray[Index+1];
	}
    }
}
#else
static void WaitForPrompt(char *Prompt)
{
    char c;
    int Index;

    /* Wait for first character of prompt */
    while (TRUE) {
	do {
	    ReadSerialPortFixed(&c, 1);
	    printf("%c", c);
	} while(c != *Prompt);

	for(Index = 1; Prompt[Index]; Index++) {
	    ReadSerialPortFixed(&c, 1);
	    printf("%c", c);
	    if(c != Prompt[Index]) break;
	}

	/* If we got all the way to the end of the prompt string
	 * without breaking, it's a match!
	 */
	if(Prompt[Index] == '\0') {
	    return;
	}
    }
}
#endif /* OLD */

#if defined(__osf__) && defined(__alpha)
/* UNIX-style TTY handling */

int FD;                     /* open file descriptor to remote console  */
int odisc;                  /* initial tty line discipline             */
int disc;                   /* initial tty line discipline             */

struct sgttyb arg;          /* current mode of tty                */
struct sgttyb defarg;       /* initial mode of tty                */
struct tchars tchars;       /* current state of tty               */
struct tchars defchars;     /* initial state of tty               */
struct ltchars ltchars;     /* current local characters of tty    */
struct ltchars deflchars;   /* iniital local characters of tty    */

 /*---------------------------------------------------------------
  * Set up the "remote" tty's state
  */
static void SetupSerialPort(char * name, int baudrate)
{
    unsigned bits = LDECCTQ;
    int FlushAll = 0;
    int izero = 0;
    int speed;

    if ((FD = open(name, O_RDWR|O_NDELAY)) < 0) {
	printf("%%ULOAD-E-DEVOPN, Cannot open device %s.  May be in use.\n", 
									name);
	fflush(stdout);
	exit(-1);
    }

    /* Don't look at modem signals. The zero indicates reset to 
     * original setting when tty is closed
     */
    if (ioctl(FD, TIOCNMODEM, &izero) != 0) perror("UNoModem: ");

    /* set "exclusive use" mode. */
    if (ioctl(FD, TIOCEXCL, 0) != 0) perror("UExcl: ");

    /* get the current line settings  */
    if (ioctl(FD, TIOCGETP, (char *)&defarg) < 0)     perror("TiocGetp");
    if (ioctl(FD, TIOCGETC, (char *)&defchars) < 0)   perror("TiocGetc");
    if (ioctl(FD, TIOCGLTC, (char *)&deflchars) < 0)  perror("TiocGltc");
    if (ioctl(FD, TIOCGETD, (char *)&odisc) < 0)      perror("TiocGetd");

    switch(BaudRate) {
	case 300:       speed=B300;     break;
	case 1200:      speed=B1200;    break;
	case 2400:      speed=B2400;    break;
	case 4800:      speed=B4800;    break;
	case 9600:      speed=B9600;    break;
	case 19200:     speed=B19200;   break;
	case 38400:     speed=B38400;   break;
	case 57600:     speed=B57600;   break;
	case 115200:    speed=B115200;  break;
	default:
	    printf("Invalid baud rate specified: %d\n", BaudRate);
	    printf("Valid baud rates are:\n");
	    printf("    300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200\n");
	    exit(-1);
    }

    arg = defarg;
    tchars = defchars;
    ltchars = deflchars;

    arg.sg_ispeed = arg.sg_ospeed = speed;
    arg.sg_flags = RAW;

    if (ioctl(FD, TIOCSETP, (char *)&arg) < 0)  perror("TiocSetp");
    if (ioctl(FD, TIOCLBIS, (char *)&bits) < 0) perror("TiocLbis");

    if (ioctl(FD,TIOCFLUSH, &FlushAll) != 0) Error("DataFlush"); 

    tchars.t_intrc = tchars.t_quitc = tchars.t_startc = tchars.t_stopc = -1;
    ltchars.t_suspc = ltchars.t_dsuspc = ltchars.t_flushc
		 = ltchars.t_lnextc = ltchars.t_werasc = ltchars.t_rprntc = -1;

    if (ioctl(FD, TIOCSETC, &tchars) < 0)        perror("Raw");
    if (ioctl(FD, TIOCSLTC, &ltchars) < 0)       perror("Raw");
}

void CloseSerialPort()
{
    /* Restore original modes */
    if (ioctl(FD, TIOCSETD, (char *)&odisc) < 0)     perror("UnRaw");
    if (ioctl(FD, TIOCSETP, (char *)&defarg) < 0)    perror("UnRaw");
    if (ioctl(FD, TIOCSETC, (char *)&defchars) < 0)  perror("UnRaw");
    if (ioctl(FD, TIOCSLTC, (char *)&deflchars) < 0) perror("UnRaw");
    close(FD);
}

static int ReadSerialPort(void)
{
    int n;
    time_t      StartTime;
    time_t      EndTime;

    InputBuffer[0] = '\0';
    StartTime = time(NULL);

    while (TRUE) {
	n = read(FD, InputBuffer, INPUT_BUFFER_SIZE);   /* Read 1 line from port */
	if (n > 0) break;
	if ((n < 0) && (errno != EAGAIN)) Error("ReadSerialPort");
	EndTime = time(NULL);
	if (difftime(EndTime, StartTime) > 10.0) { 
	  printf("Timed out in ReadSerialPort\n\n");
	  fflush(stdout);
	  exit(0);
      }
    }

    InputBuffer[n] = '\0';          /* Terminate the input line     */
    return n;
}

static int ReadSerialPortFixed(char * buffer, int readsize)
{
    int n;
    int nread;
    time_t      StartTime;
    time_t      EndTime;

    InputBuffer[0] = '\0';
    StartTime = time(NULL);

    nread = 0;
    while (nread < readsize) {
	n = read(FD, buffer+nread, readsize-nread);
	if(n > 0) nread += n;
	if ((n < 0) && (errno != EAGAIN)) Error("ReadSerialPort");
	EndTime = time(NULL);
	if (difftime(EndTime, StartTime) > 10.0) { 
	  printf("Timed out in ReadSerialPortFixed,  %d/%d read\n\n", nread, readsize);
	  fflush(stdout);
	  exit(0);
      }
    }

    buffer[nread] = '\0';          /* Terminate the input line     */
    return nread;
}


static void WriteSerialPortBinary(char *OutputString, int BytesToWrite, int WaitForReply)
{
    int BytesWritten;
    int Index;
    time_t      StartTime;
    time_t      EndTime;



    for (Index = 0; Index < BytesToWrite; Index++) {
	StartTime = time(NULL);
	do
	    BytesWritten = write(FD, &OutputString[Index], 1);  /* Write 1 byte out at a time. */
	while( BytesWritten < 0 && errno == EAGAIN &&
	       difftime(time(NULL), StartTime) < 10.0 );

	if( BytesWritten < 0 ) {
	    if( errno == EAGAIN ) {             /* check for a time out */
		printf("Timed out in WriteSerialPortBinary\n\n");
		fflush(stdout);
		exit(0);
	    }
	    Error("WriteSerialPort Write");     /* all other errors */
	}

	if (WaitForReply) {
	    tcdrain(FD);  /* Wait until all characters have been transmitted */
	    ReadSerialPort();
	    printf("%s", InputBuffer);
	    fflush(stdout);
	}
    }  /* for */
    tcdrain(FD);  /* Wait until all characters have been transmitted */
}

static void WriteSerialPortSlow(char *OutputString, int WaitForReply)
{
    int i, BytesToWrite;
    clock_t     StartClock;

    BytesToWrite = strlen(OutputString);
    for( i=0; i<BytesToWrite; i++ )
    {
	StartClock = clock();
	while ( (int)(clock() - StartClock) < 5000);  /* Wait before sending char */
	WriteSerialPortBinary(OutputString+i, 1, WaitForReply);
    }
}

#else
#ifdef _WIN32

static HANDLE hPort;
void PrintLastError(const char * userstr)
{
    LPVOID lpMsgBuf;

    FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	NULL,
	GetLastError(),
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	(LPTSTR)&lpMsgBuf,
	0,
	NULL);

    printf("%s: %s\n", userstr, lpMsgBuf);
    LocalFree(lpMsgBuf);
}


/* These are the functions needed */
static void SetupSerialPort(char * name, int baudrate)
{
    char tmpstr[64];
    DCB dcb;
    COMMTIMEOUTS cto = {MAXDWORD, MAXDWORD, 10000, 0, 0};

    hPort = CreateFile(name, GENERIC_READ|GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, 0, NULL);
    if(hPort == INVALID_HANDLE_VALUE) {
	PrintLastError("Cannot open serial port");
	exit(-1);
    }

    /* Set comm parameters */
    FillMemory(&dcb, sizeof(dcb), 0);
    dcb.DCBlength = sizeof(dcb);
    sprintf(tmpstr, "%d,n,8,1", baudrate);
    if(!BuildCommDCB(tmpstr, &dcb)) {
	PrintLastError("Cannot set up serial port");
	exit(-1);
    }

    if (!SetCommState(hPort, &dcb)) {
	PrintLastError("SetCommState");
	CloseHandle(hPort);
	exit(-1);
    }

    if (!SetCommTimeouts(hPort, &cto)) {
	PrintLastError("SetCommTimeouts");
	CloseHandle(hPort);
	exit(-1);
    }

}


void CloseSerialPort()
{
    CloseHandle(hPort);
}

static int ReadSerialPort(void)
{
    int n;

    InputBuffer[0] = '\0';

    /* Read one line from the port */

    if(!ReadFile(hPort, InputBuffer, INPUT_BUFFER_SIZE, &n, 0)) {
	PrintLastError("ReadSerialPort");
	exit(-1);
    }

    /* Since we opened the port with a 10-second timeout window, if we
     * return with no characters it's because the read timed out.
     */
    if(n == 0) {
	printf("Timed out in ReadSerialPort\n\n");
        fflush(stdout);
        exit(0);
    }

    InputBuffer[n] = '\0';          /* Terminate the input line     */
    return n;
}

static int ReadSerialPortFixed(char * buffer, int readsize)
{
    int n;
    int nread;
    time_t	StartTime;
    time_t	EndTime;

    InputBuffer[0] = '\0';

    nread = 0;
    StartTime = time(NULL);
    while (nread < readsize) {
	if(!ReadFile(hPort, buffer+nread, readsize-nread, &n, 0)) {
	    PrintLastError("ReadSerialPortFixed");
	    exit(0);
	}
	if(n > 0) nread += n;
	EndTime = time(NULL);
	if (difftime(EndTime, StartTime) > 10.0) { 
	  printf("Timed out in ReadSerialPortFixed,  %d/%d read\n\n", nread, readsize);
	  fflush(stdout);
	  exit(0);
      }
    }

    buffer[nread] = '\0';          /* Terminate the input line     */
    return nread;
}

static void WriteSerialPortBinary(char *OutputString, int BytesToWrite, int WaitForReply)
{
    int BytesWritten;

    if(!WriteFile(hPort, OutputString, BytesToWrite, &BytesWritten, 0)) {
	PrintLastError("WriteSerialPort");
	exit(0);
    }
    if(BytesWritten < BytesToWrite) {
	printf("Short Write\n");
	exit(0);
    }

    if (WaitForReply) {
	ReadSerialPort();
	printf("%s", InputBuffer);
	fflush(stdout);
    }

}

static void WriteSerialPortSlow(char *OutputString, int WaitForReply)
{
    int i, BytesToWrite;

    BytesToWrite = strlen(OutputString);
    for( i=0; i<BytesToWrite; i++ )
    {
	Sleep(5);       /* sleep for 5 milliseconds */
	WriteSerialPortBinary(OutputString+i, 1, WaitForReply);
    }
}

#else
#ifdef __linux__

/* LINUX-style TTY handling */

int FD;                     /* open file descriptor to remote console  */
struct termios oldmode;
struct termios newmode;


void printmode(const char * label, struct termios *m)
{
    int i;

    printf("termios struct: %s\n", label);

    printf("iflags: ");
    if(m->c_iflag & IGNBRK) printf("IGNBRK ");
    if(m->c_iflag & BRKINT) printf("BRKINT ");
    if(m->c_iflag & IGNPAR) printf("IGNPAR ");
    if(m->c_iflag & PARMRK) printf("PARMRK ");
    if(m->c_iflag & INPCK) printf("INPCK ");
    if(m->c_iflag & ISTRIP) printf("ISTRIP ");
    if(m->c_iflag & INLCR) printf("INLCR ");
    if(m->c_iflag & IGNCR) printf("IGNCR ");
    if(m->c_iflag & ICRNL) printf("ICRNL ");
    if(m->c_iflag & IUCLC) printf("IUCLC ");
    if(m->c_iflag & IXON) printf("IXON ");
    if(m->c_iflag & IXANY) printf("IXANY ");
    if(m->c_iflag & IXOFF) printf("IXOFF ");
    if(m->c_iflag & IMAXBEL) printf("IMAXBEL ");
    printf("\noflags: ");

    if(m->c_oflag & OPOST) printf("OPOST ");
    if(m->c_oflag & OLCUC) printf("OLCUC ");
    if(m->c_oflag & ONLCR) printf("ONLCR ");
    if(m->c_oflag & OCRNL) printf("OCRNL ");
    if(m->c_oflag & ONOCR) printf("ONOCR ");
    if(m->c_oflag & ONLRET) printf("ONLRET ");
    if(m->c_oflag & OFILL) printf("OFILL ");
    if(m->c_oflag & OFDEL) printf("OFDEL ");
    if(m->c_oflag & NLDLY) {
	switch(m->c_oflag & NLDLY) {
	    case NL0:   printf("NL0 "); break;
	    case NL1:   printf("NL1 "); break;
	}
    }
    if(m->c_oflag & CRDLY) {
	switch(m->c_oflag & CRDLY) {
	    case CR0:   printf("CR0 "); break;
	    case CR1:   printf("CR1 "); break;
	    case CR2:   printf("CR2 "); break;
	    case CR3:   printf("CR3 "); break;
	}
    }
    if(m->c_oflag & TABDLY) {
	switch(m->c_oflag & TABDLY) {
	    case TAB0:  printf("TAB0 "); break;
	    case TAB1:  printf("TAB1 "); break;
	    case TAB2:  printf("TAB2 "); break;
	    case TAB3:  printf("TAB3 "); break;
	    case XTABS: printf("XTABS "); break;
	}
    }
    if(m->c_oflag & BSDLY) {
	switch(m->c_oflag & BSDLY) {
	    case BS0:   printf("BS0 "); break;
	    case BS1:   printf("BS1 "); break;
	}
    }
    if(m->c_oflag & VTDLY) {
	switch(m->c_oflag & VTDLY) {
	    case VT0:   printf("VT0 "); break;
	    case VT1:   printf("VT1 "); break;
	}
    }
    if(m->c_oflag & FFDLY) {
	switch(m->c_oflag & FFDLY) {
	    case FF0:   printf("FF0 "); break;
	    case FF1:   printf("FF1 "); break;
	}
    }
    printf("\ncflags: ");

    switch(m->c_cflag & CSIZE) {
	case CS5:       printf("CS5 "); break;
	case CS6:       printf("CS6 "); break;
	case CS7:       printf("CS7 "); break;
	case CS8:       printf("CS8 "); break;
    }
    if(m->c_cflag & CSTOPB) printf("CSTOPB ");
    if(m->c_cflag & CREAD) printf("CREAD ");
    if(m->c_cflag & PARENB) printf("PARENB ");
    if(m->c_cflag & PARODD) printf("PARODD ");
    if(m->c_cflag & HUPCL) printf("HUPCL ");
    if(m->c_cflag & CLOCAL) printf("CLOCAL ");
    if(m->c_cflag & CRTSCTS) printf("CRTSCTS ");

    switch(m->c_cflag & CBAUD) {
	case B0:	printf("B0"); 	break;
	case B50:	printf("B50"); 	break;
	case B75:	printf("B75"); 	break;
	case B110:	printf("B110"); 	break;
	case B134:	printf("B134"); 	break;
	case B150:	printf("B150"); 	break;
	case B200:	printf("B200"); 	break;
	case B300:	printf("B300"); 	break;
	case B600:	printf("B600"); 	break;
	case B1200:	printf("B1200"); 	break;
	case B1800:	printf("B1800"); 	break;
	case B2400:	printf("B2400"); 	break;
	case B4800:	printf("B4800"); 	break;
	case B9600:	printf("B9600"); 	break;
	case B19200:	printf("B19200"); 	break;
	case B38400:	printf("B38400"); 	break;
	case B57600:	printf("B57600"); 	break;
	case B115200:	printf("B115200"); 	break;
	case B230400:	printf("B230400"); 	break;
	case B460800:	printf("B460800"); 	break;
    }

    printf("\nlflags: ");
    if(m->c_lflag & ISIG) printf("ISIG ");
    if(m->c_lflag & ICANON) printf("ICANON ");
    if(m->c_lflag & XCASE) printf("XCASE ");
    if(m->c_lflag & ECHO) printf("ECHO ");
    if(m->c_lflag & ECHOE) printf("ECHOE ");
    if(m->c_lflag & ECHOK) printf("ECHOK ");
    if(m->c_lflag & ECHONL) printf("ECHONL ");
    if(m->c_lflag & ECHOCTL) printf("ECHOCTL ");
    if(m->c_lflag & ECHOPRT) printf("ECHOPRT ");
    if(m->c_lflag & ECHOKE) printf("ECHOKE ");
    if(m->c_lflag & FLUSHO) printf("FLUSHO ");
    if(m->c_lflag & NOFLSH) printf("NOFLSH ");
    if(m->c_lflag & TOSTOP) printf("TOSTOP ");
    if(m->c_lflag & PENDIN) printf("PENDIN ");
    if(m->c_lflag & IEXTEN) printf("IEXTEN ");
    printf("\nControl chars: ");
    for(i = 0; i < NCCS; i++) {
	printf("%02x ", m->c_cc[i] & 0xff);
    }
    printf("\n\n");
}



 /*---------------------------------------------------------------
  * Set up the "remote" tty's state
  */
static void SetupSerialPort(char * name, int baudrate)
{
    int FlushAll = 0;
    int izero = 0;
    int speed;

    if ((FD = open(name, O_RDWR|O_NDELAY)) < 0) {
	printf("%%ULOAD-E-DEVOPN, Cannot open device %s.  May be in use.\n", 
									name);
	fflush(stdout);
	exit(-1);
    }


    if(tcgetattr(FD, &oldmode) < 0) {
	perror("tcgetattr");
    }

    tcgetattr(FD, &newmode);

    printmode("Before", &newmode);

    switch(BaudRate) {
	case 300:       speed=B300;     break;
	case 1200:      speed=B1200;    break;
	case 2400:      speed=B2400;    break;
	case 4800:      speed=B4800;    break;
	case 9600:      speed=B9600;    break;
	case 19200:     speed=B19200;   break;
	case 38400:     speed=B38400;   break;
	case 57600:     speed=B57600;   break;
	case 115200:    speed=B115200;  break;
	default:
	    printf("Invalid baud rate specified: %d\n", BaudRate);
	    printf("Valid baud rates are:\n");
	    printf("    300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200\n");
	    exit(-1);
    }

    cfmakeraw(&newmode);

#if 0
    if(cfsetospeed(&newmode, speed) < 0) {
	perror("cfsetospeed");
	exit(-1);
    }

    if(cfsetispeed(&newmode, speed) < 0) {
	perror("cfsetispeed");
	exit(-1);
    }
#endif

    newmode.c_cflag &= ~CBAUD;
    newmode.c_cflag |= speed;

    newmode.c_oflag &= ~ONLCR;

    printmode("After", &newmode);

    if(tcsetattr(FD, TCSANOW, &newmode) < 0) {
	perror("tcsetattr");
	exit(-1);
    }

    if(tcflush(FD, TCIOFLUSH) < 0) {
	perror("tcflush");
	exit(-1);
    }

}

void CloseSerialPort()
{
    /* Restore original modes */
    if (tcsetattr(FD, TCSADRAIN, &oldmode) < 0) perror("CloseSerialPort: tcsetattr");
    close(FD);
}

static int ReadSerialPort(void)
{
    int n;
    time_t      StartTime;
    time_t      EndTime;

    InputBuffer[0] = '\0';
    StartTime = time(NULL);

    while (TRUE) {
	n = read(FD, InputBuffer, INPUT_BUFFER_SIZE);   /* Read 1 line from port */
	if (n > 0) break;
	if ((n < 0) && (errno != EAGAIN)) Error("ReadSerialPort");
	EndTime = time(NULL);
	if (difftime(EndTime, StartTime) > 10.0) { 
	  printf("Timed out in ReadSerialPort\n\n");
	  fflush(stdout);
	  exit(0);
      }
    }

    InputBuffer[n] = '\0';          /* Terminate the input line     */
    return n;
}

static int ReadSerialPortFixed(char * buffer, int readsize)
{
    int n;
    int nread;
    time_t      StartTime;
    time_t      EndTime;

    InputBuffer[0] = '\0';
    StartTime = time(NULL);

    nread = 0;
    while (nread < readsize) {
	n = read(FD, buffer+nread, readsize-nread);
	if(n > 0) nread += n;
	if ((n < 0) && (errno != EAGAIN)) Error("ReadSerialPort");
	EndTime = time(NULL);
	if (difftime(EndTime, StartTime) > 10.0) { 
	  printf("Timed out in ReadSerialPortFixed,  %d/%d read\n\n", nread, readsize);
	  fflush(stdout);
	  exit(0);
      }
    }

    buffer[nread] = '\0';          /* Terminate the input line     */
    return nread;
}

static void WriteSerialPortBinary(char *OutputString, int BytesToWrite, int WaitForReply)
{
    int BytesWritten;
    int Index;
    time_t      StartTime;
    time_t      EndTime;


    for (Index = 0; Index < BytesToWrite; Index++) {
	StartTime = time(NULL);
	do
	    BytesWritten = write(FD, &OutputString[Index], 1);  /* Write 1 byte out at a time. */
	while( BytesWritten < 0 && errno == EAGAIN &&
	       difftime(time(NULL), StartTime) < 10.0 );

	if( BytesWritten < 0 ) {
	    if( errno == EAGAIN ) {             /* check for a time out */
		printf("Timed out in WriteSerialPortBinary\n\n");
		fflush(stdout);
		exit(0);
	    }
	    Error("WriteSerialPort Write");     /* all other errors */
	}

	if (WaitForReply) {
	    tcdrain(FD);  /* Wait until all characters have been transmitted */
	    ReadSerialPort();
	    printf("%s", InputBuffer);
	    fflush(stdout);
	}
#ifdef VERBOSE
	else {
	    int n;
	    tcdrain(FD);
	    do {
		n = read(FD, InputBuffer, INPUT_BUFFER_SIZE);
		if(n > 0) {
		    printf("%s", InputBuffer);
		    if(transferring_file && strstr(InputBuffer, "SROM>")) {
			printf("\n\nHow did we get back to the SROM prompt?!\n\n");
			exit(-1);
		    }
		}
	    } while(n > 0);
	}
#endif
    }  /* for */
    tcdrain(FD);  /* Wait until all characters have been transmitted */
}

static void WriteSerialPortSlow(char *OutputString, int WaitForReply)
{
    int i, BytesToWrite;
    clock_t     StartClock;

    BytesToWrite = strlen(OutputString);
    for( i=0; i<BytesToWrite; i++ )
    {
	StartClock = clock();
	while ( (int)(clock() - StartClock) < 5000);  /* Wait before sending char */
	WriteSerialPortBinary(OutputString+i, 1, WaitForReply);
    }
}

#else

#error You need to provide the tty-support functions for this platform

/* These are the functions needed */
static void SetupSerialPort(char * name, int baudrate)
{
}


void CloseSerialPort()
{
}

static int ReadSerialPort(void)
{
}

static void WriteSerialPortBinary(char *OutputString, 
				 int BytesToWrite, 
				 int WaitForReply)
{
}


static void WriteSerialPortSlow(char *OutputString, int WaitForReply)
{
}
#endif /* __linux__ */
#endif /* _WIN32 */
#endif /* __osf__ */
