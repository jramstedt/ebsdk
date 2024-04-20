/*
 * simport.c
 *
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
 * Abstract:	SIMPort commands and routines
 *
 * Author:	Judith E. Gold
 *
 * Creation Date: 20-May-1994
 *
 * Modifications:
 *
 *	dtr	1-jun-1995	added a second polling routine for checking
 *				status.  Need because the sim_poll does a 1/2
 *				second delay each and every time.  It was easier
 *				to create a new routine than to make the
 *				existing code work.
 *				
 *      dpm	5-Dec-1994	Added windows base support for mikasa/avanti
 *				everywhere the board would access memory the
 *				windows base register is or'ed in. If the 
 *				cpu accesses it the windows base is anded out. 
 *				This is done via the pks_phys macro, to "or" it
 *				in. and the sim_phys macro to "and" it out
 *				The sim_init was changed to free the malloc'ed
 *				structures. pks_init called it mulitple times 
 *				if it failed, and each time new structures 
 *				would be allocated without freeing the old.
 *
 *
 */

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:common.h"
#include "cp$src:stddef.h"
#include "cp$src:pb_def.h"
#include "cp$src:simport.h"
#include "cp$src:sim_pb_def.h"
#include "cp$src:cam.h"
#include "cp$src:scsi_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:hwrpb_def.h"
#include "cp$inc:platform_io.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#if MODULAR
void sim_shutdown( struct sim_pb *pb) {

    sim_cleanup( pb );

}
#endif

#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define free(x) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)

#if !MODULAR
#define pks_phys(x) ((int)x|io_get_window_base(pb))
#else
#define pks_phys(x) ((int)x|0)
#endif

#define SIM_DEBUG 0

#if SIM_DEBUG
#define qprintf pprintf
#define err_printf pprintf
#endif

/* function prototypes */
unsigned int *sim_alloc_ccb(struct sim_pb *pb);
unsigned __int64 sim_read_long_csr(struct sim_pb *pb, unsigned __int64 csr);
unsigned __int64 sim_write_long_csr(struct sim_pb *pb, unsigned __int64 csr, 
			unsigned __int64 value);
int sim_set_channel_state(struct sim_pb *pb, int enable, unsigned char channel,
				unsigned int *sim_CCB);
sim_channel_state_set_response(struct CAMHeader *camh, int desired_state, 
		unsigned char channel, unsigned int *id);

/*+
 * ============================================================================
 * = sim_init - initialize HBA or its associated channels                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes a SIMPort compatible host bus adapter (HBA).
 *	The routine is invoked with channel = 0 to initialize the adapter.
 *	Additionally, it is called once per channel to initialize each channel
 *	on the adapter.  This follows the model of sim_init() as described by
 *	the SIMPort spec.
 *  
 * FORM OF CALL:
 *  
 *	sim_init( )
 *  
 * RETURNS:
 *
 *	msg_success - initialization complete.
 *	msg_failure - error in initialization.
 *       
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to port block
 *	int channel - channel number to be initialized.  Channel = 0 when the
 *		      routine is first called by a driver to initialize the
 *		      adapter.
 *
 * SIDE EFFECTS:
 *
 *	The adapter and its associated channels may be initialized.
 *	
 *
-*/
int sim_init(struct sim_pb *pb,unsigned char channel,int print_flag)
 {
	unsigned int status;	
	struct sim_regs *reg;
	int i;
	unsigned int asr;

	reg = (struct sim_regs *)(pb->sim_regs);
        if(!channel)		/* reset the adapter */
	 {
		/* error in reset */
		status = sim_reset(pb);
		sim_cleanup(pb);
		if(status == NO_ASR_ERRORS)
		 {	
			return NO_ASR_ERRORS;
		 }
		if(status == msg_failure)  /* error bit set in asr */
		 {
			asr = sim_read_long_csr(pb,&reg->asr_lo);
			err_printf("error initializing %s, ASR = %x",pb->pb.name,asr);
			return msg_failure;
		 }

		/* we've finished resetting, so allocate the Adapter Block */

		pb->ab = (struct AdapBlock *)malloc(sizeof (struct AdapBlock));

		pb->malloc_queue = (struct QUEUE *)malloc(sizeof (struct QUEUE));
		pb->malloc_queue->flink = &pb->malloc_queue->flink;
		pb->malloc_queue->blink = &pb->malloc_queue->flink;

		/* Init the queues. */

		sim_init_queue(pb, &pb->ab->DACQ, 0);
		sim_init_queue(pb, &pb->ab->ADRQ, 1);
		sim_init_queue(pb, &pb->ab->DAFQ, 0);
		sim_init_queue(pb, &pb->ab->ADFQ, 1);

		/* car_tokens get the address of the entries themselves */

#if SIM_DEBUG
		err_printf("pb->ab = %08x\n",
			pb);
		err_printf("pb->ab->DACQ.tail_ptr = %08x\n",
			pb->ab->DACQ.tail_ptr);
		err_printf("pb->ab->DACQ.head_ptr->next_ptr = %08x\n",
			pb->ab->DACQ.head_ptr->next_ptr);
		err_printf("pb->ab->ADRQ.head_ptr = %08x\n",
			pb->ab->ADRQ.head_ptr);
		err_printf("pb->ab->ADRQ.tail_ptr = %08x\n",
			pb->ab->ADRQ.tail_ptr);
  		err_printf("pb->ab->ADRQ.head_ptr->next_ptr = %08x\n",
			pb->ab->ADRQ.head_ptr->next_ptr);
		err_printf("pb->ab->DAFQ.head_ptr = %08x\n",
			pb->ab->DAFQ.head_ptr);
		err_printf("pb->ab->DAFQ.tail_ptr = %08x\n",
			pb->ab->DAFQ.tail_ptr);
  		err_printf("pb->ab->DAFQ.head_ptr->next_ptr = %08x\n",
			pb->ab->DAFQ.head_ptr->next_ptr);
		err_printf("pb->ab->ADFQ.head_ptr = %08x\n",
			pb->ab->ADFQ.head_ptr);
		err_printf("pb->ab->ADFQ.tail_ptr = %08x\n",
			pb->ab->ADFQ.tail_ptr);
  		err_printf("pb->ab->ADFQ.head_ptr->next_ptr = %08x\n",
			pb->ab->ADFQ.head_ptr->next_ptr);
#endif

		/* maximum number of bytes for QueueBuffer */

		pb->ab->qb_size = sizeof(struct CAMHeader) + LARGEST_CAM_CB;

		/* use 32 bit ccb pointers */

		pb->ab->ccb_ptr_size = 4;

		/* let the adapter know the address of the Adapter Block */

		mb();
		sim_write_long_csr(pb,&reg->abbr_lo,pks_phys(pb->ab) >> 5);

		/* set adapter to the disabled state */
		if(sim_set_adapter_state(pb,0,sim_alloc_ccb(pb)) != msg_success)
		 {
		      	if(print_flag)
		     		qprintf("sim_set_adapter_state failed\n");
			sim_cleanup(pb);
			return msg_failure;
		 } 
		/* place the required number of entries onto the DAFQ */

		for(i=0;i<pb->adap_state->n_freeq;i++)
		 {
			queue_insert(pb,&pb->ab->DAFQ,sim_alloc_ccb(pb),0);
		 }

		/* issue a Set Parameters command if required */
		if(sim_set_parameters(pb,sim_alloc_ccb(pb)) != msg_success)
		 {
			qprintf("sim_set_parameters failed\n");
			sim_cleanup(pb);
			return msg_failure;
		 }

		/* set adapter to the enabled state */
		if(sim_set_adapter_state(pb,1,sim_alloc_ccb(pb)) != msg_success)
		 {
			qprintf("second sim_set_adapter_state failed\n");
			sim_cleanup(pb);
			return msg_failure;
		 } 
		return msg_success;

	 }
	else		/* reset the channel specified */
	 {
		if(sim_set_channel_state(pb,1,channel-1,sim_alloc_ccb(pb)) != msg_success)                  
		 {
			qprintf("sim_set_channel_state failed\n"); 
			return msg_failure;  
		 }
	 }
        return msg_success;
 }

