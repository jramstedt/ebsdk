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
 * Abstract:	I2O utility services
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
#include "cp$inc:kernel_entry.h"

/*
 * I2O specific includes
 */
#include "cp$src:i2odep.h"
#include "cp$src:i2o_def.h"
#include "cp$src:i2o_pb_def.h"
#include "cp$src:i2outil.h"

#define __I2O__	1
protoif(__I2O__)
#include "cp$inc:prototypes.h"

/*
 * Routine prototypes
 */

I2O_UtilParamsGet( struct i2o_pb *pb, int tid, int cmd_size, 
		    I2O_PARAM_OPERATIONS_LIST_HEADER *cmd_header, 
		    int group_size, char *group_buf );
I2O_GetParamGroup( struct i2o_pb *pb, int tid, int group_num, int group_size, 
							    char *group_buf );
I2O_UtilEventRegister( struct i2o_pb *pb, int tid, unsigned int mask, 
							int handler );

#if 0
I2O_UtilClaim( struct i2o_pb *pb, int tid, int claim_type, int claim_flags );
#endif

/*
 *********************************************************************
 *                                                                   *
 *  Start of DEBUG routines                                          *
 *                                                                   *
 *********************************************************************
 */

#if DEBUG

void i2o_dump_util_claim( struct _I2O_UTIL_CLAIM_MESSAGE *cl ) {

    pprintf("Util claim at %x\n", cl );
    i2o_dump_mfa( &cl->StdMessageFrame );
    pprintf("\t TransactionContext %x\n", cl->TransactionContext );
    pprintf("\t ClaimFlags %x\n", cl->ClaimFlags );
    pprintf("\t ClaimType %x\n", cl->ClaimType );
}


void i2o_dump_util_params_get( struct _I2O_UTIL_PARAMS_GET_MESSAGE *pg ) {

    pprintf("Util Params Get at %x\n", pg );
    i2o_dump_mfa( &pg->StdMessageFrame );
    pprintf("\t TransactionContext %x\n", pg->TransactionContext );
    pprintf("\t SGL.u.Simple[0].FlagsCount.Count %x\n", pg->SGL.u.Simple[0].FlagsCount.Count );
    pprintf("\t SGL.u.Simple[0].FlagsCount.Flags %x\n", pg->SGL.u.Simple[0].FlagsCount.Flags );
    pprintf("\t SGL.u.Simple[0].PhysicalAddress %x\n", pg->SGL.u.Simple[0].PhysicalAddress );
    pprintf("\t SGL.u.Simple[1].FlagsCount.Count %x\n", pg->SGL.u.Simple[1].FlagsCount.Count );
    pprintf("\t SGL.u.Simple[1].FlagsCount.Flags %x\n", pg->SGL.u.Simple[1].FlagsCount.Flags );
    pprintf("\t SGL.u.Simple[1].PhysicalAddress %x\n", pg->SGL.u.Simple[1].PhysicalAddress );

}

void i2o_dump_util_get_param_op( struct _I2O_PARAM_OPERATIONS_LIST_HEADER *cmd_header ) {
    I2O_PARAM_OPERATION_SPECIFIC_TEMPLATE *cmd;

    /*
     * Currently expecting only one operation.
     */

    pprintf("Util Get Params oplist at %x\n", cmd_header );
    pprintf("\t OperationCount %x\n", cmd_header->OperationCount );
    cmd = ( char * ) cmd_header + sizeof( *cmd_header );
    pprintf("\t Operation %x\n", cmd->Operation );
    pprintf("\t GroupNumber %x\n", cmd->GroupNumber );
    pprintf("\t FieldCount %x\n", cmd->FieldCount );
}

void i2o_dump_util_get_param_result( struct _I2O_PARAM_RESULTS_LIST_HEADER *result_header ) {
    I2O_PARAM_READ_OPERATION_RESULT *result;

    pprintf("Util Get Params result at %x\n", result_header );
    pprintf("\t OperationCount %x\n", result_header->ResultCount );
    result = ( char * ) result_header + sizeof( *result_header );
    pprintf("\t BlockSize %x\n", result->BlockSize );
    pprintf("\t BlockStatus %x\n", result->BlockStatus );
    pprintf("\t ErrorInfoSize %x\n", result->ErrorInfoSize );
}

