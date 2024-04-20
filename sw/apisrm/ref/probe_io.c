/*
 * Copyright (C) 1993 by
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	I/O probing code
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *
 *	er	 8-oct-1999	Add Yukon PCI Hot-Plug controller
 *      mar	 7-jun-1999	Add DE600-AA.DE602-AA,FA,TA Ethernet Cards
 *				DE602-FA/TA are daughter cards.                  
 *      mar	27-may-1999	Use common naming for Intel 82558/9 chips
 *				because they use the same Device ID.
 *      ska	06-may-1999	Increased UNIX window from 512 to 640MB for YUKONA.
 *      mar     21-apr-1999     Add new device id for pbxdp-aa                  ci()
 *      mar     11-Mar-1999     Add DAPCA-FA, DAPBA-UA, DAPBA-FA                ci()
 *      tna	 9-Feb-1999     Initializing 'total_dense' for YukonA in probe_pci()
 * 	tna	 4-Jan-1998	Universe size configurations
 *	tna      6-Aug-1998     Removal of VIP/VIC initialization for
 *                              Yukona boards (based on the Tundrachip)
 *				through "Universe" conditionals 
 *
 *	mar 	08-sep-1998	Add DEGPA-SA gigabit etherner card             
 *	mar 	01-sep-1998	Update device ID's for Emulex card: PBXDP-AB/AC
 *	dtr	14-apr-1998	Added adaptec 2940 and pixelworks support.
 *	mar     06-mar-1998	Added PCI-to-Cardbus support. 
 *	mar	10-Feb-1998     Rename PBXNP-AC to SN-PBXNP-AC
 *				Rename Thomas Conrad Token Ring to PBXNP-AA
 *	mar	04-Feb-1998     Add PBXDA-AA/AB, -AC
 *	mar	29-Jan-1998	Add PBXNP-AC
 *	mar	11-Nov-1997	Add Cateyes 4D5IT graphics controller
 *	mar	16-Sep-1997	Add DE500-FA                          
 *	er	 1-May-1997	Add ISA bus probing.
 *	mar	 5-Mar-1997	Add DE500-BA which uses DECchip 21143 
 *                              which has Vendor ID [00191011].
 *
 *      rhb     29-Jul-1996     Add Emulex PCI-Synch controllers(PBXDP-AA thru -AF)
 *                              ECO #1061.1
 *      rhb     23-Jul-1996     Add Digital FullVideo Supreme JPEG (AV321)
 *                              Add Digital FullVideo Supreme (AV301)
 *                              ECO #1061
 *	pas	18-Mar-1996	Add Digital ATMworks 350; use hw controller
 *	mar	 4-Aug-1995	Add io_device [ID] 
 *				DECChip 21041 [00141011] used on DE450 module
 *				DECChip 21140 [00091011] used on DE500 module
 *	sfs	11-Nov-1993	Initial entry.
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:probe_io_def.h"
#include "cp$inc:platform_io.h"
#include "cp$src:eisa.h"
#include "cp$inc:kernel_entry.h"

#define PROBE_EISA_SLOTS (PROBE_EISA && EISA_MAX_SLOT)
#define PROBE_ISA_SLOTS (PROBE_ISA && ISA_MAX_SLOT)

#if PROBE_ISA_SLOTS
#include "cp$src:isacfg.h"
#include "cp$src:hwrpb_def.h"
#endif

#if FIBRE_CHANNEL
#include "cp$src:wwid.h"
#endif

#ifndef HOSE
#define HOSE(x) x
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef DEBUG_PROBE
#define DEBUG_PROBE 1
#endif

int dq_controllers;
int dr_controllers;
int dw_controllers;
int en_controllers;
int er_controllers;
int ew_controllers;
int fr_controllers;
int fw_controllers;
int mc_controllers;
int pk_controllers;
int pg_controllers;
int pu_controllers;
int rt_controllers;
int tg_controllers;
int ei_controllers;

int i2o_controllers;
int usb_controllers;

int isa_eisa_count;

int probe_number;

char *pci_space_strings[] = { "16 bit I/O", "I/O", "sparse", "dense" };

unsigned int pci_base[SIZE_GRAN_BASE_NUM];
unsigned int pci_size[SIZE_GRAN_BASE_NUM];
int pci_has_eisa_isa;
int pci_slot;
int pci_type0_type1_bug;

#if MODULAR
extern int vg_controllers;
extern int manual_config;
extern int pci_bus;

extern struct SEMAPHORE scs_lock;
extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
extern unsigned int num_pbs_all;
extern struct SEMAPHORE sbs_lock;
extern struct sb **sbs;
extern unsigned int num_sbs;
extern unsigned int num_sbs_all;
extern struct SEMAPHORE ubs_lock;
extern struct ub **ubs;
extern unsigned int num_ubs;
extern unsigned int num_ubs_all;
extern struct SEMAPHORE wwids_lock;
extern struct registered_wwid **wwids;
extern unsigned int num_wwids;
extern unsigned int num_wwids_all;
extern unsigned int wwid_ev_mask;
extern struct registered_portname **portnames;
extern unsigned int num_portnames;
extern unsigned int portname_ev_mask;
#else 
int vg_controllers;
int manual_config;
int pci_bus;

struct SEMAPHORE scs_lock;
struct SEMAPHORE pbs_lock;
struct pb **pbs;
unsigned int num_pbs;
unsigned int num_pbs_all;
struct SEMAPHORE sbs_lock;
struct sb **sbs;
unsigned int num_sbs;
unsigned int num_sbs_all;
struct SEMAPHORE ubs_lock;
struct ub **ubs;
unsigned int num_ubs;
unsigned int num_ubs_all;
#if FIBRE_CHANNEL
struct SEMAPHORE wwids_lock;
struct registered_wwid **wwids;
unsigned int num_wwids;
unsigned int num_wwids_all;
unsigned int wwid_ev_mask;
struct registered_portname **portnames;
unsigned int num_portnames;
unsigned int portname_ev_mask;
extern int wwidmgr_mode;
#endif
#endif

#if FRU5
struct QUEUE vpd_data_q;
#endif

struct pci_size_info *parent_psi;
struct pci_size_info *current_psi;
                                                                             
#if UNIVERSE
struct pci_size_info *universe_640_psi;
int total_dense;
#endif

#if SABLE || MIKASA
int bus_probe_algorithm;
#else
#define bus_probe_algorithm 1
#endif

#if MEDULLA
int set_pcmcia_mask( );
static int med_bridge = 0;
#endif

find_pb( char *device, int size, int (*callback)( ), int p1, int p2 )
    {
    int i;
    struct pb *pb;

    krn$_wait( &pbs_lock );
    for( i = 0; i < num_pbs; i++ )
	{
	pb = pbs[i];
	if( pb->device && ( strcmp( pb->device, device ) == 0 ) )
     	    {
	    if( size )
		{
		pb = realloc_noown( pb, size );
		pbs[i] = pb;
		}
	    if( callback )
		callback( pb, p1, p2 );
	    else 
		{
		krn$_post( &pbs_lock );
		return( pb );
		}
	    }
	}
    krn$_post( &pbs_lock );
    return( NULL );
    }

/*   
 *++
 * =========================================================================
 * = find_dev_pb - look for 'device' in the port block list		   =
 * =========================================================================
 *
 *  FUNCTIONAL DESCRIPTION:
 *
 *      find_dev_pb
 *            
 *  FORMAL PARAMETERS:
 *
 *      char *c - string containing a device mnemonic
 *
 *  RETURN VALUE:
 *      
 *	pb	- A pointer to the port block if found.
 *		  If not found NULL is returned.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *            
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *
 *--
 */                       
find_dev_pb( char *c )
    {
    int i;
    struct pb *pb;
    char mnem[64];

    krn$_wait( &pbs_lock );
    for( i = 0; i < num_pbs; i++ )
	{
	pb = pbs[i];
	sprintf(mnem, "%s%c", pb->protocol, pb->controller + 'a');
	if (strcmp_nocase(c,mnem) == 0)
	    {
	    krn$_post( &pbs_lock );
	    return( pb );
	    }
	}
    krn$_post( &pbs_lock );
    return( NULL );
    }


