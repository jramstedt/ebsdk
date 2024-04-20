/*
 * kfesa_def.h
 *
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
 * Abstract:	KFESA Definitions
 *
 * Author:	Console Firmware Team
 *
 */
 
#define kfesa_shac		0x800
#define kfesa_misc_irq7		0x00000000
#define kfesa_misc_irq9		0x00001000
#define kfesa_misc_irq10	0x00002000
#define kfesa_misc_irq11	0x00003000
#define kfesa_misc_irq12	0x00004000
#define kfesa_misc_irq14	0x00005000
#define kfesa_misc_irq15	0x00006000
#define kfesa_misc_ng		0x00007000
#define kfesa_misc_wuld		0x00000001
#define kfesa_misc_id		0x00000700
#define kfesa_misc_irq		0x00007000
#define kfesa_misc_edge		0x00010000
#define kfesa_misc_oe		0x00020000

#define kfesa_product		0xc80
#define kfesa_misc		0xc88
#define kfesa_address_ext	0xc8c
#define kfesa_lock		0xc90

#define ADDRESS_EXTENDED	0xc0000000

#define MASK_SHAC_WRITE_UNLOCK	1

volatile struct kfesa_pb {
    struct pb pb;
    unsigned long int node_id;
    struct shac_csr *csr;
    struct sshma *sshma;
    struct SEMAPHORE adap_s;	        /* Owner of adapter */
    struct SEMAPHORE receive_s;
    struct SEMAPHORE receive_done_s;
    struct pdt *pdt;
    int	kfesa_receive_pid;
};

#define ppd_k_max_dg_msg 128

#define ppd_k_dg	1
#define ppd_k_msg	2
#define ppd_k_idreq	5
#define ppd_k_rst	6
#define ppd_k_strt	7
#define ppd_k_reqdat	8
#define ppd_k_snddat	16
#define ppd_k_setckt	25
#define ppd_k_dgrec	33
#define ppd_k_msgrec	34
#define ppd_k_cnfrec	35
#define ppd_k_idrec	43
#define ppd_k_datrec	49

#define ppd_k_start	0
#define ppd_k_stack	1
#define ppd_k_ack	2
#define ppd_k_scs_dg	3
#define ppd_k_scs_msg	4

#define ppd_k_p0	1
#define ppd_k_p1	2
#define ppd_k_rp_p0	2
#define ppd_k_rp_p1	4
#define ppd_k_sp_p0	16
#define ppd_k_sp_p1	32

volatile struct ppd_header {
    struct QUEUE ppd_header;
    unsigned short int dg;
    unsigned short int perm;
    unsigned char port;
    unsigned char status;
    unsigned char opcode;
    unsigned char flags;
};

volatile struct ppd_dg {
    struct QUEUE ppd_header;
    unsigned short int dg;
    unsigned short int perm;
    unsigned char port;
    unsigned char status;
    unsigned char opcode;
    unsigned char flags;
    unsigned short int length;
    unsigned short int mtype;
};

volatile struct ppd_dg_start_stack {
    struct QUEUE ppd_header;
    unsigned short int dg;
    unsigned short int perm;
    unsigned char port;
    unsigned char status;
    unsigned char opcode;
    unsigned char flags;
    unsigned short int length;
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
};

volatile struct ppd_dg_ack {
    struct QUEUE ppd_header;
    unsigned short int dg;
    unsigned short int perm;
    unsigned char port;
    unsigned char status;
    unsigned char opcode;
    unsigned char flags;
    unsigned short int length;
    unsigned short int mtype;
};

volatile struct ppd_msg {
    struct QUEUE ppd_header;
    unsigned short int dg;
    unsigned short int perm;
    unsigned char port;
    unsigned char status;
    unsigned char opcode;
    unsigned char flags;
    unsigned short int length;
    unsigned short int mtype;
};

volatile struct ppd_cnf {
    struct QUEUE ppd_header;
    unsigned short int dg;
    unsigned short int perm;
    unsigned char port;
    unsigned char status;
    unsigned char opcode;
    unsigned char flags;
    unsigned int xct_id [2];
};

volatile struct ppd_idreq {
    struct QUEUE ppd_header;
    unsigned short int dg;
    unsigned short int perm;
    unsigned char port;
    unsigned char status;
    unsigned char opcode;
    unsigned char flags;
    unsigned int xct_id [2];
};

volatile struct ppd_reqdat {
    struct QUEUE ppd_header;
    unsigned short int dg;
    unsigned short int perm;
    unsigned char port;
    unsigned char status;
    unsigned char opcode;
    unsigned char flags;
    unsigned int xct_id [2];
    unsigned long int xct_len;
    unsigned long int snd_name;
    unsigned long int snd_offset;
    unsigned long int rec_name;
    unsigned long int rec_offset;
};

volatile struct ppd_id {
    struct QUEUE ppd_header;
    unsigned short int dg;
    unsigned short int perm;
    unsigned char port;
    unsigned char status;
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
};

volatile struct ppd_snddat {
    struct QUEUE ppd_header;
    unsigned short int dg;
    unsigned short int perm;
    unsigned char port;
    unsigned char status;
    unsigned char opcode;
    unsigned char flags;
    unsigned int xct_id [2];
    unsigned long int xct_len;
    unsigned long int snd_name;
    unsigned long int snd_offset;
    unsigned long int rec_name;
    unsigned long int rec_offset;
};

volatile struct ppd_dat {
    struct QUEUE ppd_header;
    unsigned short int dg;
    unsigned short int perm;
    unsigned char port;
    unsigned char status;
    unsigned char opcode;
    unsigned char flags;
    unsigned int xct_id [2];
};

volatile struct ppd_setckt {
    struct QUEUE ppd_header;
    unsigned short int dg;
    unsigned short int perm;
    unsigned char port;
    unsigned char status;
    unsigned char opcode;
    unsigned char flags;
    unsigned short int mask;
    short int fill1;
    unsigned short int value;
    short int fill2;
    unsigned short int orig;
    short int fill3;
};

typedef union p_status {
    char status;
    struct {
	unsigned int fail : 1;	/* Failure indicator */
	unsigned int type : 7;	/* Failure type */
    } l;
}  P_STATUS;

