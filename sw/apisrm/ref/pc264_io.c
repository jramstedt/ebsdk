/* file:	pc264_io.c
 * 
 * Copyright (C) 1996, 1997, 1998 by
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
 */

/*
 *++                                                                     
 *  FACILITY:
 *
 *      Alpha Firmware for PC264
 *                                            
 *  MODULE DESCRIPTION:     
 *
 *	PC264 platform specific io routines
 *
 *  AUTHORS:
 *
 *	Eric Goehl
 *
 *  CREATION DATE:
 *  
 *      13-Nov-1996
 *
 *  MODIFICATION HISTORY:
 *
 *	er	17-Nov-1998	Adding get_com1_baud() and set_com1_baud().
 *	er	13-Mar-1998	Moved interrupt handling support,
 *				probe_local(), PowerUpProgress(),
 *				and use_eisa_ecu_data() stub from 
 *				PC264.C to here.
 *	er	24-Aug-1997	Fixed TIG bus xflash().
 *	er	28-Jul-1997	Fixed offset mask in xcfgx.
 *	er	14-Jul-1997	Added probe_pci_setup routine,
 *				cleaned up includes, external defs.
 *     	egg	13-Nov-1996	Initial port from eb164_io.c
 *
 *--
 */                                                

extern struct pb *pb;

/* $INCLUDE_OPTIONS$ */
#include	"cp$inc:platform_io.h"
/* $INCLUDE_OPTIONS_END$ */
#include        "cp$src:platform.h"
#include        "cp$src:stddef.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:pb_def.h"
#include 	"cp$src:common.h"	/* uint64 */
#include	"cp$inc:prototypes.h"
#include	"cp$src:probe_io_def.h"	/* TYPE_PCI, etc. */
#include	"cp$src:dynamic_def.h"	/* DYN$K_... constants. */
#include	"cp$src:pal_def.h"	/* CSERVE$IIC_WRITE */
#include	"cp$src:common_def.h"	/* msg_failure, msg_success */
#include	"cp$src:ev_def.h"	/* set_param_table */
#include	"cp$src:ctype.h"	/* toupper */
                                     
                                     
  /* Platform specific include files */
#include 	"cp$src:platform_cpu.h"            
#include 	"cp$src:tsunami.h"   
#include	"cp$src:cy82c693_def.h"    
                                     
#define DEBUG	0                    
                                     
/*                                   
** External Data Declarations 
*/                            
extern struct LOCK spl_kernel;                  
extern int primary_cpu;       
extern  int iic_inited;       
extern struct set_param_table baud_table[];
extern int cbip;
                                                        
/*                            
** External Function Prototype Declarations     
*/                       
                       
  /* From tsunami_io.c */
extern void tsunami_probe_local( );
extern unsigned char xtig( struct pb *pb, unsigned __int64 offset, unsigned char data,
			   unsigned char (*ld_or_st)( ) );
                                                
  /* From kernel_alpha.mar */
extern unsigned __int64 ldq( unsigned __int64 address );
extern void stq( unsigned __int64 address, unsigned __int64 data );
extern spinlock( struct LOCK *spl );
extern spinunlock( struct LOCK *spl );
extern int do_bpt();

  /* From printf.c */
extern int qprintf( char *format, ... );
extern int err_printf( char *format, ... );

  /* From pc264.c */
extern int whoami( void );

  /* From pc264_init.c */
extern void SMC_init( void );

