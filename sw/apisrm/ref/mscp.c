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
 * Abstract:	MSCP class driver
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	pel	22-Dec-1992	Copy get unit status response data to ub for 
 *				use in FRU table.
 *
 *	jeg	07-Dec-1992	last byte to be read/written must be < length
 *				of file 
 *
 *	ajb	11-Dec-1991	can't have have message numbers in case
 *				statements, since they aren't compile time
 *				constants.
 *
 *	pel	12-Mar-1991	let fopen/close incr/decr inode ref & collapse
 *				wrt_ref, rd_ref into just ref.
 *
 *	sfs	19-Feb-1991	Fix calls to ALLOCINODE.  Remove unneeded code
 *				in FD_OPEN.  Use common routines to manipulate
 *				reference counts.  Make INODES write protected
 *				by default.
 *
 *	sfs	06-Feb-1991	Implement du_write.
 *
 *	ajb	04-Feb-1991	Change calling sequence to xx_read and xx_write
 *
 *	pel	08-Jan-1991	handle allocinode amibiguous filename return.
 *
 *	sfs	04-Dec-1990	Fix device lost message.
 *
 *	pel	14-Nov-1990	Add new arguments to allocinode call.
 *				Do allocinodes in dk/mk_open.
 *
 *	ajb	08-Nov-1990	Change calling sequence to krn$_create
 *
 *	pel	31-Oct-1990	add one level of indirection to fp->offset
 *
 *	pel	07-Sep-1990	Call freeinode.
 *
 *	sfs	14-Aug-1990	Complete command timeout work.
 *
 *	sfs	01-Aug-1990	Replace individual system-specific polling
 *				tasks with a single polling task, driven by
 *				a queue.
 *
 *	sfs	11-Jul-1990	Add command timeout processing.
 *
 *	sfs	06-Jul-1990	Work on open/read/close.
 *
 *	sfs	05-Jun-1990	Add support for aborting commands due to
 *				connection errors.
 *
 *	sfs	30-May-1990	Initial entry.
 */

#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define free(x) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)
#define nop

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:mscp_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:cb_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:ctype.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$inc:kernel_entry.h"

#if MODULAR
extern struct DDB *du_ddb_ptr;
extern struct DDB *mu_ddb_ptr;
#endif

extern struct QUEUE mscp_cbs;
struct QUEUE *mscp_cbs_ptr = &mscp_cbs;

int mscp_mode;

extern struct SEMAPHORE scs_lock;
extern int cbip;
extern null_procedure( );
extern struct FILE *el_fp;
extern int mscp_controllers;

int mscp_delete( );
int mscp_setmode( );

int du_read( );
int du_write( );
int du_open( );
int du_close( );

