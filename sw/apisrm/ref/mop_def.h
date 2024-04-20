/********************************************************************************************************************************/
/* Created 17-JAN-1991 14:10:26 by VAX SDL V3.2-12     Source: 17-JAN-1991 14:10:04 FILY_USER:[DENISCO.COBRA.CP.SRC]MOP_DEF.SDL;6 */
/********************************************************************************************************************************/
 
/*** MODULE $MOPDEFS IDENT 1.1 ***/
/*                                                                          */
/*                                                                          */
/* file:        MOP_DEFS.SDL                                                */
/*                                                                          */
/* Copyright (C) 1988 by                                                    */
/* Digital Equipment Corporation, Maynard, Massachusetts.                   */
/* All rights reserved.                                                     */
/*                                                                          */
/* This software is furnished under a license and may be used and copied    */
/* only  in  accordance  of  the  terms  of  such  license  and with the    */
/* inclusion of the above copyright notice. This software or  any  other    */
/* copies thereof may not be provided or otherwise made available to any    */
/* other person.  No title to and  ownership of the  software is  hereby    */
/* transferred.                                                             */
/*                                                                          */
/* The information in this software is  subject to change without notice    */
/* and  should  not  be  construed  as a commitment by digital equipment    */
/* corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by digital.                  */
/*                                                                          */
/*                                                                          */
/* Facility:    KA6xx Firmware                                              */
/*                                                                          */
/* Abstract:    This file describes all the data structures and constants   */
/*              that the KA6xx firmware uses and that are specific to the   */
/*              MOP support.                                                */
/*                                                                          */
/* Author:      Kevin Peterson                                              */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	jds	16-Aug-1991	Added XNA def; added processor ID defs.	    */
/*									    */
/*	jad	25-Jul-1990	Stolen for Cobra.                           */
/*                                                                          */
/* 	kp      08-SEP-1989     Added remote console definitions.           */
/*                                                                          */
/*	kp      03-MAY-1989     Minor changes.                              */
/*                                                                          */
/*	kp      23-Sep-1988     Initial version.                            */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/* MOP General constants.                                                   */
/*                                                                          */
#define MOP$K_MAX_FILE_NAME 16          /* Max mop file name size.          */
/*                                                                          */
/* Maintenance Operations Protocol (MOP) versions.                          */
/*                                                                          */
#define MOP$K_VERSION_3 1               /* All MOP versions before 4.       */
#define MOP$K_VERSION_4 4               /* MOP version 4.                   */
#define MOP$K_MAX_MSG_VER_3 1500        /* Max message size v3 (without headers). */
#define MOP$K_MAX_MSG_VER_4 1492        /* Max message size v4 (without headers). */
#define MOP$K_MAX_MSG 1500              /* Max message size both (without headers). */
#define MOP$K_MAX_DATA_AVAIL_V3_INC_LEN 1496 /* Max space for data available  */
#define MOP$K_MAX_DATA_AVAIL_V3_NO_LEN 1500 /* in a message                 */
#define MOP$K_MAX_DATA_AVAIL_V4 1492
#define MOP$K_MAX_DATA_AVAIL 1492
/*                                                                          */
/* MOP network device types.                                                */
/*                                                                          */
#define MOP$K_DEQNA 5                   /* DEQNA.                           */
#define MOP$K_DELQA 37                  /* DELQA or DESQA.                  */
#define MOP$K_LANCE 61                  /* LANCE on the KA640.              */
#define MOP$K_XGEC 73                   /* SGEC on KA660,KA670,KA680, TGEC on COBRA */
#define MOP$K_DEMNA 66			/* DEMNA aka XNA		    */

