/*
 * Copyright (C) 1998 by
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
 * Abstract:	Intel 82558 port block Definitions
 *
 * Author:	D. Golden
 *
 * Modifications:
 *
 */
 
/* Driver State */
#define	XP_K_UNINITIALIZED	0
#define	XP_K_HARDWARE_INIT	1
#define	XP_K_STOPPED		2
#define	XP_K_STARTED		3

/* Modes */
#define XP_K_NOT_USED	0			/*Not used		    */
#define XP_K_AUTO	1			/*Auto sensing		    */
#define XP_K_TWISTED	2			/*Twisted Pair		    */
#define XP_K_AUI	3			/*AUI			    */
#define XP_K_BNC	4			/*BNC			    */
#define XP_K_TWISTED_FD	5			/*Full Duplex, Twisted Pair */
#define XP_K_FAST	6			/*Fast 100BaseT		    */
#define XP_K_FASTFD	7			/*Fast 100BaseT Full Duplex */
#define XP_K_AUTO_NEGOTIATE 8			/*Auto negotiate	    */

/*Mode flag passed to OS in ???? */	

#define TU_K_OS_NOT_USED   0			/*Not used		    */
#define TU_K_OS_TWISTED	   0x300		/*Twisted Pair		    */
#define TU_K_OS_TWISTED_FD 0x400		/*Full Duplex, Twisted Pair */
#define TU_K_OS_AUI	   0x100		/*AUI			    */
#define TU_K_OS_BNC	   0x200		/*BNC			    */
#define TU_K_OS_F          0x500		/*FAST Half Duplex 100BaseTx*/
#define TU_K_OS_FFD        0x600		/*FAST Full Duplex 100BaseTx*/
#define TU_K_OS_FX         0x700		/*FAST Half Duplex 100BaseFx*/
#define TU_K_OS_FFX   	   0x800		/*FAST Full Duplex 100BaseFx*/
#define TU_K_OS_AN	   0x1000		/*Auto Negotiation	    */

/*Min and max packet size for an ethernet*/
#define EI_MIN_PKT_SIZE 60			/*Minimum packet size*/
#define EI_CRC_SIZE 4				/*CRC size	     */
#define EI_MAX_PKT_SIZE NDL$K_MAX_PACK_LENGTH 	/*Maximum packet size*/

/*Default environment vars*/
#define EI_XMT_MOD 512			/*Environment variable defaults     */
#define EI_XMT_REM 0
#define EI_RCV_MOD 512
#define EI_RCV_REM 0
#define EI_MSG_BUF_SIZE EI_MAX_PKT_SIZE
#define EI_MSG_MOD 0
#define EI_MSG_REM 0
#define EI_XMT_BUF_CNT 8
#define EI_RCV_BUF_CNT 8
/* #define EI_RCV_BUF_CNT 2	/* just for stress testing */

/*Callback message received*/
#define EI_MAX_RCV_MSGS 10
#define EI_CALLBACK_TIMEOUT 100		/*100 * 1/100 of a second*/
#define EI_CALLBACK_WAIT 10		/*10 milliseconds = 1/100 second*/
struct EI_MSG {
    struct EI_MSG *flink;
    struct EI_MSG *blink;
    unsigned char *msg;
    int size;
};

#define EI_MAX_PHY  2		    /* maximum number of PHY's per interface */

/* I82558 port block structure*/
volatile struct I82558PB
{
    struct pb pb;                   /*System port block             */
    struct LOCK spl_port;	    /*Must be aligned on a quadword */
    struct POLLQ pqp;		    /*Poll queue                    */
    struct INODE *ip;		    /* Pointer to associated INODE */
    struct SEMAPHORE rx_isr;	    /* receive process semaphore */
    struct SEMAPHORE tx_wait;	    /* allow ei_send to wait */
#if STARTSHUT
    struct SEMAPHORE cmplt_rx_s;    /* Receive process semaphore */
    struct SEMAPHORE ei_port_s;	    /* Port driver semaphore */
    struct SEMAPHORE ei_ready_s;    /* Used to sync up port processes */
#endif
    struct TIMERQ *tx_isr;	    /* transmit timer queue/semaphores */
    struct MOP$_V4_COUNTERS *mc;    /* Pointer to the MOP counters */
    void (*lrp)(struct FILE*,char*,U_INT_16); /*Pointer to the datalink rcv*/
    int (*lwp)();			/* Pointer to send routine */
    int mode;				/*Driver mode			*/
    int state;				/*Driver state flag		*/
    int rcv_msg_cnt;			/*Number of recieved messages*/
    struct EI_MSG rqh;			/* rcv message queue */
    int fcount;				/* # of open files no by driver */

    volatile struct xp_cb *cbl_base;	/* pointer to the list of cmd blocks */
    volatile struct xp_cb *cbl_first;	/* pointer to the first active command block */
    volatile struct xp_cb *cbl_last;	/* pointer to the last active command block */
    volatile int cb_active;		/* count of active commands */
    volatile struct xp_rfd *rfd_base;	/* pointer to the list of rx frame desc */
    volatile struct xp_rfd *rfd_first;	/* pointer to the first complete rx */
    volatile struct xp_rfd *rfd_last;	/* pointer to the last available rx */
    volatile struct xp_rbd *rbd_base;	/* pointer to the first rx buf desc */
    volatile struct xp_rbd *rbd_first;	/* pointer to the first used rx buf desc */
    volatile struct xp_rbd *rbd_last;	/* pointer to the last avail rx buf desc */

    unsigned char sa[8];	    /* station address */
    unsigned char phy_adr[EI_MAX_PHY];	    /* phy records from the eeprom */
    unsigned char phy_dev[EI_MAX_PHY];
    unsigned char connectors;	    /* available connectors from the eeprom */
    unsigned char pad[3];
    int	selected_phy;		    /* which PHY interface? */
    int loopback;		    /* 0 = none, 1 = internal, 2 = ext. */
    int rom_bits;		    /* # of address bits in the ROM */
    volatile int ru_restarts;
    volatile int cu_restarts;
    volatile int cu_timeouts;
    volatile int last_scb_status;   /* for debug */
    volatile struct xp_cb *last_cb_in; /* for debug */
    volatile struct xp_cb *last_cb_out; /* for debug */
    volatile int last_index;
    volatile int cb_highwater;
    volatile int last_to_index;
};
