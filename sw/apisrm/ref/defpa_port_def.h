/* File:	depfa_port_def.h
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
 * Abstract:	DEFPA port definitions Definitions
 *
 * Author:	John R. Kirchoff
 *
 * Modifications:
 *
 *	jrk	17-Aug-1994	Initial entry.
 */
 
/*+
/* Define types 
-*/
typedef unsigned char FDDI_NET_ADDR[6];
typedef unsigned int  FW_ULONG;

/*+
/* The TURBO_DMA_MASK is used to mask off the top five bits of the addresses
/* stuffed into the PDQ DMA descriptors. On Turbochannel platforms, the DMA
/* address for a given physical address is calculated by masking off the top
/* five bits of the address. It's weird, but it works.
-*/
#define TURBO_DMA_MASK 0x07FFFFFF
  
/*+
/* Define maximum and minimum legal sizes for frame types 
-*/
#define FDDI_LLC_K_LENGTH_MIN	20
#define FDDI_LLC_K_LENGTH_MAX	4495
#define FDDI_SMT_K_LENGTH_MIN	33
#define FDDI_SMT_K_LENGTH_MAX	4494
#define FDDI_MAC_K_LENGTH_MIN	18
#define FDDI_MAC_K_LENGTH_MAX	4494

/*+
/* Define FC's - Frame Control
-*/
#define FDDI_FC_K_VOID			0x00	
#define FDDI_FC_K_NON_RESTRICTED_TOKEN	0x80	
#define FDDI_FC_K_RESTRICTED_TOKEN	0xC0	
#define FDDI_FC_K_SMT			0x41	
#define FDDI_FC_K_MAC			0xC1	
#define FDDI_FC_K_LLC			0x50	
#define FDDI_FC_K_IMPLEMENTOR		0x60
#define FDDI_FC_K_RESERVED		0x70

#define FDDI_FC_M_BASE			0xF1	/* Get unique part of FC */

#define FDDI_K_SMT_XID_APP		0x80000000
#define FDDI_K_SMT_XID_ENT		0x00000000

#define FDDI_FRAME_K_SMT_TYPE_RSP	0x01
#define FDDI_FRAME_K_SMT_TYPE_NO_RSP	0x00

/*+
/* FDDI frame header 
-*/
typedef struct {		
    unsigned char   prh_0;		/* byte alignment	*/
    unsigned char   prh_1;		/* byte alignment	*/
    unsigned char   prh_2;		/* byte alignment	*/
    unsigned char   fc;			/* frame controle	*/
    FDDI_NET_ADDR   dst_addr;		/* destination address 	*/
    FDDI_NET_ADDR   src_addr;		/* source address	*/
} FDDI_HEADER;

/*+
/* General FDDI frame 
-*/
typedef struct {
    FDDI_HEADER	    header;
    unsigned char   info[4500];
} FDDI_FRAME_GEN;


typedef struct rcv_status {
  union {
    FW_ULONG 	status;
    struct {
      unsigned rpkt_len : 13;
      unsigned rcc : 9;
      unsigned fsb : 5;
      unsigned fsc : 3;
      unsigned resrv : 2;
    } rstat;
  } descr;
} FW_RCV_STATUS; 

typedef struct pkt_req_header0 {
  union {
    unsigned char pkt_head0;
    struct {
      unsigned beacon_frame : 1;
      unsigned send_first : 1;
      unsigned imm_mode : 1;
      unsigned sync_frame : 1;
      unsigned token_type : 2;
      unsigned format_type : 2;
    } pkt;
  } header;
} FW_PKT_REQ_HEAD0;

typedef struct pkt_req_header1 {
  union {
    unsigned char pkt_head1;
    struct {
      unsigned extra_fs : 3;
      unsigned token_send : 2;
      unsigned append_crc : 1;
      unsigned send_last : 1;
      unsigned resrv0 : 1;
    } pkt;
  } header;
} FW_PKT_REQ_HEAD1;

