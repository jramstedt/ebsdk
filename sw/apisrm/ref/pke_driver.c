#define SSM 0

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
 * Abstract:	PCI SCSI port driver for the SymBios (NCR) 810/825/875.
 *
 *		The 810/825/875 developed by NCR is now owned by SymBios.
 *		The 810/825/875 is used on many platforms as the default
 * 		removable storage PCI SCSI controller.
 *		The 810 is also used on the KZPAA module.
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *      ska	11-Nov-1998	Conditionalize for YukonA
 *
 *	jwj	22-Oct-1997	removed medulla conditionals for enable_interrupts() and disable_interrupts()
 *
 *	dpm	22-Apr-1996	Fixed the timeout reg write for SCAM
 *
 *	jje	26-Feb-1996	Conditionalize for Cortex
 *
 *	dwn	02-Nov-1995	Added APS SCAM Support
 *
 *	rbb	13-Mar-1995	Conditionalize for EB164
 *
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	er	14-Sep-1994	Conditionalize for SD164
 *
 *	er	30-Jun-1994	Conditionalize for EB64+
 *
 *	dtr	16-Feb-1994	made the driver interrupt driven for mustang
 *
 *	mdr	07-Jan-1994	Added LCA support for Medulla.  Added Medulla
 *				hardware build switches.  
 *
 *	jmp	11-29-93	Add avanti support
 *
 *	sfs	14-Apr-1993	Initial entry.
 *
 *      D. Marsh 30-Jul-1993    Took out pass 1 fixes for morgan.
 *
 *	D. Rice	10-Nov-1993	Changes for mustang.  Defined window_base for
 *				the DMA.  The dma will go to address 1gb and up
 *				but gets remapped by the cpu to address 0.  The
 *				only change was in the definition of pke_phys.
 *				The rest of the code was unchanged.
 *
 *      D. Marsh 17-Dec-1994    Going to polled mode to fix a temporary problem
 *				with mustang.
 *
 *
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:n810_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:n810_pb_def.h"
#include "cp$src:scsi_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:ub_def.h"
#include "cp$inc:platform_io.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#if MEDULLA
#include "cp$src:medulla_system.h"
#endif

#define DEBUG 0

#define rb(x) n810_read_byte_csr(pb,offsetof(struct n810_csr,x))
#define rl(x) n810_read_long_csr(pb,offsetof(struct n810_csr,x))
#define wb(x,y) n810_write_byte_csr(pb,offsetof(struct n810_csr,x),y)
#define wl(x,y) n810_write_long_csr(pb,offsetof(struct n810_csr,x),y)
#define prefix(x) set_io_prefix(pb,name,x)
#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define malloc_a(x,y,z) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN|DYN$K_ALIGN,y,z)
#define free(x) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)

#define d_(x) n810_dien_/**/x
#define s0_(x) n810_sien0_/**/x
#define s1_(x) n810_sien1_/**/x

#if SSM
#define d_mask (d_(mdpe)|d_(bf)|d_(ssi)|d_(sir)|d_(iid))
#else
#define d_mask (d_(mdpe)|d_(bf)|d_(sir)|d_(iid))
#endif

#define s0_mask (s0_(ma)|s0_(sge)|s0_(udc)|s0_(rst)|s0_(par))
#define s1_mask (s1_(sto))

#define pke_phys(x) ((int)(x) | io_get_window_base(pb))
#define pke_virt(x) (pb->scripts+(int)(x)-(int)&pke_scripts)

extern struct FILE *el_fp;
extern struct QUEUE pollq;
#if !(STARTSHUT || DRIVERSHUT)
extern int scsi_poll;
#endif
extern int pke_script;
extern int pke_sel;
extern int pke_msg_out;
extern int pke_cmd;
extern int pke_dat_out;
extern int pke_dat_in;
extern int pke_check_sts;
extern int pke_sts;
extern int pke_msg_in;
extern int pke_msg_in_ext;
extern int pke_msg_in_continue;
extern int pke_msg_in_reject;
extern int pke_msg_out_short;
extern int pke_flush;
extern int pke_msg_in_buf;
extern int pke_scripts;
extern int pke_scripts_size;
extern int pke_patches;
extern int pke_patches_size;

#if SCAM
extern unsigned char scam_time;
#endif

