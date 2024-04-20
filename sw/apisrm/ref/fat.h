#ifndef _fat
#define _fat 1

/*
 * msdos common header file
 */

#define SECSIZE		512		/* MSDOS sector size in bytes */
#define MAX_PATH	128		/* largest MSDOS path length */


struct direntry {                       /* byte positons in decimal */
  char  filename[8];            	/*  0-7 */
  char  extention[3];                   /*  8-10 */
  unsigned char attribute;           /* 11	*/
  unsigned char pad[10];                /* 12-21 */
  unsigned char update_time[2];         /* 22-23 */
  unsigned char update_date[2];         /* 24-25 */
  unsigned char start_cluster[2];	/* 26-27 */
  unsigned char file_size[4];           /* 28-31 */
};

/*
 * File attribute bytes
 */
#define	FATTR_READ	0x01   		/* bit 0 */
#define	FATTR_HIDDEN	0x02            /* bit 1 */
#define	FATTR_SYSTEM	0x04            /* bit 2 */
#define	FATTR_VOLUME	0x08            /* bit 3 */
#define	FATTR_DIRECTORY	0x10            /* bit 4 */
#define	FATTR_ARCHIVE	0x20            /* bit 5 */
#define FATTR_RES6	0x40            /* bit 6 */
#define FATTR_RES7	0x80            /* bit 7 */

#define FAT_DIRENT_NEVER_USED            0x00
#define FAT_DIRENT_REALLY_0E5            0x05
#define FAT_DIRENT_DIRECTORY_ALIAS       0x2e
#define FAT_DIRENT_DELETED               0xe5

/* 
 * Cluster/Index routines
 */

/* The following constants the are the valid Fat index values. */

#define FAT_CLUSTER_AVAILABLE            0x0000
#define FAT_CLUSTER_RESERVED             0xfff0
#define FAT_CLUSTER_BAD                  0xfff7
#define FAT_CLUSTER_LAST                 0xffff

typedef enum _CLUSTER_TYPE {
    CLUSTER_AVAILABLE,
    CLUSTER_RESERVED,
    CLUSTER_BAD,
    CLUSTER_LAST,
    CLUSTER_NEXT
} CLUSTER_TYPE;

struct msdos_partition {
 	unsigned char	status;		/* partition status	*/
	unsigned char	start_head;	/* starting head 	*/
	unsigned char	start_sector;	/* starting sector	*/
	unsigned char	start_cylinder;	/* starting cylinder	*/
	unsigned char	type;		/* partition type	*/
	unsigned char	end_head;	/* ending head		*/
	unsigned char	end_sector;
	unsigned char	end_cylinder;
	unsigned char	abs [4];	/* starting absolute sector	*/
	unsigned char	numsec [4];	/* number of sectors	*/
};
#define PART_UNKNOWN	0
#define	PART_FAT12	1
#define PART_FAT16_S	4	/* small 16 bit fat, <32M	*/
#define	PART_XDOS	5	/* extended DOS partition */
#define	PART_FAT16_L	6	/* large 16 bit fat, >=32M	*/


struct msdos_bootblock {                      	/* bytes position in decimal */
	unsigned char jump [3];            	/* 0-2 */
	unsigned char oem_name [8];             /* 3-10, DOS name */
	unsigned char bytes_per_sector [2];     /* 11-12 */
	unsigned char sectors_per_clust [1]; /* size of a cluster, 13 */
	unsigned char reserved_sectors [2];  	/* 14-15 */
	unsigned char num_FATs [1];             /* 16 */
	unsigned char num_root_dir_entries [2]; /* 17-18 */
	unsigned char total_sectors [2];        /* 19-20 */
	unsigned char media_descriptor [1];     /* 21 */
	unsigned char sectors_per_FAT [2];      /* 22-23 */
	unsigned char nsect [2];		/* Sectors/track, 24-25 */
	unsigned char nheads [2];		/* Heads 26-27 */

	/* DOS 5.0 specific */
	unsigned char hidden [4];               /* 38-31 */
	unsigned char sec32 [4];                /* 32-35 */
	unsigned char dnum [1];                 /* 36 */
	unsigned char reserved [1];             /* 37 */
	unsigned char signature [1];            /* 38 */
	unsigned char vol_id [4];               /* 39-42 */
	unsigned char vol_label [11];           /* 43-53 */
	unsigned char fs_type [8];              /* 54-61 */

	unsigned char junk [384];	/* who cares? */
	struct msdos_partition  ptab [4];
	unsigned char valid_boot [2];
};       

#endif
