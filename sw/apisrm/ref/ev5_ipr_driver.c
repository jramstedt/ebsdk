/* file:	ev5_ipr_driver.c
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
 * tansferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 *
 *
 * Abstract:	Internal Processor Register driver for examine/deposit.
 *
 * Author:	David Mayo
 *
 * Created:
 *
 *	dtm	27-Oct-1993	Initial, derived from ipr_driver.c (EV4).
 */

#include    "cp$src:platform.h"
#include    "cp$src:prdef.h"
#include    "cp$src:stddef.h"
#include    "cp$src:kernel_def.h"
#include    "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include    "cp$src:msg_def.h"
#include    "cp$src:exdep_def.h"
#include    "cp$src:alpha_def.h"
#include    "cp$src:alphaps_def.h"
#include    "cp$src:aprdef.h"
#include    "cp$src:pal_def.h"
#include    "cp$src:impure_def.h"
#include    "cp$src:platform_cpu.h"

#define DEBUG 0

/* External references */

extern int exdep_rsvdop_handler();		/* Reserved-operand exception handler */

#define HIRR$M_SIRR ((1<<HIRR$S_SIRR)-1)
#define HIRR$M_ASTRR ((1<<HIRR$S_ASTRR)-1)
#define HIER$M_ASTER ((1<<HIER$S_ASTER)-1)

int ipr_read ();
int ipr_write ();
int ipr_open ();
int ipr_close ();
int null_procedure ();

struct DDB ipr_ddb = {
	"ipr",			/* how this routine wants to be called	*/
	ipr_read,		/* read routine				*/
	ipr_write,		/* write routine			*/
	ipr_open,		/* open routine				*/
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
 * = ipr_open - open the ipr driver                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	    This routine opens ipr by establishing an exception handler for
 *	the process.  Since iprs are processor specific, and some of them
 *	reference real hardware, this driver is very sensitive to procesor
 *	affinity.  With an affinity set to more than one cpu, two successive
 *	reads are not likely to reference the same cpu, yet they will both
 *	merrily return data.
 *
 *	To catch this insidious situation without a lot of synchronization
 *	overhead, this routine complains if the affinity and active set
 *	indicates there are more than one cpu to run on.
 *
 *
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	    - Ignored.
 *	char *info	    - ipr number to reference
 *	char *next	    - Ignored.
 *	char *mode	    - Ignored.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE.
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE.
 *
 *  FUNCTION VALUE:
 *
 *      msg_success
 *
 *  SIDE EFFECTS:
 *
 *
 *--
 */
int ipr_open (struct FILE *fp, char *info, char *next, char *mode) {

	/*
	 * Complain if we're eligible to run on more than one
	 * processor.
	 */
	unicheck ("ipr_driver");

	/*
	 * Use the explicit offset if present.  We complain about
	 * bad offsets until read/write time.  Treat the offset
	 * as a register number.  A null info field results in 
	 * setting the offset to 0.
	 */
	fp->offset = &fp->local_offset;
	*fp->offset = xtoi (info);

	return msg_success;
}

/*+
 * ============================================================================
 * = ipr_read - read from the ipr driver                                      =
 * ============================================================================
 *
 * OVERVIEW:
 * 
 *	This routine reads an IPR.  The address is stored in the FILE
 * structure, and the data is written to the specified buffer.  If the IPR
 * is stored in the saved entry context, then the value is read from there.
 * Otherwise, the value is read directly from the IPR.
 *
 * If a reserved operand exception occurs in an attempt to read directly
 * from an IPR, then the handler set up by the ipr_open routine will
 * longjmp back to ipr_read, which then loads an error status into the
 * file structure and returns 0 to indicate the error.
 *
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	- Pointer to FILE structure.  This should
 *			correspond to the IPR device.
 *	int size	- size of items to read
 *	int len		- number of items to read (ignored).
 *	char *buf	Address to write the data to.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes read.
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
int ipr_read (struct FILE *fp, int size, int len, char *buf) {
    int ipr_num;
    int stat;
    struct impure *paltempcontext;
    int xfer_length;
    __int64 data;	/* aligned quadword data */
    __int64 src;
    __int64 temp;
    __int64 psdata;
    struct PS *ps;

    /* get the ipr number and advance */
    ipr_num = *fp->offset;
    *fp->offset += 1;
    fp->status = msg_success;


	/*
	 * Set up some generic pointers and information
 	 */
	paltempcontext = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
	    whoami() * PAL$IMPURE_SPECIFIC_SIZE;
	psdata = 0;
	ps = &psdata;
	ps->ipl = *(__int64 *)&paltempcontext->cns$ipl;
	temp = *(__int64 *)&paltempcontext->cns$ps;
	ps->cm = ((struct PS *)&temp)->cm;
	read_pt (PT$_PS, &temp);
	ps->sw = ((struct PS *)&temp)->sw;

