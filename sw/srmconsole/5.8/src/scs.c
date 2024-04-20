/*
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
 * Abstract:	SCS routines
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	28-Aug-1995	Initial entry.
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:stddef.h"
#include "cp$src:scs_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:cb_def.h"
#if MSCP_SERVER
#include "cp$src:server_def.h"
#endif
#include "cp$src:version.h"
#include "cp$inc:prototypes.h"

#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define malloc_a(x,y,z) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN|DYN$K_ALIGN,y,z)
#define free(x) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)

extern struct SEMAPHORE scs_lock;
#if MSCP_SERVER
extern struct server servers[];
extern int servers_enabled;
#endif
extern struct FILE *el_fp;

/*+
 * ============================================================================
 * = scs_alloc_appl_msg - allocate an application message packet              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine allocates an application message.  It does this by
 *	calling the lower level SCS allocation routine, which allocates any
 *	extra bytes that the lower level SCS routines require, yet it returns
 *	a pointer which hides those extra bytes.
 *  
 * FORM OF CALL:
 *  
 *	scs_alloc_appl_msg( cb, len )
 *  
 * RETURNS:
 *
 *	A pointer to the application portion of the allocated packet.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	int len				- number of application bytes to
 *					  allocate
 *
 * SIDE EFFECTS:
 *
 *	A credit is consumed.
 *
-*/
int scs_alloc_appl_msg( struct cb *cb, int len )
    {
    struct mscp_sb *sb;
    struct scs_appl_msg *p;

    sb = cb->sb;
    len = max( len, 48 );
    /*
     *  We must hold a credit in order to send a message, and the credit
     *  accounting is done when the message is allocated.  If we don't have
     *  a credit at the moment, we suspend ourselves, to be awakened when
     *  a received message carries a credit back to us.  Eventually we'll
     *  have the credit we need.
     */
    while( cb->credits < 1 )
	{
	krn$_post_wait( &scs_lock, &cb->credit_s );
	krn$_wait( &scs_lock );
	if( cb->state != cb_k_open )
	    break;
	}
    --cb->credits;
    /*
     *  Allocate an SCS packet which is large enough to hold the application
     *  message.
     */
    p = sb->alloc( sb, sizeof( *p ) + len );
    /*
     *  Return a pointer to the application portion, which is just past
     *  the SCS portion.  Zero the application portion.
     */
    return( memset( p+1, 0, len ) );
    }

