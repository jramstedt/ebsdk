/*
 * Copyright (C) 1998 by
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
 * Abstract:	Adaptec AIC-78xx SCSI Port Driver
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	27-Mar-1998	Initial entry.
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:scsi_def.h"
#include "cp$src:aic78xx_def.h"
#include "cp$src:aic78xx_chim.h"
#include "cp$src:aic78xx_scsi.h"
#include "cp$src:aic78xx_pb_def.h"
#include "cp$src:aic78xx_him_xlm.h"
#include "cp$src:aic78xx_driver.h"
#include "cp$inc:platform_io.h"

#define __AIC78XX__	1
protoif(__AIC78XX__)
#include "cp$inc:prototypes.h"

extern struct LOCK spl_kernel;
extern struct QUEUE pollq;
#if !( STARTSHUT || DRIVERSHUT )
extern int scsi_poll;
#endif

extern int ev_initing;

extern ev_sev( );
extern ev_sev_init( );

int aic78xx_ev_sev( char *name, struct EVNODE *ev );

struct env_table aic78xx_host_id_ev = {
	0, 7,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0, aic78xx_ev_sev, 0, ev_sev_init
};

struct SEMAPHORE aic78xx_owner_s;

HIM_FUNC_PTRS SCSIRoutines;

HIM_OSM_FUNC_PTRS OSMRoutines = {
    0,                                     /* version number */
    OSMMapIOHandle,
    OSMReleaseIOHandle,
    OSMEvent,
    OSMGetBusAddress,
    OSMAdjustBusAddress,
    OSMGetNVSize,
    OSMPutNVData,
    OSMGetNVData,
    OSMReadUExact8,
    OSMReadUExact16,
    OSMReadUExact32,
    OSMReadStringUExact8,
    OSMReadStringUExact16,
    OSMReadStringUExact32,
    OSMWriteUExact8,
    OSMWriteUExact16,
    OSMWriteUExact32,
    OSMWriteStringUExact8,
    OSMWriteStringUExact16,
    OSMWriteStringUExact32,
    OSMSynchronizeRange,
    OSMWatchdog,
    OSMSaveInterruptState,
    OSMSetInterruptState,
    OSMReadPCIConfigurationDword,
    OSMReadPCIConfigurationWord,
    OSMReadPCIConfigurationByte,
    OSMWritePCIConfigurationDword,
    OSMWritePCIConfigurationWord,
    OSMWritePCIConfigurationByte,
    OSMDelay
};

/***************************************************************************
*  Function prototypes
***************************************************************************/
void EnqueueOsmIOB(OSMIOB *pOsmIOB,TARGET *pTarget);
void SendIOBsMaybe(TARGET *pTarget);
OSMIOB *AllocOSMIOB(ADAPTER *);
void FreeOSMIOB(ADAPTER *,OSMIOB *);
HIM_UEXACT8 OSMIntHandler(HIM_TASK_SET_HANDLE);            
HIM_UINT32 NormalPostRoutine(HIM_IOB *);
void HandleHIMFrozen(HIM_IOB *pIob);
HIM_UINT32 PostUnfreezeHIMQueue(HIM_IOB *);
HIM_UINT32 PostProtocolAutoConfig(HIM_IOB *);
HIM_UINT32 PostRoutineEventPAC (HIM_IOB *iob);
HIM_UINT32 PostRoutineEventResetHW (HIM_IOB *iob);

int aic78xx_init( void );
void aic78xx_init_pb( struct aic78xx_pb *pb );
void aic78xx_reset( struct aic78xx_pb *pb );
int aic78xx_start_adapter( struct aic78xx_pb *pb );
void aic78xx_stop_adapter( struct aic78xx_pb *pb );
int aic78xx_init_adapter( struct aic78xx_pb *pb, HIM_TASK_SET_HANDLE initTSH );
void aic78xx_scan_adapter( struct aic78xx_pb *pb );
int aic78xx_command( struct scsi_sb *sb, int lun,
	unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int dat_out_len,
	unsigned char *dat_in, int dat_in_len, unsigned char *sts,
	unsigned char *sense_dat_in, int *sense_dat_in_len );
void aic78xx_poll( struct aic78xx_pb *pb );
void aic78xx_setmode( struct aic78xx_pb *pb, int mode );
void aic78xx_poll_for_interrupt( struct aic78xx_pb *pb );
void aic78xx_interrupt( struct aic78xx_pb *pb );
void GetHIMFunctions( void );
void AllocateAdapterMemory( struct aic78xx_pb *pb, ADAPTER *pAdapter, HIM_TASK_SET_HANDLE initTSH );
void AllocateTargets( struct aic78xx_pb *pb, ADAPTER *pAdapter );
void InitOsmIOBPool( struct aic78xx_pb *pb, ADAPTER *pAdapter );

void *AllocMemory( struct aic78xx_pb *pb, HIM_UINT32 size );
void FreeMemory( void *p );
void FreeAllMemory( struct aic78xx_pb *pb );
HIM_BUS_ADDRESS VirtualToPhysical( struct aic78xx_pb *pb, void *va );

#define prefix(x) set_io_prefix(pb,name,x)
#define pprintf		qprintf

int aic78xx_ev_sev( char *name, struct EVNODE *ev )
    {
    char c;
    unsigned int id;
    int t;
    struct aic78xx_pb *pb;

    pb = ev->misc;
    c = name[5];
    if( c == 'h' )
	{
	id = ev->value.integer;
	if( id > 15 )
	    id -= 6;
	if( id > 15 )
	    {
	    ev->value.integer = 7;
	    err_printf( msg_ev_badvalue, name );
	    return( msg_failure );
	    }
	if( id > 9 )
	    ev->value.integer = id + 6;
	else
	    ev->value.integer = id;
	if( ev_initing )
	    pb->ev_node_id = id;
	}
    if( ev_initing )
	return( msg_success );
    else
	return( ev_sev( name, ev ) );
    }

int aic78xx_init( void )
    {
    krn$_seminit( &aic78xx_owner_s, 1, "aic78xx_him_owner" );

    /* Get CHIM Function Pointers */
    GetHIMFunctions( );

    find_pb( "aic78xx", sizeof( struct aic78xx_pb ), aic78xx_init_pb );
    return( msg_success );
    }

void aic78xx_init_pb( struct aic78xx_pb *pb )
    {
    char name[32];
    int t;

    log_driver_init( pb );
    pb->node_id = 7;
    pb->ev_node_id = 7;
    pb->nvram_node_id = -1;
    aic78xx_host_id_ev.ev_name = prefix( "_host_id" );
    ev_init_ev( &aic78xx_host_id_ev, pb );
    pb->malloc_queue.flink = &pb->malloc_queue;
    pb->malloc_queue.blink = &pb->malloc_queue;
    krn$_seminit( &pb->owner_s, 1, "aic78xx_owner" );
#if !( STARTSHUT || DRIVERSHUT )
    pb->pb.setmode = aic78xx_setmode;
#endif
    pb->pb.pq.routine = aic78xx_poll_for_interrupt;
    pb->pb.pq.param = pb;
    if( pb->pb.vector )
	{
	pb->pb.mode = DDB$K_INTERRUPT;
	pb->pb.desired_mode = DDB$K_INTERRUPT;
	int_vector_set( pb->pb.vector, aic78xx_interrupt, pb );
	io_enable_interrupts( pb, pb->pb.vector );
	}
    else
	{
	pb->pb.mode = DDB$K_POLLED;
	pb->pb.desired_mode = DDB$K_POLLED;
	insq_lock( &pb->pb.pq.flink, &pollq );
	}
    pb->pb.sb = malloc_noown( 16 * sizeof( struct scsi_sb * ) );
    pb->pb.num_sb = 16;
    krn$_seminit( &pb->isr_t.sem, 0, "aic78xx_isr" );
    krn$_init_timer( &pb->isr_t );
    krn$_wait( &pb->owner_s );
    t = aic78xx_start_adapter( pb );
    krn$_post( &pb->owner_s );
    aic78xx_reset( pb );
    set_io_name( pb->pb.name, 0, 0, pb->node_id, pb );
    set_io_alias( pb->pb.alias, 0, 0, pb );
    sprintf( pb->pb.info, "SCSI Bus ID %d", pb->node_id );
    sprintf( pb->pb.string, "%-24s   %-8s   %24s",
	    pb->pb.name, pb->pb.alias, pb->pb.info );
    if( t == msg_success )
	{
	pb->poll_active = 1;
#if !( STARTSHUT || DRIVERSHUT )
	krn$_create( aic78xx_poll, 0, 0, 5, 0, 4096,
		prefix( "_poll" ), "nl", "r", "nl", "w", "nl", "w", pb, 1 );
#endif
	}
    }

