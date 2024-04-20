/* file:	vga_bios_driver.c
 *
 * Copyright (C) 1992 by
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
 * FACILITY:
 *
 *      Alpha Console
 *
 * ABSTRACT:	Simple VGA routines
 *
 *	This code assumes that the monitor is a VGA (analog) monitor.
 *	The Code uses the X86 emulator to provide the Init and Setmode
 *      functions.
 *
 * AUTHORS:
 *
 *	David Baird
 *
 * CREATION DATE:
 *  
 *	07-Sep-1993	
 *
 * MODIFICATION HISTORY:
 *
 *     jmk  09-Mar-2000     Support VGA on any hose (not just 0)
 *
 *     gdm  03-Feb-1998     Conditionalize for DMCC Eiger
 *
 *     ska  11-Nov-1998     Conditionalize for YukonA
 *
 *	er  14-Feb-1996	    No longer need to raise IPL before calling
 *			    StartBiosEmulator.  Added mb() calls to incfgx
 *			    routines in EB164_IO.C to fix problem.
 *
 *	er  13-Dec-1995	    EB164 - raise IPL before calling StartBiosEmulator
 *
 *	al   1-nov-1995     Make driver put out remapped characters.
 *                          In vga_paputchar, if char is greater than 128,
 *                          then it should be remapped to what is specified
 *                          in char_lookup_table.  This was happening, but
 *                          the remapped value was not being used for output
 *                          to screen, the original value was.
 *
 *	dtr  24-sep-1995    Backed out changes for graphics console
 *
 *	dtr  24-jul-1995   removed conditionals for console determination
 *	  
 *	jfr   7-Jun-1995    ALCOR raise ipl before calling StartBiosEmulator
 *
 *	noh   19-May-1995   Added "quick_start" environment variable for
 *			    AVMIN.  This skips memory testing and does 
 *			    not start the VGA bios emulator.
 *
 *	hcb   03-may-1995   initialize reusememory variable so it's not stale
 *			    on an init.
 *
 *	dtr	26-apr-1995 added reusememory variable to be passed into
 *			    emulator.
 *
 *	adm   31-Mar-195    updated vga_ttpb due to change in TTPB structure
 *
 *	rbb   13-Mar-1995   Conditionalize for EB164
 *
 *	er    27-Dec-1994   Conditionalize for EB66
 *
 *	dwb   04-Nov-1994   Save and restore the screen data on a vga_reinit to
 *			    not cause a vga screen clear on conole re-entry
 *
 *	dwb   13-Oct-1994   Change the vga_bios_init return code to always
 *                          return success to prevent console error messages
 *
 *	dtr   30-Aug-1994   Setup a return code from the emulator so that we can
 *			    determine the bus type.  The code will get back ffff
 *			    if no bus or isa bus.
 *			    
 *	dwb   03-02-94	Initial Entry, taken from vgag_driver.c
 *
 *	dtr - 21-apr-1994 - Corrected the init code so that it no longer
 *	initializes the emulator each time hardware init is called.  The
 *	emulator init is now done only when the driver is setup.
 *
 *	dtr	12-may-1994 - added a lookup table for the iso-latin display
 *			      set.  Hopefully all vendors of vga board have
 *			      their fonts mapped the same.
 *
 *	dwb	08-jul-1994 - add a erase screen to vga_reinit
 *
 *	dtr	140Jul-1994 - Added pb_def.h because vgapb had a new pb added
 *			      
 *	er	15-Aug-1994 - Conditionalize for SD164
 *
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:common.h"
#include "cp$src:msg_def.h"
#include "cp$src:tt_def.h"
#include "cp$src:ansi_def.h"
#include "cp$src:alpha_arc.h"
#include "cp$src:chstate.h"
#include "cp$src:display.h"
#include "cp$src:vgag.h"
#include "cp$src:pb_def.h"
#include "cp$src:vgapb.h"
#include "cp$inc:platform_io.h"
#include "cp$src:hwrpb_def.h"
#include "cp$src:mem_def.h"

#if SABLE || MIKASA || MEDULLA || CORTEX || UNIVERSE || LX164 || SX164 || DRIVERSHUT || REGATTA || WILDFIRE || MARVEL || PC264 || MONET || EIGER
#include "cp$src:probe_io_def.h"
#endif

#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#if !TURBO
extern void outportb (struct pb *pb,unsigned __int64 addr,unsigned char data);
extern void outportw (struct pb *pb,unsigned __int64 addr,unsigned short data);
extern void outmemb (struct pb *pb,unsigned __int64 addr,unsigned char data);
extern void outmemw (struct pb *pb,unsigned __int64 addr,unsigned short data);
extern unsigned char inmemb (struct pb *pb,unsigned __int64 addr);
extern unsigned short inmemw (struct pb *pb,unsigned __int64 addr);
#endif

#if MODULAR
extern struct  VGA_PB *vgapb;
extern struct TTPB *graphics_ttpbs;
extern int graphics_page_size;
extern int graphics_term_type;
extern int graphics_enabled;
extern int graphics_device;
extern int vg_controllers;
extern struct window_head config;
#endif

extern int display_ForegroundColor;
extern int display_BackgroundColor;
extern int display_HighIntensity;
extern int display_UnderScored;
extern int display_ReverseVideo;

#if DRIVERSHUT
extern match_controller();
#endif

#if RAWHIDE
extern struct device *iod_device[MAX_IOD_CNT];
#endif

#if GALAXY && RAWHIDE
extern int galaxy_partitions;
extern int galaxy_io_mask;
#endif

#if RAWHIDE || TURBO
struct pb vga_pb;
extern struct pb eisa_pb;
extern struct pb *vga_pb_ptr;
extern struct pb *eisa_pb_ptr;
struct device *vga_dev;     
struct device *eisa_dev;     
#define ETAPciHose 0
#else
struct pb *vga_pb_ptr = 0;
extern struct pb *eisa_pb_ptr;
extern int ETAPciHose;
#endif

int x86_running_flag = 0;

#if STARTSHUT || DRIVERSHUT
int vga_shutdown_flag = 1;
#else
int vga_shutdown_flag = 0;
#endif

#if SABLE || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER
extern struct HQE mchq_660;		     	/*MCHECK handler queue*/
int MachineCheckFlag;				/* flag for bios emulator */
#else
extern int ExpectMachineCheck;
#endif

#if RAWHIDE || TURBO 
extern struct HQE mchq_660;		     	/*MCHECK handler queue*/
extern int MachineCheckFlag;			/* flag for bios emulator */
struct HQE *mchq = &mchq_660;			/* help with compiler bug */
#endif

#if SABLE || RAWHIDE || TURBO || CORELLE
extern int eisa_bus_reset;
#endif

#if IO_BUS_RESET || LX164 || SX164
extern int io_bus_reset;
#endif

#if BACKING_STORE
int vga_console_size;
char *vga_console_buffer;
char *vga_console_attributes;
#endif

