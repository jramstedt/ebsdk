/* pue_driver.c
 *
 * Copyright (C) 1995 by
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
 * Abstract:	DSSI port driver.  This driver supports both the
 *              modular-start-stop style of operation as well as
 *              the monolithic always-running style of operation.  This
 *              driver operates with the MSCP class driver and 
 *              works with the KFPSA adapter.
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	njc	10-Dec-1995	Added startup and shutdown for turbo.
 *	sfs	17-Aug-1995	Initial entry.
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:prototypes.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:n810_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:n810_pb_def.h"
#include "cp$src:dssi_def.h"
#include "cp$src:pue_ppd_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:cb_def.h"
#include "cp$src:print_var.h"
#include "cp$inc:platform_io.h"
#include "cp$src:version.h"

#if MSCP_SERVER
#include "cp$src:server_def.h"
#endif

#include "cp$inc:kernel_entry.h"

#if ( STARTSHUT || DRIVERSHUT )
#include "cp$src:kfpsa_def.h"
#include "cp$src:mbx_def.h"
#include "cp$src:mem_def.h"
#include "cp$src:shac_def.h"
#define BASE0 0x10
#define BASE1 0x14
#define KFPSA_ALLOWED_ERRORS 10
#endif

#if ( STARTSHUT || DRIVERSHUT )
#define DEBUG 0
#define DEBUG_MALLOC 0
#else
#define DEBUG 0
#define DEBUG_MALLOC 0
#endif

int kfpsa_debug_flag = 0;

/*
 * 1 = top level
 * 2 = malloc/free
 */

#define rb(x) n810_read_byte_csr(pb,offsetof(struct n810_csr,x))
#define rl(x) n810_read_long_csr(pb,offsetof(struct n810_csr,x))
#define wb(x,y) n810_write_byte_csr(pb,offsetof(struct n810_csr,x),y)
#define wl(x,y) n810_write_long_csr(pb,offsetof(struct n810_csr,x),y)
#define prefix(x) set_io_prefix(pb,name,x)
#define msg_selected 2

#define d_(x) n810_dien_/**/x
#define s0_(x) n810_sien0_/**/x
#define s1_(x) n810_sien1_/**/x

#define d_mask (d_(mdpe)|d_(bf)|d_(sir)|d_(iid))
#define s0_mask (s0_(ma)|s0_(sel)|s0_(sge)|s0_(udc)|s0_(rst)|s0_(par))
#define s1_mask (s1_(sto)|s1_(hth))

#define pue_phys(x) ((int)(x) | io_get_window_base(pb))
#define pue_virt(x) (pb->scripts+(int)(x)-(int)&pue_scripts)

#if DEBUG
char *pue_opcodes[32] = {
	"0", "DG", "MSG", "CNF", "4", "IDREQ", "RST", "STRT",
	"DATREQ0", "DATREQ1", "DATREQ2", "ID", "12", "LB", "14", "15",
	"SNTDAT", "RETDAT", "18", "19", "20", "21", "22", "23", "24",
	"25", "26", "27", "28", "29", "30", "31" };
#endif

extern struct SEMAPHORE scs_lock;
extern null_procedure( );
#if !(MODULAR && !( STARTSHUT || DRIVERSHUT ))
extern int scs_connect( );
#if MSCP_SERVER
extern int scs_accept( );
#endif
#endif

#if ( STARTSHUT || DRIVERSHUT )
extern int boot_retry_counter;
extern int diagnostic_mode_flag;
#if !RAWHIDE
void pke_init_port(){};
#endif
struct device *kfpsa_d;
#endif

#if DEBUG
#define dqprintf _dqprintf
#define dprintf _dprintf
#define d2printf _d2printf
#else
#define dqprintf qprintf
#define dprintf(fmt, args)
#define d2printf(fmt, args)
#endif

#if DEBUG_MALLOC
#define malloc(size,sym) \
	kfpsa_malloc(size,"sym")
#define free(adr,sym) \
	kfpsa_free(adr,"sym")
#else
#define malloc(x,y) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define free(x,y) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)
#endif

#if MSCP_SERVER
extern struct server servers[];
extern int servers_enabled;
#endif

extern struct QUEUE pollq;

extern int pue_ini_script;
extern int pue_ini_sel;
extern int pue_ini_cmd;
extern int pue_ini_dat_out;
extern int pue_ini_sts_buf;
extern int pue_tgt_script;
extern int pue_tgt_cmd_buf;
extern int pue_tgt_dat_out;
extern int pue_tgt_sts;
extern int pue_scripts;
extern int pue_scripts_size;
extern int pue_patches;
extern int pue_patches_size;
extern int pue_csr_patches;
extern int pue_csr_patches_size;

/*+
 * ============================================================================
 * = pue_init - initialize port driver                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the NCR 53C810.
 *
 * FORM OF CALL:
 *  
 *	pue_init( )
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
#if !( STARTSHUT || DRIVERSHUT )
int pue_init( )
    {
    int pue_init_port( );

    find_pb( "n825_dssi", sizeof( struct n810_pb ), pue_init_port );
    return( msg_success );
    }
#endif

/*+
 * ============================================================================
 * = pue_init_port - initialize a given port                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the port driver data structures and the
 *	port hardware of a specific, given port.
 *  
 * FORM OF CALL:
 *  
 *	pue_init_port( pb )
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
 *	A port block is initialized for each instance of port
 *	hardware of the appropriate type.
 *
-*/
#if ( STARTSHUT || DRIVERSHUT )
void pue_init_port( struct n810_pb *pb, struct kfpsa_blk *blk_ptr )
#else
void pue_init_port( struct n810_pb *pb )
#endif
    {
    char name[32];
    int i;
    int *p;
    int *q;
    int node_id;
    int pue_setmode( );
    int pue_poll_for_interrupt( );
    int pue_interrupt( );
    int pue_send( );
    int pue_receive( );
    int pue_poll( );
#if ( STARTSHUT || DRIVERSHUT )
    int kfpsa_setmode( );
    char cletter[MAX_NAME];
#endif

    node_id = ( 04261537 >> ( ( rb( gpreg ) & 7 ) * 3 ) ) & 7; 
#if ( STARTSHUT || DRIVERSHUT )
    dprintf("pue_init_port pb= %x node_id= %x\n",p_args2(pb,node_id));
#endif
    /*
     *  Hold the SCS lock for the duration.
     */
    krn$_wait( &scs_lock );
    pb->scripts = malloc( &pue_scripts_size, scripts );
    memcpy( pb->scripts, &pue_scripts, &pue_scripts_size );
    p = &pue_patches;
    for( i = 0; i < &pue_patches_size; i++ )
	{
	q = p[i] + pb->scripts;
	*q = pue_phys( *q + pb->scripts );
	}
    p = &pue_csr_patches;
    for( i = 0; i < &pue_csr_patches_size; i++ )
	{
	q = p[i] + pb->scripts;
	*q += pb->pb.csr;
	}
    pb->node_id = node_id;
#if ( STARTSHUT || DRIVERSHUT )
    pb->pb.setmode = kfpsa_setmode;
    pb->pb.pq.routine = pue_interrupt;
#else
    pb->pb.setmode = pue_setmode;
    pb->pb.pq.routine = pue_poll_for_interrupt;
#endif
    pb->pb.pq.param = pb;
    if( pb->pb.vector )
	{
#if ( STARTSHUT || DRIVERSHUT )
	pb->pb.mode = DDB$K_STOP;
#else
	pb->pb.mode = DDB$K_INTERRUPT;
#endif
	pb->pb.desired_mode = DDB$K_INTERRUPT;
	int_vector_set( pb->pb.vector, pue_interrupt, pb );
	io_enable_interrupts( pb, pb->pb.vector );
	}
    else
	{
	pb->pb.mode = DDB$K_POLLED;
	pb->pb.desired_mode = DDB$K_POLLED;
	insq_lock( &pb->pb.pq.flink, &pollq );
	}
#if ( STARTSHUT || DRIVERSHUT )
    sprintf(pb->pb.port_name, "%s%s.%d.%d.%d.%d",
          pb->pb.protocol,
          controller_num_to_id(pb->pb.controller, cletter),
          node_id, 0,
          pb->pb.slot, pb->pb.hose); 
#endif
    pb->pb.protocol = "pu";
    set_io_name( pb->pb.name, 0, 0, node_id, pb );
    set_io_alias( pb->pb.alias, "PI", 0, pb );
    sprintf( pb->pb.info, "DSSI Bus ID %d", node_id );
    pb->pb.sb = malloc( 8 * sizeof( struct pue_sb * ), pb.sb );
    pb->pb.num_sb = 8;
    sprintf( pb->pb.string, "%-24s   %-8s   %24s",
	    pb->pb.name, pb->pb.alias, pb->pb.info );
    krn$_seminit( &pb->isr_t.sem, 0, "pue_isr" );
    krn$_init_timer( &pb->isr_t );
    pb->send_q.flink = &pb->send_q;
    pb->send_q.blink = &pb->send_q;
    pb->receive_q.flink = &pb->receive_q;
    pb->receive_q.blink = &pb->receive_q;
    krn$_seminit( &pb->send_s, 0, "pue_send" );
    krn$_seminit( &pb->receive_s, 0, "pue_receive" );
#if ( STARTSHUT || DRIVERSHUT )
    krn$_seminit( &pb->owner_s, 0, "pue_owner" );
#else
    krn$_seminit( &pb->owner_s, 1, "pue_owner" );
#endif
    pb->owner_l.req_ipl = IPL_SYNC;
    pue_reset( pb );

    dprintf("port %s initialized, scripts are at %x\n", p_args2(pb->pb.name, pb->scripts));

#if ( STARTSHUT || DRIVERSHUT )
    blk_ptr->p_send_pid = krn$_create( pue_send, 0, &blk_ptr->kfpsa_snd,
      6, 0, 4096, prefix( "_send" ), "tt", "r", "tt", "w", "tt", "w", pb );
    blk_ptr->p_receive_pid = krn$_create( pue_receive, 0, &blk_ptr->kfpsa_rec,
      6, 0, 4096, prefix( "_receive" ), "tt", "r", "tt", "w", "tt", "w", pb );
    dprintf("create p_send_pid= %x \ncreate p_receive_pid=%x\n",
      p_args2(blk_ptr->p_send_pid,blk_ptr->p_receive_pid));
#else
    krn$_create( pue_send, 0, 0, 6, 0, 4096,
	    prefix( "_send" ), "nl", "r", "nl", "w", "nl", "w", pb );
    krn$_create( pue_receive, 0, 0, 6, 0, 4096,
	    prefix( "_receive" ), "nl", "r", "nl", "w", "nl", "w", pb );
    krn$_create( pue_poll, 0, 0, 5, 0, 4096,
	    prefix( "_poll" ), "nl", "r", "nl", "w", "nl", "w", pb );
#endif
    /*
     *  Drop the SCS lock, and leave.
     */
    krn$_post( &scs_lock );
}

/*+
 * ============================================================================
 * = pue_reset - reset the port hardware                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine resets the port hardware.  It initializes it completely
 *	so that further operations may take place.
 *  
 * FORM OF CALL:
 *  
 *	pue_reset( pb )
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
void pue_reset( struct n810_pb *pb )
    {
    int i;
    int pue_scs_alloc( );
    int pue_scs_map( );
    int pue_scs_unmap( );
#if MSCP_SERVER
    int pue_ppd_send_sntdat( );
    int pue_ppd_send_reqdat( );
#endif
    int pue_ppd_break_vc( );
    int pue_ppd_send_scs_msg( );
    struct pue_sb *sb;

    pue_init_n810( pb );
    /*
     *  Statically allocate an SB for each possible node connected to this
     *  port.  The N810 supports up to eight attached nodes (the N810 counts
     *  as one but for simplicity we ignore this fact).  Fill in each SB.
     */
    for( i = 0; i < 8; i++ )
	{
	sb = malloc( sizeof( *sb ), sb );
#if ( STARTSHUT || DRIVERSHUT )
        dprintf("sb= %x \n",p_args1(sb));
#endif
	pb->pb.sb[i] = sb;
	sb->pb = pb;
	sb->cb.flink = &sb->cb.flink;
	sb->cb.blink = &sb->cb.flink;
	sb->connect = scs_connect;
#if MSCP_SERVER
	sb->accept = scs_accept;
#endif
	sb->alloc = pue_scs_alloc;
	sb->map = pue_scs_map;
	sb->unmap = pue_scs_unmap;
#if MSCP_SERVER
	sb->send_data = pue_ppd_send_sntdat;
	sb->request_data = pue_ppd_send_reqdat;
#endif
	sb->break_vc = pue_ppd_break_vc;
	sb->send_scs_msg = pue_ppd_send_scs_msg;
	sb->poll_count = 30;
#if ( STARTSHUT || DRIVERSHUT )
	sb->poll_delay = 5;
#else
	sb->poll_delay = 0;
#endif
	sb->novc = 1;
	sb->node_id = i;
	if( pb->node_id == i )
	    sb->local = 1;
#if ( STARTSHUT || DRIVERSHUT )
	krn$_seminit(&sb->ready_s, 0, "dssi_sb_ready");
	krn$_seminit(&sb->disk_s, 0, "disk");
	krn$_seminit(&sb->tape_s, 0, "tape");
#endif 
	set_io_name( sb->name, 0, 0, sb->node_id, pb );
	insert_sb( sb );
	}
}

