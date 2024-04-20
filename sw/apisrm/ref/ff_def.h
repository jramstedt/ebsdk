/* File:	ff_def.h
 *
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
 * Abstract:	FF port definitions (ff_def) Definitions
 *
 * Author:	John A. DeNisco
 *		D.W. Neale
 *
 * Modifications:
 *
 *	dwn	16-Mar-1992	continue development
 *	jad	12-Mar-1991	Initial entry.
 */
 
typedef unsigned long int PORT_ULONG;

/*
/* PDQ Chip CSRs. Note that the offsets for the PDQ CSRs also apply in    
/* memory space when the CSRs are selected to be memory mapped.           
*/                                                                        
#define         PORT_RESET              0x800
#define         HOST_DATA               0x804
#define         PORT_CONTROL            0x808
#define         PORT_DATA_A             0x80C
#define         PORT_DATA_B             0x810
#define         PORT_STATUS             0x814
#define         HOST_INTERRUPT          0x818
#define         HOST_INTERRUPT_MASK     0x81C
#define         HOST_RCV_TX_DAT_PROD_NR 0x820
#define         HOST_RCV_TX_DAT_PROD_R  0x824
#define         HOST_RCV_CMD_PROD       0x828
#define         HOST_TX_CMD_PROD        0x82C
#define         HOST_RCV_SMT_PROD       0x830
#define         HOST_RCV_UNSOL_PROD     0x834


/* Additional port definitions
*/
#define FF_RCV_DES_K_BUFF_HI_SHIFT  32
#define FF_RCV_DESCR_K_128          128
#define FF_M_COMP_BLOCK_PORT_DATA_B 0xffff
#define FF_K_COMP_BLK_SHIFT_DATA_B  32
#define FF_M_DESCR_BLK_PORT_DATA_B  0xffff0000
#define FF_K_DESCR_BLK_SHIFT_DATA_B 32

/* Shared host memory modulus and remainder
*/
#define FF_K_DESC_BLK_MOD 4096
#define FF_K_DESC_BLK_REM 0
#define FF_K_COMP_BLK_MOD 64
#define FF_K_COMP_BLK_REM 0

/* Descriptor constants
*/
#define FF_K_XMT_DATA_CNT       256
#define FF_K_XMT_DATA_MAX_BYTES 8192
#define FF_K_XMT_DATA_ALIGN     0
#define FF_K_XMT_DATA_CHAIN     2
#define FF_K_RCV_DATA_CNT       256
#define FF_K_RCV_DATA_MAX_BYTES 8192
#define FF_K_RCV_DATA_ALIGN     512
#define FF_K_RCV_DATA_CHAIN     2
#define FF_K_CMD_REQ_CNT        16
#define FF_K_CMD_REQ_MAX_BYTES  512
#define FF_K_CMD_REQ_ALIGN      0 
#define FF_K_CMD_REQ_CHAIN      1
#define FF_K_CMD_RSP_CNT        16
#define FF_K_CMD_RSP_MAX_BYTES  512
#define FF_K_CMD_RSP_ALIGN      512
#define FF_K_CMD_RSP_CHAIN      1
#define FF_K_UNSOL_CNT          16
#define FF_K_UNSOL_MAX_BYTES    512
#define FF_K_UNSOL_ALIGN        512
#define FF_K_UNSOL_CHAIN        1
#define FF_K_SMT_CNT            64
#define FF_K_SMT_MAX_BYTES      8192
#define FF_K_SMT_ALIGN          512
#define FF_K_SMT_CHAIN          2

/* Min and max packet size for an FDDI
*/
#define FF_K_MIN_PKT_SIZE 0		/*Minimum packet size*/
#define FF_K_MAX_PKT_SIZE 8192		/*Maximum packet size*/

/* End of additional port definitions
*/

/* Register mask
*/
#define FF_M_CSR_MASK -1

/*
*/
#define FF_K_SOB 0X80000000
#define FF_K_EOB 0X40000000


/* How we'll fill in some queues
*/
#define FF_K_CMD_RSP_FILL 4
#define FF_K_UNSOL_FILL   4
#define FF_K_SMT_FILL     4

