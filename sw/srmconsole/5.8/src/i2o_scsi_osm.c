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
 * Abstract:	I2O SCSI OSM
 *
 * Author:	Kevin LeMieux
 *
 * Modifications:
 *
 */

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:scsi_def.h"
#include "cp$src:common.h"
#include "cp$src:stddef.h"
#include "cp$src:pb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:ub_def.h"
#include "cp$inc:platform_io.h"
#include "cp$inc:kernel_entry.h"
#if MODULAR
#include "cp$src:mem_def.h"
#endif
#include "cp$src:i2o_pb_def.h"
#include "cp$src:i2obscsi.h"
#include "cp$src:i2oadptr.h"
#include "cp$src:i2oexec.h"
#include "cp$src:i2omsg.h"
#include "cp$src:i2o_def.h"

#define __I2O__	1
protoif(__I2O__)
#include "cp$inc:prototypes.h"

#if ( MODULAR ) 
#define DRIVERSHUT 1
#endif

extern struct FILE *el_fp;

i2o_scsi_init( struct i2o_pb *pb, int tid );
i2o_scsi_shutdown( struct scsi_sb *sb );
i2o_scsi_command( struct scsi_sb *sb, int lun, unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int dat_out_len,
	unsigned char *dat_in, int dat_in_len, unsigned char *sts,
	unsigned char *sense_dat_in, int *sense_dat_in_len );
i2o_scsi_new_unit( struct i2o_pb *pb, int sb_index, I2O_LCT_ENTRY *lct, int verbose  );


struct osm i2o_scsi_osm = { I2O_CLASS_SCSI_PERIPHERAL,
			    I2O_SUBCLASS_SCSI,i2o_scsi_new_unit,
			    i2o_scsi_shutdown,
			    i2o_scsi_init };

/*
 *********************************************************************
 *                                                                   *
 *  Start of DEBUG routines                                          *
 *                                                                   *
 *********************************************************************
 */

#if DEBUG

void i2o_dump_scsi_execute( struct _I2O_SCSI_SCB_EXECUTE_MESSAGE *sc, int lun ) {
    
    int i;

    if ( lun != 0 ) return;

    pprintf("SCSI Execute at %x\n", sc );
    i2o_dump_mfa( &sc->StdMessageFrame );
    pprintf("\t TransactionContext %x\n", sc->TransactionContext );
    pprintf("\t CDBLength  %x\n", sc->CDBLength );
    for (i = 0; i < sc->CDBLength; i++)
	pprintf("	cdb[%d] %x\n", i, sc->CDB[i]);    
    pprintf("\t SCBFlags  %x\n", sc->SCBFlags );
    pprintf("\t ByteCount  %x\n", sc->ByteCount );
    pprintf("\t SGL.u.Simple[0].FlagsCount.Count %x\n", sc->SGL.u.Simple[0].FlagsCount.Count );
    pprintf("\t SGL.u.Simple[0].FlagsCount.Flags %x\n", sc->SGL.u.Simple[0].FlagsCount.Flags );
    pprintf("\t SGL.u.Simple[0].PhysicalAddress %x\n", sc->SGL.u.Simple[0].PhysicalAddress );
    pprintf("\n");    
}

void i2o_dump_scsi_reply( struct _I2O_SCSI_ERROR_REPLY_MESSAGE_FRAME *sr, int lun ) {
    
    int i;
    unsigned int device_status;
    unsigned int adapter_status;

    if ( lun != 0 ) return;

    pprintf("SCSI Reply message at %x\n", sr );
    i2o_dump_reply( &sr->StdReplyFrame );
    device_status = sr->StdReplyFrame.DetailedStatusCode & I2O_SCSI_DEVICE_DSC_MASK;
    adapter_status = sr->StdReplyFrame.DetailedStatusCode & I2O_SCSI_HBA_DSC_MASK;
    
    if ( adapter_status != I2O_SCSI_HBA_DSC_SUCCESS ) {    
	pprintf("Adapter Status %x\n", adapter_status );
	pprintf("SCSI Status %x\n", device_status );
    }

    pprintf("\t TransferCount %x\n", sr->TransferCount );
    pprintf("\t AutoSenseTransferCount %x\n", sr->AutoSenseTransferCount );
    for (i = 0; i < sr->AutoSenseTransferCount; i++)    
	pprintf("\t SenseData[%d] %x\n", i, sr->SenseData[i]);
    pprintf("\n");
}

