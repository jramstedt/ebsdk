/*
 *      VID: [2.0] PT: [Thu Aug 21 16:25:43 1997] SF: [platform.h]
 *       TI: [/sae_share/apps/bin/vice -iplatform.s -l// -p# -DEB164 -DLX164 -h -m -Xlint -XDEBUG -XPAL_K_REV -XKDEBUG -XDISABLE_CRD -XDISABLE_MACHINE_CHECKS -XSROM_SERIAL_PORT -XCONSOLE_ENTRY -alx164 ]
 */
#define	__PLATFORM_LOADED	1
/*
*****************************************************************************
**                                                                          *
**  Copyright © 1993, 1994	       					    *
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
**  FACILITY:
**
**	DECchip 21164 OSF/1 PALcode
**
**  MODULE:
**
**	platform.h
**
**  MODULE DESCRIPTION:
**
**      Platform specific definitions.
**
**  AUTHOR: ES
**
**  CREATION DATE:  07-JUN-1994
**
**  $Id: platform.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $
**
**  MODIFICATION HISTORY:
**
**  $Log: platform.h,v $
**  Revision 1.1.1.1  1998/12/29 21:36:07  paradis
**  Initial CVS checkin
**
**  Revision 1.9  1996/06/17  16:13:15  fdh
**  Added error status register definitions.
**
**  Added the LEDWRITE and LEDWRREG macro definitions
**  for printing register data to the PC diagnostic card
**  for use in debugging.
**
**  Revision 1.8  1995/11/30  20:54:17  fdh
**  Added definitions required to initialize
**  the CIA Control Register.
**
**  Revision 1.7  1995/04/21  02:06:30  fdh
**  Replaced C++ style comments with Standard C style comments.
**
**  Revision 1.6  1995/02/02  19:32:23  samberg
**  Added CACK_EN and MCR
**
**  Revision 1.5  1994/09/26  14:17:01  samberg
**  Complete VICE work and EB164/SD164 breakout
**
**  Revision 1.4  1994/08/30  14:40:37  samberg
**  Remove SIO configuration register info, done by someone else
**
**  Revision 1.3  1994/08/17  14:44:18  samberg
**  Fixed SIO_CFIG address -- should be 1000000.
**
**  Revision 1.2  1994/07/26  17:40:12  samberg
**  Changes for SD164, bit 39 in I/O
**
**  Revision 1.1  1994/07/08  17:04:45  samberg
**  Initial revision
**
**
*/

#if !defined(CONSOLE_ENTRY)
#define CONSOLE_ENTRY	        0x10000
#endif /* CONSOLE_ENTRY */



/*======================================================================*/
/*           DECchip 21164/21071 EVALUATION BOARD DEFINITIONS           */
/*======================================================================*/
/*
** EB164 Address Space Partitioning
**
**	Address Bit
**   39 34 33 32 31 30 29 28          Description
**   -- --------------------	   -----------
**   1   1  0  1  1  0  -  -	   PCI Sparse I/O Region A
**   1   1  1  1  0  0  0  -	   PCI Configuration
**   1   1  1  1  0  0  1  -	   PCI Interrupt Ack
**   1   1  1  1  0  1  0  0	   CIA Main CSRs
**   1   1  1  1  0  1  0  1	   CIA Memory Control CSRs
**
** Note that we need to set bit 39! Since the span between bit 39
** and the byte enable field is more than 32, we set bit 39 in the
** port macros.
*/

/*
** The following definitions need to be shifted left 28 bits
** to obtain their respective reference points.
*/

#define IO_BASE			0x58	/* PCI I/O Base Address */
#define CFIG_BASE		0x70	/* PCI Configuration Base Address */
#define I_ACK_BASE		0x72	/* PCI Interrupt Acknowledge */
#define CIA_BASE                0x74    /* CIA Main CSRs */
#define CIA_MEM_BASE            0x75    /* CIA Memory Control CSRs */

#define BYTE_ENABLE_SHIFT	5


