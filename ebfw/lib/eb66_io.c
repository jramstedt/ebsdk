
/*****************************************************************************

       Copyrigh 1993, 1994, 1995 Digital Equipment Corporation,
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
static char *rcsid = "$Id: eb66_io.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 * $Log: eb66_io.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.39  1996/08/13  18:43:14  fdh
 * Removed a superfluous DMA controller setting.
 *
 * Revision 1.38  1996/07/11  16:01:05  fdh
 * Moved IOBusInit() to a separate module.
 *
 * Revision 1.37  1996/06/13  03:25:08  fdh
 * Modified data type returned from inmeml.
 *
 * Revision 1.36  1996/05/22  21:15:48  fdh
 * Moved includes to a single file to minimize makedepend warnings.
 *
 * Revision 1.35  1996/05/16  18:31:40  fdh
 * Removed inVti and outVti routines.
 *
 * Revision 1.34  1996/05/16  15:22:11  fdh
 * Removed inportX and outportX aliases.
 *
 * Revision 1.33  1996/02/07  18:51:55  cruz
 * Removed set_haxr().
 * Fixed SetHAE() to keep the last value stored since this
 * register is write-only on 21066 and its variants.
 *
 * Revision 1.32  1995/10/31  22:48:51  cruz
 * Commented out code not in use.
 *
 * Revision 1.31  1995/10/13  14:14:20  fdh
 * Updated some internal function prototypes.
 *
 * Revision 1.30  1995/08/25  19:47:20  fdh
 * Added func arg to PCI Config space access routines.
 *
 * Revision 1.29  1995/07/05  20:09:14  cruz
 * Added support for accessing PCI dense memory.
 *
 * Revision 1.28  1994/11/16  22:00:32  fdh
 * Corrected register mask in PCI configuration space
 * definitions.
 *
 * Revision 1.27  1994/11/16  16:27:02  fdh
 * Corrected arguments in calls to outcfgb().
 *
 * Revision 1.26  1994/11/08  21:34:27  fdh
 * Added routine to set the Host Address Extension Register.
 *
 * Revision 1.25  1994/11/01  11:31:56  rusling
 * Changed to support PCI-PCI bridges with Type 0
 * and Type 1 configuration address cycles.
 *
 * Revision 1.24  1994/10/18  11:04:02  berent
 * Fixed clearing errors following pci config commands to
 * empty slots (2nd attempt)
 *
 * Revision 1.23  1994/10/17  18:19:22  rusling
 * Modified to clear any pending error flag
 * when clearing nodevice errors.
 *
 * Revision 1.22  1994/08/08  21:54:16  fdh
 * Fixup RCS Id string...
 *
 * Revision 1.21  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.20  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.19  1994/04/04  15:21:05  fdh
 * Implemented outLed().
 *
 * Revision 1.18  1994/03/24  21:40:35  fdh
 * Removed unnecessary compile-time conditionals.
 *
 * Revision 1.17  1994/03/24  21:11:42  fdh
 * Moved csrv() to ffexec.c and fixed masks.
 *
 * Revision 1.16  1994/03/05  19:29:38  fdh
 * Added LCA pass 2.0 support. Use -DLCA_PASS1 for LCA pass 1.0 support.
 *
 * Revision 1.15  1994/01/19  10:40:08  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.14  1993/12/13  15:35:19  rusling
 * Fixed up IOC register access.
 *
 * Revision 1.12  1993/12/13  13:12:37  rusling
 * Set up PCI to system translation buffers based on #defines.
 *
 * Revision 1.11  1993/11/24  15:07:03  rusling
 * Added IOPCIClearNODEV() routine for machine specific
 *  PCI NODEV clearing/reading.
 *
 * Revision 1.10  1993/11/22  16:30:52  rusling
 * Moved to a bus init routine called by main() (in cmd.c)
 * instead of from uart_init().
 *
 * Revision 1.9  1993/11/22  13:20:18  rusling
 * Added in_ioc_csr() routine.
 *
 * Revision 1.8  1993/11/19  17:02:36  fdh
 * Moved some definitions to global header file.
 *
 * Revision 1.7  1993/09/30  22:13:14  fdh
 * Establish PCI Target Windows properly.
 * Removed obsolete code.
 *
 * Revision 1.6  1993/08/25  04:18:32  fdh
 * Corrected I/O controller CSR base address.
 *
 * Revision 1.5  1993/08/21  02:05:27  fdh
 * Correct base address for access to IOC csr's
 * Fixed out_ioc_csr()
 * Cleaned up masks for i/o functions.
 * Turned off GAT and BP bits.
 * Establish DMA in range of 1MB and 16MB.
 *
 * Revision 1.4  1993/08/09  20:53:05  fdh
 * Establish base address of Real Time Clock as global variable.
 *
 * Revision 1.3  1993/07/28  17:04:14  fdh
 * Put channel 5 DMA controller into Cascade Mode.
 * Enable DMA busmastership for 1to16M
 * Establish I/O Top of Memory at 16MB.
 *
 * Revision 1.2  1993/07/27  21:36:51  fdh
 * Fixes to configuration register access functions.
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
**  EB66 I/O procedures                                                   **
**                                                                        **
**      inport[b|w|t|l], outport[b|w|t|l] 8:16:24:32 IO xfers             **
**                                                                        **
**      inmem[b|w|t|l], outmem[b|w|t|l] 8:16:24:32 ISA memory xfers       **
**                                                                        **
**------------------------------------------------------------------------*/

/* Add includes to this file to minimize makedepend warnings */
#include "eb66_io.h"

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
 * | | | | | | | | | | | | | | | | | | | | | | | |F|F|F|R|R|R|R|R|R|0|0|
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
#define PCI_CONFIG_TYPE_0(device,function,register) \
	( (ul)( (register & 0xff) | ((function & 0x7) << 8) | (1 << device) ) )

