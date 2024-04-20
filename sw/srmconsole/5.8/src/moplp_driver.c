/* file:	moplp_driver.c
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
 *  Abstract:	MOP Loop driver.
 *
 *	This module implements the MOP loop functions.
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
 *	dwn     15-Mar-1992	Fixed max pattern size in mop_loop_requester().
 *				Also included killpendings in mop_wait().
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
#include "cp$src:dynamic_def.h"			/*Memory allocation defs*/
#include "cp$src:msg_def.h"			/*Message definitions*/
#include "cp$src:ni_env.h"			/*Environment variables*/
#include "cp$src:ni_dl_def.h"			/*Datalink constants*/
#include "cp$src:mop_def.h"			/*MOP definitions*/
#include "cp$src:mb_def.h"			/*MOP block defs*/
#include "cp$src:mop_counters.h"		/*MOP counters*/
#include "cp$src:ev_def.h"			/*Environment vars*/
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

/*Some debug constants*/
#define		MOPLP_DEBUG 0

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

/*External references*/
extern int cpip;

extern struct FILE* fopen(char*,char*);
extern void* ndl_allocate_msg(struct INODE *ip,int version,int inc_len);
extern void mop_read_env(struct NI_ENV *ev,char *name);

/*Routine definitions*/
void mop_build_loop_data(struct INODE *ip);
int mop_build_loop_fwd(struct INODE *ip,struct MOP$LOOP_FWD *lf,
						     unsigned short size,
						     unsigned char *fwd,
						     unsigned short rec_number,
						     int patt);
void mop_fill(unsigned char *loop_data,char *mem,int num,int patt);
void mop_handle_loopback(struct FILE *fp,unsigned char *lpbck,
							unsigned char *source,
							unsigned short size,
							int version);
void mop_handle_loop_reply(struct FILE *fp,unsigned char *lpbck,
							unsigned char *source,
							unsigned short size,
							int version);
int mop_loop_reply (char *port_file);
int mop_loop_init(struct INODE *ip);
int mop_loop_requester (struct FILE *pf,int version);
int moplp_wait(struct MB *mp,struct NI_ENV *ev, int secs);
void mop_send_loop_fwd(struct FILE *fp,unsigned char *dest,unsigned short size,
							unsigned char *fwd,
							int ver,
							unsigned short rec_number,
				       			int patt);
void mop_send_loop_tst(struct FILE *pf,unsigned char *dest,
							unsigned short is,
							int ver,
							unsigned short rec_number,
				       			int patt);
void mop_send_loop_pt_pt(struct FILE *fp,unsigned char *dest,
							unsigned short size,
							int ver,int patt);


/*+
 * ============================================================================
 * = mop_send_loop_pt_pt - Sends the loopback point to point message.         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will send the loopback point to point message.
 *	This routine will allocate the buffer,fill in the opcodes etc.,
 *	and fill in the data pattern.
 *
 * FORM OF CALL:
 *
 *	mop_send_loop_pt_pt(fp,dest,size,ver,patt)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file to be used.
 *
 *	char *dest - Pointer to 6 bytes containing the destination address.
 *		     If its a null address we use the loopback multicast
 *		     address
 *
 *	unsigned short size - Size of the loopback packet.
 *
 *	int ver - Specifies whether we want to use version 3 or 4.
 *		      			MOP$K_VERSION_3 = versions 1-3.
 *		      			MOP$K_VERSION_4 = version 4.
 *
 *	int patt - Specifies the pattern to be used.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_send_loop_pt_pt(struct FILE *fp,unsigned char *dest,
							unsigned short size,
							int ver,int patt)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
int i,ms,sum;
struct MOP$PT_PT_LOOP_DATA *lpbck;

	trace("mop_send_loop_pt_pt\n",0);

/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Allocate the message*/
	lpbck = ndl_allocate_msg(fp->ip,ver,MOP$K_DONT_INC_LENGTH);

/*Check for a null address*/
	sum = 0;
	for(i=0; i<6; i++)
		sum += (int)dest[i];

