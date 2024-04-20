/*
 * kzpaa_def.h
 *
 * Copyright (C) 1995 by
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
 * Abstract:	Definitions for KZPAA SCSI Adapter
 *
 * Author:	Console Firmware Team
 *
 */
 
struct kzpaa_blk {
    u_int pid;
    u_int status;
    u_int refcnt;
    struct kzpaa_pb *pb;
    struct SEMAPHORE init;
    struct SEMAPHORE shut;
    struct SEMAPHORE process;
};

volatile struct kzpaa_pb {
    struct pb pb;
    u_int node_id;
    u_int fast;
    u_int disconnect;
    u_int int_pending;
    u_char *scripts;
    struct TIMERQ isr_t;
    struct QUEUE send_q;
    struct QUEUE receive_q;
    struct SEMAPHORE send_s;
    struct SEMAPHORE receive_s;
    struct SEMAPHORE owner_s;
    struct LOCK owner_l;
    u_char dstat;
    u_char sist0;
    u_char sist1;
    u_char pad;
} ;

