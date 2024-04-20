/*
 * cimna_common.c
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
 * Abstract:	CI common support routines
 *
 * Author:	John R. Kirchoff
 *
 * Modifications:
 *
 *	jrk 	04-Oct-1994	Derived from Laser
 *
 */

#define TYP2_BLD     0
#define DFQE_SIZE   512
#define DEBUG 0
#define DEBUG_QUEUE  0


#include "cp$src:platform.h"
#include "cp$src:ctype.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:common.h"
#include "cp$src:pb_def.h"
#include "cp$src:cb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:nport_q_def.h"
#include "cp$src:nport_def.h"
#include "cp$src:cipca_def.h"
#include "cp$src:ci_cipca_def.h"
#include "cp$src:print_var.h"
#include "cp$inc:kernel_entry.h"

#define __CIPCA__	1
protoif(__CIPCA__)
#include "cp$inc:prototypes.h"

#define controller_num_to_id(x,y) null_procedure()
#define _dprintf(x,y) pprintf(x,y)

#include "cp$src:cipca_debug_rtns.h"

#if DEBUG & 0
#define dprintf(fmt, args)	\
    _dprintf(fmt, args)
#else
#define dprintf(fmt, args)
#endif

#define MAX_MAINT_ID 9
int CI_MAINTENANCE_ID[] = {0x2, 0x6, 0x7, 0x8, 0xB, 0xE, 0x10, 0x11, 0x12};

#if DEBUG
extern int ci_debug_flag;
#if EMUL_DEBUG
   u_long cixcd_write_csr(u_long *csr, u_long value, char *name, int hose, struct MBX *mbx);
#endif

char *poll_active_txt[]={"POLL_OK","POLL_WAITING","POLL_NOPATH","POLL_TIMEOUT"};

char *nqueue_txt[] = {"CCQ2","CCQ1","CCQ0","DADFQ","DAMFQ",
                      "ADDFQ","ADMFQ","ADRQ","DDFQ"};


char *N_opcodes[] = {"SNDPM - send physical medium message", 
                     "PMREC - physical medium message received", 
                     "RETQE - return queue entry", 
                     "PURGQ - purge queu", 
                     "INVTC - invalidate translation cache", 
                     "RDCNT - read counters",
                     "SETCHNL - set channel", 
                     "SETCKT - set virtual circuit"};

char *ci_opcodes[] = {			/* */
  "0", "DG", "MSG", "CNF", 		/* */
  "MCNF", "IDREQ", "RST", "STRT", 	/* */
  "DAT0", "DAT1", "DAT2", "ID", 	/* */
  "PSREQ", "LB", "MDAT", "RETPS", 	/* */
  "SNDDAT", "RETDAT", "SNDMDAT", "RETMDAT"
					/* */
  };

char *ci_status_type[] = {		/* */
  "OK", 				/* */
  "VC Closed (before cmd executed)", 	/* */
  "Invalid buffer name", 		/* */
  "Buffer length violation", 		/* */
  "Access control violation", 		/* */
  "No Path", 				/* */
  "Buffer memory system error", 	/* */
  "Subtype applies"			/* */
  };

char *ci_status_subtype[] = {		/* */
  "Packet Size Violation", 		/* */
  "Unrecognized packet", 		/* */
  "Invalid DST_XPORT", 			/* */
  "Unrecognized command", 		/* */
  "Abort", 				/* */
  "Invalid xport_addr in Rec Packet", 	/* */
  "Invalid Sequence Number", 		/* */
  "Insuffficient CVCD Entries", 	/* */
  "Insufficint RESEQ Resources", 	/* */
  "Discarded VC packet", 		/* */
  "Invalid Destination Datalink error", 
					/* */
  "11 reserved", 			/* */
  "12 reserved", 			/* */
  "13 reserved", 			/* */
  "14 reserved", 			/* */
  "15 reserved"				/* */
  };

char *ci_status_nopath[] = {		/* */
  "ACK,NO_PATH,NONE", 			/* */
  "NAK", 				/* */
  "NO_RSP", 				/* */
  "ARB_TO."				/* */
  };

char *scs_state[] = {			/* */
  "closed", 				/* */
  "con_req_out", 			/* */
  "con_rsp_in", 			/* */
  "open", 				/* */
  "disc_req_out", 			/* */
  "disc_rsp_in", 			/* */
  "disc_match", 			/* */
  "acc_req_out"				/* */
  };

char *ci_ppd_mtypes[] = {		/* */
  "start", 				/* */
  "stack", 				/* */
  "ack", 				/* */
  "scs_dg", 				/* */
  "scs_msg", 				/* */
  "error_log", 				/* */
  "node_stop", 				/* */
  ""};

char *mscp_opcodes[] = {		/* */
  "0", 					/* */
  "1", 					/* */
  "get_command", 			/* */
  "get_unit", 				/* */
  "set_cont", 				/* */
  "5", 					/* */
  "6", 					/* */
  "7", 					/* */
  "available", 				/* */
  "online", 				/* */
  "set_unit", 				/* */
  "find_paths", 			/* */
  "12"};				/* */

