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
static char *rcsid = "$Id: dp264_io.c,v 1.2 1999/01/21 19:05:05 gries Exp $";
#endif

/*
 * $Log: dp264_io.c,v $
 * Revision 1.2  1999/01/21 19:05:05  gries
 * First Release to cvs gries
 *
 * Revision 1.6  1999/01/19  16:51:35  gries
 * isa_present changes for Web Brick
 *
 * Revision 1.5  1998/12/17  19:28:56  gries
 * changes to detect isa (goldruch vs webbrick)
 *
 * Revision 1.4  1998/10/08  13:56:27  gries
 * added outcfgb(0, 5, 0, CY_BIOS_CONTROL_REG, 0xe0);
 * so that I can write to goldrush lcd
 *
 * Revision 1.3  1998/06/26  18:43:12  gries
 * changes to detect pc264 vs dp264 (1 pchip)
 *
 * Revision 1.2  1997/12/15  20:44:01  pbell
 * Added finished file.
 *
 */

/*------------------------------------------------------------------------**
**                                                                        **
**  DP264 I/O procedures                                                  **
**                                                                        **
**      inport[b|w|l], outport[b|w|l] 8:16:32 IO xfers                    **
**                                                                        **
**      inmem[b|w|l], outmem[b|w|l] 8:16:32 PCI memory xfers              **
**                                                                        **
**------------------------------------------------------------------------*/

#include "system.h"
#include "lib.h"
#include "pci.h"
#include "prtrace.h"
#include "tsunami.h"

BOOLEAN BootSROMPort;
int isa_present= 0;
#define QW(d)	((ul)(d))

#define SWZ(d) ( \
		(((QW(d)>>(0*8))&0xff)<<(0*8)) | \
		(((QW(d)>>(1*8))&0xff)<<(2*8)) | \
		(((QW(d)>>(2*8))&0xff)<<(4*8)) | \
		(((QW(d)>>(3*8))&0xff)<<(6*8)) | \
		(((QW(d)>>(4*8))&0xff)<<(1*8)) | \
		(((QW(d)>>(5*8))&0xff)<<(3*8)) | \
		(((QW(d)>>(6*8))&0xff)<<(5*8)) | \
		(((QW(d)>>(7*8))&0xff)<<(7*8)))

#define UNSWZ(d) ( \
		(((QW(d)>>(0*8))&0xff)<<(0*8)) | \
		(((QW(d)>>(2*8))&0xff)<<(1*8)) | \
		(((QW(d)>>(4*8))&0xff)<<(2*8)) | \
		(((QW(d)>>(6*8))&0xff)<<(3*8)) | \
		(((QW(d)>>(1*8))&0xff)<<(4*8)) | \
		(((QW(d)>>(3*8))&0xff)<<(5*8)) | \
		(((QW(d)>>(5*8))&0xff)<<(6*8)) | \
		(((QW(d)>>(7*8))&0xff)<<(7*8)))

int IOCounter;

static void IOCount(void) {
  if (++IOCounter > 10000) {
    IOCounter = 0;
    outmeml(0x7ffffffc, 0);	/* 0 --> 1GB - 4 */
  }
}

static void WriteIOB(ul address,ub value) {WriteB((address), (value)); IOCount();}
static void WriteIOW(ul address,uw value) {WriteW((address), (value)); IOCount();}
static void WriteIOL(ul address,ui value) {WriteL((address), (value)); IOCount();}
static void WriteIOQ(ul address,ul value) {WriteQ((address), (value)); IOCount();}

static ub ReadIOB(ul address) {IOCount(); return(ReadB(address));}
static uw ReadIOW(ul address) {IOCount(); return(ReadW(address));}
static ui ReadIOL(ul address) {IOCount(); return(ReadL(address));}
static ul ReadIOQ(ul address) {IOCount(); return(ReadQ(address));}


