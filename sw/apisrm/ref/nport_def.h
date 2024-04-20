/*
 * Copyright (C) 1990 by
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
 * Abstract:	Adapter Block (AB) Definitions for N_Port
 *		Queue Definitions for N_Port
 *		Buffer Descriptor Definitions for N+Port
 *
 * Author:	Thomas G. Arnold
 *
 * Modifications:
 *
 *	tga	18-Apr-1991	Initial entry.
 */
 
/*
 * N_Port Buffer Descriptors 
 */

/* Mask to quickly extract physical address; assumes 8 kb page */
#define PA_MASK ((1<<13)-1)

/* Buffer Pointer Types */
#define TYP0 0	/* ( unsigned * ) */
#define TYP1 1	/* ( TYPE_PTR * ) */
#define TYP2 2	/* ( TYPE_PTR ** ) */
#define TYP3 3  /* ( unsigned ** ) */

/* Buffer Pointers */
typedef struct type_ptr {
  unsigned int     type  : 2;	/* Type of buffer pointer */
  unsigned int           : 3;	/* Should Be Zero */
  unsigned int     pa    : 27;	/* Physical Address of buffer */
  unsigned int     pa_e  : 16;	/* Physical Address of buffer */
  unsigned int     count : 16;	/* Should Be Zero */
} TYPE_PTR;

typedef struct pa_ptr {
  unsigned int           : 5;	/* Should Be Zero */
  unsigned int      pa   : 27;	/* Physical Address of buffer */
  unsigned int      pa_e : 16;	/* Physical Address of buffer */
  unsigned int           : 16;	/* Should Be Zero */
} PA_PTR;

typedef struct ampb_ptr {
  unsigned int      v    : 1;	/* Pointer Valid */
  unsigned int           : 4;	/* Should Be Zero */
  unsigned int      pa   : 27;	/* Physical Address of buffer */
  unsigned int      pa_e : 16;	/* Physical Address of buffer */
  unsigned int           : 16;	/* Should Be Zero */
} AMPB_PTR;

/* Buffer Name */
typedef struct buf_name {
  unsigned int bdl_idx : 8;	/* Buffer Descriptor Leaf index */
  unsigned int bdlt_idx : 12;	/* Buffer Descriptor Leaf Table index */
  unsigned int key : 12;	/* Buffer Key */
} BUF_NAME;

/* Buffer Descriptor Leaf */
typedef struct bdl {
  unsigned int page_offset : 13;	/* Offset to beginning of buffer */
  unsigned int             : 3;		/* Should Be Zero */
  unsigned int v           : 1;		/* Buffer Valid */
  unsigned int ro          : 1;		/* Read Only */
  unsigned int             : 14;	/* Should Be Zero */
  BUF_NAME     buf_name;		/* Buffer Name */
  unsigned int buf_len;			/* Buffer Length */
  unsigned int             : 32;	/* Should Be Zilch */
  TYPE_PTR     root0;			/* Root buffer pointer */
  TYPE_PTR     root1;			/* Additional type 0 buffer pointer */
} BDL;

/* Pointer to Buffer Descriptor Leaf */
typedef struct bdl_ptr {
  unsigned int      v         : 1;	/* Buffer Descriptor Valid */
  unsigned int                : 12;	/* Should Be Zero */
  unsigned long int bdl_ptr   : 19;	/* Pointer to Buffer Descriptor Leaf */
  unsigned long int bdl_ptr_e : 16;	/* Pointer to Buffer Descriptor Leaf */
  unsigned long int           : 16;	/* Should Be Zero */
  unsigned char     soft[8];		/* Reserved for software */
} BDL_PTR;

/* Buffer Descriptor Leaf Table (array of pointers to BDL) size; that */
/* is, the number of BDL_PTRs to fill an 8kb page. */
#define BDLT_SIZ (HWRPB$_PAGESIZE / sizeof(BDL_PTR))

/* N_Port Operation Codes */
enum N_OPC {
  SNDPM=0,   PMSNT=0,	/* Send Physical Medium message */
             PMREC=1,	/* Physical Medium message received */
  RETQE=2,   QERET=2,	/* Return Queue Entry */
  PURGQ=3,   QPURG=3,	/* Purge Queue */
  INVTC=4,   TCINV=4,	/* Invalidate Translation Cache */
  RDCNT=5,   CNTRD=5,	/* Read Counters */
  SETCHNL=6, CHNLSET=6,	/* Set Channel */
  SETCKT=7,  CKTSET=7};	/* Set Virtual Circuit */

