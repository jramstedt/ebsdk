/*
 * cipca_support.c
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
 * Abstract:	CI support routines for Subnode Addressing / Explicit Format
 *
 * Author:	John R. Kirchoff
 *
 * Modifications:
 *
 *	jrk 	04-Oct-1994	Derived from Laser
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
#include "cp$src:nport_q_def.h"
#include "cp$src:nport_def.h"
#include "cp$src:cipca_def.h"
#include "cp$src:ci_cipca_def.h"
#include "cp$src:scs_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:cb_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:mscp_def.h"
#include "cp$src:cipca_sa_ppd_def.h"
#include "cp$src:print_var.h"
#include "cp$inc:prototypes.h"
#include "cp$inc:kernel_entry.h"

#if TURBO
#include "cp$src:mem_def.h"
extern struct window_head config;
#endif


#undef mscp_create_poll
#undef mscp_destroy_poll

int cipca_path = 0;   /* For debug only */

extern struct ZONE *zlist[];
extern struct ci_pb *ci_pbs[10];


/* 1 = top level, 2 = packets, 4 = rec packet type */

#if DEBUG
extern int ci_debug_flag;
#define dqprintf _dqprintf
#define dprintf(fmt, args)	\
    _dprintf(fmt, args)
#define d2printf(fmt, args)	\
    _d2printf(fmt, args)
#else
#define dqprintf qprintf
#define dprintf(fmt, args)
#define d2printf(fmt, args)
#endif

extern struct SEMAPHORE scs_lock;

extern null_procedure();

extern int cixcd_incarnation;

extern cipca_prefix();
#define prefix(x) cipca_prefix(pb,name,x)

int print_new_node = 0;


#if DEBUG 
extern char *poll_active_txt[];
extern char *N_opcodes[];
extern char *ci_opcodes[];
extern char *ci_status_type[];
extern char *ci_status_subtype[];
extern char *ci_status_nopath[];
extern char *scs_state[];
extern char *ci_ppd_mtypes[];
extern char *mscp_opcodes[];
extern char *ci_scs_mtype[];
#endif



#define TRACE 0 || TURBO
#if TRACE
int print_trace_flag = 0;

int enable_trace_flag = 0;             /* Mask                    */
				       /*   Bit 0   -  NBUF trace */
                                       /*   Bit 1   -  sb state   */

int trace_nodefilter = 99;
int trace_adapterfilter = 99;

#define MAX_DATA 13

typedef struct {
   int seqnum;
   int type;
   int node_id;	    /* controller node */
   int time;
   int data[MAX_DATA];
} TRACELOG;

#define TRACE_START 0x900000
TRACELOG *tracepos = TRACE_START;
int trace_seqnum = 0;
int npcnt=0;

int trace_time(){
    unsigned char t[4];

#if TURBO
    t[2] = read_watch_hour();
    t[1] = read_watch_min();
    t[0] = read_watch_sec();
#else
    t[2] = 0;
    t[1] = 0;
    t[0] = 0;
#endif
    t[3] = 'T';
    return *(int*)t;
}

void trace_np(char c, struct ci_pb *pb, N_Q_BUFFER *n_q_buf){
    N_Q_BUF_HEAD *p = &n_q_buf->a;
    N_STATUS *n_status = &p->status;
    char *cmd = n_q_buf->b;
    XPORT *src = &p->src_xport;
    XPORT *dst = &p->dst_xport;
    N_FLAGS *n_flags = &p->flags;
    if (!(enable_trace_flag&1)) return;
    if ((trace_adapterfilter != pb->node_id) && (trace_adapterfilter != 99)){
       return;
    }
    if ( (trace_nodefilter == 99)               || 
         (dst->addr.p_grp == trace_nodefilter)  ||
	 (src->addr.p_grp == trace_nodefilter)  ) 
    {
      tracepos->seqnum = trace_seqnum++;
      tracepos->type = (int)c;
      tracepos->node_id = pb->node_id;
      tracepos->time = trace_time();
      memcpy(&tracepos->data, n_q_buf, MAX_DATA*4 );
      if(print_trace_flag)
        dump_trace_record(tracepos);
      tracepos++;
      npcnt++;
    }
}

void trace_sb(char c, struct ci_pb *pb, struct cipca_sb *sb){
    if (!(enable_trace_flag&2)) return;
    if ((trace_adapterfilter != pb->node_id) && (trace_adapterfilter != 99)){
       return;
    }
    if ( (trace_nodefilter == 99)             || 
	 (sb->node_id == trace_nodefilter)  ) {
       tracepos->seqnum = trace_seqnum++;
       tracepos->type =(int)c;
       tracepos->node_id = pb->node_id;
       tracepos->time = trace_time();
       tracepos->data[0] = sb->node_id;
#if MOORE
       tracepos->data[1] = sb->state;
#else
       tracepos->data[1] = sb->ackrec;
#endif
       tracepos->data[2] = sb->vc;
       tracepos->data[3] = sb->poll_active;
       tracepos->data[4] = sb->stallvc;
       if(print_trace_flag)
         dump_trace_record(tracepos);
       tracepos++;
    }
}

void trace_end(){
    tracepos->seqnum = trace_seqnum++;
    tracepos->type = 'E';
    tracepos->time = trace_time();
    tracepos++;
}

void trace_init(){
    tracepos = TRACE_START;
    trace_seqnum = 0;
    npcnt = 0;
    memset(tracepos,0,0x100000);
}

void dump_trace_record(int *t){
  printf("%04d %c%x %08x %08x %08x %08x %08x %08x %08x %08x\n", 
          t[0],t[1],t[2],t[3],t[4],t[5],t[6],t[7],t[8],t[9],t[10]);
}



void dump_trace(int start, int end){
  TRACELOG *spos = TRACE_START + (start * sizeof(TRACELOG));
  TRACELOG *epos = TRACE_START + (end   * sizeof(TRACELOG));
  TRACELOG *i;
  for (i=spos; i <= epos; i++){
     dump_trace_record(i);
  }

}


#endif


#define COMMAND_MODE 0
#if COMMAND_MODE

int cixcd_node;

#if TURBO
void start_cipca(int n){
    int i;
    struct device *cipca_dev[1];
    struct ci_pb *pb;
    struct cipca_sb *sb;

    find_all_dev(&config, 0x06601095, 1, cipca_dev, 0);
    pprintf("cipca_dev:%x\n",cipca_dev[0]);

    cixcd_setmode(DDB$K_START,cipca_dev[0]);
    pb = ci_pbs[n];

    pprintf("sb:%x\n",pb->pb.sb[cixcd_node]);

    /* disable all nodes */
    krn$_wait(&scs_lock);
    for (i = 0; i < pb->pb.num_sb; i++) {
	sb = pb->pb.sb[i];
	if (i == pb->node_id) {
	    sb->poll_active = POLL_OK;
	    continue;
	}
	sb->dqi = 1;				/* inhibit datagrams*/
	ci_sa_ppd_send_dqi(sb);			/* make sure DQI is right */
    }
    krn$_post(&scs_lock);
}

/* this routine does not work w/ more than one adapter */
void stop_cipca(){
    struct device cipca_dev;
    find_all_dev(&config, 0x06601095, 1, &cipca_dev, 0);
    cixcd_setmode(DDB$K_STOP,cipca_dev);
}
#endif



void dqi_off(int n){
  struct ci_pb *pb = ci_pbs[n];
  struct cipca_sb *sb;

  sb = ci_pbs[n]->pb.sb[cixcd_node];
  krn$_wait(&scs_lock);
  sb->dqi = 0;				/* allow datagrams*/
  ci_sa_ppd_dqi_off(sb);		/* make sure DQI is right */
  krn$_post(&scs_lock);
}


void idreq(int n){
  struct ci_pb *pb = ci_pbs[n];
  struct cipca_sb *sb;

  sb = pb->pb.sb[cixcd_node];
  krn$_wait(&scs_lock);
  ci_sa_ppd_send_idreq(sb); 
  krn$_post(&scs_lock);
}



void cistart(int n){
  struct ci_pb *pb = ci_pbs[n];
  struct cipca_sb *sb;

  sb = pb->pb.sb[cixcd_node];
  krn$_wait(&scs_lock);
  ci_sa_ppd_send_dg_start(sb);
  krn$_post(&scs_lock);
}


void stack(int n){
  struct ci_pb *pb = ci_pbs[n];
  struct cipca_sb *sb;

  sb = pb->pb.sb[cixcd_node];
  krn$_wait(&scs_lock);
  ci_sa_ppd_send_dg_stack(sb);
  krn$_post(&scs_lock);
}


void ack(int n){
  struct ci_pb *pb = ci_pbs[n];
  struct cipca_sb *sb;

  sb = pb->pb.sb[cixcd_node];
  krn$_wait(&scs_lock);
  ci_sa_ppd_send_dg_ack(sb);
  krn$_post(&scs_lock);
}


void open_vc(int n){
  struct ci_pb *pb = ci_pbs[n];
  struct cipca_sb *sb;

  sb = pb->pb.sb[cixcd_node];
  krn$_wait(&scs_lock);
  sb->state = sb_k_open;
  ci_sa_ppd_open_vc(sb);
  krn$_post(&scs_lock);
}



void close_vc(int n){
  struct ci_pb *pb = ci_pbs[n];
  struct cipca_sb *sb;

  sb = pb->pb.sb[cixcd_node];
  krn$_wait(&scs_lock);
  sb->state = sb_k_open;
  ci_sa_ppd_close_vc(sb);
  krn$_post(&scs_lock);
}



void set_sbvc(struct cipca_sb *sb, int value){
   sb->vc = value;
}

void set_sbstate(struct cipca_sb *sb, int value){
   sb->state = value;
}

void set_sbpoll_active(struct cipca_sb *sb, int value){
   sb->poll_active = value;
}

void set_stallvc(struct cipca_sb *sb, int value){
   sb->stallvc= value;
}


void set_ackrec(struct cipca_sb *sb, int value){
   sb->ackrec= value;
}
#endif

#if DEBUG
void ci_sa_ppd_dqi_off(struct cipca_sb *sb)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    N_SETCKT *p;

    sb->dqi = 0;
    sb->vc = 0;
    nbuf = ci_sa_ppd_alloc(sb, 0);
    ncmd = &nbuf->a;
    p = nbuf->b;

    ncmd->opc = SETCKT;

    p->mask.dqi = 0x1;

    p->m_value.dqi = 0x0;

    ci_sa_queue_to_send(sb, nbuf);
}



#endif





/*+
 * ============================================================================
 * = ci_sa_scs_alloc - allocate an SCS message packet                           =
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
 *	ci_sa_scs_alloc( sb, len )
 *  
 * RETURNS:
 *
 *	A pointer to the SCS portion of the allocated packet.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *	int len				- number of SCS bytes to allocate
 *
 * SIDE EFFECTS:
 *
-*/
int ci_sa_scs_alloc(struct cipca_sb *sb, int len)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct ppd_msg *p;

/*  Allocate a temporary message packet. */

    nbuf = ci_sa_ppd_alloc(sb, 0);
    if (nbuf == NULL)
	return NULL;
    ncmd = &nbuf->a;
    p = nbuf->b;

/*
 * Remember the size of the allocated packet.  The length is stored in
 * the PPD header, and covers the SCS message plus that portion of the
 * PPD packet which actually gets transmitted to the other node (starting 
 * with the MTYPE field).
 */
    ncmd->i_opc = SNDMSG;
    p->length = len + sizeof(*p) - offsetof(struct ppd_msg, mtype);

    dprintf("     CI SCS ALLOC len %d\n", p_args1(p->length));

/* Return a pointer to the SCS portion, which is just past the PPD portion. */

    return (p + 1);
}