char *ci_scs_mtype[] = {		/* */
  "con_req", 				/* */
  "con_rsp", 				/* */
  "acc_req", 				/* */
  "acc_rsp", 				/* */
  "rej_req", 				/* */
  "rej_rsp", 				/* */
  "disc_req", 				/* */
  "disc_rsp", 				/* */
  "cred_req", 				/* */
  "cred_rsp", 				/* */
  "appl_msg ", 				/* */
  "appl_dg ", 				/* */
  "12", 				/* */
  "13", 				/* */
  "14", 				/* */
  "15", 				/* */
  ""					/* */
  };

/*
 * ci_showchar -- print character in visible manner
 */
void ci_showchar(int c)
{
    c &= 0xff;
    if (isprint(c))
	dprintf("%c ", p_args1(c));
    else
	dprintf(". ", p_args0);
}
#endif


#define TRACE_BDL 0
#if TRACE_BDL

int print_bdltrace = 0;
int enable_bdltrace = 0;
int bdl_adapterfilter = 99;

#define MAX_BDL_DATA 10


typedef struct {
   int seqnum;
   int type;
   int node_id;	    /* controller node */
   int data[MAX_BDL_DATA];
} BDLLOG;

#define BDL_START 0xC00000
BDLLOG *bdlpos = BDL_START;
int bdl_seqnum = 0;


void trace_bdl(char c, struct ci_pb *pb, BDL_PTR *bdl_ptr, BDL *bdl, 
            struct cp *cb, int *buffer){
    if (!(enable_bdltrace&1)) return;
    if ((bdl_adapterfilter != pb->node_id) && (bdl_adapterfilter != 99)){
       return;
    }
      bdlpos->seqnum = bdl_seqnum++;
      bdlpos->type = (int)c;
      bdlpos->node_id = pb->node_id;
      bdlpos->data[0] = (int)bdl_ptr;
      bdlpos->data[1] = (int)bdl;
      bdlpos->data[2] = (int)cb;
      bdlpos->data[3] = (int)buffer;
      if(print_bdltrace)
        dump_BDL_record(bdlpos);
      bdlpos++;
}

void bdl_log_init(){
    bdlpos = BDL_START;
    bdl_seqnum = 0;
    memset(bdlpos,0,0x100000);
}

void dump_BDL_record(int *t){
  printf("%04d %c%x %08x %08x %08x %08x %08x %08x %08x %08x\n", 
          t[0],t[1],t[2],t[3],t[4],t[5],t[6],t[7],t[8],t[9],t[10]);
}



void dump_bdllog(int start, int end){
  BDLLOG *spos = BDL_START + (start * sizeof(BDLLOG));
  BDLLOG *epos = BDL_START + (end   * sizeof(BDLLOG));
  BDLLOG *i;
  for (i=spos; i <= epos; i++){
     dump_BDL_record(i);
  }

}

#endif



/*+
 * ============================================================================
 * = cixcd_get_nbuf     - get nport carrier and buffer for a command          =
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
 *	cixcd_get_nbuf( sb, p )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cipca_sb *sb	- system block
 *	N_CARRIER	- nport carrier.
 *	N_Q_BUFFER	- nport buffer.
 *
 * SIDE EFFECTS:
 *
 *	nport q entries are put on ddfq for use by ppd_alloc.
 *
-*/
int cixcd_get_nbuf(struct ci_pb *pb, N_CARRIER **car, N_Q_BUFFER **nbuf)
{
    struct PCB *pcb;
    int st = msg_success;

#if 0
    dprintf("\ncixcd_get_nbuf: attempting ddq removal, ddq entries %x\n",
      p_args1(pb->ddfq_entries));
#endif
   

    if (pb->ddfq_entries <= 0)
	st = cixcd_replenish_ddfq(pb, DDFQREFILL, 0, MFQ);

    st = n_driver_remove_entry(&pb->ddfq.head_ptr, car, nbuf);
    if (st != msg_success) {
	pb->car = NULL;
	pprintf("cixcd_get_nbuf: ddfq carrier removal failure car %x, buff %x, num ddfq entries %x\n",
	  *car, *nbuf, pb->ddfq_entries);
	return msg_failure;
    }

#if 0
    dump_n_carrier(*car);
    dump_n_q_buffer(*nbuf);
#endif

    pb->car = *car;
    pb->ddfq_entries--;
    memset(*nbuf, 0, sizeof(N_Q_BUFFER));

#if 0
    dprintf("\ncixcd_get_nbuf: removed ddq: st  %x, *car %x, *nbuf %x, ddfq_ent %x\n",
      p_args4(st, *car, *nbuf, pb->ddfq_entries));
#endif

    return st;
}

