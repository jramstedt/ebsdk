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
 * Abstract:	Diagnostic Utility Protocol (DUP) Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	20-Jun-1990	Initial entry.
 */
 
#define dup_k_maxmsg 64
#define dup_k_host_tmo 30
#define dup_k_cont_tmo 3

#define dup_k_get_cont 1
#define dup_k_exe_local 3
#define dup_k_send 4
#define dup_k_receive 5
#define dup_k_send_imm 7
#define dup_k_end 128

#define dup_k_success 0
#define dup_k_invalid_cmd 1
#define dup_k_region_unavailable 2
#define dup_k_region_unsuitable 3
#define dup_k_unknown_program 4
#define dup_k_load_failure 5
#define dup_k_standalone 6
#define dup_k_host_buffer 9
#define dup_k_aborted 31

#define dup_allow_standalone 1

volatile struct dup_mod_exe_local {
    unsigned allow_standalone : 1;
    unsigned fill1 : 15;
    } ;

#define dup_to_be_cont 8

volatile struct dup_mod_send_imm {
    unsigned fill1 : 2;
    unsigned to_be_cont : 1;
    unsigned fill2 : 13;
    } ;

#define dup_major 31
#define dup_minor 65504

volatile struct dup_status {
    unsigned major : 5;
    unsigned minor : 11;
    } ;

volatile struct dup_generic_rsp {
    unsigned long int cmd_ref;
    long int fill1;
    unsigned char opcode;
    unsigned char flags;
    struct dup_status status;
    } ;

volatile struct dup_get_cont_cmd {
    unsigned long int cmd_ref;
    long int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    } ;

volatile struct dup_get_cont_rsp {
    unsigned long int cmd_ref;
    long int fill1;
    unsigned char opcode;
    char fill2;
    struct dup_status status;
    unsigned char extension [3];
    unsigned char cont_flag;
    unsigned long int progress;
    unsigned short int cont_tmo;
    unsigned char abort_tmo;
    } ;

volatile struct dup_exe_local_cmd {
    unsigned long int cmd_ref;
    long int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    unsigned char program [6];
    } ;

volatile struct dup_exe_local_rsp {
    unsigned long int cmd_ref;
    long int fill1;
    unsigned char opcode;
    char fill2;
    struct dup_status status;
    unsigned short int version;
    unsigned char prog_tmo;
    unsigned char prog_flag;
    } ;

volatile struct dup_send_cmd {
    unsigned long int cmd_ref;
    long int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    unsigned long int size;
    unsigned long int buffer [3];
    } ;

volatile struct dup_send_rsp {
    unsigned long int cmd_ref;
    long int fill1;
    unsigned char opcode;
    char fill2;
    struct dup_status status;
    unsigned long int size;
    } ;

volatile struct dup_receive_cmd {
    unsigned long int cmd_ref;
    long int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    unsigned long int size;
    unsigned long int buffer [3];
    } ;

volatile struct dup_receive_rsp {
    unsigned long int cmd_ref;
    long int fill1;
    unsigned char opcode;
    char fill2;
    struct dup_status status;
    unsigned long int size;
    } ;

volatile struct dup_send_imm_cmd {
    unsigned long int cmd_ref;
    long int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    unsigned short int size;
    unsigned char text [22];
    } ;

volatile struct dup_send_imm_rsp {
    unsigned long int cmd_ref;
    long int fill1;
    unsigned char opcode;
    char fill2;
    struct dup_status status;
    unsigned char extension [3];
    unsigned char cont_flag;
    unsigned long int progress;
    unsigned short int cont_tmo;
    unsigned char abort_tmo;
    } ;

struct dup_cb {
    struct QUEUE dup_cb;
    struct cb *cb;
    } ;

volatile struct rsp {
    struct QUEUE rsp;
    struct SEMAPHORE sem;
    unsigned char buffer [dup_k_maxmsg];
    unsigned long int size;
    unsigned active : 1;
    unsigned fill1 : 31;
    unsigned long int key;
    } ;
