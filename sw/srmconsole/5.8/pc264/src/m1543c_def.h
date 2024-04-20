/*++

Copyright (c) 1995,1996 Digital Equipment Corporation

Module Name:

    m1543c.h

Abstract:

    This file contains definitions necessary for the combo
    driver for the Acer m1543c Super I/O Controller.

Author:

    Dick Bissen      15-Jun-1998

Revision History:


--*/

#ifndef __ACER_m1543c_H__
#define __ACER_m1543c_H__

#include	"cp$src:nt_types.h"

/*++
// Configuration on/off keys
--*/
#define M1543c_CONFIG_ON_KEY1        (0x51)
#define M1543c_CONFIG_ON_KEY2        (0x23)
#define M1543c_CONFIG_OFF_KEY        (0xBB)

/*++
// Constants
--*/
#define M1543c_DEVICE_ON             (0x01)
#define M1543c_DEVICE_OFF            (0x00)
#define M1543c_DEVICE_ID             (0x43)
#define M1535d_DEVICE_ID             (0x53)
#define M1543c_DEVICE_ALTID          (0x15)

/*++
// SMC M1543c config registers
--*/
typedef struct _M1543c_CONFIG_REGS {
  UCHAR ConfigurationIndexRegister; /*  W				*/
  UCHAR ConfigurationDataRegister;  /* R/W (config mode)	*/
} M1543c_CONFIG_REGS, *PM1543c_CONFIG_REGS;

/*
** Device Function Definitions
*/
#define SERIAL_0	0
#define SERIAL_1	1
#define PARALLEL_0	2
#define FLOPPY_0	3
#define KBD_CTRL_0	4
#define NUM_FUNCS	5
#define UNUSED_OPTION   ((ULONG)0xffffffff)

/*
** Default Device Function Mappings
*/
#define COM1_BASE	0x3F8
#define COM1_IRQ	4
#define COM2_BASE	0x2F8
#define COM2_IRQ	3
#define PARP_BASE	0x3BC
#define PARP_IRQ	7
#define PARP_DRQ	3
#define FDC_BASE	0x3F0
#define FDC_IRQ		6
#define FDC_DRQ		2
#define KBD_IRQ		1
#define PS2_IRQ		12

/*++
// Logical device numbers
--*/
#define M1543c_FLOPPY_DISK   (0)
#define M1543c_PARALLEL_PORT (3)
#define M1543c_SERIAL_1      (4)
#define M1543c_SERIAL_2      (5)
#define M1543c_KEYBOARD      (7)
#define M1543c_COMMONIO      (8)
#define M1543c_SERIAL_3      (0xB)
#define M1543c_HOTKEY        (0xC)
#define M1543c_INVALID_DEVICE        (0xff)

/*++
// chip level register offsets
--*/
#define M1543c_CONFIG_RESET_INDEX            (0x02)
#define M1543c_INDEX_ADDRESS_INDEX           (0x03)
#define M1543c_LOGICAL_DEVICE_NUMBER_INDEX   (0x07)
#define M1543c_DEVICE_REV_INDEX              (0x1F)
#define M1543c_DEVICE_ID_INDEX               (0x20)
#define M1543c_DEVICE_ALTID_INDEX            (0x21)
#define M1543c_POWER_CONTROL_INDEX           (0x22)
#define M1543c_POWER_MGMT_INDEX              (0x23)
#define M1543c_ACTIVATE_INDEX                (0x30)
#define M1543c_ADDR_HI_INDEX                 (0x60)
#define M1543c_ADDR_LOW_INDEX                (0x61)
#define M1543c_INTERRUPT_INDEX               (0x70)
#define M1543c_SECOND_INTERRUPT_INDEX        (0x72)
#define M1543c_DMA_CHANNEL_INDEX             (0x74)
#define M1543c_FDD_MODE_INDEX                (0xf0)
#define M1543c_FDD_OPTION_INDEX              (0xf1)

/*
//  Defines for the M5229 IDE Controller contained within the 
//  Acer M1543c.
*/

/*
// IDE Command Register
*/

typedef union _M5229_IDE_COMMAND {
    struct {
        USHORT IoAccessEnable:1;        /* bit 0 IO Access Enable */
        USHORT MemoryAccessEnable:1;    /* bit 1 Memory Access Enable */
        USHORT BusMasterEnable:1;       /* bit 2 Bus Master (DMA) Enable */
        USHORT Reserved1:4;             /* bits 6:3 Reserved */
        USHORT Reserved2:1;             /* bit 7 Reserved */
        USHORT SErrEnable:1;            /* bit 8 SERR Reporting Enable */
        USHORT Reserved3:1;             /* bit 9 Reserved */
        USHORT Reserved4:6;             /* bits 15:10 Reserved */
    } u;
    USHORT AsUSHORT;
} M5229_IDE_COMMAND, *PM5229_IDE_COMMAND;

/*
//  M5229 IDE Class Code Attribute III Register
*/

typedef union _M5229_IDE_CCAIII {
    struct {
        UCHAR IdeEnable:1;
        UCHAR ProgInterfaceEnb:1;
        UCHAR Reserved2:1;
        UCHAR DecodeMethodCompat:1;
        UCHAR Reserved4:1;
        UCHAR DecodeMethodNative:1;
        UCHAR Reserved6:1;
        UCHAR Reserved7:1;
    } u;
    UCHAR AsUCHAR;
} M5229_IDE_CCAIII, *PM5229_IDE_CCAIII;

/*
//  M5229 IDE Flexible Channel Setting Register
*/

