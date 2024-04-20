/* file:	vmem_driver.c
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
 * Abstract:	Virtual memory driver for examine/deposit.
 *
 * Author:	JDS - Jim Sawin
 *
 * Modifications:
 *
 *  13	by JDS, Fri Apr  5 09:24:47 1991
 *	Filled in validate routine.
 *
 *  12	by JDS, Thu Mar 28 13:38:09 1991
 *	Use correct offset within IPR driver to access PS.  Changed printfs to err_printfs.
 *
 *  11	by JDS, Thu Mar  7 22:36:55 1991
 *	Fixed VAX PSL read.
 *
 *  10	by JDS, Thu Feb 28 15:37:04 1991
 *	Added hooks to call Alpha memory mgmt routines.
 *
 *  9	sfs	19-Feb-1991	Fix call to ALLOCINODE.
 *
 *  8	by JDS, Thu Feb 14 09:59:26 1991
 *	Fixed get_pte reference for alpha build.  Fixed modification history.
 *
 *  7	by JDS, Wed Feb  6 09:22:03 1991
 *	Filled in vmem read/write routines.
 *
 *  6	by ajb 04-Feb-1991
 *	change calling sequence to xx_read, xx_write
 *
 *  5	by JDS, Wed Dec 12 09:56:27 1990
 *	Disabled NYIs by default.
 *
 *  4   by pel, 14-Nov-1990
 *      Add new arguments to allocinode call.
 *
 *  3	by ajb, 13-Nov-1990
 *	Fix declarations for compilation by gemcc
 *
 *  2	by JDS, Wed Nov  7 16:01:50 1990
 *	Changed to a real driver (but read/write/validate are still NYI).
 *
 *  1   16-OCT-1990 21:55:25 BEAVERSON "Virtual memory driver for examine/deposit.
 *
 ***** NEW CMS LIBRARY.  GENERATIONS RESET TO 1. *****
 *
 *  2	by JDS, Fri Sep 14 11:23:59 1990
 *	Enhanced portability by changing calling interface.  Address arguments are now passed as generic pointers
 * rather than a union.  Data arguments are also passed as generic pointers.
 *
 *  1	by JDS, Thu Sep 13 09:07:28 1990
 *	Initial entry.
 */

#include "cp$src:platform.h"
#include    "cp$src:kernel_def.h"
#include    "cp$src:aprdef.h"
#include    "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include    "cp$src:msg_def.h"
#include    "cp$src:exdep_def.h"
#include    "cp$src:alpha_def.h"
#include    "cp$src:alphaps_def.h"
#include    "cp$src:alphamm_def.h"
#include    "cp$src:get_console_base.h"

#define	 PAGESIZE 8192
#include    "cp$src:alphascb_def.h"

/* External references */
extern int fwrite ();
extern int fread ();
extern int prober ();
extern int probew ();

#define umin64(a,b) (((UINT) (a)) < ((UINT) (b)) ? ((UINT) (a)) : ((UINT) (b)))
#define umax64(a,b) (((UINT) (a)) > ((UINT) (b)) ? ((UINT) (a)) : ((UINT) (b)))

int vmem_read ();
int vmem_write ();
int vmem_open ();
int vmem_close ();
int null_procedure ();

