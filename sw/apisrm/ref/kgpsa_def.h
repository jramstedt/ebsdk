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
 * Abstract:	KGPSA  (Emulex LightPulse)  definitions
 *
 * Author:	Dennis Mazur
 *
 * Modifications:
 *
 *	dm	28-May-1996	Initial entry.
 *  
 */


/* fc_hw.h 1/18/96
 */

#if !defined(KGPSA_DEF_HDR)
#define KGPSA_DEF_HDR

#define RAWHIDE_DEBUG 0
#define TURBO_DEBUG 0
#define KGPSA_DEBUG (MKLOAD || SBLOAD || RAWHIDE_DEBUG || TURBO_DEBUG)


/*
 * Symbol and structure definitions local to this driver
 */

#define LOOP_NOT_SUPPORTED   1
#define FC_MAX_NPORTS 256

#define PROBE_SET            1
#define PROBE_GET            2
#define PROBE_DISPLAY_BOTH   3
#define PROBE_DISPLAY_DONE   4
#define PROBE_DISPLAY_INPROG 5
#define ALL_DID 0

#define RSP_INACTIVE 0
#define RSP_WAITING  1
#define RSP_READY    2
#define RSP_TIMEOUT  3
#define RSP_ABORT    4

#define	MS_500	    500
#define	SECOND_1    1000
#define SECONDS_2   2000
#define SECONDS_5   5000
#define SECONDS_10  10000
#define SECONDS_30  30000
#define SECONDS_60  60000
#define MINUTE_1    60000

#define FC_NO_SWAP_BYTES 0
#define FC_SWAP_BYTES    1

#define NO_VERBOSE 0
#define VERBOSE    1

volatile struct kgpsa_sb {
    include_struct_scsi_sb
#if MODULAR || STARTSHUT
    include_struct_ext_scsi_sb
#endif
    include_struct_ext_scsi3_sb
    int		valid;			/* invalid before known to exist */
    int		did;			/* N_port device id */
    int		rpi;			/* rpi for logged in nodes */
    int		nstate;			/* Current state of node */
    int		retries;		/* Command retries left for this sb */
    int		LINKUPeventTag;         /* Link event tag when rpi assigned */
    int		RSCNeventTag;           /* RSCN event tag when rpi assigned */
#     define	NLP_UNUSED	0		/* NL_PORT is not in loop */
#     define	NLP_LOGOUT	1		/* NL_PORT is not logged in */
#     define	NLP_LOGIN	2		/* NL_PORT is logged in */
#     define	NLP_ADAPTER	3		/* NL_PORT is initiating adapter */
#     define    NLP_UNKNOWN     4               /* NL_PORT state is unknown */
    int		nstate_iotag;	       /* iotag at nstate change */
    int		prli_state;
#     define    PRLI_OK		1
#     define    PRLI_NG		2
    SERV_PARM	serv_parm;		/* service parameters */
    int		scsi_target;		/* set if nport does target mode */
    int		permanent;		/* any wwids found off this sb?    */
    } ;




typedef struct iocbhdr  {
	struct QUEUE iocb_q;
	int          bde1_bufptr;
	int          bde2_bufptr;
	int          bde3_bufptr;
        void         (*callback)(void *arg, struct iocbhdr *iocbhdr);
        void         *callback_arg;
	int	     LINKUPeventTag;
	int          RSCNeventTag;
	int	     iocb_state;
#	   define      UNKNOWN		 0
#          define      LOGO_SENT         1  
#          define      LOGO_ACC_SENT     2
#	   define      LOGO_LS_RJT_SENT  3
#	   define      PLOGI_SENT        4
#	   define      PLOGI_ACC_SENT	 5
#          define      PLOGI_LS_RJT_SENT 6
#          define      PRLI_ACC_SENT     7
#	   define      PRLI_LS_RJT_SENT  8
#          define      PRLO_ACC_SENT     9
#	   define      PRLO_LS_RJT_SENT  10
#	   define      PDISC_SENT        11
#          define      PDISC_ACC_SENT    12
#	   define      PDISC_LS_RJT_SENT 13
#          define      LS_RJT_SENT       14
#          define      IOCB_FCP_DONE     15
#	   define      IOCB_INCARNERR    16
#          define      RSCN_ACC_SENT     17
#	   define      IOCB_ABORTED      99
	int	     is_a_probe;
	int	     retries;
	int	     did;
	int	     rpi;
	int	     wait_for_completion;
	int          iocb_q_state;
#          define      IOCB_NOSTATE   0x00
#          define      IOCB_PENDING   0x01    /* waiting to be sent to adapter */
#          define      IOCB_ACTIVE    0x02    /* active in adapter */
#          define      IOCB_DONE      0x03    /* complete */
#          define      IOCB_ABORTED   0x04    /* aborted  */
	int	     iotag;
	struct SEMAPHORE rsp;
	struct TIMERQ    xrsp_t;
	IOCB         cmd_iocb;
	int	     cmd_slim_idx;       /* informational; for debug */
	IOCB	     cont_iocb;
	int	     cont_slim_idx;      /* informational; for debug */
	IOCB	     rsp_iocb;
	int	     rsp_slim_idx;       /* informational; for debug */
	struct kgpsa_sb *sb;     /* can I obsolete this field? */
        struct kgpsa_pb *pb;
} IOCB_HDR;


struct command {
    unsigned char entry_type;
    unsigned char entry_count;
    unsigned char entry_unused;
    unsigned char entry_flags;
    unsigned long handle;
    unsigned char lun;
    unsigned char target;
    unsigned short cdb_length;
    unsigned short control_flags;
    unsigned short reserved;
    unsigned short timeout;
    unsigned short data_count;
    unsigned char cdb[12];
    unsigned long data_address;
    unsigned long data_length;
    unsigned long unused[6];
    volatile unsigned long rsp_state;
    } ;




struct iocbq {
      struct QUEUE q;
      IOCB  iocb;
};

struct srb {
      struct kgpsa_sb      *sb;
      int                  lun;
      unsigned char        *cmd;
      int                  cmd_len;
      unsigned char        *dat_out;
      int                  dat_out_len;
      unsigned char        *dat_in;
      int                  dat_in_len;
      unsigned char        *sts;
      unsigned char        *sense_dat_in;
      int                  *sense_dat_in_len;
    } ;


#define KGPSA_NVRAM_REGION 3
#define KGPSA_REGION3_BASE 0x73000
#define KGPSA_NVRAM_VERSION 1
#define COMPAQ_VENDOR_ID 0x0e11

/* This structure must be padded to the longword granularity */
struct kgpsa_nvram_region {
      unsigned short vendor_id;
      unsigned short version;
      unsigned long checksum;
      unsigned char topology;
      unsigned char alpa;
      unsigned char fill1;       
      unsigned char fill2;
      unsigned long rsvd1;
      unsigned long rsvd2;
      unsigned long rsvd3;
    } ;



#endif


