/*
 * file:        lfu_kzpsa_driver.c
 *
 * Copyright (C) 1994 by
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
 *
 * Abstract:    LFU Driver for the KZPSA adapter.
 *
 *	This driver will update firmware on the KZPSA (PCI to SCSI) adapter.
 *
 * Author:      William Clemence
 *
 * Modifications:
 *
 *       wcc     22-Nov-1994	First release.
 *
*/

/*
 * All include files here
*/
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:mem_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:ctype.h"
#include "cp$inc:platform_io.h"	
#include "cp$inc:kernel_entry.h"
#include "cp$src:pb_def.h"
#include "cp$src:pks_flash_def.h"
#include "cp$src:nport_q_def.h"
#include "cp$src:nport_def.h"

/*
 * An auxillary structure that hangs off the inode for the driver
 * to keep track of private buffers.
 */
struct FLASH {
    struct SEMAPHORE sync;		/* serialize access */
    struct pb *pb;
    int mem_address;			/* memory base address of kzpsa */
    int OldImagePtr;			/* flash base address of kzpsa */
};

#include "cp$inc:prototypes.h"

static int FlashErase(struct FLASH *flp, int FAddr, int *FStat, int mask);
static int FlashWrite(struct FLASH *flp, int FAddr, int FData, int *FStat, int mask);

/*
 *  All definitions defined here
*/


/* define the pci kzpsa device type if it hasn't been allready. */
#ifndef PCI$K_DTYPE_DEC_KZPSA
#define PCI$K_DTYPE_DEC_KZPSA	0x00081011
#endif

#define debug_print 0
#if debug_print
#define dprintf printf
#else
#define dprintf
#endif


/* Retry counts and timeouts                                                  */
#define PZA_FLASH_TIMEOUT   		6000
					/* > 5 seconds */

#define FLASH_REV_OFFSET (64 * 1024)

#define FLASH_UNLOCK    0xC0DEC0DE
#define FLASH_LOCK      0x0
#define FLASH_VERIFY    0xBEEFBEEF

#define FLASH_LOCK_OFFSET   0x01B0

#define KZPSA_READ_ROM(flp, a) inmeml(flp->pb, a)
#define KZPSA_WRITE_ROM(flp, a, d) outmeml(flp->pb, a, d)

#define PZA_FLASH_UNLOCK(flp) (outmeml(flp->pb, flp->mem_address + \
  FLASH_LOCK_OFFSET, FLASH_UNLOCK))
#define PZA_FLASH_LOCK(flp) (outmeml(flp->pb, flp->mem_address + \
  FLASH_LOCK_OFFSET, FLASH_LOCK))
#define PZA_FLASH_VERIFY(flp) (FLASH_VERIFY == inmeml( \
  flp->pb, flp->mem_address + FLASH_LOCK_OFFSET))


/*
*  All variables declared here
*/



/*
*  All routines declared here
*/
extern void null_procedure();

static unsigned long CalculateChecksum(unsigned long *AddrPtr, int Length);
static int FlashVerify(int *FlashBaseAddr);

int lfu_kzpsa_init(void);
static int kzpsa_inquire(char *fw, char *fname, struct FILE *fp);
static int kzpsa_validate(int command, char *new_name, char *fw,
  char *firm_data, int firm_size, struct FILE *fp);
static int kzpsa_read(struct FILE *fp, int size, int number, char *buf);
static int kzpsa_write(struct FILE *fp, int size, int number, char *buf);
static int kzpsa_open(struct FILE *fp, char *info, char *next, char *mode);
static int kzpsa_close(struct FILE *fp);

/* ddb data base.... */
static struct DDB kzpsa_ddb = {"", 	/* how this wants to be called  */
  kzpsa_read, 				/* read routine                 */
  kzpsa_write, 				/* write routine                */
  kzpsa_open, 				/* open routine                 */
  kzpsa_close, 				/* close routine                */
  null_procedure, 			/* name expansion routine       */
  null_procedure, 			/* delete routine               */
  null_procedure, 			/* create routine               */
  null_procedure, 			/* setmode                      */
  kzpsa_validate, 			/* validation routine           */
  kzpsa_inquire, 			/* class specific use           */
  0, 					/* allows information           */
  0, 					/* must be stacked              */
  1, 					/* is a flash update driver     */
  0, 					/* is a block device            */
  0, 					/* not seekable                 */
  0, 					/* is an ethernet device        */
  0, 					/* is a filesystem driver       */
  };