typedef union _M5229_IDE_FCS {
    struct {
        UCHAR   SecondaryDrive1TimingSelect:1;
        UCHAR   SecondaryDrive0TimingSelect:1;
        UCHAR   PrimaryDrive1TimingSelect:1;
        UCHAR   PrimaryDrive0TimingSelect:1;
        UCHAR   ExchangeChannels:1;
        UCHAR   Cfg1PrimaryChannel:1;
        UCHAR   Cfg1SecondaryChannel:1;
        UCHAR   ExchangeMaster:1;
    } u;
    UCHAR   AsUCHAR;
} M5229_IDE_FCS, *PM5229_IDE_FCS;

/*
// M5229 IDE CDROM Control Register
*/
typedef union _M5229_IDE_CDRC {
    struct {
        UCHAR   CdromDmaMode:1;
        UCHAR   CdromFifoEnb:1;
        UCHAR   Reserved2:1;
        UCHAR   MaskBaseAddr:1;
        UCHAR   Reserved4:1;
        UCHAR   Reserved5:1;
        UCHAR   Reserved6:1;
        UCHAR   LockAttrControl:1;
    } u;
    UCHAR   AsUCHAR;
} M5229_IDE_CDRC, *PM5229_IDE_CDRC;

/*
// M5229 IDE FIFO Threshold Register for Primary/Secondary
*/
typedef union _M5229_IDE_FTH {
    struct {
        UCHAR   FifoThresDrive0:2;
        UCHAR   OperationLevelDrive0:2;
        UCHAR   FifoThresDrive1:2;
        UCHAR   OperationLevelDrive1:2;
    } u;
    UCHAR   AsUCHAR;
} M5229_IDE_FTH, *PM5229_IDE_FTH;

/*
// M5229 IDE Ultra DMA Setting Register for Primary/Secondary
*/
typedef union _M5229_IDE_UDMA {
    struct {
        UCHAR   Drive0Timing:3;
        UCHAR   Drive0UDMAEnb:1;
        UCHAR   Drive1Timing:3;
        UCHAR   Drive1UDMAEnb:1;
    } u;
    UCHAR   AsUCHAR;
} M5229_IDE_UDMA, *PM5229_IDE_UDMA;

/*
// M5229 IDE Channel Address Setup Timing for Primary/Secondary
*/
typedef union _M5229_IDE_CAS {
    struct {
        UCHAR   AddrTiming:3;
        UCHAR   Reserved3:1;
        UCHAR   Reserved4:1;
        UCHAR   Reserved5:1;
        UCHAR   Reserved6:1;
        UCHAR   Reserved7:1;
    } u;
    UCHAR   AsUCHAR;
} M5229_IDE_CAS, *PM5229_IDE_CAS;

/*
// M5229 IDE Channel Command Block Timing for Primary/Secondary
*/
typedef union _M5229_IDE_CCB {
    struct {
        UCHAR   CmdRecovery:4;
        UCHAR   CmdActive:3;
        UCHAR   Reserved7:1;
    } u;
} M5229_IDE_CCB, *PM5229_IDE_CCB;

/*
// M5229 IDE Channel Drive Read/Write Data Timing
*/
typedef union _M5229_IDE_CDT { 
    struct {
        UCHAR   DataRecovery:4;
        UCHAR   DataActive:3;
        UCHAR   Reserved7:1;
    } u;
    UCHAR   AsUCHAR;
} M5229_IDE_CDT, *PM5229_IDE_CDT;

/*
// M5229 IDE Channel Configuration Data Structure
*/
typedef struct _M5229_IDE_CCFG {
    M5229_IDE_CAS   CAS;
    M5229_IDE_CCB   CCB;
    M5229_IDE_CDT   Drive[2];
} M5229_IDE_CCFG, *PM5229_IDE_CCFG;

/*
// Primary/Secondary IDE Channel PCI Configuration
*/

typedef struct _M5229_IDE_PCI_CONFIG {
    USHORT  VendorID;                               /* 0x00 */
    USHORT  DeviceID;                               /* 0x02 */
    M5229_IDE_COMMAND  Command;                     /* 0x04 */
    USHORT  Status;                                 /* 0x06 */
    UCHAR   RevisionID;                             /* 0x08 */
    UCHAR   ProgIf;                                 /* 0x09 */
    UCHAR   SubClass;                               /* 0x0a */
    UCHAR   BaseClass;                              /* 0x0b */
    UCHAR   CacheLineSize;                          /* 0x0c */
    UCHAR   LatencyTimer;                           /* 0x0d */
    UCHAR   HeaderType;                             /* 0x0e */
    UCHAR   Mbz1;                                   /* 0x0f */
    ULONG   PrimaryCommandAddress;                  /* 0x10 */
    ULONG   PrimaryControlAddress;                  /* 0x14 */
    ULONG   SecondaryCommandAddress;                /* 0x18 */
    ULONG   SecondaryControlAddress;                /* 0x1c */
    ULONG   BaseAddressRegisterV;                   /* 0x20 */
    UCHAR   Mbz2[24];                               /* 0x24-0x3b */
    UCHAR   InterruptLine;                          /* 0x3c */
    UCHAR   InterruptPin;                           /* 0x3d */
    UCHAR   MinGrantRegister;                       /* 0x3e */
    UCHAR   MaxLatencyRegister;                     /* 0x3f */
    UCHAR   Mbz3[3];                                /* 0x40-0x42 */
    UCHAR   ClassCodeAttrI;                         /* 0x43 */
    UCHAR   Mbz4[9];                                /* 0x44-0x4C */
    UCHAR   ClassCodeAttrII;                        /* 0x4D */
    UCHAR   Mbz5[2];                                /* 0x4E-0x4F */
    M5229_IDE_CCAIII ClassCodeAttrIII;              /* 0x50 */
    UCHAR   RestTest;                               /* 0x51 */
    M5229_IDE_FCS   FlexChannelSet;                 /* 0x52 */
    M5229_IDE_CDRC  CDROMCtrl;                      /* 0x53 */
    M5229_IDE_FTH  FifoThres[2];                    /* 0x54-0x55 */
    M5229_IDE_UDMA UltraDmaSet[2];                  /* 0x56-0x57 */
    M5229_IDE_CCFG  ChannelCfg[2];                  /* 0x58-0x5F */
    
} M5229_IDE_PCI_CONFIG, *PM5229_IDE_PCI_CONFIG;