void aic78xx_reset( struct aic78xx_pb *pb )
    {
    int i;
    struct scsi_sb *sb;

    /*
     *  Statically allocate an SB for each possible node connected to this
     *  port.  The AIC-78xx supports up to sixteen attached nodes (the AIC-78xx 
     *  counts as one but for simplicity we ignore this fact).  Fill in each
     *  SB.
     */
    for( i = 0; i < 16; i++ )
	{
	sb = malloc_noown( sizeof( *sb ) );
	pb->pb.sb[i] = sb;
	sb->pb = pb;
	sb->ub.flink = &sb->ub.flink;
	sb->ub.blink = &sb->ub.flink;
	sb->command = aic78xx_command;
	sb->ok = 1;
	sb->node_id = i;
	if( pb->node_id == i )
	    sb->local = 1;
	set_io_name( sb->name, 0, 0, sb->node_id, pb );
#if ( STARTSHUT || DRIVERSHUT )
        krn$_seminit( &sb->ready_s, 0, "scsi_sb_ready" );
#endif
	}
    }

int aic78xx_start_adapter( struct aic78xx_pb *pb )
    {
    int t;
    void *initTSCB;
    HIM_TASK_SET_HANDLE initTSH;

    krn$_wait( &aic78xx_owner_s );
    initTSCB = malloc_noown( HIM_SCSI_INIT_TSCB_SIZE );
    initTSH = HIMtCreateInitializationTSCB( initTSCB );
    t = aic78xx_init_adapter( pb, initTSH );
    if( t == msg_success )
	aic78xx_scan_adapter( pb );
    free( initTSCB );
    krn$_post( &aic78xx_owner_s );
    return( t );
    }

void aic78xx_stop_adapter( struct aic78xx_pb *pb )
    {
    ADAPTER *pAdapter;

    krn$_wait( &aic78xx_owner_s );
    pb->online = 0;
    pAdapter = pb->adapter;
#if 0
    HIMtRestoreState( pAdapter->adapterTSH, pAdapter->pHWState );
#endif
    outmemb( pb, pb->pb.csr + SCSI_HCNTRL, 1 );
    FreeAllMemory( pb );
    krn$_post( &aic78xx_owner_s );
    }

int aic78xx_init_adapter( struct aic78xx_pb *pb, HIM_TASK_SET_HANDLE initTSH )
    {
    ADAPTER *pAdapter;
    HIM_TASK_SET_HANDLE adapterTSH;

    pAdapter = AllocMemory( pb, sizeof( ADAPTER ) );
    pAdapter->pciLocation.pciAddress.treeNumber = pb->pb.hose;
    pAdapter->pciLocation.pciAddress.busNumber = pb->pb.bus;
    pAdapter->pciLocation.pciAddress.deviceNumber = pb->pb.slot;
    pAdapter->pciLocation.pciAddress.functionNumber = pb->pb.function;
    pAdapter->productID = incfgl( pb, 0x00 );
    pAdapter->pb = pb;
    pb->adapter = pAdapter;

    /* Get and modify configuration, which must be done
     * before adapter TSCB is generated. */
    HIMtGetConfiguration( initTSH,
	    &pAdapter->configuration, pAdapter->productID );

    /* Assume we want to reduce HIM memory utilization */
    if( pAdapter->configuration.maxInternalIOBlocks > 4 )
	pAdapter->configuration.maxInternalIOBlocks = 4;

    HIMtSetConfiguration( initTSH,
	    &pAdapter->configuration, pAdapter->productID );

    /* Create adapter Task Set Control Block, and have HIM
     * assign AdapterTSH */
    pAdapter->pTscb = AllocMemory( pb,
	    HIMtSizeAdapterTSCB( initTSH, pAdapter->productID ) );

    pAdapter->adapterTSH = HIMtCreateAdapterTSCB( initTSH,
	    pAdapter->pTscb, pAdapter,
	    pAdapter->pciLocation, pAdapter->productID );
    adapterTSH = pAdapter->adapterTSH;

    HIMtSetupAdapterTSCB( adapterTSH,
	    &OSMRoutines, sizeof( HIM_OSM_FUNC_PTRS ) );

    /* Verify adapter hardware is accessible.  Within this call, HIM
     * calls OSMMapIOHandle to gain access to hardware registers.
     * Until this point, OSM can only 'see' the hardware through
     * PCI configuration space. */
    if( HIMtVerifyAdapter( adapterTSH ) != 0 )
	{
	/* code to abandon initialization */
	/* free adapter structure, etc. */
	return( msg_failure );
	}

    /* Separate call to allocate more HIM memory chunks */
    AllocateAdapterMemory( pb, pAdapter, initTSH );

    /* Get profile, package important fields separately */
    HIMtProfileAdapter( adapterTSH, &pAdapter->profile );
    pAdapter->iobReserveSize = pAdapter->profile.AP_IOBReserveSize;

    /* Update adapter profile to effect change(s) above */
    HIMtAdjustAdapterProfile( adapterTSH, &pAdapter->profile );
   
#if 0
    /* Save the pre-initialized (real-mode) state */
    /* Not all OSM's need to do this, but if this hardware state
     * is needed, OSM must fetch it before HIMInitialize. */
    pAdapter->pHWState = 
	    (void *)AllocMemory( pb, (int)pAdapter->profile.AP_StateSize );
    HIMtSaveState( adapterTSH, pAdapter->pHWState );
#endif

    /* Get IOBs for Adapter */
    InitOsmIOBPool( pb, pAdapter );

    /* Initialize adapter.  Adapter is now ready to generate
     * interrupts for external events (bus resets, etc.), but
     * is not quite ready for sending new requests. */

    spinlock( &spl_kernel );	/* disable system interrupts */
    if( HIMtInitialize( adapterTSH ) != 0 )
	{
	/* hardware failure. abandon adapter. (HIM has already
	 * retried the initialize.) */
	}
    pb->online = 1;
    spinunlock( &spl_kernel );	/* enable system interrupts */

    return( msg_success );
    }

void aic78xx_scan_adapter( struct aic78xx_pb *pb )
    {
    ADAPTER *pAdapter = pb->adapter;
    TARGET *pTarget;
    OSMIOB *pOsmIOB;
    HIM_TASK_SET_HANDLE adapterTSH = pAdapter->adapterTSH;
    int i;

    /* Because this is also used for re-scan, there may be active
     * items.  Set flag to ensure nothing else goes to the HIM,
     * then wait for everything to clear */
    pAdapter->AutoConfigActive = HIM_TRUE;

    /* Turn interrupts on, let active requests finish. */
    while( pAdapter->NumActiveIOBs > 0 )
	krn$_sleep( 10 );

    /* Disable hardware interrupt for re-entrancy protection.
     * Could disable system interupts instead, if desired. 
     * HIMDisableIRQ uses PIO to poke hardware registers, so it's
     * expensive. */
    HIMtDisableIRQ( adapterTSH );

    /* Make sure we have an up-to-date adapter profile */
    HIMtProfileAdapter( adapterTSH, &pAdapter->profile );

    /* Set host SCSI ID */
    if( pb->nvram_node_id == -1 )
	{
	pb->nvram_node_id = pAdapter->profile.himu.TS_SCSI.AP_SCSIAdapterID;
	if( pb->nvram_node_id == 7 )
	    pb->node_id = pb->ev_node_id;
	else
	    pb->node_id = pb->nvram_node_id;
	}
    pAdapter->profile.himu.TS_SCSI.AP_SCSIAdapterID = pb->node_id;

    /* Set number of LUNs per target */
    for( i = 0; i < pAdapter->profile.AP_MaxTargets; i++ )
	if( pAdapter->profile.himu.TS_SCSI.AP_SCSINumberLuns[i] > 8 )
	    pAdapter->profile.himu.TS_SCSI.AP_SCSINumberLuns[i] = 8;

    /* Update adapter profile to effect change(s) above */
    HIMtAdjustAdapterProfile( adapterTSH, &pAdapter->profile );

    /* Get an pOsmIOB for protocol auto config, and write out fields */
    pOsmIOB = AllocOSMIOB( pAdapter );
    pOsmIOB->chimObject.pAdapter = pAdapter;
    pOsmIOB->iob.function = HIM_PROTOCOL_AUTO_CONFIG;
    pOsmIOB->iob.taskSetHandle = adapterTSH;
    pOsmIOB->iob.postRoutine = PostProtocolAutoConfig;

    pAdapter->NumActiveIOBs++;
#if DEBUG
    pprintf( "...queueing pac\n" );
#endif
    HIMtQueueIOB( &pOsmIOB->iob );
    HIMtEnableIRQ( adapterTSH );

    /* wait around here until finished */
    while( pOsmIOB->iob.taskStatus == 0 )
	krn$_sleep( 100 );
#if DEBUG
    pprintf( "...pac completed\n" );
#endif

    /* If auto-config went OK, look for targets. */
    if( pOsmIOB->iob.taskStatus == HIM_IOB_GOOD )
	{
	HIMtDisableIRQ( adapterTSH );
	/* check target memory requirement, allocate target memory
	 * and validate target TSCB */
#if DEBUG
	pprintf( "...allocating targets\n" );
#endif
	AllocateTargets( pb, pAdapter );
#if DEBUG
	pprintf( "found %d targets => ", pAdapter->numberTargetTSH );
	for( i = 0; i < pAdapter->numberTargetTSH; i++ )
	    {
	    pTarget = pAdapter->pAttachedTarget[i];
	    pprintf( "ID %d, LUN %d;  ",
		    pTarget->profile.himu.TS_SCSI.TP_SCSI_ID,
		    pTarget->profile.himu.TS_SCSI.TP_SCSILun );
	    }
	pprintf( "\n" );
#endif
	/* Finally release queue to normal operations */
	pAdapter->AutoConfigActive = HIM_FALSE;
	/* enable hardware interrupt */
	HIMtEnableIRQ( adapterTSH );
#if 0
	/* If requests were queued (during our blocking, for example)
	 * we need to make sure they go out */
	for( i = 0; i < pAdapter->numberTargetTSH; i++ )
	    SendIOBsMaybe( pAdapter->pAttachedTarget[i] );
#endif
	}
    else
	{
	/* retry auto config? abandon? */
	}
    }

