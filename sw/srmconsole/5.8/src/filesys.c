/* file:	filesys.c
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
 *      Cobra Firmware
 *                                            
 *  MODULE DESCRIPTION:
 *      File system for ALPHA/Cobra.  At this level of abstraction, all
 *	devices and files appear as files.  Access is via routines that
 *	have the same interface as FOPEN, FCLOSE etc from the ANSII C
 *	standard.  All machine specific code is in filesys_<machine>.c
 *
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 * 
 *      08-Oct-1991
 *
 *  MODIFICATION HISTORY:
 *
 *      jwj     17-Mar-1999     Add hack for UNIX callback with IDE slave devices
 *                              Unix will send 100 as the unit number for DQA1/DQB1
 *                              because the UNIX IDE driver is tied to the SCSI driver.
 *                              During the conversion from device name to file name if the
 *                              device is 'IDE' and the unit number is '100' we will
 *                              convert it back to 1.
 *
 *	tna     11-Nov-1998     Conditionalized for YUKONA
 *
 *      wcc     29-Nov-1993     added in a free to the inode ponter clone in
 *				fexpand.
 *
 *	jmp	29-Nov-1993	Add avanti support
 *
 *      cbf     03-mar-1993     modify for removal of variable noarg
 *
 *	pel	25-Jan-1993	temporarily add MORGAN qprintfs in ddb_startup.
 *
 *	ajb	10-Sep-1992	remove unecessary ilist_alloc
 *
 *	jad	07-Jul-1992	add ddb_startup_driver routine
 *
 *	ajb	15-Jun-1992	add setattr routine
 *
 *      cjd     27-May-1992     validate_filename: permit ':' and '/' (quoted
 *                              as '//') in info field.
 *
 *	phk	07-May-1992	Add ip->loc to -l info 
 *
 *	ajb	30-Apr-1992	Let inode list grow dynamically, and keep
 *				it sorted.
 *
 *	pel	14-Apr-1992	show_iobq; malloc from sys heap w/ wait
 *
 *	pel	08-Apr-1992	show_iobq; malloc from memzone to save heap.
 *
 *	ajb	12-feb-1992	Break out check command
 *
 *	pel	05-Feb-1992	fread,fwrite: use qwd math not qaddu routine
 *
 *	pel	27-jan-1992	fix show_iobq by padding clone struct
 *
 *	ajb	23-jan-1992	Make file offsets 64 bits.  Drivers that only
 *				need 32 bits can still work unchanged under
 *				the 32 bit compiler.
 *
 *	pel	22-Jan-1992	expand IOB.bytes_read,written,io_count to qwd
 *
 *	sfs	22-Jan-1992	Expand driver_setmode to include miscellaneous
 *				port drivers.
 *
 *	pel	03-Jan-1992	init sys_iob.
 *
 *	ajb	10-Dec-1991	Add driver_setmode 
 *
 *	ajb	08-Oct-1991	Break out generic files system stuff from
 *				machine specific stuff.
 *
 *	jad	01-Oct-1991	Add some debug
 *
 *	pel	30-Sep-1991	replace io_init w/ lbus_init, diag_lbus
 *
 *	phk	20-Sep-1991	don't call memconfig for fbe
 *
 *	pel	17-Sep-1991	don't call mbx_init for pele, cobra, cb_proto
 *				and fbe targets.
 *
 *	ajb	16-Sep-1991	Check for operations on closed files
 *
 *	pel	11-Sep-1991	call io_init (IO board init) if pele,cobra
 *
 *	jds	16-Aug-1991	Moved mop_set_env() to ex_init_port().
 *
 *	jds	16-Aug-1991	Added XNA inits.
 *
 *	tga	13-Aug-1991	Add call to pkz_init()
 *
 *	jds	27-Jun-1991	added xmi_init() call
 *
 *	pel	08-Aug-1991	fix show_iobq, make iob clone quadwd multiple.
 *				
 *	jds	 2-Aug-1991	Added examine_init() call.
 *
 *	phk	30-Jul-1991	added 's' mode to fopen
 *				added 'check' shell command
 *
 *	jds	27-Jun-1991	added xmi_init() call
 *
 *	jds	16-Aug-1991	Moved mop_set_env() to ex_init_port().
 *
 *	jds	16-Aug-1991	Added XNA inits.
 *
 *	tga	13-Aug-1991	Add call to pkz_init()
 *
 *	jds	27-Jun-1991	added xmi_init() call
 *
 *	pel	08-Aug-1991	fix show_iobq, make iob clone quadwd multiple.
 *				
 *	jds	 2-Aug-1991	Added examine_init() call.
 *
 *	phk	30-Jul-1991	added 's' mode to fopen
 *				added 'check' shell command
 *
 *	jds	27-Jun-1991	added xmi_init() call
 *
 *	pel	19-Jun-1991	update autodoc headers
 *
 *	pel	03-Jun-1991	cosmetic changes to show_iobq
 *
 *	pel	24-May-1991	chg iobq inserts to be at blink instead of fl.
 *
 *	pel	23-May-1991	expand iob name to inode name when opening
 *				an existing filestream.
 *
 *	pel	20-May-1991	complete show_iobq status report
 *
 *	jrk	17-May-1991	add OPTIONAL conditional
 *
 *	ajb	16-May-1991	Check for poorly formed regular expressions
 *
 *      pel     08-May-1991     Give new ram disk files execute permission.
 *
 *      djm     06-May-1991     Added xct_init
 *
 *	jds	03-May-1991	Add puu_init
 *
 *	pel	03-May-1991	remove getpcb from fread/fwrite.
 *				Add IOB accounting to fgetc/fputc
 *
 *	ajb	24-Apr-1991	Use static storage for create_noopen
 *
 *	pel	03-Apr-1991	finish iobq implementation.
 *
 *	kl	02-Apr-1991	Mailbox driver init
 *
 *	pel	29-Mar-1991	more iobq work; not fully implemented yet
 *
 *	pel	28-Mar-1991	fclose; keep iolock out if pipe file.
 *				have fread/fwrite, fwrite_nolock 
 *				incrment IOB io_count & byte counts.
 *				start coding alloc_iob, init_iob
 *
 *	jds	28-Mar-1991	Added init calls for FPR, PT drivers.
 *
 *	cfc	27-Mar-1991	Add check for null fp in fclose();  
 *				If an input file is non-existent, fopen
 *				returned a null fp to krn$_process which 
 *				fclose tried to close, de-referencing thru 0.
 *
 *	pel	26-Mar-1991	remove PCB byte count updates from fread/fwrite
 *
 *	jad	25-Mar-1991	Add automatic NI inits.
 *
 *	pel	21-Mar-1991	remove fp/ddb_dump, routines.
 *
 *	ajb	21-Mar-1991	Add one character pushback for all files
 *
 *	jad	14-Mar-1991	Add some database inits.
 *
 *	pel	12-Mar-1991	let fopen/close incr/decr inode ref & collapse
 *				wrt_ref, rd_ref into just ref.
 *				Comment out iic_init so it'll work on h/w
 *				iic interface.
 *
 *	db	11-Mar-1991	Add init call for iic bus driver
 *
 *	sfs	28-Feb-1991	Extend LS to work with the ODS-2 driver.
 *
 *	ajb	26-Feb-1991	Let LS print out entire directory if no
 *				arguments on command line.
 *
 *	jad	21-Feb-1991	add network database render.
 *
 *	sfs	19-Feb-1991	Lots of changes to implement a true, flat
 *				name space.  Tightening of rules regarding
 *				presence of colons and slashes in file names.
 *				Simplification of ALLOCINODE, rewrite of
 *				VALIDATE_FILENAME.  Fixed bugs in CREATE_NOOPEN
 *				and REMOVE, mainly regarding unsupported driver
 *				functions.  Performance enhancement to LS.
 *				Implementation of common routines to increment
 *				or decrement read and write reference counts.
 *
 *	pel	07-Feb-1991	add file protection. print remove errors.
 *
 *	ajb	04-Feb-1991	Pass size and number transparently down to
 *				drivers for fread and fwrite.
 *
 *	ajb	21-Jan-1991	Remove putchar_nolock
 *
 *	pel	15-Jan-1991	create_noopen, let driver print failure msg.
 *				Terminate ls command if killpending set.
 *				
 *	pel	14-Jan-1991	fopen, let driver print failure msg.
 *				
 *	pel	10-Jan-1991	allocinode; match on filename abbreviations.
 *				fix end of ilist search wraparound.
 *
 *	ajb	10-Jan-1991	Remove port initialization routines
 *
 *	pel	08-Jan-1991	handle allocinode amibiguous filename return.
 *				fix end of ilist search wraparound.
 *
 *	pel	07-Jan-1991	validate_filename, remove extra regexp_extract
 *				bitflag. 
 *
 *	ajb	02-Jan-1991	Add ssctt to isatty
 *
 *	pel	21-Dec-1990	fix fopen("column/foo so filename is "".
 *				make lower limit for column be 1 not 0.
 *
 *	ajb	13-Dec-1990	Add isatty routine
 *
 *	pel	06-Dec-1990	allow ':' in filename. use hint in allocinode.
 *
 *	pel	30-Nov-1990	use err_printf instead of fprintf.
 *
 *	pel	28-Nov-1990	access stdin/out/err differently.
 *
 *	pel	20-Nov-1990	Output errs to STDERR.
 *
 *	pel	19-Nov-1990	chg filter/hexify/column_open for protocol
 *				towers. Chg fopen error msg.
 *
 *	pel	16-Nov-1990	create_noopen; work w/ protocol towers
 *
 *	pel	14-Nov-1990	chg fopen for rd/pi drivers & protocol towers.
 *
 *	jds	12-Nov-1990	Add inits for examine/deposit drivers to filesys_init().
 *
 *	sfs	08-Nov-1990	Add protocol drivers.
 *
 *	pel	07-Nov-1990	fopen, create_nopen; chg file_name array size
 *
 *	pel	06-Nov-1990	fopen, add fixed inode attribute support.
 *
 *	pel	02-Nov-1990	Plug mem leak in fopen; don't malloc file_name.
 *
 *	ajb	02-Nov-1990	Compensate for compiler bug in fgetc
 *
 *	sfs	02-Nov-1990	Add fd_init() (for Mannequin).
 *
 *	pel	01-Nov-1990	add create_noopen.
 *
 *	pel	29-Oct-1990	add level of indirection to file offset
 *
 *	pel	25-Oct-1990	pprintf in fopen instead of printf. Init
 *				fp status to msg_success in fopen.
 *
 *	ajb	24-Oct-1990	Remove MCS character in regular expressions
 *				to avoid aggravating a bug in REGEXP.C
 *
 *	pel	22-Oct-1990	Use MAX_NAME instead of MAX_DRIVER_NAME.
 *                              In remove, ls, use &ip->iolock, not ip->iolock
 *                              
 *	pel	11-Oct-1990	Add fwrite_nolock.
 *                              
 *	pel	04-Oct-1990	Add ftell, fseek constants.
 *                              
 *	pel	28-Sep-1990	Delete fslock code; sync ls; chg freeinode.
 *                              Fix remove to post iolock if error.
 *                              
 *	pel	27-Sep-1990	Sync on iolock before allocating an inode.
 *                              Init all inode iolock semaphores at startup.
 *                              
 *	pel	25-Sep-1990	Validate filenames in fopen. Process status
 *                              returned from xx_open & xx_create.  Dump 
 *                              file descr mode in fp_dump.
 *                              
 *	pel	17-Sep-1990	Display read/wrt ref counts in ls.
 *                              
 *	cfc	15-Sep-1990	Change to ls:  use qscan to expand file names.
 *                              Now passes rd_scan callback (located in 
 *				rd_driver.c) to get walk inodes.  Note: INODE
 *				expansion has been removed from LEXICAL.
 *
 *	pel	14-Sep-1990	alloc inode only if !inuse and ref count = 0.
 *                              allow only writeable files to be deleted.
 *                              chg fopen to strcmp on "pi" not ==
 *
 *	pel	12-Sep-1990	init append_offset in allocinode.
 *
 *	ajb	11-Sep-1990	Add missing =
 *
 *	pel	07-Sep-1990	make it portable.
 *                              
 *	pel	06-Sep-1990	return error if fopen pipe access mode=write.
 *                              kludgy for now until fopen changed to return
 *                              driver dd_open status.
 *                              wait/post write_lock for fwrite,fputc,fputs.
 *                              Add un/lock_write_fp/fd, freeinode functions.
 *
 *	pel	21-Aug-1990	don't take out fslock in fclose; & fix fwrite.
 *
 *	pel	17-Aug-1990	start chging to reflect des spec by making chgs
 *				required for pipe driver & serialized writes.
 *
 *	pel	06-Aug-1990	autodoc to reflect how code will soon work.
 *
 *	pel	17-Jul-1990	put ls back in after deleting it accidentally.
 *
 *	pel	12-Jul-1990	renamed fdelete to remove. Added autodoc text.
 *
 *	pel	10-Jul-1990	moved fread from boot.c
 *
 *	sfs	06-Jul-1990	Fix calling sequence for fseek.
 *
 *	sfs	27-Jun-1990	Rework SCSI and DSSI hooks.
 *
 *	pel	20-Jun-1990	Add pipe driver, pi_driver hook.
 *				Streamline remove.
 *
 *	ajb	30-May-1990	Newly created files have as access
 *				control a system wide default value.
 *
 *	ajb	14-May-1990	Remove tty specifics from fgets.
 *
 *	ajb	10-May-1990	Add autodoc templates for routines.
 *
 *	ajb	30-Apr-1990	Extract device specific driver functions.
 *
 *	ajb	27-Apr-1990	Add driver function tables.
 *
 *	ajb	19-Apr-1990	Initial entry.
 *
 *--
 */
