/*
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
 * Abstract:	CAM (SCSI Common Access Method transport) Definitions
 *
 * Author:	Judith Gold
 *
 * Creation Date: 20-May-1994
 *
 * Modifications:
 *
 */

/* NOTE: simport.h defines the CCB header.  All extensions to the CCB are */
/* defined here. */

/* number of bytes of largest extension to the CAM header.  This is currently */
/* the execute_scsi_io CCB with associated SIM private data area. */

#define SIM_PRIV		56
#define LARGEST_CAM_CB		(22 * 4) + SIM_PRIV

/* CAM flags */

/* byte 0 */
#define CFLAG$M_CDB_POINTER		0x1       /* CDB is a pointer */
#define CFLAG$M_TQAE			0x2  	  /* tagged q action enable */
#define CFLAG$M_LINK_CDB                0x4       /* linked CDB */
#define CFLAG$M_DCOC			0x8	  /* disable callbk on complete*/
#define CFLAG$M_SG			0x10	  /* scatter-gather */
#define CFLAG$M_DIS_AUTOS		0x20	  /* disable autosense */
#define CFLAG$M_DIR			0xC0	  /* direction of data xfer */

/* direction defs */
#define DIR_IN				0x01
#define DIR_OUT				0x02
#define DIR_NO_XFER			0x03	  /* no data transfer */

/* byte 1 */

/* NOTE: this next bit is defined as reserved in the CAM spec.  It is used */
/* in the console SIMPort driver to signal command complete.  */
#define CFLAG$M_CMD_DONE		0x100	  /* command is done */

#define CFLAG$M_ENG_SYNC		0x200     /* engine synchronize */
#define CFLAG$M_SIMQ_FZ_DIS		0x400	  /* SIM queue freeze disable */
#define CFLAG$M_SIMQ_FZ			0x800	  /* SIM queue freeze */
#define CFLAG$M_SIMQ_PRTY		0x1000	  /* SIM queue priority */
#define CFLAG$M_DST			0x2000	  /* disable sync. xfers */
#define CFLAG$M_IST			0x4000	  /* initiate sync. xfers */
#define CFLAG$M_DD			0x8000	  /* disable disconnect */

/* byte 2 */
/* bits 0-6 0 = VA, 1 = PA */
#define CFLAG$M_SGLP			0x10000   /* sg list pointers */
#define CFLAG$M_COC			0x20000	  /* callback on completion */
#define CFLAG$M_NEXT_CCB		0x40000	  
#define CFLAG$M_MSG_BUFF		0x80000	  /* message buffer */
#define CFLAG$M_SNS_BUFF		0x100000  /* sense buffer */
#define CFLAG$M_SGLD			0x200000  /* sg list/data */
#define CFLAG$M_CDBPTR			0x400000  /* CDB pointer */
#define CFLAG$M_SGLST_ENG		0x800000  /* sg list,  1-eng, 0-host */

/* byte 3 - target mode specific flags */
/* phase cognizant */
#define CFLAG$M_DASR			0x1000000 /* disable autosave/restore*/
#define CFLAG$M_DAD			0x2000000 /* disable autodisconnect */
#define CFLAG$M_TCCBA			0x4000000 /* target CCB available */
#define CFLAG$M_PCM			0x8000000 /* phase-cognizant mode */
#define CFLAG$M_MBV			0x20000000/* message buffer valid */
#define CFLAG$M_SBV			0x40000000/* status buffer valid */
#define CFLAG$M_DBV			0x80000000/* data buffer valid */
/* host target */
#define CFLAG$M_NOT_HT			0x8000000 /* 0 = host target */
#define CFLAG$M_TIO			0x20000000/* terminate I/O */
#define CFLAG$M_DM			0x40000000/* disconnects mandatory */
#define CFLAG$M_SS			0x80000000/* send status */


/* XPT function codes */
#define XPT_NOP			0
#define XPT_EXECUTE_SCSI_IO 	1
#define XPT_GET_DEVICE_TYPE	2
#define XPT_PATH_INQUIRY	3
#define XPT_RELEASE_SIM_QUEUE	4
#define XPT_SET_ASYNC_CALLBK	5
#define XPT_SET_DEVICE_TYPE	6
#define XPT_SCAN_SCSI_BUS	7
#define XPT_ABORT_SCSI_CMD	0x10
#define XPT_RESET_SCSI_BUS	0x11
#define XPT_RESET_SCSI_DEVICE	0x12
#define XPT_TERMINATE_IO_PROC   0x13
#define XPT_ENGINE_INQUIRY	0x20
#define XPT_EXECUTE_ENG_REQ	0x21
/* target mode */
#define XPT_ENABLE_LUN		0x30
#define XPT_EXECUTE_TARGET_IO	0x31
#define XPT_ACCEPT_TARGET_IO	0x32
#define XPT_CONTINUE_TARGET_IO  0x33
#define XPT_IMMEDIATE_NOTIFY	0x34
#define XPT_NOTIFY_ACKNOWLEDGE  0x35

