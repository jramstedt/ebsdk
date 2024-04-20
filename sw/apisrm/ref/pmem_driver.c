/* file:	pmem_driver.c
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
 * Abstract:	Physical memory driver for examine/deposit.  On Alpha, this
 *		driver handle 64 bit offsets.  On VAX, only 32.
 *
 * Author:	JDS - Jim Sawin
 *
 * Modifications:
 *
 *	hcb	21-Aug-1992	Add exdep spinlock
 *
 *	ajb	21-Apr-1992	Make all pmem access use ldqp.
 *
 *	ajb	23-jan-1992	use 64 bit offsets so that we can reach all
 *				physical addresses
 *
 * 20	by ajb 20-dec-1991
 *	use ldqp/stqp for alpha
 *
 * 19	by ajb 25-Sep-1991
 *	return 0 on errors for pmem_read, pmem_write
 *
 * 18	by JDS Fri Apr  5 09:25:29 1991
 *	Filled in validate routine.  Updated revision history.
 *
 * 17    11-MAR-1991 10:43:04 BEAVERSON ""
 * 16    11-MAR-1991 10:03:16 BEAVERSON ""
 * 15     7-MAR-1991 14:00:45 BEAVERSON ""
 * 13A1   7-MAR-1991 13:53:05 BEAVERSON ""
 *
 * 14	sfs	19-Feb-1991	Fix calls to ALLOCINODE.
 *
 * 13    13-FEB-1991 12:16:39 BEAVERSON ""
 * 12    11-FEB-1991 16:23:56 SHIRRON ""
 *
 * 11A1	by AJB 07-Feb-1991
 *	Create RMEM and PMEM devices.  PMEM is a device that trys to handle
 *	and fix up alignment exceptions; RMEM is a more primative device
 *	that reflects all exceptions back to the caller.
 *
 * 11	by AJB 04-Feb-1991
 *	changed calling sequence to xx_read, xx_write; insure that pmem_read
 *	and pmem_write can handle any data size
 *
 * 10	by JDS, Fri Jan 25 13:26:27 1991
 *	Added calls to Alpha ldqp_u/stqp_u to provide access to physical address space.  The previous code
 * was a temporary solution which operated only on a byte-by-byte basis on the console's virtual space.
 *
 *  9A1    5-FEB-1991 17:27:14 BEAVERSON "change calling sequence to xx_read"
 *
 *  9	by JDS, Fri Jan 25 09:42:51 1991
 *	Incremented *fp->offset by the data length for the VAX case in the read/write routines.
 *
 *  8	by JDS, Thu Jan 24 17:00:09 1991
 *	Merged 6A1 with 7.  Fixed generation numbers in revision history.
 *
 *  7	by ajb 23-jan-1991
 *	file offset not being updated
 *
 *  6A1	by JDS, Mon Jan 21 16:16:54 1991
 *	Conditionalized VAX/Alpha portions.  Used instructions to match data types for the VAX case.
 *
 *  6	by JDS, Wed Dec 12 09:55:17 1990
 *	Disabled NYIs by default.
 *
 *  5	by jds, Tue Dec  4 15:24:55 1990
 *	Added pmem_open & pmem_close to establish and clear the process-specific machine-check exception handler.
 * Added setjmp calls to read/write routines to handle exceptions.
 *
 *  4   by pel, 14-Nov-1990
 *      Add new arguments to allocinode call.
 *
 *  3	by ajb, 13-Nov-1990
 *	fix declarations for compilation under gemcc
 *
 *  2	by JDS, Wed Nov  7 16:01:50 1990
 *	Changed to a real driver.  Removed requirement of instruction type matching the data type since this is
 * physical memory anyway.
 *
 *  1	by JDS, Fri Sep 14 11:16:59 1990
 *	Enhanced portability by changing calling interface.  Address arguments are now passed as generic pointers
 * rather than a union.  Data arguments are now passed as generic pointers as well.  Denoted processor-specific
 * code with conditional compiler constant VAXC.
 *
 *** NEW CMS LIBRARY CREATED 16-OCT-1990.  GENERATIONS RESET TO 1. ***
 *
 *  1	by JDS, Thu Sep 13 09:07:10 1990
 *	Initial entry.
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include    "cp$src:kernel_def.h"
#include    "cp$src:common.h"
#include    "cp$src:dynamic_def.h"                 
#include    "cp$src:exdep_def.h"
#include    "cp$src:msg_def.h"
#include    "cp$src:alphascb_def.h"
#if TURBO
#include    "cp$src:emulate.h"
#endif
#include "cp$inc:prototypes.h"

/* External references */

#if TURBO
extern int pci_debug_pcia_hose;
#endif
extern struct HQE mchq_660;			/*MCHECK handler queue*/
extern int memcpy ();
extern struct LOCK spl_exdep;

/* Function declarations: */

int pmem_close (struct FILE *fp);				/* PMEM close routine */
int pmem_init(void);						/* DDB initialization */
int pmem_open (struct FILE *fp, char *info, char *next, char *mode); /* PMEM open routine */
int pmem_read (struct FILE *fp, int size, int number, unsigned char *buf);		/* PMEM read routine (examine) */
int pmem_write (struct FILE *fp, int size, int number, unsigned char *buf);		/* PMEM write routine (deposit) */
int null_procedure ();

