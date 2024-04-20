/* file:	mopdl_driver.c
 *
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:                               
 *                                              
 *      Cobra Firmware
 *
 *  ABSTRACT:	MOP Datalink driver.
 *
 *	This module implements the network datalink functions.
 *
 *  AUTHORS:                                     
 *
 *	John DeNisco                                                     
 *
 *  CREATION DATE:
 *  
 *      08-APR-1990
 *
 *--
 */

/*Include files*/

#include "cp$src:platform.h"
#include "cp$src:common.h"		/*Common definitions*/
#include "cp$src:kernel_def.h"		/*Kernel definitions*/
#include "cp$src:inet.h"
#include "cp$src:prdef.h"		/* Processor definitions*/
#include "cp$src:dynamic_def.h"		/*Memory allocation defs*/
#include "cp$src:msg_def.h"		/*Message definitions*/
#include "cp$src:ni_env.h"		/*Environment variables*/
#include "cp$src:mop_def.h"		/*MOP definitions*/
#include "cp$src:mb_def.h"		/*MOP block defs*/
#include "cp$src:mop_counters.h"	/*MOP counters*/
#include "cp$src:ev_def.h"		/*Environment vars*/
#include "cp$src:ctype.h"		/*ASCII stuff*/
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

/*Some debug constants*/

#if MODULAR
#define DEBUG 0
#else
#define DEBUG 0
#endif

/* Debug routines */
/*
 * debug flag
 * 1 = top level
 * 2 = malloc and free
 * 4 = print packets
 * 100 = qprintf
 */

#if DEBUG
int mopdl_debug = 0;
#include "cp$src:print_var.h"
#define dqprintf _dqprintf
#define dprintf(fmt, args) _dprintf(fmt, args)
#define d2printf(fmt, args) _d2printf(fmt, args)
#define d4printf(fmt, args) _d4printf(fmt, args)
#else
#define dqprintf qprintf
#define dprintf(fmt, args)
#define d2printf(fmt, args)
#define d4printf(fmt, args)
#endif

#if DEBUG
static void _dprintf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((mopdl_debug & 1) != 0) {
	if ((mopdl_debug & 0x100) == 0)
	    pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
	else
	    qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    }
}
static void _d2printf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((mopdl_debug & 2) != 0) {
	if ((mopdl_debug & 0x100) == 0)
	    pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
	else
	    qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    }
}
static void _d4printf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((mopdl_debug & 4) != 0) {
	if ((mopdl_debug & 0x100) == 0)
	    pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
	else
	    qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    }
}
static void _dqprintf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if (mopdl_debug)
	_dprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    else
	qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
}
#endif

#define DEBUG_MALLOC 0
#if DEBUG_MALLOC
extern int net_trace_malloc;		/* mallocs and frees */
#define malloc(size,sym) \
	dmalloc(size,"sym");
#define malloc_opt(size,opt,mod,rem,zone,sym) \
	dmalloc_opt(size,opt,mod,rem,zone,"sym")
#define free(ptr,sym) \
	dfree(ptr,"sym")
#else
#define malloc(size,sym) \
	dyn$_malloc(size,DYN$K_SYNC|DYN$K_NOOWN)
#define malloc_opt(size,opt,mod,rem,zone,sym) \
	dyn$_malloc(size,opt,mod,rem)
#define free(ptr,sym) \
	dyn$_free(ptr,DYN$K_SYNC)
#endif

/*External references*/

extern struct FILE *fopen(char *, char *);
extern void *ndl_allocate_msg(struct INODE *ip, int version, int inc_len);

/*Routine definitions*/

#if !MODULAR
int mopdl_close(struct FILE *fp);
int mopdl_open(struct FILE *fp, char *info, char *next, char *mode);
int mopdl_read(struct FILE *fp, int size, int number, unsigned char *c);
#endif
int mop_backoff_transact(struct FILE *fp, char *file_name);
int mop_dump_load(struct FILE *fp);
void mop_handle_dump_load(struct FILE *fp, unsigned char *dmpld,
  unsigned char *source, unsigned short size, int version);
int mop_handle_asst_vol(struct FILE *fp, struct MSG_RCVD *mi);
int mop_handle_mem_load(struct FILE *fp, struct MSG_RCVD *mi);
int mop_handle_mem_load_w_xfer(struct FILE *fp, struct MSG_RCVD *mi);
int mop_handle_param_load_w_xfer(struct FILE *fp, struct MSG_RCVD *mi);

void mop_send_req_program(struct FILE *fp, unsigned char *dest,
  unsigned char ver, unsigned char prog_type, char sw_id, char *fn);
void mop_send_req_mem_load(struct FILE *fp, unsigned char *dest,
  unsigned char ver, unsigned char seq);

/*+
 * ============================================================================
 * = mop_send_req_mem_load - Sends the request memory load message.           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will send the request memory load message.
 *
 * FORM OF CALL:
 *
 *	mop_send_req_mem_load(fp,dest,ver,seq);
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointer needed for access to the MOP block.
 *	char *dest - Pointer to 6 bytes containing the destination address.
 *		     If its a null address we use the DUMP/LOAD multicast
 *		     address
 *
 *	unsigned char ver - Specifies whether we want to use version 3 or 4.
 *					MOP$K_VERSION_3 = versions 1-3.
 *					MOP$K_VERSION_4 = version 4.
 *
 *	unsigned char seq - Sequence number.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_send_req_mem_load(struct FILE *fp, unsigned char *dest,
  unsigned char ver, unsigned char seq)
{
    struct MOP$REQ_MEM_LOAD *rp;	/*Pointer to the REQ_MEM_LOAD pkt*/

    dprintf("mop_send_req_mem_load\n", p_args0);