void setup_io( void )
{
    unsigned __int64 pctl_bits = 0;
    unsigned int perrmask_bits = 0;
    CY82C693_CONTROL_REGS *CY82C693_regs;
    struct pb pb;
    unsigned __int64 temp;
    unsigned int i;

/*
** Disable all interrupts
*/
    WriteTsunamiCSR( CSR_DIM0, 0 );
    WriteTsunamiCSR( CSR_DIM1, 0 );
/*
** Initialize Pchip Control Registers (PCTL)
*/
    pctl_bits = (
	  pctl_m_fdsc			    /* Enable fast discard */
	| pctl_m_tgtlat			    /* Enable target latency timers */
	| pctl_m_eccen			    /* Enable ECC for DMA and SGTE accesses */
	| pctl_m_ptevrfy		    /* Enable PTE verify for DMA read */
    );
    WriteTsunamiCSR( PCHIP0_PCTL, ReadTsunamiCSR( PCHIP0_PCTL ) | pctl_bits );
    WriteTsunamiCSR( PCHIP1_PCTL, ReadTsunamiCSR( PCHIP1_PCTL ) | pctl_bits );
/*
** Dismiss any pending errors
*/
    temp = ReadTsunamiCSR( CSR_MISC );
    temp |= ( uint64 )1 << 28;	    /* Clear NXM error */
    WriteTsunamiCSR( CSR_MISC, temp );

    WriteTsunamiCSR( PCHIP0_PERROR, ReadTsunamiCSR( PCHIP0_PERROR ) );
    WriteTsunamiCSR( PCHIP1_PERROR, ReadTsunamiCSR( PCHIP1_PERROR ) );
/*
** Initialize Pchip Error Mask Registers (PERRMASK)
*/
    perrmask_bits = (
	  perrmask_m_lost
	| perrmask_m_serr
	| perrmask_m_perr
	| perrmask_m_sge
	| perrmask_m_ape
	| perrmask_m_ta
	| perrmask_m_rdpe
	| perrmask_m_nds
	| perrmask_m_rto
	| perrmask_m_uecc
	| perrmask_m_cre
    );
    WriteTsunamiCSR( PCHIP0_PERRMASK, perrmask_bits );
    WriteTsunamiCSR( PCHIP1_PERRMASK, perrmask_bits );
/* 
**  Set up PCI latency to be full latency
*/  
    WriteTsunamiCSR( PCHIP0_PLAT, 0xff00 );
    WriteTsunamiCSR( PCHIP1_PLAT, 0xff00 );
/*  
**  Invalidate all TB entries by writing the the invalidate all CSR.
*/                                                       
    WriteTsunamiCSR( PCHIP0_TLBIA, 0 );                                  
    WriteTsunamiCSR( PCHIP1_TLBIA, 0 );                    
/*
** Set up the PCI Target Windows
**
**	Window	    Start Address   Size    Mapping	Console
**	-------	    -------------   ----    -------	--------
**	   0	       00800000	    8 MB		Disabled
**	   1	       80000000	    1 GB    Direct	Enabled
**	   2	       C0000000	    1 GB    S/G		Disabled
**	   3						Disabled
*/
#define    WSBA2_VALUE 	0xC0000000
/*                                                       
**  PCI Target Window 0                          
*/                                                       
    WriteTsunamiCSR( PCHIP0_WSBA0, 0x00800000 );
    WriteTsunamiCSR( PCHIP0_WSM0,  0x00700000 );
    WriteTsunamiCSR( PCHIP0_TBA0,  0 );
                        
    WriteTsunamiCSR( PCHIP1_WSBA0, 0x00800000 );
    WriteTsunamiCSR( PCHIP1_WSM0,  0x00700000 );
    WriteTsunamiCSR( PCHIP1_TBA0,  0 );
/*                                                       
**  PCI Target Window 1                          
*/                                                       
    WriteTsunamiCSR( PCHIP0_WSBA1, 0x80000000 | wsba_m_ena );
    WriteTsunamiCSR( PCHIP0_WSM1,  wmask_k_sz1gb );
    WriteTsunamiCSR( PCHIP0_TBA1,  0 );

    WriteTsunamiCSR( PCHIP1_WSBA1, 0x80000000 | wsba_m_ena  );
    WriteTsunamiCSR( PCHIP1_WSM1,  wmask_k_sz1gb );
    WriteTsunamiCSR( PCHIP1_TBA1,  0 );
/*
**  PCI Target Window 2
*/
    WriteTsunamiCSR( PCHIP0_WSBA2,  0 );
    WriteTsunamiCSR( PCHIP1_WSBA2,  0 );
/*
**  PCI Target Window 3
*/
    WriteTsunamiCSR( PCHIP0_WSBA3,  0 );
    WriteTsunamiCSR( PCHIP1_WSBA3,  0 );
/*
** Cypress CY82C693 Peripheral Controller Initialization
**
** The CY82C693 Control Registers are accessed through I/O
** ports 22h (index) and 23h (data).
*/
    CY82C693_regs = ( CY82C693_CONTROL_REGS * )0x22;
/* 
** Set up needed fields in a port block to address the
** CY82C693 controller's PCI configuration space.
*/
    pb.hose = 0;
    pb.bus = 0;
    pb.slot = 5;
    pb.function = 0;
/*
** Initialize the CY82C693 Control Registers to their
** default (D) values.
**
** Register 1 - Peripheral Configuration Register #1
**
**	<7:6> - I/O Wait State Control
**		00 : 1 wait state
**		01 : 2 wait states
**		10 : 3 wait states
**		11 : 4 wait states (D)
**
**	<5:4> - 16-bit DMA Wait State Control
**		00 : 1 wait state (D)
**		01 : 2 wait states
**		10 : 3 wait states
**		11 : 4 wait states
**
**	<3:2> - 8-bit DMA Wait State Control
**		00 : 1 wait state (D)
**		01 : 2 wait states
**		10 : 3 wait states
**		11 : 4 wait states
**
**	<1:1> - MEMR Leading Edge Delay Control
**		 0 : 1 DMA clock delay (D)
**		 1 : No delay
**
**	<0:0> - DMA Controller Clock Speed Control
**		 0 : DMA clock is ATCLK divided by two (D)
**		 1 : DMA clock is ATCLK
*/
    outportb( &pb, &CY82C693_regs->index, CY82C693_CFG_1_INDEX );
    outportb( &pb, &CY82C693_regs->data,  0xC0 );
/*
** Register 2 - Peripheral Configuration Register #2
**
**	<7:3> - Reserved
**	<2:2> - Peripheral Controller Test Mode Control
**		 0 : Normal operation (D)
**		 1 : Test mode
**
**	<1:1> - Interrupt Request Configuration Control
**		 0 : Normal operation (D)
**		 1 : Enable interrupt request edge/level control
**
**	<0:0> - DMA Extended Mode Control
**		 0 : Enable (D)
**		 1 : Disable
*/
    outportb( &pb, &CY82C693_regs->index, CY82C693_CFG_2_INDEX );
    outportb( &pb, &CY82C693_regs->data, 0x00 );
/*
** Register 3 - Interrupt Request Level/Edge Control Register #1
**
**	<7> - IRQ7 Edge-Triggered/Level-Sensitive Selector
**		 0 : Edge-Triggered (D)
**		 1 : Level-Sensitive
**
**	<6> - IRQ6 Edge-Triggered/Level-Sensitive Selector
**		 0 : Edge-Triggered (D)
**		 1 : Level-Sensitive
**
**	<5> - IRQ5 Edge-Triggered/Level-Sensitive Selector
**		 0 : Edge-Triggered (D)
**		 1 : Level-Sensitive
**
**	<4> - IRQ4 Edge-Triggered/Level-Sensitive Selector
**		 0 : Edge-Triggered (D)
**		 1 : Level-Sensitive
**
**	<3> - IRQ3 Edge-Triggered/Level-Sensitive Selector
**		 0 : Edge-Triggered (D)
**		 1 : Level-Sensitive
**
**	<2:0> - Reserved MBZ
*/
    outportb( &pb, &CY82C693_regs->index, CY82C693_ELCR_1_INDEX );
    outportb( &pb, &CY82C693_regs->data, 0x00 );
/*
** Register 4 - Interrupt Request Level/Edge Control Register #2
**
**	<7> - IRQ15 Edge-Triggered/Level-Sensitive Selector
**		 0 : Edge-Triggered (D)
**		 1 : Level-Sensitive
**
**	<6> - IRQ14 Edge-Triggered/Level-Sensitive Selector
**		 0 : Edge-Triggered (D)
**		 1 : Level-Sensitive
**
**	<5> - Reserved MBZ
**
**	<4> - IRQ12 Edge-Triggered/Level-Sensitive Selector
**		 0 : Edge-Triggered (D)
**		 1 : Level-Sensitive
**
**	<3> - IRQ11 Edge-Triggered/Level-Sensitive Selector
**		 0 : Edge-Triggered (D)
**		 1 : Level-Sensitive
**
**	<2> - IRQ10 Edge-Triggered/Level-Sensitive Selector
**		 0 : Edge-Triggered (D)
**		 1 : Level-Sensitive
**
**	<1> - IRQ9 Edge-Triggered/Level-Sensitive Selector
**		 0 : Edge-Triggered (D)
**		 1 : Level-Sensitive
**
**	<0> - Reserved MBZ
*/
    outportb( &pb, &CY82C693_regs->index, CY82C693_ELCR_2_INDEX );
    outportb( &pb, &CY82C693_regs->data, 0x00 );
/*
** Register 5 - Real Time Clock Configuration Register
**
**	<7:3> - Reserved
**	<2:0> - Value used to fine tune the 32 KHz oscillator for
**		greater RTC accuracy.
*/
    outportb( &pb, &CY82C693_regs->index, CY82C693_RTC_CFG_INDEX );
    outportb( &pb, &CY82C693_regs->data, 0x00 );
/*
** DMA Register 13 - DMAC1 Master Clear Register
*/
    outportb( &pb, CY82C693_DMAC1_MasterClear, 0 );
/*
** DMA Register 29 - DMAC2 Master Clear Register
*/
    outportb( &pb, CY82C693_DMAC2_MasterClear, 0 );
/*
** DMA Register 27 - DMAC2 DMA Mode Register
**
**	<7:6> - DMA Mode = Cascade
**	<5:5> - Counter Direction = Increment
**	<4:4> - Autoinitialization = Disabled
**	<3:2> - DMA Transfer Type = Verify
**	<1:0> - Channel Selector = Channel 0
*/
    outportb( &pb, CY82C693_DMAC2_Mode, 0xC0 );
/*
**  Register 3 - Command Register
**
**	<15:10> - Reserved
**	    <9> - Fast Back-to-Back = DISABLED
**          <8> - SERR Reporting = ENABLED
**	  <7:4> - Reserved
**	    <3> - Special Cycle Decoding = ENABLED
**	    <2> - Bus Master = ENABLED
**          <1> - Memory Access = ENABLED
**          <0> - I/O Access = ENABLED
*/
    outcfgw( &pb, CY82C693_Command, 0x10F );
/*
**  Register 10 - PCI Interrupt A Routing Control Register
**
**	  <7> - Interrupt A Routing Control:
**		    0:  Interrupt A Routing Enabled
**		    1:  Interrupt A Routing Disabled
**	<6:4> - Reserved
**	<3:0> - Interrupt Request Level INTA is Routed to = IRQ10
*/
    outcfgb( &pb, CY82C693_PCI_INTA_Routing, 0x0A );
/*
**  Register 11 - PCI Interrupt B Routing Control Register
**
**	  <7> - Interrupt B Routing Control:
**		    0:  Interrupt B Routing Enabled
**		    1:  Interrupt B Routing Disabled
*/
    outcfgb( &pb, CY82C693_PCI_INTB_Routing, 0x80 );
/*
**  Register 12 - PCI Interrupt C Routing Control Register
**
**	  <7> - Interrupt C Routing Control:
**		    0:  Interrupt C Routing Enabled
**		    1:  Interrupt C Routing Disabled
*/
    outcfgb( &pb, CY82C693_PCI_INTC_Routing, 0x80 );
/*
**  Register 13 - PCI Interrupt D Routing Control Register
**
**	  <7> - Interrupt D Routing Control:
**		    0:  Interrupt D Routing Enabled
**		    1:  Interrupt D Routing Disabled
*/
    outcfgb( &pb, CY82C693_PCI_INTD_Routing, 0x80 );
/*
**  Register 14 - PCI Control Register
**
**          <7> - Reserved
**	  <6:5> - PCI Master Grant Arbitration Time = REEVALUATE AFTER 16 CLOCK
**	    <4> - Retry on Excessive Delay Control = DISABLED
**	    <3> - Reserved
**	    <2> - PCI Post-Write Buffer Control = ENABLED
**          <1> - ISA Master Line Buffer Control = SINGLE 8/16-BIT XFERS
**          <0> - DMA Line Buffer Control = SINGLE 8/16-BIT XFERS
*/
    outcfgb( &pb, CY82C693_PCI_Control, 0x24 );
/*
**  Register 15 - PCI Error Control Register
**
**          <7> - SERR Reporting on Target-Abort Control = ENABLED
**	    <6> - SERR Reporting on Special Cycle Data Parity Errors Control = ENABLED
**	    <5> - SERR Reporting on Address Parity Errors Control = ENABLED
**	  <4:2> - Reserved
**          <1> - ISA Master Post-Write = DISABLED
**          <0> - DMA Post-Write = DISABLED
*/
    outcfgb( &pb, CY82C693_PCI_Error_Control, 0xE0 );
/*
**  Register 17 - PCI BIOS Control Register
**
**	<7> - Set to generate ROMCS for addresses 0xFFF80000 to 0xFFFDFFFF
**	<6> -             "                       0x000E0000 to 0x000EFFFF
**                                            and 0xFFFE0000 to 0xFFFEFFFF
**	<5> - ROM Write-protect Control (Set 1 to enable writes on ROMCS cycles)
*/                                                     
    outcfgb( &pb, CY82C693_PCI_BIOS_Control, 0xE0 );
/*
**  Register 18 - ISA/DMA Top of Memory Control Register
**
**	<7:4> - Top of ISA Memory = 16 MBytes
**	<3:0> - ISA/DMA BIOS Forwarding Control:
**		<0> - ISA/DMA 768K - 832K BIOS Forwarding = DISABLED
**		<1> - ISA/DMA 832K - 896K BIOS Forwarding = DISABLED
**		<2> - ISA/DMA 512K - 640K BIOS Forwarding = DISABLED
**		<3> - ISA/DMA Low BIOS Forwarding = DISABLED
*/
    outcfgb( &pb, CY82C693_ISA_DMA_TopOfMemory, 0xF0 );
/*
**  Register 19 - AT Control Register #1
**
**          <7> - Reserved
**	    <6> - PIO IDE Routable Interrupt Request Control = HARDWIRED TO IRQ14/15
**	    <5> - IDE DMA Status (Read Only)
**	    <4> - Keyboard Controller Status (Read Only)
**          <3> - 16-bit I/O Recovery Control = DISABLED
**          <2> - 8-bit I/O Recovery Control = DISABLED
**        <1:0> - ATCLK Control = PCICLK DIVIDED BY 4
*/
    outcfgb( &pb, CY82C693_AT_Control_1, 0x40 );
/*
**  Register 21 - PCI IDE Interrupt Request 0 Routing Control Register
**
**	  <7> - IDE Interrupt 0 Routing Control:
**		    0:  Interrupt 0 Routing Enabled
**		    1:  Interrupt 0 Routing Disabled
*/
    outcfgb( &pb, CY82C693_IDE_IRQ_0_Routing, 0x80 );
/*
**  Register 22 - PCI IDE Interrupt Request 1 Routing Control Register
**
**	  <7> - IDE Interrupt 1 Routing Control:
**		    0:  Interrupt 1 Routing Enabled
**		    1:  Interrupt 1 Routing Disabled
*/
    outcfgb( &pb, CY82C693_IDE_IRQ_1_Routing, 0x80 );
/*
**  Register 23 - Stand-Alone Control and USB Host Control Register
**
**          <7> - External PCI Arbiter Protocol Control
**	  <6:5> - Flush Request/Acknowledge Handshake Control
**	    <4> - IDE Controller Decode Control = 64KB
**	    <3> - ROM Space Decode Control = 512KB 
**	    <2> - Reserved
**          <1> - Reserved
**          <0> - USB Interface Control = ENABLED
*/
    outcfgb( &pb, CY82C693_SA_USB_Control, 0xF1 );
/*
** Set up 32-byte cache line size on USB controller
*/
    pb.function = 3;
    outcfgl( &pb, 0x40, 0x00000000 );
/*
** The following code fixes a potential deadlock problem between
** an IDE I/O read and an ISA DMA cycle, by allowing I/O accesses
** not completed by 16 PCI clocks to be retried.
*/
    pb.function = 1;   /* Primary IDE interface */
    outcfgl( &pb, 0x40, ( incfgl( &pb, 0x40 ) | 0x02000 ) );

    pb.function = 2;   /* Secondary IDE interface */
    outcfgl( &pb, 0x40, ( incfgl( &pb, 0x40 ) | 0x02000 ) );
/*
** Clear NMI status
*/
    pb.function = 0;   /* PCI-ISA bridge */
    outportb( &pb, 0x61, inportb( &pb, 0x61 ) | 0x04 );
    outportb( &pb, 0x61, inportb( &pb, 0x61 ) & 0xFB );
/*
** Initialize the SMC37c669 Super I/O Controller
*/
    SMC_init( );
/*                  
** Initialize the PCI interrupt Controller registers
*/                  
    temp = ReadTsunamiCSR( cchip_offset( primary_cpu, CSR_DIM0 ) );
    temp |= ((uint64)1 << 48) | ((uint64)1 << 55) | ((uint64)0xE << 60);
    WriteTsunamiCSR( cchip_offset( primary_cpu, CSR_DIM0 ), temp );
/*
** Initialize the slave ISA interrupt controller
**   
**  ICW1 - ICW4 write required
**  ICW2 - Vector table base address = 8h
**  ICW3 - Slave identification code = 2h
**  ICW4 - x86 mode, normal EOI
**  OCW1 - IRQ8 - IRQ15 disabled
*/
    pb.function = 0;   /* PCI-ISA bridge */
    outportb( &pb, CY82C693_IntC2_ICW1, CY82C693_IntC2_DICW1 );
    outportb( &pb, CY82C693_IntC2_ICW2, CY82C693_IntC2_DICW2 );
    outportb( &pb, CY82C693_IntC2_ICW3, CY82C693_IntC2_DICW3 );
    outportb( &pb, CY82C693_IntC2_ICW4, CY82C693_IntC2_DICW4 );
    outportb( &pb, CY82C693_IntC2_OCW1, CY82C693_IntC2_DOCW1 );
/*
** Initialize the master ISA interrupt controller
**
**  ICW1 - ICW4 write required
**  ICW2 - Vector table base address = 0h
**  ICW3 - Slave controller connected to IRQ2
**  ICW4 - x86 mode, normal EOI
**  OCW1 - IRQ2 enabaled, IRQ0, IRQ1, IRQ3-IRQ7 disabled
*/
    outportb( &pb, CY82C693_IntC1_ICW1, CY82C693_IntC1_DICW1 );
    outportb( &pb, CY82C693_IntC1_ICW2, CY82C693_IntC1_DICW2 );
    outportb( &pb, CY82C693_IntC1_ICW3, CY82C693_IntC1_DICW3 );
    outportb( &pb, CY82C693_IntC1_ICW4, CY82C693_IntC1_DICW4 );
    outportb( &pb, CY82C693_IntC1_OCW1, CY82C693_IntC1_DOCW1 );
/*
** Send a non-specific EOI command to clear any pending ISA interrupts
*/
    for ( i = 0;  i < 8;  i++) {
	outportb( &pb, CY82C693_IntC2_OCW2, CY82C693_EOI_NS | i );
	outportb( &pb, CY82C693_IntC1_OCW2, CY82C693_EOI_NS | i );
    }
}
                                                                          
