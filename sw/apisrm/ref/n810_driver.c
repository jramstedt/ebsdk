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
 * Abstract:	SCSI/DSSI bus poller
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	noh	20-Nov-1995	BURNS: Disabled Burst mode.
 *
 *	raa	26-jun-1995	Set correct scint13 value on Medulla/M224
 *
 *	rbb	13-Mar-1995	Conditionalize for EB164
 *
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	er	14-Sep-1994	Conditionalize for SD164
 *
 *	er	30-Jun-1994	Conditionalize for EB64+
 *
 *	dtr	20-Jun-1994	Removed APS conditionals for initializing
 *				device.  find_pb has been implemented and is now
 *				a common routine support by all platforms.
 *
 *	dtr	24-Mar-1994	Converted conditionals for avanti, mustang,
 *				alcor, and mikasa into a single conditional.
 *				The code isn't as messy as it was before.
 *				
 *      dpm	22-Feb-1994	Added a call to insert pb so the controller  
 *				shows up when a show device is done  
 *
 *      dpm	18-Feb-1994	Changed n810_prefix so that it uses the 
 *				string "pk" instead of the pb.protocol var.  
 *
 *	D. Rice	1-Feb-1994	Added new configuration determination code for
 *				the pci.  It uses the routines and functions
 *				derivived from the tulip driver.
 *
 *	D. Rice 20-Jan-1994	Change the interrupt vector for mustang to be a
 *				symbol because there is a difference in pass 0
 *				and pass 1.  Once we go to pass 1 on the riser
 *				the 8b value is gone.
 *      wcc	11-Jan-1994	Adjust I/O calls to be more common comforming.
 *
 *      dpm	11-Jan-1994	Added burst support for MUSTANG by setting
 *				the Latency register to 128, and the DMODE 
 *				register to 0xc0
 *
 *	mdr	07-Jan-1994	Added LCA support for Medulla.  Added Medulla
 *				hardware build switches.  Fixed spelling of
 *				Medulla somebody put in.
 *
 *      dpm	6-Jan-1994	Reading the PCI registers instead of writing 
 *                              them, for MUSTANG. They are now set in 
 *				MUSTANG.C
 *
 *
 *	jmp	11-29-93	Add avanti support. The equivalent hae register 
 *		on apecs chip set is the host address extension register (1). There
 *		are three registers actually, where harx0 maps to the first 16 Mbyte,
 *		harx1 the next 112 Mbyte and harx2 for PCI I/O space. The harx1 
 *		register lives at 1.A000.01A0 on apecs set.
 *
 *
 *	dtr - 12-Nov-1993 - Corrected problems created in last edit.  I set the
 *	conditional for mustang_memory wrong.  Also added a second definition to
 *	the mustang code for the pci memory base.  I wasn't sure how the code
 *	worked for sabel so I left it the way it was in that the mustang code
 *	assumes the hae is at 81000000 and the sable can access eisa memory iwht
 *	full 32 bits without the hae.
 *	
 *	dtr 1-Nov-1993 Removed conditionals for Medulla, and Morgan.  The driver
 *	now is generic enough to use without the conditionals in there.  Medulla
 *	and Mustang have been setup to be the same.  Only low level routines in
 *	the io modules need to cahnge to make this driver work.  This will be
 *	the first attemp, other than  medulla to use the pb for I-O.
 *
 *	joes 15-Oct-1993 added support for new bus functions, port block malloc
 *			 moved to n810_init_sub, n810_init_port now takes *pb 
 *			 as parameter.
 *
 *	joes 12-jul-1993 added support for Medulla
 *
 *	sfs	14-Apr-1993	Initial entry.
 *
 *	D.Marsh 29-jul-1993	Took out the pass 1 bugfixes for MORGAN
 *
 *	D. Rice 24-Mar-1994	Combined all of the conditionals for mustang,
 *				avanti, and mikasa into a single conditional.
 *
 *
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:platform_cpu.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:ctype.h"
#include "cp$src:n810_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:n810_pb_def.h"
#include "cp$inc:platform_io.h"
#include "cp$inc:kernel_entry.h"
#if MEDULLA
#include "cp$src:medulla_system.h"
#endif
#include "cp$inc:prototypes.h"

