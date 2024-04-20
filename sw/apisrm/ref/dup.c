/*
 * Copyright (C) 1990 by
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
 * Abstract:	DUP class driver
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	cbf	27-Jan-1993	add arg to read_with_prompt for echo functn.
 *
 *	pel	06-Mar-1992	add arg to satisfy new read_with_prompt functn.
 *
 *	ajb	08-Nov-1990	Change calling sequence to krn$_create
 *
 *	sfs	14-Aug-1990	Complete command timeout work.
 *
 *	ajb	13-Aug-1990	Add parameter to read_with_prompt for timeout.
 *
 *	sfs	20-Jun-1990	Initial entry.
 */

#define DEBUG 0

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:ctype.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:dup_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:cb_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:print_var.h"
#if STARTSHUT
#include "cp$src:mem_def.h"
#endif
#include "cp$inc:prototypes.h"

int dup_setmode( );
int null_procedure ();

struct DDB fy_ddb = {
	"fy",			/* how this routine wants to be called	*/
	null_procedure,		/* read routine				*/
	null_procedure,		/* write routine			*/
	null_procedure,		/* open routine				*/
	null_procedure,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	dup_setmode,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	0,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};

#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define malloc_z(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)

struct QUEUE dup_cbs;
struct SEMAPHORE dup_poll_process;
int dup_mode;
int dup_poll_pid = 0;
int dup_poll_ref = 0;

extern struct SEMAPHORE scs_lock;
extern null_procedure( );

int dup_debug_flag = 0;

#if DEBUG
#define dprintf(fmt, args)	\
    _dprintf(fmt, args)
#else
#define dprintf(fmt, args)
#endif

#if DEBUG
void _dprintf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((dup_debug_flag & 1) != 0)
	printf(fmt, a, b, c, d, e, f, g, h, i, j);
}
#endif

#if !STARTSHUT
int dup_startup()
{
    return 0;
}
#endif
#if STARTSHUT
/*+
 * ============================================================================
 * = dup_startup - Startup the DUP process.				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine starts up the DUP session.
 *  
 * FORM OF CALL:
 *  
 *	dup_startup(inode, task, bg )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_dup_no_conn			- the DUP connection is not open
 *	others				- other codes returned by subordinates
 *       
 * ARGUMENTS:
 *
 *	struct INODE *inode		- file inode
 *	char *task			- name of DUP local program to run
 *	int bg				- flag to take all default answers
 *
 * SIDE EFFECTS:
 *
 *	A connection may be brought up, used, and then torn down.
 *
-*/

int dup_startup(struct INODE *inode, char *task, int bg)
{
    int i, status;
    int hose, slot;
    int loadadr;
    struct device *dev;
    extern struct window_head config;
    struct mscp_sb *sb;
    struct cb *cb;
    struct ub *ub;

    dprintf("dup_startup: %s\n", p_args1(inode->name));

/* Lookup setmode pointer based on filename: */

    explode_devname(inode->name, 0, 0, 0, 0, 0, &slot, &hose);

    dev = find_dev_in_slot(&config, hose, slot);
    if (dev == NULL)
	return msg_failure;

    if (dev->tbl->overlay)
	loadadr = ovly_load(dev->tbl->overlay);
    else
	loadadr = ovly_load(dev->tbl->name);
    if (!loadadr)
	return msg_failure;

/* Call setmode to start the port (if not already started): */

    status = dev->tbl->setmode(DDB$K_START, dev);
    if (status != msg_success)
	return status;

/* Find the unit we want: */

    status = dev->tbl->establish(inode, dev, DEVCLASS$K_DISK);
    if (status != msg_success)
	return status;

/* If DUP poll has not been started, then do it now */

    dup_create_poll();

    dprintf("dup_startup: unit found\n", p_args0);

/*  The device is valid, initialize what we need. */

    ub = inode->misc;
    cb = ub->cb;
    sb = ub->sb;

    dprintf("dup_startup: device valid, ub = %x, cb = %x, sb = %x\n", p_args3(ub, cb, sb));

    status = dup_begin(sb, task, bg);

/* Call setmode to stop the port: */

dup_shutdown: 

    dprintf("dup_shutdown:\n", p_args0);

/* stop DUP poll */

    dup_destroy_poll();

    dprintf("dup_shutdown: stop\n", p_args0);

    dev->tbl->setmode(DDB$K_STOP, dev);

    if (dev->tbl->overlay)
	ovly_remove(dev->tbl->overlay);
    else
	ovly_remove(dev->tbl->name);

    dprintf("dup_shutdown: status = %x\n", p_args1(status));

    return (status);
}

/*+
 * ============================================================================
 * = dup_create_poll - Create dup polling process                             =
 * ============================================================================
 *
 * OVERVIEW:
 * 
 *	This routine create the DUP polling process.
 * 
 * FORM OF CALL:
 * 
 *	dup_create_poll( )
 * 
 * RETURNS:
 *
 *	Nothing.
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
 *	DUP polling process is started.
 *
-*/

void dup_create_poll()
{
    int dup_poll();

    dup_poll_ref++;

    if (dup_poll_pid != 0)
	return;

    krn$_seminit(&dup_poll_process, 0, "dup_poll");

    dup_poll_pid = krn$_create(		/* */
      dup_poll, 			/* address of process   */
      null_procedure, 			/* startup routine      */
      &dup_poll_process, 		/* completion semaphore */
      5, 				/* process priority     */
      -1, 				/* cpu affinity mask    */
      4096, 				/* stack size           */
      "dup_poll", 			/* process name         */
      0, 0, 0, 0, 0, 0); 		/* i/o                  */

    dprintf("Created dup_poll: pid %x\n", p_args1(dup_poll_pid));
}

