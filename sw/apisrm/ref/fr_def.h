/* File:	fr_def.h
 *
 * Copyright (C) 1994 by
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
 * Abstract:	DEFEA Eisa/FDDI Port Definitions 
 *
 * Author:	D.W. Neale
 *
 * Modifications:
 *
 *	dwn	24-Feb-1994	Initial entry.
*/

typedef unsigned long int PORT_ULONG;

/* System constants
*/
#define PDQ_K_VECTOR    0x850

/* Default DEFEA On-Board, Eisa Memory Mapped, Buffer Constants
*/
#define PDQ_K_EISA_ADDR 0xD0000		/* Base Eisa Memory Mapped */
#define PDQ_K_EISA_SIZE 0x10000		/* Size of DEFEA On-Board Memory */

/* Offsets into PDQ On-Board, Eisa Memory Mapped Buffer
*/
#define PDQ_K_RcvDesc	0x0000 /* */
#define PDQ_K_XmtDesc	0x0000 /* */
#define PDQ_K_SmtDesc	0x0000 /* */
#define PDQ_K_UnsolDesc	0x0000 /* */
#define PDQ_K_CmdRes	0x0000 /* */
#define PDQ_K_CmdReq	0x0000 /* */
#define PDQ_K_XmtRcvData	0x0000 /* */
#define PDQ_K_SmtHost	0x0000 /* */
#define PDQ_K_Unsol	0x0000 /* */
#define PDQ_K_CmdRsp	0x0000 /* */
#define PDQ_K_CmdReq	0x0000 /* */
#define PDQ_K_XmtBuf	0x0000 /* */
#define PDQ_K_RcvBuf	0x0000 /* */




/* Additional port definitions
*/
#define PDQ_RCV_DES_K_BUFF_HI_SHIFT  32
#define PDQ_RCV_DESCR_K_128          128
#define PDQ_M_COMP_BLOCK_PORT_DATA_B 0xffff
#define PDQ_K_COMP_BLK_SHIFT_DATA_B  32
#define PDQ_M_DESCR_BLK_PORT_DATA_B  0xffff0000
#define PDQ_K_DESCR_BLK_SHIFT_DATA_B 32

/* Shared host memory modulus and remainder
*/
#define PDQ_K_DESC_BLK_MOD 4096
#define PDQ_K_DESC_BLK_REM 0
#define PDQ_K_COMP_BLK_MOD 64
#define PDQ_K_COMP_BLK_REM 0

/* Descriptor constants
*/
#define PDQ_K_XMT_DATA_CNT       256
#define PDQ_K_XMT_DATA_MAX_BYTES 8192
#define PDQ_K_XMT_DATA_ALIGN     0
#define PDQ_K_XMT_DATA_CHAIN     2
#define PDQ_K_RCV_DATA_CNT       256
#define PDQ_K_RCV_DATA_MAX_BYTES 8192
#define PDQ_K_RCV_DATA_ALIGN     512
#define PDQ_K_RCV_DATA_CHAIN     2
#define PDQ_K_CMD_REQ_CNT        16
#define PDQ_K_CMD_REQ_MAX_BYTES  512
#define PDQ_K_CMD_REQ_ALIGN      0 
#define PDQ_K_CMD_REQ_CHAIN      1
#define PDQ_K_CMD_RSP_CNT        16
#define PDQ_K_CMD_RSP_MAX_BYTES  512
#define PDQ_K_CMD_RSP_ALIGN      512
#define PDQ_K_CMD_RSP_CHAIN      1
#define PDQ_K_UNSOL_CNT          16
#define PDQ_K_UNSOL_MAX_BYTES    512
#define PDQ_K_UNSOL_ALIGN        512
#define PDQ_K_UNSOL_CHAIN        1
#define PDQ_K_SMT_CNT            64
#define PDQ_K_SMT_MAX_BYTES      8192
#define PDQ_K_SMT_ALIGN          512
#define PDQ_K_SMT_CHAIN          2

/* Min and max packet size for an FDDI
*/
#define PDQ_K_MIN_PKT_SIZE 0		/*Minimum packet size*/
#define PDQ_K_MAX_PKT_SIZE 8192		/*Maximum packet size*/

/* Register mask
*/
#define PDQ_M_CSR_MASK -1

/*
*/
#define PDQ_K_SOB 0X80000000
#define PDQ_K_EOB 0X40000000


/* How we'll fill in some queues
*/
#define PDQ_K_CMD_RSP_FILL 4
#define PDQ_K_UNSOL_FILL   4
#define PDQ_K_SMT_FILL     4

/* Environment variables
*/
#define PDQ_K_XMT_MAX_SIZE PDQ_K_MAX_PKT_SIZE
#define PDQ_K_XMT_INT_MSG   1
#define PDQ_K_XMT_MSG_POST  1
#define PDQ_K_XMT_FULL_POST 1
#define PDQ_K_RCV_BUF_NO    16
#define PDQ_K_MSG_BUF_SIZE  DYN$K_NILOOKASIDE
#define PDQ_K_MSG_MOD       0
#define PDQ_K_MSG_REM       0
#define PDQ_K_MODE_NORMAL   0