#define rb(x) n810_read_byte_csr(pb,offsetof(struct n810_csr,x))
#define rl(x) n810_read_long_csr(pb,offsetof(struct n810_csr,x))
#define wb(x,y) n810_write_byte_csr(pb,offsetof(struct n810_csr,x),y)
#define wl(x,y) n810_write_long_csr(pb,offsetof(struct n810_csr,x),y)
#define prefix(x) set_io_prefix(pb,name,x)
#define debug_fprintf if(0)fprintf
#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define realloc(x,y) dyn$_realloc(x,y,DYN$K_SYNC|DYN$K_NOOWN)
#define free(x) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)
#define reserved_scsi_controller stp
#define reserved_mscp_controller sxfer

#if MODULAR
#include "cp$src:mem_def.h"
#define N810_CSR_BASE 1
#endif

#if STARTSHUT || DRIVERSHUT
extern int boot_retry_counter;
extern int diagnostic_mode_flag;
#endif

extern int ev_initing;

#define d_(x) n810_dien_/**/x
#define s0_(x) n810_sien0_/**/x
#define s1_(x) n810_sien1_/**/x

#define d_mask (d_(mdpe)|d_(bf)|d_(sir)|d_(iid))
#define s0_mask (s0_(ma)|s0_(sel)|s0_(sge)|s0_(udc)|s0_(rst)|s0_(par))
#define s1_mask (s1_(sto))

#define n810_phys(x) ((int)(x)+io_get_window_base(pb))
#define n810_virt(x) (pb->scripts+(int)(x)-(int)&n810_scripts)

unsigned char n810_read_byte_csr( struct n810_pb *pb, int offset );
unsigned char n810_write_byte_csr( struct n810_pb *pb, int offset, unsigned char value );
unsigned long n810_read_long_csr( struct n810_pb *pb, int offset );
unsigned long n810_write_long_csr( struct n810_pb *pb, int offset, unsigned long value );

extern null_procedure( );
#if !MODULAR
extern ev_sev( );
extern ev_sev_init( );

n810_ev_sev( );

struct env_table n810_host_id_ev = {
	0, 7,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0, n810_ev_sev, 0, ev_sev_init
};

struct env_table n810_fast_ev = {
	0, 1,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init
};

struct env_table n810_disconnect_ev = {
	0, 1,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init
};
#else
struct env_table n810_host_id_ev = {
	0, 7,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0, 0, 0, 0
};

struct env_table n810_fast_ev = {
	0, 1,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0, 0, 0, 0
};

struct env_table n810_disconnect_ev = {
	0, 1,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0, 0, 0, 0
};
#endif

int n810_ev_sev( char *name, struct EVNODE *ev )
    {
    char c;
    unsigned int id;
    struct n810_pb *pb;

    if( ev_initing )
	return( msg_success );
    pb = ev->misc;
    c = name[5];
    if( c == 'h' )
	{
	id = ev->value.integer;
	if( id > 7 )
	    {
	    ev->value.integer = 7;
	    err_printf( msg_ev_badvalue, name );
	    return( msg_failure );
	    }
	}
    return( ev_sev( name, ev ) );
    }

#if STARTSHUT

