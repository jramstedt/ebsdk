/* file:	pt_driver.c
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 *
 *
 * Abstract:	Alpha PALtemp Register driver.  This is a "raw" interface in
 *		that it has no knowledge of how the pal temps are used, or
 *		what information stored in paltemps must be kept consistent
 *		with other cells.
 *
 * Author:	JDS - Jim Sawin
 *
 * Modifications:
 *
 *  5	by ajb 13-Dec-1991
 *	Make it reentrant, remove ldqp, make it conform to standard driver
 *	interface.
 *
 *  4	by ajb 25-Sep-1991
 *	return 0 on errors for fread, fwrite
 *
 *  3	by JDS, Mon Apr  8 10:15:52 1991.
 *	Filled in validate routine.
 *
 *  2	by JDS, Wed Apr  3 16:09:22 1991.
 *	Removed SEXT macro (now in common.h).
 * 
 *  1	by JDS, Fri Mar 22 16:19:54 1991.
 *	Initial entry.
 */

#include "cp$src:platform.h"
#include    "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include    "cp$src:kernel_def.h"
#include    "cp$src:msg_def.h"
#include    "cp$src:exdep_def.h"
#include    "cp$src:alpha_def.h"
#include    "cp$src:pal_def.h"
#include    "cp$src:impure_def.h"

#define DEBUG 0

int pt_read ();
int pt_write ();
int pt_open ();
int null_procedure ();

struct DDB pt_ddb = {
	"pt",			/* how this routine wants to be called	*/
	pt_read,		/* read routine				*/
	pt_write,		/* write routine			*/
	pt_open,		/* open routine				*/
	null_procedure,		/* close routine			*/
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
 * = pt_open - open the pal temp driver                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Open the pal temp driver for access.  This open will open the pal impure
 * area on the processor it is currently running on, and then remember the
 * address so that if the process migrates, a consistent view of pal is
 * obtained.
 *
 * The info field is interpreted as a hex offset into the pal temp area. Like
 * all drivers, this driver exports a byte stream model even though underneath
 * it paltemps are really not byte oriented.  This means that if you want
 * to access paltemp register 28, for example, the following code sequence
 * would be required:
 *		fp = fopen ("pt", "r");
 *		fseek (pt, 28, SEEK_SET)
 *		fread (fp, 8, 1, buffer)
 *  
 * FORM OF CALL:
 *  
 *	pt_open (fp, info, next, mode)
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 *	msg_fatal - offset out of range on open
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *	char *info	- information that follows the colon.  In the case of
 *			the pal temp driver, this is an offset.
 *	char *next	- ignored, and should be null (pal temp driver can't be
 *			stacked)
 *	char *mode	- ignored.
-*/
int pt_open (struct FILE *fp, char *info, char *next, char *mode) {
	struct impure *paltempcontext;

	fp->offset = &fp->local_offset;
	*fp->offset = 0;

	/*
	 * Treat the info field as a hex offset into pal temp area.
         * Use common_convert() to handle 64 bit offsets.
	 */
	common_convert( info, 16, fp->offset, sizeof( fp->local_offset ) );

	/* complain if we're eligible on more than one
	 * processor.
	 */
	unicheck ("pt_driver");

	/*
	 * Remember where pal temps are stored in the impure area for this
	 * processor.  Once remembered, we'll always access the paltemps for
	 * this processor even if we migrate.
	 */
	paltempcontext = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
	    whoami() * PAL$IMPURE_SPECIFIC_SIZE
	;
	fp->misc = (unsigned) &paltempcontext->cns$pt[0][0];

	return msg_success;
}

/*
 *+
 * ============================================================================
 * = pt_read - read saved copies of pal temps                                 =
 * ============================================================================
 *
 *  FUNCTIONAL DESCRIPTION:
 *
 * 	This routine  reads  the  value  of a PALtemp, as saved in the entry
 *	context.  The address is stored in the FILE structure, and the data
 *	is written to the specified buffer.
 * 	
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should correspond to the PALtemp device.
 *	int size	size of item in bytes
 *	int number 	number of items to read from a PALtemp.
 *	char *buf	Address to write the data to.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes read, or 0 on error.
 *
 *
 *  SIDE EFFECTS:
 *
 *      NONE
 *
 *--
 */
int pt_read (struct FILE *fp, int size, int number, char *buf) {
	int	xfer_length;
	struct INODE *ip;
	char	*regadr;

/* Serialize access */

	ip = fp->ip;
	INODE_LOCK (ip);
	fp->status = msg_success;

/* figure out how much data can be moved and then move it */

	if ((fp->offset[1] != 0) ||
		((unsigned) fp->offset[0] >= (unsigned) ip->len[0])) {
	    xfer_length = 0;
	    fp->status = msg_exdep_badadr;
	} else {
 	    xfer_length = min ((size*number), (ip->len[0] - *fp->offset) * sizeof (REGDATA));
	}

	regadr = *fp->offset * sizeof (REGDATA);

	memcpy (buf, fp->misc + regadr, xfer_length);
	*fp->offset += 1;

/* Release access */

	INODE_UNLOCK (fp->ip);

	return xfer_length;
}

/*
 *+
 * ============================================================================
 * = pt_write - write data to saved pal context                               =
 * ============================================================================
 *
 *  FUNCTIONAL DESCRIPTION:
 *
 * 	This routine  writes data to save pal context.
 * 	
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should correspond to the PALtemp device.
 *	int size	size of item in bytes
 *	int number 	number of items to read from a PALtemp.
 *	char *buf	Address to write the data to.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes read, or 0 on error.
 *
 *
 *  SIDE EFFECTS:
 *
 *      NONE
 *
 *--
 */
int pt_write (struct FILE *fp, int size, int number, char *buf) {
	int	xfer_length;
	struct INODE *ip;
	char	*regadr;

/* Serialize access */

	ip = fp->ip;
	INODE_LOCK (ip);

/* figure out how much data can be moved and then move it */

	if ((fp->offset[1] != 0) ||
		((unsigned) fp->offset[0] >= (unsigned) ip->len[0])) {
	    xfer_length = 0;
	    fp->status = msg_exdep_badadr;
	} else {
 	    xfer_length = min ((size*number), (ip->len[0] - *fp->offset) * sizeof (REGDATA));
	}

	regadr = *fp->offset * sizeof (REGDATA);

	memcpy (fp->misc + regadr, buf, xfer_length);
	*fp->offset += 1;

/* Release access */

	INODE_UNLOCK (fp->ip);

	return xfer_length;
}

/*
 *++
 *  FUNCTIONAL DESCRIPTION:
 *
 *      pt_init
 * 	This routine initializes the PALtemp driver entry in the global DDB
 *	(Driver DataBase) and creates an associated INODE so that the file
 *	system recognizes the PALtemp device (PALtemp Registers) for
 * 	examines and deposits.
 *
 *  FORMAL PARAMETERS:
 *
 *      NONE
 *
 *  RETURN VALUE:
 *
 *	Always returns msg_success.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *      Modifies the Driver DataBase and creates an INODE for
 *	the PALtemp device.
 *
 *--
 */
int pt_init (void) {
	struct INODE *ip;	/* Pointer to INODE for this driver */
	char *name;
	int	i;


	allocinode (pt_ddb.name, 1, &ip);
	ip->dva = & pt_ddb;
	ip->attr = ATTR$M_READ | ATTR$M_WRITE;
	ip->loc = 0;
	ip->len[0] = MAX_PT + 1;
	ip->misc = 0;
	ip->bs = sizeof (REGDATA);
	INODE_UNLOCK (ip);
 
	return msg_success;
}
