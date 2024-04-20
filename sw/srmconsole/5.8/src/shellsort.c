/* file:	shellsort.c
 *
 * Copyright (C) 1993 by
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
 */

/*
 *++
 *  FACILITY:
 *
 *      Common Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      shellsort
 *
 *  AUTHORS:
 *
 *      Paul LaRochelle
 *
 *  CREATION DATE:
 *  
 *      05-Oct-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	8-Jun-1993	Broken out from sort.
 *
 *	ajb	14-Feb-1991	Make it a proper subset of U*x sort command
 *
 *      pel	17-Oct-1990	return err if fopen's fail.
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"

/*----------*/
void shellsort (int *v, int n, int (*cmp) ()) {
	int	i, j, gap;
	int	*hold;

	for (gap = n/2; gap > 0; gap /= 2) {
	    for (i= gap; i < n; i++) {
		for (j = i-gap; j >= 0; j -= gap) {
		    if ((*cmp) (v [j], v [j+gap]) <= 0) break;
		    hold = v [j];
		    v [j] = v[j+gap];
		    v [j+gap] = hold;
		}
	    }
	}
}