/*+
 * ============================================================================
 * = pke_setmode - change driver state				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	This routine changes the state of the port driver.  The following
 * 	states are relevant:
 *
 *	    DDB$K_STOP -
 *			Suspends the  port driver.  pke_poll stops checking
 *			things in this state, and interrupts from the
 *			device are dismissed.
 *
 *	    DDB$K_RESTARTABLE -
 *			Same as DDB$K_STOP, except the port driver may be
 *			restarted dynamically by the class driver.
 *
 *	    DDB$K_START -
 *			Changes the   port   driver   to   interrupt  mode.
 *			pke_poll is once again enabled, as well as device
 *			interrupt handling.
 *
 *	    DDB$K_INTERRUPT -
 *			Same as DDB$K_START.
 *  
 *	    DDB$K_ASSIGN  -	
 *			Perform system startup functions.
 *
 * FORM OF CALL:
 *  
 *	pke_setmode(mode, dev)
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
 *	struct device *dev - pointer to device information
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int pke_setmode(int mode, struct device *dev)
{
    int i, j, k, t;
    struct n810_pb *pb;
    struct scsi_sb *sb;

    switch (mode) {
	case DDB$K_ASSIGN: 
	    pke_setmode( DDB$K_START, dev );
	    pke_setmode( DDB$K_STOP, dev );
	    break;

	case DDB$K_START: 

	case DDB$K_INTERRUPT: 
	    pb = (struct n810_pb *) dev->devdep.io_device.devspecific;

	    if (pb) {
		pb->pb.ref++;
		return msg_success;
	    }

	    if (dev->flags)
		return msg_failure;

/* initialize the class driver */

	    scsi_shared_adr = ovly_load("SCSI");
	    if (!scsi_shared_adr)
		return msg_failure;

	    krn$_set_console_mode(0);

	    if (n810_init_adapter(dev) != msg_success)
		return msg_failure;
	    break;	    

	case DDB$K_STOP: 
	    pb = (struct n810_pb *) dev->devdep.io_device.devspecific;

	    if (!pb)
		return msg_failure;

	    if (pb->pb.ref == 0)
		return msg_success;

	    pb->pb.ref--;
	    if (pb->pb.ref != 0)
		return msg_success;


/* Break all virtual connections */

	    for( i = 0; i < pb->pb.num_sb; i++ ) {
		sb = pb->pb.sb[i];
		if (sb->state == sb_k_open) {
		    scsi_break_vc(sb);
		}
                krn$_semrelease(&sb->ready_s);
	    }


/* 
 * If the chip is not reset when shutdown, then it can still generate 
 * interrupts. This is a problem when there are multiple active hosts on
 * the bus.
 */

	    wb( istat, n810_istat_rst );
	    wb( istat, 0 );
	    krn$_sleep( 1000 );


/* Free all vectors */
	
	    if (pb->pb.vector) {
		io_disable_interrupts( pb, pb->pb.vector );
		int_vector_clear(pb->pb.vector);
	    } else {
		remq_lock( &pb->pb.pq.flink );
	    }

/* Free all semaphores */

	    krn$_semrelease( &pb->owner_s );
	    krn$_stop_timer( &pb->isr_t );
	    krn$_semrelease( &pb->isr_t.sem );
	    krn$_release_timer( &pb->isr_t );

/* Free all dynamic memory */

	    for( i = 0; i < pb->pb.num_sb; i++ )
		free(pb->pb.sb[i]);
	    free(pb->pb.sb);
	    
	    free(pb->scripts);

	    free(pb);

	    dev->devdep.io_device.devspecific = 0;

	    break;

    }					/* switch */

    return msg_success;

}					/* pke_setmode */

/*+
 * ============================================================================
 * = pke_poll_units -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  
 * FORM OF CALL:
 *  
 *	pke_poll_units(dev, verbose)
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct device *dev	- Pointer to device structure of adapter.
 *	int verbose		- if set, print node information.
 *
 * SIDE EFFECTS:
 *
 *	Files will be created.
 *
-*/
void pke_poll_units(struct device *dev, int verbose)
{
    struct n810_pb *pb = (struct n810_pb *) dev->devdep.io_device.devspecific;
    int i;
    struct scsi_sb *sb;
   
    /*
     *  Poll all available nodes.
     */

    for( i = 0; i < pb->pb.num_sb; i++ )
	{
	if( i == pb->node_id )
	    continue;
	sb = pb->pb.sb[i];
	sb->verbose = 1;
	scsi_send_inquiry(sb);
	if (killpending())
	    return;
	}
}