#if DRIVERSHUT
#include "cp$src:io_device_list.h"
#endif
#if !DRIVERSHUT
struct io_device io_device[] = {
#if PROBE_EISA_SLOTS || PROBE_ISA_SLOTS
    { TYPE_EISA, 0x70009835, 0x00000000, "dac960", "Mylex DAC960", "dr", &dr_controllers },
    { TYPE_EISA, 0x10116126, 0x00000000, "dq", "ISA IDE", "dq", &dq_controllers },
    { TYPE_EISA, 0x2042a310, 0x00000000, "er", "DE422", "er", &er_controllers },
    { TYPE_EISA, 0x5042a310, 0x00000000, "ew", "DE425", "ew", &ew_controllers },
    { TYPE_EISA, 0x0030a310, 0x00000000, "fr", "DEFEA", "fr", &fr_controllers },
    { TYPE_EISA, 0x00009004, 0x00000000, "pkh", "Adaptec 1740", "pk", &pk_controllers },
    { TYPE_EISA, 0x002ea310, 0x00000000, "pua", "KFESA", "pu", &pu_controllers },
    { TYPE_EISA, 0x1030110e, 0x00000000, "vga", "Compaq Qvision", "vg", &vg_controllers },
    { TYPE_EISA, 0x1031110e, 0x00000000, "vga", "Compaq Qvision", "vg", &vg_controllers },
    { TYPE_EISA, 0x2031110e, 0x00000000, "vga", "Compaq Qvision", "vg", &vg_controllers },
    { TYPE_EISA, 0x40800941, 0x00000000, "vga_bios", "ISA VGA", "vg", &vg_controllers },
    { TYPE_EISA, 0x0025a310, 0x00000000, "eisa_nvram", "DEC EISA NVRAM", 0, 0, },
    { TYPE_EISA, 0x00604f42, 0x00000000, "dw", "PRO6000", "dw", &dw_controllers },
    { TYPE_EISA, 0x00000001, 0x00000000, "DE200-LE", "DEC LEMAC", "en", &en_controllers },
#endif
#if PROBE_PCI
    { TYPE_PCI, 0x050810da, 0x00000000, "dw", "PBXNP-AA Token Ring", "dw", &dw_controllers },
    { TYPE_PCI, 0x815410ef, 0x00000000, "dw", "SN-PBXNP-AC Token Ring", "dw", &dw_controllers },
    { TYPE_PCI, 0x000f1011, 0x00000000, "fw", "DEC PCI FDDI", "fw", &fw_controllers },
    { TYPE_PCI, 0x00181011, 0x00000000, "mc", "DEC PCI MC", "mc", &mc_controllers },
    { TYPE_PCI, 0x00011000, 0x00000000, "n810", "NCR 53C810", "pk", &pk_controllers },
    { TYPE_PCI, 0x00031000, 0x00000000, "n810", "NCR 53C825", "pk", &pk_controllers },
    { TYPE_PCI, 0x80031000, 0x00000000, "n825_dssi", "DEC KFPSA", "pu", &pu_controllers },
    { TYPE_PCI, 0x000f1000, 0x00000000, "n810", "NCR 53C875", "pk", &pk_controllers },
    { TYPE_PCI, 0x000c1000, 0x00000000, "n810", "NCR 53C895", "pk", &pk_controllers },
    { TYPE_PCI, 0x00121000, 0x00000000, "n810", "NCR 53C895A", "pk", &pk_controllers },
    { TYPE_PCI, 0x000b1000, 0x00000000, "n810", "NCR 53C896", "pk", &pk_controllers },
    { TYPE_PCI, 0x00201000, 0x00000000, "n810", "SYM 53C1010", "pk", &pk_controllers },
    { TYPE_PCI, 0x04828086, 0x00000000, "pci_eisa", "Intel 82375", 0, &isa_eisa_count },
    { TYPE_PCI, 0x04848086, 0x00000000, "pci_isa", "Intel 82378", 0, &isa_eisa_count },
    { TYPE_PCI, 0x12298086, 0x00000000, "ei", "Intel 8255x Ethernet", "ei", &ei_controllers },
    { TYPE_PCI, 0x12298086, 0xb1440e11, "ei", "DE600-AA", "ei", &ei_controllers },
    { TYPE_PCI, 0x12298086, 0xb0e10e11, "ei", "DE602-FA", "ei", &ei_controllers },
    { TYPE_PCI, 0x12298086, 0xb0dd0e11, "ei", "DE602-AA", "ei", &ei_controllers },
    { TYPE_PCI, 0x12298086, 0xb0de0e11, "ei", "DE602-TA", "ei", &ei_controllers },
    { TYPE_PCI, 0xc6931080, 0x00000000, "pci_isa", "Cypress 82C693", 0, &isa_eisa_count },
    { TYPE_PCI, 0xc6931080, 0x00000101, "dq", "Cypress 82C693 IDE", "dq", &dq_controllers },
#if 0                                                
    { TYPE_PCI, 0x09011000, 0x00000000, "usb", "USB", 0, &usb_controllers },
#endif                                               
    { TYPE_PCI, 0xc6931080, 0x00000C03, "usb", "Cypress 82C693 USB", 0, &usb_controllers },
    { TYPE_PCI, 0x153310b9, 0x00000000, "pci_isa", "Acer Labs M1543C", 0, &isa_eisa_count },
    { TYPE_PCI, 0x522910b9, 0x00000000, "dq", "Acer Labs M1543C IDE", "dq", &dq_controllers },
    { TYPE_PCI, 0x523710b9, 0x00000000, "usb", "Acer Labs M1543C USB", 0, &usb_controllers },
    { TYPE_PCI, 0x710110b9, 0x00000000, "acer_pmu", "Acer Labs M1543C PMU", 0, 0 },
    { TYPE_PCI, 0x00011011, 0x00000000, 0, "DECchip 21050-AA", 0, 0 },
    { TYPE_PCI, 0x00211011, 0x00000000, 0, "DECchip 21052-AA", 0, 0 },
    { TYPE_PCI, 0x00241011, 0x00000000, 0, "DECchip 21152-AA", 0, 0 },
    { TYPE_PCI, 0x00251011, 0x00000000, 0, "DECchip 21153-AA", 0, 0 },
    { TYPE_PCI, 0x00261011, 0x00000000, 0, "DECchip 21154-AA", 0, 0 },
    { TYPE_PCI, 0x00461011, 0x00000000, 0, "DECchip 21554-AA", 0, 0 },
    { TYPE_PCI, 0x00021011, 0x00000000, "ew", "DECchip 21040-AA", "ew", &ew_controllers },
    { TYPE_PCI, 0x00141011, 0x00000000, "ew", "DECchip 21041-AA", "ew", &ew_controllers },
    { TYPE_PCI, 0x00091011, 0x00000000, "ew", "DECchip 21140-AA", "ew", &ew_controllers },
    { TYPE_PCI, 0x00191011, 0x00000000, "ew", "DECchip 21143-AA", "ew", &ew_controllers },
    { TYPE_PCI, 0x00091011, 0x500a1011, "ew", "DE500-AA Network Controller", "ew", &ew_controllers },
    { TYPE_PCI, 0x00191011, 0x500b1011, "ew", "DE500-BA Network Controller", "ew", &ew_controllers },
    { TYPE_PCI, 0x00191011, 0x500f1011, "ew", "DE500-FA Network Controller", "ew", &ew_controllers },
    { TYPE_PCI, 0x00eb1091, 0x00000000, "vga", "Cateyes - 4D51T Graphics Controller","vg", &vg_controllers },
    { TYPE_PCI, 0x00041011, 0x00000000, "tga", "DECchip ZLXp 21030", "tg", &tg_controllers },
    { TYPE_PCI, 0x00093d3d, 0x010013e9, "vga_bios", "Permedia - P2V Graphics Controller","vg", &vg_controllers },
    { TYPE_PCI, 0x00093d3d, 0x000213e9, "vga_bios", "Permedia - P2V Graphics Controller","vg", &vg_controllers },
    { TYPE_PCI, 0x000d1011, 0x00000000, "vga_bios", "DEC PowerStorm", "vg", &vg_controllers },
    { TYPE_PCI, 0x3d07104c, 0x0a321048, "vga_bios", "ELSA GLoria Synergy", "vg", &vg_controllers },
    { TYPE_PCI, 0x3d07104c, 0x4d101011, "vga_bios", "PowerStorm 4D10", "vg", &vg_controllers },
    { TYPE_PCI, 0x030410ba, 0x4d310e11, "vga_bios", "PowerStorm 300", "vg", &vg_controllers },
    { TYPE_PCI, 0x030410ba, 0x4d351011, "vga_bios", "PowerStorm 350", "vg", &vg_controllers },
    { TYPE_PCI, 0x00101011, 0x00000000, "vip7407", "DECchip 7407", 0, 0 },
    { TYPE_PCI, 0x10201077, 0x00000000, "isp1020", "QLogic ISP10x0", "pk", &pk_controllers },
    { TYPE_PCI, 0x00081011, 0x00000000, "pks", "DEC KZPSA", "pk", &pk_controllers },
    { TYPE_PCI, 0x11001013, 0x00000000, 0, "Cirrus PD6729", 0, 0 },
    { TYPE_PCI, 0x740211a8, 0x00000000, 0, "Systech EtPlex 7402", 0, 0 },
    { TYPE_PCI, 0x740811a8, 0x00000000, 0, "Systech EtPlex 7408", 0, 0 },
    { TYPE_PCI, 0x730211a8, 0x00000000, 0, "Systech EtPlex 7302", 0, 0 },
    { TYPE_PCI, 0x730811a8, 0x00000000, 0, "Systech EtPlex 7308", 0, 0 },
    { TYPE_PCI, 0x00071011, 0x00000000, "pci_nvram","DEC PCI NVRAM", 0, 0 },
    { TYPE_PCI, 0x0518102b, 0x00000000, "vga", "Compaq 2000/P", "vg", &vg_controllers },
    { TYPE_PCI, 0x30320e11, 0x00000000, "vga", "Compaq 1280/P", "vg", &vg_controllers },
    { TYPE_PCI, 0x88115333, 0x00000000, "vga_bios", "S3 Trio64/Trio32", "vg", &vg_controllers },
    { TYPE_PCI, 0x88c05333, 0x00000000, "vga_bios", "S3 Trio864", "vg", &vg_controllers },
    { TYPE_PCI, 0x88f05333, 0x00000000, "vga_bios", "Digital 2T-PMPAA/Pixelwork", "vg", &vg_controllers },
    { TYPE_PCI, 0x00a01013, 0x00000000, "vga_bios", "Cirrus CL-GD5430", "vg", &vg_controllers },
    { TYPE_PCI, 0x00a81013, 0x00000000, "vga_bios", "Cirrus CL-GD5434", "vg", &vg_controllers },
    { TYPE_PCI, 0x41581002, 0x00000000, "vga_bios", "ATI Mach32", "vg", &vg_controllers },
    { TYPE_PCI, 0x43541002, 0x00000000, "vga_bios", "ATI Mach64", "vg", &vg_controllers },
    { TYPE_PCI, 0x00011069, 0x00000000, "dac960", "Mylex DAC960", "dr", &dr_controllers },
    { TYPE_PCI, 0x00021069, 0x00000000, "dac960", "Mylex DAC960", "dr", &dr_controllers },
    { TYPE_PCI, 0x06601095, 0x00000000, "cipca", "CIPCA", "pu", &pu_controllers },
    { TYPE_PCI, 0x1ae510df, 0x00000000, 0, "KGPSA-A", 0, 0 },
    { TYPE_PCI, 0xf70010df, 0x00000000, "kgpsa", "KGPSA-B", "pg", &pg_controllers },
    { TYPE_PCI, 0xf80010df, 0x00000000, "kgpsa", "KGPSA-C", "pg", &pg_controllers },
    { TYPE_PCI, 0x21001077, 0x00000000, 0, "KGPBA", 0, 0 },
    { TYPE_PCI, 0x00161011, 0x00000000, 0, "Digital ATMworks 350", 0, 0 },
    { TYPE_PCI, 0x0020102f, 0x00581011, 0, "Digital ATMworks 351", 0, 0 },
    { TYPE_PCI, 0x0020102f, 0x00781011, 0, "Digital ATMworks 351", 0, 0 },
    { TYPE_PCI, 0x04001127, 0x04001127, 0, "Fore ATM 155/622 Adapter", 0, 0 },
    { TYPE_PCI, 0x04001127, 0x01000E11, 0, "DAPBA-FA ATM155 MMF", 0, 0 },
    { TYPE_PCI, 0x04001127, 0x02000E11, 0, "DAPBA-UA ATM155 UTP", 0, 0 },
    { TYPE_PCI, 0x04001127, 0x03000E11, 0, "DAPCA-FA ATM622 MMF", 0, 0 },
    { TYPE_PCI, 0x0020102f, 0x00000000, 0, "Toshiba Meteor", 0, 0 },
    { TYPE_PCI, 0x00221014, 0x00000000, 0, "BIT 3/IBM PCI-to-PCI Bridge", 0, 0 },
    { TYPE_PCI, 0x000e1011, 0x00000000, 0, "AV321", 0, 0 },
    { TYPE_PCI, 0x00131011, 0x00000000, 0, "AV301", 0, 0 },
    { TYPE_PCI, 0x0350109e, 0x00000000, 0, "AV901", 0, 0 },
    { TYPE_PCI, 0x000112ae, 0x600a1011, 0, "DEGPA-SA", "ew", &ew_controllers },
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
    { TYPE_PCI, 0x90b010b5, 0x132610df, 0, "PDXDP-AA", 0, 0 },
    { TYPE_PCI, 0x90b010b5, 0x115510df, 0, "PDXDP-AB", 0, 0 },
    { TYPE_PCI, 0x90b010b5, 0x115610df, 0, "PDXDP-AC", 0, 0 },
    { TYPE_PCI, 0x5001112f, 0x00000000, 0, "Arnet Sync 570 IP", 0, 0 },
    { TYPE_PCI, 0x00171011, 0x00000000, 0, "DEC PV-PCI Graphics", 0, 0 },
    { TYPE_PCI, 0x06461095, 0x00000000, "dq", "CMD PCI0646 IDE", "dq", &dq_controllers },
    { TYPE_PCI, 0xac12104c, 0x00000000, 0, "PCI to CardBus", 0, 0 },
    { TYPE_PCI, 0xac13104c, 0x00000000, 0, "PCI to PC Card", 0, 0 }, 
    { TYPE_PCI, 0x81789004, 0x00000000, "aic78xx", "Adaptec AIC-7887", "pk", &pk_controllers },
    { TYPE_PCI, 0x78959004, 0x00000000, "aic78xx", "Adaptec AIC-7895", "pk", &pk_controllers },
    { TYPE_PCI, 0x00c09005, 0x00000000, "aic78xx", "Adaptec AIC-7899", "pk", &pk_controllers },
    { TYPE_PCI, 0x001E1011, 0x00000000, "rtc", "DEC RT clock", "rt", &rt_controllers },
    { TYPE_PCI, 0x000010e3, 0x00000000, 0, "Tundra Universe II", 0, 0},
    { TYPE_PCI, 0x000014fc, 0x00000000, 0, "QSW ELan3", 0, 0},
    { TYPE_PCI, 0x0e11a0f7, 0x00000000, 0, "Yukon PCI Hot-Plug Controller", 0, 0},
#endif
    { 0, 0, 0, 0, 0, 0 }
    };         

