/*
 * cixcd_def.h
 *
 * Copyright (C) 1990 by
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
 * Abstract:	CIPCA Register Definitions
 *
 * Author:	Dennis Mazur
 *
 * Modifications:
 *
 *	djm	14-Jul-1995	Initial entry.
 */
 
#define GLOBAL_DEBUG 0

#define N_MAX_TYP_PTRS	    1024
#define HWRPB$_PAGESIZE	    0x2000
#define VAX_PAGE_OFFSET     9
#define VAX_PAGE_MASK	    0x1ff
#define PAGE_OFFSET	    13
#define PAGE_MASK	    0x1fff


#define prefix(x) ci_prefix(pb,name,x)
#define malloc_a(x,y,z) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN|DYN$K_ALIGN,y,z)
#define malloc_z(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define malloc_az(x,y,z) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN|DYN$K_ALIGN,y,z)

#define get_pfn(x) ((int)x>>PAGE_OFFSET) 
#define map_pa(x) ((int)x+0x80000000)

/*
#define rl(x) cixcd_read_csr(pb->csr+x,"x",pb->pb.hose,pb->mbx)
#define wl(x,y) cixcd_write_csr(pb->csr+x,y,"x",pb->pb.hose,pb->mbx)
*/

/*
#define wl(r,v)   (outmeml(pb,pb->soft_base+r,v))
#define rl(r)     (inmeml(pb,pb->soft_base+r))
*/
 
#define wl(r,v)   (cipca_write_csr(blk_ptr,r,v))
#define rl(r)     (cipca_read_csr(blk_ptr,r))

enum ACC_VAL
         { NONE0,           /* No value                   (0) */
	   DATATEST,        /* Data Miscompare detected   (1) */
           TXTO,            /* Timeout waiting for TX int (2) */
           TXSTS,           /* Bad Tx ring status         (3) */
           RXTO,            /* Timeout waiting for RX int (4) */
	   RXSTS,           /* Bad Rx ring status         (5) */
           MISCOMP,         /* Data Miscomp during CI loop(6) */
           NONE7,NONE8,NONE9,NONEA,NONEB,NONEC,NONED,NONEE,NONEF,
           BAD_PTR,         /* Out of range passed to free   (0x10) */
           NOT_FREE,        /* "freed" still linked to queue (0x11) */
           INSANE,          /* Genereal error                (0x12) */
           PCIDMA0_INT,     /* PCI DMA eng 0 int pending     (0x13) */
           INV_CONTENT,     /* PCI DMA eng 1 counter > 3     (0x14) */
           PORTREG_RDINT,   /* Port Reg read int pend        (0x15) */
           PORTREG_INVLD,   /* Unexp Port reg access by host (0x16) */
           INVLD_XQE_LEN,   /* DQE_LEN or MQE_LEN > IBUF_LEN (0x17) */
           ABBR_BAD,        /* SBZ fields in ABBR not zero   (0x18) */
           QPTR_STOPPER,    /* A D2A queue entry was stopper (0x19) */
           MTD_AMTECR,      /* AMTECR written while MTD in AMCSR set  (0x1a) */
           PCIDMA0,         /* Error in PCI DMA ring eng 0   (0x1b) */ 
           PCIDMA1,         /* Error in PCI DMA ring eng 1   (0x1c) */
           PE,              /* Parity error                  (0x1d) */
           CI_INV_RTX,      /* invalid retransmit index      (0x1e) */
           CI_TX_PARERR,    /* Parity error tx ring          (0x1f) */
           CI_TX_UNDERUN,   /* Underrun Tx ring              (0x20) */
           CI_TX_NOPATH,    /* No Tx path when VC open       (0x21) */
           CI_RX_PARERR,    /* Parity error in Rx ring       (0x22) */
           CI_ASIC0_RXFAIL, /* Fatal error ASIC0 rx stat reg (0x23) */
           CI_ASIC1_RXFAIL, /* Fatal error ASIC1 rx stat reg (0x24) */
           CI_ASIC0_TXFAIL, /* Fatal error ASIC0 tx stat reg (0x25) */
           CI_ASIC1_TXFAIL, /* Fatal error ASIC1 tx stat reg (0x26) */
           NRP1_NULL,       /* NR Plus 1 buff Null           (0x27) */
           MACHINE_CHECK    /* Miscellaneous exception       (0x28) */ 
         };

#define emubase  0x500
#define memhardbase 0x000
#define memsoftbase 0x100
#define hardbase 0x0
#define softbase 0x0

