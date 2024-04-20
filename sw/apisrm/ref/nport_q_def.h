/*
 * Copyright (C) 1990 by
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
 * Abstract:	Adapter Block (AB) Definitions for N_Port
 *		Queue Definitions for N_Port
 *		Buffer Descriptor Definitions for N+Port
 *
 * Author:	Thomas G. Arnold
 *
 * Modifications:
 *
 *	tga	18-Apr-1991	Initial entry.
 */
 
/*
 * N_Port Queue definitions
 */

enum queue_type {DriverToAdapter, AdapterToDriver, AdapterToAdapter,
		   DriverToDriver};

#define DFQE_SIZE 512
#define ADAPTER_STOPPER ((N_CARRIER *) 0) /* Adapter -- LSB == 0 as stopper */
#define DRIVER_STOPPER ((N_CARRIER *) 1)  /* Driver -- LSB == 1 as stopper */
  
/* N_Port Queue Buffer */
typedef volatile struct n_q_buf_head {	
  struct n_q_buf_head  *token;	/* Driver address of queue buffer */
  unsigned int   rsvd;		/* ALPHA C compiler does not support 64 bit */
  unsigned char  opc;		/* Command operation code */
  unsigned char  chnl_idx;	/* Channel index */
  unsigned short int flags;	/* Operation modifier flags */
  unsigned short int status;	/* Response status */
  unsigned char  i_opc;		/* Interconnect opcode */
  unsigned char  i_flags;	/* Interconnect flags */
  unsigned short dst_xport;	/* Destination Xport */
  unsigned short src_xport;	/* Source Xport */
  unsigned int   rsvd1;		/* Reserved */
} N_Q_BUF_HEAD;

/* Queue structures.  Originally for VAX architecture, but the C language */
/* people do not support the base type of ALPHA (64 bit long integers,) so */
/* we have to kludge it. (Other developers refer to this as "slime"-- I */
/* totally agree.) */

/* N_Port Queue Carrier */
typedef volatile struct n_carrier {	
  struct n_carrier *next_ptr;	/* Forward link & stopper flag */
  int               rsvd0;	/* ALPHA C does not support 64-bit long */
  N_Q_BUF_HEAD     *q_buf_ptr;	/* Pointer to cmd/resp buffer */
  int               rsvd1;	/* ALPHA C does not support 64-bit long */
  N_Q_BUF_HEAD     *q_buf_token;/* Driver address of q_buffer */
  int               rsvd2;	/* ALPHA C does not support 64-bit long */
  struct n_carrier *car_token;	/* Driver address of carrier */
  int               rsvd3;	/* ALPHA C does not support 64-bit long */
} N_CARRIER;

/* N_Port Queue Pointer */
typedef volatile struct n_qp {		
  N_CARRIER *head_ptr;		/* Pointer to head of queue */
  int        rsvd0;		/* ALPHA C does not support 64-bit long */
  N_CARRIER *tail_ptr;		/* Pointer to tail of queue */
  int        rsvd1;		/* ALPHA C does not support 64-bit long */
} N_QP;

