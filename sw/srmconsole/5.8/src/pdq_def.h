/* File:	pdq_def.h
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
 * Abstract:	PDQ FDDI Adapter Port Definitions
 *
 * Author:	
 *	D.W. Neale 		Taken from DEFAA FDDI Port Driver
 *
 * Modifications:
 *	dwn	24-Feb-1994	Initial entry.
 */

/* Define types 
*/
typedef unsigned char SA[6];
typedef unsigned int  PDQ_ULONG;

/* The TURBO_DMA_MASK is used to mask off the top five bits of the addresses
/* stuffed into the PDQ DMA descriptors. On Turbochannel platforms, the DMA
/* address for a given physical address is calculated by masking off the top
/* five bits of the address. It's weird, but it works.
*/
#define TURBO_DMA_MASK 0x07FFFFFF
  

/* Define maximum and minimum legal sizes for frame types 
*/
#define PDQ_LLC_K_LENGTH_MIN	20
#define PDQ_LLC_K_LENGTH_MAX	4495
#define PDQ_SMT_K_LENGTH_MIN	33
#define PDQ_SMT_K_LENGTH_MAX	4494
#define PDQ_MAC_K_LENGTH_MIN	18
#define PDQ_MAC_K_LENGTH_MAX	4494

/* Define FC's 
*/
#define PDQ_FC_K_VOID			0X00	
#define PDQ_FC_K_NON_RESTRICTED_TOKEN	0X80	
#define PDQ_FC_K_RESTRICTED_TOKEN	0XC0	
#define PDQ_FC_K_SMT			0X41	
#define PDQ_FC_K_MAC			0XC1	
#define PDQ_FC_K_LLC			0X50	
#define PDQ_FC_K_IMPLEMENTOR		0X60
#define PDQ_FC_K_RESERVED		0X70

#define PDQ_FC_M_BASE			0XF1	/* Get unique part of FC */

#define PDQ_K_SMT_XID_APP		0X80000000
#define PDQ_K_SMT_XID_ENT		0X00000000

#define PDQ_FRAME_K_SMT_TYPE_RSP	0X01
#define PDQ_FRAME_K_SMT_TYPE_NO_RSP	0X00

/* FDDI frame header 
*/
typedef struct {		
    unsigned char   prh_0;
    unsigned char   prh_1;
    unsigned char   prh_2;
    unsigned char   fc;
    SA   	    dst_addr;
    SA              src_addr;
    } PDQ_HEADER;

/* General FDDI frame 
*/
typedef struct {
    PDQ_HEADER	    header;
    unsigned char   info[4500];
    } PDQ_FRAME_GEN;


/* FDDI Receive Frame Format general structures 
*/
typedef struct {
    unsigned int    rcv_descriptor;	/* Port Receive Status */
    unsigned char   prh_0;		/* Byte alignment */
    unsigned char   prh_1;		/* Byte alignment */
    unsigned char   prh_2;		/* Byte alignment */
    unsigned char   fc;			/* Frame Control */
    SA        	   dest_addr;		/* Destination Address */
    SA              source_addr;		/* Source      Address */
    unsigned char   dsap;		/* Destination Service Access Point */
    unsigned char   ssap;		/* Source Service Access Point */
    unsigned char   cntr;		/* Unnumbered Information */
    unsigned char   pid_0;		/* IEEE Protocol Prefix [0] */
    unsigned char   pid_1;		/* IEEE Protocol Prefix [1] */
    unsigned char   pid_2;		/* IEEE Protocol Prefix [2] */
    unsigned char   pid_3;		/* IEEE Protocol  [3] */
    unsigned char   pid_4;		/* IEEE Protocol  [4] */
    unsigned char   data[4500];		/* Receive data */
    }  PDQ_RCV_DATA_BUFFER;

/* FDDI Transmit Frame Format general structures 
*/
typedef struct {
   					/* NO Port Status */
    unsigned char   prh_0;		/* Packet Request Header 0 */
    unsigned char   prh_1;		/* Packet Request Header 1 */
    unsigned char   prh_2;		/* Packet Request Header 2 */
    unsigned char   fc;			/* Frame Control */
    SA              dest_addr;		/* Destination Address */
    SA              source_addr;		/* Source      Address */
    unsigned char   dsap;		/* Destination Service Access Point */
    unsigned char   ssap;		/* Source Service Access Point */
    unsigned char   cntr;		/* Unnumbered Information */
    unsigned char   pid_0;		/* IEEE Protocol Prefix [0] */
    unsigned char   pid_1;		/* IEEE Protocol Prefix [1] */
    unsigned char   pid_2;		/* IEEE Protocol Prefix [2] */
    unsigned char   pid_3;		/* IEEE Protocol  [3] */
    unsigned char   pid_4;		/* IEEE Protocol  [4] */
    unsigned char   data[4500];		/* Transmit data */
    } PDQ_XMT_DATA_BUFFER;


/* IEEE 802.3 Data Link Frame Format 
*/
typedef struct {
    SA            dest_addr;	/*Destination                       */
    SA            source_addr;	/*Source                            */
    unsigned char len [2];   	/*length                            */
    unsigned char dsap;		/* Destination Service Access Point */
    unsigned char ssap;		/* Source Service Access Point */
    unsigned char cntr;		/* Unnumbered Information */
    unsigned char pid_0;	/* IEEE Protocol Prefix [0] */
    unsigned char pid_1;	/* IEEE Protocol Prefix [1] */
    unsigned char pid_2;	/* IEEE Protocol Prefix [2] */
    unsigned char pid_3;	/* IEEE Protocol  [3] */
    unsigned char pid_4;	/* IEEE Protocol  [4] */
    unsigned char data[1492];	/* Transmit data */
    } IEEE_FRAME;

/* ENET Data Link Frame Format
*/
typedef struct {
    SA              dest_addr;
    SA              source_addr;
    unsigned char   protocol_type_0;
    unsigned char   protocol_type_1;
    unsigned char   data[1500];
    }  ETHERNET_FRAME;


/* 64-bit counter 
*/
typedef struct {                              
    PDQ_ULONG  ms;
    PDQ_ULONG  ls;
    } PDQ_CNTR;

/* LAN address 
*/
typedef struct {                               
    PDQ_ULONG  lwrd_0;
    PDQ_ULONG  lwrd_1;
    } PDQ_LAN_ADDR;

/* Station ID address 
*/
typedef struct {
    PDQ_ULONG  octet_7_4;
    PDQ_ULONG  octet_3_0;
    } PDQ_STATION_ID;

/* Define general constants 
*/
#define PDQ_DMA_K_TIMEOUT		    2000    /* Milliseconds */

#define PDQ_PHY_K_S			    0	    /* Index to S phy */
#define PDQ_PHY_K_A			    0	    /* Index to A phy */
#define PDQ_PHY_K_B			    1	    /* Index to B phy */
#define PDQ_PHY_K_MAX			    2	    /* Max number of phys */

/* Define FDDI Data Link Functional Specification Data Types 
*/
#define PDQ_STATION_TYPE_K_SAS		    0       /* Station Type */
#define PDQ_STATION_TYPE_K_DAC		    1
#define PDQ_STATION_TYPE_K_SAC		    2
#define PDQ_STATION_TYPE_K_NAC		    3
#define PDQ_STATION_TYPE_K_DAS		    4

#define PDQ_STATION_STATE_K_OFF		    0       /* Station State */
#define PDQ_STATION_STATE_K_ON		    1
#define PDQ_STATION_STATE_K_LOOPBACK	    2

#define PDQ_LINK_STATE_K_OFF 		    1       /* Link State */
#define PDQ_LINK_STATE_K_OFF_FAULT           2
#define PDQ_LINK_STATE_K_ON_RING_INIT        3
#define PDQ_LINK_STATE_K_ON_RING_RUN         4
#define PDQ_LINK_STATE_K_BROKEN              5

#define PDQ_DA_TEST_STATE_K_UNKNOWN          0       /* Dupl Addr Test State */
#define PDQ_DA_TEST_STATE_K_SUCCESS          1
#define PDQ_DA_TEST_STATE_K_DUPLICATE        2

#define PDQ_RP_STATE_K_OFF                   0       /* Ring Purger State */
#define PDQ_RP_STATE_K_CANDIDATE             1
#define PDQ_RP_STATE_K_NON_PURGER            2
#define PDQ_RP_STATE_K_PURGER                3

#define PDQ_FS_MODE_K_SA_MATCH               0       /* Frame Strip Mode */
#define PDQ_FS_MODE_K_FCI_STRIP              1

