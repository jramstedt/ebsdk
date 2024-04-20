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
 * Abstract:	Port-to-Port Driver CIMNA Definitions
 *		Subnode Addressing / Explicit Format
 *
 * Author:	Charlie Crabb
 *              
 *
 * Modifications:
 *
 *	cfc	 2-Mar-1993	add    poll_status to cimna_sa_ppd_def so
 *                              it is usable in cimna_ci_sa_response.c
 *
 *	jrk	 8-Mar-1992	made max_dg bigger to prevent error from
 *				HSC error_log packet.
 *
 *	cfc	27-Feb-1992	CIMNA 
*/

#define ppd_k_max_dg_msg 256

#define DQE_LEN  512


typedef enum ci_poll_status {POLL_OK, 
                             POLL_WAITING, 
                             POLL_NOPATH, 
                             POLL_NEED_FORCE_PATH,
                             POLL_TIMEOUT,
                            } CI_POLL_STATUS;


enum   CI_OPC {
  SNDDG   =0x01,   DGSNT=0x01,	DGREC=0x01,	
  SNDMSG  =0x02,  MSGSNT=0x02,	MSGREC=0x02,	
  SNDDAT  =0x10,  DATSNT=0x10,	SMTREC=0x10,	
  RETCNF  =0x03,  CNFRET=0x03,	CNFREC=0x03,	
  REQDAT0 =0x08,   	
  REQDAT1 =0x09,   	
  REQDAT2 =0x0A,   	
  RETDAT  =0x11,  DATRET=0x11,	DATREC=0x11,	
   SNDLB  =0x11,   LBSNT=0x11,	LBREC=0x11,	
   REQID  =0x05,   IDREQ=0x05,	IDREC=0xb,	
   RETID  =0x0B,   IDRET=0x0B,	
   REQPS  =0x0C,   PSREQ=0x0C,	
   RETPS  =0x0F,   PSRET=0x0F,	
  SNDRST  =0x06,  RSTSNT=0x06,	
  SNDMDAT =0x12, MDATSNT=0x12,	
  RETMCNF =0x04, MCNFRET=0x04,	MCNFREC=0x04,	
  REQMDAT =0x0E, MDATREQ=0x0E,	
  RETMDAT =0x13, MDATRET=0x13,	MDATREC=0x13,	
  SNDSTRT =0x07, STRTSNT=0x07  
  };	
#define ppd_k_dg 1
#define ppd_k_msg 2
#define ppd_k_dgrec 1 
#define ppd_k_msgrec 2 
#define ppd_k_cnfrec 3 
#define ppd_k_idreq 5
#define ppd_k_idrec 11
#define ppd_k_rst 6
#define ppd_k_strt 7
#define ppd_k_reqdat 8
#define ppd_k_reqdat0 8
#define ppd_k_reqdat1 9
#define ppd_k_reqdat2 10
#define ppd_k_snddat 16
#define ppd_k_datrec 17

#define ppd_k_start 0
#define ppd_k_stack 1
#define ppd_k_ack 2
#define ppd_k_scs_dg 3
#define ppd_k_scs_msg 4
#define ppd_k_error_log 5
#define ppd_k_node_stop 6

#define ppd_k_p0 1
#define ppd_k_p1 2
#define ppd_k_rp_p0 2
#define ppd_k_rp_p1 4
#define ppd_k_sp_p0 16
#define ppd_k_sp_p1 32

struct ppd_header {
    struct n_q_buf_head *token;	    /* Driver address of queue buffer */
    unsigned int	 rsvd;	    /* ALPHA C compiler does not support 64 bit */
    unsigned char	 opc;	    /* Command operation code */
    unsigned char	 chnl_idx;  /* Channel index */
    unsigned short int	 flags;	    /* Operation modifier flags */
    unsigned short int   status;    /* Response status */
    unsigned char	 opcode;    /* Interconnect opcode */
    unsigned char	 i_flags;   /* Interconnect flags */
    unsigned char	 dst_member;/* Destination Xport */
    unsigned char	 dst_port;	
    unsigned char	 src_member;/* Source Xport */
    unsigned char	 src_port;
    unsigned int	 rsvd1;	    /* Reserved */
    };

volatile struct ppd_dg {
    unsigned short int length;
    unsigned short int mtype;
    } ;

volatile struct ppd_dg_start_stack {
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
    } ;

volatile struct ppd_dg_ack {
    unsigned short int length;
    unsigned short int mtype;
    } ;

volatile struct ppd_msg {
    unsigned short int length;
    unsigned short int mtype;
    } ;

volatile struct ppd_cnf {
    unsigned int xct_id [2];
    } ;

volatile struct ppd_idreq {
    unsigned int xct_id [2];
    } ;

volatile struct ppd_reqdat {
    unsigned int xct_id [2];
    unsigned long int xct_len;
    unsigned long int snd_name;
    unsigned long int snd_offset;
    unsigned long int rec_name;
    unsigned long int rec_offset;
    } ;

typedef struct xport_fcn_ext{
    unsigned int rst_mem    :8;
    unsigned int ast	    :3;
    unsigned int xpre	    :1;
    unsigned int aarb	    :1;    
    unsigned int xnr	    :1;
    unsigned int rsv	    :2;
    unsigned int mbl	    :13;
    unsigned int csz	    :3;

    unsigned int num_mems   :8 ;
    unsigned int rsv2	    :3;
    unsigned int oni        :1;    
    unsigned int nadp       :1;    
    unsigned int rdp 	    :1;    
    unsigned int fsn 	    :1;    
    unsigned int eas 	    :1;    
    unsigned int rsv3	    :15;
    unsigned int mmv 	    :1;    
    } XPORT_FCN_EXT;

volatile struct ppd_id      {
    unsigned int	    xct_id[2];		/* Transaction ID */
    unsigned int	    rport_typ 	:   31;     
    unsigned int	            d	:    1;     
    unsigned int	     code_rev;     
    unsigned int	    xport_fcn;     
    unsigned int	    rst_p_grp	:    8;     
    unsigned int	          mnt	:    1;     
    unsigned int	        pt_st	:    2;     
    unsigned int	       sys_st	:   21;     
    unsigned int     xport_fcn_ext[2];	
    unsigned int	   mem_map[2];	
    unsigned int       reserv[DQE_LEN-24];/* Res for sw */
    } ;

volatile struct ppd_snddat {
    unsigned int xct_id [2];
    unsigned long int xct_len;
    unsigned long int snd_name;
    unsigned long int snd_offset;
    unsigned long int rec_name;
    unsigned long int rec_offset;
    } ;

volatile struct ppd_dat {
    unsigned int xct_id [2];
    } ;

volatile struct ppd_setckt {
    unsigned short int mask;
    short int fill1;
    unsigned short int value;
    short int fill2;
    unsigned short int orig;
    short int fill3;
    } ;
