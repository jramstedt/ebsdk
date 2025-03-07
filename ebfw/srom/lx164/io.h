#ifndef	__IO_LOADED
#define	__IO_LOADED	1
/*
*****************************************************************************
**                                                                          *
**  Copyright © 1993							    *
**  by Digital Equipment Corporation, Maynard, Massachusetts.		    *
**                                                                          *
**  All Rights Reserved							    *
**                                                                          *
**  Permission  is  hereby  granted  to  use, copy, modify and distribute   *
**  this  software  and  its  documentation,  in  both  source  code  and   *
**  object  code  form,  and without fee, for the purpose of distribution   *
**  of this software  or  modifications  of this software within products   *
**  incorporating  an  integrated   circuit  implementing  Digital's  AXP   *
**  architecture,  regardless  of the  source of such integrated circuit,   *
**  provided that the  above copyright  notice and this permission notice   *
**  appear  in  all copies,  and  that  the  name  of  Digital  Equipment   *
**  Corporation  not  be  used  in advertising or publicity pertaining to   *
**  distribution of the  document  or  software without specific, written   *
**  prior permission.							    *
**                                                                          *
**  Digital  Equipment  Corporation   disclaims  all   warranties  and/or   *
**  guarantees  with  regard  to  this  software,  including  all implied   *
**  warranties of fitness for  a  particular purpose and merchantability,   *
**  and makes  no  representations  regarding  the use of, or the results   *
**  of the use of, the software and documentation in terms of correctness,  *
**  accuracy,  reliability,  currentness  or  otherwise;  and you rely on   *
**  the software, documentation and results solely at your own risk.	    *
**                                                                          *
**  AXP is a trademark of Digital Equipment Corporation.		    *
**                                                                          *
*****************************************************************************
**
*/

/*
 *  $Id: io.h,v 1.1.1.1 1998/12/29 21:36:12 paradis Exp $;
 *
 *  This file has been merged with EB164, PC164 and PC64's io.h files.
 *  Make sure any changes made to this new file work with these
 *  boards.
 *
 * $Log: io.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:12  paradis
 * Initial CVS checkin
 *
 * Revision 1.12  1997/05/22  18:26:57  fdh
 * Added conditions for the SX164.
 *
 * Revision 1.11  1997/05/08  09:30:20  fdh
 * Added conditions for the LX164.
 *
 * Revision 1.10  1996/04/30  18:35:09  cruz
 * Updated io.h so it can be shared with PC64/EB164/PC164 boards.
 *
 */

#include "ascitab.h"

#ifdef EB164
#define BASE_SHIFT	28              /* ALCOR Chipset.               */
#define MEM_CSR_BASE    0x875
#define CSR_BASE        0x874
#define PCI_IO_BASE	0x858
#define PCI_CONFIG_BASE 0x870
#define PCI_S_MEM_BASE	0x800
#define PCI_D_MEM_BASE	0x860
#ifdef SX164
#define PCI_D_FLASH_BASE	0x87C
#endif /* SX164 */

#if defined(LX164) || defined(SX164)
#define load_pyxis_adrtrn_base(register)	\
    lda	    register, pyxis_k_addr_trans(r31);	\
    sll	    register, pyxis_v_addr_trans, register;
#else
#define load_cia_adrtrn_base(register)		\
    lda	    register, cia_k_addr_trans(r31);	\
    sll	    register, cia_v_addr_trans, register;
#endif

#else
#define BASE_SHIFT	28              /* APECS Chipset.               */
#define MEM_CSR_BASE    0x18
#define CSR_BASE        0x1A
#define PCI_IO_BASE	0x1C
#define PCI_CONFIG_BASE 0x1E
#define PCI_S_MEM_BASE	0x20
#define PCI_D_MEM_BASE	0x30

#define epic_pbr1	0x100
#define epic_pbr2	0x120
#define epic_pmr1	0x140
#define epic_pmr2	0x160
#define epic_tbr1	0x0c0
#define epic_tbr2	0x0e0
#define epic_haxr0	0x180
#define epic_haxr1	0x1a0
#define epic_haxr2	0x1c0
#define epic_pmtr	0x1e0