void sim_init_queue(struct sim_pb *pb, struct QueueHeader *queue, int stopper)
 {
	struct QueueCarrier *qc;

	qc = sim_malloc(pb,sizeof(struct QueueCarrier));
	if (stopper)
	 {
		queue->head_ptr = qc;
		queue->tail_ptr = pks_phys(qc);
	 }
	else
	 {
		queue->head_ptr = pks_phys(qc);
		queue->tail_ptr = qc;
	 }
	qc->next_ptr = stopper;
	qc->car_token = qc;
 }

int sim_malloc(struct sim_pb *pb, int size)
 {
	struct QUEUE *owned_p;
	void *p;

	owned_p = malloc(sizeof(struct QUEUE) + size);
	p = (int)owned_p + sizeof(struct QUEUE);
	insq_lock(owned_p, pb->malloc_queue->blink);
	return p;
 }

void sim_free(void *p)
 {
	struct QUEUE *owned_p;

	owned_p = (int)p - sizeof(struct QUEUE);
	remq_lock(owned_p);
	free(owned_p);
 }

void sim_free_all(struct QUEUE *malloc_queue)
 {
	struct QUEUE *owned_p;

	while(malloc_queue->flink != &malloc_queue->flink)
	 {
		owned_p = malloc_queue->flink;
		remq_lock(owned_p);
		free(owned_p);
	 }
	free(malloc_queue);
 }

/*+
 * ============================================================================
 * = sim_reset - initiate adapter reset by setting MIN bit in AMCSR  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      SIMPort adapter is reset.  This "command" is not placed in the DACQ.
 *	Instead, the AMCSR register is written to initiate an adapter reset.
 *	Channels will be disabled after the execution of this command.
 *
 * FORM OF CALL:
 *
 *	sim_reset()
 *
 * RETURNS:
 *                        
 *	msg_success		- successful reset
 *	msg_failure		- unsuccessful reset
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *
 * SIDE EFFECTS:
 *
 *      SIMPort adapter may be transitioned into the uninitialized state.  
 *	Adapter channels may be disabled.
 *
-*/
int sim_reset(struct sim_pb *pb)
 {
	struct sim_regs *reg;
	unsigned __int64 asr;

	reg = pb->sim_regs;

	sim_write_long_csr(pb,&reg->amcsr_lo,AMCSR$M_MIN);
	krn$_sleep(2000);	/* wait 2 seconds */
        asr = sim_read_long_csr(pb,&reg->asr_lo);
	if(asr != ASR$M_UNIN)
	 {
		if(asr & (ASR$M_ADSE | ASR$M_AMSE | ASR$M_AAC | ASR$M_AME))
			return msg_failure;
		else                        
			return NO_ASR_ERRORS;
	 }
	return msg_success;		/* reset succeeded */
 }

/*+
 * ============================================================================
 * = queue_insert - insert a SIMPort queue   =
 * ============================================================================
 *
 * OVERVIEW:
 * 	The tail of the linked list of queue carriers is made into a valid
 *	QueueCarrier by pointing to a command queue buffer and by setting 
 *	bit <0> of the next_ptr field.  This indicates that the QueueCarrier is
 *	no longer a stopper.  Also, a new stopper is allocated.  Bit <0> of the
 *	next_ptr field of this QueueCarrier is set to 0 indicating a stopper.
 *	Also, the car_token field is filled in to point to itself.  car_token
 *	and q_buf_token are normally VA's, but since we are running physical,
 *	the appropriate PA is filled in.  Note that the mb()'s are necessary
 *	to insure proper adapter operation.  It is assumed that the entry will
 *	be inserted onto a driver queue since the adapter owns the enqueing to
 *	its own queues.
 *      
 *	
 *
 * FORM OF CALL:
 *
 *	queue_insert()
 *
 * RETURNS:
 *
 *	msg_success - successful inclusion of entry into queue
 *	
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to sim port block
 *	struct QueueHeader *queue - pointer to queue to add an entry to
 *	void *new_cmd_buf - command buffer for CCB to be added to queue
 *	int cmd - 1, if entry is to be placed onto command queue
 *		  0, if entry is to be placed onto the free queue
 *
 *
 * SIDE EFFECTS:
 *
 *      
 *
-*/                      
int queue_insert(struct sim_pb *pb,struct QueueHeader *queue, 
		void *new_cmd_buf, int cmd)
 {
	struct QueueCarrier *entry, *new_car;
	struct CAMHeader *camh;	
	struct sim_regs *reg;

#if SIM_DEBUG
	pprintf("queue_insert\n");
#endif

	reg = pb->sim_regs;

	/* get a new end of list */
	new_car = sim_malloc(pb,sizeof(struct QueueCarrier));
	new_car->next_ptr = 0;
	new_car->car_token = new_car;

	spinlock(&pb->q_lock);
	entry = queue->tail_ptr;	/* points to the current stopper */
	camh = new_cmd_buf;

	/* get a queue buffer for the new command to send */
        entry->q_buf_ptr = pks_phys(new_cmd_buf);  
        entry->q_buf_token = new_cmd_buf;

	/* fill in the rest of the QueueCarrier */
	if(cmd) 			/* for command queue */
	 {
		entry->func_code = camh->function;
		entry->status = camh->status;
		entry->CID.channel = camh->conn_id.channel;	
		entry->CID.target = camh->conn_id.target;	
		entry->CID.lun = camh->conn_id.lun;
		camh->cam_flags.simq_fz_dis = 1;
		camh->cam_flags.cmd_done = 0;
         }
	mb();

	/* update the tail of the list */
        queue->tail_ptr = new_car;
	mb();

	/* make this a valid entry (non-stopper) */
	entry->next_ptr = 1 + pks_phys(new_car);
	mb();

	if(cmd)
	 {
		/* notify adapter of the new queue entry */
		sim_write_long_csr(pb,&reg->dacqir_lo,pks_phys(new_car) >> 3);
	 }
	else                            /* for free queue */
	 {
		sim_write_long_csr(pb,&reg->dafqir_lo,pks_phys(new_car) >> 3);
         }

	spinunlock(&pb->q_lock);
	return msg_success;
 }

/*+
 * ============================================================================
 * = queue_remove - remove an entry from a SIMPort queue  =
 * ============================================================================
 *
 * OVERVIEW:
 * 	If there is an entry in the queue other than a stopper, a valid entry
 *  	is removed from the head of the queue.  A pointer to the CCB is
 *	returned in the argument q_buffer and a pointer to the QueueCarrier
 *	itself is returned in the argument qc.  Although one would normally
 *	remove entries from the adapter queues during routine operations, 
 *	driver queues may be accessed during error handling.
 *
 * FORM OF CALL:
 *
 *	queue_remove()
 *
 * RETURNS:
 *
 *	msg_success - successful removal of entry from queue
 *	msg_failure - queue is empty
 *	
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to simport port block
 *	struct QueueHeader *queue - pointer to the queue to remove the 
 *					entry from
 *	void *q_buffer - pointer to command control block removed from the q
 *	struct QueueCarrier *qc - pointer to QueueCarrier removed from the q
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int queue_remove(struct sim_pb *pb,struct QueueHeader *queue, void **q_buffer, 
		struct QueueCarrier **qc)
 {
	struct QueueCarrier *entry;

#if SIM_DEBUG
	pprintf("queue_remove\n");
#endif

	spinlock(&pb->q_lock);
	entry = queue->head_ptr;
	mb();

	/* adapter queue, for valid carrier, stopper = 0 */
	/* check to make sure there is a queue entry */                
	if(((int)entry->next_ptr & 1) == 0)
	 {
		/* return these values to calling function */
		*q_buffer = entry->q_buf_ptr;
		*qc = entry;
		mb();
		/* update head of queue */
		queue->head_ptr = entry->next_ptr;
		mb();
		spinunlock(&pb->q_lock);
		return msg_success;
	 }

	spinunlock(&pb->q_lock);
	return msg_failure;	/* queue is empty */
 }	

