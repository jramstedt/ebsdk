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
 * Abstract:	Mass Storage Control Protocol (MSCP) Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	30-May-1990	Initial entry.
 */
 
#define mscp_k_maxmsg 64
#define mscp_k_host_tmo 120
#define mscp_k_cont_tmo 60
#define mscp_k_cont_flag 0x80	/* enable attention messages */

#define mscp_k_nocmd 0
#define mscp_k_get_command 2
#define mscp_k_get_unit 3
#define mscp_k_set_cont 4
#define mscp_k_available 8
#define mscp_k_online 9
#define mscp_k_set_unit 10
#define mscp_k_find_paths 11
#define mscp_k_erase 18
#define mscp_k_read 33
#define mscp_k_write 34
#define mscp_k_avatn 64
#define mscp_k_dupun 65
#define mscp_k_acpth 66

#define mscp_k_end 128

#define mscp_k_success 0
#define mscp_k_invalid_cmd 1
#define mscp_k_aborted 2
#define mscp_k_unit_offline 3
#define mscp_k_unit_available 4
#define mscp_k_media_format 5
#define mscp_k_write_prot 6
#define mscp_k_compare 7
#define mscp_k_data 8
#define mscp_k_host_buffer 9
#define mscp_k_controller 10
#define mscp_k_drive 11
#define mscp_k_formatter 12
#define mscp_k_bot 13
#define mscp_k_eof 14
#define mscp_k_data_truncated 16
#define mscp_k_eot 19

#define mscp_k_unit_unknown 0
#define mscp_k_unit_disabled 8
#define mscp_k_no_media 1
#define mscp_k_inoperative 2
#define mscp_k_exclusive 16
#define mscp_k_already_in_use 32

#define mscp_suppress_recovery 256
#define mscp_suppress_correction 512
#define mscp_clear_exception 8192
#define mscp_compare 16384

#define mscp_all_class_drivers 2
#define mscp_exclusive_access 0x20
#define mscp_multihost 4

#define mscp_ssmem 0x4000
#define mscp_ssmst 0x0200

volatile struct mscp_mod_generic {
    unsigned fill1 : 8;
    unsigned suppress_recovery : 1;
    unsigned suppress_correction : 1;
    unsigned fill2 : 3;
    unsigned clear_exception : 1;
    unsigned compare : 1;
    unsigned fill3 : 1;
    } ;

#define mscp_next_unit 1

volatile struct mscp_mod_get_unit {
    unsigned next_unit : 1;
    unsigned fill1 : 15;
    } ;

#define mscp_n 127
#define mscp_a2 3968
#define mscp_a1 126976
#define mscp_a0 4063232
#define mscp_d1 130023424
#define mscp_d0 -134217728

volatile struct mscp_media_id {
    unsigned n : 7;
    unsigned a2 : 5;
    unsigned a1 : 5;
    unsigned a0 : 5;
    unsigned d1 : 5;
    unsigned d0 : 5;
    } ;

#define mscp_major 31
#define mscp_minor 65504

volatile struct mscp_status {
    unsigned major : 5;
    unsigned minor : 11;
    } ;

volatile struct mscp_generic_cmd {
    unsigned long int cmd_ref;
    unsigned short int unit;
    short int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    } ;

volatile struct mscp_generic_rsp {
    unsigned long int cmd_ref;
    unsigned short int unit;
    unsigned short int seq_num;
    unsigned char opcode;
    unsigned char flags;
    struct mscp_status status;
    } ;

volatile struct mscp_get_command_cmd {
    unsigned long int cmd_ref;
    unsigned short int unit;
    short int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    unsigned long int out_ref;
    } ;

volatile struct mscp_get_command_rsp {
    unsigned long int cmd_ref;
    unsigned short int unit;
    unsigned short int seq_num;
    unsigned char opcode;
    unsigned char flags;
    struct mscp_status status;
    unsigned long int out_ref;
    unsigned long int cmd_sts;
    } ;

volatile struct mscp_get_unit_cmd {
    unsigned long int cmd_ref;
    unsigned short int unit;
    short int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    } ;

volatile struct mscp_get_unit_rsp {
    unsigned long int cmd_ref;
    unsigned short int unit;
    unsigned short int seq_num;
    unsigned char opcode;
    unsigned char flags;
    struct mscp_status status;
    unsigned short int mlt_unit;
    unsigned short int unit_flag;
    long int fill1;
    unsigned int unit_id [2];
    struct mscp_media_id media_id;
    unsigned short int shdw_unit;
    unsigned short int shdw_sts;
    unsigned short int track;
    unsigned short int group;
    unsigned short int cylinder;
    unsigned char unit_sv;
    unsigned char unit_hv;
    unsigned short int rct_size;
    unsigned char rbns;
    unsigned char rct_copies;
    } ;

volatile struct mscp_set_cont_cmd {
    unsigned long int cmd_ref;
    short int fill1;
    short int fill2;
    unsigned char opcode;
    char fill3;
    unsigned short int modifier;
    unsigned short int version;
    unsigned short int cont_flag;
    unsigned short int host_tmo;
    short int fill4;
    unsigned int time [2];
    unsigned long int cont_dvdp;
    } ;

