/*
 * file:        lfu_cipca_driver.c
 *
 * Copyright (C) 1995 by
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
 * Abstract:    LFU Driver for the CIPCA adapter.
 *
 *	This driver will update firmware on the CIPCA (PCI to CI) adapter.
 *
 * Author:      Bill Clemence
 *
 * Modifications:
 *
 *
 *	wcc     16-Aug-1995	Initial release
 * 
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
#include "cp$inc:kernel_entry.h"
#include "cp$inc:platform_io.h"
#include "cp$src:pb_def.h"
#include "cp$src:ctype.h"

extern struct ZONE *conzone;

/*
 * An auxillary structure that hangs off the inode for the driver
 * to keep track of private buffers.
 */
struct FLASH {
    struct SEMAPHORE sync;		/* serialize access */
    struct pb *pb;
    int csr_base;			/* base address for the csr */
    char *fwname;			/* recommended firmware name */
    int fw_base;			/* base address for firmware */
    char *fwid;				/* firmware id name */
    int (*read)();			/* read routine */
    int (*write)();			/* write routine */
};

#include "cp$src:prototypes.h"

/*
 *  All definitions defined here
*/

/*                                                                          */
/* CMD nodespace register offsets:                                          */
/*                                                                          */
#define ADPRST	0x000
#define NODESTS	0x020
#define MREV	0x1ec
#define MUSR	0x1f0
#define MUCAR	0x1f8


/*
 * Adapter states
*/
#define ADPSTA_INPROCESS  0
#define ADPSTA_INIT       1
#define ADPSTA_INV_UCODE  2
#define ADPSTA_CPURAM_BAD 3
#define ADPSTA_BUFRAM_BAD 4
#define ADPSTA_CABLESWAP  5
#define ADPSTA_CI_PATHA   6
#define ADPSTA_CI_PATHB   7
#define ADPSTA_NODADR_MM  8
#define ADPSTA_CLUSIZ_MM  9

/*
 * Ucode Status bits
*/
#define MUSR_ENABLE     1
#define MUSR_CMPLT      2
#define MUSR_STS        0x10000

/*
 * reset mode types
*/
#define UPDATE_MODE       1
#define RESET_MODE        4
#define RESET_TEST_MODE  44


#define dprintf if (lfu_cipca_debug) printf

/*
 * i/o defintions
*/
#define X_READ(flp, a) (*flp->read)(flp->pb, flp->csr_base|a) 
#define X_WRITE(flp, a, d) (*flp->write)(flp->pb, flp->csr_base|a, d)

/*
*  All functions declared here
*/
#if !TURBO
extern unsigned long inmeml( struct pb *pb, unsigned __int64 offset );
extern void outmeml( struct pb *pb, unsigned __int64 offset, unsigned long data );
#endif

extern void null_procedure();

static long lfu_mode(struct FLASH *flp, long mode);

int lfu_cipca_init(void);

static int cipca_flash_inquire(char *fw, char *fname, struct FILE *fp);
static int cipca_flash_validate(int command, char *new_name, char *fw,
  char *firm_data, int firm_size, struct FILE *fp);
static int cipca_flash_write(struct FILE *fp, int size, int number, char *buf);
static int cipca_flash_open(struct FILE *fp, char *info, char *next,
  char *mode);
static int cipca_flash_close(struct FILE *fp);

int lfu_cipca_debug = 0;

/*
*  All variables declared here
*/
static struct DDB flash_ddb = {"", 
					/* how this wants to be called  */
  null_procedure, 			/* no read routine              */
  cipca_flash_write, 			/* write routine                */
  cipca_flash_open, 			/* open routine                 */
  cipca_flash_close,	 		/* close routine                */
  null_procedure, 			/* name expansion routine       */
  null_procedure, 			/* delete routine               */
  null_procedure, 			/* create routine               */
  null_procedure, 			/* setmode                      */
  cipca_flash_validate, 		/* validation routine           */
  cipca_flash_inquire, 			/* class specific use           */
  0, 					/* allows information           */
  0, 					/* must be stacked              */
  1, 					/* is a flash update driver     */
  0, 					/* is a block device            */
  0, 					/* not seekable                 */
  0, 					/* is an ethernet device        */
  0, 					/* is a filesystem driver       */
  };

