/* file:	edit.c
 *
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
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	Edit command
 *
 *  AUTHORS:
 *
 *      Stephen F. Shirron
 *
 *  CREATION DATE:
 *  
 *      11-Feb-1992
 *
 *  MODIFICATION HISTORY:
 *
 *      cbf     27-Jan-1993     add arg to read_with_prompt for echo functn.

 *	ajb	17-Dec-1992	Eliminate internal restrictions on file length.
 *
 *	pel	06-Mar-1992	add arg to satisfy new read_with_prompt functn.
 *
 *	sfs	11-Feb-1992	Initial entry.
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:common.h"
#include "cp$src:ctype.h"

/*
 * A line structure keeps track of the line number, and a pointer to the
 * line itself.
 */
struct ls {
    int lnum;
    char *text;
} ;

/*
 * The data buffer is collected into a structure so that we may pass
 * easily edit information across subroutine boundaries and remain reentrant.
 * This structure is reallocated as needed to expand the lsp structures, which
 * are of necessity placed at the end of this structure.
 */
struct db {
    int num_allocated;		/* number of line structures allocated	*/
    int num_lines;		/* number of line structures in use	*/
    int textindex;	      	/* index into the actual text portion of the line */
    char input[256];		/* input buffer		     		*/
    struct ls lsp[256];		/* array of line structures (KEEP LAST!) */
} ;

#include "cp$inc:prototypes.h"