#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include	"cp$src:wwid.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:pb_def.h"
#include	"cp$src:sb_def.h"
#include	"cp$src:ub_def.h"
#include	"cp$src:ni_dl_def.h"
#include 	"cp$src:probe_io_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:kgpsa_emx.h"
#include	"cp$src:kgpsa_def.h"

#define MAX_PATH 128

/*
 * Component blocks. There is one of these for each component in a 
 * protocol tower.
 */
struct CB {
	char	name [MAX_PATH];
	char	filename [MAX_PATH];
	char	*info;
	char	*next;
	int	matches;	/* number of matches reported			*/

	int	ilc;		/* number of inodes that matched filename	*/
	struct INODE **ilv;	/* list of inodes that matched filename		*/
};

/*
 * Callback structure.  All variables for fexpand are put in here to
 * facilitate the recursive callback mechanism.
 */
struct CBS {
	int cbmax;		/* number of components allocated	*/
	int cbc;		/* number of components	found 		*/
	struct CB *cbl;		/* list of decomposed protocol elements	*/

	char	curpath [MAX_PATH];

	/* a clone of fexpand's arguments	 	*/
	int	longformat;
	int	(*callback) ();	/* callback pass to fexpand */
	int	p0;
	int	p1;
};

#include	"cp$inc:prototypes.h"

#if MODULAR
int	num_builtins;
int	num_encap;

/* 
 * Table of supported file systems. fopen()/fclose() use this table to 
 * determine if a file system overlay must be loaded/unloaded
 */
struct FS_TABLE {
    char *fs;
    int ref;
} fs_table [] = {
    {"FAT", 0},
    {"ODS2", 0},
    {"ISO9660",0}
};

#define LOAD 1
#define UNLOAD 2

#endif

#if !MODULAR
#include	"cp$src:dst.c"
#endif

extern	null_procedure ();

extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;

struct FILE *fopen( char *pathname, char *mode);

#if FIBRE_CHANNEL
extern struct registered_wwid *find_wwidfilter_from_evnum( int unit, int *idx );
extern struct registered_wwid **wwids;
#endif

/*
 * Inode access rules:
 *	1) Inodes are created as they are needed.
 *	2) Once created, they never go away.
 *	3) They are alway put at the head of the list
 *	4) No synchronization techniques are required for
 *	   traversing the list if you don't mind not getting
 *	   the latest version.
 *	5) Looking inside the inode requires using ilist_lock and ilist_unlock
 *	6) A NULL link indicates the end of the list
 */
struct INODE *ilistp;		/* pointer to singly linked list of inodes */
struct SEMAPHORE ilist_sync;
struct SEMAPHORE_OWNER ilist_sync_owner;
/*
 * Allocate space for this many inodes initially.  This is roughly the
 * size of the number of builtins plus a few.
 */
int ilist_alloc;	/* number of inodes allocated */
	
struct QUEUE _align (QUADWORD) iobq;	/* system wide IO status block q*/
struct LOCK _align (QUADWORD) spl_iobq = {0, IPL_SYNC, 0, 0, 0, 0, 0};
int iobq_count;				/* count of system wide iobq entries */
struct IOB	_align (QUADWORD) sys_iob; /* system iob to record errs that */
					   /* aren't assoc w/ a diagnostic */
extern struct LOCK spl_kernel;

	/* a few function declarations */
void insq_iob (struct QUEUE *entry);
void remq_iob (struct QUEUE *entry);
void init_iob (struct IOB *iob, char *name);
void create_iob (struct IOB *iob, char *name);

#if AVANTI || MIKASA || K2
extern  int PowerUpProgress (int value);
#endif


/*+
 * ============================================================================
 * = filesys_init - initialize the file system                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Initialize the file system, ie,. allocate the inode lists.
 * 	There should be no machine specific code in this routine.
 *
 *	This routine is called at elevated IPL by the primary in the init
 *	flows, with no semaphore posting/waiting available.  The heap has
 *	been initialized.
 *
 *  
 * FORM OF CALL:
 *  
 *   	filesys_init ();
 *  
 * RETURN CODES:
 *
 *	msg_success - success
 *       
 * SIDE EFFECTS:        
 *
 *	see overview
-*/
int filesys_init () {
	int	inode_estimate;
	int	i;
	struct INODE	*ip;
	extern int	num_builtins;
	extern int	num_drivers;
	extern int	num_encap;

	/* Initialize the sync point */
	krn$_seminit (&ilist_sync, 1, "ilist_sync");
	ilist_sync.owner = &ilist_sync_owner;
	ilist_sync_owner.owner_pcb = 0;
	ilist_sync_owner.new_affinity = -1;
	ilist_sync_owner.new_priority = 7;

	/*
	 * Allocate space for the initial inode array, and the pointers
	 * to those inodes.  We're still running in the context of the idle
	 * process, so we can't use DYN$K_SYNC when allocating from the heap.
	 */
	ilist_alloc = 0;
	inode_estimate = num_builtins + num_drivers + num_encap + 8;
	ip = (struct INODE *) dyn$_malloc (
	    inode_estimate * sizeof (struct INODE), 
	    DYN$K_NOSYNC | DYN$K_FLOOD | DYN$K_NOOWN
	);

	/*
	 * Link all the inodes together.
	 */
	ilistp = 0;
	for (i=0; i< inode_estimate; i++, ip++) {
	    inode_init (ip);
	}

	/*
	 * init the IOB queue header
	 */
	iobq.flink = (void *) &iobq.flink;
	iobq.blink = (void *) &iobq.flink;
	iobq_count = 0; 

	/* create system wide IOB as a catchall for errors not
	 * associated with a diagnostic, such as an ISR encountering
	 * an error & reporting it via report_error
	 */
	init_iob( &sys_iob, "system");
	sys_iob.pcb = getpcb();
	sys_iob.pcbq.flink = (void *) &sys_iob.pcbq.flink;
	sys_iob.pcbq.blink = (void *) &sys_iob.pcbq.flink;
	insq_iob( &sys_iob.sysq);

	return msg_success;
}

/*
 * Initialize an inode by putting it on the inode linked list.
 */
void inode_init (struct INODE *ip) {
	ip->flink = ilistp;
	ilistp = ip;
	ilist_alloc++;
}

#if !MODULAR
/*+
 * ============================================================================
 * = ddb_startup - start up drivers listed in the startup table               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * For the current phase, start each driver in the startup table.
 * 
 * FORM OF CALL:
 *  
 *  ddb_startup (phase)
 *  
 * RETURN CODES:
 *
 *	msg_success - success
 *       
 * ARGUMENTS:
 *
 *  int phase - start all drivers in this phase
 *
 * SIDE EFFECTS:
 *
-*/
int log_count;

int ddb_startup (int phase) {
	struct DST *dstp;
	int	status;
	int	i;

#if LOG_DRIVER_INIT
	if (phase == 5) log_count = pprintf ("initializing ");
#endif
	for (dstp=dst, i=0; i<num_drivers; i++, dstp++) {
	    if (dstp->phase != phase) continue;

		if (!BadDriver(i))							/* Not a driver that failed last time */
		{
			SetMissDriver(i);

			if (SimpleComDebug())
			    PowerUpProgress (0xd0|i, "calling module %s\n", dstp->name);

			status = (*dstp->startup) (phase);
			if (status != msg_success) {
			qprintf ("%08X exit status for %s_init\n", status, dstp->name);
			}

			if (SimpleComDebug())
			    PowerUpProgress (0xd0|i, "exiting module %s\n", dstp->name);

			ClearMissDriver(i);
		}
	}
#if LOG_DRIVER_INIT
	if (phase == 5) pprintf ("\n");
#endif
	return msg_success;
}
#endif

void log_driver_init (struct pb *pb) {
#if LOG_DRIVER_INIT
    if (log_count > 72) {
	pprintf ("\n ");
	log_count = 1;
    }
    log_count += pprintf ("%s%s ", pb->protocol, pb->controller_id);
#endif
}

#if !MODULAR
/*+
 * ============================================================================
 * = ddb_startup_driver - start up a single driver.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * This routine will startup a single driver in the driver startup table.
 * 
 * FORM OF CALL:
 *  
 *  ddb_startup (phase)
 *  
 * RETURN CODES:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *  char *driver - The name of the driver to be started.
 *
 * SIDE EFFECTS:
 *
-*/
void ddb_startup_driver (char *driver) {
	int i;
	struct DST *dstp;
	int	status;

	/* Go through the driver list and look for the driver*/
	for (dstp=dst, i=0; i<num_drivers; i++, dstp++) {

	    /*If we found the driver start it*/ 
	    if (!strcmp(dstp->name,driver)) {
#if AVANTI || MIKASA || K2 || MTU
/* Mustang continually restart the vga driver when running the
 * graphics script. This fills the event log, so supress vga start messages.
 */
		if (strcmp( dstp->name, "vga"))
	            qprintf (msg_init_port, dstp->name);
	    	(*dstp->startup) ();
		if (strcmp( dstp->name, "vga"))
	            qprintf (msg_port_inited, dstp->name);
#else
	        qprintf (msg_init_port, dstp->name);
	    	(*dstp->startup) ();
	        qprintf (msg_port_inited, dstp->name);
#endif
		break;
	    }	/* end for */
	}

}
#endif