	/*
	 * dispatch on the ipr number
	 */
	switch (ipr_num) {
	case APR$K_ASN:			
	    mfpr64 (&data, APR$K_ASN);
	    break;

	case APR$K_ASTEN:		
	    data = (*(__int64 *)&paltempcontext->cns$aster) & 0xf;
	    break;

	case APR$K_ASTSR:		
	    data = (*(__int64 *)&paltempcontext->cns$astrr) & 0xf;
	    break;

	case APR$K_FEN:			
	    data = ((*(__int64 *)&paltempcontext->cns$icsr) >> 26) & 1;
	    break;

	case APR$K_IPL:
	    data = *(__int64 *)&paltempcontext->cns$ipl;
	    break;

	case APR$K_MCES:		
	    read_pt (PT$_MCES, &data);
	    data = data & 31;
	    break;

	case APR$K_PCBB:
	    read_pt (PT$_PCBB, &data);
	    break;

	case APR$K_PRBR:
	    read_pt (PT$_PRBR, &data);
	    break;

	case APR$K_PTBR:
	    read_pt (PT$_PTBR, &data);
	    data >>= HWRPB$_PAGEBITS;
	    break;

	case APR$K_SCBB:
	    read_pt(PT$_SCBB, &data);
	    data >>= HWRPB$_PAGEBITS;
	    break;

	case APR$K_SISR:
	    data = ((*(__int64 *)&paltempcontext->cns$sirr) >> 3) & 0xfffe;
	    break;

	/*
	 * when reading stack pointers, we have to look at the
	 * current mode.  If the mode implied by the stack pointer
	 * and the saved mode are the same, the PCBB is not valid
	 * and the saved sp must be used.  Otherwise, we use the
	 * sp in the PCBB.
	 */
	case APR$K_KSP:
	case APR$K_ESP:
	case APR$K_SSP:
	case APR$K_USP:
	    if (ps->cm == (ipr_num - APR$K_KSP)) {
		read_gpr (30, &data);
	    } else {
		read_pt (PT$_PCBB, &src);
		src += (ipr_num - APR$K_KSP) * sizeof (REGDATA);
		ldqp (&src, &data);
	    }
	    break;

	case APR$K_VPTB:
	    read_pt (PT$_L3VIRT, &data);
	    break;	    

	case APR$K_WHAMI:
	    read_pt (PT$_WHAMI, &data);
	    data = (data >> 8) & 0xff;
	    break;

	case APR$K_PS:
	    data = psdata;
	    break;

	case APR$K_ICSR:
	    data = *(__int64 *)&paltempcontext->cns$icsr;
	    break;

	case APR$K_MCSR:
	    data = *(__int64 *)&paltempcontext->cns$mcsr;
	    break;

	/* out of range */
	default:
	    fp->status = msg_exdep_badadr;
	    return 0;
	    break;
	}

	/*
	 * block copy the data to the user's buffer without
	 * exceeding it.
	 */
	xfer_length = umin(sizeof (REGDATA), size * len);
	memcpy (buf, &data, xfer_length);

	return xfer_length;
}

