/*
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 *
 *
 * Abstract:	HWRPB TT terminal port driver.  This driver it used at boot
 *		time by secondary processors to receive console commands
 *		(e.g. START) from the operating system.
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	16-Oct-1992	Initial entry.
 */

#include "cp$src:platform.h"
#include "cp$src:stddef.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:common.h"
#include "cp$src:msg_def.h"
#include "cp$src:tt_def.h"
#include "cp$src:hwrpb_def.h"
#include "cp$inc:kernel_entry.h"

struct hwrpbtt_pb {
    int id;
    struct HWRPB *hwrpb;
    struct SLOT *slot;
    char *rxbuf;
    int rxip;
    char *txbuf;
    int txip;
    } ;

#include "cp$inc:prototypes.h"

#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define free(x) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)

extern int primary_cpu;
extern struct HWRPB *hwrpb;
extern int shell_stack;
extern int shell_startup ();
extern sh( );
extern null_procedure( );
extern struct LOCK spl_kernel;

extern struct TTPB *hwrpbtt_ttpbs[];

void hwrpbtt_rxoff( struct hwrpbtt_pb *pb )
    {
    }

void hwrpbtt_rxon( struct hwrpbtt_pb *pb )
    {
    }

int hwrpbtt_rxready( struct hwrpbtt_pb *pb )
    {
    char *c;

    if( !pb->rxip )
	if( ( *pb->hwrpb->RXRDY & ( 1 << pb->id ) ) && pb->slot->RXLEN )
	    {
	    pb->rxbuf = pb->slot->RX;
	    pb->rxip = 1;
	    pb->rxbuf[pb->slot->RXLEN-2] = 0;
	    }
    return( pb->rxip );
    }

int hwrpbtt_rxread( struct hwrpbtt_pb *pb )
    {
    char c;

    c = *pb->rxbuf++;
    if( c == 0 )
	{
	c = 13;
	pb->rxip = 0;
	pb->rxbuf = pb->slot->RX;
	*pb->rxbuf = 0;
	pb->slot->RXLEN = 0;
	mb( );
	_bbcci( pb->id, pb->hwrpb->RXRDY );
	mb( );
	}
    return( c );
    }

void hwrpbtt_txoff( struct hwrpbtt_pb *pb )
    {
    }

void hwrpbtt_txon( struct hwrpbtt_pb *pb )
    {
    }

int hwrpbtt_txready( struct hwrpbtt_pb *pb )
    {
    if( !pb->txip )
	if( !( *pb->hwrpb->TXRDY & ( 1 << pb->id ) ) )
	    {
	    pb->txbuf = pb->slot->TX;
	    pb->txip = 1;   
	    } 
    return( pb->txip );
    }

void hwrpbtt_txsend( struct hwrpbtt_pb *pb, char c )
    {
    *pb->txbuf++ = c;
    if( c == 10 )
	{
	sprintf( pb->txbuf, "P%02d>>>", pb->id );
	pb->txip = 0;
	pb->slot->TXLEN = (int)pb->txbuf - (int)pb->slot->TX + 6;
	mb( );
	_bbssi( pb->id, pb->hwrpb->TXRDY );
	mb( );
	mtpr_ipir( primary_cpu );
	}
    }

int hwrpbtt_init( int id )
    {
    char name1[16];
    char name2[16];
    char **shv;
    struct hwrpbtt_pb *pb;
    struct TTPB *ttpb;

    pb = malloc( sizeof( *pb ) );
    pb->hwrpb = hwrpb;
#if TURBO
    pb->id = real_slot( id );
#else
    pb->id = id;
#endif
    pb->slot = (int)pb->hwrpb
	    + *pb->hwrpb->SLOT_OFFSET
	    + *pb->hwrpb->SLOT_SIZE * pb->id;
    pb->txip = 0;
    pb->rxip = 0;
    sprintf( name1, "hwrpbtta%d", id );	
    ttpb = tt_init_port( name1 );
    ttpb->port = pb;
    ttpb->rxoff = hwrpbtt_rxoff;
    ttpb->rxon = hwrpbtt_rxon;
    ttpb->rxready = hwrpbtt_rxready;
    ttpb->rxread = hwrpbtt_rxread;
    ttpb->txoff = hwrpbtt_txoff;
    ttpb->txon = hwrpbtt_txon;
    ttpb->txready = hwrpbtt_txready;
    ttpb->txsend = hwrpbtt_txsend;
    ttpb->perm_mode = DDB$K_POLLED;
    ttpb->perm_poll = 3;
    ttpb->poll = 3;
    spinunlock( &spl_kernel );
    tt_setmode_pb( ttpb, DDB$K_POLLED );
    hwrpbtt_ttpbs[id] = ttpb;
    sprintf( name2, "shell_%d", id );
    shv = malloc( 2 * sizeof( char * ) );
    shv[0] = mk_dynamic( "shell" );
    shv[1] = 0;
    krn$_create( sh, shell_startup, 0, 3, 1<<id, shell_stack, name2,
	    name1, "r", name1, "w", "tt", "w", 1, shv );
    return( msg_success );
    }

void hwrpb_comm_flush( )
    {
    int i;
    struct hwrpbtt_pb *pb;
    struct TTPB *ttpb;

    for( i = 0; i < MAX_PROCESSOR_ID; i++ )
	{
	ttpb = hwrpbtt_ttpbs[i];
	if( ttpb )
	    {
	    pb = ttpb->port;
	    if( *pb->hwrpb->TXRDY & ( 1 << pb->id ) )
		{
		pb->slot->TX[pb->slot->TXLEN] = 0;
#if TURBO
		if (*(int *) (0x200004) != 'TLR0')
#endif
		pprintf( "\n%s\n", pb->slot->TX );
		mb( );
		_bbcci( pb->id, pb->hwrpb->TXRDY );
		mb( );
		}
	    }
	}
    }

void hwrpb_tt_flush( int id )
    {
    struct hwrpbtt_pb *pb;
    struct TTPB *ttpb;

    ttpb = hwrpbtt_ttpbs[id];
    if( ttpb )
	{
	pb = ttpb->port;
	pb->slot->TX[pb->slot->TXLEN] = 0;
	mb( );
	_bbcci( pb->id, pb->hwrpb->TXRDY );
	mb( );
	pb->slot->TXLEN = 0;
	pb->txip = 0;
	pb->rxip = 0;
	}
    }
