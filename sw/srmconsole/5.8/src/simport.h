/*
 * Copyright (C) 1992,1993,1994 by
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
 * Abstract:	Simport Definitions
 *
 * Author:	Judith Gold
 *
 * Creation Date: 20-May-1994 
 * Modifications:
 *
 *     15-JUL-1992           Mike Mackay 	Provided to SIMport architect
 *
 *     16-JUL-1992           Rich Whalen	Updated Queue Carrier format
 *						Adapter State Set format
 *
 *     24-AUG-1992           Rich Whalen	Corrected typedef syntax,
 *                                  		utiny changed to unsigned char
 *
 *     26-OCT-1992           Rich Whalen	Miscellaneous changes to insure
 *                                              agreement with SIMport 
 *						architecture document
 *
 *     12-Mar-1993           Rich Whalen	Added definitions for 
 *						Environment fault AEN messages
 *
 *     10-Dec-1993           Rich Whalen        Added Target Mode definitions
 *
 *     18-Jan-1994           Rich Whalen        Private Data Area updates
 *
 *     10-Feb-1994	     Judy Gold 		Ported to the console.
 */

#ifndef SIMPORT_H
#define SIMPORT_H

#define PATZA	0		/* proto module */

#define QUEUE_BUFFER_LEN	256	/* TEMP TEMP */
#define SIM_INIT_RETRIES	5
#define NO_ASR_ERRORS		2
#define SIM_ENABLE		1
#define SIM_DISABLE		2
#define NO_STOPPER_DAQ		0x00000001 /* indicates a queue entry other */
					   /* than a stopper 	            */
						 

/************************************************************
**  MODULE DATA STRUCTURE:  Miscellaneous Structures
**
**     The following are miscellaneous structures used in the SIMport
**     modules.
**-----------------------------------------------------------
**  DESCRIPTION:
**
**
*************************************************************
**/

/*
** Host Virtual Address
*/
struct VA
{
    unsigned int   lo;
    unsigned int   hi;
};
/*
** Host Physical Address
*/
struct PA
{
    unsigned int   lo;
    unsigned int   hi;
};
/*
** unsigned 64 bit integer type
*/
typedef struct
{
    unsigned int   lo;
    unsigned int   hi;
} uxlong;

/*
** Compressed version of Execute SCSI IO (part of the QProcSlot)
*/
struct PackedSCSIIO
{
    unsigned int  DataXferLen;
    unsigned char  SenseBuffLen;
    unsigned char  CDBLen;
    unsigned char  CDB[12];
    unsigned int  CmdTimOut;
    unsigned short VUFlags;
    unsigned char  QTag;
};

/**********************************************************
**  MODULE DATA STRUCTURE:  QueueCarrier
**
**     The QueueCarrier is the SIMport defined data structures used to
**     form the interface queues between the host and the adapter.
**---------------------------------------------------------
**  DESCRIPTION:
**
**     The QueueCarrier is used to pass queue buffers between the host and
**     the adapter. It contains a link field which is used to form a
**     one-way linked list. It contains a pointer to a queue buffer which
**     must be formatted as a CAM or SIMport CCB command.  It also contains
**     queue buffer and queue carrier token fields which are host virtual
**     addresses. In addition, for adapter internal use only, the CarPtr
**     field is the physical address of the queue carrier. Note that the
**     CarPtr field is not part of the host resident queue carrier.
***********************************************************
**/

struct FLAGS 
{
    unsigned int  SBZpadding: 14;
    unsigned int  QueuePriority: 1;
    unsigned int  ReturnStatusFormat: 1;
};

/*
** CAM Command Header Connect ID.  The ConnId is a field in the
** CCBHeader used to address a bus, device, and LUN.
*/
struct ConnID
{
    unsigned char           rsvd;
    unsigned char           channel;
    unsigned char           target;
    unsigned char           lun;
};

	

/*
** SIMport queue carrier
*/
struct QueueCarrier
{
    struct QueueCarrier *next_ptr;   /* pointer to next carrier */
    unsigned int next_ptr_hi;
    unsigned int  q_buf_ptr;	     /* pointer to associated QueueBuffer */
    unsigned int q_but_ptr_hi;
    unsigned int  q_buf_token;       /* same, but VA if necessary */
    unsigned int q_buf_token_hi;
    unsigned int  car_token;
    unsigned int car_token_hi;
    unsigned char func_code;   	     /* CAM or SIM function code */
    unsigned char status;            /* status of CAM or SIM cmd */
    struct FLAGS  flags;	     /* CAM queue priority flag */
    struct ConnID CID;		     
};