/*Allocate the message*/
    rp = ndl_allocate_msg(fp->ip, ver, MOP$K_INC_LENGTH);

/*Fill in the message*/
    rp->MOP$B_RQM_CODE = MOP$K_MSG_REQ_MEM_LOAD;
    rp->MOP$B_RQM_LOAD_NUM = seq;
    rp->MOP$B_RQM_RSVD = 0;

/*Send the message to the volunteer*/
    ndl_send(fp, ver, (char *) rp, sizeof(struct MOP$REQ_PROGRAM),
      MOP$K_INC_LENGTH, dest, MOP$K_PROT_DUMPLOAD);
}

/*+
 * ============================================================================
 * = mop_send_req_program - Sends the request program packet.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will send the request program message.
 *
 * FORM OF CALL:
 *          
 *	mop_send_req_program(fp,dest,ver,prog_type,sw_id,fn);
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointer needed for access to the MOP block.
 *	char *dest - Pointer to 6 bytes containing the destination address.
 *		     If its a null address we use the DUMP/LOAD multicast
 *		     address.
 *
 *	unsigned char ver - Specifies whether we want to use version 3 or 4.
 *					MOP$K_VERSION_3 = versions 1-3.
 *					MOP$K_VERSION_4 = version 4.
 *
 *	unsigned char prog_type - Specifies the program type. Types include:
 *					MOP$K_SECONDARY_LOADER 
 *					MOP$K_TERTIARY_LOADER 
 *					MOP$K_SYSTEM_IMAGE 
 *					MOP$K_MANAGEMENT_IMAGE
 *					MOP$K_CMIP_SCRIPT_FILE 
 *
 *	char sw_id - Specifies the software ID. If its > 0 then a filename is
 *			to be used
 *
 *	char *fn - Filename. To be used only if sw_id is > 0.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_send_req_program(struct FILE *fp, unsigned char *dest,
  unsigned char ver, unsigned char prog_type, char sw_id, char *fn)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct MB *mp;			/*Pointer to the MOP block*/
    char *p;				/*General pointer to the packet*/
    unsigned short size, is;		/*Size of the packets*/
    struct MOP$REQ_PROGRAM *rp;		/*Pointer to the start of the REQ_PROGRAM pkt*/
    int i, sum;

/*Get the pointer to the MOP block*/

    np = (struct NI_GBL *) fp->ip->misc;
    mp = (struct MB *) np->mbp;

/*Check for a null address*/

    sum = 0;
    for (i = 0; i < 6; i++)
	sum += (int) dest[i];

/*If it is a null address we'll use the multicast address*/

    if (!sum) {			/*The MOP DUMP/LOAD multicast*/
	i = 0;
	dest[i++] = 0xab;
	dest[i++] = 0x00;
	dest[i++] = 0x00;
	dest[i++] = 0x01;
	dest[i++] = 0x00;
	dest[i++] = 0x00;
    }

    dprintf("mop_send_req_program: ver = %x", p_args1(ver));
    dprintf(" SA: %02x-%02x-%02x-%02x-%02x-%02x\n",
	  p_args6(dest[0], dest[1], dest[2],
	  dest[3], dest[4], dest[5]));

/*Allocate the message*/

    rp = ndl_allocate_msg(fp->ip, ver, MOP$K_INC_LENGTH);

/*Fill in the fix message part of the message*/

    rp->MOP$B_RQP_CODE = MOP$K_MSG_REQ_PROGRAM;
    rp->MOP$B_RQP_DEVICE = mp->net_type;
    rp->MOP$B_RQP_FORMAT = ver;
    rp->MOP$B_RQP_PROGRAM = prog_type;
    rp->MOP$B_RQP_SW_ID_LEN = sw_id;

/*Get packet pointer and current size*/

    p = (char *) rp;
    size = sizeof(struct MOP$REQ_PROGRAM);

/*Fill in the software ID*/

    if (sw_id > 0) {
	strcpy(&(p[size]), fn);	/*Copy the Software ID name*/
	size += sw_id;		/*Current size*/
    }

/* Fill in the PROCESSOR field */

    p[size++] = MOP$K_SYSTEM_PROCESSOR;

/* Fill in the info fields */

    if (!mp->no_mop_info) {
	is = mop_build_info(fp, &(p[size]), ver);
	size += is;			/*Get the new size*/
    }

/*Send the message*/

    ndl_send(fp, ver, (char *) rp, size, MOP$K_INC_LENGTH, dest,
	MOP$K_PROT_DUMPLOAD);
}