/*+
/* Define general structures 
-*/
typedef struct {
   unsigned int    rcv_descriptor;	/* Receive status */
   unsigned char   prh_0;		/* Byte alignment */
   unsigned char   prh_1;		/* Byte alignment */
   unsigned char   prh_2;		/* Byte alignment */
   unsigned char   fc;			/* Frame Control */
   FDDI_NET_ADDR   dest_addr;		/* Destination Address */
   FDDI_NET_ADDR   source_addr;		/* Source      Address */
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

typedef struct {
   unsigned char   prh_0;		/* Packet Request Header 0 */
   unsigned char   prh_1;		/* Packet Request Header 1 */
   unsigned char   prh_2;		/* Packet Request Header 2 */
   unsigned char   fc;			/* Frame Control */
   FDDI_NET_ADDR   dest_addr;		/* Destination Address */
   FDDI_NET_ADDR   source_addr;		/* Source      Address */
   unsigned char   dsap;		/* Destination Service Access Point */
   unsigned char   ssap;		/* Source Service Access Point */
   unsigned char   cntr;		/* Unnumbered Information */
   unsigned char   pid_0;		/* IEEE Protocol Prefix [0] */
   unsigned char   pid_1;		/* IEEE Protocol Prefix [1] */
   unsigned char   pid_2;		/* IEEE Protocol Prefix [2] */
   unsigned char   pid_3;		/* IEEE Protocol  [3] */
   unsigned char   pid_4;		/* IEEE Protocol  [4] */
   unsigned char   data[4500];		/* Transmit data */
}  PDQ_XMT_DATA_BUFFER;


typedef struct {
   FDDI_NET_ADDR   dest_addr;
   FDDI_NET_ADDR   source_addr;
   unsigned char   protocol_type_0;
   unsigned char   protocol_type_1;
   unsigned char   data[4500];
}  ETHERNET_FRAME;


/*+
/* 64-bit counter 
-*/
typedef struct {                              
    FW_ULONG  ms;
    FW_ULONG  ls;
    } FW_CNTR;

/*+
/* LAN address 
-*/
typedef struct {                               
    FW_ULONG  lwrd_0;
    FW_ULONG  lwrd_1;
    } FW_LAN_ADDR;

/*+
/* Station ID address 
-*/
typedef struct {
    FW_ULONG  octet_7_4;
    FW_ULONG  octet_3_0;
    } FW_STATION_ID;

/*+
/* Define general constants 
-*/
#define FW_DMA_K_TIMEOUT		    2000    /* Milliseconds */

#define FW_PHY_K_S			    0	    /* Index to S phy */
#define FW_PHY_K_A			    0	    /* Index to A phy */
#define FW_PHY_K_B			    1	    /* Index to B phy */
#define FW_PHY_K_MAX			    2	    /* Max number of phys */

/*+
/* Define FDDI Data Link Functional Specification Data Types 
-*/
#define FW_STATION_TYPE_K_SAS		    0       /* Station Type */
#define FW_STATION_TYPE_K_DAC		    1
#define FW_STATION_TYPE_K_SAC		    2
#define FW_STATION_TYPE_K_NAC		    3
#define FW_STATION_TYPE_K_DAS		    4

#define FW_STATION_STATE_K_OFF		    0       /* Station State */
#define FW_STATION_STATE_K_ON		    1
#define FW_STATION_STATE_K_LOOPBACK	    2

#define FW_LINK_STATE_K_OFW_READY           1       /* Link State */
#define FW_LINK_STATE_K_OFW_FAULT           2
#define FW_LINK_STATE_K_ON_RING_INIT        3
#define FW_LINK_STATE_K_ON_RING_RUN         4
#define FW_LINK_STATE_K_BROKEN              5

#define FW_DA_TEST_STATE_K_UNKNOWN          0       /* Dupl Addr Test State */
#define FW_DA_TEST_STATE_K_SUCCESS          1
#define FW_DA_TEST_STATE_K_DUPLICATE        2

#define FW_RP_STATE_K_OFF                   0       /* Ring Purger State */
#define FW_RP_STATE_K_CANDIDATE             1
#define FW_RP_STATE_K_NON_PURGER            2
#define FW_RP_STATE_K_PURGER                3

#define FW_FS_MODE_K_SA_MATCH               0       /* Frame Strip Mode */
#define FW_FS_MODE_K_FCI_STRIP              1

#define FW_RING_ERR_RSN_K_NO_ERROR          0       /* Ring Error Reason */
#define FW_RING_ERR_RSN_K_INIT_INIT	    5
#define FW_RING_ERR_RSN_K_INIT_RCVD	    6
#define FW_RING_ERR_RSN_K_BEACON_INIT	    7
#define FW_RING_ERR_RSN_K_DUP_ADDR	    8
#define FW_RING_ERR_RSN_K_DUP_TOKEN	    9
#define FW_RING_ERR_RSN_K_PURGE_ERROR	    10
#define FW_RING_ERR_RSN_K_FCI_ERROR	    11
#define FW_RING_ERR_RSN_K_RING_OP	    12
#define FW_RING_ERR_RSN_K_DIR_BEACON	    13
#define FW_RING_ERR_RSN_K_TRACE_INIT	    14
#define FW_RING_ERR_RSN_K_TRACE_RCVD	    15

#define FW_STATION_MODE_K_NORMAL            0       /* Station Mode */
#define FW_STATION_MODE_K_LOOPBACK          1

#define FW_PHY_TYPE_K_A                     0       /* PHY Type */
#define FW_PHY_TYPE_K_B                     1
#define FW_PHY_TYPE_K_S                     2
#define FW_PHY_TYPE_K_M                     3
#define FW_PHY_TYPE_K_UNKNOWN               4

#define FW_PMD_TYPE_K_ANSI_MULTI            0       /* PMD Type */
#define FW_PMD_TYPE_K_ANSI_SINGLE_1         1
#define FW_PMD_TYPE_K_ANSI_SINGLE_2         2
#define FW_PMD_TYPE_K_ANSI_SONET            3
#define FW_PMD_TYPE_K_LOW_POWER             100
#define FW_PMD_TYPE_K_THIN_WIRE             101
#define FW_PMD_TYPE_K_SHIELD_TWISTED        102
#define FW_PMD_TYPE_K_UNSHIELD_TWISTED      103

#define FW_PHY_STATE_K_INT_LOOPBACK         0       /* PHY State */
#define FW_PHY_STATE_K_BROKEN               1
#define FW_PHY_STATE_K_OFW_READY            2
#define FW_PHY_STATE_K_WAITING              3
#define FW_PHY_STATE_K_STARTING             4
#define FW_PHY_STATE_K_FAILED               5
#define FW_PHY_STATE_K_WATCH                6
#define FW_PHY_STATE_K_IN_USE               7

#define FW_REJECT_RSN_K_NONE                0       /* Reject Reason */
#define FW_REJECT_RSN_K_LOCAL_LCT           1
#define FW_REJECT_RSN_K_REMOTE_LCT          2
#define FW_REJECT_RSN_K_LCT_BOTH            3
#define FW_REJECT_RSN_K_LEM_REJECT          4
#define FW_REJECT_RSN_K_TOPOLOGY_ERROR      5
#define FW_REJECT_RSN_K_NOISE_REJECT        6
#define FW_REJECT_RSN_K_REMOTE_REJECT       7
#define FW_REJECT_RSN_K_TRACE_IN            8
#define FW_REJECT_RSN_K_TRACE_RCVD_DIS      9
#define FW_REJECT_RSN_K_STANDBY             10
#define FW_REJECT_RSN_K_PROTO_ERROR         11

#define FW_RI_RSN_K_TVX_EXPIRED             0       /* RI Reason */
#define FW_RI_RSN_K_TRT_EXPIRED             1
#define FW_RI_RSN_K_RING_PURGER             2
#define FW_RI_RSN_K_PURGE_ERROR             3
#define FW_RI_RSN_K_TOKEN_TIMEOUT           4

#define FW_LCT_DIRECTION_K_LOCAL            1       /* LCT Direction */
#define FW_LCT_DIRECTION_K_REMOTE           2
#define FW_LCT_DIRECTION_K_BOTH             3

/*+
/* Define FMC descriptor fields 
-*/
#define FW_FMC_DESCR_V_SOP		    31
#define FW_FMC_DESCR_V_EOP		    30
#define FW_FMC_DESCR_V_FSC		    27
#define FW_FMC_DESCR_V_FSB_ERROR	    26
#define FW_FMC_DESCR_V_FSB_ADDR_RECOG	    25
#define FW_FMC_DESCR_V_FSB_ADDR_COPIED	    24
#define FW_FMC_DESCR_V_FSB		    22
#define FW_FMC_DESCR_V_RCC_FLUSH	    21
#define FW_FMC_DESCR_V_RCC_CRC		    20
#define FW_FMC_DESCR_V_RCC_RRR		    17
#define FW_FMC_DESCR_V_RCC_DD               15
#define FW_FMC_DESCR_V_RCC_SS               13
#define FW_FMC_DESCR_V_RCC		    13
#define FW_FMC_DESCR_V_LEN		    0

#define FW_FMC_DESCR_M_SOP		    0x80000000	
#define FW_FMC_DESCR_M_EOP		    0x40000000	

/*+
/* Format Receive Status
-*/
#define FW_FMC_DESCR_M_FSC		    0x38000000	/* <29:27> */
#define FW_FMC_DESCR_M_FSB_ERROR	    0x04000000	/* <26:26> */
#define FW_FMC_DESCR_M_FSB_ADDR_RECOG	    0x02000000	/* <25:25> */
#define FW_FMC_DESCR_M_FSB_ADDR_COPIED	    0x01000000	/* <24:24> */
#define FW_FMC_DESCR_M_FSB		    0x07C00000	/* <26:22> */
#define FW_FMC_DESCR_M_RCC_FLUSH	    0x00200000	/* <21:21> */
#define FW_FMC_DESCR_M_RCC_CRC		    0x00100000	/* <20:20> */
#define FW_FMC_DESCR_M_RCC_RRR		    0x000E0000	/* <19:17> */
#define FW_FMC_DESCR_M_RCC_DD               0x00018000	/* <16:15> */
#define FW_FMC_DESCR_M_RCC_SS               0x00006000	/* <13:14> */
#define FW_FMC_DESCR_M_RCC		    0x003FE000	/* <21:13> */
#define FW_FMC_DESCR_M_LEN		    0x00001FFF	/* <12:0> */

#define FW_FMC_DESCR_K_RCC_FMC_INT_ERR	    0x01AA

#define FW_FMC_DESCR_K_RRR_SUCCESS	    0x00
#define FW_FMC_DESCR_K_RRR_SA_MATCH	    0x01
#define FW_FMC_DESCR_K_RRR_DA_MATCH	    0x02
#define FW_FMC_DESCR_K_RRR_FMC_ABORT	    0x03
#define FW_FMC_DESCR_K_RRR_LENGTH_BAD	    0x04
#define FW_FMC_DESCR_K_RRR_FRAGMENT	    0x05
#define FW_FMC_DESCR_K_RRR_FORMAT_ERR	    0x06
#define FW_FMC_DESCR_K_RRR_MAC_RESET	    0x07

#define FW_FMC_DESCR_K_DD_NO_MATCH          0x0
#define FW_FMC_DESCR_K_DD_PROMISCUOUS       0x1
#define FW_FMC_DESCR_K_DD_CAM_MATCH         0x2
#define FW_FMC_DESCR_K_DD_LOCAL_MATCH       0x3

#define FW_FMC_DESCR_K_SS_NO_MATCH          0x0
#define FW_FMC_DESCR_K_SS_BRIDGE_MATCH      0x1
#define FW_FMC_DESCR_K_SS_NOT_POSSIBLE      0x2
#define FW_FMC_DESCR_K_SS_LOCAL_MATCH       0x3

/*+
/* Define some max buffer sizes 
-*/
#define FW_CMD_REQ_K_SIZE_MAX		    512
#define FW_CMD_RSP_K_SIZE_MAX		    512
#define FW_UNSOL_K_SIZE_MAX		    512
#define FW_SMT_HOST_K_SIZE_MAX		    4608		/* 4 1/2 K */
#define FW_RCV_DATA_K_SIZE_MAX		    4608		/* 4 1/2 K */
#define FW_XMT_DATA_K_SIZE_MAX		    4608		/* 4 1/2 K */


/* Define general frame formats 
*/

typedef struct {
    FDDI_FRAME_GEN  frame;			    /* Regular FDDI frame */
    } FW_RCV_FRAME;

/*+
/* Define codes for command type 
-*/
#define FW_CMD_K_START			    0x00
#define FW_CMD_K_FILTERS_SET		    0x01
#define FW_CMD_K_FILTERS_GET	            0x02
#define FW_CMD_K_CHARS_SET	            0x03
#define FW_CMD_K_STATUS_CHARS_GET	    0x04
#define FW_CMD_K_CNTRS_GET		    0x05
#define FW_CMD_K_CNTRS_SET		    0x06
#define FW_CMD_K_ADDR_FILTER_SET	    0x07
#define FW_CMD_K_ADDR_FILTER_GET	    0x08
#define FW_CMD_K_ERROR_LOG_CLEAR	    0x09
#define FW_CMD_K_ERROR_LOG_GET		    0x0A
#define FW_CMD_K_FDDI_MIB_GET               0x0B
#define FW_CMD_K_DEC_EXT_MIB_GET            0x0C
#define FW_CMD_K_DEVICE_SPECIFIC_GET        0x0D
#define FW_CMD_K_STATUS_CHARS_TEST	    0x0E
#define FW_CMD_K_UNSOL_TEST                 0x0F
#define FW_CMD_K_SMT_MID_GET		    0x10
#define FW_CMD_K_SMT_MID_SET		    0x11
#define FW_CMD_K_MAX			    0x11	/* Must match prev */

/*+
/* Define item codes for Chars_Set and Filters_Set commands 
-*/
#define FW_ITEM_K_EOL                       0x00
#define FW_ITEM_K_T_REQ                     0x01      /* MACTReq */
#define FW_ITEM_K_TVX                       0x02      /* MACTvxValue */
#define FW_ITEM_K_RESTRICTED_TOKEN          0x03      /* eMACRestrictedTokenTimeout */
#define FW_ITEM_K_LEM_THRESHOLD             0x04
#define FW_ITEM_K_RING_PURGER               0x05      /* eMACRingPurgerEnable */
#define FW_ITEM_K_CNTR_INTERVAL             0x06
#define FW_ITEM_K_IND_GROUP_PROM            0x07
#define FW_ITEM_K_GROUP_PROM                0x08
#define FW_ITEM_K_BROADCAST                 0x09
#define FW_ITEM_K_SMT_PROM                  0x0A
#define FW_ITEM_K_SMT_USER                  0x0B
#define FW_ITEM_K_RESERVED                  0x0C
#define FW_ITEM_K_IMPLEMENTOR               0x0D
#define FW_ITEM_K_LOOPBACK_MODE             0x0E
#define FW_ITEM_K_SMT_VERSION_ID            0x0F      /* SMTOpVersionId */
#define FW_ITEM_K_CONFIG_POLICY             0x10      /* SMTConfigPolicy */
#define FW_ITEM_K_CON_POLICY                0x11      /* SMTConnectionPolicy */
#define FW_ITEM_K_T_NOTIFY                  0x12      /* SMTTNotify */
#define FW_ITEM_K_STATION_ACTION            0x13      /* SMTStationAction */
#define FW_ITEM_K_T_MAX_GREATEST_LB         0x14      /* MACTMaxGreatestLowerBound */
#define FW_ITEM_K_MAC_PATHS_REQ       	    0x15      /* MACPathsRequested */
#define FW_ITEM_K_FRAME_STATUS              0x16      /* MACCurrentFrameStatus */
#define FW_ITEM_K_MAC_ACTION                0x17      /* MACAction */
#define FW_ITEM_K_CON_POLICIES              0x18      /* PORTConnectionPolicies */
#define FW_ITEM_K_PORT_PATHS_REQ            0x19      /* PORTPathsRequested */
#define FW_ITEM_K_MAC_LOOP_TIME             0x1A      /* PORTMACLoopTime */
#define FW_ITEM_K_TB_MAX                    0x1B      /* PORTTBMax */
#define FW_ITEM_K_LER_CUTOFF                0x1C      /* PORTLerCutoff */
#define FW_ITEM_K_LER_ALARM                 0x1D      /* PORTLerAlarm */
#define FW_ITEM_K_PORT_ACTION               0x1E      /* PORTAction */
#define FW_ITEM_K_INSERT_POLICY             0x1F      /* ATTACHMENTInsertPolicy */
#define FW_ITEM_K_FLUSH_TIME                0x20 
#define FW_ITEM_K_SMT_USER_DATA		    0x21      
#define FW_ITEM_K_SMT_STAT_RPT_POLICY	    0x22
#define FW_ITEM_K_SMT_TRACE_MAX_EXPIR	    0x23
#define FW_ITEM_K_MAC_FRMERR_THRESHLD       0x24
#define FW_ITEM_K_MAC_UNIT_DATA_ENA         0x25
#define FW_ITEM_K_PATH_TVX_LOWBND           0x26
#define FW_ITEM_K_PATH_TMAX_LOWBND          0x27
#define FW_ITEM_K_PATH_MAX_TREQ             0x28
#define FW_ITEM_K_MAC_TREQ		    0x29
#define FW_ITEM_K_EMAC_RING_PURGER	    0x2A
#define FW_ITEM_K_EMAC_RTOKEN_TIMEOUT	    0x2B
#define FW_ITEM_K_FULL_DUPLEX_ENA           0x2C
#define FW_ITEM_K_MAX			    0x2C      /* Must equal high item */

/*+
/* Values for some of the items 
-*/
#define FW_K_OFF			    0       /* Generic OFF|ON consts */
#define FW_K_ON				    1

#define FW_FSTATE_K_BLOCK		    0       /* Filter State */
#define FW_FSTATE_K_PASS		    1

/*+
/* Item value limits and defaults 
-*/
#define FW_TIME_UNIT			    80
#define FW_ITEM_K_T_REQ_MIN		    (4000000/FW_TIME_UNIT)
#define FW_ITEM_K_T_REQ_MAX		    (167772080/FW_TIME_UNIT)
#define FW_ITEM_K_T_REQ_DEF		    (8000000/FW_TIME_UNIT)
#define FW_ITEM_K_TVX_MIN		    (2500000/FW_TIME_UNIT)
#define FW_ITEM_K_TVX_MAX		    (5222400/FW_TIME_UNIT)
#define FW_ITEM_K_TVX_DEF		    (2621440/FW_TIME_UNIT)
#define FW_ITEM_K_RESTRICTED_TOKEN_MIN	    0
#define FW_ITEM_K_RESTRICTED_TOKEN_MAX	    ((1000000000/FW_TIME_UNIT)*10)
#define FW_ITEM_K_RESTRICTED_TOKEN_DEF	    (1000000000/FW_TIME_UNIT)
#define FW_ITEM_K_LEM_THRESHOLD_MIN	    5
#define FW_ITEM_K_LEM_THRESHOLD_MAX	    8
#define FW_ITEM_K_LEM_THRESHOLD_DEF	    8
#define FW_ITEM_K_RING_PURGER_DEF           FW_K_ON  /* def - ring purger ON */
#define FW_ITEM_K_IND_GROUP_PROM_DEF	    FW_FSTATE_K_BLOCK
#define FW_ITEM_K_GROUP_PROM_DEF	    FW_FSTATE_K_BLOCK
#define FW_ITEM_K_BROADCAST_DEF		    FW_FSTATE_K_BLOCK
#define FW_ITEM_K_SMT_PROM_DEF		    FW_FSTATE_K_BLOCK
#define FW_ITEM_K_SMT_USER_DEF	            FW_FSTATE_K_BLOCK
#define FW_ITEM_K_RESERVED_DEF		    FW_FSTATE_K_BLOCK
#define FW_ITEM_K_IMPLEMENTOR_DEF	    FW_FSTATE_K_BLOCK
#define FW_ITEM_K_LOOPBACK_NONE             0
#define FW_ITEM_K_LOOPBACK_INT              1
#define FW_ITEM_K_LOOPBACK_EXT              2
#define FW_ITEM_K_SMT_VERSION_ID_DEF        2        /* supported SMT Version */
#define FW_ITEM_K_CONFIG_POLICY_DEF         0        /* none supported (def) */
#define FW_ITEM_K_CON_POLICY_MIN            0        /* Reject none */
#define FW_ITEM_K_CON_POLICY_MAX            65535    /* Reject A-A A-B A-S... */
#define FW_ITEM_K_CON_POLICY_DEF            0x8021   /* Reject A-A B-B M-M */
#define FW_ITEM_K_T_NOTIFY_MIN              2        /* 2 second minimum */
#define FW_ITEM_K_T_NOTIFY_MAX              30       /* 30 second maximum */
#define FW_ITEM_K_T_NOTIFY_DEF              10       /* 10 second default */
#define FW_ITEM_K_T_MAX_GREATEST_LB_MIN     0
#define FW_ITEM_K_T_MAX_GREATEST_LB_MAX     (165000000/FW_TIME_UNIT)
#define FW_ITEM_K_T_MAX_GREATEST_LB_DEF     (165000000/FW_TIME_UNIT)
#define FW_ITEM_K_MAC_PATHS_REQ_DEF   	    1        /* Primary path default */
#define FW_ITEM_K_FRAME_STATUS_DEF          1        /* FSC-Type0 default */
#define FW_ITEM_K_CON_POLICIES_MIN          0        /* none */
#define FW_ITEM_K_CON_POLICIES_MAX          7        /* LCT, Loop & Placement */
#define FW_ITEM_K_CON_POLICIES_DEF          4        /* Pc-MAC-Placement def */
#define FW_ITEM_K_PORT_PATHS_REQ_DEF        1        /* Primary path default */
#define FW_ITEM_K_MAC_LOOP_TIME_MIN         (200000000/FW_TIME_UNIT)
#define FW_ITEM_K_MAC_LOOP_TIME_MAX         ((1000000000/FW_TIME_UNIT)*10)
#define FW_ITEM_K_MAC_LOOP_TIME_DEF         (200000000/FW_TIME_UNIT)
#define FW_ITEM_K_TB_MAX_MIN                (30000000/FW_TIME_UNIT)
#define FW_ITEM_K_TB_MAX_MAX                ((1000000000/FW_TIME_UNIT)*10)
#define FW_ITEM_K_TB_MAX_DEF                (30000000/FW_TIME_UNIT)
#define FW_ITEM_K_LER_CUTOFW_MIN            4        /* min value - SMT spec */
#define FW_ITEM_K_LER_CUTOFW_MAX            15       /* max value - SMT spec */
#define FW_ITEM_K_LER_CUTOFW_DEF            7        /* def value - SMT spec */
#define FW_ITEM_K_LER_ALARM_MIN             4        /* min value - SMT spec */
#define FW_ITEM_K_LER_ALARM_MAX             15       /* max value - SMT spec */
#define FW_ITEM_K_LER_ALARM_DEF             7        /* def value - SMT spec */
#define FW_ITEM_K_INSERT_POLICY_DEF         1        /* insert = true (def) */
#define FW_ITEM_K_FLUSH_TIME_MIN            0	     /* Disables flush */
#define FW_ITEM_K_FLUSH_TIME_MAX            255      /* Number of seconds */
#define FW_ITEM_K_FLUSH_TIME_DEF            3        /* Arch recommended */

/*+
/* Define command return codes 
-*/
#define FW_RSP_K_SUCCESS		    0x00
#define FW_RSP_K_FAILURE		    0x01
#define FW_RSP_K_WARNING		    0x02
#define FW_RSP_K_LOOP_MODE_BAD		    0x03
#define FW_RSP_K_ITEM_CODE_BAD		    0x04
#define FW_RSP_K_TVX_BAD		    0x05
#define FW_RSP_K_TREQ_BAD		    0x06
#define FW_RSP_K_TOKEN_BAD		    0x07
#define FW_RSP_K_PROM_LLC_BAD		    0x08
#define FW_RSP_K_PROM_GROUP_BAD		    0x09
#define FW_RSP_K_PROM_SMT_USER_BAD	    0x0A
#define FW_RSP_K_SMT_PROM_BAD		    0x0B
#define FW_RSP_K_NO_EOL			    0x0C
#define FW_RSP_K_FILTER_STATE_BAD	    0x0D
#define FW_RSP_K_CMD_TYPE_BAD		    0x0E
#define FW_RSP_K_ADAPTER_STATE_BAD	    0x0F
#define FW_RSP_K_RING_PURGER_BAD	    0x10
#define FW_RSP_K_LEM_THRESHOLD_BAD	    0x11
#define FW_RSP_K_LOOP_NOT_SUPPORTED	    0x12
#define FW_RSP_K_FLUSH_TIME_BAD		    0x13
#define FW_RSP_K_NOT_YET_IMPL		    0x14
#define FW_RSP_K_CONFIG_POLICY_BAD	    0x15
#define FW_RSP_K_STATION_ACTION_BAD         0x16
#define FW_RSP_K_MAC_ACTION_BAD             0x17
#define FW_RSP_K_CON_POLICIES_BAD           0x18
#define FW_RSP_K_MAC_LOOP_TIME_BAD          0x19
#define FW_RSP_K_TB_MAX_BAD                 0x1A
#define FW_RSP_K_LER_CUTOFW_BAD   	    0x1B
#define FW_RSP_K_LER_ALARM_BAD	            0x1C
#define FW_RSP_K_MAC_PATHS_REQ_BAD	    0x1D
#define FW_RSP_K_MAC_T_REQ_BAD              0x1E
#define FW_RSP_K_EMAC_RING_PURGER_BAD       0x1F
#define FW_RSP_K_EMAC_TOKEN_TOUT_BAD        0x20
#define FW_RSP_K_NO_SUCH_ENTRY              0x21
#define FW_RSP_K_T_NOTIFY_BAD               0x22
#define FW_RSP_K_TR_MAX_EXP_BAD		    0x23
#define FW_RSP_K_FRAME_ERR_THRES_BAD        0x24
#define FW_RSP_K_MAX_TREQ_BAD               0x25
#define FW_RSP_K_FULL_DUPLEX_ENA_BAD        0x26
#define FW_RSP_K_ITEM_INDEX_BAD	            0x27


/*+
/* Commonly used structures 
-*/

/*+
/* Item list 
-*/
typedef struct {
    FW_ULONG  item_code;
    FW_ULONG  value;
    } FW_ITEM_LIST;

/*+
/* Response header 
-*/
typedef struct {
    FW_ULONG  rsvd;
    FW_ULONG  cmd_type;
    FW_ULONG  status;
    } FW_RSP_HEADER;

/*+
/* Start Command 
-*/
typedef struct {
    FW_ULONG  cmd_type;
    } FW_CMD_START_REQ;

/*+
/* Start Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    } FW_CMD_START_RSP;

/*+
/* Filters_Set Request 
-*/
#define FW_CMD_FILTERS_SET_K_ITEMS_MAX  8	/* Num filters + 1 */

typedef struct {
    FW_ULONG	    cmd_type;
    FW_ITEM_LIST    item[FW_CMD_FILTERS_SET_K_ITEMS_MAX];
    } FW_CMD_FILTERS_SET_REQ;

/*+
/* Filters_Set Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    } FW_CMD_FILTERS_SET_RSP;

/*+
/* Filters_Get Request 
-*/
typedef struct {
    FW_ULONG        cmd_type;   
    } FW_CMD_FILTERS_GET_REQ;

/*+
/* Filters_Get Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    FW_ULONG	    ind_group_prom;
    FW_ULONG	    group_prom;
    FW_ULONG	    broadcast_all;
    FW_ULONG	    smt_all;
    FW_ULONG	    smt_user;
    FW_ULONG	    reserved_all;
    FW_ULONG	    implementor_all;
    } FW_CMD_FILTERS_GET_RSP;

/*+
/* Chars_Set Request 
-*/
#define FW_CMD_CHARS_SET_K_ITEMS_MAX (FW_ITEM_K_MAX + 1)       /* 1 for tests */

typedef struct {
    FW_ULONG	    cmd_type;
    struct {                               /* Item list */
        FW_ULONG    item_code;
        FW_ULONG    value;
        FW_ULONG    item_index;
        } item[FW_CMD_CHARS_SET_K_ITEMS_MAX];    
    } FW_CMD_CHARS_SET_REQ;

/*+
/* Chars_Set Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    } FW_CMD_CHARS_SET_RSP;

/*+
/* Addr_Filter_Set Request 
-*/
#define FW_CMD_ADDR_FILTER_K_SIZE   62

typedef struct {
    FW_ULONG	cmd_type;
    FW_LAN_ADDR	entry[FW_CMD_ADDR_FILTER_K_SIZE];
    } FW_CMD_ADDR_FILTER_SET_REQ;

/*+
/* Addr_Filter_Set Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    } FW_CMD_ADDR_FILTER_SET_RSP;

/*+
/* Addr_Filter_Get Request 
-*/
typedef struct {
    FW_ULONG	cmd_type;
    } FW_CMD_ADDR_FILTER_GET_REQ;

/*+
/* Addr_Filter_Get Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    FW_LAN_ADDR	    entry[FW_CMD_ADDR_FILTER_K_SIZE];
    } FW_CMD_ADDR_FILTER_GET_RSP;

/*+
/* Dev_Specific_Get Request 
-*/
typedef struct {
    FW_ULONG  cmd_type;
    } FW_CMD_DEVICE_SPECIFIC_GET_REQ;

/*+
/* Dev_Specific_Get Response => defined in device specific module 
/* Status_Chars_Get Request 
-*/
typedef struct {
    FW_ULONG  cmd_type;
    } FW_CMD_STATUS_CHARS_GET_REQ;

/*+
/* Status_Chars_Get Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    /*+
    /* Station Characteristic Attributes 	
    -*/
    FW_STATION_ID   station_id;		/* Station */
    FW_ULONG	    station_type;
    FW_ULONG	    smt_ver_id;
    FW_ULONG	    smt_ver_id_max;
    FW_ULONG	    smt_ver_id_min;
    /*+
    /* Station status attributes
    -*/
    FW_ULONG	    station_state;
    /*+
    /* link characteristic attributes
    -*/
    FW_LAN_ADDR	    link_addr;		/* Link */
    FW_ULONG	    t_req;
    FW_ULONG	    tvx;
    FW_ULONG	    token_timeout;
    FW_ULONG	    purger_enb;
    FW_ULONG	    link_state;
    FW_ULONG	    tneg;
    FW_ULONG	    dup_addr_flag;
    FW_LAN_ADDR	    una;		/* upstream neighbor		*/
    FW_LAN_ADDR	    una_old;		/* upstream neighbor old	*/
    FW_ULONG	    un_dup_addr_flag;
    FW_LAN_ADDR	    dna;		/* downstream neighbor		*/
    FW_LAN_ADDR	    dna_old;
    FW_ULONG	    purger_state;
    FW_ULONG	    fci_mode;		/* frame strip mode 		*/
    FW_ULONG	    error_reason;	/* ring error reason		*/
    FW_ULONG	    loopback;
    FW_ULONG	    ring_latency;
    FW_LAN_ADDR	    last_dir_beacon_sa;
    FW_LAN_ADDR	    last_dir_beacon_una;
    /*+
    /* physical characteristic attributes
    -*/
    FW_ULONG	    phy_type[FW_PHY_K_MAX];		/* Phy */
    FW_ULONG	    pmd_type[FW_PHY_K_MAX];
    FW_ULONG	    lem_threshold[FW_PHY_K_MAX];
    /*+
    /* physical status attributes
    -*/
    FW_ULONG	    phy_state[FW_PHY_K_MAX];
    FW_ULONG	    nbor_phy_type[FW_PHY_K_MAX];
    FW_ULONG	    link_error_est[FW_PHY_K_MAX];
    FW_ULONG	    broken_reason[FW_PHY_K_MAX];
    FW_ULONG	    reject_reason[FW_PHY_K_MAX];
    /*+
    /* miscellaneous
    -*/
    FW_ULONG	    cntr_interval;			/* Miscellaneous */
    FW_ULONG	    module_rev;
    FW_ULONG	    firmware_rev;
    FW_ULONG	    mop_device_type;
    FW_ULONG	    phy_led[FW_PHY_K_MAX];
    FW_ULONG	    flush_time;
    } FW_CMD_STATUS_CHARS_GET_RSP;

