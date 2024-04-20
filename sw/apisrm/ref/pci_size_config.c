/* clist/off */

/* file:	pci_size_config.c
 *
 * Copyright (C) 1994 by
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
*  FACILITY:  SRM console
*
*  MODULE DESCRIPTION:
*
*      This module contains all common functions required for PCI bus
*      sizing and configuration
*
*  AUTHORS:
*
*      Tony Camuso  (PCI bus configuration routines)
*      Jim Hamel    (PCI bus sizing routines)
*
*  CREATION DATE:  March 10, 1994 
*
*  DESIGN ISSUES:
*
*      All common functions specific to the PCI bus sizing and 
*      Configuration should be put here ..
*
*      NOTE: NO #if CONSTRUCTS SHOULD BE PLACED IN THIS MODULE
*            ANY SYSTEM SPECIFIC FUNCTIONALITY SHOULD BE INCLUDED
*            IN THE FILE PCI_SIZE_CONFIG_'platform'.c AND A STUB 
*            ROUTINE NEEDS TO BE CREATED SO YOU DON'T BREAK OTHER 
*            SYSTEM'S BUILDS.
*
*
*  MODIFICATION HISTORY:
*
*
*	    1.42	MAR 	21-apr-1991	Add new device id for pbxdp-aa
*	    1.41	MAR 	08-sep-1998	Add DEGPA-SA gigabit ethernet
*	    1.40	MAR 	01-sep-1998	Update device ID's for PBXDP-AB/AC
*	    1.39	MAR	06-Mar-1998	Add PCI-to-Cardbus adapter.
*           1.38	MAR	29-Jan-1998	Rename PBXNP-AC to SN-PBXNP-AC
*                                               Rename Thomas Conrad Token Ring to PBXNP-AA
*	    1.37	MAR	04-Feb-1998     Add PBXDA-AA/AB, -AC
*           1.36	MAR	29-Jan-1998	Add PBXNP-AC
*	    1.35	MAR	11-Nov-1997	Add Cateyes 4D5IT graphics controller
*	    1.33	MAR     4-Mar-1997	Add DE500-FA 
*	    1.33	MAR     4-Mar-1997	Add DE500-BA which uses the 24113 chip
*	    1.32	DTR	8-Oct-1996	Added 21152 bridge support.
*
*	    1.31	SMD	4-sep-1996	Changed find_io_device calls to
*						take sub_id arg.
*
*	    1.30	DTR	1-Aug-1996	added 64 bit device id support
*						and removed the config table in
*						show config aps. (let there be
*						only one table.)
*
*	    1.29	NOH	05-JUN-1996	Merged table used in show_config_aps
*						with io_device_tbl.
*
*	    1.28	DTR	11-Apr-1996	Added miata conditional to list.
*
*	    1.27	NOH	26-JAN-1996	Added BURNS to find_pb code.
*						slot loops to start with PCI_MIN_SLOT. 
*
*           1.26        DTR     22-JAN-1995     ADDED CONSTANTS FOR PPB SETUP as
*                                               well as adding platform_cpu.h.
*
*	    1.25	hcb	4-jan-1996	Added sniff code for kfpsa.
*						Added pci_assign_letters to
*						assign controller letters
*						after all sniffing(including
*						kfpsa) is complete.
*
*	    1.24	DTR	13-Dec-1995	Correct io space allocation
*
*	    1.23	DTR	08-Oct-1995	Correct eisa bus probing error
*						Needed to restore bridge value.
*
*	    1.22	dpm	28-Sep-1995     Added the Cirrus PD6729 to 
*						the valid pci devices. Fixed
*						a problem with requesting real
*						small io spaces.
*					
*	    1.21	dtr	21-Aug-1995	Flag for alcor/eisa systems to
*						force eisa bus probe as old
*						method.
*						
*	    1.20	dtr	15-aug-1995	Correct problem in old probing
*						code.  Failed to reset the
*						bridge pointers if old and eisa
*						bridge.
*
*	    1.19	dwn	10-aug-1995	Added Thomas Conrad TR support
*
*	    1.19	sm	10-aug-1995	Added 21052 support
*
*	    1.18	dtr	 9-aug-1995	Removed special vga code.
*
*	    1.17	dtr	 8-aug-1995	Special code to handle vga
*						devices with roms.  Force VGA
*						ROMs to c0000.
*
*	    1.16	sm	 8-aug-1995	Make Noname use find_pb etc
*
*	     0.6	noh	13-Feb-1995	Made changes to compile using relaxed ANSI
*						standard.
*
*	    1.15	dtr	18-jul-1995	secondary bus address now
*						limited to 15 and not 21 per
*						pci spec.
*
*	    1.14	dtr	14-jul-1995	Enable latency timer and 
*						disabled request time mask 
*						(whatever that does)
*
*	    1.13	dtr	10-Jul-1995	Added 21041 to list and os type
*						in init code.
*						
*  	    1.12	dtr	15-jun-1995	major changes for bridge 
*						probing.
*
*	    1.11	rbb	8-jun-1995	Conditionalize for EB164.
*
*	    1.10	dtr	7-jun-1994	prefetchable memory behind
*						bridge support added.  Added a
*						third space definition to it.
*						
*	    1.9		dtr	31-may-1994	added ibm bridge.
*	    
*	    1.8		er	19-may-1995	Make EB boards use find_pb and friends.
*
*	    1.7		jfr	19-may-1995	TGA hack fixed to only allocate
*						128Mb for first bar.
*
*	    1.6		dtr	11-may-1995	Change the order of eisa bus
*						probing.  This cause device
*						names to be wrong
*						
*	    1.5		dtr	15-Apr-1995	Routine added to convert the
*						interrupt pint value to the
*						corresponding slot code.
*						
*	    1.5		dtr	13-apr-1995	added a routine to return the
*						top level bridge pointer.  This
*						is needed by allocate vector for
*						those devices that reside two
*						levels deep behind a bridge.
*						
*	    1.4		dtr	7-apr-1995	Rewrote the configuration code
*						to better handle cascade bridges
*						on the aps platforms.
*						
*	    1.3		dpm	30-mar-1995	Removed the pb_found flag in
*						find_pb so multiple eisa boards
*						of the same type showed up.
*
*	    1.2		dtr	27-mar-1995	Corrected the problem handling
*						eisa/pci devices at the same
*						time.  The check was for not
*						pb_found and required a check
*						for callbacks to be present.
*
*						Corrected the init code for the
*						pbs array to be done only once
*						at entry time and not for each
*						device found.
*						
*   	    1.1		dpm	23-mar-1995	Added DE200-LE (de205) to the 
*						io_device_tbl, it was in the 
*						avanti.c version of the table
*						and should still be supported.
*						No longer increment NUM_PBS in                        
*						the isa section of find_pb as 
*						causes the en device to get 
*						listed twice with show dev and 
*						is done in insert_pb anyway.
*
*
*	    1.0		dtr	15-mar-1995	added generic platforms.  A problem was
*						discovered with pci_nvram module
*						which call find_pb multiple
*						times.  The original code would
*						always allocate a port block for
*						every device found instead of
*						return the first instance of
*						one.  Note that this is also
*						wrong as well but sable was ther
*						first.
*						
*	    0.9		dtr	6-mar-1995	environmnet variable support for
*						depth/breadth probing.
*						Expansion rom support added.
*						NOTE: 1280P is a problem.
*
*	    0.8		dpm	23-Feb-1995	moved the reseting the bridge_dev_ptr
*						to outside the 	supported bridge check.
*
*	    0.7		dtr	15-Feb-1995	Removed allocated_pins array and
*						moved it to platform init code.
*						
*	    0.6		dtr	13-Feb-1995	Correct problem in
*						get_bridge_info.  Found variable
*						was not set up.  Also changed
*						the condition on checking if a
*						bridge was valid.  Code was
*						checking for -1.  Changed to >
*						0.
*
*            0.5        jrh     15-Nov-1994	Added in hooks for depth first controller 
*                                               lettering (conditionally compiled out) and 
*                                               setup pci-pci bridge to generate master aborts
*                                               non-existant bus probes
*	     0.4	dtr	15-Nov-1994	added complete 64bit device
*						sizing support. 
*	     0.3	jmp	06-08-94	pci to pci bridge changes from hamel
*	     0.2	jmp	05-19-94	tga requires 128 Mbytes
*	     0.1	jmp	04-14-94	add start_baddr    
*--
*/

