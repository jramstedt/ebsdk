/* File:	smcc669_def.h
 *
 * Copyright (C) 1997 by
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
 * Abstract:	
 *
 *	This file contains header definitions for the SMC37c669 
 *	Super I/O controller. 
 *
 * Author:	
 *
 *	Eric Rasmussen
 *
 * Modification History:
 *
 *	er	28-Jan-1997	Initial Entry
 */

#ifndef __SMC37c669_H
#define __SMC37c669_H

/*
** Macros for handling device IRQs
**
** The mask acts as a flag used in mapping actual ISA IRQs (0 - 15) 
** to device IRQs (A - H).
*/
#define SMC37c669_DEVICE_IRQ_MASK	0x80000000
#define SMC37c669_DEVICE_IRQ( __i )	( ( SMC37c669_DEVICE_IRQ_MASK ) | ( __i ) )
#define SMC37c669_IS_DEVICE_IRQ( __i )	( ( ( __i ) & ( SMC37c669_DEVICE_IRQ_MASK ) ) == ( SMC37c669_DEVICE_IRQ_MASK ) )
#define SMC37c669_RAW_DEVICE_IRQ( __i )	( ( __i ) & ~( SMC37c669_DEVICE_IRQ_MASK ) )
 
/*
** Macros for handling device DRQs
**
** The mask acts as a flag used in mapping actual ISA DMA
** channels to device DMA channels (A - C).
*/
#define SMC37c669_DEVICE_DRQ_MASK	0x80000000
#define SMC37c669_DEVICE_DRQ( __d )	( ( SMC37c669_DEVICE_DRQ_MASK ) | ( __d ) )
#define SMC37c669_IS_DEVICE_DRQ( __d )	( ( ( __d ) & ( SMC37c669_DEVICE_DRQ_MASK ) ) == ( SMC37c669_DEVICE_DRQ_MASK ) )
#define SMC37c669_RAW_DEVICE_DRQ( __d )	( ( __d ) & ~( SMC37c669_DEVICE_DRQ_MASK ) )

#define SMC37c669_DEVICE_ID	0x3

/*
** SMC37c669 Device Function Definitions
*/
#define SERIAL_0	0
#define SERIAL_1	1
#define PARALLEL_0	2
#define FLOPPY_0	3
#define IDE_0		4
#define NUM_FUNCS	5

/*
** Default Device Function Mappings
*/
#define COM1_BASE	0x3F8
#define COM1_IRQ	4
#define COM2_BASE	0x2F8
#define COM2_IRQ	3
#define PARP_BASE	0x3BC
#define PARP_IRQ	7
#define PARP_DRQ	3
#define FDC_BASE	0x3F0
#define FDC_IRQ		6
#define FDC_DRQ		2

/*
** Configuration On/Off Key Definitions
*/
#define SMC37c669_CONFIG_ON_KEY		0x55
#define SMC37c669_CONFIG_OFF_KEY	0xAA

/*
** SMC 37c669 Device IRQs
*/
#define SMC37c669_DEVICE_IRQ_A	    ( SMC37c669_DEVICE_IRQ( 0x01 ) )
#define SMC37c669_DEVICE_IRQ_B	    ( SMC37c669_DEVICE_IRQ( 0x02 ) )
#define SMC37c669_DEVICE_IRQ_C	    ( SMC37c669_DEVICE_IRQ( 0x03 ) )
#define SMC37c669_DEVICE_IRQ_D	    ( SMC37c669_DEVICE_IRQ( 0x04 ) )
#define SMC37c669_DEVICE_IRQ_E	    ( SMC37c669_DEVICE_IRQ( 0x05 ) )
#define SMC37c669_DEVICE_IRQ_F	    ( SMC37c669_DEVICE_IRQ( 0x06 ) )
/*      SMC37c669_DEVICE_IRQ_G	    *** RESERVED ***/
#define SMC37c669_DEVICE_IRQ_H	    ( SMC37c669_DEVICE_IRQ( 0x08 ) )