/*+
 * ============================================================================
 * = pue_init_n810 - initialize the N810 hardware                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the N810 hardware.  It walks through the
 *	necessary steps to bring a DSSI port online.
 *  
 * FORM OF CALL:
 *  
 *	pue_init_n810( pb )
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
void pue_init_n810( struct n810_pb *pb )
{
    int	i;

    /*
     *  Perform necessary register setup.
     */
    n810_init_n810( pb, d_mask, s0_mask, s1_mask );
    wb( sxfer, 0x43 );
    wb( scid, n810_scid_sre | pb->node_id );
    wb( gpreg, 0x10 );
    wb( stest2, n810_stest2_dif | n810_stest2_erf );
    wb( stime0, 0x52 );
}

/*+
 * ============================================================================
 * = pue_bus_reset - reset the DSSI bus                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine resets the DSSI bus.
 *  
 * FORM OF CALL:
 *  
 *	pue_bus_reset( pb )
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
 *	The DSSI bus is reset.
 *
-*/
void pue_bus_reset( struct n810_pb *pb )
    {
    /*
     *  Reset the DSSI bus.
     */
    wb( scntl1, n810_scntl1_rst );
    krn$_micro_delay( 25 );
    wb( scntl1, n810_scntl1_exc );
    /*
     *  Clear internal chip error bits.
     */
    rb( dstat );
    rb( sist0 );
    rb( sist1 );
    }

/*+
 * ============================================================================
 * = pue_scs_alloc - allocate an SCS message packet                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine allocates an SCS message.  It does this by calling the
 *	lower level PPD allocation routine, which allocates any extra bytes
 *	that the lower level PPD routines require, yet it returns a pointer
 *	which hides those extra bytes.
 *  
 * FORM OF CALL:
 *  
 *	pue_scs_alloc( sb, len )
 *  
 * RETURNS:
 *
 *	A pointer to the SCS portion of the allocated packet.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *	int len				- number of SCS bytes to allocate
 *
 * SIDE EFFECTS:
 *
-*/
int pue_scs_alloc( struct pue_sb *sb, int len )
    {
    struct ppd_msg *p;

    /*
     *  Allocate a PPD packet of the right size.  Include the necessary
     *  overhead.
     */
    p = pue_ppd_alloc( sb, sizeof( *p ) + len );
    /*
     *  Return a pointer to the SCS portion, which is just past the PPD
     *  portion.
     */
    return( p+1 );
    }

/*+
 * ============================================================================
 * = pue_scs_map - map a user's buffer for the port hardware to access        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine maps a user's buffer so that the port hardware may
 *	access it, to either copy data out of it or copy data into it.
 *  
 * FORM OF CALL:
 *  
 *	pue_scs_map( cb, buffer, len, p, q )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	unsigned char *buffer		- pointer to user's buffer
 *	int len				- length of user's buffer
 *	int *p				- output area for map information
 *	int *q				- scratch area for map information
 *
 * SIDE EFFECTS:
 *
 *	Mapping resources are allocated.
 *
-*/
void pue_scs_map( struct cb *cb, unsigned char *buffer, int len, int *p, int *q )
    {
    /*
     *  Fill in the output area with the output of the map operation.
     */
    p[0] = buffer;
    p[1] = 0;
    p[2] = cb->dst_id;
    /*
     *  Fill in the scratch area with enough information to be able to
     *  later unmap this buffer.
     */
    q[0] = 0;
    q[1] = 0;
    q[2] = 0;
    q[3] = 0;
    }

/*+
 * ============================================================================
 * = pue_scs_unmap - unmap a user's buffer                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine unmaps a user's buffer.
 *  
 * FORM OF CALL:
 *  
 *	pue_scs_unmap( cb, q )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	int *q				- scratch area for map information
 *
 * SIDE EFFECTS:
 *
 *	Mapping resources are deallocated.
 *
-*/
void pue_scs_unmap( struct cb *cb, int *q )
    {
    /*
     *  We didn't really map, so there's nothing to unmap.
     */
    }

/*+
 * ============================================================================
 * = pue_ppd_alloc - allocate a PPD packet                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine allocates a PPD.  It makes no distinction between
 *	datagram or message, permanent or temporary.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_alloc( sb, len )
 *  
 * RETURNS:
 *
 *	A pointer to the PPD portion of the allocated packet.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *	int len				- number of bytes to allocate
 *
 * SIDE EFFECTS:
 *
-*/
int pue_ppd_alloc( struct pue_sb *sb, int len )
    {
    struct ppd_header *p;
    struct dssi_header *q;
    struct n810_pb *pb;

    pb = sb->pb;
    /*
     *  Allocate a fixed number of bytes, which is the size of the largest
     *  legal PPD packet, plus the required PPD overhead.
     */
    p = malloc( max( ppd_k_max_dg_msg, len ) + sizeof( *p ), p );
    /*
     *  Initialize the DSSI header.  The size reflects the number of bytes
     *  which are transmitted in the Data Out phase; this means that it
     *  includes everything in the PPD header, starting with the opcode, in
     *  addition to the requested length.
     */
    q = &p->dssi_header;
    q->opcode = dssi_k_cmd_dssi;
    q->mbz = 0;
    q->req_ack = 3;
    q->dst_port = sb->node_id;
    q->src_port = pb->node_id;
    q->size = len - offsetof( struct ppd_header, opcode );
    /*
     *  Clear the opcode and flags fields, for simplicity.
     */
    p->opcode = 0;
    p->flags = 0;
    return( p );
    }

/*+
 * ============================================================================
 * = pue_ppd_new_node - perform common processing for a new node              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine is called when a new node is discovered by the poller,
 *	and the sequence to start a VC has completed.  This routine copies
 *	information from the START or STACK packet into the SB.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_new_node( sb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *	struct ppd_dg_start_stack *p	- pointer to PPD START/STACK packet
 *
 * SIDE EFFECTS:
 *
 *	The VC is marked "open", the SB is updated, and MSCP is notified.
 *
-*/
void pue_ppd_new_node( struct pue_sb *sb, struct ppd_dg_start_stack *p )
    {
    int i;

    pue_ppd_open_vc( sb );
    for( i = 0; i < sizeof( p->nodename ); i++ )
	if( p->nodename[i] == ' ' )
	    p->nodename[i] = 0;
    for( i = 0; i < sizeof( p->hwtype ); i++ )
	if( p->hwtype[i] == ' ' )
	    p->hwtype[i] = 0;
    for( i = 0; i < sizeof( p->swtype ); i++ )
	if( p->swtype[i] == ' ' )
	    p->swtype[i] = 0;
    sprintf( sb->alias, "%.8s", p->nodename );
    sprintf( sb->info, "%.4s/%.4s", p->hwtype, p->swtype );
    sprintf( sb->version, "%4.4s", p->swvers );
    sprintf( sb->string, "%-24s   %-16s   %16s  %s",
	    sb->name, sb->alias, sb->info, sb->version );
    memcpy( sb->node_name, p->nodename, sizeof( sb->node_name ) );
    memcpy( sb->system_id, p->systemid, sizeof( sb->system_id ) );
    }

/*+
 * ============================================================================
 * = pue_ppd_open_vc - open a VC                                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine opens a VC.  VC state is held internally and not in any
 *	external hardware.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_open_vc( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
 *	The VC is opened.
 *
-*/
void pue_ppd_open_vc( struct pue_sb *sb )
    {
    sb->vc = 1;
    sb->ns = 0;
    sb->nr = 0;
    sb->poll_count = 0;
    }

/*+
 * ============================================================================
 * = pue_ppd_close_vc - close a VC                                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine closes a VC.  VC state is held internally, and not in
 *	any external hardware.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_close_vc( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
 *	The VC is closed.
 *
-*/
void pue_ppd_close_vc( struct pue_sb *sb )
    {
    sb->vc = 0;
    }

/*+
 * ============================================================================
 * = pue_ppd_send_idreq - send a PPD IDREQ                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD IDREQ command.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_send_idreq( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
-*/
void pue_ppd_send_idreq( struct pue_sb *sb )
    {
    struct ppd_idreq *p;

    p = pue_ppd_alloc( sb, sizeof( *p ) );
    p->opcode = ppd_k_idreq;
    p->xct_id[0] = 1;
    p->xct_id[1] = 1;
    pue_queue_to_send( sb, p );
    sb->poll_in_prog++;
    }

/*+
 * ============================================================================
 * = pue_ppd_send_id - send an ID                                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an ID packet, in response to a received
 *	IDREQ (ID Requested) packet.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_send_id( sb, q )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *	struct ppd_idreq *q		- pointer to PPD IDREQ packet
 *
 * SIDE EFFECTS:
 *
-*/
void pue_ppd_send_id( struct pue_sb *sb, struct ppd_idreq *q )
    {
    struct ppd_id *p;
    struct n810_pb *pb;

    pb = sb->pb;
    p = pue_ppd_alloc( sb, sizeof( *p ) );
    p->opcode = ppd_k_id;
    p->flags |= ppd_k_sp_p0;
    p->xct_id[0] = q->xct_id[0];
    p->xct_id[1] = q->xct_id[1];
    p->rport_typ = 38;
    p->rport_rev = 0;
    p->rport_fcn = 0x83ff0c00;
    p->rst_port = pb->node_id;
    p->rport_state = 4;
    p->mbz = 0;
    p->port_fcn_ext[0] = dssi_k_max_pkt<<16;
    p->port_fcn_ext[1] = 1<<14;
    memset( p->unusedid, 0, sizeof( p->unusedid ) );
    pue_queue_to_send( sb, p );
    }

/*+
 * ============================================================================
 * = pue_ppd_send_cnf - send a CNF                                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a CNF (Confirm) packet, in response to a
 *	received SNTDAT (Sent Data) packet.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_send_cnf( sb, q )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *	struct ppd_sntdat *q		- pointer to PPD SNTDAT packet
 *
 * SIDE EFFECTS:
 *
-*/
void pue_ppd_send_cnf( struct pue_sb *sb, struct ppd_sntdat *q )
    {
    struct ppd_cnf *p;

    p = pue_ppd_alloc( sb, sizeof( *p ) );
    p->opcode = ppd_k_cnf;
    p->xct_id[0] = q->xct_id[0];
    p->xct_id[1] = q->xct_id[1];
    pue_queue_to_send_vc( sb, p );
    }

/*+
 * ============================================================================
 * = pue_ppd_send_dg_start - send a PPD DG command of type START              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD DG command, with the PPD message
 *	type set to START.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_send_dg_start( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
-*/
void pue_ppd_send_dg_start( struct pue_sb *sb )
    {
    pue_ppd_send_dg_start_stack( sb, ppd_k_start );
    }

/*+
 * ============================================================================
 * = pue_ppd_send_dg_stack - send a PPD DG command of type STACK              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD DG command, with the PPD message
 *	type set to STACK.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_send_dg_stack( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
-*/
void pue_ppd_send_dg_stack( struct pue_sb *sb )
    {
    pue_ppd_send_dg_start_stack( sb, ppd_k_stack );
    }

