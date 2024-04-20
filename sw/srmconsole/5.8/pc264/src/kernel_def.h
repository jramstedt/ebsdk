/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:04:56 by OpenVMS SDL EV1-31     */
/* Source:   9-MAR-2000 18:12:46 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]KERNEL_DEF.S */
/********************************************************************************************************************************/
/* file:	kernel_def.sdl                                              */
/*                                                                          */
/* Copyright (C) 1990, 1995 by                                              */
/* Digital Equipment Corporation, Maynard, Massachusetts.                   */
/* All rights reserved.                                                     */
/*                                                                          */
/* This software is furnished under a license and may be used and copied    */
/* only  in  accordance  of  the  terms  of  such  license  and with the    */
/* inclusion of the above copyright notice. This software or  any  other    */
/* copies thereof may not be provided or otherwise made available to any    */
/* other person.  No title to and  ownership of the  software is  hereby    */
/* transferred.                                                             */
/*                                                                          */
/* The information in this software is  subject to change without noticec   */
/* and  should  not  be  construed  as a commitment by digital equipment    */
/* corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by digital.                  */
/*                                                                          */
/*                                                                          */
/* Abstract:	Kernel data structure definitions for                       */
/*		EVAX/Cobra firmware.                                        */
/*                                                                          */
/* Author:	AJ Beaverson                                                */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	dwn	15-Sep-1995	Added DDB$K_TOKEN                           */
/*                                                                          */
/*	cto	14-Jan-1994	Diag_common addition/subtractions to PCB    */
/*                                                                          */
/*	kl	19-Aug-1993	Ruby merge                                  */
/*                                                                          */
/*	pel	15-Jan-1993	Conditionalize for morgan                   */
/*                                                                          */
/*	phk	22-Jun-1992	Add FB driver                               */
/*                                                                          */
/*	ajb	28-May-1992	Create FLASH, which is an unstructured      */
/*				uniform byte stream for flash roms.         */
/*                                                                          */
/*	ajb	30-Apr-1992	Trim semaphore names to 16 characters for a */
/*				savings of 6500 bytes.  Semaphore names are */
/*				exist only for human readability, and are not */
/*				used by programs or scripts.                */
/*                                                                          */
/*	ajb	30-Apr-1992	Remove rlock, wlock from inodes.  The pipe  */
/*				driver is the only driver that uses them, so */
/*				let it create a private structure that hangs */
/*				of the misc field.                          */
/*                                                                          */
/*	ajb	06-feb-1992	Use ifsymbol constructs                     */
/*				to encode max_processor                     */
/*                                                                          */
/*	pel	05-Feb-1992	put IOB w/in FILE quadword aligned.         */
/*				Pad FILE struct to be multiple of quadwords. */
/*                                                                          */
/*	hcb	27-Jan-1992	include platform.defs                       */
/*                                                                          */
/*	pel	22-Jan-1992	expand IOB bytes_written,read, io_count to qwd */
/*                                                                          */
/*	pel	09-Jan-1992	Add len field to RAB struct                 */
/*                                                                          */
/*	pel	19-Dec-1991	Add SWAP DDB entry                          */
/*                                                                          */
/*	phk	15-Nov-1991	Add code_entry to FILE structure            */
/*                                                                          */
/*	phk	08-Oct-1991	Add EL & TEE  DDB entries                   */
/*                                                                          */
/*	hcb	03-Oct-1991	Add Cobra Primary IO space driver DDB entry  */
/*                                                                          */
/*	pel	30-Sep-1991	make lbus, fbus csr driver DDB entries instead */
/*				of separate csr drivers for each device.    */
/*                                                                          */
/*	pel	11-Sep-1991	gen MAX_DRIVERS automatically; add SCRAM, NCR */
/*				csr drivers.                                */
/*                                                                          */
/*	jds	16-Aug-1991	Added EX (XNA) DDB entry.                   */
/*                                                                          */
/*	phk	31-Jul-1991	Add MODE_M_SILENT                           */
/*                                                                          */
/*	ajb	29-May-1991	Add stack size to INODE                     */
/*                                                                          */
/*	hcb	22-May-1991	Add memtest setjmp id                       */
/*                                                                          */
/*	ajb	22-May-1991	Export a pointer to the shell's parse structure */
/*				into the PCB so that EXIT, BREAK, certain SET */
/*				commands and CONTROL X can touch the current */
/*				shell.                                      */
/*                                                                          */
/*	pel	20-May-1991	Add pcb pointer to IOB                      */
/*                                                                          */
/*	ajb	17-May-1991	Remember parent's pid when starting a process. */
/*                                                                          */
/*	dtm	15-May-1991	Add GBUS driver to DDB                      */
/*                                                                          */
/*	twp	14-May-1991	Removed error fields in PCB that have been  */
/*				moved into the IOB.  Added two new EV's     */
/*				to diag_ev structure (completion and startup) */
/*                                                                          */
/*	ajb	09-May-1991	Add control-c flink/blink to PCB            */
/*                                                                          */
/*      djm	25-Apr-1991	Added XCT to driver database.               */
/*                                                                          */
/*	phk	24-Apr-1991	Add CSR to driver database.                 */
/*                                                                          */
/*	ajb	24-Apr-1991	statically allocate standard filenames in   */
/*				the PCB.                                    */
/*                                                                          */
/*	ajb	23-Apr-1991	Add MAX_PATHNAME, which is the largest size */
/*				a protocol string can be.                   */
/*                                                                          */
/*	dwb	16-Apr-1991	Increase MAX_DRIVERS to 64.                 */
/*                                                                          */
/*	kl	16-Apr-1991	Entry constants, saved_pcb field for VAX    */
/*				entry context.	                            */
/*                                                                          */
/*	jad	04-Apr-1991	Add PBQ.                                    */
/*                                                                          */
/*	kl	02-Apr-1991	Add MBX to DDB                              */
/*                                                                          */
/*	pel	29-Mar-1991	add iobq to PCB                             */
/*                                                                          */
/*	hcb	29-Mar-1991	Add DDB$K_MOPLP to driver database.         */
/*                                                                          */
/*	jds	28-Mar-1991	Added FPR, PT driver defs.                  */
/*                                                                          */
/*	pel	27-Mar-1991	add IOB structure to FILE structure; delete */
/*				fatal_errors from pcb                       */
/*                                                                          */
/*	ajb	21-mar-1991	Insure at least one character of pushback for */
/*				all file types.                             */
/*                                                                          */
/*	jad	13-Mar-1991	Add ff to driver database.                  */
/*                                                                          */
/*	pel	12-Mar-1991	collapse inode wrt_ref, rd_ref into just ref. */
/*                                                                          */
/*	db	11-Mar-1991	Add DDB$K_IIC                               */
/*                                                                          */
/*	phk	08-Mar-1991	Add DDB$K_DMA                               */
/*                                                                          */
/*	kl	01-Mar-1991	Change HWRPB communications driver to a TT  */
/*				port driver.                                */
/*                                                                          */
/*	twp	25-Feb-1991	Removed expected, received, failing_vec, and */
/*				failing_addr fields from PCB - will only be */
/*				used for extended error reports if so desired. */
/*				Removed cleanup routine from PCB -- diagnostic */
/*				will now load cleanup routine as direct     */
/*				pcb rundown routine.  Removed req_section and */
/*				req_passes not needed in the PCB -- can be  */
/*				referenced directly from QSTRUCT array.  Also */
/*				renamed the 'sections' variable in the dispatch */
/*				table to now be 'groups'.                   */
/*                                                                          */
/*	ajb	25-Feb-1991	Add pointer to qualifiers in PCB.           */
/*                                                                          */
/*	jad	21-Feb-1991	Add DDB$K_NDBR                              */
/*                                                                          */
/*	sfs	19-Feb-1991	Add ALLOW_COLON and ALLOW_SLASH to DDB; remove */
/*				REF from INODE.                             */
/*                                                                          */
/*	ajb	14-Feb-1991	Pass values through semaphores.  The value  */
/*				is OR'd in so that it can be treated as a   */
/*				cluster of event flags.                     */
/*                                                                          */
/*	mrc	05-Feb-1991	Added contents of STATUS_BLOCK structure    */
/*				directly into PCB.  Eliminated DIAG_FLAGS.  */
/*				Defined a process kill setjmp id            */
/*				(PROC_KILL_SJ_ID) used to allow killpending to */
/*				jump directly to krn$_process if killpending is */
/*				set.  Added to DIAG_EVS structure.          */
/*                                                                          */
/*	kl	25-Jan-1991	Add VAX exception context to PCB            */
/*                                                                          */
/*	sfs	25-Jan-1991	Rearrange PCB; add lightweight saved Alpha  */
/*				context.                                    */
/*                                                                          */
/*	pel	24-Jan-1991	add ptr to opened semaphore to PCB.         */
/*                                                                          */
/*	jds	16-Jan-1991	Added Alpha setjmp structure.               */
/*                                                                          */
/*	mrc	14-Jan-1991	Add STATUS_BLOCK structure and diag constants. */
/*				Also add DIAG_DIS_TABLE structure, DIAG_FLAGS */
/*				structure and DIAG_EVS structure. Update PCB */
/*				to have a pointer to the status block.      */
/*                                                                          */
/*	kl	03-Jan-1991	Add hwrpb_comm driver entry                 */
/*                                                                          */
/*	sfs	20-Dec-1990	Add exception R2 through R7 to ALPHA_CTX.   */
/*                                                                          */
/*	ajb	19-Dec-1990	Add drivers types for ssc tt port and tty class */
/*				driver.                                     */
/*                                                                          */
/*	ajb	12-Dec-1990	Add a Poll Queue Element. These live on a   */
/*				polling queue, visited by the timer process */
/*				every tick, and are used to trigger drivers */
/*				when we are running polled.                 */
/*                                                                          */
/*	ajb	11-Dec-1990	Add adutt driver entry                      */
/*                                                                          */
/*	jds	07-Dec-1990	Added setjmp/longjmp defs.                  */
/*                                                                          */
/*	ajb	06-Dec-1990	Add a structure that describes the stack as */
/*				delivered by PAL code, with some enhancements */
/*				to allow saving of context.                 */
/*                                                                          */
/*	pel	05-Dec-1990	use ! to define DEF_PERMISSION              */
/*                                                                          */
/*	pel	29-Nov-1990	add a_stdin/out/err to PCB. increase max_fopen. */
/*				In PCB keep pointers to stdin/out/err filenames */
/*				instead of the names themselves.            */
/*                                                                          */
/*	jds	16-Nov-1990	Restructured entry context.                 */
/*                                                                          */
/*	ajb	15-Nov-1990	Add timestamp in PCB so that we keep better */
/*				track of time.                              */
/*                                                                          */
/*	kl	15-Nov-1990	add handler queue to PCB                    */
/*                                                                          */
/*	dtm	15-Nov-1990	add entry context fields for callback parameters */
/*                                                                          */
/*	ajb	14-nov-1990	remove control t fields                     */
/*                                                                          */
/*	pel	14-Nov-1990	Chgd MODE_M_ values; added create, rewrite  */
/*				Add DVX_WILDCARD, SEARCH_START              */
/*                                                                          */
/*	jds	12-Nov-1990	Add ids for examine/deposit drivers.        */
/*                                                                          */
/*	sfs	08-Nov-1990	Add protocol drivers.                       */
/*                                                                          */
/*	ajb	08-Nov-1990	Add access modes for stdin, stdout and stderr */
/*                                                                          */
/*	jad	07-Nov-1990	Add DDB$K_EA                                */
/*                                                                          */
/*	pel	05-Nov-1990	Add MODE_M_FIXED to prevent a file from moving */
/*                                                                          */
/*	ajb	02-Nov-1990	Remove ownership queues for semaphores and  */
/*				timers, save FPRs on Alpha                  */
/*                                                                          */
/*	sfs	31-Oct-1990	Define Alpha hardware context.              */
/*                                                                          */
/*	pel	29-Oct-1990	Add local_offset field to FILE file descriptor. */
/*                                                                          */
/*	kl	25-Oct-1990	Modify entry_context                        */
/*                                                                          */
/*	sfs	24-Oct-1990	Modify LOCK structure.                      */
/*                                                                          */
/*	ajb	16-Oct-1990	Add PCB_ALIGNMENT                           */
/*                                                                          */
/*	dtm	15-Oct-1990	break out environment variable definitions  */
/*                                                                          */
/*	jad	11-Oct-1990	Add MOPDL to driver database.               */
/*                                                                          */
/*	sfs	09-Oct-1990	Add "status" and "count" fields to FILE.    */
/*                                                                          */
/*	kl	05-Oct-1990	add entry_ctx                               */
/*                                                                          */
/*	kl	05-Oct-1990	add last_processor field to PCB	            */
/*                                                                          */
/*	dtm	05-Oct-1990	add environment variable action routines    */
/*                                                                          */
/*	pel	02-Oct-1990	delete inode fl,bl. reduce MAX_INODES to 200. */
/*                                                                          */
/*	ajb	01-Oct-1990	make pcb size a multiple of 4               */
/*                                                                          */
/*	ajb	28-Sep-1990	Add validation entry point to drivers       */
/*                                                                          */
/*	dtm	25-Sep-1990	Change EV$NOSYSTEM to EV$USER.              */
/*                                                                          */
/*	jad	24-Sep-1990	Add to NI_GBL environment variable pointer. */
/*                                                                          */
/*	dtm	19-Sep-1990	Add environment variable definitions        */
/*                                                                          */
/*	jad	07-Sep-1990	Add Misc field to control T Queue.          */
/*                                                                          */
/*	ajb	04-Sep-1990	Add user defined startup routine.           */
/*                                                                          */
/*	jad	30-aug-1990	Add alternate to TIMERQ.                    */
/*                                                                          */
/*	pel	17-Aug-1990	Add access mode field to FILE, file descriptor. */
/*                              Add binary permission inode attribute.      */
/*                                                                          */
/*	pel	14-Aug-1990	Add fields to inode per file_sys design spec. */
/*                                                                          */
/*	jad	13-Aug-1990	Add some ni definitions                     */
/*                                                                          */
/*	ajb	08-Aug-1990	Bump up idle stack size                     */
/*                                                                          */
/*	ajb	01-Aug-1990	Add process quantums                        */
/*                                                                          */
/*	ajb	30-Jul-1990	Create a spinlock data structure            */
/*                                                                          */
/*	ajb	26-Jul-1990	Add a field in the PCB that indicates what  */
/*				state the process is in.                    */
/*                                                                          */
/*	ajb	25-Jul-1990	Add ownership queue for timer queue elements. */
/*                                                                          */
/*	ajb	24-Jul-1990	Add process ids to PCB                      */
/*                                                                          */
/*	jad	24-Jul-1990	Add MOP to driver database.                 */
/*                                                                          */
/*	kl	20-Jul-1990     Additional documentation.                   */
/*                                                                          */
/*	kl,dm	17-Jul-1990	Multiprocessor upgrade (first pass)         */
/*                                                                          */
/*	ajb	4-Jul-1990	Add nokill bit to PCB so processes can protect */
/*				themselves when necessary.                  */
/*                                                                          */
/*	ajb	3-Jul-1990	Add backpointer to semaphore in PCB for when */
/*				the PCB is on a semaphore queue.            */
/*                                                                          */
/*	jad	20-Jun-1990	Add EZ to driver database.                  */
/*                                                                          */
/*	ajb	13-Jun-1990	Bump up minimum stack size                  */
/*                                                                          */
/*	sfs	30-May-1990	Add SCSI and DSSI definitions.              */
/*                                                                          */
/*	ajb	14-May-1990	Add a small buffer in the file descriptor   */
/*				to support pushing back at least one character */
/*				into the input strream.                     */
/*                                                                          */
/*	ajb	11-May-1990	Add address of completion semaphore to PCB. */
/*                                                                          */
/*	ajb	20-Apr-1990	Added file system definitions.              */
/*                                                                          */
/*	ajb	07-Mar-1990	Initial entry.                              */
/*                                                                          */
/*** MODULE $kerneldef ***/
#define TIMEOUT$K_SEM_VAL 0x80000000	/* Value bposted by krn$_timer */
#define KRN$K_MINSTACK 4096             /* minimum stack size               */
#define MAX_PRI 8                       /* number of priorities             */
#define MAX_NAME 32                     /* max size of an identifier        */
#define MAX_IPL 31
#define MAX_WHOAMI 1                    /* maximum value that whoami can return. */
#define MAX_CPU 2                       /* max number of cpus allowed       */
#define IPL_SYNC 31                     /* kernel synchronization IPL       */
#define IPL_ERR 30                      /* high IPL which still allows errors to be delivered */
#define IPL_RUN 0                       /* normally running IPL             */
#define MAX_ARGS 64                     /* max # procedure arguments        */
#define DEF_ALLOC 2048                  /* default allocation size for newly created files */
#define DEF_EXPAND 512                  /* default expansion size for files */
#define DEF_QUANTUM 10                  /* default process quantum, in ticks */
#define PCB_ALIGNMENT 128               /* required HW pcb alignment for evax */
#define ERROR_ENTRY 1                   /* error entry                      */
#define SYSTEM_RESET_ENTRY 2            /* system_reset_entry               */
#define CALLBACK_ENTRY 4                /* callback entry                   */
#define EXTERNAL_ENTRY 8                /* external entry                   */
#define NODE_RESET_ENTRY 16             /* node reset entry                 */
/*+                                                                         */
/* ===============================================                          */
/* = QUEUE - queue structure                     =                          */
/* ===============================================                          */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/*	A standard console queue is a double linked list.	            */
/*                                                                          */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ---------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ---------------------------------------------------------------------- */
/*                     flink  |	address	   | forward link                   */
/*                            |	       	   |                                */
/*                     blink  |	address    | backward link                  */
/*:      ---------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
struct QUEUE {
    struct QUEUE *flink;
    struct QUEUE *blink;
    } ;
/*+                                                                         */
/* ===============================================                          */
/* = SEMAPHORE - semaphore structure 		 =                          */
/* ===============================================                          */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* Semaphore queues are FIFO, and not prioritized. (In most cases, there will  */
/* be only one task on the queue).                                          */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ---------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ---------------------------------------------------------------------- */
/*                     flink  |	byte 	   | forward link                   */
/*                            |	       	   |                                */
/*                     blink  |	address    | backward link                  */
/*                            |	       	   |                                */
/*		       gsq    | queue      | global semaphore queue         */
/*                            |	       	   |                                */
/*                     count  | longword   | count                          */
/*                            |	       	   |                                */
/*		       value  | longword   | value that krn$_wait will return */
/*                            |	       	   |                                */
/*		       owner  | address    | auxiliary structure for owner  */
/*                            |	       	   |                                */
/*                     name   | string     | name                           */
/*:      ---------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
struct SEMAPHORE_OWNER {
    int *owner_pcb;
    int new_affinity;
    int new_priority;
    int old_affinity;
    int old_priority;
    } ;
struct SEMAPHORE {
    struct SEMAPHORE *flink;
    struct SEMAPHORE *blink;
    struct QUEUE gsq;
    int count;
    int value;
    struct SEMAPHORE_OWNER *owner;
    char name [16];
    } ;
/*                                                                          */
/* Structure of a file entry                                                */
/*                                                                          */
/*+                                                                         */
/* ===============================================                          */
/* = NI_GBL - NI Global data definitions	 =                          */
/* ===============================================                          */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* This structure is to be pointed to by the misc feild in the inode.       */
/* It simply contains pointers to the NI data structures to be used by the  */
/* drivers.                                                                 */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ---------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ---------------------------------------------------------------------- */
/*                     pbp    |	address    | Pointer to the port block info */
/*                            |	       	   |                                */
/*                     dlp    |	address    | Pointer to the datalink info   */
/*                            |	       	   |                                */
/*                     mbp    |	address    | Pointer to the MOP block info  */
/*                            |	       	   |                                */
/*                     mcp    |	address    | Pointer to the MOP counters    */
/*                            |	       	   |                                */
/*                     enp    |	address    | Pointer to environment variables */
/*:      ---------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
struct NI_GBL {
    int *pbp;                           /*Pointer to the port block info    */
    int *dlp;                           /*Pointer to the datalink info      */
    int *mbp;                           /*Pointer to the MOP block info     */
    int *mcp;                           /*Pointer to the MOP counters       */
    int *lpp;                           /*Pointer to device loopback info   */
    int *enp;                           /*Pointer to environment variables  */
    } ;
