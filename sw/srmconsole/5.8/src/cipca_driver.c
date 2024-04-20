/*
 * cipca_driver.c
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
 * ABSTRACT:	CIPCA port driver
 *
 *	A PCI to CI adapter.  It operates with
 *	the MSCP class driver.  It currently (15-Jan-1997) is 
 *	available on the  Sable, Gamma, Lynx, Turbo, and Rawhide
 *	platforms.  This is both a modular/startup-shutdown 
 *	implementation and a monolithic/always-running implementation.
 *                      
 * OVERVIEW:
 *
 *	The CIPCA port driver is composed of a set of files:	
 *                       CIPCA_DRIVER.C
 *                       CIPCA_COMMON.C
 *                       CIPCA_RESPONSE.C
 *                       CIPCA_SUPPORT.C
 *                       CIPCA_IO.C
 *                       CIPCA_DEF.H
 *                       CI_CIPCA_DEF.H
 *                       CIPCA_SA_PPD_DEF.H
 *
 *              The CIPCA support is dependent upon:
 *                       MSCP.C
 *                       MSCP_LASER.C
 *                       NPORT_Q.C
 *                       NPORT_DEF.H
 *                       NPORT_Q_DEF.H
 *
 *              Future enhancements would include eliminateing the 
 *              CIPCA specific SCS routines and using the commone
 *              SCS routines found in SCS.C.
 *                       
 *
 * Author:	Dennis Mazur
 *
 * Modifications:
 *
 */

#define DEBUG 0
#define DEBUG_QUEUE  0
#define DEBUG_MALLOC 0

#define DFQE_SIZE 512
#define HWRPB$_PAGESIZE 0x2000
#define PAGE_OFFSET 13

#define BASE0 0x10
#define BASE1 0x14
#define BASE2 0x18
#define BASE3 0x1c
#define BASE4 0x20
#define BASE5 0x24

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:mem_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:nport_q_def.h"
#include "cp$src:nport_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:cipca_def.h"
#include "cp$src:ci_cipca_def.h"
#include "cp$src:cipca_sa_ppd_def.h"
#include "cp$src:server_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:cb_def.h"
#include "cp$src:print_var.h"
#include "cp$src:ctype.h"
#include "cp$src:cipca_debug_rtns.h"
#include "cp$inc:platform_io.h"
#include "cp$inc:prototypes.h"
#include "cp$inc:kernel_entry.h"

#if ( RAWHIDE || TURBO )
#define $DMA_BASE()  0
#else
#define $DMA_BASE() io_get_window_base( pb )
#endif


/* HACK */
struct cixcd_blk *cipca_blk_ptr;
struct ci_pb *cipca_pb;
struct device *cipca_d;
#if DEBUG
int cipca_cnt = 0;
struct ci_pb *ci_pbs[10];
#endif

#if TURBO || RAWHIDE
extern struct window_head config;
#endif

char *ctl_id(int num, char *id);

int cixcd_incarnation = 0;
int cipca_poll_delay = 200;
extern int diagnostic_mode_flag;

extern struct FILE *el_fp;
extern struct QUEUE pollq;

extern struct pb **pbs;
extern int num_pbs;
extern int _align (QUADWORD) cbip;
#if DEBUG
extern void ci_sa_ppd_dqi_off(struct cipca_sb *sb);
extern char *poll_active_txt[];
#endif

int ci_debug_flag = 0;
#if DEBUG
#define dqprintf _dqprintf
#define dprintf(fmt, args)	\
    _dprintf(fmt, args)
#define d2printf(fmt, args)	\
    _d2printf(fmt, args)
#else
#define _dprintf(fmt,args) 
#define dqprintf qprintf
#define dprintf(fmt, args)
#define d2printf(fmt, args)
#endif

#if DEBUG_MALLOC
#define dm_printf(fmt, args)	\
    _dprintf(fmt, args)
#else
#define dm_printf(fmt, args)
#endif
extern struct SEMAPHORE scs_lock;

int cixcd_misc_rcv();
int cixcd_write_pcq0cr();
extern null_procedure();

extern struct FILE *el_fp;

char *adapstat[] = { "Unknown", "Uninitialized", "Disabled", "Enabled"};

#define prefix(x) cipca_prefix(pb,name,x)
int cipca_prefix( struct ci_pb *pb, char *name, char *string )
    {
    sprintf( name, "pu%c0%s",
	    pb->pb.controller + 'a',
	    string );
    return( name );
    }   




/*+
 * ============================================================================
 * = cixcd_setmode - change driver state				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine changes the state of the port driver. The following
 *	states are relevant:
 *
 *	DDB$K_STOP -	Suspends the port driver.  Interrupts from the
 *			device are dismissed.
 *
 *	DDB$K_RESTARTABLE -
 *			Same as DDB$K_STOP, except the port driver may be
 *			restarted dynamically by the class driver.
 *
 *	DDB$K_START -	Changes the port driver to interrupt mode.
 *			device interrupt handling is once again enabled.
 *
 *	DDB$K_INTERRUPT -
 *			Same as DDB$K_START.
 *
 *	DDB$K_ASSIGN  -	Perform system startup functions.
 *
 * FORM OF CALL:
 *
 *	cixcd_setmode(mode)
 *
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *	msg_port_state_running - port running
 *
 * ARGUMENTS:
 *
 *	int mode - Desired mode for port driver.
 *
 * SIDE EFFECTS:
 *
 *	None.
 *            
-*/

#if TURBO || RAWHIDE
int cixcd_setmode(int mode, struct device *dev)
{
    int i, status;
    struct cipca_sb *sb;
    struct ci_pb *pb;
    struct cixcd_blk *blk_ptr;
    struct PCB *pcb;
    int cipca_poll();
    char name[16];
    int verbose=1;

    switch (mode) {

	case DDB$K_ASSIGN: 
	    break;

	case DDB$K_START: 
	case DDB$K_INTERRUPT: 

	    dprintf("CIPCA_setmode START (%x)\n", p_args1(mode));


	    if ((void *)dev->devdep.io_device.devspecific2 == 0 ) {
	      cipca_init(dev);
	    }

	    blk_ptr = (void *) dev->devdep.io_device.devspecific;
	    if (blk_ptr == 0) {
	        blk_ptr = malloc_z(sizeof(struct cixcd_blk));
	        dev->devdep.io_device.devspecific = (void *) blk_ptr;
                blk_ptr->pb = dev->devdep.io_device.devspecific2;
                blk_ptr->refcnt = 1;
            } else {
                blk_ptr->refcnt++;
	        if (blk_ptr->pb->mode == DDB$K_INTERRUPT) {
                   return msg_success;
              }
            }
	    krn$_set_console_mode(INTERRUPT_MODE);


            /* 
             * initialize class drivers 
             */

	    mscp_shared_adr = ovly_load("MSCP");
	    if (!mscp_shared_adr)
		return msg_failure;

	    nport_shared_adr = ovly_load("NPORT");
	    if (!nport_shared_adr)
		return msg_failure;


	    cipca_alloc((void *)dev->devdep.io_device.devspecific2);



            /*
             * See ECO 903.   This may look out of place being in a
             * setmode routine, but since the setmode routine is already
             * #if TURBO'ed, putting it here keeps the init_adapter
             * flow  more generic.   Besides, TURBO uses the protocol
             * string differently than the other platforms.
             */
            blk_ptr->pb->protocol = "pu";

	    status = cixcd_init_adapter(blk_ptr, dev);
	    if (status != msg_success){
	        pprintf("Failed cixcd_init_adapter\n");
                /*do_bpt();*/
		return status;
            }
            blk_ptr->pb->mode = DDB$K_INTERRUPT;
	    verbose = !cbip;
            cipca_blk_ptr = blk_ptr;
	    cipca_pb = blk_ptr->pb;
#if 0
            cixcd_poll_port(blk_ptr->pb,verbose);
#endif
	    mscp_create_poll();
	    break;




	case DDB$K_STOP: 

	    blk_ptr = (void *) dev->devdep.io_device.devspecific;
	    if (blk_ptr == 0) {
		dprintf("CIPCA setmode - stop, never inited...\n",p_args0);
		return msg_failure;
	    }

            /* if it was already shutdown(diag shutdown), return */
            if (blk_ptr->refcnt == 0)
                return msg_success;

            blk_ptr->refcnt--;
            if (blk_ptr->refcnt != 0)
                return msg_success;

	    pb = blk_ptr->pb;
            pb->pb.mode = DDB$K_STOP;

	    cixcd_shutdown_port(pb);
	    cixcd_stop_port(pb);

           /* Disable interrupts.  When a CIPCA module is resetting, no access
            * can be made to any of its soft registers.   If a lingering 
            * interrupt came in, the interrupt routine read casr, a soft reg, and
            * would cause the CIPCA reset to fail.   It is speculating that this
            * was happening, never proven.
            */
	    wl_intena(blk_ptr,0);         /* disable interrupts */

	    if( pb->pb.desired_mode == DDB$K_INTERRUPT )
	      {
	       io_disable_interrupts(pb,pb->pb.vector);
              }
	    else
	      {
	       dprintf("Removing cipca from pollq\n", p_args0 );
	       remq_lock( &pb->pb.pq.flink );
	      }

            cipca_reset(blk_ptr);
	    mscp_destroy_poll(dev);

	    /* 
             * kill processes 
             */

	    krn$_wait(&scs_lock);	/* grab scs_lock to synchronize */
	    krn$_delete(blk_ptr->completion_pid);
	    krn$_post(&blk_ptr->cixcd_completion_s);
	    krn$_wait(&blk_ptr->cixcd_completion_compl_s);
	    krn$_delete(blk_ptr->p_misc_pid);
	    krn$_post(&blk_ptr->cixcd_misc_s);
	    krn$_wait(&blk_ptr->cixcd_misc_compl_s);

	    krn$_semrelease(&blk_ptr->cixcd_poll_s);
	    krn$_semrelease(&blk_ptr->cixcd_poll_compl_s);
	    krn$_semrelease(&blk_ptr->cixcd_completion_s);
	    krn$_semrelease(&blk_ptr->cixcd_misc_s);
	    krn$_semrelease(&blk_ptr->cixcd_misc_compl_s);
	    krn$_semrelease(&blk_ptr->cixcd_completion_compl_s);
	    krn$_semrelease(&blk_ptr->cixcd_adap_s);
	    int_vector_clear(blk_ptr->vect1);
	    int_vector_clear(blk_ptr->vect2);


	    /* 
             * free system blocks 
             */
	    for (i = 0; i < pb->pb.num_sb; i++) {
		sb = pb->pb.sb[i];
		krn$_semrelease(&sb->ready_s);
		krn$_semrelease(&sb->disk_s);
		krn$_semrelease(&sb->tape_s);
		free(sb);
	    }
	    free(pb->pb.sb);
/*	    ci_dealloc_bdlt(blk_ptr);*/
	    cipca_dealloc(pb);
	    ci_dealloc_nport(blk_ptr);
	    free(blk_ptr);
	    dev->devdep.io_device.devspecific = (void *) 0;
	    krn$_post(&scs_lock);	/* release scs_lock */
	    dqprintf("CIPCA setmode - stop, complete\n", p_args0);
	    break;


	case DDB$K_READY:
#if 0
	    verbose = 0;
            cixcd_poll_units(dev,verbose);
#endif
            break;

	case DDB$K_POLLED: 
	default: 
	    err_printf("CIPCA setmode error - illegal mode.\n");
	    return msg_failure;
	    break;
    }					/* switch */
    return msg_success;
}