/*+
 * ============================================================================
 * = dup_destroy_poll - Destroy DUP polling process                           =
 * ============================================================================
 *
 * OVERVIEW:
 * 
 *	This routine stops the DUP polling process.
 * 
 * FORM OF CALL:
 * 
 *	dup_destroy_poll( )
 * 
 * RETURNS:
 *
 *	Nothing.
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/

void dup_destroy_poll()
{
    if (dup_poll_ref > 0)
	--dup_poll_ref;

    if (dup_poll_ref == 0) {

	krn$_delete(dup_poll_pid);
	krn$_wait(&dup_poll_process);

	krn$_semrelease(&dup_poll_process);

	dup_poll_pid = 0;
    }
}
#endif

/*+
 * ============================================================================
 * = dup_init - initialize the DUP DDB                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine gives us a chance to initialize the DUP DDB (for FY).
 *	Private entry points are made public.
 *  
 * FORM OF CALL:
 *  
 *	dup_init( )
 *  
 * RETURN CODES:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
 *	FY DDB is filled in.
 *
-*/
int dup_init( )
    {
    int dup_poll( );

#if STARTSHUT
    if (dup_cbs.flink != 0)
	return msg_success;
#endif
    dup_cbs.flink = &dup_cbs.flink;
    dup_cbs.blink = &dup_cbs.flink;
    dup_mode = DDB$K_INTERRUPT;
#if !STARTSHUT
    krn$_create( dup_poll, null_procedure, 0, 5, 0, 4096,
	    "dup_poll", "nl", "r", "nl", "w", "nl", "w" );
#endif
    return( msg_success );
    }

/*+
 * ============================================================================
 * = dup_setmode - stop or start the DUP driver                               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine deals with requests to stop or start the DUP driver.
 *  
 * FORM OF CALL:
 *  
 *	dup_setmode( mode )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	int mode			- the new mode
 *
 * SIDE EFFECTS:
 *
 *	The driver may be stopped or started.
 *
-*/
void dup_setmode( int mode )
    {
    if( mode != DDB$K_READY )
	dup_mode = mode;
    }

/*+
 * ============================================================================
 * = dup_poll - poll all known DUP connections                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine basically sleeps, waking periodically do deal with all
 *	known DUP connections.  If a particular DUP connection is being
 *	broken, then that connection is removed from the list of known DUP
 *	connections, and we deal with it no longer.  Else, we issue keep-alive
 *	messages as required, and handle timing of outstanding DUP commands.
 *	Again, we use reference counts in the CB to make sure that no CB
 *	vanishes while it is in use.
 *  
 * FORM OF CALL:
 *  
 *	dup_poll( )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
 *	Keep-alive messages may be issued.  Servers may be requested to supply
 *	status of outstanding commands.
 *
-*/
void dup_poll( )
    {
    struct dup_cb *p;
    struct cb *cb;
    struct rsp *rsp;
    struct PCB *pcb;

    pcb = getpcb();

    while( 1 )
	{
	/*
	 *  Take it easy.  Go to sleep.  Every now and then, wake up.  Use
	 *  fudge factors when calculating when to do the next thing; err
	 *  on the "too short" side for things that the other end is timing,
	 *  and err on the "too long" side for things that we are timing.
	 *  Hold the SCS lock for an entire pass through the list of known
	 *  DUP connections.
	 */
	krn$_sleep( 1000 );
#if STARTSHUT
	if (pcb->pcb$l_killpending & SIGNAL_KILL) {
	    dprintf("dup_poll killed\n", p_args0);
	    return;
	}
#endif
	krn$_wait( &scs_lock );
	for( p = dup_cbs.flink; p != &dup_cbs.flink; p = p->dup_cb.flink )
	    {
	    /*
	     *  We've got another connection to watch.
	     */
	    cb = p->cb;
	    /*
	     *  If the connection is going away, we'll stop watching.
	     */
	    if( cb->state != cb_k_open )
		{
		struct dup_cb *q;

		/*
		 *  The connection is broken.  Forget this connection.
		 */
		q = p;
		p = p->dup_cb.blink;
		remq( q );
		free( q );
		break;
		}
	    /*
	     *  Pretend that an unsolicited command has arrived.  This allows
	     *  any task waiting for such input to be periodically woken.
	     */
	    if( cb->receive_s.count < 0 )
		krn$_post( &cb->receive_s );
	    /*
	     *  If it's time, send a keep-alive message to make the remote
	     *  DUP server happy.
	     */
	    if( --cb->host_tmo == 0 )
		{
		dup_keepalive( cb );
		cb->host_tmo = dup_k_host_tmo - 10;
		}
	    /*
	     *  If the CB's RSP queue is not empty, then see if we have a new
	     *  command to check up on.  The first RSP in the CB's RSP queue
	     *  is the oldest command, and if the active bit is set in that
	     *  RSP and the keys match, then the command is active and needs
	     *  to be timed.
	     */
	    if( cb->rsp.flink != &cb->rsp.flink )
		/*
		 *  The queue is not empty; see if the old "oldest" command
		 *  is still worth our interest (the RSP should be the old
		 *  RSP, it should still be active, and the keys should still
		 *  match).
		 */
		if( ( ( rsp = cb->rsp.flink ) == cb->old_rsp )
			&& rsp->active && ( rsp->key == cb->old_key ) )
		    {
		    /*
		     *  The old "oldest" command is still the oldest and is
		     *  still active.  If the timer counts down to 0, kill
		     *  this connection.
		     */
		    if( --cb->cont_tmo == 0 )
			{
			qprintf( msg_dup_cmd_tmo_fatal, cb->sb->name );
			cb->break_vc( cb );
			}
		    }
		/*
		 *  We either have a new "oldest", or no good "oldest" at all.
		 *  (This is controlled by whether the new "oldest" RSP is now
		 *  active or not.)
		 */
		else if( rsp->active )
		    {
		    /*
		     *  This is a new "oldest".  Remember it, and restart the
		     *  timer.
		     */
		    cb->old_rsp = rsp;
		    cb->old_key = rsp->key;
		    cb->cont_tmo = cb->timeout + 10;
		    }
		else
		    /*
		     *  There's nothing worth remembering.
		     */
		    cb->old_rsp = 0;
	    else
		/*
		 *  The queue is empty, so there's no "oldest" any more.
		 */
		cb->old_rsp = 0;
	    }
	/*
	 *  This pass is complete.  Drop the SCS lock.
	 */
	krn$_post( &scs_lock );
	}
    }

