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
 *	hcb	17-Feb-1992	Initial entry.
 */
 
/* EISA NVRAM I/O REGISTERS */
    
#define NVRAM_CSR   0xC00
#define BAT	    0xC04
#define	HI_BASE	    0xC08
#define CONFIG	    0xC0C
#define ID	    0xC80
#define	CONTROL	    0xC84
#define	DMA_ADD0    0xC88
#define	DMA_ADD1    0xC8C


/* EISA NVRAM Control and Status Register Definitions */

#define LED		0x100
#define	IENBF		0x200
#define IENPF		0x400
#define	BF		0x800
#define BDCONN		0x1000
#define WRMEM		0x2000
#define	SETDREQ		0x4000

#define PRESTO_BATT_ENAB 0x13
#define PRESTO_BDISC 0x80

/* Diagnostic register bits */
#define DIAG_FAIL	    0x8
#define DIAG_PASS	    0x0

/* Memory area definitions */

#define DIAG_REG_OFFSET	    0x3F8
#define COOKIE_OFFSET	    0x400		
#define COOKIE_VALUE	    0xbd100248	

/* DMA test defines */
#define DMA2_CMD_STAT_REG	0x0D0
#define	DMA2_MODE_REG		0x0D6
#define DMA2_EXTND_MODE_REG	0x4D6
#define	DMA2_CLR_BYTE_PTR	0x0D8
#define DMA2_MASTER		0x0DA
#define DMA2_MASK_REF		0x0DE

#define DMA_CH7_BASE_REG	0x0CC
#define DMA_CH7_COUNT_REG	0x0CE
#define DMA_CH7_HIGH_COUNT_REG	0x4CE
#define DMA_CH7_LOW_PAGE_REG	0x08A
#define DMA_CH7_HIGH_PAGE_REG	0x48A

#define DMA_CH5_BASE_REG	0x0C4
#define DMA_CH5_COUNT_REG	0x0C6
#define DMA_CH5_HIGH_COUNT_REG	0x4C6
#define DMA_CH5_LOW_PAGE_REG	0x08B
#define DMA_CH5_HIGH_PAGE_REG	0x48B

#define DMA_CH5_MODE		0x01
#define DMA_CH5_MASK_SEL	0x02
#define DMA_CH5_MASK_SET	0x0D

#define DMA_CH7_MODE		0x03
#define DMA_CH7_MASK_SEL	0x08
#define DMA_CH7_MASK_SET	0x07

#define DMA_DEMAND_MODE		0x00
#define DMA_READ_TRNFR		0x08
#define DMA_BURST		0x30
#define DMA_32BITIO		0x08
#define DMA_ROTATE		0x10

#define DMA_BUFF_SIZE		0x07f
#define DMA_RO_OFFSET		0x100

volatile struct EISA_NVRAM_PB {
    struct pb pb;
    unsigned char *nvr_addr;
    int nvr_size;
    int diag_status;
    };
