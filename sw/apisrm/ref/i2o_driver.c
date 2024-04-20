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
 * Abstract:	I2O Driver - this driver allows for both DRIVERSHUT
 *		mode of operation where the driver is loaded into memory when 
 *		needed and removed thereafter, and for non-DRIVERSHUT
 *		mode in which the driver remains in memory.
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
#include "cp$src:common.h"
#include "cp$src:print_var.h"
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
#include "cp$src:i2oadptr.h"
#include "cp$src:i2oexec.h"
#include "cp$src:i2omsg.h"
#include "cp$src:i2o_def.h"

#if ( RAWHIDE & MODULAR ) 
#define DRIVERSHUT 1
#endif

#define EVENT_DEBUG 0
extern struct QUEUE pollq;

/*
 * Routine prototypes for routines associated excusively with DRIVERSHUT.
 */
#if DRIVERSHUT
i2o_establish_connection( struct INODE *inode, struct pb *gpb, int class );
int i2o_setmode( int mode, struct pb *gpb );
#endif

/*
 * Routine prototypes for routines associated excusively with non-DRIVERSHUT.
 */
#if !DRIVERSHUT
int i2o_setmode( struct pb *gpb, int mode );
void i2o_adapter_poll( struct i2o_pb *pb );
i2o_start( struct i2o_pb *pb );
i2o_init();
#endif

/*
 * Routine prototypes for routines that are shared bewteen DRIVERSHUT and 
 * non-DRIVERSHUT.
 */

struct osm *i2o_osm( int class, int subclass );
i2o_initialize_port( struct i2o_pb *pb );
i2o_poll_units( struct pb *gpb, int verbose );
i2o_get_lct( struct i2o_pb *pb, struct I2O_LCT **lct_head, unsigned int size );
i2o_iop_event_handler( struct i2o_pb *pb, I2O_CNSL_MFHDR *mfhdr );
i2o_rq_malloc( struct i2o_pb *pb, unsigned int size );
i2o_rq_free( struct i2o_pb *pb );
I2O_poll( struct I2O_pb *pb );
I2O_IOPinit( struct i2o_pb *pb );
I2O_IOPshutdown( struct i2o_pb *pb );
I2O_restart( struct i2o_pb *pb );

/*
 * External references
 */ 

extern int scsi_poll;
extern struct osm i2o_bsa_osm;
extern struct osm i2o_scsi_osm;
extern null_procedure();
extern I2O_handle_reply();
extern I2O_interrupt();

/* 
 * OSM table.
 */

i2o_osm_table[] = {
    { &i2o_bsa_osm },
    { &i2o_scsi_osm },
    { 0 }
};


#if EVENT_DEBUG 
struct event_message {
    unsigned int code;
    char *message;
} event_m[] = {
    { I2O_EVENT_IND_RESOURCE_LIMIT, "Resource Limit" },
    { I2O_EVENT_IND_CONNECTION_FAIL, "Connection Fail" },
    { I2O_EVENT_IND_ADAPTER_FAULT, "Adapter Fault" }, 
    { I2O_EVENT_IND_POWER_FAIL, "Power Fail" },
    { I2O_EVENT_IND_RESET_PENDING, "Reset Pending" },
    { I2O_EVENT_IND_RESET_IMMINENT, "Reset Imminent" },
    { I2O_EVENT_IND_HARDWARE_FAIL, "Hardware Fail" }, 
    { I2O_EVENT_IND_XCT_CHANGE, "XCT Change" }, 
    { I2O_EVENT_IND_NEW_LCT_ENTRY, "New LCT Entry" }, 
    { I2O_EVENT_IND_MODIFIED_LCT, "Modified LCT" }, 
    { I2O_EVENT_IND_DDM_AVAILABILITY, "DDM Availability" }, 
    { I2O_EVENT_IND_STATE_CHANGE, "State Change" }, 
    { I2O_EVENT_IND_GENERAL_WARNING, "General Warning" }, 
    { I2O_EVENT_IND_CONFIGURATION_FLAG, "Configuration Flag" },
    { I2O_EVENT_IND_LOCK_RELEASE, "Lock Release" }, 
    { I2O_EVENT_IND_CAPABILITY_CHANGE, "Capability Change" }, 
    { I2O_EVENT_IND_DEVICE_RESET, "Device Reset" }, 
    { I2O_EVENT_IND_EVENT_MASK_MODIFIED, "Event Mask Modified" }, 
    { I2O_EVENT_IND_FIELD_MODIFIED, "Field Modified" }, 
    { I2O_EVENT_IND_VENDOR_EVENT, "Vendor Event" }, 
    { I2O_EVENT_IND_DEVICE_STATE, "Device State" },
    { 0, 0 }
};
#endif

