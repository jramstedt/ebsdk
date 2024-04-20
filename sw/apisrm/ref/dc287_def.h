/* File:	dc287_def.h
 *
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
 * Abstract:	DC287 "TULIP" Chip header definitions file
 *
 * Author:	D.W. Neale
 *
 * Modifications:
 *
 *	dwn	21-May-1993	Initial Entry
 */
 
/* LOCAL SYMBOLS and Macro definitions
*/

/* DC287 CSR PCI offset definitions
*/
#define DC287_PCI_BASE   0x10000    /* DC287  pci base 2nd 64k block */
#define DC287_REG_INC    0x08       /* DC287  register incremenet */

/* PCI Register offsets
*/
#define DC287_OCSR0	0x00	/* Bus Mode register */
#define DC287_OCSR1	0x08	/* Transmit Poll Demand	register */
#define DC287_OCSR2	0x10	/* Receive Poll	register */
#define DC287_OCSR3	0x18	/* Rx Ring Base Address register */
#define DC287_OCSR4	0x20	/* Tx Ring Base Address	register */
#define DC287_OCSR5	0x28	/* Status  register */
#define DC287_OCSR6	0x30	/* Serial Command register */
#define DC287_OCSR7	0x38	/* Interrupt Mask register */
#define DC287_OCSR8	0x40	/* Missed frame counter	register */
#define DC287_OCSR9	0x48	/* Address and Mode Diagnostic register */
#define DC287_OCSR10	0x50	/* Data Diagnostic register */
#define DC287_OCSR11	0x58	/* Full Duplex register */
#define DC287_OCSR12	0x60	/* SIA Status register */
#define DC287_OCSR13    0x68    /* SIA conectivity register */
#define DC287_OCSR14    0x70    /* SIA Tx Rx register */
#define DC287_OCSR15    0x78    /* SIA General register */

/* PCI Register access macros
*/
#define DC287_CSR0	(DC287_PCI_BASE+ DC287_OCSR0)
#define DC287_CSR1	(DC287_PCI_BASE+ DC287_OCSR1)
#define DC287_CSR2	(DC287_PCI_BASE+ DC287_OCSR2)
#define DC287_CSR3	(DC287_PCI_BASE+ DC287_OCSR3)
#define DC287_CSR4	(DC287_PCI_BASE+ DC287_OCSR4)
#define DC287_CSR5	(DC287_PCI_BASE+ DC287_OCSR5)
#define DC287_CSR6	(DC287_PCI_BASE+ DC287_OCSR6)
#define DC287_CSR7	(DC287_PCI_BASE+ DC287_OCSR7)
#define DC287_CSR8	(DC287_PCI_BASE+ DC287_OCSR8)
#define DC287_CSR9	(DC287_PCI_BASE+ DC287_OCSR9)
#define DC287_CSR10	(DC287_PCI_BASE+ DC287_OCSR10)
#define DC287_CSR11	(DC287_PCI_BASE+ DC287_OCSR11)
#define DC287_CSR12	(DC287_PCI_BASE+ DC287_OCSR12)
#define DC287_CSR13     (DC287_PCI_BASE+ DC287_OCSR13)
#define DC287_CSR14     (DC287_PCI_BASE+ DC287_OCSR14)
#define DC287_CSR15     (DC287_PCI_BASE+ DC287_OCSR15)


/* DC287 CSR Bit Mask definitions
/*    RW bits have an associated 1
/*    RO bits have an associated 0
*/
#define DC287_MCSR0	0x0000FFFE	/* Bus Mode register */

#if MEDULLA||CORTEX|| YUKONA
#define DC287_MCSR1	0x00000000	/* Transmit Poll Demand	register */
#define DC287_MCSR2	0x00000000	/* Receive Poll	register */
#else
#define DC287_MCSR1	0x00000001	/* Transmit Poll Demand	register */
#define DC287_MCSR2	0x00000001	/* Receive Poll	register */
#endif

#define DC287_MCSR3	0xFFFFFFFC	/* Rx Ring Base Address register */
#define DC287_MCSR4	0xFFFFFFFC	/* Tx Ring Base Address	register */

#if MEDULLA||CORTEX || YUKONA
#define DC287_MCSR5	0x00000000	/* Status  register */
#define DC287_MCSR6	0x0003DFE8	/* Serial Command register */
#else
#define DC287_MCSR5	0x0CFFBFEF	/* Status  register */
#define DC287_MCSR6	0x0000DFFB	/* Serial Command register */
#endif

#define DC287_MCSR7	0x0001BFEF	/* Interrupt MaCSR register */

#if MEDULLA||CORTEX  || YUKONA
#define DC287_MCSR8	0x00000000	/* Missed frame counter	register */
#define DC287_MCSR9	0x00000000	/* Address and Mode Diagnostic register */
#define DC287_MCSR10	0x00000000      /* Data Diagnostic register */
#else
#define DC287_MCSR8	0x0001FFFF	/* Missed frame counter	register */
#define DC287_MCSR9	0x000003FB	/* Address and Mode Diagnostic register */
#define DC287_MCSR10	0xFFFFFFFF	/* Data Diagnostic register */
#endif

#define DC287_MCSR11	0x00000000	/* Full Duplex register */
#define DC287_MCSR12	0x00000000	/* SIA Status register */

#if MEDULLA||CORTEX || YUKONA
#define DC287_MCSR13    0x00000000      /* SIA conectivity register */
#else
#define DC287_MCSR13    0x0000FF7F      /* SIA conectivity register */
#endif

