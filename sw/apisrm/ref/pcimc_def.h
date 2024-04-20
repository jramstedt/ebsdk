/* File:	pcimc_def.h
 *
 * Copyright (C) 1995 by
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
 * Abstract:	Pci-Memory Channel Port Definitions 
 *
 * Author:	William Clemence
 *
 * Modifications:
 *
 *	wcc	12-Apr-1995	Rewrite from pcirm_def.h
*/


#define PCIMC_MAX_LINECARD 8

#define PCIMC_MAX_SLOT 8

/* CONSTANT DECLARATIONS
*/

/* Buffer size constants
*/
/* 	General program constants
*/
#define SIZE$K_512MB	((1024*1024)*512)
#define SIZE$K_128MB	((1024*1024)*128)
#define SIZE$K_64MB	((1024*1024)*64)
#define SIZE$K_11MB	(SIZE$K_1MB*11)
#define SIZE$K_10MB	(SIZE$K_1MB*10)
#define SIZE$K_9_5MB	((SIZE$K_1MB*9)+(SIZE$K_256K*2))
#define SIZE$K_9MB	(SIZE$K_1MB*9)
#define SIZE$K_8MB	(SIZE$K_1MB*8)
#define SIZE$K_1MB	((1024*1024))
#define SIZE$K_256K	(1024*256)
#define SIZE$K_64K	(1024*64)
#define SIZE$K_32K	(1024*32)
#define SIZE$K_16K	(1024*16)
#define SIZE$K_8K       (1024*8)
#define SIZE$K_4K       (1024*4)
#define SIZE$K_2K       (1024*2)
#define SIZE$K_1K       (1024)
#define SIZE$K_512      512
#define SIZE$K_256	256
#define SIZE$K_LW       4

#define PCT$K_PctSize  		SIZE$K_64K
#define PCT$K_PctInc		1
#define PCT$K_PctBase		0x00000000
#define PCT$K_AckPage		0x00000000
#define PCT$K_FirstPage		0
#define PCT$K_SecondPage	1
#define PCT$K_LastPage		SIZE$K_64K-1
					/* array index of LW */

/*	MC-Transmit MC-Receive Constants
*/
#define MCPage$K_FirstPage	0
#define MCPage$K_SecondPage	1
#define MCPage$K_NumHw		SIZE$K_256
#define MCPage$K_NumOw		SIZE$K_512
#define MCPage$K_NumQw		SIZE$K_1K
#define MCPage$K_NumLw		SIZE$K_2K
#define MCPage$K_NumBytes	SIZE$K_8K

/* fwrite() constants
*/
#define ObjSize$K_HW	       32
#define ObjSize$K_OW	       16
#define ObjSize$K_QW	       8
#define ObjSize$K_LW	       4
#define ObjSize$K_W	       2
#define ObjSize$K_B	       1

#define NumObj$K_1	       1
#define NumObj$K_2	       2
#define NumObj$K_4	       4
#define NumObj$K_8	       8
#define NumObj$K_16	       16
#define NumObj$K_32	       32
#define NumObj$K_16k	       SIZE$K_16K
#define NumObj$K_8k	       SIZE$K_8K
#define NumObj$K_2k	       SIZE$K_2K
#define NumObj$K_1k	       SIZE$K_1K


typedef unsigned long int PORT_ULONG;

/* PCIMC Configuration Register offsets
*/
#define PCIMC_CONFIG00	0x00		
#define PCIMC_CONFIG04	0x04		
#define PCIMC_CONFIG08	0x08
#define PCIMC_CONFIG0C	0x0C
#define PCIMC_CONFIG10	0x10
#define PCIMC_CONFIG2C	0x2C
#define PCIMC_CONFIG3C	0x3C

/* PCIMC Memory Mapped "Front Chip" Registers
*/
#define PCIMC_LCSR	0x40000
#define PCIMC_PRBAR	0x40008

/* PCIMC Memory Mapped "Back Chip" Registers
*/
#define PCIMC_MCERR	0x41000
#define PCIMC_MCPORT 	0x41004
#define PCIMC_MODCFG	0x41008
#define PCIMC_POS	0x4100C

/* Config00 bit Masks
*/
#define Cfg00h$M_DevID	0xFFFF0000
#define Cfg00h$M_VenID	0x0000FFFF