/* CAM status bytes */
#define REQ_IN_PROG		0X00  	/* request in progress */
#define REQ_DONE_NO_ERR		0X01    /* request completed w. no error */
#define REQ_ABORT_BY_HOST	0X02    /* request aborted by host */
#define CANT_ABORT_REQ		0X03    /* unable to abort request */
#define REQ_DONE_ERR		0X04    /* request completed with error */
#define CAM_BUSY		0X05	
#define INVAL_REQ		0X06    /* invalid request */
#define INVAL_PATH_ID		0X07    /* invalid path ID */
#define DEV_NOT_INST		0X08	/* SCSI device not installed */
#define CANT_STOP_IO		0x09	/* unable to terminate I/O process */
#define TGT_SEL_TO		0x0a    /* target selection timeout */
#define CMD_TO			0x0b	/* command timeout */
#define MSG_REJ_REC		0x0d	/* message reject received */
#define SCSI_BUS_RESET	        0x0e	/* SCSI bus reset sent/received */
#define UNCOR_PE		0x0f	/* uncorrectable parity error */
#define REQ_SENSE_FAIL		0x10	/* autosense request sense cmd failed*/
#define NO_HBA			0x11	/* no host bus adapter detected */
#define DATA_OV_UN		0x12	/* data overrun/underrun */
#define UNEXPECT_BUS_FREE	0x13	/* unexpected bus free */
#define TGT_PHS_SEQ_FAIL	0x14	/* target bus phase sequence failure */
#define CCB_LEN_BAD		0x15	/* CCB length inadequate */
#define NO_REQ_CAP		0x16	/* can't provide requested capability*/
#define DEV_RESET_SENT		0x17	/* bus device reset sent */
#define STOP_IO			0x18	/* terminate I/O process */
#define HBA_ERROR		0x19	/* unrecoverable host bus adapter err */
/* target mode only status */
#define RES_UNAV		0x34	/* resource unavailable */
#define UNACK_BY_HOST		0x35	/* unacknowledged event by host */
#define MSG_REC			0x36	/* message received */
#define INVAL_CDB		0x37
#define INVAL_LUN		0x38
#define INVAL_TGT_ID		0x39	/* invalid target ID */
#define FUNC_NO_IMPLE		0x3a	/* function not implemented */
#define NO_NEXUS		0x3b	/* nexus not established */
#define INVAL_INIT_ID		0x3c	/* invalid initiator ID */
#define CDB_REC			0x3d	/* SCSI CDB received */
#define LUN_ALR_EN		0x3e	/* LUN already enabled */
#define SCSI_BUS_BUSY   	x03f	

#define CAMS_SIMQ_FROZ		0x40	/* SIM queue is frozen, add 0x40 to 
					 * above codes */
#define CAMS_SENSE		0x80	/* autosense is valid, add 0x80 to 
					 * above codes */
#define ERR_MASK		0X3F	/* error codes minus sense,frozq info */

/* all CAM commands require the CCB header.  Some also include additional */
/* fields.  These structs are listed here.   */

/* path inquiry */
struct PATH_INQUIRY
 {
	unsigned char version;
	unsigned char capabil;
	unsigned char tgt_mode;
	unsigned char misc;
	unsigned short eng_count;
	unsigned char vendor[14];
	unsigned int  priv_data_size;
	unsigned int async_event;
	unsigned char high_path_id;
	unsigned char init_id;
	unsigned char reserved[2];
	unsigned int sim_ven_id[4];
	unsigned int hba_ven_id[4];
	unsigned int osd_usage;
 } ;

/* path inquiry bitfields */