struct DDB {
    char *name;                         /* how this device wants to be called */
    int (*read)();                      /* read routine                     */
    int (*write)();                     /* write routine                    */
    int (*open)();                      /* open routine                     */
    int (*close)();                     /* close routine                    */
    int (*expand)();                    /* expand routine                   */
    int (*delete)();                    /* delete routine                   */
    int (*create)();                    /* address of create routine        */
    int (*setmode)();                   /* sets interrupt/polled mode       */
    int (*validation)();                /* validation routine               */
    int misc;                           /* class specific use               */
    unsigned allow_info : 1;
    unsigned allow_next : 1;
    unsigned flash : 1;                 /* is a flash update driver         */
    unsigned block : 1;                 /* is a block device                */
    unsigned sequential : 1;            /* can't be fseek'd                 */
    unsigned net_device : 2;            /* is a network device              */
    unsigned filesystem : 1;            /* is a filesystem                  */
    } ;
#define DDB$K_INTERRUPT 0               /* set to interrupt mode            */
#define DDB$K_POLLED 1                  /* set to polled mode               */
#define DDB$K_STOP 2                    /* set to stopped                   */
#define DDB$K_START 3                   /* set to start                     */
#define DDB$K_ASSIGN 4                  /* assign controller letters        */
#define DDB$K_LOOPBACK_INTERNAL 5       /* device internal loopback         */
#define DDB$K_LOOPBACK_EXTERNAL 6       /* device external loopback         */
#define DDB$K_NOLOOPBACK 7              /* no loopback                      */
#define DDB$K_OPEN 8                    /* file open                        */
#define DDB$K_CLOSE 9                   /* file close                       */
#define DDB$K_READY 10                  /* get ready to boot                */
#define DDB$K_MULTICAST 11              /* enable multicast                 */
#define DDB$K_ETHERNET 1                /* Ethernet device                  */
#define DDB$K_FDDI 2                    /* FDDI device                      */
#define DDB$K_TOKEN 3                   /* TOKEN RING                       */
/*+                                                                         */
/* ===============================================                          */
/* = INODE - file entry structure 		 =                          */
/* ===============================================                          */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* Inodes are allocated from the ilist array.                               */
/* Each inode points to a file. This file may be a read only file, such as a */
/* script, a write only file such as a log, an executable, such as a        */
/* routine which executes a command, etc. The inode also points into the    */
/* driver database to a set of routines, associated with a specific driver, */
/* that may perform actions on the file.                                    */
/*                                                                          */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*   ------------------------------------------------------------------------- */
/*   Structure Members | Data Type  | Description                           */
/*   ------------------------------------------------------------------------- */
/*   dvx           | longword   | index into the driver database            */
/*                 |            |                                           */
/*   dev           | longword   | address of device structure               */
/*                 |            |                                           */
/*   inuse         | longword   | set if node is in use                     */
/*                 |	        |                                           */
/*   name          | string     | name of file                              */
/*                 |	        |                                           */
/*   attr          | longword   | file attributes (read,write,execute)      */
/*                 |	        |                                           */
/*   loc           | address    | address of file                           */
/*                 |	        |                                           */
/*   len           | quadword   | size of file in bytes                     */
/*                 |	        |                                           */
/*   alloc         | quadword   | size allocated                            */
/*                 |	        |                                           */
/*   ref           | longword   | open reference count  (for drivers)       */
/*                 |	        |                                           */
/*   append_offset | quadword   | + loc = addr to append next block of data to */
/*                 |	        |                                           */
/*   misc          | address    | miscellaneous pointer                     */
/*                 |	        |                                           */
/*:  ------------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
struct INODE {
    struct INODE *flink;                /* singly linked list of inodes     */
    struct DDB *dva;                    /* pointer to driver dispatch table */
    int *dev;                           /* pointer to device descriptor     */
    int inuse;                          /* set if node is in use            */
    int attr;                           /* attribute mask                   */
    char *loc;                          /* first byte of file               */
    int len [2];                        /* actual size in bytes.            */
    int alloc [2];                      /* size allocated                   */
    int append_offset [2];              /* + inode.loc = addr to append next data to */
    int ref;                            /* open reference count (for drivers) */
    int *misc;                          /* miscellaneous pointer            */
    char name [32];
    int bs;                             /* natural block size of the device, normally 0 */
    } ;