int aic78xx_command( struct scsi_sb *sb, int lun,
	unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int dat_out_len,
	unsigned char *dat_in, int dat_in_len, unsigned char *sts,
	unsigned char *sense_dat_in, int *sense_dat_in_len )
    {
    int i;
    int t;
    struct aic78xx_pb *pb;
    ADAPTER *pAdapter;
    TARGET *pTarget;
    OSMIOB *pOsmIOB;
    SGELEMENT sgElement;
    IOREQUEST ioRequest;

    pb = sb->pb;
    krn$_wait( &pb->owner_s );
    /* find the target hanging off the adapter */
    pAdapter = pb->adapter;
    for( i = 0; i < pAdapter->numberTargetTSH; i++ )
	{
	pTarget = pAdapter->pAttachedTarget[i];
	if( ( pTarget->profile.himu.TS_SCSI.TP_SCSI_ID == sb->node_id )
		&& ( pTarget->profile.himu.TS_SCSI.TP_SCSILun == lun ) )
	    break;
	}
    if( i == pAdapter->numberTargetTSH )
	{
	krn$_post( &pb->owner_s );
	return( msg_failure );
	}

    /* iobReserve set up in advance */
    /* OSM needn't zero out on every usage (to improve performance) */
    pOsmIOB = AllocOSMIOB( pAdapter );
    pOsmIOB->chimObject.pTarget = pTarget;

    /* setup iob */
    pOsmIOB->iob.function = HIM_INITIATE_TASK;
    pOsmIOB->iob.flagsIob.autoSense = 1;
    pOsmIOB->iob.flagsIob.freezeOnError = 0;

    pOsmIOB->iob.transportSpecific = 0;
    pOsmIOB->iob.osRequestBlock = 0;

    pOsmIOB->iob.taskSetHandle = pTarget->targetTSH;
    pOsmIOB->iob.postRoutine = NormalPostRoutine;

    pOsmIOB->iob.targetCommand = cmd;
    pOsmIOB->iob.targetCommandLength = cmd_len;

    sgElement.length = 0x80000000;
    if( dat_out_len )
	{
	sgElement.address = VirtualToPhysical( pb, dat_out );
	sgElement.length |= dat_out_len;
	}
    if( dat_in_len )
	{
	sgElement.address = VirtualToPhysical( pb, dat_in );
	sgElement.length |= dat_in_len;
	}

    pOsmIOB->iob.data.virtualAddress = &sgElement;
    pOsmIOB->iob.data.busAddress = 
	    OSMGetBusAddress( pAdapter->adapterTSH, HIM_MC_LOCKED, &sgElement );
    pOsmIOB->iob.data.bufferSize = sizeof( sgElement );

    pOsmIOB->iob.errorData = sense_dat_in;
    pOsmIOB->iob.errorDataLength = MAX_SENSE_SZ;

    pOsmIOB->iob.taskAttribute = HIM_TASK_SIMPLE;

    pOsmIOB->iob.priority = 0;
    pOsmIOB->iob.graphNode = 0;
    pOsmIOB->iob.sortTag = 0;
    pOsmIOB->iob.relatedIob = 0;

    pOsmIOB->iob.taskStatus = 0;
    pOsmIOB->iob.residual = 0;
    pOsmIOB->iob.residualError = 0;

    pOsmIOB->iob.osRequestBlock = &ioRequest;
    ioRequest.status = 0;

    krn$_wait( &aic78xx_owner_s );
    EnqueueOsmIOB( pOsmIOB, pTarget );
    while( ioRequest.status == 0 )
	krn$_sleep( 1 );
    *sts = scsi_k_sts_good;
    t = msg_success;
    if( pOsmIOB->iob.taskStatus == HIM_IOB_ERRORDATA_VALID )
	{
	*sense_dat_in_len = MAX_SENSE_SZ - pOsmIOB->iob.residualError;
	*sts = scsi_k_sts_check;
	}
    else if( ( pOsmIOB->iob.taskStatus != HIM_IOB_GOOD )
	    && ( pOsmIOB->iob.taskStatus != HIM_IOB_DATA_OVERUNDERRUN ) )
	{
	pprintf( "pOsmIOB->iob.taskStatus = %x\n", pOsmIOB->iob.taskStatus );
	t = msg_failure;
	}
    FreeOSMIOB( pAdapter, pOsmIOB );
    krn$_post( &aic78xx_owner_s );

    krn$_post( &pb->owner_s );
    return( t );
    }

#if !( STARTSHUT || DRIVERSHUT )

void aic78xx_poll( struct aic78xx_pb *pb, int poll )
    {
    char name[32];
    int i;

    /*
     *  Poll all sixteen nodes as a group, pause for a while and repeat forever.
     */
    for( i = 0; i < 16; i++ )
	{
	if( pb->pb.mode == DDB$K_STOP )
	    continue;
	if( i == pb->node_id )
	    continue;
	if( poll || scsi_poll )
	    scsi_send_inquiry( pb->pb.sb[i] );
	}
    pb->poll_active = 0;
    krn$_create_delayed( 30000, 0, aic78xx_poll, 0, 0, 5, 0, 4096,
	    prefix( "_poll" ), pb, 0 );
    }

void aic78xx_setmode( struct aic78xx_pb *pb, int mode )
    {
    switch( mode )
	{
	case DDB$K_STOP:
	    if( pb->pb.mode != DDB$K_STOP )
		{
		krn$_wait( &pb->owner_s );
		if( pb->pb.mode == DDB$K_INTERRUPT )
		    io_disable_interrupts( pb, pb->pb.vector );
		else
		    remq_lock( &pb->pb.pq.flink );
		aic78xx_stop_adapter( pb );
		pb->pb.mode = DDB$K_STOP;
		}
	    break;	    

	case DDB$K_START:
	    if( pb->pb.mode == DDB$K_STOP )
		{
		pb->pb.mode = pb->pb.desired_mode;
		if( pb->pb.desired_mode == DDB$K_INTERRUPT )
		    io_enable_interrupts( pb, pb->pb.vector );
		else
		    insq_lock( &pb->pb.pq.flink, &pollq );
		aic78xx_start_adapter( pb );
#if 0
		pb->poll_active = 1;
#endif
		krn$_post( &pb->owner_s );
		}
	    break;

	case DDB$K_READY:
	    if( pb->pb.mode != DDB$K_STOP )
		{
		while( pb->poll_active )
		    krn$_sleep( 100 );
		}
	    break;

#if 0
	case DDB$K_POLLED:
	    if( pb->pb.mode == DDB$K_STOP )
		{
		pb->pb.mode = DDB$K_POLLED;
		insq_lock( &pb->pb.pq.flink, &pollq );
		aic78xx_start_adapter( pb );
		krn$_post( &pb->owner_s );
		}
	    break;
#endif
	}
    }

