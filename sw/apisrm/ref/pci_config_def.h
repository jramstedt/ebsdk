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
 * Abstract:	PCI configuration space header and associated definitions
 *             
 * Author:	Judith E. Gold
 *
 * Modifications:
 *
 *		jeg	12-Apr-1993	initial version.
 */

#ifndef pci_cfg
#define pci_cfg 1

/*----------------------PCI Configuration Utility-------------------------*/

/* a more general solution can be developed later to accommodate devices with
 * many address ranges required.
 */

typedef struct
 {
	unsigned long	vend_dev_id;	/* vendor and device id */
	unsigned char 	driver_id[4];	/* refers to class or port driver */
	unsigned int	io_address;	/* base IO mapped into memory */
	unsigned int 	mem_address;	/* PCI memory mapped into main memory */
	unsigned int	mem_addr2;	/* second memory range for VGA device */
 } PCICFG_TABLE;

/*-----------------------PCI Architecture---------------------------------*/

/* header types used by the PCI configuration Space Header */
/* header type 00 - the only one that exists so far */
struct HEADER00  {
	unsigned long	base_address_reg[6];
	unsigned long	reserved0[2];
	unsigned long	expnsn_rom_base;
	unsigned long	reserved1[2];
	unsigned char	interrupt_line;		
	unsigned char	interrupt_pin;		
	unsigned char	min_gnt;		
	unsigned char	max_lat;
 } header00;

/* header type 01 - placeholder for now, probably will be for addin memories */
struct HEADER01  {
	unsigned long	base_address_reg[3];
	unsigned long	reserved0[1];
	unsigned long	expnsn_rom_base;
	unsigned long	reserved1[1];
	unsigned char	interrupt_line;		
	unsigned char	interrupt_pin;		
	unsigned char	min_gnt;		
	unsigned char	max_lat;
 } header01;


/* PCI Configuration Space Header */
typedef struct  {
     	unsigned short	vendor_id;
	unsigned short	device_id;
	unsigned short	command;
	unsigned short	status;
	unsigned char	revision_id;
	unsigned char	pgm_iface;
	unsigned char	subclass;
	unsigned char	base_class;
	unsigned char	cache_line_size;
	unsigned char	latency_timer;
	unsigned char 	header_type;
	unsigned char	bist;		/* built-in self test */
	union HEADER{
		struct HEADER00 hd00;
		struct HEADER01 hd01;
	      } header;
	unsigned char dev_specfc[192]; /* device-specific area */ 
 } PCI_CFG_HDR;		


/*------------------------------------------------------------------------*/

/* vendor id */
#define VI_NO_DEVICE	0xffff

/* base class codes */
#define	BC_BACK_COMPAT	0	/* devices before class codes defs */
#define BC_MASS_STORAGE 1
#define BC_NETWORK	2
#define BC_DISPLAY	3
#define BC_MULTIMEDIA   4
#define BC_MEMORY	5
#define BC_BRIDGE	6
#define BC_UNKNOWN	0XFF

/* sub-class codes */

/* for base class 00 - PCI devices prior to class definitions */
#define	SC0_NOT_VGA	0	/* not VGA-compatible */
#define SC0_VGA		1       /* VGA-compatible */

/* for base class 01 - mass storage controller */
#define SC1_SCSI	0
#define SC1_IDE		1
#define SC1_FLOPPY	2
#define SC1_IPI		3
#define SC1_OTHER	0X80

/* for base class 02 - network controller */
#define SC2_ETHERNET	0
#define SC2_TOKEN_RING	1
#define SC2_FDDI	2
#define SC2_OTHER	0X80

/* for base class 03 - display controller */
#define SC3_VGA		0
#define SC3_SUPER_VGA	1
#define SC3_XGA		2
#define SC3_OTHER	0X80

/* for base class 04 - multimedia device */
#define SC4_VIDEO	0
#define SC4_AUDIO	1
#define SC4_OTHER	0X80

/* for base class 05 - memory controller */
#define SC5_RAM		0
#define SC5_FLASH	1
#define SC5_OTHER	0X80

/* for base class 06 - bridge device */
#define SC6_HOST	0
#define SC6_ISA		1
#define SC6_EISA        2
#define SC6_MICRO_CH	3
#define SC6_PCI_TO_PCI  4
#define SC6_PCMCIA	5
#define SC6_OTHER	0X80


/* programming interface codes */
#define NO_PGM_IFACE	0

