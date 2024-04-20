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
 * Abstract:	I2O exec services
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
#include "cp$src:i2oexec.h"
#include "cp$src:i2o_def.h"
#include "cp$src:i2o_pb_def.h"

#define __I2O__	1
protoif(__I2O__)
#include "cp$inc:prototypes.h"

/*
 * Routine prototypes
 */

I2O_ExecIopReset( struct i2o_pb *pb );
#if 0
I2O_ExecIopClear( struct i2o_pb *pb );
I2O_ExecSysQuiesce( struct i2o_pb *pb );
#endif
I2O_ExecLctNotify( struct i2o_pb *pb, int class_id, I2O_LCT *lct_head, 
							    int lct_size );
I2O_ExecSysEnable( struct i2o_pb *pb );
I2O_ExecSysTabSet( struct i2o_pb *pb );
#if 0
I2O_ExecHRTGet( struct i2o_pb *pb, int *buf, int bufsize );
#endif
I2O_prime_outbound( struct i2o_pb *pb );
I2O_ExecOutboundInit( struct i2o_pb *pb, I2O_EXEC_OUTBOUND_INIT_STATUS *buf );
I2O_ExecStatusGet( struct i2o_pb *pb, I2O_EXEC_STATUS_GET_REPLY *buf );


/*
 *********************************************************************
 *                                                                   *
 *  Start of DEBUG routines                                          *
 *                                                                   *
 *********************************************************************
 */

#if DEBUG

void i2o_dump_exec_status( struct _I2O_EXEC_STATUS_GET_REPLY *s ) {

    pprintf("Exec Status at %x\n", s );
    pprintf("\t OrganizationID: %x \n", s->OrganizationID );
    pprintf("\t IOP_ID: %x \n", s->IOP_ID );
    pprintf("\t HostUnitID: %x \n", s->HostUnitID );
    pprintf("\t SegmentNumber: %x \n", s->SegmentNumber );
    pprintf("\t I2oVersion: %x \n", s->I2oVersion );
    pprintf("\t IopState: %x \n", s->IopState );
    pprintf("\t MessengerType: %x \n", s->MessengerType );
    pprintf("\t InboundMFrameSize: %x \n", s->InboundMFrameSize );
    pprintf("\t InitCode: %x \n", s->InitCode );
    pprintf("\t MaxInboundMFrames: %x \n", s->MaxInboundMFrames );
    pprintf("\t CurrentMFrames: %x \n", s->CurrentInboundMFrames );
    pprintf("\t MaxOutboundMFrames: %x \n", s->MaxOutboundMFrames );
    pprintf("\t ProductIDString: %s \n", s->ProductIDString );
    pprintf("\t ExpectedLCTSize: %x \n", s->ExpectedLCTSize );
    pprintf("\t IopCapabilities: %x \n", s->IopCapabilities );
    pprintf("\t DesiredPrivateMemSize: %x \n", s->DesiredPrivateMemSize );
    pprintf("\t CurrentPrivateMemSize: %x \n", s->CurrentPrivateMemSize );
    pprintf("\t CurrentPrivateMemBase: %x \n", s->CurrentPrivateMemBase );
    pprintf("\t DesiredPrivateMemSize: %x \n", s->DesiredPrivateIOSize );
    pprintf("\t CurrentPrivateMemSize: %x \n", s->CurrentPrivateIOSize );
    pprintf("\t CurrentPrivateMemBase: %x \n", s->CurrentPrivateIOBase );
    pprintf("\t SyncByte: %x \n", s->SyncByte );
} 


