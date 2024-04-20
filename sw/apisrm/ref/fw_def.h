/* File:	fw_def.h
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
 * Abstract:	DEFPA port definitions
 *
 * Author:	John R. Kirchoff
 *
 * Modifications:
 *
 *	jrk	17-Aug-1994	Initial entry.
 *
 *	dpm	16-Mar-1995	added eisc defines to merge fr and fw drivers.
 */
 
typedef unsigned long int PORT_ULONG;

/*+
 * PDQ Chip CSRs.
 * Note that the offsets for the PDQ CSRs also apply in    
 * memory space when the CSRs are selected to be memory mapped.           
-*/                                                                        

#define	PDQ_PortReset		0x00
#define	PDQ_HostData		0x04
#define	PDQ_PortControl		0x08
#define	PDQ_PortDataA		0x0C
#define	PDQ_PortDataB		0x10
#define	PDQ_PortStatus		0x14
#define	PDQ_HostIntType0	0x18
#define	PDQ_HostIntEnbX		0x1C
#define	PDQ_Type2Prod		0x20
#define	PDQ_rsvd1		0x24
#define	PDQ_CmdRspProd		0x28
#define	PDQ_CmdReqProd		0x2C
#define	PDQ_HostSmtProd		0x30
#define	PDQ_UnsolProd		0x34
#define	PDQ_rsvd2		0x38
#define	PDQ_rsvd3		0x3C

#define	PFI_ModeControl		0X40
#define	PFI_Status		0X44
#define	PFI_FifoWrite		0X48
#define	PFI_FifoRead		0X4C


#define         ESIC_SlotCntrl		0xC84
#define         ESIC_IoAdrsCmp0		0xC92
#define         ESIC_IoAdrsCmp1		0xC94
#define         ESIC_IntChanCfgSta	0xCA9
#define         ESIC_FuncCntrl		0xCAE


/*+
 * Port Control Register - Command codes for primary commands 
-*/
#define PDQ_PCTRL_M_CMD_ERROR		0x8000
#define PDQ_PCTRL_M_BLAST_FLASH		0x4000
#define PDQ_PCTRL_M_HALT		0x2000
#define PDQ_PCTRL_M_COPY_DATA		0x1000
#define PDQ_PCTRL_M_ERROR_LOG_START	0x0800
#define PDQ_PCTRL_M_ERROR_LOG_READ	0x0400
#define PDQ_PCTRL_M_XMT_DATA_FLUSH_DONE	0x0200
#define PDQ_PCTRL_M_INIT		0x0100
#define PDQ_PCTRL_M_INIT_START		0X0080
#define PDQ_PCTRL_M_CONS_BLOCK		0x0040
#define PDQ_PCTRL_M_UNINIT		0x0020
#define PDQ_PCTRL_M_RING_MEMBER		0x0010
#define PDQ_PCTRL_M_MLA			0x0008		
#define PDQ_PCTRL_M_FW_REV_READ		0x0004
#define PDQ_PCTRL_M_DEV_SPECIFIC	0x0002
#define PDQ_PCTRL_M_SUB_CMD		0x0001

/*+
 * Define sub-commands accessed via the PDQ_PCTRL_M_SUB_CMD command 
-*/
#define PDQ_SUB_CMD_K_LINK_UNINIT	0X0001
#define PDQ_SUB_CMD_K_BURST_SIZE_SET	0X0002

/*+
 * Define some Port Data B values 
-*/
#define PDQ_PDATA_B_DMA_BURST_SIZE_4     0       /* Valid values for command */
#define PDQ_PDATA_B_DMA_BURST_SIZE_8     1
#define PDQ_PDATA_B_DMA_BURST_SIZE_16    2
#define PDQ_PDATA_B_DMA_BURST_SIZE_32    3

/*+
 * Define timeout period for all port control commands but blast_flash 
-*/
#define PDQ_PCTRL_K_TIMEOUT	2000		/* millisecs */
#define	PDQ_MAX_START_TIMEOUT	5000
#define	PDQ_MAX_RESET_TIMEOUT 	5000

/*+
 * Port Data A Reset state 
-*/
#define PDQ_PDATA_A_RESET_M_UPGRADE	0x00000001
#define PDQ_PDATA_A_RESET_M_SOFT_RESET	0x00000002
#define PDQ_PDATA_A_RESET_M_SKIP_ST	0x00000004