/* Driver flags
*/
#define PDQ_K_DRIVER_FLAGS_DEF 0		/*Driver flags default*/
#define	PDQ_M_BROADCAST        1		/*Broadcast*/
#define	PDQ_M_ENA_BROADCAST    1		/*Enable broadcast*/
#define	PDQ_M_DIS_BROADCAST    0		/*Disable broadcast*/

/* Queue definitions
*/
#define PDQ_MAX_QUEUE_NAME 80		/*Maximum size of a queue name*/

/* General queue descriptor
*/
volatile struct PDQ_GEN_DESCR {
    PORT_ULONG	long_0;
    PORT_ULONG	long_1;
    };

/* Process queue information
*/
volatile struct PDQ_QUEUE_INFO {
    struct SEMAPHORE    is;		/*Pointer to the isr semaphore*/
    struct SEMAPHORE    fs;		/*Pointer to the full semaphore*/
    struct PDQ_GEN_DESCR *dp;		/*Descriptor pointer*/	 
    int                 qs;		/*Size of the queue*/
    unsigned char *ci, *hc;		/*Completion index, host consumer index*/	
    unsigned char *pi;			/*Producer index*/	
    struct INODE  *ip;			/*Pointer to the port device INODE*/
    void (*rp)(struct PDQ_GEN_DESCR *dp);/*Pointer to a routine to be executed */
    };



    /* State constants
    */
#define PDQ$K_UNINITIALIZED 0
#define PDQ$K_STOPPED       1
#define PDQ$K_STARTED       2

    /* General program constants
    */
#define SIZE$K_LW	       4	/* four bytes to a longword */
#define SIZE$K_BYTE	       1	/* 1 bytes to 1 byte */
#define PDQ$K_OPEN_TIMEOUT_SECS 1	/*1 seconds			    */

    /* Callback message received
    */
#define PDQ_MAX_RCV_MSGS      10
#define PDQ_CALLBACK_TIMEOUT 100		/*100 * 1/100 of a second*/
#define PDQ_CALLBACK_WAIT     10		/*10 milliseconds = 1/100 second*/

struct PDQ_MSG {
    struct PDQ_MSG *flink;
    struct PDQ_MSG *blink;
    unsigned char *msg;
    int           size;
    };

 
#define defea_k_reg_base 0x0000

/* PDQ Register Structure
*/
volatile struct PDQ_CSRS {
    unsigned int PDQ_PortReset;
    unsigned int PDQ_HostData;			
    unsigned int PDQ_PortControl;			
    unsigned int PDQ_PortDataA;
    unsigned int PDQ_PortDataB;
    unsigned int PDQ_PortStatus;			
    unsigned int PDQ_HostIntType0;
    unsigned int PDQ_HostIntEnbX;
    unsigned int PDQ_Type2Prod;
    unsigned int PDQ_rsvd1;
    unsigned int PDQ_CmdRspProd;
    unsigned int PDQ_CmdReqProd;
    unsigned int PDQ_HostSmtProd;
    unsigned int PDQ_UnsolProd;
    };

/* ESIC Register Definitions
*/                                                                        
#define         ESIC_SlotCntrl		0xC84
#define         ESIC_IoAdrsCmp0		0xC92
#define         ESIC_IoAdrsCmp1		0xC94
#define         ESIC_IntChanCfgSta	0xCA9
#define         ESIC_FuncCntrl		0xCAE

/* MISC Register Definitions
*/                                                                        
#define         PDQ_BurstHoldoff	0x040
#define         PDQ_SlotIO		0xC80



 
/* DEFEA port block structure
*/
volatile struct PDQ_PB {

    /* Fix Port Block structure for all devices
    /*	See pb_def.h for details.
    */
    struct pb 	        pb;

    /* Device Specific Port Block structure definitions
    */
    struct LOCK 	spl_port;	/* Must be aligned on a quadword */
    struct PDQ_CSRS	*rp;            /* Pointer to PDQ registers */
    char       		name[32];	/* DEFEA Port Name */
    char 		short_name[32];	/* DEFEA Port Name (Short version) */
    U_INT_8 		sa[6];		/* Our station address */
    char       		loopback;	/* fr_rx looback indicator */
    char		fill1;

    struct SEMAPHORE 	fr_out;		/*fr_out syncronize*/
    struct SEMAPHORE 	fr_stop_rx;	/*setmode(DDB$STOP), exit fr_rx()*/
    struct SEMAPHORE 	fr_isr_tx,	/*Semaphores for EZ interrupts*/
    			fr_isr_rx; 

    DEFEA_EISA_MEMORY   *emp;		/* DEFEA Eisa Mapped Mem Pointer */
    unsigned int	emp_size;	/* size of DEFEA Mapped memory */

    PDQ_PRODUCER_INDEX   *pip;		/* Producer Index pointer */

    int 		XmtIntCnt,	/*Xmt Interrupt counter */
    			RcvIntCnt;	/* Rcv Interrupt counter */

    int 		type0_vector,	/* Interrupt vectors */ 
    			type1_vector,
    			type2_vector, 
    			type3_vector;

    int 		me_cnt,		/* fr_control_t counters */
    			rw_cnt,
    			tw_cnt,
    			bo_cnt;

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
    			filter_type,
    			state;			/* Driver state*/
    };
