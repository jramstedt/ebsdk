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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	NVRAM protocol driver
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *      ska	11-Nov-1998	Conditionalize for YukonA.
 *
 *	dtr	3-Oct-1997	Removed the code to permanently allocate the 
 *				nvram file pointer.  This way we have one set of
 *				routines that will/should work on all platforms
 *				regardless of how they access the eerom.
 *
 *	er	07-May-1996	Conditionalize for EBxx boards
 *
 *      jje     26-Mar-1996     Remove "access NVRAM" print for CORTEX
 * 
 *	pmd	23-Feb-1996	Remove "access NVRAM" print for MTU.
 *
 *	noh	20-Nov-1995	Remove "access NVRAM" print for BURNS.
 *
 *	swt	28-Jun-1995	Add Noritake platform.
 *
 *	mdr	25-Aug-1994	Remove "access NVRAM" print for medulla.
 *
 *	pel	25-Aug-1993	Use NVRAM_EV_LEN not 8192; Use
 *				NVRAM_BYTE_ALIGNMENT instead of 4 (cobra).
 *
 *	sfs	11-Feb-1992	Initial entry.
 *
 *      dda     17-Oct-1992     Fix append bug.
 */

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:common.h"
#include "cp$src:stddef.h"
#include "cp$src:alpha_arc.h"
#include "cp$src:alpha_arc_eisa.h"
#include "cp$src:nvram_def.h"
#include "cp$inc:prototypes.h"

#if GALAXY
#include "cp$inc:kernel_entry.h"
#include "cp$src:ev_def.h"

extern int env_table_ptr;	/* to support nvram_update_env() */
extern struct EVNODE evlist;
#if TURBO
extern int ev_initing;
#define nvram_write_sev galaxy_nvram_write_sev
#define nvram_read_sev galaxy_nvram_read_sev
#endif
#endif

#if RAWHIDE
/* missing from the red-line dynamic.c */
#define	malloc_noown(xx1)   \
    dyn$_malloc(xx1, DYN$K_SYNC|DYN$K_FLOOD|DYN$K_NOOWN, 0, 0, 0)
#endif

#if GALAXY && REGATTA
#define NVRAM_EV_OFF \
(get_console_base_pa() ? (EEROM_LENGTH-(2*NVRAM_EV_LEN)) : (EEROM_LENGTH-NVRAM_EV_LEN))
#else
#define NVRAM_EV_OFF (EEROM_LENGTH-NVRAM_EV_LEN)
#endif

struct FILE *nvram_fp;
struct srm_nvram *nvram_srm_nvram;

int nvram_read( );
int nvram_write( );
int nvram_open( );
int nvram_close( );
int null_procedure( );

struct DDB nvram_ddb = {
	"nvram",		/* how this routine wants to be called	*/
	nvram_read,		/* read routine				*/
	nvram_write,		/* write routine			*/
	nvram_open,		/* open routine				*/
	nvram_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	0,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};

int nvram_init( )
    {
    struct INODE *ip;
    struct FILE *fp;

    allocinode( nvram_ddb.name, 1, &ip );
    ip->dva = &nvram_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE | ATTR$M_EXECUTE;
    ip->loc = 0;
    ip->len[0] = 0;
    ip->alloc[0] = NVRAM_EV_LEN;
    INODE_UNLOCK( ip );
    /*
     *  Open EEROM permanently.  Then allocate a permanent copy of the
     *  NVRAM structure, and read it in.
     */
#if AVANTI || MIKASA || MEDULLA || ALCOR || K2 || MTU || CORTEX || UNIVERSE \
    || TAKARA || APC_PLATFORM || MONET || EIGER   /* no print for these platforms ... */
#else
    qprintf( "access NVRAM\n" );
#endif
    nvram_srm_nvram = malloc_noown( NVRAM_EV_LEN );
    nvram_get( nvram_srm_nvram );
    /*
     *  Open NVRAM in order to set ip->len correctly.
     */
    fp = fopen( "nvram", "r" );
    fclose( fp );
    return( msg_success );
    }

int nvram_read( struct FILE *fp, int size, int number, unsigned char *c )
    {
    char *value;
    int len;
    int len_value;

    len = size * number;
    value = fp->misc;
    len_value = strlen( value );
    if( fp->local_offset[0] > len_value )
	return( 0 );
    if( fp->local_offset[0] + len > len_value )
	len = len_value - fp->local_offset[0];
    memcpy( c, value + fp->local_offset[0], len );
    fp->local_offset[0] += len;
    return( len );
    }

int nvram_write( struct FILE *fp, int size, int number, unsigned char *c )
    {
    char *value;
    int len;

    len = size * number;
    value = fp->misc;
    if( fp->local_offset[0] > NVRAM_EV_LEN )
	return( 0 );
    if( fp->local_offset[0] + len > NVRAM_EV_LEN )
	len = NVRAM_EV_LEN - fp->local_offset[0];
    memcpy( value + fp->local_offset[0], c, len );
    fp->local_offset[0] += len;
    memset( value + fp->local_offset[0], 0, 1 );
    fp->ip->len[0] = fp->local_offset[0];
    return( len );
    }

