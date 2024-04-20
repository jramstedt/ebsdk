/*
 * Copyright (C) 1997 by
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
 * Abstract:	KGPSA port driver literals.  
 *
 *              These literals are mostly used under debug conditions.
 *
 * Author:	Dennis Mazur
 *
 * Modifications:
 *
 *	dm	25-Aug-97	Initial entry.
 */
#if !defined(KGPSA_LIT_HDR)
#define KGPSA_LIT_HDR

#include "cp$src:kgpsa_def.h"

/*
 * Literals
 */
char *latt_type[] = {"0","LINK UP","LINK DOWN"};
char *nstate_text[] = {"UNUSED", "LOGOUT", "LOGIN", "ADAPTER" };
char *prli_state_text[] = {"UNUSED", "PRLI_OK", "PRLI_NG" };




#if KGPSA_DEBUG
char *iocbtype[]  = {"BAD","CMD","RSP"};
char *lip_type[]  = {"An L_PORT is Initializing",
		     "An L_PORT has detected an error at its receiver",
		     "An L_PORT has sent a Reset LIP to a port other than this",
		     "An L_PORT has sent a Reset LIP to this port",
		     "other" };


char * mode_text[] = {"INTERRUPT","POLLED","STOP","START","ASSIGN",
                      "LOOP_INTERNAL","LOOP_EXTERN","NOLOOP",
		      "OPEN","CLOSE","READY"};


char *devnstate_text[] = {"0","READY","INITIALIZING","INITIAL_DISCOVER",
                  "RE_DISCOVER", "PROBE_NEEDED", "PROBE_IN_PROGRESS" };

char *Discstate_text[] = {"0","DISCOVERY_NEEDED",
                              "DISCOVERY_IN_PROGRESS",
			      "DISCOVERY_DONE" };

char *rsp_nstate_text[] = {"RSP_INACTIVE","RSP_WAITING","RSP_READY",
                          "RSP_TIMEOUT","RSP_ABORT" };

char *nprobe_state_text[] = {"INVALID/UNINITED", "PROBE_IN_PROGRESS",
                              "PROBE_DONE" };

char *iocb_status_text[] = {
                              "IOSTAT_SUCCESS",        /*   0x0  */
                              "IOSTAT_FCP_RSP_ERROR",  /*   0x1  */
                              "IOSTAT_REMOTE_STOP",    /*   0x2  */
                              "IOSTAT_LOCAL_REJECT",   /*   0x3  */
                              "IOSTAT_NPORT_RJT",      /*   0x4  */
                              "IOSTAT_FABRIC_RJT",     /*   0x5  */
                              "IOSTAT_NPORT_BSY",      /*   0x6  */
                              "IOSTAT_FABRIC_BSY",     /*   0x7  */
                              "IOSTAT_INTERMED_RSP",   /*   0x8  */
                              "IOSTAT_LS_RJT",         /*   0x9  */
                              "IOSTAT_BA_RJT"          /*   0xA  */
};

char *ffcsrtbl[] = {
    "HA - Host Attention",
    "CA - Chip Attention/Firefly Attention",
    "HS - BIU2 Host Status",
    "HC - BIU2 Host Control"
};

char *mbx_txt[] =
{"MBX_SHUTDOWN",
 "MBX_LOAD_SM",
 "MBX_READ_NV",
 "MBX_WRITE_NV",
 "MBX_RUN_BIU_DIAG",
 "MBX_INIT_LINK",
 "MBX_DOWN_LINK",
 "MBX_CONFIG_LINK",
 "MBX_PART_SLIM",
 "MBX_CONFIG_RING",
 "MBX_RESET_RING",
 "MBX_READ_CONFIG",
 "MBX_READ_RCONFIG",
 "MBX_READ_SPARM",
 "MBX_READ_STATUS",
 "MBX_READ_RPI",
 "MBX_READ_XRI",
 "MBX_READ_REV",
 "MBX_READ_LNK_STAT",
 "MBX_REG_LOGIN",
 "MBX_UNREG_LOGIN",
 "MBX_READ_LA",
 "MBX_CLEAR_LA",
 "MBX_DUMP_MEMORY",
 "MBX_DUMP_CONTEXT",
 "MBX_RUN_DIAGS",
 "MBX_RESTART",
 "MBX_MAX_CMDS	"
};