#endif

void aic78xx_poll_for_interrupt( struct aic78xx_pb *pb )
    {
    ADAPTER *pAdapter = pb->adapter;

    if( pb->pb.mode == DDB$K_POLLED )
	if( pb->online )
	    if( HIMtPollIRQ( pAdapter->adapterTSH ) )
		(void)OSMIntHandler( pAdapter );
    }

void aic78xx_interrupt( struct aic78xx_pb *pb )
    {
    ADAPTER *pAdapter = pb->adapter;

    if( pb->online )
	(void)OSMIntHandler( pAdapter );
    if( pb->pb.mode == DDB$K_INTERRUPT )
	io_issue_eoi( pb, pb->pb.vector );
    }

void GetHIMFunctions( void )
    {
    HIM_ENTRYNAME_SCSI( &SCSIRoutines, (HIM_UINT16)sizeof( HIM_FUNC_PTRS ) );
    }

/*********************************************************************
*
*  AllocateAdapterMemory
*     Allocate HIM-requested memory for adapter.
*     Biggest trick is aligning blocks for the HIM,
*     and allocating locked vs. unlocked blocks properly.
*
*********************************************************************/
void AllocateAdapterMemory( struct aic78xx_pb *pb, ADAPTER *pAdapter, HIM_TASK_SET_HANDLE initTSH )
    {
    HIM_TASK_SET_HANDLE adapterTSH = pAdapter->adapterTSH;
    HIM_UINT16 index = 0;
    HIM_UEXACT8 *memory;
    HIM_UEXACT8 *aligned; /* we do math on these pointers, so 'void *' won't work */
    HIM_UINT8 category;
    HIM_UINT32 minimumBytes;
    HIM_UINT32 granularity;
    HIM_UEXACT32 alignmentMask;
    HIM_UINT16 size;

    while( ( size = (HIM_UINT16)HIMtCheckMemoryNeeded( initTSH,
	    adapterTSH, pAdapter->productID, index, &category,
	    &minimumBytes, &granularity, &alignmentMask ) ) != 0 )
	{
	/* Allocate, including extra memory so we can line things
	 * up properly.  (We assume we can always allocate the
	 * maximum.) */
	memory = AllocMemory( pb, size + alignmentMask );

	/* Assume OSM knows virtual pointers are 32 bits.  Use this
	 * to do the math to line things up. */
	aligned = (HIM_UEXACT8 *)
		( ( (HIM_UEXACT32)memory + alignmentMask ) & ~alignmentMask );
               
	HIMtSetMemoryPointer( adapterTSH, index, category,
		aligned, (HIM_UINT32)size );
	index++;

	/* Note: If OSM is concerned about freeing adapter memory 
	 * (i.e. driver can be unloaded without downing system)
	 * the **original** memory pointers need to be remembered
	 * so they can be freed when driver unloads.
	 */
	}
    }

/*********************************************************************
*
*  AllocateTargets
*     After successful HIM_PROTOCOL_AUTO_CONFIG, go through sequence
*     where HIM declares new targets, and validates continued 
*     existence of old targets.
*
*********************************************************************/
void AllocateTargets( struct aic78xx_pb *pb, ADAPTER *pAdapter )
    {
    HIM_TASK_SET_HANDLE adapterTSH = pAdapter->adapterTSH;
    TARGET *pTarget;
    HIM_UINT16 index = 0;
    HIM_UINT16 targetsize;
    HIM_UINT16 i;

    /* caller guarantees interrupts off */

    /* Validate old targets are still around. */
    for( i = 0; i < MAX_TARGETS; i++ )
	{
	if( pTarget = pAdapter->pAttachedTarget[i] )
	    {
	    if( HIMtValidateTargetTSH( pTarget->targetTSH ) == HIM_TARGET_INVALID )
		{
		/* We know nothing is active, by HIM_PROTOCOL_AUTO_CONFIG
		 * rules.  There may be waiting items, however.
		 * These should be dequeued and returned to the OS
		 * (not shown here) */
		/* No need to call 'HIMClearTargetTSH'.  That's for OSM
		 * deletion on his own volition. */
		FreeMemory( pTarget->pTscb );
		FreeMemory( pTarget );
		pAdapter->pAttachedTarget[i] = 0;
		}
	    }
	}

    /* get target TSCB size */
    targetsize = (HIM_UINT16)HIMtSizeTargetTSCB( adapterTSH );

    while( ( pAdapter->numberTargetTSH < MAX_TARGETS )
	    && HIMtCheckTargetTSCBNeeded( adapterTSH, index ) )
	{
	pTarget = AllocMemory( pb, sizeof( TARGET ) );

	/* Find new slot in adapter array.
	 * We know this works, because we've already checked
	 * that we're under num-target maximum. */
	for( i = 0; i < MAX_TARGETS; i++ )
	    if( pAdapter->pAttachedTarget[i] == 0 )
		break;

	pAdapter->pAttachedTarget[i] = pTarget;
	pTarget->pAdapter = pAdapter;

	pTarget->pTscb = AllocMemory( pb, targetsize );
	pTarget->targetTSH = 
		HIMtCreateTargetTSCB( adapterTSH, index, pTarget->pTscb );

	HIMtProfileTarget( pTarget->targetTSH, &pTarget->profile );

	/* OS-specific declaration of target might go here, 
	 * depending on your OS.  OS may want to know things
	 * like SCSI ID, etc., which OSM gets from profile. */
	}

    /* Calculate total number of attached targets */
    for( i = 0; i < MAX_TARGETS; i++ )
	if( pAdapter->pAttachedTarget[i] == 0 )
	    break;
    pAdapter->numberTargetTSH = i;
    }

/*********************************************************************
*
*  InitOsmIOBPool
*     Allocate, zero out, and link together a pool of IOB's.
*     Also allocates iobReserve region, immediately after
*     OSMIOB.  Code assumes that all adapters have same iobReserve 
*     requirements.
*
*********************************************************************/
void InitOsmIOBPool( struct aic78xx_pb *pb, ADAPTER *pAdapter )
    {
    HIM_UINT32 iobReserveSize = pAdapter->iobReserveSize;
    OSMIOB *pOsmIOB;
    int i;

    pOsmIOB = AllocMemory( pb, sizeof( OSMIOB ) + iobReserveSize );
    pOsmIOB->pPrev = 0;
    pOsmIOB->pNext = 0;
    pOsmIOB->iob.iobReserve.virtualAddress = (HIM_UEXACT8 *)pOsmIOB + sizeof( OSMIOB ); 
    pOsmIOB->iob.iobReserve.busAddress = VirtualToPhysical( pb, (HIM_UEXACT8 *)pOsmIOB + sizeof( OSMIOB ) ); 
   
    pOsmIOB->iob.iobReserve.bufferSize = iobReserveSize;
    pAdapter->pFreeHeadOsmIOB = pOsmIOB;

    for( i = 1; i < NUMBER_IOBS; i++ )
	{
	pOsmIOB = AllocMemory( pb, sizeof( OSMIOB ) + iobReserveSize );
	pOsmIOB->pPrev = 0;
	pOsmIOB->pNext = pAdapter->pFreeHeadOsmIOB;
	pAdapter->pFreeHeadOsmIOB->pPrev = pOsmIOB;
	pOsmIOB->iob.iobReserve.virtualAddress = (HIM_UEXACT8 *)pOsmIOB + sizeof( OSMIOB );
	pOsmIOB->iob.iobReserve.busAddress = VirtualToPhysical( pb, (HIM_UEXACT8 *)pOsmIOB + sizeof( OSMIOB ) );

	pOsmIOB->iob.iobReserve.bufferSize = iobReserveSize;
	pAdapter->pFreeHeadOsmIOB = pOsmIOB;
	}
    }

/*********************************************************************
*
*  EnqueueOsmIOB
*     Put IOB on waiting queue.
*     Typically called by SetupReqeust for new request, but can
*     also called by post routine attempting a retry.
*       
*********************************************************************/
void EnqueueOsmIOB(OSMIOB *pOsmIOB,
                   TARGET *pTarget)
{
    spinlock( &spl_kernel );	/* disable system interrupts */
   if (pTarget->HeadWaiting)
   {
      /* put latest on tail */
      pOsmIOB->pPrev = pTarget->TailWaiting;
      pTarget->TailWaiting->pNext = pOsmIOB;
      pTarget->TailWaiting = pOsmIOB;
   } else
   {
      pTarget->HeadWaiting = pOsmIOB;
      pTarget->TailWaiting = pOsmIOB;
   }
    spinunlock( &spl_kernel );	/* enable system interrupts */

   SendIOBsMaybe(pTarget);
}