/*+
/* STATUS_CHARS_TEST Request 
-*/
typedef struct {
    FW_ULONG  cmd_type;
    } FW_CMD_STATUS_CHARS_TEST_REQ;

/*+
/* STATUS_CHARS_TEST Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    } FW_CMD_STATUS_CHARS_TEST_RSP;
	
/*+
/* FDDI_MIB_Get Request 
-*/
typedef struct {
    FW_ULONG  cmd_type;
    } FW_CMD_FDDI_MIB_GET_REQ;

/*+
/* FDDI_MIB_Get Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;		

    /*+
    /* SMT GROUP 
    -*/
    FW_STATION_ID   smt_station_id;			
    FW_ULONG	    smt_op_version_id;
    FW_ULONG	    smt_hi_version_id;
    FW_ULONG	    smt_lo_version_id;
    FW_ULONG	    smt_mac_ct;				
    FW_ULONG	    smt_non_master_ct;			
    FW_ULONG	    smt_master_ct;				
    FW_ULONG	    smt_paths_available;			
    FW_ULONG	    smt_config_capabilities;		
    FW_ULONG	    smt_config_policy;		
    FW_ULONG	    smt_connection_policy;		
    FW_ULONG	    smt_t_notify;	
    FW_ULONG	    smt_status_reporting;
    FW_ULONG	    smt_ecm_state;	
    FW_ULONG	    smt_cf_state;	
    FW_ULONG	    smt_hold_state;		
    FW_ULONG	    smt_remote_disconnect_flag;
    FW_ULONG	    smt_station_action;			

    /*+
    /* MAC GROUP 
    -*/
    FW_ULONG	    mac_frame_status_capabilities;	
    FW_ULONG	    mac_t_max_greatest_lower_bound;
    FW_ULONG	    mac_tvx_greatest_lower_bound;
    FW_ULONG	    mac_paths_available;
    FW_ULONG	    mac_current_path;
    FW_LAN_ADDR	    mac_upstream_nbr;			
    FW_LAN_ADDR	    mac_old_upstream_nbr;		
    FW_ULONG	    mac_dup_addr_test;			
    FW_ULONG	    mac_paths_requested;
    FW_ULONG	    mac_downstream_port_type;
    FW_LAN_ADDR     mac_smt_address;			
    FW_ULONG	    mac_t_req;				
    FW_ULONG	    mac_t_neg;
    FW_ULONG        mac_t_max;				
    FW_ULONG	    mac_tvx_value;			
    FW_ULONG	    mac_t_min;				
    FW_ULONG	    mac_current_frame_status;
    /*              mac_frame_cts 			*/
    /* 		    mac_error_cts 			*/
    /* 		    mac_lost_cts 			*/
    FW_ULONG	    mac_frame_error_threshold;		
    FW_ULONG	    mac_frame_error_ratio;		
    FW_ULONG	    mac_rmt_state;
    FW_ULONG	    mac_da_flag;
    FW_ULONG	    mac_una_da_flag;			
    FW_ULONG	    mac_frame_condition;
    FW_ULONG	    mac_chip_set;			
    FW_ULONG	    mac_action;				

    /* PATH GROUP => Does not need to be implemented */
    /*+
    /* Port objects
    -*/
    FW_ULONG	    port_pc_type[FW_PHY_K_MAX];			
    FW_ULONG	    port_pc_neighbor[FW_PHY_K_MAX];			
    FW_ULONG	    port_connection_policies[FW_PHY_K_MAX];
    FW_ULONG	    port_remote_mac_indicated[FW_PHY_K_MAX];
    FW_ULONG	    port_ce_state[FW_PHY_K_MAX];
    FW_ULONG	    port_paths_requested[FW_PHY_K_MAX];
    FW_ULONG	    port_mac_placement[FW_PHY_K_MAX];
    FW_ULONG	    port_available_paths[FW_PHY_K_MAX];
    FW_ULONG	    port_mac_loop_time[FW_PHY_K_MAX];
    FW_ULONG	    port_tb_max[FW_PHY_K_MAX];
    FW_ULONG	    port_bs_flag[FW_PHY_K_MAX];
    /*		    port_lct_fail_cts[FW_PHY_K_MAX];	*/
    FW_ULONG	    port_ler_estimate[FW_PHY_K_MAX];			    
    /*		    port_lem_reject_cts[FW_PHY_K_MAX];	*/
    /*	    	    port_lem_cts[FW_PHY_K_MAX];		*/
    FW_ULONG	    port_ler_cutoff[FW_PHY_K_MAX];			    
    FW_ULONG	    port_ler_alarm[FW_PHY_K_MAX];			    
    FW_ULONG	    port_connect_state[FW_PHY_K_MAX];
    FW_ULONG	    port_pcm_state[FW_PHY_K_MAX];
    FW_ULONG	    port_pc_withhold[FW_PHY_K_MAX];
    FW_ULONG	    port_ler_condition[FW_PHY_K_MAX];			    
    FW_ULONG	    port_chip_set[FW_PHY_K_MAX];			    
    FW_ULONG	    port_action[FW_PHY_K_MAX];			    

    /*+
    /* Attachment objects
    -*/
    FW_ULONG	    attachment_class;
    FW_ULONG	    attachment_ob_present;
    FW_ULONG	    attachment_imax_expiration;
    FW_ULONG	    attachment_inserted_status;
    FW_ULONG	    attachment_insert_policy;

    /* CHIP SET GROUP => Does not need to be implemented */

    } FW_CMD_FDDI_MIB_GET_RSP;

