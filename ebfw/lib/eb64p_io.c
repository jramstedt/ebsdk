
/*****************************************************************************

       Copyright 1993, 1994, 1995 Digital Equipment Corporation,
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
static char *rcsid = "$Id: eb64p_io.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 * $Log: eb64p_io.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.31  1996/08/13  18:42:33  fdh
 * Removed a superfluous DMA controller setting.
 * Check for Target Aborts as well as Master Aborts
 * when checking for "No Device" errors.
 *
 * Revision 1.30  1996/07/11  16:00:33  fdh
 * Moved IOBusInit() to a separate module.
 *
 * Revision 1.29  1996/06/13  03:25:31  fdh
 * Modified data type returned from inmeml.
 *
 * Revision 1.28  1996/05/22  21:17:21  fdh
 * Removed obsoleted i/o port function aliases.
 * Moved includes to a single file to minimize makedepend warnings.
 *
 * Revision 1.27  1995/12/13  15:42:49  cruz
 * Added casting from ul to ui in SetHAE().
 *
 * Revision 1.26  1995/10/31  22:50:04  cruz
 * Commented out code not in use.
 *
 * Revision 1.25  1995/10/13  14:14:20  fdh
 * Updated some internal function prototypes.
 *
 * Revision 1.24  1995/08/25  19:47:20  fdh
 * Added func arg to PCI Config space access routines.
 *
 * Revision 1.23  1995/07/05  20:22:53  cruz
 * Added code for accessing PCI dense memory space.
 *
 * Revision 1.22  1994/11/16  21:59:29  fdh
 * Corrected register mask in PCI configuration space
 * definitions.
 *
 * Revision 1.21  1994/11/11  20:23:56  fdh
 * Removed redundant HAXR2 definition.
 *
 * Revision 1.20  1994/11/08  21:35:01  fdh
 * Added routine to set the Host Address Extension Register.
 *
 * Revision 1.19  1994/11/01  11:31:26  rusling
 * Changed to support PCI-PCI bridges with Type 0
 * and Type 1 configuration address cycles.
 *
 * Revision 1.18  1994/08/08  21:52:32  fdh
 * Fixup RCS Id string.
 *
 * Revision 1.17  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.16  1994/07/26  18:51:17  fdh
 * Cleaned up MACROs used in out_ioc_csr() and in_ioc_csr().
 *
 * Revision 1.15  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.14  1994/06/03  18:55:13  fdh
 * Fixed return type for outVti().
 *
 * Revision 1.13  1994/04/04  15:20:06  fdh
 * Implemented outLed().
 *
 * Revision 1.12  1994/03/24  21:39:24  fdh
 * Removed unnecessary compile-time conditionals.
 *
 * Revision 1.11  1994/03/24  21:09:44  fdh
 * Moved csrv() to ffexec.c.
 * and fixed masks.
 *
 * Revision 1.10  1994/01/27  19:27:06  fdh
 * Added APECS/EPIC pass2 support.
 * APECS_PASS1 must be defined for pass1 support.
 *
 * Revision 1.9  1994/01/19  10:40:08  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.8  1993/12/15  10:32:22  rusling
 * Fixed up the setting up of the PCI to system address
 * translation registers.
 *
 * Revision 1.7  1993/11/24  15:06:32  rusling
 * Added IOPCIClearNODEV() routine for machine specific
 * PCI NODEV clearing/reading.
 *
 * Revision 1.6  1993/11/22  16:30:52  rusling
 * Moved to a bus init routine called by main() (in cmd.c)
 * instead of from uart_init().
 *
 * Revision 1.5  1993/11/19  17:01:51  fdh
 * Moved some things to global header file.
 * Added in_ioc_csr()
 *
 * Revision 1.4  1993/09/27  18:48:50  fdh
 * Correct PCI target to allow DMA access between 1 and 2 MB.
 * Took out obsolete sections of code.
 *
 * Revision 1.3  1993/08/21  03:15:07  fdh
 * Correct base address for IACK and Special cycles.
 * Cleaned up masks for i/o functions.
 * Turned off GAT and BP bits.
 * Establish DMA and ISA mastering in range of 1MB to 16MB.
 *
 * Revision 1.2  1993/08/19  07:20:40  fdh
 * added out_ioc_csr()
 * establish DMA access in range from 1MB to 16MB.
 *
 * Revision 1.1  1993/08/19  06:19:42  fdh
 * Initial revision
 *
 */




