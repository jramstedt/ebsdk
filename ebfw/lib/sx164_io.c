/*****************************************************************************

       Copyright 1993, 1994, 1995  Digital Equipment Corporation,
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
static char *rcsid = "$Id: sx164_io.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: sx164_io.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.6  1997/07/09  18:03:37  pbell
 * modified to use ReadB instead of a special utility.
 *
 * Revision 1.5  1997/06/05  18:25:41  pbell
 * *** empty log message ***
 *
 * Revision 1.4  1997/06/05  17:37:05  pbell
 * Updated PCIBusInit to turn on the Pyxis interupt mask register
 * fields for RTC,Halt, and NMI interupts.
 *
 * Revision 1.3  1997/05/16  02:05:54  fdh
 * Turned on the SetHAE function.
 *
 * Revision 1.2  1997/05/08  09:40:39  fdh
 * Modified the list of include files.
 *
 * Revision 1.1  1997/05/01  21:12:44  pbell
 * Initial revision
 *
 * Revision 1.16  1996/08/13  18:35:58  fdh
 * Disable GAT mode.
 * Check for Target Aborts as well as Master Aborts
 * when checking for "No Device" errors.
 *
 * Revision 1.15  1996/07/11  16:00:17  fdh
 * Moved IOBusInit() to a separate module.
 *
 * Revision 1.14  1996/06/13  03:25:47  fdh
 * Modified data type returned from inmeml.
 *
 * Revision 1.13  1996/05/25  20:36:34  fdh
 * Conditionalized the SMC initialization.
 *
 * Revision 1.12  1996/05/22  21:20:28  fdh
 * Removed some obsoleted i/o function aliases.
 * Moved includes to a single file to minimize makedepend warnings.
 *
 * Revision 1.11  1996/02/20  18:44:44  cruz
 * Added call to SMCInit().
 *
 * Revision 1.10  1995/10/31  18:41:14  cruz
 * Commented out some unused routines.
 *
 * Revision 1.9  1995/10/26  23:47:04  cruz
 * Fixed up prototypes.
 *
 * Revision 1.8  1995/10/13  17:43:55  fdh
 * Modified ISABusInit to be internal.
 *
 * Revision 1.7  1995/10/13  14:14:20  fdh
 * Updated some internal function prototypes.
 *
 * Revision 1.6  1995/08/25  19:47:20  fdh
 * Added func arg to PCI Config space access routines.
 *
 * Revision 1.5  1995/07/05  18:21:55  cruz
 * Added routines for accessing PCI dense space and
 * setting the HAXR (HAE).
 *
 * Revision 1.4  1995/04/21  17:42:06  cruz
 * Added casting operators to avoid warnings under NT.
 *
 * Revision 1.3  1994/12/12  21:42:56  cruz
 * Turned on GAT mode to fix problem with ISA DMA.
 *
 * Revision 1.2  1994/11/28  19:06:01  cruz
 * Modified IO_MASK and added MEM_SP1_MASK for masking the
 * correct bits for each of the PCI regions.
 *
 * Revision 1.1  1994/11/23  20:03:02  cruz
 * Initial revision
 *
 *
 */




/*------------------------------------------------------------------------**
**                                                                        **
**  EB164 I/O procedures                                                   **
**                                                                        **
**      inport[b|w|t|l], outport[b|w|t|l] 8:16:24:32 IO xfers             **
**                                                                        **
**      inmem[b|w|t|l], outmem[b|w|t|l] 8:16:24:32 ISA memory xfers       **
**                                                                        **
**------------------------------------------------------------------------*/

#include "system.h"
#include "lib.h"
#include "pci.h"
#include "prtrace.h"

#include "pyxis.h"