/*+
/* FDDI_MIB_TEST Request 
-*/
typedef struct {
    FW_ULONG  cmd_type;
    } FW_CMD_FDDI_MIB_TEST_REQ;

/*+
/* FDDI_MIB_TEST Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    } FW_CMD_FDDI_MIB_TEST_RSP;

/*+
/* DEC_Ext_MIB_Get Request 
-*/
typedef struct {
    FW_ULONG  cmd_type;
    } FW_CMD_DEC_EXT_MIB_GET_REQ;

/*+
/* DEC_Ext_MIB_Get (efddi group only) Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;		
    /*+
    /* SMT GROUP 
    -*/
    FW_ULONG	    esmt_station_type;
    /*+
    /* MAC GROUP 
    -*/
    FW_ULONG	    emac_link_state;			
    FW_ULONG	    emac_ring_purger_state;
    FW_ULONG        emac_ring_purger_enable;
    FW_ULONG	    emac_frame_strip_mode;
    FW_ULONG	    emac_ring_error_reason;
    FW_ULONG	    emac_up_nbr_dup_addr_flag;
    FW_ULONG	    emac_restricted_token_timeout;
    /*+
    /* PORT GROUP 
    -*/
    FW_ULONG	    eport_pmd_type[FW_PHY_K_MAX];
    FW_ULONG	    eport_phy_state[FW_PHY_K_MAX];
    FW_ULONG	    eport_reject_reason[FW_PHY_K_MAX];
    /*+
    /* Full Duplex objects
    -*/
    FW_ULONG	    fdx_enable;
    FW_ULONG	    fdx_operational;
    FW_ULONG        fdx_state;

    } FW_CMD_DEC_EXT_MIB_GET_RSP;

