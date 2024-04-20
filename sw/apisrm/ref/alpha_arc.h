/*
 * Copyright (C) 1993 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Alpha/AXP ARC Console define file.  All the information in this 
 *              file was found in the Advanced Risc Computing (ARC) Standard 
 *		Specification Revision 1.1, May 8, 1992.
 *
 *             
 * Authors:	Carl Furbeck and Judy Gold  	23-Feb-1993                 
 *
 * Modifications:
 *
 *	jrk 	6-Oct-1994    	align ARC_DISPLAY_STATUS structure.
 *
 *	phs	02-Jul-1993	Conditionalized typedef of LARGE_INTEGER with 
 *				#if REALLY_INT64, to allow arithmetic.  See
 *				ntfsboot.c for an example.
 *
 *	cbf 	8-Mar-1993    	removed prototypes, added ifndef and entered 
 *				in cms
 *
 *	jeg	10-Mar-1993     added status codes, path mnemonics, config tree
 *				defs
 *
 *	jeg	06-May-1993	modified according to V1.2 (NT source arc.h)
 *
 */

#ifndef arcdef
#define arcdef 1

#define CCP CONFIGURATION_COMPONENT

#define NT_LINK_ADDRESS (512 * 1024)

#define MAX_MAPPED_MEMORY (32 * 1024 * 1024)		/* 32MB */

#define PROG1_BASE (4 * 1024 * 1024)			/* 4MB */
#define PROG2_BASE (8 * 1024 * 1024)			/* 8MB */
#define SCRATCH_BASE (16 * 1024 * 1024)			/* 16MB */
#define ARCZ_BASE  (24 * 1024 * 1024) 			/* 24MB */

#define PROG_SIZE (SCRATCH_BASE - PROG1_BASE)		/* 12MB */
#define SCRATCH_SIZE (4 * 1024 * 1024)			/* 4MB */
#define ARCZ_SIZE (MAX_MAPPED_MEMORY - ARCZ_BASE)	/* 8MB */

#ifndef _DATA_TYPES_H_
/* define ARC sizes */
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
typedef CHAR BOOLEAN;
#endif

typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;

typedef char* PCHAR;
typedef short* PSHORT;
typedef long* PLONG;

typedef unsigned char* PUCHAR;
typedef unsigned short* PUSHORT;
typedef unsigned long* PULONG;

typedef unsigned short WCHAR;
typedef unsigned short* PWCHAR;

typedef void *PVOID;
typedef ULONG ARC_STATUS;

/* from oli2msft.h */
typedef ULONG  BOOLEAN_ULONG;
typedef BOOLEAN_ULONG  *PBOOLEAN;
#define MAX_MNEMONIC_LEN        20             /* max name length (with '\0')*/
#define MAX_DEVICE_PATH_LEN     63             /* ending '\0' excluded  */


typedef unsigned __int64 UQUAD;

/* same as a QUEUE, but QUEUE has no tag */
typedef struct _LIST_ENTRY {
		struct _LIST_ENTRY *flink;
		struct _LIST_ENTRY *blink;
	       } LIST_ENTRY, *PLIST_ENTRY;

/* misc. Microsoft defines */
#define IN
#define OUT
#define VOID void
#define OPTIONAL
#define _ALPHA_

/* current ARC spec version and revision */
#define ARC_VERSION	1
#define ARC_REVISION	2
#define ADAPTER_COUNT	1

/* ARC Console */
#define ARCPROMPT ">"
#define ARC_CONSOLE_INPUT 0
#define ARC_CONSOLE_OUTPUT 1

/* pg 96 + 97 */
#define StandardIn		0
#define StandardOut		1
#define ArcReadOnlyFile 	1
#define ArcHiddenFile		2
#define ArcSystemFile		4
#define ArcArchiveFile		8
#define ArcDirectoryFile 	16
#define ArcDeleteFile		32

/* status codes - p. 85 */

/* status codes */
#define ESUCCESS	0	/* no error */
#define E2BIG		1	/* argument list too long */
#define EACCES		2	/* permission denied */
#define EAGAIN		3	/* resource temporarily unavailable */
#define	EBADF		4	/* bad file descriptor */
#define EBUSY		5	/* resource busy */
#define EFAULT		6	/* bad address */
#define EINVAL		7	/* invalide argument */
#define EIO		8	/* input/output error */
#define EISDIR		9	/* is a directory */
#define EMFILE		10	/* too many open files */
#define EMLINK		11	/* too many links */
#define ENAMETOOLONG	12	/* filename too long */
#define ENODEV		13	/* no such device */
#define ENOENT		14	/* no such file or directory */
#define ENOEXEC		15	/* execute format error */
#define ENOMEM		16	/* not enough space */
#define ENOSPC		17	/* no space left on device */
#define ENOTDIR		18	/* not a directory */
#define ENOTTY		19	/* inappropriate I/O control operation */
#define ENXIO		20	/* media not loaded */
#define	EROFS		21	/* read-only file system */