/*If it is a null address we'll use the multicast address*/
	if(!sum)
	{
	/*The MOP loopback multicast*/
	    dest[0] = 0xcf; 
	    for(i=1; i<6; i++)
		dest[i] = 0x00; 
	}

/*Fill in the info*/
	lpbck -> MOP$B_LP_FUNCTION = MOP$K_MSG_LOOP_DATA;
	lpbck -> MOP$B_LP_RECEIPT = 0;

/*Fill in the pattern not including the function and receipt fields*/
	mop_fill((unsigned char*)mp->lp.loop_data,
		(char*)((long)lpbck+(sizeof(struct MOP$PT_PT_LOOP_DATA)-1)),
		(size-(sizeof(struct MOP$PT_PT_LOOP_DATA)-1)),
		patt);

/*Send the message*/
	ndl_send(fp,ver,(char*)lpbck,size,MOP$K_DONT_INC_LENGTH,
						     dest,MOP$K_PROT_LOOPBACK);

}


/*+
 * ============================================================================
 * = mop_send_loop_fwd - Sends the loopback forward message.                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will send the loopback forward message.
 *
 * FORM OF CALL:
 *
 *	mop_send_loop_fwd(fp,dest,size,fwd,ver,rec_number,patt)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file beign used.
 *
 *	char *dest - Pointer to 6 bytes containing the destination address.
 *		     If its a null address we use the loopback multicast
 *		     address
 *
 *	unsigned char *fwd - Pointer to 6 bytes containing the forward address.
 *
 *	int ver - Specifies whether we want to use version 3 or 4.
 *		      			MOP$K_VERSION_3 = versions 1-3.
 *		      			MOP$K_VERSION_4 = version 4.
 *
 *	rec_number - Receipt number for this message.
 *
 *	int patt - Specifies the pattern to be used.
 *
 *	unsigned short size - Size of the loopback information field.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_send_loop_fwd(struct FILE *fp,unsigned char *dest,unsigned short size,
							unsigned char *fwd,
							int ver,
							unsigned short rec_number,
							int patt)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
int i,sum;
struct MOP$LOOP_FWD *lf;
struct MOP$LOOPED_DATA *lr;

	trace("mop_send_loop_fwd\n",0);

/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Allocate the message*/
/*Add an extra 14 bytes for the envelope*/
	lf = ndl_allocate_msg(fp->ip,ver,MOP$K_DONT_INC_LENGTH);

/*Check for a null address*/
	sum = 0;
	for(i=0; i<6; i++)
		sum += (int)dest[i];

/*If it is a null address we'll use the multicast address*/
	if(!sum)
	{
	/*The MOP loopback multicast*/
	    dest[0] = 0xcf; 
	    for(i=1; i<6; i++)
		dest[i] = 0x00; 
	}

/*Fill in the info*/
	lf -> MOP$W_LF_SKIP_COUNT = 0;
	lf -> MOP$R_LF.MOP$W_LP_FUNCTION = MOP$K_MSG_LOOP_FORWARD;
	memcpy(lf->MOP$R_LF.MOP$B_LP_ADDRESS,fwd,6);

/*No lets put a reply in the envelope*/
	lr = (struct MOP$LOOPED_DATA*)((long)lf+10);
	lr -> MOP$W_LP_FUNCTION = MOP$K_MSG_LOOP_REPLY;
	lr -> MOP$W_LP_RECEIPT = rec_number;

/*Fill in the data*/
	mop_fill((unsigned char*)mp->lp.loop_data,
		 (char*)((long)lr+(sizeof(struct MOP$LOOPED_DATA)-1)),size,
									patt);

/*Send the message*/
/*with the extra 14 bytes for the envelope*/
	ndl_send(fp,ver,(char*)lf,(size+14),MOP$K_DONT_INC_LENGTH,dest,
							   MOP$K_PROT_LOOPBACK);
}