/*+
 * ============================================================================
 * = driver_setmode - apply a mode to all existent drivers                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Call each drivers' setmode entry point with the passed mode.  We have to
 * walk down all the inodes in order to discover all of the drivers.
 *
 * FORM OF CALL:
 *  
 *  driver_setmode (mode, log, one_pb)
 *  
 * RETURN CODES:
 *
 *	msg_success - success
 *       
 * ARGUMENTS:
 *
 *  int mode - value to pass to each drivers' setmode
 *
 * SIDE EFFECTS:
 *
-*/
struct setmode_info {
	char *name;
	int more;
	int wait;
};

int driver_setmode (int mode, int log, protoarg struct pb *one_pb) {
	struct PCB *pcb;
	int	pri;
	int	aff;
	int	driver_setmode_sub ();
	char	*last_name;
	int	tick;
	int	wait;
	char	change[32];
	char	*what;
	struct setmode_info *si;

	si = dyn$_malloc (sizeof (*si), DYN$K_SYNC | DYN$K_NOOWN);
	si->name = 0;
	si->more = 1;
	si->wait = 1;
	pcb = getpcb ();
	pri = pcb->pcb$l_pri;
	aff = pcb->pcb$l_affinity;
	krn$_create (driver_setmode_sub, null_procedure, 0, pri, aff, 0,
		"setmode", "nl", "r", "nl", "w", "nl", "w",
		mode, log, si, one_pb);

	if (log)
	    wait = 100;
	else
	    wait = 600;

	if (mode == DDB$K_READY) {
	    what = "poll";
	} else if (mode == DDB$K_START) {
	    what = "start";
	} else if (mode == DDB$K_STOP) {
	    what = "stop";
	} else {
	    sprintf (change, "change mode to %d", mode);
	    what = change;
	}

	while (si->more) {
	    if ((mode == DDB$K_READY) && killpending ()) {
		si->wait = 0;
		return msg_ctrlc;
	    }
	    krn$_sleep (100);
	    if (si->name) {
		if (last_name == si->name) {
		    tick++;
		    if (tick > wait) {
			pprintf ("waiting for %s to %s...\n", last_name, what);
			tick = 0;
		    }
		} else {
		    last_name = si->name;
		    tick = 0;
		}
	    }
	}
	si->wait = 0;
	krn$_sleep (200);

	return msg_success;
}

void driver_setmode_sub (int mode, int log, struct setmode_info *si, struct pb *one_pb) {
	int	i;
	struct pb	*pb;
	struct DDB	*ddb;
	struct DDB	**ddblist;

	if (!(log & 2)) {
	    ddblist = generate_ddblist ();
	    for (i=0; ddb = ddblist [i]; i++) {
		si->name = ddb->name;
		ddb->setmode (mode);
	    }
	    free (ddblist);
	}

	if (!(log & 4) || !one_pb || !one_pb->setmode) {
	    krn$_wait (&pbs_lock);
	    for (i=0; i<num_pbs; i++) {
		pb = pbs[i];
		if (pb->setmode) {
		    si->name = pb->name;
		    pb->setmode (pb, mode);
		}
	    }
	    krn$_post (&pbs_lock);
	} else {
	    si->name = one_pb->name;
	    one_pb->setmode (one_pb, mode);
	}

	si->more = 0;

	while (si->wait) {
	    krn$_sleep (100);
	}
	dyn$_free (si, DYN$K_SYNC | DYN$K_NOOWN);
}

/*+
 * ============================================================================
 * = ilist_lock - lock up access to the inode list                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Using a global spinlock, syncrhonize access to the inode list.
 * 
 * FORM OF CALL:
 *  
 *  ilist_lock ()
 *       
-*/
void ilist_lock () {
	krn$_wait (&ilist_sync);
}

void ilist_unlock () {
	krn$_post (&ilist_sync);
}


/*+
 * ============================================================================
 * = allocinode - find/create a file on the inode list                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Find a file on the list of inodes.  If an inode with a matching filename
 * does not exist, then use an available inode to create a file entry if the
 * create flag is set.  If a matching inode if found but the inode inuse
 * flag is 0 then return not found status if create flag not set.
 *
 * Expand the inode space if there isn't enough room by allocating a new inode
 * and tacking it onto the begining of the list.
 * 
 * In non error cases return with the ilist_sync taken out so that the caller gets
 * atomic access to the inode.  The caller needs to release this semaphore.
 * 
 * FORM OF CALL:
 *  
 *  allocinode( name, can_create, ipp);
 *  
 * RETURN CODES:
 *
 *	0 -	file not found and not created.  If can_create is true then
 *              all inodes are currently used, and we couldn't allocate
 *		space for a new one (a fairly catastrophic event!).
 *
 *	1 -	file found and iolock taken out.
 *
 *	2 -	file created and iolock taken out.
 *
 *	3 -	filename is ambiguous, no iolock taken out.
 *       
 * ARGUMENTS:
 *
 *  char *name - name on inode list to look for/create
 *  int can_create - create an inode if name not found and this flag is set.
 *  struct INODE **ipp - pointer to a pointer to an inode.  Gets set to the
 *			inode that was found/created.
 *
 * SIDE EFFECTS:
 *
 *  Modifies inode list.
-*/

int allocinode (char *name, int can_create, struct INODE **ipp) {
    int		i, matches;
    struct INODE	*first_free;
    struct INODE	*ip, *match_ip;

    /* serialize access to the inode list */
    ilist_lock ();

    /*
     * Walk the inode list and count proper substring matches. Also remember
     * the first free inode in case we have to create the file.
     */
    matches = 0;
    first_free = 0;
    ip = ilistp;
    while (ip) {

	/* Node is not in use.  Since this is a free node, remember
	 * its location for future reference.
	 */
        if ((!ip->inuse) && (ip->ref == 0)) {
	    first_free = ip;

	/* node is in use */
        } else {
	    switch (substr_nocase( name, ip->name) ) {

	    /* exact match */
	    case 1:
		*ipp = ip;
		return 1;
	        break;

	    /* Not an exact match; save a pointer to the inode
	     * and see if other inode names match the name.
	     */
	    case 2:
		if (!can_create) {
	            matches++;
	            match_ip = ip;
		}
	        break;
            }
        }

	/* advance to the next inode */
	ip = ip->flink;
    }


    /*
     * Unique match, file found
     */
    if (matches == 1) {
	*ipp = match_ip;
	return 1;

    /*
     * Filename is ambiguous
     */
    } else if (matches != 0) {
	ilist_unlock ();
	return 3;

    /*
     * Name wasn't found, yet we can create it
     */
    } else if (can_create) {

	/*
	 * Allocate room for the inode if there isn't one available, and then
	 * link it onto the list.
	 */
	if (!first_free) {
	    first_free = (struct INODE *) dyn$_malloc (sizeof (struct INODE),
		DYN$K_SYNC | DYN$K_FLOOD | DYN$K_NOOWN
	    );
	    inode_init (first_free);
	}

	ip = first_free;
        ip->inuse = 1;
        ip->loc = 0;
        ip->len[0] = 0;
        ip->len[1] = 0;
        ip->alloc[0] = 0;
        ip->alloc[1] = 0;
        ip->ref = 0;
        memset (ip->append_offset, 0, sizeof (ip->append_offset));

        strncpy( ip->name, name, MAX_NAME);
        *ipp = ip;
        return 2;

    /*
     * Name not found, and we can't create a new one
     */
    } else {
	ilist_unlock ();
	return 0;
    }

}

/*+
 * ============================================================================
 * = freeinode - delete an inode from the inode list                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Delete an inode from the inode list. 
 * The caller needs to take out the inode iolock before calling freeinode.
 *  
 * FORM OF CALL:
 *  
 *  freeinode( ip);
 *  
 * RETURN CODES:
 *
 *	msg_success - success
 *       
 * ARGUMENTS:
 *
 *  struct INODE *ip - pointer to the inode to be deleted.
-*/

int freeinode (struct INODE *ip) {

    ip->inuse = ip->ref = 0;
    INODE_UNLOCK (ip);

    return msg_success;
}

/*+
 * ============================================================================
 * = allocfp - find and allocate a slot in the process's open file list       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Find and allocate a slot in the process's open file list, which is located 
 * in the Process Control Block.
 *  
 * FORM OF CALL:
 *  
 *	allocfp(name,perm);
 *  
 * RETURN CODES:
 *
 *	address of slot - success
 *
 *	0               - if no slots available
 *
 * ARGUMENTS:
 *
 *  char *name - address of name string to be assigned to the IOB of the
 *  file descriptor.
 *
 *  int perm - flag to be set for permanent (not owned by process) fp
-*/

/*----------*/
struct FILE *allocfp (char *name, int perm) {
	struct PCB	*pcb;
	struct FILE	*fp;

	if (perm) {
	    fp = dyn$_malloc (sizeof (struct FILE), DYN$K_SYNC|DYN$K_NOOWN|DYN$K_FLOOD);
	} else {
	    pcb = getpcb();
	    fp = dyn$_malloc (sizeof (struct FILE), DYN$K_SYNC|DYN$K_FLOOD);
	    create_iob (&fp->iob, name);
	    insq (fp, pcb->pcb$r_fq.blink);
	}
	fp->offset = &fp->local_offset;
	fp->status = msg_success;
	return fp;
}



/*----------*/
void insq_iob (struct QUEUE *entry) {

	/* The iobq queue is synchronized with the iobq spinlock */
    spinlock (&spl_iobq);
    iobq_count++;
    insq (entry, iobq.blink);
    spinunlock (&spl_iobq);
    return;
}


/*----------*/
void remq_iob (struct QUEUE *entry) {

    spinlock (&spl_iobq);
    remq (entry);
    iobq_count--;
    spinunlock (&spl_iobq);
    return;
}

/*----------*/
void init_iob (struct IOB *iob, char *name) {
    iob->hard_errors = iob->soft_errors = iob->retry_count = 0;
    *(INT*) &iob->bytes_read    = 0;
    *(INT*) &iob->bytes_written = 0;
    *(INT*) &iob->io_count = 0;
    strncpy( iob->name, name, MAX_NAME);
    return;
}

/*----------*/
void create_iob (struct IOB *iob, char *name) {
    struct PCB	*pcb;

    pcb = getpcb();
    init_iob( iob, name);
    iob->pcb = pcb;
    insq (&iob->pcbq, pcb->pcb$r_iobq.blink);
    insq_iob( &iob->sysq);
    return;
}