/* maximum size of environment variable */
#define MAXIMUM_ENVIRONMENT_VALUE 256


/* common console specific defines */


/* for config tree callbacks */

#if 0
#define DEVICE_INDX 0
#define ID_INDX 1
#define KEY_ARGS 3
#endif

struct ADAPTER {
	int	type;
	int	vector_length;
	int	vector [];
	};


/* these are used by the System Parameter Block */


/* debug block */
typedef struct _DEBUG_BLOCK {
	ULONG Signature;
	ULONG Length;
 } DEBUG_BLOCK, *PDEBUG_BLOCK;

/* restart block and boot status */
typedef struct _BOOT_STATUS {
	ULONG	BootStarted : 1;
	ULONG	BootFinished : 1;
	ULONG	RestartStarted : 1;
	ULONG   RestartFinished : 1;
	ULONG	PowerFailStarted : 1;
	ULONG	PowerFailFinished : 1;
	ULONG   ProcessorReady : 1;
	ULONG	ProcessorRunning : 1;
	ULONG   ProcessorStart : 1;
	ULONG	unused : 32-9;
 } BOOT_STATUS, *PBOOT_STATUS;

typedef struct _I386_RESTART_STATE {

#if defined(_X86_)

    //
    // Put state structure here.
    //

    ULONG PlaceHolder;

#else

    ULONG PlaceHolder;

#endif

} I386_RESTART_STATE, *PI386_RESTART_STATE;


typedef struct _ALPHA_RESTART_STATE {
    ULONG PlaceHolder;
} ALPHA_RESTART_STATE, *PALPHA_RESTART_STATE;

typedef struct _RESTART_BLOCK {
    ULONG Signature;
    ULONG Length;
    USHORT Version;
    USHORT Revision;
    struct _RESTART_BLOCK *NextRestartBlock;
    PVOID  RestartAddress;
    ULONG BootMasterId;
    ULONG ProcessorId;
    volatile BOOT_STATUS BootStatus;
    ULONG CheckSum;
    ULONG SaveAreaLength;
    ALPHA_RESTART_STATE Alpha;
} RESTART_BLOCK, *PRESTART_BLOCK;

/* TestUnicodeCharacter and GetDisplayStatus defines */

typedef struct _ARC_DISPLAY_STATUS {
	USHORT	CursorXPosition;
	USHORT  CursorYPosition;
	USHORT  CursorMaxXPosition;
	USHORT  CursorMaxYPosition;
	UCHAR	ForegroundColor;
	UCHAR	BackgroundColor;
	BOOLEAN	HighIntensity;
	BOOLEAN	UnderScored;
	BOOLEAN	ReverseVideo;
	UCHAR 	Resvd[3];
	} ARC_DISPLAY_STATUS, *PARC_DISPLAY_STATUS;


/*
** System Parameter Block Structure - pg 38
*/
struct SPB  {
	ULONG Signature;  			/* must be 0x53435241 (ARCS) */
	ULONG Length;     			/* length of SPB in bytes    */
        USHORT Version;                    	/* ARC spec version          */
        USHORT Revision;                     	/* ARC spec revision         */
        PRESTART_BLOCK RestartBlock;      	/* pointer to restart blk    */
	PDEBUG_BLOCK DebugBlock;      		/* Pointer to Debug blk      */
	PVOID GenerateExceptionVector;          /* General Exceptions        */
	PVOID TlbMissExceptionVector;           /* UTLBM exceptions          */
	ULONG FirmwareVectorLength;          	/* Length of fvs (bytes)     */
	PVOID *FirmwareVector;             	/* Pointer to FVS Block      */
	ULONG  VendorVectorLength;           	/* Length of vvs (bytes)     */
	PVOID  *VendorVector;     		/* Pointer to VVS      	     */
	ULONG AdapterCount;                  	/* Number of adapters        */
    	ULONG Adapter0Type;    			/* EISA adapter 	     */
    	ULONG Adapter0Length;                   /* length of EISA vector     */
    	PVOID *Adapter0Vector;                  /* where it's located 	     */
	};