/* DP264 ADDRESS BIT DEFINITIONS
 *
 *  4 4 4 4 3 3 3 3|3 3 3 3|3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1 
 *  3 2 1 0 9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |1|0|0|0|0|0|0|0|0|0|x| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |0|0|0|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                   | 0 x x x x x x x x = Linear Memory Space		  4GB
 *  |       Target PCI -+ 1 0 0 x x x x x x = Reserved (TIG Bus)		  1GB
 *  |                     1 1 x x x x x x x = Pchip CSR Space			256MB
 *  +-- IO space,         1 1 0 1 0 x x x x = Reserved (Cchip CSRs)		256MB
 *      not cached.       1 1 0 1 1 x x x x = Reserved (Dchip CSRs)		256MB
 *                        1 1 1 1 1 1 x x x = Iack/Spacial Cycle		 64MB
 *                        1 1 1 1 1 1 1 0 x = Linear I/O Space			 32MB
 *                        1 1 1 1 1 1 1 1 0 = Linear Configuration Space	 16MB
 */

#define PCI_MEM   ((ul) PCI0_MEMORY<<24)
#define IACK      ((ul) PCI0_IACK<<24)
#define PCI_IO    ((ul) PCI0_IO<<24)
#define PCI_CFG   ((ul) PCI0_CONFIG<<24)
#define TIG       ((ul) TIG_BUS<<24)


/*
 * The following macro define Type 0 and Type 1 configuration address format
 *
 *  4 4 4 4|3 3 3 3|3 3 3 3|3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1 
 *  3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |1|0|0|0|0|0|0|0|0|0|x|      0x1FE      |       bus     |  device |func.|  register |   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *                                                                                       |
 *                                                                               Byte ---+
 */

#define PCI_CONFIG(bus,device,function,register) \
	((ul)(PCI_CFG | (register & 0xff) | ((function & 0x7) << 8) \
	      | ((device & 0x1f) << 11) | ((bus & 0xff) << 16)))

#define IO_ACCESS(p) ((ul)(PCI_IO | p))
#define MEM_ACCESS(p) ((ul)(PCI_MEM | p))
#define TIG_ACCESS(p) ((ul)(TIG | (p<<6)))

/* Establish base address of Real Time Clock */
int rtcBase = RTCBASE;

void outcfgb(ui bus, ui dev, ui func, ui reg, ui data)
{
  data&=0xff; 
  WriteIOB(PCI_CONFIG(bus,dev,func,reg), data);
  mb();
}
void outcfgw(ui bus, ui dev, ui func, ui reg, ui data)
{
  data&=0xffff; 
  WriteIOW(PCI_CONFIG(bus,dev,func,reg), data);
  mb();
}
void outcfgl(ui bus, ui dev, ui func, ui reg, ui data)
{
  WriteIOL(PCI_CONFIG(bus,dev,func,reg), data);
  mb();
}

ui incfgb(ui bus, ui dev, ui func, ui reg)
{
  ul result;
  result = ReadIOB(PCI_CONFIG(bus,dev,func,reg));
  return(result & 0xff);
}

ui incfgw(ui bus, ui dev, ui func, ui reg)
{
  ul result;
  result = ReadIOW(PCI_CONFIG(bus,dev,func,reg));
  return((result) & 0xffff);
}

ui incfgl(ui bus, ui dev, ui func, ui reg)
{
  ul result;
  result = ReadIOL(PCI_CONFIG(bus,dev,func,reg));
  return(result & 0xffffffff);
}

/* ----- port routines -------------*/
void outportb(ui port,ui data)
{
  data&=0xff;
  WriteIOB(IO_ACCESS(port),data);
  mb();
}

void outportw(ui port,ui data)
{
  data&=0xffff;
  WriteIOW(IO_ACCESS(port),data);
  mb();
}

void outportl(ui port,ui data)
{
  WriteIOL(IO_ACCESS(port),data);
  mb();
}

ui inportb(ui port)
{
  ul result;
  result = ReadIOB(IO_ACCESS(port));
  return(result & 0xff);
}

ui inportw(ui port)
{
  ul result;
  result = ReadIOW(IO_ACCESS(port));
  return(result & 0xffff);
}