/* N_Port path selection */
enum PATH_SELECT {AUTO=0, PATH_A=1, PATH_B=2};

/* N_Port received path */
enum RECV_PATH {LOOP=0, RECV_A=1, RECV_B=2};

/* N_Port command flags */
typedef union n_flags {
  unsigned short int i;		/* Flags as a short integer (16 bits ) */
  struct {
    unsigned int     r  : 1;	/* Generate response */
    unsigned int     i  : 1;	/* Suppress interrupts */
    unsigned int f_d_ca : 1;	/* Flush buffer cache (INVTC) */
    unsigned int        : 1;	/* Should Be Zero */
    unsigned int     sw : 4;	/* Reserved for software */
    unsigned int     gn : 3;	/* Virtual Circuit Generation Number */
    unsigned int     m  : 3;	/* Packet Multiple */
    unsigned int     ps : 2;	/* Path Select */
  }                  f;		/* Flags as bit fields (16 bits) */
} N_FLAGS ;

enum STATUS_TYPE {
  OK=0,			/* No failure */
  PKTSZVIO=1,		/* Packet Size Violation */
  INVBUFNAM=2,		/* Invalid Buffer Name */
  BUFLENVIO=3,		/* Buffer Length Violatin */
  UNRECCMD=16,		/* Unrecognized Command */
  INVDESTX=17,		/* Invalid Destination Xport Address */
  VCCLOSED=18,		/* Virtual Circuit Closed before command execution */
  INVVCGEN=19,		/* Invalid Virtual Circuit Generation Number */
  NOPATH=20,		/* No Path to destination Xport */
  INSUFVCD=21,		/* Insufficient Virtual Circuit descriptors */
  INSUFRESEQ=22,	/* NRp1 Cache Resources exhausted */
  UNRECPKT=32,		/* Unrecognized received packet */
  INVRECVX=34,		/* Invalid Received Destination Xport Address */
  INVSEQN=35,		/* Invalid Sequence Number */
  DISCARDVC=36		/* Discarded VC packet */
};

typedef union n_status {
  unsigned short int i;		/* Status treated as a short integer */
  struct {
    unsigned int     fail : 1;	/* Failure */
    unsigned int     type : 7;	/* Failure type */
    unsigned int     ptha : 2;	/* Path A status */
    unsigned int     pthb : 2;	/* Path B status */
    unsigned int     ca   : 1;	/* Counted events for All addresses */
    unsigned int          : 3;	/* Should Be Zero */
  } l;				/* Status for locally generated responses */
  struct {
    unsigned int     fail : 1;	/* Failure */
    unsigned int     type : 7;	/* Failure type */
    unsigned int     rp   : 2;	/* Receive path */
    unsigned int          : 6;	/* Should Be Zero */
  } r;				/* Status for remotely generated responses */
} N_STATUS;

#define N_Q_HEAD_SIZ sizeof(N_Q_BUF_HEAD)

/* Number of bytes in an N_Port Queue Buffer Body (section B) */
#define N_Q_BODY_SIZ (DFQE_SIZE-N_Q_HEAD_SIZ)

/* N_Port Queue Buffer */
typedef struct n_q_buffer {	
  N_Q_BUF_HEAD a;		/* Generic command header */
  unsigned char b[N_Q_BODY_SIZ];/* Command specific area */
} N_Q_BUFFER;

/*
 * N_Port Adapter Block
 */
#define MAX_CHAN 16
#define MAX_CHAN_IDX 15

/* Nport Channel Command Queues */
typedef volatile struct n_ccq {
  N_QP dccq3;			/* Driver Channel Command Queue 3 */
  N_QP dccq2;			/* Driver Channel Command Queue 2 */
  N_QP dccq1;			/* Driver Channel Command Queue 1 */
  N_QP dccq0;			/* Driver Channel Command Queue 0 */
  N_QP cccq3;			/* Channel Channel Command Queue 3 */
  N_QP cccq2;			/* Channel Channel Command Queue 2 */
  N_QP cccq1;			/* Channel Channel Command Queue 1 */
  N_QP cccq0;			/* Channel Channel Command Queue 0 */
} N_CCQ;

/* Adapter Parameter Block */
typedef volatile struct n_apb {
  unsigned short int vcdt_len;	/* Virtual Circuit Descriptor Table Size */
  unsigned short int ibuf_len;	/* Internal DMA buffer size */
  unsigned short int rampb;	/* Requested Adapter Memory Pointer Block */
  unsigned short int type;	/* Adapter Type */
  unsigned char imp_spec[8];	/* Implementation specific */
  unsigned char sbz[16];	/* Should Be Zero */
} N_APB;

