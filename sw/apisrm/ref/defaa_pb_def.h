/*
 * Copyright (C) 1996 by
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
 * Abstract:	DEFAA Future Bus FDDI port block Definitions
 *
 * Author:	Console Firmware Team
 *
 */

/*Min and max packet size for an ethernet*/

#define EZ$K_MIN_PKT_SIZE 60			/*Minimum packet size*/
#define EZ$K_MAX_PKT_SIZE NDL$K_MAX_PACK_LENGTH /*Maximum packet size*/

/*General program constants */

#define EZ$K_XMT_TIMEOUT (10*1000)		/* Ten seconds				*/
#define EZ$K_XMT_EMPTY_TIMEOUT 10 		/* 10 * empty_timeout_sleep		*/
#define EZ$K_XMT_EMPTY_TIMEOUT_SLEEP 100 	/* 100 milliseconds			*/
#define EZ$K_OPEN_TIMEOUT_SECS 1		/* 1 seconds				*/
#define EZ$K_DESC_BYTES 16              	/* Number of bytes in a descriptor	*/
#define EZ$K_SETUP_FRAME_SIZE 128       	/* Number of bytes in a setup frame	*/
#define EZ$K_HASH_SIZE 72			/* Number of bytes in a hash frame	*/
#define EZ$K_HASH_ADDR_OFFSET 64		/* Place in hash filter for address	*/
#define EZ$K_HASH_BROADCAST_INDEX 255		/* Hash index for the broadcast address	*/
#define EZ$K_XMT_RCV 0                  	/* Transmit and receive			*/
#define EZ$K_XMT_ONLY 1                 	/* Transmit only			*/
#define EZ$K_RCV_ONLY 2                 	/* Receive only				*/
#define EZ$K_TX_HOLD_CNT 0x01			/* Hold before porcessing next tx	*/
#define EZ$K_XMT_MOD 512			/* Environment variable defaults	*/
#define EZ$K_XMT_REM 0
#define EZ$K_XMT_MAX_SIZE EZ$K_MAX_PKT_SIZE
#define EZ$K_XMT_INT_MSG 1
#define EZ$K_XMT_MSG_POST 1
#define EZ$K_RCV_MOD 512
#define EZ$K_RCV_REM 0
#define EZ$K_MSG_BUF_SIZE EZ$K_MAX_PKT_SIZE
#define EZ$K_MSG_MOD 0
#define EZ$K_MSG_REM 0

#define EZ$K_XMTS_BEFORE_MEMZONE 50	/* Number of xmt descriptors before using memzone */
#define EZ$K_RCVS_BEFORE_MEMZONE 50	/* Number of RCV descriptors before using memzone */
#define EZ$K_XMT_BUF_CNT 0x10		/* Buf cnt must be a power of 2 */
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

/*
 * Transmit info structure and constants
 * This structure will be allocated in parallel
 * with the transmit descriptors. This structure
 * is used to pass information about the packet
 * to the transmit process ez_tx
 */ 

#define EZ$K_TX_ASYNC 0			/* Transmits will be asyncronous */
#define EZ$K_TX_SYNC 1			/* Transmits will be syncronous */

volatile struct EZ_TX_INFO {
    struct TIMERQ *tx_timer; 		/* TX timer */
    int sync_flag;			/* Synchronous flag */
    struct EZ_TX_INFO *chain;		/* Pointer the next TX info */
};

/*Callback message received*/

#define EZ_MAX_RCV_MSGS 10
#define EZ_CALLBACK_TIMEOUT 100		/* 100 * 1/100 of a second */
#define EZ_CALLBACK_WAIT 10		/* 10 milliseconds = 1/100 second */

/* PDQ queues entry definitions */

#define		FF$K_MAX_START_TIMEOUT	5000
#define		FF$K_MAX_RESET_TIMEOUT  5000
#define		FF$K_MAX_TYPE1_ENTRIES	16
#define		FF$K_MAX_TYPE2_ENTRIES	256
#define		FF$K_MAX_SMT_ENTRIES	64
#define		FF$K_MAX_RCV_DESCR	4
#define		FF$K_MAX_RSP_DESCR	2
#define		FF$K_TYPE1_QUEUES	1
#define		FF$K_TYPE2_QUEUES	2
#define		FF$K_MSG_PREFIX_SIZE	0

volatile struct FF_PB {
    struct pb 	        pb;			/* common port block */
    struct PBQ		*pbq;			/* pointer to the PBQ struct */
    struct INODE	*ip;			/* pointer to the associate INODE */
    struct SEMAPHORE	ff_port_s;		/* port semaphore */
    struct SEMAPHORE	ff_ready_s;		/* used to sync up port process */
    struct SEMAPHORE	ff_done_s;		/* completion semaphore for process */
    struct SEMAPHORE 	ff_isr_s;		/* Semaphores for interrupts */
    struct SEMAPHORE	mbx_s;			/* synchronize access to shared mailbox */
    struct TIMERQ	ff_cmd_tqe;
    struct MBX		*mbx;			/* point to mailbox */
    struct FF_MSG 	rqh;			/* Recieved msg queue header */
    FF_DESCR_BLOCK      *dbp;			/* Descriptor block */
    FF_CONSUMER_BLOCK   *cbp;			/* Consumer block pointer */
    FF_PRODUCER_INDEX   *pip;			/* Producer Index pointer */
    void (*lrp)();				/* Pointer to the datalink rcv */
    int (*lwp)();				/* Pointer to send routine */
    int			fcount;			/* count of open files that not part of the driver */
    int 		type0_vector;		/* Interrupt vectors */ 
    int			type1_vector;
    int 		type2_vector;
    int			type3_vector;
    int 		rcv_msg_cnt;		/* Number of recieved messages */
    int			CmdRspBlock;		/* Command Response buffer pointer */
    int			CmdReqBlock;		/* Command Request  buffer pointer */
    int			XmtDatBlock;		/* Xmt Data buffer pointer */
    int			UnsolBlock;		/* Command rcv buffer pointer */
    int 		password_init;	
    int			setup_init;		/* Remote boot stuff */
    int			filter_type;
    int       		loopback;		/* ff_rx looback indicator */
    int                 *saved_first_segment;	/* Saved first segment */
    U_INT_8		*bb;			/* Fbus node base address */
    U_INT_8	    	sa[6];			/* Our station address */
};
