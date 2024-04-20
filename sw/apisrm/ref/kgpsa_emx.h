#if !defined(KGPSA_EMX_HDR)
#define KGPSA_EMX_HDR


typedef	unsigned UBIT;
typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef unsigned int U32;

/*
 *  Begin Adapter configuration parameters.  Many of these will be replaced
 *  (someday) with parameters read from the registry or a configuration file.
 */

#define MAX_FC_BRDS		 4 /* Maximum # boards per system */
#define FC_MAX_TRANSFER	   0x20000 /* Maximum transfer size per operation */
#define MAX_CONFIGURED_RINGS     3 /* # rings currently used */

#if 0
#define IOCB_CMD_R0_ENTRIES      5 /* ELS command ring entries */
#define IOCB_RSP_R0_ENTRIES      5 /* ELS response ring entries */
#define IOCB_CMD_R1_ENTRIES      1 /* IP command ring entries */
#define IOCB_RSP_R1_ENTRIES      1 /* IP response ring entries */
#define IOCB_CMD_R2_ENTRIES     78 /* FCP command ring entries */
#define IOCB_RSP_R2_ENTRIES     30 /* FCP response ring entries */
#endif

#if 1
#define IOCB_CMD_R0_ENTRIES     50 /* ELS command ring entries */
#define IOCB_RSP_R0_ENTRIES     50 /* ELS response ring entries */
#define IOCB_CMD_R1_ENTRIES      1 /* IP command ring entries */
#define IOCB_RSP_R1_ENTRIES      1 /* IP response ring entries */
#define IOCB_CMD_R2_ENTRIES     10 /* FCP command ring entries */
#define IOCB_RSP_R2_ENTRIES      8 /* FCP response ring entries */
#endif

#define FCBUFSIZE	      2048 /* maximum transfer size, fc frame size */
#define FC_MAXRETRY		 3 /* max retries for ELS commands */
#define FC_ELS_RING		 0 /* use ring 0 for ELS commands */
#define FC_IP_RING               1 /* use ring 1 for IP commands */
#define FC_FCP_RING		 2 /* use ring 2 for FCP commands */

#define FF_DEF_EDTOV           500 /* Default E_D_TOV (500ms) */ 
#define FF_DEF_ALTOV             1 /* Default AL_TIME (1ms) */ 
#define MB_WAIT_PERIOD         500 /* Wait period in usec inbetween MB polls */
#define MAX_MB_COMPLETION     1000 /* # MB_WAIT_PERIODs to wait for MB cmplt */

#define FF_REG_AREA_SIZE       256 /* size, in bytes, of i/o register area */
#define FF_SLIM_SIZE	      4096 /* size, in bytes, of SLIM */

/*
 * Miscellaneous stuff....
 */
#define OWN_CHIP        1        /* IOCB / Mailbox is owned by FireFly */
#define OWN_HOST        0        /* IOCB / Mailbox is owned by Host */
#define END_OF_CHAIN    0
#define MAX_BIOCB     120        /* max# of BIU IOCBs in shared memory */
#define IOCB_WORD_SZ    8        /* # of words in ULP BIU XCB */
#define MAX_RINGS       4        /* Max # of supported rings */

/* defines for type field in fc header */
#define FC_ELS_DATA	0x1
#define FC_LLC_SNAP	0x5		

/* defines for rctl field in fc header */
#define FC_DEV_DATA	0x0		
#define FC_ELS_REQ	0x22


#define KGPSA_COMMAND_QUEUE_SIZE 10

#define DEV_READY                   0x01 /* Only this state accept commands */
#define DEV_INITIALIZING            0x02
#define DEV_INITIAL_DISCOVER        0x03
#define DEV_RE_DISCOVER             0x04
#define DEV_PROBE_NEEDED            0x05
#define DEV_PROBE_IN_PROGRESS       0x06
#define DEV_UNINITIALIZED           0x07



/* defines for fc_flag */
#define FC_CHG_NVP      1       /* Update NVRAM on board */
#define FC_CHG_TOV      2       /* Update board timers with CONFIG_LINK */
#define FC_MULTI        4       /* Enable multicast address processing */
#define FC_LINK_DOWN    8       /* same as ffstate */

/* defines for fc_ffstate */
#define FC_INIT_START           1
#define FC_INIT_NVPARAMS        2
#define FC_INIT_REV             3
#define FC_INIT_PARTSLIM        4
#define FC_INIT_CFGRING         5
#define FC_INIT_SPARAM          6
#define FC_INIT_INITLINK        7
#define FC_LINK_DOWN            8
#define FC_LINK_UP              9
#define FC_FLOGI                10
#define FC_REG_LOGIN            11
#define FC_READ_LA              12
#define FC_CLEAR_LA             13
#define FC_CFG_LINK             14
#define FC_LOOP_DISC            15
#define FC_READY                16
#define FC_ERROR                0xff

#define NADDR_LEN       6       /* MAC network address length */





/*=====================================================================*/

/*
 *  Start FireFly Register definitions
 */

/* PCI register offsets */
#define CMD_REG_OFFSET	0x4	/* PCI command configuration */
#define MEM_ADDR_OFFSET	0x10	/* SLIM base memory address */
#define MEMH_OFFSET	0x14	/* SLIM base memory high address */
#define REG_ADDR_OFFSET	0x18	/* REGISTER base memory address */
#define REGH_OFFSET	0x1c	/* REGISTER base memory high address */
#define IO_ADDR_OFFSET	0x20	/* BIU I/O registers */

#define CMD_CFG_VALUE	0x147	/* I/O and mem enable, master, SERR, PERR */


typedef struct      /* FireFly BIU registers */
   {
   ulong  hostAtt;          /* See definitions for Host Attention register */
   ulong  chipAtt;          /* See definitions for Chip Attention register */
   ulong  hostStatus;       /* See definitions for Host Status register */
   ulong  hostControl;      /* See definitions for Host Control register */
   } FF_REGS, *PFF_REGS;

/* ==== Register Bit Definitions ==== */

/* Host Attention Register */

#define HA_REG_OFFSET  0 /* Word offset from register base address */
#define HA 0		 /* Byte offset */

#define HA_R0RE_REQ    0x00000001  /* Bit  0 */
#define HA_R0CE_RSP    0x00000002  /* Bit  1 */
#define HA_R0ATT       0x00000008  /* Bit  3 */
#define HA_R1RE_REQ    0x00000010  /* Bit  4 */
#define HA_R1CE_RSP    0x00000020  /* Bit  5 */
#define HA_R1ATT       0x00000080  /* Bit  7 */
#define HA_R2RE_REQ    0x00000100  /* Bit  8 */
#define HA_R2CE_RSP    0x00000200  /* Bit  9 */
#define HA_R2ATT       0x00000800  /* Bit 11 */
#define HA_R3RE_REQ    0x00001000  /* Bit 12 */
#define HA_R3CE_RSP    0x00002000  /* Bit 13 */
#define HA_R3ATT       0x00008000  /* Bit 15 */
#define HA_LATT        0x20000000  /* Bit 29 */
#define HA_MBATT       0x40000000  /* Bit 30 */
#define HA_ERATT       0x80000000  /* Bit 31 */


/* Chip Attention Register */

#define CA_REG_OFFSET  1 /* Word offset from register base address */
#define CA 0x4		 /* Byte offset */

#define CA_R0CE_REQ    0x00000001  /* Bit  0 */
#define CA_R0RE_RSP    0x00000002  /* Bit  1 */
#define CA_R0ATT       0x00000008  /* Bit  3 */
#define CA_R1CE_REQ    0x00000010  /* Bit  4 */
#define CA_R1RE_RSP    0x00000020  /* Bit  5 */
#define CA_R1ATT       0x00000080  /* Bit  7 */
#define CA_R2CE_REQ    0x00000100  /* Bit  8 */
#define CA_R2RE_RSP    0x00000200  /* Bit  9 */
#define CA_R2ATT       0x00000800  /* Bit 11 */
#define CA_R3CE_REQ    0x00001000  /* Bit 12 */
#define CA_R3RE_RSP    0x00002000  /* Bit 13 */
#define CA_R3ATT       0x00008000  /* Bit 15 */
#define CA_MBATT       0x40000000  /* Bit 30 */


/* Host Status Register */

#define HS_REG_OFFSET  2 /* Word offset from register base address */
#define HS 0x8		 /* Byte offset */

#define HS_MBRDY       0x00400000  /* Bit 22 */
#define HS_FFRDY       0x00800000  /* Bit 23 */
#define HS_FFER8       0x01000000  /* Bit 24 */
#define HS_FFER7       0x02000000  /* Bit 25 */
#define HS_FFER6       0x04000000  /* Bit 26 */
#define HS_FFER5       0x08000000  /* Bit 27 */
#define HS_FFER4       0x10000000  /* Bit 28 */
#define HS_FFER3       0x20000000  /* Bit 29 */
#define HS_FFER2       0x40000000  /* Bit 30 */
#define HS_FFER1       0x80000000  /* Bit 31 */
#define HS_FFERM       0xFF000000  /* Mask for error bits 31:24 */


/* Host Control Register */

#define HC_REG_OFFSET  3 /* Word offset from register base address */
#define HC 0xC		 /* Byte offset */

#define HC_MBINT_ENA   0x00000001  /* Bit  0 */
#define HC_R0INT_ENA   0x00000002  /* Bit  1 */
#define HC_R1INT_ENA   0x00000004  /* Bit  2 */
#define HC_R2INT_ENA   0x00000008  /* Bit  3 */
#define HC_R3INT_ENA   0x00000010  /* Bit  4 */
#define HC_INITHBI     0x02000000  /* Bit 25 */
#define HC_INITMB      0x04000000  /* Bit 26 */
#define HC_INITFF      0x08000000  /* Bit 27 */
#define HC_LAINT_ENA   0x20000000  /* Bit 29 */
#define HC_ERINT_ENA   0x80000000  /* Bit 31 */

/*
 *  End FireFly Register definitions
 */

/*=====================================================================*/

/*
 *  Start of FCP specific structures
 */



/*
 * These are defined in the FCP standard(X3.269-199X revision 12) Chapter 7
 */

/*
 * FCP Commmand 
 */