#endif

#if !(TURBO || RAWHIDE)
int cixcd_setmode(int mode, struct device *dev)
{
    int i, status;
    struct cipca_sb *sb;
    struct ci_pb *pb;
    struct cixcd_blk *blk_ptr;
    struct PCB *pcb;
    char name[16];
    int cipca_poll();

    switch (mode) {

	case DDB$K_ASSIGN: 
	    break;

	case DDB$K_START: 
	case DDB$K_INTERRUPT: 


	    dprintf("CIPCA_setmode START (%x)\n", mode);

	    krn$_wait(&scs_lock);
	    blk_ptr = (void *) dev->devdep.io_device.devspecific;


           /* 
            * if blk_ptr already setup, device is already initialized 
            */


	    if (blk_ptr == 0) {
	        blk_ptr = malloc_z(sizeof(struct cixcd_blk));
	        dev->devdep.io_device.devspecific = (void *) blk_ptr;
            } else {
	        if (blk_ptr->pb->mode == DDB$K_INTERRUPT){
	           krn$_post(&scs_lock);
		   return msg_success;
		}
	    }

	    krn$_post(&scs_lock);

/*	    krn$_set_console_mode(INTERRUPT_MODE); */

	    status = cixcd_init_adapter(blk_ptr, dev);
	    if (status != msg_success)
		return status;

	    blk_ptr->pb->mode = DDB$K_INTERRUPT;
            cipca_blk_ptr = blk_ptr;
	    cipca_pb = blk_ptr->pb;
            pb = blk_ptr->pb;

            blk_ptr->p_poll_pid = krn$_create( 
                 cipca_poll,	        	/* address of process */
                 null_procedure,                /* startup routine */
		 &blk_ptr->cixcd_poll_compl_s,   /* completion semaphore */
                 5,                             /* process priority */ 
                 1<<whoami(),			/* cpu affinity */
                 4096,				/* stack size */
	         prefix( "_poll" ),		/* process name */
                 0, 0, 0, 0, 0, 0,              /* i/o */
                 blk_ptr->pb );			/* parameter */

/*	    krn$_post(&blk_ptr->cixcd_poll_s); */

	    stack_check();
	    break;

	case DDB$K_STOP: 


	    blk_ptr = (void *) dev->devdep.io_device.devspecific;
	    if (blk_ptr == 0) {
		dprintf("CIPCA setmode - stop, never inited...\n",p_args0);
		return msg_failure;
	    }

	    pb = blk_ptr->pb;
            pb->pb.mode = DDB$K_STOP;


	    /*
	     * kill off the poll process
	     */
	    krn$_delete(blk_ptr->p_poll_pid);
	    krn$_wait(&blk_ptr->cixcd_poll_compl_s);

	    stack_check();

	    cixcd_shutdown_port(pb);
	    cixcd_stop_port(pb);


	    cipca_mscp_destroy(pb);


	    wl_intena(blk_ptr,0);         /* disable interrupts */
	    pb->adapter_state = Disabled;
	    if( pb->pb.desired_mode == DDB$K_INTERRUPT )
	      {
	       io_disable_interrupts(pb,pb->pb.vector);
              }
	    else
	      {
	       dprintf("Removing cipca from pollq\n", p_args0 );
	       remq_lock( &pb->pb.pq.flink );
	      }
            cipca_reset(blk_ptr);

	    /* 
             * kill processes 
             */

	    krn$_wait(&scs_lock);	/* grab scs_lock to synchronize */

	    /* 
             * kill off the completion process
	     */
	    krn$_delete(blk_ptr->completion_pid);
	    krn$_post(&blk_ptr->cixcd_completion_s);
	    krn$_wait(&blk_ptr->cixcd_completion_compl_s);

	    /*
	     * kill off the misc process
	     */
	    krn$_delete(blk_ptr->p_misc_pid);
	    krn$_post(&blk_ptr->cixcd_misc_s);
	    krn$_wait(&blk_ptr->cixcd_misc_compl_s);


	    krn$_semrelease(&blk_ptr->cixcd_poll_s);
	    krn$_semrelease(&blk_ptr->cixcd_poll_compl_s);
	    krn$_semrelease(&blk_ptr->cixcd_completion_s);
	    krn$_semrelease(&blk_ptr->cixcd_misc_s);
	    krn$_semrelease(&blk_ptr->cixcd_misc_compl_s);
	    krn$_semrelease(&blk_ptr->cixcd_completion_compl_s);
	    krn$_semrelease(&blk_ptr->cixcd_adap_s);
	    int_vector_clear(blk_ptr->vect1);
	    int_vector_clear(blk_ptr->vect2);


	    /* 
             * free system blocks 
             */
	    for (i = 0; i < pb->pb.num_sb; i++) {
		sb = pb->pb.sb[i];
		krn$_semrelease(&sb->ready_s);
		krn$_semrelease(&sb->disk_s);
		krn$_semrelease(&sb->tape_s);
		free(sb);
	    }
	    free(pb->pb.sb);

/*	    ci_dealloc_bdlt(blk_ptr); */
	    ci_dealloc_nport(blk_ptr);

	    free(blk_ptr);
	    dev->devdep.io_device.devspecific = (void *) 0;
	    krn$_post(&scs_lock);	/* release scs_lock */
	    dqprintf("CIPCA setmode - stop, complete\n", p_args0);
	    break;


	case DDB$K_READY:
	            dprintf("setmode READY\n",p_args0);
                    break;

	case DDB$K_POLLED: 
	default: 
	    err_printf("CIPCA setmode error - illegal mode.\n");
	    return msg_failure;
	    break;
    }					/* switch */
    return msg_success;
}
#endif



void cixcd_setmode_pb(struct pb *pb, int mode){
  cixcd_setmode(mode,pb->config_device);
}


/*+
 * ============================================================================
 * = cixcd_poll_units - poll a remote server to find attached units           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine polls a remote MSCP server in order to find all attached
 *	MSCP units.  It does this by sending MSCP Get Unit Status commands,
 *	using the Next Unit modifier, until an MSCP response is received which
 *	indicates that no more units exist.  Polling occurs only on the
 *	specified adapter.
 *  
 * FORM OF CALL:
 *  
 *	cixcd_poll_units(dev, verbose)
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct device *dev	- Pointer to device structure of adapter.
 *	int verbose		- if set, print node information.
 *
 * SIDE EFFECTS:
 *
 *	Files will be created.
 *
-*/
#if TURBO || RAWHIDE
void cixcd_poll_units(struct device *dev, int verbose)
{
    int status;
    struct ci_pb *pb;
    struct cixcd_blk *blk_ptr;
    u_char cletter[MAX_NAME];

    blk_ptr = (void *) dev->devdep.io_device.devspecific;
    pb = blk_ptr->pb;

    dprintf("CIPCA poll_units\n", p_args0);

    if (verbose) {
	printf("%-18s %-7s %20s  %s\n", pb->pb.port_name, pb->pb.alias, pb->pb.info, pb->pb.version);
    }


/* do an IDREQ to each node */

    cixcd_poll_port(pb, verbose);

    force_path_idreqs(pb,0);


/* wait for nodes to connect, no path or timeout */

    dprintf("poll_units: wait\n", p_args0);

    cixcd_wait(blk_ptr);

#if TURBO || RAWHIDE
/* poll for units */

    dprintf("poll_units: mscp_poll_units\n", p_args0);

    mscp_poll_units(dev);
#endif

    dprintf("poll_units: complete\n", p_args0);
}
#endif


