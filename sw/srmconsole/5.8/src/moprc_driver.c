/* file:	moprc_driver.c
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
 *  ABSTRACT:	MOP Remote Console driver.
 *
 *	This module implements the network remote comsole functions.
 *
 *  AUTHORS:                                     
 *
 *	John DeNisco                                                     
 *
 *  CREATION DATE:
 *  
 *      08-APR-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	phk	19-Nov-1991	Pass the password to 
 *				mop_send_rem_boot
 *
 *	jad	08-Aug-1991	Allow kill for unsolicited ID
 *
 *	ajb	10-May-1991	Convert to Bourne shell
 *
 *	jad	08-Apr-1991	Initial entry.
 *
 *--
 */
                                                
/*Include files*/                                      

#include "cp$src:platform.h"
#include "cp$src:common.h"			/*Common definitions*/
#include "cp$src:kernel_def.h"			/*Kernel definitions*/
#include "cp$src:inet.h"
#include "cp$src:prdef.h"			/* Processor definitions*/
#include "cp$src:dynamic_def.h"			/*Memory allocation defs*/
#include "cp$src:msg_def.h"			/*Message definitions*/
#include "cp$src:ni_env.h"			/*Environment variables*/
#include "cp$src:mop_def.h"			/*MOP definitions*/
#include "cp$src:mb_def.h"			/*MOP block defs*/
#include "cp$src:mop_counters.h"		/*MOP counters*/
#include "cp$src:ev_def.h"			/*Environment vars*/
#include "cp$inc:prototypes.h"

/*Some debug constants*/
#define		MOPRC_DEBUG 0

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

/*Local constants*/                                            
#define RC_PASSWORD char password[8] = {0,0,0,0,0,0,0,0};

/*External references*/
extern int spl_kernel;			/* Kernel spinlock	*/	

extern struct FILE* fopen(char*,char*);
extern void* ndl_allocate_msg(struct INODE *ip,int version,int inc_len);
extern int null_procedure ();
extern int sh ();

/*Routine definitions*/
char moprc_in (struct MB *mp);                         
moprc_out (struct MB *mp,char c);
int moprc_read (struct FILE *fp,int size,int number,char *buf);
void moprc_rx (struct MB *mp,struct MSG_RCVD *mi,
						struct MOP$CON_RSP_N_ACK *cra,
						struct MOP$CON_CMD_N_POLL *ccp);
void moprc_tx (struct MB *mp,struct MOP$CON_RSP_N_ACK *cra);
int moprc_write (struct FILE *fp, int size,int number,char *buf);
int mop_handle_con_cmd_n_poll(struct FILE *fp,struct MSG_RCVD *mi);
int mop_handle_res_console(struct FILE *fp,struct MSG_RCVD *mi);
int mop_handle_rel_console(struct FILE *fp,struct MSG_RCVD *mi);
int mop_handle_sys_id(struct FILE *fp,struct MSG_RCVD *mi);
int mop_rc_watchdog(char *port_file);
int mop_release_console(struct FILE *fp);
void mop_send_rem_boot(struct FILE *fp,unsigned char *dest, unsigned *pwd);
void mop_send_req_counters(struct FILE *fp,unsigned char *dest,
						unsigned char ver);
void mop_send_req_id(struct FILE *fp,unsigned char *dest,unsigned char ver);
#if MONET
extern int tt_reprompt;
int mop_handle_trig_boot(struct FILE *fp,struct MSG_RCVD *mi);
extern void boot_system( char *boottype ); /* needed to perform trigger boot */
int moptrig_debug = 0;
#endif

/*+
 * ============================================================================
 * = mop_send_req_id - Sends the request ID packet.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will send the req ID message.
 *
 * FORM OF CALL:
 *
 *	mop_send_req_id(fp,dest,ver);
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointer needed for access to the MOP block.
 *	char *dest - Pointer to 6 bytes containing the destination address.
 *	unsigned char ver - Specifies whether we want to use version 3 or 4.
 *					MOP$K_VERSION_3 = versions 1-3.
 *					MOP$K_VERSION_4 = version 4.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_send_req_id(struct FILE *fp,unsigned char *dest,
						unsigned char ver)
{
struct MOP$REQUEST_ID *ri;	/*Pointer to the start of the REQ_ID pkt*/
int i,sum;

	trace("mop_send_req_id\n",0);