/*
** SMC 37c669 Device DMA Channel Definitions
*/
#define SMC37c669_DEVICE_DRQ_A		    ( SMC37c669_DEVICE_DRQ( 0x01 ) )
#define SMC37c669_DEVICE_DRQ_B		    ( SMC37c669_DEVICE_DRQ( 0x02 ) )
#define SMC37c669_DEVICE_DRQ_C		    ( SMC37c669_DEVICE_DRQ( 0x03 ) )

/*
** Configuration Register Index Definitions
*/
#define SMC37c669_CR00_INDEX	    0x00
#define SMC37c669_CR01_INDEX	    0x01
#define SMC37c669_CR02_INDEX	    0x02
#define SMC37c669_CR03_INDEX	    0x03
#define SMC37c669_CR04_INDEX	    0x04
#define SMC37c669_CR05_INDEX	    0x05
#define SMC37c669_CR06_INDEX	    0x06
#define SMC37c669_CR07_INDEX	    0x07
#define SMC37c669_CR08_INDEX	    0x08
#define SMC37c669_CR09_INDEX	    0x09
#define SMC37c669_CR0A_INDEX	    0x0A
#define SMC37c669_CR0B_INDEX	    0x0B
#define SMC37c669_CR0C_INDEX	    0x0C
#define SMC37c669_CR0D_INDEX	    0x0D
#define SMC37c669_CR0E_INDEX	    0x0E
#define SMC37c669_CR0F_INDEX	    0x0F
#define SMC37c669_CR10_INDEX	    0x10
#define SMC37c669_CR11_INDEX	    0x11
#define SMC37c669_CR12_INDEX	    0x12
#define SMC37c669_CR13_INDEX	    0x13
#define SMC37c669_CR14_INDEX	    0x14
#define SMC37c669_CR15_INDEX	    0x15
#define SMC37c669_CR16_INDEX	    0x16
#define SMC37c669_CR17_INDEX	    0x17
#define SMC37c669_CR18_INDEX	    0x18
#define SMC37c669_CR19_INDEX	    0x19
#define SMC37c669_CR1A_INDEX	    0x1A
#define SMC37c669_CR1B_INDEX	    0x1B
#define SMC37c669_CR1C_INDEX	    0x1C
#define SMC37c669_CR1D_INDEX	    0x1D
#define SMC37c669_CR1E_INDEX	    0x1E
#define SMC37c669_CR1F_INDEX	    0x1F
#define SMC37c669_CR20_INDEX	    0x20
#define SMC37c669_CR21_INDEX	    0x21
#define SMC37c669_CR22_INDEX	    0x22
#define SMC37c669_CR23_INDEX	    0x23
#define SMC37c669_CR24_INDEX	    0x24
#define SMC37c669_CR25_INDEX	    0x25
#define SMC37c669_CR26_INDEX	    0x26
#define SMC37c669_CR27_INDEX	    0x27
#define SMC37c669_CR28_INDEX	    0x28
#define SMC37c669_CR29_INDEX	    0x29

/*
** Configuration Register Alias Definitions
*/
#define SMC37c669_DEVICE_ID_INDEX		    SMC37c669_CR0D_INDEX
#define SMC37c669_DEVICE_REVISION_INDEX		    SMC37c669_CR0E_INDEX
#define SMC37c669_FDC_BASE_ADDRESS_INDEX	    SMC37c669_CR20_INDEX
#define SMC37c669_IDE_BASE_ADDRESS_INDEX	    SMC37c669_CR21_INDEX
#define SMC37c669_IDE_ALTERNATE_ADDRESS_INDEX	    SMC37c669_CR22_INDEX
#define SMC37c669_PARALLEL0_BASE_ADDRESS_INDEX	    SMC37c669_CR23_INDEX
#define SMC37c669_SERIAL0_BASE_ADDRESS_INDEX	    SMC37c669_CR24_INDEX
#define SMC37c669_SERIAL1_BASE_ADDRESS_INDEX	    SMC37c669_CR25_INDEX
#define SMC37c669_PARALLEL_FDC_DRQ_INDEX	    SMC37c669_CR26_INDEX
#define SMC37c669_PARALLEL_FDC_IRQ_INDEX	    SMC37c669_CR27_INDEX
#define SMC37c669_SERIAL_IRQ_INDEX		    SMC37c669_CR28_INDEX