/*------------------------------------------------------------------------**
**                                                                        **
**  EB66 I/O procedures                                                   **
**                                                                        **
**      inport[b|w|t|l], outport[b|w|t|l] 8:16:24:32 IO xfers             **
**	inportbxt: 8 bits only                                            **
**      inport:    alias of inportw                                       **
**      outport:   alias of outportw                                      **
**                                                                        **
**      inmem[b|w|t|l], outmem[b|w|t|l] 8:16:24:32 ISA memory xfers       **
**	inmembxt: 8 bits only                                             **
**      inmem:    alias of inmemw                                         **
**      outmem:   alias of outmemw                                        **
**                                                                        **
**------------------------------------------------------------------------*/

#include "eb64p_io.h"

/* EB66 ADDRESS BIT DEFINITIONS
 *
 *  3 3|3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1 
 *  3 2|1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |0|0|0|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  \_/                                                   \_/ \_/
 *   |                                                     |   |
 *   +--  10 = PCI Mem,                      Byte Enable --+   |
 *        11 = PCI I/O                       Transfer Length --+
 *
 *  CPU Adr[33]=1 select I/O space.
 *  CPU Adr[31:7] translate to IO Adr[26:2]
 *  Note: When CPU Adr[31:29] != 0 IO Addr[31:27] is
 *        provided in HAE<31:27> of the IOC_CTRL register.
 *  CPU Adr[6:3] select transfer length and byte enables
 *  CPU Adr[2:0] Must be zero
 *
 *
 *   Byte      Transfer
 *   Enable    Length    Transfer  Byte    Address
 *   adr<6:5>  adr<4:3>  Length    Enable  Adder
 *   ---------------------------------------------
 *      00        00      Byte      1110   0x000
 *      01        00      Byte      1101   0x020
 *      10        00      Byte      1011   0x040
 *      11        00      Byte      0111   0x060
 *
 *      00        01      Word      1100   0x008
 *      01        01      Word      1001   0x028 <= Not supported in this code.
 *      10        01      Word      0011   0x048
 *
 *      00        10      Tribyte   1000   0x010
 *      01        10      Tribyte   0001   0x030
 *
 *      10        11      Longword  0000   0x058
 *
 *      Note that byte enables are asserted low.
 *
 */

#define BYTE_ENABLE_SHIFT 5
#define TRANSFER_LENGTH_SHIFT 3
#define IO_MASK 0x7ffffff

/*
 * The following macros define Type 0 and Type 1 configuration commands (addresses
 * punted out onto the PCI bus).
 * 
 * Type 0:
 *
 *  3 3|3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1 
 *  3 2|1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | | |D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|F|F|F|R|R|R|R|R|R|0|0|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	31:11	Device select bit.
 * 	10:8	Function number
 * 	 7:2	Register number
 *
 * Type 1:
 *
 *  3 3|3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1 
 *  3 2|1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | | | | | | | | | | |B|B|B|B|B|B|B|B|D|D|D|D|D|F|F|F|R|R|R|R|R|R|0|1|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	31:24	reserved
 *	23:16	bus number (8 bits = 128 possible buses)
 *	15:11	Device number (5 bits)
 *	10:8	function number
 *	 7:2	register number
 *  
 * Notes:
 *	The function number selects which function of a multi-function device 
 *	(eg scsi and ethernet.
 * 
 *	On the EB66 and EB64+, you select the device by picking a bit between 11 and 31
 *	(by left shifting).   This PCI code assumes that the low order device number is
 *	11, hence the lack of a shift by 11 bits.  Also note that bits 1:0 are
 *  	specified via a register.
 * 
 *	The register selects a DWORD (32 bit) register offset.  Hence it doesn't
 *	get shifted by 2 bits as we want to "drop" the bottom two bits.
 *
 */

#ifdef APECS_PASS1
#define PCI_CONFIG_TYPE_0(device,function,register) \
	( (ul)( (register & 0xff) | ((function & 0x7) << 8) | (1 << device) ) )

#define PCI_CONFIG_TYPE_1(bus,device,function,register) \
	( (ul)( (register & 0xff) | ((function & 0x7) << 8) | ((device & 0x1f) << 11) | \
		((bus & 0xff) << 16) ) )
#else
#define PCI_CONFIG_TYPE_0(device,function,register) \
	( (ul)( (register & 0xff) | ((function & 0x7) << 8) | ((device - 11) << 11) ) )

#define PCI_CONFIG_TYPE_1(bus,device,function,register) \
	( (ul)( (register & 0xff) | ((function & 0x7) << 8) | ((device & 0x1f) << 11) | \
		((bus & 0xff) << 16) ) )
#endif

#define PCI_IO	  ((ul)0x1c<<28) /*    CPU Adr[33:28]=1C select PCI I/O.	*/
#define PCI_MEM   ((ul)2<<32)    /*    CPU Adr[33:32]=2 select PCI Mem.	*/
#define PCI_CFG   ((ul)0x1E<<28) /*    CPU Adr[33:28]=1E select PCI Cfg.	*/
#define IOC_CSR(x) (((ul)0x1A<<28)|(ul)(x))
#define IACK      ((ul)0x1B<<28)
#define PCI_D_MEM ((ul)0x3<<32)    /*  select PCI dense Mem.*/