/*Allocate the message*/
	ri = ndl_allocate_msg(fp->ip,ver,MOP$K_INC_LENGTH);

/*Check for a null address*/
	sum = 0;
	for(i=0; i<6; i++)
		sum += (int)dest[i];

/*If it is a null address we'll use the multicast address*/
	if(!sum)
	{
	/*The console multicast*/
	    dest[0] = 0xab; dest[1] = 0x00; dest[2] = 0x00;
	    dest[3] = 0x02; dest[4] = 0x00; dest[5] = 0x00;
	}

/*Fill in the message*/
	ri->MOP$B_RID_CODE = MOP$K_MSG_REQUEST_ID;	
	ri->MOP$B_RID_RESERVED = 0;
	ri->MOP$W_RID_RECEIPT_NUM = 1;

/*Send the message*/
	ndl_send(fp,ver,(char*)ri,sizeof(struct MOP$REQUEST_ID),
							MOP$K_INC_LENGTH,
							dest,
							MOP$K_PROT_CONSOLE);
}


/*+
 * ============================================================================
 * = mop_send_req_counters - Sends the request counters packet.               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will send the req counters message.
 *
 * FORM OF CALL:
 *
 *	mop_send_req_counters(fp,dest,ver);
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointer needed for access to the MOP block.
 *	char *dest - Pointer to 6 bytes containing the destination address.
 *	unsigned char ver - Specifies whether we want to use version 3 or 4.
 *					MOP$K_VERSION_3 = versions 1-3.
 *					MOP$K_VERSION_4 = version 4.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_send_req_counters(struct FILE *fp,unsigned char *dest,
						unsigned char ver)
{
struct MOP$REQ_COUNTERS *rp;	/*Pointer to the start of the REQ_COUNTERS pkt*/

	trace("mop_send_req_counters\n",0);

/*Allocate the message*/
	rp = ndl_allocate_msg(fp->ip,ver,MOP$K_INC_LENGTH);

/*Fill in the message*/
	rp->MOP$B_RCT_CODE = MOP$K_MSG_REQ_COUNTERS;	
	rp->MOP$W_RCT_RECEIPT_NUM = 0;

/*Send the message*/
	ndl_send(fp,ver,(char*)rp,sizeof(struct MOP$REQ_COUNTERS),
							MOP$K_INC_LENGTH,
							dest,
							MOP$K_PROT_CONSOLE);
}


/*+
 * ============================================================================
 * = mop_release_console - Release the remote console message.                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will perform the functions needed to release
 *	remote console.
 *
 * FORM OF CALL:
 *
 *	mop_release_console(fp);
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

int mop_release_console(struct FILE *fp)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
int i;

	trace("mop_release_console\n",0);

/*Get the pointer to the MOP counters*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Reset the timer*/
	mp->rc.timer = MOP$K_RC_CONNECTION_TIMEOUT;

/*reset the console reserved flag*/
	mp->flags &= ~MOP$M_RC_RESERVED;	

/*reset the user address*/
	for(i=0; i<6; i++)	
	    mp->rc.console_user[i] = 0;

/*Set the message number*/
	mp->rc.message_number = 0xff;

/*Return success*/
	return(msg_success);
}


/*+
 * ============================================================================
 * = mop_rc_watchdog - Checks for remote console timeout.                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will check the remote console timeout
 *	and will insure there is still a connection.
 *
 * FORM OF CALL:
 *
 *	 mop_rc_watchdog (port_file); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	char *port_file - Name of the port that is to be used.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
    
int mop_rc_watchdog (char *port_file)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
struct FILE *pf;			/*Port file pointer*/

	trace("mop_rc_watchdog\n",0);

/*Open the files*/
	pf = fopen(port_file,"r+");
	if(!pf)
	    return(msg_failure);

/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)pf->ip->misc;
	mp = (struct MB*)np->mbp;