/*+
/* DEC_MIB_TEST Request 
-*/
typedef struct {
    FW_ULONG  cmd_type;
    } FW_CMD_DEC_EXT_MIB_TEST_REQ;

/*+
/* DEC_MIB_TEST Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    } FW_CMD_DEC_EXT_MIB_TEST_RSP;

/*+
/* Counter get response block
-*/
typedef struct {
    FW_CNTR	    traces_rcvd;			/* Station */
    FW_CNTR	    frame_cnt;				/* Link */
    FW_CNTR	    error_cnt;
    FW_CNTR	    lost_cnt;
    FW_CNTR	    octets_rcvd;
    FW_CNTR	    octets_sent;
    FW_CNTR	    pdus_rcvd;
    FW_CNTR	    pdus_sent;
    FW_CNTR	    mcast_octets_rcvd;
    FW_CNTR	    mcast_octets_sent;
    FW_CNTR	    mcast_pdus_rcvd;
    FW_CNTR	    mcast_pdus_sent;
    FW_CNTR	    xmt_underruns;
    FW_CNTR	    xmt_failures;
    FW_CNTR         block_check_errors;
    FW_CNTR	    frame_status_errors;
    FW_CNTR         pdu_length_errors;
    FW_CNTR	    rcv_overruns;
    FW_CNTR	    user_buff_unavailable;
    FW_CNTR	    inits_initiated;
    FW_CNTR	    inits_rcvd;
    FW_CNTR	    beacons_initiated;
    FW_CNTR	    dup_addrs;
    FW_CNTR	    dup_tokens;
    FW_CNTR	    purge_errors;
    FW_CNTR	    fci_strip_errors;
    FW_CNTR	    traces_initiated;
    FW_CNTR	    directed_beacons_rcvd;
    FW_CNTR         emac_frame_alignment_errors;
    FW_CNTR	    ebuff_errors[FW_PHY_K_MAX];		/* Phy */
    FW_CNTR	    lct_rejects[FW_PHY_K_MAX];
    FW_CNTR	    lem_rejects[FW_PHY_K_MAX];
    FW_CNTR	    link_errors[FW_PHY_K_MAX];
    FW_CNTR	    connections[FW_PHY_K_MAX];
    FW_CNTR         kongs;				/* Private counters */
    } FW_CNTR_BLK;

