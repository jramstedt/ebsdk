/* file:	i2o_bsa.c
 *
 * Copyright (C) 1994 by
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
 *      Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	I2O BSA driver
 *
 *  AUTHOR:
 *                         
 *	Console firmware team
 *
 *  CREATION DATE:
 *  
 *      06-Dec-1994
 *
 *  MODIFICATION HISTORY:
 *
 *	kl	06-Dec-1994
 *
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$inc:kernel_entry.h"

#if ( RAWHIDE & MODULAR ) 
#define DRIVERSHUT 1
#endif

extern common_driver_open( );
extern null_procedure( );

int i2o_bsa_read( );
int i2o_bsa_write( );
int i2o_bsa_open( );
int i2o_bsa_close( );

struct DDB i2o_bsa_ddb = {
	"dz",			/* how this routine wants to be called	*/
	i2o_bsa_read,		/* read routine				*/
	i2o_bsa_write,		/* write routine			*/
	i2o_bsa_open,		/* open routine				*/
	i2o_bsa_close,		/* close routine			*/
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

#if DRIVERSHUT
extern struct DDB *i2o_bsa_ddb_ptr;
#else
struct DDB *i2o_bsa_ddb_ptr = &i2o_bsa_ddb;
#endif

i2o_bsa_open( struct FILE *fp, char *info, char *next, char *mode )
    {
    int t = msg_success;
#if DRIVERSHUT
    t = start_driver(fp);
    if (t != msg_success) {
	stop_driver(fp);
	return (t);	
    }
#endif
    common_driver_open( fp, info, next, mode );
    return (t);	
    }

i2o_bsa_close( struct FILE *fp )
    {
#if DRIVERSHUT
    stop_driver(fp);
    return( msg_success );
#endif
    }

i2o_bsa_read( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    return( i2o_bsa_transfer( fp, ilen * inum, c, TRUE) );
    }

i2o_bsa_write( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    return( i2o_bsa_transfer( fp, ilen * inum, c, FALSE ) );
    }

i2o_bsa_init( )
    {
    i2o_bsa_ddb_ptr = &i2o_bsa_ddb;
    }