/*
** Firmware Vector Structure - points to routines in console
** see page 40 
*/
struct FVS {                             	/* see pg86-88, 91-94       */
	ARC_STATUS (* Load)();                 	/* ptr to load program      */
	ARC_STATUS (* Invoke)();               	/* ptr to invoke program    */
	ARC_STATUS (* Execute)();              	/* ptr to execute program   */
	VOID (* Halt)();                 	/* ptr to halt program      */
	VOID (* PowerDown)();            	/* ptr to powerdown program */
	VOID (* Restart)();              	/* ptr to restart program   */
	VOID (* Reboot)();               	/* ptr to reboot program    */
	VOID (* EnterInteractiveMode)(); 	/* ptr to interactive progr */
	LONG    Reserved1;               	/* reserved                 */
	struct COMPONENT *(* GetPeer)();  	/* get peer from config     */
	struct COMPONENT *(* GetChild)(); 	/* get child from config    */
	struct COMPONENT *(* GetParent)();	/* get parent from config   */
	ARC_STATUS (* GetConfigurationData)(); 	/* get configuration        */
	struct COMPONENT (* AddChild)(); 	/* add a child to config    */
	ARC_STATUS (* DeleteComponent)();      	/* delete a component data  */
	struct COMPONENT *(* GetComponent)();   /* get component data       */
	ARC_STATUS (* SaveConfiguration)();    	/* save config data         */
	struct SYSTEMID *(* GetSystemId)();     /* ptr to system id         */
	struct MEMORYDESCRIPTOR *(* GetMemoryDescriptor)();/* ptr to mem des*/
	LONG  Reserved2;                 	/* reserved                 */
	struct TIME_FIELD *(* GetTime)();   	/* yr,mon,d,h,m,s, millisec */
	ULONG (* GetRelativeTime)();      	/* in seconds for timing    */
	ARC_STATUS (* GetDirectoryEntry)();    	/* ptr to read dir entries  */
	ARC_STATUS (* Open)();                 	/* ptr to open file routine */
	ARC_STATUS (* Close)();                	/* ptr to close file routine*/
	ARC_STATUS (* Read)();                 	/* ptr to read file routine */
	ARC_STATUS (* GetReadStatus)();        	/* ptr to get read status   */
	ARC_STATUS (* Write)();                	/* ptr to write file routine*/
	ARC_STATUS (* Seek)();		 	/* ptr to seek routine      */
	ARC_STATUS (* Mount)();                	/* ptr to mount routine     */
	PCHAR (* GetEnvironmentVariable)(); 	/* get en_var from non-vol  */
	ARC_STATUS (* SetEnvironmentVariable)();/* set en_var in non-vol    */
 	ARC_STATUS (* GetFileInformation)();   	/* pg 104 file attributes   */ 
 	ARC_STATUS (* SetFileInformation)();   	/* modify file attributes   */
        /* WARNING: FlushAllCaches is not explicitly defined */
	VOID (* FlushAllCaches)();		/* flush caches?? 	    */
	ARC_STATUS (* TestUnicodeCharacter)();	/* isa character unicode    */
	PARC_DISPLAY_STATUS(* GetDisplayStatus)();/* display info */
	}; 

/* Vendor-specific routines struct */

struct VVS {
	PVOID (* AllocatePool)();		/* allocate fw memory */	
	VOID (* StallExecution)();		/* sleep for usec */
        ULONG (* Print)();                      /* print */
	VOID (* ReturnExtendedSystemInformation)();  /* system/processor info*/
	ARC_STATUS(* VideoDisplayInitialize)();	/* inits vga/clears screen  */
	ULONG (* EISAReadRegisterBufferUCHAR)();/* read byte from EISA mem */
	ULONG (* EISAWriteRegisterBufferUCHAR)();/* write byte to EISA mem */
	UCHAR (* EISAReadPortUCHAR)();		/* read byte from EISA IO */
	USHORT (* EISAReadPortUSHORT)();	/* read word from EISA IO */
	ULONG (* EISAReadPortULONG)();		/* read longword from EISA IO */
	UCHAR (* EISAWritePortUCHAR)();		/* write byte to EISA IO */
	USHORT (* EISAWritePortUSHORT)();	/* write word to EISA IO */
	ULONG (* EISAWritePortULONG)();		/* write longword to EISA IO */
	VOID (* FreePool)();			/* Free allocated memory */
	VOID (* CallBios)();
	BOOLEAN (* TranslateBusAddress)(); 	/* Translate a bus address */
	UCHAR (* ReadPortUCHAR)();
	USHORT (* ReadPortUSHORT)();
	ULONG (* ReadPortULONG)();
	UCHAR (* WritePortUCHAR)();
	USHORT (* WritePortUSHORT)();
	ULONG (* WritePortULONG)();
	UCHAR (* ReadRegisterUCHAR)();
	USHORT (* ReadRegisterUSHORT)();
	ULONG (* ReadRegisterULONG)();
	UCHAR (* WriteRegisterUCHAR)();
	USHORT (* WriteRegisterUSHORT)();
	ULONG (* WriteRegisterULONG)();
	ULONG (* GetBusDataByOffset)();
	ULONG (* SetBusDataByOffset)();
  	ULONG (* WidePrint)();
	LONG (* ReturnLanguageId)();
	VOID (* GetAdapter)();
	VOID (* AllocateCommonBuffer)();
	VOID (* FreeCommonBuffer)();
 };	