#define PDQ_PDATA_A_MLA_K_LO		0
#define PDQ_PDATA_A_MLA_K_HI		1

typedef struct port_data_a {
  union {
    PORT_ULONG	da;
    struct {
      unsigned upgrade : 1;
      unsigned soft_reset : 1;
      unsigned skip_selftest : 1;
      unsigned reserv : 29;
    } pda;
  } reg;
} PDA;

/*+
 * Port Reset Register 
-*/
#define PDQ_RESET_M_ASSERT_RESET		1

/*+
 * Port Status register 
-*/
#define PDQ_PSTATUS_V_RCV_DATA_PENDING	31
#define PDQ_PSTATUS_V_XMT_DATA_PENDING	30
#define PDQ_PSTATUS_V_SMT_HOST_PENDING	29
#define PDQ_PSTATUS_V_UNSOL_PENDING	28
#define PDQ_PSTATUS_V_CMD_RSP_PENDING	27
#define PDQ_PSTATUS_V_CMD_REQ_PENDING	26
#define PDQ_PSTATUS_V_TYPE_0_PENDING	25
#define PDQ_PSTATUS_V_RESERVED_1	16
#define PDQ_PSTATUS_V_RESERVED_2	11
#define PDQ_PSTATUS_V_STATE		8
#define PDQ_PSTATUS_V_HALT_ID		0

typedef struct fw_statreg {
  union {
    PORT_ULONG	statreg;
    struct {
      unsigned halt_id : 8;        
      unsigned adapter_state : 3;
      unsigned resrv0 : 5;
      unsigned resrv1 : 9;
      unsigned type_0_pending : 1;
      unsigned cmd_req_pending : 1;
      unsigned cmd_rsp_pending : 1;
      unsigned unsol_pending : 1;
      unsigned smt_host_pending : 1;      
      unsigned xmt_data_pending : 1;	
      unsigned rev_data_pending : 1;	
    } status;
  } reg;
} PORT_STATREG;

#define PDQ_PSTATUS_M_RCV_DATA_PENDING	0X80000000
#define PDQ_PSTATUS_M_XMT_DATA_PENDING	0X40000000
#define PDQ_PSTATUS_M_SMT_HOST_PENDING	0X20000000
#define PDQ_PSTATUS_M_UNSOL_PENDING	0X10000000
#define PDQ_PSTATUS_M_CMD_RSP_PENDING	0X08000000
#define PDQ_PSTATUS_M_CMD_REQ_PENDING	0X04000000
#define PDQ_PSTATUS_M_TYPE_0_PENDING	0X02000000
#define PDQ_PSTATUS_M_RESERVED_1	0X01FF0000
#define PDQ_PSTATUS_M_RESERVED_2	0X0000F800
#define PDQ_PSTATUS_M_STATE		0X00000700
#define PDQ_PSTATUS_M_HALT_ID		0X000000FF

/*+
 * Define adapter states 
-*/
#define PDQ_STATE_K_RESET		0
#define PDQ_STATE_K_UPGRADE		1
#define PDQ_STATE_K_DMA_UNAVAIL		2
#define PDQ_STATE_K_DMA_AVAIL		3
#define PDQ_STATE_K_LINK_AVAIL		4
#define PDQ_STATE_K_LINK_UNAVAIL	5
#define PDQ_STATE_K_HALTED		6
#define PDQ_STATE_K_RING_MEMBER		7
#define PDQ_STATE_K_NUMBER		8

/*+
 * Define Halt Id's 
-*/
#define PDQ_HALT_ID_K_SELFTEST_TIMEOUT	0
#define PDQ_HALT_ID_K_PARITY_ERROR	1
#define PDQ_HALT_ID_K_HOST_DIR_HALT	2
#define PDQ_HALT_ID_K_SW_FAULT		3
#define PDQ_HALT_ID_K_HW_FAULT		4
#define PDQ_HALT_ID_K_PC_TRACE		5
#define PDQ_HALT_ID_K_DMA_ERROR		6	/* Host Data has error reg */

/*+
 * Host_Int_Enb_X [lower bits defined in Host Int Type 0 register] 
-*/
#define PDQ_TYPE_X_M_XMT_DATA_ENB	    0x80000000  /* Type 2 Enables */
#define PDQ_TYPE_X_M_RCV_DATA_ENB	    0x40000000  

