/*****************************************************************************

Copyright 1993, 1994, 1995 Digital Equipment Corporation, Maynard, Massachusetts.

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
__attribute__((unused)) static const char *rcsid = "$Id: DEC21040.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif

/*
 * MODULE DESCRIPTION:
 *
 *     DECCHIP_21040 Ethernet device interface for
 *     the Alpha Evaluation Board Debug Monitor.
 *
 * HISTORY:
 *
 * $Log: DEC21040.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.52  1997/09/29  18:23:07  pbell
 * Moved all perminant buffers to the net buffer pool.
 *
 * Revision 1.51  1997/07/23  20:07:37  pbell
 * Updated to support the shipping version of the DC21143
 * where autonegotiation works.
 *
 * Revision 1.50  1997/07/10  00:57:38  fdh
 * Modified to init buffers based on the DMA_BUFF_BASE macro.
 *
 * Revision 1.49  1997/07/03  21:03:57  pbell
 * Cleaned up device names used by eshow and removed most of the
 * 21040 components of names and strings using 21x4x instead.
 *
 * Revision 1.48  1997/07/01  20:18:58  pbell
 * Fixed an init error after a hard reset for the 21140.
 *
 * Revision 1.47  1997/07/01  19:46:17  pbell
 * Added MII port support for DC2114[023] 10/100 Mbit.
 *
 * Revision 1.46  1997/05/23  15:14:17  pbell
 * Added recognition support for DC21140A, DC21142, DC21143 and
 * operational support for DC21140A on 10Base-T.
 *
 * Revision 1.45  1997/05/14  14:44:37  pbell
 * Removed debug register definitions TDES1_ADD, RDES1_ADD, and PCI_DIAG
 *
 * Revision 1.44  1996/06/13  03:32:38  fdh
 * Minor cleanup.
 *
 * Revision 1.43  1996/06/03  17:41:09  fdh
 * Added support for the 21041 and 21140 devices.
 *
 * Revision 1.42  1996/05/24  20:10:39  fdh
 * Modified in accordance with to a change to the PCIDevice data structure.
 *
 * Revision 1.41  1996/05/22  21:45:24  fdh
 * Modified to remove use of obsoleted delay routines.
 *
 * Revision 1.40  1995/12/15  19:45:00  cruz
 * Changed name from DECChip to Digital Semiconductor to reflect
 * the current name.
 *
 * Revision 1.39  1995/12/11  21:00:31  cruz
 * Changed a printed message.
 *
 * Revision 1.38  1995/12/11  19:35:04  cruz
 * Print bus number in messages printed.
 * Print the CBMA register with ereg command.
 *
 * Revision 1.37  1995/12/08  02:48:09  cruz
 * Removed extra linefeeds in print statement.
 *
 * Revision 1.36  1995/11/13  14:48:48  cruz
 * Corrected chip name to follow official spelling.
 *
 * Revision 1.35  1995/10/30  16:54:25  cruz
 * Removed declaration of PutDotChar which is not used here.
 * Commented out routine(s) not in use.
 *
 * Revision 1.34  1995/10/27  15:16:37  cruz
 * Removed extern declaration of etherprom and etherdump since they
 * are now included in ether.h
 *
 * Revision 1.33  1995/10/23  15:21:09  fdh
 * Condensed startup messages to one line.
 *
 * Revision 1.32  1995/10/20  20:05:57  cruz
 * Cleanup for lint
 *
 * Revision 1.31  1995/10/20  18:37:18  cruz
 * Performed some clean up.  Updated copyright headers.
 * Fixed bug which used the NOT (!) operator instead of
 * the Negate (~) operator.
 *
 * Revision 1.30  1995/10/18  18:10:11  fdh
 * Added DECchip_21040_device_stop().
 *
 * Revision 1.29  1995/09/02  06:55:40  fdh
 * Removed declaration for WaitUs.
 *
 * Revision 1.28  1995/09/02  06:21:56  fdh
 * Included time.h
 *
 * Revision 1.27  1995/08/25  20:07:32  fdh
 * Added function arg to references to PCI Config space
 * access routines.
 *
 * Revision 1.26  1995/02/23  21:42:04  fdh
 * Properly cast a data type.
 *
 * Revision 1.25  1995/01/20  18:47:52  cruz
 * Fixed a few bugs in the device initialization routine.
 *
 * Revision 1.24  1994/11/01  11:30:49  rusling
 * Now use new PCI configuration header support rather
 * than directly accessing it myself.
 *
 * Revision 1.23  1994/10/18  11:09:00  berent
 * Added   broadcast address to perfect filter; hence allowing
 * the evaluation boards to respond to broadcast arp messages.
 * This fixes the problem where Netload and ladebug used to die
 * some time after reset.
 *
 * Revision 1.22  1994/07/16  16:54:01  fdh
 * Modified device startup messages...
 *
 * Revision 1.21  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.20  1994/06/22  15:11:46  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.19  1994/06/20  14:41:21  fdh
 * Fixup preprocessor #include conditionals.
 *
 * Revision 1.18  1994/06/17  19:36:25  fdh
 * Clean-up...
 *
 * Revision 1.17  1994/06/03  20:07:13  fdh
 * Replaced <stdio.h> with <stddef.h>.
 *
 * Revision 1.16  1994/04/10  03:23:56  fdh
 * Added some small delays around the device reset.
 *
 * Revision 1.15  1994/04/09  05:18:48  fdh
 * Implemented the ..device_clear_interrupts() routine
 * which was previously stubbed.
 *
 * Revision 1.14  1994/04/07  19:07:16  fdh
 * Removed obsoleted saved_sromid variable.
 *
 * Revision 1.13  1994/03/30  09:45:00  rusling
 * Fixed up the initialization code.  It can now select between
 * both TP and AUI correctly.
 *
 * Revision 1.12  1994/03/16  14:18:35  rusling
 * Now we always soft reset the chip on device_init().
 * Before it was optional.
 *
 *
 */

#include "ether.h"

#ifdef NEED21040
#include "pci.h"
#include "edriver.h"
#include "DEC21040.h"


/*
 *  Define the size of the transmit and receive rings.
 */
#define RRING_SIZE 8
#define SRING_SIZE 4

/* MODULE DESCRIPTION:
 *
 * This file contains DECCHIP_21x4x specific routines needed for an Ethernet
 * driver. The driver follows the generic EB64 Ethernet architecture functions
 * with support for the following routines:
 *
 *	    o ether_device_init()
 *	    o ether_device_read()
 *	    o ether_device_write()
 *	    o ether_device_flush()
 *	    o ether_device_print_stats()
 *	    o ether_device_clear_interrupts()
 *	    o ether_device_get_hw_addr()
 *
 *	    o ether_device_register()
 *	    
 * Please see the individual routines for the parameter listings and any
 * returned values.
 *
 * When a device is installed (registered) a structure including pointers to
 * these functions will be built. The structure is part of an array, the array
 * entry being dictated by the device number. While these routines have 
 * prefixes of AMD79C960, they will be called from the registered array using
 * the generic "ether" prefixed routines.
 * 
 * Supporting routines include:
 *
 *        Routines to read and write the RDP and IDP registers for CSR IO
 *        (these include implicit RAP register control)
 *
 *        Start the device and reset the counter array
 *
 *        A selection of utilities to print packets (RX and TX), print
 *        the counter array, and dump the CSR's.
 *
 * The main functions are:
 *
 *        Ethernet initialisation - set up initialisation block, set up
 *        RX and TX descriptor rings, allocate buffer space and define 
 *        arrays of pointers to the TX and RX buffers, then initialise
 *        the device.
 *
 *        Receive a packet in a single buffer
 *
 *        Send a packet from a single buffer
 *
 *	  Take control (with a watchdog) until all queued packets sent
 *
 *        Clear down the daisy chained interrupt in the 82C486 ISA controller
 *
 *	  Fetch the associated MAC address
 *
 *	  Register a new device in the Ethernet dispatch table
 *
 * Assumptions are:
 *
 *        All buffers can hold maximum length packets - no chaining
 *
 *        NOTE: static allocation of buffer memory means that the
 *              maximum number of DECCHIP_21x4x must be declared at compile time
 *	        using DECCHIP_21x4x_MAX_DEVICES. IO base addresses are also fixed
 *              for all of these at compile time. Device registration has to 
 *              ensure there is no overlapping IO address ports between 
 *	        different device types if the possibility exists at compile 
 *		time.
 *
 *	  Once a driver is running and sending/receiving traffic, pointers
 *	  to buffers are then swapped with the datalink layer, meaning that
 *        the number of buffers allocated to the routine remains constant,
 *	  but the actual buffers statically assigned at compile time migrate
 *	  through the system.
 *
 *	  An array is used to map device numbers to ISA bus base addresses.
 *	  Entries are added to the array when a device is registered and
 *	  allocated a device number. 
 *
 *        PLEASE NOTE: This array is currently Ethernet specific.
 *
 *        Global parameters are used to set up the following:
 *
 *                etherdump - dictates whether packets sent and received
 *                            are output to the monitor using printf()
 *
 */