/*+
 * ============================================================================
 * = mop_send_loop_tst - Sends the loopback forward message.                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will send the loopback forward message. This routine
 *	differs from send_loop_fwd in that is to be used for DVT and
 *	diagnostics.
 *
 * FORM OF CALL:
 *
 *	mop_send_loop_tst(pf,dest,is,ver,rec_number,patt)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *pf - Pointer to the port file being used.
 *
 *	char *dest - Pointer to 6 bytes containing the destination address.
 *		     If its a null address we use the loopback multicast
 *		     address
 *
 *	int ver - Specifies whether we want to use version 3 or 4.
 *		      			MOP$K_VERSION_3 = versions 1-3.
 *		      			MOP$K_VERSION_4 = version 4.
 *
 *	rec_number - Receipt number for this message.
 *
 *	int patt - Specifies the pattern to be used.
 *
 *	unsigned short is - Size of the loopback information field.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_send_loop_tst(struct FILE *pf,unsigned char *dest,
							unsigned short is,
							int ver,
							unsigned short rec_number,
							int patt)
{
struct NI_GBL *np;		/*Pointer to NI global database*/	
struct MB *mp;			/*Pointer to the MOP block*/
int ms;				/*Entire message size*/
unsigned char *msg,*inp;	/*Message pointers*/

	trace("mop_send_loop_tst\n",0);

/*Get the pointer to the MOP block*/
     	np = (struct NI_GBL*)pf->ip->misc;
  	mp = (struct MB*)np->mbp;   

/*Get the message*/
	ms = ndl_preallocate(pf->ip,&msg,&inp,ver,MOP$K_DONT_INC_LENGTH,
						is,
						MOP$K_PROT_LOOPBACK);

/*Build the loop fwd part of the message*/
	mop_build_loop_fwd(pf->ip,(struct MOP$LOOP_FWD*)inp,is,mp->sa,
		    					rec_number,patt);

/*Add the loop tx size*/
	mp->lp.tx_bytes += is;

/*Send the message*/
#if MOPLP_DEBUG
	print_pkt(msg,ms);
#endif
	ndl_send(pf,NDL$K_NO_VERSION,msg,ms,0,dest,0);
}

                                       
/*+
 * ============================================================================
 * = moplp_wait - Wait for a number of loop messages   		              =
 * ============================================================================
 *                                     
 * OVERVIEW:
 *                                     
 *	This routine will wait for a number of loop messages. The
 *	loop messages are specified by the environment variable
 *	loop_count.
 *
 * FORM OF CALL:         
 *                                    
 *	moplp_wait(mp,ev,secs) 
 *                  
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *               
 * ARGUMENTS:
 *
 *	struct MB *mp - Pointer to the mop block.
 *	struct NI_ENV *ev - Pointer to the environment variables.
 *	int secs - Number of seconds to wait for.          
 *          
 * SIDE EFFECTS:  
 *
 *      None
 *
-*/

int moplp_wait(struct MB *mp, struct NI_ENV *ev, int secs)
{                
struct EVNODE evn,*evp;		/*Pointers to the evnode*/
int i;				/*Index into the message outstanding list*/
int s;			      	/*How many seconds we've already waited*/
int status;			/*Status to be returned*/
int display_error;		/*Display the error message*/

	trace("moplp_wait\n",0);                  

/*Set the fast boot flag if the environment variable exists*/
	evp = &evn;
	if(ev_read("supress_timeout_message",&evp,0) == msg_success)
	    display_error=0;
	else
	    display_error=1;

/*Init the status*/
	status = msg_success;

/*Adjust the seconds*/
	secs = secs*10;

/*Go through the entire message outstanding list*/
	for(i=0,s=0; i< mp->lp.mo_count; i++)
	{
	/*For a number of seconds keep checking the loop count*/
	    for(;s<secs; s++)
	    {
	    /*If we have enough exit the loop*/
		if(mp->lp.mo[i].count == ev->loop_count)
		    break;

	    /*Check for kill*/
		if(killpending())
		{
		    status = msg_killed;
		    break;
		}

	    /*Sleep for a second*/
		krn$_sleep(100);
	    }

	/*Check for kill*/
	    if(killpending())
	    {
		status = msg_killed;
		break;
	    }

	/*If we still don't have enough loop messages return an error*/
	    if((mp->lp.mo[i].count != ev->loop_count) && display_error)
	    {
		ni_error(mp->short_name,msg_mop_err_lp_to,mp->lp.mo[i].sa[0],
							mp->lp.mo[i].sa[1],
							mp->lp.mo[i].sa[2],
							mp->lp.mo[i].sa[3],
							mp->lp.mo[i].sa[4],
							mp->lp.mo[i].sa[5]);
		err_printf(msg_mop_err_lp_to1,i,mp->lp.mo[i].count,
							ev->loop_count);

	    /*Return an error*/
		status = msg_failure;
	    }
	}

/*If a list exists clean it up*/
	if(mp->lp.mo_count)
	{
	/*Reset the count*/
	    mp->lp.mo_count = 0;

	/*Give the memory back*/
	    free(mp->lp.mo, mo);
	}

/*Return a success*/
	return(status);
}