#define PDQ_RING_ERR_RSN_K_NO_ERROR          0       /* Ring Error Reason */
#define PDQ_RING_ERR_RSN_K_INIT_INIT	    5
#define PDQ_RING_ERR_RSN_K_INIT_RCVD	    6
#define PDQ_RING_ERR_RSN_K_BEACON_INIT	    7
#define PDQ_RING_ERR_RSN_K_DUP_ADDR	    8
#define PDQ_RING_ERR_RSN_K_DUP_TOKEN	    9
#define PDQ_RING_ERR_RSN_K_PURGE_ERROR	    10
#define PDQ_RING_ERR_RSN_K_FCI_ERROR	    11
#define PDQ_RING_ERR_RSN_K_RING_OP	    12
#define PDQ_RING_ERR_RSN_K_DIR_BEACON	    13
#define PDQ_RING_ERR_RSN_K_TRACE_INIT	    14
#define PDQ_RING_ERR_RSN_K_TRACE_RCVD	    15

#define PDQ_STATION_MODE_K_NORMAL            0       /* Station Mode */
#define PDQ_STATION_MODE_K_LOOPBACK          1

#define PDQ_PHY_TYPE_K_A                     0       /* PHY Type */
#define PDQ_PHY_TYPE_K_B                     1
#define PDQ_PHY_TYPE_K_S                     2
#define PDQ_PHY_TYPE_K_M                     3
#define PDQ_PHY_TYPE_K_UNKNOWN               4

#define PDQ_PMD_TYPE_K_ANSI_MULTI            0       /* PMD Type */
#define PDQ_PMD_TYPE_K_ANSI_SINGLE_1         1
#define PDQ_PMD_TYPE_K_ANSI_SINGLE_2         2
#define PDQ_PMD_TYPE_K_ANSI_SONET            3
#define PDQ_PMD_TYPE_K_LOW_POWER             100
#define PDQ_PMD_TYPE_K_THIN_WIRE             101
#define PDQ_PMD_TYPE_K_SHIELD_TWISTED        102
#define PDQ_PMD_TYPE_K_UNSHIELD_TWISTED      103

#define PDQ_PHY_STATE_K_INT_LOOPBACK         0       /* PHY State */
#define PDQ_PHY_STATE_K_BROKEN               1
#define PDQ_PHY_STATE_K_OFF_READY            2
#define PDQ_PHY_STATE_K_WAITING              3
#define PDQ_PHY_STATE_K_STARTING             4
#define PDQ_PHY_STATE_K_FAILED               5
#define PDQ_PHY_STATE_K_WATCH                6
#define PDQ_PHY_STATE_K_IN_USE               7

#define PDQ_REJECT_RSN_K_NONE                0       /* Reject Reason */
#define PDQ_REJECT_RSN_K_LOCAL_LCT           1
#define PDQ_REJECT_RSN_K_REMOTE_LCT          2
#define PDQ_REJECT_RSN_K_LCT_BOTH            3
#define PDQ_REJECT_RSN_K_LEM_REJECT          4
#define PDQ_REJECT_RSN_K_TOPOLOGY_ERROR      5
#define PDQ_REJECT_RSN_K_NOISE_REJECT        6
#define PDQ_REJECT_RSN_K_REMOTE_REJECT       7
#define PDQ_REJECT_RSN_K_TRACE_IN            8
#define PDQ_REJECT_RSN_K_TRACE_RCVD_DIS      9
#define PDQ_REJECT_RSN_K_STANDBY             10
#define PDQ_REJECT_RSN_K_PROTO_ERROR         11

#define PDQ_RI_RSN_K_TVX_EXPIRED             0       /* RI Reason */
#define PDQ_RI_RSN_K_TRT_EXPIRED             1
#define PDQ_RI_RSN_K_RING_PURGER             2
#define PDQ_RI_RSN_K_PURGE_ERROR             3
#define PDQ_RI_RSN_K_TOKEN_TIMEOUT           4

#define PDQ_LCT_DIRECTION_K_LOCAL            1       /* LCT Direction */
#define PDQ_LCT_DIRECTION_K_REMOTE           2
#define PDQ_LCT_DIRECTION_K_BOTH             3

/* Define FMC descriptor fields 
*/
#define PDQ_FMC_DESCR_V_SOP		    31
#define PDQ_FMC_DESCR_V_EOP		    30
#define PDQ_FMC_DESCR_V_FSC		    27
#define PDQ_FMC_DESCR_V_FSB_ERROR	    26
#define PDQ_FMC_DESCR_V_FSB_ADDR_RECOG	    25
#define PDQ_FMC_DESCR_V_FSB_ADDR_COPIED	    24
#define PDQ_FMC_DESCR_V_FSB		    22
#define PDQ_FMC_DESCR_V_RCC_FLUSH	    21
#define PDQ_FMC_DESCR_V_RCC_CRC		    20
#define PDQ_FMC_DESCR_V_RCC_RRR		    17
#define PDQ_FMC_DESCR_V_RCC_DD               15
#define PDQ_FMC_DESCR_V_RCC_SS               13
#define PDQ_FMC_DESCR_V_RCC		    13
#define PDQ_FMC_DESCR_V_LEN		    0

#define PDQ_FMC_DESCR_M_SOP		    0X80000000	
#define PDQ_FMC_DESCR_M_EOP		    0X40000000	

    /* Format Receive Status
    */
#define PDQ_FMC_DESCR_M_FSC		    0X38000000	/* <29:27> */
#define PDQ_FMC_DESCR_M_FSB_ERROR	    0X04000000	/* <26:26> */
#define PDQ_FMC_DESCR_M_FSB_ADDR_RECOG	    0X02000000	/* <25:25> */
#define PDQ_FMC_DESCR_M_FSB_ADDR_COPIED	    0X01000000	/* <24:24> */
#define PDQ_FMC_DESCR_M_FSB		    0X07C00000	/* <26:22> */
#define PDQ_FMC_DESCR_M_RCC_FLUSH	    0X00200000	/* <21:21> */
#define PDQ_FMC_DESCR_M_RCC_CRC		    0X00100000	/* <20:20> */
#define PDQ_FMC_DESCR_M_RCC_RRR		    0X000E0000	/* <19:17> */
#define PDQ_FMC_DESCR_M_RCC_DD               0X00018000	/* <16:15> */
#define PDQ_FMC_DESCR_M_RCC_SS               0X00006000	/* <13:14> */
#define PDQ_FMC_DESCR_M_RCC		    0X003FE000	/* <21:13> */
#define PDQ_FMC_DESCR_M_LEN		    0X00001FFF	/* <12:0> */

#define PDQ_FMC_DESCR_K_RCC_FMC_INT_ERR	    0X01AA

#define PDQ_FMC_DESCR_K_RRR_SUCCESS	    0X00
#define PDQ_FMC_DESCR_K_RRR_SA_MATCH	    0X01
#define PDQ_FMC_DESCR_K_RRR_DA_MATCH	    0X02
#define PDQ_FMC_DESCR_K_RRR_FMC_ABORT	    0X03
#define PDQ_FMC_DESCR_K_RRR_LENGTH_BAD	    0X04
#define PDQ_FMC_DESCR_K_RRR_FRAGMENT	    0X05
#define PDQ_FMC_DESCR_K_RRR_FORMAT_ERR	    0X06
#define PDQ_FMC_DESCR_K_RRR_MAC_RESET	    0X07

#define PDQ_FMC_DESCR_K_DD_NO_MATCH          0X0
#define PDQ_FMC_DESCR_K_DD_PROMISCUOUS       0X1
#define PDQ_FMC_DESCR_K_DD_CAM_MATCH         0X2
#define PDQ_FMC_DESCR_K_DD_LOCAL_MATCH       0X3

#define PDQ_FMC_DESCR_K_SS_NO_MATCH          0X0
#define PDQ_FMC_DESCR_K_SS_BRIDGE_MATCH      0X1
#define PDQ_FMC_DESCR_K_SS_NOT_POSSIBLE      0X2
#define PDQ_FMC_DESCR_K_SS_LOCAL_MATCH       0X3

/* Define some max buffer sizes 
*/
#define PDQ_CMD_REQ_K_SIZE_MAX		    512
#define PDQ_CMD_RSP_K_SIZE_MAX		    512
#define PDQ_UNSOL_K_SIZE_MAX		    512
#define PDQ_SMT_HOST_K_SIZE_MAX		    4608		/* 4 1/2 K */
#define PDQ_RCV_DATA_K_SIZE_MAX		    4608		/* 4 1/2 K */
#define PDQ_XMT_DATA_K_SIZE_MAX		    4608		/* 4 1/2 K */