#define ATTR$M_READ 1                   /* for compatability                */
#define ATTR$M_WRITE 2                  /* writes (and deletes) allowed     */
#define ATTR$M_EXECUTE 4                /* executable file                  */
#define ATTR$M_BINARY 8                 /* binary file                      */
#define ATTR$M_EXPAND 16                /* shell expands wildcards          */
#define ATTR$M_SECURE 32                /* access allowed in secure mode    */
#define ATTR$M_NOFREE 64                /* don't free file space            */
#define ATTR$M_DEVICE 128               /* device                           */
#define ATTR$M_ADAPTER 256              /* adapter                          */
#define ATTR$M_BUS 512                  /* bus                              */
#define ATTR$M_FILE 1024                /* file                             */
#define DEF_PERMISSION 3
#define DEF_PUSHBACK 8                  /* max amount of pushback           */
#define DEF_RBUF 2048                   /* max size of read ahead           */
/*+                                                                              */
/* ===============================================                               */
/* = RAB - Read Ahead Buffer                     =                               */
/* ===============================================                               */
/*                                                                               */
/* STRUCTURE OVERVIEW:                                                           */
/*                                                                               */
/* Drivers that do not supports FSEEKs require buffers, so that                  */
/* functions such as ungetc can work in a transparent manner.  Define a          */
/* generic read ahead buffer, and allow for a predetermined amount of            */
/* pushback.  The buffer is implemented as a circular queue.  Because            */
/* these buffers are process specific, no synchronization is required.           */
/*                                                                               */
/*                                                                               */
/* STRUCTURE MEMBERS:                                                            */
/*                                                                          */
/*:                                                                              */
/*       ----------------------------------------------------------------------  */
/*       Structure Members    | Data Type  | Description                         */
/*       ----------------------------------------------------------------------  */
/*                     alloc  | longword   | number of bytes that are allocated  */
/*                            |            |                                */
/*                     len    | longword   | number of bytes in the buffer       */
/*                            |            |                                */
/*                     buf    | address    | pointer to buffer                   */
/*                            |            |                                */
/*                     inx    | longword   | input index                         */
/*                            |            |                                */
/*                     outx   | longword   | output index                        */
/*:      ----------------------------------------------------------------------  */
/*                                                                               */
/*-                                                                             */
/*                                                                               */
struct RAB {
    unsigned int len;                   /*                                     */
    unsigned int alloc;                 /*                                     */
    char *buf;                          /*                                     */
    unsigned int inx;                   /*                                     */
    unsigned int outx;                  /*                                     */
    } ;