/*+
 * ============================================================================
 * = sim_alloc_ccb - allocate a CCB                                           =
 * ============================================================================
 *
 * OVERVIEW:
 * 	A CCB is allocated and partially filled in for a command to be sent to
 *	the adapter.  The rest of the CCB is filled in by the routine specified
 *      as an argument to send_command.
 *	
 *
 * FORM OF CALL:
 *
 *	sim_alloc_ccb()
 *
 * RETURNS:
 *
 *	msg_success		- successful 
 *	msg_failure		- unsuccessful 
 *   
 * ARGUMENTS:
 *	None
 *	
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
unsigned int *sim_alloc_ccb(struct sim_pb *pb)
 {
	struct CAMHeader *camh;

	camh = sim_malloc(pb,sizeof(struct CAMHeader)+LARGEST_CAM_CB); 
	camh->CCB_addr = pks_phys(camh);
	return camh;
 }

/*+
 * ============================================================================
 * = sim_set_adapter_state - SIMPort adapter is enabled or disabled  =
 * ============================================================================
 *
 * OVERVIEW:
 * 	SIMPort adapter is enabled or disabled according to the value of the
 *	enable parameter.
 *      
 *	
 *
 * FORM OF CALL:
 *
 *	sim_set_adapter_state()
 *
 * RETURNS:
 *
 *	msg_success		
 *	msg_failure		
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *	int enable - 1 = enable, 0 = disable adapter
 *	unsigned int *sim_CCB - pointer to command control block
 *
 * SIDE EFFECTS:
 *
 *      SIMPort adapter may be transitioned to the enabled or disabled state.
 *
-*/
int sim_set_adapter_state(struct sim_pb *pb, int enable, unsigned int *sim_CCB)
 {
	struct CAMHeader *camh;	
	struct SPO_SetAdap_State *param;
	int status;

#if SIM_DEBUG
	pprintf("sim_set_adapter_state\n");
#endif
	/* fill in command specific fields of CAM header */
	camh = (struct CAMHeader *)sim_CCB;
	camh->function = SPO_VU_SET_ADAP_STATE;
	camh->CCB_len = sizeof(struct CAMHeader) + 
				sizeof(struct SPO_SetAdap_State);
	camh->cam_flags.dir = DIR_NO_XFER;
	camh->conn_id.channel = 0;   /* path id */ 
	camh->conn_id.target = 0;    /* target id hasn't been determined yet */
	camh->conn_id.lun = 0;
	/* fill in SIMPort portion of the command */
	param = (struct SPO_SetAdap_State *)((unsigned char *)sim_CCB + 
			   sizeof(struct CAMHeader));
	if(enable)
	   {
		param->state = SIM_ENABLE;
	   }
	else
	   {
		param->state = SIM_DISABLE;
	   }
	/* send the command */
	queue_insert(pb,&pb->ab->DACQ,camh,1);
	while(!camh->cam_flags.cmd_done)
		if(sim_poll(pb) == msg_failure)
			return msg_failure;
	if(camh->cam_flags.sim_error)
		return msg_failure;
	status = sim_adapter_state_set_response(camh,pb,param->state);
        return status;
 }

/*+
 * ============================================================================
 * = sim_adapter_state_set_response - SIMPort adapter is enabled or disabled  =
 * ============================================================================
 *
 * OVERVIEW:
 * 	Response to adapter set state command.
 *	
 *      
 *	
 *
 * FORM OF CALL:
 *
 *	sim_adapter_state_set_response()
 *
 * RETURNS:
 *
 *	msg_success		- successful 
 *	msg_failure		- unsuccessful 
 *   
 * ARGUMENTS:
 *
 *	struct CAMHeader *camh - pointer to CAM header
 *	struct sim_pb *pb - pointer to SIMPort port block
 *	int enable - 1 = enable, 0 = disable adapter
 *
 * SIDE EFFECTS:
 *
 *      SIMPort adapter may be transitioned to the enabled or disabled state.
 *
-*/
int sim_adapter_state_set_response(struct CAMHeader *camh,struct sim_pb *pb, 
			unsigned int desired_state)
 {  
	struct SPO_AdapStateSet *param;
	int status;
 

#if SIM_DEBUG
	pprintf("sim_adapter_state_set_response\n");
#endif
	param = (struct SPO_AdapStateSet *)((unsigned char *)camh + 
			sizeof(struct CAMHeader));
	/* make sure the right channel was enabled/disabled as requested. */
	if(param->state == desired_state)
	 {
		if(desired_state == SIM_DISABLE)
		 {
			/* make a copy of the adapter info for later use */
			/* info is valid only on transition to disabled state*/
			pb->adap_state = sim_malloc(pb,sizeof(struct SPO_AdapStateSet));
			memcpy(pb->adap_state,param,sizeof(struct SPO_AdapStateSet));
		 }
		status = msg_success;
	 }
	else
	 {
		status = msg_failure;
	 }
	sim_free(camh);
	return status;
 }

/*+
 * ============================================================================
 * = sim_set_parameters - set various adapter parameters  =
 * ============================================================================
 *
 * OVERVIEW:
 *	SIMPort adapter parameters may be set using this command.  Parameters
 *	include number of 4K segments used by adapter in host memory, minimum
 *	time adapter must wait for Bus Reset Response command from host, and
 *	others.
 *
 * FORM OF CALL:
 *
 *	sim_set_parameters()
 *
 * RETURNS:
 *
 *	msg_success		
 *	msg_failure		
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *	unsigned int *sim_CCB - pointer to command control block
 *
 * SIDE EFFECTS:
 *
 *	None      
 *
-*/
int sim_set_parameters(struct sim_pb *pb, unsigned int *sim_CCB)
 {
	struct CAMHeader *camh;	
	struct SPO_SetParam *param;
        struct toy_date_time *p;
	unsigned int base_year;
	unsigned int seconds;	/* number of seconds since 1970 */
	int status;

#if SIM_DEBUG
        pprintf("sim_set_parameters\n");
#endif
	/* fill in command specific fields of CAM header */
	camh = (struct CAMHeader *)sim_CCB;
	camh->function = SPO_VU_SET_PARAM;
	camh->CCB_len = sizeof(struct CAMHeader) + 
				sizeof(struct SPO_SetParam);
	camh->cam_flags.dir = DIR_NO_XFER;
	camh->conn_id.channel = 0;   /* path id */ 
	camh->conn_id.target = 0;    /* target id hasn't been determined yet */
	camh->conn_id.lun = 0;
	
	/* fill in SIMPort portion of the command */
	param = (struct SPO_SetParam *)((unsigned char *)sim_CCB + 
			sizeof(struct CAMHeader));
	if(pb->adap_state && pb->set_params)
	 {
		/* first fill in device specific parameters */
		memcpy(param,pb->set_params,sizeof(struct SPO_SetParam));

		/* now fill in what we can from other sources */
		param->n_host_sg = pb->adap_state->n_host_sg;
	 }
	else
	 {
		qprintf("must send Set Adapter State cmd.\n");
		return msg_failure;
	 }
	/* send the command */
	queue_insert(pb,&pb->ab->DACQ,camh,1);
	while(!camh->cam_flags.cmd_done)
		if(sim_poll(pb) == msg_failure)
			return msg_failure;
	if(camh->cam_flags.sim_error)
		return msg_failure;
	return msg_success;  
 }

