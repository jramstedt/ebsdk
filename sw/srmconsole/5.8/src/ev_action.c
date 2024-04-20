/* file:	ev_action.c
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
 *
 *
 * Abstract:	This module contains the validation and action routines
 *		for pre-defined environment variables.
 *
 * Author:	David Mayo
 *
 * Modifications:
 *
 *      mar     19-Jun-2000	Add sys_com1_rmc ev which is used to 
 *				enable/disable internal system COM1 
 *				(e.g. SRM Console, OS, application) 
 *                              access to the RMC. This solve the "RMC parser 
 *				in use" problem. See regatta_rmc_routines.c and 
 *				clipper_dpr_def.sdl
 *	mdr	16-Mar-2000	Falcon: 1) add thermal_override
 *					2) Default os_type to UNIX
 *					3) put rtc NVRAM boot EV's in flash not
 *					   not using ev_driver.  No battery on
 *					   Falcon for RTC device.
 *	jwj	02-Mar-2000	Add new environment variable rmc_halt for webbrick.  This will
 *				enable/disable a check for the rmc issued halt request.  See
 *				halt_switch_in() in webbrick.c.
 *	jwj	14-Sep-1999	Correct the order of the environment variables in env_table[]
 *				so that it matches ENV_VAR_TABLE:: in callbacks_alpha.mar.  
 *				This allows callbacks to works properly.
 *	jrk	21-Jul-1999	Added "usb_enable" for MONET.
 *
 *	JWJ	25-Mar-1999	add com1_mode for WEBBRICK and conditionalize for WEBBRICK	
 *	gdm	4-Feb-1999	Conditionalized for DMCC Eiger platform
 *
 *	tna	4-Jan-1999	Conditionalized for Yukona
 *
 *      mar	24-Dec-1998     ev_os_type() - cleanup by removing AVMIN, IMP
 *				preprocessor directives, #if 0 section that
 *				mucks up the year
 *      mar	04-Jun-1998     Eco 1230 - add srm2ctrl environment variable.
 *				This ev is invoked via callback-by-name only.
 *
 *      mar	15-Oct-1997     Add ability to expand SRM specifier to display 
 *				mop and ethernet address. This is part of the
 *				multicast ability requested by OpenVMS. See
 *				ev_write_dev().
 *
 *	gdm	28-feb-1997	Add ocp_text, psu_sense and new defaults for Takara and K2
 *
 *	sm	 9-jul-1996	Conditionalise for Takara.
 *
 *	tfm/dm	26-Jun-1996	ECO 1049 - HEAP_EXPAND ev for CIPCA
 *
 *      rhb     18-Jun-1996     ECO 1045, added controlp EV to AVANTI, MIKASA,
 *                              and PINNACLE
 *
 *      twp     25-Mar-1996     Include tt_allow_login code for Rawhide.
 *
 *	jcc	21-Feb-1996	Conditionalize for Cortex
 *
 *	dch     12-Feb-1996	Medulla: added set pci_pcmcia_size EV
 *				for allowing pci->pcmcia bus adapters the 
 *				flexibilty of mapping an appropriate window
 *				in either dense or sparse pci memory.
 * 
 *	noh	09-nov-1995	BURNS: Added set srom_mini command.
 *
 *	al	6-sep-1995	Add kbd_hardware_type EV.
 *
 *	jrk	24-jul-1995	remove turbo stuff
 *
 *	smcg	 5-jul-1995	turn on probe algoritm for K2
 *
 *	dtr	29-jun-1995	turn on bus probe algorithm for avanti/mustang
 *
 *	swt	28-Jun-1995	add Noritake platform.
 *
 *	jjd	23-Jun-1995	OSF is now called UNIX, for OS_TYPE.
 *                              OSF is a valid synonym.
 *
 *	noh	21-Jun-1995	AVMIN: Changed handling of "quick_start".
 *				Added "quick_start" read routine.
 *
 *	dtr	20-jun-1995	added date checking to set os_type.  To many
 *				complaints about the date not being setup 
 *				properly when switching from nt to osf/vms.  
 *				Added the same code used on jensen.  This also
 *				effects kernel.c.
 *
 *	rbb	01-Jun-1995	Added bus_probe_algorithm support for EB164
 *
 *	noh	19-May-1995	Added "quick_start" environment variable for
 *				AVMIN.  This skips memory testing and does 
 *				not start the VGA bios emulator.
 *
 *	jha	20-Apr-1995	Add support for environment variable selectable
 *				bus probe algorithm on MIKASA.
 *
 *	rbb	13-Mar-1995	Conditionalize for EB164
 *
 *	pas	10-feb-1995	Sable: make tt_allow_login non-volatile
 *
 *	dch	 13-Jan-1995	Medulla Only: added pci_bcd_int_select.
 *
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	dtr	16-Nov-1994	APS changes - force the console to graphics 
 *				when os_type is set to nt.  This is needed so
 *				that the flash update code will work from nt.
 *
 *	bobf	12-Oct-1994	Medulla Only: Removed range check of 
 *				all VME ev's.
 *
 *	mar	09-sep-1994	Change ocp_text_default to Model 4/200
 *                
 *	pas	30-Aug-1994	add in ocp_text change gen=27 to gen=28
 *
 *	mar	26-Aug-1994	Sable: Name change to ocp_text_default
 *
 *	bobf	22-Aug-1994	Medulla Only: Changed 'osflash' to 'userflash'
 *
 *	er	15-Aug-1994	Conditionalize for SD164
 *
 *      dpm	12-Aug-1994	Added the bootp definitions for avanti/mustang
 *
 *	dwb	13-Jul-1994	Changed the default for PCI_PARITY from 'on'
 *				to 'off'
 *
 *	bobf	 7-Jul-1994	Added tt_baud for Medulla platform.
 *
 *	er	23-Jun-1994	Conditionalize for EB64+ NVRAM.
 *
 *	dch	 7-Jun-1994	added pci_park_dev for Medulla.
 *
 *	dch	 24-MAY-1994    added pci_arb_mode for Medulla.
 *
 *	dch	 19-MAY-1994    added vme_a32_base
 *			              vme_a24_base
 *				      vme_a16_base
 *				      vme_a32_size 
 *                                    vme_a24_size for Medulla.
 *				     
 *	dtr	9-May-1994	Enabled all of the workdation platforms on the
 *				keyboard language code.
 *	
 *	dtr(brown) 29-Apr-1994	add in lean support and set up for the merge
 *
 *	bobf	14-Mar-1994	Medulla Change Only: changed 'elx_bootline' to
 *				'vx_bootline'
 *
 *	jrk	22-Feb-1994	fix interleave #endif bug
 *			
 *	dpm	3-Feb-1994	Added os selection for MUSTANG
 *                                                                
 *	mdr	21-Jan-1994	relocate elx_bootline and vme_config; add
 *				ncr2_setup for medulla.
 *
 *	mc	28-Dec-1993	Changed default OCP message
 *
 *	dtr	16-Dec-1993	Lean support
 *
 *	mdr	 15-Dec-1993	added elx_bootline and vme_config for Medulla.
 *
 *	mc	 08-Dec-1993	added ocp_text and ocp_text_default for Sable.
 *
 *	mdr	 29-Sep-1993	Conditionalize for Medulla's NVRAM.
 *
 *	dwr	 16-Sep-1993	Add CPU ev for TURBO
 *
 *      pel	 24-Aug-1993    Conditionalize for morgan to support nvram.
 *				  The following are Morgan non-volatile:
 *				auto_action, bootdef_dev, boot_file, 
 *				boot_osflags, boot_reset, dump_dev, enable_audit
 *				language, sys_serial_num, era0_def_inetaddr
 *				era0_def_ginetaddr, era0_def_sinetaddr
 *				era0_def_inetfile, era0_inet_init
 *				era0_bootp_server, era0_bootp_file, 
 *				era0_protocols, era0_bootp_tries
 *				era0_arp_tries, era0_tftp_tries
 *
 *      pel      23-Jun-1993    Include tt_allow_login code for Morgan.
 *
 *	phs	 22-Jun-1993	Allow pmem boot.
 *
 *      cbf      27-Jan-1993    modify ev var password init routine
 *
 *	pel	 14-Jan-1993	Conditionalize for morgan
 *
 *	pel	 12-Oct-1992	ev_console; fix calc of CTB_OFFSET
 *
 *      pel      22-Sep-1992    change boot_reset, enable_audit initial values
 *				from "0" to "OFF", "ON", respectively
 *
 *      pel      17-Sep-1992    change language env var to support SRM
 *
 *      pel      15-Sep-1992    write sys serial # to HWRPB at init & write time
 *				Revalidate then recompute HWRPB checksum.
 *
 *      pel      20-Jun-1992    Add tt_allow_login env variable
 *
 *	ajb	 14-Jul-1992	Conditionalize for jensen
 *
 *      pel      26-Jun-1992    Add screen_mode env variable
 *
 *      cjd      09-Jun-1992    Added variables for inet driver
 *
 *	hcb	 05-Jun-1992	Fix bug in validate interleave
 *
 *	hcb	 29-May-1992	add interleave_init
 *
 *	hcb	 28-May-1992	Change seek_cur to seek_set and "r" to "r+"
 *
 *	hcb	 27-May-1992	add interleave action routines
 *                              add validate_cmm_desc
 *                                       
 *	hcb	 04-May-1992	add validate interleave for Cobra
 *
 *	phk	 17-Dec-1991	add fbe specific env variables
 *
 *	dtm	 5-Dec-1991	Re-order environment variables to comply with
 *				SRM ECO #15.
 *
 *	dtm	 4-Dec-1991	Set tty_dev
 *
 *	cfc	11-Sep-1991	Add extern decl for intl2db.  Add conditional
 *				in ev_validate_interleave arount LSB constant.
 *
 *	dtm	12-Jun-1991	validate_interleave completed.  ev_interleave
 *				added.                        
 *
 *	kl	11-Jun-1991	Remove hwrpb environment variable.
 *
 *	jds	29-May-1991	Updated CTB field reference.
 *
 *	phk     09-May-1991	Add an ev action on read
 *
 *	kl	18-Mar-1991	Replace set cpu command with environment variables
 *				and validation/action routines.
 *
 *	phk	08-Mar-1991	Add the FBE specific ev
 *
 *	dtm	14-Feb-1991	Change HWRPB ev to a string.  Add other 
 *				SRM-defined environment variables.  Add
 *				SRM attributes to facilitate callbacks.
 *
 *	dtm	06-Nov-1990	Work on console terminal action routines
 *				and interleave validation.
 *
 *	dtm	12-Oct-1990	Initial. 
 */
#include	"cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:ev_def.h"
#include	"cp$src:pb_def.h"
#include	"cp$src:tt_def.h"
#include        "cp$src:combo_def.h"
#include        "cp$inc:platform_io.h"
#include        "cp$src:platform_cpu.h"
#include 	"cp$inc:kernel_entry.h"
#include	"cp$src:kbd.h"
#include	"cp$src:hwrpb_def.h"                          
#include	"cp$src:mem_def.h"
#include	"cp$src:wwid.h"

#if RAWHIDE
#include 	"cp$src:eerom_def.h"
#endif

extern struct LOCK spl_kernel;

#if MEDULLA
int ev_wr_pciarb( char *name, struct EVNODE *ev );
int ev_wr_pcipark( char *name, struct EVNODE *ev );
int ev_wr_pci_int_sel( char *name, struct EVNODE *ev );
int ev_wr_ttbaud( char *name, struct EVNODE *ev );
int ev_ttbaud_init ();
#endif

#include	"cp$src:pal_def.h"
#include	"cp$src:impure_def.h"

#if !MODULAR
int controlp_enable = 1;
#else
extern int controlp_enable;
#endif

extern int primary_cpu;
extern int in_console;
extern int cpu_mask;
extern int ev_initing;
extern int cbip;
extern int cpip;

extern struct TTPB *console_ttpb;

extern struct DDB *pmem_ddb_ptr;

extern int servers_enabled;

#if SECURE
extern int ev_wr_pwd();          /* for secure */
extern int ev_pwd_init();        /* for secure */
#endif

extern struct HWRPB *hwrpb;

#if SABLE && EXTRA
extern int ev_remote_boot_password (char *name);
extern int ev_remote_boot_mode (char *name);
#endif

#if PC264
extern int ev_wr_shutdown_temp( char *name, struct EVNODE *ev );
extern int ev_rd_shutdown_temp( char *name, struct EVNODE *ev, struct EVNODE *ev_copy );
extern int validate_temp_value( int *value );
#endif

#if BOOTBIOS
extern int bootbios_validate( char *name );
#endif

int ev_wr_dev ();
int ev_wr_dev_or_pmem ();
int ev_rd_dev ();

int ev_rd_ctrl ();

int ev_wr_fc ();
int ev_rd_fc ();
int ev_fc_init ();

int ev_ffnext_init();
int ev_wr_ffauto();
int ev_wr_ffnext();
int ev_rd_ffnext();

#if MODULAR || MONET
int ev_term_type_wr();
int ev_term_page_wr();
int ev_graphics_type_wr();
int ev_graphics_page_wr();
int ev_term_type_rd();
int ev_term_page_rd();
int ev_graphics_type_rd();
int ev_graphics_page_rd();
#endif

#if RAWHIDE || PC264
int ev_sys_serial_num_wr ();
int ev_sys_serial_init ();
int validate_model_num ();
int validate_variant ();
int validate_cpu_mask ();
int validate_com1_baud ();
int validate_com1_flow ();
int validate_req64_value ();
int validate_temp_value ();
#endif

#if RAWHIDE || REGATTA || PC264 || FALCON
int ev_memory_test ();
int ev_memory_test_init ();
#endif

#if MODULAR || MONET
struct COM_DEVTAB {
	char name [8];
	unsigned int	port;
	unsigned int	rx_vector;
	unsigned int	tx_vector;
	struct TTPB	*ttpb;
	struct pb	*pb;
	unsigned int	ier;
};
extern int com_devtab_ptr;
extern struct TTPB *graphics_ttpbs;
extern int graphics_term_type;
extern int graphics_page_size;
#endif

int ev_sev ();
int ev_sev_init ();
int ev_kbd_hardware_type_wr ();
int ev_language_wr ();
int ev_sys_serial_num_wr ();                                                 
                                                                             
extern struct QUEUE tt_pbs;
int ev_wr_tt_allow_login ();

extern int force_failover_auto;
extern int force_failover_next;

#if SABLE || RAWHIDE || MIKASA || REGATTA || WILDFIRE || MONET || PC264 || EIGER || MARVEL || FALCON
int ev_com1_baud ();
int ev_com2_baud ();
int ev_com1_baud_init ();
int ev_com1_modem ();
int ev_com2_modem ();
int com1_modem;
int com2_modem;
int ev_com1_flow ();
int ev_com2_flow ();
int com1_flow;
int com2_flow;
#if WEBBRICK
int ev_com1_mode ();
int ev_com1_mode_init ();
int ev_com1_mode_rd ();
int ev_com1_flow_init ();
#endif
#if RAWHIDE
extern int com1_baud;		/* Declared in kernel on Rawhide */
extern int com2_baud;
#else
int com1_baud;
int com2_baud;
#endif
#endif