/*+
 * ============================================================================
 * = scs_receive - process a received SCS message                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine processes received SCS messages.  SCS messages can either
 *	be control messages (pertaining to forming or disolving connections),
 *	which are handled here, or application messages, which are handed off
 *	to a higher layer.
 *  
 * FORM OF CALL:
 *  
 *	scs_receive( sb, p, len )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct mscp_sb *sb		- system block
 *	struct scs_msg *p		- pointer to SCS message
 *	int len				- length of SCS message
 *
 * SIDE EFFECTS:
 *
 *	The received SCS message is processed.
 *
-*/
void scs_receive( struct mscp_sb *sb, struct scs_msg *p, int len )
    {
    struct cb *cb;

    /*
     *  Most SCS messages carry a destination ID; if so, it is the CB of
     *  the connection to which the received message pertains.
     */
    cb = p->dst_id;
    switch( p->mtype )
	{
	/*
	 *  A connection is formed by sending a Connect Request, and waiting
	 *  for a Connect Response to come back.  If the remote server wants
	 *  to accept this connection, it will send back an Accept Request, to
	 *  which we respond with an Accept Response.  If the remote server
	 *  doesn't want to accept this connection, it can either send back a
	 *  Connect Response with a failure status, or it can send back a good
	 *  Connect Response followed by a Reject Request, to which we respond
	 *  with a Reject Response.
	 */
	case scs_k_con_req:
	    /*
	     *  We don't support Connect Request, so return a Connect Response
	     *  telling the other side to go away.
	     */
	    scs_send_con_rsp( sb, p );
	    break;

	case scs_k_con_rsp:
	    /*
	     *  We must have already send a Connect Request to the other side
	     *  for a Connect Response to be valid.  If we are in the right
	     *  state, and if the status of the Connect Response is good, then
	     *  advance to the appropriate new state, else the attempt to
	     *  connect has failed, and the CB control semaphore should be
	     *  posted so that the task connecting can exit.
	     */
	    if( cb->state != cb_k_con_req_out )
		{
		fprintf( el_fp, "SCS Connect Response received in wrong state\n" );
		sb->break_vc( sb );
		}
	    else
		{
		struct scs_con_rsp *q = p;

		if( q->status == scs_k_normal )
		    cb->state = cb_k_con_rsp_in;
		else
		    krn$_bpost( &cb->control_s );
		}
	    break;

	case scs_k_acc_req:
	    /*
	     *  We must have already received a Connect Response from the
	     *  other side for an Accept Request to be valid.  If we are in
	     *  the right state, then remember a few things in the CB, say
	     *  that the connection is open, return an Accept Response, and
	     *  awaken the task which started the connect attempt.
	     */
	    if( cb->state != cb_k_con_rsp_in )
		{
		fprintf( el_fp, "SCS Accept Request received in wrong state\n" );
		sb->break_vc( sb );
		}
	    else
		{
		struct scs_acc_req *q = p;

		cb->credits = q->credit;
		cb->dst_id = q->src_id;
		cb->state = cb_k_open;
		scs_send_acc_rsp( cb );
		krn$_bpost( &cb->control_s );
		}
	    break;

#if MSCP_SERVER
	case scs_k_acc_rsp:
	    /*
	     *  We must have already send an Accept Request to the other side
	     *  for an Accept Response to be valid.  If we are in the right
	     *  state, then say the connection is open and awaken the task
	     *  which started the accept attempt.
	     */
	    if( cb->state != cb_k_acc_req_out )
		{
		fprintf( el_fp, "SCS Accept Response received in wrong state\n" );
		sb->break_vc( sb );
		}
	    else
		{
		struct scs_acc_rsp *q = p;

		cb->state = cb_k_open;
		krn$_bpost( &cb->control_s );
		}
	    break;
#endif

	case scs_k_rej_req:
	    /*
	     *  If we get a Reject Request, the other side is telling us to
	     *  go away.  Do so.  Let the task which originated the attempt at
	     *  connection exit as well.
	     */
	    scs_send_rej_rsp( sb, p );
	    cb->state = cb_k_closed;
	    krn$_bpost( &cb->control_s );
	    break;

	/*
	 *  The disconnect exchange consists of each side sending a Disconnect
	 *  Request, and responding with a Disconnect Response.  There are
	 *  three possible states, depending on how messages cross on the wire.
	 *  We always send the Disconnect Request first.  If the next thing
	 *  that arrives is a Disconnect Response, we must then wait for the
	 *  Disconnect Request from the other side, and send a Disconnect
	 *  Response.  If instead the next thing to arrive is a Disconnect
	 *  Request, we must send a Disconnect Response and wait for the other
	 *  Disconnect Response to arrive.
	 */
	case scs_k_disc_req:
	    /*
	     *  If the connection is open, then we must be the server side, and
	     *  the client on the other end is disconnecting.  Change our state
	     *  to show the received disconnect request, and send a Disconnect
	     *  Response.  The server on this connection will soon discover the
	     *  disconnect request and will itself disconnect, completing the
	     *  handshake.
	     */
	    if( cb->state == cb_k_open )
		{
		cb->state = cb_k_disc_match;
		scs_send_disc_rsp( cb );
		}
	    /*
	     *  When we receive a Disconnect Request, we respond with a
	     *  Disconnect Response.  If we have not received a Disconnect
	     *  Response yet, then we continue to wait for one, else we
	     *  are done with the disconnect exchange.
	     */
	    else if( cb->state == cb_k_disc_req_out )
		{
		cb->state = cb_k_disc_match;
		scs_send_disc_rsp( cb );
		}
	    else if( cb->state == cb_k_disc_rsp_in )
		{
		scs_send_disc_rsp( cb );
		krn$_bpost( &cb->control_s );
		}
	    else
		{
		fprintf( el_fp, "SCS Disconnect Request received in wrong state\n" );
		sb->break_vc( sb );
		}
	    break;

	case scs_k_disc_rsp:
	    /*
	     *  When we receive a Disconnect Response, we may or may not be
	     *  done, depending on whether we have received a Disconnect
	     *  Request from the other side or not.  If we haven't, we continue
	     *  waiting, while if we have, we're done with the disconnect
	     *  exchange.
	     */
	    if( cb->state == cb_k_disc_req_out )
		cb->state = cb_k_disc_rsp_in;
	    else if( cb->state == cb_k_disc_match )
		krn$_bpost( &cb->control_s );
	    else
		{
		fprintf( el_fp, "SCS Disconnect Response received in wrong state\n" );
		sb->break_vc( sb );
		}
	    break;

#if MSCP_SERVER
	/*
	 *  We tolerate Credit Request messages.  When credits are offered, we
	 *  accept them; if credits are being retracted, we decline to give any
	 *  back.
	 */
	case scs_k_cred_req:
	    {
	    struct scs_cred_req *q = p;

	    /*
	     *  If we get credits, remember to wake any tasks waiting for
	     *  those credits!
	     */
	    if( q->credit > 0 )
		{
		krn$_bpost( &cb->credit_s );
		cb->credits += q->credit;
		}
	    else
		q->credit = 0;
	    scs_send_cred_rsp( cb, q );
	    break;
	    }
#endif

	/*
	 *  Upon receiving an application message, we verify that the
	 *  connection is open, and if so, we fix up the credit count, wake
	 *  any task waiting for credits, and call the application to deliver
	 *  the message.
	 */
	case scs_k_appl_msg:
	    if( cb->state != cb_k_open )
		{
		fprintf( el_fp, "SCS Application Message received in wrong state\n" );
		sb->break_vc( sb );
		}
	    else
		{
		struct scs_appl_msg *q = p;

		cb->credits += q->credit;
		cb->owed_credits++;
		krn$_bpost( &cb->credit_s );
		cb->appl_msg_input( cb, q+1, len - sizeof( *q ) );
		}
	    break;

	/*
	 *  If we don't understand this SCS message, break the VC.
	 */
	default:
	    {
	    fprintf( el_fp, "unrecognized SCS message (%04x) received\n", p->mtype );
	    sb->break_vc( sb );
	    }
	}
    }