#define MOP$K_DEFTA 160		/* fddi/Turbochannel */
#define MOP$K_DEFAA 161		/* fddi/fbus */
#define MOP$K_DEFEA 162		/* fddi/eisa */
#define MOP$K_DEFIA 163		/* fddi/isa */
#define MOP$K_DEFNA 164		/* fddi/sbus */
/*                                                                          */
/*Datalink type                                                             */
/*                                                                          */
#define MOP$K_DL_CSMA_CD 1              /*	..IEEE 802.3.               */
#define MOP$K_DL_DDCMP 2
#define MOP$K_DL_LAPB 3
#define MOP$K_DL_HDLC 4
#define MOP$K_DL_FDDI 5
#define MOP$K_DL_TOKEN 11
/*                                                                          */
/* Ethernet protocol types.                                                 */
/*                                                                          */
#define MOP$K_PROT_DUMPLOAD 24577       /* 60-01 MOP dump/load.             */
#define MOP$K_PROT_CONSOLE 24578        /* 60-02 MOP remote console.        */
#define MOP$K_PROT_LOOPBACK 36864       /* 90-00 MOP loopback assistance.   */
/*                                                                          */
/* Program type constants.                                                  */
/*                                                                          */
#define MOP$K_SECONDARY_LOADER 0
#define MOP$K_TERTIARY_LOADER 1
#define MOP$K_SYSTEM_IMAGE 2
#define MOP$K_MANAGEMENT_IMAGE 3
#define MOP$K_CMIP_SCRIPT_FILE 4
/*                                                                          */
/* Software ID constants                                                    */
/*                                                                          */
#define MOP$K_SW_ID_OS -1
#define MOP$K_SW_ID_MAINT -2
/*									    */
/* Processor ID constants						    */
/*									    */
#define MOP$K_SYSTEM_PROCESSOR 0
#define MOP$K_COMM_PROCESSOR 1
/*                                                                          */
/* MOP message codes.                                                       */
/*                                                                          */
#define MOP$K_MSG_MEM_LOAD_W_XFER 0     /*   0 = Memory load with transfer address. */
#define MOP$K_MSG_DUMP_COMPLETE 1       /*   1 = Dump complete.             */
#define MOP$K_MSG_MEM_LOAD 2            /*   2 = Memory load.               */
#define MOP$K_MSG_VOLUNTEER 3           /*   3 = Assistance volunteer.      */
#define MOP$K_MSG_REQ_MEM_DUMP 4        /*   4 = Request memory dump.       */
#define MOP$K_MSG_REQUEST_ID 5          /*   5 = Request identification.    */
#define MOP$K_MSG_BOOT 6                /*   6 = Boot.                      */
#define MOP$K_MSG_SYSTEM_ID 7           /*   7 = System identification.     */
#define MOP$K_MSG_REQ_PROGRAM 8         /*   8 = Request program load.      */
#define MOP$K_MSG_REQ_COUNTERS 9        /*   9 = Request counters.          */
#define MOP$K_MSG_REQ_MEM_LOAD 10       /*  10 = Request memory load with load number. */
#define MOP$K_MSG_COUNTERS 11           /*  11 = Counters.                  */
#define MOP$K_MSG_REQ_DUMP_SERVICE 12   /*  12 = Request dump service.      */
#define MOP$K_MSG_RESERVE_CONSOLE 13    /*  13 = Reserve console.           */
#define MOP$K_MSG_MEM_DUMP_DATA 14      /*  14 = Memory dump data.          */
#define MOP$K_MSG_RELEASE_CONSOLE 15    /*  15 = Release console.           */
/*  16                                                                      */
#define MOP$K_MSG_CON_CMD_N_POLL 17     /*  17 = Console command and poll.  */
/*  18                                                                      */
#define MOP$K_MSG_CON_RSP_N_ACK 19      /*  19 = Console response and acknowledge. */
#define MOP$K_MSG_PARAM_LOAD_W_XFER 20  /*  20 = Parameter load with transfer address. */
/*  21                                                                      */
/*  22                                                                      */
/*  23                                                                      */
#define MOP$K_MSG_LOOP_DATA 24          /*  24 = Loop data.                 */
/*  25                                                                      */
#define MOP$K_MSG_LOOPED_DATA 26        /*  26 = Looped data.               */
/*  27                                                                      */
#define MOP$K_MSG_ENET_REPLY 28         /*  28 = Ethernet loopback "reply". */
#define MOP$K_MSG_ENET_LOOPBACK 29      /*  29 = Ethernet loopback "forward data". */
#define MOP$K_MSG_LOOP_REPLY 1          /* Reply (Ethernet "looped data").  */
#define MOP$K_MSG_LOOP_FORWARD 2        /* Forward data (Ethernet "loop data"). */
#define MOP$K_MSG_MAX_CODE 31           /* Largest MOP message code.        */
/*                                                                          */
/* MOP SYSTEM_ID INFOrmation data.                                          */
/*	Zero elements indicate supply elsewhere.                            */
/*	* indicates a required info field.                                  */
/*	+ indicates a field supplied only when remote console is enabled.   */
/*                                                                          */
#define MOP$K_UNSOLICITED_REC_NUM 0     /*Receipt number for unsolicted ID  */
#define MOP$K_RES_CON_REC_NUM 1         /*Receipt number for unsolicted ID  */
#define MOP$M_FUNC_LOOP 1               /* 0 Loopback.                      */
#define MOP$M_FUNC_DUMP 2               /* 1 Memory dump.                   */
#define MOP$M_FUNC_PRIMARY 4            /* 2 Primary loader.                */
#define MOP$M_FUNC_MULTI_BLOCK 8        /* 3 Multi-block loader.            */
#define MOP$M_FUNC_BOOT 16              /* 4 Bootstrap.                     */
#define MOP$M_FUNC_CONSOLE 32           /* 5 Remote console carrier.        */
#define MOP$M_FUNC_COUNTERS 64          /* 6 Datalink counters.             */
#define MOP$M_FUNC_RESERVED 128         /* 7 Remote console reserved.       */
#define MOP$K_SYSID_TYP_VERSION 1       /*   1* MOP Version.                */
#define MOP$K_SYSID_LEN_VERSION 3       /*	..Get value field from..    */
#define MOP$K_SYSID_VAL_VERSION 0       /*	..MOP$K_VERSION3/4.         */
#define MOP$K_SYSID_TYP_FUNCTIONS 2     /*   2* Functions (bitmap).         */
#define MOP$K_SYSID_LEN_FUNCTIONS 2     /*	..Loop(0),multi-block load(3), */
#define MOP$K_SYSID_VAL_FUNCTIONS 89    /*	..boot(4), and counters(6). */
#define MOP$K_SYSID_TYP_CON_USER 3      /*   3+ Console user.               */
#define MOP$K_SYSID_LEN_CON_USER 6      /*	..Address of system currently.. */
#define MOP$K_SYSID_VAL_CON_USER 0      /*	..using the console.        */
#define MOP$K_SYSID_TYP_CON_TIMER 4     /*   4+ Console reservation timer.  */
#define MOP$K_SYSID_LEN_CON_TIMER 2     /*	..Time-out value in seconds.. */
#define MOP$K_SYSID_VAL_CON_TIMER 60    /*	..for non-active console user. */
#define MOP$K_SYSID_TYP_CMD_SIZE 5      /*   5+ Console command size.       */
#define MOP$K_SYSID_LEN_CMD_SIZE 2      /*	..Minimum size of..         */
#define MOP$K_SYSID_VAL_CMD_SIZE 1      /*	..console command buffer.   */
#define MOP$K_SYSID_TYP_RSP_SIZE 6      /*   6+ Console response size.      */
#define MOP$K_SYSID_LEN_RSP_SIZE 2      /*	..Minimum size of..         */
#define MOP$K_SYSID_VAL_RSP_SIZE 1      /*	..console response buffer.  */
#define MOP$K_SYSID_TYP_E_HW_ADDR 7     /*   7* Data link HW address.       */
#define MOP$K_SYSID_LEN_E_HW_ADDR 6     /*	..Get from..                */
#define MOP$K_SYSID_VAL_E_HW_ADDR 0     /*	..the boot driver.          */
#define MOP$K_SYSID_TYP_NODE_ID 9       /*   9  Node ID.                    */
#define MOP$K_SYSID_LEN_NODE_ID 6       /*	..Non-primitive state..     */
#define MOP$K_SYSID_VAL_NODE_ID 0       /*	..Node ID.                  */
#define MOP$K_SYSID_TYP_SYS_TIME 10     /*  10  System time.                */
#define MOP$K_SYSID_LEN_SYS_TIME 8      /*	..Represented in..          */
#define MOP$K_SYSID_VAL_SYS_TIME 0      /*	..Binary Absolute Time.     */
#define MOP$K_SYSID_TYP_NODE_NAME 11    /*  11  Node name.                  */
#define MOP$K_SYSID_LEN_NODE_NAME 0     /*	..Non-primitive state..     */
#define MOP$K_SYSID_VAL_NODE_NAME 0     /*	..Name Service Full Name.   */
#define MOP$K_STATID_TYP_NODE_NAME 13    /*  13  Station ID */
#define MOP$K_STATID_LEN_NODE_NAME 8     /*	..Non-primitive state..     */
#define MOP$K_STATID_VAL_NODE_NAME 0     /*	..Name Service Full Name.   */
#define MOP$K_SYSID_TYP_COM_DVICE 100   /* 100* Communication device.       */
#define MOP$K_SYSID_LEN_COM_DVICE 1     /*	..Get from..                */
#define MOP$K_SYSID_VAL_COM_DVICE 0     /* 	..VMB device list.          */
#define MOP$K_SYSID_TYP_SW_ID 200       /* 200  Software Id.                */
#define MOP$K_SYSID_LEN_SW_ID 0         /*	..C-128 form..              */
#define MOP$K_SYSID_VAL_SW_ID 0         /* 	..same as for BOOT message. */
#define MOP$K_SYSID_TYP_PROCESSOR 300   /* 300  System Processor.           */
#define MOP$K_SYSID_LEN_PROCESSOR 1     /*	..Reference MOP spec..      */
#define MOP$K_SYSID_VAL_PROCESSOR 0     /* 	..Appendix A.               */
#define MOP$K_SYSID_TYP_DATA_LINK 400   /* 400  Data link buffer size.      */
#define MOP$K_SYSID_LEN_DATA_LINK 1
#define MOP$K_SYSID_TYP_BUF_SIZE 401    /* 401  Data link buffer size.      */
#define MOP$K_SYSID_LEN_BUF_SIZE 2      /*	..Use length of..           */
#define MOP$K_SYSID_VAL_BUF_SIZE 1      /*	..Receive buffer.           */
/*                                                                          */
/* Parameter constants                                                      */
/*                                                                          */
#define MOP$K_PARM_END_MARK 0           /* 0                                */
#define MOP$K_PARM_NODE_NAME 1          /* 1                                */
#define MOP$K_PARM_NODE_ADDR 2          /* 2                                */
#define MOP$K_PARM_HOST_NAME 3          /* 3                                */
#define MOP$K_PARM_HOST_ADDR 4          /* 4                                */
#define MOP$K_PARM_V3_TOD 5             /* 5                                */
#define MOP$K_PARM_V4_TOD 6             /* 6                                */
/*                                                                          */
/* Point to point LOOP_DATA message.                                        */
/*                                                                          */
struct MOP$PT_PT_LOOP_DATA {
    U_INT_8 MOP$B_LP_FUNCTION;
    U_INT_8 MOP$B_LP_RECEIPT;
    U_INT_8 MOP$B_LP_DATA;
    } ;
