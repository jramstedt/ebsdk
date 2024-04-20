/*
 * file: kbd_driver.c
 *
 * Copyright (C) 1992, 1996 by
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
 *      Console Firmware
 *
 *  ABSTRACT:	Keyboard Driver
 *
 *	Keyboard driver for vl82c106 Combo chip.  Handles both 84 and 101 key
 *	keyboards.
 *
 *  AUTHORS:
 *
 *	Judith Gold
 *	Adapted from initial version by Tom Kopec and Dave Conroy. 
 *
 *  CREATION DATE:
 *  
 *	17-August-1992	
 *
 *  MODIFICATION HISTORY:
 *
 *	tna     11-Nov-1998     Conditionalized for YUKONA
 *
 *	er	05-Aug-1996	Removed EBxxx conditionals for polled mode.
 *
 *	wcc	01-Aug-1996	Format cleanup only in preperation for retiring 
 *				the kbd_driver_turbo.c file.
 *
 *	nh	03-Apr-1996	Conditionalize for Cortex.
 *
 *	nh	22-nov-1995	BURNS: kbd_attached always returns true.
 *
 *	nh	22-nov-1995	BURNS: Added PS2 S2 to LK4XX scan code conversion
 *				table.  Burns kbd controller only supports mode 2.
 *
 *	nh	20-nov-1995	Removed "initializing keyboard.." print for BURNS.
 *
 *	nh	13-nov-1995	Changed init_raw_kbd and init_raw_mouse for BURNS.
 *				BURNS hardware is unique and requires extra setup.
 *				Keyboard and mouse setup code moved to burns.c
 *
 *	nh	31-oct-1995	Added a mouse reset for BURNS
 *
 *	al	6-sep-1995	Added ALT key and additional language support.
 *
 *	er	21-Aug-1995	Changed EB platforms to run in polled mode.
 *
 *	noh	16-Aug-1995	Added "quick_start" environment variable for
 *				AVMIN.  This skips memory testing and does 
 *				not start the VGA bios emulator. In this module
 *				the kdb_init code returns as if there was no
 *				keyboard present if the variable is set.
 *
 *	dtr	24-jul-1995	merged sable and aps platforms console routines
 *
 *	dtr	11-Jul-1995	Major cleanup of conditionals
 *
 *	rbb	13-Mar-1995	Conditionalize for EB164
 *
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	dtr	30-Aug-1994	Added interrupt enable/disable for the keyboard.
 *				The routines for the call backs were
 *				there but the code was non-existant.  MOUSE
 *				Support for the enable is still non-existant.
 *				
 *	er	15-aug-1994	Conditionalize for SD164
 *
 *	er	23-jun-1994	Conditionalize for EB64+
 *
 *	fwk	21-jun-1994	Added conditional compiles for LEAN build
 *
 *	dtr	21-may-1994	Disable the mouse controller.  Let the os
 *				handle the real enable.  If the os sees an
 *				interrupt from the mouse before the drivers are
 *				started it will try to use the mouse as a
 *				terminal.  At least VMS behaves this way.
 *
 *	dtr	9-may-1994	Rolled in the mustang cahnges with the sable
 *				changes.  This was just a change in the
 *				conditionals.  Got rid of one of the int_raw
 *				routines.
 *				
 *	DTR	27-Jan-1994	Add Mustang support inport/outport to eisa_x
 *
 *	jmp	11-29-93	Add avanti support
 *
 *	jad	07-Oct-1993	Make Platform specific init_raw_device section.
 *
 *	jeg	18-May-1993	removed support as a standalone driver in the
 *				interest of saving space.
 *
 *	pel	25-Feb-1993	replace xbus_in/outport w/ in/outport so
 * 				they'll work w/ either xbus or eisa.
 *
 *	jeg	24-Sep-1992	call combo read/write routines, other fixes 
 * 				after initial debug.
 *
 *	jeg	28-Jan-1992	allow more than one character (escape
 *				sequences) to return to the class driver.
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:stddef.h"
#include "cp$src:common.h"
#include "cp$src:ctype.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:hwrpb_def.h"
#include "cp$src:tt_def.h"
#include "cp$src:kbd.h"
#include "cp$src:pb_def.h"
#include "cp$src:vgapb.h"
#include "cp$src:kbd_pb_def.h"
#include "cp$inc:platform_io.h"
#include "cp$src:platform_cpu.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#if (RAWHIDE || TURBO)
#include "cp$src:mem_def.h"
#endif

/*Constants*/

#if MODULAR
#define DEBUG 0
#define DEBUG_MALLOC 0
#define KBD_PROCESS_DEBUG 0
#else
#define DEBUG 0
#define DEBUG_MALLOC 0
#define KBD_PROCESS_DEBUG 0
#endif

#if IPL_RUN >= 20
#define KBD_POLLED 1
#else
#define KBD_POLLED 0
#endif

#define KBD_TYPE_AT 1
#define KBD_TYPE_USB 2

#if MTU || CORTEX || YUKONA
#define null_flush() 0
#define build_ctb(a,b) 0
#endif

/* KBD_LANG was originally defined here to allow for some kind of explanation 
 * as to why it's done as it is.  The first language, LANG_K_DANSK, is 
 * defined as 0x30. Why 0x30 was chosen is a mystery.  The next language, 
 * LANG_K_DEUTSCH, is defined as 0x32.  Why there is a gap of two between 
 * languages is another mystery.  So if you want to offset into a table for 
 * LANG_K_DEUTSCH, you subtract its language number from the first language, 
 * LANG_K_DANSK, and then you must divide by two, or shifted right by 1,
 * because there's a gap of two between languages.  The declaration for 
 * kbd_lang use to be in kernel.c.
 * #define KBD_LANG  (((kbd_lang-LANG_K_DANSK) >> 1))
 *
 * Now we use kbd_lang_array as our global position it is set to the proper
 * position in ev_action.c and kernel.c using the same formula above. 
 */

/*External references*/

extern struct alt_diffs *alt_table[][NUMBER_OF_LANGUAGES];
extern struct xlate_t xlate_table[];
extern struct xlate_t *(xlate_diffs[][NUMBER_OF_LANGUAGES]);

extern struct multi_byte_xlate common_multi_byte_xlate_table[];
extern struct multi_byte_xlate *specific_multi_byte_xlate_table[];
extern struct keypad_xlate pcxal_keypad_xlate_table[];
extern struct xlate_t lk411_number_pad_xlate_table[];
extern struct xlate_t common_number_pad_xlate_table[] ;
extern struct keypad_xlate diff_pc_keypad_xlate_table[];

extern unsigned int kbd_lang_array;
extern unsigned int kbd_hardware_type;

extern struct LOCK spl_kernel;
extern struct QUEUE tt_pbs;
extern int keybd_present;
extern int keybd_type;
extern struct TTPB serial_ttpb;
extern struct TTPB null_ttpb;
extern struct TTPB *console_ttpb;	/* pointer to primary console port */

extern int graphics_enabled;
extern int graphics_console;
extern int both_console;
extern int arc_fe_inited;
extern int CTB_units[];

extern struct CTB_WS_table CTB_WS_tab[];

extern struct HWRPB *hwrpb;
#if (RAWHIDE || TURBO)
extern unsigned __int64 kbd_pci_ptr;	/* KBD Pci bus pointer */
extern struct window_head config;
#endif


/*Platform specific functions*/

int init_raw_kbd(void);
int kbd_attatched(void);
int init_raw_mouse(void);
int primary_console_sel(struct TTPB *ttpb, int bus_type, int hose, int bus, int slot);

#if RAWHIDE || TURBO
int kbd_mouse_init(void);
#else
void kbdtt_init(void);
#endif

/*Function prototypes*/

int kbd_get(void);
int mouse_get(void);
int kbd_put(unsigned char c, int port);
int mouse_put(unsigned char c, int port);
int kbd_init();
int kbd_rxoff(int dummy);
int kbd_rxon(int dummy);
int kbd_rxready(int dummy);
int kbd_rxread(int dummy);
void kbd_leds(void);
int kbd_output(unsigned char c, int port);
int kbd_output_noretry(unsigned char c, int port);
int kbd_input(void);
void kbd_reinit(void);
void kbd_process(int scan_code);
unsigned char get_alt_code(int scan_code);
int translate_multi_byte_code(int scan_code);
int translate_single_byte_code(int scan_code);
int translate_mixed_byte_code(int scan_code);
int find_multi_byte(int scan_code, struct multi_byte_xlate *ptr);
int find_multi_byte_keypad_code(int scan_code, struct keypad_xlate *ptr, int case_t);
int translate_single_byte_code(int scan_code);
int find_single_byte_code(int scan_code, struct xlate_t *ptr, int case_t);
void translate_control_codes(void);
int search_main_table(int scan_code);
int search_number_pad_table(int scan_code);


/*Globals*/

int kbd_keyboard_flag = 0;
int kbd_mouse_flag = 0;
int keyboard_established;

static int alt_flag;
static int e0_seen;
static int f0_seen;
static int ctrl_flag;
static int shift_flag;
static int scroll_lock;
static int num_lock;
static int caps_lock;
static int kbd_type;
static int mouse_state;


/* typeahead buffer (primarily for escape sequences */

static char ibuff[MAX_LENGTH_OF_MULTI_BYTE_SEQUENCES];
static char *buffptr = ibuff;		/* current pointer */
static char *buffend = ibuff;		/* last character to xmit */
static int initing;


