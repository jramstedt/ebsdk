/* file:	ps_driver.c
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
 * Abstract:	Processor Status Longword/Program Counter driver for examine/deposit.
 *
 * Author:	JDS - Jim Sawin
 *
 * Modifications:
 *
 *  12	by JDS, Thu Apr  4 10:20:09 1991
 *	Added code to clean up Alpha pc before returning.  The pc in the impure area encodes additional
 * information in the 2 low-order bits.  This is now masked out before returning the pc.
 *
 *  11	by JDS, Wed Apr  3 16:08:48 1991
 *	Removed SEXT macro (now in common.h).
 *	
 *  10	by JDS, Wed Mar  6 13:38:33 1991
 *	Added Alpha PC read/write support.
 *
 *	sfs	19-Feb-1991	Fix call to ALLOCINODE.
 *
 *  8   by ajb 04-Feb-1991
 *	change calling sequence to xx_read, xx_write
 *
 *  7	by JDS, Wed Dec 12 09:57:46 1990
 *	Disabled NYIs by default.
 *
 *  6	by jds, Fri Nov 30 15:20:41 1990
 *	Filled in ps_read/ps_write routines.
 *
 *  5   by pel, 14-Nov-1990
 *      Add new arguments to allocinode call.
 *
 *  4	by ajb, 13-Nov-1990
 *	fix declarations for compilation under gemcc
 *
 *  3	by JDS, Wed Nov  7 16:01:50 1990
 *	Changed to a real driver (but read/write/validate are still NYI).
 *
 *  2	by JDS, Fri Sep 14 17:04:53 1990
 *	Enhanced portability by changing the routine interface.
 *
 *  1	by JDS, Thu Sep 13 09:08:07 1990
 *	Initial entry.
 */

#include "cp$src:platform.h"
#include    "cp$src:kernel_def.h"
#include    "cp$src:prdef.h"
#include    "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include    "cp$src:msg_def.h"
#include    "cp$src:exdep_def.h"
#include    "cp$src:alpha_def.h"
#include    "cp$src:pal_def.h"
#include    "cp$src:impure_def.h"

#define DEBUG 0


/* External references */

extern int null_procedure();					/* Null procedure */

/* Function declarations: */

int ps_init(void);						/* DDB initialization */
int ps_read (struct FILE *fp, int size, int number, char *buf);	/* PS read routine (examine) */
int ps_write (struct FILE *fp, int size, int number, char *buf);/* PS write routine (deposit) */

struct DDB psr_ddb = {
	"psr",			/* how this routine wants to be called	*/
	ps_read,		/* read routine				*/
	ps_write,		/* write routine			*/
	null_procedure,		/* open routine				*/
	null_procedure,		/* close routine			*/
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



/*
 *++
 *  FUNCTIONAL DESCRIPTION:
 *
 *      ps_init
 *	This routine initializes the PS driver entry in the global DDB (Driver DataBase) and creates an
 * associated INODE so that the file system recognizes the PS device (Processor Status Register) for examines
 * and deposits.
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
 *      Modifies the Driver DataBase and creates an INODE for the PS device.
 *
 *--
 */
int ps_init (void)
{
    struct INODE *ip;						/* Pointer to INODE for this driver */


/* Create an inode entry, thus making the device visible as a file: */

    allocinode (psr_ddb.name, 1, &ip);
    ip->dva = & psr_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;			/* Read and writes allowed */
    ip->loc = 0;
    ip->bs = sizeof (REGDATA);

    INODE_UNLOCK (ip);

    return msg_success;
}

/*
 *++
 *  FUNCTIONAL DESCRIPTION:
 *
 *      ps_read
 *	This routine reads the VAX PSL or Alpha PC from the saved entry context.  The address is stored in the
 * FILE structure, and the data is written to the specified buffer.
 *
 * NOTE: whoami() is used to index into the saved entry context; therefore, the current process must be running on
 * the processor whose PSL/PC is desired in a multiprocessor system.
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should correspond to the PS device.
 *	int size	- size of item
 *	int number	- number of items to read
 *	char *buf	Address to write the data to.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes read (size of PSL/PC).
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
int ps_read (struct FILE *fp, int size, int number, char *buf)
{
    struct impure *paltempcontext;				/* Physical pointer to PALtemp saved context */

    fp->status = msg_success;					/* Default to success */
    if (*fp->offset != 0)
	return 0;

    paltempcontext = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
	whoami() * PAL$IMPURE_SPECIFIC_SIZE;			/* Calculate address of PALtemp area */

    *(__int64 *)buf = *(__int64 *)&paltempcontext->cns$exc_addr;
    *buf &= ~2;

    *fp->offset = sizeof(REGDATA);
    return sizeof(REGDATA);					/* Return with length of PSR */
}

/*
 *++
 *  FUNCTIONAL DESCRIPTION:
 *
 *      ps_write
 *	This routine writes to the VAX PSL or Alpha PC in the saved entry context, which is restored on any exit
 * from console (such as START or CONTINUE).  The address is stored in the FILE structure, and the data is read
 * from the specified buffer.
 *
 * NOTE: whoami() is used to index into the saved entry context; therefore, the current process must be running on
 * the processor whose PSL/PC is targeted in a multiprocessor system.
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should correspond to the PS device.
 *	int size	- size of item
 *	int number	- number of items to read
 *	char *buf	Address to read the data from.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes written (size of PSL/PC).
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
int ps_write (struct FILE *fp, int size, int number, char *buf)
{
    struct impure *paltempcontext;				/* Physical pointer to PALtemp saved context */

    fp->status = msg_success;					/* Default to success */
    if (*fp->offset != 0)
	return 0;

    paltempcontext = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
	whoami() * PAL$IMPURE_SPECIFIC_SIZE;			/* Calculate address of PALtemp area */

    *buf &= ~2;
    *(__int64 *)&paltempcontext->cns$exc_addr = *(__int64 *)buf;

    *fp->offset = sizeof(REGDATA);
    return sizeof(REGDATA);					/* Return with length of PSR */
}