/*+
 * ============================================================================
 * = mop_handle_param_load_w_xfer - Handles the param load with transfer msg. =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle the parameter load with transfer address
 *	message.
 *
 * FORM OF CALL:
 *
 *	mop_handle_param_load_w_xfer(fp,mi);
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointer needed for access to the MOP block.
 *	struct MSG_RCVD *mi - Pointer the message received info.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mop_handle_param_load_w_xfer(struct FILE *fp, struct MSG_RCVD *mi)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct MB *mp;			/*Pointer to the MOP block*/
    struct MOP$PARAM_LOAD_W_XFER *px;	/*param load with xfer address
					 * pointer*/
    unsigned char p_type, p_len;	/*Parameter type and length*/
    unsigned char *pf;			/*Pointer to the parameter field*/
    unsigned long xfr;			/*Transfer address*/
    int i, j;

    dprintf("mop_handle_param_load_w_xfer\n", p_args0);

#if DEBUG
    if (mopdl_debug & 4)
	pprint_pkt(mi->msg, mi->size);
#endif

/*Get the pointer to the MOP block*/
    np = (struct NI_GBL *) fp->ip->misc;
    mp = (struct MB *) np->mbp;

/*Get the correct typecast*/
    px = (struct MOP$PARAM_LOAD_W_XFER *) (mi->msg);

/*If the sequence number Doesn't match We have a failure*/

    dprintf("Param load w xfer: %d\n", p_args1(mp->dl.seq));

    if (px->MOP$B_PLX_LOAD_NUM != mp->dl.seq) {
#if DEBUG
	err_printf(msg_mop_wrn_out_o_seq_plxf);
	err_printf(msg_mop_det_sv_msg, mp->dl.seq, px->MOP$B_PLX_LOAD_NUM);
#endif
	mp->dl.load_status = msg_failure;
	return (msg_mop_wrn_out_o_seq_plxf);
    }

/*The memory load is in now complete*/
    mp->flags &= ~MOP$M_LIP;
    mp->dl.load_progress = MOP$K_MEM_LOAD_COMPLETE;

/*Save the parameter fields*/
    pf = (unsigned char *) ((int) px + sizeof(struct MOP$PARAM_LOAD_W_XFER));
    i = 0;
    do {
	p_type = pf[i++];
	if (p_type != MOP$K_PARM_END_MARK) {
	    p_len = pf[i++];
	    switch (p_type) {
		case MOP$K_PARM_NODE_NAME: 
		    for (j = 0; j < p_len; j++)
			mp->dl.tgt_sys_name[j] = pf[i++];

		    /*Terminate the string*/
		    mp->dl.tgt_sys_name[j] = 0;
		    break;
		case MOP$K_PARM_NODE_ADDR: 
		    if (p_len == 2) {

			/*Append  HIORD value*/
			j = 0;
			mp->dl.tgt_sys_addr[j++] = 0xaa;
			mp->dl.tgt_sys_addr[j++] = 0x00;
			mp->dl.tgt_sys_addr[j++] = 0x04;
			mp->dl.tgt_sys_addr[j++] = 0x00;

			/*Fill in specific value*/
			for (j = 0; j < p_len; j++)
			    mp->dl.tgt_sys_addr[j + 4] = pf[i++];
		    } else
			for (j = 0; j < p_len; j++)
			    mp->dl.tgt_sys_addr[j] = pf[i++];
		    break;
		case MOP$K_PARM_HOST_NAME: 
		    for (j = 0; j < p_len; j++)
			mp->dl.host_sys_name[j] = pf[i++];

		    /*Terminate the string*/
		    mp->dl.host_sys_name[j] = 0;
		    break;
		case MOP$K_PARM_HOST_ADDR: 
		    if (p_len == 2) {

			/*Append  HIORD value*/
			j = 0;
			mp->dl.host_sys_addr[j++] = 0xaa;
			mp->dl.host_sys_addr[j++] = 0x00;
			mp->dl.host_sys_addr[j++] = 0x04;
			mp->dl.host_sys_addr[j++] = 0x00;

			/*Fill in specific value*/
			for (j = 0; j < p_len; j++)
			    mp->dl.host_sys_addr[j + 4] = pf[i++];
		    } else
			for (j = 0; j < p_len; j++)
			    mp->dl.host_sys_addr[j] = pf[i++];
		    break;
		case MOP$K_PARM_V3_TOD: 
		    for (j = 0; j < p_len; j++)
			mp->dl.v3_tod[j] = pf[i++];
		    break;
		case MOP$K_PARM_V4_TOD: 
		    for (j = 0; j < p_len; j++)
			mp->dl.v4_tod[j] = pf[i++];
		    break;
		default: 
		    ni_error(mp->short_name, msg_success);
	    }
	}
    } while (p_type != MOP$K_PARM_END_MARK);

/*Get the transfer address*/
    memcpy(&xfr, &(pf[i]), 4);
    mp->dl.xfer_addr = (void *) xfr;

/*No image data*/
    mp->dl.actual_data_size = 0;

/*Tell caller we've got the message*/
    mp->dl.mlt.alt = TRUE;
    krn$_post(&(mp->dl.mlt.sem));

/*Return success*/
    return (msg_success);
}

/*+
 * ============================================================================
 * = mop_handle_mem_load_w_xfer - Handles the memory load with transfer msg.  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle the memory load with transfer message.
 *
 * FORM OF CALL:
 *
 *	mop_handle_mem_load_w_xfer(fp,mi);
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointer needed for access to the MOP block.
 *	struct MSG_RCVD *mi - Pointer the message received info.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mop_handle_mem_load_w_xfer(struct FILE *fp, struct MSG_RCVD *mi)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct MB *mp;			/*Pointer to the MOP block*/
    struct MOP$MEM_LOAD_W_XFER *mx;	/*Mem load with xfer address pointer*/
    char *bp;				/*Base pointer*/

    dprintf("mop_handle_mem_load_w_xfer\n", p_args0);

