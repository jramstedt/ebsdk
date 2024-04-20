/*
	FILE:   i82378_def.h
    
    AUTHOR:     R. Scott
    
    REVISION HISTORY:
	27-aug-95      Robert Scott    Created
    
    DESCRIPTION:
	An attempt to define register and contents for the Intel 82378 System I/O chip    

*/

/* PCI Control */
#define I82378_PCICTL		0x40	/* Register default value: 0x20, IAE */
#define I82378_DLBC_1B		0x00	/* DMA Line Buffer Configuration - Single Transaction Mode */
#define	I82378_DLBC_8B		0x01	/* DMA Line Buffer Configuration - 8-byte Mode */
#define I82378_ILBC_1B		0x00	/* ISA Master Line Buffer Configuration - Single Transaction Mode */
#define I82378_ILBC_8B		0x02	/* ISA Master Line Buffer Configuration - 8-byte Mode */
#define I82378_PPBE		0x04	/* PCI Posted Write Buffer Enable - 0=Disable, 1=Enable */
#define I82378_SDSP_SLOW	0x00   	/* Subtractive Decoding Sample Point - Slow Sample Point */
#define I82378_SDSP_TYP		0x08	/* Subtractive Decoding Sample Point - Typical Sample Point */
#define I82378_SDSP_FAST	0x10	/* Subtractive Decoding Sample Point - Fast Sample Point */
#define I82378_IAE		0x20	/* Interrupt Acknowledge Enable - 0=ignore INTA, 1=respond to INTA */

/* PCI Arbiter Control */
#define I82378_PCIACTL		0x41	/* Register default value:  0x00 */
#define I82378_GAT		0x01	/* Guaranteed Access Time - 0=Disable, 1=Enable */
#define I82378_BL		0x02	/* Bus Lock - 0=Resource lock is enabled, 1=Bus lock is enabled */
#define I82378_BP		0x04	/* Bus Park - 0=SIO drives AD, C/BE#, PAR, 1=SIO parks CPUREQ# on PCI bus while idle */
#define I82378_MRT_DIS		0x00	/* Master Retry Timer - Timer disabled */
#define I82378_MRT_16		0x08	/* Master Retry Timer - Retry after 16 PCICLK's */
#define I82378_MRT_32		0x10	/* Master Retry Timer - Retry after 32 PCICLK's */
#define I82378_MRT_64		0x18	/* Master Retry Timer - Retry after 64 PCICLK's */

/* ISA Address Decoder Control */
#define I82378_ISAADECODE	0x48	/* Register default value:  0x01 */
#define I82378_TOP_16M		0xf0 	/* Top of "main memory region", 0-1M, 1-2M,...,15-16M */
#define I82378_896K		0x08 	/* Region enable:  896-960 K, Low BIOS */
#define I82378_640K		0x04	/* Region enable:  640-768 K, VGA memory */
#define I82378_512K		0x02	/* region enable:  512-640 K */
#define I82378_0K		0x01	/* region enable:  0-512K */

/* ISA Controller Recovery Timer */
#define I82378_ICRT		0x4c	/* Register Default value:  0x56 */
#define I82378_16RE		0x04
#define I82378_16RT1		0x01
#define I82378_16RT2		0x02
#define I82378_16RT3		0x03
#define I82378_16RT4		0x00
#define I82378_8RE		0x40
#define I82378_8RT8		0x00
#define I82378_8RT2		0x10

/* ISA Clock Divisor */
#define	I82378_ISACLKDIV	0x4d	/* Register default value:  0x40 */
#define I82378_DIV_33		0x00
#define I82378_DIV_25		0x01
#define I82378_RSTDRV		0x08
#define I82378_IRQ12M		0x10
#define I82378_COPROC		0x20
#define I82378_BIOSEN		0x40

