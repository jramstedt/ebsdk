/* file:	rd_driver.c
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
 *      Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *  Driver that implements the ram disk.
 *  When errors occur an error code is written to the file desc status.
 *
 *  AUTHORS:
 *
 *      Console Firmware Team
 *
 *  CREATION DATE:
 * 
 *      30-Apr-1990
 *
 *--
 */

#include "cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:msg_def.h"
#if !MODULAR
#include	"cp$src:encap.c"
#endif

#if MODULAR
struct encap_s_files {
	int	*address;
	int	attributes;
	char	*name;
};
#endif

#include "cp$inc:prototypes.h"

extern int null_procedure ();

int rd_read ();
int rd_write ();
int rd_open ();
int rd_close ();
int rd_delete ();
int rd_create ();
int rd_validate ();

struct DDB rd_ddb = {
	"rd",			/* how this routine wants to be called	*/
	rd_read,		/* read routine				*/
	rd_write,		/* write routine			*/
	rd_open,		/* open routine				*/
	rd_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	rd_delete,		/* delete routine			*/
	rd_create,		/* create routine			*/
	null_procedure,		/* setmode				*/
	rd_validate,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	1,			/* is a filesystem driver		*/
};
/*+
 * ============================================================================
 * = rd_read - read a block of data from a specified file                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * read a block of data from a specified file.
 * Transfers the minimum of either maxchar or however many bytes are in file.
 *  
 * FORM OF CALL:
 *  
 *	rd_read( fp, int size, int number, buf);
 *  
 * RETURNS:
 *
 *      number of bytes transfered
 *       
 *      0 if file is pending delete or EOF
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *	int size	- size of item
 *	int number	- number of items
 *      char *buf       - address of buffer to read into
-*/

int rd_read (struct FILE *fp, int size, int number, char *buf) {
	struct INODE	*ip;
	int		xfer_length;

	/*
	 * Synchronize access
	 */
	ip = fp->ip;
	INODE_LOCK (ip);
        if (ip->inuse == 0) {
            xfer_length = 0;  /* file is pending delete, return error status */
	    fp->status = msg_fatal;

	} else {
	    /*
	     * Figure out how much data can be moved
	     */
            xfer_length = max (min ((size*number), ip->len[0] - *fp->offset), 0);
	    memcpy (buf, & ip->loc [*fp->offset], xfer_length);
	    *fp->offset += xfer_length;
        }

	/*
	 * release access
	 */
	INODE_UNLOCK (ip);
	return xfer_length;
}

/*+
 * ============================================================================
 * = rd_write - store a block of characters to a specified file               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Store a block of characters to a specified file. Extend the file if 
 * necessary and if file does not have the fixed, "f" inode attribute.
 * If the file has the fixed attribute and all of the data, len chars, cannot
 * fit into the file then no chars are written.
 *  
 * FORM OF CALL:
 *  
 *	rd_write( fp, size, number, buf);
 *  
 * RETURNS:
 *
 *	number of bytes transfered.
 *       
 *      0 if error.
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *	int size	- size of item
 *	int number	- number of items
 *      char *buf       - address of buffer to write characters from
 *
 * SIDE EFFECTS:
 *
 *	Will extend the file if file doesn't have room for len characters.
-*/

int rd_write (struct FILE *fp, int size, int number, char *buf) {
    struct INODE	*ip;
    int		        expansion;
    int		        requested_expansion;

    /* Serialize access to the file. */
    ip = fp->ip;
    INODE_LOCK (ip);

    if (ip->inuse == 0) {
	fp->status = msg_fatal;
        INODE_UNLOCK (ip);
        return 0;      /* file is pending delete, return error status */
    }

    *fp->offset = ip->len[0];

    /*
     * Extend the file if we don't have enough room and file is not of
     * fixed attribute.  Expand by at least DEF_EXPAND bytes.
     */
    requested_expansion = (*fp->offset + size*number) - ip->alloc[0];
    if (requested_expansion > 0) {
	expansion = max (requested_expansion, DEF_EXPAND);
	ip->alloc[0] += expansion;
	ip->loc = (char *) dyn$_realloc (ip->loc, ip->alloc[0], 
					     DYN$K_SYNC | DYN$K_NOOWN);
    }

    /*
     * Now presume we have enough room.  Stuff the buffer
     * and bump the length if necessary.
     */
    memcpy (&ip->loc [*fp->offset], buf, size*number);
    *fp->offset += size*number;
    ip->len[0] = *fp->offset;

    /*
     * Release the serialization lock
     */
    INODE_UNLOCK (ip);
    return size*number;
}

