#define ISP1020_INTERRUPTS 0

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
 * Abstract:	SCSI port driver
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *
 *
 *      mar     13-jul-1998     Remove setting differential mode from console
 *                              because isp1040 family nvram does not privide
 *                              the diff bit anymore. The diff bit on isp1020
 *                              is the 60hz bit on isp1040b. There is not
 *                              direct diff bit.
 *
 *      mar     14-feb-1996     Add support to determine the clock frequency 
 *				using Processor Status Register reserved bit-5.
 *
 *	swt	28-Jun-1995	Add Noritake platform.
 *	
 *	dtr	6-apr-1995	Disabled interrupts if vector is 0
 *	
 * 	rbb	13-Mar-1995	Conditionlize for the EB164
 *
 *	jmp	17-jun-1994 	Jim Peacock
 *				Add apecs chip suppot (window base)
 *				and clean up interrupt handler 
 * 
 *	sfs	03-Feb-1994	Initial entry.
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
#include "cp$src:isp1020_def.h"
#include "cp$src:isp1020_pb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:ub_def.h"
#include "cp$inc:platform_io.h"
#include "cp$inc:kernel_entry.h"
#if MODULAR
#include "cp$src:mem_def.h"
#undef isp1020_fw_version
#undef isp1020_fw_default_pointer
#undef isp1020_fw_default_length
#undef isp1020_soft_term_table
#define ISP1020_CSR_BASE 1
#endif

#if USE_BIOS_ROM
#else
#include "cp$src:isp1020_fw.h"
int isp1020_fw_default_pointer = isp1020_fw_default;
int isp1020_fw_default_length = sizeof( isp1020_fw_default );
#endif
                       
#include "cp$inc:prototypes.h"

#define DEBUG 0
#if DEBUG
int isp1020_debug = 0;
#endif

#define prefix(x) set_io_prefix(pb,name,x)
#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define free(x) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)
#define rw(x) isp1020_read_word_csr(pb,x)
#define ww(x,y) isp1020_write_word_csr(pb,x,y)

unsigned short isp1020_read_word_csr( struct isp1020_pb *pb, int offset );
unsigned short isp1020_write_word_csr( struct isp1020_pb *pb, int offset, unsigned short value );

extern struct FILE *el_fp;
extern struct QUEUE pollq;
extern struct EVNODE evlist;	/* header of the list of variables */
extern int ev_initing;
extern int scsi_reset;
#if !( STARTSHUT || DRIVERSHUT )
extern int scsi_poll;
#else
extern int boot_retry_counter;
extern int diagnostic_mode_flag;
#endif

isp1020_ev_sev( );
isp1020_ev_sev_init( );

struct set_param_table isp1020_soft_term_table[] = {
	{"off", 0},
	{"low", 0},
	{"high", 0},
	{"on", 0},
	{0}
};

struct env_table isp1020_host_id_ev = {
	0, 7,
	EV$K_NODELETE | EV$K_INTEGER,
	0, isp1020_ev_sev, 0, isp1020_ev_sev_init
};

struct env_table isp1020_soft_term_ev = {
	0, 1,
	EV$K_NODELETE | EV$K_STRING,
	isp1020_soft_term_table, isp1020_ev_sev, 0, isp1020_ev_sev_init
};

struct nvram isp1020_default_nvram = {
	NVRAM_DEF
};

#if MODULAR
int isp1020_nvram() {
    return (&isp1020_default_nvram);
}
#endif

int isp1020_ev_sev( char *name, struct EVNODE *ev )
    {
    char c;
    unsigned int id;
    int t;
    struct isp1020_pb *pb;

    if( ev_initing )
	return( msg_success );
    pb = ev->misc;
    isp1020_get_nvram( pb, &pb->nvram );
    c = name[5];
    if( c == 'h' )
	{
	id = ev->value.integer;
	if( id > 15 )
	    id -= 6;
	if( id > 15 )
	    {
	    ev->value.integer = pb->nvram.initiator_scsi_id;
	    err_printf( msg_ev_badvalue, name );
	    return( msg_failure );
	    }
	pb->nvram.initiator_scsi_id = id;
	if( id > 9 )
	    ev->value.integer = id + 6;
	else
	    ev->value.integer = id;
	}
    if( c == 's' )
	{
	    c = ev->value.string[1];
	    pb->nvram.low_term_enable = ( c == 'o' ) || ( c == 'n' );
	    pb->nvram.high_term_enable = ( c == 'i' ) || ( c == 'n' );
	    pb->nvram.diff_enable = 0;
	}
    t = isp1020_put_nvram( pb, &pb->nvram );
    return( t );
    }

int isp1020_ev_sev_init( struct env_table *et, char *value, struct isp1020_pb *pb )
    {
    char c;
    int t;

    c = et->ev_name[5];
    if( c == 'h' )
	{
	t = pb->nvram.initiator_scsi_id;
	if( t > 9 )
	    t += 6;
	}
    if( c == 's' )
	{
	    if( pb->nvram.low_term_enable )
		if( pb->nvram.high_term_enable )
		    t = "on";
		else
		    t = "low";
	    else
		if( pb->nvram.high_term_enable )
		    t = "high";
		else
		    t = "off";
	}
    return( t );
    }