/************************************************************
**  MODULE DATA STRUCTURE:  CAMHeader
**
**     The CAMHeader is part of a CAM defined CCB command. It defines the
**     fields common to all CCB commands. 
**-----------------------------------------------------------
**  DESCRIPTION:
**
**     The CCBAddr is the host virtual address of the CCB. The CCBLen is
**     the length in bytes of the CCB. The function is the function code
**     for the command or message. The CAMStatus is where the command
**     complete status is posted. The CID contains the target address for
**     the command. The CAMFlags is a set of command specific flags. Refer
**     to the CAM specification for additional details.
*************************************************************
**/
struct CAM_FLAGS
 {
	/* byte 0 */
	unsigned int	cdb_pointer: 1;
	unsigned int 	tqae: 1;
	unsigned int 	link_cdb: 1;
	unsigned int 	dcoc: 1;
	unsigned int 	sg: 1;
	unsigned int  	dis_autos: 1;
	unsigned int    dir: 2;
        /* byte 1 */
	unsigned int    cmd_done: 1; /* defined as reserved in CAM spec */
	unsigned int	eng_sync: 1; 
	unsigned int 	simq_fz_dis: 1;
	unsigned int 	simq_fz: 1;
	unsigned int 	simq_prty: 1;
	unsigned int 	dst: 1;
	unsigned int 	ist: 1;
	unsigned int 	dd: 1;
	/* byte 2*/
	unsigned int    sglp: 1;
	unsigned int 	coc: 1;
	unsigned int 	next_ccb: 1;
	unsigned int 	msg_buff: 1;
	unsigned int    sns_buff: 1;
	unsigned int 	sgld: 1;
	unsigned int 	cdbptr: 1;
	unsigned int 	sglst_eng: 1;
	/* byte 3*/
	unsigned int 	dasr: 1;
	unsigned int 	dad: 1;
	unsigned int 	tccba: 1;
	unsigned int 	pcm_not_ht: 1;
	unsigned int	sim_error: 1;   /* defined as reserved in CAM spec */
	unsigned int 	mbv_tio: 1;
	unsigned int 	sbv_dm: 1;
	unsigned int 	dbv_ss: 1;
 };


/*
** CAM Command Header
*/
struct CAMHeader
{
    /*
    ** note that the CCBAddr and the CCBLen is not required
    ** when the CCBHeader resides in a QProcSlot or an ImmedProcSlot.
    */
    struct CAMHeader   *CCB_addr;
    unsigned short      CCB_len;
    unsigned char       function;
    unsigned char       status;
    struct ConnID   	conn_id;
    struct CAM_FLAGS    cam_flags;
};


/*************************************************************************
**     MODULE DATA STRUCTURE:  SIMport Command/Message Formats
**
**     The following structures define the SIMport command and message
**     formats. SIMPortBodies is a union of all the SIMport specific command
**     and message structure definitions. Subsequently, are the definition
**     of each SIMport command and message. Included are parameter
**     definitions where appropriate.
*************************************************************************
**/

/*
** SIMport vendor unique function codes definitions
*/
#define SPO_VU_SET_ADAP_STATE   0x80
#define SPO_VU_SET_PARAM        0x81
#define SPO_VU_SET_CHAN_STATE   0x82
#define SPO_VU_SET_DEV_STATE    0x83
#define SPO_VU_VER_ADAP_SANITY  0x84
#define SPO_VU_READ_CNTRS       0x85
#define SPO_VU_BSD_REQUEST      0x86	  /* an unsolicited message */
#define SPO_VU_BUS_RESET_REQ    0x87      /* an unsolicited message */
#define SPO_VU_UNSOL_RESEL      0x88      /* an unsolicited message */
#define SPO_VU_CHAN_DISABLED    0x89      /* an unsolicited message */
#define SPO_VU_DEV_DISABLED     0x8A      /* an unsolicited message */
#define SPO_VU_TARGET_EVENT	0x8B
#define SPO_VU_SET_TARGET_STATE	0x8c