/* Define general FDDI frame formats 
*/

typedef struct {
    PDQ_FRAME_GEN  frame;			    /* Regular FDDI frame */
    } PDQ_RCV_FRAME;

/* Define PDQ Adapter States 
*/
#define PDQ_STATE_K_RESET	    0
#define PDQ_STATE_K_UPGRADE          1
#define PDQ_STATE_K_DMA_UNAVAIL	    2
#define PDQ_STATE_K_DMA_AVAIL	    3
#define PDQ_STATE_K_LINK_AVAIL	    4
#define PDQ_STATE_K_LINK_UNAVAIL     5
#define PDQ_STATE_K_HALTED           6
#define PDQ_STATE_K_RING_MEMBER      7
#define PDQ_STATE_K_NUMBER	    8

/* Define PDQ codes for command type 
*/
#define PDQ_CMD_K_START			    0x00
#define PDQ_CMD_K_FILTERS_SET		    0x01
#define PDQ_CMD_K_FILTERS_GET	            0x02
#define PDQ_CMD_K_CHARS_SET	            0x03
#define PDQ_CMD_K_STATUS_CHARS_GET	    0x04
#define PDQ_CMD_K_CNTRS_GET		    0x05
#define PDQ_CMD_K_CNTRS_SET		    0x06
#define PDQ_CMD_K_ADDR_FILTER_SET	    0x07
#define PDQ_CMD_K_ADDR_FILTER_GET	    0x08
#define PDQ_CMD_K_ERROR_LOG_CLEAR	    0x09
#define PDQ_CMD_K_ERROR_LOG_GET		    0x0A
#define PDQ_CMD_K_PDQ_MIB_GET               0x0B
#define PDQ_CMD_K_DEC_EXT_MIB_GET            0x0C
#define PDQ_CMD_K_DEVICE_SPECIFIC_GET        0x0D
#define PDQ_CMD_K_STATUS_CHARS_TEST	    0x0E
#define PDQ_CMD_K_UNSOL_TEST                 0x0F
#define PDQ_CMD_K_MAX			    0x0F	/* Must match prev */

/* Define PDQ item codes for Chars_Set and Filters_Set commands 
*/
#define PDQ_ITEM_K_EOL                       0x00
#define PDQ_ITEM_K_T_REQ                     0x01      /* MACTReq */
#define PDQ_ITEM_K_TVX                       0x02      /* MACTvxValue */
#define PDQ_ITEM_K_RESTRICTED_TOKEN          0x03      /* eMACRestrictedTokenTimeout */
#define PDQ_ITEM_K_LEM_THRESHOLD             0x04
#define PDQ_ITEM_K_RING_PURGER               0x05      /* eMACRingPurgerEnable */
#define PDQ_ITEM_K_CNTR_INTERVAL             0x06
#define PDQ_ITEM_K_IND_GROUP_PROM            0x07
#define PDQ_ITEM_K_GROUP_PROM                0x08
#define PDQ_ITEM_K_BROADCAST                 0x09
#define PDQ_ITEM_K_SMT_PROM                  0x0A
#define PDQ_ITEM_K_SMT_USER                  0x0B
#define PDQ_ITEM_K_RESERVED                  0x0C
#define PDQ_ITEM_K_IMPLEMENTOR               0x0D
#define PDQ_ITEM_K_LOOPBACK_MODE             0x0E
#define PDQ_ITEM_K_SMT_VERSION_ID            0x0F      /* SMTOpVersionId */
#define PDQ_ITEM_K_CONFIG_POLICY             0x10      /* SMTConfigPolicy */
#define PDQ_ITEM_K_CON_POLICY                0x11      /* SMTConnectionPolicy */
#define PDQ_ITEM_K_T_NOTIFY                  0x12      /* SMTTNotify */
#define PDQ_ITEM_K_STATION_ACTION            0x13      /* SMTStationAction */
#define PDQ_ITEM_K_T_MAX_GREATEST_LB         0x14      /* MACTMaxGreatestLowerBound */
#define PDQ_ITEM_K_MAC_PATHS_REQ       	    0x15      /* MACPathsRequested */
#define PDQ_ITEM_K_FRAME_STATUS              0x16      /* MACCurrentFrameStatus */
#define PDQ_ITEM_K_MAC_ACTION                0x17      /* MACAction */
#define PDQ_ITEM_K_CON_POLICIES              0x18      /* PORTConnectionPolicies */
#define PDQ_ITEM_K_PORT_PATHS_REQ            0x19      /* PORTPathsRequested */
#define PDQ_ITEM_K_MAC_LOOP_TIME             0x1A      /* PORTMACLoopTime */
#define PDQ_ITEM_K_TB_MAX                    0x1B      /* PORTTBMax */
#define PDQ_ITEM_K_LER_CUTOFF                0x1C      /* PORTLerCutoff */
#define PDQ_ITEM_K_LER_ALARM                 0x1D      /* PORTLerAlarm */
#define PDQ_ITEM_K_PORT_ACTION               0x1E      /* PORTAction */
#define PDQ_ITEM_K_INSERT_POLICY             0x1F      /* ATTACHMENTInsertPolicy */
#define PDQ_ITEM_K_FLUSH_TIME                0X20 
#define PDQ_ITEM_K_MAX			    0x20      /* Must equal high item */

/* Values for some of the items 
*/
#define PDQ_K_OFF			    0       /* Generic OFF|ON consts */
#define PDQ_K_ON				    1

#define PDQ_FSTATE_K_BLOCK		    0       /* Filter State */
#define PDQ_FSTATE_K_PASS		    1