/*Send the loop packets forever*/
	while(1)
	{
	/*Wait for a before we check the list*/
	    krn$_sleep(MOP$K_RC_WATCHDOG_POLL);

	/*Stop this process if we're are supposed to*/
	    if(killpending())
		return(msg_success);

	/*Decrement the timer*/
	    mp->rc.timer -= MOP$K_RC_WATCHDOG_POLL;

	/*If its less than our equal to zero its a timeout*/
	    if((int)mp->rc.timer <= 0)
	    {
	    /*Release the console*/
		mop_release_console(pf);
		return(msg_success);
	    }
	}
}

/*+
 * ============================================================================
 * = mop_handle_rel_console - Handles the release console message.            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle the release console message.
 *
 * FORM OF CALL:
 *
 *	mop_handle_rel_console(fp,mi);
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

int mop_handle_rel_console(struct FILE *fp,struct MSG_RCVD *mi)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
struct PCB *pcb;			/*Timer process pcb*/
int i;

	trace("mop_handle_rel_console\n",0);

#if MOPRC_DEBUG
	print_pkt(mi->msg,mi->size);
#endif

/*Get the pointer to the MOP counters*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Make sure the console is reserved*/
	if((mp->flags & MOP$M_RC_RESERVED) != MOP$M_RC_RESERVED)
	{
		ni_error(mp->short_name,msg_moprc_err_rc_n_res);
		return(msg_failure);
	}

/*Kill the timer process*/
	pcb = (struct PCB*)krn$_findpcb((mp->rc.timer_pid));
	if(pcb)
	{
	    pcb->pcb$l_killpending = 1;
	    spinunlock( &spl_kernel );
	}
	else
	    ni_error(mp->short_name,msg_mop_err_cd_kl_rc_t);

/*Kill the shell process*/
	pcb = (struct PCB*)krn$_findpcb((mp->rc.shell_pid));
	if(pcb)
	{
	    pcb->pcb$l_killpending = 1;
	    spinunlock( &spl_kernel );
	}
	else
	    ni_error(mp->short_name,msg_moprc_err_cd_kl_sh);

/*Enque a control Z so the shell will be killed*/
	mp->rc.rbuf[(mp->rc.rx_index_in)] = 26;
	(mp->rc.rx_index_in)++;
	mp->rc.rx_index_in %= MOP$K_CH_IN_MAX;
	krn$_bpost(&(mp->rc.rx_avail));

/*Release the console*/
	mop_release_console(fp);

/*Display a short message*/
	err_printf(msg_moprc_inf_rc_rel,mi->source[0],mi->source[1],
					mi->source[2],mi->source[3],
					mi->source[4],mi->source[5]);

/*Return success*/
	return(msg_success);
}


/*+
 * ============================================================================
 * = mop_handle_res_console - Handles the reserve console message.            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle the reserve console message.
 *
 * FORM OF CALL:
 *
 *	mop_handle_res_console(fp,mi);
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

int mop_handle_res_console(struct FILE *fp,struct MSG_RCVD *mi)
{
RC_PASSWORD
struct PCB *pcb;			/*Timer process pcb*/
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
struct MOP$RESERVE_CONSOLE *rsv;	/*Reserve console message pointer*/
char name[128];				/*Process name*/
int i;
char **shv;
extern int shell_startup ();

	trace("mop_handle_res_console\n",0);

#if MOPRC_DEBUG
	print_pkt(mi->msg,mi->size);
#endif

/*Get the pointer to the MOP counters*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Get the correct typecast fro the received message*/
	rsv = (struct MOP$RESERVE_CONSOLE*)(mi->msg);

/*Make sure the console is not already reserved*/
	if((mp->flags & MOP$M_RC_RESERVED) == MOP$M_RC_RESERVED)
	{
		ni_error(mp->short_name,msg_moprc_err_rc_already_res);
		return(msg_failure);
	}

/*Check verification*/
	if(memcmp(password,rsv->MOP$B_RSV_VERICATION,8))
	{
		ni_error(mp->short_name,msg_moprc_err_rc_b_ver);
		return(msg_failure);
	}

