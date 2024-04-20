/*
 * ci_cimna_def.h 
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
 * Abstract:	CIPCA Definitions
 *
 * Author:	Dennis Mazur
 *
 * Modifications:
 *
 *	djm	14-Jul-1995	Initial entry.
 */
 
#define XZA_CHANNELS 1
#define CIMNA_NODES 16
#define N_ADAP_DG_REFILL 8 

#define XZA_DRIVERFREE 64
#define XZA_MSGFREE 32
#define XZA_DGFREE 32

#define CIMNA_CHANNEL_Q 32
#define CIMNA_ADAPTER_Q 7
#define CIMNA_DRIVER_Q  1
#define XZA_BUFFERS ( XZA_DRIVERFREE + XZA_DGFREE + XZA_MSGFREE)
#define XZA_NUM_QUEUES (CIMNA_CHANNEL_Q  * XZA_CHANNELS) + CIMNA_ADAPTER_Q + CIMNA_DRIVER_Q
#define MINDDFQ 3
#define DDFQREFILL 8

#define N_BDLT_NUMBER   1
#define N_BDL_NUMBER    HWRPB$_PAGESIZE/sizeof(BDL)
#define N_BDLT_MAX_SIZE N_BDLT_NUMBER * sizeof(BDL_PTR)
#define N_BDL_MAX_SIZE  N_BDLT_NUMBER * HWRPB$_PAGESIZE
#define N_CARRIER_POOL_SZ ( sizeof( struct n_carrier ) * ( XZA_BUFFERS + XZA_NUM_QUEUES) )
#define N_Q_BUFFER_POOL_SZ ( sizeof( struct n_q_buffer ) * XZA_BUFFERS )

typedef enum adapter_state {Unknown, Uninitialized, Disabled, Enabled} 
   ADAPTER_STATE;
typedef enum fq {MFQ,DFQ} FQ;
typedef enum sntstate {SNT,RCV} SNTSTATE;
typedef enum   nqueue {CCQ2,CCQ1,CCQ0,DADFQ,DAMFQ,ADDFQ,ADMFQ,ADRQ,DDFQ} NQUEUE;
enum N_PATH_STATUS {N_PATH_ACK=0, N_PATH_NAK=1, N_PATH_NO_RSP=2, N_PATH_ARB_TO=3};

volatile struct ci_pb {
    struct pb pb;
    unsigned long int node_id;
    unsigned long int vector;
    unsigned long int vector2;
    unsigned long int hard_base;
    unsigned long int soft_base;
    unsigned long int mem_base;
    unsigned long int poll;
    unsigned long int interrupt;
    struct kfmsa_blk *blk_ptr;
    struct SEMAPHORE *adap_s;
    NPORT_AB *ab ;		/* Adapter block */
    N_QP   ddfq;		/* Driver-Driver Free Queue */
    int	   ddfq_entries;	/* number on ddfq */
    N_Q_BUFFER	*N_buffer_blk;    /* nport command buffer block */
    N_CARRIER   *N_carrier_blk;	/* nport carrier block */
    BDL_PTR *bdlt_ptr_blk;
    BDL     *bdl_blk;
    N_CARRIER *car;		/* carrier of command */
    struct MBX *mbx;
    int (*poke)();
    ADAPTER_STATE adapter_state;
    } ;

union ci_status {
    char chr;
    struct {
	unsigned int fail : 1;
	unsigned int subtype : 4;
	unsigned int type : 3;
    } bits;
};

/* 
** CI command flags.  See nport spec table 7-4 
*/

typedef union   ci_flags {
    unsigned char  i;		/* Flags as a char (8 bits ) */
    struct {
	unsigned    lp  : 1;	/* last packet      */
	unsigned    ns  : 3;	/* Sequence number  */
	unsigned    m	: 3;
	unsigned    	: 1;
	} LP;
    struct {
	unsigned        : 4;
	unsigned    sp	: 2;
	unsigned	: 1;
	unsigned    fr	: 1;
	} SP;
    struct {
	unsigned	: 7;
	unsigned    dsa	: 1;
	} DSA;
    struct {
	unsigned	: 6;
	unsigned  w_sel	: 2;
	} W_SEL;
    } CI_FLAGS;
typedef union  xport {
    short int i;
    struct {
	char member;
	char p_grp;
	}addr;
    }XPORT;

volatile struct pqb {
    struct QUEUE cmdq0;
    struct QUEUE cmdq1;
    struct QUEUE cmdq2;
    struct QUEUE cmdq3;
    struct QUEUE rspq;
    struct QUEUE *dfq_head;
    struct QUEUE *mfq_head;
    unsigned long int dqe_size;
    unsigned long int mqe_size;
    unsigned long int pqb_base;
    unsigned long int bdt_base;
    unsigned long int bdt_size;
    unsigned long int spt_base;
    unsigned long int spt_size;
    unsigned long int gpt_base;
    unsigned long int gpt_size;
    long int fill1 [107];
    } ;

volatile struct bdt {
    unsigned byte_offset : 9;
    unsigned fill1 : 6;
    unsigned valid : 1;
    unsigned short int key;
    unsigned long int byte_count;
    unsigned long int svapte;
    long int fill2;
    } ;

volatile struct pdt {
    struct pqb pqb;
    struct bdt bdt [32];
    struct QUEUE dfq;
    struct QUEUE mfq;
    } ;