/* EB164 ADDRESS BIT DEFINITIONS
 *
 *  3 3 3 3|3 3 3 3|3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1 
 *  9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |1| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |0|0|0|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
 *  |                                                                        \_/ \_/
 *  |                                                                         |   |
 *  +-- IO space, not cached.                                   Byte Enable --+   |
 *                                                              Transfer Length --+
 *
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
#define IO_MASK 0x1ffffff  /* IO mask is 25 bits */
#define MEM_SP1_MASK 0x1fffffff  /* Mem sparse space 1 mask is 29 bits */
#define FLASH_OFFSET_MASK ((ul)0x3ffffff)
#define CBOX_CONFIG ((ul)0xFFFFFCFFFFF00008)	/* kseg address for cbox_cfg */


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
 *      31:11   Device select bit.
 *      10:8    Function number
 *       7:2    Register number
 *
 * Type 1:
 *
 *  3 3|3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1 
 *  3 2|1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | | | | | | | | | | |B|B|B|B|B|B|B|B|D|D|D|D|D|F|F|F|R|R|R|R|R|R|0|1|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *      31:24   reserved
 *      23:16   bus number (8 bits = 128 possible buses)
 *      15:11   Device number (5 bits)
 *      10:8    function number
 *       7:2    register number
 *  
 * Notes:
 *      The function number selects which function of a multi-function device 
 *      (eg scsi and ethernet.
 * 
 *      On the EB66, EB64+ and EB164, you select the device by picking a bit between 11 and 31
 *      (by left shifting).   This PCI code assumes that the low order device number is
 *      11, hence the lack of a shift by 11 bits.  Also note that bits 1:0 are
 *      specified via a register.
 * 
 *      The register selects a DWORD (32 bit) register offset.  Hence it doesn't
 *      get shifted by 2 bits as we want to "drop" the bottom two bits.
 *
 */

#define PCI_CONFIG_TYPE_0(device,function,register) \
	( (ul)( (register & 0xff) | ((function & 0x7) << 8) | ((device - 11) << 11) ) )

#define PCI_CONFIG_TYPE_1(bus,device,function,register) \
	( (ul)( (register & 0xff) | ((function & 0x7) << 8) | ((device & 0x1f) << 11) | \
		((bus & 0xff) << 16) ) )

#define PCI_IO    ((ul)0x858<<28)    /*    CPU Adr[39:28]=0x858 select PCI I/O. */
#define PCI_MEM   ((ul)0x800<<28)    /*    CPU Adr[39:28]=0x800 select PCI Mem. */
#define PCI_CFG   ((ul)0x870<<28)    /*    CPU Adr[39:28]=0x870 select PCI Cfg. */
#define IOC_CSR(x) (((ul)0x874<<28)|(ul)(x))
#define IACK      ((ul)0x872<<28)
#define PCI_D_MEM ((ul)0x860<<28)    /*    CPU Adr[39:28]=0x860 select PCI dense Mem.*/
#define IOC_PCI   ((ul)0x876<<28)    /* PCI related            */
#define FLASH     ((ul)0xFFFFFC87c<<28)        /* Flash base address */
#define PYXIS_ICR(x) (((ul)MC_ICR_BASE<<MC_ICR_BASE_SHIFT)|(ul)(x))/* INT Controller Registers	*/

#define Byt       ((ul)0<<TRANSFER_LENGTH_SHIFT)
#define Wrd       ((ul)1<<TRANSFER_LENGTH_SHIFT)
#define Tri       ((ul)2<<TRANSFER_LENGTH_SHIFT)
#define Lng       ((ul)3<<TRANSFER_LENGTH_SHIFT)

#define IO_B(x) ((ul)(PCI_IO|Byt|(((ul)(x)&IO_MASK)<<BYTE_ENABLE_SHIFT)))
#define IO_W(x) ((ul)(PCI_IO|Wrd|(((ul)(x)&IO_MASK)<<BYTE_ENABLE_SHIFT)))
#define IO_T(x) ((ul)(PCI_IO|Tri|(((ul)(x)&IO_MASK)<<BYTE_ENABLE_SHIFT)))
#define IO_L(x) ((ul)(PCI_IO|Lng|(((ul)(x)&IO_MASK)<<BYTE_ENABLE_SHIFT)))

#define MEM_B(x) ((ul)(PCI_MEM|Byt|(((ul)(x)&MEM_SP1_MASK)<<BYTE_ENABLE_SHIFT)))
#define MEM_W(x) ((ul)(PCI_MEM|Wrd|(((ul)(x)&MEM_SP1_MASK)<<BYTE_ENABLE_SHIFT)))
#define MEM_T(x) ((ul)(PCI_MEM|Tri|(((ul)(x)&MEM_SP1_MASK)<<BYTE_ENABLE_SHIFT)))
#define MEM_L(x) ((ul)(PCI_MEM|Lng|(((ul)(x)&MEM_SP1_MASK)<<BYTE_ENABLE_SHIFT)))

#define D_MEM_L(x) ((ul)(PCI_D_MEM|((ul)(((x) & ~3) & 0xFFFFFFFF))))


#define CFG_B(command) ( (ul)( PCI_CFG|Byt|((command)<<BYTE_ENABLE_SHIFT) ) )
#define CFG_W(command) ( (ul)( PCI_CFG|Wrd|((command)<<BYTE_ENABLE_SHIFT) ) )
#define CFG_L(command) ( (ul)( PCI_CFG|Lng|((command)<<BYTE_ENABLE_SHIFT) ) )

#define FLASH_B(x) ((ul)(FLASH | ((ul)(x)&FLASH_OFFSET_MASK)))

/*
 * PC164SX interrupt bits.
 *
 *  INTERRUPT REQUEST REGISTER (INT_REQ)  87.A000.0000
 *
 *
 *  |  GPI7   |  GPI6   |  GPI5   |  GPI4   |  GPI3   |  GPI2   |  GPI1   | GPI0   |
 *  +---------+---------+---------+---------+---------+---------+---------+--------+
 *  | ISA_INT | RTC_IRQ | REV 1   | REV0    | HLO ID  | Halt    |  NMI    |Reserved|
 *  |         |         |(low)    | (low)   | (MBZ)   |         |         |        |
 *  +---------+---------+---------+---------+---------+---------+---------+--------+
 *
 *
 *  |  GPI15  |  GPI14  |  GPI13  |  GPI12  |  GPI11  |  GPI10  |  GPI9   | GPI8   |
 *  +---------+---------+---------+---------+---------+---------+---------+--------+
 *  | PCI_INT | PCI_INT | PCI_INT | PCI_INT | PCI_INT | PCI_INT | PCI_INT |PCI_INT |
 *  |   B0    |   B1    |  B2     |   B3    |   A0    |   A1    |   A2    |  A3    |
 *  +---------+---------+---------+---------+---------+---------+---------+--------+
 *
 *
 *  |  GPI23  |  GPI22  |  GPI21  |  GPI20  |  GPI19  |  GPI18  |  GPI17  | GPI16  |
 *  +---------+---------+---------+---------+---------+---------+---------+--------+
 *  | PCI_INT | PCI_INT | PCI_INT | PCI_INT | PCI_INT | PCI_INT | PCI_INT |PCI_INT |
 *  |   D0    |   D1    |  D2     |   D3    |   C0    |   C1    |   C2    |  C3    |
 *  +---------+---------+---------+---------+---------+---------+---------+--------+
 *
 *
 *  |  GPI31  |  GPI30  |  GPI29  |  GPI28  |  GPI27  |  GPI26  |  GPI25  | GPI24  |
 *  +---------+---------+---------+---------+---------+---------+---------+--------+
 *  | CONFIG  | CONFIG  | CONFIG  | CONFIG  | CONFIG  | CONFIG  | CONFIG  | CONFIG |
 *  | BIT 0   | BIT 1   | BIT 2   | BIT 3   | BIT 4   | BIT 5   | BIT 6   | BIT 7  |
 *  +---------+---------+---------+---------+---------+---------+---------+--------+
 *
 *    The Configuration Bits are defined as follows
 *
 *    Config (1:0)  BC_Size		
 *				     11 - 2M  (Default)
 *				     10 - 4M
 *				     01 - 1M
 *				     00 - 512M
 *
 *    Config (3:2)  BC_Speed		
 *				     11 - 100MHz (Default)
 *				     10 - 117MHz
 *				     01 - 133MHz
 *				     00 - Reserved
 *
 *    Config (6:4)  CPU_Speed 	
 *				    111 - 400MHz (Default)
 *				    110 - 466MHZ 
 *				    101 - 533MHz
 *				    100 - 600MHZ
 *				    011 - 666MHz
 *				    010 - Reserved
 *				    001 - Reserved
 *				    000 - Reserved
 *
 *    Config (7)   Fail Safe Booter   1  - NT/OSF
 *				      0  - Fail Safe
 */

/*
 *  Macros used to access the I/O controller csr's.
 */
#define _IN_IOC_CSR(p)    (ReadL(IOC_CSR(p)))
#define _OUT_IOC_CSR(p,d) WriteL((IOC_CSR(p)),(ui)d);mb();

void out_ioc_csr(ui p, ul d){ _OUT_IOC_CSR(p, d);}
ul in_ioc_csr(ui p){ return((ui)_IN_IOC_CSR(p));}
void out_ioc_pci(ui p, ul d){WriteL(IOC_PCI|(ul)p,(ui)d&0xffffffff);mb();}

#ifdef NOT_IN_USE
ul in_ioc_pci(ui p){return ((ReadL(IOC_PCI|(ul)p))&0xFFFFFFFF);}
#endif

/* Establish base address of Real Time Clock */
int rtcBase = RTCBASE;

/* ----- port routines -------------*/
void outportb(ui p,ui d){d&=0xff; WriteL(IO_B(p),d<<(8*(p&3))); mb();}
void outportw(ui p,ui d){d&=0xffff; WriteL(IO_W(p),d<<(8*(p&2))); mb();}
void outportl(ui p,ui d){WriteL(IO_L(p),d); mb();}

#define PYXIS_CFG 0x480
void outcfgb(ui bus, ui dev, ui func, ui reg, ui data)
{
    data&=0xff; 
    out_ioc_csr(PYXIS_CFG, (bus !=0));
    WriteL(CFG_B(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) : 
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)), data<<(8*(reg&3)));
    mb();
}
void outcfgw(ui bus, ui dev, ui func, ui reg, ui data)
{
    data&=0xffff; 
    out_ioc_csr(PYXIS_CFG, (bus !=0));
    WriteL(CFG_W(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) : 
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)), data<<(8*(reg&2)));
    mb();
}
void outcfgl(ui bus, ui dev, ui func, ui reg, ui data)
{
    out_ioc_csr(PYXIS_CFG, (bus !=0));
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

    out_ioc_csr(PYXIS_CFG, (bus !=0));
    result = ReadL(CFG_B(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) :
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)));
    return (result >> ((reg&3)*8)) & 0xff;
}

