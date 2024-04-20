/*
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
 *
 * Abstract:	Port Block (PB) Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	02-Oct-1995	Initial entry.
 */
 
volatile struct dq_pb {
    struct pb pb;
    struct SEMAPHORE owner_s;
    unsigned long int csr;
    unsigned long int csr2;
    unsigned long int sectors;
    unsigned long int heads;
    unsigned long int cylinders;
    unsigned long int dma;
    unsigned long int lba;
    unsigned long int poll_active;
    } ;