/*+
 * ============================================================================
 * = edit - Invoke the console BASIC-like line editor on a file.              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	'edit' is a console editor which behaves much like a BASIC line editor.
 *	With 'edit' lines of a file may be added, inserted, or deleted.
 *	The editor has the following set of subcommands:
 *	HELP, LIST, RENUMBER, EXIT, and QUIT.
 *
 *      To invoke the editor simply 'edit filename', where filename is an
 *	existing file.
 *	
 *	The editor may be used to modify the user powerup script, 'nvram',
 *	or any other user created file, such as 'foo' in the examples below.
 *	Note 'nvram' is a special script which is always invoked during
 *	the powerup sequence.  Hence, any actions which the user wishes
 *	to execute on any powerup, can be saved in the non-volatile memory
 *	using the 'nvram' script file.
 *  
 *	The editor will handle arbitrarily large input streams, up to the
 *	size of the heap.
 *
 *   COMMAND FMT: edit 2 NZ 0 edit
 *
 *   COMMAND FORM:
 *
 *      edit ( <file>
 *             [Subcommands: HELP, LIST, RENUMBER, EXIT or CTL/Z, QUIT]
 *             [<nn> : Delete line number <nn>.]
 *             [<nn> <text> : Add or overwrite line <nn> with <text>.] )
 *
 *   COMMAND TAG: edit 0 RXBZ edit
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<file> - Specifies the name of the file to be edited. 
 *		 Note the file must already exist.
 *
 *   SUBCOMMAND OPTION(S):
 *
 *	HELP - Display the brief help file.
 *	LIST - List the current file prefixed with line numbers.
 *	RENUMBER - Renumber the lines of the file in increments of 10.
 *	EXIT - Leave the editor and close the file saving all changes.
 *	QUIT - Leave the editor and close the file without saving changes.
 *	CTL/Z - Acts the same as EXIT, leave and save all changes.
 *	<nn> - Delete line number <nn>.
 *	<nn> <text> - Add or overwrite line <nn> with <text>.
 *
 *   COMMAND EXAMPLE(S):
 *
 *      The following is an example showing how to create an modify a script
 *      'foo' using edit.
 *~
 *      >>>echo > foo 'this is a test'      # Create a sample file.
 *      >>>cat foo
 *      this is a test
 *      >>>edit foo                         # Edit the newly created file.
 *      editing `foo'
 *      15 bytes read in
 *      *help
 *      Think "BASIC line editor", and see if that'll do the trick
 *      *list
 *        10  this is a test
 *      *20 of the console BASIC-like line editor
 *      *30 This editor supports HELP, LIST, RENUMBER, EXIT, and QUIT.
 *      *list
 *        10  this is a test
 *        20  of the console BASIC-like line editor
 *        30  This editor supports HELP, LIST, RENUMBER, EXIT, and QUIT.
 *      *10 This is a test of the console BASIC-like line editor.
 *      *20
 *      *list
 *        10  This is a test of the console BASIC-like line editor.
 *        30  This editor supports HELP, LIST, RENUMBER, EXIT, and QUIT.
 *      *15 It may be used to create scripts files at the console.
 *      *list
 *        10  This is a test of the console BASIC-like line editor.
 *        15  It may be used to create scripts files at the console.
 *        30  This editor supports HELP, LIST, RENUMBER, EXIT, and QUIT.
 *      *renumber
 *      *list
 *        10  This is a test of the console BASIC-like line editor.
 *        20  It may be used to create scripts files at the console.
 *        30  This editor supports HELP, LIST, RENUMBER, EXIT, and QUIT.
 *      *exit
 *      168 bytes written out
 *      >>>cat foo		            # Note EXIT saves changes.
 *      This is a test of the console BASIC-like line editor.
 *      It may be used to create scripts files at the console.
 *      This editor supports HELP, LIST, RENUMBER, EXIT, and QUIT.
 *      >>>edit foo
 *      editing `foo'
 *      168 bytes read in
 *      *20
 *      *list
 *        10  This is a test of the console BASIC-like line editor.
 *        30  This editor supports HELP, LIST, RENUMBER, EXIT, and QUIT.
 *      *quit
 *      >>>cat foo                          # Note QUIT does not save changes.
 *      This is a test of the console BASIC-like line editor.
 *      It may be used to create scripts files at the console.
 *      This editor supports HELP, LIST, RENUMBER, EXIT, and QUIT.
 *      >>>
 *~
 *      The next example shows how do modify the non-volatile user-defined
 *      power-up script, 'nvram'.
 *~
 *      >>>edit nvram                   # Modify user powerup script, nvram.
 *      editing `nvram'                     
 *      0 bytes read in
 *      *10 set boot_dev eza0
 *      *20 set boot_osflags 0,0
 *      37 bytes written out
 *      >>>nvram                        # Execute the silent script, nvram.
 *      >>>edit nvram
 *      editing `nvram'
 *      37 bytes read in
 *      *15 show boot_dev
 *      *25 show boot_osflags
 *      *list
 *        10  set boot_dev eza0
 *        15  show boot_dev
 *        20  set boot_osflags 0,0
 *        25  show boot_osflags
 *      *exit
 *      69 bytes written out
 *      >>>cat nvram                    # List the modified file.
 *      set boot_dev eza0
 *      show boot_dev
 *      set boot_osflags 0,0
 *      show boot_osflags
 *      >>>nvram                        # Execute nvram, note the SHOWs.
 *      boot_dev                eza0
 *      boot_osflags            0,0
 *      >>>
 *                                      #
 *                                      # Reset system, note nvram execution.
 *	                                #
 *      Cobra powerup script start
 *      boot_dev                eza0
 *      boot_osflags            0,0
 *      Cobra powerup script end
 *      
 *      Cobra/Laser (COBRA) console X1.3-1505, built on Feb 13 1992 at 01:28:52
 *~
 *   COMMAND REFERENCES:
 *
 *	cat, echo
 *
 * FORM OF CALL:
 *  
 *	edit ( int argc, char *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - normal completion.
 *	msg_failure - routine failed
 *       
 * ARGUMENTS:
 *
 * 	argc		- number of words present on command line
 *	char *argv[]	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	A file may be modified.
 *
-*/

