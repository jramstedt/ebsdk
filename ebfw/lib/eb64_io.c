
/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

#ifndef LINT
static char *rcsid = "$Id: eb64_io.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 * $Log: eb64_io.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.32  1996/08/13  18:43:54  fdh
 * Removed a superfluous DMA controller setting.
 *
 * Revision 1.31  1996/07/11  16:00:48  fdh
 * Moved IOBusInit() to a separate module.
 *
 * Revision 1.30  1996/06/13  03:25:19  fdh
 * Modified data type returned from inmeml.
 *
 * Revision 1.29  1996/05/22  21:16:27  fdh
 * Moved includes to a single file to minimize makedepend warnings.
 *
 * Revision 1.28  1996/05/16  18:43:32  fdh
 * Removed aliases to inportx and outportx routines.
 *
 * Revision 1.27  1996/02/27  15:46:43  fdh
 * Renamed set_haxr() to SetHAE().  Modified it to
 * return the old value.
 *
 * Revision 1.26  1995/11/10  16:09:25  cruz
 * Added return statements to routines which required a return value.
 *
 * Revision 1.25  1995/10/31  22:49:52  cruz
 * Commented out code not in use.
 *
 * Revision 1.24  1995/10/26  23:47:22  cruz
 *  Fixed up prototypes.
 *
 * Revision 1.23  1995/10/13  14:14:20  fdh
 * Updated some internal function prototypes.
 *
 * Revision 1.22  1995/03/06  06:03:44  fdh
 * Make inrom() call ReadL() instead of ReadB().
 *
 * Revision 1.21  1995/03/05  12:04:10  fdh
 * Adjusted ROM macro to force longword alignment of the
 * addresses passesd to it.
 *
 * Revision 1.20  1995/03/05  04:20:38  fdh
 * Corrected set_haxr to or in previous state
 * of the system control register.
 * Corrected ROM macro and inrom().
 *
 * Revision 1.19  1995/01/20  21:15:13  fdh
 * Included prtrace.h
 *
 * Revision 1.18  1994/11/28  18:24:49  fdh
 * Added routine to set the "Host Address Extension"
 * portion of the System Control Register.
 *
 * Revision 1.17  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.16  1994/07/22  21:04:12  fdh
 * Moved rtc base address initialization to a routine that
 * can be called from demo programs.
 *
 * Revision 1.15  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.14  1994/06/03  18:54:42  fdh
 * Fixed return type for outVti().
 *
 * Revision 1.13  1994/04/04  15:20:28  fdh
 * Modified outLed() to make use of diagnostic card.
 *
 * Revision 1.12  1994/03/24  21:40:22  fdh
 * Removed unnecessary compile-time conditionals.
 *
 * Revision 1.11  1994/03/24  21:11:10  fdh
 * Moved csrv() to ffexec.c and fixed masks.
 *
 * Revision 1.10  1994/01/19  10:40:08  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.9  1993/11/22  16:30:52  rusling
 * Moved to a bus init routine called by main() (in cmd.c)
 * instead of from uart_init().
 *
 * Revision 1.8  1993/10/06  14:26:03  fdh
 * No longer sets RTC base address.  Read the current RTC
 * base address from the RTC address mapping register and
 * set the rtcBase global variable instead.  The RTC base
 * address is now established in the reset module of PALcode.
 *
 * Revision 1.7  1993/10/02  23:54:57  berent
 * Merge in development by Anthony Berent
 *
 *>>  Revision 1.6  1993/10/01  12:47:52  berent
 *>>  Start periodic interrupt in initisa; this avoids any dependency on the PAL
 *>>  code starting the periodic interrupt.
 *
 * Revision 1.6  1993/08/27  21:53:28  fdh
 * Modify IO shifts for clarity.
 *
 * Revision 1.5  1993/08/09  20:46:31  fdh
 * Establish base address of Real Time Clock as global variable.
 *
 * Revision 1.4  1993/07/28  12:32:44  fdh
 * Insure that address of the RTC is 0x70.
 *
 * Revision 1.3  1993/07/12  20:45:27  fdh
 * Cleaned up I/O initialization stuff. Should no longer
 * be duplicated in SROM.
 *
 * Revision 1.2  1993/06/18  14:33:25  fdh
 * Added clarification to comment about word (16 bit) I/O access.
 *
 * Revision 1.1  1993/06/08  19:56:20  fdh
 * Initial revision
 *
 */




/*------------------------------------------------------------------------**
**                                                                        **
**  EB64 I/O procedures                                                   **
**  Last modified by FDH May 04 1993                                      **
**                                                                        **
**      inport[b|w|t|l], outport[b|w|t|l] 8:16:24:32 IO xfers             **
**                                                                        **
**      inmem[b|w|t|l], outmem[b|w|t|l] 8:16:24:32 EISA memory xfers      **
**                                                                        **
**------------------------------------------------------------------------*/

/* Add includes to this file to minimize makedepend warnings */
#include "eb64_io.h"

