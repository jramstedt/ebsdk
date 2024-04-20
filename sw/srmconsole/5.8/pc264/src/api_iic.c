/* file:    api_iic.c
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
#include "cp$src:platform_cpu.h"
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

#include "cp$src:iic_devices.h"
#include "cp$src:api_iic.h"
#include "cp$src:Nt_Types.h"

#include "cp$src:m1543c_def.h"
#include "cp$src:M1543c_SBridge.h"

/*External references*/
extern int null_procedure();       /* Null procedure   */

static int pca8550_read_write(struct FILE *fp,int read,int count,char *buf);
static int pcf8574_read_write(struct FILE *fp,int read,int count,char *buf);
static int lm75_read_write(struct FILE *fp,int read,int count,char *buf);
static int adm9240_read_write(struct FILE *fp,int read,int count,char *buf);
static int dimmspd_read_write(struct FILE *fp,int read,int count,char *buf);
static int systemIICEventOccured = FALSE;

#define DELAY(time)		krn$_sleep(time * 10);

#define I2CACTION(notResult,fp,read,node,offset,size,data)	if (!notResult)	notResult |= (I2cTransaction(fp,read,node,offset,size,data) != TRUE);

enum iic_actions {
	IICA_NOTHING = 0,
	IICA_INITIONCE = 1,
	IICA_INITALWAYS = 2,
};


struct RWINFO {
	int (* readWriteRoutine)(struct FILE *fp,int read,int count,char *buf);	/* The read write routine. */
	unsigned long node;														/* ID of item to read */
	enum iic_actions action;												/* Action to do before read */
};

struct IICCB {
	char *name;																/* Name by which it will be known. */
	struct RWINFO rwInfo;													/* Read write info. */
	struct SEMAPHORE avail_sem;												/* IIC Bus Node available Semaphore */
	int mode;																/* Possible modes. */
	int length;																/* Length of file */
};