/*
** IPL translation table definitions:
**
** EB164 specific IRQ pins are
**
**  Line   IPL  Source			        OSF/1 IPL
**  ----   ---	------			        ---------
**  IRQ0   20	Corrected ECC error	        7
**  IRQ1   21	PCI/ISA                         3
**  IRQ2   22	Real Time Clock 	        5
**  IRQ3   23	SIO NMI, CIA errors	        7
**
**  The mask contains one byte for each IPL level, with IPL0 in the
**  least significant (right-most) byte and IPL7 in the most
**  significant (left-most) byte.  Each byte in the mask maps the
**  OSF/1 IPL to the DC21164 IPL.
**
**  OSF/1 IPL	IPL
**  ---------	---
**	0	0
**	1	1
**	2	2
**	3	21 (to account for PCI/ISA at IPL 21)
**	4	21
**	5	22 (to account for clock at IPL 21)
**	6	30 (to account for powerfail)
**	7	31
*/

#define INT_K_MASK_HIGH         0x1F1E1615
#define INT_K_MASK_LOW          0x15020100


/*
** Interrupt Source Register Definitions:
**
** On the EB164, the we need to handle 17 PCI interrupts:
** four from each of the two PCI slots, SCSI, Ethernet, and the SIO.
** Once the interrupt occurs, the source can be determined by
** reading registers at ISA addresses 804h, 805h and 806h.
**
** However, each interrupt can be individually masked by writing a '1'
** to these same registers. Thus, the masking is a write-only function.
** For now, we leave it to the OS to check the mask against the source.
** OS to check the mask against the source.
**
*/

#define INT_SRC_804    	DefinePort(IO_BASE,0x804) /* 804h register */
#define INT_SRC_805   	DefinePort(IO_BASE,0x805) /* 805h register */
#define INT_SRC_806   	DefinePort(IO_BASE,0x806) /* 806h register */

/*
** CIA Machine Checks
*/

#define MCHK_K_CIA_0       0x021E       /* IRQ0 -- IPL 20 */
#define MCHK_K_CIA_3       0x0220       /* IRQ3 -- IPL 23 */

/*
** Offsets for General CIA CSRs
*/

#define CIA_CTRL        0x100   /* CIA Control Register */
#define CIA_CACK_EN     0x600   /* CIA Acknowledgement Control Register */

/*
** Offsets for System Configuration Registers
*/
#define CIA_MCR         0x000   /* Memory Configuration Register */

#define CIA_ERROR_BASE	0x8740008	/* Base >> 12 */
#define CIA_CPU_ERR0	0x000
#define CIA_CPU_ERR1	0x040
#define CIA_ERR		0x200
#define CIA_STAT	0x240
#define CIA_ERR_MASK	0x280
#define CIA_SYN		0x300
#define CIA_MEM_ERR0	0x400
#define CIA_MEM_ERR1	0x440
#define CIA_PCI_ERR0	0x800
#define CIA_PCI_ERR1	0x840
#define CIA_PCI_ERR2	0x880


/*
**  Initial CIA Control Register
**	MC_CIA_CTRL_V_PCI_EN		= 1
**	MC_CIA_CTRL_V_PCI_LOCK_EN	= 0
**	MC_CIA_CTRL_V_PCI_LOOP_EN	= 0
**	MC_CIA_CTRL_V_FST_BB_EN		= 0
**	MC_CIA_CTRL_V_PCI_MST_EN	= 1
**	MC_CIA_CTRL_V_PCI_MEM_EN	= 1
**	MC_CIA_CTRL_V_PCI_REQ64_EN	= 1
**	MC_CIA_CTRL_V_PCI_ACK64_EN	= 1
**	MC_CIA_CTRL_V_ADDR_PE_EN	= 0
**	MC_CIA_CTRL_V_PERR_EN		= 0
**	MC_CIA_CTRL_V_FILLERR_EN	= 0
**	MC_CIA_CTRL_V_MCHKERR_EN	= 0
**	MC_CIA_CTRL_V_ECC_CHK_EN	= 0
**	MC_CIA_CTRL_V_ASSERT_IDLE_BC	= 0
**	MC_CIA_CTRL_V_CON_IDLE_BC	= 1
**	MC_CIA_CTRL_V_CSR_IOA_BYPASS	= 1
**	MC_CIA_CTRL_V_IO_FLUSHREQ_EN	= 0
**	MC_CIA_CTRL_V_CPU_FLUSHREQ_EN	= 0
**	MC_CIA_CTRL_V_ARB_EV5_EN	= 0
**	MC_CIA_CTRL_V_EN_ARB_LINK	= 0
**	MC_CIA_CTRL_V_RD_TYPE		= 0
**	MC_CIA_CTRL_V_RL_TYPE		= 1
**	MC_CIA_CTRL_V_RM_TYPE		= 2
*/
#define CIA_CTRL_K_INIT 0x2100C0F1
#define CIA_CTRL_K_SYS_MCHK 0x1C00