/*Display a short message*/
	err_printf(msg_moprc_inf_rc_res,mi->source[0],mi->source[1],
					mi->source[2],mi->source[3],
					mi->source[4],mi->source[5]);

/*Set the console message number and timer*/
	mp->rc.message_number = 0xff;
	mp->rc.timer = MOP$K_RC_CONNECTION_TIMEOUT;

/*Set the console reserved flag*/
	mp->flags |= MOP$M_RC_RESERVED;	

/*Set the user address*/
	memcpy(mp->rc.console_user,mi->source,6);

/*Init the indices and counts*/
	mp->rc.tx_index_in = mp->rc.tx_index_out = 0;
	mp->rc.tx_sent = 0; 
	mp->rc.rx_index_in = mp->rc.rx_index_out = 0;

/*Create the remote console watchdog process*/
	sprintf(name,"moprcwd_%4.4s",fp->ip->name);
	mp->rc.timer_pid = krn$_create (mop_rc_watchdog,null_procedure,
								0,2,-1,1024*4,
								name,
								"tt", "r", 
								"tt", "w",
								"tt", "w",
								fp->ip->name);

/*Startup a remote console shell*/
	sprintf(name,"rc%2.2s",&(fp->ip->name[2]));
	shv = malloc (sizeof (char *) * 2, shv);
	shv [0] = mk_dynamic ("shell");
	mp->rc.shell_pid =krn$_create (
		sh,			/* address of process		*/
		shell_startup,		/* startup routine		*/
		0,			/* completion semaphore		*/
		3,			/* process priority		*/
		-1,			/* cpu affinity mask		*/
		1024 * 8,		/* stack size			*/
		"rem_cns",		/* process name			*/
		name, "r",		/* stdin			*/
		name, "w",		/* stdout			*/
		name, "w",		/* stderr			*/
	 	1, shv);

/*Return success*/
	return(msg_success);
}


/*+
 * ============================================================================
 * = mop_handle_sys_id - Handles the system ID message.                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle the system ID message.
 *
 * FORM OF CALL:
 *
 *	mop_handle_sys_id(fp,mi);
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

int mop_handle_sys_id(struct FILE *fp,struct MSG_RCVD *mi)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
struct MOP$SYSTEM_ID *id;		/*Pointer to the start of the ID pkt*/
struct NODE_ID nid;			/*Node ID*/
struct FILE *ndp;			/*Pointer to the network database file*/
int found,i,status;			/*Flag, counter, status*/

	trace("mop_handle_sys_id\n",0);

#if MOPRC_DEBUG
	print_pkt(mi->msg,mi->size);
#endif

/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Get the correct typecast for the received message*/
	id = (struct MOP$SYSTEM_ID*)(mi->msg);

/*Open the network database file*/
	ndp = fopen(mp->ndb_filename,"r+");
	if(!ndp)
	    return(msg_failure);

/*See if the element node is already on the list*/
	found = FALSE;
	fseek(ndp,0,0);
	for(i=0; i<mp->ndb_count; i++)
	{
	/*Read an ID*/
	    status = fread(&nid,sizeof(struct NODE_ID),1,ndp);
	    if(!status)
	    {
	    /*Return a failure*/
		ni_error(mp->short_name,msg_mop_err_n_net_fl);
		fclose(ndp);
		return(msg_mop_err_n_net_fl);
	    }

	/*The element is already on the list say we found it*/
	    if(!memcmp(nid.sa,mi->source,6))
	    {
		found = TRUE;
		break;
	    }
	}

/*If we didn't find this node put it on the list*/
	if(!found)
	{
	/*Make an element*/
	    mp->ndb_count++;
	    memcpy(nid.sa,mi->source,6);

	/*Add it to the list*/
	    status = fwrite(&nid,sizeof(struct NODE_ID),1,ndp);
	    if(!status)
	    {
	    /*Return a failure*/
		ni_error(mp->short_name,msg_mop_err_nr_net_fl);
		fclose(ndp);
		return(msg_mop_err_nr_net_fl);
	    }
#if MOPRC_DEBUG
	    print_enet_address("Found: ",nid.sa);
#endif
	}

/*Return success*/
	fclose(ndp);
	return(msg_success);
}