/* EB64 ADDRESS BIT DEFINITIONS
 *
 *  3 3|3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1 
 *  3 2|1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  ^                                                 \_/ \_/
 *  |                                                  |   |
 *  +----  1 = I/O                       Byte Enable --+   |
 *         0 = Memory                                      |
 *                                       Transfer Length --+
 *
 *  CPU Adr[33]=1 select I/O space.
 *  CPU Adr[29:9] translate to IO Adr[22:2]
 *  System Register bits [16:8] provide IO Adr[31:23]
 *  CPU Adr[8:5] select transfer length and byte enables
 *
 *
 *   Byte      Transfer
 *   Enable    Length    Transfer  Byte    Address
 *   adr<8:7>  adr<6:5>  Length    Enable  Adder
 *   ---------------------------------------------
 *      00        00      Byte      1110   0x000
 *      01        00      Byte      1101   0x080
 *      10        00      Byte      1011   0x100
 *      11        00      Byte      0111   0x180
 *
 *      00        01      Word      1100   0x020
 *      01        01      Word      1001   0x0A0 <= Not supported in this code.
 *      10        01      Word      0011   0x120
 *      11        01      Reserved  ---    ---
 *
 *      00        10      Tribyte   1000   0x020
 *      01        10      Tribyte   0001   0x0A0
 *      1x        10      Reserved  ---    ---
 *
 *      10        11      Longword  0000   0x060
 *      01        11      Reserved  ---    ---
 *      1x        11      Reserved  ---    ---
 *
 *      Note that byte enables are asserted low.
 *
 */

#define BYTE_ENABLE_SHIFT 7
#define TRANSFER_LENGTH_SHIFT 5

/* Select kseg address space.
 * Kseg bits will be ignored when memory is mapped
 * one to one, virtual to physical
 */
#define IO	  (((ul)(-1)<<42)|((ul)1<<33))	/*    CPU Adr[33]=1 select I/O space.	*/

#define IO486  	  ((ul)0xC<<28)	/*    SC486 I/O (Adr[32:30]=011)	*/
#define MEM486 	  ((ul)0x8<<28)	/*    SC486 MEMORY (Adr[32:30]=010)	*/
#define IAck      ((ul)0x4<<28)	/*    Interrupt ACK (Adr[32:30]=001)	*/

#define Byt       ((ui)0<<TRANSFER_LENGTH_SHIFT)
#define Wrd       ((ui)1<<TRANSFER_LENGTH_SHIFT)
#define Tri       ((ui)2<<TRANSFER_LENGTH_SHIFT)
#define Lng       ((ui)3<<TRANSFER_LENGTH_SHIFT)

#define IO_B(x) ((ul)(IO|IO486|Byt|(((ul)(x)&0x7fffff)<<BYTE_ENABLE_SHIFT)))
#define IO_W(x) ((ul)(IO|IO486|Wrd|(((ul)(x)&0x7fffff)<<BYTE_ENABLE_SHIFT)))
#define IO_T(x) ((ul)(IO|IO486|Tri|(((ul)(x)&0x7fffff)<<BYTE_ENABLE_SHIFT)))
#define IO_L(x) ((ul)(IO|IO486|Lng|(((ul)(x)&0x7fffff)<<BYTE_ENABLE_SHIFT)))

#define MEM_B(x) ((ul)(IO|MEM486|Byt|(((ul)(x)&0x7fffff)<<BYTE_ENABLE_SHIFT)))
#define MEM_W(x) ((ul)(IO|MEM486|Wrd|(((ul)(x)&0x7fffff)<<BYTE_ENABLE_SHIFT)))
#define MEM_T(x) ((ul)(IO|MEM486|Tri|(((ul)(x)&0x7fffff)<<BYTE_ENABLE_SHIFT)))
#define MEM_L(x) ((ul)(IO|MEM486|Lng|(((ul)(x)&0x7fffff)<<BYTE_ENABLE_SHIFT)))

int rtcBase;   /* Base address of Real Time Clock */

/* ----- port routines -------------*/

void outportb(ui p,ui d){d&=0xff; WriteL(IO_B(p),d<<(8*(p&3))); mb();}
void outportw(ui p,ui d){d&=0xffff; WriteL(IO_W(p),d<<(8*(p&2))); mb();}
void outportl(ui p,ui d){WriteL(IO_L(p),d); mb();}


ui inportb(ui p){return ((ui)(ReadL(IO_B(p))>>((p&3)*8))&(ui)0xFF);}
ui inportw(ui p){return ((ui)(ReadL(IO_W(p))>>((p&2)*8))&(ui)0xFFFF);}
ui inportl(ui p){return ((ReadL(IO_L(p)))&0xFFFFFFFF);}

/* ----- memory routines -------------*/

void outmemb(ui p,ui d){d&=0xff;WriteL(MEM_B(p),(d<<24)|(d<<16)|(d<<8)|d);mb();}
void outmemw(ui p,ui d){d&=0xffff;WriteL(MEM_W(p),(d<<16)|d);mb();}
void outmeml(ui p,ui d){WriteL(MEM_L(p),d);mb();}

