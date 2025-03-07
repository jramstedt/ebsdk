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
__attribute__((unused)) static const char *rcsid = "$Id: smc.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: smc.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1997/05/01  20:59:09  pbell
 * Added an ifdef test to exclude this code if not the SX.
 *
 * Revision 1.3  1996/08/13  18:47:35  fdh
 * Corrected I/O port used to setup dma channel for
 * the floppy controller.
 * Added SMC init for the parallel port.
 *
 * Revision 1.2  1996/04/30  20:27:25  cruz
 * Deleted some unused variables and commented out
 * unused routines to make lint happy.
 *
 * Revision 1.1  1996/02/20  18:42:48  cruz
 * Initial revision
 *
 *
 * Original author: Ken Curewitz
 * date: 7-aug-95
 *
 */
#ifndef	SX164
#include "lib.h"
#include "smc.h"

static uintptr_t SMCUltraBase;

static uintptr_t 
SMCConfigState(uintptr_t baseAddr)
{
  ui devId;
  uintptr_t configPort;
  uintptr_t indexPort;
  uintptr_t dataPort;

  configPort = indexPort = baseAddr;
  dataPort = (uintptr_t) ((char *) configPort + 1);

  outportb( configPort, CONFIG_ON_KEY );
  outportb( configPort, CONFIG_ON_KEY );
  outportb( indexPort,  DEVICE_ID );
  devId = inportb( dataPort );
  if (devId != VALID_DEVICE_ID){
    baseAddr = 0;
  }
  return (baseAddr);
}

static uintptr_t 
SMCdetectUltraIO()
{
  uintptr_t baseAddr;

  baseAddr = 0x3f0;
  if ((baseAddr = SMCConfigState(baseAddr)) == 0x3f0){
    return baseAddr;
  }
  baseAddr = 0x370;
  if ((baseAddr = SMCConfigState(baseAddr)) == 0x370){
    return baseAddr;
  }
  return (uintptr_t) 0;
}

static void 
SMCrunState(uintptr_t baseAddr)
{
  outportb( baseAddr, CONFIG_OFF_KEY );
}

/*
 *
 */
static void
SMCEnableCOM1(uintptr_t baseAddr)
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
SMCEnableCOM2(uintptr_t baseAddr)
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


void SMCEnablePARP (uintptr_t baseAddr)
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

#ifdef DEBUG_SMC
static void
SMCEnableIDE1(uintptr_t baseAddr)
{
  uintptr_t indexPort;
  uintptr_t dataPort;

  indexPort = baseAddr;
  dataPort = (uintptr_t) ((char *) baseAddr + 1);

  outportb( indexPort, LOGICAL_DEVICE_NUMBER );
  outportb( dataPort, IDE1 );	/* select IDE1 */

  outportb( indexPort, ACTIVATE );
  outportb( dataPort,  DEVICE_ON );
}
#endif

static void
SMCDisableIDE(uintptr_t baseAddr, uintptr_t ide_id)
{
  uintptr_t indexPort;
  uintptr_t dataPort;

  indexPort = baseAddr;
  dataPort = (uintptr_t) ((char *) baseAddr + 1);

  outportb( indexPort, LOGICAL_DEVICE_NUMBER );
  outportb( dataPort, (ub) ide_id ); /* select IDEn */

  outportb( indexPort, ACTIVATE );
  outportb( dataPort,  DEVICE_OFF );
}

static void
SMCEnableRTC(uintptr_t baseAddr)
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
SMCEnableKeyboard(uintptr_t baseAddr)
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
SMCEnableFDC(uintptr_t baseAddr)
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

#ifdef DEBUG_SMC
void
SMCReportDeviceStatus(uintptr_t baseAddr)
{
  uintptr_t indexPort;
  uintptr_t dataPort;
  ui currentControl;
  ui fer;

  indexPort = baseAddr;
  dataPort = (ui) ((char *) baseAddr + 1);

  outportb( indexPort, POWER_CONTROL );
  currentControl = inportb( dataPort );

  if (currentControl & (1 << FDC)){
    printf("\t+FDC enabled\n");
  } else {
    printf("\t-FDC disabled\n");
  }

  if (currentControl & (1 << IDE1)){
    printf("\t+IDE1 enabled\n");
  } else {
    printf("\t-IDE1 disabled\n");
  }

  if (currentControl & (1 << IDE2)){
    printf("\t+IDE2 enabled\n");
  } else {
    printf("\t-IDE2 disabled\n");
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
SMCInit(void)
{

    SMCUltraBase = SMCdetectUltraIO();
    if (SMCUltraBase != 0) {    /* SMC Super I/O chip was detected. */
        printf("SMC board detected @ base %x\n", SMCUltraBase);
        SMCReportDeviceStatus(SMCUltraBase);
        SMCEnableCOM1(SMCUltraBase);
        SMCEnableCOM2(SMCUltraBase);

        SMCEnableRTC(SMCUltraBase);
        SMCDisableIDE(SMCUltraBase, IDE1);
        SMCDisableIDE(SMCUltraBase, IDE2);
        SMCEnableKeyboard(SMCUltraBase);
        SMCEnableFDC(SMCUltraBase);

        SMCReportDeviceStatus(SMCUltraBase);
        printf("about to disable onboard superIO, etc...\n");
        disable_onboard();
        SMCrunState(SMCUltraBase);
        enable_offboard();
    } 
    else {
        printf("No SMC board detected.\n");
    }

}
#else
void 
SMCInit(void)
{

    SMCUltraBase = SMCdetectUltraIO();
    if (SMCUltraBase != 0) {    /* SMC Super I/O chip was detected. */
        SMCEnableRTC(SMCUltraBase);
        SMCEnableFDC(SMCUltraBase);
	SMCEnablePARP(SMCUltraBase);
        SMCDisableIDE(SMCUltraBase, IDE1);   /* We'll be using on board IDE */
        SMCDisableIDE(SMCUltraBase, IDE2);
        SMCEnableCOM1(SMCUltraBase);
        SMCEnableCOM2(SMCUltraBase);
        SMCEnableKeyboard(SMCUltraBase);
        SMCrunState(SMCUltraBase);
    }
}
#endif

#endif	/* SX164 */
