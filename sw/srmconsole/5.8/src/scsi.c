/*
 * Copyright (C) 1990 by
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
 * Abstract:	SCSI class driver
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	kl	01-Oct-1993	For driver shutdown always malloc a ub
 *				in scsi_new_unit.
 *
 *	kl	19-Aug-1993	merge
 *	
 *	jeg	 -Mar-1993	Implement mk_write.
 *
 *	pel	22-Dec-1992	Copy inquiry response data to ub for use in
 *				FRU table. Use malloc instead of malloc_z.
 *
 *	jeg	07-Dec-1992	last byte to be read/written must be < length
 *				of file 
 *
 *	ajb	08-Aug-1992	remove duplicate qprintf/err_printf constructs
 *				now that err_printf also goes to the event
 *				logger.  Comment out "failed to send command"
 *				message for now so that the event logger isn't
 *				filled up.
 *
 *	ajb	11-Dec-1991	can't have have message numbers in case
 *				statements, since they aren't compile time
 *				constants.
 *
 *	sfs	21-Nov-1991	Back out the last edit.
 *
 *	tga	13-Aug-1991	Add check for writable device in
 *				scsi_new_unit()
 *
 *	kl	22-Aug-1991	Add dk_setmode(DDB$K_STOP) for ADU to 
 *				support console reentry boot.
 *
 *	tga	13-Aug-1991	Add check for writable device in
 *				scsi_new_unit()
 *
 *	pel	12-Mar-1991	let fopen/close incr/decr inode ref & collapse
 *				wrt_ref, rd_ref into just ref.
 *
 *	sfs	19-Feb-1991	Fix calls to ALLOCINODE.  Remove unneeded code
 *				in FD_OPEN.  Use common routines to manipulate
 *				reference counts.  Make INODES write protected
 *				by default.
 *
 *	sfs	06-Feb-1991	Implement dk_write.
 *
 *	ajb	04-Feb-1991	Change calling sequence to xx_read, xx_write
 *
 *	sfs	15-Jan-1991	Rewind a tape at both mk_open and mk_close.
 *
 *	pel	08-Jan-1991	handle allocinode amibiguous filename return.
 *
 *	sfs	04-Dec-1990	Fix rundown problems.
 *
 *	pel	14-Nov-1990	Add arguments to allocinode call.
 *				Do allocinodes in dk/mk_open.
 *
 *	pel	31-Oct-1990	add one level of indirection to fp->offset
 *
 *	pel	07-Sep-1990	Call freeinode.
 *
 *	sfs	06-Jul-1990	Work on open/read/write.
 *
 *	sfs	26-Jun-1990	Split out port driver functions.
 *
 *	sfs	31-May-1990	Flesh out class driver functions.
 *
 *	sfs	31-May-1990	Add support for nodes disappearing.
 *
 *	sfs	30-May-1990	Initial entry.
 */

#define DEBUG 0
#define DEBUG_MALLOC 0

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:scsi_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:ctype.h"
#include "cp$src:common.h"
#include "cp$src:wwid.h"
#include "cp$inc:prototypes.h"
#include "cp$src:print_var.h"
#include "cp$inc:kernel_entry.h"

#if MODULAR
extern struct DDB *dk_ddb_ptr;
extern struct DDB *mk_ddb_ptr;
extern int cbip;
#endif

extern struct SEMAPHORE scsi_ub_lock;

#if DEBUG

/*
 * debug flag
 * 1 = top level
 * 2 = malloc and free
 */

int scsi_debug = 0;

#define dprintf(fmt, args)	\
    _dprintf(fmt, args)
#define d2printf(fmt, args)	\
    _d2printf(fmt, args)
#else
#define dprintf(fmt, args)
#define d2printf(fmt, args)
#endif

#if DEBUG
static _dprintf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((scsi_debug & 1) != 0)
	pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
}
static _d2printf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((scsi_debug & 2) != 0)
	pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
}
#endif

#if DEBUG_MALLOC
#define malloc(size,sym) \
	dmalloc(size,"sym");
#define free(ptr,sym) \
	dfree(ptr,"sym")
#else
#define malloc(size,sym) \
	dyn$_malloc(size,DYN$K_SYNC|DYN$K_NOOWN)
#define free(ptr,sym) \
	dyn$_free(ptr,DYN$K_SYNC)
#endif

#define memzone_malloc( x ) dyn$_malloc(x, DYN$K_SYNC | DYN$K_NOOWN | DYN$K_ZONE, 0, 0, memzone)

#if DEBUG_MALLOC
static dmalloc(int size, char *sym)
{
    register addr;

    addr = dyn$_malloc(size, DYN$K_SYNC|DYN$K_NOOWN);
    d2printf("scsi_malloc: %s %x,%d\n", p_args3(sym, addr, size));
    return addr;
}

static dfree(int ptr, char *sym)
{
    dyn$_free(ptr, DYN$K_SYNC);
    d2printf("scsi_free: %s %x\n", p_args2(sym, ptr));
}
#endif

int scsi_mode;

#if MODULAR && ( STARTSHUT || DRIVERSHUT )
extern int cdrom_count;
extern int cdrom_find;
#endif

extern int cbip;
extern null_procedure( );
extern fatal_procedure( );
extern struct FILE *el_fp;

#if FIBRE_CHANNEL
extern int wwidmgr_mode;
extern struct registered_wwid **wwids;
extern struct SEMAPHORE wwids_lock;
extern struct ZONE *memzone;
#endif
extern int force_failover_next;

int scsi_delete( );
int scsi_setmode( );

int dk_read( );
int dk_write( );
int dk_open( );
int dk_close( );