/*+                                                                              */
/* ===============================================                               */
/* = IOB  - IO block structure                   =                               */
/* ===============================================                               */
/*                                                                               */
/* STRUCTURE OVERVIEW:                                                           */
/*                                                                          */
/* Contains IO count data.  Every file descriptor and driver port block     */
/* has an IOB.  All IOBs live on a linked list, iobq, synchronized by the   */
/* spinlock, spl_iobq.  IOBs are created by fopen and driver init routines  */
/* and they may be created by diagnostics or any other code.                */
/* IOBs are deleted by fclose and drivers or other code.                    */
/*                                                                          */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                            */
/*                                                                          */
/*:                                                                              */
/*  -----------------------------------------------------------------------  */
/*    Members    | Data Type  | Description                                 */
/*  -----------------------------------------------------------------------  */
/*		       sysq   |	queue	   | system wide IOB queue          */
/*                            |            |                                */
/*		       pcbq   |	queue      | PCB wide q; all IOBs for 1 PCB */
/*                            |            |                                */
/*		      name    | string     | name string for this IO block  */
/*                            |            |                                */
/*        	      pcb     | address    | address of pcb                 */
/*               	      |            |                                */
/*		hard_errors   | longword   | number of hard errors reported */
/*                            |            |                                */
/*		soft_errors   | longword   | number of soft errors reported */
/*                            |            |                                */
/*		bytes_read    | quadword   | number of bytes read           */
/*                            |            |                                */
/*		bytes_written | quadword   | number of bytes written        */
/*                            |            |                                */
/*		io_count      | quadword   | number of I/O operations done  */
/*                            |            |                                */
/*		retry_count   | longword   | number of retries performed    */
/*: -----------------------------------------------------------------------  */
/*                                                                               */
/*-                                                                             */
/*                                                                               */
struct IOB {
    struct QUEUE sysq;                  /* system wide IOB q                */
    struct QUEUE pcbq;                  /* pcb q; all IOBs for 1 pcb        */
    int bytes_read [2];
    int bytes_written [2];
    int io_count [2];
    char name [32];
    int *pcb;                           /* addr of pcb                      */
    unsigned int hard_errors;
    unsigned int soft_errors;
    unsigned int retry_count;
    } ;