/*+  
 * ============================================================================
 * = ci_sa_scs_alloc_appl_msg - allocate an application message packet          =
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
 *	ci_sa_scs_alloc_appl_msg( cb, len )
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
int ci_sa_scs_alloc_appl_msg(struct cb *cb, int len)
{
    struct scs_appl_msg *p;

/*
 * We must hold a credit in order to send a message, and the credit
 * accounting is done when the message is allocated.  If we don't have  a 
 * credit at the moment, we suspend ourselves, to be awakened when  a
 * received message carries a credit back to us.  Eventually we'll  have
 * the credit we need.
 */

    dprintf("\nscs_alloc_appl_msg\n", p_args0);

    while (cb->credits < 1) {

	dprintf("scs_alloc_appl_msg: wait on credit\n", p_args0);

	krn$_post_wait(&scs_lock, &cb->credit_s);
     	krn$_wait(&scs_lock);
	if (cb->state != cb_k_open)
	    break;
    }
    --cb->credits;

/*
 * Allocate an SCS packet which is large enough to hold the application
 * message.
 */
    p = ci_sa_scs_alloc(cb->sb, sizeof(*p) + len);

/*
 * Return a pointer to the application portion, which is just past  the
 * SCS portion.  Zero the application portion.
 */
    return (memset(p + 1, 0, len));
}

/*+
 * ============================================================================
 * = ci_sa_scs_connect - connect to a remote server                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles the SCS connect exchange.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_connect( sb, driver, dst_proc, src_proc, src_data,
 *		min_credit, appl_msg_input, connection_error )
 *  
 * RETURNS:
 *
 *	A pointer to a CB if a connection is open, or 0 otherwise.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
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
int ci_sa_scs_connect(struct cipca_sb *sb, int driver, char *dst_proc, char *src_proc,
  char *src_data, int min_credit, int *appl_msg_input, int *connection_error)
{
    int ci_sa_scs_alloc_appl_msg();
    int ci_scs_map();
    int ci_sa_scs_send_appl_msg();
    int ci_sa_scs_send_data();
    int ci_sa_scs_request_data();
    int ci_scs_unmap();
    int ci_sa_scs_disconnect();
    int ci_sa_scs_break_vc();
    int ci_sa_scs_discard_cb();
    struct cb *cb;
    struct ci_pb *pb;
    struct cb *return_cb;

    dprintf("\nci_sa_scs_connect\n", p_args0);

/*
 * Allocate a CB, and fill in what we can.  The CB contains addresses of  
 * routines in the port driver that a class driver can call, and also  of
 * routines in the class driver that a port driver can call.
 */
     
    cb = malloc_z(sizeof(*cb));
    cb->sb = sb;
    cb->driver = driver;
    cb->alloc_appl_msg = ci_sa_scs_alloc_appl_msg;
    cb->map = ci_scs_map;
    cb->send_appl_msg = ci_sa_scs_send_appl_msg;
    cb->send_data = ci_sa_scs_send_data;
    cb->request_data = ci_sa_scs_request_data;
    cb->unmap = ci_scs_unmap;
    cb->disconnect = ci_sa_scs_disconnect;
    cb->break_vc = ci_sa_scs_break_vc;
    cb->discard_cb = ci_sa_scs_discard_cb;
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

    krn$_seminit(&cb->receive_s, 0, "appl_receive");
    krn$_seminit(&cb->control_s, 0, "scs_control");
    krn$_seminit(&cb->credit_s, 0, "scs_credit");

    insq(cb, sb->cb.blink);

/*
 * Send a Connect Request, and wait for the connection to either open,  or
 * else fail.
 */

    cb->control_ip = 1;

    ci_sa_scs_send_con_req(cb, dst_proc, src_proc, src_data, min_credit);

    krn$_post_wait(&scs_lock, &cb->control_s);

    krn$_wait(&scs_lock);

      dprintf("ci_sa_scs_conn: send con req - got it: %x\n",
                                p_args1(((struct mscp_sb *)cb->sb)->node_id));

    cb->control_ip = 0;

/*
 * If the connection is open, then allocate ten messages for use by this  
 * connection, and give them to the port.  The messages are permanent,
 * meaning that if the port uses them and puts them onto the response
 * queue, we give them right back to the port instead of freeing them.  If
 * the connection is not open, we undo part of the CB initialization  and
 * free the CB.
 */
    if ((cb->state == cb_k_open) || (cb->state == cb_k_disc_match)) {

	dprintf("ci_sa_scs_connect - open\n", p_args0);

	pb = sb->pb;

	cb->msgs = XZA_MSGFREE;		/* allocated in nport init */

	return_cb = cb;
	goto exit_ci_sa_scs_connect;
    }

/*  If the connection is NOT open, we should discard the CB. */

    dprintf("ci_sa_scs_connect - discard\n", p_args0);

    ci_sa_scs_discard_cb(cb);
    return_cb = 0;

exit_ci_sa_scs_connect: 
    return (return_cb);
}    

/*+
 * ============================================================================
 * = ci_sa_scs_accept - accept from a remote client                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles the SCS accept exchange.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_accept( sb, cb, dst_proc, src_proc, src_data,
 *		min_credit, appl_msg_input, connection_error )
 *  
 * RETURNS:
 *
 *	A pointer to a CB if a connection is open, or 0 otherwise.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
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
int ci_sa_scs_accept(struct cipca_sb *sb, struct cb *cb, char *src_proc, char *src_data,
  int min_credit, int *appl_msg_input, int *connection_error)
{
    int ci_sa_scs_alloc_appl_msg();
    int ci_scs_map();
    int ci_sa_scs_send_appl_msg();
    int ci_sa_scs_send_data();
    int ci_sa_scs_request_data();
    int ci_scs_unmap();
    int ci_sa_scs_disconnect();
    int ci_sa_scs_break_vc();
    struct ci_pb *pb;
    struct cb *ci_sa_scs_accept_retval;

    dprintf("ci_sa_scs_accept\n", p_args0);
/*
 * Using the given CB, fill in what we can.  The CB contains addresses of 
 * routines in the port driver that a class driver can call, and also  of
 * routines in the class driver that a port driver can call.
 */

    cb->sb = sb;
    cb->alloc_appl_msg = ci_sa_scs_alloc_appl_msg;
    cb->map = ci_scs_map;
    cb->send_appl_msg = ci_sa_scs_send_appl_msg;
    cb->send_data = ci_sa_scs_send_data;
    cb->request_data = ci_sa_scs_request_data;
    cb->unmap = ci_scs_unmap;
    cb->disconnect = ci_sa_scs_disconnect;
    cb->break_vc = ci_sa_scs_break_vc;
    cb->appl_msg_input = appl_msg_input;
    cb->connection_error = connection_error;
    cb->state = cb_k_acc_req_out;
    cb->ref = 0;
    cb->ub.flink = &cb->ub.flink;
    cb->ub.blink = &cb->ub.flink;
    cb->rsp.flink = &cb->rsp.flink;
    cb->rsp.blink = &cb->rsp.flink;
    cb->receive_q.flink = &cb->receive_q.flink;
    cb->receive_q.blink = &cb->receive_q.flink;
    krn$_seminit(&cb->receive_s, 0, "appl_receive");
    krn$_seminit(&cb->control_s, 0, "scs_control");
    krn$_seminit(&cb->credit_s, 0, "scs_credit");
    insq(cb, sb->cb.blink);

/*
 * Send an Accept Request, and wait for the connection to either open,  or
 * else fail.
 */
    cb->control_ip = 1;
    ci_sa_scs_send_acc_req(cb, cb->dst_proc, src_proc, src_data, min_credit);
    krn$_post_wait(&scs_lock, &cb->control_s);
    krn$_wait(&scs_lock);

/*
 * If the connection is open, then allocate ten messages for use by this  
 * connection, and give them to the port.  The messages are permanent,
 * meaning that if the port uses them and puts them onto the response
 * queue, we give them right back to the port instead of freeing them.  If
 * the connection is not open,  we undo part of the CB initialization  and 
 * free the CB.
 */
    if ((cb->state == cb_k_open) || (cb->state == cb_k_disc_match)) {

	dprintf("ci_sa_scs_accept  - open\n", p_args0);

	cb->msgs = XZA_MSGFREE;

	ci_sa_scs_accept_retval = cb;
	goto exit_ci_sa_scs_accept;
    }
         
/*  If the connection is NOT open, we should discard the CB. */

    dprintf("ci_sa_scs_accept - discard\n", p_args0);

    ci_sa_scs_discard_cb(cb);
    ci_sa_scs_accept_retval = 0;
    goto exit_ci_sa_scs_accept;

exit_ci_sa_scs_accept: 
    return (ci_sa_scs_accept_retval);
}

/*+
 * ============================================================================
 * = ci_sa_scs_disconnect - disconnect from a remote server                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles the SCS disconnect exchange.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_disconnect( cb )
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
void ci_sa_scs_disconnect(struct cb *cb)
{
/*  Send a Disconnect Request, and wait for the connection to close. */

    cb->control_ip = 1;
    if (cb->state == cb_k_open)
	cb->state = cb_k_disc_req_out;
    ci_sa_scs_send_disc_req(cb);
    krn$_post_wait(&scs_lock, &cb->control_s);
    krn$_wait(&scs_lock);
    cb->control_ip = 0;

/*  Once the connection is closed, discard the CB. */

    ci_sa_scs_discard_cb(cb);
}

/*+
 * ============================================================================
 * = ci_sa_scs_discard_cb - discard a CB                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine discards a CB after releasing all allocated resources.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_discard_cb( cb )
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
void ci_sa_scs_discard_cb(struct cb *cb)
{
    struct ci_pb *pb;
    N_Q_BUFFER *p;
    N_CARRIER *car;

/*
 * If there is a control sequence in progress, we ignore this call to
 * dispose of the CB, relying on the fact that we will be called again
 * after the control sequence completes.
 */

    dprintf("\nci_sa_scs_discard_cb\n", p_args0);

    if (cb->control_ip) {
	if (!cb->dequeued) {
	    remq(cb);
	    cb->dequeued = 1;
	}
     	goto exit_ci_sa_scs_discard;
    }

/*
 * We prepare to discard the CB.  We must reclaim the messages we gave  to
 * the port as we opened the connection.  (And, we must reclaim  permanent 
 * messages only; if a temporary message is pulled off of the  message free
 * queue, we put it back on and look for something else.)  And finally we
 * free the CB itself.
** Not sure if above is appropriate for NPORT.
 */
    pb = cb->sb->pb;

/*  All done, discard the CB. */

    krn$_semrelease(&cb->receive_s);
    krn$_semrelease(&cb->control_s);
    krn$_semrelease(&cb->credit_s);
    if (!cb->dequeued)
	remq(cb);
    free(cb);

exit_ci_sa_scs_discard: 

    dprintf("ci_sa_scs_discard_cb: end\n", p_args0);
    return;
}