void i2o_dump_lct ( struct _I2O_LCT *lct_head ) {

    I2O_LCT_ENTRY	  *lct;
    int		  i;
    int		  num_lct_entries;

    pprintf("Logical Configuration Table at %x\n", lct_head );
    pprintf("\t TableSize: %x \n", lct_head->TableSize );
    pprintf("\t BootDeviceTID: %x \n", lct_head->BootDeviceTID );
    pprintf("\t LctVer: %x \n", lct_head->LctVer );
    pprintf("\t IopFlags: %x \n", lct_head->IopFlags );
    pprintf("\t CurrentChangeIndicator: %x \n", lct_head->CurrentChangeIndicator );
    num_lct_entries = ( lct_head->TableSize - (( sizeof( I2O_LCT ) - sizeof( I2O_LCT_ENTRY )) / 4 )) 
				    / (sizeof( I2O_LCT_ENTRY ) / 4 );

    for ( i = 0; i < num_lct_entries; i++ ) {
	lct = &( lct_head->LCTEntry[i] );
	pprintf("\n\t Entry %d - ", i );
	switch ( lct->ClassID.Class ) {
	    case I2O_CLASS_EXECUTIVE:   
		pprintf("Executive");
		if ( lct->SubClassInfo == I2O_SUBCLASS_i960 )
		pprintf(" - i960\n");
		break;
	    case I2O_CLASS_DDM:
		pprintf("DDM");
		if ( lct->SubClassInfo == I2O_SUBCLASS_ISM ) {
		    pprintf(" - ISM\n");
		} else {
		    if ( lct->SubClassInfo == I2O_SUBCLASS_HDM ) {
			pprintf(" - HDM\n");
		    } else {
			pprintf("\n");
		     }
		}
		break;
	    case I2O_CLASS_RANDOM_BLOCK_STORAGE:
		pprintf("Block Storage\n");
		break;
	    case I2O_CLASS_SCSI_PERIPHERAL:
		pprintf("SCSI Peripheral\n");
		break;
	    case I2O_CLASS_BUS_ADAPTER_PORT:
		pprintf("Bus Adapter Port");
		if ( lct->SubClassInfo == I2O_SUBCLASS_SCSI ) {
		    pprintf(" - SCSI\n");
		} else {
		    pprintf("\n");
		}
		break;
	    case I2O_CLASS_SEQUENTIAL_STORAGE:
		pprintf("Sequential Storage\n");
		break;
	    default:
		pprintf("Unknown LCT Entry class ID\n");
		break;
	}
	pprintf("\t   TableEntrySize %x\n", lct->TableEntrySize );
	pprintf("\t   LocalTID %x\n", lct->LocalTID );
	pprintf("\t   ChangeIndicator %x\n", lct->ChangeIndicator );
	pprintf("\t   DeviceFlags %x\n", lct->DeviceFlags );
	pprintf("\t   ClassID %x\n", lct->ClassID );
	pprintf("\t   SubClassInfo %x\n", lct->SubClassInfo );
	pprintf("\t   UserTID %x\n", lct->UserTID );
	pprintf("\t   ParentTID %x\n", lct->ParentTID );
	pprintf("\t   BiosInfo %x\n", lct->BiosInfo );
	pprintf("\t   IdentityTag %s\n", &lct->IdentityTag[0] );
	pprintf("\t   EventCapabilities %x\n", lct->EventCapabilities );
    }
}
    
void i2o_dump_lct_notify( struct _I2O_EXEC_LCT_NOTIFY_MESSAGE *ln ) {

    pprintf("LCT notify at %x\n", ln );
    i2o_dump_mfa( &ln->StdMessageFrame );
    pprintf("\t TransactionContext %x\n", ln->TransactionContext );
    pprintf("\t ClassIdentifier %x\n", ln->ClassIdentifier );
    pprintf("\t LastReportedChangeIndicator %x\n", ln->LastReportedChangeIndicator );
    pprintf("\t SGL.u.Simple[0].FlagsCount.Count %x\n", ln->SGL.u.Simple[0].FlagsCount.Count );
    pprintf("\t SGL.u.Simple[0].FlagsCount.Flags %x\n", ln->SGL.u.Simple[0].FlagsCount.Flags );
    pprintf("\t SGL.u.Simple[0].PhysicalAddress %x\n", ln->SGL.u.Simple[0].PhysicalAddress );
}

#endif

/*
 *********************************************************************
 *                                                                   *
 *  End of DEBUG routines                                            *
 *                                                                   *
 *********************************************************************
 */

/*
 * All Executive class messages are single transaction messages.
 * Typically the sender sets the MessageFlags field for requests to
 *     
 *           0x00 for 32 bit context sizes
 *           0x02 for 64 bit context sizes
 */

#define I2O_EXEC_MESSAGEFLAGS  0x00