/* Item value limits and defaults 
*/
#define PDQ_TIME_UNIT			    80
#define PDQ_ITEM_K_T_REQ_MIN		    (4000000/PDQ_TIME_UNIT)
#define PDQ_ITEM_K_T_REQ_MAX		    (167772080/PDQ_TIME_UNIT)
#define PDQ_ITEM_K_T_REQ_DEF		    (8000000/PDQ_TIME_UNIT)
#define PDQ_ITEM_K_TVX_MIN		    (2500000/PDQ_TIME_UNIT)
#define PDQ_ITEM_K_TVX_MAX		    (5222400/PDQ_TIME_UNIT)
#define PDQ_ITEM_K_TVX_DEF		    (2621440/PDQ_TIME_UNIT)
#define PDQ_ITEM_K_RESTRICTED_TOKEN_MIN	    0
#define PDQ_ITEM_K_RESTRICTED_TOKEN_MAX	    ((1000000000/PDQ_TIME_UNIT)*10)
#define PDQ_ITEM_K_RESTRICTED_TOKEN_DEF	    (1000000000/PDQ_TIME_UNIT)
#define PDQ_ITEM_K_LEM_THRESHOLD_MIN	    5
#define PDQ_ITEM_K_LEM_THRESHOLD_MAX	    8
#define PDQ_ITEM_K_LEM_THRESHOLD_DEF	    8
#define PDQ_ITEM_K_RING_PURGER_DEF           PDQ_K_ON  /* def - ring purger ON */
#define PDQ_ITEM_K_IND_GROUP_PROM_DEF	    PDQ_FSTATE_K_BLOCK
#define PDQ_ITEM_K_GROUP_PROM_DEF	    PDQ_FSTATE_K_BLOCK
#define PDQ_ITEM_K_BROADCAST_DEF		    PDQ_FSTATE_K_BLOCK
#define PDQ_ITEM_K_SMT_PROM_DEF		    PDQ_FSTATE_K_BLOCK
#define PDQ_ITEM_K_SMT_USER_DEF	            PDQ_FSTATE_K_BLOCK
#define PDQ_ITEM_K_RESERVED_DEF		    PDQ_FSTATE_K_BLOCK
#define PDQ_ITEM_K_IMPLEMENTOR_DEF	    PDQ_FSTATE_K_BLOCK
#define PDQ_ITEM_K_LOOPBACK_NONE             0
#define PDQ_ITEM_K_LOOPBACK_INT              1
#define PDQ_ITEM_K_LOOPBACK_EXT              2
#define PDQ_ITEM_K_SMT_VERSION_ID_DEF        2        /* supported SMT Version */
#define PDQ_ITEM_K_CONFIG_POLICY_DEF         0        /* none supported (def) */
#define PDQ_ITEM_K_CON_POLICY_MIN            0        /* Reject none */
#define PDQ_ITEM_K_CON_POLICY_MAX            65535    /* Reject A-A A-B A-S... */
#define PDQ_ITEM_K_CON_POLICY_DEF            0x8021   /* Reject A-A B-B M-M */
#define PDQ_ITEM_K_T_NOTIFY_MIN              2        /* 2 second minimum */
#define PDQ_ITEM_K_T_NOTIFY_MAX              30       /* 30 second maximum */
#define PDQ_ITEM_K_T_NOTIFY_DEF              10       /* 10 second default */
#define PDQ_ITEM_K_T_MAX_GREATEST_LB_MIN     0
#define PDQ_ITEM_K_T_MAX_GREATEST_LB_MAX     (165000000/PDQ_TIME_UNIT)
#define PDQ_ITEM_K_T_MAX_GREATEST_LB_DEF     (165000000/PDQ_TIME_UNIT)
#define PDQ_ITEM_K_MAC_PATHS_REQ_DEF   	    1        /* Primary path default */
#define PDQ_ITEM_K_FRAME_STATUS_DEF          1        /* FSC-Type0 default */
#define PDQ_ITEM_K_CON_POLICIES_MIN          0        /* none */
#define PDQ_ITEM_K_CON_POLICIES_MAX          7        /* LCT, Loop & Placement */
#define PDQ_ITEM_K_CON_POLICIES_DEF          4        /* Pc-MAC-Placement def */
#define PDQ_ITEM_K_PORT_PATHS_REQ_DEF        1        /* Primary path default */
#define PDQ_ITEM_K_MAC_LOOP_TIME_MIN         (200000000/PDQ_TIME_UNIT)
#define PDQ_ITEM_K_MAC_LOOP_TIME_MAX         ((1000000000/PDQ_TIME_UNIT)*10)
#define PDQ_ITEM_K_MAC_LOOP_TIME_DEF         (200000000/PDQ_TIME_UNIT)
#define PDQ_ITEM_K_TB_MAX_MIN                (30000000/PDQ_TIME_UNIT)
#define PDQ_ITEM_K_TB_MAX_MAX                ((1000000000/PDQ_TIME_UNIT)*10)
#define PDQ_ITEM_K_TB_MAX_DEF                (30000000/PDQ_TIME_UNIT)
#define PDQ_ITEM_K_LER_CUTOPDQ_MIN            4        /* min value - SMT spec */
#define PDQ_ITEM_K_LER_CUTOPDQ_MAX            15       /* max value - SMT spec */
#define PDQ_ITEM_K_LER_CUTOPDQ_DEF            7        /* def value - SMT spec */
#define PDQ_ITEM_K_LER_ALARM_MIN             4        /* min value - SMT spec */
#define PDQ_ITEM_K_LER_ALARM_MAX             15       /* max value - SMT spec */
#define PDQ_ITEM_K_LER_ALARM_DEF             7        /* def value - SMT spec */
#define PDQ_ITEM_K_INSERT_POLICY_DEF         1        /* insert = true (def) */
#define PDQ_ITEM_K_FLUSH_TIME_MIN            0	     /* Disables flush */
#define PDQ_ITEM_K_FLUSH_TIME_MAX            255      /* Number of seconds */
#define PDQ_ITEM_K_FLUSH_TIME_DEF            3        /* Arch recommended */

/* Define PDQ command return codes 
*/
#define PDQ_RSP_K_SUCCESS		    0x00
#define PDQ_RSP_K_FAILURE		    0x01
#define PDQ_RSP_K_WARNING		    0x02
#define PDQ_RSP_K_LOOP_MODE_BAD		    0x03
#define PDQ_RSP_K_ITEM_CODE_BAD		    0x04
#define PDQ_RSP_K_TVX_BAD		    0x05
#define PDQ_RSP_K_TREQ_BAD		    0x06
#define PDQ_RSP_K_TOKEN_BAD		    0x07
#define PDQ_RSP_K_PROM_LLC_BAD		    0x08
#define PDQ_RSP_K_PROM_GROUP_BAD	    0x09
#define PDQ_RSP_K_PROM_SMT_USER_BAD	    0x0A
#define PDQ_RSP_K_SMT_PROM_BAD		    0x0B
#define PDQ_RSP_K_NO_EOL		    0x0C
#define PDQ_RSP_K_FILTER_STATE_BAD	    0x0D
#define PDQ_RSP_K_CMD_TYPE_BAD		    0x0E
#define PDQ_RSP_K_ADAPTER_STATE_BAD	    0x0F
#define PDQ_RSP_K_RING_PURGER_BAD	    0x10
#define PDQ_RSP_K_LEM_THRESHOLD_BAD	    0x11
#define PDQ_RSP_K_LOOP_NOT_SUPPORTED	    0x12
#define PDQ_RSP_K_FLUSH_TIME_BAD	    0X13


/* PDQ COMMAND STRUCTURES 
*/

/* Item list 
*/
typedef struct {
    PDQ_ULONG  item_code;
    PDQ_ULONG  value;
    } PDQ_ITEM_LIST;

/* Response header 
*/
typedef struct {
    PDQ_ULONG  rsvd;
    PDQ_ULONG  cmd_type;
    PDQ_ULONG  status;
    } PDQ_RSP_HEADER;

/* Start Command 
*/
typedef struct {
    PDQ_ULONG  cmd_type;
    } PDQ_CMD_START_REQ;

/* Start Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    } PDQ_CMD_START_RSP;

/* Filters_Set Request 
*/
#define PDQ_CMD_FILTERS_SET_K_ITEMS_MAX  8	/* Num filters + 1 */

typedef struct {
    PDQ_ULONG	    cmd_type;
    PDQ_ITEM_LIST    item[PDQ_CMD_FILTERS_SET_K_ITEMS_MAX];
    } PDQ_CMD_FILTERS_SET_REQ;

/* Filters_Set Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    } PDQ_CMD_FILTERS_SET_RSP;

/* Filters_Get Request 
*/
typedef struct {
    PDQ_ULONG        cmd_type;   
    } PDQ_CMD_FILTERS_GET_REQ;

/* Filters_Get Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    PDQ_ULONG	    ind_group_prom;
    PDQ_ULONG	    group_prom;
    PDQ_ULONG	    broadcast_all;
    PDQ_ULONG	    smt_all;
    PDQ_ULONG	    smt_user;
    PDQ_ULONG	    reserved_all;
    PDQ_ULONG	    implementor_all;
    } PDQ_CMD_FILTERS_GET_RSP;

/* Chars_Set Request 
*/
#define PDQ_CMD_CHARS_SET_K_ITEMS_MAX (PDQ_ITEM_K_MAX + 1)       /* 1 for tests */

typedef struct {
    PDQ_ULONG	    cmd_type;
    struct {                               /* Item list */
        PDQ_ULONG    item_code;
        PDQ_ULONG    value;
        PDQ_ULONG    item_index;
        } item[PDQ_CMD_CHARS_SET_K_ITEMS_MAX];    
    } PDQ_CMD_CHARS_SET_REQ;

/* Chars_Set Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    } PDQ_CMD_CHARS_SET_RSP;

/* Addr_Filter_Set Request 
*/
#define PDQ_CMD_ADDR_FILTER_K_SIZE   62

typedef struct {
    PDQ_ULONG	cmd_type;
    PDQ_LAN_ADDR	entry[PDQ_CMD_ADDR_FILTER_K_SIZE];
    } PDQ_CMD_ADDR_FILTER_SET_REQ;

/* Addr_Filter_Set Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    } PDQ_CMD_ADDR_FILTER_SET_RSP;

/* Addr_Filter_Get Request 
*/
typedef struct {
    PDQ_ULONG	cmd_type;
    } PDQ_CMD_ADDR_FILTER_GET_REQ;

/* Addr_Filter_Get Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    PDQ_LAN_ADDR     entry[PDQ_CMD_ADDR_FILTER_K_SIZE];
    } PDQ_CMD_ADDR_FILTER_GET_RSP;

/* Dev_Specific_Get Request 
*/
typedef struct {
    PDQ_ULONG  cmd_type;
    } PDQ_CMD_DEVICE_SPECIFIC_GET_REQ;

/* Dev_Specific_Get Response => defined in device specific module 
/* Status_Chars_Get Request 
*/
typedef struct {
    PDQ_ULONG  cmd_type;
    } PDQ_CMD_STATUS_CHARS_GET_REQ;