#if (RAWHIDE || TURBO)
struct kbd_pb *kbd_pb;
struct kbd_pb *mouse_pb;
#else
#if WILDFIRE || WFSTDIO
extern struct pb stdio_pb;
#define kbd_pb &stdio_pb
#define mouse_pb &stdio_pb
#else
#define kbd_pb 0
#define mouse_pb 0
#endif
#endif

/* Debug routines */

/*
 * debug flag
 * 1 = top level
 * 2 = malloc and free
 * 4 = regs
 * 8 = kbd_process
 */

int kbd_debug = 0;
int kbd_reg_debug = 0;

#if DEBUG
#include "cp$src:print_var.h"
#define dqprintf _dqprintf
#define dprintf(fmt, args)	\
    _dprintf(fmt, args)
#define d2printf(fmt, args)	\
    _d2printf(fmt, args)
#define d4printf(fmt, args)	\
    _d4printf(fmt, args)
#else
#define dqprintf qprintf
#define dprintf(fmt, args)
#define d2printf(fmt, args)
#define d4printf(fmt, args)
#endif

#if DEBUG_MALLOC
#define malloc(size,sym) \
	dmalloc(size,"sym");
#define malloc_opt(size,opt,mod,rem,zone,sym) \
	dmalloc_opt(size,opt,mod,rem,zone,"sym")
#define free(ptr,sym) \
	dfree(ptr,"sym")
#else
#define malloc(size,sym) \
	dyn$_malloc(size,DYN$K_SYNC|DYN$K_NOOWN)
#define malloc_opt(size,opt,mod,rem,zone,sym) \
	dyn$_malloc(size,opt,mod,rem)
#define free(ptr,sym) \
	dyn$_free(ptr,DYN$K_SYNC)
#endif

unsigned char kbd_84to101_table[256] =
    {
    0x00, 0x47, 0x02, 0x27, 0x17, 0x07, 0x0f, 0x07, /* 00 to 07 */
    0x08, 0x4f, 0x3f, 0x2f, 0x1f, 0x0d, 0x0e, 0x0f, /* 08 to 0f */
    0x10, 0x19, 0x12, 0x13, 0x11, 0x15, 0x16, 0x17, /* 10 to 17 */
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /* 18 to 1f */
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, /* 20 to 27 */
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /* 28 to 2f */
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 30 to 37 */
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /* 38 to 3f */
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* 40 to 47 */
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* 48 to 4f */
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* 50 to 57 */
    0x14, 0x59, 0x5a, 0x5b, 0x5c, 0x5c, 0x5e, 0x5f, /* 58 to 5f */
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, /* 60 to 67 */
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, /* 68 to 6f */
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x08, 0x76, /* 70 to 77 */
    0x78, 0x7c, 0x7a, 0x84, 0x57, 0x7d, 0x5f, 0x7f, /* 78 to 7f */
    0x80, 0x81, 0x82, 0x37, 0x00, 0x85, 0x86, 0x87, /* 80 to 87 */
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, /* 88 to 8f */
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, /* 90 to 97 */
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, /* 98 to 9f */
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, /* a0 to a7 */
    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, /* a8 to af */
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, /* b0 to b7 */
    0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, /* b8 to bf */
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, /* c0 to c7 */
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, /* c8 to cf */
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, /* d0 to d7 */
    0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, /* d8 to df */
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, /* e0 to e7 */
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, /* e8 to ef */
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, /* f0 to f7 */
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff  /* f8 to ff */
};


/* keyboard, mouse  -  input - output routines */
#if DEBUG
void kbd_outportb(unsigned int adr, unsigned char c)
{
    d4printf("kbd_outport: %x %02x\n", p_args2(adr, c));
    outportb(kbd_pb, adr, c);
}
unsigned char kbd_inportb(unsigned int adr)
{
    unsigned char c;
    c = inportb(kbd_pb, adr);
    d4printf("kbd_inport: %x %02x\n", p_args2(adr, c));
    return (c);
}
void mouse_outportb(unsigned int adr, unsigned char c)
{
    d4printf("mouse_outport: %x %02x\n", p_args2(adr, c));
    outportb(mouse_pb, adr, c);
}
unsigned char mouse_inportb(unsigned int adr)
{
    unsigned char c;
    c = inportb(mouse_pb, adr);
    d4printf("mouse_inport: %x %02x\n", p_args2(adr, c));
    return (c);
}
#else
#define kbd_outportb(adr, c) outportb(kbd_pb, adr, c)
#define kbd_inportb(adr) inportb(kbd_pb, adr)
#define mouse_outportb(adr, c) outportb(mouse_pb, adr, c)
#define mouse_inportb(adr) inportb(mouse_pb, adr)
#endif

