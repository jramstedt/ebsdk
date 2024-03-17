
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
static char *rcsid = "$Id: eb164_io.c,v 1.1.1.1 1998/12/29 21:36:10 paradis Exp $";
#endif

/*
 * $Log: eb164_io.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:10  paradis
 * Initial CVS checkin
 *
 * Revision 1.17  1997/02/21  03:43:13  fdh
 * Enable target abort reporting in CIA.
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

#include "cia.h"

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
 *	On the EB66, EB64+ and EB164, you select the device by picking a bit between 11 and 31
 *	(by left shifting).   This PCI code assumes that the low order device number is
 *	11, hence the lack of a shift by 11 bits.  Also note that bits 1:0 are
 *  	specified via a register.
 * 
 *	The register selects a DWORD (32 bit) register offset.  Hence it doesn't
 *	get shifted by 2 bits as we want to "drop" the bottom two bits.
 *
 */

#ifdef CIA_PASS1
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

#define PCI_IO	  ((ul)0x858<<28)    /*    CPU Adr[39:28]=0x858 select PCI I/O.	*/
#define PCI_MEM   ((ul)0x800<<28)    /*    CPU Adr[39:28]=0x800 select PCI Mem.	*/
#define PCI_CFG   ((ul)0x870<<28)    /*    CPU Adr[39:28]=0x870 select PCI Cfg.	*/
#define IOC_CSR(x) (((ul)0x874<<28)|(ul)(x))
#define IACK      ((ul)0x872<<28)
#define PCI_D_MEM ((ul)0x860<<28)    /*    CPU Adr[39:28]=0x860 select PCI dense Mem.*/
#define IOC_PCI   ((ul)0x876<<28)    /* PCI related            */

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

#define CIA_CFG 0x480
void outcfgb(ui bus, ui dev, ui func, ui reg, ui data)
{
    data&=0xff; 
    out_ioc_csr(CIA_CFG, (bus !=0));
    WriteL(CFG_B(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) : 
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)), data<<(8*(reg&3)));
    mb();
}
void outcfgw(ui bus, ui dev, ui func, ui reg, ui data)
{
    data&=0xffff; 
    out_ioc_csr(CIA_CFG, (bus !=0));
    WriteL(CFG_W(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) : 
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)), data<<(8*(reg&2)));
    mb();
}
void outcfgl(ui bus, ui dev, ui func, ui reg, ui data)
{
    out_ioc_csr(CIA_CFG, (bus !=0));
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

    out_ioc_csr(CIA_CFG, (bus !=0));
    result = ReadL(CFG_B(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) :
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)));
    return (result >> ((reg&3)*8)) & 0xff;
}

ui incfgw(ui bus, ui dev, ui func, ui reg)
{
    ui result;

    out_ioc_csr(CIA_CFG, (bus !=0));
    result = ReadL(CFG_W(bus == 0 ? PCI_CONFIG_TYPE_0(dev,func,reg) :
	PCI_CONFIG_TYPE_1(bus,dev,func,reg)));
    return (result >> ((reg&2)*8)) & 0xffff;
}

ui incfgl(ui bus, ui dev, ui func, ui reg)
{
    ui result;

    out_ioc_csr(CIA_CFG, (bus !=0));
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
  mask = in_ioc_csr(cia_err_mask);	/* Read current mask */
  mask |= ((1<<cia_err_mask_v_rcvd_mas_abt)
	   |(1<<cia_err_mask_v_rcvd_tar_abt));
  out_ioc_csr(cia_err_mask, mask);	/* of master abort */

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

#ifdef NEED_SMC_ULTRA_IO
  SMCInit();
#endif

/*  
 * Turn off GAT (Guaranteed Access Time).
 */
  outcfgb(0, 19, 0,  0x41, incfgb(0, 19, 0, 0x41) & 0xfe);

/*
 * Allow ISA master or DMA accesses to 16MB
 */
  outcfgb(0, 19, 0, 0x48, 0xf0);
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
  status = in_ioc_csr(cia_err);

  PRTRACE2("cia_err:%04x\n", status);

  status = status &
    ((1<<cia_err_v_rcvd_mas_abt)|
     (1<<cia_err_v_rcvd_tar_abt));

/*
 * Clear it.
 */
  out_ioc_csr(cia_err, status);

  return status;
}
