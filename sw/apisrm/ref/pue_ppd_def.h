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
 * Abstract:	Port-to-Port Driver (DSSI) Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	30-May-1995	Initial entry.
 */
 
#define ppd_k_max_dg_msg 128

#define ppd_k_dg 1
#define ppd_k_msg 2
#define ppd_k_cnf 3
#define ppd_k_idreq 5
#define ppd_k_rst 6
#define ppd_k_strt 7
#define ppd_k_datreq0 8
#define ppd_k_datreq1 9
#define ppd_k_datreq2 10
#define ppd_k_id 11
#define ppd_k_lb 13
#define ppd_k_sntdat 16
#define ppd_k_retdat 17

#define ppd_k_start 0
#define ppd_k_stack 1
#define ppd_k_ack 2
#define ppd_k_scs_dg 3
#define ppd_k_scs_msg 4

#define ppd_k_p0 1
#define ppd_k_p1 2
#define ppd_k_rp_p0 2
#define ppd_k_rp_p1 4
#define ppd_k_sp_p0 16
#define ppd_k_sp_p1 32

#define ppd_k_lp 1

volatile struct ppd_header {
    struct QUEUE ppd_header;
    struct dssi_header dssi_header;
    unsigned char opcode;
    unsigned char flags;
    } ;

volatile struct ppd_dg {
    struct QUEUE ppd_header;
    struct dssi_header dssi_header;
    unsigned char opcode;
    unsigned char flags;
    unsigned short int mtype;
    } ;

volatile struct ppd_dg_start_stack {
    struct QUEUE ppd_header;
    struct dssi_header dssi_header;
    unsigned char opcode;
    unsigned char flags;
    unsigned short int mtype;
    unsigned char systemid [6];
    unsigned char protocol;
    unsigned char mbz;
    unsigned short int maxdg;
    unsigned short int maxmsg;
    unsigned char swtype [4];
    unsigned char swvers [4];
    unsigned int incarn [2];
    unsigned char hwtype [4];
    unsigned char hwvers [12];
    unsigned char nodename [8];
    unsigned int curtime [2];
    } ;

volatile struct ppd_dg_ack {
    struct QUEUE ppd_header;
    struct dssi_header dssi_header;
    unsigned char opcode;
    unsigned char flags;
    unsigned short int mtype;
    } ;

volatile struct ppd_msg {
    struct QUEUE ppd_header;
    struct dssi_header dssi_header;
    unsigned char opcode;
    unsigned char flags;
    unsigned short int mtype;
    } ;

volatile struct ppd_cnf {
    struct QUEUE ppd_header;
    struct dssi_header dssi_header;
    unsigned char opcode;
    unsigned char flags;
    unsigned int xct_id [2];
    } ;

volatile struct ppd_idreq {
    struct QUEUE ppd_header;
    struct dssi_header dssi_header;
    unsigned char opcode;
    unsigned char flags;
    unsigned int xct_id [2];
    } ;

volatile struct ppd_datreq {
    struct QUEUE ppd_header;
    struct dssi_header dssi_header;
    unsigned char opcode;
    unsigned char flags;
    unsigned int xct_id [2];
    unsigned long int xct_len;
    unsigned long int snd_name;
    unsigned long int snd_offset;
    unsigned long int rec_name;
    unsigned long int rec_offset;
    } ;

volatile struct ppd_id {
    struct QUEUE ppd_header;
    struct dssi_header dssi_header;
    unsigned char opcode;
    unsigned char flags;
    unsigned int xct_id [2];
    unsigned long int rport_typ;
    unsigned long int rport_rev;
    unsigned long int rport_fcn;
    unsigned char rst_port;
    unsigned char rport_state;
    short int mbz;
    unsigned int port_fcn_ext [2];
    unsigned char unusedid [16];
    } ;

volatile struct ppd_sntdat {
    struct QUEUE ppd_header;
    struct dssi_header dssi_header;
    unsigned char opcode;
    unsigned char flags;
    unsigned int xct_id [2];
    unsigned long int name;
    unsigned long int offset;
    } ;

volatile struct ppd_retdat {
    struct QUEUE ppd_header;
    struct dssi_header dssi_header;
    unsigned char opcode;
    unsigned char flags;
    unsigned int xct_id [2];
    unsigned long int name;
    unsigned long int offset;
    } ;