/* command register */
struct pci_command {
	unsigned pcicmd$v_io   : 1;  	/* IO space access */
	unsigned pcicmd$v_mem  : 1;	/* memory space access */
	unsigned pcicmd$v_busm : 1;	/* device can be a bus master */
	unsigned pcicmd$v_spcy : 1;	/* special cycle operations */
	unsigned pcicmd$v_memwi: 1;	/* memory write and invalidate enable */
	unsigned pcicmd$v_vgaps: 1;	/* VGA pallette snoop 	*/
	unsigned pcicmd$v_perr : 1;	/* parity error response */
	unsigned pcicmd$v_wcc  : 1;	/* wait cycle control */
	unsigned pcicmd$v_serr : 1;	/* system error enable */
 };
#define	pcicmd$m_io	1
#define	pcicmd$m_mem	2
#define	pcicmd$m_busm	4
#define	pcicmd$m_spcy	8
#define	pcicmd$m_memwi	16
#define	pcicmd$m_vgaps	32
#define	pcicmd$m_perr	64	
#define	pcicmd$m_wcc	128
#define pcicmd$m_serr	256

/* status register */
struct pci_status {
	unsigned pcists$v_res      : 8;	/* unused */
	unsigned pcists$v_dpd      : 1;	/* data parity detected */
	unsigned pcists$v_devsel   : 2;	/* DEVSEL timing */
	unsigned pcists$v_sta      : 1;	/* signaled target abort */
	unsigned pcists$v_rta      : 1;	/* received target abort */
	unsigned pcists$v_rma      : 1;	/* received master abort */
	unsigned pcists$v_sse      : 1;	/* signaled system error */
	unsigned pcists$v_dpe      : 1;	/* detected parity error */
 };
#define pcists$m_dpd	256
#define pcists$m_devsel 1536
#define pcists$m_sta	2048
#define pcists$m_rta	4096
#define pcists$m_rma	8192
#define pcists$m_sse    16384
#define pcists$m_dpe    32768

/* DEVSEL timing */
#define devsel$k_fast	0
#define devsel$k_meduim 1
#define devsel$k_slow	2

/* BIST register */
struct pci_bist {
	unsigned pcibist$v_cc	: 4;	/* completion code */
	unsigned pcibist$v_res	: 2;	/* unused */
	unsigned pcibist$v_sb	: 1;	/* start BIST */
	unsigned pcibist$v_bc	: 1;	/* BIST capable */
 };
#define pcibist$m_cc	15
#define pcibist$m_sb    64
#define pcibist$m_bc    128

/*------------------------------------------------------------------------*/

/* header00 registers */

/* base address registers */
struct pci_base_addr {
	unsigned pciba$v_type	: 1;	/* IO or memory space */
	unsigned pciba$v_32bit	: 1;	/* locate anywhere in 32-bit adr space*/
	unsigned pciba$v_b1m	: 1;	/* locate below 1 Meg */
	unsigned pciba$v_64bit	: 1;	/* locate anywhere in 64-bit adr space*/
	unsigned pciba$v_pf	: 1;	/* prefetchable */
 };
#define pciba$m_type	1
#define pciba$m_32bit	2
#define pciba$m_b1m	4
#define pciba$m_64bit	8
#define pciba$m_pr	16

#define pciba$k_mem	0		/* memory base address register */
#define pciba$k_io	1               /* I/O base address register */

/* expansion ROM base address register */
#define pcierb$v_ade	1		/* expansion ROM address decode enable*/

/*------------------------------------------------------------------------*/

/* expansion ROM */

/* expansion ROM header */
typedef struct {
	unsigned short romsig;	/* ROM signature */
	unsigned char res[16];  /* reserved */
	unsigned short p_pcids; /* address (16 bits) of PCI data structure */
 } PCI_ER_HEADER;

/* romsig */
#define pcierh$k_sig	0XAA55

/* expansion ROM PCI data structure */
typedef struct  {
	unsigned int   signature;	/* "PCIR" */
	unsigned short vendor_id;	/* vendor identification */
	unsigned short device_id;	/* device identification */
	unsigned short p_vpd;		/* ptr to vital product data */
	unsigned short pcids_len;	/* PCI data structure length */
	unsigned char  pcids_rev;	/* PCI data structure revision */
	unsigned char  pgm_iface;       /* class code fields */
	unsigned char  base_class;	
	unsigned char  subclass;	
	unsigned char  image_len;	/* image length  in 512 byte units */
	unsigned short revlvl;		/* revision level of expnsn ROM code */
	unsigned char  code_type;	/* Intel, OPENBOOT, etc. compatible */
	unsigned char  indicator;	/* last image in ROM indicator */
 } PCI_DS;

/* signature */
#define pcids$k_sig	"PCIR"

/* code_type */
#define CT_INTEL_X86	0
#define CT_OPENBOOT	1

/* indicator */
#define pcids$k_last_image	1	
#define pcids$v_last_image	7
#define pcids$m_last_image   	268435456	

/*----------------------------------------------------------------------------*/
#define MAPPED_IO_BASE 0x3a014000    /* bogus for compile purposes ! */

#endif