void pue_ppd_send_dg_start_stack( struct pue_sb *sb, int mtype )
    {
    struct ppd_dg_start_stack *p;
    struct n810_pb *pb;
    struct EVNODE *ev;

    pb = sb->pb;
    ev = malloc( sizeof( *ev ), ev );
    p = pue_ppd_alloc( sb, sizeof( *p ) );
    p->opcode = ppd_k_dg;
    p->mtype = mtype;
    ev_read( "scssystemid", &ev, 0 );
    *(long *)&p->systemid[0] = atoi( ev->value.string );
    ev_read( "scssystemidh", &ev, 0 );
    *(short *)&p->systemid[4] = 0;
    p->protocol = 1;
    p->mbz = 0;
    p->maxdg = ppd_k_max_dg_msg;
    p->maxmsg = ppd_k_max_dg_msg;
    memcpy( p->swtype, "CONS", sizeof( p->swtype ) );
    p->swvers[0] = v_variant;
    p->swvers[1] = v_major + '0';
    p->swvers[2] = '.';
    p->swvers[3] = v_minor + '0';
    memset( p->incarn, 0, sizeof( p->incarn ) );
    memcpy( p->hwtype, "EVAX", sizeof( p->hwtype ) );
    memset( p->hwvers, 0, sizeof( p->hwvers ) );
    ev_read( "scsnode", &ev, 0 );
    memcpy( p->nodename, ev->value.string, sizeof( p->nodename ) );
    memset( p->curtime, 0, sizeof( p->curtime ) );
    free( ev, ev );
    pue_queue_to_send( sb, p );
    }

/*+
 * ============================================================================
 * = pue_ppd_send_dg_ack - send a PPD DG command of type ACK                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD DG command, with the PPD message
 *	type set to ACK.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_send_dg_ack( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
-*/
void pue_ppd_send_dg_ack( struct pue_sb *sb )
    {
    struct ppd_dg_ack *p;

    p = pue_ppd_alloc( sb, sizeof( *p ) );
    p->opcode = ppd_k_dg;
    p->mtype = ppd_k_ack;
    pue_queue_to_send( sb, p );
    }

/*+
 * ============================================================================
 * = pue_ppd_send_scs_msg - send a PPD MSG command of type SCS_MSG            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD MSG command, with the PPD message
 *	type set to SCS_MSG.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_send_scs_msg( sb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *	struct ppd_msg *p		- pointer to message to send
 *
 * SIDE EFFECTS:
 *
-*/
void pue_ppd_send_scs_msg( struct pue_sb *sb, struct ppd_msg *p )
    {
    --p;
    p->opcode = ppd_k_msg;
    p->mtype = ppd_k_scs_msg;
    pue_queue_to_send_vc( sb, p );
    }

#if MSCP_SERVER
/*+
 * ============================================================================
 * = pue_ppd_send_sntdat - send a PPD SNTDAT                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD SNTDAT command.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_send_sntdat( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *	struct SEMAPHORE *sem		- semaphore to be posted
 *	int len				- transaction length
 *	int *snd			- pointer to sender's name/offset
 *	int *rec			- pointer to receiver's name/offset
 *
 * SIDE EFFECTS:
 *
-*/
void pue_ppd_send_sntdat( struct pue_sb *sb, struct SEMAPHORE *sem,
	int len, int *snd, int *rec )
    {
    int t;
    int offset;
    struct ppd_sntdat *p;

    if( len > 65536 )
	{
	dqprintf("required SNTDAT too large (%d) on %s\n", len, sb->name );
	pue_ppd_break_vc( sb );
	}
    offset = 0;
    while( 1 )
	{
	t = min( len, 4096 );
	p = pue_ppd_alloc( sb, sizeof( *p ) + t );
	p->opcode = ppd_k_sntdat;
	if( t == len )
	    p->flags |= ppd_k_lp;
	p->xct_id[0] = 1;
	p->xct_id[1] = sem;
	p->name = rec[1];
	p->offset = rec[0] + offset;
	memcpy( p+1, snd[0] + offset, t );
	pue_queue_to_send_vc( sb, p );
	offset += t;
	if( offset == len )
	    break;
	}
    }

/*+
 * ============================================================================
 * = pue_ppd_send_reqdat - send a PPD REQDAT                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD REQDAT command.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_send_reqdat( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *	struct SEMAPHORE *sem		- semaphore to be posted
 *	int len				- transaction length
 *	int *snd			- pointer to sender's name/offset
 *	int *rec			- pointer to receiver's name/offset
 *
 * SIDE EFFECTS:
 *
-*/
void pue_ppd_send_reqdat( struct pue_sb *sb, struct SEMAPHORE *sem,
	int len, int *snd, int *rec )
    {
    struct ppd_datreq *p;

    p = pue_ppd_alloc( sb, sizeof( *p ) );
    p->opcode = ppd_k_datreq0;
    p->xct_id[0] = 1;
    p->xct_id[1] = sem;
    p->xct_len = len;
    p->snd_name = snd[1];
    p->snd_offset = snd[0];
    p->rec_name = rec[1];
    p->rec_offset = rec[0];
    pue_queue_to_send_vc( sb, p );
    }
#endif

/*+
 * ============================================================================
 * = pue_ppd_break_vc - tear down a VC                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles VC shutdown.  It disables further VC formation
 *	until this (lengthy) process is complete.  Since this routine is called
 *	from tasks which potentially cannot afford to block, it cannot do
 *	anything itself which might block, and neither can the connection error
 *	routine associated with each CB.  When the VC is completely run down
 *	(each CB has been torn down) a new VC is allowed to form.  A free poll
 *	is performed to help this happen more rapidly.
 *  
 * FORM OF CALL:
 *  
 *	pue_ppd_break_vc( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
 *	The VC is closed.  CBs are run down.
 *
-*/
void pue_ppd_break_vc( struct pue_sb *sb )
    {
    struct cb *cb;
    struct cb *next_cb;

    if( sb->state != sb_k_open )
	return;
    sb->state = sb_k_stall;
    sb->novc = 1;
    pue_ppd_close_vc( sb );
    dprintf("breaking VC to %s\n", p_args1(sb->name));
    /*
     *  Rip through the list of CBs.  Say the connection is closed.  Post the
     *  two semaphores associated with this CB, in case there are waiters.
     *  Call the connection error routine associated with this CB.  When the
     *  CB's reference count reaches zero, the CB can be discarded.
     */
    for( cb = sb->cb.flink; cb != &sb->cb.flink; cb = next_cb )
	{
	next_cb = cb->cb.flink;
	cb->state = cb_k_closed;
	krn$_bpost( &cb->control_s );
	krn$_bpost( &cb->credit_s );
	cb->connection_error( cb );
	}
    /*
     *  All CBs have been run down.  Allow a new VC to form.
     */
    sb->state = sb_k_closed;
    sb->poll_count = 30;
    sb->poll_delay = 0;
    pue_ppd_send_idreq( sb );
    }

/*+
 * ============================================================================
 * = pue_queue_to_send - queue a command to the port                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine gives a PPD command to the port to be executed.  The
 *	command is inserted onto the send queue in the PB.
 *  
 * FORM OF CALL:
 *  
 *	pue_queue_to_send( sb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *	struct ppd_header *p		- pointer to PPD packet
 *
 * SIDE EFFECTS:
 *
 *	A PPD packet is queued onto the send queue, and the send semaphore
 *	is posted.
 *
-*/
void pue_queue_to_send( struct pue_sb *sb, struct ppd_header *p )
    {
    struct n810_pb *pb;

    pb = sb->pb;
    insq( p, pb->send_q.blink );
    krn$_post( &pb->send_s );
    }

/*+
 * ============================================================================
 * = pue_queue_to_send_vc - queue a command to the port with VC checking      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine gives a PPD command to the port to be executed.  If the
 *	VC is open, then the send sequence number is computed and put into
 *	the packet, and the command is inserted onto the send queue in the PB;
 *	if no VC is open, the command is discarded.
 *  
 * FORM OF CALL:
 *  
 *	pue_queue_to_send_vc( sb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *	struct ppd_header *p		- pointer to PPD packet
 *
 * SIDE EFFECTS:
 *
 *	A PPD packet is queued onto the send queue, and the send semaphore
 *	is posted.
 *
-*/
void pue_queue_to_send_vc( struct pue_sb *sb, struct ppd_header *p )
    {
    struct n810_pb *pb;

    pb = sb->pb;
    if( sb->vc )
	{
	p->flags |= ( sb->ns << 1 );
	sb->ns = ( sb->ns + 1 ) & 7;
	insq( p, pb->send_q.blink );
	krn$_post( &pb->send_s );
	}
    else
        free( p, p );
    }

/*+
 * ============================================================================
 * = pue_send - process packets from the command queue                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine processes port commands.  It is notified when a new
 *	command exists on the command queue, and it removes and processes
 *	that new command.
 *  
 * FORM OF CALL:
 *  
 *	pue_send( pb )
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
 *	The command queue is periodically emptied.
 *
-*/
void pue_send( struct n810_pb *pb )
    {
    int t;
    struct pue_sb *sb;
    struct ppd_header *p;
#if ( STARTSHUT || DRIVERSHUT )
    struct PCB *pcb;

    pcb = getpcb( );
#endif
    while( 1 )
	{
	krn$_wait( &pb->send_s );
#if ( STARTSHUT || DRIVERSHUT )
	if (pcb->pcb$l_killpending & SIGNAL_KILL) {
	    dprintf("pue_send killed\n", p_args0);
            return;
	}
#endif
	krn$_wait( &scs_lock );
	p = pb->send_q.flink;
	sb = pb->pb.sb[p->dssi_header.dst_port];
	if( sb->node_id == pb->node_id )
	    t = pue_response( sb, p );
	else
	    t = pue_command( sb, p );
	if( t != msg_success )
	    if( sb->state == sb_k_open )
		{
		dqprintf("transmit failed (%s)\n", sb->name );
		pue_ppd_break_vc( sb );
		}
	if( p->opcode == ppd_k_idreq )
	    --sb->poll_in_prog;
	remq( p );
        free( p, p );
	krn$_post( &scs_lock );
	}
    }

/*+
 * ============================================================================
 * = pue_command - send a DSSI command to a remote node                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine sends a DSSI command to a remote node, allowing 1
 *	second for the remote node to complete the command.  Since only
 *	one command may be in progress at a time, the PB owner semaphore
 *	is used as a hardware lock.
 *  
 * FORM OF CALL:
 *  
 *	pue_command( sb, p )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_failure			- failed to send the command
 *
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *	struct ppd_header *p		- pointer to PPD packet to send
 *
 * SIDE EFFECTS:
 *
 *	A command may be sent to a DSSI device.
 *
-*/
int pue_command( struct pue_sb *sb, struct ppd_header *p )
    {
    int retry;
    int t;
    struct n810_pb *pb;

    /*
     *  Prepare to retry the command; the retry count is large, except in the
     *  case of sending an IDREQ to a node with a closed VC.
     */
    pb = sb->pb;
    if( ( p->opcode == ppd_k_idreq ) && !sb->vc )
	retry = 2;
    else
	retry = 200;
    /*
     *  Send the command.
     */
    krn$_post( &scs_lock );
    while( --retry >= 0 )
	{
	/*
	 *  Acquire ownership of the port hardware, and start a timer to deal
	 *  with selection timeouts.
     	 */
	krn$_wait( &pb->owner_s );
	krn$_start_timer( &pb->isr_t, 1000 );
	t = pue_do_send( sb, p );
	/*
	 *  Stop the timer, release ownership of the port hardware, and return.
	 */
	krn$_stop_timer( &pb->isr_t );
	krn$_post( &pb->owner_s );
	if( t == msg_success )
	    break;
	if( t == msg_selected )
	    {
	    retry++;
	    krn$_sleep( 1 );
	    }
	else
	    krn$_sleep( 20 );
	}
    krn$_wait( &scs_lock );
    return( t );
    }

