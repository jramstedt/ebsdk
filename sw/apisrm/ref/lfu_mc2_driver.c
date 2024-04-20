/*
* file:        lfu_mc2_driver.c
*
* Copyright (C) 1996 by
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
* Abstract:    LFU Driver Code.  This driver provides the ability to program
*              the onboard Am29F040 Flash Memory Chip located on the PCI MC2 
*              module.  
*
*
* Author:      Domenic LaCava
*
* Modifications:
*
*       djl	21-OCT-1997	First release.
*
*/

/*
* All include files here
*/
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:mem_def.h"
#include "cp$src:common.h"
#include "cp$src:msg_def.h"
#include "cp$src:ctype.h"
#include "cp$inc:platform_io.h"	
#include "cp$inc:kernel_entry.h"
#include "cp$src:pb_def.h"

/*
 * An auxillary structure that hangs off the inode for the driver
 * to keep track of private buffers.
 */
struct FLASH {
    struct SEMAPHORE sync;		/* serialize access */
    struct pb *pb;
    int RomBaseAddr;			/* flash base address of mc2 */
};

/*
*  All definitions defined here
*/

/* define the pci mc2 device type if it hasn't been already. */
#ifndef PCI$K_DTYPE_DEC_MC2
#define PCI$K_DTYPE_DEC_MC2	0x00181011
#endif

#define Cfg08h$M_RevID          0x000000FF

#define debug_print 0
#if debug_print
#define dprintf printf
#else
#define dprintf
#endif

/* Flash Size */
#define FLASH_IMAGE_SIZE  524288 

/* VPD defines*/
#define VPD_SECTOR_SIZE   65536 /* size of sector containing VPD and 
 				 * associated structures */
#define VPD_SECTOR_END    65535 /* last offset in the sector */
#define VPD_SIZE          86    /* # of bytes of actual VPD data */

/* VPD offsets from start of sector */
#define VPD_PCI_TAG       0x080
#define VPD_START         0x100
#define VPD_REV		  0x149 
#define VPD_CHKSUM	  0x155 


/* Return values for Data Polling algorithm */
#define msg_write_success 1
#define msg_write_failure 0

#define SIZE$K_64K 65536

#define MC2_READ_ROM(flp, a) inmemb( flp->pb, (flp->RomBaseAddr | ((~(a)) & 0x7fffc)) | ((a) & 0x3))
#define MC2_WRITE_ROM(flp, a, d) outmeml(flp->pb, (flp->RomBaseAddr | ((a)<<2)), d & 0xFF)

/* AMD Chip Data Bits */
#define DATA$V_Sector_Erase_Timer 0x08	/* bit 3 */
#define DATA$V_Exceeded_Timing	  0x20	/* bit 5 */
#define DATA$V_Toggle_Bit	  0x40	/* bit 6 */
#define DATA$V_Data_Polling	  0x80	/* bit 7 */

/*
*  All routines declared here
*/
extern void null_procedure();

int lfu_mc2_init(void);
static int mc2_inquire(char *fw, char *fname, struct FILE *fp);
static int mc2_validate(int command, char *new_name, char *fw, char *firm_data,
  int firm_size, struct FILE *fp);
static int mc2_read(struct FILE *fp, int size, int number, char *buf);
static int mc2_write(struct FILE *fp, int size, int number, char *buf);
static int mc2_open(struct FILE *fp, char *info, char *next, char *mode);
static int mc2_close(struct FILE *fp);

static int erase_flash_sector(struct FLASH *flp, int sector);
static int prog_byte(struct FLASH *flp, int addr, unsigned char *data);
static int data_polling(struct FLASH *flp, int rom_address, int bit7);
static int comp_rom_to_file(struct FLASH *flp, int sector_SA, int file_SA);

static int vpd_check(struct FLASH *flp); 
static int vpd_compute_checksum(struct FLASH *flp, unsigned char *VPD_buf);

/* FW Rev, as extracted from header.  Global value. */
unsigned char FW_Rev_from_header[4];