#if DEBUG

/*
 *********************************************************************
 *                                                                   *
 *  Start of MEMORY LEAK  routines                                   *
 *                                                                   *
 *********************************************************************
 */

struct QUEUE leak_q = { &leak_q,&leak_q };
struct LEAKDATA {
    struct QUEUE q;
    int adr;
    int callerpc;
    char *label;
    int size;
};

#define MALLOC 1
#define FREE   2

/*
 *  Configurable control variables.
 *  Easy to >>> d -l sym: these variables during debug session
 */
int i2o_dyn_debug  = 0;   /* enable flag */
int i2o_flood_byte = 0;   /* enable flag */

#define malloc( x,y )   dyn$_malloc( x,DYN$K_SYNC|DYN$K_NOOWN );
#define free( x,y )     dyn$_free( x,DYN$K_SYNC|DYN$K_NOOWN );

leak_watcher( int type, int adr, int callerpc, int size ) {
    
    struct LEAKDATA *l;

    switch( type ) {
	case MALLOC:
	    dprintf("MALLOC ", p_args0 );
	    l = dyn$_malloc( sizeof ( struct LEAKDATA ), DYN$K_SYNC|DYN$K_NOOWN );
	    l->adr      = adr;
	    l->callerpc = callerpc;
	    l->size     = size;
	    insq_lock( &l->q.flink, leak_q.blink );
	    dprintf("inserting %x onto leak watch queue - size:%d\n", 
                                                p_args2( adr, size ) );
	    break;
	case FREE:
	    dprintf("FREE ", p_args0 );
	    l = leak_q.flink;
	    while (l != &leak_q.flink ) {
		if ( adr == l->adr ) {
		    dprintf("( %x ) found element on leak queue - size:%d\n", 
                                              p_args2( adr, l->size ) );
		    remq_lock( l );
		    dyn$_free( l, DYN$K_SYNC|DYN$K_NOOWN );
		    return;
		}
		l = l->q.flink;
	    }
	    pprintf("( %x ) not found on allocation queue. \n", adr );
	    break;
	default:
	    pprintf("leak_watcher default\n");
	    break;
    }
}

dump_leak_queue() {

    struct LEAKDATA *l;

    pprintf("dump_leak_queue\n");
    l = leak_q.flink;
    while ( l != &leak_q.flink ) {
	pprintf("( %x ) found element on leak queue - size:%d  callerpc:%x\n", 
                                    l->adr, l->size, l->callerpc );
	l = l->q.flink;
    }  
}

/*
 * Helper routine for DYNAMIC that prints out a usage summary for a zone
 */

leak_dyn$_status ( struct ZONE *zp, int *header ) {
    
    int	i;
    struct DYNAMIC	*p;
    int	free_bytes, used_bytes;
    int	free_blocks, used_blocks;

    free_bytes = used_bytes = 0;
    free_blocks = used_blocks = 0;
    krn$_wait ( &zp->sync );
    p = ( void * ) &zp->header.adj_fl;

    /*
     * walk down the list
     */

    do {
	p = ( void * ) (( int ) p - adjacent_bias );
	if ( p->bsize > 0 ) {
	    free_bytes += p->bsize;
	    free_blocks++;
	} else {
	    used_bytes += abs ( p->bsize );
	    used_blocks++;
	}
	p = p->adj_fl;
	if (killpending ()) 
	    break;
    } while ( p != ( void * ) &zp->header.adj_fl );
    krn$_post ( &zp->sync );

    return used_bytes;
}

#endif


/*+
 * ============================================================================
 * = i2o_malloc - 							      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Allocate buffer
 *  
 * FORM OF CALL:
 *  
 *	i2o_malloc( x )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	int x - address of buffer
 *
 * SIDE EFFECTS:
 *
 *
-*/
i2o_malloc( int x ) {
    
    int *buf;

    buf = dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN);
#if DEBUG
    if ( i2o_dyn_debug ) 
	pprintf("malloc: %x %d %s ( pc:%x )\n", buf, x, callers_pc() );

    /* 
     * Flood with something bad to help find bugs. 
     */

    memset ( buf, i2o_flood_byte, x );  
    leak_watcher( MALLOC, buf, callers_pc(), x ); 
#endif

    return buf;
}

/*+
 * ============================================================================
 * = i2o_free - 							      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Free allocated buffer
 *  
 * FORM OF CALL:
 *  
 *	i2o_free( x )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	int x - address of buffer
 *
 * SIDE EFFECTS:
 *
 *
-*/
i2o_free( int x ) {
      
#if DEBUG  
    if ( i2o_dyn_debug ) 
	pprintf("free: %x %s ( pc:%x )\n", x, callers_pc() );
#endif
    dyn$_free( x, DYN$K_SYNC | DYN$K_NOOWN );

#if DEBUG
    leak_watcher( FREE, x, callers_pc(), 0 );
#endif
}


