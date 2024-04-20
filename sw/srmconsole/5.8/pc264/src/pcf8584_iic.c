/* file:    pcf8584_iic.c
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
 *      API Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *  IIC bus driver for API firmware.
 *
 *  AUTHORS:
 *
 *  David Baird
 *
 *  CREATION DATE:
 *  
 *
 *  MODIFICATION HISTORY:
 * 
 *
 *--
 */

/*Include files*/
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:prdef.h"
#include "cp$src:kernel_def.h"


#define DEBUG	0

#if DEBUG
#define dprintf(x)		qprintf x
#else
#define dprintf(x)
#endif

/*External references*/
                                                               
/*
 *  Define the Available IIC bus nodes with addresses. 
 */


unsigned long PCF8584I2cTransaction(
struct FILE *fp,
unsigned int read,
unsigned long node,
unsigned long offset,
unsigned long size,
unsigned char *data)
{
	char filename[32];
	struct FILE *file;
	char offsetChar = offset;
	unsigned long byteRW;

	sprintf(filename,"%s__",fp->ip->name);

	dprintf(("IIC read(%x) of %s\n",read,filename));

	file = fopen(filename,"r+");
	dprintf(("fopen(filename)=%x\n",file));

	if (file)
	{
		if (offset != -1)
			fseek(file,offset,SEEK_SET);

		if (read)
		{
			dprintf(("fread\n"));
			byteRW = fread(data,1,size,file);
			dprintf(("Data %x %x %x %x\n",size,byteRW,data[0],data[1]));
		}
		else
		{
			dprintf(("fwrite\n"));
			byteRW = fwrite(data,1,size,file);
		}

		fclose(file);
	}

	return(byteRW == size);
}