ui inportl(ui port)
{
  ul result;
  result = ReadIOL(IO_ACCESS(port));
  return(result & 0xffffffff);
}

/* ----- memory routines -------------*/

void outmemb(ui port,ui data)
{
  data&=0xff;
  WriteIOB(MEM_ACCESS(port),data);
  mb();
}

void outmemw(ui port,ui data)
{
  data&=0xffff;
  WriteIOW(MEM_ACCESS(port),data);
  mb();
}

void outmeml(ui port,ui data)
{
  WriteIOL(MEM_ACCESS(port),data);
  mb();
}

ui inmemb(ui port)
{
  ub result;
  result = ReadIOB(MEM_ACCESS(port));
  return(result & 0xff);
}

ui inmemw(ui port)
{
  uw result;
  result = ReadIOW(MEM_ACCESS(port));
  return(result & 0xffff);
}

ui inmeml(ui port)
{
  ui result;
  result = ReadIOL(MEM_ACCESS(port));
  return(result & 0xffffffff);
}

/*------------------------------------------------*/

void outdmemb(ui port,ui data)
{
  data&=0xff;
  WriteIOB(MEM_ACCESS(port),data);
  mb();
}

void outdmemw(ui port,ui data)
{
  data&=0xffff;
  WriteIOW(MEM_ACCESS(port),data);
  mb();
}

void outdmeml(ui port,ui data)
{
  WriteIOL(MEM_ACCESS(port),data);
  mb();
}

ui indmemb(ui port)
{
  ub result;
  result = ReadIOB(MEM_ACCESS(port));
  return(result & 0xff);
}

ui indmemw(ui port)
{
  uw result;
  result = ReadIOW(MEM_ACCESS(port));
  return(result & 0xffff);
}

ui indmeml(ui port)
{
  ui result;
  result = ReadIOL(MEM_ACCESS(port));
  return(result & 0xffffffff);
}

/*------------------------------------------------*/

ui SetHAE (ui Value)
{
  return Value;
}

ui inIack(void){return(*((ui*)IACK)&0xffffffffL);}

/*
 *  Initialize the PCI bus, we don't go look for devices
 *  until we've set up the address mappings into and out of
 *  PCI and System address space.  The PCI sniffing is done
 *  in /lib/pci.c
 *
 */
void PCIBusInit(void)
{
  ul mask;
  int Pchip1_present;
  IOCounter = 0;

#if 0
  mask = in_ioc_csr(pyxis_err_mask);    /* Read current mask */
  mask |= (1<<pyxis_err_mask_v_mas_abt);/* Enable detection/reporting */
  mask |= (1<<pyxis_err_mask_v_tar_abt);/* Enable detection/reporting */
  out_ioc_csr(pyxis_err_mask, mask);    /* of master abort */

  mask = ReadQ( PYXIS_ICR(MC_ICR_INT_MASK) );/* Read current mask */
  mask |= 0x46;				     /* Enable Clock/Halt/NMI interupts */
  WriteQ( PYXIS_ICR(MC_ICR_INT_MASK), mask );
  mb();
#endif
  Pchip1_present= ((ReadIOQ(((ul) CCHIP_CSR<<24) | CSR_CSC) >> 14) & 1);

/*
 * Establish DMA access.
 */
#if (PCI_BASE_1_USED)
  WriteIOQ((((ul) PCHIP0_CSR<<24) | PCHIP0_WSM0), PCI_MASK_1);  
  WriteIOQ((((ul) PCHIP0_CSR<<24) | PCHIP0_TBA0), PCI_TBASE_1);
  WriteIOQ((((ul) PCHIP0_CSR<<24) | PCHIP0_WSBA0), PCI_BASE_1 | 1);
#endif

#if (PCI_BASE_2_USED)
  if (Pchip1_present)
  {
    WriteIOQ((((ul) PCHIP1_CSR<<24) | PCHIP1_WSM0), PCI_MASK_2);  
    WriteIOQ((((ul) PCHIP1_CSR<<24) | PCHIP1_TBA0), PCI_TBASE_2);
    WriteIOQ((((ul) PCHIP1_CSR<<24) | PCHIP1_WSBA0), PCI_BASE_2 | 1);
  }
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

//  WriteIOQ((((ul) CCHIP_CSR<<24) | CSR_PRBEN), 1);
  BootSROMPort = 0;
  if (Pchip1_present)
  BootSROMPort = !((ReadIOQ(((ul) TIG_BUS<<24) | (5<<27)) >> 7) & 1);
}