char *elstype_text[] = {
"0-NULL PAYLOAD",     /* 0x00 */
"ELS_LS_RJT",    /* 0x01 */
"ELS_ACC",       /* 0x02 */
"ELS_PLOGI",     /* 0x03 */
"ELS_FLOGI",     /* 0x04 */
"ELS_LOGO",      /* 0x05 */
"ELS_ABTX",      /* 0x06 */
"ELS_RCS",       /* 0x07 */
"ELS_RES",       /* 0x08 */
"ELS_RSS",       /* 0x09 */
"ELS_RSI",       /* 0x0A */
"ELS_ESTS",      /* 0x0B */
"ELS_ESTC",      /* 0x0C */
"ELS_ADVC",      /* 0x0D */
"ELS_RTV",       /* 0x0E */
"ELS_RLS",       /* 0x0F */
"ELS_ECHO",      /* 0x10 */
"ELS_TEST",      /* 0x11 */
"ELS_RRQ",       /* 0x12 */
"13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F",
"ELS_PRLI",      /* 0x20 */
"ELS_PRLO",      /* 0x21 */
          "22","23","24","25","26","27","28","29","2A","2B","2C","2D","2E","2F",
"30","31","32","33","34","35","36","37","38","39","3A","3B","3C","3D","3E","3F",
"40","41","42","43","44","45","46","47","48","49","4A","4B","4C","4D","4E","4F",
"ELS_PDISC"      /* 0x50 */
};

char * wait_text[] = { "DONT_WAIT", "WAIT" };

char * iocbtype_text[] = {
			   "NO_0x00",
                           "CMD_RCV_SEQUENCE_CX",  
                           "CMD_XMIT_SEQUENCE_CR",
                           "CMD_XMIT_SEQUENCE_CX",
                           "CMD_XMIT_BCAST_CN",
                           "CMD_XMIT_BCAST_CX",
                           "CMD_QUE_RING_BUF_CN",
                           "CMD_QUE_XRI_BUF_CX",
                           "CMD_IOCB_CONTINUE_CN",
                           "CMD_RET_XRI_BUF_CX",
                           "CMD_ELS_REQUEST_CR",
                           "CMD_ELS_REQUEST_CX",
                           "NO_0x0C",
                           "CMD_RCV_ELS_REQ_CX",
                           "CMD_ABORT_XRI_CN",
                           "CMD_ABORT_XRI_CX",
                           "CMD_CLOSE_XRI_CR",
                           "CMD_CLOSE_XRI_CX",
                           "CMD_CREATE_XRI_CR",
                           "CMD_CREATE_XRI_CX",
                           "CMD_GET_RPI_CN",
                           "CMD_XMIT_ELS_RSP_CX",
                           "CMD_GET_RPI_CR",
                           "CMD_XRI_ABORTED_CX",
                           "CMD_FCP_IWRITE_CR",
                           "CMD_FCP_IWRITE_CX",
                           "CMD_FCP_IREAD_CR",
                           "CMD_FCP_IREAD_CX",
                           "CMD_FCP_ICMND_CR",
                           "CMD_FCP_ICMND_CX",
                           "NO_0x1E",
                           "CMD_FCP_TSEND_CX",
                           "NO_0x20",
                           "CMD_FCP_TRECEIVE_CX",
                           "NO_0x22",
                           "IOCB_FCP_TRSP_CX",
                         };

char * iocb_state_text[] = { "UNKNOWN", 
                             "LOGO_SENT", 
                             "LOGO_ACC_SENT",
                             "LOGO_LS_RJT_SENT", 
                             "PLOGI_SENT", 
                             "PLOGI_ACC_SENT", 
                             "PLOGI_LS_RJT_SENT",
                             "PRLI_ACC_SENT",
                             "PRLI_LS_RJT_SENT",
                             "PRLO_ACC_SENT",
			     "PRLO_LS_RJT_SENT",
                             "PDISC_SENT", 
			     "PDISC_ACC_SENT",
                             "PDISC_LS_RJT_SENT",
                             "LS_RJT_SENT",
                             "IOCB_FCP_DONE",
                             "IOCB_INCARNERR",
                             "RSCN_ACC_SENT"
                          };

#else

int null_strings[] = 
		{ "", "", "", "", "", "", "", "", "", "",
		  "", "", "", "", "", "", "", "", "", "",
		  "", "", "", "", "", "", "", "", "", "",
		  "", "", "", "", "", "", "", "", "", "",
		  "", "", "", "", "", "", "", "", "", "",
		  "", "", "", "", "", "", "", "", "", "",
		  "", "", "", "", "", "", "", "", "", "",
		  "", "", "", "", "", "", "", "", "", "",
		  "", "", "", "", "", "", "", "", "", "",
		  "", "", "", "", "", "", "", "", "", ""};

#define mbx_txt         null_strings
#define iocbtype        null_strings
#define lip_type        null_strings
#define mode_text       null_strings
#define devnstate_text  null_strings
#define Discstate_text  null_strings
#define rsp_nstate_text null_strings
#define ffcsrtbl        null_strings
#define elstype_text    null_strings
#define wait_text       null_strings
#define iocbtype_text   null_strings
#define iocb_state_text null_strings

#endif




#endif