/* Status_Chars_Get Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    PDQ_STATION_ID   station_id;				/* Station */
    PDQ_ULONG	    station_type;
    PDQ_ULONG	    smt_ver_id;
    PDQ_ULONG	    smt_ver_id_max;
    PDQ_ULONG	    smt_ver_id_min;
    PDQ_ULONG	    station_state;
    PDQ_LAN_ADDR	    link_addr;				/* Link */
    PDQ_ULONG	    t_req;
    PDQ_ULONG	    tvx;
    PDQ_ULONG	    token_timeout;
    PDQ_ULONG	    purger_enb;
    PDQ_ULONG	    link_state;
    PDQ_ULONG	    tneg;
    PDQ_ULONG	    dup_addr_flag;
    PDQ_LAN_ADDR	    una;
    PDQ_LAN_ADDR	    una_old;
    PDQ_ULONG	    un_dup_addr_flag;
    PDQ_LAN_ADDR	    dna;
    PDQ_LAN_ADDR	    dna_old;
    PDQ_ULONG	    purger_state;
    PDQ_ULONG	    fci_mode;
    PDQ_ULONG	    error_reason;
    PDQ_ULONG	    loopback;
    PDQ_ULONG	    ring_latency;
    PDQ_LAN_ADDR	    last_dir_beacon_sa;
    PDQ_LAN_ADDR	    last_dir_beacon_una;
    PDQ_ULONG	    phy_type[PDQ_PHY_K_MAX];		/* Phy */
    PDQ_ULONG	    pmd_type[PDQ_PHY_K_MAX];
    PDQ_ULONG	    lem_threshold[PDQ_PHY_K_MAX];
    PDQ_ULONG	    phy_state[PDQ_PHY_K_MAX];
    PDQ_ULONG	    nbor_phy_type[PDQ_PHY_K_MAX];
    PDQ_ULONG	    link_error_est[PDQ_PHY_K_MAX];
    PDQ_ULONG	    broken_reason[PDQ_PHY_K_MAX];
    PDQ_ULONG	    reject_reason[PDQ_PHY_K_MAX];
    PDQ_ULONG	    cntr_interval;			/* Miscellaneous */
    PDQ_ULONG	    module_rev;
    PDQ_ULONG	    firmware_rev;
    PDQ_ULONG	    mop_device_type;
    PDQ_ULONG	    phy_led[PDQ_PHY_K_MAX];
    PDQ_ULONG	    flush_time;
    } PDQ_CMD_STATUS_CHARS_GET_RSP;

/* STATUS_CHARS_TEST Request 
*/
typedef struct {
    PDQ_ULONG  cmd_type;
    } PDQ_CMD_STATUS_CHARS_TEST_REQ;

/* STATUS_CHARS_TEST Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    } PDQ_CMD_STATUS_CHARS_TEST_RSP;
	
/* PDQ_MIB_Get Request 
*/
typedef struct {
    PDQ_ULONG  cmd_type;
    } PDQ_CMD_PDQ_MIB_GET_REQ;

/* PDQ_MIB_Get Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;		

    /* SMT GROUP */

    PDQ_STATION_ID   smt_station_id;			
    PDQ_ULONG	    smt_op_version_id;
    PDQ_ULONG	    smt_hi_version_id;
    PDQ_ULONG	    smt_lo_version_id;
    PDQ_ULONG	    smt_mac_ct;				
    PDQ_ULONG	    smt_non_master_ct;			
    PDQ_ULONG	    smt_master_ct;				
    PDQ_ULONG	    smt_paths_available;			
    PDQ_ULONG	    smt_config_capabilities;		
    PDQ_ULONG	    smt_config_policy;		
    PDQ_ULONG	    smt_connection_policy;		
    PDQ_ULONG	    smt_t_notify;	
    PDQ_ULONG	    smt_status_reporting;
    PDQ_ULONG	    smt_ecm_state;	
    PDQ_ULONG	    smt_cf_state;	
    PDQ_ULONG	    smt_hold_state;		
    PDQ_ULONG	    smt_remote_disconnect_flag;
    PDQ_ULONG	    smt_station_action;			

    /* MAC GROUP */

    PDQ_ULONG	    mac_frame_status_capabilities;	
    PDQ_ULONG	    mac_t_max_greatest_lower_bound;
    PDQ_ULONG	    mac_tvx_greatest_lower_bound;
    PDQ_ULONG	    mac_paths_available;
    PDQ_ULONG	    mac_current_path;
    PDQ_LAN_ADDR	    mac_upstream_nbr;			
    PDQ_LAN_ADDR	    mac_old_upstream_nbr;		
    PDQ_ULONG	    mac_dup_addr_test;			
    PDQ_ULONG	    mac_paths_requested;
    PDQ_ULONG	    mac_downstream_port_type;
    PDQ_LAN_ADDR     mac_smt_address;			
    PDQ_ULONG	    mac_t_req;				
    PDQ_ULONG	    mac_t_neg;
    PDQ_ULONG        mac_t_max;				
    PDQ_ULONG	    mac_tvx_value;			
    PDQ_ULONG	    mac_t_min;				
    PDQ_ULONG	    mac_current_frame_status;
    /*              mac_frame_cts 			*/
    /* 		    mac_error_cts 			*/
    /* 		    mac_lost_cts 			*/
    PDQ_ULONG	    mac_frame_error_threshold;		
    PDQ_ULONG	    mac_frame_error_ratio;		
    PDQ_ULONG	    mac_rmt_state;
    PDQ_ULONG	    mac_da_flag;
    PDQ_ULONG	    mac_una_da_flag;			
    PDQ_ULONG	    mac_frame_condition;
    PDQ_ULONG	    mac_chip_set;			
    PDQ_ULONG	    mac_action;				

    /* PATH GROUP => Does not need to be implemented */

    /* PORT GROUP */

    PDQ_ULONG	    port_pc_type[PDQ_PHY_K_MAX];			
    PDQ_ULONG	    port_pc_neighbor[PDQ_PHY_K_MAX];			
    PDQ_ULONG	    port_connection_policies[PDQ_PHY_K_MAX];
    PDQ_ULONG	    port_remote_mac_indicated[PDQ_PHY_K_MAX];
    PDQ_ULONG	    port_ce_state[PDQ_PHY_K_MAX];
    PDQ_ULONG	    port_paths_requested[PDQ_PHY_K_MAX];
    PDQ_ULONG	    port_mac_placement[PDQ_PHY_K_MAX];
    PDQ_ULONG	    port_available_paths[PDQ_PHY_K_MAX];
    PDQ_ULONG	    port_mac_loop_time[PDQ_PHY_K_MAX];
    PDQ_ULONG	    port_tb_max[PDQ_PHY_K_MAX];
    PDQ_ULONG	    port_bs_flag[PDQ_PHY_K_MAX];
    /*		    port_lct_fail_cts[PDQ_PHY_K_MAX];	*/
    PDQ_ULONG	    port_ler_estimate[PDQ_PHY_K_MAX];			    
    /*		    port_lem_reject_cts[PDQ_PHY_K_MAX];	*/
    /*	    	    port_lem_cts[PDQ_PHY_K_MAX];		*/
    PDQ_ULONG	    port_ler_cutoff[PDQ_PHY_K_MAX];			    
    PDQ_ULONG	    port_ler_alarm[PDQ_PHY_K_MAX];			    
    PDQ_ULONG	    port_connect_state[PDQ_PHY_K_MAX];
    PDQ_ULONG	    port_pcm_state[PDQ_PHY_K_MAX];
    PDQ_ULONG	    port_pc_withhold[PDQ_PHY_K_MAX];
    PDQ_ULONG	    port_ler_condition[PDQ_PHY_K_MAX];			    
    PDQ_ULONG	    port_chip_set[PDQ_PHY_K_MAX];			    
    PDQ_ULONG	    port_action[PDQ_PHY_K_MAX];			    

    /* ATTACHMENT GROUP */

    PDQ_ULONG	    attachment_class;
    PDQ_ULONG	    attachment_ob_present;
    PDQ_ULONG	    attachment_imax_expiration;
    PDQ_ULONG	    attachment_inserted_status;
    PDQ_ULONG	    attachment_insert_policy;

    /* CHIP SET GROUP => Does not need to be implemented */

    } PDQ_CMD_PDQ_MIB_GET_RSP;

/* PDQ_MIB_TEST Request 
*/
typedef struct {
    PDQ_ULONG  cmd_type;
    } PDQ_CMD_PDQ_MIB_TEST_REQ;