/*+                                                                        
 * ============================================================================
 * = in_flash - Read data from flash ROM address space.                 =  
 * ============================================================================
 *                                                                         
 * OVERVIEW:                                                               
 *                                                                         
 *   	This routine will read a byte from the flash ROM address space.    
 *                                                                         
 * FORM OF CALL:                                                           
 *                                                                         
 *   	data = in_flash( pb, offset );                                     
 *                                                                         
 * RETURNS:      
 *   
 *   	unsigned char - Data from read.
 *   
 * ARGUMENTS:
 *   
 *	struct pb *pb - pointer to a device port block data structure.
 *	                See pb_def.h for more info.
 *
 *	unsigned int offset - Longword offset in flash ROM region to read.
 *
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

unsigned char in_flash( struct pb *pb, unsigned int offset ) 
{
    return( xtig( pb, offset, 0, ldq ) );
}                              
                               
                              
/*+                            
 * ============================================================================
 * = out_flash - Write data to flash ROM address space.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will write a byte to the flash ROM address space.
 *
 * FORM OF CALL:
 *
 *	out_flash( pb, offset, data );
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.                   
 *                                                                    
 *	unsigned int offset - Longword offset in flash ROM region to write.
 *
 *	unsigned __int64 - Data to write, passed by value.
 *
 * SIDE EFFECTS:
 *                                        
 *      None
 *
-*/