/*+
 * ============================================================================
 * = rd_open - open a RAM disk file                                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Open a file on the ram disk.
 *  
 * FORM OF CALL:
 *  
 *	rd_open( fp);
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 *	msg_fatal - file is pending delete.
 *       
 *      msg_write_prot - device not writeable
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
-*/


int rd_open (struct FILE *fp, char *info, char *next, char *mode) {
    int status;
    struct INODE *ip;

    ip = fp->ip;
    INODE_LOCK (ip);
    if (!ip->inuse) {
	status = msg_fatal;
    } else {
	if (fp->mode & MODE_M_REWRITE) {
	    ip->len[0] = 0;
	    memset (&ip->append_offset, 0, sizeof (ip->append_offset));
	    ip->attr = DEF_PERMISSION | ATTR$M_EXECUTE;
	    if (strchr (mode, 'b'))
		ip->attr |= ATTR$M_BINARY;
        }
	if (((fp->mode & MODE_M_READ)  &&  !(ip->attr & ATTR$M_READ)) ||
	    ((fp->mode & MODE_M_WRITE) &&  !(ip->attr & ATTR$M_WRITE)) ) {
	    status = msg_bad_access_mode;
	} else {
	    if (fp->mode & MODE_M_APPEND) {
		fp->offset = &ip->append_offset;
		memcpy (&ip->append_offset, &ip->len[0], sizeof (ip->len[0]));
	    } else {
		fp->offset = &fp->local_offset;
		if (*info) *fp->offset = xtoi (info);
	    }
	    status = msg_success;
	}
    }
    INODE_UNLOCK (ip);

    return status;
}

/*+
 * ============================================================================
 * = rd_close - close a RAM disk file                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	close a RAM disk file
 *  
 * FORM OF CALL:
 *  
 *	rd_close( fp);
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
-*/

int rd_close (struct FILE *fp) {
    struct INODE	*ip;

    ip = fp->ip;
    INODE_LOCK (ip);

    /*
     * if file was pending delete & this is last process closing the file
     * then delete the file.
     */
    if ( (ip->ref == 0) && (ip->inuse == 0) )
        rd_delete( ip);       /* rd_delete posts iolock */
    else 
        INODE_UNLOCK (ip);

    return msg_success;
}

/*+
 * ============================================================================
 * = rd_create - create a RAM disk file                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	create a RAM disk file
 *  
 * FORM OF CALL:
 *  
 *	rd_create( ip, attributes, filesz);
 *  
 * RETURNS:
 *
 *	msg_success
 *	msg_no_file_slot - no inodes available 
 *      msg_file_in_use  - file is open by this or another process
 *	msg_ambig_file   - filename is ambiguous
 *       
 * ARGUMENTS:
 *
 * 	struct INODE *ip - address of empty inode slot
 *      int attributes   - read/write/execute permission
 *      int filesz - initial file size. Use DEF_ALLOC if filesz = 0.
 *
 * SIDE EFFECTS:
 *
 *	Inits inode ref count to 0 & all inode semaphores to 1.
-*/

/*----------*/
void rd_create_helper (struct INODE *ip, int attributes, int filesz ) {
		/* inode's iolock must be taken out & posted by caller */
        ip->dva = & rd_ddb;
	ip->attr = attributes;
	ip->alloc[0] = filesz;
	ip->loc = (char *) dyn$_malloc (filesz, DYN$K_SYNC | DYN$K_NOOWN);
}

