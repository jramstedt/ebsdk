/* File:	depfa_pb_def.h
 *
 * Copyright (C) 1994 by
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
 * Abstract:	DEFPA port block Definitions
 *
 * Author:	John R. Kirchoff
 *
 * Modifications:
 *
 *	jrk	17-Aug-1994	Initial entry.
 */
 
volatile struct FW_PB {
    struct pb 	        pb;			/* see cp$src:PB_DEF.H */
    struct PBQ		*pbq;			/* pointer to the PBQ struct */
/*    struct LOCK 	spl_port;	*/	/* Must be aligned on a quadword */
    struct INODE	*ip;			/* pointer to the associate INODE */
    struct SEMAPHORE	fw_port_s;		/* port semaphore	*/
    struct SEMAPHORE	fw_ready_s;		/* used to sync up port process */
    struct SEMAPHORE	fw_done_s;		/* completion semaphore for process */
    struct SEMAPHORE 	fw_isr_s;		/* Semaphores for interrupts*/
    struct SEMAPHORE	mbx_s;			/* synchronize access to shared mailbox */
    struct TIMERQ	fw_cmd_tqe;
    struct MBX		*mbx;			/* point to mailbox	*/
    struct FF_MSG 	rqh;			/* Recieved msg queue header*/
    FF_DESCR_BLOCK      *dbp;			/* Descriptor block */
    FF_CONSUMER_BLOCK   *cbp;			/* Consumer block pointer */
    FF_PRODUCER_INDEX   *pip;			/* Producer Index pointer */
    int			(*lrp)();		/* Pointer to the datalink rcv*/
    int			(*lwp)();		/* Pointer to send routine */
    int			fcount;			/* count of open files that not part of the driver */
    int 		type0_vector;		/* Interrupt vectors */ 
    int			type1_vector;
    int 		type2_vector;
    int			type3_vector;
    int 		rcv_msg_cnt;		/* Number of recieved messages*/
    int			CmdRspBlock;		/* Command Response buffer pointer */
    int			CmdReqBlock;		/* Command Request  buffer pointer */
    int			XmtDatBlock;		/* Xmt Data buffer pointer */
    int			UnsolBlock;		/* Command rcv buffer pointer */
    int 		password_init;	
    int			setup_init;		/* Remote boot stuff*/
    int			filter_type;
    int       		loopback;		/* fw_rx looback indicator */
    int                 *saved_first_segment;	/*Saved first segment*/
    U_INT_8		*bb;			/* Fbuse node base address */
    U_INT_8	    	sa[6];			/* Our station address */
/*    char       	name[32];		*//* Python Port Name */
/*    char 		short_name[32];		*//* Python Port Name (Short version) */
/*    char      	fb_name[6];		*//* Fbus Port Name */
/*    struct FW_CSRS  	ff;		*//* Csrs*/
/*  int 		XmtIntCnt,	*//* Xmt Interrupt counter */
/*   			RcvIntCnt,	*//* Rcv Interrupt counter */
/*   			ru_cnt;         */
/*    int 		me_cnt,		*//* fw_control_t counters */
/*    			rw_cnt,
    			tw_cnt,
    			bo_cnt;
*/
/*  struct FW_QUEUE_INFO *crq;		*//* Command request queue */
/*  unsigned char 	xmt_pi, 	*//* Producer indexes */
/*    			rcv_pi, 
    			unsol_pi; 

    unsigned char 	cmdreq_pi, 
    			cmdrsp_pi, 
    			smt_pi;

    int			msg_post_cnt; 	        */
/*    			state;			*//* Driver state*/
};