/* used to indicate to emulator that memory allocation should not be done */
/* again*/ 

#if MODULAR
extern int reusememory;
extern int VGAOwnMemory;
extern int VGADisplayBanner;
#else
int reusememory;
int VGADisplayBanner;
#endif

#if MODULAR
struct SEMAPHORE x86_c;
struct SEMAPHORE x86_i;
struct SEMAPHORE x86_s;
#endif

extern struct pb *console_pb;
struct  TTPB *ttpb;
struct  VGA_PB *vpb;
struct CH_STATE *chs;

/*Constants*/
#define VGA_DEBUG 0

/*External references*/
extern void aputchar (struct CH_STATE *chs,char c);
extern int spl_kernel;
extern struct TTPB null_ttpb;
extern struct TTPB *console_ttpb;
extern int graphics_console;
extern null_procedure ();

/*Function prototypes*/
int  vga_close ();
void vga_erase(void);
int  vga_bios_init ();
int  vgatt_init (struct CH_STATE *chs);
void vga_ioctl(int function);
void vga_load_fonts(void);
int  vga_open ();
char vga_paputchar (struct CH_STATE *chs,char ch,unsigned char attribute,
							   int xpos,int ypos);
char vga_pagetchar (struct CH_STATE *chs,char* ch,unsigned char* attribute,
							   int xpos,int ypos);
int  vga_read ();
int  vga_write ();
void vga_setloc(struct CH_STATE *chs);
void vga_software_init (struct CH_STATE *chs);
void vga_txoff (struct VGA_PB *vpb);
void vga_txon (struct VGA_PB *vpb);
int  vga_txready (struct VGA_PB *vpb);
void vga_txsend (struct VGA_PB *vpb,char ch);
#if SABLE || RAWHIDE || TURBO || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER
bios_mchk_handler( unsigned int *flag, int *frame );
#endif


#ifndef	DEC_TGA2
#define DEC_TGA2 0x000d1011
#endif
#ifndef ELSA_VGA
#define	ELSA_VGA 0x3d07104c
#endif

static int zero()
{
    return( 0 );
}

#if !MODULAR
/*	 
**  the is the charcter lookup table for the ISO-latin 1 set.  The fall back on
**  this is to map the character to the character it would be with out the
**  accent.  A with a ring would become A without a ring.
**
**  The following is a list of characters by entry that are remapped.
**
**	10/08	general currency -> underscore
**	10/09	copyright -> small c
**	11/03	superscript 3	->  3
**	11/09	superscript 1	->  1
**	12/00	capital A with grave accent -> capital A
**	12/01	capital A with acute accent -> capital A
**	12/02	capital A with circumflex accent -> capital A
**	12/03	capital A with tilde accent -> capital A
**	12/08	capital E with grave accent -> capital E
**	12/10	capital E with circumflex accent -> capital E
**	12/11	capital E with umlaut accent -> capital E
**	12/12	capital I with accute accent -> capital I
**	12/13	capital I with grave accent -> capital I
**	12/14	capital I with circumflex accent -> capital I
**	12/15	capital I with umlaut accent -> capital I
**	13/02	capital O with accute accent -> capital O
**	13/03	capital O with grave accent -> capital O
**	13/04	capital O with circumflex accent -> capital O
**	13/05	capital O with tilde accent -> capital O
**	13/07	Capital OE -> capital O
**	13/08	Capital O with a slash -> capital O
**	13/09	Capital U with grave accent -> capital U
**	13/10	Capital U with acute accent -> capital U
**	13/11	Capital U with circumflex accent -> capital U
**	13/13	Capital Y with umlaut accent -> capital Y
**	14/03	small a with tilde -> small a
**	15/05	small o with tilde -> small o
**	15/07	small oe ligature -> small o
**	15/08	small o with slash -> small o
**	
*/	

unsigned char char_lookup_table[128] = {
    0,0,0,0,0,0,0,0,			/* 128 */
    0,0,0,0,0,0,0,0,			/* 136 */
    0,0,0,0,0,0,0,0,			/* 144 */
    0,0,0,0,0,0,0,0,			/* 152 */
    0,173,155,156,0,157,0,21,		/* 160 */
    '_','c',166,174,0,0,0,0,		/* 168 */
    248,241,253,'3',0,230,20,249,	/* 176 */
    0,'1',167,175,172,171,0,168,	/* 184 */
    'A','A','A','A',142,143,146,128,	/* 192 */
    'E',144,'E','E','I','I','I','I',	/* 200 */
    0,165,'O','O','O','O',153,'O',	/* 208 */
    'O','U','U','U',154,'Y',0,225,	/* 216 */
    133,160,131,0,132,134,145,135,	/* 224 */
    138,130,136,137,141,161,140,139,	/* 232 */
    0,164,149,162,147,'o',148,'o',	/* 240 */
    'o',151,163,150,129,152,0,0		/* 248 */
};
#endif

struct CH_STATE *vga_ch_state;

int x86_pid;
int x86_status;
unsigned int bios_return[4];	/* JMK_FIX: not using bios_return[3] (the bus type) yet */

#if !BACKING_STORE
static char *msg_buf;
#endif

/*Global references*/
#if VGA_DEBUG && 0
struct DDB vga_ddb = {
	"vga",			/* how this routine wants to be called	*/
	vga_read,		/* read routine				*/
	vga_write,		/* write routine			*/
	vga_open,		/* open routine				*/
	vga_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	0,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};
#endif

#define	ATI_GFX        0x41581002

#if MODULAR
#if STARTSHUT
int vga_set_pb_ptr() {
    eisa_dev = find_dev("eisa0", &config);
    if (eisa_dev) {
	eisa_pb_ptr->hose = eisa_dev->hose;
	eisa_pb_ptr->slot = eisa_dev->slot;
    }

    vga_dev = find_dev("vga0", &config);
    if (vga_dev) {

/* load vga_pb into device */

	vga_pb_ptr = &vga_pb;
	vga_dev->devdep.io_device.devspecific = vga_pb_ptr;

	eisa_pb_ptr->hose = vga_dev->hose;	/* point eisa to vga also */
	eisa_pb_ptr->slot = vga_dev->slot;

/* load the base address from device to vga_pb */

	vga_pb.hose = vga_dev->hose;
	vga_pb.slot = vga_dev->slot;
	vga_pb.bus = vga_dev->bus;
	vga_pb.function = vga_dev->function;
	vga_pb.channel = vga_dev->channel;
	vga_pb.controller = vga_dev->devdep.io_device.controller;
	vga_pb.config_device = vga_dev;
    } else {
	if (eisa_dev) {
	    vga_pb_ptr = &eisa_pb;
	} else {
	    return msg_failure;
	}
    }
    return msg_success;
}
#endif

#if DRIVERSHUT
char *vga_string = "vga_bios";