struct io_device i2o_device = 
    { TYPE_PCI, 0, 0, "pz", "I2O", "pz", &i2o_controllers };

struct io_device_name io_name[] = {
    { 0x19608086, 0x03a2113c, "AMI 431" },
    { 0, 0, 0 }
    };

#endif                                              

struct io_device *find_io_device( int type, unsigned int id, unsigned int id_ext )
    {
    struct io_device *iod;

    for( iod = io_device; iod->type && iod->id; iod++ )
	if( ( iod->type == type ) && ( iod->id == id ) && ( iod->id_ext == id_ext ) )
	    return( iod );
    return( 0 );
    }

#if PROBE_PCI
probe_pci( int hose )
    {
    char *name;
    char *protocol;
    int *controller;
    int device;
    int i;
    unsigned int id;
    unsigned int rom_bar;
    struct io_device *iod;
    struct pci_size_info top_psi;
    struct pci_size_info *child_psi;
    struct pb top_pb;
    struct pb *pb;

    /*
     *  Set up a top-level PSI to represent the PCI hose, and make that
     *  PSI also be the current PSI.
     */
    parent_psi = &top_psi;
    current_psi = &top_psi;
    memset( &top_psi, 0, sizeof( top_psi ) );
    top_psi.pb = &top_pb;
    memset( &top_pb, 0, sizeof( top_pb ) );
    top_pb.hose = hose;
    /*
     *  Initialize PCI base and size.  Platforms can override this in the
     *  probe_pci_setup call.
     */
    memset( pci_base, 0, sizeof( pci_base ) );
    memset( pci_size, -1, sizeof( pci_size ) );
    pci_has_eisa_isa = 0;
    pci_bus = 0;
    probe_pci_setup( hose );
#if DEBUG_PROBE
    err_printf( "probing hose %d, PCI\n", hose );
#endif
    /*
     *  Probe the top-level PCI bus (bus 0).  PCI buses behind bridges will
     *  also be probed with probe_pci_bus, recursively.
     */

#if UNIVERSE
total_dense =0;
#endif
    probe_pci_bus( hose, PCI_MIN_SLOT, PCI_MAX_SLOT );
    
    /*
     * Now that the whole bus has been probed check to see if there is
     * any dense space left to increase the size of the 640MB window on the
     * Universe II
     */
#if UNIVERSE
        #if DEBUG
                pprintf ("\n Amount of Dense space left for Universe: %x\n", pci_size[SIZE_GRAN_BASE_DENSE] - total_dense);
                pprintf ("Universe GRAN_BASE_DENSE: %x\n", universe_640_psi->size[SIZE_GRAN_BASE_DENSE]);
                pprintf ("PCI SIZE: %x\n", pci_size[SIZE_GRAN_BASE_DENSE]);
                pprintf ("Total Dense Space: %x\n", total_dense);
        #endif


        if( (pci_size[ SIZE_GRAN_BASE_DENSE] - total_dense) > 0x04000000 )
        while((universe_640_psi->size[SIZE_GRAN_BASE_DENSE] < 0x3c000000) && (total_dense<pci_size[SIZE_GRAN_BASE_DENSE]))
        {
                universe_640_psi->size[SIZE_GRAN_BASE_DENSE] += 0x04000000;
                total_dense += 0x04000000;
        }

        #if DEBUG
        pprintf("Total dense is: %x \n", total_dense);
        pprintf("Total dense space for Universe 2 after allocation is: %x\n\n", universe_640_psi->size[SIZE_GRAN_BASE_DENSE]);
        # endif
#endif

    child_psi = top_psi.child_psi;
    if( child_psi )
	{
	/*
	 *  The PCI hose has been probed.  Information needed to assign PCI
	 *  address space has been gathered and saved in a tree of PSIs.  Now
	 *  this information must be propogated from the bottom up, through
	 *  any PCI-to-PCI bridge PSIs.  The top-level PSI will have no peers,
	 *  so start with its first child.
	 */
	probe_propogate_pci_bus( child_psi );
	/*
	 *  All address space information has been propogated throughout the
	 *  tree of PSIs.  Now use the PSI tree to actually configure the PCI
	 *  hose.  The top-level PSI will have no peers, so start with its
	 *  first child.
	 */
	probe_configure_pci_bus( child_psi );
	}
    /*
     *  Give platforms a chance to clean up any messes that we've left
     *  before we actually do the PCI programming.
     */
    probe_pci_cleanup( hose );
    /*
     *  PCI address space has been assigned.  Now call get_pci_csr_vector
     *  for each PB which was build for this PCI hose, so that the actual
     *  programming of PCI configuration space can take place.
     */
    krn$_wait( &pbs_lock );
    for( i = 0; i < num_pbs; i++ )
	{
	pb = pbs[i];
	if( ( pb->hose == hose ) && ( pb->type == TYPE_PCI ) )
	    {
	    get_pci_csr_vector( pb );
	    /*
	     *  Special-case the NCR 53C825, to see whether this is a
	     *  SCSI interface or a DSSI interface.
	     */
	    id = incfgl( pb, 0x00 );
	    if( id == 0x00031000 )
		{
		rom_bar = incfgl( pb, 0x30 );
		outcfgl( pb, 0x30, rom_bar | 1 );
		if( inmeml( pb, rom_bar ) == 'DSSI' )
		    {
		    iod = find_io_device( TYPE_PCI, id + 0x80000000, 0 );
		    if( iod )
			{
#if DRIVERSHUT
			pb->tbl = iod;
#endif
			name = iod->name;
			device = iod->device;
			protocol = iod->protocol;
			controller = iod->controller;
			if( protocol && device )
			    {
			    pb->protocol = protocol;
			    pb->device = device;
			    pb->letter = controller;
			    pb->hardware_name = name;
			    }
			}
		    }
		outcfgl( pb, 0x30, rom_bar );
		}
	    }
	}
    krn$_post( &pbs_lock );
    /*
     *  Now assign controller letters for those devices just probed.
     */
    probe_assign_letters( hose );

    fru5_pci_vpd( hose );
    }

