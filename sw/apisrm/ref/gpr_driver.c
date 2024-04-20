/* file:	gpr_driver.c
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
 *
 *
 * Abstract:	General Purpose Register driver for examine/deposit.
 *
 * Author:	JDS - Jim Sawin
 *
 * Modifications:
 *
 */

#include "cp$src:platform.h"
#include    "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include    "cp$src:prdef.h"
#include    "cp$src:kernel_def.h"
#include    "cp$src:msg_def.h"
#include    "cp$src:exdep_def.h"
#include    "cp$src:alphaps_def.h"
#include    "cp$src:alpha_def.h"
#include    "cp$src:pal_def.h"
#include    "cp$src:impure_def.h"

#define DEBUG 0

/*
 * Certain gprs are located elsewhere in the saved context (such as the PC)
 * yet are presented as being a member of the regular array of
 * GPRs.  We have to trap out access to these gprs.  Since the drivers deliver
 * a byte stream interface, and the read/write routines allow for any size
 * transfer on any boundary, we have to know the byte ranges of the registers
 * that have to be trapped.
 */
#define	RAZ_LOWER	(RAZ_GPR * sizeof (REGDATA))
#define RAZ_UPPER	(RAZ_LOWER + sizeof (REGDATA) - 1)

int gpr_read ();
int gpr_write ();
int gpr_open ();
int null_procedure ();

struct DDB gpr_ddb = {
	"gpr",			/* how this routine wants to be called	*/
	gpr_read,		/* read routine				*/
	gpr_write,		/* write routine			*/
	gpr_open,		/* open routine				*/
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
 * = gpr_open - open the gpr driver                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Open the gpr driver for access.  This open will open the saved context
 * area on the processor it is currently running on, and then remember the
 * address so that if the process migrates, a consistent view of the gprs
 * is obtained.
 *
 * The info field is interpreted as a hex offset into the gpr area.  Like all
 * drivers, this driver exports a byte stream model even though underneath it
 * gprs are really not byte oriented.   This means that if you want
 * to access gpr 13, for example, the following code sequence
 * would be required:
 *		fp = fopen ("gpr", "r");
 *		fseek (fp, 13, SEEK_SET)
 *		fread (fp, 8, 1, buffer)
 *
 * Use the count file in the FILE structure to keep track of the processor
 * we're on, and the misc field for the base of the register save area.
 *
 *  
 * FORM OF CALL:
 *  
 *	gpr_open (fp, info, next, mode)
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *	char *info	- information that follows the colon.  Not allowed
 *			for the gpr driver.
 *	char *next	- ignored, and should be null (gpr driver can't be
 *			stacked)
 *	char *mode	- ignored.
-*/
int gpr_open (struct FILE *fp, char *info, char *next, char *mode) {
	struct impure *paltempcontext;

	fp->count = whoami ();

	/*
	 * Treat the info field as a hex offset into FPR area
         * Use common_convert() to handle 64 bit offsets.
	 */
	common_convert( info, 16, fp->offset, sizeof( fp->local_offset ) );

	/* Complain if we're eligible for running on more than one
	 * processor.
	 */
	unicheck ("gpr_open");

	/*
	 * Remember where gprs are stored in the impure area for this
	 * processor.  Once remembered, we'll always access the gprs for
	 * this processor even if we migrate.
	 */
	paltempcontext = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
	    fp->count * PAL$IMPURE_SPECIFIC_SIZE;
	fp->misc = (unsigned) &paltempcontext->cns$gpr[0][0];
	return msg_success;
}

/*+
 * ============================================================================
 * = gpr_read - read the general purpose register array                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine reads the value of a GPR, as saved in the entry context.
 *	The address is stored in the FILE structure, and the data is written to
 *	the specified buffer.
 *
 * 	This routine traps access to the PC and the stack pointer(s).  In order
 *	to trap these location correctly, the driver requires that the offset
 *	be pointing to one of these registers exactly.  Block reads/writes that
 *	span these registers will not be trapped.
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should correspond to the GPR device.
 *	int size	size of item in bytes
 *	int number 	number of items to read from a GPR.
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
int gpr_read (struct FILE *fp, int size, int number, char *buf) {
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
 * = gpr_write - read a block of data from a specified file                     =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine writes a GPR value to the saved entry context, which is
 * restored on any exit from console (such as START or CONTINUE).  The
 * address is stored in the FILE structure, and the data is read from the
 * specified buffer.
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should correspond to the GPR device.
 *	int size	size of item in bytes
 *	int number 	number of items to read from a GPR.
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
int gpr_write (struct FILE *fp, int size, int number, char *buf) {
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
 * = gpr_init - initialize the gpr driver                                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine initializes the GPR driver entry in the global DDB
 *	(Driver DataBase) and creates an associated INODE so that the file
 *	system recognizes the GPR device (General Purpose Registers).
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
 *      Modifies the Driver DataBase and creates an INODE for the GPR device.
 *
 *--
 */
int gpr_init (void) {
    struct INODE *ip;			/* Pointer to INODE for this driver */

/* Create an inode entry, thus making the device visible as a file: */

    allocinode (gpr_ddb.name, 1, &ip);
    ip->dva = & gpr_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;	/* Read and writes allowed */
    ip->loc = 0;
    ip->len[0] = MAX_GPR+1;	/* total number of bytes written */
    ip->bs = sizeof (REGDATA);

    INODE_UNLOCK (ip);

    return msg_success;
}