/* DECCHIP_21x4x_MAX_DEVICES copies of resources are statically assigned at compile time.  */

/*
 * Control block and data memory requirements are as follows:
 *
 *        Initialisation block
 *        RX descriptors (number defined as RRING_SIZE)
 *        TX descriptors (number defined as SRING_SIZE)
 *        RX buffers (number defined as RRING_SIZE; size of BUFFER_SIZE)
 *        TX buffers (number defined as SRING_SIZE; size of BUFFER_SIZE)
 *
 * Two pointer arrays and index variables are created for buffer manipulation:
 *
 *        current_rring_buffers[][] for RX
 *        current_sring_buffers[][] for TX (send)
 */
/*
 * A description of all the DECchip 21x4x devices in this system.
 * Information is filled in via DECchip_21040_device_init_module().
 */
static DECchip_21x4x_device_t devices[DECCHIP21x4x_MAX_DEVICES];
/* 
 * Allocated by DECchip_21040_device_init_module().  Buffers are
 * only allocated for valid devices.
 */

static unsigned char *current_rring_buffers[DECCHIP21x4x_MAX_DEVICES][
  RRING_SIZE];
static unsigned char *current_sring_buffers[DECCHIP21x4x_MAX_DEVICES][
  SRING_SIZE];

/* 
 * Statically allocate all of the rx and tx descriptors (there aren't
 * too many).
 */

static DECCHIP_21x4x_RX_BD *rring[DECCHIP21x4x_MAX_DEVICES];
					/* pointer to RX ring descriptor memory
					 * for all potential devices */
static DECCHIP_21x4x_TX_BD *sring[DECCHIP21x4x_MAX_DEVICES];
					/* pointer to TX ring descriptor memory
					 * for all potential devices */

/* 
 * Functions declared within 
 */
static DECchip_21x4x_device_t *DECchip_21x4x_device_find(int device_no);
static int DECchip_21x4x_device_init(int device_no);
static int DECchip_21x4x_device_read(int device_no, 
  unsigned char balance_buffer[ETHER_BUFFER_SIZE], 
  unsigned char **frame_buffer);

static int DECchip_21x4x_device_write(int device_no, unsigned char *frame, 
  int size, unsigned char frame_buffer[ETHER_BUFFER_SIZE], 
  unsigned char **balance_buffer);
static int DECchip_21x4x_device_flush(int device_no);
static void DECchip_21x4x_device_print_stats(int device_no);
static void DECchip_21x4x_device_clear_interrupts(int device_no);
static void DECchip_21040_device_get_hw_address(int device_no, 
  mac_addr hw_address);
static void DECchip_21x4x_device_stop(int device_no);
static void DECchip_21x4x_device_preg(int device_no);
					/* dump all registers */

static int DECchip_21x4x_device_check_romid(unsigned char * romid);
static int DECchip_21x4x_device_read_romid(mac_addr hw_address);
static void DECchip_21x4x_device_allocate_buffers(DECchip_21x4x_device_t * device);
static int DECchip_21x4x_device_hw_address_valid(mac_addr hw_address);

static void DECchip_21x4x_device_get_hw_address(int device_no, 
  mac_addr hw_address);

static void srom_latch(ui command, uw addr);
static void srom_command(ui command, uw addr);
static void srom_address(ui command, uw addr, int offset);
static void sendto_srom(ui command, uw addr);
static ui getfrom_srom(uw addr);

#ifdef NOT_IN_USE
static void DECchip_21x4x_device_start(int device_no);
#endif

static struct {
  int DeviceID;
  char *DeviceIDString;
} DC21X4X_DID_Data[] = {
  { DECCHIP_21040_DID, "DC21040" },
  { DECCHIP_21041_DID, "DC21041" },
  { DECCHIP_21140_DID, "DC21140" },
  { DECCHIP_21142_DID, "DC21142" },
  { DECCHIP_21143_DID, "DC21143" },
  { 0, NULL }
};

/* DECchip_21x4x_device_find()
 *
 * Arguments:
 * 
 *    device_no    - device number 
 *
 * Returned Value:
 *    address of device or NULL if failed.
 */
static DECchip_21x4x_device_t *DECchip_21x4x_device_find(int device_no)
{
    register int i;
    register DECchip_21x4x_device_t *device = devices;

    for (i = 0; i < DECCHIP21x4x_MAX_DEVICES; i++, device++)
	if ((int)(device->device) == device_no)
	    return (device);
    return (NULL);
}


/* DECchip_21x4x_Reset - Resets the chip and sets up PCI space.
 *
 * Arguments:
 *
 *    device    - device structure
 */

static void DECchip_21x4x_Reset(
    DECchip_21x4x_device_t *device)
{
    ui l;
    /*
     * Reset the chip.
     */
    device->flags.initialised = TRUE;
    _21x4xWriteCSR(device, CSR0, CSR0_SWR);
    /*
     *  Delay a bit before accessing device again after
     *  asserting reset.
     */
    msleep(1);
    /*
     *  Tell the chip where in PCI I/O space its CSRs live.  
     */
    PCISetDeviceConfig(device->pci);
    /*
     *  Allow the device to be bus master.
     */
    l = incfgl(_PCI_Bus(device->pci), _PCI_Slot(device->pci), 0, CFCS);
    l |= 0x4;
    outcfgl(_PCI_Bus(device->pci), _PCI_Slot(device->pci), 0, CFCS, l);
    outcfgl(_PCI_Bus(device->pci), _PCI_Slot(device->pci), 0, CFLT, 0x0000FF00);
    /* Make sure that the chip is not in sleep or snooze mode */
    outcfgl(_PCI_Bus(device->pci), _PCI_Slot(device->pci), 0, CFDD, 0);
    /*
     *  Initialise the chip.
     */
    _21x4xWriteCSR(device, CSR0, 0x00008000);
					/* CAL=2, PBL=b_len, DSL=0 */
    /*
     *  The first bus transaction to this device should occur
     *  after at least 50 PCI cycles from the deassertion of reset.
     */
    msleep(1);

    _21x4xWriteCSR(device, CSR4, 
      l = (unsigned int) 
      (PCIMapAddress((unsigned char *) &sring[device->index][0].u_tdes0.tdes0))
      & CSR4_TXB);
					/* TX ring address       */
    _21x4xWriteCSR(device, CSR3, 
      l = (unsigned int) 
      (PCIMapAddress((unsigned char *) &rring[device->index][0].u_rdes0.rdes0))
      & CSR3_RXB);
					/* RX ring address       */

    _21x4xWriteCSR(device, CSR7, 0x00000000);
					/* Int mask = no interrupt */
    _21x4xWriteCSR(device, CSR13, 0x00000000);
					/* Stop SIA */
}


/* DECchip_21x4x_WaitAutoNegotiationComplete - Waits for autonegotiation to
 *    finish and returns TRUE if a successfull link was made.
 *    *NOTE* the 21142 may return TRUE when there is no valid link so another
 *    examination of csr12 is necessary to determine if the link is valid.
 *
 * Arguments:
 *
 *    device    - device structure
 *
 * Returned value
 *    TRUE         - link successfull.
 *    FALSE        - link failed after negotiation.
 */

static BOOLEAN DECchip_21x4x_WaitAutoNegotiationComplete(
    DECchip_21x4x_device_t *device)
{
    int i, csr12;
    for( i=0; i<6; i++ )
    {
	msleep(500);
	csr12 = _21x4xReadCSR(device, CSR12);
	if( (csr12 & CSR12_ANS) == CSR12_ANS_COM )
	    return( (csr12 & (CSR12_LS10|CSR12_LS100)) != (CSR12_LS10|CSR12_LS100) );
    }
    return( FALSE );
}


/* DECchip_21x4x_device_init - initialises an ethernet device
 *
 * Arguments:
 *
 *    device_no    - device number
 *
 * Returned value
 *    TRUE         - initialised succesfully
 *    FALSE        - error on initialisation
 */