#define xdev	emubase+0x000 	/* XMI Device Register */
#define xber	emubase+0x004 	/* XMI Bus Error Register */
#define xfadr	emubase+0x008	/* XMI Failing Address Register */
#define xcomm	emubase+0x010 	/* XMI Communications Register */
#define pscr	emubase+0x014 	/* Port Scan Control Register */
#define psdr	emubase+0x018 	/* Port Scan Data Register */
#define pdcsr	emubase+0x020 	/* Port Diagnostic Control/Status Register */
#define xfaer	emubase+0x02C 	/* Failing address reg */

/* 
 *  Hard Registers
 */
#define adprst	memhardbase+0x000 	/* Adapter Reset */
#define clrinta memhardbase+0x008	/* Clear Interrupt A */
#define clrintb memhardbase+0x010	/* Clear Interrupt B */
#define aitcr	memhardbase+0x018	/* Interrupt Holdoff */
#define nodests memhardbase+0x020	/* Node Status */
#define intena  memhardbase+0x028	/* Interrupte Enable */

/*
 * Soft registers
 */
#define abbr  	memsoftbase+0x0000	/* Adapter Block Base */ 
#define ccq2ir	memsoftbase+0x0008	/* Port Command Queue 2 Control Register */ 
#define ccq1ir	memsoftbase+0x0010	/* Port Command Queue 1 Control Register */ 
#define ccq0ir	memsoftbase+0x0018	/* Port Command Queue 0 Control Register */ 
#define adfqir	memsoftbase+0x0020	/* Port Datagram Free Queue Control Register */
#define amfqir	memsoftbase+0x0028	/* Port Message Free Queue Control Register */
#define psr  	memsoftbase+0x0030	/* channel adapter status reg */
#define casr0	memsoftbase+0x0030	/* channel adapter status reg */
#define casr1	memsoftbase+0x0034	/* channel adapter status reg */
#define cafar0	memsoftbase+0x0038	/* Port Failing address       Register */
#define cafar1	memsoftbase+0x003C	/* Port Failing address       Register */
#define psrcr	memsoftbase+0x0040	/* Port Status Release Control Register */
#define casrcr	memsoftbase+0x0040	/* Port Status Release Control Register */
#define picr	memsoftbase+0x0048	/* Port init   Control Register */
#define cicr	memsoftbase+0x0048	/* Channel init   Control Register */
#define pecr	memsoftbase+0x0050	/* Channel Enable Control Register */
#define cecr	memsoftbase+0x0050	/* Channel Enable Control Register */
#define amtcr	memsoftbase+0x0058	/* Adapter Maintenance Timer Control Register */
#define amtecr	memsoftbase+0x0060	/* Adapter Maintenance Timer Expiration Control Register */
#define amcsr	memsoftbase+0x0068	/* Adapter Maintenance Control/Status Register */
#define pmcsr	memsoftbase+0x0068	/* Adapter Maintenance Control/Status Register */
#define accx    memsoftbase+0x0070 	/* ACC extension register */
#define mrev    memsoftbase+0x00e8 	/* Microcode Revision and Status */
#define mrev_hw memsoftbase+0x00e8 	/* Hardware revision info */
#define mrev_fw memsoftbase+0x00ec	/* Firmware revision info */
#define musr	memsoftbase+0x00f0	/* Microcode Update Status Register */
#define mucar	memsoftbase+0x00f8 	/* Microcode Update Control and Address */
#define alt_clrinta memsoftbase+0x0108  /* Alternate Clear Interrupt A */
#define alt_aitcr   memsoftbase+0x0118  /* Alternate Int Holdoff */
#define alt_intena  memsoftbase+0x0128  /* Alternate Int Enable */

#define asnr 	0x0538 	/* Port Serial Number Register */
#define aidr	0x0540	/* interrupt dest reg */
#define amivr	0x0548 	/* misc int reg */
#define acivr	0x0550 	/* Completion interrupt reg */
#define compirr	0x0560 	/* Port Interrupt Vector Register */
#define asubr 	0x0568 	/* Port Command Queue 0 Control Register */ 



#define cixcd_psr_amfqe		0x1
#define cixcd_psr_mfqe		0x1
#define cixcd_psr_adfqe		0x2
#define cixcd_psr_dfqe		0x2
#define cixcd_psr_dse		0x4 
#define cixcd_psr_mse		0x8 
#define cixcd_psr_ac 		0x10
#define cixcd_psr_ste 		0x20
#define cixcd_psr_ic  		0x40 
#define cixcd_psr_pic  		0x40 
#define cixcd_psr_ec            0x80
#define cixcd_psr_pec           0x80
#define cixcd_psr_asic		0x100
#define cixcd_psr_me            0x80000000