/* ddb data base.... */
static struct DDB mc2_ddb = {"", 	/* how this wants to be called  */
  mc2_read, 				/* read routine                 */
  mc2_write, 				/* write routine                */
  mc2_open, 				/* open routine                 */
  mc2_close, 				/* close routine                */
  null_procedure, 			/* name expansion routine       */
  null_procedure, 			/* delete routine               */
  null_procedure, 			/* create routine               */
  null_procedure, 			/* setmode                      */
  mc2_validate, 			/* validation routine           */
  mc2_inquire, 				/* class specific use           */
  0, 					/* allows information           */
  0, 					/* must be stacked              */
  1, 					/* is a flash update driver     */
  0, 					/* is a block device            */
  0, 					/* not seekable                 */
  0, 					/* is an ethernet device        */
  0, 					/* is a filesystem driver       */
  };

static struct DDB mc2fw_ddb = {"", 	/* how this wants to be called  */
  mc2_read, 				/* read routine                 */
  mc2_write, 				/* write routine                */
  mc2_open, 				/* open routine                 */
  mc2_close, 				/* close routine                */
  null_procedure, 			/* name expansion routine       */
  null_procedure, 			/* delete routine               */
  null_procedure, 			/* create routine               */
  null_procedure, 			/* setmode                      */
  mc2_validate, 			/* validation routine           */
  mc2_inquire, 				/* class specific use           */
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
 * = lfu_mc2_init - Initalize driver					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Initalizes the driver, by creating all the inodes needed for all mc2
 * adapters in the system.
 *  
 * FORM OF CALL:
 *  
 *	lfu_mc2_init()
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
int lfu_mc2_init(void) 
{
    struct INODE *ip;
    struct FLASH *flp;
    struct pb pb;
    struct device *dev;
    int devcnt, i, k;
    struct device **devptr;
    char name[20];

    /* Find all the mc2 devices available */
    devcnt = lfu_find_all_dev(PCI$K_DTYPE_DEC_MC2, 0, 0, NULL);
    if (devcnt) {
	devptr = malloc(devcnt * sizeof(struct device *));
	lfu_find_all_dev(PCI$K_DTYPE_DEC_MC2, 0, devcnt, devptr);
    }
    for (i = 0; i < devcnt; i++) {

        /* fill in local port block to keep track of device infomration */
        dev = devptr[i];
        pb.hose = dev->hose;
        pb.slot = dev->slot;
	pb.bus  = dev->bus;
 	pb.function = dev->function;

        /* if mc module is MC2 and higher (MC1 and MC1.5 not updatable) */

	if ((incfgl(&pb, 0x08) & (Cfg08h$M_RevID & 0x0000007F)) >= 0x20) {
	    sprintf(name, "%s%d_flash", dev->tbl->mnemonic, dev->unit);

	    allocinode(name, 1, &ip); /*provides name on inode list */

	    ip->dva = &mc2_ddb; /* dva = device address for ddb */

	    /* define the maximimum image size and amount permently
	     * allocated... */
	    ip->len[0] = FLASH_IMAGE_SIZE;

	    ip->attr = ATTR$M_READ;

	    /* Permanently allocate an auxillary structure for the flash device
	     */
	    flp = ip->misc =
	      dyn$_malloc(sizeof(struct FLASH), DYN$K_SYNC | DYN$K_NOOWN);
	    krn$_seminit(&flp->sync, 1, "mc2sync");
	    INODE_UNLOCK(ip);

	    /* create a port block for this adapter. */
	    flp->pb = dyn$_malloc(sizeof(struct pb), DYN$K_SYNC | DYN$K_NOOWN);
	    flp->pb->hose = dev->hose;
	    flp->pb->slot = dev->slot;
	    flp->pb->bus = dev->bus;
	    flp->pb->function = dev->function;

	    /* expansion rom base addr of mc2, offset 0x30 in pci cnfg space */
	    flp->RomBaseAddr = (incfgl(flp->pb, 0x30) & 0xFFFFFFF0);

            /* Set the Expansion ROM Base Address Enable bit in order to
             * allow mc2 device to respond to expansion ROM accesses     */
	    outcfgl(flp->pb, 0x30, flp->RomBaseAddr | 1);

	    ip->attr = ATTR$M_READ;

	    ip->misc = flp;  

	    INODE_UNLOCK(ip);
	}

    }					/* next device */


    /* NOTE: need more conditionals other than devcnt for this piece of code*/
    if (devcnt) { /* if we found an MC module...we're done */
	free(devptr);
    } else { /* found none */
	allocinode("zzzzzzfw_ccmab_fw", 1, &ip); /* creates filename for the LFU
		to be able to show when "list" command is issued only if MC
                adapter is not present */

	ip->dva = &mc2fw_ddb;

	ip->attr = ATTR$M_READ;

	/* Permanently allocate an auxillary structure for the flash device */
	flp = ip->misc =
	  dyn$_malloc(sizeof(struct FLASH), DYN$K_SYNC | DYN$K_NOOWN);
	krn$_seminit(&flp->sync, 1, "mc2sync");

	INODE_UNLOCK(ip);
    }

    return (msg_success);
}

/*+
 * ============================================================================
 * = mc2_open - open the flash rom					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Opens up the mc2 flash rom for read or write operations. 
 *
 *				 NOTE 
 * 	The driver allows for only one reader/writer at a time.
 *  
 * FORM OF CALL:
 *  
 *	mc2_open(struct FILE *fp, char *info, char *next, char *mode)
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
static int mc2_open(struct FILE *fp, char *info, char *next, char *mode)
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
 * = mc2_close - close the flash rom					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Close the flash rom.
 *
 *  
 * FORM OF CALL:
 *  
 *	mc2_close(struct FILE *fp)
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
static int mc2_close(struct FILE *fp)
{

    struct FLASH *flp;

    flp = fp->ip->misc;

    /* We only allow one process to access the devices at a time. */
    krn$_post(&flp->sync);

