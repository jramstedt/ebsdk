/*
 * cipca_response.c
 *
 * Copyright (C) 1992 by
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
 * Abstract:	CIXCD and CIMNA MSCP protocol
 *
 * Author:	John R. Kirchoff, Charlie Crabb
 *
 * Modifications:
 *
 *	jrk	04-Oct-1994	Derived from Laser
 *
 */

#define DEBUG 0
#define DEBUG_MALLOC 0

/* Length of the Free Queue Entries */

#define DFQE_SIZE 512

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:scs_def.h"
#define CIPCA 1
#include "cp$src:sb_def.h"
#include "cp$src:cb_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:server_def.h"
#include "cp$src:mscp_def.h"
#include "cp$src:cipca_sa_ppd_def.h"
#include "cp$src:print_var.h"
#include "cp$src:nport_q_def.h"
#include "cp$src:nport_def.h"
#include "cp$src:cipca_def.h"
#include "cp$src:ci_cipca_def.h"
#include "cp$src:cipca_debug_rtns.h"
#include "cp$inc:prototypes.h"
#include "cp$inc:kernel_entry.h"

#if DEBUG
extern char *poll_active_txt[];
extern int ci_debug_flag;
#define dqprintf _dqprintf
#else
#define dqprintf qprintf
#endif

#if DEBUG
#define dprintf(fmt, args)	\
    _dprintf(fmt, args)
#define d2printf(fmt, args)	\
    _d2printf(fmt, args)
#else
#define dprintf(fmt, args)
#define d2printf(fmt, args)
#endif