/*+
 * ============================================================================
 * = pke_establish_connection - establish a connection to given unit        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine establishes a connection to a given unit.
 *  
 * FORM OF CALL:
 *  
 *	pke_establish_connection( inode, dev, class )
 *  
 * RETURNS:
 *
 *	success or failure.
 *       
 * ARGUMENTS:
 *
 *	struct INODE *inode	- pointer to inode.
 *	struct device *dev	- pointer to device block
 *	int class		- class of connection
 *
 * SIDE EFFECTS:
 *
 *	Connection established to given unit.
 *
-*/
int pke_establish_connection(struct INODE *inode, struct device *dev, int class)
{

    struct scsi_sb *sb;
    struct n810_pb *pb;
    int unit;
    int node;
    int channel;

/* diagnostic mode, node already configured */

    if (diagnostic_mode_flag)
	return msg_success;

/* if device not in file system, fail */

    if (!inode->inuse) {
	return msg_failure;
    }

/* get device particulars */

    if (explode_devname(inode->name, 0, 0, &unit, &node, &channel, 0, 0) != msg_success) {
	boot_retry_counter = 0;
	return msg_failure;
    }

/* get device pointers */

    pb = (struct n810_pb *) dev->devdep.io_device.devspecific;

    if (( node >= 0 ) && ( node < 8 ))
	sb = pb->pb.sb[node];
    else
	return msg_failure;

/* if sb in use, device already configured */

    if (sb->ref != 0)
	return msg_success;

    sb->ref++;

    sb->verbose = 0;

    scsi_send_inquiry(sb);
    return msg_success;
}


int n810_init_adapter( struct device *dev )
    {
    char name[32];
    struct n810_pb *pb;

    pb = malloc(sizeof(*pb));

    /* load pb into device */

    dev->devdep.io_device.devspecific = (void *) pb;

    /* Load the vector from device to pb */
    
    pb->pb.vector = pci_get_vector(dev);

#if !RAWHIDE

    /* Run PKE in polled mode */

    pb->pb.vector = 0;

#endif

    /* Load the base address from the device to pb */

    pb->pb.hose = dev->hose;
    pb->pb.slot = dev->slot;
    pb->pb.bus = dev->bus;
    pb->pb.function = dev->function;
    pb->pb.channel = dev->channel;
    pb->pb.controller = dev->devdep.io_device.controller;
    pb->pb.config_device = dev;
    pb->pb.protocol = "pk";
    
    if (pci_get_base_address(dev, N810_CSR_BASE, &pb->pb.csr) != msg_success)
	return msg_failure;

#if RAWHIDE
    if ( platform() == ISP_MODEL ) {
	pprintf("NCR810 PCI to SCSI adapter is not emulated\n"); 
	return msg_failure;
    }
#endif

    n810_init_pb( pb );
    return msg_success;
    }

#endif

#if !( STARTSHUT || DRIVERSHUT )

/*+
 * ============================================================================
 * = n810_init - initialize port driver                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the NCR 53C810.
 *
 * FORM OF CALL:
 *  
 *	n810_init( )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
 *	A port block is allocated and initialized for each instance of port
 *	hardware of the appropriate type.
 *
-*/
int n810_init( )
    {
    int n810_init_pb( );

#if MODULAR
    ovly_load("PKE");
#endif
    find_pb( "n810", sizeof( struct n810_pb ), n810_init_pb );
    return( msg_success );
    }

#endif

void n810_init_pb( struct n810_pb *pb )
    {
    char name[32];

    n810_host_id_ev.ev_name = prefix( "_host_id" );
    ev_init_ev( &n810_host_id_ev );
    n810_fast_ev.ev_name = prefix( "_fast" );
    ev_init_ev( &n810_fast_ev );
    n810_disconnect_ev.ev_name = prefix( "_disconnect" );
    ev_init_ev( &n810_disconnect_ev );
    pke_init_port( pb );
    }