#ifdef __cplusplus
extern "C" {
#endif
      
#include    "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include    "cp$src:kernel_def.h"
#include    "cp$src:dynamic_def.h"                                          
#include    "cp$inc:platform_io.h" 	/* required before probe_io_def.h */	
#include    "cp$src:probe_io_def.h"
#include    "cp$src:isacfg.h"  
#include    "cp$src:pci_size_config.h"
#include    "cp$src:pb_def.h" 
#include    "cp$src:strfunc.h" 		/* strcmp() */ 
#if !(K2 || TAKARA)
#include    "cp$src:dynamic.h" 		/* malloc() */ 
#endif
#include    "cp$src:semaphore.h" 	/* krn$_post()  */
/* Platform specific includes */
#include    "cp$src:platform_cpu.h"
        
#ifndef PRIMARY_BRIDGE
#define PRIMARY_BRIDGE 0x0c
#endif

#ifndef SECONDARY_BRIDGE
#define SECONDARY_BRIDGE 0x08
#endif                                                                                               

#define PCI_DEPTH 1
    
#ifndef	PCI_K_MAX_SLOTS_PRIMARY
#define PCI_K_MAX_SLOTS_PRIMARY PCI_K_MAX_SLOTS
#endif

#ifndef	PCI_K_MAX_SLOTS_SECONDARY
#define PCI_K_MAX_SLOTS_SECONDARY 16
#endif


/* externals */
extern int start_baddr[PCI_K_MAX_BUS][MAX_SPACE];	/* in pci_size_config_platform.c	*/
extern int current_baddr[PCI_K_MAX_BUS][MAX_SPACE];	/* in pci_size_config_platform.c */
extern int bus_count[PCI_K_MAX_BUS];			/* in pci_size_config_platform.c */

/* globals */

enum pci_size_config_errors
    {
    INV_BUS_DEV = 1,	/* invalid bus device */
    INV_DEV_BADDR,	/* invalid device base address register */
    ZERO_PTR		/* pointer is a zero */
    };

/* common place to put the variabls */

void decppb(int hose, int bus, int slot);
void ibmppb(int hose, int bus, int slot);

int mc_controllers=0;
int dr_controllers=0;
int er_controllers=0; 
int en_controllers=0; 
int ew_controllers=0;
int fr_controllers=0;
int dq_controllers=0;
int dw_controllers=0;
int pk_controllers=0;
int pu_controllers=0;
int vg_controllers=0;
int no_controllers=0;
int fw_controllers=0;
int hw_controllers=0;
int tg_controllers=0;
int isa_eisa_count=0;

int isa_count = 0;

int pci_os_type;	    /* needed for vector allocation */
int last_vector_used;

int force_eisa = 0;	    /* 1 = force eisa in breadth method.  If it's   */
			    /* zero then we used the bus_probe_algorithm    */
			    /* flag.  */


int tga2Rombase;
/*	 
**  moved from platform specific code to generic code.
*/	 

struct pb **pbs;
unsigned int num_pbs;
unsigned int num_pbs_all;
unsigned int num_sbs;
unsigned int num_sbs_all;
unsigned int num_ubs;
unsigned int num_ubs_all;

struct SEMAPHORE scs_lock;
struct SEMAPHORE pbs_lock;
struct SEMAPHORE sbs_lock;
struct sb **sbs;
struct SEMAPHORE ubs_lock;
struct ub **ubs;

int pci_video_detected;

int manual_config = 1;
extern	unsigned char ETAPciMaxBusNumber;

int rec_ctr = 0;			/* recursion counter */

struct pci_bus_elem *primary_bus_ptr[PCI_K_MAX_BUS]; /* will contain	    */
						    /* pointer to primary   */
						    /* bus element/hose	    */

struct pci_bus_elem *first_bus_ptr;         

/* pointer to bridge on current primary slot */
struct pci_dev_elem *bridge_dev_ptr =  0;

						    /* list of PCI-PCI bridges supported by this code */
struct supported_bridge bridges[] = {		    
	{0x00011011, 0x00000000, 2, (PSetupBridgeFcn) decppb, (PSetupBridgeFcn) 0  },	    /* DECs PCI-PCI Bridge */
	{0x00221014, 0x00000000, 2, (PSetupBridgeFcn) ibmppb, (PSetupBridgeFcn) 0 },	    /* IBM PCI-PCI Bridge */
	{0x00211011, 0x00000000, 2, (PSetupBridgeFcn) decppb, (PSetupBridgeFcn) 0  },	    /* DECs enhanced PCI-PCI Bridge */
	{0x00241011, 0x00000000, 2, (PSetupBridgeFcn) 0, (PSetupBridgeFcn) 0  },	    /* DECs enhanced PCI-PCI Bridge */
	{0x00261011, 0x00000000, 2, (PSetupBridgeFcn) 0, (PSetupBridgeFcn) 0  },	    /* DECs enhanced PCI-PCI Bridge */
	{0x00000000}                                                                                   
	};

int  bridge_aligns[MAX_SPACE] = {MEM_BAR_MIN, IO_BAR_MIN,MEM_BAR_MIN};
                   
struct pci_dev_elem *parent_bridge[MAX_CASCADE_LEVELS];
                   
/* definitions used to get at dynamic memory allocation */

#define realloc(x,y) dyn$_realloc(x,y,DYN$K_SYNC|DYN$K_NOOWN, 0, 0, (struct ZONE *)0)
#define malloc(x) dyn$_malloc(x,DYN$K_FLOOD|DYN$K_SYNC|DYN$K_NOOWN, 0, 0,(struct ZONE *)0)
#define free(x) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)


/* Local Prototypes */
int rtc_read(int );    		/* in toy_driver.c */
int krn$_sleep(int );	     	/* in timer.c      */
int insert_pb(struct pb* );	/* in kernel_support.c */                          
				/* isacfg.c 	   */
unsigned long isacfg_get_device(char *handle,int instance,struct TABLE_ENTRY *ptr);
void print_size_info(void);    
void pci_config_bus (int hose, struct pci_bus_elem *pci_bus_ptr, int space);

int bus_probe_algorithm = 0;

#define DEC_TGA			0x00041011
#define DEC_TGA2		0x000d1011
                                                                                                          
#define PCI_DEBUG 0

struct io_device io_device_tbl[] = {


  { TYPE_EISA, 0x02604f42,0x00000000, "dw", 0, "dw", &dw_controllers },      /* PRO6002 */
  { TYPE_EISA, 0x01604f42,0x00000000, "dw", 0, "dw", &dw_controllers },      /* PRO6001 */
  { TYPE_EISA, 0x00604f42,0x00000000, "dw", 0, "dw", &dw_controllers },      /* PRO6000 */
  { TYPE_EISA, 0x0030a310,0x00000000, "fr", 0, "fr", &fr_controllers },      /* DEC3000 */
  { TYPE_EISA, 0x70009835,0x00000000, "dac960", 0, "dr", &dr_controllers },  /* MLX0070 */
  { TYPE_EISA, 0x2042a310,0x00000000, "er", 0, "er", &er_controllers },      /* DEC4220 */
  { TYPE_EISA, 0x5042a310,0x00000000, "ew", 0, "ew", &ew_controllers },      /* DEC4250 */	
  { TYPE_EISA, 0x00009004,0x00000000, "pkh", 0, "pk", &pk_controllers },     /* ADP0000 */
  { TYPE_EISA, 0x002ea310,0x00000000, "pua", 0, "pu", &pu_controllers },     /* DEC2E00 */
  { TYPE_EISA, 0x1030110E,0x00000000, "vga", 0, "vg", &vg_controllers },     /* CPQ3010 */
  { TYPE_EISA, 0x0025a310,0x00000000, "eisa_nvram", 0, 0, &no_controllers }, /* DEC2500 */

    { TYPE_PCI, 0x050810da,0x00000000, "dw", "PBXNP-AA Token Ring Controller", "dw", &dw_controllers },
    { TYPE_PCI, 0x815410ef,0x00000000, "dw", "SN-PBXNP-AC Token Ring Controller", "dw", &dw_controllers },
    { TYPE_PCI, 0x000f1011,0x00000000, "fw", "Digital PCI FDDI", "fw", &fw_controllers },
    { TYPE_PCI, 0x06601095,0x00000000, "cipca", "CIPCA", "pu", &pu_controllers },
    { TYPE_PCI, 0x00181011,0x00000000, "mc", "Digital Memory Channel", "mc", &mc_controllers },
    { TYPE_PCI, 0x06461095,0x00000000, "dq", "PCI IDE", "dq", &dq_controllers },
    { TYPE_PCI, 0x00011000,0x00000000, "n810", "NCR     810 Scsi Controller", "pk", &pk_controllers },
    { TYPE_PCI, 0x00051000,0x00000000, "n810", "NCR     810A Scsi Controller", "pk", &pk_controllers },
    { TYPE_PCI, 0x00031000,0x00000000, "n810", "NCR     825 Scsi Controller", "pk", &pk_controllers },
    { TYPE_PCI, 0x80031000,0x00000000, "n825_dssi", "Digital KFPSA", "pu", &pu_controllers },
    { TYPE_PCI, 0x000f1000,0x00000000, "n810", "NCR     875 Scsi Controller", "pk", &pk_controllers },
    { TYPE_PCI, 0x000c1000,0x00000000, "n810", "NCR     895 Scsi Controller", "pk", &pk_controllers },
    { TYPE_PCI, 0x00121000,0x00000000, "n810", "NCR     895A Scsi Controller", "pk", &pk_controllers },
    { TYPE_PCI, 0x000b1000,0x00000000, "n810", "NCR     896 Scsi Controller", "pk", &pk_controllers },
    { TYPE_PCI, 0x04828086,0x00000000, "pci_eisa", "Intel   8275EB PCI to Eisa Bridge", 0, &isa_eisa_count },
    { TYPE_PCI, 0x04848086,0x00000000, "pci_isa", "Intel SIO 82378", 0, &isa_eisa_count },
    { TYPE_PCI, 0x00011011,0x00000000, 0, "Digital PCI to PCI Bridge Chip", 0, 0 },
    { TYPE_PCI, 0x00211011,0x00000000, 0, "DECchip 21052-AA", 0, 0 },
    { TYPE_PCI, 0x00241011,0x00000000, 0, "DECchip 21152-AA", 0, 0 },
    { TYPE_PCI, 0x00261011,0x00000000, 0, "DECchip 21154 PCI to PCI Bridge", 0, 0 },
    { TYPE_PCI, 0x00221014,0x00000000, 0, "IBM PCI to PCI Bridge Chip", 0, 0 },
    { TYPE_PCI, 0x00021011,0x00000000, "ew", "DECchip 21040 Network Controller", "ew", &ew_controllers },
    { TYPE_PCI, 0x00141011,0x00000000, "ew", "DECchip 21041 Network Controller", "ew", &ew_controllers },
    { TYPE_PCI, 0x00091011,0x00000000, "ew", "DECchip 21140 Network Controller", "ew", &ew_controllers },
    { TYPE_PCI, 0x00091011,0x500a1011, "ew", "DE500-AA Network Controller", "ew", &ew_controllers },
    { TYPE_PCI, 0x00191011,0x500b1011, "ew", "DE500-BA Network Controller", "ew", &ew_controllers },
    { TYPE_PCI, 0x00191011,0x500f1011, "ew", "DE500-FA Network Controller", "ew", &ew_controllers },
    { TYPE_PCI, 0x00191011,0x00000000, "ew", "Digital Semiconductor 21142 Network Controller", "ew", &ew_controllers },
    { TYPE_PCI, 0x00041011,0x00000000, "tga", "Digital ZLXp Graphics Controller", "tg", &tg_controllers },
    { TYPE_PCI, 0x00101011,0x00000000, "vip7407", "DECchip 7407", 0, 0 },
    { TYPE_PCI, 0x10201077,0x00000000, "isp1020", "ISP1020 Scsi Controller", "pk", &pk_controllers },
    { TYPE_PCI, 0x10201077,0x00001077, "isp1020", "ISP1040B Scsi Controller", "pk", &pk_controllers },
    { TYPE_PCI, 0x00081011,0x00000000, "pks", "Digital KZPSA", "pk", &pk_controllers },
    { TYPE_PCI, 0x00000001,0x00000000, "DE200-LE", "DEC LEMAC", "en", &en_controllers},
    { TYPE_PCI, 0x11001013,0x00000000, "pci_pcmcia", "Cirrus PD6729", 0, 0},
    { TYPE_PCI, 0x00071011,0x00000000, "pci_nvram","Digital PCI NVRAM", 0, &no_controllers },
    { TYPE_PCI, 0x00011069,0x00000000, "dac960", "DAC960  Scsi Raid Controller", "dr", &dr_controllers },
    
    { TYPE_PCI, 0x00eb1091,0x00000000, 0, "Cateyes - 4D51T Graphics Controller",0, 0},
    { TYPE_PCI, 0x000D1011,0x00000000, 0, "Digital TGA2 Graphics Controller", 0, 0},
    { TYPE_PCI, 0x88115333,0x00000000, 0, "Digital S3 TRIO 64 Graphics Controller", 0, 0},
    { TYPE_PCI, 0x88C05333,0x00000000, 0, "Digital S3 TRIO 864 Graphics Controller", 0, 0},
    { TYPE_PCI, 0x00171011,0x00000000, 0, "Digital PV-PCI Graphics Controller", 0, 0},
    { TYPE_PCI, 0x43581002,0x00000000, 0, "ATI MACH 64 Graphics Controller", 0, 0},
    { TYPE_PCI, 0x0519102b,0x00000000, 0, "Matrox Millenium", 0, 0},
    { TYPE_PCI, 0x0518102b,0x00000000, 0, "Matrox Impression", 0, 0},
    { TYPE_PCI, 0x3d07104c,0x0a321048, 0, "ELSA GLoria Synergy", 0, 0},
    { TYPE_PCI, 0x30320e11,0x00000000, 0, "Compaq 1280/P", 0, 0},
    { TYPE_PCI, 0x000e1011,0x00000000, 0, "Digital FullVideo Supreme JPEG", 0, 0},                    
    { TYPE_PCI, 0x00131011,0x00000000, 0, "Digital FullVideo Supreme", 0, 0},
    { TYPE_PCI, 0x5001112F,0x00000000, 0, "Arnet Sync 570 IP", 0, 0},
    { TYPE_PCI, 0x10110016,0x00000000, 0, "Digital ATMworks 350", 0, 0},
    { TYPE_PCI, 0x00161011, 0x00000000, "hw", "Digital ATMworks 350", "hw", &hw_controllers },
    { TYPE_PCI, 0x0020102f, 0x00581011, "hw", "Digital ATMworks 351", "hw",&hw_controllers },
    { TYPE_PCI, 0x0020102f, 0x00781011, "hw", "Digital ATMworks 351", "hw", &hw_controllers },
    { TYPE_PCI, 0x04001127, 0x04001127, "hw", "Fore ATM 155/622 Adapter", "hw", &hw_controllers },
    { TYPE_PCI, 0x04001127, 0x01000E11, "hw", "DAPBA-FA ATM155 MMF", "hw", &hw_controllers },
    { TYPE_PCI, 0x04001127, 0x02000E11, "hw", "DAPBA-UA ATM155 UTP", "hw", &hw_controllers },
    { TYPE_PCI, 0x04001127, 0x03000E11, "hw", "DAPCA-FA ATM622 MMF", "hw", &hw_controllers },
    { TYPE_PCI, 0x0020102f, 0x00000000, "hw", "Toshiba Meteor", "hw", &hw_controllers },
    { TYPE_PCI, 0x000112ae, 0x600a1011, 0, "DEGPA-SA", 0, 0 },
    { TYPE_PCI, 0x0009114f, 0x00000000, 0, "PBXDA-AA/AB", 0, 0 },
    { TYPE_PCI, 0x0004114f, 0x00000000, 0, "PBXDA-AC", 0, 0 },
    { TYPE_PCI, 0x906d10b5, 0x113310df, 0, "PBXDP-AA", 0, 0 },
    { TYPE_PCI, 0x908010b5, 0x113310df, 0, "PBXDP-AA", 0, 0 },
    { TYPE_PCI, 0x906d10b5, 0x115510df, 0, "PBXDP-AB", 0, 0 },
    { TYPE_PCI, 0x908010b5, 0x115510df, 0, "PBXDP-AB", 0, 0 },
    { TYPE_PCI, 0x906d10b5, 0x115610df, 0, "PBXDP-AC", 0, 0 },
    { TYPE_PCI, 0x908010b5, 0x115610df, 0, "PBXDP-AC", 0, 0 },
    { TYPE_PCI, 0x113110df, 0x00000000, 0, "PBXDP-AD", 0, 0 },
    { TYPE_PCI, 0x115110df, 0x00000000, 0, "PBXDP-AE", 0, 0 },
    { TYPE_PCI, 0x115210df, 0x00000000, 0, "PBXDP-AF", 0, 0 },
    { TYPE_PCI, 0xac12104c, 0x00000000, 0, "PCI to CardBus", 0, 0 },
    { TYPE_PCI, 0xac13104c, 0x00000000, 0, "PCI to PC Card", 0, 0 },  
		
  { 0, 0, 0, 0, 0}};


/* the only platforms tested so far are the ones on the next line.  If you  */
/* want to use this code and support the nvram module then add you platform */
/* here and disable the same routines in the platform code. */

#if K2 || AVANTI || NORITAKE || MIKASA || MIATA || TAKARA || ALCOR || APC_PLATFORM

/* start of the port block io routines.  These are a cut and paste of the   */
/* eisa driver routines.  Only the parameters have changed to reflect that  */
/* a port block is now expected.					    */


struct io_device *find_io_device( int type, unsigned int id, unsigned int id_ext )
    {
    struct io_device *iod;

    for( iod = io_device_tbl; iod->type && iod->id; iod++ )
	if( ( iod->type == type ) && ( iod->id == id ) && ( iod->id_ext == id_ext ) )
	    return( iod );
    return( 0 );
    }


struct io_device *find_io_device_dev(char *device, 
    int type, 
    struct io_device *start)
{
    struct io_device *iod;

    if (start)
    {
	iod = ++start;
    }
    else
    {
	iod = io_device_tbl;
    }

    for (; iod->type && iod->id; iod++)
    {
	if (strcmp((uint8*)iod->device, (uint8*)device) == 0 && iod->type == type )
	{
	    return (iod);
	}
    }
    return (0);                                                               
}


int device_type ( char *device , int type )
	{
	struct io_device *iod;

	for( iod = io_device_tbl; iod->type && iod->id; iod++ )
		{
		if ( (strcmp((uint8*)iod->device, (uint8*)device) == 0) && iod->type == type )
			return( 1);
		}
	return( 0 );
	}
                                                                                        

/*+
 * ============================================================================
 * = find_pb - locate a port block for the specified device.	    =
 * ============================================================================
 *
 * OVERVIEW:
 *                                                                                                       
 *	Locate a port block for the device specified.  If one isn't available
 *	then create one if it is known.  This version only handles pci devices
 *	as the mustang and avanti have a single pci bus and not eisa buss.  Note
 *	that this routine is based on the imp
 *
 * FORM OF CALL:
 *
 *	find_pb(device, size, callback)
 *                    
 * RETURNS:
 *
 *	pointer to port block if found else it returns a 0.
 *
 * ARGUMENTS:
 *
 *	device - pointer to device name
 *	size - size of the port block to locate/create
 *	callback - address of device specific init routine
 *
 *
 * SIDE EFFECTS:
 *
 *      If no port block is found it will create one.
 *
-*/
                                                      
#ifdef __cplusplus
  typedef int (*PCallbackFcn)(...);   
#else                             
  typedef int (*PCallbackFcn)();   
#endif
void *find_pb(char *device, int size, PCallbackFcn callback)
{
    int i = 0;
    int config_get_device();    
    struct TABLE_ENTRY temp_entry;   /*a place to store isacfg stuff*/                            
    struct pb *pb, *pb_new;
    struct io_device *iod,*new_iod;
    unsigned long sid;			/*Sniffed id*/
    unsigned int slot;			/*Slot*/
    int pb_size,number_found;
    char name[8];
    struct pci_devs_found *dev_fnd_ptr;
    struct pci_devs_found *prev_dev_fnd_ptr;
    int	*controllers;
    char *iod_device;

    iod = (struct io_device *)0; /* init the loop */

    if (callback)	/* not eisa */
    {
	if (size > sizeof(*pb))
	    pb_size = size;
	else
	    pb_size = sizeof(*pb);

	pb=(void *)NULL;

	iod = find_io_device_dev(device,TYPE_PCI, iod);

	while (iod != 0)
	{
	    dev_fnd_ptr = pci_find(iod->id, iod->id_ext);

	    iod_device = iod->device;

	    if (dev_fnd_ptr)
	    {

		number_found = dev_fnd_ptr->nbr_found;
		for (i = 0; i < number_found; i++)
		{

		    /* needed for breadth probing */
		    controllers = iod->controller;

		    slot = dev_fnd_ptr->dev_found[i].slot;

		    pb = (struct pb*) malloc( pb_size );

		    pb->vector = dev_fnd_ptr->dev_found[i].vector;  
		    /*Get a port name*/

		    pb->protocol = (unsigned char *)iod->protocol;
		    if (bus_probe_algorithm)
		    {
			pb->controller = dev_fnd_ptr->dev_found[i].controller;
		    }
		    else
		    {
			pb->controller = (*controllers)++;
		    }
		    if (iod_device)
		    {
			pb->device = (unsigned char *)iod_device;
		    }
		    pb->type = iod->type;
		    pb->hose = 0;
		    pb->slot = slot;
		    pb->bus =  dev_fnd_ptr->dev_found[i].bus;
		    pb->function = 0;
		    pb->channel = 0;
		    pb->csr = incfgl (pb, BASE_ADDR0 );
		    /* for this implementation we can only work with memory */
		    if (pb->csr & 1)
		    {
			pb->csr = incfgl (pb, BASE_ADDR1 );
		    }

		    /* isolate the memory/io data */
		    pb->csr &= 0xfffffffc;

		    krn$_sleep( 500 );

		    insert_pb ( pb );

		    if (callback)
		    {
			callback(pb);
		    }
		    else
		    {
			krn$_post(&pbs_lock, 1);
		    }
		    pb=(void *)NULL;
		}
	    }
	    else 
	    {
		/* it wasen't a PCI device check the isa_cfg */ 
		isa_count=1;
		while  (isacfg_get_device(device,isa_count,&temp_entry)!=0)
		{
		    if (temp_entry.device_enable == 1)
		    {
			pb = (struct pb*) malloc( size );

			pb->type = TYPE_ISA;
			insert_pb ( pb );
			if (callback)
			{
			    callback(pb);
			}	
		    } 
		    isa_count++;
		}
	    }
	iod = find_io_device_dev(device,TYPE_PCI, iod);
	}
    }

    /*	 
    **  we search the pbs structure if we are doing eisa or no call back is
    **	specified.
    */	 


    if  ( (device_type ( device , TYPE_EISA ) || (!callback)))
    {

	krn$_wait( &pbs_lock );

	for( i = 0; i < num_pbs; i++ )
	{
	    pb = pbs[i];

	    if( strcmp( (uint8 *)pb->device, (uint8*)device ) == 0 && pb->type == TYPE_EISA)
	    {

		if( size )
		{
		    /* Assign the Controller Number in T\the Order that	the */
		    /* Drivers start in .bld file consistent with pci	    */
		    /* drivers PCI disk drives come first (DKA One of those */
		    /* hose concepts.  It should be noted that this only    */
		    /* applies if the probing code is th eold style.  If it */
		    /* is the new style then the the controllers are named  */
		    /* as they are seen in the probing order.		    */

		    iod = find_io_device_dev(device,
			TYPE_EISA, 
			(struct io_device *)0);


		    if( iod && !bus_probe_algorithm)
		    {
			controllers = iod->controller;
			pb->controller = (*controllers)++;
		    }


		    pb = (struct pb *)realloc(pb, size);
		    pbs[i] = pb;
		}


		if (callback)
		{
		    callback(pb);
		}
		else
		{
		    krn$_post(&pbs_lock, 1);
		    return ((void *)pb);
		}
	    }
	}
	krn$_post( &pbs_lock, 1);
    }

    return ((void *)pb);
}
#endif


/*+
 * ============================================================================
 * = get_bridge_info -  determines whether or not a device is a supported bridge =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *     This routine will determine whether or not a known PCI-PCI bridge chip is 
 *     found
 *
 * FORM OF CALL:
 *
 *	get_bridge_info(vendor_dev_id, bus_nbr)
 *                    
 * RETURNS:
 *
 *	pointer to bridges supported_bridge structure, if this is a configurable bridge
 *     -1   , if this is a unconfigurable bridge
 *      0   , if this is not a bridge
 *
 * ARGUMENTS:
 *
 *      vendor_dev_id - Vendor and Device ID for this device
 *	bus_nbr - bus number that bridge was found on
-*/
struct supported_bridge *get_bridge_info(int vendor_dev_id,
		     int bus_nbr)
{
int bridge_found = 0;
int index = 0;
struct supported_bridge *bridge_ptr;

	bridge_ptr = &bridges[0];

	/*
	    Search through the bridges table to find legal bridge
	*/
	do
	{
	    /* If bridge chip found */
	    if (bridge_ptr->vid_did == vendor_dev_id)
	    {
		/* flag that bridge is found */
		bridge_found = 1;

		/* if we are on secondary and bridge does not support this  */
		/* cascade level					    */

		/*	 
		**  the next line should be cascade levels.  It will be changed
		**  as soon as I get a ppb board
		*/	 		
		if ((bus_nbr) && (bridge_ptr->cascade_level < 2))
		{
		    /* return non-configurable bridge chip */
		    bridge_ptr = (struct supported_bridge *) -1;
		}
	    }
	    else
	    {
		bridge_ptr++;
	    }
	}
	while ((bridge_found == 0) && (bridge_ptr->vid_did));

	/* if bridge not found then ... return as 0 */
	if (!bridge_found)
	{
	    bridge_ptr = (struct supported_bridge *) 0;
	}

	return(bridge_ptr);
}

/*+
 * ============================================================================
 * = ppb_setcfg -  Setup a PCI-PCI Bridge so we can config secondary bus      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will set up a secondary bus number in a DEC PPB bridge so we
 *  can perform configuration cycles on the other side of the bridge
 *
 * FORM OF CALL:
 *
 *	ppb_setcfg(vendor_dev_id, hose, this_bus_nbr, new_bus_nbr, slot)
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *      vendor_dev_id	- Vendor Device ID for this bridge 
 *      hose		- primary bus number for multi-bus systems
 *	this_bus_nbr	- bus number that bridge was found on
 *      new_bus_nbr	- bus number for secondary bus
 *      slot		- slot number where the bridge was found
-*/

 void ppb_setcfg(int vendor_dev_id,
		   int hose,
		   int this_bus_nbr, 
		   int new_bus_nbr, 
		   int slot,
		   struct   pci_dev_elem *parent_ptr)
{
int data;
struct supported_bridge *bridge_ptr; 
struct pb dummy_pb;
struct pci_dev_elem *decendents = ( struct pci_dev_elem * )0;

    /* setup config data */
    data = this_bus_nbr |
	   (new_bus_nbr << 8) | 0xff000000;

    /* write it out */		    
    sys_pci_config_wr(hose, this_bus_nbr, slot, BUS_NBR_OFS, data);

    /* disable memory */
    sys_pci_config_wr(hose, this_bus_nbr, slot, MEM_BAR_OFS, 0x0000fff0);

    /* disable io */
    sys_pci_config_wr(hose, this_bus_nbr, slot, IO_BAR_OFS, 0x00f0);

    /* and finally prefetchable memory */
    sys_pci_config_wr(hose, this_bus_nbr, slot, PREMEM_BAR_OFS, 0xfff0);

    /* turn on master abort and set serr forward enable */
#if MIATA
    /* initial board has a problem with this */
    sys_pci_config_wr(hose, this_bus_nbr, slot, BRG_CNTL_OFS, 0x0000);
#else
/* turn off for pyxis */
    sys_pci_config_wr(hose, this_bus_nbr, slot, BRG_CNTL_OFS, 0x220000);
#endif
    /* set p_serr_l enable, master enable, memory transaction enable */
    sys_pci_config_wr(hose, this_bus_nbr, slot, 0x04, 0x106);

    if (parent_ptr->parent)
    {
        decendents = parent_ptr->parent;
        sys_pci_config_wr(hose, this_bus_nbr, slot, 0x40, SECONDARY_BRIDGE);
    }
    else
    {
        sys_pci_config_wr(hose, this_bus_nbr, slot, 0x40, PRIMARY_BRIDGE);
    }

    /*	 
    **  0 says we are on the primary
    */	 
    while (decendents)
    {
	dummy_pb.hose = hose;
	dummy_pb.slot = decendents->slot;
	dummy_pb.bus  = decendents->bus;
	dummy_pb.function  = 0;

	outcfgb(&dummy_pb, BUS_NBR_OFS+2, new_bus_nbr);
	decendents = decendents->parent;
    }         
    
    
    /* perform any device specific initialization here ...*/
    bridge_ptr = get_bridge_info(vendor_dev_id,this_bus_nbr);    
    
    /* if device specific config routine exists then  (also check valid return status from get_bridge_info)*/
    if ((bridge_ptr->setup_bridge_cfg) && (bridge_ptr != 0) && (bridge_ptr != -1))
    {
	(*bridge_ptr->setup_bridge_cfg)(hose, this_bus_nbr, slot);
    }

}

/*+
 * ============================================================================
 * = ppb_setmio -  Setup a PCI-PCI bridge so it accepts memory/io accesses =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will setup a PCI-PCI bridge so we can
 *  perform memory and I/O  cycles on the other side of the bridge
 *
 * FORM OF CALL:
 *
 *	ppb_setmio(vendor_dev_id, hose, this_bus_nbr, slot, new_bus_ptr,baddr_ptr)
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *      vendor_dev_id	- primary bus number for multi-bus systems
 *      hose		- primary bus number for multi-bus systems
 *	this_bus_nbr	- bus number that bridge was found on
 *      slot		- slot number where the bridge was found
 *      new_bus_ptr	- pointer to the bus element for the secondary bus 
 *			behind the PCI-PCI Bridge chip
 *	baddr_ptr	- pointer to the base address element for this device  
 *
 * GLOBALS:
 *
 *      current_baddr - this is used to determine the current base address of the 
 *                      regions that we need to program. They will be updated after 
 *                      we program the appropriate space.
-*/
 void ppb_setmio(int vendor_dev_id,
		   int hose,
		   int this_bus_nbr, 
		   int slot,
		   struct pci_bus_elem *new_bus_ptr,
		   struct pci_baddr_elem *baddr_ptr)
{
    int data;
    int bridge_ena_data;
    struct supported_bridge *bridge_ptr;
    int space_req;
    int space;
    int base_addr;
    int offset;

    /* get the space for the base address */
    space = baddr_ptr->space;

    /* get  memory requirements  */
    space_req = baddr_ptr->space_req;

    if (space_req)
    {
	/* get current base address */
	base_addr = baddr_ptr->pci_addr;

	if (space == MEM_SPACE)
	{
	    /* put base address into data field */
	    data = (base_addr >> 16) & 0xffff;

	    /* 
	     Put in the limit ... where:
	     
             limit = base_addr + (space_req - 1Mbyte)
	    */
	    data |= ((base_addr+(space_req-0x100000)) & 0Xffff0000);

	    offset = MEM_BAR_OFS;
	    bridge_ena_data = (ENABLE_MEM | ENABLE_MST);
	}

	else if (space == IO_SPACE)
	{
	    /* put base address into data field */
	    data = base_addr >> 8;
	    data |= ((base_addr+space_req) >> 16);
	    offset = IO_BAR_OFS;
	    bridge_ena_data = (ENABLE_IO | ENABLE_MST);

	    /*
		IF base address > 64Kbyte THEN
		    Can not use it ... Bridge can only support 0-64kbyte range
	    */
	    if  (base_addr >= 0x10000)
	    {
		/* force bridge to not forward I/O cycles */
		data = 0xffff00ff;
		bridge_ena_data = 0;
	    } 
	}
	else if (space == PREFETCHABLE_SPACE)
	{
	    /* put base address into data field */
	    data = (base_addr >> 16) & 0xffff;

	    data |= ((base_addr+(space_req-0x100000)) & 0Xffff0000);
	    offset = PREMEM_BAR_OFS;
	    bridge_ena_data = (ENABLE_MEM | ENABLE_MST);
	}

	/* final check is for vga */
	if (new_bus_ptr->flags)
	{
	    int control_data = 
		sys_pci_config_rd(hose,this_bus_nbr,slot,BRG_CNTL_OFS,0);;
	    bridge_ena_data |= 0x1; /* enable vga snooping and standard io */

	    control_data |= 0x80000;
	    sys_pci_config_wr(hose, this_bus_nbr, slot , BRG_CNTL_OFS, control_data);
	}

	/* write it out */
#if PCI_DEBUG
	pprintf("ppb_setmio - Write hose %x bus %x slot %x offset %x data %x\n",
	hose, this_bus_nbr, slot , offset, data);
#endif
	sys_pci_config_wr(hose, this_bus_nbr, slot , offset, data);

    }

    /* Setup cache line size */
    data = CACHE_LWORD_SIZE | 0xff00;
    sys_pci_config_wr(hose,this_bus_nbr,slot,CACHE_L_SIZ, data);
    
    /* Enable the bridge to respond to space if needed  */
    data = sys_pci_config_rd(hose,this_bus_nbr,slot,COM_STAT,0);
    data |= bridge_ena_data;
    sys_pci_config_wr(hose,this_bus_nbr,slot,COM_STAT, data);

#if 0
    /* Disable Prefetchable memory space for now */
    data = 0x0000ffff;
    sys_pci_config_wr(hose,this_bus_nbr,slot,PREMEM_BAR_OFS,data);
#endif

    /* perform any device specific initialization here ...*/
    bridge_ptr = get_bridge_info(vendor_dev_id,this_bus_nbr);    
    
    /* if device specific config routine exists then (also check valid return status from get_bridge_info)*/
    if ((bridge_ptr->setup_bridge_mio) && (bridge_ptr != 0) && (bridge_ptr != -1))
    {
	(*bridge_ptr->setup_bridge_mio)();
    }
}


/*+
 * ============================================================================
 * = insert_baddr_elem -  insert a base address element into a sorted list for bus =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *     This routine will insert a base address element into a sorted list 
 *     for a bus. This list is used by the pci configuration code to 
 *     allow for easy determination of a best fit
 *
 * FORM OF CALL:
 *
 *	insert_baddr_elem(bus_ptr, baddr_ptr, space)
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *      bus_ptr	    - pointer to pci bus element for this device
 *	baddr_ptr   - pointer to a base address element for this device
 *      space	    - indicator of which list to put this into
 *                    0 - means Memory space
 *                    1 - means I/O space
-*/
 void insert_baddr_elem(struct pci_bus_elem *bus_ptr, 
		  struct pci_baddr_elem *baddr_ptr, 
		  int space)
{
struct pci_baddr_elem *temp_baddr_ptr;
struct pci_baddr_elem *first_baddr_ptr;
int elem_inserted;

    /* if space > max_space then exit */
    if (space > MAX_SPACE)
	return;
    
    /*
	Get the pointer to the beginning of the sorted list 
	for this space
    */
    temp_baddr_ptr = bus_ptr->sorted_bar[space];
    first_baddr_ptr = temp_baddr_ptr;
    
    /*
	If list is empty then 
	    this entry gets put at the start of the queues
    */
    if (!temp_baddr_ptr)
    {
	bus_ptr->sorted_bar[space] = baddr_ptr;
	baddr_ptr->prev_bar_ptr = 
	    (struct pci_baddr_elem *) 0;
	baddr_ptr->next_bar_ptr = 
	    (struct pci_baddr_elem *) 0;
	bus_ptr->alignment[space] = baddr_ptr->alignment;

    }
    else
    {	 
	elem_inserted = 0;
	/* 
	   find location to insert entry 
	*/
	do
	{
	    /*
	     IF memory requirments >= current memory requirements THEN
		insert this element in the list
	    */
	    if (baddr_ptr->space_req >= temp_baddr_ptr->space_req)
	    {
		/* link new element to the list */
		baddr_ptr->next_bar_ptr = temp_baddr_ptr;
		baddr_ptr->prev_bar_ptr = temp_baddr_ptr->prev_bar_ptr;

		/* link temp element's previous to new element */
		if (temp_baddr_ptr->prev_bar_ptr)
		{
		    temp_baddr_ptr->prev_bar_ptr->next_bar_ptr = baddr_ptr;
		}

		/* link temp element back to element just inserted */
		temp_baddr_ptr->prev_bar_ptr = baddr_ptr;

	    bus_ptr->alignment[space] = baddr_ptr->alignment;
		elem_inserted = 1;
		/* 
		  if we inserted at the head of the list then
		    we must update pointer at the bus element level
		*/  
		if (temp_baddr_ptr == first_baddr_ptr)
		{
		    bus_ptr->sorted_bar[space] = baddr_ptr;
		}

	    } 
	    else
	    {
		/*
		    IF no next entry found THEN
			insert new entry at the end of the queue
		*/
		if (temp_baddr_ptr->next_bar_ptr == (struct pci_baddr_elem *)0 ) 
		{
		    temp_baddr_ptr->next_bar_ptr = baddr_ptr;
		    baddr_ptr->prev_bar_ptr = temp_baddr_ptr;
		    baddr_ptr->next_bar_ptr = 
			    (struct pci_baddr_elem *) 0;

		    elem_inserted = 1;
		}
		/*
		   ELSE
		       point to the next entry
		*/
		else
		{
		    temp_baddr_ptr = temp_baddr_ptr->next_bar_ptr;
		}
	    }
	}
	while (!elem_inserted); 
    }
}


/*+
 * ============================================================================
 * = pci_init_bar_elem - Create and initialize a base address element
 * ============================================================================
 *
 * OVERVIEW:
 *
 *     this routine will ...
 *     - create a base address element
 *     - initialize the data only portion of the structure
 *     - link it in with the bars for this device
 *     - link it in to the sorted list for this bus
 *      
 *
 * FORM OF CALL:
 *
 *	pci_init_bar_elem(bus_ptr, dev_ptr, space_req, space, bar_nbr)
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	bus_ptr - pointer to pci bus element structure for this slot
 *      dev_ptr - pointer to pci device element structure for this slot
 *      space_req - size of space required in bytes
 *      space   - the type of space required 
 *      bar_nbr	- base address number
 *
-*/
 void pci_init_bar_elem(struct pci_bus_elem *bus_ptr, 
		      struct pci_dev_elem *dev_ptr, 
		      int space_req,
		      int alignment, 
		      int space,
		      int bar_nbr)
{
struct pci_baddr_elem *baddr_ptr;
struct pci_baddr_elem *dev_baddr_ptr;

    /* if space is out of range then just return */
    if (space > MAX_SPACE)
	return;

    /*
	allocate a base address element
    */
    baddr_ptr = (struct pci_baddr_elem *)malloc(sizeof(struct  pci_baddr_elem));
    
    /*
	fill common fields 
    */
    baddr_ptr->space_req = space_req;
	baddr_ptr->alignment = alignment;
    baddr_ptr->bar_nbr = bar_nbr;
    baddr_ptr->dev_bar_ptr = (struct pci_baddr_elem *)0;
    baddr_ptr->dev_back_ptr = dev_ptr; 			
    baddr_ptr->space = space;

    /* 
     IF no base address elements off of device element then 
        make this the first 
     ELSE 
       Insert base address element at the end of baddr list
       for this device
    */
    if  (!(dev_baddr_ptr = dev_ptr->baddr_ptr))
    {
	dev_ptr->baddr_ptr = baddr_ptr;
    }
    else
    {
	while (dev_baddr_ptr->dev_bar_ptr)
	{
	    dev_baddr_ptr = dev_baddr_ptr->dev_bar_ptr;
	}
	dev_baddr_ptr->dev_bar_ptr = baddr_ptr;
    }

    /* update space requirements at the bus level */

    bus_ptr->space_req[space] += space_req;

    /* insert this into sorted list for this bus */
    insert_baddr_elem(bus_ptr, baddr_ptr, space); 
}

/*+
 * ============================================================================
 * = pci_size_baddr_regs - find all usable base addresses used by a device
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Find all base addresses used by a device and ...
 *      - create a base address element 
 *      - insert it into a sorted list (1 for I/O, 1 for Memory)
 *      
 *
 * FORM OF CALL:
 *
 *	pci_size_baddr_regs(hose, bus_ptr, dev_ptr, bus, slot)
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *      hose    - primary bus number (for systems with multiple buses)
 *	bus_ptr - pointer to pci bus element structure for this slot
 *      dev_ptr - pointer to pci device element structure for this slot
 *      bus     - the bus number
 *      slot    - the slot number on this bus 
 *
-*/
void pci_size_baddr_regs(int hose,
		    struct pci_bus_elem *bus_ptr,
		    struct pci_dev_elem *dev_ptr,
		    int bus,
		    int slot)
{
int base_addr;
int space_req;
int probe = 0;
int space;
int size;
int cmd_status;
unsigned int	rev_id;


    /* and always disable device in the event that it was set up improperly */
    /* the last time*/

    sys_pci_config_wr( hose, bus, slot, 0xc, 0x00 );
    sys_pci_config_wr( hose, bus, slot, 4, 0x0 );

    cmd_status = 0x47;

    /*
	Loop through base addresses to determine needed space
    */
    for (base_addr = 0; base_addr < PCI_BARS; base_addr++)
    {

	if (base_addr == 6)
	{
	    base_addr += 2;
	}

	/* Determine amount of space needed */
	/* NOTE: mask is now fffffffe to allow for expansion rom addresses */
	sys_pci_config_wr( hose, bus, slot, BASE_ADDR0 + (base_addr*4),0xfffffffe);
	space_req = sys_pci_config_rd( hose, bus, slot, BASE_ADDR0 + (base_addr*4), probe );

	/* 
	    if memory required then
	*/ 
	if (space_req & CFGADDR_MASK)
	{
	    /* 0.2 Tga asic bug really needs 128 Mbytes */
	    /* 0.2 It has only 1 base address register	*/
	    /* 0.2 And it is in memory space 		*/
            if ((dev_ptr->vend_dev_id==DEC_TGA)&&(base_addr == 0))
		space_req = 0xf8000008;

     /* fix io space reqs  pci_init_bar_elem looks   */
     /*  at bits 1, 2 and 3 which are different between  */
     /* IO and MEM space making them 0 just means giving */
     /* it more memory than it may want 		 */
	    if ((space_req & IO_SPACE) == 1 )
	     space_req=space_req&0xfffffff1;

	    /* get space and size elemets */
	    space = space_req & (IO_SPACE | 0x08);
	    if (space == 8)
	    {
		space = PREFETCHABLE_SPACE;
	    }

	    size = ~(space_req & CFGADDR_MASK) + 1;
	    /*
		allocate a base address element
	    */
	    pci_init_bar_elem(bus_ptr, dev_ptr, size, size, space, base_addr);

	    /* check for 64 bit address.  We do this by seeing if it is	    */
	    /* memory.  Only memory addresses can be 64bits		    */

	    if (!(space) && (space_req & 4))
	    {
		/*	 
		**  in 64 bit address mode we have to make sure that the second
		**  address in line is set to zero.  On the systems currently
		**  supported 32 bits is the maximum addressing capability
		*/	 

		/* first we update the base address to get to the high long */
		/* word of the 64 bit address				    */

		base_addr++;
		/* this makes sure its initialized */
		sys_pci_config_wr( hose, bus, slot,
		    BASE_ADDR0 + (base_addr*4), 0);
	    }

	}
    }

    rev_id = dev_ptr->revision_id >> 16;

    if ((rev_id) == 0x1 || (rev_id) == 0x0300 || rev_id == 0x0301)
    {
	if (bus_ptr)
	{
	    /* mark as vga detected behind bridge */
	    bus_ptr->flags |= 1;
	}
    }
    /* here we know we have a video device.  Now determine if it is vga and alter
    ** the current bridge setting to show that.
    */

    if (rev_id == 0x0300 || rev_id == 0x0001 || rev_id == 0x0301)
    {

	if(pci_video_detected)
	{
	    cmd_status = 0x44;	/* disable board */
	}
	pci_video_detected = 1;
    }

    /* and always enable device */
    sys_pci_config_wr( hose, bus, slot, 0xc, 0xff00 );
    sys_pci_config_wr( hose, bus, slot, 4, cmd_status );
}

#if PCI_DEPTH

/*+
 * ============================================================================
 * = pci_get_controller - get controller number for device        	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Initialize the necessary fields in the device element that are needed 
 *      by the port block  	
 *
 * FORM OF CALL:
 *
 *	pci_get_controller(dev_ptr)
 *                    
 * RETURNS:
 *
 *	a pointer to the device_element structure for this device
 *
 * ARGUMENTS:
 *
 *      dev_ptr - pointer for the device element whe have found 
-*/
void pci_get_controller(struct pci_dev_elem *dev_ptr)
{
    int vendor_dev_id = dev_ptr->vend_dev_id;
    unsigned int sub_id = dev_ptr->sub_id;
    struct io_device *iod;
    int *controllers;

    /* see if this is one of our supported devices */
    iod = find_io_device(TYPE_PCI, vendor_dev_id, sub_id);
    if (iod)
    {
	/* get pointer to controller number */
	controllers = iod->controller;    

	/* save it in the device element and postincrement */
	if (controllers)
	{
	    dev_ptr->controller = (*controllers)++;
	}
    }
}
#endif

/*+
 * ============================================================================
 * = pci_size_bus - find all devices on a given PCI bus        		      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Find all pci devices on a PCI bus and determine the memory/IO mapping 
 *      required ...
 *
 * FORM OF CALL:
 *
 *	pci_size_bus(hose, bus_nbr, bus_known, parent)
 *                    
 * RETURNS:
 *
 *	a pointer to the bus_element structure built by this routine
 *
 * ARGUMENTS:
 *
 *      hose    - hose number that you want to size (needed to support multiple PCI primary busses)
 *	bus_nbr - bus number that you want to size
 *                0 means primary bus
 *                > 0 means this is a secondary bus
 *	bus_known - 0 indicates pci 1 indicates eisa ( treated differently)
 *	parent - parent bridge pointer
 *
 * GLOBALS:
 *
 *	bus_count[] - an array of integers that will contain the total number of buses on a primary bus.
 *                    it will be equal to 1 if no secondary buses are found off the primary bus, otherwise
 *                    this will be equal to number of secondary buses found + 1. 
 *
-*/
 struct pci_bus_elem *pci_size_bus(long hose,
				  long bus_nbr,
				  long bus_known,
				  struct pci_dev_elem *parent)
{
    struct pci_bus_elem *this_bus_ptr;
    struct pci_bus_elem *next_bus_ptr;
    struct pci_bus_elem *bus_ptr;
    struct pci_dev_elem	*dev_ptr, *temp_dev_ptr;
    struct pci_dev_elem	*temp_bridge_ptr = bridge_dev_ptr;
    int vendor_dev_id;
    int rev_class_info;
    int sub_id;
    int probe;
    int space;
    int space_req;
    int	alignment;
    int next_bus_nbr;
    int current_align;
    int first_dev_found = 1;
    struct supported_bridge *bridge_ptr;
    int slot;
    int int_line;
    int	max_slots;
    rec_ctr++;


    bridge_dev_ptr = parent;

    if (parent)
    {
	max_slots = PCI_K_MAX_SLOTS_SECONDARY;
    }
    else
    {
	max_slots = PCI_K_MAX_SLOTS_PRIMARY;
    }
    
    /* 
	allocate a pci bus element
    */
    this_bus_ptr = (struct pci_bus_elem *) malloc(sizeof (struct pci_bus_elem));

    /* save pointer to the first element in the list */
    if (bus_nbr == 0)
    {
	first_bus_ptr = this_bus_ptr;
    }


    /* initialize fields in the bus element */
    this_bus_ptr->next_bus_elem = (struct pci_bus_elem *) 0;
    this_bus_ptr->prev_bus_elem = (struct pci_bus_elem *) 0;
    this_bus_ptr->sorted_bar[MEM_SPACE] = (struct pci_baddr_elem *)0;
    this_bus_ptr->sorted_bar[IO_SPACE] = (struct pci_baddr_elem *)0;
    this_bus_ptr->sorted_bar[PREFETCHABLE_SPACE] = (struct pci_baddr_elem *)0;
    this_bus_ptr->bus = bus_count[hose] - 1;
    this_bus_ptr->dev_list = (struct pci_dev_elem *)0;
    this_bus_ptr->space_req[MEM_SPACE] = 0;
    this_bus_ptr->space_req[IO_SPACE] = 0;
    this_bus_ptr->space_req[PREFETCHABLE_SPACE] = 0;

    this_bus_ptr->alignment[MEM_SPACE] = 0;
    this_bus_ptr->alignment[IO_SPACE] = 0;
    this_bus_ptr->alignment[PREFETCHABLE_SPACE] = 0;

    if (bus_known)
    {
#if	1

	/* Get Eisa Information for Phase 4 and 5 drivers */
	eisa_size_config (hose,this_bus_ptr->bus);
#endif
    }
/* end of test code */
    else
    {

	for( slot = 0; slot < max_slots; slot++ )
	{
	    /* first access to config space is a probe */
	    probe = 1; 
	    vendor_dev_id = sys_pci_config_rd(hose, 
		bus_nbr, 
		slot, 
		VEND_DEV_ID, 
		probe);

	    /*
		IF vendor ID comes back as 0 THEN
		    No device found
	    */
	    if(vendor_dev_id == 0 || vendor_dev_id == 0xffffffff) 
	    {
#if PCI_DEBUG 
		pprintf("pci_size_bus - No device found bus %d slot %x \n",bus_nbr,slot);
		err_printf("No pci device slot= %x \n",slot);
#endif
	    }
	    /*
	       ELSE
		  We have found a device 
	    */
	    else                                    /* found a pci device */
	    {
		/* get revision and class information */
		probe = 0; 
		rev_class_info = sys_pci_config_rd(hose, bus_nbr, slot, REV_ID, probe);
		sub_id = sys_pci_config_rd(hose, bus_nbr, slot, SUB_VENDOR_ID, probe);

#if PCI_DEBUG
		err_printf("pci_size_bus - device found bus %d slot= 0x%x vid_did=0x%x int_line 0x%x\n",
		    bus_nbr,slot,vendor_dev_id,int_line);
#endif
		/* allocate a device element */
		dev_ptr = (struct pci_dev_elem *)malloc(sizeof (struct pci_dev_elem));

		/* if first device found then  link to bus element */
		if (first_dev_found)
		{
		    /* link this device to the bus element */
		    this_bus_ptr->dev_list = dev_ptr;

		    /* clear out forward and back links */
		    dev_ptr->next_dev_ptr = (struct pci_dev_elem *)0;
		    dev_ptr->prev_dev_ptr = (struct pci_dev_elem *)0;
		    dev_ptr->parent = (struct pci_dev_elem *)0;

		    /* clear first device found flag .... so we don't enter this */
		    first_dev_found = 0;


		}
		else
		{
		    /* find last element in the list */
		    temp_dev_ptr = this_bus_ptr->dev_list;
		    while (temp_dev_ptr->next_dev_ptr)
		    {   
			temp_dev_ptr = temp_dev_ptr->next_dev_ptr;
		    }

		    /* link this element to the last device */
		    temp_dev_ptr->next_dev_ptr = dev_ptr;
		    
		    /* setup this element's links */
		    dev_ptr->prev_dev_ptr = temp_dev_ptr;
		    dev_ptr->next_dev_ptr = (struct pci_dev_elem *)0;
		}

		/* setup device specific data */
		dev_ptr->vend_dev_id = vendor_dev_id;
		dev_ptr->revision_id = rev_class_info;
		dev_ptr->sub_id = sub_id;
		dev_ptr->bus = bus_nbr;
		dev_ptr->slot = slot;
		dev_ptr->sec_bus_ptr = (struct pci_bus_elem *)0;

		dev_ptr->bridge_dev_ptr = bridge_dev_ptr;

		/* if we find a PCI-PCI Bridge */

#if 0
		if ((rev_class_info & 0xffff0000) == PCI_PCI_BRIDGE) 
#else
		if ((rev_class_info & 0xffff0000) == PCI_PCI_BRIDGE ||
		    (rev_class_info & 0xffff0000) == PCI_EISA_BRIDGE ||
		    ((vendor_dev_id == 0x04828086) && (rev_class_info >= 3)))
#endif

		{

		    dev_ptr->parent = parent;
		    /* save pointer to any bridge device found on the primary bus */


		    /* 
		      get the next bus nbr and then update bus count 

		      NOTE .... need to update bus count before we make 
				recursive call to deal with multiple levels
				of bus cascading
		    */
		    next_bus_nbr = bus_count[hose];
		    bus_count[hose]++;
		    /* code change to only look a info and not known	    */
		    /* bridge.  If the device is a pci bridge then we know  */
		    /* how to handle it.  There may be some special setup   */
		    /* but that is handled by the setup code.		    */

		    if ((rev_class_info & 0xffff0000) == PCI_PCI_BRIDGE) 

		    {
		    
			/* program the bridge for config space */
			ppb_setcfg(vendor_dev_id, 
			    hose, 
			    bus_nbr, 
			    next_bus_nbr, 
			    slot,
			    dev_ptr);
			/* size the next bus .... RECURSION LAND !!! */
			next_bus_ptr = pci_size_bus(hose, next_bus_nbr,0, dev_ptr);

			/* find the end of the bus list */
			bus_ptr = first_bus_ptr;
			while (bus_ptr->next_bus_elem)
			{
			    bus_ptr = bus_ptr->next_bus_elem;
			}
			
			/* link this bus element to the end of list */
			bus_ptr->next_bus_elem = next_bus_ptr;
			next_bus_ptr->prev_bus_elem = bus_ptr;
			next_bus_ptr->next_bus_elem = (struct pci_bus_elem *) 0;

			/* link this bus element to the device structure */
			dev_ptr->sec_bus_ptr = next_bus_ptr;

			/* insert bridges virtual baddrs into sorted list for the bus */
			for (space = MEM_SPACE; space < MAX_SPACE; space++)
			{
			    space_req = next_bus_ptr->space_req[space];
			    alignment = next_bus_ptr->alignment[space];

			    if (space_req)
			    {
				    /*
				    **Align Bridge chip address space to their minimum requirements
				*/
				current_align = space_req & (bridge_aligns[space]-1);
				if (current_align)
				{
				    space_req = (space_req & ~(bridge_aligns[space]-1)) +
						bridge_aligns[space];


				} 

				/* update the space required at the bus level */
				next_bus_ptr->space_req[space] = space_req;

				/*
				    init a base address element
				*/
				pci_init_bar_elem(this_bus_ptr, dev_ptr, space_req, alignment, space, space);
			    }
			} 

		    }
		    /* don't do eisa if breadth probing or we want to always    */
		    /* force it to depth (alcor)				    */

		    else if (bus_probe_algorithm && !force_eisa) 		{

			next_bus_ptr = pci_size_bus(hose, next_bus_nbr,1,dev_ptr);

			/* find the end of the bus list */
			bus_ptr = first_bus_ptr;
			while (bus_ptr->next_bus_elem)
			{
			    bus_ptr = bus_ptr->next_bus_elem;
			}
			
			/* link this bus element to the end of list */
			bus_ptr->next_bus_elem = next_bus_ptr;
			next_bus_ptr->prev_bus_elem = bus_ptr;
			next_bus_ptr->next_bus_elem = (struct pci_bus_elem *) 0;

			/* link this bus element to the device structure */
			dev_ptr->sec_bus_ptr = next_bus_ptr;


		    }
		    else
		    {
			/* back up bus number if old probing code */
			bus_count[hose]--;
		    }

		}
#if 0
		else if ((rev_class_info & 0xff000000) != (BRIDGE_CLASS)) 
#else
		else /* always size bus */
#endif
		{
		    pci_size_baddr_regs(hose, this_bus_ptr, dev_ptr, bus_nbr, slot); 

		    /* Get the interrupt line and pin information  */
		    int_line = sys_pci_config_rd(hose, bus_nbr, slot, INT_LINE, probe);
		    dev_ptr->int_line = int_line;


		    if (rec_ctr < MAX_CASCADE_LEVELS)
		    {

			/* allocate vector for this device */
			dev_ptr->vector = sys_pci_allocate_vector(dev_ptr);
		    }
		    else
		    {
			dev_ptr->vector = 0;
		    }

#if PCI_DEPTH * 0
		    if (bus_probe_algorithm)
		    {
			/* get necessary information for device's port block */
			pci_get_controller(dev_ptr);
		    }
#endif

		}
	    }
	} 
    }

    bridge_dev_ptr = temp_bridge_ptr;

    rec_ctr--;

    return(this_bus_ptr);
}


/*+
 * ============================================================================
 * = pci_config_err - PCI Configuration error handler        		      =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *      PCI Configuration error handler    
 *
 *  FORM OF CALL:
 *
 *      pci_config_err()
 *
 *  RETURNS:
 *
 *      None
 *
 *  ARGUMENTS:
 *
 *      err_type    - an integer error type.  This will be used to index into an
 *		      array of error message strings for output to the TT
 *		      device.
-*/
 void pci_config_err(int err_type)
    {
    }

/*+
 * ============================================================================
 * = msb -  returns the most significant bit of the number argument           =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *      msb - returns the most significant bit of the number argument
 *
 * FORM OF CALL:
 *
 *      msb (number)
 *
 *  RETURNS:
 *
 *      Most Significant Bit of number.
 *	For example, msb (0x1efb) returns 0x1000
 *
 *  ARGUMENTS:
 *
 *      number
 *
-*/
 int msb (int number)
    {
    int shift_counter = 0;
    
    while (number)	/* while there are bits in the number */
	{
	shift_counter++;
	number >>= 1;
	}
    return (1 << shift_counter);    
    }

/*+
 * ============================================================================
 * = lsb -  returns the least significant bit of the number argument           =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *      lsb - returns the least significant bit of the number argument
 *
 * FORM OF CALL:
 *
 *      lsb (number)
 *
 *  RETURNS:
 *
 *      Least Significant Bit of number.
 *	For example, msb (0x1efb) returns 0x1000
 *
 *  ARGUMENTS:
 *
 *      number
 *
-*/
 int lsb (int number)
    {
    int shift_counter = 0;
    int bit_mask = 1;
    
    while (((number & bit_mask) == 0) && (shift_counter < 33))
	{
	shift_counter++;
	bit_mask <<= 1;
	}
    return (bit_mask);    
    }

/*+
 * ============================================================================
 * = even_power_of_2 - determines if number arg is an even power of 2	      =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *      even_power_of_2 - determines if number arg is an even power of 2
 *
 * FORM OF CALL:
 *
 *      even_power_of_2(number)
 *
 *  RETURNS:
 *
 *      0 if the number IS NOT an even power of 2
 *	1 if the number IS an even power of 2
 *
 *  ARGUMENTS:
 *
 *      number	- longword integer
 *
 *  CALLS:
 *
 *	msb()
 *	
-*/
 int even_power_of_2 (int number)
    {
    int temp;

    temp = msb (number);
    return (temp < number) ? 0 : 1;
    }

/*+
 * ============================================================================
 * = get_aligned_baddr - get best aligned fit for current base address	      =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *      get best aligned fit for current base address
 *
 * FORM OF CALL:
 *
 *      get_aligned_baddr (current_addr_ptr, first_baddr_ptr); 
 *
 *  RETURNS:
 *
 *	pointer to the base address element that best fits the current base
 *	address alignment.
 *
 *  ARGUMENTS:
 *
 *      current_addr_ptr - pointer to the current (cumulative) base address
 *	first_baddr_ptr	 - pointer to the first base address element in the
 *			   baddr element list for this bus.
 *
 *  CALLS:
 *
 *	lsb()
 *	get_aligned_baddr - calls itself recursively
 *	
-*/
 struct pci_baddr_elem *get_aligned_baddr (int *current_addr_ptr, struct pci_baddr_elem *first_baddr_ptr)
{
    struct pci_baddr_elem *baddr_ptr;
    int baddr_elem_found = 0;
    int end_of_list = 0;
    int alignment;
    int	space_req;
    int current_addr_mask;
    int space_req_mask;
    
    baddr_ptr = first_baddr_ptr; 
    current_addr_mask = (lsb (*current_addr_ptr)) - 1;

    /*
	Loop until we find the best fitted base address element
    */
    do
    {
	/* get alignment required by the current base address element */
	alignment = baddr_ptr->alignment;
	space_req = baddr_ptr->space_req;
	
	/* get mask for space required */
	space_req_mask = (lsb (alignment)) -1;

	/* 
	    if space required fits in current alignment the
		address found
	*/
	if (space_req_mask <= current_addr_mask)
	{
	    /* write current address as base address */
	    baddr_ptr->pci_addr = *current_addr_ptr;
	    /* update current address  */
	    *current_addr_ptr += space_req;

	    /* flag element as found */
	    baddr_elem_found = 1;
	}
	else if (baddr_ptr->next_bar_ptr)
	{
	    baddr_ptr = baddr_ptr->next_bar_ptr;
	}
	else
	{
	    /* update alignement to minimum alignment requirements */
	    
	    *current_addr_ptr &= ~space_req_mask;
	    *current_addr_ptr += lsb (alignment);

	    /* go look again .. worst case we will hit on last element in the list */
	    baddr_ptr = get_aligned_baddr (current_addr_ptr, first_baddr_ptr);

	    /* flag element as found */
	    baddr_elem_found = 1;
	}
    }
    while ( ! baddr_elem_found);

    return (baddr_ptr); 
}

/*+
 * ============================================================================
 * = pci_config_dev - Configure the next device				      =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *      Configure the next device on the bus in a best fit means
 *
 * FORM OF CALL:
 *
 *	pci_config_dev (hose, baddr_ptr)
 *
 *  RETURNS:
 *
 *	Pointer to the new base address element in the list     
 *
 *  ARGUMENTS:
 *
 *	hose	  - number of the primary bus
 *      baddr_ptr - pointer to the base address structure for this device
 *
 *  CALLS:
 *
 *	get_aligned_baddr
 *	sys_pci_config_wr
 *      get_bridge_info
 *
 *  CONSTANTS:
 *
 *	pci_size_config_errors enumeration
 *	BASE_ADDR0  - pci_size_config_alcor.h
 *
-*/
struct pci_baddr_elem *pci_config_dev (int hose, 
				       struct pci_baddr_elem *first_baddr_ptr)
{
    struct pci_baddr_elem *baddr_ptr;	/* pointer to fitted baddr  */
    struct pci_dev_elem *dev_ptr;
    int *current_addr_ptr;
    int space;
    int vendor_dev_id;
    int class_info;
    struct pci_bus_elem *bus_ptr;
    int slot;
    int bus;         
    int start_current_baddr;
    int end_current_baddr;

/*	 
**  init the port block structure table.  The find_pb will need this later.
*/	 

    space = first_baddr_ptr->space; 

    /* get pointer to the current base address for this space */

    current_addr_ptr = &current_baddr[hose][space];
 

    /* 
	Save current base address value in case next device is a bridge
    
	We will need to know this so we can configure the  devices 
	behind the brdige  
    */
    start_current_baddr = current_baddr[hose][space];

    /*
    * Find the device having the best fit for the alignment of the current
    * base address.  
    */

    baddr_ptr = get_aligned_baddr (current_addr_ptr, first_baddr_ptr);
	
    /* get pointer to this device */
    dev_ptr = baddr_ptr->dev_back_ptr;   
    
    /* get the data needed from device element */
    vendor_dev_id = dev_ptr->vend_dev_id;
    class_info = dev_ptr->revision_id & 0xffff0000;
    slot = dev_ptr->slot;
    bus = dev_ptr->bus;
    bus_ptr = dev_ptr->sec_bus_ptr;

    /*
    * IF this is a valid pci-to-pci bridge device,
    */
    if (class_info == PCI_PCI_BRIDGE)
    {
	/*
	* go setup the bridge device for normal operation 
	*/

	/* IF secondary bus exists it can be configured */
	if (dev_ptr->sec_bus_ptr)
	{
	    /* save ending base address for bridge */
	    end_current_baddr = current_baddr[hose][space]; 

	    /* restore base address value before bridge */
	    current_baddr[hose][space] = start_current_baddr;

	    /* Size the secondary bus ... */
	    pci_config_bus (hose, dev_ptr->sec_bus_ptr, space);

	    /* restore base address value after bridge */
	    current_baddr[hose][space] = end_current_baddr;

	    /* configure the bridge device */
	    ppb_setmio(vendor_dev_id, hose, bus, slot, bus_ptr, baddr_ptr ); 
	}

    }
    else 
    {
	/*
	* Write the pci_addr of this device for this address region into
	* the appropriate base address register.
	*/


	/*
	    If we are on a secondary bus and IO space BAR and Address > 64K THEN
		We can configure this BAR thus write a 0
	*/
	if ( bus && (baddr_ptr->space == IO_SPACE) && (baddr_ptr->pci_addr > 0x10000))
	{
	    baddr_ptr->pci_addr = 0;
	}

#if PCI_DEBUG
 pprintf("pci_config_dev %x- config device at bus %x slot %x bar %x to pci addr %x\n",
	baddr_ptr->space,
	bus,
	slot,
	baddr_ptr->bar_nbr,
	baddr_ptr->pci_addr);

#endif
#if K2 || TAKARA
	if (baddr_ptr->bar_nbr == 8 && vendor_dev_id == DEC_TGA2 && class_info == 0x03000000)
#else
	if (baddr_ptr->bar_nbr == 8 && vendor_dev_id == DEC_TGA2)
#endif
	{
	    baddr_ptr->pci_addr = tga2Rombase;
	    tga2Rombase += 0x8000;  /* next value */
	}

	sys_pci_config_wr( hose, bus, slot, BASE_ADDR0 + (baddr_ptr->bar_nbr*4), baddr_ptr->pci_addr);
    }
	
    /*
	if This element has a previous element linked to it then
	    this becomes 
    */    
    if (baddr_ptr->prev_bar_ptr)
    {
	baddr_ptr->prev_bar_ptr->next_bar_ptr = baddr_ptr->next_bar_ptr;
    }

    /*
	if this element has a next element then 
	    it should be linked to the previous element
    */
    if (baddr_ptr->next_bar_ptr)
    {
	baddr_ptr->next_bar_ptr->prev_bar_ptr = baddr_ptr->prev_bar_ptr;
    }

    /*
     if base address is equal to first_baddr_ptr then
        set first_baddr_ptr to the old first_baddr_ptr's next address
    */
    if (baddr_ptr == first_baddr_ptr)
    {
	first_baddr_ptr = baddr_ptr->next_bar_ptr;
    }

    /*
    * Unlink the current pci_baddr_elem from the list.
    */
    baddr_ptr->next_bar_ptr = (struct pci_baddr_elem *)0;
    baddr_ptr->prev_bar_ptr = (struct pci_baddr_elem *)0;
    
    return(first_baddr_ptr);

}

/*+
 * ============================================================================
 * = pci_assign_letters - Assign controller letters to devices	              =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *     Assign controller letters to devices. If a secondary bus is encountered,
 *     this routine will call itself recursively to assign controllers to that
 *     bus.
 *
 * FORM OF CALL:
 *
 *      pci_assign_letters(hose, bus_ptr)
 *
 *  RETURNS:
 *
 *	0 status is good
 *	nonzero status indicates problems
 *
 *  ARGUMENTS:
 *  
 *	hose	    - primary bus number
 *      bus_ptr     - pointer to the pci_bus_elem structure
 *
 *  CONSTANTS:
 *  
 *  CALLS:
 *
 *	pci_assign_letters - this routine can call itself recursively
 *
-*/
void pci_assign_letters(int hose, struct pci_bus_elem *bus_ptr)
{
    unsigned int rom_bar;
    struct pb dummy_pb;
    struct pci_dev_elem *dev_ptr;

    /*
     *  Walk the devices on this bus, assigning controller letters as we go.
     *  Also, along the way, check for the NCR 53C825 so we can special-case
     *  it, to see if it's SCSI or DSSI.
     */

    /* get pointer to the start of the device list */
    dev_ptr = bus_ptr->dev_list;

    /* while there are devices to search */
    while (dev_ptr)
    {
	if (dev_ptr->vend_dev_id == 0x00031000)
	{
	    /* turn on expansion rom, read four bytes, 
	     * compare with 'DSSI' 
	     */
	    dummy_pb.hose = hose;
	    dummy_pb.bus  = dev_ptr->bus;
	    dummy_pb.slot = dev_ptr->slot;
	    dummy_pb.function = 0;

	    rom_bar = incfgl( &dummy_pb, EXP_ROM_BASE );
	    outcfgl( &dummy_pb, EXP_ROM_BASE, rom_bar | 1 );
	    if( inmeml( &dummy_pb, rom_bar ) == 'DSSI' )
		dev_ptr->vend_dev_id += 0x80000000;
	    outcfgl( &dummy_pb, EXP_ROM_BASE, rom_bar );
	}

#if PCI_DEPTH * 1
	if (bus_probe_algorithm)
	{
	    /* get necessary information for device's port block */
	    pci_get_controller(dev_ptr);
	}
#endif
		
	/* descend if this is a bridge */
	if (dev_ptr->sec_bus_ptr)
	{
	    pci_assign_letters(hose, dev_ptr->sec_bus_ptr);
	}

	/* get pointer to the next device */
	dev_ptr = dev_ptr->next_dev_ptr;
    }
}

/*+
 * ============================================================================
 * = pci_config_bus - Configure a PCI bus        		              =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *     Configure the pci bus.  IF a secondary bus is encountered, this routine 
 *     will call itself recursively to configure that bus.
 *
 * FORM OF CALL:
 *
 *      pci_config_bus(hose, bus_nbr,space)
 *
 *  RETURNS:
 *
 *	0 status is good
 *	nonzero status indicates problems
 *
 *  ARGUMENTS:
 *  
 *	hose	    - primary bus number
 *      pci_bus_ptr - pointer to the pci_bus_elem structure
 *      space       - space to configure for that bus 
 *
 *  CONSTANTS:
 *  
 *	pci_size_config_errors enumeration
 *	MAX_SPACE   - pci_size_config.h
 *
 *  CALLS:
 *
 *	pci_config_bus - this routine can call itself recursively
 *	pci_config_dev
 *      get_bridge_info
 *
-*/
void pci_config_bus (int hose, struct pci_bus_elem *pci_bus_ptr, int space)
    {
    struct pci_baddr_elem *first_baddr_ptr;	/* pointer to device base addr list */

    /* get pointer to first element in sorted list */
    first_baddr_ptr = pci_bus_ptr->sorted_bar[space];

    /* if an element exists ..... */
    if (first_baddr_ptr)
    {
	/*
	  Loop until we have run out of base addresses to configure 
	*/
	do
	{
	    /* configure the device with the largest space requirements */
	    first_baddr_ptr = pci_config_dev (hose, first_baddr_ptr);

	}
	while (first_baddr_ptr != (struct pci_baddr_elem *)0);
    }

}


/*+
 * ============================================================================
 * = pci_size_config - find and configure all devices on all PCI buses in the system =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Find and Configure all pci devices on the PCI buses in the system.
 *
 * FORM OF CALL:
 *
 *	pci_size_config()
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *      None
 *
 * GLOBALS:
 *   
 *      primary_bus_ptr[] - this is an array of pointers to pci_bus_elem strucures for all
 *                          primary bus elements in the system. These pointer will be filled
 *                          in upon return from this routine.
 * 
 *                          Your platform_io.h file should have a constant PCI_K_MAX_BUS to 
 *                          control this
-*/     
void pci_size_config()
{
    int hose;
    int i,bus_nbr = 0;
    int space;    

    num_pbs = 0;
    num_pbs_all = 32;
    pci_video_detected = 0;
    pci_os_type = (int)rtc_read( 0x3f );
    last_vector_used = 0;

    /* 
    // xx_controllers fields get initialized to 0, on every init command, 
    // as part of the io_device_tbl initialization. See the platform_init.c 
    // file for the platform you are building.
    */

    tga2Rombase = 0xc0000;	/* hack to handle tga2 */

    pbs = (struct pb** ) malloc( sizeof( struct pb * ) * 32 );

#if PCI_DEBUG 
    pprintf("PCI_size_config entered\n");
#endif

    /* Initialize Current Value Using Start Value	*/    
                                                                                                          
    for (i=0;i<PCI_K_MAX_BUS;i++)
     {
     current_baddr[i][MEM_SPACE] = start_baddr[i][MEM_SPACE];
     current_baddr[i][IO_SPACE]  = start_baddr[i][IO_SPACE];
     current_baddr[i][PREFETCHABLE_SPACE]  = start_baddr[i][PREFETCHABLE_SPACE];
     primary_bus_ptr[i]=0;
     bus_count[i]=0;
     }

/*    manual_config = 1; */
    ETAPciMaxBusNumber = 5;
    rec_ctr = 0;			/* recursion counter */

    /*
	FOR all buses in the system 
    */
    for (hose = 0; hose < PCI_K_MAX_BUS; hose++)
    {

	bus_count[hose] = 1;
	/*
	    Size the bus and build sorted list of base address elements
	*/
	primary_bus_ptr[hose] = pci_size_bus(hose, bus_nbr, 0,0);


#if PCI_DEBUG 

	/* type debug info */
	print_size_info();
#endif
	/*
	    Configure the devices on this bus using a best fit algorithm
	*/
	for (space = MEM_SPACE; space < MAX_SPACE; space++)
	{
	    pci_config_bus(hose, primary_bus_ptr[hose],space);
	}

	pci_assign_letters(hose, primary_bus_ptr[hose]);
    }

}

/*+
 * ============================================================================
 * = pci_find - locate all devices with a given id.       		      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Find all device with the given pci id in the system
 *
 * FORM OF CALL:
 *
 *	pci_find(id, sub_id)
 *                    
 * RETURNS:
 *
 *	pointer to a structure of pci_devs_found, pointer will be null if no devices
 *      found                                                                                       
 *
 * ARGUMENTS:
 *
 *	id	- device id to look for
 *
 *	sub_id	- subd id to look for
 *
 * GLOBALS:
 *
 *      primary_bus_ptr[] - an array of pointers to primary bus elements ... one per hose in system
-*/
struct pci_devs_found *pci_find ( int vend_dev_id, int sub_id)
{
struct pci_bus_elem *bus_ptr;
struct pci_dev_elem *dev_ptr;
struct pci_devs_found *dev_table_ptr;
struct pci_device_tbl *dev_fnd_ptr;
int hose;
int devices_found = 0;

    /* allocate a device found table */
    dev_table_ptr = (struct pci_devs_found *)malloc(sizeof(struct pci_devs_found));

    for (hose = 0; hose < PCI_K_MAX_BUS; hose++)
    {
	/* get pointer to the bus structure for this hose */
	bus_ptr = primary_bus_ptr[hose];

	/* While there are buses to search */
	while (bus_ptr)
	{
	    /* get pointer to the start of the device list */
	    dev_ptr = bus_ptr->dev_list;

	    /* While there are devices to search */
	    while (dev_ptr)
	    {
		if (dev_ptr->vend_dev_id == vend_dev_id &&
		    dev_ptr->sub_id == sub_id)
		{
		    /* get pointer to entry for this device */
		    dev_fnd_ptr = &dev_table_ptr->dev_found[devices_found];

		    /* fill in the entry */
		    dev_fnd_ptr->hose = hose;
		    dev_fnd_ptr->bus = dev_ptr->bus;
		    dev_fnd_ptr->slot = dev_ptr->slot;
		    dev_fnd_ptr->int_line = dev_ptr->int_line;
		    dev_fnd_ptr->vector = dev_ptr->vector;
#if PCI_DEPTH
		    if (bus_probe_algorithm)
		    {
		        dev_fnd_ptr->controller = dev_ptr->controller;
		    }
#endif
		    /* update the device found count */
		    devices_found++;

		}

		/* get pointer to the next device */
		dev_ptr = dev_ptr->next_dev_ptr;
	    }
	    /* get next bus element */
	    bus_ptr = bus_ptr->next_bus_elem;
	}
    }

    /* 
	if device found then 
	    save number of devices found 
	else
	    free structure and set structure pointer to null

    */
    if (devices_found)
    {
	dev_table_ptr->nbr_found = devices_found;
    }
    else
    {
	free(dev_table_ptr);
	dev_table_ptr = (struct pci_devs_found *) 0;
    }
    return(dev_table_ptr);
}

/*+
 * ============================================================================
 * = pci_convert_pins - convert the interrupt pin to slot value		      =
 * ============================================================================
 *
 * OVERVIEW:
 *	
 *	    convert the interupt pin value to a normalized value based on bridge
 *	    and slot number.  The intent of this is to convert the interrupt
 *	    pins to what the main/paranet bus wants to see.
 *
 * FORM OF CALL:
 *
 *	pci_convert_pin (interrept_pin, current_bridge)
 *                    
 * RETURNS:
 *
 *	converted interrupt pins or zero if no conversion - pin is 0 or greater
 *	than 4.
 *
 * ARGUMENTS:
 *	
 *	device_slot - slot behind the bridge
 *
 *	interrupt_pin - value of pin to convert. - range is 1 through 4.
 *
 *	current_bridge - pointer to current bridge value.
 *
 * GLOBALS:
 *
 *      none
 *
 *
-*/

int pci_convert_pin(int device_slot, 
    int interrupt_pin,
    struct pci_dev_elem *current_bridge)
    
{
    struct pci_dev_elem *local_bridge_ptr = current_bridge;
    int	current_bridge_slot = local_bridge_ptr->slot;

    do
    {
	/* determine slot of PCI to PCI bridge */

	interrupt_pin = (device_slot % 4) + interrupt_pin;
	if (interrupt_pin > 4)
	{
	    interrupt_pin -= 4;
	}
	local_bridge_ptr = local_bridge_ptr->parent;	
	if (local_bridge_ptr)
	{
	    device_slot = current_bridge_slot;
	    current_bridge_slot = local_bridge_ptr->slot;
	}

    } while (local_bridge_ptr);

    return(interrupt_pin);
}


/*+
 * ============================================================================
 * = pci_get_parent - return the paraent bridge pointer.
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Return the top most bridges address.  This will be parent_bridge[2].  If
 *	a call is made to this function and there is no parent then 0 is
 *	returned.
 *
 * FORM OF CALL:
 *
 *	pci_get_parent ()
 *                    
 * RETURNS:
 *
 *	pointer to a structure of bridge_dev, pointer will be null if no bridge
 *      found, or current bridge.
 *
 * ARGUMENTS:
 *
 *	current_bridge - pointer to current bridge value.
 *
 * GLOBALS:
 *
 *      parent_bridge - array of bridge pointers.
-*/

struct pci_dev_elem *pci_get_parent(struct pci_dev_elem *current_bridge)
{

    struct pci_dev_elem *main_parent = (struct pci_dev_elem *)0;

    main_parent = current_bridge;

    while (main_parent->parent)
    {
	main_parent = main_parent->parent;
    }
    return (main_parent);
}

#if PCI_DEBUG
void print_size_info (void)
{
struct pci_bus_elem *bus_ptr;
struct pci_dev_elem *dev_ptr;
struct pci_baddr_elem *temp_baddr_ptr;
int hose;
int devices_found = 0;

    pprintf("************** PCI size info ****************\n\n");


    for (hose = 0; hose < PCI_K_MAX_BUS; hose++)
    {
	/* get pointer to the bus structure for this hose */
	bus_ptr = primary_bus_ptr[hose];
	pprintf("bus ptr = %x\n", bus_ptr);

	/* While there are buses to search */
	while (bus_ptr)
	{
	    pprintf("BUS  %x\n", bus_ptr->bus);
	    pprintf("BUS  sorted_bar = %08x	%08x\n", bus_ptr->sorted_bar[0], bus_ptr->sorted_bar[1]);
	    pprintf("BUS  dev_list = %08x\n", bus_ptr->dev_list);
	    pprintf("BUS  mem_req = %08x\n", bus_ptr->space_req[0]);
	    pprintf("BUS  io_req = %08x\n", bus_ptr->space_req[1]);
	    pprintf("BUS  prefetchable_req = %08x\n", bus_ptr->space_req[2]);
	      
	    /* get pointer to the start of the device list */
	    dev_ptr = bus_ptr->dev_list;

	    /* While there are devices to search */
	    while (dev_ptr)
	    {
		pprintf("    DEVICE in slot %x\n",dev_ptr->slot);
		pprintf("	vend_dev_id = %08x\n",dev_ptr->vend_dev_id);
		pprintf("	revision_id = %08x\n",dev_ptr->revision_id);
		pprintf("	bus = %08x\n",dev_ptr->bus);
		pprintf("	baddr_ptr = %08x\n",dev_ptr->baddr_ptr);
		pprintf("	sec_bus_ptr = %08x\n",dev_ptr->sec_bus_ptr);

		/* get pointer to the next device */
		dev_ptr = dev_ptr->next_dev_ptr;
	    }
	    /* print out linked list of baddrs */
	    pprintf("*********Memory space baddrs *******\n");
	    temp_baddr_ptr = bus_ptr->sorted_bar[0];
	    while (temp_baddr_ptr)
	    {
		pprintf("BADDR slot %x: dev_ptr = %x bar_nbr = %x space_req = %x prev_bar = %x next_bar =%x bar =%x\n",
			temp_baddr_ptr->dev_back_ptr->slot,
			temp_baddr_ptr->dev_back_ptr,
			temp_baddr_ptr->bar_nbr,
			temp_baddr_ptr->space_req,
			temp_baddr_ptr->prev_bar_ptr,
			temp_baddr_ptr->next_bar_ptr,
			temp_baddr_ptr);

			temp_baddr_ptr = temp_baddr_ptr->next_bar_ptr;		    
	    }

	    pprintf("*********I/O space baddrs *******\n");
	    temp_baddr_ptr = bus_ptr->sorted_bar[1];
	    while (temp_baddr_ptr)
	    {
		pprintf("BADDR slot %x: dev_ptr = %x bar_nbr = %x space_req = %x prev_bar = %x next_bar =%x bar =%x\n",
			temp_baddr_ptr->dev_back_ptr->slot,
			temp_baddr_ptr->dev_back_ptr,
			temp_baddr_ptr->bar_nbr,
			temp_baddr_ptr->space_req,
			temp_baddr_ptr->prev_bar_ptr,
			temp_baddr_ptr->next_bar_ptr,
			temp_baddr_ptr);
			
			temp_baddr_ptr = temp_baddr_ptr->next_bar_ptr;		    
	    }

	    pprintf("*********Prefetchable space baddrs *******\n");
	    temp_baddr_ptr = bus_ptr->sorted_bar[2];
	    while (temp_baddr_ptr)
	    {
		pprintf("BADDR slot %x: dev_ptr = %x bar_nbr = %x space_req = %x prev_bar = %x next_bar =%x bar =%x\n",
			temp_baddr_ptr->dev_back_ptr->slot,
			temp_baddr_ptr->dev_back_ptr,
			temp_baddr_ptr->bar_nbr,
			temp_baddr_ptr->space_req,
			temp_baddr_ptr->prev_bar_ptr,
			temp_baddr_ptr->next_bar_ptr,
			temp_baddr_ptr);
			
			temp_baddr_ptr = temp_baddr_ptr->next_bar_ptr;		    
	    }
	    pprintf("********* done *******\n\n");

	    /* get next bus element */
	    bus_ptr = bus_ptr->next_bus_elem;
	}


    }

}
#endif


/*+
 * ============================================================================
 * = decppb - init DEC 21050/21052/21152 specific registers  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Set up the Digital PPB specific registers
 *
 * FORM OF CALL:
 *
 *	decppb (current_bridge)
 *                    
 * RETURNS:
 *
 *	none
 *
 * ARGUMENTS:
 *
 *	current_bridge - pointer to current bridge value.
 *
 * GLOBALS:
 *
 *      parent_bridge - array of bridge pointers.
-*/

void decppb(int hose, int bus, int slot)
{
    struct pb dummy_pb;

    dummy_pb.hose = hose;
    dummy_pb.slot = slot;
    dummy_pb.bus  = bus;
    dummy_pb.function  = 0;
    /* 32 pci cycle */

#if 0
    /*
    **  need to assert bit 1 because of quad tulip.  This needs to be looked
    **  into when time allows.
    */

    outcfgb(&dummy_pb, 0x40, 0x0);
#endif

}

/*+
 * ============================================================================
 * = ibmppb - init IBM specific registers  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Set up the IBM PPB specific registers
 *
 * FORM OF CALL:
 *
 *	ibmppb (current_bridge)
 *                    
 * RETURNS:
 *
 *	none
 *
 * ARGUMENTS:
 *
 *	current_bridge - pointer to current bridge value.
 *
 * GLOBALS:
 *
 *      parent_bridge - array of bridge pointers.
-*/

void ibmppb(int hose, int bus, int slot)
{
    struct pb dummy_pb;

    dummy_pb.hose = hose;
    dummy_pb.slot = slot;
    dummy_pb.bus  = bus;
    dummy_pb.function  = 0;

    /*	 
    **  the following should give optimum performance
    */	 
    outcfgb(&dummy_pb, 0x41, 0xa1); /* max buffering and caching */
    outcfgb(&dummy_pb, 0x42, 0xa1); /* because of secondary bridging */
    outcfgb(&dummy_pb, 0x50, 0x10); /* causes bridge to park on last	    */
				    /* granted agent			    */
    

}

#ifdef __cplusplus
} // end of extern "C" block
#endif

