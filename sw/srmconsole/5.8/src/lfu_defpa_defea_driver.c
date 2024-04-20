/*
 * file:        lfu_defpa_defea_driver.c
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
 * Abstract:    LFU Driver for the DEFPA and DEFEA adapters.
 *
 *	This driver will update firmware on both the DEFPA (PCI to FDDI)
 *	and DEFEA (EISA to FDDI) adapters. 
 *	Note: Not all systems support DEFEA adapters.
 *
 * Author:      Bill Clemence
 *
 * Modifications:
 *
 *
 *	wcc     6-Jan-1995	Initial release
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
#include "cp$src:probe_io_def.h"
#include "cp$src:nport_q_def.h"
#include "cp$src:nport_def.h"

/*
 * An auxillary structure that hangs off the inode for the driver
 * to keep track of private buffers.
 */
struct FLASH {
    struct SEMAPHORE sync;		/* serialize access */
    struct pb *pb;
    int csr_base;			/* base address for the csr */
    char *fwname;			/* recommended firmware name */
    int (*read)();			/* read routine */
    int (*write)();			/* write routine */
    int eisa;				/* eisa type device flag */
};

#include "cp$inc:prototypes.h"

/*
 *  All definitions defined here
*/

#if REGATTA || TURBO || RAWHIDE || APC_PLATFORM || WILDFIRE || MONET
#define LFU_EISA_FDDI 0
#else
#define LFU_EISA_FDDI 1
#endif

#define BSWAP(x) ((((x)&0xff)<<24)|(((x)&0xff00)<<8)|\
		  (((x)&0xff000000)>>24)|(((x)&0xff0000)>>8))

/*                                                                          */
/* defpa/defea nodespace register offsets:                                  */
/*                                                                          */
#define PORT_RESET     0x0
#define HOST_DATA      0x4
#define PORT_CONTROL   0x8
#define PORT_DATA_A    0xC
#define PORT_DATA_B    0x10
#define PORT_STATUS    0x14
#define PORT_HOST_0    0x18
#define PORT_INTR_ENA  0x1C

/*
 * Adapter states
*/
#define PS_UPDATE         0x100
#define PS_DMA_UNAVAIL    0x200
#define PS_HALTED         0x600
#define PS_MASK	          0x700
#define PS_HALT_ID_MASK   0x0FF
#define PS_HALT_CRC_ERR   0x007

/*
 * Port control commands
*/
#define READ_FIRM            0x8004
#define COPY_DATA            0x9000
#define FLASH_BLAST          0xC000

/*
 * defxa_mode types
*/
#define UPDATE_MODE       1
#define RESET_MODE        4
#define RESET_TEST_MODE  44

/*
 * defxa from info
*/
#define DEFXA_READ(flp, a) (*flp->read)(flp->pb, flp->csr_base|a) 
#define DEFXA_WRITE(flp, a, d) (*flp->write)(flp->pb, flp->csr_base|a, d)

/* Define the defpa fbus values if they haven't been yet... */
#ifndef PCI$K_DTYPE_DEC_PFI
#define PCI$K_DTYPE_DEC_PFI	0x000F1011
#define PCI$K_DEXT_DEFPA        0xDEF11011
#endif
#ifndef EISA$K_DTYPE_FR
#define EISA$K_DTYPE_FR		0x0030a310
#endif

/*
*  All functions declared here
*/
#if !(TURBO || LX164 || SX164)
extern unsigned long inportl( struct pb *pb, unsigned __int64 offset );
extern void outportl( struct pb *pb, unsigned __int64 offset, unsigned long data );
extern unsigned long inmeml( struct pb *pb, unsigned __int64 offset );
extern void outmeml( struct pb *pb, unsigned __int64 offset, unsigned long data );
#endif

extern void null_procedure();

static long lfu_defxa_mode(struct FLASH *flp, long mode);
static long lfu_defxa_wait_for_done(struct FLASH *flp, int seconds,
  int command);