/*
// Primary/Secondary PCI ISA Configuration
*/

#if M1543C
typedef struct _M1543_PCI_ISA_CONFIG {
	USHORT VenderID;								/* 01h-00h */
	USHORT DeviceID;								/* 03h-02h */
	USHORT Command;									/* 05h-04h */
	USHORT Status;									/* 07h-06h */
	UCHAR RevisionID;								/* 08h */
	UCHAR ClassCode[0x0c-0x09];						/* 0Bh-09h */
	USHORT Reserved1;								/* 0Dh-0Ch */
	UCHAR HeaderType;								/* 0Eh */
	UCHAR Reserved2[0x2c-0x0f];						/* 2Bh-0Fh */
	USHORT SubsystemVendorID;						/* 2Dh-2Ch */
	USHORT SubsystemDeviceID;						/* 2Fh-2Eh */
	UCHAR Reserved3[0x40-0x30];						/* 3Fh-30h */
	UCHAR PIC;										/* 40h */
	UCHAR IORC;										/* 41h */
	UCHAR ISACI;									/* 42h */
	UCHAR ISACII;									/* 43h */
	UCHAR IDENRI;									/* 44h */
	UCHAR PIPM;										/* 45h */
	UCHAR Reserved4;								/* 46h */
	UCHAR BCSC;										/* 47h */
	UCHAR PIRTI;									/* 48h */
	UCHAR PIRTII;									/* 49h */
	UCHAR PIRTIII;									/* 4Ah */
	UCHAR PIRTIV;									/* 4Bh */
	UCHAR PILET;									/* 4Ch */
	UCHAR Reserved5[0x50-0x4d];						/* 4Fh-4Dh */
	USHORT IOPWI;									/* 51h-50h */
	USHORT IOPWII;									/* 53h-52h */
	UCHAR HSSB;										/* 54h */
	UCHAR PCSAD[0x58-0x55];							/* 57h-55h */
 	UCHAR IDEIC;									/* 58h */
	UCHAR GPIS;										/* 59h */
	USHORT GPOS ;									/* 5Bh-5Ah */
	USHORT DMDC;									/* 5Dh-5Ch */
	UCHAR SMCCI;									/* 5Eh */
	UCHAR SMCCII;									/* 5Fh */
	UCHAR Reserved6[0x6d-0x60];						/* 6Ch-60h */
	UCHAR ROMRAM;									/* 6Dh */
	UCHAR ISPSS;									/* 6Eh */
	UCHAR ISPSD;									/* 6Fh */
	UCHAR SIRQCR;									/* 70h */
	UCHAR DDMAS;									/* 71h */
	UCHAR USBIDS;									/* 72h */
	UCHAR DDMABA;									/* 73h */
	UCHAR USBIR;									/* 74h */
	UCHAR IDENRII;									/* 75h */
	UCHAR SCIIR;									/* 76h */
	UCHAR SMBIR;									/* 77h */
	UCHAR AGPIS;									/* 78h */
	UCHAR OPTIONREGISTER[0x7C-0x79];				/* 7Bh-79h */
	UCHAR MAPR[0x100-0x7c];							/* FFh-7Ch */
} M1543_PCI_ISA_CONFIG, *PM1543_PCI_ISA_CONFIG;
#endif
#if M1535C
typedef struct _M1543_PCI_ISA_CONFIG {
	USHORT VenderID;								/* 01h-00h */
	USHORT DeviceID;								/* 03h-02h */
	USHORT Command;									/* 05h-04h */
	USHORT Status;									/* 07h-06h */
	UCHAR RevisionID	;							/* 08h */
	UCHAR ClassCode[0x0c-0x09];						/* 0Bh-09h */
	USHORT Reserved1;								/* 0Dh-0Ch */
	UCHAR HeaderType;								/* 0Eh */
	UCHAR Reserved2[0x2c-0x0f];						/* 2Bh-0Fh */
	USHORT SubsystemVendorID;						/* 2Dh-2Ch */
	USHORT SubsystemDeviceID;						/* 2Fh-2Eh */
	UCHAR Reserved3[0x34-0x30];						/* 33h-30h */
	UCHAR Capabilities;								/* 34h */
	UCHAR Reserved4[0x40-0x35];						/* 3Fh-35h */
	UCHAR PIC;										/* 40h */
	UCHAR IORC;										/* 41h */
	UCHAR ISACI;									/* 42h */
	UCHAR ISACII;									/* 43h */
	UCHAR IDENRI;									/* 44h */
	UCHAR PIPM;										/* 45h */
	UCHAR Reserved5;								/* 46h */
	UCHAR BCSC;										/* 47h */
	UCHAR PIRTI;									/* 48h */
	UCHAR PIRTII;									/* 49h */
	UCHAR PIRTIII;									/* 4Ah */
	UCHAR PIRTIV;									/* 4Bh */
	UCHAR Reserved6[0x4e-0x4c];						/* 4Dh-4Ch */
	USHORT HSSB;									/* 4Fh-4Eh */
	USHORT PCC;										/* 51h-50h */
	USHORT IOPWII;									/* 53h-52h	(USBDC) */
	USHORT PCS0AD;									/* 55h-54h */
	USHORT PCS1AD;									/* 57h-56h */
	UCHAR IDEIC;									/* 58h */
	UCHAR PCSDS;									/* 59h */
	UCHAR SAGPIOC;									/* 5Ah */
	UCHAR Reserved7[0x5c-0x5b];						/* 5Bh */
	USHORT DMDC;									/* 5Dh-5Ch */
	UCHAR SMCCI;									/* 5Eh */
	UCHAR SMCCII;									/* 5Fh */
	USHORT MPS1;									/* 61h-60h */
	UCHAR Reserved8[0x6c-0x62];						/* 6Bh-62h */
	UCHAR Reserved9[0x6d-0x6c];						/* 6Ch */
	UCHAR RAMROM;									/* 6Dh */
	UCHAR ISPSS;									/* 6Eh */
	UCHAR ISPSD;									/* 6Fh */
	UCHAR SIRQCR;									/* 70h */
	UCHAR DDMAS;									/* 71h */
	UCHAR USBIDS;									/* 72h */
	UCHAR DDMABA;									/* 73h */
	UCHAR USBIR;									/* 74h */
	UCHAR IDENRII;									/* 75h (IDERT) */
	UCHAR SCIIR;									/* 76h (SCIRT) */
	UCHAR AMDC;										/* 77h */
	ULONG MPS2;										/* 7Bh-78h */
	UCHAR MPS3;										/* 7Ch */
	UCHAR Reserved10[0x80-0x7d];					/* 7Fh-7Dh */
	UCHAR RRID;										/* 80h */
	UCHAR IDRWC;									/* 81h */
	UCHAR Reserved11[0x90-0x82];					/* 8Fh-82h */
	UCHAR PPMRC;									/* 90h */
	UCHAR Reserved12[0xa0-0x91];					/* 9Fh-91h */
	UCHAR CAPID;									/* A0h */
	UCHAR NIP;										/* A1h */
	USHORT PMC;										/* A3h-A2h */
	USHORT PMCSR;									/* A5h-A4h */
	UCHAR PMCSR_BSE;								/* A6h */
	UCHAR DATAR;									/* A7h */
	UCHAR Reserved13[0xb0-0xa8];					/* AFh-A8h */
	UCHAR LPCR;										/* B0h */
	UCHAR LPCGDR1[0xb4-0xb1];						/* B3h-B1h */
	ULONG LPCLIOD;									/* B7h-B4h */
	USHORT LPCGDR2;									/* B9h-B8h */
	UCHAR Reserved14[0x100-0xba];					/* FFh-BAh */
} M1543_PCI_ISA_CONFIG, *PM1543_PCI_ISA_CONFIG;
#endif

