/*
 * Copyright (C) 1993 by
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
 * Abstract:	I2O BSA OSM
 *
 * Author:	Kevin LeMieux
 *
 * Modifications:
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:eisa.h"
#include "cp$src:scsi_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:probe_io_def.h"
#include "cp$inc:platform_io.h"
#include "cp$src:platform_cpu.h"
#include "cp$inc:kernel_entry.h"
#include "cp$src:print_var.h"
#include "cp$src:i2oexec.h"
#include "cp$src:i2o_def.h"
#include "cp$src:i2o_pb_def.h"
#include "cp$src:i2omstor.h"
#include "cp$src:i2outil.h"

#if ( RAWHIDE & MODULAR ) 
#define INT __int64
#define DRIVERSHUT 1
#endif

#undef i2o_bsa_transfer

extern int null_procedure();

i2o_bsa_new_unit( struct i2o_pb *pb, int sb_index, I2O_LCT_ENTRY *lct, int verbose  );
i2o_bsa_shutdown( struct sb *sb );
i2o_bsa_send_read_write( struct ub *ub, unsigned char *c, int len, int lbn,
	int opcode );
i2o_bsa_transfer( struct FILE *fp, int len, unsigned char *c, int opcode );

struct osm i2o_bsa_osm = { I2O_CLASS_RANDOM_BLOCK_STORAGE,
			   0, i2o_bsa_new_unit,
			   i2o_bsa_shutdown,
			   null_procedure };

/*
 *********************************************************************
 *                                                                   *
 *  Start of DEBUG routines                                          *
 *                                                                   *
 *********************************************************************
 */

#if DEBUG

i2o_bsa_dump_param_grp0( I2O_BSA_DEVICE_INFO_SCALAR *bp ) {

    pprintf("BSA param group 0 at %x\n", bp );
    pprintf("\t DeviceType %x\n", bp->DeviceType );
    pprintf("\t NumberOfPaths %x\n", bp->NumberOfPaths );
    pprintf("\t PowerState %x\n", bp->PowerState );
    pprintf("\t BlockSize %x\n", bp->BlockSize );
    pprintf("\t DeviceCapacity.LowPart %x\n", bp->DeviceCapacity.LowPart );
    pprintf("\t DeviceCapacity.HighPart %x\n", bp->DeviceCapacity.HighPart );
    pprintf("\t DeviceCapabilitySupport %x\n", bp->DeviceCapabilitySupport );
    pprintf("\t DeviceState %x\n", bp->DeviceState );
}

i2o_bsa_dump_transfer_msg( I2O_BSA_READ_MESSAGE *bt ) {

    pprintf("BSA Read message at %x\n", bt );
    i2o_dump_mfa( &bt->StdMessageFrame );
    pprintf("\t TransactionContext %x\n", bt->TransactionContext );
    pprintf("\t ControlFlags %x\n", bt->ControlFlags );
    pprintf("\t TimeMultiplier %x\n", bt->TimeMultiplier );
    pprintf("\t FetchAhead %x\n", bt->FetchAhead );
    pprintf("\t TransferByteCount %x\n", bt->TransferByteCount );
    pprintf("\t LogicalByteAddress.LowPart %x\n", bt->LogicalByteAddress.LowPart );
    pprintf("\t LogicalByteAddress.HighPart %x\n", bt->LogicalByteAddress.HighPart );
    pprintf("\t SGL.u.Simple[0].FlagsCount.Count %x\n", bt->SGL.u.Simple[0].FlagsCount.Count );
    pprintf("\t SGL.u.Simple[0].FlagsCount.Flags %x\n", bt->SGL.u.Simple[0].FlagsCount.Flags );
    pprintf("\t SGL.u.Simple[0].PhysicalAddress %x\n", bt->SGL.u.Simple[0].PhysicalAddress );
}
#endif

/*
 *********************************************************************
 *                                                                   *
 *  End of DEBUG routines                                            *
 *                                                                   *
 *********************************************************************
 */


extern struct DDB *i2o_bsa_ddb_ptr;

#define OPCODE_READ  1
#define OPCODE_WRITE 0
#define BLOCK_SIZE 512