int nvram_open( struct FILE *fp, char *info, char *next, char *mode )
    {
    char *value;

    value = malloc_noown( NVRAM_EV_LEN + 1 );
    nvram_read_sev( "nvram", value );
    fp->ip->len[0] = strlen( value );
    fp->misc = value;
    fp->offset = fp->local_offset;
    if( fp->mode & MODE_M_REWRITE )
	fp->ip->len[0] = 0;
    if( fp->mode & MODE_M_APPEND )
        fp->local_offset[0] = fp->ip->len[0];
    return( msg_success );
    }

int nvram_close( struct FILE *fp )
    {
    char *value;

    value = fp->misc;
    if( fp->mode & ( MODE_M_WRITE | MODE_M_REWRITE | MODE_M_APPEND ) )
	nvram_write_sev( "nvram", value );
    free( value );
    return( msg_success );
    }

unsigned short nvram_checksum( unsigned short *p )
    {
    unsigned short checksum;
    int i;

    checksum = 0;
    *p++;
    for( i = 1; i < ( NVRAM_EV_LEN / sizeof( *p ) ); i++ )
	checksum += *p++;
    return( checksum );
    }

int nvram_read_sev( char *name, char *value )
    {
    unsigned short *p;
    int len_name;

    len_name = strlen( name );
    /*
     *  Walk through the text section, trying to match the name given.
     */
    p = nvram_srm_nvram->text;
    while( *p )
	{
	/*
	 *  See if we've found the right name.
	 */
	if( strcmp_nocase( (int)p + sizeof( *p ), name ) == 0 )
	    {
	    /*
	     *  We've matched the name, so we return the value.
	     */
	    strcpy( value, (int)p + sizeof( *p ) + len_name + 1 );
	    return( msg_success );
	    }
	/*
	 *  Step to the next name.
	 */
	p = (int)p + *p;
	}
    return( msg_failure );
    }

int nvram_write_sev( char *name, char *value )
    {
    unsigned short *p;
    unsigned short *q;
    int len_name;
    int len_value;
    int n;

    len_name = strlen( name );
    len_value = strlen( value );
    /*
     *  Walk through the text section, trying to match the name given.
     */
    p = nvram_srm_nvram->text;
    while( *p )
	{
	/*
	 *  See if we've found the right name.
	 */
	if( strcmp_nocase( (int)p + sizeof( *p ), name ) == 0 )
	    {
	    /*
	     *  We've matched the name, so we need to get rid of this copy
	     *  of the saved environment variable.  To do this, just move
	     *  the rest of the text section down on top of where we are now.
	     */
	    q = (int)p + *p;
	    n = (int)nvram_srm_nvram + NVRAM_EV_LEN - (int)q;
	    memcpy( p, q, n );
	    }
	/*
	 *  Step to the next name.
	 */
	p = (int)p + *p;
	}
    /*
     *  Now we know that any saved copies of this environment variable have
     *  been overwritten.  We can save a copy at the end of the text section,
     *  if there is room.
     */
    q = (int)p + sizeof( *p ) + len_name + 1 + len_value + 1;
    n = (int)nvram_srm_nvram + NVRAM_EV_LEN - (int)q - sizeof( *q );
    if( n < 0 )
	{
	err_printf( "could not store %s, NVRAM full\n", name );
	nvram_get( nvram_srm_nvram );
	return( msg_failure );
	}
    /*
     *  Okay, we have room, so save this environment variable.  To do
     *  so, we save the size of the structure, the name of the environment
     *  variable, and its value.  Then we make sure that the list is
     *  properly terminated.
     */
    *p = sizeof( *p ) + len_name + 1 + len_value + 1;
    strcpy( (int)p + sizeof( *p ), name );
    strcpy( (int)p + sizeof( *p ) + len_name + 1, value );
    *q = 0;
    memset( (int)q + sizeof( *q ), 0, n );
    /*
     *  The constant EV_VALUE_LEN changed from 128 to 256 recently.
     *  If we are storing an EV longer than 128, then we must make
     *  sure the NVRAM version reflects the new version, so that it
     *  does not pollute old consoles (which could not handle such
     *  long EVs).
     */
    if( len_value >= OLD_EV_VALUE_LEN )
	if( strcmp( name, "nvram" ) )
	    if( nvram_srm_nvram->version == OLD_NVRAM_VERSION )
		nvram_srm_nvram->version = NVRAM_VERSION;
    nvram_put( nvram_srm_nvram );
    return( msg_success );
    }

