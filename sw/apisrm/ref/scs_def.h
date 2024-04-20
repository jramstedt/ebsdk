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
 * Abstract:	System Communication Services (SCS) Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	30-May-1990	Initial entry.
 */
 
#define scs_k_con_req 0
#define scs_k_con_rsp 1
#define scs_k_acc_req 2
#define scs_k_acc_rsp 3
#define scs_k_rej_req 4
#define scs_k_rej_rsp 5
#define scs_k_disc_req 6
#define scs_k_disc_rsp 7
#define scs_k_cred_req 8
#define scs_k_cred_rsp 9
#define scs_k_appl_msg 10
#define scs_k_appl_dg 11

#define scs_k_normal 1
#define scs_k_nomatch 10
#define scs_k_disconnected 26

volatile struct scs_msg {
    unsigned short int mtype;
    unsigned short int credit;
    unsigned long int dst_id;
    unsigned long int src_id;
    } ;

volatile struct scs_con_req {
    unsigned short int mtype;
    unsigned short int credit;
    unsigned long int dst_id;
    unsigned long int src_id;
    unsigned short int min_credit;
    unsigned short int mbz;
    unsigned char dst_proc [16];
    unsigned char src_proc [16];
    unsigned char src_data [16];
    } ;

volatile struct scs_con_rsp {
    unsigned short int mtype;
    unsigned short int credit;
    unsigned long int dst_id;
    unsigned long int src_id;
    unsigned short int mbz;
    unsigned short int status;
    } ;

volatile struct scs_acc_req {
    unsigned short int mtype;
    unsigned short int credit;
    unsigned long int dst_id;
    unsigned long int src_id;
    unsigned short int min_credit;
    unsigned short int mbz;
    unsigned char dst_proc [16];
    unsigned char src_proc [16];
    unsigned char src_data [16];
    } ;

volatile struct scs_acc_rsp {
    unsigned short int mtype;
    unsigned short int credit;
    unsigned long int dst_id;
    unsigned long int src_id;
    unsigned short int mbz;
    unsigned short int status;
    } ;

volatile struct scs_rej_req {
    unsigned short int mtype;
    unsigned short int credit;
    unsigned long int dst_id;
    unsigned long int src_id;
    unsigned short int mbz;
    unsigned short int status;
    } ;

volatile struct scs_rej_rsp {
    unsigned short int mtype;
    unsigned short int credit;
    unsigned long int dst_id;
    unsigned long int src_id;
    } ;

volatile struct scs_disc_req {
    unsigned short int mtype;
    unsigned short int credit;
    unsigned long int dst_id;
    unsigned long int src_id;
    unsigned short int mbz;
    unsigned short int reason;
    } ;

volatile struct scs_disc_rsp {
    unsigned short int mtype;
    unsigned short int credit;
    unsigned long int dst_id;
    unsigned long int src_id;
    } ;

volatile struct scs_cred_req {
    unsigned short int mtype;
    unsigned short int credit;
    unsigned long int dst_id;
    unsigned long int src_id;
    } ;

volatile struct scs_cred_rsp {
    unsigned short int mtype;
    unsigned short int credit;
    unsigned long int dst_id;
    unsigned long int src_id;
    } ;

volatile struct scs_appl_msg {
    unsigned short int mtype;
    unsigned short int credit;
    unsigned long int dst_id;
    unsigned long int src_id;
    } ;

volatile struct scs_appl_dg {
    unsigned short int mtype;
    unsigned short int credit;
    unsigned long int dst_id;
    unsigned long int src_id;
    } ;