#define PDQ_TYPE_X_M_UNSOL_ENB		    0x20000000	/* Type 1 Enables */
#define PDQ_TYPE_X_M_HOST_SMT_ENB	    0x10000000  
#define PDQ_TYPE_X_M_CMD_RSP_ENB	    0x08000000
#define PDQ_TYPE_X_M_CMD_REQ_ENB	    0x04000000
#define	PDQ_TYPE_X_M_RESERVED_ENB	    0x00FF0000

#define PDQ_TYPE_ALL_INT_DISABLE	    0x00000000
#define PDQ_TYPE_ALL_INT_ENB		    0xFFFFFFFF

#define	PDQ$K_TYPE1_CMDREQ	1
#define	PDQ$K_TYPE1_CMDRSP	2
#define	PDQ$K_TYPE1_UNSOL	3
#define	PDQ$K_TYPE1_SMT		4
#define	PDQ$K_TYPE2_XMT		5
#define	PDQ$K_TYPE2_RCV		6

/*+
 * PDQ_host_int_enb_X register
-*/
typedef struct PDQ_host_intena_reg {
  union {
    PORT_ULONG intreg;
    struct {
      unsigned host_bus_parity_err : 1;
      unsigned pm_parity_err : 1;
      unsigned nxm : 1;
      unsigned xmt_data_flush : 1;
      unsigned state_change : 1;
      unsigned csr_cmd_done : 1;
      unsigned twtyms : 1;
      unsigned resrv0 : 9;
      unsigned resrv1 : 10;
      unsigned cmd_req : 1;
      unsigned cmd_rsp : 1;
      unsigned host_smt : 1;
      unsigned unsol : 1;
      unsigned rcv_data : 1;
      unsigned xmt_data : 1;
    } intena;
  } hintenax;
} PDQ_HOST_INT_ENAX;

/*+
 * Host Interrupt Type 0 
-*/
#define	PDQ_HOST_INT_0_M_RESERVED	    0x0000FFC0  
#define PDQ_HOST_INT_0_M_20MS	            0X00000040
#define PDQ_HOST_INT_0_M_CSR_CMD_DONE	    0x00000020
#define PDQ_HOST_INT_0_M_STATE_CHANGE	    0x00000010
#define PDQ_HOST_INT_0_M_XMT_DATA_FLUSH	    0x00000008
#define PDQ_HOST_INT_0_M_NXM		    0x00000004
#define PDQ_HOST_INT_0_M_PM_PAR_ERR	    0x00000002
#define PDQ_HOST_INT_0_M_BUS_PAR_ERR         0x00000001

typedef struct PDQ_host_int_t0_reg {
  union {
    PORT_ULONG t0_reg;
    struct {
      unsigned host_bus_parity_err : 1;
      unsigned pm_parity_err : 1;
      unsigned nxm : 1;
      unsigned xmt_data_flush : 1;
      unsigned state_change : 1;
      unsigned csr_cmd_done : 1;
      unsigned twtyms : 1;
      unsigned onems : 1;
      unsigned resrv0 : 8;
      unsigned resrv1 : 16;
    } reg;
  } t0_int_reg;
} PDQ_HOST_INT_T0_REG;

/*+
 * Type 1 Producer Register 
-*/
#define PDQ_TYPE_1_PROD_V_REARM		    31
#define PDQ_TYPE_1_PROD_V_MBZ_1		    14
#define PDQ_TYPE_1_PROD_V_COMP	            8
#define PDQ_TYPE_1_PROD_V_MBZ_2	 	    6
#define PDQ_TYPE_1_PROD_V_PROD		    0

#define PDQ_TYPE_1_PROD_M_REARM		    0X80000000	/* 0x7FFFFFFF */
#define PDQ_TYPE_1_PROD_M_MBZ_1		    0X7FFFC000	/* 0x80003FFF */
#define PDQ_TYPE_1_PROD_M_COMP    	    0X00003F00  /* 0xFFFFC0FF */
#define PDQ_TYPE_1_PROD_M_MBZ_2		    0X000000C0	/* 0xFFFFFF3F */
#define PDQ_TYPE_1_PROD_M_PROD		    0X0000003F	/* 0xFFFFFFC0 */