/*+
 * ============================================================================
 * = i2o_bsa_transfer -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	I2O Block Storage Transfer.
 *  
 * FORM OF CALL:
 *  
 *	i2o_bsa_transfer( file, length, buffer, opcode )
 *  
 * RETURNS:
 *
 *	int t - status
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - pointer to file
 *	int len		- number of bytes to transfer
 *	char *c		- transfer buffer
 *	int opcode	- read or write
 *
 * SIDE EFFECTS:
 *
 *
-*/
i2o_bsa_transfer( struct FILE *fp, int len, unsigned char *c, int opcode )
    {
    int lbn;
    int t;
    struct INODE *inode;
    struct ub *ub;

    /*
     *  The file is open.  The MISC field in the INODE structure is used
     *  to hold a UB pointer.
     */
    lbn = *(INT *)fp->offset / BLOCK_SIZE;
    inode = fp->ip;
    ub = inode->misc;
    /*
     *  Don't let transfers go past the end of the unit.
     */
    if( *(INT *)fp->offset + len > *(INT *)inode->len )
	{
	fp->status = msg_eof;
	fp->count = 0;
	t = 0;
	}
    /*
     *  Make sure that the transfer respects block boundaries.
     */
    else if( ( ( len & 511 ) == 0 ) && ( ( *(INT *)fp->offset & 511 ) == 0 ) )
	{
	t = i2o_bsa_send_read_write( ub, c, len, lbn, opcode );
	fp->status = t;
	fp->count = len;
	if( t == msg_success )
	    t = len;
	else
	    t = 0;
	/*
	 *  Update the file offset.
	 */
	*(INT *)fp->offset += t;
	}
    else
	{
	err_printf( msg_bad_request, inode->name );
	fp->status = msg_bad_request;
	fp->count = 0;
	t = 0;
	}
    /*
     *  At this point we have kept track of the number of bytes successfully
     *  transferred.  Return that number.
     */
    return( t );
    }

/*+
 * ============================================================================
 * = i2o_bsa_send_read_write -						      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Execute I2O block storage read/write requests.
 *  
 * FORM OF CALL:
 *  
 *	i2o_bsa_send_read_write( ub, buffer, length, lbn, opcode )
 *  
 * RETURNS:
 *
 *	int status - status
 *       
 * ARGUMENTS:
 *
 *	struct ub *ub   - pointer to unit block
 *	char *c		- transfer buffer
 *	int len		- number of bytes to transfer
 *	int lbn		- logical block number
 *	int opcode	- read or write
 *
 * SIDE EFFECTS:
 *
 *
-*/
i2o_bsa_send_read_write( struct ub *ub, unsigned char *c, int len, int lbn,
	int opcode ) 
    {
    int status;
    int function = I2O_BSA_BLOCK_READ;
    unsigned int SGL_offset;
    I2O_CNSL_MFHDR                 *mfhdr;
    I2O_SINGLE_REPLY_MESSAGE_FRAME *btrsp;
    I2O_BSA_READ_MESSAGE *bt;
    int i;    
    int *p;
    struct i2o_pb *pb;

    pb = ub->sb->pb;

    if ( opcode == OPCODE_READ )
	dprintf("\t I2O BSA Reading %d bytes lbn %d from %s\n", p_args3( len, lbn, ub->inode->name ) );
    if ( opcode == OPCODE_WRITE )
	dprintf("\t I2o BSA Writing %d bytes lbn %d to %s\n", p_args3( len, lbn, ub->inode->name ) );
    dprintf("\n", p_args0 );
    
    krn$_wait( &pb->owner_s );

    mfhdr = i2o_malloc( sizeof( I2O_CNSL_MFHDR ) + 
				sizeof( I2O_BSA_READ_MESSAGE ));	
    bt = (char *) mfhdr + sizeof( I2O_CNSL_MFHDR );

    mfhdr->context_type = I2O_CNSL_CONTEXT_SEMAPHORE;
         
    /*
     * Build the block transfer message.
     */

    SGL_offset = offsetof( I2O_BSA_READ_MESSAGE, SGL ) / 4;
    if ( opcode == OPCODE_WRITE )
	function = I2O_BSA_BLOCK_WRITE;

    i2o_message_frame( &bt->StdMessageFrame, SGL_offset, 0, 
		       sizeof(I2O_BSA_READ_MESSAGE),
		       ub->unit, I2O_HOST_TID, function, mfhdr );

    bt->TransferByteCount = len;
    bt->LogicalByteAddress.LowPart = lbn * BLOCK_SIZE;
    bt->LogicalByteAddress.HighPart = 0;
    bt->FetchAhead = 0;
    bt->TimeMultiplier = 0;
    bt->ControlFlags = 0;
    bt->SGL.u.Simple[0].FlagsCount.Count = len;
    bt->SGL.u.Simple[0].FlagsCount.Flags = I2O_SGL_SIMPLE_FLAGS;
    bt->SGL.u.Simple[0].PhysicalAddress = SYSADR( pb, c );

#if DEBUG
    i2o_bsa_dump_transfer_msg( bt );
#endif

    /*
     * Send the block transfer message.
     */

    I2O_MsgSend( pb, mfhdr );
    status = I2O_MsgComplete( pb, mfhdr );
    krn$_post( &pb->owner_s );
    return( status );
    }