struct DDB dk_ddb = {
	"dk",			/* how this routine wants to be called	*/
	dk_read,		/* read routine				*/
	dk_write,		/* write routine			*/
	dk_open,		/* open routine				*/
	dk_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	scsi_delete,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	scsi_setmode,		/* setmode				*/
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

int mk_read( );
int mk_write( );
int mk_open( );
int mk_close( );

struct DDB mk_ddb = {
	"mk",			/* how this routine wants to be called	*/
	mk_read,		/* read routine				*/
	mk_write,		/* write routine			*/
	mk_open,		/* open routine				*/
	mk_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	scsi_delete,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	scsi_setmode,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	1,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};

struct DDB jk_ddb = {
	"jk",			/* how this routine wants to be called	*/
	null_procedure,		/* read routine				*/
	null_procedure,		/* write routine			*/
	fatal_procedure,	/* open routine				*/
	null_procedure,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	scsi_delete,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	scsi_setmode,		/* setmode				*/
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


/*+
 * ============================================================================
 * = dk_read - read a file                                                    =
 * ============================================================================

 * OVERVIEW:
 *  
 *	This routine reads from an open file.  Since no "file" structure is
 *	provided, a "file" is no more than a raw device.  Only full-block
 *	reads are allowed; this implies that both the current file offset and
 *	the number of bytes to read must be an integral multiple of 512.
 *  
 * FORM OF CALL:
 *  
 *	dk_read( fp, ilen, inum, c )
 *  
 * RETURNS:
 *
 *	Number of bytes successfully read.  May be 0.
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp			- pointer to an open file
 *	int ilen			- size of item
 *	int inum			- number of items
 *	unsigned char *c		- buffer to read into
 *
 * SIDE EFFECTS:
 *
 *	The current offset in the file pointer is updated by the number of
 *	bytes read.
 *
-*/
int dk_read( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    int i;
    int t;

    /*
     *  In order to make the console callback I/O calls robust, we will
     *  provide some level of retry for the most common error, loss of the
     *  connection to the SCSI server.
     */
    if( cbip )
	{
	for( i = 0; i < 8; i++ )
	    {
	    t = dk_read_sub( fp, ilen, inum, c );
	    if( t )
		return( t );
	    t = fp->status;
	    if( ( t != msg_bad_device ) && ( t != msg_scsi_bad_read ) )
		break;
	    krn$_sleep( 100 );
	    dk_close( fp );
	    t = dk_open( fp, 0, 0, 0 );
	    if( t != msg_success )
		break;
	    }
	return( 0 );
	}
    else
	return( dk_read_sub( fp, ilen, inum, c ) );
    }

int dk_read_sub( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    int len;
    int lbn;
    int t;
    struct INODE *inode;
    struct ub *ub;

    len = ilen * inum;
    /*
     *  Make sure the device is still valid (if it isn't, just return 0).
     */
    t = scsi_unit_okay( fp, 1 );
    if( t == msg_success )
	{
	/*
	 *  The file is open.  The MISC field in the INODE structure is used
	 *  to hold a UB pointer.
	 */
	lbn = *(INT *)fp->offset / 512;
	inode = fp->ip;
	ub = inode->misc;
	/*
	 *  Don't let reads go past the end of the unit.
	 */
	if( *(INT *)fp->offset + len > *(INT *)inode->len )
	    {
	    fp->status = msg_eof;
	    fp->count = 0;
	    t = 0;
	    }
	/*
	 *  Make sure that the read respects block boundaries.
	 */
	else if( ( ( len & 511 ) == 0 ) && ( ( *(INT *)fp->offset & 511 ) == 0 ) )
	    {
	    t = scsi_send_dk_read( ub, c, &len, lbn );
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
	}
    else
	{
	fp->status = t;
	fp->count = 0;
	t = 0;
	}
    /*
     *  At this point we have kept track of the number of bytes successfully
     *  read.  Return that number.
     */
    return( t );
    }

/*+
 * ============================================================================
 * = dk_write - write a file                                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine writes to an open file.  Since no "file" structure is
 *	provided, a "file" is no more than a raw device.  Only full-block
 *	writes are allowed; this implies that both the current file offset and
 *	the number of bytes to write must be an integral multiple of 512.
 *  
 * FORM OF CALL:
 *  
 *	dk_write( fp, ilen, inum, c )
 *  
 * RETURNS:
 *
 *	Number of bytes successfully written.  May be 0.
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp			- pointer to an open file
 *	int ilen			- size of item
 *	int inum			- number of items
 *	unsigned char *c		- buffer to write out of
 *
 * SIDE EFFECTS:
 *
 *	The current offset in the file pointer is updated by the number of
 *	bytes written.
 *
-*/
int dk_write( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    int i;
    int t;

    /*
     *  In order to make the console callback I/O calls robust, we will
     *  provide some level of retry for the most common error, loss of the
     *  connection to the MSCP server.
     */
    if( cbip )
	{
	for( i = 0; i < 8; i++ )
	    {
	    t = dk_write_sub( fp, ilen, inum, c );
	    if( t )
		return( t );
	    t = fp->status;
	    if( ( t != msg_bad_device ) && ( t != msg_scsi_bad_write ) )
		break;
	    krn$_sleep( 100 );
	    dk_close( fp );
	    t = dk_open( fp, 0, 0, 0 );
	    if( t != msg_success )
		break;
	    }
	return( 0 );
	}
    else
	return( dk_write_sub( fp, ilen, inum, c ) );
    }

int dk_write_sub( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    int len;
    int lbn;
    int t;
    struct INODE *inode;
    struct ub *ub;

    len = ilen * inum;
    /*
     *  Make sure the device is still valid (if it isn't, just return 0).
     */
    t = scsi_unit_okay( fp, 1 );
    if( t == msg_success )
	{
	/*
	 *  The file is open.  The MISC field in the INODE structure is used
	 *  to hold a UB pointer.
	 */
	lbn = *(INT *)fp->offset / 512;
	inode = fp->ip;
	ub = inode->misc;
	/*
	 *  If this is a CDROM, then it's read-only.
	 */
	if( ( ub->flags & 0x1f ) == 5 )
	    {
	    err_printf( msg_write_prot, inode->name );
	    fp->status = msg_write_prot;
	    fp->count = 0;
	    t = 0;
	    }
	/*
	 *  Don't let writes go past the end of the unit.
	 */
	else if( *(INT *)fp->offset + len > *(INT *)inode->len )
	    {
	    fp->status = msg_eof;
	    fp->count = 0;
	    t = 0;
	    }
	/*
	 *  Make sure that the write respects block boundaries.
	 */
	else if( ( ( len & 511 ) == 0 ) && ( ( *(INT *)fp->offset & 511 ) == 0 ) )
	    {
	    t = scsi_send_dk_write( ub, c, &len, lbn );
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
	}
    else
	{
	fp->status = t;
	fp->count = 0;
	t = 0;
	}
    /*
     *  At this point we have kept track of the number of bytes successfully
     *  written.  Return that number.
     */
    return( t );
    }

/*+
 * ============================================================================
 * = dk_open - open a file                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine opens a file.  A "file" is a raw device, i.e., a SCSI
 *	unit.  "Files" are created and deleted as SCSI units come and go.
 *	Reference counts are kept in the unit block (UB) in order to prevent
 *	UB deletion while files are still open.  This routine may return
 *	failure if the underlying unit is no longer valid.
 *  
 * FORM OF CALL:
 *  
 *	dk_open( fp )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_bad_inode			- the INODE is not in use now
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp			- pointer to the file to be opened
 *
 * SIDE EFFECTS:
 *
 *	Fields in various structures are modified.
 *
-*/
int dk_open( struct FILE *fp, char *info, char *next, char *mode )
    {
    int t;
    struct ub *ub;
    struct INODE *inode;

#if ( STARTSHUT || DRIVERSHUT )
    t = start_driver(fp);
    if (t != msg_success) {
	stop_driver(fp);
	return (t);	
    }
#endif

    common_driver_open( fp, info, next, mode );
    /*
     *  Make sure the device is still valid (if it isn't, just quit).  A
     *  device is valid if the INODE is still in use.
     */
    t = scsi_unit_okay( fp, 0 );
    if( t == msg_success )
	{
	/*
	 *  The device is valid, initialize what we need.
	 */
	inode = fp->ip;
	ub = inode->misc;
	/*
	 *  If the current reference count is 0, then see if the unit is
	 *  ready for media access commands, and if so obtain its capacity.
	 *  We must protect the commands which do this, however, by using the
	 *  reference count.  So diddle the reference count prior to those
	 *  commands, and undiddle it if those commands fail for any reason.
	 */
	ub->ref++;
	if( ub->ref == 1 )
	    {
	    t = scsi_send_test_unit_ready( ub );
	    if( t == msg_success )
		t = scsi_send_mode_select( ub );
	    if( t == msg_success )
		t = scsi_send_read_capacity( ub );
	    }
	else
	    t = msg_success;
	/*
	 *  Do the initialization below if and only if the unit is now ready.
	 */
	if( t == msg_success )
	    {
	    fp->misc = ub->incarn;
	    fp->rap = malloc( sizeof( struct RAB ), rab );
	    fp->rap->alloc = 512;
	    fp->rap->buf = malloc( 512, buf );
	    fp->rap->inx = -1;
	    fp->rap->outx = 0;
	    }
	else
	    {
	    --ub->ref;
	    fp->misc = 0;
	    }
	}

#if ( STARTSHUT || DRIVERSHUT )
    if ( t != msg_success )
        stop_driver( fp );
#endif

    /*
     *  Return success or failure as appropriate.
     */
    return( t );
    }

/*+
 * ============================================================================
 * = dk_close - close a file                                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine closes a file.
 *  
 * FORM OF CALL:
 *  
 *	dk_close( fp )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp			- pointer to the file to be opened
 *
 * SIDE EFFECTS:
 *
 *	Fields in various structures are modified.
 *
-*/
int dk_close( struct FILE *fp )
    {
    struct ub *ub;
    struct INODE *inode;

    /*
     *  There is no need to check to see whether this device is valid or not,
     *  as no device can be destroyed (made invalid) while it contains open
     *  files.  That's what that UB reference count does for us!
     */
    ub = fp->ip->misc;
    if( fp->misc )
	{
	if( ub->ref == 1 )
	    {
	    inode = ub->inode;
	    *(INT *)inode->len = (INT)0;
	    *(INT *)inode->alloc = (INT)0;
	    }
	--ub->ref;
	fp->misc = 0;
	free( fp->rap->buf, buf );
	free( fp->rap, rab);
	}

#if ( STARTSHUT || DRIVERSHUT )
    stop_driver(fp);
#endif

    return( msg_success );
    }

/*+
 * ============================================================================
 * = mk_read - read a file                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine reads from an open file.  Since no "file" structure is
 *	provided, a "file" is no more than a raw device.  Only full-record
 *	reads are allowed; the number of bytes in the record is returned,
 *	rather than the number of bytes asked for.
 *  
 * FORM OF CALL:
 *  
 *	mk_read( fp, ilen, inum, c )
 *  
 * RETURNS:
 *
 *	Number of bytes successfully read.  May be 0.
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp			- pointer to an open file
 *	int ilen			- size of item
 *	int inum			- number of items
 *	unsigned char *c		- buffer to read into
 *
 * SIDE EFFECTS:
 *
 *	The current offset in the file pointer is updated by the number of
 *	bytes read.
 *
-*/
int mk_read( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    int len;
    int t;
    struct INODE *inode;
    struct ub *ub;

    len = ilen * inum;
    /*
     *  Make sure the device is still valid (if it isn't, just return 0).
     */
    t = scsi_unit_okay( fp, 1 );
    if( t == msg_success )
	{
	/*
	 *  The file is open.  The MISC field in the INODE structure is used
	 *  to hold a UB pointer.
	 */
	inode = fp->ip;
	ub = inode->misc;
	t = scsi_send_mk_read( ub, c, &len );
	fp->status = t;
	fp->count = len;
	if( ( t == msg_success )
		|| ( t == msg_rec_underflow )
		|| ( t == msg_rec_overflow ) )
	    t = len;
	else
	    {
	    fp->count = 0;
	    t = 0;
	    }
	/*
	 *  Update the file offset.
	 */
	*(INT *)fp->offset += t;
	}
    else
	{
	fp->status = t;
	fp->count = 0;
	t = 0;
	}
    /*
     *  At this point we have kept track of the number of bytes successfully
     *  read.  Return that number.
     */
    return( t );
    }

/*+
 * ============================================================================
 * = mk_write - write a file                                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine writes to an open file.  Since no "file" structure is
 *	provided, a "file" is no more than a raw device.  Only full-record
 *	writes are allowed.
 *  
 * FORM OF CALL:
 *  
 *	mk_write( fp, ilen, inum, c )
 *  
 * RETURNS:
 *
 *	Number of bytes successfully written.  May be 0.
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp			- pointer to an open file
 *	int ilen			- size of item
 *	int inum			- number of items
 *	unsigned char *c		- buffer to write out of
 *
 * SIDE EFFECTS:
 *
 *	The current offset in the file pointer is updated by the number of
 *	bytes written.
 *
-*/
int mk_write( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    int len;
    int t;
    int size;
    struct INODE *inode;
    struct ub *ub;

    len = ilen * inum;
    /*
     *  Make sure the device is still valid (if it isn't, just return 0).
     */
    t = scsi_unit_okay( fp, 1 );
    if( t == msg_success )
	{
	/* 
	 *  The file is open.  The MISC field in the INODE structure is used
	 *  to hold a UB pointer.
	 */
     	inode = fp->ip;
       	ub = inode->misc;
	t = scsi_send_mk_write( ub, c, &len );
	fp->status = t;
	fp->count = len;
	if( ( t == msg_success )
		|| ( t == msg_rec_underflow ) )
	    t = len;
	else
	    {
	    fp->count = 0;
	    t = 0;
	    }
      	/*
       	 *  Update the file offset.
       	 */
	*(INT *)fp->offset += t;
	}
    else
	{
	fp->status = t;
	fp->count = 0;
	t = 0;
	}
    /*
     *  At this point we have kept track of the number of bytes successfully
     *  written.  Return that number.
     */
    return( t );
    }

/*+
 * ============================================================================
 * = mk_open - open a file                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine opens a file.  A "file" is a raw device, i.e., a SCSI
 *	unit.  "Files" are created and deleted as SCSI units come and go.
 *	Reference counts are kept in the unit block (UB) in order to prevent
 *	UB deletion while files are still open.  This routine may return
 *	failure if the underlying unit is no longer valid.
 *  
 * FORM OF CALL:
 *  
 *	mk_open( fp )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_bad_inode			- the INODE is not in use now
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp			- pointer to the file to be opened
 *
 * SIDE EFFECTS:
 *
 *	Fields in various structures are modified.
 *
-*/
int mk_open( struct FILE *fp, char *info, char *next, char *mode )
    {
    int t;
    struct ub *ub;
    struct INODE *inode;

#if ( STARTSHUT || DRIVERSHUT )
    t = start_driver(fp);
    if (t != msg_success) {
	stop_driver(fp);
	return (t);	
    }
#endif

    common_driver_open( fp, info, next, mode );
    /*
     *  Make sure the device is still valid (if it isn't, just quit).  A
     *  device is valid if the INODE is still in use.
     */
    t = scsi_unit_okay( fp, 0 );
    if( t == msg_success )
	{
	/*
	 *  The device is valid, initialize what we need.
	 */
	inode = fp->ip;
	ub = inode->misc;
	/*
	 *  If the current reference count is 0, then see if the unit is
	 *  ready for media access commands.  If so, rewind it.  We must
	 *  protect the commands which do this, however, by using the
	 *  reference count.  So diddle the reference count prior to those
	 *  commands, and undiddle it if those commands fail for any reason.
	 */
	ub->ref++;
	if( ub->ref == 1 )
	    {
	    t = scsi_send_test_unit_ready( ub );
	    if( t == msg_success )
		t = scsi_send_rewind( ub );
	    }
	else
	    t = msg_success;
	/*
	 *  Do the initialization below if and only if the unit is now ready.
	 */
	if( t == msg_success )
	    {
	    fp->misc = ub->incarn;
	    fp->rap = malloc( sizeof( struct RAB ), rab );
	    fp->rap->alloc = 2048;
	    fp->rap->buf = malloc( 2048, buf );
	    fp->rap->inx = -1;
	    fp->rap->outx = 0;
	    }
	else
	    {
	    --ub->ref;
	    fp->misc = 0;
	    }
	}
    /*
     *  Return success or failure as appropriate.
     */
    return( t );
    }

/*+
 * ============================================================================
 * = mk_close - close a file                                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine closes a file.
 *  
 * FORM OF CALL:
 *  
 *	mk_close( fp )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp			- pointer to the file to be opened
 *
 * SIDE EFFECTS:
 *
 *	Fields in various structures are modified.
 *
-*/
int mk_close( struct FILE *fp )
    {
    struct ub *ub;

    /*
     *  There is no need to check to see whether this device is valid or not,
     *  as no device can be destroyed (made invalid) while it contains open
     *  files.  That's what that UB reference count does for us!
     */
    ub = fp->ip->misc;
    if( fp->misc )
	{
	/*
	 *  If the current reference count is 1, then rewind the unit.
	 */
	if( ub->ref == 1 )
	    scsi_send_rewind( ub );
	--ub->ref;
	free( fp->rap->buf, buf );
	free( fp->rap, rab );
	}

#if ( STARTSHUT || DRIVERSHUT )
    stop_driver(fp);
#endif

    return( msg_success );
    }

/*+
 * ============================================================================
 * = scsi_init - initialize the SCSI DDBs                                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine gives us a chance to initialize the SCSI DDBs (one DDB
 *	for DK and one for MK).  Private entry points are made public.
 *  
 * FORM OF CALL:
 *  
 *	scsi_init( )
 *  
 * RETURN CODES:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
 *	DK and MK DDBs are filled in.
 *
-*/
int scsi_init( )
    {
#if MODULAR
    dk_ddb_ptr = &dk_ddb;
    mk_ddb_ptr = &mk_ddb;
#endif
    scsi_mode = DDB$K_INTERRUPT;
    return( msg_success );
    }


int scsi_delete( struct INODE *inode )
    {
    struct ub *ub;
    struct scsi_sb *sb;

    if( inode->inuse )
	{
	ub = inode->misc;
	sb = ub->sb;
	if( ub->ref || ( ub->flags > 65535 ) )
	    INODE_UNLOCK( inode );
	else
	    {
	    scsi_lun_map( sb, CLEAR_LUN, ub->unit);
	    if( sb->first_ub == ub )
	        sb->first_ub = 0;
	    remove_ub( ub );
	    remq( ub );
	    if( ub->wwid )
	        free( ub->wwid, wwid );
	    free( ub->inq, inq );
	    free( ub, ub );
	    freeinode( inode );
	    }
	}
    return( msg_success );
    }


int scsi_unit_okay( struct FILE *fp, int flag )
    {
    int i;
    int j;
    struct INODE *inode;
    struct ub *ub;

    inode = fp->ip;
    if( inode->inuse )
	{
	ub = inode->misc;
	if( flag )
	    {
	    if( ub->sb && ( ub->incarn == fp->misc ) )
		return( msg_success );
	    }
	else
	    {
	    if( cbip )
		j = 60;
	    else
		j = 10;
	    for( i = 0; i < j; i++ )
		{
		if( ub && ub->sb )
		    return( msg_success );
		krn$_sleep( 500 );
		ub = inode->misc;
		}
	    }
	err_printf( msg_bad_device, inode->name );
	return( msg_bad_device );
	}
    else
	{
	err_printf( msg_bad_inode, inode->name );
	return( msg_bad_inode );
	}
    }

/*+
 * ============================================================================
 * = scsi_setmode - stop or start the SCSI driver                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine deals with requests to stop or start the SCSI driver.
 *  
 * FORM OF CALL:
 *  
 *	scsi_setmode( mode )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	int mode			- the new mode
 *
 * SIDE EFFECTS:
 *
 *	The driver may be stopped or started.
 *
-*/
void scsi_setmode( int mode )
    {
    if( mode != DDB$K_READY )
	scsi_mode = mode;
    }


#if FIBRE_CHANNEL
/*+
 * ============================================================================
 * = scsi_report_luns - send a SCSI report luns cmd                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a SCSI Report_Luns command.  This routine
 *	is called by the polling routine in SCSI port drivers.  Not all 
 *      devices support report_luns and this command is allowed to fail and
 *      scsi_analyze will return a msg_expected_ill_req.
 *
 *      This code only supports the SCC positioned at lun 0.
 *  
 * FORM OF CALL:
 *  
 *	scsi_report_luns( sb, dat_in, dat_in_len )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct scsi_sb *sb		- system block
 *      unsigned char *dat_in           - buffer address for returned data
 *      int *dat_in_len                 - pointer to dat_in length
 *
 * SIDE EFFECTS:
 *
 *	None if the command is successful.
 *      If the command fails to issue, a scsi_break_vc will result.
 *      if the command fails because of an Illegal Request, no side effects.
 *
-*/
int scsi3_report_luns( struct scsi_sb *sb, unsigned char *dat_in, int *dat_in_len )
{

    unsigned char cmd[12];
    int cmd_len;
    int t;
    struct ub *ub;

    /*
     *  Fill in the Command Out bytes.
     */
    memset( &cmd, 0, sizeof( cmd) );
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_report_luns;               /* report luns */
    scsi_swap_data( &cmd[6], dat_in_len, 4 );
    ub = malloc( sizeof( *ub ), ub );
    ub->unit = 0;  
    ub->sb = sb;
    ub->pb = sb->pb;
    t = scsi_send( ub, cmd, cmd_len, 0, 0, dat_in, dat_in_len );
    free( ub, ub );
   
    return t;
}
#endif



/*+
 * ============================================================================
 * = scsi_send_inquiry - send a SCSI Inquiry command                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a SCSI Inquiry command.  This routine
 *	is called by the polling routine in SCSI port drivers.
 *  
 * FORM OF CALL:
 *  
 *	scsi_send_inquiry( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct scsi_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
 *	A new unit may be discovered.
 *
-*/

void scsi_send_inquiry( struct scsi_sb *sb )
    {
#if FIBRE_CHANNEL
      if ( is_scsi3( sb->pb ) ) {
        scsi3_send_inquiry( sb );
      } else {
        scsi2_send_inquiry( sb );
      }
#else
        scsi2_send_inquiry( sb );
#endif
    }

void scsi2_send_inquiry( struct scsi_sb *sb )
    {
    unsigned char cmd[6];
    unsigned char dat_in[MAX_EXT_INQ_SZ];
    int cmd_len;
    int dat_in_len;
    int i;
    int t;
    struct ub *ub;
    memset( &dat_in, 0, sizeof( dat_in ) );

    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_inquiry;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = sizeof( dat_in );
    cmd[5] = 0;
    /*
     *  Get a UB for use.
     */
    ub = malloc( sizeof( *ub ), ub );
    ub->sb = sb;
    ub->pb = sb->pb;
    /*
     *  Send the command.  If successful, say we have discovered a new unit.
     */
    for( i = 0; ; i++ )
	{
	ub->unit = i & 7;
	dat_in_len = sizeof( dat_in );
	t = scsi_send( ub, cmd, cmd_len, 0, 0, dat_in, &dat_in_len );
	if( i == 8 )
	    break;
	else if( t == msg_success )
	    scsi_new_unit( sb, i, dat_in, 0, 0, 0 );
	else if( i == 0 )
	    break;
	}
    /*
     * Get rid of the UB.
     */
    free( ub, ub );
    }


#if FIBRE_CHANNEL
void scsi3_send_inquiry( struct scsi3_sb *sb )
    {
    unsigned char cmd[6];
    unsigned int *report_luns_dat_in;
    unsigned char dat_in[MAX_EXT_INQ_SZ];
    char name[32];
    int cmd_len;
    int dat_in_len;
    int report_luns_dat_in_len;
    int i;
    int idx;
    int t;
    int status;
    int report_luns_status;
    int nluns;
    int lun;
    struct ub *ub;
    union wwid *wwid;
    struct registered_wwid *w;
    char alias[sizeof(ub->alias)]; 

    wwid = malloc( sizeof(union wwid), wwid );

    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_inquiry;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = sizeof( dat_in );
    cmd[5] = 0;

#define MAX_LUNS 256

    /*
     * Size for max of 256 luns
     */
    report_luns_dat_in = malloc( 8*MAX_LUNS+8, report_luns_data_in ); 
    report_luns_dat_in_len = 4*MAX_LUNS+8;
    report_luns_status = scsi3_report_luns( sb, report_luns_dat_in, 
                                                      &report_luns_dat_in_len );

    if (report_luns_status == msg_success) {
      /*
       *  Inquire the luns that were reported.
       */
      dprintf("report luns succeeded\n", p_args0 );
      scsi_swap_data( &nluns, &report_luns_dat_in[0], 4);
      nluns = nluns / 8;
      /*
       *  Check to see if we did allocate enough.
       */
      if(nluns > MAX_LUNS ) {
        printf("number of luns %d exceeded maximum allowed\n",
                                                        nluns, MAX_LUNS );
      }
      /*
       * Since we at most get back MAX_LUNS items, adjust the looping
       * to not go over MAX_LUNS.
       */
      nluns = min( MAX_LUNS, nluns );
      
    } else {
       nluns = 32; 
    }

    /*
     * Possible Enhancement:  
     *   This latest lun list should be compared against the LUN MAP to see 
     *   if any luns have gone away.  We should merge the lun map with the
     *   report luns list and probe them all.  Unlike parallel scsi which
     *   exhaustively probes all the luns on each inquiry, the fibre channel
     *   only probes the reported luns.  Therefore, an unreported lun will not
     *   get probed and we will not realize it has gone away.  On monolithic
     *   consoles, this means the show dev will have a stale device in it 
     *   until a scsi_break_vc runs clearing all the luns existant and non-
     *   existant.   The scsi_break_vc would occur if somebody tried to use
     *   the device or on stop/start of the driver.    On STARTSHUT, this is
     *   not an issue since the started fresh each time to yield a show
     *   dev display (of course in diagnostic mode, even the STARTSHUT will
     *   see this problem).  I do not think this is a biggie.  Just removing
     *   a disk from an HSZ/HSG does not make the lun go away.   A human
     *   has to type some console commands to undefine the device.  
     *   (30-Oct-1998)
     */


    dprintf("nluns: %d\n", p_args1( nluns ) );

    for( i = 0; i < nluns ; i++ ) {
      if (report_luns_status == msg_success) {
	 idx = (i*2)+2;    /* just using the LSB longword */
         lun = report_luns_dat_in[idx] >> 8;
      } else {
  	 lun = i & 0xFF;
      }
      dat_in_len = sizeof( dat_in );

      dprintf("sb: %x  lun: %x \n", p_args2(sb, lun) );

      memset( &dat_in, 0, sizeof( dat_in ) );
      t = scsi_get_page( sb, lun, scsi_k_cmd_inquiry, 0, scsi_k_inq_not_evpd,
                                                          dat_in, &dat_in_len);

      /*
       * An Inquiry should not fail if the device is present and healthy.
       * If it does fail, we will not waste our time with any more Inquiries
       * to that sb this time around.
       */
      if( t != msg_success ) 
         break;

      t = dat_in[0] & 0xc0;
      if( t )
        continue;

      /*
       * At this time only support disk device
       */
      if( dat_in[0] & 0x1f ) continue;

      t = scsi3_get_wwid( sb, lun, wwid );
      if( t ) continue;


	/*
	 * Here for setup mode. During normal operation, we only operate
	 * with the wwids registered in the ev's (wwid0, wwid1, wwid2, wwid3).
         * However, if we are in wwidmgr_mode, we do not filter out any wwids
	 * and operate on everything we find.   To facilitate this, we add
	 * any new wwids to the list of wwids, growing it as big as we want
	 * because the space is malloced from memzone.  These temporarily
	 * registered wwids never need to get backed by ev's, and we 
	 * indicate that there is no backing ev with a -1.  
         *
         * Since many adapters can be present and all actively discovering
	 * the wwids, we have to lock the wwids structure to synchronize
	 * access.
	 */
	if (wwidmgr_mode) {
	    /*
	     * Check to see if this wwid is already in use.  This can
	     * happen by simultaneous polling (from the previous rundown)
	     * or repeat polling (say a LATT kicks one off).
	     */
	    krn$_wait(&wwids_lock);
	    w = find_wwidfilter_from_wwids(wwid, &idx);
	    if (!w) {
		dprintf("scsi - wwid for lun %x not found sb:%x\n",
		  p_args2(lun, sb));
		w = memzone_malloc( sizeof(struct registered_wwid) );
                /* May 16, 2000 (djm)
                 * These fields are malloc'ed now instead of a static
		 * array.  This structure was changed to reduce the static
                 * heap consumption on the first 4 wwids which are used
		 * prior to wwidmgr_mode, but do not use these fields.
		 * See also the comment in WWIDMGR.C.
                 */
		w->sbs = memzone_malloc( sizeof(struct sb *) * MULTIPATH_MAX );
		w->lun = memzone_malloc( sizeof(int) * MULTIPATH_MAX );
		w->online = memzone_malloc( sizeof(int) * MULTIPATH_MAX );
		w->evnum = -1;
		memcpy(&w->wwid, wwid, sizeof(union wwid));
		insert_wwid(w);
	    }				/* if !w           */
	    krn$_post(&wwids_lock);
	}				/* if wwidmgr_mode */

	/*
	 * Here for normal flow.  If a device name can be assembled, then
	 * go ahead and create the unit.
	 */
	if (t == msg_success)
	    status = scsi3_get_alias(sb, lun, wwid, &name, &alias);
	if (status == msg_success) {
	    scsi_new_unit(sb, lun, dat_in, wwid, &name, &alias );
	}

    } /* for */

    free( wwid, wwid );
    free( report_luns_dat_in, report_luns_data_in );
    }
#endif /* FIBRE_CHANNEL */

#if DEBUG
struct ub *find_scsi_ub( struct scsi_sb *sb, int lun )
{
  struct ub *ub;
  ub = sb->ub.flink;
  while( ub != &sb->ub.flink ) {
    if ( (ub->sb==sb) && (ub->unit==lun) ) return ub;
    ub = ub->ub.flink;
  }
  return NULL;
}
#endif


/*+
 * ============================================================================
 * = scsi_send_start_unit - send a SCSI Start Unit command                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a SCSI Start Unit command, in order to
 *	spin up the unit.
 *  
 * FORM OF CALL:
 *  
 *	scsi_send_start_unit( ub )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct ub *ub			- unit block
 *
 * SIDE EFFECTS:
 *
 *	The unit may be spun up.
 *
-*/
int scsi_send_start_unit( struct ub *ub )
    {
    char *name;
    unsigned char cmd[6];
    int cmd_len;
    int t;
    unsigned char dat_in[1];
    int dat_in_len;
    int inq_status;

    name = ub->inode->name;
    /*
     *  For tapes, just return.
     */
    if( ub->inode->dva == &mk_ddb )
	return msg_success;

    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_inquiry;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = 1;			/* only need the peripheral qualifier */
    cmd[5] = 0;
    /*
     * Check to see if the device is connected.  Cannot send
     * a start_unit to a device that is not connected or we will
     * fail over an HSGx80 from one controller to another.  afw_eco 1226 
     */
    dat_in_len = sizeof( dat_in );
    t = scsi_send( ub, cmd, cmd_len, 0, 0, dat_in, &dat_in_len );
    if ( t == msg_success ) 
       if ( ( (dat_in[0] & 0xe0) == 0x20 ) && (dat_in[0] != 0x3f) )
          if( !force_failover_next )
             return msg_no_conn;
          else
             {
	       dprintf("Not connected - failing device over\n", p_args0);
               inq_status = msg_no_conn;
             }

    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_start_unit;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = 1;
    cmd[5] = 0;
    /*
     *  Send the command.  If successful, say we have discovered a new unit.
     */
    t = scsi_send( ub, cmd, cmd_len, 0, 0, 0, 0 );
    if( t != msg_success )
	fprintf( el_fp, msg_scsi_bad_su, name );
    else
        {
        if ( inq_status == msg_no_conn )
           {
            dprintf("Turn off force_failover_next\n", p_args0);
            force_failover_next = 0;
           }
        }
    return t;
    }

/*+
 * ============================================================================
 * = scsi_send_mode_select - send a SCSI Mode Select command                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a SCSI Mode Select command, in order
 *	to set the block size to 512.
 *  
 * FORM OF CALL:
 *  
 *	scsi_send_mode_select( ub )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct ub *ub			- unit block
 *
 * SIDE EFFECTS:
 *
 *	The block size may be set to 512.
 *
-*/
int scsi_send_mode_select( struct ub *ub )
    {
    char *name;
    unsigned char cmd[6];
    unsigned char dat_out[12];
    int cmd_len;
    int dat_out_len;
    int t;

    name = ub->inode->name;
    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_mode_select;
    cmd[1] = 16;
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = sizeof( dat_out );
    cmd[5] = 0;
    /*
     *  Fill in the Data Out bytes.
     */
    dat_out_len = sizeof( dat_out );
    dat_out[0] = 0;
    dat_out[1] = 0;
    dat_out[2] = 0;
    dat_out[3] = 8;
    dat_out[4] = 0;
    t = 0;
    scsi_swap_data( &dat_out[5], &t, 3 );
    dat_out[8] = 0;
    t = 512;
    scsi_swap_data( &dat_out[9], &t, 3 );
    /*
     *  Send the command.
     */
    t = scsi_send( ub, cmd, cmd_len, dat_out, &dat_out_len, 0, 0 );
    if( t != msg_success )
	fprintf( el_fp, msg_scsi_bad_ms, name );
    return msg_success;
    }

/*+
 * ============================================================================
 * = scsi_send_read_capacity - send a SCSI Read Capacity command              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a SCSI Read Capacity command, in order
 *	to find out how big the unit is.
 *  
 * FORM OF CALL:
 *  
 *	scsi_send_read_capacity( ub )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_scsi_not_512		- the block size is not 512 bytes
 *	msg_scsi_bad_rc			- the Read Capacity command failed
 *	others				- other codes returned by subordinates
 *
 * ARGUMENTS:
 *
 *	struct ub *ub			- unit block
 *
 * SIDE EFFECTS:
 *
 *	The capacity of the unit may be remembered.  If the block size is
 *	not 512, the unit is not usable.
 *
-*/
int scsi_send_read_capacity( struct ub *ub )
    {
    char *name;
    unsigned char cmd[10];
    unsigned char dat_in[8];
    int cmd_len;
    int dat_in_len;
    int t;
    struct scsi_sb *sb;
    struct INODE *inode;

    name = ub->inode->name;
    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_read_capacity;
    cmd[1] = 0;
    t = 0;
    scsi_swap_data( &cmd[2], &t, 4 );
    cmd[6] = 0;
    cmd[7] = 0;
    cmd[8] = 0;
    cmd[9] = 0;
    dat_in_len = sizeof( dat_in );
    /*
     *  Send the command.
     */
    t = scsi_send( ub, cmd, cmd_len, 0, 0, dat_in, &dat_in_len );
    if( t == msg_success )
	{
	/*
	 *  Record the capacity.  If the block size is not what we want,
	 *  don't let the unit be used.
	 */
	scsi_swap_data( &t, &dat_in[0], 4 );
	inode = ub->inode;
	*(INT *)inode->len = ( (INT)t + 1 ) * 512;
	*(INT *)inode->alloc = ( (INT)t + 1 ) * 512;
	scsi_swap_data( &t, &dat_in[4], 4 );
	if( t == 512 )
	    return( msg_success );
	else
	    {
	    err_printf( msg_scsi_not_512, name );
	    return( msg_scsi_not_512 );
	    }
	}
    else
	{
	err_printf( msg_scsi_bad_rc, name );
	return( msg_scsi_bad_rc );
	}
    }

/*+
 * ============================================================================
 * = scsi_send_test_unit_ready - send a SCSI Test Unit Ready command          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a SCSI Test Unit Ready command, in order
 *	to see if the unit is ready to accept commands.
 *  
 * FORM OF CALL:
 *  
 *	scsi_send_test_unit_ready( ub )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	others				- other codes returned by subordinates
 *
 * ARGUMENTS:
 *
 *	struct ub *ub			- unit block
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int scsi_send_test_unit_ready( struct ub *ub )
    {
    unsigned char cmd[6];
    int cmd_len;
    int t;

    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_test_unit_ready;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = 0;
    cmd[5] = 0;
    /*
     *  Send the command.  If successful, then the unit is ready to accept
     *  media access commands.
     */
    t = scsi_send( ub, cmd, cmd_len, 0, 0, 0, 0 );
    return( t );
    }

/*+
 * ============================================================================
 * = scsi_send_rewind - send a SCSI Rewind command                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a SCSI Rewind command, to rewind the
 *	tape.
 *  
 * FORM OF CALL:
 *  
 *	scsi_send_rewind( ub )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_scsi_bad_rewind		- the Rewind command failed
 *	others				- other codes returned by subordinates
 *
 * ARGUMENTS:
 *
 *	struct ub *ub			- unit block
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int scsi_send_rewind( struct ub *ub )
    {
    char *name;
    unsigned char cmd[6];
    int cmd_len;
    int t;

    name = ub->inode->name;
    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_rewind;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = 0;
    cmd[5] = 0;
    /*
     *  Send the command.
     */
    t = scsi_send( ub, cmd, cmd_len, 0, 0, 0, 0 );
    if( t == msg_success )
	return( t );
    err_printf( msg_scsi_bad_rewind, name );
    return( msg_scsi_bad_rewind );
    }

/*+
 * ============================================================================
 * = scsi_send_dk_read - send a SCSI Read command                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a SCSI Read command.
 *  
 * FORM OF CALL:
 *  
 *	scsi_send_dk_read( ub, dat_in, dat_in_len, lbn )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_scsi_bad_read		- the Read command failed
 *       
 * ARGUMENTS:
 *
 *	struct ub *ub			- unit block
 *	unsigned char *dat_in		- buffer for read data
 *	int *dat_in_len			- pointer to size to read
 *	int lbn				- logical block number to read
 *
 * SIDE EFFECTS:
 *
-*/
int scsi_send_dk_read( struct ub *ub, unsigned char *dat_in, int *dat_in_len,
	int lbn )
    {
    char *name;
    unsigned char cmd[10];
    int cmd_len;
    int t;

    name = ub->inode->name;
    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_dk_read;
    cmd[1] = 0;
    t = lbn;
    scsi_swap_data( &cmd[2], &t, 4 );
    cmd[6] = 0;
    t = *dat_in_len/512;
    scsi_swap_data( &cmd[7], &t, 2 );
    cmd[9] = 0;
    t = scsi_send( ub, cmd, cmd_len, 0, 0, dat_in, dat_in_len );
    if( t == msg_success )
	return( t );
    err_printf( msg_scsi_bad_read, name );
    return( msg_scsi_bad_read );
    }

/*+
 * ============================================================================
 * = scsi_send_dk_write - send a SCSI Write command                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a SCSI Write command.
 *  
 * FORM OF CALL:
 *  
 *	scsi_send_dk_write( ub, dat_out, dat_out_len, lbn )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_scsi_bad_write		- the Write command failed
 *       
 * ARGUMENTS:
 *
 *	struct ub *ub			- unit block
 *	unsigned char *dat_out		- buffer for write data
 *	int *dat_out_len		- pointer to size to write
 *	int lbn				- logical block number to write
 *
 * SIDE EFFECTS:
 *
-*/
int scsi_send_dk_write( struct ub *ub, unsigned char *dat_out, int *dat_out_len,
	int lbn )
    {
    char *name;
    unsigned char cmd[10];
    int cmd_len;
    int t;

    name = ub->inode->name;
    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_dk_write;
    cmd[1] = 0;
    t = lbn;
    scsi_swap_data( &cmd[2], &t, 4 );
    cmd[6] = 0;
    t = *dat_out_len/512;
    scsi_swap_data( &cmd[7], &t, 2 );
    cmd[9] = 0;
    t = scsi_send( ub, cmd, cmd_len, dat_out, dat_out_len, 0, 0 );
    if( t == msg_success )
	return( t );
    err_printf( msg_scsi_bad_write, name );
    return( msg_scsi_bad_write );
    }

/*+
 * ============================================================================
 * = scsi_send_mk_read - send a SCSI Read command                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a SCSI Read command.
 *  
 * FORM OF CALL:
 *  
 *	scsi_send_mk_read( ub, dat_in, dat_in_len )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_rec_underflow		- the record read was too short
 *	msg_rec_overflow		- the record read was too long
 *	msg_eof				- end of file was found
 *	msg_eot				- end of tape was found
 *	msg_scsi_bad_read		- the Read command failed
 *       
 * ARGUMENTS:
 *
 *	struct ub *ub			- unit block
 *	unsigned char *dat_in		- buffer for read data
 *	int *dat_in_len			- pointer to size to read
 *
 * SIDE EFFECTS:
 *
-*/
int scsi_send_mk_read( struct ub *ub, unsigned char *dat_in, int *dat_in_len )
    {
    char *name;
    unsigned char cmd[6];
    int cmd_len;
    int t;

    name = ub->inode->name;
    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_mk_read;
    cmd[1] = 0;
    t = *dat_in_len;
    scsi_swap_data( &cmd[2], &t, 3 );
    cmd[5] = 0;
    t = scsi_send( ub, cmd, cmd_len, 0, 0, dat_in, dat_in_len );
    if( ( t == msg_success )
	    || ( t == msg_rec_underflow )
	    || ( t == msg_rec_overflow )
	    || ( t == msg_eof )
	    || ( t == msg_eot ) )
	return( t );
    err_printf( msg_scsi_bad_read, name );
    return( msg_scsi_bad_read );
    }

/*+
 * ============================================================================
 * = scsi_send_mk_write - send a SCSI Write command                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a SCSI Write command.
 *  
 * FORM OF CALL:
 *  
 *	scsi_send_mk_write( ub, dat_out, dat_out_len )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *   	msg_rec_underflow		- the record written was too short
 *	msg_eot				- end of tape was found
 *	msg_scsi_bad_write		- the Write command failed
 *       
 * ARGUMENTS:
 *
 *	struct ub *ub			- unit block
 *	unsigned char *dat_out		- buffer for write data
 *	int *dat_out_len		- pointer to size to write
 *
 * SIDE EFFECTS:
 *
-*/
int scsi_send_mk_write( struct ub *ub, unsigned char *dat_out, int *dat_out_len )
    {
    char *name;
    unsigned char cmd[6];
    int cmd_len;
    int t;

    name = ub->inode->name;
    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_mk_write;
    cmd[1] = 0;
    t = *dat_out_len;
    scsi_swap_data( &cmd[2], &t, 3 );
    cmd[5] = 0;
    t = scsi_send( ub, cmd, cmd_len, dat_out, dat_out_len, 0, 0 ); 
    if( ( t == msg_success )
	    || ( t == msg_rec_underflow )
	    || ( t == msg_eot ) )
	return( t );
    err_printf( msg_scsi_bad_write, name );
    return( msg_scsi_bad_write );
    }


/*+
 * ============================================================================
 * = scsi_send_mk_write_filemarks - send a SCSI Write Filemarks command       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends a SCSI Write Filemarks command.
 *  
 * FORM OF CALL:
 *  
 *	scsi_send_mk_write_filemarks( ub, dat_out, dat_out_len )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *   	msg_rec_underflow		- the record written was too short
 *	msg_eot				- end of tape was found
 *	msg_scsi_bad_write		- the Write command failed
 *       
 * ARGUMENTS:
 *
 *	struct ub *ub			- unit block
 *	unsigned char *dat_out		- buffer for write data
 *	int *dat_out_len		- pointer to size to write
 *
 * SIDE EFFECTS:
 *
-*/
int scsi_send_mk_write_filemarks( struct ub *ub, unsigned char *dat_out, 
	int *dat_out_len )
    {
    char *name;
    unsigned char cmd[6];
    int cmd_len;
    int t;

    name = ub->inode->name;
    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_mk_write_filemarks;
    cmd[1] = 0;
    t = 1;
    scsi_swap_data( &cmd[2], &t, 3 );
    cmd[5] = 0;
    t = scsi_send( ub, cmd, cmd_len, dat_out, dat_out_len, 0, 0 );
    if( ( t == msg_success )
	    || ( t == msg_rec_underflow )
	    || ( t == msg_eot ) )
	return( t );
    err_printf( msg_scsi_bad_write, name );
    return( msg_scsi_bad_write );
    }

/*+
 * ============================================================================
 * = scsi_new_unit - make a unit known to the system                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds the data structures necessary to make a SCSI
 *	unit known to the system.
 *  
 * FORM OF CALL:
 *  
 *	scsi_new_unit( sb, lun, inq, wwid, name, alias )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct scsi_sb *sb		- system block
 *	int lun				- LUN
 *	unsigned char *inq		- pointer to inquiry data
 *      unsigned char *wwid             - optional wwid information
 *      char *iname                     - optional name for the inode
 *      char *alias                     - optional alias string to display
 *
 * SIDE EFFECTS:
 *
 *	A unit block is allocated, and an INODE is initialized.
 *
-*/
void scsi_new_unit( struct scsi_sb *sb, int lun, unsigned char *inq,
                                  union wwid *wwid, char *iname, char *alias )
    {
    char controller[MAX_NAME];
    char name[32];
    char cdrom_name[8];
    char protocol[8];
    char *np;
    int inq_len;
    int i;
    int t;
    struct DDB *driver;
    struct pb *pb;
    struct ub *ub;
    struct INODE *inode;