    return msg_success;
}

/*+
 * ============================================================================
 * = mc2_inquire - Inquire about firmware revision/name		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Get the firmware rev of the module, also reports back the
 * recommended firmware filename.
 *  
 * FORM OF CALL:
 *  
 *	mc2_inquire (char *fw, char *fname, struct FILE *fp)
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
static int mc2_inquire(char *fw, char *fname, struct FILE *fp)
{
    struct FLASH *flp;
    char str[5];
    int i, j;

    flp = fp->ip->misc;

    /* just read the rev. */
    if(vpd_check(flp) == msg_success) {
       for (i = 0; i < 4; i++)
          str[i] = (char) inmemb( flp->pb, (flp->RomBaseAddr | (VPD_REV + i)) );
       str[4] = 0;
    } else {
        sprintf(str, "TBD");
      }

    sprintf(fw, "%s", str);

    /* recommended firmware file, passed to LFU */
    strcpy(fname, "ccmab_fw"); 

    return (msg_success);

}

/*+
 * ============================================================================
 * = mc2_validate - validate or verify fimrware			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Validate the file and return the firmware rev if good or
 * verify the file.
 *  
 * FORM OF CALL:
 *  
 *	mc2_validate(int command, char *new_name, char *fw,
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
static int mc2_validate(int command, char *new_name, char *fw, char *firm_data,
  int firm_size, struct FILE *fp)
{
    struct FLASH *flp;
    int checksum;
    char imgfw_str[9];
    char fw_str[9];
    char dummy_str[32];
    int status = msg_failure;
    int i, j, k, l;
    int len, size;
    unsigned char *data;
    unsigned char *fdata;

    flp = fp->ip->misc;

    /* get the img fw rev and strip off all non-meaningfull characters */
    strncpy(imgfw_str, firm_data + 32, 8);
    imgfw_str[8] = 0;

    /* save FW rev for global use */
    for (i = 0; i < 4; i++)
        FW_Rev_from_header[i] = imgfw_str[i];

    for (i = j = 0; i < 9 && imgfw_str[i]; i++) {
	if (isprint(imgfw_str[i]) && !isspace(imgfw_str[i]))
	    imgfw_str[j++] = imgfw_str[i];
    }
    imgfw_str[j] = 0;

    /* validate file. */
    if (command == 0 || command == 2) {

	checksum = compute_checksum(firm_data + 512, (firm_size - 512) / 4, 0);

	if ((substr_nocase(firm_data, "ccmab_fw") != 0) &&
	  (checksum == *(int *) (firm_data + 44))) {

	    /* good file get rev and */
	    sprintf(fw, "%s", imgfw_str);
	    status = msg_success;


            if (command == 0) {

                /* do the same for the hardware fw rev */
                if (mc2_inquire(fw_str, dummy_str, fp) == msg_success) {
                    for (i = j = 0; i < 9 && fw_str[i]; i++) {
                        if (isprint(fw_str[i]) && !isspace(fw_str[i]))
                            fw_str[j++] = fw_str[i];
                    }
                    fw_str[j] = 0;
                } else
                    fw_str[0] = 0;

		if (strcmp_nocase(fw_str, imgfw_str) == 1)
		    status = 2;		/* return 2; fimrware is older */
	    }
	}
    }

    /* verify file */
    if (command == 1) {

	/* ensure pointer is unsigned */
	fdata = firm_data + 512;
	data = malloc(2048);
	k = 0;
	l = 0;

	/* Set size to the end of the read size or total requested 
           (takes smaller of these two as the size */
	size = min(FLASH_IMAGE_SIZE, (firm_size - 512));

        /* subtract off the size of the VPD area.  This is done because the VPD
           data will be different for each module and thus this section of data
	   will never match during the following compare */
        size -= VPD_SECTOR_SIZE;

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
 * = mc2_read - read data from the roms				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Reads from the roms.  This is normally only used by LFU via this driver's
 * validate routine to read the rom.
 *  
 * FORM OF CALL:
 *  
 *	mc2_read( fp, size, number, buf)
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
static int mc2_read(struct FILE *fp, int size, int number, char *buf)
{
    int i;
    unsigned char *data = buf;
    int len;
    struct FLASH *flp;

    flp = fp->ip->misc;

    /* adjust the read byte length to either: The end of the read size or total
     * requested */
    len = min(FLASH_IMAGE_SIZE - *fp->offset, size * number);

    /* read the flashrom */
    for (i = 0; i < len; i++) {
	*(data) = MC2_READ_ROM(flp, *(fp->offset) + i);
	data++;          
    }
    *fp->offset = *(fp->offset) + i;

    /* return the byte count of all the locations read. */
    return (i);
}

/*+
 * ============================================================================
 * = mc2_write - write a block of characters to the flash rom		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Write a block of characters to the flash rom.  This code actually 
 * performs the write to the rom. It requires the user to supply all of image
 * in one write.
 *  
 * FORM OF CALL:
 *  
 *	mc2_write( fp, size, number, buf)
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
static int mc2_write(struct FILE *fp, int size, int number, char *buf)
{
int sector, i, x, status, count, inc;
int sector_equals_file;
struct FLASH *flp;
unsigned char file_byte;
unsigned char *vpd_buf;
unsigned char *vpd_buf_modified;

flp = fp->ip->misc;

/* Default status to success...*/
status = msg_success;

buf = (int)(buf) + 512; /* point to start of firmware, after header */

    /* go through each sector of ROM, check, and re-program if necessary */ 
    for (sector = 0; sector < 8; sector++) {
 	/* compare data in entire ROM sector to corresponding sector of
  	 * data in the file */
	sector_equals_file = comp_rom_to_file(flp, (sector * 0x10000), buf);

        /* if sector data in ROM equals corresponding data in file, do not
         * erase and write to the ROM */
	if (sector_equals_file == msg_success)
	  continue;

        /* else erase the sector, for first 7 sectors or invalid vpd sector */
        if ( (sector == 7) && (vpd_check(flp) == msg_success) ) {

          /* save current contents of actual VPD sector of ROM */
          vpd_buf = malloc(VPD_SECTOR_SIZE);
          for (x = 0; x < VPD_SECTOR_SIZE; x++) {
              vpd_buf[x] = 
                 inmemb( flp->pb, (flp->RomBaseAddr | x) );
          }

          /* write in the rev of code as appears in FW file */ 
          for (x = 0; x < 4; x++)
               vpd_buf[VPD_REV + x] = FW_Rev_from_header[x];

	  /* re-calculate checksum of vpd section with new rev in place */
          vpd_buf[VPD_CHKSUM] =
              vpd_compute_checksum(flp, vpd_buf);

          /* erase the sector */
	  if (erase_flash_sector(flp, sector) == msg_success) {

	    /* Remember, we can't just stick in the vpd sector as is.
             * There is an addressing algorithm needed to program the ROM,
             * and therefore we have to mofify the sector data in order
             * to put the data in the expected form.  prog_byte() will
             * then program the FLASH in the correct fashion */  
            vpd_buf_modified = malloc(VPD_SECTOR_SIZE);

            for (count = x = 0, inc = 3; x < VPD_SECTOR_SIZE; x++) {
               vpd_buf_modified[VPD_SECTOR_END - (inc - x)] = vpd_buf[x]; 

               count++;

               if (count > 3) {
                  count = 0;
                  inc = inc + 8;
               }
            }
	    
            /* re-program entire VPD sector, a byte at a time */
	    for (i = sector * 0x10000; i < ((sector + 1) * 0x10000); i++) {
                file_byte = *(vpd_buf_modified + (i - (sector * 0x10000)));

                /* write byte from file to ROM for corresp. sector byte */
	        if (prog_byte(flp, i, file_byte) == msg_failure)
                  status = msg_failure;
                else
                  status = msg_success;

                if (status == msg_failure)
                  break;
	    }

          } else status = msg_failure; /*else erase failed */


        } else { /* else, non-VPD sector or invalid VPD sector */

	    if (erase_flash_sector(flp, sector) == msg_success) {

              /* re-program entire sector, a byte at a time */
	      for (i = sector * 0x10000; i < ((sector + 1) * 0x10000); i++) {
                  file_byte = *(buf + i);

                  /* write byte from file to ROM for corresp. sector byte */
	          if (prog_byte(flp, i, file_byte) == msg_failure)
                    status = msg_failure;
                  else
                    status = msg_success;

                  if (status == msg_failure)
                    break;
	      }

            } else status = msg_failure; /*else erase failed */
          }

        if (status == msg_failure)
          break;  
    }

    /* return the bytes written if we were successful...*/
    if (status == msg_success) {
       return (size * number);
    } else {
        dprintf("\nFAILed to update\n");
        return 0;
    }
}

