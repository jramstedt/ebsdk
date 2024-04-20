/* File:	pdq_pb_def.h
 *
 * Copyright (C) 1994, 1995 by
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
 * Abstract:	Port Block Definitions 
 *
 * Author:	D.W. Neale
 *
 * Modifications:
 *
 *	dwn	05-Sep-1995	Added PDQ$K_RESTARTED, removed duplicate state
 *
 *	dwn	24-Feb-1994	Initial entry.
*/
#define PDQ_K_MIN_PKT_SIZE 0		/*Minimum packet size*/
#define PDQ_K_MAX_PKT_SIZE 8192		/*Maximum packet size*/

#define PDQ_K_SOB 0X80000000
#define PDQ_K_EOB 0X40000000

#define PDQ$K_UNINITIALIZED 0
#define PDQ$K_STOPPED       1
#define PDQ$K_STARTED       2
#define PDQ$K_RESTARTED     3

#define PDQ$K_OPEN_TIMEOUT_SECS 1	/*1 seconds			    */

#define PDQ_MAX_RCV_MSGS      10
#define PDQ_CALLBACK_TIMEOUT 100		/*100 * 1/100 of a second*/
#define PDQ_CALLBACK_WAIT     10		/*10 milliseconds = 1/100 second*/

/* PDQ Register Definitions
*/
#define	PDQ_PortReset		0x00
#define	PDQ_HostData		0x04
#define	PDQ_PortControl		0x08
#define	PDQ_PortDataA		0x0C
#define	PDQ_PortDataB		0x10
#define	PDQ_PortStatus		0x14
#define	PDQ_HostIntType0	0x18
#define	PDQ_HostIntEnbX		0x1C
#define	PDQ_Type2Prod		0x20
#define	PDQ_CmdRspProd		0x28
#define	PDQ_CmdReqProd		0x2C
#define	PDQ_HostSmtProd		0x30
#define	PDQ_UnsolProd		0x34

/* ESIC Register Definitions
*/                                                                        
#define ESIC_SlotCntrl		0xC84
#define ESIC_IoAdrsCmp0		0xC92
#define ESIC_IoAdrsCmp1		0xC94
#define ESIC_IntChanCfgSta	0xCA9
#define ESIC_FuncCntrl		0xCAE

/* MISC Register Definitions
*/                                                                        
#define PDQ_BurstHoldoff	0x040
#define PDQ_SlotIO		0xC80

/* PFI Register Definitions
*/
#define	PFI_ModeControl		0x40
#define	PFI_Status		0x44
#define	PFI_FifoWrite		0x48
#define	PFI_FifoRead		0x4C

struct PDQ_MSG {
    struct PDQ_MSG *flink;
    struct PDQ_MSG *blink;
    unsigned char *msg;
    int           size;
    };



volatile struct PDQ_PB {

    /* Fix Port Block structure for all devices
    /*	See pb_def.h for details.
    */
    struct pb 	        pb;

    struct INODE	*ip;		/* Pointer to associated INODE */

    /* Device Specific Port Block structure definitions
    */
    struct LOCK 	spl_port;	/* Must be aligned on a quadword */
    struct PDQ_CSRS	*rp;            /* Pointer to PDQ registers */
    char       		name[32];	/* DEFEA Port Name */
    char 		short_name[32];	/* DEFEA Port Name (Short version) */
    U_INT_8 		sa[6];		/* Our station address */
    char       		loopback;	/* pdq_rx looback indicator */
    char		fill1;

    struct SEMAPHORE 	pdq_out;	/*pdq_out syncronize*/
    struct SEMAPHORE 	pdq_stop_rx;	/*setmode(DDB$STOP), exit pdq_rx()*/
    struct SEMAPHORE 	pdq_isr_tx,	/*Semaphores for EZ interrupts*/
    			pdq_isr_rx; 

    DEFEA_EISA_MEMORY   *emp;		/* DEFEA Eisa Mapped Mem Pointer */
    unsigned int	emp_size;	/* size of DEFEA Mapped memory */

    PDQ_PRODUCER_INDEX   *pip;		/* Producer Index pointer */

    int 		XmtIntCnt,	/*Xmt Interrupt counter */
    			RcvIntCnt;	/* Rcv Interrupt counter */

    unsigned int lwbuf;

    /* The following save the calculated offsets from the beginning
    /* of DEFEA On-Board Eisa Mapped Memory.  This base adrs is saved 
    /* in pdq->emp
    */
    unsigned int
    		    	rcv_offset,	/* Receive Data Block offset */
    		    	xmt_offset,	/* Transmit Data Block offset */
    		    	CmdRsp_offset,	/* Command Response Block offset */
    		    	CmdReq_offset,	/* Command Request Block offset */
    			unsol_offset,	/* Unsolicited Block offset */
    		    	dbp_offset,	/* Descriptor Block offset */
    		    	cbp_offset;	/* Consumer Block offset */
   

    void (*lrp)(struct FILE*,char*,U_INT_16);	/* Pointer to the datalink rcv*/
    int 		rcv_msg_cnt;		/* Number of recieved messages*/
    struct PDQ_MSG 	rqh;			/* Recieved msg queue header*/

    int			XmtDatBlock,		/* Xmt Data buffer pointer */
    			CmdRspBlock,		/* Command Response buffer pointer */
    			CmdReqBlock;		/* Command Request  buffer pointer */

    int 		password_init,
    			setup_init,		/* Remote boot stuff*/
    			filter_type;
    };
