/*
 * kfpsa_def.h
 *
 * Copyright (C) 1996 by
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
 * Abstract:	KFPSA Definitions
 *
 * Author:	Nigel Croxon
 *
 * Modifications:
 *
 *	njc	02-Jan-1996	Initial entry.
 */
 
typedef enum adapter_state {Unknown, Uninitialized, Disabled, Enabled} 
   ADAPTER_STATE;

typedef enum pu_poll_status {POLL_OK, POLL_WAITING, POLL_NOPATH, POLL_TIMEOUT
                            } PU_POLL_STATUS;

#define HWRPB$_PAGESIZE	    0x2000
#define DSSI_NODES 8
#define SB_POOL_SZ ( sizeof( struct mscp_sb ) * DSSI_NODES )
#define SB_PTR_POOL_SZ ( sizeof( struct mscp_sb *) * DSSI_NODES )

/* DSSI status definition.  Note that this defines more files of STATUS_TYPE */

enum DSSI_STATUS_TYPE {
  PKTSZVIOL      = 1,		/* packet size violation Rz*/
  INVBUFNAME     = 2,		/* invalid buffer name Rz*/
  BUFLENVIOL     = 3 ,		/* buffer length violation Rz*/
  SSR		 = 4,		/* DSSI Status Returned in bit 15:8 */
  UNRCMD         = 16 ,		/* unrecognized command Rz*/
  INVDEST        = 17 ,		/* invalid destination xport Rz*/
  INSUFRES       = 22 ,		/* insufficient RESEQ resources Rz*/
  UNRPKT         = 32 ,		/* Unrecognized received packet Rz*/
  INVXPORT       = 33 ,		/* Invalid received Xport address Rz*/
  INVDL          = 34 ,		/* Invalid destination datalink address Rz*/
  DISCVC         = 36 ,		/* Discarded VC packet Rz*/
  ABRTINPROG     = 65,		/* command aborted in progress Rz*/
  ABRTBEFORE     = 66,		/* command aborted before execution Rz*/
  CMDNOTFOUND    = 67,		/* aborted command not found Rz*/
  BUSRST         = 68,		/* bus was reset Rz*/
  SELTMO         = 69,		/* selection timeout Rz*/
  CMDTMO         = 70,		/* command timeout expired Rz*/
  PARERR         = 71,		/* SIOP parity error detected Rz*/
  UNEXDISCON     = 72,		/* Unexpected disconnect Rz*/
  ILLPHASE       = 73,		/* illegal DSSI phase transition Rz*/
  GROSSERR       = 74,		/* DSSI Gross error Rz*/
  NAK            = 75,		/* Transmit not acknowledged Rz*/
  RTRYEXHSTD     = 76,		/* Retries Exhausted Rz*/
  ILLADSTATE     = 77,		/* rcv'd non-diag cmd in AdDiag state Rz*/
  ILLBUFPTR      = 78		/* illegal buffer ptr type in cmd Rz*/ 
};
                     
typedef union dssi_status {
  unsigned short int i;		/* Status treated as a short integer */
  struct {
    unsigned int fail : 1;	/* Failure occured */
    unsigned int type : 7;	/* Error type */
    union {
      unsigned int dssi : 8;	/* DSSI-2 specific status */
      struct {
	unsigned int    : 4;
	unsigned int ca : 1;	/* Count all addresses */
	unsigned int    : 3;
      } c;
    } s;
  } f;				/* Status treated as bit fields (16 bits) */
} DSSI_STATUS;

struct kfpsa_blk {
    unsigned int vect1; 
    unsigned int p_receive_pid;
    unsigned int p_send_pid;
    unsigned int pid;
    unsigned int status;
    unsigned int refcnt;
    struct kfpsa_pb *pb;
    struct SEMAPHORE kfpsa_i;
    struct SEMAPHORE kfpsa_s;
    struct SEMAPHORE kfpsa_process;
    struct SEMAPHORE kfpsa_snd;
    struct SEMAPHORE kfpsa_rec;
} KFPSA_BLK;

typedef volatile struct kfpsa_pb {
    struct n810_pb     n810pb;          /* Real N810 PB */
    unsigned int       reset_flag;	/* Flag for reset in progress */
    unsigned int       adap_response_c;	/* Number of responses */
    ADAPTER_STATE      state;
    struct kfpsa_blk   *blk_ptr;
    struct mscp_sb     *sb_pool;	/* Pointer to System Block array */
    struct mscp_sb     **sb_ptr_pool;	/* Pointer to array to SB pointers */
    struct SEMAPHORE   reset_s;		/* Posted to start bus reset */
    struct SEMAPHORE   init_s;		/* Owner of Adapter Block initialize */
    struct SEMAPHORE   adap_s;	        /* Owner of adapter */
} KFPSA_PB;