/*
**  values for QueueCarrier status field
*/
#define        SPO_CHAN_HW_ERR    5	/* hardware error on SCSI channel */
#define	       SPO_DEV_RESET	  4	/* bus device reset sent */
#define        SPO_RESET_REQ_NO	  3	/* host denied adapter's reset bus req*/
#define        SPO_BUS_RESET	  2     /* bus reset detected on SCSI bus */
#define        SPO_SUCCESS        1	/* successful completion of command */
#define        SPO_INV_COMMAND    0	/* unsupported or bad format cmd */
#define        SPO_NOT_ENABLED    0xff	/* adap, chan, device, or func not ena*/
#define        SPO_NOT_DISABLED   0xfe    /* adapter not in disabled state */
#define        SPO_HOST_MEM       0xfd	/* host must alloc memory segments */
#define        SPO_HOST_FQE       0xfc	/* host must alloc free queue elements*/
#define        SPO_INT_HOLDOFF    0xfb    /* timer not supported or invalid */
#define        SPO_HOST_SG        0xfa	/* unsufficient host memory alloc'ed */
#define        SPO_INV_CHAN_ID    0xf9	/* invalid channel id */
#define        SPO_INV_NODE_ID    0xf8    /* invalid bus node id, or no soft set*/
#define        SPO_CANT_DISABLE   0xf7    /* can't disable device channel */
#define        SPO_INV_OFFSET     0xf6	/* offset not within buffer */
#define        SPO_INV_CCB        0xf5   /* invalid ccb address */
#define        SPO_INV_BUF_ID     0xf4   /* invalid buf_id field value */
#define        SPO_NO_RESET       0xf3   /* adap shouldn't reset scsi bus */
#define	       SPO_NO_EVENT	  0xf2	/* no target event in progress for lun*/


/*
** SIMport Set Adapter State command
*/
struct SPO_SetAdap_State
{
    unsigned char   state;
    unsigned char   unused[7];
};
/*
** SIMport Adapter State Set response message
*/

struct SPO_AdapStateSet
{
    unsigned char         state;
    unsigned char         n_host_sg;
    struct
    {
        unsigned int    int_holdoff: 1;
        unsigned int    chan_node: 1;
        unsigned int    link_BSM: 1;
        unsigned int    BSD_req: 1;
	unsigned int    tgt_host: 1;
	unsigned int    tgt_phase_cog: 1;
	unsigned int    unused: 10;
    }flags;
    unsigned short        n_adap_qc;
    unsigned char         ka_time;
    unsigned char         n_freeq;
    unsigned char         n_chan;
    unsigned char         xfer_align;
    unsigned char         n_SAC;
    unsigned char         SBZpadding[5];
    unsigned char         nodes_on_chan[16];
} ;


/*
**  values for the AdapStateSet XferAlign field
*/
#define    SPO_XFER_ALIGN_BYTE        0
#define    SPO_XFER_ALIGN_WORD        1
#define    SPO_XFER_ALIGN_LONGWORD    2
#define    SPO_XFER_ALIGN_QUADWORD    3
#define    SPO_XFER_ALIGN_OCTAWORD    4


/************************************************************************
**  MODULE DATA STRUCTURE:  BSD
**
**     The BSD (Buffer Segment Descriptor) defines either a host memory
**     segment or a BSM.
**-----------------------------------------------------------------------
**  DESCRIPTION:
**
**     The BSD is a data structure used to contain the physical address
**     and size of a contiguous segment of host memory. The BPType flag
**     indicates whether the format of the memory segment is that of a
**     host memory segment or of a SIMport Buffer Segment Map (BSM) . A
**     BSM is a list of BSDs and is used to address a host buffer that
**     consists of multiple host memory segments. The ByteCount is the
**     number of bytes in the segment or in the BSM. The remaining fields
**     contain the host address bits of the memory segment or of the BSM.
*************************************************************************
**/

