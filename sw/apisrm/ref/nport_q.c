/*
 * nport_q.c
 *
 * Copyright (C) 1990,1993 by
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
 * Abstract:	N_Port Queue Functions
 *
 * Author:	Thomas G. Arnold
 *
 * Modifications:
 *
 *	jrk	 4-May-1995	add spinlocks, mbs
 *
 *	jrk	21-Jan-1993	Cleanup, remove logging.
 *
 *	tga	19-Apr-1991	Initial entry.
 */

#define DEBUG 0

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:nport_q_def.h"
#include "cp$inc:prototypes.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:print_var.h"

#if DEBUG
int nport_debug_flag = 0;

char *qtype_txt[]={ "DriverToAdapter",
                    "AdapterToDriver",
                    "AdapterToAdapter",
		    "DriverToDriver"
                  };

#if MODULAR
char *N_opcodes[] = {"SNDPM", "PMREC", "RETQE", "PURGQ", "INVTC", "RDCNT",
  "SETCHNL", "SETCKT"};
#else 
extern char *N_opcodes[];
#endif

#if MODULAR
char *ci_opcodes[] = {			/* */
  "0", "DG", "MSG", "CNF", 		/* */
  "MCNF", "IDREQ", "RST", "STRT", 	/* */
  "DAT0", "DAT1", "DAT2", "ID", 	/* */
  "PSREQ", "LB", "MDAT", "RETPS", 	/* */
  "SNDDAT", "RETDAT", "SNDMDAT", "RETMDAT"
					/* */
  };
#else
extern char *ci_opcodes[];
#endif
#endif

#define free(x) dyn$_free(x,DYN$K_SYNC)

extern struct LOCK spl_kernel;

#if DEBUG
#define dprintf(fmt, args)	\
    _dprintf(fmt, args)
#else
#define dprintf(fmt, args)
#endif

#if DEBUG

static _dprintf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((nport_debug_flag & 0x11) == 0x11)
	qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    else if ((nport_debug_flag & 1) != 0)
	printf(fmt, a, b, c, d, e, f, g, h, i, j);
}
#endif

/*+
 * ============================================================================
 * = n_insert_entry - Insert Queue entry onto N_Port Queue                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine inserts an entry onto the tail of an N_Port style queue.
 *	It is the Carrier which is really linked onto the queue; the
 *	Carrier points at the Queue Buffer, which contains the adapter data.
 *
 *	Note that this routine does an insertion onto a DriverToAdapter queue.
 *	This effects which pointers are treated as physical and which are
 *	treated as virtual.  
 *  
 * FORM OF CALL:
 *  
 *	n_insert_entry(&tail_ptr, car_ptr, q_b_ptr);
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	tail_ptr	- Address of pointer to tail of queue. (Virtual)
 *	car_ptr		- Pointer to queue carrier. (Virtual)
 *	q_b_ptr		- Pointer to queue buffer. (Virtual)
 *
 * SIDE EFFECTS:
 *
 *	A queue entry is added to the queue.  The tail pointer is updated.
 *	This routine DOES NOT write any adapter registers.
 *
-*/

void n_insert_entry(N_CARRIER **tail_ptr, N_CARRIER *car_ptr, N_Q_BUF_HEAD *q_b_ptr)
{
    dprintf("n_insert_entry: tail %x car %x q_b %x\n", p_args3(tail_ptr, car_ptr, q_b_ptr));

    spinlock(&spl_kernel);

    car_ptr->next_ptr = ADAPTER_STOPPER;
					/* Turn carrier into stopper */
    car_ptr->car_token = car_ptr;	/* Car_token gets virtual address */
					/*  of carrier */
    q_b_ptr->token = q_b_ptr;		/* Queue buffer token gets virt addr */
					/*  of queue buffer */
    (*tail_ptr)->q_buf_ptr = vtop(q_b_ptr);
					/* Link queue buffer onto stopper. */
    (*tail_ptr)->q_buf_token = q_b_ptr;	/* Token gets VA of queue buffer. */

    mb();				/* Order writes */

    (*tail_ptr)->next_ptr = (int)(vtop(car_ptr)) + 1;
					/* Link in new stopper, & make */

    mb();				/* Order writes */

    *tail_ptr = car_ptr;		/* Update tail pointer */

    mb();				/* Order writes */

    spinunlock(&spl_kernel);
}