typedef struct PDQ_t1_prod_reg {
  union {
    PORT_ULONG t1_reg;
    struct {
      unsigned host_prod : 6;
      unsigned mbz0 : 2;
      unsigned completion : 6;
      unsigned mbz1 : 17;
      unsigned rearm : 1;
    } reg;
  } prod_reg;
} PDQ_T1_PROD_REG;

/*+
 * Type 2 Producer Register 
-*/
#define PDQ_TYPE_2_PROD_V_XMT_DATA_COMP	    24
#define PDQ_TYPE_2_PROD_V_RCV_DATA_COMP	    16
#define PDQ_TYPE_2_PROD_V_XMT_DATA_PROD	    8
#define PDQ_TYPE_2_PROD_V_RCV_DATA_PROD	    0

#define PDQ_TYPE_2_PROD_M_XMT_DATA_COMP	    0XFF000000	/* 0x00FFFFFF */
#define PDQ_TYPE_2_PROD_M_RCV_DATA_COMP	    0X00FF0000	/* 0xFF00FFFF */
#define PDQ_TYPE_2_PROD_M_XMT_DATA_PROD	    0X0000FF00	/* 0xFFFF00FF */
#define PDQ_TYPE_2_PROD_M_RCV_DATA_PROD	    0X000000FF	/* 0xFFFFFF00 */

typedef struct PDQ_t2_prod_reg {
  union {
    PORT_ULONG t2_reg;
    struct {
      unsigned rcv_data_prod_index : 8;
      unsigned xmt_data_prod_index : 8;
      unsigned rcv_data_comp_index : 8;
      unsigned xmt_data_comp_index : 8;
    } reg;
  } prod_reg;
} PDQ_T2_PROD_REG;

#define	 	FW_INTERRUPT_ADDRESS	0xA00
#define		FW_INTERRUPT_DATA	0xA04
#define		FW_BASE			0x0	/* ??? */
#define		FW_NODE_BASE		0x0	/* ??? */

/*+
 * Additional port definitions
-*/
#define FW_RCV_DES_K_BUFF_HI_SHIFT  32
#define FW_RCV_DESCR_K_128          128
#define FW_M_COMP_BLOCK_PORT_DATA_B 0xffff
#define FW_K_COMP_BLK_SHIFT_DATA_B  32
#define FW_M_DESCR_BLK_PORT_DATA_B  0xffff0000
#define FW_K_DESCR_BLK_SHIFT_DATA_B 32

/*+
 * Shared host memory modulus and remainder
-*/
#define FW_K_DESC_BLK_MOD 4096
#define FW_K_DESC_BLK_REM 0
#define FW_K_COMP_BLK_MOD 64
#define FW_K_COMP_BLK_REM 0

/*+
 * Descriptor constants
-*/
#define FW_K_XMT_DATA_CNT       256
#define FW_K_XMT_DATA_MAX_BYTES 8192
#define FW_K_XMT_DATA_ALIGN     0
#define FW_K_XMT_DATA_CHAIN     2
#define FW_K_RCV_DATA_CNT       256
#define FW_K_RCV_DATA_MAX_BYTES 8192
#define FW_K_RCV_DATA_ALIGN     512
#define FW_K_RCV_DATA_CHAIN     2
#define FW_K_CMD_REQ_CNT        16
#define FW_K_CMD_REQ_MAX_BYTES  512
#define FW_K_CMD_REQ_ALIGN      0 
#define FW_K_CMD_REQ_CHAIN      1
#define FW_K_CMD_RSP_CNT        16
#define FW_K_CMD_RSP_MAX_BYTES  512
#define FW_K_CMD_RSP_ALIGN      512
#define FW_K_CMD_RSP_CHAIN      1
#define FW_K_UNSOL_CNT          16
#define FW_K_UNSOL_MAX_BYTES    512
#define FW_K_UNSOL_ALIGN        512
#define FW_K_UNSOL_CHAIN        1
#define FW_K_SMT_CNT            64
#define FW_K_SMT_MAX_BYTES      8192
#define FW_K_SMT_ALIGN          512
#define FW_K_SMT_CHAIN          2

