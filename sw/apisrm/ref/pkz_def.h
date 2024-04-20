/*
 * Copyright (C) 1991 by
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
 * Abstract:	Definitions for XZA N_Port SCSI Adapter
 *
 * Author:	Thomas G. Arnold
 *
 * Modifications:
 *
 *      wcc     23-Feb-1993     Bug 870: removed LFU only stuff. It is now
 *                              incorperated in LFU_KZMSA_DRIVER.c
 *
 *	kl	01-Sep-1992	LFU update support
 *	tga	18-Apr-1991	Initial entry.
 *	tga	29-Aug-1991	Remove use of NRE register, and shift other
 *				registers down to avoid bb+110.  This bypasses
 *				a Gate Array bug.
 */
 
typedef struct z_intr_data {
  unsigned int slot   : 4;	/* XMI node ID to send interrupts to */
  unsigned int vector : 10;	/* Interrupt vector */
  unsigned int ipl    : 2;	/* Interrupt priority level */
  unsigned int altvec : 16;	/* Must Be Zero / Alternate vector for VAX */
} Z_INTR_DATA;

typedef volatile struct n_ab_rsvd {/*** Interrupt data in Adapter Block ***/
  Z_INTR_DATA c0_misc;		/* Channel 0 Miscellaneous Interrupt */
  Z_INTR_DATA c1_misc;		/* Channel 1 Miscellaneous Interrupt */
  Z_INTR_DATA adrq_intr;	/* Adapter Driver Response Queue Interrupt */
  char rsvd[20];		/* Reserved */
} N_AB_RSVD;

typedef volatile struct cqib {		/*** Command Queue Insertion Block ***/
  struct {
    unsigned long int ccq2ir;
    unsigned long int ccq1ir;
    unsigned long int ccq0ir;
  } cq[2];
  unsigned long int afqir;
} CQIB;

#define XZA_IPL 1 /* IPL 15 */

#define SCSI_NODES 8

#define XMI$K_DTYPE_XZA 0x0c36
#define XZA_CHANNELS 2
#define XZA_DRIVERFREE 16 
#define XZA_ADAPTERFREE 16
#define XZA_BUFFERS ( XZA_DRIVERFREE + XZA_ADAPTERFREE )
#define XZA_NUM_QUEUES (8 * XZA_CHANNELS) + 2 + 7
#define XZA$K_IMAGE_SIZE 64512

#define N_CARRIER_POOL_SZ ( sizeof( struct n_carrier ) * ( XZA_BUFFERS + XZA_NUM_QUEUES) )
#define N_Q_BUFFER_POOL_SZ ( sizeof( struct n_q_buffer ) * XZA_BUFFERS )
#define SB_POOL_SZ ( sizeof( struct scsi_sb ) * SCSI_NODES * XZA_CHANNELS )
#define SB_PTR_POOL_SZ ( sizeof( struct scsi_sb *) * SCSI_NODES * XZA_CHANNELS )
#define AMPB_SIZE HWRPB$_PAGESIZE 

typedef enum adapter_state {Unknown, Uninitialized, Disabled, Enabled} 
   ADAPTER_STATE;

/* XZA Registers */
#define C0_MSK 1
#define C1_MSK 2
#define C0_AND_C1_MSK 3
typedef enum xza_regs {ASR, ASRCR, AICR, AECR, AMCSR, AFAR,
		       XDEV, XBE, XFADR, XAER, XCOMM, XPUD,
		       QIR, GCQ0IR, GCQ1IR, CBRCR}
   XZA_REGS;

typedef volatile struct z_xdev {
  unsigned short int dtype;
  unsigned char firm_rev;
  unsigned char hard_rev;
} Z_XDEV;