#if STARTSHUT
/*+
 * ============================================================================
 * = isp1020_setmode - change driver state				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	This routine changes the state of the port driver.  The following
 * 	states are relevant:
 *
 *	    DDB$K_STOP -
 *			Suspends the  port driver.  isp1020_poll stops checking
 *			things in this state, and interrupts from the
 *			device are dismissed.
 *
 *	    DDB$K_RESTARTABLE -
 *			Same as DDB$K_STOP, except the port driver may be
 *			restarted dynamically by the class driver.
 *
 *	    DDB$K_START -
 *			Changes the   port   driver   to   interrupt  mode.
 *			isp1020_poll is once again enabled, as well as device
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
 *	isp1020_setmode(mode, dev)
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
int isp1020_setmode(int mode, struct device *dev)
{
    int i, j, k, t;
    struct isp1020_pb *pb;
    struct scsi_sb *sb;

    switch (mode) {
	case DDB$K_ASSIGN: 
	    isp1020_setmode( DDB$K_START, dev );
	    isp1020_setmode( DDB$K_STOP, dev );
	    break;

	case DDB$K_START: 

	case DDB$K_INTERRUPT: 
	    pb = (struct isp1020_pb *) dev->devdep.io_device.devspecific;

	    if (pb) {
		if (pb->pb.ref != 0) {
		    pb->pb.ref++;
		    return msg_success;
		}
	    }

	    if (dev->flags)
		return msg_failure;

/* initialize the class driver */

	    scsi_shared_adr = ovly_load("SCSI");
	    if (!scsi_shared_adr)
		return msg_failure;

	    krn$_set_console_mode(0);

	    if (isp1020_init(dev) != msg_success)
		return msg_failure;
	    break;	    

	case DDB$K_STOP: 
	    pb = (struct isp1020_pb *) dev->devdep.io_device.devspecific;

	    if (!pb)
		return msg_failure;

/* If it was already shutdown, return */

	    if (pb->pb.ref == 0)
		return msg_success;

	    pb->pb.ref--;
	    if (pb->pb.ref != 0)
		return msg_success;

/* Break all virtual connections */

	    for( i = 0; i < 16; i++ ) {
		sb = pb->pb.sb[i];
		if (sb->state == sb_k_open) {
		    fprintf( el_fp, "breaking virtual connection with sb %08x\n", sb );
		    scsi_break_vc(sb);
		}
#if STARTSHUT
                krn$_semrelease(&sb->ready_s);
#endif
	    }

/* Free all vectors */

	    fprintf( el_fp, "clearing interrupt vector %08x\n", pb->pb.vector);
	    int_vector_clear(pb->pb.vector);

/* Free all semaphores */

	    fprintf( el_fp, "free all semaphores\n");
	    krn$_semrelease( &pb->owner_s );
	    krn$_stop_timer( &pb->isr_t );
	    krn$_semrelease( &pb->isr_t.sem );
	    krn$_release_timer( &pb->isr_t );

/* Free all dynamic memory */

	    fprintf( el_fp, "free all dynamic memory\n");
	    for( i = 0; i < 16; i++ )
		free(pb->pb.sb[i]);
	    free(pb->pb.sb);

#if 0
/* need to keep the port block for isp1020 ev's */
	    free(pb);
	    dev->devdep.io_device.devspecific = 0;