/*
** Configuration Register Definitions
**
** The INDEX (write only) and DATA (read/write) ports are effective 
** only when the chip is in the Configuration State.
*/
typedef struct _SMC37c669_CONFIG_REGS {
    unsigned char index_port;
    unsigned char data_port;
} SMC37c669_CONFIG_REGS;

/*
** CR00 - default value 0x28
**
**  IDE_EN (CR00<1:0>):
**	0x - 30ua pull-ups on nIDEEN, nHDCS0, NHDCS1
**	11 - IRQ_H available as IRQ output,
**	     IRRX2, IRTX2 available as alternate IR pins
**	10 - nIDEEN, nHDCS0, nHDCS1 used to control IDE
**
**  VALID (CR00<7>):
**	A high level on this software controlled bit can
**	be used to indicate that a valid configuration
**	cycle has occurred.  The control software must
**	take care to set this bit at the appropriate times.
**	Set to zero after power up.  This bit has no
**	effect on any other hardware in the chip.
**
*/
typedef union _SMC37c669_CR00 {
    unsigned char as_uchar;
    struct {
    	unsigned ide_en : 2;	    /* See note above		*/
	unsigned reserved1 : 1;	    /* RAZ			*/
	unsigned fdc_pwr : 1;	    /* 1 = supply power to FDC  */
	unsigned reserved2 : 3;	    /* Read as 010b		*/
	unsigned valid : 1;	    /* See note above		*/
    }	by_field;
} SMC37c669_CR00;

/*
** CR01 - default value 0x9C
*/
typedef union _SMC37c669_CR01 {
    unsigned char as_uchar;
    struct {
    	unsigned reserved1 : 2;	    /* RAZ			    */
	unsigned ppt_pwr : 1;	    /* 1 = supply power to PPT	    */
	unsigned ppt_mode : 1;	    /* 1 = Printer mode, 0 = EPP    */
	unsigned reserved2 : 1;	    /* Read as 1		    */
	unsigned reserved3 : 2;	    /* RAZ			    */
	unsigned lock_crx: 1;	    /* Lock CR00 - CR18		    */
    }	by_field;
} SMC37c669_CR01;

/*
** CR02 - default value 0x88
*/
typedef union _SMC37c669_CR02 {
    unsigned char as_uchar;
    struct {
    	unsigned reserved1 : 3;	    /* RAZ			    */
	unsigned uart1_pwr : 1;	    /* 1 = supply power to UART1    */
	unsigned reserved2 : 3;	    /* RAZ			    */
	unsigned uart2_pwr : 1;	    /* 1 = supply power to UART2    */
    }	by_field;
} SMC37c669_CR02;
 
/*
** CR03 - default value 0x78
**
**  CR03<7>	CR03<2>	    Pin 94
**  -------	-------	    ------
**     0	   X	    DRV2 (input)
**     1	   0	    ADRX
**     1	   1	    IRQ_B
**
**  CR03<6>	CR03<5>	    Op Mode
**  -------	-------	    -------
**     0	   0	    Model 30
**     0	   1	    PS/2
**     1	   0	    Reserved
**     1	   1	    AT Mode
*/
typedef union _SMC37c669_CR03 {
    unsigned char as_uchar;
    struct {
    	unsigned pwrgd_gamecs : 1;  /* 1 = PWRGD, 0 = GAMECS	    */
	unsigned fdc_mode2 : 1;	    /* 1 = Enhanced Mode 2	    */
	unsigned pin94_0 : 1;	    /* See note above		    */
	unsigned reserved1 : 1;	    /* RAZ			    */
	unsigned drvden : 1;	    /* 1 = high, 0 - output	    */
	unsigned op_mode : 2;	    /* See note above		    */
	unsigned pin94_1 : 1;	    /* See note above		    */
    }	by_field;
} SMC37c669_CR03;
 
