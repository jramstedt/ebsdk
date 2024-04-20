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
 * Abstract:	I2O emulate
 *
 * Author:	Kevin LeMieux
 *
 * Modifications:
 *
 *	kl	07-Aug-1997	Initial entry.
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
#include "cp$src:i2oexec.h"
#include "cp$src:i2omsg.h"
#include "cp$src:i2o_def.h"
#include "cp$src:i2omstor.h"
#include "cp$src:i2obscsi.h"

int block_storage_tid0 = 0x26;
int block_storage_tid1 = 0x27;
int scsi_tid0 = 0x35;
int scsi_tid1 = 0x36;


i2o_lct_emulate( I2O_LCT *lct_head, int class_id ) {

  I2O_LCT_ENTRY *lct;

  if ( class_id == I2O_CLASS_BUS_ADAPTER_PORT || class_id == I2O_CLASS_MATCH_ANYCLASS ) {

    /*
     * Emulate the return of the LCT. There are two SCSI bus adapters
     * in this LCT. The Parent TID ( IOP ?) is 1. The local TID of 
     * controller 0 is 0x23. The local TID of controller 1 is 0x24.
     */


    /* LCT header */

    /* 2 LCT entries of 9 longwords each ( I2O_LCT contains 1 LCT ENTRY )  */
    lct_head->TableSize = ( sizeof( I2O_LCT ) / 4 ) + (( sizeof( I2O_LCT_ENTRY ) / 4 )) ; 
    lct_head->BootDeviceTID = 0;
    lct_head->LctVer = 0;
    lct_head->IopFlags = 0;
    lct_head->CurrentChangeIndicator = 0;
  
    /* LCT entry 0 ( SCSI Bus Controller 0 ) */
    
    lct = &( lct_head->LCTEntry[0] );
    
    lct->TableEntrySize = 9;
    lct->LocalTID = 24;
    lct->ChangeIndicator = 0;
    lct->DeviceFlags = 0;
    lct->ClassID.Class = I2O_CLASS_BUS_ADAPTER_PORT;

    /* 
     * SubClassInfo is the value returned in Parameter group 0, Field 0,
     * pre-padded with zeros. Page 6-96, section 6.7.5, Parameter Group 0
     * is controller information. Field 0 is BusType. 3 BusTypes are
     * defined, generic bus = 0, SCSI = 1, FiberChannel = 2.
     */ 

    lct->SubClassInfo = I2O_SUBCLASS_SCSI;
    lct->UserTID = I2O_RESOURCE_NOT_CLAIMED;
    lct->ParentTID = 0x1;
    lct->BiosInfo = 0xFF;   /* FF indicates no bios info */
    sprintf( &lct->IdentityTag[0], "Q1022233" );
    lct->EventCapabilities = 0;

    lct = &( lct_head->LCTEntry[1] );

    /* LCT entry 1 ( SCSI Bus Controller 1 ) */

    lct->TableEntrySize = 9;
    lct->LocalTID = 0x23;
    lct->ChangeIndicator = 0;
    lct->DeviceFlags = 0;
    lct->ClassID.Class = I2O_CLASS_BUS_ADAPTER_PORT;

    /* 
     * SubClassInfo is the value returned in Parameter group 0, Field 0,  
     * pre-padded with zeros. Page 6-96, section 6.7.5, Parameter Group 0
     * is controller information. Field 0 is BusType. 3 BusTypes are
     * defined, generic bus = 0, SCSI = 1, FiberChannel = 2.
     */ 

    lct->SubClassInfo = 0x12345678;
    lct->UserTID = I2O_RESOURCE_NOT_CLAIMED;
    lct->ParentTID = 0x1;
    lct->BiosInfo = 0xFF;   /* FF indicates no bios info */
    sprintf( &lct->IdentityTag[0], "Q1023456" );
    lct->EventCapabilities = 0;

    /*
     * If it's match_any_class, add 4 SCSI peripherals to controller 0.
     * 2 Block Storage and 2 SCSI.
     */

    if ( class_id == I2O_CLASS_MATCH_ANYCLASS ) {
      lct_head->TableSize += 7 * (( sizeof( I2O_LCT_ENTRY ) / 4 )) ; 

      lct = &( lct_head->LCTEntry[2] );

      /* LCT entry 2 ( Block Storage Disk ) */

      lct->TableEntrySize = 9;
      lct->LocalTID = 0x26;
      lct->ChangeIndicator = 0;
      lct->DeviceFlags = 0;
      lct->ClassID.Class = I2O_CLASS_RANDOM_BLOCK_STORAGE;
      lct->SubClassInfo = 0; 
      lct->UserTID = I2O_RESOURCE_NOT_CLAIMED;
      lct->ParentTID = 0x13; /* Parent = Block Storage DDM */
      lct->BiosInfo = 0xFF;   /* FF indicates no bios info */
      sprintf( &lct->IdentityTag[0], "P1" );
      lct->EventCapabilities = 0;

      lct = &( lct_head->LCTEntry[3] );

      /* LCT entry 3 ( Block Storage Disk ) */

      lct->TableEntrySize = 9;
      lct->LocalTID = 0x27;
      lct->ChangeIndicator = 0;
      lct->DeviceFlags = 0;
      lct->ClassID.Class = I2O_CLASS_RANDOM_BLOCK_STORAGE;
      lct->SubClassInfo = 0; 
      lct->UserTID = I2O_RESOURCE_NOT_CLAIMED;
      lct->ParentTID = 0x13; /* Parent = Block Storage DDM */
      lct->BiosInfo = 0xFF;   /* FF indicates no bios info */
      sprintf( &lct->IdentityTag[0], "P2" );
      lct->EventCapabilities = 0;

      lct = &( lct_head->LCTEntry[4] );

      /* LCT entry 4 ( Block storage DDM ) */

      lct->TableEntrySize = 9;
      lct->LocalTID = 0x13;
      lct->ChangeIndicator = 0;
      lct->DeviceFlags = 0;
      lct->ClassID.Class = I2O_CLASS_DDM;
      lct->SubClassInfo = I2O_SUBCLASS_HDM;
      lct->UserTID = 0;
      lct->ParentTID = 0;
      lct->BiosInfo = 0xFF;   /* FF indicates no bios info */
      sprintf( &lct->IdentityTag[0], "RX456" );
      lct->EventCapabilities = 0;

      /* LCT entry 5 ( SCSI Peripheral ) */

      lct = &( lct_head->LCTEntry[5] );

      lct->TableEntrySize = 9;
      lct->LocalTID = block_storage_tid0;
      lct->ChangeIndicator = 0;
      lct->DeviceFlags = 0;
      lct->ClassID.Class = I2O_CLASS_SCSI_PERIPHERAL;
      lct->SubClassInfo = 0;
      lct->UserTID = 0x26; /* Block Storage Disk */
      lct->ParentTID = 24; /* Bus Adapter 0 */
      lct->BiosInfo = 0xFF;   /* FF indicates no bios info */
      sprintf( &lct->IdentityTag[0], "S2" );
      lct->EventCapabilities = 0;

      /* LCT entry 6 ( SCSI Peripheral ) */

      lct = &( lct_head->LCTEntry[6] );

      lct->TableEntrySize = 9;
      lct->LocalTID = block_storage_tid1;
      lct->ChangeIndicator = 0;
      lct->DeviceFlags = 0;
      lct->ClassID.Class = I2O_CLASS_SCSI_PERIPHERAL;
      lct->SubClassInfo = 0;
      lct->UserTID = 0x27; /* Block Storage Disk */
      lct->ParentTID = 24; /* Bus Adapter 0 */
      lct->BiosInfo = 0xFF;   /* FF indicates no bios info */
      sprintf( &lct->IdentityTag[0], "S3" );
      lct->EventCapabilities = 0;

      /* LCT entry 7 ( SCSI Peripheral ) */

      lct = &( lct_head->LCTEntry[7] );

      lct->TableEntrySize = 9;
      lct->LocalTID = scsi_tid0;
      lct->ChangeIndicator = 0;
      lct->DeviceFlags = 0;
      lct->ClassID.Class = I2O_CLASS_SCSI_PERIPHERAL;
      lct->SubClassInfo = 0;
      lct->UserTID = I2O_RESOURCE_NOT_CLAIMED;
      lct->ParentTID = 24; /* Bus Adapter 0 */
      lct->BiosInfo = 0xFF;   /* FF indicates no bios info */
      sprintf( &lct->IdentityTag[0], "S2" );
      lct->EventCapabilities = 0;

      /* LCT entry 8 ( SCSI Peripheral ) */

      lct = &( lct_head->LCTEntry[8] );

      lct->TableEntrySize = 9;
      lct->LocalTID = scsi_tid1;
      lct->ChangeIndicator = 0;
      lct->DeviceFlags = 0;
      lct->ClassID.Class = I2O_CLASS_SCSI_PERIPHERAL;
      lct->SubClassInfo = 0;
      lct->UserTID = I2O_RESOURCE_NOT_CLAIMED;
      lct->ParentTID = 24; /* Bus Adapter 0 */
      lct->BiosInfo = 0xFF;   /* FF indicates no bios info */
      sprintf( &lct->IdentityTag[0], "S3" );
      lct->EventCapabilities = 0;


    }
  }
}