typedef struct{
   union {
     struct {
	ulong   msb;             /* FCP_LUN MSB */
        ulong   lsb;             /* FCP_LUN LSB */
	 } w;
	 uchar       byte[8];
   } fcp_lun;
   uchar    fcp_ctl_rsvd;         /* FCP_CTL byte 0 (reserved)              */
   uchar    fcp_ctl_task_code;    /* FCP_CTL byte 1 task codes              */
   uchar    fcp_ctl_task_mgmt;    /* FCP_CTL byte 2 task management codes   */
   uchar    fcp_ctl_exec_mgmt;    /* FCP_CTL byte 3 execution mgmt codes    */
   uchar    fcp_cdb[16];          /* FCP_CDB                                */
   ulong    fcp_dl;               /* FCP_DL Data Length                     */ 
}FCP_CMD;

#define SIZEOF_FCP_CMD	sizeof(FCP_CMD)

/*
 * FCP_CTL Task Codes 
 */
#define  SIMPLE_Q        0x00
#define  HEAD_OF_Q       0x01
#define  ORDERED_Q       0x02
#define  ACA_Q           0x04
#define  UNTAGGED        0x05

/*
 * FCP_CTL Task Management Codes 
 */
#define  TASK_MGMT_OFF   0x00
#define  ABORT_TASK_SET  0x02      /* Bit 1 */
#define  CLEAR_TASK_SET  0x04      /* bit 2 */
#define  TARGET_RESET    0x20      /* bit 5 */
#define  CLEAR_ACA       0x40      /* bit 6 */
#define  TERMINATE_TASK  0x80      /* bit 7 */

/*
 * FCP_CTL Exec Management Codes 
 */
#define  NO_DATA         0x00    /* Bit 0 & Bit 1 = 0 */
#define  WRITE_DATA      0x01    /* Bit 0             */
#define  READ_DATA       0x02    /* Bit 1             */
/*
 *  Definition of FCP_RSP Packet
 */

typedef struct _FCP_RSP
   {
   ulong   rspRsvd1;    /* FC Word 0, byte 0:3 */
   ulong   rspRsvd2;    /* FC Word 1, byte 0:3 */

   uchar   rspStatus0;  /* FCP_STATUS byte 0 (reserved) */
   uchar   rspStatus1;  /* FCP_STATUS byte 1 (reserved) */
   uchar   rspStatus2;  /* FCP_STATUS byte 2 field validity */
#define RSP_LEN_VALID  0x01 /* bit 0 */
#define SNS_LEN_VALID  0x02 /* bit 1 */
/*    struct nvram nvram; */
/*    struct nvram nvram; */
#define RESID_OVER     0x04 /* bit 2 */
#define RESID_UNDER    0x08 /* bit 3 */
   uchar   rspStatus3;  /* FCP_STATUS byte 3 SCSI status byte */
#define SCSI_STAT_GOOD        0x00
#define SCSI_STAT_CHECK_COND  0x02
#define SCSI_STAT_COND_MET    0x04
#define SCSI_STAT_BUSY        0x08
#define SCSI_STAT_INTERMED    0x10
#define SCSI_STAT_INTERMED_CM 0x14
#define SCSI_STAT_RES_CNFLCT  0x18
#define SCSI_STAT_CMD_TERM    0x22
#define SCSI_STAT_QUE_FULL    0x28

   ulong   rspResId;    /* Residual xfer if RESID_xxxx set in fcpStatus2 */
                        /* Received in Big Endian format */
   ulong   rspSnsLen;   /* Length of sense data in fcpSnsInfo */  
                        /* Received in Big Endian format */
   ulong   rspRspLen;   /* Length of FCP response data in fcpRspInfo */ 
                        /* Received in Big Endian format */

   uchar   rspInfo0;    /* FCP_RSP_INFO byte 0 (reserved) */
   uchar   rspInfo1;    /* FCP_RSP_INFO byte 1 (reserved) */
   uchar   rspInfo2;    /* FCP_RSP_INFO byte 2 (reserved) */
   uchar   rspInfo3;    /* FCP_RSP_INFO RSP_CODE byte 3 */ 
#define RSP_NO_FAILURE       0x00
#define RSP_DATA_BURST_ERR   0x01
#define RSP_CMD_FIELD_ERR    0x02
#define RSP_RO_MISMATCH_ERR  0x03
#define RSP_TM_NOT_SUPPORTED 0x04 /* Task mgmt func not sup'ed */
#define RSP_TM_NOT_COMPLETED 0x05 /* Task mgmt func not perf. */                

   ulong   rspInfoRsvd; /* FCP_RSP_INFO bytes 4-7 (reserved) */

   /* 
    *  Define maximum size of SCSI Sense buffer.
    *  Seagate never issues more than 18 bytes of Sense data
    */
#define MAX_FCP_SNS  32   
   uchar   rspSnsInfo[MAX_FCP_SNS];
   } FCP_RSP, *PFCP_RSP;

/*
 *  Definition of FCP_CMND Packet
 */

typedef struct _FCP_CMND
   {
   ulong  fcpLunMsl;   /* most  significant lun word (32 bits) */
   ulong  fcpLunLsl;   /* least significant lun word (32 bits) */ 

   uchar  fcpCntl0;    /* FCP_CNTL byte 0 (reserved) */
   uchar  fcpCntl1;    /* FCP_CNTL byte 1 task codes */
#define  SIMPLE_Q        0x00
#define  HEAD_OF_Q       0x01
#define  ORDERED_Q       0x02
#define  ACA_Q           0x04
#define  UNTAGGED        0x05
   uchar  fcpCntl2;    /* FCP_CTL byte 2 task management codes */
#define  ABORT_TASK_SET  0x02 /* Bit 1 */
#define  CLEAR_TASK_SET  0x04 /* bit 2 */
#define  TARGET_RESET    0x20 /* bit 5 */
#define  CLEAR_ACA       0x40 /* bit 6 */
#define  TERMINATE_TASK  0x80 /* bit 7 */
   uchar  fcpCntl3;
#define  WRITE_DATA      0x01 /* Bit 0 */
#define  READ_DATA       0x02 /* Bit 1 */

   uchar  fcpCdb[16];  /* SRB cdb field is copied here */
   ulong  fcpDl;       /* Total transfer length */          
   } FCP_CMND, *PFCP_CMND;

/*
 *  End of FCP specific structures
 */

#define FL_ALPA    0x00      /* AL_PA of FL_Port */

/* Fibre Channel Service Parameter definitions */

#define FC_PH_4_0   6  /* FC-PH version 4.0 */
#define FC_PH_4_1   7  /* FC-PH version 4.1 */
#define FC_PH_4_2   8  /* FC-PH version 4.2 */
#define FC_PH_4_3   9  /* FC-PH version 4.3 */

#define FC_PH_LOW   8  /* Lowest supported FC-PH version */ 
#define FC_PH_HIGH  9  /* Highest supported FC-PH version */

#define FF_FRAME_SIZE     2048


/* ==== Mailbox Commands ==== */
#define MBX_SHUTDOWN        0x00     /* terminate testing */
#define MBX_LOAD_SM         0x01
#define MBX_READ_NV         0x02
#define MBX_WRITE_NV        0x03
#define MBX_RUN_BIU_DIAG    0x04
#define MBX_INIT_LINK       0x05
#define MBX_DOWN_LINK       0x06
#define MBX_CONFIG_LINK     0x07
#define MBX_PART_SLIM       0x08
#define MBX_CONFIG_RING     0x09
#define MBX_RESET_RING      0x0A
#define MBX_READ_CONFIG     0x0B
#define MBX_READ_RCONFIG    0x0C
#define MBX_READ_SPARM      0x0D
#define MBX_READ_STATUS     0x0E
#define MBX_READ_RPI        0x0F
#define MBX_READ_XRI        0x10
#define MBX_READ_REV        0x11
#define MBX_READ_LNK_STAT   0x12
#define MBX_REG_LOGIN       0x13
#define MBX_UNREG_LOGIN     0x14
#define MBX_READ_LA         0x15
#define MBX_CLEAR_LA        0x16
#define MBX_DUMP_CFG        0x17
#define MBX_DUMP_CONTEXT    0x18
#define MBX_RUN_DIAGS       0x19
#define MBX_RESTART         0x1A
#define MBX_UPDATE_CFG      0x1B

#define MBX_MAX_CMDS	    0x1C


/* ==== IOCB Commands ==== */

#define CMD_RCV_SEQUENCE_CX     0x01
#define CMD_XMIT_SEQUENCE_CR    0x02
#define CMD_XMIT_SEQUENCE_CX    0x03
#define CMD_XMIT_BCAST_CN       0x04
#define CMD_XMIT_BCAST_CX       0x05
#define CMD_QUE_RING_BUF_CN     0x06
#define CMD_QUE_XRI_BUF_CX      0x07
#define CMD_IOCB_CONTINUE_CN    0x08
#define CMD_RET_XRI_BUF_CX      0x09
#define CMD_ELS_REQUEST_CR      0x0A
#define CMD_ELS_REQUEST_CX      0x0B
#define CMD_RCV_ELS_REQ_CX      0x0D
#define CMD_ABORT_XRI_CN        0x0E
#define CMD_ABORT_XRI_CX        0x0F
#define CMD_CLOSE_XRI_CR        0x10
#define CMD_CLOSE_XRI_CX        0x11
#define CMD_CREATE_XRI_CR       0x12
#define CMD_CREATE_XRI_CX       0x13
#define CMD_GET_RPI_CN          0x14
#define CMD_XMIT_ELS_RSP_CX     0x15
#define CMD_GET_RPI_CR          0x16
#define CMD_XRI_ABORTED_CX      0x17
#define CMD_FCP_IWRITE_CR       0x18
#define CMD_FCP_IWRITE_CX       0x19
#define CMD_FCP_IREAD_CR        0x1A
#define CMD_FCP_IREAD_CX        0x1B
#define CMD_FCP_ICMND_CR        0x1C
#define CMD_FCP_ICMND_CX        0x1D
#define CMD_FCP_TSEND_CX        0x1F
#define CMD_FCP_TRECEIVE_CX     0x21
#define CMD_FCP_TRSP_CX         0x23