#endif

#define BYTE_ENABLE_SHIFT 5

#define load_csr_base(register)		        \
    lda	    register, CSR_BASE(r31);	        \
    sll	    register, BASE_SHIFT, register;

#define load_mem_csr_base(register)		\
    lda	    register, MEM_CSR_BASE(r31);	\
    sll	    register, BASE_SHIFT, register;

#define load_pci_io_base(register)		\
    lda	    register, PCI_IO_BASE(r31);		\
    sll	    register, BASE_SHIFT, register;

#define load_pci_config_base(register)		\
    lda	    register, PCI_CONFIG_BASE(r31);	\
    sll	    register, BASE_SHIFT, register;

#define load_pci_s_mem_base(register)		\
    lda	    register, PCI_S_MEM_BASE(r31);	\
    sll	    register, BASE_SHIFT, register;

#define load_pci_d_mem_base(register)		\
    lda	    register, PCI_D_MEM_BASE(r31);	\
    sll	    register, BASE_SHIFT, register;

#ifdef SX164
#define load_pci_d_flash_base(register)		\
    lda	    register, PCI_D_FLASH_BASE(r31);	\
    sll	    register, BASE_SHIFT, register;
#endif

#define ROMSIZE 0x100000                /* Boot ROM size in bytes (1MB = 0x100000) */
#define FROM_BASE 0xFFF80000		/* base of flashrom in isa space	*/
#define FLASH_BIT_SWITCH_OFFSET 0x80000	/* half way through the flash		*/

/* String identifying CPU */
#define CPU_STRING_H b4_to_l(a_nl,a_cr,a_2,a_1)

#if defined(SX164)
#define CPU_STRING_L b4_to_l(a_1,a_6,a_4,a_P)

#elif defined(LX164) || defined(PC164)
#define CPU_STRING_L b4_to_l(a_1,a_6,a_4,a_A)

#elif defined(EB164)
#define CPU_STRING_L b4_to_l(a_1,a_6,a_4,a_nul)

#elif defined(PC64)
#define CPU_STRING_L b4_to_l(a_0,a_6,a_4,a_A)

#else
#define CPU_STRING_L b4_to_l(a_0,a_6,a_4,a_nul)
#undef ROMSIZE
#define ROMSIZE 0x80000	/* Boot ROM size in bytes (512KB = 0x80000) */
#define ROMINC 0x20	/* Adder used to address next byte in ROM */
#endif

#define SEXT(val) ((val ^ 0x8000) - 0x8000)
#define SEXT10b(val) ((val ^ 0x200) - 0x200)

/*
 * The LDLI macro is used to load a 32 bit sign extended
 * literal into the specified register.
 *
 * The lda and ldah instructions use a 16 bit sign-extended
 * displacement. To compensate for the sign-extension of the
 * lower word 0x8000 is added to the value before the lower
 * word is masked off.  Therefore if the load of the lower
 * word resulted in a sign-extension adding 0x8000 will cause
 * a carry to ripple through the upper word to remove the
 * sign-extension.
 */
#define LDLI(reg,val) \
lda reg, SEXT(val&0xffff)(r31)		; /* Mask off upper word	*/ \
ldah reg, SEXT((val+0x8000)>>16)(reg)	; /* Mask off lower word and	*/
					  /* remove its sign-extension	*/

#define LDLQ(reg, val_h, val_l) \
LDLI(reg, val_h); \
sll  reg, 32, reg; \
lda  reg, SEXT(val_l&0xffff)(reg); \
ldah reg, SEXT((val_l+0x8000)>>16)(reg);

/* 
 * NOTE: The following routines will clobber R0, R27-R29 and ALL PALtemps.  
 *	 The rest of the general purpose registers will be preserved. 
 */
#define b4_to_l(c1, c2, c3, c4)			\
	( (c4 << 24) | (c3 << 16) | (c2 << 8) | (c1 << 0) )

