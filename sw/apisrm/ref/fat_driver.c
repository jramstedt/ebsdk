/* file:	fat_driver.c
 *
 * Copyright (C) 1992, 1993 by
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
 *      Alpha Common Console
 *
 *  ABSTRACT:	Driver for File Allocation Table (FAT)
 *
 *      The File Allocation Table provides a map to the storage locations of
 *	files on a disk by indicating which clusters are allocated to each 
 *	file and what order.  Each fat entry points to the next fat entry of 
 *	the cluster   
 *
 *  OVERVIEW:
 *
 *	This driver must be stacked on top of some device (such as a
 *	floppy or hard disk).
 *
 * 	For example, cat fat:foo.txt/dva0
 *
 *	The FAT driver will always read/write to the underlying driver
 *	dv_driver in multiples of SECSIZE (defined in fat.h) bytes.
 *
 *  AUTHORS:
 *
 *	AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *	01-Dec-1992
 *
 *  Restrictions:
 *
 *	1. writing file to root directory only
 *	2. FAT-32 hasn't been tested
 *	3. SECSIZE (bytes per sector) is restricted to 512 only
 *	4. file name can't be "\"
 *	5. can't handle file name having extensive characters 
 *	6. no conversion is done to the file name starting with E5
 *	7. can't delete (rm) files from FAT formatted disk
 *
 *  MODIFICATION HISTORY:
 *
 *      17-Dec-1996	C. Lau	fix writing problem, eco 1112.0
 *				
 *--
 */


/* INCLUDE FILES */

#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:common.h"
#include	"cp$src:alpha_arc.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:fat.h"
#include	"cp$src:ctype.h"
#include	"cp$src:limits.h"
#include	"cp$src:regexp_def.h"
#include	"cp$src:time.h"

/*+
 * ============================================================================
 * = struct FAT_MCB - Map of Contiguous Blocks.	     			      =
 * ============================================================================
 * 
 * STUCTURE OVERVIEW:
 *
 * The following structure is used by the driver to map a file into sections
 * (or runs) of contiguous blocks. A file stream is accessed one run at a time
 * as indicated in the MCB.  'inuse' denotes #th of contiguous block. If the    
 * file is more segmented, 'inuse' is greater.  But it never exceeds the
 * limit FAT_MAXIMUM_MCB which is up to the user to define.  
 *
 * We chose 250 as a guess that one could have made 500 files on a floppy and
 * could have deleted half of them.
 *
-*/
#define FAT_MAXIMUM_MCB                  250

struct FAT_MCB {
    int inuse;        				/* # of runs/sections */
    unsigned int vbo[ FAT_MAXIMUM_MCB ];        
    unsigned int lbo[ FAT_MAXIMUM_MCB ];         
} ;


/*+
 * ============================================================================
 * = struct FAT_STREAM - Context need to read or write a fat file. 	      =
 * ============================================================================
 * 
 * STUCTURE OVERVIEW:
 *
 * The context is general enough to handle root directories (they don't show 
 * up in FAT, and are contiguous), regular files, and subdirectories 
 * (subdirectories have 0 length and we use the terminating cluster number to 
 * catch the end of the subdirectory).
 *
-*/
struct FAT_STREAM {
	unsigned char	*buf;		/* file read/write buff		     */
	unsigned int	filelen;	/* length of file associated w/ stream*/
	unsigned int	status;		/* return status code		     */
	unsigned int	offset;		/* byte offset within the file	     */
	unsigned int	isroot;		/* stream is a root directory	     */
        unsigned int    direntlbo;	/* LBO for this directory 	     */
        unsigned int    prev_entry;	/* last fat entry allocated to write */
        unsigned int    clusts_exist;   /* # of clusters already allocated   */ 
        struct FAT_MCB  mcb;		/* MCB for this file                 */
	struct direntry de;		/* directory entry for the stream    */
};


/*+
 * ============================================================================
 * = struct SCAN - Context for each directory level.		              =
 * ============================================================================
 *
 * STRUCTURE OVERVIEW:
 *
 * To accomodate arbitrarily deep subdirectories, we create a context for each 
 * level.  By allocating at run time we thus avoid recursive calls (on 
 * processes with small stacks).
 *
-*/
struct SCAN {
    struct SCAN *flink;         /* pointer to next scan context		     */
    struct SCAN *blink;         /* pointer to previous scan context	     */
    struct FAT_STREAM sp;	/* stream for this level	  	     */
    struct direntry de;		/* current directory entry being processed   */
				/* (points into the stream's buffer)	     */
};


/*+
 * ============================================================================
 * = struct FATS - File Allocation Table information.	      	  	      =
 * ============================================================================
 *
 * STRUCTURE OVERVIEW:
 *
 * Stores FAT descriptions which are obtained from boot sector and 
 * also creates some important links to FILE, FAT_STREAM and search QUEUE. 
 *
-*/
struct FATS {
	struct msdos_bootblock bootblock;	/* the entire bootblock	     */	
	struct FILE	*fp;			/* underlying driver         */
	int	clustsize;			/* in bytes		     */
        int	total_clusters;			/* total clusters for data   */
	int	fatsize;			/* in bytes 		     */
	unsigned char *fat;			/* clone of fat table 	     */
	int	fatlen;				/* # entries in fat table    */
	int	fatbits;			/* 12, 16 or 32		     */
	int	lastindex;		/* 0xff0 0r 0xfff0 for 12,16 bit fat */
	int	last_cluster;		/* entry used to end cluster chain   */
	int	sfa;			/* start of file area (sectors)      */
	int	(*print) ();		/* error print routine		     */

	struct FAT_STREAM rws;		/* read/write stream		     */

	/*
	 * Context used to maintain search paths.  We allow for arbitrarily
	 * long search paths and directory nesting. SCAN level 0 is for the
	 * root directory.
	 */
	struct  REGEXP *re;		/* regular expression for pathname   */
	char	curpath [MAX_PATH];	/* current resolution of above regexp*/
	struct  QUEUE scanh;	     /* scan chain for enumerating namespace */
};


#include "cp$inc:prototypes.h"

/* DEFINE FLAGS AND FUNCTIONS */

/* Not every platform may want the expansion capabilities. */
#define FAT_EXPAND 1

int fat_init ();
int fat_open (struct FILE *fp, char *info, char *next, char *mode);
int fat_close (struct FILE *fp);
int fat_read (struct FILE *fp, int size, int number, unsigned char *buf);
int fat_write (struct FILE *fp, int size, int number, unsigned char *buf);
int fat_expand (char *name, char *info, char *next, int longformat, 
		int (*callback)( ), int p0, int p1);
int fat_validate (char *next);
extern int null_procedure ();
extern int qprintf();
extern int cbip;        		/* CallBack In Progress */
struct FATS *read_bootblock (char *next);
struct FATS *fat_mount (char *next);
struct REGEXP *regexp_fat (char *regexp);
int fat_parsefname (char *fname, char *name, char *ext);

int getfat (struct FATS *fats, int entry);
void fat_dismount (struct FATS *fats);
void bytesfromnum (unsigned char *str, int val, int size);
void set_dir_ent (struct direntry *de);
void setfat ( struct FATS *fats, int entry, int next_entry);
void fat_stream_close (struct FAT_STREAM *sp);
void sprint_de (struct FATS *fats, char *cp, char *path, struct direntry *de);

/*
 * To isolate ourselves from little/big endian storage issues,
 * we access all numeric fields in the boot block via this routine,
 * which is from kernel_support.c 
 */
#define getnum(str) numfrombytes (str, sizeof (str))	

/* convert number to little endian form for writing back to disk */
#define setnum(str, num) bytesfromnum( str, num, sizeof(num) ) 


/*
 * ============================================================================
 * = struct DDB fat_ddb - FAT Driver DataBase. 			              =
 * ============================================================================
 *
 * STRUCTURE OVERVIEW:
 *
 * FAT Driver entry.  struct DDB is defined in kernel_def.h 
 *
-*/
struct DDB fat_ddb = {
	"fat",			/* how this routine wants to be called	*/
	fat_read,		/* read routine				*/
	fat_write,		/* write routine			*/
	fat_open,		/* open routine				*/
	fat_close,		/* close routine			*/
	fat_expand,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	fat_validate,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	1,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	1,			/* is a filesystem driver		*/
};


/* DEBUG FLAGS AND FUNCTIONS */
/*
 * There are a lot of codes in this driver used for debug (such as printing
 * out directories, boot blocks etc.) that take up a fair amount of room.
 * pprintf (not printf) should be used for output because printf will cause
 * exception when users redirect(>) a file to the disk.  The redirection and 
 * printf share a same output channel.  
 */
#define	FAT_DEBUG 0

/*
 * Runtime debug flags for printing.  If the sym driver is installed,
 *	
 *	"d -l sym:fat_print 1f"
 *
 * turns on all the flags.  FAT_DEBUG must also be set.
 */
#if FAT_DEBUG
int fat_print = 0;    	 /* controls printing bootblock. fat, de or mcb */
extern int pprintf();
#define dprintf pprintf
#else
#define dprintf 
#define print_bootblk
#define	print_fat
#define print_de
#define print_mcb
#endif

#define FAT_PRINT_BOOTBLOCK	1	/* print bootblock when read in	     */
#define FAT_PRINT_FAT		2	/* print fat when read in	     */
#define	FAT_PRINT_DE		4	/* print directory chain as visited  */
#define	FAT_PRINT_MCB		8	/* print MCB                         */
/*!!!!!!!!!!!!!!!!!!!!!!!!!! End Declaration !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */


/*+
 * ============================================================================
 * = fat_init - Initialize the FAT (File Allocation Table) driver.            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine initializes the FAT driver.  Since the firmware doesn't
 *	have a concept of mounting disks, and since the FAT driver must be
 *	stacked on a device such as dva0, no directory scans or reads are done 
 *	at init time.
 *
 * FORM OF CALL:
 *  
 *      fat_init ()
 *  
 * RETURNS:
 *
 *	msg_success 
 *       
 * SIDE EFFECTS:
 *
 *      Modifies the Driver DataBase and creates an INODE for the GPR device.
 *
-*/
int fat_init () 
{
	struct INODE 	*ip;

	/* create the FAT inode */
	allocinode (fat_ddb.name, 1, &ip);    /* in filesys.c */
	ip->dva = & fat_ddb;
	ip->attr = ATTR$M_READ | ATTR$M_WRITE;
	INODE_UNLOCK (ip);

	return msg_success;
}