#define IOCB_RCV_SEQUENCE_CX     0x01
#define IOCB_XMIT_SEQUENCE_CR    0x02
#define IOCB_XMIT_SEQUENCE_CX    0x03
#define IOCB_XMIT_BCAST_CN       0x04
#define IOCB_XMIT_BCAST_CX       0x05
#define IOCB_QUE_RING_BUF_CN     0x06
#define IOCB_QUE_XRI_BUF_CX      0x07
#define IOCB_IOCB_CONTINUE_CN    0x08
#define IOCB_RET_XRI_BUF_CX      0x09
#define IOCB_ELS_REQUEST_CR      0x0A
#define IOCB_ELS_REQUEST_CX      0x0B
#define IOCB_RCV_ELS_REQ_CX      0x0D
#define IOCB_ABORT_XRI_CN        0x0E
#define IOCB_ABORT_XRI_CX        0x0F
#define IOCB_CLOSE_XRI_CR        0x10
#define IOCB_CLOSE_XRI_CX        0x11
#define IOCB_CREATE_XRI_CR       0x12
#define IOCB_CREATE_XRI_CX       0x13
#define IOCB_GET_RPI_CN          0x14
#define IOCB_XMIT_ELS_RSP_CX     0x15
#define IOCB_GET_RPI_CR          0x16
#define IOCB_XRI_ABORTED_CX      0x17
#define IOCB_FCP_IWRITE_CR       0x18
#define IOCB_FCP_IWRITE_CX       0x19
#define IOCB_FCP_IREAD_CR        0x1A
#define IOCB_FCP_IREAD_CX        0x1B
#define IOCB_FCP_ICMND_CR        0x1C
#define IOCB_FCP_ICMND_CX        0x1D
#define IOCB_FCP_TSEND_CX        0x1F
#define IOCB_FCP_TRECEIVE_CX     0x21
#define IOCB_FCP_TRSP_CX         0x23





/*
 *  Define Status
 */                                   
#define MBX_SUCCESS                 0
#define MBXERR_NUM_RINGS            1
#define MBXERR_NUM_IOCBS            2
#define MBXERR_IOCBS_EXCEEDED       3
#define MBXERR_BAD_RING_NUMBER      4
#define MBXERR_MASK_ENTRIES_RANGE   5
#define MBXERR_MASKS_EXCEEDED       6
#define MBXERR_BAD_PROFILE          7
#define MBXERR_BAD_DEF_CLASS        8
#define MBXERR_BAD_MAX_RESPONDER    9
#define MBXERR_BAD_MAX_ORIGINATOR   10
#define MBXERR_RPI_REGISTERED       11
#define MBXERR_RPI_FULL             12
#define MBXERR_NO_RESOURCES         13
#define MBXERR_BAD_RCV_LENGTH       14
#define MBXERR_DMA_ERROR            15
#define MBXERR_BAD_IOCB_COUNT       16
#define MBXERR_UNSUPPORTED_FEATURE  17
#define MBXERR_UNKNOWN_CMD          18
#define MBXERR_FLASH_WRITE_FAILED   0x0100
#define MBXERR_RING_RANGE           0x0a00
#define MBXERR_NOT_ACTIVE_RPI       0x0f00
#define MBXERR_NO_ACTIVE_XRI        0x1000
#define MBXERR_XRI_NOT_ACTIVE       0x1001
#define MBXERR_UNREG_NOT_USED       0x1400
#define MBXERR_NO_LINK_ATTN         0x1500
#define MBXERR_NO_LA_TO_CLEAR       0x1600
#define MBXERR_NOT_LAST_LINK_ATTN   0x1601
#define MBX_NOT_FINISHED           255

/*
 * Error codes returned by issue_mb_cmd()
 */
#define MBX_BUSY                   253 /* Attempted cmd to a busy Mailbox */
#define MBX_TIMEOUT                254 /* Max time-out expired waiting for */
                                       /* synch. Mailbox operation */
/*
 * flags for issue_mb_cmd()
 */
#define MBX_POLL	1  /* poll mailbox till command done, then return */
#define MBX_SLEEP 	2  /* sleep till mailbox intr cmpl wakes thread up */
#define MBX_NOWAIT	3  /* issue command then return immediately */






typedef struct
   {
   ubit    crEnd      : 8;    /* Low order bit first word */
   ubit    crBegin    : 8;
   ubit    crReserved :16;
   ubit    rrEnd      : 8;    /* Low order bit second word */
   ubit    rrBegin    : 8;
   ubit    rrReserved :16;
   } RINGS;             


typedef struct
   {
   ushort  numCiocb;   
   ushort  offCiocb;   
   ushort  numRiocb;   
   ushort  offRiocb;   
   } RING_DEF;		/* dm 03jun96 */


/*
 *  The following F.C. frame stuctures are defined in Big Endian format.
 */

typedef struct _NAME_TYPE
   { /* Structure is in Big Endian format */

   ubit    nameType    : 4;   /* FC Word 0, bit 28:31 */
#define NAME_IEEE           0x1 /* IEEE name */
#define NAME_IEEE_EXT       0x2 /* IEEE extended name */
#define NAME_FC_TYPE        0x3 /* FC native name type */
#define NAME_IP_TYPE        0x4 /* IP address */
#define NAME_CCITT_TYPE     0xC
#define NAME_CCITT_GR_TYPE  0xE

   ubit    IEEEextMsn  : 4; /* FC Word 0, bit 24:27, bit 8:11 of IEEE ext */
   uchar   IEEEextLsb;      /* FC Word 0, bit 16:23, IEEE extended Lsb */
   uchar   IEEE[6];         /* FC IEEE address */
   } NAME_TYPE;  
       

typedef struct _CSP
   { /* Structure is in Big Endian format */

   uchar   fcphHigh;               /* FC Word 0, byte 0 */
   uchar   fcphLow;
   uchar   bbCreditMsb;
   uchar   bbCreditlsb;            /* FC Word 0, byte 3 */
           
   ubit    increasingOffset  : 1;  /* FC Word 1, bit 31 */
   ubit    randomOffset      : 1;
   ubit    word1Reserved2    : 1;
   ubit    fPort             : 1;
   ubit    altBbCredit       : 1;
   ubit    word1Reserved1    : 3;  /* FC Word 1, bit 24:26 */
   uchar   word1Reserved3;         /* FC Word 1, byte 1 */
   uchar   bbRcvSizeMsb;           /*          Upper nibble is reserved */ 
   uchar   bbRcvSizeLsb;           /* FC Word 1, byte 3 */
   union
      {
      struct
         {
         uchar    word2Reserved1;   /* FC Word 2 byte 0 */ 
         uchar    totalConcurrSeq;  /* FC Word 2 byte 1 */
         uchar    roByCategoryMsb;  /* FC Word 2 byte 2 */ 
         uchar    roByCategoryLsb;  /* FC Word 2 byte 3 */
         } nPort;
      ulong r_a_tov;                /* R_A_TOV must be in B.E. format */
      } w2;
           
   ulong   e_d_tov;                /* E_D_TOV must be in B.E. format */
   } CSP;


typedef struct _CLASS_PARMS
   { /* Structure is in Big Endian format */

   ubit    classValid      : 1; /* FC Word 0, bit 31 */   
   ubit    intermix        : 1; /* FC Word 0, bit 30 */
   ubit    stackedXparent  : 1; /* FC Word 0, bit 29 */
   ubit    stackedLockDown : 1; /* FC Word 0, bit 28 */
   ubit    seqDelivery     : 1; /* FC Word 0, bit 27 */
   ubit    word0Reserved1  : 3; /* FC Word 0, bit 24:26 */
   uchar   word0Reserved2;      /* FC Word 0, bit 16:23 */
   ubit    iCtlXidReAssgn  : 2; /* FC Word 0, Bit 14:15 */ 
   ubit    iCtlInitialPa   : 2; /* FC Word 0, bit 12:13 */
   ubit    iCtlAck0capable : 1; /* FC Word 0, bit 11 */
   ubit    iCtlAckNcapable : 1; /* FC Word 0, bit 10 */
   ubit    word0Reserved3  : 2; /* FC Word 0, bit  8: 9 */
   uchar   word0Reserved4;      /* FC Word 0, bit  0: 7 */

   ubit    rCtlAck0capable : 1; /* FC Word 1, bit 31 */   
   ubit    rCtlAckNcapable : 1; /* FC Word 1, bit 30 */   
   ubit    rCtlXidInterlck : 1; /* FC Word 1, bit 29 */   
   ubit    rCtlErrorPolicy : 2; /* FC Word 1, bit 27:28 */   
   ubit    word1Reserved1  : 1; /* FC Word 1, bit 26 */   
   ubit    rCtlCatPerSeq   : 2; /* FC Word 1, bit 24:25 */   
   uchar   word1Reserved2;      /* FC Word 1, bit 16:23 */
   uchar   rcvDataSizeMsb;      /* FC Word 1, bit  8:15 */
   uchar   rcvDataSizeLsb;      /* FC Word 1, bit  0: 7 */
   
   uchar   concurrentSeqMsb;    /* FC Word 2, bit 24:31 */
   uchar   concurrentSeqLsb;    /* FC Word 2, bit 16:23 */
   uchar   EeCreditSeqMsb;      /* FC Word 2, bit  8:15 */
   uchar   EeCreditSeqLsb;      /* FC Word 2, bit  0: 7 */

   uchar   openSeqPerXchgMsb;   /* FC Word 3, bit 24:31 */
   uchar   openSeqPerXchgLsb;   /* FC Word 3, bit 16:23 */
   uchar   word3Reserved1;      /* Fc Word 3, bit  8:15 */
   uchar   word3Reserved2;      /* Fc Word 3, bit  0: 7 */
   } CLASS_PARMS;


typedef struct _SERV_PARM
   { /* Structure is in Big Endian format */
   CSP          cmn;
   NAME_TYPE    portName;
   NAME_TYPE    nodeName;
   CLASS_PARMS  cls1;
   CLASS_PARMS  cls2;
   CLASS_PARMS  cls3;
   ulong        reserved[4];
   uchar        vendorVersion[16];
   } SERV_PARM, *PSERV_PARM;

#if 0
/*
 *  Extended Link Service LS_COMMAND codes (Payload Word 0)
 */

#define ELS_CMD_LS_RJT    0x01000000
#define ELS_CMD_ACC       0x02000000
#define ELS_CMD_PLOGI     0x03000000
#define ELS_CMD_FLOGI     0x04000000
#define ELS_CMD_LOGO      0x05000000
#define ELS_CMD_ABTX      0x06000000
#define ELS_CMD_RCS       0x07000000
#define ELS_CMD_RES       0x08000000
#define ELS_CMD_RSS       0x09000000
#define ELS_CMD_RSI       0x0A000000
#define ELS_CMD_ESTS      0x0B000000
#define ELS_CMD_ESTC      0x0C000000
#define ELS_CMD_ADVC      0x0D000000
#define ELS_CMD_RTV       0x0E000000
#define ELS_CMD_RLS       0x0F000000
#define ELS_CMD_ECHO      0x10000000
#define ELS_CMD_TEST      0x11000000
#define ELS_CMD_RRQ       0x12000000
#define ELS_CMD_PRLI      0x20100014
#define ELS_CMD_PRLO      0x21100014
#define ELS_CMD_PDISC     0x50000000