/*+
 * ============================================================================
 * = I2O_ExecStatusGet - Obtain the IOP's status			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Obtain IOP status.
 *
 *
 * FORM OF CALL:
 *  
 *	I2O_ExecStatusGet( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb - pointer to the i2o adapters port block
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int I2O_ExecStatusGet( struct i2o_pb *pb, struct _I2O_EXEC_STATUS_GET_REPLY *buf ) {

    int		    i;
    int		    iopstate;
    I2O_CNSL_MFHDR  *mfhdr;
    struct TIMERQ   *tq, tqe;
    I2O_EXEC_STATUS_GET_MESSAGE    *stsreq;

    tq = &tqe;
 
    dprintf("\n*** I2O_ExecStatusGet\n", p_args0 );
    memset( buf, 0, sizeof(I2O_EXEC_STATUS_GET_REPLY ));

    mfhdr  = i2o_malloc( sizeof( I2O_CNSL_MFHDR ) + 
			    sizeof( I2O_EXEC_STATUS_GET_MESSAGE ));
    stsreq = ( char * ) mfhdr + sizeof( I2O_CNSL_MFHDR );

    stsreq->VersionOffset          = I2O_VERSION_11;
    stsreq->MsgFlags               = I2O_EXEC_MESSAGEFLAGS; 
    stsreq->MessageSize            = sizeof( I2O_EXEC_STATUS_GET_MESSAGE ) / 4;
    stsreq->TargetAddress          = I2O_IOP_TID;
    stsreq->InitiatorAddress       = I2O_HOST_TID;
    stsreq->Function               = I2O_EXEC_STATUS_GET;
    stsreq->ReplyBufferAddressLow  = SYSADR( pb, buf );
    stsreq->ReplyBufferAddressHigh = 0;
    stsreq->ReplyBufferLength      = sizeof( I2O_EXEC_STATUS_GET_REPLY );
 
    mfhdr->context_type = I2O_CNSL_CONTEXT_NONE;

    I2O_MsgSend( pb, mfhdr );

    for ( i = 0; i < 100; i++ ) {
	if ( buf->SyncByte )
	    break;
	krn$_sleep( 100 );  
    }

#if DEBUG
    i2o_dump_exec_status( buf );
#endif

    iopstate = buf->IopState;
    dprintf("state: %x ( init=1,reset=2,hold=4,ready=5,op=8,fail=10,fault=11 )\n", iopstate );

    i2o_free( mfhdr );
    return ( iopstate );
}

/*+
 * ============================================================================
 * = I2O_ExecOutboundInit - Initialize the adapter FIFO with mfa's	      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *       Allocate a chunk of message frames.  Done contiguously for
 *       easy of programming.  We can find them all easily at driver
 *       shutdown. Give the adapter some reply frames to work with by
 *       posting them to the OUTBOUND FIFO.
 *
 *
 * FORM OF CALL:
 *  
 *	I2O_ExecOutboundInit( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb - pointer to the i2o adapters port block
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int I2O_ExecOutboundInit( struct i2o_pb *pb, struct _I2O_EXEC_OUTBOUND_INIT_STATUS *buf ) {
    I2O_CNSL_MFHDR                 *mfhdr;
    I2O_EXEC_OUTBOUND_INIT_MESSAGE *eoireq;
    int                            sgl_offset;

    dprintf("\n*** I2O_ExecOutboundInit - pb:%x *buf:%x\n", p_args2( pb, buf ));

    mfhdr = i2o_malloc( sizeof( I2O_CNSL_MFHDR ) + 
				sizeof( I2O_EXEC_OUTBOUND_INIT_MESSAGE ));
    eoireq = ( char * ) mfhdr + sizeof( I2O_CNSL_MFHDR );

    sgl_offset = offsetof( I2O_EXEC_OUTBOUND_INIT_MESSAGE, SGL ) / 4;
    i2o_message_frame( &eoireq->StdMessageFrame, sgl_offset, 
		     I2O_EXEC_MESSAGEFLAGS, 
		     sizeof( I2O_EXEC_OUTBOUND_INIT_MESSAGE ),
		     I2O_IOP_TID, I2O_HOST_TID, I2O_EXEC_OUTBOUND_INIT, mfhdr );

    eoireq->HostPageFrameSize                = 0x80;
    eoireq->InitCode                         = 0;
    eoireq->OutboundMFrameSize               = 0x20;
    eoireq->SGL.u.Simple[0].FlagsCount.Count = 
			    sizeof( I2O_EXEC_OUTBOUND_INIT_STATUS );
    eoireq->SGL.u.Simple[0].FlagsCount.Flags = I2O_SGL_SIMPLE_FLAGS;
    eoireq->SGL.u.Simple[0].PhysicalAddress  = SYSADR( pb, buf );
    mfhdr->context_type = I2O_CNSL_CONTEXT_NONE;

    I2O_MsgSend( pb, mfhdr );

    i2o_free( mfhdr );

    return msg_success;
}

/*+
 * ============================================================================
 * = I2O_prime_outbound - Give the IOP some memory to work with		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Give the IOP some memory to use for replies.
 *
 *
 * FORM OF CALL:
 *  
 *	I2O_prime_outbound( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb - pointer to the i2o adapters port block
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
void I2O_prime_outbound( struct i2o_pb *pb ) {

    int i;
    struct mfblock {
	char blk[I2O_CNSL_MF_SIZE];
    };
    int outbound;
    struct mfblock *mf_pool;

    mf_pool = pb->mf_pool;
    for( i = 0; i < I2O_CNSL_FIFO_SIZE; i++ ) {
	outbound = SYSADR( pb, mf_pool[i].blk );
	dprintf("Priming outbound with %x\n", p_args1( outbound ));
	I2O_write_mem( pb, OUTBOUND_FIFO, outbound );
    }
}

#if 0

/*+
 * ============================================================================
 * = I2O_ExecHRTGet - Obtain the hardware resource table		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Obtain the hardware resource table
 *
 * FORM OF CALL:
 *  
 *	I2O_ExecHRTGet( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb - pointer to the i2o adapters port block
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int I2O_ExecHRTGet( struct i2o_pb *pb, int *buf, int bufsize ) {

    I2O_CNSL_MFHDR                 *mfhdr;
    I2O_EXEC_HRT_GET_MESSAGE       *hrtreq;
    I2O_SINGLE_REPLY_MESSAGE_FRAME *hrtrsp;
    int                            sgl_offset;

    dprintf("\n*** I2O_ExecHRTGet - pb:%x *buf:%x bufsize: %x\n", 
                                       p_args3( pb, buf, bufsize) );

    mfhdr = i2o_malloc( sizeof( I2O_CNSL_MFHDR ) + 
			    sizeof( I2O_EXEC_HRT_GET_MESSAGE ));

    hrtreq = ( char * ) mfhdr + sizeof( I2O_CNSL_MFHDR );

    sgl_offset = offsetof( I2O_EXEC_HRT_GET_MESSAGE, SGL ) / 4;
    i2o_message_frame( &hrtreq->StdMessageFrame, sgl_offset, 
		     I2O_EXEC_MESSAGEFLAGS, 
		     sizeof( I2O_EXEC_HRT_GET_MESSAGE ),
		     I2O_IOP_TID, I2O_HOST_TID, I2O_EXEC_HRT_GET, mfhdr );

    hrtreq->SGL.u.Simple[0].FlagsCount.Count = bufsize;
    hrtreq->SGL.u.Simple[0].FlagsCount.Flags = I2O_SGL_SIMPLE_FLAGS;
    hrtreq->SGL.u.Simple[0].PhysicalAddress  = SYSADR( pb, buf );
    mfhdr->context_type = I2O_CNSL_CONTEXT_SEMAPHORE;

    I2O_MsgSend( pb, mfhdr );
    return ( I2O_MsgComplete( pb, mfhdr ));
}

#endif

/*+
 * ============================================================================
 * = I2O_ExecSysTabSet - Provide system configuration table enable peer op    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Provide system configuration table enable peer operation.
 *	Moves IOP from Hold to Ready state.
 *
 * FORM OF CALL:
 *  
 *	I2O_ExecSysTabSet( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb - pointer to the i2o adapters port block
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int I2O_ExecSysTabSet( struct i2o_pb *pb ) {

    I2O_CNSL_MFHDR                 *mfhdr;
    I2O_EXEC_SYS_TAB_SET_MESSAGE   *sysreq;
    I2O_SINGLE_REPLY_MESSAGE_FRAME *sysrsp;
    I2O_EXEC_STATUS_GET_REPLY      *execstatusget_buf;
    I2O_SET_SYSTAB_HEADER	   *systabhdr;
    I2O_IOP_ENTRY                  *systabentry;
    int				   *big_buf;    
    int                            sgl_offset;


    dprintf("\n*** I2O_ExecSysTabSet - pb:%x \n", p_args1( pb ) );


    big_buf = i2o_malloc( 2048 + sizeof( I2O_EXEC_STATUS_GET_REPLY ));
    execstatusget_buf = ( char * ) big_buf + 2048;
    I2O_ExecStatusGet( pb, execstatusget_buf );

    systabhdr = big_buf;
    systabhdr->NumberEntries = 1;
    systabhdr->SysTabVersion = I2O_RESOURCE_MANAGER_VERSION;
    systabhdr->CurrentChangeIndicator = 0;
    systabentry = execstatusget_buf;
    systabentry->IOP_ID = 1;
    systabentry->LastChanged = 0;
    systabentry->MessengerInfo.InboundMessagePortAddressLow = pb->mem_base+INBOUND_FIFO;
    systabentry->MessengerInfo.InboundMessagePortAddressHigh = 0;

    mfhdr = i2o_malloc( sizeof( I2O_CNSL_MFHDR ) + 
			    sizeof( I2O_EXEC_SYS_TAB_SET_MESSAGE ));

    sysreq = ( char * ) mfhdr + sizeof( I2O_CNSL_MFHDR );

    sgl_offset = offsetof( I2O_EXEC_SYS_TAB_SET_MESSAGE, SGL ) / 4;
    i2o_message_frame( &sysreq->StdMessageFrame, sgl_offset, 
		     I2O_EXEC_MESSAGEFLAGS, 
		     sizeof(I2O_EXEC_SYS_TAB_SET_MESSAGE),
		     I2O_IOP_TID, I2O_HOST_TID, I2O_EXEC_SYS_TAB_SET, mfhdr );

    sysreq->SGL.u.Simple[0].FlagsCount.Count = 
		sizeof( I2O_SET_SYSTAB_HEADER ) + sizeof( I2O_IOP_ENTRY );
    sysreq->SGL.u.Simple[0].FlagsCount.Flags = I2O_SGL_SIMPLE_FLAGS;
    sysreq->SGL.u.Simple[0].PhysicalAddress  = SYSADR( pb, systabhdr );
    mfhdr->context_type = I2O_CNSL_CONTEXT_SEMAPHORE;

    I2O_MsgSend( pb, mfhdr );

    i2o_free( big_buf );

    return ( I2O_MsgComplete( pb, mfhdr ));

}

/*+
 * ============================================================================
 * = I2O_ExecSysEnable - Release ExecSysQuiesce state and resume normal op    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Release ExecSysQuiesce state and resume normal operation.
 *	Move IOP from Ready to Operational.
 *
 * FORM OF CALL:
 *  
 *	I2O_ExecSysEnable( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb - pointer to the i2o adapters port block
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int I2O_ExecSysEnable( struct i2o_pb *pb ) {

    I2O_CNSL_MFHDR		    *mfhdr;
    I2O_EXEC_SYS_ENABLE_MESSAGE	    *enareq;
    I2O_SINGLE_REPLY_MESSAGE_FRAME  *enarsp;

    dprintf("\n*** I2O_ExecSysEnable - pb:%x \n", p_args1( pb ) );

    mfhdr = i2o_malloc( sizeof( I2O_CNSL_MFHDR ) + 
				sizeof( I2O_EXEC_SYS_ENABLE_MESSAGE ));

    enareq = ( char * ) mfhdr + sizeof( I2O_CNSL_MFHDR );

    i2o_message_frame( &enareq->StdMessageFrame, 0, 
		     I2O_EXEC_MESSAGEFLAGS, 
		     sizeof( I2O_EXEC_SYS_ENABLE_MESSAGE ),
		     I2O_IOP_TID, I2O_HOST_TID, I2O_EXEC_SYS_ENABLE, mfhdr );

    mfhdr->context_type = I2O_CNSL_CONTEXT_SEMAPHORE;

    I2O_MsgSend( pb, mfhdr );
    return ( I2O_MsgComplete( pb, mfhdr ));
}
/*+
 * ============================================================================
 * = I2O_ExecLctNotify - Send LCT notify to obtain Logical Config Table	      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Send LCT notify to obtain Logical Config Table.
 *
 * FORM OF CALL:
 *  
 *	I2O_ExecLctNotify( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb - pointer to the i2o adapters port block
 *	int class_id - class of entries to obtain
 *	I2O_LCT *lct_head - buffer for Logical Configuration Table
 *	int lct_size  - size of Logical Configuration Table
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int I2O_ExecLctNotify( struct i2o_pb *pb, int class_id, struct _I2O_LCT *lct_head, int lct_size ) {

    unsigned int		    sgl_offset;
    I2O_LCT_ENTRY		    *lct;
    I2O_CNSL_MFHDR		    *mfhdr;
    I2O_SINGLE_REPLY_MESSAGE_FRAME  *lnrsp;
    I2O_EXEC_LCT_NOTIFY_MESSAGE	    *ln;
    int				    i;    
    int				    *p;

    mfhdr = i2o_malloc( sizeof( I2O_CNSL_MFHDR ) + 
				sizeof( I2O_EXEC_LCT_NOTIFY_MESSAGE ));

    ln = ( char * ) mfhdr + sizeof( I2O_CNSL_MFHDR );

    mfhdr->context_type = I2O_CNSL_CONTEXT_SEMAPHORE;

    /*
     * Build the LCT notify message.
     */
    
    sgl_offset = offsetof( I2O_EXEC_LCT_NOTIFY_MESSAGE, SGL ) / 4;

    i2o_message_frame( &ln->StdMessageFrame, sgl_offset, 
		     I2O_EXEC_MESSAGEFLAGS, 
		     sizeof( I2O_EXEC_LCT_NOTIFY_MESSAGE ),
		     I2O_IOP_TID, I2O_HOST_TID, I2O_EXEC_LCT_NOTIFY, mfhdr );

    ln->ClassIdentifier = class_id;
    ln->LastReportedChangeIndicator = 0;

    ln->SGL.u.Simple[0].FlagsCount.Count = lct_size;
    ln->SGL.u.Simple[0].FlagsCount.Flags = I2O_SGL_SIMPLE_FLAGS;
    ln->SGL.u.Simple[0].PhysicalAddress = SYSADR( pb, lct_head );