static struct DDB kzpsafw_ddb = {"", 	/* how this wants to be called  */
  kzpsa_read, 				/* read routine                 */
  kzpsa_write, 				/* write routine                */
  kzpsa_open, 				/* open routine                 */
  kzpsa_close, 				/* close routine                */
  null_procedure, 			/* name expansion routine       */
  null_procedure, 			/* delete routine               */
  null_procedure, 			/* create routine               */
  null_procedure, 			/* setmode                      */
  kzpsa_validate, 			/* validation routine           */
  kzpsa_inquire, 			/* class specific use           */
  0, 					/* allows information           */
  0, 					/* must be stacked              */
  0, 					/* is a flash update driver     */
  0, 					/* is a block device            */
  0, 					/* not seekable                 */
  0, 					/* is an ethernet device        */
  0, 					/* is a filesystem driver       */
  };


/*+
 * ============================================================================
 * = lfu_kzpsa_init - Initalize driver					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Initalizes the driver, by creating all the inodes needed for all kzpsa
 * adapters in the system.
 *  
 * FORM OF CALL:
 *  
 *	lfu_kzpsa_init()
 *  
 * RETURN CODES:
 *
 *      msg_success
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
int lfu_kzpsa_init(void) 
{
    struct INODE *ip;
    struct FLASH *flp;
    struct device *dev;
    int devcnt, i, k;
    struct device **devptr;
    char name[20];

    /* Find all the kzpsa devices available */
    devcnt = lfu_find_all_dev(PCI$K_DTYPE_DEC_KZPSA, 0, 0, NULL);
    if (devcnt) {
	devptr = malloc(devcnt * sizeof(struct device *));
	lfu_find_all_dev(PCI$K_DTYPE_DEC_KZPSA, 0, devcnt, devptr);
    }
    for (i = 0; i < devcnt; i++) {
	dev = devptr[i];

	sprintf(name, "%s%d_flash", dev->tbl->mnemonic, dev->unit);

	allocinode(name, 1, &ip);

	ip->dva = &kzpsa_ddb;

	/* define the maximimum image size and amount permently allocated... */
	ip->len[0] = FLASH_IMAGE_SIZE;

	ip->attr = ATTR$M_READ;

	/* Permanently allocate an auxillary structure for the flash device */
	flp = ip->misc =
	  dyn$_malloc(sizeof(struct FLASH), DYN$K_SYNC | DYN$K_NOOWN);
	krn$_seminit(&flp->sync, 1, "kzpsasync");
	INODE_UNLOCK(ip);

	/* create a port block for this adapter. */
	flp->pb = dyn$_malloc(sizeof(struct pb), DYN$K_SYNC | DYN$K_NOOWN);
	flp->pb->hose = dev->hose;
	flp->pb->slot = dev->slot;
	flp->pb->bus = dev->bus;
	flp->pb->function = dev->function;

	/* memory base address of kzpsa, offset 0x10 in pci cnfg space */
	flp->mem_address = (incfgl(flp->pb, 0x10) & 0xFFFFFFF0);

	/* flash base address of kzpsa, offset 0x1c in pci cnfg space */
	flp->OldImagePtr = (incfgl(flp->pb, 0x1c) & 0xFFFFFFF0);


    }					/* next device */

    if (devcnt) {
	free(devptr);
    } else {
	allocinode("zzzzzzfw_kzpsa_fw", 1, &ip);

	ip->dva = &kzpsafw_ddb;

	ip->attr = ATTR$M_READ;

	/* Permanently allocate an auxillary structure for the flash device */
	flp = ip->misc =
	  dyn$_malloc(sizeof(struct FLASH), DYN$K_SYNC | DYN$K_NOOWN);
	krn$_seminit(&flp->sync, 1, "kzpsasync");

	INODE_UNLOCK(ip);
    }

    return (msg_success);
}

/*+
 * ============================================================================
 * = kzpsa_open - open the flash rom					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Opens up the kzpsa flash rom for read or write operations. 
 *
 *				 NOTE 
 * 	The driver allows for only one reader/writer at a time.
 *  
 * FORM OF CALL:
 *  
 *	kzpsa_open(struct FILE *fp, char *info, char *next, char *mode)
 *  
 * RETURN CODES:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *      char *info      - offset information into the file (note usually zero);
 *      char *next      - Ignored, next driver down info.
 *      char *mode      - Driver mode. Ignored here.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static int kzpsa_open(struct FILE *fp, char *info, char *next, char *mode)
{

    struct FLASH *flp;

    flp = fp->ip->misc;

    /* We only allow one process to access the device at a time. */
    krn$_wait(&flp->sync);

    /* If the information field is present, treat it as an offset into the
     * flash roms. */
    *fp->offset = xtoi(info);

    return msg_success;
}

