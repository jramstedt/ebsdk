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
__attribute__((unused)) static const char *rcsid = "$Id: acersio.c,v 1.1.1.1 1998/12/29 21:36:12 paradis Exp $";
#endif

/*
 * $Log: acersio.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:12  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1998/07/13  14:36:00  bissen
 * Initial revision
*/
/*
 *
 * Original author: Dick Bissen
 * date: 7-july-98
 *
 */
#include "lib.h"
#include "acersio.h"

static uintptr_t AcerSIOBase;

static uintptr_t 
AcerSIOConfigState(ui baseAddr)
{
  ui devId;
  uintptr_t configPort;
  uintptr_t indexPort;
  uintptr_t dataPort;

  configPort = indexPort = baseAddr;
  dataPort = (uintptr_t) ((char *) configPort + 1);

  outportb( configPort, CONFIG_ON_KEY1 );
  outportb( configPort, CONFIG_ON_KEY2 );
  outportb( indexPort,  DEVICE_ID );
  devId = inportb( dataPort );
  if (devId != VALID_DEVICE_ID){
    baseAddr = 0;
  }
  return (baseAddr);
}

static uintptr_t 
AcerSIOdetectUltraIO()
{
  uintptr_t baseAddr;

  baseAddr = 0x3f0;
  if ((baseAddr = AcerSIOConfigState(baseAddr)) == 0x3f0){
    return baseAddr;
  }
  baseAddr = 0x370;
  if ((baseAddr = AcerSIOConfigState(baseAddr)) == 0x370){
    return baseAddr;
  }
  return (uintptr_t) 0;
}

static void 
AcerSIOrunState(ui baseAddr)
{
  outportb( baseAddr, CONFIG_OFF_KEY );
}

/*
 *
 */
static void
AcerSIOEnableCOM1(uintptr_t baseAddr)
{
  uintptr_t indexPort;
  uintptr_t dataPort;

  indexPort = baseAddr;
  dataPort = (uintptr_t) ((char *) baseAddr + 1);

  outportb( indexPort, LOGICAL_DEVICE_NUMBER );
  outportb( dataPort,  SER1);	/* select com1 */

  outportb( indexPort, ADDR_LOW );
  outportb( dataPort,  (COM1_BASE & 0xff) );

  outportb( indexPort, ADDR_HI );
  outportb( dataPort,  ((COM1_BASE >> 8) & 0xff));

  outportb( indexPort, INTERRUPT_SEL );
  outportb( dataPort,  (COM1_INTERRUPT) );

  outportb( indexPort, ACTIVATE );
  outportb( dataPort,  DEVICE_ON );
}

static void
AcerSIOEnableCOM2(uintptr_t baseAddr)
{
  uintptr_t indexPort;
  uintptr_t dataPort;

  indexPort = baseAddr;
  dataPort = (uintptr_t) ((char *) baseAddr + 1);

  outportb( indexPort, LOGICAL_DEVICE_NUMBER );
  outportb( dataPort,  SER2);	/* select com2 */

  outportb( indexPort, ADDR_LOW );
  outportb( dataPort,  (COM2_BASE & 0xff) );

  outportb( indexPort, ADDR_HI );
  outportb( dataPort,  ((COM2_BASE >> 8) & 0xff));

  outportb( indexPort, INTERRUPT_SEL );
  outportb( dataPort,  (COM2_INTERRUPT) );

  outportb( indexPort, ACTIVATE );
  outportb( dataPort,  DEVICE_ON );
}


void AcerSIOEnablePARP (uintptr_t baseAddr)
{
  uintptr_t indexPort;
  uintptr_t dataPort;

    indexPort = baseAddr;
    dataPort = (uintptr_t)((char *)baseAddr + 1);

    outportb( indexPort, LOGICAL_DEVICE_NUMBER);
    outportb( dataPort, PARP);	/* Select parallel port */

    outportb( indexPort, ADDR_LOW);
    outportb( dataPort, (PARP_BASE & 0xFF));

    outportb( indexPort, ADDR_HI);
    outportb( dataPort, ((PARP_BASE >> 8) & 0xFF));

    outportb( indexPort, INTERRUPT_SEL);
    outportb( dataPort, PARP_INTERRUPT);

    outportb( indexPort, ACTIVATE);
    outportb( dataPort, DEVICE_ON);
}

static void
AcerSIOEnableRTC(uintptr_t baseAddr)
{
  uintptr_t indexPort;
  uintptr_t dataPort;

  indexPort = baseAddr;
  dataPort = (uintptr_t) ((char *) baseAddr + 1);

  outportb( indexPort, LOGICAL_DEVICE_NUMBER );
  outportb( dataPort, RTCL );	/* select real time clock */

  outportb( indexPort, ACTIVATE );
  outportb( dataPort,  DEVICE_ON );
}

static void
AcerSIOEnableKeyboard(uintptr_t baseAddr)
{
  uintptr_t indexPort;
  uintptr_t dataPort;

  indexPort = baseAddr;
  dataPort = (uintptr_t) ((char *) baseAddr + 1);

  outportb( indexPort, LOGICAL_DEVICE_NUMBER );
  outportb( dataPort, KYBD );	/* select keyboard/mouse */

  outportb( indexPort, 0x70 );	// primary interrupt
  outportb( dataPort,  0x01 );

  outportb( indexPort, 0x72 );	// secondary interrupt
  outportb( dataPort,  0x0c );

  outportb( indexPort, ACTIVATE );
  outportb( dataPort,  DEVICE_ON );
}