/* Utility Bus Chip Select Enable A */
#define I82378_UBCSEA		0x4e	/* Register default value:  0x07 */
#define I82378_EBIOSE		0x80	/* Extended BIOS Enable.  */
#define I82378_LBIOSE		0x40	/* Lower BIOS Enable. */
#define I82378_IDEDEC		0x10	/* IDE Decode Enable */
#define I82378_FDE_3F0		0x08	/* Floppy addresses enabled: 0x3f0, 0x3f1 */
#define I82378_FDE_3F2		0x04	/* Floppy addresses enabled: 0x3f2-0x3f7 */
#define I82378_FDE_3FX		I82378_FDE_3F0 | I82378_FDE_3F2	 /*  0x3f0-0x3f7 */
#define I82378_FDE_370		0x28	/* Floppy addresses enabled: 0x370, 0x371 */
#define I82378_FDE_372		0x24	/* Floppy addresses enabled: 0x372-0x37f */
#define I82378_FDE_37X		I82378_FDE_370 | I82378_FDE_372  /* 0x370-0x37f */
#define I82378_KBDENB		0x02	/* Keyboard Controller Address Location Enable */
#define I82378_RTCENB		0x01	/* RTC Address location enable */

/* Utility Bus Chip Select Enable B */
#define	I82378_UBCSEB		0x4f	/* Register default value:  0x4f */
#define I82378_CRAMDE		0x80	/* Configuration RAM Decode Enable */
#define I82378_P92ENB		0x40	/* Port 92 Enable */
#define I82378_PPE_LPT1		0x00	/* Parallel Port Enable - LPT1, 0x3bc-0x3bf */
#define I82378_PPE_LPT2		0x10	/*                        LPT2, 0x378-0x37f */
#define I82378_PPE_LPT3		0x20	/*                        LPT3, 0x278-0x27f */
#define I82378_PPE_DIS		0x30	/*                        Disable */
#define I82378_SPB_COM1		0x00	/* Serial Port B Enable - COM1, 0x3F8-0x3ff */
#define I82378_SPB_COM2		0x04	/*                        COM2, 0x2f8-0x2ff */
#define I82378_SPB_DIS		0x0c	/*                        Disable */
#define I82378_SPA_COM1		0x00	/* Serial Port A Enable - COM1, 0x3f8-0x3ff */
#define I82378_SPA_COM2		0x01	/*                        COM2, 0x2f8-0x2ff */
#define I82378_SPA_DIS		0x03	/*                        Disable */

    /* Timer Control Word Register
       0	0=binary countdown / 1=BCD 			I82378_BIN & I82378_BCD

       3 2 1    Mode    Function
       0 0 0    0       Out signal on end of count (=0)         I82378_SW1SHOT
       0 0 1    1       Hardware retriggerable one-shot         I82378_HW1SHOT
       X 1 0    2       Rate generator (divide by n counter)    I82378_RATEGEN
       X 1 1    3       Square wave output                      I82378_SQUARE
       1 0 0    4       Software triggered strobe               I82378_SWTRIGS
       1 0 1    5       Hardware triggered strobe               I82378_HWTRIGS

       5 4     Function
       0 0     Counter Latch Command                            I82378_COUNTLATCH
       0 1     R/W LSB                                          I82378_COUNTLSB
       1 0     R/W MSB                                          I82378_COUNTMSB
       1 1     R/W LSB then MSB                                 I82378_COUNTLSBMSB

       7 6     Counter select
       0 0     Counter 0   					I82378_COUNT0
       0 1     Counter 1                                        I82378_COUNT1
       1 0     Counter 2                                        I82378_COUNT2
       1 1     Read Back Command                                I82378_READBACK
    */

#define I82378_TCTLWREG		0x43			/* Timer Control Word Register */
#define I82378_COUNTER0		0x40			/* Timer Counter 1 - Counter 0 count */ 
#define I82378_COUNTER1		0x41			/* Timer Counter 1 - Counter 1 count */ 
#define I82378_COUNTER2		0x42			/* Timer Counter 1 - Counter 2 count */
 