static int DECchip_21x4x_device_init(int device_no)
{

    int i, j, k;
    DECchip_21x4x_device_t *device = DECchip_21x4x_device_find(device_no);
    ui csr6, csr12;
    unsigned int index;

    if (device == NULL)
	return FALSE;

    printf("Digital Semiconductor %s (%d): Initializing\n",
	    DC21X4X_DID_Data[device->device_type].DeviceIDString ,device_no);
/*
 *  If this device doesn't exist or hasn't been allocated, then
 *  return an error.  Otherwise, get the device index.
 */

    if (!(device->flags.exists) || !(device->flags.allocated))
	return FALSE;
    index = device->index;

/*
 * Try and figure out this device's hardware (mac) address, if
 * we cannot figure one out, then fail the initialisation.
 */
    if(device->device_type == DECCHIP_21040_TYPE )
      DECchip_21040_device_get_hw_address(device_no, device->hw_address);
    else
      DECchip_21x4x_device_get_hw_address(device_no, device->hw_address);

    if (!DECchip_21x4x_device_hw_address_valid(device->hw_address)) {
      printf("Digital Semiconductor %s (%d): Error no ethernet address!\n",
	     DC21X4X_DID_Data[device->device_type].DeviceIDString,
	     device_no);
      return FALSE;
    }
    printf("\tHardware address = ");
    ethernet_printaddress(device->hw_address);
    printf("\n");

/* 
 * Set up the transmit and receive rings  for current device_index.
 * NOTE: We set these up using chained descriptors.
 */
    for (i = 0; i < RRING_SIZE; i++) {	/* For all RX descriptor entries */
	memset(&rring[index][i], 0, sizeof(DECCHIP_21x4x_RX_BD));
					/* clear descriptor */
	rring[index][i].u_rdes0.s_rdes0.OWN = TRUE;
					/* 21x4x owns it */
/*
 * If this is the end of the ring, mark it as so.
 */
	if (i == (RRING_SIZE - 1))
	    rring[index][i].u_rdes1.s_rdes1.RER = TRUE;
/*
 * Point at the receive buffer and give its length.
 */
	rring[index][i].u_rdes1.s_rdes1.RBS1 = ETHER_BUFFER_SIZE;
	rring[index][i].rdes2 = (unsigned int) current_rring_buffers[index][i];
	rring[index][i].rdes2 = 
	  PCIMapAddress((unsigned char *) rring[index][i].rdes2);
    }
    for (i = 0; i < SRING_SIZE; i++) {	/* For all TX descriptor entries */
	memset(&(sring[index][i]), 0, sizeof(DECCHIP_21x4x_TX_BD));
					/* clear descriptor */
	sring[index][i].u_tdes0.s_tdes0.OWN = FALSE;
					/* We own it */
	sring[index][i].u_tdes1.s_tdes1.LS = TRUE;
	sring[index][i].u_tdes1.s_tdes1.FS = TRUE;
/*
 * If this is the last descriptor in the ring, then mark it as so.
 */
	if (i == (SRING_SIZE - 1))
	    sring[index][i].u_tdes1.s_tdes1.TER = TRUE;
/*
 * tdes2 contains a pointer to the transmit buffer so point to it
 * and set up its size in tdes1.
 */
	sring[index][i].tdes2 = (unsigned int) current_sring_buffers[index][i];
	sring[index][i].u_tdes1.s_tdes1.TBS1 = ETHER_BUFFER_SIZE;
	sring[index][i].tdes2 = 
	  PCIMapAddress((unsigned char *) sring[index][i].tdes2);
    }

    device->rx_index = 0;		/* Reset RX array index */
    device->tx_index = 0;		/* Reset TX array index */

    device->flags.initialised = TRUE;
    DECchip_21x4x_Reset( device );

    if (device->flags.loopback)
    {
	_21x4xWriteCSR(device, CSR13, 0x00000000);
					/* Stop SIA */
	_21x4xWriteCSR(device, CSR15, 0x00000000);
					/* EXT SIA                 */
	_21x4xWriteCSR(device, CSR14, 0x00000000);
					/* EXT SIA                 */
	_21x4xWriteCSR(device, CSR13, 0x00000001);
					/* EXT SIA (Was 0x00003041)*/
	_21x4xWriteCSR(device, CSR7, 0x00000000);
					/* Int mask = no interrupt */
    }
    else
    {
	/*
	 * Detect the type of interface that is in use.  10BASE-T, AUI, BNC
	 */
	switch( device->device_type )
	{
	case DECCHIP_21040_TYPE:
	case DECCHIP_21041_TYPE:
	    /* First try a TP (10 Base T) connection (ie BIT 3, AUI = 0).
	     * Then wait a little while and read the CSR12 LinkFail status
	     * bit.  If this is set, then the interface has failed, so
	     * try AUI. */
	    printf("\tTrying 10 Base T\n");
	    _21x4xWriteCSR(device, CSR13, (CSR13_CAC | CSR13_SRL));
	    sleep(2);
	    csr12 = _21x4xReadCSR(device, CSR12);
	    if ((csr12 & CSR12_LKF) != 0) {
		printf("\tSwitching to AUI\n");
		_21x4xWriteCSR(device, CSR13, 0x00000000);
					    /* Stop SIA */
		_21x4xWriteCSR(device, CSR13, (CSR13_AUI | CSR13_CAC | CSR13_SRL));
	    }
	    break;
	case DECCHIP_21140_TYPE:
	    /* this device supports only one connection type */
	    _21x4xSetCSRBits(device, CSR6, CSR6_PS);/* select the MII/SYM port */
	    /* Reset the chip again because the MII/SYM value change needs a
	     * reset to take affect on the 21140.
	     */
	    DECchip_21x4x_Reset( device );
	    _21x4xSetCSRBits(device, CSR6, CSR6_PS);/* select the MII/SYM port */
	    break;
	case DECCHIP_21143_TYPE:
	  {
	    BOOLEAN LinkPass;
	    /* MII link autosence */

	    _21x4xSetCSRBits(device, CSR6, CSR6_PS|CSR6_PR|CSR6_HBD|CSR6_PCS|CSR6_SCR|CSR6_FD);
	    _21x4xWriteCSR(device, CSR13, 0 );
	    /* Full duplex mode auto negotiation */
	    _21x4xWriteCSR(device, CSR14, 0x07FFFF&(~(CSR14_TXH|CSR14_TH|CSR14_T4)) );
	    _21x4xWriteCSR(device, CSR13, CSR13_SRL );
	    LinkPass = DECchip_21x4x_WaitAutoNegotiationComplete( device );
	    if( !LinkPass )
	    {
		_21x4xClearCSRBits(device, CSR6, CSR6_FD );
		_21x4xWriteCSR(device, CSR13, 0 );
		/* all auto negotiations except Full Duplex */
		_21x4xWriteCSR(device, CSR14, 0x07FFFF&(~CSR14_TXF) );
		_21x4xWriteCSR(device, CSR13, CSR13_SRL );
		LinkPass = DECchip_21x4x_WaitAutoNegotiationComplete( device );
	    }
	    if( LinkPass )
	    {
		csr12 = _21x4xReadCSR(device, CSR12);
		/* for 10Base operation Port Select, PCS function, Scrambler,
		   and Heartbeat disable should be off! */
		if( (csr12&CSR12_LS10) == 0 )
		    _21x4xClearCSRBits(device, CSR6, CSR6_PS|CSR6_HBD|CSR6_PCS|CSR6_SCR );

		csr6 = _21x4xReadCSR(device, CSR6);
		printf("\t10%sBase-Tx%s\n", csr12&CSR12_LS100 ? "" : "0",
					    csr6&CSR6_FD ? "FD" : "" );
		break;
	    }
	  }
	    /* Clear bits set durring the above tests */
	    _21x4xClearCSRBits(device, CSR6, CSR6_HBD|CSR6_PCS|CSR6_SCR);
	    /* use the 21142 configuration test if this fails */
      /* fallthrough */
	case DECCHIP_21142_TYPE:
	  {
	    BOOLEAN Found = FALSE;

	    /* Basic Setup */

	    _21x4xSetCSRBits(device, CSR6, CSR6_PR );
	    printf("\tChecking ports");

	    if( !Found ) /* Check 10 base T port */
	    {
		_21x4xClearCSRBits(device, CSR6, CSR6_PS|CSR6_FD); /* Clear the MII/SYM and Full duplex */
		_21x4xWriteCSR(device, CSR13, 0 );
		_21x4xWriteCSR(device, CSR14, 0x0FFFF );
		_21x4xWriteCSR(device, CSR13, CSR13_SRL );
		printf(", 10BaseT");
		DECchip_21x4x_WaitAutoNegotiationComplete( device );
		csr12 = _21x4xReadCSR(device, CSR12);
		Found = !(csr12 & CSR12_LKF) && (csr12 & CSR12_TRA);
	    }

	    if( !Found ) /* Check 10 base 2 (BNC) port */
	    {
		_21x4xClearCSRBits(device, CSR6, CSR6_PS|CSR6_FD); /* Clear the MII/SYM and Full duplex */
		_21x4xWriteCSR(device, CSR13, 0 );
		_21x4xWriteCSR(device, CSR14, 0x0705 );
		_21x4xClearCSRBits(device, CSR15, CSR15_ABM);
		_21x4xWriteCSR(device, CSR13, CSR13_AUI|CSR13_SRL );
		printf(", 10Base2 (BNC)");
		sleep(1);
		csr12 = _21x4xReadCSR(device, CSR12);
		Found = !(csr12 & CSR12_LKF) && (csr12 & CSR12_ARA);
	    }

	    if( !Found ) /* defaulting to MII */
	    {
		_21x4xClearCSRBits(device, CSR6, CSR6_PS); /* Clear the MII/SYM port */
		_21x4xWriteCSR(device, CSR13, 0 );
		_21x4xWriteCSR(device, CSR14, 0 );
		_21x4xSetCSRBits(device, CSR6, CSR6_PS|CSR6_HBD); /* select the MII/SYM port */
		printf(", MII 10/100Base-Tx");
		sleep(1);
	    }
	    printf( "\n" );
	    break;
	  }
	}
    }
    /*
     *  If we need to, build and transmit a setup frame.  This
     *  frame will circulate as one of the transmit buffers in the
     *  system, so it is the same size as all the others.
     */
    if (device->flags.loopback)
	_21x4xWriteCSR(device, CSR6, 0x00002542);
					/* ST. RX., Promisc., Flacky dis.,**
					 * OM=int. LB, ST. TX.     */
    else {
	if (device->flags.setup_frame) {


/*
 *  We're building an perfect filter setup with two entries
 *  (the broadcast address and our hardware address).
 */
	    memset(device->setup_frame, 0x00, SETUP_FRAME_SIZE);
/*
 *  Put the broadcast address at the start of the setup frame
 *  This is done by filling the first 12 bytes with 0xFF (see the 21040 HRM)
 */
	    k = 0;
	    while(k < 12) device->setup_frame[k++] = (char) 0xFF;
/*
 *  Put in our physical address in the remaining entries (in byte order 103254).
 */
	    while (k < SETUP_FRAME_SIZE)
		for (i = 0; i < 6; i += 2) {
		    for (j = 0; j < 2; j++)
			device->setup_frame[k++] = 
			  device->hw_address[i + j];
		    k += 2;
		}

/*
 *  Now send it to the 21x4x.  Note that this transmit buffer
 *  will *not* get recycled as its address never gets put into
 *  current_sring_buffers.  However, we do keep the address of
 *  it in the device data structure so that we can use it when
 *  debugging.  Also note that we do not increment the tx index,
 *  this is because we wait for the frame to be returned to
 *  us before leaving this procedure.
 */
	    sring[device->index][device->tx_index].u_tdes1.s_tdes1.SET = TRUE;
	    sring[device->index][device->tx_index].u_tdes1.s_tdes1.TBS1 = 
	      SETUP_FRAME_SIZE;
	    sring[device->index][device->tx_index].u_tdes1.s_tdes1.LS = FALSE;
	    sring[device->index][device->tx_index].u_tdes1.s_tdes1.FS = FALSE;
/* 
 *  Perfect address filtering 
 */
	    sring[device->index][device->tx_index].u_tdes1.s_tdes1.HP = FALSE;
	    sring[device->index][device->tx_index].u_tdes1.s_tdes1.IV = FALSE;
	    sring[device->index][device->tx_index].tdes2 = 
	      (unsigned int) (
	      PCIMapAddress((unsigned char *) device->setup_frame));
	    mb();			/* make sure the rest is written before
					 * I change the owner */
	    sring[device->index][device->tx_index].u_tdes0.s_tdes0.OWN = TRUE;
	    mb();
	}
	/*
	 *  Finally, start the receive and transmit processes.
	 */
	csr6 = _21x4xReadCSR(device, CSR6);

	/* make sure that the transmit and recieve processes are stopped */

	if( csr6 & ( CSR6_ST | CSR6_SR ) )
	{
	    csr6 &= ~( CSR6_ST | CSR6_SR );
	    _21x4xWriteCSR(device, CSR6, csr6 );
	}

	csr6 = csr6 & (~CSR6_PB);	/* don't pass bad frames */

	if (device->flags.setup_frame) {
	    /*
	     *  We've sent a setup frame, now we must wait until it comes back 
	     *  before starting receive.
	     */
	    csr6 &= ~CSR6_PR;	/* don't run in promiscuous mode. */
	    _21x4xWriteCSR(device, CSR6, csr6 | CSR6_ST);
	    /*
	     *  Wait until the setup frame comes back.
	     */
	    while(sring[device->index][device->tx_index].u_tdes0.s_tdes0.OWN
	      == TRUE)
	      msleep(10);
#ifdef NEVER
	    printf("\tSetup frame returned\n");
#endif
	    /*
	     *  Fixup the descriptor.
	     */
	    sring[device->index][device->tx_index].u_tdes0.tdes0 = 0;
	    sring[device->index][device->tx_index].u_tdes1.tdes1 = 0;
	    sring[device->index][device->tx_index].u_tdes1.s_tdes1.TBS1 = ETHER_BUFFER_SIZE;
	    sring[device->index][device->tx_index].u_tdes1.s_tdes1.LS = TRUE;
	    sring[device->index][device->tx_index].u_tdes1.s_tdes1.FS = TRUE;
	    sring[device->index][device->tx_index].tdes2 = (unsigned int) 
	      PCIMapAddress((unsigned char *) sring[device->index][device->tx_index].tdes2);
	    /*
	     *  Bug Fix: increment the transmit index as the Tulip now
	     *  believes that it is on transmit descriptor 1 (*not* 0).
	     */
	    
	    device->tx_index++;
	    /*
	     *  Now start receive.
	     */
	    _21x4xWriteCSR(device, CSR6, csr6 | CSR6_ST | CSR6_SR);
	} else
	    _21x4xWriteCSR(device, CSR6, csr6 | CSR6_ST | CSR6_PR | CSR6_SR);
    }
    return TRUE;
}