#if DEBUG
    i2o_dump_lct_notify( ln );
#endif

    /*
     * Send the LCT notify message.
     */

    I2O_MsgSend( pb, mfhdr );
    return ( I2O_MsgComplete( pb, mfhdr ));
}

#if 0

/*+
 * ============================================================================
 * = I2O_ExecSysQuiesce - Stop sending messages and accept only system requests
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Stop sending messages and accept only system requests.
 *	Move IOP from Operational to Ready.
 *
 * FORM OF CALL:
 *  
 *	I2O_ExecSysQuiesce( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb - pointer to the i2o adapters port block
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int I2O_ExecSysQuiesce( struct i2o_pb *pb ) {

    I2O_CNSL_MFHDR		    *mfhdr;
    I2O_SINGLE_REPLY_MESSAGE_FRAME  *sqrsp;
    I2O_EXEC_SYS_QUIESCE_MESSAGE    *sq;

    mfhdr = i2o_malloc( sizeof( I2O_CNSL_MFHDR ) +
				sizeof( I2O_EXEC_SYS_QUIESCE_MESSAGE ));

    sq = ( char * ) mfhdr + sizeof( I2O_CNSL_MFHDR );

    mfhdr->context_type = I2O_CNSL_CONTEXT_SEMAPHORE;

    /*
     * Build the Sys Quiesce message.
     */
    
    i2o_message_frame( &sq->StdMessageFrame, 0, 
		     I2O_EXEC_MESSAGEFLAGS, 
		     sizeof( I2O_EXEC_SYS_QUIESCE_MESSAGE ),
		     I2O_IOP_TID, I2O_HOST_TID, I2O_EXEC_SYS_QUIESCE, mfhdr );
    /*
     * Send the Sys Quiesce message.
     */

    I2O_MsgSend( pb, mfhdr );
    return ( I2O_MsgComplete( pb, mfhdr ));
}