/*+
 * ============================================================================
 * = dup_begin - begin a DUP session                                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles an entire DUP session, from connecting to the
 *	remote DUP server, through running local programs, to tearing down the
 *	connection at session end.
 *  
 * FORM OF CALL:
 *  
 *	dup_begin( sb, task, bg )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_dup_no_conn			- the DUP connection is not open
 *	others				- other codes returned by subordinates
 *       
 * ARGUMENTS:
 *
 *	struct mscp_sb *sb		- system block of the system to use
 *	char *task			- name of DUP local program to run
 *	int bg				- flag to take all default answers
 *
 * SIDE EFFECTS:
 *
 *	A connection may be brought up, used, and then torn down.
 *
-*/
int dup_begin( struct mscp_sb *sb, char *task, int bg )
    {
    char input[8];
    int first;
    int i;
    int n;
    int t;
    int dup_appl_msg_input( );
    int dup_connection_error( );
    struct cb *cb;
    struct ub *ub;
    struct rsp *rsp;
    struct dup_cb *p;

    /*
     *  Grab the SCS lock, and then connect to the DUP server.  The routine
     *  to call to perform the connection is found in the SB.
     */
    krn$_wait( &scs_lock );
    if( sb->state != sb_k_open )
	{
	err_printf( msg_no_vc, sb->name );
	return( msg_no_vc );
	}
    cb = sb->connect( sb, &fy_ddb,
	    "MSCP$DUP        ",
	    "MSCP$DUP_DRIVER ",
	    "                ",
	    2, &dup_appl_msg_input, &dup_connection_error );
    /*
     *  If a connection was not established for whatever reason, just let go
     *  of the SCS lock and exit.
     */
    if( !cb )
	{
	krn$_post( &scs_lock );
	err_printf( msg_dup_no_conn, sb->name );
	return( msg_dup_no_conn );
	}
    /*
     *  Do required further initialization.  Use the reference count to
     *  protect ourselves, and allocate an RSP.  Put this connection on the
     *  list of connections to be watched.  Then ask the user what he wants
     *  to do, and do it, until he wants to quit (or an error occurs).
     */
    cb->ref++;
    rsp = malloc( sizeof( *rsp ) );
    insq( rsp, cb->rsp.blink );
    krn$_seminit( &rsp->sem, 0, "dup_rsp" );
    p = malloc( sizeof( *p ) );
    p->cb = cb;
    cb->host_tmo = dup_k_host_tmo - 10;
    cb->old_rsp = 0;
    insq( p, dup_cbs.blink );
    t = msg_success;
    first = 1;
    while( 1 )
	{
	/*
	 *  Prompt the user.  He may type in the name of a DUP program which
	 *  the remote server will execute.  The name is 1 to 6 characters
	 *  long.  A null response, or ^Z, will end the session.  Once the name
	 *  is entered, it is converted to upper case.
	 */
	if( task )
	    {
	    n = strlen( task );
	    strcpy( input, task );
	    }
	else if( first )
	    {
	    first = 0;
	    n = 6;
	    strcpy( input, "DIRECT" );
	    }
	else
	    {
	    krn$_post( &scs_lock );
	    n = read_with_prompt( "Task? ", sizeof input, input, 0, 0, 1 ) - 1;
	    krn$_wait( &scs_lock );
	    if( n <= 0 )
		break;
	    input[n] = 0;
	    }
	for( i = 0; i < n; i++ )
	    input[i] = toupper( input[i] );
	krn$_post( &scs_lock );
	printf( "starting %s on %s (%s)\n",
		input, cb->sb->name, cb->sb->alias );
	if( bg )
	    qprintf( "starting %s on %s (%s)\n",
		    input, cb->sb->name, cb->sb->alias );
	else
	    printf( "\n" );
	krn$_wait( &scs_lock );
	if( cb->state != cb_k_open )
	    break;
	while( n < 6 )
	    input[n++] = ' ';
	/*
	 *  If we can execute the DUP local program successfully, then fall
	 *  into "dialog" mode.  We exit "dialog" mode at the request of the
	 *  local program being executed (or any error), at which point we
	 *  may run another DUP local program.
	 */
	cb->timeout = dup_k_cont_tmo;
	cb->send_imm = 0;
	t = dup_get_cont_cmd( cb, rsp );
	if( t != msg_success )
	    break;
	t = dup_exe_local_cmd( cb, rsp, input );
	if( t != msg_success )
	    break;
	if( cb->send_imm )
	    t = dup_dialog_imm( cb, rsp, bg );
	else
	    t = dup_dialog( cb, rsp, bg );
	if( t != msg_success )
	    break;
	if( task || bg )
	    break;
	}
    /*
     *  Clean up and exit.  The disconnect call does not return until the
     *  connection is completely run down.  If an error occurs such that a
     *  clean disconnect is not possible, then the VC is broken.
     */
    krn$_semrelease( &rsp->sem );
    remq( rsp );
    free( rsp );
    if( ( cb->state == cb_k_open ) || ( cb->state == cb_k_disc_match ) )
	cb->disconnect( cb );
    else
	{
	--cb->ref;
	if( cb->ref == 0 )
	    cb->discard_cb( cb );
	}
    remq( p );
    free( p );
    krn$_post( &scs_lock );
    return( t );
    }