/*+
 * ============================================================================
 * = sim_set_channel_state - SIMPort channel is enabled or disabled  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	SIMPort channel is enabled or disabled according to the enable
 *	parameter.
 *
 *      
 *	
 *
 * FORM OF CALL:
 *
 *	sim_set_channel_state()
 *
 * RETURNS:
 *
 *	msg_success		
 *	msg_failure	
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *	int enable - 1 = enable, 0 = disable channel
 *	unsigned int *sim_CCB - pointer to command control block
 *
 * SIDE EFFECTS:
 *
 *      SIMPort channel may be enabled/disabled.
 *
-*/
int sim_set_channel_state(struct sim_pb *pb, int enable, unsigned char channel,
				unsigned int *sim_CCB)
 {
	struct CAMHeader *camh;	
	struct SPO_SetChanState *param;
	int status;

#if SIM_DEBUG
	pprintf("sim_set_channel_state\n");
#endif
	/* fill in command specific fields of CAM header */
	camh = (struct CAMHeader *)sim_CCB;
	camh->function = SPO_VU_SET_CHAN_STATE;
	camh->CCB_len = sizeof(struct CAMHeader) + 
				sizeof(struct SPO_SetChanState);
	camh->cam_flags.dir = DIR_NO_XFER;  
	camh->conn_id.channel = 0;   /* path id */ 
	camh->conn_id.target = 0;    /* target id hasn't been determined yet */
	camh->conn_id.lun = 0;

	/* fill in SIMPort portion of the command */
	param = (struct SPO_SetChanState *)((unsigned char *)sim_CCB +
			sizeof(struct CAMHeader));
	param->ChanID = channel;
	param->NodeID = 0xff;   /* allows real ID to be read */
	if(enable)
		param->state = SIM_ENABLE;
	else
		param->state = SIM_DISABLE;
	/* send the command */
	queue_insert(pb,&pb->ab->DACQ,camh,1);
	while(!camh->cam_flags.cmd_done)
		if(sim_poll(pb) == msg_failure)
			return msg_failure;
	if(camh->cam_flags.sim_error)
		return msg_failure;
	status = sim_channel_state_set_response(camh,param->state,channel,
					&pb->id);
	return status;
 }

/*+
 * ============================================================================
 * = sim_channel_state_set_response - SIMPort channel is enabled or disabled  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Response to the set channel state command.
 *	
 *
 * FORM OF CALL:
 *
 *	sim_channel_state_set_response()
 *
 * RETURNS:
 *
 *	msg_success		- successful 
 *	msg_failure		- unsuccessful 
 *   
 * ARGUMENTS:
 *
 *	struct QueueCarrier *qc - pointer to QueueCarrier
 *	struct sim_pb *pb - pointer to SIMPort port block
 *	int enable - 1 = enable, 0 = disable channel
 *
 * SIDE EFFECTS:
 *
 *      SIMPort channel may be enabled/disabled.
 *
-*/
int sim_channel_state_set_response(struct CAMHeader *camh, int desired_state, 
		unsigned char channel, unsigned int *id)
 {  
	struct SPO_ChanStateSet *param;
	int status; 

#if SIM_DEBUG
	pprintf("sim_channel_state_set_response\n");
#endif
	param = (struct SPO_ChanStateSet *)((unsigned char *)camh + 
			sizeof(struct CAMHeader));
	/* make sure the right channel was enabled/disabled as requested. */
	if((param->state == desired_state) && (param->ChanID == channel))
	 {
		*id = param->NodeID;	/* fill in adapter ID */
		status = msg_success;
	 }
	else
		status = msg_failure;
	sim_free(camh);
	return status;
		
 }

/*+
 * ============================================================================
 * = sim_set_device_state - SIMPort device is enabled  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	SIMPort device is enabled after a bus device reset.      
 *	
 *                 
 * FORM OF CALL:
 *
 *	sim_set_device_state()
 *
 * RETURNS:
 *
 *	msg_success		
 *	msg_failure		
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *	int enable - enable or disable device
 *	unsigned int *sim_CCB - pointer to command control block
 *
 * SIDE EFFECTS:
 *
 *      SIMPort device may be enabled.
 *
-*/
int sim_set_device_state(struct sim_pb *pb, int enable, unsigned int *sim_CCB)
 {
	struct CAMHeader *camh;	
	struct SPO_SetChanState *param;
	int status;

#if SIM_DEBUG
   	pprintf("sim_set_device_state\n");
#endif
	/* fill in command specific fields of CAM header */
	camh = (struct CAMHeader *)sim_CCB;
	camh->function = SPO_VU_SET_DEV_STATE;
	camh->CCB_len = sizeof(struct CAMHeader) + 
				sizeof(struct SPO_SetDevState);
	camh->cam_flags.dir = DIR_NO_XFER;
	camh->conn_id.channel = 0;   /* path id */ 
	camh->conn_id.target = pb->id;   /* adapter id */
	camh->conn_id.lun = 0;

	/* fill in SIMPort portion of the command */
	param = (struct SPO_SetDevState *)((unsigned char *)sim_CCB +
			sizeof(struct CAMHeader));
	if(enable)
		param->state = SIM_ENABLE;
	else
		param->state = SIM_DISABLE;
	/* send the command */

	queue_insert(pb,&pb->ab->DACQ,camh,1);
	/* wait for the device state set response */
	if(cam_wait_for_cmd_done(camh) == msg_success)
	 {
		/* check the status of the command */
		if(camh->cam_flags.sim_error)
			status = msg_failure;
		else
		
			status = sim_device_state_set_response(camh,
					    param->state);
	 }
	else 
	 {
		qprintf("SIMPORT set device state command timed out\n");
		status = msg_failure;
	 }
	return status;
 }

/*+
 * ============================================================================
 * = sim_device_state_set_response - response to device set state cmd  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Response to set device state command.
 *	
 *
 * FORM OF CALL:
 *
 *	sim_device_state_set_response()
 *
 * RETURNS:
 *
 *	msg_success		
 *	msg_failure		
 *   
 * ARGUMENTS:
 *
 *	struct CAMHeader *camh - pointer to CAM header
 *	int desired_state - enable/disable device
 *
 * SIDE EFFECTS:
 *
 *      SIMPort device may be enabled.
 *
-*/
int sim_device_state_set_response(struct CAMHeader *camh, int desired_state)
 {
	struct SPO_DevStateSet *param;
	int status; 

#if SIM_DEBUG
   	pprintf("sim_device_state_set_response\n");
#endif
	param = (struct SPO_DevStateSet *)((unsigned char *)camh + 
			sizeof(struct CAMHeader));
	/* check for successful status and make sure device was */
	/* enabled/disabled as requested. */
	if(param->state == desired_state)
		status = msg_success;
	else
		status = msg_failure;
	sim_free(camh);	
	return status;	

 }
#if 0
/*+
 * ============================================================================
 * = sim_verify_adapter_sanity - verify the operation of the SIMPort adapter  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  	The operation of the SIMPort adapter is verified.    
 *	
 *
 * FORM OF CALL:
 *
 *	sim_verify_adapter_sanity()
 *
 * RETURNS:
 *
 *	msg_success		
 *	msg_failure		
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *	unsigned int *sim_CCB - pointer to command control block
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int sim_verify_adapter_sanity(struct sim_pb *pb,unsigned int *sim_CCB)
 {	
	struct CAMHeader *camh;	

	/* fill in command specific fields of CAM header */
	camh = (struct CAMHeader *)sim_CCB;
	camh->function = SPO_VU_VER_ADAP_SANITY;
	camh->CCB_len = sizeof(struct CAMHeader);
	camh->cam_flags.dir = DIR_NO_XFER;
	camh->conn_id.channel = 0;   /* path id */ 
	camh->conn_id.target = pb->id;   /* adapter id */
	camh->conn_id.lun = 0;
	/* send the command */
	queue_insert(pb,&pb->ab->DACQ,camh,1);
	/* wait for a specified time period for the command to complete */
	krn$_sleep(pb->adap_state->ka_time * 1000);
	/* it should be done by now */
	if(cam_wait_for_cmd_done(camh) != msg_success)
   	 {
		/* command timed out */
		qprintf("SIMPort command verify adapter sanity timed out\n");
		return msg_failure;
	 }

	/* this command never fails, so if we picked it up from the */
	/* response queue, there is no status to check, we are done. */
	return msg_success;
 }