/*+
 * ============================================================================
 * = mop_loop_requester - Send loop messages.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *   	This routine will send loop messages. The process first looks
 *	at the message outstanding list. If a node doesn't have a message
 *	outstanding it sends the loop forward message to it. This process uses
 *   	the receipt number in the reply message to index into the message
 *	outstanding list.
 *                                            
 * FORM OF CALL:
 *                          
 *	 mop_loop_requester (pf,version)
 *
 * RETURN:                                
 *
 *	int Status - msg_success or
 *	msg_failure
 *		  Incorrect loop reply message size
 *                Bad loop reply data          
 *                Incorrect loop reply source
 *                Loop reply index was too large
 *                Loop request timed out
 *
 * ARGUMENTS:
 *                                                      
 *	struct FILE *pf - The file of the port that is to be used.
 *
 *   	int version - Specifies whether we want to use version 3 or 4.
 *   		      			MOP$K_VERSION_3 = versions 1-3.
 *		      			MOP$K_VERSION_4 = version 4.
 *                        
 * SIDE EFFECTS:  
 *                
 *      None
 *            
-*/               
                 
int mop_loop_requester (struct FILE *pf,int version)
{    
struct NI_GBL *np;		/*Pointer to NI global database*/	
struct MB *mp;			/*Pointer to the MOP block*/
struct NI_ENV *ev;		/*Pointer to the environment variables*/	
struct NI_DL *dl;		/*Pointer to the datalink*/	
struct FILE *lf;		/*Loop file pointer*/
struct NODE_ID nid;		/*Holder for a node id*/
int pt,is;			/*Pattern and information size*/
int i;				/*Counters*/
unsigned short mi;		/*Message index*/		
int status;
                            
	trace("mop_loop_requester\n",0);
                  
/*Get the pointer to the MOP block and environment variables*/
     	np = (struct NI_GBL*)pf->ip->misc;
  	mp = (struct MB*)np->mbp;   
	ev = (struct NI_ENV*)np->enp;     
	dl = (struct NI_DL*)np->dlp;

/*Read environment variables*/
    	mop_read_env(ev,pf->ip->name);
	(dl->cb->re)(ev,pf->ip->name);

/*Initialize the loop requester*/
     	if(mop_loop_init(pf->ip) != msg_success)
	    return(msg_failure);      
                  
/*Send the first messages*/
/*Open the loop file*/
	lf = fopen(mp->loop_file,"r");  
     	if(!lf)
	/*Close the open files*/
	     return(msg_failure);

/*Start at the beginning of the loop file*/              
	fseek(lf,0,0);

/*Initialize the pattern size and pattern type*/
	if(version == MOP$K_VERSION_3)
	{
    	    if(ev->loop_size > MOP$K_MAX_MSG_VER_3)
		is = ev->loop_size = MOP$K_MAX_MSG_VER_3;
	    else
		is = ev->loop_size;
	}
	else
	{
	    if(ev->loop_size > MOP$K_MAX_MSG_VER_4)
		is = ev->loop_size = MOP$K_MAX_MSG_VER_4;
	    else
		is = ev->loop_size;

	}
	if(ev->loop_patt == MOP$K_ALL)
	    pt = MOP$K_ALL_ZEROS;
	else
	    pt = ev->loop_patt;

/*Look through the loop file for nodes we send messages to*/
	mi=0;
	while((status = fread(&nid,sizeof(struct NODE_ID),1,lf)))
	{
	/*Set so many messages per node*/
	    for(i=0; i<ev->lp_msg_node; i++)
	    {
	    /*Copy the destination address*/
		memcpy(mp->lp.mo[mi].sa,nid.sa,6);

	    /*Send the loop message*/
		mop_send_loop_tst(pf,nid.sa,is,version,mi,pt);

	    /*Update the index*/
		mi++;

	    /*Get a new pattern type if we have to*/
		if(ev->loop_patt == MOP$K_ALL)
		{
		/*Get a new pattern*/
		    pt++;
		/*Start again if we have to*/
		    if(pt >= MOP$K_NUMBER_LOOP_PATT)
			pt = MOP$K_ALL_ZEROS;	
		}
                    
	    /*Get the new size*/
		is += ev->loop_inc;
	    /*If we past the max data size start again*/
		if(version == MOP$K_VERSION_3)
		{
		/*Check for version 3*/
		    if(is > MOP$K_MAX_MSG_VER_3)
			is = ev->loop_size;
		}
		else
		{
		/*Check for version 4*/
		    if(is > MOP$K_MAX_MSG_VER_4)
			is = ev->loop_size;
		}
	    }
	}

/*Close the file*/
	fclose(lf);
	return(msg_success);
}                                                  


