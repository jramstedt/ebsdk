/* file:	mop_driver.c
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
 *  Abstract:	MOP driver.
 *
 *	This module implements the network bootstrap and MOP "listener".
 *	The  module supports the functions of a "primitive" node as specified
 *	in the "NI Node Architecture Specification V2.0".  Both MOP V3.0 and
 *	T4.0 message formats are supported by the "listener".
 *
 *  AUTHORS:                                     
 *
 *	John DeNisco                                                     
 *
 *  CREATION DATE:
 *  
 *      20-Jul-1990
 *
 *	Modifications
 *
 *	mar     11-Dec-1998     Remove MIKASA from MOPRC define
 *				Add CORELLE to MINIMALMOP
 *
 *	tna     11-Nov-1998     Conditionalized for YUKONA
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
#include "cp$src:mop_def.h"			/*MOP definitions*/
#include "cp$src:mb_def.h"			/*MOP block defs*/
#include "cp$src:mop_counters.h"		/*MOP counters*/
#include "cp$src:ev_def.h"			/*Environment vars*/
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

/*Local constants*/                                            
#define RC_PASSWORD char password[8] = {0,0,0,0,0,0,0,0};

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
int mop_debug = 0;
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
    if ((mop_debug & 1) != 0) {
	if ((mop_debug & 0x100) == 0)
	    pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
	else
	    qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    }
}
static void _d2printf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((mop_debug & 2) != 0) {
	if ((mop_debug & 0x100) == 0)
	    pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
	else
	    qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    }
}
static void _d4printf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((mop_debug & 4) != 0) {
	if ((mop_debug & 0x100) == 0)
	    pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
	else
	    qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    }
}
static void _dqprintf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if (mop_debug)
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

/*Local constants*/                                            

#define MOPRC AVANTI||K2||TAKARA||APC_PLATFORM

#if CORELLE||SABLE||MEDULLA||TURBO||RAWHIDE||CORTEX || YUKONA||REGATTA||WILDFIRE
#define MINIMALMOP 1
#else
#define MINIMALMOP 0
#endif

/* Globals */
#if MEDULLA||CORTEX || YUKONA
struct SEMAPHORE	tulip_loopback;
#endif

extern int diagnostic_mode_flag;
extern struct FILE *fopen(char*,char*);
extern short int ndl_rcv_info_size(struct FILE *fp,int version,int inc_len);
extern void ndl_set_rcv (struct INODE *ip,unsigned short prot,
							unsigned char *mca,
							void (*rp)(struct FILE *fp,
							unsigned char *msg,
							unsigned char *source,
							unsigned short size,
							int version));
extern int cb_ref;			/*Call backs flag*/
extern int cb_fp[];
extern int null_procedure ();
extern unsigned int poweron_msec [2];
extern struct LOCK spl_kernel;
 
/*External mop references*/
extern int mopdl_close (struct FILE *fp);
extern int mopdl_open (struct FILE *fp,char *info,char *next,char *mode);
extern int mopdl_read (struct FILE *fp,int size,int number,unsigned char *c);
extern int mop_dump_load (struct FILE *fp);
extern int moplp_close (struct FILE *fp);
extern int moplp_open (struct FILE *fp,char *info,char *next,char *mode);
extern int moplp_write (struct FILE *fp,int item_size,int number,char *buf);
extern void mop_handle_dump_load(struct FILE *fp,unsigned char *dmpld,
							unsigned char *source,
							unsigned short size,
							int version);
extern void mop_handle_loopback(struct FILE *fp,unsigned char *lpbck,
							unsigned char *source,
							unsigned short size,
							int version);
extern int mop_loop_watchdog(char *port_file,int delay);

/*Routine definitions*/

unsigned short mop_build_info (struct FILE *fp, char *mp, unsigned char ver);
int mop_console(struct INODE *ip);
void mop_control_t(struct NI_GBL *np);
void mop_handle_console(struct FILE *fp,unsigned char *console,
							unsigned char *source,
							unsigned short size,
							int version);
int mop_handle_req_counters(struct FILE *fp,struct MSG_RCVD *mi);
int mop_handle_req_id(struct FILE *fp,struct MSG_RCVD *mi);
int mop_init1(struct INODE *ip ,unsigned char *sa);
void mop_read_env(struct NI_ENV *ev,char *name);
void mop_send_sys_id(struct FILE *fp,unsigned char *dest,unsigned char ver,
							unsigned short rec_num);
int mop_shutdown (char *file_name);
void mop_set_env(char *name);
void mop_set_timer (struct TIMERQ *tq);
int mop_unsolicited_id(struct INODE *ip);
mop_unsolicited_id_sub(struct INODE *ip,int v4,int r,int first);
int mop_wait_msg (struct TIMERQ *tq,int timeout_value);

#if !MODULAR
struct DDB mopdl_ddb = {
	"mopdl",			/* how this routine wants to be called	*/
	mopdl_read,		/* read routine				*/
	null_procedure,		/* write routine			*/
	mopdl_open,		/* open routine				*/
	mopdl_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	1,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};
#endif


/*+
 * ============================================================================
 * = mop_init1 - Initialization routine for the MOP driver.                    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will initialize the MOP driver. It will create
 *	the files that a mop application would use. It creates the MOP
 *	processes. This routine will also initialize the other data
 *   	structures needed by the mop driver.
 *
 * FORM OF CALL:
 *
 *	mop_init1(ip,sa);
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Name of the inode of the device.
 *	unsigned char *sa - Pointer to the station address.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mop_init1 (struct INODE *ip,unsigned char *sa)
{
struct NI_GBL *np;		/*Pointer to NI global database*/	
struct MB *mbp;			/*Pointer to the MOP block*/
struct NI_ENV *ev;		/*Pointer to the environment variables*/	
struct FILE *ndp;		/*Pointer to the network database file*/
unsigned char mca[6];		/*Storage for a multicast address*/
char name[20];			/*Process name,device name*/
int i;

	dprintf("mop_init1\n", p_args0);	

/*Get the pointers to the MOPinfo*/
	np = (struct NI_GBL*)ip->misc;
	np->mbp = (int*)malloc(sizeof(struct MB), mbp);
     	mbp = (struct MB*)np->mbp;

/*Get a pointer to the environment variables*/
	ev = (struct NI_ENV*)np->enp;

/*Set environment variable defaults*/
    	mop_read_env(ev,ip->name);

/*Get the port's short name*/
	strncpy(mbp->short_name,ip->name,4);

/*Get some MOP device specific junk*/
	if ( ip->dva->net_device )
	{
	/*Network type*/
	    mbp->net_type = MOP$K_XGEC;
	/*Datalink type*/
	    mbp->dl_type = MOP$K_DL_CSMA_CD;
	}


/*Get our address*/                         
	memcpy(mbp->sa,sa,6);

/*Clear the flags*/
	mbp->flags = 0;