int edit( int argc, char *argv[] )
    {
    int i;
    int t;
    struct PCB *pcb;
    struct FILE *fp_in;
    struct FILE *fp_out;
    struct FILE *fp_err;

    pcb = getpcb( );
    fp_err = pcb->pcb$a_stderr;

    if( argc < 2 )
	{
	t = edit_sub( pcb->pcb$a_stdin, pcb->pcb$a_stdout, pcb->pcb$a_stderr );
	fprintf( fp_err, t );
	}
    else
	{
	for( i = 1; i < argc; i++ )
	    {
	    if( killpending( ) )
		break;
	    fprintf( fp_err, "editing `%s'\n", argv[i] );
	    fp_in = fopen( argv[i], "r+" );
	    if( !fp_in )
		fp_in = fopen( argv[i], "w" );
	    if( !fp_in )
		err_printf( msg_nofile );
	    else
		{
		t = edit_sub( fp_in, fp_in, pcb->pcb$a_stderr );
		fprintf( fp_err, t );
		}
	    fclose( fp_in );
	    }
	}
    return( msg_success );
    }

int edit_sub( struct FILE *fp_in, struct FILE *fp_out, struct FILE *fp_err )
    {
    char c;
    int cur;
    int len;
    int lnum;
    int lnum1;
    int lnum2;
    int i;
    int t;
    struct db *db;
    struct INODE *ip;

    /* Set up the edit buffer */
    db = dyn$_malloc( sizeof( *db ), DYN$K_SYNC | DYN$K_FLOOD );
    db->num_allocated = sizeof( db->lsp ) / sizeof( db->lsp[0] );

    /* read in the file and give them line numbers */
    cur = 0;
    lnum = 10;
    db->textindex = 0;
    while( fgets( &db->input[0], sizeof( db->input ), fp_in ) )
	{
	len = strlen( &db->input[0] );
	db->input[len-1] = 0; /* chop off trailing newline */
	db = edit_replace( db, lnum, lnum );
	if( killpending( ) )
	    break;
	cur += len;
	lnum += 10;
	}
    fprintf( fp_err, "%d bytes read in\n", cur );

    while( 1 )
	{
	len = read_with_prompt( "*", sizeof( db->input ), db->input, fp_err, fp_err, 1 ) - 1;
	if( killpending( ) )
	    break;
	if( len == EOF )
	    strcpy( db->input, "exit" );
	else if( len == 0 )
	    continue;
	else
	    db->input[len] = 0; /* chop off terminating newline */

	if( substr( db->input, "list" ) )
	    edit_list( db, fp_err );
	else if( substr( db->input, "renumber" ) )
	    edit_renumber( db );
	else if( substr( db->input, "help" ) )
	    edit_help( db, fp_err );
	else if( substr( db->input, "quit" ) )
	    break;
	else if( substr( db->input, "exit" ) )
	    {
	    if( fp_in == fp_out )
		{
		ip = fp_out->ip;
		ip->len[0] = 0;
		fseek( fp_out, 0, 0 );
		fwrite( 0, 0, 0, fp_out );
		}
	    cur = 0;
	    for( i = 0; i < db->num_lines; i++ )
		cur += fprintf( fp_out, "%s\n", db->lsp[i].text );
	    fprintf( fp_err, "%d bytes written out to %s\n", cur, fp_out->ip->name );
	    break;
	    }

	/* parse off the line number */
	else
	    {
	    db->textindex = 0;
	    lnum1 = 0;
            while( isdigit( db->input[db->textindex] ) )
		{
		lnum1 *= 10;
		lnum1 += db->input[db->textindex] - '0';
		db->textindex++;
		}
	    lnum2 = 0;
	    if( db->input[db->textindex] == '-' )
		{
		db->textindex++;
        	while( isdigit( db->input[db->textindex] ) )
		    {
		    lnum2 *= 10;
		    lnum2 += db->input[db->textindex] - '0';
		    db->textindex++;
		    }
		}
	    if( lnum2 < lnum1 )
		lnum2 = lnum1;
	    if( db->textindex )
		/* if a space follows the number insert the line */
		if( isspace( db->input[db->textindex] ) )
		    {
		    db->textindex++;
		    db = edit_replace( db, lnum1, lnum2 );
		    }
		/* otherwise, delete it */
		else
		    edit_delete( db, lnum1, lnum2 );
	    else
		fprintf( fp_err, "missing line number\n" );
	    }
	}

    /* release all the storage */
    for( i = 0; i < db->num_lines; i++ )
	dyn$_free( db->lsp[i].text, DYN$K_SYNC );
    dyn$_free( db, DYN$K_SYNC );
    return( msg_success );
    }