/*+
 * ============================================================================
 * = cixcd_poll_port - poll to see which nodes exist                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine performs polling, in order to get an accurate picture
 *	of which nodes exist and which don't.  This routine is careful never 
 *	to poll the port's own node number.
 *  
 * FORM OF CALL:
 *  
 *	cixcd_poll( pb, verbose )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct ci_pb *pb	- port block pointer
 *	int verbose		- if set, print node information.
 *
 * SIDE EFFECTS:
 *
 *	Nodes may be found.
 *
-*/
void cixcd_poll_port(struct ci_pb *pb, int verbose)
{
    int i, status;
    struct cipca_sb *sb;

/* Visit each SB. */


    for (i = 0; i < pb->pb.num_sb; i++) {

	sb = pb->pb.sb[i];

        if (sb->poll_active == POLL_NEED_FORCE_PATH) {
          krn$_wait(&scs_lock);
	  sb->poll_active = POLL_WAITING;
          ci_sa_ppd_send_idreq_fp(sb,PATH_A);
          ci_sa_ppd_send_idreq_fp(sb,PATH_B);
          krn$_post(&scs_lock);
          krn$_sleep(cipca_poll_delay);
          continue;
        }
  
	sb->pdevice = 1;
	sb->verbose = verbose;

	sb->poll_active = POLL_WAITING;	/* waiting for IDREQ to be returned */
	sb->dqi = 0;			/* do not inhibit datagrams*/

	if (i == pb->node_id) {
	    sb->poll_active = POLL_OK;
	    continue;			/* Don't poll ourselves. */
	}

#if 0
        if( sb->poll_in_prog ){
              pprintf("poll in prog on node %d\n",i);
	      continue;
        }
#endif

	dprintf("CIPCA poll %d.%d\n", p_args2(pb->pb.channel, i));

	krn$_wait(&scs_lock);

	ci_sa_ppd_send_idreq(sb);

	krn$_post(&scs_lock);

	krn$_sleep(cipca_poll_delay);
    }
}



void force_path_idreqs(struct ci_pb *pb, int verbose)
{
    int i, status;
    struct cipca_sb *sb;

/* Visit each SB. */



    for (i = 0; i < pb->pb.num_sb; i++) {

	sb = pb->pb.sb[i];

	if (i == pb->node_id) {
	    continue;			/* Don't poll ourselves. */
	}

	krn$_wait(&scs_lock);

	ci_sa_ppd_send_idreq_fp(sb,PATH_A);

	ci_sa_ppd_send_idreq_fp(sb,PATH_B);

	krn$_post(&scs_lock);

	krn$_sleep(cipca_poll_delay);
    }
}





void cixcd_wait(struct cixcd_blk *blk_ptr)
{
    int status0;

    while (1) {

	status0 = cixcd_wait_port(blk_ptr->pb);

	if (status0 == msg_success)
	    break;

	krn$_sleep(1000);
    }
}

/*
 * cixcd_wait_port
 *	use sb->poll_active to indicate polling condition
 *	  0 = ok
 *	  1 = waiting for IDREQ packet to be returned or connect
 *	  2 = no path to node
 *	  3 = poll timeout reported
 */
int cixcd_wait_port(struct ci_pb *pb)
{
    int i, j;
    struct cipca_sb *sb;

/* Visit each SB. */

    j = 0;

    for (i = 0; i < pb->pb.num_sb; i++) {

	sb = pb->pb.sb[i];

	if (i == pb->node_id) {
	    j++;
	    continue;			/* Don't poll ourselves. */
	}
	if (sb->poll_active != POLL_WAITING)
					/* completed */
	    j++;

	if (sb->poll_active == POLL_WAITING) {
					/* still waiting */

/* If the delay time is up, then mark node bad */

	    if ( (sb->poll_delay != 0) && (sb->poll_delay2 != 0) ) {
		--sb->poll_delay;
		--sb->poll_delay2;

		if (sb->state == sb_k_closed) {
		    krn$_wait(&scs_lock);
		    ci_sa_ppd_send_idreq(sb);
		    krn$_post(&scs_lock);
		}
		if (sb->state == sb_k_start_out) {
		    krn$_wait(&scs_lock);
		    ci_sa_ppd_send_dg_start(sb);
		    krn$_post(&scs_lock);
		}

	    } else {
		if (sb->poll_active == POLL_WAITING) {

		    dprintf("CIPCA poll timeout %d.%d\n",
		      p_args2(pb->pb.channel, i));

		    sb->poll_active = POLL_TIMEOUT;
		    j++;
		}
	    }
	}
    }
    if (j != pb->pb.num_sb)
	return msg_failure;

    return msg_success;
}



#if 0
/*
 * cixcd_wait_on_nodes
 *	use sb->poll_active to indicate polling condition
 *	  0 = ok
 *	  1 = waiting for IDREQ packet to be returned or connect
 *	  2 = no path to node
 *	  3 = poll timeout reported
 */
int cixcd_wait_on_nodes(struct ci_pb *pb)
{
char *poll_active_txt[]={"POLL_OK","POLL_WAITING","POLL_NOPATH","POLL_TIMEOUT"};
    int i;
    struct cipca_sb *sb;
    struct TIMERQ *tq, tqe;
    tq = &tqe;

    /* Visit each SB. */
    for (i = 0; i < pb->pb.num_sb; i++) {

	sb = pb->pb.sb[i];
	if (i == pb->node_id) continue;  /* Don't poll ourselves. */

	sb->pdevice = 1;
	sb->verbose = !cbip;  /* verbose; */
	sb->dqi = 0;			/* do not inhibit datagrams*/
        sb->poll_active = POLL_WAITING;
	sb->stallvc = 0;


	krn$_wait(&scs_lock);
	ci_sa_ppd_send_idreq(sb);
	krn$_post(&scs_lock);

        pprintf("[1]Poll: %d  %s\n",
                    i, poll_active_txt[sb->poll_active]);

        start_timeout(2000,tq);			/* 1 second timeout */
        while( (sb->poll_active == POLL_WAITING) && (tq->sem.count==0) ) {
           krn$_sleep(10);           /* Need blocking for timeout to work */
        }
        if (tq->sem.count) {
           sb->poll_active = POLL_TIMEOUT;
        }
        stop_timeout(tq);
        pprintf("[2]Poll: %d  %s\n",
                    i, poll_active_txt[sb->poll_active]);

        krn$_sleep(cipca_poll_delay);

    } /* for */

    return msg_success;
}
#endif


#if 0
int poll_node()
{
char *poll_active_txt[]={"POLL_OK","POLL_WAITING","POLL_NOPATH","POLL_TIMEOUT"};
struct ci_pb *pb = cipca_pb;
    int i;
    struct cipca_sb *sb;
    struct TIMERQ *tq, tqe;
    int node,repcnt;
    tq = &tqe;


    node = *(int*)0x500000;
    repcnt = *(int*)0x500004;


    /* Visit each SB. */
    for (i = 0; i < repcnt; i++) {

	sb = pb->pb.sb[node];
	if (node == pb->node_id) continue;  /* Don't poll ourselves. */

	sb->pdevice = 1;
	sb->verbose = !cbip;  /* verbose; */
	sb->dqi = 0;			/* do not inhibit datagrams*/

        sb->poll_active = POLL_WAITING;



	krn$_wait(&scs_lock);
	ci_sa_ppd_send_idreq(sb);
	krn$_post(&scs_lock);

        pprintf("[1]Poll: %d  %s\n",
                    node, poll_active_txt[sb->poll_active]);

        start_timeout(2000,tq);			/* 1 second timeout */
        while( (sb->poll_active == POLL_WAITING) && (tq->sem.count==0) ) {
           krn$_sleep(10);           /* Need blocking for timeout to work */
        }
        if (tq->sem.count) {
           sb->poll_active = POLL_TIMEOUT;
        }
        stop_timeout(tq);
        pprintf("[2]Poll: %d  %s\n",
                    node, poll_active_txt[sb->poll_active]);

        krn$_sleep(cipca_poll_delay);

    } /* for */

    return msg_success;
}
#endif





/*+
 * ============================================================================
 * = cipca_poll - poll to see which nodes exist                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine performs polling, in order to get an accurate picture
 *	of which nodes exist and which don't.  Polling periodically is a way
 *	of detecting nodes coming and going.  Polling is done more frequently
 *	in the beginning, so that nodes may be found quickly, and less
 *	frequently thereafter, so that not much bandwidth is used.  This
 *	routine is careful never to poll the port's own node number.
 *  
 * FORM OF CALL:
 *  
 *	cipca_poll( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pua_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	Nodes may be found.  Nodes may be lost.
 *
-*/
void cipca_poll( struct ci_pb *pb )
    {
    char name[16];
    int i;
    struct cipca_sb *sb;
    int cipca_poll();
    volatile int verbose=1;
    struct PCB *pcb = getpcb();


#if 0
    cixcd_wait_on_nodes(pb);
    return;

    if (*(int*)0x300000==0xFEEDFACE){
        return;
    }
 
    if (*(int*)0x30004==0xFEEDFACE) {
       cixcd_poll_port(pb,verbose);
       return;
    }
#endif

    while (1) {
      if (pcb->pcb$l_killpending & SIGNAL_KILL) {
	dprintf("cipca_poll killed\n", p_args0);
	return;
      }

      if( pb->pb.mode != DDB$K_STOP ){
	  verbose = !cbip;
          cixcd_poll_port(pb,verbose);
      }

      krn$_sleep(1000);

    }



    }



