/* file:	pci_driver.c
 *
 * Copyright (C) 1997 by
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
 * Abstract:	PCI memory, I/O, and configuration space driver. 
 *
 * Author:	Eric Rasmussen
 *
 * Modifications:
 *
 *	er	24-Sep-1997	Initial entry based on code extracted 
 *				from tsunami_io.c.
 *
 */

#include    "cp$src:kernel_def.h"
#include    "cp$src:pb_def.h"
#include    "cp$src:probe_io_def.h"
#include    "cp$src:common.h"
#include    "cp$src:msg_def.h"
#include    "cp$inc:prototypes.h"
#include    "cp$inc:platform_io.h"

#define byte char
#define word short
#define quad __int64

/* External Declarations */

extern struct HQE mchq_660;
extern int pci_handler( );

extern int null_procedure( );

extern unsigned byte inportb( struct pb *pb, unsigned quad offset );
extern unsigned word inportw( struct pb *pb, unsigned quad offset );
extern unsigned long inportl( struct pb *pb, unsigned quad offset );

extern outportb( struct pb *pb, unsigned quad offset, unsigned byte data );
extern outportw( struct pb *pb, unsigned quad offset, unsigned word data );
extern outportl( struct pb *pb, unsigned quad offset, unsigned long data );

extern unsigned byte inmemb( struct pb *pb, unsigned quad offset );
extern unsigned word inmemw( struct pb *pb, unsigned quad offset );
extern unsigned long inmeml( struct pb *pb, unsigned quad offset );
extern unsigned quad inmemq( struct pb *pb, unsigned quad offset );

extern outmemb( struct pb *pb, unsigned quad offset, unsigned byte data );
extern outmemw( struct pb *pb, unsigned quad offset, unsigned word data );
extern outmeml( struct pb *pb, unsigned quad offset, unsigned long data );
extern outmemq( struct pb *pb, unsigned quad offset, unsigned quad data );

#if DENSE_SUPPORT
extern unsigned long indmeml( struct pb *pb, unsigned quad offset );
extern unsigned quad indmemq( struct pb *pb, unsigned quad offset );

extern outdmeml( struct pb *pb, unsigned quad offset, unsigned long data );
extern outdmemq( struct pb *pb, unsigned quad offset, unsigned quad data );
#endif

extern unsigned byte incfgb( struct pb *pb, unsigned quad offset );
extern unsigned word incfgw( struct pb *pb, unsigned quad offset );
extern unsigned long incfgl( struct pb *pb, unsigned quad offset );

extern outcfgb( struct pb *pb, unsigned quad offset, unsigned byte data );
extern outcfgw( struct pb *pb, unsigned quad offset, unsigned word data );
extern outcfgl( struct pb *pb, unsigned quad offset, unsigned long data );

/* Forward Declarations: */

int pci_init( );

int pci_open( );
int pci_close( );

int pciio_read( );
int pciio_write( );

int pcimem_read( );
int pcimem_write( );

int pcicfg_read( );
int pcicfg_write( );

struct DDB pciio_ddb = {
	"pciio",		/* how this routine wants to be called	*/
	pciio_read,		/* read routine				*/
	pciio_write,		/* write routine			*/
	pci_open,		/* open routine				*/
	pci_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};

struct DDB pcimem_ddb = {
	"pcimem",		/* how this routine wants to be called	*/
	pcimem_read,		/* read routine				*/
	pcimem_write,		/* write routine			*/
	pci_open,		/* open routine				*/
	pci_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};

#if DENSE_SUPPORT
int pcidmem_read( );
int pcidmem_write( );

struct DDB pcidmem_ddb = {
	"pcidmem",		/* how this routine wants to be called	*/
	pcidmem_read,		/* read routine				*/
	pcidmem_write,		/* write routine			*/
	pci_open,		/* open routine				*/
	pci_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};
#endif

struct DDB pcicfg_ddb = {
	"pcicfg",		/* how this routine wants to be called	*/
	pcicfg_read,		/* read routine				*/
	pcicfg_write,		/* write routine			*/
	pci_open,		/* open routine				*/
	pci_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};


/*+
 * ============================================================================
 * = pci_init - Initialization routine for the PCI drivers.                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will initialize the PCI driver data bases.
 *
 * FORM OF CALL:
 *
 *	pci_init( ); 
 *      
 * RETURNS:
 *   
 *	msg_success
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:               
 *      
 *      None
 *   
-*/                            