/*
 *********************************************************************
 *                                                                   *
 *  End of MEMORY LEAK routines                                      *
 *                                                                   *
 *********************************************************************
 */

/*
 *********************************************************************
 *                                                                   *
 *  Start of routines used exclusively for DRIVERSHUT                *
 *                                                                   *
 *********************************************************************
 */
#if DRIVERSHUT
/*+
 * ============================================================================
 * = i2o_setmode - change driver state					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	This routine changes the state of the port driver.  The following
 * 	states are relevant:
 *
 *	    DDB$K_STOP -
 *			Suspends the  port driver.  i2o_poll stops checking
 *			things in this state, and interrupts from the
 *			device are dismissed.
 *
 *	    DDB$K_RESTARTABLE -
 *			Same as DDB$K_STOP, except the port driver may be
 *			restarted dynamically by the class driver.
 *
 *	    DDB$K_START -
 *			Changes the   port   driver   to   interrupt  mode.
 *			i2o_poll is once again enabled, as well as device
 *			interrupt handling.
 *
 *	    DDB$K_INTERRUPT -
 *			Same as DDB$K_START.
 *  
 *	    DDB$K_ASSIGN  -	
 *			Perform system startup_functions.
 *
 * FORM OF CALL:
 *  
 *	i2o_setmode(mode, pb)
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *	msg_port_state_running - port running
 *       
 * ARGUMENTS:
 *
 *	int mode    	   - Desired mode for port driver.
 *	struct device *pb - pointer to port block information
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int i2o_setmode(int mode, struct pb *gpb ) {
    int i;
    struct i2o_pb *pb;
    struct sb *sb;
    struct osm *osm;
    struct QUEUE *p;

    switch (mode) {
	case DDB$K_ASSIGN: 
	    break;

	case DDB$K_START: 

	case DDB$K_INTERRUPT: 

	    if ( gpb->dpb ) {
		gpb->dpb->ref++;
		return msg_success;
	    } else {
		alloc_dpb( gpb, &pb, sizeof( struct i2o_pb ));
	    }

	    if (pb) {
		if (pb->pb.ref != 0) {
		    pb->pb.ref++;
		    return msg_success;
		}
	    }

	    krn$_set_console_mode(0);

	    pb->pb.ref++;
	    set_io_name( pb->pb.name, 0, 0, 0, pb );
	    krn$_seminit( &pb->hr_complete_s, 0, "hr_complete" );
	    krn$_seminit( &pb->owner_s, 1, "i2o_owner" );
	    krn$_seminit( &pb->reply_s, 0, "i2o_reply" );
	    pb->mf_pool = i2o_malloc( I2O_CNSL_MF_SIZE*I2O_CNSL_FIFO_SIZE );
	    pb->rq.flink = (void *) &pb->rq.flink;
	    pb->rq.blink = (void *) &pb->rq.flink;

	    if ( i2o_initialize_port( pb ) == msg_failure ) {
		i2o_free( pb->mf_pool );
		pb->pb.ref = 0;
		i2o_free( pb );
		gpb->dpb = 0;
		return ( msg_failure );
	    }
	    
	    pb->pb.sb = i2o_malloc( pb->pb.num_sb * sizeof( struct sb * ));

	    break;	    

	case DDB$K_STOP: 

	    pb = gpb->dpb;

	    if (!pb)
		return msg_failure;

/* If it was already shutdown, return */

	    if (pb->pb.ref == 0)
		return msg_success;

	    pb->pb.ref--;
	    if (pb->pb.ref != 0)
		return msg_success;

	    I2O_IOPshutdown( pb );

	    krn$_delete( pb->hr_pid );
	    krn$_post( &pb->reply_s );
	    krn$_wait( &pb->hr_complete_s );

	    krn$_semrelease( &pb->hr_complete_s );
	    krn$_semrelease( &pb->owner_s );
	    krn$_semrelease( &pb->reply_s );

	    for ( i = 0; i < pb->pb.num_sb; i++ ) {
		sb = pb->pb.sb[i];
		if ( sb ) 
		    if ( osm = i2o_osm( sb->class, I2O_NO_SUBCLASS )) 
			osm->shutdown( sb );
	    }

	    i2o_rq_free( pb );
	    i2o_free( pb->mf_pool );
	    i2o_free( pb->pb.sb );
	    i2o_free( pb->lct );
	    i2o_free( pb );

	    gpb->dpb = 0;
#if 0
	    dump_leak_queue();
#endif
	    break;

    }					/* switch */
    return msg_success;
}					/* i2o_setmode */

