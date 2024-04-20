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
 *	kcq	 8-Feb-1993	adapter from Cobra land
 *	dwn	16-Mar-1992	continue development
 *	jad	12-Mar-1991	Initial entry.
 */
 
typedef unsigned long int PORT_ULONG;

/*+
/* PDQ Chip CSRs. Note that the offsets for the PDQ CSRs also apply in    
/* memory space when the CSRs are selected to be memory mapped.           
-*/                                                                        
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
#define	 	FBUS_INTERRUPT_ADDRESS	0xA00
#define		FBUS_INTERRUPT_DATA	0xA04

/*+
/* Additional port definitions
-*/
#define FF_RCV_DES_K_BUFF_HI_SHIFT  32
#define FF_RCV_DESCR_K_128          128
#define FF_M_COMP_BLOCK_PORT_DATA_B 0xffff
#define FF_K_COMP_BLK_SHIFT_DATA_B  32
#define FF_M_DESCR_BLK_PORT_DATA_B  0xffff0000
#define FF_K_DESCR_BLK_SHIFT_DATA_B 32

/*
/* Shared host memory modulus and remainder
-*/
#define FF_K_DESC_BLK_MOD 4096
#define FF_K_DESC_BLK_REM 0
#define FF_K_COMP_BLK_MOD 64
#define FF_K_COMP_BLK_REM 0

/*+
/* Descriptor constants
-*/
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

/*+
/* Min and max packet size for an FDDI
-*/
#define FF_K_MIN_PKT_SIZE 0		/*Minimum packet size*/
#define FF_K_MAX_PKT_SIZE 8192		/*Maximum packet size*/

/*+
/* End of additional port definitions
-*/

/*+
/* Register mask
-*/
#define FF_M_CSR_MASK -1

/*+
/*
-*/
#define FF_K_SOB 0X80000000
#define FF_K_EOB 0X40000000

/*+
/* How we'll fill in some queues
-*/
#define FF_K_CMD_RSP_FILL 4
#define FF_K_UNSOL_FILL   4
#define FF_K_SMT_FILL     4

/*+
/* Environment variables
-*/
#define FF_K_XMT_MAX_SIZE FF_K_MAX_PKT_SIZE
#define FF_K_XMT_INT_MSG   1
#define FF_K_XMT_MSG_POST  1
#define FF_K_XMT_FULL_POST 1
#define FF_K_RCV_BUF_NO    16
#define FF_K_MSG_BUF_SIZE  DYN$K_NILOOKASIDE
#define FF_K_MSG_MOD       0
#define FF_K_MSG_REM       0
#define FF_K_MODE_NORMAL   0

/*+
/* Driver flags
-*/
#define FF_K_DRIVER_FLAGS_DEF 0		/*Driver flags default*/
#define	FF_M_BROADCAST        1		/*Broadcast*/
#define	FF_M_ENA_BROADCAST    1		/*Enable broadcast*/
#define	FF_M_DIS_BROADCAST    0		/*Disable broadcast*/

/*+
/* Queue definitions
-*/
#define FF_MAX_QUEUE_NAME 80		/*Maximum size of a queue name*/

/*+
/* General queue descriptor
-*/
volatile struct FF_GEN_DESCR {
    PORT_ULONG	long_0;
    PORT_ULONG	long_1;
    };

/*+
/* Process queue information
-*/
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

/*+
/* FF csrs
-*/
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

/*+
/* State constants
-*/
#define FF$K_UNINITIALIZED 0
#define FF$K_STOPPED       1
#define FF$K_STARTED       2

/*+
/* General program constants
-*/
#define SIZE$K_LW	       4	/* four bytes to a longword */
#define SIZE$K_BYTE	       1	/* 1 bytes to 1 byte */
#define FF$K_OPEN_TIMEOUT_SECS 1	/*1 seconds			    */

/*+
/* Callback message received
-*/
#define FF_MAX_RCV_MSGS      10
#define FF_CALLBACK_TIMEOUT 100		/*100 * 1/100 of a second*/
#define FF_CALLBACK_WAIT     10		/*10 milliseconds = 1/100 second*/

struct FF_MSG {
    struct FF_MSG *flink;
    struct FF_MSG *blink;
    unsigned char *msg;
    int           size;
    };