/*+
 * ============================================================================
 * = n_driver_insert - Insert Queue entry onto Driver-Driver N_Port Queue     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine inserts an entry onto the tail of an N_Port style queue.
 *	It is the Carrier which is really linked onto the queue; the
 *	Carrier points at the Queue Buffer, which contains the adapter data.
 *
 *	Note that this routine does a insertion onto a DriverToDriver Queue.
 *	This effects which pointers are treated as physical and which are 
 *	treated as virtual.
 *	
 * FORM OF CALL:
 *  
 *	n_driver_insert(&tail_ptr, car_ptr, q_b_ptr);
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	tail_ptr	- Address of pointer to tail of queue. (Virtual)
 *	car_ptr		- Pointer to queue carrier. (Virtual)
 *	q_b_ptr		- Pointer to queue buffer. (Virtual)
 *
 * SIDE EFFECTS:
 *
 *	A queue entry is added to the queue.  This routine DOES NOT write any
 *	adapter registers.  The tail pointer is modified.
 *
-*/

void n_driver_insert(N_CARRIER **tail_ptr, N_CARRIER *car_ptr,
  N_Q_BUF_HEAD *q_b_ptr)
{
    dprintf("n_driver_insert: tail %x car %x q_b %x\n", p_args3(tail_ptr, car_ptr, q_b_ptr));

    spinlock(&spl_kernel);

    car_ptr->next_ptr = DRIVER_STOPPER;	/* Turn carrier into stopper */
    car_ptr->car_token = car_ptr;	/* Car_token gets virtual address */
					/*  of carrier */
    q_b_ptr->token = q_b_ptr;		/* Queue buffer token gets virt addr */
					/*  of queue buffer */
    (*tail_ptr)->q_buf_ptr = vtop(q_b_ptr);
					/* Link queue buffer onto stopper. */
    (*tail_ptr)->q_buf_token = q_b_ptr;	/* Token gets VA of queue buffer. */

    mb();				/* Order writes */

    (*tail_ptr)->next_ptr = car_ptr;	/* Link in new stopper, & make */

    mb();				/* Order writes */

    *tail_ptr = car_ptr;		/* Update tail pointer */

    mb();				/* Order writes */

    spinunlock(&spl_kernel);
}

/*+
 * ============================================================================
 * = n_driver_remove_entry - Remove Queue entry from N_Port Queue             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine removes an entry from the head of an N_Port style queue.
 *	It is the Carrier which is really present on the queue; the
 *	Carrier points at the Queue Buffer, which contains the adapter data.
 *
 *	Note that this routine assumes  ; this effects
 *	which pointers are treated as physical and which are treated as
 *	virtual.  
 *  
 * FORM OF CALL:
 *  
 *	n_driver_remove_entry(&head_ptr, &car_ptr, &q_b_ptr);
 *  
 * VALUE:
 *  
 *	msg_failure	- No queue entry to return
 *
 *	msg_success	- Queue entry returned
 *  
 * RETURNS:
 *
 *	head_ptr	- Pointer to the updated queue head. (Virtual)
 *	car_ptr		- Pointer to the removed carrier or stopper value. (Virtual)
 *	q_b_ptr		- Pointer to the removed queue buffer.
 *			  Valid only if car_ptr does not
 *			  contain the stopper value. (Virtual)
 *       
 * ARGUMENTS:
 *
 *	head_ptr	- Address of pointer to head of queue. (Virtual)
 *	car_ptr		- Address of pointer to carrier (Virtual)
 *	q_b_ptr		- Address of pointer to queue buffer. (Virtual)
 *
 * SIDE EFFECTS:
 *
 *	A queue entry may be removed from the queue. Pointers are updated.
 *
-*/