#endif

                             
/*
 *  Extended Link Service LS_COMMAND codes (Payload Word 0, bits 31:24 )
 *   [NOTE: Extended link Services cmds 0x01-0x12 are found in the FC_PH-1
 *          spec. Cmds >= 0x20 are found in the FC_PH-2 spec.]
 *
 * Note: these ELS values encode all of the 1st 4 bytes of the ELS payload.
 *       (and takes into account our U32 format of little endianess)
 */

#define ELS_CMD_LS_RJT    0x00000001 /* link service reject */
#define ELS_CMD_ACC       0x00000002 /* Accept */
#define ELS_CMD_PLOGI     0x00000003 /* N_Port Login */
#define ELS_CMD_FLOGI     0x00000004 /* F_Port Login */
#define ELS_CMD_LOGO      0x00000005 /* Logout */
#define ELS_CMD_ABTX      0x00000006 /* Abort Exchange */
#define ELS_CMD_RCS       0x00000007 /* Read connection Status */
#define ELS_CMD_RES       0x00000008 /* Read Exchange Status block */
#define ELS_CMD_RSS       0x00000009 /* Read Sequence Status block */
#define ELS_CMD_RSI       0x0000000A /* Request Sequence Initiative */
#define ELS_CMD_ESTS      0x0000000B /* Establish streaming */
#define ELS_CMD_ESTC      0x0000000C /* Estimate credit */
#define ELS_CMD_ADVC      0x0000000D /* Advise credit */
#define ELS_CMD_RTV       0x0000000E /* Read timeout value */
#define ELS_CMD_RLS       0x0000000F /* Read Link status */
#define ELS_CMD_ECHO      0x00000010 /* Echo */
#define ELS_CMD_TEST      0x00000011 /* test */
#define ELS_CMD_RRQ       0x00000012 /* reinstate recovery qualifier */

#define ELS_CMD_PDISC     0x00000050 /* Discover N_port Service parameters */


#define ELS_CMD_RSCN      0x00000061 /* Register for State Chg Notification */
#define ELS_CMD_SCR       0x00000062 /* State Change Registration */


/* 
 * The following cmds are optional for FCP.(see FC-PH-2) 
 *
 * Note that these embed the PRLI els code, Page Length (0x10),
 * and payload length (0x14). This implicitly defines only FCP payloads in
 * the PRLI.
 */


#define ELS_CMD_FCP_PRLI  0x14001020 /* FCP Process Login */
#define ELS_CMD_FCP_PRLO  0x14001021 /* FCP Process logout */

/* 
 * The ELS code explicitly 
 */
#define ELS_CODE_PRLI     0x00000020 /* Process Login */
#define ELS_CODE_PRLO     0x00000021 /* Process logout */

/*
 * The PRLI/PRLO ACC embed the ACC els code, Page Length(0x10), 
 * and the payload length(0x14). This implicitly defines only FCP payloads
 * in the ACC
 */

#define ELS_CMD_FCP_ACC  0x14001002 /* FCP Process Login */


/*
 * Well known address identifier
 */

#define BROADCAST           0xFFFFFF
#define FABRIC_F_PORT       0xFFFFFE
#define FABRIC_CONTROLLER   0xFFFFFD
#define NAME_SERVER         0xFFFFFC
#define TIME_SERVER         0xFFFFFB
#define ALIAS_SERVER        0xFFFFFA
#define QUALITY_FACILITATOR 0xFFFFF9

#define GPN        0x0112        /* Get Port Name */
#define GNN1       0x0113        /* Get Node Name */
#define GCoS       0x0114        /* Get Class of Service */
#define GFC_4      0x0117        /* Get FC-4 types */
#define GSPN       0x0118        /* Get Symbolic Port Name */
#define GPT        0x011A        /* Get Port Type */
#define GP_ID1     0x0121        /* Get adr id - given Port Name */
#define GP_ID2     0x0131        /* Get adr id - given a Node Name */
#define GIP_A      0x0135        /* Get IP adr - given a Node Name */
#define GIPA1      0x0136        /* Get adr id - given a Node Name */
#define GSNN       0x0139        /* Get Symbolic Node Name -given a Node Name */
#define GNN2       0x0153        /* Get Node Name - give IP address */
#define GIPA2      0x0156        /* Get Initail Process Associator - given an IP adr */
#define GP_ID3     0x0171        /* Get adr id - given an FC-4 type value */
#define GP_ID4     0x01A1        /* Get adr id - given a port type value */

/*
 * Registration Name Service Requests
 */
#define RMA        0x0300        /* Remove all */
#define RPN        0x0212        /* Register Port Name with its adr id */
#define RNN        0x0213        /* Register Node Name with its adr id */
#define RCoS       0x0214        /* Register Class of Service w/ adr id */
#define RFC_4      0x0217        /* Register FC-4 Types w/ adr id */
#define RSPN       0x0218        /* Register Symbolic Port Name w/ adr id */
#define RPT        0x021A        /* Register Port Type w/ adr id */
#define RIP_A      0x0235        /* Register IP adr w/ its Node Name */
#define RIPA       0x0236        /* Register Initial Process Assoc w/ Node Name */
#define RSNN       0x0239        /* Register Symbolic Node Name - w/ Node Name */

/*
 * values for RSCN byte 0 - port address format
 */
#define RSCN_PAFMT_PORT_ADDRESS         0x00
#define RSCN_PAFMT_AREA_ADDRESS         0x01
#define RSCN_PAFMT_DOMAIN_ADDRESS       0x02
#define RSCN_PAFMT_FABRIC_CHANGE        0x03        /* Brocade-Specific */

/*
 * values for RSCN registration types (for SCR payload)
 */
#define RSCN_RTYPE_FABRIC_EVENTS        0x01    /* all Fabric detected events */
#define RSCN_RTYPE_NPORT_EVENTS         0x02    /* all Nport detected events */
#define RSCN_RTYPE_ALL_EVENTS           0x03    /* all detected events */
#define RSCN_RTYPE_CLEAR_REGISTRATION   0xFF    /* clear registration */


/*
 *  LS_RJT Payload Definition
 */

typedef struct _LS_RJT
   { /* Structure is in Big Endian format */
   union
      {
      ulong lsRjtError; 
      struct
         {
         uchar  lsRjtRsvd0;                    /* FC Word 0, bit 24:31 */
         uchar  lsRjtRsnCode;                  /* FC Word 0, bit 16:23 */
            /* LS_RJT reason codes */
#define LSRJT_INVALID_CMD     0x01
#define LSRJT_LOGICAL_ERR     0x03
#define LSRJT_LOGICAL_BSY     0x05
#define LSRJT_PROTOCOL_ERR    0x07
#define LSRJT_UNABLE_TPC      0x09 /* Unable to perform command */
#define LSRJT_CMD_UNSUPPORTED 0x0B
#define LSRJT_VENDOR_UNIQUE   0xFF /* See Byte 3 */
                                
         uchar  lsRjtRsnCodeExp;               /* FC Word 0, bit  8:15 */
            /* LS_RJT reason explanation */
#define LSEXP_NOTHING_MORE      0x00
#define LSEXP_SPARM_OPTIONS     0x01
#define LSEXP_SPARM_ICTL        0x03
#define LSEXP_SPARM_RCTL        0x05 
#define LSEXP_SPARM_RCV_SIZE    0x07
#define LSEXP_SPARM_CONCUR_SEQ  0x09
#define LSEXP_SPARM_CREDIT      0x0B
#define LSEXP_INVALID_PNAME     0x0D
#define LSEXP_INVALID_NNAME     0x0E
#define LSEXP_INVLAID_CSP       0x0F
#define LSEXP_INVLAID_ASSOC_HDR 0x11
#define LSEXP_ASSOC_HDR_REQ     0x13
#define LSEXP_INVALID_O_SID     0x15
#define LSEXP_INVALID_OX_RX     0x17
#define LSEXP_CMD_IN_PROGRESS   0x19
#define LSEXP_INVLID_NPORT_ID   0x1F
#define LSEXP_INVALID_SEQ_ID    0x21
#define LSEXP_INVALID_XCHG      0x23
#define LSEXP_INACTIVE_XCHG     0x25
#define LSEXP_RQ_REQUIRED       0x27 
#define LSEXP_OUT_OF_RESOURCE   0x29
#define LSEXP_CANT_GIVE_DATA    0x2A
#define LSEXP_REQ_UNSUPPORTED   0x2C
         uchar  vendorUnique;                  /* FC Word 0, bit  0: 7 */ 
         } b;
      } un;
   } LS_RJT;


/*
 *  N_Port Login (FLOGO/PLOGO Request) Payload Definition
 */

typedef struct _LOGO
   { /* Structure is in Big Endian format */
   union
      {
      ulong nPortId32;              /* Access nPortId as a word */
      struct
         {
         uchar   word1Reserved1;    /* FC Word 1, bit 31:24 */ 
         uchar   nPortIdByte0;      /* N_port  ID bit 16:23 */ 
         uchar   nPortIdByte1;      /* N_port  ID bit  8:15 */ 
         uchar   nPortIdByte2;      /* N_port  ID bit  0: 7 */ 
         } b;
      } un;
   NAME_TYPE portName;              /* N_port name field */
   } LOGO;


/*
 *  FCP Login (PRLI Request / ACC) Payload Definition
 */

#define PRLX_PAGE_LEN  0x10

