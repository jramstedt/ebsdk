/* file:	examine_driver.c
 *
 * Copyright (C) 1991 by
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
 *
 * Abstract:	Examine protocol driver.
 *
 * Authors:	JDS - Jim Sawin
 *
 * Modifications:
 *
 *	jds	23-Aug-1991	Fix error in dyn$_free() call - missing options argument.
 *
 *	jds	2-Aug-1991	Initial entry.
 *
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:kernel_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:exdep_def.h"
#include "cp$src:alpha_def.h"
#include "cp$inc:kernel_entry.h"

/* Define flags for special rendering variations: */

#define EXR_DEFAULT	0
#define EXR_ALPHA_PC	1
#define EXR_PSR		2
#define EXR_VIRTUAL	3

/* External references: */

extern null_procedure(void);

/* Define structure for storing information used by driver: */

struct examine_info  {
    struct FILE *fp;		/* File pointer of 'next' driver */
    int size;			/* Size of items to read from next driver */
    int number;			/* Number of items to read from next driver */
};

/* Declare routines: */

examine_init(void);
examine_open(struct FILE *fp, char *info, char *next, int mode);
examine_read(struct FILE *fp, int size, int number, char *buf);
examine_close(struct FILE *fp);

struct DDB examine_ddb = {
	"render",			/* how this routine wants to be called	*/
	examine_read,		/* read routine				*/
	null_procedure,		/* write routine			*/
	examine_open,		/* open routine				*/
	examine_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	1,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};



/*+
 * ============================================================================
 * = examine_init - Initialize examine protocol drivers			      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Initializes examine  protocol  driver in the DDB, and creates INODEs in
 * 	the  file  system  for  each flavor.  The protocol drivers share driver
 * 	routines, but are distinguished by the INODE misc field.
 *
 * FORM OF CALL:
 *  
 *	examine_init();
 *  
 * RETURN CODES:
 *
 *	msg_success
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
int examine_init(void) {
    struct INODE *ip;	/* INODE pointer */


    /* Create INODE for vanilla format: */
    allocinode("examine_render", 1, &ip);
    ip->dva = & examine_ddb;
    ip->attr = ATTR$M_READ;
    ip->misc = EXR_DEFAULT;
    INODE_UNLOCK (ip);

    /* Create INODE for ALPHA PC/VAX PSL format: */

    allocinode("psr_render", 1, &ip);
    ip->dva = & examine_ddb;
    ip->attr = ATTR$M_READ;
    ip->misc = EXR_ALPHA_PC;
    INODE_UNLOCK (ip);

    /* Create INODE for virtual memory format: */

    allocinode("vmem_render", 1, &ip);
    ip->dva = & examine_ddb;
    ip->attr = ATTR$M_READ;
    ip->misc = EXR_VIRTUAL;
    INODE_UNLOCK (ip);

    return msg_success;
}

/*+
 * ============================================================================
 * = examine_open - Open routine for examine protocol drivers		      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This routine  opens  the  examine  protocol  driver.  The driver must be
 * 	passed   up   to  2  arguments  in  the  info  field  of  the  protocol
 * 	specification,  which  represent  the size and number of the
 * 	item(s) to be accessed by the next driver in the chain.
 *
 *	For example:
 *~
 *	    examine_render:"2 1"/pmem:100
 *~
 * 	causes examine_render  to inherit the initial file offset to 100, use a
 * 	size  of  2  (bytes/item) and number of 1 (item) when it accesses pmem.
 * 	If  an argument is not specified, a default value is used.  The default
 * 	values are: "4 1" for VAX and "8 1" for Alpha.
 *
 * 	A structure is allocated for storing the size, number, and file pointer
 * 	of the next driver.  The structure is referenced off the fp->misc field
 * 	of this driver.
 *
 * FORM OF CALL:
 *  
 *	examine_open(fp, info, next, mode);
 *  
 * RETURN CODES:
 *
 *	msg_success   -	If successful.
 *
 * ARGUMENTS:
 *
 *      struct FILE *fp	    - File pointer.
 *	char *info	    - Pointer to info portion of filespec for this
 *			      driver.
 *	char *next	    - Pointer to next portion of filespec.
 *	int mode	    - Access mode.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int examine_open(struct FILE *fp, char *info, char *next, int mode) {
    struct examine_info *exinfo;
    char	element [32];

    fp->misc = 
    exinfo = 
    (void *) dyn$_malloc(sizeof(struct examine_info), DYN$K_SYNC);

    /* Open next file in protocol chain */
    exinfo->fp = (struct FILE *) fopen(next, mode);
    if (exinfo->fp == NULL) {
	return msg_failure;
    }

    /* Fill in default values for size, number: */

    exinfo->size = sizeof(QUADWORD);
    exinfo->number = 1;

    /*
     * If info is provided, it is of the form: <size> <number>.
     * Parse out the given fields and save them away.
     */
    if (strelement (info, 0, "\" ", element)) {
	exinfo->size = xtoi (element);
    }
    if (strelement (info, 1, "\" ", element)) {
	exinfo->number = xtoi (element);
    }

    /* Initialize file offset */
    init_file_offset(fp);

    /* inherit the next driver's position */
    memcpy (fp->offset, exinfo->fp->offset, sizeof (fp->local_offset));

    return msg_success;
}