/*+
 * ============================================================================
 * = erase_flash_sector - Erase flash sector routine			      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This function will erase the flash rom on MC2.  The erase is done on
 *	a per-sector basis, thus this function erases only the sector whose 
 *	address is passed into this function
 *
 * FORM OF CALL:
 *
 *       int erase_flash_sector(flp, sector)
 *
 * RETURNS:
 *
 *      status 
 *
 * ARGUMENTS:
 *
 *	struct FLASH *flp	- pointer to local inode info.
 *	int sector		- sector to erase.
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
-*/
static int erase_flash_sector(struct FLASH *flp, int sector)
{

    int data_polling_result = 0;

    MC2_WRITE_ROM(flp, 0x5555, 0xAA);
    MC2_WRITE_ROM(flp, 0x2AAA, 0x55);

    MC2_WRITE_ROM(flp, 0x5555, 0x80);	/* erase command */

    MC2_WRITE_ROM(flp, 0x5555, 0xAA);
    MC2_WRITE_ROM(flp, 0x2AAA, 0x55);

    MC2_WRITE_ROM(flp, (sector * 0x10000), 0x30);

    /* run data through Data Polling Algorithm to conirm operation success */
    data_polling_result = 
       data_polling(flp, (sector * 0x10000), DATA$V_Data_Polling);

    /* return to read mode */
    MC2_WRITE_ROM(flp, 0x5555, 0xAA);
    MC2_WRITE_ROM(flp, 0x2AAA, 0x55);

    MC2_WRITE_ROM(flp, 0x5555, 0xF0);

    if (data_polling_result)
	return msg_success;
    else
	return msg_failure;
}