/* Config04 Device Status bits 
*/
#define Cfg04h$V_DetectedParErr	0x80000000
#define Cfg04h$V_SigSysErr	0x40000000
#define Cfg04h$V_RcvMstAbort	0x20000000
#define Cfg04h$V_RcvTgtAbort	0x10000000
#define Cfg04h$V_SigTgtAbort	0x08000000
#define Cfg04h$M_DevSelTim	0x06000000
#define Cfg04h$V_DatParDetected	0x01000000
#define Cfg04h$V_FstBtoB	0x00800000

/* Config04 Device Commands bits 
*/
#define Cfg04h$V_FstbTObEn	0x00000200
#define Cfg04h$V_SerrEn		0x00000100
#define Cfg04h$V_WaitCyCntl	0x00000080
#define Cfg04h$V_ParErrRsp	0x00000040
#define Cfg04h$V_VgaPalSnoop	0x00000020
#define Cfg04h$V_MemWrtInvEn	0x00000010
#define Cfg04h$V_SpecCycle	0x00000008
#define Cfg04h$V_BusMaster	0x00000004
#define Cfg04h$V_MemSpacEn	0x00000002
#define Cfg04h$V_IoSpacEn	0x00000001
#define Cfg04_Default		0xFFFF0000|Cfg04h$V_BusMaster|Cfg04h$V_MemSpacEn
#define Cfg04$M_WrtRd		0x00000140

/* Config08 bit Masks
*/
#define Cfg08h$M_BaseClass	0xFF000000
#define Cfg08h$M_SubClass	0x00FF0000
#define Cfg08h$M_RegLevel	0x0000FF00
#define Cfg08h$M_RevID		0x000000FF

/* Config0C bit Masks
*/
#define Cfg0ch$M_Bist		0xFF000000
#define Cfg0ch$M_HdrType	0x00FF0000
#define Cfg0ch$M_LatTimer	0x0000F800
#define Cfg0ch$M_CacheLinSz	0x000000FF
#define Cfg0c$M_WrtRd		0x0000F800

/* Config10 bit Masks/Vectors
*/
#define Cfg10h$M_XmtWinBase	0xE0000000
#define Cfg10h$V_Prefetch	0x00000008
#define Cfg10h$M_WinType	0x00000006
#define Cfg10h$V_MemSpace	0x00000001
#define Cfg10$M_WrtRd		0xF8000000

/* Config3C bit Masks
*/
#define Cfg3ch$M_MaxLat		0xFF000000
#define Cfg3ch$M_MinGnt		0x00FF0000
#define Cfg3ch$M_IntrPin	0x0000FF00
#define Cfg3ch$M_IntrLine	0x000000FF
#define Cfg3c$M_WrtRd		0x000000FF

#define PCT$M_RWCtrl		0xC0000000  /*MC2 read/write control bits */
#define PCT$V_XmtErrorGen	0x10000000  /*MC2 Tranmsmit Error Generation*/
#define PCT$V_RcvErrorGen	0x20000000  /*MC2 Receive Error Generation*/
#define PCT$M_ExtScGathAddr	0x001F0000  /*MC2 extended Sc/Gath addr bits*/
#define PCT$V_AACK		0x00008000
#define PCT$M_NodeId		0x00007E00
#define PCT$V_BrdCst		0x00000100
#define PCT$V_MCXmtEn		0x00000080
#define PCT$V_LoopBack		0x00000040
#define PCT$V_Ack		0x00000020
#define PCT$V_Stae		0x00000010
#define PCT$V_MCRcvEn		0x00000008
#define PCT$V_Intr		0x00000004
#define PCT$V_Srae		0x00000002
#define PCT$V_Par		0x00000001

#define PCT$D_DEFAULT		0x00000000 /* disables all PCT bits */

/* SGAT Mode is when PCT entry is used as SGAT entry for scatter/gather */
#define PCT$M_SGATMode		0x0000FFFE /*bits 15:1 used for addr mapping*/