/*+
 * ============================================================================
 * = remove - delete a file/device/pipe from the system                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Delete a file from the system.  In essence this will work only with RAM disk
 * files.  In almost every case atttempting to remove devices will fail since
 * the driver delete routines of most drivers will not support deletion.
 *
 * remove sometimes works in two stages and so it won't always complete 
 * immediately.  Here's an example of why file deletion is sometimes postponed.
 * 
 * Process A is in the middle of writing data to a file residing in memory.  
 * Meanwhile process B deletes the file and process C uses the same inode to 
 * point to a new file.  Now process A's writes will trash the new file as well
 * as the file length field of the new inode (if append was the access mode).
 * 
 * remove works as follows.  The inode ref count is decremented and if it's
 * now equal to zero then the file is deleted and the inode memory is freed.  
 * 
 * If after the decrement the ref count is still > 0 then another process 
 * still has the file open so  the inode inuse field is set to zero indicating
 * that a file deletion is pending on that file.  Except for fclose, subsequent
 * I/O on that file will fail because the inode inuse flag is always checked
 * during I/O operations (at the driver level).  An fclose operation issued by 
 * the last process to have the file open will have the side effect of 
 * completing the file deletion started by remove.
 * 
 * The inode ref count decrement and setting inuse to 0 is an atomic operation.
 *  
 * FORM OF CALL:
 *  
 *	remove( name);
 *  
 * RETURN CODES:
 *
 *  msg_success - normal completion
 *       
 *  msg_nofile - file not found
 *       
 *  msg_ambig_file - ambiguous filename, not deleted
 *       
 *  msg_write_prot - device not writeable
 *       
 *  msg_fatal - driver delete failed
 *       
 * ARGUMENTS:
 *
 * 	char *name - address of name string of inode to delete
-*/
int remove (char *name) {
	struct INODE	*ip;
	int		status;
	int		index;

	/*
	 * Find the name so that we know what driver to call
	 * for the device specific portion of the delete.
	 */
	status = msg_success;
	switch( allocinode (name, 0, &ip)) {
	case 0:			/* file not found */
	    status = msg_nofile;
	    break;
	case 1:			/* file found */
            if (!(ip->attr & ATTR$M_WRITE)) {  /* must be writable to delete*/
		err_printf( msg_write_prot, ip->name);
                INODE_UNLOCK (ip);
		return msg_write_prot;
            } else if (ip->dva->delete == null_procedure) {
		err_printf( msg_illegal_operation, ip->dva->name);
		INODE_UNLOCK (ip);
		return msg_illegal_operation;
	    }
	    if ((*ip->dva->delete) (ip) == EOF)
		status = msg_fatal;
	    break;
	case 3:			/* ambiguous filename */
	    status = msg_ambig_file;
	    break;
	}

	if (status != msg_success)
	    err_printf( status);
	return status;
}

/*+
 * ============================================================================
 * = validate_filename - Validate a filename			              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	The routine verifies that a file name is syntactically correct.  It
 *	doesn't verify that the file actually exists.  As an additional
 *	service, it breaks out the "file" and "info" portions of the name and
 *	returns a pointer to the "/" character following a valid filename and
 *	optional info portion of the name.
 *
 *	Valid pathname syntax is: filename[:info][/filename[:info]]...
 *  
 * FORM OF CALL:
 *  
 *	validate_filename (name, file, info, next)
 *  
 * RETURN CODES:
 *
 *	msg_success - filename parsed and validated
 *
 *	msg_bad_filename - poorly formed file name.
 *       
 * ARGUMENTS:
 *
 * 	char *name	- pointer to null terminated string describing the 
 *			  file (pathname)
 * 	char *file	- pointer to null terminated string if valid filename 
 *			  found
 * 	char **info	- pointer to pointer to characters after ":"
 * 	char **next	- pointer to pointer to characters after "/"
-*/
int validate_filename (char *name, char *file, char **info, char **next) {
    char c;
    int i;
    int j;
    int quote_flag;

    *file = 0;
    *info = 0;
    *next = 0;
    quote_flag = FALSE;		/* No Quote's seen yet */

    for( i = 0; i < MAX_NAME; i++ )
	{
	c = *name++;
	if( c == 0 )
	    {
	    *file++ = 0;
	    return( msg_success );
	    }
	if( c == '"' )
            {
	    quote_flag = !quote_flag;	/* Quote seen invert the flag */
            continue;			/* skip processing this char  */
	    }
	if( (c == ':') && !quote_flag )
	    {
	    *file++ = 0;
	    *info = file;
	    for( j = 0; ; j++ )
		{
		c = *name++;
		if( c == 0 )
		    {
		    *file++ = 0;
		    return( msg_success );
		    }
   	        if( c == '"' )
                    {
	            quote_flag = !quote_flag;	/* Quote seen invert the flag */
                    continue;			/* skip processing this char  */
	            }
		if( (c == '/') && !quote_flag )
		    {
		    if( *name == '/' )
		        { 
			name++;
			}
		    else
		        {
			*file++ = 0;
			*next = name;
			return( msg_success );
		        }
		    }
		*file++ = c;
		}
	    }
	if( (c == '/') && !quote_flag )
	    {
	    *file++ = 0;
	    *next = name;
	    return( msg_success );
	    }
	*file++ = c;
	}
    return( msg_bad_filename );
}

/*+
 * ============================================================================
 * = create_noopen - create a file but don't open it			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  Create a new file or reinit an existing one.  This procedure does not open
 *  the file.  Reiniting an existing file will fail if that file is currently
 *  open.
 *  
 *  Only one layer of protocol is allowable in the filename.  If no driver
 *  is specified in the filename then a ram disk file will be created.
 *  
 *  If an error occurs then one error msg will be output by either the
 *  create_noopen or by the driver create routine called by create_noopen.
 *  
 * FORM OF CALL:
 *  
 *	create_noopen( name, driver, perms, filesz);
 *  
 * RETURN CODES:
 *
 *	msg_success             - success
 *  
 *	msg_bad_access_mode 	- permission attribute is invalid
 *  
 *	msg_no_file_slot	- all inodes are allocated
 *  
 *	msg_file_in_use		- file is opened by this or another process
 *  
 *	msg_bad_devicename	- invalid device syntax or unknown device name
 *
 *	msg_bad_filename 	- invalid filename syntax
 *       
 *	msg_ambig_file   	- ambiguous filename
 *       
 * ARGUMENTS:
 * 
 *  char *name	- address of file name string
 *
 *  struct DDB *dp - pointer to driver dispatch table
 *
 *  int perms 	- Permission attribute mask which is the logical OR of inode
 *		  attributes such as ATTR$M_READ, ATTR$M_WRITE, ATTR$M_EXECUTE,
 *		  ATTR$M_BINARY, ATTR$M_EXPAND.
 *		  A mask of 0 will result in the file being given the default
 *		  DEF_PERMISSION attributes which are read and write.
 *		  RAM disk files also get execute permission.
 *
 *  int filesz	- Initial filesize to be allocated. The driver create will
 *		  use filesz for its file creation but if it is 0 the driver
 *		  will use the default allocation size for that driver for
 *		  the initial filesize.
-*/

int create_noopen (char *fname, struct DDB *dp, int perms, int filesz) {
    int    status;
    char   *file;
    char   *info;
    char   *next;


    if (perms == 0) {
	perms = DEF_PERMISSION;
    }
    file = malloc (strlen (fname) + 2);
    status = validate_filename (fname, file, &info, &next);
    if (status == msg_success) {
	if (info || next) {
	    status = msg_bad_filename;
	} else if (dp->create == null_procedure) {
	    err_printf( msg_illegal_operation, dp->name);
	    status = msg_illegal_operation;
	} else {
	    status = (*dp->create) (file, perms, filesz);
	}
    }

    free (file);
    return status;
}

/*+
 * ============================================================================
 * = fopen - open a file                                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Open a file for read/write/append/update access & return the address of a
 * file descriptor.
 * 
 * RAM disk files may be created as a side effect of fopen.  Other types
 * of files (which use drivers other than rd_driver) cannot be created by fopen.
 *  
 * If an error occurs during fopen one error message is output describing the
 * the reason for the failure.  This message is output by either fopen or the
 * driver open routine called by fopen depending on which layer of code
 * detected the error.  fopen always outputs a second error message of the
 * format: file open failed for <filename>.
 *
 * FORM OF CALL:
 *  
 *	fopen( pathname, mode);
 *  
 * RETURN CODES:
 *
 *  pointer to a FILE structure - success
 *
 *  NULL - failure such as poorly formed pathname or driver open error.
 *       
 * ARGUMENTS:
 * 
 *  char *pathname - file pathname.
 * 	Format of pathname is: filename:[info][/filename:[info]]...
 *      Only filename gets stored in the inode, not the protocol specifiers.
 *      The default protocol specifier is "rd", the RAM disk.
 *
 *  char *mode	- address of access mode/attribute string.  The binary file
 *		  attribute character, 'b', is the only character
 *		  in addition to access mode characters that are allowed in
 *		  this string.  One or more characters are allowed.
 *
 *                Valid access modes are the following:
 *:
 *    -----------------------------------------------------------------------
 *    mode | description
 *    -----------------------------------------------------------------------
 *     r   | reads only, file must exist
 *         |
 *     w   | create for writing; discard previous contents if any
 *         |
 *     a   | append; open or create file for writing at eof
 *         |
 *     r+  | open for update, (i.e., read/write); file must exist
 *         |
 *     w+  | create for update; discard previous contents if any
 *         |
 *     s   | open silently; do not report anything
 *         |
 *     g   | open good only; means no fopen failures messages
 *         |
 *     b   | open file with the binary attribute
 *         |
 *     i   | open file with immediate mode (secure mode?)
 *         |
 *     p   | open file and malloc the fp pointer permanently.
 *         |
 *     z   | use memzone if append to access mode
 *:   -----------------------------------------------------------------------
 *
 *  fopens that result in a RAM disk file creation generate default permission
 *  attributes of "rwx", read/write/execute.
 *
 *  File permission attributes can be modified at execution time with the 
 *  shell command, chmod.
-*/

struct FILE *fopen (char *pathname, char *mode) {
    int    perm, status, found, i, j, msg1_printed, permission;
    char   *file;
    char   *info;
    char   *next;
    struct INODE    *ip;
    struct FILE     *fp;
    struct DDB	    *dp;