#define Byt       ((ul)0<<TRANSFER_LENGTH_SHIFT)
#define Wrd       ((ul)1<<TRANSFER_LENGTH_SHIFT)
#define Tri       ((ul)2<<TRANSFER_LENGTH_SHIFT)
#define Lng       ((ul)3<<TRANSFER_LENGTH_SHIFT)

#define IO_B(x) ((ul)(PCI_IO|Byt|(((ul)(x)&IO_MASK)<<BYTE_ENABLE_SHIFT)))
#define IO_W(x) ((ul)(PCI_IO|Wrd|(((ul)(x)&IO_MASK)<<BYTE_ENABLE_SHIFT)))
#define IO_T(x) ((ul)(PCI_IO|Tri|(((ul)(x)&IO_MASK)<<BYTE_ENABLE_SHIFT)))
#define IO_L(x) ((ul)(PCI_IO|Lng|(((ul)(x)&IO_MASK)<<BYTE_ENABLE_SHIFT)))

#define MEM_B(x) ((ul)(PCI_MEM|Byt|(((ul)(x)&IO_MASK)<<BYTE_ENABLE_SHIFT)))
#define MEM_W(x) ((ul)(PCI_MEM|Wrd|(((ul)(x)&IO_MASK)<<BYTE_ENABLE_SHIFT)))
#define MEM_T(x) ((ul)(PCI_MEM|Tri|(((ul)(x)&IO_MASK)<<BYTE_ENABLE_SHIFT)))
#define MEM_L(x) ((ul)(PCI_MEM|Lng|(((ul)(x)&IO_MASK)<<BYTE_ENABLE_SHIFT)))

#define D_MEM_L(x) ((ul)(PCI_D_MEM|((ul)(((x) & ~3) & 0xFFFFFFFF))))

#define CFG_B(command) ( (ul)( PCI_CFG|Byt|((command)<<BYTE_ENABLE_SHIFT) ) )
#define CFG_W(command) ( (ul)( PCI_CFG|Wrd|((command)<<BYTE_ENABLE_SHIFT) ) )
#define CFG_L(command) ( (ul)( PCI_CFG|Lng|((command)<<BYTE_ENABLE_SHIFT) ) )

/*
 *  Macros used to access the I/O controller csr's.
 */
#define _IN_IOC_CSR(p)    (ReadL(IOC_CSR(p)))
#define _OUT_IOC_CSR(p,d) WriteL((IOC_CSR(p)),(ui)d);mb();

void out_ioc_csr(ui p, ul d){ _OUT_IOC_CSR(p, d);}
ul in_ioc_csr(ui p){ return((ui)_IN_IOC_CSR(p));}


/* Establish base address of Real Time Clock */
int rtcBase = RTCBASE;

/* ----- port routines -------------*/
void outportb(ui p,ui d){d&=0xff; WriteL(IO_B(p),d<<(8*(p&3))); mb();}
void outportw(ui p,ui d){d&=0xffff; WriteL(IO_W(p),d<<(8*(p&2))); mb();}
void outportl(ui p,ui d){WriteL(IO_L(p),d); mb();}

void outcfgb(ui bus, ui dev, ui func, ui reg, ui data)
{
    data&=0xff; 
    out_ioc_csr(EPIC_HAXR2, (bus !=0));
    WriteL(CFG_B(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) : 
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)), data<<(8*(reg&3)));
    mb();
}
void outcfgw(ui bus, ui dev, ui func, ui reg, ui data)
{
    data&=0xffff; 
    out_ioc_csr(EPIC_HAXR2, (bus !=0));
    WriteL(CFG_W(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) : 
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)), data<<(8*(reg&2)));
    mb();
}
void outcfgl(ui bus, ui dev, ui func, ui reg, ui data)
{
    out_ioc_csr(EPIC_HAXR2, (bus !=0));
    WriteL(CFG_L(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) : 
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)), data);
    mb();
}

ui inportb(ui p){return ((ui)(ReadL(IO_B(p))>>((p&3)*8))&(ui)0xFF);}
ui inportw(ui p){return ((ui)(ReadL(IO_W(p))>>((p&2)*8))&(ui)0xFFFF);}
ui inportl(ui p){return ((ReadL(IO_L(p)))&0xFFFFFFFF);}

ui incfgb(ui bus, ui dev, ui func, ui reg)
{
    ui result;

    out_ioc_csr(EPIC_HAXR2, (bus !=0));
    result = ReadL(CFG_B(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) :
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)));
    return (result >> ((reg&3)*8)) & 0xff;
}

