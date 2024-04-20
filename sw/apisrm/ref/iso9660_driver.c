#if LOCAL_DEBUG
#define err_printf printf
#define qprintf printf
#endif
#define zprintf if(0)qprintf
#define yprintf if(0)qprintf
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
 * Abstract:	ISO 9660 protocol driver
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	30-Apr-1993	Initial entry.
 */

#if LOCAL_DEBUG
#if LOCAL_DEBUG_RAW
#include "sys$library:iodef.h"		/* exclude */
#endif
#else
#include "cp$src:platform.h"
#endif
#include "cp$src:common.h"
#include "cp$src:alpha_arc.h"
#include "cp$src:prototypes.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:regexp_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:f11_cd_def.h"

static char *iso9660_months[] = {
	"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
	"JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

struct od {
    struct FILE *fp;
    unsigned int size;
    struct PVD *pvd;
    } ;

struct of {
    struct od *od;
    unsigned long int raw;
    unsigned char *block;
    unsigned long int curr_lbn;
    unsigned long int last_lbn;
    unsigned long int ffbyte;
    unsigned long int rtype;
    unsigned long int rattr;
    unsigned long int len;
    unsigned long int bib;
    unsigned long int bir;
    unsigned long int offset;
    } ;

int iso9660_read( );
int iso9660_write( );
int iso9660_open( );
int iso9660_close( );
int iso9660_expand( );
int iso9660_validate( );
int null_procedure( );

struct DDB iso9660_ddb = {
    "iso9660",			/* how this device wants to be called	*/
    iso9660_read,		/* read routine				*/
    iso9660_write,		/* write routine			*/
    iso9660_open,		/* open routine				*/
    iso9660_close,		/* close routine			*/
    iso9660_expand,		/* expand routine			*/
    null_procedure,		/* delete routine			*/
    null_procedure,		/* address of create routine		*/
    null_procedure,		/* sets interrupt/polled mode		*/
    iso9660_validate,		/* validation routine			*/
    0,				/* class specific use			*/
    1,				/* allows information			*/
    1,				/* must be stacked			*/
    0,				/* is a flash update driver		*/
    1,				/* is a block device			*/
    0,				/* not seekable				*/
    0,				/* is an ethernet device		*/
    1,				/* is a filesystem driver		*/
};

#if LOCAL_DEBUG
#else
int iso9660_init( )
    {
    struct INODE *inode;

    allocinode( "iso9660", 1, &inode );
    inode->dva = &iso9660_ddb;
    inode->attr = ATTR$M_READ;
    INODE_UNLOCK( inode );

    allocinode( "iso9660_raw", 1, &inode );
    inode->dva = &iso9660_ddb;
    inode->attr = ATTR$M_READ;
    INODE_UNLOCK( inode );

    return( msg_success );
    }
#endif

int iso9660_read( struct FILE *fp, int size, int number, unsigned char *c )
    {
    int i;
    int len;
    int t;
    int lbn;
    struct of *of;

    len = size * number;
    of = fp->misc;
    if( *fp->offset != of->offset )
	{
	if( !of->raw
		&& ( ( of->rtype == XAR_RFM$K_LSB_VARIABLE )
			|| ( of->rtype == XAR_RFM$K_MSB_VARIABLE ) ) )
	    return( 0 );
	lbn = *fp->offset / of->od->size;
	if( of->curr_lbn != lbn )
	    {
	    of->curr_lbn = lbn;
	    of->bib = of->od->size;
	    }
	t = iso9660_find_byte( of );
	if( t != msg_success )
	    return( 0 );
	of->bib = *fp->offset & 511;
	}
    for( i = 0; i < len; i++ )
	{
	t = iso9660_read_file( of, &c[i] );
	if( t != msg_success )
	    break;
	}
    *fp->offset += i;
    of->offset = *fp->offset;
    return( i );
    }

int iso9660_write( struct FILE *fp, int size, int number, unsigned char *c )
    {
    return( 0 );
    }

int iso9660_open( struct FILE *fp, char *info, char *next, char *mode )
    {
    int t;
    struct of *od;
    struct of *of;

    if( strchr( info, ',' ) || ( strlen( next ) == 0 ) )
	return( msg_bad_filename );
    od = malloc_noown( sizeof( *od ) );
    t = iso9660_mount( od, next, mode );
    if( t != msg_success )
	goto EXIT1;
    of = malloc_noown( sizeof( *of ) );
    of->od = od;
    t = iso9660_open_file( of, info );
    if( t != msg_success )
	goto EXIT2;
    fp->misc = of;
    of->raw = ( fp->ip->name[7] == '_' );
    init_file_offset( fp );
    return( msg_success );

EXIT2:
    if( !( fp->mode & MODE_M_SILENT ) )
	err_printf( "file %s not found\n", info );
    free( of );
    iso9660_dismount( od );
EXIT1:
    free( od );
    return( t );
    }

int iso9660_close( struct FILE *fp )
    {
    struct of *od;
    struct of *of;

    of = fp->misc;
    od = of->od;
    iso9660_close_file( of );
    free( of );
    iso9660_dismount( od );
    free( od );
    return( msg_success );
    }

struct cb {
    int longformat;
    int (*callback)();
    int p0;
    int p1;
    int t;
    char *name;
    char *info;
    char *next;
    } ;

int iso9660_expand( char *name, char *info, char *next, int longformat,
	int (*callback)( ), int p0, int p1 )
    {
    int t;
    struct cb *cb;
    struct od *od;
    struct of *of;

    if( killpending( ) )
	return( msg_ctrlc );
    if( strchr( info, ',' ) )
	return( msg_bad_filename );
    cb = malloc_noown( sizeof( *cb ) );
    cb->longformat = longformat;
    cb->callback = callback;
    cb->p0 = p0;
    cb->p1 = p1;
    cb->t = msg_success;
    cb->name = name;
    cb->info = info;
    cb->next = next;
    od = malloc_noown( sizeof( *od ) );
    t = iso9660_mount( od, next, "r" );
    if( t != msg_success )
	goto EXIT1;
    of = malloc_noown( sizeof( *of ) );
    of->od = od;
    t = iso9660_get_header( of, od->pvd->PVD$B_ROOT_DIRECTORY );
    if( t != msg_success )
	goto EXIT2;
    of->block = malloc_noown( LOGICAL_BLOCK_SIZE );
    t = iso9660_scan_directory( of, "[]", cb->info, cb );
    free( of->block );
EXIT2:
    free( of );
    iso9660_dismount( od );
EXIT1:
    free( od );
    t = cb->t;
    free( cb );
    return( t );
    }

int iso9660_mount( struct od *od, char *vname, char *mode )
    {
#if LOCAL_DEBUG && LOCAL_DEBUG_RAW
    short chan[1];
    int desc[2];
#endif
    int t;
    struct VD *vd;

#if LOCAL_DEBUG
#if LOCAL_DEBUG_RAW
    desc[1] = vname;
    desc[0] = strlen( vname );
    t = sys$assign( desc, chan, 0, 0 );
    if( t & 1 )
	od->fp = (int)*chan;
    else
	od->fp = 0;
#else
    od->fp = fopen( vname, mode, "ctx=bin" );
#endif
#else
    od->fp = fopen( vname, mode );
#endif
    if( !od->fp )
	return( msg_failure );
    od->size = LOGICAL_BLOCK_SIZE;
    vd = malloc_noown( LOGICAL_BLOCK_SIZE );
    od->pvd = vd;
#if LOCAL_DEBUG
#if LOCAL_DEBUG_RAW
#else
    fseek( od->fp, LOGICAL_BLOCK_SIZE, 0 );
    fread( vd, 1, LOGICAL_BLOCK_SIZE, od->fp );
#endif
#endif
    t = iso9660_get_lbn( DATA_AREA, vd, od );
    if( t != msg_success )
	goto EXIT1;
    yprintf( "volume descriptor type = %x\n", vd->VD$B_VOLUME_DESCRIPTOR_TYPE );
    if( vd->VD$B_VOLUME_DESCRIPTOR_TYPE != VD$K_PVD )
	goto EXIT2;
    yprintf( "volume descriptor version = %x\n", vd->VD$B_VOLUME_DESCRIPTOR_VERS );
    if( vd->VD$B_VOLUME_DESCRIPTOR_VERS != ISO_9660_VERSION_1988 )
	goto EXIT2;
    od->size = od->pvd->PVD$W_LOGICAL_BLOCK_SIZE;
    yprintf( "primary volume logical block size = %d\n", od->size );
    if( od->size > LOGICAL_BLOCK_SIZE )
	goto EXIT2;
    return( msg_success );

EXIT2:
    t = msg_failure;
EXIT1:
    iso9660_dismount( od );
    return( t );
    }

void iso9660_dismount( struct od *od )
    {
    free( od->pvd );
#if LOCAL_DEBUG && LOCAL_DEBUG_RAW
#else
    fclose( od->fp );
#endif
    }

int iso9660_open_file( struct of *of, char *fname )
    {
    int t;

    zprintf( "open file `%s'\n", fname );
    t = iso9660_get_header( of, of->od->pvd->PVD$B_ROOT_DIRECTORY );
    if( t != msg_success )
	goto EXIT1;
    of->block = malloc_noown( LOGICAL_BLOCK_SIZE );
    t = iso9660_lookup( of, fname );
    if( t != msg_success )
	goto EXIT2;
    return( msg_success );

EXIT2:
    free( of->block );
EXIT1:
    return( t );
    }

void iso9660_close_file( struct of *of )
    {
    free( of->block );
    }

int iso9660_lookup( struct of *of, char *fname )
    {
    char ldelim;
    char rdelim;
    int t;

    zprintf( "lookup `%s'\n", fname );
    ldelim = *fname;
    rdelim = 0;
    if( ldelim == '[' )
	rdelim = ']';
    if( ldelim == '<' )
	rdelim = '>';
    if( rdelim )
	{
	char *i;
	char *j;
	char *tfname;

	tfname = malloc_noown( strlen( fname ) + 16 );
	i = strchr( fname, rdelim );
	if( !i )
	    goto EXIT1;
	j = strchr( fname, '.' );
	if( j && ( j < i ) )
	    i = j;
	sprintf( tfname, "%.*s", i - fname - 1, fname + 1 );
	t = iso9660_lookup( of, tfname );
	if( t != msg_success )
	    goto EXIT1;
	if( i == j )
	    sprintf( tfname, "%c%s", ldelim, i + 1 );
	else
	    sprintf( tfname, "%s", i + 1 );
	t = iso9660_lookup( of, tfname );
EXIT1:
	free( tfname );
	}
    else
	t = iso9660_find_file( of, fname );
    return( t );
    }

int iso9660_find_file( struct of *of, char *fname )
    {
    char *c;
    int len;
    int match;
    int t;
    struct DIRECTORY *drec;

    zprintf( "trying to find `%s'\n", fname );
    len = strlen( fname );
    match = 0;
    t = iso9660_find_directory_record( of );
    if( t != msg_success )
	goto EXIT1;
    of->bib += of->len;
    t = iso9660_find_directory_record( of );
    if( t != msg_success )
	goto EXIT1;
    of->bib += of->len;
    while( 1 )
	{
	if( killpending( ) )
	    goto EXIT2;
	t = iso9660_find_directory_record( of );
	if( t != msg_success )
	    goto EXIT1;
	drec = &of->block[of->bib];
	zprintf( "comparing with `%.*s'\n",
		drec->DREC$B_FILE_ID_LENGTH, drec->DREC$B_FILE_ID );
#if 0
	match = ( ( len == drec->DREC$B_FILE_ID_LENGTH )
		&& ( strncmp_nocase( fname, drec->DREC$B_FILE_ID, len ) == 0 ) );
#else
        match = match_filename( drec->DREC$B_FILE_ID, fname, len );
#endif
	if( match )
	    {
	    t = iso9660_get_header( of, drec );
	    goto EXIT1;
	    }
	of->bib += of->len;
	}
EXIT2:
    t = msg_failure;
EXIT1:
    return( t );
    }

int iso9660_read_file( struct of *of, char *c )
    {
    int t;

    if( of->raw )
	t = iso9660_read_file_raw( of, c );
    else
	switch( of->rtype )
	    {
	    case XAR_RFM$K_LSB_VARIABLE:
	    case XAR_RFM$K_MSB_VARIABLE:
		t = iso9660_read_file_variable( of, c );
		break;

	    default:
		t = iso9660_read_file_raw( of, c );
		break;
	    }
    return( t );
    }

int iso9660_read_file_raw( struct of *of, char *c )
    {
    int t;

    t = iso9660_find_byte( of );
    if( t != msg_success )
	return( t );
    *c = of->block[of->bib++];
    return( msg_success );
    }

int iso9660_read_file_variable( struct of *of, char *c )
    {
    int t;

    if( of->bir >= of->len )
	{
	if( ( of->bir == of->len ) && ( of->rattr == XAR_ATR$K_CRLF ) )
	    {
	    of->bir++;
	    *c = '\n';
	    return( msg_success );
	    }
	t = iso9660_find_record( of );
	if( t != msg_success )
	    return( t );
	t = iso9660_read_file_variable( of, c );
	return( t );
	}
    t = iso9660_find_byte( of );
    if( t != msg_success )
	return( t );
    *c = of->block[of->bib++];
    of->bir++;
    return( msg_success );
    }

int iso9660_find_byte( struct of *of )
    {
    int t;

    if( of->bib >= of->od->size )
	{
	of->curr_lbn++;
	if( of->curr_lbn > of->last_lbn )
	    return( msg_eof );
	t = iso9660_get_lbn( of->curr_lbn, of->block, of->od );
	if( t != msg_success )
	    return( t );
	of->bib = 0;
	}
    if( ( of->curr_lbn == of->last_lbn ) && ( of->bib >= of->ffbyte ) )
	return( msg_eof );
    return( msg_success );
    }

int iso9660_find_record( struct of *of )
    {
    int t;

    if( of->bib & 1 )
	of->bib++;
    if( of->bib >= of->od->size )
	{
	of->curr_lbn++;
	if( of->curr_lbn > of->last_lbn )
	    return( msg_eof );
	t = iso9660_get_lbn( of->curr_lbn, of->block, of->od );
	if( t != msg_success )
	    return( t );
	of->bib = 0;
	}
    if( ( of->curr_lbn == of->last_lbn ) && ( of->bib >= of->ffbyte ) )
	{
	zprintf( "curr_lbn = %d, last_lbn = %d, bib = %d, ffb = %d\n",
		of->curr_lbn, of->last_lbn, of->bib, of->ffbyte );
	return( msg_eof );
	}
    switch( of->rtype )
	{
	case XAR_RFM$K_LSB_VARIABLE:
	    of->len = ( of->block[of->bib+1] << 8 ) + of->block[of->bib];
	    break;

	case XAR_RFM$K_MSB_VARIABLE:
	    of->len = ( of->block[of->bib] << 8 ) + of->block[of->bib+1];
	    break;
	}
    of->bib += 2;
    of->bir = 0;
    return( msg_success );
    }

int iso9660_find_directory_record( struct of *of )
    {
    int i;
    int t;

    while( 1 )
	{
	if( of->bib < LOGICAL_BLOCK_SIZE )
	    {
	    of->len = of->block[of->bib];
	    if( of->len )
		{
		of->bir = 0;
		return( msg_success );
		}
	    }
	for( i = 0; i < LOGICAL_BLOCK_SIZE; i += of->od->size )
	    {
	    of->curr_lbn++;
	    if( of->curr_lbn > of->last_lbn )
		return( msg_eof );
	    t = iso9660_get_lbn( of->curr_lbn, of->block + i, of->od );
	    if( t != msg_success )
		return( t );
	    }
	of->bib = 0;
	}
    }

int iso9660_scan_directory( struct of *of, char *dname, char *fname, struct cb *cb )
    {
    char ldelim;
    char rdelim;
    char *c;
    char *x;
    char *tfname;
    char *t1fname;
    char *t2fname;
    char *t3fname;
    int print;
    int recurse;
    int match;
    int t;
    struct REGEXP *r;
    struct of *tof;
    struct DIRECTORY *drec;

    if( killpending( ) )
	cb->t = msg_ctrlc;
    if( cb->t != msg_success )
	return( cb->t );
    zprintf( "scanning directory `%s', looking for `%s'\n", dname, fname );
    t1fname = malloc_noown( strlen( fname ) + 16 );
    t2fname = malloc_noown( strlen( fname ) + 16 );
    t3fname = malloc_noown( strlen( fname ) + 16 );
    t = msg_failure;
    ldelim = *fname;
    rdelim = 0;
    if( ldelim == '[' )
	rdelim = ']';
    if( ldelim == '<' )
	rdelim = '>';
    if( rdelim )
	{
	char *i;
	char *j;

	i = strchr( fname + 1, rdelim );
	if( !i )
	    goto EXIT1;
	if( strncmp_nocase( fname + 1, "...", 3 ) == 0 )
	    {
	    if( i == fname + 4 )
		{
		strcpy( t1fname, fname + 5 );
		strcpy( t2fname, fname );
		strcpy( t3fname, fname );
		print = 1;
		}
	    else
		{
		strcpy( t2fname, i + 1 );
		strcpy( t3fname, fname );
		j = strchr( fname + 4, '.' );
		if( j == fname + 4 )
		    goto EXIT1;
		if( j && ( j < i ) )
		    i = j;
		sprintf( t1fname, "%.*s", i - fname - 4, fname + 4 );
		print = 0;
		}
	    }
	else
	    {
	    j = strchr( fname + 1, '.' );
	    if( j == fname + 1 )
		goto EXIT1;
	    if( j && ( j < i ) )
		{
		sprintf( t1fname, "%.*s", j - fname - 1, fname + 1 );
		if( strncmp_nocase( j, "...", 3 ) == 0 )
		    sprintf( t2fname, "%c%s", ldelim, j );
		else
		    sprintf( t2fname, "%c%s", ldelim, j + 1 );
		}
	    else
		{
		sprintf( t1fname, "%.*s", i - fname - 1, fname + 1 );
		sprintf( t2fname, "%s", i + 1 );
		}
	    strcpy( t3fname, "" );
	    print = 0;
	    }
	recurse = 1;
	}
    else
	{
	strcpy( t1fname, fname );
	strcpy( t2fname, "" );
	strcpy( t3fname, "" );
	print = 1;
	recurse = 0;
	}
    zprintf( "t1fname = `%s'\n", t1fname );
    zprintf( "t2fname = `%s'\n", t2fname );
    zprintf( "t3fname = `%s'\n", t3fname );
    r = iso9660_compile( t1fname );
    tof = malloc_noown( sizeof( *tof ) );
    tof->od = of->od;
    tof->block = malloc_noown( LOGICAL_BLOCK_SIZE );
    match = 0;
    tfname = malloc_noown( 38 );
    t = iso9660_find_directory_record( of );
    if( t == msg_eof )
	goto EXIT4;
    if( t != msg_success )
	goto EXIT2;
    of->bib += of->len;
    t = iso9660_find_directory_record( of );
    if( t == msg_eof )
	goto EXIT4;
    if( t != msg_success )
	goto EXIT2;
    of->bib += of->len;
    while( 1 )
	{
	if( killpending( ) )
	    goto EXIT3;
	t = iso9660_find_directory_record( of );
	if( t == msg_eof )
	    goto EXIT4;
	if( t != msg_success )
	    goto EXIT2;
	drec = &of->block[of->bib];
	t = drec->DREC$B_FILE_ID_LENGTH;
	strncpy( tfname, drec->DREC$B_FILE_ID, t );
	zprintf( "tfname = `%s'\n", tfname );
        x = strchr( tfname, ';' );
        if( x )
	    t = x - tfname;
	tfname[t] = 0;
	zprintf( "new tfname = `%s'\n", tfname );
	match = ( regexp_match( r, tfname ) == 1 );
	if( print && match )
	    {
	    if( killpending( ) )
		goto EXIT3;
	    t = iso9660_show_file( tof, dname, tfname, drec, cb );
	    if( t != msg_success )
		goto EXIT2;
	    }
	if( recurse && drec->DREC$V_DIRECTORY )
	    {
	    char *tdname;
	    int i;
	    int j;

	    t = iso9660_get_header( tof, drec );
	    if( t != msg_success )
		goto EXIT2;
	    i = strlen( tfname );
	    if( strcmp_nocase( dname, "[]" ) == 0 )
		{
		tdname = malloc_noown( i + 16 );
		sprintf( tdname, "%c%.*s%c", ldelim, i, tfname, rdelim );
		}
	    else
		{
		j = strlen( dname ) - 1;
		tdname = malloc_noown( i + j + 16 );
		sprintf( tdname, "%.*s.%.*s%c", j, dname, i, tfname, rdelim );
		}
	    if( match )
		t = iso9660_scan_directory( tof, tdname, t2fname, cb );
	    else if( strlen( t3fname ) != 0 )
		t = iso9660_scan_directory( tof, tdname, t3fname, cb );
	    free( tdname );
	    if( t != msg_success )
		goto EXIT2;
	    }
	of->bib += of->len;
	}
EXIT4:
    t = msg_success;
    goto EXIT2;
EXIT3:
    t = msg_ctrlc;
    cb->t = t;
EXIT2:
    free( tfname );
    free( tof->block );
    free( tof );
    regexp_release( r );
EXIT1:
    free( t3fname );
    free( t2fname );
    free( t1fname );
    return( t );
    }

int iso9660_compile( char *fname )
    {
    char c;
    char *tfname;
    char *t;
    struct REGEXP *r;

    zprintf( "compiling `%s'\n", fname );
    tfname = malloc_noown( strlen( fname ) * 2 + 5 );
    t = tfname;
    c = *fname;
    if( !c || ( c == '.' ) || ( c == ';' ) )
	{
	*t++ = '.';
	*t++ = '*';
	}
    while( c = *fname++ )
	{
	switch( c )
	    {
	    case '*':
		*t++ = '.';
		*t++ = '*';
		break;

	    case '%':
		*t++ = '.';
		break;

	    default:
		*t++ = '\\';
		*t++ = c;
		break;
	    }
	}
    *t++ = 0;
    zprintf( "tfname = `%s'\n", tfname );
    r = regexp_compile( tfname, 1 );
    free( tfname );
    return( r );
    }

int iso9660_show_file( struct of *of, char *dname, char *fname,
	struct DIRECTORY *drec, struct cb *cb )
    {
    char *raw;
    char *name;
    char *attr;
    char *both;
    struct BINARY_DATE_TIME *butc;
    
    raw = cb->name[7] == '_' ? "_raw" : "";
    name = malloc_noown( 1024 );
    attr = malloc_noown( 37 );
    both = malloc_noown( 1024 + 81 );
    if( cb->longformat )
	{
	butc = drec->DREC$B_FILE_RECORDING;
	sprintf( attr + 0, "%11d", drec->DREC$L_DATA_LENGTH );
	if( ( butc->BUTC$B_DAY >= 1 )
		&& ( butc->BUTC$B_MONTH >= 1 )
		&& ( butc->BUTC$B_MONTH <= 12 ) )
	    sprintf( attr + 11, "  %2d-%3s-%4d %02d:%02d:%02d.00",
		    butc->BUTC$B_DAY,
		    iso9660_months[butc->BUTC$B_MONTH-1],
		    butc->BUTC$B_YEAR + 1900,
		    butc->BUTC$B_HOUR,
		    butc->BUTC$B_MINUTE,
		    butc->BUTC$B_SECOND );
	}
    else
	attr[0] = 0;
#if LOCAL_DEBUG
    sprintf( name, "%s%s", dname, fname );
#else
    sprintf( name, "iso9660%s:%s%s/%s", raw, dname, fname, cb->next );
#endif
    if( strlen( attr ) )
	if( strlen( name ) < 42 )
	    sprintf( both, "%-42.42s%s", name, attr );
	else
	    sprintf( both, "%s\n%42.42s%s", name, "", attr );
    else
	strcpy( both, name );
    cb->t = cb->callback( both, cb->p0, cb->p1 );
    free( name );
    free( attr );
    free( both );
    return( cb->t );
    }

int iso9660_get_header( struct of *of, struct DIRECTORY *drec )
    {
    int size;
    int t;
    struct XAR_RECORD *xar;

    if( drec->DREC$V_DIRECTORY && ( drec->DREC$B_FILE_ID_LENGTH == 1 ) )
	zprintf( "getting header for root directory\n" );
    if( !drec->DREC$V_DIRECTORY )
	zprintf( "getting header for `%.*s'\n",
		drec->DREC$B_FILE_ID_LENGTH, drec->DREC$B_FILE_ID );
    size = of->od->size;
    of->rtype = XAR_RFM$K_UNDEFINED;
    if( drec->DREC$V_RECORD && drec->DREC$B_XAR_LENGTH )
	{
	xar = malloc_noown( size );
	t = iso9660_get_lbn( drec->DREC$L_EXTENT_LOCATION, xar, of->od );
	if( t == msg_success )
	    {
	    of->rtype = xar->XAR$B_RECORD_FORMAT;
	    of->rattr = xar->XAR$B_RECORD_ATTRIBUTES;
	    }
	free( xar );
	}
    of->curr_lbn = drec->DREC$L_EXTENT_LOCATION + drec->DREC$B_XAR_LENGTH - 1;
    of->last_lbn = drec->DREC$L_EXTENT_LOCATION + drec->DREC$B_XAR_LENGTH
	    + ( drec->DREC$L_DATA_LENGTH - 1 ) / size;
    of->ffbyte = drec->DREC$L_DATA_LENGTH % size;
    if( of->ffbyte == 0 )
	of->ffbyte = size;
    of->len = 0;
    of->bib = -1;
    of->bir = -1;
    of->offset = 0;
    return( msg_success );
    }

int iso9660_get_lbn( int lbn, unsigned char *block, struct od *od )
    {
#if LOCAL_DEBUG
#if LOCAL_DEBUG_RAW
    short iosb[4];
    int t;

    t = sys$qiow( 0, od->fp, IO$_READLBLK, iosb, 0, 0,
	    block, od->size, lbn * od->size / 512, 0, 0, 0 );
    if( !( t & 1 ) || !( iosb[0] & 1 ) || ( iosb[1] != od->size ) )
	return( msg_failure );
#else
    fseek( od->fp, ( (INT)lbn - 1 ) * od->size, 0 );
    if( fread( block, 1, od->size, od->fp ) != od->size )
	return( msg_failure );
#endif
#else
    yprintf( "reading %x bytes at LBN %x\n", od->size, lbn );
    fseek( od->fp, (INT)lbn * od->size, 0 );
    if( fread( block, 1, od->size, od->fp ) != od->size )
	return( msg_failure );
#endif
    return( msg_success );
    }

/*
 *  Return true if a pathname contains a valid ISO9660 file system, and false 
 *  otherwise.
 */
int iso9660_validate( char *vname )
    {
    int t;
    struct VD *vd;
    struct od *od;

    t = 0;
    od = malloc_noown( sizeof( *od ) );
    od->fp = fopen( vname, "rs" );
    if( !od->fp )
	goto EXIT1;
    od->size = LOGICAL_BLOCK_SIZE;
    vd = malloc_noown( LOGICAL_BLOCK_SIZE );
    od->pvd = vd;
    t = iso9660_get_lbn( DATA_AREA, vd, od );
    if( t != msg_success )
	goto EXIT2;
    yprintf( "volume descriptor type = %x\n", vd->VD$B_VOLUME_DESCRIPTOR_TYPE );
    if( vd->VD$B_VOLUME_DESCRIPTOR_TYPE != VD$K_PVD )
        goto EXIT2;
    yprintf( "volume descriptor version = %x\n", vd->VD$B_VOLUME_DESCRIPTOR_VERS );
    if( vd->VD$B_VOLUME_DESCRIPTOR_VERS != ISO_9660_VERSION_1988 )
        goto EXIT2;
    t = 1;
EXIT2:
    fclose( od->fp );
    free( vd );
EXIT1:
    free( od );
    return( t );
    }

u_long iso_getfile_info ( struct FILE *fp, struct _FILE_INFORMATION *Buffer )
{
    return ESUCCESS;
}
/*
 * Match 2 filenames. Ignore ';' and terminating .'s 
 */
int match_filename (char *dirname, char *fname, int len ) 
{
    char *x;
    int count;

    count = len;

/* Look for a ';'. If found, adjust the count to end at it's position */

    x = strchr (dirname, ';');
    if ( x != 0 ) 
        count = (int)(x - dirname); 

    if ( strncmp_nocase (dirname, fname, count) == 0 )
         return TRUE;
    else
        return FALSE;
}