typedef volatile union z_xbe {
  unsigned int i;
  struct {
    unsigned int rsvd    : 1;	/* <0>   Reserved */
    unsigned int emp     : 1;	/* <1>   Enable MORE Protocol */
    unsigned int dxto    : 1;	/* <2>   Disable XMI Timeout */
    unsigned int notimp  : 1;	/* <3>   Not implemented */
    unsigned int fcid    : 6;	/* <4:9> Failing Commander ID */
    unsigned int stf     : 1;	/* <10>  Self-Test Failed */
    unsigned int notimp2 : 1;	/* <11>  Not implemented */
    unsigned int notimp3 : 1;	/* <12>  Not implemented */
    unsigned int tto     : 1;	/* <13>  Transaction Time Out */
    unsigned int rsvd2   : 1;	/* <14>  Reserved */
    unsigned int cnak    : 1;	/* <15>  Command No ACK */
    unsigned int rer     : 1;	/* <16>  Read Error Response */
    unsigned int rse     : 1;	/* <17>  Read Sequence Error */
    unsigned int nrr     : 1;	/* <18>  No Read Response */
    unsigned int crd     : 1;	/* <19>  Corrected Read Data */
    unsigned int wdnak   : 1;	/* <20>  Write Data No ACK */
    unsigned int ridnak  : 1;	/* <21>  Read / Ident Data No ACK */
    unsigned int wse     : 1;	/* <22>  Write Sequence Error */
    unsigned int pe      : 1;	/* <23>  Parity Error */
    unsigned int notimp4 : 1;	/* <24>  Not implemented */
    unsigned int notimp5 : 1;	/* <25>  Not implemented */
    unsigned int notimp6 : 1;	/* <26>  Not implemented */
    unsigned int cc      : 1;	/* <27>  Corrected confirmation */
    unsigned int notimp7 : 1;	/* <28>  Not implemented */
    unsigned int nhalt   : 1;	/* <29>  Node Halt */
    unsigned int nrst    : 1;	/* <30>  Node Reset */
    unsigned int es      : 1;	/* <31>  Error Summary */
  } f;
} Z_XBE;

typedef volatile struct z_xfadr {
  unsigned int fadr : 30;	/* Failing address */
  unsigned int fln  : 2;	/* Failing length */
} Z_XFADR;

typedef volatile struct z_xfaer {
  unsigned int mask    : 16;	/* mask */
  unsigned int addrext : 10;	/* Address extension */
  unsigned int mbz     : 2;	/* Must Be Zero */
  unsigned int fcmd    : 4;	/* Failing Command */
} Z_XFAER;

typedef volatile struct z_xcomm {
  unsigned char i_chr;		/* Incoming character */
  unsigned int  i_nodeid : 4;	/* Incoming node ID */
  unsigned int  mbz      : 3;	/* Must Be Zero */
  unsigned int  i_busy   : 1;	/* Incoming Busy */
  unsigned char o_chr;		/* Outgoing character */
  unsigned int  o_nodeid : 4;	/* Outgoing node ID */
  unsigned int  mbz3     : 3;	/* Must Be Zero */
  unsigned int  o_busy   : 1;	/* Outgoing Busy */
} Z_XCOMM;

typedef struct {
  unsigned int       : 4;
  unsigned int v13_4 : 10;
  unsigned int       : 18;
} VECTOR;

typedef volatile union z_xpd1 {
  unsigned i;
  struct {
    unsigned char rcode;	/* register code */
    unsigned c0 : 1;		/* channel 0 operation */
    unsigned c1 : 1;		/* channel 1 operation */
    unsigned rsvd : 3;		/* reserved */
    unsigned pax : 3;		/* physical address extension <39:37> */
    unsigned slot : 4;		/* XMI node for miscellaneous interrupt */
    unsigned vector : 10;	/* vector <13:4> for miscellaneous interrupt */
    unsigned ipl : 2;		/* ipl for miscellaneous interrupt */
  } f;
} Z_XPD1;

typedef unsigned int Z_XPD2;	/* physical address bits <36:5> */

typedef unsigned int Z_XPST;	