/*+
 * ============================================================================
 * = i2o_bsa_new_unit -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Create a file to associate with the device.
 *  
 * FORM OF CALL:
 *  
 *	i2o_bsa_new_unit( pb, sb_index, LCT, verbose )
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
i2o_bsa_new_unit( struct i2o_pb *pb, int sb_index, I2O_LCT_ENTRY *lct, int verbose ) {
    char *status;
    char name[32];
    int i, inode_status;
    struct sb *sb;
    struct ub *ub;
    struct INODE *inode;
    I2O_BSA_DEVICE_INFO_SCALAR *info;
    int new_ub = FALSE;

    info = i2o_malloc( sizeof( I2O_BSA_DEVICE_INFO_SCALAR ));
    if ( I2O_GetParamGroup( pb, lct->LocalTID, I2O_BSA_DEVICE_INFO_GROUP_NO, sizeof( *info), info ) == msg_success ) {
#if DEBUG
	i2o_bsa_dump_param_grp0( info );
#endif

	if ( !pb->pb.sb[sb_index] ) {
	    
	    sb = i2o_malloc( sizeof( *sb ));
	} else {
	    sb = pb->pb.sb[sb_index];
	}
	pb->pb.sb[sb_index] = sb;
	sb->pb = pb;
	sb->class = lct->ClassID.Class;
	strcpy( sb->name, pb->pb.name );

	/*
	 *  Create a new "file".
	 */
	set_io_name( name, i2o_bsa_ddb_ptr->name, lct->LocalTID, 0, pb );
	inode_status = allocinode( name, 1, &inode );

#if DRIVERSHUT
	ub = i2o_malloc( sizeof( *ub ));
	new_ub = TRUE;
#else
	if ( inode_status == 1 ) {
	    ub = inode->misc;
	} else {
	    if ( inode_status == 2 ) {
		ub = i2o_malloc( sizeof( *ub ));
		new_ub = TRUE;
	    } else {
		return;
	    }
	}
#endif

	ub->sb = sb;
	ub->pb = pb;
	ub->unit = lct->LocalTID;

	inode->dva = i2o_bsa_ddb_ptr;
	inode->attr = ATTR$M_READ;
	*(INT *)inode->len = *(( INT *) &info->DeviceCapacity.LowPart );
	inode->bs = BLOCK_SIZE;
	/*
	 *  Set up required pointers and back-pointers.  The INODE points to the
	 *  UB, the UB points to the SB, the SB points to the UB.
	 */
	inode->misc = ub;
	ub->inode = inode;
	sb->first_ub = ub;
	INODE_UNLOCK( inode );
	sprintf( ub->alias, "%s%c%d (BSA)",
		i2o_bsa_ddb_ptr->name, pb->pb.controller + 'A',
		ub->unit );
	strupper( ub->alias );
	sprintf( ub->string, "%-24s   %-8s   %24s  %s",
	    name, ub->alias, ub->info, ub->version );
	if ( new_ub )
	    insert_ub( ub );
	if ( verbose )
	    printf( "%-18s %-8s\n", ub->inode->name, ub->alias );
	i2o_free( info );
    }
}


/*+
 * ============================================================================
 * = i2o_bsa_shutdown -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Shutdown the device.
 *  
 * FORM OF CALL:
 *  
 *	i2o_bsa_shutdown( sb )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	struct sb *sb   - pointer to system block
 *
 * SIDE EFFECTS:
 *
 *
-*/
i2o_bsa_shutdown( struct sb *sb ) {

    struct ub *ub;

    ub = sb->first_ub;
    if( ub ) {
	remove_ub( ub );
	i2o_free( ub );
    }
    i2o_free( sb );
}