/*********************************************************************
*
*  SendIOBsMaybe
*       If HIM can accept new requests, send it to HIM, put on
*          'active' (waiting for interrupt) queue.
*       Typically called by EnqueueOSMIob for new or retried request,
*          but any routine that forces IOB's to queue (protocol auto
*          config, for example) should also call this routine
*          directly for all targets which were stalled.
*       
*********************************************************************/
void SendIOBsMaybe(TARGET *pTarget)
{
   OSMIOB *pOsmIOB;

    spinlock( &spl_kernel );	/* disable system interrupts */
   while (pTarget->HeadWaiting &&
          (pTarget->pAdapter->frozen != HIM_TRUE) &&
          (pTarget->pAdapter->AutoConfigActive != HIM_TRUE))
   {
      /* dequeue off top of waiting queue */
      pOsmIOB = pTarget->HeadWaiting;

      pTarget->HeadWaiting = pOsmIOB->pNext;
      if (pTarget->HeadWaiting == 0)
      {
         pTarget->TailWaiting = 0;
      } else
      {
         pTarget->HeadWaiting->pPrev = 0;
      }

      /* keep track of total active at adapter, because sometimes
       * OSM needs to know when adapter is entirely idle */
      pTarget->pAdapter->NumActiveIOBs++;

      /* give request to the HIM */
      HIMtQueueIOB(&(pOsmIOB->iob));

      /* Keep track of all active commands.  This is needed
       * for finding right IOB for abort. */
      if (pTarget->HeadActive)
      {
         /* put latest on tail */
         pOsmIOB->pPrev = pTarget->TailActive;
         pTarget->TailActive->pNext = pOsmIOB;
         pTarget->TailActive = pOsmIOB;
      } else
      {
         pTarget->HeadActive = pOsmIOB;
         pTarget->TailActive = pOsmIOB;
         pOsmIOB->pNext = 0;
         pOsmIOB->pPrev = 0;
      }
   }
    spinunlock( &spl_kernel );	/* enable system interrupts */
}

/*********************************************************************
*
*  AllocOSMIOB
*     Acquire new IOB off head of free list.
*
*********************************************************************/
OSMIOB *AllocOSMIOB(ADAPTER *pAdapter)
{
   OSMIOB *pOsmIOB;
   if (pAdapter->pFreeHeadOsmIOB)
   {
      pOsmIOB = pAdapter->pFreeHeadOsmIOB;
      pAdapter->pFreeHeadOsmIOB = pAdapter->pFreeHeadOsmIOB->pNext;
      if (pAdapter->pFreeHeadOsmIOB)
      {
         pAdapter->pFreeHeadOsmIOB->pPrev = 0;
      }
      pOsmIOB->pNext = 0;
      return(pOsmIOB);
   } else
   {
      pprintf("AllocOSMIOB failed\n");
      /* maybe ask system for more memory, maybe return failure */
      return(0);
   }
}

/*********************************************************************
*
*  FreeOSMIOB
*     Acquire new IOB off head of free list.
*
*********************************************************************/
void FreeOSMIOB(ADAPTER *pAdapter,OSMIOB *pOsmIOB)
{
   pOsmIOB->pPrev = 0;
   pOsmIOB->pNext = pAdapter->pFreeHeadOsmIOB;
   pAdapter->pFreeHeadOsmIOB = pOsmIOB;
}

/*********************************************************************
*
*  OSMIntHandler
*     Assumes each adapter and interrupt line has been registered 
*     with OS.  OS sorts out which adapters match the currently-
*     active interrupt line.
*     Returns non-zero to acknowledge interrupt was for this adapter.
*
*********************************************************************/
HIM_UEXACT8 OSMIntHandler(ADAPTER *pAdapter)
{
   HIM_UEXACT8 retVal = 0;

   /* service IRQ */
   if (HIMtFrontEndISR(pAdapter->adapterTSH) != 0)
   {
      /* post routines (if any) will occur within HIMtBackEndISR */
      HIMtBackEndISR(pAdapter->adapterTSH);
      retVal = 1;
   }

   return(retVal);
}

/*********************************************************************
*
*  NormalPostRoutine
*     Post 'normal' IOB (not abort, auto config) back to OSM
*
*********************************************************************/
HIM_UINT32 NormalPostRoutine(HIM_IOB *pIob)
{
   OSMIOB *pOsmIOB = GET_OSMIOB(pIob);
   TARGET    *pTarget = pOsmIOB->chimObject.pTarget;
   IOREQUEST *pIORequest;
   HIM_UINT8 status;

   /* We know interrupts are off, because we're nested
    * within HIMBackEndISR. */

   /* keep track of total active at adapter, because sometimes
    * OSM needs to know when adapter is entirely idle */
   pTarget->pAdapter->NumActiveIOBs--;

   /* remove from linked queue */
   if (pOsmIOB->pNext)
      pOsmIOB->pNext->pPrev = pOsmIOB->pPrev;
   if (pOsmIOB->pPrev)
      pOsmIOB->pPrev->pNext = pOsmIOB->pNext;

   /* pOSMIOB's pointers are still good.  Move head and/or tail
    * if either (or both) equals pOsmIOB */
   if (pOsmIOB == pTarget->TailActive)
      pTarget->TailActive = pOsmIOB->pPrev;
   if (pOsmIOB == pTarget->HeadActive)
      pTarget->HeadActive = pOsmIOB->pNext;

   /* Interpret status */
   switch (pIob->taskStatus)
   {
      case HIM_IOB_GOOD:
         status = OS_REQUEST_GOOD;
         break;
      case HIM_IOB_ERRORDATA_VALID:
         if ((pIob->errorDataLength - pIob->residualError) < 14)
         {
            /* not enough sense data received to interpret */
            status = OS_REQUEST_FAILURE;
         } else if ((((HIM_UEXACT8 *)(pIob->errorData))[2] == 2) &&
                    (((HIM_UEXACT8 *)(pIob->errorData))[12] == 0x3a))
         {
            /* scsi sense data tells us medium is not present */
            status = OS_REQUEST_MEDIUM_NOT_PRESENT;
         } else
         {
            /* assume all other errors are just 'failures' */
            status = OS_REQUEST_FAILURE;
         }
         break;
      case HIM_IOB_PARITY_ERROR:
         if (pOsmIOB->retriesLeft > 0 )
         {
            pOsmIOB->retriesLeft--;
            status = ATTEMPT_RETRY;
         } else
         {
            status = OS_REQUEST_FAILURE;
         }
         break;
      default:
         status = OS_REQUEST_FAILURE;
         break;
   }
   
   if ((pIob->taskStatus != HIM_IOB_GOOD && pIob->flagsIob.freezeOnError) ||
         pIob->taskStatus == HIM_IOB_TASK_SET_FULL ||
         pIob->taskStatus == HIM_IOB_BUSY)
   {
      /* HIM's queues are frozen so make a call to HandleHIMFrozen */
      HandleHIMFrozen(pIob);
   }

   if (status == ATTEMPT_RETRY)
   {
      /* We can re-enter the HIM from a post routine.
       * Therefore, normal I/O dispatch code can be reused
       * for retries */
      EnqueueOsmIOB(pOsmIOB,pTarget);
   } else
   {
      pIORequest = (IOREQUEST *)(pIob->osRequestBlock);
      pIORequest->status = (HIM_UEXACT8) status;

      /* return OSM control block to master pool */
      FreeOSMIOB(pTarget->pAdapter,pOsmIOB);
   }

   return(0);
}

/*********************************************************************
*
*  HandleHIMFrozen
*     This function handles cases where the HIM's queues are frozen.
*     The primary purpose of this code is to queues an IOB of 
*     HIM_UNFREEZE_QUEUE.  An OSM may want do some checking based on 
*     maxTags at this point or schedule the QueueIOB call after some
*     delay, but we are skipping this and just calling QueueIOB for
*     simplicity.
*
*********************************************************************/
void HandleHIMFrozen(HIM_IOB *pIob)
{
   OSMIOB *pOsmIOB = GET_OSMIOB(pIob);
   OSMIOB    *pOsmIOBSpecial;
   TARGET    *pTarget = pOsmIOB->chimObject.pTarget;
   ADAPTER   *pAdapter = pTarget->pAdapter;

   /* Get an pOsmIOB for unfreeze_queue */
   pOsmIOBSpecial = AllocOSMIOB(pAdapter);
   pOsmIOBSpecial->iob.function = HIM_UNFREEZE_QUEUE;
   pOsmIOBSpecial->iob.taskSetHandle = pIob->taskSetHandle;
   pOsmIOBSpecial->iob.postRoutine = PostUnfreezeHIMQueue;

   EnqueueOsmIOB(pOsmIOBSpecial,pTarget);
}