/*+
 * ============================================================================
 * = examine_read - examine protocol driver read routine		      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Reads an  item  from the next driver in the chain, and returns a string
 * 	which  contains  the  item  in  the  appropriate  format for an examine
 * 	command  response.   This  read routine supports 3 variations of output
 * 	formats,  allowing it to serve as a common read routine for 3 different
 * 	protocol  drivers.   The  routine  determines which variation to use by
 * 	looking  at  the  driver's INODE misc field, which was initialized when
 * 	the INODE was created in examine_init().
 *
 *	As the examine_render driver, the output format produced is:
 *~
 *	    driver: hex-location (symbolic-name) hex-data
 *~
 * 	driver is  the  name of the next driver in the chain (the source of the
 * 	data),  hex-location  is  hexadecimal  location  or address referenced,
 * 	symbolic-name  is an optional field which is printed if a symbolic name
 * 	is known for the item, and hex-data is the hexadecimal data read at the
 * 	selected location.
 *
 * 	As the  vmem_render  driver,  the  output  format  is the same, but the
 * 	hex-location represents the translated physical address rather than the
 * 	virtual  address.  The physical address is passed back via the fp->misc
 * 	field of the vmem_driver.
 *
 * 	As the  psr_render  driver,  the  output  format  shows  a  VAX  PSL in
 * 	hexadecimal longword format, and displays the value of each field:
 *~
 *			CM TP FPD IS CURMOD PRVMOD IPL DV FU IV T N Z V C
 *	PSL 00000000     0  0  0   0 KERNEL KERNEL  00  0  0  0 0 0 0 0 0
 *~
 *	The protocol driver fseeks to the same location in the next driver as
 * its own file offset.  It then freads an item, using the size and number
 * values stored by the examine_open routine.  Then the data is formatted and
 * written to the caller's buffer as a string.
 *
 * FORM OF CALL:
 *  
 *	examine_read(fp, size, number, buf);
 *  
 * RETURN CODES:
 *
 *	x - number of byte written into buffer
 *	0 - all errors, EOF etc.
 *
 * ARGUMENTS:
 *
 *      struct FILE *fp	    - File pointer.
 *	int size	    - Size of item to read into buffer (item = char = byte)
 *	int number	    - Number of items to read into buffer (number =
 *			      number of chars in buffer)
 *	char *buf	    - Pointer to buffer.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int examine_read(struct FILE *fp, int size, int number, char *buf) {
    INT offset_before;
    INT offset_after;
    BIGCHUNK inbuf;
    int status;
    int ex_byte;
    static char *modes[4] =
	{"KERNEL", " EXEC ", " SUPER", " USER "};		/* VAX/ALPHA processor modes */
    char *symbol;
    struct examine_info *exinfo;
    char *bp;

    bp = buf;
    exinfo = (struct examine_info *) fp->misc;

    /*
     * Get the current offset of our driver and pass it down the chain.
     * Then read the data from the next driver into an internal buffer:
     */
    memcpy (exinfo->fp->offset, fp->offset, sizeof (fp->local_offset));
    memcpy (&offset_before, exinfo->fp->offset, sizeof (offset_before));

    status = fread(&inbuf, exinfo->size, exinfo->number, exinfo->fp);
    fp->status = exinfo->fp->status;
    if (status == 0)
	return 0;

    /*
     * remember where the driver below us left off
     */
    memcpy (fp->offset, exinfo->fp->offset, sizeof (fp->local_offset));
    memcpy (&offset_after, exinfo->fp->offset, sizeof (offset_after));

    /*
     * On Alpha platforms, check to see if this is the PC.  If so, print
     * a symbol in the location field
     */
    if (fp->ip->misc == EXR_ALPHA_PC) {
	bp += sprintf(bp, "%s ", "PC");
    }

    /* If this is the virtual_render formatter, get the translated physical
     * address from the misc field of the next driver's file pointer:
     */
    if (fp->ip->misc == EXR_VIRTUAL) {
/*	offset_before = exinfo->fp->misc;*/
        virtual_to_physical(offset_before,&offset_before);
    }

    /* Write driver name and location into buffer: */
    bp += sprintf(bp, "%s: %16X ", exinfo->fp->ip->name, offset_before);

    /*
     * Look for a symbol associated with this location.  Include it in
     * the output if found.
     */
#if MODULAR
    ovly_load("EXAMINE");
#endif
    symbol = get_exdep_symbol (offset_before, exinfo->fp->ip->name);
#if MODULAR
    ovly_remove("EXAMINE");
#endif
    if (symbol != NULL)
	bp += sprintf(bp, "(%6s) ", symbol);

    /* Print the data to the user's buffer: */
    ex_byte = exinfo->size * exinfo->number;
    while (--ex_byte >= 0) {
    	bp += sprintf(bp, "%02X", inbuf[ex_byte]);
	if ((ex_byte % sizeof(QUADWORD)) == 0)
	    bp += sprintf(bp, " ");
    }

    /* Terminate the line: */
    bp += sprintf(bp, "\n");

    return bp - buf; /* characters read */
}

/*+
 * ============================================================================
 * = examine_close - examine protocol driver close routine		      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Closes the  next  file  in  the  chain,  frees the examine_info storage
 * 	structure, and returns.
 *
 * FORM OF CALL:
 *  
 *	examine_close(fp);
 *  
 * RETURN CODES:
 *
 *	msg_success   -	If successful.
 *
 * ARGUMENTS:
 *
 *      struct FILE *fp	    - File pointer.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int examine_close(struct FILE *fp) {
    struct examine_info *exinfo;

    exinfo = (struct examine_info *) fp->misc;
    fclose(exinfo->fp);
    dyn$_free(exinfo, DYN$K_SYNC);
    return msg_success;
}
