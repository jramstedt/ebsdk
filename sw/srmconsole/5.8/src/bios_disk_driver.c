/*
 * Copyright (C) 1993 by
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
 * Abstract:	BIOS disk driver
 *
 * Author:	Kevin LeMieux
 *
 * Modifications:
 *
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:prototypes.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:eisa.h"
#include "cp$src:scsi_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:probe_io_def.h"
#include "cp$inc:platform_io.h"
#include "cp$src:platform_cpu.h"
#include "cp$src:ev_def.h"
#include "cp$inc:kernel_entry.h"
#if MODULAR
#include "cp$src:mem_def.h"
#endif

#define BLOCK_SIZE 512

extern null_procedure( );
extern common_driver_open( );
int bios_restart_flag;

volatile struct bios_disk_pb {
    struct pb pb;
    struct SEMAPHORE owner_s;
};

extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
extern int ETABiosResident;

int bios_disk_read( );
int bios_disk_write( );
void bios_disk_setmode( struct bios_disk_pb *pb, int mode );

struct DDB bios_disk_ddb = {
	"dy",			/* how this routine wants to be called	*/
	bios_disk_read,		/* read routine				*/
	bios_disk_write,	/* write routine			*/
	common_driver_open,	/* open routine				*/
	null_procedure,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	1,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};

struct DDB *bios_disk_ddb_ptr = &bios_disk_ddb;

struct pb *bootbios_pb = 0;

int bios_disk_init( ) {
    struct pb *pb;
    int j;

    if ( bios_disk_pb( &pb, sizeof( struct bios_disk_pb ))) {
	open_bios( pb );
	bios_disk_init_pb( pb  );
	bootbios_pb = pb;
    }
    return( msg_success );
}

int bios_disk_pb( struct pb **ppb, int size ) {

    struct EVNODE evp_bootbios;
    struct EVNODE *eb = &evp_bootbios;
    int i;
    int valid = FALSE;
    struct pb *pb;
    struct pb *pb_match;
    char *name;
 
    if ( ev_read( "bootbios", &eb, EV$K_SYSTEM ) == msg_success ) {
	if ( *eb->value.string ) {
	    pb_match = pb_name_match( eb->value.string );
	    if ( pb_match ) {
		name = eb->value.string;
		if ( bootbios_validate( &name ) == msg_success ) {
		    krn$_wait( &pbs_lock );
		    for( i = 0; i < num_pbs; i++ ) {
			pb = pbs[i];
			if ( pb == pb_match ) {
			    pb = realloc_noown( pb, size );
			    pbs[i] = pb;
			    *ppb = pb; 
			    valid = TRUE;
			    break;
			}
		    }
		    krn$_post( &pbs_lock );
		}
	    }
	}
    }
    return ( valid );
}


struct pb *bootbios_validate( char **name ) {

    U32 id;
    struct pb *pb;
    int status = msg_success;
    int controller_ok = FALSE;
    int heap_ok = FALSE;

    if ( **name ) {
	pb = pb_name_match( *name );
	if ( pb ) {
	    id = incfgl( pb, 0x00 );
	    if ( bios_rom_id_valid( id )) {
		controller_ok = TRUE;
		heap_ok = bios_heap_check();
		if ( !heap_ok )
		    pprintf("\ninsufficient console heap to run bootbios (%s)\n", *name );
	    }
	}
	if ( !controller_ok ) 
	    pprintf("\nbootbios does not map to a valid controller\n");
	if (!( controller_ok && heap_ok )) 
	    status = msg_failure;
    }
    return ( status );
}


int bios_heap_check() {

#define EV_HEAP_EXPAND 0x18
#define EV_HEAP_EXPAND_VALID 0x19
#define MAX_HEAP_EXPAND 4096

    int heap_ok = FALSE;
    char *heap;
    int size;
    int heap_work;
    extern struct set_param_table ev_heap_table[];
   
    /*
     * If the boot BIOS is already resident then we have already allocated 
     * the necessary heap.
     */

    if ( !ETABiosResident ) {
	heap_work = rtc_read ( EV_HEAP_EXPAND );
	if ((( -heap_work ) &0xff ) == rtc_read( EV_HEAP_EXPAND_VALID )) {
	    
	    /*
	     * For the emulator we need about 750KB. Try to allocate this plus 
	     * another half MB for headroom. If this fails then tell the user to 
	     * increase the heap using heap_expand. If we're already at the 
	     * expand maximum then just check if we have 750KB.
	     */

	    if (( ev_heap_table[heap_work].code ) == MAX_HEAP_EXPAND )
		size = 750 * 1024;
	    else
		size = 750 * 1024 + 512 * 1024;

	    heap = dyn$_malloc (size, DYN$K_SYNC | DYN$K_NOOWN | DYN$K_NOWAIT, 0, 0, 0);
	
	    if ( heap ) {
		free ( heap );
		heap_ok = TRUE;
	    }
	}
    } else {
	heap_ok = TRUE;
    }
    return ( heap_ok );
}