/*+                                                                              */
/* ===============================================                               */
/* = File - File descriptor structure            =                               */
/* ===============================================                               */
/*                                                                               */
/* STRUCTURE OVERVIEW:                                                           */
/*                                                                          */
/* When accessing files, a small integer is passed to the routines.  This        */
/* integer is an index into an array of pointers to file descriptors.  Each      */
/* PCB has a prebuilt array of file descriptors.                                 */
/*                                                                          */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                            */
/*                                                                          */
/*:                                                                              */
/*  -----------------------------------------------------------------------  */
/*    Members    | Data Type  | Description                                 */
/*  -----------------------------------------------------------------------  */
/*  	  ip     | address    | address of inode                            */
/*               |            |                                             */
/*  local_offset | quadword   | current location in file                    */
/*               |            |                                             */
/*        offset | address    | pointer to current location in file         */
/*               |            |                                             */
/*        mode   | longword   | access mode                                 */
/*               |            |                                             */
/*        rap    | longword   | address of read ahead buffer, 0 if not used */
/*               |            |                                             */
/*        status | longword   | status of last operation                    */
/*               |            |                                             */
/*        count  | longword   | count of last operation                     */
/*               |            |                                             */
/*   code_entry  | longword   | code entry point in file                    */
/*               |            |                                             */
/*        iob    | IOB        | IOB; contains IO counts                     */
/*: -----------------------------------------------------------------------  */
/*                                                                               */
/*-                                                                             */
/*                                                                               */
struct FILE {
    struct FILE *flink;                 /* forward link                     */
    struct FILE *blink;                 /* backward link                    */
    int local_offset [2];               /* current location in file         */
    struct IOB iob;                     /* IOB; contains IO counts;keep on qwd */
    struct INODE *ip;                   /* address of inode                 */
    int *offset;                        /* pointer to current location in file */
    int mode;                           /* access mode                      */
    struct RAB *rap;                    /* addr of read ahead buf, 0 if not used */
    int status;                         /* status of last operation         */
    int count;                          /* count of last operation          */
    int code_entry;                     /* code entry point in file         */
    int misc;                           /* miscellaneous field              */
    int pushback;                       /* pushback value                   */
    int pushed;                         /* is value valid                   */
    char quad_fill2 [8];
    } ;
/* access modes; more description in ANSI C standard, fopen                 */
/*                                                                          */
/* Access mode attributes are combined (ORed) to create a file descr mode.  */
/* Rewrite access mode attribute means to discard previous file contents if */
/* the file existed before the fopen.					    */
/*                                                                          */
/* access mode     access mode attributes                                   */
/*     "r"         read                                                     */
/*     "w"         write, rewrite, create                                   */
/*     "a"         write, append, create                                     */
/*     "r+"        read, write                                              */
/*     "w+"        read, write, rewrite, create                             */
/*     "a+"        read, write, append, create                               */
/*     "f"         fixed, file's memory address will not be moved           */
/*     "z"         use memzone if append to above access modes              */
/*                                                                          */
/* access mode attributes:                                                  */
#define MODE_M_READ 1                   /* read, same as inode read attrib  */
#define MODE_M_WRITE 2                  /* write,same as inode write attrib */
#define MODE_M_APPEND 16                /* append                           */
#define MODE_M_REWRITE 32               /* rewrite, discard old file        */
#define MODE_M_CREATE 64                /* creation allowed                 */
#define MODE_M_SILENT 128               /* error message disabled           */
#define MODE_M_PERM 256                 /* file is permanent (fp is malloc'd) */
#define MODE_M_SECURE 512               /* access allowed in secure mode    */
#define MODE_M_IMMEDIATE 1024           /* complete immediately, don't block */
#define MODE_M_MZONE 2048               /* use memzone                      */
/*+                                                                         */
/* ===============================================                          */
/* = TIMERQ - Queue of sleeping processes        =                          */
/* ===============================================                          */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* The timer queue holds those processes that have put themselves           */
/* to sleep. Each time a timer interrupt is posted on the primary processor, */
/* the timer task walks down the timer queue awakening those                */
/* processes that have finished sleeping.                                   */
/*                                                                          */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ---------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ---------------------------------------------------------------------- */
/*                     flink  |	address    | forward link                   */
/*                            |            |                                */
/*                     blink  |	address    | backward link                  */
/*                            |            |                                */
/*                     msec   |	longword   | time to sleep                  */
/*                            |            |                                */
/*                     sem    |	semaphore  | semaphore the sleeper waits on */
/*                            |            |                                */
/*                     active |	longword   | active flag (TRUE = active)    */
/*                            |            |                                */
/*                     perm   |	longword   | permanent flag (TRUE = permanent) */
/*                            |            |                                */
/*                     alt    |	longword   | alternate flag (TRUE = alternate) */
/*:      ---------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
struct TIMERQ {
    struct TIMERQ *flink;
    struct TIMERQ *blink;
    int msec;
    struct SEMAPHORE sem;
    int active;
    int perm;
    int alt;
    } ;
/*+                                                                         */
/* =======================================================                  */
/* = DELAYYQ - Queue of processes awaiting creation      =                  */
/* =======================================================                  */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* This queue contains processes whose creation has been delayed.           */
/* After a delay interval is complete, each process will be created.        */
/*                                                                          */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ---------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ---------------------------------------------------------------------- */
/*                     flink  |	address    | forward link                   */
/*                            |            |                                */
/*                     blink  |	address    | backward link                  */
/*                            |            |                                */
/*                     msec   |	longword   | number of milliseconds to delay */
/*                            |            |                                */
/*               use_tt_not_nl|	longword   | flag to use "tt", not "nl"     */
/*                            |            |                                */
/*                     code   |	address    | code to run                    */
/*                            |            |                                */
/*                     startup|	address    | process startup routine        */
/*                            |            |                                */
/*                     sem    |	SEMAPHORE  | pointer to completion semaphore */
/*                            |            |                                */
/*                     pri    |	longword   | process priority               */
/*                            |            |                                */
/*                    affinity|	longword   | process affinity               */
/*                            |            |                                */
/*                     stack  |	longword   | size of process stack          */
/*                            |            |                                */
/*                     name   |	array      | process name                   */
/*                            |            |                                */
/*                     a0     |	longword   | first argument                 */
/*                            |            |                                */
/*                     a1     |	longword   | second argument                */
/*                            |            |                                */
/*                     a2     |	longword   | third argument                 */
/*                            |            |                                */
/*                     a3     |	longword   | fourth argument                */
/*:      ---------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
struct DELAYQ {
    struct DELAYQ *flink;
    struct DELAYQ *blink;
    int msec;
    int use_tt_not_nl;
    int (*code)();
    int (*startup)();
    struct SEMAPHORE *sem;
    int pri;
    int affinity;
    int stack;
    char name [32];
    int a0;
    int a1;
    int a2;
    int a3;
    } ;
/*+                                                                         */
/* ===============================================                          */
/* = POLLQ - Queue of poll routines              =                          */
/* ===============================================                          */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* This queue contains all the active poll routines.  On every timer tick,  */
/* the timer process walks down this queue calling the routines.  Drivers   */
/* register/deregister their poll routines in this queue as a result of a   */
/* call to xx_setmode.                                                      */
/*                                                                          */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ---------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ---------------------------------------------------------------------- */
/*                     flink  |	address    | forward link                   */
/*                            |            |                                */
/*                     blink  |	address    | backward link                  */
/*                            |            |                                */
/*                     routine|	address    | poll routine to call           */
/*                            |            |                                */
/*                     param  |	longword   | parameter to pass to routine   */
/*:      ---------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
struct POLLQ {
    struct POLLQ *flink;
    struct POLLQ *blink;
    int (*routine)();
    int param;
    } ;
/*+                                                                         */
/* ===============================================                          */
/* = PBQ - Queue of port blocks                  =                          */
/* ===============================================                          */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* This queue contains generic pointers to a list of port blocks.           */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ---------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ---------------------------------------------------------------------- */
/*                     flink  |	address    | forward link                   */
/*                            |            |                                */
/*                     blink  |	address    | backward link                  */
/*                            |            |                                */
/*                     pb     |	address    | Address of a port block.       */
/*:      ---------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
struct PBQ {
    struct PBQ *flink;
    struct PBQ *blink;
    int *pb;
    } ;
/*+                                                                         */
/* ==============================================================           */
/* = DIAG_DIS_TABLE - diagnostic dispatch table                 =           */
/* ==============================================================           */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* The diagnostic dispatch table contains the information necessary to      */
/* dispatch to tests.  A diagnostic program should define and initialize    */
/* an array of DIAG_DIS_TABLE to create the dispatch table.  Each entry in  */
/* the array will describe a single test.                                   */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*   --------------------------------------------------------------------   */
/*   Structure Member   | Data Type     | Description                       */
/*   --------------------------------------------------------------------   */
/*   routine            | address       | address of test routine           */
/*                      |               |                                   */
/*   groups             | longword      | name of groups to which this test */
/*                      |               | belongs                           */
/*:  --------------------------------------------------------------------   */
/*-                                                                         */
/*                                                                          */
struct DIAG_DIS_TABLE {
    int (*routine)();
    unsigned int groups;
    } ;