struct DDB du_ddb = {
	"du",			/* how this routine wants to be called	*/
	du_read,		/* read routine				*/
	du_write,		/* write routine			*/
	du_open,		/* open routine				*/
	du_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	mscp_delete,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	mscp_setmode,		/* setmode				*/
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

int mu_read( );
int mu_write( );
int mu_open( );
int mu_close( );

struct DDB mu_ddb = {
	"mu",			/* how this routine wants to be called	*/
	mu_read,		/* read routine				*/
	mu_write,		/* write routine			*/
	mu_open,		/* open routine				*/
	mu_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	mscp_delete,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	mscp_setmode,		/* setmode				*/
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

/*+
 * ============================================================================
 * = du_read - read a file                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine reads from an open file.  Since no "file" structure is
 *	provided, a "file" is no more than a raw device.  Only full-block
 *	reads are allowed; this implies that both the current file offset and
 *	the number of bytes to read must be an integral multiple of 512.
 *  
 * FORM OF CALL:
 *  
 *	du_read( fp, ilen, inum, c )
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
du_read( struct FILE *fp, int ilen, int inum, unsigned char *c )
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
	    t = du_read_sub( fp, ilen, inum, c );
	    if( t )
		return( t );
	    t = fp->status;
	    if( ( t != msg_bad_device ) && ( t != msg_mscp_no_conn ) )
		break;
	    krn$_sleep( 100 );
	    du_close( fp );
	    t = du_open( fp, 0, 0, 0 );
	    if( t != msg_success )
		break;
	    }
	return( 0 );
	}
    else
	return( du_read_sub( fp, ilen, inum, c ) );
    }

du_read_sub( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    int len;
    int lbn;
    int t;
    struct INODE *inode;
    struct ub *ub;
    struct cb *cb;
    struct rsp *rsp;

    len = ilen * inum;
    /*
     *  Acquire the SCS lock, and then make sure the device is still valid (if
     *  it isn't, just return 0).  The device may be invalid if the connection
     *  to the remote server has been broken (recently).
     */
    krn$_wait( &scs_lock );
    t = mscp_unit_okay( fp, 1 );
    if( t == msg_success )
	{
	/*
	 *  The file is open.  The MISC field in the INODE structure is used
	 *  to hold a UB pointer; use this to find the CB for this device.
	 */
	lbn = *(INT *)fp->offset / 512;
	inode = fp->ip;
	ub = inode->misc;
	rsp = fp->misc;
	cb = rsp->cb;
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
	    t = mscp_read_cmd( cb, rsp, ub, c, &len, lbn );
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
     *  read.  Release the SCS lock, and return that number.
     */
    krn$_post( &scs_lock );
    return( t );
    }

/*+
 * ============================================================================
 * = du_write - write a file                                                  =
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
 *	du_write( fp, ilen, inum, c )
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
du_write( struct FILE *fp, int ilen, int inum, unsigned char *c )
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
	    t = du_write_sub( fp, ilen, inum, c );
	    if( t )
		return( t );
	    t = fp->status;
	    if( ( t != msg_bad_device ) && ( t != msg_mscp_no_conn ) )
		break;
	    krn$_sleep( 100 );
	    du_close( fp );
	    t = du_open( fp, 0, 0, 0 );
	    if( t != msg_success )
		break;
	    }
	return( 0 );
	}
    else
	return( du_write_sub( fp, ilen, inum, c ) );
    }

du_write_sub( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    int len;
    int lbn;
    int t;
    struct INODE *inode;
    struct ub *ub;
    struct cb *cb;
    struct rsp *rsp;

    len = ilen * inum;
    /*
     *  Acquire the SCS lock, and then make sure the device is still valid (if
     *  it isn't, just return 0).  The device may be invalid if the connection
     *  to the remote server has been broken (recently).
     */
    krn$_wait( &scs_lock );
    t = mscp_unit_okay( fp, 1 );
    if( t == msg_success )
	{
	/*
	 *  The file is open.  The MISC field in the INODE structure is used
	 *  to hold a UB pointer; use this to find the CB for this device.
	 */
	lbn = *(INT *)fp->offset / 512;
	inode = fp->ip;
	ub = inode->misc;
	rsp = fp->misc;
	cb = rsp->cb;
	/*
	 *  Don't let writes go past the end of the unit.
	 */
	if( *(INT *)fp->offset + len > *(INT *)inode->len )
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
	    t = mscp_write_cmd( cb, rsp, ub, c, &len, lbn );
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
     *  written.  Release the SCS lock, and return that number.
     */
    krn$_post( &scs_lock );
    return( t );
    }

/*+
 * ============================================================================
 * = du_open - open a file                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine opens a file.  A "file" is a raw device, i.e., an MSCP
 *	unit connected to an MSCP server.  "Files" are created and deleted as
 *	MSCP units and MSCP servers come and go.  Reference counts are kept
 *	in both the unit block (UB) and the connection block (CB) in order to
 *	prevent UB and CB deletion while files are still open.  This routine
 *	may return failure if the underlying unit or connection is no longer
 *	valid.
 *  
 * FORM OF CALL:
 *  
 *	du_open( fp )
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
du_open( struct FILE *fp, char *info, char *next, char *mode )
    {
    int t;
    struct ub *ub;
    struct cb *cb;
    struct rsp *rsp;
    struct INODE *inode;

    common_driver_open( fp, info, next, mode );
    /*
     *  Acquire the SCS lock, and then make sure the device is still valid (if
     *  it isn't, just quit).  A device is valid if the INODE is still in use.
     *
     *  Note that we must make the check for validity while holding the SCS
     *  lock, as the INODE can be marked invalid only by a code stream which
     *  itself holds the SCS lock.  This makes the check atomic.
     */
    krn$_wait( &scs_lock );
    t = mscp_unit_okay( fp, 0 );
    if( t == msg_success )
	{
	/*
	 *  The device is valid, initialize what we need.
	 */
	inode = fp->ip;
	ub = inode->misc;
	cb = ub->cb;
	cb->ref++;
	/*
	 *  Allocate an RSP for later use.
	 */
	rsp = malloc( sizeof( *rsp ) );
	rsp->incarn = ub->incarn;
	rsp->cb = ub->cb;
	insq( rsp, cb->rsp.blink );
	krn$_seminit( &rsp->sem, 0, "mscp_rsp" );
	/*
	 *  If the current reference count is 0, then bring the unit online.
	 *  We must protect the command which does this, however, by using the
	 *  reference count.  So diddle the reference count prior to that
	 *  command, and undiddle it if that command fails for any reason.
	 */
	ub->ref++;
	if( ub->ref == 1 )
	    t = mscp_online_cmd( cb, rsp, ub );
	else
	    t = msg_success;
	/*
	 *  Do the initialization below if and only if the unit is now online.
	 *  If the unit is not online, get rid of the RSP.
	 */
	if( t == msg_success )
	    {
	    fp->misc = rsp;
	    fp->rap = malloc( sizeof( struct RAB ) );
	    fp->rap->alloc = 512;
	    fp->rap->buf = malloc( 512 );
	    fp->rap->inx = -1;
	    fp->rap->outx = 0;
	    }
	else
	    {
	    if( ub->incarn == rsp->incarn )
		--ub->ref;
	    --cb->ref;
	    if( cb->ref == 0 )
		cb->discard_cb( cb );
	    krn$_semrelease( &rsp->sem );
	    remq( rsp );
	    free( rsp );
	    fp->misc = 0;
	    }
	}
    /*
     *  Release the lock, and return success or failure as appropriate.
     */
    krn$_post( &scs_lock );
    return( t );
    }

/*+
 * ============================================================================
 * = du_close - close a file                                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine closes a file.
 *  
 * FORM OF CALL:
 *  
 *	du_close( fp )
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
du_close( struct FILE *fp )
    {
    struct ub *ub;
    struct cb *cb;
    struct rsp *rsp;

    krn$_wait( &scs_lock );
    /*
     *  There is no need to check to see whether this device is valid or not,
     *  as no device can be destroyed (made invalid) while it contains open
     *  files.  That's what those UB and CB reference counts do for us!
     */
    ub = fp->ip->misc;
    rsp = fp->misc;
    if( rsp )
	{
	cb = rsp->cb;
	/*
	 *  If the current reference count is 1, then make the unit available.
	 */
	if( ub->incarn == rsp->incarn )
	    {
	    if( ub->ref == 1 )
		mscp_available_cmd( cb, rsp, ub );
	    --ub->ref;
	    }
	/*
	 *  Get rid of the RSP.
	 */
	krn$_semrelease( &rsp->sem );
	remq( rsp );
	free( rsp );
	fp->misc = 0;
	/*
	 *  Get rid of the CB (if needed).
	 */
	--cb->ref;
	if( cb->ref == 0 )
	    cb->discard_cb( cb );
	free( fp->rap->buf );
	free( fp->rap );
	}
    krn$_post( &scs_lock );
    return( msg_success );
    }

/*+
 * ============================================================================
 * = mu_read - read a file                                                    =
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
 *	mu_read( fp, ilen, inum, c )
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
mu_read( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    int len;
    int t;
    struct INODE *inode;
    struct ub *ub;
    struct cb *cb;
    struct rsp *rsp;

    len = ilen * inum;
    /*
     *  Acquire the SCS lock, and then make sure the device is still valid (if
     *  it isn't, just return 0).  The device may be invalid if the connection
     *  to the remote server has been broken (recently).
     */
    krn$_wait( &scs_lock );
    t = mscp_unit_okay( fp, 1 );
    if( t == msg_success )
	{
	/*
	 *  The file is open.  The MISC field in the INODE structure is used
	 *  to hold a UB pointer; use this to find the CB for this device.
	 */
	inode = fp->ip;
	ub = inode->misc;
	rsp = fp->misc;
	cb = rsp->cb;
	t = mscp_read_cmd( cb, rsp, ub, c, &len, 0 );
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
     *  read.  Release the SCS lock, and return that number.
     */
    krn$_post( &scs_lock );
    return( t );
    }

/*+
 * ============================================================================
 * = mu_write - write a file                                                  =
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
 *	mu_write( fp, ilen, inum, c )
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
mu_write( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    int len;
    int t;
    struct INODE *inode;
    struct ub *ub;
    struct cb *cb;
    struct rsp *rsp;

    len = ilen * inum;
    /*
     *  Acquire the SCS lock, and then make sure the device is still valid (if
     *  it isn't, just return 0).  The device may be invalid if the connection
     *  to the remote server has been broken (recently).
     */
    krn$_wait( &scs_lock );
    t = mscp_unit_okay( fp, 1 );
    if( t == msg_success )
	{
	/*
	 *  The file is open.  The MISC field in the INODE structure is used
	 *  to hold a UB pointer; use this to find the CB for this device.
	 */
	inode = fp->ip;
	ub = inode->misc;
	rsp = fp->misc;
	cb = rsp->cb;
	t = mscp_write_cmd( cb, rsp, ub, c, &len, 0 );
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
     *  written.  Release the SCS lock, and return that number.
     */
    krn$_post( &scs_lock );
    return( t );
    }