/*+
 * ============================================================================
 * = pke_init_port - initialize a given port                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the port driver data structures and the
 *	port hardware of a specific, given port.
 *  
 * FORM OF CALL:
 *  
 *	pke_init_port ( pb );
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct n810_pb *pb
 *
 * SIDE EFFECTS:
 *
 *	A port block is initialized for each instance of port
 *	hardware of the appropriate type.
 *
-*/
void pke_init_port( struct n810_pb *pb )
    {
    char *c;
    char name[32];
    int i;
    int t;
    int *p;
    int *q;
    int node_id;
    int fast;
    int disconnect;
    int temp;
#if !(STARTSHUT || DRIVERSHUT)
    int pke_setmode( );
#endif
    int pke_poll_for_interrupt( );
    int pke_interrupt( );
#if !(STARTSHUT || DRIVERSHUT)
    int pke_poll( );
#endif
    struct EVNODE evnode;
    struct EVNODE *ev;

    log_driver_init( pb );

#if (STARTSHUT || DRIVERSHUT)
    pb->pb.ref++;
#endif

    ev = &evnode;    
    ev_read( prefix( "_host_id" ), &ev, 0 );
    node_id = ev->value.integer & 7;
    ev_read( prefix( "_fast" ), &ev, 0 );
    fast = ev->value.integer & 1;
    ev_read( prefix( "_disconnect" ), &ev, 0 );
    disconnect = ev->value.integer & 1;
    pb->scripts = malloc( &pke_scripts_size );
    memcpy( pb->scripts, &pke_scripts, &pke_scripts_size );
    p = &pke_patches;
    for( i = 0; i < &pke_patches_size; i++ )
	{
	q = p[i] + pb->scripts;
	*q = pke_phys( *q + pb->scripts );
	}
    pb->node_id = node_id;
    pb->fast = fast;
    pb->disconnect = disconnect;
#if !(STARTSHUT || DRIVERSHUT)
    pb->pb.setmode = pke_setmode;
#endif
    pb->pb.pq.routine = pke_poll_for_interrupt;
    pb->pb.pq.param = pb;
    if( pb->pb.vector )
	{
	pb->pb.mode = DDB$K_INTERRUPT;
	pb->pb.desired_mode = DDB$K_INTERRUPT;
	int_vector_set( pb->pb.vector, pke_interrupt, pb );
	io_enable_interrupts( pb, pb->pb.vector );
	}
    else
	{
	pb->pb.mode = DDB$K_POLLED;
	pb->pb.desired_mode = DDB$K_POLLED;
	insq_lock( &pb->pb.pq.flink, &pollq ); 
	}
    pb->pb.protocol = "pk";
    set_io_name( pb->pb.name, 0, 0, node_id, pb );
    set_io_alias( pb->pb.alias, 0, 0, pb );
    sprintf( pb->pb.info, "SCSI Bus ID %d", node_id );
    temp = incfgl( pb, 0x00 );
    if( ( temp == 0x00011000 ) || ( temp == 0x00051000 ) || ( temp == 0x00031000 ) )
	{
	pb->pb.sb = malloc( 8 * sizeof( struct scsi_sb * ) );
	pb->pb.num_sb = 8;
	}
    else
	{
	pb->pb.sb = malloc( 16 * sizeof( struct scsi_sb * ) );
	pb->pb.num_sb = 16;
	}
    sprintf( pb->pb.string, "%-24s   %-8s   %24s",
	    pb->pb.name, pb->pb.alias, pb->pb.info );
    krn$_seminit( &pb->isr_t.sem, 0, "pke_isr" );
    krn$_init_timer( &pb->isr_t );
    krn$_seminit( &pb->owner_s, 1, "pke_owner" );
    pb->owner_l.req_ipl = IPL_SYNC;
    pke_reset( pb );
    fprintf( el_fp, "port %s initialized, scripts are at %x\n",
	    pb->pb.name, pb->scripts );
    pb->poll_active = 1;
#if !(STARTSHUT || DRIVERSHUT)
    krn$_create( pke_poll, 0, 0, 5, 0, 4096,
	    prefix( "_poll" ), "nl", "r", "nl", "w", "nl", "w", pb, 1 );
#endif
    }