/* PDQ_MIB_TEST Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    } PDQ_CMD_PDQ_MIB_TEST_RSP;

/* DEC_Ext_MIB_Get Request 
*/
typedef struct {
    PDQ_ULONG  cmd_type;
    } PDQ_CMD_DEC_EXT_MIB_GET_REQ;

/* DEC_Ext_MIB_Get (efddi group only) Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;		

    /* SMT GROUP */

    PDQ_ULONG	    esmt_station_type;

    /* MAC GROUP */

    PDQ_ULONG	    emac_link_state;			
    PDQ_ULONG	    emac_ring_purger_state;
    PDQ_ULONG        emac_ring_purger_enable;
    PDQ_ULONG	    emac_frame_strip_mode;
    PDQ_ULONG	    emac_ring_error_reason;
    PDQ_ULONG	    emac_up_nbr_dup_addr_flag;
    PDQ_ULONG	    emac_restricted_token_timeout;

    /* PORT GROUP */

    PDQ_ULONG	    eport_pmd_type[PDQ_PHY_K_MAX];
    PDQ_ULONG	    eport_phy_state[PDQ_PHY_K_MAX];
    PDQ_ULONG	    eport_reject_reason[PDQ_PHY_K_MAX];

    } PDQ_CMD_DEC_EXT_MIB_GET_RSP;

/* DEC_MIB_TEST Request 
*/
typedef struct {
    PDQ_ULONG  cmd_type;
    } PDQ_CMD_DEC_EXT_MIB_TEST_REQ;

/* DEC_MIB_TEST Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    } PDQ_CMD_DEC_EXT_MIB_TEST_RSP;

typedef struct {
    PDQ_CNTR	    traces_rcvd;			/* Station */
    PDQ_CNTR	    frame_cnt;				/* Link */
    PDQ_CNTR	    error_cnt;
    PDQ_CNTR	    lost_cnt;
    PDQ_CNTR	    octets_rcvd;
    PDQ_CNTR	    octets_sent;
    PDQ_CNTR	    pdus_rcvd;
    PDQ_CNTR	    pdus_sent;
    PDQ_CNTR	    mcast_octets_rcvd;
    PDQ_CNTR	    mcast_octets_sent;
    PDQ_CNTR	    mcast_pdus_rcvd;
    PDQ_CNTR	    mcast_pdus_sent;
    PDQ_CNTR	    xmt_underruns;
    PDQ_CNTR	    xmt_failures;
    PDQ_CNTR         block_check_errors;
    PDQ_CNTR	    frame_status_errors;
    PDQ_CNTR         pdu_length_errors;
    PDQ_CNTR	    rcv_overruns;
    PDQ_CNTR	    user_buff_unavailable;
    PDQ_CNTR	    inits_initiated;
    PDQ_CNTR	    inits_rcvd;
    PDQ_CNTR	    beacons_initiated;
    PDQ_CNTR	    dup_addrs;
    PDQ_CNTR	    dup_tokens;
    PDQ_CNTR	    purge_errors;
    PDQ_CNTR	    fci_strip_errors;
    PDQ_CNTR	    traces_initiated;
    PDQ_CNTR	    directed_beacons_rcvd;
    PDQ_CNTR         emac_frame_alignment_errors;
    PDQ_CNTR	    ebuff_errors[PDQ_PHY_K_MAX];		/* Phy */
    PDQ_CNTR	    lct_rejects[PDQ_PHY_K_MAX];
    PDQ_CNTR	    lem_rejects[PDQ_PHY_K_MAX];
    PDQ_CNTR	    link_errors[PDQ_PHY_K_MAX];
    PDQ_CNTR	    connections[PDQ_PHY_K_MAX];
    PDQ_CNTR         kongs;				/* Private counters */
    } PDQ_CNTR_BLK;

/* Counters_Get Request 
*/
typedef struct {
    PDQ_ULONG  cmd_type;
    } PDQ_CMD_CNTRS_GET_REQ;

/* Counters_Get Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    PDQ_CNTR	    time_since_reset;			
    PDQ_CNTR_BLK	    cntrs;			    
    } PDQ_CMD_CNTRS_GET_RSP;

/* Counters_Set Request 
*/
typedef struct {
    PDQ_ULONG	cmd_type;
    PDQ_CNTR_BLK	cntrs;			    
    } PDQ_CMD_CNTRS_SET_REQ;

/* Counters_Set Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    } PDQ_CMD_CNTRS_SET_RSP;

/* CNTRS_TEST Request 
*/
typedef struct {
    PDQ_ULONG  cmd_type;
    } PDQ_CMD_CNTRS_TEST_REQ;

/* CNTRS_TEST Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    } PDQ_CMD_CNTRS_TEST_RSP;

/* Error_Log_Clear Request 
*/
typedef struct {
    PDQ_ULONG  cmd_type;
    } PDQ_CMD_ERROR_LOG_CLEAR_REQ;

/* Error_Log_Clear Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    } PDQ_CMD_ERROR_LOG_CLEAR_RSP;

/* Error_Log_Get Request 
*/
#define PDQ_LOG_ENTRY_K_INDEX_MIN    0	    /* Minimum index for entry */

typedef struct {
    PDQ_ULONG  cmd_type;
    PDQ_ULONG  entry_index;
    } PDQ_CMD_ERROR_LOG_GET_REQ;

/* Error_Log_Get Response 
*/
#define PDQ_K_LOG_FW_SIZE	111	    /* Max number of fw longwords */
#define PDQ_K_LOG_DIAG_SIZE      6	    /* Max number of diag longwords */

typedef struct {
    struct {
        PDQ_ULONG    fru_imp_mask;
        PDQ_ULONG    test_id;
        PDQ_ULONG    reserved[PDQ_K_LOG_DIAG_SIZE];
        } diag;
    PDQ_ULONG	    fw[PDQ_K_LOG_FW_SIZE]; /* Refer to port_dev.h for details */
    } PDQ_LOG_ENTRY;

typedef struct {
    PDQ_RSP_HEADER   header;    
    PDQ_ULONG	    event_status;
    PDQ_ULONG	    caller_id;
    PDQ_ULONG	    timestamp_l;
    PDQ_ULONG	    timestamp_h;
    PDQ_ULONG	    write_count;
    PDQ_LOG_ENTRY    entry_info;
    } PDQ_CMD_ERROR_LOG_GET_RSP;

/* Define error log related constants and types 
*/
#define PDQ_LOG_EVENT_STATUS_K_VALID	0            /* Valid Event Status */
#define PDQ_LOG_EVENT_STATUS_K_INVALID	1            /* Invalid Event Status */
#define PDQ_LOG_CALLER_ID_K_RESERVED     0            /* Reserved Caller Id */
#define PDQ_LOG_CALLER_ID_K_FW           1            /* Firmware Caller Id */
#define PDQ_LOG_CALLER_ID_K_DIAG         2            /* Diagnostics Caller Id */

/* TEST_UNSOL Request 
*/
typedef struct {
    PDQ_ULONG	cmd_type;
    } PDQ_CMD_UNSOL_TEST_REQ;

/* TEST_UNSOL Response 
*/
typedef struct {
    PDQ_RSP_HEADER   header;    
    } PDQ_CMD_TEST_UNSOL_RSP;

/* Define the types of unsolicited events 
*/
#define PDQ_UNSOL_TYPE_K_EVENT_REPORT	    0
#define PDQ_UNSOL_TYPE_K_EVENT_CNTRS	    1

/* Define entity codes for unsolicited event reports 
*/
#define PDQ_UNSOL_ENTITY_K_STATION	    0
#define PDQ_UNSOL_ENTITY_K_LINK		    1
#define PDQ_UNSOL_ENTITY_K_PHY		    2

/* Define station unsolicited event and argument codes 
*/
#define PDQ_UNSOL_STAT_K_SELFTEST_FAILED     0                  
#define PDQ_UNSOL_STAT_K_PC_TRACE_RCVD       1
#define PDQ_UNSOL_STAT_K_CONFIG_CHANGE       2

#define PDQ_UNSOL_STAT_ARG_K_REASON	    0