/*+
 * ============================================================================
 * = n810_init_n810 - initialize the N810 hardware                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the N810 hardware.  It walks through the
 *	necessary steps to bring a SCSI/DSSI port online.
 *  
 * FORM OF CALL:
 *  
 *	n810_init_n810( pb, dien, sien0, sien1 )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct n810_pb *pb		- port block
 *	int dien			- DMA interrupt mask
 *	int sien0			- SCSI interrupt mask 0
 *	int sien1			- SCSI interrupt mask 1
 *
 * SIDE EFFECTS:
 *
 *	The N810 is now ready for operation.
 *
-*/
void n810_init_n810( struct n810_pb *pb, int dien, int sien0, int sien1 )
    {
    /*
     *  Perform necessary register setup.
     */
    wb( istat, n810_istat_rst );
    wb( istat, 0 );
    wb( scntl0, n810_scntl0_epc|n810_scntl0_aap );
    wb( scntl1, n810_scntl1_exc );
#if MEDULLA
    if( ( modcnfg_read( ) & 0x18 ) == 0x18 )
	wb( scntl3, 0x33 );	/* 40 MHz clk */
    else
	wb( scntl3, 0x22 );	/* 32 MHz clk */
#else
    if( ( incfgl( pb, 0x00 ) == 0x000f1000 ) && ( incfgb( pb, 0x08 ) < 0x03 ) )
	wb( scntl3, 0x55 );	/* 80 MHz clock */
    else
	wb( scntl3, 0x33 );	/* 40 MHz clock */
#endif
    wb( scid, pb->node_id );
    wb( sxfer, 0 );
    wb( ctest4, n810_ctest4_mpee );
    wb( dmode, n810_dmode_bl1|n810_dmode_bl0 );
    wb( dien, dien );
    wb( dcntl, n810_dcntl_irqm|n810_dcntl_com );
    wb( sien0, sien0 );
    wb( sien1, sien1 );
    wb( stest3, n810_stest3_ean );
    wb( respid, 1 << pb->node_id );
    wl( scratchb, 0x96000069 | ( pb->node_id << 8 ) | ( pb->fast << 16 ) );
    }

unsigned char n810_read_byte_csr( struct n810_pb *pb, int offset )
    {
    unsigned char value;

    value = inmemb( pb, pb->pb.csr + offset );
    return( value );
    }

void n810_write_byte_csr( struct n810_pb *pb, int offset, unsigned char value )
    {
    outmemb( pb, pb->pb.csr + offset, value );
    }

unsigned long n810_read_long_csr( struct n810_pb *pb, int offset )
    {
    unsigned long value;

    value = inmeml( pb, pb->pb.csr + offset );
    return( value );
    }

void n810_write_long_csr( struct n810_pb *pb, int offset, unsigned long value )
    {
    outmeml( pb, pb->pb.csr + offset, value );
    }

#if DRIVERSHUT

/*+
 * ============================================================================
 * = pke_setmode - change driver state				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	This routine changes the state of the port driver.  The following
 * 	states are relevant:
 *
 *	    DDB$K_STOP -
 *			Suspends the  port driver.  pke_poll stops checking
 *			things in this state, and interrupts from the
 *			device are dismissed.
 *
 *	    DDB$K_RESTARTABLE -
 *			Same as DDB$K_STOP, except the port driver may be
 *			restarted dynamically by the class driver.
 *
 *	    DDB$K_START -
 *			Changes the   port   driver   to   interrupt  mode.
 *			pke_poll is once again enabled, as well as device
 *			interrupt handling.
 *
 *	    DDB$K_INTERRUPT -
 *			Same as DDB$K_START.
 *  
 *	    DDB$K_ASSIGN  -	
 *			Perform system startup functions.
 *
 * FORM OF CALL:
 *  
 *	pke_setmode(mode, dev)
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
 *	struct device *dev - pointer to device information
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int pke_setmode(int mode, struct pb *gpb)
{
    int i, j, k, t;
    struct scsi_sb *sb;
    struct n810_pb *pb;

    switch (mode) {
	case DDB$K_ASSIGN: 
	    pke_setmode( DDB$K_START, gpb );
	    pke_setmode( DDB$K_STOP, gpb );
	    break;

	case DDB$K_START: 

	case DDB$K_INTERRUPT: 

	    if ( gpb->dpb ) {
		gpb->dpb->ref++;
		return msg_success;
	    } else {
		alloc_dpb( gpb, &pb, sizeof( struct n810_pb ));
	    }
#if 0
	    if (dev->flags)
		return msg_failure;

#endif

/* initialize the class driver */

#if MODULAR
	    scsi_shared_adr = ovly_load("SCSI");
	    if (!scsi_shared_adr)
		return msg_failure;
#endif

	    krn$_set_console_mode(0);

	    n810_init_pb( pb );
	    
	    return ( msg_success );
    
	case DDB$K_STOP: 

	    pb = gpb->dpb;

	    if (!pb)
		return msg_failure;

	    if (pb->pb.ref == 0)
		return msg_success;

	    pb->pb.ref--;
	    if (pb->pb.ref != 0)
		return msg_success;