/*+
 * ============================================================================
 * = mop_loop_init - Initialize the loop server.                              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will initialize the loop server.
 *	It will build the loop list.
 *
 * FORM OF CALL:
 *
 *	mop_loop_init(ip)
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - The inode of the port that is to be used.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mop_loop_init(struct INODE *ip)
{       
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
struct NI_ENV *ev;			/*Pointer to the environment variables*/	
struct FILE *lf;			/*Loop file pointer*/
struct NODE_ID nid;			/*Holder for a node id*/
struct MSG_OUTSTANDING *mo;		/*Local message oustanding list pointer*/
int i,status;
                                    
	trace("mop_loop_init\n",0);                  

/*Get the pointer to the MOP block and environment variables*/
	np = (struct NI_GBL*)ip->misc;
	mp = (struct MB*)np->mbp;
	ev = (struct NI_ENV*)np->enp;     

/*Create the outstanding message list*/
/*Open the loop file*/
	lf = fopen(mp->loop_file,"r");  
     	if(!lf)
	/*Close the open files*/
	     return(msg_failure);

/*Init the loop sizes*/
	mp->lp.tx_bytes = mp->lp.rx_bytes = 0;

/*Make the message outstanding list*/
/*Start at the beginning of the file*/              
	fseek(lf,0,0);

/*Make the message outstanding list using the loop file database*/	    
/*First lets findout how many nodes we have*/
	mp->lp.mo_count = 0;   
	while((status = fread(&nid,sizeof(struct NODE_ID),1,lf)))
	    mp->lp.mo_count++;

/*Account for several messages per node*/
	mp->lp.mo_count *= ev->lp_msg_node;

/*Lets get the memory for the message outstanding list*/
	mo = (struct MSG_OUTSTANDING*)malloc(
					(sizeof(struct MSG_OUTSTANDING)*
					mp->lp.mo_count), mo);

/*Let fill in the structures*/      
	for(i=0; i<mp->lp.mo_count; i++)
	{
     		mo[i].count = 0;              
     		mo[i].crc = 0;              
     		mo[i].time = 0;              
     		mo[i].message_sent = FALSE;              
	}

/*Build the table that is to be used for the loopback data*/
	mop_build_loop_data(ip);

/*We've used the files now close them*/
	fclose(lf);

/*Make the message outstanding list valid*/
	mp->lp.mo = mo;  

/*Return a success*/
	return(msg_success);
}


