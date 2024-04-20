/* file:	alcor_io.h
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
 *
 *
 * Abstract:	Pci definitions.
 *
 * Author: Joe Smyth
 *
 * Modifications:
 *
 *  jimh    - Nov-06-1993 - added in true Alcor Specifics ...
 *  tonyc   - Jan-26-1995 - changed EISA_K_MAX_SLOTS from 6 to 4
 *
 */

/* Device Vendor ID's dummy defines, to be filled in later */
#define DEC_TULIP		0x00021011
#define DEC_TGA			0x00041011
#define NCR_810			0x00011000
#define INTEL_SIO		0x04828086

#define PCI_EISA_SLOT 11


/* Register offsets into PCI config space; only longword aligned regs shown */
#define VEND_DEV_ID		0x0
#define COM_STAT		0x4
#define REV_ID			0x8
#define CACHE_L_SIZ		0xC
#define BASE_ADDR0		0x10
#define BASE_ADDR1		0x14
#define BASE_ADDR2		0x18
#define BASE_ADDR3		0x1C
#define BASE_ADDR4		0x20
#define BASE_ADDR5		0x24
#define RESERVED0		0x28
#define RESERVED1		0x2C
#define EXP_ROM_BASE	0x30
#define RESERVED2		0x34
#define RESERVED3		0x38
#define INT_LINE		0x3C

/* Base Address allocation constants */
#define MIN_ALIGN_MEM	0x1000 /* 4k bytes is the minimum MEM page alignment size */
#define MIN_ALIGN_IO	0x100 /* 256 bytes is the minimum IO page alignment size */
#define MIN_ALIGN_IO_MASK  0xff  /* mask for doing page aligns */
#define ONE_MEG 0x100000

#define PCI_K_MAX_SLOTS 13
#define PCI_K_MAX_BUS 	1   

#define PCI_K_ID_REG 0xc80
#define PCI_K_ID_MASK 0x00ffffff


/*
    Alcor specific Addressing for the PCI/CSRs
*/
#define IO_ADDR_SEL	0x858	/* Sparse I/O region A */
#define CONFIG_ADDR_SEL	0x870	/* Configuration space */
#define DMEM_ADDR_SEL	0x860	/* Dense Memory Space */
#define SMEM_ADDR_SEL	0x800	/* sparse memory space region 0 */
#define MEM_ADDR_SEL	0x800	/* sparse memory space region 0 */ 

#define PCI_SPECIAl_ACK 0x872	/* Special cycle/Interrupt ack register */
#define CIA_MEMO_CSR	0x875	/* CIA memory CSR's */
#define CIA_MAIN_CSRS	0x874	/* CIA Main CSR's */
#define CIA_XLAT_CSR     0x876	/* Address translation CSR's */
#define GRU_ROM_CSR	0x878	/* GRU and ROM CSR's */

#define Pci_mem	        (BaseMem<<28)
#define Pci_IO		(BaseIO<<28)
#define Pci_cfg_off	(BaseIO<<28)
#define Pci_cfg(id,func,ofst)	(((cfg_one<<id)<<11)|((func&0x7)<<8)|(ofst&0xff))

/* pci_size_config.c */
#define	Pci_cfg1(bus,id,func,ofst) ((bus << 16) | (id << 11) | ((func & 0x7) << 8) | (ofst & 0xff))
/* end pci_size_config.c */


#define Wrd		(one<<3)
#define Tri		(two<<3)
#define Lng		(three<<3)
#define Local_CSR       (BaseCSR<<28)
#define MAX_Local_CSR   0x0



/*PCI bus macros*/
#define Bmask(x) (((x)&0xff))
#define Wmask(x) (((x)&0xffff))
#define Lmask(x) (((x)&0xffffffff))

#define PCI_B_DATA_OUT(d) ((Bmask(d)<<24) | (Bmask(d)<<16) | (Bmask(d)<<8) | Bmask(d))
#define PCI_W_DATA_OUT(d) ((Wmask(d)<<16) | Wmask(d))
#define PCI_L_DATA_OUT(d) (Lmask(d))
#define PCI_CSR_DATA_OUT(d) (Wmask(d))

/*
 * The following Macros shift the relevant data into the least significant
 * part of the datum's 64bit buffer. For further explanation see Figure 2-6 
 * of the EV4-CPU DaughterCard Functional Spec.
 */
#define PCI_B_DATA_IN(p,d) (Bmask(d >> ((p&3)*8)))
#define PCI_W_DATA_IN(p,d) (Wmask(d >> ((p&3)*8)))
#define PCI_L_DATA_IN(p,d) (Lmask(d))
#define PCI_CSR_DATA_IN(p,d) (Wmask(d))

#define IO_B(x)		(Pci_IO|(((x)&0x3fffffff)<<5))
#define IO_W(x)		(Pci_IO| Wrd | ( ((x) & 0x3fffffff) <<5 ) )
#define IO_L(x)		(Pci_IO|Lng|(((x)&0x3fffffff)<<5))