/*+
 * ============================================================================
 * = I2O_ExecIopClear - Abort all pending requests.
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Abort all pending requests without replying. Rebuild inbound 
 *	message queues and delete all entriesin external connection table.
 *	Move IOP to Hold state.
 *
 * FORM OF CALL:
 *  
 *	I2O_ExecIopClear( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb - pointer to the i2o adapters port block
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int I2O_ExecIopClear( struct i2o_pb *pb ) {

    I2O_CNSL_MFHDR		    *mfhdr;
    I2O_SINGLE_REPLY_MESSAGE_FRAME  *clrsp;
    I2O_EXEC_IOP_CLEAR_MESSAGE	    *cl;


    mfhdr = i2o_malloc( sizeof( I2O_CNSL_MFHDR ) + 
				sizeof( I2O_EXEC_IOP_CLEAR_MESSAGE ));

    cl = ( char * ) mfhdr + sizeof( I2O_CNSL_MFHDR );

    mfhdr->context_type = I2O_CNSL_CONTEXT_SEMAPHORE;

    /*
     * Build the IOP Clear message.
     */
    
    i2o_message_frame( &cl->StdMessageFrame, 0, 
		     I2O_EXEC_MESSAGEFLAGS, 
		     sizeof( I2O_EXEC_IOP_CLEAR_MESSAGE ),
		     I2O_IOP_TID, I2O_HOST_TID, I2O_EXEC_IOP_CLEAR, mfhdr );

    /*
     * Send the IOP Clear message.
     */

    pprintf("IOP clear\n");

    I2O_MsgSend( pb, mfhdr );
    return ( I2O_MsgComplete( pb, mfhdr ));
}