/*+
 * ============================================================================
 * = pke_reset - reset the port hardware                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine resets the port hardware.  It initializes it completely
 *	so that further operations may take place.
 *  
 * FORM OF CALL:
 *  
 *	pke_reset( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct n810_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	The port hardware may be initialized and made ready for action.
 *
-*/
void pke_reset( struct n810_pb *pb )
    {
    int i;
    int pke_command( );
    struct scsi_sb *sb;

    pke_init_n810( pb );
    /*
     *  Statically allocate an SB for each possible node connected to this
     *  port.  The N810 supports up to eight attached nodes (the N810 counts
     *  as one but for simplicity we ignore this fact).  Fill in each SB.
     */
    for( i = 0; i < pb->pb.num_sb; i++ )
	{
	sb = malloc( sizeof( *sb ) );
	pb->pb.sb[i] = sb;
	sb->pb = pb;
	sb->ub.flink = &sb->ub.flink;
	sb->ub.blink = &sb->ub.flink;
	sb->command = pke_command;
	sb->ok = 1;
	sb->node_id = i;
	if( pb->node_id == i )
	    sb->local = 1;
	sb->sdtr = 1;
	set_io_name( sb->name, 0, 0, sb->node_id, pb );
#if (STARTSHUT || DRIVERSHUT)
        krn$_seminit(&sb->ready_s, 0, "scsi_sb_ready");
#endif
	}
    pke_bus_reset( pb );
    }

/*+
 * ============================================================================
 * = pke_init_n810 - initialize the N810 hardware                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the N810 hardware.  It walks through the
 *	necessary steps to bring a SCSI port online.
 *  
 * FORM OF CALL:
 *  
 *	pke_init_n810( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct n810_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	The N810 is now ready for operation.
 *
-*/
void pke_init_n810( struct n810_pb *pb )
    {
    /*
     *  Perform necessary register setup.
     */
    if( pb->pb.vector )
	n810_init_n810( pb, d_mask, s0_mask, s1_mask );
    else
	n810_init_n810( pb, 0, 0, 0 );
    /* give the chip time to figure out if it's SE or LVD */
    krn$_micro_delay( 5000 );
    }

/*+
 * ============================================================================
 * = pke_bus_reset - reset the SCSI bus                                       =
 * ============================================================================
 *         
 * OVERVIEW:
 *  
 *	This routine resets the SCSI bus.
 *  
 * FORM OF CALL:
 *  
 *	pke_bus_reset( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct n810_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	The SCSI bus is reset.
 *
-*/
void pke_bus_reset( struct n810_pb *pb )
    {
    int i;
    struct scsi_sb *sb;

    fprintf( el_fp, "resetting the SCSI bus on %s\n", pb->pb.name );
    /*
     *  Reset the SCSI bus.
     */
    wb( scntl1, n810_scntl1_rst );
    krn$_micro_delay( 25 );
    wb( scntl1, n810_scntl1_exc );
    krn$_sleep( 250 );
    /*
     *  Clear internal chip error bits.
     */
    rb( sist0 );
    rb( sist1 );
    rb( dstat );
    /*
     *  Show we've lost context.
     */
    for( i = 0; i < pb->pb.num_sb; i++ )
	{
	sb = pb->pb.sb[i];
	if( sb )
	    sb->sdtr = 1;
	}
    }

/*+
 * ============================================================================
 * = pke_command - send a SCSI command to a remote node                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine sends a SCSI command to a remote node, allowing 30
 *	seconds for the remote node to complete the command.  Since only
 *	one command may be in progress at a time, the PB owner semaphore
 *	is used as a hardware lock.
 *  
 * FORM OF CALL:
 *  
 *	pke_command( sb, lun, cmd, cmd_len, dat_out, dat_out_len,
 *		dat_in, dat_in_len, sts )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_failure			- failed to send the command
 *
 * ARGUMENTS:
 *
 *	struct scsi_sb *sb		- system block
 *	int lun				- LUN
 *	unsigned char *cmd		- pointer to Command bytes
 *	int cmd_len			- length of Command bytes
 *	unsigned char *dat_out		- pointer to Data Out bytes
 *	int dat_out_len			- length of Data Out bytes
 *	unsigned char *dat_in		- pointer to Data In bytes
 *	int dat_in_len			- length of Data In bytes
 *	unsigned char *sts		- pointer to Status byte
 *
 * SIDE EFFECTS:
 *
 *	A command may be sent to a SCSI device.
 *
-*/
int pke_command( struct scsi_sb *sb, int lun, unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int dat_out_len,
	unsigned char *dat_in, int dat_in_len, unsigned char *sts )
    {
    int t;
    int pri;
    struct PCB *pcb;
    struct n810_pb *pb;

    /*
     *  Bump up the priority of this thread for the duration.
     */
    pcb = getpcb( );
    pri = pcb->pcb$l_pri;
    pcb->pcb$l_pri = 5;
    /*
     *  Acquire ownership of the port hardware, and start a timer to deal
     *  with selection timeouts.
     */
    pb = sb->pb;
#if DEBUG
    fprintf( el_fp, "attempting to send to %s\n", sb->name );
#endif
    krn$_wait( &pb->owner_s );
    if( ( sb->lun_map & ( 1 << lun ) ) || ( cmd[0] != scsi_k_cmd_inquiry ) )
         krn$_start_timer( &pb->isr_t, 300000 );
    else
         krn$_start_timer( &pb->isr_t, 10000 );
    /*
     *  Send the command.
     */
    t = pke_do_send( sb, lun, cmd, cmd_len, dat_out, dat_out_len,
	    dat_in, dat_in_len, sts );
    /*
     *  Stop the timer, release ownership of the port hardware, and return.
     */
    krn$_stop_timer( &pb->isr_t );
    krn$_post( &pb->owner_s );
    if( ( t != msg_success ) && ( sb->lun_map & ( 1 << lun ) ) )
	fprintf( el_fp, "failed to send to %s\n", sb->name );
    /*
     *  Restore the old priority.
     */
    pcb->pcb$l_pri = pri;
    return( t );
    }

