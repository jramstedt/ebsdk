/* file:	update.c
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
 */

/*
 *++
 *  FACILITY:
 *
 *      Cobra Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      A shell command to be used to update feroms on the io module.
 *
 *  AUTHORS:
 *
 *      Judy Gold
 *
 *  CREATION DATE:
 *  
 *      08-Nov-1991
 *
 *  MODIFICATION HISTORY:
 *
 *     	ska	11-Nov-1998	Conditionalize for YukonA
 *
 *	pmd	23-Feb-1996	Conditionalize for MTU
 *
 *     	jje	22-Feb-1996	Conditionalize for Cortex
 *
 *	rbb	28-Aug-1995	Conditionalize for EB164
 *
 *	jje	28-Apr-1995	Conditionalize for Medulla
 *
 *	er	26-Apr-1995	Conditionalize for EB66+
 *
 *	bobf	21-Oct-1994	Medulla Changes Only: modified the "target"
 *				list and source name to simplify CD updates.
 *
 *	er	 7-Oct-1994	Conditionalize for Cabriolet.
 *
 *	bobf	 8-Sep-1994	Medulla Changes Only: added messages about
 *				DIP switch #2; check return status of fwrite
 *				that programs the flashes, skip verification
 *				if write was not successful.
 *
 *      wcc      3-Dec-1993     ECO 63.2: Enhanced the common console LFU 
 *				format to handle older revs by returning a
 *				value of 2 on older rev fw image being updated 
 *				with newer fw on hardware.
 *
 *      wcc     22-Nov-1993     updated code for calling updated CCLFU
 *                              cobraflash_driver.
 *
 *      cbf     27-Jan-1993     add arg to read_with_prompt for echo functn.
 *
 *	ajb	15-Jun-1992	Separated user interface from driver.
 *
 *      jeg     08-Nov-1991	Initial entry.
 *
 *	jeg	12-Nov-1991	changed to accommodate new mopdl driver
 *	
 *	jeg	 2-Jan-1992	ROM image will actually start at booted file + 1 page.  Modify program().
 *
 *	jeg	 8-Jan-1992	compute checksum and compare with chksm stored in rom, compare signatures.
 *
 *	jeg	15-Jan-1992	open "tta0" and use fp as argument to read_with_prompt, 
 *				add ignore signature switch to qscan array.
 *
 *	kp	30-Jan-1992	Modified checksum algorithm used based on COBRA_FEPROM_CHECKSUM.C.
 *				Also tweeked output to reduce to 24 lines when successful.
 *
 *	jeg	28-May-1992	initial update command entry (modified from
 *				blast).
 *--
 */

#include        "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:version.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:impure_def.h"

/*
 * The update command is capable of updating several targets.
 * When the user specifies a target, we have to figure out which
 * device in the console that target corresponds to.  Once we've
 * found the device, we then know via the inode length field how
 * many bytes are in the flash device.
 */
struct TARGETS {
	char *name;		/* the target name	*/
	char *filename;		/* file name to open	*/
} targets [] = {
#if MEDULLA || CORTEX || UNIVERSE
	{"stdio", "console"},
#endif
	{"io",	"io_flash"},
	{0, 	0}
};

extern struct ZONE *memzone;

#if MTU
#define QFILE		0
#define	QTARGET		1
#define QPROTOCOL	2
#define QDEVICE		3
#define QIGNORE		4
#define QPATH		5
#define QNOCONFIRM	6
#define	QMAX	        7
#define QSTRING "%sfile %starget %sprotocol %sdevice ignore %spath noconfirm"
#else
#define QFILE		0
#define	QTARGET		1
#define QPROTOCOL	2
#define QDEVICE		3
#define QIGNORE		4
#define QPATH		5
#define	QMAX	        6
#define QSTRING "%sfile %starget %sprotocol %sdevice ignore %spath"
#endif