    pb = sb->pb;
    /*
     *  If we are still running down this node, ignore this call.
     */
    if( sb->state == sb_k_stall )
	return;
    /*
     *  Flag that we've successfully talked to this node.
     */
    sb->state = sb_k_open;

    if ( scsi_lun_map( sb, GET_LUN, lun ) ) 
      return;
    scsi_lun_map( sb, SET_LUN, lun );

    /*
     *  30-Mar-98 afw_eco 1226;  To support the HSx80, we need to find even
     *  the devices that are not-connected.  We assume that the inquiry
     *  data is valid even for a disconnected device.
     *
     */
    t = inq[0] & 0xc0;
    if( t )
	return;
    /*
     *  The kzpsa returns 23 when polled and does not need to be displayed.
     */
    if(inq[0] == 0x23)
		return;
    /*
     *  Check the device type field.  We will accept 0 (random access),
     *  1 (sequential access), 4 (write-once), 5 (read-only), or 7 (optical).
     */
    t = inq[0] & 0x1f;
    switch( t )
	{
	/*
	 *  Sequential access devices use MK; all others use DK.
	 */
	case 0:
	case 4:
	case 5:
	case 7:
	    driver = &dk_ddb;
	    sb->ok = 1;
	    break;

	case 1:
	    driver = &mk_ddb;
	    sb->ok = 1;
	    break;
	
	case 3:
	    /*
	     *  This is a "processor" (i.e., a host adapter).  Currently only
	     *  the Adaptec 174x falls into this category, so this code may
	     *  be specific to that device.
	     */
	    if( sb->local )
		{
		sb->ok = 1;
		sprintf( pb->version, "%4.4s", &inq[32] );
		sprintf( pb->string, "%-24s   %-8s   %24s  %s",
			pb->name, pb->alias, pb->info, pb->version );
		return;
		}
	    driver = &jk_ddb;
	    sb->ok = 1;
	    break;

	case 0x1f:
	    return;

	default:
	    if( sb->ok )
		fprintf( el_fp, msg_scsi_ill_dev_typ, t, sb->name );
	    driver = &jk_ddb;
	    sb->ok = 0;
	    break;
	}
    inq_len = inq[4] + 5;

