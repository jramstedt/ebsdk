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
 * Abstract:	KZPSA Port Block (PB) Definitions
 *
 * Author:	Judith Gold
 *
 * Creation Date: 20-May-1994
 *
 * Modifications:
 *
 *	
 */
#include "cp$src:pks_flash_def.h"

#if RAWHIDE
#define DRIVERSHUT 1
#endif

volatile struct pks_pb {
    struct sim_pb pb;   		/* SIMPort port block */
    struct sim_regs *rp;		/* pointer to SIMPORT csrs base */
    void *flp;				/* pointer to KZPSA flash rom base */
    struct TIMERQ isr_t;
    struct SEMAPHORE owner_s;
    struct LOCK owner_l;
    unsigned long int int_pending;
    unsigned long int node_id;
    unsigned long int reinit;
    PSDLClassType4 HBAConfig;		/* parameters read from on-board ROM */
    unsigned long int poll_active;
#if DRIVERSHUT
    struct SEMAPHORE init;
    struct SEMAPHORE shut;
    struct SEMAPHORE process;
    int status;
#endif
    } ;
