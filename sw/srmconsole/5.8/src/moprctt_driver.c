/*
 * Copyright (C) 1991 by
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
 *
 *
 * Abstract:	MOP Remote Console terminal port driver.
 *
 *	This module implements the MOP Remote Console terminal functions.
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	08-Apr-1991	Initial entry.
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:inet.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:tt_def.h"
#include "cp$src:mop_def.h"
#include "cp$src:mb_def.h"

volatile struct moprctt_pb
{
    struct TTPB *ttpb;
    struct MB *dl;
    struct POLLQ pollq;
} ;

#include "cp$inc:prototypes.h"

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

extern int spl_kernel;
extern struct QUEUE pollq;

void moprctt_rxoff( struct moprctt_pb *pb )
    {
    }

void moprctt_rxon( struct moprctt_pb *pb )
    {
    }

int moprctt_rxready( struct moprctt_pb *pb )
    {
    return((((pb->dl->rc.rx_index_in%MOP$K_CH_IN_MAX)-
	      pb->dl->rc.rx_index_out)%
	      MOP$K_CH_IN_MAX)>0);
    }

int moprctt_rxread( struct moprctt_pb *pb )
    {
    return( moprc_in( pb->dl ) );
    }

void moprctt_txoff( struct moprctt_pb *pb )
    {
    }

void moprctt_txon( struct moprctt_pb *pb )
    {
    }

int moprctt_txready( struct moprctt_pb *pb )
    {
    return(((pb->dl->rc.tx_index_in%MOP$K_CH_OUT_MAX)-
	     pb->dl->rc.tx_index_out)%MOP$K_CH_OUT_MAX < 
	     (MOP$K_CH_OUT_MAX-1));
    }

void moprctt_txsend( struct moprctt_pb *pb, char c )
    {
    moprc_out( pb->dl, c );
    }

int moprctt_init( struct INODE *ip )
    {
    char name[MAX_NAME];
    int tt_poll( );
    struct moprctt_pb *pb;
    struct TTPB *ttpb;


    pb = malloc( sizeof( *pb ),pb );
    pb->dl = ((struct NI_GBL *)ip->misc)->mbp;
    sprintf( name, "rc%2.2s", ip->name + 2 );

    ttpb = tt_init_port (name);
    ttpb->rxoff = moprctt_rxoff;
    ttpb->rxon = moprctt_rxon;
    ttpb->rxready = moprctt_rxready;
    ttpb->rxread = moprctt_rxread;
    ttpb->txoff = moprctt_txoff;
    ttpb->txon = moprctt_txon;
    ttpb->txready = moprctt_txready;
    ttpb->txsend = moprctt_txsend;
    ttpb->port = pb;
    ttpb->perm_mode = DDB$K_INTERRUPT;
    ttpb->perm_poll = 0;
    spinunlock (&spl_kernel);
    tt_setmode_pb (ttpb, DDB$K_INTERRUPT);
    pb->ttpb = ttpb;
    
    pb->pollq.routine = tt_poll;
    pb->pollq.param = pb->ttpb;
    insq_lock( &pb->pollq.flink, &pollq );

    return (msg_success);
    }