/* Channel Parameter Block */
typedef volatile struct n_cpb {
  unsigned short int xp_addr;	/* Xport Address */
  unsigned char max_pgrp;	/* Maximum Polling Group */
  unsigned char max_mem;	/* Maximum Members */
  int sbz;			/* Should Be Zero */
  unsigned short int dlink_type;/* Data Link Type */
  unsigned char dlink_param[6];	/* Data Link Parameter */
  char imp_spec[16];		/* Implementation Specific Diagnostic Info */
} N_CPB;

/* N_Port Adapter Block */
typedef volatile struct nport_ab {
  N_CCQ             cq[MAX_CHAN];/* Channel Command Queues, chan 0 - 15 */
  				/* :B = A+2048 */
  N_QP              adrq;	/* Adapter Driver Response Queue */
  N_QP              dadfq;	/* Driver Adapter Datagram Free Queue */
  N_QP              addfq;	/* Adapter Driver Datagram Free Queue */
  N_QP              damfq;	/* Driver Adapter Message Free Queue */
  N_QP              admfq;	/* Adapter Driver Message Free Queue */
  N_QP              aadfq;	/* Adapter Adapter Datagram Free Queue */
  N_QP              aamfq;	/* Adapter Adapter Message Free Queue */
  BDL_PTR           *bdlt_base;	/* Pointer to Buffer Descriptor Leaf Table */
  unsigned int      rsvd0;	/* ALPHA pointer is only 4 bytes */
  unsigned int      bdlt_len;	/* Buffer Descriptor Leaf Table length */
  unsigned int      keep_alive;	/* Keep Alive timer value */
  unsigned int      dqe_len;	/* Datagram Queue Entry length */
  unsigned int      mqe_len;	/* Message Queue Entry length */
  unsigned int      int_holdoff;/* Interrupt Holdoff */
  long int          rsvd1;	/* ALPHA long int is only 4 bytes */
  AMPB_PTR          ampb_base;	/* Adapter Memory Parameter Block */
  unsigned int      ampb_len;	/* Adapter Memory Parameter Block length */
  unsigned int      rsvd3;	/* Reserved */
  unsigned char     rsvd[32];	/* Reserved -- 32 bytes in length */
  N_APB             apb;	/* Adapter Parameter Block */
  N_CPB             cpb[MAX_CHAN];/* Channel Parameter Blocks */
} NPORT_AB;

/*
 * N_Port Generic command and response definitions 
 *
 * Note that these are definitions of the "B" area of the N_Q_BUFFER
 * structure.
 */

/* Return Queue Entry */
typedef struct n_retqe {
  unsigned short int qe_req;	/* Number of queue entries requested */
} N_RETQE;

/* Queue Entry Returned */
typedef struct n_qeret {
  unsigned short int qe_req;	/* Number of queue entries requested */
  unsigned short int qe_ret;	/* Number of queue entries returned */
} N_QERET;

/* Invalidate Translation Cache */
typedef struct n_invtc {
  BUF_NAME           buf_name;	/* Buffer name to be invalidated */
} N_INVTC;

/* Translation Cache Invalidated */
typedef struct n_tcinv {
  BUF_NAME           buf_name;	/* Buffer name which was invalidated */
} N_TCINV;

/* Set Virtual Circuit Descriptor */

enum PATH_STATUS {A_B_BAD=0, A_GOOD=1, B_GOOD=2, A_B_GOOD=3};

enum SEND_STATE {IDLE=0, SNDING_DG=1, SNDING_NS=2, NSp1_FAIL=4,
		   NS_FAIL=5, SNDING_BOTH=6, NSp1_SNT=7};

enum RCV_STATE {XPCTING_NR=0, HOLDING_NRp1=1};

/* Virtual Circuit Descriptor */
typedef struct vcd {    
  unsigned int ns     : 3;	/* Send sequence number */
  unsigned int nr     : 3;	/* Receive sequence number */
  unsigned int rdp    : 1;	/* Resequencing Dual Path */
  unsigned int fsn    : 1;	/* Full sequency number */
  unsigned int eas    : 1;	/* Explicit addressing supported */
  unsigned int psts   : 2;	/* Path status */
  unsigned int nadp   : 1;	/* Non-alternating Dual Path */
  unsigned int dqi    : 1;	/* Datagram Free Queue Inhibit */
  unsigned int        : 2;	/* MBZ */
  unsigned int cst    : 1;	/* Circuit State 0=closed; 1=open */
  unsigned int        : 10;	/* Should Be Zero */
  unsigned int gn     : 3;	/* Generation Number */
  unsigned int        : 1;	/* Should Be Zero */
  unsigned int snd_st : 3;	/* Send State */
  unsigned int        : 5;	/* MBZ */
  unsigned int rcv_st : 2;	/* Receive State */
  unsigned int        : 22;	/* Should Be Zero */
} VCD;

