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
 * Abstract:	Connection Block (CB) Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	30-May-1990	Initial entry.
 */
 
#define cb_k_closed 0
#define cb_k_con_req_out 1
#define cb_k_con_rsp_in 2
#define cb_k_open 3
#define cb_k_disc_req_out 4
#define cb_k_disc_rsp_in 5
#define cb_k_disc_match 6
#define cb_k_acc_req_out 7

volatile struct cb {
    struct QUEUE cb;
    unsigned long int ref;
    struct sb *sb;
    int (*alloc_appl_msg)();
    int (*map)();
    int (*send_appl_msg)();
    int (*send_data)();
    int (*request_data)();
    int (*unmap)();
    int (*disconnect)();
    int (*break_vc)();
    int (*discard_cb)();
    int (*appl_msg_input)();
    int (*connection_error)();
    unsigned long int driver;
    unsigned long int timeout;
    unsigned long int state;
    unsigned long int dst_id;
    unsigned long int credits;
    long int owed_credits;
    unsigned long int msgs;
    unsigned control_ip : 1;
    unsigned dequeued : 1;
    unsigned send_imm : 1;
    unsigned multihost : 1;
    unsigned fill1 : 28;
    struct QUEUE ub;
    struct QUEUE rsp;
    struct QUEUE receive_q;
    struct SEMAPHORE receive_s;
    struct SEMAPHORE control_s;
    struct SEMAPHORE credit_s;
    struct rsp *old_rsp;
    unsigned long int old_key;
    unsigned long int host_tmo;
    unsigned long int cont_tmo;
    unsigned long int alloclass;
    unsigned char dst_proc [16];
    } ;