/*+
 * ============================================================================
 * = i2o_establish_connection - establish a connection to given unit        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine establishes a connection to a given unit.
 *  
 * FORM OF CALL:
 *  
 *	i2o_establish_connection( inode, pb, class )
 *  
 * RETURNS:
 *
 *	success or failure.
 *       
 * ARGUMENTS:
 *
 *	struct INODE *inode	- pointer to inode.
 *	struct pb *pb		- pointer to port block
 *	int class		- class of connection
 *
 * SIDE EFFECTS:
 *
 *	Connection established to given unit.
 *
-*/
i2o_establish_connection(struct INODE *inode, struct pb *gpb, int class) {

    i2o_poll_units( gpb, FALSE );
    return ( msg_success );
}

#endif
/*
 *********************************************************************
 *                                                                   *
 *  End of routines used exclusively for DRIVERSHUT		     *
 *                                                                   *
 *********************************************************************
 */


/*
 *********************************************************************
 *                                                                   *
 *  Start of routines shared between DRIVERSHUT and non-DRIVERSHUT   *
 *								     *
 *********************************************************************
 */

/*+
 * ============================================================================
 * = i2o_osm -								      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Obtains OSM from OSM table given class and subclass of the device.
 *  
 * FORM OF CALL:
 *  
 *	i2o_osm( class, subclass )
 *  
 * RETURNS:
 *
 *	osm - pointer to osm
 *       
 * ARGUMENTS:
 *
 *	int class - device class
 *	int subclass - device subclass
 *
 * SIDE EFFECTS:
 *
 *
-*/
struct osm *i2o_osm( int class, int subclass ) {

    int *p = i2o_osm_table;
    struct osm *osm; 
    int i;

    while ( osm = ( struct osm *) *p ) {
	if ( class == I2O_CLASS_BUS_ADAPTER_PORT  ) {
	    if ( osm->adapter_class == subclass )
		return ( osm );
	} else {
	    if (( osm->unit_class == class ) && ( subclass == I2O_NO_SUBCLASS ))
		return ( osm );
	}
	p++;
    }
    return ( NUL );
}