/*+
 * ============================================================================
 * = cixcd_replenish_ddfq - get nport carrier and buffer for a command          =
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
 *	cixcd_replenish_ddfq( pb, num_entries, queue_flag, dfq_flag)
 *  
 * RETURNS:
 *
 *	status
 *       
 * ARGUMENTS:
 *	queue_flag - Selects whether to malloc or pull entries off adapter queue
 *
 * SIDE EFFECTS:
 *
 *	nport q entries are put on ddfq for use by ppd_alloc.
 *	Entries can either come from the adapter queue or be
 *	malloced in the case RETQE instruction does not work.
 *
-*/
int cixcd_replenish_ddfq(struct ci_pb *pb, int num_entries, int queue_flag,
  int dfq_flag)
{
    N_CARRIER *local_car_ptr;
    N_Q_BUFFER *local_n_q_buf;
    int st = msg_success, i;
/*	 
**  Pull requested entries off queue.  Stuff in pb->ddfq. 
*/

    if (!num_entries) {
	num_entries = DDFQREFILL;
	queue_flag = 0;
    }
    if (queue_flag)
	for (i = 0; i < num_entries && st == msg_success; i++) {
	    if (dfq_flag)
		st =
		  ci_remove_queue(ADDFQ, pb, &local_car_ptr, &local_n_q_buf);
	    else
		st =
		  ci_remove_queue(ADMFQ, pb, &local_car_ptr, &local_n_q_buf);

	    dprintf("  replenish_ddfq, inserting into ddfq car %x, buff %x\n",
	      p_args2(local_car_ptr, local_n_q_buf));

	    if (st == msg_success)
		ci_insert_queue(DDFQ, pb, local_car_ptr, local_n_q_buf);
					/* replenish us */
	}

    if (st != msg_success || !queue_flag) {
	local_car_ptr = malloc_z(num_entries * sizeof(N_CARRIER));
	local_n_q_buf = malloc_az(num_entries * sizeof(N_Q_BUFFER), 64, 0);

	dprintf("  replenish_ddfq, malloced car %x, buff %x\n",
	  p_args2(local_car_ptr, local_n_q_buf));

	for (i = 0; i < num_entries; i++)
	    ci_insert_queue(DDFQ, pb, local_car_ptr++, local_n_q_buf++);
					/* replenish us */
    }
    return st;
}

/*	 
**  ci_remove_queue
*/
int ci_remove_queue(NQUEUE queue, struct ci_pb *pb, N_CARRIER **car_ptr,
  N_Q_BUFFER **nbuf)
{
    int st;

#if DEBUG & 0
    dprintf("ci_remove_queue : %s   ci_pb:%x  car_ptr:%x  nbuf:%x\n",
                  p_args4(nqueue_txt[queue], pb, car_ptr, nbuf));
#endif

    krn$_wait(pb->adap_s);
    st = ci_remove_queue_(queue, pb, car_ptr, nbuf);
    krn$_post(pb->adap_s);
    return st;
}
/*	 
**  ci_remove_queue_
*/

int ci_remove_queue_(NQUEUE queue, struct ci_pb *pb, N_CARRIER **car_ptr,
  N_Q_BUFFER **nbuf)
{
    int st = msg_success;

    switch (queue) {
	case ADRQ: 
#if 0
	    render_queue("ADRQ", &pb->ab->adrq.head_ptr, 0);
#endif
	    st = n_driver_remove_entry(&pb->ab->adrq.head_ptr, car_ptr, nbuf);
	    break;
	case ADDFQ: 
#if 0
	    render_queue("ADDFQ", &pb->ab->addfq.head_ptr, 0);
#endif
	    st = n_driver_remove_entry(&pb->ab->addfq.head_ptr, car_ptr, nbuf);
	    break;
	case ADMFQ: 
#if 0
	    render_queue("ADMFQ", &pb->ab->admfq.head_ptr, 0);
#endif
	    st = n_driver_remove_entry(&pb->ab->admfq.head_ptr, car_ptr, nbuf);
	    break;
	case DDFQ: 
	    if (pb->ddfq_entries <= 0)
		st = cixcd_replenish_ddfq(pb, DDFQREFILL, 0, MFQ);
	    n_driver_remove_entry(&pb->ddfq.head_ptr, car_ptr, nbuf);
      	    pb->ddfq_entries--;
	    break;
    }

    return st;
}

/*	 
**  ci_insert_queue
*/
int ci_insert_queue(NQUEUE queue, struct ci_pb *pb, N_CARRIER *car_ptr,
  N_Q_BUFFER *nbuf)
{

    krn$_wait(pb->adap_s);
    ci_insert_queue_(queue, pb, car_ptr, nbuf);
    krn$_post(pb->adap_s);
    return msg_success;
}
/*	 
**  ci_insert_queue_
*/

void ci_insert_queue_(NQUEUE queue, struct ci_pb *pb, N_CARRIER *car_ptr,
  N_Q_BUFFER *nbuf)
{
  struct cixcd_blk *blk_ptr;
  unsigned int stopper;
  blk_ptr = pb->blk_ptr;

    switch (queue) {
	case CCQ2:
	    n_insert_entry(&pb->ab->cq[0].dccq2.tail_ptr, car_ptr, nbuf);
            stopper = (unsigned int) vtop(car_ptr) >> 0;
	    wl(ccq2ir, stopper);
	    break;
	case CCQ1: 
	    n_insert_entry(&pb->ab->cq[0].dccq1.tail_ptr, car_ptr, nbuf);
            stopper = (unsigned int) vtop(car_ptr) >> 0;
	    wl(ccq1ir, stopper);
	    break;
	case CCQ0: 
	    n_insert_entry(&pb->ab->cq[0].dccq0.tail_ptr, car_ptr, nbuf);
            stopper = (unsigned int) vtop(car_ptr) >> 0;
	    wl(ccq0ir, stopper);
	    break;
	case DADFQ: 
	    n_insert_entry(&pb->ab->dadfq.tail_ptr, car_ptr, nbuf);
            stopper = (unsigned int) vtop(car_ptr) >> 0;

	    wl(adfqir, stopper);
	    break;
	case DAMFQ: 
	    n_insert_entry(&pb->ab->damfq.tail_ptr, car_ptr, nbuf);
            stopper = (unsigned int) vtop(car_ptr) >> 0;
	    wl(amfqir, stopper);
	    break;
	case DDFQ: 
	    n_driver_insert(&pb->ddfq.tail_ptr, car_ptr, nbuf);
	    pb->ddfq_entries++;
	    break;
    }
}
#if 0