/* DECchip_21x4x_device_start - start device
 *   Arguments:
 *      device_no - device number
 */
#ifdef NOT_IN_USE
static void DECchip_21x4x_device_start(int device_no)
{

    PRTRACE1("Entered DECchip_21x4x_device_start\n");
    printf("Digital Semiconductor 21x4x: DECchip_21x4x_device_start() called\n");

}
#endif

/* DECchip_21x4x_device_read - reads a frame from an Ethernet device
 *
 * Arguments:
 *
 *    device_no      - device number
 *    balance_buffer - spare buffer to maintain buffer balance
 *    frame_buffer   - returned buffer containing frame
 *
 * Returned value:
 *
 *    positive     - size of frame read; frame copied into buffer if <= size.
 *       0         - nothing to read
 *    negative     - error on read
 *
 * DESCRIPTION:
 *    This function reads a frame from the device if there is one ready to be 
 *    read.  The frame read is returned in frame_buffer.  To maintain the 
 *    driver's pool of buffers
 *    the caller must give the driver an unused buffer (balance_buffer) before
 *    the
 *    driver returns the frame.  Once this function has been called 
 *    balance_buffer must
 *    not be used by the calling software.
 *
 *    On return frame_buffer will always point to a valid buffer.  If the read 
 *    has failed
 *    (result <= 0) the contents of this buffer will not be valid.  Note that
 *    whether or not the read succeeds it is undefined whether the same buffer
 *    will be returned as
 *    frame_buffer as was passed down as balance_buffer.
 */