/*+
 * ============================================================================
 * = cixcd_establish_connection - establish a connection to given unit        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine establishes a connection to a given unit.
 *  
 * FORM OF CALL:
 *  
 *	cixcd_establish_connection( inode, dev, class )
 *  
 * RETURNS:
 *
 *	success or failure.
 *       
 * ARGUMENTS:
 *
 *	struct INODE *inode	- pointer to inode.
 *	struct device *dev	- pointer to device block
 *	int class		- class of connection
 *
 * SIDE EFFECTS:
 *
 *	Connection established to given unit.
 *
-*/
#if TURBO || RAWHIDE
int cixcd_establish_connection(struct INODE *inode, struct device *dev, int class)
{
    int unit;
    int node;
    int channel;
    int i;
    struct cixcd_blk *blk_ptr;
    struct cipca_sb *sb;
    struct ci_pb *pb;

    dprintf("CIPCA establish_connection: %s\n", p_args1(inode->name));

/* diagnostic mode, node already configured */

    if (diagnostic_mode_flag)
	return msg_success;

/* if device not in file system, fail */

    if (!inode->inuse) {
	return msg_failure;
    }

/* get device particulars */

    if (explode_devname(inode->name, 0, 0, &unit, &node, &channel, 0, 0) !=
      msg_success) {
	return msg_failure;
    }

/* get device pointers */

    blk_ptr = (void *) dev->devdep.io_device.devspecific;

/* do an IDREQ to given node */

    pb = blk_ptr->pb;

    sb = pb->pb.sb[node];

/* if sb in use, device already configured */

    if (sb->ref != 0)
	return msg_success;

    sb->ref++;

    sb->pdevice = 0;
    sb->verbose = 0;

    sb->dqi = 0;			/* allow datagrams for this device */
    sb->poll_active = POLL_WAITING;	/* waiting for IDREQ to be returned */

    krn$_wait(&scs_lock);

/* do an IDREQ to given node */

    ci_sa_ppd_send_idreq(sb);

    krn$_post(&scs_lock);

/* wait for node to connect, no path or timeout */

    dprintf("establish_connection: wait, %s\n", p_args1(inode->name));

    sb->poll_delay = 5;
    sb->poll_delay2 = 15;

    while ( (sb->poll_delay != 0) && (sb->poll_delay2 != 0) ) {
	if (sb->poll_active != POLL_WAITING) {
	    break;
	} else {
	    if (sb->state == sb_k_closed) {
		krn$_wait(&scs_lock);
		ci_sa_ppd_send_idreq(sb);
		krn$_post(&scs_lock);
	    }
	    if (sb->state == sb_k_start_out) {
		krn$_wait(&scs_lock);
		ci_sa_ppd_send_dg_start(sb);
		krn$_post(&scs_lock);
	    }
	    sb->poll_delay--;
	    sb->poll_delay2--;
	    krn$_sleep(1000);
	}
    }
    if ( (sb->poll_delay == 0) || (sb->poll_delay2 == 0) ) {
	dprintf("CIPCA poll timeout %d.%d\n", p_args2(channel, node));

	return msg_failure;
    }
    if (sb->poll_active != POLL_OK) {

	dprintf("CIPCA poll error %d.%d\n", p_args2(channel, node));

	return msg_failure;
    }

/* poll for units */

    dprintf("establish_connection: poll\n", p_args0);

    mscp_poll_units(dev);

    dprintf("establish_connection: complete\n", p_args0);

    return msg_success;
}
#endif


/*+
 * ============================================================================
 * = cixcd_completion_rcv - process that waits for port interrupts            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine wait for port completion interrupts.
 *  
 * FORM OF CALL:
 *  
 *	cixcd_completion_rcv( blk_ptr )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cixcd_blk *blk_ptr - block pointer
 *
 * SIDE EFFECTS:
 *
 *	The receive routine is called.
 *
-*/

void cixcd_completion_rcv(struct cixcd_blk *blk_ptr)
{
    int status;
    volatile struct ci_pb *pb = blk_ptr->pb;
    struct PCB *pcb = getpcb();

/*
 * Wait for the ISR to post the receive semaphore.
 * This indicates that a new response is available.
 */


    while (1) {

	if( (rl(nodests) & 0x10) == 0 ){
	   krn$_wait(&blk_ptr->cixcd_completion_s);
        }

	if (pcb->pcb$l_killpending & SIGNAL_KILL) {
	    dprintf("CIPCA completion_rcv killed\n", p_args0);
	    return;
	}

#if 0
	if (!pb->interrupt)
	    continue;
#endif
	/*
	 * if we got here by polling, then  we need to read the CASR
	 * to clear the hardware presentation of INTA.
	 */


        if (!pb->interrupt) {
	  rl(casr0);
          io_issue_eoi(pb,pb->pb.vector);
	}


	krn$_wait(&scs_lock);

#if 0
	if (pb->ddfq_entries <= MINDDFQ) {
	    dprintf("Sending retqe: ddfq depleted\n", p_args0);
	    status = ci_sa_ppd_send_retqe(pb->pb.sb[pb->node_id], DDFQREFILL);
	}
#endif

	d2printf("CIPCA_completion_rcv, completion interrupt\n",
	      p_args0);
/*	   
 *  Clear interrupts before we process the adrq.
 *
 *  AMTCR - The port driver forces the maintenance/sanity timer to reset its 
 *  expiration time time by writing the AMTCR.  If the AMTCR is not written
 *  again before the expiration time, the adapter will enter the Unitialized/
 *  Maintenance state and request an ASTE miscellaneous interrupt.
 *  AMTCR is ignored if the maintenance/sanity timer is not running.
 *
 *  CASRCR - After the port driver has responded to a miscellaneous
 *  interrupt and read the contents of the Channel Status Register;  
 *  it returns  the registers to the channel by writing CASRCR.
 *  
 */
	pb->interrupt = 0;
/*        io_issue_eoi(pb,pb->pb.vector);*/
	wl(amtcr, 1);

	/* MUST NOT write CASRCR on CIPCA if only ASIC set in CASR*/
#if 0
	   wl(casrcr, 1);
#endif
	cixcd_receive(pb);
	krn$_post(&scs_lock);
        stack_check();

    } /* while */
}

/*+
 * ============================================================================
 * = cixcd_misc_rcv - process packets from the response queue                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine processes port responses.  It is notified when a new
 *	response exists on the response queue, and it removes and processes
 *	that new response.
 *  
 * FORM OF CALL:
 *  
 *	cixcd_misc_rcv( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct ci_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	The response queue is periodically emptied.
 *
-*/
void cixcd_misc_rcv(volatile struct cixcd_blk *blk_ptr)
{
    int st;

    Z_CASR0 casr0_val;
    Z_CASR1 casr1_val;
    Z_AMCSR amcsr_val;
    Z_XBER xber_val;
    u_int xfadr_val;
    u_int cafar0_val;
    u_int cafar1_val;
    N_CARRIER *car_ptr = 0;		/* Queue Carrier */
    N_Q_BUFFER *nbuf = 0;		/* Queue Buffer stacked with command */
    struct ci_pb *pb = blk_ptr->pb;
    struct PCB *pcb = getpcb();
    int csr_data;

    while (1) {
        /* Wait for adapter miscellaneous interrupt */
	krn$_wait(&blk_ptr->cixcd_misc_s);
	if (pcb->pcb$l_killpending & SIGNAL_KILL) {
	    dprintf("CIPCA misc_rcv killed\n", p_args0);
	    return;
	}

	csr_data = rl(casr0);
        if (csr_data & cixcd_psr_dse) {
	   pprintf("Data Structure Error - AFAR:%x  CASR1:%x\n",
                           rl(cafar0), rl(casr1) );
           /*do_bpt();*/
	}
        if (csr_data & 2){
            int i;
            /* Adapter Free Queue exhausted.  Replenish */
            pprintf("cipca: datagram free queue exhausted.  Replenishing.\n",
              p_args0);
            krn$_wait(&scs_lock);
            for (i = 0; i < N_ADAP_DG_REFILL; i++) {
                ci_remove_queue(DDFQ, pb, &car_ptr, &nbuf);
                ci_insert_queue(DADFQ, pb, car_ptr, nbuf);
            }
            krn$_post(&scs_lock);
        }

        /*
         *  Dismiss the interrupt 
         */
        pprintf("Dismissing the misc int\n");
        pb->interrupt = 0;
        io_issue_eoi(pb,pb->pb.vector);
        wl(casrcr, 1);
    }					/* while (1) */
}

/*+
 * ============================================================================
 * = cixcd_receive - process packets from the response queue                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine processes port responses.  It is notified when a new
 *	response exists on the response queue, and it removes and processes
 *	that new response.
 *  
 * FORM OF CALL:
 *  
 *	cixcd_receive( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct ci_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	The response queue is periodically emptied.
 *
-*/
void cixcd_receive(volatile struct ci_pb *pb)
{
    int t, st;
    N_CARRIER *car_ptr;
    N_Q_BUFFER *p;

    while (1) {
	car_ptr = 0;
	p = 0;
/* 
** Read any adapter responses.  Loop until queue is exhausted.
** then restore int.  Reset expiration time.
*/                       
#if DEBUG_QUEUE
	if (ci_debug_flag & 0x20) {
	    krn$_wait(pb->adap_s);
	    render_queue(ADRQ, pb->ab->adrq.head_ptr, 0);
	    krn$_post(pb->adap_s);

	    krn$_wait(pb->adap_s);
	    render_queue(DAMFQ, pb->ab->damfq.head_ptr, 0);
	    krn$_post(pb->adap_s);

	    krn$_wait(pb->adap_s);
	    render_queue(DADFQ, pb->ab->dadfq.head_ptr, 0);
	    krn$_post(pb->adap_s);
	}
#endif
	st = ci_remove_queue(ADRQ, pb, &car_ptr, &p);

	d2printf("CIPCA_receive, pull off adrq status %x car %x buf %x\n",
	      p_args3(st, car_ptr, p));

#if 0
    dump_n_carrier(car_ptr);
    dump_n_q_buffer(p);
#endif

	if (st == msg_success) {

	    p = car_ptr->q_buf_ptr;

	    if (car_ptr != 0 && p != 0)
		ci_sa_response(pb, car_ptr, p);
	    else {

		d2printf("     cixcd_receive NULL N CMD BUF %s - \n",
		      p_args1(pb->pb.port_name));
	    }
	} else if (p) {			/* whoops.  error with non-null
					 * buffer.*/

		d2printf("CIPCA_receive error %s - \n",
		  p_args1(pb->pb.port_name));
	}

	  else {			/* done pulling all responses off the
					 * queue.*/

		d2printf("CIPCA_receive %s - break\n",
		  p_args1(pb->pb.port_name));
	    break;
	}
    }
}