/*+
 * ============================================================================
 * = kzpsa_close - close the flash rom					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Close the flash rom.
 *
 *  
 * FORM OF CALL:
 *  
 *	kzpsa_close(struct FILE *fp)
 *  
 * RETURN CODES:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static int kzpsa_close(struct FILE *fp)
{

    struct FLASH *flp;

    flp = fp->ip->misc;

    /* We only allow one process to access the devices at a time. */
    krn$_post(&flp->sync);

    return msg_success;
}

/*+
 * ============================================================================
 * = kzpsa_inquire - Inquire about firmware revision/name		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Get the firmware rev of the module, also reports back the
 * recommended firmware filename.
 *  
 * FORM OF CALL:
 *  
 *	kzpsa_inquire (char *fw, char *fname, struct FILE *fp)
 *  
 * RETURN CODES:
 *
 *      msg_success or msg_failure
 *       
 * ARGUMENTS:
 *
 *	char    *fw 	- firmware rev string
 *	char 	*fname  - suggested firmware name string
 *	struct FILE *fp	- File pointer.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static int kzpsa_inquire(char *fw, char *fname, struct FILE *fp)
{
    struct FLASH *flp;
    char str[9];
    int i, j;

    flp = fp->ip->misc;

    /* just read the rev. */
    *(int *) (str) = KZPSA_READ_ROM(flp, flp->OldImagePtr + FLASH_REV_OFFSET);
    *(int *) (str + 4) =
      KZPSA_READ_ROM(flp, flp->OldImagePtr + FLASH_REV_OFFSET + 4);

    str[8] = 0;
    sprintf(fw, "%s", str);

    /* recommended firmware file */
    strcpy(fname, "kzpsa_fw");

    return (msg_success);

}

/*+
 * ============================================================================
 * = kzpsa_validate - validate or verify fimrware			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Validate the first file and return the firmware rev if good or
 * verify the two files.
 *  
 * FORM OF CALL:
 *  
 *	kzpsa_validate(int command, char *new_name, char *fw,
 *			char *firm_data, int firm_size, struct FILE *fp)
 *  
 * RETURN CODES:
 *
 *      msg_success
 *	msg_failure
 *	2                 - firmware data is older rev than hardware
 *       
 * ARGUMENTS:
 *
 *	int command  	  - command field,
 *                          0 = validate file
 *                          1 = verify the file
 *                          2 = validate file no new filename
 *	char  *new_name   - new filename if required on validate
 *	char    *fw       - firmware rev of firmware on validate
 *	char *firm_data   - Pointer to the firmware data.
 *	int firm_size     - Firmware data size.
 *	struct FILE *fp   - file pointer of the selected device.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static int kzpsa_validate(int command, char *new_name, char *fw,
  char *firm_data, int firm_size, struct FILE *fp)
{
    int i, j, k, l;
    int len;
    unsigned char *data;
    unsigned char *fdata;
    struct FLASH *flp;
    char imgfw_str[9];
    char fw_str[9];
    unsigned int temp_data;
    int size;
    int status;

    flp = fp->ip->misc;

    /* default the return to failure. This covers bad commands */
    status = msg_failure;


    /* validate file. */
    if (command == 0 || command == 2) {

	/* Check the size and some data  */
	if ((firm_size == FLASH_IMAGE_SIZE) &&
	  (FlashVerify(firm_data) == msg_success)) {
	    strncpy(imgfw_str, firm_data + FLASH_REV_OFFSET, 8);
	    imgfw_str[8] = 0;

	    sprintf(fw, "%s", imgfw_str);

	    status = msg_success;

	    if (command == 0) {
		/* Get the fw rev of the hardware to see if it is newer */
		*(int *) (fw_str) =
		  KZPSA_READ_ROM(flp, flp->OldImagePtr + FLASH_REV_OFFSET);
		*(int *) (fw_str + 4) =
		  KZPSA_READ_ROM(flp, flp->OldImagePtr + FLASH_REV_OFFSET + 4);
                fw_str[8]=0;
		
		/* strip off all non-meaningful characters */
		for (i = j = 0; i < 8; i++) {
		    if (isgraph(imgfw_str[i]))
			imgfw_str[j++] = imgfw_str[i];
		}
		imgfw_str[j] = 0;
		for (i = j = 0; i < 8; i++) {
		    if (isgraph(fw_str[i]))
			fw_str[j++] = fw_str[i];
		}
		fw_str[j] = 0;

		if (strcmp_nocase(fw_str, imgfw_str) == 1)
		    status = 2;		/* return 2; fimrware is older */
	    }

	} else
	    status = msg_failure;
    }

    /* verify file */
    if (command == 1) {


	/* ensure pointer is unsigned */
	fdata = firm_data;
	data = malloc(2048);
	k = 0;
	l = 0;

	/* Set size to the end of the read size or total requested */
	size = min(FLASH_IMAGE_SIZE, firm_size);

	/* start the comparing. */
	i = 0;
	len = 1;			/* bogus but a real read will set me
					 * straight. */
	while (i < size && len > 0) {
	    len = fread(data, 2048, 1, fp);
	    for (j = 0;(j < len) && (i < size); j++, i++) {
		if (*(data + j) != *(fdata + i)) {
		    k = 1;
		    break;
		}
	    }

	    if (k)
		break;
	}
	free(data);

	if (k == 0)
	    status = msg_success;
	else
	    status = msg_failure;
    }

    /* return status */
    return (status);

}