/*+
 * ============================================================================
 * = dup_appl_msg_input - called on receipt of an application message         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine is called whenever an application message is received
 *	over this connection.  This is usually a response to an earlier DUP
 *	command, but it can be a DUP Send Data Immediate command, in which
 *	case we buffer it and hold it on a CB queue, to be consumed later.
 *	If it is a response, and a task is waiting for this DUP response, then
 *	said response is delivered to said task.
 *  
 * FORM OF CALL:
 *  
 *	dup_appl_msg_input( cb, p, len )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct dup_generic_rsp	*p	- pointer to DUP packet
 *	int len				- length of DUP packet
 *
 * SIDE EFFECTS:
 *
 *	A sleeping task may be awoken.  An entry may be made in a CB queue.
 *
-*/
void dup_appl_msg_input( struct cb *cb, struct dup_generic_rsp *p, int len )
    {
    struct rsp *rsp;

    switch( p->opcode )
	{
	/*
	 *  This is a list of the DUP opcodes we understand.  We shouldn't
	 *  get anything not on this list.  If we do, something's broken.
	 */
	case dup_k_send_imm:
	    /*
	     *  This is a received command; allocate an RSP for it, and fill
	     *  in the RSP.  Then hang the RSP off of the CB queue, and notify
	     *  any waiting tasks that a new RSP is available.
	     */
	    rsp = malloc( sizeof( *rsp ) );
	    rsp->size = len;
	    memcpy( rsp->buffer, p, len );
	    insq( rsp, cb->receive_q.blink );
	    krn$_post( &cb->receive_s );
	    break;

	case dup_k_get_cont+dup_k_end:
	case dup_k_exe_local+dup_k_end:
	case dup_k_send+dup_k_end:
	case dup_k_receive+dup_k_end:
	case dup_k_send_imm+dup_k_end:
	    /*
	     *  If there is an allocated RSP, then copy the received DUP
	     *  response to the RSP's buffer, and then post the associated
	     *  semaphore.  (The RSP's address, if there was one, was passed
	     *  in one of the fields of the original DUP command, and the
	     *  server on the other end is required to echo it back to us in
	     *  the DUP response.)
	     */
	    rsp = p->cmd_ref;
	    if( rsp )
		{
		rsp->size = len;
		memcpy( rsp->buffer, p, len );
		rsp->active = 0;
		remq( rsp );
		insq( rsp, cb->rsp.blink );
		krn$_post( &rsp->sem );
		}
	    break;

	/*
	 *  Very bad news if we get here.  Die a hard death (we're in effect
	 *  committing suicide).
	 */
	default:
	    qprintf( msg_dup_bad_cmd_rsp, p->opcode, cb->sb->name );
	    cb->break_vc( cb );
	}
    }

/*+
 * ============================================================================
 * = dup_connection_error - clean up a failed connection                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine gets called when a DUP connection is being torn down.
 *	All outstanding commands for which a response is required will have
 *	an RSP on a CB queue; take care of them.  All unprocessed unsolicited
 *	DUP commands will have an RSP on the CB receive queue; take care of
 *	them.  Post the CB receive semaphore.
 *  
 * FORM OF CALL:
 *  
 *	dup_connection_error( cb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *
 * SIDE EFFECTS:
 *
 *	Tasks waiting for pending commands to complete will find that those
 *	commands have magically done so, and thus those tasks will be
 *	signalled.  The CB receive queue will be emptied.  The CB receive
 *	semaphore will be posted.
 *
-*/
void dup_connection_error( struct cb *cb )
    {
    struct dup_generic_rsp *q;
    struct rsp *rsp;
    struct ub *ub;

    /*
     *  Rip through the list of RSPs pending, and pretend that we have received
     *  the expected DUP responses.  This will wake up waiting tasks.
     */
    for( rsp = cb->rsp.flink; rsp != &cb->rsp.flink; rsp = rsp->rsp.flink )
	{
	rsp->size = sizeof( *q );
	q = rsp->buffer;
	q->status.major = dup_k_aborted;
	q->status.minor = 0;
	krn$_post( &rsp->sem );
	}
    /*
     *  Rip through the receive queue (unsolicited DUP packets waiting to
     *  be processed) and deallocate all entries.
     */
    while( ( rsp = cb->receive_q.flink ) != &cb->receive_q.flink )
	{
	remq( rsp );
	free( rsp );
	}
    /*
     *  Post the receive semaphore; there is no packet waiting, so any task
     *  waiting on the receive semaphore must recognize a broken connection
     *  as a valid reason for being signalled.
     */
    krn$_post( &cb->receive_s );
    }

/*+
 * ============================================================================
 * = dup_keepalive - keep a connection alive by issuing a dummy command       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine keeps a connection alive by issuing an DUP Get DUST
 *	Status command.  We don't bother to wait for the DUP response to come
 *	back.
 *  
 * FORM OF CALL:
 *  
 *	dup_keepalive( cb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *
 * SIDE EFFECTS:
 *
-*/
void dup_keepalive( struct cb *cb )
    {
    struct dup_get_cont_cmd *p;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	return;
    /*
     *  Get an application message packet, zero it, and fill in required
     *  fields.  Then send it.
     */
    p = cb->alloc_appl_msg( cb, sizeof( *p ) );
    p->cmd_ref = 0;
    p->opcode = dup_k_get_cont;
    cb->send_appl_msg( cb, p );
    }