static struct IICCB iiccb[] = {
#if SWORDFISH
	{"lm75_primary",		{lm75_read_write,LM_75_PRIMARY,				IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(LM75)},
	{"lm75_secondary",		{lm75_read_write,LM_75_SECONDARY,			IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(LM75)},
	{"adm9240_primary",		{adm9240_read_write,ADM_9240_PRIMARY,		IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(ADM9240)},
	{"adm9240_secondary",	{adm9240_read_write,ADM_9240_SECONDARY,		IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(ADM9240)},
	{"mb_pcf8574",			{pcf8574_read_write,PCF_8574_MOTHERBOARD,	IICA_INITIONCE},		{0},		ATTR$M_READ|ATTR$M_WRITE,	sizeof(PCF8574)},
	{"mb_pcf8574_secondary",{pcf8574_read_write,PCF_8574_MOTHERBOARDSEC,IICA_INITIONCE},		{0},		ATTR$M_READ|ATTR$M_WRITE,	sizeof(PCF8574)},
	{"cs20_psu",			{adm9240_read_write,ADM_9240_CS20PSU,		IICA_INITALWAYS},		{0},		ATTR$M_READ|ATTR$M_WRITE,	sizeof(ADM9240)},
#endif
#if SHARK
	{"pca8550",				{pca8550_read_write,PCF_8550_MOTHERBOARD,	IICA_INITIONCE},		{0},		ATTR$M_READ|ATTR$M_WRITE,	sizeof(PCA8550)},
	{"mb_pcf8574",			{pcf8574_read_write,PCF_8574_MOTHERBOARD,	IICA_INITIONCE},		{0},		ATTR$M_READ|ATTR$M_WRITE,	sizeof(PCF8574)},
	{"io_pcf8574",			{pcf8574_read_write,PCF_8574_IOBOARD,		IICA_INITIONCE},		{0},		ATTR$M_READ|ATTR$M_WRITE,	sizeof(PCF8574)},
	{"adm9240_primary",		{adm9240_read_write,ADM_9240_PRIMARY,		IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(ADM9240)},
	{"adm9240_secondary",	{adm9240_read_write,ADM_9240_SECONDARY,		IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(ADM9240)},
	{"adm9240_dimm",		{adm9240_read_write,ADM_9240_DIMMMODULE,	IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(ADM9240)},
	{"adm9240_riser",		{adm9240_read_write,ADM_9240_RISERCARD,		IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(ADM9240)},
	{"adm9240_psu",			{adm9240_read_write,ADM_9240_PSU,			IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(ADM9240)},
#endif
#if SHARK || SWORDFISH
	{"dimm0_spd",			{dimmspd_read_write,DIMM0_EEPROM,			IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(DIMMSPD)},
	{"dimm1_spd",			{dimmspd_read_write,DIMM1_EEPROM,			IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(DIMMSPD)},
	{"dimm2_spd",			{dimmspd_read_write,DIMM2_EEPROM,			IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(DIMMSPD)},
	{"dimm3_spd",			{dimmspd_read_write,DIMM3_EEPROM,			IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(DIMMSPD)},
	{"dimm4_spd",			{dimmspd_read_write,DIMM4_EEPROM,			IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(DIMMSPD)},
	{"dimm5_spd",			{dimmspd_read_write,DIMM5_EEPROM,			IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(DIMMSPD)},
	{"dimm6_spd",			{dimmspd_read_write,DIMM6_EEPROM,			IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(DIMMSPD)},
	{"dimm7_spd",			{dimmspd_read_write,DIMM7_EEPROM,			IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(DIMMSPD)}
#endif
#if NAUTILUS || GLXYTRAIN
	{"lm75",				{lm75_read_write,PROCESSOR0_TEMPERATURE_NODE,IICA_INITIONCE},		{0},		ATTR$M_READ|ATTR$M_WRITE,	sizeof(LM75)},
	{"adm9240",				{adm9240_read_write,ADM_9240_SMB_UNIT,		IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(ADM9240)}
#endif
#if TINOSA
	{"lm75",				{lm75_read_write,PROCESSOR0_TEMPERATURE_NODE,IICA_INITIONCE},		{0},		ATTR$M_READ|ATTR$M_WRITE,	sizeof(LM75)},
	{"adm9240",				{adm9240_read_write,ADM_9240_SMB_UNIT,		IICA_INITIONCE},		{0},		ATTR$M_READ,				sizeof(ADM9240)}
#endif
};

static int LM75Initialized = FALSE;
static struct SEMAPHORE iic_driver_sem;										/* IIC Bus available Semaphore */

/*
 *  Define the Available IIC bus nodes with addresses. 
 */

/*Routine definitions*/
int api_iic_close(struct FILE *fp);
int api_iic_init(void);
int api_iic_open(struct FILE *fp,char *file_name,char *next,char *mode);
int api_iic_write(struct FILE *fp,int size,int number,char *buf);
int api_iic_read(struct FILE *fp,int size,int number,char *buf);

/*Global variables*/
extern int ev_initing;

/* Setting this may cause a hang due to the UART set code (Shark) being called before the
   comport is initialized */
#define IIC_DEBUG 0

struct DDB api_iic_ddb = {
	"iic",			/* how this routine wants to be called	*/
	api_iic_read,	/* read routine							*/
	api_iic_write,	/* write routine						*/
	api_iic_open,	/* open routine							*/
	api_iic_close,	/* close routine						*/
	null_procedure,	/* name expansion routine				*/
	null_procedure,	/* delete routine						*/
	null_procedure,	/* create routine						*/
	null_procedure,	/* setmode								*/
	null_procedure,	/* validation routine					*/
	0,				/* class specific use					*/
	1,				/* allows information					*/
	0,				/* must be stacked						*/
	0,				/* is a flash update driver				*/
	0,				/* is a block device					*/
	0,				/* not seekable							*/
	0,				/* is an ethernet device				*/
	0,				/* is a filesystem driver				*/
};


/*+
 * ============================================================================
 * = api_iic_read - Read a block of data.                                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  Read in a block of characters from the IIC bus.
 *       Return the number of characters read in.
 *
 *  
 * FORM OF CALL:
 *  
 *  api_iic_read (fp,size,number,buf)
 *  
 * RETURNS:
 *
 *      s - Bytes transferred. Anything other than the amount requested
 *      indicates EOF or an error.
 *       
 * ARGUMENTS:
 *
 *  struct FILE *fp - Pointer to a file.
 *  int size - Number of bytes/block to be transferred.
 *  int number - Number of blocks to be transferred.
 *  char *buf - Pointer to the buffer the data is written to.
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

int api_iic_read(
struct FILE *fp,
int size,
int number,
char *buf)
{
	struct INODE *ip;           /*Pointer to Inode     */
	struct IICCB *iicbp;
	int count;

	/* Get a pointer to the port block */
	ip = fp->ip;
    iicbp = (struct IICCB*)ip->misc;

	count = max(min((size*number),ip->len[0]-*fp->offset),0);

	if (count)
	{
		count = iicbp->rwInfo.readWriteRoutine(fp,1,count,buf);
		*fp->offset += count;
	}

	return(count);
}

/*+
 * ============================================================================
 * = api_iic_write - Read a block of data.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  Write in a block of characters from the IIC bus.
 *       Return the number of characters read in.
 *
 *  
 * FORM OF CALL:
 *  
 *  api_iic_write (fp,size,number,buf)
 *  
 * RETURNS:
 *
 *      s - Bytes transferred. Anything other than the amount requested
 *      indicates EOF or an error.
 *       
 * ARGUMENTS:
 *
 *  struct FILE *fp - Pointer to a file.
 *  int size - Number of bytes/block to be transferred.
 *  int number - Number of blocks to be transferred.
 *  char *buf - Pointer to the buffer the data is read from.
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

int api_iic_write(
struct FILE *fp,
int size,
int number,
char *buf)
{
	struct INODE *ip;           /*Pointer to Inode     */
	struct IICCB *iicbp;
	int count;

	/* Get a pointer to the port block */
	ip = fp->ip;
    iicbp = (struct IICCB*)ip->misc;

	count = max(min((size*number),ip->len[0]-*fp->offset),0);

	if (count)
	{
		count = iicbp->rwInfo.readWriteRoutine(fp,0,count,buf);
		*fp->offset += count;
	}

	return(count);
}

/*+
 * ============================================================================
 * = api_iic_open - Open the IIC Bus device.                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Open the IIC Bus device.  Allow only one writer/reader.
 *  
 * FORM OF CALL:
 *  
 *  api_iic_open (fp)
 *  
 * RETURNS:
 *
 *  0 - Success
 *       
 * ARGUMENTS:
 *
 *  struct FILE *fp - File to be opened.
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/
int api_iic_open(
struct FILE *fp,
char *file_name,
char *next,
char *mode)
{
	struct INODE *ip;           /*Pointer to Inode     */
	struct IICCB *iicbp;

	/* Get a pointer to the port block */
	ip = fp->ip;
    iicbp = (struct IICCB*)ip->misc;

    krn$_wait(&iicbp->avail_sem);

    return(msg_success);
}


/*+
 * ============================================================================
 * = api_iic_close - Close the IIC Bus device.                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Close the IIC Bus device.
 *
 * FORM OF CALL:
 *  
 *  api_iic_close (fp)
 *  
 * RETURNS:
 *
 *  None
 *       
 * ARGUMENTS:
 *
 *  struct FILE *fp - File to be closed.
 *  
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

int api_iic_close (struct FILE *fp)
{
	struct IICCB *iicbp;

	iicbp = (struct IICCB*)fp->ip->misc;
	krn$_post(&iicbp->avail_sem);

	return msg_success;
}
    
/*+
 * ============================================================================
 * = api_iic_init - Initialize the IIC Bus driver.                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  Initialize the IIC Bus driver and sniffs the iic bus creating an INODE for each
 *  device it finds on the bus.  A list of all of the address which are analyzed can be
 *  found in IIC_NODE at the top of this file.
 *  
 * FORM OF CALL:
 *  
 *  api_iic_init ()
 *  
 * RETURNS:
 *
 *  None
 *       
 * ARGUMENTS:
 *
 *  None
 *
 * SIDE EFFECTS:
 *
 *
-*/


int api_iic_init(void)
{
    struct INODE *ip;
    struct IICCB *iicbp;						/* Pointer to the port block */ 
    char sname[MAX_NAME];
	int i;

	/* Initialize the Driver Semaphores and direction */
	DbgPrint(("iic_init initialize structures \n"));

	if (Swordfish())
		return(msg_success);

	for (i = 0; i < ARRAYSIZE(iiccb); i++)
	{
		/* Create an inode entry,thus making the device visible as a file. */
		allocinode (iiccb[i].name,1,&ip);
		ip->dva = & api_iic_ddb;
		ip->attr = iiccb[i].mode;
		ip->len[0] = iiccb[i].length;
		ip->alloc[0] = 0;

		/*Get the pointers to the port block*/
		ip->misc = &iiccb[i];
	    INODE_UNLOCK (ip);

		/* Make Semphore names and init semaphores */
		sprintf(sname,"%s_avail",iiccb[i].name);
		krn$_seminit(&iiccb[i].avail_sem,1,sname);	/* Node Available */
	}

	krn$_seminit(&iic_driver_sem,1,"iic_bus_avail");		/* Node Available */

    return(msg_success);
}

int temp_value_wr(
char *name,
struct EVNODE *ev)
{
	struct FILE *fp;
	LM75 lm75;
	int temp;

	temp = atoi(ev->value.string);

	DbgPrint(("LM75 Temperature %d\n",temp));

	lm75.config = 0x1a;
	lm75.hyst = temp - 5;
	lm75.os = temp;

    if (fp = fopen("lm75","w"))
	{
		if (fwrite(&lm75,1,sizeof(lm75),fp) == sizeof(lm75))
			LM75Initialized = TRUE;				/* Remember initialized. */

		fclose(fp);
	}

    if( ev_initing )
		return( msg_success );

    return( ev_sev( name,ev ) );
}

int validate_temp_value(int *value) 
{
    int temp;
	struct FILE *fp;
	LM75 lm75;

	DbgPrint(("Validating LM75 Temperature %s\n",*value));

    temp = atoi(*value);

    if (ev_initing)
	{
		if (fp = fopen("lm75","r"))
		{
			if (fread(&lm75,1,sizeof(lm75),fp) != sizeof(lm75))
				lm75.temp = 50;						/* Just a default */

			fclose(fp);
		}
	}
	else
	{
		lm75.temp = 40-10;						/* This means at startup minimum values is 40 */
	}

	/*
	 * Don't let user select threshold temperature unless 10 degrees C more
	 * than current ambient.. Maximum threshold value supported by is 127 
	 * degrees C.
	 */
    if (temp < (lm75.temp+10))
	{
		err_printf("Shutdown temp must be 10 or more degrees above current temp (%d degrees C)\n",lm75.temp);
		return(msg_failure);
    }

    if (temp > 127)
	{
		err_printf("Shutdown temp must be less than 128 degrees C\n");
		return(msg_failure);
    }

    return(msg_success);
}

static int pca8550_read_write(
struct FILE *fp,
int read,
int count,
char *buf)
{
	unsigned char tconf;
	unsigned long notResult = FALSE;
	PCA8550 pca8550;
	char *destSrcStart = ((char *)&pca8550)+*fp->offset;
	char *destSrcEnd = destSrcStart+count;
	unsigned long node;
	struct IICCB *iicbp;

	/* Get a pointer to the port block */
	iicbp = (struct IICCB*)(fp->ip)->misc;

	node = iicbp->rwInfo.node;

	memset(&pca8550,'\0',sizeof(pca8550));

	if (!read)
		memcpy(destSrcStart,buf,count);

	I2CACTION(notResult,fp,read,node,(unsigned long)-1,1,&pca8550);

	if (notResult)
	{
		DbgPrint(("PCA8550 Read/Write error\n"));
	}

	if (notResult)
		count = 0;

	if (read)
		memcpy(buf,destSrcStart,count);

	return(count);
}

static int pcf8574_read_write(
struct FILE *fp,
int read,
int count,
char *buf)
{
	unsigned char tconf;
	unsigned long notResult = FALSE;
	PCF8574 pcf8574;
	char *destSrcStart = ((char *)&pcf8574)+*fp->offset;
	char *destSrcEnd = destSrcStart+count;
	unsigned long node;
	struct IICCB *iicbp;

	/* Get a pointer to the port block */
	iicbp = (struct IICCB*)(fp->ip)->misc;

	node = iicbp->rwInfo.node;

	memset(&pcf8574,'\0',sizeof(pcf8574));

	if (!read)
		memcpy(destSrcStart,buf,count);

	I2CACTION(notResult,fp,read,node,(unsigned long)-1,1,&pcf8574.value);

	if (notResult)
	{
		DbgPrint(("PCF8574 Read/Write error\n"));
	}

	if (notResult)
		count = 0;

	if (read)
		memcpy(buf,destSrcStart,count);

	return(count);
}

static int lm75_read_write(
struct FILE *fp,
int read,
int count,
char *buf)
{
	unsigned char tconf;
	unsigned long notResult = FALSE;
	LM75 lm75;
	char *destSrcStart = ((char *)&lm75)+*fp->offset;
	char *destSrcEnd = destSrcStart+count;
	unsigned long node;
	struct IICCB *iicbp;

	/* Get a pointer to the port block */
	iicbp = (struct IICCB*)(fp->ip)->misc;

	node = iicbp->rwInfo.node;

	memset(&lm75,'\0',sizeof(lm75));

	if (!read)
		memcpy(destSrcStart,buf,count);

#if NAUTILUS
	tconf = 0x02;
	I2CACTION(notResult,fp,FALSE,I2C_MUX_CONTROLLER,(unsigned long)-1,1,&tconf);
#endif

	I2CACTION(notResult,fp,read,node,1,1,&lm75.config);

	if (read)
	{
		I2CACTION(notResult,fp,read,node,0,2,(PUCHAR)&lm75.temp);
		lm75.temp = lm75.temp & 0x00ff;
	}

	lm75.hyst = lm75.hyst & 0x00ff;
	I2CACTION(notResult,fp,read,node,2,2,(PUCHAR)&lm75.hyst);
	lm75.hyst = lm75.hyst & 0x00ff;

	lm75.os = lm75.os & 0x00ff;
	I2CACTION(notResult,fp,read,node,3,2,(PUCHAR)&lm75.os);
	lm75.os = lm75.os & 0x00ff;

	if (notResult)
	{
		DbgPrint(("LM75 Read/Write error\n"));
	}
	else
	{
		DbgPrint(("LM75 Temperature %d\n",lm75.temp));
	}

	if (notResult)
		count = 0;

	if (read)
		memcpy(buf,destSrcStart,count);

	return(count);
}

static int dimmspd_read_write(
struct FILE *fp,
int read,
int count,
char *buf)
{
	unsigned char tconf;
	unsigned long notResult = FALSE;
	DIMMSPD dimmSpd;
	char *destSrcStart = ((char *)&dimmSpd)+*fp->offset;
	char *destSrcEnd = destSrcStart+count;
	unsigned long node;
	struct IICCB *iicbp;

	/* Get a pointer to the port block */
	iicbp = (struct IICCB*)(fp->ip)->misc;

	node = iicbp->rwInfo.node;

	memset(&dimmSpd,'\0',sizeof(dimmSpd));

	if (read)
	{
		I2CACTION(notResult,fp,read,node,OFFSETOF(DIMMSPD,rowsOfAddresses),sizeof(dimmSpd.rowsOfAddresses),(PUCHAR)&dimmSpd.rowsOfAddresses);
		I2CACTION(notResult,fp,read,node,OFFSETOF(DIMMSPD,columnsOfAddresses),sizeof(dimmSpd.columnsOfAddresses),(PUCHAR)&dimmSpd.columnsOfAddresses);
		I2CACTION(notResult,fp,read,node,OFFSETOF(DIMMSPD,moduleBanks),sizeof(dimmSpd.moduleBanks),(PUCHAR)&dimmSpd.moduleBanks);
		I2CACTION(notResult,fp,read,node,OFFSETOF(DIMMSPD,dataWidth1),sizeof(dimmSpd.dataWidth1),(PUCHAR)&dimmSpd.dataWidth1);
		I2CACTION(notResult,fp,read,node,OFFSETOF(DIMMSPD,banksPerDram),sizeof(dimmSpd.banksPerDram),(PUCHAR)&dimmSpd.banksPerDram);
	}

	if (notResult)
		count = 0;

	if (read)
		memcpy(buf,destSrcStart,count);

	return(count);
}

static int adm9240_read_write(
struct FILE *fp,
int read,
int count,
char *buf)
{
	unsigned long status;
	unsigned long notResult = FALSE;
	unsigned long node;
	ADM9240 adm9240;
	struct IICCB *iicbp;

	/* Get a pointer to the port block */
    iicbp = (struct IICCB*)(fp->ip)->misc;

	node = iicbp->rwInfo.node;

	memset(&adm9240,'\0',sizeof(adm9240));

	if (
	    (iicbp->rwInfo.action == IICA_INITIONCE) ||
		(iicbp->rwInfo.action == IICA_INITALWAYS)
	   )
	{
		if (iicbp->rwInfo.action == IICA_INITIONCE)
			iicbp->rwInfo.action = IICA_NOTHING;

		adm9240.fanDivisor.vid = 0;
		adm9240.fanDivisor.fan1Div = 0;
		adm9240.fanDivisor.fan2Div = 0;
		adm9240.fan1Limit = 0xf8;
		adm9240.fan2Limit = 0xf8;
		adm9240.intMask1 = 0x3f;
		adm9240.intMask2 = 0x03;
		adm9240.configuration = 0x03;
		adm9240.tempConfiguration= 0x01;

		I2CACTION(notResult,fp,FALSE,node,OFFSETOF(ADM9240,fanDivisor),sizeof(adm9240.fanDivisor),&adm9240.fanDivisor);						/* Set fan divisor. */
		I2CACTION(notResult,fp,FALSE,node,OFFSETOF(ADM9240,fan1Limit),sizeof(adm9240.fan1Limit),&adm9240.fan1Limit);						/* Set fan1 Limit. */
		I2CACTION(notResult,fp,FALSE,node,OFFSETOF(ADM9240,fan2Limit),sizeof(adm9240.fan2Limit),&adm9240.fan2Limit);						/* Set fan2 Limit */
		I2CACTION(notResult,fp,FALSE,node,OFFSETOF(ADM9240,intMask1),sizeof(adm9240.intMask1),&adm9240.intMask1);							/* Mask all interrupts. */
		I2CACTION(notResult,fp,FALSE,node,OFFSETOF(ADM9240,intMask2),sizeof(adm9240.intMask2),&adm9240.intMask2);							/* Mask all interrupts. */
		I2CACTION(notResult,fp,FALSE,node,OFFSETOF(ADM9240,configuration),sizeof(adm9240.configuration),&adm9240.configuration);			/* Start device sampling. */
		I2CACTION(notResult,fp,FALSE,node,OFFSETOF(ADM9240,tempConfiguration),sizeof(adm9240.tempConfiguration),&adm9240.tempConfiguration);/* Set single interrupt mode. */
	
		krn$_micro_delay(1000*1000);																							/* Now wait so the device updates once */
	}

	if (read)
	{
		I2CACTION(notResult,fp,read,node,OFFSETOF(ADM9240,fan2),sizeof(adm9240.temperature),(PUCHAR)&adm9240.temperature);
		I2CACTION(notResult,fp,read,node,OFFSETOF(ADM9240,fan1),sizeof(adm9240.fan1),(PUCHAR)&adm9240.fan1);
		I2CACTION(notResult,fp,read,node,OFFSETOF(ADM9240,fan2),sizeof(adm9240.fan2),(PUCHAR)&adm9240.fan2);
		I2CACTION(notResult,fp,read,node,OFFSETOF(ADM9240,measured25),sizeof(adm9240.measured25),(PUCHAR)&adm9240.measured25);
		I2CACTION(notResult,fp,read,node,OFFSETOF(ADM9240,measuredVCCP1),sizeof(adm9240.measuredVCCP1),(PUCHAR)&adm9240.measuredVCCP1);
		I2CACTION(notResult,fp,read,node,OFFSETOF(ADM9240,measuredP33),sizeof(adm9240.measuredP33),(PUCHAR)&adm9240.measuredP33);
		I2CACTION(notResult,fp,read,node,OFFSETOF(ADM9240,measuredP5),sizeof(adm9240.measuredP5),(PUCHAR)&adm9240.measuredP5);
		I2CACTION(notResult,fp,read,node,OFFSETOF(ADM9240,measuredP12),sizeof(adm9240.measuredP12),(PUCHAR)&adm9240.measuredP12);
		I2CACTION(notResult,fp,read,node,OFFSETOF(ADM9240,measuredVCCP2),sizeof(adm9240.measuredVCCP2),(PUCHAR)&adm9240.measuredVCCP2);
		I2CACTION(notResult,fp,read,node,OFFSETOF(ADM9240,measuredVCCP2),sizeof(adm9240.measuredVCCP2),(PUCHAR)&adm9240.measuredVCCP2);
		I2CACTION(notResult,fp,read,node,OFFSETOF(ADM9240,intStatus1),sizeof(adm9240.intStatus1),(PUCHAR)&adm9240.intStatus1);
		I2CACTION(notResult,fp,read,node,OFFSETOF(ADM9240,intStatus2),sizeof(adm9240.intStatus2),(PUCHAR)&adm9240.intStatus2);
	}

	if (notResult)
	{
		DbgPrint(("ADM9240 Read/Write error\n"));
	}

	if (notResult)
		count = 0;

	memcpy(buf,((char *)&adm9240)+*fp->offset,count);

	return(count);
}

unsigned long I2cTransaction(
struct FILE *fp,
unsigned int read,
unsigned long node,
unsigned long offset,
unsigned long size,
unsigned char *data)
{
	static int initialized = 0;
	unsigned long result = FALSE;
	unsigned long index;
	unsigned long timeOut;

	krn$_wait(&iic_driver_sem);

#if SHARK || SWORDFISH
	if (node & CCHIP_BUS)											/* This means on the GPIO controlled I2C Bus */
		result = CChipI2cTransaction(fp,read,node & 0xff,offset,size,data);
#endif

#if SWORDFISH
	if (node & PCF8584_BUS)											/* This means on the PCF8584 controlled I2C Bus */
		result = PCF8584I2cTransaction(fp,read,node & 0xff,offset,size,data);
#endif

#if M1543C || M1535C
	/* This loop does the call twice always. this is a temp fix for chris's strange bug where the SMB returns the
	** result from the last call rather than this call. */
	if (node & M1543_BUS)
	{
		int retry;

		M1543CI2cTransaction(fp,read,node & 0xff,offset,size,data);

		for (retry = 0; (result == FALSE) && (retry < 10); retry++)
			result = M1543CI2cTransaction(fp,read,node & 0xff,offset,size,data);
	}
#endif

	krn$_post(&iic_driver_sem);

	return(result);
}

#ifdef PCF_8574_MOTHERBOARD
/*+
 * ============================================================================
 * = MB8574AndOr - Switch UART to be connect to COM1&2 rather than CPU.
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Switch UART to be connect to COM1&2 rather than CPU.
 *
 * FORM OF CALL:
 *
 *	SwitchUART(); 
 *
 * RETURNS:                          
 *
 *      Nothing
 *                          
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void MB8574AndOr(
unsigned char andValue,
unsigned char orValue)
{
	struct FILE *fp;
	PCF8574 pcf8574;
	unsigned long barValue;
	unsigned short commandValue;

	if (
		((fp = fopen("mb_pcf8574","r+")) != NULL) &&
		(fread(&pcf8574,1,sizeof(pcf8574), fp) == sizeof(pcf8574))
	   )
	{
		pcf8574.value &= andValue;
		pcf8574.value |= orValue;

		fseek(fp,0,SEEK_SET);
		if (fwrite(&pcf8574,1,sizeof(pcf8574), fp) != sizeof(pcf8574))
			printf("Error writing I2C device\n");

	}
	else
	{	/* Device not present yet so do it by hand!! This is used early on for the COM1 port toggle */
#if SHARK
		barValue = M7101PmuReadConfig(OFFSETOF(M7101_PMU_CONFIG,SBASMB),sizeof(barValue));
		commandValue = M7101PmuReadConfig(OFFSETOF(M7101_PMU_CONFIG,Command),sizeof(commandValue));

		M7101PmuWriteConfig(OFFSETOF(M7101_PMU_CONFIG,SBASMB),0x00004000,sizeof(unsigned long));	/* Get SMB I/O based address from index 14h-17h of PMU device (M7101) */	
		M7101PmuWriteConfig(OFFSETOF(M7101_PMU_CONFIG,Command),0x1,sizeof(unsigned short));			/* Enable read/write of IO */
#endif
		I2cTransaction(NULL,TRUE,PCF_8574_MOTHERBOARD,(unsigned long)-1,1,&pcf8574);

		pcf8574.value &= andValue;
		pcf8574.value |= orValue;
		I2cTransaction(NULL,FALSE,PCF_8574_MOTHERBOARD,(unsigned long)-1,1,&pcf8574);

#if SHARK
		M7101PmuWriteConfig(OFFSETOF(M7101_PMU_CONFIG,SBASMB),barValue,sizeof(barValue));			/* Replace value. */
		M7101PmuWriteConfig(OFFSETOF(M7101_PMU_CONFIG,Command),commandValue,sizeof(commandValue));	/* Enable read/write of IO */
#endif
	}

	if (fp)
		fclose(fp);

}
#endif
#if SHARK
#define FIRST_IRQ	44
#define LAST_IRQ	49

/*+                                                                           
 * ============================================================================
 * = iic_event_handler
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle interrupts from the iic devices.
 *
 *  Basically does a show_power then turns the box off.
 *
 * RETURNS:  
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      int vector - SCB vector 
 *
 * SIDE EFFECTS:
 *
 *      None
 *           
-*/

void iic_event_handler(int vector)
{            
	int irq;

	pprintf("System Event Occcured %d\n",vector-PCI_BASE_VECTOR);

	for (irq = FIRST_IRQ; irq <= LAST_IRQ; irq++)
		io_disable_interrupts(NULL, PCI_BASE_VECTOR+irq);

	systemIICEventOccured = TRUE;
}

/*+
 * ============================================================================
 * = api_iic_poll_rt - poll the IIC bus for pending interrupt                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  This routine polls for an overtemperature condition and acts appropriately.
 *  
 * FORM OF CALL:
 *  
 *  api_iic_poll_rt (iicpb)
 *  
 * RETURNS:
 *
 *  None
 *       
 * ARGUMENTS:
 *
 *  iicpb - pointer to the IIC driver port block
 *              
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

void api_iic_poll_rt(void )
{
	int irq;

	for (irq = FIRST_IRQ; irq <= LAST_IRQ; irq++)
		int_vector_set(PCI_BASE_VECTOR+irq, (int)iic_event_handler, PCI_BASE_VECTOR+irq, 0);

	while(1)
	{
		for (irq = FIRST_IRQ; irq <= LAST_IRQ; irq++)
			io_enable_interrupts(NULL, PCI_BASE_VECTOR+irq);

		while(systemIICEventOccured == FALSE)
			krn$_sleep(1 * 1000);						/* Take it easy. Go to sleep for 1 seconds. */

		systemIICEventOccured = FALSE;
		printf("IIC Event Occured System Power State Is\n");
		show_power(0, NULL);

		MB8574AndOr(~MB8574_PSUOUTPUTINHIBIT_RW,0);
		MB8574AndOr(~MB8574_PSUOUTPUTINHIBIT_RW,1);		/* Should turn the PSU off */
		krn$_sleep(30 * 1000);							/* Take it easy. Go to sleep for 30 seconds. We may not go off and don't want to loop like a girl */
	}
}

/*+
 * ============================================================================
 * = init_system_monitor - Initializes the system monitor.
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *   COMMAND TAG: init_system_monitor 0 RXBZ init_system_monitor
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	None
 *
 *   COMMAND OPTION(S):
 *
 * 	None
 *
 *   COMMAND EXAMPLE(S):
 *
 *      None
 *
 *   COMMAND REFERENCES:
 *  
 *      None
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

void init_system_monitor(int argc, char *argv[])
{
	static int started = FALSE;
	struct FILE *fp;
	int irq, device;
	unsigned char data;

	if (!started)
	{
		started = TRUE;

		for (irq = FIRST_IRQ; irq <= LAST_IRQ; irq++)
			io_disable_interrupts(NULL, PCI_BASE_VECTOR+irq);

		/* Read each device to init the state. */
		for (device = 0; device < ARRAYSIZE(iiccb); device++)
		{
			if (iiccb[device].name[0] == 'a')			/* basically monitor adm devices */
			{
				printf("Monitoring %s\n", iiccb[device].name);

				fp = fopen(iiccb[device].name,"r");

				if (fp)
				{
					fread(&data,1,1,fp);					/* A read will init the state. */
					fclose(fp);
				}
			}
		}

		/* Start a polling process */
		krn$_create(api_iic_poll_rt,null_procedure,0,1,-1,1024 * 12,
				  "iic_poll",
				  "tt","r",
				  "tt","w",
				  "tt","w",
				  NULL);
	}

	printf("System monitoring started\n");
}
#else
void init_system_monitor(int argc, char *argv[])
{
	printf("System monitoring not available on this platform\n");
}
#endif

int SwordFish()
{
#if SWORDFISH
	static int tested = 1234;
	struct FILE *fp;

	if (tested != 1234)
		return(tested);

	fp = fopen("mb_pcf8574__","r");

	if (fp)
		fclose(fp);

	tested = (fp == NULL);

	return(tested);
#else
	return(FALSE);
#endif
}