int lfu_defxa_init(void);
static int defxaflash_inquire(char *fw, char *fname, struct FILE *fp);
static int defxaflash_validate(int command, char *new_name, char *fw,
  char *firm_data, int firm_size, struct FILE *fp);
static int defxaflash_write(struct FILE *fp, int size, int number, char *buf);
static int defxaflash_open(struct FILE *fp, char *info, char *next,
  char *mode);
static int defxaflash_close(struct FILE *fp);

/*
*  All variables declared here
*/
static struct DDB defxaflash_ddb = {"", /* how this wants to be called  */
  null_procedure, 			/* no read routine              */
  defxaflash_write, 			/* write routine                */
  defxaflash_open, 			/* open routine                 */
  defxaflash_close, 			/* close routine                */
  null_procedure, 			/* name expansion routine       */
  null_procedure, 			/* delete routine               */
  null_procedure, 			/* create routine               */
  null_procedure, 			/* setmode                      */
  defxaflash_validate, 			/* validation routine           */
  defxaflash_inquire, 			/* class specific use           */
  0, 					/* allows information           */
  0, 					/* must be stacked              */
  1, 					/* is a flash update driver     */
  0, 					/* is a block device            */
  0, 					/* not seekable                 */
  0, 					/* is an ethernet device        */
  0, 					/* is a filesystem driver       */
  };

static struct DDB defxafw_ddb = {"",	/* how this wants to be called  */
  null_procedure, 			/* no read routine              */
  defxaflash_write, 			/* write routine                */
  defxaflash_open, 			/* open routine                 */
  defxaflash_close, 			/* close routine                */
  null_procedure, 			/* name expansion routine       */
  null_procedure, 			/* delete routine               */
  null_procedure, 			/* create routine               */
  null_procedure, 			/* setmode                      */
  defxaflash_validate, 			/* validation routine           */
  defxaflash_inquire, 			/* class specific use           */
  0, 					/* allows information           */
  0, 					/* must be stacked              */
  0, 					/* is a flash update driver     */
  0, 					/* is a block device            */
  0, 					/* not seekable                 */
  0, 					/* is an ethernet device        */
  0, 					/* is a filesystem driver       */
  };


/* crc failure flag used only during check firmware (_validate, verify) */
static int lfu_defxa_crc_failure;
static char *master_fwname[] = {"zzzzzzfw_dfxaa_fw","zzzzzzfw_dfeaa_fw",
  "zzzzzzfw_dfeab_fw"};

/*+
 * ============================================================================
 * = lfu_defpa_defea_init - Initalize driver					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Initalizes the driver, by creating all the inodes needed for all defpas 
 * and defeas in the system.
 *  
 * FORM OF CALL:
 *  
 *	lfu_defpa_defea_init()
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
int lfu_defpa_defea_init(void) 
{
    struct INODE *ip;
    struct FLASH *flp;
    struct device *dev;
    int devcnt, devcnt1, devcnt2, i, eisa_rev;
    struct device **devptr, **devptr2;
    struct FILE *fp;
    char name[20];

#if RAWHIDE
    /* For Rawhide, to avoid some some questions about what file to
     * use, dfxaa_fw or dfpaa_fw; we will look for a dfxaa_fw file 
     * if it is there leave it alone. If we don't it and find dfpaa_fw
     * change to look for dfpaa_fw. If we don't find either one then 
     * default to the new one(dfxaa_fw). */
    if (fp = fopen("dfxaa_fw", "rs")) {
	fclose(fp);
    } else { 
	if (fp = fopen("dfpaa_fw", "rs")) {
	     fclose(fp);
	     master_fwname[0][11] = 'p';
	}
    }
#endif

