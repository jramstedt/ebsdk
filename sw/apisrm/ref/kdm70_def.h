/*
 * kdm70_def.h
 *
 * Copyright (C) 1992 by
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
 * Abstract:	KDM70 Definitions
 *
 * Author:	John R. Kirchoff
 *
 * Modifications:
 *
 *	jrk	12-May-1992	new driver scheme
 *
 *	jrk	 1-Apr-1992	Initial entry.
 */

#define xdev	0x0000 		/* XMI Device Register */
#define xber	0x0004 		/* XMI Bus Error Register */
#define xfadr	0x0008 		/* XMI Failing Address Register */

#define XMI$K_IP 0x40
#define XMI$K_SA 0x44
#define XMI$K_PD 0x48

struct kdm70_blk {
    u_int hose;
    u_int slot;
    u_int controller;
    u_int csr;
    u_int vector;
    u_int pid;
    u_int refcnt;
    struct pb *pb;
    struct SEMAPHORE kdm70_s;
    struct SEMAPHORE kdm70_process;
    struct MBX *mbx_int;	/* Pointer to mailbox to use in port's ISR */
};