struct DDB vmem_ddb = {
	"vmem",			/* how this routine wants to be called	*/
	vmem_read,		/* read routine				*/
	vmem_write,		/* write routine			*/
	vmem_open,		/* open routine				*/
	vmem_close,		/* close routine			*/
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
 * = vmem_open - open virtual memory for access                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Open up the virtual memory device.  Wire in exception handlers to
 *	catch non existent memory, alignment faults and other self induced
 *	errors.
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	    - file pointer
 *	char *info	    - optional offset into virtual memory
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
 *--
 */
int vmem_open (struct FILE *fp, char *info, char *next, char *mode) {
    int status;

    /*
     * Treat the info field as a hex offset if it is present
     */
    status = common_convert (info, 16, fp->offset, sizeof (QUADWORD));
    if (status) return status;

    /*
     * Open the physical memory device.  We use the pmem driver so
     * that we don't have to care if we ourselves are running virtual.
     * Since the misc field is used to store the last physical to virtual
     * translation, we save the file pointer for pmem in the count field, which
     * is otherwise not used by this driver.
     */
    if ((fp->count = fopen ("pmem", mode)) == NULL) {
	fp->status = msg_vmem_devopenrderr;
	return fp->status;
    }

    return msg_success;
}

/*+
 * ============================================================================
 * = vmem_close - close virtual memory                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Close the virtual memory device by clearing the machine check exception
 * handlers.
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	    - file pointer
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
 * of the current process.
 *
 *--
 */
int vmem_close (struct FILE *fp) {
    int stat1;
    int stat2;

    fclose (fp->count);	/* close out pmem driver */

    stat1 = exc_vector_clear(SCB$Q_SYS_MCHK);
    stat2 = exc_vector_clear(SCB$Q_PR_MCHK);
    return (stat1 == msg_success) ? stat2 : stat1;
}

/*+
 * ============================================================================
 * = vmem_read - read from virtual memory                                     =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine reads a byte, word, longword, or quadword from virtual
 * memory.  The address is stored in the FILE structure, and the data is
 * written to the specified buffer.
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	- Pointer to FILE structure.  This should correspond to
 *			 the VMEM device.
 *	int size	- size of item in bytes
 *	int number	- number of items to read from virtual memory.
 *	char *buf	Address to write the data to.
 *
 *  RETURN VALUE:
 *
 *   	Number of bytes read.
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
int vmem_read (struct FILE *fp, int size, int number, char *buf) {
    return vmem_readwrite (fp, size, number, buf, prober, fread);
}

/*+
 * ============================================================================
 * = vmem_write - write virtual memory                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine writes a byte, word, longword, or quadword to virtual
 * memory.  The address is stored in the FILE structure, and the data is
 * read from the specified buffer.
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should correspond to the VMEM device.
 *	int size	- size of item in bytes
 *	int number	- number of items to write to virtual memory.
 *	char *buf	Address to read the data from.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes written.
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
int vmem_write (struct FILE *fp, int size, int number, char *buf) {
    return vmem_readwrite (fp, size, number, buf, probew, fwrite);
}


/*
 * Common read/write routines
 */
int vmem_readwrite (struct FILE *fp, int size, int number, char *buf, int (*probe) (), int (*readwrite) ()) {
    INT stat;
    INT len;
    UINT pa;  /* ADDRESS pa; */
    UINT va_end, va_page_end;
    UINT bytesleft;
    UINT xfer_length;
    UINT va;
    UINT pagemask;
    int mode;
    struct FILE *ipr_fp;		/* File pointer for IPR device */
    struct PTE *pte;
    struct PS ps;				/* Copy of saved PS */

    /* Get PS from saved context: */

    read_ipr (APR$K_PS, &ps);
    mode = ps.cm;				/* Get execution mode from saved PS */

    va = * (UINT *) fp->offset;
    pagemask = PAGESIZE - 1;
    pagemask = ~pagemask;
   
    /*
     * process the request in chunks that don't span page
     * boundaries
     */
     bytesleft = size * number;
     va_end = *(UINT *) fp->offset + bytesleft - 1;
     while (bytesleft) {

	/* calculate the address of the end of this page */
	va_page_end = (va & pagemask) + (PAGESIZE - 1);

	/* calculate the number of bytes that will be taken
	 * from this page
	 */
	xfer_length = umin64(va_end, va_page_end) - va + 1;

	/* probe for access */
	if ((stat = (*probe) (mode, (UINT) va)) != msg_success) {
	    fp->status = stat;
	    break;
	}

	/* Convert the virtual address to a physical address and remember it */
	virtual_to_physical ((UINT) va, &pa);
	fp->misc = pa;

	/* read/write the block */
	if (((int)pa & 7) || ((int)buf & 7) || ((int)xfer_length & 7)) {
	    fseek (fp->count, pa, SEEK_SET);
	    len = (*readwrite) (buf, 1, xfer_length, fp->count);
	    if (len == 0) {
		break;
	    }
	} else {
	    int i;
#pragma pointer_size save
#pragma pointer_size 64
	    uint64 *src;
	    uint64 *dst;
#pragma pointer_size restore

	    if (readwrite == fread) {
		src = phys_to_virt (pa);
		dst = buf;
	    } else {
		src = buf;
		dst = phys_to_virt (pa);
	    }
	    for (i = 0; i < xfer_length/8; i++)
		*dst++ = *src++;
	    len = xfer_length;
	}

	* (UINT *) fp->offset += len;
	buf += len;
	bytesleft -= len;
	va += len;
    }

    return size * number - bytesleft;
}

/*
 *++
 *  FUNCTIONAL DESCRIPTION:
 *
 *      vmem_init
 *	This routine initializes the VMEM driver entry in the global DDB
 * (Driver DataBase) and creates an associated INODE so that the file system
 * recognizes the VMEM device (virtual memory) for examines and deposits.
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
 *      Modifies the Driver DataBase and creates an INODE for the VMEM device.
 *
 *--
 */
int vmem_init (void) {
    struct INODE *ip;

/* Create an inode entry, thus making the device visible as a file: */

    allocinode (vmem_ddb.name, 1, &ip);
    ip->dva = & vmem_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;

    INODE_UNLOCK (ip);

    return msg_success;
}