/*+
 * ============================================================================
 * = fat_open - Open a file on a FAT device.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Open up a file on a FAT device.  The driver must be stacked upon a device
 * that looks like a FAT device.  Furthermore, we have to read the bootblock
 * of the device to see what kind of FAT system it is (fat 12, 16 or 32).
 *
 * The boot block and the first FAT table are cached for the duration of the
 * open/close.
 *
 * 'mode' passed to fat_open() is ignored.  Instead fp->mode is used. 
 * The relation between them is shown below:
 *~ 
 *    mode            fp->mode
 *     "r"         hex 1  = read(1)  
 *     "w"         hex 62 = write(1), rewrite(32), create(64)     
 *~
 *
 * FORM OF CALL:
 *  
 *	fat_open (fp, info, next, mode)
 *  
 * RETURNS:
 *
 *	msg_success or msg_failure
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *	char *info	- information that follows the colon.  
 *	char *next	- information that follows the slash.  This is the
 *			  remaining portion of the protocol tower.  The buffer
 *			  driver must always be stacked.
 *	char *mode	- ignored.
 *
-*/
int fat_open (struct FILE *fp, char *info, char *next, char *mode) 
{
	struct INODE *ip;
	struct FATS *fats;
	char	*fname;
	int	found;


	ip = fp->ip;

 	dprintf ("fat_open, info=%s, mode =%s\n", info, mode);

	/*
	 * Mount the device, establish connection with the underlying driver 
	 */
	fats = fp->misc = fat_mount (next);
	if (fats == 0) {
	    return msg_failure;
	}
	    
        /* 
         * Check if this is the root directory in which case we just open 
         * a stream 
         */
        if ( (info[0] == '\\') && (strlen(info) == 1) ) {

	    /* set up a stream  */
	    fat_stream_open (fats, &fats->rws, 0);

        } else {

	    /* Look up the filename on the volume */

	    if (fat_scan_open (fats, info) == msg_failure) {
		fat_dismount (fats);
	        return msg_failure;
	    }
	    found = fat_scan_next (fats, &fats->rws.de);
	    fat_scan_close (fats);

	    dprintf("fp mode = %x \n", fp->mode);
	    if ( !found ) {
                if ( fp->mode & MODE_M_CREATE ) {

                    /* Not found and user wants to create */

                    memset( &fats->rws.de, 0, sizeof(struct direntry) );

		    /* separate file name and extension */
    		    if (fat_parsefname( info, fats->rws.de.filename, 
				fats->rws.de.extention ) == msg_failure){
			fat_dismount (fats);
			return msg_failure;
		    }	
		
                    set_dir_ent(&fats->rws.de);
                    if (create_dir_ent( fats ) == msg_failure){
                        fat_dismount (fats);
			return msg_failure;
		    }	

                } else {
                   
                    /* Not found and user did not want to create */

	            (*fats->print) ("FAT: can't find %s\n", info);
		    fat_dismount (fats);
	            return msg_failure;
                }
	    }
            		/* found */
            else {
                /* If opened for rewrite (supersede), discard old contents */
            	if ( fp->mode & MODE_M_REWRITE ) {
		    dprintf("rewrite an existing file\n");
   	 	    dealloc_clusters(fats, &fats->rws.de);		
    		    set_dir_ent(&fats->rws.de); 
    		    fat_disk_write( fats, fats->rws.direntlbo, 
				sizeof(struct direntry), &fats->rws.de);
                }

    	        else {
		    /* set up a stream for the file */
	    	    fat_stream_open (fats, &fats->rws, &fats->rws.de);
                }
	    }	
        }

 	return msg_success;
}


/*+
 * ============================================================================
 * = fat_close -  Close a FAT device.  			 	              = 
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Close FAT stream and dismount a FAT device.
 *
 * FORM OF CALL:
 *  
 *      fat_close (fp)
 *  
 * RETURNS:
 *
 *      msg_success
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp	 - Pointer to FILE structure
 *
-*/
int fat_close (struct FILE *fp) 
{
	struct FATS *fats;

	fats = fp->misc;
	fat_stream_close (&fats->rws);
	fat_dismount (fats);

	return msg_success;
}


/*+
 * ============================================================================
 * = fat_read - Read bytes from a FAT file.				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Read bytes from a FAT file.  Since FAT handles bytes in units of
 *      clusters, and read requests don't necessarily come in those sizes, we 
 *      have to do some deblocking.  For example, the command cat can allow the
 *	user to specify the size and number which will pass to fat_read. 
 *
 * FORM OF CALL:
 *  
 *      fat_read (fp, size, number, buf)
 *  
 * RETURNS:
 *
 *	Number of bytes read, or 0 on error.
 *       
 * ARGUMENTS:
 *
 *      struct FILE *fp	      -	Pointer to FILE structure.
 *	int size	      -	size of item in bytes
 *	int number 	      -	number of items to read
 *	unsigned char *buf    -	Address to write the data to.
 *
-*/
int fat_read (struct FILE *fp, int size, int number, unsigned char *buf) 
{
	struct FATS *fats;
	int	bytes_read;

        dprintf("read number= %d and size=%x hex \n", number, size);
	fats = fp->misc;      
	fats->rws.offset = *fp->offset;

	bytes_read = fat_stream_read (fats, &fats->rws, size * number, buf);
	*fp->offset += bytes_read;

	if (bytes_read == 0) {
	    fp->status = fats->rws.status;
	} else {
	    fp->status = msg_success;
	}

	return bytes_read;
}


/*+
 * ============================================================================
 * = fat_write - Write bytes to a FAT file.				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Write bytes to a FAT file.  Since FAT handles bytes in units of
 *      clusters, and write requests could be in any size, some deblocking
 *	is required.  
 *
 * FORM OF CALL:
 *  
 *      fat_write (fp, size, number, buf)
 *  
 * RETURNS:
 *
 *	Number of bytes written, or 0 on error.
 *       
 * ARGUMENTS:
 *
 *      struct FILE *fp	      -	Pointer to FILE structure.
 *	int size	      -	size of item in bytes
 *	int number 	      -	number of items to read
 *	unsigned char *buf    -	Address to write the data to.
 *
-*/
int fat_write (struct FILE *fp, int size, int number, unsigned char *buf) 
{
    struct FATS *fats;
    int	bytes_written;

    dprintf("write number = %d, size = %x hex \n", number, size);	
    fats = fp->misc;
    fats->rws.offset = *fp->offset;

    bytes_written = fat_stream_write (fats, &fats->rws, size * number, buf);
   
    *fp->offset += bytes_written;

    if (bytes_written == 0) {
        fp->status = fats->rws.status;
    } else {
        fp->status = msg_success;
    }

    return bytes_written;
}


/*+
 * ============================================================================
 * = fat_expand - Expand a regular expression over the FAT name space.        =
 * ============================================================================
 *
 * OVERVIEW:
 *	Take a regular expression and apply it to all the names in the FAT file
 *	system.  Return all names that match via a callback.
 *
 *	Assume that the downstream driver also has a regular expresssion, and
 *	expands those names.  For each downstream expansion, apply the
 *	regular expression to that FAT file system.
 *
 * FORM OF CALL:
 *  
 *      fat_expand ()
 *  
 * RETURNS:
 *
 *	msg_success or msg_failure	
 *       
 * ARGUMENTS:
 *
 *	char *name            - the name of?
 *	char *info	      -	the regular expression to be expanded
 *	char *next	      -	the driver on which this is stacked
 *	int   longformat      -	print full directory information or just name
 *	int *(callback) ()    -	routine to call on every match
 *	int	p0	      -	callback parameter 0
 *	int	p1	      -	callback parameter 1
 *
-*/
int fat_expand (char *name, char *info, char *next, int longformat, 
		int (*callback)( ), int p0, int p1) 
{
#if FAT_EXPAND
	struct FATS *fats;
	struct direntry de;
	char	*line;

	dprintf("fat_expand starts \n");
	/*
	 * Mount a fat file system on the fully qualified
	 * downstream driver.
	 */
	if ((fats = fat_mount (next)) == 0) {
	   return msg_failure;
	}

        /* If the command is ls fat:\\, then it's the root directory */
        if ( (strlen (info) == 1) && (info[0] == '\\') )
            info[0] = '*';

	/*
	 * Set up the scan context
	 */
	if (fat_scan_open (fats, info) == msg_failure) {
	    return msg_failure;
	}
	line = malloc (256);

	/*
	 * Generate all the names that the expression matches
	 */
	while (fat_scan_next (fats, &de)) {
	    if (longformat) {
		sprint_de (fats, line, fats->curpath, &de);        	
	    } else {
		sprintf (line, "'fat:%s/%s'", fats->curpath, next);
	    }
	    (*callback) (line, p0, p1);
	}

	/* clean up */
	free (line);
	fat_scan_close (fats);
     	fat_dismount (fats);

	return msg_success;
#endif
}


/*+
 * ============================================================================
 * = fat_validate - Check if a valid FAT system.                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Validate a FAT system by reading bootblock.
 * 
 * FORM OF CALL:
 *  
 *      fat_validate (next)
 *  
 * RETURNS:
 *
 *	msg_success      
 *       
 * ARGUMENTS:
 *
 *      char *next 	- underneath driver (information after the slash) 
 *
-*/
int fat_validate (char *next) 
{
	struct FATS *fats;

	fats = read_bootblock(next);
	if (fats == 0)
	    return 0;

	fclose(fats->fp);
	free(fats);
	return 1;
}


/*+
 * ============================================================================
 * = read_bootblock - Extract bootblock information.    	              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This entails opening the underlying driver and reading in the
 * 	boot block which is the first sector in general, verifying the basic 
 *	structure.	
 * 
 * FORM OF CALL:
 *  
 *      read_bootblock (next)
 *  
 * RETURNS:
 *
 *      pointer to struct FATS on success, or 0 on failure.
 *       
 * ARGUMENTS:
 *
 *      char *next 	- underneath driver (information after the slash) 
 *
-*/
struct FATS *read_bootblock(char *next)
{
	struct FATS *fats;
	struct msdos_bootblock *bbp;

	dprintf("read_bootblock starts \n");

	fats = malloc (sizeof (struct FATS));
        if ( cbip )
            fats->print = null_procedure;
        else
	    fats->print = qprintf;

#if FAT_DEBUG
	fats->print = pprintf;
#endif

	/* 
	 * Open up the driver dv underneath us.  The FAT driver must always
	 * be stacked.
	 */
	fats->fp = fopen (next, "rs" );
	if (fats->fp == NULL) {
            (*fats->print)("Can't open device %s\n", next);
	    free (fats);
	    return 0;
	}

	/*
	 * Read in and cache the boot block, normally first sector, 512 bytes 
	 */
	bbp = & fats->bootblock;
	if (fread (bbp, 1, sizeof (struct msdos_bootblock), fats->fp) 
		!= sizeof (struct msdos_bootblock)) {
	    (*fats->print) ("FAT: can't read boot block on device %s\n", next);
	    goto return_bad;
	}

	/*
	 * Verify that the bootblock looks sane by checking ranges
	 */
	if (fat_validate_bootblock (fats) == 0) {
	    goto return_bad;
	}

	/*
	 * Determine the fat encoding (12 or 16 bit FAT)
	 */
	if ( get_fatbits(fats) == 0 ) {
	    goto return_bad;
        }

	print_bootblk (&fats->bootblock);   /* for debug */

	return fats;

return_bad:
	fclose (fats->fp);
	free (fats);
	return 0;
}


/*+
 * ============================================================================
 * = fat_mount - Mount a FAT device.                                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This calls read_bootblock() which opens the underlying driver and 
 *	reads in the MSDOS bootblock, root directory structure, fatbits.
 *	Then figures out additional file system info used by read and write 
 *	later on.
 *                
 * FORM OF CALL:
 *  
 *	fat_mount (next)
 *  
 * RETURNS:
 *
 *  	pointer to struct FATS on success, or 0 on failure.
 *       
 * ARGUMENTS:
 *
 *	char *next	- information that follows the slash.  This is the
 *			  remaining portion of the protocol tower.  The buffer
 *			  driver must always be stacked.
 *
-*/
struct FATS *fat_mount (char *next) 
{
	struct FATS *fats;
	struct msdos_bootblock *bbp;  		/* in fat.h */
	unsigned char *alternate_fat;
	int	i;

	dprintf("fat_mount starts \n");

        fats = read_bootblock(next);
        if (fats == 0)
	    return 0;
	
	bbp = & fats->bootblock;

	/*
	 * Compute offset to start of file area (in sectors).
	 * File area starts after reserved sectors, FAT tables,
	 * and root directory.  For 3-1/2 HD double sides, 
	 * reserved_sectors:1, num_FATs:2, sectors_per_FAT:9
	 * num_root_dir_entries:224, direntry size:32
	 */
	fats->sfa = getnum (bbp -> reserved_sectors) +
	      getnum (bbp -> num_FATs) * getnum (bbp -> sectors_per_FAT) +
	      getnum (bbp -> num_root_dir_entries) * sizeof (struct direntry) /
	      SECSIZE;


    	fats->clustsize = getnum(bbp -> sectors_per_clust) * SECSIZE;

        /* total clusters for data */
    	fats->total_clusters = ( getnum(bbp -> total_sectors) +
			   getnum(bbp -> sec32) - fats -> sfa ) /
                           getnum(bbp -> sectors_per_clust); 
                         
	/*
	 * Read in and cache the FAT (the FAT is immediately after the last
	 * reserved sector, most DOS disks have only one reserved sector, 
	 * which is bootstrap sector).
	 */
	fats->fatsize = getnum (bbp -> sectors_per_FAT) * SECSIZE;  
	fats->fat = malloc (fats->fatsize);
	fats->fatlen = (fats->fatsize * 8)/ fats->fatbits;