/*+
 * ============================================================================
 * = i2o_poll_units -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	For end devices found in Locical Configuration Table ( LCT ),
 *	call the OSM new unit routine associated with the end device
 *	I2O class. 
 *  
 *  
 * FORM OF CALL:
 *  
 *	i2o_poll_units(pb, verbose)
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pb *pb		- Pointer to device structure of adapter.
 *	int verbose		- if set, print node information.
 *
 * SIDE EFFECTS:
 *
 *	Files will be created.
 *
-*/
i2o_poll_units(struct pb *gpb, int verbose) {

    int i;
    int j;
    I2O_LCT *lct_head;
    I2O_LCT_ENTRY *lct;
    int status = msg_failure;
    struct i2o_pb *pb;
    struct osm *osm;

#if DRIVERSHUT
    pb = gpb->dpb;
#else
    pb = gpb;
#endif

    if ( pb->lct ) {
	lct_head = ( I2O_LCT * ) pb->lct;
	for ( i = 0; i < NUM_LCT_ENTRIES( lct_head ); i++ ) {
	    lct = &( lct_head->LCTEntry[i] );
	    if ( lct->UserTID == I2O_RESOURCE_NOT_CLAIMED ) {
#if 0
		if ( I2O_UtilClaim( pb, lct->LocalTID, I2O_CLAIM_TYPE_PRIMARY_USER, 0 ) == msg_success ) {
#endif
		if ( 1 )
		    if ( osm = i2o_osm( lct->ClassID.Class, I2O_NO_SUBCLASS ))
			osm->new_unit( pb, i, lct, verbose );
	    }
	}
    }
}

/*+
 * ============================================================================
 * = i2o_initialize_port -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Initialize the IOP.
 *	Obtain Logical Configuration Table.
 *      For any adapters found, call OSM adapter init routine.
 *  
 * FORM OF CALL:
 *  
 *	i2o_initialize_port( pb )
 *  
 * RETURNS:
 *
 *	nothing
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb pb - pointer to I2O port block 
 *
 * SIDE EFFECTS:
 *
 *
-*/
i2o_initialize_port( struct i2o_pb *pb ) {

    int status = msg_failure;
    struct osm *osm;
    int i;
    I2O_LCT *lct_head;
    I2O_LCT_ENTRY *lct;
    unsigned int lct_size;
    int adapter_count = 0, sb_count = I2O_MAX_SB;
    int *mfhdr;
    I2O_HRT_ENTRY *hrtbuf;

#if 0
    bpt();
#endif

    if( pb->pb.vector ) {
	pb->pb.mode = DDB$K_INTERRUPT;
	pb->pb.desired_mode = DDB$K_INTERRUPT;
    } else {
	pb->pb.mode = DDB$K_POLLED;
	pb->pb.desired_mode = DDB$K_POLLED;
    }

    sprintf( pb->pb.alias, "I2O%c0", pb->pb.controller + 'A' );

    sprintf( pb->pb.string, "%-24s   %-8s", pb->pb.name, pb->pb.alias );


    pb->hr_pid = krn$_create( I2O_handle_reply, null_procedure, &pb->hr_complete_s,
		     5, 1 << whoami(), 0, "I2O_reply", 0, 0, 0, 0, 0, 0, pb );

    /*
     * Init the IOP ( check status, init outbound queue );
     */

    if ( I2O_IOPinit( pb ) == msg_success ) {
	
#if 0
	/*
	 * Get the Hardware Resource Table. 
	 */
	    
	hrtbuf = i2o_malloc( sizeof( I2O_HRT_ENTRY ));
	I2O_ExecHRTGet( pb, hrtbuf, sizeof(I2O_HRT_ENTRY) );
	i2o_free( hrtbuf );
#endif

	/*
	 * Get the Logical Configuration Table and process the entries.
	 */

	lct_size = sizeof( I2O_LCT ) + sizeof( I2O_LCT_ENTRY ) * LCT_ENTRY_BLOCK;
	lct_head = i2o_malloc( lct_size );
	if ( i2o_get_lct( pb, &lct_head, lct_size ) == msg_success ) {
#if DEBUG
	    i2o_dump_lct( lct_head );
#endif
	    pb->lct = ( int * ) lct_head;
	    if ( lct_head->IopFlags != 0 )
		pprintf("IOP is requesting configuration dialogue\n");

	    if (( lct_head->LctVer == 0 ) || ( lct_head->LctVer == 1 )) {
		for ( i = 0; i < NUM_LCT_ENTRIES( lct_head ); i++ ) {
		    lct = &( lct_head->LCTEntry[i] );
		    if ( lct->ClassID.Class == I2O_CLASS_BUS_ADAPTER_PORT ) {
			if ( osm = i2o_osm( lct->ClassID.Class, lct->SubClassInfo ))
			    osm->adapter_init( pb, lct->LocalTID );
			    adapter_count++;
		    }
		}
		if ( adapter_count )
		    sb_count = adapter_count * I2O_MAX_SB;
		pb->pb.num_sb = sb_count;
		status = msg_success;
	    }
	    I2O_UtilEventRegister( pb, I2O_IOP_TID, I2O_EVENT_ALL_EVENTS, i2o_iop_event_handler );
	}
    }
    return ( status );
}

/*+
 * ============================================================================
 * = i2o_get_lct -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Issue Logical Configuration Table ( LCT ) Notify to obtain LCT.
 *  
 * FORM OF CALL:
 *  
 *	i2o_get_lct( pb, lct, lct_size )
 *  
 * RETURNS:
 *
 *	status - LCT obtained/ not obtained
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb pb - pointer to I2O port block 
 *	I2O_LCT lct - address of pointer to buffer for LCT
 *	unsigned int lct_size - size of buffer for LCT
 *
 * SIDE EFFECTS:
 *
 *
-*/
i2o_get_lct( struct i2o_pb *pb, I2O_LCT **lct, unsigned int lct_size ) {

    int status = msg_success;
    int lct_obtained = FALSE;
    I2O_LCT *lct_head = *lct;

    /* 
     * Send an ExecLctNotify request to obtain the device's Logical 
     * Configuration Table. If the buffer supplied is not large enough,
     * free it and malloc a new one.
     */

    krn$_wait( &pb->owner_s );
    while (( !lct_obtained ) && ( status != msg_failure )) {
	if ( I2O_ExecLctNotify( pb, I2O_CLASS_MATCH_ANYCLASS, lct_head, lct_size ) == msg_success ) {
	    if (( lct_head->TableSize * 4 ) > lct_size ) {
		lct_obtained = FALSE;
		i2o_free( lct_head );
	    	lct_size += sizeof( I2O_LCT_ENTRY ) * LCT_ENTRY_BLOCK;
		lct_head = i2o_malloc( lct_size );
		*lct = lct_head;
	    } else {
		lct_obtained = TRUE;
	    }
	} else {
	    status = msg_failure;
	}
    }
    krn$_post( &pb->owner_s );
    return ( status );
}

/*+
 * ============================================================================
 * = i2o_iop_event_handler -						      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Handle IOP events. If LCT event occurs, then poll for units.
 *	
 *  
 * FORM OF CALL:
 *  
 *	i2o_iop_event_handler( pb, mfhdr )
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb - IOP port block pointer
 *	I2O_CNSL_MFHDR *mfhdr - pointer to I2O message structure
 *	
 *
 * SIDE EFFECTS:
 *
 *
-*/
i2o_iop_event_handler( struct i2o_pb *pb, I2O_CNSL_MFHDR *mfhdr ) {

    I2O_UTIL_EVENT_REGISTER_REPLY  *evrsp;
    unsigned int event, data;
    int i;

    evrsp = mfhdr->reply;
    event = evrsp->EventIndicator;
    data = evrsp->EventData[0];

    I2O_ReplyFree( pb, evrsp );

#if EVENT_DEBUG
    pprintf("IOP EVENT: %x ", event );
    for ( i = 0; event_m[i].code; i++ ) {
	if ( event_m[i].code == event ) {
	    pprintf("%s ", event_m[i].message );
	    break;
	}
    }
    pprintf("( Data: %x )\n", data );
#endif
    switch ( event ) {
	case I2O_EVENT_IND_NEW_LCT_ENTRY:
	case I2O_EVENT_IND_MODIFIED_LCT:
#if !DRIVERSHUT
	    i2o_adapter_poll( pb );
#endif
	    break;
	case I2O_EVENT_IND_EVENT_MASK_MODIFIED:
	    break;
	default:
	    break;
    }

}

/*+
 * ============================================================================
 * = i2o_rq_malloc/free -						      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Allocate resources onto pb's resource queue. Free on shutdown.
 *  
 * FORM OF CALL:
 *  
 *	i2o_rq_malloc( pb, size )
 *  
 * RETURNS:
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb pb - pointer to I2O port block 
 *	unsigned int size - size of buffer
 *
 * SIDE EFFECTS:
 *
 *
-*/
i2o_rq_malloc( struct i2o_pb *pb, unsigned int size ) {
    
    int p;
    struct QUEUE *q;

    q = i2o_malloc( sizeof( *q ) + size );
    insq_lock( q, pb->rq.blink );
    p = ( int ) q + sizeof( *q );
    return ( p );
}
i2o_rq_free( struct i2o_pb *pb ) {

    struct QUEUE *p;
    
    while( pb->rq.flink != &pb->rq.flink ) {
	p = pb->rq.flink;
	remq_lock( p );
	i2o_free( p );
    }
}

/*+
 * ============================================================================
 * = I2O_IOPInit - Initialize the IOP.
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Initialize the IOP.
 *
 * FORM OF CALL:
 *  
 *	I2O_IOPInit( pb )
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
I2O_IOPinit( struct i2o_pb *pb ) {

    int				    i;
    char			    *buf;
    I2O_EXEC_STATUS_GET_MESSAGE	    *mfa;
    I2O_EXEC_STATUS_GET_REPLY	    *execstatusget_buf;
    I2O_EXEC_OUTBOUND_INIT_STATUS   *outboundinit_buf;
    I2O_LCT			    *lct_head;
    unsigned int		    base0[2];
    struct TIMERQ		    tqe, *tq;
    int status = msg_success;

    tq = &tqe;

    dprintf("\n*** I2O_IOPinit - pb;%x\n", p_args1( pb ));

    base0[0] = incfgl( pb, BASE0 ) & 0xFFFFFFF0;
    base0[1] = 0;
    dprintf("base0[0] %x    base0[1] %x\n", p_args2( base0[0], base0[1] ));
    pb->mem_base  =  *(unsigned __int64*)base0;
    dprintf("pb->mem_base ( %x ) %x \n", p_args2( &pb->mem_base, pb->mem_base ));
 
    buf = i2o_malloc( sizeof( I2O_EXEC_STATUS_GET_REPLY ) +
		      sizeof( I2O_EXEC_OUTBOUND_INIT_STATUS ));
    
    execstatusget_buf = buf;
    outboundinit_buf = ( char * ) buf + 
				sizeof( I2O_EXEC_STATUS_GET_REPLY );

    /*
     * Reset the IOP if it's not already in the RESET state.
     */

    pb->state = I2O_ExecStatusGet( pb, execstatusget_buf );
    if ( pb->state != I2O_IOP_STATE_RESET ) {
	start_timeout( I2O_RESET_TIMEOUT, tq );
	I2O_ExecIopReset( pb );
	while (( pb->state != I2O_IOP_STATE_RESET ) && ( !tq->sem.count )) { 
	    krn$_sleep( 1000 );
	    pb->state = I2O_ExecStatusGet( pb, execstatusget_buf );
	}
	if ( tq->sem.count ) {
	    pprintf("IOP failed to reset\n");
	    status = msg_failure;
	}
	stop_timeout( tq );
    }

    /*
     * Wait for IOP to init the inbound queue.
     */

    if ( status == msg_success ) {

	start_timeout( I2O_INBOUND_TIMEOUT, tq );
	mfa = ( void * )I2O_read_mem( pb, INBOUND_FIFO );
	while ( ( mfa == I2O_EMPTY_FIFO ) && ( !tq->sem.count )) { 
	    krn$_sleep( 10 );
	    mfa = ( void* )I2O_read_mem( pb, INBOUND_FIFO );
	}
	if ( tq->sem.count ) {
	    pprintf("IOP failed to init inbound\n");
	    status = msg_failure;
	}
	stop_timeout( tq );
    }

    /*
     * Init the outbound queue.
     */

    if ( status == msg_success ) {

	dprintf("IOPinit - IOP is alive - mfa:%x\n", mfa );

	/*
	 *  TODO:  Need to find a way to get this mfa back to the IOP
	 *         Issue a noop when I can.
	 */

	/*
	 * Send and ExecGetStatus.  The outbound (reply) queue is not 
	 * initialized yet so the host polls the status buffer to find out 
	 * when the IOP has responded.  This will indicate the IOP is 
	 * functional.
	 *
	 * The ExecGetStatus also serves to tell the IOP the host has completed
	 * the system scan and the IOP can now build the HRT. 
	 * Init the outbound queue. This moves the IOP from the RESET state 
	 * to the HOLD state.
	 */

	start_timeout( I2O_OUTBOUND_TIMEOUT, tq );
	I2O_ExecOutboundInit( pb, outboundinit_buf );
	dprintf("  1-IN_PROG  2-REJ  3-FAILED  4-COMPLETE \n", p_args0 );
	while(( *( volatile int * ) &outboundinit_buf->InitStatus != 
		I2O_EXEC_OUTBOUND_INIT_COMPLETE ) && ( !tq->sem.count )) {
		krn$_sleep( 10 );
	}
	if ( tq->sem.count ) {
	    pprintf("IOP failed to init outbound\n");
	    status = msg_failure;
	}
	stop_timeout( tq );
	dprintf("I2O_EXEC_OUTBOUND_INIT_STATUS %x\n", p_args1( outboundinit_buf->InitStatus ));
    }

    /*
     * Prime the outbound queue. Enable interrupts. IOP should be in HOLD
     * state. ExecSysTabSet will move it to READY state. I2O_restart will
     * move it to OPERATIONAL state.
     */

    if ( status == msg_success ) {
	I2O_prime_outbound( pb );
	status = I2O_restart( pb );
    }

    i2o_free( buf );
    return ( status );
}

