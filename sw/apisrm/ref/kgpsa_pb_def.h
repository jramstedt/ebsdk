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
 * Abstract:	Port Block (PB) Definitions
 *
 * Author:	Dennis Mazur
 *
 * Modifications:
 *
 *	dm	28-May-1996	Initial entry.
 */


struct TASK_S {
    int pid;
    struct SEMAPHORE wake_s;
    struct SEMAPHORE comp_s;
};

struct TASK_T {
    int pid;
    struct TIMERQ wake_t;
    struct SEMAPHORE comp_s;
};

volatile struct kgpsa_pb {
    struct pb pb;
    int pbx;			/* index into kgpsa_pbs table of pb's */
    struct  PBQ	pbq;
    unsigned long int node_id;
    unsigned __int64 slim_base;
    unsigned __int64 csr_mem_base;
    unsigned __int64 csr_io_base;
    int	prefetch;
    int node_count;
    FC_BRD_INFO binfo;
    struct  QUEUE iocbhdr_q;
    int	    iocbhdr_q_cnt;
    struct  QUEUE els_ring_buf_q;
    struct SEMAPHORE els_owner_s;
    struct SEMAPHORE fcp_owner_s;
    struct SEMAPHORE ring_s;
    struct LOCK mbx_l;
    struct LOCK record_l;
    struct TIMERQ poll_t;
    struct LOCK owner_l;
    struct SEMAPHORE poll_done_s;
    volatile int els_suspend;
    volatile int fcp_suspend;
    volatile unsigned long devState;
    volatile unsigned long DiscState;
#      define DISCOVERY_NEEDED      1
#      define DISCOVERY_IN_PROGRESS 2
#      define DISCOVERY_DONE        3
    unsigned long int attType;
    unsigned long int topology;
    volatile unsigned long int LINKUPeventTag;
    volatile unsigned long int DiscLINKUPeventTag;
    volatile unsigned long int RSCNeventTag;
    volatile unsigned long int DiscRSCNeventTag;
    volatile unsigned long int LINKDOWNeventTag;
    int linkdownintvcnt;
    int CT_late_intvcnt;
    unsigned short int elscmdcnt;  
    unsigned short int rsvd2;
    unsigned short int fcpcmdcnt;
    unsigned short int rsvd3;
    unsigned short int scsicmdcnt;
    unsigned short int rsvd4;
    int fcp_retries_allowed;
    int sb_cnt;
    volatile int discovery_index;
    int *scsi_targets;
    int scsi_targets_cnt;
    int last_read_iocb[8];                /* for debug; minimal trace */
    volatile char nprobe_state[FC_MAX_NPORTS];
#     define	PROBE_IN_PROGRESS 1
#     define	PROBE_DONE        2
#     define    PROBE_NON_EXISTANT_DID 0x99
    unsigned long int poll_active;
    int begin_used_bytes;
    int end_used_bytes;
    struct TASK_T fc;
    struct TASK_S poll;

    volatile int slim_trace_enable;	
    volatile int slim_trace_entries;
    volatile int slim_trace_currentry;
    volatile int slim_trace_cnt;
    volatile int *slim_trace_start;

    volatile int iocb_trace_enable;
    volatile int iocb_trace_entries;
    volatile int iocb_trace_currentry;
    volatile int iocb_trace_cnt;
    volatile int *iocb_trace_start;
    } ;

