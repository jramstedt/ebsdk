/*
 * pkz_pb_def.h
 *
 * Copyright (C) 1991,1993 by
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
 * Abstract:	Port Block (PB) Definitions for XZA SCSI N_Port adapter
 *
 * Author:	Thomas G. Arnold
 *
 * Modifications:
 *
 *	
 *	jrk	19-Jan-1993	Rewrite
 *	kl	21-Jul-1992	Driver re-init
 *	tga	 4-Sep-1991	Add save of created PIDs 
 *	tga	18-Apr-1991	Initial entry.
 */
 
typedef volatile struct pkz_chan {	/* Per channel information */
  struct pb          pk_pb;		/*   generic port block */
  void              *pkz_pb;		/*   Pointer to this structure */
  unsigned short int scsi_node_id;	/*   SCSI ID of controller */
  unsigned short int pad;		/*   Pad to longword */
  ADAPTER_STATE      state;		/*   Current channel state */
  unsigned int       cmds;		/*   Commands started for this chan */
  unsigned int       rsps;		/*   Responses returned ... */
} PKZ_CHAN;

typedef volatile struct pkz_pb {
  PKZ_CHAN           chan[XZA_CHANNELS];/* Per channel information */
  NPORT_AB          *ab;		/* N_Port Adapter Block */
  struct MBX        *mbx;		/* Pointer to mail box structure */
  unsigned int       xmi_node;		/* Adapter XMI node */
  unsigned int       lamb_node;		/* LAMB XMI node */
  unsigned int       hose;		/* Hose number */
  unsigned int       xct_id;		/* Current Transaction ID */
  XZA_REGISTERS     *csr;		/* Pointer to adapter DTYPE register */
  unsigned long int  misc_vector;	/* Miscellaneous interrupt vector */
  unsigned long int  rq_vector;		/* Response Queue interrupt vector */
  ADAPTER_STATE      state;		/* Current adapter state */
  N_QP               ddfq;		/* Driver-Driver Free Queue */
  N_Q_BUFFER        *n_q_buffer_pool;	/* Pointer to Queue Buffer array */ 
  N_CARRIER         *n_carrier_pool;	/* Pointer to Carrier array */
  struct scsi_sb    *sb_pool;		/* Pointer to System Block array */
  struct scsi_sb   **sb_ptr_pool;	/* Pointer to array to SB pointers */
  unsigned int       ampb_len;		/* Size of Adap Memory Block (bytes) */
  char              *ampb;		/* Pointer to Adapter Memory Block */
  AMPB_PTR          *ampp;		/* Pointer to array of pointers to */
  					/*   the Adapter Memory Block */
  unsigned int	     ResetInProgress;	/* Reset in progress - defer new cmds*/
  struct SEMAPHORE   reset_s;		/* Posted to start bus reset */
  unsigned int       reset_flag;	/* Flag for reset in progress & chan */
  N_QP               resetq;		/* Driver-Driver Reset Queue */
  N_QP               reset_waitq;	/* Driver-Driver Reset Wait Queue */
  unsigned int       adap_rq_int_c;	/* Number of resp Q interrupts */
  unsigned int       adap_misc_int_c;	/* Number of misc. interrupts */
  struct SEMAPHORE   adap_rq_int_s;	/* Adapter resp. Q interrupt occured */
  struct SEMAPHORE   adap_misc_int_s;	/* Adapter misc. interrupt occured */
  struct SEMAPHORE   init_s;		/* Owner of Adapter Block initialize */
  struct SEMAPHORE   adap_s;	        /* Owner of adapter */
  struct SEMAPHORE   rq_avail_done_s;	/* rq_avail process completion */
  struct SEMAPHORE   read_misc_done_s;	/* read_misc process completion */
  struct SEMAPHORE   poll_done_s;	/* poll process completion */
  struct SEMAPHORE   bus_reset_done_s;	/* bus_reset process completion */ 
  struct SEMAPHORE   pkz_poll_s;	/* poll process sem */ 
  int                rq_avail_pid;	/* PID of pkz_rq_avail() */
  int                read_misc_pid;	/* PID of pkz_read_misc() */
  int                poll_pid;		/* PID of pkz_poll() */
  int                bus_reset_pid;	/* PID of pkz_bus_reset() */
  int		     refcnt;		/* Port start/stop reference count */
  struct device	     *dev;		/* Pointer to dev structure */
} PKZ_PB;