/*+
 * ============================================================================
 * = ci_prefix() - Create a port specific name                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Create a port specific name from the protocol, controller and string
 *	specified.
 *  
 * FORM OF CALL:
 *  
 *	ci_prefix( pb, name, x )
 *  
 * RETURNS:
 *
 *	Pointer to created name string.
 *       
 * ARGUMENTS:
 *
 *	pb	- pointer to port block
 *	name	- pointer to block in which to create name
 *	x	- pointer to rest of name string
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
 */
int ci_prefix(struct ci_pb *pb, char *name, char *string)
{
    char cletter[MAX_NAME];

    sprintf(name, "%s%s%s", pb->pb.protocol,
      controller_num_to_id(pb->pb.controller, cletter), string);
    return (name);
}
#endif



/*+
 * ============================================================================
 * = ci_check_maint_id() - Check for units to respond to                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Looks for a particular unit and checks if okay to form VC with it   
 *	          
 *  
 * FORM OF CALL:
 *  
 *	ci_check_maint_id(rport_type)
 *  
 * RETURNS:
 *
 *	msg_success, fail                    
 *       
 * ARGUMENTS:
 *
 *	rport_type  - int, maintenance id of system sending IDREC
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
 */
int ci_check_maint_id(int rport)
{
    int i;

    for (i = 0; i < MAX_MAINT_ID; i++)
	if (rport == CI_MAINTENANCE_ID[i])
	    return msg_failure;
    return msg_success;
}