#if DEBUG
    if (mopdl_debug & 4)
	pprint_pkt(mi->msg, mi->size);
#endif

/*Get the pointer to the MOP block*/
    np = (struct NI_GBL *) fp->ip->misc;
    mp = (struct MB *) np->mbp;

/*Get the correct typecast*/
    mx = (struct MOP$MEM_LOAD_W_XFER *) (mi->msg);

/*If the sequence number doesn't match we have a failure*/

    dprintf("Mem load w xfer: %d\n", p_args1(mp->dl.seq));

    if (mx->MOP$B_MLX_LOAD_NUM != mp->dl.seq) {
#if DEBUG
	err_printf(msg_mop_wrn_out_o_seq_mlxf);
	err_printf(msg_mop_det_sv_msg, mp->dl.seq, mx->MOP$B_MLX_LOAD_NUM);
#endif
	return (msg_mop_wrn_out_o_seq_mlxf);
    }

/*The memory load is complete*/
    mp->dl.load_progress = MOP$K_MEM_LOAD_COMPLETE;

/*Get our base pointer*/
    bp = (char *) ((long) mi->msg + sizeof(struct MOP$MEM_LOAD));

/*Get the size*/
    mp->dl.actual_data_size = mi->size - sizeof(struct MOP$MEM_LOAD) - 4;

/*Copy the data using a supplied pointer specified from read*/
    spinlock(&(mp->dllock));
    if (mp->dl.data_ptr)
	if (mp->dl.actual_data_size > mp->dl.req_data_size) {
	    spinunlock(&(mp->dllock));
	    ni_error(mp->short_name, msg_bad_request, fp->ip->name);
	    mp->dl.actual_data_size = 0;
	    mp->dl.xfer_addr = 0;
	    return (msg_failure);
	} else {

	    /*Copy the data and transfer address*/
	    memcpy(mp->dl.data_ptr, bp, mp->dl.actual_data_size);
	    spinunlock(&(mp->dllock));
	    mp->dl.xfer_addr = *(long *) ((int) bp + mp->dl.actual_data_size);
	}
    else
	spinunlock(&(mp->dllock));

/*Tell caller we've got the message*/
    mp->dl.mlt.alt = TRUE;
    krn$_post(&(mp->dl.mlt.sem));

/*Return success*/
    return (msg_success);
}

/*+
 * ============================================================================
 * = mop_handle_mem_load - Handles the memory load messages.                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle the memory load messages.
 *
 * FORM OF CALL:
 *
 *	mop_handle_mem_load(fp,mi);
 *                     
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointer needed for access to the MOP block.
 *	struct MSG_RCVD *mi - Pointer the message received info.
 *
 * SIDE EFFECTS:
 *                     
 *      None
 *                                                       
-*/

int mop_handle_mem_load(struct FILE *fp, struct MSG_RCVD *mi)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct MB *mp;			/*Pointer to the MOP block*/
    struct MOP$MEM_LOAD *ml;		/*Mem load packet pointer*/
    char *bp;				/*Base pointer*/
    int i;

    dprintf("mop_handle_mem_load\n", p_args0);

#if DEBUG
    if (mopdl_debug & 4)
	pprint_pkt(mi->msg, mi->size);
#endif

/*Get the pointer to the MOP block*/
    np = (struct NI_GBL *) fp->ip->misc;
    mp = (struct MB *) np->mbp;

/*Get the correct typecast*/
    ml = (struct MOP$MEM_LOAD *) (mi->msg);

/*If the sequence number Doesn't match We have a failure*/

    dprintf("mem load: %d\n", p_args1(mp->dl.seq));

    if (ml->MOP$B_MLD_LOAD_NUM != mp->dl.seq) {
#if DEBUG
#if !TURBO
	err_printf(msg_mop_wrn_out_o_seq_ml);
	err_printf(msg_mop_det_sv_msg, mp->dl.seq, ml->MOP$B_MLD_LOAD_NUM);
#endif
#if TURBO
	err_printf("+");
#endif
#endif
	return (msg_mop_wrn_out_o_seq_ml);
    }

/*The memory load is in now started or continuing*/
    if (mp->dl.load_progress == MOP$K_MEM_LOAD_STARTED)
	mp->dl.load_progress = MOP$K_MEM_LOAD_CONTINUING;
    else
	mp->dl.load_progress = MOP$K_MEM_LOAD_STARTED;

/*Get our base pointer*/
    bp = (char *) ((long) mi->msg + sizeof(struct MOP$MEM_LOAD));

/*Get the size*/
    mp->dl.actual_data_size = mi->size - sizeof(struct MOP$MEM_LOAD);

/*Copy the data using a supplied pointer specified from read*/
    spinlock(&(mp->dllock));
    if (mp->dl.data_ptr)
	if (mp->dl.actual_data_size > mp->dl.req_data_size) {
	    spinunlock(&(mp->dllock));
	    ni_error(mp->short_name, msg_bad_request, fp->ip->name);
	    mp->dl.actual_data_size = 0;
	    mp->dl.xfer_addr = 0;
	    return (msg_failure);
	} else {

	    /*Copy the data and transfer address*/
	    memcpy(mp->dl.data_ptr, bp, mp->dl.actual_data_size);
	    spinunlock(&(mp->dllock));
	    mp->dl.xfer_addr = *(long *) ((int) bp + mp->dl.actual_data_size);
	}
    else
	spinunlock(&(mp->dllock));