/*
** SIO Machine Checks
*/

#define MCHK_K_SIO_SERR     0x0204
#define MCHK_K_SIO_IOCHK    0x0206

/*
** SIO Control Register Definitions
*/

#define SIO_B_NMISC	0x61	/* NMI Status And Control */
#define SIO_B_NMI	0x70	/* NMI Enable */

#define SIO_NMISC       DefinePort(IO_BASE, SIO_B_NMISC)
#define SIO_NMI         DefinePort(IO_BASE, SIO_B_NMI)

/*
** NMISC - NMI Status and Control Register
**
**	  Loc	Size	Name	Function
**	 -----	----	----	---------------------------------
**	   <7>    1     SERR	System Error
**	   <6>    1	IOCHK	IOCHK asserted on the ISA/SIO bus
**	   <3>	  1	IOCHK_E IOCHK Enable
**	   <2>	  1 	SERR_E  SERR Enable
*/

#define SIO_NMISC_V_SERR	7
#define SIO_NMISC_M_SERR	(1<<SIO_NMISC_V_SERR)
#define SIO_NMISC_V_IOCHK	6
#define SIO_NMISC_M_IOCHK	(1<<SIO_NMISC_V_IOCHK)
#define SIO_NMISC_V_IOCHK_EN    3
#define SIO_NMISC_M_IOCHK_EN	(1<<SIO_NMISC_V_IOCHK_EN)
#define SIO_NMISC_V_SERR_EN	2
#define SIO_NMISC_M_SERR_EN	(1<<SIO_NMISC_V_SERR_EN)

/*
** Intel 82C59A Priority Interrupt Controller (PIC) Definitions:
*/

#define PIC1		0x20	/* INT0 Megacell Address */
#define PIC2		0xA0	/* INT1 Megacell Address */

#define ICW1		0	/* Initialization Control Word 1 Offset */
#define ICW2		1	/* Initialization Control Word 2 Offset */
#define ICW3		1	/* Initialization Control Word 3 Offset */
#define ICW4		1	/* Initialization Control Word 4 Offset */

#define OCW1		1	/* Operation Control Word 1 Offset */
#define OCW2		0	/* Operation Control Word 2 Offset */
#define OCW3		0	/* Operation Control Word 3 Offset */

#define PIC1_ICW1	DefinePort(IO_BASE,PIC1+ICW1)
#define PIC1_ICW2	DefinePort(IO_BASE,PIC1+ICW2)
#define PIC1_ICW3	DefinePort(IO_BASE,PIC1+ICW3)
#define PIC1_ICW4	DefinePort(IO_BASE,PIC1+ICW4)
#define PIC1_OCW1	DefinePort(IO_BASE,PIC1+OCW1)
#define PIC1_OCW2	DefinePort(IO_BASE,PIC1+OCW2)
#define PIC1_OCW3	DefinePort(IO_BASE,PIC1+OCW3)

#define PIC2_ICW1	DefinePort(IO_BASE,PIC2+ICW1)
#define PIC2_ICW2	DefinePort(IO_BASE,PIC2+ICW2)
#define PIC2_ICW3	DefinePort(IO_BASE,PIC2+ICW3)
#define PIC2_ICW4	DefinePort(IO_BASE,PIC2+ICW4)
#define PIC2_OCW1	DefinePort(IO_BASE,PIC2+OCW1)
#define PIC2_OCW2	DefinePort(IO_BASE,PIC2+OCW2)
#define PIC2_OCW3	DefinePort(IO_BASE,PIC2+OCW3)

#define I_ACK   	DefinePort(I_ACK_BASE,0)

/*
** Dallas DS1287A Real-Time Clock (RTC) Definitions:
*/

#define RTCADD     	DefinePort(IO_BASE,0x70) /* RTC Address Register */
#define RTCDAT     	DefinePort(IO_BASE,0x71) /* RTC Data Register */
#define LEDPORT     	DefinePort(IO_BASE,0x80) /* LED Port */


/*
** Serial Port (COM) Definitions:
*/

#define COM1			0x3F8	/* COM1 Serial Line Port Address */
#define COM2			0x2F8	/* COM2 Serial Line Port Address */