/*+
/* Counters_Get Request 
-*/
typedef struct {
    FW_ULONG  cmd_type;
    } FW_CMD_CNTRS_GET_REQ;

/*+
/* Counters_Get Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    FW_CNTR	    time_since_reset;			
    FW_CNTR_BLK	    cntrs;			    
    } FW_CMD_CNTRS_GET_RSP;

/*+
* Counters_Set Request 
-*/
typedef struct {
    FW_ULONG	cmd_type;
    FW_CNTR_BLK	cntrs;			    
    } FW_CMD_CNTRS_SET_REQ;

/*+
/* Counters_Set Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    } FW_CMD_CNTRS_SET_RSP;

/*+
/* CNTRS_TEST Request 
-*/
typedef struct {
    FW_ULONG  cmd_type;
    } FW_CMD_CNTRS_TEST_REQ;

/*+
/* CNTRS_TEST Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    } FW_CMD_CNTRS_TEST_RSP;

/*+
/* Error_Log_Clear Request 
-*/
typedef struct {
    FW_ULONG  cmd_type;
    } FW_CMD_ERROR_LOG_CLEAR_REQ;

/*+
/* Error_Log_Clear Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    } FW_CMD_ERROR_LOG_CLEAR_RSP;

/*+
/* Error_Log_Get Request 
-*/
#define FW_LOG_ENTRY_K_INDEX_MIN    0	    /* Minimum index for entry */

typedef struct {
    FW_ULONG  cmd_type;
    FW_ULONG  entry_index;
    } FW_CMD_ERROR_LOG_GET_REQ;

/*+
/* Error_Log_Get Response 
-*/
#define FW_K_LOG_FW_SIZE	111	    /* Max number of fw longwords */
#define FW_K_LOG_DIAG_SIZE      6	    /* Max number of diag longwords */

typedef struct {
    struct {
        FW_ULONG    fru_imp_mask;
        FW_ULONG    test_id;
        FW_ULONG    reserved[FW_K_LOG_DIAG_SIZE];
        } diag;
    FW_ULONG	    fw[FW_K_LOG_FW_SIZE]; /* Refer to port_dev.h for details */
    } FW_LOG_ENTRY;

typedef struct {
    FW_RSP_HEADER   header;    
    FW_ULONG	    event_status;
    FW_ULONG	    caller_id;
    FW_ULONG	    timestamp_l;
    FW_ULONG	    timestamp_h;
    FW_ULONG	    write_count;
    FW_LOG_ENTRY    entry_info;
    } FW_CMD_ERROR_LOG_GET_RSP;