	fseek (fats->fp, getnum(bbp->reserved_sectors) * SECSIZE, SEEK_SET);  
	if (fread (fats->fat, 1, fats->fatsize, fats->fp) != fats->fatsize) {
	    (*fats->print) ("FAT: error reading FAT\n");
	    fat_dismount (fats);
	    return 0;
	}

	/*
	 * Read in and verify against alternate FATs if present
         * in general, there are 2 copies of FATs, one after another
	 */
	for (i=1; i<getnum(bbp->num_FATs); i++) {
	    alternate_fat = malloc (fats->fatsize);
	    if (fread (alternate_fat, 1, fats->fatsize, fats->fp) 
			!= fats->fatsize) {
		(*fats->print) ("FAT: error reading alternate FAT %d\n", i);
		free (alternate_fat);
		break;
	    } else {
		/* compare against the original */
		if (memcmp (fats->fat, alternate_fat, fats->fatsize)) {
		    (*fats->print) ("FAT: FAT tables aren't in sync\n"); 
		}
	    }
	    free (alternate_fat);
	}

	print_fat (fats);   /* debug */

	return fats;
}


/*+
 * ============================================================================
 * = fat_dismount -  Dismount a FAT device.  			              = 
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Close file and deallocate all the storage acquired by the mount.
 *
 * FORM OF CALL:
 *  
 *      fat_dismount (fats)
 *  
 * RETURNS:
 *
 *      NONE
 *
 * ARGUMENTS:
 *
 *	struct FATs *fat  - pointer to FAT strcuture
 *
-*/
void fat_dismount (struct FATS *fats) 
{
	fclose (fats->fp);
	free (fats->fat);
	free (fats);
}


/*+
 * ============================================================================
 * = fat_scan_open - Initialize a scan data structure.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Set up a double link list scan structure so that we may enumerate the 
 *	FAT namespace depth first.  Also check if the input name(info) has a
 *	valid expression.
 * 
 * FORM OF CALL:
 *  
 *      fat_scan_open (fats, expression)
 *  
 * RETURNS:
 *
 *      msg_success or msg_failure
 *       
 * ARGUMENTS:
 *
 *      struct FATS *fats	- Pointer to FATS structure
 *	char *expression	- filename, info field
 *
-*/
int fat_scan_open (struct FATS *fats, char *expression) 
{
	struct SCAN *scanp;

	/*
	 * Compile the regular expression that denotes the pathname
	 */
	fats->re = regexp_fat (expression);
	if (fats->re == 0) {
	    (*fats->print) ("FAT: poorly formed regular expression /%s/\n", 
				expression);
	    return msg_failure;
	}

	/*
	 * Initialize the header for the scan chain.
	 */
	fats->scanh.flink = & fats->scanh.flink;
	fats->scanh.blink = & fats->scanh.flink;

	/*
	 * set up a stream for the root directory
	 */
	scanp = malloc (sizeof (struct SCAN));
	insq (scanp, &fats->scanh);
	fat_stream_open (fats, & scanp->sp, 0);

	return msg_success;
}


/*+
 * ============================================================================
 * = fat_scan_next - Visit the entire directory tree.                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	This scheme enumerates the entire directory tree, which is expensive 
 *	on large disks.  Prune the tree based on the regular expression 
 *	success in matching.
 *~ 
 *	e.g     root -> file
 *	                subdir -> file
 *                              subdir -> ....
 *	                file <-
 *	        root <-
 *~
 *	The scan chain starts from the root directory, keep scanning the next
 *	name, if the entry is a subdirectory, then open a subdir stream and 
 *	read entries in the subdirectory, when it's done, return to its parent
 *	dir and continue, finally reach the root again to end the chain. 
 *
 * 	If it's a file, validate its current path and name, return the next 
 *	name in the FAT namespace that matches the regular expression.  Prune 
 *	off nodes (i.e. subdirectory tress) that will never match.
 *
 * FORM OF CALL:
 *  
 *      fat_scan_next (fats, de)
 *  
 * RETURNS:
 *
 * 	True if there's a name matched, and false if there isn't.
 *       
 * ARGUMENTS:
 *
 *      struct FATS *fats 	- Pointer to FATS structure
 *	struct direntry *de     - Pointer to direntry structure
 *
-*/
int fat_scan_next (struct FATS *fats, struct direntry *de) 
{
    unsigned int	bytes_read;
    struct SCAN *scanp, *oldscanp, *newscanp;
    int tmp;	
    char *cp;  		/* current path */
    char	filename [8+3+2];

    /* forward loop, starting from the root */
    while (1) {
	/*
	 * Advance the scan chain to the next name in the tree.
	 * start with bottom most scan context 
	 */
	scanp = fats->scanh.blink;	

        /* 
	 * backward loop, when a subdirectory stream is done, resume to
	 * its parent dir 
         */
	do {
	    if (killpending ()) return 0;   /* exit if the job is killed */

	    /* keep reading in the next directory entry  */
            bytes_read = fat_stream_read (fats, &scanp->sp, 
		sizeof (struct direntry), &scanp->de);

            /* not end yet, stay on the current stream  */
	    if (bytes_read != 0) break;  

            /* 
	     * exhaust this stream, that means this subdirectory is finished,
  	     * close out stream and scan context, resume to its parent dir 
	     */
	    fat_stream_close (&scanp->sp);
	    oldscanp = scanp->blink;
	    remq (scanp);
	    free (scanp);
	    scanp = oldscanp;

	    /* all done if we just close out the root directory */
	    if (scanp == & fats->scanh.flink) {
		return 0;
	    }
	}while(1);   /* end backward loop */

	/*
	 * If the current entry is a directory, then advance down the chain.
	 * Don't advance down the "." and ".." entries.
	 */
	if (isadir (&scanp->de) && (scanp->de.filename [0] != '.')) {
	    newscanp = malloc (sizeof (*newscanp));
	    insq (newscanp, scanp);
	    fat_stream_open (fats, &newscanp->sp, &scanp->de);
            vbo_to_lbo( fats, &newscanp->sp, &newscanp->sp.direntlbo, &tmp);
	    scanp = newscanp;
	}

        else {
	    /*
	     * certain files are not looked at
	     */
	    if (!isvalid_de (&scanp->de)) continue;
/*	    if (scanp->de.attribute & FATTR_HIDDEN) continue;*/
	    if (scanp->de.attribute & FATTR_VOLUME) continue;

	   /*
	    * if valid file, construct the current pathname from the scan chain.
	    */
	    cp = & fats->curpath [0];
	    *cp = 0;
	    scanp = fats->scanh.flink;
	    do {fat_unparsefname (filename, scanp->de.filename, 
					scanp->de.extention);
	        cp += sprintf (cp, "%s%s", (scanp->blink == & fats->scanh) 
				? "" : "\\", filename);
	        scanp = scanp->flink;
	    } while (scanp != & fats->scanh.flink);
		       
	    /*
	     * If the path matches the regular expression, return the 
	     * directory entry of the last component of the path.
	     */
	    if (regexp_match (fats->re, fats->curpath)) {
	        scanp = fats->scanh.blink;
		memcpy (de, &scanp->de, sizeof (struct direntry));

		/* record dir lbo */
		scanp->sp.offset -= sizeof(struct direntry);
		vbo_to_lbo(fats, &scanp->sp, &fats->rws.direntlbo, &tmp); 
		scanp->sp.offset += sizeof(struct direntry);

	        return 1;
            }
        } /* end else */
    }  /* end forward loop */
}


/*+
 * ============================================================================
 * = fat_scan_close - Free the scan chain.                         	      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Release all the structures associated with a scan path, e.g close out
 *	the stream and free the scan chain.
 * 
 * FORM OF CALL:
 *  
 *      fat_scan_close (fats)
 *  
 * RETURNS:
 *
 *      don't care
 *       
 * ARGUMENTS:
 *
 *      struct FATS *fats	- Pointer to FATS structure
 *
-*/
void fat_scan_close (struct FATS *fats) 
{
	struct SCAN *scanp;

	while (fats->scanh.flink != & fats->scanh.flink) {
	    scanp = fats->scanh.flink;
	    fat_stream_close (&scanp->sp);
	    remq (scanp);
	    free (scanp);
	}
}


/*+
 * ============================================================================
 * = fat_stream_open - Set up a stream context.                               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Given a directory entry, set up a stream context so that we may
 * 	read from or write to that file.  Each directory/file has its own 
 *	stream starting offset = 0.
 *
 *	Also handle the case where the directory is the root directory, which 
 * 	has special semantics. We assume root directory stream if de is null.
 * 
 * FORM OF CALL:
 *  
 *      fat_stream_open (fats, sp, de)
 *  
 * RETURNS:
 *
 *      don't care
 *       
 * ARGUMENTS:
 *
 *      struct FATS *fats 	- Pointer to FATS structure
 *	struct FAT_STREAM *sp   - Pointer to FAT_STREAM structure
 *	struct direntry *de	- Pointer to direntry structure
 *      
-*/
void fat_stream_open (struct FATS *fats, struct FAT_STREAM *sp, struct direntry *de)
{
	unsigned int	num_dir;
        
	/*
	 * Special case the root directory by reading in the entire directory.
	 */
	if (de == 0) {
	    memset (sp, 0, sizeof (*sp));
 	    sp->isroot = 1;
	    num_dir = getnum (fats->bootblock.num_root_dir_entries);
	    sp->filelen = num_dir * sizeof (struct direntry);
	    sp->buf = malloc (sp->filelen);
	
	    /* calculate the sector where root dir starts */	
            sp->direntlbo = ( getnum (fats->bootblock.reserved_sectors) + 
                       	      getnum (fats->bootblock.num_FATs) *
                       	      getnum (fats->bootblock.sectors_per_FAT)
			    )*SECSIZE;

	    /* load the MCB for the root dir */	
	    load_mcb (fats, sp, 0);
	    print_mcb(sp);	

            /* move the file pointer to the beginning of root dir, then read */
	    fseek (fats->fp, sp->direntlbo, SEEK_SET);
	    fread (sp->buf, 1, num_dir * sizeof (struct direntry), fats->fp);
	    return;
	}

	/*
	 * If this is a subdirectory, we set a file size to infinite, and
	 * count on the terminating cluster number to catch the EOF
 	 * condition (subdirectory entries have 0 length).
 	 * Otherwise, we use the file length in the directory entry.
	 */
	sp->isroot = 0;        
	if (de->attribute & FATTR_DIRECTORY) {
            sp->filelen = INT_MAX;
	} else {
	    sp->filelen = getnum (sp->de.file_size);
	}

	memcpy (&sp->de, de, sizeof (*de));

	/* all files have a buffer passed in from the caller */
	sp->buf = malloc (sizeof (*de) );

        /* Load the MCB for this file */
        load_mcb( fats, sp, 0);
        print_mcb( sp );
}


