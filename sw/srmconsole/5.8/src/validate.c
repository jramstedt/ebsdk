/* file:	validate.c
 *
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	Miscellaneous validation/consistency checks.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson, Philippe Klein
 *
 *  CREATION DATE:
 *  
 *      12-Dec-1991
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	12-Dec-1991	Adapted from shell_commands and qvalidate.
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:stddef.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:parse_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$inc:prototypes.h"


#define	inclusive(x,lower,upper) \
	(((lower) <= (x)) && ((x) <= (upper)))


/*+
 * ============================================================================
 * = queue_validate - queue consistency check                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Check a double linked queue for consistency.  This routine assumes
 *	that the caller has done any locking necessary so that the queue
 *	may be safely traversed.
 *
 *      The routine returns success if:
 *      -all the links point within the specified memory space
 *      -flink and blink point exactly to the same elements
 *      -the queue contains at least the specified 
 *       minimum number of elements.
 *      -the queue does not contain more elements that the specified 
 *       maximum number of elements
 *
 *  
 * FORM OF CALL:
 *  
 *	queue_validate (q, min_bound, max_bound, min_element, max_element)
 *  
 * RETURN CODES:
 *
 *	msg_success   - validated
 *      msg_failure   - any one of several resons
 *       
 * ARGUMENTS:
 *
 *	struct QUEUE *header    - a pointer to the queue header 
 *      unsigned min_bound      - lower memory bound for elements
 *      unsigned max_bound      - upper memory bound for elements
 *      			  (min_bound = 0 , max_bound = -1) 
 *			          disables the address check
 *      unsigned min_element    - the minimal number of queue elements
 *      unsigned max_element    - the maximal number of queue elements
 * 				  (min_element = 0 , max_element = -1) 
 *				  disables the size check
 *
 *
 * SIDE EFFECTS:
 *
 *	None
 *
 *
-*/
int queue_validate (struct QUEUE *header,
      unsigned min_bound, unsigned max_bound, 
      unsigned min_element, unsigned max_element)    {

        int		n;
        struct QUEUE    *q;
	int		status;

        q = header;
	status = msg_success;

	for (n = 0; (status == msg_success) && (n < max_element); n ++) { 

	    /* q element itself must be in range */
	    if (!inclusive (q, min_bound, max_bound)) {
		pprintf ("Q out of range (%08X <= %08X <= %08X)\n",
		     min_bound, q, max_bound
		);
		status = msg_failure;
		continue;
	    }

	    /* flink must be in range */
	    if (!inclusive (q->flink, min_bound, max_bound)) {
		pprintf ("Q flink out of range (%08X <= %08X <= %08X)\n",
		     min_bound, q->flink, max_bound
		);
		status = msg_failure;
		continue;
	    }

	    /* blink must be in range */
	    if (!inclusive (q->blink, min_bound, max_bound)) {
		pprintf ("Q blink out of range (%08X <= %08X <= %08X)\n",
		     min_bound, q->blink, max_bound
		);
		status = msg_failure;
		continue;
	    }

	    /* next backlink must point to here */
	    if ( q->flink->blink != q ) {
		pprintf ("q->flink->blink != q\n	%08X: %08X %08X\n		%08X: %08X %08X\n",
		    q, q->flink, q->blink,
		    q->flink, q->flink->flink, q->flink->blink
		);
		status = msg_failure;
		continue;
	    }
 
	    if (q->flink == header) {
		break;
	    }

            q =  q->flink;
	}

	/* must have proper number of elements */
	if (!inclusive (n, min_element, max_element)) {
	    pprintf ("Q number of elements out of range (%08X: %d <= %d <= %d)\n",
        	header, min_element, n, max_element
	    );
	    status = msg_failure;
	}

	return status;
}
