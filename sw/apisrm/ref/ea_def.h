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
 * Abstract:	EA port definitions (ea_def) Definitions
 *
 * Author:	John A. DeNisco
 *
 * Modifications:
 *
 *	jad	13-Nov-1990	Initial entry.
 */
 
/*Macros*/
/*Write a quadword in our environment*/
#define wq(x,v) (x.l=v, x.h=0)
/*Read a quadword in our environment*/
#define rq(x) (x.l)

/*Hardware constants*/
#define EA$K_NI_TIRQ_CHAN 0		/*NI tx channel*/			
#define EA$K_NI_RIRQ_CHAN 1		/*NI rx channel*/
#define EA$K_NI_RBR 0x00002100		/*NI base reg*/
#define EA$K_NI_ICR 0x00002200		/*NI Interrupt control reg*/
#define EA$K_NI_DBR 0x00006000		/*NI doorbell reg*/

/*Type codes*/
#define EA$K_EMPTY 0
#define EA$K_IO_MODULE 1
#define EA$K_64MB_STORAGE_MODULE 2
#define EA$K_CPU_DC227 3
#define EA$K_CPU_DC228 4

/*Flag codes*/
#define EA$K_BROKEN 1
#define EA$K_PRIMARY_CPU 2
#define EA$K_CONSOLE 4

/*Packet codes*/
#define EA$K_MIN_PKT_SIZE 60		/*Minimum packet size*/
#define EA$K_MAX_PKT_SIZE 1536		/*Maximum packet size*/

/*Environment variable defaults*/
#define EA$K_XMT_BUF_CNT 64            /*Environment variable defaults     */
#define EA$K_XMT_MOD 512
#define EA$K_XMT_REM 0
#define EA$K_RCV_BUF_CNT 64
#define EA$K_RCV_MOD 512
#define EA$K_RCV_REM 0
#define EA$K_MSG_BUF_SIZE DYN$K_NILOOKASIDE
#define EA$K_MSG_MOD 0
#define EA$K_MSG_REM 0

/*Ethernet FLAG codes*/
#define EA$K_ETHER_EMPTY 0		/**/
#define EA$K_ETHER_TRANSMIT_PACKET 1	/**/
#define EA$K_ETHER_INITIALIZE 2		/**/
#define EA$K_ETHER_RECEIVE_PACKET 3	/**/

/*Ethernet STATUS codes*/
#define EA$K_ETHER_OK 0			/**/
#define EA$K_ETHER_REJECT 1		/**/
#define EA$K_ETHER_COLLISION 2		/**/
#define EA$K_ETHER_FRAMING 3		/**/
#define EA$K_ETHER_CRC 4		/**/
#define EA$K_ETHER_FIRMWARE 5		/**/

/*Ethernet RXMODE codes*/
#define EA$K_ETHER_NORMAL 0		/**/
#define EA$K_ETHER_PROMISCUOUS 1	/**/
#define EA$K_MULTICAST_ENA 2		/**/

/*Csrs*/
volatile struct ETHER_ICR_BITS		/*Interrupt control register*/
{
	unsigned re		: 1;	/* Rx interrupt enabled*/
	unsigned te		: 1;	/* Tx interrupt enabled*/
	unsigned irq_node	: 4;	/* Node where interrupt is delivered to*/
	unsigned rirq_chan	: 5;	/* Channel number for rx interrupts*/
	unsigned tirq_chan	: 5;	/* Channel number for tx interrupts*/
};

/*Descriptors*/
volatile struct ETHER_INITIALIZE
{
	U_INT_32 flag;			/* */
	U_INT_32 status;		/* */
	U_INT_32 mbz0[6];		/* */
	U_INT_64 rxmode;		/* Receive mode			*/
	U_INT_64 rxaddr;		/* Physical receive address	*/
	U_INT_64 lamask;		/* Filter mask			*/
	U_INT_32 mbz1[2];		/* */
};

volatile struct ETHER_TRANSMIT_PACKET
{
	U_INT_32 flag;			/* */
	U_INT_32 status;		/* */
	U_INT_32 ncol;			/* Collision count		*/
	U_INT_32 mbz0;			/* */
	U_INT_64 mbz1[2];		/* */
	U_INT_64 bufp;			/* Buffer pointer		*/
	U_INT_32 ntbuf;			/* Number of bytes transmitted	*/
	U_INT_32 mbz2;			/* */
	U_INT_64 mbz3[2];		/* */
};

volatile struct ETHER_RECEIVE_PACKET
{
	U_INT_32 flag;			/* */
	U_INT_32 status;		/* */
	U_INT_32 nrbuf;			/* Nuber of bytes		*/
	U_INT_32 mbz0;			/* */
	U_INT_64 mbz1[2];		/* */
	U_INT_64 bufp;			/* Buffer pointer		*/
	U_INT_64 mbz2[3];		/* */
};

/*Configuration feilds*/
volatile struct EA_CONFIG
{
	U_INT_32 type;			/* Module Type			*/
	U_INT_32 flags;			/* */
	U_INT_32 tspc[14];		/* Type specific		*/
};

volatile struct EA_IO_MODULE_TYPE
{
	U_INT_32 type;			/* Module Type			*/
	U_INT_32 flags;			/* */
	U_INT_8 sa[6];			/* Ethernet station address	*/
	U_INT_8 mbz[2];			/* */
	U_INT_32 scsi_id;		/* */
};

/*General program constants                                                 */
#define EA$K_XMT_TIMEOUT 10*1000	/* 10 seconds			    */

/*Transmit info structure and constants*/
/*This structure will be allocated in parallel*/
/*with the transmit descriptors. This structure*/
/*is used to pass information about the packet*/
/*to the transmit process ea_tx*/ 

#define EA$K_TX_ASYNC 0			/*Transmits will be asyncronous*/
#define EA$K_TX_SYNC 1			/*Transmits will be syncronous*/

volatile struct EA_TX_INFO {
    struct TIMERQ *tx_timer; 		/*TX timer*/
    int sync_flag;			/*Synchronous flag*/
    int type_flag;			/*Type of descriptor*/
};

/*EA structure*/
volatile struct EA_PB {
    struct csr rbr;			/*Csr copies*/
    struct csr icr;
    struct csr dbr;
    unsigned char sa[6];		/*Our station address*/
    int cpu_node_id,io_node_id;		/*CPU and IO node IDs*/
    int rx_vector,tx_vector;		/*RX and TX vectors*/
    int rx_chan,tx_chan;		/*RX and TX channels*/
    struct SEMAPHORE isr_tx,isr_rx;	/*Semaphores for EA interrupts*/
    struct ETHER_RECEIVE_PACKET *rdes;	/*Pointer to rcv descriptors*/
    int rx_index;			/*Receive index*/
    struct ETHER_TRANSMIT_PACKET *tdes_in,*tdes_out; /*Pointers to xmt descriptors*/
    struct EA_TX_INFO *ti_in,*ti_out; 	/*Transmit info pointers*/
    int tx_index_in,tx_index_out;	/*Transmit index*/
    int tx_pkt_cnt;		        /*Transmit packet count*/
    void (*lrp)(struct FILE*,char*,unsigned short); /*Pointer to the datalink rcv*/
    int ti_cnt,ri_cnt;			/*Interrupt counters and flags*/
    U_INT_32 saved_tx_status;		/*Saved TX status*/
    };