/*----------*/
int rd_create (char *filename, int attributes, int filesz ) {
    int		 status, size;
    struct INODE *ip;

    status = msg_success;
    size = (filesz) ? filesz : DEF_ALLOC;

    switch( allocinode( filename, 1, &ip)) {
    case 0:        			/* not found, not created */
        status = msg_no_file_slot;	/* no inodes available */
        break;
    case 1:        			/* found */
        if (ip->ref)
	    status = msg_file_in_use;
	else {		/* rewrite over the file, realloc it if diff size */
/* ajbfix: delete then reallocate */
	    ip->len[0] = 0;
	    memset (&ip->append_offset, 0, sizeof (ip->append_offset));
	    ip->attr = attributes;
	    if (ip->alloc[0] != size) {
		ip->alloc[0] = size;
		ip->loc = (char *) dyn$_realloc (ip->loc, size, 
						 DYN$K_SYNC | DYN$K_NOOWN);
	    }
	}
	INODE_UNLOCK (ip);
        break;
    case 2:        /* not found and created */
	rd_create_helper( ip, attributes, size);
	INODE_UNLOCK (ip);
        break;
    case 3:					/* ambiguous filename */
	status = msg_ambig_file;
        break;
    }

    return status;
}

/*+
 * ============================================================================
 * = rd_delete - delete a RAM disk file                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Delete an inode on the ram disk.  Assume that the inode's iolock is taken
 * out.  If this isn't the last process using the file then the file won't be
 * deleted but will be have a deletion pending flag set.
 *  
 * FORM OF CALL:
 *  
 *	rd_delete( ip);
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 * 	struct INODE *ip - address of inode slot to be deleted
 *
 * SIDE EFFECTS:
 *
 *	the inode slot is poisoned & the file space freed.
-*/

int rd_delete (struct INODE *ip) {

    if (ip->ref == 0) {
	/*
	 * Release the storage associated with the file.
	 */
	if ((ip->attr & ATTR$M_NOFREE) == 0)
	    dyn$_free (ip->loc, DYN$K_SYNC);

	freeinode( ip);
    } 
    else {
        ip->inuse = 0;           /* mark file for pending delete */
        INODE_UNLOCK (ip);
    }

    return msg_success;
}

/* The ram disk is never stacked on anything, so it never recognizes
 * such paths as valid ram disks.
 */
int rd_validate (char *path) {
	return 0;
}

/*+
 * ============================================================================
 * = rd_init - enter the RAM disk driver functions into the driver database   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Initialize the RAM disk driver & create the builtin (executable) shell files
 *  
 * FORM OF CALL:
 *  
 *	rd_init();
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * SIDE EFFECTS:
 *
 *	makes the shell commands available for execution and allocates an
 *      inode for each of these commands.
-*/

int rd_init () {
	struct INODE	*ip;
	int i;
	int *lp;
	extern int	num_encap;

#if !MODULAR
	/*
	 * Enter the built in files into the file system.
	 */
	install_builtin_commands ();

	/*
	 * Install the encapsulated files
	 */
	for (i=0; i<num_encap; i++) {
	    allocinode (encap_files [i].name, 1, &ip);
	    ip->dva = & rd_ddb;
	    strcpy (ip->name, encap_files [i].name);
	    ip->attr = encap_files [i].attributes;
	    lp = encap_files [i].address;
	    ip->len[0] = *lp++;
	    ip->loc = lp;
	    ip->alloc[0] = ip->len[0];
	    INODE_UNLOCK (ip);
	}
#endif
	return msg_success;
}
#if MODULAR
/*
 * Install the files
 */
int install_builtin_files(int count, struct encap_s_files *encap)
{
    int i;
    int *lp;
    struct INODE *ip;

    if (count == 0)
	return msg_success;

    for (i=0; i<count; i++) {
	allocinode (encap[i].name, 1, &ip);
	ip->dva = & rd_ddb;
	strcpy (ip->name, encap[i].name);
	ip->attr = encap[i].attributes + ATTR$M_NOFREE;
	lp = encap[i].address;
	ip->len[0] = *lp++;
	ip->loc = lp;
	ip->alloc[0] = ip->len[0];
	INODE_UNLOCK (ip);
    }
    return msg_success;
}
int remove_builtin_files(int count, struct encap_s_files *encap)
{
    int i;
    struct INODE *ip;

    if (count == 0)
	return msg_success;

    for (i=0; i<count; i++) {
	remove(encap[i].name);
    }
    return msg_success;
}
#endif

