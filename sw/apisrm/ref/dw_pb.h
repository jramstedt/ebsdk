/*
 * Copyright (C) 1992 by
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
 * Abstract:    DW300 (Proteon p1990 Token Ring Adaptor) port block Definitions
 *
 * Author:  Lowell Kopp
 *
 * Modifications:
 *
 *  lek 14-Oct-1994 Initial entry. Derived from DE422_PB.H
 */


/* Min and max packet size for an ethernet */
#define DW_K_MIN_PKT_SIZE 60         /* Minimum packet size */
#define DW_K_MAX_PKT_SIZE NDL$K_MAX_PACK_LENGTH /* Maximum packet size */
#define DW_K_MAX_LOOP_SIZE 0x20      /* Max loop packet size */
#define DW_K_XMT_TIMEOUT (10*1000)   /* Ten seconds */

/* Environment variable defaults     */
#define DW_K_XMT_MOD 512
#define DW_K_XMT_REM 0
#define DW_K_XMT_MAX_SIZE DW_K_MAX_PKT_SIZE
#define DW_K_XMT_MSG_POST 1
#define DW_K_RCV_MOD 512
#define DW_K_RCV_REM 0
#define DW_K_MSG_BUF_SIZE DW_K_MAX_PKT_SIZE
#define DW_K_MSG_MOD 0
#define DW_K_MSG_REM 0

/* Permanent xmt, rcv translation buffers - assign one to each xmt or
 * rcv parameter list.  Ethernet / IEEE 802.3 has max 1536 byte frame.
 * Token ring allows user specification of max frame size: we need
 * to accomodate max ethernet in our max frame -- nearest token ring
 * choices are 1470 bytes data (rqd 1500) or 2052 bytes. Pick arbitrary
 * buffer size larger than 2052, either 0x900 (mod 256) or 0xA00 (mod 512)
 * or 0xC00 (mod 1024) */

#define DW_K_XMT_BUF_CNT 4        /* Buf cnt must be >= 3 for ring */
#define DW_K_XMT_BUF_SIZ 0x900       
#define DW_K_RCV_BUF_CNT 20       /* at last 18 kb for max tr frame @ 16 mbs */
                                  /* make it 20 to handle largest error frame, 40k */
#define DW_K_RCV_BUF_SIZ 0x900       

/* System constants */
#define DW_K_VECTOR 0x850
#define DW_K_EISA_ADDR 0xd0000
#define DW_K_EISA_SIZE 0x10000


#define DW_K_PKT_ALLOC 0x900

/* General program constants */
#define DW_K_MODE_NORMAL 0x00000000
#define DW_K_INIT_TIMEOUT 5000       /* Use timeout of 5 secs for init */
#define DW_K_OPEN_TIMEOUT_SECS 60     /* 5 phases, max 18 sec each + margin */

#if 0
/* State constants */
/*                                  v  <-   <-   <-    <-     <-             */
/* Initial sequence: _UNIT -> _INIT -> _STARTED -> _STOPPED -> ^             */
/*                                                                           */
/* _UNINIT to _INIT loads MAC code, does BUD, passes Init Params.            */
/* _INIT to _STARTED issues Open cmd, (inserts in ring)                      */
/* _STARTED to _STOPPED does adapter HARD RESET - needs Init Params          */
/* _STOPPED to _STARTED passes Init Params, issues Open cmd                  */
#endif

#define DW_K_ADPTR_UNINITIALIZED 0 /* on powerup, RAM version needs MAC image */
#define DW_K_ADPTR_INITIALIZED 1   /* BUD passed & adpter initialized */
#define DW_K_STARTED  2            /* Open, inserted in ring */
#define DW_K_STOPPED 3             /* Hard reset, needs init block, open */


/* Transmit info structure and constants */
/* This structure will be allocated in parallel */
/* with the transmit descriptors. This structure */
/* is used to pass information about the packet */
/* to the transmit process dw_tx */
#define DW_K_TX_ASYNC 0             /* Transmits will be asyncronous */
#define DW_K_TX_SYNC 1              /* Transmits will be syncronous */

volatile struct DW_TX_INFO 
{
    struct TIMERQ *tx_timer;        /* TX timer */
    int sync_flag;                  /* Synchronous flag */
    struct DW_TX_INFO *chain;       /* Pointer the next TX info */
};

/* Callback message received */
#define DW_MAX_RCV_MSGS 10
#define DW_CALLBACK_TIMEOUT 100     /* 100 * 1/100 of a second */
#define DW_CALLBACK_WAIT 10         /* 10 milliseconds = 1/100 second */

struct DW_MSG 
{
    struct DW_MSG *flink;
    struct DW_MSG *blink;
    UBYTE *msg;
    int size;
};


#define DW_K_SCBLISTS  4
  
typedef struct _scblist {
    struct _scblist *a_next_list;
    SCB scb;
} SCB_LIST;




/*DW Port Block structure*/

