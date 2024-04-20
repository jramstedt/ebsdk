/* file:	psc_def.h
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
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      The psc_def.h module contains the data Structures used by the 
 *	Cobra Power System Controller (PSC) Interface routines. 
 *
 *  AUTHORS:
 *
 *      David Baird
 *
 *  CREATION DATE:
 *  
 *      10-Apr-1992
 *
 *  MODIFICATION HISTORY:
 *
 *	db	16-Jul-1992	Update the fan speed lower limit
 *
 *	ini	10-Apr-1992	David Baird
 *	
 *--
 */


#ifndef	psc_h

#define psc_h 0		    /* Prevent multiple compiles of this module */

#define PSC_MAX_RESP 25

struct PSC_CMD_PKT {
	unsigned char length;
	unsigned char reply_addr;
	unsigned char command;
	unsigned char parameter;
	unsigned char data[5];
};
	
union PSC_CMD {
	struct PSC_CMD_PKT detail;
	unsigned char bytes[sizeof(struct PSC_CMD_PKT)];
};

struct PSC_RESP_PKT {
	unsigned char length;
	unsigned char sour_addr;
	unsigned char command;
	unsigned char parameter;
	unsigned char status;
	union {
		unsigned char bytes[PSC_MAX_RESP * 2];
		unsigned short int words[PSC_MAX_RESP];
	} data;
};

union PSC_RESP {
	struct PSC_RESP_PKT detail;
	unsigned char bytes[sizeof(struct PSC_RESP_PKT)];
};

#define PSC_HOST_ADDR	0xB6  /* Temp Def */

/* PSC command Definations */
#define PSC_SEND	0x01
#define PSC_RESET	0x02
#define PSC_SET		0x03
#define PSC_CLEAR	0x04

/* PSC Qualifier definations */
#define PSC_UPDATE	0x01
#define PSC_STATUS	0x02
#define PSC_PARAMETRIC	0x03
#define PSC_HOST	0x04
#define PSC_POWER	0x05
#define PSC_SWITCHED	0x06
#define PSC_DC		0x07
#define PSC_LDC		0x08
#define PSC_HEATER	0x09
#define PSC_FANS	0x0A
#define PSC_FAN		0x0B

/* PSC Parameter definations */
#define PSC_2.1V	0x00
#define PSC_3.3V	0x01
#define PSC_5.1V	0x02
#define PSC_12.0V	0x03
#define PSC_LDC0	0x00
#define PSC_LDC1	0x01
#define PSC_LDC2	0x02
#define PSC_LDC3	0x03
#define PSC_LDC4	0x04
#define PSC_LDC5	0x05
#define PSC_LDC6	0x06
#define PSC_LDC7	0x07
#define PSC_LDCA	0x08
#define PSC_LOW_SPEED	0x00
#define PSC_HIGH_SPEED	0x01
#define PSC_FIXED_SPEED	0x02

/* PSC ERROR CODES */
#define PSC_OK		0x00
#define PSC_ERROR	0x01

/* Other PSC Constaints */
#define PSC_MAX_PARAMETRICS	9
#define PSC_DATA_OFF 		5
#define PSC_MAX_LDC 		4
#define PSC_FAN_LOW	0x40
#define PSC_FAN_HIGH	0xff

#endif
