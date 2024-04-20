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
 * Abstract:	I2O message layer services
 *
 * Author:	Dennis Mazur
 *
 * Modifications:
 *
 *	dm	07-Aug-1997	Initial entry.
 */


/*
 *  Standard console includes
 */
#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:common.h"
#include "cp$src:stddef.h"
#include "cp$src:pb_def.h"
#include "cp$src:mem_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:ub_def.h"
#include "cp$inc:platform_io.h"
#include "cp$src:print_var.h"
#include "cp$src:ctype.h"

/*
 * I2O specific includes
 */
#include "cp$src:i2o_def.h"
#include "cp$src:i2o_pb_def.h"
#include "cp$src:i2omsg.h"

/*
 * External symbols
 */


/*
 * Routine prototypes
 */

I2O_write_mem( struct i2o_pb *pb, int offset, unsigned int data );
unsigned int I2O_read_mem ( struct i2o_pb *pb, int offset );
I2O_handle_reply( struct i2o_pb *pb );
I2O_interrupt( struct i2o_pb *pb );
I2O_MsgSend( struct i2o_pb *pb, I2O_CNSL_MFHDR *mfhdr );
I2O_ReplyFree( struct i2o_pb *pb, I2O_MESSAGE_FRAME *mfa );
i2o_message_frame( I2O_MESSAGE_FRAME *mf,  int sgl_offset, int flags,
		   int size, int target, int initiator, int function, 
		   I2O_CNSL_MFHDR *mfhdr );
I2O_MsgComplete( struct i2o_pb *pb, I2O_CNSL_MFHDR *mfhdr );



/*
 *********************************************************************
 *                                                                   *
 *  Start of DEBUG routines                                          *
 *                                                                   *
 *********************************************************************
 */

#if DEBUG

i2o_dump_reply ( I2O_SINGLE_REPLY_MESSAGE_FRAME *rfa ) {
    
    pprintf("Reply Frame at %x\n", rfa );
    i2o_dump_mfa( rfa );
    pprintf("\t TransactionContext: %x \n", (int)rfa->TransactionContext );
    pprintf("\t DetailedStatusCode: %x \n", (int)rfa->DetailedStatusCode );
    pprintf("\t ReqStatus: %x \n", (int)rfa->ReqStatus );
    pprintf("\t ReqStatus: %x \n", (int)rfa->ReqStatus );
    pprintf("\n");
}

i2o_dump_mfa ( I2O_MESSAGE_FRAME *mfa ) {

    pprintf("Message Frame at %x\n", mfa );
    pprintf("\t VersionOffset: %x\n", mfa->VersionOffset );
    pprintf("\t MsgFlags: %x \n", mfa->MsgFlags );
    pprintf("\t MessageSize: %x \n", mfa->MessageSize );
    pprintf("\t TargetAddress: %x \n", mfa->TargetAddress );
    pprintf("\t InitiatorAddress: %x \n", mfa->InitiatorAddress );
    pprintf("\t Function: %x \n", mfa->Function );
    pprintf("\t InitiatorContext: %x \n", (int)mfa->InitiatorContext );
    pprintf("\n");
}

i2o_dump_mfhdr ( I2O_CNSL_MFHDR *mfhdr ) {

    pprintf("Message Frame Header at %x\n", mfhdr );
    pprintf("\t request: %x \n", (char *) mfhdr + sizeof( I2O_CNSL_MFHDR ));
    pprintf("\t reply: %x \n", mfhdr->reply );
    pprintf("\t context_type: %x \n", mfhdr->context_type );
    pprintf("\t rsp_t: %x \n", mfhdr->rsp_t );
    pprintf("\t callback: %x \n", mfhdr->callback );
    pprintf("\n");
}

i2omsg_trace ( struct i2o_pb *pb, int op, int q, I2O_MESSAGE_FRAME *mfa ) {

    char optext[] = { '0', 'R', 'W' };
    int *mf = mfa;

    pprintf("%c %x %x(%x %x %x %x %x ) \n", optext[op], q, mfa, mf[0], mf[1],
                                                      mf[2], mf[3], mf[4] );
}

#endif

/*
 *********************************************************************
 *                                                                   *
 *  End of DEBUG routines                                            *
 *                                                                   *
 *********************************************************************
 */

/*+
 * ============================================================================
 * = I2O_message_frame -						      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Load a standard I2O message frame.
 *  
 * FORM OF CALL:
 *  
 *	I2O_message_frame( *mf, sgl_offset, flags,
 *		   size, target, initiator, function, *mfhdr )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	I2O_MESSAGE_FRAME *mf - pointer to message frame
 *	int sgl_offset - scatter gather list pointer
 *	int size - size of message
 *	int target - target ID
 *	int initiator - initiator ID
 *	int function - function code
 *	I2O_CNSL_MFHDR *mfhdr - Initiator Context
 *
 * SIDE EFFECTS:
 *
 *
-*/
i2o_message_frame( I2O_MESSAGE_FRAME *mf,  int sgl_offset, int flags,
		   int size, int target, int initiator, int function, 
		   I2O_CNSL_MFHDR *mfhdr ) {
    
    
    mf->VersionOffset    = ( sgl_offset << 4 ) | I2O_VERSION_11; 
    mf->MsgFlags         = flags; 
    mf->MessageSize      = size / 4;
    mf->TargetAddress    = target; 
    mf->InitiatorAddress = initiator; 
    mf->Function         = function;
    mf->InitiatorContext = mfhdr;

}