static void
AcerSIOEnableFDC(uintptr_t baseAddr)
{
  uintptr_t indexPort;
  uintptr_t dataPort;

  uintptr_t oldValue;

  indexPort = baseAddr;
  dataPort = (uintptr_t) ((char *) baseAddr + 1);

  outportb( indexPort, LOGICAL_DEVICE_NUMBER );
  outportb( dataPort, FDC );	/* select floppy controller */

  outportb( indexPort, FDD_MODE_REGISTER );
  oldValue = inportb( dataPort );

  oldValue |= 0x0E;		/* enable burst mode */
  outportb( dataPort, oldValue );

  outportb( indexPort, 0x70 );	/* set up primary interrupt select */
  outportb( dataPort,  0x06 );	/*    to 6 */

  outportb( indexPort, 0x74 );	/* set up dma channel select       */
  outportb( dataPort,  0x02 );	/*    to 2 */

  outportb( indexPort, ACTIVATE );
  outportb( dataPort,  DEVICE_ON );
}

#ifdef DEBUG_AcerSIO
void
AcerSIOReportDeviceStatus(uintptr_t baseAddr)
{
  uintptr_t indexPort;
  uintptr_t dataPort;
  ui currentControl;
  ui fer;

  indexPort = baseAddr;
  dataPort = (uintptr_t) ((char *) baseAddr + 1);

  outportb( indexPort, POWER_CONTROL );
  currentControl = inportb( dataPort );

  if (currentControl & (1 << FDC)){
    printf("\t+FDC enabled\n");
  } else {
    printf("\t-FDC disabled\n");
  }

  if (currentControl & (1 << PARP)){
    printf("\t+Parallel port enabled\n");
  } else {
    printf("\t-Parallel port disabled\n");
  }

  if (currentControl & (1 << SER1)){
    printf("\t+SER1 enabled\n");
  } else {
    printf("\t-SER1 disabled\n");
  }

  if (currentControl & (1 << SER2)){
    printf("\t+SER2 enabled\n");
  } else {
    printf("\t-SER2 disabled\n");
  }

  outportb( 0x398, 0 );
  fer = inportb( 0x399 ) & 0xff;
  printf("onboard super i/o: %x\n", fer);
  printf("\n");
}

/*
 * disable onboard super I/O functions and RTC, KBD, Mouse
 */
void disable_onboard()
{
  outportb( 0x3f2, 0 );		/* disable old floppy from driving int6,drq  */

  outportb( 0x398, 0 );		/* FER index */
  outportb( 0x399, 0x00 );	/* FER data - disable all functions */
  outportb( 0x399, 0x00 );	/* (need to write twice) */

  outcfgb( 0, 19, 0x4d, 0x40 );	/* disable keyboard and RTC access thru sio */
#ifdef OFFBOARD_RTC
  outcfgb( 0, 19, 0x4e, 0xc0 );	/* disable keyboard and RTC access thru sio */
#else
  outcfgb( 0, 19, 0x4e, 0xc1 );	/* disable keyboard access thru sio */
#endif
  outcfgb( 0, 19, 0x4f, 0xff );	/* (was 7f) disable all but port 92 */
}


void enable_offboard()
{

  outportb( 0x3f2, 0x08 );	/* DMA enable/reset for offboard floppy */
  outportb( 0x3f2, 0x0c );	/* clear reset                          */

}
void 
AcerSIOInit(void)
{

    AcerSIOBase = AcerSIOdetectUltraIO();
    if (AcerSIOBase != 0) {    /* AcerSIO Super I/O chip was detected. */
        printf("AcerSIO board detected @ base %x\n", AcerSIOBase);
        AcerSIOReportDeviceStatus(AcerSIOBase);
        AcerSIOEnableCOM1(AcerSIOBase);
        AcerSIOEnableCOM2(AcerSIOBase);

        AcerSIOEnableRTC(AcerSIOBase);
        AcerSIOEnableKeyboard(AcerSIOBase);
        AcerSIOEnableFDC(AcerSIOBase);

        AcerSIOReportDeviceStatus(AcerSIOBase);
        printf("about to disable onboard superIO, etc...\n");
        disable_onboard();
        AcerSIOrunState(AcerSIOBase);
        enable_offboard();
    } 
    else {
        printf("No AcerSIO board detected.\n");
    }

}
#else
void 
AcerSIOInit(void)
{

    AcerSIOBase = AcerSIOdetectUltraIO();
    if (AcerSIOBase != 0) {    /* AcerSIO Super I/O chip was detected. */
        AcerSIOEnableRTC(AcerSIOBase);
        AcerSIOEnableFDC(AcerSIOBase);
        AcerSIOEnablePARP(AcerSIOBase);
        AcerSIOEnableCOM1(AcerSIOBase);
        AcerSIOEnableCOM2(AcerSIOBase);
        AcerSIOEnableKeyboard(AcerSIOBase);
        AcerSIOrunState(AcerSIOBase);
    }
}
#endif