/*
** CR04 - default value 0x00
**
**  PP_EXT_MODE:
**	If CR01<PP_MODE> = 0 and PP_EXT_MODE =
**	    00 - Standard and Bidirectional
**	    01 - EPP mode and SPP
**	    10 - ECP mode
**		 In this mode, 2 drives can be supported
**		 directly, 3 or 4 drives must use external
**		 4 drive support.  SPP can be selected
**		 through the ECR register of ECP as mode 000.
**	    11 - ECP mode and EPP mode
**		 In this mode, 2 drives can be supported
**		 directly, 3 or 4 drives must use external
**		 4 drive support.  SPP can be selected
**		 through the ECR register of ECP as mode 000.
**		 In this mode, EPP can be selected through
**		 the ECR register of ECP as mode 100.
**
**  PP_FDC:
**	00 - Normal
**	01 - PPFD1
**	10 - PPFD2
**	11 - Reserved
**
**  MIDI1:
**	Serial Clock Select: 
**	    A low level on this bit disables MIDI support,
**	    clock = divide by 13.  A high level on this 
**	    bit enables MIDI support, clock = divide by 12.
**
**	MIDI operates at 31.25 Kbps which can be derived 
**	from 125 KHz (24 MHz / 12 = 2 MHz, 2 MHz / 16 = 125 KHz)
**
**  ALT_IO:
**	0 - Use pins IRRX, IRTX
**	1 - Use pins IRRX2, IRTX2
**
**	If this bit is set, the IR receive and transmit
**	functions will not be available on pins 25 and 26
**	unless CR00<IDE_EN> = 11.
*/
typedef union _SMC37c669_CR04 {
    unsigned char as_uchar;
    struct {
    	unsigned ppt_ext_mode : 2;  /* See note above		    */
	unsigned ppt_fdc : 2;	    /* See note above		    */
	unsigned midi1 : 1;	    /* See note above		    */
	unsigned midi2 : 1;	    /* See note above		    */
	unsigned epp_type : 1;	    /* 0 = EPP 1.9, 1 = EPP 1.7	    */
	unsigned alt_io : 1;	    /* See note above		    */
    }	by_field;
} SMC37c669_CR04;
 
/*
** CR05 - default value 0x00
**
**  DEN_SEL:
**	00 - Densel output normal
**	01 - Reserved
**	10 - Densel output 1
**	11 - Densel output 0
**
*/
typedef union _SMC37c669_CR05 {
    unsigned char as_uchar;
    struct {
    	unsigned reserved1 : 2;	    /* RAZ					*/
	unsigned fdc_dma_mode : 1;  /* 0 = burst, 1 = non-burst			*/
	unsigned den_sel : 2;	    /* See note above				*/
	unsigned swap_drv : 1;	    /* Swap the FDC motor selects		*/
	unsigned extx4 : 1;	    /* 0 = 2 drive, 1 = external 4 drive decode	*/
	unsigned reserved2 : 1;	    /* RAZ					*/
    }	by_field;
} SMC37c669_CR05;
 
/*
** CR06 - default value 0xFF
*/
typedef union _SMC37c669_CR06 {
    unsigned char as_uchar;
    struct {
    	unsigned floppy_a : 2;	    /* Type of floppy drive A	    */
	unsigned floppy_b : 2;	    /* Type of floppy drive B	    */
	unsigned floppy_c : 2;	    /* Type of floppy drive C	    */
	unsigned floppy_d : 2;	    /* Type of floppy drive D	    */
    }	by_field;
} SMC37c669_CR06;
 
/*
** CR07 - default value 0x00
**
**  Auto Power Management CR07<7:4>:
**	0 - Auto Powerdown disabled (default)
**	1 - Auto Powerdown enabled
**
**	This bit is reset to the default state by POR or
**	a hardware reset.
**
*/
typedef union _SMC37c669_CR07 {
    unsigned char as_uchar;
    struct {
    	unsigned floppy_boot : 2;   /* 0 = A:, 1 = B:		    */
	unsigned reserved1 : 2;	    /* RAZ			    */
	unsigned ppt_en : 1;	    /* See note above		    */
	unsigned uart1_en : 1;	    /* See note above		    */
	unsigned uart2_en : 1;	    /* See note above		    */
	unsigned fdc_en : 1;	    /* See note above		    */
    }	by_field;
} SMC37c669_CR07;
 
