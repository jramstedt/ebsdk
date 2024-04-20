#ifndef _H_APIFLASH
#define _H_APIFLASH

#define ARRAYSIZE(x)	(sizeof(x)/sizeof(x[0]))
#define BUFFERSIZE		(2*1024*1024)
#define INITIALPADSIZE	(256*1024)

typedef int BOOL;
#define TRUE	((BOOL)1)
#define FALSE	((BOOL)0)

#define CHECKSUMDATABLOCK	(CalcChecksum((char *)(&(headerStruct.checksum)+1), headerStruct.filelength+(sizeof(headerStruct)-sizeof(headerStruct.checksum))))

typedef struct _HEADERSTRUCT {
	unsigned long checksum;
	char filename[64];
	char description[64];
	long romAddress;
	long filelength;
} HEADERSTRUCT;

typedef HEADERSTRUCT *PHEADERSTRUCT;
#define SIZEOFHEADERSTRUCTNOCHECKSUM		(sizeof(HEADERSTRUCT)-sizeof(((PHEADERSTRUCT)0)->checksum))


typedef struct _OPTIONS {
	char *option;
	BOOL powerEnable;
	BOOL *present;
	char **strData;
} OPTIONS;

typedef enum _MAGIC_ADDRESS {
	MA_FILE = -1,
	MA_SCRIPT = -2,
} MAGIC_ADDRESS;

#ifdef _MSC_VER						// Basically not on a pc, under VC++.

#define OUTPUTEXENAME	"ApiUpg.exe"

#else

#define O_BINARY		0x0000
#define OUTPUTEXENAME	"upgflash"

#endif
#endif