/*+
 * ============================================================================
 * = ci_sa_scs_break_vc - break a VC                                            =
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
 *	ci_sa_scs_break_vc( cb )
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
void ci_sa_scs_break_vc(struct cb *cb)
{
    dprintf("\nci_sa_scs_break_vc\n", p_args0);
    ci_sa_ppd_break_vc(cb->sb);
}

/*+
 * ============================================================================
 * = ci_sa_scs_send_con_req - send an SCS Connect Request message               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Connect Request message.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_send_con_req( cb, dst_proc, src_proc, src_data, min_credit )
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
void ci_sa_scs_send_con_req(struct cb *cb, char *dst_proc, char *src_proc,
  char *src_data, int min_credit)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct ppd_msg *q, *tmp;
    struct scs_con_req *p;

    nbuf = ci_sa_ppd_alloc(cb->sb, sizeof(*p));
    ncmd = &nbuf->a;
    tmp = q = nbuf->b;
    p = ++tmp;				/*advance to scs portion */

    ncmd->i_opc = SNDMSG;

    q->length = sizeof(*p) + sizeof(*q) - offsetof(struct ppd_msg, mtype);
    q->mtype = ppd_k_scs_msg;

    p->mtype = scs_k_con_req;
    p->credit = 10;
    p->dst_id = 0;
    p->src_id = cb;
    p->min_credit = min_credit;
    p->mbz = 0;
    memcpy(p->dst_proc, dst_proc, sizeof(p->dst_proc));
    memcpy(p->src_proc, src_proc, sizeof(p->src_proc));
    memcpy(p->src_data, src_data, sizeof(p->src_data));

    ci_sa_queue_to_send(cb->sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_scs_send_con_rsp - send an SCS Connect Response message              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Connect Response message, with
 *	an appropriate status.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_send_con_rsp( sb, q )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *	struct scs_con_req *q		- pointer to SCS Connect Request
 *
 * SIDE EFFECTS:
 *
-*/
void ci_sa_scs_send_con_rsp(struct cipca_sb *sb, struct scs_con_req *q)
{
    int i = 0;
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct scs_con_rsp *p;
    struct cb *cb;
    struct ppd_msg *r, *tmp;

    nbuf = ci_sa_ppd_alloc(sb, sizeof(*p));
    ncmd = &nbuf->a;
    r = tmp = nbuf->b;
    p = ++tmp;

    ncmd->opc = SNDPM;
    ncmd->i_opc = SNDMSG;
    r->length = sizeof(*p) + sizeof(*r) - offsetof(struct ppd_msg, mtype);
    r->mtype = ppd_k_scs_msg;

    p->mtype = scs_k_con_rsp;
    p->credit = 0;
    p->dst_id = q->src_id;
    p->src_id = 0;
    p->mbz = 0;

    p->status = scs_k_nomatch;
    ci_sa_queue_to_send(sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_scs_send_acc_req - send an SCS Accept Request message                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Accept Request message.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_send_con_req( cb, dst_proc, src_proc, src_data, min_credit )
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
void ci_sa_scs_send_acc_req(struct cb *cb, char *dst_proc, char *src_proc,
  char *src_data, int min_credit)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct scs_acc_req *p;
    struct ppd_msg *q, *tmp;

    nbuf = ci_sa_ppd_alloc(cb->sb, sizeof(*p));
    ncmd = &nbuf->a;
    q = tmp = nbuf->b;
    p = ++tmp;

    q->length = sizeof(*p) + sizeof(*q) - offsetof(struct ppd_msg, mtype);
    q->mtype = ppd_k_scs_msg;

    ncmd->i_opc = SNDMSG;
    p->mtype = scs_k_acc_req;
    p->credit = 10;
    p->dst_id = cb->dst_id;
    p->src_id = cb;
    p->min_credit = min_credit;
    p->mbz = 0;
    memcpy(p->dst_proc, dst_proc, sizeof(p->dst_proc));
    memcpy(p->src_proc, src_proc, sizeof(p->src_proc));
    memcpy(p->src_data, src_data, sizeof(p->src_data));

    ci_sa_queue_to_send(cb->sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_scs_send_acc_rsp - send an SCS Accept Response message               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Accept Response message.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_send_acc_rsp( cb )
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
void ci_sa_scs_send_acc_rsp(struct cb *cb)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct scs_acc_rsp *p;
    struct ppd_msg *q, *tmp;

    dprintf("\nci_sa_send_acc_rsp\n", p_args0);

    nbuf = ci_sa_ppd_alloc(cb->sb, sizeof(*p));
    ncmd = &nbuf->a;
    q = tmp = nbuf->b;
    p = ++tmp;

    ncmd->opc = SNDPM;
    ncmd->i_opc = SNDMSG;
    q->length = sizeof(*p) + sizeof(*q) - offsetof(struct ppd_msg, mtype);
    q->mtype = ppd_k_scs_msg;

    p->mtype = scs_k_acc_rsp;
    p->credit = 0;
    p->dst_id = cb->dst_id;
    p->src_id = cb;
    p->mbz = 0;
    p->status = scs_k_normal;

    ci_sa_queue_to_send(cb->sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_scs_send_rej_rsp - send an SCS Reject Response message               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Reject Response message.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_send_rej_rsp( sb, q )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *	struct scs_rej_req *q		- pointer to SCS Reject Request
 *
 * SIDE EFFECTS:
 *
-*/
void ci_sa_scs_send_rej_rsp(struct cipca_sb *sb, struct scs_rej_req *q)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct ppd_msg *r, *tmp;
    struct scs_rej_rsp *p;

    nbuf = ci_sa_ppd_alloc(sb, sizeof(*p));
    ncmd = &nbuf->a;
    r = tmp = nbuf->b;
    p = ++tmp;

    ncmd->opc = SNDPM;
    ncmd->i_opc = SNDMSG;
    r->length = sizeof(*p) + sizeof(*r) - offsetof(struct ppd_msg, mtype);
    r->mtype = ppd_k_scs_msg;

    ncmd->i_opc = SNDMSG;
    p->mtype = scs_k_rej_rsp;
    p->credit = 0;
    p->dst_id = q->src_id;
    p->src_id = 0;

    ci_sa_queue_to_send(sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_scs_send_disc_req - send an SCS Disconect Request message            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Disconnect Request message.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_send_disc_req( cb )
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
void ci_sa_scs_send_disc_req(struct cb *cb)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct scs_disc_req *p;
    struct ppd_msg *q, *tmp;

    nbuf = ci_sa_ppd_alloc(cb->sb, sizeof(*p));
    ncmd = &nbuf->a;
    q = tmp = nbuf->b;
    p = ++tmp;

    ncmd->i_opc = SNDMSG;
    q->length = sizeof(*p) + sizeof(*q) - offsetof(struct ppd_msg, mtype);
    q->mtype = ppd_k_scs_msg;

    ncmd->i_opc = SNDMSG;
    p->mtype = scs_k_disc_req;
    p->credit = 0;
    p->dst_id = cb->dst_id;
    p->src_id = cb;
    p->mbz = 0;
    p->reason = scs_k_normal;

    ci_sa_queue_to_send(cb->sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_scs_send_disc_rsp - send an SCS Disconnect Response message          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Disconnect Response message.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_send_disc_rsp( cb )
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
void ci_sa_scs_send_disc_rsp(struct cb *cb)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct scs_disc_rsp *p;
    struct ppd_msg *q, *tmp;

    nbuf = ci_sa_ppd_alloc(cb->sb, sizeof(*p));
    ncmd = &nbuf->a;
    q = tmp = nbuf->b;
    p = ++tmp;

    ncmd->opc = SNDPM;
    ncmd->i_opc = SNDMSG;
    q->length = sizeof(*p) + sizeof(*q) - offsetof(struct ppd_msg, mtype);
    q->mtype = ppd_k_scs_msg;

    ncmd->i_opc = SNDMSG;
    p->mtype = scs_k_disc_rsp;
    p->credit = 0;
    p->dst_id = cb->dst_id;
    p->src_id = cb;

    ci_sa_queue_to_send(cb->sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_scs_send_cred_rsp - send an SCS Credit Response message              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Credit Response message.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_send_cred_rsp( cb, q )
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
void ci_sa_scs_send_cred_rsp(struct cb *cb, struct scs_cred_req *q)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct scs_cred_rsp *p;
    struct ppd_msg *r, *tmp;

    nbuf = ci_sa_ppd_alloc(cb->sb, sizeof(*p));
    ncmd = &nbuf->a;
    r = tmp = nbuf->b;
    p = ++tmp;

    ncmd->opc = SNDPM;
    ncmd->i_opc = SNDMSG;
    r->length = sizeof(*p) + sizeof(*r) - offsetof(struct ppd_msg, mtype);
    r->mtype = ppd_k_scs_msg;

    p->mtype = scs_k_cred_rsp;
    p->credit = q->credit;
    p->dst_id = cb->dst_id;
    p->src_id = cb;

    ci_sa_queue_to_send(cb->sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_scs_send_appl_msg - send an SCS Application Message                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an SCS Application Message.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_send_appl_msg( cb, p )
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
void ci_sa_scs_send_appl_msg(struct cb *cb, struct scs_appl_msg *p)
    {
    int credits;
    struct cipca_sb *sb;

    struct mscp_generic_cmd *q;
    q = p;
    dprintf("\nSEND_APPL_MSG:-- mscp_opc: %x --  -- %s -- \n",
	 p_args2(q->opcode,mscp_opcodes[q->opcode]));



    if( cb->owed_credits > 0 )
	{
#if 0
	pprintf("ci_sa_scs_send_appl_msg: cb->owed_credits:%x\n",
                                                     cb->owed_credits);
#endif
	credits = 1 + cb->owed_credits;
	cb->owed_credits = 0;
	}
    else
	credits = 1;
    sb = cb->sb;
    --p;
    p->mtype = scs_k_appl_msg;
#if 0
    pprintf("ci_sa_scs_send_appl_msg: credits:%x\n",credits);
#endif
    p->credit = credits;
    p->dst_id = cb->dst_id;
    p->src_id = cb;
    ci_sa_ppd_send_scs_msg(cb->sb, p);
    --cb->owed_credits;
    }



/*+
 * ============================================================================
 * = ci_sa_scs_send_data - send SCS data                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initiates an SCS Block Data Transfer.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_send_data( cb, p )
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
void ci_sa_scs_send_data(struct cb *cb, struct SEMAPHORE *sem, int len, int *snd,
  int *rec)
{
    ci_sa_ppd_send_snddat(cb->sb, sem, len, snd, rec);
}

/*+
 * ============================================================================
 * = ci_sa_scs_request_data - request SCS data                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initiates an SCS Block Data Transfer.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_scs_request_data( cb, p )
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
void ci_sa_scs_request_data(struct cb *cb, struct SEMAPHORE *sem, int len, int *snd,
  int *rec)
{
    ci_sa_ppd_send_reqdat(cb->sb, sem, len, snd, rec);
}

/*+
 * ============================================================================
 * = ci_sa_ppd_alloc - allocate a PPD packet                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine allocates a PPD.  It can allocate a datagram or a message,
 *	permanent or temporary.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_alloc( sb, dg )
 *  
 * RETURNS:
 *
 *	A pointer to the PPD portion of the allocated packet.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *	int dg				- DG/MSG flag
 *
 * SIDE EFFECTS:
 *
-*/
int ci_sa_ppd_alloc(struct cipca_sb *sb, int dg)
{

    N_Q_BUFFER *n_q_buffer;		/* Nport command header + ci specific
					 * section*/
    N_CARRIER *car_ptr;
    N_Q_BUF_HEAD *p;			/* Nport command header */
    XPORT *dst, *src;
    N_FLAGS nport_flags;
    CI_FLAGS ci_flags;
    N_STATUS status;			/* Returned packet status */
    struct pb *pb = sb->pb;
    int st;

/*
 * Allocate a fixed number of bytes, which is the size of the largest
 * legal PPD packet, plus the required PPD overhead.
 */

    st = cixcd_get_nbuf(pb, &car_ptr, &n_q_buffer);
    if (st != msg_success) {
	printf("n cixcd_sa_ppd_alloc buffer removal error.\n");
	return 0;
    }

    p = &n_q_buffer->a;

    status.i = 0;
    nport_flags.i = 0;
    nport_flags.f.r = 1;		/* Always generate response */
    nport_flags.f.ps = cipca_path;		/* path select*/
    ci_flags.i = 0;
    ci_flags.SP.sp = 0;			/* path select */

    src = &p->src_xport;
    src->addr.p_grp = 0;
    dst = &p->dst_xport;
    if (sb) {
	dst->addr.p_grp = sb->node_id;
    }

    p->opc = SNDPM;			/* NPORT opc */
    p->token = (int) p;
    p->chnl_idx = 0;
    p->status = status.i;
    p->flags = nport_flags.i;
    p->i_flags = ci_flags.i;

    d2printf("\nCI PPD ALLOC nq car %x cmd buff %x , dest %x p_grp %x cmd xport %x\n",
	  p_args5(car_ptr, p, dst->i, dst->addr.p_grp, p->dst_xport));

    return (n_q_buffer);
}

/*+
 * ============================================================================
 * = ci_sa_ppd_new_node - perform common processing for a new node              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine is called when a new node is discovered by the poller,
 *	and the sequence to start a VC has completed.  This routine copies
 *	information from the START or STACK packet into the SB, and then
 *	calls the MSCP "new node" routine.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_new_node( sb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *	struct ppd_dg_start_stack *p	- pointer to PPD START/STACK packet
 *
 * SIDE EFFECTS:
 *
 *	The VC is marked "open", the SB is updated, and MSCP is notified.
 *
-*/
void ci_sa_ppd_new_node(struct cipca_sb *sb, struct ppd_dg_start_stack *p)
{
    int i;
    int *swtype;

/*  Remove trailing blanks, and zero-terminate the node name string. */

    for (i = 0; i < sizeof(p->nodename); i++)
	if (p->nodename[i] == ' ')
	    break;
    p->nodename[i] = 0;

#if MODULAR
    if (sb->verbose)
	printf("%-18s node %2d  %6.6s %4.4s %4.4s %4.4s\n",
	sb->name, sb->node_id, p->nodename, p->swtype, p->swvers, p->hwtype);
#else
    if(print_new_node){
      if (sb->verbose)
	 pprintf("%s New node %d %s %4.4s %4.4s %4.4s\n",
	 sb->name, sb->node_id, p->nodename, p->swtype, p->swvers, p->hwtype);
    }
#endif

    sprintf(sb->alias, "%.8s", p->nodename);
    sprintf(sb->info, "%.4s/%.4s", p->hwtype, p->swtype);
    sprintf(sb->version, "%4.4s", p->swvers);
    sprintf(sb->string, "%-24s   %-16s   %16s  %s", sb->name, sb->alias,
      sb->info, sb->version);

    memcpy(sb->node_name, p->nodename, sizeof(sb->node_name));
    memcpy(sb->system_id, p->systemid, sizeof(sb->system_id));


/* do new node only if node is an HSC */

    swtype = &p->swtype;
    sb->swtype = *swtype;

    if (*swtype == 0x20205043)		/* sp sp P C */
	sb->poll_active = 2;		/* indicate NO PATH */


    return;
}

#if 0
int ci_sa_ppd_check_system(struct cipca_sb *sb, struct ppd_dg_start_stack *p)
{
    int i;

    i = strncmp(p->hwtype, "VAX ", 4);

    return i;
}
#endif

/*+
 * ============================================================================
 * = ci_sa_ppd_send_dqi - close a VC                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine inhibits datagram activity from a ci node.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_send_dqi( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
 *	The VC is closed.
 *
-*/
void ci_sa_ppd_send_dqi(struct cipca_sb *sb)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    N_SETCKT *p;

    sb->dqi = 1;
    sb->vc = 0;
    nbuf = ci_sa_ppd_alloc(sb, 0);
    ncmd = &nbuf->a;
    p = nbuf->b;

    ncmd->opc = SETCKT;

    p->mask.dqi = 0x1;

    p->m_value.dqi = 0x1;

    ci_sa_queue_to_send(sb, nbuf);
}




/*+
 * ============================================================================
 * = ci_sa_ppd_open_vc - open a VC                                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine opens a VC.  It builds and sends a SETCKT command.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_open_vc( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
 *	The VC is opened.
 *
-*/
void ci_sa_ppd_open_vc(struct cipca_sb *sb)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    N_SETCKT *p;
    XPORT_FCN_EXT *fext = sb->xport_fcn_ext;

    sb->vc = 1;
    sb->poll_count = 0;
    nbuf = ci_sa_ppd_alloc(sb, 0);
    ncmd = &nbuf->a;
    p = nbuf->b;

    ncmd->opc = SETCKT;

    p->mask.cst = 0x1;
    p->mask.nr = 0x7;
    p->mask.ns = 0x7;
    p->mask.nadp = 1;
    p->mask.rdp = 1;
    p->mask.fsn = 1;

    p->m_value.cst = 1;
    p->m_value.nr = 0;
    p->m_value.ns = 0;
    p->m_value.nadp = fext->nadp;
    p->m_value.rdp = fext->rdp;
    p->m_value.fsn = fext->fsn;

    ci_sa_queue_to_send(sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_ppd_close_vc - close a VC                                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine closes a VC.  It builds and sends a PPD SETCKT command.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_close_vc( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
 *	The VC is closed.
 *
-*/
void ci_sa_ppd_close_vc(struct cipca_sb *sb)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    N_SETCKT *p;

    dprintf("\nci_sa_ppd_close_vc\n", p_args0);

    sb->vc = 0;
    sb->ackrec = 0;
    nbuf = ci_sa_ppd_alloc(sb, 0);
    ncmd = &nbuf->a;
    p = nbuf->b;

    ncmd->opc = SETCKT;

    p->mask.cst = 0x1;

    p->m_value.cst = 0x0;

    ci_sa_queue_to_send(sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_ppd_send_idreq - send a PPD IDREQ                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD IDREQ command.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_send_idreq( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
-*/
void ci_sa_ppd_send_idreq(struct cipca_sb *sb)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct ppd_idreq *p;

    nbuf = ci_sa_ppd_alloc(sb, 1);
    ncmd = &nbuf->a;
    p = nbuf->b;

    ncmd->i_opc = ppd_k_idreq;
    p->xct_id[0] = 1;
    p->xct_id[1] = 1;

    ci_sa_queue_to_send(sb, nbuf);
    sb->poll_in_prog++;
}



void ci_sa_ppd_send_idreq_fp(struct cipca_sb *sb, int path)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct ppd_idreq *p;
    N_FLAGS nport_flags;

    nbuf = ci_sa_ppd_alloc(sb, 1);
    ncmd = &nbuf->a;
    p = nbuf->b;

    ncmd->i_opc = ppd_k_idreq;
    p->xct_id[0] = 1;
    p->xct_id[1] = 1;

    nport_flags.i = ncmd->flags;
    nport_flags.f.ps = path;
    ncmd->flags = nport_flags.i;

    ci_sa_queue_to_send(sb, nbuf);
    sb->poll_in_prog++;
}




/*+
 * ============================================================================
 * = ci_sa_ppd_send_retqe  - send a PPD IDREQ                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD IDREQ command.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_send_retqe ( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
-*/

/*+
 * ============================================================================
 * = ci_sa_ppd_send_retqe  - send a PPD IDREQ                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD IDREQ command.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_send_retqe ( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *	If there are not enough entries in the ddfq, ppd_alloc will
 *	alloc another entry in get_nbuf.  Usually you should send a
 *	retqe *before* the ddfq becomes exhausted.
 *
-*/
#if 0
int ci_sa_ppd_send_retqe(struct cipca_sb *sb, unsigned int num_requested)
{

    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    N_RETQE *p;

    nbuf = ci_sa_ppd_alloc(sb, 1);
    ncmd = &nbuf->a;
    p = nbuf->b;

    ncmd->opc = RETQE;
    p->qe_req = num_requested;
    ci_sa_queue_to_send(sb, nbuf);
    return msg_success;
}
#endif

/*+
 * ============================================================================
 * = ci_sa_ppd_poll_idreq - send a PPD IDREQ                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends the poll PPD IDREQ command.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_poll_idreq( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
-*/
#if 0
void ci_sa_ppd_poll_idreq(struct cipca_sb *sb)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct ppd_idreq *p;

    dprintf("\n ci_sa_poll_idreq\n", p_args0);

    nbuf = ci_sa_ppd_alloc(sb, 1);
    ncmd = &nbuf->a;
    p = nbuf->b;

    ncmd->i_opc = ppd_k_idreq;
    p->xct_id[0] = 1;
    p->xct_id[1] = 1;

    ci_sa_queue_to_send(sb, nbuf);
}
#endif

/*+
 * ============================================================================
 * = ci_sa_ppd_send_dg_start - send a PPD DG command of type START              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD DG command, with the PPD message
 *	type set to START.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_send_dg_start( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
-*/
void ci_sa_ppd_send_dg_start(struct cipca_sb *sb)
{

    dprintf("ci_sa_ppd_send_dg_start to %s\n", p_args1(sb->name));

    ci_sa_ppd_send_dg_start_stack(sb, ppd_k_start);
}

/*+
 * ============================================================================
 * = ci_sa_ppd_send_dg_stack - send a PPD DG command of type STACK              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD DG command, with the PPD message
 *	type set to STACK.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_send_dg_stack( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
-*/
void ci_sa_ppd_send_dg_stack(struct cipca_sb *sb)
{

    dprintf("ci_sa_ppd_send_dg_stack to %s\n", p_args1(sb->name));

    ci_sa_ppd_send_dg_start_stack(sb, ppd_k_stack);
}

void ci_sa_ppd_send_dg_start_stack(struct cipca_sb *sb, int mtype)
{
    char id[3];
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct ppd_dg_start_stack *p;
    struct ci_pb *pb;

    pb = sb->pb;
    nbuf = ci_sa_ppd_alloc(sb, 1);
    ncmd = &nbuf->a;
    p = nbuf->b;

    ncmd->i_opc = ppd_k_dg;
    p->length = sizeof(*p) - 2;
    p->mtype = mtype;
    memset(p->systemid, 0, sizeof(p->systemid));
    p->systemid[0] = pb->node_id;
    p->systemid[4] = pb->node_id;
    p->protocol = 1;
    p->mbz = 0;
    p->maxdg = ppd_k_max_dg_msg;
    p->maxmsg = ppd_k_max_dg_msg;
    memcpy(p->swtype, "CP  ", sizeof(p->swtype));
    memcpy(p->swvers, "V1.0", sizeof(p->swvers));
    p->incarn[0] = 0;
    p->incarn[1] = 0;
    memcpy(p->hwtype, "VAX ", sizeof(p->hwtype));
    memset(p->hwvers, 0, sizeof(p->hwvers));
    memcpy(p->nodename, "CP      ", sizeof(p->nodename));
    sprintf(id, "%02X", pb->node_id);

    p->nodename[2] = id[0];
    p->nodename[3] = id[1];
    p->curtime[0] = 0;
    p->curtime[1] = 0;

    ci_sa_queue_to_send(sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_ppd_send_dg_ack - send a PPD DG command of type ACK                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD DG command, with the PPD message
 *	type set to ACK.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_send_dg_ack( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
-*/
void ci_sa_ppd_send_dg_ack(struct cipca_sb *sb)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct ppd_dg_ack *p;

    dprintf("ci_sa_ppd_send_dg_ack to %s\n", p_args1(sb->name));

    nbuf = ci_sa_ppd_alloc(sb, 1);
    ncmd = &nbuf->a;
    p = nbuf->b;

    ncmd->i_opc = ppd_k_dg;
    p->length = sizeof(*p) - 2;
    p->mtype = ppd_k_ack;

    ci_sa_queue_to_send(sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_ppd_send_scs_msg - send a PPD MSG command of type SCS_MSG            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD MSG command, with the PPD message
 *	type set to SCS_MSG.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_send_scs_msg( sb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *	struct ppd_msg *p		- pointer to message to send
 *
 * SIDE EFFECTS:
 *
-*/
void ci_sa_ppd_send_scs_msg(struct cipca_sb *sb, struct ppd_msg *p)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct ppd_msg *ppd;

    if (p < 100) {
	pprintf("ci_sa_ppd_send_scs_msg - p:%x\n",p);
	sysfault(0xdeadbeef);
    }

/*	 
**  Back up to the N_Q_HEADER section.  Recall that this
**  was allocated by sa_scs_alloc_appl_msg.
*/

    --p;				/* back up to ppd portion */

    p->mtype = ppd_k_scs_msg;

    ncmd = p;
    --ncmd;				/* back up to nport command section */

    ncmd->i_opc = ppd_k_msg;
    nbuf = ncmd;			/* just for documentation, assign the
					 * header */

    ci_sa_queue_to_send(sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_ppd_send_snddat - send a PPD SNDDAT                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD SNDDAT command.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_send_snddat( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *	struct SEMAPHORE *sem		- semaphore to be posted
 *	int len				- transaction length
 *	int *snd			- pointer to sender's name/offset
 *	int *rec			- pointer to receiver's name/offset
 *
 * SIDE EFFECTS:
 *
-*/
void ci_sa_ppd_send_snddat(struct cipca_sb *sb, struct SEMAPHORE *sem, int len, int *snd,
  int *rec)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct ppd_snddat *p;

    nbuf = ci_sa_ppd_alloc(sb, 1);
    ncmd = &nbuf->a;
    p = nbuf->b;

    ncmd->i_opc = ppd_k_snddat;
    p->xct_id[0] = 1;
    p->xct_id[1] = sem;
    p->xct_len = len;
    p->snd_name = snd[1];
    p->snd_offset = snd[0];
    p->rec_name = rec[1];
    p->rec_offset = rec[0];

    ci_sa_queue_to_send(sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_ppd_send_reqdat - send a PPD REQDAT                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a PPD REQDAT command.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_send_reqdat( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *	struct SEMAPHORE *sem		- semaphore to be posted
 *	int len				- transaction length
 *	int *snd			- pointer to sender's name/offset
 *	int *rec			- pointer to receiver's name/offset
 *
 * SIDE EFFECTS:
 *
-*/
void ci_sa_ppd_send_reqdat(struct cipca_sb *sb, struct SEMAPHORE *sem, int len, int *snd,
  int *rec)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    struct ppd_reqdat *p;

    nbuf = ci_sa_ppd_alloc(sb, 1);
    ncmd = &nbuf->a;
    p = nbuf->b;

    ncmd->opc = ppd_k_reqdat;
    p->xct_id[0] = 1;
    p->xct_id[1] = sem;
    p->xct_len = len;
    p->snd_name = snd[1];
    p->snd_offset = snd[0];
    p->rec_name = rec[1];
    p->rec_offset = rec[0];

    ci_sa_queue_to_send(sb, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_ppd_break_vc - tear down a VC                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles VC shutdown.  It disables further VC formation
 *	until this (lengthy) process is complete.  Since this routine is called
 *	from tasks which potentially cannot afford to block, it creates another
 *	task to perform most of the work; said task CAN block.  When the VC is
 *	completely run down, a new VC is allowed to form.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_break_vc( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
 *	The VC is closed.  A task is created to run down all open connections
 *  	and delete all known units.
 *
-*/
void ci_sa_ppd_break_vc(struct cipca_sb *sb)
{
  int pid;
    char name[48];
    struct ci_pb *pb;
    struct SEMAPHORE *cb_adr;
    struct cb *cb;
    struct cb *next_cb;

    pb = sb->pb;
    if (!sb->vc) {
	dprintf("ppd_break_vc: vc not open\n", p_args0);
	krn$_post(&sb->ready_s);
	return;
    }

    dprintf("ci_sa_ppd_break_vc: breaking VC to %s %d\n", p_args2(sb->name,
      sb->node_id));

    sb->stallvc = 1;

    if (sb->state != sb_k_stall) {
      sb->vc = 0;
      sb->state = sb_k_stall;
      ci_sa_ppd_close_vc(sb);
      for (cb = sb->cb.flink; cb != &sb->cb.flink; cb = next_cb) {
	dprintf("ppd_rundown, cb = %x\n", p_args1(cb));
	next_cb = cb->cb.flink;
	cb->state = cb_k_closed;
	cb_adr = cb->control_s.flink;
        krn$_bpost(&cb->control_s);
	cb_adr = cb->credit_s.flink;
	krn$_bpost(&cb->credit_s);
	cb_adr = cb->connection_error;
	cb->connection_error(cb);

      }    
    }

#if TURBO || RAWHIDE
    /*
     * My apologies.  I hate platform specific code.  I dont know why this
     * helps Turbo with TX-B missing, but worsens Gamma.  
     */
    sb->poll_active = POLL_WAITING;
    sb->stallvc = 0;
    sb->poll_delay = 5;
#endif

    dprintf("ppd_break_vc: done\n", p_args0);  
}

/*+
 * ============================================================================
 * = ci_sa_ppd_shutdown_vc - tear down a VC                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles VC shutdown.  It disables further VC formation
 *	until this (lengthy) process is complete.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_shutdown_vc( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
 *	The VC is closed.  All open connections are run down 
 *  	and all known units are deleted.
 *
-*/
void ci_sa_ppd_shutdown_vc(struct cipca_sb *sb)
{
    int pid;
    char name[48];
    int ci_sa_ppd_rundown();
    struct ci_pb *pb;

    pb = sb->pb;

    dprintf("ci_sa_ppd_shutdown_vc: breaking VC to %s\n", p_args1(sb->name));

    if (!sb->vc) {

	dprintf("CI shutdown VC, %d not active\n", p_args1(sb->node_id));

	krn$_post(&sb->ready_s);
	return;
    }

    sb->stallvc = 1;

    ci_sa_ppd_break_vc(sb);
    
    dprintf("ppd_shutdown_vc: done\n", p_args0);
}

/*+
 * ============================================================================
 * = ci_sa_ppd_rundown - perform VC-related rundown                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles final rundown of a VC.  Each CB is visited, and
 *	in turn is closed, semaphores are posted, and the class driver's
 *	connection error routine is called.  (The connection error routine is
 *	responsible for getting rid of known units.)  We wait for the CB's
 *	reference count to go to zero, indicating that all users of the CB
 *	have cleaned up (and exited).  Semaphores are deleted, and the
 *	permanent messages that were allocated at connection formation are
 *	reclaimed.  Then the CB is freed.  When all CBs are cleaned up, a new
 *	VC is allowed to form.  A free poll is performed.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_ppd_rundown( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
 *	Each CB is run down and eventually deleted.
 *
-*/
void ci_sa_ppd_rundown(struct cipca_sb *sb)
{
    struct SEMAPHORE *cb_adr;
    struct cb *cb;
    struct cb *next_cb;

    dprintf("ppd_rundown...\n", p_args0);

    krn$_wait(&scs_lock);
/*
 * Rip through the list of CBs.  Say the connection is closed.  Post the  
 * two semaphores associated with this CB, in case there are waiters.  Call
 * the connection error routine associated with this CB.  When the  CB's
 * reference count reaches zero, the CB can be discarded.
 */
    for (cb = sb->cb.flink; cb != &sb->cb.flink; cb = next_cb) {

	dprintf("ppd_rundown, cb = %x\n", p_args1(cb));

	next_cb = cb->cb.flink;

	cb->state = cb_k_closed;

	cb_adr = cb->control_s.flink;

	krn$_bpost(&cb->control_s);

	cb_adr = cb->credit_s.flink;

	krn$_bpost(&cb->credit_s);

	cb_adr = cb->connection_error;

	cb->connection_error(cb);

    }


    krn$_post(&scs_lock);

    dprintf("ppd_rundown: done\n", p_args0);

    krn$_post(&sb->ready_s);
}

/*+
 * ============================================================================
 * = ci_sa_queue_to_send - queue a command to the port                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine gives a PPD command to the port to be executed.  All
 *	commands are inserted onto command queue 0.
 *  
 * FORM OF CALL:
 *  
 *	ci_sa_queue_to_send( sb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb		- system block
 *	struct ppd_header *p		- pointer to PPD packet
 *
 * SIDE EFFECTS:
 *
 *	A PPD packet is queued onto command queue 0, and the port's trigger
 *	register is written.
 *
-*/          
void ci_sa_queue_to_send(struct cipca_sb *sb, N_Q_BUFFER *nbuf)
{
    int st = msg_success;
    struct ci_pb *pb = sb->pb;
    N_CARRIER *car_ptr = pb->car;
    N_Q_BUF_HEAD *p = &nbuf->a;		/* generic NPORT buffer */
    XPORT *dst = &p->dst_xport;
    NQUEUE queue = CCQ2;

    /*	 
    **  Insert the command
    */

#if TRACE
    trace_np('S',pb,nbuf);
    trace_sb('B',pb,sb);
#endif

    st = ci_insert_queue(queue, pb, car_ptr, nbuf);
}

/*+
 * ============================================================================
 * = ci_sa_examine_n_status - examines NPORT command returned status          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This process handles the CIMNA Response queue completion status by
 *	casing on the returned status in the nport q buffer.
 *  
 * FORM OF CALL:
 *  
 *	puc_examine_n_status( nport command buf, port_name, sb)
 *  
 * RETURNS:
 *
 *	Nothing
 *       
 * ARGUMENTS:
 *
 *	N_Q_BUF_HEAD- NPORT command buffer
 *      port_name  - sb->name or pb.port_name
 *	sb - system block
 *
 * SIDE EFFECTS:
 *
 *	
 *
-*/
int ci_sa_examine_n_status(struct cipca_sb *sb, N_Q_BUF_HEAD *p, char *comment)
{

    N_STATUS *n_status = &p->status;
    XPORT *dst = &p->dst_xport;
    struct ppd_dg *q = p + 1;
    int ppdm = q->mtype;
    N_FLAGS nport_flags;
    struct ci_pb *pb = sb->pb;

    nport_flags.i = p->flags;

    if (n_status->l.fail) {
    

       if (sb->ok){
	if ( p->i_opc==0x5 ) {
	   if (n_status->i == 0x229) { 
             printf(msg_ci_bad_path,pb->pb.string,'A',sb->node_id);
	     return msg_failure;
           } else if (n_status->i == 0x829) {
             printf(msg_ci_bad_path,pb->pb.string,'B',sb->node_id);
	     return msg_failure;
	   } else if (n_status->i == 0x029) {
	     /* See this case when HSJ reinited - need to force path */
	     /* before CIPCA will try either path again              */
	     sb->poll_active = POLL_NEED_FORCE_PATH;
	     return msg_failure;
	   }
	}
       }



        if (p->i_opc == 0xb) return msg_success;


	switch (n_status->l.type) {
	    case OK: 
		if (n_status->l.ptha == N_PATH_ACK ||
		  n_status->l.pthb == N_PATH_ACK)
		    return msg_success;
		else {
		    sb->poll_active = POLL_NOPATH; 
		    return msg_failure;
		}
		break;
	    default: 
		if (sb->vc)
		    if (ppdm != ppd_k_error_log) {
			dprintf("cmd/res failed.  BREAKING VC, opcode = %x, status = %x (%s)\n",
			  p_args3(p->i_opc, n_status->l.type, sb->name));
			ci_sa_ppd_break_vc(sb);
		    }
		    sb->poll_active = POLL_NOPATH;  
                }
		return msg_failure;

    } else {
	return msg_success;
    }
}



/*+
 * ============================================================================
 * = ci_sa_response - process a port response                                   =
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
 *	ci_sa_response( pb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct ci_pb *pb		- port block
 *	struct ppd_header *p		- pointer to PPD packet
 *
 * SIDE EFFECTS:
 *
 *	A VC may be closed.  A VC may be opened.  Lots of other possibilities
 *	exist.
 *
-*/
void ci_sa_response(struct ci_pb *pb, N_CARRIER *car_ptr, N_Q_BUFFER *n_q_buf)
{
    int saved_poll, status;
    struct cipca_sb *sb;
    N_Q_BUF_HEAD *p = &n_q_buf->a;
    N_STATUS *n_status = &p->status;
    char *cmd = n_q_buf->b;
    XPORT *src = &p->src_xport;
    XPORT *dst = &p->dst_xport;
    N_FLAGS *n_flags = &p->flags;
/*
 * Reset timeout upon receipt of a message.
 * This allows for no response to timeout or for as long as we continue
 * to talk to the server for the operation to continue.
 */

/*
 * If ERR is set, tear down the VC, else dispatch to opcode-specific
 * processing.
 */



#if TRACE
    trace_np('R',pb,n_q_buf);
#endif

    if (src->addr.p_grp >= pb->pb.num_sb) {
      pprintf("Illegal node found:%d for cluster size %d\n", 
                   src->addr.p_grp, pb->pb.num_sb);
      ci_insert_queue( DADFQ, pb, car_ptr, n_q_buf );
      return;
    }


#if TRACE
    sb = pb->pb.sb[dst->addr.p_grp];
    trace_sb('B',pb,sb);
#endif




    switch (p->opc) {
	case PMSNT: 
	    sb = pb->pb.sb[dst->addr.p_grp];
	    status = ci_sa_examine_n_status(sb, p, "PMSNT");

	    if (n_flags->f.r) {
	      ci_insert_queue(DDFQ, pb, car_ptr, n_q_buf);
            } else {				
              ci_insert_queue( DADFQ, pb, car_ptr, n_q_buf );
            }
	    break;


	case PMREC: 
	    sb = pb->pb.sb[src->addr.p_grp];
	    sb->poll_delay = 60;
	    status = ci_sa_examine_n_status(sb, p, "PMREC");
	    if (status == msg_success)
		ci_mscp_response(sb, n_q_buf);
            switch(p->i_opc) {
                case ppd_k_msgrec:
                case ppd_k_cnfrec:
                case ppd_k_datrec:
                    ci_insert_queue( DAMFQ, pb, car_ptr, n_q_buf );
                    break;
                case ppd_k_dgrec:
                case ppd_k_idrec:
                    ci_insert_queue( DADFQ, pb, car_ptr, n_q_buf );
                    break;
            }
	    break;
	case QERET: 
	    sb = pb->pb.sb[pb->node_id];
	    if (ci_sa_examine_n_status(sb, p, "QERET") == msg_success) {
		N_QERET *q = cmd;
		int st;

		st = cixcd_replenish_ddfq(pb, q->qe_ret, 1, n_flags->f.f_d_ca);
	    }
	    ci_insert_queue(DDFQ, pb, car_ptr, n_q_buf);
					/* replenish us */
	    break;
	case CKTSET: 
	    if (dst->addr.p_grp == pb->node_id)
		sb = pb->pb.sb[src->addr.p_grp];
	    else
		sb = pb->pb.sb[dst->addr.p_grp];
	    sb->send_active = 0;

	    {
		N_CKTSET *cktset = cmd;
                unsigned __int64 mask;
                unsigned __int64 m_value;
                unsigned __int64 in_vcd;
                mask = *(unsigned __int64*)(&cktset->mask);
                m_value = *(unsigned __int64*)(&cktset->m_value);
                in_vcd = *(unsigned __int64*)(&cktset->in_vcd);
                dprintf("CKTSET: node %x",sb->node_id);
                dprintf("  CKTSET: mask %x mvalue %x",p_args2(mask,m_value));
                dprintf("   in_vcd %x \n", p_args1(in_vcd));
	    }

	    ci_insert_queue(DDFQ, pb, car_ptr, n_q_buf);
					/* replenish us */
	    break;

	default: 
	    if (dst->addr.p_grp == pb->node_id)
		sb = pb->pb.sb[src->addr.p_grp];
	    else
		sb = pb->pb.sb[dst->addr.p_grp];

	    dprintf("  PMSNT: unhandled opcode %s - no action\n",
	      p_args1(N_opcodes[p->opc & 0x3f]));

	    ci_insert_queue(DDFQ, pb, car_ptr, n_q_buf);
					/* replenish us */
	    break;
    }
}

#if DEBUG
/*
 * ci_sa_print_packet
 *
 * send_rec =0, send packet, = 1, received packet
 */

void ci_sa_print_packet(N_Q_BUFFER *nbuf, char *port_name, SNTSTATE snt)
{
    int nopc, popc, ppdm, scsm, len = 0, msg = FALSE;
    FQ fq = MFQ;
    int *m, opc, ppacket, printopc;
    char *n;
    char *opc_b;
    char returnmsg;
    N_Q_BUF_HEAD *ncmd = &nbuf->a;
    N_STATUS *n_status = &ncmd->status;
    struct ppd_dg *ppd = nbuf->b;
    struct scs_msg *scs = ppd + 1;
    XPORT *dst = &ncmd->dst_xport, *src = &ncmd->src_xport;
    N_FLAGS nport_flags;
    CI_FLAGS ci_flags;
    N_STATUS status;			/* Returned packet status */

    if (ci_debug_flag & 2)
	printopc = TRUE;
    else
	printopc = FALSE;
    if (ci_debug_flag & 4)
	ppacket = TRUE;
    else
	ppacket = FALSE;

    m = ncmd;
    nopc = ncmd->opc;
    popc = ncmd->i_opc;
    ppdm = 0;
    scsm = 0;

    if (snt == RCV)
	dprintf("CI Received ", p_args0);
    else
	dprintf("CI Sent ", p_args0);
    dprintf("%s %s from %s src %d dst %d, status_bit %x, type: %d\n, PATH_A: %s  PATH_B: %s,  ci_flags %x\n",
      p_args10(N_opcodes[nopc], ci_opcodes[popc], port_name, src->addr.p_grp,
      dst->addr.p_grp, n_status->l.fail, n_status->l.type, ci_status_nopath[
      n_status->l.ptha], ci_status_nopath[n_status->l.pthb], ncmd->i_flags));

    if (ppacket || printopc) {
	switch (popc) {
	    case DGREC: 
		msg = TRUE;
		fq = DFQ;
		len = 40;
		ppdm = ppd->mtype;
		scsm = 0;
		break;
	    case MSGREC: 
		msg = TRUE;
		ppdm = ppd->mtype;
		scsm = scs->mtype;	/* point to scs msg */
		len = ppd->length;
		break;
	    case IDREC: 
	    default: 
		msg = FALSE;
		len = 40;
		break;
	}
	if (msg)
	    dprintf("%s", p_args1(ci_ppd_mtypes[ppdm]));
	dprintf("\n<- %08x : ", p_args1(*(m + 7)));
	if (msg)
	    dprintf("%s\n", p_args1(ci_scs_mtype[scsm]));
	else
	    printf("\n", p_args0);

	if (ppacket) {
	    dprintf("\n", p_args0);
	    dprintf("<- %08x : token\n", p_args1(*m));
	    dprintf("<- %08x : token\n", p_args1(*(m + 1)));
	    returnmsg = ' ';
	    if (ncmd->flags != 0)
		returnmsg = 'R';
	    dprintf("<- %08x : %c flags: %04x %s\n", p_args4(*(m + 2),
	      returnmsg, (*(m + 2) >> 16) & 0xffff, N_opcodes[nopc]));
	    dprintf("<- %08x : %s status: %04x\n",
	      p_args3(*(m + 3), ci_opcodes[popc], *(m + 3) & 0xffff));
	    dprintf("<- %08x : src %x dst %x\n", p_args3(*(m + 4),
	      (*(m + 4) >> 16) & 0xffff, (*(m + 4)) & 0xffff));
	    dprintf("<- %08x : reserved\n", p_args1(*(m + 5)));
	    dprintf("<- %08x : ", p_args1(*(m + 6)));
	    if (msg)
		dprintf("%s", p_args1(ci_ppd_mtypes[ppdm]));
	    dprintf("\n<- %08x : ", p_args1(*(m + 7)));
	    if (msg)
		dprintf("%s\n", p_args1(ci_scs_mtype[scsm]));
	    else
		dprintf("\n", p_args0);

	    for (m += 8, len = (len / 4) - 1; len > 0; len--, m++) {
		dprintf("<- %08x : ", p_args1(*(m)));
		n = m;
		ci_showchar(*n++);
		ci_showchar(*n++);
		ci_showchar(*n++);
		ci_showchar(*n);
		dprintf("\n", p_args0);
	    }
	    dprintf("\n", p_args0);
	}
    }
}


void dump_ci_pb(struct ci_pb *pb){

   pprintf("ci_pb: %x\n",&(pb->pb));
   pprintf("  unsigned long int node_id: %x\n",pb->node_id);
   pprintf("  unsigned long int vector: %x\n",pb->vector);
   pprintf("  unsigned long int vector2: %x\n",pb->vector2);
   pprintf("  unsigned long int hard_base: %x\n",pb->hard_base);
   pprintf("  unsigned long int soft_base: %x\n",pb->soft_base);
   pprintf("  unsigned long int mem_base: %x\n",pb->mem_base);
   pprintf("  unsigned long int poll: %x\n",pb->poll);
   pprintf("  unsigned long int interrupt: %x\n",pb->interrupt);
   pprintf("  struct kfmsa_blk *blk_ptr: %x\n",pb->blk_ptr);
   pprintf("  struct SEMAPHORE *adap_s: %x\n",pb->adap_s);
   pprintf("  NPORT_AB *ab: %x\n",pb->ab);
   pprintf("  N_QP ddfq: %x\n", &pb->ddfq);
   pprintf("  int ddfq_entries: %x\n",pb->ddfq_entries);
   pprintf("  N_Q_BUFFER *N_buffer_blk: %x\n",pb->N_buffer_blk);
   pprintf("  N_CARRIER *N_carrier_blk: %x\n",pb->N_carrier_blk);
   pprintf("  BDL_PTR *bdlt_ptr_blk: %x\n",pb->bdlt_ptr_blk);
   pprintf(  "BDL *bdl_blk: %x\n",pb->bdl_blk);
   pprintf("  N_CARRIER *car: %x\n",pb->car);
   pprintf("  struct MBX *mbx: %x\n",pb->mbx);
   pprintf("  int (*poke)(): %s\n",pb->poke);
   pprintf("  ADAPTER_STATE adapter_state: %x\n",&pb->adapter_state);

}


void dump_pb(struct pb *pb){
  pprintf("\t setmode: %x\n",pb->setmode);
  pprintf("\t ref: %x\n",pb->ref);
  pprintf("\t state: %x\n",pb->state);
  pprintf("\t mode: %x\n",pb->mode);
  pprintf("\t desired_mode: %x\n",pb->desired_mode);
  pprintf("\t hose: %x\n",pb->hose);
  pprintf("\t bus: %x\n",pb->bus);
  pprintf("\t slot: %x\n",pb->slot);
  pprintf("\t function: %x\n",pb->function);
  pprintf("\t channel: %x\n",pb->channel);
  pprintf("\t protocol: %x\n",pb->protocol);
  pprintf("\t controller: %x\n",pb->controller);
  pprintf("\t port_name: %s\n", pb->port_name);
  pprintf("\t type: %x\n",pb->type);
  pprintf("\t csr: %x\n",pb->csr);
  pprintf("\t vector: %x\n",pb->vector);
  pprintf("\t device: %s\n",pb->device);
  pprintf("\t misc: %x\n",pb->misc);
  pprintf("\t pdm: %x\n",pb->pdm);
  pprintf("\t config_device: %x\n",pb->config_device);
  pprintf("\t order: %x\n",pb->order);
  pprintf("\t sb: %x\n",pb->sb);
  pprintf("\t num_sb: %x\n",pb->num_sb);
  pprintf("\t name: %s\n",pb->name);
  pprintf("\t alias: %s\n",pb->alias);
  pprintf("\t info: %s\n",pb->info);
  pprintf("\t version: %s\n",pb->version);
  pprintf("\t string: %s\n",pb->string);
  pprintf("\t id: %s\n",pb->id);
  pprintf("\t e_memq: %x\n", pb->e_memq);
  pprintf("\t status: %x\n",pb->status);
  pprintf("\t pq: %x\n",pb->pq);
  pprintf("\t parent: %x\n",pb->parent);
}
                                        

void dump_cixcd_blk(struct cixcd_blk *blk_ptr){
  pprintf("cixcd_blk : %x\n",blk_ptr);
  pprintf("  u_int hose: %x\n",blk_ptr->hose);
  pprintf("  u_int slot: %x\n",blk_ptr->slot);
  pprintf("  u_int controller: %x\n",blk_ptr->controller);
  pprintf("  u_int hard_base: %x\n",blk_ptr->hard_base);
  pprintf("  u_int soft_base: %x\n",blk_ptr->soft_base);
  pprintf("  u_int mem_base: %x\n",blk_ptr->mem_base);
  pprintf("  u_int vect1: %x\n",blk_ptr->vect1);
  pprintf("  u_int vect2: %x\n",blk_ptr->vect2);
  pprintf("  u_int completion_pid: %x\n",blk_ptr->completion_pid);
  pprintf("  u_int p_misc_pid: %x\n",blk_ptr->p_misc_pid);
  pprintf("  u_int refcnt: %x\n",blk_ptr->refcnt);
  pprintf("  struct pb *pb: %x\n",blk_ptr->pb);
  pprintf("  struct SEMAPHORE completion_compl_s: %x\n",&blk_ptr->cixcd_completion_compl_s);
  pprintf("  struct SEMAPHORE misc_compl_s: %x\n",&blk_ptr->cixcd_misc_compl_s);
  pprintf("  struct SEMAPHORE completion_s: %x\n",&blk_ptr->cixcd_completion_s);
  pprintf("  struct SEMAPHORE misc_s: %x\n",&blk_ptr->cixcd_misc_s);
  pprintf("  struct SEMAPHORE adap_s: %x\n",&blk_ptr->cixcd_adap_s);
  pprintf("  struct MBX *mbx_int: %x\n",blk_ptr->mbx_int);
  pprintf("  NPORT_AB *ab: %x\n",blk_ptr->ab);
}


/* Buffer Pointers */
void dump_type_ptr(TYPE_PTR *type_ptr){
  pprintf("TYPE_PTR : %x\n", type_ptr);    
  pprintf("  unsigned int     type  : 2 :%x\n",type_ptr->type);
  pprintf("  unsigned int           : 3 :\n");
  pprintf("  unsigned int     pa    : 27 :%x\n",type_ptr->pa );
  pprintf("  unsigned int     pa_e  : 16 :%x\n",type_ptr->pa_e );
  pprintf("  unsigned int     count : 16 :%x\n",type_ptr->count );
  pprintf("\n");
};


void dump_pa_ptr(PA_PTR *pa_ptr){
  pprintf("PA_PTR : %x\n",pa_ptr);
  pprintf("unsigned int           : 5 :\n");
  pprintf("unsigned int      pa   : 27 :%x\n", pa_ptr->pa );
  pprintf("unsigned int      pa_e : 16 :%x\n", pa_ptr->pa_e);
  pprintf("unsigned int           : 16 :\n");
  pprintf("\n");
};


void dump_ampb_ptr(AMPB_PTR *ampb_ptr){
  pprintf("AMPB_PTR : %x\n",ampb_ptr);
  pprintf("  unsigned int      v    : 1 :%x\n",ampb_ptr->v );
  pprintf("  unsigned int           : 4 :\n");
  pprintf("  unsigned int      pa   : 27 :%x\n",ampb_ptr->pa );
  pprintf("  unsigned int      pa_e : 16 :%x\n",ampb_ptr->pa_e );
  pprintf("  unsigned int           : 16 :\n");
  pprintf("\n");
};

/* Buffer Name */
void dump_buffer_name(BUF_NAME *buffer_name){
  pprintf("BUF_NAME : %x\n",buffer_name);
  pprintf("  unsigned int bdl_idx : 8 :%x\n",buffer_name->bdl_idx );
  pprintf("  unsigned int bdlt_idx : 12 :%x\n",buffer_name->bdlt_idx);
  pprintf("  unsigned int key : 12 :%x\n",buffer_name->key);
  pprintf("\n");
};

/* Buffer D  escriptor Leaf */
void dump_bdl(BDL *bdl){
  pprintf("Buffer Descriptor Leaf  DBL : %x\n",bdl);
  pprintf("  unsigned int page_offset : 13 :%x\n",bdl->page_offset );
  pprintf("  unsigned int             : 3 :\n");
  pprintf("  unsigned int v           : 1 :%x\n",bdl->v);
  pprintf("  unsigned int ro          : 1 :%x\n",bdl->ro);
  pprintf("  unsigned int             : 14 :\n");
  pprintf("  BUF_NAME     buf_name :%x\n",&bdl->buf_name);
  pprintf("  unsigned int buf_len :%x\n",bdl->buf_len);
  pprintf("  unsigned int             : 32 :\n");
  pprintf("  TYPE_PTR     root0 :%x\n",&bdl->root0);
  pprintf("  TYPE_PTR     root1 :%x\n",&bdl->root1);
  pprintf("\n");
};

/* Pointer to Buffer Descriptor Leaf */
void dump_bdl_ptr(BDL_PTR *bdl_ptr){
  pprintf("Pointer to Buffer Descriptor Leaf: %x\n",bdl_ptr);
  pprintf("  unsigned int      v         : 1 :%x\n",bdl_ptr->v );
  pprintf("  unsigned int                : 12 :\n");
  pprintf("  unsigned long int bdl_ptr   : 19 :%x\n",bdl_ptr->bdl_ptr );
  pprintf("  unsigned long int bdl_ptr_e : 16 :%x\n",bdl_ptr->bdl_ptr_e );
  pprintf("  unsigned long int           : 16 :\n");
  pprintf("  unsigned char     soft[8] :%x\n",bdl_ptr->soft );
  pprintf("\n");
};

/* N_Port command flags */
void dump_n_flags(N_FLAGS *n_flags){
  pprintf("N_Port command flags:%x\n",n_flags);
  pprintf("  unsigned short int i : %x\n",n_flags->i);
  pprintf("  --unsigned int     r  : 1 Generate response : %x\n",n_flags->f.r);
  pprintf("  --unsigned int     i  : 1 Suppress interrupts : %x\n",n_flags->f.i);
  pprintf("  --union        f_d_ca : 1 : %x\n",n_flags->f.f_d_ca);
  pprintf("  --unsigned int        : 1 Should Be Zero \n");
  pprintf("  --unsigned int     sw : 4 Reserved for software : %x\n",n_flags->f.sw);
  pprintf("  --unsigned int     gn : 3 Virtual Circuit Generation Number : %x\n",n_flags->f.gn);
  pprintf("  --unsigned int     m  : 3 Packet Multiple : %x\n",n_flags->f.m);
  pprintf("  --unsigned int     ps : 2 Path Select : %x\n",n_flags->f.ps);
  pprintf("\n");
};

char *status_type[] = {
  "No failure",                    /*  0 */
  "Packet Size Violation",         /*  1 */
  "Invalid Buffer Name",           /*  2 */
  "Buffer Length Violatin",        /*  3 */
  0,0,0,0,0,0,0,0,0,0,0,0,         /* 4 - 15 */
  "Unrecognized Command",          /* 16 */
  "Invalid Destination Xport Address",                /* 17 */
  "Virtual Circuit Closed before command execution",  /* 18 */
  "Invalid Virtual Circuit Generation Number",        /* 19 */
  "No Path to destination Xport",                     /* 20 */
  "Insufficient Virtual Circuit descriptors",         /* 21 */
  "NRp1 Cache Resources exhausted",                   /* 22 */
  0,0,0,0,0,0,0,0,0,                                  /* 23 - 31 */
  "Unrecognized received packet",                     /* 32 */
  0,                                                  /* 33 */
  "Invalid Received Destination Xport Address",       /* 34 */
  "Invalid Sequence Number",                          /* 35 */
  "Discarded VC packet"                               /* 36 */
};


void dump_n_status(N_STATUS *n_status){
  pprintf("N_STATUS : %x\n",n_status);
  pprintf("  unsigned short int i : %x\n",n_status->i);
  pprintf("                     or \n");
  pprintf("  --unsigned int     fail : 1 : %x\n",n_status->l.fail);
  pprintf("  --unsigned int     type : 7 : %x %s\n",n_status->l.type,
                                           status_type[n_status->l.type]);
  pprintf("  --unsigned int     ptha : 2 Path A status : %x\n",n_status->l.ptha);
  pprintf("  --unsigned int     pthb : 2 Path B status : %x\n",n_status->l.pthb);
  pprintf("  --unsigned int     ca   : 1 Counted events for All addresses : %x\n", n_status->l.ca);
  pprintf("  --unsigned int          : 3 Should Be Zero \n");
  pprintf("                     or \n");
  pprintf("  --unsigned int     fail : 1 : %x\n",n_status->r.fail);
  pprintf("  --unsigned int     type : 7 : %x %s\n",n_status->r.type,
                                           status_type[n_status->r.type]);
  pprintf("  --unsigned int     rp   : 2  Receive path : %x\n",n_status->r.rp);
  pprintf("\n");
};


/* N_Port Queue Buffer */
void dump_n_q_buffer(N_Q_BUFFER *n_q_buffer){
  int i;
  pprintf("N_Q_BUFFER : %x\n",n_q_buffer);
  pprintf("  N_Q_BUF_HEAD a :%x\n",&n_q_buffer->a );
    dump_n_q_buf_head(&n_q_buffer->a);
  pprintf("  unsigned char b[N_Q_BODY_SIZ] / %X[%x]\n",n_q_buffer->b,N_Q_BODY_SIZ );
  pprintf("\n");
};

/* Adapter Parameter Block */
void dump_n_apb(N_APB *n_apb){
  pprintf("Adapter Parameter Block N_APB:%x\n",n_apb);
  pprintf("  unsigned short int vcdt_len :%x\n",n_apb->vcdt_len );
  pprintf("  unsigned short int ibuf_len :%x\n",n_apb->ibuf_len);
  pprintf("  unsigned short int rampb :%x\n", n_apb->rampb);
  pprintf("  unsigned short int type :%x\n", n_apb->type);
  pprintf("  unsigned char imp_spec[8] :%x\n",n_apb->imp_spec );
  pprintf("  unsigned char sbz[16] :%2x\n",n_apb->sbz );
  pprintf("\n");
};

/* Channel Parameter Block */
void dump_n_cpb(N_CPB *n_cpb){
  pprintf("Nport Channel Parameter Block : %x\n",n_cpb);
  pprintf("  unsigned short int xp_addr :%x\n", n_cpb->xp_addr);
  pprintf("  unsigned char max_pgrp :%x\n", n_cpb->max_pgrp);
  pprintf("  unsigned char max_mem :%x\n", n_cpb->max_mem);
  pprintf("  int sbz :%x\n", n_cpb->sbz );
  pprintf("  unsigned short int dlink_type :%x\n", n_cpb->dlink_type);
  pprintf("  unsigned char dlink_param[6] : %x %x %x %x %x %x\n", 
                             n_cpb->dlink_param[0],
                             n_cpb->dlink_param[1],
                             n_cpb->dlink_param[2],
                             n_cpb->dlink_param[3],
                             n_cpb->dlink_param[4],
                             n_cpb->dlink_param[5]
                             );
  pprintf("  char imp_spec[16] :%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", 
              n_cpb->imp_spec[0],n_cpb->imp_spec[1],n_cpb->imp_spec[2],
              n_cpb->imp_spec[3],n_cpb->imp_spec[4],n_cpb->imp_spec[5],
              n_cpb->imp_spec[6],n_cpb->imp_spec[7],n_cpb->imp_spec[8],
              n_cpb->imp_spec[9],n_cpb->imp_spec[10],n_cpb->imp_spec[11],
              n_cpb->imp_spec[12],n_cpb->imp_spec[13],n_cpb->imp_spec[14],
              n_cpb->imp_spec[15]
              );
  pprintf("\n");
};



/* Nport Channel Command Queues */
void dump_n_ccq(N_CCQ *n_ccq){
  pprintf("  Nport Channel Command Queues : %x\n",n_ccq);
  pprintf("    N_QP dccq3 head :%x\n",n_ccq->dccq3.head_ptr );
  pprintf("         dccq3 tail :%x\n",n_ccq->dccq3.tail_ptr );
  pprintf("    N_QP dccq2 head :%x\n",n_ccq->dccq2.head_ptr );
  pprintf("         dccq2 tail :%x\n",n_ccq->dccq2.tail_ptr );
  pprintf("    N_QP dccq1 head :%x\n",n_ccq->dccq1.head_ptr );
  pprintf("         dccq1 tail :%x\n",n_ccq->dccq1.tail_ptr );
  pprintf("    N_QP dccq0 head :%x\n",n_ccq->dccq0.head_ptr );
  pprintf("         dccq0 tail :%x\n",n_ccq->dccq0.tail_ptr );
  pprintf("    N_QP cccq3 head :%x\n",n_ccq->dccq3.head_ptr );
  pprintf("         cccq3 tail :%x\n",n_ccq->dccq3.tail_ptr );
  pprintf("    N_QP cccq2 head :%x\n",n_ccq->dccq2.head_ptr );
  pprintf("         cccq2 tail :%x\n",n_ccq->dccq2.tail_ptr );
  pprintf("    N_QP cccq1 head :%x\n",n_ccq->dccq1.head_ptr );
  pprintf("         cccq1 tail :%x\n",n_ccq->dccq1.tail_ptr );
  pprintf("    N_QP cccq0 head :%x\n",n_ccq->dccq0.head_ptr );
  pprintf("         cccq0 tail :%x\n",n_ccq->dccq0.tail_ptr );
  pprintf("\n");
};
/* N_Port Adapter Block */
void dump_nport_ab(NPORT_AB *nport_ab){
  pprintf("N_Port Adapter Block : %x\n", nport_ab);
  pprintf("  N_CCQ             cq[MAX_CHAN] :%x[%x]\n",&nport_ab->cq,MAX_CHAN );
  pprintf("  PRINTING ONLY cq[0] : \n"); dump_n_ccq(&nport_ab->cq[0]);
  pprintf("  N_QP              adrq head:%x\n", nport_ab->adrq.head_ptr);
  pprintf("                    adrq tail:%x\n", nport_ab->adrq.tail_ptr);
  pprintf("  N_QP              dadfq head:%x\n",nport_ab->dadfq.head_ptr );
  pprintf("                    dadfq tail:%x\n",nport_ab->dadfq.tail_ptr );
  pprintf("  N_QP              addfq head:%x\n",nport_ab->addfq.head_ptr );
  pprintf("                    addfq tail:%x\n",nport_ab->addfq.tail_ptr );
  pprintf("  N_QP              damfq head:%x\n",nport_ab->damfq.head_ptr );
  pprintf("                    damfq tail:%x\n",nport_ab->damfq.tail_ptr );
  pprintf("  N_QP              admfq head:%x\n",nport_ab->admfq.head_ptr );
  pprintf("                    admfq tail:%x\n",nport_ab->admfq.tail_ptr );
  pprintf("  N_QP              aadfq head:%x\n",nport_ab->aadfq.head_ptr );
  pprintf("                    aadfq tail:%x\n",nport_ab->aadfq.tail_ptr );
  pprintf("  N_QP              aamfq head:%x\n",nport_ab->aamfq.head_ptr );
  pprintf("                    aamfq tail:%x\n",nport_ab->aamfq.tail_ptr );
  pprintf("  BDL_PTR           *bdlt_base :%x\n", nport_ab->bdlt_base);
  pprintf("  unsigned int      rsvd0 :%x\n",nport_ab->rsvd0 );
  pprintf("  unsigned int      bdlt_len :%x\n", nport_ab->bdlt_len);
  pprintf("  unsigned int      keep_alive :%x\n",nport_ab->keep_alive );
  pprintf("  unsigned int      dqe_len :%x\n", nport_ab->dqe_len);
  pprintf("  unsigned int      mqe_len :%x\n", nport_ab->mqe_len);
  pprintf("  unsigned int      int_holdoff :%x\n", nport_ab->int_holdoff);
  pprintf("  long int          rsvd1 :%x\n", nport_ab->rsvd1);
  pprintf("  AMPB_PTR          ampb_base :%x\n", &nport_ab->ampb_base);
  pprintf("  unsigned int      ampb_len :%x\n", nport_ab->ampb_len);
  pprintf("  unsigned int      rsvd3 :%x\n", nport_ab->rsvd3);
  pprintf("  unsigned char     rsvd[32] :%x[32]\n",nport_ab->rsvd );
  pprintf("  N_APB             apb :%x\n",&nport_ab->apb );
  pprintf("  N_CPB             cpb[MAX_CHAN] :%x[%x]\n", &nport_ab->cpb,MAX_CHAN);
  pprintf("\n");
};

/* Virtual Circuit Descriptor */
void dump_vcd(VCD *vcd){
  pprintf("Virtual Circuit Descriptor : %x\n",vcd);
  pprintf("  unsigned int ns     : 3 :%x\n",vcd->ns );
  pprintf("  unsigned int nr     : 3 :%x\n",vcd->nr );
  pprintf("  unsigned int rdp    : 1 :%x\n",vcd->rdp );
  pprintf("  unsigned int fsn    : 1 :%x\n",vcd->fsn );
  pprintf("  unsigned int eas    : 1 :%x\n",vcd->eas );
  pprintf("  unsigned int psts   : 2 :%x\n",vcd->psts );
  pprintf("  unsigned int nadp   : 1 :%x\n",vcd->nadp );
  pprintf("  unsigned int dqi    : 1 :%x\n",vcd->dqi );
  pprintf("  unsigned int        : 2 :n" );
  pprintf("  unsigned int cst    : 1 :%x\n",vcd->cst );
  pprintf("  unsigned int        : 10 :\n" );
  pprintf("  unsigned int gn     : 3 :%x\n",vcd->gn );
  pprintf("  unsigned int        : 1 :\n" );
  pprintf("  unsigned int snd_st : 3 :%x\n",vcd->snd_st );
  pprintf("  unsigned int        : 5 :\n" );
  pprintf("  unsigned int rcv_st : 2 :%x\n",vcd->rcv_st );
  pprintf("  unsigned int        : 22 :\n" );
  pprintf("\n");
};


void dump_n_setckt(N_SETCKT *n_setckt){
  pprintf("N_SETCKT : %x\n",n_setckt);
  pprintf("  VCD                mask :%x\n", n_setckt->mask);
  pprintf("  VCD                m_value :%x\n", n_setckt->m_value);
  pprintf("\n");
};

void dump_n_cktset(N_CKTSET *n_cktset){
  pprintf("N_CKTSET : %x\n",n_cktset);
  pprintf("  VCD                mask :%x\n", *(unsigned __int64*)(&n_cktset->mask));
  pprintf("  VCD                m_value :%x\n", *(unsigned __int64*)(&n_cktset->m_value));
  pprintf("  VCD                in_vcd :%x\n", *(unsigned __int64*)(&n_cktset->in_vcd));
  pprintf("  unsigned short int vcd_res :%x\n",n_cktset->vcd_res );
  pprintf("  unsigned short int rseq_res :%x\n", n_cktset->rseq_res);
  pprintf("  unsigned short int sbz :%x\n", n_cktset->sbz);
  pprintf("\n");
};



/* Set Channel Variables */
void dump_n_chnv(N_CHNV *n_chnv){
  pprintf("N_CHNV : %x\n",n_chnv);
  pprintf("  unsigned int       dqii : 1 :%x\n",n_chnv->dqii );
  pprintf("  unsigned int       dqid : 1 :%x\n",n_chnv->dqid );
  pprintf("  unsigned int       dqia : 1 :%x\n",n_chnv->dqia );
  pprintf("  unsigned int            : 29 :\n" );
  pprintf("  unsigned int       rsvd :%x\n",n_chnv->rsvd );
  pprintf("\n");
} ;

void dump_n_setchnl(N_SETCHNL *n_setchnl){
  pprintf("N_SETCHNL : %x\n",n_setchnl);
  pprintf("  N_CHNV             mask :%x\n",&n_setchnl->mask );
  pprintf("  N_CHNV             m_value :%x\n",&n_setchnl->m_value );
  pprintf("\n");
} ;


void dump_n_chnlset(N_CHNLSET *n_chnlset){
  pprintf("N_CHNLSET : %x\n",n_chnlset);
  pprintf("  N_CHNV             mask :%x\n",&n_chnlset->mask );
  pprintf("  N_CHNV             m_value :%x\n", &n_chnlset->m_value);
  pprintf("  N_CHNV             in_chnlst :%x\n", &n_chnlset->in_chnlst);
  pprintf("  unsigned short int vcd_res :%x\n", n_chnlset->vcd_res);
  pprintf("\n");
};


void dump_n_purgq(N_PURGQ *n_purgq){
  pprintf("N_PURGQ : %x\n",n_purgq);
  pprintf("  unsigned char      cmdq_mask :%x\n",n_purgq->cmdq_mask );
  pprintf("\n");
};

void dump_n_qpurg(N_QPURG *n_qpurg){
  pprintf("N_QPURG : %x\n",n_qpurg);
  pprintf("  unsigned char      cmdq_mask :%x\n",n_qpurg->cmdq_mask );
  pprintf("\n");
};


void dump_cipca_regs(struct cixcd_blk *blk_ptr){

   pprintf("adprst (%x) :  %s\n",adprst,"WO");
   pprintf("clrinta (%x) :  %s\n",clrinta,"WO");
   pprintf("clrintb (%x) :  %s\n",clrintb,"WO");
   pprintf("aitcr (%x) :  %s\n",aitcr,"WO");
   pprintf("nodests (%x) :  %x\n",nodests,rl(nodests));
   pprintf("intena (%x) :  %x\n",intena,rl(intena));
   pprintf("abbr (%x) :  %x\n",abbr,rl(abbr));
   pprintf("ccq2ir (%x) :  %s\n",ccq2ir,"WO");
   pprintf("ccq1ir (%x) :  %s\n",ccq1ir,"WO");
   pprintf("ccq0ir (%x) :  %s\n",ccq0ir,"WO");
   pprintf("adfqir (%x) :  %s\n",adfqir,"WO");
   pprintf("amfqir (%x) :  %s\n",amfqir,"WO");
   pprintf("casr0 (%x) :  %x\n",casr0,rl(casr0));
   pprintf("casr1 (%x) :  %x\n",casr1,rl(casr1));
   pprintf("cafar0 (%x) :  %x\n",cafar0,rl(cafar0));
   pprintf("casrcr (%x) :  %s\n",casrcr,"WO");
   pprintf("cicr (%x) :  %s\n",cicr,"WO");
   pprintf("cecr (%x) :  %s\n",cecr,"WO");
   pprintf("amtcr (%x) :  %s\n",amtcr,"WO");
   pprintf("amtecr (%x) :  %s\n",amtecr,"WO");

   pprintf("amcsr (%x) :  %x\n",amcsr,rl(amcsr));
   pprintf("accx (%x) :  %x\n",accx,rl(accx));

   pprintf("mrev (%x) :  %x\n",amtecr,rl(mrev));
   pprintf("musr (%x) :  %x\n",musr,rl(musr));
   pprintf("amtecr (%x) :  %s\n",amtecr,"WO");

}

void dump_ci_sb(struct cipca_sb *sb){

   pprintf("\n");
   pprintf("sb:%x   %s  %s  node_id:%x \n",sb,sb->name, 
                     poll_active_txt[sb->poll_active],sb->node_id);
   pprintf("\t pdevice:%x   verbose:%x   dqi:%x\n",
                     sb->pdevice,sb->verbose,sb->dqi);
   pprintf("\t stallvc:%x   vc:%x  ackrec:%x\n",sb->stallvc,sb->vc,
                          sb->ackrec);

}

void dump_all_sb(struct cixcd_blk *blk_ptr){
  int i;
  struct ci_pb *pb;
  struct cipca_sb *sb;
  pb = blk_ptr->pb;

  /* Visit each SB. */
   for (i = 0; i < pb->pb.num_sb; i++) {
     sb = pb->pb.sb[i];
     dump_ci_sb(sb);
   }
}


void query_ckt_sb(struct cipca_sb *sb)
{
    N_Q_BUFFER *nbuf;
    N_Q_BUF_HEAD *ncmd;
    N_SETCKT *p;
    XPORT_FCN_EXT *fext = sb->xport_fcn_ext;

    nbuf = ci_sa_ppd_alloc(sb, 0);
    ncmd = &nbuf->a;
    p = nbuf->b;

    ncmd->opc = SETCKT;

    ci_sa_queue_to_send(sb, nbuf);
}



void query_ckt(int n)
{
    int i, status;
    struct cipca_sb *sb;
    for (i = 0; i < ci_pbs[n]->pb.num_sb; i++) {
        sb = ci_pbs[n]->pb.sb[i];
        if (i == ci_pbs[n]->node_id) {
            continue;                   /* Don't poll ourselves. */
        }
        krn$_wait(&scs_lock);
        query_ckt_sb(sb);
        krn$_post(&scs_lock);
    }
}



#if MODULAR
void dump_n_q_buf_head(){
  pprintf("In the NPORT overlay;  need the call between\n");
}


void dump_n_carrier(){
  pprintf("In the NPORT overlay;  need the call between\n");
}
#endif

#endif