/*
** CR08 - default value 0x00
*/
typedef union _SMC37c669_CR08 {
    unsigned char as_uchar;
    struct {
    	unsigned zero : 4;	    /* 0			    */
	unsigned addrx7_4 : 4;	    /* ADR<7:3> for ADRx decode	    */
    }	by_field;
} SMC37c669_CR08;
 
/*
** CR09 - default value 0x00
**
**  ADRx_CONFIG:
**	00 - ADRx disabled
**	01 - 1 byte decode A<3:0> = 0000b
**	10 - 8 byte block decode A<3:0> = 0XXXb
**	11 - 16 byte block decode A<3:0> = XXXXb
**
*/
typedef union _SMC37c669_CR09 {
    unsigned char as_uchar;
    struct {
    	unsigned adra8 : 3;	    /* ADR<10:8> for ADRx decode    */
	unsigned reserved1 : 3;
	unsigned adrx_config : 2;   /* See note above		    */
    }	by_field;
} SMC37c669_CR09;
 
/*
** CR0A - default value 0x00
*/
typedef union _SMC37c669_CR0A {
    unsigned char as_uchar;
    struct {
    	unsigned ecp_fifo_threshold : 4;
	unsigned reserved1 : 4;
    }	by_field;
} SMC37c669_CR0A;
 
/*
** CR0B - default value 0x00
*/
typedef union _SMC37c669_CR0B {
    unsigned char as_uchar;
    struct {
    	unsigned fdd0_drtx : 2;	    /* FDD0 Data Rate Table	    */
	unsigned fdd1_drtx : 2;	    /* FDD1 Data Rate Table	    */
	unsigned fdd2_drtx : 2;	    /* FDD2 Data Rate Table	    */
	unsigned fdd3_drtx : 2;	    /* FDD3 Data Rate Table	    */
    }	by_field;
} SMC37c669_CR0B;
 
/*
** CR0C - default value 0x00
**
**  UART2_MODE:
**	000 - Standard (default)
**	001 - IrDA (HPSIR)
**	010 - Amplitude Shift Keyed IR @500 KHz
**	011 - Reserved
**	1xx - Reserved
**
*/
typedef union _SMC37c669_CR0C {
    unsigned char as_uchar;
    struct {
    	unsigned uart2_rcv_polarity : 1;    /* 1 = invert RX		*/
	unsigned uart2_xmit_polarity : 1;   /* 1 = invert TX		*/
	unsigned uart2_duplex : 1;	    /* 1 = full, 0 = half	*/
	unsigned uart2_mode : 3;	    /* See note above		*/
	unsigned uart1_speed : 1;	    /* 1 = high speed enabled	*/
	unsigned uart2_speed : 1;	    /* 1 = high speed enabled	*/
    }	by_field;
} SMC37c669_CR0C;
 
/*
** CR0D - default value 0x03
**
**  Device ID Register - read only
*/
typedef union _SMC37c669_CR0D {
    unsigned char as_uchar;
    struct {
    	unsigned device_id : 8;	    /* Returns 0x3 in this field    */
    }	by_field;
} SMC37c669_CR0D;
 
/*
** CR0E - default value 0x02
**
**  Device Revision Register - read only
*/
typedef union _SMC37c669_CR0E {
    unsigned char as_uchar;
    struct {
    	unsigned device_rev : 8;    /* Returns 0x2 in this field    */
    }	by_field;
} SMC37c669_CR0E;
 