/* EISA-specific callbacks - for Adapter0 */
/* these may not be used.		  */
struct EISACALLBACK {
	ARC_STATUS (*ProcessEndOfInterrupt)();
	BOOLEAN	(*TestEISAInterrupt)();
	ARC_STATUS (*RequestEISADMATransfer)();
	ARC_STATUS (*AbortEISADMATransfer)();
	ARC_STATUS (*GetEISADMATransferStatus)();
	VOID (*DoEISALockedOperation)();
	ARC_STATUS (*RequestBusMasterTransfer)();
	ARC_STATUS (*ReleaseBusMasterTransfer)();
	ARC_STATUS (*RequestCPUAccessToEISABus)();
	ARC_STATUS (*ReleaseCPUAccessToEISABus)();
	VOID (*FlushCache)();
	VOID (*InvalidateCache)();
	VOID (*RESERVED)();
	VOID (*BeginCriticalSection)();
	VOID (*EndCriticalSection)();
	ARC_STATUS (*GenerateTone)();	
	VOID (*FlushWriteBuffer)();
	BOOLEAN (*Yield)();
	VOID (*StallProcessor)();
 }; 

#if defined(_ALPHA_)


#define SYSTEM_BLOCK ((PSYSTEM_PARAMETER_BLOCK)(KSEG0_BASE | 0x6FE000))

#elif defined(_X86_)

extern SYSTEM_PARAMETER_BLOCK GlobalSystemBlock;
#define SYSTEM_BLOCK (&GlobalSystemBlock)

#endif
/*
** System Id returned from GetSystemId call  pg 93
*/
typedef struct _SYSTEM_ID {
	CHAR VendorId[8];
	CHAR ProductId[8];
	} SYSTEM_ID, *PSYSTEM_ID;
#define _MEMORY_TABLE_SIZE 20

/*
** Memory Type structure - pg 94
*/
typedef enum _MEMORY_TYPE {
	MemoryExceptionBlock,
	MemorySystemBlock,
	MemoryFree,
	MemoryBad,
	MemoryLoadedProgram,
	MemoryFirmwareTemporary,
	MemoryFirmwarePermanent,
	MemoryFreeContiguous,
	MemoryMaximum
	} MEMORY_TYPE;

/*
** Memory Descriptor - pg 94
*/
typedef struct _MEMORY_DESCRIPTOR {
	MEMORY_TYPE  MemoryType;
	ULONG	    BasePage;
	ULONG        PageCount;
	} MEMORY_DESCRIPTOR, *PMEMORY_DESCRIPTOR;


typedef struct _FW_MEMORY_DESCRIPTOR {
	LIST_ENTRY ListEntry;
	MEMORY_DESCRIPTOR MemoryEntry;
	} FW_MEMORY_DESCRIPTOR, *PFW_MEMORY_DESCRIPTOR;

/*
 * Memory descriptors completely tile memory, are always
 * kept in ascending order, and no two adjacent descriptors
 * will have the same memory type.
 */
typedef struct _ARC_MEMD {
    struct _ARC_MEMD *flink;
    struct _ARC_MEMD *blink;
    MEMORY_TYPE memorytype;
    ULONG basepage;
    ULONG pagecount;
} ARC_MEMD;

/*
** Time information structure - pg 95
*/
typedef struct {
	USHORT   Year;
	USHORT   Month;
	USHORT   Day;
	USHORT   Hour;
	USHORT   Minute;
	USHORT   Second;
	USHORT   Milliseconds;
	USHORT	 Weekday; /* may not be USHORT - not explicitly defined */
	} TIME_FIELDS, *PTIME_FIELDS;

/*
** Directory entry structure - pg 96
*/
typedef struct _DIRECTORY_ENTRY {
	ULONG	FileNameLength;
	UCHAR	FileAttribute;
	CHAR	FileName[32];
	char	pad[3];
	}DIRECTORY_ENTRY, *PDIRECTORY_ENTRY;

/*
** Input / Output fucntions - pg 97
*/
typedef enum _OPEN_MODE {
	ArcOpenReadOnly,
	ArcOpenWriteOnly,
	ArcOpenReadWrite,
	ArcCreateWriteOnly,
	ArcCreateReadWrite,
	ArcSupersedeWriteOnly,
	ArcSupersedeReadWrite,
	ArcOpenDirectory,
	ArcCreateDirectory,
	ArcOpenMaximumMode
	} OPEN_MODE;