#define CY_COMMAND_REG			0x04
#define CY_INTA_ROUTE			0x40
#define CY_INTB_ROUTE			0x41
#define CY_INTC_ROUTE			0x42
#define CY_INTD_ROUTE			0x43
#define CY_CONTROL_REG			0x44
#define CY_ERR_CONTROL_REG		0x45
#define CY_BIOS_CONTROL_REG		0x47
#define CY_TOP_OF_MEM			0x48
#define CY_ATCTRL_REG1			0x49
#define CY_ATCTRL_REG2			0x4A
#define CY_IDE0_ROUTE			0x4B
#define CY_IDE1_ROUTE			0x4C
#define	CY_SA_USB_CTRL			0x4D

void outLed(ui d)
{
  long i;
  outportb(LEDPORT, d);
  for(i=0;i<0x2000000;i++);
}


void ISABusInit(void)
{
  int device_id;

  isa_present= 0;
  device_id= incfgl(0, 5, 0, 0);
  if ((device_id!=0) && (device_id!=-1))
  {
    isa_present= 1;
    outcfgb(0, 5, 0, CY_COMMAND_REG, 0xf);
    outcfgb(0, 5, 0, CY_INTA_ROUTE, 0x80);
    outcfgb(0, 5, 0, CY_INTB_ROUTE, 0x80);
    outcfgb(0, 5, 0, CY_INTC_ROUTE, 0x80);
    outcfgb(0, 5, 0, CY_INTD_ROUTE, 0x80);
    outcfgb(0, 5, 0, CY_CONTROL_REG, 0x27);
    outcfgb(0, 5, 0, CY_ERR_CONTROL_REG, 0xe0);
    outcfgb(0, 5, 0, CY_BIOS_CONTROL_REG, 0xe0);
    outcfgb(0, 5, 0, CY_TOP_OF_MEM, 0xf0);
    outcfgb(0, 5, 0, CY_ATCTRL_REG1, 0x40);
    outcfgb(0, 5, 0, CY_IDE0_ROUTE, 0x80);
    outcfgb(0, 5, 0, CY_IDE1_ROUTE, 0x80);
    outcfgb(0, 5, 0, CY_SA_USB_CTRL, 0x70);
    SMC669Init();
  }
  else 
    AcerSIOInit();
//  printf("PCI_CONFIG(bus,dev,func,reg) %lx= %x %x %x %x %x\n",
//       PCI_CONFIG(0, 5, 0, 0),device_id,0, 5, 0, 0);
 
}

/*
 * Clear the NODEV status and
 * return the current value (TRUE/FALSE).
 */
ul IOPCIClearNODEV(void)
{
#if 0
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
#endif
  return FALSE;
}

void WRITE_FLASH_UCHAR(ul Offset, ub Value)
{
  Value&=0xff;
  WriteIOQ(TIG_ACCESS(Offset),Value);
  mb();
}

ub READ_FLASH_UCHAR ( ul FlashOffset )
{
  ul result;
  result = ReadIOQ(TIG_ACCESS(FlashOffset));
  return(result & 0xff);
}

ub inrom(ui p){
	return (READ_FLASH_UCHAR((ul )p));
}

#define FLASH_ENABLE_PORT	0x00C00001

void FlashWriteEnable(int enable)
{
  WRITE_FLASH_UCHAR(FLASH_ENABLE_PORT, enable ? 1 : 0);
}


