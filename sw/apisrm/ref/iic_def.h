/* file:	iic_def.h
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
 *      The iic_def.h module contains the data Structures used by the 
 *	IIC Bus class driver routines. 
 *
 *  AUTHORS:
 *
 *      David Baird
 *
 *  CREATION DATE:
 *  
 *      3-Jan-1991
 *
 *  MODIFICATION HISTORY:
 *
 *	db	27-Jul-1992	Fix command timeout errors acessing iic bus 
 *				devices
 *
 *	db	25-Jun-1992	Updated to add ISR timeout support
 *
 *	db	10-Jun-1992	Updated to add PSC command Timeout support
 *
 *	db	27-Apr-1992	Updated to support the PSC command
 *
 *	db	20-Dec-1991 	Updated to add an Interrupt Driver for Cobra
 *
 *	db	06-Dec-1991	Updated to support the Cobra I/O module based IIC Bus 
 *					interface.
 *
 *	db	24-Oct-1991	Updated to enhance the Serial ROM based
 *				Mini-Console
 *
 *	db	05-Aug-1991	Updated to support the Serial ROM based 
 *				Mini-Console
 *
 *	ini	03-Jan-1991	David Baird
 *	
 *--
 */


#ifndef	iic_h
#define iic_h 0		    /* Prevent multiple compiles of this module */

#if SABLE
#define IIC_VECTOR 0x97
#endif
#if RAWHIDE
#define IIC_VECTOR1 0xa9
#define IIC_VECTOR2 0xaa
#endif
#if MIKASA
#define IIC_VECTOR 0x97
#endif
#if ALCOR
#define IIC_VECTOR 0x97
#endif

/*
 * Register Bit Definations for the Control Register
 */

#define IIC_S0  0x00			/* Data Register		*/
#define IIC_S0P 0x00			/* Own Address Register		*/
#define IIC_S2  0x20			/* Clock Register		*/
#define IIC_S3  0x10			/* Interrupt Vector Register	*/

/*
 * SCL Frequency Bit Definations for the Clock Register
 */

#define IIC_SCL_90  0x00		/* 90 KHZ			*/
#define IIC_SCL_45  0x01		/* 45 KHZ			*/
#define IIC_SCL_11  0x02		/* 11 KHZ			*/
#define IIC_SCL_15  0x03		/* 1.5 KHZ			*/

/*
 * Input Clock Frequency Bit Definations for the Clock Register
 */
 
#define IIC_CLOCK_3   0x00 		/* 3 MHZ			*/
#define IIC_CLOCK_443 0x10 		/* 4.43 MHZ			*/
#define IIC_CLOCK_6   0x14 		/* 6 MHZ			*/
#define IIC_CLOCK_8   0x18 		/* 8 MHZ			*/
#define IIC_CLOCK_12  0x1c 		/* 12 MHZ			*/


/*
 * Defination of the IIC Control word
 */

#define IIC_ACKB 0x01			/* Acknowledge after each byte send*/
#define	IIC_STO  0x02			/* Send Stop condition		*/
#define	IIC_STA  0x04			/* Send Start condition		*/
#define IIC_ENI  0x08			/* External Interrupt Enable	*/
#define IIC_ESO  0x40			/* Enable serial output		*/
#define	IIC_PIN  0x80			/* Pending Interrupt Not (must be 0) */

/*
 * Define direction flags
 */

#define IIC_READ_DIR  0x01
#define IIC_WRITE_DIR 0x00
#define IIC_IDLE      0x02
#define IIC_S_WRITE   0x03
#define IIC_S_READ    0x04
#define IIC_SR_DONE   0x0A
#define IIC_TIMEOUT   0x00
#define IIC_REC_TIMEOUT 2000  /* 2 Sec Timeout value */
#define IIC_MAS_TIMEOUT 2000  /* 2 Sec Timeout value */

/*
 * Define setup condition for this module
 */
#if WEBBRICK
#define IIC_CLOCK IIC_SCL_45 + IIC_CLOCK_6 /* Clock register setup	*/
#else
#define IIC_CLOCK IIC_SCL_90 + IIC_CLOCK_6 /* Clock register setup	*/
#endif

#define IIC_STOP  0xC3 	/*IIC_S0+IIC_STO+IIC_ACKB+IIC_ESO+IIC_PIN */
#define IIC_START 0xC5 	/*IIC_S0+IIC_STA+IIC_ACKB+IIC_ESO */
#define IIC_INIT  0xC0	/*IIC_S0+IIC_ESO */
#define IIC_NACK  0xC0	/*IIC_S0+IIC_ESO */
#define IIC_ACK   0xC1	/*IIC_ACKB+IIC_S0+IIC_ESO */

/*
 * Defination of the IIC Status Word
 */

struct IIC_STATUS_BITS {
	unsigned int bb:1;		/* Bus Busy NOT			*/
	unsigned int lab:1;		/* Lost Arbitration 		*/
	unsigned int aas:1;		/* Addressed as Slave 		*/
	unsigned int ado:1;		/* Address 0 (general Call) or	*/
					/*   Last recieved bit 		*/
	unsigned int ber:1;		/* Bus Error			*/
	unsigned int sts:1;		/* External Stop signal recieved*/
	unsigned int res:1;		/* Reserved must be 0		*/
	unsigned int pin:1;		/* Pending Interrupt NOT	*/
};

union IIC_STATUS_UNION {
	struct IIC_STATUS_BITS bits;
	char word;
};

struct IICPB {
	int	mode;			/* DDB$K_Interrupt of POLLED    */
	char	*buffer_addr;		/* Pointer to transfer buffer	*/
	int	count;			/* Number of bytes to transfer	*/
	int	rec_count;		/* Number of bytes recieved	*/
	int	direction;		/* Direction of current data Trans. */
	int	isr_complete;		/* isr completed flag for timeouts */
	int	slave_count;		/* Number of slave bytes to read */
	int	led_count[8];		/* LED on/off counters		*/ 
	struct FILE *iic_fp1;
	struct FILE *iic_fp2;
	struct POLLQ pqp;		/* Driver Poll Queue entry	*/
	struct SEMAPHORE avail_sem;	/* IIC Bus available Semaphore */
	struct TIMERQ misr_t;		/* Master Completion Timer */
	struct TIMERQ isr_t;		/* Slave Completion Timer */
	unsigned char	*slave_addr;	/* Pointer to slave buffer	*/
	char	status;			/* Driver Status on Error 	*/
	char	int_flag;		/* Interrupt Enable State	*/
};

struct IICCB {

	int	type;			/* Device Type for this Device	*/
	struct IICPB *pb;		/* Pointer to IIC port block	*/
	struct SEMAPHORE avail_sem;	/* IIC Bus Node available Semaphore */
	char	address;		/* Device IIC bus node address  */
};

#if SABLE || MIKASA || ALCOR || RAWHIDE || K2 || TAKARA || PC264
#define IIC_DATA 0			/* Data register Address */
#define IIC_STATUS 1			/* Status register Address */
#endif

#endif