/*+
 * ============================================================================
 * = cixcd_init_adapter - initialize the CIXCD adapter                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the adapter data structures and the
 *	common hardware of the CIXCD.
 *  
 * FORM OF CALL:
 *  
 *	cixcd_init_adapter( blk_ptr, dev )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cixcd_blk *blk_ptr	- pointer to parameter block
 *	struct device *dev		- pointer to device block
 *
 * SIDE EFFECTS:                                   
 *
 *	None.
 *
-*/
int cixcd_init_adapter(struct cixcd_blk *blk_ptr, struct device *dev)
{
    int i, status;
    int csr_data;
    int base_adr;
    int hose, slot, csr, vect1, vect2;
    struct ci_pb *pb;
    struct pb temp_pb;
    char driver[8];
    int cixcd_interrupt();
    void cipca_poll_ints();
    struct FILE *fp;
    unsigned int base0,base1,base2;
    int vect;

    blk_ptr->controller = dev->devdep.io_device.controller;
    blk_ptr->hose = dev->hose;
    blk_ptr->slot = dev->slot;
    blk_ptr->function = dev->function;
    blk_ptr->bus = dev->bus;

    memset(&temp_pb,0,sizeof(struct pb));
    temp_pb.hose = dev->hose;
    temp_pb.slot = dev->slot;
    temp_pb.function = dev->function;
    temp_pb.bus = dev->bus;
    base0 = incfgl(&temp_pb, BASE0) & 0xFFFFFFFE;  
    base1 = incfgl(&temp_pb, BASE1) & 0xFFFFFFFE;
    base2 = incfgl(&temp_pb, BASE2) & 0xFFFFFFFE;
    dprintf("cipca base0:%x base1:%x base2:%x\n",p_args3(base0, base1, base2));
    blk_ptr->hard_base = base0;
    blk_ptr->soft_base = base1;
    blk_ptr->mem_base = base2;


    dprintf("Vector being assigned: %x %x\n",p_args2(
                dev->devdep.io_device.vector[0],
                dev->devdep.io_device.vector[1]) );

    blk_ptr->vect1 = dev->devdep.io_device.vector[0];
    blk_ptr->vect2 = dev->devdep.io_device.vector[1];
    

    dm_printf("setmode - start\n",p_args0);

    hose = blk_ptr->hose;
    slot = blk_ptr->slot;
    csr = blk_ptr->mem_base;
    vect1 = blk_ptr->vect1;
    vect2 = blk_ptr->vect2;

    dprintf("CIPCA hose/slot/ctl/soft_base/vect %d/%d/%d/%08x/%x/%x\n",
      p_args6(hose, slot, blk_ptr->controller, csr, vect1, vect2));


/*  Hold the SCS lock for the duration. */
           
    krn$_wait(&scs_lock);

/* create port block */


    blk_ptr->pb = dev->devdep.io_device.devspecific2;
    pb = blk_ptr->pb;
    pb->hard_base = blk_ptr->hard_base;
    pb->soft_base = blk_ptr->soft_base;
    pb->mem_base = blk_ptr->mem_base;
    pb->pb.csr = blk_ptr->mem_base;
    pb->pb.config_device = dev;
    pb->adapter_state = Uninitialized;

    dprintf("cipca init_adapter: malloc pb = %x\n", p_args1(pb));

/* create the mailbox */

    pb->blk_ptr = blk_ptr;

/* set port interrupt vector */

    if( pb->pb.vector )
     {
      pb->pb.desired_mode = DDB$K_INTERRUPT;
      int_vector_clear(blk_ptr->vect1);
      int_vector_set(blk_ptr->vect1, cixcd_interrupt, blk_ptr->pb,0);
      int_vector_clear(blk_ptr->vect2);
      int_vector_set(blk_ptr->vect2, cixcd_interrupt, blk_ptr->pb,0);
     }
    else
     {
      pprintf("CIPCA in polled mode\n");
      pb->pb.mode = DDB$K_POLLED;
      pb->pb.desired_mode = DDB$K_POLLED;
      pb->pb.pq.routine = cipca_poll_ints;
      pb->pb.pq.param = blk_ptr;
     }

/* Set the semphores */

    krn$_seminit(&blk_ptr->cixcd_completion_compl_s, 0,
      "cipca_completion_compl_s");   
    krn$_seminit(&blk_ptr->cixcd_misc_compl_s, 0, "cipca_misc_compl_s");
    krn$_seminit(&blk_ptr->cixcd_misc_s, 0, "cipca_misc_s");
    krn$_seminit(&blk_ptr->cixcd_completion_s, 0, "cipca_completion_s");
    krn$_seminit(&blk_ptr->cixcd_poll_s, 0, "cipca_poll_s");
    krn$_seminit(&blk_ptr->cixcd_poll_compl_s, 0, "cipca_poll_compl_s");
    krn$_seminit(&blk_ptr->cixcd_adap_s, 1, "cipca_adapter_s");


/* initialize the port */

    status = cixcd_init_port(blk_ptr, 0);

/*  Drop the SCS lock. */

    krn$_post(&scs_lock);

    return status;
}

/*+
 * ============================================================================
 * = cixcd_init_port - initialize a given port                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the port driver data structures and the
 *	port hardware of a specific, given port.
 *  
 * FORM OF CALL:
 *  
 *	cixcd_init_port( blk_ptr, channel, num_sb ))
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cixcd_blk *blk_ptr	- pointer to parameter block
 *	int channel			- the channel of this port
 *	int num_sb			- the maximum number of SBs to handle
 *
 * SIDE EFFECTS:                                   
 *
 *	A port block is allocated and initialized for each instance of port
 *	hardware of the appropriate type.
 *
-*/
int cixcd_init_port(struct cixcd_blk *blk_ptr, int channel)
{
    int i;
    int status;
    char cletter[MAX_NAME];
    int controller = blk_ptr->controller;
    int hose = blk_ptr->hose;
    int slot = blk_ptr->slot;
    int csr = blk_ptr->mem_base;
    struct ci_pb *pb = blk_ptr->pb;
    struct cipca_sb *sb;
    int ci_sa_misc();
    int ci_sa_receive();
    int ci_sa_scs_connect();
    int ci_sa_scs_accept();
    int cixcd_setmode_pb();
    Z_MREV_HW rev_hw;
    Z_MREV_FW rev_fw;


    dprintf("init_port: chnl %d \n", p_args1(channel));

    pb->pb.setmode = cixcd_setmode_pb;
    pb->hard_base = csr;
    pb->poke = cixcd_write_pcq0cr;
    pb->vector = blk_ptr->vect1;
    pb->vector2 = blk_ptr->vect2;

    pb->interrupt = 0;
    pb->adap_s = &blk_ptr->cixcd_adap_s;

    pb->pb.hose = hose;
    pb->pb.slot = slot;
    pb->pb.channel = channel;
    pb->pb.controller = controller;
    pb->adapter_state = Uninitialized;

    sprintf(pb->pb.string,"%s  hose %x  slot %x", "CIPCA",
                                          pb->pb.hose,pb->pb.slot);

    rev_hw.i = rl(mrev_hw);
    rev_fw.i = rl(mrev_fw);
    fprintf(el_fp,"%s rev_hw:%x (REV:%c)  rev_fw:%4.4s\n",
      pb->pb.string,rev_hw,rev_hw.f.modrev,&rev_fw.f);

    sprintf(pb->pb.version, "%4.4s", &rev_fw.f);

    status = cixcd_disable_port(blk_ptr);

    strcpy(pb->pb.name,pb->pb.port_name);

    if (status != msg_success) {
	printf("CIPCA failed to disable port %s\n", pb->pb.port_name);
	return status;
    }

    dprintf("init_port:  pb.num_sb %x \n", p_args1(pb->pb.num_sb));

    pb->pb.sb = malloc_z(pb->pb.num_sb * sizeof(struct mscp_sb *));

    dm_printf("init_port: malloc pb.num_sb %x pb.sb = %x\n",
      p_args2(pb->pb.num_sb, pb->pb.sb));
/*
 * Statically allocate an SB for each possible node connected to this
 * port.  CIXCD supports up to 16 attached nodes (CIXCD counts as one
 * but for simplicity we ignore this fact).  Fill in each SB.
 */
    for (i = 0; i < pb->pb.num_sb; i++) {

	pb->pb.sb[i] = malloc_z(sizeof(*sb));
	sb = pb->pb.sb[i];       

	dm_printf("init_port: malloc sb = %x\n", p_args1(sb));

	pb->pb.sb[i] = sb;
	sb->pb = pb;
	sb->cb.flink = &sb->cb.flink;
	sb->cb.blink = &sb->cb.flink;
	sb->connect = ci_sa_scs_connect;
	sb->accept = ci_sa_scs_accept;
	sb->stallvc = 1;
	sb->ok = 0;

/*
 * hold off responding to datagrams, until we get idrec from node.
 * inhibit datagrams bit, set to 1 when idrec: and the maintenance id
 * is a device we do NOT want. When polling all devs, will be reset to
 * allow datagrams, and when connecting in establish_connection.  Then,
 * when we get the idrec, if it is an unwanted device, set to 1 so we
 * will send a setckt dqi to it.
 */
	sb->poll_count = 1;		/* number of fast polls */
	sb->poll_delay = 5;		/* 5 sec for timeout */
	sb->poll_delay2 = 15;		/* 15 sec for master timeout */
	sb->poll_active = POLL_OK;
	sb->send_active = 0;
	sb->node_id = i;

	krn$_seminit(&sb->ready_s, 0, "cipca_sb_ready");
	krn$_seminit(&sb->disk_s, 0, "disk");
	krn$_seminit(&sb->tape_s, 0, "tape");

	sprintf(sb->name, "%s%s.%d.%d.%d.%d", pb->pb.protocol,
	  ctl_id(pb->pb.controller, cletter), sb->node_id,
	  pb->pb.channel, pb->pb.slot, pb->pb.hose);
    }
/* 
** Start the CIXCD port 
*/
    status = cixcd_enable_port(blk_ptr);

    if (status != msg_success) {
	printf("CIPCA failed to enable port %s\n", pb->pb.port_name);
	return status;
    }
    return msg_success;
}