/*
** SIMport Buffer Segment Descriptor
*/
struct BSD
{
    union
    {
	u_int	BP;			/* access BufPtrLo as 32 bits */
	struct
	{
	    unsigned	BPType: 2;	/* access BufPtrLo type bits only */
	    unsigned	BPBits: 30;	/* access BufPtrLo ptr bits only */
	}		b;
    }			BufPtrLo;	/* lower 32 bits of Buf_Ptr field */
    u_short	BufPtrHi;		/* Upper 16 bits of Buf_Ptr field */
    u_short	ByteCount;		/* number of bytes in the segment or BSM */
};
#if 0
struct BSD
{
     	unsigned __int64 type: 1;		/* BSD or BSM */
	unsigned __int64 reserved: 1;	
	unsigned __int64 buf_ptr: 46;		/* PA of host buffer */
	unsigned __int64 byte_count: 16;        /* of host buffer seg or BSM */
};
#endif
/*
** SIMport Set Parameter command
*/
struct SPO_SetParam
{
    unsigned char                    ntLUNs;
    unsigned char                    n_host_sg;
    struct
    {
        unsigned int                 enable_counters: 1;
	unsigned int                 unused:15;
    } flags;
    unsigned int                     system_time;
    unsigned int                     int_holdoff;
    unsigned int                     rp_timer;
    struct BSD                       host_sg_BSD;
};
/*
** SIMport Parameter Set response message
*/
#if 0
typedef struct
{
} SPO_ParamSetType;
#endif

/*
** SIMport Set Channel State command
*/

struct SPO_SetChanState
{
    unsigned char                    state;
    unsigned char                    SBZ_1;
    unsigned short                    SBZ_flags;
    unsigned char                    ChanID;
    unsigned char                    NodeID;
    unsigned char                    SBZpadding[2];
} ;
/*
** SIMport Channel State Set response message
*/

struct SPO_ChanStateSet
{
    unsigned char                    state;
    unsigned char                    SBZ_1;
    unsigned short                    SBZ_flags;
    unsigned char                    ChanID;
    unsigned char                    NodeID;
    unsigned char                    SBZpadding[2];
} ;

/*
** SIMport Set Device State command
*/

struct SPO_SetDevState
{
    unsigned char                    state;
    unsigned char                    SBZ_1;
    unsigned short                    SBZ_flags;
    unsigned char                    SBZ_2[4];
} ;
/*
** SIMport Device State Set response message
*/

struct SPO_DevStateSet
{
    unsigned char                    state;
    unsigned char                    SBZ_1;
    unsigned short                    SBZ_flags;
    unsigned char                    SBZpadding[4];
} ;

/*
** SIMport Verify Adapter Sanity command
*/
#if 0
typedef struct
{
} SPO_VerifyAdapSanityType;
/*
** SIMport Adapter Sanity Verified response message
*/

typedef struct SPO_AdapSanityVerType
{
} ;
#endif
/*
** SIMport Read Counters command
*/

struct SPO_ReadCntrs
{
    unsigned char                    SBZpadding[2];
    struct
    {
        unsigned int       ZeroCounters:  1;
	unsigned int       Unused: 15;
    } flags;
    unsigned char                    SBZ_2[4];
} ;
/*
** SIMport Counters Read response message
*/

/* IS THIS CORRECT??? */
struct SPO_CntrsRead
{
    unsigned char                    SBZpadding[2];
    unsigned short                    SBZ_flags;
    unsigned int                     TimeSinZero;
    unsigned int                     BusFaults;
    unsigned int                     SCSIComSent;
    unsigned int                     SCSIComRecv;
    unsigned int                     DataSent;
    unsigned int                     DataRecv;
    unsigned int                     SCSIBusResets;
    unsigned int                     BDRsSent;
    unsigned int                     SelTimeouts;
    unsigned int                     ParityErrs;
    unsigned int                     UnsolReselect;
    unsigned int                     BadMessages;
    unsigned int                     MessReject;
    unsigned int                     UnexpDiscon;
    unsigned int                     UnexpPhase;
    unsigned int                     SyncViol;
    unsigned int                     ExpansionSpace[5];
    unsigned int                     ImpSpecific[22];
} ;

/*
** Values for Counters Read status field
*/
#define		SPO_CHAN_NOT_ENABLED -1

/*
** SIMport BSD Request request message
*/

struct SPO_BSDReq
{
    unsigned char                    BufID;
    unsigned char                    SBZpadding[1];
    unsigned short                   SBZ_flags;
    unsigned int                    offset;
    struct VA                CCB_VA;
} ;
/*
** SIMport BSD Response command
*/