#if !(MEDULLA || CORTEX || YUKONA)
int ev_os_type ();
int ev_os_type_init ();
#endif

#if MNSPLIT
int ev_xdelta ();
int ev_xdelta_init ();
#endif

#if SABLE
int ev_ocp_init ();
extern int tu_ev_write (char *name, struct EVNODE *ev);
#endif

#if WILDFIRE
int ev_com1_flow_init ();
int ev_dma_read_mode ();
extern int ev_screen_mode_init ();
extern int ev_wr_screen_mode (char *name, struct EVNODE *ev);
#endif

#if SABLE
extern int ev_screen_mode_init (struct env_table *et, char *value);
extern int ev_wr_screen_mode (char *name, struct EVNODE *ev);
#endif

int ev_console ();
int ev_console_init ();
#if AVMIN
int ev_quick_start();
int ev_quick_start_rd();
int ev_quick_start_init();
#endif

#if TGA
#define TgaToyOffset 0x16
int ev_tga_sync ( char *value, struct EVNODE *ev );
int ev_tga_sync_init ( struct env_table *et, char *value );
#endif

#if SABLE || RAWHIDE || REGATTA || MONET || PC264 || EIGER || FALCON
int ev_wr_pci_parity ();
#endif

#if MONET
int ev_wr_usb_enable ();
int ev_mop_password_wr();
#endif

#if RAWHIDE
int ev_wr_pci_arb ();
int ev_wr_pci_req64 ();
int ev_wr_pci_parity ();
int ev_wr_shutdown_temp ();
int ev_rd_shutdown_temp ();
#endif
#if SABLE
int ev_wr_pci_arb ();
#if !LYNX
int ev_wr_pci_read_mult ();
#endif
#if GAMMA
int ev_wr_external_mb ();
#endif
#endif

#if SABLE || MIKASA || REGATTA || WILDFIRE || K2 || TAKARA || MONET || PC264 || EIGER || FALCON
int ev_wr_controlp ();
#endif

#if TAKARA || K2 || EIGER
extern int dmcc_psu_sense;
int ev_dmcc_psu_sense ();
#endif

#if SABLE
int ev_scsnode ();
int ev_wr_scsnode ();
int ev_scssystemid ();
int ev_wr_scssystemid ();
#if MSCP_SERVER
int ev_wr_enable_servers ();
#endif
#endif

#if !RAWHIDE
int ev_wr_scsi_poll ();
extern int scsi_poll;
#endif
int ev_wr_scsi_reset ();
extern int scsi_reset;

#if SABLE || MIKASA || K2 || TAKARA || EB164
int ev_bus_probe_algorithm ();
int ev_bus_probe_algorithm_init ();
extern int bus_probe_algorithm;
#endif
#if MIKASA || SABLE || RAWHIDE
#if !CORELLE
int ev_wr_rcm_dialout ();
int ev_rcm_dialout_init ();
int ev_wr_rcm_init ();
int ev_rcm_init_init ();
int ev_wr_rcm_answer ();
int ev_rcm_answer_init ();
int ev_wr_rcm ();
int ev_rcm_init ();
#endif
#endif

#if SABLE || WILDFIRE || REGATTA || FALCON || PC264
int ev_heap_expand ();
int ev_heap_expand_init ();
#endif

#if REGATTA || WILDFIRE
int ev_com1_mode ();
int ev_com1_mode_init ();
int ev_com1_mode_rd ();
#endif

#if REGATTA
int ev_wr_ocp_text( );
int ev_com1_flow_init ();
int ev_com1_modem_init ();
int ev_sys_com1_rmc ();
#if CLIPPER
int ev_prefetch_mode ();
extern int sbe_prefetch_bits;
#endif
#endif

#if CLIPPER || WEBBRICK || PC264 || SX164 || LX164
int ev_console_memory_allocation( );
int ev_console_memory_allocation_init( );
#endif

#if NAUTILUS || TINOSA || GLXYTRAIN
int temp_value_wr ();
#endif

extern int graphics_console;
extern unsigned int kbd_lang_array;
extern unsigned int ctb_kbd_lang;
extern unsigned int kbd_hardware_type;


#if EV6
int ev_wr_page_table_levels ();
int page_table_levels;

struct set_param_table page_table_levels_table[] = {
	{"3",	0},
	{"4",	0},
	{0}
    };
#endif

struct LANG_TABLE {
	int	code;		/* SRM defined language code */
	char	*name;
    } lang_table[] = {
	{LANG_K_NONE,			"none"},
	{LANG_K_DANSK,			"Dansk"},
	{LANG_K_DEUTSCH,		"Deutsch (Deutschland/Osterreich)"},
	{LANG_K_DEUTSCH_SCHWEIZ,	"Deutsch (Schweiz)"},
	{LANG_K_ENGLISH_AMERICAN,	"English (American)"},      
	{LANG_K_ENGLISH_BRITISH,	"English (British/Irish)"},      
	{LANG_K_ESPANOL,		"Espanol"},
	{LANG_K_FRANCAIS,		"Francais"},
	{LANG_K_FRANCAIS_CANADA,	"Francais (Canadian)"},
	{LANG_K_FRANCAIS_SUISSE_ROMANDE,"Francais (Suisse Romande)"},
	{LANG_K_ITALIANO,		"Italiano"},
	{LANG_K_NEDERLANDS,		"Nederlands"},
	{LANG_K_NORSK,			"Norsk"},
	{LANG_K_PORTUGUES,		"Portugues"},
	{LANG_K_SUOMI,			"Suomi"},
	{LANG_K_SVENSKA,		"Svenska"},
	{LANG_K_VLAAMS,			"Belgisch-Nederlands"},
	{LANG_K_JAPANESE_JJ,		"Japanese (JIS)"},
	{LANG_K_JAPANESE_AJ,		"Japanese (ANSI)"},
    };    

struct set_param_table baud_table[] = {
#if SABLE || RAWHIDE || MIKASA || REGATTA || WILDFIRE || MONET || PC264 || EIGER || FALCON
	{"57600", 2},
	{"38400", 3},
	{"19200", 6},
	{"9600", 12},
	{"7200", 16},
	{"4800", 24},
	{"3600", 32},
	{"2400", 48},
#if !WILDFIRE
	{"2000", 58},
	{"1800", 64},
#if !REGATTA
	{"1200", 96},
	{"600", 192},
	{"300", 384},
	{"150", 768},
	{"134.5", 857},
	{"110", 1047},
	{"75", 1536},
	{"50", 2304},
#if API_PLATFORM
	{"115200", 1},
#endif
#endif
#endif
#else
	{"300",	0},
	{"600",	0},
	{"1200",	0},                    
	{"2400",	0},  
	{"4800",	0},
	{"9600",	0},
	{"19200",	0},
	{"38400",	0},
#endif
	{0}
    };    

struct set_param_table flow_table[] = {
	{"NONE",	0},
	{"SOFTWARE",	1},
	{"HARDWARE",	2},
	{"BOTH",	3},
	{0}
    };

#if REGATTA || WILDFIRE
struct set_param_table mode_table[] = {
	{"THROUGH",	0},
	{"SNOOP",	1},
	{"SOFT_BYPASS",	2},
	{"FIRM_BYPASS",	3},
	{"LOCAL",	4},
#if WILDFIRE
	{"HARD_BYPASS",	5},
#endif
	{0}
    };
#endif

#if WEBBRICK
/*
 * MODE Table for RMC COM1_MODE
 */
struct set_param_table mode_table[] = {
	{"SNOOP", 1},
	{"SOFT_BYPASS",	  2},
	{"FIRM_BYPASS",  3},
	{0}
};

/*
 *  Enable / Disable for RMC halt request
 */
struct set_param_table rmc_halt_request[] = {
	{ "DISABLED", 0 },
	{ "ENABLED", 1 },
	{0}
};

#endif


struct set_param_table kbd_hardware_type_table[] = {
        {"PCXAL",       PCXAL},
        {"LK411",       LK411},
        {0}
    };

#if MODULAR || MONET
struct set_param_table term_type_table[] = {
	{"VIDEO",	0}, 
	{"HARDCOPY",	0}, 
	{0}
    };
#endif

#if RAWHIDE
struct set_param_table sys_model_table[] = {
        {"1200",        0},
        {"4000",        0},
        {"4000LP",      0},
        {"4100",        0},
        {"4100LP",      0},
        {"4X00",        0},
        {"5300",        0},
        {"5305",        0},
        {"7300",        0},
        {"7305",        0},
        {"7310",        0},
        {0}
    };
struct set_param_table sys_type_table[] = {
	{"PEDESTAL",	0}, 
	{"RACKMOUNT",	0}, 
	{0}
    };
#endif

struct set_param_table aa_table[] = {
	{"BOOT",	0},
	{"HALT",	0},
	{"RESTART",	0},
	{0}
    };
                                         
struct set_param_table ot_table[] = {
#if !(REGATTA || WILDFIRE || FALCON || MNSPLIT)
	{"NT",		1},
#endif
	{"VMS",		2},
	{"OpenVMS",	2},
	{"OSF",		3},
	{"UNIX",	3},
#if REGATTA || WILDFIRE || APL_PLATFORM
	{"Linux",	4},
#endif
	{0}
    };

struct set_param_table onoff[] = {
	{"1",	0},
	{"0",	0},
	{0}
    };

struct set_param_table onoff_ascii[] = {
	{"ON",	1},
	{"OFF",	0},
	{0}
    };

struct set_param_table zero[] = {
	{"0",	0},
	{0}
    };

#if SABLE || MIKASA || K2 || TAKARA || APC_PLATFORM || REGATTA
struct set_param_table new_old[] = {
	{"new",	0},
	{"old",	0},
	{0}
    };
#endif

#if RAWHIDE || REGATTA || PC264 || FALCON
struct set_param_table memory_test_selection[] = {
	{"full",	0},
	{"partial",	0},
	{"none",	0},
	{0}
    };
#endif

struct set_param_table serial_graphics[] = {
	{"serial",	0},
	{"graphics",	0},
	{0}
    };

#if SABLE
struct set_param_table pci_arb_table[] = {
	{"Fixed",	0},
	{"Partial-Rotating",	0},
	{"Rotating",	0},
	{0}
    };
#endif

#if RAWHIDE
struct set_param_table pci_arb_table[] = {
	{"Bridge",	0},
	{"Round-Robin",	0},
	{"Modified",	0},
	{0}
    };
#endif


#if MIKASA || K2 || MTU || CORTEX || YUKONA || TAKARA || (APC_PLATFORM && !PC264)
/* This is for enable/disable of parity checking		*/
/* Clear State is 0xff 						*/
struct set_param_table pci_parity[] = {
	{"on",		0xff},
	{"off",		0x00},
#if !CORELLE
	{"sniff",	0x01},
#endif
	{0}
    };
#endif

#if MEDULLA 
struct set_param_table pci_mode_table[] = {
	{"0",	0},
	{"1",	0},
	{"2",	0},
	{"3",	0},
	{0}
};
struct set_param_table pci_int_sel_table[] = {
	{"0",	0},
	{"1",	0},
	{0}
};

struct set_param_table pci_pcmcia_size_table[] = {
	{"0",   0},        /* 0MB */
        {"800000",   0},   /* 8MB */
        {"1000000",   0},  /* 16MB */
        {"2000000",   0},  /* 32MB */
        {"4000000",   0},  /* 64MB */
        {0}
};

/*
 * Validation Table for tt_baud -- UART baud rate environment variable
 */
struct set_param_table tt_baud_table[] = {
	{"300",	  0},
	{"600",	  0},
	{"1200",  0},
	{"2400",  0},
	{"4800",  0},
	{"9600",  0},
	{"19200", 0},
	{0}
};
#endif

#if SABLE || WILDFIRE || REGATTA || MARVEL || FALCON || PC264
struct set_param_table ev_heap_table[] = {
	{"NONE",	   0},
	{"64KB",	  64},
	{"128KB",	 128},
	{"256KB",	 256},
	{"512KB",	 512},
	{"1MB",		1024},
	{"2MB",		2048},
	{"3MB",		3072},
	{"4MB",		4096},
	{0}
    };
#endif

#if TAKARA
struct set_param_table takara_prefetch[] = {
        {"LMS",  0x21},
        {"MMS",  0x11},
        {"SMS",  0x01},
        {"LSS",  0x20},
        {"MSS",  0x10},
        {"SSS",  0x00},
        {0}    };
#endif

#if NAUTILUS || GLXYTRAIN
int ev_gart_size();
int ev_gart_size_init ();

struct set_param_table ev_gart_table[] = {
	{"DISABLED",	0},
	{"32MB",		1},
	{"64MB",		2},
	{"128MB",		3},
	{"256MB",		4},
	{"512MB",		5},
	{0}
    };
#endif
#if SWORDFISH
#include "cp$src:api_debug.h"

int ev_adaptec();
int ev_adaptec_init ();

struct set_param_table ev_adaptec_table[] = {
	{"ENABLED",		0},
	{"DISABLED",	APIDB_ADPDIS},
	{0}
    };
#endif
#if API_PLATFORM
struct set_param_table ac_action_table[] = {
	{"BOOT",	0},
	{"STANDBY",	1},
	{0}
    };

int ev_api_debug();
int ev_api_debug_init();
#endif
#if SHARK
int ev_ac_action();
int ev_wol_enable();
#endif

/* Make sure the order of the environment variables in this table are listed in the same order
   that they appear in module callbacks_alpha.mar in the ENV_VAR_TABLE.  Otherwise
   callbacks will not work correctly */ 
                                  
