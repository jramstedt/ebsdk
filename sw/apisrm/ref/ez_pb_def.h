#ifndef _ez_pb_def_
#define _ez_pb_def_
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
 * Abstract:	EZ port block (ez_pb) Definitions
 *
 * Author:	John A. DeNisco
 *
 * Modifications:
 *
 *	phk	18-Nov-1991	add int setup_init, password_init & filter_type
 *				fields to EZ_PB structure
 *
 *	pel	04-Nov-1991	add file descriptors for tgec/station adr csrs
 *
 *	jad	28-Mar-1991	Support hash filter.
 *
 *	jad	25-Jul-1990	Initial entry.
 */
 

/*Min and max packet size for an ethernet*/
#define EZ$K_MIN_PKT_SIZE 60		/*Minimum packet size*/
#define EZ$K_MAX_PKT_SIZE NDL$K_MAX_PACK_LENGTH /*Maximum packet size*/

/*General program constants                                                 */
#define EZ$K_XMT_TIMEOUT (10*1000)	/*Ten seconds			    */
#define EZ$K_XMT_EMPTY_TIMEOUT 10 	/*  10 * empty_timeout_sleep	    */
#define EZ$K_XMT_EMPTY_TIMEOUT_SLEEP 100 /* 100 milliseconds		    */
#define EZ$K_OPEN_TIMEOUT_SECS 1	/*1 seconds			    */
#define EZ$K_DESC_BYTES 16              /*Number of bytes in a descriptor   */
#define EZ$K_SETUP_FRAME_SIZE 128       /*Number of bytes in a setup frame  */
#define EZ$K_HASH_SIZE 72		/*Number of bytes in a hash frame  */
#define EZ$K_HASH_ADDR_OFFSET 64	/*Place in hash filter for address  */
#define EZ$K_HASH_BROADCAST_INDEX 255	/*Hash index for the broadcast address*/
#define EZ$K_XMT_RCV 0                  /*Transmit and receive              */
#define EZ$K_XMT_ONLY 1                 /*Transmit only                     */
#define EZ$K_RCV_ONLY 2                 /*Receive only                      */
#define EZ$K_TX_HOLD_CNT 0x01		/*Hold before porcessing next tx    */
#if 0
#define EZ$K_TX_HOLD_CNT 0x10000	/*Hold before porcessing next tx    */
#endif
#define EZ$K_XMT_MOD 512		/*Environment variable defaults     */
#define EZ$K_XMT_REM 0
#define EZ$K_XMT_MAX_SIZE EZ$K_MAX_PKT_SIZE
#define EZ$K_XMT_INT_MSG 1
#define EZ$K_XMT_MSG_POST 1
#define EZ$K_RCV_MOD 512
#define EZ$K_RCV_REM 0
#define EZ$K_MSG_BUF_SIZE EZ$K_MAX_PKT_SIZE
#define EZ$K_MSG_MOD 0
#define EZ$K_MSG_REM 0

#define EZ$K_XMTS_BEFORE_MEMZONE 50	/*Number of xmt descriptors before  */
					/*using memzone			    */
#define EZ$K_RCVS_BEFORE_MEMZONE 50	/*Number of RCV descriptors before  */
					/*using memzone			    */
#define EZ$K_XMT_BUF_CNT 0x10		/*Buf cnt must be a power of 2*/
#define EZ$K_RCV_BUF_CNT 0x10

#define EZ$K_MODE_NORMAL 0x50010000
#define EZ$K_MODE_MULTICAST 0x50010004
#define EZ$K_MODE_INT_PROM 0x50010102
#define EZ$K_MODE_INT_FC 0x50010140
#define EZ$K_MODE_PROM 0x50010002
#define EZ$K_MODE_INT 0x50010100

/*State constants*/
#define EZ$K_UNINITIALIZED 0
#define EZ$K_STOPPED 1
#define EZ$K_STARTED 2

/*Transmit info structure and constants*/
/*This structure will be allocated in parallel*/
/*with the transmit descriptors. This structure*/
/*is used to pass information about the packet*/
/*to the transmit process ez_tx*/ 
#define EZ$K_TX_ASYNC 0			/*Transmits will be asyncronous*/
#define EZ$K_TX_SYNC 1			/*Transmits will be syncronous*/

volatile struct EZ_TX_INFO {
    struct TIMERQ *tx_timer; 		/*TX timer*/
    int sync_flag;			/*Synchronous flag*/
    struct EZ_TX_INFO *chain;		/*Pointer the next TX info*/
};

/*Callback message received*/
#define EZ_MAX_RCV_MSGS 10
#define EZ_CALLBACK_TIMEOUT 100		/*100 * 1/100 of a second*/
#define EZ_CALLBACK_WAIT 10		/*10 milliseconds = 1/100 second*/
struct EZ_MSG {
    struct EZ_MSG *flink;
    struct EZ_MSG *blink;
    unsigned char *msg;
    int size;
};

/*EZ structure*/
volatile struct EZ_PB {
    struct pb pb;
    struct LOCK spl_port;		/*Must be aligned on a quadword*/
    struct SEMAPHORE ez_isr_tx,ez_isr_rx; /*Semaphores for EZ interrupts*/
    struct SEMAPHORE cmd_sync; 		/*Command syncronize*/
    struct SEMAPHORE write_lock;
    struct xgec_csrs *xg;		/*Pointer to xgec csrs*/
    U_INT_32 vector;			/*Device's interrupt vector*/
    struct xgec_rdes *rdes;		/*Pointer to rcv descriptors*/
    unsigned int rx_index;		/*Receive index*/
    struct xgec_tdes *tdes_in,*tdes_out; /*Pointers to xmt descriptors*/
    struct EZ_TX_INFO *ti_in,*ti_out; 	/*Transmit info pointers*/
    unsigned int tx_index_in,tx_index_out; /*Transmit index*/
    void (*lrp)(struct FILE*,char*,U_INT_16); /*Pointer to the datalink rcv*/
    U_INT_32 saved_int_status;		/*Saved interrupt status*/
    char* saved_first_segment;		/*Saved first segment*/
    struct xgec_tdes0 saved_tdes0;	/*Saved TDES0*/
    int ti_cnt,ri_cnt,ru_cnt;		/*Interrupt counters and flags*/
    int me_cnt,rw_cnt,tw_cnt,bo_cnt;
    int rhf_cnt,rwt_cnt,int_msg_cnt,msg_post_cnt;
    int tbf_cnt;			/*Transmit buffer full count*/
    int tc_cnt;				/*Transmit complete count*/
    int rcv_msg_cnt;			/*Number of recieved messages*/
    struct EZ_MSG rqh;			/*Recieved msg queue header*/
    U_INT_8 *rx_chain_buf;		/*Pointer an rx chain buffer*/
    int rx_chain_index;			/*Rx chain buffer index*/
    int xmt_uf,xmt_tn,xmt_le,xmt_to;	/*Some error bits*/
    int xmt_hf;
    int setup_init;			/*Remote boot stuff*/
    int password_init;
    int filter_type;
    int state;				/*Driver state*/
    struct FILE *sa_fp;			/*file descriptor for station adr csr*/
    struct FILE *xgec_csr_fp;		/*file descriptors for xgec csr access*/
    char name [32];			/*Port name*/
    char short_name [32];		/*Port name (Short version)*/
    U_INT_8 sa[6];			/*Our station address*/
    };

#endif