struct SPO_BSDResp
{
    unsigned char                    BufID;
    unsigned char                    SBZpadding[1];
    unsigned short                   SBZ_flags;
    unsigned int                    offset;
    struct VA                CCB_VA;
    struct BSD               BSDBlock[12];
} ;

/*
**  values for BSDReq and BSDReqp BufID field
*/
#define    SPO_BUFID_DATA      0
#define    SPO_BUFID_CDB       1
#define    SPO_BUFID_SENSE     2
#define    SPO_BUFID_MESSAGE   3
/*
** SIMport Bus Reset Request request message
*/

struct SPO_BusResetReq
{
    unsigned char                    reason;
    unsigned char                    TargetID;
    unsigned char                    SBZ_flags[2];
    unsigned char                    SBZpadding[4];
    struct PA		      CCB;
} ;

/*
**  values for the BusResetReq reason field
*/
#define    SPO_REJECT_BDR     1
#define    SPO_PHASE_ERROR    2
#define    SPO_DATA_OUT       3
#define    SPO_RSVD_PHASE     4
#define    SPO_NO_MSG_OUT     5
/*
** SIMport Bus Reset Response command
*/
#if 0
typedef struct
{
} SPO_BusResetRespType;
#endif
/*
** SIMport Unsolicited Reselection message
*/

struct SPO_UnsolResel
{
    unsigned char                    SBZpadding[6];
} ;
/*
**  values for UnsolResel status field
*/
#define        SPO_UNSOL_RESEL      1
/*
** SIMport Unsolicited Channel Disable message
*/

typedef struct SPO_ChanDisabled
{
    unsigned char                    SBZpadding[6];
} ;

/*
**  values for ChanDisabled status field
*/
#define    SPO_BUS_WAS_RESET       2
#define    SPO_RESET_REJECT    3
#define    SPO_CHANNEL_HARD_ERR 5
/*
** SIMport Unsolicited Device Disable message
*/

struct DevDisabled
{
    unsigned char                    SBZpadding[6];
} ;

/*
**  values for DevDisabled status field
*/
#define        SPO_DEVICE_RESET      4

/*
** Environment Fault AEN message (Vendor Unique)
*/

struct SPO_EnvFaultMsg
{
    struct
    {
	unsigned int	term_power: 1;
	unsigned int	rsvd: 15;
    } flags;
    unsigned char	reserved[6];
} ;

#if 0
/*
** Target Event Message (Vendor Unique)
*/

typedef struct
{
    sequence_ID  unsigned char[2];
    I_ID         unsigned char;
    Tag_ID       unsigned char;
    struct
    {
        unsigned int   tag_id_valid: 1;
	unsigned int   clear_all_events: 1;
        unsigned int   rsvd: 6;
    } flags;
    sense_Len    unsigned char;
    reserved     unsigned char[2];
    msg_code     unsigned char;
    message_args unsigned char[7];
    sense_buffer unsigned char[32];
} SPO_TargetEventType;

/*
** Set Target State /Target State Set (Vendor Unique)
*/
typedef struct
{
    sequence_ID  unsigned char[2];
    I_ID         unsigned char;
    Tag_ID       unsigned char;
    struct
    {
        unsigned int   tag_id_valid: 1;
        unsigned int   rsvd: 7;
    } flags;
    reserved    uchar [3];
} SPO_SetTargetStateType;
#endif

/**************************************************************************
**  MODULE DATA STRUCTURE:  PrivateDataArea
**
**     The Private Data Area is part of a CAM defined Execute SCSI I/O CCB
**     command. It is used to pass SIMport specific information about the
**     SCSI I/O command.
**-------------------------------------------------------------------------
**  DESCRIPTION:
**
**     The PrivateDataArea is part of an Execute SCSI I/O command. The
**     format of the PrivateDataArea is specified by SIMport. It is passed
**     to the adapter as part of the body of an Execute SCSI I/O command.
**     It exists in the adapter as part of the QProcSlot (QPS) data
**     structure. Refer to the SIMport specification for addition details.
***************************************************************************
**/