/*
// M7101 PMU PCI Configuration
*/

#if M1543C
typedef struct _M7101_PMU_CONFIG {
	USHORT VenderID;								/* 01h-00h */
	USHORT DeviceID;								/* 03h-02h */
	USHORT Command;									/* 05h-04h */
	USHORT Status;									/* 07h-06h */
	UCHAR RevisionID;								/* 08h */
	UCHAR ClassCode[0x0c-0x09];						/* 0Bh-09h */
	USHORT Reserved1;								/* 0Dh-0Ch */
	UCHAR HeaderType;								/* 0Eh */
	UCHAR Reserved2;								/* 0Fh */
	ULONG PBAPMU;									/* 13h-10h */
	ULONG SBASMB;									/* 17h-14h */
	UCHAR Reserved3[0x2c-0x18];						/* 2Bh-18h */
	USHORT SubsystemVendorID;						/* 2Dh-2Ch */
	USHORT SubsystemDeviceID;						/* 2Fh-2Eh */
	UCHAR Reserved4[0x40-0x30];						/* 3Fh-30h */
	USHORT SEOGSMI;									/* 41h-40h */
	USHORT SSOGSMI;									/* 43h-42h */
	UCHAR SEWSSMI[0x47-0x44];						/* 46h-44h */
	UCHAR Reserved5;								/* 47h */
	UCHAR SSWSSMI[0x4b-0x48];						/* 4Ah-48h */
	UCHAR Reserved6;								/* 4Bh */
	USHORT EESS;									/* 4Dh-4Ch */
	USHORT SESS;									/* 4Fh-4Eh */
	USHORT EEGS;									/* 51h-50h */
	USHORT SEGS;									/* 53h-52h */
	UCHAR _ST;										/* 54h */
	UCHAR APMTA;									/* 55h */
	UCHAR Reserved7[0x59-0x56];						/* 58h-56h */
	UCHAR GDT;										/* 59h */
	UCHAR Reserved8;								/* 5Ah */
	UCHAR ATPC;										/* 5Bh */
	ULONG Reserved9;								/* 5Fh-5Ch */
	ULONG SEMST;									/* 63h-60h */
	USHORT DEMDT;									/* 65h-64h */
	USHORT Reserved10;								/* 67h-66h */
	UCHAR SEDI;										/* 68h */
	UCHAR Reserved11[0x6c-0x69];					/* 6Bh-69h */
	ULONG SEDII;									/* 6Fh-6Ch */
	USHORT SEDIII;									/* 71h-70h */
	USHORT SEDIV;									/* 73h-72h */
	UCHAR SWUS;										/* 74h */
	UCHAR TIMBA;									/* 75h */
	UCHAR TNTDTI;									/* 76h */
	UCHAR SMI_CNTL;									/* 77h */
	USHORT PTSPLL;									/* 79h-78h */
	UCHAR PIIGS;									/* 7Ah */
	UCHAR SCC;										/* 7Bh */
	UCHAR BESI;										/* 7Ch */
	UCHAR DCGPIO;									/* 7Dh */
	UCHAR DOGPIO;									/* 7Eh */
	UCHAR DIGPIO;									/* 7Fh */
	UCHAR COESI[0x83-0x80];							/* 82h-80h */
	UCHAR Reserved12;								/* 83h */
	ULONG COEGI;									/* 87h-84h */
	ULONG COEGII;									/* 8Bh-88h */
	UCHAR COESII[0x8f-0x8c];						/* 8Eh-8Ch */
	UCHAR Reserved13;								/* 8Fh */
	UCHAR CGPESI;									/* 90h */
	UCHAR CGPESII;									/* 91h */
	USHORT Reserved14;								/* 93h-92h */
	ULONG MGA;										/* 97h-94h */
	USHORT SLEE;									/* 99h-98h */
	UCHAR Reserved15[0xa4-0x9a];					/* A3h-9Ah */
	USHORT IOGC;									/* A5h-A4h */
	UCHAR Reserved16[0xb2-0xa6];					/* B1h-A6h */
	UCHAR CSS;										/* B2h */
	UCHAR SC;										/* B3h */
	UCHAR SLED;										/* B4h */
	UCHAR LEDC;										/* B5h */
	UCHAR Reserved17;								/* B6h */
	UCHAR RIC;										/* B7h */
	USHORT ODEGPO;									/* B9h-B8h */
	USHORT IDEGPI;									/* BBh-BAh */
	UCHAR SRIO70;									/* BCh */
	UCHAR PCCA;										/* BDH */
	UCHAR PSANS;									/* BEh */
	UCHAR Reserved18;								/* BFh */
	UCHAR DOGPOI[0xc3-0xc0];						/* C2h-C0h */
	UCHAR DOGPOII;									/* C3h */
	USHORT IDGPI;									/* C5h-C4h */
	UCHAR SMIRB;									/* C6h */
	UCHAR Reserved19;								/* C7h */
	UCHAR MMEAT;									/* C8h */
	UCHAR LRWACR;									/* C9h */
	UCHAR WBP;										/* CAh */
	UCHAR Reserved20;								/* CBh */
	USHORT DCC;										/* CDh-CCh */
	UCHAR Reserved21[0xd4-0xce];					/* D3h-CEh */
	UCHAR SMT;										/* D4h */
	UCHAR HSSI;										/* D5h */
	UCHAR HSSII;									/* D6h */
	UCHAR Reserved22;								/* D7h */
	UCHAR DSC;										/* D8h */
	UCHAR BESII;									/* D9h */
	UCHAR Reserved23[0xe0-0xda];					/* DFh-DAh */
	UCHAR SMBHSI;									/* E0h */
	UCHAR SMBHSC;									/* E1h */
	UCHAR SMBHCBC;									/* E2h */
	UCHAR Reserved24[0x100-0xe3];					/* FFh-E3h */
} M7101_PMU_CONFIG,*PM7101_PMU_CONFIG;
#endif
#if M1535C
typedef struct _M7101_PMU_CONFIG {
	USHORT VenderID;								/* 01h-00h */
	USHORT DeviceID;								/* 03h-02h */
	USHORT Command;									/* 05h-04h */
	USHORT Status;									/* 07h-06h */
	UCHAR RevisionID;								/* 08h	 */
	UCHAR ClassCode[0x0c-0x09];						/* 0Bh-09h */ 
	USHORT Reserved1;								/* 0Dh-0Ch */
	UCHAR HeaderType;								/* 0Eh	 */
	UCHAR Reserved2[0x2c-0x0f];						/* 2Bh-0Fh */
	USHORT SubsystemVendorID;						/* 2Dh-2Ch */
	USHORT SubsystemDeviceID;						/* 2Fh-2Eh */
	UCHAR Reserved3[0x34-0x30];						/* 33h-30h */
	UCHAR CAPPTR;									/* 34h */
	UCHAR Reserved4[0x40-0x35];						/* 3Fh-35h */
	USHORT SMI_EN1;                                 /* 41h-40h */ 
	USHORT SMI_STS1;								/* 43h-42h */ 
	ULONG SMI_EN2;									/* 47h-44h */ 
	ULONG SMI_STS2;									/* 4Bh-48h */ 
	UCHAR SMI_EN3;									/* 4Ch */
	UCHAR SMI_STS3;									/* 4Dh */
	UCHAR SMI_EN4[0x51-0x4e];						/* 50h-4Eh */ 
	UCHAR SMI_STS4[0x54-0x51];						/* 53h-51h */ 
	UCHAR S_TIMER;									/* 54h */
	UCHAR APM_A;									/* 55h */
	UCHAR APM_B;									/* 56h */
	UCHAR Reserved5[0x59-0x57];						/* 58h-57h */
	UCHAR SYS_MOT[0x5c-0x59];						/* 5Bh-59h */ 
	UCHAR Reserved6[0x60-0x5c];						/* 5Fh-5Ch */
	USHORT DEV_MOT;									/* 61h-60h */ 
	UCHAR BREAK1;									/* 62h */
	UCHAR DISP_TMR;                                 /* 63h */
	USHORT DISP_MOT;                                /* 65h-64h */ 
	UCHAR Reserved7[0x68-0x66];						/* 67h-66h */
	UCHAR DSMI_MOT[0x6b-0x68];						/* 6Ah-68h */ 
	UCHAR ENT_DEF1;                                 /* 6Bh */
	ULONG ENT_DEF2;									/* 6Fh-6Ch */ 
	USHORT ENT_DEF3;								/* 71h-70h */ 
	USHORT ENT_DEF4;								/* 73h-72h */ 
	UCHAR SYS_WAKE;									/* 74h */
	UCHAR PCI_ACT[2];								/* 76h-75h */ 
	UCHAR SMI_CNTL;									/* 77h */
	USHORT PLL_TMR;									/* 79h-78h */ 
	USHORT CPU_CNTL;								/* 7Bh-7Ah */ 
	UCHAR BREAK2[0x7f-0x7c];						/* 7Eh-7Ch */ 
	UCHAR BURST_MOD;								/* 7Fh */
	UCHAR ACPI_TEST;								/* 80h */
	UCHAR AUTO_DUTY;								/* 81h */
	UCHAR CON_PME;									/* 82h */
	UCHAR CON_THRM;									/* 83h */
	UCHAR RSM_BLK1[4];								/* 87h-84h */ 
	UCHAR RSM_BLK2;									/* 88h */
	UCHAR MULTI_BLK1;								/* 89h */
	UCHAR GPO_BLK1;									/* 8Ah */
	UCHAR RUN_BLK1;									/* 8Bh */
	ULONG RUN_BLK2;									/* 8Fh-8Ch */ 
	ULONG RUN_BLK3;									/* 93h-90h */ 
	UCHAR Reserved8[0x97-0x94];						/* 96h-94h */
	UCHAR GPI_BLK1;									/* 97h */
	USHORT GPI_BLK2;								/* 99h-98h */ 
	USHORT MULTI_BLK2;								/* 9Bh-9Ah */ 
	UCHAR GPO_BLK2[0x9f-0x9c];						/* 9Eh-9Ch */
	UCHAR Reserved9[0xa0-0x9f];						/* 9Fh	 */
	UCHAR MEMGPA[0xa3-0xa0];						/* A2h-A0h */
	UCHAR AC_TMR;									/* A3h */
	UCHAR MEMGPB[0xa7-0xa4];						/* A6h-A4h */
	UCHAR MISC_CNTL1;								/* A7h	 */
	USHORT IOGPA;									/* A9h-A8h */
	USHORT IOGPB;									/* ABh-AAh */
	USHORT IOGPC;									/* ADh-ACh */
	USHORT IOGPD;									/* AFh-AEh */
	UCHAR LCD_CNTL;									/* B0h */
	UCHAR MISC_CNTL2;								/* B1h */
	UCHAR SPKR_CNTL;								/* B2h */
	UCHAR SUS_LED;									/* B3h */
	UCHAR LED_DEB;									/* B4h */
	UCHAR GPI_BLK3;									/* B5h */
	UCHAR GPI_BLK4;									/* B6h */
	UCHAR GPO_BLK3;									/* B7h */
	USHORT GPO_BLK4;								/* B9h-B8h */
	UCHAR GPO_BLK5;									/* BAh */
	UCHAR RSM_MULTI;								/* BBh */
	UCHAR RICTR;									/* BCh */
	UCHAR Reserved10[0xbf-0xbd];					/* BEh-BDh */
	UCHAR CFG_RSM;									/* BFh */
	UCHAR CFG_RSM8;									/* C0h */
	UCHAR SMI_RSM8;									/* C1h */
	UCHAR GPI_BLK5;									/* C2h */
	UCHAR GPO_BLK6;									/* C3h */
	UCHAR MULTI_BLK4;								/* C4h  */
	UCHAR RES_MOT[0xc8-0xc5];						/* C7h-C5h */
	ULONG FAN_CNTL;									/* CBh-C8h */
	ULONG WAH_DOG1;									/* CFh-CCh */
	UCHAR THR_FAN;									/* D0h */
	UCHAR CFG_SET;									/* D1h */
	UCHAR CLK_RUN;									/* D2h */
	UCHAR WR_BEEP;									/* D3h */
	USHORT  DYNA_CTL;								/* D5h-D4h */
	UCHAR Reserved11[0xd8-0xd6];					/* D7h-D6h */
	UCHAR TEST_MOD;									/* D8h */
	UCHAR WAH_DOG2;									/* D9h */
	USHORT DELAY_ENT;								/* DBh-DAh */
	USHORT SHADOW;									/* DDh-DCh */
	UCHAR Reserved12[0xe0-0xde];					/* DFh-Deh */
	USHORT ACPI_BASE;								/* E1h-E0h */
	USHORT SBASMB;									/* E3h-E2h 	SMB_BASE */	
	UCHAR CAPID;									/* E4h */
	UCHAR NIP;										/* E5h */
	USHORT PMC;										/* E7h-E6h */
	USHORT PMCSR;									/* E9h-E8h */
	UCHAR PMCSR_BE;									/* EAh */
	UCHAR DATAR;									/* EBh */
	UCHAR Reserved13[0xee-0xec];					/* EDh-ECh */
	USHORT FS_TMR;									/* EFh-EEh */
	UCHAR SMBHSI;									/* F0h	     SMB_HOST */	
	UCHAR Reserved14[0xf2-0xf1];					/* F1h	 */
	UCHAR SMBHCBC;									/* F2h	 	SMB_IDLE */	
	UCHAR SMB_INSTS;								/* F3h */
	UCHAR Reserved15[0x100-0xf4];					/* FFh-F4h */
} M7101_PMU_CONFIG,*PM7101_PMU_CONFIG;