/*Tell caller we've got the message*/
    mp->dl.mlt.alt = TRUE;
    krn$_post(&(mp->dl.mlt.sem));

/*Return success*/
    return (msg_success);
}

/*+
 * ============================================================================
 * = mop_handle_asst_vol - Handles the assistance volunteer message.          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle the assistance volunteer message.
 *
 * FORM OF CALL:
 *
 *	mop_handle_asst_vol(fp,mi);
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointer needed for access to the MOP block.
 *	struct MSG_RCVD *mi - Pointer the message received info.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mop_handle_asst_vol(struct FILE *fp, struct MSG_RCVD *mi)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct MB *mp;			/*Pointer to the MOP block*/

    dprintf("mop_handle_asst_vol\n", p_args0);

/*Get the pointer to the MOP block*/

    np = (struct NI_GBL *) fp->ip->misc;
    mp = (struct MB *) np->mbp;

/*If a load is already in progress just discard this packet*/

    if (mp->flags & MOP$M_LIP) {
	dprintf("  Discard SA: %02x-%02x-%02x-%02x-%02x-%02x\n",
	  p_args6(mi->source[0], mi->source[1], mi->source[2],
	  mi->source[3], mi->source[4], mi->source[5]));
	return (msg_success);		/*Return a success*/
    }

/*The load is in progress now*/

    mp->flags |= MOP$M_LIP;
    mp->dl.load_progress = MOP$K_FOUND_VOLUNTEER;
    mp->dl.version = mi->version;

/*Get the volunteer*/

    memcpy(mp->dl.va, mi->source, 6);

    dprintf("  Volunteer SA: %02x-%02x-%02x-%02x-%02x-%02x\n",
	  p_args6(mi->source[0], mi->source[1], mi->source[2],
	  mi->source[3], mi->source[4], mi->source[5]));

/*Tell must transact we've got a volunteer*/

    mp->dl.avt.alt = TRUE;
    krn$_post(&(mp->dl.avt.sem));

    return (msg_success);		/*Return success*/
}

/*+
 * ============================================================================
 * = mop_handle_dump_load - Handle MOP dump/load messages.                    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle MOP dump/load messages. This routine excepts
 *	a pointer to the dump/load portion of the message (Above the protocol).
 *	This is indpendent of V3 or V4 packets.
 *
 * FORM OF CALL:
 *
 *	mop_handle_dump_load(fp,dmpld,source,size,version)
 *
 * RETURNS:
 *                   
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file opened. Returns 0 on an error.
 *
 *	char *dmpld - Pointer to the dump/load portion of the MOP message.
 *
 *	unsigned char *source - Pointer to the source address of the message.
 *
 *	unsigned short size - Size of the loopback message.
 *
 *	int version - Specifies whether we want to use version 3 or 4.
 *		      			MOP$K_VERSION_3 = versions 1-3.
 *		      			MOP$K_VERSION_4 = version 4.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_handle_dump_load(struct FILE *fp, unsigned char *dmpld,
  unsigned char *source, unsigned short size, int version)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct MB *mp;			/*Pointer to the MOP block*/
    struct MSG_RCVD *mi;		/*Message info*/
    int inc_len;			/*Counter,Include length flag*/
    char name[20];			/*Process name,device name*/
    int first;				/*First entry on queue*/

    dprintf("mop_handle_dump_load\n", p_args0);

/*Get the pointer to the MOP block*/
    np = (struct NI_GBL *) fp->ip->misc;
    mp = (struct MB *) np->mbp;

/*Take care of the size and the pointer*/
    if ((version == MOP$K_VERSION_3))
	{
	    /*First the size*/
	    size = *((unsigned short *) dmpld);

	    /*Then the pointer*/
	    dmpld = (unsigned char *) ((long) dmpld + 2);

	    /*Include length flag*/
	    inc_len = MOP$K_INC_LENGTH;
	}
    else

      /*Don't include length flag*/
	inc_len = MOP$K_DONT_INC_LENGTH;

#if DEBUG
    if (mopdl_debug & 4)
	pprint_pkt(dmpld, size);
#endif

/*Get an information pakcet*/
    mi = (struct MSG_RCVD *) malloc(sizeof(struct MSG_RCVD), mi);

/*Fill it in*/
    mi->msg = dmpld;
    mi->size = size;
    memcpy(mi->source, source, 6);
    mi->version = version;
    mi->inc_len = inc_len;

/*Insert the packet on the dump/load list*/
    krn$_wait(&(mp->dl.lock));
    first = (mp->dl.qh.flink == &mp->dl.qh);
    insq(mi, mp->dl.qh.blink);
    krn$_post(&(mp->dl.lock));

