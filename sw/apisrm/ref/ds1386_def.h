/* File:	ds1386_def.h
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
 * Abstract:	DS1386 Diagnostic Header file
 *
 * Author:	M. Robert
 *
 * Modifications:
 *
 *	mdr	25-Oct-1993	Initial Entry
*/
 
/* LOCAL SYMBOLS and Macro definitions
*/


/* NVRAM Register/Ram offsets
*/
#define NVRAM_OffBgn	0x00	/* Page beginning offset */
#define NVRAM_OffEnd	0xFF	/* Page ending    offset */
#define NVRAM_PageSize	256	/* Page size */
#define NVRAM_NumPage	128	/* Number of pages */

/* TOY Offsets */

#define CommandReg	(RTC_BASE + 0x0B) /* Toy command register */
#define SecondsReg	(RTC_BASE + 0x01) /* Toy Seconds register */
#define MinutesReg	(RTC_BASE + 0x02) /* Toy Minutes register */
#define HoursReg	(RTC_BASE + 0x04) /* Toy Hours register */
#define DaysReg		(RTC_BASE + 0x06) /* Toy Days register */
#define DateReg		(RTC_BASE + 0x08) /* Toy Date register */
#define MonthsReg	(RTC_BASE + 0x09) /* Toy Months register */
#define YearReg		(RTC_BASE + 0x0A) /* Toy Year register */

static char RegBitMask [] = {
	0xFF,	/* Reg 0 */
	0x7F,	/* Reg 1 */
	0x7F,	/* Reg 2 */
	0xFF,	/* Reg 3 */
	0x7F,	/* Reg 4 */
	0xFF,	/* Reg 5 */
	0x07,	/* Reg 6 */
	0x87,	/* Reg 7 */
	0x3F,	/* Reg 8 */
	0xDF,	/* Reg 9 */
	0xFF,	/* Reg A */
	0xFC,	/* Reg B */
	0xFF,	/* Reg C */
	0xFF,	/* Reg D */
};