/*
** The SIMport Private Data Area of Exec SCSI IO CCB (part of QPS)
** This structure also defines areas that are used for ACCEPT TARGET I/O CCBs
** check the architecture specification to see which fields are used for
** which CCBs
*/
struct PrivateDataArea
{
    unsigned char               scsi_stat;
    unsigned char               sen_res;
    unsigned char               CDBlen;  /* not in SIMPort spec?? */
    unsigned char               sen_len;
    unsigned int                data_res;
    struct BSD                  start_data_BSD;
    struct BSD                  end_data_BSD;
    struct BSD                  cdb_BSD;
    struct BSD                  sen_BSD;
    unsigned int		cmd_timeout;
    unsigned char		tag_q_action;
    unsigned char	    	tag_id;
    unsigned char		initiator_id;
    unsigned char		reserved;
    unsigned __int64            ccb_next_ptr;
};

/************************************************************************
**  MODULE DATA STRUCTURE:  SPO_AdapState
**
**     The SPO_AdapState is a state variable which maintains the current
**     state of the adapter.
**-----------------------------------------------------------------------
**  DESCRIPTION:
**
**     The primary (or steady) adapter states are UNINITIALIZED, DISABLED,
**     and ENABLED. During adapter initialization and reset the following
**     intermediate states are used SPO_WAIT_ABBR and SPO_NEW_ABBR.
*************************************************************************
**/

#define    SPO_UNINITIALIZED    0
#define    SPO_WAIT_ABBR        1
#define    SPO_NEW_ABBR         2
#define    SPO_DISABLED         3
#define    SPO_ENABLED          4

/**************************************************************************
**  MODULE DATA STRUCTURE:  AdapBlock
**
**     The AdapBlock is an adapter resident copy of the host resident
**     SIMport defined Adapter Block.
**-------------------------------------------------------------------------
**  DESCRIPTION:
**
**     The AdapBlock contains initialization parameters and the links for
**     the SIMport interface queues between the adapter and the host.
**     QBSize is the number of bytes in a SIMport queue buffer.
**     CCBPtrSize, is the number of bytes in a SIMport host pointer field.
**     The QUEUE structures contain the head and tail queue carrier
**     addresses for the SIMport queues.
***************************************************************************
**/

struct QueueHeader
 {
	struct QueueCarrier *head_ptr;		/* possible 64 bit addr */
	unsigned int head_hi;
	struct QueueCarrier *tail_ptr;		/* possible 64 bit addr */
	unsigned int tail_hi;
 };

struct AdapBlock
{
    unsigned short                  qb_size;
    unsigned char                   ccb_ptr_size;
    unsigned char                   SBZFiller[5];
    unsigned long 	            SBZFiller1[2];
    struct QueueHeader    	    DACQ;
    struct QueueHeader     	    ADRQ;
    struct QueueHeader     	    DAFQ;
    struct QueueHeader     	    ADFQ;
};

/* SIMPort registers */
struct sim_regs {
#if PATZA
	unsigned char reserved0[32];	
#else
	unsigned char reserved0[12];
	unsigned int reset_start;
	unsigned char reserved1[16];
#endif
	unsigned int argument_lo;		/* base + 20h */
	unsigned int argument_hi;	
	unsigned int test_start;
	unsigned int test_status;
	unsigned char reserved2[208];
	unsigned int amcsr_lo;			/* base + 100h */
	unsigned int amcsr_hi;
	unsigned int abbr_lo;
	unsigned int abbr_hi;
	unsigned int dafqir_lo;
	unsigned int dafqir_hi;
	unsigned int dacqir_lo;
	unsigned int dacqir_hi;
	unsigned char reserved3[32];
	unsigned int asr_lo;			/* base + 140h */
	unsigned int asr_hi;
	unsigned int afar_lo;
	unsigned int afar_hi;
	unsigned int afpr_lo;
	unsigned int afpr_hi;
	unsigned char reserved4[88];
	unsigned int flash_lock;  		/* base + 1B0h */		
	unsigned char reserved5[12];
	unsigned int err_log_n_lo;
	unsigned int err_log_n_hi;
#if PATZA
	unsigned char reserved6[917048];
	unsigned int clrint;
	unsigned char reserved7[65532];
	unsigned int reset_start;
#else
	unsigned char reserved6[312];
	unsigned int clrint;
#endif
 };



/* AMCSR - Adapter Maintenance Control and Status Register */