typedef volatile union z_xpud {
  unsigned int i;
  struct {
    unsigned int            : 1;	/* Not used */
    unsigned int            : 1;	/* Not used */
    unsigned int eepromfl   : 1;	/* EEPROM Firmware Loaded */
    unsigned int epromfl    : 1;	/* EPROM Firmware Loaded */
    unsigned int diagpbad   : 1;	/* Diagnostic Patch Table invalid */
    unsigned int errhist    : 1;	/* Error history exists */
    unsigned int            : 3;	/* Not used */
    unsigned int unin       : 1;	/* Uninitalized */
    unsigned int            : 2;	/* Not used */
    unsigned int siop0valid : 1;	/* SIOP0 Valid ID */
    unsigned int siop1valid : 1;	/* SIOP1 Valide ID */
    unsigned int xnagaok    : 1;	/* XNA Gate Array Test passed */
    unsigned int siop0ok    : 1;	/* SIOP0 Test passed */
    unsigned int siop1ok    : 1;	/* SIOP1 Test passed */
    unsigned int shrrampok  : 1;	/* Shared RAM parity test passed */
    unsigned int shrrammok  : 1;	/* Shared RAM march test passed */
    unsigned int xnadaltok  : 1;	/* XNA DAL Timeout test passed */
    unsigned int xnadalrok  : 1;	/* XNA DAL Readback test passed */
    unsigned int eepromok   : 1;	/* EEPROM test passed */
    unsigned int            : 1;	/* Unused */
    unsigned int cvaxok     : 1;	/* CVAX test passed */
    unsigned int cvaxpramok : 1;	/* CVAX RAM parity test passed */
    unsigned int cvaxmramok : 1;	/* CVAX RAM march test passed */
    unsigned int condrivrok : 1;	/* Console driver test passed */
    unsigned int sscok      : 1;	/* SSC Test passed */
    unsigned int diagregok  : 1;	/* Diagnostic register test passed */
    unsigned int cvaxintrok : 1;	/* CVAX Interrupt lines test passed */
    unsigned int bootromok  : 1;	/* Boot ROM test passed */
    unsigned int stc        : 1;	/* Self-Test Complete */
  } f;
} Z_XPUD;

typedef unsigned int Z_XPCI;

typedef unsigned int Z_XPCP;

typedef unsigned int Z_XPCS;

typedef unsigned int Z_XPRR;

typedef volatile struct xza_registers {
  Z_XDEV xdev;			/* 0x00 XMI Device Type Register */
  Z_XBE xbe;		       	/* 0x04 XMI Bus Error Register */
  Z_XFADR xfadr;		/* 0x08 XMI Failing Address Register */
  unsigned int pad;		/* 0x0C */
  Z_XCOMM xcomm;    		/* 0x10 XMI Communications Register */
  unsigned int pad2[6];		/* 0x14 */
  Z_XFAER xfaer;		/* 0x2c XMI Failing Address Extension Reg */
  unsigned int pad3[52];	/* 0x30 */
  Z_XPD1 xpd1;			/* 0x100 Port Data Register 1 */
  Z_XPD2 xpd2;			/* 0x104 Port Data Register 2 */
  Z_XPST afar0;			/* 0x108 Adapter Failing Address Register 0 */
  Z_XPUD xpud;			/* 0x10c Power-Up Diag. / Adapter Status Reg */
  unsigned int pad4;		/* 0x110 bypass reg for GA bug */
  Z_XPCI gcq0ir;		/* 0x114 Channel 0 "greased" cmd Q insert */
  Z_XPCP gcq1ir;		/* 0x118 Channel 0 "greased" cmd Q insert */
  Z_XPCS qir;			/* 0x11c Queue Entry Insertion register */
} XZA_REGISTERS;

/*
 * XZA N_Port Adapter Registers 
 */
typedef volatile struct z_asr {	/* Adapter Status Register */
  unsigned int afqe  : 1;	/* 0 -   Adapter Free Queue Exhausted */
  unsigned int       : 1;	/* 1 -   Should Be Zero */
  unsigned int dse   : 1;	/* 2 -   Data Structure Error */
  unsigned int       : 1;	/* 3 -   Should Be Zero */
  unsigned int ac    : 1;	/* 4 -   Abnormal Condition */
  unsigned int       : 1;	/* 5 -   Should Be Zero */
  unsigned int cic   : 1;	/* 6 -   Channel Initialize Complete */
  unsigned int cec   : 1;	/* 7 -   Channel Enable Complete */
  unsigned int       : 1;	/* 8 -   Should Be Zero */
  unsigned int unin  : 1;	/* 9 -   Uninitialized */
  unsigned int       : 4;	/* 10-13 Should Be Zero */
  unsigned int c0    : 1;	/* 14 -  Channel 0 */
  unsigned int c1    : 1;	/* 15 -  Channel 1 */
  unsigned int adsec : 8;	/* 16-23 Adapter Data Structure Error Code */
  unsigned int acc   : 7;	/* 24-30 Abnormal Condition Error Code */
  unsigned int ame   : 1;	/* 31 -  Adapter Maintenance Error */
} Z_ASR;

/*
 * Page manipulation constants 
 */
