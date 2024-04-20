/* file:	alphamm.c
 * 
 * Copyright (C) 1991, 1993 by
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
 */
 
/* ++
 *  FACILITY:
 * 
 *      Alpha console
 * 
 *  MODULE DESCRIPTION:
 * 
 * 	Alpha Memory Managment routines.
 * 
 *  AUTHORS:
 * 
 * 	jds - Jim Sawin
 * 
 *  MODIFICATION HISTORY:
 * 
 *	by ajb, 13-Aug-1992
 *	added kseg support everywhere else
 *
 *      by cjd, Tue Aug 11 1992
 *      hack in virtual_to_physical() to handle kseg
 *
 *  4	by jds, Wed Apr  3 16:01:47 1991
 *	Removed SEXT macro (now in common.h).
 * 
 *  3	by jds, Wed Apr  3 10:36:49 1991
 *	Fixed bug in probex.
 * 
 *  2	by jds,	Thu Mar 28 13:21:25 1991
 *	Use PALtemp driver to get PTBR.  Use correct offset into IPR
 *	driver for PS register.  Changed printfs to err_printfs.
 * 
 *  1	by jds, Thu Feb 28 13:55:07 1991
 *	Initial entry.
 * --
 */
#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:common.h"
#include "cp$src:alphamm_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:impure_def.h"
#include "cp$src:exdep_def.h"
#include "cp$src:alphaps_def.h"
#include "cp$src:alpha_def.h"
#include "cp$src:aprdef.h"
#include "cp$src:hwrpb_def.h"
#include "cp$src:platform_cpu.h"
#include "cp$inc:prototypes.h"

/* Constants for PROBEx routines, defined such that
 * they can be used as shift values when added to access
 * mode.
 */
#define VAPROT_M_READ 8		/* the three read bits in a pte start at bit 8 */
#define VAPROT_M_WRITE 12	/* the three write bits in a pte start at bit 12 */

/* Use 8K pages: */

typedef struct VA_8K VA;
#define LOG_PAGESIZE 13L
#define PAGESIZE (1L << LOG_PAGESIZE)
#define PTES_PER_PAGE (PAGESIZE / 8L)

/* valid, all access modes enabled, GH = 0 */
#define PTE_PROTO 0x0000ff1f

#if EV6
extern int page_table_levels;
#else
#define page_table_levels 3
#define HWRPB$_VA_SIZE 43
#endif

extern struct HWRPB *hwrpb;

/*+
 * ============================================================================
 * = get_pte - return the pte corresponding to a virtual address              =
 * ============================================================================
 *
 * OVERVIEW:
 *	This function looks up the Page Table Entry for a given virtual address,
 *	and returns that pte.  The user supplies a pointer to where the pte will
 *	be stored.  This pointer does not have to be quadword aligned.
 *
 *	There are 3 mapping schemes that the firmware currently knows about:
 *	VMS, OSF, and NT.  Until there is a general mechanism whereby the
 *	firmware can interrogate the OS about mapping information, we have to
 *	have this knowledge hard coded, and we have to know which mapping to
 *	use.
 *
 *	Not all translation schemes have explicit page tables.  In those cases,
 *	we construct fake PTEs.
 *
 *	Since virtual mapping is set up by the boot program, no translations
 *	are possible until the bootstrap has run; otherwise we don't know
 *	where the PTEs are, and which translation mechanisms to use.
 *
 *	In the case where KSEG0 mapping is used, we simulate a pte to keep
 *	a uniform access method to callers of this routine.
 *
 *  FORMAL PARAMETERS:
 *
 *      void va		- The virtual address.
 *	struct PTE *pte	- Pointer to page table entry.  When PTE is found,
 *			the value is written to this location.
 *
 *  IMPLICIT INPUTS:
 *
 *      The saved PTBR in the entry context.
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE.
 *
 *  FUNCTION VALUES:
 *
 *	msg_success	    - PTE was successfully found.
 *	msg_vmem_invalpfn   - Invalid PFN.
 *
 *  SIDE EFFECTS:
 *
 *      NONE.
 *
 *--
 */