#define AMCSR$M_MIN	0x1	/* maintenance initialize */
#define AMCSR$M_IE	0x8	/* interrupt enable */


/**********************************************************
**  MODULE DATA STRUCTURE:  ASRBitNumbers
**
**     The ASRBitNumbers defines the names for each bit in the ASR
**     or Adapter Status Register.
**---------------------------------------------------------
**  DESCRIPTION:
**
**     Note that the ASR register is a 64 bit register. Currently SIMport
**     does not define bits 32-63. For additional details about the ASR
**     register, refer to the SIMport specification.
***********************************************************
**/

/*
** The following define the symbols for the bit numbers in the ASR
** register.
*/
#define    SPO_ASR_0          0
#define    SPO_ASR_1          1
#define    SPO_ASR_ADSE       2
#define    SPO_ASR_AMSE       3
#define    SPO_ASR_AAC        4
#define    SPO_ASR_ASTE       5
#define    SPO_ASR_6          6
#define    SPO_ASR_7          7
#define    SPO_ASR_ASIC       8
#define    SPO_ASR_UNIN       9
#define    SPO_ASR_AME        31

#define ASR$M_ADSE		0x4     /* data structure error */
#define ASR$M_AMSE		0x8	/* memory system error */
#define ASR$M_AAC		0x10    /* abnormal adapter condition */
#define ASR$M_ASIC		0x100	/* adapter completion interrupt */
#define ASR$M_UNIN		0x200   /* unitialized adapter */
#define ASR$M_AME		0x80000000  /* adapter maintenance error */

/**********************************************************
**  MODULE DATA STRUCTURE:  AFPReg
**
**     The AFPReg structure defines the format of the Adapter Failing
**     Parameter Register.
**---------------------------------------------------------
**  DESCRIPTION:
**
**     The ErrorNum field is extracted from the ErrorCode data structure.
**     The remaining fields are passed to the SPO_Error routine in the
**     EParamBlock.
***********************************************************
**/
struct AFPReg
{
     unsigned char                     ErrorNum;
     unsigned char                     param1;
     unsigned char                     param2;
     unsigned char                     param3;
     unsigned int                      param4;
};

#define AFPR$M_ERROR	0xff	/* error code */ 

/* AFPR Error Codes */

/* when ASR ADSE bit is set */
#define AFPR_ERR_IAB	4	/* illegal adapter block format */
#define AFPR_ERR_IQB	8	/* illegal queue buffer format */
#define AFPR_ERR_IQC	16	/* invalid queue carrier data */
#define AFPR_ERR_IBS	17	/* invalid BSR or BSD field */

/* when ASR AMSE bit it set*/
#define AFPR_ERR_AAB	1	/* error when accessing Adapter Block */
#define AFPR_ERR_AQC	2	/* error when accessing Queue Carrier */
#define AFPR_ERR_AQB	3	/* error when accessing Queue Buffer */
#define AFPR_ERR_ABS	4	/* error when accessing BSM ar BSD */



/**************************************************************************
**  MODULE DATA STRUCTURE:  BSM
**
**     A BSM (Buffer Segment Map) is a SIMport data structured which is
**     used to define a host memory buffer which consists of multiple
**     segments and thus requires multiple BSDs to define.
**-------------------------------------------------------------------------
**  DESCRIPTION:
**
**     The BSM is used to define a physically discontiguous host memory
**     buffer. The NextBSMBSD field is a link to the next BSM, if
**     required. The NumEntries field is a count of the number of valid
**     BSDs in the BSDBlock. The TotalCount field contains the total byte
**     count that this BSM maps. The BufferOffset field contains the
**     buffer offset from the start of the buffer that this BSM maps.  The
**     BSDBlock field contains a BSD data structure for each physically
**     contiguous host memory segment.
***************************************************************************
**/

/*
** SIMport Buffer Segment Map
*/
struct BSM
{
    struct BSD   NextBSMBSD;            /* link for linked list of BSMs */
    unsigned short       NumEntries;            /* number of BSDs in BSDBlock */
    unsigned char       SBZpadding[6];
    unsigned int        TotalCount;            /* total byte count for this BSM */
    unsigned int        BufferOffset;          /* buffer offset for this BSM */
    struct BSD   BSDBlock[17];          /* list of buffer segment BSDs */
};

#endif