/* Break all virtual connections */

	    for( i = 0; i < pb->pb.num_sb; i++ ) {
		sb = pb->pb.sb[i];
		if (sb->state == sb_k_open) {
		    scsi_break_vc(sb);
		}
                krn$_semrelease(&sb->ready_s);
	    }


/* 
 * If the chip is not reset when shutdown, then it can still generate 
 * interrupts. This is a problem when there are multiple active hosts on
 * the bus.
 */

	    wb( istat, n810_istat_rst );
	    wb( istat, 0 );
	    krn$_sleep( 1000 );


/* Free all vectors */
	
	    if (pb->pb.vector) {
		io_disable_interrupts( pb, pb->pb.vector );
		int_vector_clear(pb->pb.vector);
	    } else {
		remq_lock( &pb->pb.pq.flink );
	    }

/* Free all semaphores */

	    krn$_semrelease( &pb->owner_s );
	    krn$_stop_timer( &pb->isr_t );
	    krn$_semrelease( &pb->isr_t.sem );
	    krn$_release_timer( &pb->isr_t );

/* Free all dynamic memory */

	    for( i = 0; i < pb->pb.num_sb; i++ )
		free(pb->pb.sb[i]);
	    free(pb->pb.sb);
	    
	    free(pb->scripts);

	    free( pb );

	    gpb->dpb = 0;

	    break;

    }					/* switch */

    return msg_success;

}					/* pke_setmode */

/*+
 * ============================================================================
 * = pke_poll_units -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  
 * FORM OF CALL:
 *  
 *	pke_poll_units(dev, verbose)
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct device *dev	- Pointer to device structure of adapter.
 *	int verbose		- if set, print node information.
 *
 * SIDE EFFECTS:
 *
 *	Files will be created.
 *
-*/
void pke_poll_units(struct pb *gpb, int verbose)
{
    int i;
    struct scsi_sb *sb;
    struct n810_pb *pb;
  
    pb = gpb->dpb;

    /*
     *  Poll all available nodes.
     */

    for( i = 0; i < pb->pb.num_sb; i++ )
	{
	if( i == pb->node_id )
	    continue;
	sb = pb->pb.sb[i];
	sb->verbose = 1;
	scsi_send_inquiry(sb);
	if (killpending())
	    return;
	}
}

/*+
 * ============================================================================
 * = pke_establish_connection - establish a connection to given unit        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine establishes a connection to a given unit.
 *  
 * FORM OF CALL:
 *  
 *	pke_establish_connection( inode, pb, class )
 *  
 * RETURNS:
 *
 *	success or failure.
 *       
 * ARGUMENTS:
 *
 *	struct INODE *inode	- pointer to inode.
 *	struct device *pb	- pointer to port block
 *	int class		- class of connection
 *
 * SIDE EFFECTS:
 *
 *	Connection established to given unit.
 *
-*/
int pke_establish_connection(struct INODE *inode, struct pb *gpb, int class)
{
    struct scsi_sb *sb;
    int unit;
    int node;
    int channel;
    struct n810_pb *pb;
  
    pb = gpb->dpb;

/* diagnostic mode, node already configured */

    if (diagnostic_mode_flag)
	return msg_success;

/* if device not in file system, fail */

    if (!inode->inuse) {
	return msg_failure;
    }

/* get device particulars */

    if (explode_devname(inode->name, 0, 0, &unit, &node, &channel, 0, 0, 0, 0) != msg_success) {
	boot_retry_counter = 0;
	return msg_failure;
    }

/* get device pointers */

    if (( node >= 0 ) && ( node < 8 ))
	sb = pb->pb.sb[node];
    else
	return msg_failure;

/* if sb in use, device already configured */

    if (sb->ref != 0)
	return msg_success;

    sb->ref++;

    sb->verbose = 0;

    scsi_send_inquiry(sb);
    return msg_success;
}
#if !MODULAR
int n810_init( )
    {
    add_driver_pointers( "n810", pke_setmode, pke_poll_units, pke_establish_connection );
    return( msg_success );
    }
#endif
#endif
