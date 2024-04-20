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
 * Abstract:	Port Block (PB) Definitions
 *
 * Author:	Judith Gold
 *
 * Modifications:
 *
 *	jeg	01-Aug-1992	Initial entry.
 */
 
volatile struct pkh_pb {
    struct pb pb;
    struct TIMERQ isr_t;
    struct SEMAPHORE owner_s;
    struct LOCK owner_l;
    unsigned long int int_pending;
    unsigned long int node_id;
    struct aha1742_regs *rp;		/* pointer to ADAPTEC csrs */
    unsigned long int mboxout0;		/* pointer to current ccb */
    unsigned long int poll_active;
    } ;