/* Find all the defpa devices available  and create an inode entry for each */

    devcnt1 = lfu_find_all_dev(PCI$K_DTYPE_DEC_PFI, 0, 0, NULL);
    devcnt2 = lfu_find_all_dev(PCI$K_DTYPE_DEC_PFI, PCI$K_DEXT_DEFPA, 0, NULL);
    devcnt = devcnt1 + devcnt2;

    if (devcnt) {
	devptr = malloc(devcnt * sizeof(struct device *));
        lfu_find_all_dev(PCI$K_DTYPE_DEC_PFI, 0, devcnt1, (int *)devptr);
        devptr2 = devptr + devcnt1;
        lfu_find_all_dev(PCI$K_DTYPE_DEC_PFI, PCI$K_DEXT_DEFPA, devcnt2, (int *)devptr2);
    }
    for (i = 0; i < devcnt; i++) {
	dev = devptr[i];
	sprintf(name, "%s%d_flash", dev->tbl->mnemonic, dev->unit);

	allocinode(name, 1, &ip);

	ip->dva = &defxaflash_ddb;

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
	flp->pb->type = TYPE_PCI;

	/* get the csr base address */
	flp->csr_base = (incfgl(flp->pb, 0x10) & 0xFFFFFFF0);
	flp->read = inmeml;
	flp->write = outmeml;

	flp->fwname = &master_fwname[0][9];
	master_fwname[0][0] = 0;

	INODE_UNLOCK(ip);
    }					/* next device */

    if (devcnt)
	free(devptr);


#if !LFU_EISA_FDDI 
    master_fwname[1][0] = 0;
    master_fwname[2][0] = 0;
#endif
#if LFU_EISA_FDDI 

/* Find all the defea devices available  and create an inode entry for each */
    devcnt = lfu_find_all_dev(EISA$K_DTYPE_FR, 0, 0, NULL);
    if (devcnt) {
	devptr = malloc(devcnt * sizeof(struct device *));
	lfu_find_all_dev(EISA$K_DTYPE_FR, 0, devcnt, devptr);
    }
    for (i = 0; i < devcnt; i++) {
	dev = devptr[i];

	/* get the eisa rev to identify the proper rev to update.. */
	eisa_rev = (read_eisa_id(dev->hose, dev->slot) & 0xF000000) >> 24;

	/* current valid eisa revs are 1-3 */
	if (eisa_rev > 0 && eisa_rev < 4) { 

	    sprintf(name, "%s%d_flash", dev->tbl->mnemonic, dev->unit);

	    allocinode(name, 1, &ip);

	    ip->dva = &defxaflash_ddb;

	    ip->attr = ATTR$M_READ;

	    /* Allocate an auxillary structure for the flash device */
	    flp = ip->misc = dyn$_malloc(sizeof(struct FLASH),
	      DYN$K_FLOOD | DYN$K_SYNC | DYN$K_NOOWN);
	    krn$_seminit(&flp->sync, 1, "flashsync");

	    /* create a port block for this adapter. */
	    flp->pb = dyn$_malloc(sizeof(struct pb), DYN$K_SYNC | DYN$K_NOOWN);
	    flp->pb->hose = dev->hose;
	    flp->pb->slot = dev->slot;
	    flp->pb->bus = dev->bus;
	    flp->pb->type = TYPE_EISA;

	    /* get the csr base address */
	    flp->csr_base = (dev->slot << 12);
	    flp->read = inportl;
	    flp->write = outportl;

	    /* flag eisa type */
	    flp->eisa = 1;

	    /* identify the proper name based on eisa rev. */
	    if (eisa_rev == 1) {
		flp->fwname = &master_fwname[1][9];
		master_fwname[1][0] = 0;
	    } else {
		if (eisa_rev == 2) {
		    flp->fwname = &master_fwname[2][9];
		    master_fwname[2][0] = 0;
		} else {
		    if (eisa_rev == 3) {	/* rev 3 is like defpa */
			flp->fwname = &master_fwname[0][9];
			master_fwname[0][0] = 0;
		    }
		}
	    }
	}
	INODE_UNLOCK(ip);
    }					/* next device */

    if (devcnt)
	free(devptr);