#define HEX_INC    0x20
#define HEX_MASK   0x1f
#define PAGE_MASK (HWRPB$_PAGESIZE-1)
#define PAGE_INC  HWRPB$_PAGESIZE 
#define PAGE_SIZE HWRPB$_PAGESIZE

/*
 * XZA Buffer Definitions
 * Note that these are definitions of the "B" area of the N_Q_BUFFER
 * structure.
 */

/* XZA Specific Opcodes.  Note that the N_Port generic Opcodes are defined */
/* in the enumeration "N_OPC" in nport_def.h; thus, these opcodes are */
/* defined in addition for the N_Q_BUF_HEAD.opc field. */
enum XZA_OPC {
  SETDIAG=8, DIAGSET=8,	/* Send Diagnostic Command */
  SETNEX=9,  NEXSET=9};	/* Set Nexus */

/* XZA SCSI Specific Opcodes.  These codes are defined for the */
/* N_Q_BUF_HEAD.i_opc field in the command queue header. */
enum SCSI_OPC {
  SNDSCSI=0x40, SCSISNT=0x40,	/* Send SCSI Command */
  SNDVNDR=0x41, VNDRSNT=0x41,	/* Send Vendor-Unique Command */
  ABRTCMD=0x42, CMDABRT=0x42,	/* Abort Command */
  MRKCMD=0x44,  CMDMRK=0x44};	/* Mark Command1 */

enum XZA_DIAG_OPC {
  EE$WRITEPARAM=0x80,
  EE$READPARAM=0x81,
  EE$WRITEIMAGE=0x84,
  EE$READIMAGE,
  EE$WRITEEEPROM,
  EE$READEEPROM
};

#define SEMAPHORE_MAGIC 0xf1e2d3c4
#define MAX_RETRIES 3

/* Driver Context */
typedef volatile struct drvr_cntx {
  struct SEMAPHORE *caller_s;	/* Caller wait semaphore */
  int              *caller_f;	/* Caller interrupt pending flag */
  unsigned int      magic;	/* Magic number to help insure sem is valid */
  N_CARRIER	   *car;	/* Pointer to current carrier */
  int               retries;	/* Number of retries command has experienced */
} DRVR_CNTX;

/* Number of bytes in an XZA_Port Queue Buffer Body (section B) */
#define XZA_Q_BODY_SIZ ( DFQE_SIZE - ( N_Q_HEAD_SIZ + sizeof( DRVR_CNTX ) ) )
			
/* XZA_Port Queue Buffer */
/* This generic form of the buffer is used to access the driver context */
typedef struct xza_q_buffer {	
  N_Q_BUF_HEAD a;		/* Generic command header */
  unsigned char b[XZA_Q_BODY_SIZ];/* Command specific area */
  DRVR_CNTX drvr_cntx;		/* Driver context */
} XZA_Q_BUFFER;

/* Queues we recognize */

enum XZA_QUEUES {DCCQ2, DCCQ1, DCCQ0, DAFQ, DDFQ, ADRQ, RESETQ, RESETW};

/* SCSI_N_Port command flags */
typedef volatile union scsi_n_flags {
  unsigned short int i;		/* Flags as a short integer (16 bits ) */
  struct {
    unsigned int  r    : 1;	/* Generate response */
    unsigned int  i    : 1;	/* Suppress interrupts */
    unsigned int  ca   : 1;	/* Count events for all addresses */
    unsigned int       : 1;	/* Should Be Zero */
    unsigned int  sw   : 4;	/* Reserved for software */
    unsigned int       : 6;	/* Should Be Zero */
    unsigned int  nike : 1;	/* Next In Kommand Execution */
    unsigned int       : 1;	/* Should Be Zero */
  }               f;		/* Flags as bit fields (16 bits) */
} SCSI_N_FLAGS;

/* SCSI command flags */
enum SCSI_DIR {HostToSCSI=0, SCSIToHost=1};
typedef union scsi_flags {
  unsigned char  i;		/* Flags as a char (8 bits ) */
  struct {
    unsigned int rd     : 1;	/* 0 - HostToSCSI; 1 - SCSIToHost */
    unsigned int tq     : 1;	/* Tagged Queue command */
    unsigned int as     : 1;	/* Autosense enabled */
    unsigned int sdtr   : 1;	/* Synchronous Data Transfer Request */
    unsigned int disc   : 1;	/* Disconnet privilege enabled */
    unsigned int tiop   : 1;	/* Terminate I/O Process */
    unsigned int distry : 1;	/* Disable SCSI parity error retry */
    unsigned int        : 1;	/* Should Be Zero */
  }              f;		/* Flags as bit fields (8 bits) */
} SCSI_FLAGS;