void i2o_scsi_dump_grp0( struct _I2O_SCSI_DEVICE_INFO_SCALAR *sc ) {

    pprintf("SCSI peripheral param group 0 at %x\n", sc );
    pprintf("\t DeviceType %x\n", sc->DeviceType );
    pprintf("\t Flags %x\n", sc->Flags );
    pprintf("\t Identifier %x\n", sc->Identifier );
    pprintf("\t LunInfo[8] %x\n", *sc->LunInfo );
    pprintf("\t QueueDepth %x\n", sc->QueueDepth );
    pprintf("\t NegOffset %x\n", sc->NegOffset );
    pprintf("\t NegDataWidth %x\n", sc->NegDataWidth );
    pprintf("\t NegSyncRate.HighPart %x\n", sc->NegSyncRate.HighPart );
    pprintf("\t NegSyncRate.LowPart %x\n", sc->NegSyncRate.LowPart );
}

void i2o_scsi_dump_grp200( struct _I2O_HBA_SCSI_CONTROLLER_INFO_SCALAR *sc ) {

    pprintf("SCSI param group 200 at %x\n", sc );
    pprintf("\t SCSIType %x\n", sc->SCSIType );
    pprintf("\t ProtectionManagement %x\n", sc->ProtectionManagement );
    pprintf("\t Settings %x\n", sc->Settings );
    pprintf("\t InitiatorID %x\n", sc->InitiatorID );
    pprintf("\t ScanLun0Only.LowPart %x\n", sc->ScanLun0Only.LowPart );
    pprintf("\t ScanLun0Only.HighPart %x\n", sc->ScanLun0Only.HighPart );
    pprintf("\t DisableDevice %x\n", sc->DisableDevice );
    pprintf("\t MaxOffset %x\n", sc->MaxOffset );
    pprintf("\t MaxDataWidth %x\n", sc->MaxDataWidth );
    pprintf("\t MaxDataWidth %x\n", sc->MaxDataWidth );
    pprintf("\t MaxSyncRate.LowPart %x\n", sc->MaxSyncRate.LowPart );
    pprintf("\t MaxSyncRate.HighPart %x\n", sc->MaxSyncRate.HighPart );
    pprintf("\n");
}