int vga_set_pb_ptr() {

    /* EISA VGA NOT WORKING */

    find_pb( vga_string, 0, match_controller, &vga_pb_ptr, 0 );
    if ( vga_pb_ptr ) {

	eisa_pb_ptr->hose = vga_pb_ptr->hose;	/* point eisa to vga also */
	eisa_pb_ptr->slot = vga_pb_ptr->slot;

    } else {
	if (0) {
	    vga_pb_ptr = &eisa_pb;
	} else {
	    return msg_failure;
	}
    }
    return msg_success;
}
#endif
#endif

/*+
 * ============================================================================
 * = vga_bios_init - vga init.                                                =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This routine initializes the generic vga driver.
 *
 *  FORM OF CALL:
 *
 *	vga_bios_init (); 
 *
 *  RETURNS:
 *
 *	status - msg_success returned on success.
 *
 *  ARGUMENTS:
 *
 *	None
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */

int vga_init()
{
    int status;

    status = vga_bios_init();
    return status;
}

int vga_bios_init()
{
ULONG VideoCardType;
struct INODE *ip;
int status;
#if RAWHIDE
int iod;
struct device *dev;
#endif
#if SABLE || RAWHIDE || TURBO || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER
struct HQE hqe;
#endif
#if AVMIN
	unsigned char quick_start;
#endif
#if ALCOR
int temp_ipl;
#endif

#if REGATTA && (GALAXY)
    if (get_console_base_pa())
	return(msg_success);	   /* no BIOS emulation in instance #1 */
#endif

#if MODULAR
    qprintf("Initializing VGA\n");
#endif

    vpb = 0;			/* For consoles that don't reload on init */

#if MODULAR && ( STARTSHUT || DRIVERSHUT )
    status = vga_set_pb_ptr();
    if (status != msg_success)
	return status;
#endif

bios_return[0] = 0xffff;
bios_return[1] = 0xffff;
bios_return[2] = 0;

#if MIKASA || ALCOR || SABLE
bios_return[3] = EISA_BUS;
#else
bios_return[3] = ISA_BUS;
#endif

/*Allocate the character and display structure */
	chs = (struct CH_STATE*)malloc_noown(sizeof(struct CH_STATE));

/* Allocate a buffer for vga_reinit */
#if !BACKING_STORE
	msg_buf = (char*)malloc_noown(MAX_ROW*MAX_COL);
#endif
#if BACKING_STORE
    vga_console_size = MAX_ROW * MAX_COL;
	vga_console_buffer = (char*)malloc_noown(vga_console_size);
	vga_console_attributes = (char*)malloc_noown(vga_console_size);
#endif

/*Initialize the VGA software*/

    vga_software_init(chs);

/* leave the following in until we know exactly how to determine if a bios */
/* is really there.  Looking for c0000 in target .c doesn't always work.   */

#if AVMIN
    quick_start = rtc_read(0x3e);
    if (quick_start == 0x31) {
	return(msg_success); 
    }
#endif

#if SABLE || RAWHIDE || TURBO || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER
    hqe.handler = bios_mchk_handler;
#if MODULAR
    insq_lock( &hqe, mchq->blink );
#else
    insq_lock( &hqe, mchq_660.blink );
#endif
#else
    ExpectMachineCheck = 1;
#endif

#if RAWHIDE
/*
 * Enable hard error interrupts on bridge to let PCI errors (660s) through.
 * Do so on all PCI buses in case we ever officially support BIOS emulation
 * on non-zero PCIs..
 */
    for ( iod = 0; iod < MAX_IOD_CNT; iod++ )
        if ( dev = iod_device[iod] )
            pci_enable_error_interrupts( dev );
#endif

    reusememory = 0;

#if ALCOR
    /* raise ipl so PCI bus probing is not interrupted by the timer */
    temp_ipl = mfpr_ipl();	/*jfr*/
    mtpr_ipl(31);
#endif

/* Init the Emulation subsystem */

#if MODULAR
    VGADisplayBanner = 1;
    VGAOwnMemory = 1;
    vga_start_bios();
#else
    VGADisplayBanner = 1;
    x86_status = StartBiosEmulator(bios_return);
    if (x86_status == msg_success)
        vga_pb_ptr = get_matching_pb(bios_return[2], bios_return[0], bios_return[1], 0, 0);
#endif

#if ALCOR
    mtpr_ipl(temp_ipl);
#endif

/* indicate to the system that memory has been allocated and it's a sable */
/* I'm assuming that it needs to be done at least once */

#if !MODULAR
    reusememory = SABLE || CORELLE || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER || SX164 || LX164;
#endif

/*******************************/
    
#if RAWHIDE
    for ( iod = 0; iod < MAX_IOD_CNT; iod++ )
        if ( dev = iod_device[iod] )
            pci_disable_error_interrupts( dev );
#endif

#if SABLE || RAWHIDE || TURBO || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER
    remq_lock( &hqe );
#else
    ExpectMachineCheck = 0;
#endif

#if MODULAR
    if (x86_status == msg_failure) {
	vga_stop_bios();
	free(chs);
#if !BACKING_STORE
	free(msg_buf);
#endif
#if BACKING_STORE
	free(vga_console_buffer);
	free(vga_console_attributes);
#endif
	return(msg_success); 
    }
#endif

#if !MODULAR
    if (x86_status == msg_failure)
	return(msg_success); 
#endif

#if VGA_DEBUG && 0
/* Allocate an inode so that the VGA port appears as a write only inode.*/

    allocinode ("vga", 1, &ip);
    ip->dva = & vga_ddb;
    ip->len[0] = ip->alloc[0] = chs->ds.CursorMaxYPosition *
					chs->ds.CursorMaxXPosition;
    ip->loc = 0;
    ip->attr = ATTR$M_WRITE;
    ip->misc = (int*)chs;
    INODE_UNLOCK (ip);
#endif

/* install the vga device as a port driver to the TTY class driver. */

    vgatt_init(chs);
  
/*Initialize the VGA hardware*/

#if MODULAR
    qprintf("Initialize VGA hardware\n");
#endif

    vga_hardware_init(chs);

/*Clear the screen*/

    vga_erase();

#if MODULAR
    graphics_enabled = 1;		/* graphics are now enabled */
    graphics_device = 1;		/* indicate graphics is VGA */
    if (!vg_controllers)
	vg_controllers++;		/* need to do this for an EISA VGA */

/* Output the banner */

    vga_banner();

    if (vga_shutdown_flag)
	vga_stop_bios();
#endif

    return msg_success;       
}

/*+
 * ============================================================================
 * = vgatt_init - vga tt init.                                                =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This routine initializes the VGA driver port driver to
 *	tty class driver.
 *
 *
 *  FORM OF CALL:
 *
 *	vgatt_init (chs); 
 *
 *  RETURNS:
 *
 *	status - msg_success returned on success.
 *
 *  ARGUMENTS:
 *
 *	struct CH_STATE *chs - Pointer to the character and display state.
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */

int vgatt_init (struct CH_STATE *chs)
{
    int	vector=0,dev_slot=0,bus_type,hose,slot,bus;
    struct  pci_devs_found *dev_fnd_ptr;
    int status = msg_failure;

    if (vpb)
	return msg_success;

/*Allocate the port block */
	    vpb = (struct VGA_PB*)malloc_noown(sizeof(struct VGA_PB));
#if MODULAR
	vgapb = vpb;
#endif

/*Save the pointer to the state structure*/
	vpb->chs = chs;

/* this code MUST be invoked before the kbd driver !! */
/* take care of it in *.bld file 		      */

/* grab a port block and spinlock */
	ttpb = tt_init_port("vga0");
#if MODULAR
	graphics_ttpbs = ttpb;
	ttpb->page = graphics_page_size;
	ttpb->spage = graphics_page_size;
	ttpb->type = graphics_term_type;
#endif

/*Disable logins and control characters if we are interactive*/
#if INTERACTIVE_SCREEN
	ttpb->login_ena = 0;
	ttpb->ctrl_o_ena = 0;
	ttpb->ctrl_c_ena = 0;
	ttpb->ctrl_x_ena = 0;
	ttpb->ctrl_t_ena = 0;
#endif

/* the other half of the port block will be filled in by the kbd driver */
	ttpb->txoff = vga_txoff;
	ttpb->txon = vga_txon;
	ttpb->txready = vga_txready;
	ttpb->txsend = vga_txsend;
	ttpb->rxoff = null_procedure;
	ttpb->rxon = null_procedure;
	ttpb->rxready = null_procedure;
	ttpb->rxread = null_procedure;

 /* set so our txon routine will be called */
	ttpb->mode = DDB$K_INTERRUPT;

	ttpb->port = vpb;
	vpb->ttpb = ttpb;
	spinunlock(&spl_kernel);

#if MODULAR
	if (!kbd_shared_adr)
	    ovly_load("KBD");
	if (!kbd_shared_adr)
	    return msg_failure;

	status = kbd_init();
	if (status != msg_success)
	    return status;

#if RAWHIDE || TURBO
	primary_console_sel(ttpb, PCI_BUS, vga_pb.hose, vga_pb.bus, vga_pb.slot);
#endif
#endif

	if (graphics_console && (console_ttpb == &null_ttpb)) {
	    console_ttpb = ttpb;
	    if ((bios_return[0] == 0xffff) && (bios_return[1] == 0xffff)) {
#if MIKASA || ALCOR || SABLE
		bus_type = EISA_BUS;
#else
		bus_type = ISA_BUS;
#endif
		hose = 0;
		slot = 0;
		bus = 0;
	    } else {
#if RAWHIDE || TURBO
		bus_type = PCI_BUS;
		hose = ETAPciHose;
		slot = bios_return[1];
		bus = bios_return[0];
		console_pb = &vga_pb;
#else
		bus_type = PCI_BUS;
		hose = bios_return[2];
		slot = bios_return[1];
		bus = bios_return[0];
		console_pb = get_matching_pb(hose, bus, slot, 0, 0);
#endif
	    }
#if !(RAWHIDE || TURBO)
	    fill_in_ctb_ws_slotinfo (bus_type, hose, bus, slot);
#endif
	}
	return msg_success;
}  

/*+
 * ============================================================================
 * = vga_software_init - Initialize the vga software.                         =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This routine initializes the vga software data structures.
 *
 *  FORM OF CALL:
 *
 *	vga_software_init (chs); 
 *
 *  RETURNS:
 *
 *	None.
 *
 *  ARGUMENTS:
 *
 *	struct CH_STATE *chs - Character state structure.
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */

void vga_software_init (struct CH_STATE *chs)
{

/*Initialize the display state*/
	vga_ch_state = chs;
	memset (chs, 0, sizeof (struct CH_STATE));
	chs->ds.CursorXPosition = 0;
	chs->ds.CursorYPosition = 0;
	chs->ds.CursorMaxXPosition = MAX_COL-1;
	chs->ds.CursorMaxYPosition = MAX_ROW-1;
	chs->ds.ForegroundColor = FOREGROUND_COLOR;
	chs->ds.BackgroundColor = BACKGROUND_COLOR;
	chs->ds.HighIntensity = OFF;
	chs->ds.UnderScored = OFF;
	chs->ds.ReverseVideo = OFF;
	chs->CurrentAttribute = NORMAL_VIDEO;
	chs->putc = (PVF)vga_paputchar;
	chs->getc = (PVF)vga_pagetchar;
	chs->ioctl = (PVF)vga_ioctl;
	display_ForegroundColor = FW_COLOR_WHITE;		/* white */
	display_BackgroundColor = FW_COLOR_BLUE;		/* blue */
	display_HighIntensity = TRUE;
	display_UnderScored = FALSE;
	display_ReverseVideo = FALSE;
	chs->CurrentAttribute = (display_BackgroundColor<<4) | (display_ForegroundColor);
	if (display_HighIntensity)
	    chs->CurrentAttribute |= 0x8;
} 

/*+
 * ============================================================================
 * = vga_hardware_init - Initialize the vga hardware.                         =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This routine initializes the vga hardware using the table
 *	supplied from the ARC console.
 *
 *  FORM OF CALL:
 *
 *	vga_hardware_init (chs); 
 *
 *  RETURNS:
 *
 *	None.
 *
 *  ARGUMENTS:
 *
 *	struct CH_STATE *chs - Character state structure.
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */

void vga_hardware_init(struct CH_STATE *chs)
{
#if SABLE || RAWHIDE || TURBO || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER
struct HQE hqe;
#endif
#if RAWHIDE
int iod;
struct device *dev;
#endif

#if SABLE || RAWHIDE || TURBO || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER
    hqe.handler = bios_mchk_handler;
#if MODULAR
    insq_lock( &hqe, mchq->blink );
#else
    insq_lock( &hqe, mchq_660.blink );
#endif
#else
ExpectMachineCheck = 1;
#endif

#if RAWHIDE
/*
 * Enable hard error interrupts on bridge to let PCI errors (660s) through.
 * Do so on all PCI buses in case we ever officially support BIOS emulation
 * on non-zero PCIs..
 */
    for ( iod = 0; iod < MAX_IOD_CNT; iod++ )
        if ( dev = iod_device[iod] )
            pci_enable_error_interrupts( dev );
#endif

    SetupBiosGraphics();  /* Change the Video mode to show a Blue Screen    */

    /*Set the cursor position*/
    vga_setloc(chs);

#if !INTERACTIVE_SCREEN
    /*Turn the cursor on*/
    vga_ioctl(ENA_CURSOR);
#endif

#if RAWHIDE
    for ( iod = 0; iod < MAX_IOD_CNT; iod++ )
        if ( dev = iod_device[iod] )
            pci_disable_error_interrupts( dev );
#endif

#if SABLE || RAWHIDE || TURBO || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER
    remq_lock( &hqe );
#else
    ExpectMachineCheck = 0;
#endif

}