/*+
 * ============================================================================
 * = dup_get_cont_cmd - send a DUP Get DUST Status command                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a DUP Get DUST Status command.
 *  
 * FORM OF CALL:
 *  
 *	dup_get_cont_cmd( cb, rsp )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_dup_no_conn			- the DUP connection is not open
 *	msg_dup_bad_get_cont		- the Get Controller Status command failed
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- a place for the response
 *
 * SIDE EFFECTS:
 *
 *	The remote server timeout value is set.
 *
-*/
int dup_get_cont_cmd( struct cb *cb, struct rsp *rsp )
    {
    struct dup_get_cont_cmd *p;
    struct dup_get_cont_rsp *q;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	{
	err_printf( msg_dup_no_conn, cb->sb->name );
	return( msg_dup_no_conn );
	}
    /*
     *  Get an application message packet, zero it, and fill in required
     *  fields.  Then send it.
     */
    p = cb->alloc_appl_msg( cb, sizeof( *p ) );
    p->cmd_ref = rsp;
    p->opcode = dup_k_get_cont;
    rsp->active = 1;
    rsp->key++;
    cb->send_appl_msg( cb, p );
    /*
     *  Now wait for the response.  When it comes, remember the server's
     *  timeout value.
     */
    krn$_post( &scs_lock );
    krn$_wait( &rsp->sem );
    krn$_wait( &scs_lock );
    q = rsp->buffer;
    if( q->status.major != dup_k_success )
	{
	err_printf( msg_dup_bad_status, q->status.major, q->status.minor );
	err_printf( msg_dup_bad_get_cont, cb->sb->name );
	return( msg_dup_bad_get_cont );
	}
    cb->timeout = q->cont_tmo;
    if( q->cont_flag & 16 )
	cb->send_imm = 1;
    return( msg_success );
    }

/*+
 * ============================================================================
 * = dup_exe_local_cmd - send a DUP Execute Local Program command             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a DUP Execute Local Program command.
 *  
 * FORM OF CALL:
 *  
 *	dup_exe_local_cmd( cb, rsp, program )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_dup_no_conn			- the DUP connection is not open
 *	msg_dup_bad_exe_local		- the Execute Local Program command failed
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- a place for the response
 *	char *program			- name of program to execute
 *
 * SIDE EFFECTS:
 *
-*/
int dup_exe_local_cmd( struct cb *cb, struct rsp *rsp, char *program )
    {
    struct dup_exe_local_cmd *p;
    struct dup_exe_local_rsp *q;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	{
	err_printf( msg_dup_no_conn, cb->sb->name );
	return( msg_dup_no_conn );
	}
    /*
     *  Get an application message packet, zero it, and fill in required
     *  fields.  Then send it.
     */
    p = cb->alloc_appl_msg( cb, sizeof( *p ) );
    p->cmd_ref = rsp;
    p->opcode = dup_k_exe_local;
    p->modifier = dup_allow_standalone;
    memcpy( p->program, program, sizeof( p->program ) );
    rsp->active = 1;
    rsp->key++;
    cb->send_appl_msg( cb, p );
    /*
     *  Now wait for the response.  When it comes, return success or failure.
     */
    krn$_post( &scs_lock );
    krn$_wait( &rsp->sem );
    krn$_wait( &scs_lock );
    q = rsp->buffer;
    if( q->status.major != dup_k_success )
	{
	err_printf( msg_dup_bad_status, q->status.major, q->status.minor );
	err_printf( msg_dup_bad_exe_local, cb->sb->name );
	return( msg_dup_bad_exe_local );
	}
    cb->timeout = q->prog_tmo;
    return( msg_success );
    }

/*+
 * ============================================================================
 * = dup_send_cmd - send a DUP Send Data command                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a DUP Send Data command.
 *  
 * FORM OF CALL:
 *  
 *	dup_send_cmd( cb, rsp, buffer, size )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_dup_no_conn			- the DUP connection is not open
 *	msg_dup_bad_send		- the Send Data command failed
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- a place for the response
 *	unsigned char *buffer		- buffer for read data
 *	int *size			- pointer to size to read
 *
 * SIDE EFFECTS:
 *
-*/
int dup_send_cmd( struct cb *cb, struct rsp *rsp,
	unsigned char *buffer, int *size )
    {
    unsigned long int map_info[4];
    struct dup_send_cmd *p;
    struct dup_send_rsp *q;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	return( msg_dup_no_conn );
    /*
     *  Get an application message packet, zero it, and fill in required
     *  fields.  Then send it.  Notice the call to MAP; this call into the
     *  port driver takes care of mapping the user's buffer in some form that
     *  the port hardware can understand.  Map information is kept in the RSP
     *  for convenience sake.
     */
    p = cb->alloc_appl_msg( cb, sizeof( *p ) );
    p->cmd_ref = rsp;
    p->opcode = dup_k_send;
    p->size = *size;
    cb->map( cb, buffer, *size, p->buffer, map_info );
    rsp->active = 1;
    rsp->key++;
    cb->send_appl_msg( cb, p );
    /*
     *  Now wait for the response.  When it comes, we can unmap the user's
     *  buffer.  Then we return success or failure based on the status in the
     *  DUP response packet.
     */
    krn$_post( &scs_lock );
    krn$_wait( &rsp->sem );
    krn$_wait( &scs_lock );
    cb->unmap( cb, map_info );
    q = rsp->buffer;
    if( q->status.major != dup_k_success )
	{
	err_printf( msg_dup_bad_status, q->status.major, q->status.minor );
	err_printf( msg_dup_bad_send, cb->sb->name );
	return( msg_dup_bad_send );
	}
    *size = q->size;
    return( msg_success );
    }

