/*
 * Copyright (C) 1990, 1991 by
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
 * Abstract: NI environment variable Definitions
 *
 * Author:	John A. DeNisco
 *
 * Modifications:
 *
 *	mdr	09-Jul-1993	Conditionalize for Medulla.
 *
 *	pel	15-Jan-1993	Conditionalize for morgan.
 *
 *	jds	11-Jul-1991	Conditionalized port-specific environment variables.
 *
 *	hcb	29-Mar-1991	Add loop_count for MOP loop
 *				requester.
 *
 *	jad	08-Feb-1990	Driver flags.
 *
 *	jad	24-Sep-1990	Initial entry.
 */

volatile struct NI_ENV {
/*Port variables*/
	long int mode;			/*Mode information*/
	long int xmt_buf_no;		/*Number of transmit buffers*/
	long int xmt_mod;		/*XMT Descriptor alignment modulus*/
	long int xmt_rem;		/*XMT Descriptor alignment remainder*/
	long int xmt_max_size;		/*Maximum transmit buffer size*/
	long int xmt_int_msg;		/*Transmit interrupts per message*/
	long int xmt_msg_post;		/*Messages before posting XMT*/
	long int xmt_full_post;		/*Full conditions before post*/
	long int rcv_buf_no;		/*Number of receive buffers*/
	long int rcv_mod;		/*Receive Descriptor alignment modulus*/
	long int rcv_rem;		/*Receive Descriptor alignment remainder*/
	long int msg_prefix_size;	/*Message buffer size*/
	long int msg_buf_size;		/*Message buffer size*/
	long int msg_mod;		/*Message alignment modulus*/
	long int msg_rem;		/*Message alignment remainder*/
	long int driver_flags;		/*Driver flags*/

/*Class variables*/	
	long int version;		/*Mop version for requesters*/
	long int loop_size;		/*Loop requester message size*/
	long int loop_inc;		/*Loop requester increment*/
	long int loop_patt;		/*Loop requester pattern*/
	long int loop_count;		/*Loop requester count*/
	long int lp_msg_node;		/*Loop messages node*/
    };                                    