int n_driver_remove_entry(N_CARRIER **head_ptr, N_CARRIER **car_ptr,
  N_Q_BUF_HEAD **q_b_ptr)
{
    dprintf("n_driver_remove_entry: head %x car %x q_b %x\n", p_args3(head_ptr, car_ptr, q_b_ptr));

    spinlock(&spl_kernel);

    *car_ptr = *head_ptr;		/* Return carrier or stopper */

    if (((u_int) (*car_ptr)->next_ptr & 1) == DRIVER_STOPPER) {
	spinunlock(&spl_kernel);
	dprintf("n_driver_remove_entry: stopper\n", p_args0);
	return (msg_failure);		/* Is a stopper; no queue buffer */
    }

/* Not a stopper */

    *q_b_ptr = (*car_ptr)->q_buf_token;	/* Return queue buffer (virtual) */
    *head_ptr = (*car_ptr)->next_ptr;	/* Update head link */

    mb();				/* Order writes */

    spinunlock(&spl_kernel);

    dprintf("n_driver_remove_entry: head %x car %x q_b %x\n", p_args3(*head_ptr, *car_ptr, *q_b_ptr));
    return (msg_success);		/* Valid queue buffer returned */
}

int n_driver_remove_entry1(N_CARRIER **head_ptr, N_CARRIER **car_ptr,
  N_Q_BUF_HEAD **q_b_ptr)
{
    dprintf("n_driver_remove_entry1: head %x car %x q_b %x\n", p_args3(head_ptr, car_ptr, q_b_ptr));

    spinlock(&spl_kernel);

    *car_ptr = *head_ptr;		/* Return carrier or stopper */

    if (((u_int) (*car_ptr)->next_ptr & 1) == DRIVER_STOPPER) {
	spinunlock(&spl_kernel);
	dprintf("n_driver_remove_entry1: stopper\n", p_args0);
	return (msg_failure);		/* Is a stopper; no queue buffer */
    }

/* Not a stopper */

    *q_b_ptr = (*car_ptr)->q_buf_ptr;	/* Return queue buffer (virtual) */
    *head_ptr = (*car_ptr)->next_ptr;	/* Update head link */

    mb();				/* Order writes */

    spinunlock(&spl_kernel);

    dprintf("n_driver_remove_entry1: head %x car %x q_b %x\n", p_args3(*head_ptr, *car_ptr, *q_b_ptr));
    return (msg_success);		/* Valid queue buffer returned */
}

/*+
 * ============================================================================
 * = n_adapter_remove_entry - Remove Queue entry from N_Port Queue            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine removes an entry from the head of an N_Port style queue.
 *	It is the Carrier which is really present on the queue; the
 *	Carrier points at the Queue Buffer, which contains the adapter data.
 *
 *	Note that this routine assumes removal is from a Driver to Adapter
 *	queue; this effects which pointers are treated as physical and
 *	which are treated as virtual. 
 *  
 * FORM OF CALL:
 *  
 *	n_adapter_remove_entry(&head_ptr, &car_ptr, &q_b_ptr);
 *  
 * VALUE:
 *  
 *	msg_failure	- No queue entry to return
 *
 *	msg_success	- Queue entry returned
 *  
 * RETURNS:
 *
 *	head_ptr	- Pointer to the updated queue head. (Virtual)
 *	car_ptr		- Pointer to the removed carrier or stopper value. (Virtual)
 *	q_b_ptr		- Pointer to the removed queue buffer.
 *			  Valid only if car_ptr does not
 *			  contain the stopper value. (Virtual)
 *       
 * ARGUMENTS:
 *
 *	head_ptr	- Address of pointer to head of queue. (Virtual)
 *	car_ptr		- Address of pointer to carrier (Virtual)
 *	q_b_ptr		- Address of pointer to queue buffer. (Virtual)
 *
 * SIDE EFFECTS:
 *
 *	A queue entry may be removed from the queue. Pointers are updated.
 *
-*/