#define I82378_BIN	0x00
#define I82378_BCD	0x01
#define	I82378_SW1SHOT	0x00
#define	I82378_HW1SHOT 	0x02
#define	I82378_RATEGEN	0x04
#define	I82378_SQUARE	0x06
#define	I82378_SWTRIGS	0x08
#define	I82378_HWTRIGS	0x0A
#define	I82378_COUNTLATCH	0x00
#define	I82378_COUNTLSB		0x10
#define	I82378_COUNTMSB		0x20
#define	I82378_COUNTLSBMSB	0x30
#define	I82378_COUNT0		0x00
#define	I82378_COUNT1		0x40
#define	I82378_COUNT2		0x80
#define	I82378_READBACK		0xC0

/*** Interrupt Controller Register ***/
/* Initialization Command Word 1 Register */
#define I82378_C1_ICW1		0x20		/* Interrupt controller 1:  I82378_C1_... */
#define I82378_C2_ICW1		0xA0            /* Interrupt controller 2:  I82378_C2_... */
#define I82378_ICWSEL		0x10		/* 1=ICW Select, 0=OCW Select */
#define I82378_LTIM_EDGE	0x00		/* Edge Triggered Mode */
#define I82378_LTIM_LEVEL	0x08		/* Level Triggered Mode */
#define I82378_IC4		0x01		/* Must be 1, ICW4 must be programmed */

/* Initialization Command Word 2 Register */
#define I82378_C1_ICW2		0x21
#define I82378_C2_ICW2		0xA1
#define I82378_VECTOR_BASE_0	0x00
#define I82378_VECTOR_BASE_1	0x08

/* Initialization Command Word 3 Register */
#define I82378_C1_ICW3		0x21
#define I82378_C1_CICIRQ	0x04

#define I82378_C2_ICW3		0xA1
#define I82378_C2_SLAVEID	0x02		/* required! */

/* Initialization Command Word 4 Register */
#define I82378_C1_ICW4		0x21
#define I82378_C2_ICW4		0xA1
#define I82378_SFNM		0x10		/* Special Fully Nested Mode */
#define I82378_BUFM		0x08		/* Buffered Mode */
#define I82378_MSBM		0x04		/* Master/Slave in Buffered Mode - should always be used */
#define I82378_AEOI		0x02		/* Automatic End of Interrupt */
#define I82378_MM		0x01		/* Microprocessor Mode - should always be used */

/* Operational Control Word 1 Register */
#define I82378_C1_OCW1		0x21		/* Interrupt controller 1: I82378_C1_... */
#define I82378_C2_OCW1		0xa1		/* Interrupt controller 2: I82378_C2_... */
#define I82378_IRQ0		0x01		/* Interrupt mask values: 1=IRQ masked off, 0=IRQ enabled */
#define I82378_IRQ1		0x02
#define I82378_IRQ2		0x04
#define I82378_IRQ3		0x08
#define I82378_IRQ4		0x10
#define I82378_IRQ5		0x20
#define I82378_IRQ6		0x40
#define I82378_IRQ7		0x80
#define I82378_IRQALL		0xff

/* Operational Control Word 2 Register */
#define I82378_C1_OCW2		0x20
#define I82378_C2_OCW2		0xa0
#define I82378_EOI_NS		0x20		/* Non-specific EOI command */
#define I82378_EOI_S		0x60		/* Specific EOI command */
#define I82378_EOI_RNS		0xa0		/* Rotate on Non-specific EOI command */
#define I82378_EOI_RAS		0x80		/* Rotate on Auto EOI Mode (Set) */
#define I82378_EOI_RAC		0x00		/* Rotate on Auto EOI Mode (Clear) */
#define I82378_EOI_RS		0xe0		/* Rotate on specific EOI command */
#define I82378_EOI_SP		0xc0		/* Set Priority Command */
#define I82378_EOI_NOP		0x40		/* No Operation */

/*** NMI Enable and Real-Time Clock Address Register ***/
#define I82378_NMIRTC		0x70		/* default value: 1xxxxxxxB */
#define I82378_NMIE		0x80		/* NMI Enable */