/*+
 * ============================================================================
 * = fat_stream_read - Read a directory entry or a file stream.               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Read bytes from a stream at the current offset and return the number of
 * 	bytes read.  Meantime adjust the offset.  For scanning the file headers,
 *	bytes_requested is sizeof(struct direntry) 32, the read size varies if
 *	read the file contents.  Some partitions are necessary if it doesn't
 *	start or end at the sector boundary.  
 * 
 * FORM OF CALL:
 *  
 *      fat_stream_read (fats, sp, bytes_requested, buf)
 *  
 * RETURNS:
 *
 *      xfer_count which is total number of bytes read
 *       
 * ARGUMENTS:
 *
 *      struct FATS *fats 	- Pointer to FATS structure
 *	struct FAT_STREAM *sp   - Pointer to FAT_STREAM structure
 *	int bytes_requested	- bytes requested to read
 *	unsigned char *buf	- buffer storing either entry header or file
 *			          contents
 *
-*/
int fat_stream_read (struct FATS *fats, struct FAT_STREAM *sp, 
		int bytes_requested, unsigned char *buf) 
{
        unsigned char 	*tmpbuf;
	unsigned int 	lbo;
        unsigned int 	run_count;      /* run count for the lbo */
	unsigned int	off_sect;       /* bytes off the sector boundary */ 
	unsigned int    bytes_remaining;
	unsigned int	bytes_read;   	/* bytes read for each fread() */ 	
	unsigned int	xfer_count; 	/* total bytes read for a call */

        xfer_count = 0;
        bytes_remaining = bytes_requested;

	/*
	 * Special case the root directory, get each file header, 32 bytes
	 */
	if (sp->isroot) {
	    bytes_read = min (bytes_requested, sp->filelen - sp->offset);
	    memcpy (buf, sp->buf + sp->offset, bytes_read);
	    sp->offset += bytes_read;    /* move to the next entry header */
	    return bytes_read;
	}

        while ( bytes_remaining && (sp->offset < sp->filelen) ) {

	    /* Get the LBO for the run denoted by the file position */

	    sp->status = vbo_to_lbo (fats, sp, &lbo, &run_count);
 	    if ( sp->status != msg_success )                          
                return 0;

	    /*
             * If the current position isn't on a sector boundary(lbo is not 
	     * the multiple of SECSIZE 512) or the request is less then a
	     * sector, read the first sector into a local buffer and copy 
	     * the data.  
             */
	    off_sect = lbo & (SECSIZE - 1);  	/* equiv. lbo mod SECSIZE */
	    dprintf("off_sect = %x hex\n", off_sect);

            if ( (off_sect != 0) || (bytes_remaining < SECSIZE) ) {
		dprintf("bytes_remaining = %x hex \n", bytes_remaining);
                tmpbuf = malloc (SECSIZE);

                /* 
	         * Seek to the start of the sector and read it all.  It
	         * MUST start at the sector boundary, otherwise, fread will
	         * fail. thus fseek (lbo-off_sect) bytes then later, 
	         * start copying to buf from (tmpbuf+off_sect)	 
	         */
                fseek (fats->fp, lbo-off_sect, SEEK_SET);
                if ( fread(tmpbuf, 1, SECSIZE, fats->fp) != SECSIZE){
		    free(tmpbuf); 
                    return 0;
		}

		/* Don't include data beyond the end of request */
                bytes_read = min3 ( SECSIZE - off_sect, bytes_remaining, 
				sp->filelen - sp->offset );

                /* Copy the remaining part of the sector to the users buffer */
                memcpy (buf, tmpbuf+off_sect, bytes_read);

                /* adjust pointers accordingly */
                buf += bytes_read;
                sp->offset += bytes_read;
                xfer_count += bytes_read;
	    	bytes_remaining -= bytes_read;
                free (tmpbuf);
		continue;
	    }

            /* 
             * If the request is less than the run, read only the request.
             * Also, if the request doesn't end on a sector boundary, stop 
             * short so we read the last sector separtely.
             */
            if ( bytes_remaining < run_count ) {
                run_count = bytes_remaining;
                off_sect = bytes_remaining & (SECSIZE - 1);
                if ( off_sect !=0 ) 
                    run_count -= off_sect;
            }

            fseek (fats->fp, lbo, SEEK_SET);
            if ( fread(buf, 1, run_count, fats->fp) != run_count)
		return 0;

	    /* 
	     * Read data is limited by the EOF and the amount left in the run
             */
	    dprintf("remaining=%x, run_count=%x, filelen-offset=%x hex\n",
		bytes_remaining, run_count, sp->filelen-sp->offset);	
            bytes_read = min3 ( bytes_remaining, run_count,
                		sp->filelen - sp->offset );	
                
	    /* adjust pointers accordingly */
            buf += bytes_read;
            sp->offset += bytes_read;
            xfer_count += bytes_read;
            bytes_remaining -= bytes_read;
	}
        dprintf("read xfer_count = %x hex \n", xfer_count);
	return xfer_count;  /* total bytes read */
}


/*+
 * ============================================================================
 * = fat_stream_write - Write file to the underneath driver.                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Bytes requested could be any size.  First allocate new clusters if
 *	needed.  Get the lbo from MCB, write to the disk and adjust all 
 *	relevant pointers until all requested bytes are written.  Update the
 *	file_size in dir entry. 
 * 
 * FORM OF CALL:
 *  
 *      fat_stream_write (fats, sp, bytes_requested, buf)
 *  
 * RETURNS:
 *
 *      bytes_requested if success, or 0 if any error
 *       
 * ARGUMENTS:
 *
 *      struct FATS *fats 	- Pointer to FATS structure
 *	struct FAT_STREAM *sp   - Pointer to FAT_STREAM structure
 *	int bytes_requested	- bytes requested to write
 *	unsigned char *buf	- file contents
 *
-*/
int fat_stream_write (struct FATS *fats, struct FAT_STREAM *sp, 
		int bytes_requested, char *buf)
{
	unsigned int 	lbo;
        unsigned int 	run_count;      /* run count for the lbo */
  	unsigned int	write_size;
	unsigned int	bytes_remaining;
	unsigned int	bytes_written;  /* bytes written for each disk write */

        dprintf("fat_stream_write \n");
        bytes_remaining = bytes_requested;
        
	sp->status = allocate_clusters (fats, bytes_requested);
	if (sp->status != msg_success)
	    return 0;

	/* If this is the first time that we are writing, offset == 0, then
	 * load up the cluster pointers. */
        if (sp->offset == 0)
	    fat_stream_open(fats, sp, &sp->de);

        /*
         * Write the the data in runs (streams of contiguous clusters). The MCB 
         * provides the map of these clusters.
         */
        while( bytes_remaining ){
	    sp->status = vbo_to_lbo( fats, sp, &lbo, &run_count);
	    if (sp->status != msg_success)
	  	return 0;
	    write_size = min(run_count, bytes_remaining);
	    bytes_written = fat_disk_write(fats, lbo, write_size, buf);

            /* 
	     * write data is limited by the EOF and the amount left 
	     * in the run 
	     */
	    bytes_written = min(bytes_written, bytes_remaining);
	
	    /* adjust pointers accordingly */
	    buf += bytes_written;
	    sp->offset += bytes_written;
	    bytes_remaining -= bytes_written;
 	}	

        /* if no error, all bytes_requested should be written */
        /* update file size in de, be careful! file_size is 4 bytes */
        sp->filelen += bytes_requested;
        setnum( sp->de.file_size, sp->filelen );
        fat_disk_write( fats, sp->direntlbo, sizeof(struct direntry), 
				&sp->de );

    	return bytes_requested;
}


/*+
 * ============================================================================
 * = fat_stream_close - Close out a stream.                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *     	
 *	If the stream was open for write, then we have to flush the buffer 
 *	out to disk. 
 * 
 * FORM OF CALL:
 *  
 *      fat_stream_close (sp)
 *  
 * RETURNS:
 *
 *      NONE
 *       
 * ARGUMENTS:
 *
 *	struct FAT_STREAM *sp - Pointer to FAT_STREAM structure      
 *
-*/
void fat_stream_close (struct FAT_STREAM *sp) 
{
	free (sp->buf);
}


/*+
 * ============================================================================
 * = fat_disk_write - Generic fwrite to the underlying driver.                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Issue a write to the disk. Do any necessary partitioning of the request
 *	so that the downstream driver is accessed on sector boundaries. 
 *
 * FORM OF CALL:
 *  
 *      fat_disk_write ()
 *  
 * RETURNS:
 *
 *      bytes_written
 *       
 * ARGUMENTS:
 *
 *    	struct FATS *fats	- Pointer to FATS structure
 *	int lbo			- offset
 *	int bytes_requested	- bytes requested to be updated  
 *	unsigned char *buf	- write contents 
 *      
-*/
int fat_disk_write( struct FATS *fats, int lbo, int bytes_requested, 
			unsigned char *buf )
{
    unsigned char *tmpbuf;    	/* buffer for read/write to the disk */
    unsigned int off_sect;	/* bytes off the sector boundary */
    unsigned int bytes_written; /* actual bytes written */
    unsigned int remainder;

    dprintf("fat_disk_write\n");
    /*
     * If the current position isn't on a sector boundary or the request is
     * less then a sector; read the the sector, merge the write data and 
     * write back the sector.
     */
    off_sect = lbo & (SECSIZE - 1);
    if ( (off_sect != 0) || (bytes_requested < SECSIZE) ) {

        tmpbuf = malloc (SECSIZE);

        /*
	 * Seek to the start of the sector and read in an entire sector
 	 * MUST start at the sector boundary, otherwise fread will fail 
  	 */
        fseek( fats->fp, (lbo - off_sect), SEEK_SET );
        if ( fread( tmpbuf, 1, SECSIZE, fats->fp ) != SECSIZE) {
            free( tmpbuf );
            return 0;
        }

        bytes_written = min( SECSIZE - off_sect, bytes_requested );

        /* Merge the data to be written with that read from the disk */
        memcpy( (tmpbuf+off_sect), buf, bytes_written );

        /* Now write the sector back with the merged data */
        fseek( fats->fp, (lbo - off_sect), SEEK_SET );
        if ( fwrite( tmpbuf, 1, SECSIZE, fats->fp ) != SECSIZE ) {
            free( tmpbuf );
            return 0;
        }

        free( tmpbuf );

    } else {
  
        /*
         * Here the data is aligned on a sector boundary. If the request 
         * doesn't end on a sector, stop short so we come back and read the 
         * last sector separately.
         */
        remainder = bytes_requested & (SECSIZE - 1);
        if ( remainder )
            bytes_requested -= remainder;

        fseek( fats->fp, lbo, SEEK_SET );
        bytes_written = fwrite( buf, 1, bytes_requested, fats->fp );

    }

    return bytes_written;
}


/*+
 * ============================================================================
 * = load_mcb - Load the Map of Contiguous Blocks.                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  	For each file, a MCB is built to describe the groups of continguous 
 *	clusters that make up the file because some older disks might have
 *	deleted some files and that free space is reclaimed, resulting 
 *	fragmentation.   
 *
 *	For instance, a file might consist of multiple contiguous clusters, e.g.
 *	clusters 2-4 and 10-13.  MCB will record these information VBO, LBO
 *	(bytes offset from the beginning of a file) for each contiguous section
 *	and inuse denoting # of segments. In this case, there would be 2 runs:
 *	inuse=1, vbo[0]=0, vbo[1]=3*clustsize, lbo[0]= 4200 hex
 *	inuse=2, vbo[1]=3*clustsize, vbo[2]=7*clustsize, lbo[1]=5200 hex
 *
 *	startvbo is the lower range of vbo. for the first load_mcb call, 
 *	it just opens a stream, startvbo is 0.
 *
 *	For the special case, root directory, MCB maps the entire root directory
 *	section which marks the lbo of dir entries.  
 *
 * FORM OF CALL:
 *  
 *      load_mcb (fats, sp, startvbo)
 *  
 * RETURNS:
 *
 *      msg_success
 *       
 * ARGUMENTS:
 *
 *	struct FATS *fats	- Pointer to FATS structure      
 *	struct FAT_STREAM *sp	- Pointer to FAT_STREAM structure      
 *	int startvbo		- first vbo in the run
 *
-*/
int load_mcb (struct FATS *fats, struct FAT_STREAM *sp, int startvbo )
{
    struct FAT_MCB *mcb;
    int fat_entry;
    int clustsize;
    unsigned int vbo, lbo;

    mcb = &sp->mcb;
    mcb->inuse = 0;
    mcb->vbo[0] = 0;

    clustsize = fats->clustsize ;

    /* Check if this is the root directory.  If it is then build a single run
     * MCB entry for it. */
    if (sp->isroot){
	mcb->inuse = 1;
	mcb->lbo[0] = sp->direntlbo;
	mcb->vbo[1] = sp->filelen;
        return msg_success;
    }

    fat_entry = getnum (sp->de.start_cluster);
    dprintf("start_cluster = %d \n", fat_entry);

    /* 
     * Scan through the fat until we reach the vbo we're after and then build 
     * the mcb for the file
     */
    for (vbo = clustsize; vbo <= startvbo; vbo += clustsize) {

        /*
         * Check if the file does not have any allocation beyond this point in
         * which case the mcb we return is empty
         */
        if ( fat_entry >= fats->lastindex) 
            return msg_success;

        fat_entry = getfat (fats, fat_entry);
    }

    /* 
     * We need to check again if the file does not have any allocation beyond 
     * this point in which case the mcb we return is empty
     */
    if ( fat_entry >= fats->lastindex )  
        return msg_success;

    /* 
     * At this point fat_entry denotes another cluster, and it happens to be 
     * the cluster we want to start loading into the MCB.  Set up the first 
     * run in the MCB to be this cluster, with a size of a single cluster.
     */
    mcb->inuse = 1;
    mcb->vbo[0] = vbo - clustsize;
    mcb->lbo[0] = fatindex_to_lbo( fats, fat_entry );
    mcb->vbo[1] = vbo;

    dprintf("lbo[0]=x, vb0[0]=%x, vbo[1]=%x hex\n", 
	   mcb->lbo[0], mcb->vbo[0], mcb->vbo[1]);

    /*
     * Now we'll scan through the fat chain until we either exhaust the 
     * chain or fill up the MCB
     */
    while (1) {

        /* Get the next fat entry and check if it's the last entry */

        fat_entry = getfat (fats, fat_entry);
       	dprintf("fat_entry in while = %d \n", fat_entry);
 	if ( fat_entry >= fats->lastindex )
            return msg_success;

        /*
         * Now calculate the lbo for this cluster          
	 */                      
        lbo = fatindex_to_lbo (fats, fat_entry);
        dprintf("lbo=%x hex\n", lbo);
        /* 
         * It is a continuation of the previous run if the lbo of the last run
	 * plus the current size of the run is equal to the lbo for the next 
	 * cluster.  If it is a continuation then we only need to add a cluster
	 * amount to the last vbo to increase the run size.  If it is a new run
 	 * then we need to check if the run will fit, and if so then add in the
         * new run.
         */ 
        if ((mcb->lbo[mcb->inuse-1] + 
             (mcb->vbo[mcb->inuse] - mcb->vbo[mcb->inuse-1])) == lbo) {

            dprintf("contiunue last run\n");
            mcb->vbo[mcb->inuse] += clustsize;
        } else {
            dprintf("new run \n");
            if ( (mcb->inuse + 1) >= FAT_MAXIMUM_MCB ) 
                return msg_success;

            mcb->inuse += 1;
            mcb->lbo[mcb->inuse-1] = lbo;
            mcb->vbo[mcb->inuse] = mcb->vbo[mcb->inuse-1] + clustsize;
        }
    }
}


