/*
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Port Block (PB) Definitions
 *
 * Author:	Harold Buckingham
 *
 * Modifications:
 *
 *	hcb	29-Jun-1992	Initial entry.
 *
 *	dtr	17-mar-1995	base_addr1 to base_addr0, base_addr2 to
 *				base_addr1
 *
 */
 
/* PCI NVRAM PCI Registers */
#define ID		0x00
#define DEV_CSR		0x04
#define	DEV_CLASS_REV	0x08
#define MISC_FUNC1	0x0c
#define BASE_ADDR0	0x10
#define BASE_ADDR1	0x14
#define	RESERVED	0x30
#define	MISC_FUNC2	0X3c

/* PCI NVRAM Memory Registers */
#define	ERROR		0x40
#define	FAIL_ADDR	0x44
#define	MEM_CONFIG	0x48
#define	EDC_CTRL	0x4c
#define	EDC_SYND	0x50
#define	BATT_CSR	0x54
#define	BATT_DIAG_RESET	0x58
#define	EEPROM_INTRFACE	0x5c
#define	DMA_SLAVE_ADDR	0x60
#define	DMA_MASTR_ADDR	0x64
#define	DMA_BYTE_CNT	0x68
#define	DMA_COMMAND	0x6c
#define	INT_CSR		0x70


/* PCI NVRAM Control and Status Register Bit Definitions */
#define CSR1_ENA_MEM		 0x00000002
#define CSR1_ENA_MSTR		 0x00000004
#define CSR1_ENA_SPEC_CYCL	 0x00000008
#define CSR1_ENA_PAR_ERR	 0x00000040
#define CSR1_ENA_SER_DRIVR	 0x00000100
#define CSR1_MSTR_DET_PAR_ERR	 0x01000000
#define CSR1_MSTR_REC_TAR_ABORT  0x08000000
#define CSR1_MSTR_SIG_MSTR_ABORT 0x10000000
#define CSR1_MSTR_SIG_TAR_ABORT  0x20000000
#define CSR1_DEV_SIG_SYS_ERR	 0x40000000
#define CSR1_DEV_DET_PAR_ERR	 0x80000000

/* PCI NVRAM Miscellaneous Function Register 1 Bit Definitions */
#define CSR3_CACHE_LN_SIZE	 0x000000ff
#define CSR3_LATENCY_TIMER	 0x0000f800

/* PCI NVRAM Miscellaneous Function Register 2 Bit Definitions */
#define CSR7_INT_LINE		 0x000000ff

/* PCI NVRAM Memory Configuration Register Bit Definitions */
#define CSRA_MEM_BANKS		 0x00000003
#define CSRA_SRAM_SIZE		 0x0000000c
#define CSRA_MOD_REV		 0x000000f0
#define CSRA_PCHIP_REV		 0x00000f00

/* PCI NVRAM EDC Control Register Bit Definitions */
#define CSRB_DISA_EDC_LOG	 0x00001000
#define CSRB_DISA_CRD_CORR	 0x00002000
#define CSRB_DISA_EDC_RPT	 0x00008000

/* PCI NVRAM Battery Control and Status Register Bit Definitions */
#define CSRD_BATT_CHRG		 0x00000001
#define CSRD_BATT_DISC		 0x00000002
#define CSRD_BATT_FAIL		 0x00000004
#define CSRD_BATT_OK		 0x00000008
#define CSRD_DIAG_MODE0		 0x00000010
#define CSRD_DIAG_MODE1		 0x00000020
#define CSRD_DIAG_MODE2		 0x00000040
#define CSRD_SPEED_BITS		 0x00000300

/* PCI NVRAM DMA Command Control and Status Register Bit Definitions */
#define CSR13_NO_CMD		 0x00000000
#define CSR13_WRT_TO_SLAVE	 0x00000001
#define CSR13_RD_FROM_SLAVE	 0x00000002

/* PCI NVRAM Interrupt Control and Status Register Bit Definitions */
#define CSR14_ENA_DMA_INT	 0x00000001
#define CSR14_ENA_CRD_INT	 0x00000002
#define CSR14_DMA_INT		 0x00000100
#define CSR14_CRD_INT		 0x00000200

/* Other definitions */
#define PCI_NVRAM_ID	0x71011
#define PRESTO_BATT_ENAB 0x13


/* Memory area definitions */
#define DIAG_REG_OFFSET	    0x3F8
#define DIAG_PASS	    0x0
#define DIAG_FAIL	    0x8
#define COOKIE_OFFSET	    0x400		
#define COOKIE_VALUE	    0xbd100248	

volatile struct PCI_NVRAM_PB {
    struct pb pb;
    unsigned int nvram_mem_base;
    int nvram_size;
    int diag_status;
    };