/* pg 97 */
typedef enum _SEEK_MODE {
	SeekAbsolute,
	SeekRelative,
	SeekMaximum
	} SEEK_MODE;

/* pg 97 */
typedef enum _MOUNT_OPERATION {
	MountLoadMedia,
	MountUnLoadMedia
	} MOUNT_OPERATION;

/* pg 97 */
#if REALLY_INT64
typedef __int64 LARGE_INTEGER;
#else /* !REALLY_INT64 */
typedef struct {
	ULONG	LowPart;
	LONG	HighPart;
	} LARGE_INTEGER;
#endif /* !REALLY_INT64 */



/* pg 89 */
typedef enum _CONFIGURATION_TYPE
 {
	ArcSystem,
	CentralProcessor,
	FloatingPointProcessor,
	PrimaryICache,
	PrimaryDCache,
	SecondaryICache,
	SecondaryDCache,
	SecondaryCache,
	EisaAdapter,
	TcAdapter,
	ScsiAdapter,
	DtiAdapter,
	MultiFunctionAdapter,
	DiskController,
	TapeController,
	CdromController,
	WormController,
	SerialController,
	NetworkController,
	DisplayController,
	ParallelController,
	PointerController,
	KeyboardController,
	AudioController,
	OtherController,
	DiskPeripheral,
	FloppyDiskPeripheral,
	TapePeripheral,
	ModemPeripheral,
	MonitorPeripheral,
	PrinterPeripheral,
	PointerPeripheral,
	KeyboardPeripheral,
	TerminalPeripheral,
	OtherPeripheral,
	LinePeripheral,
	NetworkPeripheral,
	SystemMemory,
	MaximumType
 } CONFIGURATION_TYPE, *PCONFIGURATION_TYPE;

/* pg 98 */
typedef struct _FILE_INFORMATION {
	LARGE_INTEGER	StartingAddress;
	LARGE_INTEGER 	EndingAddress;
	LARGE_INTEGER   CurrentPosition;
	CONFIGURATION_TYPE	Type;
	ULONG		FileNameLength;
	UCHAR		Attributes;
	CHAR		FileName[32];
	} FILEINFORMATION, *PFILE_INFORMATION;

/* pg 89 */
typedef enum _CONFIGURATION_CLASS {
	SystemClass,
	ProcessorClass,
	CacheClass,
	AdapterClass,
	ControllerClass,
	PeripheralClass,
	MemoryClass,
	MaximumClass
 	} CONFIGURATION_CLASS, *PCONFIGURATION_CLASS;



/* pg 89 */
typedef struct _DEVICE_FLAGS_ {
	ULONG Failed : 1;
	ULONG ReadOnly : 1;
	ULONG Removable : 1;
	ULONG ConsoleIn : 1;
	ULONG ConsoleOut : 1;
	ULONG Input : 1;
	ULONG Output : 1;
	ULONG unused : 32-7;
	} DEVICE_FLAGS;

#define	Flags$m_Failed  1
#define	Flags$m_ReadOnly 2
#define	Flags$m_Removable 4
#define	Flags$m_ConsoleIn 8
#define	Flags$m_ConsoleOut 16
#define	Flags$m_Input 32
#define	Flags$m_Output 64


/* pg 89 */
typedef struct _CONFIGURATION_COMPONENT {
	CONFIGURATION_CLASS	Class;
	CONFIGURATION_TYPE	Type;
	DEVICE_FLAGS	Flags;
	USHORT		Version;
	USHORT		Revision;
	ULONG		Key;
	ULONG		AffinityMask;
	ULONG		ConfigurationDataLength;
	ULONG		IdentifierLength;
	CHAR		*Identifier; /* not necessarily null terminated */

	/* begining of our private stuff */

	/* For components that are real devices, and therefore at the
	 * end of the tree, we remember our internal console name.
	 * eg: dka0.0.0.5.0
	 */
	char		cname [32];

	/* For devices that support file systems, such as disks,
	 * we also remember the name of the file system driver.
	 * This name may include the partition driver, eg:
	 * cdfs, fat/part:a, ntfs
	 */
	char		fs [32];

 	} CONFIGURATION_COMPONENT, *PCONFIGURATION_COMPONENT;	




/* pg 89 */
typedef struct _CONFIGDATAHEADER
 {
	USHORT 		Version;
	USHORT		Revision;
	PCHAR		Type;
	PCHAR		Vendor;
	PCHAR		ProductName;
	PCHAR		SerialNumber;
 } CONFIGDATAHEADER, *PCONFIGDATAHEADER;

#define CONFIGDATAHEADER_SIZE sizeof(CONFIGDATAHEADER)