/*+
 * ============================================================================
 * = vbo_to_lbo - Get lbo from a given vbo.                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Look up the requested VBO (stream offset) in the streams MCB, then 
 *	return the corresponding LBO (bytes offset from the beginning of 
 *	a file) and # of bytes denoted by the run.  
 *	 
 * FORM OF CALL:
 *  
 *      vbo_to_lbo (fats, sp, lbo, run_count)
 *  
 * RETURNS:
 *
 *      msg_success if in MCB
 *       
 * ARGUMENTS:
 *
 *      struct FATS *fats	- Pointer to FATS structure
 *	struct FAT_STREAM *sp 	- Pointer to FAT_STREAM structure
 *	int	*lbo		- Pointer to int, the new lbo will be returned
 *	int 	*run_count	- Pointer to int, return # of bytes remained
 *				  within the VBO range 
 *
-*/
int vbo_to_lbo ( struct FATS *fats, struct FAT_STREAM *sp, 
		unsigned int *lbo, int *run_count)
{
    struct FAT_MCB *mcb;
    unsigned int vbo;
    int i;

    mcb = &sp->mcb;
    vbo = sp->offset;

    /* 
     * Check that the MCB contains the VBO we're looking for. 
     * load in the MCB for this VBO.
     */
    if ( (vbo < mcb->vbo[0]) || (vbo >= mcb->vbo[mcb->inuse]) ) 
        load_mcb ( fats, sp, vbo );    

    /*
     * Now search for the slot in the MCB where the VBO fits
     */
    for ( i = 0; i < mcb->inuse; i++ ) {

        if ( vbo < mcb->vbo[i+1] ) {
            *lbo = mcb->lbo[i] + (vbo - mcb->vbo[i]);   /* increase lbo */
            *run_count = mcb->vbo[i+1] - vbo;           /* decrease run_count*/
	    dprintf("lbo is %x hex and count = %x hex\n", *lbo, *run_count);	
            return msg_success;
        }
    }

    return msg_failure;
}


/*+
 * ============================================================================
 * = fatindex_to_lbo - Convert index to lbo.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      fat_entry indicates the cluster that the data occupies.  This routine 
 *	translates it into lbo, bytes off the beginning of a file. 
 *
 *	sfa (Start of File Area) is after reserved sectors, FAT, Root dir
 *	headers, (for example, 3+1/2 HD, sfa=1+2*9+14=33). 
 *
 * Doc:	The data area is divided into a number of clusters of equal size
 *	specified in byte 13 of boot sector. And each FAT entry(index) except 
 *	entries 0 and 1, is associated with the cluster with the same number 
 *	as the entry.  FAT entry 0 contains the media descriptor byte padded 
 *	with 1s. Entry 1 contains all 1s.  To be consistent with FAT, the 
 *	cluster numbering starts with 2. Thus fat_entry subtracts 2. 
 *
 * FORM OF CALL:
 *  
 *      fatindex_to_lbo (fats, fat_entry)
 *  
 * RETURNS:
 *
 *      lbo
 *       
 * ARGUMENTS:
 *
 *      struct FATS *fats	- Pointer to FATS structure
 *	int	fat_entry	- sector number off the data area
 *
-*/
int fatindex_to_lbo (struct FATS *fats, int fat_entry) 
{
    int lbo;  	/* in unit of bytes */

    lbo = ( (fat_entry - 2) * getnum (fats->bootblock.sectors_per_clust) +
	fats->sfa) * SECSIZE;

    return lbo;
}


/*+
 * ============================================================================
 * = getfat - Get the next fat entry.                  			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Get contents of current fat entry which also points to the next fat 
 *	entry in this file.
 *
 * Doc:	To locate a file, the file's directory entry contains its beginning FAT
 *	entry number.  This FAT entry, in turn, contains the entry number of
 *	the next cluster.  If a file has size of 10 clusters, it will have 10 
 *	FAT entries and 9 links(called chain). If the entry has all 1s (FFF), 
 *	it marks the end of the cluster chain. 
 *
 * 	Since 12 bit entry is suitable for floppy, here 12 bit is used to 
 *	explain how to read the FAT entry. Each group of 3 consecutive bytes 
 *	contains 2 12-bit entries, arranged as follows:
 *##
 *	1 1st byte contains the 8 least significant bits of the first entry
 *	2 lower nibble of 2nd byte contains the 4 most significant bits of  
 *	   the first entry	 
 *      3 higher nibble of 2nd byte contains the 4 least significant bits of
 *	   the second entry.
 *	4 3rd byte contains the 8 most significant bits of the second entry
 *#
 *	For example, if AB, CD and EF are the hex of the 3 consecutive bytes,
 *	then the entries are DAB and EFC in 12-bit representation.
 *
 * 	For 16-bit entry, if AB, CD, EF and GH are hex of 4 consecutive bytes,
 *	then the entries are CDAB and GHEF.
 *
 *	not sure about 32-bit entry.  maybe GHEFCDAB is one entry
 *
 * FORM OF CALL:
 *  
 *      getfat(fats, entry)
 *  
 * RETURNS:
 *
 *      the next fat entry, which is also the contents of current entry
 *       
 * ARGUMENTS:
 *
 *	struct FATS *fats	- Pointer to FATS structure, get fatbits and fat
 *      int entry		- index from 1 to fatlen
 *
-*/
int getfat (struct FATS *fats, int entry) {
 	unsigned char *fat;		      
	int	nibs_per_fat;
	int	nibx;
	int	nibble;
	int   	i;
	int   	next_entry;

        fat = fats->fat;
	next_entry = 0;

        /* number of nibbles are required for each fat entry */
	nibs_per_fat = fats->fatbits / 4;    

        /* start from the higher byte */
	nibx = entry * nibs_per_fat + nibs_per_fat - 1;
	for (i=0; i<nibs_per_fat; i++) {
	    if (nibx & 1) {
		nibble = fat [nibx/2] >> 4;  /* get high nibble if odd */
	    } else {
		nibble = fat [nibx/2] & 0x0f;  /* get low nibble if even */
	    }
	    next_entry <<= 4;
	    next_entry += nibble;
	    nibx--;
	}
	return next_entry;
}


/*+
 * ============================================================================
 * = get_fatbits - Determine the file system type 12, 16 or 32 bits.          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Whether 12-bit or 16-bit entries are used depends on the number of
 *	cluster in the data area, if the range from 002 to FEF is enough to 
 *	number them all, 12-bit entries are used, otherwise 16-bit.
 *
 *: 	Interpretation of FAT entry
 *	----------------------------------------------------------------------
 *	12-bit        | 16-bit     |    meaning
 *	----------------------------------------------------------------------
 *	000	      | 0000	   | 	unassigned and available cluster
 *   		      |		   | 	
 *	001	      | 0001	   | 	invalid entry
 *                    |   	   | 
 *	002-FEF	      | 0002-FFEF  | 	assigned, link to next cluster in file
 *                    |  	   | 
 *	FF0-FF6	      | FFF0-FFF6  | 	reserved
 *                    |            |
 * 	FF7	      | FFF7	   | 	bad cluster, can't be used  
 *                    |            |
 *      FF8-FFF	      | FFF8-FFFF  | 	last cluster assigned to a file or dir
 *:	----------------------------------------------------------------------
 *
 * Note:The fs_type from bytes 36-3D in boot sector has format of e.g. FAT16, 
 *	It does NOT always mean 16-bit FAT?  That information is filled in when 
 *	try to boot from an unbootable disk.
 *	Thus, finding fat bits from fs_type is taken out by the always false 
 *	condition "if (0 && strstr(cp, "12")".
 *
 * FORM OF CALL:
 *  
 *      get_fatbits (fats)
 *  
 * RETURNS:
 *
 *      fatbits 12 or 16 (32 is not tested)
 *       
 * ARGUMENTS:
 *
 *      struct FATS *fats  - Pointer to FATS structure
 *      
-*/
int get_fatbits (struct FATS *fats) {
	struct msdos_bootblock *bbp;
	unsigned char *cp;
	int disk_sectors;
	int disk_clusters;
	int estimated_width;

/*
 *
 * We would really like to use the header for the fat type, but as
 * with many PC programs do not all of them fill in the header fully
 * or correctly. So we must determine this by sizing the disk...
 *
 *	cp = fats->bootblock.fs_type;
 *	if (0 && strstr (cp, "12")) {
 *	    fats->fatbits = 12;
 *	    fats->lastindex = 0x0ff0;  
 *	    return 12;
 *
 *	} else if (0 && strstr (cp, "16")) {
 *	    fats->fatbits = 16;
 *	    fats->lastindex = 0xfff0;
 *	    return 16;
 *
 *	 * otherwise determination is more difficult...
 *	 * We estimate by looking at total number of clusters on the disk,
 *	 * and seeing how many bits would be needed to encode that number.
 *	 *
 *	} else {
 *
 */
	    bbp = & fats->bootblock;
	    disk_sectors = getnum (bbp->sec32) + getnum (bbp->total_sectors);
	    disk_clusters = disk_sectors / getnum (bbp->sectors_per_clust);
	    estimated_width = (getnum (bbp->sectors_per_FAT) * SECSIZE *
		8 * 10) / disk_clusters;
	    qprintf ("%d estimated fat table width\n", estimated_width);

	    if (estimated_width > 140) {
		fats->fatbits = 16;
		fats->lastindex = 0xfff0; /* include reserved + bad clusters */
                fats->last_cluster = 0xffff;  /* used to mark chain end */	

	    } else {
		fats->fatbits = 12;
		fats->lastindex = 0x0ff0; /* include reserved + bad clusters */
		fats->last_cluster = 0xfff;   /* used to mark chain end */
	    }

	return fats->fatbits;
}