/*+
 * ============================================================================
 * = mu_open - open a file                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine opens a file.  A "file" is a raw device, i.e., an MSCP
 *	unit connected to an MSCP server.  "Files" are created and deleted as
 *	MSCP units and MSCP servers come and go.  Reference counts are kept
 *	in both the unit block (UB) and the connection block (CB) in order to
 *	prevent UB and CB deletion while files are still open.  This routine
 *	may return failure if the underlying unit or connection is no longer
 *	valid.
 *  
 * FORM OF CALL:
 *  
 *	mu_open( fp )
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
mu_open( struct FILE *fp, char *info, char *next, char *mode )
    {
    int t;
    struct ub *ub;
    struct cb *cb;
    struct rsp *rsp;
    struct INODE *inode;

    common_driver_open( fp, info, next, mode );
    /*
     *  Acquire the SCS lock, and then make sure the device is still valid (if
     *  it isn't, just quit).  A device is valid if the INODE is still in use.
     *
     *  Note that we must make the check for validity while holding the SCS
     *  lock, as the INODE can be marked invalid only by a code stream which
     *  itself holds the SCS lock.  This makes the check atomic.
     */
    krn$_wait( &scs_lock );
    t = mscp_unit_okay( fp, 0 );
    if( t == msg_success )
	{
	/*
	 *  The device is valid, initialize what we need.
	 */
	inode = fp->ip;
	ub = inode->misc;
	cb = ub->cb;
	cb->ref++;
	/*
	 *  Allocate an RSP for later use.
	 */
	rsp = malloc( sizeof( *rsp ) );
	rsp->incarn = ub->incarn;
	rsp->cb = ub->cb;
	insq( rsp, cb->rsp.blink );
	krn$_seminit( &rsp->sem, 0, "mscp_rsp" );
	/*
	 *  If the current reference count is 0, then bring the unit online.
	 *  We must protect the command which does this, however, by using the
	 *  reference count.  So diddle the reference count prior to that
	 *  command, and undiddle it if that command fails for any reason.
	 */
	ub->ref++;
	if( ub->ref == 1 )
	    t = mscp_online_cmd( cb, rsp, ub );
	else
	    t = msg_success;
	/*
	 *  Do the initialization below if and only if the unit is now online.
	 *  If the unit is not online, get rid of the RSP.
	 */
	if( t == msg_success )
	    {
	    fp->misc = rsp;
	    fp->rap = malloc( sizeof( struct RAB ) );
	    fp->rap->alloc = 2048;
	    fp->rap->buf = malloc( 2048 );
	    fp->rap->inx = -1;
	    fp->rap->outx = 0;
	    }
	else
	    {
	    if( ub->incarn == rsp->incarn )
		--ub->ref;
	    --cb->ref;
	    if( cb->ref == 0 )
		cb->discard_cb( cb );
	    krn$_semrelease( &rsp->sem );
	    remq( rsp );
	    free( rsp );
	    fp->misc = 0;
	    }
	}
    /*
     *  Release the lock, and return success or failure as appropriate.
     */
    krn$_post( &scs_lock );
    return( t );
    }

/*+
 * ============================================================================
 * = mu_close - close a file                                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine closes a file.
 *  
 * FORM OF CALL:
 *  
 *	mu_close( fp )
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
mu_close( struct FILE *fp )
    {
    struct ub *ub;
    struct cb *cb;
    struct rsp *rsp;

    krn$_wait( &scs_lock );
    /*
     *  There is no need to check to see whether this device is valid or not,
     *  as no device can be destroyed (made invalid) while it contains open
     *  files.  That's what those UB and CB reference counts do for us!
     */
    ub = fp->ip->misc;
    rsp = fp->misc;
    if( rsp )
	{
	cb = rsp->cb;
	/*
	 *  If the current reference count is 1, then make the unit available.
	 */
	if( ub->incarn == rsp->incarn )
	    {
	    if( ub->ref == 1 )
		mscp_available_cmd( cb, rsp, ub );
	    --ub->ref;
	    }
	/*
	 *  Get rid of the RSP.
	 */
	krn$_semrelease( &rsp->sem );
	remq( rsp );
	free( rsp );
	/*
	 *  Get rid of the CB (if needed).
	 */
	--cb->ref;
	if( cb->ref == 0 )
	    cb->discard_cb( cb );
	free( fp->rap->buf );
	free( fp->rap );
	}
    krn$_post( &scs_lock );
    return( msg_success );
    }

/*+
 * ============================================================================
 * = mscp_init - initialize the MSCP DDBs                                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine gives us a chance to initialize the MSCP DDBs (one DDB
 *	for DU and one for MU).  Private entry points are made public.
 *  
 * FORM OF CALL:
 *  
 *	mscp_init( )
 *  
 * RETURN CODES:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
 *	DU and MU DDBs are filled in.
 *
-*/
mscp_init( )
    {
    int mscp_poll( );

#if MODULAR
    du_ddb_ptr = &du_ddb;
    mu_ddb_ptr = &mu_ddb;
#endif
    mscp_cbs_ptr->flink = &mscp_cbs_ptr->flink;
    mscp_cbs_ptr->blink = &mscp_cbs_ptr->flink;
    mscp_mode = DDB$K_INTERRUPT;
    mscp_controllers = 0;
    krn$_create( mscp_poll, null_procedure, 0, 5, 0, 4096,
	    "mscp_poll", "nl", "r", "nl", "w", "nl", "w" );
    return( msg_success );
    }

mscp_delete( struct INODE *inode )
    {
    struct ub *ub;

    krn$_wait( &scs_lock );
    if( inode->inuse )
	{
	ub = inode->misc;
	if( ub->sb || ub->cb || ub->ref )
	    INODE_UNLOCK( inode );
	else
	    {
	    --ub->cb->ref;
	    if( ub->sb->first_ub == ub )
	        ub->sb->first_ub = 0;
	    remove_ub( ub );
	    remq( ub );
	    free( ub );
	    free( ub->inq );
	    freeinode( inode );
	    }
	}
    krn$_post( &scs_lock );
    return( msg_success );
    }

