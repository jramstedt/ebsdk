/* file:	fpr_driver.c
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
 * Abstract:	Alpha Floating Point Register driver for examine/deposit.
 *
 * Author:	JDS - Jim Sawin
 *
 */

#include "cp$src:platform.h"
#include    "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include    "cp$src:kernel_def.h"
#include    "cp$src:prdef.h"
#include    "cp$src:msg_def.h"
#include    "cp$src:exdep_def.h"
#include    "cp$src:alpha_def.h"
#include    "cp$src:impure_def.h"
#include    "cp$src:pal_def.h"

#define DEBUG 0


/* External references */

#define	RAZ_LOWER	(RAZ_FPR * sizeof (REGDATA))
#define RAZ_UPPER	(RAZ_LOWER + sizeof (REGDATA) - 1)

int fpr_read ();
int fpr_write ();
int fpr_open ();
int null_procedure ();

struct DDB fpr_ddb = {
	"fpr",			/* how this routine wants to be called	*/
	fpr_read,		/* read routine				*/
	fpr_write,		/* write routine			*/
	fpr_open,		/* open routine				*/
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
 * = fpr_open - open the fpr driver                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Open the fpr driver for access.  This open will open the saved context
 * area on the processor it is currently running on, and then remember the
 * address so that if the process migrates, a consistent view of the fprs
 * is obtained.
 *
 * The info field is interpreted as a hex offset into the fpr area.  Like all
 * drivers, this driver exports a byte stream model even though underneath it
 * fprs are really not byte oriented.   This means that if you want
 * to access fpr 13, for example, the following code sequence
 * would be required:
 *		fp = fopen ("fpr", "r");
 *		fseek (fp, 13, SEEK_SET)
 *		fread (fp, 8, 1, buffer)
 *
 * Use the count file in the FILE structure to keep track of the processor
 * we're on, and the misc field for the base of the register save area.
 *
 *  
 * FORM OF CALL:
 *  
 *	fpr_open (fp, info, next, mode)
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *	char *info	- information that follows the colon.  In the case of
 *			the fpr driver, this is an offset.
 *	char *next	- ignored, and should be null (fpr driver can't be
 *			stacked)
 *	char *mode	- ignored.
-*/
int fpr_open (struct FILE *fp, char *info, char *next, char *mode) {
	struct impure *paltempcontext;

	fp->offset = &fp->local_offset;
	*fp->offset = 0;
	fp->count = whoami ();

	/*
	 * Treat the info field as a hex offset into FPR area
         * Use common_convert() to handle 64 bit offsets.
	 */
	common_convert( info, 16, fp->offset, sizeof( fp->local_offset ) );

	/* complain if we're eligible on more than one
	 * processor.
	 */
	unicheck ("ipr_dirver");

	/*
	 * Remember where fprs are stored in the impure area for this
	 * processor.  Once remembered, we'll always access the fprs for
	 * this processor even if we migrate.
	 */
	paltempcontext = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
	    fp->count * PAL$IMPURE_SPECIFIC_SIZE
	;
	fp->misc = (unsigned) &paltempcontext->cns$fpr[0][0];

	return msg_success;
}

/*+
 * ============================================================================
 * = fpr_read - read the floating point register array                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine reads the value of an FPR, as saved in the entry context.
 *	The address is stored in the FILE structure, and the data is written to
 *	the specified buffer.
 *
 * 	This routine traps access to F31, which is always 0.
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should correspond to the FPR device.
 *	int size	size of item in bytes
 *	int number 	number of items to read from an FPR.
 *	char *buf	Address to write the data to.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes read, or 0 on error.
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
 *      NONE
 *
 *--
 */
int fpr_read (struct FILE *fp, int size, int number, char *buf) {
	int	i;
	int	xfer_length;
	struct INODE *ip;
	int	offset;
	char	*regadr;

	/* Serialize access */
	ip = fp->ip;
	INODE_LOCK (ip);
	fp->status = msg_success;

	if ((fp->offset[1] != 0) ||
		((unsigned) fp->offset[0] >= (unsigned) ip->len[0])) {
	    xfer_length = 0;
	    fp->status = msg_exdep_badadr;
	} else {
 	    xfer_length = min ((size*number), (ip->len[0] - *fp->offset) * sizeof (REGDATA));
	}

	regadr = *fp->offset * sizeof (REGDATA);

	/*
	 * Transfer the buffer byte by byte so that we can trap
	 * registers that are located elsewhere.
	 */
	for (i=0; i<xfer_length; i++) {
	    /* RAZ trap */
	    if (inrange (regadr, RAZ_LOWER, RAZ_UPPER+1)) {
		*buf++ = 0;

	    /* not trapped */
	    } else {
		*buf++ = *(char *) (fp->misc + regadr++);
	    }
	}
	*fp->offset += 1;

	/* Release access */
	INODE_UNLOCK (fp->ip);
	return i; /* bytes transfered */
}


/*+
 * ============================================================================
 * = fpr_write - write the floating point register array                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine writes a FPR value to the saved entry context, which is
 *	restored on any exit from console (such as START or CONTINUE).  The
 *	address is stored in the FILE structure, and the data is read from the
 *	specified buffer.
 *
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should correspond to the FPR device.
 *	int size	size of item in bytes
 *	int number 	number of items to read from a FPR.
 *	char *buf	Address to read the data from.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes written, or 0 on error.
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
 *      NONE
 *
 *--
 */
int fpr_write (struct FILE *fp, int size, int number, char *buf) {
	int	i;
	int	xfer_length;
	struct INODE *ip;
	int	offset;
	char	*regadr;

	/* Serialize access */
	ip = fp->ip;
	INODE_LOCK (ip);
	fp->status = msg_success;

	if ((fp->offset[1] != 0) ||
		((unsigned) fp->offset[0] >= (unsigned) ip->len[0])) {
	    xfer_length = 0;
	    fp->status = msg_exdep_badadr;
	} else {
 	    xfer_length = min ((size*number), (ip->len[0] - *fp->offset) * sizeof (REGDATA));
	}

	regadr = *fp->offset * sizeof (REGDATA);

	/*
	 * Transfer the buffer byte by byte so that we can trap
	 * registers that are located elsewhere.
	 */
	for (i=0; i<xfer_length; i++) {

	    /* RAZ trap */
	    if (inrange (regadr, RAZ_LOWER, RAZ_UPPER+1)) {
		;

	    /* not trapped */
	    } else {
		*(char *) (fp->misc + regadr++) = *buf++;
	    }
	}
	*fp->offset += 1;

	/* Release access */
	INODE_UNLOCK (fp->ip);
	return i; /* bytes transfered */
}

/*+
 * ============================================================================
 * = fpr_init - init the fpr driver                                           =
 * ============================================================================
 *
 *	This routine initializes the FPR driver entry in the global DDB
 *	(Driver DataBase) and creates an associated INODE so that the file
 *	system recognizes the FPR device (Floating Point Registers) for
 *	examines and deposits.
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
 *      Modifies the Driver DataBase and creates an INODE for the FPR device.
 *
 *--
 */
int fpr_init (void) {
    struct INODE *ip;


/* Create an inode entry, thus making the device visible as a file: */

    allocinode (fpr_ddb.name, 1, &ip);
    ip->dva = & fpr_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;
    ip->loc = 0;
    ip->len[0] = (MAX_FPR + 1);
    ip->bs = sizeof (REGDATA);

    INODE_UNLOCK (ip);

    return msg_success;
}