/*+
 * ============================================================================
 * = dup_receive_cmd - send a DUP Receive Data command                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a DUP Receive Data command.
 *  
 * FORM OF CALL:
 *  
 *	dup_receive_cmd( cb, rsp, buffer, size )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_dup_no_conn			- the DUP connection is not open
 *	msg_dup_bad_receive		- the Receive Data command failed
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- a place for the response
 *	unsigned char *buffer		- buffer for read data
 *	int *size			- pointer to size to read
 *
 * SIDE EFFECTS:
 *
-*/
int dup_receive_cmd( struct cb *cb, struct rsp *rsp,
	unsigned char *buffer, int *size )
    {
    unsigned long int map_info[4];
    struct dup_receive_cmd *p;
    struct dup_receive_rsp *q;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	return( msg_dup_no_conn );
    /*
     *  Get an application message packet, zero it, and fill in required
     *  fields.  Then send it.  Notice the call to MAP; this call into the
     *  port driver takes care of mapping the user's buffer in some form that
     *  the port hardware can understand.  Map information is kept in the RSP
     *  for convenience sake.
     */
    p = cb->alloc_appl_msg( cb, sizeof( *p ) );
    p->cmd_ref = rsp;
    p->opcode = dup_k_receive;
    p->size = *size;
    cb->map( cb, buffer, *size, p->buffer, map_info );
    rsp->active = 1;
    rsp->key++;
    cb->send_appl_msg( cb, p );
    /*
     *  Now wait for the response.  When it comes, we can unmap the user's
     *  buffer.  Then we return success or failure based on the status in the
     *  DUP response packet.
     */
    krn$_post( &scs_lock );
    krn$_wait( &rsp->sem );
    krn$_wait( &scs_lock );
    cb->unmap( cb, map_info );
    q = rsp->buffer;
    if( q->status.major != dup_k_success )
	{
	err_printf( msg_dup_bad_status, q->status.major, q->status.minor );
	err_printf( msg_dup_bad_receive, cb->sb->name );
	return( msg_dup_bad_receive );
	}
    *size = q->size;
    return( msg_success );
    }

/*+
 * ============================================================================
 * = dup_send_imm_cmd - send a DUP Send Data Immediate command                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends as many DUP Send Data Immediate commands
 *	as it takes to send the given text string to the remote DUP server.
 *	Each command except the last will be marked "to be continued".
 *  
 * FORM OF CALL:
 *  
 *	dup_send_imm_cmd( cb, rsp, text, len )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_dup_no_conn			- the DUP connection is not open
 *	msg_dup_bad_receive		- the Receive Data command failed
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- a place for the response
 *	char *text			- text string
 *	int len				- length of text string
 *
 * SIDE EFFECTS:
 *
-*/
int dup_send_imm_cmd( struct cb *cb, struct rsp *rsp, char *text, int len )
    {
    struct dup_send_imm_cmd *p;
    struct dup_send_imm_rsp *q;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	return( msg_dup_no_conn );
    while( 1 )
	{
	/*
	 *  Get an application message packet, zero it, and fill in required
	 *  fields.  Then send it.
	 */
	p = cb->alloc_appl_msg( cb, sizeof( *p ) );
	p->cmd_ref = rsp;
	p->opcode = dup_k_send_imm;
	if( len > sizeof( p->text ) )
	    {
	    /*
	     *  The remaining text string will not fit in a single DUP Send
	     *  Data Immediate command, so send what will fit, and say that
	     *  there's more to come.
	     */
	    p->modifier = dup_to_be_cont;
	    p->size = sizeof( p->text );
	    }
	else
	    {
	    /*
	     *  The remaining text string will fit in a single DUP Send Data
	     *  Immediate command, so send it all, and say we're done.
	     */
	    p->size = len;
	    }
	memcpy( p->text, text, len );
	text += p->size;
	len -= p->size;
	rsp->active = 1;
	rsp->key++;
	cb->send_appl_msg( cb, p );
	/*
	 *  Now wait for the response.  When it comes, return success or
	 *  failure.
         */
	krn$_post( &scs_lock );
	krn$_wait( &rsp->sem );
	krn$_wait( &scs_lock );
	q = rsp->buffer;
	if( q->status.major != dup_k_success )
	    {
	    err_printf( msg_dup_bad_status, q->status.major, q->status.minor );
	    err_printf( msg_dup_bad_send, cb->sb->name );
	    return( msg_dup_bad_send );
	    }
	if( len == 0 )
	    break;
	}
    return( msg_success );
    }

/*+
 * ============================================================================
 * = dup_send_imm_rsp - send a DUP Send Data Immediate response               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a DUP Send Data Immediate response.
 *  
 * FORM OF CALL:
 *  
 *	dup_send_imm_rsp( cb, q )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct dup_send_imm_cmd *q	- the command to respond to
 *
 * SIDE EFFECTS:
 *
-*/
void dup_send_imm_rsp( struct cb *cb, struct dup_send_imm_cmd *q )
    {
    struct dup_send_imm_rsp *p;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	return;
    /*
     *  Get an application message packet, zero it, and fill in required
     *  fields.  Then send it.
     */
    p = cb->alloc_appl_msg( cb, sizeof( *p ) );
    p->cmd_ref = q->cmd_ref;
    p->opcode = dup_k_send_imm+dup_k_end;
    p->status.major = dup_k_success;
    p->status.minor = 0;
    cb->send_appl_msg( cb, p );
    }