/*+
 * ============================================================================
 * = mop_handle_con_cmd_n_poll - Handles the console command and poll message. =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle the console command and poll message.
 *	This routine is really the main part of the remote console driver.
 *	When the command and poll message is received. This routine
 *	Will first see if this is a new message, if it is the data
 *	received is moved into the received buffer, and then any data
 *	that needs to be transmitted is. If it is an old message
 *	the old data that needs to be transmitted is.
 *
 * FORM OF CALL:
 *
 *	mop_handle_con_cmd_n_poll(fp,mi);
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

int mop_handle_con_cmd_n_poll(struct FILE *fp,struct MSG_RCVD *mi)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
struct MOP$CON_CMD_N_POLL *ccp;		/*Console command and poll message*/
struct MOP$CON_RSP_N_ACK *cra;		/*Console response ack message*/

	trace("mop_handle_con_cmd_n_poll\n",0);

#if MOPRC_DEBUG
	print_pkt(mi->msg,mi->size);
#endif

/*Get the pointer to the MOP counters*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Make sure the console is reserved*/
	if((mp->flags & MOP$M_RC_RESERVED) != MOP$M_RC_RESERVED)
	{
		ni_error(mp->short_name,msg_moprc_err_rc_n_res);
		return(msg_failure);
	}

/*Reset the timer*/
	mp->rc.timer = MOP$K_RC_CONNECTION_TIMEOUT;

/*Get the command and poll message*/
	ccp = (struct MOP$CON_CMD_N_POLL*)(mi->msg);

/*Allocate the message for the response and acknowledge*/
	cra = ndl_allocate_msg(fp->ip,mi->version,MOP$K_INC_LENGTH);

/*Build the header portion of the ack*/
	cra->MOP$B_CRA_CODE = MOP$K_MSG_CON_RSP_N_ACK;	
	cra->MOP$B_CRA_FLAGS = (ccp->MOP$B_CCP_FLAGS) & MOP$M_FLAG_NUMBER;	

/*Check to see if this message is new or old*/
/*handle it appropriatly*/
	if(mp->rc.message_number != ((ccp->MOP$B_CCP_FLAGS)&MOP$M_FLAG_NUMBER))
	{                
	/*Its a new message*/
	    mp->rc.message_number = (ccp->MOP$B_CCP_FLAGS)&MOP$M_FLAG_NUMBER;

	/*Take care of some things if some characters*/
	/*have been sent*/
	    if(mp->rc.tx_sent)
	    {
	    /*Update the tx index*/
		mp->rc.tx_index_out += mp->rc.tx_sent;
		mp->rc.tx_index_out %= MOP$K_CH_OUT_MAX;
	    /*Post a hole*/
		krn$_bpost(&(mp->rc.tx_hole));
	    }

	/*Handle the received characters*/
	    moprc_rx(mp,mi,cra,ccp);

	/*Get a new count of how many characters we have*/
	    mp->rc.tx_sent = ((mp->rc.tx_index_in % MOP$K_CH_OUT_MAX) - 
			   mp->rc.tx_index_out) % MOP$K_CH_OUT_MAX;

	/*Send the characters*/
	    moprc_tx(mp,cra);
	}
	else
	{
	/*Its an old message*/
	    mp->rc.message_number = (ccp->MOP$B_CCP_FLAGS)&MOP$M_FLAG_NUMBER;

	/*Send the characters*/
	    moprc_tx(mp,cra);
	}

#if MOPRC_DEBUG
	err_printf("index_out: %d c_sent: %d\n",mp->rc.tx_index_out,mp->rc.tx_sent);
	err_printf("flags: %02x size: %d\n",cra->MOP$B_CRA_FLAGS,mi->size);
	err_printf("  index_in: %d\n",mp->rc.rx_index_in);
#endif

/*Send the message using the size of the data portion of the*/
/*message (tx_sent)+ the header portion of the message 2 bytes*/
	ndl_send(fp,mi->version,(unsigned char*)cra,(mp->rc.tx_sent)+2,
							MOP$K_INC_LENGTH,
							mi->source,
							MOP$K_PROT_CONSOLE);

/*Return success*/
	return(msg_success);
}