/* Environment variables
*/
#define FF_K_XMT_MAX_SIZE FF_K_MAX_PKT_SIZE
#define FF_K_XMT_INT_MSG   1
#define FF_K_XMT_MSG_POST  1
#define FF_K_XMT_FULL_POST 1
#define FF_K_RCV_BUF_NO    16
#define FF_K_MSG_BUF_SIZE  DYN$K_NILOOKASIDE
#define FF_K_MSG_MOD       0
#define FF_K_MSG_REM       0
#define FF_K_MODE_NORMAL   0

/* Driver flags
*/
#define FF_K_DRIVER_FLAGS_DEF 0		/*Driver flags default*/
#define	FF_M_BROADCAST        1		/*Broadcast*/
#define	FF_M_ENA_BROADCAST    1		/*Enable broadcast*/
#define	FF_M_DIS_BROADCAST    0		/*Disable broadcast*/

/* Queue definitions
*/
#define FF_MAX_QUEUE_NAME 80		/*Maximum size of a queue name*/

/* General queue descriptor
*/
volatile struct FF_GEN_DESCR {
    PORT_ULONG	long_0;
    PORT_ULONG	long_1;
    };

/* Process queue information
*/
volatile struct FF_QUEUE_INFO {
    struct SEMAPHORE    is;		/*Pointer to the isr semaphore*/
    struct SEMAPHORE    fs;		/*Pointer to the full semaphore*/
    struct FF_GEN_DESCR *dp;		/*Descriptor pointer*/	 
    int                 qs;		/*Size of the queue*/
    unsigned char *ci, *hc;		/*Completion index, host consumer index*/	
    unsigned char *pi;			/*Producer index*/	
    struct INODE  *ip;			/*Pointer to the port device INODE*/
    void (*rp)(struct FF_GEN_DESCR *dp);/*Pointer to a routine to be executed */
    };

/* FF csrs
*/
volatile struct FF_CSRS {
    unsigned int port_reset;	/*Csr copies*/
    unsigned int port_control;			
    unsigned int port_data_a;
    unsigned int port_data_b;
    unsigned int t2_producer_arm;
    unsigned int t2_producer_no_arm;
    unsigned int t1_producer_unsol;
    unsigned int t1_producer_smt_host;
    unsigned int t1_producer_cmd;
    unsigned int cmdrsp_p;

    unsigned int port_status;
    unsigned int host_data;
    unsigned int host_int_type_0;
    unsigned int host_int_enb_x;
    };

    /* State constants
    */
#define FF$K_UNINITIALIZED 0
#define FF$K_STOPPED       1
#define FF$K_STARTED       2

    /* General program constants
    */
#define SIZE$K_LW	       4	/* four bytes to a longword */
#define SIZE$K_BYTE	       1	/* 1 bytes to 1 byte */
#define FF$K_OPEN_TIMEOUT_SECS 1	/*1 seconds			    */

    /* Callback message received
    */
#define FF_MAX_RCV_MSGS      10
#define FF_CALLBACK_TIMEOUT 100		/*100 * 1/100 of a second*/
#define FF_CALLBACK_WAIT     10		/*10 milliseconds = 1/100 second*/

struct FF_MSG {
    struct FF_MSG *flink;
    struct FF_MSG *blink;
    unsigned char *msg;
    int           size;
    };