/*+
 * ============================================================================
 * = cixcd_disable_port - disable the CIXCD port                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine disables the CIMNA adapter, reads the adapter block
 *	for adapter data.
 *  
 * FORM OF CALL:
 *  
 *	cixcd_disable_port( cixcd_blk_ptr )
 *  
 * RETURNS:
 *     
 *	Success or failure (1 or 0).
 *       
 * ARGUMENTS:
 *
 *	struct cixcd_blk *blk_ptr - cixcd block data
 *
 * SIDE EFFECTS:
 *
 *	The CIMNA port is now ready for operation.
 *	Completion interrupts are enabled.
 *     
-*/
int cixcd_disable_port(struct cixcd_blk *blk_ptr)
{
    int i;
    int csr_data, status;
    char cletter[MAX_NAME];
    struct ci_pb *pb = blk_ptr->pb;
    Z_ABBR0 abbr0;
    XPORT *node_addr;
    struct ddma *ddma;
    int cixcd_misc_rcv();
    struct TIMERQ *tq, tqe;
    tq = &tqe;

    dprintf("disable_port\n", p_args0);

/*	 
**  Create misc interrupt handler.
*/
    blk_ptr->p_misc_pid = krn$_create(cixcd_misc_rcv, 
					/* address of process   */
      null_procedure, 			/* startup routine      */
      &blk_ptr->cixcd_misc_compl_s, 	/* completion semaphore */
      6, 				/* process priority     */
      1 << whoami(), 			/* cpu affinity mask    */
      4096, 				/* stack size           */
      "cipca_misc_rcv", 		/* process name         */
      0, 0, 0, 0, 0, 0,			/* i/o 			*/
      blk_ptr);
/*
 * Make sure the CIXCD is stopped. This puts the port in an UNINITIALIZED state.
 */
    cipca_reset(blk_ptr);

    if(pb->pb.desired_mode == DDB$K_INTERRUPT)
      {
        io_enable_interrupts( blk_ptr->pb, blk_ptr->vect1 );
      }
    else
      {
        dprintf("inserting cipca poller onto pollq\n", p_args0 );
	insq_lock( &pb->pb.pq.flink, &pollq );
      }

/*
** Make an ab and write adapter block base reg 
*/
    blk_ptr->ab = pb->ab = malloc_az(sizeof(NPORT_AB), HWRPB$_PAGESIZE, 0);
    abbr0.i = (int) vtop(pb->ab);
    ddma = &pb->ab->rsvd3;
    ddma->ba_ddma_base0 = $DMA_BASE();
    ddma->ddma_len0 = 0x40000000;               /* Lets say it is 1GB */
    wl(abbr, abbr0.i + $DMA_BASE() );

/*  
** enable interrupts, disable sanity timer
*/
    pb->pb.state = DDB$K_INTERRUPT;
    wl_intena(blk_ptr,CIPCA_INTENA_M_ENAA);
    wl(amcsr, cixcd_pmcsr_mtd);
/*	 
** DISABLED state  
*/               
    /* CICR - The port driver initializes the channel by writing CICR.
     * When the initialization is complete and the channel has written
     * its Parameter Block into the Adapter Block, the channel request
     * a miscellaneous interrupt.
     */
    pb->adapter_state = Disabled;
    wl(cicr, 1);

    /* wait for entry to Disabled state */
    start_timeout(1000,tq);			/* 1 second timeout */
    while ( ( (rl(casr0) & cixcd_psr_ic) == 0 ) && (tq->sem.count==0) ) { 
     krn$_sleep(0);           /* Need blocking for timeout to work */
    }
    if (tq->sem.count) pprintf("Timed out waiting to enter DISABLED state\n");
    stop_timeout(tq);


    csr_data = rl(casr0);
    dprintf("CIPCA CASR: %x\n",p_args1(csr_data));
    if (csr_data & cixcd_psr_ic) {
      dprintf("CIPCA initialization complete\n",p_args0);
    } else {
      dqprintf("CIPCA did not initialize\n",p_args0);
      /*do_bpt();*/
    }

    wl(casrcr,1); /* Initialize the interrupt */

    if (pb->adapter_state != Disabled) {
	pprintf("CIPCA does not have PORT INIT COMPLETE\n");
	return msg_failure;
    }



/*	 
**  Modify ab now that port is disabled
**  init the NPORT queues.  Read the cpb to get node info.
**  adapter wrote the cpb.
*/
    node_addr = &pb->ab->cpb[0].xp_addr;
    pb->node_id = node_addr->addr.p_grp;

#if 0
    dump_n_apb(&(pb->ab->apb));
    dump_n_cpb(&(pb->ab->cpb[0]));
#endif
    pb->pb.num_sb = pb->ab->cpb[0].max_pgrp + 1;

    sprintf(pb->pb.port_name, "%s%s.%d.%d.%d.%d", pb->pb.protocol,
      ctl_id(pb->pb.controller, cletter), pb->node_id,
      pb->pb.channel, pb->pb.slot, pb->pb.hose);

    sprintf(pb->pb.info, "              Bus ID %d", pb->node_id);

    sprintf(pb->pb.alias, "pu%s", ctl_id(pb->pb.controller, cletter));

    dprintf("CIPCA is CI node %d type %x, max_pgrp %x, max_mem %x\n", p_args4(
      pb->node_id, pb->ab->apb.type, pb->pb.num_sb, pb->ab->cpb[0].max_mem));

    pb->ab->dqe_len = DFQE_SIZE;
    pb->ab->mqe_len = DFQE_SIZE;

    status = ci_alloc_nport(blk_ptr);
    if (status != msg_success) {
	pprintf("CIPCA failed to init nport queues %s\n", pb->pb.port_name);
	return msg_failure;
    }

/*	        
**  Init the buffer descriptor leaf table.
*/
    status = ci_alloc_bdlt(blk_ptr, &pb->ab->bdlt_base, &pb->ab->bdlt_len);
    if (status != msg_success) {
	printf("CIPCA failed to init BDLT %s\n", pb->pb.port_name);
	return msg_failure;
    }


    dprintf("Start port:  Port name: %s%c.%d.%d.%d.%d\n",
      p_args6(pb->pb.protocol, pb->pb.controller + 'a', pb->node_id,
      pb->pb.channel, pb->pb.slot, pb->pb.hose));

    fprintf(el_fp,"CIPCA %s is CI node %d\n",
      p_args2(pb->pb.port_name, pb->node_id));

#if 0
#if DEBUG || 1
    if(ci_debug_flag || 1){
      dump_ci_pb(pb);
      dump_nport_ab(pb->ab);
      dump_n_apb(&(pb->ab->apb));
      dump_n_cpb(&(pb->ab->cpb[0]));
      dump_bdl_ptr(pb->ab->bdlt_base);
      dump_bdl(pb->ab->bdlt_base->bdl_ptr);    
    };
#endif
#endif


    return msg_success;
}

/*+
 * ============================================================================
 * = cixcd_enable_port - enable the CIXCD port                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine disables the CIMNA adapter, reads the adapter block
 *	for adapter data.
 *  
 * FORM OF CALL:
 *  
 *	cixcd_enable_port( cixcd_blk_ptr )
 *  
 * RETURNS:
 *
 *	Success or failure (1 or 0).
 *       
 * ARGUMENTS:
 *
 *	struct cixcd_blk *blk_ptr - cixcd block data
 *
 * SIDE EFFECTS:
 *
 *	The CIMNA port is now ready for operation.
 *	Completion interrupts are enabled.
 *
-*/         
int cixcd_enable_port(struct cixcd_blk *blk_ptr)
{
    struct ci_pb *pb = blk_ptr->pb;
    int cixcd_completion_rcv();
    int csr_data;
    struct TIMERQ *tq, tqe;
    tq = &tqe;

/*
** ENABLED state 
** write enable control reg 
*/

    if (pb->adapter_state == Disabled) {
/* 
** create completion process for adapter 
*/
	blk_ptr->completion_pid = krn$_create(
					/* */
	  cixcd_completion_rcv, 	/* address of process   */
	  null_procedure, 		/* startup routine      */
	  &blk_ptr->cixcd_completion_compl_s, 
					/* completion semaphore */
	  6, 				/* process priority     */
	  1 << whoami(), 		/* cpu affinity mask    */
	  4096, 			/* stack size           */
	  "cipca_completion_rcv", 	/* process name         */
	  0, 0, 0, 0, 0, 0,		/* i/o 			*/
	  blk_ptr);

	dprintf("Created cipca_process: pid %x\n",
	  p_args1(blk_ptr->completion_pid));
            
	wl(pecr, 1);
    } else {
	pprintf("CIPCA in wrong state to be enabled\n");
	return msg_failure;
    }

    /* wait for entry to Enabled state */
    start_timeout(2000,tq);			/* 1 second timeout */
    while ( ((rl(casr0) & (cixcd_psr_ec|cixcd_psr_ic)) == 0 ) &&
                   (tq->sem.count==0) ) { 
     krn$_sleep(0);           /* Need blocking for timeout to work */
    }
    if (tq->sem.count) pprintf("Timed out waiting to enter ENABLED state\n");
    stop_timeout(tq);


    csr_data = rl(casr0);
    if (csr_data & (cixcd_psr_ec|cixcd_psr_ic) ) {
      pb->adapter_state = Enabled;
      if (ci_debug_flag)
        pprintf("Adapter is ENABLED\n",p_args0);
    } else {
      pprintf("Adapter did not ENABLE\n",p_args0);
      /*do_bpt();*/
    }

    wl(casrcr,1);
                              

    if (pb->adapter_state != Enabled) {
	pprintf("CIPCA does not have PORT ENABLED COMPLETE\n");
	return msg_failure;
    }

    if (ci_debug_flag)
      pprintf("CIPCA %s is CI node %d is ENABLED\n",
      p_args2(pb->pb.port_name, pb->node_id));

    return msg_success;
}