/*+
 * ============================================================================
 * = dup_dialog_imm - handle the DUP dialog of an executing program           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles the standard DUP dialog of an executing DUP
 *	local program for a server which implements DUP's Send Data Immediate
 *	command.  These commands can arrive at any time from the remote DUP
 *	server; when they do arrive, they are buffered and hung off of the
 *	CB, for eventual comsumption by this routine.  A series of Send Data
 *	Immediate commands may be received, which contain a string to be
 *	displayed for the user, and optionally a string may be solicited from
 *	the user and returned to the remote DUP server via another series of
 *	Send Data Immediate commands.  This continues until the executing
 *	program indicates that it is time to exit.
 *  
 * FORM OF CALL:
 *  
 *	dup_dialog( cb, rsp, bg )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_dup_no_conn			- the DUP connection is not open
 *	msg_dup_ill_msg_typ		- an illegal message type was received
 *	others				- other codes returned by subordinates
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- a place for the response
 *	int bg				- flag to take all default answers
 *
 * SIDE EFFECTS:
 *
-*/
int dup_dialog_imm( struct cb *cb, struct rsp *rsp, int bg )
    {
    char bell[1];
    char prompt[130];
    char input[130];
    char *port;
    int index;
    int type;
    int tmo;
    int i;
    int n;
    int t;
    struct dup_send_imm_cmd *p;
    struct rsp *cmd;

    port = cb->sb->name;
    /*
     *  There is currently no message, so indicate as much.
     */
    type = 0;
    tmo = 0;
    bell[0] = 7;
    while( 1 )
	{
	if( killpending( ) )
	    return( msg_ctrlc );
	/*
	 *  Wait for an unsolicited Send Data Immediate command to appear on
	 *  the CB's receive queue.
	 */
	krn$_post_wait( &scs_lock, &cb->receive_s );
	krn$_wait( &scs_lock );
	/*
	 *  If the connection is gone, so are we.
	 */
	if( cb->state != cb_k_open )
	    {
	    err_printf( msg_dup_no_conn, port );
	    return( msg_dup_no_conn );
	    }
	cmd = cb->receive_q.flink;
	/*
	 *  See if this is a spurious wake-up.  If it is, and we get 30 of
	 *  them in a row, we prod the other side to tell us what's going on.
	 */
	if( cmd == &cb->receive_q.flink )
	    {
	    if( ++tmo == 30 )
		if( bg )
		    dup_get_cont_cmd( cb, rsp );
		else
		    dup_send_imm_cmd( cb, rsp, bell, 1 );
	    }
	else
	    {
	    tmo = 0;
	    p = cmd->buffer;
	    /*
	     *  The message type appears in the upper 4 bits of the second
	     *  byte of a new message.  If we are just starting a new message,
	     *  fetch the type, and then skip the first two bytes of the
	     *  message.  If we are in the middle of a message, then just
	     *  accumulate message bytes (but no more than 128 of them!).
	     */
	    i = 0;
	    if( type == 0 )
		{
		type = ( p->text[1] >> 4 ) & 0xf;
		index = 0;
		i += 2;
		p->size -= 2;
		}
	    if( index + p->size > 128 )
		p->size = 128 - index;
	    memcpy( &prompt[index], &p->text[i], p->size );
	    index += p->size;
	    /*
	     *  Respond to the Send Data Immediate command just received, and
	     *  consume it.
	     */
	    dup_send_imm_rsp( cb, p );
	    i = p->modifier & dup_to_be_cont;
	    remq( cmd );
	    free( cmd );
	    /*
	     *  If the message is not to be continued, then we've got it all.
	     *  In that case, we need to act on it.
	     */
	    if( !i )
		{
		prompt[index] = 0;
		if( bg )
		    switch( type )
			{
			case 1:
			    /*
			     *  Message type 1 involves displaying the received
			     *  text for the user, and collecting a response,
			     *  which is sent back to the remote DUP server.
			     *  Since there is no user to enter a response,
			     *  this type is not allowed.
			     */
			    return( msg_dup_ill_msg_typ );

			case 2:
			    /*
			     *  Message type 2 involves displaying the received
			     *  text for the user, and sending back a null
			     *  response to the remote DUP server.
			     */
			    qprintf( "%s --> %s\n", port, prompt );
			    t = dup_send_imm_cmd( cb, rsp, "", 0 );
			    if( t != msg_success )
				return( t );
			    break;
	
			case 3:
			    /*
			     *  Message type 3 involves displaying the received
			     *  text for the user.
			     */
			    qprintf( "%s --> %s\n", port, prompt );
			    break;
	
			case 4:
			case 5:
			    /*
			     *  Message types 4 and 5 involve displaying the
			     *  received text for the user, and then exiting.
			     */
			    qprintf( "%s --> %s\n", port, prompt );
			    return( msg_success );
	
			default:
			    /*
			     *  Die an ugly death for unrecognized message
			     *  types.
			     */
			    err_printf( msg_dup_ill_msg_typ, type, port );
			    cb->break_vc( cb );
			    return( msg_dup_ill_msg_typ );
			}
		else
		    switch( type )
			{
			case 1:
			case 2:
			    /*
			     *  Message types 1 and 2 involve displaying the
			     *  received text for the user, and collecting a
			     *  response, which is sent back to the remote DUP
			     *  server.
			     */
			    krn$_post( &scs_lock );
			    n = read_with_prompt( prompt, sizeof input, input, 0,0,1 ) - 1;
			    if( n < 0 )
				{
				input[0] = 26;
				n = 1;
				}
			    else
				for( i = 0; i < n; i++ )
				    input[i] = toupper( input[i] );
			    krn$_wait( &scs_lock );
			    if( cb->state != cb_k_open )
				{
				err_printf( msg_dup_no_conn, port );
				return( msg_dup_no_conn );
				}
			    t = dup_send_imm_cmd( cb, rsp, input, n );
			    if( t != msg_success )
				return( t );
			    break;
	
			case 3:
			    /*
			     *  Message type 3 involves displaying the received
			     *  text for the user.
			     */
			    krn$_post( &scs_lock );
			    printf( "%s\n", prompt );
			    krn$_wait( &scs_lock );
			    break;
	
			case 4:
			case 5:
			    /*
			     *  Message types 4 and 5 involve displaying the
			     *  received text for the user, and then exiting.
			     */
			    krn$_post( &scs_lock );
			    printf( "%s\n", prompt );
			    krn$_wait( &scs_lock );
			    if( cb->state != cb_k_open )
				{
				err_printf( msg_dup_no_conn, port );
				return( msg_dup_no_conn );
				}
			    else
				return( msg_success );
	
			default:
			    /*
			     *  Die an ugly death for unrecognized message
			     *  types.
			     */
			    err_printf( msg_dup_ill_msg_typ, type, port );
			    cb->break_vc( cb );
			    return( msg_dup_ill_msg_typ );
			}
		type = 0;
		}
	    /*
	     *  See if somebody's trying to tell us something, and if so,
	     *  listen.
	     */
	    if( cb->state != cb_k_open )
		{
		err_printf( msg_dup_no_conn, port );
		return( msg_dup_no_conn );
		}
	    }
	}
    }