int nvram_delete_sev( char *name )
    {
    unsigned short *p;
    unsigned short *q;
    int n;

    /*
     *  Walk through the text section, trying to match the name given.
     */
    p = nvram_srm_nvram->text;
    while( *p )
	{
	/*
	 *  See if we've found the right name.
	 */
	if( strcmp_nocase( (int)p + sizeof( *p ), name ) == 0 )
	    {
	    /*
	     *  We've matched the name, so we need to get rid of this copy
	     *  of the saved environment variable.  To do this, just move
	     *  the rest of the text section down on top of where we are now.
	     */
	    q = (int)p + *p;
	    n = (int)nvram_srm_nvram + NVRAM_EV_LEN - (int)q;
	    memcpy( p, q, n );
	    }
	/*
	 *  Step to the next name.
	 */
	p = (int)p + *p;
	}
    q = p;
    n = (int)nvram_srm_nvram + NVRAM_EV_LEN - (int)q;
    memset( q, 0, n );
    nvram_put( nvram_srm_nvram );
    return( msg_success );
    }

void nvram_get( struct srm_nvram *srm_nvram )
{
    struct FILE *fp;
    unsigned short checksum;

    if ( (fp = fopen( "eerom", "r" )) != NULL ) {
	fseek( fp, NVRAM_EV_OFF, SEEK_SET );
	fread( srm_nvram, NVRAM_EV_LEN, 1, fp );
	checksum = nvram_checksum( srm_nvram );
	if( (srm_nvram->checksum != checksum)
		|| (srm_nvram->version != NVRAM_VERSION
			&& srm_nvram->version != OLD_NVRAM_VERSION) ) {
	    memset( srm_nvram, 0, NVRAM_EV_LEN );
	    srm_nvram->version = NVRAM_VERSION;
	}
	fclose( fp );
    }
}

void nvram_put( struct srm_nvram *srm_nvram )
{
    struct FILE *fp;

    srm_nvram->checksum = nvram_checksum( srm_nvram );
    if ( (fp = fopen( "eerom", "r+" )) != NULL ) {
	fseek( fp, NVRAM_EV_OFF, SEEK_SET );
	fwrite( srm_nvram, NVRAM_EV_LEN, 1, fp );
	fclose( fp );
    }
}
#if GALAXY && RAWHIDE
/*+
 * ============================================================================
 * = nvram_update_env - Initialize environment variables from nvram	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine creates and initializes environment variables which
 *	are present in the nvram but are not built-in console variables.
 *	It is similar in function to the turbolaser specific routine.
 *
 * FORM OF CALL:
 *
 *	nvram_update_env(); 
 *
 * RETURNS:
 *
 *      Nothing
 *                          
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int nvram_update_env()
{
    unsigned short *p;
    char *name_ptr;
    char *value_ptr;
    struct env_table *et;
    struct EVNODE *evp;
    int status;
    int found;

    p = nvram_srm_nvram->text;	    /* start at the beginning of the vars */

    while (*p) {
    	name_ptr = (char *) ((int) p + sizeof(*p)); /* get the name */
	value_ptr = (char *) ((int) p + sizeof(*p) + strlen(name_ptr) + 1);

	/*
	 * Search the predefined list (from EV_ACTION.C) for this
	 * environment variable.
	 */
	found = 0;
	for (et = env_table_ptr; et->ev_name; et++) {
	    if (strcmp_nocase(name_ptr, et->ev_name) == 0) {
		found = 1;
		break;
	    }
	}

	/*
	 * If this is a user defined EV, add it to the environment
	 */
	if (!found) {
	    status = ev_write(name_ptr, value_ptr, EV$K_STRING);
	    if (status != msg_success)
		return msg_failure;
	    status = ev_locate(&evlist, name_ptr, &evp);
	    if (status != msg_success)
		return msg_failure;
	    evp->attributes |= EV$K_NONVOLATILE;    /* keep it non-volatile */
	}

	/*
	 * step to the next name.
	 */
	p = (int) p + *p;
    }

    return msg_success;
}
#endif

#if GALAXY & TURBO
/*+
 * ============================================================================
 * = nvram_update_env - Initialize environment variables from nvram	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine re-initializes environment variables which
 *	are present in the nvram, overwriting the values initialized
 *	from the Flash EEPROM.
 *
 * FORM OF CALL:
 *
 *	nvram_update_env(); 
 *
 * RETURNS:
 *
 *      Nothing
 *                          
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int nvram_update_env()
{
    unsigned short *p;
    char *name_ptr;
    char *value_ptr;
    struct env_table *et;
    struct EVNODE *evp;
    int status;
    int found;
    struct srm_nvram tmp_nvram;

    nvram_init();

    nvram_get( &tmp_nvram );
    p = tmp_nvram.text;		    /* start at the beginning of the vars */

    ev_initing = 1;
    while (*p) {
    	name_ptr = (char *) ((int) p + sizeof(*p)); /* get the name */
	value_ptr = (char *) ((int) p + sizeof(*p) + strlen(name_ptr) + 1);

	/*
	 * Search the EV list for this environment variable.
	 */
	if (ev_locate(&evlist,name_ptr,&evp) == msg_success)
	    ev_write(name_ptr, value_ptr,0,0);

	/*
	 * step to the next name.
	 */
	p = (int) p + *p;
    }
    ev_initing = 0;

    return msg_success;
}
#endif
