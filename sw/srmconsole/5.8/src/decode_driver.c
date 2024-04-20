/* file:	decode_driver.c
 *
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Instruction Decode protocol driver
 *
 * Author:	Console Firmware Team
 *
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"

#if MODULAR
#define lib$ins_decode	(*(adr[1]))
#endif

struct decode_db {
    struct FILE *fp;
    } ;

int decode_read ();
int decode_write ();
int decode_open ();
int decode_close ();
int null_procedure ();

struct DDB decode_ddb = {
	"decode",			/* how this routine wants to be called	*/
	decode_read,		/* read routine				*/
	decode_write,		/* write routine			*/
	decode_open,		/* open routine				*/
	decode_close,		/* close routine			*/
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


int decode_init( )
    {
    struct INODE *inode;

    allocinode( "decode", 1, &inode );
    inode->dva = & decode_ddb;
    inode->attr = ATTR$M_READ;
    INODE_UNLOCK (inode);
    return msg_success;
    }

int decode_read( struct FILE *fp, int size, int number, unsigned char *c )
    {
    unsigned long int inbuf[1];
    int len;
    int n;
    int pc;
    int offset;
    int (**adr)();
    unsigned long int outdsc[2];
    unsigned short int outlen;
    struct decode_db *db;

    len = size * number;
    if( len < 80 )
	return( 0 );
    db = fp->misc;
    fseek( db->fp, *(INT *)fp->offset, 0 );
    if( fread( inbuf, sizeof( inbuf ), 1, db->fp ) != sizeof( inbuf ) )
	return( 0 );
    sprintf( c, "%s: %16X %08X", db->fp->ip->name, *(INT *)fp->offset, inbuf[0] );
    n = strlen( c );
    c[n] = '\t';
    outdsc[0] = len - ( n + 1 );
    outdsc[1] = &c[n+1];
    pc = (int)inbuf;
    offset = *(INT *)fp->offset - pc;

#if MODULAR
/*
 * Load xdelta overlay
 * if not present in Flash, just return
 */
    adr = ovly_load("XDELTA");		/* Load and setup overlay */
    if (!adr)
	goto nodecode;
#endif

    if( lib$ins_decode( &pc, outdsc, &outlen, 0 ) == 1 )
	{
	*(INT *)fp->offset += pc - (int)inbuf;
	n += outlen + 1;
	}
    else
	{
nodecode:
	*(INT *)fp->offset += sizeof( long int );
	}
    c[n] = '\n';
    c[n+1] = '\0';						/* make asciz */
    return( n + 1 );
    }

int decode_write( struct FILE *fp, int size, int number, unsigned char *c )
    {
    return( 0 );
    }

int decode_open( struct FILE *fp, char *info, char *next, char *mode )
    {
    struct decode_db *db;

    db = dyn$_malloc( sizeof( *db ), DYN$K_SYNC );
    db->fp = fopen( next, mode );
    if( !db->fp )
	{
	dyn$_free( db, DYN$K_SYNC );
	return( msg_bad_open );
	}
    if( strlen( info ) )
	common_convert( info, 16, fp->offset, sizeof( fp->local_offset ) );
    *(INT *)fp->offset += *(INT *)db->fp->offset;
    fp->misc = db;
    fp->rap = 0;
    init_file_offset( fp );
    return( msg_success );
    }

int decode_close( struct FILE *fp )
    {
    struct decode_db *db;

    db = fp->misc;
    fclose( db->fp );
    dyn$_free( db, DYN$K_SYNC );
    return( msg_success );
    }
