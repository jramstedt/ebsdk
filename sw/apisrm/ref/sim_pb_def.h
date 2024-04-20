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
 * Abstract:	SIMPort Port Block (PB) Definitions
 *
 * Author:	Judith Gold
 *
 * Creation Date: 20-May-1994
 *
 * Modifications:
 *
 */

volatile struct sim_pb {
	struct pb pb; 		/* generic port block */
	struct QUEUE *malloc_queue;
    	struct AdapBlock *ab;	/* pointer to Adapter Block */
	struct sim_regs *sim_regs;  /* SIMPort csr base */
	struct SPO_AdapStateSet *adap_state;/* pointer to info from Adapter */
					    /* State Set Response Message */
	struct SPO_SetParam *set_params;    /* pointer to device specific */
					    /* parameter values for Set Param */
				            /* command. */
	struct SEMAPHORE *sem;              /* pointer to isr semaphore */ 
	struct SEMAPHORE rx_s;		    /* cmd on adsr semaphore */
	struct SEMAPHORE ready;		    /* ready semaphore */
	struct SEMAPHORE done;		    /* done semaphore */
	unsigned int id;		    /* hba scsi id */
	int (*reset)();	    		    /* pointer to port reset routine */
	int (*interrupt)();		    /* pointer to port isr */
	void *adap_pb;			    /* pointer to adapter pb */
	struct SEMAPHORE *owner_s;	    /* pointer to adapter owner semaphore */
	struct LOCK q_lock;		    /* lock for simport queues */
	unsigned int asr;		    /* value of asr on interrupt */
        unsigned int afar_lo;		    /* value of afar_lo on interrupt */
        unsigned int afar_hi;		    /* value of afar_hi on interrupt */
        unsigned int afpr;		    /* value of afpr on interrupt */
 	};                                  