/*Wakeup the dump/load process*/
    if (first) {
	sprintf(name, "mopdl_%4.4s", fp->ip->name);
	krn$_create(			/* */
	  mop_dump_load, 		/* address of process */
	  0, 				/* startup routine */
	  0, 				/* completion semaphore */
	  3, 				/* process priority */
	  -1, 				/* cpu affinity mask */
	  1024 * 4, 			/* stack size */
	  name, 			/* process name */
	  "tt", "r", "tt", "w", "tt", "w", fp);
    }
}

/*+
 * ============================================================================
 * = mop_dump_load - MOP dump/load process.                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is actually a process started when the driver was opened.
 *	This process is woken up when a dump/load message is received. When
 *	woken up the process removes the message from the dump/load queue
 *	and handles it accordingly.
 *
 * FORM OF CALL:
 *
 *	mop_dump_load(fp)
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointer needed for access to the MOP block.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_dump_load(struct FILE *fp)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct MB *mp;			/*Pointer to the MOP block*/
    struct MSG_RCVD *mi;		/*Message info*/
    struct INODE *ip;			/*Pointer to the inode being used*/
    int last;				/*Last entry on queue*/

    dprintf("mop_dump_load\n", p_args0);

/*Get the pointer to the MOP block*/

    np = (struct NI_GBL *) fp->ip->misc;
    mp = (struct MB *) np->mbp;

    while (1) {

	/*Get the message*/
	mi = mp->dl.qh.flink;

	/*Handle the correct message*/
	/*The code is the first byte of the message*/
	if (!mp->dl.shutdown) {
	    switch (*(mi->msg)) {
		case MOP$K_MSG_MEM_LOAD_W_XFER: 
		    mop_handle_mem_load_w_xfer(fp, mi);
		    break;
		case MOP$K_MSG_MEM_LOAD: 
		    mop_handle_mem_load(fp, mi);
		    break;
		case MOP$K_MSG_PARAM_LOAD_W_XFER: 
		    mop_handle_param_load_w_xfer(fp, mi);
		    break;
		case MOP$K_MSG_VOLUNTEER: 
		    mop_handle_asst_vol(fp, mi);
		    break;
	    }
	}

	/*Deallocate the message*/
	ndl_deallocate_msg(fp->ip, mi->msg, mi->version, mi->inc_len);

	/*Dequeue the information packet*/
	krn$_wait(&(mp->dl.lock));
	remq(mi);
	last = (mp->dl.qh.flink == &mp->dl.qh);
	krn$_post(&(mp->dl.lock));

	/*Free the information packet*/
	free(mi, mi);

	/*Quit if we've handled them all*/
	if (last)
	    break;
    }
}

/*+
 * ============================================================================
 * = mop_backoff_transact - Looks for a boot volunteer.                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will search for a boot volunteer. It uses the
 *	Mop backoff transact algorithm. Repeated requests cycle between
 *	MOP V4 format and V3 format. Each cycle the retry wait period
 *	is backed-off linearly up to a maximum retry wait.
 *
 * FORM OF CALL:
 *
 *	mop_backoff_transact(fp,file_name)
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointer needed for access to the MOP block.
 *	char *file_name - Name of the file to be loaded.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mop_backoff_transact(struct FILE *fp, char *file_name)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct MB *mbp;			/*Pointer to the MOP block*/
    int backoff, retransmit_time, timeout;
					/*Timeout values*/
    int index, r, jitter, m;		/*Used to generate random numbers*/
    unsigned char dest[10];		/*Destination address*/
    unsigned short seed;		/*Random number generator seed*/
    int i, status, retry, fast_boot;
    struct EVNODE evn, *evp;		/*Pointers to the evnode*/

    dprintf("mop_backoff_transact\n", p_args0);

/*Set the fast boot flag if the environment variable exists*/
    evp = &evn;
    if (ev_read("mopv3_boot", &evp, 0) == msg_success)
	fast_boot = 1;
    else
	fast_boot = 0;

/*Get the pointer to the MOP block*/
    np = (struct NI_GBL *) fp->ip->misc;
    mbp = (struct MB *) np->mbp;

/*Make a null destination this makes the routine*/
/*use the multicast destination*/
    for (i = 0; i < 6; i++)
	dest[i] = 0;

/*Get some initial values*/
    backoff = MOP$K_INITIAL_BACKOFF;
    retransmit_time = MOP$K_MIN_RETRANSMIT_TIME;

/*Index into the LCG table*/
    index = MOP$K_LCG_INDEX;

/*Maximum generated range*/
    m = 1 << index;

/*Random number seed*/
    seed = *(unsigned short *) &(mbp->sa[4]);
    r = seed & (m - 1);