/* SCSI Xport Address */
typedef union scsi_xport {
  unsigned short i;
  struct {
    unsigned char scsi_id;	/* SCSI Node ID */
    unsigned char scsi_lun;	/* SCSI Logical Unit Number */
  } f;
} SCSI_XPORT;

/* SCSI status definition.  Note that this defines more files of STATUS_TYPE */
enum SCSI_STATUS_TYPE {
  SSR=4,		/* SCSI Status Returned in bit 15:8 */
  INSUFNEXT=64,		/* Insufficient Nexus Resources */
  ABORTED=65,		/* Command Aborted in progress */
  STOPPED=66,		/* Command Aborted before execution */
  NOTFOUND=67,		/* Command not found */
  DEVRESET=68,		/* Device was reset */
  BUSRESET=69,		/* SCSI bus was reset */
  AUTOTRUNC=70,		/* Autosense data was truncated */
  CMDTIMO=71,		/* Command timed out */
  BUSWEDGED=72,		/* SCSI Bus Wedged */
  CMDWEDGED=73,		/* Command timeout / SCSI bus wedged */
  SCSIPARITY=74,	/* SCSI Bus Parity Error */
  ILLPHASE=75,		/* Illegal SCSI bus phase */
  ILLWEDGED=76,		/* Illegal phase / SCSI bus wedged */
  SELTIMO=77};		/* SCSI bus Selection Timeout */

typedef union scsi_status {
  unsigned short int i;		/* Status treated as a short integer */
  struct {
    unsigned int fail : 1;	/* Failure occured */
    unsigned int type : 7;	/* Error type */
    union {
      unsigned int scsi : 8;	/* SCSI-2 specific status */
      struct {
	unsigned int    : 4;
	unsigned int ca : 1;	/* Count all addresses */
	unsigned int    : 3;
      } c;
    } s;
  } f;				/* Status treated as bit fields (16 bits) */
} SCSI_STATUS;

/* Set SCSI Nexus */

/* Nexus Descriptor */
typedef struct z_nexus {
  unsigned int qfroz  : 1;	/* LUN Queue frozen */
  unsigned int qfull  : 1;	/* LUN Queue full / spilled to host memory */
  unsigned int        : 2;	/* Must Be Zero */
  unsigned int sync   : 1;	/* Synchronous data transfer enable */
  unsigned int bdr    : 1;	/* Bus Device Reset */
  unsigned int        : 23;	/* Must Be Zero */
  unsigned int qall   : 1;	/* LUN Queue Allocated */
  unsigned int qdea   : 1;	/* LUN Queue Deallocated */
  unsigned int tgt    : 1;	/* Target-wide function */
  unsigned int ph_chg_tmo;	/* Phase change Timeout (seconds) */
} Z_NEXUS;

typedef struct z_setnex {
  Z_NEXUS             mask;		/* Mask to modify VCD */
  Z_NEXUS             indt_value;	/* Value to set */
} Z_SETNEX;

typedef struct z_nexset {
  Z_NEXUS              mask;		/* Mask to modify VCD */
  Z_NEXUS              m_value;		/* Value to set */
  Z_NEXUS              in_nex_0;	/* Value prior to modification */
  unsigned short int next_res;
  unsigned char      rsvd[6];
  Z_NEXUS              in_nex_n[7];
} Z_NEXSET;

/* Send SCSI */

#define MAX_SGPS 1024
#define SNDSCSI_SGPS 20
#define MAINT_SGPS 10

typedef struct sg_ptr {
  unsigned int      pa     : 32;        /* Arbitrary physical address */
  unsigned int      pa_e   : 16;        /* Arbitrary physical address */
  unsigned int      length : 16;        /* Length of physical segment */
} SG_PTR;

typedef struct as_ptr {
  unsigned int             : 5;		/* Should Be Zero */
  unsigned int      pa     : 27;	/* Physical Address of AS buffer */
  unsigned int      pa_e   : 16;	/* Physical Address of AS buffer */
  unsigned int      as_len : 16;	/* Auto-Sense buffer length */
} AS_PTR;