typedef struct _AUDREG {
	UCHAR DMAR[0x10-0x00];							/* 0x00 - 0x0f */
	UCHAR SBR[0x20-0x10];							/* 0x10 - 0x1f */
	UCHAR MPUR0;									/* 0x20        */
	UCHAR MPUR1;									/* 0x21        */
	UCHAR MPUR2;									/* 0x22        */
	UCHAR MPUR3;									/* 0x23        */
	UCHAR Reseved1[0x40-0x24];						/* 0x24 - 0x3F */
	ULONG ACWR;										/* 0x40 - 0x43 */
	USHORT RD;										/* 0x44 - 0x45 */
	USHORT AC;										/* 0x46 - 0x47 */
	ULONG SCTRL;									/* 0x48 - 0x4B */
	ULONG ACGPIO;									/* 0x4C - 0x4F */
	ULONG ASR0;										/* 0x50 - 0x53 */
	UCHAR ASR1;										/* 0x54        */
	UCHAR RSVD1;									/* 0x55        */
	UCHAR ASR2;										/* 0x56        */
	UCHAR RSVD2;									/* 0x57        */
	ULONG ASR3;										/* 0x58 - 0x5b */
	UCHAR ASR4;										/* 0x5c        */
	UCHAR RSVD3;									/* 0x5d        */
	UCHAR ASR5;										/* 0x5e        */
	UCHAR ASR6;										/* 0x5f        */
	ULONG AOPLSR;									/* 0x60 - 0x63 */
	UCHAR Reserved3[0x7c-0x64];						/* 0x7b - 0x64 */
	UCHAR GPI;										/* 0x7c        */
	UCHAR GPO;										/* 0x7d        */
	UCHAR GPCONTROL;								/* 0x7e        */
	UCHAR Reserved5;								/* 0x7f        */
	ULONG START;									/* 0x80 - 0x83 */
	ULONG STOP;										/* 0x84 - 0x87 */
	ULONG DLY;										/* 0x88 - 0x8b */
	ULONG SIGN_CSO;									/* 0x8c - 0x8f */
	ULONG CSPF;										/* 0x90 - 0x93 */
	ULONG CEBC;										/* 0x94 - 0x97 */
	ULONG AINT;										/* 0x98 - 0x9B */
	ULONG EINT;										/* 0x9c - 0x9F */
	ULONG GC_CIR;									/* 0xA0 - 0xA3 */
	ULONG AINTEN;									/* 0xA4 - 0xA7 */
	USHORT WAVEVOL;									/* 0xA8 - 0xA9 */
	USHORT MUSICVOL;								/* 0xAA - 0xAB */
	USHORT Reserved6;								/* 0xAC - 0xAD */
	USHORT SBDELTA;									/* 0xAE - 0xAF */
	ULONG MISCINT;									/* 0xB0 - 0xB3 */
	ULONG Reserved[0x100-0xb4];						/* 0xb4 - 0xff */
} AUDREG;