probe_pci_bus( int hose, int min_slot, int max_slot )
    {
    char *name;
    char *protocol;
    int *controller;
    int device;
    int bus;
    int slot;
    int function;
    int max_function;
    unsigned int class;
    int id;
    int sub_id;
    struct io_device *iod;
    struct pb *pb;

    /*
     *  Check for PCI devices which have a bug which causes them not to
     *  distinguish between type 0 and type 1 configuration cycles.
     */
    if( !pci_bus )
	{
	pci_type0_type1_bug = 0;
	for( slot = min_slot; slot <= max_slot; slot++ )
	    {
	    if( ( slot >= 5 ) && ( slot <= 12 ) )
		{
		id = read_pci_id( hose, pci_bus, slot, 0 );
		if( id )
		    {
		    bus = 1 << ( slot - 5 );
		    if( id == read_pci_id( hose, bus, 0, 0 ) )
			{
			pci_type0_type1_bug |= bus;
			err_printf( "*** PCI device incorrectly decodes Type 1 configuration cycles\n" );
			err_printf( "*** bus %d, slot %d, may interfere with PCI bus %d\n",
				pci_bus, slot, bus );
			}
		    }
		}
	    }
	}
    /*
     *  Remember what PCI bus we are starting with.
     */
    bus = pci_bus;
    for( slot = min_slot; slot <= max_slot; slot++ )
	{
	/*
	 *  If we are on the top-level bus, then remember our PCI slot, for
	 *  use by the code which handles PCI-to-PCI bridges (devices behind
	 *  a bridge need to know the parent slot on the top-level bus).
	 */
	if( !bus )
	    pci_slot = slot;
	/*
	 *  Start off assuming that this is not a multi-function capable
	 *  PCI device.  If it turns out to be so, max_function will be
	 *  adjusted accordingly.
	 */
	max_function = 0;
	for( function = 0; function <= max_function; function++ )
	    {
	    /*
	     *  Remember the order that we're probing this PCI slot in.
	     *  We need to do this so that the PB list remains properly
	     *  ordered, so that in the future, we walk the PB list
	     *  correctly, visiting PCI-to-PCI bridges before we visit
	     *  any devices behind those PCI-to-PCI bridges.
	     */
	    probe_number++;
	    /*
	     *  Read the PCI ID for the current PCI device.  The call
	     *  will return 0 if there is no PCI device present.
	     */
	    id = read_pci_id( hose, bus, slot, function );
	    if( id )
		{
#if DEBUG
		err_printf( "bus %d, slot %d", bus, slot );
		if( function )
		    err_printf( ", function %d", function );
		err_printf( " -- %08X\n", id );
#endif
		/*
		 *  We've found another PCI device, so build a PB for it.
		 */
		pb = malloc_noown( sizeof( *pb ) );
		pb->type = TYPE_PCI;
		pb->hose = hose;
		pb->bus = bus;
		pb->slot = slot;
		pb->function = function;
		pb->order = probe_number;
		if( bus )
		    pb->parent = parent_psi->pb;
		insert_pb( pb );
		/*
		 *  Do the right thing if this PCI device is multi-function
		 *  capable.
		 */
		if( function == 0 )
		    if( incfgb( pb, 0x0e ) & 0x80 )
			max_function = 7;
		/*
		 *  Search the I/O device table for this PCI device.  If we
		 *  find it, then the console knows how to handle it.
		 */
		iod = 0;
		class = ( unsigned int )incfgw( pb, 0x0a );
		sub_id = incfgl( pb, 0x2c );
		if( BASE_CLASS( class ) == I2O_CLASS )
		    iod = &i2o_device;
		if( ( iod == 0 ) && ( class != 0 ) )
		    iod = find_io_device( TYPE_PCI, id, class );
		if( ( iod == 0 ) && ( sub_id != 0 ) )
		    iod = find_io_device( TYPE_PCI, id, sub_id );
		if( iod == 0 )
		    iod = find_io_device( TYPE_PCI, id, 0 );
		if( iod )
		    {
#if DRIVERSHUT
		    pb->tbl = iod;
#endif
		    name = iod->name;
		    if( BASE_CLASS( class ) == I2O_CLASS )
			find_io_name( id, sub_id, &name );
		    device = iod->device;
		    protocol = iod->protocol;
		    controller = iod->controller;
		    if( device )
			{
			pb->protocol = protocol;
			pb->device = device;
			pb->letter = controller;
			}
		    }
		else
		    {
		    name = pb->id;
		    if( sub_id )
			sprintf( name, "%08X/%08X", id, sub_id );
		    else
			sprintf( name, "%08X", id );
		    }
		pb->hardware_name = name;
		/*
		 *  First handle this as a PCI device.  Then, if this
		 *  is a PCI-to-PCI bridge, handle it as a PCI bridge.
		 */
		handle_pci_device( pb );

		if( class == 0x0607 )
		    handle_cardbus_bridge(pb);

		if( class == 0x0604 )
		    {
#if MEDULLA
		    med_bridge = 1;
#endif
		    handle_pci_bridge( pb );
#if MEDULLA
		    med_bridge = 0;
#endif
		    }
#if PROBE_EISA
		if( ( class == 0x0602 ) || ( id == 0x04828086 ) )
		    if( bus_probe_algorithm )
			handle_pci_eisa_bridge( pb );
		    else
			sprintf( pb->info, "Bridge to Hose 1, EISA" );
#endif
#if PROBE_ISA
		if( ( class == 0x0601 ) || ( id == 0x04848086 ) )
		    if( bus_probe_algorithm )
			handle_pci_isa_bridge( pb );
		    else
			sprintf( pb->info, "Bridge to Hose 1, ISA" );
#endif
/*
 * Special case handling for PCI IDE controllers.  
 *
 * Separate port blocks need to be established for the primary and secondary 
 * IDE interfaces.  The exception to this rule is the embedded IDE controller 
 * in the Cypress CY82C693.  Because Cypress implements the primary and 
 * secondary IDE interfaces as separate PCI functions, separate port blocks 
 * for each function will be established by default.
 *
 * The following code establishes a port block for the secondary (that is,
 * pb->channel = 1) interface.
 */
		if( ( iod ) && ( class == 0x0101 ) )
		    {
		    if( id == 0xC6931080 )
			{
		    	if( function == 2 )
			    pb->channel = 1;
			}
		    else
			{
			probe_number++;
			pb = malloc_noown( sizeof( *pb ) );
			pb->type = TYPE_PCI;
			pb->hose = hose;
			pb->bus = bus;
			pb->slot = slot;
			pb->function = function;
			pb->order = probe_number;
			pb->channel = 1;
			if( bus )
			    pb->parent = parent_psi->pb;
			insert_pb( pb );
#if DRIVERSHUT
			pb->tbl = iod;
#endif
			if( iod->device )
			    {
			    pb->protocol = iod->protocol;
			    pb->device = iod->device;
			    pb->letter = iod->controller;
			    }
			pb->hardware_name = iod->name;
			handle_pci_device( pb );
			}
		    }
		}
	    }
	}
    }

#if PROBE_EISA
handle_pci_eisa_bridge( struct pb *pb )
    {
#if SABLE
    pci_bus = 1;
#endif
#if DEBUG_PROBE
    err_printf( "probing PCI-to-EISA bridge, bus 1\n" );
#endif

    sprintf( pb->info, "Bridge to Bus 1, EISA" );
    probe_eisa_bus( pb->hose, 1 );
    }
#endif

#if PROBE_ISA
handle_pci_isa_bridge( struct pb *pb )
    {
#if DEBUG_PROBE
    err_printf( "probing PCI-to-ISA bridge, bus 1\n" );
#endif
    sprintf( pb->info, "Bridge to Bus 1, ISA" );
    probe_isa_bus( pb->hose, 1 );
    }
#endif

unsigned int pci_bridge_gran[SIZE_GRAN_BASE_NUM] =
	{ 4*1024, 4*1024, 1024*1024, 1024*1024 };