typedef struct z_sndscsi {
  unsigned int       xct_id[2];		/* Transaction ID */
  unsigned int       xfer_len;		/* Data transfer length */
  unsigned int       xfer_offset;	/* Offset to start of data buffer */
  unsigned int       ret_len;		/* Returned transfer length */
  unsigned short int cdb_len;		/* Length of SCSI CDB */
  unsigned short int sbz;		/* Should Be Zero */
  TYPE_PTR	     root_ptr[2];	/* Data buffer root pointers 0 & 1 */
  unsigned char      scsi_cdb[14];	/* SCSI Command Descriptor Block */
  unsigned short int tag_msg;		/* SCSI Queue Tag Message code byte */
  AS_PTR             as;		/* Autosense buf phy addr and length */
  unsigned long int  cmd_tmo;		/* Command Timeout (seconds) */
  unsigned char      pad[36];		/* Pad to 32 byte boundary */
  SG_PTR             sg[SNDSCSI_SGPS];  /* Array of Scatter/Gather pointers */
} Z_SNDSCSI;

/* Send Vendor */

typedef struct cdb_ptr {
  unsigned int              : 5;	/* Should Be Zero */
  unsigned int      pa      : 27;	/* Physical Address of CDB buffer */
  unsigned int      pa_e    : 16;	/* Physical Address of CDB buffer */
  unsigned int      tag_msg : 16;	/* Tagged Queue code */
} CDB_PTR;

typedef struct z_sndvndr {
  unsigned int       xci_id[2];		/* Transaction ID */
  unsigned int       xfer_len;		/* Data transfer length */
  unsigned int       xfer_offset;	/* Offset to start of data buffer */
   unsigned int       ret_len;		/* Returned transfer length */
  unsigned short int cdb_len;		/* Length of SCSI CDB */
  unsigned short int cdb_offset;	/* Offset into SCSI CDB */
  TYPE_PTR	     root_ptr[2];	/* Data buffer root pointers 0 & 1 */
  CDB_PTR	     cdb_ptr[2];	/* SCSI Cmd Descr Block pointers */
  AS_PTR             as;		/* Autosense buf phy addr and length */
  unsigned long int  cmd_tmo;		/* Command Timeout (seconds) */
} Z_SNDVNDR;

/* Zen Port Read Counters Command */

typedef struct z_rdcnt {
  unsigned int sndscsi_cmd_cnt;		/* Send SCSI command count */
  unsigned int sndvndr_cmd_cnt;		/* Send Vendor command count */
  unsigned int abrtcmd_cmd_cnt;		/* Abort Command command count */
  unsigned int mrkabrt_cmd_cnt;		/* Mark Abort command count */
  unsigned int bdrst_cmd_cnt;		/* Bus Device Reset command count */
  unsigned int purgq_cmd_cnt;		/* Purge Queue command count */
  unsigned int setnex_cmd_cnt;		/* Set Nexus command count */
  unsigned int rdcnt_cmd_cnt;		/* Read Counters command count */
  unsigned int direct_cmd_cnt;		/* Direct command count */
  unsigned int read_cmd_cnt;		/* Read-class command count */
  unsigned int write_cmd_cnt;		/* Write-class command count */
  unsigned int inquiry_rcvd_cnt;	/* Inquiries Received count */
  struct {
    unsigned int read_data;		/* Target Read data count */
    unsigned int pad0;
    unsigned int write_data;		/* Target Write data count */
    unsigned int pad1;
  } scsi_tgt[8];
  unsigned int scsi_parity_cnt;		/* SCSI bus parity error count */
  unsigned int unsol_resel_cnt;		/* Unsolicited reselection count */
  unsigned int ill_phase_cnt;		/* Illegal phase count */
  unsigned int cmd_timo_cnt;		/* Command Timeout count */
  unsigned int sel_timo_cnt;		/* Selection Timeout count */
  unsigned int unexp_discon_cnt;	/* Unexpected Disconnect count */
  unsigned int bus_reset_snt_cnt;	/* Bus Reset Sent count */
  unsigned int bus_reset_rcv_cnt;	/* Bus Reset Received count */
  unsigned int bus_dev_reset_snt_cnt;	/* Bus Device Reset sent count */
  unsigned int bus_dev_reset_rcv_cnt;	/* Bus Device Reset received count */
} Z_RDCNT;