/*+
 * ============================================================================
 * = pue_receive - process packets from the response queue                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine processes port responses.  It is notified when a new
 *	response exists on the response queue, and it removes and processes
 *	that new response.
 *  
 * FORM OF CALL:
 *  
 *	pue_receive( pb )
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
 *	The response queue is periodically emptied.
 *
-*/
void pue_receive( struct n810_pb *pb )
    {
    int t;
    struct ppd_header *p;
    struct pue_sb *sb;
#if ( STARTSHUT || DRIVERSHUT )
    struct PCB *pcb;

    pcb = getpcb( );
#endif
    p = malloc( sizeof( *p ) + dssi_k_max_pkt, p );
    while( 1 )
	{
	/*
	 *  Wait for the ISR to post the receive semaphore.  This indicates
	 *  that a new response is available.
	 */
	krn$_wait( &pb->receive_s );
#if ( STARTSHUT || DRIVERSHUT )
	if (pcb->pcb$l_killpending & SIGNAL_KILL) {
	    free(p, p);
	    dprintf("pue_rec killed\n", p_args0);
            return;
	}
#endif
	krn$_wait( &pb->owner_s );
	krn$_start_timer( &pb->isr_t, 1000 );
	t = pue_do_receive( pb, p );
	krn$_stop_timer( &pb->isr_t );
	krn$_post( &pb->owner_s );
	if( t == msg_success )
	    {
	    krn$_wait( &scs_lock );
	    sb = pb->pb.sb[p->dssi_header.src_port];
	    pue_response( sb, p );
	    krn$_post( &scs_lock );
	    }
	}
    }

/*+
 * ============================================================================
 * = pue_response - process a port response                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine processes a single port response, fetched from the
 *	response queue.  The response could be generated by port execution
 *	of a command, or by port reception of a packet.  If the response
 *	indicates that an error occurred, the affected VC is closed.
 *  
 * FORM OF CALL:
 *  
 *	pue_response( sb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *	struct ppd_header *p		- pointer to PPD packet to receive
 *
 * SIDE EFFECTS:
 *
 *	A VC may be closed.  A VC may be opened.  Lots of other possibilities
 *	exist.
 *
-*/
void pue_response( struct pue_sb *sb, struct ppd_header *p )
    {
    switch( p->opcode )
	{
	case ppd_k_dg:
	    {
	    /*
	     *  This is a received datagram.  Dispatch on the PPD message
	     *  type field.  Any that we don't care about are ignored
	     *  (they are datagrams, after all).
	     */
	    struct ppd_dg *q = p;

	    switch( q->mtype )
		{
		case ppd_k_start:
		    /*
		     *  Upon receiving a START, we close any existing VC,
		     *  send back a STACK, and open a new VC.
		     */
		    switch( sb->state )
			{
			case sb_k_closed:
			    if( sb->novc )
				break;
			case sb_k_start_out:
			case sb_k_start_in:
			    sb->state = sb_k_start_in;
			    pue_ppd_send_dg_stack( sb );
			    pue_ppd_new_node( sb, q );
			    break;

			case sb_k_open:
			    dprintf("requested to restart VC on %s\n", p_args1(sb->name ));
			    pue_ppd_break_vc( sb );
			    break;
			}
		    break;

		case ppd_k_stack:
		    /*
		     *  Upon receiving a STACK, we send back an ACK, and
		     *  open the VC.
		     */
		    switch( sb->state )
			{
			case sb_k_start_out:
			case sb_k_start_in:
			    sb->state = sb_k_open;
			    pue_ppd_send_dg_ack( sb );
			    pue_ppd_new_node( sb, q );
			    mscp_new_node( sb );
			    break;

			case sb_k_open:
			    pue_ppd_send_dg_ack( sb );
			    break;
			}
		    break;

		case ppd_k_ack:
		    /*
		     *  Upon receiving an ACK, we tell MSCP that it's time
		     *  to go.
		     */
		    switch( sb->state )
			{
			case sb_k_start_in:
			    sb->state = sb_k_open;
			    mscp_new_node( sb );
			    break;
			}
		    break;
		}
	    break;
	    }

	case ppd_k_msg:
	    {
	    /*
	     *  This is a received message.  Dispatch on the PPD message
	     *  type field if and only if a VC is open.
	     */
	    struct ppd_msg *q = p;

	    if( pue_ppd_check_vc( sb, p ) != msg_success )
		break;
	    switch( q->mtype )
		{
		case ppd_k_scs_msg:
		    /*
		     *  If we are waiting for an ACK, then receipt of an
		     *  SCS message is suffient cause to assume that the ACK
		     *  was lost and will never be received, so pretend that
		     *  it did in fact come in.
		     */
		    if( sb->state == sb_k_start_in )
			{
			sb->state = sb_k_open;
			mscp_new_node( sb );
			}
		    /*
		     *  A received SCS message is passed up to a higher
		     *  layer to process.
		     */
		    scs_receive( sb, q+1, p->dssi_header.size - 2 );
		    break;

		default:
		    /*
		     *  Unknown message types are fatal to the VC.
		     */
		    dqprintf("unrecognized PPD message received\n" );
		    pue_ppd_break_vc( sb );
		    break;
		}
	    break;
	    }

#if MSCP_SERVER
	case ppd_k_cnf:
	    {
	    /*
	     *  This is a confirmation of data sent.  Process it if and
	     *  only if a VC is open.
	     */
	    struct ppd_cnf *q = p;

	    if( pue_ppd_check_vc( sb, p ) != msg_success )
		break;
	    krn$_post( q->xct_id[1] );
	    break;
	    }
#endif

	case ppd_k_idreq:
	    /*
	     *  This is a request to send back an ID packet.
	     */
	    pue_ppd_send_id( sb, p );
	    break;

	case ppd_k_datreq0:
	case ppd_k_datreq1:
	case ppd_k_datreq2:
	    {
	    /*
	     *  This is a request for data.  Send back as much as was asked
	     *  for.
	     */
	    int t;
	    int offset;
	    int length;
	    struct ppd_datreq *q = p;
	    struct ppd_retdat *r;

	    if( pue_ppd_check_vc( sb, p ) != msg_success )
		break;
	    if( q->snd_name != 0 )
		{
		dqprintf( "corrupted DATREQ received\n" );
		pue_ppd_break_vc( sb );
		break;
		}
	    if( q->xct_len > 65536 )
		{
		dqprintf( "required RETDAT too large (%d) on %s\n", q->xct_len, sb->name );
		pue_ppd_break_vc( sb );
		break;
		}
	    offset = 0;
	    length = q->xct_len;
	    while( 1 )
		{
		t = min( length, 4096 );
		r = pue_ppd_alloc( sb, sizeof( *r ) + t );
		r->opcode = ppd_k_retdat;
		if( t == length )
		    r->flags |= ppd_k_lp;
		r->xct_id[0] = q->xct_id[0];
		r->xct_id[1] = q->xct_id[1];
		r->name = q->rec_name;
		r->offset = q->rec_offset + offset;
		memcpy( r+1, q->snd_offset + offset, t );
		pue_queue_to_send_vc( sb, r );
		if( t == length )
		    break;
		offset += t;
		length -= t;
		}
	    break;
	    }

	case ppd_k_id:
	    {
	    /*
	     *  This is a received ID.  If no VC is open, and VC formation
	     *  is not disabled, then try to open a VC.
	     */
	    struct ppd_id *q = p;

	    sb->port_type = q->rport_typ & 0x7fffffff;

#if MODULAR
/* EF51R maint ID = 0x60, EF52R maint ID = 0x61,
 * EF53R 0x62, EF54R 0x63, EF58R 0x64 */

	    sb->dssi_disk =
		(((sb->port_type >= 0x60) && (sb->port_type <= 0x64)) ||
		((sb->port_type >= 48) && (sb->port_type <= 63)));
	    sb->dssi_tape =
		((sb->port_type >= 64) && (sb->port_type <= 79));
#endif
	    switch( sb->state )
		{
		case sb_k_closed:
#if MSCP_SERVER
		    sb->novc = ( sb->port_type < 48 )
			    && ( sb->port_type != 4 )
			    && !servers_enabled;
#else
		    sb->novc = ( sb->port_type < 48 )
			    && ( sb->port_type != 4 );
#endif
		    if( sb->novc )
			break;
		case sb_k_start_out:
		case sb_k_start_in:
		    sb->state = sb_k_start_out;
		    pue_ppd_send_dg_start( sb );
		    break;
		}
	    break;
	    }

	case ppd_k_sntdat:
	    {
	    /*
	     *  This is sent data (unsolicited).  Just accept it.
	     */
	    struct ppd_sntdat *q = p;

	    if( pue_ppd_check_vc( sb, p ) != msg_success )
		break;
	    if( q->name != 0 )
		{
		dqprintf( "corrupted SNTDAT received\n" );
		pue_ppd_break_vc( sb );
		break;
		}
	    memcpy( q->offset, q+1, p->dssi_header.size - 18 );
	    if( q->flags & 1 )
		pue_ppd_send_cnf( sb, q );
	    break;
	    }

#if MSCP_SERVER
	case ppd_k_retdat:
	    {
	    /*
	     *  This is returned data (solicited).  Just accept it.
	     */
	    struct ppd_retdat *q = p;

	    if( pue_ppd_check_vc( sb, p ) != msg_success )
		break;
	    if( q->name != 0 )
		{
		dqprintf( "corrupted RETDAT received\n" );
		pue_ppd_break_vc( sb );
		break;
		}
	    memcpy( q->offset, q+1, p->dssi_header.size - 18 );
	    if( q->flags & 1 )
		krn$_post( q->xct_id[1] );
	    break;
	    }
#endif

	default:
	    /*
	     *  This is definitely bad news.
	     */
	    dqprintf( "unrecognized opcode %x, packet = %x\n",
		    p->opcode, p );
	}
    return;
    }

int pue_ppd_check_vc( struct pue_sb *sb, struct ppd_header *p )
    {
    int e_nr;
    int r_nr;

    if( !sb->vc )
	return( msg_failure );
    e_nr = sb->nr;
    r_nr = ( p->flags >> 1 ) & 7;
    if( e_nr != r_nr )
	{
	if( e_nr != ( ( r_nr + 1 ) & 7 ) )
	    {
	    dprintf( "out of sequence %s from %s, e_nr = %d, r_nr = %d\n",
		    p_args4(pue_opcodes[p->opcode&0x1f], sb->name, e_nr, r_nr ));
	    pue_ppd_break_vc( sb );
	    }
	return( msg_failure );
	}
    sb->nr = ( e_nr + 1 ) & 7;
    return( msg_success );
    }

/*+
 * ============================================================================
 * = pue_poll - poll to see which nodes exist                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine performs polling, in order to get an accurate picture
 *	of which nodes exist and which don't.  Polling periodically is a way
 *	of detecting nodes coming and going.  Polling is done more frequently
 *	in the beginning, so that nodes may be found quickly, and less
 *	frequently thereafter, so that not much bandwidth is used.  This
 *	routine is careful never to poll the port's own node number.
 *  
 * FORM OF CALL:
 *  
 *	pue_poll( pb )
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
void pue_poll( struct n810_pb *pb )
    {
    char name[32];
    int i;
    struct pue_sb *sb;

    /*
     *  Visit each SB.
     */
    for( i = 0; i < 8; i++ ) 
	{
	sb = pb->pb.sb[i];
	/*
	 *  Don't poll if we're stopped -- it just depletes the heap.
	 */
	if( pb->pb.mode == DDB$K_STOP )
	    continue;
	if( sb->poll_in_prog )
	    continue;
	/*
	 *  If the delay time is up, then go ahead and poll.
	 */
	if( sb->poll_delay == 0 )
	    {
	    krn$_wait( &scs_lock );
	    pue_ppd_send_idreq( sb );
	    krn$_post( &scs_lock );
	    /*
	     *  Reset the delay time, depending on the remaining "fast
	     *  poll" count.  If there are more "fast polls" left, then
	     *  set a short delay; otherwise, set a long delay.
	     */
	    if( sb->poll_count == 0 )
		sb->poll_delay = 10; 
	    else
		--sb->poll_count;
	    }
	else
	    --sb->poll_delay;
	}
    krn$_create_delayed( 1000, 0, pue_poll, 0, 0, 5, 0, 4096,
	    prefix( "_poll" ), pb );
    }