/*+
 * ============================================================================
 * = scs_connect - connect to a remote server                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles the SCS connect exchange.
 *  
 * FORM OF CALL:
 *  
 *	scs_connect( sb, driver, dst_proc, src_proc, src_data,
 *		min_credit, appl_msg_input, connection_error )
 *  
 * RETURNS:
 *
 *	A pointer to a CB if a connection is open, or 0 otherwise.
 *       
 * ARGUMENTS:
 *
 *	struct mscp_sb *sb		- system block
 *	int driver			- offset into DDB list (DDB$K_xx)
 *	char *dst_proc			- name of remote server
 *	char *src_proc			- name of local client
 *	char *src_data			- data to pass during connect attempt
 *	int min_credit			- number of credits required
 *	int *appl_msg_input		- routine to call to deliver
 *					  application message input
 *	int *connection_error		- routine to call to notify of a
 *					  connection error
 *
 * SIDE EFFECTS:
 *
 *	A CB may be allocated, and a connection opened.
 *
-*/
int scs_connect( struct mscp_sb *sb, int driver, char *dst_proc, char *src_proc,
	char *src_data, int min_credit, int *appl_msg_input,
	int *connection_error )
    {
    int scs_alloc_appl_msg( );
    int scs_send_appl_msg( );
#if MSCP_SERVER
    int scs_send_data( );
    int scs_request_data( );
#endif
    int scs_disconnect( );
    int scs_break_vc( );
    int scs_discard_cb( );
    struct cb *cb;

    /*
     *  Allocate a CB, and fill in what we can.  The CB contains addresses of
     *  routines in the port driver that a class driver can call, and also
     *  of routines in the class driver that a port driver can call.
     */
    cb = malloc( sizeof( *cb ) );
    cb->sb = sb;
    cb->driver = driver;
    cb->map = sb->map;
    cb->unmap = sb->unmap;
    cb->alloc_appl_msg = scs_alloc_appl_msg;
    cb->send_appl_msg = scs_send_appl_msg;
#if MSCP_SERVER
    cb->send_data = scs_send_data;
    cb->request_data = scs_request_data;
#endif
    cb->disconnect = scs_disconnect;
    cb->break_vc = scs_break_vc;
    cb->discard_cb = scs_discard_cb;
    cb->appl_msg_input = appl_msg_input;
    cb->connection_error = connection_error;
    cb->state = cb_k_con_req_out;
    cb->ref = 0;
    cb->msgs = 0;
    cb->ub.flink = &cb->ub.flink;
    cb->ub.blink = &cb->ub.flink;
    cb->rsp.flink = &cb->rsp.flink;
    cb->rsp.blink = &cb->rsp.flink;
    cb->receive_q.flink = &cb->receive_q.flink;
    cb->receive_q.blink = &cb->receive_q.flink;
    krn$_seminit( &cb->receive_s, 0, "appl_receive" );
    krn$_seminit( &cb->control_s, 0, "scs_control" );
    krn$_seminit( &cb->credit_s, 0, "scs_credit" );
    insq( cb, sb->cb.blink );
    /*
     *  Send a Connect Request, and wait for the connection to either open,
     *  or else fail.
     */
    cb->control_ip = 1;
    scs_send_con_req( cb, dst_proc, src_proc, src_data, min_credit );
    krn$_post_wait( &scs_lock, &cb->control_s );
    krn$_wait( &scs_lock );
    cb->control_ip = 0;
    if( ( cb->state == cb_k_open ) || ( cb->state == cb_k_disc_match ) )
	{
	/*
	 *  If the connection is open, then let the PPD layer know.
	 */
	if( sb->conn_open )
	    sb->conn_open( cb, sb );
	return( cb );
	}
    else
	{
	/*
	 *  If the connection is NOT open, we should discard the CB.
	 */
	scs_discard_cb( cb );   
	return( 0 );
	}
    }