/*Copy the loop file name*/
	sprintf(name,"lp_nodes_%4.4s",ip->name);
	strcpy(mbp->loop_file,name);
                
/*Initialize the network database*/
	strcpy (mbp->ndb_filename, "ni_db_");
	strncat(mbp->ndb_filename,ip->name,4);
	mbp->ndb_count = 0;

/*Open the network database file to create it*/
	ndp = fopen(mbp->ndb_filename,"w");
	if(!ndp)
	{
	     fclose(ndp);
	     return(msg_failure);
	}	

/*Since its now created just close it*/
	fclose(ndp);                        
        
/*Initialize the loopback stuff*/
/*Set message outstanding list to 0*/
	mbp->lp.mo = 0;	
	mbp->lp.mo_count = 0;	
	mbp->lp.kill_server = FALSE;	

/*Init the loop lists*/
	mbp->lp.rph.flink = &(mbp->lp.rph);
	mbp->lp.rph.blink = &(mbp->lp.rph);

/*Enable loopback multicast*/
	i = 0;
	mca[i++] = 0xcf; mca[i++] = 0x00; mca[i++] = 0x00;
	mca[i++] = 0x00; mca[i++] = 0x00; mca[i++] = 0x00; 
	ndl_set_rcv (ip,MOP$K_PROT_LOOPBACK,mca,mop_handle_loopback);

/* 
 * Initialize loop parameters:
 */
	sprintf(name, "mop_%s_rq_s", ip->name);
	krn$_seminit(&mbp->lp.rq_s, 1, name);
	mbp->lp.rq.flink = &mbp->lp.rq;
	mbp->lp.rq.blink = &mbp->lp.rq;
	mbp->lp.rq_elements = 0;

/*Initialize the dump/load stuff*/
	mbp->dllock.value = 0;	
	mbp->dllock.req_ipl = IPL_SYNC;	
	mbp->dllock.sav_ipl = 0;	
	mbp->dllock.attempts =	0;
	mbp->dllock.retries = 0;	
	mbp->dllock.owner = 0;	
	mbp->dllock.count = 0;	

/*Init some semaphores*/
	sprintf(name,"mop_%4.4s_dll",ip->name);
	krn$_seminit(&(mbp->dl.lock),1,name);

/*Init the list*/
	mbp->dl.qh.flink = &(mbp->dl.qh);
	mbp->dl.qh.blink = &(mbp->dl.qh);

/*Init the timers*/
	sprintf(name,"mop_%4.4s_avt",ip->name);
	krn$_seminit(&(mbp->dl.avt.sem),0,name);
	krn$_init_timer(&(mbp->dl.avt));
     	sprintf(name,"mop_%4.4s_mlt",ip->name);
	krn$_seminit(&(mbp->dl.mlt.sem),0,name);
	krn$_init_timer(&(mbp->dl.mlt));

/*Set the receive routine*/
/*Don't enable dump/load multicast*/
	for(i=0 ; i<6; i++)
	    mca[i] = 0;
	ndl_set_rcv (ip,MOP$K_PROT_DUMPLOAD,mca,mop_handle_dump_load);

/*Initialize some required console commands*/
/*Init some semaphores*/                    
	sprintf(name,"mop_%4.4s_cnl",ip->name);
	krn$_seminit(&(mbp->cn.lock),1,name);

/*Init the list*/
	mbp->cn.qh.flink = &(mbp->cn.qh);
	mbp->cn.qh.blink = &(mbp->cn.qh);
              
/*Start some processes*/

#if TURBO||RAWHIDE
    if (!diagnostic_mode_flag)
	goto nomopid;
#endif

	sprintf(name,"mopid_%4.4s",ip->name);
	krn$_create (mop_unsolicited_id,    /* address of process */
                     0,                     /* startup routine */ 
                     0,                     /* completion semaphore */
                     2,                     /* process priority */
                     1 << whoami(),         /* cpu affinity mask */
                     1024*4,                /* stack size */
                     name,                  /* process name */ 
                     "tt","r","tt","w","tt","w",ip);

nomopid:

#if MEDULLA||CORTEX || YUKONA
	/*
	 * Create the blocking semaphore to make sure that the Unsolicited
	 * MOP ID task here is not affected by the Tulip Loopback tests. 
	 */
	krn$_seminit (&tulip_loopback, 1, "tulip_loopback");
#endif

/*Enable console multicast address*/
	i = 0;
#if MIKASA || AVANTI || K2 || MTU || TAKARA || APC_PLATFORM
	mca[i++] = 0xff; mca[i++] = 0xff; mca[i++] = 0xff;
	mca[i++] = 0xff; mca[i++] = 0xff; mca[i++] = 0xff; 
#else
	mca[i++] = 0xab; mca[i++] = 0x00; mca[i++] = 0x00;
	mca[i++] = 0x02; mca[i++] = 0x00; mca[i++] = 0x00; 
#endif

/*Set the receive routine*/
	ndl_set_rcv (ip,MOP$K_PROT_CONSOLE,mca,mop_handle_console);

#if MOPRC
/*Initialize remote console functions*/
/*Clear the console user address*/
	for(i=0 ; i<6; i++)
	    mbp->rc.console_user[i] = 0;

/*Enable the remote console*/
     	mbp->flags |= MOP$M_RC_ENABLED;	

/*Init the Remote console buffers*/
/*Init the indices and counts*/
	mbp->rc.tx_index_in = mbp->rc.tx_index_out = 0;
	mbp->rc.tx_sent = 0; 
	mbp->rc.rx_index_in = mbp->rc.rx_index_out = 0;

/*Allocate the buffers*/
	mbp->rc.tbuf = (char*)malloc(MOP$K_CH_OUT_MAX, tbuf);
	mbp->rc.rbuf = (char*)malloc(MOP$K_CH_IN_MAX, rbuf);

/*Initialize some semaphores*/
	sprintf(name,"mop_%4.4s_txca",ip->name);
	krn$_seminit(&(mbp->rc.tx_hole),0,name);
	sprintf(name,"mop_%4.4s_rxca",ip->name);
	krn$_seminit(&(mbp->rc.rx_avail),0,name);

/*Initialize the remote console tt_driver*/


	moprctt_init (ip);

#endif

	return (msg_success);
}


#if !MODULAR
/*+
 * ============================================================================
 * = mop_init - Initialization routine for the mop driver database        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routines will initialize the mop driver database the DDB.
 *
 * FORM OF CALL:
 *
 *	mop_init (); 
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mop_init ()
{
struct INODE *inode;		/*Pointer an inode*/

	dprintf("mop_init\n", p_args0);

	allocinode( mopdl_ddb.name, 1, &inode );
	inode->dva = & mopdl_ddb;
	inode->attr = ATTR$M_READ;
	INODE_UNLOCK (inode);
	return (msg_success);


}
#endif
                                          