handle_pci_bridge( struct pb *pb )
    {
    struct pci_device_misc *pdm;
    struct pci_bridge_misc *pbm;
    struct pci_size_info *psi;

    /*
     *  Build a PBM to describe this bridge.  Increment the PCI bus number.
     *  The primary bus number is the current PCI bus number, and the
     *  secondary bus number is the just-incremented PCI bus number.  The
     *  subordinate bus number is not known until we probe through this
     *  PCI-to-PCI bridge (there may be bridges behind this bridge), so set
     *  it to 255 for the time being.
     */
    pdm = pb->pdm;
    pbm = malloc_noown( sizeof( *pbm ) );
    pdm->pbm = pbm;
#if !SABLE && !YUKON && !CORTEX && !UNIVERSE
    /*
     *  Reserve bus 1 on all PCI hoses for EISA/ISA.
     *
     *  This won't affect any platform which has only one PCI hose, and which
     *  has the EISA/ISA bridge located before any slots.  (This is all current
     *  platforms except for the Spanky variant of Sable, so exempt Sable from
     *  this change.)
     *
     *  This affects all AlphaVME SBC's when an I/O option card is installed, 
     *  so they will be exempt from this change as well.
     */
    if( pci_bus == 0 )
	pci_bus = 1;
#endif 
    while( ++pci_bus & pci_type0_type1_bug )
	err_printf( "*** skipping PCI bus %d due to detected interference\n", pci_bus );
    pbm->pri_bus = pb->bus;
    pbm->sec_bus = pci_bus;
    outcfgb( pb, 0x18, pbm->pri_bus );
    outcfgb( pb, 0x19, pbm->sec_bus );
    outcfgb( pb, 0x1a, 0xff );
#if DEBUG_PROBE
    err_printf( "probing PCI-to-PCI bridge, bus %d\n", pci_bus );
#endif

    sprintf( pb->info, "Bridge to Bus %d, PCI", pci_bus );
    /*
     *  Build a PSI to describe this bridge.  Insert the PSI into the tree.
     *  (If our parent already has a child, then we are a peer of that child,
     *  else we are our parent's child.)  Since PCI-to-PCI bridges cause a
     *  new level in the tree, set the new PSI as the current and parent PSI.
     *
     *  Bridges have a minimum granularity irrespective of the granularity
     *  of any devices behind them.  Take this into account.
     */
    psi = malloc( sizeof( *psi ) );
    if( parent_psi->child_psi )
	current_psi->peer_psi = psi;
    else
	current_psi->child_psi = psi;
    psi->parent_psi = parent_psi;
    psi->pb = pb;
    if( ( ( incfgb( pb, 0x1c ) & 15 ) == 0 )
	    || ( parent_psi->flags & PCI_FLAGS_IO_16_BIT ) )
	psi->flags = PCI_FLAGS_PCI_BRIDGE | PCI_FLAGS_IO_16_BIT;
    else
	psi->flags = PCI_FLAGS_PCI_BRIDGE;
    memcpy( psi->gran, pci_bridge_gran, sizeof( pci_bridge_gran ) );
    current_psi = psi;
    parent_psi = psi;
    /*
     *  Probe behind this PCI-to-PCI bridge.
     */
    probe_pci_bus( pb->hose, 0, 31 );
    /*
     *  Restore the current and parent PSIs; these will have been modified
     *  while probing behind this bridge, and they need to be set back to
     *  continue probing on this bus.
     */
    current_psi = psi;
    parent_psi = psi->parent_psi;
    if( parent_psi->flags & PCI_FLAGS_PCI_BRIDGE )
	parent_psi->flags |= psi->flags & ( PCI_FLAGS_IO_16_BIT | PCI_FLAGS_VGA );
    /*
     *  Now we can set the proper subordinate bus number for this bridge.
     *  We know it now because we have finished probing behind this bridge.
     */
    pbm->sub_bus = pci_bus;
    outcfgb( pb, 0x1a, pbm->sub_bus );
    }

handle_pci_device( struct pb *pb )
    {
    int i;
    int j;
    int header;
    int class;
    int eisa_isa;
    int offset;
    int data;
    int io;
    int type;
    int dense;
    int size;
    int base;
    int offset_bars;
    int num_bars;
    int offset_rom_bar;
    int flags;
    unsigned int id;
    struct pci_device_misc *pdm;
    struct pci_size_info *psi;

    eisa_isa = 0;
    id = incfgl( pb, 0x00 );
    class = incfgw( pb, 0x0a );
    header = incfgb( pb, 0x0e );
    if( ( class == 0x0602 ) || ( id == 0x04828086 ) )
	eisa_isa = 1;
    if( ( class == 0x0601 ) || ( id == 0x04848086 ) )
	eisa_isa = 1;
    if( ( class == 0x0001 ) || ( class == 0x0300 ) )
	parent_psi->flags |= PCI_FLAGS_VGA | PCI_FLAGS_IO_16_BIT;
#if WFSTDIO
    if( id == 0x153310B9 )
	setup_wf_stdio( pb, 0 );
#endif
    /*
     *  Disable this device in case it was previously enabled (but
     *  leave EISA/ISA bridges alone).
     */
    if( eisa_isa )
#if !(MEDULLA || CORTEX || UNIVERSE)
	pci_has_eisa_isa = 1;
#else
	pci_has_eisa_isa = 0; /* Medulla, Yukona and Cortex have no actual isa slots*/
#endif
    else
	{
#if MEDULLA || CORTEX || UNIVERSE
	/*
	 *  Leave the VIP alone.
	 *
	 *  Since PALcode can be invoked for interrupts, the VIP
	 *  must always be accessible...
	 */
	if( id != 0x00101011 ) 
	    outcfgw( pb, 0x04, 0x0000 );
#else
	outcfgw( pb, 0x04, 0x0000 );
#endif
	}
    /*
     *  Build a PDM to describe this device.  Remember the base slot in case
     *  this device is behind a PCI-to-PCI bridge (the base slot is used to
     *  find the correct interrupt vector information later).
     */
    pdm = malloc_noown( sizeof( *pdm ) );
    pb->pdm = pdm;
    pdm->base_slot = pci_slot;
    /*
     *  Figure out what header type we're dealing with.
     */
    switch( header & 0x7f )
	{
	case 0:
	    offset_bars = 0x10;
	    num_bars = 6;
	    offset_rom_bar = 0x30;
	    break;

	case 1:
	    offset_bars = 0x10;
	    num_bars = 2;
	    offset_rom_bar = 0x38;
	    break;

	case 2:
	    offset_bars = 0x10;
	    num_bars = 1;
	    offset_rom_bar = 0x0;
	    break;

	default:
	    return;
	}
    /*
     *  Handle the header types that we know.
     */
    for( i = 0; i < num_bars + 1; i++ )
	{
#if MEDULLA || CORTEX || UNIVERSE
	 /*
	  *  Skip first BAR of VIP so not to destroy its previous initialization.
	  */
	if( id == 0x00101011 )
	    if( i == 0 )
		continue;
#if !CORTEX && !UNIVERSE
	/*
	 *  Skip all BARs of PCI-PCMICA IDE controller.  Device not enabled on
	 *  Ramix PMC expansion module.
	 */
	if( ( id == 0x12228086 ) && ( med_bridge == 1 ) )
	    if( i < 4 )
		continue;
#endif
#endif
	/*
	 *  The Base Addresses Registers are in a contiguous space starting
	 *  at offset offset_bars.  Figure out what to do.
	 */
	if( i < num_bars )
	    {
	    flags = 0;
	    offset = offset_bars + i * 4;
	    outcfgl( pb, offset, 0xffffffff );
	    data = incfgl( pb, offset );
	    /*
	     *  The following hack compensates for a completely bogus bug
	     *  which causes a BAR to read back the same value as the device's
	     *  ID.  The Emulex synchronous communications cards do this for
	     *  two of their BARs.
	     */
	    if( id == (u_int)0x906d10b5 )
		if( data == id )
		    data = 0;
	    /*
	     *  The following hack compensates for a TGA bug.  The TGA asks
	     *  for 32MB, but really needs 128MB, in dense space.
	     */
	    if( id == 0x00041011 )
		if( incfgb( pb, 0x08 ) < 3 )
		    if( i == 0 )
			data = 0xf8000008;

	    /*	 
	     *  Compensate for the Pixel works module.  It claims 32mb but
	     *	really needs 64mb.
	     */
	    if( id == (u_int)0x88f05333 )
		if( i == 0 )
		    data &= 0xfc000008;

	    /*
	     * For DPT's Domino, ignore the very large request by the second 
	     * BAR. The DPT design allows the mapping of each Domino in a 
	     * system into the same address space. This space is mapped, as a 
	     * hack, during PCI space allocation.
	     */

	    if ( id == 0x10121044 )
		if ( i == 1 )
		    data = 0;

	    /*
	     *  For I2O devices ignore the prefetchable bit and map
	     *  into sparse memory.
	     */
	    if( BASE_CLASS( class ) == I2O_CLASS )
		data &= ~8;
	    /*	  
	     *  Map Intel 82558 fast ethernet devices into sparse
	     *  space.
	     */
	    if ( id == 0x12298086 )
		data &= ~8;

	    /*
	     *  Force the Cypress IDE controllers (primary and secondary) to
	     *  legacy addresses.
	     */
	    if( id == (u_int)0xc6931080 )
		{
		if( pb->function == 1 )
		    {
		    if( i == 0 )
			{
			pdm->base[0] = 0x1f0;
			data = 0;
			}
		    if( i == 1 )
			{
			pdm->base[1] = 0x3f6;
			data = 0;
			}
		    }
		if( pb->function == 2 )
		    {
		    if( i == 0 )
			{
			pdm->base[0] = 0x170;
			data = 0;
			}
		    if( i == 1 )
			{
			pdm->base[1] = 0x376;
			data = 0;
			}
		    }
		}
#if MEDULLA
	    /* 
	     *  This "fix" is for the PCI->PCMCIA bus adpater (82092).
	     *  Because PCMCIA devices are "hot swappable" at the OS level the 
	     *	address space required by the device is not programmed.  This
	     *  is not good for PPB cases.  Let the user select it.
	     *  64MB is the maximum.
	     */
	    if( ( id == 0x12218086 ) && ( med_bridge == 1 ) )
		if( i == 1 )
		    data = set_pcmcia_mask( );
#endif
	    /*
	     *  Set the VIP's prefetchable bit so that the large VME window
	     *  goes into dense space.
	     */
	    if( id == 0x00101011 )
		if( i == 1 )
		    data |= 1 << 3;
#if UNIVERSE
             /*
              * The Tundra Universe needs two windows of PCI dense space for
              * PCI to VME transactions. It does not use BARS for these two windows,
              * it uses registers in the CSR space.  Allocate one 64MB window
              * and one 640 - 1 GB window and create PSI's for these windows.
              * BARs 3 - 6 are not used by Tundra Universe II so use 3 & 4 to
              * allocate windows.
              */
        if( id == 0x000010E3 )
                {
                  if( i == 2 )
                        data = 0xfc000008; /*  64MB dense */
                  if( i == 3)
                        data = 0xd8000008; /*  640 MB dense  */

                }
#endif
     	    
	    io = data & 1;
	    if( io )
		{
		if( ( data & 0xffff8000 ) == 0x00008000 )
		    {
		    data |= 0xffff0000;
		    flags = PCI_FLAGS_IO_16_BIT;
		    if( parent_psi->flags & PCI_FLAGS_PCI_BRIDGE )
			parent_psi->flags |= PCI_FLAGS_IO_16_BIT;
		    }
		type = 0;
		dense = 0;
		size = -( data & ~3 );
		}
	    else
		{
		type = ( data >> 1 ) & 3;
		dense = ( data >> 3 ) & 1;
		size = -( data & ~15 );
		}
	    }
	/*
	 *  The last to consider is the expansion ROM Base Address Register,
	 *  which is at offset offset_rom_bar.  Figure out what to do.
	 */
	else if( offset_rom_bar )		/* add this for cardbus */
	    {
	    offset = offset_rom_bar;
	    outcfgl( pb, offset, 0xfffffffe );
	    data = incfgl( pb, offset );
	    /*
	     *  If this is a TGA2 and it is not behind a bridge, and its VGA
	     *  is enabled, then program the BIOS ROM on a 32MB boundary.
	     */ 
	    if( id == 0x000d1011 )
		{
		if( pb->parent )
		    data = 0x00000000;
		else
		    {
		    if( incfgb( pb, 0x0a ) == 0x00 )
			data = 0xfe000001;
		    else
			data = 0x00000000;
		    }
		}
	    io = 0;
	    type = 0;
	    dense = 0;
	    size = -( data & ~2047 );
	    }
	outcfgl( pb, offset, 0x00000000 );
	/*
	 *  If this base address requires assigment, then build a PSI to
	 *  describe this base address on this device.  Insert the PSI into
	 *  the tree.  (If our parent already has a child, then we are a
	 *  peer of that child, else we are our parent's child.)  Set the
	 *  new PSI as the current PSI.
	 */
	if( size )
	    {
	    psi = malloc( sizeof( *psi ) );
	    if( parent_psi->child_psi )
		current_psi->peer_psi = psi;
	    else
		current_psi->child_psi = psi;
	    psi->parent_psi = parent_psi;
	    current_psi = psi;
	    psi->pb = pb;
	    psi->index = i;
	    psi->data = data;
	    /*
	     *  For this base address, select one of I/O space, sparse
	     *  memory space, and dense memory space.
	     */
	    if( io )
		j = SIZE_GRAN_BASE_IO;
	    else if( !dense )
		j = SIZE_GRAN_BASE_SPARSE;
	    else
		j = SIZE_GRAN_BASE_DENSE;
#if UNIVERSE
	    	total_dense += size;
                if ( ( id == 0x000010e3) && (i == 3) )
			universe_640_psi = psi;     /* save pointer to psi for 640 MB window */

#endif                  
            
	    psi->size[j] = size;
	    psi->gran[j] = size;
	    psi->flags = flags;
	    }
	/*
	 *  For 64-bit addresses, skip the next base address so that it
	 *  remains 0.  (We zero-extend rather than sign-extend when going
	 *  from 32-bit to 64-bit addresses.)
	 */
	if( type == 2 )
	    i++;
	}
    }