volatile struct mscp_set_cont_rsp {
    unsigned long int cmd_ref;
    unsigned char alloc;
    char fill1;
    unsigned short int seq_num;
    unsigned char opcode;
    unsigned char flags;
    struct mscp_status status;
    unsigned short int version;
    unsigned short int cont_flag;
    unsigned short int cont_tmo;
    unsigned char cont_sv;
    unsigned char cont_hv;
    unsigned int cont_id [2];
    unsigned long int max_byte_cnt;
    } ;

volatile struct mscp_available_cmd {
    unsigned long int cmd_ref;
    unsigned short int unit;
    short int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    } ;

volatile struct mscp_available_rsp {
    unsigned long int cmd_ref;
    unsigned short int unit;
    unsigned short int seq_num;
    unsigned char opcode;
    unsigned char flags;
    struct mscp_status status;
    } ;

volatile struct mscp_online_cmd {
    unsigned long int cmd_ref;
    unsigned short int unit;
    short int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    short int fill3;
    unsigned short int unit_flag;
    unsigned long int fill4;
    unsigned int fill5 [2];
    unsigned long int unit_dvdp;
    unsigned long int fill6;
    } ;

volatile struct mscp_online_rsp {
    unsigned long int cmd_ref;
    unsigned short int unit;
    unsigned short int seq_num;
    unsigned char opcode;
    unsigned char flags;
    struct mscp_status status;
    unsigned short int mlt_unit;
    unsigned short int unit_flag;
    long int fill1;
    unsigned int unit_id [2];
    struct mscp_media_id media_id;
    long int fill2;
    unsigned long int unit_size;
    unsigned long int serial_number;
    } ;

volatile struct mscp_set_unit_cmd {
    unsigned long int cmd_ref;
    unsigned short int unit;
    short int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    short int fill3;
    unsigned short int unit_flag;
    unsigned long int fill4;
    unsigned int fill5 [2];
    unsigned long int unit_dvdp;
    unsigned long int fill6;
    } ;

volatile struct mscp_set_unit_rsp {
    unsigned long int cmd_ref;
    unsigned short int unit;
    unsigned short int seq_num;
    unsigned char opcode;
    unsigned char flags;
    struct mscp_status status;
    unsigned short int mlt_unit;
    unsigned short int unit_flag;
    long int fill1;
    unsigned int unit_id [2];
    struct mscp_media_id media_id;
    long int fill2;
    unsigned long int unit_size;
    unsigned long int serial_number;
    } ;

volatile struct mscp_find_paths_cmd {
    unsigned long int cmd_ref;
    unsigned short int unit;
    short int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    } ;

volatile struct mscp_find_paths_rsp {
    unsigned long int cmd_ref;
    unsigned short int unit;
    unsigned short int seq_num;
    unsigned char opcode;
    unsigned char flags;
    struct mscp_status status;
    } ;

volatile struct mscp_erase_cmd {
    unsigned long int cmd_ref;
    unsigned short int unit;
    short int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    unsigned long int size;
    unsigned long int fill3 [3];
    unsigned long int lbn;
    } ;

volatile struct mscp_erase_rsp {
    unsigned long int cmd_ref;
    unsigned short int unit;
    short int seq_num;
    unsigned char opcode;
    unsigned char flags;
    struct mscp_status status;
    unsigned long int size;
    unsigned long int fill1 [3];
    unsigned long int bad_lbn;
    } ;

volatile struct mscp_read_cmd {
    unsigned long int cmd_ref;
    unsigned short int unit;
    short int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    unsigned long int size;
    unsigned long int buffer [3];
    unsigned long int lbn;
    } ;

volatile struct mscp_read_rsp {
    unsigned long int cmd_ref;
    unsigned short int unit;
    short int seq_num;
    unsigned char opcode;
    unsigned char flags;
    struct mscp_status status;
    unsigned long int size;
    unsigned long int fill1 [3];
    unsigned long int bad_lbn;
    } ;

volatile struct mscp_write_cmd {
    unsigned long int cmd_ref;
    unsigned short int unit;
    short int fill1;
    unsigned char opcode;
    char fill2;
    unsigned short int modifier;
    unsigned long int size;
    unsigned long int buffer [3];
    unsigned long int lbn;
    } ;

volatile struct mscp_write_rsp {
    unsigned long int cmd_ref;
    unsigned short int unit;
    short int seq_num;
    unsigned char opcode;
    unsigned char flags;
    struct mscp_status status;
    unsigned long int size;
    unsigned long int fill1 [3];
    unsigned long int bad_lbn;
    } ;

struct mscp_cb {
    struct QUEUE mscp_cb;
    struct cb *cb;
    } ;

volatile struct rsp {
    struct QUEUE rsp;
    struct SEMAPHORE sem;
    struct cb *cb;
    unsigned long int incarn;
    unsigned char buffer [mscp_k_maxmsg];
    unsigned long int size;
    unsigned active : 1;
    unsigned timed : 1;
    unsigned fill1 : 30;
    unsigned long int key;
    unsigned long int cmd_sts;
    unsigned short int unit;
    } ;