/*Try to get a volunteer*/
    for (retry = 0; retry < 1; retry++) {

	/*First send a burst version 4 packets*/
	for (i = 0; i < MOP$K_BURST_SIZE; i++) {

	    /*Show some progress*/
	    if (!(fp->mode & MODE_M_SILENT))
		print_progress();

	    /*Don't send the v4 messages if fast boot is set*/
	    if (!fast_boot) {

		/*Send and wait for the response*/
		mop_set_timer(&(mbp->dl.avt));
		mop_send_req_program(fp, dest, MOP$K_VERSION_4,
		  MOP$K_SYSTEM_IMAGE, strlen(file_name), file_name);

		/*Wait for the response*/
		status = mop_wait_msg(&(mbp->dl.avt), retransmit_time);

		/*Check first to see if we are killed*/
		if (status == MOP$K_KILLED)
		    return (msg_failure);
		if (status == MOP$K_SUCCESS)
		    return (msg_success);
	    }
	}

	/*Then a burst version 3 packets*/
	for (i = 0; i < MOP$K_BURST_SIZE; i++) {

	    /*Show some progress*/
	    if (!(fp->mode & MODE_M_SILENT))
		print_progress();

	    /*If its the last try backoff a bit*/
	    if (i == (MOP$K_BURST_SIZE - 1))
		timeout = backoff;
	    else
		timeout = retransmit_time;

	    /*Add some jitter*/
	    r = random(r, index);
	    jitter =
	      (r * ((timeout * 1000) / ((m - 1) * 2))) / 1000 - (timeout / 4);
	    timeout += jitter;

	    /*Send and wait for the response*/
	    mop_set_timer(&(mbp->dl.avt));
	    mop_send_req_program(fp, dest, MOP$K_VERSION_3, MOP$K_SYSTEM_IMAGE,
	      strlen(file_name), file_name);

	    /*Wait for the response*/
	    status = mop_wait_msg(&(mbp->dl.avt), timeout);

	    /*Check first to see if we are killed*/
	    if (status == MOP$K_KILLED)
		return (msg_failure);
	    if (status == MOP$K_SUCCESS)
		return (msg_success);
	}

	/*Double the backoff up to the MAX*/
	if (backoff < MOP$K_MAX_BACKOFF)
	    backoff *= 2;
	else
	    backoff = MOP$K_MAX_BACKOFF;
    }
    return (msg_failure);
}

#if !MODULAR
/*+
 * ============================================================================
 * = mopdl_read - Read the MOP dump/load file.                               =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      Read the MOP dump/load file. This is actually a mop load.
 *  
 * FORM OF CALL:
 *  
 *	mopdl_read (fp,size,number,c);
 *  
 * RETURNS:
 *
 *	number of read objects
 *      0 - If EOF or error (fp->status = failure message)
 *                          (fp->code_entry = boot start address)
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to a file.
 *	int size - Not used for MOP LOAD.
 *	int number - Not used for MOP LOAD.
 *	unsigned char *c - For MOP load the base of where this is to be loaded.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int mopdl_read(struct FILE *fp, int size, int number, unsigned char *c)
{
    struct FILE *nfp;			/*Pointer to ther next level of
					 * protocol*/
    struct DL_FILE_BLOCK *dl;		/*Pointer to the dump load file block*/
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct MB *mp;			/*Pointer to the MOP block*/
    int i, ds;				/*Counter and data size*/
    int status;

    dprintf("mopdl_read\n", p_args0);

/*Get the pointer to the mopdl block*/
    dl = (struct DL_FILE_BLOCK *) fp->misc;
/*Get the pointer to the MOP block*/
    np = (struct NI_GBL *) dl->fp->ip->misc;
    mp = (struct MB *) np->mbp;
/*Get the pointer to the next level of protocol*/
    nfp = (struct FILE *) dl->fp;

    dprintf("mopdl_read rq_len=%d bf_addr=%x ld_pr=%d\n",
	p_args3(size * number, c, mp->dl.load_progress));

/*Initialize the size and data pointers*/
    mp->dl.req_data_size = size * number;
    spinlock(&(mp->dllock));
    mp->dl.data_ptr = c;
    spinunlock(&(mp->dllock));

/*For the specified number of retries look for the response*/
    for (i = 0;(i < MOP$K_RETRY_MAX) &&
      (mp->dl.load_progress != MOP$K_MEM_LOAD_COMPLETE); i++) {

	/*Set the timer*/
	mop_set_timer(&(mp->dl.mlt));

	/*If this is the first mopdl_read call after mopdl_open*/
	/*has been invoked, send the 2nd req_program*/
	if (mp->dl.load_progress == MOP$K_FOUND_VOLUNTEER) {

	    dprintf("load_progress = MOP$K_FOUND_VOLUNTEER\n", p_args0);

	    /*Save the load start address*/
	    mp->dl.load_start_addr = c;

	    /*Send request program*/
	    mop_send_req_program(nfp, mp->dl.va, mp->dl.version,
	      MOP$K_SYSTEM_IMAGE, strlen(dl->load_file), dl->load_file);

	} else {

	  /*Send the Ack's*/

	    dprintf("load_progress = %d\n", p_args1(mp->dl.load_progress));

	    mop_send_req_mem_load(nfp, mp->dl.va, mp->dl.version, mp->dl.seq);
	}

	/*Wait for a response*/
	status = mop_wait_msg(&(mp->dl.mlt), MOP$K_MIN_RETRANSMIT_TIME);

	/*Check for some status*/
	if (status == msg_success)
	    break;
	if (status == MOP$K_KILLED) {
	    spinlock(&(mp->dllock));
	    mp->dl.data_ptr = 0;
	    spinunlock(&(mp->dllock));
	    fp->status = msg_ctrlc;
	    return 0;
	}
    }

/*If we're out of retries return a failure*/
    if (i == MOP$K_RETRY_MAX) {

	dprintf("retry failure: return 0\n", p_args0);

	spinlock(&(mp->dllock));
	mp->dl.data_ptr = 0;
	spinunlock(&(mp->dllock));
	fp->status = msg_failure;
	return 0;
    }

/*Get the size of the data*/
    ds = mp->dl.actual_data_size;