int pci_init( )
{
    struct INODE *ip;

    allocinode( pciio_ddb.name, 1, &ip );
    ip->dva = &pciio_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;
    INODE_UNLOCK( ip );

    allocinode( pcimem_ddb.name, 1, &ip );
    ip->dva = &pcimem_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;
    INODE_UNLOCK( ip );

#if DENSE_SUPPORT
    allocinode( pcidmem_ddb.name, 1, &ip );
    ip->dva = &pcidmem_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;
    INODE_UNLOCK( ip );
#endif

    allocinode( pcicfg_ddb.name, 1, &ip );
    ip->dva = &pcicfg_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;
    INODE_UNLOCK( ip );

    return( msg_success );
}


/*+
 * ============================================================================
 * = pci_open - Open a PCI driver.                                            =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      This routine opens a PCI driver.
 *  
 * FORM OF CALL:
 *  
 *	pci_open( fp, info, next, mode )
 *  
 * RETURNS:
 *
 *	msg_success - Success
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - File to be opened.
 *	     char *info - Offset into the file.		     
 *	     char *next - Ignored
 *	     char *mode - Ignored.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int pci_open( struct FILE *fp, char *info, char *next, char *mode )
{
    struct HQE *hqe;

    fp->offset = &fp->local_offset;
    *(UINT *)fp->offset = 0;
    if( info && *info )
	common_convert( info, 16, fp->offset, 8 );
    hqe = malloc( sizeof( *hqe ) );
    hqe->handler = pci_handler;
    hqe->param = fp;
    insq_lock( hqe, mchq_660.blink );
    fp->misc = hqe;
    return( msg_success );
}


/*+
 * ============================================================================
 * = pci_close - Close a PCI driver.                                          =
 * ============================================================================
 *
 * OVERVIEW:       
 *  
 *      This routine closes a PCI driver.
 *
 * FORM OF CALL:
 *  
 *	pci_close( fp )
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - File to be closed.
 *	
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int pci_close (struct FILE *fp)
{
    struct HQE *hqe;

    hqe = fp->misc;
    remq_lock( hqe );
    free( hqe );
    return( msg_success );
}


/*+
 * ============================================================================
 * = pciio_read - Read PCI I/O space.                                         =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      This routine is used to read PCI I/O space. It's primary function
 *	is to hide all the translations needed to read PCI I/O space.
 *
 *      The offset, passed in the info field, is interpreted as follows:
 *     
 *       3             3 3
 *       9             2 1                                        0   
 *      +---------------+------------------------------------------+
 *      |   PCI Hose    |                PCI Offset                |
 *      +---------------+------------------------------------------+
 *  
 * FORM OF CALL:
 *  
 *	pciio_read( fp, size, number, c );
 *  
 * RETURNS:
 *
 *	Number of bytes read
 *       
 * ARGUMENTS:
 *
 *	 struct FILE *fp - Pointer to a file.
 *	        int size - Size of item in bytes.
 *	      int number - Number of items to read.
 *	unsigned char *c - Buffer to put the data in.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int pciio_read( struct FILE *fp, int size, int number, unsigned char *c )
{
    int i;
    unsigned quad offset;
    struct pb pb;

    pb.hose = fp->offset[1];
    offset = fp->offset[0];
    if( ( size != 2 ) && ( size != 4 ) )
	{
    	number *= size;
    	size = 1;
	}

    for( i = 0; i < number; i++ )
	{
	switch( size )
	    {
	    case 1:
		*(unsigned byte *)c = inportb( &pb, offset );
		break;

	    case 2:
		*(unsigned word *)c = inportw( &pb, offset );
		break;

	    case 4:
		*(unsigned long *)c = inportl( &pb, offset );
		break;
	    }

	if( fp->status != msg_success )
	    {
	    err_printf( fp->status );
	    return( 0 );
	    }

	c += size;
	offset += size;
	}

    fp->offset[0] = offset;

    return( size * number );
}


/*+
 * ============================================================================
 * = pciio_write - Write PCI I/O space.                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine writes PCI I/O space. Its primary function is to hide
 *	the translations needed to write PCI I/O space.
 *
 *      The offset, passed in the info field, is interpreted as follows:
 *     
 *       3             3 3
 *       9             2 1                                        0   
 *      +---------------+------------------------------------------+
 *      |   PCI Hose    |                PCI Offset                |
 *      +---------------+------------------------------------------+
 *  
 * FORM OF CALL:
 *
 *	pciio_write( fp, size, number, c ); 
 *                    
 * RETURNS:
 *
 *	Number of bytes written
 *
 * ARGUMENTS:
 *
 *       struct FILE *fp - Pointer to the file to be written.
 *	        int size - size of item in bytes.
 *	      int number - number of items to write.
 *	unsigned char *c - Buffer to be written. 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int pciio_write( struct FILE *fp, int size, int number, unsigned char *c )
{
    int i;
    unsigned quad offset;
    struct pb pb;

    pb.hose = fp->offset[1];
    offset = fp->offset[0];
    if( ( size != 2 ) && ( size != 4 ) )
	{
    	number *= size;
    	size = 1;
	}

    for( i = 0; i < number; i++ )
	{
	switch( size )
	    {
	    case 1:
		outportb( &pb, offset, *(unsigned byte *)c );
		break;

	    case 2:
		outportw( &pb, offset, *(unsigned word *)c );
		break;

	    case 4:
		outportl( &pb, offset, *(unsigned long *)c );
		break;
	    }

	if( fp->status != msg_success )
	    {
	    err_printf( fp->status );
	    return( 0 );
	    }

	c += size;
	offset += size;
	}

    fp->offset[0] = offset;

    return( size * number );
}


/*+
 * ============================================================================
 * = pcimem_read - Read PCI memory space.                                     =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      This routine is used to read PCI memory space. It's primary function
 *	is to hide all the translations needed to access PCI memory space.
 *  
 *      The offset, passed in the info field, is interpreted as follows:
 *     
 *       3             3 3
 *       9             2 1                                        0   
 *      +---------------+------------------------------------------+
 *      |   PCI Hose    |                PCI Offset                |
 *      +---------------+------------------------------------------+
 *
 * FORM OF CALL:
 *  
 *	pcimem_read( fp, size, number, c );
 *  
 * RETURNS:
 *
 *	Number of bytes read
 *       
 * ARGUMENTS:
 *
 *	 struct FILE *fp - Pointer to a file.
 *	        int size - size of item in bytes.
 *	      int number - number of items to read.
 *	unsigned char *c - Buffer to put the data in.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int pcimem_read( struct FILE *fp, int size, int number, unsigned char *c )
{
    int i;
    unsigned quad offset;
    struct pb pb;

    pb.hose = fp->offset[1];
    offset = fp->offset[0];
    if( ( size != 2 ) && ( size != 4 ) && ( size != 8 ) )
	{
    	number *= size;
    	size = 1;
	}

    for( i = 0; i < number; i++ )
	{
	switch( size )
	    {
	    case 1:
		*(unsigned byte *)c = inmemb( &pb, offset );
		break;

	    case 2:
		*(unsigned word *)c = inmemw( &pb, offset );
		break;

	    case 4:
		*(unsigned long *)c = inmeml( &pb, offset );
		break;

	    case 8:
		*(unsigned long *)c = inmemq( &pb, offset );
		break;
	    }

	if( fp->status != msg_success )
	    {
	    err_printf( fp->status );
	    return( 0 );
	    }

	c += size;
	offset += size;
	}

    fp->offset[0] = offset;

    return( size * number );
}


/*+
 * ============================================================================
 * = pcimem_write - Write PCI memory space.                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine writes PCI memory space. Its primary function is to hide
 *	the translations needed to write PCI memory space.
 *
 *      The offset, passed in the info field, is interpreted as follows:
 *     
 *       3             3 3
 *       9             2 1                                        0   
 *      +---------------+------------------------------------------+
 *      |   PCI Hose    |                PCI Offset                |
 *      +---------------+------------------------------------------+
 *
 * FORM OF CALL:
 *
 *	pcimem_write( fp, size, number, c ); 
 *                    
 * RETURNS:
 *
 *	Number of bytes written.
 *
 * ARGUMENTS:
 *
 *       struct FILE *fp - Pointer to the file to be written.
 *	        int size - size of item in bytes.
 *	      int number - number of items to write.
 *	unsigned char *c - Buffer to be written. 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int pcimem_write( struct FILE *fp, int size, int number, unsigned char *c )
{
    int i;
    unsigned quad offset;
    struct pb pb;

    pb.hose = fp->offset[1];
    offset = fp->offset[0];
    if( ( size != 2 ) && ( size != 4 ) && ( size != 8 ) )
	{
    	number *= size;
    	size = 1;
	}

    for( i = 0; i < number; i++ )
	{
	switch( size )
	    {
	    case 1:
		outmemb( &pb, offset, *(unsigned byte *)c );
		break;

	    case 2:
		outmemw( &pb, offset, *(unsigned word *)c );
		break;

	    case 4:
		outmeml( &pb, offset, *(unsigned long *)c );
		break;

	    case 8:
		outmemq( &pb, offset, *(unsigned long *)c );
		break;
	    }

	if( fp->status != msg_success )
	    {
	    err_printf( fp->status );
	    return( 0 );
	    }

	c += size;
	offset += size;
	}

    fp->offset[0] = offset;

    return( size * number );
}

#if DENSE_SUPPORT

/*+
 * ============================================================================
 * = pcidmem_read - Read PCI dense memory space.                              =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      This routine is used to read PCI dense memory space. It's primary 
 *	function is to hide all the translations needed to access PCI dense
 *	memory space.
 *  
 *      The offset, passed in the info field, is interpreted as follows:
 *     
 *       3             3 3
 *       9             2 1                                        0   
 *      +---------------+------------------------------------------+
 *      |   PCI Hose    |                PCI Offset                |
 *      +---------------+------------------------------------------+
 *
 * FORM OF CALL:
 *  
 *	pcidmem_read( fp, size, number, c );
 *  
 * RETURNS:
 *
 *	Number of bytes read
 *       
 * ARGUMENTS:
 *
 *	 struct FILE *fp - Pointer to a file.
 *	        int size - size of item in bytes.
 *	      int number - number of items to read.
 *	unsigned char *c - Buffer to put the data in.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int pcidmem_read( struct FILE *fp, int size, int number, unsigned char *c )
{
    int i;
    unsigned quad offset;
    struct pb pb;

    pb.hose = fp->offset[1];
    offset = fp->offset[0];
    if( ( size != 4 ) && ( size != 8 ) )
	{
	err_printf( msg_exdep_badincsize );
	fp->status = msg_exdep_badincsize;
	return( 0 );
	}
    if( offset & ( size - 1 ) )
	{
	err_printf( msg_exdep_badadr );
	fp->status = msg_exdep_badadr;
	return( 0 );
	}

    for( i = 0; i < number; i++ )
	{
	switch( size )
	    {
	    case 4:
		*(unsigned long *)c = indmeml( &pb, offset );
		break;

	    case 8:
		*(unsigned quad *)c = indmeml( &pb, offset );
		break;
	    }

	if( fp->status != msg_success )
	    {
	    err_printf( fp->status );
	    return( 0 );
	    }

	c += size;
	offset += size;
	}

    fp->offset[0] = offset;

    return( size * number );
}


/*+
 * ============================================================================
 * = pcidmem_write - Write PCI dense memory space.                            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine writes PCI dense memory space. Its primary function is 
 *	to hide the translations needed to write PCI memory space.
 *
 *      The offset, passed in the info field, is interpreted as follows:
 *     
 *       3             3 3
 *       9             2 1                                        0   
 *      +---------------+------------------------------------------+
 *      |   PCI Hose    |                PCI Offset                |
 *      +---------------+------------------------------------------+
 *
 * FORM OF CALL:
 *
 *	pcidmem_write( fp, size, number, c ); 
 *                    
 * RETURNS:
 *
 *	Number of bytes written.
 *
 * ARGUMENTS:
 *
 *       struct FILE *fp - Pointer to the file to be written.
 *	        int size - size of item in bytes.
 *	      int number - number of items to write.
 *	unsigned char *c - Buffer to be written. 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int pcidmem_write( struct FILE *fp, int size, int number, unsigned char *c )
{
    int i;
    unsigned quad offset;
    struct pb pb;

    pb.hose = fp->offset[1];
    offset = fp->offset[0];
    if( ( size != 4 ) && ( size != 8 ) )
	{
	err_printf( msg_exdep_badincsize );
	fp->status = msg_exdep_badincsize;
	return( 0 );
	}
    if( offset & ( size - 1 ) )
	{
	err_printf( msg_exdep_badadr );
	fp->status = msg_exdep_badadr;
	return( 0 );
	}

    for( i = 0; i < number; i++ )
	{
	switch( size )
	    {
	    case 4:
		outdmeml( &pb, offset, *(unsigned long *)c );
		break;

	    case 8:
		outdmeml( &pb, offset, *(unsigned quad *)c );
		break;
	    }

	if( fp->status != msg_success )
	    {
	    err_printf( fp->status );
	    return( 0 );
	    }

	c += size;
	offset += size;
	}

    fp->offset[0] = offset;

    return( size * number );
}
#endif


/*+
 * ============================================================================
 * = pcicfg_read - Read PCI configuration space.                              =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *	This routine is used to read PCI configuration space. 
 *
 *      The offset, passed in the info field, is interpreted as follows:
 *
 *      3           3 3           2 2           1 1                     
 *      9           2 1           4 3           6 5           8 7           0
 *     +-------------+-------------+-------------+-------------+-------------+
 *     |  PCI Hose   |   PCI Bus   |  PCI Slot   |  PCI Func # | PCI offset  |
 *     +-------------+-------------+-------------+-------------+-------------+
 *
 *	PCI Offset - specifies an offset in the range 0 - 256.
 *	PCI Func # - specifies a function in a multi-function device in the 
 *                   range 0-7.
 *	PCI Slot   - specifies one of 32 devices on a given bus.
 *	PCI Bus    - specifies one of 256 possible buses in a system.
 *	PCI Hose   - specifies one of 256 possible hoses in a system.
 *  
 * FORM OF CALL:
 *  
 *	pcicfg_read( fp, size, number, c );
 *  
 * RETURNS:
 *
 *	Number of bytes read
 *       
 * ARGUMENTS:
 *
 *	 struct FILE *fp - Pointer to a file.
 *	        int size - size of item in bytes.
 *	      int number - number of items to read.
 *	unsigned char *c - Buffer to put the data in.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/                        

int pcicfg_read( struct FILE *fp, int size, int number, unsigned char *c )
{
    int i;
    unsigned quad offset;
    struct pb pb;

    pb.hose = fp->offset[1];
    pb.bus = ( fp->offset[0] >> 24 ) & 0xff;
    pb.slot = ( fp->offset[0] >> 16 ) & 0xff;
    pb.function = ( fp->offset[0] >> 8 ) & 0xff;
    offset = fp->offset[0] & 0xff;

    if( ( size != 2 ) && ( size != 4 ) )
	{
    	number *= size;
    	size = 1;
	}

    for( i = 0; i < number; i++ )
	{
	switch( size )
	    {
	    case 1:
		*(unsigned byte *)c = incfgb( &pb, offset );
		break;

	    case 2:
		*(unsigned word *)c = incfgw( &pb, offset );
		break;

	    case 4:
		*(unsigned long *)c = incfgl( &pb, offset );
		break;
	    }

	if( fp->status != msg_success )
	    {
	    err_printf( fp->status );
	    return( 0 );
	    }

	c += size;
	offset += size;
	offset &= 0xff;
	}

    fp->offset[0] &= ~0xff;
    fp->offset[0] += offset;

    return( size * number );
}


/*+
 * ============================================================================
 * = pcicfg_write - Write PCI configuration space.                            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine writes PCI configuration space. 
 *
 *      The offset, passed in the info field, is interpreted as follows:
 *
 *      3           3 3           2 2           1 1                     
 *      9           2 1           4 3           6 5           8 7           0
 *     +-------------+-------------+-------------+-------------+-------------+
 *     |  PCI Hose   |   PCI Bus   |  PCI Slot   |  PCI Func # | PCI offset  |
 *     +-------------+-------------+-------------+-------------+-------------+
 *
 *	PCI Offset - specifies an offset in the range 0 - 256.
 *	PCI Func # - specifies a function in a multi-function device in the 
 *                   range 0-7.
 *	PCI Slot   - specifies one of 32 devices on a given bus.
 *	PCI Bus    - specifies one of 256 possible buses in a system.
 *	PCI Hose   - specifies one of 256 possible hoses in a system.
 *  
 * FORM OF CALL:
 *
 *	pcicfg_write( fp, size, number, c ); 
 *                    
 * RETURNS:
 *
 *	Number of bytes written.
 *
 * ARGUMENTS:
 *
 *	 struct FILE *fp - Pointer to the file to be written.
 *	        int item - size of item in bytes.
 *	      int number - number of items to write.
 *	unsigned char *c - Buffer to be written. 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int pcicfg_write( struct FILE *fp, int size, int number, unsigned char *c )
{
    int i;
    unsigned quad offset;
    struct pb pb;

    pb.hose = fp->offset[1];
    pb.bus = ( fp->offset[0] >> 24 ) & 0xff;
    pb.slot = ( fp->offset[0] >> 16 ) & 0xff;
    pb.function = ( fp->offset[0] >> 8 ) & 0xff;
    offset = fp->offset[0] & 0xff;

    if( ( size != 2 ) && ( size != 4 ) )
	{
    	number *= size;
    	size = 1;
	}

    for( i = 0; i < number; i++ )
	{
	switch( size )
	    {
	    case 1:
		outcfgb( &pb, offset, *(unsigned byte *)c );
		break;

	    case 2:
		outcfgw( &pb, offset, *(unsigned word *)c );
		break;

	    case 4:
		outcfgl( &pb, offset, *(unsigned long *)c );
		break;
	    }

	if( fp->status != msg_success )
	    {
	    err_printf( fp->status );
	    return( 0 );
	    }

	c += size;
	offset += size;
	offset &= 0xff;
	}

    fp->offset[0] &= ~0xff;
    fp->offset[0] += offset;

    return( size * number );
}