/*+
 * ============================================================================
 * = pke_do_send - build the script to send a SCSI command                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds the script to send a SCSI command to a remote
 *	node.  After selection is complete, the timer value is changed from
 *	250 milliseconds to 30 seconds.
 *  
 * FORM OF CALL:
 *  
 *	pke_do_send( sb, lun, cmd, cmd_len, dat_out, dat_out_len,
 *		dat_in, dat_in_len, sts )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_failure			- failed to send the command
 *
 * ARGUMENTS:
 *
 *	struct scsi_sb *sb		- system block
 *	int lun				- LUN
 *	unsigned char *cmd		- pointer to Command bytes
 *	int cmd_len			- length of Command bytes
 *	unsigned char *dat_out		- pointer to Data Out bytes
 *	int dat_out_len			- length of Data Out bytes
 *	unsigned char *dat_in		- pointer to Data In bytes
 *	int dat_in_len			- length of Data In bytes
 *	unsigned char *sts		- pointer to Status byte
 *
 * SIDE EFFECTS:
 *
 *	A command may be sent to a SCSI device.
 *
-*/

/*
 *  Note:  The script knows about the layout of the following
 *         structure (struct dsa)!  Don't touch it!
 */
struct dsa {
    unsigned int sel_id;
    unsigned int msg_out_len;
    unsigned char *msg_out;
    unsigned int cmd_len;
    unsigned char *cmd;
    unsigned int dat_out_len;
    unsigned char *dat_out;
    unsigned int dat_in_len;
    unsigned char *dat_in;
    unsigned int sts_len;
    unsigned char *sts;
    };