#define PCI_CONFIG_TYPE_1(bus,device,function,register) \
	( (ul)( (register & 0xff) | ((function & 0x7) << 8) | ((device & 0x1f) << 11) | \
		((bus & 0xff) << 16) ) )


#ifdef LCA_PASS1
#define PCI_IO	  ((ul)3<<32)    /*    CPU Adr[33:32]=3 select PCI I/O.	*/
#define PCI_MEM   ((ul)2<<32)    /*    CPU Adr[33:32]=2 select PCI Mem.	*/
#define PCI_CFG   ((ul)0x1E<<28) /*    CPU Adr[33:28]=1E select PCI Cfg.	*/
#define IOC_CSR   ((ul)0x18<<28)
#define IACK      ((ul)0x1C<<28)
#else
#define PCI_IO	  ((ul)0x1c<<28) /*    CPU Adr[33:28]=0x1c select PCI I/O.	*/
#define PCI_MEM   ((ul)2<<32)    /*    CPU Adr[33:32]=2 select PCI Mem.	*/
#define PCI_D_MEM ((ul)0x3<<32)    /*  select PCI dense Mem.*/
#define PCI_CFG   ((ul)0x1E<<28) /*    CPU Adr[33:28]=1E select PCI Cfg.	*/
#define IOC_CSR   ((ul)0x18<<28)
#define IACK      ((ul)0x1A<<28)
#endif

#define Byt       ((ui)0<<TRANSFER_LENGTH_SHIFT)
#define Wrd       ((ui)1<<TRANSFER_LENGTH_SHIFT)
#define Tri       ((ui)2<<TRANSFER_LENGTH_SHIFT)
#define Lng       ((ui)3<<TRANSFER_LENGTH_SHIFT)

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

void out_ioc_csr(ui p, ul d){WriteQ(IOC_CSR|(ul)p,d);mb();}
ul in_ioc_csr(ui p){return ((ReadQ(IOC_CSR|(ul)p))&0xFFFFFFFF);}
 
/* Establish base address of Real Time Clock */
int rtcBase = RTCBASE;

/* ----- port routines -------------*/

void outportb(ui p,ui d){d&=0xff; WriteL(IO_B(p),d<<(8*(p&3))); mb();}
void outportw(ui p,ui d){d&=0xffff; WriteL(IO_W(p),d<<(8*(p&2))); mb();}
void outportl(ui p,ui d){WriteL(IO_L(p),d); mb();}

#define IOC_CCT   0x20
#define IOC_HAXR1 0x00

void outcfgb(ui bus, ui dev, ui func, ui reg, ui data)
{
    data&=0xff; 
    out_ioc_csr(IOC_CCT, (bus !=0));
    WriteL(CFG_B(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) : 
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)), data<<(8*(reg&3)));
    mb();
}
void outcfgw(ui bus, ui dev, ui func, ui reg, ui data)
{
    data&=0xffff; 
    out_ioc_csr(IOC_CCT, (bus !=0));
    WriteL(CFG_W(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) : 
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)), data<<(8*(reg&2)));
    mb();
}
void outcfgl(ui bus, ui dev, ui func, ui reg, ui data)
{
    out_ioc_csr(IOC_CCT, (bus !=0));
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

    out_ioc_csr(IOC_CCT, (bus !=0));
    result = ReadL(CFG_B(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) :
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)));
    return (result >> ((reg&3)*8)) & 0xff;
}

ui incfgw(ui bus, ui dev, ui func, ui reg)
{
    ui result;

    out_ioc_csr(IOC_CCT, (bus !=0));
    result = ReadL(CFG_W(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) :
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)));
    return (result >> ((reg&2)*8)) & 0xffff;
}

ui incfgl(ui bus, ui dev, ui func, ui reg)
{
    ui result;

    out_ioc_csr(IOC_CCT, (bus !=0));
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
static ui OldValue, temp;

    Value &= 0xf8000000;   
    out_ioc_csr(IOC_HAXR1, Value);
    temp = OldValue;
    OldValue = Value;
    return temp;
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
    out_ioc_csr(IOC_W_MASK_0, PCI_MASK_1);  
    out_ioc_csr(IOC_W_T_BASE_0, PCI_TBASE_1); 
    out_ioc_csr(IOC_W_BASE_0, PCI_BASE_1 | IOC_W_BASE_M_WEN);
#else
    out_ioc_csr(IOC_W_BASE_0, (ul)(~IOC_W_BASE_M_WEN));
#endif

#if (PCI_BASE_2_USED)
    out_ioc_csr(IOC_W_MASK_1, PCI_MASK_2);  
    out_ioc_csr(IOC_W_T_BASE_1, PCI_TBASE_2); 
    out_ioc_csr(IOC_W_BASE_1, PCI_BASE_2 | IOC_W_BASE_M_WEN);
#else
    out_ioc_csr(IOC_W_BASE_1, (ul)(~IOC_W_BASE_M_WEN));
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
  outcfgb(0, 19, 0, 0x41, 0x00);

  outcfgb(0, 19, 0, 0x48, 0xf0); /* Allow ISA master or DMA accesses to 16MB */
}

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
  status = in_ioc_csr(IOC_REG_STATUS_0);

  PRTRACE2("IOC_REG_STATUS:%04x\n", status);

  status = status & IOC_STATUS_0_M_ERR;

/*
 * Clear it.
 */
  out_ioc_csr(IOC_REG_STATUS_0, status);

  return status;
}

void outLed(ui d) { outportb(LEDPORT, d); }