#define MC_IO_MINIDEBUGGER 	0
#define MC_IO_PRINT_STRING	1
#define MC_IO_PRINT_LONG	2
#define MC_IO_PRINT_CHAR	3
#define MC_IO_READ_CHAR		4
#define MC_IO_SET_BAUD		5
#define MC_IO_MINI_PROMPT	6
#define MC_IO_PRINT_BYTE	7

#define PRINTS8(lw_lo, lw_hi)			\
	lda	r29, MC_IO_PRINT_STRING(r31);	\
	LDLQ (r28, lw_hi, lw_lo);		\
	bsr	r0, minidebugger ;

#define PRINTS4(lw_lo)				\
	lda	r29, MC_IO_PRINT_STRING(r31);	\
	LDLI (r28, (lw_lo));			\
	bsr	r0, minidebugger ;

#define PRINTS2(c1, c2)				\
	lda	r29, MC_IO_PRINT_STRING(r31);	\
	lda r28, ((c1 << 0) | (c2 << 8))(r31);	\
	bsr	r0, minidebugger ;

#define PRINTL(reg)				\
	lda	r29, MC_IO_PRINT_LONG(r31);	\
	bis	r31, reg, r28;			\
	bsr	r0, minidebugger ;

#define PRINTB(reg)				\
	lda	r29, MC_IO_PRINT_BYTE(r31);	\
	bis	r31, reg, r28;			\
	bsr	r0, minidebugger ;

#define PRINTC(c)				\
	lda	r29, MC_IO_PRINT_CHAR(r31);	\
	lda	r28, c(r31);			\
	bsr	r0, minidebugger ;

#define READC()					\
	lda	r29, MC_IO_READ_CHAR(r31);	\
	bsr	r0, minidebugger ;		\
	bis	r28, r28, r0;

#define SET_BAUD(baud)				\
	lda	r29, MC_IO_SET_BAUD(r31);	\
	bis	r31, baud, r28;			\
	bsr	r0, minidebugger ;

#define jump_to_minidebugger()		       	\
	lda	r29, MC_IO_MINIDEBUGGER(r31);	\
	bsr	r0, minidebugger ;

#define jump_to_mini_prompt()		       	\
	lda	r29, MC_IO_MINI_PROMPT(r31);	\
	bsr	r0, minidebugger ;


#define	OutLEDPORT(VAL)	\
	bis	r31, VAL, r2	; 	/* load byte into (7:0)		*/\
	bsr	r29, WriteLEDS	; 	/* Go Write LED PORT		*/

#define USDELAY(DELAY) \
	LDLI	(r16, DELAY)	; \
	bsr		r27, usdelay	;


/*
**
** 82378 SIO Configuration Registers.
**
*/

#define SIO_ARB_CTRL            0x41
#define SIO_ARB_PRIO            0x42
#define SIO_ISA_CLK_DIV         0x4D
#define SIO_UTIL_BUS_A          0x4E
#define SIO_UTIL_BUS_B          0x4F


/*
**
>>--->>>
** Cypress CY82C693 Configuration Registers.
<<<---<<
**
*/
#ifdef SX164
#define CY_COMMAND_REG			0x04
#define CY_INTA_ROUTE			0x40
#define CY_INTB_ROUTE			0x41
#define CY_INTC_ROUTE			0x42
#define CY_INTD_ROUTE			0x43
#define CY_CONTROL_REG			0x44
#define CY_ERR_CONTROL_REG		0x45
#define CY_TOP_OF_MEM			0x48
#define CY_ATCTRL_REG1			0x49
#define CY_ATCTRL_REG2			0x4A
#define CY_IDE0_ROUTE			0x4B
#define CY_IDE1_ROUTE			0x4C
#define	CY_SA_USB_CTRL			0x4D
#endif /* SX164 */

/*
**
** ISA addresses
**
*/

/*
 * SIO's timer2, timer control word, and NMI
 * status and control ports
 */
#define SIO_TIMER2_PORT         0x42
#define SIO_TCW_PORT            0x43
#define SIO_NMISC_PORT          0x61

/*
 * realtime clock address and data ports
 */