#define RBR			0	/* Receive Buffer Register Offset */
#define THR			0	/* Xmit Holding Register Offset */
#define DLL			0	/* Divisor Latch (LS) Offset */
#define DLH			1	/* Divisor Latch (MS) Offset */
#define IER			0x1	/* Interrupt Enable Register Offset */
#define IIR			0x2	/* Interrupt ID Register Offset */
#define LCR			0x3	/* Line Control Register Offset */
#define MCR			0x4	/* Modem Control Register Offset */
#define LSR			0x5	/* Line Status Register Offset */
#define MSR			0x6	/* Modem Status Register Offset */
#define SCR			0x7	/* Scratch Register Offset */

#define DLA_K_BRG		12	/* Baud Rate Divisor = 9600 */

#define LSR_V_THRE		5	/* Xmit Holding Register Empty Bit */

#define LCR_M_WLS		3	/* Word Length Select Mask */
#define LCR_M_STB		4	/* Number Of Stop Bits Mask */
#define LCR_M_PEN		8	/* Parity Enable Mask */
#define LCR_M_DLAB		128	/* Divisor Latch Access Bit Mask */

#define LCR_K_INIT	      	(LCR_M_WLS | LCR_M_STB)

#define MCR_M_DTR		1	/* Data Terminal Ready Mask */
#define MCR_M_RTS		2	/* Request To Send Mask */
#define MCR_M_OUT1		4	/* Output 1 Control Mask */
#define MCR_M_OUT2		8	/* UART Interrupt Mask Enable */

#define MCR_K_INIT	      	(MCR_M_DTR  | \
				 MCR_M_RTS  | \
				 MCR_M_OUT1 | \
				 MCR_M_OUT2)

#define COM1_RBR		DefinePort(IO_BASE,COM1+RBR)
#define COM1_THR		DefinePort(IO_BASE,COM1+THR)
#define COM1_DLL		DefinePort(IO_BASE,COM1+DLL)
#define COM1_DLH		DefinePort(IO_BASE,COM1+DLH)
#define COM1_IER		DefinePort(IO_BASE,COM1+IER)
#define COM1_IIR		DefinePort(IO_BASE,COM1+IIR)
#define COM1_LCR		DefinePort(IO_BASE,COM1+LCR)
#define COM1_MCR		DefinePort(IO_BASE,COM1+MCR)
#define COM1_LSR		DefinePort(IO_BASE,COM1+LSR)
#define COM1_MSR		DefinePort(IO_BASE,COM1+MSR)
#define COM1_SCR		DefinePort(IO_BASE,COM1+SCR)

#define COM2_RBR		DefinePort(IO_BASE,COM2+RBR)
#define COM2_THR		DefinePort(IO_BASE,COM2+THR)
#define COM2_DLL		DefinePort(IO_BASE,COM2+DLL)
#define COM2_DLH		DefinePort(IO_BASE,COM2+DLH)
#define COM2_IER		DefinePort(IO_BASE,COM2+IER)
#define COM2_IIR		DefinePort(IO_BASE,COM2+IIR)
#define COM2_LCR		DefinePort(IO_BASE,COM2+LCR)
#define COM2_MCR		DefinePort(IO_BASE,COM2+MCR)
#define COM2_LSR		DefinePort(IO_BASE,COM2+LSR)
#define COM2_MSR		DefinePort(IO_BASE,COM2+MSR)
#define COM2_SCR		DefinePort(IO_BASE,COM2+SCR)

/*
** Macro to define a port address
*/
#define IO_MASK 	0x7FFFFFF

#define DefinePort(Base,PortAddr) \
	((Base<<(28-4)) | ((IO_MASK&PortAddr) << (BYTE_ENABLE_SHIFT-4)))

/* NOTE ON ADDITIONAL PORT DEFINITION:
**
** We also need to set bit 39! Since the span between bit 39
** and the byte enable field is more than 32, we set bit 39 in the
** port macros.
*/

/*
** Macro to write a byte literal to a specified port
*/
#define OutPortByte(port,val,tmp0,tmp1) \
	LDLI	(tmp0, port); \
	sll	tmp0, 4, tmp0; \
	lda	tmp1, 1(zero); \
	sll	tmp1, 39, tmp1; \
	bis	tmp0, tmp1, tmp0; \
	lda	tmp1, (val)(zero); \
	insbl	tmp1, ((port>>(BYTE_ENABLE_SHIFT-4))&3), tmp1; \
	stl_p	tmp1, 0x00(tmp0); \
	mb			; \
	mb