/*+
 * ============================================================================
 * = ci_scs_map - map a user's buffer for the port hardware to access         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine maps a user's buffer so that the port hardware may
 *	access it, to either copy data out of it or copy data into it.
 *  
 * FORM OF CALL:
 *  
 *	ci_scs_map( cb, buffer, len, p, q )
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
 *	unsigned long int p[3]		- output area for map information
 *	int q[4]			- scratch area for map information
 *
 * SIDE EFFECTS:
 *
 *	Mapping resources are allocated.
 *
-*/
int ci_scs_map(struct cb *cb, unsigned char *buffer, int len, unsigned long int *p,
  unsigned long int *q)
{
    int i, j, num_ptrs, num_bdl, num_typ0, num_typ1, num_pages, page0,
      page_offset, page_cross, next_page;
    char *tmp;
    unsigned long int *name;
    struct ci_pb *pb;
    BDL *bdl;
    BDL_PTR *bdl_ptr;
    TYPE_PTR *typ0_buffer, *typ0_buffer_root, *typ1_buffer;

    pb = cb->sb->pb;
    num_ptrs = pb->ab->bdlt_len / sizeof(BDL_PTR);
    num_bdl = HWRPB$_PAGESIZE / sizeof(BDL);
    bdl_ptr = (BDL_PTR *) ptov(pb->ab->bdlt_base);
    page0 = (int) vtop(buffer) & ~PAGE_MASK;
    next_page = page0 + HWRPB$_PAGESIZE;
    page_offset = (int) vtop(buffer) & PAGE_MASK;
    num_pages = 1 + len / HWRPB$_PAGESIZE;
    page_cross = len % HWRPB$_PAGESIZE ? 1 : 0;
    if (num_pages == 2 && page_cross) {
	++num_pages;
	page_cross = 0;
    }

    dprintf("\nci_scs_map bdlt_base, %x bdl_ptr pa %x, user buffer %x, length: %x, num_pages %x\n",
      p_args5(pb->ab->bdlt_base, bdl_ptr, buffer, len, num_pages));
/*
 * Look for an unused BDT.  If none are currently available, wait until
 * one becomes available.
 */
    while (1) {
	for (i = 0; i < num_ptrs; bdl_ptr++, i++)
	    if (1) {
		bdl = (BDL *) ptov(bdl_ptr->bdl_ptr << PAGE_OFFSET);
		for (j = 0; j < num_bdl; j++, bdl++) {
		    if (!bdl->v)
			goto found_bdl;
		    krn$_sleep(10);
		}
	    }
	krn$_sleep(10);
    }
found_bdl: 

#if TRACE_BDL
    trace_bdl('M',pb,bdl_ptr,bdl,cb,buffer);
#endif

#if 0
    if( bdl == 0 ){
       pprintf("num_ptrs:%x \n", num_ptrs);
       pprintf("num_bdl:%x \n", num_bdl);
       pprintf("bld_ptr:%x \n", bdl_ptr);
       pprintf("page0:%x \n",page0);
       pprintf("next_page:%x \n",next_page);
       pprintf("page_offset:%x \n",page_offset);
       pprintf("num_pages:%x \n",num_pages);
       pprintf("page_cross:%x \n",page_cross);
       pprintf("bdl:%x \n",bdl);
       pprintf("i:%x     j:%x \n",i,j);
       pprintf("sizeof BDL:%x\n",sizeof(BDL) );
       pprintf("sizeof BDL_PTR: %x\n",sizeof(BDL_PTR) );
       if (bdl_debug) do_bpt();
    }
#endif

    dprintf("ci_scs_map: bdl %x, buf_key %x, bdlt_idx %x, bdl_idx %x\n",
      p_args4(bdl, bdl->buf_name.key, bdl->buf_name.bdlt_idx,
      bdl->buf_name.bdl_idx));

    bdl->buf_len = len;
    bdl->buf_name.key = (int) (&bdl) & 0xfff;

    switch (num_pages) {
	case 0: 

	    dprintf("\n CIMNA driver error: User buffer size is 0\n", p_args0);

	    return msg_failure;
	    break;
	case 1: 

	    dprintf("cixcd_scs_map TYP0 %x pages \n", p_args1(num_pages));
	    dprintf("cixcd_scs_map page0 %x next page %x page_cross %x\n",
	      p_args3(page0, next_page, page_cross));

	    bdl->root0.type = TYP0;
	    bdl->root0.pa = page0 >> 5;
	    bdl->page_offset = page_offset;
	    if ((int) buffer + len > (int) next_page) {
		bdl->root1.type = TYP0;
		bdl->root1.pa = next_page >> 5;
	    } else {
		bdl->root1.type = 0;
		bdl->root1.pa = 0;
	    }
	    break;
	case 2: 

	    dprintf("cixcd_scs_map TYP0 %x pages \n", p_args1(num_pages));
	    dprintf("cixcd_scs_map page0 %x next page %x page_cross %x\n",
	      p_args3(page0, next_page, page_cross));

	    bdl->root0.type = TYP0;
	    bdl->root0.pa = page0 >> 5;
	    bdl->page_offset = page_offset;
	    bdl->root1.type = TYP0;
	    bdl->root1.pa = next_page >> 5;
	    break;
	default: 
/*  
** Got a heap o' pages.  See if we need type1 or 2 buffer pointers.
*/

	    /*
	     * eco 775.19 djm  Mar 14, 1996
	     * There used to be an erroneous adding of page_offset to
	     * num_pages.   num_pages need only be incremented by one
	     * if there is any partial page.  Cheapest code is to
	     * always increment num_pages.
	     */
	    num_pages++;

	    if (num_pages <= N_MAX_TYP_PTRS) {
					/*typ1 array*/

		dprintf("cixcd_scs_map TYP1, %x pages \n", p_args1(num_pages));
		dprintf("cixcd_scs_map page0 %x next page %x page_cross %x\n",
		  p_args3(page0, next_page, page_cross));
/*	 
**  Alloc an array of TYPE_PTRS. These will be typ0's
*/
		bdl->root0.type = TYP1;
		bdl->root1.type = 0;
		typ0_buffer_root =
		  (TYPE_PTR *) malloc_az(num_pages * sizeof(TYPE_PTR), 32, 0);
/*	 
**  pa now *is* the physical addr of the typ0 array.
*/
		bdl->root0.pa = ((int) vtop(typ0_buffer_root)) >> 5;
		bdl->root1.pa = 0;
/*	 
**  Bits <11:00> of page offset are the offset of the user buffer.
*/
		bdl->page_offset = page_offset;
/*	 
**  Now segment user buffer on page boundaries
*/
		for (i = 1, tmp = page0, typ0_buffer = typ0_buffer_root;
		  i <= num_pages; i++, typ0_buffer++, tmp += HWRPB$_PAGESIZE) {
		    typ0_buffer->type = TYP0;
		    typ0_buffer->pa = (int) vtop(tmp) >> 5;
		}
	    }
#if TYP2_BLD
	    else {			/* typ2 array*/

		dprintf("cixcd_scs_map TYP2\n", p_args0);

/*	 
**  Alloc array of typ1's
*/
		bdl->root0.type = TYP2;
		num_typ1 = num_pages / N_MAX_TYP_PTRS +
		  num_pages % N_MAX_TYP_PTRS ? 1 : 0;
		typ1_buffer =
		  (TYPE_PTR *) malloc_az(num_typ1 * sizeof(TYPE_PTR), 32, 0);
		bdl->root0.pa = (int) vtop(typ1_buffer);
/*	 
**  Bits <11:00> of page offset are the offset of the user buffer.
*/
		bdl->page_offset = page_offset;
/*	 
**  Segment user buffer into typ1 ptr of user pages.  Point to each seg 
**  with a typ0.
*/
		tmp = buffer;
		for (j = 1; j <= num_typ1, j <= N_MAX_TYP_PTRS;
		  j++, typ1_buffer++) {
		    typ1_buffer->type = TYP1;
		    num_typ0 =
		      num_pages >= N_MAX_TYP_PTRS ? N_MAX_TYP_PTRS : num_pages;
		    typ0_buffer = (TYPE_PTR *) malloc_az(num_typ0 *
		      sizeof(TYPE_PTR), 32, 0);
		    typ1_buffer->pa = (int) vtop(typ0_buffer);
		    for (i = 1; i <= num_typ0, i <= N_MAX_TYP_PTRS; i++,
		      typ0_buffer++, tmp += HWRPB$_PAGESIZE, num_pages--) {
			typ0_buffer->type = TYP0;
			typ0_buffer->pa = ((int) vtop(tmp) & ~PAGE_MASK) >> 5;
		    }
		}
	    }
#endif
	    break;
    }

    dprintf("ci_scs_map: \n  user buf: %x\n  mapped root0 %x %x, root1 %x %x, \
		    \nbdl %x\n  buf_key %x\n  bdlt_idx %x\n  bdl_idx %x\n",
      p_args9(buffer, bdl->root0.pa_e, bdl->root0.pa, bdl->root1.pa_e,
      bdl->root1.pa, bdl, bdl->buf_name.key, bdl->buf_name.bdlt_idx,
      bdl->buf_name.bdl_idx));
/*  
** Fill in the output area with the output of the map operation. 
*/
    name = &bdl->buf_name;
    p[0] = 0;
    p[1] = *name;
    p[2] = cb->dst_id;
/*
 * Fill in the scratch area with enough information to be able to  later
 * unmap this buffer.
 */
    q[0] = (unsigned long int) bdl->buf_name.bdlt_idx;
    q[1] = (unsigned long int) bdl->buf_name.bdl_idx;
    q[2] = (unsigned long int) *name;
    q[3] = 0;

    dprintf(" ci_scs_map  q0 %x, q1 %x, q2 %x\n", p_args3(q[0], q[1], q[2]));

    bdl_ptr->v = 1;
    bdl->v = 1;
    return msg_success;
}