#define RTC_ADDRESS_PORT        0x70
#define RTC_DATA_PORT           0x71

/*
 * the following refers to an ISA board with
 * LEDS (2 hexadecimal numbers) for debugging
 */
#define LED_PORT                0x80

/*
 *  SMC's address and data ports
 */
#define SMC_ADDRESS_PORT1       0x370
#define SMC_DATA_PORT1          0x371

/* 
 *  Port for switching flash ROM banks
 */
#define FLASH_PORT              0x800

/*
 *  Ports for reading the configuration jumpers.
 */
#define CONF_JUMPERS1           0x801
#define CONF_JUMPERS2           0x802

/*
** UBCSA -- Utility Bus Chip Select A Register
*/
#define sio$ubcsa_rtc		(1 << 0) /* 1 enables RTC locations 70h-77h         */
#define sio$ubcsa_kbd		(1 << 1) /* 1 enables kbd ctlr loc. 60h, 62h, 66h   */
#define sio$ubcsa_fl_3f2	(1 << 2) /* bits[5,3:2]=001 floppy loc. 3F2h-3f7h   */
#define sio$ubcsa_ide		(1 << 4) /* IDE enable, look at 82378 spec	    */
#define sio$ubcsa_lbios		(1 << 6) /* 1 enables access to FFFE0000h-FFFEFFFFh */
#define sio$ubcsa_xbios		(1 << 7) /* 1 enables access to FFF80000h-FFFDFFFFh */

/*
** UBCSB -- Utility Bus Chip Select B Register
*/
#define sio$ubcsb_v_porta	0	/* Enable Serial Port A		*/
#define sio$ubcsb_v_portb	2	/* Enable Serial Port B		*/
#define sio$ubcsb_v_parallel	4	/* Enable parallel port.	*/
#define sio$ubcsb_v_port92	6	/* Enable port 92.		*/
#define sio$ubcsb_v_confram	7	/* Enable configuration RAM	*/

/*
** PAC -- PCI Arbiter Control Register 
*/
#define sio$pac_gat		(1 << 0)	/* 1 = Guaranteed Access Time for ISA */
#define sio$pac_bl		(1 << 1)	/* 1 = Bus Lock */
#define sio$pac_bp		(1 << 2)	/* 1 = Park bus on CPUREQ# */
#define sio$mrt			(3 << 3)	/* Master Retry Timer */

/* RTC - Reat Time Clock Register Definitions */
#define rtc$reg_a		0xA	/* Address of Register A	*/
#define rtc$reg_b		0xB	/* Address of Register B	*/
#define rtc$reg_c		0xC	/* Address of Register C	*/
#define rtc$v_a_uip		7	/* Update In Progress bit	*/
#define rtc$m_a_uip		(1 << rtc$v_a_uip)
#define rtc$v_a_osc		4	/* Turn on/off oscillator.	*/
				/* Turn on osc. and allow RTC to keep time*/
#define rtc$m_a_osc_on		(2 << rtc$v_a_osc)
				/* Turn on osc. but keep countdown chain*/
				/* in reset.				*/
#define rtc$m_a_osc_partial	(7 << rtc$v_a_osc)

#define rtc$v_a_rs		0	/* Sets the oscillator rate	*/
#define rtc$m_a_rs_none		(0 << rtc$v_a_rs)
#define rtc$m_a_rs_256hz	(1 << rtc$v_a_rs)
#define rtc$m_a_rs_128hz	(2 << rtc$v_a_rs)
#define rtc$m_a_rs_8192hz	(3 << rtc$v_a_rs)	/* 8.192 Khz	*/
#define rtc$m_a_rs_4096hz	(4 << rtc$v_a_rs)	/* 4.096 Khz	*/
#define rtc$m_a_rs_2048hz	(5 << rtc$v_a_rs)	/* 2.048 Khz	*/
#define rtc$m_a_rs_1024hz	(6 << rtc$v_a_rs)	/* 1.024 Khz	*/
#define rtc$m_a_rs_512hz	(7 << rtc$v_a_rs)
#define rtc$m_a_rs_256hz_2	(8 << rtc$v_a_rs)
#define rtc$m_a_rs_128hz_2	(9 << rtc$v_a_rs)
#define rtc$m_a_rs_64hz		(10 << rtc$v_a_rs)
#define rtc$m_a_rs_32hz		(11 << rtc$v_a_rs)
#define rtc$m_a_rs_16hz		(12 << rtc$v_a_rs)
#define rtc$m_a_rs_8hz		(13 << rtc$v_a_rs)
#define rtc$m_a_rs_4hz		(14 << rtc$v_a_rs)
#define rtc$m_a_rs_2hz		(15 << rtc$v_a_rs)