/*+
 * ============================================================================
 * = kbd_get - get keyboard input					      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Get a character from the keyboard.
 *
 * FORM OF CALL:
 *
 *	kbd_get (); 
 *
 * RETURNS:
 *
 *	-1   - no input
 *	0-ff - input character
 *   
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int kbd_get(void)
{
    unsigned char r;
    unsigned char s;

    s = kbd_inportb(KBD_STAT);
    if (s & KBD_OBF) {
	r = kbd_inportb(KBD_DATA);
#if WFSTDIO
	if (!(s & (KBD_PERR | KBD_GTO)))
	    if (s & KBD_ODS)
		pprintf ("mouse byte %02x\n", r);
	    else
		pprintf ("keyboard byte %02x\n", r);
#endif
	if (!(s & (KBD_PERR | KBD_GTO | KBD_ODS)))
	    return (r & 0xff);
    }
    return (-1);
}


/*+
 * ============================================================================
 * = mouse_get - get mouse input					      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Get a character from the mouse.
 *
 * FORM OF CALL:
 *
 *	mouse_get (); 
 *
 * RETURNS:
 *
 *	-1   - no input
 *	0-ff - input character
 *   
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mouse_get(void)
{
    unsigned char r;
    unsigned char s;

    s = mouse_inportb(KBD_STAT) ^ KBD_ODS;
    if (s & KBD_OBF) {
	r = mouse_inportb(KBD_DATA);
	if (!(s & (KBD_PERR | KBD_GTO | KBD_ODS)))
	    return (r & 0xff);
    }
    return (-1);
}


/*+
 * ============================================================================
 * = kbd_put - send to keyboard						      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Sends a character to the keyboard.
 *
 * FORM OF CALL:
 *
 *	kbd_put (c, port); 
 *
 * RETURNS:
 *
 *	-1   - failure
 *	0-ff - output character
 *   
 * ARGUMENTS:
 *
 *	c    - character to send
 *	port -
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int kbd_put(unsigned char c, int port)
{
    unsigned char s;
    int i;

    for (i = 0; i < 100; i++) {
	s = kbd_inportb(KBD_STAT);
	if (!(s & KBD_IBF)) {
	    kbd_outportb(port, c);
	    return (c & 0xff);
	}
	krn$_micro_delay(1000);
    }
    return (-1);
}


/*+
 * ============================================================================
 * = mouse_put - send to mouse						      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Sends a character to the mouse.
 *
 * FORM OF CALL:
 *
 *	mouse_put (c, port); 
 *
 * RETURNS:
 *
 *	-1   - failure
 *	0-ff - output character
 *   
 * ARGUMENTS:
 *
 *	c    - character to send
 *	port -
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mouse_put(unsigned char c, int port)
{
#if (RAWHIDE || TURBO)
    unsigned char s;
    int i;

    for (i = 0; i < 100; i++) {
	s = mouse_inportb(KBD_STAT);
	if (!(s & KBD_IBF)) {
	    mouse_outportb(port, c);
	    return (c & 0xff);
	}
	krn$_micro_delay(1000);
    }
    return (-1);
#else
    return (kbd_put(c, port));
#endif
}


/*+
 * ============================================================================
 * = kbd_init - keyboard initialization.             			      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Initialize keyboard.
 *	Make the kbd device visible as a file. 
 *
 * FORM OF CALL:
 *
 *	kbd_init (); 
 *
 * RETURNS:
 *
 *	xxxx
 *   
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int kbd_init(void)
{
    int status = msg_success;

    keyboard_established = 0;

#if !(RAWHIDE || TURBO)
    null_flush();
#endif

    initing = 1;

#if (RAWHIDE || TURBO)
    status = kbd_mouse_init();
#else
    kbdtt_init();
#endif

    initing = 0;

    return (status);
}


/*****************************************************************************/
/*                                                                           */
/*            PORT DRIVER ROUTINES (Interface to tt_driver)                  */
/*                                                                           */
/*****************************************************************************/
#if (RAWHIDE || TURBO)
/*+
 * ============================================================================
 * = mouse_rxoff - disable mouse interrupts             		      =
 * ============================================================================
 *
 * OVERVIEW:
 *      
 *      disables mouse-generated interrupts.
 *
 * FORM OF CALL:
 *
 *	mouse_rxoff (); 
 *
 * RETURNS:
 *
 *	none
 *   
 * ARGUMENTS:
 *
 *	int dummy - dummy argument
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mouse_rxoff(int dummy)
{
    int status;

    dprintf("mouse_rxoff\n", p_args0);

#if TURBO || RAWHIDE
    status = io_disable_interrupts(mouse_pb, mouse_pb->pb.vector);
    if (status != msg_success) {
	err_printf("mouse_rxoff: failed to disable interrupts\n");
	return msg_failure;
    }
#else
    io_disable_interrupts(0, MOUSE_VECTOR);
#endif
    return msg_success;
}


/*+
 * ============================================================================
 * = mouse_rxon - enable mouse interrupts                                     =
 * ============================================================================
 *
 * OVERVIEW:
 *      
 *      enables mouse-generated interrupts.
 *
 * FORM OF CALL:
 *
 *	mouse_rxon (); 
 *
 * RETURNS:
 *
 *	xxxx
 *   
 * ARGUMENTS:
 *
 *	int dummy - dummy argument
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int mouse_rxon(int dummy)
{
    int status;

    dprintf("mouse_rxon\n", p_args0);

#if TURBO || RAWHIDE
    status = io_enable_interrupts(mouse_pb, mouse_pb->pb.vector);
    if (status != msg_success) {
	err_printf("mouse_rxon: failed to enable interrupts\n");
	return msg_failure;
    }
#else
    io_enable_interrupts(0, MOUSE_VECTOR);
#endif
    return msg_success;
}
#endif


/*+
 * ============================================================================
 * = kbd_rxoff - disable keyboard interrupts				      =
 * ============================================================================
 *
 * OVERVIEW:
 *      
 *      disables keyboard-generated interrupts.
 *
 * FORM OF CALL:
 *
 *	kbd_rxoff (); 
 *
 * RETURNS:
 *
 *	none
 *   
 * ARGUMENTS:
 *
 *	int dummy - dummy argument
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int kbd_rxoff(int dummy)
{
#if (RAWHIDE || TURBO)
    int status;

    dprintf("kbd_rxoff\n", p_args0);

    status = io_disable_interrupts(kbd_pb, kbd_pb->pb.vector);
    if (status != msg_success) {
	err_printf("kbd_rxoff: failed to disable interrupts\n");
	return msg_failure;
    }
#else
    io_disable_interrupts(0, KEYBD_VECTOR);
#endif
    return msg_success;
}


/*+
 * ============================================================================
 * = kbd_rxon - enable keyboard interrupts				      =
 * ============================================================================
 *
 * OVERVIEW:
 *      
 *      enables keyboard-generated interrupts (normally only called when 
 *	transitioning from polled to interrupt mode.  If we were ever to 
 *	implement reverse flow control, then it would be called then also).
 *
 * FORM OF CALL:
 *
 *	kbd_rxon (); 
 *
 * RETURNS:
 *
 *	xxxx
 *   
 * ARGUMENTS:
 *
 *	int dummy - dummy argument
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int kbd_rxon(int dummy)
{
#if (RAWHIDE || TURBO)
    int status;

    dprintf("kbd_rxon\n", p_args0);

    status = io_enable_interrupts(kbd_pb, kbd_pb->pb.vector);
    if (status != msg_success) {
	err_printf("kbd_rxon: failed to enable interrupts\n");
	return msg_failure;
    }
#else
    io_enable_interrupts(0, KEYBD_VECTOR);
#endif
    return msg_success;
}


/*+
 * ============================================================================
 * = kbd_rxready - checks to see if a keycode is waiting		      =
 * ============================================================================
 *
 * OVERVIEW:
 *      
 *      determines whether a character is waiting or not.
 *
 * FORM OF CALL:
 *
 *	kbd_rxready (); 
 *
 * RETURNS:
 *
 *	0 - there is no character waiting
 *	non-0 - there is a character waiting
 *   
 * ARGUMENTS:
 *
 *	int dummy - dummy argument
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int kbd_rxready(int dummy)
{
    int c;

    if (initing)
	return (0);
    if (buffptr != buffend)
	return (1);

    c = kbd_get();
    if (c < 0)
	return (0);

    kbd_process(c);

#if KBD_PROCESS_DEBUG
    if ((kbd_debug & 8) != 0)
	kbd_process_debug(c);
#endif

    if (buffptr != buffend)
	return (1);
    return (0);
}

#if KBD_PROCESS_DEBUG
void kbd_process_debug(int c)
{
    int i;
    char *bptr = buffptr;
    char *bend = buffend;

    pprintf("\nkbd_process - scan code = %x(%d) ",c,c);
    i = 0;
    while (bptr != bend) {
	pprintf(" %d(%x)",i,(*bptr & 0xff));
	bptr++;
	i++;
    }
    pprintf("\n");
}
#endif

/*+
 * ============================================================================
 * = kbd_rxread - read a character from the keyboard			      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      read a character from the keyboard
 *
 * FORM OF CALL:
 *
 *	kbd_rxread (); 
 *
 * RETURNS:
 *
 *	the ASCII character
 *   
 * ARGUMENTS:
 *
 *	int dummy - dummy argument
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int kbd_rxread(int dummy)
{
    int data;
#if !TURBO
    return (*buffptr++ & 0xff);
#endif
#if TURBO
    data = (*buffptr++ & 0xff);

    if (secure_switch()) {

	/* secure switch is on; eat character; return bell. */

	return 0x07;

    } else {

	/* secure switch not on, behave normally */

	return data;
    }
#endif
}

/*+
 * ============================================================================
 * = keyboard_interrupt - Process keyboard interrupt   			      =
 * ============================================================================
 *
 * OVERVIEW:
 *      
 *      Process a keyboard interrupt
 *
 * FORM OF CALL:
 *
 *	keyboard_interrupt (); 
 *
 * RETURNS:
 *
 *      None
 *   
 * ARGUMENTS:
 *
 *	struct TTPB *ttpb - tt port block structure
 *
 * SIDE EFFECTS:
 *
 *      kbd_keyboard_flag is incremented
 *
-*/

void keyboard_interrupt(struct TTPB *ttpb)
{
    int status;

    kbd_keyboard_flag++;

#if (RAWHIDE || TURBO)
    status = io_disable_interrupts(kbd_pb, kbd_pb->pb.vector);
    if (status != msg_success)
	pprintf("keyboard_interrupt: failed to disable interrupts\n");
#else
    io_disable_interrupts(0, KEYBD_VECTOR);
#endif
}


/*+
 * ============================================================================
 * = mouse_interrupt - Process mouse interrupt   			      =
 * ============================================================================
 *
 * OVERVIEW:
 *      
 *      Process a mouse interrupt
 *
 * FORM OF CALL:
 *
 *	mouse_interrupt (); 
 *
 * RETURNS:
 *
 *      None
 *   
 * ARGUMENTS:
 *
 *	struct TTPB *ttpb - tt port block structure
 *
 * SIDE EFFECTS:
 *
 *      kbd_mouse_flag is incremented
 *
-*/

void mouse_interrupt(struct TTPB *ttpb)
{
    int status;

    kbd_mouse_flag++;

#if (RAWHIDE || TURBO)
    status = io_disable_interrupts(mouse_pb, mouse_pb->pb.vector);
    if (status != msg_success)
	pprintf("mouse_interrupt: failed to disable interrupts\n");
#else
    io_disable_interrupts(0, MOUSE_VECTOR);
#endif
}


/*+
 * ============================================================================
 * = kbd_leds - lights keyboard LEDs if necessary			      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Will light up scroll lock, num lock or caps lock LEDs on keyboard
 *	if the corresponding keys have been pressed.
 *
 * FORM OF CALL:
 *
 *	kbd_leds (); 
 *
 * RETURNS:
 *
 * 	None
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	Keyboard LEDs may light.
 *
-*/

void kbd_leds(void)
{
    int d;

    d = 0x00;
    if (scroll_lock != FALSE)
	d |= 0x01;
    if (num_lock != FALSE)
	d |= 0x02;
    if (caps_lock != FALSE)
	d |= 0x04;
    kbd_output(KBD_SETLEDS, KBD_DATA);
    kbd_output(d, KBD_DATA);
}


/*+
 * ============================================================================
 * = kbd_output - Output to the keyboard                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * FORM OF CALL:
 *
 *	kbd_output (); 
 *
 * RETURNS:
 *
 * 	int d -
 *   
 * ARGUMENTS:
 *
 *	c -
 *	port -
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int kbd_output(unsigned char c, int port)
{
    int r;
    int t;

    dprintf("kbd_output: %x %x\n", p_args2(c, port));

    while (1) {
	t = kbd_put(c, port);
	if (t < 0)
	    return (t);
	r = kbd_input();
	if (r != KBD_RESEND)
	    return (r);
    }
}


/*+
 * ============================================================================
 * = kbd_output_noretry - ??						      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 *
 * FORM OF CALL:
 *
 *	kbd_output_noretry (); 
 *
 * RETURNS:
 *
 * 	?? -
 *
 * ARGUMENTS:
 *
 *	c -
 *	port -
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int kbd_output_noretry(unsigned char c, int port)
{
    int r;
    int t;

    dprintf("kbd_output_noretry: %x %x\n", p_args2(c,port));

    t = kbd_put(c, port);
    if (t < 0)
	return (t);
    r = kbd_input();
    return (r);
}


/*+
 * ============================================================================
 * = kbd_input - Input from keyboard					      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * FORM OF CALL:
 *
 *	kbd_input (); 
 *
 * RETURNS:
 *
 * 	?? -
 *   
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int kbd_input(void)
{
    int r;
    int i;

    dprintf("kbd_input\n", p_args0);

    for (i = 0; i < 1000; i++) {
	r = kbd_get();
	if (r >= 0) {
	    dprintf("kbd_input: %x\n", p_args1(r));
	    return (r);
	}
	krn$_micro_delay(1000);
    }
    dprintf("kbd_input: -1\n", p_args0);
    return (-1);
}


/*+
 * ============================================================================
 * = kbdtt_init - connects this port to the tt class driver		      =
 * ============================================================================
 *
 * OVERVIEW:
 *      
 *      Connects kbd_driver to the tt class driver, and initializes the 
 *	keyboard and mouse device.
 *
 * FORM OF CALL:
 *
 *	kbdtt_init (); 
 *
 * RETURNS:
 *
 *      None
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
void kbdtt_init(void)
{

    struct TTPB *ttpb;
    struct INODE *ip;
    int found;
    unsigned char quick_start;

/* 
 * This code MUST be invoked after all graphics drivers. It assumes that the
 * graphics port block has been allocated if any graphics devices were found.
 * This graphics port block will be shared with this keybd driver.
 * This will avoid conflicts in both drivers trying to allocate the graphics 
 * port block at the same time. This is taken care of by file ordering in the 
 * build (platform_files.bld) file.
 */