int pke_do_send( struct scsi_sb *sb, int lun, unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int dat_out_len,
	unsigned char *dat_in, int dat_in_len, unsigned char *sts )
    {
    unsigned char msg_out[16];
    unsigned char *msg_in;
    unsigned char dstat;
    unsigned char sist0;
    unsigned char sist1;
    unsigned char *sav_dat_out;
    unsigned char *sav_dat_in;
    unsigned int sav_dat_out_len;
    unsigned int sav_dat_in_len;
    int i;
    int t;
    int sdtr;
    int tp;
    int mo;
    struct n810_pb *pb;
    struct n810_bm *bm;
    struct n810_io *io;
    struct n810_tc *tc;
    struct dsa dsa;

    pb = sb->pb;
    /*
     *  Always send an Identify message.
     */
    if( pb->disconnect )
	msg_out[0] = scsi_k_msg_identify | scsi_k_msg_id_disc | lun;
    else
	msg_out[0] = scsi_k_msg_identify | lun;
    t = 1;
    /*
     *  Set bus speed correctly, and prepare for SDTR (to turn off synchronous
     *  transfers) if necessary.
     */
#if AVANTI || MIKASA || ALCOR || K2 || MTU || CORTEX || YUKONA || TAKARA || APC_PLATFORM		/* no new platforms on this line */
    sdtr = 0;
#else
    sdtr = sb->sdtr;
#endif
    if( sdtr )
	{
#if DEBUG
	fprintf( el_fp, "attempting SDTR with %s\n", sb->name );
#endif
	msg_out[1] = scsi_k_msg_ext_message;
	msg_out[2] = 3;
	msg_out[3] = scsi_k_msg_ext_sdtr;
	msg_out[4] = 0;
	msg_out[5] = 0;
	t = 6;
	}
    /*
     *  Fill in the address and length fields of the pre-built SCSI script
     *  and of the DSA structure.
     */
    io = pke_virt( &pke_sel );
    io->io_id = sb->node_id;
    dsa.msg_out_len = t;
    dsa.msg_out = pke_phys( msg_out );
    dsa.cmd_len = cmd_len;
    dsa.cmd = pke_phys( cmd );
    if( dat_out_len )
	{
	dsa.dat_out_len = dat_out_len;
	dsa.dat_out = pke_phys( dat_out );
	sav_dat_out_len = dsa.dat_out_len;
	sav_dat_out = dsa.dat_out;
	}
    if( dat_in_len )
	{
	dsa.dat_in_len = dat_in_len;
	dsa.dat_in = pke_phys( dat_in );
	sav_dat_in_len = dsa.dat_in_len;
	sav_dat_in = dsa.dat_in;
	}
    dsa.sts_len = 1;
    dsa.sts = pke_phys( sts );
#if SSM
    wb( dcntl, rb( dcntl ) | n810_dcntl_ssm );
#endif
#if SCAM
    wb( stime0, scam_time );
#else
    wb( stime0, 0x0d );
#endif
    wb( ctest3, n810_ctest3_clf );
    wl( dsa, pke_phys( &dsa ) );
    /*
     *  Prepare for an interrupt to occur, and hand the script to the N810.
     */
    spinlock( &pb->owner_l );
    dstat = 0;
    sist0 = 0;
    sist1 = 0;
    pb->int_pending = 1;
    pb->isr_t.sem.count = 0;
    pb->dstat = 0;
    pb->sist0 = 0;
    pb->sist1 = 0;
    wl( dsp, pke_phys( pke_virt( &pke_script ) ) );
#if SSM
    wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
    spinunlock( &pb->owner_l );
    while( 1 )
	{
	/*
	 *  Wait for an interrupt or a timeout.
	 */
	krn$_wait( &pb->isr_t.sem );
	if( pb->int_pending )
	    {
	    /*
	     *  No interrupt occurred, so reset things.
	     */
	    if( sb->state == sb_k_open )
		fprintf( el_fp, "command timeout on %s, dstat = %02x, sist0 = %02x, sist1 = %02x\n",
			sb->name, dstat, sist0, sist1 );
	    pke_init_n810( pb );
	    pke_bus_reset( pb );
	    return( msg_failure );
	    }
	/*
	 *  Prepare for another interrupt.
	 */
	spinlock( &pb->owner_l );
	dstat = pb->dstat;
	sist0 = pb->sist0;
	sist1 = pb->sist1;
	pb->int_pending = 1;
	pb->isr_t.sem.count = 0;
	pb->dstat = 0;
	pb->sist0 = 0;
	pb->sist1 = 0;
	spinunlock( &pb->owner_l );
	/*
	 *  If we see a SCSI bus reset, then quit now.  If we know about
	 *  this node, return success and a SCSI status of busy so that the
	 *  command gets retried; otherwise, return failure.
	 */
	if( sist0 & n810_sist0_rst )
	    {
	    if( sb->state == sb_k_open )
		{
		sts[0] = scsi_k_sts_busy;
		return( msg_success );
		}
	    else
		return( msg_failure );
	    }
	/*
	 *  If a script INT instruction has been executed, dispatch on the
	 *  interrupt value.
	 */
	else if( dstat & n810_dstat_sir )
	    {
	    t = rl( dsps );

	    switch( t )
		{
		case n810_k_ok:
		    /*
		     *  The interrupt indicates success.
		     */
		    sb->ok = 1;
		    if( sdtr && ( sts[0] == scsi_k_sts_good ) )
			sb->sdtr = 0;
		    return( msg_success );

		case n810_k_dat_out_done:
		    /*
		     *  The interrupt indicates that the Data Out phase has
		     *  completed.
		     */
		    dsa.dat_out_len = 0;
		    wl( dsp, pke_phys( pke_virt( &pke_check_sts ) ) );
#if SSM
		    wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
		    break;

		case n810_k_dat_in_done:
		    /*
		     *  The interrupt indicates that the Data In phase has
		     *  completed.
		     */
		    dsa.dat_in_len = 0;
		    wl( dsp, pke_phys( pke_virt( &pke_check_sts ) ) );
#if SSM
		    wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
		    break;

		case n810_k_dssi:
		    /*
		     *  The interrupt indicates that the remote device is a
		     *  DSSI device rather than a SCSI device.
		     */
		    if( sb->ok )
			{
			sb->ok = 0;
			fprintf( el_fp, "device at %s is not SCSI\n", sb->name );
			}
		    pke_init_n810( pb );
		    return( msg_failure );

		case n810_k_timeout:
		    /*
		     *  The interrupt indicates that selection failed due
		     *  to a timeout.
		     */
		    if( sb->state == sb_k_open )
			fprintf( el_fp, "selection timeout on %s, dstat = %02x, sist0 = %02x, sist1 = %02x\n",
				sb->name, dstat, sist0, sist1 );
		    return( msg_failure );

		case n810_k_reselected:
		    /*
		     *  The interrupt indicates that the target reselected.
		     */
		    dsa.dat_out_len = sav_dat_out_len;
		    dsa.dat_out = sav_dat_out;
		    dsa.dat_in_len = sav_dat_in_len;
		    dsa.dat_in = sav_dat_in;
		    wl( dsp, pke_phys( pke_virt( &pke_msg_in_continue ) ) );
#if SSM
		    wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
		    break;

		case n810_k_got_msg_in:
		    /*
		     *  The interrupt indicates that a Message In has been
		     *  received.  Handle it.
		     */
		    msg_in = pke_virt( &pke_msg_in_buf );
		    bm = pke_virt( &pke_msg_in_ext );
		    if( sdtr
			    && ( msg_in[0] == scsi_k_msg_ext_message )
			    && ( msg_in[1] == scsi_k_msg_ext_sdtr ) )
			{
			tp = msg_in[2] * 4;
			mo = max( msg_in[3], 8 );
			fprintf( el_fp, "SDTR with %s completed\n", sb->name );
			sb->sdtr = 0;
			wl( dsp, pke_phys( pke_virt( &pke_msg_in_continue ) ) );
#if SSM
			wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
			}
		    else if( msg_in[0] == scsi_k_msg_save_dp )
			{
			sav_dat_out_len = dsa.dat_out_len;
			sav_dat_out = dsa.dat_out;
			sav_dat_in_len = dsa.dat_in_len;
			sav_dat_in = dsa.dat_in;
			wl( dsp, pke_phys( pke_virt( &pke_msg_in_continue ) ) );
#if SSM
			wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
			}
		    else if( msg_in[0] == scsi_k_msg_restore_dp )
			{
			dsa.dat_out_len = sav_dat_out_len;
			dsa.dat_out = sav_dat_out;
			dsa.dat_in_len = sav_dat_in_len;
			dsa.dat_in = sav_dat_in;
			wl( dsp, pke_phys( pke_virt( &pke_msg_in_continue ) ) );
#if SSM
			wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
			}
#if 0
		    else if( pb->disconnect
			    && ( msg_in[0] == scsi_k_msg_ext_message )
			    && ( msg_in[1] == scsi_k_msg_ext_modify_dp ) )
			{
			/*
			 *  The message is "Modify Data Pointer".  I have no
			 *  way to test if I implement this message correctly,
			 *  and few/no devices ever send it, and it is legal
			 *  to reject it, so that's what I'll do.  That's why
			 *  this code is disabled.
			 */
			fprintf( el_fp, "modify data pointer received on %s\n", sb->name );
			}
#endif
		    else if( msg_in[0] == scsi_k_msg_reject )
			{
			if( sdtr )
			    fprintf( el_fp, "SDTR with %s rejected\n",
				    sb->name );
			wl( dsp, pke_phys( pke_virt( &pke_msg_in_continue ) ) );
#if SSM
			wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
			}
		    else
			{
			if( msg_in[0] == 0x01 )
			    fprintf( el_fp, "message %02x/%02x/%02x... received from %s\n",
				    msg_in[0], bm->bm_byte_count, msg_in[1], sb->name );
			else if( ( msg_in[0] >= 0x20 ) && ( msg_in[0] <= 0x2f ) )
			    fprintf( el_fp, "message %02x/%02x received from %s\n",
				    msg_in[0], msg_in[1], sb->name );
			else
			    fprintf( el_fp, "message %02x received from %s\n",
				    msg_in[0], sb->name );
			wl( dsp, pke_phys( pke_virt( &pke_msg_in_reject ) ) );
#if SSM
			wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
			}
		    break;

		default:
		    /*
		     *  Some error occurred.
		     */
		    fprintf( el_fp, "while connected to %s, dsps = %d\n",
			    sb->name, t );
		    wl( dsp, pke_phys( pke_virt( &pke_flush ) ) );
#if SSM
		    wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
		    krn$_sleep( 500 );
		    if( rl( dsps ) != n810_k_ok )
			{
			fprintf( el_fp, "failed to flush %s, dsps = %d\n",
				sb->name, t );
			pke_init_n810( pb );
			pke_bus_reset( pb );
			}
		    return( msg_failure );
		}
	    }
	/*
	 *  If the interrupt indicates "phase mismatch", then check first
	 *  for a short Data In or Data Out phase.
	 */
	else if( sist0 & n810_sist0_ma )
	    {
	    int byte_count;
	    int dfifo;
	    int in_fifo;
	    int t;
	    struct n810_dcmd_dbc *p = &t;

	    /*
	     *  Get the instruction being executed when the phase mismatch
	     *  occurred.  Dispatch on the expected phase (the "old" phase).
	     */
	    t = rl( dcmd_dbc );
	    byte_count = p->dbc_byte_count;
	    dfifo = rb( dfifo ) & n810_dfifo_bo;
	    wb( ctest3, n810_ctest3_clf );
	    switch( p->dcmd_phase )
		{
		/*
		 *  Fix up a short Data Out or Data In phase.  Shorten the
		 *  length by the remaining byte count.  The next phase
		 *  should be Status, so restart the script at that point.
		 */
		case n810_k_dat_out:
		    in_fifo = ( dfifo - byte_count ) & 0x7f;
		    byte_count += in_fifo;
		    if( rb( sstat0 ) & n810_sstat0_olf )
			byte_count += 1;
		    dsa.dat_out += dsa.dat_out_len - byte_count;
		    dsa.dat_out_len = byte_count;
		    wl( dsp, pke_phys( pke_virt( &pke_check_sts ) ) );
#if SSM
		    wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
		    break;

		case n810_k_dat_in:
		    dsa.dat_in += dsa.dat_in_len - byte_count;
		    dsa.dat_in_len = byte_count;
		    wl( dsp, pke_phys( pke_virt( &pke_check_sts ) ) );
#if SSM
		    wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
		    break;

		/*
		 *  If it's Command phase, then the target is being stupid
		 *  and probably wants to report a Check condition.  Let it.
		 */
		case n810_k_cmd:
		    wl( dsp, pke_phys( pke_virt( &pke_check_sts ) ) );
#if SSM
		    wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
		    break;

		/*
		 *  If it's Message Out phase, then the target is rejecting
		 *  our SDTR.  Go with the flow.
		 */
		case n810_k_msg_out:
		    wl( dsp, pke_phys( pke_virt( &pke_msg_out_short ) ) );
#if SSM
		    wb( dcntl, rb( dcntl ) | n810_dcntl_std );
#endif
		    break;

		/*
		 *  Any phase mismatches that we don't expect cause the
		 *  operation to be aborted, and things get reset.
		 */
		default:
		    fprintf( el_fp, "bad phase mismatch on %s, dcmd/dbc = %08x\n",
			    sb->name, t );
		    pke_init_n810( pb );
		    pke_bus_reset( pb );
		    break;
		}
	    }
	/*
	 *  If the interrupt indicates that selection failed due to a
	 *  timeout, quit now.
	 */
	else if( sist1 & n810_sist1_sto )
	    {
	    if( sb->state == sb_k_open )
		fprintf( el_fp, "selection timeout on %s, dstat = %02x, sist0 = %02x, sist1 = %02x\n",
			sb->name, dstat, sist0, sist1 );
	    return( msg_failure );
	    }
	else
	    /*
	     *  Complain if we don't understand why this interrupt occurred.
	     */
	    {
	    fprintf( el_fp, "bad interrupt received on %s, dstat = %02x, sist0 = %02x, sist1 = %02x\n",
		    sb->name, dstat, sist0, sist1 );
	    pke_init_n810( pb );
	    pke_bus_reset( pb );
	    }
	}
    }