/*+
 * ============================================================================
 * = sim_read_counters - read SIMPort adapter maintenance/performance counters  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  	The adapter maintenance and performance counters are read on a per
 *	channel basis.   
 *	
 *
 * FORM OF CALL:
 *
 *	sim_read_counters()
 *
 * RETURNS:
 *
 *	msg_success		
 *	msg_failure		
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *	unsigned int *sim_CCB - pointer to command control block
 *
 * SIDE EFFECTS:
 *
 *      SIMPort 
 *
-*/
int sim_read_counters(struct sim_pb *pb, int zero_counters, unsigned int *sim_CCB)
 {
	struct CAMHeader *camh;	
	struct SPO_ReadCntrs *param;
 	int status;

	/* fill in command specific fields of CAM header */
	camh = (struct CAMHeader *)sim_CCB;
	camh->function = SPO_VU_READ_CNTRS;
	camh->CCB_len = sizeof(struct CAMHeader) + 
				sizeof(struct SPO_ReadCntrs);
	camh->cam_flags.dir = DIR_NO_XFER;
	camh->conn_id.channel = 0;   /* path id */ 
	camh->conn_id.target = pb->id;   /* adapter id */
	camh->conn_id.lun = 0;

	/* fill in SIMPort portion of the command */
	param = (struct SPO_ReadCntrs *)((unsigned char *)camh +
			sizeof(struct CAMHeader));
	if(zero_counters)    /* reset counters after reading them */
		param->flags.ZeroCounters = 1;
	else 
		param->flags.ZeroCounters = 0;
	/* send the command */

	queue_insert(pb,&pb->ab->DACQ,camh,1);
	/* wait for the counters read response */
	if(cam_wait_for_cmd_done(camh) == msg_success)
   	 {
		/* check the status of the command */
		if(camh->cam_flags.sim_error)
			status = msg_failure;
		else
		
			status = sim_counters_read_response(camh,pb);
	 }
	else			/* transaction timed out */
	 {
		qprintf("SIMPort command read counters timed out\n");
		return msg_failure;
		/* reset the adapter */
	 }
    return status;
 }

/*+
 * ============================================================================
 * = sim_counters_read_response - response to read_counters command  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  	The adapter maintenance and performance counters are read on a per
 *	channel basis.   
 *	
 *
 * FORM OF CALL:
 *
 *	sim_counters_read_response()
 *
 * RETURNS:
 *
 *	msg_success		- successful 
 *	msg_failure		- unsuccessful 
 *   
 * ARGUMENTS:
 *
 *
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int sim_counters_read_response(struct CAMHeader *camh,struct sim_pb *pb)
 { 
	struct SPO_CntrsRead *counters;
	int i;
	
	/* start of counter info */
	counters = (struct SPO_CntrsRead *)((unsigned char *)camh + 
		sizeof(struct CAMHeader));    
	printf("Time since counters zero'ed: %d msec \n",
		counters->TimeSinZero);			  	
	printf("Host bus faults: %d\n", counters->BusFaults);
	printf("SCSI Commands send: %d\n", counters->SCSIComSent);
	printf("SCSI Commands received: %d\n",counters->SCSIComRecv);
	printf("Total Data Phase bytes sent: %d\n",counters->DataSent);
	printf("Total Data Phase bytes rec'd: %d\n",counters->DataRecv);
        printf("SCSI bus resets: %d\n",counters->SCSIBusResets);
	printf("Device resets sent: %d\n",counters->BDRsSent);
	printf("Sel/Resel timeouts: %d\n", counters->SelTimeouts);
	printf("Parity Errors: %d\n", counters->ParityErrs);
	printf("Unsol. reselect: %d\n", counters->UnsolReselect);
	printf("Bad messages rec'd: %d\n", counters->BadMessages);
        printf("Message rejects rec'd: %d\n", counters->MessReject);
	printf("Unexpect. Disconnects: %d\n", counters->UnexpDiscon);
	printf("Phase mismatches: %d\n", counters->UnexpPhase);
	printf("Handshake-to-handshake TO: %d\n",counters->SyncViol);
	/* implementation specific counters */
	printf("\n");
	for(i=0;i<22;i++)
		printf("Adapter specific counter[i]:%d\n", 
				counters->ImpSpecific[i]); 
	sim_free(camh);
	return msg_success;
 }

/*+
 * ============================================================================
 * = sim_bus_reset_response - host response to Bus Reset Request Message  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      When receiving a Bus Reset Request Message from the adapter, the host
 *	checks to see if the request reason is valid.  If so, the host will
 *	send a Bus Reset Response Message back to the adapter indicating that
 *	a Bus Reset is acceptable.  The QueueCarrier status bit indicating
 *	the bus reset should be perform is set in queue_insert().
 *	
 *
 * FORM OF CALL:
 *
 *	sim_bus_reset_response()
 *
 * RETURNS:
 *
 *	msg_success		
 *	msg_failure		
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *	unsigned int *sim_CCB - pointer to command control block
 *
 * SIDE EFFECTS:
 *
 *      A SCSI bus reset may occur as a result of sendig this command.
 *
-*/
int sim_bus_reset_response(struct sim_pb *pb, unsigned int *sim_CCB)
 {
	struct CAMHeader *camh;	
	struct QueueCarrier *qc;

	camh = (struct CAMHeader *)sim_CCB;
	camh->function = SPO_VU_BUS_RESET_REQ;
	camh->status = 1;
	camh->CCB_len = sizeof(struct CAMHeader);
	camh->cam_flags.dir = DIR_NO_XFER;
	camh->conn_id.channel = 0;   /*path id */
	camh->conn_id.target = pb->id;   /* adapter id */
	camh->conn_id.lun = 0;
	/* send the command */
	queue_insert(pb,&pb->ab->DACQ,camh,1);
#if 0
	/* the CCB winds up on the ADFQ, so remove it */
	while(queue_remove(pb,&pb->ab->ADFQ,&camh,&qc) != msg_success)
		krn$_sleep(10);
	sim_free(camh);
	sim_free(qc);
#endif
 }
#endif
/*+
 * ============================================================================
 * = sim_execute_scsi_io - initiate SCSI I/O on an adapter channel  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	SCSI I/O on an adapter channel is performed.  This is a CAM defined
 *	command.      
 *	
 *
 * FORM OF CALL:
 *
 *	sim_execute_scsi_io()
 *
 * RETURNS:
 *
 *	msg_success		
 *	msg_failure		
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *	unsigned int *CCB - pointer to the command control block
 *
 * SIDE EFFECTS:
 *
 *      A command may be sent to a SCSI device. 
 *
-*/
int sim_execute_scsi_io(struct sim_pb *pb, unsigned int *sim_CCB)
 {           
	struct CAMHeader *camh;	

#if SIM_DEBUG
	pprintf("sim_execute_scsi_io\n");
#endif

	camh = (struct CAMHeader *)sim_CCB;
	/* send the command */
	queue_insert(pb,&pb->ab->DACQ,camh,1);
	/* wait for the command to complete */
	if(cam_wait_for_cmd_done(camh) != msg_success)
	 {
		qprintf("CAM command EXECUTE_SCSI_IO timed out\n");
		return msg_failure;
	 }
	return msg_success;
 }
#if 0
/*+
 * ============================================================================
 * = sim_release_sim_queue - unfreeze a SIM queue  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *   	Unfreeze a SIMPort queue.  This is a CAM defined command.   
 *	
 *
 * FORM OF CALL:
 *
 *	sim_release_sim_queue()
 *
 * RETURNS:
 *
 *	msg_success		
 *	msg_failure		
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *	unsigned int *sim_CCB - pointer to command control block
 *
 * SIDE EFFECTS:
 *
 *      The unfrozen queue will be placed onto the ADFQ by the adapter. 
 *
-*/
int sim_release_sim_queue(struct sim_pb *pb, unsigned int *sim_CCB)
 {
	struct CAMHeader *camh;	

	camh = (struct CAMHeader *)sim_CCB;
	camh->function = XPT_RELEASE_SIM_QUEUE;
	camh->CCB_len = sizeof(struct CAMHeader);
	camh->conn_id.channel = 0;   /*path id */
	camh->conn_id.target = pb->id;   /* adapter id */
	camh->conn_id.lun = 0;
	/* send the command */
	queue_insert(pb,&pb->ab->DACQ,camh,1);
	/* wait for the command to complete */
	return cam_wait_for_cmd_done(camh);
 }

