/*++

Copyright (c) 1995  Digital Equipment Corporation

Module Name:

    m1543_iic.h

Abstract:

    Private definitions for the CMOSIRQ module.

Author:

    Chris Gearing              10-Jun-1999

  
Revision History:

    10-Jun-1999         Chris Gearing
        Created.

--*/

#ifndef _API_IIC_
#define _API_IIC_

/*
// Define the address of the operation control word 3, and the IRR write
// mask, in the platform. (Typically, this would be in the 82357.)
// This is used only for checking for the presence of a floppy interrupt.
*/
#define LM75_INT_OCW3			EisaIOQva(0xa0)

/*
// Define magic value to ask the interrupt controller to return
// the IRR interrupt wires through OCW3.
*/
#define LM75_INT_OCW3_IRR		0xa
#define LM75_INT_OCW3_IRQ		(0x01 << (SMB_INTERRUPT - 0x08))
#define LM75_END_OF_IRQ			0x20

unsigned int I2cTransaction(struct FILE *fp,unsigned int read,unsigned longnode,unsigned longoffset,unsigned longsize,unsigned char *data);

#endif