/*+
 * ============================================================================
 * = pue_do_send - build the script to send a DSSI command                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds the script to send a DSSI command to a remote
 *	node.
 *  
 * FORM OF CALL:
 *  
 *	pue_do_send( sb, p )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_failure			- failed to send the command
 *	msg_selected			- selected by another node
 *
 * ARGUMENTS:
 *
 *	struct pue_sb *sb		- system block
 *	struct ppd_header *p		- pointer to PPD command to send
 *
 * SIDE EFFECTS:
 *
 *	A command may be sent to a DSSI device.
 *
-*/
int pue_do_send( struct pue_sb *sb, struct ppd_header *p )
    {
    unsigned char *ini_sts;
    unsigned char dstat;
    unsigned char sist0;
    unsigned char sist1;
    struct n810_pb *pb;
    struct dssi_header *q;
    struct n810_bm *bm;
    struct n810_io *io;

    pb = sb->pb;
    q = &p->dssi_header;

    dprintf( "attempting to send to %s\n", p_args1(sb->name ));
    /*
     *  Fill in the address and length fields of the pre-built DSSI script.
     */
    io = pue_virt( &pue_ini_sel );
    io->io_id = q->dst_port;
    bm = pue_virt( &pue_ini_cmd );
    bm->bm_addr = pue_phys( q );
    bm = pue_virt( &pue_ini_dat_out );
    bm->bm_byte_count = q->size;
    bm->bm_addr = pue_phys( q+1 );
    ini_sts = pue_virt( &pue_ini_sts_buf );
    *ini_sts = 0;
    /*
     *  Prepare for an interrupt to occur, and hand the script to the N810.
     */
    spinlock( &pb->owner_l );
    dstat = pb->dstat;
    sist0 = pb->sist0;
    sist1 = pb->sist1;
    if( pb->selected )
	{
	spinunlock( &pb->owner_l );
	return( msg_selected );
	}
    pb->int_pending = 1;
    pb->isr_t.sem.count = 0;
    pb->dstat = 0;
    pb->sist0 = 0;
    pb->sist1 = 0;
    wl( dsp, pue_phys( pue_virt( &pue_ini_script ) ) );
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
		dqprintf( "command timeout on %s, dstat = %02x, sist0 = %02x, sist1 = %02x\n",
			sb->name, dstat, sist0, sist1 );
	    pue_init_n810( pb );
	    pue_bus_reset( pb );
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
	 *  If we've been selected, then signal the current script to stop.
	 */
	if( pb->selected && !( dstat || sist0 || sist1 ) )
	    wb( istat, n810_istat_sigp );
	/*
	 *  If the status byte has been written, then we have transmitted
	 *  the entire packet, so return success or failure based on the
	 *  status byte's value.
	 */
	if( *ini_sts )
	    {
	    if( *ini_sts == dssi_k_sts_good )
		{
		dprintf( "sent %s (length %d) to node %d...\n",
			p_args3(pue_opcodes[p->opcode&0x1f], q->size, q->dst_port ));
		return( msg_success );
		}
	    return( msg_failure );
	    }
	/*
	 *  See if we have been selected; if so, say so.
	 */
	if( dstat & n810_dstat_sir )
	    if( rl( dsps ) == n810_k_reselected )
		return( msg_selected );
	/*
	 *  If we are connected to the bus, clean up.
	 */
	if( rb( scntl1 ) & n810_scntl1_con )
	    {
	    pue_init_n810( pb );
	    pue_bus_reset( pb );
	    }
	return( msg_failure );
	}
    }

/*+
 * ============================================================================
 * = pue_do_receive - build the script to receive a DSSI command              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds the script to receive a DSSI command from a remote
 *	node.
 *  
 * FORM OF CALL:
 *  
 *	pue_do_receive( pb, p )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_failure			- failed to receive the command
 *
 * ARGUMENTS:
 *
 *	struct pb *pb			- port block
 *	struct ppd_header *p		- pointer to PPD command to receive
 *
 * SIDE EFFECTS:
 *
 *	A command may be received from a DSSI device.
 *
-*/
int pue_do_receive( struct n810_pb *pb, struct ppd_header *p )
    {
    unsigned char dstat;
    unsigned char sist0;
    unsigned char sist1;
    struct dssi_header *q;
    struct n810_bm *bm;

    pb->selected = 0;
    if( !( rb( scntl1 ) & n810_scntl1_con ) )
	return( msg_failure );
    q = &p->dssi_header;
    /*
     *  Fill in the address and length fields of the pre-built DSSI script.
     */
    bm = pue_virt( &pue_tgt_dat_out );
    bm->bm_addr = pue_phys( q+1 );
    /*
     *  Prepare for an interrupt to occur, and hand the script to the N810.
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
    wb( scratcha, 0 );
    wl( dsp, pue_phys( pue_virt( &pue_tgt_script ) ) );
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
	    pue_init_n810( pb );
	    pue_bus_reset( pb );
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
	 *  If SCRATCHA contains "success", then we have received the entire
	 *  packet, so accept it, regardless of interrupting conditions.
	 */
	if( rb( scratcha ) == dssi_k_sts_good )
	    {
	    memcpy( q, pue_virt( &pue_tgt_cmd_buf ), 6 );
	    if( pue_verify_header( pb, q ) != msg_success )
		return( msg_failure );
	    dprintf( "received %s (length %d) from node %d...\n",
		    p_args3(pue_opcodes[p->opcode&0x1f], q->size, q->src_port ));
	    return( msg_success );
	    }
	/*
	 *  If we are connected to the bus, clean up.
	 */
	if( rb( scntl1 ) & n810_scntl1_con )
	    {
	    pue_init_n810( pb );
	    pue_bus_reset( pb );
	    }
	return( msg_failure );
	}
    }

int pue_verify_header( struct n810_pb *pb, struct dssi_header *q )
    {
    if( ( q->opcode != dssi_k_cmd_dssi )
	    || ( q->mbz != 0 )
	    || ( ( q->req_ack != 3 ) && ( q->req_ack != 7 ) )
	    || ( q->dst_port != pb->node_id )
	    || ( q->src_port == pb->node_id )
	    || ( ( q->src_port & ~7 ) != 0 )
	    || ( q->size > dssi_k_max_pkt ) )
	{
	dqprintf( "bad packet received on port %s, header = %08x%04x\n",
		pb->pb.name, *(unsigned int *)((int)q+2), *(unsigned short *)((int)q+0) );
	return( msg_failure );
	}
    return( msg_success );
    }

/*+
 * ============================================================================
 * = pue_setmode - stop or start the port driver                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine deals with requests to stop or start the port driver.
 *  
 * FORM OF CALL:
 *  
 *	pue_setmode( pb, mode )
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
void pue_setmode( struct n810_pb *pb, int mode )
    {
    int i;

    switch( mode )
	{
	case DDB$K_STOP:
	    if( pb->pb.mode != DDB$K_STOP )
		{
		krn$_wait( &pb->owner_s );
		wb( istat, n810_istat_rst );
		wb( istat, 0 );
		if( pb->pb.mode == DDB$K_INTERRUPT )
		    io_disable_interrupts( pb, pb->pb.vector );
		else
		    remq_lock( &pb->pb.pq.flink );
		pb->pb.mode = DDB$K_STOP;
		}
	    else
		{
		wb( istat, n810_istat_rst );
		wb( istat, 0 );
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
		pue_init_n810( pb );
		krn$_post( &pb->owner_s );
		krn$_wait( &scs_lock );
		for( i = 0; i < 8; i++ )
		    pue_ppd_break_vc( pb->pb.sb[i] );
		krn$_post( &scs_lock );
		}
	    break;

	case DDB$K_READY:
	    if( pb->pb.mode != DDB$K_STOP )
		for( i = 0; i < 8; i++ )
		    {
		    krn$_wait( &scs_lock );
		    pue_ppd_send_idreq( pb->pb.sb[i] );
		    krn$_post( &scs_lock );
		    }
	    break;
	}
    }

/*+
 * ============================================================================
 * = pue_poll_for_interrupt - poll for port interrupts                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine polls for port interrupts.  If an interrupt is pending,
 *	the ISR is called.
 *  
 * FORM OF CALL:
 *  
 *	pue_poll_for_interrupt( pb )
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
void pue_poll_for_interrupt( struct n810_pb *pb )
    {
    if( pb->pb.mode == DDB$K_POLLED )
	if( rb( istat ) & 3 )
	    pue_interrupt( pb );
    }

/*+
 * ============================================================================
 * = pue_interrupt - service a port interrupt                                 =
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
 *	pue_interrupt( pb )
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
void pue_interrupt( struct n810_pb *pb )
    {
    unsigned char dstat;
    unsigned char sist0;
    unsigned char sist1;
    int t;

    spinlock( &pb->owner_l );
    sist0 = rb( sist0 ) & s0_mask;
    sist1 = rb( sist1 ) & s1_mask;
    dstat = rb( dstat ) & d_mask;
    /*
     *  If anything interesting happened, tell whoever's waiting.
     */
    if( sist0 & n810_sist0_sel )
	{
	sist0 &= ~n810_sist0_sel;
	pb->selected = 1;
	krn$_post( &pb->receive_s );
	}
    if( dstat || sist0 || sist1 || pb->selected )
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

#if ( STARTSHUT )
/*+
 * ============================================================================
 * = kfpsa_setmode - change driver state				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine changes the state of the port driver. The following
 *	states are relevant:
 *
 *	DDB$K_STOP -	Suspends the port driver.  Interrupts from the
 *			device are dismissed.
 *
 *	DDB$K_RESTARTABLE -
 *			Same as DDB$K_STOP, except the port driver may be
 *			restarted dynamically by the class driver.
 *
 *	DDB$K_START -	Changes the port driver to interrupt mode.
 *			device interrupt handling is once again enabled.
 *
 *	DDB$K_INTERRUPT -
 *			Same as DDB$K_START.
 *
 *	DDB$K_ASSIGN  -	Assigns controller device letters.
 *
 * FORM OF CALL:
 *
 *	kfpsa_setmode(mode)
 *
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *	msg_port_state_running - port running
 *
 * ARGUMENTS:
 *
 *	int mode - Desired mode for port driver.
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int kfpsa_setmode(int mode, struct device *dev)
{
    int i, j, status;
    int kfpsa_process();
    struct pue_sb *sb;
    KFPSA_PB *pb;
    struct kfpsa_blk *blk_ptr;
    struct PCB *pcb;

    switch (mode) {

	case DDB$K_START: 
	case DDB$K_INTERRUPT:
                
	    blk_ptr = (void *) dev->devdep.io_device.devspecific;
                
/* if blk_ptr already setup, device is already initialized */

	    if (blk_ptr != 0) {
		blk_ptr->refcnt++;
		dprintf("Setmode: already running\n", p_args0);
		return msg_success;
	    }

/* initialize class drivers */

	    ovly_load("MSCP");
	    if (!mscp_shared_adr)
		return msg_failure;

	    /* Start DSSI */

	    dprintf("KFPSA setmode - start\n", p_args0);

	    blk_ptr = malloc(sizeof(struct kfpsa_blk), kfpsa_blk);

	    dev->devdep.io_device.devspecific = (void *) blk_ptr;

	    krn$_seminit(&blk_ptr->kfpsa_i, 0, "kfpsa_i");
	    krn$_seminit(&blk_ptr->kfpsa_s, 0, "kfpsa_s");
	    krn$_seminit(&blk_ptr->kfpsa_process, 0, "kfpsa_process");
	    krn$_seminit(&blk_ptr->kfpsa_snd, 0, "kfpsa_snd");
	    krn$_seminit(&blk_ptr->kfpsa_rec, 0, "kfpsa_rec");

	    blk_ptr->refcnt = 1;	/* initial start */

/* Call kernel routine here to make sure IPL is lowered: */

	    krn$_set_console_mode(INTERRUPT_MODE);

	    /* create process for adapter */

	    blk_ptr->pid = krn$_create(	/* */
	      kfpsa_process, 		/* address of process   */
	      null_procedure, 		/* startup routine      */
	      &blk_ptr->kfpsa_process, 	/* completion semaphore */
	      6, 			/* process priority     */
	      1 << whoami(), 		/* cpu affinity mask    */
	      4096, 			/* stack size           */
	      "kfpsa", 			/* process name         */
	      0, 0, 0, 0, 0, 0, 	/* i/o                  */
	      blk_ptr, dev);

	    dprintf("Created kfpsa_process: pid %x\n", p_args1(blk_ptr->pid));

	    krn$_wait(&blk_ptr->kfpsa_i);

	    if (blk_ptr->status != msg_success) {
		err_printf("KFPSA setmode - start, failed, status = %m\n",
		  blk_ptr->status);
		return blk_ptr->status;
	    }