#if !(STARTSHUT || DRIVERSHUT)
/*+
 * ============================================================================
 * = pke_poll - poll to see which nodes exist                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine performs polling, in order to get an accurate picture
 *	of which nodes exist and which don't.  Polling periodically is a way
 *	of detecting nodes coming and going.  This routine is careful never to
 *	poll the port's own node number.
 *  
 * FORM OF CALL:
 *  
 *	pke_poll( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct n810_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	Nodes may be found.  Nodes may be lost.
 *
-*/
void pke_poll( struct n810_pb *pb, int poll )
    {
    char name[32];
    int i;

#if SCAM
    n810_Scam( pb );
#endif
    /*
     *  Poll all eight nodes as a group, pause for a while and repeat forever.
     */
    for( i = 0; i < pb->pb.num_sb; i++ )
	{
	if( i == pb->node_id )
	    continue;
	if( poll || scsi_poll )
	    scsi_send_inquiry( pb->pb.sb[i] );
	}
    pb->poll_active = 0;
    krn$_create_delayed( 30000, 0, pke_poll, 0, 0, 5, 0, 4096,
	    prefix( "_poll" ), pb, 0 );
    }

/*+
 * ============================================================================
 * = pke_setmode - stop or start the port driver                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine deals with requests to stop or start the port driver.
 *  
 * FORM OF CALL:
 *  
 *	pke_setmode( pb, mode )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct n810_pb *pb		- port block
 *	int mode			- the new mode
 *
 * SIDE EFFECTS:
 *
 *	The driver may be stopped or started.
 *
-*/
void pke_setmode( struct n810_pb *pb, int mode )
    {
    switch( mode )
	{
	case DDB$K_STOP:
	    if( pb->pb.mode != DDB$K_STOP )
		{
		krn$_wait( &pb->owner_s );
		wb( istat, n810_istat_rst );
		wb( istat, 0 );
		if( pb->pb.mode == DDB$K_INTERRUPT )
		    {
		    io_disable_interrupts( pb, pb->pb.vector );
		    }
		else
		    remq_lock( &pb->pb.pq.flink );
		pb->pb.mode = DDB$K_STOP;
		}
	    break;	    

	case DDB$K_START:
	    if( pb->pb.mode == DDB$K_STOP )
		{
		pb->pb.mode = pb->pb.desired_mode;
		if( pb->pb.desired_mode == DDB$K_INTERRUPT )
		    {
		    io_enable_interrupts( pb, pb->pb.vector );
		    }
		else
		    insq_lock( &pb->pb.pq.flink, &pollq );
		pke_init_n810( pb );
		pke_bus_reset( pb );
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
		pke_init_n810( pb );
		pke_bus_reset( pb );
		krn$_post( &pb->owner_s );
		}
	    break;
#endif
	}
    }