I2O_MsgComplete( struct i2o_pb *pb, I2O_CNSL_MFHDR *mfhdr ) {

    int status = msg_success;
    I2O_SINGLE_REPLY_MESSAGE_FRAME *rp;
    I2O_MESSAGE_FRAME *mfa;

    rp = mfhdr->reply;
    mfa = (char *) mfhdr + sizeof( I2O_CNSL_MFHDR );

    if ( rp ) {
	if ( rp->ReqStatus != I2O_REPLY_STATUS_SUCCESS ) {
	    pprintf("Request function %x has failed: %x|%x\n", mfa->Function, 
		    rp->ReqStatus, rp->DetailedStatusCode );
	    status = msg_failure; 
	}
	I2O_ReplyFree( pb, rp );
    }
    i2o_free( mfhdr );

    return ( status );
}

/*+
 * ============================================================================
 * = I2O_ReplyFree -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Tell the IOP that we are done with this reply by writing the 
 *	address of the reply to the outbound queue.
 *  
 * FORM OF CALL:
 *  
 *	I2O_ReplyFree( pb, mfa )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	pb - I2O port block pointer
 *	mfa - pointer to reply
 *
 * SIDE EFFECTS:
 *
 *
-*/
I2O_ReplyFree( struct i2o_pb *pb, I2O_MESSAGE_FRAME *mfa ) {

    int status;

    dprintf("I2O_ReplyFree - %x \n", p_args1( mfa ) );
    mfa = SYSADR( pb, mfa );
    I2O_write_mem( pb, OUTBOUND_FIFO, mfa );
}


/*+
 * ============================================================================
 * = I2O_MsgSend -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Send a request to the IOP.
 *  
 * FORM OF CALL:
 *  
 *	I2O_MsgSend( pb, mfhdr )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	pb - I2O port block pointer
 *	mfhdr - pointer to message frame header
 *
 * SIDE EFFECTS:
 *
 *
-*/
I2O_MsgSend( struct i2o_pb *pb, I2O_CNSL_MFHDR *mfhdr ) {

    int i;
    I2O_MESSAGE_FRAME *request, *maprequest;
    char semtitle[20];
    int *inbound;
    int post;

    request = (char *) mfhdr + sizeof( I2O_CNSL_MFHDR );
    
    /*
     * Obtain a request frame from the IOP. Copy the user supplied 
     * request into the request frame supplied by the IOP. 
     */

    inbound = ( void* ) I2O_read_mem( pb, INBOUND_FIFO );
    dprintf("FIFO read %x\n", p_args1(inbound) );
    for ( i = 0; i < 32; i++ ) {
	I2O_write_mem( pb, &inbound[i], (( int* ) request ) [i] );
    }

    /*
     * Check that the Initiator Context field of the request has
     * been setup.
     */

    if ( request->InitiatorContext != mfhdr ) {
	dprintf("Initiator context needs to be set up\n", p_args0 );
	request->InitiatorContext = mfhdr;
    }

    /*
     * Write the request to the IOP's inbound queue.
     * The context type determines how the request will be handled.
     * For each request an interrupt will be generated. The interrupt
     * will post handle_reply. The handle_reply process will handle the 
     * reply based on the context specified in the message frame header.
     * For semaphore context, wait for the semaphore to be posted by the 
     * handle_reply process. For callback context, the handle_reply 
     * process executes the callback.
     */

    switch( mfhdr->context_type ) {

	case I2O_CNSL_CONTEXT_SEMAPHORE:
	    start_timeout( I2O_REQUEST_TIMEOUT, &mfhdr->rsp_t );
	    I2O_write_mem( pb, INBOUND_FIFO, inbound );
	    post = krn$_wait( &mfhdr->rsp_t.sem );
	    if ( post != I2O_REQUEST_SUCCESS ) {
		pprintf("I2O request timed out - function %x pb %x mfhdr %x\n",
				request->Function, pb, mfhdr );
	    }
	    stop_timeout( &mfhdr->rsp_t );
	    break;
	case I2O_CNSL_CONTEXT_CALLBACK:
	    I2O_write_mem( pb, INBOUND_FIFO, inbound );
	    break;
	case I2O_CNSL_CONTEXT_NONE:
	    I2O_write_mem( pb, INBOUND_FIFO, inbound );
	    break;
    }
}

