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
 * Abstract:	Port Block (PB) Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	03-Feb-1994	Initial entry.
 */
 
#define QUEUE_SIZE 4

volatile struct isp1020_pb {
    struct pb pb;
    struct nvram nvram;
    unsigned long int node_id;
    unsigned long int soft_term;
    unsigned char *fw_address;
    unsigned long int fw_length;
    struct TIMERQ isr_t;
    unsigned long int int_pending;
    struct SEMAPHORE owner_s;
    unsigned long int command_index;
    unsigned long int status_index;
    struct command command[QUEUE_SIZE];
    struct status status[QUEUE_SIZE];
    unsigned long int poll_active;
    } ;