/* The pointer to the primary console port block, console_ttpb, can be set to
 * one of three possible type ports.  
 * 1) points to a null terminal port block so exit.  This means the console 
 *    environment var is set to graphics mode and a keybd is attatched but no 
 *    graphics device was found.
 * 2) points to the serial port terminal port block.  So use a graphics
 *    port block found on the terminal port block queue.  See algorithm below.
 * 3) If neither 1) or 2) then it must point to the graphics port block chosen
 *    as the primary console so use it as the other half of the kbd port block.
 */

#if AVMIN
    quick_start = rtc_read(0x3e);
    if (quick_start == 0x31) {
	return;				/* no kbd or no graphics device found */
    }
#endif

#if MTU
    show_version();

    /* Don't spend time initializing the keyboard  if OS_TYPE is set to NT (TOY
     * 0x3f is 1).  Just return. */

    if (rtc_read(0x3f) == 1)
	return;
#endif

    if ((!keybd_present) || (console_ttpb == &null_ttpb))
	return;				/* no kbd or no graphics device found */

    if (console_ttpb != &serial_ttpb)
	ttpb = console_ttpb;		/* use primary (graphics) port */
    else {

	/*  The serial port is being used as the primary console but we need to
	 * pair the keyboard with a graphics display if we can find one. Look
	 * for the first graphics port block on the terminal port block queue. 
	 * If none found then exit since there will be no display to go with
	 * the keyboard.  */
	found = 0;
	ttpb = tt_pbs.flink;
	while (ttpb != &tt_pbs.flink) {
	    if (strstr(ttpb->ip->name, "tta") == NULL) {
		found = 1;
		break;
	    }
	    ttpb = ttpb->ttpb.flink;	/* next queue entry */
	}

	if (!found)
	    return;
    }			/* end else, console_ttpb must == &serial_ttpb */

#if AVANTI || MIKASA || ALCOR || K2 || TAKARA || APC_PLATFORM || MONET || EIGER

    /* No printouts for these platforms ... */
#else
    qprintf("initializing keyboard\n");
#endif
    kbd_keyboard_flag = 0;
    kbd_mouse_flag = 0;

#if !(KBD_POLLED)
    int_vector_set(KEYBD_VECTOR, keyboard_interrupt, ttpb);
    int_vector_set(MOUSE_VECTOR, mouse_interrupt, ttpb);

    io_enable_interrupts(0, KEYBD_VECTOR);
    io_enable_interrupts(0, MOUSE_VECTOR);
#endif

    if (!init_raw_kbd()) {
	return;
    }

    if (!init_raw_mouse()) {
	err_printf("\n** mouse error **\n");
    }
/*       
 * Disable the mouse controller.  Let the os handle the real enable.
 * If the os sees an interrupt from the mouse before the drivers
 * are started it will try to use the mouse as a terminal.  At least
 * VMS behaves this way.
 */
#if !WFSTDIO
    kbd_put(0xa7, KBD_STAT);
#endif

#if !(KBD_POLLED)
    if (!kbd_keyboard_flag) {
	err_printf("** no keyboard interrupts received **\n");
    }

    if (!kbd_mouse_flag) {
	err_printf("** no mouse interrupts received **\n");
    }
#endif

    ttpb->rxoff = kbd_rxoff;
    ttpb->rxon = kbd_rxon;
    ttpb->rxready = kbd_rxready;
    ttpb->rxread = kbd_rxread;
    ttpb->perm_mode = DDB$K_POLLED;
    ttpb->perm_poll = 0;

    /* Set the vga driver to polled mode for now */
    tt_setmode_pb(ttpb, DDB$K_POLLED);

    keyboard_established = KBD_TYPE_AT;

}


#if (RAWHIDE || TURBO)
#if STARTSHUT
/*+
 * ============================================================================
 * = kbd_mouse_init - initialize keyboard and mouse hardware                  =
 * ============================================================================
 *
 * OVERVIEW:
 *      
 *	initialize keyboard and mouse hardware
 *
 * FORM OF CALL:
 *
 *	kbd_mouse_init (); 
 *
 * RETURNS:
 *
 *      msg_success - keyboard and mouse initialized
 *	msg_failure - error in initializing keyboard and mouse
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
int kbd_mouse_init(void)
{
    int status;
    struct device *kbd_dev;
    struct device *mouse_dev;

/*******************************************************************/
/*                     Initialize keyboard                         */
/*******************************************************************/

    dqprintf("kbd_init\n");

    kbd_dev = find_dev("kbd0", &config);
    if (!kbd_dev) {
	err_printf("kbd0: no such device\n");
	return msg_failure;
    }

    if (!kbd_pb) {
	kbd_pb = malloc(sizeof(struct kbd_pb), kbd_pb);
	d2printf("malloc: kbd_pb %x %d\n", p_args2(kbd_pb, sizeof(struct kbd_pb)));
    }

/* load kbd_pb into device */

    kbd_dev->devdep.io_device.devspecific = kbd_pb;

/* load the base address from device to kbd_pb */

    kbd_pb->pb.hose = kbd_dev->hose;
    kbd_pb->pb.slot = kbd_dev->slot;
    kbd_pb->pb.bus = kbd_dev->bus;
    kbd_pb->pb.function = kbd_dev->function;
    kbd_pb->pb.channel = kbd_dev->channel;
    kbd_pb->pb.controller = kbd_dev->devdep.io_device.controller;
    kbd_pb->pb.config_device = kbd_dev;

/* load the vector from device to kbd_pb */

    kbd_pb->pb.vector = KEYBD_VECTOR;

    dqprintf("KBD h/s/b/f/c/dev/vect %d/%d/%d/%d/%d/%x/%x\n",
	kbd_pb->pb.hose,
	kbd_pb->pb.slot,
	kbd_pb->pb.bus,
	kbd_pb->pb.function,
	kbd_pb->pb.controller,
	kbd_pb->pb.config_device,
	kbd_pb->pb.vector);

/* compute pci base address */

    kbd_pci_ptr = IO_B(kbd_pb->pb.hose, 0);
    dprintf("kbd_pci_ptr = %16x\n", p_args1(kbd_pci_ptr));

    status = io_disable_interrupts(kbd_pb, kbd_pb->pb.vector);
    if (status != msg_success) {
	err_printf("KBD: failed to disable interrupts\n");
	return msg_failure;
    }

/*******************************************************************/
/*                     Initialize mouse                            */
/*******************************************************************/

    dqprintf("mouse_init\n");

    mouse_dev = find_dev("mouse0", &config);
    if (!mouse_dev) {
	err_printf("mouse0: no such device\n");
	return msg_failure;
    }

/* Use kbd_pb structure definitions for mouse */

    if (!mouse_pb) {
	mouse_pb = malloc(sizeof(struct kbd_pb), mouse_pb);
	d2printf("malloc: mouse_pb %x %d\n", p_args2(mouse_pb, sizeof(struct kbd_pb)));
    }

/* load mouse_pb into device */

    mouse_dev->devdep.io_device.devspecific = mouse_pb;

/* load the base address from device to mouse_pb */

    mouse_pb->pb.hose = mouse_dev->hose;
    mouse_pb->pb.slot = mouse_dev->slot;
    mouse_pb->pb.bus = mouse_dev->bus;
    mouse_pb->pb.function = mouse_dev->function;
    mouse_pb->pb.channel = mouse_dev->channel;
    mouse_pb->pb.controller = mouse_dev->devdep.io_device.controller;
    mouse_pb->pb.config_device = mouse_dev;

/* load the vector from device to mouse_pb */

    mouse_pb->pb.vector = MOUSE_VECTOR;

    dqprintf("MOUSE h/s/b/f/c/dev/vect %d/%d/%d/%d/%d/%x/%x\n",
	mouse_pb->pb.hose,
	mouse_pb->pb.slot,
	mouse_pb->pb.bus,
	mouse_pb->pb.function,
	mouse_pb->pb.controller,
	mouse_pb->pb.config_device,
	mouse_pb->pb.vector);

    status = io_disable_interrupts(mouse_pb, mouse_pb->pb.vector);
    if (status != msg_success) {
	err_printf("MOUSE: failed to disable interrupts\n");
	return msg_failure;
    }