/*+
 * ============================================================================
 * = vga_reinit - Initialize the vga hardware after returning from an OS      =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This routine initializes the primary vga hardware.  It may be used  
 *	after returning to the console from an operating system.
 *
 *  FORM OF CALL:
 *
 *	vga_reinit (erase_screen); 
 *
 *  RETURNS:
 *
 *	None.
 *
 *  ARGUMENTS:
 *
 *	msg_success - vga successfully reinitialized
 *	msg_failure - vga not successfully reinitialized
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */

int vga_reinit(int erase_screen)
{
    struct CH_STATE *chs;
    unsigned __int64 addr;
    register int i;
    char *buffer;
    char *attributes;
    int r;
    int c;
    char ch;
    unsigned char attribute;
    unsigned int base;
#if SABLE || RAWHIDE || TURBO || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER
    struct HQE hqe;
#endif
#if RAWHIDE
    int iod;
    struct device *dev;
#if GALAXY
    int saved_int0_reg;
    struct pb pb;
#endif
#endif

    chs = vga_ch_state;

    if (!chs)			/* VGA not initialized */
	return msg_success;

    if (graphics_console) {
#if (( RAWHIDE || TURBO ) && STARTSHUT )
	struct device *vga_dev;
	vga_dev = find_dev("vga0",&config);
	if (vga_dev) {
	    if (vga_dev->dtype == DEC_TGA2) {
		base = incfgl( vga_pb_ptr, 0x10 ) & ~15;
		outdmeml( vga_pb_ptr, base + 0x100070, 0 );
	    }
	    if (vga_dev->dtype == ELSA_VGA) {
		unsigned int chip_config;
		base = incfgl( vga_pb_ptr, 0x10 ) & ~15;
		chip_config = inmeml( vga_pb_ptr, base + 0x70 );
		chip_config |= 0x2;
		chip_config &= 0xfffffffe;
		outmeml( vga_pb_ptr, base + 0x70, chip_config );
	    }
	}
#endif
#if SABLE || MIKASA || MEDULLA || CORTEX || UNIVERSE || LX164 || SX164 || DRIVERSHUT || REGATTA || WILDFIRE || MARVEL || PC264 || MONET || EIGER
	if( console_pb && (console_pb->type == TYPE_PCI) ) {
	    if( incfgl( console_pb, 0x00 ) == DEC_TGA2 ) {
		base = incfgl( console_pb, 0x10 ) & ~15;
		outdmeml( console_pb, base + 0x100070, 0 );
	    }
	    if( incfgl( console_pb, 0x00 ) == ELSA_VGA ) {
		unsigned int chip_config;
		base = incfgl( console_pb, 0x10 ) & ~15;
		chip_config = inmeml( console_pb, base + 0x70 );
		chip_config |= 0x2;
		chip_config &= 0xfffffffe;
		outmeml( console_pb, base + 0x70, chip_config );
	    }
        }
#endif
    }

#if K2 || TAKARA
    reset_tga2();
#endif

#if !BACKING_STORE
    if (erase_screen)
       for (i=0; i<MAX_ROW*MAX_COL; ++i)
          msg_buf[i] = 0;
    else {
	addr = VGAASEG;
	for (i = 0; i < MAX_ROW * MAX_COL; ++i) {
	    msg_buf[i] = inmemb(vga_pb_ptr, addr);
	    addr += 2;
	}
    } /* else not erase */
#endif

#if MODULAR
    if (vga_shutdown_flag)
	eisa_bus_reset = 1;
#endif

#if SABLE || RAWHIDE || TURBO
    if (eisa_bus_reset) {
	hqe.handler = bios_mchk_handler;
#if MODULAR
	insq_lock(&hqe, mchq->blink);
#else
	insq_lock(&hqe, mchq_660.blink);
#endif

#if RAWHIDE
/*
 * Enable hard error interrupts on bridge to let PCI errors (660s) through.
 * Do so on all PCI buses in case we ever officially support BIOS emulation
 * on non-zero PCIs..
 * Do so ONLY on PCI buses in THIS galaxy if we're in a galaxy environment
 */
        for ( iod = 0; iod < MAX_IOD_CNT; iod++ )
            if ( dev = iod_device[iod] )
#if GALAXY
	    if ((galaxy_io_mask & (1 << dev->slot)) ||
		    (galaxy_partitions < 2)) {
#endif
                pci_enable_error_interrupts( dev );
#if GALAXY
	    } else {	/* disable error interrupts on the other IOD in galaxy */
		pb.hose = dev->hose;
		saved_int0_reg = pcia_incsr(&pb, INT_MASK0);
		pci_disable_error_interrupts( dev );
	    }
#endif
#endif

	VGADisplayBanner = 0;

#if MODULAR
	vga_start_bios();
#else
	x86_status = StartBiosEmulator(bios_return);
	if (x86_status == msg_success)
	    vga_pb_ptr = get_matching_pb(bios_return[2], bios_return[0], bios_return[1], 0, 0);
#endif
	remq_lock(&hqe);

#if RAWHIDE
        for ( iod = 0; iod < MAX_IOD_CNT; iod++ )
            if ( dev = iod_device[iod] )
#if GALAXY
	    if ((galaxy_io_mask & (1 << dev->slot)) ||
		    (galaxy_partitions < 2)) {
#endif
                pci_disable_error_interrupts( dev );
#if GALAXY
	    } else {	/* restore error interrupts on the other IOD in galaxy */
		pb.hose = dev->hose;
		pcia_outcsr(&pb, saved_int0_reg);
	    }
#endif
#endif

	if (x86_status != msg_success) {
#if MODULAR
	    vga_stop_bios();
#endif
	    eisa_bus_reset = 0;
	    err_printf("VGA Bios failed, status = %x\n", x86_status);
	    return msg_failure;
	}
    }
#endif
#if CORELLE
	if (eisa_bus_reset) {
	    ExpectMachineCheck = 1;

	    StartBiosEmulator(bios_return);

	    eisa_bus_reset = 0;
	    ExpectMachineCheck = 0;
	}
#endif
#if LX164 || SX164
    if ( io_bus_reset ) {
	ExpectMachineCheck = 1;
	StartBiosEmulator(bios_return);
	io_bus_reset = 0;
	ExpectMachineCheck = 0;
    }
#endif
#if IO_BUS_RESET
    if (io_bus_reset) {
	VGADisplayBanner = 0;
	hqe.handler = bios_mchk_handler;
	insq_lock(&hqe, mchq_660.blink);
	x86_status = StartBiosEmulator(bios_return);
	if (x86_status == msg_success)
	    vga_pb_ptr = get_matching_pb(bios_return[2], bios_return[0], bios_return[1], 0, 0);
	remq_lock(&hqe);
	io_bus_reset = 0;
    }
#endif

	vga_hardware_init(chs);

#if SABLE || RAWHIDE || TURBO
#if MODULAR
	if (eisa_bus_reset)
	    if (vga_shutdown_flag)
		vga_stop_bios();
#endif
    eisa_bus_reset = 0;
#endif

#if BACKING_STORE
    vga_erase();
    buffer = vga_console_buffer;
    attributes = vga_console_attributes;
    for (r = 0; r < MAX_ROW; r++) {
	for (c = 0; c < MAX_COL; c++) {
	    ch = *buffer++;
	    attribute = *attributes++;
	    if (ch && ch != ' ')
		vga_paputchar(chs, ch, attribute, c, r);
	}
    }
    vga_paputchar(chs, 0, 0, chs->ds.CursorXPosition, chs->ds.CursorYPosition);
#endif

#if !BACKING_STORE
/*Clear the screen*/
/*    vga_erase(); */

    addr = VGAASEG;
    for (i = 0; i < MAX_ROW * MAX_COL; ++i) {
	outmemb(vga_pb_ptr, addr++, msg_buf[i]);
	outmemb(vga_pb_ptr, addr++,
	  FOREGROUND_COLOR | (BACKGROUND_COLOR << 4));
    }
#endif
    return msg_success;
}