/*+
 * ============================================================================
 * = cixcd_stop_port - stop the CIXCD port                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine stops the CIXCD port by invoking a hardware reset.
 *  
 * FORM OF CALL:
 *  
 *	cixcd_stop_port( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct ci_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	The CIXCD is stopped.
 *
-*/
void cixcd_stop_port(struct ci_pb *pb)
{
#if 0

No min bit on CIPCA.   cipca_reset() is the recommend way to stop the port.
    int i;
    struct ppd_header *p;
    volatile struct cixcd_blk *blk_ptr;
    
    blk_ptr = pb->blk_ptr;
    dprintf("stop_port: pb = %x\n", p_args1(pb));

    pb->pb.state = DDB$K_STOP;		/* Change port driver state */

    wl(amcsr, cixcd_pmcsr_min);		/* Puts port in UNINITIALIZED state */
    krn$_sleep(100);
#endif
}

/*+
 * ============================================================================
 * = cixcd_shutdown_port - tear down all VCs on a port                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles Port VC shutdown.
 *  
 * FORM OF CALL:
 *  
 *	cixcd_shutdown_port( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct ci_pb *pb	- port block
 *
 * SIDE EFFECTS:
 *
 *	All VCs are closed.  All open connections are run down 
 *  	and all known units are deleted.
 *
-*/
void cixcd_shutdown_port(struct ci_pb *pb)
{
    int i;
    struct cipca_sb *sb;
    int status;

    dprintf("shutdown_port...\n", p_args0);

    /* 
     * Visit each SB. 
     **/

    for (i = 0; i < pb->pb.num_sb; i++) {

	if (i == pb->node_id)
	    continue;			/* Don't shutdown ourselves. */

	sb = pb->pb.sb[i];
	if (!sb) {
            dprintf("No sb to %x\n",p_args1(i));
	    continue;			/* no system block */
        }

	if (!sb->vc) {
            dprintf("No VC to %x \n",p_args1(i));
	    continue;			/* no vc established */
        } 

	dprintf("shutdown_port: break vc node %d\n", p_args1(i));

/*	 
**  Send_active is reset upon receipt of the cktset command from the
**  shutdown of the circuit with this sb.
*/

	krn$_wait(&scs_lock);
	sb->send_active = 1;
        dprintf("call ci_sa_ppd_shutdown_vc(%x)\n",p_args1(sb));
	ci_sa_ppd_shutdown_vc(sb);
	krn$_post(&scs_lock);


/* wait for all sent packets to be returned */
	sb->poll_delay2 = 30;		/* 15 sec ( 30 * 500) */
	while (sb->poll_delay2 != 0) {
	    if (sb->send_active == 0){
                dprintf("sb->send_active went to 0\n",p_args0);
		break;
            }
	    sb->poll_delay2--;
	    krn$_sleep(500);
	}

	if (sb->poll_delay2 == 0)
	    dprintf("CIPCA shutdown send timeout %d\n", p_args1(i));

	dqprintf("shutdown_port: complete node %d\n", p_args1(i));

    }

    dprintf("shutdown_port: complete\n", p_args0);
}




/*+
 * ============================================================================
 * = cipca_poll_ints - used in POLLED mode                                    =
 * ============================================================================
 *
-*/
void cipca_poll_ints(struct cixcd_blk *blk_ptr)
{
    int value;
    int csr_data;
    int completion;
    struct ci_pb *pb = blk_ptr->pb;

    if ( rl(nodests) & 0x10 ) {
	pb->interrupt = 1;		/* notify receive process */
        value = rl(casr0);
	completion = value & cipca_casr_completion;
	if (completion || (value==0)) {
   	  krn$_post(&blk_ptr->cixcd_completion_s);
	} else {
	  krn$_post(&blk_ptr->cixcd_misc_s);
	}
    }

}



/*+
 * ============================================================================
 * = cixcd_interrupt - service a port interrupt                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine services port interrupts.  It posts the ISR semaphore,
 *	and also posts the receive semaphore if a new response is available.
 *	It then acknowledges the port's interrupt, allowing the port to
 *	interrupt again as needed.
 *  
 * FORM OF CALL:
 *  
 *	cixcd_interrupt( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct ci_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	Waiting tasks may be awoken.
 *
-*/
void cixcd_interrupt(struct ci_pb *pb)
{
    int value;
    struct cixcd_blk *blk_ptr;
    int completion;


    blk_ptr = pb->blk_ptr;
    value = rl(casr0);
    io_issue_eoi(pb,pb->pb.vector);

    if(pb->adapter_state != Enabled){
        pb->interrupt = 0;
        io_issue_eoi(pb,pb->pb.vector);
        return;
    }

    if (pb->pb.state == DDB$K_INTERRUPT) {
        pb->interrupt = 1;              /* notify receive process */

        if (value&0x8000003f){
          pprintf("CIPCA_interrupt casr0:%x  casr1:%x  posting misc\n",
                                                        value, rl(casr1) );
          krn$_post(&blk_ptr->cixcd_misc_s);
        }

        completion = value & cipca_casr_completion;
        if (completion || (value==0)) {
          krn$_post(&blk_ptr->cixcd_completion_s);
        }

    } else
        pprintf("CIPCA erroneous interrupt, psr = %08x\n", p_args1(value));
}


/*
 * cixcd_write_pcq0cr
 */
void cixcd_write_pcq0cr(struct ci_pb *pb)
{
    cipca_write_csr(pb, ccq0ir, 1);
}




#if !(TURBO || RAWHIDE)
int cipca_init(){
  int cipca_init_pb();
  struct EVNODE *evp, evn;		/*Pointer to the evnode,evnode*/
  char c;

#if 0
  extern int print_debug_flag;
  int zerocheck();
  print_debug_flag = 1;


   pprintf("Launching zero checker\n");

         krn$_create( 
                 zerocheck,	        	/* address of process */
                 null_procedure,                /* startup routine */
		 0,   /* completion semaphore */
                 5,                             /* process priority */ 
                 1<<whoami(),			/* cpu affinity */
                 4096,				/* stack size */
	         "zerocheck",		/* process name */
                 0, 0, 0, 0, 0, 0,              /* i/o */
                 0 );			/* parameter */

#endif




#if MODULAR
    ovly_load("NPORT");
#endif

  evp = &evn;
  if (ev_read("os_type", &evp, 0) != msg_success) 
    {
     fprintf(el_fp, "cipca driver failed to read os_type environment variable\n");
    }
  if (strcmp(evp->value.string,"OpenVMS") != 0)
    {
     fprintf(el_fp, "os_type: %s - console CIPCA driver not started\n",
                                                           evp->value.string);
     return msg_success;
    }

#if DEBUG
  cipca_cnt = 0;
#endif
  find_pb("cipca", sizeof(struct ci_pb), cipca_init_pb);


}

void cipca_init_pb(struct pb *pb){
   struct device *cipca_dev;
   char name[16];

   log_driver_init(pb);
#if DEBUG
   ci_pbs[cipca_cnt] = pb;
   cipca_cnt++;
#endif

   /*
    *  Make a TURBO-like device structure
    */
    cipca_dev = (struct device *)malloc_z(sizeof(struct device));
    cipca_dev->slot = pb->slot;
    cipca_dev->hose = pb->hose;
    cipca_dev->function = pb->function;
    cipca_dev->bus = pb->bus;
    cipca_dev->devdep.io_device.controller = pb->controller;
    cipca_dev->devdep.io_device.vector[0] = pb->vector;
    cipca_dev->devdep.io_device.devspecific = 0;
    cipca_dev->devdep.io_device.devspecific2 = (int)pb;
    cipca_alloc(pb);
    cixcd_setmode(DDB$K_START,cipca_dev);

   
    krn$_sleep(5000);

    force_path_idreqs(pb,0);



}
#endif


#if TURBO || RAWHIDE
void cipca_init(struct device *cipca_dev)
{
   struct pb *pb;
   cipca_d = cipca_dev;
   cipca_dev->devdep.io_device.vector[0] = pci_get_vector(cipca_dev);
   cipca_dev->devdep.io_device.vector[1] = pci_get_vector(cipca_dev)+1;
   pb = malloc_z(sizeof(struct ci_pb));
   pb->hose = cipca_dev->hose;
   pb->slot = cipca_dev->slot;
   pb->function = cipca_dev->function;
   pb->bus = cipca_dev->bus;
   pb->vector = pci_get_vector(cipca_dev);
   cipca_dev->devdep.io_device.devspecific = 0;
   cipca_dev->devdep.io_device.devspecific2 = (int)pb;
}
#endif

void cipca_alloc(struct ci_pb *pb){
    pb->N_buffer_blk = malloc_az(N_Q_BUFFER_POOL_SZ, 64, 0);
    pb->N_carrier_blk = malloc_z(N_CARRIER_POOL_SZ);
    pb->bdlt_ptr_blk = (BDL_PTR *) malloc_az(N_BDLT_MAX_SIZE, HWRPB$_PAGESIZE, 0);
    pb->bdl_blk = (BDL *) malloc_az(N_BDL_MAX_SIZE, HWRPB$_PAGESIZE, 0);
}