/*+
 * ============================================================================
 * = setfat - Link the next fat entry.                 			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Change the FAT table to have the current entry points to the next entry
 *      This is a reverse process of getfat();
 *
 * Doc:	To locate a file, the file's directory entry contains its beginning FAT
 *	entry number.  This FAT entry, in turn, contains the entry number of
 *	the next cluster.  If a file has size of 10 clusters, it will have 10 
 *	FAT entries and 9 links(called chain). If the entry has all 1s (FFF), 
 *	it marks the end of the cluster chain. 
 *
 * 	Since 12 bit entry is suitable for floppy, here 12 bit is used to 
 *	explain how to read the FAT entry. Each group of 3 consecutive bytes 
 *	contains 2 12-bit entries, arranged as follows:
 *## 
 *	1 1st byte contains the 8 least significant bits of the first entry
 *	2 lower nibble of 2nd byte contains the 4 most significant bits of  
 *	   the first entry	 
 *      3 higher nibble of 2nd byte contains the 4 least significant bits of
 *	   the second entry.
 *	4 3rd byte contains the 8 most significant bits of the second entry
 *	For example, if AB, CD and EF are the hex of the 3 consecutive bytes,
 *	then the entries are DAB and EFC in 12-bit representation.
 *#
 * 	For 16-bit entry, if AB, CD, EF and GH are hex of 4 consecutive bytes,
 *	then the entries are CDAB and GHEF.
 *
 *	not sure about 32-bit entry.  maybe GHEFCDAB is one entry
 *
 * FORM OF CALL:
 *  
 *      setfat(fats, entry, next_entry)
 *  
 * RETURNS:
 *
 *	NONE      
 *       
 * ARGUMENTS:
 *
 * 	struct FATS *fats	- Pointer to FATS structure, get fatbits, fat
 *      int entry		- index from 1 to fatlen
 *	int next_entry		- the next entry to be linked	      
 *
-*/
void setfat ( struct FATS *fats, int entry, int next_entry)
{
	unsigned char *fat;
        int     nibs_per_fat;
        int     nibx;
        int     nibble;
        int     i;

	fat = fats->fat;

        /* number of nibbles are required for each fat entry */
        nibs_per_fat = fats->fatbits / 4;

        /* start from the lower nibble */
        nibx = entry * nibs_per_fat;
        for (i=0; i<nibs_per_fat; i++) {
            nibble = next_entry & 0X0f;      /* get last digit only */
            if (nibx & 1) {          /* change high nibble if odd */
                fat[nibx/2] = (fat[nibx/2] & 0x0f) | (nibble << 4);
            } else {                 /* change low nibble if even */
                fat[nibx/2] = (fat[nibx/2] & 0xf0) | nibble ;
            }
            nibx++;
            next_entry >>= 4;		/* move to higher byte */
        }

}


/*+
 * ============================================================================
 * = flush_fats - Update all copies of FATs.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Flush the in-memory copy of the FAT back to disk. 
 * 
 * FORM OF CALL:
 *  
 *      flush_fats (fats)
 *  
 * RETURNS:
 *
 *      NONE
 *       
 * ARGUMENTS:
 *
 * 	struct FATS *fats	- Pointer to FATS structure	     
 *      
-*/
void flush_fats ( struct FATS *fats )
{
    int num_fats;
    int offset, i;
    
    offset = getnum(fats->bootblock.reserved_sectors)*SECSIZE;
    fseek(fats->fp, offset, SEEK_SET);	/* FAT starts after reserved sector */

    /* update all copies of FATS */
    num_fats = getnum( fats->bootblock.num_FATs );
    for ( i = 0; i < num_fats; i++ ) {
    	fwrite(fats->fat, 1, fats->fatsize, fats->fp);
    }	
}


/*+
 * ============================================================================
 * = allocate_clusters - Allocate new clusters if needed.                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	As the write size increases, check if more clusters are needed to 
 *	accomodate all bytes, then allocate new clusters and update the last 
 *	fat entry and total number of clusters had allocated to the file.  
 * 
 * FORM OF CALL:
 *  
 *      allocate_clusters (fats, bytes_requested)
 *  
 * RETURNS:
 *
 *      msg_success if success or msg_failure if no enough free clusters
 *       
 * ARGUMENTS:
 *
 *     	struct FATS *fats	- Pointer to FATS structure
 *	int bytes_requested	- Additional bytes required to write
 *      
-*/
int allocate_clusters(struct FATS *fats, int bytes_requested)
{
    struct FAT_STREAM *sp;
    unsigned char *tmpbuf;
    int    clustsize;
    int    total_clusters;
    int	   clusts_needed,       /* total clusters required */
           *clusts_exist; 	/* clusters already allocated */
    int    *prev_entry,
           cur_entry,
	   tmp_entry;
    int	   clust_lbo;

    dprintf("allocate clusters \n");
    sp = & fats->rws;	

    clustsize = fats->clustsize;

    /* adjust indexing because cluster numbering starts at 2 */	
    total_clusters = fats->total_clusters + 2;

    clusts_needed = (sp->filelen+bytes_requested + clustsize - 1)/clustsize;

    clusts_exist = & (sp-> clusts_exist);

    if (clusts_needed == *clusts_exist) 
	return msg_success;

    prev_entry = & (sp->prev_entry);	
    tmpbuf= malloc (clustsize);

    /* loop through clusters until allocate enough clusters */	
    for ( cur_entry = *prev_entry; cur_entry < total_clusters; cur_entry++ ){

        /* 
	 * See if this entry points to an available cluster. 
         * If not, keep looping.
         */
        tmp_entry = getfat (fats, cur_entry);
        if (tmp_entry != 0 ) 
            continue;

        /* 
	 * We've found a free cluster, add it to the chain.
	 * if first cluster is 0, it indicates this is a first run, then
	 * the first entry in chain is the current entry.  Otherwise, 
 	 * have the previous entry linked with the current entry.
	 * The most current entry is always the last entry. 
	 */
        if ( *prev_entry == 0 ) { 		/* 1st entry in chain */
                      /* set start_cluster */
            setnum( sp->de.start_cluster, (short)cur_entry );
            fat_disk_write( fats, sp->direntlbo, sizeof(struct direntry), 
			&sp->de);
        } else{		/* link the previous entry with the current one */	
            setfat( fats, *prev_entry, cur_entry );
        }
        
	/* set the current entry be the last entry */     
        setfat( fats, cur_entry, fats->last_cluster );

	/* now clear the cluster on the disk */
	clust_lbo = fatindex_to_lbo(fats, cur_entry);
	fseek(fats->fp, clust_lbo, SEEK_SET);
	fwrite(tmpbuf, 1, clustsize, fats->fp);

        *prev_entry = cur_entry;
	(*clusts_exist) ++;
        dprintf ("clusts_needed=%d, clusts_exist=%d, prev_entry=%d\n",
		  clusts_needed, *clusts_exist, *prev_entry);	

	if (clusts_needed == *clusts_exist){
	    flush_fats(fats);	
	    free(tmpbuf);	
	    return msg_success;
	}
	
    }
    (*fats->print)("FAT: Failed to allocate clusters.\n");	
    return msg_failure;  
}


/*+
 * ============================================================================
 * = dealloc_clusters - Release a chain of allocated clusters.                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      When a file is removed/rewrite, all allocated clusters must be 
 *	released, starting off the first cluster until it reaches the end of 
 *	the chain. 
 * 
 * FORM OF CALL:
 *  
 *      dealloc_clusters (fats, de)
 *  
 * RETURNS:
 *
 *      msg_success
 *       
 * ARGUMENTS:
 *
 *     	struct FATS *fats	- Pointer to FATS structure
 *	struct direnty *de	- Pointer to direntry struture 
 *      
-*/
int dealloc_clusters( struct FATS *fats, struct direntry *de )
{
    int	fat_entry;
    int cur_entry;
	
    fat_entry = getnum (de->start_cluster);
    while(fat_entry < fats->lastindex){
        cur_entry = fat_entry;
        fat_entry = getfat( fats, fat_entry);
        setfat( fats, cur_entry, 0);
    }
	
    fats->rws.prev_entry = 0;
    fats->rws.clusts_exist = 0;

    flush_fats(fats);	/* update fats */
    return msg_success;
}


/*+
 * ============================================================================
 * = set_dir_ent - Set up a directory entry.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Fill in elements defined in direntry structure, such as attribute,
 *	update_time and update_date.  Initialize file_size and start_cluster
 *	to be 0. 
 * 
 * FORM OF CALL:
 *  
 *      set_dir_ent (de)
 *  
 * RETURNS:
 *
 *      NONE
 *       
 * ARGUMENTS:
 *
 *	struct  direntry *de    	- Pointer to the new directory entry
 *      
-*/
void set_dir_ent (struct direntry *de)
{
    unsigned short date = 0; 		/* ensure date is 2 bytes */
    unsigned short time = 0;            /* ensure time is 2 bytes */
    struct toy_date_time toy;    	/* defined in time.h  */

    dprintf("set_dir_ent\n");	

    de->attribute = (unsigned char)(FATTR_ARCHIVE);

    get_date_time( &toy );

    date = (((toy.year - 80) & 0xff) << 9) |    /* bit 15-9 */
           ((toy.month & 0xf) << 5) |           /* bit  8-5 */
           (toy.day_of_month & 0x1f);           /* bit  4-0 */

    time = (toy.hours << 11) |           	/* bit 15-11 */
           ((toy.minutes & 0x03f) << 5) |       /* bit 10-5  */
           ((toy.seconds & 0x1f) / 2);          /* bit  4-0 */
    
    setnum( de->update_date, (unsigned short) date );
    setnum( de->update_time, (unsigned short) time );

    setnum( de->file_size, 0 );
    setnum( de->start_cluster, 0);

}


/*+
 * ============================================================================
 * = create_dir_ent - Create a new directory entry.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Assume it starts off root, go through each directory entry, if the entry
 *	is never used or has been erased, it will be used for a new file.    
 * 
 * FORM OF CALL:
 *  
 *      create_dir_ent (fats)
 *  
 * RETURNS:
 *
 *      msg_success if successful creation or msg_failure if no available entry
 *       
 * ARGUMENTS:
 *
 * 	struct FATS *fats	- Pointer to FATS structure      
 *      
-*/
int create_dir_ent( struct FATS *fats )
{
    int rootdir_size;
    int rootdir_lbo;
    unsigned int *lbo;         	/* new dir lbo */	
    unsigned int vbo;           /* vbo within root dir table */
    struct direntry tmp_de;  	/* store individual root dir entry */
    unsigned char *buf;         /* entire buffer for root dir */

    dprintf("create dir ent\n");
    lbo = &fats->rws.direntlbo;

    /* size allocated to root directory entries */
    rootdir_size = getnum( fats->bootblock.num_root_dir_entries ) *
                   sizeof( struct direntry );

    /* root directory offset from the file */ 
    rootdir_lbo = (getnum( fats->bootblock.reserved_sectors ) +
                   getnum( fats->bootblock.num_FATs ) *
                   getnum( fats->bootblock.sectors_per_FAT )) * SECSIZE;

    /* HACK! For now, assume the root dir */

    buf = malloc( rootdir_size );

    fseek (fats->fp, rootdir_lbo, SEEK_SET); 	/* seek to start of root dir */
    fread ( buf, 1, rootdir_size, fats->fp);    /* read all root dir entries */

    /* 
     *	Go through all root directory entries, exit the loop when an available
     *	entry(never used or deleted) is found 
     */
    for ( vbo = 0; vbo < rootdir_size; vbo += sizeof(struct direntry) ) {

        memcpy( &tmp_de, buf, sizeof(struct direntry) );

	/* found the first available entry */
	if (!isvalid_de(&tmp_de)){
            *lbo = vbo + rootdir_lbo;      /* update direntlbo */

            /* create a new directory entry in underneath driver */
            if ( fat_disk_write(fats, *lbo, sizeof(struct direntry), 
		     &fats->rws.de) != sizeof(struct direntry) ) 
		break;
            return msg_success;
        }
        buf += sizeof(struct direntry);     /* start at the next entry */
    }
    (*fats->print)("FAT: can't create a new entry\n");
    return msg_failure;
}