/*+
 * ============================================================================
 * = mop_shutdown - Shutdown routine for the MOP driver.                    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will shutdown the MOP driver. It will delete
 *	the files that a mop application would use. It kills the MOP
 *	processes. This routine will also delete the other data
 *   	structures used by the mop driver.
 *
 * FORM OF CALL:
 *
 *	mop_shutdown(file_name);
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	char *file_name - Name of the device to be used.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int mop_shutdown (char *file_name)
{
struct FILE *fp;		/*Pointer to the file*/ 
struct NI_GBL *np;		/*Pointer to NI global database*/	
struct MB *mbp;			/*Pointer to the MOP block*/
struct NI_ENV *ev;		/*Pointer to the environment variables*/	
char name[20];			/*Process name,device name*/
struct MSG_RCVD *mi;
struct PCB *pcb;
int empty;
                              
#if TURBO||RAWHIDE
/*Get a pointer to the file*/
	fp = fopen(file_name,"r+");
	if(!fp)
	    return(msg_failure);

/*Get the pointers to the MOPinfo*/
	np = (struct NI_GBL*)fp->ip->misc;
     	mbp = (struct MB*)np->mbp;

	mbp->dl.shutdown = 1;

    if (!diagnostic_mode_flag)
	goto nomopid_shutdown;

	sprintf(name,"mopid_%4.4s",fp->ip->name);
	while(1)
	{
	    if(krn$_kill_delayed(name))
		break;
	    krn$_sleep(1);
	}
	while(pcb = krn$_findpcb(mbp->mopid_pid))
	{
	    spinunlock(&spl_kernel);
	    if(pcb == getpcb())
		break;
	    krn$_sleep(1);
	}

nomopid_shutdown:

/* Release timers/semaphores: */
	krn$_release_timer(&(mbp->dl.mlt));
	krn$_semrelease(&(mbp->dl.mlt.sem));
	krn$_release_timer(&(mbp->dl.avt));
	krn$_semrelease(&(mbp->dl.avt.sem));

/* Wait for the console list to empty: */         
	while(1)
	{
	    krn$_wait(&(mbp->cn.lock));
	    empty = (mbp->cn.qh.flink == &mbp->cn.qh);
	    krn$_post(&(mbp->cn.lock));
	    if(empty)
		break;
	    else
		krn$_sleep(1);
	}

/* Release console semaphores: */
	krn$_semrelease(&(mbp->cn.lock));

/* Wait for the dump/load list to empty: */         
	while(1)
	{
	    krn$_wait(&(mbp->dl.lock));
	    empty = (mbp->dl.qh.flink == &mbp->dl.qh);
	    krn$_post(&(mbp->dl.lock));
	    if(empty)
		break;
	    else
		krn$_sleep(1);
	}

/* Release dump/load semaphores: */
	krn$_semrelease(&(mbp->dl.lock));

/* Shutdown loopback parameters: */
	krn$_semrelease(&mbp->lp.rq_s);
	while(mbp->lp.rq.flink != &mbp->lp.rq)
	{   
	    mi = remq(mbp->lp.rq.flink);
	    ndl_deallocate_msg(fp->ip, mi->msg, mi->version, MOP$K_DONT_INC_LENGTH);
	    free(mi, mi);
	}

       
/* Delete MOP block: */
	free(np->mbp, mbp);
	np->mbp = NULL;

	fclose(fp);

#endif  /* if TURBO */

	return (msg_success);        
}



/*+
 * ============================================================================
 * = mop_build_info - Builds the info portion of sys_is or req_prog.          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will build the information portion of the MOP SYSTEM_ID
 *	or the REQ_PROGRAM message.
 *
 * FORM OF CALL:
 *
 *	mop_build_info(fp,mp,ver)
 *
 * RETURNS:
 *
 *	unsigned short size - Size of the info portion of the message.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointer needed for access to the MOP block.
 *	char *mp - Pointer to the info portion of the message.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
unsigned short mop_build_info (struct FILE *fp, char *mp, unsigned char ver) {
struct NI_GBL *np;		/*Pointer to NI global database*/	
struct MB *mbp;			/*Pointer to the MOP block*/
unsigned short size,fun;	/*Size and function*/
int i;				/*Counter*/

	dprintf("mop_build_info\n", p_args0);

/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)fp->ip->misc;
	mbp = (struct MB*)np->mbp;

/*Start at 0*/
	size = 0;

        /* OTHER INFORMATION(*)=	
        /*	Further information to describe the system.  Consists of zero to more
        /*	entries, in any order.  Each info entry consists of:
        /*
        /*    2Bytes  1Byte   xBytes
        /*	+-------+-------+-------+
        /*	| INFO	| INFO	| INFO	|
        /*	| TYPE	|LENGTH	|VALUE	|
        /*	+-------+-------+-------+
        */

	*(short*)&(mp[size]) = MOP$K_SYSID_TYP_VERSION; /*Maintenance version*/
	size += 2;
	mp[size++] = MOP$K_SYSID_LEN_VERSION;  
	mp[size++] = MOP$K_VERSION_4;			/*Version 4 only*/
	*(short*)&(mp[size]) = 0;
	size += 2;
	*(short*)&(mp[size]) = MOP$K_SYSID_TYP_FUNCTIONS; /*Functions*/
	size += 2;
	mp[size++] = MOP$K_SYSID_LEN_FUNCTIONS;

	fun = MOP$K_SYSID_VAL_FUNCTIONS; 	/*Start with these functions*/
	if(mbp->flags & MOP$M_RC_ENABLED)	/*Console enabled*/
		fun |= MOP$M_FUNC_CONSOLE;
	if(mbp->flags & MOP$M_RC_RESERVED)	/*Console reserved*/
		fun |= MOP$M_FUNC_RESERVED;
	*(short*)&(mp[size]) = fun;		/*Write the function field*/
	size += 2;

/*Fill in the console fields if the console is enabled*/
	if(mbp->flags & MOP$M_RC_ENABLED)	
	{
	    *(short*)&(mp[size]) = MOP$K_SYSID_TYP_CON_USER;  /*Console user*/
	    size += 2;
	    mp[size++] = MOP$K_SYSID_LEN_CON_USER;
	    if(mbp->flags & MOP$M_RC_RESERVED)		      /*Console user*/
		for(i=0 ; i<6; i++)			      /*address*/
		    mp[size++] = mbp->rc.console_user[i];
	    else
		for(i=0 ; i<6; i++)
		    mp[size++] = 0;		 		
	    *(short*)&(mp[size]) = MOP$K_SYSID_TYP_CON_TIMER; /*Console Timer*/
	    size += 2;
	    mp[size++] = MOP$K_SYSID_LEN_CON_TIMER;
	    *(short*)&(mp[size]) = MOP$K_SYSID_VAL_CON_TIMER;
	    size += 2;
	    *(short*)&(mp[size]) = MOP$K_SYSID_TYP_CMD_SIZE; /*Console Command*/
	    size += 2;                                       /*Size*/
	    mp[size++] = MOP$K_SYSID_LEN_CMD_SIZE;
	    *(short*)&(mp[size]) = MOP$K_CH_IN_MAX;
	    size += 2;
	    *(short*)&(mp[size]) = MOP$K_SYSID_TYP_RSP_SIZE; /*Console response*/
	    size += 2;                                       /*Size*/
	    mp[size++] = MOP$K_SYSID_LEN_RSP_SIZE;
	    *(short*)&(mp[size]) = MOP$K_CH_OUT_MAX;
	    size += 2;
	}