/*+
 * ============================================================================
 * = update - Update flash roms on the system                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Update FEPROMS with new firmware.
 *  
 * FEPROMs may be updated using the 'update' shell command.  Normally, no
 * arguments or qualifiers are needed if just the console roms are being
 * updated.
 *
 * Update kits normally contain the required images for updating.  The update
 * program looks for these images (in the ram disk).  If the images are not
 * present, or some other image is to be used, then one of the following 
 * command constructs must be used:
 *
 *	1) specify a device, protocol and filename, eg:
 *		update -file cfw_e42 -protocol mopdl -device eza0
 *
 *	2) specify the fully qualified pathname, eg:
 *		update -path mopdl:cfw_e42.sys/eza0
 *
 * 
 *
 * The process works as follows:
 *	1) The image is loaded into memory 
 *	2) consistency checks are applied to the image.  Specifically,
 *	   checksums must match, signatures must match, sizes must match etc.
 *	   Consistency checks also insure that a VAX program doesn't get burned
 *	   on an ALPHA.  The program complains if any of the consistency
 *	   checks fail.  The program will then exit unless the -ignore
 *	   qualifier is present.
 *	3) The program asks the user to confirm that an update is desired.
 *	4) The program actually burns the FEPROMS.
 *
 *
 * If all of these requirements are satisfied, the user is given a console 
 * prompt of 'Do you really want to continue [Y/N]?'  If 'n' (or 'N') is typed,
 * the program will be aborted with a return to the console prompt.  If 'y' 
 * (or 'Y') is typed, the blast will commence.  At this point it is imperative 
 * that the program is NOT broken, halted, or interrupted for any reason.  If 
 * interrupted, the module will most likely be left in an inoperable state and 
 * the FEPROMs will need to be replaced.
 *
 * There are three steps to the blasting process:
 *##
 * 1 All longwords are programmed to '00000000'. This is accomplished via 
 * function uniform_program() internal to the update command.
 *
 * 2 All longwords are erased.  The erased state is 'ffffffff'.  This is
 * accomplished via function erase() internal to the update command. 
 *
 * 3 All longwords are reprogrammed to the values in the image booted into
 * memory.  This is accomplished via the function program() internal to the 
 * update command.
 *#
 *
 * Progress messages to the user are printed out for each of the three steps. 
 *
 * Each longword of the FEPROM is verified in each of the three steps.  Each 
 * step provides for a certain number of chances to perform the operation 
 * successfully on a particular longword of the FEPROM.  These algorithms are 
 * taken directly from the 28f010 spec.  If a failure occurs in any of the 
 * steps, an error message is printed to the console. 
 *
 * If the blasting operation is successful, a success message is printed to 
 * the console.   
 *
 * Note that it is necessary to reset or cycle power on the system to unload
 * the new image of the FEPROMs into memory.  Until a power down, the old
 * image is still being executed.
 *
 * COMMAND FMT: update 2 Z 0 update
 *
 * COMMAND FORM:
 *
 *	update ( [-file <filename>] 
 *               [-protocol <transport>] [-device <source_device>]
 *               [-target <target_name>] 
 *		 [-path <full_pathname>] )
 *
 * COMMAND TAG: update 0 RXBZ update
 *
 * COMMAND ARGUMENT(S):
 *
 *	None
 *
 * COMMAND OPTION(S):
 *
 *	-file <filename> - Specifies the name of the new FEPROM update image.
 *		The default is the name of the platform that the update is
 *		running on.  For example, if the update is running on a COBRA
 *	        using a pass 4 chip, the default name is COBRA_EV4P2.
 *
 *	-protocol <transport> - Specifies the source transport protocol.
 *		The default is MOPDL.
 *
 *	-device - Specifies the device from which to load the
 *		new FEPROM update image file.  The default is EZA0.
 *
 *	-target <device> - Specifies the device which contains the FEPROMs to
 *		be upgraded.  The default is IO (the FEPROMs on I/O module).
 *		If the target is not recognized, then it is assumed to be the
 *		name of device driver this is of type flash rom.
 *
 *      -path <full_pathname> - This qualifier is used if the file, protocol
 *		and device qualifiers are not sufficient to fully specify
 *		the source filename.  This qualifier over rides the
 *		file, protocol and device qualifiers.
 *
 *	-ignore	- Ignore the signature and consistency checks. If specified,
 *		the burn occurs regardless of the built in consistency checks.
 *		This qualifier should be used with caution!
 *
 * COMMAND EXAMPLE(S):
 *~
 *      >>>update -file cobra_feb01_e42
 *
 *                              FEPROM UPDATE UTILITY
 *                              -----> CAUTION <-----
 *              EXECUTING THIS PROGRAM WILL CHANGE YOUR CURRENT ROM!
 *      
 *      Do you really want to continue [Y/N] ? : y
 *      
 *              DO NOT ATTEMPT TO INTERRUPT PROGRAM EXECUTION!
 *              DOING SO MAY RESULT IN LOSS OF OPERABLE STATE.
 *      
 *      The program will take at most several minutes.
 *      
 *      Starting uniform programming to 0's of block...
 *              00000  10000  20000  30000  40000  50000  60000  70000
 *      Starting erase of block...
 *              00000  10000  20000  30000  40000  50000  60000  70000
 *      Starting reprogramming of block...
 *              00000  10000  20000  30000  40000  50000  60000  70000
 *      
 *      update successful!
 *      >>> 
 *~
 * FORM OF CALL:
 *  
 *	update ( int argc, char *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - if the blast process was successful
 *	msg_fe_spec_file - if not enough parameters were supplied
 *  	msg_failure - if bootable image file cannot be read, its size is too
 *		      large or small, signature or checksum match fails, or
 *		      reprogramming the FEPROMs is unsuccessful   	 
 *       
 * ARGUMENTS:
 *
 * 	int argc 	- Number of arguments on the command line.
 *	char *argv[] 	- Array of pointers to argument strings.
 *
 * SIDE EFFECTS:
 *
 *	FEPROMs have changed.  If the unit is powered down, the new contents of
 *	the FEPROMs will be loaded into memory.
 *
-*/
update (int argc, char *argv[]) {
	struct QSTRUCT qual [QMAX];   
	struct FILE *srcp;	/* where the bits came from */
	struct FILE *dstp;	/* where the bits are going */
	struct FILE *burntp;	/* what actually got there */
	int status;
	int fwrite_status;	/* status returned from fwrite call */
	int i, j;
	char srcname [256];	/* path to read in to get image */
	char dstname [32];	/* filename that is updated */

	unsigned char *new;	/* what we will be burning */
	unsigned int newlen;
	unsigned int badbits;
	unsigned int cc;
	int c;
	unsigned int oldattr;
	int (*cb_revision)();
	char cb_hfw[15];
	char cb_jk[132];
	char cb_fw[15];

	/*
	 * Decode the qualifiers
	 */
	status = qscan( &argc, argv, "-", QSTRING, qual);
	if (status != msg_success) {
            err_printf (status);
	    err_printf (msg_noupdates);
	    return status;
        }

	/*
	 * We don't allow command line arguments
	 */
	if (argc != 1) {
	   err_printf ("%s: %s\n", argv [0], msg_extra_params);
	   return msg_failure;
	}

	
	/*
	 * Form the fully qualified source and destination names
	 */
	if (update_mksrcname (qual, srcname) || update_mkdstname (qual, dstname)) {
	    err_printf (msg_noupdates);
	    return msg_failure;
	}

#if 0 || 1
	pprintf ("(%s -path %s -target %s)\n", argv [0], srcname, dstname);
#endif
	/*
	 * Open up the target device so we can find out how
	 * big it is.  Change the attributes to write access.
	 */
	oldattr = setattr (dstname, ATTR$M_READ | ATTR$M_WRITE);
	dstp = fopen (dstname, "r+");
	/* verify that we have a valid flash device driver */
	if (dstp == NULL)
	    return msg_failure;
	if (!dstp->ip->dva->flash) /* is it a flash driver */
	{
	    printf("Device %s is not a flash driver\n",dstname);
	    return msg_failure;
	}

	new = dyn$_malloc (dstp->ip->len[0], DYN$K_SYNC | DYN$K_NOFLOOD | DYN$K_NOWAIT | DYN$K_ZONE, 0, 0, memzone);
	if (new == 0) {
	    err_printf (msg_insuff_mem, dstp->ip->len[0]);
	    err_printf (msg_noupdates);
	    return msg_failure;
	}

	/* Read in the new image into a private buffer */
	srcp = fopen (srcname, "r");
	if (srcp == NULL) {
	    err_printf (msg_noupdates);
	    return msg_failure;
	} else {
	    int bytesleft = dstp->ip->len[0];
	    int len;
	    newlen = 0;
	    while (len = fread (new + newlen, 1, bytesleft, srcp)) {
		newlen += len;
		bytesleft -= len;
	    }
	    fclose (srcp);
	}
	    

	/*
	 * Validate the image.  We let the driver do this, since it's in the
	 * best position to know what constitutes a valid image.  This
	 * presumes that all flash drivers export a validate routine.
	 * Flash drivers also print out the version numbers of the old
	 * and new images.
	 */
	if ((*dstp->ip->dva->validation) (new, newlen, dstp)) {
	    if (qual [QIGNORE].present) {
		err_printf (msg_conscheck);
	    } else {
		err_printf (msg_noupdates);
		return msg_failure;
	    }
	}

#if MEDULLA || CORTEX || UNIVERSE
	/*
	 * Notify user to check position of DIP Switch #2
	 *	DIP Switch #2 Open   = Updates are disabled
	 *	DIP Switch #2 Closed = Updates are enabled
	 */
	printf ("\n\n\tNote: Module DIP Switch #2 must be CLOSED to enable Updates!\n\n\n");
#endif

	/* wait for the user to confirm a go ahead */
#if MTU
	if (qual [QNOCONFIRM].present) {
	    err_printf ("\0");
    	} else {
	    if (!update_confirmed (msg_fe_pgm)) {
	    	err_printf (msg_noupdates);
	    	return msg_failure;
	    }
    	}
#else
	if (!update_confirmed (msg_fe_pgm)) {
	    err_printf (msg_noupdates);
	    return msg_failure;
	}
#endif

	/* warn the user to not touch the beast while burning */
	printf (msg_fe_no_int);
	printf (msg_fe_lose_opstate);
	printf (msg_fe_sev_min);

	/* burn it (flash drivers burn on the close) */
	fwrite_status = fwrite (new, 1, newlen, dstp);
	if (fclose (dstp) != msg_success) {
	    err_printf ("Image not burned.  Don't reset and try again.\n");
#if MTU
	    err_printf (dstp->status);
#endif
	}

	/* reset the attributes to previous values */
	setattr (dstname, oldattr);

#if MEDULLA || EB164 || CORTEX || UNIVERSE
	/*
	 * Only verify reprogrammed flash devices if the write was successful
	 */
	if (fwrite_status)
	{
#endif

  	/* Always read the device back in and verify */
	err_printf ("Verifying...\n");
	badbits = 0;
	burntp = fopen (dstname, "r");
	status = msg_goodupdate;
	if (burntp) {

	    /* read it in and compare */
	    badbits = 0;
	    cc = 0;
	    j = 0;
#if MEDULLA || EB164 || CORTEX || UNIVERSE
	    /*
	     * Only verify the segments of the flash device that were reprogrammed.
	     */
	    while ((cc < newlen) && ((c = fgetc (burntp)) != EOF)) {
#else
	    while ((c = fgetc (burntp)) != EOF) {
#endif
		if (c != new [cc]) {
		    if (j < 10)
		    	err_printf ("not equal at %08X %02X %02X\n", cc, new [cc], c & 0xff);
		    if (j == 10)
			err_printf("more than 10 errors occurred, errors no longer printed\n");
		    badbits += count_bits (new [cc] ^ c);
		    j++;
		}
		cc++;
	    }
	    fclose (burntp);

	    if (badbits) {
		err_printf ("A total of %d bits did not verify\n", badbits);
		status = msg_badupdate;
	    }

	} else {
	    err_printf ("Can't open device for verification\n");
	    status = msg_badupdate;
	}

#if MEDULLA || CORTEX || UNIVERSE
	}
	else
	{
	 err_printf ("Flash programming FAILED -- Check DIP Switch #2\n");
	 status = msg_badupdate;
	}
#endif

#if EB164
	}
	else
	{
	 err_printf ("Flash programming FAILED\n");
	 status = msg_badupdate;
	}
#endif
  
	err_printf (status);

#if MEDULLA || CORTEX || UNIVERSE
	/*
	 * Notify user to disable further UPDATES by OPENING DIP Switch #2
	 */
	printf ("\n\n\tNote: Module DIP Switch #2 should be OPENED to disable Updates!\n\n\n");
#endif

	if (status == msg_goodupdate) return msg_success;
	return msg_failure;
}