/*+
/* Define error log related constants and types 
-*/
#define FW_LOG_EVENT_STATUS_K_VALID	0            /* Valid Event Status */
#define FW_LOG_EVENT_STATUS_K_INVALID	1            /* Invalid Event Status */
#define FW_LOG_CALLER_ID_K_RESERVED     0            /* Reserved Caller Id */
#define FW_LOG_CALLER_ID_K_FW           1            /* Firmware Caller Id */
#define FW_LOG_CALLER_ID_K_DIAG         2            /* Diagnostics Caller Id */

/*+
/* TEST_UNSOL Request 
-*/
typedef struct {
    FW_ULONG	cmd_type;
    } FW_CMD_UNSOL_TEST_REQ;

/*+
/* TEST_UNSOL Response 
-*/
typedef struct {
    FW_RSP_HEADER   header;    
    } FW_CMD_TEST_UNSOL_RSP;

/*+
/* Define the types of unsolicited events 
-*/
#define FW_UNSOL_TYPE_K_EVENT_REPORT	    0
#define FW_UNSOL_TYPE_K_EVENT_CNTRS	    1

/*+
/* Define entity codes for unsolicited event reports 
-*/
#define FW_UNSOL_ENTITY_K_STATION	    0
#define FW_UNSOL_ENTITY_K_LINK		    1
#define FW_UNSOL_ENTITY_K_PHY		    2

/*+
/* Define station unsolicited event and argument codes 
-*/
#define FW_UNSOL_STAT_K_SELFTEST_FAILED     0                  
#define FW_UNSOL_STAT_K_PC_TRACE_RCVD       1
#define FW_UNSOL_STAT_K_CONFIG_CHANGE       2

#define FW_UNSOL_STAT_ARG_K_REASON	    0

/*+
/* Define link unsolicited event and argument codes 
-*/
#define FW_UNSOL_LINK_K_XMT_UNDERRUN        0
#define FW_UNSOL_LINK_K_XMT_FAILURE         1
#define FW_UNSOL_LINK_K_BLOCK_CHECK_ERR	    2
#define FW_UNSOL_LINK_K_FRAME_STAT_ERR      3
#define FW_UNSOL_LINK_K_LENGTH_ERR	    4
#define FW_UNSOL_LINK_K_BAD_IND_DST	    5
#define FW_UNSOL_LINK_K_BAD_MCAST_DST	    6
#define FW_UNSOL_LINK_K_RCV_OVERRUN         7
#define FW_UNSOL_LINK_K_NO_LINK_BUFFER      8
#define FW_UNSOL_LINK_K_NO_USER_BUFFER      9
#define FW_UNSOL_LINK_K_INIT_INITD	    10
#define FW_UNSOL_LINK_K_RING_INIT_RCVD      11
#define FW_UNSOL_LINK_K_BEACON_INITD	    12
#define FW_UNSOL_LINK_K_DUP_ADDR_FOUND	    13
#define FW_UNSOL_LINK_K_DUP_TOKEN_FOUND	    14
#define FW_UNSOL_LINK_K_RING_PURGE_ERR	    15
#define FW_UNSOL_LINK_K_FCI_STRIP_ERR	    16
#define FW_UNSOL_LINK_K_PC_TRACE_INITD	    17
#define FW_UNSOL_LINK_K_BEACON_RCVD         18

#define FW_UNSOL_LINK_ARG_K_REASON	    0
#define FW_UNSOL_LINK_ARG_K_DL_HEADER       1
#define FW_UNSOL_LINK_ARG_K_SOURCE          2
#define FW_UNSOL_LINK_ARG_K_UP_NBR          3

/*+
/* Define PHY event and argument codes 
-*/
#define FW_UNSOL_PHY_K_LEM_REJECT	    0
#define FW_UNSOL_PHY_K_EBUFF_ERR	    1
#define FW_UNSOL_PHY_K_LCT_REJECT           2

#define FW_UNSOL_PHY_ARG_K_DIRECTION        0

/*+
/* Define End-of-Argument list code 
-*/
#define FW_UNSOL_ARG_K_EOL                  0xFF

/*+
/* Event Header 
-*/
typedef struct {
    FW_ULONG	entity;
    FW_ULONG	entity_index;
    FW_ULONG	event_code;
    } FW_EVENT_HEADER;

/*+
/* Reason Argument Description 
-*/
typedef struct {
    FW_ULONG    arg_code;
    FW_ULONG	reason_code;
    } FW_UNSOL_ARG_REASON_DESC;

/*+
/* Data Link Header Argument Desc 
-*/
typedef struct {
    FW_ULONG    arg_code;
    struct {
        FW_ULONG    fc;
        FW_LAN_ADDR dst_addr;
        FW_LAN_ADDR src_addr;
        } mac;
    struct {
        FW_ULONG    dsap;
        FW_ULONG    ssap;
        FW_ULONG    control;
        FW_ULONG    pid_1;
        FW_ULONG    pid_2;
        } llc;
    } FW_UNSOL_ARG_DL_HEADER_DESC;

/*+
/* Net Address Argument Description 
-*/
typedef struct {
    FW_ULONG    arg_code;
    FW_LAN_ADDR	net_addr;
    } FW_UNSOL_ARG_NET_ADDR_DESC;

/*+
/* Direction Argument Description 
-*/
typedef struct {
    FW_ULONG    arg_code;
    FW_ULONG	direction;
    } FW_UNSOL_ARG_DIRECTION_DESC;

/*+
/* Unsol Report: Cntr or Event 
-*/
typedef struct {
    FW_ULONG	event_type;
    union {
	struct {
	    FW_CNTR	    time_since_reset;			
	    FW_CNTR_BLK	    cntrs;
	    } cntrs;
        struct {
            FW_EVENT_HEADER event_header;
            FW_ULONG        event_data[75];
            } report;
	} info;
    } FW_UNSOL_REPORT;


/*+
/* Define format of Consumer Block (resident in host memory) 
-*/
typedef struct {
    FW_ULONG	xmt_rcv_data;
    FW_ULONG	reserved_1;
    FW_ULONG	smt_host;
    FW_ULONG	reserved_2;
    FW_ULONG	unsol;
    FW_ULONG	reserved_3;
    FW_ULONG	cmd_rsp;
    FW_ULONG	reserved_4;
    FW_ULONG	cmd_req;
    } FF_CONSUMER_BLOCK;

#define FW_CONS_M_RCV_INDEX	0X000000FF
#define FW_CONS_M_XMT_INDEX     0X00FF0000
#define FW_CONS_V_RCV_INDEX     0
#define FW_CONS_V_XMT_INDEX     16

/*+
/* Offsets into consumer block 
-*/
#define FW_CONS_BLK_K_XMT_RCV	0X00
#define FW_CONS_BLK_K_SMT_HOST	0X08
#define FW_CONS_BLK_K_UNSOL	0X10
#define FW_CONS_BLK_K_CMD_RSP	0X18
#define FW_CONS_BLK_K_CMD_REQ	0X20

/*+
/* Offsets into descriptor block 
-*/
#define FW_DESCR_BLK_K_RCV_DATA	0X0000
#define FW_DESCR_BLK_K_XMT_DATA	0X0800
#define FW_DESCR_BLK_K_SMT_HOST 0X1000
#define FW_DESCR_BLK_K_UNSOL	0X1200
#define FW_DESCR_BLK_K_CMD_RSP	0X1280
#define FW_DESCR_BLK_K_CMD_REQ	0X1300	

/*+
/* Define format of a rcv descr (Rcv Data, Cmd Rsp, Unsolicited, SMT Host) 
-*/
typedef struct rcv_descr {
    union {
      FW_ULONG	word0;
      struct {
        unsigned pa_hi : 16;
        unsigned seg_cnt : 4;
        unsigned mbz : 3;
        unsigned seg_len_hi : 6;
        unsigned seg_len_lo : 2;
        unsigned sob : 1;
      } rbits;
    } lword0;
    FW_ULONG	buff_lo;
} FW_RCV_DESCR;