/*Finished with console fields*/
	*(short*)&(mp[size]) = MOP$K_SYSID_TYP_E_HW_ADDR; /*Hardware address*/
	size += 2;
	mp[size++] = MOP$K_SYSID_LEN_E_HW_ADDR;
	for(i=0 ; i<6; i++)
	    mp[size++] = mbp->sa[i];		 		

#if 0
	if(mbp->dl_type == MOP$K_DL_FDDI)	/*Fill in FDDI field if we have to*/	
	{
	    ni_error(mbp->short_name,msg_mop_err_001);
	}
#endif


    	/*+	MOP$K_STATID_TYP_NODE_NAME
    	-*/
    	if ((fp->ip->dva->net_device == DDB$K_FDDI) && (ver == MOP$K_VERSION_4))
    	    {
            *(short*)&(mp[size]) = MOP$K_STATID_TYP_NODE_NAME; 
            size += 2;
            mp[size++]           = MOP$K_STATID_LEN_NODE_NAME;

            for(i=0 ; i<8; i++)
    	        mp[size++] = mbp->sid[i];		 		
    	}

	*(short*)&(mp[size]) = MOP$K_SYSID_TYP_COM_DVICE; /*Comm device*/
	size += 2;
	mp[size++] = MOP$K_SYSID_LEN_COM_DVICE;
	mp[size++] = mbp->net_type;
	*(short*)&(mp[size]) = MOP$K_SYSID_TYP_DATA_LINK; /*Datalink type*/
	size += 2;
	mp[size++] = MOP$K_SYSID_LEN_DATA_LINK;
	mp[size++] = mbp->dl_type;
	*(short*)&(mp[size]) = MOP$K_SYSID_TYP_BUF_SIZE; /*Datalink buffer*/
	size += 2;					 /*size*/
	mp[size++] = MOP$K_SYSID_LEN_BUF_SIZE;
/*Size including the protocol*/
	*(short*)&(mp[size]) =
		       ndl_rcv_info_size(fp,MOP$K_VERSION_4,MOP$K_INC_LENGTH)+2;
	size += 2;

/*Return the size*/
	return(size);

}


/*+
 * ============================================================================
 * = mop_set_timer - Set timer.                                               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will set and start the timer specified.
 *
 * FORM OF CALL:
 *
 *	mop_set_timer (tq); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct TIMERQ *tq - Pointer to the timer to be set and started.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_set_timer (struct TIMERQ *tq)
{                          

	dprintf("mop_set_timer\n", p_args0);

/*Set Timer alt flag*/
	tq->alt = FALSE;
}


/*+
 * ============================================================================
 * = mop_read_env - Read the mop environment variables.                       =
 * ============================================================================
 *                         
 * OVERVIEW:
 *
 *	This routine will read the mop environment variables.
 *
 * FORM OF CALL:
 *
 *	mop_read_env (ev,name); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct NI_ENV *ev - Pointer to the environment variables.
 *	char *name - Name of the unique part of the environment variable
 *			(Should be the name of the port).
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_read_env (struct NI_ENV *ev,char *name)
{
struct EVNODE *evp;		/*Pointer to the evnode*/
char var[EV_NAME_LEN];		/*General holder for a name*/

/*Get the memory for the evnode*/
	evp = (void *)malloc(sizeof(struct EVNODE), evp);

/*Read the mop environment variables*/
  	sprintf(var,"%4.4s_loop_size",name);
	if(ev_read(var,&evp,0) == msg_success)
	    ev-> loop_size = evp->value.integer;
	else
	{                  
	    ev-> loop_size = MOP$K_LOOP_SIZE;
	}
	sprintf(var,"%4.4s_loop_inc",name);
	if(ev_read(var,&evp,0) == msg_success)
	    ev-> loop_inc = evp->value.integer;
	else
	{
	    ev-> loop_inc = MOP$K_LOOP_INC;
  	}
	sprintf(var,"%4.4s_loop_patt",name);
	if(ev_read(var,&evp,0) == msg_success)
	    ev-> loop_patt = evp->value.integer;
	else
	{
	    ev-> loop_patt = MOP$K_ALL;
	}
	sprintf(var,"%4.4s_loop_count",name);
	if(ev_read(var,&evp,0) == msg_success)
  	    ev-> loop_count = evp->value.integer;
	else
    	{
	    ev-> loop_count = MOP$K_LOOP_COUNT;
	}
	sprintf(var,"%4.4s_lp_msg_node",name);
	if(ev_read(var,&evp,0) == msg_success)
  	    ev-> lp_msg_node = evp->value.integer;
	else
    	{
	    ev-> lp_msg_node = MOP$K_LP_MSG_NODE;
	}

/*Free the evnode memory*/

	free(evp, evp);

	dprintf("Class environment vars:\n", p_args0);
	dprintf("loop_size: %08x loop_inc: %08x\n", p_args2(ev->loop_size, ev->loop_inc));
	dprintf("loop_patt: %08x lp_msg_node: %08x\n", p_args2(ev->loop_patt, ev->lp_msg_node));
	dprintf("loop_count: %08x\n", p_args1(ev->loop_count));
}

/*+
 * ============================================================================
 * = mop_set_env - Sets MOP environment variables.                            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will create and set the MOP environment variables.
 *
 * FORM OF CALL:
 *
 *	mop_set_env (name); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	char *name - Pointer to a name of the port.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_set_env (char *name)
{
char var[EV_NAME_LEN];			/*General holder for a name*/

#if !MODULAR
/*Write the environment variables with defaults*/
	sprintf(var,"%4.4s_loop_size",name);
	ev_write(var,MOP$K_LOOP_SIZE,EV$K_INTEGER);
	sprintf(var,"%4.4s_loop_inc",name);
	ev_write(var,MOP$K_LOOP_INC,EV$K_INTEGER);
	sprintf(var,"%4.4s_loop_patt",name);
	ev_write(var,MOP$K_ALL,EV$K_INTEGER);
	sprintf(var,"%4.4s_loop_count",name);
	ev_write(var,MOP$K_LOOP_COUNT,EV$K_INTEGER);
	sprintf(var,"%4.4s_lp_msg_node",name);
	ev_write(var,MOP$K_LP_MSG_NODE,EV$K_INTEGER);
#endif
#if !MODULAR
/*Let the Inet driver do some EVs too*/
	inet_set_env (name);
#endif
}


