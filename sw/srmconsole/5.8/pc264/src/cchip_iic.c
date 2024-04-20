/* file:    cchip_iic.c
 *
 * Copyright (C) 1990 by
 * Digital Equipment Corporation,Maynard,Massachusetts.
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
 *      Cobra Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *  IIC bus driver for EVAX/Cobra firmware.
 *
 *  AUTHORS:
 *
 *  David Baird
 *
 *  CREATION DATE:
 *  
 *  2-Jan-1991
 *
 *
 *  MODIFICATION HISTORY:
 * 
 *	jp	24-mar-1994	Add support for mikasa. Mikasa ocp is same
 *				as sable,however our rom space is not on
 *				iic bus. Also,no interrupt line available
 *				so we will run polled mode (like pele)
 *
 *	mc	02-Apr-1993	Initialize ilist pointer to first entry in
 *				iic_node_list before creation of iic master
 *				inode.
 *
 *	db	11-Sep-1992	added check for driver enabled in the open
 *					routine
 *
 *	db	27-Jul-1992	Fixed iic bus command timeouts 
 *
 *	db	25-Jun-1992	Add ISR timeout support
 *
 *	db	10-Jun-1992	Add PSC command timeout support
 *
 *	db	9-Jun-1992	correct cpu naming convention cpu1/2 to cpu0/1
 *
 *	db	3-Jun-1992	Try again - see below
 *
 *	db	2-Jun-1992	Fix a driver race condition
 *
 *	db	29-May-1992	changed printf's to msg numbers
 *
 *	db	04-May-1992	Added iic_vterm (0x44) to driver list
 *
 *	db	27-Apr-1992	Updated to support the PSC command
 *
 *	ajb	17-Jan-1992	Change calling sequence to common_convert
 *
 *	db	20-Dec-1991 Updated to add an Interrupt Driver for Cobra
 *
 *	db	06-Dec-1991	Updated to support the Cobra I/O module based IIC Bus 
 *					interface.
 *
 *	db	24-Oct-1991	Updated to enhance the Serial ROM based
 *					Mini-Console
 *
 *  db  05-Aug-1991 	Add support for the SROM-based 
 *          			Mini-console.
 *
 *  db  17_Jun-1991 	Add support for Lost Arbitration and Slave
 *              		operation
 *
 *  ajb 13-jun-1991 	change getpcb call
 *
 *  db 	19-Apr-1991 	Fix a compile time error
 *
 *  db 	17-Apr-1991 	verify that a device exists before creating
 *              		an inode for it. 
 *
 *  db 	16-Apr-1991 	add diagnostic code for the init routine
 *
 *  db 	28-Mar-1991 	add support for fseek,and clean up the error 
 *              		reporting.
 *
 *  pel 12-Mar-1991 	let fopen/close incr/decr inode ref & collapse
 *              		wrt_ref,rd_ref into just ref.
 *
 *  dwb 2-Jan-1991  	Initial Code 
 *
 *--
 */

/*Include files*/
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:prdef.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ansi_def.h"
#include "cp$src:stddef.h"
#include "cp$src:srom_def.h"
#include "cp$src:msg_def.h"
#include "cp$inc:platform_io.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"
#include "cp$src:ev_def.h"
#include "cp$src:tsunami.h"
#include "cp$src:Nt_Types.h"

/*External references*/
static unsigned char portValue = 0xff;						/* Remember what we want the port to be. */

#define	I2CTRANSITIONDELAY		krn$_micro_delay(10/2)
#define	I2CDATADELAY			krn$_micro_delay(500/2)


#define DATA_RECEIVE	0x08
#define CLOCK_RECEIVE	0x04
#define DATA_SEND		0x02
#define CLOCK_SEND		0x01

static void SetSCL(unsigned char bitValue)
{
	portValue = (portValue & ~CLOCK_SEND)|(bitValue?CLOCK_SEND:0);
	WriteTsunamiCSR(CSR_MPD,portValue);
	I2CTRANSITIONDELAY;									/* Wait for a time. */
	/*DbgPrint(("WC:X X %d %d\n",(portValue >> 1) & 1,(portValue >> 0) & 1));*/

	/* Ensure other party not holding it low. */
	while (bitValue != ((ReadTsunamiCSR(CSR_MPD) & CLOCK_RECEIVE)?1:0))
		DbgPrint(("Waiting Clock\n"));
}

static unsigned char  SetSDA(unsigned char bitValue)
{
	unsigned char value;

	if (bitValue == 0xff)
	{
		portValue |= DATA_SEND;
		WriteTsunamiCSR(CSR_MPD,portValue);
		I2CTRANSITIONDELAY;											/* Wait for a time. */
		value = ((ReadTsunamiCSR(CSR_MPD) & DATA_RECEIVE)?1:0);
		/*DbgPrint(("RD:X %d %d %d\n",value,(portValue >> 1) & 1,(portValue >> 0) & 1));*/
	}
	else
	{
		portValue = (portValue & ~DATA_SEND)|(bitValue?DATA_SEND:0);
		WriteTsunamiCSR(CSR_MPD,portValue);
		I2CTRANSITIONDELAY;											/* Wait for a time. */
		/*DbgPrint(("WD:X X %d %d\n",(portValue >> 1) & 1,(portValue >> 0) & 1));*/
	}

	return(value);
}