/*+
 * ============================================================================
 * = mop_handle_loop_reply - Handle the loop replys.                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This process will handle the loop message with the reply function
 *	code.
 *
 * FORM OF CALL:
 *
 *	 mop_handle_loop_reply (fp,lpbck,source,size,version)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file opened. Returns 0 on an error.
 *
 *	char *lpbck - Pointer to the loopback portion of the MOP message.
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

void mop_handle_loop_reply(struct FILE *fp,unsigned char *lpbck,
							unsigned char *source,
							unsigned short size,
							int version)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
struct NI_ENV *ev;			/*Pointer to the environment variables*/	
struct MOP$LOOP_FWD *lf;		/*Loop message pointer*/
struct MOP$LOOPED_DATA *lr;		/*Pointer to the looped data field*/
struct MSG_OUTSTANDING *mo;		/*Local message oustanding list pointer*/
unsigned long crc;			/*CRC copy*/

	trace("mop_handle_loop_reply\n",0);

#if MOPLP_DEBUG
#if 0
	print_pkt(lpbck,size);
#endif
#endif

/*Get the pointer to the MOP block and environment variables*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;
	ev = (struct NI_ENV*)np->enp;

/*Add the loop rx size*/
	mp->lp.rx_bytes += size;

/*Get the pointer to the loop message*/
	lf = (struct MOP$LOOP_FWD*)lpbck;

/*Add the skip count, 2 bytes for the skip count and get a new pointer*/
	lr = (struct MOP$LOOPED_DATA*)((long)lpbck + 
					      (long)lf->MOP$W_LF_SKIP_COUNT + 
					       sizeof(lf->MOP$W_LF_SKIP_COUNT));

/*Get a pointer to the message outstanding*/
	mo = &(mp->lp.mo[lr->MOP$W_LP_RECEIPT]);

/*If the crc isn't saved yet save it*/
/*else compare it*/
	if(!(mo->count))
	    memcpy((unsigned char*)(&(mo->crc)),
				(unsigned char*)((unsigned int)lpbck+size),4);
	else
	{
	/*Compare the crcs*/
	    if(memcmp((unsigned char*)(&(mo->crc)),
				(unsigned char*)((unsigned int)lpbck+size),4))
	    {
	    /*Handle CRC error*/
	    	memcpy((unsigned char*)&crc,
				(unsigned char*)((unsigned int)lpbck+size),4);
		ni_error(mp->short_name,msg_moplp_err_crc,
					lr->MOP$W_LP_RECEIPT,mo->crc,crc);
	    /*Deallocate the message*/
		ndl_deallocate_msg(fp->ip,lpbck,version,MOP$K_DONT_INC_LENGTH);
		return;
	    }						
	}

/*If the loop server is killed deallocate the message and return*/
	if(mp->lp.kill_server)
	{
	    ndl_deallocate_msg(fp->ip,lpbck,version,MOP$K_DONT_INC_LENGTH);
	    return;
	}

/*Check the count and send it again if we have to*/
	mo->count++;
	if((mo->count < ev->loop_count) || !(ev->loop_count))
	{
	/*Send the message again*/
	/*Add the loop tx size*/
	    mp->lp.tx_bytes += size;

	/*Fix the skip count*/
	    lf->MOP$W_LF_SKIP_COUNT = 0;
	    ndl_send(fp,version,lpbck,size,MOP$K_DONT_INC_LENGTH,source,
							   MOP$K_PROT_LOOPBACK);
	}
	else
	/*We're done with this message*/
	    ndl_deallocate_msg(fp->ip,lpbck,version,MOP$K_DONT_INC_LENGTH);
}


/*+
 * ============================================================================
 * = mop_handle_loopback - Handle the MOP loopback message.                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle MOP loopback message. This routine excepts
 *	a pointer to the loopback portion of the message (Above the protocol).
 *	This is indpendent of V3 or V4 packets.
 *
 * FORM OF CALL:
 *
 *	mop_handle_loopback(fp,lpbck,source,size,version)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file opened. Returns 0 on an error.
 *
 *	char *lpbck - Pointer to the loopback portion of the MOP message.
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

void mop_handle_loopback(struct FILE *fp,unsigned char *lpbck,
							unsigned char *source,
							unsigned short size,
							int version)
{
struct MOP$LOOP_FWD *lf;		/*Loop message pointer*/
char *gp;				/*Msg after skip count*/
unsigned short code;
struct MOP$LOOP_DATA *ld;		/*Rest of the loop message*/
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/

	trace("mop_handle_loopback\n",0);

