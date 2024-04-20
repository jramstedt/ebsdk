/*
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
 * Abstract:	IDE Port Block (PB) Definitions
 *
 * Author:	Judith Gold
 *
 * Modifications:
 *
 *	jeg	19-Nov-1992	Initial entry.
 */
 
volatile struct ide_pb {
    struct pb pb;
    struct POLLQ pqp;			/*Poll queue*/			
    struct TIMERQ isr_t;
    struct SEMAPHORE owner_s;
    unsigned int cur_cyl;		/* current cylinder disk is at */
    unsigned int retries;		/* for read/write commands */
    struct ub *ub[IDE_MAX_UNITS];	/* num of units off of the controller */
    unsigned char *results[IDE_MAX_UNITS];/* results after issuing command    */
    struct bootsector *bs[IDE_MAX_UNITS];/* boot sector device parameters    */
    } ;