/*EZ structure*/
#if 0
volatile struct EZ_PB {
    struct pb 		pb;
    struct LOCK         spl_port;		/*Must be aligned on a quadword*/
    struct SEMAPHORE 	ez_isr_tx,		/*Semaphores for EZ interrupts*/
    			ez_isr_rx; 
    struct SEMAPHORE 	cmd_sync; 		/*Command syncronize*/
    struct SEMAPHORE 	write_lock;
    struct xgec_csrs 	*xg;			/*Pointer to xgec csrs*/
    U_INT_32 		vector;			/*Device's interrupt vector*/
    struct xgec_rdes 	*rdes;			/*Pointer to rcv descriptors*/
    unsigned int 	rx_index;		/*Receive index*/
    struct xgec_tdes 	*tdes_in,		/*Pointers to xmt descriptors*/
    			*tdes_out; 
    struct EZ_TX_INFO 	*ti_in,			/*Transmit info pointers*/
    			*ti_out;
    unsigned int 	tx_index_in,		/*Transmit index*/
    			tx_index_out;
    void (*lrp)(struct FILE*,char*,U_INT_16);	/*Pointer to the datalink rcv*/
    U_INT_32 		saved_int_status;	/*Saved interrupt status*/
    char* 		saved_first_segment;	/*Saved first segment*/
    struct xgec_tdes0 	saved_tdes0;		/*Saved TDES0*/

    int 		ti_cnt,			/*Interrupt counters and flags*/
    			ri_cnt,
    			ru_cnt;

    int 		me_cnt,
    			rw_cnt,
    			tw_cnt,
    			bo_cnt;

    int 		rhf_cnt,
    			rwt_cnt,
    			int_msg_cnt,
    			msg_post_cnt;

    int 		tbf_cnt;		/*Transmit buffer full count*/
    int 		tc_cnt;			/*Transmit complete count*/
    int 		rcv_msg_cnt;		/*Number of recieved messages*/
    struct EZ_MSG 	rqh;			/*Recieved msg queue header*/
    U_INT_8 		*rx_chain_buf;		/*Pointer an rx chain buffer*/
    int 		rx_chain_index;		/*Rx chain buffer index*/
    int 		xmt_uf,			/*Some error bits*/
    			xmt_tn,
    			xmt_le,
    			xmt_to;	
    int 		xmt_hf;
    int 		setup_init;		/*Remote boot stuff*/
    int 		password_init;
    int 		filter_type;
    int 		state;			/*Driver state*/
    struct FILE 	*sa_fp;			/*file descriptor for station adr csr*/
    struct FILE 	*xgec_csr_fp;		/*file descriptors for xgec csr access*/
    char 		name[32];		/*Port name*/
    char 		short_name[32];		/*Port name (Short version)*/
    U_INT_8 		sa[6];			/*Our station address*/
    };
#endif

/* FF port block structure
*/
volatile struct FF_PB {
    struct pb 	        pb;		/* see cp$src:PB_DEF.H */
    struct LOCK 	spl_port;	/* Must be aligned on a quadword */
    char       		loopback;	/* ff_rx looback indicator */
    char       		name[32];	/* Python Port Name */
    char 		short_name[32];	/* Python Port Name (Short version) */
    char       		fb_name[6];	/* Fbus Port Name */
    char* 		saved_first_segment;	/*Saved first segment*/
    unsigned char    	sa[6];		/* Our station address */
    struct SEMAPHORE 	cmd_sync; 	/*Command syncronize*/
    struct SEMAPHORE 	ff_isr_tx,	/*Semaphores for EZ interrupts*/
    			ff_isr_rx; 
    struct FF_CSRS  	ff;		/* Csrs*/

    FF_DESCR_BLOCK        *dbp;		/* Descriptor block */
    FF_CONSUMER_BLOCK     *cbp;		/* Consumer block pointer */
    FF_PRODUCER_INDEX     *pip;		/* Producer Index pointer */

    int 		XmtIntCnt,	/*Xmt Interrupt counter */
    			RcvIntCnt,	/* Rcv Interrupt counter */
    			ru_cnt;

    int 		type0_vector,	/* Interrupt vectors */ 
    			type1_vector,
    			type2_vector, 
    			type3_vector;

    int 		me_cnt,		/* ff_control_t counters */
    			rw_cnt,
    			tw_cnt,
    			bo_cnt;

    struct FF_QUEUE_INFO *crq;		/* Command request queue */

    unsigned char 	xmt_pi, 	/* Producer indexes */
    			rcv_pi, 
    			unsol_pi; 

    unsigned char 	cmdreq_pi, 
    			cmdrsp_pi, 
    			smt_pi;

    int			msg_post_cnt;


    void (*lrp)(struct FILE*,char*,U_INT_16);	/* Pointer to the datalink rcv*/
    int 		rcv_msg_cnt;		/* Number of recieved messages*/
    struct FF_MSG 	rqh;			/* Recieved msg queue header*/
    int			CmdRspBlock,		/* Command Response buffer pointer */
    			CmdReqBlock,		/* Command Request  buffer pointer */
    			XmtDatBlock,		/* Xmt Data buffer pointer */
    			UnsolBlock;		/* Command rcv buffer pointer */

    int 		password_init,
    			setup_init,		/* Remote boot stuff*/
    			filter_type,
    			state;			/* Driver state*/
    };