/*+                                                                         */
/* ==============================================================           */
/* = DIAG_EVS - diagnostic environment variable states          =           */
/* ==============================================================           */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* The diagnostic evs structure consists of bits used to represent the      */
/* current state of the global diagnostic environment variables.  These     */
/* bits should not be modified by the diagnostic program, but can be read   */
/* to determine the current state of the environment variables.             */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*   --------------------------------------------------------------------   */
/*   Structure Member   | Data Type     | Description                       */
/*   --------------------------------------------------------------------   */
/*   bell               | bitfield      | set if D_BELL is ON               */
/*                      |               |                                   */
/*   clean              | bitfield      | set if D_CLEANUP is ON            */
/*                      |               |                                   */
/*   comp               | bitfield      | set if D_COMPLETE is ON           */
/*                      |               |                                   */
/*   eop                | bitfield      | set if D_EOP is ON                */
/*                      |               |                                   */
/*   head               | bitfield      | set if D_STARTUP is ON            */
/*                      |               |                                   */
/*   log                | bitfield      | set if D_LOGERR is ON             */
/*                      |               |                                   */
/*   oper               | bitfield      | set if D_OPER is ON               */
/*                      |               |                                   */
/*   quick              | bitfield      | set if D_QUICK is ON              */
/*                      |               |                                   */
/*   status             | bitfield      | set if D_STATUS is ON             */
/*                      |               |                                   */
/*   tt                 | bitfield      | set if D_TRACE is ON              */
/*                      |               |                                   */
/*   halt_hard          | bitfield      | set if D_HARDERR is HALT          */
/*                      |               |                                   */
/*   loop_hard          | bitfield      | set if D_HARDERR is LOOP          */
/*                      |               |                                   */
/*   halt_soft          | bitfield      | set if D_SOFTERR is HALT          */
/*                      |               |                                   */
/*   loop_soft          | bitfield      | set if D_SOFTERR is LOOP          */
/*                      |               |                                   */
/*   rpt_summ           | bitfield      | set if D_REPORT is SUMMARY        */
/*                      |               |                                   */
/*   rpt_full           | bitfield      | set if D_REPORT is FULL           */
/*                      |               |                                   */
/*   rpt_off            | bitfield      | set if D_REPORT is OFF            */
/*:  --------------------------------------------------------------------   */
/*-                                                                         */
/*                                                                          */
#define m_bell 0x1
#define m_clean 0x2
#define m_comp 0x4
#define m_eop 0x8
#define m_start 0x10
#define m_logs 0x20
#define m_logh 0x40
#define m_oper 0x80
#define m_quick 0x100
#define m_status 0x200
#define m_tt 0x400
#define m_halt_hard 0x800
#define m_loop_hard 0x1000
#define m_halt_soft 0x2000
#define m_loop_soft 0x4000
#define m_rpt_summ 0x8000
#define m_rpt_full 0x10000
#define m_rpt_off 0x20000
struct DIAG_EVS {
    unsigned bell : 1;                  /* D_BELL state                     */
    unsigned clean : 1;                 /* D_CLEANUP state                  */
    unsigned comp : 1;                  /* D_COMPLETE state                 */
    unsigned eop : 1;                   /* D_EOP state                      */
    unsigned start : 1;                 /* D_STARTUP state                  */
    unsigned logs : 1;                  /* D_LOGSERR state                  */
    unsigned logh : 1;                  /* D_LOGHERR state                  */
    unsigned oper : 1;                  /* D_OPER state                     */
    unsigned quick : 1;                 /* D_QUICK state                    */
    unsigned status : 1;                /* D_STATUS state                   */
    unsigned tt : 1;                    /* D_TRACE state                    */
    unsigned halt_hard : 1;             /* D_HARDERR state                  */
    unsigned loop_hard : 1;             /* D_HARDERR state                  */
    unsigned halt_soft : 1;             /* D_SOFTERR state                  */
    unsigned loop_soft : 1;             /* D_SOFTERR state                  */
    unsigned rpt_summ : 1;              /* D_REPORT state                   */
    unsigned rpt_full : 1;              /* D_REPORT state                   */
    unsigned rpt_off : 1;               /* D_REPORT state                   */
    unsigned DIAG_EVS$$V_FILL_0 : 6;
    } ;
struct ALPHA_HW_PCB {
    int alpha_hw_pcb$q_r2 [2];
    int alpha_hw_pcb$q_r3 [2];
    int alpha_hw_pcb$q_r4 [2];
    int alpha_hw_pcb$q_r5 [2];
    int alpha_hw_pcb$q_r6 [2];
    int alpha_hw_pcb$q_r7 [2];
    int alpha_hw_pcb$q_r8 [2];
    int alpha_hw_pcb$q_r9 [2];
    int alpha_hw_pcb$q_r10 [2];
    int alpha_hw_pcb$q_r11 [2];
    int alpha_hw_pcb$q_r12 [2];
    int alpha_hw_pcb$q_r13 [2];
    int alpha_hw_pcb$q_r14 [2];
    int alpha_hw_pcb$q_r15 [2];
    int alpha_hw_pcb$q_r26 [2];
    int alpha_hw_pcb$q_r27 [2];
    int alpha_hw_pcb$q_r29 [2];
    int alpha_hw_pcb$q_r30 [2];
    } ;