#if MEDULLA||CORTEX  || YUKONA
#define DC287_MCSR14    0x00008000      /* SIA Tx Rx register */
#define DC287_MCSR15    0x00000000      /* SIA General register */
#else
#define DC287_MCSR14    0x0000773F      /* SIA Tx Rx register */
#define DC287_MCSR15    0x00003737      /* SIA General register */
#endif

/* DC287 CSR reset values 
*/
#define DC287_ICSR0	0xFFF80000	/* Bus Mode register */
#define DC287_ICSR1	0xFFFFFFFF	/* Transmit Poll Demand	register */
#define DC287_ICSR2	0xFFFFFFFF	/* Receive Poll	register */
#define DC287_ICSR3	0x00000000	/* Rx Ring Base Address register */
#define DC287_ICSR4	0x00000000	/* Tx Ring Base Address	register */
#define DC287_ICSR5	0xFC000000	/* Status  register */
#define DC287_ICSR6	0xfffc0040	/* Serial Command register */
#define DC287_ICSR7	0xfffe0000	/* Interrupt MaCSR register */
#define DC287_ICSR8	0xfffe0000	/* Missed frame counter	register */
#define DC287_ICSR9	0x7fffff00	/* Address and Mode Diagnostic register */
#define DC287_ICSR10	0xffffffff	/* Data Diagnostic register */
#define DC287_ICSR11	0xffffffff	/* Full Duplex register */
#define DC287_ICSR12	0xffffffc4	/* SIA Status register */
#define DC287_ICSR13    0xffff0000      /* SIA conectivity register */
#define DC287_ICSR14    0xffffffff      /* SIA Tx Rx register */
#define DC287_ICSR15    0xffff0000      /* SIA General register */

/* DC287 CSR Default Program values 
*/
#define DC287_DCSR0	0x00000000	/* Bus Mode register */
#define DC287_DCSR1	0x00000000	/* Transmit Poll Demand	register */
#define DC287_DCSR2	0x00000000	/* Receive Poll	register */
#define DC287_DCSR3	0x00000000	/* Rx Ring Base Address register */
#define DC287_DCSR4	0x00000000	/* Tx Ring Base Address	register */
#define DC287_DCSR5	0xFFFFFFFF	/* W1C Status  register */
#define DC287_DCSR6	0xFFFE0044	/* Serial Command register */
#define DC287_DCSR7	0xFFFE4010	/* Interrupt MaCSR register */
#define DC287_DCSR8	0xFFFE0000	/* Missed frame counter	register */
#define DC287_DCSR9	0x00000000	/* Address and Mode Diagnostic register */
#define DC287_DCSR10	0x00000000	/* Data Diagnostic register */
#define DC287_DCSR11	0xFFFF0000	/* Full Duplex register */
#define DC287_DCSR12	0xFFFFFFC4	/* SIA Status register */
#define DC287_DCSR13    0xFFFF0000      /* SIA conectivity register */
#define DC287_DCSR14    0xFFFFFFFF      /* SIA Tx Rx register */
#define DC287_DCSR15    0xFFFF0000      /* SIA General register */


/* DC287 PCI CONFIGURATION REGISTER offset definitions
*/
#define DC287_CONFIG_INC    0x4		/* DC287  config register incremenet */
#define DC287_DCBIO_IO      0x00000001	/* Config Base Register IO  Adrs space */
#define DC287_DCBIO_MEM     0x00000000  /* Config Base Register MEM Adrs space */

/* PCI Register offsets
*/
#define DC287_CFID      0x00    /* Configuration ID */
#define DC287_CFCS      0x04    /* Configuration Command Status */
#define DC287_CFRV      0x08    /* Configuration Revision */
#define DC287_CFLT      0x0C    /* Configuration Latency Timer */
#define DC287_CBIO      0x10    /* Configuration Base IO Address */
/* rsvd 14h-3Ch */

/* DC287 PCI CONFIGURATION REGISTER mask definitions
/*	RO bits denoted with 0
/*	WR bits denoted with 1
*/
#define DC287_MCFID      0x00000000	/* Configuration ID */
#define DC287_MCFCS      0x00000045     /* Configuration Command Status */
#define DC287_MCFRV      0x00000000     /* Configuration Revision */
#define DC287_MCFLT      0x0000FF00     /* Configuration Latency Timer */
#define DC287_MCBIO      0xFFFFFC01     /* Configuration Base IO Address */

/* DC287 PCI CONFIGURATION REGISTER init values
*/
#define DC287_ICFID      0x00021011	/* Configuration ID */
#define DC287_ICFCS      0x02800000     /* Configuration Command Status */
#define DC287_ICFRV      0x02000020     /* Configuration Revision */
#define DC287_ICFLT      0x00000000     /* Configuration Latency Timer */
#define DC287_ICBIO      0x00010001     /* Configuration Base IO Address */

/* DC287 PCI CONFIGURATION REGISTER default program values 
*/
#define DC287_DCFID      0x00000000	/* Configuration ID */
#define DC287_DCFCS      0x00000045	/* Configuration Command Status */
#define DC287_DCFRV      0x00000020	/* Configuration Revision */
#define DC287_DCFLT      0x00000800	/* Configuration Latency Timer 8 PCI Ticks */
#define DC287_DCBIO      (DC287_PCI_BASE|DC287_DCBIO_IO) 

/* rsvd 14h-3Ch */