#endif


#if 0
/*+
 * ============================================================================
 * = I2O_UtilClaim - request claim of the target.
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Request claim of the target.
 *
 * FORM OF CALL:
 *  
 *	I2O_UtilClaim( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb   - pointer to the i2o adapters port block
 *	int tid		    - target ID
 *	int claim_type	    - type of user making claim 
 *	int claim_flag	    - claim flags
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int I2O_UtilClaim( struct i2o_pb *pb, int tid, int claim_type, int claim_flags ) {

    I2O_UTIL_CLAIM_MESSAGE	    *cl;
    I2O_CNSL_MFHDR		    *mfhdr;
    I2O_SINGLE_REPLY_MESSAGE_FRAME  *clrsp;
    int status;

    mfhdr = i2o_malloc( sizeof( I2O_CNSL_MFHDR ) + 
				sizeof( I2O_UTIL_CLAIM_MESSAGE ));

    cl = ( char * ) mfhdr + sizeof( I2O_CNSL_MFHDR );

    mfhdr->context_type = I2O_CNSL_CONTEXT_SEMAPHORE;

    /*
     * Build the Util Claim message.
     */
    
    i2o_message_frame( &cl->StdMessageFrame, 0, 0, 
		       sizeof(I2O_UTIL_CLAIM_MESSAGE),
		       tid, I2O_HOST_TID, I2O_UTIL_CLAIM, mfhdr );

    cl->ClaimFlags = claim_flags;
    cl->ClaimType = claim_type;

#if DEBUG
    i2o_dump_util_claim( cl );
#endif

    /*
     * Send the Util Claim message.
     */

    I2O_MsgSend( pb, mfhdr );
    return ( I2O_MsgComplete( pb, mfhdr ));
}
#endif

/*+
 * ============================================================================
 * = I2O_UtilEventRegister - register a handler for IOP events.
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Register a handler for IOP events.
 *
 * FORM OF CALL:
 *  
 *	I2O_UtilEventRegister( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb   - pointer to the i2o adapters port block
 *	int tid		    - target ID
 *	unsigned int mask   - which events
 *	int handler	    - event handler
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int I2O_UtilEventRegister( struct i2o_pb *pb, int tid, unsigned int mask, int handler ) {

    I2O_UTIL_EVENT_REGISTER_MESSAGE   *ev;
    I2O_CNSL_MFHDR		    *mfhdr;
    I2O_SINGLE_REPLY_MESSAGE_FRAME    *evrsp;

    mfhdr = i2o_rq_malloc( pb, sizeof( I2O_CNSL_MFHDR ) + 
				sizeof( I2O_UTIL_EVENT_REGISTER_MESSAGE ));
    ev = ( char * ) mfhdr + sizeof( I2O_CNSL_MFHDR );

    mfhdr->context_type = I2O_CNSL_CONTEXT_CALLBACK;
    mfhdr->callback = handler;

    /*
     * Build the Event Register message.
     */
    
    i2o_message_frame( &ev->StdMessageFrame, 0, 0, 
		       sizeof(I2O_UTIL_EVENT_REGISTER_MESSAGE),
		       tid, I2O_HOST_TID, I2O_UTIL_EVENT_REGISTER, mfhdr );

    ev->EventMask = mask;

    /*
     * Send the Event Register message.
     */

    I2O_MsgSend( pb, mfhdr );

    return ( msg_success );

}

/* Maximum result error size ? */

#define MAX_RESULT_ERROR_SIZE 16

/*
 * Get an entire parameter group.
 */