/*
 * print out a message and force the user to confirm with a yes or
 * no response.
 */
update_confirmed (char *message) {
	char answer [32];
	
	do {
	    printf (message);
	    read_with_prompt (msg_fe_areyousure, sizeof (answer), answer, 0, 0, 1);
	    answer [0] = tolower (answer [0]);
	    if ((answer [0] == 'y') || (answer [0] == 'n')) break;
	    if (killpending ()) {
		answer [0] = 'n';
		break;
	    }
	    printf (msg_pleaseyesno);
	} while (1);
	
	if (answer [0] == 'y') return 1;
	return 0;
}


/*
 * return what we call ourseleves for udpate purposes
 */
char *whats_my_name () {
	static char name [32];
	int type;


#if MEDULLA
	strcpy (name, "medullarom");
#else
	strcpy (name, "noname");
#endif

	return name;
}


/*
 * Build up a fully qualified name of the source of the bits to be
 * updated.
 */
update_mksrcname (struct QSTRUCT *qual, char *name) {
	char *protocol;
	char *file;
	char *device;
	char *myname;
	struct FILE *fin;

	myname = whats_my_name ();

	/*
	 * If none of the qualifiers path, protocol, file or device
	 * are present, see if there is a file in the ram
	 * disk that has the right name.
	 */
	if ((qual [QPATH].present + qual [QPROTOCOL].present + 
	    qual [QDEVICE].present + qual [QFILE].present) == 0) {
	    if (fin = fopen (myname, "r")) {
		fclose (fin);
		strcpy (name, myname);
		return 0;
	    }
	}

	/* explicit pathname overrides protocol, filename and device */
	if (qual [QPATH].present) {
	    strcpy (name, qual [QPATH].value.string);
	    return 0;
	}

	if (qual [QPROTOCOL].present) {
	    protocol = qual [QPROTOCOL].value.string;
	} else {
	    protocol = "mopdl";
	}

	if (qual [QFILE].present) {
	    file = qual [QFILE].value.string;
	} else {
	    file = myname;
	}

	if (qual [QDEVICE].present) {
	    device = qual [QDEVICE].value.string;
	} else {
	    device = "eza0";
	}

	sprintf (name, "%s:%s/%s", protocol, file, device);
	return 0;
}


/*
 * Build up a fully qualified name of the destination, i.e. the item we're
 * updating.
 */
update_mkdstname (struct QSTRUCT *qual, char *name) {
	struct TARGETS *tp;

	/*
	 * If no target was specified, then we default to the first target on
	 * the target list, which should be ourselves.
	 */
	if (qual [QTARGET].present) {
	    strcpy (name, qual [QTARGET].value.string);
	} else {
	    strcpy (name, targets [0].name);
	}

	/*
	 * Find the target in the list of legal targets
	 */
	for (tp=targets; tp->name; tp++) {
	    if (strcmp (tp->name, name) == 0) {
		strcpy (name, tp->filename);
		return 0;
	    }
	}

	/*
	 * Not found, use what was passed anyway 
	 */
	return 0;
}