#endif

    /* for all the files I missed. */
    for (i = 0; i < 3; i++) {
	if (master_fwname[i][0] != 0) {
	    allocinode(master_fwname[i], 1, &ip);

	    ip->dva = &defxafw_ddb;

	    ip->attr = ATTR$M_READ;

	    /* Permanently allocate an auxillary structure for the flash 
	     * device */
	    flp = ip->misc =
	      dyn$_malloc(sizeof(struct FLASH), DYN$K_SYNC | DYN$K_NOOWN);
	    krn$_seminit(&flp->sync, 1, "flashsync");

	    /* create a port block for this adapter.  We only need the pb
	     * type so that dfpaa_fw in disquise of dfxaa_fw will be see as
	     * valid. */
	    flp->pb = dyn$_malloc(sizeof(struct pb), DYN$K_SYNC | DYN$K_NOOWN);
	    flp->pb->type = TYPE_PCI;

	    flp->fwname = &master_fwname[i][9];

	    INODE_UNLOCK(ip);
	}
    }

    return (msg_success);
}

/*+
 * ============================================================================
 * = defxaflash_inquire - Inquire about revision/firmware name                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Get the firmware and hardware rev of the module, also reports back the
 * recommended firmware revision.
 *  
 * FORM OF CALL:
 *  
 *	defxaflash_inquire (char *fw, char *fname, struct FILE *fp)
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
static int defxaflash_inquire(char *fw, char *fname, struct FILE *fp)
{
    unsigned int temp_data;
    struct FLASH *flp;

    flp = fp->ip->misc;

    if (lfu_defxa_mode(flp, RESET_MODE) == msg_success) {

	/* perform the read firm command */
	DEFXA_WRITE(flp, PORT_CONTROL, READ_FIRM);

	/* if we completed then read the firmware rev */
	if (lfu_defxa_wait_for_done(flp, 2, READ_FIRM) == msg_success) {
	    temp_data = DEFXA_READ(flp, HOST_DATA);

	    sprintf(fw, "%c%c%c%c", ((temp_data >> 24) & 0xFF), ((temp_data >>
	      16) & 0xFF), ((temp_data >> 8) & 0xFF), (temp_data & 0xFF));
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
 * = defxaflash_open - open the flash rom                                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Opens up the defxa's (python) flash rom for read or write operations. 
 *
 *				 NOTE 
 * 	The driver allows for only one reader/writer at a time.
 *  
 * FORM OF CALL:
 *  
 *	defxaflash_open(struct FILE *fp, char *info, char *next, char *mode)
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
static int defxaflash_open(struct FILE *fp, char *info, char *next, char *mode)
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
 * = defxaflash_close - close the flash rom                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Close the flash rom.
 *
 *  
 * FORM OF CALL:
 *  
 *	defxaflash_close(struct FILE *fp)
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
static int defxaflash_close(struct FILE *fp)
{

    struct FLASH *flp;

    flp = fp->ip->misc;

    /* We only allow one process to access the devices at a time. */
    krn$_post(&flp->sync);

    return msg_success;
}

/*+
 * ============================================================================
 * = defxaflash_validate - validate or verify firmware                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Validate the first file and return success if good (can't read firmware rev)
 *                   or
 * Verify the two files. Can't really read the firmware either other than
 * report the firmware is corrupt in the python. This may not work either.
 * Initial developemnt on corrupted firmware module idicated module will hang
 * so it cannot be talked to. 
 *  
 * FORM OF CALL:
 *  
 *	defxaflash_validate (int command, char *new_name, char *fw,
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

static int defxaflash_validate(int command, char *new_name, char *fw,
  char *firm_data, int firm_size, struct FILE *fp)
{
    struct FLASH *flp;
    int checksum, i, j;
    char imgfw_str[9];
    char fw_str[9];
    char dummy_str[32];
    int status;

    flp = fp->ip->misc;

    /* get the img fw rev and strip off all non-meaningfull caracters */
    strncpy(imgfw_str, firm_data + 32, 8);
    imgfw_str[8] = 0;
    for (i = j = 0; i < 9 && imgfw_str[i]; i++) {
	if (isprint(imgfw_str[i]) && !isspace(imgfw_str[i]))
	    imgfw_str[j++] = imgfw_str[i];
    }
    imgfw_str[j] = 0;

    if (command != 2) {
	/* do the same for the hardware fw rev */
	if (defxaflash_inquire(fw_str, dummy_str, fp) == msg_success) {
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

    /* We are going from a defined name dfpaa to a dfxaa for the newer 
     * revisions of firmware. To allow for downreving the defpa adapter,
     * we will adjust the filedata header of the dfpaa to dfxaa. We do
     * this by getting the 11 byte from master_fwname 0; this way if we are
     * a RAWHIDE and we adjusted our name back to dfpaa_fw we will coreect 
     * the name to the correct master name. */
     if (flp->pb->type == TYPE_PCI && firm_data[2] == 'p')
	firm_data[2] = master_fwname[0][11];

    /* Since we cannot really compare the file lets verify it is good before we
     * either validate or verify it. */
    checksum = compute_checksum(firm_data + 512, (firm_size - 512) / 4, 0);
    if ((substr_nocase(firm_data, flp->fwname) != 0) &&
      (checksum == *(int *) (firm_data + 44))) {


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

	    lfu_defxa_crc_failure = 0;
	    if (lfu_defxa_mode(flp, RESET_TEST_MODE) == msg_success) {
		if (lfu_defxa_crc_failure == 0)
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
 * = defxaflash_write - write a block of data to the flash roms               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Write a block of data to the flash roms.
 *  
 * FORM OF CALL:
 *  
 *	defxaflash_write( fp, int size, int number, buf)
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
static int defxaflash_write(struct FILE *fp, int size, int number, char *buf)
{
    struct FLASH *flp;
    unsigned long temp_data;
    int i;
    unsigned int *fptr;
    unsigned int total_size;
    int status;

    total_size = size * number;

    /* we loose the first block of header info */
    fptr = buf + 0x200;
    total_size -= 0x200;

    flp = fp->ip->misc;

    if (status = lfu_defxa_mode(flp, UPDATE_MODE) == msg_success) {
	for (i = 0; i < (total_size / 4); i += 2) {


	    /* copy the data */
	    DEFXA_WRITE(flp, PORT_DATA_A, BSWAP(*(fptr + i)));
	    DEFXA_WRITE(flp, PORT_DATA_B, BSWAP(*(fptr + i + 1)));

	    /* perform the copy data command */
	    DEFXA_WRITE(flp, PORT_CONTROL, COPY_DATA);

	    /* If we didn't complete the copy data command then */
	    /* set the return to failure and stop writing bits. */
	    status = lfu_defxa_wait_for_done(flp, 2, COPY_DATA);

	    if (status == msg_failure)
		break;
	}

	if (status == msg_success) {

	    /* If we copied the data ok then attempt the write. */
	    DEFXA_WRITE(flp, PORT_DATA_A, total_size);
	    DEFXA_WRITE(flp, PORT_DATA_B, 0);

	    /* perform the firmware blast command */
	    DEFXA_WRITE(flp, PORT_CONTROL, FLASH_BLAST);


	    /* wait a little before checking */
	    krn$_sleep(5000);

	    /* If we time out or the write fails return 0 */
	    status = lfu_defxa_wait_for_done(flp, (10 * 60), FLASH_BLAST);
	}
    }


    /* return the bytes written if we were successful */
    if (status == msg_success) {
	return (size * number);
    } else
	return 0;

}

/*+
 * ============================================================================
 * = lfu_defxa_mode - defxa  driver mode routine		              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Performs a reset to initialize the defxa into accepting 
 *	maintenance commands.
 *
 * FORM OF CALL:
 *
 *	lfu_defxa_mode(mode);
 *
 * RETURN CODES:
 *
 *	msg_success or msg_failure.
 *
 * ARGUMENTS:
 *
 *      Mode  - a defined mode to put the defxa into.
 *  			update     mode
 *			reset      mode
 *			reset test mode
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
-*/
static long lfu_defxa_mode(struct FLASH *flp, long mode)
{

    unsigned long temp;
    int i;

    /* if eisa type then enable it. */
    if (flp->eisa) {
	outportb(flp->pb, flp->csr_base | 0x40, 0);
					/* burst holdoff */
	outportb(flp->pb, flp->csr_base | 0xc92, (flp->pb->slot * 0x10));
					/* io_cmp_0_1 */
	outportb(flp->pb, flp->csr_base | 0xc94, (flp->pb->slot * 0x10));
					/* io_cmp_1_1 */
	outportb(flp->pb, flp->csr_base | 0xcae, 3);
					/* function cntrl */
	outportb(flp->pb, flp->csr_base | 0xc84, 1);
					/* enable the slot */
    }


    /* Perform a HARD INIT to the controller. */
    DEFXA_WRITE(flp, PORT_DATA_A, (mode & 0x7));
    DEFXA_WRITE(flp, PORT_DATA_B, 0);
    DEFXA_WRITE(flp, PORT_RESET, 1);
    krn$_sleep(500);			/* sleep 1/2 second (> 1 micro) */
    DEFXA_WRITE(flp, PORT_RESET, 0);
    for (i = 0; i < 100; i++) {
	krn$_sleep(100);
	temp = DEFXA_READ(flp, PORT_STATUS);
	if (mode == RESET_MODE && temp == PS_DMA_UNAVAIL)
	    break;
	if (mode == UPDATE_MODE && temp == PS_UPDATE)
	    break;
	if (mode == RESET_TEST_MODE && temp == PS_DMA_UNAVAIL)
	    break;
    }

    /* returned failed if we timed out unless we's in reset test mode. */
    if (i == 100 && mode != RESET_TEST_MODE)
	return (msg_failure);

    /* for reset test mode set the crc_failed flag if we got  a crc error.  If 
     * we didn't then this module is badddddddd...... */
    if (mode == RESET_TEST_MODE && i == 100) {
	temp = DEFXA_READ(flp, PORT_STATUS);
	if ((temp & PS_MASK) == PS_HALTED &&
	  (temp & PS_HALT_ID_MASK) == PS_HALT_CRC_ERR)
	    lfu_defxa_crc_failure = 1;
	else
	    return (msg_failure);
    }

    return (msg_success);		/* default */
}

/*+
 * ======================================================================
 * = lfu_defxa_wait_for_done - waits for the command to complete        =
 * ======================================================================
 *
 * OVERVIEW:
 *
 * 	Wait for the command sent is done command clear.
 *
 * FORM OF CALL:
 *
 *	lfu_defxa_wait_for_done(flp, seconds, command);
 *
 * RETURN CODES:
 *
 *	msg_success or msg_failure.
 *
 * ARGUMENTS:
 *
 *      Seconds - the maximum amount of time to wait for done bit.
 *      command - The command sent to check for clear.
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
-*/
static long lfu_defxa_wait_for_done(struct FLASH *flp, int seconds,
  int command)
{
    int i;

    /* wait around for the command to clear */
    for (i = 0; i < (200 * seconds); i++) {
	krn$_sleep(5);
	if (!((DEFXA_READ(flp, PORT_CONTROL)) & command))
	    break;
    }

    /* If we completed before we timed out then */
    if (i < 100 * seconds)
	return (msg_success);
    else {
	return (msg_failure);
    }
}