/* LCSR bit vectors
*/
#define LCSR$M_Serial_id		0x07F80000 
#define LCSR$V_COMPLEMENT_CA_PAR	0x00040000	/* bit 18 */
#define LCSR$V_ZERO_DATA_PAR		0x00020000	/* bit 17 */
#define LCSR$V_ERROR_DISABLE		0x00010000	/* bit 16 */
#define LCSR$V_IntrSum			0x00008000	/* bit 15 */
#define LCSR$V_STATE_CHANGE_INTR	0x00004000	/* bit 14 */
#define LCSR$V_AlphaIntelMode		0x00002000	/* bit 13 */
#define LCSR$V_SCATTER_GATHER_EN  	0x00001000	/* bit 12 */
#define LCSR$V_PUT			0x00000800	/* bit 11 */
#define LCSR$V_XMTPEND			0x00000400	/* bit 10 */
#define LCSR$V_RCVPEND			0x00000200	/* bit 09 */
#define LCSR$V_RESET			0x00000100	/* bit 08 */
#define LCSR$V_MCINTRPND		0x00000080	/* bit 07 */
#define LCSR$V_STATE_CHANGE_INTR_EN	0x00000040	/* bit 06 */
#define LCSR$V_MCINTREN			0x00000020	/* bit 05 */
#define LCSR$V_XMTERRINTREN		0x00000010	/* bit 04 */
#define LCSR$V_RCVERRINTREN		0x00000008	/* bit 03 */
#define LCSR$V_VIRTUAL_HUB_CONFIG	0x00000004	/* bit 02 */
#define LCSR$V_RCVERR			0x00000002	/* bit 01 */
#define LCSR$V_XMTERR			0x00000001	/* bit 00 */
#define LCSR$M_WRTRD			0x00000028	/* mask   */


/* PRBAR bit vectors
*/
#define PRBAR$M_MCRcvBase 	0xF8000000
#define PRBAR$M_WrtRd		0xF8000000


/* MCPORT    
*/
#define MCPORT$V_rsvd 		0x0000FFFF
#define MCPORT$V_VirHubMode	0x80000000
#define MCPORT$V_LnkHubOk	0x40000000
#define MCPORT$V_LnkHubOnLine	0x20000000
#define MCPORT$V_ADAPTER_OK	0x10000000
#define MCPORT$V_OnLineEnable	0x08000000
#define MCPORT$V_HEARTBEAT_TIMEOUT_SELECT	0x04000000
#define MCPORT$M_HubType	0x01C00000
#define MCPORT$M_NodeID		0x003F0000

/* MODCFG
*/
#define MODCFG$M_rsvd		0xFFFFFF00
#define MODCFG$V_TurboModOvride	0x00000080
#define MODCFG$V_TurboMode	0x00000040
#define MODCFG$V_FiberModeJmpr	0x00000020
#define MODCFG$V_WinSzJmpOvride	0x00000010
#define MODCFG$V_PgSzJmpOvride	0x00000008
#define MODCFG$V_WindowSize	0x00000004
#define MODCFG$V_PageSize	0x00000002
#define MODCFG$V_CfgJmprOveride	0x00000001

/* MCERR    
*/
#define MCERR$V_XmtPctParErr	0x80000000  /* MC1 only */
#define MCERR$V_LinkFatalSummary	0x40000000
#define MCERR$V_HEARTBEAT_TIMEOUT	0x20000000
#define MCERR$V_XmtParErr	0x10000000
#define MCERR$V_HubError	0x08000000
#define MCERR$V_RcvFifoOv	0x04000000
#define MCERR$V_Rcv_CA_ParErr	0x02000000
#define MCERR$V_RcvDataParErr	0x01000000
#define MCERR$V_RcvVertParErr	0x00800000
#define MCERR$V_Rsvd1		0x00400000
#define MCERR$V_RcvPctParErr	0x00200000
#define MCERR$V_AckDropped	0x00100000
#define MCERR$V_RcvCmdFRamErr	0x00080000
#define MCERR$V_FATAL		0x00040000
#define MCERR$V_HEARTBEAT_TIMEOUT_EN 	0x00020000
#define MCERR$V_ParChkDis	0x00010000
#define MCERR$M_Undefined	0x0000FC00
#define MCERR$V_LnkRXCntlErrHis	0x00000200
#define MCERR$V_LnkRXEndPackTO	0x00000100
#define MCERR$V_LnkTXSupTooLong	0x00000080
#define MCERR$V_LnkCntlPacHbtTO	0x00000040
#define MCERR$V_LnkRcvFifoOvflw	0x00000020
#define MCERR$V_LnkPllOutLckSta	0x00000010
#define MCERR$V_LnkTXError	0x00000008
#define MCERR$V_RemoteRXChanErr	0x00000004
#define MCERR$V_RcvErrOnDataPak	0x00000002
#define MCERR$V_RcvErrOnHeader	0x00000001

