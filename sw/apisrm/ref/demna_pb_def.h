/* demna_pb_def.h
 *
 * Copyright (C) 1991, 1992, 1996 by
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
 * Abstract:	DEMNA (EX) port block Definitions
 *
 * Author:	jds - Jim Sawin
 *
 */
 

/* NOTE: demna_def.h must be included before this file. */

/*Min and max packet size for an ethernet*/

#define EX$K_MIN_PKT_SIZE 60		/*Minimum packet size*/
#define EX$K_MAX_PKT_SIZE 1518		/*Maximum packet size*/
#define EX$K_MAX_UPDATE_BLOCK 1506	/* Max EEPROM update block size */

/*General program constants                                                 */

#define EX$K_RCV_BUFS 8
#define EX$K_IPL14 0
#define EX$K_IPL15 1
#define EX$K_IPL16 2
#define EX$K_IPL17 3
#define EX$K_MSG_BUF_SIZE EX$K_MAX_PKT_SIZE
#define EX$K_MSG_PREFIX_SIZE 0
#define EX$K_MSG_MOD 0
#define EX$K_MSG_REM 0

struct EX_MSG {
    struct EX_MSG *flink;
    struct EX_MSG *blink;
    unsigned char *msg;
    int		  size;
};

/* EX port block: */

volatile struct EX_PB {
    struct pb pb;			/* Standard port block sub-struct */
    struct SEMAPHORE ex_port_s;		/* Port semaphore */
    struct SEMAPHORE ex_ready_s;	/* Used to sync up port processes */
    struct SEMAPHORE cmplt_ex_s;	/* completion semaphore for process_ex */
    int fcount;				/* Count of open files that are not part of the driver */
    U_INT_8 *bb;			/* XMI node base address */
    struct INODE *ip;			/* Pointer to associated INODE */
    struct MBX *mbx;			/* Pointer to mailbox to use on behalf of port */
    struct SEMAPHORE mbx_s;		/* Used to synchronize access to shared mailbox */
    struct PortDataBlock *pdb;		/* Pointer to Port Data Block */
    int RingReleaseCnt;			/* Ring Release Counter */
    U_INT_32 vector;			/* Device's interrupt vector */
    struct SEMAPHORE ex_isr_s;		/* XNA isr semaphore */
    struct TIMERQ ex_cmd_tqe;		/* XNA command Timer Queue Entry */
    unsigned int tx_in, tx_out;		/* Transmit ring indeces */
    unsigned int rx_in, rx_out;		/* Receive ring indeces */
    int (*lrp)();			/* Pointer to the datalink rcv */
    int (*lwp)();			/* Pointer to send routine */
    int rcv_msg_cnt;			/* Number of received messages */
    struct EX_MSG rqh;			/* Recieved msg queue header */
    U_INT_8 sa[6];			/* Our station address */
    U_INT_8 pad[2];
    U_INT_8 level;			/* Device interrupt level (0-3 => IPL14-17) */
    U_INT_8 pad1;
    U_INT_16 mask;			/* Device interrupt mask */
    };