    /*
     * Make unit protocol from driver->name and port protocol.
     */    

    strcpy( protocol, driver->name );
    protocol[1] = sb->pb->protocol[1];	

    /*
     *  Create a new "file".
     */

    if (iname==NULL){
      set_io_name( name, protocol, sb->node_id * 100 + lun,
                                                             sb->node_id, pb );
    } else {
      strcpy(name,iname);
    }

    /*
     * If we fail to get a name, then the port driver does not want
     * an inode made out of this information.  Think about if I want it
     * out of the lun map or not.  The below if statement may not be
     * needed at all.  
     */
    if ( name[0] == 0 ) {
      scsi_lun_map( sb, CLEAR_LUN, lun); 
      return;
    }

#if MODULAR && ( STARTSHUT || DRIVERSHUT )
    if (cdrom_find) {
	if (t == 5) {		/* is this a cdrom ? */
	    sprintf(cdrom_name, "CDA%d", cdrom_count);
	    ev_write(cdrom_name, name, EV$K_STRING|EV$K_USER|EV$K_NONVOLATILE);
	    cdrom_count++;
	}
    }
#endif

    t = allocinode( name, 1, &inode );
    inode->dva = driver;
    inode->attr = ATTR$M_READ;
    if( driver == &mk_ddb )
	inode->bs = 2048;
    else
	inode->bs = 512;
    /*
     *  If the file already exists, then a unit block is already allocated;
     *  otherwise, we must allocate one.
     */
#if ( STARTSHUT || DRIVERSHUT )
    if( 0 )
#else
    if( t == 1 )
#endif
	ub = inode->misc;
    else
	{
	ub = malloc( sizeof( *ub ), ub );
	ub->inq = malloc( MAX_INQ_SZ, inq );
	}
    ub->inq_len = umin( inq_len, MAX_INQ_SZ );
    memcpy( ub->inq, inq, ub->inq_len );
    ub->sb = sb;
    ub->pb = pb;
    ub->incarn = krn$_unique_number( );
    ub->unit = lun;
    ub->flags = ( inq[1]<<8 ) + inq[0] + 65536;
    if( ( inq_len > 242 ) && 
        (( memcmp( &inq[16], "HSZ70", 5 ) == 0 )    ||
         ( memcmp( &inq[16], "HSZ80", 5 ) == 0 ) )   ) 
        {
        scsi_swap_data( &ub->allocation_class, &inq[242], 4);
        if( ub->allocation_class == 0 ) 
           ub->allocation_class = 0x80000000;
        ub->multibus = ( (inq[96] & 0x20) == 0x20);
        dprintf("Setting allocation class to %d  and multibus to %d\n", 
                             p_args2(ub->allocation_class, ub->multibus) );
        } else {
        ub->allocation_class = 0x80000000; /* no data available code     */
        ub->multibus = 0x80000000;         /* indicate no data available */
        }
    t = inq[3] & 0x0f;
    if( t == 0 )
	{
	/*
	 *  Take the brain-dead TK50 and TZ30 into account.  These devices
	 *  send back only 5 bytes of inquiry data, and the second byte
	 *  distinguishes them apart.
	 */
	if( ( inq_len == 5 ) && ( driver == &mk_ddb ) )
	    {
	    if( inq[1] == 0x80+0x30 )
		strcpy( ub->info, "TZ30" );
	    else if( inq[1] == 0x80+0x50 )
		strcpy( ub->info, "TK50" );
	    else
		strcpy( ub->info, "" );
	    }
	else
	    strcpy( ub->info, "" );
	}
    else
	{
	/*
	 *  Convert the space-padded vendor ID and product ID fields into
	 *  a normal 0-terminated string.
	 */
	np = ub->info;
	if( memcmp( &inq[8], "DEC ", 4 ) == 0 )
	    {
	    memcpy( np, &inq[16], 16 );
	    for( i = 0; i < 16; i++, np++ )
		if( np[0] == ' ' )
		    break;
	    }
	else
	    {
	    memcpy( np, &inq[8], 8 );
	    np += 8;
	    for( i = 0; i < 8; i++, np-- )
		if( np[-1] != ' ' )
		    break;
	    if( inq[16] != ' ' )
		*np++ = ' ';
	    memcpy( np, &inq[16], 16 );
	    np += 16;
	    for( i = 0; i < 16; i++, np-- )
		if( np[-1] != ' ' )
		    break;
	    }
	*np++ = 0;
	sprintf( ub->version, "%4.4s", &inq[32] );
	}