    msg1_printed = 0;	/* clear driver prints its own failure msg boolean */
    file = malloc (strlen (pathname) + 2);
    status = validate_filename (pathname, file, &info, &next);
    if (status == msg_success) {
	perm = strchr (mode, 'p') ? 1 : 0;
	if ((fp = allocfp (pathname, perm)) == NULL) {
            status = msg_no_fp_slot;		/* no room in open file table */
	} else {    
		/* Decode the access modes (ignore extraneous characters). */
	    fp->mode = 0;
	    if (strchr( mode, 'r') != NULL)
		fp->mode |= MODE_M_READ;
	    if (strchr( mode, 'w') != NULL)
		fp->mode |= (MODE_M_WRITE | MODE_M_REWRITE | MODE_M_CREATE);
	    if (strchr( mode, 'a') != NULL)
		fp->mode |= (MODE_M_WRITE | MODE_M_APPEND | MODE_M_CREATE);
	    if (strchr( mode, '+') != NULL)
		fp->mode |= (MODE_M_READ | MODE_M_WRITE);
	    if (strchr( mode, 's') != NULL)
		fp->mode |= MODE_M_SILENT;
	    if (strchr( mode, 'z') != NULL)
		fp->mode |= MODE_M_MZONE;

/* p means permanent, not secure.  use a different character */
#if 0
	    if (strchr( mode, 'p') != NULL)
		fp->mode |= MODE_M_SECURE;
#endif
	    if (strchr( mode, 'i') != NULL)
		fp->mode |= MODE_M_IMMEDIATE;

	    if (perm)
		fp->mode |= MODE_M_PERM;
#if MODULAR
            if ( check_filesys_ovly (file, LOAD, fp ) != msg_success ) {
                free (file);
                return NULL;
            }
#endif
	    switch (allocinode (file, fp->mode & MODE_M_CREATE, &ip)) {
	    case 0:				/* not found, not created */
		if (fp->mode & MODE_M_CREATE)
		    status = msg_no_ip_slot;
		else
		    status = msg_nofile;
		break;
	    case 1:				/* found */
         	if  ((fp->mode & MODE_M_WRITE) 
	        && (!(ip->attr & ATTR$M_WRITE))) {
		    INODE_UNLOCK (ip);
		    if (!(fp->mode & MODE_M_SILENT))
		          err_printf( msg_write_prot, ip->name);
		    msg1_printed = 1;
		    status = msg_write_prot;
		    break;
		}
		ip->ref++;
		INODE_UNLOCK (ip);
		strcpy( fp->iob.name, ip->name);
                break;
                
	    case 2:				/* not found, created */
		if (info || next) {
		    ip->inuse = 0;
		    status = msg_bad_filename;
		} else {
		    permission = DEF_PERMISSION | ATTR$M_EXECUTE;
		    if (strchr (mode, 'b'))
			permission |= ATTR$M_BINARY;
		    rd_create_helper (ip, permission, DEF_ALLOC);
		    ip->ref++;
		}
		INODE_UNLOCK (ip);
		break;
	    case 3:				/* ambiguous filename */
		status = msg_ambig_file;
		break;
	    } /* end switch */

	    if (status == msg_success) {
		dp = ip->dva;
		if (!dp) {
		    status = msg_failure;
		} else {	
		if ((info && !dp->allow_info)
			|| (next && !dp->allow_next)
			|| (!next && dp->allow_next)) {
		    status = msg_no_infonext;
		} else {
		    fp->ip = ip;
		    if (!info)
			info = "";
		    if (!next)
			next = "";
		    status = dp->open (fp, info, next, mode);
		    if (status != msg_success)
			msg1_printed = 1; /* driver prints failure msg */
		}
		}
		if (status != msg_success) {
		    INODE_LOCK (ip);
		    ip->ref--;
		    INODE_UNLOCK (ip);
		}
	    }
	}
    } else {
	fp = NULL;
    }
    free (file);
    if (status == msg_success) {
        return fp;
    } else {
	if (fp != NULL) {
	    if (!(fp->mode & MODE_M_PERM)) {
		remq (&fp->iob.pcbq);
		remq_iob (&fp->iob.sysq);
		remq (fp);
	    }
	    free (fp);
	}
        if (strchr(mode, 'g') == NULL && strchr (mode, 's') == NULL) {
	    if (!msg1_printed)
	        err_printf(status);
	    err_printf( msg_bad_open, pathname);
        }
        return NULL;
    }
}

/*+
 * ============================================================================
 * = fclose - close an open file                                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Close an open file, and release the file descriptor associated
 * with that file.  Multiple fclose operations have no side effects.
 *
 * If during the fclose the inode ref count goes from 1 to 0 and inuse = 0 then
 * the file had been marked for deletion prior to the fclose.  In this case 
 * fclose will release the memory allocated to the inode and to the file itself
 * if it resides in memory thereby completing the deletion operation previously
 * attempted.
 *  
 * FORM OF CALL:
 *  
 *	fclose( fp);
 *  
 * RETURN CODES:
 *
 *	msg_success - success 
 *       
 * ARGUMENTS:
 *
 *  struct FILE *fp	- address of the file descriptor
 *
 * SIDE EFFECTS:
 *
 *  Poisons the released FILE descriptor in the PCB to catch usage after
 *  closure. Could delete a file which is pending deletion; see overview. 
 *  Decrements inode ref count.
-*/

int fclose (struct FILE *fp) {
	struct INODE *ip;

	if (fp == NULL)
	    return msg_success;
	ip = fp->ip;
	INODE_LOCK (ip);
	ip->ref--;
	if (strcmp(fp->ip->dva->name,"pi"))	/* pi_close must decremnt ref count  */
	    INODE_UNLOCK (ip);

	/*
	 * Device specific close.
	 */
	(*ip->dva->close) (fp);

	/*
	 * Release the file descriptor in the PCB
	 */
	if (!(fp->mode & MODE_M_PERM)) {
	    remq (&fp->iob.pcbq);
	    remq_iob (&fp->iob.sysq);
	    remq (fp);
	}

#if 0
        if ( check_filesys_ovly (fp->ip->dva->name, UNLOAD, fp ) != msg_success )
            return msg_failure;
#endif

	free (fp);
	return msg_success;
}

/*+
 * ============================================================================
 * = getchar - gets one character from an stdin                               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * gets one character from stdin
 *  
 * FORM OF CALL:
 *  
 *	getchar();
 *  
 * RETURN CODES:
 *
 * 	an integer representing the character - if success
 *       
 *      EOF - if error or EOF
 *       
 * SIDE EFFECTS:
 *
 *  updated file descriptor
-*/

int getchar (void) {
	struct PCB *pcb;

	pcb = getpcb();
	return fgetc (pcb->pcb$a_stdin);
}

/*+
 * ============================================================================
 * = ungetc - push a character back into the input stream                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Push a character back into the input stream.
 *  
 * FORM OF CALL:
 *  
 *	ungetc( c, fp);
 *  
 * RETURNS:
 *
 *  doesn't return anything
 *       
 * ARGUMENTS:
 *
 *  char c              - Character to be pushed back
 *  struct FILE *fp	- Address of FILE descriptor for the input stream
-*/

void ungetc (int c, struct FILE *fp) {
	fp->pushback = c;
	fp->pushed = 1;
}

/*+
 * ============================================================================
 * = fgetc - return the next character in a specified stream                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Return the next character in the stream.
 *  
 * FORM OF CALL:
 *  
 *	fgetc ( fp);
 *  
 * RETURN CODES:
 *
 *	integer character - if success
 *       
 *      EOF - if error or EOF
 *       
 * ARGUMENTS:
 *
 *  struct FILE *fp - Address of FILE descriptor for the input stream
 *
 * SIDE EFFECTS:
 *
 * IOB bytes read counter is incremented if driver succeeds.
 * IOB io_count is always incremented.
-*/

int fgetc (struct FILE *fp) {
	unsigned char	c;

	/*
	 * First check for pushback
	 */
	if (fp->pushed) {
	    fp->pushed = 0;
	    return fp->pushback;
	}

	/*
	 * Call the driver's read routine with a length of 1.
	 */
	(*(INT*) &fp->iob.io_count)++;
	if ((*fp->ip->dva->read) (fp, 1, 1, &c) != 1) {
	    return EOF;
	} else {
	    (*(INT*) &fp->iob.bytes_read)++;
	    return (c & 0xff);
	}
}

/*+
 * ============================================================================
 * = fgets - read the next line from a specified input stream                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  Read in the next line from an input stream.  If we know that we are reading
 *  from the terminal, then call read_with_prompt instead of the TT_READ entry.
 *  
 * FORM OF CALL:
 *  
 *	fgets( s, maxchar, fp);
 *  
 * RETURN CODES:
 *
 *  address of input buffer - if success
 *
 *  NUL - if error or EOF
 *       
 * ARGUMENTS:
 *
 *  char *s		    - address of buffer
 *	int maxchar	    - size of buffer in bytes
 *	struct FILE *fp	- address of file descriptor for input stream.
 *
 * SIDE EFFECTS:
 *
 *      Line will be buffered when calling ttdriver.
-*/

char *fgets (char *s, int maxchar, struct FILE *fp) {
	int	c;
	char	*old_s;

	/*
	 * Read in a character at a time until we
	 * see the newline.  Unlike gets, fgets stores the newline.
	 */
	old_s = s;
	while ((--maxchar > 0)  &&  ((c = fgetc (fp)) != EOF)) {
	    if ((*s++ = c) == '\n')  break;
	}
	*s = '\0';

	return ((c == EOF) && (s == old_s)) ? NUL : s;
}

/*+
 * ============================================================================
 * = fread - reads up to a specified number of items from a stream            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Read up to a specified number of items from a specified filestream.
 * Buffering, if any, is done at the driver level.
 *  
 * FORM OF CALL:
 *  
 *	fread( buffer, size, number, fp);
 *  
 * RETURN CODES:
 *
 *	size*number - the number of characters written
 *		      This is not ANSI C standard.
 *
 *	0 - if either error or end of file
 *       
 * ARGUMENTS:
 *
 *  char *buffer    - address where stream will be read into
 *  int  size       - size of each object
 *  int number      - number of objects to read
 *  struct FILE *fp - pointer to file descriptor of input stream
 *
 * SIDE EFFECTS:
 *
 * IOB bytes read counter is incremented by # of bytes actually read.
 * IOB io_count is always incremented.
-*/

int fread( char *buffer, int size, int number, struct FILE *fp ) {
    INT bytes_read;
    INT i;
    INT n;
    INT t;

    if (size > 0xfe00 && number == 1) {
	number = size;
	size = 1;
    }
    if (fp == NULL || size > 0xfe00)
	sysfault (100);
    (*(INT*) &fp->iob.io_count)++;
    t = size * number;
    bytes_read = 0;
    while (t) {
	i = min( t, 0xfe00 );
	n = (*fp->ip->dva->read)( fp, size, i / size, buffer + bytes_read );
	bytes_read += n;
	*(INT*) &fp->iob.bytes_read += n;
	t -= n;
	if (n != i)
	    break;
    }

    return bytes_read;
}


/*+
 * ============================================================================
 * = fwrite - store a block of characters to a file stream                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Store a block of characters to a specified filestream.
 * Stall if/while another process is writing to same file.
 * Buffering, if any, is done at the driver level.
 *
 * FORM OF CALL:
 *  
 *	fwrite( buffer, size, number, fp);
 *  
 * RETURN CODES:
 *
 *	size*number - the number of characters written
 *		      This is not ANSI C standard.
 *
 *	0 - if error
 *
 * ARGUMENTS:
 *
 *  char *buffer    - address where stream will be written from
 *  int  size       - size of each object
 *  int number      - number of objects to read
 *  struct FILE *fp - pointer to file descriptor of output stream
 *
 * SIDE EFFECTS:
 *
 * IOB bytes written counter is incremented by actual # of bytes written.
 * IOB io_count is always incremented.
-*/

int fwrite ( char *buffer, int size, int number, struct FILE *fp ) {
    INT bytes_written;

    if (fp == NULL)
	sysfault (100);
    (*(INT*) &fp->iob.io_count)++;
    bytes_written = (*fp->ip->dva->write)( fp, size, number, buffer);

    *(INT*) &fp->iob.bytes_written += bytes_written;
    
    return bytes_written;
}


/*+
 * ============================================================================
 * = putchar - writes one character to the standard output channel            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * writes one character to the standard output channel.
 *  
 * FORM OF CALL:
 *  
 *	putchar( c);
 *  
 * RETURN CODES:
 *
 *  the character written - if success
 *
 *  EOF - if error or EOF
 *       
 * ARGUMENTS:
 *
 * 	char c - an integer representing the character
-*/

int putchar (char c) {
    struct PCB *pcb;

    pcb = getpcb();
    return fputc (c, pcb->pcb$a_stdout);
}


/*+
 * ============================================================================
 * = fputc - write one character to the specified file stream                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * writes one character to the specified file stream
 *  
 * FORM OF CALL:
 *  
 *	fputc( c, fp);
 *  
 * RETURN CODES:
 *
 *  the character written - if success
 *
 *  EOF - if error or EOF
 *       
 * ARGUMENTS:
 *
 *  int c           - character to be written
 *  struct FILE *fp - address of file descriptor of filestream to write
 *
 * SIDE EFFECTS:
 *
 * IOB bytes written counter is incremented if driver succeeds.
 * IOB io_count is always incremented.
-*/