/*                                                                          */
/* LOOP_DATA (Ethernet format) message.                                     */
/*                                                                          */
struct MOP$LOOP_DATA {
    U_INT_16 MOP$W_LP_FUNCTION;
    U_INT_8 MOP$B_LP_ADDRESS [6];
    U_INT_8 MOP$B_LP_INFO;
    } ;
/*                                                                          */
/* LOOPED_DATA (Ethernet format) message.                                   */
/*                                                                          */
struct MOP$LOOPED_DATA {
    U_INT_16 MOP$W_LP_FUNCTION;
    U_INT_16 MOP$W_LP_RECEIPT;
    U_INT_8 MOP$B_LP_INFO;
    } ;
/*                                                                          */
/* Loop messages forward and reply                                          */
/*                                                                          */
struct MOP$LOOP_FWD {
    U_INT_16 MOP$W_LF_SKIP_COUNT; 	/*Skip count                        */
    struct MOP$LOOP_DATA MOP$R_LF;      /*The rest of the packet	    */
    } ;
struct MOP$LOOP_REPLY {
    U_INT_16 MOP$W_LR_SKIP_COUNT;   	/*Skip count                        */
    struct MOP$LOOPED_DATA MOP$R_LR;    /*The rest of the packet	    */
    } ;
/*                                                                          */
/* REQ_PROGRAM message.                                                     */
/*                                                                          */
struct MOP$REQ_PROGRAM {
    U_INT_8 MOP$B_RQP_CODE;
    U_INT_8 MOP$B_RQP_DEVICE;
    U_INT_8 MOP$B_RQP_FORMAT;
    U_INT_8 MOP$B_RQP_PROGRAM;
    U_INT_8 MOP$B_RQP_SW_ID_LEN;
    } ;