/* Initialize keyboard and mouse hardware */

    dqprintf("initializing keyboard\n");

#if TURBO
    eisa_int_vector_set(kbd_pb->pb.vector, keyboard_interrupt);

    eisa_int_vector_set(mouse_pb->pb.vector, mouse_interrupt);
#endif

#if RAWHIDE
	int_vector_set (kbd_pb->pb.vector, keyboard_interrupt);
	int_vector_set (mouse_pb->pb.vector, mouse_interrupt);
#endif


    status = io_enable_interrupts(kbd_pb, kbd_pb->pb.vector);
    if (status != msg_success) {
	err_printf("** keyboard enable int error **\n");
	return msg_failure;
    }

    status = io_enable_interrupts(mouse_pb, mouse_pb->pb.vector);
    if (status != msg_success) {
	err_printf("** mouse enable int error **\n");
	return msg_failure;
    }

    if (!init_raw_kbd()) {
	err_printf("** keyboard error **\n");
	return msg_failure;
    }

    if (!init_raw_mouse()) {
	err_printf("** mouse error **\n");
    }
/*
 * Disable the mouse controller.  Let the os handle the real enable.
 * If the os sees an interrupt from the mouse before the drivers
 * are started it will try to use the mouse as a terminal.  At least
 * VMS  behaves this way.
 */
    kbd_put(0xa7, KBD_STAT);

    if (!kbd_keyboard_flag) {
	err_printf("** no keyboard interrupts received **\n");
    }

    if (!kbd_mouse_flag) {
	err_printf("** no mouse interrupts received **\n");
    }

    return (msg_success);
}
#endif
#if DRIVERSHUT
int kbd_mouse_init(void)
{
    int status;

/*******************************************************************/
/*                     Initialize keyboard                         */
/*******************************************************************/

    dprintf("kbd_init\n", p_args0);

    if ( !find_pb( "pci_eisa", 0, 0, 0, 0 )) {
	err_printf("kbd0: no such device\n");
	return msg_failure;
    }

    if (!kbd_pb) {
	kbd_pb = malloc(sizeof(struct kbd_pb), kbd_pb);
	d2printf("malloc: kbd_pb %x %d\n", p_args2(kbd_pb, sizeof(struct kbd_pb)));
    }

    kbd_pb->pb.hose = PCEB_HOSE;
    kbd_pb->pb.slot = PCEB_SLOT;
    kbd_pb->pb.bus = PCEB_BUS;
    kbd_pb->pb.function = PCEB_FUNCTION;

    kbd_pb->pb.vector = KEYBD_VECTOR;

    dqprintf("KBD h/s/b/f/vect %d/%d/%d/%d/%x\n",
	kbd_pb->pb.hose,
	kbd_pb->pb.slot,
	kbd_pb->pb.bus,
	kbd_pb->pb.function,
	kbd_pb->pb.vector);

/* compute pci base address */

    kbd_pci_ptr = IO_B(kbd_pb->pb.hose, 0);
    dprintf("kbd_pci_ptr = %16x\n", p_args1(kbd_pci_ptr));

    status = io_disable_interrupts(kbd_pb, kbd_pb->pb.vector);
    if (status != msg_success) {
	err_printf("KBD: failed to disable interrupts\n");
	return msg_failure;
    }


/*******************************************************************/
/*                     Initialize mouse                            */
/*******************************************************************/

    dprintf("mouse_init\n", p_args0);

    if ( !find_pb( "pci_eisa", 0, 0, 0, 0 )) {
	err_printf("mouse0: no such device\n");
	return msg_failure;
    }

    if (!mouse_pb) {
	mouse_pb = malloc(sizeof(struct kbd_pb), mouse_pb);
	d2printf("malloc: mouse_pb %x %d\n", p_args2(mouse_pb, sizeof(struct kbd_pb)));
    }

    mouse_pb->pb.hose = PCEB_HOSE;
    mouse_pb->pb.slot = PCEB_SLOT;
    mouse_pb->pb.bus = PCEB_BUS;
    mouse_pb->pb.function = PCEB_FUNCTION;
    mouse_pb->pb.vector = MOUSE_VECTOR;

    dqprintf("MOUSE h/s/b/f/vect %d/%d/%d/%d/%x\n",
	mouse_pb->pb.hose,
	mouse_pb->pb.slot,
	mouse_pb->pb.bus,
	mouse_pb->pb.function,
	mouse_pb->pb.vector);

    status = io_disable_interrupts(mouse_pb, mouse_pb->pb.vector);
    if (status != msg_success) {
	err_printf("MOUSE: failed to disable interrupts\n");
	return msg_failure;
    }

/* Initialize keyboard and mouse hardware */

    dqprintf("initializing keyboard\n");

    int_vector_set (kbd_pb->pb.vector, keyboard_interrupt);
    int_vector_set (mouse_pb->pb.vector, mouse_interrupt);

    status = io_enable_interrupts(kbd_pb, kbd_pb->pb.vector);
    if (status != msg_success) {
	err_printf("** keyboard enable int error **\n");
	return msg_failure;
    }

    status = io_enable_interrupts(mouse_pb, mouse_pb->pb.vector);
    if (status != msg_success) {
	err_printf("** mouse enable int error **\n");
	return msg_failure;
    }

    if (!init_raw_kbd()) {
	err_printf("** keyboard error **\n");
	return msg_failure;
    }

    if (!init_raw_mouse()) {
	err_printf("** mouse error **\n");
    }
/*
 * Disable the mouse controller.  Let the os handle the real enable.
 * If the os sees an interrupt from the mouse before the drivers
 * are started it will try to use the mouse as a terminal.  At least
 * VMS  behaves this way.
 */
    kbd_put(0xa7, KBD_STAT);

    if (!kbd_keyboard_flag) {
	err_printf("** no keyboard interrupts received **\n");
    }

    if (!kbd_mouse_flag) {
	err_printf("** no mouse interrupts received **\n");
    }

    return (msg_success);
}
#endif
#endif		/* end if (RAWHIDE || TURBO) */

/*+
 * ============================================================================
 * = init_raw_kbd - initialize the keyboard                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *      
 *      This platform specific function will initiailze the keyboard
 *	hardware. This function can be used for platforms
 *	that use the (Intel 8242) Phoenix Multikey/42 keyboard interface.
 *
 * FORM OF CALL:
 *
 *	init_raw_kbd (); 
 *
 * RETURNS:
 *
 *      1 - keyboard initialized
 *	0 - error in initializing keyboard
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

int init_raw_kbd(void)
{
    unsigned char s;
    int i;
    int t;
    int status = 0;

    dprintf("init_raw_kbd\n", p_args0);

    if (!kbd_attatched())
	return (0);

/*  Set up some global state. */

    alt_flag = 0;
    e0_seen = 0;
    f0_seen = 0;
    ctrl_flag = 0;
    shift_flag = 0;
    caps_lock = FALSE;
    num_lock = FALSE;
    scroll_lock = FALSE;
    kbd_type = 84;

/*  Enable PS/2 mode. */

    kbd_outportb(RTCA, RTCCR1);
    s = kbd_inportb(RTCD);
    kbd_outportb(RTCA, RTCCR1);
    kbd_outportb(RTCD, s & ~ATKBD);

/*  Test the keyboard. */

    kbd_put(CTL_TEST1, KBD_STAT);
    krn$_micro_delay(30000);
    t = kbd_input();
    if (t < 0) {
#if (RAWHIDE || TURBO)
	err_printf("*** keyboard test error...\n");
#else
	qprintf("*** keyboard test error...\n");
#endif
	return (0);
    }
    if (t != 0x55) {
	status = 1;
	goto INIT_RAW_KBD_ERR_EXIT;
    }
    t = kbd_output_noretry(CTL_TEST2, KBD_STAT);
    if (t != 0x00) {
	status = 2;
	goto INIT_RAW_KBD_ERR_EXIT;
    }
    t = kbd_output_noretry(CTL_ENABLE, KBD_STAT);
    while (t >= 0)
	t = kbd_input();

/*  Reset the keyboard. */

    for (i = 0; i < 10; i++) {
	t = kbd_output_noretry(CTL_RESET, KBD_DATA);
	if (t == KBD_ACK)
	    break;
	kbd_input();
    }
    if (i == 10) {
	status = 3;
	goto INIT_RAW_KBD_ERR_EXIT;
    }
    kbd_input();

/*  Turn on keyboard and mouse interrupts. */

    t = kbd_output_noretry(CTL_RDMODE, KBD_STAT);

    if (t >= 0) {
	kbd_put(CTL_WRMODE, KBD_STAT);
	t &= ~(KBD_SYS | KBD_DKB | KBD_DMS | KBD_KCC);
	t |= (KBD_EKI | KBD_EMI);
	kbd_put(t, KBD_DATA);
    }

