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
 * Abstract:	I2O miscellaneous routines
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

i2o_get_controllers( struct pb *pb, int *c ) {

    I2O_EXEC_LCT_NOTIFY_MESSAGE *ln;
    I2O_LCT *lct_head;
    I2O_LCT_ENTRY *lct;
    int num_lct_entries;
    int i;
    int status = msg_failure;
    int lct_size;

    /*
     * Init the IOP ( check status, init outbound queue );
     */

#if 0
    I2O_IOPinit( pb );
#endif

    /* 
     * Send an ExecLctNotify request to obtain the device's Logical 
     * Configuration Table.
     */

    lct_size = sizeof( I2O_LCT ) + sizeof( I2O_LCT_ENTRY ) * ( I2O_MAX_CONTROLLERS - 1 );
    lct_head = i2o_malloc( lct_size, "LCT" );
    if ( I2O_ExecLctNotify( pb, I2O_CLASS_BUS_ADAPTER_PORT, lct_head, lct_size ) == msg_success ) {

	if ( lct_head->IopFlags != 0 ) {
	    dprintf("IOP is requesting configuration dialogue\n", p_args0 );
	}

	/*
	 * Table version must be zero.
	 */

	if ( lct_head->LctVer == 0 ) {

	    num_lct_entries = ( lct_head->TableSize - (( sizeof( I2O_LCT ) - sizeof( I2O_LCT_ENTRY )) / 4 )) 
				    / (sizeof( I2O_LCT_ENTRY ) / 4 );

	    /* 
	     * Get the controller type from the subclass info for each 
	     * controller entries. Controller type is found in subclassinfo field.
	     * 
	     * SubClassInfo is the value returned in Parameter group 0, Field 0,
	     * pre-padded with zeros. Page 6-96, section 6.7.5, Parameter Group 0
	     * is controller information. Field 0 is BusType. 3 BusTypes are
	     * defined, generic bus = 0, SCSI = 1, FiberChannel = 2.
	     */

	    for ( i = 0; i < num_lct_entries; i++ ) {
		lct = &( lct_head->LCTEntry[i] );
		if ( lct->UserTID == I2O_RESOURCE_NOT_CLAIMED )
		    c[i] = lct->SubClassInfo;
	    }
	    status = msg_success;
	}
    }
    return status;
}
