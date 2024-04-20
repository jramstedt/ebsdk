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
 * Abstract:	DE205 port block Definitions
 *
 * Author:	Dan Marsh
 *
 * Modifications:
 *
 *	dpm	02-Apr-1994	Initial entry.
 *
 *      dpm	12-aug-1994	Changed the isr semaphores to TIMERQ to do 
 *				timeouts.
 *
 */
 

/*Min and max packet size for an ethernet*/
#define DE205_K_MIN_PKT_SIZE 60		/*Minimum packet size*/
#define DE205_K_MAX_PKT_SIZE NDL$K_MAX_PACK_LENGTH /*Maximum packet size*/
#define DE205_K_MAX_LOOP_SIZE 0x20	/*Max loop packet size*/
#define DE205_K_XMT_TIMEOUT (10*1000)	/*Ten seconds			    */

/*Environment variable defaults     */
#define DE205_K_XMT_MOD 512
#define DE205_K_XMT_REM 0
#define DE205_K_XMT_MAX_SIZE DE205_K_MAX_PKT_SIZE
#define DE205_K_XMT_MSG_POST 1
#define DE205_K_RCV_MOD 512
#define DE205_K_RCV_REM 0
#define DE205_K_MSG_BUF_SIZE DE205_K_MAX_PKT_SIZE
#define DE205_K_MSG_MOD 0
#define DE205_K_MSG_REM 0
#define DE205_K_XMT_BUF_CNT 0x08		/*Buf cnt must be a power of 2*/
#define DE205_K_RCV_BUF_CNT 0x08

/*System constants*/
#define DE205_K_VECTOR 0x850
#define DE205_K_EISA_ADDR 0xd0000
#define DE205_K_EISA_SIZE 0x10000

/*Buffer constants*/
#define DE205_K_NIIB_EADDR     0x0000
#define DE205_K_TBD_EADDR      0x0040
#define DE205_K_RBD_EADDR      0x0800
#define DE205_K_TB_ESTART_ADDR 0x1000
#define DE205_K_RB_ESTART_ADDR 0x4000
#define DE205_K_PKT_ALLOC 0x600

/*General program constants */
#define DE205_K_MODE_NORMAL 0x00000000
#define DE205_K_INIT_TIMEOUT 5000		/*Use timeout of 5 secs for init*/
#define DE205_K_OPEN_TIMEOUT_SECS 5	/*5 secs*/

/*State constants*/
#define DE205_K_UNINITIALIZED 0
#define DE205_K_STOPPED 1
#define DE205_K_STARTED 2

/*Transmit info structure and constants*/
/*This structure will be allocated in parallel*/
/*with the transmit descriptors. This structure*/
/*is used to pass information about the packet*/
/*to the transmit process de205_tx*/ 
#define DE205_K_TX_ASYNC 0			/*Transmits will be asyncronous*/
#define DE205_K_TX_SYNC 1			/*Transmits will be syncronous*/

volatile struct DE205_TX_INFO {
    struct TIMERQ *tx_timer; 		/*TX timer*/
    int sync_flag;			/*Synchronous flag*/
    struct DE205_TX_INFO *chain;		/*Pointer the next TX info*/
};

/*Callback message received*/
#define DE205_MAX_RCV_MSGS 10
#define DE205_CALLBACK_TIMEOUT 100	/*100 * 1/100 of a second*/
#define DE205_CALLBACK_WAIT 10		/*10 milliseconds = 1/100 second*/
struct DE205_MSG {
    struct DE205_MSG *flink;
    struct DE205_MSG *blink;
    unsigned char *msg;
    int size;
};

/*DE205 structure*/
volatile struct DE205_PB {
    struct pb pb;                       /*Port block*/
    struct POLLQ pqp;			/*Poll queue*/			
    struct LOCK spl_port;		/*Must be aligned on a quadword*/
    struct de205_regs *rp;		/*Pointer to LeMAC csrs*/
    struct TIMERQ de205_isr_tx,de205_isr_rx; /*Semaphores for ES interrupts*/
    struct TIMERQ *it;			/*Initialization timer*/
    struct DE205_TX_INFO *ti;	 	/*Transmit info pointers*/
    void (*lrp)(struct FILE*,char*,U_INT_16); /*Pointer to the datalink rcv*/
    unsigned char *rx_chain_buf;	/*Pointer an rx chain buffer*/
    int slot;				/*Slot we are in*/
    int rx_chain_index;			/*Rx chain buffer index*/
    unsigned int tx_index_in,tx_index_out; /*Transmit index*/
    unsigned int rx_index;		/*Receive index*/
    char* saved_first_segment;		/*Saved first segment*/
    unsigned char* eisa_addr;  		/*Buffer address*/
    int eisa_size;	    		/*Buffer size*/
    int state;				/*Driver state flag*/
    int vector;				/*Interrupt vector*/
    int ti_cnt,ri_cnt,tx_bue_cnt,rx_bue_cnt; /*Interrupt counters and flags*/
    int babl_cnt,cerr_cnt,miss_cnt,merr_cnt;
    int msg_post_cnt,tbf_cnt,bp_cnt;
    int rcv_msg_cnt;			/*Number of recieved messages*/
    struct DE205_MSG rqh;		/*Recieved msg queue header*/
    char name [32];			/*Port name*/
    char short_name [32];		/*Port name (Short version)*/
    unsigned short irqval;		/*Saved tbdw3*/
    U_INT_8 sa[6];			/*Our station address*/
    };