/*+
 * ============================================================================
 * = kzpsa_read - read data from the roms				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Reads from the roms.  This is normally only used by LFU via this driver's
 * validate routine to read the rom.
 *  
 * FORM OF CALL:
 *  
 *	kzpsa_read( fp, size, number, buf)
 *  
 * RETURN CODES:
 *
 *      number of bytes transfered
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *	int size	- size of item
 *	int number	- number of items
 *      char *buf       - address of buffer to read into
-*/
static int kzpsa_read(struct FILE *fp, int size, int number, char *buf)
{
    int i, base;
    int *data = buf;
    int len;
    struct FLASH *flp;

    flp = fp->ip->misc;

    /* adjust the read byte length to either: The end of the read size or total
     * requested; also make it longword aligned */
    len = min(FLASH_IMAGE_SIZE - *fp->offset, size * number);
    len -= len % 4;

    /* read the flashrom */
    base = flp->OldImagePtr + *fp->offset;
    for (i = 0; i < len; i += 4) {
	*(data) = KZPSA_READ_ROM(flp, base + i);
	data++;
    }
    *fp->offset += i;

    /* return the byte count of all the locations read. */
    return (i);
}

/*+
 * ============================================================================
 * = kzpsa_write - write a block of characters to the flash rom		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Write a block of characters to the flash rom.  This code actually 
 * performs the write  to the rom. It requires the user to supply all of image
 * in one write.
 *  
 * FORM OF CALL:
 *  
 *	kzpsa_write( fp, size, number, buf)
 *  
 * RETURN CODES:
 *
 *      number of bytes transfered or 0 for errors.
 *
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *	int size	- size of item
 *	int number	- number of items
 *      char *buf       - address of buffer to write characters from
 *
 * SIDE EFFECTS:
 *
-*/
static int kzpsa_write(struct FILE *fp, int size, int number, char *buf)
{
    int i, j;
    int len = (size * number);
    struct FLASH *flp;
    unsigned long *NewImagePtr = buf;
    int flash_timeout;
    int Status;
    int FlashStatus;
    int bank_mask, offset;


    flp = fp->ip->misc;

    /* complain if it isn't the whole thing... */
    if (len != FLASH_IMAGE_SIZE || *fp->offset != 0)
	return 0;

    /* The flash needs to be unlocked before it can be updated.  This is done
     * by writing one of our simport memory registers. */

    /* unlock the flash */
    PZA_FLASH_UNLOCK(flp);
    flash_timeout = PZA_FLASH_TIMEOUT;
    while ((!(PZA_FLASH_VERIFY(flp))) && --flash_timeout) {
	krn$_sleep(1);
    }

    if (0 == flash_timeout) {

	/* display message stating timeout occured but still continue */
	dprintf("PZA_FLASH_TIMEOUT on PZA_FLASH_UNLOCK\n");
    }

    /* erase 1st block of flash */
    Status = FlashErase(flp, (flp->OldImagePtr + FLASH_BLOCK_0), &FlashStatus,
      FLASH_BANK_M_ALL);
    if (Status == msg_success) {

	/* if first block erase successfully erase the 2nd */
	Status = FlashErase(flp, (flp->OldImagePtr + FLASH_BLOCK_1),
	  &FlashStatus, FLASH_BANK_M_ALL);
    }

    /* If the erase worked then go on to writing the data */
    if (Status == msg_success) {

	for (offset = 0;(offset < len) && (Status == msg_success);
	  offset += 4) {

	    bank_mask = 0xff;
	    for (j = 0;(j < 4) && (Status == msg_success); j++) {

		/* write this bank */
		Status = FlashWrite(flp, flp->OldImagePtr + offset,
		  *(long *) (buf + offset), &FlashStatus, bank_mask);

		/* next bank */
		if (Status == msg_success)
		    bank_mask <<= 8;
	    }
	}
    }

    /* lock up the flash again */
    PZA_FLASH_LOCK(flp);
    flash_timeout = PZA_FLASH_TIMEOUT;
    while ((!(PZA_FLASH_VERIFY(flp))) && --flash_timeout) {
	krn$_sleep(1);
    }
    if (0 == flash_timeout) {

	/* display message stating timeout occured but still continue */
	dprintf("PZA_FLASH_TIMEOUT on PZA_FLASH_LOCK\n");
    }


    if (Status != msg_success)
	len = 0;			/* failure */

    return len;
}