/*+
 * ============================================================================
 * = mop_wait_msg - Wait for a message or timeout.                            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will wait for a message or timeout.
 *	msg_success is returned if we got the interrupt, msg_failure
 *	returned if we timed out. The mop_set_timer routine
 *	should be used to set up the timer before this is
 *	routine is called.
 *
 * FORM OF CALL:
 *
 *	 mop_wait_msg (tq,timeout_value); 
 *
 * RETURNS:
 *
 *	int Status - MOP$K_SUCCESS,MOP$K_FAILURE or MOP$K_KILLED.
 *
 * ARGUMENTS:
 *
 *	struct TIMERQ *tq - Pointer to the timer to be set and started.
 *	int timeout_value - Value at which the timer will timeout.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mop_wait_msg (struct TIMERQ *tq,int timeout_value)
{
int i;

	dprintf("mop_wait_msg\n", p_args0);

/*Loop so we can check kill pending periodically*/
	for (i=0; i<(MOP$K_BOOT_POLL+1); i++)
	{
	/*Set the semaphore count and start the timer*/
	    tq->sem.count = 0;
	    krn$_start_timer(tq,timeout_value/MOP$K_BOOT_POLL);

	/*Wait for the semaphore or timer*/
	    krn$_wait(&(tq->sem));
	    if(tq->alt)
	    {
    		krn$_stop_timer(tq);
		return(MOP$K_SUCCESS);
	    }
	    else if(killpending())
 		return(MOP$K_KILLED);
	}

/*Return a timeout*/
	return(MOP$K_FAILURE);
}


/*+
 * ============================================================================
 * = mop_control_t - MOP's control T routine.                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is used to display MOP status (counters etc.) in real time.
 *
 * FORM OF CALL:
 *
 *	mop_control_t (np); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct NI_GBL *np - Pointer to the NI global database.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

void mop_control_t(struct NI_GBL *np)
{
struct MOP_PB	*pb;		/*Pointer to the port block*/	
struct MB 	*mb;		/*Pointer to the MOP block*/
struct MOP$_V4_COUNTERS *mc;	/*Pointer to the MOP counters*/	

/*Get some pointers*/
	pb = (struct EZ_PB*)np->pbp;
	mc = (struct MOP$_V4_COUNTERS*)np->mcp;
	mb = (struct MB*)np->mbp;

/*Display some status*/
	printf(msg_ez_inf_mop_blk);
	printf(msg_ez_inf_net_lst,mb->ndb_count);
/*Display the mop counters*/
	print_mop_counters(mc);

}


/*+
 * ============================================================================
 * = mop_unsolicited_id - MOP unsolicited ID process.                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is actually a process started when the driver was opened.
 *	This process will send an unsolicited ID message every 5 minutes.
 *	The first is sent in V4 format the next in V3 format, and so in an
 *	alternating format.
 *
 * FORM OF CALL:
 *
 *	mop_unsolicited_id(ip)
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Pointer to the Device's inode.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mop_unsolicited_id(struct INODE *ip)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
int r,m,tmo,index;			/*Random number generator values*/
unsigned short seed;			/*Random number generator seed*/
struct FILE *fp;			/*Pointer to the file being used*/

	dprintf("mop_unsolicited_id\n", p_args0);

/*Get a file pointer for this device*/
	fp = fopen(ip->name,"r+");
	if(!fp)
	    return(msg_failure);

/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Index into the LCG table*/
	index = MOP$K_LCG_INDEX;

/*Maximum generated range*/
	m = 1<<index;

/*Random number seed*/
	seed = *(unsigned short*)&(mp->sa[4]);
	r = seed & (m-1);

	mop_unsolicited_id_sub(ip,1,r,1);
	fclose(fp);
}

void mop_unsolicited_id_sub(struct INODE *ip,int v4,int r,int first)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
unsigned char dest[6];			/*Destinaton address*/
int m,tmo,index;			/*Random number generator values*/
struct FILE *fp;			/*Pointer to the file being used*/
char name[20];				/*Process name,device name*/

/*Get a file pointer for this device*/
	fp = fopen(ip->name,"r+");
	if(!fp)
	    return;

/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)ip->misc;
	mp = (struct MB*)np->mbp;

/*Use the multicast destination*/
	memset(dest, 0, 6);

/*Index into the LCG table*/
	index = MOP$K_LCG_INDEX;

/*Maximum generated range*/
	m = 1<<index;

	if(first)
	{
	    tmo = 10000;
	}
	else
	{
	    if(v4)
	    {
	    /*Send the system ID in version 4 format*/
		mop_send_sys_id(fp,dest,MOP$K_VERSION_4,MOP$K_UNSOLICITED_REC_NUM);
	    }
	    else
	    {
	    /*Send the system ID in version 3 format*/
		mop_send_sys_id(fp,dest,MOP$K_VERSION_3,MOP$K_UNSOLICITED_REC_NUM);
	    }

	/*Get some randomness*/
	    r = random(r,index);
	    tmo = (r*((MOP$K_ID_RAN*2*1000)/(m-1)))/1000-MOP$K_ID_RAN;
	    tmo += MOP$K_5_MINUTES;
	}

/*Create a new delayed process*/
	mp->mopid_pid = getpcb()->pcb$l_pid;
	sprintf(name,"mopid_%4.4s",ip->name);
	krn$_create_delayed (
		tmo,
		1,
		mop_unsolicited_id_sub,
		0,
		0,
		2,
		1 << whoami(),
		4096,
		name,
		ip,
		v4 ^ 1,
		r,
		0);
}


/*+
 * ============================================================================
 * = mop_send_sys_id - Sends the system ID message.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will send the system ID message.
 *
 * FORM OF CALL:
 *
 *	mop_send_sys_id(fp,dest,ver,rec_num);
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointer needed for access to the MOP block.
 *	char *dest - Pointer to 6 bytes containing the destination address.
 *		     If its a null address we use the CONSOLE multicast
 *		     address.
 *
 *	unsigned char ver - Specifies whether we want to use version 3 or 4.
 *					MOP$K_VERSION_3 = versions 1-3.
 *					MOP$K_VERSION_4 = version 4.
 *
 *	unsigned char rec_num - The receipt number.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mop_send_sys_id(struct FILE *fp,unsigned char *dest,unsigned char ver,
							unsigned short rec_num)
{
struct NI_GBL *np;		/*Pointer to NI global database*/	
struct MB *mp;			/*Pointer to the MOP block*/
char *p;			/*General pointer to the packet*/
unsigned short size,is;		/*Size of the packets*/
struct MOP$SYSTEM_ID *id;	/*Pointer to the start of the ID pkt*/
int i,sum;

	dprintf("mop_send_sys_id\n", p_args0);

/*Don't send while in callback mode*/
	if(cb_ref)
	    return;

#if MEDULLA||CORTEX || YUKONA
	/*
	 * Wait for Tulip Loopback tests to complete (NIIL_DIAG.C)
	 */
	krn$_wait (&tulip_loopback);
#endif


/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Don't send if flag is set not to. */
	if (mp->no_mop_unsolicited_id)
	    return;