typedef struct n_setckt {
  VCD                mask;	/* Mask to control modification of VCD */
  VCD                m_value;	/* Value to set */
} N_SETCKT;

typedef struct n_cktset {
  VCD                mask;	/* Mask to control modification of VCD */
  VCD                m_value;	/* Value to set */
  VCD                in_vcd;	/* Input VCD */
  unsigned short int vcd_res;	/* VCD resources remaining */
  unsigned short int rseq_res;	/* Resequencing resources remaining */
  unsigned short int sbz;	/* Should Be Zero */
} N_CKTSET;

/* Set Channel Variables */

typedef struct n_chnv {
  unsigned int       dqii : 1;	/* Datagram Queue Inhibit Illegal */
  unsigned int       dqid : 1;	/* Datagram Queue Inhibit Default */
  unsigned int       dqia : 1;	/* Datagram Queue Inhibit All */
  unsigned int            : 29;	/* Should Be Zero */
  unsigned int       rsvd;	/* Should Be Zero */
} N_CHNV;

typedef struct n_setchnl {
  N_CHNV             mask;	/* Mask of values to modify */
  N_CHNV             m_value;	/* Value to set variable to */
} N_SETCHNL;

typedef struct n_chnlset {
  N_CHNV             mask;	/* Mask of values to modify */
  N_CHNV             m_value;	/* Value to set variable to */
  N_CHNV             in_chnlst;	/* Input channel variables */
  unsigned short int vcd_res;	/* VCD resources */
} N_CHNLSET;

/* Queue Purge */

#define CCCQ0 (1<<0)
#define CCCQ1 (1<<1)
#define CCCQ2 (1<<2)
#define CCCQ3 (1<<3)

typedef struct n_purgq {
  unsigned char      cmdq_mask;	/* Channel-Channel command queue mask */
} N_PURGQ;

typedef struct n_qpurg {
  unsigned char      cmdq_mask;	/* Channel-Channel command queue mask */
} N_QPURG;

/* Send Datagram */

typedef struct n_snddg {
  unsigned short int dg_len;	/* Length of datagram */
  unsigned char body[N_Q_BODY_SIZ-sizeof(unsigned short int)];
} N_SNDDG;

typedef struct n_dgsnt {
  unsigned short int dg_len;	/* Length of datagram returned */
  unsigned char body[N_Q_BODY_SIZ-sizeof(unsigned short int)];
} N_DGSNT;

typedef struct n_dgrec {
  unsigned short int dg_len;	/* Length of datagram returned */
  unsigned char body[N_Q_BODY_SIZ-sizeof(unsigned short int)];
} N_DGREC;

/* Send Message */

typedef struct n_sndmsg {
  unsigned short int dg_len;	/* Length of datagram */
  unsigned char body[N_Q_BODY_SIZ-sizeof(unsigned short int)];
} N_SNDMSG;

typedef struct n_msgsnt {
  unsigned short int dg_len;	/* Length of datagram returned */
  unsigned char body[N_Q_BODY_SIZ-sizeof(unsigned short int)];
} N_MSGSNT;

typedef struct n_msgrec {
  unsigned short int dg_len;	/* Length of datagram returned */
  unsigned char body[N_Q_BODY_SIZ-sizeof(unsigned short int)];
} N_MSGREC;

/* N_Port Generic command type.  Any N_Port command field may be accessed */
/* using this structure */
typedef volatile union n_command {
  N_RETQE   retqe;
  N_QERET   qeret;
  N_INVTC   invtc;
  N_TCINV   tcinv;
  N_SETCKT  setckt;
  N_CKTSET  cktset;
  N_SETCHNL setchnl;
  N_CHNLSET chnlset;
  N_PURGQ   purgq;
  N_QPURG   qpurg;
  N_SNDDG   snddg;
  N_DGSNT   dgsnt;
  N_DGREC   dgrec;
  N_SNDMSG  sndmsg;
  N_MSGSNT  msgsnt;
  N_MSGREC  msgrec;
} N_COMMAND;
