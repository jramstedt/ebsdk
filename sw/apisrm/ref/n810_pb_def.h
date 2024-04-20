/*
 * Copyright (C) 1993 by
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
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	14-Apr-1993	Initial entry.
 */
 
volatile struct n810_pb {
    struct pb pb;
    unsigned long int node_id;
    unsigned long int fast;
    unsigned long int disconnect;
    unsigned char *scripts;
    struct TIMERQ isr_t;
    unsigned long int int_pending;
    struct QUEUE send_q;
    struct QUEUE receive_q;
    struct SEMAPHORE send_s;
    struct SEMAPHORE receive_s;
    struct SEMAPHORE owner_s;
    struct LOCK owner_l;
    unsigned char dstat;
    unsigned char sist0;
    unsigned char sist1;
    unsigned char pad;
    unsigned long int selected;
    unsigned long int poll_active;
    } ;