int n_adapter_remove_entry(N_CARRIER **head_ptr, N_CARRIER **car_ptr,
  N_Q_BUF_HEAD **q_b_ptr)
{
    dprintf("n_adapter_remove_entry: head %x car %x q_b %x\n", p_args3(head_ptr, car_ptr, q_b_ptr));

    spinlock(&spl_kernel);

    *car_ptr = (N_CARRIER *) ptov(*head_ptr);
					/* Return carrier or stopper */

    if (((u_int) (*car_ptr)->next_ptr & 1) == ADAPTER_STOPPER) {
	spinunlock(&spl_kernel);
	dprintf("n_adapter_remove_entry: stopper\n", p_args0);
	return (msg_failure);		/* Is a stopper; no queue buffer */
    }

/* Not a stopper */

    *q_b_ptr = (*car_ptr)->q_buf_token;	/* Return queue buffer (virtual) */
    *head_ptr = (*car_ptr)->next_ptr;	/* Update head link */

    mb();				/* Order writes */

    spinunlock(&spl_kernel);

    dprintf("n_adapter_remove_entry1: head %x car %x q_b %x\n", p_args3(*head_ptr, *car_ptr, *q_b_ptr));
    return (msg_success);		/* Valid queue buffer returned */
}

/*+
 * ============================================================================
 * = n_driver_entry_present - Test N_Port Queue for Presence of Entry         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine tests an N_Port style queue for the presence of entries.
 *	The entry is NOT removed from the queue.  Note that this driver assumes
 *	a Driver to Driver, or an Adapter to Driver Queue.
 *	
 * FORM OF CALL:
 *  
 *	n_driver_entry_present(&head_ptr);
 *  
 * VALUE:
 *  
 *	msg_failure	- No queue entry
 *
 *	msg_success	- Queue entry on queue
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	head_ptr	- Address of pointer to head of queue. (Virtual)
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/

int n_driver_entry_present(N_CARRIER **head_ptr)
{
    return ((((u_int) (*head_ptr)->next_ptr & 1) != DRIVER_STOPPER) ?
      msg_success : msg_failure);
}

/*+
 * ============================================================================
 * = n_adapter_entry_present - Test N_Port Queue for Presence of Entry        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine tests an N_Port style queue for the presence of entries.
 *	The entry is NOT removed from the queue.  Note that this routine
 *	assumes a Driver to Adapter, or an Adapter to Adapter Queue.
 *	
 * FORM OF CALL:
 *  
 *	n_adapter_entry_present(&head_ptr);
 *  
 * VALUE:
 *  
 *	msg_failure	- No queue entry
 *
 *	msg_success	- Queue entry on queue
 *  
 * RETURNS:
 *
 *	
 *       
 * ARGUMENTS:
 *
 *	head_ptr	- Address of pointer to head of queue.
 *			  (Virtual, since called by driver)
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/

int n_adapter_entry_present(N_CARRIER **head_ptr)
{
    N_CARRIER *v;

    v = (N_CARRIER *) ptov(*head_ptr);

    return ((((u_int) v->next_ptr & 1) != ADAPTER_STOPPER) ? msg_success :
      msg_failure);
}

/*+
 * ============================================================================
 * = n_init_queue - Initialize N_Port Queue                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes an N_Port style queue.  The stopper carrier is
 *	initialized.
 *  
 * FORM OF CALL:
 *  
 *	n_init_queue(&head_ptr, &tail_ptr, qtype, car_ptr);
 *  
 * RETURNS:
 *
 *	head_ptr	- Pointer to the updated queue head.
 *			  (Physical for DriverToAdapter)
 *			  (Virtual for AdapterToDriver)
 *			  (Physical for AdapterToAdapter)
 *       
 *	tail_ptr	- Pointer to the updated queue tail.
 *			  (Virtual for DriverToAdapter)
 *			  (Physical for AdapterToDriver)
 *			  (Physical for AdapterToAdapter)
 *
 *
 * ARGUMENTS:
 *
 *	head_ptr	- Address of pointer to head of queue.
 *	tail_ptr	- Address of pointer to tail of queue.
 *      qtype		- AdapterToDriver, DriverToAdapter,
 *			  AdapterToAdapter or DriverToDriver
 *	car_ptr		- Pointer to the carrier to be used
 *
 * SIDE EFFECTS:
 *
 *	The N_Port Queue is initialized.
 *
-*/