typedef struct _PRLI
   { /* Structure is in Big Endian format */
   uchar   prliType;              /* FC Parm Word 0, bit 24:31 */ 
#define PRLI_FCP_TYPE 0x08
   uchar   word0Reserved1;        /* FC Parm Word 0, bit 16:23 */  
   ubit    acceptRspCode   :  4;  /* FC Parm Word 0, bit 8:11, ACC ONLY */ 
#define PRLI_REQ_EXECUTED     0x1
#define PRLI_NO_RESOURCES     0x2
#define PRLI_INIT_INCOMPLETE  0x3
#define PRLI_NO_SUCH_PA       0x4
#define PRLI_PREDEF_CONFIG    0x5
#define PRLI_PARTIAL_SUCCESS  0x6
#define PRLI_INVALID_PAGE_CNT 0x7
   ubit    word0Reserved2  :  1;  /* FC Parm Word 0, bit 12 */ 
   ubit    estabImagePair  :  1;  /* FC Parm Word 0, bit 13 */ 
   ubit    respProcAssocV  :  1;  /* FC Parm Word 0, bit 14 */
                                  /*    ACC = imagePairEstablished */ 
   ubit    origProcAssocV  :  1;  /* FC Parm Word 0, bit 15 */
   uchar   word0Reserved3;        /* FC Parm Word 0, bit 0:7 */
   ulong   origProcAssoc;         /* FC Parm Word 1, bit 0:31 */
   ulong   respProcAssoc;         /* FC Parm Word 2, bit 0:31 */
   uchar   word3Reserved1;        /* FC Parm Word 3, bit 24:31 */
   uchar   word3Reserved2;        /* FC Parm Word 3, bit 16:23 */
   uchar   word3Reserved3;        /* FC Parm Word 3, bit  8:15 */
   ubit    writeXferRdyDis :  1;  /* FC Parm Word 3, bit  0 */
   ubit    readXferRdyDis  :  1;  /* FC Parm Word 3, bit  1 */
   ubit    dataRspMixEna   :  1;  /* FC Parm Word 3, bit  2 */
   ubit    cmdDataMixEna   :  1;  /* FC Parm Word 3, bit  3 */
   ubit    targetFunc      :  1;  /* FC Parm Word 3, bit  4 */
   ubit    initiatorFunc   :  1;  /* FC Parm Word 3, bit  5 */
   ubit    dataOverLay     :  1;  /* FC Parm Word 3, bit  6 */
   ubit    word3Reserved4  :  1;  /* FC Parm Word 3, bit  7 */
   } PRLI;       /* dm 1-aug-97 */



/*
 *  FCP Logout (PRLO Request / ACC) Payload Definition
 */

typedef struct _PRLO
   { /* Structure is in Big Endian format */
   uchar   prloType;              /* FC Parm Word 0, bit 24:31 */ 
#define PRLO_FCP_TYPE  0x08
   uchar   word0Reserved1;        /* FC Parm Word 0, bit 16:23 */  
   ubit    origProcAssocV  :  1;  /* FC Parm Word 0, bit 15 */
   ubit    respProcAssocV  :  1;  /* FC Parm Word 0, bit 14 */
   ubit    word0Reserved2  :  2;  /* FC Parm Word 0, bit 12:13 */ 
   ubit    acceptRspCode   :  4;  /* FC Parm Word 0, bit 8:11, ACC ONLY */ 
#define PRLO_REQ_EXECUTED     0x1
#define PRLO_NO_SUCH_IMAGE    0x4
#define PRLO_INVALID_PAGE_CNT 0x7
   uchar   word0Reserved3;        /* FC Parm Word 0, bit 0:7 */

   ulong   origProcAssoc;         /* FC Parm Word 1, bit 0:31 */

   ulong   respProcAssoc;         /* FC Parm Word 2, bit 0:31 */

   ulong   word3Reserved1;        /* FC Parm Word 3, bit 0:31 */

   } PRLO;



/*
 * SCR payload
 */
typedef union {
    ulong     word;
    struct {
        uchar   rsvd1;      /* Payload word 1 bits 31:24 */
        uchar   rsvd2;      /* Payload word 1 bits 23:16 */
        uchar   rsvd3;      /* Payload word 1 bits 15:8 */
        uchar   regType;    /* Payload word 1 bits 7:0 - registration type */
    } bytes;
} SCR;


/*
 * Structure that overlays on a 32 bit value normally containing
 * a FC port id (e.g. 1 byte reserved, with 3 bytes of FC port id (address))
 */
typedef union gen_port_id {
    ulong     word;
    struct {
        ulong     bits24      : 24;
        ulong     bitsRsvd    :  8;
    }       bits;
    struct {
        uchar   b0; /* AL_PA on a loop */
        uchar   b1;
        uchar   b2;
        uchar   b3; /* Unused */
    }       bytes;
} GEN_PORT_ID;



/*
 * RSCN payload
 *  Note: there can actually be up to 63 GEN_PORT_ID elements returned
 *        in this payload, not just the one defined here.
 *    (256 byte els pkt, minus 4bytes header = 252, divided by sizeof
 *     gen_port_id (4) = 63)
 */
typedef struct {
    GEN_PORT_ID     addr;           /*
                                     * byte0 indicates port address format
                                     * bytes 1-3 contain port id
                                     */
} RSCN;



/*
 *  Structure to define all ELS Payload types
 */

typedef struct _ELS_PKT
   { /* Structure is in Big Endian format */
   uchar     elsCode;           /* FC Word 0, bit 24:31 */
   uchar     elsByte1;
   uchar     elsByte2;
   uchar     elsByte3;
   union
      {
      LS_RJT        lsRjt;   /* Payload for LS_RJT ELS response */
      SERV_PARM     logi;    /* Payload for PLOGI/FLOGI/PDISC/ACC */
      LOGO          logo;    /* Payload for PLOGO/FLOGO/ACC */
      PRLI          prli;    /* Payload for PRLI/ACC */
      PRLO          prlo;    /* Payload for PRLO/ACC */
      SCR           scr;     /* Payload for SCR registration */
      RSCN          rscn;    /* Payload for RSCN receipt */
      uchar         pad[124];  /* pad to total ELS pkt size of 128 bytes */
      } un;      
   } ELS_PKT;   


/*
 *    Begin Structure Definitions for Mailbox Commands
 */

typedef struct
   {
   uchar   rmask;   
   uchar   rval;   
   uchar   tmask;   
   uchar   tval;   
   } RR_REG;
       
typedef struct
   {       
   uchar  *bdeAddress;
   ubit    bdeSize     : 24;
   ubit    bdeAddrHigh :  4;
   ubit    bdeReserved :  4;
   } ULP_BDE;                  /* dm 03jun96 */


/* Structure for MB Command LOAD_SM (01) */

typedef struct
   {
   ubit    rsvd1         : 30;
   ubit    update_flash  :  1;
   ubit    load_cmplt    :  1;
   ushort  offset;
   ushort  length;
   ulong   load_array[1]; /* array size = 4084 */
   } LOAD_SM_VAR; 


/* Structure for MB Command READ_NVPARM (02) */

typedef struct
   {       
   ulong   rsvd1[3];          /* Read as all one's */
   ulong   rsvd2;             /* Read as all zero's */
   ulong   portname[2];       /* N_PORT name */
   ulong   nodename[2];       /* NODE name */
   ubit    pref_DID  : 24;
   ubit    hardAL_PA :  8;
   ulong   rsvd3[247];        /* Read as all one's */
   } READ_NV_VAR;


/* Structure for MB Command WRITE_NVPARMS (03) */

typedef struct
   {
   ulong   rsvd1[3];          /* Must be all one's */
   ulong   rsvd2;             /* Must be all zero's */
   ulong   portname[2];       /* N_PORT name */
   ulong   nodename[2];       /* NODE name */
   ubit    pref_DID  : 24;
   ubit    hardAL_PA :  8;
   ulong   rsvd3[247];        /* Must be all one's */
   } WRITE_NV_VAR;

 
/* Structure for MB Command RUN_BIU_DIAG (04) */

typedef struct
   {
   ulong   rsvd1;
   ULP_BDE xmit_bde;
   ULP_BDE rcv_bde;
   } BIU_DIAG_VAR;


/* Structure for MB Command INIT_LINK (05) */

typedef struct 
   {
   ubit    lipsr_AL_PA :  8; /* AL_PA to issue Lip Selective Reset to */
   ubit    rsvd1       : 24;

   uchar   link_flags;
#define FLAGS_LOCAL_LB               0x01 /* link_flags (=1) ENDEC loopback */
#define FLAGS_TOPOLOGY_MODE_LOOP_PT  0x00 /* Attempt loop then pt-pt */
#define FLAGS_TOPOLOGY_MODE_PT_PT    0x02 /* Attempt pt-pt only */
#define FLAGS_TOPOLOGY_MODE_LOOP     0x04 /* Attempt loop only */
#define FLAGS_TOPOLOGY_MODE_PT_LOOP  0x06 /* Attempt pt-pt then loop */
#define FLAGS_LIRP_LILP              0x80 /* LIRP / LILP is disabled */
   uchar   rsvd2;
   uchar   rsvd3;
   uchar   fabric_AL_PA;     /* If using a Fabric Assigned AL_PA */

   } INIT_LINK_VAR;    /* 09-sep-96 djm */


/* Structure for MB Command DOWN_LINK (06) */

typedef struct 
   {
   ulong   rsvd1;
   } DOWN_LINK_VAR;


/* Structure for MB Command CONFIG_LINK (07) */

typedef struct
   {
   ulong   rsvd1;
   ulong   myId;
   ulong   rsvd2;
   ulong   edtov;
   ulong   arbtov;
   ulong   ratov;
   ulong   rttov;
   ulong   altov;
   ulong   crtov;
   ulong   citov;
   } CONFIG_LINK;


/* Structure for MB Command PART_SLIM (08) */

typedef struct
   {
   ubit     numRing  :  8;
   ubit     unused1  : 24;
   RING_DEF ringdef[4];
   } PART_SLIM_VAR;            /* dm 03Jun96 */


/* Structure for MB Command CONFIG_RING (09) */

typedef struct
   {
   ubit    ring      :  4;
   ubit    unused1   :  4;
   ubit    profile   :  8;
   ubit    numMask   :  8;
   ubit    recvNotify:  1;
   ubit    unused2   :  7;

   ushort  maxOrigXchg;
   ushort  maxRespXchg;

   RR_REG  rrRegs[6];

   } CONFIG_RING_VAR;            /* dm 02jul96 */


/* Structure for MB Command RESET_RING (10) */

typedef struct 
   {
   ulong   ring_no;
   } RESET_RING_VAR;        /* dm 11-jul-96 */


/* Structure for MB Command READ_CONFIG (11) */