struct ALPHA_CTX {
    int acx$q_acp [2];
    int acx$q_exc_r2 [2];
    int acx$q_exc_r3 [2];
    int acx$q_exc_r4 [2];
    int acx$q_exc_r5 [2];
    int acx$q_exc_r6 [2];
    int acx$q_exc_r7 [2];
    int acx$q_r8 [2];
    int acx$q_r9 [2];
    int acx$q_r10 [2];
    int acx$q_r11 [2];
    int acx$q_r12 [2];
    int acx$q_r13 [2];
    int acx$q_r14 [2];
    int acx$q_r15 [2];
    int acx$q_r18 [2];
    int acx$q_r19 [2];
    int acx$q_r20 [2];
    int acx$q_r21 [2];
    int acx$q_r22 [2];
    int acx$q_r23 [2];
    int acx$q_r24 [2];
    int acx$q_r25 [2];
    int acx$q_r26 [2];
    int acx$q_r27 [2];
    int acx$q_r28 [2];
    int acx$q_r29 [2];
    int acx$q_r30 [2];
    int acx$q_r0 [2];
    int acx$q_r1 [2];
    int acx$q_r16 [2];
    int acx$q_r17 [2];
    int acx$q_r2 [2];                   /* this part is delivered by PAL    */
    int acx$q_r3 [2];
    int acx$q_r4 [2];
    int acx$q_r5 [2];
    int acx$q_r6 [2];
    int acx$q_r7 [2];
    int acx$q_pc [2];
    int acx$q_ps [2];
    } ;
/*+                                                                         */
/* ===============================================                          */
/* = PCB - Process Context Block                 =                          */
/* ===============================================                          */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* Process context.                                                         */
/*                                                                          */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ---------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ---------------------------------------------------------------------- */
/*		flink 	      | address    | forward link                   */
/*                            |            |                                */
/*	        blink 	      | address    | backward link                  */
/*                            |            |                                */
/*	        alpha_hw_pcb  | ALPHA_HW_PCB | Alpha hardware context       */
/*                            |            |                                */
/*	        semp          | address    | semaphore the process may be waiting on */
/*                            |            |                                */
/*		ctrlcq        |	queue	   | control C queue                */
/*                            |            |                                */
/*		pq            |	queue	   | PCB queue                      */
/*                            |            |                                */
/*		dq            |	queue      | dynamic memory ownership queue */
/*                            |            |                                */
/*		hq            |	queue      | handler queue                  */
/*                            |            |                                */
/*		sjq           |	queue      | setjmp queue                   */
/*                            |            |                                */
/*		iobq          |	queue      | IO block queue for this PCB    */
/*                            |            |                                */
/*              pstate        | longword   | process state                  */
/*                            |            |                                */
/*		shellp	      | longword   | address of shell's context if this */
/*                            |            | process is a shell.            */
/*                            |            |                                */
/*		killpending   | longword   | bit requesting that process exit */
/*                            |            |                                */
/*		cmd_mode      | longword   | command mode                   */
/*                            |            |                                */
/*		cmd_switch    | longword   | command mode switch            */
/*                            |            |                                */
/*		stdin_name    | address    | filename addr for standard input */
/*                            |            |                                */
/*		stdout_name   | address    | filename addr for standard output */
/*                            |            |                                */
/*		stderr_name   | address    | filename addr for standard error */
/*                            |            |                                */
/*		stdin	      | address    | address of stdin  file descriptor */
/*                            |            |                                */
/*		stdout	      | address    | address of stdout file descriptor */
/*                            |            |                                */
/*		stderr	      | address    | address of stderr file descriptor */
/*                            |            |                                */
/*              qexpire       | longword   | number of ticks to go before the */
/*                            |            | quantum expires                */
/*                            |            |                                */
/*              qexpire_dec   | longword   | value by which qexpire is      */
/*                            |            | decremented on every timer tick. */
/*                            |            |                                */
/*              quantum       | longword   | number of timer ticks that this */
/*                            |            | process gets to run before being */
/*                            |            | bumped                         */
/*                            |            |                                */
/*              timestamp     | quadword   | value of timer when this process */
/*                            |            | was swapped in.                */
/*                            |            |                                */
/*		cputime	      | quadword   | number of nsec process was running */
/*                            |            |                                */
/*              exit_status   |	longword   | final exit status for the process */
/*                            |            |                                */
/*              affinity      |	longword   | cpu affinity mask              */
/*                            |            |                                */
/*            created_affinity|	longword   | cpu affinity mask at creation  */
/*                            |            |                                */
/*              parentpid     | longword   | process id of parent           */
/*                            |            |                                */
/*              pid           | longword   | process id                     */
/*                            |            |                                */
/*              last_processor| longword   | last processor this process ran on */
/*                            |            |                                */
/*		name	      | array      | process name                   */
/*                            |            |                                */
/*		startup	      | address    | user defined startup routine   */
/*                            |            |                                */
/*		task	      | address    | task address                   */
/*                            |            |                                */
/*		rundown	      |	address    | rundown routine                */
/*                            |            |                                */
/*	        completion    |	address    | posted by rundown routine      */
/*                            |            |                                */
/*	        opened        |	SEMAPHORE  | posted by krn$_process         */
/*                            |            |                                */
/*	        pri	      | longword   | 0 <= pri <= MAX_PRI            */
/*                            |            |                                */
/*	        acp           | ALPHA_CTX  | ptr to Alpha hardware context  */
/*                            |            | (on the stack)                 */
/*                            |            |                                */
/*		qualp	      | address    | address of qualifier list      */
/*			      |            |                                */
/*		dev_name      | byte array | name of target device          */
/*                            |            |                                */
/*		test_mask     | address    | array of test numbers          */
/*                            |            |                                */
/*		dispatch      | address    | address of diag dispatch table */
/*                            |            |                                */
/*		sup_dev       | address    | names of supported devices     */
/*                            |            |                                */
/*		globals       | address    | generic pointer                */
/*                            |            |                                */
/*		diag_common   | address    | address of common diagnostic   */
/*			      |            | structure variables            */
/*                            |            |                                */
/*		evs           | longword   | current state of global diag evs */
/*                            |            |                                */
/*		stacksize     |	longword   | size of stack in bytes         */
/*                            |            |                                */
/*		argc	      | longword   | argument count                 */
/*                            |            |                                */
/*	   	argv          | array      | argument list                  */
/*                            |            |                                */
/*		stack	      |	array      | process stack                  */
/*:      ---------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
#define SIGNAL_KILL 1                   /* bit assignments for              */
#define SIGNAL_CONTROLC 2               /* killpending field                */
struct PCB {
    struct PCB *pcb$a_flink;            /* forward link                     */
    struct PCB *pcb$a_blink;            /* backward link                    */
    struct ALPHA_HW_PCB pcb$r_alpha_hw_pcb; /* Alpha hardware context       */
    struct SEMAPHORE *pcb$a_semp;       /* back pointer to semaphore        */
    struct QUEUE pcb$r_ctrlcq;          /* Control C queue                  */
    struct QUEUE pcb$r_pq;              /* PCB queue                        */
    struct QUEUE pcb$r_dq;              /* dynamic memory ownership q       */
    struct QUEUE pcb$r_hq;              /* handler queue                    */
    struct QUEUE pcb$r_sjq;             /* setjmp queue                     */
    struct QUEUE pcb$r_iobq;            /* IO block queue for this pcb      */
    int pcb$l_pstate;                   /* process state                    */
    int pcb$l_shellp;                   /* address of shell's context       */
    int pcb$l_killpending;              /* please exit...                   */
    int pcb$l_cmd_mode;                 /* command mode                     */
    int pcb$l_cmd_switch;               /* command mode switch              */
    struct QUEUE pcb$r_fq;              /* file queue                       */
    char *pcb$a_stdin_name;
    char *pcb$a_stdout_name;
    char *pcb$a_stderr_name;
    char pcb$b_stdin_mode [8];          /* access modes for                 */
    char pcb$b_stdout_mode [8];         /* standard channels                */
    char pcb$b_stderr_mode [8];
    struct FILE *pcb$a_stdin;           /* addr of stdin file descr         */
    struct FILE *pcb$a_stdout;          /* addr of stdout file descr        */
    struct FILE *pcb$a_stderr;          /* addr of stderr file descr        */
    char pcb$b_quad_fill0 [8];
    int pcb$q_timestamp [2];
    int pcb$q_cputime [2];              /* number of nsec process was running */
    int pcb$l_qexpire;
    int pcb$l_qexpire_dec;
    int pcb$l_quantum;
    int pcb$l_exit_status;              /* final status code                */
    int pcb$l_affinity;
    int pcb$l_created_affinity;
    int pcb$l_parentpid;                /* parent's pid                     */
    int pcb$l_pid;
    int pcb$l_last_processor;
    char pcb$b_name [32];
    int (*pcb$a_startup)();             /* user defined startup             */
    int (*pcb$a_task)();                /* task address                     */
    int (*pcb$a_rundown)();             /* rundown routine                  */
    struct SEMAPHORE *pcb$a_completion; /* posted by rundown routine        */
    struct SEMAPHORE *pcb$a_opened;     /* posted by krn$_process           */
    int pcb$l_pri;                      /* 0 <= pri <= MAX_PRI              */
    struct ALPHA_CTX *pcb$a_acp;        /* context is on stack              */
    int *pcb$a_qualp;
    char pcb$b_dev_name [32];
    unsigned char *pcb$a_test_mask;
    struct DIAG_DIS_TABLE *pcb$a_dis_table;
    char *pcb$a_sup_dev;
    int *pcb$a_globals;
    int *pcb$a_diag_common;
    int pcb$l_stacksize;                /* # bytes in stack                 */
    int pcb$l_argc;
    int pcb$l_argv [4];                 /* keep this first                  */
    char pcb$b_quad_fill2 [8];
    char pcb$b_stack [8];               /* this must be last                */
    } ;