/*
** Macro to write a byte from a register to a specified port
*/
#define OutPortByteReg(port,reg,tmp0,tmp1) \
	LDLI	(tmp0, port); \
	sll	tmp0, 4, tmp0; \
	lda	tmp1, 1(zero); \
	sll	tmp1, 39, tmp1; \
	bis	tmp0, tmp1, tmp0; \
	and	reg, 0xFF, tmp1; \
	insbl	tmp1, ((port>>(BYTE_ENABLE_SHIFT-4))&3), tmp1; \
	stl_p	tmp1, 0x00(tmp0); \
	mb			; \
	mb
/*
** Macro to write a longword from a register to a specified port
*/
#define OutPortLongReg(port,reg,tmp0,tmp1) \
	LDLI	(tmp0, port); \
	sll	tmp0, 4, tmp0; \
	lda	tmp1, 1(zero); \
	sll	tmp1, 39, tmp1; \
	bis	tmp0, tmp1, tmp0; \
	stl_p	reg, 0x00(tmp0); \
	mb			; \
	mb
/*
** Macro to read a byte from a specified port
*/
#define InPortByte(port,tmp0,tmp1) \
	LDLI	(tmp0, port); \
	sll	tmp0, 4, tmp0; \
	lda	tmp1, 1(zero); \
	sll	tmp1, 39, tmp1; \
	bis	tmp0, tmp1, tmp0; \
	ldl_p	tmp0, 0x00(tmp0); \
	srl	tmp0, (8*((port>>(BYTE_ENABLE_SHIFT-4))&3)), tmp0; \
	zap	tmp0, 0xfe, tmp0

/*
** Macro to acknowledge interrupts
**
** A typical SIO interrupt acknowledge sequence is as follows:
** The CPU generates an interrupt acknowledge cycle that is
** translated into a single PCI command and broadcast across
** the PCI bus to the SIO.  The SIO interrupt controller
** translates this command into the two INTA# pulses expected
** by the 82C59A interrupt controller subsystem.
**
** On the first iAck cycle, the cascading priority is resolved
** to detemine which of the two megacells will output the
** interrupt vector onto the data bus. On the second iAck cycle,
** the appropriate megacell drives the data bus with an 8-bit
** pointer to the correct interrupt vector for the highest priority
** interrupt
**
**	Read port to initiate PCI iACK cycle
**
** INPUT PARAMETERS:
**
**	tmp0	scratch
**	tmp1	scratch
**
** OUTPUT PARAMETERS:
**
**	tmp0	interrupt vector
*/

#define IACK(tmp0,tmp1)	\
	InPortByte(I_ACK,tmp0,tmp1)


#define LEDDPORT		0x80
#define IO_SHIFT 	5
#define PCI_SPARSE 	0x0858
#define BASE_SHIFT	28
#define WaitDelay	0x800
#define LEDWRITE(value, reg1, reg2)			\
    lda		reg1, PCI_SPARSE(r31);			\
    sll 	reg1, (BASE_SHIFT-IO_SHIFT), reg1;	\
    lda 	reg1, LEDDPORT(reg1);			\
    sll 	reg1, IO_SHIFT, reg1;			\
    bis 	r31, value, reg2; 			\
    stl_p	reg2, 0(reg1);				\
    mb;							\
    ldah	reg1, WaitDelay(r31);			\
0:  subq	reg1, 1, reg1;				\
    bne		reg1, 0b;

#define LEDWRREG(inreg, reg1, reg2, reg3, reg4)	\
	LEDWRITE(0xEE, reg1, reg2)	/* Signal that 8 bytes coming	*/\
	bis	r31, 8, reg3;		/* Load up byte counter		*/\
	bis	inreg, r31, reg4;	/* Move Address into reg4	*/\
1:	LEDWRITE(reg4,reg1,reg2)	/* Write it out to the port	*/\
	subq	reg3, 1, reg3;		/* Decrement counter		*/\
	srl	reg4, 8, reg4;		/* Shift next byte down		*/\
	bne	reg3, 1b;		/* Are we done yet?		*/\
	LEDWRITE(0xEE, reg1, reg2)	/* Output Done Signal		*/