/*                                                                          */
/* MEM_LOAD_w_XFER message.                                                 */
/*                                                                          */
struct MOP$MEM_LOAD_W_XFER {
    U_INT_8 MOP$B_MLX_CODE;
    U_INT_8 MOP$B_MLX_LOAD_NUM;
    U_INT_32 MOP$L_MLX_LOAD_ADDRESS;
    } ;
/*                                                                          */
/* MEM_LOAD message.                                                        */
/*                                                                          */
struct MOP$MEM_LOAD {
    U_INT_8 MOP$B_MLD_CODE;
    U_INT_8 MOP$B_MLD_LOAD_NUM;
    U_INT_32 MOP$L_MLD_LOAD_ADDRESS;
    } ;
/*                                                                          */
/* VOLUNTEER message.                                                       */
/*                                                                          */
struct MOP$VOLUNTEER {
    U_INT_8 MOP$B_VOL_CODE;
    U_INT_8 MOP$B_VOL_SIZE;
    } ;
/*                                                                          */
/* REQ MEM load message.                                                    */
/*                                                                          */
struct MOP$REQ_MEM_LOAD {
    U_INT_8 MOP$B_RQM_CODE;
    U_INT_8 MOP$B_RQM_LOAD_NUM;
    U_INT_8 MOP$B_RQM_RSVD;
    } ;