void out_flash( struct pb *pb, unsigned int offset, unsigned __int64 data ) 
{
    ( void )( xtig( pb, offset, data, stq ) );
}

                                                   
void PowerUpProgress ( unsigned char value, char *string, int a, int b, int c, int d )
{                                                            
    struct FILE *fp;
#if DEBUG                   
    qprintf( "CPU %d: %2x.", whoami( ), value );                        
    qprintf( string, a, b, c, d );
#endif                                           
                                                 
    outportb( NULL, 0x80, value );  
                 
    if (primary())
       cserve(CSERVE$IIC_WRITE, 0x80000000 | (~value<<16) | 0x4E);
}                 
                  
                 
probe_local( )    
{                 
    tsunami_probe_local( );
}                 

                                                     
void use_eisa_ecu_data( struct pb *pb )             
{                                                   
}                                                   

                                                                                                           
/*******************************************************************************
 *                                                                             *
 *                  Interrupt Handling and Support Routines                    *
 *                                                                             *
 ******************************************************************************/

/*
**
** Device Raw Interrupt Request Register (DRIR) 801.A000.0300
**
**  Bit	Interrupt    			Bit	Interrupt
**  63	reserved for Internal           31	PCI0 - intA0
**  62  PCI0_BUS_ERROR                  30 	PCI0 - intB0
**  61  PCI1_BUS_ERROR                  29	PCI0 - intC0
**  60-56 Unused                        28	PCI0 - intD0
**  55  PCI0 - INT (from Cypress SIO)   27	PCI0 - intA1
**  54  PCI0 - SMI_INT (from Cypress)   26	PCI0 - intB1
**  53  PCI0 - NMI (from Cypress)	25	PCI0 - intC1
**  52-48 Unused			24	PCI0 - intD1
**  47  PCI1 - intA0			23 	PCI0 - intA2
**  46  PCI1 - intB0			22	PCI0 - intB2
**  45  PCI1 - intC0			21	PCI0 - intC2
**  44  PCI1 - intD0			20 	PCI0 - intD2
**  43  PCI1 - intA1			19	PCI0 - IRQ_ADPTA (Adaptec 7895)
**  42  PCI1 - intB1			18	PCI0 - IRQ_ADPTB (Adaptec 7895)
**  41  PCI1 - intC1			17-0 	Unused
**  40  PCI1 - intD1
**  39  PCI1 - intA2                                  
**  38  PCI1 - intB2
**  37  PCI1 - intC2
**  36  PCI1 - intD2
**  35  PCI1 - intA3
**  34  PCI1 - intB3
**  33  PCI1 - intC3
**  32  PCI1 - intD3
**
** AlphaPC 264/264DP Primary PCI Bus IDSEL and INT_PIN to INT_LINE Mappings
** -------------------------------------------------------------------------
** IDSEL                  PCI          Physical         INT_PIN/INT_LINE
** Device             Address Bit      Address       IntA  IntB  IntC  IntD
** -------------------------------------------------------------------------
** PCI0-to-ISA Bridge  PCI0_AD<16>   801.FE00.2800      Bits 55-53
** Adaptec SCSI        PCI0_AD<17>   801.FE05.3000      Bits 19-18
** PCI0 option 0       PCI0_AD<18>   801.FE00.3800     31    30    29    28
** PCI0 option 1       PCI0_AD<19>   801.FE00.4000     27    26    25    24
** PCI0 option 2       PCI0_AD<20>   801.FE00.4800     23    22    21    20
**
** PCI1 option 0       PCI0_AD<18>   803.FE00.3800     47    46    45    44
** PCI1 option 1       PCI0_AD<19>   803.FE00.4000     43    42    41    40
** PCI1 option 2       PCI0_AD<20>   803.FE00.4800     39    38    37    36
** PCI1 option 3       PCI0_AD<21>   803.FE00.5000     35    34    33    32
**
*/
unsigned char pci_irq_table[2][28] = {
      /* PChip 0 */
   { 255, 255, 255, 255,    /* PCI0 slot 5  - Cypress PCI/ISA Bridge */
      19,  18, 255, 255,    /* PCI0 slot 6  - Adaptec SCSI */
      31,  30,  29,  28,    /* PCI0 slot 7  - PCI0 option 0 */
      27,  26,  25,  24,    /* PCI0 slot 8  - PCI0 option 1 */
      23,  22,  21,  20,    /* PCI0 slot 9  - PCI0 option 2 */
     255, 255, 255, 255,    /* PCI0 slot 10 - N/A */
       0,   0,   0,   0
   },
      /* PChip 1 */
   { 255, 255, 255, 255,    /* PCI1 slot 5  - N/A */
     255, 255, 255, 255,    /* PCI1 slot 6  - N/A */
      47,  46,  45,  44,    /* PCI1 slot 7  - PCI1 option 0 */
      43,  42,  41,  40,    /* PCI1 slot 8  - PCI1 option 1 */
      39,  38,  37,  36,    /* PCI1 slot 9  - PCI1 option 2 */
      35,  34,  33,  32,    /* PCI1 slot 10 - PCI1 option 2 */
       0,   0,   0,   0
   }                                                 
};
unsigned char vector_allocated[ 64 ];