#endif

/*+
 * ============================================================================
 * = I2O_ExecIopReset - Reset the IOP.
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Reset the IOP. Abort all pending requests without reply. Rebuild 
 *	IOP envirnoment - reload IRTOS and resident DDM's.
 *
 * FORM OF CALL:
 *  
 *	I2O_ExecIopReset( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb - pointer to the i2o adapters port block
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int I2O_ExecIopReset( struct i2o_pb *pb ) {

    int				    status = msg_success;
    I2O_CNSL_MFHDR		    *mfhdr;
    I2O_EXEC_IOP_RESET_MESSAGE	    *reset;
    unsigned int		    reset_status = 0;
    int				    reset_count = 1000;

    mfhdr = i2o_malloc( sizeof( I2O_CNSL_MFHDR ) + 
			    sizeof( I2O_EXEC_IOP_RESET_MESSAGE ));

    reset = ( char * ) mfhdr + sizeof( I2O_CNSL_MFHDR );

    mfhdr->context_type = I2O_CNSL_CONTEXT_NONE;

    /*
     * Build the Reset message.
     */
    
    reset->VersionOffset = I2O_VERSION_11;  
    reset->MsgFlags = 0;
    reset->MessageSize = sizeof( I2O_EXEC_IOP_RESET_MESSAGE ) / 4;
    reset->InitiatorAddress = mfhdr;
    reset->TargetAddress    = I2O_IOP_TID; 
    reset->InitiatorAddress = I2O_HOST_TID; 
    reset->Function = I2O_EXEC_IOP_RESET;
    reset->StatusWordHighAddress = 0;
    reset->StatusWordLowAddress = SYSADR( pb, &reset_status );

    /*
     * Send the Reset message.
     */

#if DEBUG
    pprintf("Resetting IOP\n");
    pprintf("Reset %x\n", reset );
    pprintf("High %x %x Low %x %x\n", &reset->StatusWordHighAddress, reset->StatusWordHighAddress, 
				    &reset->StatusWordLowAddress, reset->StatusWordLowAddress );
#endif

    I2O_MsgSend( pb, mfhdr );

#if DEBUG
    pprintf("Reset message sent\n");
#endif

    while( !(*( volatile int *)&reset_status ) && ( reset_count-- > 0 ) ) { 
	krn$_sleep( 10 );
    }

    if (( reset_status & 0xF ) != I2O_EXEC_IOP_RESET_IN_PROGRESS ) {
	status = msg_failure;
    }

    i2o_free( mfhdr );

    return ( status );
}

protoendif(__I2O__)