probe_propogate_pci_bus( struct pci_size_info *original_psi )
    {
    int i;
    int isa;
    int io_16_bit;
    unsigned int size;
    unsigned int gran;
    struct pb *pb;
    struct pci_device_misc *pdm;
    struct pci_bridge_misc *pbm;
    struct pci_size_info *psi;
    struct pci_size_info *parent_psi;

    /*
     *  Remember whether this bus is in ISA mode or not, and in 16 bit I/O
     *  mode or not.
     */
    parent_psi = original_psi->parent_psi;
    if( parent_psi->flags & PCI_FLAGS_ISA )
	isa = 1;
    else
	isa = 0;
    if( parent_psi->flags & PCI_FLAGS_IO_16_BIT )
	io_16_bit = 1;
    else
	io_16_bit = 0;
    /*
     *  Loop through all child PSIs first, setting the ISA mode flag in all
     *  PCI-to-PCI bridges which require it.
     */
    psi = original_psi;
    while( psi )
	{
	if( psi->flags & PCI_FLAGS_PCI_BRIDGE )
	    {
	    /*
	     *  Figure out if we have to run this PCI-to-PCI bridge in ISA
	     *  mode.  We do if there is a PCI-to-EISA or PCI-to-ISA bridge
	     *  somewhere on this PCI hose and this PCI-to-PCI bridge needs
	     *  to run in 16-bit mode.
	     */
	    if( pci_has_eisa_isa && ( psi->flags & PCI_FLAGS_IO_16_BIT ) )
		psi->flags |= PCI_FLAGS_ISA;
	    }
	if( psi->child_psi )
	    probe_propogate_pci_bus( psi->child_psi );
	psi = psi->peer_psi;
	}
#if DEBUG
    err_printf( "propogating PCI bus %d%s%s\n",
	    original_psi->pb->bus,
	    isa ? ", ISA mode" : "",
	    io_16_bit ? ", 16 bit I/O mode" : "" );
#endif
    /*
     *  Loop through all peer PSIs.
     */
    psi = original_psi;
    while( psi )
	{
	pb = psi->pb;
	pdm = pb->pdm;
	pbm = pdm->pbm;
	if( psi->flags & PCI_FLAGS_PCI_BRIDGE )
	    {
	    /*
	     *  Move PSI flags to the PBM.
	     */
	    pbm->flags = psi->flags;
	    /*
	     *  We must delay fixing up the bridge size until now, since
	     *  the fixup requires taking ISA mode into account, and we've
	     *  only just now figured out whether we must run in ISA mode.
	     */
	    for( i = 0; i < SIZE_GRAN_BASE_NUM; i++ )
		{
		psi->size[i] += ( pci_bridge_gran[i] - 1 );
		psi->size[i] &= ~( pci_bridge_gran[i] - 1 );
		}
	    }
	else if( io_16_bit || ( psi->flags & PCI_FLAGS_IO_16_BIT ) )
	    {
	    /*
	     *  If we're in 16 bit I/O mode, then move this device's
	     *  I/O size and granularity requirements.  And, if we're in
	     *  ISA mode, make the size and granularity sensible there.
	     */
	    size = psi->size[SIZE_GRAN_BASE_IO];
	    gran = psi->gran[SIZE_GRAN_BASE_IO];
	    if( isa && size )
		/*
		 *  If the size is too large for ISA mode, then say that
		 *  it's just not going to work.  Otherwise, increase the
		 *  size and granularity to match the ISA mode requirement.
		 */
		if( size > 256 )
		    psi->flags |= PCI_FLAGS_IMPOSSIBLE;
		else
		    {
		    size = 1024;
		    gran = 1024;
		    }
	    psi->size[SIZE_GRAN_BASE_IO_16_BIT] = size;
	    psi->gran[SIZE_GRAN_BASE_IO_16_BIT] = gran;
	    psi->size[SIZE_GRAN_BASE_IO] = 0;
	    psi->gran[SIZE_GRAN_BASE_IO] = 0;
	    }
	/*
	 *  Fix up our parent's PSI to reflect our size and granularity.
	 */
	for( i = 0; i < SIZE_GRAN_BASE_NUM; i++ )
	    {
	    size = psi->size[i];
	    gran = psi->gran[i];
	    if( psi->flags & PCI_FLAGS_IMPOSSIBLE )
		{
		size = 0;
		gran = 0;
		}
	    parent_psi->size[i] += size;
	    if( size > parent_psi->gran[i] )
		parent_psi->gran[i] = size;
#if DEBUG
	    err_printf( "device %d %d %d %d %08x %08x %d\n",
		    pb->hose, pb->bus, pb->slot, pb->function,
		    size, gran, i );
	    err_printf( "parent %d %d %d %d %08x %08x %d\n",
		    parent_psi->pb->hose, parent_psi->pb->bus,
		    parent_psi->pb->slot, parent_psi->pb->function,
		    parent_psi->size[i], parent_psi->gran[i], i );
#endif
	    }
	psi = psi->peer_psi;
	}
    }