int get_pte (uint64 va, struct PTE *pte) {
    uint64 seg0pte, seg1pte, seg2pte, seg3pte;
    struct PTE *seg0p, *seg1p, *seg2p, *seg3p;
    uint64 ptbr; /* PFN of the page tables */
    uint64 x;
    uint64 mask = 0;
    struct IMPURE *impure;
    uint64 pted;
    struct PTE *ptep;
    int stat;
    uint64 vasize;

    if (page_table_levels == 4) {
	vasize = HWRPB$_VA_SIZE;
    } else {
	vasize = 43;
    }

    /* Get PTBR from saved context.
     */
    read_ipr (APR$K_PTBR, &ptbr);
#if SEPARATE_PAGE_TABLES
    read_ipr (APR$K_VIRBND, &x);
    if (va > x) {
	read_ipr (APR$K_SYSPTBR, &ptbr);
    }
#endif

    /*
     * HACK ALERT!!!!
     * OSF kseg addresses map one-to-one, not through the page tables.
     * This hack works because VMS only uses 32 bit virtual addresses
     * and thus should never generate a virtual address in the range
     * of OSF kseg (FFFF FC00 0000 0000 .. FFFF FFFF FFFF FFFF).
     * So test for address in kseg range and return direct mapping.
     *
     * The correct long term solution is a call forward to the os 
     * to replace this entire routine.
     */
    if (((va >> (vasize - 2)) & 3) == 2) {
    	*(unsigned __int64 *) pte = PTE_PROTO;
	pte->pfn =  (va & ~(~mask << (vasize-2))) >> LOG_PAGESIZE;
	return msg_success;
    }

    seg0p = & seg0pte;
    seg1p = & seg1pte;
    seg2p = & seg2pte;
    seg3p = & seg3pte;

    if (page_table_levels == 4) {
	/* fetch the pte from the zeroth segment */
	x = ptbr * PAGESIZE + ((va >> VA_V_SEG0) & VA_M_SEG) * 8;
	ldqp (&x, seg0p);
	if (!seg0p->V) return (msg_vmem_invalpfn);

	/* fetch the pte from the first segment */
	x = (uint64)seg0p->pfn * PAGESIZE + ((va >> VA_V_SEG1) & VA_M_SEG) * 8;
    } else {
	/* fetch the pte from the first segment */
	x = ptbr * PAGESIZE + ((va >> VA_V_SEG1) & VA_M_SEG) * 8;
    }
    ldqp (&x, seg1p);
    if (!seg1p->V) return (msg_vmem_invalpfn);

    /* fetch the pte from the second segment */
    x = (uint64)seg1p->pfn * PAGESIZE + ((va >> VA_V_SEG2) & VA_M_SEG) * 8;
    ldqp (&x, seg2p);
    if (!seg2p->V) return (msg_vmem_invalpfn);

    /* fetch the pte from the third segment */
    x = (uint64)seg2p->pfn * PAGESIZE + ((va >> VA_V_SEG3) & VA_M_SEG) * 8;
    ldqp (&x, seg3p);
    if (!seg3p->V) return (msg_vmem_invalpfn);

    /* copy the pte (and work around a compiler bug) */
    memcpy (pte, seg3p, sizeof (struct PTE));

    return msg_success;
}


/*+
 * ============================================================================
 * = probex - probe a virtual address for read or write access                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Probe a virtual address for read or write access.  Use the processors
 *	mode defined in the saved context to determine which protections have
 *	to be checked.
 *
 * FORM OF CALL:
 *
 * 	probex(int accessmask, int mode, uint64 len, uint64 va)
 *
 * RETURNS:
 *      msg_success	    Access is permitted.
 *	msg_vmem_accvio	    Access violation.
 *	other		    as returned by get_pte
 *
 * ARGUEMNTS:
 *
 *	int accessmask	- Determines whether to probe for read or write access. 
 *			 Should be one of VAPROT_M_READ or VAPROT_M_WRITE.
 *      int mode	- Processor mode for access (Kernel, Exec, Super, User).
 *			- should be one of 0, 1, 2 or 3 respectively.
 *	void *va	- Virtual Address to probe.
 *
 *
 *  SIDE EFFECTS:
 *
 *      NONE.
 *
 *--
 */
int probex(int accessmask, int mode, uint64 va) {
    uint64 pted;
    int stat;

    if ((stat = get_pte ((uint64) va, &pted)) != msg_success)
	return stat;

    if (!(pted & (1 << (accessmask+mode)))) {
	return (msg_vmem_accvio);
    }

    return msg_success;
}