/*                                                                          */
/* PARAM_LOAD_w_XFER message.                                               */
/*                                                                          */
struct MOP$PARAM_LOAD_W_XFER {
    U_INT_8 MOP$B_PLX_CODE;
    U_INT_8 MOP$B_PLX_LOAD_NUM;
    } ;
/*                                                                          */
/* REQUEST_ID message.                                                      */
/*                                                                          */
struct MOP$REQUEST_ID {
    U_INT_8 MOP$B_RID_CODE;
    U_INT_8 MOP$B_RID_RESERVED;
    U_INT_16 MOP$W_RID_RECEIPT_NUM;
    } ;
/*                                                                          */
/* SYSTEM_ID message.                                                       */
/*                                                                          */
struct MOP$SYSTEM_ID {
    U_INT_8 MOP$B_SID_CODE;
    U_INT_8 MOP$B_SID_RESERVED;
    U_INT_16 MOP$W_SID_RECEIPT_NUM;
    } ;
/*                                                                          */
/* REQ_COUNTERS message.                                                    */
/*                                                                          */
struct MOP$REQ_COUNTERS {
    U_INT_8 MOP$B_RCT_CODE;
    U_INT_16 MOP$W_RCT_RECEIPT_NUM;
    } ;
/*                                                                          */
/* COUNTERS message.                                                        */
/*                                                                          */
struct MOP$COUNTERS {
    U_INT_8 MOP$B_CNT_CODE;
    U_INT_16 MOP$W_CNT_RECEIPT_NUM;
    } ;
/*                                                                          */
/* RESERVE_CONSOLE message.                                                 */
/*                                                                          */
struct MOP$RESERVE_CONSOLE {
    U_INT_8 MOP$B_RSV_CODE;
    U_INT_8 MOP$B_RSV_VERICATION [6];
    } ;
/*                                                                          */
/* RELEASE_CONSOLE message.                                                 */
/*                                                                          */
struct MOP$RELEASE_CONSOLE {
    U_INT_8 MOP$B_RLS_CODE;
    } ;
/*                                                                          */
/* CON_CMD_N_POLL message.                                                  */
/*                                                                          */
struct MOP$CON_CMD_N_POLL {
    U_INT_8 MOP$B_CCP_CODE;
    U_INT_8 MOP$B_CCP_FLAGS;
    U_INT_8 MOP$B_CCP_DATA;
    } ;
/*                                                                          */
/* CON_RSP_N_ACK message.                                                   */
/*                                                                          */
struct MOP$CON_RSP_N_ACK {
    U_INT_8 MOP$B_CRA_CODE;
    U_INT_8 MOP$B_CRA_FLAGS;
    U_INT_8 MOP$B_CRA_DATA;
    } ;
#define MOP$M_FLAG_NUMBER 1             /* 0 Message sequence number, 0 or 1. */
#define MOP$M_FLAG_BREAK 2              /* 1 Command Break sent preceding data. */
#define MOP$M_FLAG_CMD_LOST 2           /* 1 Command data lost.             */
#define MOP$M_FLAG_RSP_LOST 4           /* 2 Response data lost.            */
#define MOP$V_FLAG_NUMBER 0             /* 0 Message sequence number, 0 or 1. */
#define MOP$V_FLAG_BREAK 1              /* 1 Command Break sent preceding data. */
#define MOP$V_FLAG_CMD_LOST 1           /* 1 Command data lost.             */
#define MOP$V_FLAG_RSP_LOST 2           /* 2 Response data lost.            */
/*                                                                          */
/* BOOT message.                                                            */
/*                                                                          */
struct MOP$BOOT {
    U_INT_8 MOP$B_BOOT_CODE;
    U_INT_32 MOP$Q_BOOT_VERIFICATION [2];
    U_INT_8 MOP$B_BOOT_PROCESSOR;
    U_INT_8 MOP$B_BOOT_CONTROL;
    U_INT_8 MOP$B_BOOT_DEVICE_ID_LEN;
    U_INT_8 MOP$B_BOOT_DEVICE_ID;
    } ;
/* $MOPDEFS                                                                 */
