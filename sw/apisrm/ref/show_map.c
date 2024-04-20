/* file:	show_map.c
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
 *      Common console
 * 
 *  MODULE DESCRIPTION:
 * 
 * 	Alpha Memory display routines.
 * 
 *  AUTHORS:
 * 
 * 	J. Kirchoff
 * 
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

#define	PTESIZE			8L		/* in bytes */
#define BYTES_PER_LEVEL3	PAGESIZE
#define	BYTES_PER_LEVEL2	((PAGESIZE / PTESIZE) * BYTES_PER_LEVEL3)

#define LOG_PAGESIZE 13L
#define PAGESIZE (1L << LOG_PAGESIZE)
#define PTES_PER_PAGE (PAGESIZE / 8L)

#if EV6
extern int page_table_levels;
#else
#define page_table_levels 3
#define HWRPB$_VA_SIZE 43
#endif

physcheck (uint64 pfn);

/*+
 * ============================================================================
 * = show map - Display the system virtual memory map.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *          
 *      Display the current system virtual memory map.  This routine will not
 *	show PTEs that are hardwired into the chip (i.e. super pages).
 *
 *   COMMAND FORM:
 *  
 *   	show map ()
 *  
 *   COMMAND ARGUMENT(S):
 *
 *	None
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>show map
 *      pte 00001020 v FFFFFC0902408000 p 00000000 V KR    SR            FR FW
 *      pte 00001028 v FFFFFC090240A000 p 00000000 V KR    SR               FW
 *      pte 00001020 v FFFFFC0902C08000 p 00000000 V KR    SR            FR FW
 *      pte 00001028 v FFFFFC0902C0A000 p 00000000 V KR    SR               FW
 *      pte 00001020 v FFFFFC0B02408000 p 00000000 V KR    SR            FR FW
 *      pte 00001028 v FFFFFC0B0240A000 p 00000000 V KR    SR               FW
 *      pte 00001020 v FFFFFC0B02C08000 p 00000000 V KR    SR            FR FW
 *      pte 00001028 v FFFFFC0B02C0A000 p 00000000 V KR    SR               FW
 *	>>>
 *~
 * FORM OF CALL:                             
 *  
 *	showmap( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *      msg_success	    - Conversion was successful.
 *	msg_vmem_invalpfn   - Invalid PFN.
 *	others		    - as returned by get_pte().
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      None                                 
 *--
 */
void showmap (int argc, char *argv[]) {
    uint64 ptep0, pte0;
    uint64 ptep1, pte1;
    uint64 ptbr;
    uint64 vasize, vamask, vasign;
    struct PTE *p0p, *p1p;
    int64 va, last_va;
    int	p0, p1;
    uint64 bytes_per_level0;
    uint64 bytes_per_level1;
    int reported;

    reported = 0;
    p0p = & pte0;
    p1p = & pte1;

    /* Get PTBR from saved context */

    read_ipr (APR$K_PTBR, &ptbr);
    if (ptbr) {
	if (physcheck (ptbr)) return;
	va = 0;
	bytes_per_level1 = (PAGESIZE / PTESIZE);
	bytes_per_level1 *= BYTES_PER_LEVEL2;
	bytes_per_level0 = (PAGESIZE / PTESIZE);
	bytes_per_level0 *= bytes_per_level1;

	/* Set up a bitmask so that we can sign extend the va */
	if (page_table_levels == 4) {
	    vasize = HWRPB$_VA_SIZE;
	} else {
	    vasize = 43;
	}
	vasign = 1;
	vasign <<= (vasize - 1);
	vamask = 1;
	vamask <<= vasize;
	vamask = ~(vamask - 1);

	if (page_table_levels == 4) {
	    /*
	     * Visit every PTE that can be reached, starting with the level 0
	     * page tables.  Assume there is only one page of level 0 PTEs.
	     */
	    for (p0 = 0; p0 < PTES_PER_PAGE; p0++) {
		if (killpending ()) break;

		/* read in level 0 PTE */
		ptep0 = ptbr * PAGESIZE + p0 * PTESIZE;
#if DEBUG
		printf ("ptbr 0x%x ptep0 0x%x \n",ptbr,ptep0);
#endif
		ldqp (&ptep0, &pte0);

		/* if it's valid, we continue on to level 1 */
		if (p0p->V) {
		    if (physcheck (p0p->pfn)) continue;
		    for (p1 = 0; p1 < PTES_PER_PAGE; p1++) {
			if (killpending ()) break;

			/* read in level 1 PTE */
			ptep1 = (uint64) p0p->pfn * PAGESIZE + p1 * sizeof (struct PTE);
#if DEBUG
			printf ("p0p->pfn 0x%x ptpe1 0x%x \n",p0p->pfn,ptep1);
#endif
			ldqp (&ptep1, &pte1);

			/* if it's valid, we continue on to level 2 */
			if (p1p->V) {
			    if (physcheck (p1p->pfn)) continue;
			    va = p0 * bytes_per_level0 + p1 * bytes_per_level1;
			    showmap_level2 (p1p, va, &last_va, &reported);
			}
		    }
		}
	    }
	} else {
	    /*
	     * Visit every PTE that can be reached, starting with the level 1
	     * page tables.  Assume there is only one page of level 1 PTEs.
	     */
	    for (p1 = 0; p1 < PTES_PER_PAGE; p1++) {
		if (killpending ()) break;

		/* read in level 1 PTE */
		ptep1 = ptbr * PAGESIZE + p1 * PTESIZE;
#if DEBUG
		printf ("ptbr 0x%x ptep1 0x%x \n",ptbr,ptep1);
#endif
		ldqp (&ptep1, &pte1);

		/* if it's valid, we continue on to level 2 */
		if (p1p->V) {
		    if (physcheck (p1p->pfn)) continue;
		    va = p1 * bytes_per_level1;
		    showmap_level2 (p1p, va, &last_va, &reported);
		}
	    }
	}
    }
}