/*+
 * ============================================================================
 * = prog_byte() - write flash, byte at a time                               =
 * ============================================================================
 *
 * OVERVIEW:		programs the specified location to the specified value
 *			in the FROM.  Programming is one byte at a time for the 
 *			number of consecutive bytes indicated.
 *
 * FORM OF CALL:	
 *
 *	prog_byte (flp, addr, data)
 *
 * RETURN CODES:
 *
 *	msg_success
 *
 * ARGUMENTS:
 *
 *  struct FLASH *flp	- pointer to local inode info.
 *  	
 *  int addr		- location in pcia flash to write to.
 *
 *  unsigned char data	- the data to write. Passed by value.
 *
 * SIDE EFFECTS:
 *
 *
-*/
static int prog_byte(struct FLASH *flp, int addr, unsigned char data)
{
    int actual_bit7, data_polling_result;

    /* write the two "unlock" cycles */
    MC2_WRITE_ROM(flp, 0x5555, 0xAA);
    MC2_WRITE_ROM(flp, 0x2AAA, 0x55);

    /* write the program setup command */
    MC2_WRITE_ROM(flp, 0x5555, 0xA0);

    /* program the flash with a write */
    MC2_WRITE_ROM(flp, addr, data);

    /* compute data bit 7 value for data polling comparison needs */
    actual_bit7 = data & DATA$V_Data_Polling;

    /* run data through Data Polling Alg. to conirm operation success*/
    data_polling_result = data_polling(flp, addr, actual_bit7);

    if (data_polling_result)
	return msg_success;
    else
	return msg_failure;
}