/*+
 * ============================================================================
 * = dup_dialog - handle the DUP dialog of an executing program               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles the standard DUP dialog of an executing DUP
 *	local program for a server which implements DUP's Send Data and
 *	Receive Data commands.
 *  
 * FORM OF CALL:
 *  
 *	dup_dialog( cb, rsp, bg )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_dup_no_conn			- the DUP connection is not open
 *	msg_dup_ill_msg_typ		- an illegal message type was received
 *	others				- other codes returned by subordinates
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- a place for the response
 *	int bg				- flag to take all default answers
 *
 * SIDE EFFECTS:
 *
-*/
int dup_dialog( struct cb *cb, struct rsp *rsp, int bg )
    {
    char prompt[130];
    char input[130];
    char *port;
    int index;
    int type;
    int size;
    int i;
    int n;
    int t;
    struct dup_receive_rsp *p;

    port = cb->sb->name;
    while( 1 )
	{
	if( killpending( ) )
	    return( msg_ctrlc );
	/*
	 *  Wait for data from the remote side.
	 */
	size = 128;
	t = dup_receive_cmd( cb, rsp, prompt, &size );
	if( t != msg_success )
	    return( t );
	prompt[size] = 0;
	/*
	 *  The message type appears in the upper 4 bits of the second
	 *  byte of a message.
	 */
	type = ( prompt[1] >> 4 ) & 0xf;
	if( bg )
	    switch( type )
		{
		case 1:
		    /*
		     *  Message type 1 involves displaying the received
		     *  text for the user, and collecting a response,
		     *  which is sent back to the remote DUP server.
		     *  Since there is no user to enter a response,
		     *  this type is not allowed.
		     */
		    return( msg_dup_ill_msg_typ );

		case 2:
		    /*
		     *  Message type 2 involves displaying the received
		     *  text for the user, and sending back a null
		     *  response to the remote DUP server.
		     */
		    qprintf( "%s --> %s\n", port, prompt + 2 );
		    size = 0;
		    t = dup_send_cmd( cb, rsp, input, &size );
		    if( t != msg_success )
			return( t );
		    break;
	
		case 3:
		    /*
		     *  Message type 3 involves displaying the received
		     *  text for the user.
		     */
		    qprintf( "%s --> %s\n", port, prompt + 2 );
		    break;
	
		case 4:
		case 5:
		    /*
		     *  Message types 4 and 5 involve displaying the
		     *  received text for the user, and then exiting.
		     */
		    qprintf( "%s --> %s\n", port, prompt + 2 );
		    return( msg_success );
	
		default:
		    /*
		     *  Die an ugly death for unrecognized message
		     *  types.
		     */
		    err_printf( msg_dup_ill_msg_typ, type, port );
		    cb->break_vc( cb );
		    return( msg_dup_ill_msg_typ );
		}
	else
	    switch( type )
		{
		case 1:
		case 2:
		    /*
		     *  Message types 1 and 2 involve displaying the
		     *  received text for the user, and collecting a
		     *  response, which is sent back to the remote DUP
		     *  server.
		     */
		    krn$_post( &scs_lock );
		    n = read_with_prompt( prompt + 2, sizeof input, input, 0, 0, 1 ) - 1;
		    if( n < 0 )
			{
			input[0] = 26;
			n = 1;
			}
		    else
			for( i = 0; i < n; i++ )
			    input[i] = toupper( input[i] );
		    krn$_wait( &scs_lock );
		    if( cb->state != cb_k_open )
			{
			err_printf( msg_dup_no_conn, port );
			return( msg_dup_no_conn );
			}
		    size = n;
		    t = dup_send_cmd( cb, rsp, input, &size );
		    if( t != msg_success )
			return( t );
		    break;
	
		case 3:
		    /*
		     *  Message type 3 involves displaying the received
		     *  text for the user.
		     */
		    krn$_post( &scs_lock );
		    printf( "%s\n", prompt + 2 );
		    krn$_wait( &scs_lock );
		    break;
	
		case 4:
		case 5:
		    /*
		     *  Message types 4 and 5 involve displaying the
		     *  received text for the user, and then exiting.
		     */
		    krn$_post( &scs_lock );
		    printf( "%s\n", prompt + 2 );
		    krn$_wait( &scs_lock );
		    if( cb->state != cb_k_open )
			{
			err_printf( msg_dup_no_conn, port );
			return( msg_dup_no_conn );
			}
		    else
			return( msg_success );
	
		default:
		    /*
		     *  Die an ugly death for unrecognized message
		     *  types.
		     */
		    err_printf( msg_dup_ill_msg_typ, type, port );
		    cb->break_vc( cb );
		    return( msg_dup_ill_msg_typ );
		}
	/*
	 *  See if somebody's trying to tell us something, and if so,
	 *  listen.
	 */
	if( cb->state != cb_k_open )
	    {
	    err_printf( msg_dup_no_conn, port );
	    return( msg_dup_no_conn );
	    }
	}
    }