/*+
 * ============================================================================
 * = prober - probe a virtual region for read access                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	See the description on probex.
 *
 * FORM OF CALL:
 *
 * RETURNS:
 *
 * ARGUEMNTS:
 *
 *  SIDE EFFECTS:
 *
 *--
 */
int prober(int mode, uint64 va) {
    return probex(VAPROT_M_READ, mode, (uint64) va);
}

/*+
 * ============================================================================
 * = probew - probe a virtual region for write access                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	See the description on probex.
 *
 * FORM OF CALL:
 *
 * RETURNS:
 *
 * ARGUEMNTS:
 *
 *  SIDE EFFECTS:
 *
 *--
 */
int probew(int mode, uint64 va) {
    return probex(VAPROT_M_WRITE, mode, (uint64) va);
}

/*+
 * ============================================================================
 * = virtual_to_physical - translate a virtual address to a physical address =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Emulate the translation of a virtual address to a physical address,
 *	using the page tables specified by the saved entry context.
 *
 *  FORMAL PARAMETERS:
 *
 *      void va		- The virtual address.
 *	void *pa	- Address to store the resultant physical address.
 *
 *  IMPLICIT INPUTS:
 *
 *      Saved page table registers from entry context.
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE.
 *
 *  FUNCTION VALUES:
 *
 *      msg_success	    - Conversion was successful.
 *	msg_vmem_invalpfn   - Invalid PFN.
 *	OTHERS		    - as returned by get_pte().
 *
 *  SIDE EFFECTS:
 *
 *      NONE.
 *
 *--
 */
int virtual_to_physical( uint64 va, uint64 *pa) {
    uint64 pted;
    struct PTE *ptep;
    int stat;
    int bwp;

    /* get the pte */
    ptep = & pted;
    if ((stat = get_pte(va, ptep)) != msg_success)
	return stat;

    /* Extract byte within page field */
    bwp = ((unsigned) va >> VA_V_BWP) & VA_M_BWP;

    /* check for a valid pte */
    if (ptep->V) {
    	*pa = ((uint64)ptep->pfn * PAGESIZE) + bwp;
    } else {
	*pa = 0;
	return(msg_vmem_invalpfn);
    }
    return msg_success;
}


int physcheck (uint64 pfn) {
	int i;
	struct HWRPB *hwrpb_ptr;
	struct MEMDSC *memdsc;
	uint64 pfn_lo;
	uint64 pfn_hi;

	hwrpb_ptr = hwrpb;
	memdsc = (int)hwrpb_ptr + *hwrpb_ptr->MEM_OFFSET;
	for (i = 0; i < *memdsc->CLUSTER_COUNT; i++) {
	    pfn_lo = *(uint64 *)memdsc->CLUSTER[i].START_PFN;
	    pfn_hi = pfn_lo + *(uint64 *)memdsc->CLUSTER[i].PFN_COUNT;
	    if ((pfn >= pfn_lo) && (pfn < pfn_hi)) {
		return 0;
	    }
	}
	err_printf ("%X:  PFN out of range\n", pfn);
	return 1;
}

/* translate a kseg0 address to a console native address.
 */
unsigned __int64 kseg0_to_native (unsigned __int64 va) {
	return va & 0x7fffffff;
}

/* translate a native address to a kseg0 address.
 */
int native_to_kseg0 (int pa) {
	return pa | 0x80000000;
}

/*+
 * ============================================================================
 * = vtop - Virtual to physical address translation.                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will return a physical address from a virtual address.
 *
 * FORM OF CALL:
 *
 *	vtop (vaddr); 
 *
 * RETURNS:
 *
 *      void* paddr - Physical address.
 *
 * ARGUMENTS:
 *
 *	void *vaddr - Virtual address to be converted.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

void* vtop (void *vaddr)
{
	return(vaddr);
}

/*+
 * ============================================================================
 * = ptov - Physical to virtual address translation.                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will return a virtual address from a physical address.
 *
 * FORM OF CALL:
 *
 *	ptov (paddr); 
 *
 * RETURNS:
 *
 *      void *vaddr - Virtual address.
 *
 * ARGUMENTS:
 *
 *	void *paddr - Physical address to be converted.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

void* ptov (void *paddr)
{
	return(paddr);
}