/*+
 * Min and max packet size for an FDDI
-*/
#define	FW$K_MSG_PREFIX_SIZE	3
#define FW_K_MIN_PKT_SIZE 	0	/*Minimum packet size*/
#define FW_K_MAX_PKT_SIZE 	8192	/*Maximum packet size*/
#define EZ$K_MAX_PKT_SIZE 	1518	/*Maximum packet size*/
#define EZ$K_MSG_BUF_SIZE 	1518
#define EZ$K_XMT_MAX_SIZE	1518

#define EZ$K_MODE_NORMAL	0x50010000
#define EZ$K_XMT_INT_MSG	1
#define EZ$K_XMT_MSG_POST	1

/*+
 * Format Receive Status
-*/
#define FDDI_FMC_DESCR_M_FSC		 0x38000000	/* <29:27> */
#define FDDI_FMC_DESCR_M_FSB_ERROR	 0x04000000	/* <26:26> */
#define FDDI_FMC_DESCR_M_FSB_ADDR_RECOG	 0x02000000	/* <25:25> */
#define FDDI_FMC_DESCR_M_FSB_ADDR_COPIED 0x01000000	/* <24:24> */
#define FDDI_FMC_DESCR_M_FSB		 0x07C00000	/* <26:22> */
#define FDDI_FMC_DESCR_M_RCC_FLUSH	 0x00200000	/* <21:21> */
#define FDDI_FMC_DESCR_M_RCC_CRC	 0x00100000	/* <20:20> */
#define FDDI_FMC_DESCR_M_RCC_RRR	 0x000E0000	/* <19:17> */
#define FDDI_FMC_DESCR_M_RCC_DD		 0x00018000	/* <16:15> */
#define FDDI_FMC_DESCR_M_RCC_SS		 0x00006000	/* <13:14> */
#define FDDI_FMC_DESCR_M_RCC		 0x003FE000	/* <21:13> */
#define FDDI_FMC_DESCR_M_LEN		 0x00001FFF	/* <12:0> */

/*+
 * End of additional port definitions
-*/

/*+
 * Register mask
-*/
#define FW_M_CSR_MASK -1

/*+
 *
-*/
#define FW_K_SOB 0X80000000
#define FW_K_EOB 0X40000000

/*+
 * How we'll fill in some queues
-*/
#define FW_K_CMD_RSP_FILL 4
#define FW_K_UNSOL_FILL   4
#define FW_K_SMT_FILL     4

/*+
 * Environment variables
-*/
#define FW_K_XMT_MAX_SIZE FW_K_MAX_PKT_SIZE
#define FW_K_XMT_INT_MSG   1
#define FW_K_XMT_MSG_POST  1
#define FW_K_XMT_FULL_POST 1
#define FW_K_RCV_BUF_NO    16
#define FW_K_MSG_BUF_SIZE  DYN$K_NILOOKASIDE
#define FW_K_MSG_MOD       0
#define FW_K_MSG_REM       0
#define FW_K_MODE_NORMAL   0

/*+
 * Driver flags
-*/
#define FW_K_DRIVER_FLAGS_DEF 0		/*Driver flags default*/
#define	FW_M_BROADCAST        1		/*Broadcast*/
#define	FW_M_ENA_BROADCAST    1		/*Enable broadcast*/
#define	FW_M_DIS_BROADCAST    0		/*Disable broadcast*/

/*+
 * Queue definitions
-*/
#define FW_MAX_QUEUE_NAME 80		/*Maximum size of a queue name*/

/*+
 * General queue descriptor
-*/
volatile struct FW_GEN_DESCR {
    PORT_ULONG	long_0;
    PORT_ULONG	long_1;
    };

/*+
 * Process queue information
-*/
volatile struct FW_QUEUE_INFO {
    struct SEMAPHORE    is;		/*Pointer to the isr semaphore*/
    struct SEMAPHORE    fs;		/*Pointer to the full semaphore*/
    struct FW_GEN_DESCR *dp;		/*Descriptor pointer*/	 
    int                 qs;		/*Size of the queue*/
    unsigned char *ci, *hc;		/*Completion index, host consumer index*/	
    unsigned char *pi;			/*Producer index*/	
    struct INODE  *ip;			/*Pointer to the port device INODE*/
    void (*rp)(struct FW_GEN_DESCR *dp);/*Pointer to a routine to be executed */
    };