#endif
	    break;

    }					/* switch */
    return msg_success;
}					/* isp1020_setmode */
/*+
 * ============================================================================
 * = isp1020_poll_units -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  
 * FORM OF CALL:
 *  
 *	isp1020_poll_units(dev, verbose)
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
void isp1020_poll_units(struct device *dev, int verbose)
{
    int i;
    struct scsi_sb *sb;
    struct isp1020_pb *pb;
   
    pb = (struct isp1020_pb *) dev->devdep.io_device.devspecific;

    /*
     *  Poll all sixteen nodes.
     */

    for( i = 0; i < 16; i++ )
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
 * = isp1020_establish_connection - establish a connection to given unit        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine establishes a connection to a given unit.
 *  
 * FORM OF CALL:
 *  
 *	isp1020_establish_connection( inode, dev, class )
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
int isp1020_establish_connection(struct INODE *inode, struct device *dev, int class)
{

    struct scsi_sb *sb;
    struct isp1020_pb *pb;
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

#if 0
/* Now that there is LUN support, this is not a good check */
    if ((unit % 100) != 0) {
	printf("Invalid unit number\n");
	boot_retry_counter = 0;
	return msg_failure;
    }
#endif

/* get device pointers */

    pb = (struct isp1020_pb *) dev->devdep.io_device.devspecific;

    if (( node >= 0 ) && ( node < 16 ))
	sb =  pb->pb.sb[node];
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


int isp1020_init( struct device *dev )
    {
    char name[32];
    struct isp1020_pb *pb;
    int vector_index;
    int int_pin;

    if (!( pb = dev->devdep.io_device.devspecific )) {
	pb = dyn$_malloc(sizeof(*pb),DYN$K_SYNC|DYN$K_NOOWN);
	dev->devdep.io_device.devspecific = (void *) pb;
    }

    /* Load the vector from device to pb */

    pb->pb.vector = pci_get_vector(dev);

    /* Load the base address from the device to pb */

    pb->pb.hose = dev->hose;
    pb->pb.slot = dev->slot;
    pb->pb.bus = dev->bus;
    pb->pb.function = dev->function;
    pb->pb.channel = dev->channel;
    pb->pb.controller = dev->devdep.io_device.controller;
    pb->pb.config_device = dev;
    pb->pb.protocol = "pk";
    
    io_disable_interrupts( pb, pb->pb.vector );

    if (pci_get_base_address(dev, ISP1020_CSR_BASE, &pb->pb.csr) != msg_success)
	return msg_failure;

    krn$_seminit( &pb->owner_s, 1, "pkq_owner" );

    isp1020_get_nvram( pb, &pb->nvram );

    isp1020_init_pb( pb );
    return msg_success;
    }
#endif
#if !( STARTSHUT || DRIVERSHUT )
int isp1020_init( )
    {
    int isp1020_init_pb( );

    find_pb( "isp1020", sizeof( struct isp1020_pb ), isp1020_init_pb );
    return( msg_success );
    }
#endif

void isp1020_init_pb( struct isp1020_pb *pb )
    {
    int i;
    int j;
    char name[32];
    struct EVNODE *evp;

    log_driver_init( pb );
    krn$_seminit( &pb->owner_s, 1, "pkq_owner" );
    isp1020_get_nvram( pb, &pb->nvram );
    isp1020_host_id_ev.ev_name = prefix( "_host_id" );
#if MODULAR
    i = ovly_call_save("ISP1020", isp1020_ev_sev_$offset);
    isp1020_host_id_ev.ev_wr_action = i;
    ev_init_ev( &isp1020_host_id_ev, pb );
#else
    ev_init_ev( &isp1020_host_id_ev, pb );
#endif
    isp1020_soft_term_ev.ev_name = prefix( "_soft_term" );
#if MODULAR
    j = ovly_call_save("ISP1020", isp1020_soft_term_table_$offset);
    isp1020_soft_term_ev.ev_wr_action = i;
    isp1020_soft_term_ev.validate = j;
    ev_init_ev( &isp1020_soft_term_ev, pb );
#else
    ev_init_ev( &isp1020_soft_term_ev, pb );
#endif
    isp1020_init_port( pb );
    }

void isp1020_init_port( struct isp1020_pb *pb )
    {
    char name[32];
#if !( STARTSHUT || DRIVERSHUT )
    int isp1020_setmode( );
    int isp1020_poll( );
#endif
    int isp1020_interrupt( );

#if ( STARTSHUT || DRIVERSHUT )
    pb->pb.ref++;
#endif    
    pb->node_id = pb->nvram.initiator_scsi_id;
    pb->soft_term = ( 0xff00
		    + ( pb->nvram.low_term_enable ? 0x0000 : 0x0040 )
		    + ( pb->nvram.high_term_enable ? 0x0000 : 0x0080 ) );
    set_io_name( pb->pb.name, 0, 0, pb->node_id, pb );
    set_io_alias( pb->pb.alias, 0, 0, pb );
    sprintf( pb->pb.info, "SCSI Bus ID %d", pb->node_id );
#if !( STARTSHUT || DRIVERSHUT )
    pb->pb.setmode = isp1020_setmode;
#endif
    if( pb->pb.vector )
	{
	pb->pb.mode = DDB$K_INTERRUPT;
	pb->pb.desired_mode = DDB$K_INTERRUPT;
	int_vector_set( pb->pb.vector, isp1020_interrupt, pb );
#if ISP1020_INTERRUPTS
	pprintf( "enabling ISP1020 interrupts on %s\n", pb->pb.name );
	pb->int_pending = 4;
	io_enable_interrupts( pb, pb->pb.vector );
#endif
	}
    else
	{
	pb->pb.mode = DDB$K_POLLED;
	pb->pb.desired_mode = DDB$K_POLLED;
	}
    pb->pb.sb = malloc( 16 * sizeof( struct scsi_sb * ) );
    pb->pb.num_sb = 16;
    sprintf( pb->pb.string, "%-24s   %-8s   %24s",
	    pb->pb.name, pb->pb.alias, pb->pb.info );
    krn$_seminit( &pb->isr_t.sem, 0, "pkq_isr" );
    krn$_init_timer( &pb->isr_t );
    isp1020_reset( pb );
    pb->poll_active = 1;
#if !( STARTSHUT || DRIVERSHUT )
    krn$_create( isp1020_poll, 0, 0, 5, 0, 4096,
	    prefix( "_poll" ), "nl", "r", "nl", "w", "nl", "w", pb, 1 );
#endif
    }

void isp1020_reset( struct isp1020_pb *pb )
    {
    int i;
    int n;
    int p;
    int isp1020_command( );
    struct scsi_sb *sb;

    /*
     *  Locate the firmware.
     */
#if USE_BIOS_ROM
    outcfgl( pb, 0x0030, 0x801f0001 );
    n = inmemw( 0x1f212e ) * 2;
    p = malloc( n );
    for( i = 0; i < n; i += 2 )
	*(unsigned short *)( p + i ) = inmemw( 0x1f2128 + i );
    outcfgl( pb, 0x0030, 0x801f0000 );
#else
    n = sizeof( isp1020_fw_default );
    p = isp1020_fw_default;
#endif
    pb->fw_address = p;
    pb->fw_length = n;
    /*
     *  Initialize the chip, and load the firmware.
     */
    krn$_wait( &pb->owner_s );
    isp1020_init_isp1020( pb );
    krn$_post( &pb->owner_s );
    /*
     *  Statically allocate an SB for each possible node connected to this
     *  port.  The ISP1020 supports up to sixteen attached nodes (the ISP1020
     *  counts as one but for simplicity we ignore this fact).  Fill in each
     *  SB.
     */
    for( i = 0; i < 16; i++ )
	{
	sb = malloc( sizeof( *sb ) );
	pb->pb.sb[i] = sb;
	sb->pb = pb;
	sb->ub.flink = &sb->ub.flink;
	sb->ub.blink = &sb->ub.flink;
	sb->command = isp1020_command;
	sb->ok = 1;
	sb->node_id = i;
	if( pb->node_id == i )
	    sb->local = 1;
	set_io_name( sb->name, 0, 0, sb->node_id, pb );
#if ( STARTSHUT || DRIVERSHUT )
        krn$_seminit(&sb->ready_s, 0, "scsi_sb_ready");
#endif
	}
    if( scsi_reset )
	{
	krn$_wait( &pb->owner_s );
	isp1020_bus_reset( pb );
	krn$_post( &pb->owner_s );
	}
    }

void isp1020_init_isp1020( struct isp1020_pb *pb )
    {
    int i;

    for( i = 0; i < 5; i++ )
	if( isp1020_init_isp1020_sub( pb ) == msg_success )
	    return;
	else
	    fprintf( el_fp, "failed to initialize %s, retrying...\n", pb->pb.name );
    fprintf( el_fp, "failed to initialize %s\n", pb->pb.name );
    }

int isp1020_init_isp1020_sub( struct isp1020_pb *pb )
    {
    unsigned char *c;
    unsigned short mor[6];
    unsigned short mir[6];
    int i;
    int q;
    int clock_rate;
    int sync_period;

    /*
     *  Perform necessary register setup.
     */
    ww( ICR, ICR_SOFT_RESET );
    krn$_sleep( 10 );
    for( i = 0; i < 100; i++ )
	if( rw( ICR ) & ICR_SOFT_RESET )
	    krn$_sleep( 10 );
	else
	    break;
    if( i == 100 )
	return( msg_failure );
    ww( HCCR, HCCR_RESET_RISC );
    ww( HCCR, HCCR_RELEASE_RISC );
    ww( HCCR, HCCR_WRITE_BIOS_ENA );
    for( i = 0; i < 100; i++ )
	if( rw( MOR + 0 ) )
	    krn$_sleep( 10 );
	else
	    break;
    if( i == 100 )
	return( msg_failure );
    ww( C1R, 0 );
    ww( ICR, 0 );
    /*
     *  Determine clock frequency: PSR<PSR_CLOCK_RATE> = 1 for 60Mhz,
     *  or = 0 for 40Mhz.
     *
     *  Sequence:  pause RISC processor, read PSR, unpause RISC processor.
     */
    ww( HCCR, HCCR_PAUSE_RISC );
    clock_rate = rw( PSR ) & PSR_CLOCK_RATE;
    ww( HCCR, HCCR_RELEASE_RISC );
    /*
     *  See if the firmware is already loaded.  If not, do so now, but if
     *  so, DON'T try to do it, or we'll hang.
     *
     *  We execute a command which will succeed if the firmware is loaded,
     *  and will fail otherwise.
     */
    mor[0] = OPCODE_ABOUT_FW;
    isp1020_issue_cmd( pb, mor, mir, 4 );
    if( ( mir[0] != 0x4000 ) && ( mir[0] != 0xffff ) )
	isp1020_load_start_fw( pb );
    mor[0] = OPCODE_SET_CLOCK_RATE;
    mor[1] = clock_rate ? 60 : 40;
    isp1020_issue_cmd( pb, mor, mir, 4 );
    if( mir[0] != 0x4000 )
	return( msg_failure );
    mor[0] = OPCODE_SET_ASYNC_SETUP_TIME;
    mor[1] = clock_rate ? 9 : 6;
    isp1020_issue_cmd( pb, mor, mir, 4 );
    if( mir[0] != 0x4000 )
	return( msg_failure );
    mor[0] = OPCODE_SET_SCSI_ID;
    mor[1] = pb->node_id;
    isp1020_issue_cmd( pb, mor, mir, 4 );
    if( mir[0] != 0x4000 )
	return( msg_failure );
    mor[0] = OPCODE_SET_RETRY_COUNT;
    mor[1] = 0;
    mor[2] = 0;
    isp1020_issue_cmd( pb, mor, mir, 4 );
    if( mir[0] != 0x4000 )
	return( msg_failure );
    mor[0] = OPCODE_WRITE_RAM_WORD;
    mor[1] = 0xff00;
    mor[2] = ( ( pb->soft_term >> 6 ) & 3 ) ^ 3;
    isp1020_issue_cmd( pb, mor, mir, 4 );
    if( mir[0] != 0x4000 )
	return( msg_failure );
    mor[1] = 0xff10;
    isp1020_issue_cmd( pb, mor, mir, 4 );
    if( mir[0] != 0x4000 )
	return( msg_failure );
    mor[1] = 0xff20;
    isp1020_issue_cmd( pb, mor, mir, 4 );
    if( mir[0] != 0x4000 )
	return( msg_failure );
    mor[1] = 0xff30;
    isp1020_issue_cmd( pb, mor, mir, 4 );
    if( mir[0] != 0x4000 )
	return( msg_failure );
    mor[1] = pb->soft_term;
    isp1020_issue_cmd( pb, mor, mir, 4 );
    if( mir[0] != 0x4000 )
	return( msg_failure );
    sync_period = clock_rate ? 17 : 25;
    for( i = 0; i < 16; i++ )
	{
	c = &pb->nvram.id[i];
	mor[0] = OPCODE_SET_TARGET_PARAMS;
	mor[1] = i << 8;
	mor[2] = c[0] << 8;
	mor[3] = ( ( c[3] & 15 ) << 8 ) | min( sync_period, c[2] );
	isp1020_issue_cmd( pb, mor, mir, 4 );
	if( mir[0] != 0x4000 )
	    return( msg_failure );
	}
    pb->command_index = 0;
    pb->status_index = 0;
    mor[4] = 0;
    mor[5] = 0;
    q = (int)pb->command | io_get_window_base( pb );
    mor[0] = OPCODE_INIT_REQUEST_QUEUE;
    mor[1] = QUEUE_SIZE;
    mor[2] = ( q >> 16 ) & 0xffff;
    mor[3] = q & 0xffff;
    isp1020_issue_cmd( pb, mor, mir, 6 );
    if( mir[0] != 0x4000 )
	return( msg_failure );
    q = (int)pb->status | io_get_window_base( pb );
    mor[0] = OPCODE_INIT_RESPONSE_QUEUE;
    mor[1] = QUEUE_SIZE;
    mor[2] = ( q >> 16 ) & 0xffff;
    mor[3] = q & 0xffff;
    isp1020_issue_cmd( pb, mor, mir, 6 );
    if( mir[0] != 0x4000 )
	return( msg_failure );
    ww( C1R, C1R_BURST_ENA | pb->nvram.fifo_threshold );
    ww( ICR, 0 );
    ww( SR, 0 );
    return( msg_success );
    }

int isp1020_load_start_fw( struct isp1020_pb *pb )
    {
    unsigned short mor[6];
    unsigned short mir[6];
    int n;
    int p;
    int q;

    /*
     *  Load and start the firmware.  Notice that we are allocating a
     *  temporary buffer to hold the firmware while we tell the chip to
     *  copy it.  This is because the firmware may be linked as part of
     *  the console.  If so, then it may fall into the fixed hole in EISA
     *  space, in which case, the chip can't reach it.  We guarantee to
     *  avoid this problem by telling the chip to access heap storage.
     */
    n = pb->fw_length;
    p = malloc( n );
    q = p | io_get_window_base( pb );
    memcpy( p, pb->fw_address, n );
    ww( HCCR, HCCR_WRITE_BIOS_ENA );
    mor[0] = OPCODE_LOAD_RAM;
    mor[1] = 0x1000;
    mor[2] = ( q >> 16 ) & 0xffff;
    mor[3] = q & 0xffff;
    mor[4] = n / 2;
    isp1020_issue_cmd( pb, mor, mir, 6 );
    free( p );
    if( mir[0] == 0x4000 )
	{
	mor[0] = OPCODE_VERIFY_CHECKSUM;
	mor[1] = 0x1000;
	isp1020_issue_cmd( pb, mor, mir, 4 );
	}
    if( mir[0] == 0x4000 )
	{
	mor[0] = OPCODE_EXECUTE_FW;
	mor[1] = 0x1000;
	isp1020_issue_cmd( pb, mor, mir, 4 );
	}
    if( mir[0] == 0x4000 )
	{
	mor[0] = OPCODE_ABOUT_FW;
	isp1020_issue_cmd( pb, mor, mir, 4 );
	}
    if( mir[0] == 0x4000 )
	{
	sprintf( pb->pb.version, "%d.%02d", mir[1], mir[2] );
	sprintf( pb->pb.string, "%-24s   %-8s   %24s  %s",
		pb->pb.name, pb->pb.alias, pb->pb.info, pb->pb.version );
	return( msg_success );
	}
    else
	{
	sprintf( pb->pb.version, "" );
	sprintf( pb->pb.string, "%-24s   %-8s   %24s",
		pb->pb.name, pb->pb.alias, pb->pb.info );
	return( msg_failure );
	}
    }

void isp1020_bus_reset( struct isp1020_pb *pb )
    {
    unsigned short mor[6];
    unsigned short mir[6];

    fprintf( el_fp, "resetting the SCSI bus on %s\n", pb->pb.name );
    mor[0] = OPCODE_BUS_RESET;
    mor[1] = 2;
    isp1020_issue_cmd( pb, mor, mir, 4 );
    }

int isp1020_issue_cmd( struct isp1020_pb *pb,
	unsigned short *mor, unsigned short *mir, int n )
    {
    int i;

    /*
     *  Wait for the mailbox to be free, and load it.
     */
    for( i = 0; i < 100; i++ )
	if( rw( HCCR ) & HCCR_HOST_INT )
	    krn$_sleep( 10 );
	else
	    break;
    if( i == 100 )
	{
	mir[0] = 0xffff;
	return( msg_failure );
	}
    for( i = 0; i < n; i++ )
	ww( MOR + i * 2, mor[i] );
    /*
     *  Give the mailbox command to the firmware, and wait for it to be done.
     */
    ww( HCCR, HCCR_SET_HOST_INT );
#if ISP1020_INTERRUPTS
    if( pb->pb.vector )
	ww( ICR, ICR_RISC_INT_ENA | ICR_INT_ENA );
#endif
    for( i = 0; i < 1000; i++ )
	if( rw( ISR ) & ISR_RISC_INT )
	    break;
	else
	    krn$_sleep( 10 );
    if( i == 1000 )
	{
	mir[0] = 0xffff;
	return( msg_failure );
	}
    /*
     *  Acknowledge command completion, and read status.
     */
    ww( HCCR, HCCR_CLEAR_RISC_INT );
    for( i = 0; i < n; i++ )
	mir[i] = rw( MIR + i * 2 );
    /*
     *  Unlock the mailbox.
     */
    ww( SR, 0 );
    return( msg_success );
    }

int isp1020_command( struct scsi_sb *sb, int lun, unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int dat_out_len,
	unsigned char *dat_in, int dat_in_len, unsigned char *sts,
	unsigned char *sense_dat_in, int *sense_dat_in_len )
    {
    unsigned short mir[6];
    int i;
    int t;
    struct isp1020_pb *pb;
    struct command *command;
    struct status *status;

    pb = sb->pb;
    krn$_wait( &pb->owner_s );
    command = pb->command + pb->command_index++;
    if( pb->command_index >= QUEUE_SIZE )
	pb->command_index = 0;
    memset( command, 0, sizeof( *command ) );
    command->entry_type = 1;
    command->entry_count = 1;
    command->lun = lun;
    command->target = sb->node_id;
    command->cdb_length = cmd_len;
    memcpy( command->cdb, cmd, cmd_len );
    if( ( sb->lun_map & ( 1 << lun ) ) || ( cmd[0] != scsi_k_cmd_inquiry ) )
	command->timeout = 300;
    else
	command->timeout = 10;
    if( dat_out_len )
	{
	command->control_flags = 0x0040;
	command->data_count = 1;
	command->data_address = (int)dat_out | io_get_window_base( pb );
	command->data_length = dat_out_len;
	}
    if( dat_in_len )
	{
	command->control_flags = 0x0020;
	command->data_count = 1;
	command->data_address = (int)dat_in | io_get_window_base( pb );
	command->data_length = dat_in_len;
	}
    if( cmd[0] == scsi_k_cmd_inquiry )
	{
	if( pb->nvram.id[sb->node_id].sync_data_transfers )
	    command->control_flags |= 0x0400;	/* force SDTR */
	if( pb->nvram.id[sb->node_id].wide_data_transfers )
	    command->control_flags |= 0x0800;	/* force WDTR */
	}

#if DEBUG

    /* Dump the control block */

    if (isp1020_debug) {
	pprintf("ISP1020 COMMAND BLOCK %x %x\n", command-pb->command, command);
	pprintf("	entry type %x\n", command->entry_type);
	pprintf("	entry count %x\n", command->entry_count);
	pprintf("	entry unused %x\n", command->entry_unused);
	pprintf("	entry flags %x\n", command->entry_flags);
	pprintf("	handle %x\n", command->handle);
	pprintf("	lun %x\n", command->lun);
	pprintf("	target %x\n", command->target);
	pprintf("	cdb length %x\n", command->cdb_length);
	pprintf("	control flags  %x\n", command->control_flags);
	pprintf("	reserved %x\n", command->reserved);
	pprintf("	timeout %x\n", command->timeout);
	pprintf("	data count %x\n", command->data_count);
	pprintf("	data address %x\n", command->data_address);
	pprintf("	data length %x\n", command->data_length);
	for (i = 0; i < command->cdb_length; i++)
	    pprintf("	cdb[%d] %x\n", i, command->cdb[i]);    
    }

#endif

    /*
     *  Write the updated request queue pointer.
     */
#if ISP1020_INTERRUPTS
    if( pb->pb.vector )
	{
	pb->isr_t.sem.count = 0;
	ww( ICR, ICR_RISC_INT_ENA | ICR_INT_ENA );
	}
#endif
    mb( );
    ww( MOR + 4 * 2, pb->command_index );
    /*
     *  Wait for an interrupt.
     */
#if ISP1020_INTERRUPTS
    if( pb->pb.vector )
	krn$_wait( &pb->isr_t.sem );
#else
    t = command->timeout * 100 + 1000;
    for( i = 0; i < t; i++ )
	if( rw( ISR ) & ISR_RISC_INT )
	    break;
	else
	    krn$_sleep( 10 );
    if( i == t )
	{
	mir[0] = 0xffff;
	status = 0;
	}
    else
#endif
	{
	/*
	 *  Read response queue pointer before clearing interrupt.
	 */
	mir[5] = rw( MIR + 5 * 2 );
	/*
	 *  Acknowledge command completion, and read status.
	 */
	ww( HCCR, HCCR_CLEAR_RISC_INT );
	if( rw( SR ) & SR_LOCK )
	    {
	    mir[0] = rw( MIR + 0 * 2 );
	    status = 0;
	    /*
	     *  Unlock the mailbox.
	     */
	    ww( SR, 0 );
	    }
	else
	    {
	    /*
	     *  No asynchronous status, so go visit the response queue.
	     */
	    status = pb->status + pb->status_index++;
	    if( pb->status_index >= QUEUE_SIZE )
		pb->status_index = 0;
	    if( pb->status_index != mir[5] )
		mir[0] = 0xfffe;
	    else if( pb->command_index != pb->status_index )
		mir[0] = 0xfffd;
	    else if( status->entry_type != 3 )
		mir[0] = 0xfffc;
	    else
		mir[0] = 0x0000;
	    mb( );
	    ww( MOR + 5 * 2, pb->status_index );
	    }
	}

#if DEBUG

    /* Dump the data */

    if (isp1020_debug && dat_in) {
	pprintf("\nDATA IN\n");
	for (i = 0; i <= 16; i++)
	    pprintf("   %x\n", dat_in[i]);
    }

    if (isp1020_debug && dat_out) {
	pprintf("\nDATA OUT\n");
	for (i = 0; i <= 16; i++)
	    pprintf("   %x\n", dat_out[i]);
    }

    /* Dump the status block */

    if (isp1020_debug) {
	pprintf("ISP1020 STATUS BLOCK %x %x\n", status-pb->status, status);
	pprintf("	entry type %x\n", status->entry_type);
	pprintf("	entry count %x\n", status->entry_count);
	pprintf("	entry unused %x\n", status->entry_unused);
	pprintf("	entry flags %x\n", status->entry_flags);    
	pprintf("	handle %x\n", status->handle);
	pprintf("	scsi status %x\n", status->scsi_status);
	pprintf("	completion status %x\n", status->completion_status);
	pprintf("	state flags %x\n", status->state_flags);
	pprintf("	status flags %x\n", status->status_flags);
	pprintf("	time %x\n", status->time);
	pprintf("	request sense length %x\n", status->request_sense_length);
	pprintf("	residual length %x\n", status->residual_length);
	for (i = 0; i < status->request_sense_length; i++)    
	    pprintf("	request sense[%d] %x\n", i, status->request_sense[i]);
    }

#endif

    if( status && ( mir[0] == 0x0000 ) )
	{
	if( ( status->completion_status == 0x0000 )
		|| ( status->completion_status == 0x0015 ) )
	    {
	    if( status->state_flags & 0x2000 )
		{
		t = min( MAX_SENSE_SZ, status->request_sense_length );
		memcpy( sense_dat_in, status->request_sense, t );
		*sense_dat_in_len = t;
		}
	    *sts = status->scsi_status;
	    t = msg_success;
	    }
	else if( status->completion_status == 0x0004 )
	    {
	    isp1020_init_isp1020( pb );
	    *sts = scsi_k_sts_busy;
	    t = msg_success;
	    }
	else
	    {
	    if( ( status->completion_status != 0x0001 )
		    || ( ( status->state_flags & 0x0200 ) != 0 )
		    || ( ( status->status_flags & 0x0040 ) == 0 ) )
		{
		isp1020_init_isp1020( pb );
		fprintf( el_fp, "I/O failed on %s\n", sb->name );
		hd_helper( el_fp, 0, command, sizeof( *command ), 1 );
		hd_helper( el_fp, 0, status, sizeof( *status ), 1 );
		}
	    t = msg_failure;
	    }
	}
    else if( mir[0] == 0x8001 )
	{
	isp1020_init_isp1020( pb );
	*sts = scsi_k_sts_busy;
	t = msg_success;
	}
    else
	{
	isp1020_init_isp1020( pb );
	fprintf( el_fp, "mailbox I/O failed on %s, status = %04x\n",
		sb->name, mir[0] );
	t = msg_failure;
	}
    krn$_post( &pb->owner_s );
    return( t );
    }

#if !( STARTSHUT || DRIVERSHUT )
void isp1020_poll( struct isp1020_pb *pb, int poll )
    {
    char name[32];
    int i;

    /*
     *  Poll all sixteen nodes as a group, pause for a while and repeat forever.
     */
    for( i = 0; i < 16; i++ )
	{
	if( i == pb->node_id )
	    continue;
	if( poll || scsi_poll )
	    scsi_send_inquiry( pb->pb.sb[i] );
	}
    pb->poll_active = 0;
    krn$_create_delayed( 30000, 0, isp1020_poll, 0, 0, 5, 0, 4096,
	    prefix( "_poll" ), pb, 0 );
    }

void isp1020_setmode( struct isp1020_pb *pb, int mode )
    {
    switch( mode )
	{
	case DDB$K_STOP:
	    if( pb->pb.mode != DDB$K_STOP )
		{
		krn$_wait( &pb->owner_s );
		if( pb->pb.mode == DDB$K_INTERRUPT )
		    io_disable_interrupts( pb, pb->pb.vector );
		isp1020_init_isp1020( pb );
		pb->pb.mode = DDB$K_STOP;
		}
	    break;	    

	case DDB$K_START:
	    if( pb->pb.mode == DDB$K_STOP )
		{
		pb->pb.mode = pb->pb.desired_mode;
		isp1020_init_isp1020( pb );
		if( scsi_reset )
		    isp1020_bus_reset( pb );
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
		isp1020_init_isp1020( pb );
		if( scsi_reset )
		    isp1020_bus_reset( pb );
		krn$_post( &pb->owner_s );
		}
	    break;
#endif
	}
    }

#endif

void isp1020_interrupt( struct isp1020_pb *pb )
    {
#if ISP1020_INTERRUPTS
    if( pb->int_pending )
	{
	pb->int_pending -= 1;
	pprintf( "ISP1020 interrupt on %s\n", pb->pb.name );
	}
    ww( ICR, 0 );
    io_issue_eoi( pb, pb->pb.vector );
    krn$_post( &pb->isr_t.sem );
#else
    ww( HCCR, HCCR_CLEAR_RISC_INT );
    ww( SR, 0 );
    io_disable_interrupts( pb, pb->pb.vector );
#endif
    }

unsigned short isp1020_read_word_csr( struct isp1020_pb *pb, int offset )
    {
    unsigned short value;

    value = inmemw( pb, pb->pb.csr + offset );
#if DEBUG
    if (isp1020_debug)
	pprintf("CSR READ: pb %x pb->pb.csr %x offset %x value %x\n", pb,pb->pb.csr,offset,value); 
#endif
    return( value );
    }

void isp1020_write_word_csr( struct isp1020_pb *pb, int offset, unsigned short value )
    {
#if DEBUG
    if (isp1020_debug)
	pprintf("CSR WRITE: pb %x pb->pb.csr %x offset %x value %x\n", pb,pb->pb.csr,offset,value); 
#endif
    outmemw( pb, pb->pb.csr + offset, value );
    }

void isp1020_nvram_on( struct isp1020_pb *pb )
    {
    krn$_wait( &pb->owner_s );
    ww( NR, NR_SELECT );
    krn$_micro_delay( 2 );
    ww( NR, NR_SELECT|NR_CLOCK );
    krn$_micro_delay( 2 );
    }

void isp1020_nvram_off( struct isp1020_pb *pb )
    {
    ww( NR, 0 );
    krn$_post( &pb->owner_s );
    }

void isp1020_nvram_toggle( struct isp1020_pb *pb )
    {
    ww( NR, 0 );
    krn$_micro_delay( 2 );
    ww( NR, NR_CLOCK );
    krn$_micro_delay( 2 );
    ww( NR, NR_SELECT );
    krn$_micro_delay( 2 );
    ww( NR, NR_SELECT|NR_CLOCK );
    krn$_micro_delay( 2 );
    }

void isp1020_nvram_command( struct isp1020_pb *pb, int command )
    {
    int bit;
    int i;

    /*
     *  Write the command MSB first.
     */
    for( i = 8; i >= 0; i-- )
	{
	if( command & ( 1 << i ) )
	    bit = NR_SELECT|NR_DATA_OUT;
	else
	    bit = NR_SELECT;
	ww( NR, bit );
	krn$_micro_delay( 2 );
	ww( NR, bit | NR_CLOCK );
	krn$_micro_delay( 2 );
	ww( NR, bit );
	krn$_micro_delay( 2 );
	}
    }

unsigned short isp1020_read_nvram( struct isp1020_pb *pb, int offset )
    {
    int data;
    int bit;
    int i;

    isp1020_nvram_on( pb );
    /*
     *  Send a READ command.
     */
    isp1020_nvram_command( pb, NVRAM_READ | offset );
    /*
     *  Set up to read a 16-bit data value.
     */
    data = 0;
    /*
     *  Read the data MSB first.
     */
    for( i = 15; i >= 0; i-- )
	{
	data <<= 1;
	ww( NR, NR_SELECT|NR_CLOCK );
	krn$_micro_delay( 2 );
	if( rw( NR ) & NR_DATA_IN )
	    data |= 1;
	ww( NR, NR_SELECT );
	krn$_micro_delay( 2 );
	}
    /*
     *  We're done with this chunk, turn the chip off.
     */
    isp1020_nvram_off( pb );
    return( data );
    }

int isp1020_write_nvram( struct isp1020_pb *pb, int offset, unsigned short data )
    {
    int bit;
    int i;

    isp1020_nvram_on( pb );
    /*
     *  Send a WRITE ENABLE command.
     */
    isp1020_nvram_command( pb, NVRAM_WRITE_ENABLE );
    /*
     *  Turn the NVRAM chip off and back on.  This finishes the WRITE ENABLE
     *  command.
     */
    isp1020_nvram_toggle( pb );
    /*
     *  Send a WRITE command.
     */
    isp1020_nvram_command( pb, NVRAM_WRITE | offset );
    /*
     *  Set up to write a 16-bit data value.
     *
     *  Write the data MSB first.
     */
    for( i = 15; i >= 0; i-- )
	{
	if( ( data >> i ) & 1 )
	    bit = NR_SELECT|NR_DATA_OUT;
	else
	    bit = NR_SELECT;
	ww( NR, bit );
	krn$_micro_delay( 2 );
	ww( NR, bit | NR_CLOCK );
	krn$_micro_delay( 2 );
	ww( NR, bit );
	krn$_micro_delay( 2 );
	}
    /*
     *  Turn the NVRAM chip off and back on.  This starts the programming
     *  cycle.
     */
    isp1020_nvram_toggle( pb );
    /*
     *  Wait until NR_DATA_IN indicates that the programming cycle is
     *  complete.
     */
    for( i = 0; i < 5000; i++ )
	if( rw( NR ) & NR_DATA_IN )
	    break;
	else
	    krn$_micro_delay( 2 );
    isp1020_nvram_off( pb );
    if( i < 5000 )
	return( msg_success );
    else
	return( msg_failure );
    }

int isp1020_get_nvram( struct isp1020_pb *pb, struct nvram *nvram )
    {
    unsigned char sum;
    unsigned char *pc;
    unsigned short *ps;
    int i;

    pc = nvram;
    ps = nvram;
    sum = 0;
    for( i = 0; i < sizeof( *nvram ) / 2; i++ )
	{
	*ps++ = isp1020_read_nvram( pb, i );
	sum += *pc++;
	sum += *pc++;
	}
    if( ( nvram->isp_id == 'ISP ' )
	    && ( nvram->version >= 2 )
	    && ( sum == 0 ) )
	{
	return( msg_success );
	}
    else
	{
	memcpy( nvram, &isp1020_default_nvram, sizeof( *nvram ) );
	return( msg_failure );
	}
    }

int isp1020_put_nvram( struct isp1020_pb *pb, struct nvram *nvram )
    {
    unsigned char sum;
    unsigned char *pc;
    unsigned short *ps;
    int i;
    int t;

    pc = nvram;
    ps = nvram;
    sum = 0;
    for( i = 0; i < sizeof( *nvram ) / 2; i++ )
	{
	sum += *pc++;
	if( pc == &nvram->check_sum )
	    *pc++ = 256 - sum;
	else
	    sum += *pc++;
	t = isp1020_write_nvram( pb, i, *ps++ );
	if( t != msg_success )
	    {
	    err_printf( "NVRAM programming failed on %s\n", pb->pb.name );
	    return( t );
	    }
	}
    return( msg_success );
    }

int isp1020_get_clock(struct isp1020_pb *pb)
{
    int clock_rate;
    ww( HCCR, HCCR_PAUSE_RISC );
    clock_rate = rw( PSR ) & PSR_CLOCK_RATE;
    ww( HCCR, HCCR_RELEASE_RISC );
    return(clock_rate);
}


#if DRIVERSHUT
/*+
 * ============================================================================
 * = isp1020_setmode - change driver state				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	This routine changes the state of the port driver.  The following
 * 	states are relevant:
 *
 *	    DDB$K_STOP -
 *			Suspends the  port driver.  isp1020_poll stops checking
 *			things in this state, and interrupts from the
 *			device are dismissed.
 *
 *	    DDB$K_RESTARTABLE -
 *			Same as DDB$K_STOP, except the port driver may be
 *			restarted dynamically by the class driver.
 *
 *	    DDB$K_START -
 *			Changes the   port   driver   to   interrupt  mode.
 *			isp1020_poll is once again enabled, as well as device
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
 *	isp1020_setmode(mode, pb)
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
int isp1020_setmode(int mode, struct pb *gpb)
{
    int i, j, k, t;
    struct isp1020_pb *pb;
    struct scsi_sb *sb;

    switch (mode) {
	case DDB$K_ASSIGN: 
	    isp1020_setmode( DDB$K_START, gpb );
	    isp1020_setmode( DDB$K_STOP, gpb );
	    break;

	case DDB$K_START: 

	case DDB$K_INTERRUPT: 

	    if ( gpb->dpb ) {
		gpb->dpb->ref++;
		return msg_success;
	    } else {
		alloc_dpb( gpb, &pb, sizeof( struct isp1020_pb ));
	    }

	    if (pb) {
		if (pb->pb.ref != 0) {
		    pb->pb.ref++;
		    return msg_success;
		}
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

	    io_disable_interrupts( pb, pb->pb.vector );
	    isp1020_init_pb( pb );
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

/* Break all virtual connections */

	    for( i = 0; i < 16; i++ ) {
		sb = pb->pb.sb[i];
		if (sb->state == sb_k_open) {
		    fprintf( el_fp, "breaking virtual connection with sb %08x\n", sb );
		    scsi_break_vc(sb);
		}
                krn$_semrelease(&sb->ready_s);
	    }

/* Free all vectors */

	    fprintf( el_fp, "clearing interrupt vector %08x\n", pb->pb.vector);
	    int_vector_clear(pb->pb.vector);

/* Free all semaphores */

	    fprintf( el_fp, "free all semaphores\n");
	    krn$_semrelease( &pb->owner_s );
	    krn$_stop_timer( &pb->isr_t );
	    krn$_semrelease( &pb->isr_t.sem );
	    krn$_release_timer( &pb->isr_t );

/* Free all dynamic memory */

	    fprintf( el_fp, "free all dynamic memory\n");
	    for( i = 0; i < 16; i++ )
		free(pb->pb.sb[i]);
	    free(pb->pb.sb);

	    free(pb);
	    gpb->dpb = 0;
	    break;

    }					/* switch */
    return msg_success;
}					/* isp1020_setmode */
/*+
 * ============================================================================
 * = isp1020_poll_units -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  
 * FORM OF CALL:
 *  
 *	isp1020_poll_units(pb, verbose)
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
int isp1020_poll_units(struct pb *gpb, int verbose)
{
    int i;
    struct scsi_sb *sb;
    struct isp1020_pb *pb;
   
    pb = gpb->dpb;

    /*
     *  Poll all sixteen nodes.
     */

    for( i = 0; i < 16; i++ )
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
 * = isp1020_establish_connection - establish a connection to given unit        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine establishes a connection to a given unit.
 *  
 * FORM OF CALL:
 *  
 *	isp1020_establish_connection( inode, pb, class )
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
int isp1020_establish_connection(struct INODE *inode, struct pb *gpb, int class)
{

    struct scsi_sb *sb;
    struct isp1020_pb *pb;
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

    if (explode_devname(inode->name, 0, 0, &unit, &node, &channel, 0, 0, 0, 0) != msg_success) {
	boot_retry_counter = 0;
	return msg_failure;
    }

/* get device pointers */

    pb = gpb->dpb;

    if (( node >= 0 ) && ( node < 16 ))
	sb =  pb->pb.sb[node];
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
#if MODULAR

int isp1020_init( struct isp1020_pb *pb )
    {
    char name[16];
    int vector_index;
    int int_pin;

    io_disable_interrupts( pb, pb->pb.vector );

    krn$_seminit( &pb->owner_s, 1, "pkq_owner" );

    isp1020_get_nvram( pb, &pb->nvram );

    isp1020_init_pb( pb );
    return msg_success;
    }

#else

int isp1020_init( )
    {
    add_driver_pointers( "isp1020", isp1020_setmode, isp1020_poll_units, isp1020_establish_connection );
    return( msg_success );
    }

#endif
#endif
