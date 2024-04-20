/* file:	ndbr_driver.c
 *
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
 *  ABSTRACT:	File system for Alpha.
 *
 *      At this level of abstraction, all devices and files appear as
 *	files.  Access is via routines that have the same interface as
 *	FOPEN, FCLOSE etc from the ANSII C standard.
 *
 *  AUTHORS:
 *
 *      Jon Denisco
 *
 *  CREATION DATE:
 * 
 *      19-Apr-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	pel	12-Mar-1991	let fopen/close incr/decr inode ref & collapse
 *				wrt_ref, rd_ref into just ref.
 *
 *	ajb	27-Feb-1991	Broken out from filesys.c
 *
 *--
 */
#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:parse_def.h"
#include 	"cp$src:mop_def.h"			/*MOP definitions*/
#include 	"cp$src:mb_def.h"			/*MOP block defs*/
#include	"cp$inc:kernel_entry.h"


int ndbr_read( );
int ndbr_write( );
int ndbr_open( );
int ndbr_close( );
extern int null_procedure ();

struct DDB ndbr_ddb = {
	"ndbr",			/* how this routine wants to be called	*/
	ndbr_read,		/* read routine				*/
	ndbr_write,		/* write routine			*/
	ndbr_open,		/* open routine				*/
	ndbr_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	0,			/* allows information			*/
	1,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};



struct ndbr_db
    {
    struct FILE *fp;
    char buf[256];
    int n;
    };

int ndbr_init( )
    {
    struct INODE *inode;

    allocinode( ndbr_ddb.name, 1, &inode );
    inode->dva = & ndbr_ddb;
    inode->attr = ATTR$M_WRITE;
    INODE_UNLOCK (inode);
    return msg_success;
    }

int ndbr_read( struct FILE *fp, int size, int number, unsigned char *c )
    {
    struct NODE_ID nid;			/*Node ID*/
    struct ndbr_db *db;

    db = fp->misc;
    if(fread(&nid,sizeof(struct NODE_ID),1,db->fp))
    {
	sprintf(c,"Node: %02x-%02x-%02x-%02x-%02x-%02x\n",
		nid.sa[0],nid.sa[1],nid.sa[2],nid.sa[3],nid.sa[4],nid.sa[5]);
	return(strlen(c));
    }
    else
	return(0);

    }

int ndbr_write( struct FILE *fp,int size,int len, unsigned char *c )
    {
    struct NODE_ID nid;
    struct ndbr_db *db;
    int i;
    char sa[32];

    db = fp->misc;
    memcpy(&(db->buf[db->n]),c,(size*len));
    db->n += size*len;    
    db->buf[db->n] = 0;

    for(i=0; i<db->n; i++)
	if(db->buf[i] == '\n')
	{
	    db->n = 0;
	    strelement (db->buf,1,"\t ",sa);
	    sa[17] = 0;
	    if(convert_enet_address(nid.sa,sa) == msg_success)
		fwrite(&nid,sizeof(struct NODE_ID),1,db->fp);
	    break;
	}
    return(1);
    }

int ndbr_open( struct FILE *fp, char *info, char *next, char *mode )
    {
    struct ndbr_db *db;

    db = malloc( sizeof( *db ));
    db->fp = fopen( next, mode );
    if( !db->fp )
	{
	free( db );
	return( msg_bad_open );
	}
    fp->misc = db;
    fp->rap = 0;
    init_file_offset( fp );
    db->n = 0;
    return( msg_success );
    }

int ndbr_close( struct FILE *fp )
    {
    struct ndbr_db *db;

    db = fp->misc;
    fclose( db->fp );
    free( db );
    return( msg_success );
    }
                              