#if MSCP_SERVER
/*+
 * ============================================================================
 * = scs_accept - accept from a remote client                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles the SCS accept exchange.
 *  
 * FORM OF CALL:
 *  
 *	scs_accept( sb, cb, dst_proc, src_proc, src_data,
 *		min_credit, appl_msg_input, connection_error )
 *  
 * RETURNS:
 *
 *	A pointer to a CB if a connection is open, or 0 otherwise.
 *       
 * ARGUMENTS:
 *
 *	struct mscp_sb *sb		- system block
 *	struct cb *cb			- skeleton connection block
 *	char *dst_proc			- name of remote server
 *	char *src_proc			- name of local client
 *	char *src_data			- data to pass during connect attempt
 *	int min_credit			- number of credits required
 *	int *appl_msg_input		- routine to call to deliver
 *					  application message input
 *	int *connection_error		- routine to call to notify of a
 *					  connection error
 *
 * SIDE EFFECTS:
 *
 *	A CB may be allocated, and a connection opened.
 *
-*/
int scs_accept( struct mscp_sb *sb, struct cb *cb, char *src_proc, char *src_data,
	int min_credit, int *appl_msg_input, int *connection_error )
    {
    int scs_alloc_appl_msg( );
    int scs_send_appl_msg( );
    int scs_send_data( );
    int scs_request_data( );
    int scs_disconnect( );
    int scs_break_vc( );
    int scs_discard_cb( );

    /*
     *  Using the given CB, fill in what we can.  The CB contains addresses of
     *  routines in the port driver that a class driver can call, and also
     *  of routines in the class driver that a port driver can call.
     */
    cb->sb = sb;
    cb->map = sb->map;
    cb->unmap = sb->unmap;
    cb->alloc_appl_msg = scs_alloc_appl_msg;
    cb->send_appl_msg = scs_send_appl_msg;
    cb->send_data = scs_send_data;
    cb->request_data = scs_request_data;
    cb->disconnect = scs_disconnect;
    cb->break_vc = scs_break_vc;
    cb->discard_cb = scs_discard_cb;
    cb->appl_msg_input = appl_msg_input;
    cb->connection_error = connection_error;
    cb->state = cb_k_acc_req_out;
    cb->ref = 0;
    cb->msgs = 0;
    cb->ub.flink = &cb->ub.flink;
    cb->ub.blink = &cb->ub.flink;
    cb->rsp.flink = &cb->rsp.flink;
    cb->rsp.blink = &cb->rsp.flink;
    cb->receive_q.flink = &cb->receive_q.flink;
    cb->receive_q.blink = &cb->receive_q.flink;
    krn$_seminit( &cb->receive_s, 0, "appl_receive" );
    krn$_seminit( &cb->control_s, 0, "scs_control" );
    krn$_seminit( &cb->credit_s, 0, "scs_credit" );
    insq( cb, sb->cb.blink );
    /*
     *  Send an Accept Request, and wait for the connection to either open,
     *  or else fail.
     */
    cb->control_ip = 1;
    scs_send_acc_req( cb, cb->dst_proc, src_proc, src_data, min_credit );
    krn$_post_wait( &scs_lock, &cb->control_s );
    krn$_wait( &scs_lock );
    cb->control_ip = 0;
    if( ( cb->state == cb_k_open ) || ( cb->state == cb_k_disc_match ) )
	{
	/*
	 *  If the connection is open, then let the PPD layer know.
	 */
	if( sb->conn_open )
	    sb->conn_open( cb, sb );
	return( cb );
	}
    else
	{
	/*
	 *  If the connection is NOT open, we should discard the CB.
	 */
	scs_discard_cb( cb );   
	return( 0 );
	}
    }