/* set Datagram Queue Inhibit for all ports */

	    dprintf("KFPSA setmode - start, complete\n", p_args0);
	    break;

	case DDB$K_STOP: 

	    blk_ptr = (void *) dev->devdep.io_device.devspecific;

	    if (blk_ptr == 0) {
		dprintf("KFPSA setmode - stop, blk_ptr is null..\n", p_args0);
		return msg_failure;
	    }

	    if (blk_ptr->refcnt == 0)
		return msg_success;

	    blk_ptr->refcnt--;
	    if (blk_ptr->refcnt != 0)
		return msg_success;

	    pb = blk_ptr->pb;  
	    pb->n810pb.pb.mode = DDB$K_STOP;

	    dprintf("KFPSA setmode - stop, pb= %x\n",
	      p_args1(pb));

            /* Currently this is not needed. But it might be someday.
	       When shutdown_port is called, it causes a longer delay
	       to get a prompt after the devices are listed . */

	    kfpsa_shutdown_port(pb);

	    krn$_post(&pb->reset_s);  /* new njc */

	    /* Stop the adapter  */
	    krn$_wait( &pb->n810pb.owner_s );
	    wb( istat, n810_istat_rst );
	    wb( istat, 0 );
	    io_disable_interrupts( pb, pb->n810pb.pb.vector );

	    dprintf("mscp destroy poll dev= %x\n", p_args1(dev));
	    mscp_destroy_poll(dev);

	    krn$_wait(&scs_lock);	/* grab scs_lock to synchronize */

	    dprintf("kill processes\n", p_args0);
                
	    /* free system blocks */
	    sb = &pb->sb_pool;

	    for (i = 0; i < pb->n810pb.pb.num_sb; i++, sb++) {
		sb = pb->n810pb.pb.sb[i];
		dprintf("release ready_S disk_s tape_s  sb= %x\n",
		      p_args1(sb));
		krn$_semrelease(&sb->ready_s);
		krn$_semrelease(&sb->disk_s);
		krn$_semrelease(&sb->tape_s);   
		remove_sb(sb);
		free(sb,sb);
	     }

	    free(pb->sb_pool, sb_pool);
	    free(pb->sb_ptr_pool, sb_ptr_pool);
	    free(pb->n810pb.pb.sb, pb.sb);

            free(pb->n810pb.scripts, n810pb.scripts);   
                              
	    dprintf("delete pid= %x\n",p_args1(blk_ptr->p_send_pid));
	    krn$_delete(blk_ptr->p_send_pid);
	    krn$_post(&pb->n810pb.send_s);
	    krn$_wait(&blk_ptr->kfpsa_snd);
            krn$_semrelease(&blk_ptr->kfpsa_snd);

	    dprintf("delete pid= %x\n",p_args1(blk_ptr->p_receive_pid));
	    krn$_delete(blk_ptr->p_receive_pid);
	    krn$_post(&pb->n810pb.receive_s);
	    krn$_wait(&blk_ptr->kfpsa_rec);
            krn$_semrelease(&blk_ptr->kfpsa_rec);

            krn$_semrelease(&pb->n810pb.send_s);
	    krn$_semrelease(&pb->n810pb.receive_s);
            krn$_semrelease(&pb->n810pb.owner_s);

	    krn$_semrelease(&pb->reset_s);
	    krn$_semrelease(&pb->init_s);
	    krn$_semrelease(&pb->adap_s);
                                 
	    dprintf("delete pid= %x\n",p_args1(blk_ptr->pid));
	    krn$_delete(blk_ptr->pid);
	    krn$_post(&blk_ptr->kfpsa_s);
	    krn$_wait(&blk_ptr->kfpsa_process);

	    krn$_semrelease(&blk_ptr->kfpsa_i);
	    krn$_semrelease(&blk_ptr->kfpsa_s);
	    krn$_semrelease(&blk_ptr->kfpsa_process);

	    krn$_stop_timer(&pb->n810pb.isr_t);
            krn$_release_timer(&pb->n810pb.isr_t );
	    krn$_semrelease(&pb->n810pb.isr_t.sem);

	    dprintf("clear vector\n", p_args0);
	    int_vector_clear(pb->n810pb.pb.vector); 

	    free(pb, pb);
	    free(blk_ptr, blk_ptr);  
	    dev->devdep.io_device.devspecific = (void *) 0;
	    dev->devdep.io_device.vector[0] = (void *) 0;
	    dev->devdep.io_device.devspecific2 = (void *) 0;

	    krn$_post(&scs_lock);	/* release scs_lock */
	    break;

       case DDB$K_READY:   

	    blk_ptr = (void *) dev->devdep.io_device.devspecific;
	    pb = blk_ptr->pb;  

            dprintf("received READY mode is %d\n",p_args1(pb->n810pb.pb.mode));
	    if( pb->n810pb.pb.mode != DDB$K_STOP )
		for( i = 0; i < 8; i++ )
		    {
		    krn$_wait( &scs_lock );
		    pue_ppd_send_idreq( pb->n810pb.pb.sb[i] );
		    krn$_post( &scs_lock );
		    }
	    break;

	case DDB$K_ASSIGN: 
	    break;

	case DDB$K_POLLED: 
	default: 
	    err_printf("KFPSA setmode error - illegal mode.\n");
	    return msg_failure;
	    break;
    }					/* switch */
    return msg_success;
}


/*+
 * ============================================================================
 * = kfpsa_process - initialize the KFPSA and process response queue entries  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the KFPSA and then
 *	handles the Response queue insertion interrupt by removing
 *	and processing entries found on the Adapter Driver Response Queue.  
 *  
 * FORM OF CALL:
 *  
 *	kfpsa_process( blk_ptr, dev )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct kfpsa_blk *blk_ptr - block pointer
 *	struct device *dev - device pointer
 *
 * SIDE EFFECTS:
 *
 *	Dequeues responses on adapter output queues.
 *
-*/

void kfpsa_process(struct kfpsa_blk *blk_ptr, struct device *dev)
{
    int status, i;
    KFPSA_PB *pb;
    struct PCB *pcb;
    DSSI_STATUS dssi_status;
    int reset_flag, st;

/* 
 * Initialize the DSSI adapter 
 */
    status = kfpsa_init_port(blk_ptr, dev);   /* setup port */
    blk_ptr->status = status;

    if (status != msg_success) {
        dprintf("failed kfpsa_init_port\n",p_args0);
	krn$_post(&blk_ptr->kfpsa_i);
	return;
    }

    pb = blk_ptr->pb;
    pcb = getpcb();

    krn$_post( &pb->n810pb.owner_s );

/* If MSCP poll has not been started, then do it now */

    dprintf("kfpsa process - mscp create poll\n", p_args0);
    mscp_create_poll();

    krn$_post(&blk_ptr->kfpsa_i);

    while (1) {

/* Wait for adapter interrupt */

	krn$_wait(&blk_ptr->kfpsa_s);

	if (pcb->pcb$l_killpending & SIGNAL_KILL) {
	    dprintf("kfpsa_process killed\n", p_args0);
            return;
	}
                                  
    }					/* while( 1 ) outer loop */
}
 

/*
 * ============================================================================
 * = kfpsa_init_port - initialize a given port                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the port driver data structures and the
 *	port hardware of a specific, given port.
 *  
 * FORM OF CALL:
 *  
 *	kfpsa_init_port( blk_ptr, dev )
 *  
 * RETURNS:
 *
 *	msg_success or failure code
 *       
 * ARGUMENTS:
 *
 *	struct kfpsa_blk *blk_ptr    - adapter relevent data.
 *	struct device   *dev	    - pointer to this device
 *
 * SIDE EFFECTS:
 *
 *	A port block is allocated and initialized for each instance of port
 *	hardware of the appropriate type.
 *
-*/
int kfpsa_init_port(struct kfpsa_blk *blk_ptr, struct device *dev)
{
    int i, st, t;
    int controller;
    int hose, slot;
    int node, bus, csr, vect1;
    int kfpsa_send_count = 0;
    int kfpsa_receive_count = 0;
    unsigned int base0, base1;
    struct kfpsa_pb *psa_pb;
    char name[80];
    int kfpsa_setmode();
    int pue_interrupt();

    dprintf("kfpsa_init_port  blk_ptr= %x\n", p_args1(blk_ptr));

/*
 * Allocate and initialize the Port Block;
 */ 
    blk_ptr->pb = psa_pb = malloc(sizeof(struct kfpsa_pb), kfpsa_pb);

    dev->devdep.io_device.vector[0] = pci_get_vector(dev);
    dev->devdep.io_device.devspecific = blk_ptr;
    dev->devdep.io_device.devspecific2 = (int)psa_pb;

    node = blk_ptr->pb->n810pb.node_id = dev->node; 
    vect1 = blk_ptr->vect1 = dev->devdep.io_device.vector[0];
    blk_ptr->pb->n810pb.pb.vector = vect1;
    
    blk_ptr->pb->n810pb.pb.setmode = kfpsa_setmode;
    blk_ptr->pb->n810pb.pb.pq.routine = pue_interrupt; 
    blk_ptr->pb->n810pb.pb.pq.param = psa_pb;

dprintf("dev->controller= %x\n",p_args1(dev->devdep.io_device.controller));

    blk_ptr->pb->n810pb.pb.slot = slot = dev->slot;
    blk_ptr->pb->n810pb.pb.hose = hose = dev->hose; 
    blk_ptr->pb->n810pb.pb.bus = bus = dev->bus;
    blk_ptr->pb->n810pb.pb.function = dev->function;
    blk_ptr->pb->n810pb.pb.channel = 0;
    blk_ptr->pb->n810pb.pb.controller = dev->devdep.io_device.controller;
    blk_ptr->pb->n810pb.pb.protocol = "pu";
    blk_ptr->pb->n810pb.pb.config_device = dev;

    base0 = incfgl(&psa_pb->n810pb.pb, BASE0) & 0xFFFFFFFE;
    base1 = incfgl(&psa_pb->n810pb.pb, BASE1) & 0xFFFFFFFE;
    blk_ptr->pb->n810pb.pb.csr = csr = base1;


    psa_pb->adap_response_c = 0;
    psa_pb->sb_pool = malloc(SB_POOL_SZ, sb_pool);
    psa_pb->sb_ptr_pool = malloc(SB_PTR_POOL_SZ, sb_ptr_pool);
    psa_pb->state = Unknown;
    
    krn$_seminit(&psa_pb->reset_s, 0, "kfpsa_bus_reset");
    krn$_seminit(&psa_pb->init_s, 1, "kfpsa_initialize");
    krn$_seminit(&psa_pb->adap_s, 1, "kfpsa_adapter");

    dprintf("KFPSA hose/slot/ctl/vect/base %d/%d/%d/%x/%x\n",
       p_args5(hose,slot,dev->devdep.io_device.controller,vect1,base1));

    pue_init_port(psa_pb, blk_ptr);

    return (msg_success);
}                            /* kfpsa_init_port() */



int kfpsa_establish_connection(struct INODE *inode, struct device *dev, int class)
{
    int unit;
    int node;
    int channel;
    int i;
    struct kfpsa_blk *blk_ptr;
    struct pue_sb *sb;
    KFPSA_PB *pb;

    dprintf("establish_connection dev=%x\n", p_args1(dev));

/* diagnostic mode, node already configured */

    if (diagnostic_mode_flag)
	return msg_success;  

/* if device not in file system, fail */

    if (!inode->inuse) {
	return msg_failure;
    }

/* get device particulars */

    if (explode_devname(inode->name, 0, 0, &unit, &node, &channel, 0, 0) !=
      msg_success) {
	boot_retry_counter = 0;
	return msg_failure;
    }

/* get device pointers */

    blk_ptr = (void *) dev->devdep.io_device.devspecific;

/* do an IDREQ to given node */

    pb = blk_ptr->pb;
    sb = pb->n810pb.pb.sb[node];

    dprintf("target pb= %x\nref= %x\n",p_args2(pb,sb->ref));

/* if sb in use, device already configured */

    if (sb->ref != 0)
	return msg_success;

    sb->ref++;

    sb->pdevice = 0;
    sb->verbose = 0;

    sb->poll_active = 1;		/* waiting for IDREQ to be returned */

    krn$_wait(&scs_lock);

/* do an IDREQ to given node */

    dprintf("targe sb= %x, node= %x\n", p_args2(sb, node));

    pue_ppd_send_idreq(sb);

    krn$_post(&scs_lock);

/* wait for node to connect, no path or timeout */

    sb->poll_delay = 5;

    while (sb->poll_delay != 0) {

	if (sb->poll_active != 1) {
	    break;
	} else {
	    if (sb->state == sb_k_closed) {
		krn$_wait(&scs_lock);
		pue_ppd_send_idreq(sb);
		krn$_post(&scs_lock);
	    }
	    if (sb->state == sb_k_start_out) {
		krn$_wait(&scs_lock);
		pue_ppd_send_dg_start(sb);
		krn$_post(&scs_lock);
	    }
	}
	sb->poll_delay--;
	krn$_sleep(1000);		/* 1 sec */
    }

    if (sb->poll_delay == 0) {
	dprintf("delay is zero \n",p_args0);
	return msg_failure;
    }

    if (sb->poll_active != 0) {
        dprintf("not active\n",p_args0);
	return msg_failure;
    }

/* poll for units */

    dprintf("call mscp_poll_units\n",p_args0);
    mscp_poll_units(dev);
    return msg_success;
}

