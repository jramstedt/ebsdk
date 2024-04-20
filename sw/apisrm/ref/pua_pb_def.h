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
 *      djm	17-Jun-1991	Gave CSR struct union
 *
 *	sfs	27-Jun-1990	Initial entry.
 */
 
volatile struct pua_pb {
    struct pb pb;
    unsigned long int node_id;
    struct shac_csr *csr;
    struct sshma *sshma;
    struct PBQ pbq;
    struct pdt *pdt;
    unsigned long int unsupported;
    } ;