struct env_table env_table[] = {
{"auto_action","HALT",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	aa_table,ev_sev,0,ev_sev_init},
{"boot_dev","",
	EV$K_NODELETE | EV$K_NOWRITE | EV$K_STRING | EV$K_SRM_DEF,
       	0,ev_wr_dev_or_pmem,ev_rd_dev,0},
{"bootdef_dev","",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	0,ev_wr_dev,ev_rd_dev,ev_sev_init},
{"booted_dev","",
	EV$K_NODELETE | EV$K_NOWRITE | EV$K_STRING | EV$K_SRM_DEF,
       	0,0,ev_rd_dev,0},
{"boot_file","",
	EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"booted_file","",
	EV$K_NODELETE | EV$K_NOWRITE | EV$K_STRING | EV$K_SRM_DEF,
       	0,0,0,0},
{"boot_osflags","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"booted_osflags","",
	EV$K_NODELETE | EV$K_NOWRITE | EV$K_STRING | EV$K_SRM_DEF,
	0,0,0,0},
{"boot_reset","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	onoff_ascii,ev_sev,0,ev_sev_init},
{"dump_dev","",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	0,ev_wr_dev,ev_rd_dev,ev_sev_init},
{"enable_audit","ON",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	onoff_ascii,ev_sev,0,ev_sev_init},
{"license","MU",
	EV$K_NODELETE | EV$K_NOWRITE | EV$K_STRING | EV$K_SRM_DEF,
	0,0,0,0},
{"char_set",0,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_SRM_DEF,
	0,0,0,0},
{"language",LANG_K_ENGLISH_AMERICAN,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	0,ev_language_wr,0,ev_sev_init},
{"tty_dev","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF,
	0,0,0,0},
{"srm2dev","",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF,
	0,ev_wr_dev,ev_rd_dev,ev_sev_init},

#if SABLE || RAWHIDE || MIKASA || WILDFIRE || MONET || (PC264 && !WEBBRICK) || EIGER
#if RAWHIDE || PC264
{"com1_baud","9600",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE | EV$K_ROUTINE,
	validate_com1_baud,ev_com1_baud,0,ev_com1_baud_init},
#else
{"com1_baud","9600",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	baud_table,ev_com1_baud,0,ev_com1_baud_init},
#endif
{"com1_modem","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	onoff_ascii,ev_com1_modem,0,ev_sev_init},
#if RAWHIDE
{"com1_flow","SOFTWARE",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE | EV$K_ROUTINE,
	validate_com1_flow,ev_com1_flow,0,ev_sev_init},
#else
#if WILDFIRE
{"com1_flow","SOFTWARE",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	flow_table,ev_com1_flow,0,ev_com1_flow_init},
#else
{"com1_flow","SOFTWARE",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	flow_table,ev_com1_flow,0,ev_sev_init},
#endif
#endif
{"com2_baud","9600",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	baud_table,ev_com2_baud,0,ev_sev_init},
{"com2_modem","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	onoff_ascii,ev_com2_modem,0,ev_sev_init},
{"com2_flow","SOFTWARE",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	flow_table,ev_com2_flow,0,ev_sev_init},
#endif
#if REGATTA || WEBBRICK
{"com1_baud","9600",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	baud_table,ev_com1_baud,0,ev_com1_baud_init},
#if !WEBBRICK
{"com1_modem","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	onoff_ascii,ev_com1_modem,0,ev_com1_modem_init},
#else
{"com1_modem","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	onoff_ascii,ev_com1_modem,0,ev_sev_init},
#endif
{"com1_flow","SOFTWARE",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	flow_table,ev_com1_flow,0,ev_com1_flow_init},
{"com2_baud","9600",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	baud_table,ev_com2_baud,0,ev_sev_init},
{"com2_modem","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	onoff_ascii,ev_com2_modem,0,ev_sev_init},
{"com2_flow","SOFTWARE",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	flow_table,ev_com2_flow,0,ev_sev_init},
#endif

#if MIKASA || RAWHIDE
{"fru_table","ON",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_sev,0,ev_sev_init},
#endif
#if EV6
{"page_table_levels","3",
	EV$K_NODELETE | EV$K_NONVOLATILE,
	page_table_levels_table,ev_wr_page_table_levels,0,ev_sev_init},
#endif
#if SECURE
{"password","",
	EV$K_INTEGER | EV$K_NOREAD | EV$K_NODELETE | EV$K_ROUTINE | EV$K_NONVOLATILE, 
       	0,ev_wr_pwd,0,ev_pwd_init},
#endif
/*
 * Support for the ARC front requires their own set of variables.
 * These are (for now) machine independent.
 */

#if MODULAR
#if ARC_SUPPORT
{"arc_enable","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_sev,0,ev_sev_init},
#endif
#endif
#if !MODULAR
#if ARC_SUPPORT
{"OSLOADER",			"",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"SYSTEMPARTITION",		"",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"OSLOADFILENAME",		"",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"OSLOADPARTITION",		"",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"OSLOADOPTIONS",		"nodebug",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"CONSOLEIN",			"multi(0)key(0)keyboard(0)console(0)",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"CONSOLEOUT",			"eisa(0)video(0)monitor(0)console(0)",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"LOADIDENTIFIER",		"Windows NT",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"AUTOLOAD",			"NO",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"TIMEZONE",			"PST8PDT",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"FWSEARCHPATH",		"",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"PHYSICALADDRESSBITS",		"40",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"MAXIMUMADDRESSSPACENUMBER",	"127",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"SYSTEMSERIALNUMBER",		"0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"CYCLECOUNTERPERIOD",		"2670",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"A:",				"eisa(0)disk(0)fdisk(0)",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"FLOPPY",			"2",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"FLOPPY2",			"N",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"KEYBOARDTYPE",		"0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"VERSION",			"4.51",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"ENABLEPCIPARITYCHECKING",	"YES",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"LASTKNOWNGOOD",		"False",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"COUNTDOWN",			"10",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},

{"PROCESSORPAGESIZE",		"8192",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ARC,
	0, ev_sev, 0, ev_sev_init},
#endif
#endif

#if WILDFIRE
{"lp_count","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_cpu_mask0","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_io_mask0","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_mem_size0","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_cpu_mask1","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_io_mask1","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_mem_size1","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_cpu_mask2","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_io_mask2","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_mem_size2","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_cpu_mask3","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_io_mask3","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_mem_size3","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_cpu_mask4","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_io_mask4","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_mem_size4","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_cpu_mask5","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_io_mask5","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_mem_size5","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_cpu_mask6","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_io_mask6","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_mem_size6","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_cpu_mask7","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_io_mask7","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_mem_size7","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_shared_mem_size","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"lp_error_target","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},

{"dma_read_mode","",
	EV$K_NOREAD | EV$K_STRING,
	0,ev_dma_read_mode,0,ev_sev_init},
#endif

#if SABLE || REGATTA || WILDFIRE || (PC264 && !WEBBRICK) || FALCON
{"cpu_enabled",0xffffffff,
	EV$K_NODELETE | EV$K_NONVOLATILE | EV$K_INTEGER,
	0,ev_sev,0,ev_sev_init},
#endif

#if MODULAR || MONET
{"tta0_type","VIDEO",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_TABLE,
  	term_type_table,ev_term_type_wr,ev_term_type_rd,ev_sev_init},
{"tta0_page","0",
	EV$K_NODELETE | EV$K_NONVOLATILE | EV$K_STRING,
	0,ev_term_page_wr,ev_term_page_rd,ev_sev_init},
{"graphics_type","VIDEO",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_TABLE,
  	term_type_table,ev_graphics_type_wr,ev_graphics_type_rd,ev_sev_init},
{"graphics_page","0",
	EV$K_NODELETE | EV$K_NONVOLATILE | EV$K_STRING,
	0,ev_graphics_page_wr,ev_graphics_page_rd,ev_sev_init},
#endif

#if RAWHIDE
{"cpu_enabled",0xff,
	EV$K_NODELETE | EV$K_NONVOLATILE | EV$K_INTEGER | EV$K_ROUTINE,
	validate_cpu_mask,ev_sev,0,ev_sev_init},
{"sys_model_num","4X00",
	EV$K_NODELETE | EV$K_NONVOLATILE | EV$K_STRING | EV$K_ROUTINE,
	validate_model_num, ev_sev, 0, ev_sev_init},
{"sys_type","PEDESTAL",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_TABLE,
  	sys_type_table, ev_sev, 0, ev_sev_init},
{"sys_variant","0",
        EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ROUTINE,
        validate_variant, ev_sev, 0, ev_sev_init},
#endif

#if SABLE
#if LYNX
{"pci_arb","Partial-Rotating",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	pci_arb_table,ev_wr_pci_arb,0,ev_sev_init},
#else
{"pci_arb","Fixed",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	pci_arb_table,ev_wr_pci_arb,0,ev_sev_init},
#endif
#if !LYNX
{"pci_read_mult","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_wr_pci_read_mult,0,ev_sev_init},
#endif
#if GAMMA
{"external_mb","ON",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_wr_external_mb,0,ev_sev_init},
#endif
#endif

#if SABLE || WILDFIRE
{"screen_mode","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
  	onoff_ascii,ev_wr_screen_mode,0,ev_screen_mode_init},
{"full_powerup_diags","ON",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_sev,0,ev_sev_init},
#endif

#if SABLE || RAWHIDE || REGATTA || MONET || PC264 || EIGER || FALCON
{"pci_parity","ON",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_wr_pci_parity,0,ev_sev_init},
#endif

#if MONET
{"usb_enable","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_wr_usb_enable,0,ev_sev_init},
{"mopv3_boot",
	"OFF",
        EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
        onoff_ascii, ev_sev, 0, ev_sev_init},
{"mop_password",
	"00-00000000-00000000",
	EV$K_NODELETE | EV$K_NONVOLATILE | EV$K_STRING,
	0, ev_mop_password_wr, 0, ev_sev_init},
{"mop_version",
	(void *)1,
	EV$K_NODELETE | EV$K_NONVOLATILE | EV$K_INTEGER,
	0, ev_sev, 0, ev_sev_init},
{"mop_debug",
	"OFF",
	EV$K_NODELETE | EV$K_NONVOLATILE | EV$K_STRING,
	onoff_ascii, ev_sev, 0, ev_sev_init},
#endif

#if RAWHIDE
{"pci_arbmode","Round-Robin",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	pci_arb_table,ev_wr_pci_arb,0,ev_sev_init},
{"pci_req64","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ROUTINE,
	validate_req64_value,ev_wr_pci_req64,0,ev_sev_init},
{"shutdown_temp","55",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ROUTINE,
	validate_temp_value,ev_wr_shutdown_temp,ev_rd_shutdown_temp,ev_sev_init},
{"prompt",">>>",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"sys_serial_num","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE, 
	0,ev_sys_serial_num_wr,0,ev_sys_serial_init},
#if !RHMIN
{"reset_boot_arg0","",
	EV$K_NODELETE | EV$K_NOWRITE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"reset_boot_arg1","",
	EV$K_NODELETE | EV$K_NOWRITE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"reset_boot_arg2","",
	EV$K_NODELETE | EV$K_NOWRITE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
#endif
#endif
#if MIKASA || BOOT_RESET
{"reset_boot_arg0","",
	EV$K_NODELETE | EV$K_NOWRITE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"reset_boot_arg1","",
	EV$K_NODELETE | EV$K_NOWRITE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"reset_boot_arg2","",
	EV$K_NODELETE | EV$K_NOWRITE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
#endif
#if MIKASA
#if !CORELLE
{"ocp_text","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
#endif
#endif

#if !RAWHIDE
{"scsi_poll","ON",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_wr_scsi_poll,0,ev_sev_init},
#endif
{"scsi_reset","ON",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_wr_scsi_reset,0,ev_sev_init},

#if SABLE || MIKASA || K2 || TAKARA || EB164
{"bus_probe_algorithm","new",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	new_old,ev_bus_probe_algorithm,0,ev_bus_probe_algorithm_init},
#endif
{"console","graphics",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	serial_graphics,ev_console,0,ev_console_init},

#if RAWHIDE
{"cda0","0",
	EV$K_NODELETE | EV$K_NONVOLATILE | EV$K_STRING,
	0,ev_sev,0,ev_sev_init},
{"ocp_text","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
#endif

#if RAWHIDE || REGATTA || PC264 || FALCON
{"full_powerup_diags","ON",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_sev,0,ev_sev_init},
{"memory_test","full",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	memory_test_selection,ev_memory_test,0,ev_memory_test_init},
#endif

#if TGA
{"tga_sync_green",0xff,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0,ev_tga_sync,0,ev_tga_sync_init},
#endif

#if MIKASA || K2 || MTU || CORTEX || YUKONA|| TAKARA || (APC_PLATFORM && !PC264)
#if CORELLE
 /* CIA Parity 		*/
{"pci_parity","on",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	pci_parity,ev_sev,0,ev_sev_init},
#else
 /* Epic Parity 		*/
{"pci_parity","off",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	pci_parity,ev_sev,0,ev_sev_init},
#endif
#endif
#if SABLE || MIKASA || REGATTA || WILDFIRE || K2 || TAKARA || MONET || PC264 || EIGER || FALCON
{"controlp","ON",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_wr_controlp,0,ev_sev_init},
#endif
#if SABLE || RAWHIDE
{"tt_allow_login",1,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
  	0, ev_wr_tt_allow_login, 0, ev_sev_init},
#else
{"tt_allow_login",1,
	EV$K_NODELETE | EV$K_INTEGER,
  	0, ev_wr_tt_allow_login, 0, 0},
#endif
#if SABLE
{"scsnode","",
	EV$K_ROUTINE | EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE, 
	ev_scsnode,ev_wr_scsnode,0,ev_sev_init},
{"scssystemid","65534",
	EV$K_ROUTINE | EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE, 
	ev_scssystemid,ev_wr_scssystemid,0,ev_sev_init},
{"scssystemidh","0",
	EV$K_ROUTINE | EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE, 
	ev_scssystemid,ev_wr_scssystemid,0,ev_sev_init},
#if MSCP_SERVER
{"enable_servers","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE, 
	onoff_ascii,ev_wr_enable_servers,0,ev_sev_init},
#endif
#endif

#if FALCON
{"thermal_override","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_sev,0,ev_sev_init},
#endif

#if MEDULLA || CORTEX || K2 || TAKARA || YUKONA || EIGER
{"vx_bootline","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
#endif


#if YUKONA
                       
{"univ_vsi0","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},  
{"univ_vsi1","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"univ_vsi2","", 
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"univ_vsi3","", 
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"univ_vsi4","", 
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"univ_vsi5","", 
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"univ_vsi6","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},  
{"univ_vsi7","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"univ_vrai","", 
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"univ_lsi0","", 
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"univ_lsi1","", 
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"univ_lsi2","", 
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init}, 
{"univ_lsi3","", 
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init}, 
{"univ_lsi4","", 
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init}, 
{"univ_lsi5","", 
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init}, 
{"univ_lsi6","", 
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init}, 
{"univ_lsi7","", 
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init}, 
  
#endif



#if MEDULLA || CORTEX
{"vme_config","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"vme_a32_base", 0,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"vme_a24_base", 0,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"vme_a16_base", 0,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"vme_a32_size", 0,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},
{"vme_a24_size", 0,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0,ev_sev,0,ev_sev_init},


#if MEDULLA
{"pcmcia_smem_size", "0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_TABLE,
	pci_pcmcia_size_table,ev_sev,0,ev_sev_init},
{"pcmcia_dmem_size", "0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_TABLE,
	pci_pcmcia_size_table,ev_sev,0,ev_sev_init},
{"pci_arb_mode", "0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_TABLE,
	pci_mode_table,ev_wr_pciarb,0,ev_sev_init},
{"pci_park_dev", "0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_TABLE,
	pci_mode_table,ev_wr_pcipark,0,ev_sev_init},
{"pci_bcd_int_select", "0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_TABLE,
	pci_int_sel_table,ev_wr_pci_int_sel,0,ev_sev_init},
{"tt_baud", "9600",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_TABLE,
	tt_baud_table, ev_wr_ttbaud,0,ev_ttbaud_init},
#endif

{"ncr2_setup","AUTO 7",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE, 
	0,ev_sev,0,ev_sev_init},
#endif

#if !RAWHIDE
{"sys_serial_num","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE, 
	0,ev_sys_serial_num_wr,0,ev_sev_init},
#endif

#if !(MEDULLA || CORTEX || YUKONA)
#if APC_PLATFORM || TAKARA || FALCON
#if API_PLATFORM
{"os_type","LINUX",
#else
{"os_type","UNIX",
#endif
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	ot_table,ev_os_type,0,ev_os_type_init},
#else
{"os_type","OpenVMS",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	ot_table,ev_os_type,0,ev_os_type_init},
#endif
#endif

#if SABLE
{"ocp_text_default","Model 4/200",
	EV$K_NODELETE | EV$K_STRING | EV$K_NOWRITE
	| EV$K_NONVOLATILE, 0, 0, 0, 0},
{"ocp_text","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
#endif

#if REGATTA || WILDFIRE
{"com1_mode","THROUGH",
	EV$K_NODELETE | EV$K_STRING,
	mode_table,ev_com1_mode,ev_com1_mode_rd,ev_com1_mode_init},
#endif

#if REGATTA
{"sys_com1_rmc","ON",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	onoff_ascii,ev_sys_com1_rmc,0,ev_sev_init},
{"ocp_text","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_wr_ocp_text, 0, ev_sev_init},
#endif

#if CLIPPER
{"prefetch_mode","ON",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_prefetch_mode,0,ev_sev_init},
{"dimm_zmdi","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
#endif

#if WEBBRICK
{"com1_mode","SNOOP",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE,
	mode_table,ev_com1_mode,0,ev_sev_init},
#endif

#if CLIPPER || WEBBRICK || PC264 || SX164 || LX164
{"console_memory_allocation", "old",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	new_old, ev_console_memory_allocation, 0, ev_console_memory_allocation_init},
#endif


#if APC_PLATFORM
/*
** Alpha Microprocessor Evaluation Board specific Environment Variables
*/
{"oem_string","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE, 
	0, ev_sev, 0, ev_sev_init},
#if ( PC264 && !WEBBRICK && !NAUTILUS && !TINOSA && !GLXYTRAIN)
{"shutdown_temp","55",
        EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ROUTINE,
        validate_temp_value,ev_wr_shutdown_temp,ev_rd_shutdown_temp,ev_sev_init},
{"ocp_text","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
#endif
#endif

#if WEBBRICK
{"shutdown_temp","60",
        EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ROUTINE,
        validate_temp_value,ev_wr_shutdown_temp,ev_rd_shutdown_temp,ev_sev_init},
{"mfg_status","",
        EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE ,
        0, ev_sev, 0, ev_sev_init},
{"rmc_halt", "DISABLED",
        EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE ,
        rmc_halt_request, ev_sev, 0, ev_sev_init},
	
#endif

#if AVMIN
{"quick_start","off",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii, ev_quick_start, ev_quick_start_rd, ev_quick_start_init},
#endif

{"kbd_hardware_type","PCXAL",
        EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
        kbd_hardware_type_table,ev_kbd_hardware_type_wr,0,ev_sev_init},

#if MTU
{"UIP","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_sev,0,ev_sev_init},

{"pwrup_show","ON",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_sev,0,ev_sev_init},

{"pwrup_update","ON",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_sev,0,ev_sev_init},

#endif
#if PINNACLE
{"full_powerup_diags","ON",
        EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
        onoff_ascii, ev_sev,0,ev_sev_init},
#endif
#if MIKASA || SABLE || RAWHIDE
#if !CORELLE
{"rcm_dialout","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_wr_rcm_dialout,0,ev_rcm_dialout_init},
{"rcm_init","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_wr_rcm_init,0,ev_rcm_init_init},
{"rcm_answer","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0,ev_wr_rcm_answer,0,ev_rcm_answer_init},
#endif
#endif

#if SABLE || WILDFIRE || REGATTA || FALCON || PC264
{"heap_expand","",
        EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	ev_heap_table,ev_heap_expand,0,ev_heap_expand_init},
#endif

#if TAKARA || K2 || EIGER
{"ocp_text","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"psu_sense","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii, ev_dmcc_psu_sense, 0, ev_sev_init},
#endif

#if TAKARA
{"pci_prefetch", "LMS",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	takara_prefetch, ev_sev, 0, ev_sev_init},
#endif

{"srm2ctrl","",
	EV$K_NODELETE | EV$K_STRING,
	0,0,ev_rd_ctrl,ev_sev_init},

{"ffauto","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii, ev_wr_ffauto, 0, ev_sev_init},

{"ffnext","OFF",
	EV$K_NODELETE | EV$K_STRING,
	onoff_ascii, ev_wr_ffnext, ev_rd_ffnext, ev_sev_init },

#if GALAXY && REGATTA
{"lp_count","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},

{"lp_cpu_mask0","f",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},

{"lp_io_mask0","ff",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},

{"lp_mem_size0","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},

{"lp_cpu_mask1","f",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},

{"lp_io_mask1","ff",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},

{"lp_mem_size1","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},

{"lp_shared_mem_size","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
#endif

#if PRIVATEER
{"hose_1_default_speed","33",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"hose_3_default_speed","33",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
#endif

#if BOOTBIOS
{"bootbios","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ROUTINE,
	bootbios_validate, ev_sev, 0 ,ev_sev_init},
#endif

#if MNSPLIT
{"xdelta","OFF",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	onoff_ascii,ev_xdelta,0,ev_xdelta_init},
#endif

#if NAUTILUS || TINOSA || GLXYTRAIN
{"shutdown_temp","60",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ROUTINE,
	validate_temp_value,temp_value_wr,0,ev_sev_init},
#endif
#if SWORDFISH
{"onboard_adaptec","ENABLED",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	ev_adaptec_table,ev_adaptec,0,ev_adaptec_init},
#endif
#if NAUTILUS || GLXYTRAIN
{"gartsize","32MB",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	ev_gart_table,ev_gart_size,0,ev_gart_size_init},
#endif
#if API_PLATFORM
{"diags_action","",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init},
{"api_debug","0",
	EV$K_NODELETE | EV$K_STRING | EV$K_SRM_DEF | EV$K_NONVOLATILE | EV$K_ROUTINE,
	0,ev_api_debug,0,ev_api_debug_init},

#endif
#if SHARK
{"ac_action","BOOT",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	ac_action_table,ev_ac_action,0,ev_sev_init},
{"wol_enable","OFF",
	EV$K_NODELETE | EV$K_STRING,
	onoff_ascii,ev_wol_enable,0,ev_sev_init},
#endif

	{0}
    };



/*+
 * ============================================================================
 * = ev_language_wr - action routine for 'language' environment variable      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Validate the value specified as the new language.  If valid,
 *	decode and modify the 'language_name' environment variable.
 *
 * FORM OF CALL:
 *
 *	ev_language_wr ()
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *	msg_ev_badvalue - illegal value specified 
 *
 * ARGUMENTS:
 *
 *      char *name	  - string containing name of the environment variable
 *   	struct EVNODE *ev - address of a pointer to an evnode.
 *
 * SIDE EFFECTS:
 *
 *      Environment variable lists referenced.
 *	The EVLOCK semaphore has been taken out.
-*/
int ev_language_wr (char *name, struct EVNODE *ev) {
    struct LANG_TABLE *lt;
    int i;

    lt = lang_table;
    for (i=0; i < sizeof(lang_table) / sizeof(lang_table[0]) ;i++) {
	if (lt[i].code == ev->value.integer) {
	    ev_write( "language_name", lt[i].name,
		    	EV$K_STRING | EV$K_NODELETE | EV$K_NOWRITE);
	    /* Save this globally for hwrpb and for kbd_driver.
	     * Note: if the user defined language is NONE tell the
	     * it to default to the english/american keyboard. */
	    ctb_kbd_lang = ev->value.integer;
	    if (ev->value.integer == LANG_K_NONE) 
		kbd_lang_array = (LANG_K_ENGLISH_AMERICAN-LANG_K_DANSK) >> 1;
	    else 
		kbd_lang_array = (ev->value.integer-LANG_K_DANSK) >> 1;
	    return( ev_sev( name, ev ) );
	}
    }


/*
 * Report that didn't match and give the user the allowed values.
 */
#if 0
    err_printf("set language 0 and reset system for language menu selection\n");
#else
    err_printf("bad value - valid selections:\n");
    for (i=0; i < sizeof(lang_table) / sizeof(lang_table[0]) ;i++) {
	err_printf(" %15x          -   %s\n",lt[i].code, lt[i].name);
    }
#endif


    /* Restore the last valid write of this integer. */
    ev->value.integer = ctb_kbd_lang;

    return msg_ev_badvalue;
}

/*+
 * ============================================================================
 * = ev_kbd_hardware_type_wr - write routine for 'kbd_hardware_type' EV       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Used to write the 'kbd_hardware_type' environment variable.
 *
 * FORM OF CALL:
 *
 *	ev_kbd_hardware_type_wr ()
 *
 * RETURN CODES:
 *
 *	return code from routine ev_sev
 *
 * ARGUMENTS:
 *
 *      char *name	  - string containing name of the environment variable
 *   	struct EVNODE *ev - address of a pointer to an evnode.
 *
 * SIDE EFFECTS:
 *
 *      kbd_hardware_type global has been filled in.
-*/
int ev_kbd_hardware_type_wr (char *name, struct EVNODE *ev)
{
    struct set_param_table *p;

    for( p = kbd_hardware_type_table; p->param; p++ )
	if( strcmp( p->param, ev->value.string ) == 0 )
	    kbd_hardware_type = p->code;

    return( ev_sev( name, ev ) );
}

int ev_wr_dev( char *name, struct EVNODE *ev )
    {
    char *c;
    char *device;
    char *result;
    char *string;
    int i;
    int j;
    int t;
    int trim;
    int fields;
    struct INODE *ip;
    struct EVNODE *ev_copy;

    if( ev_initing )
	return( msg_success );
    trim = strcmp( name, "booted_dev" ) && strcmp( name, "srm2dev" );
    device = malloc( EV_VALUE_LEN );
    result = malloc( EV_VALUE_LEN );
    string = malloc( EV_VALUE_LEN );
    *string = 0;
    for( i = 0; strelement( ev->value.string, i, " ,", device ); i++ )
	{
	t = allocinode( device, 0, &ip );
	if( t == 3 )
	    {
	    err_printf( "device %s is ambiguous\n", device );
	    continue;
	    }
	if( t == 1 )
	    {
	    strcpy( device, ip->name );
	    INODE_UNLOCK (ip);
	    }
	file2dev( device, result, 0 );
	if( strlen( result ) )
	    {
            if( trim )
		{
		fields = 8;
		if( !strncmp( result, "SCSI3", 5 ) )
		    fields = 9;
		c = result;
		for( j = 0; j < fields; j++ )
		    if( !( c = strchr( c + 1, ' ' ) ) )
			break;
		if( c )
		    *c = 0;
		}
	    if( strlen( string ) )
		{
		if( strlen( result ) + strlen( string ) + 1 >= EV_VALUE_LEN )
		    {
		    err_printf( "string is too long to add %s\n", device );
		    continue;
		    }
		strcat( string, "," );
		}
	    strcat( string, result );
	    }
	else
	    {
	    err_printf( "device %s is invalid\n", device );
	    continue;
	    }
	}
    strcpy( ev->value.string, string );
    ev->size = strlen( string );
    free( device );
    free( result );
    free( string );
    if( ( ev->attributes & EV$M_VOLATILE ) == EV$K_NONVOLATILE )
	return( ev_sev( name, ev ) );
    return( msg_success );
    }

int ev_wr_dev_or_pmem( char *name, struct EVNODE *ev )
    {
    if( ev_initing )
	return( msg_success );
    /*
     * If exactly one device name was provided, and that device is pmem, then
     * write the name exactly as given, with any flags specified (should supply
     * the pmem address).  If these conditions are not satisfied, fall through
     * to ev_wr_pmem for normal validation.
     */
    if( !strpbrk( ev->value.string, " ," ) )
	{
	if( strncmp( ev->value.string, "pmem", 4 ) == 0 )
	    {
	    if( ( ev->attributes & EV$M_VOLATILE ) == EV$K_NONVOLATILE )
		return( ev_sev( name, ev ) );
	    return( msg_success );
	    }
#if MEDULLA || CORTEX || YUKONA
	if( strncmp( ev->value.string, "userflash", 9 ) == 0 )
	    {
	    if( ( ev->attributes & EV$M_VOLATILE ) == EV$K_NONVOLATILE )
		return( ev_sev( name, ev ) );
	    return( msg_success );
	    }
#endif
	}
    return( ev_wr_dev( name, ev ) );
    }

int ev_rd_dev( char *name, struct EVNODE *ev, struct EVNODE *ev_copy )
    {
    char *device;
    char *result;
    char *string;
    int i;

    if( ev_initing )
	return( msg_success );
    /*
     *  Avoid the dev2file translation for pmem.  This will allow
     *  the "b pmem:24000" type of command to work.
     */
    if( strncmp( ev->value.string, "pmem", 4 ) == 0 )
	{
	strcpy( ev_copy->value.string, ev->value.string );
	return( msg_success );
	}
#if MEDULLA || CORTEX || YUKONA
    if( strncmp( ev->value.string, "userflash", 9 ) == 0 )
	{
	strcpy( ev_copy->value.string, ev->value.string );
	return( msg_success );
	}
#endif
    device = malloc( EV_VALUE_LEN );
    result = malloc( EV_VALUE_LEN );
    string = malloc( EV_VALUE_LEN );
    *string = 0;
    for( i = 0; strelement( ev->value.string, i, ",", device ); i++ )
	{
	while( *device == ' ' )
	    memcpy( device, device+1, EV_VALUE_LEN );
	dev2file( device, result );
	if( strlen( string ) )
	    {
	    if( strlen( result ) + strlen( string ) + 1 >= EV_VALUE_LEN )
		{
		err_printf( "string is too long\n" );
	    	break;
		}
	    strcat( string, " " );
	    }
	strcat( string, result );
	}
    strcpy( ev_copy->value.string, string );
    ev_copy->size = strlen( string );
    free( device );
    free( result );
    free( string );
    return( msg_success );
    }             


#if ( STARTSHUT || DRIVERSHUT )

extern struct window_head config;

int ev_rd_ctrl( char *name, struct EVNODE *ev, struct EVNODE *ev_copy ) {

    struct pb *pb;
    struct device *dev;
    char *string;
    char buf[EV_VALUE_LEN];
    char protocol[8];
    char controller[8];
    char result[32];
    int unit, node, channel, slot, hose, idx;

    strcpy( ev_copy->value.string, "" );
    ev_copy->size = 0;

    if( ev->value.string[0] == '\0' )
      return msg_failure;

    dev2file( ev->value.string, buf );
    if(!buf[0]) {
      return msg_failure;
    }

    explode_devname(buf, protocol, controller, &unit, &node, &channel,
        &slot, &hose);
    dev = find_dev_in_slot(&config, hose, slot);
    if ( (dev) && is_device(dev) && (dev->tbl->overlay) ) {
#if TURBO
      sprintf(result, "%s%d", dev->tbl->mnemonic, dev->unit );
#endif

#if RAWHIDE
      sprintf(result, "%s%s0", dev->tbl->protocol, controller );
#endif
      strcpy( ev_copy->value.string, result );
      ev_copy->size = strlen( result );
      return msg_success;
    } else {
      return msg_failure;
    }
}

#else

int ev_rd_ctrl( char *name, struct EVNODE *ev, struct EVNODE *ev_copy ) {

    struct pb *pb;
    char *d, *t;
    char *string;
    int dl, tl;
    int n[5];
    int hose, bus, slot, function;
    int status;

    strcpy( ev_copy->value.string, "" );
    ev_copy->size = 0;

    if( ev->value.string[0] == '\0' )
      return msg_failure;

    status = dev2file_parse( ev->value.string, n, &d, &dl, &t, &tl );
    if( status != msg_success )
	return msg_failure;

    hose = n[0];
    bus  = n[1] / 1000;
    slot = n[1] % 100;
    function = ( n[1] % 1000 ) / 100;

    pb = get_matching_pb( hose, bus, slot, function, 0 );

    if ( !pb )
      return msg_failure;

    strcpy( ev_copy->value.string, pb->name );
    ev_copy->size = strlen( pb->name );
}

#endif

int ev_sev( char *name, struct EVNODE *ev )
    {
    char value[EV_VALUE_LEN];

    if( ev_initing )
	return( msg_success );
    if( ev->attributes & EV$K_INTEGER )
	sprintf( value, "%x", ev->value.integer );
    else    
	sprintf( value, "%s", ev->value.string );
#if RHMIN
    return( msg_success );
#else
    return( nvram_write_sev( name, value ) );
#endif
    }

int ev_sev_init( struct env_table *et, char *value )
    {
    int t;           
    
#if !RHMIN
    t = nvram_read_sev( et->ev_name, value );
#else
    t = msg_failure;
#endif
    if( t == msg_success )
	{
	if( ( et->ev_options & EV$M_TYPE ) == EV$K_INTEGER )
	    {
    	    value = xtoi( value );
	    if( et->ev_value == value )
		{
#if 0
		err_printf( "saved EV value is same as default for %s (%x)\n",
			et->ev_name, et->ev_value );
#endif
		nvram_delete_sev( et->ev_name );
		}
	    }
	else
	    {
	    if( strcmp_nocase( et->ev_value, value ) == 0 )
		{
#if 0
		err_printf( "saved EV value is same as default for %s (`%s')\n",
			et->ev_name, et->ev_value );
#endif
		nvram_delete_sev( et->ev_name );
		}
	    }
	}
    else
	{
    	value = et->ev_value;
	}
    return( value );
    } 

#if EV6
int ev_wr_page_table_levels( char *name, struct EVNODE *ev )
    {
    int extra_va;
    struct HWRPB *hwrpb_ptr;

    if( ev->value.string[0] == '4' )
	{
	page_table_levels = 4;
	extra_va = HWRPB$_VA_SIZE - (HWRPB$_PAGEBITS * 4 - 3 * 3);
	}
    else
	{
	page_table_levels = 3;
	extra_va = 0;
	}
    if( hwrpb_ptr = hwrpb )
	hwrpb_ptr->PA_SIZE[1] = extra_va;
    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
    }
#endif

#if SABLE
int ev_wr_pci_arb( char *name, struct EVNODE *ev )
    {
    if( ev->value.string[0] == 'F' )
	set_pci_arb( 0x0499 );
    else if( ev->value.string[0] == 'P' )
	set_pci_arb( 0xc098 );
    else if( ev->value.string[0] == 'R' )
	set_pci_arb( 0xf098 );
    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
    }

#if !LYNX
int ev_wr_pci_read_mult( char *name, struct EVNODE *ev )
    {
    if( ev->value.string[1] == 'F' )
	set_pci_read_mult( 0 );
    else
	set_pci_read_mult( 1 );
    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
    }
#endif

#if GAMMA
int ev_wr_external_mb( char *name, struct EVNODE *ev )
    {
    if( ev->value.string[1] == 'F' )
	set_external_mb( 0 );
    else
	set_external_mb( 1 );
    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
    }
#endif
#endif
#if SABLE || MIKASA || REGATTA || WILDFIRE || K2 || TAKARA || MONET || PC264 || EIGER || FALCON
int ev_wr_controlp( char *name, struct EVNODE *ev )
    {
    if( ev->value.string[1] == 'F' )
	controlp_enable = 0;
    else
	controlp_enable = 1;
    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
    }
#endif

#if SABLE
int ev_scsnode( char **value )
    {
    char *c;
    int n;
    int i;

    c = *value;
    n = strlen( c );
    if( n > 8 )
	return( msg_ev_badvalue );
    for( i = 0; i < n; i++ )
	if( !isalnum( c[i] ) )
	    return( msg_ev_badvalue );
    return( msg_success );
    }

int ev_wr_scsnode( char *name, struct EVNODE *ev )
    {
    char c;
    int n;
    int i;

    if( ev_initing )
	return( msg_success );
    n = strlen( ev->value.string );
    for( i = 0; i < 8; i++ )
	{
	if( i < n )
	    c = toupper( ev->value.string[i] );
	else
	    c = ' ';
	ev->value.string[i] = c;
	}
    ev->value.string[8] = 0;
    ev->size = 8;
    return( ev_sev( name, ev ) );
    }

int ev_scssystemid( char **value )
    {
    int n;

    return( common_convert( *value, 10, &n, sizeof( n ) ) );
    }

int ev_wr_scssystemid( char *name, struct EVNODE *ev )
    {
    int n;
    int t;

    if( ev_initing )
	return( msg_success );
    common_convert( ev->value.string, 10, &n, sizeof( n ) );
    sprintf( ev->value.string, "%d", n );
    ev->size = strlen( ev->value.string );
    return( ev_sev( name, ev ) );
    }

#if MSCP_SERVER
int ev_wr_enable_servers( char *name, struct EVNODE *ev )
    {
    if( strcmp( ev->value.string, "ON" ) == 0 )
	servers_enabled = 1;
    else
	servers_enabled = 0;
    return( ev_sev( name, ev ) );
    }
#endif
#endif

#if !RAWHIDE
int ev_wr_scsi_poll( char *name, struct EVNODE *ev )
    {
    if( strcmp( ev->value.string, "ON" ) == 0 )
	scsi_poll = 1;
    else
	scsi_poll = 0;
    return( ev_sev( name, ev ) );
    }
#endif

int ev_wr_scsi_reset( char *name, struct EVNODE *ev )
    {
    if( strcmp( ev->value.string, "ON" ) == 0 )
	scsi_reset = 1;
    else
	scsi_reset = 0;
    return( ev_sev( name, ev ) );
    }

#if SABLE || RAWHIDE || MIKASA || REGATTA || WILDFIRE || MONET || PC264 || EIGER || FALCON
int ev_com1_baud( char *name, struct EVNODE *ev ) {
    struct set_param_table *p;

    for( p = baud_table; p->param; p++ )
	if( strcmp( p->param, ev->value.string ) == 0 ) {
#if RAWHIDE
	    rtc_write( 0x24, 69 );		/* AlphaBIOS uses TOY<15:0E> */
	    rtc_write( 0x25, p - baud_table );
#else
	    set_com1_baud( p - baud_table );
#endif
	    com1_baud = p->code;
#if !RHMIN
	    set_serial_params( COM1, com1_baud, com1_flow, com1_modem );
	    if( cpip )
		krn$_sleep( 100 );
	    if( !cbip )
		init_serial_port( 0, COM1, com1_baud );
#endif
	}
    return( msg_success );
}

int ev_com1_baud_init( struct env_table *et, char *value )
    {
    int data;

#if RAWHIDE
    if( rtc_read( 0x24 ) == 69 )	/* AlphaBIOS uses TOY<15:0E> */
	data = rtc_read( 0x25 );
    else
	data = -1;
#else
    data = get_com1_baud( );
#endif
    if( ( data >= 0 ) && ( data < (sizeof(baud_table)/sizeof(baud_table[0])) ) )
	return( baud_table[data].param );
    else
	return( et->ev_value );
    }

int ev_com1_modem( char *name, struct EVNODE *ev )
    {
    if( ev->value.string[1] == 'F' )
	com1_modem = 0;
    else
	com1_modem = 1;
#if !RHMIN
    set_serial_params( COM1, com1_baud, com1_flow, com1_modem );
#endif
    if( ev_initing )
	return( msg_success );
#if REGATTA
    set_com1_modem( com1_modem );
    return( msg_success );
#else
    return( ev_sev( name, ev ) );
#endif
    }

int ev_com1_flow( char *name, struct EVNODE *ev )
    {
    struct set_param_table *p;

    for( p = flow_table; p->param; p++ )
	if( strcmp( p->param, ev->value.string ) == 0 )
	    {
	    com1_flow = p->code;
#if !RHMIN
	    set_serial_params( COM1, com1_baud, com1_flow, com1_modem );
#endif
	    }
    if( ev_initing )
	return( msg_success );
#if REGATTA || WEBBRICK || WILDFIRE
    set_com1_flow( com1_flow );
    return( msg_success );
#else
    return( ev_sev( name, ev ) );
#endif
    }

int ev_com2_baud( char *name, struct EVNODE *ev )
    {
    struct set_param_table *p;

    for( p = baud_table; p->param; p++ )
	if( strcmp( p->param, ev->value.string ) == 0 ) {
	    com2_baud = p->code;
#if !RHMIN
	    set_serial_params( COM2, com2_baud, com2_flow, com2_modem );
	    if( !cbip )
		init_serial_port( 0, COM2, com2_baud );
#endif
	}
    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
    }

int ev_com2_modem( char *name, struct EVNODE *ev )
    {
    if( ev->value.string[1] == 'F' )
	com2_modem = 0;
    else
	com2_modem = 1;
#if !RHMIN
    set_serial_params( COM2, com2_baud, com2_flow, com2_modem );
#endif
    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
    }

int ev_com2_flow( char *name, struct EVNODE *ev )
    {
    struct set_param_table *p;

    for( p = flow_table; p->param; p++ )
	if( strcmp( p->param, ev->value.string ) == 0 )
	    {
	    com2_flow = p->code;
#if !RHMIN
	    set_serial_params( COM2, com2_baud, com2_flow, com2_modem );
#endif
	    }
    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
    }
#endif

#if SABLE || MIKASA || K2 || TAKARA || EB164

#define BPA_NEW 69
#define BPA_OLD 0

int ev_bus_probe_algorithm( char *name, struct EVNODE *ev )
    {
    unsigned char data;

    if( ev->value.string[0] == 'n' )
	data = BPA_NEW;
    else
	data = BPA_OLD;
    rtc_write( 0x0f, data );
    return( msg_success );
    }

int ev_bus_probe_algorithm_init( struct env_table *et, char *value )
    {
#if CORELLE
    bus_probe_algorithm = 1;
    return( "new" );
#else
    unsigned char data;

    data = rtc_read( 0x0f );
    if( data != BPA_OLD )	/* the default is "new" */
	{
	bus_probe_algorithm = 1;
	return( "new" );
	}
    else
	{
	bus_probe_algorithm = 0;
	return( "old" );
	}
#endif
    }
#endif

#define CMA_NEW 0x35
#define EV_MEMORY_ALLOC 0x22

#if CLIPPER || WEBBRICK || PC264 || SX164 || LX164
int ev_console_memory_allocation( char *name, struct EVNODE *ev )
    {
    unsigned char data;

    if( ev->value.string[0] == 'n' )
	data = CMA_NEW;
    else
	data = 0;
    rtc_write( EV_MEMORY_ALLOC, data );
    return( msg_success );
    }

int ev_console_memory_allocation_init( struct env_table *et, char *value )
    {
    unsigned char data;

    data = rtc_read( EV_MEMORY_ALLOC );
    if( data != CMA_NEW )	/* the default is "old" */
	return( "old" );
    else
	return( "new" );
    }
#endif

#if CLIPPER
int ev_prefetch_mode( char *name, struct EVNODE *ev )
    {

    if( strcmp( ev->value.string, "ON" ) == 0 )
	sbe_prefetch_bits = 3;
    else
	sbe_prefetch_bits = 0;
    
    prefetch_onoff( sbe_prefetch_bits );

    return( ev_sev( name, ev ) );
    }
#endif

#if WILDFIRE
int ev_dma_read_mode( char *name, struct EVNODE *ev )
    {
    int hose;
    int slot;
    int mode;
    int mode1_prefetches;
    int i;
    char *c;
    char t[4];

    if( ev_initing )
	return( msg_success );
#if 0
    pprintf( "%s\n", ev->value.string );
#endif

    c = ev->value.string;

    for( i = 0; i < 2; i++ )
	{
	t[i] = *c;
	c++;
	if( *c == ' ' )	
	    break;
	}
    hose = atoi( t );
#if 0
    printf( "hose = %d\n", hose );
#endif

    c++;
    memset( t, 0, sizeof( t ) );
    for( i = 0; i < 2; i++ )
	{
	t[i] = *c;
	c++;
	if( *c == ' ' )	
	    break;
	}
    slot = atoi( t );
#if 0
    pprintf( "slot = %d\n", slot );
#endif

    c++;
    if( *c == NULL )
	return( msg_insuff_params );

    memset( t, 0, sizeof( t ) );
    for( i = 0; i < 2; i++ )
	{
	t[i] = *c;
	c++;
	if( *c == ' ' )	
	    break;
	}
    mode = atoi( t );
#if 0
    pprintf( "mode = %d\n", mode );
#endif

    mode1_prefetches = 0;
    if( mode == 1 )
	{
	c++;
	if( *c == NULL )
	    return( msg_insuff_params );
	memset( t, 0, sizeof( t ) );
	for( i = 0; i < 2; i++ )
	    {
	    t[i] = *c;
	    c++;
	    if( *c == ' ' )	
		break;
	    }
	mode1_prefetches = atoi( t );
#if 0
	pprintf( "mode1_prefetches = %d\n", mode1_prefetches );
#endif
	}
    if( set_dma_read_mode( hose, slot, mode, mode1_prefetches ) == msg_success )
	return( msg_success );
    else
	return( msg_failure );
    }
#endif

#if SABLE
#define EV_CONSOLE_TOY_OFFSET 0x0e
#else
#define EV_CONSOLE_TOY_OFFSET 0x17
#endif

#if MONET
#define GC_FALSE 2
#else
#define GC_FALSE 0
#endif

#define GC_TRUE 1


int ev_console( char *name, struct EVNODE *ev )
    {
    unsigned char data;

#if AVMIN
    unsigned char quick_start;

    quick_start = rtc_read(0x3e);
#endif

    if( ev->value.string[0] == 'g' ) {
#if AVMIN
        quick_start = 0;
#endif
	data = GC_TRUE;
    } else
	data = GC_FALSE;
#if !IMP
#if FALCON
    boot_ev_write( EV_CONSOLE_TOY_OFFSET, data );
#else
    rtc_write( EV_CONSOLE_TOY_OFFSET, data );
#endif
#endif
#if AVMIN
    rtc_write(0x3e, quick_start);
#endif
    return( msg_success );
    }

int ev_console_init( protonoargs struct env_table *et, char *value )
    {
    unsigned char data;

#if !IMP
#if FALCON
    data = boot_ev_read( EV_CONSOLE_TOY_OFFSET );
#else
    data = rtc_read( EV_CONSOLE_TOY_OFFSET );
#endif
#else
    data = 0;
#endif

#if MONET

/* forces default to graphics */
    if (data != GC_TRUE && data != GC_FALSE && et)
    {
        char *tempPointer = (char *)et->ev_value;
        if (tempPointer[0] == 'g')
        {
            data = GC_TRUE;
        }
        else
        {
            data = GC_FALSE;
        }
    }

#endif
#if GALAXY && REGATTA
    if (get_console_base_pa())
	data = GC_FALSE;
#endif

    if( data == GC_TRUE)
	{
	graphics_console = 1;
	return( "graphics" );
	}
    else
	{
	graphics_console = 0;
	return( "serial" );
	}
    }

#if SABLE || RAWHIDE || REGATTA || MONET || PC264 || EIGER || FALCON
int ev_wr_pci_parity( char *name, struct EVNODE *ev )
    {
    if( ev->value.string[1] == 'F' )
	set_pci_parity( 0 );
    else
	set_pci_parity( 1 );
    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
    }
#endif

#if MONET
int ev_wr_usb_enable( char *name, struct EVNODE *ev )
{
    if( ev->value.string[1] == 'F' )
	set_usb_enable( 0 );
    else
	set_usb_enable( 1 );

    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
}
#endif

#if RAWHIDE
int ev_wr_pci_arb( char *name, struct EVNODE *ev )
    {
    if( ev->value.string[0] == 'B' )
	set_pci_arb( 0 );
    else if( ev->value.string[0] == 'R' )
	set_pci_arb( 1 );
    else if( ev->value.string[0] == 'M' )
	set_pci_arb( 2 );
    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
    }

int ev_wr_pci_req64( char *name, struct EVNODE *ev ) {
    struct pb pb;
    struct device *dev;
    int i;

    if( ev->value.string[0] != NULL ) {
	if( ev->value.string[1] == 'F' )
	    set_pci_req64( 0 );                   /* OFF */
	else
	    set_pci_req64( 1 );                   /* ON */
    }
    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
}

int validate_req64_value(int *value)
{
    int i, data;
    char buf[80];

    /* 
     * Rev D Horse modules have all of the right ECO work applied to them to
     * allow support for 64-bit PCI (REQ64). V1.2-4 console was released as a
     * hack work-around to disable REQ64 on all systems due to an issue in MFG
     * around 64-bit PCI (where REQ64 on PCI1 could inadvertently reset PCI0).
     */

    if( strcmp( *value, "" ) != 0 )
	if ( ev_validate_table(value, onoff_ascii) != msg_success )
	    return msg_failure;

    if( strcmp( *value, "ON" ) == 0 ) {
	data = *(unsigned __int64*) SYSTEM_CSR(GLOBAL_ID(whoami()), IOD0, PCI_REV);
	if ( ( ( data & HORSE_REV ) >> 4 ) < 3 ) {
	    read_with_prompt("Your PCI Bridge should be at Rev D or greater. Override? (Y/[N]) ",sizeof(buf),buf,0,0,1);
	    if (toupper(buf[0]) == 'Y')
		return msg_success;
	    else
		return msg_failure;
	}
    }
    return msg_success;
}

int ev_wr_shutdown_temp( char *name, struct EVNODE *ev ) {
    unsigned char data[4];
    struct FILE *fp;

    if ( fp = fopen( "iic_rcm_temp", "sr+" ) ) {
	data[0] = THRESHOLD_CSR;
	data[1] = atoi(ev->value.string);    /* Get user-specified temp */
        if ( tincup() )                      /* If Tincup, add 9 degrees C since */
	    data[1] += 9;                    /* temp sensor on hot side of box */
	data[2] = 0;                         /* We won't support half degrees */
	data[3] = 0;
	fwrite( data, 4, 1, fp );            /* Write the DS1625's TH register */
        fclose( fp );                        /* Not sure why need to close/open */
	fp = fopen( "iic_rcm_temp", "sr+" );
	data[0] = HYSTERESIS_CSR;
	data[1] -= 1;                        /* Set hysteresis 1 degrees less */
	fwrite( data, 4, 1, fp );            /* (than shutdown threshold) */
        fclose( fp );
    } else {
	qprintf("Could not open temp sensor device (on RCM)\n");
	return msg_failure;
    }

    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
}

int ev_rd_shutdown_temp( char *name, struct EVNODE *ev, struct EVNODE *ev_copy ) {

    if( ev_initing )
	return msg_success;

    if( tincup() )
	strcpy( ev_copy->value.string, ev->value.string );
    else
	strcpy( ev_copy->value.string, "51" );

    return msg_success;
}

int validate_temp_value(int *value) {
    int temp;
    unsigned char data[1];
    struct FILE *fp;

    if ( !tincup() ) {
	err_printf("Shutdown temperature not modifiable on the 4000/4100 platform\n");
	return msg_failure;
    }

    if ( fp = fopen( "iic_rcm_temp", "sr+" ) ) {
	data[0] = CONTINUOUS_MODE;
	fwrite( data, 1, 1, fp );
	data[0] = READ_AMBIENT;
	fwrite( data, 1, 1, fp );
	fread( data, 1, 1, fp );             /* Get ambient temperature.. */
        fclose( fp );
    } else {
	err_printf("Cannot read ambient temperature from RCM\n");
	return msg_failure;
    }

    temp = atoi(*value);

   /*
    * Don't let user select threshold temperature unless 10 degrees C more
    * than current ambient.. Maximum threshold value supported by DS1625
    * is 127 degrees C.
    */

    data[0] -= 9;                            /* Subtract 9 degrees C, if Tincup */
    if ( temp < data[0] + 10 ) {
	err_printf("Shutdown temp must be 10 or more degrees above current ambient temp (%d degrees C)\n", data[0]);
	return msg_failure;
    }
    if ( temp > 127 ) {
	err_printf("Shutdown temp must be less than 128 degrees C\n");
	return msg_failure;
    }

    return msg_success;

}
#endif
#if RAWHIDE || REGATTA || PC264 || FALCON

#define EV_MEMORY_TEST_TOY_OFFSET 0x21
#define MEMORY_TEST_NONE 0xDE
int ev_memory_test( char *name, struct EVNODE *ev ) {
    unsigned char data;

    if ( ev->value.string[0] == 'p' ) {
	data = EV_MEMORY_TEST_TOY_OFFSET;
    } else {
	if ( ev->value.string[0] == 'n' )
	    data = MEMORY_TEST_NONE;
	else
	    data = 0;
    }
#if FALCON
    boot_ev_write( EV_MEMORY_TEST_TOY_OFFSET, data );
#else
    rtc_write( EV_MEMORY_TEST_TOY_OFFSET, data );
#endif
    return( msg_success );
}

int ev_memory_test_init( struct env_table *et, char *value ) {
    unsigned char data;

#if FALCON
    data = boot_ev_read( EV_MEMORY_TEST_TOY_OFFSET );
#else
    data = rtc_read( EV_MEMORY_TEST_TOY_OFFSET );
#endif
    if ( data == EV_MEMORY_TEST_TOY_OFFSET ) {
	return( "partial" );
    } else {
	if ( data == MEMORY_TEST_NONE )
	    return( "none");
	else
	    return( "full" );
    }
}
#endif

#if TGA
int ev_tga_sync ( char *value, struct EVNODE *ev )
    {
    rtc_write( TgaToyOffset, ev->value.integer);
    return( msg_success );
    }

int ev_tga_sync_init ( struct env_table *et, char *value )
    {
    return (rtc_read(TgaToyOffset));
    }
#endif


#if !(MEDULLA || CORTEX || YUKONA)

/* ev_os_type() - remove preprocessor directves AVMIN & IMP */

int ev_os_type( char *name, struct EVNODE *ev )
    {
    unsigned char data;
    unsigned char console_type;
    unsigned char year;


/* pick up existing console type */

#if FALCON
    console_type = boot_ev_read( EV_CONSOLE_TOY_OFFSET );
#else
    console_type = rtc_read( EV_CONSOLE_TOY_OFFSET );
#endif

    if( ev->value.string[1] == 'T' )
    {
#if MNSPLIT
	data = 2;		/* If NT, force to VMS */
	console_type = 2;
#else
	data = 1;
	console_type = 1;
#endif
    }

    else if (( ev->value.string[1] == 'S' ) ||
             ( ev->value.string[1] == 'N' ) )
	data = 3;
    else if ( ev->value.string[1] == 'i' )
	data = 4;
    else
	data = 2;

#if FALCON
    boot_ev_write( 0x3f, data );
#else
    rtc_write( 0x3f, data );
#endif

#if !RAWHIDE
    if( !ev_initing )
	{
#if FALCON
        if( boot_ev_read( EV_CONSOLE_TOY_OFFSET ) != console_type )
#else
	if( rtc_read( EV_CONSOLE_TOY_OFFSET ) != console_type )
#endif
	    {
	    err_printf( "console environment variable set to graphics\n" );
	    err_printf( "type init or reset the system for the change to take effect\n" );
#if FALCON
            boot_ev_write( EV_CONSOLE_TOY_OFFSET, console_type );
#else
	    rtc_write( EV_CONSOLE_TOY_OFFSET, console_type );
#endif
	    }
	}
#endif
#if 0
    year = rtc_read(0x09);

    if (data == 1) {
	if ((year >= 93) && (year <= 99)) {
	    year -= 80;
	}
	else if ((year >= 0 ) && (year <= 12)) {
	    year += 20;
	}	
    }
    else {
	if ((year >= 13) && (year <= 19)) {
	    year += 80;
	}
	else if ((year >= 20 ) && (year <= 40)) {
	    year -= 20;
	}
    }

    rtc_write(0x09, year);

#endif
    return( msg_success );
    }


int ev_os_type_init( struct env_table *et, char *value )
    {
    unsigned char data;

#if !IMP
#if FALCON
    data = boot_ev_read( 0x3f );
#else
    data = rtc_read( 0x3f );
#endif
#else
    data = 2;
#endif
    if( data == 1 )
	return( "NT" );
    else if( data == 3 )
	return( "UNIX" );
    else if( data == 4 )
	return( "Linux" );
    else
	return( "OpenVMS" );
    }
#endif

#if AVMIN
int ev_quick_start( char *name, struct EVNODE *ev )
{
    unsigned char data;
    unsigned char console_type;

#if !IMP
    console_type = rtc_read( EV_CONSOLE_TOY_OFFSET );
#endif
    /*
     * Allow to set only if console is set to serial
     */
    if (console_type == 0) {
	if( strncmp( ev->value.string, "ON", 2) == 0 ) {
	    data = 0x31;
	} else {
	    data = 0;
	}
#if !IMP
	rtc_write(0x3e, data );
#endif
    }
    return( msg_success);
}

int ev_quick_start_rd(char *name, struct EVNODE *evp, struct EVNODE *evp_copy)
{
    unsigned char data;

#if !IMP
    data = rtc_read(0x3e);
#else
    data = 0;
#endif
    if( data == 0x31 ) 	{
	strcpy(evp_copy->value.string, "ON");
    } else {
	strcpy(evp_copy->value.string, "OFF");
    }

    return msg_success;
}

int ev_quick_start_init( struct env_table *et, char *value )
{
    unsigned char data;

#if !IMP
    data = rtc_read(0x3e);
#else
    data = 0;
#endif
    if( data == 0x31 ) 	{
	return( "ON" );
    } else {
	return( "OFF" );
    }
}
#endif

#if TAKARA || K2 || EIGER
int ev_dmcc_psu_sense(char *name, struct EVNODE *ev)
    {
    if( ev->value.string[1] == 'F' )
	dmcc_psu_sense = 0;
    else
	dmcc_psu_sense = 1;
    if( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
    }
#endif

/*+
 * ============================================================================
 * = ev_wr_tt_allow_login - write the tt_allow_login environment variable     =
 * ============================================================================
 *                                 
 * OVERVIEW:
 *
 *	Turn tt_allow_login global variable flag off if the value of 
 *	environment variable is being set to 0.
 *                                           
 * FORM OF CALL:                                                          
 *
 *	ev_wr_tt_allow_login (name, ev)
 *                      
 * RETURN CODES:
 *   
 *     	msg_success	  
 *                                 
 * ARGUMENTS:         
 *
 *      char *name	  - string containing name of the environment variable
 *   	struct EVNODE *ev - address of a pointer to an evnode.
-*/             

int ev_wr_tt_allow_login(char *name, struct EVNODE *ev) {
struct TTPB *ttpb;

/*Set all the login enables*/
	spinlock (&spl_kernel);
	ttpb = tt_pbs.flink;
	while (ttpb != &tt_pbs.flink) {
	    if (ttpb != console_ttpb)
		ttpb->login_ena = ev->value.integer;
	    ttpb = ttpb->ttpb.flink;
	}
	spinunlock (&spl_kernel);

#if SABLE || RAWHIDE || WILDFIRE
	return( ev_sev( name, ev ) );	/* write to non-volatile RAM */
#else
	return msg_success;
#endif
}                                                                         

#if !RAWHIDE
/*+
 * ============================================================================
 * = ev_sys_serial_num_wr - write system serial# environment variable to HWRPB=
 * ============================================================================
 *                                 
 * OVERVIEW:
 *
 *	Copy the system serial number environment variable into the HWRPB.
 *                                           
 * FORM OF CALL:                                                          
 *
 *	ev_sys_serial_num_wr (name, ev)
 *                      
 * RETURN CODES:
 *   
 *     	msg_success	  
 *                                 
 * ARGUMENTS:         
 *
 *      char *name	  - string containing name of the environment variable
 *   	struct EVNODE *ev - address of a pointer to an evnode.
-*/             
int ev_sys_serial_num_wr (char *name, struct EVNODE *ev) {
    struct HWRPB *hwrpb_ptr;

    hwrpb_ptr = hwrpb;
    if (!hwrpb_ptr || (!hwrpb_valid( hwrpb_ptr)))
	return msg_failure;
    strncpy( (char *) hwrpb_ptr->SERIALNUM, ev->value.string,
			sizeof( hwrpb_ptr->SERIALNUM));
    twos_checksum((int *)hwrpb_ptr,offsetof(struct HWRPB,CHKSUM)/4,hwrpb_ptr->CHKSUM);

    if( ev_initing )
	return( msg_success );

    gct_sys_serial_write( ev->value.string );  /* propagate into FRU IIc */

    return( ev_sev( name, ev ) );	/* write ser# to non-volatile RAM */
}                                                                         
#endif

#if RAWHIDE
int ev_sys_serial_num_wr (char *name, struct EVNODE *ev) {
    struct HWRPB *hwrpb_ptr;
    int status = msg_success;

    if( ev_initing )
	return( msg_success );

    /* Write to iic eeprom */

    status = write_system_serial( ev->value.string );	

    /* Write to HWRPB */

#if !RHMIN
    if (status == msg_success) {  
	hwrpb_ptr = hwrpb;
	if (!hwrpb_ptr || (!hwrpb_valid( hwrpb_ptr)))
	    return msg_failure;
	strncpy( (char *) hwrpb_ptr->SERIALNUM, ev->value.string, 10);
	twos_checksum((int *)hwrpb_ptr,offsetof(struct HWRPB,CHKSUM)/4,hwrpb_ptr->CHKSUM);
    }
#endif
    return status;
}

int ev_sys_serial_init( struct env_table *et, char *value )
    {
    int t;           
    
    t = read_system_serial( value );
    if( t == msg_success )
	{
	if( ( et->ev_options & EV$M_TYPE ) == EV$K_INTEGER )
    	    value = xtoi( value );
	}
    else
	{
    	value = et->ev_value;
	}
    return( value );
    } 

#define SERIAL_OFFSET 12
#define SERIAL_SIZE 5
int read_system_serial( char *system_serial ) {

    int status = msg_failure;
    char serial_num[16];

#if !RHMIN
    status = read_i2c_eeprom("iic_mthrbrd", SERIAL_OFFSET, SERIAL_SIZE, serial_num );
    if ( status == msg_success )
	fru_decode_sn_scv3( *(unsigned __int64*) serial_num, system_serial );
#else
    status = msg_success;
#endif
    return status;
}

int write_system_serial( char *system_serial ) {

    int status = msg_failure;
    char serial_num[16];

    *(unsigned __int64*) serial_num = fru_encode_sn( system_serial );
#if !RHMIN
    status = write_i2c_eeprom("iic_mthrbrd", SERIAL_OFFSET, SERIAL_SIZE, serial_num );
#else
    status = msg_success;
#endif
    return status;
}

int validate_model_num(char **value)
{
    int status;
    char *model;

    if ( ev_validate_table(value, sys_model_table) != msg_success )
        return msg_failure;

    model = *value;
    if( !(strcmp( *value, "1200" )) || (model[0] == '5') ) {
        if ( tincup() )
	    status = msg_success;
    } else {
        if ( !tincup() )
	    status = msg_success;
    }

    if ( !( *(UINT *) (SYSTEM_CSR(GLOBAL_ID(whoami()), IOD0, SCRATCH_REG)) & NTONLY) )
	if( model[1] == '3' )
	    status = msg_failure;

    if ( status == msg_failure )
	err_printf("This is not a %s platform!\n", *value);

    return status;
}

int validate_variant(char **value)
{
    int status = msg_success;

    if ( ev_validate_table(value, onoff) != msg_success )
        status = msg_failure;

/*
 * Only applicable for Tincup (AlphaServer 1200) and DaVinci (DIGITAL Ultimate
 * Workstation). Don't let customer turn a 4X00/7X00 into a Workstation.
 */
    if ( strcmp( *value, "0" ) && !( tincup() ) )
        status = msg_failure;

    return status;
}

int validate_cpu_mask(int *value)
{
    int mask = *value;
    int id = whoami();

    if ( (mask <= 0xff) && (mask > 0) ) {
      if ( !(mask & (1<<id)) ) { 
        err_printf("Disabling the primary CPU may produce unpredictable system behavior!\n");
        return msg_failure;
      }
      return msg_success;
    }

    err_printf("Invalid CPU mask\n");
    return msg_failure;
}

int validate_com1_baud(char **value)
{
    int i;
    char srev[8];
    char buf[80];

    if (cbip)
	return msg_success;

    if ( ev_validate_table(value, baud_table) != msg_success )
	return msg_failure;

    if( strcmp( *value, "9600" ) != 0 ) {
	for ( i = 0; i < MAX_CPU_CNT; i++ )
	    if ( cpu_mask & (1 << i) ) {
		get_srom_firmware_rev(&srev[0], i);
		if ( strcmp( srev, "V1.1" ) == 0 ) {
		    err_printf("Serial ROM must be at V2.0 or greater to support alternate baud rates.\n");
		    return msg_failure;
		}
	    }
	read_with_prompt("Embedded Remote Console only supports 9600 baud. Continue? (Y/[N]) ",sizeof(buf),buf,0,0,1);
	if (toupper(buf[0]) != 'Y')
	    return msg_failure;
    }
    return msg_success;
}

int validate_com1_flow(char **value)
{
    struct set_param_table *p;
    char buf[80];

    if (cbip)
	return msg_success;

    if ( ev_validate_table(value, flow_table) != msg_success )
	return msg_failure;

    for( p = flow_table; p->param; p++ )
	if( strcmp( *value, p->param ) == 0 )
	    if ( p->code >= 2 ) {		/* If "hardware" or "both" */
		read_with_prompt("You must bypass embedded Remote Console. Continue? (Y/[N]) ",sizeof(buf),buf,0,0,1);
		if (toupper(buf[0]) != 'Y')
		    return msg_failure;
	    }
    return msg_success;
}
#endif

#if MEDULLA 
int ev_wr_pciarb( char *name, struct EVNODE *ev ) 
{
unsigned char mod_ctrl;
unsigned char arb_val;
char mask = 0x18;

    if( ev_initing )
	return( msg_success );

    mod_ctrl = read_module_register( MOD_CTRL_REG );
    mod_ctrl &= 0xf3;

    if((ev->attributes & EV$M_UNUSED) == 0) /* ev_validate_table passed so we can write regs */
      {
      /* Check for prototype hardware, do not allow mode 1 and 3 to be set.
       * This can be removed after field test. 
       */
      if((!(mask &= modcnfg_read())) && (ev->value.string[0] == '1' || ev->value.string[0] == '3')) 
        {
        return(msg_ev_badvalue);
        }
      else
        {
        arb_val = ((ev->value.integer & 0x3)<<2);
        mod_ctrl |= arb_val;
        write_module_register( MOD_CTRL_REG, mod_ctrl );
        return( ev_sev( name, ev ) );
        }
      }
}

int ev_wr_pcipark( char *name, struct EVNODE *ev )
{
unsigned char mod_ctrl;
unsigned char arb_val;

    if( ev_initing )
	return( msg_success );

    if((ev->attributes & EV$M_UNUSED) == 0) /* ev_validate_table passed so we can write regs */
      {
      mod_ctrl = read_module_register( MOD_CTRL_REG );
      mod_ctrl &= 0x3f;
      arb_val = ((ev->value.integer & 0x3)<<6);
      mod_ctrl |= arb_val;
      write_module_register( MOD_CTRL_REG, mod_ctrl );
      }
    return( ev_sev( name, ev ) );
}

int ev_wr_pci_int_sel( char *name, struct EVNODE *ev )
{
unsigned char mod_ctrl;
unsigned char sel_val;

    if( ev_initing )
	return( msg_success );
    if((ev->attributes & EV$M_UNUSED) == 0) /* ev_validate_table passed so we can write regs */
      {
      mod_ctrl = read_module_register( MOD_CTRL_REG );
      mod_ctrl &= 0xfd;
      sel_val = ((ev->value.integer & 0x1)<<1);
      mod_ctrl |= sel_val;
      write_module_register( MOD_CTRL_REG, mod_ctrl );
      }
    return( ev_sev( name, ev ) );
}

/*
 * Specific 'init' routine for tt_baud EV that does the following:
 *
 *	1. Reads value from NVRAM 
 *	2. Update the baud rate fields of the CTB of the HWRPB
 */
int ev_ttbaud_init( struct env_table *et, char *value )
{
 int t;           
 struct HWRPB  *hwrpb_ptr;
 struct CTB_VT *ctb;
 int unit;
    
 t = nvram_read_sev( et->ev_name, value );
 if ( t == msg_success )
   {
    if ( ( et->ev_options & EV$M_TYPE ) == EV$K_INTEGER )
       value = xtoi( value );
   }
 else
   {
    value = et->ev_value;
   }

 /*
  * Write new baud rate values to CTB of the HWRPB
  */
 hwrpb_ptr = hwrpb;
 unit = 0;
 ctb = (struct CTB_VT *)((int)hwrpb_ptr + *hwrpb_ptr->CTB_OFFSET + *hwrpb_ptr->CTB_SIZE * unit);

 /*
  * Update Baud Rate for Unit 0 (Channel A) CTB
  */
#if TGA
 if(!graphics_console) {   /* If graphics, ctb type is CTB$Q_DZ_WS. So we can't
                              update it */
 ctb->CTB_VT$Q_BAUD[0] = atoi(value);
 ctb->CTB_VT$Q_BAUD[1] = 0;
 }
#else
 ctb->CTB_VT$Q_BAUD[0] = atoi(value);
 ctb->CTB_VT$Q_BAUD[1] = 0;
#endif
 unit = 1;
 ctb = (struct CTB_VT *)((int)hwrpb_ptr + *hwrpb_ptr->CTB_OFFSET + *hwrpb_ptr->CTB_SIZE * unit);

 /*
  * Update Baud Rate for Unit 1 (Channel B) CTB
  */
 ctb->CTB_VT$Q_BAUD[0] = atoi(value);
 ctb->CTB_VT$Q_BAUD[1] = 0;

 return( value );
} 

/*
 * Specific 'write' routine for tt_baud EV that does the following:
 *
 *	1. Update NVRAM bytes (in private NVRAM area) for new baud rate
 *	2. Update the baud rate fields of the CTB of the HWRPB
 *	3. Update WR12 and WR13 in Channel A and B of the Z8530 UART
 */
int ev_wr_ttbaud( char *name, struct EVNODE *ev )
{
 int baud_rate;
 unsigned char wr12;
 unsigned char wr13;
 unsigned char checksum;
 struct HWRPB  *hwrpb_ptr;
 struct CTB_VT *ctb;
 struct CONSOLE_NVRAM *nv_ptr = CONSOLE_NVRAM_BASE;
 int unit;
 
 if( ev_initing ) return( msg_success );

 baud_rate = atoi(ev->value.string);

 switch (baud_rate)
  {
   case 300:	wr12 = 0x7e;
		wr13 = 0x06;
		checksum = 0x27;
		break;

   case 600:	wr12 = 0x3f;
		wr13 = 0x03;
		checksum = 0x69;
		break;

   case 1200:	wr12 = 0x9f;
		wr13 = 0x01;
		checksum = 0x0b;
		break;

   case 2400:	wr12 = 0xce;
		wr13 = 0x00;
		checksum = 0xdd;
		break;

   case 4800:	wr12 = 0x66;
		wr13 = 0x00;
		checksum = 0x45;
		break;

   case 9600:	wr12 = 0x32;
		wr13 = 0x00;
		checksum = 0x79;
		break;

   case 19200:	wr12 = 0x18;
		wr13 = 0x00;
		checksum = 0x93;
		break;

   default:     pprintf("Invalid baud rate\n");
		return (msg_ev_badvalue);
  }

 /*
  * Write new baud rate values to private NVRAM locations
  */
 outportb(NULL, &nv_ptr->wr13_uart, wr13);
 outportb(NULL, &nv_ptr->wr12_uart, wr12);
 outportb(NULL, &nv_ptr->uart_checksum, checksum);

 /*
  * Write new baud rate values to CTB of the HWRPB
  */
 hwrpb_ptr = hwrpb;
 unit = 0;
 ctb = (struct CTB_VT *)((int)hwrpb_ptr + *hwrpb_ptr->CTB_OFFSET + *hwrpb_ptr->CTB_SIZE * unit);

 /*
  * Update Baud Rate for Unit 0 (Channel A) CTB
  */
#if TGA
 if(!graphics_console) {   /* If graphics, ctb type is CTB$Q_DZ_WS. So we can't
                              update it */
   ctb->CTB_VT$Q_BAUD[0] = baud_rate;
   ctb->CTB_VT$Q_BAUD[1] = 0;
 }
#else
 ctb->CTB_VT$Q_BAUD[0] = baud_rate;
 ctb->CTB_VT$Q_BAUD[1] = 0;
#endif
 unit = 1;
 ctb = (struct CTB_VT *)((int)hwrpb_ptr + *hwrpb_ptr->CTB_OFFSET + *hwrpb_ptr->CTB_SIZE * unit);

 /*
  * Update Baud Rate for Unit 1 (Channel B) CTB
  */
 ctb->CTB_VT$Q_BAUD[0] = baud_rate;
 ctb->CTB_VT$Q_BAUD[1] = 0;

 /*
  * Write new baud rate values to Ch. A and Ch. B UART
  */
 spinlock (&spl_kernel);
 outportb(NULL, CHA_RD_WR_ADDR, 12);
 outportb(NULL, CHA_RD_WR_ADDR, wr12);
 outportb(NULL, CHA_RD_WR_ADDR, 13);
 outportb(NULL, CHA_RD_WR_ADDR, wr13);
  
 outportb(NULL, CHB_RD_WR_ADDR, 12);
 outportb(NULL, CHB_RD_WR_ADDR, wr12);
 outportb(NULL, CHB_RD_WR_ADDR, 13);
 outportb(NULL, CHB_RD_WR_ADDR, wr13);
 spinunlock (&spl_kernel);
  
 /*
  * Update regular NVRAM EV
  */
 return( ev_sev( name, ev ) );
}
#endif

#if MODULAR || MONET
/*+
 * ============================================================================
 * = ev_term_type_wr - action routine for 'term_type' environment variable    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Validate the value specified for the console terminal type.
 *
 * FORM OF CALL:
 *
 *	ev_term_type_wr ()
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *	msg_ev_badvalue - illegal value specified 
 *
 * ARGUMENTS:
 *
 *      char *name	  - string containing name of the environment variable
 *   	struct EVNODE *ev - address of a pointer to an evnode.
 *
 * SIDE EFFECTS:
 *
 *      Environment variable lists referenced.
 *	The EVLOCK semaphore has been taken out.
-*/
int ev_term_type_wr (char *name, struct EVNODE *ev)
{
    int console_type;
    struct TTPB *ttpb;
    struct COM_DEVTAB *cp;

    console_type = 0;

    if (ev->value.string[0] == 'V')
	console_type = 0;

    if (ev->value.string[0] == 'H')
	console_type = 1;

#if !RHMIN
    cp = com_devtab_ptr;
    ttpb = (*cp).ttpb;
    ttpb->type = console_type;
#endif

    return( ev_sev( name, ev ) );	/* write to non-volatile RAM */
}

int ev_term_type_rd (char *name, struct EVNODE *ev)
{
    int console_type;
    struct TTPB *ttpb;
    struct COM_DEVTAB *cp;

    console_type = 0;

    if (ev->value.string[0] == 'V')
	console_type = 0;

    if (ev->value.string[0] == 'H')
	console_type = 1;

#if !RHMIN
    cp = com_devtab_ptr;
    ttpb = (*cp).ttpb;
    ttpb->type = console_type;
#endif
    return msg_success;
}
/*+
 * ============================================================================
 * = ev_term_page_wr - action routine for 'page' environment variable         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Setup the value specified for the console terminal page size.
 *
 * FORM OF CALL:
 *
 *	ev_term_page_wr (evptr)
 *
 * RETURN CODES:
 *
 *	msg_success	- Success.
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	The terminal page size variable is set.
 *
-*/
int ev_term_page_wr(char *name, struct EVNODE *ev)
{
    int page;
    int status;
    struct TTPB *ttpb;
    struct COM_DEVTAB *cp;
/* 
 * Convert the ASCII number to decimal.
 * Return failure if a decimal # wasn't entered.
 */
    status =  common_convert (ev->value.string, 10, &page, 4);
    if (status) {
	err_printf(status);
	return status;
    }
#if !RHMIN
    cp = com_devtab_ptr;
    ttpb = (*cp).ttpb;
    ttpb->page = page;
    ttpb->spage = page;
#endif
    return( ev_sev( name, ev ) );	/* write to non-volatile RAM */
}
int ev_term_page_rd(char *name, struct EVNODE *ev)
{
    int page;
    int status;
    struct TTPB *ttpb;
    struct COM_DEVTAB *cp;
/* 
 * Convert the ASCII number to decimal.
 * Return failure if a decimal # wasn't entered.
 */
    status =  common_convert (ev->value.string, 10, &page, 4);
    if (status) {
	err_printf(status);
	return status;
    }
#if !RHMIN
    cp = com_devtab_ptr;
    ttpb = (*cp).ttpb;
    ttpb->page = page;
    ttpb->spage = page;
#endif
    return msg_success;
}
/*+
 * ============================================================================
 * = ev_graphics_type_wr - action routine for 'graphics_type' ev	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Validate the value specified for the graphics terminal type.
 *
 * FORM OF CALL:
 *
 *	ev_graphics_type_wr ()
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *	msg_ev_badvalue - illegal value specified 
 *
 * ARGUMENTS:
 *
 *      char *name	  - string containing name of the environment variable
 *   	struct EVNODE *ev - address of a pointer to an evnode.
 *
 * SIDE EFFECTS:
 *
 *      Environment variable lists referenced.
 *	The EVLOCK semaphore has been taken out.
-*/
int ev_graphics_type_wr (char *name, struct EVNODE *ev)
{
    graphics_term_type = 0;

    if (ev->value.string[0] == 'V') {
	graphics_term_type = 0;
    }

    if (ev->value.string[0] == 'H') {
	graphics_term_type = 1;
    }

    if (graphics_ttpbs)
	graphics_ttpbs->type = graphics_term_type;

    return( ev_sev( name, ev ) );	/* write to non-volatile RAM */
}
int ev_graphics_type_rd (char *name, struct EVNODE *ev)
{
    graphics_term_type = 0;

    if (ev->value.string[0] == 'V') {
	graphics_term_type = 0;
    }

    if (ev->value.string[0] == 'H') {
	graphics_term_type = 1;
    }

    if (graphics_ttpbs)
	graphics_ttpbs->type = graphics_term_type;

    return msg_success;
}
/*+
 * ============================================================================
 * = ev_graphics_page_wr - action routine for 'page' environment variable     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Setup the value specified for the graphics terminal page size.
 *
 * FORM OF CALL:
 *
 *	ev_graphics_page_wr (evptr)
 *
 * RETURN CODES:
 *
 *	msg_success	- Success.
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	The graphics_page_size variable is set.
 *
-*/
int ev_graphics_page_wr(char *name, struct EVNODE *ev)
{
    int page;
    int status;
/* 
 * Convert the ASCII number to decimal.
 * Return failure if a decimal # wasn't entered.
 */
    status =  common_convert (ev->value.string, 10, &page, 4);
    if (status) {
	err_printf(status);
	return status;
    }
    graphics_page_size = page;

    if (graphics_ttpbs){
	graphics_ttpbs->page = graphics_page_size;
	graphics_ttpbs->spage = graphics_page_size;
    }

    return( ev_sev( name, ev ) );	/* write to non-volatile RAM */
}
int ev_graphics_page_rd(char *name, struct EVNODE *ev)
{
    int page;
    int status;
/* 
 * Convert the ASCII number to decimal.
 * Return failure if a decimal # wasn't entered.
 */
    status =  common_convert (ev->value.string, 10, &page, 4);
    if (status) {
	err_printf(status);
	return status;
    }
    graphics_page_size = page;

    if (graphics_ttpbs){
	graphics_ttpbs->page = graphics_page_size;
	graphics_ttpbs->spage = graphics_page_size;
    }
    return msg_success;
}
#endif

#if MIKASA || SABLE || RAWHIDE
#if !CORELLE
int ev_wr_rcm_dialout( char *name, struct EVNODE *ev )
    {
    return( ev_wr_rcm( 0xd0, 46, ev ) );
    }

int ev_rcm_dialout_init( char *name, char *value )
    {
    return( ev_rcm_init( 0xd0, value ) );
    }

int ev_wr_rcm_init( char *name, struct EVNODE *ev )
    {
    return( ev_wr_rcm( 0x70, 30, ev ) );
    }

int ev_rcm_init_init( char *name, char *value )
    {
    return( ev_rcm_init( 0x70, value ) );
    }

int ev_wr_rcm_answer( char *name, struct EVNODE *ev )
    {
    return( ev_wr_rcm( 0x90, 15, ev ) );
    }

int ev_rcm_answer_init( char *name, char *value )
    {
    return( ev_rcm_init( 0x90, value ) );
    }

int ev_rcm_init( int offset, char *value )
    {
    struct FILE *fp;
    int count;

    fp = fopen( "iic_rcm_nvram0", "rs" );
    if( fp == NULL )
	return( "" );

    fseek( fp, offset, SEEK_SET );
    count = 0;
    fread( &count, 1, 1, fp );
    if( count == 0 )
	{
	fclose( fp );
	return( "" );
 	}

    count = min( count-1, EV_VALUE_LEN );
    memset( value, 0, EV_VALUE_LEN );
    fread( value, 1, count, fp );
    fclose( fp );
    return( value );
    }

int ev_wr_rcm( int offset, int max_len, struct EVNODE *ev )
    {
    struct FILE *fp;
    int count;
    char buf[1] = 0x0d;
    char buf1[48];

    if( ev_initing )
	return( msg_success );

    count = strlen( ev->value.string );
    if( count > max_len )
	{
	err_printf( "string is too long\n" );
	return( msg_failure );
	}
    fp = fopen( "iic_rcm_nvram0", "rs" );
    if( fp == NULL )
	return( msg_failure );

    fseek( fp, offset, SEEK_SET );
    memset( buf1, 0, sizeof( buf1 ) );
    fwrite( buf1, 1, max_len+2, fp );
    if( count == 0 )
	{
	fclose( fp );
	return( msg_success );
	}
    count += 1;
    fseek( fp, offset, SEEK_SET );
    fwrite( &count, 1, 1, fp );
    fwrite( ev->value.string, 1, count-1, fp );
    fwrite( buf, 1, 1, fp );
    fclose( fp );
    return( msg_success );
    }
#endif
#endif

#if SABLE || WILDFIRE || REGATTA || FALCON || PC264
#define EV_HEAP_EXPAND 0x18
#define EV_HEAP_EXPAND_VALID 0x19

int ev_heap_expand ( char *name, struct EVNODE *ev )
    {
    int i;
    for (i=0; ev_heap_table[i].param != 0; i++) {
	if (!strcmp( ev->value.string, ev_heap_table[i].param )) {
#if FALCON
            boot_ev_write( EV_HEAP_EXPAND , i );
            boot_ev_write( EV_HEAP_EXPAND_VALID , -i );
#else
	    rtc_write( EV_HEAP_EXPAND , i );
	    rtc_write( EV_HEAP_EXPAND_VALID , -i );
#endif
	    return ( msg_success );
	    }
	}
	return (msg_failure);	
    }

int ev_heap_expand_init ( struct env_table *et, char *value )
    {
    char heap_work;
#if FALCON
    heap_work = boot_ev_read(EV_HEAP_EXPAND);
    if (((-heap_work)&0xff) == boot_ev_read(EV_HEAP_EXPAND_VALID) )
        return (ev_heap_table[boot_ev_read(EV_HEAP_EXPAND)].param);
#else
    heap_work = rtc_read(EV_HEAP_EXPAND);
    if (((-heap_work)&0xff) == rtc_read(EV_HEAP_EXPAND_VALID) )
	return (ev_heap_table[rtc_read(EV_HEAP_EXPAND)].param);
#endif
    else
	return (ev_heap_table[0].param);
    }
#endif

#if REGATTA || WEBBRICK || WILDFIRE
int ev_com1_mode( char *name, struct EVNODE *ev )
    {
    struct set_param_table *p;

    if( ev_initing )
	return( msg_success );

    for( p = mode_table; p->param; p++ )
	{
	if( strcmp( p->param, ev->value.string ) == 0 )
	    set_com1_mode( p->code );
	}
    return( msg_success );
    }

int ev_com1_mode_init( struct env_table *et, char *value )
    {
    unsigned char data;

    data = get_com1_mode( );
#if WEBBRICK
    if( ( data > 0 ) && ( data < 4 ) )
#else
#if WILDFIRE
    if( ( data >= 0 ) && ( data < 6 ) )
#else
    if( ( data >= 0 ) && ( data < 5 ) )
#endif
#endif
	return( mode_table[data].param );
    else
	return( et->ev_value );
    }

int ev_com1_mode_rd( char *name, struct EVNODE *ev, struct EVNODE *ev_copy )
    {
    unsigned char data;

    data = get_com1_mode( );
#if WEBBRICK
    if( ( data > 0 ) && ( data < 4 ) )
#else
#if WILDFIRE
    if( ( data >= 0 ) && ( data < 6 ) )
#else
    if( ( data >= 0 ) && ( data < 5 ) )
#endif
#endif
	strcpy( ev_copy->value.string, mode_table[data].param );
    else
	strcpy( ev_copy->value.string, mode_table[0].param );

    return msg_success;
    }
#endif

#if REGATTA || WEBBRICK || WILDFIRE
int ev_com1_flow_init( struct env_table *et, char *value )
    {
    unsigned char data;

    data = get_com1_flow( );
    if( ( data >= 0 ) && ( data < 4 ) )
	return( flow_table[data].param );
    else
	return( et->ev_value );
    }
#endif

#if REGATTA
int ev_com1_modem_init( struct env_table *et, char *value )
    {
    unsigned char data;

    data = get_com1_modem( );
    if( ( data >= 0 ) && ( data < 2 ) )
	return( onoff_ascii[onoff_ascii[data].code].param );
    else
	return( et->ev_value );
    }

int ev_sys_com1_rmc( char *name, struct EVNODE *ev )
    {
    int value;

    if( ev->value.string[1] == 'F' )
	value = 0;
    else
    	value = 1;
    set_sys_com1_rmc ( value );
    return( ev_sev( name, ev ) );
    }

int ev_wr_ocp_text( char *name, struct EVNODE *ev )
    {
    unsigned char buf[17];

    if( ev_initing )
	return( msg_success );

    if( strlen( ev->value.string ) != 0 )
	{
	sprintf( buf, "%-16.16s", ev->value.string );
	rmc_cmd( 3, 16, buf, 0 );
	}
    return( ev_sev( name, ev ) );
    }
#endif

int ev_wr_ffauto( char *name, struct EVNODE *ev )
    {
    if( strcmp( ev->value.string, "ON" ) == 0 )
	force_failover_auto = 1;
    else
	force_failover_auto = 0;
    return( ev_sev( name, ev ) );
    }

int ev_wr_ffnext( char *name, struct EVNODE *ev )
    {
    if( strcmp( ev->value.string, "ON" ) == 0 )
	force_failover_next = 1;
    else
	force_failover_next = 0;
    return( msg_success );
    }

int ev_rd_ffnext( char *name, struct EVNODE *ev, struct EVNODE *ev_copy )
    {
    if( force_failover_next == 1 )
       {
       strcpy(ev_copy->value.string, "ON" );
       ev_copy->size = 2;
       }
    else
       {
       strcpy(ev_copy->value.string, "OFF" );
       ev_copy->size = 3;
       }
    return( msg_success );
    }

#if MONET
int ev_mop_password_wr(char *name, struct EVNODE *ev)
{
    if (strlen(ev->value.string) != 20 || ev->value.string[2] != '-' || ev->value.string[11] != '-') {
	printf("%s incorrect format (pp-vvvvvvvv-vvvvvvvv) - not modified\n", ev->value.string);
	return msg_failure;
    }
    return( ev_sev( name, ev ) );	/* write to non-volatile RAM */
}
#endif

#if MNSPLIT

int ev_xdelta(char *name, struct EVNODE *ev)
{
    unsigned char data;
    unsigned char xdelta_on;

/* pick up existing xdelta control */

    xdelta_on = rtc_read(0x3d);

    if (ev->value.string[1] == 'N')
	data = 1;
    else
	data = 0;

    rtc_write( 0x3d, data );

    if (!ev_initing) {
	if (data != xdelta_on) {
	    pprintf( "xdelta environment variable changed\n" );
	    pprintf( "RESET the system for the change to take effect\n" );
	}
    }
    return( msg_success );
}

int ev_xdelta_init(struct env_table *et, char *value)
{
    unsigned char data;

    data = rtc_read( 0x3d );
    if (data == 1)
	return ("ON");
    else
	return ("OFF");
}
#endif

int ev_search_code(struct EVNODE *ev,char *string,int def_value)
{
	struct set_param_table *search;

	qprintf("ev_search_code = %s\n",string);

	for (search = ev->validate.table; search->param; search++)
	{
		qprintf("compare = %s\n",search->param);

		if (strcmp(string,search->param) == 0)
			def_value = search->code;
	}

	qprintf("result = %x\n",def_value);

	return(def_value);
}