extern struct DDB *pmem_ddb_ptr;

struct DDB pmem_ddb = {
	"pmem",			/* how this routine wants to be called	*/
	pmem_read,		/* read routine				*/
	pmem_write,		/* write routine			*/
	pmem_open,		/* open routine				*/
	pmem_close,		/* close routine			*/
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

int pmem_handle_nxm (int param,int mchk_frame);
int pmem_bad_addr;

/*
 *++
 *  FUNCTIONAL DESCRIPTION:
 *
 *      pmem_close:
 *	    This routine closes pmem by clearing the machine-check exception
 * handler for the process.  No other special action is required.
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
int pmem_close (struct FILE *fp) {
    int stat1;
    int stat2;

    /*
     * Release exception handlers
     */
/* Restore the default handler */                        
    remq_lock(fp->misc);     
    dyn$_free(fp->misc,DYN$K_SYNC);
    return (msg_success);
}

/*
 *++
 *  FUNCTIONAL DESCRIPTION:
 *
 *      pmem_init
 *	This routine initializes the PMEM driver entry in the global DDB
 * (Driver DataBase) and creates an associated INODE so that the file 
 * system recognizes the PMEM device (physical memory) for examines and
 * deposits.
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
 *      Modifies the Driver DataBase and creates an INODE for the PMEM device.
 *
 *--
 */
int pmem_init (void) {
    struct INODE *ip;

    pmem_ddb_ptr = &pmem_ddb;

    allocinode (pmem_ddb.name, 1, &ip);
    ip->dva = & pmem_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;
    ip->loc = 0;
    INODE_UNLOCK (ip);

    return msg_success;
}


/*
 *++
 *  FUNCTIONAL DESCRIPTION:
 *      
 *      Open up the memory device.  Wire in exception handlers to catch non
 *	existent memory, alignment faults and other self induced errors.
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	    - Ignored.
 *	char *info	    - treated as a hex address if present.
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
 *      Calls exc_vector_set, which updates the exception handler queue for
 * the current process.
 *
 *--
 */
int pmem_open (struct FILE *fp, char *info, char *next, char *mode) {
    struct HQE *mqe;				/*MCHECK handler queue entry*/
    fp->offset = &fp->local_offset;
    memset (&fp->local_offset, 0, sizeof (fp->local_offset));
        
    /*
     * Treat the info field as a hex offset if it is present
     */
    if (info  &&  *info) {
	common_convert (info, 16, &fp->local_offset, sizeof (fp->local_offset));
    }

    /*
     * Set up exception handlers
     */
/* set up the exception handler */
    fp->misc = mqe = (struct HQE*)dyn$_malloc(sizeof(struct HQE),DYN$K_SYNC|DYN$K_NOOWN);
    mqe->handler = pmem_handle_nxm;
    mqe->param = 0;
    insq_lock(mqe,mchq_660.blink);

    return msg_success;
}


/*
 *++
 *  FUNCTIONAL DESCRIPTION:
 *
 *      pmem_handle_nxm
 *
 * This routine handles non exsistent memory acceses for the pmem driver.
 * It simply checks to see if it is a nxm then calls the exdep handler.
 *
 *  FORMAL PARAMETERS:
 *
 *	int param - Parameter passed from the global handler.
 *	int mchk_frame - Address of the machine check frame.
 *
 *  RETURN VALUE:
 *
 *   	msg_success - The machine check was expected
 *   	msg_failure - The machine check was unexpected
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

int pmem_handle_nxm (int param,int mchk_frame)
{
	/* Check for machine specific handling of nxms */
	machine_handle_nxm(mchk_frame);
	pmem_bad_addr = 1;

/*Always return a success*/
	return (msg_success);
}


/*
 *++
 *  FUNCTIONAL DESCRIPTION:
 *
 *      pmem_read
 * This routine reads a byte, word, longword, or quadword from physical memory. 
 * The address is stored in the FILE structure, and the data is written to the
 * specified buffer.  When the read is attempted, a machine-check exception may
 * occur.  In that case, the handler set up by the pmem_open routine will field
 * the exception and cause a longjmp back to pmem_read, which will load an error
 * status into the file structure and return with 0 to indicate the error.
 *
 * On alpha machines, this routine always reads in quadword chunks using LDQP
 * (LoaD Quad Physical), and then does the necessary deblocking.
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.
 *	int size	- size of item
 *	int number	- number of items to read
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
int pmem_read (struct FILE *fp, int size, int number, unsigned char *buf) {
    INT length;
    UINT offset;
    UINT datum;
    int i;
    int t;

    fp->status = msg_success;
    length = size * number;
    offset = *(INT *) &fp->local_offset;

    /*
     * read as many quadwords as we need to to cover the entire source
     */