/*+
 * ============================================================================
 * = sim_abort_scsi_io - abort a SCSI I/O command  =
 * ============================================================================
 *                 
 * OVERVIEW:
 *
 *	A previously issued Execute SCSI I/O command is aborted.  This is a 
 *	CAM defined command.      
 *	
 *
 * FORM OF CALL:
 *
 *	sim_abort_scsi_io()
 *
 * RETURNS:
 *
 *	msg_success		
 *	msg_failure		
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *
 * SIDE EFFECTS:
 *
 *      A command to a SCSI device may be aborted.
 *
-*/
int sim_abort_scsi_io(struct sim_pb *pb, unsigned int *abort_CCB, unsigned int *sim_CCB)
 {
	struct CAMHeader *camh;	
	struct ABORT_SCSI *param;

	/* fill in CAM header */
	camh = (struct CAMHeader *)sim_CCB;
	camh->function = XPT_ABORT_SCSI_CMD;
	camh->CCB_len = sizeof(struct CAMHeader) +
				sizeof(struct ABORT_SCSI);
	camh->cam_flags.dir = DIR_NO_XFER;
	camh->conn_id.channel = 0;   /*path id */
	camh->conn_id.target = pb->id;   /* adapter id */
	camh->conn_id.lun = 0;

	/* fill in address of CAM to be aborted */
	param = (struct ABORT_SCSI *)((unsigned char *)sim_CCB +
			sizeof(struct CAMHeader));
	param->abort_ccb_ptr = abort_CCB;
	/* send the command */
	queue_insert(pb,&pb->ab->DACQ,camh,1);
	/* wait for the command to complete */
	return cam_wait_for_cmd_done(camh);
 }                                                
#endif
/*+
 * ============================================================================
 * = sim_reset_scsi_bus - reset the bus of a SCSI channel  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *   	A bus associated with a particular SCSI channel will be reset.  This is
 *	a CAM defined command.  
 *	
 *
 * FORM OF CALL:
 *
 *	sim_reset_scsi_bus()
 *
 * RETURNS:
 *
 *	msg_success		- successful reset
 *	msg_failure		- unsuccessful reset
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *
 * SIDE EFFECTS:
 *
 *      A SCSI bus may be reset.
 *
-*/
void sim_reset_scsi_bus(struct sim_pb *pb)
 {
	struct CAMHeader *camh;	
	struct QueueCarrier *qc;

	camh = (struct CAMHeader *)sim_alloc_ccb(pb);
	camh->function = XPT_RESET_SCSI_BUS;
	camh->CCB_len = sizeof(struct CAMHeader);
	camh->cam_flags.dir = DIR_NO_XFER;
	camh->conn_id.channel = 0;   /*path id */
	camh->conn_id.target = pb->id;   /* adapter id */
	camh->conn_id.lun = 0;
	/* send the command */
	queue_insert(pb,&pb->ab->DACQ,camh,1);
#if 0
	/* the CCB winds up on the ADFQ, so remove it */
	while(queue_remove(pb,&pb->ab->ADFQ,&camh,&qc) != msg_success)
		krn$_sleep(10);
	sim_free(camh);
	sim_free(qc);
#endif
 }
#if 0
/*+
 * ============================================================================
 * = sim_reset_scsi_device - reset the device on a SCSI channel   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      A device on a particular SCSI channel will be reset.  This is a CAM
 *	defined command.
 *	
 *
 * FORM OF CALL:
 *
 *	sim_reset_scsi_device()
 *
 * RETURNS:
 *
 *	msg_success		- unsuccessful reset
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *	unsigned int *sim_CCB - pointer to command control block
 *
 * SIDE EFFECTS:
 *
 *      A SCSI device may be reset.
 *
-*/
int sim_reset_scsi_device(struct sim_pb *pb, unsigned int *sim_CCB)
 {
	struct CAMHeader *camh;	

	camh = (struct CAMHeader *)sim_CCB;
	camh->function = XPT_RESET_SCSI_DEVICE;
	camh->CCB_len = sizeof(struct CAMHeader);
	camh->cam_flags.dir = DIR_NO_XFER;
	camh->conn_id.channel = 0;   /*path id */
	camh->conn_id.target = pb->id;	/* adapter id */
	camh->conn_id.lun = 0;
	/* send the command */
	queue_insert(pb,&pb->ab->DACQ,camh,1);
	/* wait for the command to complete */
	return cam_wait_for_cmd_done(camh);
 }

/*+
 * ============================================================================
 * = sim_path_inquiry - obtain CAM and SCSI parameters  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *    	Prompt adapter to return the values of certain CAM and SCSI parameters.
 *	This is a CAM defined command.  
 *	
 *
 * FORM OF CALL:
 *
 *	sim_path_inquiry()
 *
 * RETURNS:
 *
 *	msg_success		
 *	msg_failure		
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to SIMPort port block
 *	unsigned int *sim_CCB - pointer to command control block
 *
 * SIDE EFFECTS:
 *
 *      SIMPort 
 *
-*/
int sim_path_inquiry(struct sim_pb *pb, unsigned int *sim_CCB)
 {
	struct CAMHeader *camh;	

	camh = (struct CAMHeader *)sim_CCB;
	camh->function = XPT_PATH_INQUIRY;
	camh->CCB_len = sizeof(struct CAMHeader) +
			sizeof(struct PATH_INQUIRY); 
	camh->cam_flags.dir = DIR_NO_XFER;
	camh->conn_id.channel = 0;   /*path id */
	camh->conn_id.target = pb->id;   /* adapter id */
	camh->conn_id.lun = 0;
	/* send the command */
	queue_insert(pb,&pb->ab->DACQ,camh,1);
	/* wait for the command to complete */
	return cam_wait_for_cmd_done(camh);
 }