probe_configure_pci_bus( struct pci_size_info *original_psi )
    {
    int i;
    unsigned int max_gran;
    unsigned int base;
    unsigned int size;
    unsigned int gran;
    unsigned int hole;
    unsigned int lo_base;
    unsigned int lo_base_mod;
    unsigned int lo_hole;
    unsigned int hi_base;
    unsigned int hi_base_mod;
    unsigned int hi_hole;
    unsigned int new_base;
    unsigned int saved_base[SIZE_GRAN_BASE_NUM];
    unsigned int saved_size[SIZE_GRAN_BASE_NUM];
    struct pci_size_info *psi;
    struct pci_size_info *max_psi;
    struct pb *pb;
    struct pci_bridge_misc *pbm;
    struct pci_device_misc *pdm;

#if DEBUG
    err_printf( "configuring PCI bus %d\n", original_psi->pb->bus );
#endif
    /*
     *  Assign base addresses for I/O space, sparse memory space, and dense
     *  memory space, in that order.  We do this by visiting every peer on
     *  this bus, selecting the PSI with the largest granularity, assigning
     *  a base address for that PSI, and then removing that PSI from further
     *  consideration.  We repeat this process until there are no more PSIs
     *  needing base address assignment.
     *
     *  All child PSIs will be processed AFTER all peer PSIs.
     */
    for( i = 0; i < SIZE_GRAN_BASE_NUM; i++ )
	{
	/*
	 *  Do this forever.  We will break out when we fail to select a new
	 *  PSI to process (because all PSIs have been processed).
	 */
	while( 1 )
	    {
	    /*
	     *  Find the PSI which has the largest granularity.  Remember
	     *  which PSI it was.
	     */
	    psi = original_psi;
	    max_gran = 0;
	    max_psi = 0;
	    while( psi )
		{
		if( psi->gran[i] > max_gran )
		    {
		    max_gran = psi->gran[i];
		    max_psi = psi;
		    }
		psi = psi->peer_psi;
		}
	    /*
	     *  If we got a PSI to process, do so.
	     */
	    psi = max_psi;
	    if( psi )
		{
		pb = psi->pb;
		pdm = pb->pdm;
		pbm = pdm->pbm;
		/*
		 *  Assign a base address which meets the granularity
		 *  requirements.
		 */
		size = psi->size[i];
		gran = max_gran;
		if( psi->flags & PCI_FLAGS_IMPOSSIBLE )
		    {
		    size = -1;
		    gran = 1;
		    }
		lo_base = pci_base[i];
		lo_base_mod = ( lo_base + ( gran - 1 ) ) & ~( gran - 1 );
		lo_hole = lo_base_mod - lo_base;
		hi_base = pci_base[i] + pci_size[i] - size;
		hi_base_mod = ( ( hi_base + size ) & ~( gran - 1 ) ) - size;
		hi_hole = hi_base - hi_base_mod;
		if( lo_hole > hi_hole )
		    {
#if DEBUG
		    if( hi_hole )
			err_printf( "hole left from %08x to %08x (high)\n",
				hi_base_mod + size, hi_base + size );
#endif
		    base = hi_base_mod;
		    hole = hi_hole;
		    new_base = pci_base[i];
		    }
		else
		    {
#if DEBUG
		    if( lo_hole )
			err_printf( "hole left from %08x to %08x (low)\n",
				lo_base, lo_base_mod );
#endif
		    base = lo_base_mod;
		    hole = lo_hole;
		    new_base = base + size;
		    }
		if( size + hole > pci_size[i] )
		    {
		    err_printf( "*** unable to assign PCI base address\n" );
		    err_printf( "*** bus %d, slot %d, function %d, size %08x (%s)\n",
			    pb->bus, pb->slot, pb->function,
			    psi->size[i], pci_space_strings[i] );
		    psi->size[i] = 0;
		    psi->gran[i] = 0;
		    pdm->flags |= PCI_FLAGS_NOT_ASSIGNED;
		    switch( i )
			{
			case SIZE_GRAN_BASE_IO_16_BIT:
			case SIZE_GRAN_BASE_IO:
			    pdm->flags |= PCI_FLAGS_NOT_ASSIGNED_IO;
			    break;

			case SIZE_GRAN_BASE_SPARSE:
			case SIZE_GRAN_BASE_DENSE:
			    pdm->flags |= PCI_FLAGS_NOT_ASSIGNED_MEM;
			    break;
			}
		    continue;
		    }
		/*
		 *  Modify the base and size to reflect this assignment.
		 */
#if DEBUG
		err_printf( "old pci_base[%d] = %08x, pci_size[%d] = %08x\n",
			i, pci_base[i], i, pci_size[i] );
#endif
		pci_base[i] = new_base;
		pci_size[i] -= size + hole;
#if DEBUG
		err_printf( "new pci_base[%d] = %08x, pci_size[%d] = %08x\n",
			i, pci_base[i], i, pci_size[i] );
#endif
		psi->base[i] = base;
		/*
		 *  If this is a bridge, then the size may be larger than the
		 *  granularity (but it will never be smaller); if not, then
		 *  the size is the same as the granularity.  In either case,
		 *  remember the base address (and size) assigned for this PSI.
		 */
		if( psi->flags & PCI_FLAGS_PCI_BRIDGE )
		    {
#if DEBUG
		    err_printf( "bridge %d %d %d %d %08x %08x %08x %d\n",
			    pb->hose, pb->bus, pb->slot, pb->function,
			    base, size, gran, i );
#endif
		    pbm->base[i] = base;
		    pbm->size[i] = size;
		    }
		else
		    {
#if DEBUG
		    err_printf( "device %d %d %d %d %08x %08x %08x %d\n",
			    pb->hose, pb->bus, pb->slot, pb->function,
			    base, size, gran, i );
#endif
		    pdm->base[psi->index] = base;
		    pdm->size[psi->index] = size;
		    }
		/*
		 *  Remove this PSI from further consideration.
		 */
		psi->gran[i] = 0;
		}
	    /*
	     *  If we didn't get a PSI to process, quit.
	     */
	    else
		break;
	    }
	}
    /*
     *  There may be bridges on this bus, in which case the PSIs for those
     *  bridges describe the size and granularity requirements for all
     *  buses behind those bridges.  Once a bridge has been assigned base
     *  addresses, we can then assign base addresses to the buses behind
     *  that bridge.  We do this by calling this routine recursively for
     *  every child on this bus (a child will be behind a bridge).
     */
    psi = original_psi;
    while( psi )
	{
	if( psi->child_psi )
	    {
	    /*
	     *  The appropriate base addresses to begin assigning are those
	     *  which were just assigned to this PCI-to-PCI bridge.  So save
	     *  the current set of base addresses, replace them with our own,
	     *  do the base address assignment for the bus below this bridge,
	     *  and restore the current set of base addresses.  (While the
	     *  save/restore is not strictly necessary, without it the global
	     *  pci_base cells wind up with the wrong values after base address
	     *  assignment of buses behind bridges has taken place.)
	     */
	    memcpy( saved_base, pci_base, sizeof( pci_base ) );
	    memcpy( saved_size, pci_size, sizeof( pci_size ) );
	    memcpy( pci_base,  psi->base, sizeof( pci_base ) );
	    memcpy( pci_size, psi->size, sizeof( pci_size ) );
	    probe_configure_pci_bus( psi->child_psi );
	    memcpy( pci_base, saved_base, sizeof( pci_base ) );
	    memcpy( pci_size, saved_size, sizeof( pci_size ) );
	    }
	psi = psi->peer_psi;
	}
    }

pci_add_region( unsigned int base, unsigned int size, int type )
    {
#if DEBUG
    err_printf( "region base = %x, size = %x, type = %d\n", base, size, type );
#endif
    pci_base[type] = base;
    pci_size[type] = size;
    }
#endif

dec_pci_pci_bridge( struct pb *pb )
    {
    unsigned int vendor;
    unsigned int device;

    vendor = incfgw( pb, 0x00 );
    device = incfgw( pb, 0x02 );
    if( ( vendor == 0x1011 )
	    && ( ( device == 0x0001 ) || ( device == 0x0021 ) || 
		 ( device == 0x0024 ) || ( device == 0x0025 ) || 
		 ( device == 0x0026 ) || ( device == 0x0046 ) ) )
	return( 1 );
    else
	return( 0 );
    }

#if PROBE_EISA_SLOTS
extern int eisa_ids[];
extern unsigned char *eisa_ecu_data[];
#endif

#if PROBE_EISA
probe_eisa( int hose )
    {
#if DEBUG_PROBE
    err_printf( "probing hose %d, EISA\n", hose );
#endif
    probe_eisa_bus( hose, 0 );
    probe_assign_letters( hose );
    }

probe_eisa_bus( int hose, int bus )
    {
#if PROBE_EISA_SLOTS
    char *name;
    char *protocol;
    int *controller;
    int device;
    int slot;
    unsigned int id;
    struct io_device *iod;
    struct pb *pb;

#if MODULAR
    ovly_load( "EISA" );
#endif
    /*
     *  Make sure that the ECU data for this hose has been processed before
     *  probing.
     */
    get_eisa_ecu_data( hose );
    for( slot = EISA_MIN_SLOT; slot <= EISA_MAX_SLOT; slot++ )
	{
	probe_number++;
	/*
	 *  Read the EISA ID for the current EISA device.  The call will
	 *  return 0 if there is no EISA device present.  If that happens,
	 *  there might be an ISA device in this slot, so check again.
	 */
	id = read_eisa_id( hose, slot );
	if( id == 0 )
	    id = eisa_ids[slot];
	if( id )
	    {
#if DEBUG
	    err_printf( "slot %d -- %08X\n", slot, id );
#endif
	    /*
	     *  We've found another EISA device, so build a PB for it.
	     */
	    pb = malloc_noown( sizeof( *pb ) );
	    pb->type = TYPE_EISA;
	    pb->hose = hose;
	    pb->bus = bus;
	    pb->slot = slot;
	    pb->order = probe_number;
	    insert_pb( pb );
	    /*
	     *  Search the I/O device table for this EISA device.  If we
	     *  find it, then the console knows how to handle it.
	     */
	    name = pb->id;
	    convert_eisa_id( name, id );
	    iod = find_io_device( TYPE_EISA, id & EISA_K_ID_MASK, 0 );
	    if( iod )
		{
		if( iod->name )
		    {
		    name = iod->name;
		    pb->hardware_name = name;
		    }
		device = iod->device;
		protocol = iod->protocol;
		controller = iod->controller;
		if( device )
		    {
		    pb->protocol = protocol;
		    pb->device = device;
		    pb->letter = controller;
		    }
		}
	    if( !eisa_ecu_data[slot] )
		{
		/*
		 *  If there's no ECU data, then say so, and leave the
		 *  board alone (unless it's a VGA card!).
		 */
		if( strcmp( pb->device, "vga" ) != 0 )
		    pb->device = 0;
		}
	    /*
	     *  Get the EISA IRQ (hence interrupt vector) for this EISA
	     *  device.
	     */
	    get_eisa_csr_vector( pb );
	    if( id == 0x002ea310 )
		{
		outportl( pb, pb->csr + 0xc8c, -1 );
		outportl( pb, pb->csr + 0xc90, 0 );
		if( inportl( pb, pb->csr + 0xc8c ) )
		    pb->hardware_name = "KFESB";
		}
	    }
	}
#endif
    }
#endif

#if PROBE_ISA_SLOTS
extern unsigned long int isacfg( int argc, char *argv[] );
extern union isa_config_table *isacfg_table;
extern struct HWRPB *hwrpb;
#endif