/* If the transfer is completed send the last acknowledge  */
    if (mp->dl.load_progress == MOP$K_MEM_LOAD_COMPLETE) {

	dprintf("last ack...\n", p_args0);

	/*Save the address for the boot*/
	fp->code_entry = (long int) mp->dl.xfer_addr;

	/* Send the ACK */
	mp->dl.seq++;
	mp->dl.seq %= 256;
	mop_send_req_mem_load(nfp, mp->dl.va, mp->dl.version, mp->dl.seq);
    } else {

	/*Bump the sequence number*/
	mp->dl.seq++;
	mp->dl.seq %= 256;
    }

/*Return the size*/
    spinlock(&(mp->dllock));
    mp->dl.data_ptr = 0;
    spinunlock(&(mp->dllock));

    dprintf("return len = %d\n", p_args1(ds));

    *fp->offset += ds;
    return ds;
}

/*+
 * ============================================================================
 * = mopdl_open - Open the MOP dumop/load file.                                 =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      Open the MOP dump/load file.
 *  
 * FORM OF CALL:
 *  
 *      mopdl_open (fp,info,next,mode);
 *  
 * RETURNS:
 *
 *      msg_success - Success
 *      msg_failure - Failure
 *       
 * ARGUMENTS:
 *
 *      struct FILE *fp - Pointer to the file that is opened.
 *      char *info - Name of a file to be used.
 *      char *next - The next part of the open string.
 *      char *mode - The mode of the file being opened..
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mopdl_open(struct FILE *fp, char *info, char *next, char *mode)
{
    struct DL_FILE_BLOCK *dl;		/*Pointer to the dump load file block*/
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct MB *mp;			/*Pointer to the MOP block*/
    int i;
    int status;
    unsigned char c;

    dprintf("mopdl_open\n", p_args0);

/*Get some memory for the file block*/
    dl = (struct DL_FILE_BLOCK *) malloc(sizeof(struct DL_FILE_BLOCK), dl);

/*Open the next protocol*/
    dl->fp = fopen(next, mode);
    if (!dl->fp) {

	/*We couldn't open the file*/
	free(dl, dl);
	return (msg_failure);
    }

/*Fill the file name, converting to uppercase*/
    for (i = 0; i < sizeof(dl->load_file) - 1; i++) {
	c = info[i];
	if (!c)
	    break;
	dl->load_file[i] = c;
    }
    dl->load_file[i] = 0;

/*Fill in the pointer field*/
    fp->misc = (long int) dl;

/*Take care of some counters*/
    init_file_offset(fp);
    fp->rap = 0;
    fp->status = msg_success;

/*Get the pointer to the MOP block*/
    np = (struct NI_GBL *) dl->fp->ip->misc;
    mp = (struct MB *) np->mbp;

/*Take care of flags*/
    mp->flags &= ~MOP$M_LIP;
    mp->dl.load_progress = MOP$K_LOAD_STARTED;
    mp->dl.load_status = msg_success;
    mp->dl.host_sys_name[0] = 0;

/*Look for a volunteer*/
    status = mop_backoff_transact(dl->fp, dl->load_file);

    if (status == msg_failure) {
	fclose(dl->fp);
	free(dl, dl);
    }

/*Init the sequence number*/
    mp->dl.seq = 0;
    mp->dl.image_size = 0;

/*Return status*/
    return status;
}

/*+
 * ============================================================================
 * = mopdl_close - Close the MOP dump/load file.                              =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      Close the MOP dump/load file.
 *  
 * FORM OF CALL:
 *  
 *      mopdl_close (fp);
 *  
 * RETURNS:
 *
 *      msg_success - Success
 *      msg_failure - Failure
 *       
 * ARGUMENTS:
 *
 *      struct FILE *fp - Pointer to the file that is to be closed.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mopdl_close(struct FILE *fp)
{
    struct DL_FILE_BLOCK *dl;		/*Pointer to the dump load file block*/
    struct NI_GBL *np;
    struct MB *mp;			/*Pointer to the MOP block*/

    dprintf("mopdl_close\n", p_args0);

/*Get the pointer to the mopdl block*/
    dl = (struct DL_FILE_BLOCK *) fp->misc;
/*Get the pointer to the MOP block*/
    np = (struct NI_GBL *) dl->fp->ip->misc;
    mp = (struct MB *) np->mbp;

/*Fix output*/
    err_printf("\n");

/*Display some info*/
    if (fp->status == msg_success) {
	err_printf(msg_mop_inf_ld_com);
	if (mp->dl.host_sys_name[0]) {
	    err_printf(msg_mop_inf_hst_name, mp->dl.host_sys_name);
	    err_printf(msg_mop_inf_hst_addr, mp->dl.host_sys_addr[0],
	      mp->dl.host_sys_addr[1], mp->dl.host_sys_addr[2],
	      mp->dl.host_sys_addr[3], mp->dl.host_sys_addr[4],
	      mp->dl.host_sys_addr[5]);
	}
    } else
	err_printf(msg_net_003);

/*Close the next protocol and free the memory*/
    fclose(dl->fp);
    free(dl, dl);

/*Return a success*/
    return (msg_success);
}

#endif

#if MODULAR
int mopdl_ovly_init(void) 
{
    mop_init();

    return msg_success;
}

#endif