typedef AUDREG *PAUDREG;

#endif

typedef struct _M5237_USB_CONFIG {
	USHORT VenderID;								/* 01h-00h */
	USHORT DeviceID;								/* 03h-02h */
	USHORT Command;									/* 05h-04h */
	USHORT Status;									/* 07h-06h */
	UCHAR RevisionID;								/* 08h */
	UCHAR ClassCode[0x0c-0x09];						/* 0Bh-09h  */
	UCHAR CacheLineSize;							/* 0Ch CLS Cache Line Size R/W 00h */
	UCHAR LatencyTimer;								/* 0Dh LT Latency Timer R/W 00h */
	UCHAR HeaderType;								/* 0Eh */
	UCHAR Bist;										/* 0Fh BIST Built In Self Test R 00h */
	ULONG BaseAddress;								/* 13h-10h BA Base Address Register R/W 00000000h */
	UCHAR Reserved1[0x2c-0x14];						/* 2Bh-14h Reserved Reserved R 00h */
	USHORT SubsystemVendor;							/* 2Dh-2Ch SVID Subsystem Vendor ID R/W Lock 0000h */
	USHORT SubsystemDevice;							/* 2Fh-2Eh SDID Subsystem Device ID R/W Lock 0000h */
	UCHAR Reserved2[0x3c-0x30];						/* 3Bh-30h Reserved Reserved R 00h */
	UCHAR IRQLine;									/* 3Ch IL Interrupt Line R/W 00h */
	UCHAR IRQPin;									/* 3Dh IP Interrupt Pin R 01h */
	UCHAR MinGnt;									/* 3Eh MG Min_Gnt R 00h */
	UCHAR MaxGnt;									/* 3Fh ML Max_Lat R 50h */
	ULONG TestMode;									/* 43h-40h TM Test Mode R/W 00000000h */
	UCHAR Reserved3[0x100-0x44];					/* FFh-44h Reserved Reserved R 00h */
} M5237_USB_CONFIG, *PM5237_USB_CONFIG;