/*+
 * ============================================================================
 * = ci_scs_unmap - unmap a user's buffer                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine unmaps a user's buffer.
 *  
 * FORM OF CALL:
 *  
 *	ci_scs_unmap( cb, q )
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
void ci_scs_unmap(struct cb *cb, unsigned long int *q)
{
    struct ci_pb *pb;
    BDL *bdl;
    BDL_PTR *bdl_ptr;
    TYPE_PTR *typ0_buffer, *typ1_buffer;

/*  
** Using map information from the scratch area, free the allocated BDL's
*/

    dprintf(" ci_scs_unmap  q0 %x, q1 %x, q2 %x\n", p_args3(q[0], q[1], q[2]));

    pb = cb->sb->pb;
    bdl_ptr = (BDL_PTR *) ptov(pb->ab->bdlt_base);
    bdl = (BDL *) ptov((BDL *) (bdl_ptr[q[0]].bdl_ptr << PAGE_OFFSET));

    dprintf("\nci_scs_unmap q0 %xq0 %x q1 bdlt_base, %x bdlq1 %x bdl_ptr pa %x %x\n",
      p_args7(q[0], q[1], bdl_ptr, bdl, bdl[q[1]], bdl[q[1]].root0.pa,
      bdl[q[1]].root1.pa));
    dprintf("\nci_scs_unmap  TYPE %x KEY %x\n",
      p_args2(bdl[q[1]].root0.type, bdl[q[1]].buf_name.key));

    bdl[q[1]].v = 0;
    bdl[q[1]].buf_name.key = 0;
    switch (bdl[q[1]].root0.type) {
	case TYP0: 
	    dprintf("ci_scs_unmap TYPE0 pointer\n", p_args0);
	    bdl[q[1]].root0.pa = 0;
	    bdl[q[1]].root1.pa = 0;
	    bdl[q[1]].page_offset = 0;
	    break;
	case TYP1: 
	    typ0_buffer = ptov((TYPE_PTR *) (bdl[q[1]].root0.pa << 5));
	    dprintf("\nci_scs_unmap  TYPE1 ptr: typ0 array pa %x %x, unpacked %x \n",
	      p_args3(bdl[q[1]].root0.pa_e, bdl[q[1]].root0.pa, typ0_buffer));
/*
** free entire array of type 0 pointers.
*/
	    bdl[q[1]].root0.pa = 0;
	    bdl[q[1]].root1.pa = 0;
	    bdl[q[1]].page_offset = 0;
	    free(typ0_buffer);
	    break;
	case TYP2: 
	    typ1_buffer = ptov((TYPE_PTR *) (bdl[q[1]].root0.pa << 5));
	    typ0_buffer = ptov((TYPE_PTR *) (typ1_buffer->pa << 5));
	    free(typ0_buffer);
	    free(typ1_buffer);
	    break;
    }
}