/* version */
#define PI$M_VER_LO	0xff	/* prior to rev 1.7 */
#define PI$M_VER_17	0x100  /* rev 1.7 */
#define PI$M_VER_HI	0xfe00 /* later versions */
/* scsi capabilities */
#define PI$M_SOFT_RESET 0x01	/* soft reset */
#define PI$M_TAGGED_Q	0x02	/* tagged queueing */
#define PI$M_LINKED_CMD 0x08	/* linked commands */
#define PI$M_SYNC_XFER  0x10	/* synchronous transfers */
#define PI$M_WIDE_16	0x20	/* wide bus 16 */
#define PI$M_WIDE_32	0x40	/* wide bus 32 */
#define PI$M_MOD_DATA_P 0x80	/* modify data pointers */
/* target mode support*/
#define PI$M_GP_7	0x04	/* group 7 commands */
#define PI$M_GP_6	0x08	/* group 6 commands */
#define PI$M_TERM_IO	0x10	/* terminate I/O process */
#define PI$M_TGT_DISCON 0x20	/* target mode disconnects */
#define PI$M_PHAS_COG   0x40	/* phase cognizant mode */
#define PI$M_HOST_TGT   0x80	/* host target mode */
/* miscellaneous field */
#define PI$M_NO_XPT_INQ 0x20	/* inquiry data not kept by XPT */
#define PI$M_REMOV_NOT_SCAN 0x40 /* removables not included in scan */
#define PI$M_SCAN_H_TO_L 0x80	/* scanned high to low */
/* asynchronous event capabilities */
#define PI$M_UNSOL_BUS_RESET 0x01 /* unsolicited SCSI bus reset */
#define PI$M_UNSOL_RESEL 0x02	/* unsolicited reselection */
#define PI$M_SCSI_AEN	0x08	
#define PI$M_RESET_TGT  0x10	/* sent bus device reset to target */
#define PI$M_SIM_MOD_REG 0x20	/* SIM module registered */
#define PI$M_SIM_MOD_DREG 0x40	/* SIM module de-registered */
#define PI$M_NEW_DEV	0x80	/* new devices found during rescan */


/* set async callback */
struct ASYNC_CB
 {
	unsigned int async_event_ena;
	unsigned int async_cb_ptr;
	unsigned int drvr_buff_ptr;
	unsigned char drvr_buff_size;
	unsigned char fill1[3];
 } ;

/* abort SCSI command */
struct ABORT_SCSI
 {
	unsigned int abort_ccb_ptr;
 } ;


/* terminate I/O process request */
struct TERM_IO
 {
	unsigned int abort_ccb_ptr;
 } ;

/* execute SCSI I/O */
struct EXECUTE_SCSI_IO
 {
	unsigned int drvr_ptr;
	unsigned int next_ccb_ptr;
	unsigned int req_map_info;
	unsigned int cb_on_comp;
	unsigned int sg_buf_ptr;
	unsigned int xfer_len;
	unsigned int sense_buf_ptr;
	unsigned char sense_buf_len;
	unsigned char cdb_len;
	unsigned short num_sg_entries;
	unsigned int vu_field;
	unsigned char scsi_status;
	unsigned char sense_res_len;
	unsigned char reserved[2];
	unsigned int res_len;
	unsigned char cdb[12];
	unsigned int timeout;
	unsigned int msg_buf_ptr;
	unsigned short msg_buf_len;
	unsigned short vu_flags;
	unsigned char tag_q_action;
	unsigned char tag_id;
	unsigned char init_id;
	unsigned char reserved1;
 } ;

/* TARGET MODE */

/* enable lun */

struct ENABLE_LUN
 {
	unsigned short group6_len;
	unsigned short group7_len;
	unsigned int imm_not_ptr;
	unsigned int imm_not_count;
	unsigned int target_ccb_ptr;
	unsigned short target_ccb_count;
	unsigned char fill1[2];
 } ;

/* execute target io */
struct EXECUTE_TARGET_IO
 {
	unsigned int drvr_ptr;
	unsigned int next_ccb_ptr;
	unsigned int req_map_info;
	unsigned int cb_on_comp;
	unsigned int sg_buf_ptr;
	unsigned int xfer_len;
	unsigned int sense_buf_ptr;
	unsigned char sense_buf_len;
	unsigned char cdb_len;
	unsigned short num_sg_entries;
	unsigned int vu_field;
	unsigned char scsi_status;
	unsigned char sense_res_len;
	unsigned char reserved[2];
	unsigned int res_len;
	unsigned char cdb[12];
	unsigned int timeout;
	unsigned int msg_buf_ptr;
	unsigned short msg_buf_len;
	unsigned short vu_flags;
	unsigned char tag_q_action;
	unsigned char tag_id;
	unsigned char init_id;
	unsigned char reserved1;
 } ;