/*+
 * ============================================================================
 * = I2O_GetParamGroup - get a parameter group.
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	 Get a parameter group.
 *
 * FORM OF CALL:
 *  
 *	I2O_GetParamGroup( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb	- port block pointer
 *	int tid			- target ID
 *	int group_num		- group number
 *	int group_size		- group size
 *	char *group_buf		- group buf
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int I2O_GetParamGroup( struct i2o_pb *pb, int tid, int group_num, int group_size, char *group_buf ) {

    I2O_PARAM_OPERATIONS_LIST_HEADER *cmd_header;
    I2O_PARAM_OPERATION_SPECIFIC_TEMPLATE *cmd;
    int *p;
    int cmd_size = sizeof( *cmd ) + sizeof( *cmd_header );
    int status = msg_failure;

    /*
     * Get the entire parameter group by specifying a field count of negative one.
     */

    cmd_header = i2o_malloc( cmd_size );
    cmd_header->OperationCount = 1;
    cmd = (char *) cmd_header + sizeof( *cmd_header );
    cmd->Operation = I2O_PARAMS_OPERATION_FIELD_GET;
    cmd->GroupNumber = group_num;
    cmd->FieldCount = -1;

#if DEBUG
    i2o_dump_util_get_param_op( cmd_header );
#endif

    status = I2O_UtilParamsGet( pb, tid, cmd_size, cmd_header, group_size, group_buf );

    /* 
     * Need to process result error information ( if any ).
     */

    i2o_free( cmd_header );

    return ( status );
}

/*+
 * ============================================================================
 * =  I2O_UtilParamsGet - get parameters.
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	 Get parameters.
 *
 * FORM OF CALL:
 *  
 *	I2O_UtilParamsGet( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb			     - port block pointer
 *	I2O_PARAM_OPERATIONS_LIST_HEADER *cmd_header - command header
 *	int group_num				     - group number
 *	int group_size				     - group size
 *	char *group_buf				     - group buf
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int I2O_UtilParamsGet( struct i2o_pb *pb, int tid, int cmd_size, 
		    struct _I2O_PARAM_OPERATIONS_LIST_HEADER *cmd_header, 
		    int group_size, char *group_buf ) {

    unsigned int		    SGL_offset;
    I2O_CNSL_MFHDR		    *mfhdr;
    I2O_SINGLE_REPLY_MESSAGE_FRAME  *pgrsp;
    I2O_UTIL_PARAMS_GET_MESSAGE	    *pg;
    int				    pg_size;
    I2O_PARAM_RESULTS_LIST_HEADER   *result_header;
    I2O_PARAM_READ_OPERATION_RESULT *result;
    char *p;

    /*
     * Need a ParamsGet message frame plus and extra SGL buffer. One buffer
     * for the command, one buffer for the reply.
     */

    pg_size = sizeof( *pg ) + sizeof( I2O_SGE_SIMPLE_ELEMENT );

    mfhdr = i2o_malloc( sizeof( I2O_CNSL_MFHDR ) + pg_size );

    pg = ( char * ) mfhdr + sizeof( I2O_CNSL_MFHDR );

    mfhdr->context_type = I2O_CNSL_CONTEXT_SEMAPHORE;

    /*
     * Build the ParamsGet message.
     */
    
    SGL_offset = offsetof( I2O_UTIL_PARAMS_GET_MESSAGE, SGL ) / 4;
    i2o_message_frame( &pg->StdMessageFrame, SGL_offset, 0, 
		       sizeof(I2O_UTIL_PARAMS_GET_MESSAGE),
		       tid, I2O_HOST_TID, I2O_UTIL_PARAMS_GET, mfhdr );

    pg->SGL.u.Simple[0].FlagsCount.Count = cmd_size;
    pg->SGL.u.Simple[0].FlagsCount.Flags = I2O_SGL_SIMPLE_FLAGS;
    pg->SGL.u.Simple[0].PhysicalAddress = SYSADR( pb, cmd_header );

#if DEBUG
    i2o_dump_util_params_get( pg );
#endif
    /*
     * Send the GetParams message.
     */

    I2O_MsgSend( pb, mfhdr );

    pgrsp = mfhdr->reply;
  
    result = ( char * ) pgrsp + sizeof( *pgrsp ) + sizeof( *result_header );
  
    if ( result->BlockStatus != 0 ) 
	dprintf("GetParam Error BlockStatus %x\n", p_args1( result->BlockStatus ) );

    p = ( char * ) result + sizeof( *result );
    memcpy( group_buf, p, group_size );

    return ( I2O_MsgComplete( pb, mfhdr ));

}

protoendif(__I2O__)