/*Check for a null address*/
	sum = 0;
	for(i=0; i<6; i++)
	    sum += (int)dest[i];

/*If it is a null address we'll use the multicast address*/
	if(!sum)
	{
	/*The MOP CONSOLE multicast*/
	    i = 0;
	    dest[i++] = 0xab; dest[i++] = 0x00; dest[i++] = 0x00;
	    dest[i++] = 0x02; dest[i++] = 0x00; dest[i++] = 0x00;
	}

/*Allocate the message*/
	id = ndl_allocate_msg(fp->ip,ver,MOP$K_INC_LENGTH);

/*Fill in the fix message part of the message*/
	id->MOP$B_SID_CODE = MOP$K_MSG_SYSTEM_ID;	
	id->MOP$B_SID_RESERVED = 0;
	id->MOP$W_SID_RECEIPT_NUM = rec_num;

/*Get packet pointer and current size*/
	p = (char*)id;
	size = sizeof(struct MOP$SYSTEM_ID);

/*Fill in the info fields*/
	is = mop_build_info(fp, &(p[size]), ver);

/*Get the new size*/
	size += is;

/*Send the message*/
	ndl_send(fp,ver,(char*)id,size,MOP$K_INC_LENGTH,dest,
							MOP$K_PROT_CONSOLE);
#if MEDULLA||CORTEX || YUKONA
	/*
	 * Signal the Tulip loopback test that we have finished. 
	 */
	krn$_post (&tulip_loopback);
#endif

}


/*+
 * ============================================================================
 * = mop_handle_console - Handle MOP console messages.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle MOP console messages. This routine excepts
 *	a pointer to the console portion of the message (Above the protocol).
 *	This is indpendent of V3 or V4 packets.
 *
 * FORM OF CALL:
 *
 *	mop_handle_console(fp,console,source,size,version)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file opened. Returns 0 on an error.
 *
 *	char *console - Pointer to the console portion of the MOP message.
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

void mop_handle_console(struct FILE *fp,unsigned char *console,
							unsigned char *source,
							unsigned short size,
							int version)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
struct MSG_RCVD *mi;			/*Message info*/
int inc_len;				/*Counter,Include length flag*/
char name[20];				/*Process name,device name*/
int first;				/*First entry on queue*/

	dprintf("mop_handle_console\n", p_args0);

/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*Take care of the size and the pointer*/
	if(version == MOP$K_VERSION_3)
	{
	/*First the size*/
	    size = *((unsigned short*)console);
	/*Then the pointer*/
	    console = (unsigned char*)((long)console + 2);
	/*Include length flag*/
	    inc_len = MOP$K_INC_LENGTH;
	}
	else
	/*Don't include length flag*/
	    inc_len = MOP$K_DONT_INC_LENGTH;

#if DEBUG
	if (mop_debug & 4)
	    pprint_pkt(console,size);
#endif

/*Get an information packet*/
	mi = (struct MSG_RCVD*)malloc(sizeof(struct MSG_RCVD), mi);

/*Fill it in*/
	mi->msg = console;
	mi->size = size;
	memcpy(mi->source,source,6);
	mi-> version = version;
	mi-> inc_len = inc_len;

/*Insert the packet on the dump/load list*/
	krn$_wait(&(mp->cn.lock));
	first = (mp->cn.qh.flink == &mp->cn.qh);
	insq(mi,mp->cn.qh.blink);
	krn$_post(&(mp->cn.lock));

/*Wakeup the dump/load process*/
	if(first)
	{
	    sprintf(name,"mopcn_%4.4s",fp->ip->name);
	    krn$_create (mop_console,     /* address of process */ 
                	 0,               /* startup routine */ 
                	 0,               /* completion semaphore */
                	 3,               /* process priority */
                         -1,              /* cpu affinity mask */
                         1024*4,          /* stack size */  
                         name,            /* process name */   
		         "tt","r","tt","w","tt","w",fp->ip);
	}
}


/*+
 * ============================================================================
 * = mop_console - MOP console process.                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is actually a process started when the driver was opened.
 *	This process is woken up when a console message is received. When
 *	woken up the process removes the message from the console queue
 *	and handles it accordingly.
 *
 * FORM OF CALL:
 *
 *	mop_console(ip)
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Pointer to the Device's inode.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mop_console(struct INODE *ip)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MB *mp;				/*Pointer to the MOP block*/
struct MSG_RCVD *mi;			/*Message info*/
struct FILE *fp;			/*Pointer to the file being used*/
int last;				/*Last entry on queue*/

	dprintf("mop_console\n", p_args0);

/*Get a file pointer for this device*/
/*This file stays open forever*/
	fp = fopen(ip->name,"r+");
	if(!fp)
	    return(msg_failure);

/*Get the pointer to the MOP block*/
	np = (struct NI_GBL*)fp->ip->misc;
	mp = (struct MB*)np->mbp;

/*This will run forever*/
	while(1)
	{
	/*Get the message*/
	    mi = mp->cn.qh.flink;

	/*Handle the correct message*/
	/*The code is the first byte of the message*/
	    if(!mp->dl.shutdown)
	    {
		switch(*(mi->msg))
		{
		    case MOP$K_MSG_REQUEST_ID:
			mop_handle_req_id(fp,mi);
			break;
		    case MOP$K_MSG_REQ_COUNTERS:
			mop_handle_req_counters(fp,mi);
			break;
#if !MINIMALMOP
       		    case MOP$K_MSG_COUNTERS:
			ni_error(mp->short_name,msg_mop_inf_cntrs);
			break;
		    case MOP$K_MSG_SYSTEM_ID:
			mop_handle_sys_id(fp,mi);
			break;
		    case MOP$K_MSG_RESERVE_CONSOLE:
			mop_handle_res_console(fp,mi);
			break;
		    case MOP$K_MSG_RELEASE_CONSOLE:
			mop_handle_rel_console(fp,mi);
			break;
		    case MOP$K_MSG_CON_CMD_N_POLL:
			mop_handle_con_cmd_n_poll(fp,mi);
			break;
		    case MOP$K_MSG_BOOT:
                	break;
#endif
		}
	    }

	/*Deallocate the message*/
	    ndl_deallocate_msg(fp->ip,mi->msg,mi->version,mi->inc_len);

	/*Dequeue the information packet*/
	    krn$_wait(&(mp->cn.lock));
	    remq(mi);
	    last = (mp->cn.qh.flink == &mp->cn.qh);
	    krn$_post(&(mp->cn.lock));

	/*Free the information packet*/
	    free(mi,mi);

	/*Quit if we've handled them all*/
	    if(last)
		break;
	}
}


/*+
 * ============================================================================
 * = mop_handle_req_id - Handles the request ID message.                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle the request ID message.
 *
 * FORM OF CALL:
 *
 *	mop_handle_req_id(fp,mi);
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

int mop_handle_req_id(struct FILE *fp,struct MSG_RCVD *mi)
{
struct MOP$REQUEST_ID *ri;		/*Request counters packet pointer*/

	dprintf("mop_handle_req_id\n", p_args0);