/*+
 * ============================================================================
 * = sys_irq_init - Initialize PCI irq table				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Initialize the PCI irq allocation table entries to default values.
 *      Called from POWERUP.C.
 *                        
 * FORM OF CALL: 
 *               
 *	sys_irq_init( )
 *                    
 * FUNCTIONAL DESCRIPTION 
 *               
 *	Clear the PCI irq allocation entries so drivers can allocate interrupt 
 *	vectors.
 *
-*/
void sys_irq_init( void )
{                                        
    int i;                               
                                         
    for ( i = 0;  i < sizeof( vector_allocated );  i++ ) {
    	vector_allocated[i] = 0;
    }
}

/*+
 * ============================================================================
 * = assign_pci_vector - Allocate a PCI device interrupt routing assignment   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will allocate an interrupt routing assignment for a
 *	PCI device base on the specified slot number and interrupt pin.
 *
 * FORM OF CALL:
 *
 *	assign_pci_vector( pb, pin, base_pin, slot );
 *                    
 * RETURNS:
 *
 *   	Interrupt routing assignment, otherwise, -1.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	      int pin - interrupt pin register in PCI config space.
 *	     int slot - PCI slot number
 *
 * SIDE EFFECTS:
 *
 *      An interrupt vector may also be assigned.
 *
-*/
int assign_pci_vector( struct pb *pb, int pin, int base_pin, int slot )
{
    int irq;
    unsigned short class;
    unsigned int id;

    irq = pci_irq_table[pb->hose][( ( slot - PCI_MIN_SLOT ) * 4 ) + pin - 1];
    if ( irq == 0xFF ) {
/*
** Special case handling for PCI devices routed through ISA IRQs
*/
	id = incfgl( pb, 0 );
	if ( id == 0xC6931080 ) {
	    class = incfgw( pb, 0x0A );
/*
** IDE interrupts are routed through ISA IRQ 14 & 15
*/
	    if ( class == 0x0101 ) {
		if ( pb->function == 1 )
		    irq = 0xE0 + 14;
		else if ( pb->function == 2 )
		    irq = 0xE0 + 15;
	    }
/*
** USB interrupt is routed through ISA IRQ 10
*/
	    else if ( class == 0x0C03 ) {
		irq = 0xE0 + 10;
	    }
	}
    }
    if ( irq == 0xFF ) {
    	pb->vector = 0;
    }
    else if ( irq >= 0xE0 ) {
    	pb->vector = ( irq - 0xE0 ) + ISA_BASE_VECTOR;
    }
    else {
	if ( vector_allocated[irq] ) {
	    err_printf(              
		"Vector allocation failed for hose %d, bus %d, slot %d, pin %d, irq %d\n",
		 pb->hose, pb->bus, slot, pin, irq );                                          
	    err_printf(                       
		"Vector %04x already allocated\n", PCI_BASE_VECTOR + irq );
	    pb->vector = 0;
	}                                     
	else {                                                                      
	    vector_allocated[irq] = 1;
	    pb->vector = irq + PCI_BASE_VECTOR;
	}
    }
#if ((IPL_RUN) && (IPL_RUN > 19))
    pb->vector = 0;
#endif      
    return( irq );
}