ui incfgw(ui bus, ui dev, ui func, ui reg)
{
    ui result;

    out_ioc_csr(PYXIS_CFG, (bus !=0));
    result = ReadL(CFG_W(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) :
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)));
    return (result >> ((reg&2)*8)) & 0xffff;
}

ui incfgl(ui bus, ui dev, ui func, ui reg)
{
    ui result;

    out_ioc_csr(PYXIS_CFG, (bus !=0));
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
void outdmemw(ui p,ui d){d&=0xff; d<<=((p&2)*8); WriteL(D_MEM_L(p), (((ui)indmemw(p) & ~(0xFFFF<<((p&2)*8)))|d));mb();}
void outdmeml(ui p,ui d){WriteL(D_MEM_L(p),d);mb();}

ui inmemb(ui p){return ((ui)(ReadL(MEM_B(p))>>((p&3)*8))&(ui)0xFF);}
ui inmemw(ui p){return ((ui)(ReadL(MEM_W(p))>>((p&2)*8))&(ui)0xFFFF);}
ui inmeml(ui p){return ((ReadL(MEM_L(p)))&0xFFFFFFFF);}

/*
 *  This routine sets up the Host Address Extension register.
 */
#define HAE_MEM 0x400
ui SetHAE (ui Value)
{
    ui OldValue;

    OldValue = (ui) in_ioc_csr(HAE_MEM);
    out_ioc_csr(HAE_MEM, Value & (ui) ~MEM_SP1_MASK);

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
  ul mask;
  mask = in_ioc_csr(pyxis_err_mask);    /* Read current mask */
  mask |= (1<<pyxis_err_mask_v_mas_abt);/* Enable detection/reporting */
  mask |= (1<<pyxis_err_mask_v_tar_abt);/* Enable detection/reporting */
  out_ioc_csr(pyxis_err_mask, mask);    /* of master abort */

  mask = ReadQ( PYXIS_ICR(MC_ICR_INT_MASK) );/* Read current mask */
  mask |= 0x46;				     /* Enable Clock/Halt/NMI interupts */
  WriteQ( PYXIS_ICR(MC_ICR_INT_MASK), mask );
  mb();

/*
 * Establish DMA access.
 */
#if (PCI_BASE_1_USED)
    out_ioc_pci(w_mask0, PCI_MASK_1);  
    out_ioc_pci(t_base0, PCI_TBASE_1 >> 2); 
    out_ioc_pci(w_base0, PCI_BASE_1 | w_m_en);
#endif

#if (PCI_BASE_2_USED)
    out_ioc_pci(w_mask1, PCI_MASK_2);  
    out_ioc_pci(t_base1, PCI_TBASE_2 >> 2); 
    out_ioc_pci(w_base1, PCI_BASE_2 | w_m_en);
#endif

#if (PCI_BASE_3_USED)
    out_ioc_pci(w_mask2, PCI_MASK_3);  
    out_ioc_pci(t_base2, PCI_TBASE_3 >> 2); 
    out_ioc_pci(w_base2, PCI_BASE_3 | w_m_en);
#endif

#if (PCI_BASE_4_USED)
    out_ioc_pci(w_mask3, PCI_MASK_4);  
    out_ioc_pci(t_base3, PCI_TBASE_4 >> 2); 
    out_ioc_pci(w_base3, PCI_BASE_4 | w_m_en);
#endif
}


void ISABusInit(void)
{

  SMC669Init();

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
  status = in_ioc_csr(pyxis_err);

  PRTRACE2("pyxis_err:%04x\n", status);

  status = status &
    ((1<<pyxis_err_v_rcvd_mas_abt)|
     (1<<pyxis_err_v_rcvd_tar_abt));

/*
 * Clear it.
 */
  out_ioc_csr(pyxis_err, status);

  return status;
}

void WRITE_FLASH_UCHAR (
		       ul FlashOffset, 
		       ub Value
		       )
{
	//ul	data;
	//data = cServe(0, 0, CSERVE_K_RD_MAF_MODE);
	//cServe((data | 2), 0, CSERVE_K_WR_MAF_MODE);
	WriteFlash( FLASH_B(FlashOffset) , Value, CBOX_CONFIG);
	//cServe(data, 0, CSERVE_K_WR_MAF_MODE);
}


/* Note that the following function will work only if asmstuff is compiled
   with CPU_SUPPORTS_BYTE_WORD defined. */

ub READ_FLASH_UCHAR (
		       ul FlashOffset 
		       )
{
    return ReadB( FLASH_B(FlashOffset) );
}

ub inrom(ui p){
	return (READ_FLASH_UCHAR((ul )p));
}