/*+
 * ============================================================================
 * = sim_path_inquiry_response - response to path inquiry command  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  	
 *	
 *	
 *
 * FORM OF CALL:
 *
 *	sim_path_inquiry_response()
 *
 * RETURNS:
 *
 *	msg_success		
 *	msg_failure		
 *   
 * ARGUMENTS:
 *
 *	struct CAMHeader *camh - pointer to CAM header
 *	struct sim_pb *pb - pointer to SIMPort port block
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void sim_path_inquiry_response(struct CAMHeader *camh,struct sim_pb *pb)
 {
	struct PATH_INQUIRY * pi;
	int i;

	/* start of path inquiry data */
	pi = (struct PATH_INQUIRY *)((unsigned char *)camh +
		sizeof(struct CAMHeader));
	/* print the information to the console */
	printf("Path Inquiry data: \n");

	printf("Version: %d\n",pi->version);
	printf("SCSI capabilities...\n");
	if(pi->capabil & PI$M_SOFT_RESET)
		printf("\t soft reset \n"); 
	if(pi->capabil & PI$M_TAGGED_Q)
		printf("\t tagged queueing \n");
	if(pi->capabil & PI$M_LINKED_CMD)
		printf("\t linked commands \n");
	if(pi->capabil & PI$M_SYNC_XFER)
		printf("\t synchronous transfers \n");
	if(pi->capabil & PI$M_WIDE_16)
		printf("\t wide bus 16 \n");
	if(pi->capabil & PI$M_WIDE_32)
		printf("\t wide bus 32 \n");
	if(pi->capabil & PI$M_MOD_DATA_P)
		printf("\t modify data pointers \n");

	printf("Target mode support...\n");
	if(pi->tgt_mode & PI$M_GP_7)
		printf("\t group 7 cmds \n");
	if(pi->tgt_mode & PI$M_GP_6)
		printf("\t group 6 cmds \n");
	if(pi->tgt_mode & PI$M_TERM_IO)
		printf("\t terminate I/O process \n");
	if(pi->tgt_mode & PI$M_TGT_DISCON)
		printf("\t target mode disconnects \n");
	if(pi->tgt_mode & PI$M_PHAS_COG)
		printf("\t phase cognizant mode \n");
	if(pi->tgt_mode & PI$M_HOST_TGT)
		printf("\t host target mode \n");

	printf("HBA engine count:%04x\n",pi->eng_count);
	printf("private data area size:%d \n",pi->priv_data_size);

	printf("asynch event capabilities: \n");
	if(pi->async_event & PI$M_UNSOL_BUS_RESET)
		printf("\t unsolicited SCSI bus reset \n");
	if(pi->async_event & PI$M_UNSOL_RESEL)
		printf("\t unsolicited reselection \n");
	if(pi->async_event & PI$M_SCSI_AEN)
		printf("\t SCSI AEN \n");
	if(pi->async_event & PI$M_RESET_TGT)
		printf("\t sent bus device reset to target \n");
	if(pi->async_event & PI$M_SIM_MOD_REG)
		printf("\t SIM module registered \n");
	if(pi->async_event & PI$M_SIM_MOD_DREG)
		printf("\t SIM module de-registered \n");
	if(pi->async_event & PI$M_NEW_DEV)
		printf("\t new devices found during rescan \n");

	printf("highest path id:%02x \n",pi->high_path_id);
	printf("initiator SCSI id:%02x \n",pi->init_id);
	printf("SIM-supplier vendor id: \n");
	for(i=0; i<4;i++)
		printf("\t%08x\n",pi->sim_ven_id[i]);
	printf("host bus adapter-supplier vendor id: \n");
	for(i=0; i<4;i++)
		printf("\t%08x\n",pi->hba_ven_id[i]);

 }
#endif
/*+
 * ============================================================================
 * = sim_check_status - read status bit of command header  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *    	The status bit of the QueueCarrier is read after a command has been
 *	processed by the adapter.  The function returns msg_failure on error
 *	status and msg_success on successful completion of the command.
 *	
 *
 * FORM OF CALL:
 *
 *	sim_check_status()
 *
 * RETURNS:
 *
 *	msg_success		- successful completion of command
 *	msg_failure		- command unable to complete
 *   
 * ARGUMENTS:
 *
 *	struct QueueCarrier *qc- pointer to QueueCarrier
 *
 * SIDE EFFECTS:
 *
 *      None 
 *
-*/
int sim_check_status(struct QueueCarrier *qc)
 {
	if(qc->status == SPO_SUCCESS)
	 {
		return msg_success;
	 }
	if(qc->func_code != SPO_VU_CHAN_DISABLED)
	 {
		qprintf("error, qc->status = %x, qc->func_code = %x\n",qc->status,qc->func_code);
	 }
	return msg_failure;  
 }

/*+
 * ============================================================================
 * = cam_wait_for_cmd_done - polls for completion of CAM command  =
 * ============================================================================
 *
 * OVERVIEW:
 * 	
 *	A reserved Flag bit in the CAM Header is polled for command completion
 *	(state set).  If the command does not complete in the CMD_TIMEOUT 
 *	amount of time, a failure message is returned.
 *
 * FORM OF CALL:
 *
 *	cam_wait_for_cmd_done()
 *
 * RETURNS:
 *
 *	msg_success - command has been removed from the ADRQ
 *	msg_failure - command has timed out
 *   
 * ARGUMENTS:
 *
 *   	struct *CAMHeader camh - pointer to CAM header	
 *	
 *
 * SIDE EFFECTS:
 *
 *      
 *
-*/
int cam_wait_for_cmd_done(struct CAMHeader *camh)
    {
    int i;

#if SIM_DEBUG
	pprintf("cam_wait_for_cmd_done\n");
#endif

    for( i = 0; i < 4000; i++ )
	{
	if( camh->cam_flags.cmd_done )
	    return( msg_success );
	krn$_sleep( 10 );
	}
    return( msg_failure );
    }

/*+
 * ============================================================================
 * = sim_proc_int - process a SIMport interrupt  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	When a semaphore is posted indicating that an interrupt has occured, it 
 *	is determined whether it was caused by a command completion, or by an 
 *	adapter error.  If it was caused by a command completion, the 
 *	QueueCarrier is removed from the ADRQ.  If it was caused by an adapter 
 *	error, the error is logged and the adapter is reset.
 *
 * FORM OF CALL:
 *
 *	sim_proc_int()
 *
 * RETURNS:
 *
 *	None
 *	
 *   
 * ARGUMENTS:
 *
 *      struct sim_pb *pb - pointer to sim port block	
 *	
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void sim_proc_int(struct sim_pb *pb)
 {

	unsigned int asr;	  /* value read from ASR register */
	struct sim_regs *reg;
	struct CAMHeader *camh;   /* to be written by queue_remove() */
	struct QueueCarrier *qc;  /* to be written by queue_remove() */


	reg = (struct sim_regs *)(pb->sim_regs);

#if ( STARTSHUT || DRIVERSHUT )
    krn$_post(&pb->ready);		/* indicate interrupt process ready */
#endif

	while(1)
	 {
		krn$_wait(&pb->rx_s);

#if ( STARTSHUT || DRIVERSHUT )
/* Are we being shutdown ? */

		if (pb->pb.state != DDB$K_START)
		    return;
#endif
		/* determine whether interrupt is a Command Complete or */
		/* Adapter Miscellaneous interrupt. */

		/* command completed */
		if(pb->asr & ASR$M_ASIC)
	 	 {
			/* remove all elements from the queue */
			while(queue_remove(pb,&pb->ab->ADRQ,&camh,&qc) == msg_success)
				sim_proc_adrq(pb,camh,qc);
	 	 }
		else    /* adapter error (misc. interrupt) */	
	 	 {
			sim_dump_registers(pb->sim_regs,pb); /* DEBUG */
			/* log the error, */
			log_sim_error(pb,pb->asr,pb->afpr);
			pb->reset(pb->adap_pb); /* reset the adapter */
	 	 }
#if ( STARTSHUT || DRIVERSHUT )
	krn$_post(pb->sem);		/* post isr_t from higher level driver */
#endif
    	 }
 }

/*+
 * ============================================================================
 * = sim_proc_adrq - remove element(s) from the response q and dispatch  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This command is called from when an element is waiting to be removed
 *	from the ADRQ (Adapter-Driver Response Queue).  The message may be
 *	solicited or unsolicited.  
 *
 * FORM OF CALL:
 *
 *	sim_process_adrq()
 *
 * RETURNS:
 *
 *	
 *	
 *   
 * ARGUMENTS:
 *
 * 	struct sim_pb *pb - pointer to sim port block	
 *	struct CAMHeader *camh - pointer to CAM header
 *	struct QueueCarrier *qc - pointer to queue carrier
 *	
 *
 * SIDE EFFECTS:
 *
 *      Various, depending on what is removed from the response queue.   
 *
-*/
void sim_proc_adrq(struct sim_pb *pb, struct CAMHeader *camh,struct QueueCarrier *qc)
 { 

	unsigned int asr;	/* value read from ASR register */
	struct sim_regs *reg;

#if SIM_DEBUG
	pprintf("sim_proc_adrq\n");
#endif

	reg = (struct sim_regs *)(pb->sim_regs);
	/* init the camh flag to success */

	camh->cam_flags.sim_error = 0;

	/* first check for a simport command */
	if(qc->func_code >= 0x80)
	 {
		/* this dummy read appears to be necessary */
		asr = sim_read_long_csr(pb,&reg->asr_lo);
		
		/* check sim status for error */
		if(sim_check_status(qc) != msg_success)
		 {
			camh->cam_flags.sim_error = 1;
		 }
 	 }

	/* do we have an unsolicited message to process? */
	if( (qc->func_code > 0x85) && (qc->func_code < 0x8b) )
	 {
#if SIM_DEBUG	
             	qprintf("unsolic. msg: %02x\n",qc->func_code);
#endif
		switch(qc->func_code)
	 	 {
			case SPO_VU_BSD_REQUEST:
			case SPO_VU_BUS_RESET_REQ:
			case SPO_VU_UNSOL_RESEL:
					break;
			case SPO_VU_CHAN_DISABLED:
				{
					/* reenable the channel */
				sim_set_channel_state(pb,1,0,sim_alloc_ccb(pb));
					break;
				}
			case SPO_VU_DEV_DISABLED:
				{
					/* reenable the device */
					sim_set_device_state(pb,1,sim_alloc_ccb(pb));
					break;
				}
			default:
					qprintf("unknown unsolicited msg:%08x\n", qc->func_code);
					break;
		 }
		/* replenish DAFQ entry supply */
		sim_free(camh);  /* get rid of old entry */
		queue_insert(pb,&pb->ab->DAFQ,sim_alloc_ccb(pb),0);
	 }
	else
	 {
		/* regular response to cmd */
		/* this dummy read appears to be necessary */
		asr = sim_read_long_csr(pb,&reg->asr_lo);
		/* first copy the queue carrier status to the cam header */
		camh->status = qc->status;
		camh->cam_flags.cmd_done = 1;	/* signal command complete */
	 }
	sim_free(qc);   
 }