/*+
 * ============================================================================
 * = fat_validate_bootblock - Check ranges.                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Given a pointer to a bootblock, verify that it smells ok for this 
 *	driver. We do this by performing some simple range checks such as 
 *	bytes_per_sector, sectors_per_clust, num_FATs on cells.
 *
 * 	inrange(x, lo, hi) is defined in common.h, true if (x>=lo && x<hi)
 *
 * FORM OF CALL:
 *  
 *      fat_validate_bootblock (fats)
 *  
 * RETURNS:
 *
 *      1 if success, 0 if failure
 *       
 * ARGUMENTS:
 *
 *      struct FATS *fats  - Pointer to FATs structure	
 *      
-*/
int fat_validate_bootblock (struct FATS *fats) 
{
	struct msdos_bootblock *bbp;

	bbp = & fats->bootblock;

	/* range check bytes_per_sector */
	if (getnum (bbp->bytes_per_sector) != SECSIZE) 
		return 0;	

	/* range check sectors per allocatation */
	if (!inrange (getnum (bbp->sectors_per_clust), 1, 1024)) 
		return 0;

	/* range check number of FATs */
	if (!inrange (getnum (bbp->num_FATs), 1, 4)) 
		return 0;
 
	return 1;
}


/*+
 * ============================================================================
 * = isadir - Examine if it's a subdirectory.                  	              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Check if the structure is a valid directory name.
 * 
 * FORM OF CALL:
 *  
 *      isadir (de)
 *  
 * RETURNS:
 *
 *      1 if valid directory, otherwise 0
 *       
 * ARGUMENTS:
 *
 *      struct direntry *de	- pointer to struct direntry
 *
-*/
int isadir (struct direntry *de) 
{
	if (isvalid_de (de) && (de->attribute & FATTR_DIRECTORY)) {
	    return 1;
	} else {
	    return 0;
	}
}


/*+ 
 * ============================================================================ 
 * = isvalid_de - Test if it's a valid entry.                         	      =
 * ============================================================================
 * 
 * OVERVIEW: 
 *   
 * 	The very first byte of a directory entry has another function. If 
 *	hex e5 (FAT_DIRENT_DELETED) - a deleted file, subdir or voulume label. 
 *	hex 00 (FAT_DIRENT_NEVER_USED) - a terminating entry, never used and 
			there are no entries  
 *	hex 05 - (FAT_DIRENT_REALLY_0E5)
 *		E5 is also one of the extended character which is allowed in 
 *		the file names under DOS3.0 or later, to be compatible with 
 *		the original E5 represention of a deleted entry.  DOS 3.0
 *		converts the extended character E5 into 05 before writing it 
 *		to the directory entry. 
 * 
 * FORM OF CALL: 
 *  
 *      isvalid_de (de) 
 *   
 * RETURNS: 
 * 
 *      1 if valid direntry, otherwise 0
 *        
 * ARGUMENTS: 
 * 
 *      struct direntry *de	- pointer to struct direntry 
 *
-*/ 
int isvalid_de (struct direntry *de) 
{
	switch (de->filename [0] & 0xff) {
	case FAT_DIRENT_NEVER_USED:
	case FAT_DIRENT_DELETED:
	    return 0;      
	}
	return 1;
}


/*+
 * ============================================================================
 * = regexp_fat - Compile a regular expression denoting a FAT filename.       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Compile a regular expression that denotes a pathname on a FAT file
 * 	system.  Use traditionaly DOS conventions for wildcards, and
 *	additionally support wildcarding of directories, so that an entire
 *	tree may be searched without having to explicitly specify each
 *	directory level.
 *
 *	Constructs supported are:
 *~
 *		? 	matches any single character excluding backslash
 *		*	matches zero or more characters, including backslash
 *		[]	denotes a character class
 *      - more regexp functions in regexp.c
 *~
 * FORM OF CALL:
 *  
 *	regexp_fat (regular_expression)
 *  
 * RETURNS:
 *
 *      x - address of regular expression data structure REGEXP structure.
 *
 * ARGUMENTS:
 *
 *      char *regular_expression 	- address of null terminated string 
 *					  defining regular expression
 *
-*/
struct REGEXP *regexp_fat (char *expression) {
	struct TOKEN	*tp;
	struct REGEXP	*re;
	int		explength;
	unsigned int	c, term;
	int		i;
	extern unsigned int universal_class [];
	int		n;

	/*
	 * Allocate and initialize the tokens.  For an initial allocation,
	 * use the size of the string.  The number of tokens will always
	 * be less than this number.
	 */
	re = (void *) malloc (sizeof (struct REGEXP));
	explength = strlen (expression);
                                        /* struct TOKEN *tlist */
	re->tlist = (void *) malloc (explength * sizeof (struct TOKEN));
	memset (re->tlist, 0, explength * sizeof (struct TOKEN));
           
	/*
	 * Visit each character in the regular expression
	 */
	tp = re->tlist;

	n = 0;
	while (c = *expression++) {
            tp->minimum = 1;
	    tp->maximum = 1;
	
	    switch (c) {

	    /*
	     * Wildcard characters.
	     */
	    case '%':
	    case '?':
		memcpy (tp->chmask, universal_class, sizeof (tp->chmask));
		tp++;
		break;

	    /*
	     * Kleene closure. 0 or more occurances of anything.
	     */
	    case '*':
		memcpy (tp->chmask, universal_class, sizeof (tp->chmask));
		tp->minimum = 0;
		tp->maximum = INT_MAX;
		tp++;
		break;

	    case '[':
		if (compile_class (&expression, tp->chmask)) {
		    regexp_release (re);
		    return 0;
		}
		tp++;
		break;

	    /* ignore leading backslash if the first character */
	    case '\\':
		if (n == 0) {
		    continue;
		}
		/* fall through */

	    /*
	     * None of the special cases were flagged, so we treat
	     * the character as representing itself.
	     */
	    default:
		setbit (1, c, tp->chmask);
		tp++;
	    	break;
	    }

	    n++;
	}
	re->tcount = tp - re->tlist;


	/*
	 * revisit each token in the list and fold case
	 */
	for (tp=re->tlist, i=0; i<re->tcount; i++, tp++) {
	    regexp_foldcase (tp->chmask);
	}

	return re;
}


/*+
 * ============================================================================
 * = fat_parsefname - Parse a dotted finename into name and extension.	      =	
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Separate a dotted filename into name and extension.
 * 
 * FORM OF CALL:
 *  
 *      fat_parsefname (fname, name, ext)
 *  
 * RETURNS:
 *
 *      msg_success or msg_failure if null file name
 *       
 * ARGUMENTS:
 *
 *      char *fname	- full file name
 *	char *name	- name only
 *	char *ext	- extension only
 *
-*/
int fat_parsefname (char *fname, char *name, char *ext) 
{
	int   i;

	/* discard any leading '\' in the file name */
   	while ( fname[0] == '\\' ) 
            fname++;

	/* null filenames are illegal */
	if (strlen(fname) == 0){
	    pprintf("FAT: null file name.\n");
    	    return msg_failure;
	}

	memset (name, ' ', 8);
	memset (ext, ' ', 3);
	i = 0;
	while (i < 8 && *fname != '\0' && *fname != '.') {
	    *name++ = toupper (*fname);
	    fname++;
	    i++;
	}

	while (*fname != '\0' && *fname != '.') 
	    fname++;
	if (*fname == '.')     /* skip over the dot */
	    fname++;

	i = 0;
	while (i < 3 && *fname != '\0') {
	    *ext++ = toupper (*fname);
	    fname++;
	    i++;
	}

	return msg_success;
}             


/*+
 * ============================================================================
 * = fat_unparsefname - Create a full name.				      =	
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Take an root name and an extention and generate a full name.  Don't 
 *	include the separating period if the extention is blank.
 * 
 * FORM OF CALL:
 *  
 *      fat_unparsefname (fname, name, ext)
 *  
 * RETURNS:
 *
 *      msg_success
 *       
 * ARGUMENTS:
 *
 *      char *fname	- full file name
 *	char *name	- name only
 *	char *ext	- extension only
 *
-*/
void fat_unparsefname (char *fname, char *name, char *ext) 
{
	int	i;

	for (i=0; i<8 && *name != ' '; i++) *fname++ = *name++;
	*fname++ = '.';           /* add dot */
	for (i=0; i<3 && *ext != ' '; i++) *fname++ = *ext++;
	if (i == 0) fname--;      /* take out the dot if extension is blank */
	*fname++ = 0;
}


/*+
 * ============================================================================
 * = sprint_de - Format a msdos directory entry into a line.                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	sprint directory entry such as attributes, date and time, file length
 * 	and path.
 * 
 * FORM OF CALL:
 *  
 *      sprint_de(fats, cp, path, de)
 *  
 * RETURNS:
 *
 *      NONE
 *       
 * ARGUMENTS:
 *
 *      struct FATS *fats   - Pointer to msdos_bootblock structure
 *	char *cp	    - output of direntry entry	
 *	char *path	    - 	
 *	struct direntry *de - Pointer to direntry structure
 *
-*/
#if FAT_EXPAND
void sprint_de (struct FATS *fats, char *cp, char *path, struct direntry *de) 
{
	int	i;
	char	fattr_chars [] = "rhsvda67";
	unsigned int	date;
	unsigned int	time;

	/* 
	 * print the file attributes, attibute byte is 11th byte.
	 * bit	 7 - unused      '7'	6 - unsed  	'6'
	 * bit   5 - archive,    'a'	4 - subdir 	'd'
	 * bit   3 - volume      'v'	2 - system file 's'
	 * bit   1 - hidden file 'h'    0 - read only   'r'
	 */
	for (i=7; i>=0; i--) {
	    if (de->attribute & (1 << i)) {  /* check the i bit */
		cp += sprintf (cp, "%c", fattr_chars [i]);
	    } else {
		cp += sprintf (cp, "-");
	    }
	}

	/* 
	 * print the update date (bytes 24-25) and time(bytes 22-23)
	 * date: bits 15-9	years elapsed since 1980 (0-127)
	 *	       8-5	month (1=January... 12=December)
	 *	       4-0	day (1-31)
	 * time: bits 15-11	hour (0-23)
 	 * 	      10-5	minute (0-59)
 	 *	       4-0	double seconds (0-29)		 
	 */
	date = getnum (de->update_date);
	time = getnum (de->update_time);
	cp += sprintf (cp, " %02d/%02d/%02d %02d:%02d:%02d ",
	    (date >> 5) & 0x0f,	/* month */
	    (date & 0x1f),	/* day of month	*/
	    ((date >> 9) & 0xff) + 1980,	/* year		*/
	    time >> 11,		/* military hours */
	    (time >> 5) & 0x3f,	/* minutes	*/
	    (time & 0x1f) * 2	/* seconds  / 2	*/
	);

	/* print the file length */
	cp += sprintf (cp, "%9d ", getnum (de->file_size));

	/* print the name and the extention */
	cp = sprintf (cp, "%s", path);
	for (i=0; i<sizeof (de->filename); i++){ 
	    cp += sprintf (cp, "%c", isprint (de->filename [i]) 
			? de->filename [i] : '?');
        }
	cp += sprintf (cp, " ");

	for (i=0; i<sizeof (de->extention); i++){ 
	    cp += sprintf (cp, "%c", isprint (de->extention [i]) 
			? de->extention [i] : '?');
	}
}
#endif


/*+
 * ============================================================================
 * = print_de - Print an msdos directory entry.				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Call sprint_de to get entry info.
 * 
 * FORM OF CALL:
 *  
 *      print_de(fats, path, de)
 *  
 * RETURNS:
 *
 *      NONE
 *       
 * ARGUMENTS:
 *
 *      struct FATS *fats   - Pointer to msdos_bootblock structure
 *	char *path	    - 	
 *	struct direntry *de - Pointer to direntry structure
 *
-*/
#if (FAT_EXPAND && FAT_DEBUG)
void print_de (struct FATS *fats, char *path, struct direntry *de) {
	char line [80];

	if (!(fat_print & FAT_PRINT_DE)) return;
	sprint_de (fats, line, path, de);
	pprintf ("%s\n", line);
}
#endif            


