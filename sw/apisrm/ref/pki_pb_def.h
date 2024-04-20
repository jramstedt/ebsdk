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
 *
 * Modifications:
 *
 *	sfs	30-May-1990	Initial entry.
 */
 
volatile struct pki_pb {
    struct pb pb;
    unsigned long int node_id;
    struct sii1_csr *csr;
    unsigned long int vector;
    unsigned char *buffer;
    unsigned short int cstat;
    unsigned short int dstat;
    unsigned short int cstat_mask;
    unsigned short int dstat_mask;
    struct TIMERQ isr_t;
    unsigned long int int_pending;
    struct SEMAPHORE owner_s;
    } ;