/*+
 * ============================================================================
 * = io_get_vector - Allocate an ISA device interrupt vector                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will allocate an ISA device interrupt vector for the 
 *	specified IRQ channel.
 *
 *	The following table shows the allocation of ISA device interrupt 
 *	vectors:
 *
 *	IRQ	Vector	       Typical Interrupt Source
 *	====	======	========================================
 *	irq0	 0x80	Reserved - Interval timer 1 counter 0 output
 *	irq1	 0x81	Keyboard controller
 *	irq2	 0x82	Reserved - INTR from slave controller
 *	irq3	 0x83	COM2 serial port
 *	irq4	 0x84	COM1 serial port
 *	irq5	 0x85	Available
 *	irq6	 0x86	Floppy controller
 *	irq7	 0x87	Parallel port 1
 *	irq8	 0x88	Reserved
 *	irq9	 0x89	Available
 *	irq10	 0x8A	Universal Serial Bus controller
 *	irq11	 0x8B	Available
 *	irq12	 0x8C	Mouse controller
 *	irq13	 0x8D	Available
 *	irq14    0x8E	IDE controller
 *	irq15	 0x8F	IDE controller
 *
 * FORM OF CALL:
 *
 *	io_get_vector( pb, irq );
 *                    
 * RETURNS:
 *
 *   	Interrupt vector allocated (base vector + (IRQ from 0 to 15) ),
 *	otherwise, zero if vector not allocated.
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned int irq - An ISA IRQ channel number.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
unsigned int io_get_vector( struct pb *pb, unsigned int irq )
{
    unsigned int vector;

    vector = ISA_BASE_VECTOR + irq;

#if ((IPL_RUN) && (IPL_RUN > 19))
    vector = 0;
#endif

    return( vector );
}

/*+
 * ============================================================================
 * = io_issue_eoi - Issue an End-Of-Interrupt command.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will issue an End-Of-Interrupt (EOI) command to
 *	clear the specified interrupt request.
 *
 * FORM OF CALL:
 *
 *	io_issue_eoi( pb, vector );
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *                                                       
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned int vector - A device interrupt vector.
 *
-*/
void io_issue_eoi( struct pb *pb, unsigned int vector )
{
    unsigned int irq;
/*
** Is this an ISA device interrupt vector?
*/
    if ( ( vector & 0xF0 ) == ISA_BASE_VECTOR ) {
	irq = vector & 0x0F;
/*
** Issue a Specific EOI command to clear the interrupt request
** corresponding to IRQ0 - IRQ7 by writing the Operation Control
** Word 2 (OCW) register on interrupt controller 1.
*/                             
	spinlock( &spl_kernel );
	if ( irq < 8 ) {       
	    outportb( pb, CY82C693_IntC1_OCW2, CY82C693_EOI + ( irq & 7 ) );
/*                             
** Issue a Specific EOI command to clear the interrupt request
** corresponding to IRQ8 - IRQ15 by writing the Operation Control
** Word 2 (OCW) register on interrupt controller 2.
**                             
** Cascaded interrupt controller configurations must also issue
** a second Specific EOI command to clear the slave input (IRQ2) 
** on the master controller.   
*/                             
	} else {               
	    outportb( pb, CY82C693_IntC2_OCW2, CY82C693_EOI + ( irq & 7 ) );
	    outportb( pb, CY82C693_IntC1_OCW2, CY82C693_EOI + 2 );
	}                      
	spinunlock( &spl_kernel );
    }                          
}                              
                              
