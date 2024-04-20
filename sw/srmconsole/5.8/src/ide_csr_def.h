/*
 * Copyright (C) 1992 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:    IDE (Intelligent Drive Electronics) Definitions
 *
 * Author:      Judith Gold
 *
 * Modifications:
 *
 *      jeg     19-Nov-1992     Initial entry.
 */



/* IDE CSRs */

struct ide_regs {
        unsigned char sra;
        unsigned char srb;
        unsigned char dor;
        unsigned char tdr;
        unsigned char msr_dsr;
        unsigned char fifo;
        unsigned char unused;
        unsigned char dir_ccr;
 };



/* bootsector */

struct bootsector {
	unsigned char jump[3];		/* jump to boot code */
	unsigned char oem_name[8];	/* OEM name and DOS version */
	unsigned char bytes_per_sec[2]; /* bytes per sector */
	unsigned char bytes_per_clust[1];  /* bytes per cluster */
	unsigned char res_sectors[2];	/* # of reserved sectors */
	unsigned char FATs[1];		/* # of file-allocation tables */
	unsigned char root_dir_ents[2];	/* # of root-directory entries */
	unsigned char sectors[2];	/* total # of sectors; 0 = drive>32M */
	unsigned char media1[1];	/* media descriptor */
	unsigned char FAT_secs[2];	/* # of sectors per FAT */
	unsigned char sec_per_track[2];	/* sectors per track */
	unsigned char heads[2];		/* # of heads */
	unsigned char hidden_secs[4];	/* # of hidden sectors */
	unsigned char huge_sectors[4];	/* # of sectors if drive > 32M */
	unsigned char drive_number[1];	
	unsigned char reserved1[1];
	unsigned char boot_signature[1];/* extended boot signature */
	unsigned char volume_ID[4];	
	unsigned char volume_label[11];	
	unsigned char file_sys_type[8];	
 };


struct deviceparams {
	unsigned char spec_func;
	unsigned char dev_type;
	unsigned char dev_attr[2];
	unsigned char cylinders[2];
	unsigned char media_type;
	unsigned char bytes_per_sec[2];	/* bytes per sector */
	unsigned char sec_per_clust;	/* sectors per cluster */
	unsigned char res_sectors[2];	/* # of reserved sectors */
	unsigned char FATs;		/* number of FATs */
	unsigned char root_dir_ents[2];	/* maximum # of entries in root dir */
	unsigned char media;		/* media descriptor value */
	unsigned char FAT_secs[2];	/* # sectors per FAT */
	unsigned char secs_per_track[2];/* # sectors per track */
	unsigned char heads[2];		/* # heads per drive */
	unsigned char hidden_secs[4];	/* # hidden sectors per drive */
	unsigned char huge_secs[4];	/* # huge sectors per drive */
 };


char device_sel[IDE_MAX_UNITS] = {0x1C,0x2D, 0x4E,0x8F};