/* TYPE 0 Configuration cycles */
#define IO_CFG_L(id,func,ofst)	(Pci_cfg_off |Lng| ( ( (Pci_cfg(id,func,ofst) &0x1ffffff) <<5) ) )
#define IO_CFG_W(id,func,ofst)	(Pci_cfg_off |Wrd|(((Pci_cfg(id,func,ofst)&0x1ffffff)<<5)))
#define IO_CFG_B(id,func,ofst)	(Pci_cfg_off |(((Pci_cfg(id,func,ofst)&0x1ffffff)<<5)))


/* pci_size_config.c */

/* TYPE 1 Configuration cycles */
#define IO_CFG_L1(bus, id,func,ofst)	(Pci_cfg_off| Lng | (((Pci_cfg1(bus,id,func,ofst)&0x1fffffff)<<5)))
#define IO_CFG_W1(bus, id,func,ofst)	(Pci_cfg_off|Wrd|(((Pci_cfg1(bus,id,func,ofst)&0x1fffffff)<<5)))
#define IO_CFG_B1(bus, id,func,ofst)	(Pci_cfg_off|(((Pci_cfg1(bus,id,func,ofst)&0x1fffffff)<<5)))

/* end pci_size_config.c */


#define PCI_B_IOPTR(x) (IO_B(x))
#define PCI_W_IOPTR(x) (IO_W(x))
#define PCI_L_IOPTR(x) (IO_L(x))
#define PCI_CSR_IOPTR(x) (IO_CSR(x))
#define PCI_CFG_L_IOPTR(id,func,ofst) (IO_CFG_L(id,func,ofst))
#define PCI_CFG_W_IOPTR(id,func,ofst) (IO_CFG_W(id,func,ofst))
#define PCI_CFG_B_IOPTR(id,func,ofst) (IO_CFG_B(id,func,ofst))

#define SMEM_B(x) (Pci_mem|(((x)&0x3fffffff)<<5))
#define SMEM_W(x) (Pci_mem|Wrd|(((x)&0x3fffffff)<<5))
#define SMEM_L(x) (Pci_mem|Lng|(((x)&0x3fffffff)<<5))

#define DMEM_L(x) (Pci_mem|((x)&0xffffffff))


#define PCI_B_SMEMPTR(x) SMEM_B(x)
#define PCI_W_SMEMPTR(x) SMEM_W(x)
#define PCI_L_SMEMPTR(x) SMEM_L(x)

#define PCI_L_DMEMPTR(x) DMEM_L(x)

/*                                   					*
 *		EISA BUS DEFINITIONS                                    *
 *									*/

/*
    Alcor specific Addressing for the EISA/PCI
*/

#define EISA_BUSES       1	/* number of eisa buses*/
#define EISA_HOSE	 1
#define EISA_K_MAX_SLOTS 4	/* number of physical eisa slots */
#define MAX_PCI_SLOTS	14

#define EISA_ID_BYTE0   0x00		/* dummy values */
#define EISA_ID_BYTE1   0x00
#define EISA_ID_BYTE2   0x00
#define EISA_ID_BYTE3   0x00

#define Eisa_mem	(BaseMem<<28)
#define Eisa_IO		(BaseIO<<28)
#define Eisa_cfg(id,func,ofst)	(((cfg_one<<id)<<11)|((func&0x7)<<8)|(ofst&0xff))

#define MEM_X(x)	(Eisa_mem|((x)&0xffffffff))
#define MEM_B(x)	(Eisa_mem|(((x)&0x3fffffff)<<5))
#define MEM_W(x)	(Eisa_mem|Wrd|(((x)&0x3fffffff)<<5))
#define MEM_L(x)	(Eisa_mem|Lng|(((x)&0x3fffffff)<<5))
#define MEM_Q(x)	(Eisa_mem|Lng|(((x)&0x3fffffff)<<5))


/*
ALCOR specific addressing for CSR write/reads 
*/

#define CSR_L(x) (Local_CSR | ((x)&0x3fffffff))

/*
  ALCOR CSR offsets ....
*/

/* main CSR's */
#define CSR_cia_rev	    0x80
#define CSR_pci_lat	    0xc0
#define CSR_cia_ctrl	    0x100
#define CSR_cia_cnfg	    0x140	/* available on pass 3 CIA */
#define CSR_hae_mem	    0x400
#define CSR_hae_io	    0x440
#define CSR_cfg		    0x480
#define CSR_cack_en	    0x600
#define CSR_cia_diag	    0x2000
#define CSR_diag_check	    0x3000
#define CSR_cia_perf_mon    0x4000
#define CSR_cia_perf_cntl   0x4040