/*+
 * ============================================================================
 * = FlashErase - Erase flash routine					      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This function will erase the flash rom on KZPSA.
 *
 * FORM OF CALL:
 *
 *       int FlashErase (flp, FAddr, FStat, mask)
 *
 * RETURNS:
 *
 *      status 
 *
 * ARGUMENTS:
 *
 *	struct FLASH *flp	- pointer to local inode info.
 *	int FAddr		- address to erase.
 *      int *FStat		- error status passed back (debug only)
 *	int mask		- mask to tell me which from to send command to.
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
-*/
static int FlashErase(struct FLASH *flp, int FAddr, int *FStat, int mask)
{
    int FlashStat;
    int status = msg_success;
    int Retries = MAX_FLASH_RETRY;
    int Timer;

    dprintf("Called FlashErase for flash %x\n", FAddr);

    do {

	/***  NOTE - mask is ANDed with constants by definition for PZA */

	KZPSA_WRITE_ROM(flp, FAddr, FLASH_SETUP_ERASE);
	KZPSA_WRITE_ROM(flp, FAddr, FLASH_ERASE);
	KZPSA_WRITE_ROM(flp, FAddr, FLASH_READ_STATUS);
	Timer = MAX_FLASH_ERASE;
	do {
	    FlashStat = KZPSA_READ_ROM(flp, FAddr);
	} while ((FLASH_OP_CMP != (FlashStat & FLASH_OP_CMP)) && (--Timer));

	if ((Timer == 0) ||
	  (FLASH_VPP_ERROR == (FlashStat & FLASH_VPP_ERROR)) ||
	  (FLASH_CMD_SEQ_ERROR == (FlashStat & FLASH_CMD_SEQ_ERROR)) ||
	  (FLASH_ERASE_ERROR == (FlashStat & FLASH_ERASE_ERROR))) {
	    *FStat = FlashStat;
	    status = msg_failure;
	    --Retries;
	} else {
	    status = msg_success;
	}

	KZPSA_WRITE_ROM(flp, FAddr, FLASH_CLEAR_STATUS);
    } while ((status == msg_failure) && (Retries));

    KZPSA_WRITE_ROM(flp, FAddr, FLASH_READ_ARRAY);
    return (status);
}
;