/*+
 * ============================================================================
 * = print_bootblock - Print key fields of boot block.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Printing out bootblock is purely for debugging purpose.
 * 
 * FORM OF CALL:
 *  
 *      print_bootblock (bbp)
 *  
 * RETURNS:
 *
 *      NONE
 *       
 * ARGUMENTS:
 *
 *      struct msdos_bootblock *bbp - Pointer to msdos_bootblock structure
 *      
-*/
#if FAT_DEBUG
void print_bootblk (struct msdos_bootblock *bbp) 
{
	int	i;

	if (!(fat_print & FAT_PRINT_BOOTBLOCK)) return;

	/* print string */
	#define xs(id, msg) pprintf ("%20.20s x ", msg);\
		for(i=0; i<sizeof (id); i++) \
	    		pprintf ("%02X ", id [i] & 0xff);\
		pprintf("\n"); 

	/* print number */	
	#define xn(id, msg) pprintf ("%20.20s d %d\n", msg, getnum (id));

	xs (bbp->jump , "jump ");
	xs (bbp->oem_name , "oem_name ");
	xn (bbp->bytes_per_sector , "bytes_per_sector ");
	xn (bbp->sectors_per_clust , "sectors_per_clust ");
	xn (bbp->reserved_sectors , "reserved_sectors ");
	xn (bbp->num_FATs , "num_FATs ");
	xn (bbp->num_root_dir_entries , "num_root_dir_entries ");
	xn (bbp->total_sectors , "total_sectors ");
	xs (bbp->media_descriptor , "media_descriptor ");
	xn (bbp->sectors_per_FAT , "sectors_per_FAT ");
	xn (bbp->nsect , "nsect ");
	xn (bbp->nheads , "nheads ");
	xn (bbp->hidden , "hidden ");
	xn (bbp->sec32 , "sec32 ");
	xs (bbp->dnum , "dnum ");
	xs (bbp->reserved , "reserved ");
	xs (bbp->signature , "signature ");
	xs (bbp->vol_id , "vol_id ");
	xs (bbp->vol_label , "vol_label ");
	xs (bbp->fs_type , "fs_type ");
	xs (bbp->valid_boot , "valid_boot ");
}
#endif


/*+
 * ============================================================================
 * = print_fat - Print fat info.                       			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	print out the information returned by getfat() every 8 entries per row.
 *
 * FORM OF CALL:
 *  
 *      print_fat(fats)
 *  
 * RETURNS:
 *
 *      NONE
 *       
 * ARGUMENTS:
 *
 *	struct FATS *fats	- Pointer to FATS structure, get fatlen and
 *					fatbits
 *
-*/
#if FAT_DEBUG
void print_fat (struct FATS *fats) 
{
	int	i;

	if (!(fat_print & FAT_PRINT_FAT)) return;

        pprintf("This is a %d-bit FAT.\n", fats->fatbits);
	for (i=0; i<fats->fatlen; i++) {
	    pprintf ("%04X:%04X", i, getfat (fats, i));
	    if ((i&7) == 7) {
		pprintf ("\n");
	    } else {
		pprintf (" ");
	    }
	}
	pprintf ("\n");
}
#endif


/*+
 * ============================================================================
 * = print_mcb - Print Map of Contiguous Blocks.       			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Print MCB for debugging.
 *
 * FORM OF CALL:
 *  
 *      print_mcb(sp)
 *  
 * RETURNS:
 *
 *      NONE
 *       
 * ARGUMENTS:
 *
 *	struct FAT_STREAM *sp - pointer to FAT_STREAM structure
 *
-*/
#if FAT_DEBUG
void print_mcb (struct FAT_STREAM *sp)
{
    	int i;

    	if (!(fat_print & FAT_PRINT_MCB)) return; 

    	for ( i = 0; i < sp->mcb.inuse; i+=2) {
            pprintf("vbo[%d] = %08x\tlbo[%d] = %08x\n",i,sp->mcb.vbo[i],i,
			sp->mcb.lbo[i]);
            pprintf("vbo[%d] = %08x\n", i+1, sp->mcb.vbo[i+1]);
    	}
}
#endif


/*+
 * ============================================================================
 * = bytesfromnum - Convert an integer to an unsigned string.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	this routine builds a byte character string from an integer
 * 
 * FORM OF CALL:
 *  
 *      bytesfromnum (str, val, sizeofval)
 *  
 * RETURNS:
 *
 *      NONE
 *       
 * ARGUMENTS:
 *
 *      unsigned char *str	- the string after conversion
 *	int	val		- integer value
 *	int	size		- sizeof(val)
 *
-*/
void bytesfromnum (unsigned char *str, int val, int size)
{
    int   i;
    for (i = 0; i < size; i++) {
        str[i] = val & 0xff;
        val = val >> 8;
    }
}


/*+
 * ============================================================================
 * = min3 - Return the smallest of 3 values.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	note: min() is defined in common.h 
 * 
 * FORM OF CALL:
 *  
 *      min3 (a, b, c)
 *  
 * RETURNS:
 *
 *      the smallest integer
 *       
 * ARGUMENTS:
 *
 *      int a, b, c 	- any 3 integers
 *
-*/
int min3 (int a, int b, int c) {
	return min (min (a,b), min (b, c));
}




/*+
 * ============================================================================
 * = get_cluster_status - Return cluster usage status.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Each cluster's use status is indicated by its corresponding FAT value.
 *	FORMAT initially marks the FAT value for each cluster as free, having 
 *	value of 0.  When the cluster is used, it will be marked and points to 
 *	the next cluster of file until the last cluster. 
 * 
 * FORM OF CALL:
 *  
 *      get_cluster_status (fats, entry)
 *  
 * RETURNS:
 *
 *      CLUSTER_TYPE       (enum type defined in fat.h)
 *       
 * ARGUMENTS:
 *
 *    	struct FATS *fats	- Pointer to FATS structure, provide fatbit  
 *	int	entry		- fat entry
 *      
-*/
#if 0 	/* currently it is not used, but leave a good picture of clusters */
CLUSTER_TYPE get_cluster_status (struct FATS *fats, int entry)
{
    if ( fats->fatbits == 12 ) {
        if ( entry == 0x000 ) 
            return CLUSTER_AVAILABLE;   /* free cluster */
        if ( entry == 0x001 )
	    return INVALID_FAT_ENTRY;	
        if ( (entry >= 0x002) && (entry <= 0xfef) )
            return CLUSTER_NEXT;        /* link to next cluster */
        if ( (entry >= 0xff0) && (entry <= 0xff6) )
            return CLUSTER_RESERVED;
        if ( entry == 0xff7 ) 
            return CLUSTER_BAD;
        if ( (entry >= 0xff8) && (entry <= 0xfff) )
           return CLUSTER_LAST;       	/* last cluster of file */
    }
    else if (fats->fatbits == 16) {
        if ( entry == 0x0000 ) 
            return CLUSTER_AVAILABLE;   /* free cluster */
        if ( entry == 0x0001 )
	    return INVALID_FAT_ENTRY;	
        if ( (entry >= 0x0002) && (entry <= 0xffef) )
            return CLUSTER_NEXT;        /* link to next cluster */
        if ( (entry >= 0xfff0) && (entry <= 0xfff6) )
            return CLUSTER_RESERVED;
        if ( entry == 0xfff7 ) 
            return CLUSTER_BAD;
        if ( (entry >= 0xfff8) && (entry <= 0xffff) )
           return CLUSTER_LAST;         /* last cluster of file */
    }
}
#endif


/* 
 * The remaining functions in this file are required only for ARC support.
 */
#if ARC_SUPPORT
/*+
 * ============================================================================
 * = fat_getfile_info - Get information about a FAT file. 	              =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This routine fills in an ARC file information structure for the
 *	specified file.
-*/
ARC_STATUS fat_getfile_info ( struct FILE *fp, PFILE_INFORMATION Buffer )
{
    struct FATS	*fats;

    /* Clear out the buffer */
    memset ( Buffer, 0, sizeof(FILEINFORMATION) );
    
    fats = fp->misc;
    if ( fats == NULL )
        return EACCES;

    Buffer->EndingAddress.LowPart = fats->rws.filelen;

    Buffer->CurrentPosition.LowPart = *fp->offset;
    Buffer->CurrentPosition.HighPart = 0;

    if ( fats->rws.de.attribute & FATTR_READ ) 
        Buffer->Attributes |= ArcReadOnlyFile;
    if ( fats->rws.de.attribute & FATTR_HIDDEN )
        Buffer->Attributes |= ArcHiddenFile;
    if ( fats->rws.de.attribute & FATTR_SYSTEM )
        Buffer->Attributes |= ArcSystemFile;
    if ( fats->rws.de.attribute & FATTR_ARCHIVE )
        Buffer->Attributes |= ArcArchiveFile;
    if ( fats->rws.de.attribute & FATTR_DIRECTORY )
        Buffer->Attributes |= ArcDirectoryFile;

    Buffer->FileNameLength = strlen ( fats->curpath );

    strcpy ( Buffer->FileName, fats->curpath );

    return ESUCCESS;
}


/*+
 * ============================================================================
 * = fat_getdir_entry - 
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This routine reads directory entries from the specified disk.
 *
-*/
ARC_STATUS fat_getdir_entry (struct FILE *fp, DIRECTORY_ENTRY *Buf, 
			ULONG NumDir, PULONG Count )
{
    struct direntry de;
    struct FATS *fats;
    struct FAT_STREAM *sp;
    int eofdir = 0;
    int x;

    fats = fp->misc;
    sp = &fats->rws;

    /* If this isn't a directory, return failure */
    if ( !fats->rws.de.attribute & FATTR_DIRECTORY )
        return ENOTDIR;

    /* If there are no entries to read, return */
    if ( NumDir == 0 )
        return ESUCCESS;

    *Count = 0;			/* Initialize count of entries read */

    /* Now attempt to read the requested number of entries */

    do {
        x = fat_stream_read (fats, sp, sizeof (struct direntry), &de);
    
        if ( x != sizeof(struct direntry) )
            return EBADF;

        /* Exit if we're at the logical end of the directory */
        if ( de.filename[0] == (char) FAT_DIRENT_NEVER_USED ) {
            eofdir = TRUE;
            break;
        }

        /* Skip this entry if it has been deleted */
        if ( de.filename[0] == (char) FAT_DIRENT_DELETED )
            continue;

        /* Skip if this entry is a volume lable */
        if ( de.attribute & FATTR_VOLUME )
            continue;

        /* Copy the entries information to the users buffer */
        copy_fatdir_to_arcdir (Buf++, &de );

        if ( ++*Count >= NumDir )
            break;

    } while ( !eofdir && (*Count < NumDir) );

    return *Count ? ESUCCESS : ENOTDIR;
}


/*+
 * ============================================================================
 * = copy_fatdir_to_arcdir -  	              				      =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	
-*/
void copy_fatdir_to_arcdir (DIRECTORY_ENTRY *ArcDe, struct direntry *de )
{
    int i, j;
    
    /* Clear out the users buffer */
    memset ( ArcDe, 0, sizeof (DIRECTORY_ENTRY) );

    /* Set the attributes */   
    if ( de->attribute & FATTR_READ ) 
        ArcDe->FileAttribute |= ArcReadOnlyFile;
    if ( de->attribute & FATTR_HIDDEN )
        ArcDe->FileAttribute |= ArcHiddenFile;
    if ( de->attribute & FATTR_SYSTEM )
        ArcDe->FileAttribute |= ArcSystemFile;
    if ( de->attribute & FATTR_ARCHIVE )
        ArcDe->FileAttribute |= ArcArchiveFile;
    if ( de->attribute & FATTR_DIRECTORY )
        ArcDe->FileAttribute |= ArcDirectoryFile;
 
    /* Copy the 1st part of the name */
    for (i = 0;  (i < 8) && (de->filename[i] != ' ');  i += 1) 
        ArcDe->FileName[i] = de->filename[i];

    /* Check for an extenstion */
    if ( de->filename[8] != ' ' ) {

        /* store the dot char */
        ArcDe->FileName[i++] = '.';

        /* add the extension */
        for (j = 8;  (j < 11) && (de->filename[j] != ' ');  j ++ ) 
            ArcDe->FileName[i++] = de->filename[j];

    }

    ArcDe->FileNameLength = i;

    return;
}
#endif