/*********************************************************************
*
*  PostUnfreezeHIMQueue
*     Post HIM_UNFREEZE_QUEUE IOB back to OSM.  This is the result of
*     the IOB that was sent in the function HandleHIMFrozen.
*
*********************************************************************/
HIM_UINT32 PostUnfreezeHIMQueue(HIM_IOB *pIob)
{
   OSMIOB *pOsmIOB = GET_OSMIOB(pIob);
   TARGET    *pTarget = pOsmIOB->chimObject.pTarget;

   /* We know interrupts are off, because we're nested
    * within HIMBackEndISR. */

   /* keep track of total active at adapter, because sometimes
    * OSM needs to know when adapter is entirely idle */
   pTarget->pAdapter->NumActiveIOBs--;

   /* remove from linked queue */
   if (pOsmIOB->pNext)
      pOsmIOB->pNext->pPrev = pOsmIOB->pPrev;
   if (pOsmIOB->pPrev)
      pOsmIOB->pPrev->pNext = pOsmIOB->pNext;

   /* pOSMIOB's pointers are still good.  Move head and/or tail
    * if either (or both) equals pOsmIOB */
   if (pOsmIOB == pTarget->TailActive)
      pTarget->TailActive = pOsmIOB->pPrev;
   if (pOsmIOB == pTarget->HeadActive)
      pTarget->HeadActive = pOsmIOB->pNext;

   /* Interpret status */
   if (pIob->taskStatus == HIM_IOB_GOOD)
   {
      /* return OSM control block to master pool */
      FreeOSMIOB(pTarget->pAdapter,pOsmIOB);
   }
   else
   {
      /* If HIM_UNFREEZE_QUEUE IOB fails were in big trouble. */
      /* If it fails we will do catastrophic stuff like free */
      /* memory and unload driver, etc. */
   }
      
   return(0);
}

/*********************************************************************
*
*  PostProtocolAutoConfig
*     Post process for protocol auto config
*     Separate code is polling on taskStatus, and auto config
*     isn't queued like normal operations, so this routine
*     needn't do much.
*
*********************************************************************/
HIM_UINT32 PostProtocolAutoConfig(HIM_IOB *pIob)
{
   OSMIOB *pOsmIOB = GET_OSMIOB(pIob);
   ADAPTER *pAdapter = pOsmIOB->chimObject.pAdapter;

   pAdapter->NumActiveIOBs--;
   FreeOSMIOB(pAdapter,pOsmIOB);
   return(0);
}

/*********************************************************************
*
*  PostRoutineEventResetHW
*     Post process for Reset HW that is generated by an 
*     OSMEvent.
*
*********************************************************************/
HIM_UINT32 PostRoutineEventResetHW (HIM_IOB *pIob)
{
   OSMIOB *pOsmIOB = GET_OSMIOB(pIob);
   OSMIOB *pOsmIOBSpecial;
   ADAPTER   *pAdapter = pOsmIOB->chimObject.pAdapter;

   /* setup IOB, call HIMQueueIOB, wait for taskStatus,
    * as in original scan case */

   if (pOsmIOB->iob.taskStatus == HIM_IOB_GOOD) {
      pAdapter->NumActiveIOBs--;
      FreeOSMIOB(pAdapter,pOsmIOB);

      /* Get an pOsmIOB for unfreeze_queue */
      pOsmIOBSpecial = AllocOSMIOB(pAdapter);
      pOsmIOBSpecial->iob.function = HIM_PROTOCOL_AUTO_CONFIG;
      pOsmIOBSpecial->iob.taskSetHandle = pIob->taskSetHandle;
      pOsmIOBSpecial->iob.postRoutine = PostRoutineEventPAC;

      /* stop other commands from starting */
      pAdapter->AutoConfigActive = HIM_TRUE;

      pAdapter->NumActiveIOBs++;
      /* HIM should have already aborted all IO's or can guarantee */
      /* that it can accept a PAC is so it should be safe to */
      /* call QueueIOB for the PAC here. Also no protection is */
      /* necessary here since postRoutine is typically called in */
      /* interrupt context and is already protected. */
      HIMtQueueIOB(&(pOsmIOBSpecial->iob));

   } else {
      /* If HIM_RESET_HARDWARE IOB fails were in big trouble. */
      /* If it fails we will do catastrophic stuff like free */
      /* memory and unload driver, etc. */
   }

   return(0);
}

/*********************************************************************
*
*  PostRoutineEventPAC
*     Post process for protocol auto config that is generated by an 
*     OSMEvent.
*
*********************************************************************/
HIM_UINT32 PostRoutineEventPAC (HIM_IOB *pIob)
{
   OSMIOB *pOsmIOB = GET_OSMIOB(pIob);
   TARGET    *pTarget;
   ADAPTER   *pAdapter = pOsmIOB->chimObject.pAdapter;
   HIM_UINT8 k;

   /* setup IOB, call HIMQueueIOB, wait for taskStatus,
    * as in original scan case */

   if (pOsmIOB->iob.taskStatus == HIM_IOB_GOOD) {
      pAdapter->NumActiveIOBs--;
      FreeOSMIOB(pAdapter,pOsmIOB);

      for (k=0 ; k<MAX_TARGETS ; k++ ){
         if (pTarget = pAdapter->pAttachedTarget[k]){
            if (HIMtValidateTargetTSH(pTarget->targetTSH) == 
                HIM_TARGET_INVALID) {
               /* Dequeue any IOB waiting to go to this non-existent
                * target (not shown) */
               free(pTarget->pTscb);
               free(pTarget);
               pAdapter->pAttachedTarget[k] = 0;
            }
         }
      }
      /* Some OSM's may look for new targets here.  We wont
      * purposes of this sample code.  If we did look for new
      * targets here we would need to get size of target tscb,
      * while loop for new targets, as in original scan */

      /* Clear autoconfig flag and re-start target queues */
      pAdapter->AutoConfigActive = HIM_FALSE;
   }

   return(0);
}

/*********************************************************************
*
*  OSMEvent
*     Async event notificatioon
*
*********************************************************************/
void OSMEvent(void *osmAdapterContext,
               HIM_UINT16 event, void *pEventContext,...)
{
   HIM_UINT8 k;
   ADAPTER *pAdapter = (ADAPTER *) osmAdapterContext;
   OSMIOB    *pOsmIOBSpecial;

   switch (event)
   {
      case HIM_EVENT_OSMFREEZE:
         pAdapter->frozen = HIM_TRUE;
         break;
      case HIM_EVENT_OSMUNFREEZE:
         pAdapter->frozen = HIM_FALSE;
         for (k=0 ; k<pAdapter->numberTargetTSH ; k++ ) 
         {
            SendIOBsMaybe(pAdapter->pAttachedTarget[k]);
         }
         break;
      case HIM_EVENT_IO_CHANNEL_RESET:
      case HIM_EVENT_AUTO_CONFIG_REQUIRED:
         /* Get an pOsmIOB for pac */
         pOsmIOBSpecial = AllocOSMIOB(pAdapter);
         pOsmIOBSpecial->iob.function = HIM_PROTOCOL_AUTO_CONFIG;
         pOsmIOBSpecial->iob.taskSetHandle = pAdapter->adapterTSH;
         pOsmIOBSpecial->chimObject.pAdapter = pAdapter;
         pOsmIOBSpecial->iob.postRoutine = PostRoutineEventPAC;

         /* stop other commands from starting */
         pAdapter->AutoConfigActive = HIM_TRUE;

         pAdapter->NumActiveIOBs++;
         /* HIM should have already aborted all IO's or can guarantee */
         /* that it can accept a PAC is so it should be safe to */
         /* call QueueIOB for the PAC here. Also no protection is */
         /* necessary here since OSMEvent is typically called in */
         /* interrupt context and is already protected. */
         HIMtQueueIOB(&(pOsmIOBSpecial->iob));
         break;
      case HIM_EVENT_HA_FAILED:
         /* Get an pOsmIOB for reset HW  */
         pOsmIOBSpecial = AllocOSMIOB(pAdapter);
         pOsmIOBSpecial->iob.function = HIM_RESET_HARDWARE;
         pOsmIOBSpecial->iob.taskSetHandle = pAdapter->adapterTSH;
         pOsmIOBSpecial->chimObject.pAdapter = pAdapter;
         /* OSM will queue the PAC in the PostRoutineEventResetHW */
         pOsmIOBSpecial->iob.postRoutine = PostRoutineEventResetHW;

         pAdapter->NumActiveIOBs++;
         HIMtQueueIOB(&(pOsmIOBSpecial->iob));
         break;
   }

}