void n_init_queue(N_CARRIER **head_ptr, N_CARRIER **tail_ptr, int Qtype,
  N_CARRIER *car_ptr)
{

    dprintf("n_init_queue: head %x tail %x qt %s cp %x\n",
	p_args4(head_ptr, tail_ptr, qtype_txt[Qtype], car_ptr));

/* Setup the carrier for use as a stopper */

    car_ptr->car_token = car_ptr;	/* Carrier token gets VA */

    switch (Qtype) {

	case DriverToAdapter: 
	    car_ptr->next_ptr = ADAPTER_STOPPER;
	    *head_ptr = vtop(car_ptr);
	    *tail_ptr = car_ptr;
	    break;

	case AdapterToDriver: 
	    car_ptr->next_ptr = DRIVER_STOPPER;
	    *head_ptr = car_ptr;
	    *tail_ptr = vtop(car_ptr);
	    break;

	case AdapterToAdapter: 
	    car_ptr->next_ptr = ADAPTER_STOPPER;
	    *head_ptr = vtop(car_ptr);
	    *tail_ptr = vtop(car_ptr);
	    break;

	case DriverToDriver: 
	    car_ptr->next_ptr = DRIVER_STOPPER;
	    *head_ptr = car_ptr;
	    *tail_ptr = car_ptr;
	    break;
    }					/* switch(direction) */

    mb();				/* Order writes */
}

#if DEBUG
void dump_n_qp(N_QP *n_qp){
   pprintf("N_Port Queue Pointer: %x\n",n_qp);
   pprintf("  N_CARRIER *head_ptr: %x\n",n_qp->head_ptr);
   pprintf("  rsvd0: %x\n", n_qp->rsvd0);
   pprintf("  N_CARRIER *tail_ptr: %x\n",n_qp->tail_ptr);
   pprintf("  rsvd1: %x\n", n_qp->rsvd1);
   pprintf("\n");
};


void dump_n_carrier(N_CARRIER *n_carrier){
   pprintf("N_Port Queue Carrier: %x\n",n_carrier);
   pprintf("  struct n_carrier *next_ptr: %x\n", n_carrier->next_ptr);
   pprintf("  int rsvd0: %x\n",n_carrier->rsvd0);
   pprintf("  N_Q_BUF_HEAD *q_buf_ptr: %X\n",n_carrier->q_buf_ptr);
   pprintf("  int rsvd1: %x\n",n_carrier->rsvd1);
   pprintf("  N_Q_BUF_HEAD: *q_buf_token: %x\n",n_carrier->q_buf_token);
   pprintf("  int rsvd2: %x\n",n_carrier->rsvd2);
   pprintf("  struct n_carrier *car_token: %x\n",n_carrier->car_token);
   pprintf("  int rsvd3: %x\n",n_carrier->rsvd3);
   pprintf("\n");
};

void dump_n_q_buf_head(N_Q_BUF_HEAD *n_q_buf_head){
   pprintf("  N_Port Queue Buffer: %x\n",n_q_buf_head);
   pprintf("    struct n_q_buf_head *token : %x\n",n_q_buf_head->token);
   pprintf("    unsigned int rsvd : %x\n",n_q_buf_head->rsvd);
   pprintf("    unsigned char opc : %x : %s\n",n_q_buf_head->opc,
                                     N_opcodes[n_q_buf_head->opc]);
   pprintf("    unsigned char chnl_idx : %x\n",n_q_buf_head->chnl_idx);
   pprintf("    unsigned short int flags : %x\n",n_q_buf_head->flags);
   pprintf("    unsigned short int status : %x\n",n_q_buf_head->status);
   pprintf("    unsigned char i_opc : %x : %s\n",n_q_buf_head->i_opc,
                                     ci_opcodes[n_q_buf_head->i_opc]);
   pprintf("    unsigned char i_flags : %x\n",n_q_buf_head->i_flags);
   pprintf("    unsigned short dst_xport : %x\n",n_q_buf_head->dst_xport);
   pprintf("    unsigned short src_xport : %x\n",n_q_buf_head->src_xport);
   pprintf("    unsigned int rsvd1 : %x\n",n_q_buf_head->rsvd1);
};
#endif