/*
** CR0F - default value 0x00
*/
typedef union _SMC37c669_CR0F {
    unsigned char as_uchar;
    struct {
    	unsigned test0 : 1;	    /* Reserved - set to 0	    */
	unsigned test1 : 1;	    /* Reserved - set to 0	    */
	unsigned test2 : 1;	    /* Reserved - set to 0	    */
	unsigned test3 : 1;	    /* Reserved - set t0 0	    */
	unsigned test4 : 1;	    /* Reserved - set to 0	    */
	unsigned test5 : 1;	    /* Reserved - set t0 0	    */
	unsigned test6 : 1;	    /* Reserved - set t0 0	    */
	unsigned test7 : 1;	    /* Reserved - set to 0	    */
    }	by_field;
} SMC37c669_CR0F;
 
/*
** CR10 - default value 0x00
*/
typedef union _SMC37c669_CR10 {
    unsigned char as_uchar;
    struct {
    	unsigned reserved1 : 3;	     /* RAZ			    */
	unsigned pll_gain : 1;	     /* 1 = 3V, 2 = 5V operation    */
	unsigned pll_stop : 1;	     /* 1 = stop PLLs		    */
	unsigned ace_stop : 1;	     /* 1 = stop UART clocks	    */
	unsigned pll_clock_ctrl : 1; /* 0 = 14.318 MHz, 1 = 24 MHz  */
	unsigned ir_test : 1;	     /* Enable IR test mode	    */
    }	by_field;
} SMC37c669_CR10;
 
/*
** CR11 - default value 0x00
*/
typedef union _SMC37c669_CR11 {
    unsigned char as_uchar;
    struct {
    	unsigned ir_loopback : 1;   /* Internal IR loop back		    */
	unsigned test_10ms : 1;	    /* Test 10ms autopowerdown FDC timeout  */
	unsigned reserved1 : 6;	    /* RAZ				    */
    }	by_field;
} SMC37c669_CR11;
 
/*
** CR12 - CR1D are reserved registers
*/

/*
** CR1E - default value 0x80
**
**  GAMECS:
**	00 - GAMECS disabled
**	01 - 1 byte decode ADR<3:0> = 0001b
**	10 - 8 byte block decode ADR<3:0> = 0XXXb
**	11 - 16 byte block decode ADR<3:0> = XXXXb
**
*/
typedef union _SMC37c66_CR1E {
    unsigned char as_uchar;
    struct {
    	unsigned gamecs_config: 2;   /* See note above		    */
	unsigned gamecs_addr9_4 : 6; /* GAMECS Addr<9:4>	    */
    }	by_field;
} SMC37c669_CR1E;
 
/*
** CR1F - default value 0x00
**
**  DT0 DT1 DRVDEN0 DRVDEN1 Drive Type
**  --- --- ------- ------- ----------
**   0   0  DENSEL  DRATE0  4/2/1 MB 3.5"
**                          2/1 MB 5.25"
**                          2/1.6/1 MB 3.5" (3-mode)
**   0   1  DRATE1  DRATE0
**   1   0  nDENSEL DRATE0  PS/2
**   1   1  DRATE0  DRATE1
**
**  Note: DENSEL, DRATE1, and DRATE0 map onto two output
**	  pins - DRVDEN0 and DRVDEN1.
**
*/
typedef union _SMC37c669_CR1F {
    unsigned char as_uchar;
    struct {
    	unsigned fdd0_drive_type : 2;	/* FDD0 drive type	    */
	unsigned fdd1_drive_type : 2;	/* FDD1 drive type	    */
	unsigned fdd2_drive_type : 2;	/* FDD2 drive type	    */
	unsigned fdd3_drive_type : 2;	/* FDD3 drive type	    */
    }	by_field;
} SMC37c669_CR1F;
 
/*
** CR20 - default value 0x3C
**
**  FDC Base Address Register
**	- To disable this decode set Addr<9:8> = 0
**	- A<10> = 0, A<3:0> = 0XXXb to access.
**
*/
typedef union _SMC37c669_CR20 {
    unsigned char as_uchar;
    struct {
    	unsigned zero : 2;	    /* 0			    */
	unsigned addr9_4 : 6;	    /* FDC Addr<9:4>		    */
    }	by_field;
} SMC37c669_CR20;
 