static struct DDB fw_ddb = {"", 	/* how this wants to be called  */
  null_procedure, 			/* no read routine              */
  cipca_flash_write, 			/* write routine                */
  cipca_flash_open, 			/* open routine                 */
  cipca_flash_close, 			/* close routine                */
  null_procedure, 			/* name expansion routine       */
  null_procedure, 			/* delete routine               */
  null_procedure, 			/* create routine               */
  null_procedure, 			/* setmode                      */
  cipca_flash_validate, 		/* validation routine           */
  cipca_flash_inquire, 			/* class specific use           */
  0, 					/* allows information           */
  0, 					/* must be stacked              */
  0, 					/* is a flash update driver     */
  0, 					/* is a block device            */
  0, 					/* not seekable                 */
  0, 					/* is an ethernet device        */
  0, 					/* is a filesystem driver       */
  };
/*
 * CMD device struct array list
*/
static struct cmd_struct {
    char *fwid;
    int dev_code;
    char *fwname;
} cmd_list[] = {
{ "CIPCA", 0x06601095, "zzzzzzfw_cipca_fw" },
{0}
};


/*+
 * ============================================================================
 * = lfu_cipca_init - Initalize driver					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Initalizes the driver, by creating all the inodes needed for all cmd 
 * options in the system.
 *  
 * FORM OF CALL:
 *  
 *	lfu_cipca_init()
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
int lfu_cipca_init(void)
{
    struct INODE *ip;
    struct FLASH *flp;
    struct device *dev;

    int devcnt, i, j;
    struct device **devptr;
    char name[20];

    /* go throught all the cmd devices to setup. */
    for (j = 0; cmd_list[j].fwid; j++) {
    /* Find all the cmd devices available  and create an inode entry  for
     * each */
    devcnt = lfu_find_all_dev(cmd_list[j].dev_code, 0, 0, NULL);
    if (devcnt) {
	devptr = malloc(devcnt * sizeof(struct device *));
	lfu_find_all_dev(cmd_list[j].dev_code, 0, devcnt, devptr);
    }
    for (i = 0; i < devcnt; i++) {
	dev = devptr[i];
	sprintf(name, "%s%d_flash", dev->tbl->mnemonic, dev->unit);

	allocinode(name, 1, &ip);

	ip->dva = &flash_ddb;

	ip->attr = ATTR$M_READ;

	/* Permanently allocate an auxillary structure for the flash device */
	flp = ip->misc = dyn$_malloc(sizeof(struct FLASH),
	  DYN$K_FLOOD | DYN$K_SYNC | DYN$K_NOOWN);
	krn$_seminit(&flp->sync, 1, "flashsync");

	/* create a port block for this adapter. */
	flp->pb = dyn$_malloc(sizeof(struct pb), DYN$K_SYNC | DYN$K_NOOWN);
	flp->pb->hose = dev->hose;
	flp->pb->slot = dev->slot;
	flp->pb->bus = dev->bus;

	/* get the csr base address */
	flp->csr_base = (incfgl(flp->pb, 0x18) & 0xFFFFFFF0);
	flp->fwname = &cmd_list[j].fwname[9];
	flp->fwid = cmd_list[j].fwid;
	flp->read = inmeml;
	flp->write = outmeml;

	INODE_UNLOCK(ip);
    }					/* next device */

    if (devcnt) {
	free(devptr);
    } else {
	allocinode(cmd_list[j].fwname, 1, &ip);

	ip->dva = &fw_ddb;

	ip->attr = ATTR$M_READ;

	/* Permanently allocate an auxillary structure for the flash device */
	flp = ip->misc =
	  dyn$_malloc(sizeof(struct FLASH), DYN$K_SYNC | DYN$K_NOOWN);
	krn$_seminit(&flp->sync, 1, "flashsync");
	flp->fwid = cmd_list[j].fwid;

	INODE_UNLOCK(ip);
    }
    } /* next cmd_list entry.  */
    return (msg_success);
}