typedef struct
   {                                         
   ulong   rsvd1;
   ubit    myDid    : 24;
   ubit    topology :  8;
             /* Defines for topology (defined previously) */

   ulong   rsvd2;
   ulong   edtov;
   ulong   arbtov;
   ulong   ratov;
   ulong   rttov;
   ulong   altov;
   ulong   lmt;
#define LMT_RESERVED    0x0    /* Not used */
#define LMT_266_10bit   0x1    /*  265.625 Mbaud 10 bit iface */
#define LMT_532_10bit   0x2    /*  531.25  Mbaud 10 bit iface */
#define LMT_1063_10bit  0x3    /* 1062.5   Mbaud 20 bit iface */

   ulong   crtov;
   ulong   citov;
   } READ_CONFIG_VAR;      /* dm 02jul96 */


/* Structure for MB Command READ_RCONFIG (12) */

typedef struct
   {
   ubit    ring       :  4;
   ubit    rsvd1      :  4;
   ubit    profile    :  8;
   ubit    numMask    :  8;
   ubit    recvNotify :  1;
   ubit    rsvd2      :  7;

   ushort  maxOrig;
   ushort  maxResp;

   RR_REG  rrRegs[6];

   ushort  cmdEntryCnt;
   ushort  cmdRingOffset;

   ushort  rspEntryCnt;
   ushort  rspRingOffset;

   ushort  rsvd3;
   ushort  nextCmdOffset;

   ushort  rsvd4;
   ushort  nextRspOffset;

   } READ_RCONF_VAR;           /* dm 02jul96 */


/* Structure for MB Command READ_SPARM (13) */

typedef struct
   {
   ulong   rsvd1;
   ulong   rsvd2;
   ULP_BDE sp;         /* This BDE points to SERV_PARM structure */
   }  READ_SPARM_VAR;        /* dm 02jul96 */


/* Structure for MB Command READ_STATUS (14) */

typedef struct
   {
   ubit    rsvd1        : 31;
   ubit    clrCounters  :  1;
   ushort  activeXriCnt;
   ushort  activeRpiCnt;
   ulong   xmitByteCnt;
   ulong   rcvbyteCnt;
   ulong   xmitFrameCnt;
   ulong   rcvFrameCnt;
   ulong   xmitSeqCnt;
   ulong   rcvSeqCnt;
   ulong   totalOrigExchanges;
   ulong   totalRespExchanges;
   ulong   rcvPbsyCnt;
   ulong   rcvFbsyCnt;
   } READ_STATUS_VAR;       


/* Structure for MB Command READ_RPI (15) */

typedef struct
   {
   ushort  reqRpi;
   ushort  nextRpi;
   ubit    DID   : 24;
   ubit    rsvd2 :  8;
   ULP_BDE sp;
   }  READ_RPI_VAR;  /* djm 12-dec=97 */


/* Structure for MB Command READ_XRI (16) */

typedef struct
   {
   ushort  nextXri;
   ushort  reqXri;
   ushort  rsvd1;
   ushort  rpi;
   ubit    rsvd2     :  8;
   ubit    DID       : 24;
   ubit    rsvd3     :  8;
   ubit    SID       : 24;
   ulong   rsvd4;
   uchar   seqId;
   uchar   rsvd5;
   ushort  seqCount;
   ushort  oxId;
   ushort  rxId;
   ubit    rsvd6     : 30;      
   ubit    si        :  1;
   ubit    exchOrig  :  1;
   } READ_XRI_VAR; 


/* Structure for MB Command READ_REV (17) */

typedef struct
   {
   ulong   rsvd1;
   ulong   biuRev;
   ulong   smRev;
   ulong   smFwRev;
   ulong   endecRev;

   uchar   fcphLow;
   uchar   fcphHigh;
   ushort  rsvd2;

   } READ_REV_VAR;    /* dm 02jul96 */


/* Structure for MB Command READ_LINK_STAT (18) */

typedef struct
   {  
   ulong   rsvd1;
   ulong   linkFailureCnt;
   ulong   lossSyncCnt;

   ulong   lossSignalCnt;
   ulong   primSeqErrCnt;
   ulong   invalidXmitWord;
   ulong   crcCnt;
   ulong   primSeqTimeout;
   ulong   elasticOverrun;
   ulong   arbTimeout;
   } READ_LNK_VAR;


/* Structure for MB Command REG_LOGIN (19) */

typedef struct
   {
   ushort  rpi;
   ushort  rsvd1;

   ubit    did   : 24;
   ubit    rsvd2 : 8;

   ULP_BDE sp;

   }  REG_LOGIN_VAR;     /* dm 02jul96 */


/* Structure for MB Command UNREG_LOGIN (20) */

typedef struct
   {
   ushort  rpi;
   ushort  rsvd1;
   } UNREG_LOGIN_VAR;


/* Structure for MB Command READ_LA (21) */

typedef struct
   {
   ulong   eventTag;         /* Event tag */

   ubit    attType :  8;
#define AT_RESERVED    0x00  /* Reserved */
#define AT_LINK_UP     0x01  /* Link is up */
#define AT_LINK_DOWN   0x02  /* Link is down */
   ubit    il      :  1;
   ubit    rsvd1   : 23;


   uchar   topology;
#define TOPOLOGY_PT_PT 0x01  /* Topology is pt-pt / pt-fabric */
#define TOPOLOGY_LOOP  0x02  /* Topology is FC-AL */
   uchar   lipType;
#define LT_PORT_INIT    0x00 /* An L_PORT initing (F7, AL_PS) */
#define LT_PORT_ERR     0x01 /* Err @L_PORT rcv'er (F8, AL_PS) */
#define LT_RESET_APORT  0x02 /* Lip Reset of some other port */
#define LT_RESET_MYPORT 0x03 /* Lip Reset of my port */
   uchar   lipAlPs;
   uchar   granted_AL_PA;

   ULP_BDE lilpBde;         /* This BDE points to a 128 byte buffer to */
                            /* store the LILP AL_PA position map into */
   } READ_LA_VAR;   /* dm 28-aug-1996 */


/* Structure for MB Command CLEAR_LA (22) */

typedef struct
   {
   ulong   eventTag;         /* Event tag */
   ulong   rsvd1;
   } CLEAR_LA_VAR;     /* dm 11-july-1996 */


/* Struct for MB Command DUMP (23) */

typedef struct
   {
   ubit	    type           : 4;    /* This bit selects the type of data to dump */
#define DUMP_CONGURATION_DATA 0x2
   ubit	    version        : 1;    /* 0 for SLI-1;  1 for SLI-2 */
   ubit     offset_present : 1;    /* 0 for SLI-1;  see spec for SLI-2 */
   ubit	    ack		   : 1;	   /* 0 for SLI-1; */
   ubit	    rsvd1	   : 25;   /* MBZ */
   ushort   region_id;		   /* region number (0 to 16) to dump */
   ushort   entry_index;	   /* starting offset within the entry */
   ulong    rsvd2;
   ulong    blen;		   /* C: words requested;  R: bytes returned */
   ulong    data;		   /* R: word 0 of response data */
   } DUMP_VAR;             /* dm 25-sep-1999 */


typedef struct
   {
   ubit	    type	   : 4;	/* operation being requested */
#define UPDATE_CFG_INITIALIZE  0x1
#define UPDATE_CFG_UPDATE      0x2
#define UPDATE_CFG_CLEAN_UP    0x3
   ubit	    version	   : 1;	/* 0 for SLI-1 */
   ubit	    offset_present : 1;	/* 0 for SLI-1 */
   ubit	    ack		   : 1; /* 0 for SLI-1 */
   ubit	    rsvd1	   : 1; /* MBZ */
   uchar    proctype;		/* type or region being initailized */
   ushort   rsvd2;		/* MBZ */
   ushort   region_id;	        /* region number (0 to 16) to dump */
   ushort   entry_length;       /* byte count of region */
   ulong    response_info;	/* error codes */
#define UPDATE_CFG_INV_REGION  -2
#define UPDATE_CFG_NO_REGION   -3
#define UPDATE_CFG_BAD_REGION  -4
#define UPDATE_CFG_TOO_LONG   -11
#define UPDATE_CFG_NO_UNUSED  -12
#define UPDATE_CFG_BAD_BMAP   -13
#define UPDATE_CFG_WRITE_FAIL -14
#define UPDATE_CFG_NO_SAVE    -15
#define UPDATE_CFG_BAD_ERASE  -16
#define UPDATE_CFG_NOT_ERASED -17
#define UPDATE_CFG_BAD_LENGTH -18
#define UPDATE_CFG_BAD_DATA   -19
   ulong    byte_count;		/* bytes of data in words 5-31 (SLI-1) */
   ulong    data[];		/* configuration data (SLI-1) */
   } UPDATE_CFG_VAR;   /* dm 25-sep-1999 */

/* Union of all Mailbox Command types */

typedef union
   {
   ulong           varWords[31];
   LOAD_SM_VAR     varLdSM;       /* cmd =  1 (LOAD_SM)        */
#if 0
   READ_NV_VAR     varRDnvp;      /* cmd =  2 (READ_NVPARMS)   */
   WRITE_NV_VAR    varWTnvp;      /* cmd =  3 (WRITE_NVPARMS)  */
#endif
   BIU_DIAG_VAR    varBIUdiag;    /* cmd =  4 (RUN_BIU_DIAG)   */
   INIT_LINK_VAR   varInitLnk;    /* cmd =  5 (INIT_LINK)      */
   DOWN_LINK_VAR   varDwnLnk;     /* cmd =  6 (DOWN_LINK)      */
   CONFIG_LINK     varCfgLnk;     /* cmd =  7 (CONFIG_LINK)    */
   PART_SLIM_VAR   varSlim;       /* cmd =  8 (PART_SLIM)      */
   CONFIG_RING_VAR varCfgRing;    /* cmd =  9 (CONFIG_RING)    */
   RESET_RING_VAR  varRstRing;    /* cmd = 10 (RESET_RING)     */ 
   READ_CONFIG_VAR varRdConfig;   /* cmd = 11 (READ_CONFIG)    */ 
   READ_RCONF_VAR  varRdRConfig;  /* cmd = 12 (READ_RCONFIG)   */ 
   READ_SPARM_VAR  varRdSparm;    /* cmd = 13 (READ_SPARM)     */
   READ_STATUS_VAR varRdStatus;   /* cmd = 14 (READ_STATUS)    */
   READ_RPI_VAR    varRdRPI;      /* cmd = 15 (READ_RPI)       */
   READ_XRI_VAR    varRdXRI;      /* cmd = 16 (READ_XRI)       */
   READ_REV_VAR    varRdRev;      /* cmd = 17 (READ_REV)       */
   READ_LNK_VAR    varRdLnk;      /* cmd = 18 (READ_LNK_STAT)  */
   REG_LOGIN_VAR   varRegLogin;   /* cmd = 19 (REG_LOGIN)      */
   UNREG_LOGIN_VAR varUnregLogin; /* cmd = 20 (UNREG_LOGIN)    */
   READ_LA_VAR     varReadLA;     /* cmd = 21 (READ_LA)        */
   CLEAR_LA_VAR    varClearLA;    /* cmd = 22 (CLEAR_LA)       */
   DUMP_VAR	   varDump;	  /* cmd = 23 (DUMP)           */
   UPDATE_CFG_VAR  varUpdateCfg;  /* cmd = 24 (UPDATE_CFG)     */
   } MAILVARIANTS;
    