#if PROBE_ISA
probe_isa_bus( int hose, int bus )
{
#if PROBE_ISA_SLOTS
    unsigned int i;
    struct pb *pb;
    struct TABLE_ENTRY *entry;
    struct HWRPB *hwrpb_ptr = hwrpb;

    /*
     * Make sure that the ISA configuration data table has been 
     * initialized before probing.
     */
    ( void )isacfg( 0, 0 );
    for ( i = 0; i < isacfg_table->table.header.num_entries; i++ ) {
	entry = &isacfg_table->table.device[i];
	/*
	 * Slot 0 is reserved for on-board "junk I/O" multi-function 
	 * devices ( keyboard, mouse, serial and parallel port controllers)
	 * which do not need a port block defined.
	 */
	if ( entry->isa_slot > 0 ) {
	    probe_number++;
	    if ( entry->device_enable ) {
#if DEBUG
		err_printf( "slot %d -- %s\n", entry->isa_slot, entry->handle );
#endif
		pb = ( struct pb * )malloc_noown( sizeof( *pb ) );
		pb->type = TYPE_ISA;
		pb->hose = hose;
		pb->bus = bus;
		pb->slot = entry->isa_slot;
		pb->device = entry->handle;
		pb->order = probe_number;
		insert_pb( pb );
	    }
	}
    } 
    hwrpb_ptr->CONFIG_OFFSET[0] = isacfg_table;
    twos_checksum( hwrpb_ptr, offsetof( struct HWRPB, CHKSUM ) / 4, ( UINT * )hwrpb_ptr->CHKSUM );
#endif
}
#endif

probe_assign_letters( int hose )
    {
    char *name;
    int i;
    int *letter;
    struct pb *pb;

    krn$_wait( &pbs_lock );
    for( i = 0; i < num_pbs; i++ )
	{
	pb = pbs[i];
	if( pb->hose == hose )
	    {
	    letter = pb->letter;
	    if( letter )
		{
		pb->letter = 0;
		pb->controller = *letter;
		controller_num_to_id( pb->controller, pb->controller_id );
		*letter = *letter + 1;
#if DEBUG_PROBE
		if( pb->protocol )
		    {
#if MIKASA
		    err_printf( "bus %d, slot %2d", pb->bus, pb->slot );
#else
		    err_printf( "bus %d, slot %d", pb->bus, pb->slot );
#endif
		    if( pb->type == TYPE_PCI )
			if( incfgb( pb, 0x0e ) & 0x80 )
			    err_printf( ", function %d", pb->function );
		    err_printf( " -- %s%s -- %s\n",
			    pb->protocol, pb->controller_id,
			    pb->hardware_name );
		    }
#endif
		}
	    }
	}
    krn$_post( &pbs_lock );
    }

struct io_bus io_bus[] = {
#if PROBE_EISA
    { TYPE_EISA, probe_eisa },
#endif
#if PROBE_PCI
    { TYPE_PCI, probe_pci },
#endif
#if PROBE_VME
    { TYPE_VME, probe_vme },
#endif
    { 0, 0 }
    };

struct io_bus *find_io_bus( int type )
    {
    int i;
    struct io_bus *iob;

    for( iob = io_bus; iob->type; iob++ )
	if( ( iob->type == type ) )
	    return( iob );
    return( 0 );
    }

probe_io( )
    {
    int i;
    int hose;
    int type;
    struct io_bus *iob;

    dq_controllers = 0;
    dr_controllers = 0;
    dw_controllers = 0;
    en_controllers = 0;
    er_controllers = 0;
    ew_controllers = 0;
    fr_controllers = 0;
    fw_controllers = 0;
    mc_controllers = 0;
    pk_controllers = 0;
    pg_controllers = 0;
    pu_controllers = 0;
    rt_controllers = 0;
    tg_controllers = 0;
    vg_controllers = 0;
    ei_controllers = 0;

    i2o_controllers = 0;
    usb_controllers = 0;

    isa_eisa_count = 0;

    probe_number = 0;

    krn$_seminit( &scs_lock, 1, "scs_lock" );
    krn$_seminit( &ubs_lock, 1, "ubs_lock" );

    num_pbs = 0;
    num_pbs_all = 32;
    pbs = malloc_noown( sizeof( struct pb * ) * 32 );

    krn$_seminit( &sbs_lock, 1, "sbs_lock" );
    num_sbs = 0;
    num_sbs_all = 32;
    sbs = malloc_noown( sizeof( struct sb * ) * 32 );

    krn$_seminit( &pbs_lock, 1, "pbs_lock" );
    num_ubs = 0;
    num_ubs_all = 32;
    ubs = malloc_noown( sizeof( struct ub * ) * 32 );

#if FIBRE_CHANNEL
    krn$_seminit( &wwids_lock, 1, "wwids_lock" );
    num_wwids = 0;
    num_wwids_all = MAX_WWID_ALIAS_EV;
    wwid_ev_mask = 0;
    wwids = malloc_noown( sizeof( struct registered_wwid * ) * num_wwids_all );
    num_portnames = 0;
    portname_ev_mask = 0;
    portnames = malloc_noown( sizeof( struct registered_portname * ) * 
                                                         MAX_PORTNAME_ALIAS  );
    wwidmgr_mode = 0;
#endif

#if FRU5
vpd_data_q.flink = &vpd_data_q;
vpd_data_q.blink = &vpd_data_q;
#endif

#if PROBE_LOCAL
    probe_local( );
#endif
#ifdef MIN_HOSE
#ifdef MAX_HOSE
    for( i = MIN_HOSE; i <= MAX_HOSE; i++ )
	{
	hose = HOSE( i ); 
	type = get_hose_type( hose );
	iob = find_io_bus( type );
	if( iob )
	    iob->probe( hose );
	}
#endif
#endif
    }

#if MEDULLA
/*
 *  This routine is used to read in the PCMCIA size requirements
 *  as specified by 2 size EV's.  It returns the appropriate mask.
 *  This setting will effect all PCI->PCMCIA adapters.  We may
 *  want to individualize this later.
 */
int set_pcmcia_mask () 
{
    char value[8];
    int smem, dmem, data = 0;

    nvram_read_sev("pcmcia_smem_size",value);
    smem = xtoi(value);
    nvram_read_sev("pcmcia_dmem_size",value);
    dmem = xtoi(value);

    if (dmem && smem)
        return (0);  /* can't have both address spaces set - 1 BAR */

    switch (smem | dmem)
        {
	   case 0x800000: 	/* 8MB */
		data = 0xff700000;
	        break;
	   case 0x1000000: 	/* 16MB */
		data = 0xff000000;
	        break;
	   case 0x2000000: 	/* 32MB */
                data = 0xfe000000;
                break;
           case 0x4000000: 	/* 64MB */
                data = 0xfd000000;
                break;
	   default:     	/* 0 */
    	        return (0);	  
        }

    if (dmem)
       data |= 0x8;

    return (data);	  
}
#endif

#if DRIVERSHUT && !MODULAR
add_driver_pointers( char *device, int setmode, int config, int establish )
    {
    struct io_device *iod;

    for( iod = io_device; iod->type && iod->id; iod++ )
	{
	if( !strcmp_nocase( device, iod->device ) )
	    {
	    iod->setmode = setmode;
	    iod->config = config;
	    iod->establish = establish;
	    }
	}
    }
#endif

find_io_name( unsigned int id, unsigned int id_ext, char **name )
    {
    struct io_device_name *io;

    for( io = io_name; io->id; io++ )
	if(( io->id == id ) && ( io->id_ext == id_ext ))
	    {
	    *name = io->name;
	    break;
	    }
    }

/*****************************************************************************
	 PCI to CardBus Bridge
******************************************************************************/
/*Device info:
* The device is not supported behind a ppb. 
* 
* This is a multifunction PCI device which uses function 0 and 1 for
* Cardbus and/or PC Card. 
*
* This routine is similar to handle_pci_bridge() except probing 
* is not done on the secondary side of the bridge.
* 
*/

handle_cardbus_bridge( struct pb *pb ) 
    {
    struct pci_device_misc *pdm;
    struct pci_bridge_misc *pbm;
    struct pci_size_info *psi;

    /*
     *  Build a PBM to describe this Cardbus bridge.  
     *  Increment the PCI bus number.
     *  The primary bus number is the current PCI bus number, and the
     *  secondary bus number is the just-incremented PCI bus number.  The
     *  subordinate bus number will ALWAYS be the same as the secondary bus
     *  number for this device.
     */
    pdm = pb->pdm;
    pbm = malloc_noown( sizeof( *pbm ) );
    pdm->pbm = pbm;

    ++pci_bus;
    pbm->pri_bus = pb->bus;
    pbm->sec_bus = pci_bus;
    pbm->sub_bus = pci_bus;

#if DEBUG_PROBE
    err_printf( "Probing PCI-to-Cardbus bridge, bus %d\n", pci_bus );
#endif
    sprintf( pb->info, "Bridge to Cardbus %d,", pci_bus );

    /*
     *  Build a PSI to describe this bridge.  Insert the PSI into the tree.
     *  (If our parent already has a child, then we are a peer of that child,
     *  else we are our parent's child.)  Since PCI-to-PCI bridges cause a
     *  new level in the tree, set the new PSI as the current and parent PSI.
     *
     *  Bridges have a minimum granularity irrespective of the granularity
     *  of any devices behind them.  Take this into account.
     */
    psi = malloc( sizeof( *psi ) );
    if( parent_psi->child_psi )
	current_psi->peer_psi = psi;
    else
	current_psi->child_psi = psi;
    psi->parent_psi = parent_psi;
    psi->pb = pb;
    psi->flags = PCI_FLAGS_PCI_BRIDGE;
    psi->size[SIZE_GRAN_BASE_IO] = 16*1024;
    psi->gran[SIZE_GRAN_BASE_IO] = 16*1024;
    psi->size[SIZE_GRAN_BASE_SPARSE] = 1024*1024;
    psi->gran[SIZE_GRAN_BASE_SPARSE] = 1024*1024;
    psi->size[SIZE_GRAN_BASE_DENSE] = 1024*1024;
    psi->gran[SIZE_GRAN_BASE_DENSE] = 1024*1024;
    current_psi = psi;
    if( parent_psi->flags & PCI_FLAGS_PCI_BRIDGE )
	parent_psi->flags |= psi->flags & ( PCI_FLAGS_IO_16_BIT | PCI_FLAGS_VGA );
    }