/* identifier for vender specific config details */
typedef struct {
	CHAR  Details[];
 	} CONFIGDETAILS;

/* pg 90 */
typedef struct  {
	CONFIGDATAHEADER Header;
	CONFIGDETAILS	 ComponentDetails;  
 	} CONFIGDATA;

typedef struct tree				/* configuration tree node */
{
	CONFIGURATION_COMPONENT component;
	struct tree *first_child;
	struct tree *next_sibling;
	struct tree *parent;

} TREE;


/* generic display configration data structure */

typedef struct _MONITOR_CONFIGURATION_DATA {
    USHORT Version;
    USHORT Revision;
    USHORT HorizontalResolution;
    USHORT HorizontalDisplayTime;
    USHORT HorizontalBackPorch;
    USHORT HorizontalFrontPorch;
    USHORT HorizontalSync;
    USHORT VerticalResolution;
    USHORT VerticalBackPorch;
    USHORT VerticalFrontPorch;
    USHORT VerticalSync;
    USHORT HorizontalScreenSize;
    USHORT VerticalScreenSize;
} MONITOR_CONFIGURATION_DATA, *PMONITOR_CONFIGURATION_DATA;


/* generic floppy disk configuration data structure */

typedef struct _FLOPPY_CONFIGURATION_DATA {
    USHORT Version;
    USHORT Revision;
    CHAR Size[8];
    ULONG MaxDensity;
    ULONG MountDensity;
} FLOPPY_CONFIGURATION_DATA, *PFLOPPY_CONFIGURATION_DATA;

/*
 * Define G300 configuration data structure.
 * To minimize code changes, Alpha/Jensen will use the same structure.
 */

typedef struct _JAZZ_G300_CONFIGURATION_DATA {
    USHORT Version;
    USHORT Revision;
    USHORT Irql;
    USHORT Vector;
    ULONG ControlBase;
    ULONG ControlSize;
    ULONG CursorBase;
    ULONG CursorSize;
    ULONG FrameBase;
    ULONG FrameSize;            
} JAZZ_G300_CONFIGURATION_DATA, *PJAZZ_G300_CONFIGURATION_DATA;

typedef JAZZ_G300_CONFIGURATION_DATA JENSEN_CONFIGURATION_DATA;
typedef PJAZZ_G300_CONFIGURATION_DATA PJENSEN_CONFIGURATION_DATA;
/* for Display Controller configuration data */
#define VIDEO_CONTROL_VIRTUAL_BASE  0xB60003C0  /*THESE PROBABLY WILL CHANGE!*/
#define VIDEO_MEMORY_VIRTUAL_BASE   0xB40B8000 

/* types of video cards.  for VideoDisplayInitialize */
typedef enum _ALPHA_VIDEO_TYPE {
    _Paradise_WD90C11,
    _Compaq_QVision,
    _Cardinal_S3_924,
    _S3_928,
    _ATI_Mach,
    MaximumJensenVideo
    } ALPHA_VIDEO_TYPE, *PALPHA_VIDEO_TYPE;


/* memory allocation structures */

typedef enum _TYPE_OF_MEMORY {
    LoaderExceptionBlock = MemoryExceptionBlock,
    LoaderSystemBlock = MemorySystemBlock,
    LoaderFree = MemoryFree,
    LoaderBad = MemoryBad,
    LoaderLoadedProgram = MemoryLoadedProgram,
    LoaderFirmwareTemporary = MemoryFirmwareTemporary,
    LoaderFirmwarePermanent = MemoryFirmwarePermanent,
    LoaderOsloaderHeap,
    LoaderOsloaderStack,
    LoaderSystemCode,
    LoaderHalCode,
    LoaderBootDriver,
    LoaderConsoleInDriver,
    LoaderConsoleOutDriver,
    LoaderStartupDpcStack,
    LoaderStartupKernelStack,
    LoaderStartupPanicStack,
    LoaderStartupPcrPage,
    LoaderStartupPdrPage,
    LoaderRegistryData,
    LoaderMemoryData,
    LoaderNlsData,
    LoaderMaximum
    } TYPE_OF_MEMORY;

typedef struct _MEMORY_ALLOCATION_DESCRIPTOR {
    LIST_ENTRY ListEntry;
    TYPE_OF_MEMORY MemoryType;
    ULONG BasePage;
    ULONG PageCount;
} MEMORY_ALLOCATION_DESCRIPTOR, *PMEMORY_ALLOCATION_DESCRIPTOR;


/* loader parameter block */

typedef struct _NLS_DATA_BLOCK {
    PVOID AnsiCodePageData;
    PVOID OemCodePageData;
    PVOID UnicodeCaseTableData;
} NLS_DATA_BLOCK, *PNLS_DATA_BLOCK;

