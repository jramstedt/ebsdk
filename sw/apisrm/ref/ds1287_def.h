/* File:	ds1287_def.h
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
 * Abstract:	DALLAS DS1287 "Real Time Clock" header definitions file
 *
 * Author:	D.W. Neale
 *
 * Modifications:
 *
 *	dwn	21-June-1993	Initial Entry
 */
 
/* LOCAL SYMBOLS and Macro definitions
*/


/* DS1287 PCI CSR definitions
/*	DS1287 internal registers and User Ram is accessed thru the ESC,
/*	via an INDEX and data register.  Writes to the INDEX register 
/*	selects and latches the address in the DS1287.  Read/Writes to 
/*	the DATA register either write/read from/to the DS1287.
/*
/* Port 70 is both NMI Control and Real-Time Clk address
/* NMI Cntl bit[7] must be preserved when writing to RTC bits[6:0].
/* See ESC Chip Spec page 111
*/
#define DS1287_INDEX	0x70	/* Configuration Index    Base Address */
#define DS1287_DATA	0x71	/* Configuration Data     Base Address */

/* DS1287 Register/Ram offsets
*/
#define DS1287_SEC	0x00|ESC$DNmiCsrRtc	/* Seconds */
#define DS1287_SEC_ALRM	0x01|ESC$DNmiCsrRtc	/* Seconds Alarm */
#define DS1287_MIN	0x02|ESC$DNmiCsrRtc	/* Minutes */
#define DS1287_MIN_ALRM	0x03|ESC$DNmiCsrRtc	/* Minutes Alarm */
#define DS1287_HRS	0x04|ESC$DNmiCsrRtc	/* Hours */
#define DS1287_HRS_ALRM	0x05|ESC$DNmiCsrRtc	/* Hours Alarm */
#define DS1287_DAY_WK	0x06|ESC$DNmiCsrRtc	/* Day of the Week */
#define DS1287_DAY_MH	0x07|ESC$DNmiCsrRtc	/* Day of the Month */
#define DS1287_MONTH	0x08|ESC$DNmiCsrRtc	/* Month */
#define DS1287_YEAR	0x09|ESC$DNmiCsrRtc	/* Year */

#define DS1287_REGA	0x0A|ESC$DNmiCsrRtc	/* Register A */
#define DS1287_REGB	0x0B|ESC$DNmiCsrRtc	/* Register B */
#define DS1287_REGC	0x0C|ESC$DNmiCsrRtc	/* Register C */
#define DS1287_REGD	0x0D|ESC$DNmiCsrRtc	/* Register D */

#define DS1287_RAM_BGN	0x0E|ESC$DNmiCsrRtc	/* User Ram Beginning */
#define DS1287_RAM_END	0x3F|ESC$DNmiCsrRtc	/* User Ram End */

/* DS1287 CSR Bit Mask definitions
/*    RW bits have an associated 1
/*    RO bits have an associated 0
*/
#define DS1287_MSEC	 0xFF	/* Seconds */
#define DS1287_MSEC_ALRM 0xFF	/* Seconds Alarm */
#define DS1287_MMIN	 0xFF	/* Minutes */
#define DS1287_MMIN_ALRM 0xFF	/* Minutes Alarm */
#define DS1287_MHRS	 0xFF	/* Hours */
#define DS1287_MHRS_ALRM 0xFF	/* Hours Alarm */
#define DS1287_MDAY_WK	 0xFF	/* Day of the Week */
#define DS1287_MDAY_MH	 0xFF	/* Day of the Month */
#define DS1287_MMONTH	 0xFF	/* Month */
#define DS1287_MYEAR	 0xFF	/* Year */

#define DS1287_MREGA	 0x7F	/* REgister A */
#define DS1287_MREGB	 0xFF	/* Register B */
#define DS1287_MREGC	 0x00	/* Register C */
#define DS1287_MREGD	 0x00	/* Register D */

#define DS1287_MRAM	 0xFF	/* 50 Bytes of User Defined Ram */

/* DS1287 CSR reset values 
*/
#define DS1287_ISEC	 0x00	/* Seconds */
#define DS1287_ISEC_ALRM 0x00	/* Seconds Alarm */
#define DS1287_IMIN	 0x00	/* Minutes */
#define DS1287_IMIN_ALRM 0x00	/* Minutes Alarm */
#define DS1287_IHRS	 0x00	/* Hours */
#define DS1287_IHRS_ALRM 0x00	/* Hours Alarm */
#define DS1287_IDAY_WK	 0x00	/* Day of the Week */
#define DS1287_IDAY_MH	 0x00	/* Day of the Month */
#define DS1287_IMONTH	 0x00	/* Month */
#define DS1287_IYEAR	 0x00	/* Year */

#define DS1287_IREGA	 0x00	/* REgister A */
#define DS1287_IREGB	 0x87	/* Register B */
#define DS1287_IREGC	 0x00	/* Register C */
#define DS1287_IREGD	 0x80	/* Register D */

/* DS1287 CSR Default Program values 
*/
#define DS1287_DSEC	 0x3B	/* Seconds */
#define DS1287_DSEC_ALRM 0x00	/* Seconds Alarm */
#define DS1287_DMIN	 0x36	/* Minutes */
#define DS1287_DMIN_ALRM 0x00	/* Minutes Alarm */
#define DS1287_DHRS	 0x01	/* Hours */
#define DS1287_DHRS_ALRM 0xFFFFFFFF	/* Hours Alarm */
#define DS1287_DDAY_WK	 0x01	/* Day of the Week */
#define DS1287_DDAY_MH	 0x0F	/* Day of the Month */
#define DS1287_DMONTH	 0x05	/* Month */
#define DS1287_DYEAR	 0xFFFFFF90	/* Year */

#define DS1287_DREGA	 0x25	/* REgister A */
#define DS1287_DREGB	 0x0E	/* Register B */
#define DS1287_DREGC	 0x00	/* RO Register C */
#define DS1287_DREGD	 0x00	/* RO Register D */