/* Define link unsolicited event and argument codes 
*/
#define PDQ_UNSOL_LINK_K_XMT_UNDERRUN        0
#define PDQ_UNSOL_LINK_K_XMT_FAILURE         1
#define PDQ_UNSOL_LINK_K_BLOCK_CHECK_ERR	    2
#define PDQ_UNSOL_LINK_K_FRAME_STAT_ERR      3
#define PDQ_UNSOL_LINK_K_LENGTH_ERR	    4
#define PDQ_UNSOL_LINK_K_BAD_IND_DST	    5
#define PDQ_UNSOL_LINK_K_BAD_MCAST_DST	    6
#define PDQ_UNSOL_LINK_K_RCV_OVERRUN         7
#define PDQ_UNSOL_LINK_K_NO_LINK_BUFFER      8
#define PDQ_UNSOL_LINK_K_NO_USER_BUFFER      9
#define PDQ_UNSOL_LINK_K_INIT_INITD	    10
#define PDQ_UNSOL_LINK_K_RING_INIT_RCVD      11
#define PDQ_UNSOL_LINK_K_BEACON_INITD	    12
#define PDQ_UNSOL_LINK_K_DUP_ADDR_FOUND	    13
#define PDQ_UNSOL_LINK_K_DUP_TOKEN_FOUND	    14
#define PDQ_UNSOL_LINK_K_RING_PURGE_ERR	    15
#define PDQ_UNSOL_LINK_K_FCI_STRIP_ERR	    16
#define PDQ_UNSOL_LINK_K_PC_TRACE_INITD	    17
#define PDQ_UNSOL_LINK_K_BEACON_RCVD         18

#define PDQ_UNSOL_LINK_ARG_K_REASON	    0
#define PDQ_UNSOL_LINK_ARG_K_DL_HEADER       1
#define PDQ_UNSOL_LINK_ARG_K_SOURCE          2
#define PDQ_UNSOL_LINK_ARG_K_UP_NBR          3

/* Define PHY event and argument codes 
*/
#define PDQ_UNSOL_PHY_K_LEM_REJECT	    0
#define PDQ_UNSOL_PHY_K_EBUFF_ERR	    1
#define PDQ_UNSOL_PHY_K_LCT_REJECT           2

#define PDQ_UNSOL_PHY_ARG_K_DIRECTION        0

/* Define End-of-Argument list code 
*/
#define PDQ_UNSOL_ARG_K_EOL                  0xFF

/* Event Header 
*/
typedef struct {
    PDQ_ULONG	entity;
    PDQ_ULONG	entity_index;
    PDQ_ULONG	event_code;
    } PDQ_EVENT_HEADER;

/* Reason Argument Description 
*/
typedef struct {
    PDQ_ULONG    arg_code;
    PDQ_ULONG	reason_code;
    } PDQ_UNSOL_ARG_REASON_DESC;

/* Data Link Header Argument Desc 
*/
typedef struct {
    PDQ_ULONG    arg_code;
    struct {
        PDQ_ULONG    fc;
        PDQ_LAN_ADDR dst_addr;
        PDQ_LAN_ADDR src_addr;
        } mac;
    struct {
        PDQ_ULONG    dsap;
        PDQ_ULONG    ssap;
        PDQ_ULONG    control;
        PDQ_ULONG    pid_1;
        PDQ_ULONG    pid_2;
        } llc;
    } PDQ_UNSOL_ARG_DL_HEADER_DESC;

/* Net Address Argument Description 
*/
typedef struct {
    PDQ_ULONG    arg_code;
    PDQ_LAN_ADDR	net_addr;
    } PDQ_UNSOL_ARG_NET_ADDR_DESC;

/* Direction Argument Description 
*/
typedef struct {
    PDQ_ULONG    arg_code;
    PDQ_ULONG	direction;
    } PDQ_UNSOL_ARG_DIRECTION_DESC;

/* Unsol Report: Cntr or Event 
*/
typedef struct {
    PDQ_ULONG	event_type;
    union {
	struct {
	    PDQ_CNTR	    time_since_reset;			
	    PDQ_CNTR_BLK	    cntrs;
	    } cntrs;
        struct {
            PDQ_EVENT_HEADER event_header;
            PDQ_ULONG        event_data[75];
            } report;
	} info;
    } PDQ_UNSOL_REPORT;


/* Define PDQ Consumer Block (resident in host memory) 
*/
typedef struct {
    PDQ_ULONG	xmt_rcv_data;
    PDQ_ULONG	reserved_1;
    PDQ_ULONG	smt_host;
    PDQ_ULONG	reserved_2;
    PDQ_ULONG	unsol;
    PDQ_ULONG	reserved_3;
    PDQ_ULONG	cmd_rsp;
    PDQ_ULONG	reserved_4;
    PDQ_ULONG	cmd_req;
    PDQ_ULONG	reserved_5;
    } PDQ_CONSUMER_BLOCK;

#define PDQ_CONS_M_RCV_INDEX	0X000000FF
#define PDQ_CONS_M_XMT_INDEX     0X00FF0000
#define PDQ_CONS_V_RCV_INDEX     0
#define PDQ_CONS_V_XMT_INDEX     16

/* Offsets into consumer block 
*/
#define PDQ_CONS_BLK_K_XMT_RCV	0X00
#define PDQ_CONS_BLK_K_SMT_HOST	0X08
#define PDQ_CONS_BLK_K_UNSOL	0X10
#define PDQ_CONS_BLK_K_CMD_RSP	0X18
#define PDQ_CONS_BLK_K_CMD_REQ	0X20

/* Offsets into descriptor block 
*/
#define PDQ_DESCR_BLK_K_RCV_DATA	0X0000
#define PDQ_DESCR_BLK_K_XMT_DATA	0X0800
#define PDQ_DESCR_BLK_K_SMT_HOST 0X1000
#define PDQ_DESCR_BLK_K_UNSOL	0X1200
#define PDQ_DESCR_BLK_K_CMD_RSP	0X1280
#define PDQ_DESCR_BLK_K_CMD_REQ	0X1300	


/* Define PDQ Receive Descriptor (Rcv Data, Cmd Rsp, Unsolicited, SMT Host) 
*/
typedef struct {
    PDQ_ULONG	long_1;
    PDQ_ULONG	buff_lo;
    } PDQ_RCV_DESCR;

#define	PDQ_RCV_DESCR_M_SOP	0X80000000
#define PDQ_RCV_DESCR_M_MBZ	0X60000000 
#define PDQ_RCV_DESCR_M_SEG_LEN	0X1F800000
#define PDQ_RCV_DESCR_M_SEG_CNT	0X000F0000
#define PDQ_RCV_DESCR_M_BUFF_HI	0X0000FFFF

#define	PDQ_RCV_DESCR_V_SOP	31
#define PDQ_RCV_DESCR_V_MBZ	29
#define PDQ_RCV_DESCR_V_SEG_LEN	23
#define PDQ_RCV_DESCR_V_SEG_CNT	16
#define PDQ_RCV_DESCR_V_BUFF_HI	0

/* Define the format of a transmit descriptor (Xmt Data, Cmd Req) 
*/
typedef struct {
    PDQ_ULONG	long_1;
    PDQ_ULONG	buff_lo;
    } PDQ_XMT_DESCR;

#define	PDQ_XMT_DESCR_M_SOP	0X80000000
#define PDQ_XMT_DESCR_M_EOP	0X40000000
#define PDQ_XMT_DESCR_M_MBZ	0X20000000 
#define PDQ_XMT_DESCR_M_SEG_LEN	0X1FFF0000
#define PDQ_XMT_DESCR_M_BUFF_HI	0X0000FFFF

#define	PDQ_XMT_DESCR_V_SOP	31
#define	PDQ_XMT_DESCR_V_EOP	30
#define PDQ_XMT_DESCR_V_MBZ	29
#define PDQ_XMT_DESCR_V_SEG_LEN	16
#define PDQ_XMT_DESCR_V_BUFF_HI	0

/* Define format of the Descriptor Block (resident in host memory) 
*/
#define PDQ_RCV_DATA_ENTRIES	256
#define PDQ_XMT_DATA_ENTRIES	256
#define PDQ_SMT_HOST_ENTRIES	64
#define PDQ_UNSOL_ENTRIES	16
#define PDQ_CMD_RSP_ENTRIES	16
#define PDQ_CMD_REQ_ENTRIES	16


typedef struct {
    PDQ_RCV_DESCR  rcv_data [PDQ_RCV_DATA_ENTRIES];
    PDQ_XMT_DESCR  xmt_data [PDQ_XMT_DATA_ENTRIES];
    PDQ_RCV_DESCR  smt_host [PDQ_SMT_HOST_ENTRIES];
    PDQ_RCV_DESCR  unsol    [PDQ_UNSOL_ENTRIES];
    PDQ_RCV_DESCR  cmd_rsp  [PDQ_CMD_RSP_ENTRIES];
    PDQ_XMT_DESCR  cmd_req  [PDQ_CMD_REQ_ENTRIES];
    } PDQ_DESCR_BLOCK;