#define cipca_casr_disabled	(cixcd_psr_ic)
#define cipca_casr_enabled	(cixcd_psr_ic | cixcd_psr_ec)
#define cipca_casr_completion 	(cixcd_psr_ec | cixcd_psr_ic | cixcd_psr_asic)
#define cipca_casr_mfqe	        (cipca_casr_completion | cixcd_psr_amfqe)

#define cipca_casr_errors      (cixcd_psr_amfqe |  \
                                cixcd_psr_adfqe |  \
		                cixcd_psr_dse   |  \
				cixcd_psr_mse   |  \
				cixcd_psr_ste   |  \
	                        cixcd_psr_ac    |  \
                                cixcd_psr_me)

typedef volatile union z_nodests {
  unsigned int i;
  struct {
    unsigned int adpsta	  : 4;  /* <3:0> Adapter status */
    unsigned int inta     : 1;  /* <4>   Interrupt A */
    unsigned int intb     : 1;  /* <5>   Interrupt B */
    unsigned int rsvd     : 10; /* <15:6> not used */
    unsigned int cfcs     : 16; /* <31:16> Alt. path to CFCS */
  }f;
} Z_NODESTS;

enum ADPSTA {INPROCESS,       /* Initialization is in progress (0) */
             INIT,            /* Reset Init completed          (1) */
             INV_UCODE,       /* Microcode chksum invalid      (2) */
             CPURAM_BAD,      /* uP SRAM test failed           (3) */
             BUFRAM_BAD,      /* Buffer SRAM failed            (4) */
             CABLESWAP,       /* CI cables are swapped         (5) */
             CI_PATHA,        /* Path A is bad                 (6) */
             CI_PATHB,        /* Path B is bad                 (7) */
             NODADR_MISMATCH, /* Node adr mismatch between A/B (8) */
             CLUSIZ_MISMATCH  /* Cluster size mismatch b/w A/B (9) */
};


typedef volatile union z_intena {
  unsigned int i;
  struct {
    unsigned int enaa     : 1;  /* <0>   Interrupt A */
    unsigned int enab     : 1;  /* <1>   Interrupt B */
  }f;
} Z_INTENA;
#define CIPCA_INTENA_M_ENAA  1
#define CIPCA_INTENA_M_ENAB  2


typedef volatile union z_mrev_hw {
  unsigned int i;
  struct {
    unsigned short int serialnum;  /* Two-byte little endian value */
    unsigned char modrev;          /* ASCII - PCB major rev */
    unsigned char modflg;          /* module capabilites bitfield */
  }f;
} Z_MREV_HW;

typedef volatile union z_mrev_fw {
  unsigned int i;
  struct {
    char rev[4];                   /* ASCII fw revision */
  }f;
} Z_MREV_FW;


typedef volatile union z_xber {
  unsigned int i;
  struct {
    unsigned int rsvd    : 1;	/* <0>   Reserved */
    unsigned int emp     : 1;	/* <1>   Enable MORE Protocol */
    unsigned int dxto    : 1;	/* <2>   Disable XMI Timeout */
    unsigned int notimp  : 1;	/* <3>   Not implemented */
    unsigned int fcid    : 6;	/* <4:9> Failing Commander ID */
    unsigned int stf     : 1;	/* <10>  Self-Test Failed */
    unsigned int notimp2 : 1;	/* <11>  Not implemented */
    unsigned int notimp3 : 1;	/* <12>  Not implemented */
    unsigned int tto     : 1;	/* <13>  Transaction Time Out */
    unsigned int rsvd2   : 1;	/* <14>  Reserved */
    unsigned int cnak    : 1;	/* <15>  Command No ACK */
    unsigned int rer     : 1;	/* <16>  Read Error Response */
    unsigned int rse     : 1;	/* <17>  Read Sequence Error */
    unsigned int nrr     : 1;	/* <18>  No Read Response */
    unsigned int crd     : 1;	/* <19>  Corrected Read Data */
    unsigned int wdnak   : 1;	/* <20>  Write Data No ACK */
    unsigned int ridnak  : 1;	/* <21>  Read / Ident Data No ACK */
    unsigned int wse     : 1;	/* <22>  Write Sequence Error */
    unsigned int pe      : 1;	/* <23>  Parity Error */
    unsigned int notimp4 : 1;	/* <24>  Not implemented */
    unsigned int notimp5 : 1;	/* <25>  Not implemented */
    unsigned int notimp6 : 1;	/* <26>  Not implemented */
    unsigned int cc      : 1;	/* <27>  Corrected confirmation */
    unsigned int notimp7 : 1;	/* <28>  Not implemented */
    unsigned int nhalt   : 1;	/* <29>  Node Halt */
    unsigned int nrst    : 1;	/* <30>  Node Reset */
    unsigned int es      : 1;	/* <31>  Error Summary */
  } f;
} Z_XBER;
typedef volatile union z_amcsr {
  unsigned int i;
  struct {
    unsigned int mi         : 1;	
    unsigned int std        : 1;	
    unsigned int ie         : 1;	
    unsigned int foo        : 1;	
    unsigned int cpded      : 1;	
    unsigned int cperr      : 1;	
    unsigned int foo1       : 2;       	
    unsigned int stuf       : 1;	
    unsigned int stof       : 1;	
    unsigned int yrpe       : 1;	
    unsigned int xrpe       : 1;	
    unsigned int ibpe       : 1;	
    unsigned int cspe       : 1;	
    unsigned int pribpe     : 1;	
    unsigned int mppbpe     : 1;	
    unsigned int xmrgpe     : 1;	
    unsigned int mapbpe     : 1;	
    unsigned int mbpbpe     : 1;	
    unsigned int prer       : 1;	
    unsigned int pwer       : 1;	
    unsigned int cwxape     : 1;	
    unsigned int cwxbpe     : 1;	
    unsigned int maibe      : 1;	
    unsigned int mbibe      : 1;	
    unsigned int cdibe      : 1;	
    unsigned int itibe      : 1;	
    unsigned int rsibe      : 1;	
    unsigned int mbpie      : 1;	
    unsigned int mabpe      : 1;	
    unsigned int mbbpe      : 1;	
    unsigned int foo3       : 1;	
  } f;
} Z_AMCSR;