mscp_unit_okay( struct FILE *fp, int flag )
    {
    int i;
    int j;
    struct INODE *inode;
    struct ub *ub;
    struct rsp *rsp;

    inode = fp->ip;
    if( inode->inuse )
	{
	ub = inode->misc;
	if( flag )
	    {
	    rsp = fp->misc;
	    if( ub->sb && ub->cb && rsp
		    && ( ub->cb == rsp->cb ) && ( ub->incarn == rsp->incarn ) )
		if( ub->cb->state == cb_k_open )
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
		if( ub && ub->sb && ub->cb )
		    if( ub->cb->state == cb_k_open )
			return( msg_success );
		krn$_post( &scs_lock );
		krn$_sleep( 500 );
		ub = inode->misc;
		krn$_wait( &scs_lock );
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
 * = mscp_setmode - stop or start the MSCP driver                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine deals with requests to stop or start the MSCP driver.
 *  
 * FORM OF CALL:
 *  
 *	mscp_setmode( mode )
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
mscp_setmode( int mode )
    {
    if( mode != DDB$K_READY )
	mscp_mode = mode;
    }

/*+
 * ============================================================================
 * = mscp_poll - poll all known MSCP connections                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine basically sleeps, waking periodically to deal with all
 *	known MSCP connections.  If a particular MSCP connection is being
 *	broken, then that connection is removed from the list of known MSCP
 *	connections, and we deal with it no longer.  Else, we issue keep-alive
 *	messages as required, and handle timing of outstanding MSCP commands.
 *	Again, we use reference counts in the CB to make sure that no CB
 *	vanishes while it is in use.
 *  
 * FORM OF CALL:
 *  
 *	mscp_poll( )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
 *	Keep-alive messages may be issued.  Servers may be requested to supply
 *	status of outstanding commands.
 *
-*/
mscp_poll( )
    {
    struct mscp_cb *p;
    struct cb *cb;
    struct rsp *rsp;

    while( 1 )
	{
	/*
	 *  Take it easy.  Go to sleep.  Every now and then, wake up.  Use
	 *  fudge factors when calculating when to do the next thing; err
	 *  on the "too short" side for things that the other end is timing,
	 *  and err on the "too long" side for things that we are timing.
	 *  Hold the SCS lock for an entire pass through the list of known
	 *  MSCP connections.
	 */
	krn$_sleep( 1000 );
	if( mscp_mode == DDB$K_STOP )
	    continue;
	krn$_wait( &scs_lock );
	for( p = mscp_cbs_ptr->flink; p != &mscp_cbs_ptr->flink; p = p->mscp_cb.flink )
	    {
	    /*
	     *  We've got another connection to watch.
	     */
	    cb = p->cb;
	    /*
	     *  If the connection is going away, we'll stop watching.
	     */
	    if( cb->state != cb_k_open )
		{
		struct mscp_cb *q;

		/*
		 *  If the connection state indicates that we have received a
		 *  request to disconnect, then the server is telling us that
		 *  a host access timeout occurred.  Break the VC to recover
		 *  cleanly.
		 */
		if( cb->state != cb_k_closed )
		    cb->break_vc( cb );
		/*
		 *  The connection is broken.  Fix up the reference count so
		 *  that the CB can be deleted, and forget this connection.
		 */
		q = p;
		p = p->mscp_cb.blink;
		remq( q );
		free( q );
		--cb->ref;
		if( cb->ref == 0 )
		    cb->discard_cb( cb );
		continue;
		}
	    /*
	     *  If it's time, send a keep-alive message to make the remote
	     *  MSCP server happy.
	     */
	    if( --cb->host_tmo == 0 )
		{
		mscp_keepalive( cb );
		cb->host_tmo = mscp_k_host_tmo - 10;
		}
	    /*
	     *  If the CB's RSP queue is not empty, then see if we have a new
	     *  command to check up on.  The first RSP in the CB's RSP queue
	     *  is the oldest command, and if the active bit is set in that
	     *  RSP and the keys match, then the command is active and needs
	     *  to be timed.
	     */
	    if( cb->rsp.flink != &cb->rsp.flink )
		/*
		 *  The queue is not empty; see if the old "oldest" command
		 *  is still worth our interest (the RSP should be the old
		 *  RSP, it should still be active, and the keys should still
		 *  match).
		 */
		if( ( ( rsp = cb->rsp.flink ) == cb->old_rsp )
			&& rsp->active && ( rsp->key == cb->old_key ) )
		    {
		    /*
		     *  The old "oldest" command is still the oldest and is
		     *  still active.  Some commands are immediate, and must
		     *  complete within one server timeout interval; these
		     *  RSPs are distinguished by clearing the timed bit in
		     *  the RSP.  Other commands are sequential, and may take
		     *  forever to complete; these RSPs are distinguished by
		     *  settin the timed bit in the RSP, asking that their
		     *  progress be timed.  If an RSP needs to be timed, then
		     *  we send a message to make sure that it's making some
		     *  progress; if an RSP should NOT be timed, then for sure
		     *  we aren't making progress, so break the connection.
		     *  When we send the MSCP message asking for a progress
		     *  report, we mark that RSP as not needing timing, and
		     *  receipt of the response to that MSCP message turns
		     *  timing back on.  What this gives us is a way to time
		     *  the message sent asking about the other message in
		     *  question.
		     */ 
		    if( --cb->cont_tmo == 0 )
			{
			/*
			 *  If the message is being timed, then ask for a
			 *  progress report, and reset the timeout interval.
			 *  Else, kill the connection and VC.
			 */
			if( rsp->timed )
			    {
			    fprintf( el_fp, msg_mscp_cmd_tmo_check, cb->sb->name );
			    mscp_get_command_cmd( cb, rsp );
			    cb->cont_tmo = cb->timeout + 10;
			    }
			else
			    {
			    fprintf( el_fp, msg_mscp_cmd_tmo_fatal, cb->sb->name );
			    cb->break_vc( cb );
			    }
			}
		    }
		/*
		 *  We either have a new "oldest", or no good "oldest" at all.
		 *  (This is controlled by whether the new "oldest" RSP is now
		 *  active or not.)
		 */
		else if( rsp->active )
		    {
		    /*
		     *  This is a new "oldest".  Remember it, and restart the
		     *  timer.
		     */
		    cb->old_rsp = rsp;
		    cb->old_key = rsp->key;
		    cb->cont_tmo = cb->timeout + 10;
		    }
		else
		    /*
		     *  There's nothing worth remembering.
		     */
		    cb->old_rsp = 0;
	    else
		/*
		 *  The queue is empty, so there's no "oldest" any more.
		 */
		cb->old_rsp = 0;
	    }
	/*
	 *  This pass is complete.  Drop the SCS lock.
	 */
	krn$_post( &scs_lock );
	}
    }

/*+
 * ============================================================================
 * = mscp_new_node - bring up connections to a newly discovered system        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine is called when a new remote system is discovered which
 *	is deemed capable of supporting an MSCP disk or tape server.  (This
 *	is usually the result of bringing up a VC to that remote system.)
 *	We start up two tasks, one which attempts to connect to the new
 *	system's MSCP disk server, and another which attempts to connect to
 *	the new system's MSCP tape server.  Usually only one of these connect
 *	attempts will succeed; the other will fail, and when it does, that
 *	task will exit.
 *  
 * FORM OF CALL:
 *  
 *	mscp_new_node( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct mscp_sb *sb		- system block of the new system
 *
 * SIDE EFFECTS:
 *
 *	Two new tasks are created.
 *
-*/
mscp_new_node( struct mscp_sb *sb )
    {
    char name[20];
    int mscp_unit( );

    sprintf( name, "mscp_disk_%d", sb->node_id );
    krn$_create( mscp_unit, null_procedure, 0, 5, 0, 4096,
            name, "nl", "r", "nl", "w", "nl", "w", sb, &du_ddb );
    sprintf( name, "mscp_tape_%d", sb->node_id );
    krn$_create( mscp_unit, null_procedure, 0, 5, 0, 4096,
            name, "nl", "r", "nl", "w", "nl", "w", sb, &mu_ddb );

    }

/*+
 * ============================================================================
 * = mscp_unit - bring up a connection and find all units                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine first tries to establish a connection to the MSCP disk
 *	or tape server in a remote system.  If successful, we poll that
 *	remote system to find all attached units, and UBs get built for each
 *	unit found.  For disks, we bring each unit online, and then make it
 *	available again.  We use reference counts in the CB to make sure that
 *	no CB vanishes while it is in use.
 *  
 * FORM OF CALL:
 *  
 *	mscp_unit( sb, driver )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct mscp_sb *sb		- system block
 *	struct DDB *driver		- pointer to driver dispatch table
 *
 * SIDE EFFECTS:
 *
 *	A connection may be established, units may be discovered, files
 *	(devices) may be created.
 *
-*/
mscp_unit( struct mscp_sb *sb, struct DDB *driver )
    {
    int mscp_appl_msg_input( );
    int mscp_connection_error( );
    struct cb *cb;
    struct ub *ub;
    struct rsp *rsp;
    struct mscp_cb *p;

    /*
     *  Grab the SCS lock, and then connect to the MSCP disk or tape server,
     *  as appropriate.  The routine to call to perform the connection is
     *  found in the SB.
     */
    krn$_wait( &scs_lock );
    if( driver == &du_ddb )
	{
	cb = sb->connect( sb, &du_ddb,
		"MSCP$DISK       ",
		"MSCP$DISK_DRIVER",
		"                ",
		2, mscp_appl_msg_input, mscp_connection_error );
	}
    if( driver == &mu_ddb )
	{
	cb = sb->connect( sb, &mu_ddb,
		"MSCP$TAPE       ",
		"MSCP$TAPE_DRIVER",
		"                ",
		2, mscp_appl_msg_input, mscp_connection_error );
	}
    /*
     *  If a connection was not established for whatever reason, just let go
     *  of the SCS lock and exit.
     */
    if( !cb )
	{
	krn$_post( &scs_lock );
	return;
	}
    /*
     *  Do required further initialization.  Use the reference count to
     *  protect ourselves, and allocate an RSP.  Put this connection on the
     *  list of connections to be watched.  Next, send a MSCP Set Controller
     *  Characteristics command, and wait for its response.  Then poll for
     *  units, building a UB for each one found.
     */
    cb->ref++;
    rsp = malloc( sizeof( *rsp ) );
    insq( rsp, cb->rsp.blink );
    krn$_seminit( &rsp->sem, 0, "mscp_rsp" );
    p = malloc( sizeof( *p ) );
    p->cb = cb;
    cb->host_tmo = mscp_k_host_tmo - 10;
    cb->old_rsp = 0;
    insq( p, mscp_cbs_ptr->blink );
    cb->timeout = mscp_k_cont_tmo;
    cb->ref++;
    mscp_set_cont_cmd( cb, rsp );
    mscp_poll_units( cb, rsp );
    mscp_poll_units( cb, rsp );
    --cb->ref;
    if( cb->ref == 0 )
	cb->discard_cb( cb );
    krn$_semrelease( &rsp->sem );
    remq( rsp );
    free( rsp );
    /*
     *  Initialization and setup is done for this connection and associated
     *  units.
     */
    krn$_post( &scs_lock );
    }

/*+
 * ============================================================================
 * = mscp_appl_msg_input - called on receipt of an application message        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine is called whenever an application message is received
 *	over this connection.  This is usually a response to an earlier MSCP
 *	command (if not, it must be an attention message).  If any task is
 *	waiting for this MSCP response, then said response is delivered to
 *	said task.
 *  
 * FORM OF CALL:
 *  
 *	mscp_appl_msg_input( cb, p, len )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct mscp_generic_rsp	*p	- pointer to MSCP packet
 *	int len				- length of MSCP packet
 *
 * SIDE EFFECTS:
 *
 *	A sleeping task may be awoken.
 *
-*/
mscp_appl_msg_input( struct cb *cb, struct mscp_generic_rsp *p, int len )
    {
    struct rsp *rsp;

    switch( p->opcode )
	{
	/*
	 *  This is a list of the MSCP opcodes we understand.  We shouldn't
	 *  get anything not on this list.  If we do, something's broken.
	 */
	case mscp_k_get_command+mscp_k_end:
	    {
	    struct mscp_get_command_rsp *q = p;

	    /*
	     *  Is the command that we asked about still the oldest?  We need
	     *  to check since it may have completed between sending the Get
	     *  Command Status command and receiving this response.  If the
	     *  command in question is no longer the oldest, then ignore this
	     *  response.
	     */
	    if( ( ( rsp = q->out_ref ) == cb->rsp.flink )
		    && ( rsp == cb->old_rsp )
		    && rsp->active && ( rsp->key == cb->old_key ) )
		{
		/*
		 *  Make sure that the outstanding command is making progress.
		 *  If it is, then enable the timer again; otherwise, tear
		 *  down the connection.
		 */
		rsp->timed = 1;
		if( q->cmd_sts < rsp->cmd_sts )
		    rsp->cmd_sts = q->cmd_sts;
		else
		    {
		    fprintf( el_fp, msg_mscp_no_progress, cb->sb->name );
		    cb->break_vc( cb );
		    }
		}
	    break;
	    }

	case mscp_k_get_unit+mscp_k_end:
	case mscp_k_set_cont+mscp_k_end:
	case mscp_k_available+mscp_k_end:
	case mscp_k_online+mscp_k_end:
	case mscp_k_read+mscp_k_end:
	case mscp_k_write+mscp_k_end:
	    /*
	     *  If there is an allocated RSP, then copy the received MSCP
	     *  response to the RSP's buffer, and then post the associated
	     *  semaphore.  (The RSP's address, if there was one, was passed
	     *  in one of the fields of the original MSCP command, and the
	     *  server on the other end is required to echo it back to us in
	     *  the MSCP response.)
	     */
	    rsp = p->cmd_ref;
	    if( rsp )
		{
		rsp->size = len;
		memcpy( rsp->buffer, p, len );
		rsp->active = 0;
		remq( rsp );
		insq( rsp, cb->rsp.blink );
		krn$_post( &rsp->sem );
		}
	    break;

   	case mscp_k_avatn:
   	    mscp_new_unit( cb, p );
            break; 

   	case mscp_k_dupun:
   	case mscp_k_acpth:
   	    break;

	/*
	 *  Very bad news if we get here.  Die a hard death (we're in effect
	 *  committing suicide).
	 */
	default:
	    fprintf( el_fp, msg_mscp_bad_cmd_rsp, p->opcode, cb->sb->name );
	    cb->break_vc( cb );
	}
    }

/*+
 * ============================================================================
 * = mscp_connection_error - clean up a failed connection                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine gets called when an MSCP connection is being torn down.
 *	All outstanding commands for which a response is required will have an
 *	RSP on a CB queue; take care of them.  All units have UBs on a CB
 *	queue; take care of them.
 *  
 * FORM OF CALL:
 *  
 *	mscp_connection_error( cb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *
 * SIDE EFFECTS:
 *
 *	Tasks waiting for pending commands to complete will find that those
 *	commands have magically done so, and thus those tasks will be
 *	signalled.  UBs will be deleted; files (devices) will be deleted.
 *
-*/
mscp_connection_error( struct cb *cb )
    {
    struct mscp_generic_rsp *q;
    struct rsp *rsp;
    struct ub *ub;
    struct INODE *inode;
    
    /*
     *  Rip through the list of RSPs pending, and pretend that we have received
     *  the expected MSCP responses.  This will wake up waiting tasks.
     */
    for( rsp = cb->rsp.flink; rsp != &cb->rsp.flink; rsp = rsp->rsp.flink )
	{
	rsp->size = sizeof( *q );
	q = rsp->buffer;
	q->status.major = mscp_k_aborted;
	q->status.minor = 0;
	krn$_post( &rsp->sem );
	}
    /*
     *  Rip through the list of units.  For each unit, say we're losing it,
     *  and unlink it from the CB.
     */
    while( ( ub = cb->ub.flink ) != &cb->ub.flink )
	{
	--cb->ref;
	inode = ub->inode;
	fprintf( el_fp, msg_dev_lost, inode->name, ub->info, cb->sb->name );
	if( ub->sb->first_ub == ub )
	    ub->sb->first_ub = 0;
	ub->cb = 0;
	ub->sb = 0;
	remove_ub( ub );
	remq( ub );
	}
    if( cb->ref == 0 )
	cb->discard_cb( cb );
    }

/*+
 * ============================================================================
 * = mscp_keepalive - keep a connection alive by issuing a dummy command      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine keeps a connection alive by issuing an MSCP Get Unit
 *	Status command to unit 0.  We don't bother to wait for the MSCP
 *	response to come back.
 *  
 * FORM OF CALL:
 *  
 *	mscp_keepalive( cb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *
 * SIDE EFFECTS:
 *
-*/
mscp_keepalive( struct cb *cb )
    {
    struct mscp_get_unit_cmd *p;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	return;
    /*
     *  Get an application message packet, zero it, and fill in required
     *  fields.  Then send it.
     */
    p = cb->alloc_appl_msg( cb, sizeof( *p ) );
    p->cmd_ref = 0;
    p->unit = 0;
    p->opcode = mscp_k_get_unit;
    cb->send_appl_msg( cb, p );
    }

/*+
 * ============================================================================
 * = mscp_get_command_cmd - send an MSCP Get Command Status command           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an MSCP Get Command Status command.
 *  
 * FORM OF CALL:
 *  
 *	mscp_get_command_cmd( cb, rsp )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- the response we are asking about
 *
 * SIDE EFFECTS:
 *
-*/
mscp_get_command_cmd( struct cb *cb, struct rsp *rsp )
    {
    struct mscp_get_command_cmd *p;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	return;
    /*
     *  Get an application message packet, zero it, and fill in required
     *  fields.  Then send it.
     */
    p = cb->alloc_appl_msg( cb, sizeof( *p ) );
    p->cmd_ref = 0;
    p->unit = rsp->unit;
    p->opcode = mscp_k_get_command;
    p->out_ref = rsp;
    rsp->timed = 0;
    cb->send_appl_msg( cb, p );
    }

/*+
 * ============================================================================
 * = mscp_set_cont_cmd - send an MSCP Set Controller Characteristics command  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an MSCP Set Controller Characteristics
 *	command.
 *  
 * FORM OF CALL:
 *  
 *	mscp_set_cont_cmd( cb, rsp )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- a place for the response
 *
 * SIDE EFFECTS:
 *
 *	The remote server timeout value is set.
 *
-*/
mscp_set_cont_cmd( struct cb *cb, struct rsp *rsp )
    {
    struct mscp_set_cont_cmd *p;
    struct mscp_set_cont_rsp *q;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	return;
    /*
     *  Get an application message packet, zero it, and fill in required
     *  fields.  Then send it.
     */
    p = cb->alloc_appl_msg( cb, sizeof( *p ) );
    p->cmd_ref = rsp;
    p->opcode = mscp_k_set_cont;
    p->host_tmo = mscp_k_host_tmo;
    p->cont_flag = mscp_k_cont_flag;
    rsp->active = 1;
    rsp->timed = 0;
    rsp->key++;
    cb->send_appl_msg( cb, p );
    /*
     *  Now wait for the response.  When it comes, remember the server's
     *  timeout value.
     */
    krn$_post( &scs_lock );
    krn$_wait( &rsp->sem );
    krn$_wait( &scs_lock );
    q = rsp->buffer;
    cb->alloclass = q->alloc;
    cb->timeout = q->cont_tmo;
    }

/*+
 * ============================================================================
 * = mscp_available_cmd - send an MSCP Available command                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an MSCP Available command.
 *  
 * FORM OF CALL:
 *  
 *	mscp_available_cmd( cb, rsp, ub )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_mscp_no_conn		- the MSCP connection is not open
 *	msg_mscp_bad_available		- the Available command failed
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- a place for the response
 *	struct ub *ub			- unit block
 *
 * SIDE EFFECTS:
 *
 *	The unit is made available.
 *
-*/
mscp_available_cmd( struct cb *cb, struct rsp *rsp, struct ub *ub )
    {
    struct mscp_available_cmd *p;
    struct mscp_available_rsp *q;
    struct INODE *inode;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	{
	err_printf( msg_mscp_no_conn, cb->sb->name );
	return( msg_mscp_no_conn );
	}
    /*
     *  Get an application message packet, zero it, and fill in required
     *  fields.  Then send it.
     */
    p = cb->alloc_appl_msg( cb, sizeof( *p ) );
    p->cmd_ref = rsp;
    p->unit = ub->unit;
    p->opcode = mscp_k_available;
    if( cb->driver == &mu_ddb )
	p->modifier = mscp_clear_exception;
    rsp->active = 1;
    rsp->timed = 1;
    rsp->key++;
    rsp->unit = ub->unit;
    rsp->cmd_sts = 0xffffffff;
    cb->send_appl_msg( cb, p );
    /*
     *  Now wait for the response.
     */
    krn$_post( &scs_lock );
    krn$_wait( &rsp->sem );
    krn$_wait( &scs_lock );
    q = rsp->buffer;
    if( q->status.major != mscp_k_success )
	{
	err_printf( msg_mscp_bad_status, q->status.major, q->status.minor );
	err_printf( msg_mscp_bad_available, ub->inode->name );
	return( msg_mscp_bad_available );
	}
    inode = ub->inode;
    *(INT *)inode->len = (INT)0;
    *(INT *)inode->alloc = (INT)0;
    return( msg_success );
    }

/*+
 * ============================================================================
 * = mscp_online_cmd - send an MSCP Online command                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an MSCP Online command.
 *  
 * FORM OF CALL:
 *  
 *	mscp_online_cmd( cb, rsp, ub )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_mscp_no_conn		- the MSCP connection is not open
 *	msg_mscp_bad_online		- the Online command failed
 *
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- a place for the response
 *	struct ub *ub			- unit block
 *
 * SIDE EFFECTS:
 *
 *	The unit is brought online.
 *
-*/
mscp_online_cmd( struct cb *cb, struct rsp *rsp, struct ub *ub )
    {
    struct mscp_online_cmd *p;
    struct mscp_online_rsp *q;
    struct INODE *inode;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	{
	err_printf( msg_mscp_no_conn, cb->sb->name );
	return( msg_mscp_no_conn );
	}
    /*
     *  Get an application message packet, zero it, and fill in required
     *  fields.  Then send it.
     */
    p = cb->alloc_appl_msg( cb, sizeof( *p ) );
    p->cmd_ref = rsp;
    p->unit = ub->unit;
    p->opcode = mscp_k_online;
    if( cb->driver == &mu_ddb )
	p->modifier = mscp_clear_exception;
    rsp->active = 1;
    rsp->timed = 1;
    rsp->key++;
    rsp->unit = ub->unit;
    rsp->cmd_sts = 0xffffffff;
    cb->send_appl_msg( cb, p );
    /*
     *  Now wait for the response.  When it comes, remember the server's
     *  unit size.
     */
    krn$_post( &scs_lock );
    krn$_wait( &rsp->sem );
    krn$_wait( &scs_lock );
    q = rsp->buffer;
    if( q->status.major != mscp_k_success )
	{
	if( ( q->status.major == mscp_k_unit_offline )
		&& ( q->status.minor == 1 ) )
	    {
	    err_printf( msg_no_media, ub->inode->name );
	    return( msg_no_media );
	    }
	else
	    {
	    err_printf( msg_mscp_bad_status, q->status.major, q->status.minor );
	    err_printf( msg_mscp_bad_online, ub->inode->name );
	    return( msg_mscp_bad_online );
	    }
	}
    if( cb->driver == &du_ddb )
	{
	inode = ub->inode;
	*(INT *)inode->len = (INT)q->unit_size * 512;
	*(INT *)inode->alloc = (INT)q->unit_size * 512;
	}
    return( msg_success );
    }

/*+
 * ============================================================================
 * = mscp_read_cmd - send an MSCP Read command                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an MSCP Read command.
 *  
 * FORM OF CALL:
 *  
 *	mscp_read_cmd( cb, rsp, ub, buffer, size, lbn )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_rec_underflow		- the record read was too short
 *	msg_rec_overflow		- the record read was too long
 *	msg_eof				- end of file was found
 *	msg_eot				- end of tape was found
 *	msg_no_media			- there is no media in the unit
 *	msg_mscp_no_conn		- the MSCP connection is not open
 *	msg_mscp_bad_read		- the Read command failed
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- a place for the response
 *	struct ub *ub			- unit block
 *	unsigned char *buffer		- buffer for read data
 *	int *size			- pointer to size to read
 *	int lbn				- logical block number to read
 *
 * SIDE EFFECTS:
 *
-*/
mscp_read_cmd( struct cb *cb, struct rsp *rsp, struct ub *ub,
	unsigned char *buffer, int *size, int lbn )
    {
    unsigned long int map_info[4];
    struct mscp_read_cmd *p;
    struct mscp_read_rsp *q;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	return( msg_mscp_no_conn );
    /*
     *  Get an application message packet, zero it, and fill in required
     *  fields.  Then send it.  Notice the call to MAP; this call into the
     *  port driver takes care of mapping the user's buffer in some form that
     *  the port hardware can understand.  Map information is kept in the RSP
     *  for convenience sake.
     */
    p = cb->alloc_appl_msg( cb, sizeof( *p ) );
    p->cmd_ref = rsp;
    p->unit = ub->unit;
    p->opcode = mscp_k_read;
    if( cb->driver == &mu_ddb )
	p->modifier = mscp_clear_exception;
    p->size = *size;
    cb->map( cb, buffer, *size, p->buffer, map_info );
    p->lbn = lbn;
    rsp->active = 1;
    rsp->timed = 1;
    rsp->key++;
    rsp->unit = ub->unit;
    rsp->cmd_sts = 0xffffffff;
    cb->send_appl_msg( cb, p );
    /*
     *  Now wait for the response.  When it comes, we can unmap the user's
     *  buffer.  Then we return success or failure based on the status in the
     *  MSCP response packet.
     */
    krn$_post( &scs_lock );
    krn$_wait( &rsp->sem );
    krn$_wait( &scs_lock );
    cb->unmap( cb, map_info );
    q = rsp->buffer;
    if( q->status.major == mscp_k_aborted )
	return( msg_mscp_no_conn );
    if( q->status.major == mscp_k_unit_available )
	return( msg_mscp_no_conn );
    if( q->status.major == mscp_k_eof )
	return( msg_eof );
    if( ( q->status.major == mscp_k_eot )
	    || ( ( q->status.major == mscp_k_data )
		    && ( q->status.minor == 0 ) )
	    || ( q->flags & 0x08 ) )
	return( msg_eot );
    if( q->status.major == mscp_k_data_truncated )
	return( msg_rec_overflow );
    if( ( q->status.major == mscp_k_unit_offline )
	    && ( q->status.minor == 1 ) )
	{
	err_printf( msg_no_media, ub->inode->name );
	return( msg_no_media );
	}
    if( q->status.major != mscp_k_success )
	{
	err_printf( msg_mscp_bad_status, q->status.major, q->status.minor );
	err_printf( msg_mscp_bad_read, ub->inode->name );
	return( msg_mscp_bad_read );
	}
    if( q->size != *size )
	{
	*size = q->size;
	return( msg_rec_underflow );
	}
    return( msg_success );
    }

/*+
 * ============================================================================
 * = mscp_write_cmd - send an MSCP Write command                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an MSCP Write command.
 *  
 * FORM OF CALL:
 *  
 *	mscp_write_cmd( cb, rsp, ub, buffer, size, lbn )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_rec_underflow		- the record written was too short
 *	msg_eot				- end of tape was found
 *	msg_no_media			- there is no media in the unit
 *	msg_write_prot			- the media is write protected
 *	msg_mscp_no_conn		- the MSCP connection is not open
 *	msg_mscp_bad_write		- the Write command failed
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- a place for the response
 *	struct ub *ub			- unit block
 *	unsigned char *buffer		- buffer for write data
 *	int *size			- pointer to size to write
 *	int lbn				- logical block number to write
 *
 * SIDE EFFECTS:
 *
-*/
mscp_write_cmd( struct cb *cb, struct rsp *rsp, struct ub *ub,
	unsigned char *buffer, int *size, int lbn )
    {
    unsigned long int map_info[4];
    struct mscp_write_cmd *p;
    struct mscp_write_rsp *q;

    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	return( msg_mscp_no_conn );
    /*
     *  Get an application message packet, zero it, and fill in required
     *  fields.  Then send it.  Notice the call to MAP; this call into the
     *  port driver takes care of mapping the user's buffer in some form that
     *  the port hardware can understand.  Map information is kept in the RSP
     *  for convenience sake.
     */
    p = cb->alloc_appl_msg( cb, sizeof( *p ) );
    p->cmd_ref = rsp;
    p->unit = ub->unit;
    p->opcode = mscp_k_write;
    if( cb->driver == &mu_ddb )
	p->modifier = mscp_clear_exception;
    p->size = *size;
    cb->map( cb, buffer, *size, p->buffer, map_info );
    p->lbn = lbn;
    rsp->active = 1;
    rsp->timed = 1;
    rsp->key++;
    rsp->unit = ub->unit;
    rsp->cmd_sts = 0xffffffff;
    cb->send_appl_msg( cb, p );
    /*
     *  Now wait for the response.  When it comes, we can unmap the user's
     *  buffer.  Then we return success or failure based on the status in the
     *  MSCP response packet.
     */
    krn$_post( &scs_lock );
    krn$_wait( &rsp->sem );
    krn$_wait( &scs_lock );
    cb->unmap( cb, map_info );
    q = rsp->buffer;
    if( q->status.major == mscp_k_aborted )
	return( msg_mscp_no_conn );
    if( q->status.major == mscp_k_unit_available )
	return( msg_mscp_no_conn );
    if( ( q->status.major == mscp_k_eot )
	    || ( ( q->status.major == mscp_k_data )
		    && ( q->status.minor == 0 ) )
	    || ( q->flags & 0x08 ) )
	return( msg_eot );
    if( ( q->status.major == mscp_k_unit_offline )
	    && ( q->status.minor == 1 ) )
	{
	err_printf( msg_no_media, ub->inode->name );
	return( msg_no_media );
	}
    if( q->status.major == mscp_k_write_prot )
	{
	err_printf( msg_write_prot, ub->inode->name );
	return( msg_write_prot );
	}
    if( q->status.major != mscp_k_success )
	{
	err_printf( msg_mscp_bad_status, q->status.major, q->status.minor );
	err_printf( msg_mscp_bad_write, ub->inode->name );
	return( msg_mscp_bad_write );
	}
    if( q->size != *size )
	{
	*size = q->size;
	return( msg_rec_underflow );
	}
    return( msg_success );
    }

/*+
 * ============================================================================
 * = mscp_poll_units - poll a remote server to find attached units            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine polls a remote MSCP server in order to find all attached
 *	MSCP units.  It does this by sending MSCP Get Unit Status commands,
 *	using the Next Unit modifier, until an MSCP response is received which
 *	indicates that no more units exist.
 *  
 * FORM OF CALL:
 *  
 *	mscp_poll_units( cb, rsp )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct rsp *rsp			- a place for the response
 *
 * SIDE EFFECTS:
 *
 *	Files will be created.
 *
-*/
mscp_poll_units( struct cb *cb, struct rsp *rsp )
    {
    int t;
    struct mscp_get_unit_cmd *p;
    struct mscp_get_unit_rsp *q;

    t = 0;
    while( 1 )
	{
	/*
	 *  If the connection is broken, do nothing.
	 */
	if( cb->state != cb_k_open )
	    break;
	/*
	 *  Get an application message packet, zero it, and fill in required
	 *  fields.  Then send it.  Note that each MSCP command will use
	 *  the last known unit number plus one.
	 */
	p = cb->alloc_appl_msg( cb, sizeof( *p ) );
	p->cmd_ref = rsp;
	p->unit = t;
	p->opcode = mscp_k_get_unit;
	p->modifier = mscp_next_unit;
	rsp->active = 1;
	rsp->timed = 0;
	rsp->key++;
	cb->send_appl_msg( cb, p );
	/*
	 *  Now wait for the response.  When it comes, we potentially have
	 *  a new unit to deal with.  If so, build its data structures.  We
	 *  have a new unit if the status in the MSCP response is either
	 *  Available, or Offline with a subcode of 0.
	 */
	krn$_post( &scs_lock );
	krn$_wait( &rsp->sem );
	krn$_wait( &scs_lock );
	q = rsp->buffer;
	/*
	 *  Stop polling if the unit number returned is 0, yet we didn't ask
	 *  about 0.
	 */
	if( t && !q->unit )
	    break;
	/*
	 *  If this is a good unit, remember it!
	 */
	if( ( q->status.major == mscp_k_unit_available )
		|| ( ( q->status.major == mscp_k_unit_offline )
			&& q->status.minor != 0 ) )
	    mscp_new_unit( cb, q );
	t = q->unit + 1;
	}
    }

/*+
 * ============================================================================
 * = mscp_new_unit - make a unit known to the system                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds the data structures necessary to make an MSCP
 *	unit known to the system.
 *  
 * FORM OF CALL:
 *  
 *	mscp_new_unit( cb, q )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct cb *cb			- connection block
 *	struct mscp_get_unit_rsp *q	- pointer to MSCP response packet
 *
 * SIDE EFFECTS:
 *
 *	A unit block is allocated, and an INODE is initialized.
 *
-*/
mscp_new_unit( struct cb *cb, struct mscp_get_unit_rsp *q )
    {
    char name[32];
    char c;
    char *np;
    int t;
    struct pb *pb;
    struct mscp_sb *sb;
    struct ub *ub;
    struct INODE *inode;

    sb = cb->sb;
    pb = sb->pb;
    /*
     *  If the connection is broken, do nothing.
     */
    if( cb->state != cb_k_open )
	return;
    /*
     *  Create a new "file".
     */
    set_io_name( name,
	    ((struct DDB *)(cb->driver))->name,
	    q->unit,
	    sb->node_id,
	    pb );
    t = allocinode( name, 1, &inode );
    inode->dva = cb->driver;
    inode->attr = ATTR$M_READ;
    if( cb->driver == &mu_ddb )
	inode->bs = 2048;
    else
	inode->bs = 512;
    /*
     *  If the file already exists, then a unit block is already allocated;
     *  otherwise, we must allocate one.
     */
    if( t == 1 )
	{
	ub = inode->misc;
	if( ub->cb )
	    {
	    INODE_UNLOCK( inode );
	    return;
	    }
        }  
    else
	{
	ub = malloc( sizeof( *ub ) );
	ub->inq = malloc( MAX_INQ_SZ );
	ub->inq_len = umin( sizeof( *q ), MAX_INQ_SZ );
	memcpy( ub->inq, q, ub->inq_len );
	ub->unit = q->unit;
	ub->flags = q->unit_flag;
	}
    ub->cb = cb;
    ub->sb = sb;
    ub->pb = pb;
    ub->incarn = krn$_unique_number( );
    ub->ref = 0;
    np = ub->info;
    c = q->media_id.a0;
    if( c )
	*np++ = c + '@';
    c = q->media_id.a1;
    if( c )
	*np++ = c + '@';
    c = q->media_id.a2;
    if( c )
	*np++ = c + '@';
    sprintf( np, "%d", q->media_id.n );
    ub->dd[0] = q->media_id.d0 + '@';
    ub->dd[1] = q->media_id.d1 + '@';
    if( cb->alloclass )
	sprintf( ub->alias, "$%d$%sA%d (%s)",
		cb->alloclass,
		ub->dd,
		q->unit,
		sb->alias );
    else if( strlen( sb->alias ) )
	sprintf( ub->alias, "%s$%sA%d",
		sb->alias,
		ub->dd,
		q->unit );
    else
	sprintf( ub->alias, "%s%c%d",
		ub->dd,
		pb->controller + 'A',
		q->unit );
    /*
     *  Set up required pointers and back-pointers.  The INODE points to the
     *  UB, the UB points to the CB, the CB has a queue of UBs.
     */
    cb->ref++;
    inode->misc = ub;
    ub->inode = inode;
    if( !sb->first_ub )
	sb->first_ub = ub;
    insq( ub, cb->ub.blink );
    INODE_UNLOCK( inode );
    sprintf( ub->string, "%-24s   %-26s   %6s", name, ub->alias, ub->info );
    insert_ub( ub );
    fprintf( el_fp, msg_dev_found, inode->name, ub->info, sb->name );
    }