/*+
 * ============================================================================
 * = vga_txoff - Tx off function.                                             =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This routine is simply a place holder for the tx off function.
 *
 *  FORM OF CALL:
 *
 *	vga_txoff (); 
 *
 *  RETURNS:
 *
 *	None.
 *
 *  ARGUMENTS:
 *
 *	None
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */


void vga_txoff (struct VGA_PB *vpb) {
}


/*+
 * ============================================================================
 * = vga_txon - vga tx on function.                                           =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This function handles the tt tx on function.
 *
 *	In order to coerce the VGA driver to live with the TTY's
 *	requirement of a transmit queue, we have to set the VGA driver
 *	to DDB$K_INTERRUPT, and then have the txon routine do the actual
 *	sending of characters.
 *
 *  FORM OF CALL:
 *
 *	vga_txon (vpb); 
 *
 *  RETURNS:
 *
 *	None.
 *
 *  ARGUMENTS:
 *
 *	struct VGA_PB *vpb - Pointer to the tt port (ttpb->port).
 *			     In the vga's case VGA_PB. 
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */

void vga_txon (struct VGA_PB *vpb) {
	tt_tx_interrupt (vpb->ttpb);
}


/*+
 * ============================================================================
 * = vga_txready - The tx ready function.                                     =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This routine handles the tx ready function.
 *
 *	We're always ready since we're polled and writing to a
 *	character cell bitmap.
 *
 *  FORM OF CALL:
 *
 *	vga_txready (); 
 *
 *  RETURNS:
 *
 *	status - msg_success returned on success.
 *
 *  ARGUMENTS:
 *
 *	None
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */

int vga_txready (struct VGA_PB *vpb)
{
#if MODULAR
    if (vpb == 0)
	vpb = vgapb;
#endif

    if (vpb->chs->scroll_in_prog) {
	scroll (vpb->chs);
	return 0;
    } else {
	return 1;
    }
}

/*+
 * ============================================================================
 * = vga_txsend - The tx send function.                                       =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This routine handles the tx send function.
 *
 *  FORM OF CALL:
 *
 *	vga_txsend (vpb,ch); 
 *
 *  RETURNS:
 *
 *	None.
 *
 *  ARGUMENTS:
 *
 *	struct VGA_PB *vpb - Pointer to the tt port (ttpb->port).
 *			     In the vga's case VGA_PB. 
 *	char ch - Character to send.
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */

void vga_txsend (struct VGA_PB *vpb,char ch)
{
#if MODULAR
    if (vpb == 0)
	vpb = vgapb;
#endif

    aputchar (vpb->chs,ch); 		/*Put the character*/
}

/*+
 * ============================================================================
 * = vga_open - Open the VGA.                                                 =
 * ============================================================================
 *
 *  OVERVIEW:         
 *  
 *      Open the VGA device. 
 *	Takes as info field row column
 *  
 *  FORM OF CALL:
 *  
 *	vga_open (fp,info,next,mode)
 *  
 *  RETURNS:
 *
 *	msg_success - Success
 *       
 *  ARGUMENTS:
 *
 *	struct FILE *fp - File to be opened.
 *	char *info - Row and column
 *	char *next - Next driver
 *	char *mode - Mode for open
 *
 *  SIDE EFFECTS:
 *
 *	None
 *
-*/

int vga_open( struct FILE *fp, char *info, char *next, char *mode )
{
#if VGA_DEBUG
int	i;
char	number [32];
int	n [2];
struct CH_STATE *chs;

	/*
	 * Treat the information field as a (row, column) specification, origin
	 * 0.  We capture the cursor position here, but delay the range check
	 * until read/write time.
	 */
	chs = (struct CH_STATE*)fp->ip->misc;
	if (info && *info) {
	    for (i=0; i<2; i++) {
		strelement (info, i, "	 \r\n", number);
		n [i] = atoi (number);
	    }
	    *fp->offset = n [0] + n [1] * chs->ds.CursorMaxYPosition;
	}
#endif
	return msg_success;
}

/*+
 * ============================================================================
 * = vga_close - Close the VGA.                                               =
 * ============================================================================
 *
 *  OVERVIEW:       
 *  
 *      This routine closes the VGA port.
 *
 *  FORM OF CALL:
 *  
 *	vga_close (fp)
 *  
 *  RETURNS:
 *
 *	None
 *       
 *  ARGUMENTS:
 *
 *	struct FILE *fp - File to be closed.
 *	
 *
 *  SIDE EFFECTS:
 *
 *	None
 *
-*/

int vga_close( struct FILE *fp )
{
	return msg_success;
}


/*+
 * ============================================================================
 * = vga_read - read characters from the character cell map                   =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This reads characters from the character cell map (REAL useful).  A
 *	read starts at the current cursor position, and terminates when the
 *	end of the screen is encountered.
 *
 *	The cursor position is kept local to this file descriptor (implying
 *	simultaneous reads by different processes will not collide with each
 *	other).
 *
 *
 *  FORM OF CALL:
 *  
 *	vga_read (fp,size,number,buf)
 *
 *  ARGUMENTS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should
 *			correspond to the VGA device.
 *	int size	size of item in bytes
 *	int number 	number of items.
 *	char *buf	Address to write the data to.
 *
 *  RETURNS:
 *
 *	Number of bytes read, or 0 on error.
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */

int vga_read( struct FILE *fp, int size, int number, unsigned char *s )
{
    int	i = 0;
    int	video_addr;

#if VGA_DEBUG && 0
	for (i=0; i<size*number; i++) {
	    /* range check against the screen size */
	    if (!inrange (*fp->offset, 0, fp->ip->len[0])) {
		break;
	    }
	    video_addr = VGAASEG + ((*fp->offset ) << 1);

	    /* read from video ram directly the character and the attributes */
	    *s++ = inmemw(vga_pb_ptr,video_addr & 0xffff);

	    *fp->offset += 1;	   
	}
#endif
    return i;
}