/* Port Control Register - Command codes for primary commands 
*/
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

/* Define sub-commands accessed via the PDQ_PCTRL_M_SUB_CMD command 
*/
#define PDQ_SUB_CMD_K_LINK_UNINIT	0X0001
#define PDQ_SUB_CMD_K_BURST_SIZE_SET	0X0002

/* Define some Port Data B values 
*/
#define PDQ_PDATA_B_DMA_BURST_SIZE_4     0       /* Valid values for command */
#define PDQ_PDATA_B_DMA_BURST_SIZE_8     1
#define PDQ_PDATA_B_DMA_BURST_SIZE_16    2
#define PDQ_PDATA_B_DMA_BURST_SIZE_32    3

/* Define timeout period for all port control commands but blast_flash 
*/
#define PDQ_PCTRL_K_TIMEOUT		2000		    /* millisecs */

/* Port Data A Reset state 
*/
#define PDQ_PDATA_A_RESET_M_UPGRADE	0X00000001
#define PDQ_PDATA_A_RESET_M_SOFT_RESET	0X00000002
#define PDQ_PDATA_A_RESET_M_SKIP_ST	0X00000004

#define PDQ_PDATA_A_MLA_K_LO		0
#define PDQ_PDATA_A_MLA_K_HI		1

/* Port Reset Register 
*/
#define PDQ_RESET_M_ASSERT_RESET		1

/* Port Status register 
*/
#define PDQ_PSTATUS_V_RCV_DATA_PENDING	31
#define PDQ_PSTATUS_V_XMT_DATA_PENDING	30
#define PDQ_PSTATUS_V_SMT_HOST_PENDING	29
#define PDQ_PSTATUS_V_UNSOL_PENDING	28
#define PDQ_PSTATUS_V_CMD_RSP_PENDING	27
#define PDQ_PSTATUS_V_CMD_REQ_PENDING	26
#define PDQ_PSTATUS_V_TYPE_0_PENDING	25
#define PDQ_PSTATUS_V_RESERVED_1		16
#define PDQ_PSTATUS_V_RESERVED_2		11
#define PDQ_PSTATUS_V_STATE		8
#define PDQ_PSTATUS_V_HALT_ID		0

#define PDQ_PSTATUS_M_RCV_DATA_PENDING	0X80000000
#define PDQ_PSTATUS_M_XMT_DATA_PENDING	0X40000000
#define PDQ_PSTATUS_M_SMT_HOST_PENDING	0X20000000
#define PDQ_PSTATUS_M_UNSOL_PENDING	0X10000000
#define PDQ_PSTATUS_M_CMD_RSP_PENDING	0X08000000
#define PDQ_PSTATUS_M_CMD_REQ_PENDING	0X04000000
#define PDQ_PSTATUS_M_TYPE_0_PENDING	0X02000000
#define PDQ_PSTATUS_M_RESERVED_1		0X01FF0000
#define PDQ_PSTATUS_M_RESERVED_2		0X0000F800
#define PDQ_PSTATUS_M_STATE		0X00000700
#define PDQ_PSTATUS_M_HALT_ID		0X000000FF

/* Define Halt Id's 
*/
#define PDQ_HALT_ID_K_SELFTEST_TIMEOUT	0
#define PDQ_HALT_ID_K_PARITY_ERROR	1
#define PDQ_HALT_ID_K_HOST_DIR_HALT	2
#define PDQ_HALT_ID_K_SW_FAULT		3
#define PDQ_HALT_ID_K_HW_FAULT		4
#define PDQ_HALT_ID_K_PC_TRACE		5
#define PDQ_HALT_ID_K_DMA_ERROR		6	/* Host Data has error reg */

/* Host_Int_Enb_X [lower bits defined in Host Int Type 0 register] 
*/
#define PDQ_TYPE_X_M_XMT_DATA_ENB	    0x80000000  /* Type 2 Enables */
#define PDQ_TYPE_X_M_RCV_DATA_ENB	    0x40000000  

#define PDQ_TYPE_X_M_UNSOL_ENB		    0x20000000	/* Type 1 Enables */
#define PDQ_TYPE_X_M_HOST_SMT_ENB	    0x10000000  
#define PDQ_TYPE_X_M_CMD_RSP_ENB		    0x08000000
#define PDQ_TYPE_X_M_CMD_REQ_ENB		    0x04000000
#define	PDQ_TYPE_X_M_RESERVED_ENB	    0x00FF0000

#define PDQ_TYPE_ALL_INT_DISABLE		    0x00000000
#define PDQ_TYPE_ALL_INT_ENB		    0xFFFFFFFF

/* Host Interrupt Type 0 
*/
#define	PDQ_HOST_INT_0_M_RESERVED	    0x0000FFC0  
#define PDQ_HOST_INT_0_M_20MS	            0X00000040
#define PDQ_HOST_INT_0_M_CSR_CMD_DONE	    0x00000020
#define PDQ_HOST_INT_0_M_STATE_CHANGE	    0x00000010
#define PDQ_HOST_INT_0_M_XMT_DATA_FLUSH	    0x00000008
#define PDQ_HOST_INT_0_M_NXM		    0x00000004
#define PDQ_HOST_INT_0_M_PM_PAR_ERR	    0x00000002
#define PDQ_HOST_INT_0_M_BUS_PAR_ERR         0x00000001

/* Type 1 Producer Register 
*/
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

/* Type 2 Producer Register 
*/
#define PDQ_TYPE_2_PROD_V_XMT_DATA_COMP	    24
#define PDQ_TYPE_2_PROD_V_RCV_DATA_COMP	    16
#define PDQ_TYPE_2_PROD_V_XMT_DATA_PROD	    8
#define PDQ_TYPE_2_PROD_V_RCV_DATA_PROD	    0

#define PDQ_TYPE_2_PROD_M_XMT_DATA_COMP	    0XFF000000	/* 0x00FFFFFF */
#define PDQ_TYPE_2_PROD_M_RCV_DATA_COMP	    0X00FF0000	/* 0xFF00FFFF */
#define PDQ_TYPE_2_PROD_M_XMT_DATA_PROD	    0X0000FF00	/* 0xFFFF00FF */
#define PDQ_TYPE_2_PROD_M_RCV_DATA_PROD	    0X000000FF	/* 0xFFFFFF00 */



/* Application copy of Completion Indicees. The completion indicees are
/* stored in registers on the adapter, and are READ ONLY by the adapter.
/* When the application updates the completion indicees in the adapter,
/* the host copies are updated also. The most frequent use of the
/* completion index is to detect when a queue needs servicing. This
/* allows the application to compare the consumer indeicees to host
/* copies of the completion indicees, saving costly CSR accesses.
*/
typedef struct {
    unsigned int  XmtDataCompletionIndex;
    unsigned int  RcvDataCompletionIndex;
    unsigned int  SMTCompletionIndex;
    unsigned int  UnsolCompletionIndex;
    unsigned int  CmdRspCompletionIndex;
    unsigned int  CmdReqCompletionIndex;

    unsigned int  XmtDataProducer;
    unsigned int  RcvDataProducer;
    unsigned int  SMTProducer;
    unsigned int  UnsolProducer;
    unsigned int  CmdRspProducer;
    unsigned int  CmdReqProducer;
    } PDQ_PRODUCER_INDEX;


/* This struct is mapped to the on-Board DEFEA Eisa Mapped Memory.
/* It is only used for DEFEA Eisa Bus Memory Address calculations.
/* Within the DEFEA Port Block is defined the pointer "DEFEA_EISA_MEMORY *emp"
/* which is assigned the beginning On-Board Eisa Memory address.
/*	ie &pdq->emp->db.xmt_data[i]
*/
typedef struct {
    PDQ_DESCR_BLOCK    db;		/* must be 8k   byte aligned */
    unsigned char RcvBuf    [8192];	/* must be 128  byte aligned */
    unsigned char CmdRspBuf [512];	/* must be 128  byte aligned */
    unsigned char CmdReqBuf [512];	/* must be 128  byte aligned */
    unsigned char UnsolBuf  [512];	/* must be 128  byte aligned */
    PDQ_CONSUMER_BLOCK cb;		/* must be 64   byte aligned */
    unsigned char junk [1624];		/*         4096 byte aligned */
    unsigned char XmtBuf    [4096];	/* must be 4096 byte aligned */
    } DEFEA_EISA_MEMORY;