/*+
 * ============================================================================
 * = I2O_interrupt -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Post the process that handles all replies.
 *  
 * FORM OF CALL:
 *  
 *	I2O_interrupt( pb )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	pb - I2O port block pointer
 *
 * SIDE EFFECTS:
 *
 *
-*/
I2O_interrupt( struct i2o_pb *pb ) {

    int int_mask;

    /*
     * Disable Outbound Post List interrupt in Interrupt Mask Register
     */

    int_mask = I2O_read_mem( pb, I2O_INTERRUPT_MASK_REG );
    int_mask = int_mask | I2O_OUTBOUND_INTERRUPT;
    I2O_write_mem( pb, I2O_INTERRUPT_MASK_REG, int_mask );

    /*
     * Post the process that will handle replies from the IOP.
     * When the process is done processing replies it will reenable 
     * the interrupt.
     */

    if( pb->pb.vector ) {
	io_issue_eoi( pb, pb->pb.vector );
    }

    krn$_post( &pb->reply_s );
}

/*+
 * ============================================================================
 * = I2O_handle_reply -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Process that handles all replies. It is posted by the interrupt
 *	handler.
 *  
 * FORM OF CALL:
 *  
 *	I2O_handle_reply( pb )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	pb - I2O port block pointer
 *
 * SIDE EFFECTS:
 *
 *
-*/
I2O_handle_reply( struct i2o_pb *pb ) {

    I2O_MESSAGE_FRAME *mapreply, *reply;
    I2O_CNSL_MFHDR *mfhdr;
    int int_mask;
    struct PCB *pcb;

    pcb = getpcb();

    while ( 1 ) {

	krn$_wait( &pb->reply_s );

	if ( pcb->pcb$l_killpending & SIGNAL_KILL )
	    return ( msg_success );

	/*
	 * Read the outbound queue for a reply.
	 */


	mapreply = ( void* ) I2O_read_mem( pb, OUTBOUND_FIFO );
    
	/*
	 * Process all replies on the outbound queue.
	 */

	while( mapreply != I2O_EMPTY_FIFO ) {
	
	    /*
	     * On Rawhide the window base is equal to the heap base so that
	     * the | of the window base has no effect on the address so there's
	     * no need to subtract it out.
	     */

#if RAWHIDE
	    reply = ( int ) mapreply;
#else
	    reply = ( int ) mapreply - io_get_window_base( pb );
#endif

	    dprintf("Interrupt - mfa:%x\n", p_args1( mapreply ) );

	    /*
	     * Obtain the message frame header from the reply's Initiator context
	     * field. This field is loaded by the IOP from the request Initiator
	     * context field. This supplies an association between the request and
	     * the reply. From this we obtain the context type to determine 
	     * whether we post a semaphore, execute a callback, etc.
	     */
    
	    mfhdr = reply->InitiatorContext;
	    dprintf("\nInitiatorContext: %x mapreply:%x\n", 
			    p_args2( reply->InitiatorContext, mapreply ) );
	    mfhdr->reply = reply;

	    /*
	     *  For semaphore context, post the semaphore. For callback 
	     *  context call the callback routine supplied by the request.
	     */

	    switch( mfhdr->context_type ) {
		case I2O_CNSL_CONTEXT_NONE:
		    break;
		case I2O_CNSL_CONTEXT_CALLBACK:
		    mfhdr->callback( pb, mfhdr );
		    break;
		case I2O_CNSL_CONTEXT_SEMAPHORE:
		    krn$_post( &mfhdr->rsp_t.sem, I2O_REQUEST_SUCCESS );
		    break;
	    }
    
	    /*
	     * Read the outbound queue for another reply.
	     */

	    mapreply = ( void* ) I2O_read_mem( pb, OUTBOUND_FIFO );

	} /* while */

	/*
	 * Enable Outbound Post List interrupt in Interrupt Mask Register
	 */

	int_mask = I2O_read_mem( pb, I2O_INTERRUPT_MASK_REG );
	int_mask = int_mask & ~I2O_OUTBOUND_INTERRUPT;
	I2O_write_mem( pb, I2O_INTERRUPT_MASK_REG, int_mask );
    }
}

/*+
 * ============================================================================
 * = I2O_read_mem -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Read the adapter's PCI space
 *  
 * FORM OF CALL:
 *  
 *	I2O_read_mem( pb, offset )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	pb - I2O port block pointer
 *	offset - offset into adapter's space
 *
 * SIDE EFFECTS:
 *
 *
-*/
unsigned int I2O_read_mem ( struct i2o_pb *pb, int offset ) {
    
    unsigned int retval;

#if DEBUG
    stack_check();
#endif
    retval = inmeml( pb, pb->mem_base+offset );
    return retval;
}


/*+
 * ============================================================================
 * = I2O_write_mem -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Write adapter's PCI space
 *  
 * FORM OF CALL:
 *  
 *	I2O_write_mem( pb, offset )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	pb - I2O port block pointer
 *	offset - offset into adapter's space
 *
 * SIDE EFFECTS:
 *
 *
-*/
I2O_write_mem( struct i2o_pb *pb, int offset, unsigned int data ) {

#if DEBUG
    stack_check();
#endif
    outmeml( pb, pb->mem_base+offset, data );
}