static int DECchip_21x4x_device_read(int device_no, 
  unsigned char balance_buffer[ETHER_BUFFER_SIZE], 
  unsigned char **frame_buffer)
{

    DECchip_21x4x_device_t *device = DECchip_21x4x_device_find(device_no);
    int count;
    unsigned int csr5;

#ifdef NEVER
    printf("DECchip 21x4x: DECchip_21x4x_device_read() called\n");
#endif
/* 
 *  Set up initial return value for error cases 
 */

    *frame_buffer = balance_buffer;
/*
 *  Valid device?
 */
    if (device == NULL) {
	printf("\n\nDigital Semiconductor 21x4x driver internal error\n");
	printf("Invalid device (%d)\n", device_no);
	return -1;
    }
/* 
 *  Check that the buffer address is valid 
 */
    if (!_21x4xValidBuffer(balance_buffer, ETHER_BUFFER_SIZE)) {
	printf("\n\nDigital Semiconductor %s driver internal error\n",
		DC21X4X_DID_Data[device->device_type].DeviceIDString );
	printf("Bad receive balance_buffer address (0x%08X)\n", 
	  balance_buffer);
	return -1;
    }

/*
 *  If the receive process has stopped or is suspended, then restart it.
 */
    csr5 = _21x4xReadCSR(device, CSR5);
    if (((csr5 & CSR5_RPS) != 0) || 
      ((csr5 & CSR5_RS) == CSR5_RS_SUSPENDED)) {
	_21x4xWriteCSR(device, CSR2, CSR2_RPD);
	device->counters->rx_restarted++;
    }
/*
 *  Keep going until we either get a good receive or the receive
 *  descriptor is owned by the 21x4x.
 */
    while (rring[device->index][device->rx_index].u_rdes0.s_rdes0.OWN == 
      FALSE) {
/*
 *  We've got a received buffer, check for errors reported by the
 *  device.  First check for the Error Summary bit if it is FALSE,
 *  all is good, else check further into the error.
 */
	if (rring[device->index][device->rx_index].u_rdes0.s_rdes0.ES) {
/*
 *  There was an error, so count it and return an error status.
 */
	    if (rring[device->index][device->rx_index].u_rdes0.s_rdes0.OF)
		device->counters->rx_err_OF++;
	    if (rring[device->index][device->rx_index].u_rdes0.s_rdes0.CE)
		device->counters->rx_err_CE++;
	    if (rring[device->index][device->rx_index].u_rdes0.s_rdes0.CS)
		device->counters->rx_err_CS++;
	    if (rring[device->index][device->rx_index].u_rdes0.s_rdes0.TL)
		device->counters->rx_err_TL++;
	    if (rring[device->index][device->rx_index].u_rdes0.s_rdes0.LE)
		device->counters->rx_err_LE++;
	    if (rring[device->index][device->rx_index].u_rdes0.s_rdes0.RF)
		device->counters->rx_err_RF++;
/*
 *  Clear the whole of the descriptor.
 */
	    rring[device->index][device->rx_index].u_rdes0.rdes0 = 0;
/*
 *  Set the count to -1 to indicate an error.
 */
	    count = -1;
	} else {
/*
 *  Received a buffer correctly, count it if its multicast.
 */

	    if (
	      rring[device->index][device->rx_index].u_rdes0.s_rdes0.MF)
		device->counters->mf_rx++;
	    count = 
	      rring[device->index][device->rx_index].u_rdes0.s_rdes0.FL - 
	      4;
					/* -4 deducts the CRC bytes */

	    PRTRACE3("21x4x: %d bytes received on device %d\n", count, 
	      device_no);
#ifdef NEVER
	    if (etherdump) {
		printf("\nDevice %d: Receive Buffer [%x]...\n", device_no, 
		  current_rring_buffers[device->index][device->rx_index]);
		ethernet_printpacket(current_rring_buffers[device->index][
		  device->rx_index], count + 4);
					/* Print CRC bytes too */
	    }
#endif
/*
 *  Increment counters.
 */
	    device->counters->p_rx++;
	    device->counters->b_rx += count;

/* 
 *  Check for errors.  For all errors, set count negative. If no errors,
 *  copy the frame to the caller buffer. 
 */
	    if (count > ETHER_BUFFER_SIZE)
		count = -1;
	    else {
		*frame_buffer = 
		  current_rring_buffers[device->index][device->rx_index];
					/* return pointer to buffer with the
					 * frame in it */
		current_rring_buffers[device->index][device->rx_index] = 
		  balance_buffer;
					/* swap in free buffer supplied */
		rring[device->index][device->rx_index].rdes2 = 
		  (unsigned int) balance_buffer & 0xFFFFFFFF;
					/* update ring with new pointer */
		rring[device->index][device->rx_index].rdes2 |= DMA_BUFF_BASE;
	    }
	}				/* ES not set in descriptor     */
/* 
 *  Give the buffer back to the 21x4x. Advance the ring index to the next
 *  buffer. 
 */
	rring[device->index][device->rx_index].u_rdes0.s_rdes0.OWN = TRUE;
	device->rx_index = (device->rx_index + 1) % RRING_SIZE;
/*
 *  If we have a valid count, then return to the caller.
 */
	if (count != -1)
	    return count;
    }					/* end of while descriptors are ours */
/*
 *  We didn't find anything to read (but we may have drained part if
 *  not all of the receive ring).  Return 0 to the caller.
 */
    return 0;
}
/* DECchip_21x4x_device_write - queue a frame to be sent on an ethernet device
 * Arguments:
 *     device_no         - device number    
 *     frame             - pointer to start of frame within the buffer    
 *     size              - size of frame    
 *     frame_buffer      - buffer containing frame    
 *     balance_buffer    - returned buffer to maintain buffer balance
 * Returned value:
 *    TRUE - successfully queued for sending    FALSE - unable to send
 * Description:
 *
 * This function writes a frame to the device  To maintain the caller's pool of
 * buffers    the driver must give the caller an unused buffer (balance_buffer)
 * in return for the buffer containing the frame to be transmitted.
 * If the send succeeds then frame_buffer must not be accessed by the caller
 * after this  call.  If the send can't be queued then frame_buffer will remain
 * valid and the returned value of balance_buffer is undefined and must not 
 * be used.
 */
static int DECchip_21x4x_device_write(int device_no, unsigned char *frame, 
  int size, unsigned char frame_buffer[ETHER_BUFFER_SIZE], 
  unsigned char **balance_buffer)
{
    DECchip_21x4x_device_t *device = DECchip_21x4x_device_find(device_no);
    unsigned int csr5;

#ifdef NEVER
    printf("Digital Semiconductor 21x4x: DECchip_21x4x_device_write() called\n");
    printf("csr5 = 0x%08X\n, tx index = %d\n", _21x4xReadCSR(device, CSR5), 
      device->tx_index);
#endif
/*
 * Valid device?
 */

    if (device == NULL) {
	printf("\n\nDigital Semiconductor 21x4x driver internal error\n");
	printf("Attempt to send on an invalid device (%d)\n", device_no);
	return FALSE;
    }
/* 
 * Check that the buffer address is valid 
 */

    if (!_21x4xValidBuffer(frame, ETHER_BUFFER_SIZE)) {
	printf("\n\nDigital Semiconductor %s driver internal error\n",
		DC21X4X_DID_Data[device->device_type].DeviceIDString );
	printf("Bad send frame address (0x%08X)\n", frame);
	return FALSE;
    }

    PRTRACE2("Sending ethernet frame from buffer %x", frame);
/* 
 * Exit if next buffer is owned by the 21x4x. Enforce minimum packet
 * length. Check the buffer for previous errors. Swap buffer into the lance
 * ring and associated buffer pool. Update send counters. 
 */
    if (sring[device->index][device->tx_index].u_tdes0.s_tdes0.OWN)
	return FALSE;			/* TX ring full - cannot send */

    if (size < MIN_ENET_PACKET)
	size = MIN_ENET_PACKET;		/* ensure minimum pkt size criteria met
					 */
    if (size > MAX_ENET_PACKET) {	/* flag an error if the pkt is too big
					 */
	return FALSE;
    }
/*
 *  Check that there wasn't an error the last time we sent using this 
 *  descriptor. 
 */
    if (sring[device->index][device->tx_index].u_tdes0.s_tdes0.ES) {
/*
 *  There was an error, so count it.  
 */
	if (sring[device->index][device->tx_index].u_tdes0.s_tdes0.UF)
	    device->counters->tx_err_UF++;
	if (sring[device->index][device->tx_index].u_tdes0.s_tdes0.EC)
	    device->counters->tx_err_EC++;
	if (sring[device->index][device->tx_index].u_tdes0.s_tdes0.LC)
	    device->counters->tx_err_LC++;
	if (sring[device->index][device->tx_index].u_tdes0.s_tdes0.NC)
	    device->counters->tx_err_NC++;
	if (sring[device->index][device->tx_index].u_tdes0.s_tdes0.LO)
	    device->counters->tx_err_LO++;
	if (sring[device->index][device->tx_index].u_tdes0.s_tdes0.TO)
	    device->counters->tx_err_TO++;
	if (sring[device->index][device->tx_index].u_tdes0.s_tdes0.LF)
	    device->counters->tx_err_LF++;
    }
/*
 *  Put the transmit buffer passed into the current set of transmit
 *  ring buffers and return as the balance buffer the one that the
 *  transmit buffer replaces (ie swap them).
 */
    *balance_buffer = current_sring_buffers[device->index][device->tx_index];
    current_sring_buffers[device->index][device->tx_index] = frame_buffer;
/*
 *  Now set up the transmit pointer in the descriptor.
 */
    sring[device->index][device->tx_index].tdes2 = 
      (unsigned int) frame & 0xffffffff;
    sring[device->index][device->tx_index].tdes2 |= DMA_BUFF_BASE;

/*
 *  Optionally dump the transmit buffer.
 */
    if (etherdump) {
	printf("\nTransmit Buffer [%x]...\n", 
	  sring[device->index][device->tx_index].tdes2);
	ethernet_printpacket((unsigned char *) sring[device->index][device->
	  tx_index].tdes2, size);
    }
/*
 *  Update counters.
 */
    device->counters->p_tx++;
    device->counters->b_tx += size;
/* 
 * Fill in fields in buffer descriptor and give buffer to the 21x4x.
 * (By changing the ownership bit).  The mb before the ownership
 * bit is changed is to make sure the other memory writes happen
 * and the descriptor is correctly set up before the ownship is
 * changed.
 */
    sring[device->index][device->tx_index].u_tdes1.tdes1 = 0;
    sring[device->index][device->tx_index].u_tdes1.s_tdes1.TBS1 = size;
    sring[device->index][device->tx_index].u_tdes1.s_tdes1.FS = TRUE;
    sring[device->index][device->tx_index].u_tdes1.s_tdes1.LS = TRUE;
    sring[device->index][device->tx_index].u_tdes1.s_tdes1.AC = FALSE;
    if (device->tx_index == (SRING_SIZE - 1))
	sring[device->index][device->tx_index].u_tdes1.s_tdes1.TER = TRUE;

    sring[device->index][device->tx_index].u_tdes0.tdes0 = 0;
    mb();

    sring[device->index][device->tx_index].u_tdes0.s_tdes0.OWN = TRUE;
/*
 *  Advance the transmit ring index.
 */
    device->tx_index = (device->tx_index + 1) % SRING_SIZE;
/* 
 *  If the transmit process has stopped or is suspended, then restart it.
 */
    csr5 = _21x4xReadCSR(device, CSR5);
    if (((csr5 & CSR5_TPS) != 0) || ((csr5 & CSR5_TS) == CSR5_TS_SUSPENDED)) {
#ifdef NEVER
	printf("21x4x: restarting transmit process\n");
#endif
	_21x4xWriteCSR(device, CSR1, CSR1_TPD);
	device->counters->tx_restarted++;
    }
    return TRUE;
}
/* DECchip_21x4x_device_flush - wait for all writes on an ethernet device to complete
 *
 * Argument:
 *
 *    device_no - device to be flushed
 *
 * Returned value:
 *    TRUE	    - flush successful
 *    FALSE	    - flush timeout failure
 */