void i2o_scsi_dump_grp201( struct _I2O_HBA_SCSI_BUS_PORT_INFO_SCALAR *sc ) {

    pprintf("SCSI param group 201 at %x\n", sc );
    pprintf("\t PhysicalInterface %x\n", sc->PhysicalInterface );
    pprintf("\t ElectricalInterface %x\n", sc->ElectricalInterface );
    pprintf("\t Isochronous %x\n", sc->Isochronous );
    pprintf("\t ConnectorType %x\n", sc->ConnectorType );
    pprintf("\t ConnectorGender %x\n", sc->ConnectorGender );
    pprintf("\t MaxNumberDevices %x\n", sc->MaxNumberDevices );

    pprintf("\t DeviceIdBegin %x\n", sc->DeviceIdBegin );
    pprintf("\t DeviceIdEnd %x\n", sc->DeviceIdEnd );
    pprintf("\t LunBegin %x\n", sc->LunBegin );
    pprintf("\t LunEnd %x\n", sc->LunEnd );
    pprintf("\n");
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
 * = i2o_scsi_new_unit -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Create the system block associated with the device. Call 
 *	scsi_send_inquiry to create the file(s) associated with this
 *	system block.
 *  
 * FORM OF CALL:
 *  
 *	i2o_scsi_new_unit( pb, sb_index, LCT, verbose )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb   - pointer to I2O port block
 *	int sb_index	    - system block index
 *	int lct		    - Logical Configuration Table entry for this device
 *	int verbose	    - ( flag ) print device info if set
 *
 * SIDE EFFECTS:
 *
 *
-*/
void i2o_scsi_new_unit( struct i2o_pb *pb, int sb_index, struct _I2O_LCT_ENTRY *lct, int verbose  ) {

    int i2o_scsi_command( );
    struct scsi_sb *sb;
    int info_size;
    I2O_SCSI_DEVICE_INFO_SCALAR *info;

    /*
     * Dump SCSI peripheral information
     */

    info_size = sizeof( I2O_SCSI_DEVICE_INFO_SCALAR );
    info = i2o_malloc( info_size );
    if ( I2O_GetParamGroup( pb, lct->LocalTID, I2O_SCSI_DEVICE_INFO_GROUP_NO, info_size, info ) == msg_success ) {
#if DEBUG
	i2o_scsi_dump_grp0( info );
#endif

	if ( !pb->pb.sb[sb_index] )
	    sb = i2o_malloc( sizeof( *sb ));
	else 
	    sb = pb->pb.sb[sb_index];
#if DRIVERSHUT
	sb->verbose = TRUE;
#endif
	sb->node_id = lct->LocalTID;
	sb->class = lct->ClassID.Class;
	pb->pb.sb[sb_index] = sb;
	sb->pb = pb;
	sb->ub.flink = &sb->ub.flink;
	sb->ub.blink = &sb->ub.flink;
	sb->command = i2o_scsi_command;
	sb->ok = 1;
	sb->node_id = lct->LocalTID;
	set_io_name( sb->name, 0, 0, sb->node_id, pb );

	scsi_send_inquiry( sb );

	i2o_free( info );
    }
}


/*+
 * ============================================================================
 * = i2o_scsi_command -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Execute I2O SCSI command.
 *  
 * FORM OF CALL:
 *  
 *	i2o_scsi_command( sb, lun, cmd, cmd_len, dat_out, dat_out_len, dat_in,
 *			  dat_in_len, sts, sense_dat_in, sense_dat_in_len )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	struct scsi_sb *sb	    - SCSI system block
 *	int lun			    - logical unit number
 *	unsigned char *cmd	    - pointer to SCSI command
 *	int cmd_len		    - SCSI command length
 *	unsigned char *dat_out	    - pointer to write data
 *	int data_out_len	    - length of write data
 *	unsigned char *dat_in	    - pointer to read data
 *	int data_in_len		    - length of read data 
 *	unsigned int *sts	    - pointer to device status
 *	unsigned char *sense_dat_in - pointer to SCSI sense info
 *	int *sense_dat_in_len	    - pointer to SCSI sense info length
 *
 * SIDE EFFECTS:
 *
 *
-*/
int i2o_scsi_command( struct scsi_sb *sb, int lun, unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int dat_out_len,
	unsigned char *dat_in, int dat_in_len, unsigned char *sts,
	unsigned char *sense_dat_in, int *sense_dat_in_len ) {
    

    int					status = msg_failure;
    unsigned int			SGL_offset, t = msg_success;
    I2O_CNSL_MFHDR			*mfhdr;
    I2O_SCSI_SCB_EXECUTE_MESSAGE	*sc;
    I2O_SINGLE_REPLY_MESSAGE_FRAME	*rsp;
    I2O_SCSI_ERROR_REPLY_MESSAGE_FRAME  *scrsp;
    unsigned int			slen;
    unsigned int			device_status;
    unsigned int			adapter_status;
    struct i2o_pb			*pb;

    if ( lun )
	return msg_failure;

    pb = sb->pb;
    krn$_wait( &pb->owner_s );

    mfhdr = i2o_malloc( sizeof( I2O_CNSL_MFHDR ) + 
			    sizeof( I2O_SCSI_SCB_EXECUTE_MESSAGE ));

    sc = (char *) mfhdr + sizeof( I2O_CNSL_MFHDR );

    mfhdr->context_type = I2O_CNSL_CONTEXT_SEMAPHORE;

    SGL_offset = offsetof( I2O_SCSI_SCB_EXECUTE_MESSAGE, SGL ) / 4;

    i2o_message_frame( &sc->StdMessageFrame, SGL_offset, 0, 
		       sizeof(I2O_SCSI_SCB_EXECUTE_MESSAGE),
		       sb->node_id, I2O_HOST_TID, I2O_SCSI_SCB_EXEC, mfhdr );
    sc->CDBLength = cmd_len;
    memcpy( sc->CDB, cmd, cmd_len );

    if( dat_out_len ) {
	sc->SCBFlags = I2O_SCB_FLAG_XFER_TO_DEVICE;
	sc->ByteCount = dat_out_len;
	sc->SGL.u.Simple[0].FlagsCount.Count = dat_out_len;
	sc->SGL.u.Simple[0].FlagsCount.Flags = I2O_SGL_SIMPLE_FLAGS;
	sc->SGL.u.Simple[0].PhysicalAddress = SYSADR( pb, dat_out );
    }
    if( dat_in_len ) {
	sc->SCBFlags  = I2O_SCB_FLAG_XFER_FROM_DEVICE;
	sc->ByteCount = dat_in_len;
	sc->SGL.u.Simple[0].FlagsCount.Count = dat_in_len;
	sc->SGL.u.Simple[0].FlagsCount.Flags = I2O_SGL_SIMPLE_FLAGS;
	sc->SGL.u.Simple[0].PhysicalAddress = SYSADR( pb, dat_in );
    }


    sc->SCBFlags |= I2O_SCB_FLAG_SENSE_DATA_IN_MESSAGE;

#if DEBUG
    i2o_dump_scsi_execute( sc, lun );
#endif

    /*
     * Send the SCSI Execute message.
     */

    I2O_MsgSend( pb, mfhdr );

    scrsp = ( I2O_SCSI_ERROR_REPLY_MESSAGE_FRAME * ) mfhdr->reply;
    rsp = &scrsp->StdReplyFrame;

    if ( rsp->ReqStatus != I2O_REPLY_STATUS_SUCCESS )
	pprintf("SCSI Execute has failed: %x|%x\n", rsp->ReqStatus,
                                                  rsp->DetailedStatusCode);
    /* Dump the status block */

#if DEBUG
    i2o_dump_scsi_reply( scrsp, lun );
#endif
    rsp = &scrsp->StdReplyFrame;

    device_status = rsp->DetailedStatusCode & I2O_SCSI_DEVICE_DSC_MASK;
    adapter_status = rsp->DetailedStatusCode & I2O_SCSI_HBA_DSC_MASK;

    if ( adapter_status == I2O_SCSI_HBA_DSC_SUCCESS ) {
	if ( scrsp->AutoSenseTransferCount ) {
	    slen = min( I2O_SCSI_SENSE_DATA_SZ, scrsp->AutoSenseTransferCount );
	    memcpy( sense_dat_in, scrsp->SenseData, slen );
	    *sense_dat_in_len = slen;
	}
	*sts = device_status;
    } else { 
	if( adapter_status == I2O_SCSI_HBA_DSC_SCSI_BUS_RESET ) {

	    /* Reinit the port */
	    *sts = scsi_k_sts_busy;
	} else {
	    t = msg_failure;
	}
    }	

    status = I2O_MsgComplete( pb, mfhdr );
    krn$_post( &pb->owner_s );

    return( t );    
}


/*+
 * ============================================================================
 * = i2o_scsi_init -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Perform any adapter level setup. 
 *  
 * FORM OF CALL:
 *  
 *	i2o_scsi_init( pb, tid )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb   - pointer to I2O port block
 *	int tid		    - target ID of adapter
 *
 * SIDE EFFECTS:
 *
 *
-*/
void i2o_scsi_init( struct i2o_pb *pb, int tid ) {
    int *buf;
    int buf_size;

#if DEBUG

    /*
     * Dump SCSI adapter information
     */

    buf_size = sizeof( I2O_HBA_SCSI_CONTROLLER_INFO_SCALAR );
    buf = i2o_malloc( buf_size );
    I2O_GetParamGroup( pb, tid, I2O_HBA_SCSI_CONTROLLER_INFO_GROUP_NO, buf_size, buf );
    i2o_scsi_dump_grp200( buf );
    i2o_free( buf );

    /*
     * Dump SCSI Port information
     */    

    buf_size = sizeof( I2O_HBA_SCSI_BUS_PORT_INFO_SCALAR );
    buf = i2o_malloc( buf_size );
    I2O_GetParamGroup( pb, tid, I2O_HBA_SCSI_BUS_PORT_INFO_GROUP_NO, buf_size, buf );
    i2o_scsi_dump_grp201( buf );
    i2o_free( buf );

#endif

}


/*+
 * ============================================================================
 * = i2o_scsi_shutdown -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Shutdown the device. Call scsi_break_vc.
 *  
 * FORM OF CALL:
 *  
 *	i2o_scsi_shutdown( sb )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	struct scsi_sb *sb   - pointer to system block
 *
 * SIDE EFFECTS:
 *
 *
-*/
void i2o_scsi_shutdown( struct scsi_sb *sb ) {
    
    if (sb->state == sb_k_open) {
	fprintf( el_fp, "breaking virtual connection with sb %08x\n", sb );
	scsi_break_vc(sb);
    }
#if DRIVERSHUT
    krn$_semrelease(&sb->ready_s);
#endif
    i2o_free( sb );
}

protoendif(__I2O__)