int fputc (int c, struct FILE *fp) {
    int status;

    (*(INT*) &fp->iob.io_count)++;
    status = (*fp->ip->dva->write)( fp, 1, 1, &c);

    if (status != 1)
        return EOF;
    else {
	(*(INT*) &fp->iob.bytes_written)++;
        return c;
    }
}

/*+
 * ============================================================================
 * = fputs - write a string to a file stream                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Write a null terminated string to a file at the current stream pointer.
 *  
 * FORM OF CALL:
 *  
 *	fputs( s, fp);
 *  
 * RETURN CODES:
 *
 *  1   - success
 *
 *  EOF - error or EOF
 *       
 * ARGUMENTS:
 *
 *  char *s         - address of null terminated string
 *  struct FILE *fp - pointer to file descriptor of output stream
-*/

int fputs (char *s, struct FILE *fp) {
    int status;

    status = (*fp->ip->dva->write)( fp, 1, strlen( s), s);

    if (status == 0)
        return EOF;
    else 
        return 1;
}

/*+
 * ============================================================================
 * = fseek - position the file offset                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Position the file offset for subsequent reads/writes.  If the pointer
 * exceeds the current file length an error status is returned.
 *
 *  Will stall if another process is in the middle of appending a chunk of
 *  data.  After that single chunk of data has been written then fseek will
 *  resume execution.
 *  
 * FORM OF CALL:
 *  
 *	fseek( fp, offset, direction);
 *  
 * RETURN CODES:
 *
 *  msg_success - success
 *
 *  non-zero - error
 *       
 * ARGUMENTS:
 *
 *  struct FILE *fp     - pointer to stream
 *	int *offset     - address of offset
 *	int direction   - direction to move in. Valid values are:
 *                          SEEK_SET = from beginning
 *                          SEEK_CUR = from current position
 *                          SEEK_END = from end of file
 *      Seeking to file end, with offset = 0 and direction = SEEK_END, will
 *      position the file offset at the byte beyond the last append thereby
 *      setting the file offset one byte beyond the last complete "record"
 *      written.
-*/

int fseek (struct FILE *fp, INT offset, int direction) {
	struct INODE *ip;

	switch (direction) {
	case SEEK_SET:
	    *(INT *)fp->offset = offset;
	    break;
	case SEEK_CUR:
	    *(INT *)fp->offset += offset;
	    break;
	case SEEK_END:
	    ip = fp->ip;
	    *(INT *)fp->offset = max(0, *(INT *)ip->len - offset);
	    break;
	}
	return msg_success;
}

/*+
 * ============================================================================
 * = ftell - position the file offset                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Get the current file offset.
 *
 * FORM OF CALL:
 *  
 *	ftell( fp);
 *  
 * RETURNS:
 *
 *  file descriptor's current offset
 *
 *  EOF if inode unused or marked for deletion.
 *       
 * ARGUMENTS:
 *
 *  struct FILE *fp     - address of file descriptor
-*/

INT ftell (struct FILE *fp) {
    if (fp->ip->inuse)
         return *(INT *)fp->offset;
    else return EOF;
}

/*+
 * ============================================================================
 * = isatty - determine if the device being talked to is console device       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Return true if the device is a console device.  Primarily used by the shell
 *	to determine whether or not a prompt should be printed.
 *  
 * FORM OF CALL:
 *  
 *	isatty (fp)
 *  
 * RETURN CODES:
 *
 *	0 - device is not a console device (i.e. interactive)
 *
 *	1 - device is a console, and interactive
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - pointer to file descriptor
-*/
int isatty (struct FILE *fp) {
	extern struct DDB tt_ddb;

	if (fp->ip->dva == & tt_ddb) {
	    return 1;
	} else {
	    return 0;
	}
}

int isapipe (struct FILE *fp) {

	    return(!strcmp(fp->ip->dva->name,"pi"));
}

/*+
 * ============================================================================
 * = fexpand - expand a regular expression into a list of names               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine takes a regular expression and expands it into a list of
 *	names.  A callback is provided so that the names may be registered as
 *	they are discovered.  The callback takes two user (as in caller of this
 *	routine) defined parameters.
 *
 *	The expansion occurs over the inode list for the 'name' portion of a
 *	filename, and over the namespace owned by the driver for the 'info'
 *	field of the path.  For the later, the driver specific expansion
 *	routine is called.
 *

There are cases where drivers need to export simply names, or names in
conjuntion with other driver specific information.  Consider the two cases:

	ls -l foo:*txt/bar	# case 1

	for i in foo:*txt/bar 	# case 2

In the first case, the name and additional information is requested by the
user.  In the case of directories, this may be the size and attributes of the
file.  In the case of the SYM driver, it is the address of the symbol.

In the second case, just the names in the name space are provided.  Remember
that drivers determine what information is returned, and it is returned in the
form of ascii string. Iterating over a namespace would be difficult if
more information was returned other than simply the name.

A "long" format only makes sense for the topmost driver of a protocol tower,
therefore all driver expansion routines that are not at the top of the tower
see this flag as false.

In all cases, driver expansion routines are required to pass back, via the
callback, a null terminated string, without the newline.

 *
 * FORM OF CALL:
 *  
 *  fexpand (fname, longformat, callback, p0, p1)
 *  
 * RETURN CODES:
 *
 *	msg_success - success
 *      other - errors returned by callback
 * 
 * ARGUMENTS:
 *
 *  char *fname		- regular expression to be expanded
 *  int longformat	- print extended information (topmost driver only)
 *  int (*callback) ()	- address of callback.  The callback
 *  int p0		- parameter to callback
 *  int p1		- parameter to callback
 *
 * SIDE EFFECTS:
 *
-*/

int strcmp_inodes (struct INODE *ipa, struct INODE *ipb) {
	return strcmp (ipa->name, ipb->name);
}

int fexpand (char *fname, int longformat, int (*callback) (), int p0, int p1) {
	int i;
	struct INODE *ip;
	struct DDB *dp;
	struct CBS *cbs;
	struct CB *cbp;
	struct REGEXP *re;

	int	ila;		/* number of inode pointers allocated	*/
	int	ilc;		/* number of inodes cloned		*/
	struct INODE **ilv;	/* list of inodes			*/

	int	fexpand_cb ();
	int status;
	char *name;
	int	re_matches;
	int	substring_matches;

	cbs = malloc (sizeof (struct CBS));
	cbs->longformat = longformat;
	cbs->callback = callback;
	cbs->p0 = p0;
	cbs->p1 = p1;


	/*
	 * Clone the names in the inode list. Over allocate space in case
	 * more inodes get created during the clone.
	 */
	ila = ilist_alloc + 8;
	ilv = malloc (ila * sizeof (struct INODE *));

	/*
	 * Clone inode pointers.
	 */
	ilc = 0;
	ip = ilistp;
	while (ip && (ilc <= ila)) {
	    if (ip->inuse) {
		ilv [ilc++] = ip;
	    }
	    ip = ip->flink;
	}

	/*
	 * Sort the inode list in ascending order
	 */
	shellsort (ilv, ilc, strcmp_inodes);

	/*
	 * Break up the protocol tower into individual name, info and next
	 * fields.  The number of components in the tower is at most no
	 * more than the number of protocol separators (plus 1).
	 */
	cbs->cbmax = strcount (fname, "/") + 1;
	cbs->cbl = malloc (sizeof (struct CB) * cbs->cbmax);
	cbs->cbc = 0;
	cbp = & cbs->cbl [0];
	name = fname;
	status = msg_success;
	do {
	    strcpy (cbp->name, name);
	    if (status = validate_filename (cbp->name, cbp->filename, &cbp->info, &cbp->next)) {
		break;
	    }

	    /*
	     * Compile the regular expression that denotes the filename.
	     * We'll need this we scan the inode list.
	     */
	    re = regexp_shell (cbp->filename, 1);
	    if (re == 0) {
		err_printf ("%s\n", fname);
		err_printf ("poorly formed regular expression\n");
		return msg_failure;
	    }

	    /*
	     * Tag all the inodes that match this regular expression.
	     */
	    cbp->ilv = malloc (sizeof (struct INODE *) * ilc);
	    cbp->ilc = 0;
	    re_matches = 0;
	    for (i=0; i<ilc; i++) {
		ip = ilv [i];
		if (regexp_match (re, ip->name)) {
		    cbp->ilv [cbp->ilc++] = ip;
		    re_matches++;
		}
	    }

	    /*
	     * If no names matched, then see if there is one and
	     * only one proper substring match.
	     */
	    substring_matches = 0;
	    if (re_matches == 0) {
		int len;
		len = strlen (cbp->filename);
		for (i=0; i<ilc; i++) {
		    ip = ilv [i];
		    if (anchored_match (re, ip->name, 0) == len) {
			cbp->ilv [cbp->ilc] = ip; /* remember the last one */
			substring_matches++;
		    }
		}

		/* we have to have exactly one match to continue */
		if (substring_matches == 1) {
		    cbp->ilc++;
		} else {
		    status = msg_nofile;
		    regexp_release (re);
		    break;
		}
	    }

	    regexp_release (re);

#if 0
	    pprintf ("%d |%s| |%s| |%s| |%s|",
		cbs->cbc,
		cbp->name,
		cbp->filename,
		cbp->info ? cbp->info : "",
		cbp->next ? cbp->next : ""
	    );
	    for (i=0; i<cbp->ilc; i++) {
		ip = cbp->ilv [i];
		pprintf (" %s", ip->name);
	    }
	    pprintf ("\n");
#endif

	    name = cbp->next;
	    cbp++;
	    cbs->cbc++;
	} while (name);

	/*
	 * Expand the namespace of the bottom driver.  This then recursively
	 * triggers the expansion of all upstream drivers via the callback.
	 */
	if (status == msg_success) {
	    status = fexpand_cb ("", cbs, cbs->cbc-1);
	}

	/* report a failure if no expansions happened */
	if (cbs->cbl [0].matches == 0) {
	    status = msg_nofile;
	}

	/*
	 * clean up
	 */
	for (i=0; i<cbs->cbc; i++) {
	    free (cbs->cbl [i].ilv);
	}
	free (cbs->cbl);
	free (cbs);
	free (ilv);

	return status;
}

/*
 * This routine is called by driver specific expansion routines when
 * they want to report a name in their namespace.
 *
 * Think of a protocol tower as defining a namespace that is a 
 * tree, where the lowest driver  on the tower constitutes the root node,
 * and terminal nodes the topmost driver.  There are as many levels in the
 * tree as there are drivers in the tower.  Furthermore, enumerating the
 * namespace defined by the protocol tower is tantamount to a depth first
 * traversal of this tree. This means we resolve names from right to
 * left across the tower.
 *
 * The depth first traversal is done recursively via the callbacks.
 *
 */