/* POS    
*/
#define POS$M_PortOnlineState	0x0000FFFF   /* nodes 15:0 */
#define POS$M_undefined		0xFFFF0000 

/* Communication link definitions. 
*/
#define PCIMC_COMM$V_LCSR	0x00000001   /* report lcsr */
#define PCIMC_COMM$V_MCERR	0x00000002   /* report mcerr */

/* Read and Write definitions
*/

#define LCSR_READ(c)    (*c->csr_read)(c, c->lcsr)
#define MCERR_READ(c)   (*c->csr_read)(c, c->mcerr)
#define MCPORT_READ(c)  (*c->csr_read)(c, c->mcport)
#define PRBAR_READ(c)   (*c->csr_read)(c, c->prbar)
#define MODCFG_READ(c)  (*c->csr_read)(c, c->modcfg)
#define POS_READ(c)  	(*c->csr_read)(c, c->pos)
#define PCT_READ(c,o)   (*c->csr_read)(c, o)

#define LCSR_WRITE(c,d)    (*c->csr_write)(c, c->lcsr, d)
#define MCERR_WRITE(c,d)   (*c->csr_write)(c, c->mcerr, d)
#define MCPORT_WRITE(c,d)  (*c->csr_write)(c, c->mcport, d)
#define PRBAR_WRITE(c,d)   (*c->csr_write)(c, c->prbar, d)
#define MODCFG_WRITE(c,d)  (*c->csr_write)(c, c->modcfg, d)
#define POS_WRITE(c,d)     (*c->csr_write)(c, c->pos, d)
#define PCT_WRITE(c,o,d)   (*c->csr_write)(c, o, d)

/* Callback message received
*/
#define PCIMC_MAX_RCV_MSGS      10
#define PCIMC_CALLBACK_TIMEOUT 100	/*100 * 1/100 of a second*/
#define PCIMC_CALLBACK_WAIT     10	/*10 milliseconds = 1/100 second*/

/* Pci-Memory Channel Structure:
/*
/* The Pct Table is mapped to first 8/32 MC Pages.
/* The MC/Pct Table are mapped to Pci Dense memory and therefore LW addressable.
/* Pct Entries are of size LW and are LW aligned, with data bits<31:16> rsvd
/* Each Pct Entry point to an 8kb MC Page
/*
/* For an 512mb MC Memory there are:
/*   64klw Pct entries * 8kb == 512mb MC 
/* For a 512mb MC Memory the first:
/*   32 * 8kb MC pages are reserved for the Pct Table == 256kb
/*
/* For an 128mb MC Memory there are:
/*   16klw Pct entries * 8kb == 128mb MC
/* For a 128mb MC Memory the first:
/*    8 * 8kb MC pages are reserved for the Pct Table ==  64kb
/*
/* WE will allways size it for a 512mb memory structure.  Even if we only
/* use the first 128.
/*
/* PCT is described in Longwords
*/ 
struct PCT {
    unsigned int pct[SIZE$K_64K];	/* PCT'S == 32 pages */
};

/* Pci-Memory Channel Page Structure described in HexiWords
/*	(256 * HW) == 2048 LW == 8192 Bytes
*/
struct MCPage {
    unsigned int el[256][8];
};



/* MC-Exerciser MC-Page
*/
struct MCPgFmt {
    unsigned int Data[2044];
    unsigned __int64 ChkSum;

    union {
	unsigned int CntlLw;
	struct {
	    unsigned char Dir;
	    unsigned char Done;
	    unsigned char Start;
	    unsigned char Stop;
	} CntlByte;
    } Cntl;

    union {
	unsigned int MiscLw;
	struct {
	    unsigned short PageNum;
	    unsigned char Misc1;
	    unsigned char Cmd;
	} MiscByte;
    } Misc;
};


/* MC-Exerciser MC-Ack-Page
*/
struct MCAckPgFmt {
    struct {
	unsigned char ack[48];
	unsigned int PgAddr;
	unsigned int data;
	unsigned __int64 ChkSum;
    } ack[16];
};



struct PCIMCQ {
    struct PCIMCQ *flink;
    struct PCIMCQ *blink;
    struct TIMERQ tqe;
    } ;