/*+
 * ============================================================================
 * = data_polling - Data Polling algorithm - confirm success of erase or write=
 * ============================================================================
 *
 * OVERVIEW:
 *
 * This function is performs the algorithm supplied by the Am29F040 device in 
 * order to indicate that the embedded erase or write algorithms are in
 * progress or completed.  During the embedded program algorithm, an attempt
 * to read the device will produce the compliment of the data last writtten to
 * DQ7.  Upon completion, an attempt to read the device will produce the true 
 * data last written to DQ7. During the embedded erase algorithm, an attempt 
 * to read the device will produce a "0" at the DQ7 output and upon completion
 * an attempt to read the device will produce a "1" at the DQ7 output.  
 *
 * FORM OF CALL:
 *
 *     int data_polling(struct FLASH *flp, int rom_addr, int DQ7_success_value)
 *
 * RETURNS:
 *
 *      status 
 *         - msg_write_success or msg_write_failure
 *
 * ARGUMENTS:
 * 	flp 	   	  - pointer to local inode info
 *	rom_addr	  - address being checked for proper erase or program
 *      DQ7_success_value - expected value of DQ7 to be tested against
 *
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
-*/
static int data_polling(struct FLASH *flp, int rom_addr, int DQ7_success_value)
{
    int data_check;
    int timer = 60000;			/* initialize clock to 1 minute */

    do {
        krn$_sleep(1);
	timer--;

	/* read data from ROM address */
	data_check = MC2_READ_ROM(flp, rom_addr);

	if ((data_check & DATA$V_Data_Polling) == DQ7_success_value)
					/* if DQ7(erase)=1, DQ7(prog)=DQ7 */
	    break;			/* exit test because erase/prog is
					 * successful */

	if (~(data_check & DATA$V_Exceeded_Timing))
					/* else if DQ5 != 1 */
	    continue;			/* skip remaining checks, return to
					 * start of loop */

	/* read data from ROM address */
	data_check = MC2_READ_ROM(flp, rom_addr);

	if ((data_check & DATA$V_Data_Polling) == DQ7_success_value)
					/* if DQ7(erase)=1, DQ7(prog)=DQ7 */
	    break;			/* exit test because erase/prog is
					 *   succesful*/

    }					/* check until DQ5 = 1 or one minute
					 * timer has expired */
      while ((~(data_check & DATA$V_Exceeded_Timing)) && (timer != 0));

    /* Algorithm is done.  Now return status */
    if ((data_check & DATA$V_Data_Polling) == DQ7_success_value)
	return msg_write_success;
    else
	return msg_write_failure;
}