/*
** CR21 - default value 0x3C
**
**  IDE Base Address Register
**	- To disable this decode set Addr<9:8> = 0
**	- A<10> = 0, A<3:0> = 0XXXb to access.
**
*/
typedef union _SMC37c669_CR21 {
    unsigned char as_uchar;
    struct {
    	unsigned zero : 2;	    /* 0			    */
	unsigned addr9_4 : 6;	    /* IDE Addr<9:4>		    */
    }	by_field;
} SMC37c669_CR21;
 
/*
** CR22 - default value 0x3D
**
**  IDE Alternate Status Base Address Register
**	- To disable this decode set Addr<9:8> = 0
**	- A<10> = 0, A<3:0> = 0110b to access.
**
*/
typedef union _SMC37c669_CR22 {
    unsigned char as_uchar;
    struct {
    	unsigned zero : 2;	    /* 0			    */
	unsigned addr9_4 : 6;	    /* IDE Alt Status Addr<9:4>	    */
    }	by_field;
} SMC37c669_CR22;
 
/*
** CR23 - default value 0x00
**
**  Parallel Port Base Address Register
**	- To disable this decode set Addr<9:8> = 0
**	- A<10> = 0 to access.
**	- If EPP is enabled, A<2:0> = XXXb to access.
**	  If EPP is NOT enabled, A<1:0> = XXb to access
**
*/
typedef union _SMC37c669_CR23 {
    unsigned char as_uchar;
    struct {
	unsigned addr9_2 : 8;	    /* Parallel Port Addr<9:2>	    */
    }	by_field;
} SMC37c669_CR23;
 
/*
** CR24 - default value 0x00
**
**  UART1 Base Address Register
**	- To disable this decode set Addr<9:8> = 0
**	- A<10> = 0, A<2:0> = XXXb to access.
**
*/
typedef union _SMC37c669_CR24 {
    unsigned char as_uchar;
    struct {
    	unsigned zero : 1;	    /* 0			    */
	unsigned addr9_3 : 7;	    /* UART1 Addr<9:3>		    */
    }	by_field;
} SMC37c669_CR24;
 
/*
** CR25 - default value 0x00
**
**  UART2 Base Address Register
**	- To disable this decode set Addr<9:8> = 0
**	- A<10> = 0, A<2:0> = XXXb to access.
**
*/
typedef union _SMC37c669_CR25 {
    unsigned char as_uchar;
    struct {
    	unsigned zero : 1;	    /* 0			    */
	unsigned addr9_3 : 7;	    /* UART2 Addr<9:3>		    */
    }	by_field;
} SMC37c669_CR25;
 
/*
** CR26 - default value 0x00
**
**  Parallel Port / FDC DMA Select Register
**
**  D3 - D0	  DMA
**  D7 - D4	Selected
**  -------	--------
**   0000	 None
**   0001	 DMA_A
**   0010	 DMA_B
**   0011	 DMA_C
**
*/
typedef union _SMC37c669_CR26 {
    unsigned char as_uchar;
    struct {
    	unsigned ppt_drq : 4;	    /* See note above		    */
	unsigned fdc_drq : 4;	    /* See note above		    */
    }	by_field;
} SMC37c669_CR26;
 
/*
** CR27 - default value 0x00
**
**  Parallel Port / FDC IRQ Select Register
**
**  D3 - D0	  IRQ
**  D7 - D4	Selected
**  -------	--------
**   0000	 None
**   0001	 IRQ_A
**   0010	 IRQ_B
**   0011	 IRQ_C
**   0100	 IRQ_D
**   0101	 IRQ_E
**   0110	 IRQ_F
**   0111	 Reserved
**   1000	 IRQ_H
**
**  Any unselected IRQ REQ is in tristate
**
*/
typedef union _SMC37c669_CR27 {
    unsigned char as_uchar;
    struct {
    	unsigned ppt_irq : 4;	    /* See note above		    */
	unsigned fdc_irq : 4;	    /* See note above		    */
    }	by_field;
} SMC37c669_CR27;
 