typedef union _M5229_BUS_MASTER_STATUS {
    struct {
        UCHAR BusMasterIDEActive:1; /* IDE Bus Master Active indicator */
        UCHAR IDEDMAError:1;        /* IDE DMA Error indicator */
        UCHAR IDEInterrupt:1;       /* IDE DMA Interrupt pending indicator */
        UCHAR Reserved:2;           /* Reserved (MBZ) */
        UCHAR Drive0DMACapable:1;   /* Drive 0 DMA Capability Status */
        UCHAR Drive1DMACapable:1;   /* Drive 1 DMA Capability Status */
        UCHAR SimplexStatus:1;      /* Channel Simplex status */
    } u;
    UCHAR AsUCHAR;
} M5229_BUS_MASTER_STATUS, *PM5229_BUS_MASTER_STATUS;

#define PRIMARY_BUS_MASTER_COMMAND      ((UCHAR)0x00)
#define PRIMARY_BUS_MASTER_STATUS       ((UCHAR)0x02)
#define SECONDARY_BUS_MASTER_COMMAND    ((UCHAR)0x08)
#define SECONDARY_BUS_MASTER_STATUS     ((UCHAR)0x0a)

typedef union _SMBSTATUS {
	struct {
		UCHAR hostSlaveBusy:1;	/* 0 (0b) HSTSLV_BSY, Host Slave Busy, '1' means that SMB slave interface is going to receive a command. (RO) */
		UCHAR hostSlaveStatus:1;/* 1 (0b) HSTSLV_STS, Host Slave Status, '1' means the interrupt (or SMI) was caused by the host SMB slave interface. */
		UCHAR idleStatus:1;		/* 2 (0b) IDL_STS, '1' means SMBus at Idle Status. (RO) */
		UCHAR hostBusy:1;		/* 3 (0b) HST_BSY, Host Controller Busy, '1' means that the SMB host controller is going to complete a command.(RO) */
		UCHAR smiStr:1;			/* 4 (0b) SMI_I_STS, '1' means that the Interrupt (or SMI) was caused by the SMB controller after completing a command. (RO) */
		UCHAR deviceErr:1;		/* 5 (0b) DEVICE_ERR, Device Error, '1' means the interrupt (or SMI) was caused by the SMB controller or device due to the generation of an error. */
		UCHAR collision:1;		/* 6 (0b) BUS_COLLI, Bus Collision, '1' means the interrupt (or SMI) was caused by the collision of bus transaction or no acknowledge. */
		UCHAR terminate:1;		/* 7 (0b) TERMINATE, '1' means the interrupt (or SMI) was caused by a terminated bus transaction in response to "ABORT". */
	} u;
	UCHAR AsUCHAR;
} SMBSTATUS;

