/*
 * Copyright (C) 1997 by
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
 * Abstract:	I2O Port Block (PB) Definitions
 *
 * Author:	Dennis Mazur
 *
 * Modifications:
 *
 *	dm 	14-Aug-1997	Initial entry.
 */
 
volatile struct i2o_pb {
    struct pb pb;
    struct SEMAPHORE owner_s;
    struct SEMAPHORE reply_s;
    struct SEMAPHORE hr_complete_s;
    int hr_pid;
    int mem_base;
    int state;
    int *mf_pool;                  /* pointer to block of message frames */
    int *lct;			   /* pointer to this IOP's LCT */
    unsigned long int poll_active;
    struct QUEUE rq;		   /* queue for any resources that must be */
				   /* freed on shutdown */
    } ;