int showmap_level2 (struct PTE *p1p, int64 va, int64 *last_va, int *reported) {
    uint64 ptep2, pte2;
    uint64 ptep3, pte3;
    uint64 vasize, vamask, vasign;
    struct PTE *p2p, *p3p;
    int	p2, p3;
    struct VA *vap;
    
    p2p = & pte2;
    p3p = & pte3;

    /* Set up a bitmask so that we can sign extend the va */
    if (page_table_levels == 4) {
	vasize = HWRPB$_VA_SIZE;
    } else {
	vasize = 43;
    }
    vasign = 1;
    vasign <<= (vasize - 1);
    vamask = 1;
    vamask <<= vasize;
    vamask = ~(vamask - 1);

    /* visit each PTE in this level 2 page */
    for (p2 = 0; p2 < PTES_PER_PAGE; p2++) {
	if (killpending ()) break;

	/* read in level 2 PTE */
	ptep2 = (uint64) p1p->pfn * PAGESIZE + p2 * sizeof (struct PTE);
#if DEBUG
	printf ("p1p->pfn 0x%x ptpe2 0x%x \n",p1p->pfn,ptep2);
#endif
	ldqp (&ptep2, &pte2);

	/* if it's valid, we continue on to level 3 */
	if (p2p->V) {
	    if (physcheck (p2p->pfn)) continue;
	    /* visit each PTE in this level 3 page */
	    for (p3 = 0; p3 < PTES_PER_PAGE; p3++) {
		if (killpending ()) break;

		/* read in level 3 PTE */
		ptep3 = (uint64) p2p->pfn * PAGESIZE + p3 * sizeof (struct PTE);
#if DEBUG
	    	printf ("p2p->pfn 0x%x ptpe3 0x%x \n",p2p->pfn,ptep3);
#endif
		ldqp (&ptep3, &pte3);

		/* if this level 3 PTE is valid, we print the results */
		if (p3p->V) {
		    if (*reported && (va != (*last_va + PAGESIZE))) {
			printf ("===\n");
		    }
		    *reported = 1;
		    *last_va = va;
		    printf ("pte %016X %016X  va %016X  pa %016X\n",
			    (uint64) ptep3,
			    (uint64) pte3,
			    (uint64) (va & vasign) ? (va | vamask) : va,
			    (uint64) p3p->pfn * PAGESIZE
		    );
		}

		/* bump va regardless of level validity */
		va += BYTES_PER_LEVEL3;
	    }

	/* level 2 not valid, bump VA anyway */
	} else {
	    va += BYTES_PER_LEVEL2;
	}
    }
    return va;
}