/*+
 * ============================================================================
 * = cipca_flash_inquire - Inquire about revision/firmware name 	      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Get the firmware and hardware rev of the module, also reports back the
 * recommended firmware revision.
 *  
 * FORM OF CALL:
 *  
 *	cipca_flash_inquire (char *fw, char *fname, struct FILE *fp)
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
static int cipca_flash_inquire(char *fw, char *fname, struct FILE *fp)
{
    unsigned int temp_data;
    struct FLASH *flp;

    flp = fp->ip->misc;

    if (lfu_mode(flp, RESET_MODE) == msg_success) {

	if ((X_READ(flp, NODESTS) & 0xFF) == ADPSTA_INIT) {

	    temp_data = X_READ(flp, MREV);

	    sprintf(fw, "%c%c%c%c", (temp_data & 0xFF), ((temp_data >> 8) &
	      0xFF), ((temp_data >> 16) & 0xFF), ((temp_data >> 24) & 0xFF));
	} else
	    sprintf(fw, "");

	/* recommended file name */
	sprintf(fname, flp->fwname);

	return (msg_success);
    }

    /* Couldn't read so return failure */
    return (msg_failure);

}

/*+
 * ============================================================================
 * = cipca_flash_open - open the flash rom                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Opens up the cmd device's flash rom for read or write operations. 
 *
 *				 NOTE 
 * 	The driver allows for only one reader/writer at a time.
 *  
 * FORM OF CALL:
 *  
 *	cipca_flash_open(struct FILE *fp, char *info, char *next, char *mode)
 *  
 * RETURNS:
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
static int cipca_flash_open(struct FILE *fp, char *info, char *next, char *mode)
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
 * = cipca_flash_close - close the flash rom                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Close the flash rom.
 *
 *  
 * FORM OF CALL:
 *  
 *	cipca_flash_close(struct FILE *fp)
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
static int cipca_flash_close(struct FILE *fp)
{

    struct FLASH *flp;

    flp = fp->ip->misc;

    /* We only allow one process to access the devices at a time. */
    krn$_post(&flp->sync);

    return msg_success;
}

/*+
 * ============================================================================
 * = cipca_flash_validate - validate or verify firmware                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Validate the first file and return success if good (can't read firmware rev)
 *                   or
 * Verify the two files. Can't really read the firmware either other than
 * report the firmware is corrupt in the cmd device.
 *  
 * FORM OF CALL:
 *  
 *	cipca_flash_validate (int command, char *new_name, char *fw,
 *			char *firm_data, int firm_size, struct FILE *fp)
 *  
 * RETURN CODES:
 *
 *      msg_success or msg_failure
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

static int cipca_flash_validate(int command, char *new_name, char *fw,
  char *firm_data, int firm_size, struct FILE *fp)
{
    struct FLASH *flp;
    int i, j;
    char imgfw_str[17];
    char fw_str[16];
    char dummy_str[32];
    int status;

    flp = fp->ip->misc;


    /* get the img fw rev and strip off all non-meaningfull caracters */
    strncpy(imgfw_str, firm_data + 24, 16);
    imgfw_str[16] = 0;
    for (i = j = 0; i < 9 && imgfw_str[i]; i++) {
	if (isprint(imgfw_str[i]) && !isspace(imgfw_str[i]))
	    imgfw_str[j++] = imgfw_str[i];
    }
    imgfw_str[j] = 0;

    if (command != 2) {

	/* do the same for the hardware fw rev */
	if (cipca_flash_inquire(fw_str, dummy_str, fp) == msg_success) {
	    for (i = j = 0; i < 9 && fw_str[i]; i++) {
		if (isprint(fw_str[i]) && !isspace(fw_str[i]))
		    fw_str[j++] = fw_str[i];
	    }
	    fw_str[j] = 0;
	} else
	    fw_str[0] = 0;
    }

    /* default the return to failure. This covers bad commands too */
    status = msg_failure;

    /* Since we cannot really compare the file lets verify it is good before we
     * either validate or verify it. */
    if (*(int *) firm_data == 0xfeedf00d && 
      strcmp_nocase((firm_data + 8), flp->fwid) == 0) {

	/* validate file. */
	if (command == 0 || command == 2) {

	    /* good file get rev and */
	    sprintf(fw, "%s", imgfw_str);
	    status = msg_success;

	    if (command == 0) {
		if (strcmp_nocase(fw_str, imgfw_str) == 1)
		    status = 2;		/* return 2; fimrware is older */
	    }
	}

	/* verify file. */
	if (command == 1) {

	    if (lfu_mode(flp, RESET_TEST_MODE) == msg_success) {
		if (strcmp_nocase(fw_str, imgfw_str) == 0)
		    status = msg_success;
	    }
	}

    }

    /* return status */
    return (status);
}