    if( !alias )
        {
        ub->alias[0] = '\0';
        if( ub->allocation_class != 0x80000000 )
          {
          sprintf( ub->alias, "$%d$", ub->allocation_class );
          }
        sprintf( ub->alias+strlen(ub->alias), "%s%s%d",
	        protocol,
	        controller_num_to_id( pb->controller, controller ),
	        sb->node_id * 100 + lun );
        if ( sb->class )
            strcat( ub->alias, "(SCSI)" );
        strupper( ub->alias );
        }
    else
        {
        strcpy( ub->alias, alias );
        }


#if FIBRE_CHANNEL
    if( is_scsi3( pb ) ) 
	if( wwid ) { 
            if( ub->wwid == 0 ) {
	       ub->wwid = malloc( sizeof(struct wwid_hdr)+wwid->hdr.len, wwid );
            }
	    memcpy( ub->wwid, wwid, sizeof(struct wwid_hdr)+wwid->hdr.len );
       } /* wwid */
#endif /* FIBRE_CHANNEL */
    /*
     *  Set up required pointers and back-pointers.  The INODE points to the
     *  UB, the UB points to the SB, the SB points to the UB.
     */
    inode->misc = ub;
    ub->inode = inode;
    if( !sb->first_ub )
	sb->first_ub = ub;
    insq( ub, sb->ub.blink );
    INODE_UNLOCK( inode );
#if ( STARTSHUT || DRIVERSHUT )
    sprintf( ub->string, "%-18s %-24s %5s  %s",
	    name, ub->alias, ub->info, ub->version );
#else
    sprintf( ub->string, "%-24s   %-8s   %24s  %s",
	    name, ub->alias, ub->info, ub->version );
#endif
    insert_ub( ub );
#if !( STARTSHUT || DRIVERSHUT )
    fprintf( el_fp, msg_dev_found, inode->name, ub->info, sb->name );
#endif
    /*
     *  Disk-type devices require some extra handling.  Kick off commands to
     *  spin up the unit and set the block size of the unit to 512.
     */
#if 0
    scsi_send_start_unit( ub );
#endif
    ub->flags -= 65536;
#if ( STARTSHUT || DRIVERSHUT )
    if( sb->verbose )
	printf( "%-18s %-24s %5s %5s\n", ub->inode->name, ub->alias, ub->info, ub->version );
#endif
    }

/*+
 * ============================================================================
 * = scsi_break_vc - make a unit vanish from the system                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine removes all traces of a unit from the system, due to
 *	some fatal error.
 *  
 * FORM OF CALL:
 *  
 *	scsi_break_vc( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct scsi_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
 *	A UB may be freed and its associated INODE released.
 *
-*/
void scsi_break_vc( struct scsi_sb *sb )
    {
    char name[48];
    int scsi_rundown( );

    /*
     *  If there's currently no valid unit, just quit.
     */
    if( sb->state != sb_k_open )
	return;

/* 
#if ( STARTSHUT || DRIVERSHUT )
    krn$_semreinit(&sb->ready_s, 0, 0);
#endif
*/

    /*
     *  Tell others that the destruction is in progress.
     */
    sb->state = sb_k_stall;
    scsi_lun_map( sb, ZERO_MAP, 0 );
    strcpy( name, "rundown_" );
    strcat( name, sb->name );


#if !( STARTSHUT || DRIVERSHUT )
    krn$_create( scsi_rundown, null_procedure, 0, 5, 0, 4096,
	    name, "nl", "r", "nl", "w", "nl", "w", sb );
#else

#if FIBRE_CHANNEL
    krn$_create( scsi_rundown, null_procedure, wwidmgr_mode ? 0 : &sb->ready_s, 5, 0, 4096,
	    name, "nl", "r", "nl", "w", "nl", "w", sb );

    if(!wwidmgr_mode)
       krn$_wait(&sb->ready_s);

#else   /* no FIBRE_CHANNEL */

    krn$_create( scsi_rundown, null_procedure, &sb->ready_s, 5, 0, 4096,
	    name, "nl", "r", "nl", "w", "nl", "w", sb );

    krn$_wait(&sb->ready_s);

#endif /* FIBRE_CHANNEL */

#endif /* STARTSHUT || DRIVERSHUT */

    }

/*+
 * ============================================================================
 * = scsi_rundown - perform VC-related rundown                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles final rundown of a VC.
 *  
 * FORM OF CALL:
 *  
 *	scsi_rundown( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct scsi_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
 *	The unit is run down and eventually deleted.  A free poll is performed.
 *
-*/
void scsi_rundown( struct scsi_sb *sb )
    {
    struct ub *ub;
    struct INODE *inode;

    krn$_wait( &scsi_ub_lock );
    while( ( ub = sb->ub.flink ) != &sb->ub.flink )
	{
	inode = ub->inode;
#if !( STARTSHUT || DRIVERSHUT )
	fprintf( el_fp, msg_dev_lost, inode->name, ub->info, sb->name );
#endif
	if( sb->first_ub == ub )
	    sb->first_ub = 0;
	remove_ub( ub );
	remq( ub );
#if ( STARTSHUT || DRIVERSHUT )
	if( ub->wwid )
            free( ub->wwid, wwid );
	free( ub->inq, inq );
	free( ub, ub );
	inode->misc = 0;
#endif
	}
    krn$_post( &scsi_ub_lock );

    /*
     *  Tell others that the destruction is complete.
     */
    sb->sdtr = 1;
    sb->state = sb_k_closed;
    scsi_lun_map( sb, ZERO_MAP, 0 );
#if !( STARTSHUT || DRIVERSHUT )
    if( sb->send_inquiry )
	sb->send_inquiry( sb );
    else
	scsi_send_inquiry( sb );
#endif

    }

/*+
 * ============================================================================
 * = scsi_analyze_check - analyze a SCSI unit's check condition               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine is called whenever a status of "check" is returned by a
 *	SCSI unit.  It builds and sends a SCSI Request Sense command.  Upon
 *	receipt of the Request Sense data, it determines the correct recovery
 *	procedure and executes it.
 *  
 * FORM OF CALL:
 *  
 *	scsi_analyze_check( ub, unsigned char *cmd, int *out_len, *in_len,
 *			    sense_dat_in, sense_dat_in_len )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_rec_underflow		- the record read was too short
 *	msg_rec_overflow		- the record read was too long
 *	msg_eof				- end of file was found
 *	msg_eot				- end of tape was found
 *	msg_not_ready			- the unit is not ready
 *	msg_no_media			- there is no media in the unit
 *	msg_write_prot			- the media is write protected
 *	msg_scsi_bad_check		- the check condition reported is fatal
 *	msg_scsi_retry			- the command should be retried
 *	others				- other codes returned by subordinates
 *       
 * ARGUMENTS:
 *
 *	struct ub *ub			- unit block
 *	unsigned char *in_cmd		- pointer to Command Out bytes
 *	int *in_dat_out_len		- pointer to Data Out length
 *	int *in_dat_in_len		- pointer to Data In length
 *	unsigned char *sense_dat_in	- pointer to Sense data
 *	unsigned int sense_dat_in	- Sense data length
 *
 * SIDE EFFECTS:
 *
 *	The SCSI unit's check condition is cleared.
 *
-*/
int scsi_analyze_check( struct ub *ub, unsigned char *in_cmd,
	int *in_dat_out_len, int *in_dat_in_len,
	unsigned char *sense_dat_in, int sense_dat_in_len )
    {
    unsigned char cmd[6];
    unsigned char dat_in[MAX_SENSE_SZ];
    char *name;
    int cmd_len;
    int dat_in_len;
    int t;
    int okay;
    struct scsi_sb *sb;

    sb = ub->sb;
    if( ( sb->state == sb_k_open ) && ( ub->inode ) )
	{
	name = ub->inode->name;
	okay = 1;
	}
    else
	{
	name = sb->name;
	okay = 0;
	}
    /*
     *  Use the Sense data (if any) passed in.
     */
    if( sense_dat_in_len )
	{
	memcpy( dat_in, sense_dat_in, sense_dat_in_len );
	dat_in_len = sense_dat_in_len;
	t = msg_success;
	}
    else
	{
	/*
	 *  Fill in the Command Out bytes.
	 */
	cmd_len = sizeof( cmd );
	cmd[0] = scsi_k_cmd_request_sense;
	cmd[1] = 0;
	cmd[2] = 0;
	cmd[3] = 0;
	cmd[4] = sizeof( dat_in );
	cmd[5] = 0;
	dat_in_len = sizeof( dat_in );
	/*
	 *  Send the command.
	 */
	t = scsi_send( ub, cmd, cmd_len, 0, 0, dat_in, &dat_in_len );
	}
    /*
     *  If successful, see what the drive has to say.
     */
    if( t == msg_success )
	{
	if( ( dat_in[0] & 0x7e ) != 0x70 )
	    {
	    err_printf( msg_scsi_ill_err_cod, dat_in[0], name );
	    return( msg_scsi_bad_check );
	    }
	t = dat_in[2] & 0x0f;
	switch( t )
	    {
	    case 0:
		fprintf( el_fp, msg_scsi_sense_key, "No Sense",
			dat_in[12], dat_in[13], name );
		if( dat_in[2] & 0x80 )
		    return( msg_eof );
		else if( dat_in[2] & 0x20 )
		    {
		    scsi_swap_data( &t, &dat_in[3], 4 );
		    if( t > 0 )
			{
			if( in_dat_out_len )
			    *in_dat_out_len -= t;
			if( in_dat_in_len )
			    *in_dat_in_len -= t;
			return( msg_rec_underflow );
			}
		    else if( t < 0 )
			return( msg_rec_overflow );
		    else
			return( msg_success );
		    }
		else
		    return( msg_success );

	    case 1:
		fprintf( el_fp, msg_scsi_sense_key, "Recovered Error",
			dat_in[12], dat_in[13], name );
		return( msg_success );

	    case 2:
		fprintf( el_fp, msg_scsi_sense_key, "Not Ready",
			dat_in[12], dat_in[13], name );
		if( ( dat_in[12] == 0x00 ) && ( dat_in[13] == 0x00 ) )
		    {
		    err_printf( msg_not_ready, name );
		    return( msg_not_ready );
		    }
		if( ( ( dat_in[12] == 0x3a ) && ( dat_in[13] == 0x00 ) ) ||
                    ( ( dat_in[12] == 0x04 ) && ( dat_in[13] == 0x03 ) ) )
		    {
		    err_printf( msg_no_media, name );
		    return( msg_no_media );
		    }
		if( ( dat_in[12] == 0x04 ) && ( dat_in[13] == 0x01 ) )
		    {
		    krn$_sleep( 1000 );
		    return( msg_scsi_retry );
		    }
		if( okay )
		    if( in_cmd[0] != scsi_k_cmd_start_unit )
			{
			t = scsi_send_start_unit( ub );
			if( t == msg_no_conn ) {
		           err_printf( msg_no_conn, name );
			   return ( msg_no_conn );
			 }
			return( msg_scsi_retry );
			}
		err_printf( msg_not_ready, name );
		return( msg_not_ready );

	    case 3:
		fprintf( el_fp, msg_scsi_sense_key, "Medium Error",
			dat_in[12], dat_in[13], name );
		return( msg_scsi_bad_check );

	    case 4:
		fprintf( el_fp, msg_scsi_sense_key, "Hardware Error",
			dat_in[12], dat_in[13], name );
		return( msg_scsi_bad_check );

	    case 5:
		fprintf( el_fp, msg_scsi_sense_key, "Illegal Request",
			dat_in[12], dat_in[13], name );
		if(  (  (in_cmd[0] == scsi_k_cmd_report_luns ) ||
                        (in_cmd[0] == scsi_k_cmd_rd_devid    ) ||
                        (in_cmd[0] == scsi_k_cmd_prout       )  )  && 
                     ( dat_in[12] == 0x20 ) && ( dat_in[13] == 0x00 ) )
		      return( msg_expected_ill_req );
		else if( (in_cmd[0] == scsi_k_cmd_inquiry ) && 
                         (in_cmd[1] == scsi_k_inq_evpd)     &&
                         ( dat_in[12] == 0x24 ) && ( dat_in[13] == 0x00 ) )
		      return( msg_expected_ill_req );
                else 
		  return( msg_scsi_bad_check );

	    case 6:
		fprintf( el_fp, msg_scsi_sense_key, "Unit Attention",
			dat_in[12], dat_in[13], name );
		if( ( dat_in[12] == 0x28 ) && ( dat_in[13] == 0x00 ) )
		    if( okay ) 
			scsi_send_start_unit( ub );
		sb->sdtr = 1;
		return( msg_scsi_retry );

	    case 7:
		fprintf( el_fp, msg_scsi_sense_key, "Data Protect",
			dat_in[12], dat_in[13], name );
		return( msg_scsi_bad_check );

	    case 8:
		fprintf( el_fp, msg_scsi_sense_key, "Blank Check",
			dat_in[12], dat_in[13], name );
		return( msg_eot );

	    default:
	        err_printf( msg_scsi_sense_key_x, t,
			dat_in[12], dat_in[13], name );
		sb->sdtr = 1;
		return( msg_scsi_bad_check );
	    }
	}
    else
	return( t );
    }

/*+
 * ============================================================================
 * = scsi_send - send a command to a SCSI unit                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine sends a command to a SCSI unit, retrying multiple times
 *	in case of bad status (busy, check) being returned.
 *  
 * FORM OF CALL:
 *  
 *	scsi_send( ub, cmd, cmd_len, dat_out, dat_out_len, dat_in, dat_in_len )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_failure			- failure
 *	msg_scsi_bad_status		- the SCSI status was bad
 *	msg_scsi_retries_gone		- all retries have been exhausted
 *	others				- other codes returned by subordinates
 *
 * ARGUMENTS:
 *
 *	struct ub *ub			- unit block
 *	unsigned char *cmd		- pointer to Command Out bytes
 *	int cmd_len			- Command Out length
 *	unsigned char *dat_out		- pointer to Data Out bytes
 *	int *dat_out_len		- pointer to Data Out length
 *	unsigned char *dat_in		- pointer to Data In bytes
 *	int *dat_in_len			- pointer to Data In length
 *
 * SIDE EFFECTS:
 *
-*/
int scsi_send( struct ub *ub, unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int *dat_out_len,
	unsigned char *dat_in, int *dat_in_len )
    {
    unsigned char sts[1];
    unsigned char sense_dat_in[MAX_SENSE_SZ];
    int out_len;
    int in_len;
    int sense_dat_in_len;
    int i;
    int t;
    struct scsi_sb *sb;

    sb = ub->sb;
    if( dat_out_len )
	out_len = *dat_out_len;
    else
	out_len = 0;
    if( dat_in_len )
	in_len = *dat_in_len;
    else
	in_len = 0;

    /*
     *  Try this a good number of times.
     */
    for( i = 0; i < 60; i++ )
	{
	/*
	 *  Call into the port driver to attempt to send this command.  If
	 *  the port driver failed to send it, then we fail also.
	 */
	sense_dat_in_len = 0;
	t = sb->command( sb, ub->unit, cmd, cmd_len, dat_out, out_len,
		dat_in, in_len, sts, sense_dat_in, &sense_dat_in_len );
	if( t != msg_success )
	    break;
	/*
	 *  The command got sent; look at the returned Status In byte.
	 *  Return success if the status is good; retry (after a delay) if
	 *  the status is busy or check; fail otherwise.
	 */
	if( sts[0] == scsi_k_sts_good )
	    return( msg_success );
	else if( sts[0] == scsi_k_sts_busy )
	    {
	    if( ( i > 10 ) && killpending( ) )
		break;
	    krn$_sleep( 1000 );
	    }
	else if( sts[0] == scsi_k_sts_check )
	    {
	    if( ub->unit )
		if( cmd[0] == scsi_k_cmd_inquiry ) 
		    return( msg_failure );
	    if( cmd[0] == scsi_k_cmd_request_sense )
		return( msg_scsi_bad_check );
	    t = scsi_analyze_check( ub, cmd, dat_out_len, dat_in_len,
		    sense_dat_in, sense_dat_in_len );
	    if( ( t == msg_success )
		    || ( t == msg_rec_underflow )
		    || ( t == msg_rec_overflow )
		    || ( t == msg_eof )
		    || ( t == msg_eot )
		    || ( t == msg_no_media )
		    || ( t == msg_no_conn )
		    || ( t == msg_expected_ill_req )
		    || ( t == msg_not_ready ) )
		return( t );
	    if( t != msg_scsi_retry )
		break;
	    }
	else if( sts[0] == scsi_k_sts_reservation_conflict )
            {
	    if( cmd[0] == scsi_k_cmd_prout )
               {
	       t = msg_reservation_conflict;
	       break;
               }
	    t = scsi_send_prout( ub );
	    if( ( t != msg_success ) || (i > 2 ) )  /* limit retries to 2 */
               {
	       t = msg_reservation_conflict;
	       break;
               }
            else
               {
               t = msg_scsi_retry;
               }
            }
	else
	    {
	    err_printf( msg_scsi_bad_status, sts[0], sb->name ); 
	    t = msg_scsi_bad_status;
	    break;
	    }
	}

    if(t == msg_reservation_conflict)
      {
      char *name;
      if( ( sb->state == sb_k_open ) && ( ub->inode ) )
         name = ub->inode->name;
      else
         name = sb->name;
      err_printf( msg_reservation_conflict, name );
      return( msg_reservation_conflict );
      }

    /*
     *  If the current status is "success", we must keep encountering a SCSI
     *  status of either busy or check; turn this into a status of "retries
     *  exhausted".
     */
    if( t == msg_success )
	{
	err_printf( msg_scsi_retries_gone, sb->name );
	t = msg_scsi_retries_gone;
	}
    /*
     *  We've either used up all retries, or we've hit a hard error, so we've
     *  lost contact with the device.  Let the world know.
     */
#if 0
    err_printf( msg_scsi_send_fail, sb->name );
#endif
    /*
     * 20-jan-1999 Dennis Mazur
     * If a report_luns is not supported, the above code will handle that
     * in the msg_expected_ill_req flow.  That error is benign since not all 
     * devices support report_luns, and we would not break_vc.  However, if 
     * the command failed to issue, it is desirable to break_vc.  A report_lun
     * command is always issued at lun 0 (i.e. ub->unit would be 0).   However,
     * there may not be a device behind lun 0, so scsi_lun_map may not be set.
     * I have added the 2nd clause to this 'if' statement to force a break_vc
     * on a report_lun failure.
     */
    if( scsi_lun_map( sb, GET_LUN, ub->unit ) || (cmd[0] == scsi_k_cmd_report_luns) )
	{
	fprintf( el_fp, msg_scsi_send_fail, sb->name );
	scsi_break_vc( sb );
	}
    return( t );
    }

/*+
 * ============================================================================
 * = scsi_swap - swap bytes (little-endian to big-endian or vice-versa)       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine swaps bytes from little-endian order to big-endian
 *	order, or vice-versa (the swapping is commutative).  No check is
 *	made concerning overlap between the source and destination fields.
 *  
 * FORM OF CALL:
 *  
 *	scsi_swap( dst, src, len )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	unsigned char *dst		- pointer to destination
 *	unsigned char *src		- pointer to source
 *	int len				- number of bytes to swap
 *
 * SIDE EFFECTS:
 *
-*/
void scsi_swap_data( unsigned char *dst, unsigned char *src, int len )
    {
    src += len;
    while( --len >= 0 )
	*dst++ = *--src;
    }



int is_scsi3( struct pb *pb )
{
  int scsi3 = FALSE;
  if ( !strncmp(pb->hardware_name, "KGPSA", 5 ) ) {
    scsi3 = TRUE;
  }
  return scsi3;
}

/*+
 * ============================================================================
 * = scsi_lun_map - SCSI2 lun_map management routine                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * This routing manages the lun_map state.  It used to be a simple bit
 * on/off manipulation within an int (prior to Mar 25, 97).   However,
 * In SCSI3, there can be a lot of luns, and the field for the lun_map
 * in scsi_sb is not big enough, and other routines substitute for this
 * routine depending on the port driver and is accessed thru the
 * indication from is_scsi3. 
 *
 * This routine is provided as the default routine for lun management 
 * and is valid for a maximum of 32 luns.
 *
 *  
 * FORM OF CALL:
 *  
 *	scsi_lun_map( sb, func, lun )
 *
 *        func -  
 *               GET_LUN   - return the lun_map state
 *               SET_LUN   - set the lun state to 1  (configured)
 *               CLEAR_LUN - set the lun state to 0  (not configured)
 *               ZERO_MAP  - set all luns to state 0 (not configured)
 *  
 * RETURNS:
 *
 *    func -                  return
 *               GET_LUN   -  0 or 1
 *               SET_LUN   -  nothing
 *	         CLEAR_LUN -  nothing
 *               ZERO_MAP  -  nothing
 *       
 * ARGUMENTS:
 *
 *	struct scsi_sb *sb		- pointer to an sb
 *	int func    		        - function code
 *	int lun				- lun to operate on
 *
 * SIDE EFFECTS:
 *
-*/

int scsi_lun_map( struct scsi_sb *sb, int function, int lun )
{
  int i;
  int *lun_map;
  int lun_sz;
  struct scsi3_sb *sb3;

  if ( is_scsi3(sb->pb) ){
    sb3 = sb;
    lun_map = &sb3->lun_ext[0];
    lun_sz = 8;
  } else {
    lun_map = &sb->lun_map;
    lun_sz = 1;
  }

 switch ( function ) {
    case GET_LUN:
       return getbit(lun, lun_map);
       break;
    case SET_LUN:
       setbit( 1, lun, lun_map);
       break;
    case CLEAR_LUN:
       setbit( 0, lun, lun_map);
       break;
    case ZERO_MAP:
       for (i=0; i<lun_sz; i++){
         lun_map[i] = 0;
       }
       break;
 }
    return msg_success;
}

/*
 *  This routine will look up in the fc ev's for a match on the
 *  device and path.
 *
 *  NOTE: Still needs Sicoloa LUN WWN enhancements
 */

#if FIBRE_CHANNEL
/*+
 * ============================================================================
 * = scsi_get_alias - Create a device name associated with wwid information   =
 * ============================================================================
 *
 * OVERVIEW:
 *    
 *    Create a device name for a wwid identified device.   Since wwids are
 *    large and do not fit into the standard console naming scheme, they
 *    have to be aliased to something more standard.   Device names are 
 *    usually created in scsi_new_unit, but its algorithm to name a device
 *    does not work for devices that have user define aliases or alias
 *    that are not based on the lun.  This routine just implements a different
 *    algorithm for creating a device name for devices with wwids.  The
 *    resultant name from this routine is then passed into scsi_new_unit.
 *
 *  
 * FORM OF CALL:
 *  
 *    scsi_get_alias( sb, lun, wwid, name )
 *
 *  
 * RETURNS:
 *
 *    msg_success
 *    msg_failure
 *       
 * ARGUMENTS:
 *
 *    struct scsi_sb *sb		- pointer to an sb
 *    int lun				- lun to operate on
 *    union wwid *wwid                - wwid of the lun
 *    char *name                      - buffer to return asciz name
 *    char *alias                     - buffer to return a VMS like name
 *
 * SIDE EFFECTS:
 *
 *    None.
 *
-*/
int scsi3_get_alias( struct scsi3_sb *sb, int lun, union wwid *wwid, 
                                                      char *name, char *alias )
{
  int status = msg_failure;
  int idx;
  char c;
  struct pb *pb;
  int busslot, hose;
  int filter_index;
  char srm[128];  
  struct registered_wwid *w;

  w = find_wwidfilter_from_wwids( wwid, &idx );
  if (w) {
    pb = sb->pb;

    /*
     * We don't have to go thru dev2file() and could just use set_io_name.
     * But going thru dev2file helps flush out bugs especially since the
     * dev2file is not a common console routine.   But dev2file can only
     * be called for wwids that are backed by ev's.  Only the 
     * registered wwids have ev, (the temporary wwids created in wwidmgr_mode
     * do not have backing ev, just because the ev's were not necessary in
     * wwidmgr_mode since temporary wwids will never appear in an SRM
     * specifier).  Dev2file pulls info out of the ev.  For temporary
     * wwids, set_io_name is used.
     */
    if (wwids[idx]->evnum != -1 )
     {
       sprintf(srm, "SCSI3 %d %d %d %d %d 0 0 @wwid%d", 
         pb->hose, 
         pb->bus * 1000 + pb->function * 100 + pb->slot,
         pb->channel,
         sb->node_id, 
         0,
         wwids[idx]->evnum);
       dev2file ( &srm, name );
    } else {
       set_io_name( name, "dg", wwids[idx]->unit, 
                                 wwids[idx]->collision*1000+sb->node_id, pb );
    }

    /*
     * Create a string to end up in the ub->alias field and visible on 
     * show device.  This sting is an attempt to display a VMS-like
     * device name.
     */
    sprintf( alias, "$%d$DGA%d", wwids[idx]->collision, wwids[idx]->unit );

    status = msg_success;
  }
  return status;
}


/*+
 * ============================================================================
 * = scsi_get_page - Get SCSI inquiry pages                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *    This routine does the same thing as scsi_send_inquiry but more.
 *    It is used to get p80 and p83 information used in composing the wwid.
 *    It is also coded so that it can be used to get mode pages.
 *    
 *    Unlike most of the other scsi routines, this routine is not passed
 *    a ub.  Just as scsi_inquiry creates a temporary ub, so does this
 *    routine.  That has the advantage that this routine is conveniently
 *    used via ">>> call" if you know the sb.
 *
 *  
 * FORM OF CALL:
 *  
 *    scsi_get_page( sb, lun, wwid, name )
 *
 *  
 * RETURNS:
 *
 *    msg_success
 *    msg_failure
 *       
 * ARGUMENTS:
 *
 *    struct scsi_sb *sb	- pointer to an sb
 *    int lun			- lun to operate on
 *    int cmdcode               - either inquiry or mode page command code
 *    int page                  - page number
 *    int evpd                  - vital product date bit
 *    unsigned char *dat_in     - buffer to store page
 *    int *dat_in_len           - pointer to length of dat_in buffer
 *
 * SIDE EFFECTS:
 *
 *    None.
 *
 *
-*/
int scsi_get_page( struct scsi_sb *sb , int lun, int cmdcode, int page, 
 	                   int evpd, unsigned char *dat_in, int *dat_in_len )
    {
    unsigned char cmd[6];
    int cmd_len;
    int i;
    int t;
    struct ub *ub;

    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = cmdcode;
    cmd[1] = 0;                 
    cmd[2] = page;
    cmd[3] = 0;
    cmd[4] = *dat_in_len;
    cmd[5] = 0;

    if ( evpd ) 
      cmd[1] = scsi_k_inq_evpd;                 
    

    if ( cmdcode == scsi_k_cmd_mode_sense ) {
      cmd[1] = 8;               /* disable block descriptors */
    }
    ub = malloc( sizeof( *ub ), ub );
    ub->sb = sb;
    ub->pb = sb->pb;
    ub->unit = lun;
    t = scsi_send( ub, cmd, cmd_len, 0, 0, dat_in, dat_in_len );
    free( ub, ub );
    return t;
    }


/*+
 * ============================================================================
 * = scsi3_get_wwid - derive wwid from the data available                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      This routine implements the Digital's (Compaq's) algorithm for 
 *      creating a WWID for a device.  Some devices support SCSI3 WWIDs
 *      and those are used.   Other devices (like the Seagate drives) do
 *      not have a p83 and a the algorithm creates a unique id from other
 *      pieces of data.
 *
 *      TODO: This code is not up to current specification.   The below
 *            comment is current (30-Oct-98) but the code is not up to
 *            the comment's specifications.
 *
 *  
 *  "Type" field in the WWID Header
 *  -------------------------------
 *  
 *  I'd like to suggest a change, and some additions, to the definition of the 
 *  type field.  Fortunately,  this shouldn't require anyone to change any
 *  code, just structure definitions.
 *  
 *  The current WWID header definition is described in a mail message from 
 *  James (attached for reference).  The immediate problem is that there are 
 *  four different 64-bit WWID formats that can be delivered in page 83:
 *  
 *  	page 83, ID type 3, NAA 0101	IEEE Registered
 *  	page 83, ID type 3, NAA 0010	IEEE 48-bit, vendor extended to 64-bits
 *  	page 83, ID type 3, NAA 0001	IEEE 48-bit, zero extended to 64-bits
 *  	page 83, ID type 2		IEEE EUI-64
 *  
 *  but we only have one type value defined: SCSI_WWID_PG83_IEEE64.  So, we 
 *  need three additional type definitions, and the current 64-bit type needs 
 *  to be renamed so that it is clear which ID it refers to.
 *  
 *  To help avoid this problem in the future, I'd like to suggest that whenever 
 *  an ID type has a well-defined name, we use the name rather than where the 
 *  ID is obtained from.
 *  
 *  The following table shows the changes and additions:
 *  
 *  type  new name	           old name		   location
 *  ----  --------             --------                --------
 *  1  SCSI_WWID_IEEE_REGEXT   SCSI_WWID_PG83_IEEE128  page 83, ID type 3, NAA 0110
 *  2  SCSI_WWID_IEEE_REG  	   SCSI_WWID_PG83_IEEE64   page 83, ID type 3, NAA 0101
 *  3  SCSI_WWID_PG83TY1_VID   SCSI_WWID_PG83_VENDOR   PID+Page 83, ID type 1
 *  
 *  4  SCSI_WWID_PG80_SERIALNO SCSI_WWID_PG80_SERIALNO VID+PID+serial number 
 *  						   from Page 80
 *  
 *  5  SCSI_WWID_INQ_SERIALNO  SCSI_WWID_SERIALNO 	   VID+PID+serial number from 
 *  						   Standard Inquiry data, 
 *  						   vendor-specific area
 *  
 *  6  SCSI_WWID_TGT_IDENT     SCSI_WWID_TGT_IDENT     VID+PID+persistent tgt ID
 *  						   (eg. N_Port WWID, or SCAM ID)
 *  						   +LUN 
 *  
 *  7  SCSI_WWID_BTL           SCSI_WWID_BTL   	   VID+PID+bus number+tgt ID+LUN
 *  
 *! 8  SCSI_WWID_DEC_UNIQUE    SCSI_WWID_DEC_UNIQUE    currently, HSZ-specific
 *! 						   serial numbers etc. 
 *  9  SCSI_WWID_IEEE48_V_EXT                          page 83, ID type 3, NAA 0010	
 *  10 SCSI_WWID_IEEE48_0_EXT			   page 83, ID type 3, NAA 0001	
 *  11 SCSI_WWID_IEEE_EUI64 			   page 83, ID type 2
 *  11 SCSI_WWID_MAX_TYPE
 *  
 *! Display Issues 
 *! --------------
 *! The types SCSI_WWID_TGT_IDENT and SCSI_WWID_BTL contain a mixture of ASCII and
 *! binary data.  The rule will be that any identifier that contains ASCII data
 *! will be converted as necessary to ASCII for display.
 *  
 *  WWID Precedence
 *  ---------------
 *! Devices may supply multiple identifiers (for example: multiple entries in page
 *! 83, plus page 80, and BTL).  We have to agree on which one gets used.  Here's a
 *! determination of the precedence that shall be used:
 *! 
 *! 1   SCSI_WWID_IEEE_REGEXT    page 83, ID type 3, NAA 0110
 *! 2   SCSI_WWID_IEEE_REG       page 83, ID type 3, NAA 0101
 *! 3   SCSI_WWID_IEEE48_V_EXT   page 83, ID type 3, NAA 0010	
 *! 4   SCSI_WWID_IEEE48_0_EXT   page 83, ID type 3, NAA 0001	
 *! 5   SCSI_WWID_IEEE_EUI64     page 83, ID type 2
 *! 6   SCSI_WWID_PG83TY1_VID    PID+Page 83, ID type 1
 *! 7   SCSI_WWID_PG80_SERIALNO  VID+PID+serial number 
 *! 			     from Page 80
 *! 8   SCSI_WWID_TGT_IDENT      VID+PID+persistent tgt ID+LUN
 *! 9   SCSI_WWID_INQ_SERIALNO   VID+PID+serial number from 
 *! 			     Standard Inquiry data, 
 *! 			     vendor-specific area
 *! 10  SCSI_WWID_DEC_UNIQUE     HSZ-specific
 *! 11  SCSI_WWID_BTL            VID+PID+bus number+tgt ID+LUN
 *! 
 *! Note that if there are multiple IDs in the page 83 data, then the first one
 *! with the highest precedence shall be used.
 *! 
 *! One of the goals of this ordering is to position the identifiers that require
 *! a table of product-specific information as low as possible (e.g. numbers 9 and
 *! 10).  It is assumed that the console will not have such a table.
 *! 
 *! OVMS will implement precedence 1 through 7.
 *! DUNIX has asked the console to implement through 8.
 *! DUNIX will implement all 11.
 *! 
 *  
 *  A New Field in the Device ID Page
 *  ---------------------------------
 *  
 *  SPC-2 defines a new field in the Identification Descriptor, which is
 *  returned in page 83.  The field is called "Association", and is located in
 *  byte 1, bits 4 and 5.  If zero, the descriptor is associated with the
 *  device.  If one, the descriptor is associated with the port that received
 *  the request.  We must check this field, and only use the Identification
 *  Descriptor as the basis for a WWID when the field is zero.
 *  
 *  Reminder regarding invalid Inquiry data
 *  ---------------------------------------
 *  
 *  SPC says that a device can return zero or ASCII spaces in fields that it is
 *  not yet ready to supply (usually because the info. is on media that is
 *  spinning up).  We must check for this and never accept bad data as a WWID.
 *  I'd suggest that if the field is ASCII we reject a response of all zeros or
 *  of all ASCII spaces.  If the field is binary we reject all zeros.  
 *  
 *  
 * FORM OF CALL:
 *  
 *	scsi3_get_wwid( sb, lun, wwid )  
 *  
 * RETURNS:
 *
 *      msg_success
 *      msg_failure
 *       
 * ARGUMENTS:
 *
 *      wwid - will be set to a value by an algorith which both
 *             the OS and the console have agreed upon.
 *
 * SIDE EFFECTS:
 *    
 *
-*/

int scsi3_get_wwid( struct scsi3_sb *sb, int lun, union wwid *wwid )
{
    int i,t;
    int status = msg_failure;
    unsigned char dat_in[MAX_INQ_SZ];
    int dat_in_len;
    int remaining;
    int p83_supported = 0;
    struct inquiry_p0 *p0 = &dat_in;
    struct inquiry_p80 *p80 = &dat_in;
    struct inquiry_p83 *p83 = &dat_in;
    struct inquiry_standard *standard = &dat_in;
    struct p83_idlist *p83id;
    struct PCB *pcb = getpcb();


    dat_in_len = sizeof( dat_in );

    dprintf("----------------------------------------------------------\n", p_args0);
    dprintf("scsi3_get_wwid - scsi_get_page sb: %x lun: %x %s \n", 
                                        p_args3( sb, lun, pcb->pcb$b_name ) );


#if LOOKING_FOR_SUPPORTED_PAGES

    t = scsi_get_page( sb, 0, scsi_k_cmd_inquiry, 0x00, scsi_k_inq_evpd, 
                                                          dat_in, &dat_in_len);
    
    for( i=0; i < p0->page_len; i++)
     {
       dprintf("support page %x\n", p0->page_list[i] );
       if( p0->page_list[i] == 0x83 ) p83_supported = 1;
     }
#endif

    t = scsi_get_page( sb, lun, scsi_k_cmd_inquiry, 0x83, scsi_k_inq_evpd,
                                                          dat_in, &dat_in_len);
    if ( t == msg_success ) {
      dprintf("Page 83 access succeeded\n", p_args0 );
      dprintf("Peripheral device & qual: %x\n", p_args1(p83->type_qual) );
      dprintf("page_code: %x\n", p_args1(p83->page_code) );
      dprintf("page_len: %x\n", p_args1(p83->page_len) );
      dprintf("idlist[0]:(%x) %x\n", p_args2(&p83->idlist[0], p83->idlist[0]) );
      
      remaining = p83->page_len;
      p83id = p83->idlist;
      while( remaining ) {
         dprintf("\t code_set:%x \n", p_args1(p83id->code_set) );
         dprintf("\t idtype:%x\n", p_args1(p83id->idtype) );
	 dprintf("\t page_len:%x\n", p_args1(p83id->page_len) );
	 for( i = 0; i < p83id->page_len; i++ ) {
	   dprintf("%02x", p_args1(p83id->id[i]) );
	 }
         dprintf("\n", p_args0);
	 switch (p83id->idtype) {
	   case P83_ID_TYPE_NOAUTHORITY:
	   case P83_ID_TYPE_IEEE64:
	       wwid->hdr.type = SCSI_WWID_PG83_IEEE128;
	       wwid->hdr.len  = 16;
	       wwid->hdr.ascii = 0;
	       memcpy( &wwid->ieee128.ieee, p83id->id, 16 );
	       return msg_success;
	       break;
	   case P83_ID_TYPE_VENDORID:
	   case P83_ID_TYPE_IEEE_EXTENDED:
	       break;
	 } /* switch */
	 remaining -= (p83id->page_len + 4); /* size of current id entry */
	 dprintf("new remaining %x\n", remaining);
	 p83id = &p83id->id[p83id->page_len];
      } /* while */
      return msg_failure;
    } 

    dprintf("FAILURE to read P.83.  Going after page 80.\n", p_args0 );
    t = scsi_get_page( sb, lun, scsi_k_cmd_inquiry, 0x80, scsi_k_inq_evpd,
                                                          dat_in, &dat_in_len);
    if ( t == msg_success ) {
      dprintf("Page 80 access succeeded\n", p_args0 );
      dprintf("type_qual: %x \n", p_args1(p80->type_qual) );
      dprintf("page_code: %x \n", p_args1(p80->page_code) );
      dprintf("rsvd: %x \n", p_args1(p80->rsvd) );
      dprintf("page_len: %x \n", p_args1(p80->page_len) );
      dprintf("serial_number: |%s| \n", p_args1(p80->serial_number) );
      wwid->hdr.type  = SCSI_WWID_PG80_SERIALNO;
      wwid->hdr.ascii = 1;
      strncpy( wwid->serialno.sn, p80->serial_number, p80->page_len );
      wwid->hdr.len   = p80->page_len;

      /* 
       * TODO: strlen vs byte field len 
       *  Have to check out lengths before writing the ev
       */

      t = scsi_get_page( sb, lun, scsi_k_cmd_inquiry, 0, scsi_k_inq_not_evpd,
                                                          dat_in, &dat_in_len);
      strncpy( wwid->serialno.vid, standard->vendor_id, WWID_VID_SZ );
      wwid->hdr.len  += WWID_VID_SZ;

      strncpy( wwid->serialno.pid, standard->product_id, WWID_PID_SZ );
      wwid->hdr.len  += WWID_PID_SZ;
      return msg_success;
    } 

    dprintf("FAILURE to read P.80\n", p_args0);
    return msg_failure;
}

/*+
 * ============================================================================
 * = scsi3_get_udid - get the device identifier from the unit                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *    Devices have WWIDs of 128 bits.  Since that is a pain to work with,
 *    Tom Coughlan proposed this new command (which was accepted by the 
 *    SCSI committee).  Devices can have a small integer "unit identifier".
 *    There is a partner SCSI command to set the udid which console does not
 *    have to implement.  In our configurations, the udid is set by hand
 *    at the HSx80 console CLI.  
 *
 *    This small value is much easier to use than a WWID.  However, the 
 *    drawbacks are that it must be managed by the system manager to
 *    guarantee that all devices in the configuration are set to a unique
 *    udid.   
 *
 *    This is a new command and all devices do not support it.  The console
 *    allows this command to fail without causing a scsi_break_vc.  
 *    Upon an illegal command failure, scsi_analyze will return
 *    a msg_expected_ill_req error.
 *
 *    At this time (30-Oct-98) Unix is not using the udid at all and they
 *    rely strictly on the wwid.  VMS uses the udid and does nothing with
 *    the wwid.  Console is wwid based in all its internals, but it does
 *    acknowledge the udid in the wwidmgr to make device selection easy.
 *
 *    VMS has a maximum UDID value is 32767 (7FFF hex).   
 *  
 * FORM OF CALL:
 *  
 *    scsi_get_udid( ub, udid )
 *
 *  
 * RETURNS:
 *
 *    msg_success
 *    msg_failure
 *       
 * ARGUMENTS:
 *
 *    struct ub *ub	- pointer to an ub
 *    int *udid		- pointer to a unit identifier
 *
 * SIDE EFFECTS:
 *
 *    None.
 *
 *
-*/
int scsi3_get_udid( struct ub *ub, int *udid )
{
  int i,j;
  int t;
  int cmd_len;
  int dat_in_len;
  int valid;
  int idlen;
  unsigned char cmd[10];
  unsigned char dat_in[MAX_EXT_INQ_SZ];
  unsigned char *id;

  /*
   * Build the cdb
   */
  memset( &cmd, 0, sizeof( cmd) );
  cmd_len = sizeof( cmd );
  cmd[0] = scsi_k_cmd_rd_devid;
  dat_in_len = sizeof( dat_in );
  scsi_swap_data( &cmd[6], &dat_in_len, 2 );

  /*
   * Issue the SCSI command to the unit
   */
  t = scsi_send( ub, cmd, cmd_len, 0, 0, dat_in, &dat_in_len );
  if( t != msg_success ) 
     return msg_failure;

  /*
   * Check the valid bit
   */
  if (!dat_in[2])
     return msg_failure;

  idlen = dat_in[3];
  id = &dat_in[4];
  while ((*id == 0) && (idlen > 0))
    {
    id++;
    idlen--;
    }

  /*
   * The ID cannot be zero or greater than 7FFF.  Check its length.
   */
  if ((idlen <= 0) || (idlen > 2))
     return msg_failure;

  /*
   * Copy the UDID byte-by-byte into the location pointed to by the address
   * passed in.  Reverse the byte order so that the LSB comes first and store 
   * in UDID.
   */
  *udid = 0;
  for (i = 0; i < idlen; i++)
      {
      *udid <<= 8;
      *udid += *id++;
      }

  /*
   * Maximum UDID value is 32767 (7FFF hex).
   */
  if (*udid > MAX_UDID_VALUE)
      return msg_failure;


  return msg_success;
}
#endif /* FIBRE_CHANNEL */


#if MODULAR & (!FIBRE_CHANNEL)
/* 
 * stub out scsi_get_udid
 */
int scsi3_get_udid( struct ub *ub, int *udid )
{
 return msg_failure;
}
#endif


/*+
 * ============================================================================
 * = scsi3_send_prout - persistent reserve register with a device             =
 * ============================================================================
 *
 *
 * OVERVIEW:
 *
 *    In a cluster environment, the OSes will be using the feature of
 *    Persistent Reserve.  Reads will always work, but console write access
 *    will only be allowed if console registers with the dump lun.
 *    (see afw_eco 1276.0)
 *
 *    This is a new command and all devices do not support it.  The console
 *    allows this command to fail without causing a scsi_break_vc.  
 *    Upon an illegal command failure, scsi_analyze will return
 *    a msg_expected_ill_req error.
 *
 *  
 * FORM OF CALL:
 *  
 *    scsi_send_prout( ub )
 *
 *  
 * RETURNS:
 *
 *    status from scsi_send
 *       
 * ARGUMENTS:
 *
 *    struct ub *ub	- pointer to an ub
 *    int *udid		- pointer to a unit identifier
 *
 * SIDE EFFECTS:
 *
 *    None.
 *
 *
 *
-*/
int scsi_send_prout( struct ub *ub )
{
  int i,j;
  int t;
  int cmd_len, dat_out_len;
  unsigned __int64 action_key;
  unsigned char cmd[10];
  unsigned char dat_out[24];

  memset( &cmd, 0, sizeof( cmd) );
  memset( &dat_out, 0, sizeof(dat_out) );
  cmd_len = sizeof( cmd );
  dat_out_len = sizeof( dat_out );
  cmd[0] = scsi_k_cmd_prout;
  cmd[1] = scsi_k_pr_action_register; 
  cmd[2] = (scsi_k_pr_scope_lu << 4) | scsi_k_pr_type_shared; 
  cmd[8] = sizeof( dat_out );
  cmd[9] = 0; 

  action_key = PERSISTENT_RESERVE_CONSOLE_KEY;

  scsi_swap_data( &dat_out[8], &action_key, 8);

  t = scsi_send( ub, cmd, cmd_len, dat_out, &dat_out_len, 0, 0 );
  return t;
}


#if IN_DEVELOPMENT
void scsi_send_prin( struct ub *ub, int action)
{
  int i,j;
  int t;
  int cmd_len, dat_in_len;
  unsigned char cmd[10];
  unsigned char dat_in[100];

  cmd_len = sizeof( cmd );
  dat_in_len = sizeof( dat_in );
  cmd[0] = scsi_k_cmd_prin;
  cmd[1] = action; 
  cmd[8] = sizeof( dat_in );
  cmd[9] = 0; 


  t = scsi_send( ub, cmd, cmd_len, 0, 0, dat_in, &dat_in_len );
   if( t != msg_success )
     pprintf("Well this didn't work\n");
   else
     {	
       pprintf("dat_in: %x  len: %x\n", dat_in, dat_in_len );
       do_bpt();
     }
}

#endif