i2o_get_params_emulate( int tid, int group, char *group_buf ) {

    I2O_PARAM_READ_OPERATION_RESULT *result;
    I2O_BSA_DEVICE_INFO_SCALAR *bp;
    I2O_SCSI_DEVICE_INFO_SCALAR *sc;

    /*
     * Emulation Hack Alert - this is hardcoded to the TID's found in the LCT.
     */

    if (( tid == block_storage_tid0 ) || ( tid == block_storage_tid1 )) {
	switch ( group ) {
	    case I2O_BSA_DEVICE_INFO_GROUP_NO:

		/*
		 * BSA Group 0.
		 */

		bp = ( I2O_BSA_DEVICE_INFO_SCALAR * ) group_buf;

		bp->DeviceType = 0;
		bp->NumberOfPaths = 1;
		bp->PowerState = 0;
		bp->BlockSize = 512;
		bp->DeviceCapacity.LowPart = 0x10000000;
		bp->DeviceCapacity.HighPart = 0;
		bp->DeviceCapabilitySupport = 0; 
		bp->DeviceState = 0;
		break;
	    default:
		pprintf("Param group %x not emulated\n", group );

	}
    }

    if (( tid == scsi_tid0 ) || ( tid == scsi_tid1 )) {
	switch ( group ) {
	    case I2O_SCSI_DEVICE_INFO_GROUP_NO:


		sc = ( I2O_SCSI_DEVICE_INFO_SCALAR * ) group_buf;

		sc->DeviceType = 0;
		sc->Flags = 0;
		sc->Identifier = tid - 0x30;
		*sc->LunInfo = 0; /* SCSI-2 8-bit scalar LUN goes into offset 1 */
		sc->QueueDepth = 0;
		sc->NegOffset = 0;
		sc->NegDataWidth = 0;
		sc->NegSyncRate.LowPart = 0;
		sc->NegSyncRate.HighPart = 0;
		break;
	    default:
		pprintf("Param group %x not emulated\n", group );
	}
    }
}