/*
 * Assume that we have a 101-key keyboard which has powered up in mode 2  
 * (the wrong mode).  Switch it into mode 3.  If we really have an 84-key  
 * keyboard, then the first byte will be ACK'ed as a NOP, and the second
 *  byte will be RESEND'ed as garbage.  On the 101-key keyboard we also
 *  need to reset the mode of the right ALT and CTRL keys.
 */
    t = kbd_output(KBD_SELECTCODE, KBD_DATA);

    if (t != KBD_ACK) {
	if (t == KBD_RESEND) {
	    return (1);			/* success */

	} else {
	    status = 4;
	    goto INIT_RAW_KBD_ERR_EXIT;
	}
    }

    t = kbd_output_noretry(0x03, KBD_DATA);
    if (t == KBD_ACK) {
	if (kbd_output(KBD_DEFAULTS, KBD_DATA) != KBD_ACK) {
	    status = 5;
	    goto INIT_RAW_KBD_ERR_EXIT;
	}
	if (kbd_output(KBD_MAKEBREAK, KBD_DATA) != KBD_ACK) {
	    status = 6;
	    goto INIT_RAW_KBD_ERR_EXIT;
	}
	if (kbd_output(0x39, KBD_DATA) != KBD_ACK) {
	    status = 7;
	    goto INIT_RAW_KBD_ERR_EXIT;
	}
	if (kbd_output(KBD_MAKEBREAK, KBD_DATA) != KBD_ACK) {
	    status = 8;
	    goto INIT_RAW_KBD_ERR_EXIT;
	}
	if (kbd_output(0x58, KBD_DATA) != KBD_ACK) {
	    status = 9;
	    goto INIT_RAW_KBD_ERR_EXIT;
	}
	kbd_type = 101;
    }

/*  Enable the keyboard. */

    t = kbd_output(KBD_ENABLE, KBD_DATA);
    if (t != KBD_ACK) {
	status = 10;
	goto INIT_RAW_KBD_ERR_EXIT;
    } else {
#if (RAWHIDE || TURBO)
	dqprintf("kbd test passed\n");
#endif
	return (1);			/* success */
    }

INIT_RAW_KBD_ERR_EXIT: 
#if (RAWHIDE || TURBO)
    err_printf("kbd test %d failed\n", status);
#else
    qprintf("kbd test %d failed\n", status);
#endif
    return (0);
}


/* ============================================================================
 * = kbd_attatched - Check to see if the keyboard is attatched		      =
 * ============================================================================
 *
 * OVERVIEW:
 *      
 *      Check to see if the keyboard is attatched.
 *
 * FORM OF CALL:
 *
 *	kbd_attatched (); 
 *
 * RETURNS:
 *
 *      1	- keyboard is attatched
 *      0	- keyboard is not attatched or there is a recurring parity
 *		  error.
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

int kbd_attatched(void)
{
    unsigned char s;
    int i;
    int t;

#if AVMIN
    unsigned char quick_start;

    quick_start = rtc_read(0x3e);
    if (quick_start == 0x31) {
	return (0);			/* no kbd or no graphics device found
					 */
    }
#endif

    dprintf("kbd_attatched\n", p_args0);

/*
** Send 0xAA test pattern to wake up keyboard/mouse controller
** on Cypress CY82C693.
*/
    kbd_put(CTL_TEST1, KBD_STAT);
    krn$_micro_delay(30000);
    t = kbd_input();
    if (t != 0x55) {
	dqprintf("*** keyboard wake up error...\n");
	return (0);
    }

/*
 * Enable the keyboard, and check for parity errors.  If one is found,
 * reset the keyboard and try again.  Give up if it can't be done without 
 * error.
 */
    kbd_outportb(KBD_DATA, KBD_ENABLE);
    for (i = 0; i < 5; i++) {
	s = kbd_inportb(KBD_STAT);
	if (s & KBD_PERR)
	    kbd_output_noretry(CTL_RESET, KBD_DATA);
	else if (s & KBD_OBF)
	    kbd_get();
	else
	    break;
    }

    if (i == 5) {
	dqprintf("*** Keyboard parity error...\n");
	return (0);
    }

/*  Test the keyboard. */

    for (i = 0; i < 5; i++) {
	t = kbd_output_noretry(CTL_RESET, KBD_DATA);
	if (t == KBD_ACK)
	    break;
	kbd_input();
    }
    if (i == 5) {
#if !MONET && !SHARK
	dqprintf("*** keyboard not plugged in...\n");
#endif
	return (0);
    }
    kbd_input();

    keybd_type = 1;			/* PS/2 keyboard present */
    dprintf("kbd is attatched\n", p_args0);
    return (1);				/* keyboard is attatched */
}


/*+
 * ============================================================================
 * = init_raw_mouse  - initialize the mouse				      =
 * ============================================================================
 *
 * OVERVIEW:
 *      
 *      This platform specific function will initiailze the mouse
 *	hardware.
 *
 * FORM OF CALL:
 *
 *	init_raw_mouse (); 
 *
 * RETURNS:
 *
 *      1 - mouse initialized
 *	0 - error in initializing mouse
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

int init_raw_mouse(void)
{
    int i;
    int t;
    int status = 0;

    dprintf("init_raw_mouse\n", p_args0);

/*  Enable the mouse. */

    t = mouse_put(0xa8, KBD_STAT);
    if (t < 0) {
	status = 1;
	goto INIT_RAW_MOUSE_ERR_EXIT;
    }
    t = mouse_put(0xd4, KBD_STAT);
    if (t < 0) {
	status = 2;
	goto INIT_RAW_MOUSE_ERR_EXIT;
    }
    t = mouse_put(0xf4, KBD_DATA);
    if (t < 0) {
	status = 3;
	goto INIT_RAW_MOUSE_ERR_EXIT;
    }
    for (i = 0; i < 100; i++) {
	t = mouse_get();
	if (t == KBD_ACK)
	    break;
	krn$_micro_delay(10000);
    }

    if (t != KBD_ACK) {
	status = 4;
	goto INIT_RAW_MOUSE_ERR_EXIT;
    }

#if (RAWHIDE || TURBO)
    dqprintf("mouse test passed\n");
#else
    dprintf("mouse test passed\n", p_args0);
#endif
    return (1);

INIT_RAW_MOUSE_ERR_EXIT:
#if (RAWHIDE || TURBO)
    err_printf("mouse test %d failed\n", status);
    return (1);			/* Don't care if mouse fails */
#else
    dprintf("mouse test %d failed\n", p_args1(status));
    return (0);
#endif
}


/*+
 * ============================================================================
 * = kbd_reinit - Reinit the keyboard					      =
 * ============================================================================
 *
 * OVERVIEW:
 *      
 *
 * FORM OF CALL:
 *
 *	kbd_reinit (); 
 *
 * RETURNS:
 *
 *      None
 *   
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      Keyboard is reinited...
 *
-*/

void kbd_reinit(void)
{
#if (RAWHIDE || TURBO)
    qprintf("kbd_reinit\n");
#endif

#if USB_SUPPORT
    if ( keyboard_established == KBD_TYPE_USB ) {
	usb_reinit();
    } else
#endif
    {
	initing = 1;
	init_raw_kbd();
	initing = 0;
    }
}

#if !MODULAR
/*+
 * ============================================================================
 * = fill_in_ctb_ws_slotinfo - write bus type, bus number & slot number      =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	This routine fills in the ctb type 4 inslotinfo field for the HWRPB.
 *
 *  FORM OF CALL:
 *
 *	fill_in_ctb_ws_slotinfo (); 
 *
 *  RETURNS:
 *
 *	None.
 *
 *  ARGUMENTS:
 *
 *	int bus_type - pci, eisa or isa
 *	int hose - platform-specific hose number
 *	int bus - platform-specific bus number
 *	int slot - slot number of device
 *
 *  SIDE EFFECTS:
 *
 *	inslotinfo field of ctb_type_4 struct in HWRPB is filled in
 *
 *--
 */
void fill_in_ctb_ws_slotinfo(int bus_type, int hose, int bus, int slot)
{
    CTB_WS_tab[0].TERM_OUT_LOC = CTB_WS_tab[0].TERM_IN_LOC =
      ((bus_type << 16) | (hose << 24) | (bus << 8) | slot);
    build_ctb(hwrpb, 0);
}
#endif

#if (RAWHIDE || TURBO)
/*+
 * ============================================================================
 * = primary_console_sel - primary console determination & setup              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Connects kbd_driver to the tt class driver.
 *	
 * 	Determine if this device is to be used as the primary console terminal
 * 	and if so then do whatever needs to be done to make this device the
 * 	primary console.
 *
 *	It assumes that the graphics port block has been allocated if any
 *	graphics devices were found.
 *	This graphics port block will be shared with this keybd driver.
 *	This will avoid conflicts in both drivers trying to allocate the
 *	graphics port block at the same time.
 *
 *	This function assumes that only one PCI graphics device is allowed.
 *	
 * 	This routine should be called by the graphics driver but should be 
 *	called only after/if the driver has located a device that it supports.
 *	
 *	The pointer to the primary console port block, console_ttpb, can be
 *	set to one of three possible type ports.  
 *	1) points to a null terminal port block so exit.  This means the
 *	console environment var is set to graphics mode and a keybd is
 *	attatched but no graphics device was found.
 *	2) points to the serial port terminal port block.  So use a graphics
 *	port block found on the terminal port block queue.  See algorithm below.
 *	3) If neither 1) or 2) then it must point to the graphics port block
 *	chosen as the primary console so use it as the other half of the kbd
 *	port block.
 *
 * FORM OF CALL:
 *
 *	primary_console_sel (*ttpb)
 *  
 * RETURNS:
 *
 * 	msg_success 	- graphics selected
 * 	msg_failure	- no keyboard or error
 *       
 * ARGUMENTS:
 *
 *	TTPB *ttpb	- pointer to terminal port block of graphics driver
 *
 * SIDE EFFECTS:
 *
 *	none.
 *
-*/