#define MAILVARIANTS_SIZE    sizeof(MAILVARIANTS)
#define MAILBOX_CMD_WSIZE    32
#define MAILBOX_CMD_BSIZE    MAILBOX_CMD_WSIZE*4
#define IOCB_WSIZE	     8
#define IOCB_BSIZE	     IOCB_WSIZE*4
#define NVPARMS_CMD_BSIZE    1028


typedef struct
   {

   ubit    mbxOwner      :  1;    /* Low order bit first word */
   ubit    mbxHc         :  1;
   ubit    mbxReserved   :  6;
   uchar   mbxCommand;
   ushort  mbxStatus;

   MAILVARIANTS un;
/*
 * dm 04Sep96
   RINGS   mbxCring[4];
   ulong   mbxUnused[24];
*/
   } MAILBOX, *PMAILBOX;   /* dm 30May96 */
#define M_MBXOWNER  1
#define M_MBXHC     2



/*
 *    End Structure Definitions for Mailbox Commands
 */


/*
 *    Begin Structure Definitions for IOCB Commands
 */

typedef struct
   {
   uchar   statAction;
             /* FBSY reason codes */
#define FBSY_RSN_MASK   0xF0  /* Rsn stored in upper nibble */
#define FBSY_FABRIC_BSY 0x10  /* F_bsy due to Fabric BSY */
#define FBSY_NPORT_BSY  0x30  /* F_bsy due to N_port BSY */

             /* PBSY action codes */
#define PBSY_ACTION1    0x01  /* Sequence terminated - retry */
#define PBSY_ACTION2    0x02  /* Sequence active - retry */
         
             /* P/FRJT action codes */
#define RJT_RETRYABLE   0x01  /* Retryable class of error */
#define RJT_NO_RETRY    0x02  /* Non-Retryable class of error */

   uchar   statRsn;
             /* LS_RJT reason codes defined in LS_RJT structure */

             /* P_BSY reason codes */
#define PBSY_NPORT_BSY  0x01  /* Physical N_port BSY */
#define PBSY_RESRCE_BSY 0x03  /* N_port resource BSY */
#define PBSY_VU_BSY     0xFF  /* See VU field for rsn */

             /* P/F_RJT reason codes */
#define RJT_BAD_D_ID    0x01  /* Invalid D_ID field */
#define RJT_BAD_S_ID       0x02  /* Invalid S_ID field */
#define RJT_UNAVAIL_TEMP   0x03  /* N_Port unavailable temp. */
#define RJT_UNAVAIL_PERM   0x04  /* N_Port unavailable perm. */
#define RJT_UNSUP_CLASS    0x05  /* Class not supported */
#define RJT_DELIM_ERR      0x06  /* Delimiter usage error */
#define RJT_UNSUP_TYPE     0x07  /* Type not supported */
#define RJT_BAD_CONTROL    0x08  /* Invalid link conrtol */
#define RJT_BAD_RCTL       0x09  /* R_CTL invalid */
#define RJT_BAD_FCTL       0x0A  /* F_CTL invalid */
#define RJT_BAD_OXID       0x0B  /* OX_ID invalid */
#define RJT_BAD_RXID       0x0C  /* RX_ID invalid */
#define RJT_BAD_SEQID      0x0D  /* SEQ_ID invalid */
#define RJT_BAD_DFCTL      0x0E  /* DF_CTL invalid */
#define RJT_BAD_SEQCNT     0x0F  /* SEQ_CNT invalid */
#define RJT_BAD_PARM       0x10  /* Param. field invalid */
#define RJT_XCHG_ERR       0x11  /* Exchange error */
#define RJT_PROT_ERR       0x12  /* Protocol error */
#define RJT_BAD_LENGTH     0x13  /* Invalid Length */
#define RJT_UNEXPECTED_ACK 0x14  /* Unexpected ACK */
#define RJT_LOGIN_REQUIRED 0x16  /* Login required */
#define RJT_TOO_MANY_SEQ   0x17  /* Excessive sequences */
#define RJT_XCHG_NOT_STRT  0x18  /* Exchange not started */
#define RJT_UNSUP_SEC_HDR  0x19  /* Security hdr not supported */
#define RJT_UNAVAIL_PATH   0x1A  /* Fabric Path not available */
#define RJT_VENDOR_UNIQUE  0xFF  /* Vendor unique error */

             /* BA_RJT reason codes */
#define BARJT_BAD_CMD_CODE 0x01  /* Invalid command code */ 
#define BARJT_LOGICAL_ERR  0x03  /* Logical error */ 
#define BARJT_LOGICAL_BSY  0x05  /* Logical busy */ 
#define BARJT_PROTOCOL_ERR 0x07  /* Protocol error */ 
#define BARJT_VU_ERR       0xFF  /* Vendor unique error */ 

   uchar   statBaExp;
             /* LS_RJT reason explanation defined in LS_RJT structure */

             /* BA_RJT reason explanation */
#define BARJT_EXP_INVALID_ID  0x01  /* Invalid OX_ID/RX_ID */
#define BARJT_EXP_ABORT_SEQ   0x05  /* Abort SEQ, no more info */

   uchar   statLocalError;
             /* FireFly localy detected errors */
#define IOERR_SUCCESS                 0x00
#define IOERR_MISSING_CONTINUE        0x01
#define IOERR_SEQUENCE_TIMEOUT        0x02
#define IOERR_INTERNAL_ERROR          0x03
#define IOERR_INVALID_RPI             0x04
#define IOERR_NO_XRI                  0x05
#define IOERR_ILLEGAL_COMMAND         0x06
#define IOERR_XCHG_DROPPED            0x07
#define IOERR_ILLEGAL_FIELD           0x08
#define IOERR_BAD_CONTINUE            0x09
#define IOERR_TOO_MANY_BUFFERS        0x0A
#define IOERR_RCV_BUFFER_WAITING      0x0B
#define IOERR_NO_CONNECTION           0x0C
#define IOERR_TX_DMA_FAILED           0x0D
#define IOERR_RX_DMA_FAILED           0x0E
#define IOERR_ILLEGAL_FRAME           0x0F
#define IOERR_EXTRA_DATA              0x10
#define IOERR_NO_RESOURCES            0x11
#define IOERR_NO_CLASS                0x12
#define IOERR_ILLEGAL_LENGTH          0x13
#define IOERR_UNSUPPORTED_FEATURE     0x14
#define IOERR_ABORT_IN_PROGRESS       0x15
#define IOERR_ABORT_REQUESTED         0x16
#define IOERR_RECEIVE_BUFFER_TIMEOUT  0x17
#define IOERR_LOOP_OPEN_FAILURE	      0x18
#define IOERR_RING_RESET              0x19
#define IOERR_LINK_DOWN               0x1A
#define IOERR_CORRUPTED_DATA          0x1B
#define IOERR_CORRUPTED_RPI           0x1C
#define IOERR_OUT_OF_ORDER_DATA       0x1D
#define IOERR_OUT_OF_ORDER_ACK        0x1E
#define IOERR_DUP_FRAME               0x1F
#define IOERR_LINK_CONTROL_FRAME      0x20
   } PARM_ERR;  

typedef union 
   {
   struct
      {
      uchar   Fctl;                      /* Bits 0-7 of IOCB word 5 */
      uchar   Dfctl;                     /* DF_CTL field */
      uchar   Type;                      /* TYPE field */
      uchar   Rctl;                      /* R_CTL field */
#define BC      0x02     /* Broadcast Received */
#define SI      0x04     /* Sequence Initiative */
#define LS      0x80     /* Last Sequence */
      } hcsw;
   ulong  reserved;
   } WORD5;


/* IOCB Command template for a generic response */
typedef struct
   {
   ulong    reserved[4];
   PARM_ERR perr;
   } GENERIC_RSP;


/* IOCB Command template for XMIT / XMIT_BCAST / RCV_SEQUENCE / XMIT_ELS */
typedef struct
   {
   ULP_BDE xrsqbde[2];
   ulong   xrsqRo;              /* Starting Relative Offset */
   WORD5   w5;                  /* Header control/status word */
   } XR_SEQ_FIELDS;

/* IOCB Command template for ELS_REQUEST */
typedef struct
   {
   ULP_BDE  elsReq;
   ULP_BDE  elsRsp;
   ubit     word4Rsvd :  7;
   ubit     fl        :  1;
   ubit     myID      : 24;
   ubit     word5Rsvd :  8;
   ubit     remoteID  : 24;
   } ELS_REQUEST;

/* IOCB Command template for RCV_ELS_REQ */
typedef struct
   {
   ULP_BDE  elsReq[2];
   ulong    parmRo;
   ubit     remoteID  : 24;
   ubit     word5Rsvd :  8;
   } RCV_ELS_REQ;      /* djm 20-jul-97 */

/* IOCB Command template for ABORT / CLOSE_XRI */
typedef struct
   {
   ulong    rsvd[3];
   ulong    abortType;
#define ABORT_TYPE_ABTX  0x00000000
#define ABORT_TYPE_ABTS  0x00000001
   ulong    parm;
   ushort   abortContextTag;      /* ulpContext from command to abort/close */
   ushort   abortIoTag;           /* ulpIoTag from command to abort/close */
   } AC_XRI;

/* IOCB Command template for GET_RPI */
typedef struct
   {
   ulong    rsvd[4];
   ulong    parmRo;
   ubit     word5Rsvd :  8;
   ubit     remoteID  : 24;
   } GET_RPI;

/* IOCB Command template for all FCP Initiator commands */
typedef struct
   {
   ULP_BDE fcpi_cmnd;           /* FCP_CMND payload descriptor */
   ULP_BDE fcpi_rsp;            /* Rcv buffer */
   ulong   fcpi_parm;
   ulong   fcpi_XRdy;           /* transfer ready for IWRITE */
   } FCPI_FIELDS;