/*+
 * ============================================================================
 * = I2O_restart -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Restart the driver.
 *	
 *  
 * FORM OF CALL:
 *  
 *	I2O_restart( pb )
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
-*/
I2O_restart( struct i2o_pb *pb ) {

    I2O_EXEC_STATUS_GET_REPLY   *execstatusget_buf;
    int status = msg_success;
    
    execstatusget_buf = i2o_malloc( sizeof( I2O_EXEC_STATUS_GET_REPLY ));
    
    if ( pb->pb.vector ) {
	int_vector_set( pb->pb.vector, I2O_interrupt, pb );
	io_enable_interrupts( pb, pb->pb.vector );
	I2O_write_mem( pb, I2O_INTERRUPT_MASK_REG, 0 );
    } else {
	pb->pb.pq.routine = I2O_interrupt;
	pb->pb.pq.param = pb;
	insq_lock( &pb->pb.pq.flink, &pollq ); 
    }

    pb->state = I2O_ExecStatusGet( pb, execstatusget_buf );
    
    if ( pb->state == I2O_IOP_STATE_HOLD ) {
	I2O_ExecSysTabSet( pb );
    }
    pb->state = I2O_ExecStatusGet( pb, execstatusget_buf );
    if ( pb->state == I2O_IOP_STATE_READY ) {
	I2O_ExecSysEnable( pb );
    }

    /*
     * IOP should now be in the OPERATIONAL state.
     */ 

    pb->state = I2O_ExecStatusGet( pb, execstatusget_buf );
    if ( pb->state != I2O_IOP_STATE_OPERATIONAL ) {
	pprintf("IOP failure - state %d\n", pb->state );
	status = msg_failure;
    }
    i2o_free( execstatusget_buf );
    return ( status );
}