#if DEBUG
	if (mop_debug & 4)
	    pprint_pkt(mi->msg,mi->size);
#endif

/*Get the correct typecast for the received message*/
	ri = (struct MOP$REQUEST_ID*)(mi->msg);

/*Send the system ID*/
	mop_send_sys_id(fp,mi->source,mi->version,ri->MOP$W_RID_RECEIPT_NUM);

/*Return success*/
	return(msg_success);
}


/*+
 * ============================================================================
 * = mop_handle_req_counters - Handles the request counters message.          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle the request counters message.
 *
 * FORM OF CALL:
 *
 *	mop_handle_req_counters(fp,mi);
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

int mop_handle_req_counters(struct FILE *fp,struct MSG_RCVD *mi)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct MOP$_V4_COUNTERS *mc;		/*Pointer to the MOP counters*/	
struct MOP$REQ_COUNTERS *rc;		/*Request counters packet pointer*/
struct MOP$COUNTERS *mcp;		/*MOP counter packet pointer*/
struct MOP$_V3_COUNTERS *mv3;		/*MOP version 3 counters*/
struct MOP$_V4_COUNTERS *mv4;		/*MOP version 4 counters*/
U_INT_16 fc;				/*Failure count*/
UINT ct,tz,tz4;				/*Current time*/

	dprintf("mop_handle_req_counters\n", p_args0);

#if DEBUG
	if (mop_debug & 4)
	    pprint_pkt(mi->msg,mi->size);
#endif

/*Get the pointer to the MOP counters*/
	np = (struct NI_GBL*)fp->ip->misc;
	mc = (struct MOP$_V4_COUNTERS*)np->mcp;

/*Get the correct typecast fro the received message*/
	rc = (struct MOP$REQ_COUNTERS*)(mi->msg);

/*Get the current time*/
	ct = *(UINT*)poweron_msec;

/*Get the time since zeroed*/
	tz = (ct - *(UINT*)(&(mc->MOP_V4CNT_TIME_RELATIVE.l)))
							/MOP$K_1S_BY_1MS;   
	tz4 = (ct - *(UINT*)(&(mc->MOP_V4CNT_TIME_RELATIVE.l)))
							*MOP$K_1MS_BY_100NS;   