#endif

/*+
 * ============================================================================
 * = scs_disconnect - disconnect from a remote server                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles the SCS disconnect exchange.
 *  
 * FORM OF CALL:
 *  
 *	scs_disconnect( cb )
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
 *	The CB is deallocated, and the connection closed.
 *
-*/
void scs_disconnect( struct cb *cb )
    {
    /*
     *  Send a Disconnect Request, and wait for the connection to close.
     */
    cb->control_ip = 1;
    if( cb->state == cb_k_open )
	cb->state = cb_k_disc_req_out;
    scs_send_disc_req( cb );
    krn$_post_wait( &scs_lock, &cb->control_s );
    krn$_wait( &scs_lock );
    cb->control_ip = 0;
    /*
     *  Once the connection is closed, discard the CB.
     */
    scs_discard_cb( cb );
    }

/*+
 * ============================================================================
 * = scs_discard_cb - discard a CB                                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine discards a CB after releasing all allocated resources.
 *  
 * FORM OF CALL:
 *  
 *	scs_discard_cb( cb )
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
 *	The CB may be deallocated.
 *
-*/
void scs_discard_cb( struct cb *cb )
    {
    struct mscp_sb *sb;

    /*
     *  If there is a control sequence in progress, we ignore this call to
     *  dispose of the CB, relying on the fact that we will be called again
     *  after the control sequence completes.
     */
    sb = cb->sb;
    if( cb->control_ip )
	{
	if( !cb->dequeued )
	    {
	    remq( cb );
	    cb->dequeued = 1;
	    }
	return;
	}
    if( sb->conn_close )
	sb->conn_close( cb, sb );
    /*
     *  All done, discard the CB.
     */
    krn$_semrelease( &cb->receive_s );
    krn$_semrelease( &cb->control_s );
    krn$_semrelease( &cb->credit_s );
    if( !cb->dequeued )
	remq( cb );
    free( cb );
    }