void bios_disk_init_pb( struct bios_disk_pb *pb ) {

    INT capacity = 0;
    int index = 0;
    int i;

    pb->pb.setmode = bios_disk_setmode;
    if( pb->pb.vector ) {
	pb->pb.mode = DDB$K_INTERRUPT;
	pb->pb.desired_mode = DDB$K_INTERRUPT;
    } else {
	pb->pb.mode = DDB$K_POLLED;
	pb->pb.desired_mode = DDB$K_POLLED;
    }
    set_io_name( pb->pb.name, "py", 0, 0, pb );
    krn$_seminit( &pb->owner_s, 1, "bios_disk_owner" );
    krn$_wait( &pb->owner_s );

    pb->pb.sb = malloc_noown( 32 * sizeof( struct sb * ) );
    pb->pb.num_sb = 32;

    for( i = 0; i < pb->pb.num_sb; i++ ) {
	if( bios_disk( i, &capacity ))
	    bios_disk_new_unit( pb, index++, i, capacity );
    }
    krn$_post( &pb->owner_s );
}

void bios_disk_new_unit( struct bios_disk_pb *pb, int index, int unit, 
						    INT capacity)
    {
    char *status;
    char *raid;
    char name[32];
    int i;
    struct sb *sb;
    struct ub *ub;
    struct INODE *inode;

    sb = malloc_noown( sizeof( *sb ) );
    pb->pb.sb[index] = sb;
    sb->pb = pb;
    strcpy( sb->name, pb->pb.name );
    ub = malloc_noown( sizeof( *ub ) );
    ub->sb = sb;
    ub->pb = pb;
    ub->unit = unit;
    /*
     *  Create a new "file".
     */
    set_io_name( name, bios_disk_ddb_ptr->name, ub->unit, 0, pb );
    allocinode( name, 1, &inode );
    inode->dva = &bios_disk_ddb;
    inode->attr = ATTR$M_READ;
    *(INT *)inode->len = (INT) capacity;
    inode->bs = BLOCK_SIZE;
    /*
     *  Set up required pointers and back-pointers.  The INODE points to the
     *  UB, the UB points to the SB, the SB points to the UB.
     */
    inode->misc = ub;
    ub->inode = inode;
    sb->first_ub = ub;
    INODE_UNLOCK( inode );
    sprintf( ub->alias, "%s%c%d (BIOS)",
	     strupper(bios_disk_ddb_ptr->name), pb->pb.controller + 'A',
	     ub->unit );
    sprintf( ub->string, "%-24s   %-8s   %24s  %s",
	    name, ub->alias, ub->info, ub->version );
    insert_ub( ub );
#if 0
    pprintf( "%-18s %-8s  %20s  %5s\n", ub->inode->name, ub->alias, ub->info, ub->version );
#endif
}

int bios_disk_read( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    return( bios_disk_transfer( fp, ilen * inum, c, 0 ) );
    }

int bios_disk_write( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    return( bios_disk_transfer( fp, ilen * inum, c, 1 ) );
    }

int bios_disk_transfer( struct FILE *fp, int len, unsigned char *c, int
opcode )
    {
    int lbn;
    int t;
    struct INODE *inode;
    struct ub *ub;

    /*
     *  The file is open.  The MISC field in the INODE structure is used
     *  to hold a UB pointer.
     */
    lbn = *(INT *)fp->offset / BLOCK_SIZE;
    inode = fp->ip;
    ub = inode->misc;
    /*
     *  Don't let transfers go past the end of the unit.
     */
    if( *(INT *)fp->offset + len > *(INT *)inode->len )
	{
	fp->status = msg_eof;
	fp->count = 0;
	t = 0;
	}
    /*
     *  Make sure that the transfer respects block boundaries.
     */
    else if( ( ( len & 511 ) == 0 ) && ( ( *(INT *)fp->offset & 511 ) == 0 ) )
	{
	t = bios_disk_read_write( ub->unit, c, len, lbn, opcode );
	fp->status = t;
	fp->count = len;
	if( t == msg_success )
	    t = len;
	else
	    t = 0;
	/*
	 *  Update the file offset.
	 */
	*(INT *)fp->offset += t;
	}
    else
	{
	err_printf( msg_bad_request, inode->name );
	fp->status = msg_bad_request;
	fp->count = 0;
	t = 0;
	}
    /*
     *  At this point we have kept track of the number of bytes successfully
     *  transferred.  Return that number.
     */
    return( t );
    }


void bios_disk_setmode( struct bios_disk_pb *pb, int mode )
    {

    switch( mode )
	{
	case DDB$K_STOP:
	    if( pb->pb.mode != DDB$K_STOP )
		{
		krn$_wait( &pb->owner_s );

		if( pb->pb.mode == DDB$K_INTERRUPT )
		    io_disable_interrupts( pb, pb->pb.vector );
		ETABiosSG( FALSE );
		pb->pb.mode = DDB$K_STOP;
		}
	    break;

	case DDB$K_START:
	    if( pb->pb.mode == DDB$K_STOP )
		{
		if ( bios_restart_flag ) {
		    close_bios( pb );
		    open_bios( pb );
		    bios_restart_flag = 0;
		} else {
		    ETABiosSG( TRUE );
		}
		pb->pb.mode = pb->pb.desired_mode;
		krn$_post( &pb->owner_s );
		}
	    break;

	case DDB$K_READY:
	    break;	    

#if 0
	case DDB$K_POLLED:
	    if( pb->pb.mode == DDB$K_STOP )
		{
		pb->pb.mode = DDB$K_POLLED;
		krn$_post( &pb->owner_s );
		}
	    break;
#endif
	}
    }