typedef struct _SMBIOREG {
	SMBSTATUS smbSts;		/* SMBus Host/Slave Status Register */
	UCHAR smbCmd;			/* SMBus Host/Slave Command */
	UCHAR start;			/* I/O Port to Start to Generate the Programmed Cycle on the SMBus (STRT_PRT) */
	UCHAR address;			/* SMBus Address Register for Host Controller */
	UCHAR dataA;			/* SMBus DataA Register for Host Controller */
	UCHAR dataB;			/* SMBus DataB Register for Host Controller */
	UCHAR blockReg;			/* SMBus Block Register for Host Controller */
	UCHAR cmdReg;			/* SMBus Command Register for Host Controller */
} SMBIOREG;

typedef SMBIOREG *PSMBIOREG;

typedef struct _PMUIOREG {
	USHORT pm1_sts;				/* 01h-00h Power Management 1 Status Register R/W 0000h */
	USHORT pm1_en;				/* 03h-02h Power Management 1 Enable Register R/W 0000h */
	USHORT pm1_cntrl;			/* 05h-04h Power Management 1 Control Register R/W 1000h */
	USHORT Reserved1;			/* 07h-06h Reserved Reserved R 0000h */
	ULONG pm1_tmr ;				/* 0Bh-08h Power Management 1 Timer Register R xxh */
	ULONG Reserved2;			/* 0Fh-0Ch Reserved Reserved R 00h */
	ULONG p_cntrl;				/* 13h-10h Processor Control Register R/W 00000000h */
	UCHAR lvl2;					/* 14h Processor Level 2 Register R 00h */
	UCHAR lvl3;					/* 15h Processor Level 3 Register R 00h */
	USHORT Reserved3;			/* 17h-16h Reserved R 0000h */
	USHORT gpe0_sts;			/* 19h-18h General Purpose Event0 Status Register R/W 0000h */
	USHORT gpe0_en;				/* 1Bh-1Ah General Purpose Event0 Enable Register R/W 0000h */
	USHORT gpe1_sts;			/* 1Dh-1Ch General Purpose Event1 Status Register R/W 0000h */
	USHORT gpe1_en;				/* 1Fh-1Eh General Purpose Event1 Enable Register R/W 0000h */
	UCHAR gpe1_ctl[0x28-0x20];	/* 27h-20h General Purpose Event1 Control Register R/W 00h */
	UCHAR Reserved4[0x30-0x28];	/* 2Fh-28h Reserved Reserved R 00h */
	UCHAR pm2_cntrl;			/* 30h Power Management 2 Control Register R/W 00h */
	UCHAR Reserved[0x40-0x31];	/* 3Fh-31h Reserved Reserved R 00h */
} PMUIOREG;

typedef PMUIOREG *PPMUIOREG;


#endif /* __ACER_m1543c_H__ */