/*+
 * ============================================================================
 * = FlashWrite - Write flash routine					      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This function will write the flash rom on KZPSA.
 *
 *     set FlashAddr to FLASH_SETUP_PROGRAM
 *     set FlashAddr to FlashData
 *     set FlashBase to FlashAddr AND FLASH_ADDR_MASK
 *     set @FlashBase to FLASH_READ_STATUS_CMD
 *     set Timer to 0
 *     set Retries to 0
 *     set status to FAILED
 *     DO WHILE (status is FAILED) AND (Retries < MAX_RETRIES)
 *         set FlashStat to @FlashBase
 *         DO WHILE (FlashStat AND BIT07 is ZERO) OR (Timer is TIMEOUT)
 *             increment Timer
 *             set FlashStat to @FlashBase
 *         ENDDO
 *         IF Timer equals TIMEOUT
 *             set status to FAILED
 *             set FlashBase to FLASH_CLEAR_STATUS_CMD
 *             set *FlashStatus to FlashStat
 *             increment Retries
 *         ELSEIF ((FlashAddr AND BIT04 is ONE)
 *             set status to FAILED
 *             set FlashBase to FLASH_CLEAR_STATUS_CMD
 *             set *FlashStatus to FlashStat
 *             increment Retries
 *         ELSE
 *             set status to msg_success
 *         ENDIF
 *     set FlashBase to FLASH_NORMAL_MODE
 *     return(status)
 *
 * FORM OF CALL:
 *
 *	int FlashWrite (flp, FAddr, FData, FStat, mask)
 * RETURNS:
 *
 *      status 
 *
 * ARGUMENTS:
 *
 *	struct FLASH *flp	- pointer to local inode info.
 *      int FAddr		- address to write
 *      int FData		- data to write
 *      int *FStat		- error status passed back (debug only)
 *	int mask		- mask 
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
-*/
static int FlashWrite(struct FLASH *flp, int FAddr, int FData, int *FStat, int mask)
{
    int FlashStat;
    int FBase = (FAddr & FLASH_ADDR_MASK);
    int status = msg_success;
    int Retries = MAX_FLASH_RETRY;
    int Timer;

    do {

	KZPSA_WRITE_ROM(flp, FAddr, FLASH_SETUP_PROGRAM);
	KZPSA_WRITE_ROM(flp, FAddr, FData & mask);
	KZPSA_WRITE_ROM(flp, FAddr, FLASH_READ_STATUS);

	Timer = MAX_FLASH_BYTE_PROGRAM;
	do {
	    FlashStat = KZPSA_READ_ROM(flp, FBase);
	} while ((FLASH_OP_CMP != (FlashStat & FLASH_OP_CMP)) && (--Timer));

	if ((Timer == 0) ||
	  (FlashStat & (FLASH_VPP_ERROR + FLASH_PROG_ERROR))) {
	    *FStat = FlashStat;
	    status = msg_failure;
	    --Retries;
	} else {
	    status = msg_success;
	}
	;

	KZPSA_WRITE_ROM(flp, FAddr, FLASH_CLEAR_STATUS);
    } while ((status == msg_failure) && (Retries));

    KZPSA_WRITE_ROM(flp, FBase, FLASH_READ_ARRAY);
    return (status);
}




/*******************************************************************************
** Calculate checksum
**
**  This routine calculates the checksum using the adapted Rigel-style
**  algorithm described in the Product Support Data Logging Standards & 
**  Guidelines, revision 1.2.
**
*******************************************************************************
** INPUTS
**
** AddrPtr - Pointer to start of region to be checksummed.
**
** Length  - The number of longwords in region.
**
*******************************************************************************
** OUTPUTS
**
** CheckSum - The checksum calculated for region.
**
*******************************************************************************/
/* needs header */
static unsigned long CalculateChecksum(unsigned long *AddrPtr, int Length)
{
    unsigned long i;
    unsigned long CheckSum = 0;
    unsigned long CheckSumTemp = 0;


    for (i = 0; i < Length; i++) {
	CheckSum += AddrPtr[i];

	if ((CheckSum < CheckSumTemp) || (CheckSum < AddrPtr[i])) {
	    if (!CheckSum++)
		CheckSum++;
	}

	CheckSumTemp = CheckSum >> 1;
	if (CheckSum ^ (CheckSumTemp << 1)) {
	    CheckSumTemp += 0x80000000;	/* BIT31 */
	}

	CheckSum = CheckSumTemp;
    }					/* EndFor */

    /* return the complement of the CheckSum                              */
    return (~CheckSum);
}					/* end CalculateChecksum               
					 * 
					 *                                   */

/* needs header */
static int FlashVerify(int *FlashBaseAddr)
{
    unsigned long checksum;
    unsigned long FlashLen = (FLASH_IMAGE_SIZE - sizeof(long)) / sizeof(long);

    checksum = CalculateChecksum((unsigned long *) FlashBaseAddr, FlashLen);



    if (checksum == *(((unsigned long *) FlashBaseAddr) + FlashLen)) {
	return (msg_success);
    } else {
	return (msg_failure);
    }
}