i2o_emulate_scsi_command( I2O_SCSI_SCB_EXECUTE_MESSAGE *sc, 
		    I2O_SCSI_ERROR_REPLY_MESSAGE_FRAME  *scrsp, int lun ) {

    int *db;
    I2O_SINGLE_REPLY_MESSAGE_FRAME	*rsp;

    db = sc->SGL.u.Simple[0].PhysicalAddress;

    /* Allow a disk at TID 35 and TID 36 */

    if (( sc->StdMessageFrame.TargetAddress == 0x35) || 
	    (sc->StdMessageFrame.TargetAddress  == 0x36)) {
	process_scsi_command( sc->CDB[0], db);
    }

    /* Good status only for lun 0 */

    rsp = &scrsp->StdReplyFrame;
    if ( lun == 0 ) {
	rsp->DetailedStatusCode = 0;
	scrsp->AutoSenseTransferCount = 4;
    } else {
	rsp->DetailedStatusCode = 0xFFFFFFFF;
    }
	

}

/* 
 * This routine emulates an RZ26 in slots 3 and 5. The emulation 
 * is limited to the commands that are required to do a "show device"
 * and a "boot".
 */

process_scsi_command(int command, int *db) {

    /* Request sense */

    if (command == 0x3) {
	pprintf("request sense\n");
	*db++ = 0x00060070;
	*db++ =	0x0a000000; 
	*db++ =	0x00000000; 
	*db++ =	0x00000229; 
    }

    /* Mode select */

    if (command == 0x15) {
	pprintf("mode select\n");
    }

    /* Test unit ready */

    if (command == 0) {
	pprintf("test unit ready\n");
    }

    /* DK Read */

    if (command == 0x28) {
	pprintf("read\n");
	*db++ = 0;
	*db++ = 0x11111111;
	*db++ = 0x22222222;
	*db++ =	0x33333333;
	*db++ =	0x44444444;
	*db++ =	0x55555555;
	*db++ =	0x66666666;
	*db++ =	0x77777777;
	*db++ =	0x88888888;
	*db++ =	0x99999999;
	*db++ =	0xaaaaaaaa;
	*db++ =	0xbbbbbbbb;
	*db++ =	0xcccccccc;
	*db++ =	0xdddddddd;
	*db++ =	0xeeeeeeee;
	*db++ =	0xffffffff;
	*db++ =	0;

    }

    /* Read capacity */

    if (command == 0x25) {
	pprintf("read capacity\n");
	*db++ = 0x2b4b1f00;
	*db++ =	0x00020000; 
	*db++ =	0x00000025; 
	*db++ =	0x00000000; 
    }

    /* 
     * Send inquiry
     */

    if (command == 0x12) {
	*db++ = 0x02020000;
	*db++ = 0x1600005b;
	*db++ = 0x20434544;
	*db++ = 0x20202020;
	*db++ = 0x36325a52;
	*db++ = 0x2020204c;
	*db++ = 0x29432820;
	*db++ = 0x43454420;
	*db++ = 0x43303434;
	*db++ = 0x30303030;
	*db++ = 0x38343134;
	*db++ = 0x30323032;
	*db++ = 0x00000000;
	*db++ = 2113000000;
	*db++ = 0000000003;
    }
}




