#ifndef _NT_TYPES_
#define _NT_TYPES_

/*
// Some Types used Here
*/
#define ULONG		unsigned long
#define PULONG		ULONG *
#define LONG		long
#define PLONG		LONG *
#define UINT		unsigned __int64
#define PUINT		UINT *
#define INT			__int64
#define PINT		INT *
#define USHORT		unsigned short
#define PUSHORT		USHORT *
#define SHORT		short
#define PSHORT		SHORT *
#define UCHAR		unsigned char
#define PUCHAR		UCHAR *
#define CHAR		char
#define PCHAR		CHAR *
#define VOID		void
#define PVOID		VOID *

#define IN
#define OUT

#define OFFSETOF(s,m)   (size_t)&(((s *)0)->m)
#define ARRAYSIZE(x)	(sizeof(x)/sizeof(x[0]))
#define ASSERT(X)

#ifndef DEBUG_TXT
#define DEBUG_TXT 0
#endif
#if DEBUG_TXT
#define	DbgPrint(x)		qprintf x
#else
#define	DbgPrint(x)
#endif

/* VENDOR IDs */
#define PCI_VENDORID_ADAPTEC    0x9004
#define PCI_VENDORID_ADAPTEC_ALT 0x9005
#define PCI_VENDORID_ALTEON     0x12AE
#define PCI_VENDORID_ATI        0x1002
#define PCI_VENDORID_BUSLOGIC   0x104B
#define PCI_VENDORID_CMD        0x1095
#define PCI_VENDORID_CYPRESS    0x1080
#define PCI_VENDORID_DEC        0x1011
#define PCI_VENDORID_DPT        0x1044
#define PCI_VENDORID_INITIO     0x1101
#define PCI_VENDORID_INTEL      0x8086
#define PCI_VENDORID_MATROX     0x102B
#define PCI_VENDORID_MYLEX      0x1069
#define PCI_VENDORID_NCR        0x1000
#define PCI_VENDORID_QLOGIC     0x1077
#define PCI_VENDORID_S3         0x5333
#define PCI_VENDORID_TSENG_LABS 0x100C
#define PCI_VENDORID_ACER       0x10B9
#define PCI_VENDORID_POLARIS    0x1004
#define PCI_VENDORID_INTRASERVER 0x13E9
#define PCI_VENDORID_COMPAQ     0x0E11
#define PCI_VENDORID_EMULEX     0x10DF
#define PCI_VENDORID_ELSA       0x1048
#define PCI_VENDORID_AMD        0x1022


/*
// PCI_VENDORID_ACER
*/
#define PCI_DEVICEID_ACER_PCIISA    0x1533
#define PCI_DEVICEID_ACER_IDE       0x5229
#define PCI_DEVICEID_ACER_USB       0x5237
#define PCI_DEVICEID_ACER_PMU       0x7101

/*
// PCI_VENDORID_AMD
*/
#define PCI_DEVICEID_AMD751_PCI	0x7006
#define PCI_DEVICEID_AMD751_AGP	0x7007

#define MAKEVENID(vendor,device)		(((unsigned long)(device) << 16)|((unsigned long)(unsigned short)(vendor)))

/*
// Some useful NT type macros!!
*/
#define WRITE_PORT_UCHAR( _x_, _y_ )	outportb( NULL, _x_, _y_ )
#define WRITE_PORT_USHORT( _x_, _y_ )	outportw( NULL, _x_, _y_ )
#define WRITE_PORT_ULONG( _x_, _y_ )	outportl( NULL, _x_, _y_ )
#define READ_PORT_UCHAR( _x_ )			inportb( NULL, _x_ )
#define READ_PORT_USHORT( _x_ )			inportw( NULL, _x_ )
#define READ_PORT_ULONG( _x_ )			inportl( NULL, _x_ )

#define WRITE_REGISTER_UCHAR( _x_, _y_ )	outmemb( NULL, _x_, _y_ )
#define WRITE_REGISTER_USHORT( _x_, _y_ )	outmemw( NULL, _x_, _y_ )
#define WRITE_REGISTER_ULONG( _x_, _y_ )	outmeml( NULL, _x_, _y_ )
#define READ_REGISTER_UCHAR( _x_ )			inmemb( NULL, _x_ )
#define READ_REGISTER_USHORT( _x_ )			inmemw( NULL, _x_ )
#define READ_REGISTER_ULONG( _x_ )			inmeml( NULL, _x_ )

#endif