/*+
 * ============================================================================
 * = sim_poll - SIMPort polling routine   =
 * ============================================================================
 *
 * OVERVIEW:
 * 	
 *	The ASR (Adapter Status Register) is polled for errors and for the
 *    	indication that a queue element has been placed onto the ADRQ.  This
 *	routine is called from the timer task.  It will call the isr if there
 *	is an "interrupt".   
 *	
 *
 * FORM OF CALL:
 *
 *	sim_poll()
 *
 * RETURNS:
 *
 *	msg_success - command removed from ADRQ
 *	msg_failure - adapter error
 *	
 *   
 * ARGUMENTS:
 *
 *    	struct sim_pb *pb - pointer to sim port block	
 *	
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int sim_poll(struct sim_pb *pb)
 {
	unsigned int asr;	/* value read from ASR register */
	struct sim_regs *reg;
	struct CAMHeader *camh;
	struct QueueCarrier *qc;
	int done = 0;
	int count = 0;

	reg = (struct sim_regs *)(pb->sim_regs);
	while(!done)
	 {
		if(queue_remove(pb,&pb->ab->ADRQ,&camh,&qc) == msg_success)
		 {
			done = 1;
			break;
		 }	
		else
		 {
			krn$_sleep(100);
			count++;
		        if (count >= 5)
			 {
			      asr = sim_read_long_csr(pb,&reg->asr_lo);
				/* queue element on ADRQ or error */
			      if(asr & (ASR$M_ADSE | ASR$M_AMSE | ASR$M_AAC | ASR$M_AME))
				{
					qprintf("asr error: %08x\n",asr);
				}
			      return msg_failure;
			 }
		 }			
	 }
	sim_proc_adrq(pb,camh,qc);
	return msg_success;
 }


/*+
 * ============================================================================
 * = sim_poll1 - SIMPort polling routine   =
 * ============================================================================
 *
 * OVERVIEW:
 * 	
 *	The ASR (Adapter Status Register) is polled for errors and for the
 *    	indication that a queue element has been placed onto the ADRQ.  This
 *	routine is called from the timer task.  It will call the isr if there
 *	is an "interrupt".   
 *	
 *
 * FORM OF CALL:
 *
 *	sim_poll1()
 *
 * RETURNS:
 *
 *	nothing
 *   
 * ARGUMENTS:
 *
 *    	struct sim_pb *pb - pointer to sim port block	
 *	
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void sim_poll1(struct sim_pb *pb)
{
    unsigned int asr;	/* value read from ASR register */
    struct sim_regs *reg;
    struct CAMHeader *camh;
    struct QueueCarrier *qc;

    reg = (struct sim_regs *)(pb->sim_regs);

    asr = sim_read_long_csr(pb,&reg->asr_lo);
    /* queue element on ADRQ or error */

    if(asr & ASR$M_ASIC)
    {
	if(queue_remove(pb,&pb->ab->ADRQ,&camh,&qc) == msg_success)
	{
	    sim_proc_adrq(pb,camh,qc);
	}
	else if (asr & (ASR$M_ADSE | ASR$M_AMSE | ASR$M_AAC | ASR$M_AME))
	{
	    qprintf("asr error: %08x\n",asr);
	}
    }

}

/*+
 * ============================================================================
 * = log_sim_error - log an error mesasge  =
 * ============================================================================
 *
 * OVERVIEW:
 * 	An error message is logged.
 *	
 *      
 *	
 *
 * FORM OF CALL:
 *
 *	log_sim_error()
 *
 * RETURNS:
 *
 *	None
 *	
 *   
 * ARGUMENTS:
 *
 *   	struct sim_pb *pb - pointer to the sim port block
 *	unsigned int asr - contents of the asr register	
 *	unsigned int afpr - contents of the afpr register
 *	
 *
 * SIDE EFFECTS:
 *
 *	None      
 *
-*/
void log_sim_error(struct sim_pb *pb, unsigned int asr, unsigned int afpr)
 {
	struct sim_regs *reg;

	reg = (struct sim_regs *)(pb->sim_regs);

	qprintf("SIMport Adapter error: asr = %x, afpr = %x\n",asr,afpr);
 }

/*+
 * ============================================================================
 * = sim_cleanup - cleanup sim queues after an error  =
 * ============================================================================
 *
 * OVERVIEW:
 * 	
 *	
 *      
 *	
 *
 * FORM OF CALL:
 *
 *	sim_cleanup()
 *
 * RETURNS:
 *
 *	None
 *	
 *   
 * ARGUMENTS:
 *
 *	struct sim_pb *pb - pointer to sim port block
 *	
 *
 * SIDE EFFECTS:
 *
 *	Pending commands will find that the commands have been magically 
 *	completed (those commands will have semaphores posted).  The ADRQ will
 *	be emptied.
 *
-*/
void sim_cleanup(struct sim_pb *pb)
 {
	if( pb->ab )
	 {
	    sim_free_all(pb->malloc_queue); 
	    pb->malloc_queue = 0;
	    free(pb->ab);
	    pb->ab = 0;
	 }
 }

void sim_dump_registers(struct sim_regs *reg, struct sim_pb *pb)
 {
	unsigned __int64 val;

	val = sim_read_long_csr(pb,&reg->amcsr_lo);
	pprintf("amcsr_lo = %x\n",val);

	val = sim_read_long_csr(pb,&reg->abbr_lo);
	pprintf("abbrr_lo = %x\n",val);

	val = sim_read_long_csr(pb,&reg->dafqir_lo);
	pprintf("dafqir_lo = %x\n",val);

	val = sim_read_long_csr(pb,&reg->dacqir_lo);
	pprintf("dacqir_lo = %x\n",val);

	val = sim_read_long_csr(pb,&reg->asr_lo);
	pprintf("asr_lo = %x\n",val);

	val = sim_read_long_csr(pb,&reg->afar_lo);
	pprintf("afar_lo = %x\n",val);

	val = sim_read_long_csr(pb,&reg->afpr_lo);
	pprintf("afpr_lo = %x\n",val);
 }

unsigned __int64 sim_read_long_csr(struct sim_pb *pb, unsigned __int64 csr)
 {
	unsigned __int64 value;

	value = inmeml(pb,csr);
#if DEBUG
	pprintf("sim r %x %x\n", csr, value);
#endif
	return(value);
 }

void sim_write_long_csr(struct sim_pb *pb, unsigned __int64 csr, 
			unsigned __int64 value)
 {
#if DEBUG
	pprintf("sim w %x %x\n", csr, value);
#endif
	outmeml(pb,csr,value);
 }