/*+
 * ============================================================================
 * = scs_break_vc - break a VC                                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine breaks a VC, likely due to some fatal error.  This SCS
 *	routine exists solely due to layering considerations.  It directly
 *	calls the corresponding PPD routine.
 *  
 * FORM OF CALL:
 *  
 *	scs_break_vc( cb )
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
 *	The VC is broken.
 *
-*/
void scs_break_vc( struct cb *cb )
    {
    struct mscp_sb *sb;

    sb = cb->sb;
    sb->break_vc( sb );
    }

/*+
 * ============================================================================
 * = scs_send_con_req - send an SCS Connect Request message                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Connect Request message.
 *  
 * FORM OF CALL:
 *  
 *	scs_send_con_req( cb, dst_proc, src_proc, src_data, min_credit )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	char *dst_proc			- name of remote server
 *	char *src_proc			- name of local client
 *	char *src_data			- data to pass during connect attempt
 *	int min_credit			- number of credits required
 *
 * SIDE EFFECTS:
 *
-*/
void scs_send_con_req( struct cb *cb, char *dst_proc, char *src_proc,
	char *src_data, int min_credit )
    {
    struct mscp_sb *sb;
    struct scs_con_req *p;

    sb = cb->sb;
    p = sb->alloc( sb, sizeof( *p ) );
    p->mtype = scs_k_con_req;
    p->credit = 4;
    p->dst_id = 0;
    p->src_id = cb;
    p->min_credit = min_credit;
    p->mbz = 0;
    memcpy( p->dst_proc, dst_proc, sizeof( p->dst_proc ) );
    memcpy( p->src_proc, src_proc, sizeof( p->src_proc ) );
    memcpy( p->src_data, src_data, sizeof( p->src_data ) );
    sb->send_scs_msg( sb, p );
    }

/*+
 * ============================================================================
 * = scs_send_con_rsp - send an SCS Connect Response message                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Connect Response message, with
 *	an appropriate status.
 *  
 * FORM OF CALL:
 *  
 *	scs_send_con_rsp( sb, q )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct mscp_sb *sb		- system block
 *	struct scs_con_req *q		- pointer to SCS Connect Request
 *
 * SIDE EFFECTS:
 *
-*/
void scs_send_con_rsp( struct mscp_sb *sb, struct scs_con_req *q )
    {
    int i;
    struct scs_con_rsp *p;
    struct cb *cb;

    p = sb->alloc( sb, sizeof( *p ) );
    p->mtype = scs_k_con_rsp;
    p->credit = 0;
    p->dst_id = q->src_id;
    p->src_id = 0;
    p->mbz = 0;
#if MSCP_SERVER
    /*
     *  Search through the table of known servers, looking for a match.
     */
    for( i = 0; i < max_servers; i++ )
	if( strncmp( q->dst_proc, servers[i].src_proc, 16 ) == 0 )
	    break;
    /*
     *  If a match is found, then say so when we send the Connect Response,
     *  and notify the server.  Else, say we failed.
     */
    if( i < max_servers )
	{
	p->status = scs_k_normal;
	sb->send_scs_msg( sb, p );
	cb = malloc( sizeof( *cb ) );
	cb->credits = q->credit;
	cb->dst_id = q->src_id;
	memcpy( cb->dst_proc, q->src_proc, sizeof( cb->dst_proc ) );
	servers[i].notify( sb, cb );
	}
    else
	{
	p->status = scs_k_nomatch;
	sb->send_scs_msg( sb, p );
	}
#else
    p->status = scs_k_nomatch;
    sb->send_scs_msg( sb, p );
#endif
    }