static void SendGpioI2cStart(void)
{
	DbgPrint(("Start\n"));
	SetSDA(1);												/* Make sure SDA high */
	SetSCL(1);												/* Make sure SCL high */
	I2CDATADELAY;											/* Allow receiver to see this state. */
	SetSDA(0);												/* Then transition SDA to low to do a start bit. */
	I2CDATADELAY;											/* Allow receiver to see this state. */
	SetSCL(0);												/* Make SCL follow to 0.*/
}

static unsigned char SendGpioI2cByte(
unsigned char data)
{
	unsigned char bitCount;
	unsigned char acknowledge;

	DbgPrint(("Send Data %x\n",data));
	for (bitCount = 0; bitCount < 8; bitCount++,data *= 2)				/* 8 bits. */
	{
		SetSCL(0);											/* Set SCL low so we can change SDA. */
		SetSDA((data & 0x80) != 0);							/* Set the value on SDA */
		I2CDATADELAY;										/* Wait 1/2 clock cycle. */
		SetSCL(1);											/* Set SCL high so the bit get read. */
		I2CDATADELAY;										/* Allow reader time to read it. */
	}
																
	SetSCL(0);												/* Set SCL low to allow acknowledge to be driven onto bus. */
	I2CDATADELAY;											/* Allow reader time to write it. */
	SetSCL(1);												/* Clock back the acknowledge. */
	I2CDATADELAY;											/* Allow reader time to write it. */
	acknowledge = SetSDA(0xff);								/* TURN SDA into and input. So we can see if now acknowledge. */
	DbgPrint(("Acknowledge %x\n",acknowledge));

	SetSCL(0);												/* Set SCL low to finish the operation. */

	return(acknowledge == 0);
}

static unsigned char ReadGpioI2cByte(int acknowledge)
{
	unsigned char bitCount;
	unsigned char data;

	SetSDA(0xff);											/* Set SDA to input. */

	for (data = 0,bitCount = 0; bitCount < 8; bitCount++)	/* 8 bits. */
	{
		SetSCL(0);											/* Set SCL low so target can change SDA. */
		I2CDATADELAY;										/* Allow data to be driven on to bus. */
		SetSCL(1);											/* Set SCL high so the bit cab be read. */
		I2CDATADELAY;										/* Wait 1/2 clock cycle. */
		data = (data*2)|SetSDA(0xff);						/* Set bottom bit if present. */
	}

	SetSCL(0);												/* Set SCL low so we can change SDA. */
	SetSDA(acknowledge?0:1);								/* Clock out the acknowledge or not. */
	I2CDATADELAY;											/* Allow reader time to read it. */
	SetSCL(1);												/* Set SCL high so the bit get read. */
	I2CDATADELAY;											/* Allow reader time to read it. */

	SetSCL(0);												/* Set SCL low again. */

	DbgPrint(("Read Data %x\n",data));

	return(data);
}

static void SendGpioI2cStop(void)
{
	DbgPrint(("Stop\n"));

	SetSCL(0);												/* Make SCL goto to 0. */
	SetSDA(0);												/* Make sure SDA = 0,so it can transition to 1 later. */
	I2CDATADELAY;											/* Allow reader time to read it. */
	SetSCL(1);												/* Make SCL goto to 1. */
	SetSDA(1);												/* Then transition SDA to high to do a stop bit. */
}

unsigned long CChipI2cTransaction(
struct FILE *fp,
unsigned int read,
unsigned long node,
unsigned long offset,
unsigned long size,
unsigned char *data)
{
	int addressNotSent = TRUE;

	DbgPrint(("CChipI2cTransaction(%x,%x,%x,%x,*%x)\n",read,node,offset,size,data[0]));

	if (offset != -1)											/* Then need to send offset. */
	{
		SendGpioI2cStart();
		if (SendGpioI2cByte(node) == FALSE)						/* Address with lowest bit clear so write. */
		{
			DbgPrint(("Device(1) Not Found %x\n",node));
			SendGpioI2cStop();
			return(FALSE);
		}

		SendGpioI2cByte(offset);
		
		if (read)												/* If read then we need to send a stop/start */
		{
			SendGpioI2cStop();
		}
		else
			addressNotSent = FALSE;								/* Else we can just carry on writing. */
	}

	if (addressNotSent)
	{
		SendGpioI2cStart();

		if (SendGpioI2cByte(node|(read?0x01:0x00)) == FALSE)		/* Address with lowest bit set if read. */
		{
			DbgPrint(("Device(2) Not Found %x\n",(node|(read?0x01:0x00))));
			SendGpioI2cStop();
			return(FALSE);
		}
	}

	if (read)
	{
		data[0] = ReadGpioI2cByte(size == 2);

		if (size == 2)
			data[1] = ReadGpioI2cByte(FALSE);
	}
	else
	{
		SendGpioI2cByte(data[0]);

		if (size == 2)
			SendGpioI2cByte(data[1]);
	}

	SendGpioI2cStop();

	return(TRUE);
}