/*+
 * ============================================================================
 * = cipca_flash_write - write a block of data to the flash roms              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Write a block of data to the flash roms.
 *  
 * FORM OF CALL:
 *  
 *	cipca_flash_write( fp, int size, int number, buf)
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
 *      char *buf       - address of buffer to read data from
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static int cipca_flash_write(struct FILE *fp, int size, int number, char *buf)
{
    struct FLASH *flp;
    unsigned long temp_data;
    int i;
    int status;
    int bufadr;

    flp = fp->ip->misc;
    flp->fw_base = buf;

    /* Default status to faiure. */
    status = msg_failure;

    if (lfu_mode(flp, UPDATE_MODE) == msg_success) {


	/* Wait for the adapter to get into the proper enable state. */
	for (i = 0; i < 10; i++) {
	    krn$_sleep(1000);
	    if (X_READ(flp, MUSR) == MUSR_ENABLE)
		break;
	}

	if (X_READ(flp, MUSR) == MUSR_ENABLE) {

	    bufadr = dyn$_malloc(size*number, DYN$K_SYNC | DYN$K_ZONE, 0, 0, conzone);
	    memcpy(bufadr, buf, size*number);

	    /* write the module update command address register with the
	     * address of the firmware. Started the write operation. */

	    X_WRITE(flp, MUCAR, bufadr|io_get_window_base(flp->pb));

	    for (i = 0; i < 120; i++) {

		krn$_sleep(2000);		/* Check every 2 seconds. */

		if ((X_READ(flp, MUSR) & (MUSR_CMPLT | MUSR_ENABLE)) == (MUSR_CMPLT | MUSR_ENABLE))
		    break;
	    }

	    if (X_READ(flp, MUSR) == (MUSR_CMPLT | MUSR_ENABLE | MUSR_STS))
		status = msg_success;

	    free(bufadr);
	}
    } else
	dprintf("Failed to get to update mode \n");

    /* return the bytes written if we were successful */

    if (status == msg_success) {
	return (size * number);
    } else {
	dprintf("Failed to update, MUSR = %x\n", X_READ(flp, MUSR));
	return 0;
    }
}

/*+
 * ============================================================================
 * = lfu_mode - driver mode routine 				              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Performs a reset to initialize the adapter into accepting 
 *	maintenance commands.
 *
 * FORM OF CALL:
 *
 *	lfu_mode(mode);
 *
 * RETURN CODES:
 *
 *	msg_success or msg_failure.
 *
 * ARGUMENTS:
 *
 *      Mode  - a defined mode to put the cmd device into.
 *  			update     mode
 *			reset      mode
 *			reset test mode
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
-*/
static long lfu_mode(struct FLASH *flp, long mode)
{

    unsigned long temp;
    int i;

    /* Perform a HARD INIT to the controller. */
    X_WRITE(flp, ADPRST, 0);
    for (i = 0; i < 100; i++) {
	krn$_sleep(100);
	temp = X_READ(flp, NODESTS) & 0xFF;
	if (temp == ADPSTA_INIT || temp == ADPSTA_INV_UCODE)
	    break;

    }

    /* returned failed if we timed out. */
    if (i == 100)
	return (msg_failure);

    /* For reset test mode and the CRC is bad then return failure. */
    if (mode == RESET_TEST_MODE && temp == ADPSTA_INV_UCODE)
	return (msg_failure);

    /* If we are in the normal init state then we must go into ucode download
     * enable state. */
    if (mode == UPDATE_MODE && temp == ADPSTA_INIT) {
	krn$_sleep(1000);
	X_WRITE(flp, MUCAR, flp->fw_base|io_get_window_base(flp->pb));
    }

    return (msg_success);		/* default */
}