#if MSCP_SERVER
/*+
 * ============================================================================
 * = scs_send_acc_req - send an SCS Accept Request message                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Accept Request message.
 *  
 * FORM OF CALL:
 *  
 *	scs_send_con_req( cb, dst_proc, src_proc, src_data, min_credit )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	char *dst_proc			- name of remote server
 *	char *src_proc			- name of local client
 *	char *src_data			- data to pass during connect attempt
 *	int min_credit			- number of credits required
 *
 * SIDE EFFECTS:
 *
-*/
void scs_send_acc_req( struct cb *cb, char *dst_proc, char *src_proc,
	char *src_data, int min_credit )
    {
    struct mscp_sb *sb;
    struct scs_acc_req *p;

    sb = cb->sb;
    p = sb->alloc( sb, sizeof( *p ) );
    p->mtype = scs_k_acc_req;
    p->credit = 4;
    p->dst_id = cb->dst_id;
    p->src_id = cb;
    p->min_credit = min_credit;
    p->mbz = 0;
    memcpy( p->dst_proc, dst_proc, sizeof( p->dst_proc ) );
    memcpy( p->src_proc, src_proc, sizeof( p->src_proc ) );
    memcpy( p->src_data, src_data, sizeof( p->src_data ) );
    sb->send_scs_msg( sb, p );
    }
#endif

/*+
 * ============================================================================
 * = scs_send_acc_rsp - send an SCS Accept Response message                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Accept Response message.
 *  
 * FORM OF CALL:
 *  
 *	scs_send_acc_rsp( cb )
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
void scs_send_acc_rsp( struct cb *cb )
    {
    struct mscp_sb *sb;
    struct scs_acc_rsp *p;

    sb = cb->sb;
    p = sb->alloc( sb, sizeof( *p ) );
    p->mtype = scs_k_acc_rsp;
    p->credit = 0;
    p->dst_id = cb->dst_id;
    p->src_id = cb;
    p->mbz = 0;
    p->status = scs_k_normal;
    sb->send_scs_msg( sb, p );
    }

/*+
 * ============================================================================
 * = scs_send_rej_rsp - send an SCS Reject Response message                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Reject Response message.
 *  
 * FORM OF CALL:
 *  
 *	scs_send_rej_rsp( sb, q )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct mscp_sb *sb		- system block
 *	struct scs_rej_req *q		- pointer to SCS Reject Request
 *
 * SIDE EFFECTS:
 *
-*/
void scs_send_rej_rsp( struct mscp_sb *sb, struct scs_rej_req *q )
    {
    struct scs_rej_rsp *p;

    p = sb->alloc( sb, sizeof( *p ) );
    p->mtype = scs_k_rej_rsp;
    p->credit = 0;
    p->dst_id = q->src_id;
    p->src_id = 0;
    sb->send_scs_msg( sb, p );
    }