/*********************************************************************
*
*  OSMMapIOHandle
*     Map io registers 
*
*  Return Value:  0 - handle successfully returned
*                 1 - Unable to map this I/O range
*                  
*********************************************************************/
/* Adaptec HIM's don't use these features yet:
 *    Pacing
 *    Cachable attributes (everything is "HIM_STRICTORDER").
 *    Dynamic ranges (releasing ranges on the fly).
 *    Multiple ranges (?)
 */
HIM_UINT8 OSMMapIOHandle( void *posmAdapterContext,
	HIM_UINT8 rangeIndex,
	HIM_UINT32 offset,
	HIM_UINT32 length,
	HIM_UINT32 pacing,
	HIM_UINT16 attributes,
	HIM_IO_HANDLE *handle )
    {
    ADAPTER *pAdapter = (ADAPTER *)posmAdapterContext;
    struct aic78xx_pb *pb = pAdapter->pb;
    HIM_UINT32 rangeBase;
    HIM_IO_HANDLE newHandle;

    /* Allocate new handle structure, link to head of adapter list */
    newHandle = AllocMemory( pb, sizeof( *newHandle ) );
    newHandle->pb = pb;

    /* Get base address from pci config space.  We've previously
     * recorded the PCI coordinates (bus/device/function) 
     * For 32-bit PCI, this routine looks for configuration space 
     * registers 10x-13x for rangeIndex = 0, etc. */
    rangeBase = incfgl( pb, 0x10 + rangeIndex * 4 );

    /* Transfer io/memory map bit from PCI config space to handle,
     * and clear out 'type' bits to get the true base address */
    if( rangeBase & 1 )
	{
	newHandle->memoryMapped = HIM_FALSE;
	rangeBase &= ~1;
	}
    else
	{
	newHandle->memoryMapped = HIM_TRUE;
	rangeBase &= ~15;
	}

    /* Register this range with operating system 
     * The OS may re-map the memory space with a different virtual
     * address.  We would expect the IOMap call would return 
     * rangeBase + offset */
    newHandle->baseAddress = rangeBase + offset;

    /* Return the pointer to the structure as the new handle */
    *handle = newHandle;
    return( 0 );
    }

/*********************************************************************
*
*  OSMReleaseIOHandle
*     Release io handle specified
*
*  Return Value:  0 - handle successfully returned
*                 others for future expansion
*                  
*********************************************************************/
HIM_UINT8 OSMReleaseIOHandle(void *osmAdapterContext, 
                             HIM_IO_HANDLE handle)
{
   ADAPTER *pAdapter = (ADAPTER *) osmAdapterContext;

   /* make compiler warnings disappear */
   pAdapter = pAdapter; 
   handle = handle;

   /* Dequeue handle from the adapter's
    * list, and free memory. (Not shown.)*/

   return(0 /* success */);
}

/*********************************************************************
*
*  OSMGetBusAddress
*     For PCI (and most other buses), virtual->physical translation. 
*     (On some buses, the address transmitted on the bus isn't a
*     physical, but it is for PCI.)
*
*********************************************************************/

HIM_BUS_ADDRESS OSMGetBusAddress(
	void *osmAdapterContext,
	HIM_UINT8 category,
	void *virtualAddress )
    {
    ADAPTER *pAdapter = (ADAPTER *)osmAdapterContext;
    struct aic78xx_pb *pb = pAdapter->pb;

    return( VirtualToPhysical( pb, virtualAddress ) );
    }

/*********************************************************************
*
*  OSMAdjustBusAddress
*     Add a positive or negative value to a bus address.
*     This is an interface function only because HIM doesn't
*     know the implementation of HIM_BUS_ADDRESS, which might
*     be 32 or 64 bits.  In this example, the address is 32 bits,
*     and so the function is trivial.
*
*********************************************************************/

void OSMAdjustBusAddress(HIM_BUS_ADDRESS *busAddress,
                         int value)                
{
   (HIM_UEXACT32) (*busAddress) += value;
}

/*********************************************************************
*
*  OSMGetNVSize, et al
*     These are used then non-volatile RAM is supported on a
*     motherboard, and access is therefore platform-specific.
*     This is not yet supported.  
*
*********************************************************************/
HIM_UINT32 OSMGetNVSize(void *osmAdapterContext)
{
   return(0);
}
HIM_UINT8 OSMPutNVData(void *osmAdapterContext,
                        HIM_UINT32 destinationOffset, 
                        void *source, 
                        HIM_UINT32 length)
{
   /* return failure */
   return(1);
}
HIM_UINT8 OSMGetNVData(void *osmAdapterContext,
                        void *destination, 
                        HIM_UINT32 sourceOffset, 
                        HIM_UINT32 length)
{
   /* return failure */
   return(1);
}

/*********************************************************************
*
*  OSMReadUExact8
*     Read UEXACT8 value from hardware
*
*********************************************************************/
HIM_UEXACT8 OSMReadUExact8(
	HIM_IO_HANDLE ioBase, 
	HIM_UINT32 ioOffset )
    {
    struct pb *pb = ioBase->pb;

    if( ioBase->memoryMapped )
	return( (HIM_UEXACT8)inmemb( pb, ioBase->baseAddress + ioOffset ) );
    else
	return( (HIM_UEXACT8)inportb( pb, ioBase->baseAddress + ioOffset ) );
    }

/*********************************************************************
*
*  OSMReadUExact16
*     Read UEXACT16 value from hardware
*
*********************************************************************/
HIM_UEXACT16 OSMReadUExact16(
	HIM_IO_HANDLE ioBase,
	HIM_UINT32 ioOffset )
    {
    struct pb *pb = ioBase->pb;

    if( ioBase->memoryMapped )
	return( (HIM_UEXACT16)inmemw( pb, ioBase->baseAddress + ioOffset ) );
    else
	return( (HIM_UEXACT16)inportw( pb, ioBase->baseAddress + ioOffset ) );
    }

/*********************************************************************
*
*  OSMReadUExact32
*     Read UEXACT32 value from hardware
*
*********************************************************************/
HIM_UEXACT32 OSMReadUExact32(
	HIM_IO_HANDLE ioBase, 
	HIM_UINT32 ioOffset )
    {
    struct pb *pb = ioBase->pb;

    if( ioBase->memoryMapped )
	return( (HIM_UEXACT8)inmeml( pb, ioBase->baseAddress + ioOffset ) );
    else
	return( (HIM_UEXACT8)inportl( pb, ioBase->baseAddress + ioOffset ) );
    }

/*********************************************************************
*
*  OSMReadStringUExact8
*     Read UEXACT8 value string from hardware
*
*********************************************************************/
void OSMReadStringUExact8(
	HIM_IO_HANDLE ioBase,
	HIM_UINT32 ioOffset,
	HIM_UEXACT8 *destBuffer,
	HIM_UINT32 count,
	HIM_UINT8 stride )
    {
    /* nothing has to be done for this implementation */
    }

/*********************************************************************
*
*  OSMReadStringUExact16
*     Read UEXACT16 value string from hardware
*
*********************************************************************/
void OSMReadStringUExact16(
	HIM_IO_HANDLE ioBase,
	HIM_UINT32 ioOffset,
	HIM_UEXACT16 *destBuffer,
	HIM_UINT32 count,
	HIM_UINT8 stride )
    {
    /* nothing has to be done for this implementation */
    }

/*********************************************************************
*
*  OSMReadStringUExact32
*     Read UEXACT32 value string from hardware
*
*********************************************************************/
void OSMReadStringUExact32(
	HIM_IO_HANDLE ioBase,
	HIM_UINT32 ioOffset,
	HIM_UEXACT32 *destBuffer,
	HIM_UINT32 count,
	HIM_UINT8 stride)
    {
    /* nothing has to be done for this implementation */
    }

/*********************************************************************
*
*  OSMWriteUExact8
*     Write UEXACT8 value to harwdare
*
*********************************************************************/
void OSMWriteUExact8(
	HIM_IO_HANDLE ioBase,
	HIM_UINT32 ioOffset,
	HIM_UEXACT8 value )
    {
    struct pb *pb = ioBase->pb;

    if( ioBase->memoryMapped )
	outmemb( pb, ioBase->baseAddress + ioOffset, value );
    else
	outportb( pb, ioBase->baseAddress + ioOffset, value );
    }

