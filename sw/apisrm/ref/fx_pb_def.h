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
 * Abstract:	FX port block (fx_pb) Definitions
 *
 * Author:	jds - Jim Sawin
 *
 * Modifications:
 *
 *	jds	15-Jun-1992	Added fx_port_s, fx_ready_s, and fcount.
 *	jds	 8-May-1992	Rearranged fields to eliminate Alpha alignment problems.
 *	jds	4-Feb-1992	Initial entry; based on ex_pb_def.h.
 */

/* NOTE: xfa_def.h must be included before this file. */

/*Min and max packet size for an ethernet*/
#define FX$K_MIN_PKT_SIZE 60		/*Minimum packet size*/
#define FX$K_MAX_PKT_SIZE 1518		/*Maximum packet size*/

/*General program constants                                                 */
#define FX$K_RCV_BUFS 8
#define FX$K_UNSOL_BUFS 4
#define FX$K_IPL14 1
#define FX$K_IPL15 2
#define FX$K_IPL16 4
#define FX$K_IPL17 8
#define fx$k_tx_rx_ring 1
#define fx$k_cmd_unsol_ring 2
#define FX$K_MSG_PREFIX_SIZE 3
#define FX$K_MSG_BUF_SIZE FX$K_MAX_PKT_SIZE
#define FX$K_MSG_MOD 32			/* Must be hexaword-aligned */
#define FX$K_MSG_REM 0

/* FX port block: */

volatile struct FX_PB {
    struct pb pb;			/* Standard port block sub-struct */
    struct SEMAPHORE fx_port_s;		/* Port semaphore */
    struct SEMAPHORE fx_ready_s;	/* Used to sync up port processes */
    struct SEMAPHORE cmplt_fx_s;	/* completion sem for process_fx */
    int fcount;				/* Count of open files that are not part of the driver */
    U_INT_8 *bb;			/* XMI node base address */
    struct INODE *ip;			/* Pointer to associated INODE */
    struct MBX *mbx;			/* Pointer to mailbox to use on behalf of port */
    struct SEMAPHORE mbx_s;		/* Used to synchronize access to shared mailbox */
    struct SEMAPHORE fx_isr_s;		/* XFA isr semaphore */
    struct PortDataBlock *pdb;		/* Pointer to Port Data Block */
    struct TIMERQ fx_cmd_tqe;		/* XFA command Timer Queue Entry */
    unsigned int cmd_in, cmd_out;	/* Command ring indeces */
    unsigned int unsol_in, unsol_out;	/* Unsolicited command ring indeces */
    unsigned int tx_in, tx_out;		/* Transmit ring indeces */
    unsigned int rx_in, rx_out;		/* Receive ring indeces */
    unsigned int tx_hib, rx_hib;	/* Transmit/Receive hibernation addresses */
    void (*lrp)(struct FILE*,char*,U_INT_16);	/* Pointer to the datalink rcv */
    int (*lwp)();			/* Pointer to send routine */
    U_INT_8 MLA[6];			/* Our station address */
    U_INT_8 level;			/* Device interrupt level (0-3 => IPL14-17) */
    U_INT_8 pad;
    U_INT_32 vector[3];			/* Device's interrupt vectors */
    };