/*+
 * ============================================================================
 * = scs_send_disc_req - send an SCS Disconect Request message                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Disconnect Request message.
 *  
 * FORM OF CALL:
 *  
 *	scs_send_disc_req( cb )
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
void scs_send_disc_req( struct cb *cb )
    {
    struct mscp_sb *sb;
    struct scs_disc_req *p;

    sb = cb->sb;
    p = sb->alloc( sb, sizeof( *p ) );
    p->mtype = scs_k_disc_req;
    p->credit = 0;
    p->dst_id = cb->dst_id;
    p->src_id = cb;
    p->mbz = 0;
    p->reason = scs_k_normal;
    sb->send_scs_msg( sb, p );
    }

/*+
 * ============================================================================
 * = scs_send_disc_rsp - send an SCS Disconnect Response message              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Disconnect Response message.
 *  
 * FORM OF CALL:
 *  
 *	scs_send_disc_rsp( cb )
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
void scs_send_disc_rsp( struct cb *cb )
    {
    struct mscp_sb *sb;
    struct scs_disc_rsp *p;

    sb = cb->sb;
    p = sb->alloc( sb, sizeof( *p ) );
    p->mtype = scs_k_disc_rsp;
    p->credit = 0;
    p->dst_id = cb->dst_id;
    p->src_id = cb;
    sb->send_scs_msg( sb, p );
    }

#if MSCP_SERVER
/*+
 * ============================================================================
 * = scs_send_cred_rsp - send an SCS Credit Response message                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Credit Response message.
 *  
 * FORM OF CALL:
 *  
 *	scs_send_cred_rsp( cb, q )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct scs_cred_req *q		- pointer to SCS Credit Request
 *
 * SIDE EFFECTS:
 *
-*/
void scs_send_cred_rsp( struct cb *cb, struct scs_cred_req *q )
    {
    struct mscp_sb *sb;
    struct scs_cred_rsp *p;

    sb = cb->sb;
    p = sb->alloc( sb, sizeof( *p ) );
    p->mtype = scs_k_cred_rsp;
    p->credit = q->credit;
    p->dst_id = cb->dst_id;
    p->src_id = cb;
    sb->send_scs_msg( sb, p );
    }
#endif

/*+
 * ============================================================================
 * = scs_send_appl_msg - send an SCS Application Message                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Application Message.
 *  
 * FORM OF CALL:
 *  
 *	scs_send_appl_msg( cb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct scs_appl_msg *p		- pointer to SCS Application Message
 *
 * SIDE EFFECTS:
 *
-*/
void scs_send_appl_msg( struct cb *cb, struct scs_appl_msg *p )
    {
    int credits;
    struct mscp_sb *sb;

    if( cb->owed_credits > 0 )
	{
	credits = 1 + cb->owed_credits;
	cb->owed_credits = 0;
	}
    else
	credits = 1;
    sb = cb->sb;
    --p;
    p->mtype = scs_k_appl_msg;
    p->credit = credits;
    p->dst_id = cb->dst_id;
    p->src_id = cb;
    sb->send_scs_msg( sb, p );
    --cb->owed_credits;
    }

#if MSCP_SERVER
/*+
 * ============================================================================
 * = scs_send_data - send SCS data                                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initiates an SCS Block Data Transfer.
 *  
 * FORM OF CALL:
 *  
 *	scs_send_data( cb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct SEMAPHORE *sem		- semaphore to be posted
 *	int len				- transaction length
 *	int *snd			- pointer to sender's name/offset
 *	int *rec			- pointer to receiver's name/offset
 *
 * SIDE EFFECTS:
 *                                                 
-*/
void scs_send_data( struct cb *cb, struct SEMAPHORE *sem,
	int len, int *snd, int *rec )
    {
    struct mscp_sb *sb;

    sb = cb->sb;
    sb->send_data( sb, sem, len, snd, rec );
    }

/*+
 * ============================================================================
 * = scs_request_data - request SCS data                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initiates an SCS Block Data Transfer.
 *  
 * FORM OF CALL:
 *  
 *	scs_request_data( cb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct SEMAPHORE *sem		- semaphore to be posted
 *	int len				- transaction length
 *	int *snd			- pointer to sender's name/offset
 *	int *rec			- pointer to receiver's name/offset
 *
 * SIDE EFFECTS:
 *
-*/
void scs_request_data( struct cb *cb, struct SEMAPHORE *sem,
	int len, int *snd, int *rec )
    {
    struct mscp_sb *sb;

    sb = cb->sb;
    sb->request_data( sb, sem, len, snd, rec );
    }
#endif