/*+
 * ============================================================================
 * = moprc_tx - Handles the remote console tx chars.                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle the transmitting of chars to the remote
 *	console.
 *
 * FORM OF CALL:
 *
 *	mop_tx(mp,cra);
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct MB *mp - Pointer to the mop block.
 *	struct MOP$CON_RSP_N_ACK *cra - Pointer to the ACK message.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void moprc_tx (struct MB *mp,struct MOP$CON_RSP_N_ACK *cra)
{
int i,index;
char *data;

/*Fill in data to be transmitted*/
	data = (char*)&cra->MOP$B_CRA_DATA;

/*Move the data*/
	index = mp->rc.tx_index_out;
	for(i=0 ; (i<mp->rc.tx_sent)&&(i<MOP$K_MAX_DATA_AVAIL); i++)
	{
	    data[i] = mp->rc.tbuf[index++];
	    index %= MOP$K_CH_OUT_MAX;
	}

/*Make sure we really only remember what we sent*/
	mp->rc.tx_sent = i;
}


/*+
 * ============================================================================
 * = moprc_rx - Handles the remote console rx chars.                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle the receiving of chars from the remote
 *	console.
 *
 * FORM OF CALL:
 *
 *	mop_rx(mp,mi,cra,ccp);
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct MB *mp - Pointer to the mop block.
 *	struct MSG_RCVD *mi - Pointer to the general message received.
 *	struct MOP$CON_RSP_N_ACK *cra - Pointer to the ACK message.
 *	struct MOP$CON_CMD_N_POLL *ccp - Pointer to the cmd and poll message.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void moprc_rx (struct MB *mp,struct MSG_RCVD *mi,
						struct MOP$CON_RSP_N_ACK *cra,
						struct MOP$CON_CMD_N_POLL *ccp)
{
char *data;
int i;

/*Test to see if we have enough room in our RX buffer*/
/*the 2 in size-2 accounts for the header*/
	if((mi->size-2)>((MOP$K_CH_IN_MAX-1)-
					((mp->rc.rx_index_in%MOP$K_CH_IN_MAX)-
			     		mp->rc.rx_index_out))%MOP$K_CH_IN_MAX)
	/*We don't have enough room have the data resent*/
	    cra->MOP$B_CRA_FLAGS |= MOP$M_FLAG_CMD_LOST;
	else
	{
	/*Fill in the RX buffer*/
	    data = (char*)&ccp->MOP$B_CCP_DATA; 

	/*The 2 in size-2 accounts for the header portion of the message*/
	    for(i=0; i<(mi->size-2); i++)
	    {
	    /*Enque the data*/
		mp->rc.rbuf[(mp->rc.rx_index_in)] = data[i];
		(mp->rc.rx_index_in)++;
		mp->rc.rx_index_in %= MOP$K_CH_IN_MAX;
	    }

	/*Let everyone know that characters made mores chars available*/
	/*2 is for the header*/
	if(mi->size>2)
	    krn$_bpost(&(mp->rc.rx_avail));
	}
}


/*+
 * ============================================================================
 * = moprc_write - Write a block of data to the remote console                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Write a block of data to the remote console. Translate
 * 	newlines into CRLFs.
 *  
 * FORM OF CALL:
 *  
 *	moprc_write(fp, size, number, buf)
 *  
 * RETURNS:
 *
 *      len - bytes transferred
 *
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to a file.
 *	int size	- size of item
 *	int number	- number of items
 *	char *buf - Pointer to the buffer the data is taken from.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int moprc_write (struct FILE *fp, int size, int number, char *buf)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the mop block*/ 
char	*s;
int	l;

/*Get a pointer to the port block*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Write the characters*/
	s = buf;
	l = size * number;
	fp->ip->len[0] += l;
	while (--l >= 0) {
	    if (*s == '\n') {
		moprc_out (mp,0x0d);
	    }
	    moprc_out (mp,*s);
	    s++;
	}

	return size * number;
}