/*+                            
 * ============================================================================
 * = io_disable_interrupts - Disable a PCI or ISA device interrupt.           =
 * ============================================================================
 *                             
 * OVERVIEW:                   
 *                             
 *	This routine will disable a PCI or ISA device interrupt by setting
 *	the appropriate bit in a mask register.
 *
 * FORM OF CALL:
 *
 *	io_disable_interrupts( pb, vector );
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned int vector - An ISA or PCI device interrupt vector.
 *
-*/
void io_disable_interrupts( struct pb *pb, unsigned int vector )
{
    unsigned int irq;
    unsigned __int64 tmp;
/*
** Is this an ISA device interrupt vector?
*/
    if ( ( vector & 0xF0 ) == ISA_BASE_VECTOR ) {            
	irq = vector & 0x0F;
/*
** Disable the corresponding IRQ0 - IRQ7 channel in the Interrupt Mask 
** Register (IMR) by writing the Operation Control Word 1 (OCW1) register 
** on interrupt controller 1.
*/
	spinlock( &spl_kernel );
	if ( irq < 8 ) {
	    outportb( pb, CY82C693_IntC1_OCW1, inportb ( pb, CY82C693_IntC1_OCW1 ) | ( 1 << ( irq & 7 ) ) );
/*
** Disable the corresponding IRQ8 - IRQ15 channel in the Interrupt Mask 
** Register (IMR) by writing the Operation Control Word 1 (OCW1) register 
** on interrupt controller 2.
*/
	} else {
	    outportb( pb, CY82C693_IntC2_OCW1, inportb ( pb, CY82C693_IntC2_OCW1 ) | ( 1 << ( irq & 7 ) ) );
	}
	spinunlock( &spl_kernel );
/*
** Make sure the In Service (IS) bit for this interrupt is cleared.
*/
	io_issue_eoi( pb, vector );
    }
/*
** Not ISA, must be a PCI device interrupt vector.
*/
    else {
	tmp = ReadTsunamiCSR( cchip_offset ( primary_cpu, CSR_DIM0 ) );
	tmp &= ~( ( unsigned __int64 )1 << ( vector - 0x90 ) );
	WriteTsunamiCSR( cchip_offset( primary_cpu, CSR_DIM0 ), tmp );
    }
}

/*+
 * ============================================================================
 * = io_enable_interrupts - Enable a PCI or ISA device interrupt.             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will enable a PCI or ISA device interrupt by clearing
 *	the appropriate bit in a mask register.
 *
 * FORM OF CALL:
 *
 *	io_enable_interrupts( pb, vector );
 *                    
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct pb *pb - pointer to a device port block data structure.
 *			See pb_def.h for more info.
 *
 *	unsigned int vector - An ISA or PCI device interrupt vector.
 *
-*/
void io_enable_interrupts( struct pb *pb, unsigned int vector )
{
    unsigned int irq;
    unsigned __int64 tmp;
/*
** Is this an ISA device interrupt vector?
*/
    if ( ( vector & 0xF0 ) == ISA_BASE_VECTOR ) {
	irq = vector & 0x0F;
/*
** Enable the corresponding IRQ0 - IRQ7 channel in the Interrupt Mask 
** Register (IMR) by writing the Operation Control Word 1 (OCW1) register 
** on interrupt controller 1.
*/
	spinlock( &spl_kernel );
	if ( irq < 8 ) {
	    outportb( pb, CY82C693_IntC1_OCW1, inportb ( pb, CY82C693_IntC1_OCW1 ) & ~( 1 << ( irq & 7 ) ) );
/*
** Enable the corresponding IRQ8 - IRQ15 channel in the Interrupt Mask 
** Register (IMR) by writing the Operation Control Word 1 (OCW1) register 
** on interrupt controller 2.
*/
	} else {
	    outportb( pb, CY82C693_IntC2_OCW1, inportb ( pb, CY82C693_IntC2_OCW1 ) & ~( 1 << ( irq & 7 ) ) );
	}
	spinunlock( &spl_kernel );
/*
** Make sure the In Service (IS) bit for this interrupt is cleared.
*/
	io_issue_eoi( pb, vector );
    }
/*
** Not ISA, must be a PCI device interrupt vector.
*/
    else {
	tmp = ReadTsunamiCSR( cchip_offset ( primary_cpu, CSR_DIM0 ) );
	tmp |= ( ( unsigned __int64 )1 << ( vector - 0x90 ) );
	WriteTsunamiCSR( cchip_offset( primary_cpu, CSR_DIM0 ), tmp );
    }
}

/*+
 * ============================================================================
 * = pc264_ipir - platform-specific interprocessor interrupt handler          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle all interprocessor interrupts
 *
 * FORM OF CALL:
 *
 *	pc264_ipir( ); 
 *
 * RETURNS:
 *
 *      None
 *                                
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void pc264_ipir( )
{
    pprintf( "!!!!!!!!!!!!!! IP interrupt on CPU %d\n", whoami( ) );
}

/*+
 * ============================================================================
 * = pc264_perfmon_isr - platform-specific perf montor interrupt handler      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle all performance monitor interrupts
 *
 * FORM OF CALL:
 *
 *	pc264_perfmon_isr( ); 
 *
 * RETURNS:
 *
 *      None
 *                                
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void pc264_perfmon_isr( void )                  
{                                             
    pprintf( "!!!!!!!!!!!!!! Performance monitor interrupt on CPU %d\n", whoami( ) );
}                                             

/*+
 * ============================================================================
 * = pc264_unexpected_int - Handle all unexpected interrupts.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle all unexpected interrupts.
 *
 * FORM OF CALL:
 *
 *	pc264_unexpected_int( ); 
 *
 * RETURNS:
 *
 *      None
 *                                
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void pc264_unexpected_int( void )
{
    pprintf("\n*** Unexpected interrupt .................\n");
    do_bpt();
}
/*                                     
** =========================           
** iic_write_csr                       
** ==========================          
*/                                                                      
int iic_write_csr(char data, int addr, struct FILE * unusedParameter) {                  
#if IIC_DEBUG                                                           
     if (enable_iic_prints)                                             
     pprintf("iic_write_csr(data: 0x%x, addr: 0x%x) called\n",data, addr);
#endif                                                    
   outmemb (0, 0xfff80000 | addr, data);
      /* 
      // The chip select on the iic chip must toggle before the next byte
      // can be written.  Force the ESC chip select to toggle by reading 
      // another address 
      */
   inportb (0, 0x80);                                 
   return(0);
}                                             
                       
/*                                                   
 * ============================================================================
 * = iic_read_csr - Read a CSR on the IIC bus controler                       =
 * ============================================================================
*/                                                     
char iic_read_csr(int addr,struct FILE *fp) {
       	char data;                      
       	data = inmemb (0, 0xfff80000 | addr);            
       	   /* 
 	   // The chip select on the iic chip must toggle before the next byte 
       	   // can be read.  Force the ESC chip select to toggle by reading
       	   // another address 
           */                          
       	inportb (0, 0x80);                                 
#if IIC_DEBUG 
        if (enable_iic_prints)                              
        {                
          static uint32 last_addr=0;       
          static uint32 last_data=0;           

          if (addr != last_addr || data != last_data)        
          {                
            pprintf("iic_read_csr(addr: 0x%x, fp: 0x%x) called.", addr, fp);
   	      pprintf(" data = 0x%x.\n", data);            
          }                                              
          last_addr = addr;                              
          last_data = data;    
        }                      
#endif                                                 
       	return data;                                   
}                                                      
                                                       
