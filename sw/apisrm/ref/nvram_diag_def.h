/* File:	nvram_diag_def.h
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	NVRAM "Non-Volatile Ram" Diagnostic Header file
 *
 * Author:	D.W. Neale
 *
 * Modifications:
 *
 *	dwn	22-June-1993	Initial Entry
*/
 
/* LOCAL SYMBOLS and Macro definitions
*/


/* NVRAM Register/Ram offsets
*/
#define NVRAM_OffBgn	0x800	/* Page beginning offset */
#define NVRAM_OffEnd	0x8FF	/* Page ending    offset */
#define NVRAM_PageSize	256	/* Page size */
#define NVRAM_NumPage	32	/* Number of pages */

/* Starting with a Timeout count down of 10000, it took an average 
/*	2,198 interations for the nvram timeout,  for the nvram
/*	write data to be read back.  For variations in HW a fudge
/*	factor of x2 is implemented.
*/
#define NVRAM_TO	4000

/* NVRAM CSR Bit Mask definitions
/*    RW bits have an associated 1
/*    RO bits have an associated 0
*/
#define NVRAM_Mask	 0xFF	/* */

/* NVRAM reset values 
*/
#define NVRAM_Init	 0x00	/* */

/* NVRAM Default values 
*/
#define NVRAM_Def	 0x00	/* */