typedef struct _ARC_DISK_INFORMATION {
    ULONG BootDriveSignature;
    ULONG SystemDriveSignature;
} ARC_DISK_INFORMATION, *PARC_DISK_INFORMATION;







typedef struct _ALPHA_LOADER_BLOCK {
    ULONG DpcStack;
    ULONG FirstLevelDcacheSize;
    ULONG FirstLevelDcacheFillSize;
    ULONG FirstLevelIcacheSize;
    ULONG FirstLevelIcacheFillSize;
    ULONG GpBase;
    ULONG PanicStack;
    ULONG PcrPage;
    ULONG PdrPage;
    ULONG SecondLevelDcacheSize;
    ULONG SecondLevelDcacheFillSize;
    ULONG SecondLevelIcacheSize;
    ULONG SecondLevelIcacheFillSize;
    ULONG PhysicalAddressBits;
    ULONG MaximumAddressSpaceNumber;
    UCHAR SystemSerialNumber[16];
    UCHAR SystemType[8];
    ULONG SystemVariant;
    ULONG SystemRevision;
    ULONG ProcessorType;
    ULONG ProcessorRevision;
    ULONG CycleClockPeriod;
    ULONG PageSize;
    PVOID RestartBlock;
    UQUAD FirmwareRestartAddress;
    ULONG FirmwareRevisionId;
    PVOID PalBaseAddress;
} ALPHA_LOADER_BLOCK, *PALPHA_LOADER_BLOCK;


typedef struct _LOADER_PARAMETER_BLOCK {
    LIST_ENTRY LoadOrderListHead;
    LIST_ENTRY MemoryDescriptorListHead;
    LIST_ENTRY BootDriverListHead;
    ULONG KernelStack;
    ULONG Prcb;
    ULONG Process;
    ULONG Thread;
    ULONG RegistryLength;
    PVOID RegistryBase;
    TREE ConfigurationRoot;
    PCHAR ArcBootDeviceName;
    PCHAR ArcHalDeviceName;
    PCHAR NtBootPathName;
    PCHAR NtHalPathName;
    PCHAR LoadOptions;
    PNLS_DATA_BLOCK NlsData;
    PARC_DISK_INFORMATION ArcDiskInformation;
    PVOID OemFontFile;
    ULONG Spare2;
    ULONG Spare3;
    ALPHA_LOADER_BLOCK Alpha;

} LOADER_PARAMETER_BLOCK, *PLOADER_PARAMETER_BLOCK;


typedef struct 
 {
 	int configclass;
	int configtype;
	char *mnemonic;
	char *driver_id;
 } path_table;



/* non-ARC defined structures */

/*
 * Structure used to return some detailed information on Alpha AXP systems
 * and processors.
 */

typedef struct _EXTENDED_SYSTEM_INFORMATION {
    ULONG   ProcessorId;
    ULONG   ProcessorRevision;
    ULONG   ProcessorPageSize;
    ULONG   NumberOfPhysicalAddressBits;
    ULONG   MaximumAddressSpaceNumber;
    ULONG   ProcessorCycleCounterPeriod;
    ULONG   SystemRevision;
    UCHAR   SystemSerialNumber[16];
    UCHAR   FirmwareVersion[16];
} EXTENDED_SYSTEM_INFORMATION, *PEXTENDED_SYSTEM_INFORMATION;

/*
 * Define types of bus information. 
 */
typedef enum _BUS_DATA_TYPE {
    Cmos,
    EisaConfiguration,
    Pos,
    CbusConfiguration,
    PCIConfiguration,
    VMEConfiguration,
    NuBusConfiguration,
    PCMCIAConfiguration,
    MPIConfiguration,
    MPSAConfiguration,
    MAximumBusDataType
} BUS_DATA_TYPE, *PBUS_DATA_TYPE;

/*
 * I/O bus interface types
 */
typedef enum _INTERFACE_TYPE {
    Internal,
    Isa,
    Eisa,
    MicroChannel,
    TurboChannel,
    PCIBus,
    VMSBus,
    NuBus,
    PCMCIABus,
    Cbus,
    MPIBus,
    MPSABus,
    MaximumInterfaceType
} INTERFACE_TYPE, *PINTERFACE_TYPE;

/*
 * Vendor specific structure
 */

/* Microsoft to Common Console redefinitions */

#define FwGetPeer(x)	GetPeer(x)
#define FwGetChild(x)	GetChild(x)
#define FwGetParent(x)	GetParent(x)
#define FwAddChild(x,y,z)	AddChild(x,y,z)
#define FwDeleteComponent(x) DeleteComponent(x)
#define FwGetComponent(x)	GetComponent(x)
#define FwGetConfigurationData(x,y) GetConfigurationData(x,y)
#define FwSaveConfiguration()	SaveConfiguration()
#define FwGetTime(x)	GetTime(x)
#define FwGetSystemId() GetSystemId()
#define FwFlushAllCaches()		FlushAllCaches()
#define FwTestUnicodeCharacter(x,y)	TestUnicodeCharacter(x,y)
#define FwGetDisplayStatus(x)		GetDisplayStatus(x)