/* memory CSR's */
#define CSR_mcr		    0x0
#define	CSR_mba0	    0x600
#define	CSR_mba2	    0x680
#define	CSR_mba4	    0x700
#define	CSR_mba6	    0x780
#define	CSR_mba8	    0x800
#define	CSR_mba10	    0x880
#define	CSR_mba12	    0x900
#define	CSR_mba14	    0x980
#define CSR_tmg0	    0xb00
#define CSR_tmg1	    0xb40
#define CSR_tmg2	    0xb80

/* Error CSR's */
#define CSR_eir0	    0x8000
#define CSR_eir1	    0x8040
#define CSR_cia_err	    0x8200
#define CSR_cia_err_stat    0x8240
#define CSR_cia_err_mask    0x8280
#define CSR_cia_syn	    0x8300
#define CSR_cia_mem0	    0x8400
#define CSR_cia_mem1	    0x8440
#define CSR_pci_err0	    0x8800
#define CSR_pci_err1	    0x8840

/* Address Xlation CSR's */
#define CSR_tbia	    0x100
#define CSR_w_base0	    0x400
#define CSR_w_mask0	    0x440
#define CSR_t_base0	    0x480
#define CSR_w_base1	    0x500
#define CSR_w_mask1	    0x540
#define CSR_t_base1	    0x580
#define CSR_w_base2	    0x600
#define CSR_w_mask2	    0x640
#define CSR_t_base2	    0x680
#define CSR_w_base3	    0x700
#define CSR_w_mask3	    0x740
#define CSR_t_base3	    0x780
#define CSR_w_dac3	    0x7c0
#define CSR_ltb_tags	    0x800
#define CSR_tb_tags	    0x900
#define CSR_tb0_pages	    0x1000
#define CSR_tb1_pages	    0x1100
#define CSR_tb2_pages	    0x1200
#define CSR_tb3_pages	    0x1300
#define CSR_tb4_pages	    0x1400
#define CSR_tb5_pages	    0x1500
#define CSR_tb6_pages	    0x1600
#define CSR_tb7_pages	    0x1700

/* GRU CSR's */
#define CSR_int_req	    0x0
#define	CSR_int_msk	    0x40
#define	CSR_int_edge	    0x80
#define	CSR_int_hilo	    0xc0
#define	CSR_int_clr	    0x100
#define	CSR_cache_cnfg	    0x200
#define	CSR_scr		    0x300
#define	CSR_leds	    0x800
#define	CSR_reset	    0x900

/* cache size on the ALCOR in longwords */
#define CACHE_LWORD_SIZE    16			    /*pci_size_config.c*/

/* platform specific maximum device */
#define PCI_MAX_DEV 13				    /*pci_size_config.c*/


unsigned char inmemb( struct pb *pb, unsigned __int64 offset );

unsigned short inmemw( struct pb *pb, unsigned __int64 offset );

unsigned long inmeml( struct pb *pb, unsigned __int64 offset );

void outmemb( struct pb *pb, unsigned __int64 offset, unsigned char data );

void outmemw( struct pb *pb, unsigned __int64 offset, unsigned short data );

void outmeml( struct pb *pb, unsigned __int64 offset, unsigned long data );

unsigned char inportb( struct pb *pb, unsigned __int64 offset );

void outportb( struct pb *pb, unsigned __int64 offset, unsigned char data );

unsigned short inportw( struct pb *pb, unsigned __int64 offset );

unsigned long inportl( struct pb *pb, unsigned __int64 offset );

void outportw( struct pb *pb, unsigned __int64 offset, unsigned short data );

void outportl( struct pb *pb, unsigned __int64 offset, unsigned long data );

unsigned __int64 inmemq( struct pb *pb, unsigned __int64 offset );

void outmemq( struct pb *pb, unsigned __int64 offset, unsigned __int64 data );

unsigned long indmeml( struct pb *pb, unsigned __int64 offset );

unsigned __int64 indmemq( struct pb *pb, unsigned __int64 offset );

void outdmeml( struct pb *pb, unsigned __int64 offset, unsigned long data );

void outdmemq( struct pb *pb, unsigned __int64 offset, unsigned __int64 data );

unsigned char incfgb( struct pb *pb, unsigned __int64 offset );

unsigned short incfgw( struct pb *pb, unsigned __int64 offset );

unsigned long incfgl( struct pb *pb, unsigned __int64 offset );

void outcfgb( struct pb *pb, unsigned __int64 offset, unsigned char data );

void outcfgw( struct pb *pb, unsigned __int64 offset, unsigned short data );

void outcfgl( struct pb *pb, unsigned __int64 offset, unsigned long data );

#define CIA_PASS1 0

#define PCI_DEPTH 1

#define io_get_window_base(pb) 0x40000000

#define MODREV_ALCOR	1 
#define MODREV_MAVERICK	8
#define MODREV_BRET     10
#define MODREV_XL300    12
#define MODREV_XLT366   13