/*+
 * ============================================================================
 * = comp_rom_to_file - Compare rom data to file data in sector          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * This function compares the data already present in the flash ROM
 * to the data which is in the file that will program the flash.  If these are
 * equal, then there is no need to erase and write the same data that is 
 * already there.  This is done for an entire sector per function call, one 
 * byte at a time.
 *
 * FORM OF CALL:
 *
 *       int comp_rom_to_file(struct FLASH *flp, sector_addr, file_StartAddr)
 *
 * RETURNS:
 *
 *      status 
 *         - msg_success or msg_failure
 *
 * ARGUMENTS:
 *
 * 	flp 	       - pointer to local inode info
 * 	file_StartAddr - file starting address which is to be compared
 *	sector_addr    - ROM sector address which is to be compared
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
-*/
static int comp_rom_to_file(struct FLASH *flp, int sector_addr, int file_StartAddr)
{
    int i;
    int file_data;
    int rom_data;

    for (i = 0; i < SIZE$K_64K; i++) {
	file_data = *(unsigned char *) (file_StartAddr + sector_addr + i);
	rom_data = MC2_READ_ROM(flp, (sector_addr + i));
	if (file_data != rom_data) {
	    return msg_failure;		/* Inequality found */
        }
    }

    return msg_success;			/* ROM and file equal for entire 64k
					 * addresses*/
}

/*+
 * ============================================================================
 * = vpd_check - Check to see if VPD area contains valid VPD data             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * This function checks the 64k sector containing the VPD data.  First, the
 * checksum of the area is calculated.  Next, the checksum which should already
 * be reported within the VPD area is compared to the calculated value.  If
 * are equal, it is assumed that there is valid VPD data here.  If they are
 * equal, it is assumed that there is no VPD data in the region.
 *
 * FORM OF CALL:
 *
 *       int vpd_check(struct FLASH *flp)
 *
 * RETURNS:
 *
 *      status 
 *         - msg_success or msg_failure
 *
 * ARGUMENTS:
 *
 * 	flp 	       - pointer to local inode info
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
-*/
static int vpd_check(struct FLASH *flp)
{
   /* compare the VPD's self-contained checksum to a newly computed checksum 
    * then check the beginning of the PCI Data Structure for the "PCIR" tag */
   if ( (vpd_compute_checksum(flp, 0) == 
     inmemb(flp->pb, (flp->RomBaseAddr | VPD_CHKSUM))) &&
     (inmemb(flp->pb, (flp->RomBaseAddr | (VPD_PCI_TAG)))     == 0x50) && /*P*/
     (inmemb(flp->pb, (flp->RomBaseAddr | (VPD_PCI_TAG + 1))) == 0x43) && /*C*/
     (inmemb(flp->pb, (flp->RomBaseAddr | (VPD_PCI_TAG + 2))) == 0x49) && /*I*/
     (inmemb(flp->pb, (flp->RomBaseAddr | (VPD_PCI_TAG + 3))) == 0x52) )  /*R*/

      return msg_success;

   else return msg_failure;
}

/*+
 * ============================================================================
 * = vpd_compute_checksum - Compute the Checksum of VPD Data                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 *
-*/
static vpd_compute_checksum (struct FLASH *flp, unsigned char *vpd_buf)

{
    int i, j, checksum;

        for (j = i = 0; i < (VPD_SIZE - 1); i++) {
           if (vpd_buf == 0) /* if checksumming actual ROM */
              j += inmemb( flp->pb, (flp->RomBaseAddr | (VPD_START + i)) );
           else /* else we are checksumming new vpd data in a temp. buffer */
              j += vpd_buf[VPD_START + i];
           j = j & 0xFF;
        }

        checksum = (0x100 - j) & 0xFF;
        return (checksum);
}