/*+
 * ============================================================================
 * = ipr_write - write an ipr                                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine writes an IPR.  The address is stored in the FILE
 * structure, and the data is read from the specified buffer.  If the IPR
 * is stored in the saved entry context, then the value is written there.
 * Otherwise, the value is written directly to the IPR.
 *
 * If a reserved operand exception occurs in an attempt to write directly
 * to an IPR, then the handler set up by the ipr_open routine will longjmp
 * back to ipr_write, which then loads an error status into the file structure
 * and returns 0 to indicate the error.
 *
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp -Pointer to FILE structure.  This should correspond
 *			 to the IPR device.
 *	int size	- size of items to write
 *	int len		- number of items to write (ignored).
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
int ipr_write (struct FILE *fp, int size, int len, char *buf) {
    int ipr_num;
    int stat;
    struct impure *paltempcontext;
    int xfer_length;
    __int64 data;
    __int64 src;
    __int64 temp;
    __int64 temp1;
    __int64 psdata;
    struct PS *ps;

    /*
     * fetch the ipr number and advance
     */
    ipr_num = *fp->offset;
    *fp->offset += 1;
    fp->status = msg_success;

    /*
     * Copy the data into an aligned local buffer and
     * zero extend it.
     */
    memset (&data, 0, sizeof (data));
    xfer_length = umin (sizeof (REGDATA), size * len);
    memcpy (&data, buf, xfer_length);


	/*
	 * Set up some generic pointers
 	 */
	paltempcontext = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
	    whoami () * PAL$IMPURE_SPECIFIC_SIZE;
	psdata = 0;
	ps = &psdata;
	temp = *(__int64 *)&paltempcontext->cns$ipl;
	ps->ipl = temp;
	temp = *(__int64 *)&paltempcontext->cns$ps;
	ps->cm = ((struct PS *)&temp)->cm;
	read_pt (PT$_PS, &temp);
	ps->sw = ((struct PS *)&temp)->sw;


	/*
	 * dispatch on the ipr number
	 */
	switch (ipr_num) {

	case APR$K_ASTEN:
	    temp = *(__int64 *)&paltempcontext->cns$aster;
	    temp = ((temp & data) & 0xf) | (data >> 4);
	    *(__int64 *)&paltempcontext->cns$aster = temp;
	    break;

	case APR$K_ASTSR:
	    temp = *(__int64 *)&paltempcontext->cns$astrr;
	    temp = ((temp & data) & 0xf) | (data >> 4);
	    *(__int64 *)&paltempcontext->cns$astrr = temp;
	    break;

	case APR$K_FEN:
	    temp = *(__int64 *)&paltempcontext->cns$icsr;
	    /*
	     *  Below is a kludge to accommodate the fact that the compiler
	     *  does not deal with constants which don't fit in 32 bits.
	     */
	    src = 1;
	    temp &= ~(src << 26);
	    temp |= (data & 1) << 26;
	    *(__int64 *)&paltempcontext->cns$icsr = temp;
	    break;

	case APR$K_IPIR:
	    mtpr64 (&data, APR$K_IPIR);
	    break;

	case APR$K_IPL:
	    data &= 0x1f;
	    *(__int64 *)&paltempcontext->cns$ipl = data;
	    break;

	case APR$K_MCES:
	    read_pt (PT$_MCES, &temp);
	    temp &= ~(data & 7 | 0x18);
	    temp |= (data & 0x18);
	    write_pt (PT$_MCES, &temp);
	    break;

	case APR$K_PCBB:
	    write_pt (PT$_PCBB, &data);
	    break;

	case APR$K_PRBR:
	    write_pt (PT$_PRBR, &data);
	    break;
	  
	case APR$K_PTBR :
	    data <<= HWRPB$_PAGEBITS;
	    write_pt (PT$_PTBR, &data);
	    break;

	case APR$K_SCBB:
	    data <<= HWRPB$_PAGEBITS;
	    write_pt (PT$_SCBB, &data);
	    break;

	case APR$K_SIRR:
	    data &= 0xf;
	    if (data == 0)
		break;
	    temp = *(__int64 *)&paltempcontext->cns$sirr;
	    temp |= (1 << (data - 1)) << 4;
	    *(__int64 *)&paltempcontext->cns$sirr = temp;
	    break;

	case APR$K_TBIA:
	    mtpr64 (&data, APR$K_TBIA);
	    break;

	case APR$K_TBIAP:
	    mtpr64 (&data, APR$K_TBIAP);
	    break;

	case APR$K_TBIS:
	    mtpr64 (&data, APR$K_TBIS);
	    break;

	/*
	 * when writing stack pointers, we have to look at the
	 * current mode.  If the mode implied by the stack pointer
	 * and the saved mode are the same, the PCBB is not valid
	 * and the saved sp must be used.  Otherwise, we use the
	 * sp in the PCBB.
	 */
	case APR$K_KSP:
	case APR$K_ESP:
	case APR$K_SSP:
	case APR$K_USP:
	    if (ps->cm == (ipr_num - APR$K_KSP)) {
		write_gpr (30, &data);
	    } else {
		read_pt (PT$_PCBB, &src);
		src += (ipr_num - APR$K_KSP) * sizeof (REGDATA);
		stqp (&src, &data);
	    }
	    break;

	case APR$K_VPTB:
	    *(__int64 *)&paltempcontext->cns$ivptbr = data;
	    write_pt (PT$_L3VIRT, &data);
	    break;

	case APR$K_PS:
	    psdata = data;
	    *(__int64 *)&paltempcontext->cns$ipl = ps->ipl;
	    data = 0;
	    ((struct PS *)&data)->cm = ps->cm;
	    *(__int64 *)&paltempcontext->cns$ps = data;
	    *(__int64 *)&paltempcontext->cns$shadow11 = data;
	    data = 0;
	    ((struct PS *)&data)->sw = ps->sw;
	    write_pt (PT$_PS, &data);
	    break;

	case IBX$K_PAL_BASE:
	    *(__int64 *)&paltempcontext->cns$pal_base = data;
	    break;

	case APR$K_ICSR:
	    *(__int64 *)&paltempcontext->cns$icsr = data;
	    break;

	case APR$K_MCSR:
	    *(__int64 *)&paltempcontext->cns$mcsr = data;
	    break;

	/* out of range */
	default:
	    fp->status = msg_exdep_badadr;
	    return 0;
	    break;
	}

    return sizeof(REGDATA);
}

/*+
 * ============================================================================
 * = ev5_ipr_init - init the ipr driver                                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine initializes the IPR driver entry in the global DDB
 * (Driver DataBase) and creates an associated INODE so that the file
 * system recognizes the IPR device (Internal Processor Registers) for
 * examines and deposits.
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
 *      Modifies the Driver DataBase and creates an INODE for the IPR device.
 *
 *--
 */
int ev5_ipr_init (void) {
    struct INODE *ip;			/* Pointer to INODE for this driver */


/* Create an inode entry, thus making the device visible as a file: */

    allocinode (ipr_ddb.name, 1, &ip);
    ip->dva = & ipr_ddb;			/* Device is IPR space */
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;	/* Read and writes allowed */
    ip->loc = 0;
    ip->bs = sizeof (REGDATA);
    INODE_UNLOCK (ip);

    return msg_success;
}