#if MOPLP_DEBUG
	pprint_pkt(lpbck,size);
#endif

/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Get the pointer to the loop message*/
	lf = (struct MOP$LOOP_FWD*)lpbck;

/*Add the skip count, 2 bytes for the skip count and get a new pointer*/
	gp = (char*)((long)lpbck + (long)lf->MOP$W_LF_SKIP_COUNT + 
					       sizeof(lf->MOP$W_LF_SKIP_COUNT));

/*Get the function code then take the appropriate action*/
	code = *(unsigned short*)gp;
	switch(code)
	{
	    case MOP$K_MSG_LOOP_FORWARD:
 	    /*Get the rest of the message*/
		ld = (struct MOP$LOOP_DATA*)gp;
	    /*If its going to a multicast its illegal so don't respond or*/
	    /*if the server is not to be killed forward the message*/
		if(mp->lp.kill_server || 
		  (ld->MOP$B_LP_ADDRESS[0] & 1))
		    ndl_deallocate_msg(fp->ip,lpbck,version,
							MOP$K_DONT_INC_LENGTH);
		else
		{
		/*Bump the skip count*/
		    lf->MOP$W_LF_SKIP_COUNT += 8; 
 		/*forward the message*/

		    ndl_send(fp,version,lpbck,size,MOP$K_DONT_INC_LENGTH,
							ld->MOP$B_LP_ADDRESS,
							   MOP$K_PROT_LOOPBACK);
		}
		break;
	    case MOP$K_MSG_LOOP_REPLY:
	    /*If its a single loop then just dump the message*/
		if(!(mp->lp.mo_count))
		{
		    if(!cpip)
			printf(msg_mop_inf_reply,source[0],source[1],source[2],
						    source[3],source[4],source[5]);
	        /*Deallocate the message*/
		    ndl_deallocate_msg(fp->ip,lpbck,version,
							MOP$K_DONT_INC_LENGTH);
		}
		else
		    mop_handle_loop_reply(fp,lpbck,source,size,version);
		break;
	    default:
#if MOPLP_DEBUG
		print_enet_address("Unknown loop code from: ",source);
		print_pkt(lpbck,size);
#endif
	    /*Deallocate the message*/
		ndl_deallocate_msg(fp->ip,lpbck,version,MOP$K_DONT_INC_LENGTH);
	}
}


/*+
 * ============================================================================
 * = mop_fill - Fill in a block memory.                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine fills in a section of memory.
 *
 * FORM OF CALL:
 *
 *      mop_fill(loop_data,mem,num,patt)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	unsigned char *loop_data - Pointer to the loop data table.
 *
 *	char *mem - Pointer to the memory to be filled.
 *
 *	int num - Number of bytes to be filled.
 *
 *	int patt - Pattern types include:
 *					MOP$K_ALL_ZEROS
 *					MOP$K_ALL_ONES
 *					MOP$K_ALL_FIVES
 *					MOP$K_ALL_AAS
 *					MOP$K_INCREMENTING
 *					MOP$K_DECREMENTING
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_fill(unsigned char *loop_data,char *mem,int num,int patt)
{
unsigned char *data;			/*Pointer to the data in our table*/
int i,j;

	trace("mop_fill\n",0);

/*Get a pointer to the saved pattern*/
	data = &loop_data[patt*MOP$K_MAX_DATA_AVAIL_V3_NO_LEN];

/*Copy the data*/
	memcpy(mem,data,num);
}