/*                                                                          */
/* Process states                                                           */
/*                                                                          */
#define KRN$K_NULL 1
#define KRN$K_WAIT 2
#define KRN$K_READY 3
#define KRN$K_RUNNING 4
/*                                                                          */
/* Error Types                                                              */
/*                                                                          */
#define DIAG$K_HARD 1
#define DIAG$K_SOFT 2
#define DIAG$K_FATAL 3
/*+                                                                         */
/* ============================================================================ */
/* = LOCK - spinlock data structures                                          = */
/* ============================================================================ */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/*      This data structure defines a spinlock.                             */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ---------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ---------------------------------------------------------------------- */
/*                      lock  |  longword  | contents of the actual lock    */
/*                            |            |                                */
/*                   req_ipl  |  longword  | IPL that spinlock must run at  */
/*                            |            |                                */
/*                   sav_ipl  |  longword  | IPl previous to spinlock, and the */
/*                            |            | IPL that will be restored.     */
/*                            |            |                                */
/*                  attempts  |  longword  | number of times this spinlock has been */
/*                            |            | taken                          */
/*                            |            |                                */
/*                   retries  |  longword  | number of times the lock has failed */
/*                            |            |                                */
/*                     owner  |  longword  | the PID of the current owner   */
/*                            |            |                                */
/*                     count  |  longword  | number of concurrent locks     */
/*:      ---------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
struct LOCK {
    int value;
    int req_ipl;
    int sav_ipl;
    int attempts;
    int retries;
    int owner;
    int count;
    int next_number;
    int now_serving;
    int pad [7];                        /* Ensure 64byte alignment          */
    } ;
/*+                                                                         */
/* ===============================================                          */
/* = HQE - Handler queue entry			 =                          */
/* ===============================================                          */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* Handler queue entry                                                      */
/*                                                                          */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ---------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ---------------------------------------------------------------------- */
/*                     flink  |	address    | forward link                   */
/*                            |            |                                */
/*                     blink  |	address    | backward link                  */
/*                            |            |                                */
/*                     vector |	longword   | SCB index                      */
/*                            |            |                                */
/*                     handler|	address    | address of handler             */
/*                            |            |                                */
/*                     param  |	longword   | SCB parameter                  */
/*:      ---------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
struct HQE {
    struct HQE *flink;
    struct HQE *blink;
    int vector;
    int (*handler)();
    int param;
    } ;
/*+                                                                         */
/* ==============================================                           */
/* = ASJD - Alpha SetJmp Data			=                           */
/* ==============================================                           */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* The Alpha SetJmp Data structure holds sufficient information to implement setjmp/longjmp on an Alpha machine, */
/* given an execution environment which complies with the Alpha standard calling conventions. */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ------------------------------------------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ------------------------------------------------------------------------------------------------------- */
/*                     sp     | quadword   | SP after space is reserved for ASJD structure on the stack in setjmp. */
/*                            |            |                                */
/*                     rn     | quadword   | Rn at time of setjmp call (n=1:15, 26, 27, 29). */
/*                            |            |                                */
/*                     fn     | quadword   | Fn at time of setjmp call (n=2:9). */
/*:      ------------------------------------------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
struct ASJD {
    int ASJD$q_sp [2];
    int ASJD$q_r1 [2];
    int ASJD$q_r2 [2];
    int ASJD$q_r3 [2];
    int ASJD$q_r4 [2];
    int ASJD$q_r5 [2];
    int ASJD$q_r6 [2];
    int ASJD$q_r7 [2];
    int ASJD$q_r8 [2];
    int ASJD$q_r9 [2];
    int ASJD$q_r10 [2];
    int ASJD$q_r11 [2];
    int ASJD$q_r12 [2];
    int ASJD$q_r13 [2];
    int ASJD$q_r14 [2];
    int ASJD$q_r15 [2];
    int ASJD$q_r26 [2];
    int ASJD$q_r27 [2];
    int ASJD$q_r29 [2];
    } ;
/*+                                                                         */
/* ==============================================                           */
/* = SJD_UNION - SetJmp Data Union		=                           */
/* ==============================================                           */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* The SetJmp Data union is used to handle Alpha setjmp context data.  It is encapsulted in the SJQ */
/* structure.                                                               */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ------------------------------------------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ------------------------------------------------------------------------------------------------------- */
/*                     alpha  | ASJD       | Alpha setjmp data block.       */
/*:      ------------------------------------------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
union sjd_union {
    struct ASJD asjd;
    } ;
/*+                                                                         */
/* ==============================================                           */
/* = SJQ - SetJmp Queue				=                           */
/* ==============================================                           */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/* The SetJmp Queue is a per-process queue which holds the environment data required by setjmp/longjmp for the */
/* process.                                                                 */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ------------------------------------------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ------------------------------------------------------------------------------------------------------- */
/*                     flink  |	address    | forward link                   */
/*                            |            |                                */
/*                     blink  |	address    | backward link                  */
/*                            |            |                                */
/*                     envid  | longword   | environment identifier         */
/*                            |            |                                */
/*		       sjd    | SJD        | setjmp environment data        */
/*:      ------------------------------------------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
struct SJQ {
    struct SJQ *flink;                  /* Forward link                     */
    struct SJQ *blink;                  /* Backward link                    */
    union sjd_union sjd;                /* setjmp environment data          */
    int envid;                          /* environment ID                   */
    } ;
typedef unsigned int jmp_buf;           /* Define jmp_buf type as longint.  */
#define PROC_KILL_SJ_ID 0               /* Process kill setjmp id.          */
#define EXDEP_SJ_ID 1                   /* Examine/deposit setjmp id.       */
#define MEMTEST_SJ_ID 2                 /* Memtest setjmp id.               */
 