/*+
 * ============================================================================
 * = ci_mscp_response - process a port response                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine processes the ppd portion of a single port response 
 *	packet.  It does so for both XCD and NPORT CI queues.
 *  
 * FORM OF CALL:
 *  
 *	ci_mscp_response( sb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *	struct ppd_header *p		- pointer to the CI packet (opcode +
 *					  PPD portion.
 *
 * SIDE EFFECTS:
 *
 *	A VC may be closed.  A VC may be opened.  Lots of other possibilities
 *	exist.
 *
-*/
void ci_mscp_response(struct cipca_sb *sb, struct ppd_header *p)
{
    int status;
    char *cmd = p + 1;			/* pointer to PPD portion of NPORT
					 * queue buffer */
/*
 * Dispatch to opcode-specific processing unless ERR is set.
 * If ERR is set and a VC is active, tear down the VC.
 * If return of IDREQ with error, indicate no path, stop poll.
 */

    switch (p->opcode) {
/*
* This is a received datagram.  Dispatch on the PPD
* message type field.  Any that we don't care about are
* ignored (they are datagrams, after all).
*/
	case ppd_k_dgrec: 
	    {
		struct ppd_dg *q = cmd;

		dprintf("ppd_k_dgrec:%d\n",p_args1(sb->node_id));

		switch (q->mtype) {
/*
* Upon receiving a START, we close any existing
* VC, open a new VC, and send back a STACK.
*/
		    case ppd_k_start: 
			if (!sb->stallvc) {

			    if (sb->dqi) {
				dprintf("  DGREC start: Send dqi to  %s %d\n",
				  p_args2(sb->name, sb->node_id));
				ci_sa_ppd_send_dqi(sb);
			    } else {
				dprintf("ppd_k_start: requested to start VC on %s %d\n",
				  p_args2(sb->name, sb->node_id));

				if (!sb->vc) {
				    ci_sa_ppd_open_vc(sb);
				    ci_sa_ppd_send_dg_stack(sb);
				    ci_sa_ppd_new_node(sb, q);
				} else {
				   if (sb->ackrec) {
			              ci_sa_ppd_break_vc(sb);
                                   } else {
				      ci_sa_ppd_send_dg_stack(sb);
				   }
                                }

			    }

			} else {
			    dprintf("start received from %s but stallvc set\n",
                                      p_args1(sb->name));
                        }

			break;
/*
* Upon receiving a STACK, we send back an ACK, and open the VC.
* If node is an HSC, notify mscp, otherwise indicate
* polling complete ok.
*/
		    case ppd_k_stack: 
		        dprintf("ppd_k_stack:%d  sb->vc:%x \n",
                               p_args2(sb->node_id, sb->vc));
			if (!sb->vc)
			    ci_sa_ppd_open_vc(sb);
			ci_sa_ppd_send_dg_ack(sb);
			ci_sa_ppd_new_node(sb, q);
		        if (!sb->ackrec){
			  sb->ackrec = 1;
			  mscp_new_node(sb);
                        }
			break;

/*
* Upon receiving an ACK, we create a new node.
*/
		    case ppd_k_ack: 
			dprintf("ppd_k_ack:%d sb->vc:%x  ackrec:%x " \
                                        "poll_active:%s\n",
                            p_args4(sb->node_id,
                                    sb->vc,
                                    sb->ackrec,
                                    poll_active_txt[sb->poll_active]));
			if (!sb->ackrec)
			    if (sb->poll_active == POLL_WAITING) {
					/* if not done, i.e. waiting*/
				sb->ackrec = 1;
				mscp_new_node(sb);
			    } else {
			} else {
			}
			break;

/* Error log */
		    case ppd_k_error_log: 
			dprintf("ppd_k_error_log:%d\n",p_args1(sb->node_id));
			dprintf("CI Error_log, node %d\n", p->src_port);
			break;
		}
		break;
	    }
/*
* This is a received message.  Dispatch on the PPD
* message  type field if and only if a VC is open.
*/
	case ppd_k_msgrec: 
	    {
		struct ppd_msg *q = cmd;

		dprintf("ppd_k_msgrec:%d\n",p_args1(sb->node_id));

		if (!sb->vc)
		    break;

		switch (q->mtype) {

/*
* A received SCS message is passed up to a higher
* layer to process.
*/
		    case ppd_k_scs_msg: 
	                dprintf("ppd_k_scs_msg:%d\n",p_args1(sb->node_id));
			ci_sa_scs_receive(sb, q + 1, q->length);
			break;

/* Unknown message types are fatal to the VC. */

		    default: 
			dprintf("default:%d\n",p_args1(sb->node_id));
			dqprintf("unrecognized PPD message received\n");
			ci_sa_ppd_break_vc(sb);
			break;
		}
		break;
	    }

/* This is a confirmation of data sent.  Process it if and 
* only if a VC is open.
*/
	case ppd_k_cnfrec: 
	    {
		struct ppd_cnf *q = cmd;

		dprintf("ppd_k_cnfref:%d\n",p_args1(sb->node_id));
		if (!sb->vc)
		    break;
		krn$_post(q->xct_id[1]);
		break;
	    }

/*
** If this is a node we want, then start the 
** start-stack-ack protocol to form a VC with
** it.  Else, inhibit datagram activity with it.
*/
	case ppd_k_idrec: 
	    {
		int st, rport_typ;
		struct ppd_id *q = cmd;
		XPORT_FCN_EXT *fext = q->xport_fcn_ext;


		dprintf("ppd_k_idrec:%d   sb:%x\n",p_args2(sb->node_id,sb));
	 	sb->stallvc = 0;
	        sb->ok = 1;
		rport_typ = q->rport_typ & 0x7fffffff;
		if (st = ci_check_maint_id(rport_typ) != msg_success) {
		    sb->poll_active = POLL_NOPATH;
					/* do not wait for him any more */
		    sb->dqi = 1;
		    ci_sa_ppd_send_dqi(sb);
		     dprintf("  IDREC: unwanted node %s %d, MAINTid: %x\n",
		      p_args3(sb->name, sb->node_id, rport_typ));
		} else {
		    sb->dqi = 0;
		    sb->poll_delay = 5;
		    memcpy(sb->xport_fcn_ext, q->xport_fcn_ext,
		      sizeof(sb->xport_fcn_ext));
		    if (!sb->vc) {
			dprintf("  IDREC    Send dg start to  %s %d, MAINTid: %x\n",
			  p_args3(sb->name, sb->node_id, rport_typ));

			ci_sa_ppd_send_dg_start(sb);
		    } else {
 		        dprintf("IDREC from %s; sb->vc open;  NO start sent\n",
                                     p_args1(sb->name));
		    }
		}
		break;
	    }

/*
* This is a confirmation of data received.  Process it if
* and only if a VC is open.
*/
	case ppd_k_datrec: 
	    {
		struct ppd_dat *q = cmd;

		dprintf("ppk_k_datrec:%d\n",p_args1(sb->node_id));
		if (!sb->vc)
		    break;
		krn$_post(q->xct_id[1]);
		break;
	    }
	default: 
	    dprintf("default2:%d\n",p_args1(sb->node_id));
	    dqprintf("Unrecognized opcode %x, packet = %x\n", p->opcode, p);
	    break;
    }
}
/*+
 * ============================================================================
 * = ci_sa_scs_receive - process a received SCS message                       =
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
 *	ci_sa_scs_receive( sb, p, len )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *	struct scs_msg *p		- pointer to SCS message
 *	int len				- length of SCS message
 *
 * SIDE EFFECTS:
 *
 *	The received SCS message is processed.
 *
-*/
void ci_sa_scs_receive(struct cipca_sb *sb, struct scs_msg *p, int len)
{
    struct cb *cb;
    int entry_cb_state;

    dprintf("ci_sa_scs_receive\n",p_args0);

/*
 * Most SCS messages carry a destination ID; if so, it is the CB of  the
 * connection to which the received message pertains.
 */

    cb = p->dst_id;

    entry_cb_state = cb->state;

/*
 * A connection is formed by sending a Connect Request, and waiting
 * for a Connect Response to come back.  If the remote server wants
 * to accept this connection, it will send back an Accept Request, to 
 * which we respond with an Accept Response.  If the remote server
 * doesn't want to accept this connection, it can either send back a  
 * Connect Response with a failure status, or it can send back a good  
 * Connect Response followed by a Reject Request, to which we respond  
 * with a Reject Response.
 */
    switch (p->mtype) {

	case scs_k_con_req: 
	    dprintf("scs_k_con_req:%d\n",p_args1(sb->node_id));
/*
 * We don't support Connect Request, so return a Connect Response 
 * telling the other side to go away.
 */
	    ci_sa_scs_send_con_rsp(sb, p);
	    break;

	case scs_k_con_rsp: 

		dprintf("scs_k_con_rsp:%d\n",p_args1(sb->node_id));
/*
 * We must have already send a Connect Request to the other side  
 * for a Connect Response to be valid.  If we are in the right
 * state, and if the status of the Connect Response is good, then 
 * advance to the appropriate new state, else the attempt to
 * connect has failed, and the CB control semaphore should be
 * posted so that the task connecting can exit.
 */
	    if (cb->state != cb_k_con_req_out) {
		dqprintf("SCS Connect Response received in wrong state\n");
		ci_sa_ppd_break_vc(sb);
	    } else {
		struct scs_con_rsp *q = p;

		if (q->status == scs_k_normal)
		    cb->state = cb_k_con_rsp_in;
		else
		    krn$_bpost(&cb->control_s);
	    }
	    break;

	case scs_k_acc_req: 

	    dprintf("scs_k_acc_req:%d\n",p_args1(sb->node_id));
/*
 * We must have already received a Connect Response from the
 * other side for an Accept Request to be valid.  If we are in
 * the right state, then remember a few things in the CB, say
 * that the connection is open, return an Accept Response, and
 * awaken the task which started the connect attempt.
 */
	    if (cb->state != cb_k_con_rsp_in) {
		dqprintf("SCS Accept Request received in wrong state\n");
		ci_sa_ppd_break_vc(sb);
	    } else {
		struct scs_acc_req *q = p;

		cb->credits = q->credit;
		cb->dst_id = q->src_id;
		cb->state = cb_k_open;
		ci_sa_scs_send_acc_rsp(cb);
		krn$_bpost(&cb->control_s);
	    }
	    break;

	case scs_k_acc_rsp: 

		dprintf("scs_k_acc_rsp:%d\n",p_args1(sb->node_id));
/*
 * We must have already send an Accept Request to the other side  
 * for an Accept Response to be valid.  If we are in the right
 * state, then say the connection is open and awaken the task
 * which started the accept attempt.
 */
	    if (cb->state != cb_k_acc_req_out) {
		dqprintf("SCS Accept Response received in wrong state\n");
		ci_sa_ppd_break_vc(sb);
	    } else {
		struct scs_acc_rsp *q = p;

		cb->state = cb_k_open;
		krn$_bpost(&cb->control_s);
	    }
	    break;

	case scs_k_rej_req: 
            dprintf("scs_k_rej_req:%d\n",p_args1(sb->node_id));

/*
 * If we get a Reject Request, the other side is telling us to  go
 * away.  Do so.  Let the task which originated the attempt at
 * connection exit as well.
 */
	    ci_sa_scs_send_rej_rsp(sb, p);
	    cb->state = cb_k_closed;
	    krn$_bpost(&cb->control_s);
	    break;

/*
 * The disconnect exchange consists of each side sending a Disconnect 
 * Request, and responding with a Disconnect Response.  There are
 * three possible states, depending on how messages cross on the wire.
 * We always send the Disconnect Request first.  If the next thing
 * that arrives is a Disconnect Response, we must then wait for the
 * Disconnect Request from the other side, and send a Disconnect
 * Response.  If instead the next thing to arrive is a Disconnect
 * Request, we must send a Disconnect Response and wait for the other 
 * Disconnect Response to arrive.
 */
	case scs_k_disc_req: 

	   dprintf("scs_k_disc_req:%d\n",p_args1(sb->node_id));
/*
 * If the connection is open, then we must be the server side, and
 * the client on the other end is disconnecting.  Change our state
 * to show the received disconnect request, and send a Disconnect 
 * Response.  The server on this connection will soon discover the 
 * disconnect request and will itself disconnect, completing the
 * handshake.
 * 
 * Additional comment - 03-Nov-1999 - djm:  (also see afw_eco 1306)
 * disc_req are coming in when we are not a server, contrary to the
 * above comment.  The disc_req came in when we paused in SYSBOOT> too 
 * long or in Xdelta too long.  I deleted the disc_rsp that we had
 * been sending, and in its place added a break vc.  I 
 *
 */
	    if (cb->state == cb_k_open) {
		cb->state = cb_k_disc_match;
		dqprintf("SCS Disconnect Request received - break VC\n");
		ci_sa_ppd_break_vc(sb);
	    }

/*
 * When we receive a Disconnect Request, we respond with a
 * Disconnect Response.  If we have not received a Disconnect
 * Response yet, then we continue to wait for one, else we  are 
 * done with the disconnect exchange.
 */
	      else if (cb->state == cb_k_disc_req_out) {
		cb->state = cb_k_disc_match;
		ci_sa_scs_send_disc_rsp(cb);
	    } else if (cb->state == cb_k_disc_rsp_in) {
		ci_sa_scs_send_disc_rsp(cb);
		krn$_bpost(&cb->control_s);
	    } else {
		dqprintf("SCS Disconnect Request received in wrong state\n");
		ci_sa_ppd_break_vc(sb);
	    }
	    break;

	case scs_k_disc_rsp: 

	    dprintf("scs_k_disc_rsp:%d\n",p_args1(sb->node_id));

/*
 * When we receive a Disconnect Response, we may or may not be
 * done, depending on whether we have received a Disconnect
 * Request from the other side or not.  If we haven't, we continue
 * waiting, while if we have, we're done with the disconnect
 * exchange.
 */
	    if (cb->state == cb_k_disc_req_out)
		cb->state = cb_k_disc_rsp_in;
	    else if (cb->state == cb_k_disc_match)
		krn$_bpost(&cb->control_s);
	    else {
		dqprintf("SCS Disconnect Response received in wrong state\n");
		ci_sa_ppd_break_vc(sb);
	    }
	    break;

/*
 * We tolerate Credit Request messages.  If credits are offered, we
 * accept them; if credits are being retracted, we decline to give any
 * back.
 */
	case scs_k_cred_req: 
	    {

		struct scs_cred_req *q = p;

		dprintf("scs_k_cred_req:%d\n",p_args1(sb->node_id));

		/*  If we get credits, remember to wake any tasks waiting for  
		 * those credits! */

		if (q->credit > 0) {
		    krn$_bpost(&cb->credit_s);
		    cb->credits += q->credit;
		} else
		    q->credit = 0;
		ci_sa_scs_send_cred_rsp(cb, q);
		break;
	    }

/*
 * Upon receiving an application message, we verify that the
 * connection is open, and if so, we fix up the credit count, wake
 * any task waiting for credits, and call the application to deliver  
 * the message.
 */
	case scs_k_appl_msg: 

	    dprintf("scs_k_appl_msg:%d\n",p_args1(sb->node_id));
	    if (cb->state != cb_k_open) {
		dqprintf("SCS Application Message received in wrong state\n");
		ci_sa_ppd_break_vc(sb);
	    } else {
		struct scs_appl_msg *q = p;

		cb->credits += q->credit;
                cb->owed_credits++;
#if 0
		pprintf("q->credit:%x  cb->owed_credits:%x\n",
                                         q->credit, cb->owed_credits);
#endif
		krn$_bpost(&cb->credit_s);
		cb->appl_msg_input(cb, q + 1, len - sizeof(*q));
	    }
	    break;

/*  If we don't understand this SCS message, break the VC. */

	default: 
	    {

		dprintf("default3:%d\n",p_args1(sb->node_id));
		dqprintf("unrecognized SCS message (%04x) received\n",
		  p->mtype);
		ci_sa_ppd_break_vc(sb);
	    }
    }
}