/* IOCB Command template for all FCP Target commands */
typedef struct
   {
   ULP_BDE fcpt_Buffer[2];      /* FCP_CMND payload descriptor */
   ulong   fcpt_Offset;
   ulong   fcpt_Length;         /* transfer ready for IWRITE */
   } FCPT_FIELDS;

typedef struct _IOCB
   {       /* IOCB structure */
   union
      {
      GENERIC_RSP    grsp;            /* Generic response */
      XR_SEQ_FIELDS  xrseq;           /* XMIT / BCAST / RCV_SEQUENCE cmd */
      ULP_BDE        cont[3];         /* up to 3 continuation bdes */
      ELS_REQUEST    elsreq;          /* ELS_REQUEST template */
      RCV_ELS_REQ    rcvels;          /* RCV_ELS_REQ template */
      AC_XRI         acxri;           /* ABORT / CLOSE_XRI template */
      GET_RPI        getrpi;          /* GET_RPI template */
      FCPI_FIELDS    fcpi;            /* FCP Initiator template */
      FCPT_FIELDS    fcpt;            /* FCP target template */
      ulong  ulpWord[IOCB_WORD_SZ-2]; /* generic 6 'words' */
      } un;
   union
      {
      struct 
         {
         ushort  ulpIoTag;                  /* Low  order bits word 6 */
         ushort  ulpContext;                /* High order bits word 6 */
         } t1;
      struct 
         {

         uchar   ulpIoTag1;                 /* Low  order bits word 6 */
         uchar   ulpIoTag0;                 /* Low  order bits word 6 */

         ushort  ulpContext;                /* High order bits word 6 */

         } t2;
      } un1;
#if 0
#define ulpContext un1.t1.ulpContext
#define ulpIoTag   un1.t1.ulpIoTag
#define ulpIoTag0  un1.t2.ulpIoTag0
#define ulpIoTag1  un1.t2.ulpIoTag1
#endif
   ubit    ulpOwner      :  1;        /* Low order bit word 7 */
   ubit    ulpLe         :  1;
   ubit    ulpBdeCount   :  2;
   ubit    ulpStatus     :  4;
   ubit    ulpCommand    :  8;
   ubit    ulpClass      :  3;
   ubit    ulpIr         :  1;
   ubit    ulpPU         :  2;
   ubit    ulpRsvdbits   :  2;
   ubit    ulpRsvdByte   :  8;

#define PARM_UNUSED        0  /* PU field (Word 4) not used */
#define PARM_REL_OFF       1  /* PU field (Word 4) = R. O. */

#define CLASS1             0  /* Class 1 */
#define CLASS2             1  /* Class 2 */
#define CLASS3             2  /* Class 3 */
#define CLASS_FCP_INTERMIX 7  /* FCP Data->Cls 1, all else->Cls 2 */

#define IOSTAT_SUCCESS         0x0
#define IOSTAT_FCP_RSP_ERROR   0x1
#define IOSTAT_REMOTE_STOP     0x2
#define IOSTAT_LOCAL_REJECT    0x3
#define IOSTAT_NPORT_RJT       0x4
#define IOSTAT_FABRIC_RJT      0x5
#define IOSTAT_NPORT_BSY       0x6
#define IOSTAT_FABRIC_BSY      0x7
#define IOSTAT_INTERMED_RSP    0x8
#define IOSTAT_LS_RJT          0x9
#define IOSTAT_BA_RJT          0xA
#define IOSTAT_CMD_REJECT      0xB  /* same as 3, but exchange still active */

   } IOCB, *PIOCB;         /* dm 02Jul96 */

typedef struct
   {
   IOCB   iocb;		/* iocb entry */
   uchar  *q;		/* ptr to next iocb entry */
   uchar  *bp;		/* ptr to data buffer structure */
   uchar  *info;	/* ptr to data information structure */
   } IOCBQ;

typedef struct
   {
   ulong mb[MAILBOX_CMD_WSIZE];
   uchar  *q;
   uchar  *bp;		/* ptr to data buffer structure */
   } MAILBOXQ;

/* Given a pointer to the start of the ring, and the slot number of
 * the desired iocb entry, calc a pointer to that entry.
 * (assume iocb entry size is 32 bytes)
 */
#define IOCB_ENTRY(ring,slot) ((IOCB *)(((uchar *)ring) + (((ulong)slot)<< 5)))

/*
 *    End Structure Definitions for IOCB Commands
 */

typedef struct
   {
   MAILBOX mbx;
   IOCB    IOCBs[MAX_BIOCB];
   } SLIM;


#endif




/*-------------------------------------------------------------------------*
 *      Common Transport (CT) Structure Definitions                        *
 *-------------------------------------------------------------------------*/

#define CT_ULP_TYPE             0x20

/* CT R_CTL values */
#define CT_R_CTL_REQUEST        0x02
#define CT_R_CTL_RESPONSE       0x03

#define CT_REVISION             1

typedef struct ct_hdr {
    union {
        U32         word;
        struct {
            U32     inId    : 24;  /* Unused address */
            U32     ctRev   :  8;  /* Common Transport revision */
        }       bits;
    }       uId;
    uchar   fcsType;
    uchar   fcsSubType;
    uchar   options;
    uchar   rsvd1;
    union {
        U32         word;
        struct {
            U32     size    : 16;
            U32     cmdRsp  : 16;
        }       bits;
    }       usc;
    uchar   rsvd2;
    uchar   reasonCode;
    uchar   explanation;
    uchar   vendorUnique;

} CT_HDR;


/* last of port id bit identifier */
#define CT_NS_PORT_ID_LAST_IND			0x80

/*
 * to avoid endianness, this object is defined as a bit array.
 * each "word" is 4 bytes, with :
 *    byte 0 being word bits 31-24,
 *    byte 1 being word bits 23-16,
 *    byte 2 being word bits 15-8,
 *    byte 3 being word bits 7-0
 */
typedef struct ct_fc4_types {
    uchar   byte[32];
} CT_FC4_TYPE;

/*
 * FsType Definitions
 */
/* Vendor Unique(16)                    0x00-0x1F   */
/* Reserved                             0x20-0xF7   */
#define CT_ALIAS_SERVER                 0xF8
/* Reserved                             0xF9        */
#define CT_MANAGEMENT_SERVICE           0xFA
#define CT_TIME_SERVICE                 0xFB
#define CT_DIRECTORY_SERVICE            0xFC
#define CT_FABRIC_CONTROLLER_SERVICE    0xFD
/* Reserved                             0xFE-0xFF   */

/*
 * Directory Service Subtypes
 */
#define CT_DIRECTORY_NAME_SERVER        0x02

/*
 * CT Response Codes
 */
#define CT_NON_FS_IU                    0x0000
/*      CT_REQUEST_FS_IU                0x0001-0x7FFF   */
#define CT_RESPONSE_FS_RJT              0x8001
#define CT_RESPONSE_FS_ACC              0x8002

/*
 * CT_RESPONSE_FS_RJT Reason Codes
 */
#define CT_INVALID_COMMAND              0x01
#define CT_INVALID_VERSION              0x02
#define CT_LOGICAL_ERROR                0x03
#define CT_INVALID_IU_SIZE              0x04
#define CT_LOGICAL_BUSY                 0x05
#define CT_PROTOCOL_ERROR               0x07
#define CT_UNABLE_TO_PERFORM_REQ        0x09
#define CT_REQ_NOT_SUPPORTED            0x0b
#define CT_VENDOR_UNIQUE                0xff

/*
 * Name Server CT_UNABLE_TO_PERFORM_REQ Explanations
 */
#define CT_NO_ADDTL_EXPLANATION         0x00
#define CT_NO_PORT_ID                   0x01
#define CT_NO_PORT_NAME                 0x02
#define CT_NO_NODE_NAME                 0x03
#define CT_NO_CLASS_OF_SERVICE          0x04
#define CT_NO_IP_ADDRESS                0x05
#define CT_NO_IPA                       0x06
#define CT_NO_FC4_TYPES                 0x07
#define CT_NO_SYMBOLIC_PORT_NAME        0x08
#define CT_NO_SYMBOLIC_NODE_NAME        0x09
#define CT_NO_PORT_TYPE                 0x0A
#define CT_ACCESS_DENIED                0x10
#define CT_INVALID_PORT_ID              0x11
#define CT_DATABASE_EMPTY               0x12

/*
 * Name Server FS_Subtype Codes
 */
/*      CNTS_SUBTYPE_RESERVED           0x01    */
#define CTNS_SUBTYPE_NAME_SERVICE       0x02
/*      CNTS_SUBTYPE_FC4_SPECIFIC       0x80-EF */

/*
 * Name Server Command Codes
 */
#define CTNS_GA_NXT                     0x0100
#define CTNS_GPN_ID                     0x0112
#define CTNS_GNN_ID                     0x0113      
#define CTNS_GCS_ID                     0x0114
#define CTNS_GFT_ID                     0x0117
#define CTNS_GSPN_ID                    0x0118
#define CTNS_GPT_ID                     0x011A
#define CTNS_GID_PN                     0x0121
#define CTNS_GID_NN                     0x0131
#define CTNS_GIP_NN                     0x0135
#define CTNS_GIPA_NN                    0x0136
#define CTNS_GSNN_NN                    0x0139
#define CTNS_GNN_IP                     0x0153
#define CTNS_GIPA_IP                    0x0156
#define CTNS_GID_FT                     0x0171
#define CTNS_GID_PT                     0x01A1
#define CTNS_RPN_ID                     0x0212
#define CTNS_RNN_ID                     0x0213
#define CTNS_RCS_ID                     0x0214
#define CTNS_RFT_ID                     0x0217
#define CTNS_RSPN_ID                    0x0218
#define CTNS_RPT_ID                     0x021A
#define CTNS_RIP_NN                     0x0235
#define CTNS_RIPA_NN                    0x0236
#define CTNS_RSNN_NN                    0x0239
#define CTNS_DA_ID                      0x0300

/*
 * CT Port Types
 */
#define CTPT_UNIDENTIFIED               0x00
#define CTPT_N_PORT                     0x01
#define CTPT_NL_PORT                    0x02
#define CTPT_FNL_PORT                   0x03
#define CTPT_NX_PORT                    0x7F
#define CTPT_F_PORT                     0x81
#define CTPT_FL_PORT                    0x82
#define CTPT_E_PORT                     0x84