/*+
 * ============================================================================
 * = I2O_IOPShutdown - Shutdown the IOP.
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Shutdown the IOP.
 *
 * FORM OF CALL:
 *  
 *	I2O_IOPShutdown( pb )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct i2o_pb *pb - pointer to the IOP's port block
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
I2O_IOPshutdown( struct i2o_pb *pb ) {

    pb->state = 0;
#if 0
    I2O_ExecSysQuiesce( pb );
#endif

    if ( pb->pb.vector ) {
	I2O_write_mem( pb, I2O_INTERRUPT_MASK_REG, 0xFFFFFFFF );
	io_disable_interrupts( pb, pb->pb.vector );
	int_vector_clear( pb->pb.vector );
    } else {
	remq_lock ( &pb->pb.pq.flink );
    }
}

/*
 *********************************************************************
 *                                                                   *
 *  End of routines shared between DRIVERSHUT and non-DRIVERSHUT     *
 *								     *
 *********************************************************************
 */




/*
 *********************************************************************
 *                                                                   *
 *  Start of routines used exclusively for non-DRIVERSHUT	     *
 *                                                                   *
 *********************************************************************
 */
#if !DRIVERSHUT
/*+
 * ============================================================================
 * = i2o_init -								      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Start the driver for I2O ports in the system.
 *	
 *  
 * FORM OF CALL:
 *  
 *	i2o_init( pb )
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *
-*/
i2o_init() {

    int i2o_start( );

    find_pb( "pz", sizeof( struct i2o_pb ), i2o_start );
    return( msg_success );
}