/*                                                     
//==============================                       
//  iic_chip_reset(void)                               
//==============================                       
*/                                                     
void iic_chip_reset(void) {                            
	/*                     
  	// This is redundant. This inialization is done
        // in iic_reset in iic_driver.c
    	*/                     
#if 0                          
  outmemb(0, 0xfff80001, 0x80);                  
  outmemb(0, 0xfff80000, 0x5b);                  
  outmemb(0, 0xfff80001, 0xA0);                  
  outmemb(0, 0xfff80000, 0x18); /* 8MHz system , 90KHz SCL frequency. */
  outmemb(0, 0xfff80001, 0xC1);                                         
#endif
return;
}                                                                       


/*+
 * ============================================================================
 * = mc_sg_map_on - Enable MC non 1 to 1 mapping
 * ============================================================================
 *     
 * OVERVIEW:
 *     	This routine and its companion mc_sg_map_off will enable anothor
 *     	location for the Memory channel adapter to point its PCT page too.
 *     	The location is currently 5mb 1 to 1; 1mb window.
 *
 *     	MC_CABLE expects to be able to get to address 524000 which is
 *     	the address pct 36 will write it data.
 *
 * RETURN: 
 *     	The pci window area that was remapped.
-*/        
unsigned int mc_sg_map_on(struct pb *pb)
{
    if ( pb->hose == 1 )
	{
	WriteTsunamiCSR( PCHIP1_WSBA2, WSBA2_VALUE | wsba_m_ena  );
	WriteTsunamiCSR( PCHIP1_WSM2,  0 );	/* 0 mask = 1 Meg Window */
	WriteTsunamiCSR( PCHIP1_TBA2,  0x00500000 );
	}
	else
	{
	WriteTsunamiCSR( PCHIP0_WSBA2, WSBA2_VALUE | wsba_m_ena  );
	WriteTsunamiCSR( PCHIP0_WSM2,  0 );	/* 0 mask = 1 Meg Window */
	WriteTsunamiCSR( PCHIP0_TBA2,  0x00500000 );
	}                                                    
                                                   
    return WSBA2_VALUE; 	                      
}
/*
 * Turn off MC cable window.
*/
void mc_sg_map_off(struct pb *pb)                         
{                                                  
    if ( pb->hose == 1 )
	{
	WriteTsunamiCSR( PCHIP1_WSBA2, 0 );
	}
	else
	{
	WriteTsunamiCSR( PCHIP0_WSBA2, 0 );
	}
}                                                  

/*+
 * ============================================================================
 * = bios_sg_map_on - Enable BIOS emulation mapping via scatter/gather
 * ============================================================================
 *     
 * OVERVIEW:
 *     	This routine and its companion bios_sg_map_off will enable
 *	the BIOS emulator to perform DMA to "PC memory" addresses below
 *	1 meg. This platform specific routine programs the window base,
 *	window mask, and translated base registers to use the supplied
 *	scatter gather map.
 *
 * RETURN: 
 *     	None
-*/        
void bios_sg_map_on(int wbase, __int64 *sg_map, int hose)
{
    /* use the same registers that the mc diag will use */
    if ( hose == 1 )
	{
	WriteTsunamiCSR( PCHIP1_WSBA2, wbase | wsba_m_ena | wsba_m_sg ); 
	WriteTsunamiCSR( PCHIP1_WSM2,  0 );     /* 0 mask = 1 Meg Window */
	WriteTsunamiCSR( PCHIP1_TBA2,  sg_map );	    
	}
	else
	{
	WriteTsunamiCSR( PCHIP0_WSBA2, wbase| wsba_m_ena | wsba_m_sg );
	WriteTsunamiCSR( PCHIP0_WSM2,  0 );     /* 0 mask = 1 Meg Window */
	WriteTsunamiCSR( PCHIP0_TBA2,  sg_map );
	}                                                    

    /* invalidate the translation buffer */
    if (hose == 0) 
	{
	WriteTsunamiCSR( PCHIP0_TLBIA,  0 );
	} 
	else 
	{
	WriteTsunamiCSR( PCHIP1_TLBIA,  0 );
	}
}
/*+
 * ============================================================================
 * = bios_sg_map_off - Disable BIOS emulation mapping via scatter/gather
 * ============================================================================
 *     
 * OVERVIEW:
 *     	This routine disables the 0 to 1 meg "PC memory" scatter
 *	gather window.
 *
 * RETURN: 
 *     	None
-*/        
void bios_sg_map_off(int hose)
{
    /* use the same registers that the mc diag will use */
    if ( hose == 1 )
	{
	WriteTsunamiCSR( PCHIP1_WSBA2, 0 ); 
	WriteTsunamiCSR( PCHIP1_WSM2,  0 );     /* 0 mask = 1 Meg Window */
	WriteTsunamiCSR( PCHIP1_TBA2,  0 );	    
	}
	else
	{
	WriteTsunamiCSR( PCHIP0_WSBA2, 0 );
	WriteTsunamiCSR( PCHIP0_WSM2,  0 );     /* 0 mask = 1 Meg Window */
	WriteTsunamiCSR( PCHIP0_TBA2,  0 );
	}                                                    

}

int get_com1_baud( void )
{
    if ( rtc_read( 0x11 ) == 69 )
        return( rtc_read( 0x12 ) );
    else
        return( -1 );
}

void set_com1_baud( int baud )
{    
    rtc_write( 0x11, 69 );
    rtc_write( 0x12, baud );
}    

validate_com1_baud(char **value)                     
{                                                    
    int i;                                           
    char srev[8];                                    
    char buf[80];                                    
                                                     
    if (cbip)                                        
	return msg_success;                          
                                                     
    if ( ev_validate_table(value, baud_table) != msg_success )
	return msg_failure;                          
                                                     
    if( strcmp( *value, "9600" ) != 0 ) {            
	read_with_prompt("Embedded Remote Console only supports 9600 baud. Continue? (Y/[N]) ",sizeof(buf),buf,0,0,1);
	if (toupper(buf[0]) != 'Y')
	    return msg_failure;
    }
    return msg_success;
}