int primary_console_sel(struct TTPB *ttpb, int bus_type, int hose, int bus, int slot)
{
    struct PCB *pcb;
    struct EVNODE *evp, evn;		/* Pointer to the evnode,evnode */

    dprintf("primary_console_sel\n", p_args0);

    ttpb->rxoff = kbd_rxoff;
    ttpb->rxon = kbd_rxon;
    ttpb->rxready = kbd_rxready;
    ttpb->rxread = kbd_rxread;
    ttpb->perm_mode = DDB$K_POLLED;
    ttpb->perm_poll = 0;

/* Set the kbd driver to polled mode for now */

    tt_setmode_pb(ttpb, DDB$K_POLLED);

/* Set the control flags for the primary console */

    graphics_console = 0;
    both_console = 0;

/*
 * This terminal may be the intended primary, keybd may be
 * unintentionally disconnected, so display warning message on this
 * terminal.
 */
    if (!keybd_present) {
	err_printf(msg_no_kbd_use_serial);
	return msg_failure;
    }

    evp = &evn;
    if (ev_read("console", &evp, 0) == msg_success) {

	if ((evp->value.string[0] == 's') || (evp->value.string[0] == 'S')) {
	    ;				/* SERIAL */
	}
	if ((evp->value.string[0] == 'b') || (evp->value.string[0] == 'B')) {
	    both_console = 1;		/* BOTH */
	}
	if ((evp->value.string[0] == 'g') || (evp->value.string[0] == 'G')) {
	    graphics_console = 1;	/* GRAPHICS */
	}
    }

    if (graphics_console) {

	printf("Console in graphics mode\n");

	console_ttpb = ttpb;	/* designate this terminal as primary */

	pcb = getpcb ();

	fclose (pcb->pcb$a_stdin);
	fclose (pcb->pcb$a_stdout);
	fclose (pcb->pcb$a_stderr);

	pcb->pcb$a_stdin_name =  "tt";
	pcb->pcb$a_stdout_name = "tt";
	pcb->pcb$a_stderr_name = "tt";

	pcb->pcb$a_stderr = fopen (pcb->pcb$a_stderr_name,
	    pcb->pcb$b_stderr_mode);
	pcb->pcb$a_stdout = fopen (pcb->pcb$a_stdout_name,
	    pcb->pcb$b_stdout_mode);
	pcb->pcb$a_stdin = fopen (pcb->pcb$a_stdin_name,
	    pcb->pcb$b_stdin_mode);

	CTB_units[0] = CTB$K_DZ_WS;
        CTB_WS_tab[0].TERM_OUT_LOC = (bus_type << 16) | (hose << 24) | (bus << 8) | slot;
	CTB_WS_tab[0].TERM_IN_LOC = CTB_WS_tab[0].TERM_OUT_LOC;
    }

    return msg_success;
}
#endif

/*+
 * ============================================================================
 * = kbd_process - translate scan codes into ASCII characters		      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  	translates scan codes into ASCII characters, and checks to see if the
 *	scan code will translate into something valid.  Supports 84 and 101
 * 	key keyboards.
 *
 * FORM OF CALL:
 *
 *	kbd_process (); 
 *
 * RETURNS:
 *
 * 	None
 *   
 * ARGUMENTS:
 *
 *	int scan_code -	scan code read from kbd data buffer
 *
 * SIDE EFFECTS:
 *
-*/

void kbd_process(int scan_code)
{
    dprintf("kbd_process: scan_code = %02x", p_args1(scan_code));

    /* See if key was released. */
    if (scan_code == 0xE0) {
	e0_seen = 1;
	return;
    }
    if (scan_code == 0xF0) {
	f0_seen = 1;
	return;
    }

    if (e0_seen)
	return;
    if (kbd_type == 84) {
	scan_code = kbd_84to101_table[scan_code];
    }

    /*Take care of special cases first.*/
    switch (scan_code) {
	case RSHIFT: 
	case LSHIFT: 
	    if (f0_seen)		/* Shift key was released. */
	      {
		shift_flag = 0;
		f0_seen = 0;
	    } else
		shift_flag = 1;
	    return;
	case RALT: 
	case LALT: 
	    if (f0_seen)		/* ALT key was released. */
	      {
		alt_flag = 0;
		f0_seen = 0;
	    } else {
		alt_flag = 1;
	    }
	    return;
	case RCTRL: 
	case LCTRL: 
	    if (f0_seen)		/* CTRL key was released. */
	      {
		ctrl_flag = 0;
		f0_seen = 0;
	    } else
		ctrl_flag = 1;
	    return;
	case CAPS: 
	    if (f0_seen)
		f0_seen = 0;
	    else {
		if (caps_lock == 0)
		    caps_lock = 1;
		else
		    caps_lock = 0;
		kbd_leds();
	    }
	    return;
	case NUMLOCK: 

	    if (kbd_hardware_type == LK411 || arc_fe_inited)
		break;
	    if (num_lock)
		num_lock = 0;
	    else
		num_lock = 1;
	    kbd_leds();
	    return;

	case F1: 
        case 0x09:

	    /* The japanese keyboards use the value of 0x09 for the F1 key;
	     * so change the code to the real F1 key for these languages 
	     * else break out so that 0x09 key can be translated properly. */
	    if (scan_code == 0x09) {
		if ((kbd_lang_array == ((LANG_K_JAPANESE_JJ-LANG_K_DANSK) >> 1)) ||
		  (kbd_lang_array == ((LANG_K_JAPANESE_AJ-LANG_K_DANSK) >> 1)))
		    scan_code = F1;
		else
		    break;
	    }

	    /* fall into multi_byte translation for arc below. */
	    if (arc_fe_inited) {
		break;
	    }

	    if (scroll_lock) {
		scroll_lock = 0;
		ibuff[0] = XON;
	    } else {
		scroll_lock = 1;
		ibuff[0] = XOFF;
	    }
	    buffend = &ibuff[1];
	    buffptr = ibuff;
	    kbd_leds();
	    return;
	case S_SCANCODE: 
	    if (!ctrl_flag)
		break;
	    scroll_lock = 1;
	    ibuff[0] = XOFF;
	    buffend = &ibuff[1];
	    buffptr = ibuff;
	    kbd_leds();
	    return;
	case Q_SCANCODE: 
	    if (!ctrl_flag)
		break;
	    scroll_lock = 0;
	    ibuff[0] = XON;
	    buffptr = ibuff;
	    kbd_leds();
	    return;

    }

/*
 * If you made it through the special cases and f0 is still set, then this 
 * is probably a BURNS (laptop), where every key generates a make and
 * break.  This may be because it's in mode 2.  Anyway, just clear the f0
 * flag.
*/
    if (f0_seen) {
	f0_seen = 0;
	return;
    }

/*
 * If not special case, then go look for scan_code in tables. 
 * Look in single-byte table first, because most codes will be there.
*/
    if (!translate_single_byte_code(scan_code)) {
	if (!translate_mixed_byte_code(scan_code)) {
	    if (!translate_multi_byte_code(scan_code)) {

		dqprintf("\nERROR: scancode 0x%x not supported on", scan_code);
		if (kbd_hardware_type == PCXAL)
		    dqprintf(" PCXAL\n");
		else if (kbd_hardware_type == LK411)
		    dqprintf(" LK411\n");
		else
		    dqprintf(" UNKNOWN KBD\n");
	    }
	}
    }

    dprintf("ibuff[0] = 0x%x\n", p_args1(ibuff[0]));

}


/*+
 * ============================================================================
 * = translate_single_byte_code -  translates all single-byte keys.	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *   Search through all tables that have single-byte translations in them.
 *
 *
 * FORM OF CALL:
 *
 *      translate_single_byte_code (scan_code);
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      int scan_code - scan code read from kbd data buffer
 *
 * SIDE EFFECTS:
 *
-*/

int translate_single_byte_code(int scan_code)
{
    int found = 0;

    if (alt_flag) {
	if ((ibuff[0] = get_alt_code(scan_code)) != 0) {
	    buffend = &ibuff[1];
	    buffptr = (char *) &ibuff;
	}
	return 1;
    }

    /*Check main table*/
    if (search_main_table(scan_code))
	++found;
/* dave */
/*    else if ( search_number_pad_table ( scan_code ) )
      ++found ;
*/

    return found;
}


/*+
 * ============================================================================
 * = translate_mixed_byte_code -  translates both single and multi byte keys. =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *   Search through number pad table that has both single and multi byte
 *   translations in it.
 *
 *
 * FORM OF CALL:
 *
 *      translate_mixed_byte_code (scan_code);
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      int scan_code - scan code read from kbd data buffer
 *
 * SIDE EFFECTS:
 *
-*/
int translate_mixed_byte_code(int scan_code)
{
    int found = 0;

    if (search_number_pad_table(scan_code)) {
	++found;
    }

    return (found);
}

int search_main_table(int scan_code)
{
    int case_t = LOWERCASE;
    int status = 0;

    if (shift_flag)
	case_t = UPPERCASE;

    /*Check main tranlation table.*/
    if (find_single_byte_code(scan_code, &xlate_table[0], case_t))
	++status;

    /*Check language and kbd specific tranlation table.  If something is*found 
     * here it will just override what was found in the main table. */
    if (find_single_byte_code(scan_code,
      xlate_diffs[kbd_hardware_type][kbd_lang_array], case_t))
	++status;

    /* If something was found, then check caps_lock and translate possible 
     * control codes. */
    if (status) {

        if (caps_lock && ibuff[0] >= 'a' && ibuff[0] <= 'z') ibuff[0] -= 0x20;

	translate_control_codes();
	return 1;
    }
    return 0;
}

