/* file:	ipr_driver.c
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
 * Author:	JDS - Jim Sawin
 *
 * Modifications:
 *
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	er	23-Jun-1994	Conditionalize for EB64+
 *
 *	mdr	22-Dec-1993	use medulla.h instead of cobra.h for medulla 
 *
 *	dpm	2-Dec-1993	use mustang.h instead of cobra.h for mustang
 *
 *	jmp	11-29-93	Add avanti support
 *
 *	kl	19-Aug-1993	merge
 *
 *	joes	15-Apr-1993	Conditionalize for medulla
 *
 *	pel	15-Jan-1993	Conditionalize for morgan
 *
 *	ajb	13-Jan-1992	added read_ipr, write_ipr for easier access
 *				under program control.  Add ibox and abox ipr access.
  *
 * 	by JDS, Fri Aug  2 13:06:39 1991
 *	Added file offset increment on read/write.
 *
 * 16	by JDS, Mon May 6 13:27:03 1991
 * 	Changed SIRR/SISR  IPR  accesses  to use HIRR in impure area rather than direct access via MxPR.  This will
 * 	give  the desired result of being able to post a software interrupt from the console (eg.  via ^P, deposit)
 * 	and have it activated when PALcode restores context during console exit (eg.  continue).  Added ps_write to
 * 	insure updates of xIER occur when new IPLs are written.
 *
 *  15	by JDS, Fri Apr  5 12:50:17 1991
 *	Updated VAX IPR read/write to handle IPRs which are stored in the PCB.
 *
 *  14	by JDS, Wed Apr  3 16:06:33 1991
 *	Removed SEXT macro (now in common.h).
 *	
 *  13	by JDS, Thu Mar 28 10:04:00 1991
 *	Made module-internal routines static.  Use new PALtemp driver now for pt_read, pt_write routines.
 *
 *  12	by JDS, Mon Mar 11 10:25:21 1991
 *	Added support for IPRs not implemented in pal temps.
 *
 *  11	by JDS, Thu Feb 28 16:43:53 1991
 *	Added support for Alpha to use PAL temp area for IPR storage.
 *
 *  10	sfs	19-Feb-1991	Fix call to ALLOCINODE.
 *
 *  9	by ajb 04-Feb-1991
 *	change calling sequence to read and write routines
 *
 *  8	by JDS, Wed Dec 12 09:57:19 1990
 *	Disabled NYIs by default.
 *
 *  7	by jds, Wed Dec  5 10:12:56 1990
 *	Added ipr_open and ipr_close to establish and clear process-specific exception handler.  Added setjmp call
 * in read/write routines to handle exceptions.
 *
 *  6	by jds, Mon Dec  3 12:53:19 1990
 *	Filled in ipr_read/ipr_write routines.
 *
 *  5   by pel, 14-Nov-1990
 *      Add new arguments to allocinode call.
 *
 *  4   by ajb, 13-Nov-1990
 *	fix compilations under gemcc
 *
 *  3	by JDS, Wed Nov  7 16:01:50 1990
 *	Changed to a real driver (but read/write/validate are still NYI).
 *
 *  2	by JDS, Fri Sep 14 17:06:23 1990
 *	Enhanced portability by changing routine interface.
 *
 *  1	by JDS, Thu Sep 13 09:07:54 1990
 *	Initial entry.
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
	ipr_close,		/* close routine			*/
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
 * = ipr_close  - close the ipr driver                                       =
 * ============================================================================
 *
 * OVERVIEW:
 * 
 *	    This routine closes ipr by clearing the exception handler for
 * the process.  No other special action is required.
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	    - Ignored.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE.
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE.
 *
 *  ROUTINE VALUE:
 *
 *      As returned by exc_vector_clear.
 *
 *  SIDE EFFECTS:
 *
 *      Calls exc_vector_clear, which updates the exception handler queue
 *	of the current process.
 *
 *--
 */