#if TURBO || RAWHIDE
void cipca_dealloc(struct ci_pb *pb){
    free(pb->N_buffer_blk);
    free(pb->N_carrier_blk);
    free(pb->bdlt_ptr_blk);
    free(pb->bdl_blk);
}
#endif

#if 0
void zerocheck(){

    int i;
    int *addr;
    int error = 0;
    int fix = 0;
    /* loop forever unless broken out */
    while(1) {

	for (i = 0; i < 0x100; i+=4) {
	    addr = i;

	    /* check for 0s */
	    if (*(addr) != 0x0) {
		pprintf("addr %x : %x\n",addr, *(addr));
		if (fix) {
		     *(addr) = 0x0;
		} else {
		    error = 1;
		    break;
		}
	    }
	}
	if (error) break;

	krn$_sleep (1);			/* allow other processes to run */

	/* See if a ^c was entered */
	check_kill();

    }
}
#endif


void cipca_reset(struct cixcd_blk *blk_ptr){
  struct TIMERQ *tq, tqe;
  Z_NODESTS ndsts;
  tq = &tqe;


  wl(adprst,0xdeadbeef);
  ndsts.i = rl(nodests);

  ndsts.i = 0 ;  /* dbg */

  start_timeout(10000,tq);			/* 10 second timeout */
  while ( !(ndsts.f.adpsta==INIT) && (tq->sem.count==0) ) { 
     ndsts.i = rl(nodests);
     krn$_sleep(0);           /* Need blocking for timeout to work */
  }

  if (tq->sem.count) pprintf("Timed out waiting for INIT after adprst.  NODESTS : %x\n", ndsts.i);
  stop_timeout(tq);
}


/* wl_intena - write the interrupt enable register.  This is done 
 * differently depending on which CIPCA hardware is in use.    
 * If MREV<MODFLG(0)> is set, then the Alternate Interrupt hardware is
 * present on the hardware and MUST be used.  If the MODFLG(0) bit is
 * clear, then the on-chip interrupt mechanism MUST be used.
 */
void wl_intena(struct cixcd_blk *blk_ptr, int value) {

  Z_MREV_HW rev_hw;
  rev_hw.i = rl(mrev_hw);

  if (rev_hw.f.modflg & 0x1) {
     wl(alt_intena,value);
  } else {
     wl(intena,value);
  }
}




/*+
 * ============================================================================
 * = ctl_id - convert a unique controller number to an ID       =
 * ============================================================================
 *
 * OVERVIEW:
 * 
 *	Converts the unique numeric representation of a controller to a
 *	controller ID (string).  The caller provides the string buffer.
 *
 * FORM OF CALL:
 * 
 *	ctl_id(num, id);
 * 
 * RETURNS:
 *
 *	Pointer to the caller's string buffer.
 *
 * ARGUMENTS:
 *
 *	int num			- Controller number.
 *	char *id		- Controller ID string buffer.
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
char *ctl_id(int num, char *id)
{
#if TURBO || RAWHIDE


    return controller_num_to_id(num,id);

#else

    id[0] = 'a'+num;
    id[1] = '\0';
    return id;	

#endif
}


#if DEBUG
int scan_pb( )
    {
    int i;
    struct pb *pb;


    for( i = 0; i < num_pbs; i++ )
	{
	  pb = pbs[i];
          pprintf("pb at %x:\n",pb);
	  dump_pb(pb);
	  pprintf("\n");
	}

    return( NULL );
    }





/*+
 * ============================================================================
 * = dump_cb - Display a connection block                                     =
 * ============================================================================
 *
 * OVERVIEW:
 * 
 *	Display a connection block
 * 
 * FORM OF CALL:
 * 
 *	dump_cb(cb)
 * 
 * RETURNS:
 *
 *	Nothing.
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/
void dump_cb(struct cb *cb)
{
#define $x(y) pprintf("y %08x\n",y);

    $x(cb->cb.flink);
    $x(cb->cb.blink);
    $x(cb->state);
    $x(cb->ref);
    $x(cb->sb);
    $x(cb->alloc_appl_msg);
    $x(cb->map);
    $x(cb->send_appl_msg);
    $x(cb->send_data);
    $x(cb->request_data);
    $x(cb->unmap);
    $x(cb->disconnect);
    $x(cb->break_vc);
    $x(cb->discard_cb);
    $x(cb->appl_msg_input);
    $x(cb->connection_error);
    $x(cb->driver);
    $x(cb->timeout);
    $x(cb->state);
    $x(cb->dst_id);
    $x(cb->credits);
}

#endif

#if 0
void cipca_shutdown(){
   cixcd_setmode(DDB$K_STOP,cipca_d);
}

#endif


#if !(TURBO || RAWHIDE)

void cipca_mscp_destroy(struct ci_pb *cipb) {
#include "cp$src:mscp_def.h"
    extern struct QUEUE mscp_cbs;
    struct mscp_cb *p;
    struct cb *cb;
    struct cipca_sb *sb;
    struct pb *pb;
    struct pb *d = &cipb->pb;

/*
 * If this is a connection to the desired device, then remove it from the queue.
 * Otherwise, ignore it.
 */
   krn$_wait(&scs_lock);
   for( p = mscp_cbs.flink; p != &mscp_cbs.flink; p = p->mscp_cb.flink )
   {	struct mscp_cb *q;
	cb = p->cb;
	sb = cb->sb;
	pb = sb->pb;

#if 0
	pprintf("mscp_destroy_poll: p %x flink %x cb %x sb %x pb %x\n",
	  p_args5(p, mscp_cbs.flink, cb, sb, pb));
#endif
	if ((pb->slot == d->slot) && 
	    (pb->hose == d->hose) &&
	    (pb->bus  == d->bus ) ){
#if 0
	    pprintf("mscp_destroy_poll: remq p = %x  slot:%d hose:%d bus:%x\n", 
                        p_args4(p,d->slot,d->hose,d->bus));
#endif
	    krn$_semrelease(&cb->receive_s);
	    krn$_semrelease(&cb->control_s);
	    krn$_semrelease(&cb->credit_s);
	    q = p;
	    p = p->mscp_cb.blink;
	    remq(q);
	    free(q);
	    free(cb);
	}
    }
    krn$_post(&scs_lock);
}
#endif

#if DEBUG
void dqi_enable_all(int n)
{
    int i, status;
    struct cipca_sb *sb;
    for (i = 0; i < ci_pbs[n]->pb.num_sb; i++) {
        sb = ci_pbs[n]->pb.sb[i];
        sb->dqi = 0;                    /* do not inhibit datagrams*/
        if (i == ci_pbs[n]->node_id) {
            continue;                   /* Don't poll ourselves. */
        }
        krn$_wait(&scs_lock);
        ci_sa_ppd_dqi_off(sb);
        krn$_post(&scs_lock);
    }
}


void dadfq_refill(int n)
{
    int i;
    N_CARRIER *car_ptr = 0;             /* Queue Carrier */
    N_Q_BUFFER *nbuf = 0;               /* Queue Buffer */
    struct ci_pb *pb = ci_pbs[n];

    krn$_wait(&scs_lock);
    for (i = 0; i < N_ADAP_DG_REFILL; i++) {
        ci_remove_queue(DDFQ, pb, &car_ptr, &nbuf);
        ci_insert_queue(DADFQ, pb, car_ptr, nbuf);
    }
    krn$_post(&scs_lock);
}






/*
 * This routine is for use from Xdelta at high ipl
 * Read any adapter responses.  Loop until queue is exhausted.
 */
void dump_adrq(int n)
{
    extern int enable_trace_flag;
    extern int print_trace_flag;
    int i = 0;
    N_CARRIER *car_ptr;
    N_Q_BUFFER *p;
    int saved_enable_trace_flag = enable_trace_flag;
    int saved_print_trace = print_trace_flag;

    enable_trace_flag = 1;
    print_trace_flag= 1;

    p = 0;
    car_ptr = ci_pbs[n]->ab->adrq.head_ptr;  /* Return carrier or stopper */

    while (1) {

      if (((u_int) car_ptr->next_ptr & 1) == DRIVER_STOPPER) {
        pprintf("stopper\n", p_args0);
      i++;
      p = car_ptr->q_buf_token; /* Return queue buffer (virtual) */
      pprintf("%d ",i);
      trace_np('D',ci_pbs[n],p);
        enable_trace_flag = saved_enable_trace_flag;
        print_trace_flag = saved_print_trace;
        return;           	/* Is a stopper; no queue buffer */
      }

      /* Not a stopper */

      i++;
      p = car_ptr->q_buf_token; /* Return queue buffer (virtual) */
      pprintf("%d ",i);
      trace_np('D',ci_pbs[n],p);
      car_ptr = car_ptr->next_ptr;      /* Update head link */
    }
}


void do_eoi(){
  volatile struct ci_pb *pb = cipca_blk_ptr->pb;
  io_issue_eoi(pb,pb->pb.vector);
}

void write_casrcr(){
  volatile struct ci_pb *pb = cipca_blk_ptr->pb;
#define blk_ptr cipca_blk_ptr
  wl(casrcr, 1);
}


#endif

#if DEBUG
void _dprintf(char *fmt, protoargs int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
    if ((ci_debug_flag & 0x101) == 0x101)
	qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    else if ((ci_debug_flag & 1) != 0)
	printf(fmt, a, b, c, d, e, f, g, h, i, j);
}
void _d2printf(char *fmt, protoargs int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
    if ((ci_debug_flag & 0x102) == 0x102)
	qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    else if ((ci_debug_flag & 2) != 0)
	printf(fmt, a, b, c, d, e, f, g, h, i, j);
}
void _dqprintf(char *fmt, protoargs int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
    if (ci_debug_flag)
	_dprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    else
	qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
}
#endif

