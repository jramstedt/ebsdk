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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	TEE protocol driver
 *
 *              TEE copies a single input stream into two output streams
 *
 *              >>> cat foo > tee:t1/t2
 *
 *
 * Author:	Philippe Klein
 *
 * Modifications:
 *
 *	phk	08-Oct-1991	Initial entry.
 */
#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:msg_def.h"

#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)


null_procedure ();
int tee_write ();
int tee_open ();
int tee_close ();

struct DDB tee_ddb = {
	"tee",			/* how this routine wants to be called	*/
	null_procedure,		/* read routine				*/
	tee_write,		/* write routine			*/
	tee_open,		/* open routine				*/
	tee_close,		/* close routine			*/
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
	1,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};


struct TEE_DB {
    struct FILE *fp1;
    struct FILE *fp2;
    } ;

int tee_read( );
int tee_write( );
int tee_open( );
int tee_close( );

/*---------------------------------------------------------------------*/
int tee_init( )  {

    struct INODE *ip;

 
    allocinode( tee_ddb.name, 1, &ip );
    ip->dva = & tee_ddb;
    ip->attr = ATTR$M_WRITE;

    INODE_UNLOCK (ip);
    return msg_success;
}

/*---------------------------------------------------------------------*/
int tee_write( struct FILE *fp, int size, int number, unsigned char *c ) {

    struct TEE_DB *tdb;
    tdb = (struct TEE_DB *)fp->misc;

    if (tdb->fp1 != NULL) fwrite(c,size,number,tdb->fp1);
    if (tdb->fp2 != NULL) fwrite(c,size,number,tdb->fp2);

    return size*number;
}

/*---------------------------------------------------------------------*/
int tee_open( struct FILE *fp, char *info, char *next, char *mode ) {

    struct TEE_DB *tdb;

    fp->misc = (struct TEE_DB *)malloc(sizeof(struct TEE_DB));
    tdb = (struct TEE_DB *)fp->misc;

    if (strlen(info)) {
	if ( (tdb->fp1 = fopen(info,"w+")) == NULL ) 
	    err_printf(msg_file_not_found, info );
   }
      
    if (strlen(next)) {
	if ( (tdb->fp2 = fopen(next,"w+")) == NULL ) 
	    err_printf(msg_file_not_found, next );
    }

    return msg_success;
}

/*---------------------------------------------------------------------*/
int tee_close( struct FILE *fp ) {

    struct TEE_DB *tdb;
    tdb = (struct TEE_DB *)fp->misc;

    if (tdb->fp1 != NULL) fclose(tdb->fp1);
    if (tdb->fp2 != NULL) fclose(tdb->fp2);

    free(fp->misc);

    return msg_success;
}
    