int fexpand_cb (char *expanded_name, struct CBS *cbs, int level) {
	int	i;
	int	found;
	struct DDB	*dp;
	struct INODE	*ip;
	struct CB	*cbp;
	int	longformat;
	volatile int	*p;
	int	status;

	/*
	 * The following code fragment is here solely for quilt coverage.
	 * Its presence allows us to verify that we've checked for
	 * recursive versions of this code.
	 */
	switch (level) {
	case 0: null_procedure();	break;
	case 1: null_procedure();	break;
	case 2: null_procedure();	break;
	case 3: null_procedure();	break;
	}

	longformat = (level <= 0) ? cbs->longformat : 0;

	/*
	 * We already have an inode list of matches.  Use
	 * this list of names in conjunction with info and next
	 * fields to get at driver specific expansion routines.
	 */
	status = msg_success;
	cbp = &cbs->cbl [level];
	for (i=0; i<cbp->ilc; i++) {

	    if (killpending ()) {
		return msg_ctrlc;
	    }

	    /* determine which driver is associated with this name */
	    ip = cbp->ilv [i];
	    dp = ip->dva;

	    /*
	     * We allow for driver specific expansion iff there is an
	     * expansion routine and there is an info or next field present, and
	     * were not in this routine (at this level) as a result of a previous
	     * recursion.
	     */
	    if ((dp->expand != null_procedure) && dp->expand && (cbp->info || cbp->next)) {


		/* Complain if there is an info/next field and the driver doesn't
		 * allow it.
		 */
		if ((!dp->allow_info && cbp->info) || (!dp->allow_next && cbp->next)) {
		    err_printf ("info/next field for %s not allowed\n", ip->name);
		    return msg_failure;
		}

		/* Complain if the driver requires a next field and one isn't
		 * present.
		 */
		if (dp->allow_next && !cbp->next) {
		    err_printf ("%s has no 'next' field\n", ip->name);
		    return msg_failure;
		}

		/* invoke the driver specific expansion */
		if (level <= 0) {
		    status = (*dp->expand) (cbp->filename, cbp->info, expanded_name, longformat, cbs->callback, cbs->p0, cbs->p1);
		    cbp->matches++;
		} else {
		    status = (*dp->expand) (cbp->filename, cbp->info, expanded_name, 0, fexpand_cb, cbs, level-1);
		}

	    /*
	     * otherwise we propagate the new name back up the tower
	     */
	    } else {

		/* topmost level, report to fexpand's caller */
		if (level <= 0) {
		    inode_render (cbs->curpath, ip, longformat);
		    status = (*cbs->callback) (cbs->curpath, cbs->p0, cbs->p1);
		    cbp->matches++;

		/* more drivers on top of us, recurse with a fully qualified "next" field */
		} else {
		    if (level == cbs->cbc-1) {
			strcpy (cbs->curpath, ip->name);
		    } else {
			sprintf (cbs->curpath, "%s/%s", expanded_name, ip->name);
		    }
		    status = fexpand_cb (cbs->curpath, cbs, level-1);
		}
	    }
	}
	return status;
}

int inode_render (char *buf, struct INODE *ip, int longformat)
{
    char *proto;

    if (!ip->dva)
	proto = "  ";
    else
	proto = ip->dva->name;
    if (!proto)
	proto = "  ";

	if (longformat) {
	    sprintf (buf, "%c%c%c%c  %-8.8s  %12d/%-12d  %8x   %3d    %s",
		ip->attr & ATTR$M_READ		? 'r' : '-',
		ip->attr & ATTR$M_WRITE		? 'w' : '-',
		ip->attr & ATTR$M_EXECUTE	? 'x' : '-',
		ip->attr & ATTR$M_BINARY 	? 'b' : '-',
		proto,
		*(INT *)ip->len, *(INT *)ip->alloc, 
		ip->loc,
		ip->ref, ip->name 
	    );
	} else {
	   strcpy (buf, ip->name);
	}
	return buf;
}


int finode (char *fname, int check_info_next) {
    char *file;
    char *info;
    char *next;
    struct INODE *ip;
    struct DDB *dp;

    if (strlen (fname)) {
	file = malloc (strlen (fname) + 2);
	if ((validate_filename (fname, file, &info, &next) == msg_success)
		&& (allocinode (file, 0, &ip) == 1)) {
	    dp = ip->dva;
	    INODE_UNLOCK (ip);
	    if (check_info_next &&
		    ((info && !dp->allow_info)
			    || (next && !dp->allow_next)
			    || (!next && dp->allow_next)))
		ip = 0;
	} else {
	    ip = 0;
	}
	free (file);
    } else {
	ip = 0;
    }
    return ip;
}



void init_file_offset( struct FILE *fp )
    {
    if( fp->mode & MODE_M_APPEND ) {
	INODE_LOCK (fp->ip);
	fp->offset = &fp->ip->append_offset;
	INODE_UNLOCK (fp->ip);
    }
    else
	fp->offset = &fp->local_offset;
    }



void common_driver_open( struct FILE *fp, char *info, char *next, char *mode )
    {
    init_file_offset( fp );
    if( info && *info )
	common_convert( info, 16, &fp->local_offset, sizeof( fp->local_offset ) );
    }


/*+
 * ============================================================================
 * = setattr - change attributes of a file                                    "
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Change the attributes of a file, and return the old attributes
 * 
 * FORM OF CALL:
 *  
 *  setattr (filename, attributes)
 *  
 * RETURNS:
 *
 *	previous attributes
 *       
 * ARGUMENTS:
 *
 *  char *filename - name of file to change
 *  unsigned int attributes - bitmask of new attributes
 *
 * SIDE EFFECTS:
 *
-*/
int setattr (char *name, unsigned int attr) {
	unsigned int oldattr;
	struct INODE *ip;

	if (allocinode (name, 0, &ip) == 1) {
	    oldattr = ip->attr;
	    ip->attr = attr;
	    INODE_UNLOCK (ip);
	    return oldattr;
	} else {
	    return 0;
	}
}


/*
 * Generate a list of all the active ddbs in the system.
 * Return a malloc'd array of pointers to ddbs, which the caller
 * must deallocate.
 */
int generate_ddblist () {
	struct DDB **ddblist;
	struct INODE *ip;
	extern struct INODE *ilistp;
	extern int ilist_alloc;
	int	ddb_len;
	int	i;
	int	found;

	/* the number of drivers doesn't exceed the number of inodes */
	ddblist = malloc (sizeof (struct DDB *) * ilist_alloc);
	
	/*
	 * Walk down the inode list and remember all unique ddbs
	 */
	ip = ilistp;
	ddb_len = 0;
	while (ip) {
	    if (ip->inuse) {
		found = 0;
		for (i=0; i<ddb_len; i++) {
		    if (ip->dva == ddblist [i]) {
			found = 1;
			break;
		    }
		}
		if (!found) {
		    ddblist [ddb_len++] = ip->dva;
		}
	    }
	    ip = ip->flink;
	}
	return ddblist;
}

#if MODULAR
/* 
 * Determine if the file name specifies a file system (ie FAT, ISO9660). 
 * If so, load or unload the overlay for the file system. After unloading
 * the overlay, the files INODE is freed, removing it from the system.
 */
int check_filesys_ovly(char *file, int action, struct FILE *fp)
{
    int i, j;
    int found = FALSE;

    for (i = 0; i < (sizeof(fs_table)/sizeof(struct FS_TABLE)); i++ ) {
        j = strlen( fs_table[i].fs );
        if ( strncmp_nocase( fs_table[i].fs, file, j ) == 0 ) {
            found = TRUE;
            break;
        }
    }

    if ( found ) {
        if ( action == LOAD ) {
            if ( ovly_load( fs_table[i].fs ) == NULL )
                return msg_failure;
            fs_table[i].ref++;
        } else {
            fs_table[i].ref--;
            if ( fs_table[i].ref == 0) {
                ovly_remove( fs_table[i].fs );
                freeinode( fp->ip );
            }
        }
    }

    return msg_success;
}
#endif

#if !MODULAR

char *fd_df_atoi( char *in, int *out, char delim )
    {
    if( *in != delim )
	return( 0 );
    *out = 0;
    while( isdigit( *++in ) )
	*out = *out * 10 + *in - '0';
    return( in );
    }

struct filedev_pair {     
    char *file;
    char *protocol;
    int type;
    char *device;
    char *suffix;
    };

struct filedev_pair fd_table[] = {
    {"dk", "pk", 0, "SCSI", " 0 0"},
    {"dg", "pg", 0, "SCSI3", " 0 0"},
    {"mk", "pk", 0, "SCSI", " 1 0"},
    {"du", "pu", 0, "MSCP", " 0 0"},
    {"mu", "pu", 0, "MSCP", " 1 0"},
    {"ez", "ez", 0, "MOP", " 0 0"},
    {"ez", "ez", 1, "BOOTP", " 0 0"},
    {"fa", "fa", 0, "MOP", " 1 0"},
    {"fa", "fa", 1, "BOOTP", " 1 0"},
    {"er", "er", 0, "MOP", " 2 0"},
    {"er", "er", 1, "BOOTP", " 2 0"},
    {"ew", "ew", 0, "MOP", " 3 0"},
    {"ew", "ew", 1, "BOOTP", " 3 0"},
    {"en", "en", 0, "MOP", " 5 0"},
    {"en", "en", 1, "BOOTP", " 5 0"},
    {"fr", "fr", 0, "MOP", " 4 0"},
    {"fr", "fr", 1, "BOOTP", " 4 0"},
    {"fw", "fw", 0, "MOP", " 6 0"},
    {"fw", "fw", 1, "BOOTP", " 6 0"},
    {"dw", "dw", 0, "MOP", " 7 0"},
    {"dw", "dw", 1, "BOOTP", " 7 0"},
    {"eo", "eo", 0, "MOP", " 8 0"},
    {"eo", "eo", 1, "BOOTP", " 8 0"},
    {"dv", "dv", 0, "DVA", " 0 0"},
    {"dr", "dr", 0, "RAID", " 0 0"},
    {"dq", "dq", 0, "IDE", " 0 0"},
    {"dz", "pz", 0, "I2O", " 0 0"},
    {"dy", "py", 0, "BIOS", " 0 0"},
    {"ei", "ei", 0, "MOP", " 9 0"},
    {"ei", "ei", 1, "BOOTP", " 9 0"},
    {0}
};

/*+                       
 * ============================================================================
 * = file2dev - convert filename to environment variable device name          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Convert an internal filename corresponding to a boot device into the
 *	SRM-compatible booted_dev environment variable format. An additional
 *      int argument selects amongst protocols for devices that support more
 *      than one (such as networks).
 *
 * FORM OF CALL:
 *
 *	file2dev (char *fname, char *dname, int type)
 *
 * RETURN CODES:
 *                        
 *	msg_success	- filename successfully converted.
 *
 * ARGUMENTS:
 *
 *	char *fname - string containing the internal device filename to be
 *			converted.
 *	char *dname - buffer where converted string is written.
 *      int type - which protocol is used atop the device.
 *
 * SIDE EFFECTS:
 *
 *	none
 *
-*/