static int DECchip_21x4x_device_flush(int device_no)
{
    return (TRUE);

}
/* DECchip_21x4x_device_print_stats - print device statistics
 *
 * Argument:
 *
 *     device_no - device number
 */
static void DECchip_21x4x_device_print_stats(int device_no)
{
    DECchip_21x4x_device_t *device = DECchip_21x4x_device_find(device_no);

/*
 *  First find the device data structure for this device.
 */

    if (device == NULL) {
	printf("ERROR: invalid device (%d)\n", device_no);
	return;
    }
/*
 *  Tell the world which device.
 */
    printf("\nDevice %d, PCI slot %d bus %d:\n\n", device_no, _PCI_Slot(device->pci),
           _PCI_Bus(device->pci));
/*
 *  Now dump the counters.
 */
    printf("\tTransmitted:\n");
    printf("\t\tPackets = %d\n", device->counters->p_tx);
    printf("\t\tBytes = %d\n", device->counters->b_tx);
    printf("\tTransmit Errors:\n");
    printf("\t\tUF = %d\n", device->counters->tx_err_UF);
    printf("\t\tEC = %d\n", device->counters->tx_err_EC);
    printf("\t\tLC = %d\n", device->counters->tx_err_LC);
    printf("\t\tNC = %d\n", device->counters->tx_err_NC);
    printf("\t\tLO = %d\n", device->counters->tx_err_LO);
    printf("\t\tTO = %d\n", device->counters->tx_err_TO);
    printf("\t\tLF = %d\n", device->counters->tx_err_LF);
    printf("\tTransmit process restarted %d times\n", 
      device->counters->tx_restarted);
    printf("\tReceived:\n");
    printf("\t\tPackets = %d (of which %d were multicast)\n", 
      device->counters->p_rx, device->counters->mf_rx);
    printf("\t\tBytes = %d\n", device->counters->b_rx);
    printf("\tReceive Errors:\n");
    printf("\t\tOF = %d\n", device->counters->rx_err_OF);
    printf("\t\tCE = %d\n", device->counters->rx_err_CE);
    printf("\t\tCS = %d\n", device->counters->rx_err_CS);
    printf("\t\tTL = %d\n", device->counters->rx_err_TL);
    printf("\t\tLE = %d\n", device->counters->rx_err_LE);
    printf("\t\tRF = %d\n", device->counters->rx_err_RF);
    printf("\tReceive process restarted %d times\n", 
      device->counters->rx_restarted);
    return;
}

/* DECchip_21x4x_device_preg - print all 21x4x registers
 *
 * Argument:
 *
 *     device_no - device number
 */