/*+
 * FW csrs
-*/
volatile struct FW_CSRS {
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
 * State constants
-*/
#define FW$K_UNINITIALIZED 0
#define FW$K_STOPPED       1
#define FW$K_STARTED       2
#define FW$K_RESTARTED     3

/*+
 * General program constants
-*/
#define SIZE$K_LW	       4	/* four bytes to a longword */
#define SIZE$K_BYTE	       1	/* 1 bytes to 1 byte */
#define FW$K_OPEN_TIMEOUT_SECS 1	/*1 seconds			    */

/*+
 * Callback message received
-*/
#define FW_MAX_RCV_MSGS      10
#define FW_CALLBACK_TIMEOUT 100		/*100 * 1/100 of a second*/
#define FW_CALLBACK_WAIT     10		/*10 milliseconds = 1/100 second*/

/* System constants */

#define PDQ_K_VECTOR    0x850

/* Default Memory Mapped, Buffer Constants */

#define PDQ_K_PCI_SIZE 0x10000		/* Size of Adapter Memory */

/* Offsets into PDQ Memory Mapped Buffer */

#define PDQ_K_RcvDesc		0x0000 /* */
#define PDQ_K_XmtDesc		0x0000 /* */
#define PDQ_K_SmtDesc		0x0000 /* */
#define PDQ_K_UnsolDesc		0x0000 /* */
#define PDQ_K_CmdRes		0x0000 /* */
#define PDQ_K_CmdReq		0x0000 /* */
#define PDQ_K_XmtRcvData	0x0000 /* */
#define PDQ_K_SmtHost		0x0000 /* */
#define PDQ_K_Unsol		0x0000 /* */
#define PDQ_K_CmdRsp		0x0000 /* */
#define PDQ_K_CmdReq		0x0000 /* */
#define PDQ_K_XmtBuf		0x0000 /* */
#define PDQ_K_RcvBuf		0x0000 /* */

/* Additional port definitions */

#define PDQ_RCV_DES_K_BUFF_HI_SHIFT  32
#define PDQ_RCV_DESCR_K_128          128
#define PDQ_M_COMP_BLOCK_PORT_DATA_B 0xffff
#define PDQ_K_COMP_BLK_SHIFT_DATA_B  32
#define PDQ_M_DESCR_BLK_PORT_DATA_B  0xffff0000
#define PDQ_K_DESCR_BLK_SHIFT_DATA_B 32

/* Shared host memory modulus and remainder */

#define PDQ_K_DESC_BLK_MOD 4096
#define PDQ_K_DESC_BLK_REM 0
#define PDQ_K_COMP_BLK_MOD 64
#define PDQ_K_COMP_BLK_REM 0

/* Descriptor constants */

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

/* Min and max packet size for an FDDI */

#define PDQ_K_MIN_PKT_SIZE 0		/*Minimum packet size*/
#define PDQ_K_MAX_PKT_SIZE 8192		/*Maximum packet size*/

/* Register mask */

#define PDQ_M_CSR_MASK -1

/* */

#define PDQ_K_SOB 0X80000000
#define PDQ_K_EOB 0X40000000

/* How we'll fill in some queues */

#define PDQ_K_CMD_RSP_FILL 4
#define PDQ_K_UNSOL_FILL   4
#define PDQ_K_SMT_FILL     4

/* Environment variables */

#define PDQ_K_XMT_MAX_SIZE PDQ_K_MAX_PKT_SIZE
#define PDQ_K_XMT_INT_MSG   1
#define PDQ_K_XMT_MSG_POST  1
#define PDQ_K_XMT_FULL_POST 1
#define PDQ_K_RCV_BUF_NO    16
#define PDQ_K_MSG_BUF_SIZE  DYN$K_NILOOKASIDE
#define PDQ_K_MSG_MOD       0
#define PDQ_K_MSG_REM       0
#define PDQ_K_MODE_NORMAL   0

/* Driver flags */

#define PDQ_K_DRIVER_FLAGS_DEF 0		/*Driver flags default*/
#define	PDQ_M_BROADCAST        1		/*Broadcast*/
#define	PDQ_M_ENA_BROADCAST    1		/*Enable broadcast*/
#define	PDQ_M_DIS_BROADCAST    0		/*Disable broadcast*/

/* Queue definitions */

#define PDQ_MAX_QUEUE_NAME 80		/*Maximum size of a queue name*/

/* General queue descriptor */

volatile struct PDQ_GEN_DESCR {
    PORT_ULONG	long_0;
    PORT_ULONG	long_1;
    };

/* Process queue information */

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

/* State constants */

#define PDQ$K_UNINITIALIZED 0
#define PDQ$K_STOPPED       1
#define PDQ$K_STARTED       2
#define PDQ$K_RESTARTED     3

/* General program constants */

#define SIZE$K_LW	       4	/* four bytes to a longword */
#define SIZE$K_BYTE	       1	/* 1 bytes to 1 byte */
#define PDQ$K_OPEN_TIMEOUT_SECS 1	/*1 seconds			    */

/* Callback message received */

#define PDQ_MAX_RCV_MSGS      10
#define PDQ_CALLBACK_TIMEOUT 100		/*100 * 1/100 of a second*/
#define PDQ_CALLBACK_WAIT     10		/*10 milliseconds = 1/100 second*/

struct PDQ_MSG {
    struct PDQ_MSG *flink;
    struct PDQ_MSG *blink;
    unsigned char *msg;
    int           size;
    };

/*+
 * FDDI header 
-*/
typedef struct fddi_header {
   unsigned char   prh[3];		/* Byte alignment */
} FDDI_HEADER;

#define defea_k_reg_base 0x0000

/* MISC Register Definitions */

#define         PDQ_BurstHoldoff	0x040
#define         PDQ_SlotIO		0xC80

/* port block structure */

volatile struct PDQ_PB {
    struct pb 	        pb;
    struct LOCK 	spl_port;	/* Must be aligned on a quadword */
    struct FW_CSRS	*rp;            /* Pointer to PDQ registers */
    void 		(*lrp)();	/* Pointer to the datalink rcv */
    void 		(*lwp)();	/* Pointer to send routine */
    int		*saved_first_segment;	/* Saved first segment */
    struct PORT_CALLBACK *port_cb;	/* Port Callback pointer */

    struct PBQ 	        *pbq;
    struct INODE	*ip;		/* pointer to INODE */

    struct SEMAPHORE 	defpa_port_s;	/* port syncronize */
    struct SEMAPHORE 	defpa_isr_s;	/* interrupts */
    struct SEMAPHORE 	defpa_done_s;	/* completion */
    struct SEMAPHORE 	defpa_ready_s;	/* ready */

    DEFEA_EISA_MEMORY   *emp;		/* Mapped Mem Pointer */
    unsigned int	emp_size;	/* size of Mapped memory */

    PDQ_PRODUCER_INDEX   *pip;		/* Producer Index pointer */

    int			fcount;		/* count of file opens */

    unsigned int	lwbuf;
/*
 * The following save the calculated offsets from the beginning
 * of Mapped Memory.  This base adrs is saved in pb->emp
 */
    unsigned int	rcv_offset,	/* Receive Data Block offset */
    		    	xmt_offset,	/* Transmit Data Block offset */
    		    	CmdRsp_offset,	/* Command Response Block offset */
    		    	CmdReq_offset,	/* Command Request Block offset */
    			unsol_offset,	/* Unsolicited Block offset */
    		    	dbp_offset,	/* Descriptor Block offset */
    		    	cbp_offset;	/* Consumer Block offset */
   
    int 		rcv_msg_cnt;	/* Number of recieved messages */
    struct PDQ_MSG 	rqh;		/* Recieved msg queue header */

    int			XmtDatBlock,	/* Xmt Data buffer pointer */
    			CmdRspBlock,	/* Command Response buffer pointer */
    			CmdReqBlock;	/* Command Request  buffer pointer */

    int 		password_init,
    			setup_init,	/* Remote boot stuff*/
    			filter_type,
    			state;		/* Driver state*/

    char       		name[32];	/* Port Name */
    char 		short_name[32];	/* Port Name (Short version) */
    unsigned char	sa[6];		/* Our station address */
    char       		loopback;	/* fr_rx looback indicator */
    char		rsvd1;
    struct SEMAPHORE 	fw_out;		/*fr_out syncronize*/
    struct SEMAPHORE 	fw_stop_rx;	/*setmode(DDB$STOP), exit fr_rx()*/
    struct SEMAPHORE 	fw_isr_tx,	/*Semaphores for EZ interrupts*/
    			fw_isr_rx; 

};