/*+
 * ============================================================================
 * = ci_alloc_bdlt - create the buffer descr. leaf table                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  Create the buffer descriptor leaf table, BDL's and buffers.
 *
 *
 * FORM OF CALL:
 *  
 *	ci_alloc_bdlt (cixcd_blk,&bdlt_ptr,&bdlt_len)
 *  
 * RETURNS:
 *
 *	msg_success or error code.
 *       
 * ARGUMENTS:
 *
 *	cixcd_blk	   - pointer to CIXCD init parameters.
 *	BDL_PTR *bdlt_base - pointer to the BDL table.  Created to be put in AB.
 *			     by caller.
 *	int      *bdlt_len  - length of table.
 *
 * SIDE EFFECTS:
 *
 *  Uses memory.	
 *
-*/
int ci_alloc_bdlt(struct cixcd_blk *cixcd_blk, BDL_PTR **bdlt_base,
  unsigned int *bdlt_len)
{
    struct ci_pb *pb = cixcd_blk->pb;
    BDL *bdl, *bdl_start;
    BDL_PTR *bdlt_ptr;
    unsigned long int i, j, bdl_idx, bdlt_idx;
/*	 
**  Create BDLT.  Size is spec'd by NPORT_DEF
**  Then do the BDL's themselves.
*/

    bdlt_ptr = pb->bdlt_ptr_blk;
    *bdlt_base = vtop(bdlt_ptr);
    *bdlt_len = N_BDLT_MAX_SIZE;
/*	 
**  put the pointer to the block of BDL's into the cixcd block
*/
    bdl = pb->bdl_blk;

/*	 
**  Loop through BDLT table.  Each BDLT entry points to a BDL which is a
**  page of BDL descriptors.
*/
    for (j = 1, bdlt_idx = 0; j <= N_BDLT_NUMBER;
      j++, bdlt_ptr++, bdlt_idx++) {

	bdlt_ptr->bdl_ptr = ((unsigned long int) vtop(bdl)) >> PAGE_OFFSET;
	bdlt_ptr->v = 0;
/*	 
**  For each page of BDL descriptors (i.e. each BDL) fill in data for
**  each descriptor.
*/
	for (i = 1, bdl_idx = 0; i <= N_BDL_NUMBER; i++, bdl++, bdl_idx++) {
/*	 
**  Indices into bdl and bdlt
*/
	    bdl->buf_name.bdl_idx = bdl_idx;
	    bdl->buf_name.bdlt_idx = bdlt_idx;
	    bdl->v = 0;
	}
    }
    return msg_success;
}

/*+
 * ============================================================================
 * = ci_dealloc_bdlt - create the buffer descr. leaf table                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  Create the buffer descriptor leaf table, BDL's and buffers.
 *
 *
 * FORM OF CALL:
 *  
 *	ci_dealloc_bdlt    (&bdlt_ptr,&bdlt_len)
 *  
 * RETURNS:
 *
 *	msg_success or error code.
 *       
 * ARGUMENTS:
 *
 *	BDL_PTR *bdlt_base - pointer to the BDL table.  Created to be put in AB.
 *			     by caller.
 *	int      bdlt_len  - length of table.
 *
 * SIDE EFFECTS:
 *
 *  Uses memory.	
 *
-*/
#if 0
int ci_dealloc_bdlt(struct cixcd_blk *cixcd_blk)
{
    free(cixcd_blk->bdl_blk);
    free(cixcd_blk->bdlt_ptr_blk);	/* free block of bdlt ptrs */
    return msg_success;
}
#endif