/*+
 * ============================================================================
 * = moprc_read - Read a block of characters from the remote console.         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Read in a block of characters from the remote console.
 * 	Return the number of characters read in.
 *
 *  
 * FORM OF CALL:
 *  
 *	moprc_read (fp, len, buf)
 *  
 * RETURNS:
 *
 *      s - Bytes transferred. Anything other than the amount requested
 *	    indicates EOF or an error.
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to a file.
 *	int size - size of item
 *	int number - number of items to be transferred.
 *	char *buf - Pointer to the buffer the data is written to.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int moprc_read (struct FILE *fp,int size, int number,char *buf) {
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the mop block*/ 
int l;
char *s, c;

/*Get a pointer to the port block*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Get the characters*/
	l = size * number;
	s = buf;
	while (--l >= 0) {
	    *s++ = c = moprc_in (mp);
	    if (c == 0x1a) {
		s--;
		break;
	    }
	}
	return s - buf;
}


/*+
 * ============================================================================
 * = moprc_out - Puts a character in the remote console TX buffer.            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine puts a character in the remote console TX buffer.  
 *  
 * FORM OF CALL:
 *  
 *	moprc_out (mp,c);
 *  
 * RETURNS:
 *
 *	None       
 *       
 * ARGUMENTS:
 *
 *	struct MB *mp - Pointer to the MOP block.
 *	char c - Character to be transmitted.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

void moprc_out (struct MB *mp,char c)
{

/*Wait for a hole if our buffer is full*/
	while((((mp->rc.tx_index_in%MOP$K_CH_OUT_MAX)-
		 mp->rc.tx_index_out)%MOP$K_CH_OUT_MAX) >=
		(MOP$K_CH_OUT_MAX-1))
	    krn$_wait (&(mp->rc.tx_hole));

/*Enque the character*/
	mp->rc.tbuf[mp->rc.tx_index_in] = c;
	mp->rc.tx_index_in++;
	mp->rc.tx_index_in %= MOP$K_CH_OUT_MAX;
	
#if MOPRC_DEBUG
	err_printf("index_in: %d c: %c\n",mp->rc.tx_index_in,c);
#endif
}


/*+
 * ============================================================================
 * = moprc_in - Gets a character from the remote console buffer.              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine gets a character from the remote console buffer.  
 *  
 * FORM OF CALL:
 *  
 *	moprc_in (mp)
 *  
 * RETURNS:
 *
 *	c - Character that had been received.
 *       
 * ARGUMENTS:
 *
 *	struct MB *mp - Pointer to the MOP block.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

char moprc_in (struct MB *mp)
{
char	c;

/*Wait for a character if we have to*/
	while(mp->rc.rx_index_in == mp->rc.rx_index_out)
	    krn$_wait (&(mp->rc.rx_avail));

/*Deque the character*/
        c = mp->rc.rbuf[mp->rc.rx_index_out];
	(mp->rc.rx_index_out)++;
	mp->rc.rx_index_out %= MOP$K_CH_IN_MAX;

#if MOPRC_DEBUG
	err_printf("index_in: %d index_out: %d c: %c\n",mp->rc.rx_index_in,
						mp->rc.rx_index_out,c);
#endif
        return c;
}


/*+
 * ============================================================================
 * = mop_send_rem_boot - Sends the mop remote boot message.                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will send the mop remote boot message.
 *
 * FORM OF CALL:
 *
 *	mop_send_rem_boot(fp,dest,pwd);
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the port file being used.
 *
 *	char *dest    - Pointer to 6 bytes containing the destination address.
 *	unsigned *pwd - Pointer to the password array
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_send_rem_boot(struct FILE *fp,unsigned char *dest,unsigned *pwd)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
struct MOP$BOOT *rb;			/*Remote boot message*/

int i;
unsigned char *c;

	trace("mop_send_rem_boot\n",1);

/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Allocate the message*/
	rb = ndl_allocate_msg(fp->ip,MOP$K_VERSION_4,0);