#define rtc$v_b_set		7	/* Use to inhibit updates during*/
					/* time settings.		*/
#define rtc$m_b_set		(1 << rtc$v_b_set)
#define rtc$v_b_pie		6	/* Enable periodic interrupts.	*/
#define rtc$m_b_pie		(1 << rtc$v_b_pie)
#define rtc$v_b_aie		5	/* Enable alarm interrupts.	*/
#define rtc$m_b_aie		(1 << rtc$v_b_aie)
#define rtc$v_b_uie		4	/* Enable update ended interrupts*/
#define rtc$m_b_uie		(1 << rtc$v_b_uie)
#define rtc$v_b_sqwe		3	/* Enable square wave generation*/
#define rtc$m_b_sqwe		(1 << rtc$v_b_sqwe)
#define rtc$v_b_dm		2	/* 1 = binary, 0 = BCD format	*/
#define rtc$m_b_dm_bin		(1 << rtc$v_b_dm)
#define rtc$m_b_dm_bcd		(0 << rtc$v_b_dm)
#define rtc$v_b_24_12		1	/* 1 = 24-hour, 0 = 12-hour mode*/
#define rtc$m_b_24		(1 << rtc$v_b_24_12)
#define rtc$m_b_12		(0 << rtc$v_b_24_12)
#define rtc$v_b_dse		0	/* Enable Daylight Savings.	*/
#define rtc$m_b_dse		(1 << rtc$v_b_dse)

#define rtc$v_c_pf		6
#define rtc$m_c_pf		(1 << rtc$v_c_pf)

/*
** NMISC -- NMI status and control
*/
#define sio$nmisc_iochk	6	/* ISA Bus System ERR */
#define sio$nmisc_serr	7	/* PCI Bus System ERR */



#define sio$tcw_ctr_0		0 << 6	/* bits <7:6> select the counter or read cmd */
#define sio$tcw_ctr_1		1 << 6
#define sio$tcw_ctr_2		2 << 6
#define sio$tcw_read_back	3 << 6
#define sio$tcw_ctr_latch	0 << 4 /* bits <5:4> select read/write cmd */
#define sio$tcw_ctr_rw_lsb	1 << 4
#define sio$tcw_ctr_rw_msb	2 << 4
#define sio$tcw_ctr_rw_lsb_msb	3 << 4
#define sio$tcw_mode_end	0 << 1	/* bits <3:1> are the count mode */
#define sio$tcw_mode_os		1 << 1
#define sio$tcw_mode_rate	2 << 1
#define sio$tcw_mode_sq		3 << 1
#define sio$tcw_mode_sw		4 << 1
#define sio$tcw_mode_hw		5 << 1
#define sio$tcw_cnt_bcd		1	/* bcd count */

/* Normal beep codes. */
#define BEEP_LOAD_ENTIRE_ROM    0x4     /* Loading entire ROM.          */

/* Led and Beep codes for Fatal Errors. */
#define LED_F_BADCPUSPEED 0x04	/* Bad CPU speed selected	*/
#define LED_F_NOSIMMS	  0x05	/* No SIMMs were found when sizing memory.*/
#define LED_F_MEMCHECKSUM 0x06	/* A ROM image written to memory had a  */
                                /* checksum error when it was read back.*/
#define LED_F_MEMSIZ	  0x07	/* Could not detect SIMM type during    */
                                /* memory sizing.                       */



#endif				/* __IO_LOADED */