/*+
 * ============================================================================
 * = ci_alloc_nport - initialize a given port                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the nport queue data structures.
 *  
 * FORM OF CALL:
 *  
 *	ci_alloc_nport( ab)
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	NPORT_AB *ab			- pointer to adapter block
 *
 * SIDE EFFECTS:                                   
 *
 *	A port block is allocated and initialized for each instance of port
 *	hardware of the appropriate type.
 *
-*/
int ci_alloc_nport(struct cixcd_blk *cixcd_blk_ptr)
{

    int i;
    N_CARRIER *cp;
    N_Q_BUFFER *qp;
    struct ci_pb *pb = cixcd_blk_ptr->pb;
    NPORT_AB *ab = pb->ab;
/* 
**  Initialize the various N_Port Queues 
*/

    qp = pb->N_buffer_blk;
    cp = pb->N_carrier_blk;
    

/*	 
**  Command Queues 
*/
    krn$_wait(pb->adap_s);

#if 0
    dprintf("\n   ci_alloc_nport begin:\n", p_args0);
#endif

    n_init_queue(&ab->cq[0].dccq3.head_ptr, &ab->cq[0].dccq3.tail_ptr,
      DriverToAdapter, cp++);
    n_init_queue(&ab->cq[0].dccq2.head_ptr, &ab->cq[0].dccq2.tail_ptr,
      DriverToAdapter, cp++);
    n_init_queue(&ab->cq[0].dccq1.head_ptr, &ab->cq[0].dccq1.tail_ptr,
      DriverToAdapter, cp++);
    n_init_queue(&ab->cq[0].dccq0.head_ptr, &ab->cq[0].dccq0.tail_ptr,
      DriverToAdapter, cp++);

    n_init_queue(&ab->cq[0].cccq3.head_ptr, &ab->cq[0].cccq3.tail_ptr,
      AdapterToAdapter, cp++);
    n_init_queue(&ab->cq[0].cccq2.head_ptr, &ab->cq[0].cccq2.tail_ptr,
      AdapterToAdapter, cp++);
    n_init_queue(&ab->cq[0].cccq1.head_ptr, &ab->cq[0].cccq1.tail_ptr,
      AdapterToAdapter, cp++);
    n_init_queue(&ab->cq[0].cccq0.head_ptr, &ab->cq[0].cccq0.tail_ptr,
      AdapterToAdapter, cp++);
/*	 
**  Other Queues   
*/
    n_init_queue(&ab->adrq.head_ptr, &ab->adrq.tail_ptr, AdapterToDriver,
      cp++);
    n_init_queue(&ab->dadfq.head_ptr, &ab->dadfq.tail_ptr, DriverToAdapter,
      cp++);
    n_init_queue(&ab->addfq.head_ptr, &ab->addfq.tail_ptr, AdapterToDriver,
      cp++);
    n_init_queue(&ab->damfq.head_ptr, &ab->damfq.tail_ptr, DriverToAdapter,
      cp++);
    n_init_queue(&ab->admfq.head_ptr, &ab->admfq.tail_ptr, AdapterToDriver,
      cp++);
    n_init_queue(&ab->aadfq.head_ptr, &ab->aadfq.tail_ptr, AdapterToAdapter,
      cp++);
    n_init_queue(&ab->aamfq.head_ptr, &ab->aamfq.tail_ptr, AdapterToAdapter,
      cp++);

    n_init_queue(&pb->ddfq.head_ptr, &pb->ddfq.tail_ptr, DriverToDriver, cp++);
/*	 
**  Insert entries on free queues
*/
    for (i = 0; i < XZA_DGFREE; i++)
	ci_insert_queue_(DADFQ, pb, cp++, qp++);
    for (i = 0; i < XZA_MSGFREE; i++)
	ci_insert_queue_(DAMFQ, pb, cp++, qp++);


    pb->ddfq_entries = 0;          

    for (i = 0; i < XZA_DRIVERFREE; i++)
	ci_insert_queue_(DDFQ, pb, cp++, qp++);



    if(pb->ddfq_entries > XZA_DRIVERFREE){
	 pprintf("inconsistant ddfq_entries count current:%x  limit:%x\n",
                       pb->ddfq_entries, XZA_DRIVERFREE);
	do_bpt();
    }


#if DEBUG_QUEUE
    if (ci_debug_flag & 0x40) {

	render_queue("DCCQ2", ab->cq[0].dccq2.head_ptr, 0);
	render_queue("DCCQ1", ab->cq[0].dccq1.head_ptr, 0);
	render_queue("DCCQ0", ab->cq[0].dccq0.head_ptr, 0);
	render_queue("CCCQ2", ab->cq[0].cccq2.head_ptr, 0);
	render_queue("CCCQ1", ab->cq[0].cccq1.head_ptr, 0);
	render_queue("CCCQ0", ab->cq[0].cccq0.head_ptr, 0);

	render_queue("ADRQ",  ab->adrq.head_ptr, 0);
	render_queue("DADFQ", ab->dadfq.head_ptr, 0);
	render_queue("ADDFQ", ab->addfq.head_ptr, 0);
	render_queue("DAMFQ", ab->damfq.head_ptr, 0);
	render_queue("ADMFQ", ab->admfq.head_ptr, 0);
	render_queue("AADFQ", ab->aadfq.head_ptr, 0);
	render_queue("AAMFQ", ab->aamfq.head_ptr, 0);
	render_queue("DDFQ",  pb->ddfq.head_ptr, 0);
    }
#endif
    krn$_post(pb->adap_s);
    return msg_success;
}

/*+
 * ============================================================================
 * = ci_dealloc_nport - initialize a given port                               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine frees nport data structures
 *  
 * FORM OF CALL:
 *  
 *	ci_dealloc_nport( blk_ptr)
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cixcd_blk *blk_ptr		- pointer to adapter block
 *
 * SIDE EFFECTS:                                   
 *
 *	A port block is allocated and initialized for each instance of port
 *	hardware of the appropriate type.
 *
-*/


void ci_dealloc_nport(struct cixcd_blk *pb)
{
#if 0
    if (pb->N_carrier_blk != NULL)
	free(pb->N_carrier_blk);
    if (pb->N_buffer_blk != NULL)
	free(pb->N_buffer_blk);
    pb->N_carrier_blk = NULL;
    pb->N_buffer_blk = NULL;
#endif
    if (pb->ab != NULL)
	free(pb->ab);
}


/*	 
**  Render_queue
*/
#if DEBUG_QUEUE
void render_queue(char *qtype, N_CARRIER *car_ptr, int stopper_val)
{
    N_CARRIER *stopper;
    N_CARRIER *c;
    int cnt=0;

    c = car_ptr;
    while (c) {
      cnt++;
      pprintf("%s  %d\n",qtype,cnt);
      dump_n_carrier(c);    
      c = (N_CARRIER *) ((int) c->next_ptr & 0xfffffffe);
    }

}
#endif



/*
 * cixcd_write_csr
 */
#if EMUL_DEBUG
u_long cixcd_write_csr(u_long *csr, u_long value, char *name, int hose,
  struct MBX *mbx)
{
#if DEBUG
    if ((ci_debug_flag & 4) != 0)
	printf("  write_csr: %s\tadr = %x, data = %08x\n", name, csr, value);
#endif

/*    XMI_WRITE_L(csr, &value, hose, mbx);   */

    return 0;
}
#endif

protoendif(__CIPCA__)