ui incfgw(ui bus, ui dev, ui func, ui reg)
{
    ui result;

    out_ioc_csr(EPIC_HAXR2, (bus !=0));
    result = ReadL(CFG_W(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) :
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)));
    return (result >> ((reg&2)*8)) & 0xffff;
}

ui incfgl(ui bus, ui dev, ui func, ui reg)
{
    ui result;

    out_ioc_csr(EPIC_HAXR2, (bus !=0));
    result = ReadL(CFG_L(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) :
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)));
    return result & 0xffffffff;
}

/* ----- memory routines -------------*/

void outmemb(ui p,ui d){d&=0xff;WriteL(MEM_B(p),(d<<24)|(d<<16)|(d<<8)|d);mb();}
void outmemw(ui p,ui d){d&=0xffff;WriteL(MEM_W(p),(d<<16)|d);mb();}
void outmeml(ui p,ui d){WriteL(MEM_L(p),d);mb();}

ui indmemb(ui p){return ((ui)(ReadL(D_MEM_L(p))>>((p&3)*8))&(ui)0xFF);}
ui indmemw(ui p){return ((ui)(ReadL(D_MEM_L(p))>>((p&2)*8))&(ui)0xFFFF);}
ui indmeml(ui p){return ((ui)(ReadL(D_MEM_L(p)))&0xFFFFFFFF);}

void outdmemb(ui p,ui d){d&=0xff; d<<=((p&3)*8); WriteL(D_MEM_L(p), (((ui)indmeml(p) & ~(0xFF<<((p&3)*8)))|d));mb();}
void outdmemw(ui p,ui d){d&=0xffff; d<<=((p&2)*8); WriteL(D_MEM_L(p), ((ui)(indmeml(p) & ~(0xFF<<((p&2)*8)))|d));mb();}
void outdmeml(ui p,ui d){WriteL(D_MEM_L(p),d);mb();}


ui inmemb(ui p){return ((ui)(ReadL(MEM_B(p))>>((p&3)*8))&(ui)0xFF);}
ui inmemw(ui p){return ((ui)(ReadL(MEM_W(p))>>((p&2)*8))&(ui)0xFFFF);}
ui inmeml(ui p){return ((ReadL(MEM_L(p)))&0xFFFFFFFF);}


/*
 *  This routine sets up the Host Address Extension register.
 */
ui SetHAE (ui Value)
{
    ui OldValue;

    OldValue = (ui) in_ioc_csr(EPIC_HAXR1);
    out_ioc_csr(EPIC_HAXR1, Value);
    return OldValue;
}


ui inIack(void){return(*((ui*)IACK)&0xffffffffL);}

/*
 *  Initialise the PCI bus, we don't go look for devices
 *  until we've set up the address mappings into and out of
 *  PCI and System address space.  The PCI sniffing is done
 *  in /lib/pci.c
 *
 */
void PCIBusInit(void)
{
/*
 * Establish DMA access.
 */
#if (PCI_BASE_1_USED) 
  {
    out_ioc_csr(EPIC_PCI_MASK_1, PCI_MASK_1);  
    out_ioc_csr(EPIC_PCI_TBASE_1, PCI_TBASE_1 >> 1); 
    out_ioc_csr(EPIC_PCI_BASE_1, PCI_BASE_1 | EPIC_PCI_BASE_M_WENB);
  }
#endif

#if (PCI_BASE_2_USED) 
  {
    out_ioc_csr(EPIC_PCI_MASK_2, PCI_MASK_2);  
    out_ioc_csr(EPIC_PCI_TBASE_2, PCI_TBASE_2 >> 1); 
    out_ioc_csr(EPIC_PCI_BASE_2, PCI_BASE_2 | EPIC_PCI_BASE_M_WENB);
  }
#endif

}


void ISABusInit(void)
{

/*  
 * Turn off GAT (Guaranteed Access Time) per Saturn Errata
 * Also disable BP (Bus Park) enabled.
 * This is the default setting but a version of
 * the SROM sets some bits here.
 */
  outcfgb(0, 19, 0,  0x41, 0x00);

  outcfgb(0, 19, 0, 0x48, 0xf0); /* Allow ISA master or DMA accesses to 16MB */
}

void outLed(ui d) { outportb(LEDPORT, d); }


/*
 * Clear the NODEV status and
 * return the current value (TRUE/FALSE).
 */
ul IOPCIClearNODEV(void)
{
  ul status;
/*
 * Get the current NODEV status.
 */
  status = in_ioc_csr(EPIC_CSR);

  PRTRACE2("EPIC_CSR:%04x\n", status);

  status = status & (EPIC_CSR_M_NDEV|EPIC_CSR_M_TABT);

/*
 * Clear it.
 */
  out_ioc_csr(EPIC_CSR, status);

  return status;
}