/*
 * List all the lines with their current line numbers
 */
void edit_list( struct db *db, struct FILE *fp_err )
    {
    int i;

    for( i = 0; i < db->num_lines; i++ )
	fprintf( fp_err, "%4d  %s\n", db->lsp[i].lnum, db->lsp[i].text );
    }

/*
 * Renumber all the lines, incrementing by 10
 */
void edit_renumber( struct db *db )
    {
    int i;

    for( i = 0; i < db->num_lines; i++ )
	db->lsp[i].lnum = i * 10 + 10;
    }

/*
 * Install a line into the edit buffer, and reallocate if we've run
 * out of room.  Assume the line number has been stripped off of
 * the input text.
 */
int edit_replace( struct db *db, int lnum1, int lnum2 )
    {
    int i;

    /* get rid of previous versions of this line */
    edit_delete( db, lnum1, lnum2 );

    /* allocate more room if we've run out of lines */
    if( db->num_lines >= db->num_allocated )
	{
	db->num_allocated += 128;
	db = dyn$_realloc( db,
		sizeof( struct db ) - sizeof( db->lsp ) + sizeof( db->lsp[0] ) * db->num_allocated,
		DYN$K_SYNC );
	}

    /* install the line at the end of the structure */
    i = db->num_lines;
    db->lsp[i].text = dyn$_malloc( strlen( &db->input[db->textindex] ) + 1, DYN$K_SYNC | DYN$K_NOFLOOD );
    strcpy( db->lsp[i].text, &db->input[db->textindex] );
    db->lsp[i].lnum = lnum1;
    db->num_lines++;

    /* always insure that line list is sorted */
    edit_sort( db );
    return( db );
    }

void edit_help( struct db *db, struct FILE *fp_err )
    {
    fprintf( fp_err, "Think \"BASIC line editor\", and see if that'll do the trick\n" );
    }

void edit_delete( struct db *db, int lnum1, int lnum2 )
    {
    int i;
    int j;

    /* see if the line already exists */
    for( i = lnum1; i <= lnum2; i++ )
	if( ( j = edit_find( db, i ) ) != -1 )
	    {
	    dyn$_free( db->lsp[j].text, DYN$K_SYNC );
	    memcpy( &db->lsp[j], &db->lsp[db->num_lines-1], sizeof( db->lsp[0] ) );
	    db->num_lines--;
	    }

    /* always insure that line list is sorted */
    edit_sort( db );
    }

/*
 * find a line given its line number and return the index, or -1 if not
 * found
 */
int edit_find( struct db *db, int lnum )
    {
    int i;

    for( i=0; i < db->num_lines; i++ )
	if( db->lsp[i].lnum == lnum )
	    return i;
    return( -1 );
    }

/*
 * Sort the line list by increasing line number
 */
void edit_sort( struct db *db )
    {
    int	i;
    int	j;
    int	gap;
    int n;

    n = db->num_lines;
    for( gap = n/2; gap > 0; gap /= 2 )
	for( i = gap; i < n; i++ )
	    for( j = i - gap; j >= 0; j -= gap )
		{
		if( db->lsp[j].lnum <= db->lsp[j+gap].lnum )
		    break;
		memswp( &db->lsp[j], &db->lsp[j+gap], sizeof( db->lsp[0] ) );
		}
    }