int ipr_close (struct FILE *fp) {
    return msg_success;
}

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
ipr_read (struct FILE *fp, int size, int len, char *buf) {
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
	read_pt (PT$_PS, &psdata);
	ps = &psdata;

	/*
	 * dispatch on the ipr number
	 */
	switch (ipr_num) {
	case APR$K_ASN:			
	    mfpr64 (&data, APR$K_ASN);
	    break;

	case APR$K_ASTEN:		
	    data = ((*(__int64 *)&paltempcontext->cns$hier) >> HIER$V_ASTER) & HIER$M_ASTER;
	    break;

	case APR$K_ASTSR:		
	    data = ((*(__int64 *)&paltempcontext->cns$hirr) >> HIRR$V_ASTRR) & HIRR$M_ASTRR;
	    break;

	case APR$K_FEN:			
	    read_pt (PT$_ICCSR, &data);
	    data = (data >> ICCSR$V_FEN) & 1;
	    break;

	case APR$K_IPIR:		
	    fp->status = msg_exdep_badadr;
	    return 0;
	    break;

	case APR$K_IPL:
	    data = ps->ipl;
	    break;

	case APR$K_MCES:		
	    read_pt (PT$_MCES, &data);
	    data = (data >> 16) & 31;
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

	case APR$K_SIRR:
	    fp->status = msg_exdep_badadr;
	    return 0;
	    break;

	case APR$K_SISR:
	    data = (((*(__int64 *)&paltempcontext->cns$hirr) >> HIRR$V_SIRR) & HIRR$M_SIRR) << 1;
	    break;

	case APR$K_TBCHK:
	case APR$K_TBIA:
	case APR$K_TBIAP:
	case APR$K_TBIS:
	    fp->status = msg_exdep_badadr;
	    return 0;
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

	case APR$K_WHAMI:
	    read_pt (PT$_WHAMI, &data);
	    data &= 0xff;
	    break;

	case APR$K_VPTB:
	    read_pt (PT$_L3VIRT, &data);
	    break;

	case APR$K_PS:
	    data = psdata;
	    break;

	case IBX$K_PAL_BASE:
	    data = *(__int64 *)&paltempcontext->cns$pal_base;
	    break;

	case ABX$K_ABOX_CTL:
	    data = *(__int64 *)&paltempcontext->cns$abox_ctl;
	    break;

	case IBX$K_TB_TAG:
	case IBX$K_ITB_PTE:
	case IBX$K_ICCSR:
	case IBX$K_ITB_PTE_TEMP:
	case IBX$K_EXC_ADDR:
	case IBX$K_SL_RCV:
	case IBX$K_ITBZAP:
	case IBX$K_ITBASM:
	case IBX$K_ITBIS:
	case IBX$K_PS:
	case IBX$K_EXC_SUM:
	case IBX$K_HIRR:
	case IBX$K_SIRR:
	case IBX$K_ASTRR:
	case IBX$K_HIER:
	case IBX$K_SIER:
	case IBX$K_ASTER:
	case IBX$K_SL_CLR:
	case IBX$K_SL_XMIT:
	case ABX$K_DTB_CTL:
	case ABX$K_DTB_PTE:
	case ABX$K_DTB_PTE_TEMP:
	case ABX$K_MMCSR:
	case ABX$K_VA:
	case ABX$K_DTBZAP:
	case ABX$K_DTASM:
	case ABX$K_DTBIS:
	case ABX$K_BIU_ADDR:
	case ABX$K_BIU_STAT:
	case ABX$K_DC_ADDR:
	case ABX$K_DC_STAT:
	case ABX$K_FILL_ADDR:
	case ABX$K_ALT_MODE:
	case ABX$K_CC:
	case ABX$K_CC_CTL:
	case ABX$K_BIU_CTL:
	case ABX$K_FILL_SYNDROME:
	case ABX$K_BC_TAG:
	    mfpr64 (&data, ipr_num);
	    return 0;

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
ipr_write (struct FILE *fp, int size, int len, char *buf) {
    int ipr_num;
    int stat;
    struct impure *paltempcontext;
    int xfer_length;
    __int64 data;
    __int64 src;
    __int64 temp;
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
	read_pt (PT$_PS, &psdata);
	ps = &psdata;


	/*
	 * dispatch on the ipr number
	 */
	switch (ipr_num) {
	case APR$K_ASN:
	    fp->status = msg_exdep_badadr;
	    return 0;
	    break;

	case APR$K_ASTEN:
	    temp = *(__int64 *)&paltempcontext->cns$hier;
	    temp &= ~(~data & HIER$M_ASTER) << HIER$V_ASTER;
	    temp |= ((data >> 4) & HIER$M_ASTER) << HIER$V_ASTER;
	    *(__int64 *)&paltempcontext->cns$hier = temp;
	    break;

	case APR$K_ASTSR:
	    temp = *(__int64 *)&paltempcontext->cns$hirr;
	    temp &= ~(~data & HIRR$M_ASTRR) << HIRR$V_ASTRR;
	    temp |= ((data >> 4) & HIRR$M_ASTRR) << HIRR$V_ASTRR;
	    *(__int64 *)&paltempcontext->cns$hirr = temp;
	    break;

	case APR$K_FEN:
	    read_pt (PT$_ICCSR, &temp);
	    /*
	     *  Below is a kludge to accommodate the fact that the compiler
	     *  does not deal with constants which don't fit in 32 bits.
	     */
	    src = 1;
	    temp &= ~(src << ICCSR$V_FEN);
	    temp |= (data & 1) << ICCSR$V_FEN;
	    write_pt (PT$_ICCSR, &temp);
	    break;

	case APR$K_IPIR:
	    mtpr64 (&data, APR$K_IPIR);
	    break;

	case APR$K_IPL:
	    ps->ipl = data;
	    write_pt (PT$_PS, ps);
	    break;

	case APR$K_MCES:
	    read_pt (PT$_MCES, &temp);
	    temp &= ~((data & 7 | 0x18) << 16);
	    temp |= ((data & 0x18) << 16);
	    write_pt (PT$_MCES, &temp);
	    break;

	case APR$K_PCBB:
	    write_pt (PT$_PCBB, &data);
	    break;

	case APR$K_PRBR:
	    write_pt (PT$_PRBR, &data);
	    break;
	  
	case APR$K_PTBR:
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
	    temp = *(__int64 *)&paltempcontext->cns$hirr;
	    temp |= (1 << (data - 1)) << HIRR$V_SIRR;
	    *(__int64 *)&paltempcontext->cns$hirr = temp;
	    break;

	case APR$K_SISR:
	case APR$K_TBCHK:
	    fp->status = msg_exdep_badadr;
	    return 0;
	
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

	case APR$K_WHAMI:
	    fp->status = msg_exdep_badadr;
	    return 0;

	case APR$K_VPTB:
	    write_pt (PT$_L3VIRT, &data);
	    break;

	case APR$K_PS:
	    write_pt (PT$_PS, &data);
	    break;

	case IBX$K_PAL_BASE:
	    *(__int64 *)&paltempcontext->cns$pal_base = data;
	    break;

	case ABX$K_ABOX_CTL:
	    *(__int64 *)&paltempcontext->cns$abox_ctl = data;
	    break;

	case IBX$K_TB_TAG:
	case IBX$K_ITB_PTE:
	case IBX$K_ICCSR:
	case IBX$K_ITB_PTE_TEMP:
	case IBX$K_EXC_ADDR:
	case IBX$K_SL_RCV:
	case IBX$K_ITBZAP:
	case IBX$K_ITBASM:
	case IBX$K_ITBIS:
	case IBX$K_PS:
	case IBX$K_EXC_SUM:
	case IBX$K_HIRR:
	case IBX$K_SIRR:
	case IBX$K_ASTRR:
	case IBX$K_HIER:
	case IBX$K_SIER:
	case IBX$K_ASTER:
	case IBX$K_SL_CLR:
	case IBX$K_SL_XMIT:
	case ABX$K_DTB_CTL:
	case ABX$K_DTB_PTE:
	case ABX$K_DTB_PTE_TEMP:
	case ABX$K_MMCSR:
	case ABX$K_VA:
	case ABX$K_DTBZAP:
	case ABX$K_DTASM:
	case ABX$K_DTBIS:
	case ABX$K_BIU_ADDR:
	case ABX$K_BIU_STAT:
	case ABX$K_DC_ADDR:
	case ABX$K_DC_STAT:
	case ABX$K_FILL_ADDR:
	case ABX$K_ALT_MODE:
	case ABX$K_CC:
	case ABX$K_CC_CTL:
	case ABX$K_BIU_CTL:
	case ABX$K_FILL_SYNDROME:
	case ABX$K_BC_TAG:
	    mtpr64 (&data, ipr_num);
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
 * = ipr_init - init the ipr driver                                           =
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
ipr_init (void) {
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
