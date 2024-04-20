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
 * Author:	Stephen F. Shirron
 *		jds - Jim Sawin
 *
 * Modifications:
 *
 *	jds	28-Jun-1991	Replaced busname with mailbox pointer field.
 *
 *	jds	27-Jun-1991	Add busname field.
 *
 *	jds	05-May-1991	Add KDM70 support for Calypso platform.
 *
 *	sfs	30-May-1990	Initial entry.
 */
 
/* Adaptor capabilities: */

#define PUU$M_DISK 1
#define PUU$M_TAPE 2
#define PUU$M_BOTH 3

volatile struct puu_pb {
    struct pb pb;
    struct MBX *mbx;			    /* Pointer to mailbox to use on behalf of port */
    unsigned short int *ip;
    unsigned short int *sa;
    unsigned long int *pd;		    /* XMI-specific */
    struct comm *comm;
    struct comm *comm_mapped;
    unsigned short int step [4];
    unsigned long int vector;
    unsigned long int driver;
    struct SEMAPHORE receive_s;
    unsigned long int rsp_index;
    unsigned long int cmd_index;
    struct cb *cb [3];
    } ;