int kfpsa_poll_flag = 1;
int kfpsa_poll_units(struct device *dev, int verbose)
{
    int status;
    struct kfpsa_pb *pb;
    struct kfpsa_blk *blk_ptr;

    dprintf("KFPSA poll_units\n", p_args0);
    blk_ptr = (void *) dev->devdep.io_device.devspecific;
              
    /* do an IDREQ to each node */
    pb = blk_ptr->pb;
    dprintf("call kfpsa_poll_new   pb= %x\n",p_args1(pb));
    kfpsa_poll_new(pb, TRUE);

    dssi_wait(blk_ptr);

    /* poll for units */
    dprintf("call mscp_poll_units  dev= %x \n",p_args1(dev));
    mscp_poll_units(dev);

    dprintf("KFPSA poll_units: complete\n", p_args0);
    return msg_success;
}
#endif

#if ( STARTSHUT || DRIVERSHUT )
void kfpsa_shutdown_port(struct n810_pb *pb)
{
    int i;
    struct pue_sb *sb;
    int status;

    dprintf("shutdown_port...\n", p_args0);

    /* 
     * Visit each SB. 
     **/

    for (i = 0; i < pb->pb.num_sb; i++) {

        if (i == pb->node_id)
            continue;                   /* Don't shutdown ourselves. */

        sb = pb->pb.sb[i];
        if (!sb) {
            dprintf("No sb to %x\n",p_args1(i));
  
           continue;                   /* no system block */
        }

        if (!sb->vc) {
            dprintf("No VC to %x \n",p_args1(i));
            continue;                   /* no vc established */
        } 

        dprintf("shutdown_port: break vc node %d\n", p_args1(i));

/*       
**  Send_active is reset upon receipt of the cktset command from the
**  shutdown of the circuit with this sb.
*/
        sb->send_active = 1;
        dprintf("call pue_ppd_shutdown_vc(%x)\n",p_args1(sb));
        pue_ppd_break_vc( pb->pb.sb[i] );

/* wait for all sent packets to be returned */
        sb->poll_delay = 5;            /* 7.3 sec ( 15 * 500) */
        while (sb->poll_delay != 0) {
            if (sb->send_active == 0){
                dprintf("sb->send_active went to 0\n",p_args0);
                break;
            }
            sb->poll_delay--;
            krn$_sleep(500);
        }

        if (sb->poll_delay == 0)
            dprintf("KFPSA shutdown send timeout %d\n", p_args1(i));

        dprintf("shutdown_port: complete node %d\n", p_args1(i));

    }

    dprintf("shutdown_port: complete\n", p_args0);
}

void dssi_wait(struct kfpsa_blk *blk_ptr)
{
    int status0;

    while (1) {

	status0 = kfpsa_wait_port(blk_ptr->pb);

	if (status0 == msg_success)
	    break;

	krn$_sleep(1000);		/* 1 sec */
    }
}

/*
 * kfpsa_wait_port
 *	use sb->poll_active to indicate polling condition
 *	  0 = ok
 *	  1 = waiting for IDREQ packet to be returned or connect
 *	  2 = no path to node
 *	  3 = poll timeout reported
 */
int kfpsa_wait_port(struct kfpsa_pb *pb)
{
    int i, j, k;
    struct pue_sb *sb;

/* Visit each SB. */

	j = 0;
	for (i = 0; i < 8; i++) {

	    sb = pb->n810pb.pb.sb[i];

	    if (i == pb->n810pb.node_id) {
		j++;
		continue;
	    }

	    if (sb->poll_active != 1)	/* completed */
		j++;

	    if (sb->poll_active == 1) {	/* still waiting */

/* If the delay time is up, then mark node bad */

		if (sb->poll_delay != 0) {
		    --sb->poll_delay;
		    if (sb->state == sb_k_closed) {
			krn$_wait(&scs_lock);
			pue_ppd_send_idreq(sb);
			krn$_post(&scs_lock);
		    }
		    if (sb->state == sb_k_start_out) {
			krn$_wait(&scs_lock);
			pue_ppd_send_dg_start(sb);
			krn$_post(&scs_lock);
		    }
		} else {
		    if (sb->poll_active == 1) {
			sb->poll_active = 3;
			j++;
		    }
		}
	    }
	}				/* for (i) */

    return msg_success;
}


void kfpsa_poll_new(struct kfpsa_pb *pb, int verbose)
{
    int i, status, k;
    struct pue_sb *sb;
    u_char alias [8];
    u_char info [32];
    u_char cletter[MAX_NAME];


	if (verbose) {
	    sprintf(info, "              Bus ID %d", pb->n810pb.node_id);
	    sprintf(alias, "du%s", controller_num_to_id(pb->n810pb.pb.controller, cletter));
	    printf("%-18s %-7s %s\n", pb->n810pb.pb.port_name, alias, info);
	}
   

	/* Visit each SB. */
	for (i = 0; i < pb->n810pb.pb.num_sb; i++) {

	    sb = pb->n810pb.pb.sb[i];

	    if (i == pb->n810pb.node_id) {
		i++;
		continue;
	    }

	    sb->pdevice = 1;
	    sb->verbose = verbose;

	    sb->poll_active = 1;	/* waiting for IDREQ to be returned */

	    krn$_wait(&scs_lock);

	    pue_ppd_send_idreq(sb);

	    krn$_post(&scs_lock);

	    while (1) {
		status = kfpsa_wait_new(sb);
		if (status == msg_success)
		    break;
		krn$_sleep(1000);		/* 1 sec */
	    }
	}
    
}

int kfpsa_wait_new(struct pue_sb *sb)
{
    if (sb->poll_active != 1)	/* completed */
	return msg_success;

    if (sb->poll_active == 1) {	/* still waiting */

/* If the delay time is up, then mark node bad */

	if (sb->poll_delay != 0) {
	    --sb->poll_delay;
	    if (sb->state == sb_k_closed) {
		krn$_wait(&scs_lock);
		pue_ppd_send_idreq(sb);
		krn$_post(&scs_lock);
	    }
	    if (sb->state == sb_k_start_out) {
		krn$_wait(&scs_lock);
		pue_ppd_send_dg_start(sb);
		krn$_post(&scs_lock);
	    }
	} else {
	    if (sb->poll_active == 1) {
		sb->poll_active = 3;
		return msg_success;
	    }
	}
    }
    return msg_failure;
}
#endif

#if ( DRIVERSHUT )
/*+
 * ============================================================================
 * = kfpsa_setmode - change driver state				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine changes the state of the port driver. The following
 *	states are relevant:
 *
 *	DDB$K_STOP -	Suspends the port driver.  Interrupts from the
 *			device are dismissed.
 *
 *	DDB$K_RESTARTABLE -
 *			Same as DDB$K_STOP, except the port driver may be
 *			restarted dynamically by the class driver.
 *
 *	DDB$K_START -	Changes the port driver to interrupt mode.
 *			device interrupt handling is once again enabled.
 *
 *	DDB$K_INTERRUPT -
 *			Same as DDB$K_START.
 *
 *	DDB$K_ASSIGN  -	Assigns controller device letters.
 *
 * FORM OF CALL:
 *
 *	kfpsa_setmode(mode)
 *
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *	msg_port_state_running - port running
 *
 * ARGUMENTS:
 *
 *	int mode - Desired mode for port driver.
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int kfpsa_setmode(int mode, struct pb *gpb)
{
    int i, j, status;
    int kfpsa_process();
    struct pue_sb *sb;
    KFPSA_PB *pb;
    struct kfpsa_blk *blk_ptr;
    struct PCB *pcb;

    switch (mode) {

	case DDB$K_START: 
	case DDB$K_INTERRUPT:
                
	    blk_ptr = gpb->config_device;
                
/* if blk_ptr already setup, device is already initialized */

	    if (blk_ptr != 0) {
		blk_ptr->refcnt++;
		dprintf("Setmode: already running\n", p_args0);
		return msg_success;
	    }

/* initialize class drivers */

	    ovly_load("MSCP");
	    if (!mscp_shared_adr)
		return msg_failure;

	    /* Start DSSI */

	    dprintf("KFPSA setmode - start\n", p_args0);

	    blk_ptr = malloc(sizeof(struct kfpsa_blk), kfpsa_blk);

	    gpb->config_device = (void *) blk_ptr;

	    krn$_seminit(&blk_ptr->kfpsa_i, 0, "kfpsa_i");
	    krn$_seminit(&blk_ptr->kfpsa_s, 0, "kfpsa_s");
	    krn$_seminit(&blk_ptr->kfpsa_process, 0, "kfpsa_process");
	    krn$_seminit(&blk_ptr->kfpsa_snd, 0, "kfpsa_snd");
	    krn$_seminit(&blk_ptr->kfpsa_rec, 0, "kfpsa_rec");

	    blk_ptr->refcnt = 1;	/* initial start */

/* Call kernel routine here to make sure IPL is lowered: */

	    krn$_set_console_mode(INTERRUPT_MODE);

	    /* create process for adapter */

	    blk_ptr->pid = krn$_create(	/* */
	      kfpsa_process, 		/* address of process   */
	      null_procedure, 		/* startup routine      */
	      &blk_ptr->kfpsa_process, 	/* completion semaphore */
	      6, 			/* process priority     */
	      1 << whoami(), 		/* cpu affinity mask    */
	      4096, 			/* stack size           */
	      "kfpsa", 			/* process name         */
	      0, 0, 0, 0, 0, 0, 	/* i/o                  */
	      blk_ptr, gpb);

	    dprintf("Created kfpsa_process: pid %x\n", p_args1(blk_ptr->pid));

	    krn$_wait(&blk_ptr->kfpsa_i);

	    if (blk_ptr->status != msg_success) {
		err_printf("KFPSA setmode - start, failed, status = %m\n",
		  blk_ptr->status);
		return blk_ptr->status;
	    }