/*+
 * ============================================================================
 * = mop_build_loop_fwd - builds the loopback forward message.                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will build a loopback forward message.
 *
 * FORM OF CALL:
 *
 *	mop_build_loop_fwd(fp,lf,is,fwd,ver,rec_number,patt)
 *
 * RETURNS:
 *
 *	int patt_size - The size of the loop pattern.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Pointer to the inode being used.
 *
 *	struct MOP$LOOP_FWD *lf - Pointer where the loop message is saved.
 *
 *	unsigned short size - Size of the entire loopback message.
 *
 *	unsigned char *fwd - Pointer to 6 bytes containing the forward address.
 *
 *	rec_number - Receipt number for this message.
 *                                         
 *	int patt - Specifies the pattern to be used.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mop_build_loop_fwd(struct INODE *ip,struct MOP$LOOP_FWD *lf,
						     unsigned short size,
						     unsigned char *fwd,
						     unsigned short rec_number,
						     int patt)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
int i,sum;
struct MOP$LOOPED_DATA *lr;

	trace("mop_build_loop_fwd\n",0);

/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)ip->misc;
	mp = (struct MB*)np->mbp;

/*Fill in the info*/
	lf -> MOP$W_LF_SKIP_COUNT = 0;
	lf -> MOP$R_LF.MOP$W_LP_FUNCTION = MOP$K_MSG_LOOP_FORWARD;
	memcpy(lf->MOP$R_LF.MOP$B_LP_ADDRESS,fwd,6);
                                           
/*No lets put a reply in the envelope*/
	lr = (struct MOP$LOOPED_DATA*)((long)lf+10);
	lr -> MOP$W_LP_FUNCTION = MOP$K_MSG_LOOP_REPLY;
	lr -> MOP$W_LP_RECEIPT = rec_number;

/*Fill in the data*/
	mop_fill((unsigned char*)mp->lp.loop_data,
			(char*)((long)lr+(sizeof(struct MOP$LOOPED_DATA)-1)),
			size-
			(sizeof(struct MOP$LOOPED_DATA)-1)-
			(sizeof(struct MOP$LOOP_FWD)-1),
			patt);
                                               
/*Return the pattern size*/
	return((size-
		(sizeof(struct MOP$LOOPED_DATA)-1)-
		(sizeof(struct MOP$LOOP_FWD)-1)));
}


/*+
 * ============================================================================
 * = mop_build_loop_data - builds the loopback data table.                    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will build the loopback data table.
 *
 * FORM OF CALL:
 *
 *	mop_build_loop_data(fp)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Inode of the file to be used.
 *
 * SIDE EFFECTS:
 *
 *      None
 *             
-*/

void mop_build_loop_data(struct INODE *ip)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
int i,j;

	trace("mop_build_loop_data\n",0);

/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)ip->misc;
	mp = (struct MB*)np->mbp;

/*Fill in the table with all the patterns*/
/*All zeros*/
	for(i=0 ;i<MOP$K_MAX_DATA_AVAIL_V3_NO_LEN; i++)
	    mp->lp.loop_data[MOP$K_ALL_ZEROS][i] = 0;

/*All ones*/
	for(i=0 ;i<MOP$K_MAX_DATA_AVAIL_V3_NO_LEN; i++)
	    mp->lp.loop_data[MOP$K_ALL_ONES][i] = 0xff;

/*All fives*/
	for(i=0 ;i<MOP$K_MAX_DATA_AVAIL_V3_NO_LEN; i++)
	    mp->lp.loop_data[MOP$K_ALL_FIVES][i] = 0x55;

/*All aa's*/
	for(i=0 ;i<MOP$K_MAX_DATA_AVAIL_V3_NO_LEN; i++)
	    mp->lp.loop_data[MOP$K_ALL_AAS][i] = 0xaa;

/*Incrementing*/
	for(i=0 ;i<MOP$K_MAX_DATA_AVAIL_V3_NO_LEN; i++)
	    mp->lp.loop_data[MOP$K_INCREMENTING][i] = i;

/*Decrementing*/
	for(i=0, j=MOP$K_MAX_DATA_AVAIL_V3_NO_LEN; 
				      i<MOP$K_MAX_DATA_AVAIL_V3_NO_LEN; i++,j--)
	    mp->lp.loop_data[MOP$K_DECREMENTING][i] = j;
}