/*+
 * ============================================================================
 * = vga_write - write to the vga                                             =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This routine writes to the vga.
 *
 *  FORM OF CALL:
 *  
 *	vga_write (fp,size,number,buf)
 *
 *  ARGUMENTS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should
 *			correspond to the VGA device.
 *	int size	size of item in bytes
 *	int number 	number of items.
 *	char *buf	Address to write the data to.
 *
 *  RETURNS:
 *
 *	Number of bytes read, or 0 on error.
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */


int vga_write( struct FILE *fp, int size, int number, unsigned char *s )
{
#if VGA_DEBUG && 0
struct CH_STATE *chs;
int len;
char chr;
	
/*Get the character state*/
	chs = (struct CH_STATE*)fp->ip->misc;

/*Get the length*/
	len = size * number;
	while(len--){
	/* write this character */
	    chr = *s++;
	    aputchar(chs,chr);
         }

/* Return the length */
	return size * number - len;
#else
    return 0;
#endif
}


/*+
 * ============================================================================
 * = vga_load_fonts - Load the fonts.                                          =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This function will load the fonts.
 *
 *  FORM OF CALL:
 *  
 *	vga_load_fonts()
 *
 *  ARGUMENTS:
 *
 *	None.
 *
 *  RETURNS:
 *
 *	None
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */

void vga_load_fonts(void)
{
#if 0
unsigned __int64 addr;
register int  i;
register int  j;
register unsigned char  *cp;

/*Load the fonts*/
	addr = VGAGSEG;
	for (i=0; i<256; ++i) {
	    cp = &vga8x16xx[0];
	    if (i>=0x20 && i<=0x7F)
		cp = &vga8x16gl[16 * (i-0x20)];

	/* 16 lines */
	    for (j=0; j<16; j+=2) {
	    /* 8 bits */
		outmemb (vga_pb_ptr, addr++, cp[j+0]);
		outmemb (vga_pb_ptr, addr++, cp[j+1]);
	    }

        /* 16 unused */
	    for (j=0; j<16; j+=2) {
	    /* byte cycles */
		outmemb (vga_pb_ptr, addr++, 0xff);
		outmemb (vga_pb_ptr, addr++, 0xff);
	    }
	}                             
#endif
} 

/*+
 * ============================================================================
 * = vga_erase - Erase.                                                       =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This function will clear the screen.
 *
 *  FORM OF CALL:
 *
 *	vga_erase()
 *
 *  ARGUMENTS:
 *
 *	None.
 *
 *  RETURNS:
 *
 *	None.
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */

void vga_erase(void)
{
unsigned __int64 addr;
register int  i;

/*Clear the screen*/
	addr = VGAASEG;
	for (i=0; i<MAX_ROW*MAX_COL; ++i) { 
	    outmemb (vga_pb_ptr,addr++,' ');
	    outmemb (vga_pb_ptr,addr++,FOREGROUND_COLOR|(BACKGROUND_COLOR<<4));
	}
}

/*+
 * ============================================================================
 * = vga_setloc - Set the location.                                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This function will set the vga cursor location.
 *
 *  FORM OF CALL:
 *
 *	vga_setloc()
 *
 *  ARGUMENTS:
 *
 *	struct CH_STATE *chs - Character state structure.
 *
 *  RETURNS:
 *
 *	None
 *
 *  SIDE EFFECTS:
 *
 *	None.
 *
 *--
 */

void vga_setloc(struct CH_STATE *chs)
{
register int offset;

/*Get the postion*/
#if VGA_DEBUG && 0
	pprintf ("Set loc x: %d y: %d\n",chs->ds.CursorXPosition,
						chs->ds.CursorYPosition);
#endif
	offset = ((chs->ds.CursorMaxXPosition+1)*chs->ds.CursorYPosition)+
						  chs->ds.CursorXPosition;
						
/*Write the cursor location registers*/
#if SABLE || RAWHIDE || TURBO
	outportw(vga_pb_ptr,CRTA,(((offset>>8)&0xff)<<8)|CRT_K_CLH);
	outportw(vga_pb_ptr,CRTA,((offset&0xff)<<8)|CRT_K_CLL);
#else
	outportb(vga_pb_ptr,CRTA,CRT_K_CLH);
	outportb(vga_pb_ptr,CRTD,(offset>>8)&0xFF);
	outportb(vga_pb_ptr,CRTA,CRT_K_CLL);
	outportb(vga_pb_ptr,CRTD,offset&0xFF);
#endif
}


/*+
 * ============================================================================
 * = vga_paputchar - Put a character to the port                              =
 * ============================================================================
 *
 *  OVERVIEW.
 *
 * 	This routine will put a character to the port using a specified
 *	position and attribute
 *
 *  FORM OF CALL:
 *
 *	vga_paputchar (chs,ch,attribute,xpos,ypos)
 *
 *  RETURNS:
 *
 *	c - The character that was output
 *		EOF on error.
 *
 *  ARGUMENTS:
 *
 *	struct CH_STATE *chs - Character state structure.
 *	char ch - Character to be output. A ch of 0 will only set
 *		  the cursor position.
 *	unsigned char *attribute -  Pointer to the attribute read where:
 *
 *				7 6 5 4 | 3 2 1 0
 *                  		b b b b   f f f f
 *
 *				b is the 4-bit background color (0-15)
 *				f is the 4-bit foreground color (0-15)
 *
 *	int xpos - X coordinate where the character is to be output.
 *	int ypos - Y coordinate where the character is to be output.
 *
 *  SIDE EFFECTS:
 *
 *	None
 *
-*/

char vga_paputchar (struct CH_STATE *chs,char ch,unsigned char attribute,
							int xpos,int ypos)
{
    unsigned __int64 addr;
    register int offset;
    unsigned int char_index = (unsigned int)ch;
    int both;
    
    char_index &= 0xff;


#if VGA_DEBUG && 0
    pprintf ("ch: %02x att: %02x xpos: %d ypos: %d\n",char_index,attribute,xpos,ypos);
#endif

/*Write the character only if we have one*/

#if !MODULAR && !ARC_SUPPORT
    if (char_index > 128)
    {
    	char_index = char_lookup_table[char_index - 128];  /* redefine char */
    }
#endif

    if (char_index){
#if BACKING_STORE
	vga_console_buffer [xpos + ypos * MAX_COL] = ch;
	vga_console_attributes [xpos + ypos * MAX_COL] = attribute;
#endif

    /*Get the postion then the address*/
	offset = ((chs->ds.CursorMaxXPosition+1)*ypos)+xpos;
	addr = VGAASEG + (offset*2);

    /*Write the character then the attribute*/
	both = ((unsigned char)attribute << 8) | (unsigned char)char_index;
	outmemw (vga_pb_ptr,addr,both);
    }

/*Set the location*/ 
	vga_setloc(chs);

/*Return*/
	return ch;
}