/*Check for the version*/
	if(mi->version == MOP$K_VERSION_4)
	{
	/*Get a version 4 message*/
	    mcp = ndl_allocate_msg(fp->ip,MOP$K_VERSION_4,MOP$K_INC_LENGTH);
	/*Fill in gneric portion*/
	    mcp->MOP$B_CNT_CODE = MOP$K_MSG_COUNTERS;
	    mcp->MOP$W_CNT_RECEIPT_NUM = rc->MOP$W_RCT_RECEIPT_NUM;

	/*Point to the mop counter block*/
	    mv4 = (struct MOP$_V4_COUNTERS*)((long)mcp +
						   sizeof(struct MOP$COUNTERS));

	/*Fill in the packet*/
	    memcpy(mv4,mc,sizeof(struct MOP$_V4_COUNTERS));

	/*Fill in the relative time*/
	    memcpy((unsigned char*)(&(mv4->MOP_V4CNT_TIME_RELATIVE.l)),
		   (unsigned char*)(&tz4),8);

	/*Send the message*/
	    ndl_send(fp,MOP$K_VERSION_4,(char*)mcp,
					sizeof(struct MOP$COUNTERS)+ 
					sizeof(struct MOP$_V4_COUNTERS),
					MOP$K_INC_LENGTH,
					mi->source,
					MOP$K_PROT_CONSOLE);
	}
	else
	{
	/*Get a version 3 message*/
	    mcp = ndl_allocate_msg(fp->ip,MOP$K_VERSION_3,MOP$K_INC_LENGTH);

	/*Fill in gneric portion*/
	    mcp->MOP$B_CNT_CODE = MOP$K_MSG_COUNTERS;
	    mcp->MOP$W_CNT_RECEIPT_NUM = rc->MOP$W_RCT_RECEIPT_NUM;

	/*Point to the mop counter block*/
	    mv3 = (struct MOP$_V3_COUNTERS*)((long)mcp +
						   sizeof(struct MOP$COUNTERS));

	/*Fill in the packet latching if there's an overflow*/
	    if(tz > MOP$K_MAX_16)
		mv3->MOP_V3CNT_SECONDS_SINCE_ZERO = MOP$K_MAX_16;
	    else
		mv3->MOP_V3CNT_SECONDS_SINCE_ZERO = (U_INT_16)tz;

	    if(mc->MOP_V4CNT_RX_BYTES.l > MOP$K_MAX_32)
		mv3->MOP_V3CNT_RX_BYTES = MOP$K_MAX_32;
	    else
		mv3->MOP_V3CNT_RX_BYTES = (U_INT_32)mc->MOP_V4CNT_RX_BYTES.l;

	    if(mc->MOP_V4CNT_TX_BYTES.l > MOP$K_MAX_32)
		mv3->MOP_V3CNT_TX_BYTES = MOP$K_MAX_32;
	    else
		mv3->MOP_V3CNT_TX_BYTES = (U_INT_32)mc->MOP_V4CNT_TX_BYTES.l;

	    if(mc->MOP_V4CNT_RX_FRAMES.l > MOP$K_MAX_32)
		mv3->MOP_V3CNT_RX_FRAMES = MOP$K_MAX_32;
	    else
		mv3->MOP_V3CNT_RX_FRAMES = (U_INT_32)mc->MOP_V4CNT_RX_FRAMES.l;

	    if(mc->MOP_V4CNT_TX_FRAMES.l > MOP$K_MAX_32)
		mv3->MOP_V3CNT_TX_FRAMES = MOP$K_MAX_32;
	    else
		mv3->MOP_V3CNT_TX_FRAMES = (U_INT_32)mc->MOP_V4CNT_TX_FRAMES.l;

	    if(mc->MOP_V4CNT_RX_MCAST_BYTES.l > MOP$K_MAX_32)
		mv3->MOP_V3CNT_RX_MCAST_BYTES = MOP$K_MAX_32;
	    else
		mv3->MOP_V3CNT_RX_MCAST_BYTES = 
				(U_INT_32)mc->MOP_V4CNT_RX_MCAST_BYTES.l;

	    if(mc->MOP_V4CNT_RX_MCAST_FRAMES.l > MOP$K_MAX_32)
		mv3->MOP_V3CNT_RX_MCAST_FRAMES = MOP$K_MAX_32;
	    else
		mv3->MOP_V3CNT_RX_MCAST_FRAMES =
				(U_INT_32)mc->MOP_V4CNT_RX_MCAST_FRAMES.l;

	    if(mc->MOP_V4CNT_TX_INIT_DEFERRED.l > MOP$K_MAX_32)
		mv3->MOP_V3CNT_TX_INIT_DEFERRED = MOP$K_MAX_32;
	    else
		mv3->MOP_V3CNT_TX_INIT_DEFERRED =
				(U_INT_32)mc->MOP_V4CNT_TX_INIT_DEFERRED.l;

	    if(mc->MOP_V4CNT_TX_ONE_COLLISION.l > MOP$K_MAX_32)
		mv3->MOP_V3CNT_TX_ONE_COLLISION = MOP$K_MAX_32;
	    else
		mv3->MOP_V3CNT_TX_ONE_COLLISION = 
				(U_INT_32)mc->MOP_V4CNT_TX_ONE_COLLISION.l;

	    if(mc->MOP_V4CNT_TX_MULTI_COLLISION.l > MOP$K_MAX_32)
		mv3->MOP_V3CNT_TX_MULTI_COLLISION = MOP$K_MAX_32;
	    else
		mv3->MOP_V3CNT_TX_MULTI_COLLISION = 
				(U_INT_32)mc->MOP_V4CNT_TX_MULTI_COLLISION.l;

	/*Do the transmit failure count*/
	    fc = (U_INT_16)(mc->MOP_V4CNT_TX_FAIL_EXCESS_COLLS.l +
					mc->MOP_V4CNT_TX_FAIL_CARRIER_CHECK.l +
					mc->MOP_V4CNT_TX_FAIL_SHRT_CIRCUIT.l + 
					mc->MOP_V4CNT_TX_FAIL_OPEN_CIRCUIT.l + 
					mc->MOP_V4CNT_TX_FAIL_LONG_FRAME.l + 
					mc->MOP_V4CNT_TX_FAIL_REMOTE_DEFER.l); 
	    if(fc > MOP$K_MAX_16)
		mv3->MOP_V3CNT_TXFAILURES = MOP$K_MAX_16;
	    else
		mv3->MOP_V3CNT_TXFAILURES = fc;

	/*Now do the bitmap*/
	    mv3->MOP_V3CNT_TXFAIL_BITMAP = 0;
	    if(mc->MOP_V4CNT_TX_FAIL_EXCESS_COLLS.l)
		mv3->MOP_V3CNT_TXFAIL_BITMAP |= MOP$M_V3BIT_TXFAIL_EXCESS_COLLS;
	    if(mc->MOP_V4CNT_TX_FAIL_CARRIER_CHECK.l)
		mv3->MOP_V3CNT_TXFAIL_BITMAP |= MOP$M_V3BIT_TXFAIL_CARRIER_CHECK;
	    if(mc->MOP_V4CNT_TX_FAIL_SHRT_CIRCUIT.l)
		mv3->MOP_V3CNT_TXFAIL_BITMAP |= MOP$M_V3BIT_TXFAIL_SHRT_CIRCUIT;
	    if(mc->MOP_V4CNT_TX_FAIL_OPEN_CIRCUIT.l)
		mv3->MOP_V3CNT_TXFAIL_BITMAP |= MOP$M_V3BIT_TXFAIL_OPEN_CIRCUIT;
	    if(mc->MOP_V4CNT_TX_FAIL_LONG_FRAME.l)
		mv3->MOP_V3CNT_TXFAIL_BITMAP |= MOP$M_V3BIT_TXFAIL_LONG_FRAME;
	    if(mc->MOP_V4CNT_TX_FAIL_REMOTE_DEFER.l)
		mv3->MOP_V3CNT_TXFAIL_BITMAP |= MOP$M_V3BIT_TXFAIL_REMOTE_DEFER;

	/*Do the receive failure count*/
	    fc = (U_INT_16)(mc->MOP_V4CNT_RX_FAIL_BLOCK_CHECK.l +
					mc->MOP_V4CNT_RX_FAIL_FRAMING_ERR.l +
					mc->MOP_V4CNT_RX_FAIL_LONG_FRAME.l);
	    if(fc > MOP$K_MAX_16)
		mv3->MOP_V3CNT_RXFAILURES = MOP$K_MAX_16;
	    else
		mv3->MOP_V3CNT_RXFAILURES = fc;

	/*Now do the bitmap*/
	    mv3->MOP_V3CNT_RXFAIL_BITMAP = 0;
	    if(mc->MOP_V4CNT_RX_FAIL_BLOCK_CHECK.l)
		mv3->MOP_V3CNT_RXFAIL_BITMAP |= MOP$M_V3BIT_RXFAIL_BLOCK_CHECK;
	    if(mc->MOP_V4CNT_RX_FAIL_FRAMING_ERR.l)
		mv3->MOP_V3CNT_RXFAIL_BITMAP |= MOP$M_V3BIT_RXFAIL_FRAMING_ERR;
	    if(mc->MOP_V4CNT_RX_FAIL_LONG_FRAME.l)
		mv3->MOP_V3CNT_RXFAIL_BITMAP |= MOP$M_V3BIT_RXFAIL_LONG_FRAME;

	/*Fill in the rest*/
	    if(mc->MOP_V4CNT_UNKNOWN_DESTINATION.l > MOP$K_MAX_16)
		mv3->MOP_V3CNT_UNKNOWN_DESTINATION = MOP$K_MAX_16;
	    else
		mv3->MOP_V3CNT_UNKNOWN_DESTINATION = 
				(U_INT_16)mc->MOP_V4CNT_UNKNOWN_DESTINATION.l;

	    if(mc->MOP_V4CNT_DATA_OVERRUN.l > MOP$K_MAX_16)
		mv3->MOP_V3CNT_DATA_OVERRUN = MOP$K_MAX_16;
	    else
		mv3->MOP_V3CNT_DATA_OVERRUN =
					(U_INT_16)mc->MOP_V4CNT_DATA_OVERRUN.l;

	    if(mc->MOP_V4CNT_NO_SYSTEM_BUFFER.l > MOP$K_MAX_16)
		mv3->MOP_V3CNT_NO_SYSTEM_BUFFER = MOP$K_MAX_16;
	    else
		mv3->MOP_V3CNT_NO_SYSTEM_BUFFER =
				(U_INT_16)mc->MOP_V4CNT_NO_SYSTEM_BUFFER.l;

	    if(mc->MOP_V4CNT_NO_USER_BUFFER.l > MOP$K_MAX_16)
		mv3->MOP_V3CNT_NO_USER_BUFFER = MOP$K_MAX_16;
	    else
		mv3->MOP_V3CNT_NO_USER_BUFFER =
				(U_INT_16)mc->MOP_V4CNT_NO_USER_BUFFER.l;

	/*Send the message*/
	    ndl_send(fp,MOP$K_VERSION_3,(char*)mcp,
					sizeof(struct MOP$COUNTERS)+ 
					sizeof(struct MOP$_V3_COUNTERS),
					MOP$K_INC_LENGTH,
					mi->source,
					MOP$K_PROT_CONSOLE);
	}

/*Return success*/
	return(msg_success);
}
