/*
 * ci_def.h 
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
 * Abstract:	CI Definitions
 *
 * Author:	John R. Kirchoff
 *
 * Modifications:
 *
 *	jrk	20-Oct-1992	add mailbox semaphore
 *
 *      djm	24-Sep-1992	SPT size decreased from 16 to 8
 *
 *	jrk	12-May-1992	new driver scheme
 *
 *	jrk	27-Jan-1992	Initial entry.
 */
 
#define SPT_MAP_SIZE 8

volatile struct ci_pb {
    struct pb pb;
    unsigned long int node_id;
    unsigned long int vector;
    unsigned long int csr;
    unsigned long int poll;
    unsigned long int interrupt;
    unsigned long int pkt_adr;
    unsigned long int pkt_cnt;
    struct kfmsa_blk *blk_ptr;
    struct SEMAPHORE receive_s;
    struct pdt *pdt;
    struct MBX *mbx;
    struct SEMAPHORE mbx_s;	/* Mailbox synchronization semaphore */
    int (*poke)();
    } ;

union ci_status {
    char chr;
    struct {
	unsigned int fail : 1;
	unsigned int subtype : 4;
	unsigned int type : 3;
    } bits;
};

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