/* translate vendor-specific routines into Common Console routines */

#define FwAllocatePool(x)		AllocatePool(x)
#define FwStallExecution(x)		StallExecution(x)
#define FwDisplayExtendedSystemInformation(x) DisplayExtendedSystemInformation(x) 
#define FwVideoDisplayInitialize(x) VideoDisplayInitialize(x)
#define FwEISAReadRegisterBufferUCHAR(x,y,z,a) EISAReadRegisterBufferUCHAR(x,y,a,z)
#define FwEISAWriteRegisterBufferUCHAR(x,y,z,a) EISAWriteRegisterBufferUCHAR(x,y,z,a)
#define FwEISAReadPortUCHAR(x,y)	EISAReadPortUCHAR(x,y)
#define FwEISAReadPortUSHORT(x,y)  	EISAReadPortUSHORT(x,y)
#define FwEISAReadPortULONG(x,y)   	EISAReadPortULONG(x,y)
#define FwEISAWritePortUCHAR(x,y,z)	EISAWritePortUCHAR(x,y,z)
#define FwEISAWritePortUSHORT(x,y,z) 	EISAWritePortUSHORT(x,y,z)
#define FwEISAWritePortULONG(x,y,z)  	EISAWritePortULONG(x,y,z)

#if 0
/* Microsoft uses a function prototype w. ... */
#define FwPrint(x,...)		Print(x,...)
#endif

#define	AlphaInstMB()		mb()
#define	AlphaInstIMB()		imb()

#endif

typedef struct _MIPS_RELOCATION_TYPE {
    ULONG   SymbolIndex:24;
    ULONG   Reserved:3;
    ULONG   Type:4;
    ULONG   External:1;
} MIPS_RELOCATION_TYPE, *PMIPS_RELOCATION_TYPE;

typedef struct _MIPS_RELOCATION_ENTRY {
    ULONG   VirtualAddress;
    MIPS_RELOCATION_TYPE Type;
} MIPS_RELOCATION_ENTRY, *PMIPS_RELOCATION_ENTRY;

typedef struct _MIPS_SYMBOLIC_HEADER {
    SHORT   Magic;
    SHORT   VersionStamp;
    ULONG   NumOfLineNumberEntries;
    ULONG   BytesForLineNumberEntries;
    ULONG   PointerToLineNumberEntries;
    ULONG   NumOfDenseNumbers;
    ULONG   PointerToDenseNumbers;
    ULONG   NumOfProcedures;
    ULONG   PointerToProcedures;
    ULONG   NumOfLocalSymbols;
    ULONG   PointerToLocalSymbols;
    ULONG   NumOfOptimizationEntries;
    ULONG   PointerToOptimizationEntries;
    ULONG   NumOfAuxSymbols;
    ULONG   PointerToAuxSymbols;
    ULONG   NumOfLocalStrings;
    ULONG   PointerToLocalStrings;
    ULONG   NumOfExternalStrings;
    ULONG   PointerToExternalStrings;
    ULONG   NumOfFileDescriptors;
    ULONG   PointerToFileDescriptors;
    ULONG   NumOfRelativeFileDescriptors;
    ULONG   PointerToRelativeFileDescriptors;
    ULONG   NumOfExternalSymbols;
    ULONG   PointerToExternalSymbols;
} MIPS_SYMBOLIC_HEADER, *PMIPS_SYMBOLIC_HEADER;

typedef struct _MIPS_LOCAL_SYMBOL {
    ULONG   IndexToSymbolString;
    ULONG   Value;
    ULONG   Type:6;
    ULONG   StorageClass:5;
    ULONG   Reserved:1;
    ULONG   Index:20;
} MIPS_LOCAL_SYMBOL, *PMIPS_LOCAL_SYMBOL;

typedef struct _MIPS_EXTERNAL_SYMBOL {
    USHORT  Reserved;
    USHORT  PointerToFileDescriptor;
    MIPS_LOCAL_SYMBOL Symbol;
} MIPS_EXTERNAL_SYMBOL, *PMIPS_EXTERNAL_SYMBOL;

typedef struct _SECTION_RELOCATION_ENTRY {
    ULONG FixupValue;
    ULONG PointerToRelocations;
    USHORT NumberOfRelocations;
} SECTION_RELOCATION_ENTRY, *PSECTION_RELOCATION_ENTRY;