int search_number_pad_table(int scan_code)
{
    int status = 0;
    int case_t = LOWERCASE;

    /* The PFx keys are special cases.  On the LK411 keyboard, they generate
     * multi-byte sequences, so force this to fail, and then go get the LK411
     * translations in the multi-byte tables. Also for this for ARC */
    if (kbd_hardware_type == LK411 || arc_fe_inited ) {
	if ((scan_code == PF1_SCANCODE) || (scan_code == PF2_SCANCODE) ||
	  (scan_code == PF3_SCANCODE) || (scan_code == PF4_SCANCODE))
	    return 0;
    }

    if (num_lock)
	case_t = UPPERCASE;
    if (find_single_byte_code(scan_code, &common_number_pad_xlate_table[0],
      case_t)) {
	return 1;
    } else {
	if (kbd_hardware_type == LK411) {
	    if (find_single_byte_code(scan_code,
	      &lk411_number_pad_xlate_table[0], case_t)) {
		return 1;
	    }
	} else				/* pcxal keyboard */
	  {
	    if (find_multi_byte_keypad_code(scan_code,
	      &pcxal_keypad_xlate_table[0], case_t)) {
		++status;
	    }
/*
 * The Dansk, Deutsch (Deutschland/Osterreich), Portugues, and Suomi
 * keyboard use different symbols for the top row on the keypad.
*/
	    if ((kbd_lang_array == 0x1) || (kbd_lang_array == 0x2) ||
	      (kbd_lang_array == 0xd) || (kbd_lang_array == 0xe)) {
		if (find_multi_byte_keypad_code(scan_code,
		  &diff_pc_keypad_xlate_table[0], case_t)) {
		    ++status;
		}
	    }
	    if (status) {
		return (1);
	    }
	}
    }
    return 0;
}


/*+
 * ============================================================================
 * = find_single_byte_code -  Search through a specific table.		      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  All of the tables have the same structure.  This routine will search
 *  through whatever table you give it looking for a specific scancode.
 *
 *
 * FORM OF CALL:
 *
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *
 * SIDE EFFECTS:
 *
-*/
int find_single_byte_code(int scan_code, struct xlate_t *ptr, int case_t)
{
    int i;

    i = 0;
    while (ptr[i].scancode != 0xff) {
	if (ptr[i].scancode == scan_code) {
	    if (case_t == UPPERCASE)
		ibuff[0] = ptr[i].shifted;
	    else
		ibuff[0] = ptr[i].unshifted;
	    buffptr = ibuff;
	    buffend = &ibuff[1];
	    return 1;
	}
	++i;
    }
    return 0;
}


/*+
 * ============================================================================
 * = find_multi_byte_keypad_code -  Search through a pcxal keypad table.      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  Will search through the pcxal keypad table setting up the proper
 *  response depending if num lock is set or not.
 *
 *
 * FORM OF CALL:
 *
 *  find_multi_byte_keypad_code(scancode, &pcxal_keypad_xlate_table[0], case_t);
 *
 *
 * RETURNS:
 *
 *      1 if successful, 0 if not
 *
 * ARGUMENTS:
 *
 *	scancode - value of key depressed
 *	pcxal_keypad_xlate_table - starting address of table for pcxal keypad
 * 	case_t - upper case is equivalent to numlock being depressed
 *
 * SIDE EFFECTS:
 *
-*/
int find_multi_byte_keypad_code(int scan_code, struct keypad_xlate *ptr,
  int case_t)
{
    int i, j;

    i = j = 0;

    while (ptr[i].scancode != 0xff) {
	if (ptr[i].scancode == scan_code) {
	    if (case_t != UPPERCASE) {
		while (ptr[i].value[j] != 0) {
		    ibuff[j] = ptr[i].value[j];
		    ++j;
		}
		buffptr = ibuff;
		buffend = &ibuff[j];
		return (1);
	    } else {

		/* uppercase equals numlock */
		ibuff[0] = ptr[i].numlock;
		buffptr = ibuff;
		buffend = &ibuff[1];
		return (1);
	    }
	}				/* if scancode */
	++i;
    }					/* while scancode != 0xff */
    return (0);
}


/*+
 * ============================================================================
 * = translate_control_codes -  translates all control sequences.	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *      translate_control_codes ();
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
-*/
void translate_control_codes(void) 
{
    if (ctrl_flag) {
	if ((ibuff[0] >= 'A') && (ibuff[0] <= 'Z'))
	    ibuff[0] = ibuff[0] - 0x40;
	if ((ibuff[0] >= 'a') && (ibuff[0] <= 'z'))
	    ibuff[0] = ibuff[0] - 0x60;
	else if ((ibuff[0] == '2') || (ibuff[0] == ' '))
	    ibuff[0] = 0x00;		/* NUL */
	else if ((ibuff[0] == '3') || (ibuff[0] == '['))
	    ibuff[0] = 0x1b;		/* ESC */
	else if ((ibuff[0] == '4') || (ibuff[0] == '/'))
	    ibuff[0] = 0x1c;		/* FS */
	else if ((ibuff[0] == '5') || (ibuff[0] == ']'))
	    ibuff[0] = 0x1d;		/* GS */
	else if ((ibuff[0] == '6') || (ibuff[0] == '~'))
	    ibuff[0] = 0x1e;		/* RS */
	else if ((ibuff[0] == '7') || (ibuff[0] == '?'))
	    ibuff[0] = 0x1f;		/* US */
	else if ((ibuff[0] == '8'))
	    ibuff[0] = 0x7f;		/* DEL */
    }
}


/*+
 * ============================================================================
 * = translate_multi_byte_code -  translates all muti-byte keys.	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *      translate_multi_byte_code (scan_code);
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      int scan_code - scan code read from kbd data buffer
 *
 * SIDE EFFECTS:
 *
-*/

int translate_multi_byte_code(int scan_code)
{
    if (find_multi_byte(scan_code, &common_multi_byte_xlate_table[0]))
	return 1;
    /* For ARC, translate the upper number keys like LK411. */
    if (find_multi_byte(scan_code,
      specific_multi_byte_xlate_table[
      (arc_fe_inited) ? (LK411) : (kbd_hardware_type)]))
	return 1;
    return 0;
}

int find_multi_byte(int scan_code, struct multi_byte_xlate *ptr)
{
    int i, j;

    i = j = 0;
    while (ptr[i].scancode != 0xff) {
	if (ptr[i].scancode == scan_code) {
	    while (ptr[i].value[j] != 0) {
		ibuff[j] = ptr[i].value[j];
		++j;
	    }
	    buffptr = ibuff;
	    buffend = &ibuff[j];
	    return 1;
	}
	++i;
    }
    return 0;
}


/*+
 * ============================================================================
 * = get_alt -  translates keys that are pressed in conjuction w/ALT .        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *
 *      get_alt (scan_code);
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      int scan_code - scan code read from kbd data buffer
 *
 * SIDE EFFECTS:
 *
-*/

unsigned char get_alt_code(int scan_code)
{
    struct alt_diffs *adp;
    int case_t = LOWERCASE;

    if (shift_flag)
	case_t = UPPERCASE;

    adp = alt_table[kbd_hardware_type][kbd_lang_array];
    while (adp->scancode != 0) {
	if (scan_code == adp->scancode) {
	    if (LOWERCASE == case_t)
		return (adp->unshifted);
	    else
		return (adp->shifted);
	}
	++adp;
    }

    /*Scancode didn't match anything in table*/
    return (0);
}

#if DEBUG_MALLOC
int dmalloc(int size, char *sym)
{
    register addr;

    addr = dyn$_malloc(size, DYN$K_SYNC | DYN$K_NOOWN);
    d2printf("malloc: %s %x,%d\n", p_args3(sym, addr, size));
    return addr;
}

int dmalloc_opt(int size, int opt, int mod, int rem, int zone, char *sym)
{
    register addr;

    addr = dyn$_malloc(size, opt, mod, rem);
    d2printf("malloc_opt: %s %x,%d\n", p_args3(sym, addr, size));
    return addr;
}

void dfree(int ptr, char *sym)
{
    dyn$_free(ptr,DYN$K_SYNC);
    d2printf("free: %s %x\n", p_args2(sym, ptr));
}
#endif

#if DEBUG
void _dprintf(char *fmt, protoargs int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
    if ((kbd_debug & 1) != 0) {
	printf(fmt, a, b, c, d, e, f, g, h, i, j);
	kbd_reg_debug = 0;
    }
}
void _d2printf(char *fmt, protoargs int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
    if ((kbd_debug & 2) != 0)
	printf(fmt, a, b, c, d, e, f, g, h, i, j);
}
void _d4printf(char *fmt, protoargs int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
    if ((kbd_debug & 4) != 0) {
	kbd_reg_debug++;
	if (kbd_reg_debug < 20)
	    printf(fmt, a, b, c, d, e, f, g, h, i, j);
    }
}
void _dqprintf(char *fmt, protoargs int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
    if (kbd_debug)
	_dprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    else
	qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
}
#endif