typedef struct z_cntrd {
  unsigned int sndscsi_cmd_cnt;		/* Send SCSI command count */
  unsigned int sndvndr_cmd_cnt;		/* Send Vendor command count */
  unsigned int abrtcmd_cmd_cnt;		/* Abort Command command count */
  unsigned int mrkabrt_cmd_cnt;		/* Mark Abort command count */
  unsigned int bdrst_cmd_cnt;		/* Bus Device Reset command count */
  unsigned int purgq_cmd_cnt;		/* Purge Queue command count */
  unsigned int setnex_cmd_cnt;		/* Set Nexus command count */
  unsigned int rdcnt_cmd_cnt;		/* Read Counters command count */
  unsigned int direct_cmd_cnt;		/* Direct command count */
  unsigned int read_cmd_cnt;		/* Read-class command count */
  unsigned int write_cmd_cnt;		/* Write-class command count */
  unsigned int inquiry_rcvd_cnt;	/* Inquiries Received count */
  struct {
    unsigned int read_data;		/* Target Read data count */
    unsigned int pad0;
    unsigned int write_data;		/* Target Write data count */
    unsigned int pad1;
  } scsi_tgt[8];
  unsigned int scsi_parity_cnt;		/* SCSI bus parity error count */
  unsigned int unsol_resel_cnt;		/* Unsolicited reselection count */
  unsigned int ill_phase_cnt;		/* Illegal phase count */
  unsigned int cmd_timo_cnt;		/* Command Timeout count */
  unsigned int sel_timo_cnt;		/* Selection Timeout count */
  unsigned int unexp_discon_cnt;	/* Unexpected Disconnect count */
  unsigned int bus_reset_snt_cnt;	/* Bus Reset Sent count */
  unsigned int bus_reset_rcv_cnt;	/* Bus Reset Received count */
  unsigned int bus_dev_reset_snt_cnt;	/* Bus Device Reset sent count */
  unsigned int bus_dev_reset_rcv_cnt;	/* Bus Device Reset received count */
} Z_CNTRD;

/* Zen Port Purge Queue Command */

typedef struct z_purgq {
  unsigned char lunq_mask;		/* Target lun queue mask */
} Z_PURGQ;

typedef struct z_qpurg {
  unsigned char lunq_mask;		/* Target lun queue mask */
} Z_QPURG;

/* Abort Command */

typedef struct z_abrtcmd {
  unsigned int  xci_id[2];		/* Transaction ID */
} Z_ABRTCMD;

/* Mark Abort */

typedef struct z_mrkabrt {
  unsigned int  xci_id[2];		/* Transaction ID */
} Z_MRKABRT;

/* Note that BUSRST, and SETDIAG use NO fields from the "B" section */
/* of the command format; thus, there is no need for a special set of */
/* structures. */


/* Read\Write current state of operational parameters which are stored in EEPROM */
 
typedef struct z_param {
  unsigned short int eeprm_fcn;		/* EEPROM function word */
  unsigned char	     c0_id;		/* Channel 0 SCSI node id */
  unsigned char	     c1_id;		/* Channel 1 SCSI node id */
  unsigned char      mod_ser_no[12];	/* Module serial number */
  unsigned int	     fw_image_rev;	/* Firmware image revison */
  unsigned char	     fw_rev_date[12];	/* Firmware revision date */
} Z_PARAM;

/* Read\Write firmware image from/to local adapter memory */
 
typedef struct z_image {
  unsigned int	     buf_offset;	/* Byte offset to start of buffer */
  unsigned int 	     buf_len;		/* Length of buffer */
  TYPE_PTR	     root_ptr[2];	/* Data buffer root pointers 0 & 1 */
} Z_IMAGE;

/* XZA Generic command type.  Any XZA command field may be accessed */
/* using this structure.  Note that this structure does not include the */
/* Queue Buffer header */
typedef volatile union xza_command {  /* Section "B" only */
  N_RETQE      retqe;
  N_QERET      qeret;
  Z_RDCNT      rdcnt;
  Z_CNTRD      cntrd;
  Z_PURGQ      purgq;
  Z_QPURG      qpurg;
  Z_SETNEX     setnex;
  Z_NEXSET     nexset;
  Z_SNDSCSI    sndscsi;
  Z_SNDVNDR    sndvndr;
  Z_ABRTCMD    abrtcmd;
  Z_MRKABRT    mrkabrt;
  Z_PARAM      param;
  Z_IMAGE      image;
} XZA_COMMAND;