/*
** CR28 - default value 0x00
**
**  UART IRQ Select Register
**
**  D3 - D0	  IRQ
**  D7 - D4	Selected
**  -------	--------
**   0000	 None
**   0001	 IRQ_A
**   0010	 IRQ_B
**   0011	 IRQ_C
**   0100	 IRQ_D
**   0101	 IRQ_E
**   0110	 IRQ_F
**   0111	 Reserved
**   1000	 IRQ_H
**   1111	 share with UART1 (only for UART2)
**
**  Any unselected IRQ REQ is in tristate
**
**  To share an IRQ between UART1 and UART2, set
**  UART1 to use the desired IRQ and set UART2 to
**  0xF to enable sharing mechanism.
**
*/
typedef union _SMC37c669_CR28 {
    unsigned char as_uchar;
    struct {
    	unsigned uart2_irq : 4;	    /* See note above		    */
	unsigned uart1_irq : 4;	    /* See note above		    */
    }	by_field;
} SMC37c669_CR28;
 
/*
** CR29 - default value 0x00
**
**  IRQIN IRQ Select Register
**
**  D3 - D0	  IRQ
**  D7 - D4	Selected
**  -------	--------
**   0000	 None
**   0001	 IRQ_A
**   0010	 IRQ_B
**   0011	 IRQ_C
**   0100	 IRQ_D
**   0101	 IRQ_E
**   0110	 IRQ_F
**   0111	 Reserved
**   1000	 IRQ_H
**
**  Any unselected IRQ REQ is in tristate
**
*/
typedef union _SMC37c669_CR29 {
    unsigned char as_uchar;
    struct {
    	unsigned irqin_irq : 4;	    /* See note above		    */
	unsigned reserved1 : 4;	    /* RAZ			    */
    }	by_field;
} SMC37c669_CR29;
 
/*
** Aliases of Configuration Register formats (should match
** the set of index aliases).
**
** Note that CR24 and CR25 have the same format and are the
** base address registers for UART1 and UART2.  Because of
** this we only define 1 alias here - for CR24 - as the serial
** base address register.
**
** Note that CR21 and CR22 have the same format and are the
** base address and alternate status address registers for
** the IDE controller.  Because of this we only define 1 alias
** here - for CR21 - as the IDE address register.
**
*/
typedef SMC37c669_CR0D SMC37c669_DEVICE_ID_REGISTER;
typedef SMC37c669_CR0E SMC37c669_DEVICE_REVISION_REGISTER;
typedef SMC37c669_CR20 SMC37c669_FDC_BASE_ADDRESS_REGISTER;
typedef SMC37c669_CR21 SMC37c669_IDE_ADDRESS_REGISTER;
typedef SMC37c669_CR23 SMC37c669_PARALLEL_BASE_ADDRESS_REGISTER;
typedef SMC37c669_CR24 SMC37c669_SERIAL_BASE_ADDRESS_REGISTER;
typedef SMC37c669_CR26 SMC37c669_PARALLEL_FDC_DRQ_REGISTER;
typedef SMC37c669_CR27 SMC37c669_PARALLEL_FDC_IRQ_REGISTER;
typedef SMC37c669_CR28 SMC37c669_SERIAL_IRQ_REGISTER;

/*
** ISA/Device IRQ Translation Table Entry Definition
*/
typedef struct _SMC37c669_IRQ_TRANSLATION_ENTRY {
    int device_irq;
    int isa_irq;
} SMC37c669_IRQ_TRANSLATION_ENTRY;

/*
** ISA/Device DMA Translation Table Entry Definition
*/
typedef struct _SMC37c669_DRQ_TRANSLATION_ENTRY {
    int device_drq;
    int isa_drq;
} SMC37c669_DRQ_TRANSLATION_ENTRY;

/*
** External Interface Function Prototype Declarations
*/

SMC37c669_CONFIG_REGS *SMC37c669_detect( 
    void 
);

unsigned int SMC37c669_enable_device( 
    unsigned int func 
);

unsigned int SMC37c669_disable_device( 
    unsigned int func 
);

unsigned int SMC37c669_configure_device( 
    unsigned int func, 
    int port, 
    int irq, 
    int drq 
);

void SMC37c669_display_device_info( 
    void 
);

#endif	/* __SMC37c669_H */