/*+
 * ============================================================================
 * = i2o_start -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Initialize the port and poll for units. Establish event handler
 *	for IOP.
 *	
 *  
 * FORM OF CALL:
 *  
 *	i2o_start( pb )
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
i2o_start( struct i2o_pb *pb ) {
    pb->pb.setmode = i2o_setmode;
    set_io_name( pb->pb.name, 0, 0, 0, pb );
    krn$_seminit( &pb->hr_complete_s, 0, "hr_complete" );
    krn$_seminit( &pb->owner_s, 1, "i2o_owner" );
    krn$_seminit( &pb->reply_s, 0, "i2o_reply" );
    pb->mf_pool = i2o_malloc( I2O_CNSL_MF_SIZE*I2O_CNSL_FIFO_SIZE );
    pb->rq.flink = (void *) &pb->rq.flink;
    pb->rq.blink = (void *) &pb->rq.flink;
    if ( i2o_initialize_port( pb ) == msg_success ) {
	pb->pb.sb = i2o_malloc( pb->pb.num_sb * sizeof( struct sb * ));
	i2o_poll_units( pb, FALSE );
    }
}


/*+
 * ============================================================================
 * = i2o_adapter_poll -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	This is called bu the IOP event handler if the LCT changes.
 *	Obtain the LCT and poll for units.
 *	
 *  
 * FORM OF CALL:
 *  
 *	i2o_adapter_poll( pb )
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
void i2o_adapter_poll( struct i2o_pb *pb ) {
    I2O_LCT *current_lct = pb->lct;
    I2O_LCT *new_lct;
    unsigned int new_lct_size;

    /*
     * The LCT has changed, poll for units.
     */

     new_lct_size = current_lct->TableSize * 4;
     new_lct = i2o_malloc( new_lct_size );
     if ( i2o_get_lct( pb, &new_lct, new_lct_size ) == msg_success ) {
	i2o_free( current_lct );
	pb->lct = ( int * ) new_lct;
     }
     if ( scsi_poll )
	i2o_poll_units( pb, FALSE );
}

/*+
 * ============================================================================
 * = i2o_setmode -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	Change state of the driver, START, STOP etc.
 *  
 * FORM OF CALL:
 *  
 *	i2o_setmode( pb )
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	pb - I2O port block pointer
 *	poll - poll/not poll
 *
 * SIDE EFFECTS:
 *
 *
-*/
void i2o_setmode( struct i2o_pb *pb, int mode ) {

    switch( mode ) {
	case DDB$K_STOP:
	    if( pb->pb.mode != DDB$K_STOP ) {
		krn$_wait( &pb->owner_s );
		I2O_IOPshutdown( pb );
		pb->pb.mode = DDB$K_STOP;
	    }
	    break;	    

	case DDB$K_START:
	    if( pb->pb.mode == DDB$K_STOP ) {
		pb->pb.mode = pb->pb.desired_mode;
		I2O_IOPinit( pb );
#if 0
		pb->poll_active = 1;
#endif
		krn$_post( &pb->owner_s );
		i2o_adapter_poll( pb );
	    }
	    break;

	case DDB$K_READY:
	    if( pb->pb.mode != DDB$K_STOP ) {
		while( pb->poll_active )
		    krn$_sleep( 100 );
	    }
	    break;

#if 0
	case DDB$K_POLLED:
	    if( pb->pb.mode == DDB$K_STOP ) {
		pb->pb.mode = DDB$K_POLLED;
		krn$_post( &pb->owner_s );
	    }
	    break;
#endif
    }
}
#endif
/*
 *********************************************************************
 *                                                                   *
 *  End of routines used exclusively for non-DRIVERSHUT		     *
 *                                                                   *
 *********************************************************************
 */