/*********************************************************************
*
*  OSMWriteUExact16
*     Write UEXACT16 value to harwdare
*
*********************************************************************/
void OSMWriteUExact16(
	HIM_IO_HANDLE ioBase,
	HIM_UINT32 ioOffset,
	HIM_UEXACT16 value )
    {
    struct pb *pb = ioBase->pb;

    if( ioBase->memoryMapped )
	outmemw( pb, ioBase->baseAddress + ioOffset, value );
    else
	outportw( pb, ioBase->baseAddress + ioOffset, value );
    }

/*********************************************************************
*
*  OSMWriteUExact32
*     Write UEXACT32 value to harwdare
*
*********************************************************************/
void OSMWriteUExact32(
	HIM_IO_HANDLE ioBase,
	HIM_UINT32 ioOffset,
	HIM_UEXACT32 value )
    {
    struct pb *pb = ioBase->pb;

    if( ioBase->memoryMapped )
	outmeml( pb, ioBase->baseAddress + ioOffset, value );
    else
	outportl( pb, ioBase->baseAddress + ioOffset, value );
    }

/*********************************************************************
*
*  OSMWriteStringUExact8
*     Write UEXACT8 value string to harwdare
*
*********************************************************************/
void OSMWriteStringUExact8(
	HIM_IO_HANDLE ioBase,
	HIM_UINT32 ioOffset,
	HIM_UEXACT8 *sourceBuffer,
	HIM_UINT32 count,
	HIM_UINT8 stride )
    {
    /* nothing has to be done for this implementation */
    }

/*********************************************************************
*
*  OSMWriteStringUExact16
*     Write UEXACT16 value string to harwdare
*
*********************************************************************/
void OSMWriteStringUExact16(
	HIM_IO_HANDLE ioBase,
	HIM_UINT32 ioOffset,
	HIM_UEXACT16 *sourceBuffer,
	HIM_UINT32 count,
	HIM_UINT8 stride )
    {
    /* nothing has to be done for this implementation */
    }

/*********************************************************************
*
*  OSMWriteStringUExact32
*     Write UEXACT32 value string to harwdare
*
*********************************************************************/
void OSMWriteStringUExact32(
	HIM_IO_HANDLE ioBase,
	HIM_UINT32 ioOffset,
	HIM_UEXACT32 *sourceBuffer,
	HIM_UINT32 count,
	HIM_UINT8 stride )
    {
    /* nothing has to be done for this implementation */
    }

/*********************************************************************
*
*  OSMSynchronizeRange
*     Synchronize io range
*
*********************************************************************/
void OSMSynchronizeRange(
	HIM_IO_HANDLE ioBase,
	HIM_UINT32 ioOffset,
	HIM_UINT32 length )
    {
    mb( );
    }

/*********************************************************************
*
*  OSMWatchdog
*     Hook up with watch dog timer
*
*********************************************************************/
void OSMWatchdog(void *osmAdapterContext,
                  HIM_WATCHDOG_FUNC watchdogProcedure,
                  HIM_UINT32 microSeconds)
{
   /* Allocate OS-specific timer on behalf of the HIM.
    * This isn't demanded by CHIM implementations yet. */
}

/*********************************************************************
*
*  OSMSaveInterrupt
*     Retrieve interrupt enable/disable state in order to save it
*
*********************************************************************/
HIM_UINT8 OSMSaveInterruptState( )
    {
    /* HIM doesn't use this.  Its purpose is only for 
     * timing-critical hardware operations which can't be interrupted */
    return( 0 );
    }

/*********************************************************************
*
*  OSMSetInterruptState
*     Set interrupt to enable/disable state
*
*********************************************************************/
void OSMSetInterruptState( HIM_UINT8 interruptState )
    {
    /* HIM doesn't use this.  Its purpose is only for 
     * timing-critical hardware operations which can't be interrupted */
    }

/*********************************************************************
*
*  OSMReadPCIConfigurationDword
*     Read UEXACT32 value from PCI configuration space
*
*********************************************************************/
HIM_UEXACT32 OSMReadPCIConfigurationDword(
	void *osmAdapterContext,
	HIM_UINT8 registerNumber )
    {
    ADAPTER *pAdapter = (ADAPTER *)osmAdapterContext;
    struct pb *pb = pAdapter->pb;

    return( incfgl( pb, registerNumber ) );
    }

/*********************************************************************
*
*  OSMReadPCIConfigurationWord
*     Read UEXACT16 value from PCI configuration space
*
*********************************************************************/
HIM_UEXACT16 OSMReadPCIConfigurationWord(
	void *osmAdapterContext,
	HIM_UINT8 registerNumber )
    {
    ADAPTER *pAdapter = (ADAPTER *)osmAdapterContext;
    struct pb *pb = pAdapter->pb;

    return( incfgw( pb, registerNumber ) );
    }

/*********************************************************************
*
*  OSMReadPCIConfigurationByte
*     Read UEXACT8 value from PCI configuration space
*
*********************************************************************/
HIM_UEXACT8 OSMReadPCIConfigurationByte(
	void *osmAdapterContext,
	HIM_UINT8 registerNumber )
    {
    ADAPTER *pAdapter = (ADAPTER *)osmAdapterContext;
    struct pb *pb = pAdapter->pb;

    return( incfgb( pb, registerNumber ) );
    }

/*********************************************************************
*
*  OSMWritePCIConfigurationDword
*     Write UEXACT32 value to PCI configuration space
*
*********************************************************************/
void OSMWritePCIConfigurationDword(
	void *osmAdapterContext,
	HIM_UINT8 registerNumber,
	HIM_UEXACT32 registerValue )
    {
    ADAPTER *pAdapter = (ADAPTER *)osmAdapterContext;
    struct pb *pb = pAdapter->pb;

    outcfgl( pb, registerNumber, registerValue );
    }

/*********************************************************************
*
*  OSMWritePCIConfigurationWord
*     Write UEXACT16 value to PCI configuration space
*
*********************************************************************/
void OSMWritePCIConfigurationWord(
	void *osmAdapterContext,
	HIM_UINT8 registerNumber,
	HIM_UEXACT16 registerValue )
    {
    ADAPTER *pAdapter = (ADAPTER *)osmAdapterContext;
    struct pb *pb = pAdapter->pb;

    outcfgw( pb, registerNumber, registerValue );
    }

/*********************************************************************
*
*  OSMWritePCIConfigurationByte
*     Write UEXACT8 value to PCI configuration space
*
*********************************************************************/
void OSMWritePCIConfigurationByte(
	void *osmAdapterContext,
	HIM_UINT8 registerNumber,
	HIM_UEXACT8 registerValue )
    {
    ADAPTER *pAdapter = (ADAPTER *)osmAdapterContext;
    struct pb *pb = pAdapter->pb;

    outcfgb( pb, registerNumber, registerValue );
    }

/*********************************************************************
*
*  OSMDelay
*     Delay a number of microseconds
*
*********************************************************************/
void OSMDelay(
	void *osmAdapterContext,
	HIM_UINT32 microSeconds )
    {
    krn$_micro_delay( microSeconds );
    }

/*********************************************************************
*
*  AllocMemory
*     Allocate Memory.
*
*********************************************************************/
void *AllocMemory( struct aic78xx_pb *pb, HIM_UINT32 size )
    {
    struct QUEUE *owned_p;
    void *p;

    owned_p = (struct QUEUE *)malloc_noown( sizeof( struct QUEUE ) + (int)size );
    insq_lock( owned_p, pb->malloc_queue.blink );
    owned_p += 1;
    p = (void *)owned_p;
    return( p );
    }

/*********************************************************************
*
*  FreeAllemory
*     Free allocated Memory.
*
*********************************************************************/
void FreeMemory( void *p )
    {
    struct QUEUE *owned_p;

    owned_p = (struct QUEUE *)p;
    owned_p -= 1;
    remq_lock( owned_p );
    free( owned_p );
    }

/*********************************************************************
*
*  FreeAllemory
*     Free all allocated Memory.
*
*********************************************************************/
void FreeAllMemory( struct aic78xx_pb *pb )
    {
    struct QUEUE *owned_p;

    while( pb->malloc_queue.flink != &pb->malloc_queue )
	{
	owned_p = pb->malloc_queue.flink;
	remq_lock( owned_p );
	free( owned_p );
	}
    }

HIM_BUS_ADDRESS VirtualToPhysical( struct aic78xx_pb *pb, void *va )
    {
    return( (int)va + io_get_window_base( pb ) );
    }

protoendif(__AIC78XX__)