volatile struct DW_PB 
{
    struct pb pb;                       /* Port block */
    struct LOCK spl_port;               /* Must be aligned on a quadword */
    struct SEMAPHORE dw_isr_tx_sem;     /* Semaphore for xmt interrupt */
    struct SEMAPHORE dw_isr_rx_sem;     /* Semaphores for rcv interrupts */
    struct TIMERQ *it;                  /* Initialization timer */
    void (*lrp)( struct FILE*, char*, U_INT_16 ); /* Pointer to the datalink rcv */
    int state;				/* local driver/adapter state */
/* ------- csr addresses -------- */
    struct DW_REGS *regp;               /* Pointer to DW300 csrs */
    ULONG a_sifacl;
    ULONG a_sifadx;
    ULONG a_sifadr;
    ULONG a_sifinc;
    ULONG a_sifcmdsts;
    ULONG a_sifdat;
    ULONG a_sifdmalen;

    UBYTE sa [6];                       /* Our station address */
    int use_src_routing;                /* TRUE if using source routing */
    int need_routing_info;              /* TRUE if source routing undefined and use_src_routing */
    int routing_len;                    /* current sourc routing field length */
    UBYTE routing_info [18];            /* max length of source routing field */
    UBYTE server_addr [6];              /* MOP server address for routing info */
    int frame_format;                   /* ENET or IEEE - xlation control */


    INIT_POINTER_TABLE dw_mem;          /* ptrs to tms380 mem objects */

    SW_LEVEL_INFO dw_slevel;            /* adapter software / chip info */
    unsigned int uc_type;               /* adapter microcode type (CAF, LLC, MAC) */
    unsigned int uc_rev_major;
    unsigned int uc_rev_minor;
    char prod_id [18];                  /* Product id string for open cmd */
    
    int no_free_scblists;               /* number of free scb lists */
    SCB_LIST *a_scblist_ring;           /* ring of scb lists */
    SCB_LIST *a_next_free_scblist;      /* next free scb list in ring */
    SCB_LIST *a_next_cmd_scblist;       /* next scb containing a command to send */
    SCB *a_scb;                         /* system command buffer */
    
    SSB *a_ssb;                         /* system status buffer */

    int xmt_buf_no;                     /* number of xmt buffers */
    PARAM_LIST *a_xmtplist_ring;        /* xmt plist ring*/
    PARAM_LIST *xmt_msg_plist;          /* FIRST plist of possible multi-plist frame */
    PARAM_LIST *a_next_free_xmtplist;   /* address of next avail xmt plist */
    PARAM_LIST *a_next_sent_xmtplist;   /* next xmt plist to process */
    PARAM_LIST *a_next_status_xmtplist; /* holds ptr to most recent frame sts */
    int no_free_xmt_plist;              /* number of free xmt plists avail */
    TOKEN_RING_HEADER *a_xmtbuf;        /* xmt translation buffers */
    USHORT xmt_result;			/* status of xmt cmd */
    int xmt_cmd_active;                 /* adapter is processing tx plists */

    int tx_index_in;                    /* var to synch plist & DW_TX_INFO */
    int tx_index_out;                   /* var to synch plist & DW_TX_INFO */
    struct DW_TX_INFO *ti;              /* Transmit info pointers */

    int rcv_buf_no;                     /* number of rcv buffers */
    PARAM_LIST *a_rcvplist_ring;        /* rcv plist ring */
    PARAM_LIST *a_next_rcvplist;        /* next rcv plist to process */
    PARAM_LIST *a_last_rcvplist;        /* most recent frame received by adapter */
    TOKEN_RING_HEADER *a_rcvbuf;        /* rcv translation buffers */
    struct DW_MSG rcv_msg_qh;           /* received message queue header */
    int rcv_msg_cnt;                    /* number of recieved messages in queue */
    int rcv_result;                     /* status for receive command */
    int rcv_cmd_active;                 /* adapter is receiving msgs */

    int driver_flags;			

    CMD_OPEN *a_opn_cmd_buf;            /* TMS380 open command param buffer */
    USHORT opn_result;                  /* CMD_OPEN completion info */
    USHORT close_result;

    ERROR_LOG *a_error_log;             /* adapter error log info */
    USHORT errlog_result;               /* CMD_RDERR completion info */
    
    CHECK_FRAME *a_check_frame;         /* adapter check frame */
    
    USHORT cntr_w_rcv_int;         /* Counters */
    USHORT cntr_w_xmt_int;
    USHORT cntr_w_line_error;      /* extended adapter error log counters */
    USHORT cntr_w_burst_error;
    USHORT cntr_w_arifci_error;
    USHORT cntr_w_lostframe;
    USHORT cntr_w_rcv_congest;
    USHORT cntr_w_framecopy;
    USHORT cntr_w_token_error;
    USHORT cntr_w_dmabuserror;
    USHORT cntr_w_dmaparerror;
    
    USHORT cntr_w_signal_loss;     /* detected signal loss on ring */
    USHORT cntr_w_hard_err;        /* adptr is xmtng / rcvng beacon frames */
    USHORT cntr_w_softerr;         /* adptr has xmtd a report error MAC frame */
    USHORT cntr_w_xmt_beacon;      /* adptr is xmtng beacon frames to ring */
    USHORT cntr_w_lobe;            /* open or short wiring fault between adptr and concentrator */
    USHORT cntr_w_autoremoval_err; /* failed lobe wrap test resulting from beacon auto-removal process */
    USHORT cntr_w_remove_stn;      /* rcvd MAC frame requesting ring station removal */
    USHORT cntr_w_single_station;  /* 1=station is only one on ring, 0=not only station */
    USHORT cntr_w_ring_recovery;   /* 1=observed claim token MAC frames, 0=ring purge xmtd/rcvd */

    UBYTE *eisa_addr;                   /* FYI Buffer address*/
    int eisa_size;                      /* FYI Buffer size*/

/* ------------- TEMP TEST VARS --------------- */
    USHORT  opn_cntr;
    USHORT  close_cntr;
    USHORT  pend_adptr_opn_cntr;
};