/*+
 * ============================================================================
 * = vga_pagetchar - Get character information                                =
 * ============================================================================
 *
 *  OVERVIEW.
 *
 * 	This routine will get character information for a specified x and
 *	Y coordinate.
 *
 *  FORM OF CALL:
 *
 *	vga_pagetchar (chs,ch,attribute,xpos,ypos)
 *
 *  RETURNS:
 *
 *	c - The character read at the specified location.
 *		EOF on error.
 *
 *  ARGUMENTS:
 *
 *	struct CH_STATE *chs - Character state structure.
 *	char *ch - Pointer to the character read.
 *	unsigned char *attribute -  Pointer to the attribute read where:
 *
 *				7 6 5 4 | 3 2 1 0
 *                  		b b b b   f f f f
 *
 *				b is the 4-bit background color (0-15)
 *				f is the 4-bit foreground color (0-15)
 *
 *	int xpos - X coordinate where the character is to be read.
 *	int ypos - Y coordinate where the character is to be read.
 *
 *  SIDE EFFECTS:
 *
 *	None
 *
-*/

char vga_pagetchar (struct CH_STATE *chs,char *ch,
				unsigned char *attribute,int xpos,int ypos)
{
#if BACKING_STORE
	*ch = vga_console_buffer [xpos + ypos * MAX_COL];
	*attribute = vga_console_attributes [xpos + ypos * MAX_COL];
	if (*ch == 0) {
	    *ch = ' ';
	    *attribute = NORMAL_VIDEO;
	}
#else
unsigned __int64 addr;
register int offset;
int both;

/*Get the postion then the address*/
	offset = ((chs->ds.CursorMaxXPosition+1)*ypos)+xpos;
	addr = VGAASEG + (offset*2);

/*Read the character then the attribute*/
	both = inmemw (vga_pb_ptr,addr);
	*ch = (unsigned char)both;
	*attribute = (unsigned char)(both >> 8);
#endif

/*Return*/
	return *ch;
}


/*+
 * ============================================================================
 * = vga_ioctl - i/o control                                                  =
 * ============================================================================
 *
 *  OVERVIEW.
 *
 * 	This function will handle i/o control functions such as, cursor on
 *	and cursor off.
 *
 *  FORM OF CALL:
 *
 *	ioctl (function)
 *
 *  RETURNS:
 *
 *	None.
 *
 *  ARGUMENTS:
 *
 *	int function - Function code as defined in chstate.h.
 *
 *  SIDE EFFECTS:
 *
 *	None
 *
-*/

void vga_ioctl(int function)
{

/*Look at the function code and take action*/
	switch (function){
	    case ENA_CURSOR:
#if SABLE || RAWHIDE || TURBO
		outportw(vga_pb_ptr,CRTA,(vgacinit[CRT_K_CS]<<8)|CRT_K_CS);
#else
		outportb(vga_pb_ptr,CRTA,CRT_K_CS);
		outportb(vga_pb_ptr,CRTD,vgacinit[CRT_K_CS]);
#endif
	    break;
	    case DIS_CURSOR:
#if SABLE || RAWHIDE || TURBO
		outportw(vga_pb_ptr,CRTA,((vgacinit[CRT_K_CS]|CRT_M_CO)<<8)|CRT_K_CS);
#else
		outportb(vga_pb_ptr,CRTA,CRT_K_CS);
		outportb(vga_pb_ptr,CRTD,vgacinit[CRT_K_CS]|CRT_M_CO);
#endif
	    break;
	}
}     
#if SABLE || RAWHIDE || TURBO || REGATTA || WILDFIRE || MARVEL || MONET || PC264 || EIGER
int bios_mchk_handler( unsigned int *flag, int *frame )
    {
    machine_handle_vga( frame );
    machine_handle_nxm( frame );
#if SABLE
    clear_cpu_errors( whoami( ) );
#endif
    return( msg_success );
    }
#endif

#if MODULAR
char vga_msg[] = "\r\n";

void vga_banner()
{
    char buffer[256];

    create_banner(&buffer);

    vga_outmsg(&buffer);
    vga_outmsg(&vga_msg);

#if TURBO
    create_cpu_banner(&buffer);

    vga_outmsg(&buffer);
    vga_outmsg(&vga_msg);
#endif

    vga_outmsg(&vga_msg);
}

void vga_outmsg(char *msgptr)
{
    int status = 0;
    char c;
    char *msg_ptr;

    msg_ptr = msgptr;

    while (1) {

	c = *msg_ptr++;
	if (c == 0) {
	    return;
	}

	while (!status)
	    status = vga_txready(vgapb);

	vga_txsend(vgapb, c);
    }
}
#endif
#if MODULAR

/* Initialize x86 */

void x86_process()
{
    x86_status = StartBiosEmulator(bios_return);

    krn$_post(&x86_i);		/* initialized */
    krn$_sleep(100);
    krn$_wait(&x86_s);		/* wait for stop */
    return;
}

int vga_start_bios()
{
    qprintf("Starting Bios Emulator\n");

    if (x86_running_flag) {	/* Was X86 shutdown ? */             
	x86_status = StartBiosEmulator(bios_return);
	return 0;		/* No, Already running */
    }

    qprintf("Loading X86 & X86A\n");

    x86_running_flag = 1;

    if (!x86_shared_adr)
	ovly_load("X86");
    if (!x86A_shared_adr)
	ovly_load("X86A");

/* create X86 process */

    krn$_seminit(&x86_c, 0, "x86_c");	/* completion */
    krn$_seminit(&x86_i, 0, "x86_i");	/* initialized */
    krn$_seminit(&x86_s, 0, "x86_s");	/* stop */

    x86_pid = krn$_create(		/* */
	x86_process, 			/* address of process   */
	null_procedure, 		/* startup routine      */
	&x86_c, 			/* completion semaphore */
	6, 				/* process priority     */
	1 << whoami(), 			/* cpu affinity mask    */
	8192, 				/* stack size           */
	"x86", 				/* process name         */
	0, 0, 0, 0, 0, 0 		/* i/o                  */
	);

    qprintf("Created x86_process: pid %x\n", x86_pid);
    krn$_wait(&x86_i);
    qprintf("Completed Bios Emulation, status = %x\n", x86_status);
    return 0;
}

int vga_stop_bios()
{
    qprintf("Stopping X86 Bios\n");    

    krn$_post(&x86_s);
    krn$_sleep(100);
    krn$_wait(&x86_c);

    krn$_semrelease(&x86_s);
    krn$_semrelease(&x86_i);
    krn$_semrelease(&x86_c);

    ovly_remove("X86");
    ovly_remove("X86A");

    x86_running_flag = 0;

    qprintf("Completed VGA\n");

    return 0;
}
#endif