/*Fill in the info*/
	rb -> MOP$B_BOOT_CODE = MOP$K_MSG_BOOT;
	rb -> MOP$Q_BOOT_VERIFICATION[0] = pwd[0];
	rb -> MOP$Q_BOOT_VERIFICATION[1] = pwd[1];
	rb -> MOP$B_BOOT_PROCESSOR = pwd[2];
	rb -> MOP$B_BOOT_CONTROL = 0;
	rb -> MOP$B_BOOT_DEVICE_ID_LEN = 0;
	rb -> MOP$B_BOOT_DEVICE_ID = 0;

	err_printf(msg_moprc_001,pwd[2],pwd[1],pwd[0]);

/*Send the message*/
/*with the extra 14 bytes for the envelope*/
	ndl_send(fp,MOP$K_VERSION_4,(char*)rb,sizeof(struct MOP$BOOT),0,dest,
							   MOP$K_PROT_CONSOLE);
}

#if MONET
static unsigned int trig_mopver;
static unsigned int trig_verlo = 0;
static unsigned int trig_verhi = 0;
static unsigned int trig_msglo = 0;
static unsigned int trig_msghi = 0;
static unsigned int trig_proc;

int mop_handle_trig_boot(struct FILE *fp,struct MSG_RCVD *mi)
{
    unsigned int status;
    unsigned char *msg_ptr;
    unsigned char *temp_ptr = mi->msg;
    struct EVNODE *evp,evn;
    unsigned char str[16];

    moptrig_debug = 0;
    evp = &evn;
    if (ev_read("mop_debug", &evp, 0) == msg_success) {
	if (!(strcmp(evp->value.string, "ON"))) {
	    moptrig_debug = 1;
	}
    }

    if (moptrig_debug) {
	pprintf("\nMOP trigger: ver %d, inc_len %d, size %d\n", mi->version, mi->inc_len, mi->size);
	pprint_pkt(mi->msg, mi->size);
    }

    if (ev_read("mop_version", &evp, 0) == msg_success) {
	trig_mopver = evp->value.integer;
    } else {                  
	pprintf("MOP trigger - invalid MOP version ev\n");
	return msg_failure;
    }

    if (mi->version != trig_mopver) {
	if (moptrig_debug)
	    printf("Ignoring MOP trigger - version %d\n", mi->version);
	return msg_success;
    }

    status = ev_read("mop_password", &evp, 0); 
    if (status != msg_success) {
	pprintf("MOP trigger - invalid MOP password ev\n");
	return msg_failure;
    }

    if (moptrig_debug)
	pprintf("MOP trigger: password = %s\n", evp->value.string);

    if (strlen(evp->value.string) != 20 || evp->value.string[2] != '-' || evp->value.string[11] != '-') {
	pprintf("MOP password %s incorrect format (pp-vvvvvvvv-vvvvvvvv)\n", evp->value.string);
	return msg_failure; 
    }

/* Extract the password from the env variable */

    memcpy(str,&evp->value.string[0],2);
    str[2] = '\0';
    trig_proc = xtoi((const char *)str);

    memcpy(str,&evp->value.string[3],8);
    str[8] = '\0';
    trig_verhi = xtoi((const char *)str);

    memcpy(str,&evp->value.string[12],8);
    str[8] = '\0';
    trig_verlo = xtoi((const char *)str);

    msg_ptr = (unsigned char *)&trig_msghi;
    msg_ptr[3] = temp_ptr[1];
    msg_ptr[2] = temp_ptr[2];
    msg_ptr[1] = temp_ptr[3];
    msg_ptr[0] = temp_ptr[4];

    msg_ptr = (unsigned char *)&trig_msglo;
    msg_ptr[3] = temp_ptr[5];
    msg_ptr[2] = temp_ptr[6];
    msg_ptr[1] = temp_ptr[7];
    msg_ptr[0] = temp_ptr[8];

    if (moptrig_debug)
	pprintf("MOP trigger: verhi %x msghi %x verlo %x msglo %x\n", trig_verhi, trig_msghi, trig_verlo, trig_msglo);

    if ((trig_verhi == trig_msghi) && (trig_verlo == trig_msglo)) {
	pprintf("\nMOP trigger\n");
	tt_reprompt = 1;
	boot_system("b");
	return(msg_success);
    } else {
	if (moptrig_debug)
	    pprintf("MOP trigger: Passwords don't match\n");
	return(msg_failure);
    }
}	
#endif