ui inmemb(ui p){return ((ui)(ReadL(MEM_B(p))>>((p&3)*8))&(ui)0xFF);}
ui inmemw(ui p){return ((ui)(ReadL(MEM_W(p))>>((p&2)*8))&(ui)0xFFFF);}
ui inmeml(ui p){return ((ReadL(MEM_L(p)))&0xFFFFFFFF);}

/*
 *  Not implemented but are here as a place holder
 */
void out_ioc_csr(ui p , ul d){}
ul in_ioc_csr(ui p){return 0;}

/*
 * Dense space is not implemented on this design.
 * Aliased to Sparse Space.
 */
void outdmemb(ui p , ui d){outmemb(p, d);}
void outdmemw(ui p , ui d){outmemw(p, d);}
void outdmeml(ui p , ui d){outmeml(p, d);}
ui indmemb(ui p){return inmemb(p);}
ui indmemw(ui p){return inmemw(p);}
ui indmeml(ui p){return inmeml(p);}


/*
 *  This routine sets up the Host Address Extension register.
 */
ui SetHAE (ui Value)
{
  ui OldValue;
  OldValue = insctl();
  outsctl((OldValue & ~0x1ff00) | ((Value&0xff800000) >> (23-8)));
  return ((OldValue << (23-8)) & 0xff800000);
}


/*
 * SYSTEM ROM/REGISTER (Adr[32:30]=000)
 * This address is shared between the System ROM and System Configuration
 * Register.  On read accesses the lower 8 bits are returned from whatever
 * ROM location is addressed, and the upper 16 bits always come from the
 * SYSTEM REGISTER. On write accesses, the SYSTEM REGISTER data must
 * appear on the upper 24 bits and the lower 8 bits are ignored.
 * Note that the ROM bytes are accessed on a longword boundary.
 */
#define ROM(x) (IO|Lng|(((ul)(x)&0x7ffffc)<<(BYTE_ENABLE_SHIFT)))
ub inrom(ui p){return(ReadL(ROM(p)) & (ui)0xFF);}
ui insctl(void) {return((ReadL(ROM(0)))&0xFFFFFF00);}
void outsctl(ui d) {WriteL(ROM(0),(d&0xFFFFFF00));mb();}

/*
 * Interrupt ACK (Adr[32:30]=001)
 * A read to this address zone will perform an interrupt acknowledge
 * to the SC486 chip to obtain a vector and clear out the interrupt.
 */
ui inIack(void){return(*((ui*)(IO|IAck))&0xffL);}

void ISABusInit(void)
{
  
/* VL82C113 */
#define IDX486	0xEC
#define DATA486	0xED
#define KBDCTRL	0x1D

  ub regB;

  /*  TCLK2=50Mhz   ==>   Make SYSCLK=8.33Mhz  */
  outportb(IDX486,0x06);	/* set SYSCLK to TCLK2/6 */
  outportb(DATA486,0x11);	/* set SYSCLK to TCLK2/6 */

  /* Enable 0x100000 to 0x7FFFFF as local bus memory (DMA) */
  outportb(IDX486,0x20);
  outportb(DATA486,0xa0);  /* 8MB region starting at 0x00 */
  outportb(IDX486,0x21);
  outportb(DATA486,0xfe);  /* Enable all subregions except 1st Meg */
  
  /* Enable 0x800000 to 0xFFFFFF as local bus memory (DMA) */
  outportb(IDX486,0x22);
  outportb(DATA486,0xA1);  /* 8MB region starting at 8MB */
  outportb(IDX486,0x23);
  outportb(DATA486,0xFF);  /* Enable all subregions */

  rtcBaseInit();

  /* Set up and start periodic timer interrupt at 976.562 usec intervals 
     (see VL8C113 data sheet) */

  outportb(rtcBase, 0x0A); /* Prepare to write interval to RTC register A */
  outportb(rtcBase+1,0x06); /* A value of 6 means 976.562 usec */
  outportb(rtcBase, 0x0B); /* Prepare to read value in register B */
  regB = (ub)inportb(rtcBase+1);
  outportb(rtcBase, 0x0B); /* Prepare to write value to register B 
			      (the VL8C113's index register has to 
			      be set before every access) */
  outportb(rtcBase+1, regB | 0x40);  /* Set the Periodic Interrupt Enable bit */
  
  outportb(IDX486,0x01);	/* set up sc486 RAMTMG CLKIN setup to 13ns */
  outportb(DATA486,0xbf);	/* set up sc486 RAMTMG CLKIN setup to 13ns */
  
  
  /*
   * Initialize the 82C113 keyboard
   * controller's KBDCTRL register
   */
  outportb(IDX486, KBDCTRL);
  outportb(DATA486, 0xc5);	/* Select PS/2 mode */
}

void outLed(ui d) { outportb(LEDPORT, d); }


void rtcBaseInit(void)
{
  /*
   * Read base address of Real Time Clock (see VL82C113 data sheet)
   */
  outportb(IDX486,0x1c);
  rtcBase = (inportb(DATA486)&0xff) << 0x8;
  outportb(IDX486,0x1b);
  rtcBase |= inportb(DATA486)&0xfe;	/* Bit 0 enables the RTC */
}