#if TURBO
    if (pci_debug_pcia_hose != -1) {
	while (length) {
	    spinlock(&spl_exdep);
	    pmem_bad_addr = 0;
	    ldqp_u ((UINT) offset, &datum);
	    t = pmem_bad_addr;
	    spinunlock(&spl_exdep);
	    if (t) {
		fp->status = msg_exdep_badadr;	/* Failure */
		return 0;			/* Return 0 to indicate failure */
	    }
	    for (i=0; i<sizeof (datum) && length; i++, length--) {
		*buf++ = datum & 0xff;
		datum >>= 8;
	    }
	    offset += i;
	}
    } else 
#endif
    while (length) {
	spinlock(&spl_exdep);
	pmem_bad_addr = 0;
	if ((size == 4) && ((offset & 3) == 0) && (length >= 4)) {
	    *(int *)buf = ldl (offset);
	    buf += 4;
	    offset += 4;
	    length -= 4;
	} else if ((size == 8) && ((offset & 7) == 0) && (length >= 8)) {
	    *(INT *)buf = ldq (offset);
	    buf += 8;
	    offset += 8;
	    length -= 8;
	} else {
	    ldqp_u (offset, &datum);
	    for (i=0; i<sizeof (datum) && length; i++, length--) {
		*buf++ = datum & 0xff;
		datum >>= 8;
	    }
	    offset += i;
	}
#if WILDFIRE
	krn$_micro_delay(5);
#endif
	t = pmem_bad_addr;
    	spinunlock(&spl_exdep);
	if (t) {
#if !( TURBO || RAWHIDE )
	    err_printf(msg_exdep_badadr);
#endif
	    fp->status = msg_exdep_badadr;	/* Failure */
	    return 0;				/* Return 0 to indicate failure */
	}
    }

    *(INT *) &fp->local_offset = offset;
    return size * number;
}
                             
/*
 *++
 *  FUNCTIONAL DESCRIPTION:
 *              
 *      pmem_write
 *	This routine writes a byte, word, longword, or quadword to physical
 *  memory.  The address is stored in the FILE structure, and the data is
 * read from the specified buffer.  When the write is attempted, a
 * machine-check exception may occur.  In that case, the handler set up by the
 * pmem_open routine will field the exception and cause a longjmp back to
 * pmem_write, which will load an error status into the file structure and
 * return with 0 to indicate the error.
 *
 *  FORMAL PARAMETERS:
 *      
 *      struct FILE *fp	Pointer to FILE structure.  This should correspond to the PMEM device.
 *	int size	- size of item
 *	int number	- number of items
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
int pmem_write (struct FILE *fp, int size, int number, unsigned char *buf) {
    INT length;
    UINT offset;
    UINT datum;
    UINT merge;
    int i;                      
    int t;

    fp->status = msg_success;
    length = size * number;
    offset = *(INT *) &fp->local_offset;

#if TURBO
    if (pci_debug_pcia_hose != -1) {
	while (length) {
	    datum = 0;
	    for (i=0; i<sizeof (datum) && length; i++, length--) {
		datum |= (UINT)*buf++ << (i*8);
	    }                    

	    spinlock(&spl_exdep);
	    pmem_bad_addr = 0;
	    /* compensate for a quadword that exceeds the buffer */
	    if (i < sizeof (datum)) {
		ldqp_u ((UINT) offset, &merge);
		merge &= ~(((UINT)1 << (i*8)) - 1);
		datum |= merge;
	    }
	
	    stqp_u ((UINT) offset, &datum);
	    t = pmem_bad_addr;
	    spinunlock(&spl_exdep);
	    if (t) {
    		fp->status = msg_exdep_badadr;		/* Failure */
		return 0;				/* Return 0 to indicate failure */
	    }
	    offset += i;
	}
    } else
#endif

    while (length) {
	spinlock(&spl_exdep);
	pmem_bad_addr = 0;
	if ((size == 4) && ((offset & 3) == 0) && (length >= 4)) {
	    stl (offset, *(int *)buf);
	    buf += 4;
	    offset += 4;
	    length -= 4;
	} else if ((size == 8) && ((offset & 7) == 0) && (length >= 8)) {
	    stq (offset, *(INT *)buf);
	    buf += 8;
	    offset += 8;
	    length -= 8;
	} else {
	    datum = 0;
	    for (i=0; i<sizeof (datum) && length; i++, length--) {
		datum |= (UINT)*buf++ << (i*8);
	    }                    
	    if (i < sizeof (datum)) {
		ldqp_u (offset, &merge);
		merge &= ~(((UINT)1 << (i*8)) - 1);
		datum |= merge;
	    }
	    stqp_u (offset, &datum);
	    offset += i;
	}
#if WILDFIRE
	krn$_micro_delay(5);
#endif
	t = pmem_bad_addr;
    	spinunlock(&spl_exdep);
	if (t) {
#if !( TURBO || RAWHIDE )
	    err_printf(msg_exdep_badadr);
#endif
    	    fp->status = msg_exdep_badadr;		/* Failure */
	    return 0;					/* Return 0 to indicate failure */
	}
    }

    *(INT *) &fp->local_offset = offset;
    return size * number;
}       