/* Pci-Memory Channel Port Block Structure */
volatile struct PCIMC_PB {

/* Fixed Port Block structure for all devices   See pb_def.h for details. 
 * When created by probe_io(), pb is only (sizeof(struct pb)), which is 
 * common to all devices created.  It is reallocated() to contain the
 * additional Pci-mc data requirements.  Then put it back into pbs[i] for
 * all to enjoy. realloc(), returns a pointer to a block that's smaller
 * or larger than the original block after copying the contents of the
 * original block to the new one.  See DYNAMIC.C */
  struct pb pb;

/* Various Pci-mc Memory counters */
unsigned int mcrcverrcnt, 		/* MC-Receive Error Counter */
  mcxmterrcnt, 				/* MC-Transmit Error Counter */

  mcrcvintrcnt, 			/* Rcv Interrupt counter */
  Comm;					/* Communication link to lower
					 * functions. */

/* Miscallanious Variables */
unsigned int PtdrEnMask, 		/* PTDR register Enable mask  */
  PtdrDisMask;				/* PTDR register Disable mask */

unsigned int pcimc_HubType,
  pcimc_NodeID, 			/* Node ID, Contents of ptdr<30:25> */
  pcimc_mc2_mode,                       /* MC2 mode flag (0=MC1, 1=MC2) */
  pcimc_size, 				/* The size of the MC adapter 128 or
					 * 512 mb */
  page_size,				/* Size of MC Page (4KB or 8KB) */
  PgOffset,				/* offset = SIZE$K_8K/page_size */
  Lcsr_data, 				/* LCSR register data holder */
  mcerr_data, 				/* MCERR register data holder */
  pcimc_state, 				/* Node state on-line or offline */
  mcstachgintr, 			/* state change interrupt value */
  mcintr_report,			/* interrupt report */
  mcintr_report_data,			/* interrupt report */
  mcpolled_mode,			/* Indicates we can't intterupt. */
  state;				/* Driver state */

/* MC-Memory pointers */
unsigned int MCMalloc, MCTstPage, MCNumPages, MCXmtSize, MCRcvSize;


struct PCT *DmemBase; 			/* Mapped to Pci Dense Memory, used in 
					 * in/outdmem().. */

char name[32], 				/* Pci-Memory Channel Port Name */
     short_name[32];			/* Pci-Memory Channel Port Name (Short 
					 * version) */

/* Pci-MC Memory Mapped CSR Addresses */
unsigned int

/* Front Chip Decode */
  lcsr,
  prbar, 

/* Back Chip Decode */
  mcerr,
  mcport,
  modcfg,
  pos;

/* CSR Read and Write Routine Access */
  int (*csr_read)();
  int (*csr_write)();

/* Que of pct interrupts for each process... */
  struct PCIMCQ pctintr;

};

/* 
 * VPD defines
 */
#define VPD_SECTOR_SIZE   65536 /* size of sector containing VPD and 
 				 * associated structures */
#define VPD_SECTOR_END    65535 /* last offset in the sector */
#define VPD_SIZE          86    /* # of bytes of actual VPD data */

/* VPD offsets from start of sector */
#define VPD_PCI_TAG       0x080
#define VPD_START         0x100
#define PN_FIELD          0x117
#define FN_FIELD          0x125
#define EC_FIELD          0x130
#define MN_FIELD          0x136
#define SN_FIELD          0x13C
#define RM_FIELD      	  0x149
#define LL_FIELD      	  0x150
#define VPD_CHKSUM	  0x155 

/* VPD Keyword Lengths */
#define PN_LEN	11
#define FN_LEN	8
#define EC_LEN	3
#define MN_LEN	3
#define SN_LEN	10
#define RM_LEN	4
#define LL_LEN	4

/* Return values for Data Polling algorithm */
#define msg_write_success 1
#define msg_write_failure 0

#define MC2_READ_ROM(mcpb, a) inmemb(mcpb, (RomBaseAddr | ((~(a)) & 0x7fffc)) | ((a) & 0x3))
#define MC2_WRITE_ROM(mcpb, a, d) outmeml(mcpb, (RomBaseAddr | ((a)<<2)), d & 0xFF)

/* AMD Chip Data Bits */
#define DATA$V_Sector_Erase_Timer 0x08	/* bit 3 */
#define DATA$V_Exceeded_Timing	  0x20	/* bit 5 */
#define DATA$V_Toggle_Bit	  0x40	/* bit 6 */
#define DATA$V_Data_Polling	  0x80	/* bit 7 */