static void DECchip_21x4x_device_preg(int device_no)
{
    ul val;
    DECchip_21x4x_device_t *device = DECchip_21x4x_device_find(device_no);

/*
 *  First find the device data structure for this device.
 */

    if (device == NULL) {
	printf("ERROR: invalid device (%d)\n", device_no);
	return;
    }
/*
 *  Tell the world which device.
 */
    printf("\nDevice %d in PCI slot %d, bus %d:\n\n", device_no, 
           _PCI_Slot(device->pci), _PCI_Bus(device->pci));

/*
 *  Not strictly kosher, but print out something about this device.
 */
    printf("\tDevice data structure is at 0x%08x\n", device);
    printf("\tSetup frame is at 0x%08x\n", device->setup_frame);
    if (!device->flags.setup_frame)
	printf("\tRunning in promiscous mode\n");
    if (device->flags.loopback)
	printf("\tLoopback mode.\n");
    printf("\tFirst transmit descriptor (of %d) is at 0x%08x\n", SRING_SIZE, 
      &sring[device->index][0].u_tdes0.tdes0);
    printf("\tTransmit index is %d\n", device->tx_index);
    printf("\tFirst receive descriptor (of %d) is at 0x%08x\n", RRING_SIZE, 
      &rring[device->index][0].u_rdes0.rdes0);
    printf("\tReceive index is %d\n", device->rx_index);
    printf("\tHardware Address is ");
    ethernet_printaddress(device->hw_address);
    printf("\n");
/*
 *  Then, print out the PCI I/O configuration space
 *  for this device.
 */
    printf("\nPCI Configuration Registers\n\n");

#define DISPLAY_PCIREG( REG )						\
    val = incfgl(_PCI_Bus(device->pci), _PCI_Slot(device->pci), 0, REG);\
    printf("\t%s (%04xh) = %08xh\n", #REG, REG, val);

    DISPLAY_PCIREG( CFID )
    DISPLAY_PCIREG( CFCS )
    DISPLAY_PCIREG( CFRV )
    DISPLAY_PCIREG( CFLT )
    DISPLAY_PCIREG( CBIO )
    DISPLAY_PCIREG( CBMA )
    DISPLAY_PCIREG( CFDD )

#undef DISPLAY_PCIREG

/*
 *  Now print out the CSRs in PCI I/O space.
 */
    printf("\nConfiguration and Status Registers (PCI I/O)\n\n");

#define DISPLAY_PCIREG( REG )				\
    val = _21x4xReadCSR(device, REG);			\
    printf("\t%s (%04xh) = %08xh\n", #REG, REG, val);

    DISPLAY_PCIREG( CSR0 )
    DISPLAY_PCIREG( CSR1 )
    DISPLAY_PCIREG( CSR2 )
    DISPLAY_PCIREG( CSR3 )
    DISPLAY_PCIREG( CSR4 )
    DISPLAY_PCIREG( CSR5 )
    DISPLAY_PCIREG( CSR6 )
    DISPLAY_PCIREG( CSR7 )
    DISPLAY_PCIREG( CSR8 )
    DISPLAY_PCIREG( CSR9 )
    DISPLAY_PCIREG( CSR10 )
    DISPLAY_PCIREG( CSR11 )
    DISPLAY_PCIREG( CSR12 )
    DISPLAY_PCIREG( CSR13 )
    DISPLAY_PCIREG( CSR14 )
    DISPLAY_PCIREG( CSR15 )

#undef DISPLAY_PCIREG

    return;

}

/* DECchip_21x4x_device_clear_interrupts - clear all interrupts from an ethernet device
 *
 * Argument:
 *
 *     device_no - device number,
 */

static void DECchip_21x4x_device_clear_interrupts(int device_no)
{
    DECchip_21x4x_device_t *device = DECchip_21x4x_device_find(device_no);

#ifdef NEVER
    printf("Digital Semiconductor 21x4x: DECchip_21x4x_device_clear_interrupts() called\n");
#endif

/*
 *  Clear any interrupts there may be.
 */
    _21x4xWriteCSR(device, CSR5, _21x4xReadCSR(device, CSR5));

    return;
}
/* DECchip_21040_device_register - register a 21x4x device driver in the 
 * Ethernet device table
 *
 * Arguments:
 *
 *    device_no - device number for registration
 *
 * Returned value
 *    TRUE         - registered successfully
 *    FALSE        - registration failed
 *
 * Description
 *
 *    Add a DECchip 21x4x device to the installed driver list as follows:
 *    Check not all drivers (DECCHIP21x4x_MAX_DEVICES) in use
 *    Initialise the appropriate current_buffers pointer array
 *    Set the appropriate driver_in_use flag
 *    Fill device number table with DECchip 21x4x function pointers
 *		
 */
char DECChip21x4x_description[] = "Digital Semiconductor %s";
int DECchip_21040_device_register(int device_no)
{
  DECchip_21x4x_device_t *device = devices;
  int devices_registered = 0;
  char * DescriptionBuffer;

#ifdef NEVER
  printf("Digital Semiconductor 21x4x: DECchip_21040_device_register() called\n");
#endif

  for( ;
       devices_registered < DECCHIP21x4x_MAX_DEVICES &&
       device->flags.exists;
       devices_registered++, device++, device_no++ )
  {
    /*
     *  The DBM will only support so many ethernet devices, so check.
     */
    if (device_no >= MAX_ETHER_DEVICES)
    {
      printf("ERROR: Invalid device number %d\n", device_no);
      break;
    }

    device->flags.allocated = TRUE;
    /*
     * Now that this device has been registered, give it a system wide
     * device number.
     */

    device->device = device_no;
    switch( device->device_type )
    {
    case DECCHIP_21142_TYPE:
      /* check for the valid revisions of the DC21142 and if the number does
       * not match then this must be a 21143 based device
       */
      {
	int tmp = incfgl(_PCI_Bus(device->pci), _PCI_Slot(device->pci), 0, CFRV);
	tmp &= 0xff;
	if( tmp == 0x11 || tmp == 0x10 )
	  break;
      }
      device->device_type = DECCHIP_21143_TYPE;
    }

    PRTRACE3("DC21X4X PCI DeviceID=%x, type %s\n",
	     DC21X4X_DID_Data[device->device_type].DeviceID,
	     DC21X4X_DID_Data[device->device_type].DeviceIDString );
    /*
     * Register the device with the device type independent device
     * register 
     */
    DescriptionBuffer = net_buffer_alloc( sizeof(DECChip21x4x_description) + 10 );
    sprintf( DescriptionBuffer, DECChip21x4x_description, DC21X4X_DID_Data[device->device_type].DeviceIDString );
    ether_device_register(device_no,
			  DECchip_21x4x_device_init, 
			  DECchip_21x4x_device_read,
			  DECchip_21x4x_device_write, 
			  DECchip_21x4x_device_flush,
			  DECchip_21x4x_device_print_stats, 
			  DECchip_21x4x_device_preg, 
			  DECchip_21x4x_device_clear_interrupts, 
			  device->device_type == DECCHIP_21040_TYPE ?
			    DECchip_21040_device_get_hw_address :
			    DECchip_21x4x_device_get_hw_address,
			  DECchip_21x4x_device_stop,
			  DescriptionBuffer);
    sprintf(MessageString, "Digital Semiconductor %s driver loaded:",
	    DC21X4X_DID_Data[device->device_type].DeviceIDString);
    printf("%-50s Ethernet Device: %d\n", MessageString, device_no);
    printf("\tPCI Slot: %d, Bus: %d\n", _PCI_Slot(device->pci), _PCI_Bus(device->pci));
  }
  return(devices_registered);
}


/*
 * DECchip_21x4x_device_check_romid - check that a rom id is valid.
 *
 * Arguments:
 *    ROM ID - 32 bytes as follows:
 *  --------------------------------------------------
 *  bytes   contents
 *  --------------------------------------------------
 *  0-5     Address octets  0-5
 *  6-7     Checksum octets 1-2
 *  8-9     Checksum octets 2-1
 *  10-15   Address octets  5-0
 *  16-21   Address octets  0-5
 *  22-23   Checksum octets 1-2
 *  24-31   test pattern (ff-00-55-aa-ff-00-55-aa)
 *  --------------------------------------------------
 *
 * return:
 *    true if successful, false otherwise.
 */
#define ROM_SIZE 32
#define ROM_TEST_OFFSET	 24
#define ROM_TEST_SIZE 8
#define ROM_CHECKSUM_OFFSET 6
#define ROM_MAC_SIZE 6
static int DECchip_21x4x_device_check_romid(unsigned char *romid)
{
  unsigned char test_pattern[ROM_TEST_SIZE] = {0xff, 0x00, 0x55, 0xaa, 
						 0xff, 0x00, 0x55, 0xaa};
  unsigned int i;
/*
 *  first look at the test pattern.
 */

    for (i = 0; i < ROM_TEST_SIZE; i++)
	if (test_pattern[i] != romid[ROM_TEST_OFFSET + i]) {
#ifdef NEVER
	    printf("Digital Semiconductor21x4x: ERROR, invalid ROM ID test pattern\n");
#endif
	    return FALSE;
	}

#if 0    /* was NEVER but this test does not work! */
/*
 *  Now perform the checksum.
 */
    {
    unsigned char g, h;
    unsigned int k;

    	k = 0;
    	for (i = 0; i < 3; i++) {
		k = k * 2;
		if (k >= 0xFFFF)
	     		k = k - 0xFFFF;
		k = k + romid[2 * i] + romid[(2 * i) + 1];
		if (k >= 0xFFFF)
	     	k = k - 0xFFFF;
    	}
    	if (k == 0xFFFF)
		k = 0;
    	g = k / 256;
    	h = k % 256;
/*
 *    g should now equal checksum 1 and h should
 *    be equal to checksum 2.
 */
    	if ((romid[ROM_CHECKSUM_OFFSET] != g) || 
      		(romid[ROM_CHECKSUM_OFFSET + 1] != h)) {
#ifdef NEVER
		printf("Digital Semiconductor21x4x: ERROR, invalid ROM ID checksum\n");
		printf("\tg = 0x%X, h = 0x%X\n", g, h);
#endif
		return FALSE;
    	}
    }
#endif
/*
 *  We passed all of the tests, so return TRUE.
 */
    return TRUE;
}
static int DECchip_21x4x_device_read_romid(mac_addr hw_address)
{
    unsigned char romid[ROM_SIZE];
    ui i;
/*
 *  Select the Ethernet rom.
 */

    outportb(PAGE_REGISTER, SELECT_ROMID);
/*
 *  Now read all the bytes of the rom id
 */
    for (i = 0; i < ROM_SIZE; i++)
	romid[i] = (unsigned char)inportb(ROMID_BASE + i);
/*
 *  If the value starts with 0xff, then we cannot read
 *  the real ROM id, so return the default value.
 */
    if (romid[0] == 0xff) {
#ifdef NEVER
	printf("Digital Semiconductor21x4x: failed to read ROM ID\n");
#endif
    } else {
	if (DECchip_21x4x_device_check_romid(romid)) {
	    memcpy(hw_address, romid, ROM_MAC_SIZE);
	    return TRUE;
	} else
#ifdef NEVER
	    printf("Digital Semiconductor21x4x: ROM ID checksum failed\n");
#else
	    ;
#endif
    }
    return FALSE;
}

/* DECchip_21040_device_get_hw_address - get the hardware address of a device 
 *
 * Arguments:
 *    device_no - device number for which hardware address is required
 *    hw_address - returned hardware mac address 
 */
static void DECchip_21040_device_get_hw_address(int device_no, 
  mac_addr hw_address)
{
    DECchip_21x4x_device_t *device = DECchip_21x4x_device_find(device_no);
    int i;
    unsigned int csr9;
    unsigned char romid[ROM_SIZE];

/*
 *  First find the device data structure for this device.
 */

    if (device == NULL) {
	printf("ERROR: invalid device (%d)\n", device_no);
	return;
    }

/*
 *  If the device id has been read already it has been
 *  safely tucked away in the network device's data structure.
 *  Don't incur the overhead again of reading it from the ROM.
 */
    if (DECchip_21x4x_device_hw_address_valid(device->hw_address)) {
      memcpy(hw_address, device->hw_address, ROM_MAC_SIZE);
      return;
    }

/*
 *  First try to read the SROM ID via CSR 9.  If this fails
 *  then try the ROM ID.
 */
    _21x4xWriteCSR(device, CSR9, 0xFFFFFFFF);
    csr9 = _21x4xReadCSR(device, CSR9);
    if (csr9 == 0xFFFFFFFF) {
/*
 *  We failed to read the srom id; this may be an early rev
 *  or it may be the embedded tulip on board.  Now try and
 *  read the rom id.
 */
	if (DECchip_21x4x_device_read_romid(hw_address))
	    return;
    } else {
/*
 *  The SROM id is valid, so read it.  Each time we read the
 *  CSR we get another byte of address.  We've already read
 *  it once.
 */
	for (i = 0; i < ROM_SIZE; i++) {
	    while (csr9 & CSR9_DN) {
		csr9 = _21x4xReadCSR(device, CSR9);
	    }
	    romid[i] = csr9 & CSR9_DT;
	    csr9 = _21x4xReadCSR(device, CSR9);
	}

/*
 *  Is the SROM ID itself valid?
 */
	if (DECchip_21x4x_device_check_romid(romid)) {
	    memcpy(hw_address, romid, ROM_MAC_SIZE);
	    return;
	}

    }					/* end of srom id is valid */

#ifdef NEED_ETHERNET_ADDRESS
/*
 *  We failed to read the rom id, or read it and didn't like
 *  it, so tell the world and default its value.
 */
#ifdef NEVER
    printf("Digital Semiconductor21x4x (%d): defaulting ROM ID\n", 
      device_no);
#endif
    memcpy(hw_address, eaddr, sizeof(eaddr));
    return;
#endif /* NEED_ETHERNET_ADDRESS */
}

/* DECchip_21x4x_device_stop - Stop the device (preempts continued DMA activitity)
 *
 * Argument:
 *
 *     device_no - device number
 */

static void DECchip_21x4x_device_stop(int device_no)
{
    DECchip_21x4x_device_t *device = DECchip_21x4x_device_find(device_no);

/*
 *  First find the device data structure for this device.
 */

    if (device == NULL) {
	printf("ERROR: invalid device (%d)\n", device_no);
	return;
    }
/*
 *  Tell the world which device.
 */
    printf("Stopping network device %d in PCI slot %d, bus %d:\n",
	   device_no, _PCI_Slot(device->pci), _PCI_Bus(device->pci));
    
    _21x4xWriteCSR(device, CSR0, _21x4xReadCSR(device, CSR0) | 1 );
    return;
}

/*
 *  Allocate (and check) all the buffers required by this device.
 */
static void DECchip_21x4x_device_allocate_buffers(
  DECchip_21x4x_device_t *device)
{
    unsigned int index = device->index;
    unsigned int invalid_address = FALSE, i;

/*
 *  Allocate receive and transmit descriptors and 
 *  buffers for this device.
 */

    rring[index] = net_buffer_alloc(
      sizeof(DECCHIP_21x4x_RX_BD) * RRING_SIZE);
    if (!_21x4xValidBuffer(rring[index], 
      sizeof(DECCHIP_21x4x_RX_BD) * RRING_SIZE))
	invalid_address = TRUE;
    sring[index] = net_buffer_alloc(
      sizeof(DECCHIP_21x4x_TX_BD) * SRING_SIZE);
    if (!_21x4xValidBuffer(sring[index], 
      sizeof(DECCHIP_21x4x_TX_BD) * SRING_SIZE))
	invalid_address = TRUE;
    for (i = 0; i < RRING_SIZE; i++) {
	current_rring_buffers[index][i] = 
	  net_buffer_alloc(ETHER_BUFFER_SIZE);
	if (!_21x4xValidBuffer(current_rring_buffers[index][i], 
	  ETHER_BUFFER_SIZE))
	    invalid_address = TRUE;
    }
    for (i = 0; i < SRING_SIZE; i++) {
	current_sring_buffers[index][i] = 
	  net_buffer_alloc(ETHER_BUFFER_SIZE);
	if (!_21x4xValidBuffer(current_sring_buffers[index][i], 
	  ETHER_BUFFER_SIZE))
	    invalid_address = TRUE;
    }
    device->setup_frame = net_buffer_alloc(SETUP_FRAME_SIZE);
    if (!_21x4xValidBuffer(device->setup_frame, 
      SETUP_FRAME_SIZE))
	invalid_address = TRUE;

    if (invalid_address == TRUE) {
	printf("Digital Semiconductor %s (slot = %d): error one or more allocated\n",
	  DC21X4X_DID_Data[device->device_type].DeviceIDString,
	  _PCI_Slot(device->pci));
	printf("\tbuffers do not map into any PCI to system address\n");
	printf("\twindow, choose another value for EBUFF\n");
    }
}
/* 
 * DECchip_21040_device_init_module - initialise the module 
 * NOTE: This routine can be called after an ebuff command to reset
 * the ethernet buffer base.  
 */
void DECchip_21040_device_init_module(void) 
{
  unsigned int index;
  DECchip_21x4x_device_t *device = devices;
  PCIDevice_t *pci;
  int ID_Idx;

#ifdef NEVER
  printf("DECchip 21x4x: DECchip_21040_device_init_module() called\n");
#endif
/*
 * Set all of the devices to !exist until we find them.
 */

  for (index = 0; index < DECCHIP21x4x_MAX_DEVICES; index++)
      devices[index].flags.exists = FALSE;

/*
 *  Look for all of the 21X4Xs in the system.
 */

  for( index = ID_Idx = 0;
       DC21X4X_DID_Data[ID_Idx].DeviceID != 0;
       ID_Idx++ )
  {
    /*  We are not doing chip specific initialization here so we can skip
     *  devices in the list that have duplicate device IDs
     */
    if( ID_Idx > 0 &&
	DC21X4X_DID_Data[ID_Idx].DeviceID == DC21X4X_DID_Data[ID_Idx-1].DeviceID )
      continue;
    for( pci = PCIDeviceFind((uw)DIGITAL_VID, (uw)DC21X4X_DID_Data[ID_Idx].DeviceID);
	 pci != NULL;
	 pci = PCIDeviceFindNext(pci) )
    {
      if( index < DECCHIP21x4x_MAX_DEVICES )
      {
	/* This slot contains a DECchip 21x4x chip.  
	 */
	PRTRACE3("Digital Semiconductor %s discovered in PCI slot %d\n", 
		 DC21X4X_DID_Data[ID_Idx].DeviceIDString, _PCI_Slot(pci));
	PRTRACE2("\tindex is %d\n", index);
	device->flags.exists = TRUE;
	device->flags.allocated = FALSE;
	device->flags.initialised = FALSE;
	device->flags.setup_frame = SETUP_FRAME_NEEDED;
	device->flags.hw_valid = FALSE;
	device->index = index;
	device->pci = pci;
	device->PCI_IO_Base = pci->PCI_Bar[0].Base;
        device->device_type = ID_Idx;
	/*
	 *  Allocate somewhere to keep counters.
	 */
	device->counters = net_buffer_alloc(sizeof(counters_t));
	memset(device->counters, 0, sizeof(counters_t));
	/*
	 *  Now allocate receive and transmit descriptors and 
	 *  buffers for this device.
	 */
	DECchip_21x4x_device_allocate_buffers(device);
	/*
	 *  Finally, increment the pointers and index.
	 */
	index++;
	device++;
      }
      else
	printf("ERROR: To many DC21x4x ethernet devices\n");

    }
  }
}


static int DECchip_21x4x_device_hw_address_valid(mac_addr hw_address)
{
  int i, j;
  j = 0;
  for (i = 0; i < MAC_ADDRESS_SIZE; i++) j = j | hw_address[i];
  if (j == 0)
	return FALSE;
  return TRUE;
}

/*
** DC21041 Serial/Ethernet Address ROM (DE4X5_SROM)
*/
#define SROM_MODE 0x00008000       /* MODE_1: 0,  MODE_0: 1  (read only) */
#define SROM_RD   0x00004000       /* Read from Boot ROM */
#define SROM_WR   0x00002000       /* Write to Boot ROM */
#define SROM_BR   0x00001000       /* Select Boot ROM when set */
#define SROM_SR   0x00000800       /* Select Serial ROM when set */
#define SROM_REG  0x00000400       /* External Register Select */
#define SROM_DT   0x000000ff       /* Data Byte */

#define DT_OUT    0x00000008       /* Serial Data Out */
#define DT_IN     0x00000004       /* Serial Data In */
#define DT_CLK    0x00000002       /* Serial ROM Clock */
#define DT_CS     0x00000001       /* Serial ROM Chip Select */

/* DECchip_21x4x_device_get_hw_address - get the hardware address of a device 
 *
 * Arguments:
 *    device_no - device number for which hardware address is required
 *    hw_address - returned hardware mac address 
 */
static void DECchip_21x4x_device_get_hw_address(int device_no, 
  mac_addr hw_address)
{
  int i, j;
  DECchip_21x4x_device_t *device;
  long SromData;
  short SromWord = 0;
  uw addr;

/*
** SROM Structure
*/
  struct de4x5_srom {
    char reserved[18];
    char version;
    char num_adapters;
    char hw_addr[6];
    char info[100];
    short chksum;
  } Srom;

  short *SromP;
  device = DECchip_21x4x_device_find(device_no);
/*
 *  First find the device data structure for this device.
 */

  if (device == NULL) {
    printf("ERROR: invalid device (%d)\n", device_no);
    return;
  }

/*
 *  If the device id has been read already it has been
 *  safely tucked away in the network device's data structure.
 *  Don't incur the overhead again of reading it from the ROM.
 */
  if (DECchip_21x4x_device_hw_address_valid(device->hw_address)) {
    memcpy(hw_address, device->hw_address, ROM_MAC_SIZE);
    return;
  }

  SromP = (short *)&Srom;
  for (i=0; i<(sizeof(struct de4x5_srom)/sizeof(short)); i++) {
    addr = device->PCI_IO_Base + CSR9;
    sendto_srom(SROM_RD|SROM_SR, addr);
    srom_latch(SROM_RD|SROM_SR|DT_CS, addr);
    srom_command(SROM_RD|SROM_SR|DT_IN|DT_CS, addr);
    srom_address(SROM_RD|SROM_SR|DT_CS, addr, i);

    for (j=0; j<16; j++) {
      sendto_srom(SROM_RD|SROM_SR|DT_CS |DT_CLK, addr);
      SromData = getfrom_srom(addr);
      sendto_srom(SROM_RD|SROM_SR|DT_CS, addr);

      SromWord = (SromWord << 1) | ((SromData >> 3) & 0x01);
    }

    sendto_srom(SROM_RD|SROM_SR|DT_CS & 0x0000ff00, addr);

    *SromP++ = SromWord;
  }
  memcpy(hw_address, Srom.hw_addr, ROM_MAC_SIZE);
}


static void srom_latch(ui command, uw addr)
{
  sendto_srom(command, addr);
  sendto_srom(command | DT_CLK, addr);
  sendto_srom(command, addr);

  return;
}

static void srom_command(ui command, uw addr)
{
  srom_latch(command, addr);
  srom_latch(command, addr);
  srom_latch((command & 0x0000ff00) | DT_CS, addr);

  return;
}

static void srom_address(ui command, uw addr, int offset)
{
  long i;
  char a;

  a = (char)(offset << 2);
  for (i=0; i<6; i++, a <<= 1) {
    srom_latch(command | ((a < 0) ? DT_IN : 0), addr);
  }
  usleep(1);

  i = (getfrom_srom(addr) >> 3) & 0x01;
  if (i != 0) {
    printf("Bad SROM address phase.....\n");
  }

  return;
}

static void sendto_srom(ui command, uw addr)
{
  outportl(addr, command);
  usleep(1);

  return;
}

static ui getfrom_srom(uw addr)
{
  ui tmp;

  tmp = inportl(addr);
  usleep(1);

  return tmp;
}


#endif /* NEED21040 */
