/* file:    m1543_iic.c
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

#include "cp$src:m1543c_SBridge.h"
#include "cp$src:Nt_Types.h"
#include "cp$src:m1543c_def.h"

/*External references*/

#define DELAY(time)		krn$_sleep(time * 10)

#define TIMEOUT_SHUTDOWN	(30/CHECKINTERVAL)
#define CHECKINTERVAL		5

#define TIMEOUTVALUE	255
#define IIC_DEBUG		0

#if IIC_DEBUG
#define Debug(x)	(qprintf x)
#define outportByte(_z_,_x_,_y_)	(														\
							 Debug(("outportb(NULL,%x,%x)\n",(char *)_x_-(char *)pSmbIOReg,_y_)),	\
							 outportb(_z_,_x_,_y_ )										\
							)

#define inportByte( _z_, _x_)		(														\
							 _y_ = inportb(_z_,_x_),										\
							 Debug(("inportb(NULL, %x)=%x\n",(char *)_x_-(char *)pSmbIOReg,_y_)),	\
							 _y_														\
							)
static unsigned char _y_;

static unsigned char _y_;
#else
#define Debug(x)
#define outportByte(_z_,_x_,_y_)	outportb(_z_,_x_,_y_ )
#define inportByte( _z_, _x_)		inportb(_z_,_x_)
#endif


unsigned long M1543CI2cTransaction(
struct FILE *fp,
unsigned int read,
unsigned long node,
unsigned long offset,
unsigned long size,
unsigned char *data)
{
	static int resetController = TRUE;
	static int success = 0;
	unsigned long index;
	unsigned long timeOut;
	SMBSTATUS status;
	PSMBIOREG pSmbIOReg;

	Debug(("I2cTransaction(%x,%x,%x,%x,*%x)\n",read,node,offset,size,data[0]));

	pSmbIOReg = IIcEnabled();

	if (pSmbIOReg == NULL)
	{
		return(FALSE);											/* Fault condition. */
	}

	if (resetController)
	{
		resetController = FALSE;
		Debug(("Reset Device\n"));

		outportByte(NULL,&(pSmbIOReg->smbCmd),0x04);			/* Reset host controller. */
		DELAY(50);												/* Wait quite a long time */
		M7101PmuWriteConfig(OFFSETOF(M7101_PMU_CONFIG,SMBHSI),M7101PmuReadConfig(OFFSETOF(M7101_PMU_CONFIG,SMBHSI),sizeof(unsigned char))|0x01,sizeof(unsigned char));					/* Enable SMB Bus Controller */
		M7101PmuWriteConfig(OFFSETOF(M7101_PMU_CONFIG,SMBHCBC),(M7101PmuReadConfig(OFFSETOF(M7101_PMU_CONFIG,SMBHCBC),sizeof(unsigned char)) & ~0xf8) | 0x20,sizeof(unsigned char));	/* Set speed to 74K with IDLE detect as BaseClk*64. */
	}

	Debug(("pSmbIOReg=%x\n",pSmbIOReg));

	outportByte(NULL,&(pSmbIOReg->smbSts),0xff);				/* Clear status. */

	DELAY(2);

	for (timeOut = 0,status.AsUCHAR = 0;
		(status.u.idleStatus == 0) && (timeOut < TIMEOUTVALUE);
		timeOut++)												/* Wait for Bus Idle. */
	{
		DELAY(1);
		status.AsUCHAR = inportByte(NULL, &(pSmbIOReg->smbSts));	/* Read status.u. */
	}

	Debug(("status1=%x %x\n",status.AsUCHAR,timeOut));

	if (timeOut == TIMEOUTVALUE)
	{
		Debug(("Timeout1\n"));
		resetController = TRUE;									/* Reset next time in. */
		return(FALSE);											/* Fault condition. */
	}

	outportByte(NULL,&(pSmbIOReg->address),node|(read?0x01:0x00));	/* Set address. */

	if (offset == (unsigned long)-1)
	{
		outportByte(NULL,&(pSmbIOReg->smbCmd),0x10);			/* Set Send/Receive byte. */

		if (read == FALSE)
			outportByte(NULL,&(pSmbIOReg->cmdReg),data[0]);		/* Set first byte. */
	}
	else
	{
		if (size == 1)
			outportByte(NULL,&(pSmbIOReg->smbCmd),0x20);		/* Set Read/Write byte. */
		else
			outportByte(NULL,&(pSmbIOReg->smbCmd),0x30);		/* Set Read/Write word. */

		outportByte(NULL,&(pSmbIOReg->cmdReg),offset);			/* Set offset command. */

		if (read == FALSE)
		{
			outportByte(NULL,&(pSmbIOReg->dataA),data[0]);		/* Set first byte. */
	
			if (size == 2)
				outportByte(NULL,&(pSmbIOReg->dataB),data[1]);	/* Set second byte. */
		}
	}

	outportByte(NULL,&(pSmbIOReg->start),0xff);					/* Start. */

	for (timeOut = 0,status.AsUCHAR = 0;
		(status.u.smiStr == 0) && (timeOut < TIMEOUTVALUE);
		timeOut++)												/* Wait for command complete. */
	{
		DELAY(1);
		status.AsUCHAR = inportByte(NULL, &(pSmbIOReg->smbSts));/* Read status.u. */

		if (
		    (status.u.deviceErr) ||								/* Bus error. */
		    (status.u.collision)								/* Collision. */
		   )
		{
			Debug(("Collision/Error=%x\n",status.AsUCHAR));
			resetController = TRUE;								/* Reset next time in. */
			return(FALSE);										/* Fault condition. */
		}
	}

	Debug(("status2=%x %x\n",status.AsUCHAR,timeOut));

	if (timeOut == TIMEOUTVALUE)
	{
		resetController = TRUE;									/* Reset next time in. */
		return(FALSE);											/* Fault condition. */
	}

	if (read == TRUE)
	{
		data[0] = inportByte(NULL, &(pSmbIOReg->dataA));		/* Get first byte. */
		Debug(("data[0]=%x\n",data[0]));


		if (size == 2)
		{
			data[1] = inportByte(NULL, &(pSmbIOReg->dataB));	/* Get second byte. */
			Debug(("data[1]=%x\n",data[1]));
		}
	}

	Debug(("Success=%d\n",success++));

	return(TRUE);
}

PSMBIOREG IIcEnabled(void)
{
	PSMBIOREG pSmbIOReg;

	pSmbIOReg = (PSMBIOREG)(M7101PmuReadConfig(OFFSETOF(M7101_PMU_CONFIG,SBASMB),sizeof(pSmbIOReg)) & 0xfffffffc);	/* Get SMB I/O based address from index 14h-17h of PMU device (M7101) */

	if (pSmbIOReg == 0xfffffffc)
		pSmbIOReg = 0;

	Debug(("IIcEnabled=%x\n",pSmbIOReg));

	return(pSmbIOReg);
}