#endif

/*+
 * ============================================================================
 * = pke_poll_for_interrupt - poll for port interrupts                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine polls for port interrupts.  If an interrupt is pending,
 *	the ISR is called.
 *  
 * FORM OF CALL:
 *  
 *	pke_poll_for_interrupt( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct n810_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	The ISR may be called.
 *
-*/
void pke_poll_for_interrupt( struct n810_pb *pb )
    {
    if( pb->pb.mode == DDB$K_POLLED )
	if( rb( istat ) & 3 )
	    pke_interrupt( pb );
    }

/*+
 * ============================================================================
 * = pke_interrupt - service a port interrupt                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine services port interrupts.  It posts the ISR semaphore,
 *	and then acknowledges the port's interrupt, allowing the port to
 *	interrupt again as needed.
 *  
 * FORM OF CALL:
 *  
 *	pke_interrupt( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct n810_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	Waiting tasks may be awoken.
 *
-*/
void pke_interrupt( struct n810_pb *pb )
    {
    unsigned char dstat;
    unsigned char sist0;
    unsigned char sist1;

    spinlock( &pb->owner_l );
#if WILDFIRE /* WildBRAT 1731 */
{
    unsigned int t;
    t = rl( sien0 );
    sist0 = ( t >> 16 ) & s0_mask;
    sist1 = ( t >> 24 ) & s1_mask;
}
#else
    sist0 = rb( sist0 ) & s0_mask;
    sist1 = rb( sist1 ) & s1_mask;
#endif
    dstat = rb( dstat ) & d_mask;
#if SSM
    if( dstat & n810_dstat_ssi )
	{
	dstat &= ~n810_dstat_ssi;
	if( !( dstat || sist0 || sist1 ) )
	    {
#if 0
	    int *t;

	    t = rl( dsp );
	    fprintf( el_fp, "ssi on %s, dsp = %x %08x/%08x %08x/%08x\n",
		    pb->pb.name, t, rl( dcmd_dbc ), rl( dnad ), t[0], t[1] );
#endif
	    wb( dcntl, rb( dcntl ) | n810_dcntl_std );
	    spinunlock( &pb->owner_l );
	    if( pb->pb.mode == DDB$K_INTERRUPT )
		io_issue_eoi( pb, pb->pb.vector );
	    return;
	    }
	}
#endif
    /*
     *  If anything interesting happened, tell whoever's waiting.
     */
    if( dstat || sist0 || sist1 )
	{
	pb->dstat |= dstat;
	pb->sist0 |= sist0;
	pb->sist1 |= sist1;
	if( pb->int_pending )
	    {
	    pb->int_pending = 0;
	    krn$_post( &pb->isr_t.sem );
	    }
	}
    spinunlock( &pb->owner_l );
    if( pb->pb.mode == DDB$K_INTERRUPT )
	io_issue_eoi( pb, pb->pb.vector );
    }