typedef volatile union z_casr0 {	/* Adapter Status Register */
  unsigned int i; 
    struct {
	unsigned int amfqe  : 1;	
	unsigned int adfqe  : 1;	
	unsigned int dse    : 1;	
	unsigned int mse    : 1;	
	unsigned int ac     : 1;	
	unsigned int ste    : 1;	
	unsigned int ic     : 1;	
	unsigned int ec     : 1;	
	unsigned int asic   : 1;     
        unsigned int unin   : 1;	
	unsigned int        : 22;	
	unsigned int me     : 1;	
	} f;
} Z_CASR0;

typedef volatile union z_casr1 {
    unsigned int i;
    struct {
	unsigned int nerr  : 8;	/* 32 39 N-PORT errors */
	unsigned int cerr  : 8;	/* 40-47 CIMNA errors */
	unsigned int err   : 8;	/* 48-55    error */
	unsigned int mod   : 8 ;	/* 56-63 -  Module with error */
	} f;
    } Z_CASR1;
typedef volatile union z_abbr0 {
    unsigned int i;
    struct {
	unsigned pad   : 13; /* 0 */
	unsigned abbpa : 19;  /* upper bits of pa */	
	} f;
    } Z_ABBR0;

#define cixcd_ppr_port		0xff
#define cixcd_ppr_isdi		0x7f00
#define cixcd_ppr_ibuf_len	0x1fff0000
#define cixcd_ppr_csz		0xe0000000

#define cixcd_pmcsr_min		0x1
#define cixcd_pmcsr_mtd		0x2
#define cixcd_pmcsr_std		0x2
#define cixcd_pmcsr_ie		0x4

#define cixcd_llock_ilock 	0x1

struct cixcd_blk {
    u_int hose;
    u_int slot;
    u_int function;
    u_int bus;
    u_int controller;
    u_int hard_base;
    u_int soft_base;
    u_int mem_base;
    u_int vect1; /* misc int */
    u_int vect2; /* completion int*/
    u_int completion_pid;
    u_int p_misc_pid;
    u_int p_poll_pid;
    u_int refcnt;
    struct pb *pb;
    struct SEMAPHORE cixcd_completion_compl_s; /* interrupt handler process completion sems */
    struct SEMAPHORE cixcd_misc_compl_s;
    struct SEMAPHORE cixcd_poll_compl_s;
    struct SEMAPHORE cixcd_completion_s;       /* interrupt notification sems */
    struct SEMAPHORE cixcd_misc_s;
    struct SEMAPHORE cixcd_adap_s;
    struct SEMAPHORE cixcd_poll_s;
    struct MBX *mbx_int;	/* Pointer to mailbox to use in port's ISR */
    NPORT_AB *ab ;		/* Adapter block */
};
   

struct ddma {
  unsigned int      rsvd3[3];      /* Reserved */
  unsigned int      ba_ddma_base0;
  unsigned int      ba_ddma_base1; /* DMA window base */
  unsigned int      ddma_len0;
  unsigned int      ddma_len1;
};