/* set Datagram Queue Inhibit for all ports */

	    dprintf("KFPSA setmode - start, complete\n", p_args0);
	    break;

	case DDB$K_STOP: 

	    blk_ptr = (void *) gpb->config_device;

	    if (blk_ptr == 0) {
		dprintf("KFPSA setmode - stop, blk_ptr is null..\n", p_args0);
		return msg_failure;
	    }

	    if (blk_ptr->refcnt == 0)
		return msg_success;

	    blk_ptr->refcnt--;
	    if (blk_ptr->refcnt != 0)
		return msg_success;

	    pb = blk_ptr->pb;  
	    pb->n810pb.pb.mode = DDB$K_STOP;

	    dprintf("KFPSA setmode - stop, pb= %x\n",
	      p_args1(pb));

            /* Currently this is not needed. But it might be someday.
	       When shutdown_port is called, it causes a longer delay
	       to get a prompt after the devices are listed . */

	    kfpsa_shutdown_port(pb);

	    krn$_post(&pb->reset_s);  /* new njc */

	    /* Stop the adapter  */
	    krn$_wait( &pb->n810pb.owner_s );
	    wb( istat, n810_istat_rst );
	    wb( istat, 0 );
	    io_disable_interrupts( pb, pb->n810pb.pb.vector );

	    dprintf("mscp destroy poll pb= %x\n", p_args1(pb));
	    mscp_destroy_poll( pb );

	    krn$_wait(&scs_lock);	/* grab scs_lock to synchronize */

	    dprintf("kill processes\n", p_args0);
                
	    /* free system blocks */
	    sb = &pb->sb_pool;

	    for (i = 0; i < pb->n810pb.pb.num_sb; i++, sb++) {
		sb = pb->n810pb.pb.sb[i];
		dprintf("release ready_S disk_s tape_s  sb= %x\n",
		      p_args1(sb));
		krn$_semrelease(&sb->ready_s);
		krn$_semrelease(&sb->disk_s);
		krn$_semrelease(&sb->tape_s);   
		remove_sb(sb);
		free(sb,sb);
	     }

	    free(pb->sb_pool, sb_pool);
	    free(pb->sb_ptr_pool, sb_ptr_pool);
	    free(pb->n810pb.pb.sb, pb.sb);

            free(pb->n810pb.scripts, n810pb.scripts);   
                              

	    dprintf("delete pid= %x\n",p_args1(blk_ptr->p_send_pid));
	    krn$_delete(blk_ptr->p_send_pid);
	    krn$_post(&pb->n810pb.send_s);
	    krn$_wait(&blk_ptr->kfpsa_snd);
            krn$_semrelease(&blk_ptr->kfpsa_snd);

	    dprintf("delete pid= %x\n",p_args1(blk_ptr->p_receive_pid));
	    krn$_delete(blk_ptr->p_receive_pid);
	    krn$_post(&pb->n810pb.receive_s);
	    krn$_wait(&blk_ptr->kfpsa_rec);
            krn$_semrelease(&blk_ptr->kfpsa_rec);

            krn$_semrelease(&pb->n810pb.send_s);
	    krn$_semrelease(&pb->n810pb.receive_s);
            krn$_semrelease(&pb->n810pb.owner_s);

	    krn$_semrelease(&pb->reset_s);
	    krn$_semrelease(&pb->init_s);
	    krn$_semrelease(&pb->adap_s);
                                 
	    dprintf("delete pid= %x\n",p_args1(blk_ptr->pid));
	    krn$_delete(blk_ptr->pid);
	    krn$_post(&blk_ptr->kfpsa_s);
	    krn$_wait(&blk_ptr->kfpsa_process);

	    krn$_semrelease(&blk_ptr->kfpsa_i);
	    krn$_semrelease(&blk_ptr->kfpsa_s);
	    krn$_semrelease(&blk_ptr->kfpsa_process);

	    krn$_stop_timer(&pb->n810pb.isr_t);
            krn$_release_timer(&pb->n810pb.isr_t );
	    krn$_semrelease(&pb->n810pb.isr_t.sem);

	    dprintf("clear vector\n", p_args0);
	    int_vector_clear(pb->n810pb.pb.vector); 

	    free(pb, pb);
	    free(blk_ptr, blk_ptr);  
	    gpb->config_device = 0;
	    gpb->dpb = 0;
	    krn$_post(&scs_lock);	/* release scs_lock */
	    break;

       case DDB$K_READY:   

            dprintf("received READY mode is %d\n",p_args1(pb->n810pb.pb.mode));
	    if( pb->n810pb.pb.mode != DDB$K_STOP )
		for( i = 0; i < 8; i++ )
		    {
		    krn$_wait( &scs_lock );
		    pue_ppd_send_idreq( pb->n810pb.pb.sb[i] );
		    krn$_post( &scs_lock );
		    }
	    break;

	case DDB$K_ASSIGN: 
	    break;

	case DDB$K_POLLED: 
	default: 
	    err_printf("KFPSA setmode error - illegal mode.\n");
	    return msg_failure;
	    break;
    }					/* switch */
    return msg_success;
}


/*+
 * ============================================================================
 * = kfpsa_process - initialize the KFPSA and process response queue entries  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the KFPSA and then
 *	handles the Response queue insertion interrupt by removing
 *	and processing entries found on the Adapter Driver Response Queue.  
 *  
 * FORM OF CALL:
 *  
 *	kfpsa_process( blk_ptr, pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct kfpsa_blk *blk_ptr - block pointer
 *	struct pb *pb - pb pointer
 *
 * SIDE EFFECTS:
 *
 *	Dequeues responses on adapter output queues.
 *
-*/

void kfpsa_process(struct kfpsa_blk *blk_ptr, struct pb *gpb)
{
    int status, i;
    KFPSA_PB *pb;
    struct PCB *pcb;
    DSSI_STATUS dssi_status;
    int reset_flag, st;

/* 
 * Initialize the DSSI adapter 
 */
    status = kfpsa_init_port(blk_ptr, gpb);   /* setup port */
    blk_ptr->status = status;

    if (status != msg_success) {
        dprintf("failed kfpsa_init_port\n",p_args0);
	krn$_post(&blk_ptr->kfpsa_i);
	return;
    }

    pb = blk_ptr->pb;
    pcb = getpcb();

    krn$_post( &pb->n810pb.owner_s );

/* If MSCP poll has not been started, then do it now */

    dprintf("kfpsa process - mscp create poll\n", p_args0);
    mscp_create_poll();

    krn$_post(&blk_ptr->kfpsa_i);

    while (1) {

/* Wait for adapter interrupt */

	krn$_wait(&blk_ptr->kfpsa_s);

	if (pcb->pcb$l_killpending & SIGNAL_KILL) {
	    dprintf("kfpsa_process killed\n", p_args0);
            return;
	}
                                  
    }					/* while( 1 ) outer loop */
}
 

/*
 * ============================================================================
 * = kfpsa_init_port - initialize a given port                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the port driver data structures and the
 *	port hardware of a specific, given port.
 *  
 * FORM OF CALL:
 *  
 *	kfpsa_init_port( blk_ptr, pb )
 *  
 * RETURNS:
 *
 *	msg_success or failure code
 *       
 * ARGUMENTS:
 *
 *	struct kfpsa_blk *blk_ptr    - adapter relevent data.
 *	struct pb   *pb		     - pointer to this pb
 *
 * SIDE EFFECTS:
 *
 *	A port block is allocated and initialized for each instance of port
 *	hardware of the appropriate type.
 *
-*/
int kfpsa_init_port(struct kfpsa_blk *blk_ptr, struct pb *gpb)
{
    int i, st, t;
    int kfpsa_send_count = 0;
    int kfpsa_receive_count = 0;
    unsigned int base1;
    struct kfpsa_pb *pb;
    char name[80];
    int kfpsa_setmode();
    int pue_interrupt();

    dprintf("kfpsa_init_port  blk_ptr= %x\n", p_args1(blk_ptr));

/*
 * Allocate and initialize the Port Block;
 */ 

    if ( gpb->dpb ) {
	gpb->dpb->ref++;
	return msg_success;
    } else {
	alloc_dpb( gpb, &pb, sizeof( struct kfpsa_pb ));
    }

    blk_ptr->pb = pb;

    pb->n810pb.pb.csr = incfgl(&pb->n810pb.pb, BASE1) & 0xFFFFFFFE;

    pb->adap_response_c = 0;
    pb->sb_pool = malloc(SB_POOL_SZ, sb_pool);
    pb->sb_ptr_pool = malloc(SB_PTR_POOL_SZ, sb_ptr_pool);
    pb->state = Unknown;
    
    krn$_seminit(&pb->reset_s, 0, "kfpsa_bus_reset");
    krn$_seminit(&pb->init_s, 1, "kfpsa_initialize");
    krn$_seminit(&pb->adap_s, 1, "kfpsa_adapter");

    dprintf("KFPSA hose/slot/ctl/vect/base %d/%d/%d/%x/%x\n",
       p_args5(gpb->hose,gpb->slot,gpb->controller,gpb->vector,pb->n810pb.pb.csr));

    pue_init_port(pb, blk_ptr);

    return (msg_success);
}                            /* kfpsa_init_port() */



int kfpsa_establish_connection(struct INODE *inode, struct pb *gpb, int class)
{
    int unit;
    int node;
    int channel;
    int i;
    struct kfpsa_blk *blk_ptr;
    struct pue_sb *sb;
    KFPSA_PB *pb;

    dprintf("establish_connection pb=%x\n", p_args1(pb));

/* diagnostic mode, node already configured */

    if (diagnostic_mode_flag)
	return msg_success;  

/* if device not in file system, fail */

    if (!inode->inuse) {
	return msg_failure;
    }

/* get device particulars */

    if (explode_devname(inode->name, 0, 0, &unit, &node, &channel, 0, 0, 0, 0) !=
      msg_success) {
	boot_retry_counter = 0;
	return msg_failure;
    }

/* get device pointers */

    blk_ptr = (void *) gpb->config_device;

/* do an IDREQ to given node */

    pb = blk_ptr->pb;
    sb = pb->n810pb.pb.sb[node];

    dprintf("target pb= %x\nref= %x\n",p_args2(pb,sb->ref));

/* if sb in use, device already configured */

    if (sb->ref != 0)
	return msg_success;

    sb->ref++;

    sb->pdevice = 0;
    sb->verbose = 0;

    sb->poll_active = 1;		/* waiting for IDREQ to be returned */

    krn$_wait(&scs_lock);

/* do an IDREQ to given node */

    dprintf("targe sb= %x, node= %x\n", p_args2(sb, node));

    pue_ppd_send_idreq(sb);

    krn$_post(&scs_lock);

/* wait for node to connect, no path or timeout */

    sb->poll_delay = 5;

    while (sb->poll_delay != 0) {

	if (sb->poll_active != 1) {
	    break;
	} else {
	    if (sb->state == sb_k_closed) {
		krn$_wait(&scs_lock);
		pue_ppd_send_idreq(sb);
		krn$_post(&scs_lock);
	    }
	    if (sb->state == sb_k_start_out) {
		krn$_wait(&scs_lock);
		pue_ppd_send_dg_start(sb);
		krn$_post(&scs_lock);
	    }
	}
	sb->poll_delay--;
	krn$_sleep(1000);		/* 1 sec */
    }

    if (sb->poll_delay == 0) {
	dprintf("delay is zero \n",p_args0);
	return msg_failure;
    }

    if (sb->poll_active != 0) {
        dprintf("not active\n",p_args0);
	return msg_failure;
    }

/* poll for units */

    dprintf("call mscp_poll_units\n",p_args0);
    mscp_poll_units( pb );
    return msg_success;
}

int kfpsa_poll_flag = 1;
int kfpsa_poll_units(struct pb *gpb, int verbose)
{
    int status;
    struct kfpsa_pb *pb;
    struct kfpsa_blk *blk_ptr;

    dprintf("KFPSA poll_units\n", p_args0);
    blk_ptr = (void *) gpb->config_device;
              
    /* do an IDREQ to each node */
    pb = blk_ptr->pb;
    dprintf("call kfpsa_poll_new   pb= %x\n",p_args1(pb));
    kfpsa_poll_new(pb, TRUE);

    dssi_wait(blk_ptr);

    /* poll for units */
    dprintf("call mscp_poll_units  pb= %x \n",p_args1(pb));
    mscp_poll_units( pb );

    dprintf("KFPSA poll_units: complete\n", p_args0);
    return msg_success;
}
#endif

#if DEBUG_MALLOC
int kfpsa_malloc(int size, char *sym)
{
    register addr;

    addr = dyn$_malloc(size, DYN$K_SYNC|DYN$K_NOWAIT|DYN$K_NOOWN);
    if ((kfpsa_debug_flag & 2) == 2)
	printf("malloc: %s %x,%d\n", sym, addr, size);
    if (addr == 0) {
	err_printf("KFPSA FATAL ERROR: malloc %s %d\n", sym, size);
	sysfault(0);
    }
    return addr;
}

void kfpsa_free(int adr, char *sym)
{
    if ((kfpsa_debug_flag & 2) == 2)
	printf("free: %s %x\n", sym, adr);
    dyn$_free(adr, DYN$K_SYNC);
}
#endif

#if DEBUG
void _dprintf(char *fmt, protoargs int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
    if ((kfpsa_debug_flag & 1) == 1)
        printf(fmt, a, b, c, d, e, f, g, h, i, j);
}
void _d2printf(char *fmt, protoargs int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
    if ((kfpsa_debug_flag & 2) == 2)
        printf(fmt, a, b, c, d, e, f, g, h, i, j);
}
void _dqprintf(char *fmt, protoargs int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
    if (kfpsa_debug_flag)
	_dprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    else
	qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
}
#endif