#define	FW_RCV_DESCR_M_SOP	0X80000000
#define FW_RCV_DESCR_M_MBZ	0X60000000 
#define FW_RCV_DESCR_M_SEG_LEN	0X1F800000
#define FW_RCV_DESCR_M_SEG_CNT	0X000F0000
#define FW_RCV_DESCR_M_BUFF_HI	0X0000FFFF

#define	FW_RCV_DESCR_V_SOP	31
#define FW_RCV_DESCR_V_MBZ	29
#define FW_RCV_DESCR_V_SEG_LEN	23
#define FW_RCV_DESCR_V_SEG_CNT	16
#define FW_RCV_DESCR_V_BUFF_HI	0

/*+
/* Define the format of a transmit descriptor (Xmt Data, Cmd Req) 
-*/
typedef struct {
    union {
      FW_ULONG word0;
      struct {
        unsigned pa_hi : 16;
        unsigned seg_len : 13;
        unsigned mbz : 1;
        unsigned eob : 1;
        unsigned sob : 1;
      } word1;
    } lword0;
    FW_ULONG	buff_lo;
} FW_XMT_DESCR;

#define	FW_XMT_DESCR_M_SOP	0X80000000
#define FW_XMT_DESCR_M_EOP	0X40000000
#define FW_XMT_DESCR_M_MBZ	0X20000000 
#define FW_XMT_DESCR_M_SEG_LEN	0X1FFF0000
#define FW_XMT_DESCR_M_BUFF_HI	0X0000FFFF

#define	FW_XMT_DESCR_V_SOP	31
#define	FW_XMT_DESCR_V_EOP	30
#define FW_XMT_DESCR_V_MBZ	29
#define FW_XMT_DESCR_V_SEG_LEN	16
#define FW_XMT_DESCR_V_BUFF_HI	0

/*+
/* Define format of the Descriptor Block (resident in host memory) 
-*/
#define FW_RCV_DATA_ENTRIES	256
#define FW_XMT_DATA_ENTRIES	256
#define FW_SMT_HOST_ENTRIES	64
#define FW_UNSOL_ENTRIES	16
#define FW_CMD_RSP_ENTRIES	16
#define FW_CMD_REQ_ENTRIES	16


typedef struct {
    FW_RCV_DESCR  rcv_data[FW_RCV_DATA_ENTRIES];
    FW_XMT_DESCR  xmt_data[FW_XMT_DATA_ENTRIES];
    FW_RCV_DESCR  smt_host[FW_SMT_HOST_ENTRIES];
    FW_RCV_DESCR  unsol[FW_UNSOL_ENTRIES];
    FW_RCV_DESCR  cmd_rsp[FW_CMD_RSP_ENTRIES];
    FW_XMT_DESCR  cmd_req[FW_CMD_REQ_ENTRIES];
} FF_DESCR_BLOCK;


/*+
/* Host 


/*+
/* Application copy of Completion Indicees. The completion indicees are
/* stored in registers on the adapter, and are READ ONLY by the adapter.
/* When the application updates the completion indicees in the adapter,
/* the host copies are updated also. The most frequent use of the
/* completion index is to detect when a queue needs servicing. This
/* allows the application to compare the consumer indeicees to host
/* copies of the completion indicees, saving costly CSR accesses.
-*/
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
} FF_PRODUCER_INDEX;

/*+
/* FDDI received Frame definition
-*/
typedef struct fddi_rcv_frame {
  unsigned char			prh[3];		/* byte alignement      */
  struct NDL$FDDI_MAC_HEADER	mac;		/* FDDI mac header	*/
  struct NDL$IEEE_LLC_HEADER	llc;		/* LLC header		*/
  unsigned short                len;		/* FDDI frame length    */
  unsigned char			data[4500];	/* FDDI rcv data 	*/
} FDDI_RCV_FRAME;

/*+
/* FDDI header 
-*/
typedef struct xfw_header {
   unsigned char   prh[3];		/* Byte alignment */
} XFW_HEADER;

/*Min and max packet size for an ethernet*/
#define EZ$K_MIN_PKT_SIZE 60		/*Minimum packet size*/
#define EZ$K_MAX_PKT_SIZE NDL$K_MAX_PACK_LENGTH /*Maximum packet size*/

/*General program constants                                                 */
#define EZ$K_XMT_TIMEOUT (10*1000)	/*Ten seconds			    */
#define EZ$K_XMT_EMPTY_TIMEOUT 10 	/*  10 * empty_timeout_sleep	    */
#define EZ$K_XMT_EMPTY_TIMEOUT_SLEEP 100 /* 100 milliseconds		    */
#define EZ$K_OPEN_TIMEOUT_SECS 1	/*1 seconds			    */
#define EZ$K_DESC_BYTES 16              /*Number of bytes in a descriptor   */
#define EZ$K_SETUP_FRAME_SIZE 128       /*Number of bytes in a setup frame  */
#define EZ$K_HASH_SIZE 72		/*Number of bytes in a hash frame  */
#define EZ$K_HASH_ADDR_OFFSET 64	/*Place in hash filter for address  */
#define EZ$K_HASH_BROADCAST_INDEX 255	/*Hash index for the broadcast address*/
#define EZ$K_XMT_RCV 0                  /*Transmit and receive              */
#define EZ$K_XMT_ONLY 1                 /*Transmit only                     */
#define EZ$K_RCV_ONLY 2                 /*Receive only                      */
#define EZ$K_TX_HOLD_CNT 0x01		/*Hold before porcessing next tx    */
#define EZ$K_XMT_MOD 512		/*Environment variable defaults     */
#define EZ$K_XMT_REM 0
#define EZ$K_XMT_MAX_SIZE EZ$K_MAX_PKT_SIZE
#define EZ$K_XMT_INT_MSG 1
#define EZ$K_XMT_MSG_POST 1
#define EZ$K_RCV_MOD 512
#define EZ$K_RCV_REM 0
#define EZ$K_MSG_BUF_SIZE EZ$K_MAX_PKT_SIZE
#define EZ$K_MSG_MOD 0
#define EZ$K_MSG_REM 0

#define EZ$K_XMTS_BEFORE_MEMZONE 50	/*Number of xmt descriptors before  */
					/*using memzone			    */
#define EZ$K_RCVS_BEFORE_MEMZONE 50	/*Number of RCV descriptors before  */
					/*using memzone			    */
#define EZ$K_XMT_BUF_CNT 0x10		/*Buf cnt must be a power of 2*/
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

/*Transmit info structure and constants*/
/*This structure will be allocated in parallel*/
/*with the transmit descriptors. This structure*/
/*is used to pass information about the packet*/
/*to the transmit process ez_tx*/ 
#define EZ$K_TX_ASYNC 0			/*Transmits will be asyncronous*/
#define EZ$K_TX_SYNC 1			/*Transmits will be syncronous*/

volatile struct EZ_TX_INFO {
    struct TIMERQ *tx_timer; 		/*TX timer*/
    int sync_flag;			/*Synchronous flag*/
    struct EZ_TX_INFO *chain;		/*Pointer the next TX info*/
};

/*Callback message received*/
#define EZ_MAX_RCV_MSGS 10
#define EZ_CALLBACK_TIMEOUT 100		/*100 * 1/100 of a second*/
#define EZ_CALLBACK_WAIT 10		/*10 milliseconds = 1/100 second*/

/*+
/* PDQ queues entry definitions
-*/
#define		FW$K_MAX_START_TIMEOUT	5000
#define		FW$K_MAX_RESET_TIMEOUT  5000
#define		FW$K_MAX_TYPE1_ENTRIES	16
#define		FW$K_MAX_TYPE2_ENTRIES	256
#define		FW$K_MAX_SMT_ENTRIES	64
#define		FW$K_MAX_RCV_DESCR	4
#define		FW$K_MAX_RSP_DESCR	2
#define		FW$K_TYPE1_QUEUES	1
#define		FW$K_TYPE2_QUEUES	2
#define		FW$K_MSG_PREFIX_SIZE	3

/*+
/*
-*/
struct FF_MSG {
    struct FF_MSG *flink;
    struct FF_MSG *blink;
    unsigned char *msg;
    int           size;
    };