void file2dev( char *fname, char *dname, int type )
    {
    int n[5];
    int i;
    int hose;
    int bus;
    int slot;
    int function;
    int channel;
    int status;
    struct pb *pb;
    struct filedev_pair *fd;
#if DRIVERSHUT
    char fullname[MAX_NAME];
    int t;
    struct INODE *ip;
#endif

   
#if MEDULLA || CORTEX || YUKONA
    if( strncmp_nocase( fname, "userflash", 5 ) == 0 )
	{
	strcpy( dname, "userflash" );
	return;
	}	
#endif
    /*
     *  We start with a console-format name, and convert it to an SRM-format
     *  name.
     *
     *  Following are sample conversions:
     *
     *	    dkb200.2.0.1.0  <-->  SCSI  0 1 0 2 200 0 0
     *	    dud21.5.0.3.0   <-->  MSCP  0 3 0 5 21 0 0 DI 406945652330 JODY
     *	    mka500.5.0.0.0  <-->  SCSI  0 0 0 5 500 1 0
     *	    dwa0.0.0.1002.0 <-->  BOOTP 0 1002 0 0 0 7 0 00-00-93-C8-DF-3F 3
     *
     *  First, skip the protocol string specifier (dk, du, mk).  Also skip the
     *  controller letter (b, d, a).  Then convert five numbers from ASCII
     *  to decimal.  The delimiter is '.' for all but the first, where it
     *  is the controller letter.
     */
    *dname = 0;
#if DRIVERSHUT
    t = allocinode( fname, 0, &ip );
    if( t == 1 )
	{
	strcpy( fullname, ip->name );
	INODE_UNLOCK( ip );
	}
    else
	{
	status = expand_devname( fname, fullname );
	if( status != msg_success )
	    return;
	}
    fname = fullname;
#endif
    status = file2dev_parse( fname, 0, 0, n );
    if( status != msg_success )
	return;

    hose = n[4];
    bus = n[3] / 1000;
    slot = n[3] % 100;
    function = ( n[3] % 1000 ) / 100;
    pb = get_matching_pb( hose, bus, slot, function, 0 );

    /*
     *  Now match the protocol type specifier in the table.
     */
    for( fd = fd_table; fd->file; fd++ )
	{
	if( ( strncmp_nocase( fd->file, fname, 2 ) == 0 )
		&& ( type == fd->type ) )
	    {
	    /*
	     *  We've matched everything we need to match.  Now generate
	     *  the SRM-format name.  MSCP devices have extra information
	     *  tacked onto the end, which we find by traversing the MSCP
	     *  data structures in memory.
	     */
	    sprintf( dname, "%s %d %d %d %d %d%s",
		    fd->device, n[4], n[3], n[2], n[1], n[0], fd->suffix );

#if FIBRE_CHANNEL
	    if( !strcmp( fd->device, "SCSI3" ) )
		{
		  find_wwidfilter_from_unit_col(n[0], n[1]/1000, &i);
  	          sprintf( dname, "%s %d %d %d %d %d%s @wwid%d",
		    fd->device, n[4], n[3], n[2], n[1]%1000, 0, fd->suffix,
                                                            wwids[i]->evnum );
		}

	    if( !strcmp( fd->device, "SCSI3" ) )
		{
		int t;
		struct INODE *ip;
		struct ub *ub;
		struct kgpsa_sb *sb;
		t = allocinode( fname, 0, &ip );
		if( t == 1 )
		    {
		    ub = ip->misc;
		    if( ub )
			{
			sb = ub->sb;
			if( sb )
			    sprintf( dname + strlen( dname ), " %06x %016x",
				    sb->did, ub->unit<<8);
                                                 
			}
		    INODE_UNLOCK( ip );
		    }
		}
#endif /* FIBRE_CHANNEL */


	    if( !strcmp( fd->device, "MSCP" ) )
		{
		int t;
		struct INODE *ip;
		struct ub *ub;
		struct mscp_sb *sb;
		if( pb )
		    {
		    if( pb->type == TYPE_PCI )
			sprintf( dname + strlen( dname ) - 2, " %08X",
				incfgl( pb, 0x00 ) );
#if 0
		    if( pb->type == TYPE_EISA )
			sprintf( dname + strlen( dname ) - 2, " %08X",
				inportl( pb, pb->slot * 0x1000 + 0xc80 ) );
#endif
		    }
		t = allocinode( fname, 0, &ip );
		if( t == 1 )
		    {
		    ub = ip->misc;
		    if( ub )
			{
			sb = ub->sb;
			if( sb )
			    sprintf( dname + strlen( dname ), " %.2s %04X%08X %.8s",
				    ub->dd,
				    *(unsigned short *)(sb->system_id + 4),
				    *(unsigned long *)(sb->system_id + 0),
				    sb->node_name );
			}
		    INODE_UNLOCK( ip );
		    }
		}
	    if( !strcmp( fd->device, "MOP" ) || !strcmp( fd->device, "BOOTP" ) )
		{
		int t;
		struct INODE *ip;

		t = allocinode( fname, 0, &ip );
		if( t == 1 )
		    {
		    struct NI_GBL *ni_gbl;
		    struct NI_DL *ni_dl;
    		    struct pb *ni_pb;

		    ni_gbl = ip->misc;
		    ni_dl  = ni_gbl->dlp;
    		    ni_pb  = ni_gbl->pbp;

    		    if (ip->dva->net_device==DDB$K_TOKEN)
    			sprintf( dname + strlen( dname ), " %s %d", ni_pb->info, 3 );
    		    else
    			sprintf( dname + strlen( dname ), " %02X-%02X-%02X-%02X-%02X-%02X %d",
		            ni_dl->sa[0], ni_dl->sa[1], ni_dl->sa[2],
		            ni_dl->sa[3], ni_dl->sa[4], ni_dl->sa[5],
		            ip->dva->net_device );

		    INODE_UNLOCK( ip );
		    }
		}
	    break;
	    }
	}
#if DEBUG 
    pprintf("file2dev: fname-%s  dname-%s\n", fname, dname );
#endif
    }

/*+
 * ============================================================================
 * = dev2file - convert environment variable device name to filename          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Convert an SRM-compatible booted_dev environment variable string to
 *   	internal filename corresponding to a boot device.
 *
 * FORM OF CALL:
 *
 *	dev2file (char *dname, char *fname)
 *
 * RETURN CODES:
 *
 *	msg_success	- filename successfully converted.
 *
 * ARGUMENTS:
 *
 *	char *dname - string containing the booted_dev format device string
 *			to be converted.
 *	char *fname - buffer where converted string is written.
 *   
 * SIDE EFFECTS:
 *
 *	none
 *
 -*/
void dev2file( char *dname, char *fname )
    {
    dev2file_pb( dname, fname, 0 );
    }

int dev2file_pb( char *dname, char *fname, struct pb **pb_out )
    {
    char *c;
    char *d;
    char *t;
    char *protocol;
    char controller[4];
    int dl;
    int tl;
    int n[5];
    int i;
    int hose;
    int bus;
    int slot;
    int function;
    int channel;
    int status;
    struct filedev_pair *fd;
    struct pb *pb;
#if FIBRE_CHANNEL
    struct registered_wwid *w;
    int wwidx;
#endif

    /*
     *  We start with an SRM-format name, and convert it to a console-format
     *  name.
     *
     *  Following are sample conversions:
     *
     *	    dkb200.2.0.1.0  <-->  SCSI 0 1 0 2 200 0 0
     *	    dud21.5.0.3.0   <-->  MSCP 0 3 0 5 21 0 0 DI 406945652330 JODY
     *	    mka500.5.0.0.0  <-->  SCSI 0 0 0 5 500 1 0
     *
     *  First, find the length of the protocol specifier (SCSI, MSCP).
     *  Then convert five numbers from ASCII to decimal.  The delimiter
     *  is ' ' for all.
     */
    *fname = 0;
    if( pb_out )
	*pb_out = 0;
    status = dev2file_parse( dname, n, &d, &dl, &t, &tl );
    if( status != msg_success )
	return status;

    /*
     *  Now match the protocol specifier in the table.
     */
    for( fd = fd_table; fd->device; fd++ )
	{
	if( ( dl == strlen( fd->device ) )
		&& ( strncmp_nocase( fd->device, d, dl ) == 0 ) )
	    {
	    /*
	     *  Now match further if required.
	     */
	    if( ( tl == 0 )
		    || ( tl == 2 )
			    && ( strncmp_nocase( fd->suffix, t, tl ) == 0 ) )
		{
		/*
		 *  We've matched everything we need to match.  Now generate
		 *  the console-format name.  We are not able to determine
		 *  the controller letter, unless this is Cobra and the hose
		 *  is 0 (the Lbus).  In that case, we can generate the
		 *  controller letter given the slot.  Otherwise, we set the
		 *  controller letter to '*', and ask the file system to
		 *  replace it with the right letter.  We want to give up
		 *  after a while if we're not making progress.
		 */
		protocol = fd->protocol;
		strcpy( controller, "*" );
		hose = n[0];
		bus = n[1] / 1000;
		slot = n[1] % 100;
		function = ( n[1] % 1000 ) / 100;
		channel = n[2];
		krn$_wait( &pbs_lock );
		for( i = 0; i < num_pbs; i++ )
		    {
		    pb = pbs[i];
		    if( ( strcmp( pb->protocol, protocol ) == 0 )
			    && ( pb->hose == hose )
			    && ( pb->bus == bus )
			    && ( pb->slot == slot )
			    && ( pb->function == function )
			    && ( pb->channel == channel ) )
			{
			controller_num_to_id( pb->controller, controller );
			if( pb_out )
			    *pb_out = pb;
			break;
			}
		    }
		krn$_post( &pbs_lock );
#if FIBRE_CHANNEL
		/*
                 * Handle the case of indirection.   SCSI3 is the 
		 * only protocol right now with indirection, so we key 
		 * off of that.     
		 */
		 if ( strncmp(dname,"SCSI3",5) == 0 ){
		    c = strstr(dname,"@wwid");
		    if( c ) {
		      i = atoi(&c[5]);
		      w = find_wwidfilter_from_evnum( i, &wwidx );
		      if (w) {
		        n[4] = wwids[wwidx]->unit;
		        n[3] = wwids[wwidx]->collision*1000+n[3];
		      } else {
		        return msg_failure;
                      }
                    } else {
		      return msg_failure;
                    }
	         }
#endif
                if( strncmp(dname,"IDE",3) == 0 ){
                    if( n[4] == 100 )
                        n[4] = 1;
                 }
                sprintf( fname, "%s%s%d.%d.%d.%d.%d",
                   fd->file, controller, n[4], n[3], n[2], n[1], n[0] );
		break;
		}
	    }
	}
#if DEBUG
    pprintf("dev2file: dname-%s  fname-%s\n", dname, fname );
#endif
    }

int file2dev_parse( char *fname, char *protocol, char *controller, int *n )
    {
    char t[4];
    char *c;
    int i;

    if( !protocol )
	protocol = t;
    if( !controller )
	controller = t;
    c = fname;
    *protocol++ = *c++;
    *protocol++ = *c++;
    *protocol++ = 0;
    *controller++ = *c++;
    if( islower( *c ) )
	*controller++ = *c++;
    if( islower( *c ) )
	*controller++ = *c++;
    *controller++ = 0;
    c--;
    for( i = 0; i < 5; i++ )
	if( !( c = fd_df_atoi( c, n + i, i ? '.' : *c ) ) )
	    return( msg_failure );
    return( msg_success );
    }

int dev2file_parse( char *dname, int *n, int *pd, int *pdl, int *pt, int *ptl )
    {
    int i;
    char *c;
    char *d;
    char *t;
    int dl;
    int tl;

    c = dname;
    d = c;
    if( c = strchr( c, ' ' ) )
	dl = c - d;
    else
	return( msg_failure );
    for( i = 0; i < 5; i++ )
	if( !( c = fd_df_atoi( c, n + i, ' ' ) ) )
	    return( msg_failure );
    /*
     *  The part which comes next, if present, is used to distinguish among
     *  multiple table entries with the same protocol specifier (i.e., both
     *  dk and mk map to SCSI).  If it is not present, we match the first
     *  table entry which we can (dk in the case of SCSI).  The format of this
     *  next part is two numbers separated by spaces; only the first number
     *  is considered important.
     */
    tl = 0;
    if( c = strchr( c, ' ' ) )
	{
	t = c;
	if( c = strchr( ++c, ' ' ) )
	    tl = c - t;
	else
	    tl = strlen( t );
	}
    *pd = d;
    *pdl = dl;
    *pt = t;
    *ptl = tl;
    return( msg_success );
    }
#endif
