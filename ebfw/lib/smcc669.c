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
static char *rcsid = "$Id: smcc669.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: smcc669.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/05/01  21:06:16  pbell
 * Initial revision
 *
 */
#include "lib.h"
//#include "smc.h"
#include "smcc669.h"

static ui SMC669UltraBase;

static ui 
SMC669ConfigState(ui baseAddr)
{
  int devId;
  ui configPort;
  ui indexPort;
  ui dataPort;

  configPort = indexPort = baseAddr;
  dataPort = (ui) ((char *) configPort + 1);

  outportb( configPort, CONFIG_ON_KEY );
  outportb( configPort, CONFIG_ON_KEY );
  outportb( indexPort,  DEVICE_ID );
  devId = inportb( dataPort );
  if (devId != SMC37c669_DEVICE_ID){
    baseAddr = 0;
  }
  return (baseAddr);
}

static ui 
SMC669detectUltraIO()
{
  ui baseAddr;

  baseAddr = 0x3f0;
  if ((baseAddr = SMC669ConfigState(baseAddr)) == 0x3f0){
    return baseAddr;
  }
  baseAddr = 0x370;
  if ((baseAddr = SMC669ConfigState(baseAddr)) == 0x370){
    return baseAddr;
  }
  return (ui) 0;
}

static void 
SMC669runState(ui baseAddr)
{
  outportb( baseAddr, CONFIG_OFF_KEY );
}

/*
 *
 */
static void
SMC669EnableCOM1(ui baseAddr)
{
  ui indexPort;
  ui dataPort;
  ui irqData;

  indexPort = baseAddr;
  dataPort = (ui) ((char *) baseAddr + 1);

  outportb( indexPort, SMC37c669_SERIAL_IRQ_INDEX);
  irqData = inportb( dataPort );
  irqData = (irqData & 0x0f) | 0x50;	/* need info from dan */
  outportb( dataPort, irqData );

  outportb( indexPort, SMC37c669_SERIAL0_BASE_ADDRESS_INDEX);
  outportb( dataPort, COM1_BASE >> 2 );

}

static void
SMC669EnableCOM2(ui baseAddr)
{
  ui indexPort;
  ui dataPort;
  ui irqData;

  indexPort = baseAddr;
  dataPort = (ui) ((char *) baseAddr + 1);

  outportb( indexPort, SMC37c669_SERIAL_IRQ_INDEX);
  irqData = inportb( dataPort );
  irqData = (irqData & 0xf0) | 0x06;	/* need info from dan */
  outportb( dataPort, irqData );

  outportb( indexPort, SMC37c669_SERIAL1_BASE_ADDRESS_INDEX);
  outportb( dataPort, COM2_BASE >> 2 );

}


void SMC669EnablePARP (ui baseAddr)
{
    ui indexPort;
    ui dataPort;
	ui dmaData;
	ui irqData;

    indexPort = baseAddr;
    dataPort = (ui)((char *)baseAddr + 1);

    outportb( indexPort, SMC37c669_PARALLEL_FDC_DMA_INDEX);
	dmaData = inportb( dataPort );
	dmaData = (dmaData & 0xf0) | 0x2;	/* need info from dan */
    outportb( dataPort, dmaData);	/* Select parallel port dma */

	outportb( indexPort, SMC37c669_PARALLEL_FDC_IRQ_INDEX);
	irqData = inportb( dataPort );
	irqData = (irqData & 0xf0) | 0x03;	/* need info from dan */
	outportb( dataPort, irqData );

	outportb( indexPort, SMC37c669_PARALLEL0_BASE_ADDRESS_INDEX);
	outportb( dataPort, PARP_BASE >> 2 );

}

static void
SMC669DisableIDE(ui baseAddr)
{
    ui indexPort;
    ui dataPort;

    indexPort = baseAddr;
    dataPort = (ui)((char *)baseAddr + 1);

	outportb( indexPort, SMC37c669_IDE_BASE_ADDRESS_INDEX);
	outportb( dataPort, 0 );

	outportb( indexPort, SMC37c669_IDE_ALTERNATE_ADDRESS_INDEX);
	outportb( dataPort, 0 );

}

static void
SMC669EnableFDC(ui baseAddr)
{
    ui indexPort;
    ui dataPort;
	ui dmaData;
	ui irqData;

    indexPort = baseAddr;
    dataPort = (ui)((char *)baseAddr + 1);

    outportb( indexPort, SMC37c669_PARALLEL_FDC_DMA_INDEX);
	dmaData = inportb( dataPort );
	dmaData = (dmaData & 0x0f) | 0x10;	/* need info from dan */
    outportb( dataPort, dmaData);	/* Select parallel port dma */

	outportb( indexPort, SMC37c669_PARALLEL_FDC_IRQ_INDEX);
	irqData = inportb( dataPort );
	irqData = (irqData & 0x0f) | 0x40;	/* need info from dan */
	outportb( dataPort, irqData );

	outportb( indexPort, SMC37c669_FDC_BASE_ADDRESS_INDEX);
	outportb( dataPort, FDC_BASE >> 2 );

}

void 
SMC669Init(void)
{

    SMC669UltraBase = SMC669detectUltraIO();
    if (SMC669UltraBase != 0) {    /* SMC Super I/O chip was detected. */
        SMC669EnableFDC(SMC669UltraBase);
	SMC669EnablePARP(SMC669UltraBase);
        SMC669DisableIDE(SMC669UltraBase);   /* We'll be using on board IDE */
        SMC669EnableCOM1(SMC669UltraBase);
        SMC669EnableCOM2(SMC669UltraBase);
        SMC669runState(SMC669UltraBase);
  
    }
}

