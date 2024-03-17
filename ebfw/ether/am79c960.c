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
static char *rcsid = "$Id: am79c960.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif

/*
 * MODULE DESCRIPTION:
 *
 *     AM79C960 Ethernet device interface for EB64 monitor
 *
 * HISTORY:
 *
 * DB       05-Jul-1993     initial release
 *
 * $Log: am79c960.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.37  1997/08/12  04:57:30  fdh
 * Print watchdog timer delay in milliseconds.
 *
 * Revision 1.36  1997/05/14  16:08:41  fdh
 * Corrected problem where device was used withouth the
 * proper ethernetbase address.
 *
 * Revision 1.35  1996/11/16  08:15:56  fdh
 * Modified startup messages.
 *
 * Revision 1.34  1996/08/19  20:22:20  fdh
 * Correction for accessing hw ethernet address.
 *
 * Revision 1.33  1996/08/19  16:24:25  fdh
 * Print the ethernet address after init.
 *
 * Revision 1.32  1996/08/06  22:08:24  fdh
 * Added mode parameter to call to EnableIsaDmaChannel()
 *
 * Revision 1.31  1996/06/13  03:33:19  fdh
 * Removed the isa_ether_device_map definition.
 *
 * Revision 1.30  1996/06/03  14:48:21  fdh
 * Modified the device startup banner.
 *
 * Revision 1.29  1996/05/26  19:53:36  fdh
 * Modified ISA definitions.
 *
 * Revision 1.28  1996/05/22  21:43:56  fdh
 * Modified to autodetect the base addresses of devices.
 * Only register driver if a device is found.
 *
 * Revision 1.27  1995/11/01  16:19:15  cruz
 * *** empty log message ***
 *
 * Revision 1.26  1995/10/30  16:53:51  cruz
 * Removed declaration of PutDotChar which is not used here.
 *
 * Revision 1.25  1995/10/27  15:15:55  cruz
 * Removed extern declaration of etherdump and etherprom since they
 * are now included in ether.h
 *
 * Revision 1.24  1995/10/23  15:21:25  fdh
 * Condensed startup messages to one line.
 *
 * Revision 1.23  1995/10/20  18:39:05  cruz
 * Performed some clean up.  Updated copyright headers.
 * Fixed bug in stop_device function which used an
 * uninitialized variable instead of 0.
 *
 * Revision 1.22  1995/10/18  18:10:35  fdh
 * Added am79c960_device_stop().
 *
 * Revision 1.21  1995/09/02  06:54:14  fdh
 * Removed declaration for WaitUs().  Already defined as a macro
 * in another file.
 *
 * Revision 1.20  1995/09/02  06:21:22  fdh
 * Included time.h
 *
 * Revision 1.19  1994/12/12  22:02:44  cruz
 * Added an mb() after setting the ownership bit in the read routine
 *
 * Revision 1.18  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.17  1994/07/21  14:03:45  fdh
 * Print out IRQ as decimal value at startup.
 *
 * Revision 1.16  1994/07/16  16:54:01  fdh
 * Modified device startup messages...
 *
 * Revision 1.15  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.14  1994/06/24  15:26:32  rusling
 * Fixed initialization alignment problems.
 *
 * Revision 1.13  1994/06/22  15:11:46  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.12  1994/06/20  14:42:15  fdh
 * Fixup preprocessor #include conditionals.
 *
 * Revision 1.11  1994/06/17  19:36:25  fdh
 * Clean-up...
 *
 * Revision 1.10  1994/06/03  20:09:34  fdh
 * Removed unnecessary include file and declaration.
 *
 * Revision 1.9  1994/01/27  20:14:45  fdh
 * Print more info when driver is registered.
 *
 * Revision 1.8  1994/01/19  10:55:52  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.7  1993/11/30  10:14:17  rusling
 * Modified to use net_buff.h and net_buffer_allocate().
 *
 * Revision 1.6  1993/11/22  15:13:24  rusling
 * Re-merged with baselevel.
 *
 * Revision 1.4  1993/11/22  13:18:42  rusling
 * Fixed initialisation bug so that ebuff commands do
 * not stop you registering devices.  Plus do not make
 * this module if you do not want it.
 *
 * Revision 1.3  1993/11/02  22:01:59  fdh
 * Condense device loaded message.
 *
 * Revision 1.2  1993/10/16  00:14:37  fdh
 * correct assignment of active_base[device_index]
 *
 * Revision 1.1  1993/08/06  09:44:01  berent
 * Initial revision
 *
 *
 */


#include "ether.h"

#ifdef NEEDAM79C960
#include "edriver.h"
#include "isa_bus.h"
#include "am79c960.h"

#define RRING_SIZE 4
#define LN2_RRING_SIZE 2      /* coded version for RLEN field of init block */
#define SRING_SIZE 2
#define LN2_SRING_SIZE 1      /* coded version for TLEN field of init block */

/* MODULE DESCRIPTION:
 *
 * This file contains Am79C960 specific routines needed for an Ethernet
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
 *              maximum number of AM79C960 must be declared at compile time
 *	        using AM79C960_MAX_DEVICES. IO base addresses are also fixed
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
 *                etherprom - enables promiscuous mode when TRUE
 */

/* AM79C960_MAX_DEVICES copies of resources are statically assigned at compile time.  */

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
 *        current_rring_buffers[][] and rbuf_idx for RX
 *        current_sring_buffers[][] and sbuf_idx for TX (send)
 */

/* 
 *  Information used by registration and indexing processes 
 */

static AM79C960_CONFIG  AM79C960_base[AM79C960_MAX_DEVICES];

static int active_base[AM79C960_MAX_DEVICES]; /* Flags for active base devices */

static int ethernetbase;	/* value of the contents of AM79C960_base[x]->base_addr */


        /* Buffer pointers and indexes */

static int rbuf_idx[AM79C960_MAX_DEVICES];
static int sbuf_idx[AM79C960_MAX_DEVICES];

	/* pointers to a device are initialised when the device is registered */

static unsigned char* current_rring_buffers[AM79C960_MAX_DEVICES][RRING_SIZE];
static unsigned char* current_sring_buffers[AM79C960_MAX_DEVICES][SRING_SIZE];

	/* allocate all buffers potentially required */

static ub * init_block[AM79C960_MAX_DEVICES];        	/* pointer to init block memory */
static LANCE_RECV_BD    * rring[AM79C960_MAX_DEVICES]; 	/* pointer to RX ring descriptor memory for all potential devices */
static LANCE_SEND_BD    * sring[AM79C960_MAX_DEVICES]; 	/* pointer to TX ring descriptor memory for all potential devices */

        /* Lance counters */

static LANCE_CTRS am79c960_counters[AM79C960_MAX_DEVICES];        /* declare array of network counter structures */


        /* Externally declared functions */

extern void           intr_enable(int int_level);

        /* Functions declared within */

static void WriteRReg(ui reg, ui val);
static ui  ReadRReg(ui reg);
static void WriteIReg(ui reg, ui val);
static ui  ReadIReg(ui reg);
static int  am79c960_device_init(int device_no);
static void am79c960_device_start(int device_no);
static int  am79c960_device_read(int device_no,
				 unsigned char balance_buffer[ETHER_BUFFER_SIZE],
				 unsigned char ** frame_buffer);
static int am79c960_device_write(int device_no,
				 unsigned char *frame,
				 int size,
				 unsigned char frame_buffer[ETHER_BUFFER_SIZE],
				 unsigned char ** balance_buffer);
static int  chk_recv_err(int device_index, volatile LANCE_RECV_BD* rb);
static int  chk_send_err(int device_index, volatile LANCE_SEND_BD* tb);
static int  am79c960_device_flush(int device_no);
static void am79c960_device_print_stats(int device_no);
static void am79c960_device_clear_interrupts(int device_no);
static void am79c960_device_get_hw_address(int device_no, mac_addr hw_address);
static void am79c960_device_stop(int device_no);
static int  am79c960_device_get_index(int device_no);
static void am79c960_device_preg(int device_no);


/* Functions to Read and write 79C960 CSR's */


        /* Write a word to an RDP accessed CSR */

static void
WriteRReg(ui reg, ui val) {
	outportw(ethernetbase + RAP_OFFSET, reg);
	outportw(ethernetbase + RDP_OFFSET, val & 0xffff);
}

        /* Read a word from an RDP accessed CSR */

static ui
ReadRReg(ui reg) {
    outportw(ethernetbase + RAP_OFFSET, reg);
    return(inportw(ethernetbase + RDP_OFFSET) & 0xffff);
}

        /* Write a word to an IDP accessed CSR */

static void
WriteIReg(ui reg, ui val) {
    outportw(ethernetbase + RAP_OFFSET, reg);
    outportw(ethernetbase + IDP_OFFSET, val & 0xffff);
}

        /* Read a word from an IDP accessed CSR */

static ui
ReadIReg(ui reg) {
    outportw(ethernetbase + RAP_OFFSET, reg);
    return(inportw(ethernetbase + IDP_OFFSET) & 0xffff);
}

        /* Initialise the Ethernet environment */


/* am79c960_device_get_index - check registered device no has a valid base address, 
 *                             then evaluate index for required instantiation 
 *
 * Arguments:
 *
 *    device_no    - device number
 *
 * Returned value
 *    >= 0 - device_index
 *    -1   - no such device
 */
static int
am79c960_device_get_index(int device_no) {

    int i;
    int j = isa_ether_device_map[device_no];

    for (i=0; i < AM79C960_MAX_DEVICES; i++) {
	if (j == AM79C960_base[i].base_addr) {
	    ethernetbase = j;
	    return i;
	}
    } 
    return -1;		/* reaching this point implies an invalid base address */
}



/* am79c960_device_init - initialises an ethernet device
 *
 * Arguments:
 *
 *    device_no    - device number
 *
 * Returned value
 *    TRUE         - initialised succesfully
 *    FALSE        - error on initialisation
 */

static int  
am79c960_device_init(int device_no) {

    ui i;
    ui const device_index = am79c960_device_get_index(device_no);
    uw mode = 0;
    INIT_BLOCK_DESC bufr_ring[2];
    inportw (ethernetbase + RESET_OFFSET); /* Reset Ethernet Chip */
    
    /* Set up pointers to the start of the initialisation block
     * start of the TX and start of the RX descriptor blocks
     */
    
    printf("Init Block Address %x\n", init_block[device_index]);
    
    /* Initialize the lance */
    
    WriteRReg(LANCE_CSR0, csr0_STOP); /* Explicitly stop the ISA_LANCE */
    
    WriteIReg(2, (ReadIReg(2)|0x2)); /* Auto Select Media Interface Port */

    /* select DMA channel */
    EnableIsaDmaChannel(AM79C960_base[device_index].dma_channel, DMA_CASCADE_MODE);

    /* Set up the transmit and receive rings  for current device_index */
    
    
    for (i=0;i<RRING_SIZE;i++){	/* For all RX descriptor entries */
	memset(&rring[device_index][i],0,sizeof(LANCE_RECV_BD)); /* clear descriptor */
	rring[device_index][i].ladr =
	  ((ui) current_rring_buffers[device_index][i]) & 0x00ffffff; /* Add buffer pointer to descriptor */
	rring[device_index][i].bcnt = (uw)-ETHER_BUFFER_SIZE; /* Add buffer size field...*/
	rring[device_index][i].own  = 1;	/* Switch ownership to  the LANCE */
    }
    for (i=0;i<SRING_SIZE;i++){	/* For all TX descriptor entries */
	memset(&(sring[device_index][i]),0,sizeof(LANCE_SEND_BD)); /* clear descriptor */
	/* Add buffer pointer to descriptor */
	sring[device_index][i].ladr = ((ui) current_sring_buffers[device_index][i]) & 0x00fffffff; 
    }

    rbuf_idx[device_index] = 0;	/* Reset RX array index */
    sbuf_idx[device_index] = 0;	/* Reset TX array index */
    
    
    /* Setup the initialization block */

    memset(init_block[device_index],0, LANCE_INIT_BLOCK_SIZE); /* clear initialisation block */

    /* Fetch Enet address from PROM and add to initialisation block */

    am79c960_device_get_hw_address(device_no, init_block[device_index] + LANCE_INIT_BLOCK_PADR_OFFSET);

    printf("\tHardware address = ");
    ethernet_printaddress(init_block[device_index] + LANCE_INIT_BLOCK_PADR_OFFSET);
    printf("\n");

    /* Setup ring address and size information; [0]=RX, [1]=TX */

    bufr_ring[0].radr = ((ui) rring[device_index]) & 0x00ffffff;
    bufr_ring[0].rlen = LN2_RRING_SIZE;
    bufr_ring[1].radr = ((ui) sring[device_index]) & 0x00ffffff;
    bufr_ring[1].rlen = LN2_SRING_SIZE;
    memcpy((void *)(init_block[device_index] + LANCE_INIT_BLOCK_RD_OFFSET), 
	(const void *)bufr_ring, sizeof(bufr_ring));
    
    mode |= mode_PORTSEL0; 			/* Port Select AUI=00, 10BASE-T=01     
						  Can be overridden in SW by an autoselect
						  (ASEL bit in ISA_MC CSR) */

    if (etherprom)
      mode |= mode_PROM; /* Enable Promiscous Mode? */

    /* write the mode into the init block */
    memcpy((void *)(init_block[device_index] + LANCE_INIT_BLOCK_MODE_OFFSET), 
	(const void *)&mode, 2);

    /* Check the contents of the receive rings */
    for(i = 0; i <RRING_SIZE; i++) {
	PRTRACE3("Entry %d address %x\n", i, &rring[device_index][i] );
	PRTRACE2("ring entry word 0 = %x\n",*((short int *)&rring[device_index][i]));
	PRTRACE2("ring entry word 1 = %x\n",*(((short int *)&rring[device_index][i])+1));
	PRTRACE2("ring entry word 2 = %x\n",*(((short int *)&rring[device_index][i])+2));
	PRTRACE2("ring entry word 3 = %x\n",*(((short int *)&rring[device_index][i])+3));
    }



    /* Send the init block to the lance and wait for init to finish */

    WriteRReg(LANCE_IADRL, ((ui) init_block[device_index] & 0xffff));
    WriteRReg(LANCE_IADRH, ((ui) init_block[device_index] >>16) & 0xff);

    WriteRReg(LANCE_CSR0, csr0_INIT);
    for (i=0;i<1000;i++) {
	if (inportw(ethernetbase + RDP_OFFSET) & csr0_ERR)
	  break;		/* If an error, break and report */
	else
	  if (inportw(ethernetbase + RDP_OFFSET) & csr0_IDON)
	    {
		printf("Init Done.\n");
		WriteRReg(LANCE_CSR4, ReadRReg(LANCE_CSR4) | csr4_DPOLL); /* Disable Transmit Polling */
		
		am79c960_device_start(device_no); /* zero the counters, set the zero time and start the lance.  */
		return TRUE;	/* return init success flag */
	    }      else
	      msleep(1);	/* 1ms delay */
    }

    /* This code is only reached if an error has occured; Check errors - and dump status */

    printf("?IO: lance init\n");
    
    if (inportw(ethernetbase + RDP_OFFSET) & csr0_BABL)
      printf("   Babble: Xmit timeout\n");
    if (inportw(ethernetbase + RDP_OFFSET) & csr0_CERR)
      printf("   Collision: AUI port\n");
    if (inportw(ethernetbase + RDP_OFFSET) & csr0_MISS)
      printf("   Missed Frame:\n");
    if (inportw(ethernetbase + RDP_OFFSET) & csr0_MERR)
      printf("   Memory Error: Can't master bus\n");

    WriteRReg(LANCE_CSR0, csr0_STOP);
    return FALSE;		/* return error flag */
    
}

        /* Start the LANCE */

/* am79c960_device_start - start device
 *
 * Arguments:
 *
 *    device_no - device number
 *
 */

static void 
am79c960_device_start(int device_no) {

    /*
     * zero the counters, set the zero time and start the lance.
     */
    int const device_index = am79c960_device_get_index(device_no);

    PRTRACE1("Entered am79c960_device_start\n");

    memset(&am79c960_counters[device_index],0, sizeof(LANCE_CTRS));
    am79c960_counters[device_index].zeroed = gettime();        

    while(!(ReadRReg(LANCE_CSR0) & csr0_IDON)); /* Wait for initialisation to complete */



    /* Mask all interrupts except Receive Interrupt. Turn off EMBA and turn on DXMT2PD */

    WriteRReg(LANCE_CSR3, (csr3_MASKALL ^ csr3_RINT));
    WriteRReg(LANCE_CSR0, (ReadRReg(LANCE_CSR0) | csr0_IENA)); /* Enable Interrupts */
    WriteRReg(LANCE_CSR0, (ReadRReg(LANCE_CSR0) | csr0_STRT)); /* Start the LANCE */
    PRTRACE1("Device started; about to enable interrupt\n");

    /* Enable interrupt using the parameter table */

    printf ("Ethernet ISA Base Address: %x\n", ethernetbase);

    intr_enable(AM79C960_base[device_index].interrupt_line);	

    PRTRACE1("ISA interrupt enabled\n");
    return;
}


/* am79c960_device_read - reads a frame from an Ethernet device
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
 *    This function reads a frame from the device if there is one ready to be read. The
 *    frame read is returned in frame_buffer.  To maintain the driver's pool of buffers
 *    the caller must give the driver an unused buffer (balance_buffer) before the
 *    driver returns the frame.  Once this function has been called balance_buffer must
 *    not be used by the calling software.
 *
 *    On return frame_buffer will always point to a valid buffer.  If the read has failed
 *    (result <= 0) the contents of this buffer will not be valid.  Note that whether or
 *    not the read succeeds it is undefined whether the same buffer will be returned as
 *    frame_buffer as was passed down as balance_buffer.
 */

static int  
am79c960_device_read(int device_no,
                     unsigned char balance_buffer[ETHER_BUFFER_SIZE],
                     unsigned char ** frame_buffer) {

    int const device_index = am79c960_device_get_index(device_no);
    int count;

    /* Set up initial return value for error cases */
    *frame_buffer = balance_buffer;

    if((ul) balance_buffer > 0x00FFFFFF || (ul) balance_buffer < 0x00100000) {
	printf("\n\n*******Am79c960 driver internal error: am79c960: Bad balance buffer; address %x*********\n\n", balance_buffer);
	return -1;
    }

    /*
     * Exit if next buffer is owned by lance.
     * Get number of bytes in message.
     * Update counters.
     * Frame and byte counters both incremented as every frame in a single buffer
     * Handle one buffer then return ie. do not empty the ring
     */
    
    mb();			/* Make sure own bit is written out before checking */

    if (rring[device_index][rbuf_idx[device_index]].own)
      return 0;

    /* Check for errors reported by device */
    if( chk_recv_err(device_index, &rring[device_index][rbuf_idx[device_index]])) {
	count = -1;
    }
    else {

	count = rring[device_index][rbuf_idx[device_index]].mcnt - 4; /* -4 deducts the CRC bytes */

#if FALSE
	PRTRACE3("am79c960: %d bytes received on device %d\n", count, device_no);
#endif

	am79c960_counters[device_index].b_recv += count;
	am79c960_counters[device_index].f_recv++;
	if (*current_rring_buffers[device_index][rbuf_idx[device_index]] & 1) { /* check multicast bit */
	    am79c960_counters[device_index].mc_b_recv += count; 
	    am79c960_counters[device_index].mc_f_recv++;
	}

	if (etherdump) {
	    printf("\nDevice %d: Receive Buffer [%x]...\n", device_no,current_rring_buffers[device_index][rbuf_idx[device_index]]);
	    ethernet_printpacket(current_rring_buffers[device_index][rbuf_idx[device_index]], count+4); /* Print CRC bytes too */
	}

	/*
	 * Check for errors.  For all errors, set count negative.
	 * If no errors, copy the frame to the caller buffer.
	 */

	if (count>ETHER_BUFFER_SIZE)
	  count = -1;
	else {
	    *frame_buffer = current_rring_buffers[device_index][rbuf_idx[device_index]]; /* return pointer to buffer with the frame in it */
	    current_rring_buffers[device_index][rbuf_idx[device_index]] = balance_buffer; /* swap in free buffer supplied */ 
	    rring[device_index][rbuf_idx[device_index]].ladr = (int) balance_buffer & 0x00FFFFFF; /* update ring with new pointer */
	}
    }
    /*
     * Give the buffer back to the lance.
     * Advance the ring index to the next buffer.
     * Return the number of bytes received.
     */
    rring[device_index][rbuf_idx[device_index]].own=1;
    mb();			/* Make sure own bit is written out before checking */
    rbuf_idx[device_index] = (rbuf_idx[device_index]+1) % RRING_SIZE;

    return(count);
}

        /* Ethernet send routine */

/* am79c960_device_write - queue a frame to be sent on an ethernet device
 *
 * Arguments:
 *
 *    device_no         - device number
 *    frame              - pointer to start of frame within the buffer
 *    size              - size of frame
 *    frame_buffer      - buffer containing frame
 *    balance_buffer    - returned buffer to maintain buffer balance
 *
 * Returned value:
 *
 *    TRUE -  successfully queued for sending
 *    FALSE - unable to send
 *
 * DESCRIPTION:
 *    This function writes a frame to the device  To maintain the caller's pool of buffers
 *    the driver must give the caller an unused buffer (balance_buffer) in return for
 *    the buffer containing the frame to be transmitted.
 *
 *    If the send succeeds then frame_buffer must not be accessed by the caller after this
 *    call.  If the send can't be queued then frame_buffer will remain valid and
 *    the returned value of balance_buffer is undefined and must not be used.
 *
 */

static int 
am79c960_device_write(int device_no,
                      unsigned char * frame,
                      int size,
                      unsigned char frame_buffer[ETHER_BUFFER_SIZE],
                      unsigned char ** balance_buffer) {

    int const device_index = am79c960_device_get_index(device_no);

    /* Check that the buffer address is valid */
    if((ul) frame > 0x00FFFFFF || (ul) frame < 0x00100000) {
	printf("\n\n********Am79c960 driver internal error: Bad send frame address:address = %x**********\n\n", frame);
	return FALSE;
    }

    PRTRACE2("Sending ethernet frame from buffer %x", frame);

    /*
     * Exit if next buffer is owned by the lance.
     * Enforce minimum packet length.
     * Check the buffer for previous errors.
     * Swap buffer into the lance ring and associated buffer pool.
     * Update send counters.
     */
    if (sring[device_index][sbuf_idx[device_index]].own)
      return FALSE;		/* TX ring full - cannot send */
    if (size<MIN_ENET_PACKET)
      size=MIN_ENET_PACKET;	/* ensure minimum pkt size criteria met */
    if (size>MAX_ENET_PACKET){
	am79c960_counters[device_index].send_fail_flen++; /* flag an error if the pkt is too big */
	return FALSE;
    }

    if (chk_send_err(device_index, &sring[device_index][sbuf_idx[device_index]])) /* Check for historical errors */
      return FALSE;		/* (associated with last time descriptor used) */

    *balance_buffer = current_sring_buffers[device_index][sbuf_idx[device_index]]; /* return pointer to free buffer */
    current_sring_buffers[device_index][sbuf_idx[device_index]] = frame_buffer; /* swap in new frame buffer supplied */ 
    sring[device_index][sbuf_idx[device_index]].ladr = (int) frame & 0x00ffffff; /* update ring with new pointer to start of frame*/
    if (etherdump) {
	printf("\nTransmit Buffer [%x]...\n", sring[device_index][sbuf_idx[device_index]].ladr);
	ethernet_printpacket((unsigned char *) sring[device_index][sbuf_idx[device_index]].ladr, size);
    }
  
    am79c960_counters[device_index].b_sent += size; /* update bytes sent counter - assumes sent on wire! */
    am79c960_counters[device_index].f_sent++;	/* update frames sent counter - ditto */

    /*
     * Fill in fields in buffer descriptor and give buffer to AM79C960.
     * Advance to the next buffer.
     */

    sring[device_index][sbuf_idx[device_index]].bcnt= (uw)-size;
    sring[device_index][sbuf_idx[device_index]].stp=1;
    sring[device_index][sbuf_idx[device_index]].enp=1;
    sring[device_index][sbuf_idx[device_index]].own = 1;
    sbuf_idx[device_index] = (sbuf_idx[device_index]+1) % SRING_SIZE;

    mb();                       /* Make sure descriptor is written out */

    /* Demand Transmit, Clear Transmit Interrupt, and preserve Interrupt Enable  */

    WriteRReg(LANCE_CSR0, (csr0_IENA | csr0_TDMD | csr0_TINT));

    return TRUE;
}

/* chk_recv_err - check for RX errors
 *
 * Arguments:
 *
 *    *rb - pointer to the RX descriptor block to be checked
 *
 * Returned value
 *    TRUE         - errors found
 *    FALSE        - no errors found
 *
 * Description
 *
 *    Check for errors and increment appropriate error counters.
 *    Clear error bit.
 */


static int 
chk_recv_err(int device_index,volatile LANCE_RECV_BD *rb) {
    if (!rb->err)
      return FALSE;
    am79c960_counters[device_index].recv_fail_f_err+= rb->fram;
    am79c960_counters[device_index].recv_fail_flen+= rb->oflo;
    am79c960_counters[device_index].recv_fail_fcs+= rb->crc;

    if (rb->fram) PRTRACE1 ("Recv FRAMING ERROR\n");
    if (rb->oflo) PRTRACE1 ("Recv OVERFLOW ERROR\n");
    if (rb->crc)  PRTRACE1 ("Recv CRC ERROR\n");
    if (rb->buff) PRTRACE1 ("Recv BUFF ERROR\n");

    rb->err = 0;
    return TRUE;
}

/* chk_send_err - check for TX errors
 *
 * Arguments:
 *
 *    device_index - index of device to be checked
 *    *tb - pointer to the TX descriptor block to be checked
 *
 * Returned value
 *    TRUE         - errors found
 *    FALSE        - no errors found
 *
 * Description
 *
 *    Check for collisions and update appropriate counters.
 *    Clear collision flags.
 *    Check for errors and increment appropriate error counters.
 *    Clear error bit.
 */

static int chk_send_err(int device_index, volatile LANCE_SEND_BD *tb)
{
    am79c960_counters[device_index].f_sent_def      += tb->def;
    am79c960_counters[device_index].f_sent_sgl_col  += tb->one;
    am79c960_counters[device_index].f_sent_mult_col += tb->more;
    tb->def = 0;  tb->one = 0;  tb->more = 0;
    if (!tb->err)
      return FALSE;
    am79c960_counters[device_index].send_fail_exs_col += ((tb->tmd3 & tmd3_RTRY) != 0);
    am79c960_counters[device_index].send_fail_cc      += ((tb->tmd3 & tmd3_LCAR) != 0);
    am79c960_counters[device_index].send_fail_defer   += ((tb->tmd3 & tmd3_LCOL) != 0);

    if (tb->tmd3 & tmd3_RTRY) PRTRACE1("Send error: RETRY ERROR\n");
    if (tb->tmd3 & tmd3_LCAR) PRTRACE1("Send error: LOSS OF CARRIER\n");
    if (tb->tmd3 & tmd3_LCOL) PRTRACE1("Send error: LATE COLLISION\n");
    if (tb->tmd3 & tmd3_UFLO) PRTRACE1("Send error: UNDERFLOW ERROR\n");
    tb->err = 0;
    return TRUE;
}

/* am79c960_device_flush - wait for all writes on an ethernet device to complete
 *
 * Argument:
 *
 *    device_no - device to be flushed
 *
 * Returned value:
 *    TRUE	    - flush successful
 *    FALSE	    - flush timeout failure
 */ 

static int
am79c960_device_flush(int device_no) 
{

    int const device_index = am79c960_device_get_index(device_no);
    int i,j=1,k;
    ul watchdog;

    watchdog = AM79C960_FLUSH_WATCHDOG;
    do{
	k = 0;
	for (i=0;i<SRING_SIZE;i++)
	  k |= sring[device_index][i].own;
	if ((j++ & 0xFF)==0)
	  msleep(1);		/* wait for 1ms */
	watchdog = watchdog -1;	/* decrement watchdog timer */
    }
    while (k && watchdog);
    if (k) {
      printf("Watchdog timer expired - device %d failed to flush in %d milliseconds\n",
	     device_no,
	     AM79C960_FLUSH_WATCHDOG);
      return (FALSE);
    }
    return (TRUE);
}
/* am79c960_device_print_stats - print device statistics
 *
 * Argument:
 *
 *     device_no - device number
 */
static void 
am79c960_device_print_stats(int device_no) 
{

    int const device_index = am79c960_device_get_index(device_no);
    int i, *ctr = (int*)&am79c960_counters[device_index].zeroed;
    time_t hours, minutes, seconds;
   
    static char *fmts[23]={ "secs",
			      "bytes rcv", "bytes snt", "frms rcv", "frms snt",
			      "mc bytes rcv", "mc frms rcv", "frms snt dfrd", "frms snt - cllsn",
			      "frms snt - mult cllsn", "snd flrs - xs cllsn", "snd flrs - cc",
			      "snd flrs - shrt", "snd flrs - opn", "snd flrs - flen", "snd flrs - def",
			      "rcv flrs - fcs", "rcv flrs - ferr", "rcv flrs flen", "data ovrn",
			      "unav sbuf", "unav ubuf", "cllsn chk flr"};

    seconds = am79c960_counters[device_index].zeroed % 60;
    minutes = (am79c960_counters[device_index].zeroed/60) % 60;
    hours   = (am79c960_counters[device_index].zeroed/(60*60)) % 24;

    printf("am79c960_counters[device_index] read at time: %2d:%2d:%2d\n\n", hours,minutes,seconds);

    for (i=0;i<5;i++)
      printf("%21s: %9d\t%21s: %9d\n",
	     fmts[i], ctr[i], fmts[i+5], ctr[i+5]);
    printf("\n");
    for (i=0;i<5;i++)
      printf("%21s: %9d\t%21s: %9d\n",fmts[i+10], ctr[i+10], fmts[i+15], ctr[i+15]);
    printf("\n");
    printf("%21s: %9d\t%21s: %9d\n",fmts[20], ctr[20], fmts[21], ctr[21]);
    printf("%21s: %9d\n", fmts[22], ctr[22]);
    return;
}

/* am79c960_device_preg - print all ISA_LANCE registers
 *
 * Argument:
 *
 *     device_no - device number
 */

static void
am79c960_device_preg(int device_no) {

    int i;
    int index;

    /* Getting the index of the device gets its base address as a side effect */
    am79c960_device_get_index(device_no);

    index = inportw(ethernetbase + RAP_OFFSET); /* Save Index Register */

    /*    Print CSR0 ... 126   */

    printf ("Ethernet Controller Base Address %x,  CSR 0...126\n", ethernetbase);
    for (i=0; i<127; i++) {
	printf ("%-3d %04x   ",i, ReadRReg(i));
	if (((i+1) % 7) == 0) printf ("\n");
    }

    printf ("\n\n");

    printf ("Ethernet Controller ISACSR0 ... 7\n");
    for (i=0; i<8; i++)
      printf ("%d %04x   ",i, ReadIReg(i));

    printf ("\n");

    outportw(ethernetbase + RAP_OFFSET, index); /* Restore Index Register */
    return;

}

/* am79c960_device_clear_interrupts - clear all interrupts from an ethernet device
 *
 * Argument:
 *
 *     device_no - device number,
 */

static void 
am79c960_device_clear_interrupts(int device_no) {

    WriteRReg(LANCE_CSR0, ReadRReg(LANCE_CSR0)); /* Clear all interrupts in the lance */

    /* Tell the PIC we have finished */

    outportb(IO_82C59_PIC1,0x20); /* master 82C59A megacell, cascaded */
    outportb(IO_82C59_PIC2,0x20); /* slave 82C59A megacell */

    /* slave has edge triggered interrupts, so OK to reset second */
}
static char *description = "AMD PCnet/ISA Am79C960";
/* am79c960_device_register - register an AM79C960 device driver in the Ethernet device table
 *
 * Arguments:
 *
 *    device_no - device number for registration
 *
 * Returned value
 *    Number of devices registered.
 *
 * Description
 *
 *    Add a AM79C960 device to the installed driver list as follows:
 *		Check not all drivers (AM79C960_MAX_DEVICES) in use
 *		Add base address to the ISA address table
 *		Initialise the appropriate current_buffers pointer array
 *		Set the appropriate driver_in_use flag
 *		Fill device number table with AM79C960 function pointers
 *		
 */

int  
am79c960_device_register(int device_no) {

    int device_index;
    int devices_registered;

    devices_registered = 0;
    for(device_index=0; device_index < AM79C960_MAX_DEVICES; device_index++) {
	if((active_base[device_index] == FALSE)
	   && (AM79C960_base[device_index].detected == TRUE)) {
	  if((device_no+devices_registered) >= MAX_ETHER_DEVICES) {
	    printf("ERROR: Invalid device number %d\n",device_no + devices_registered);
	    return FALSE;
	  }
	    active_base[device_index] = TRUE; /* allocate the next free Ethernet base address */
	    isa_ether_device_map[device_no + devices_registered] =
	      AM79C960_base[device_index].base_addr;  /* add base_addr to the ISA address table */
		
	    /* Register the device with the device type independent device register */
		
	    ether_device_register(device_no + devices_registered, 
				  &am79c960_device_init, 
				  &am79c960_device_read,
				  &am79c960_device_write,
				  &am79c960_device_flush,
				  &am79c960_device_print_stats,
				  &am79c960_device_preg,
				  &am79c960_device_clear_interrupts,
				  &am79c960_device_get_hw_address,
				  &am79c960_device_stop,
				  description);
	    sprintf(MessageString, "%s driver loaded:", description);
	    printf("%-50s Ethernet Device: %d\n",
		   MessageString, device_no + devices_registered);
	    printf("\tISA base address: %x, IRQ: %d, DMA Channel: %x\n",
		   AM79C960_base[device_index].base_addr,
		   AM79C960_base[device_index].interrupt_line,
		   AM79C960_base[device_index].dma_channel);
	    ++devices_registered;
	  }
    }
    return devices_registered;
}
/* am79c960_device_get_hw_address - get the hardware address of a device 
 *
 * Arguments:
 *    device_no - device number for which hardware address is required
 *    hw_address - returned hardware mac address 
 */
static void am79c960_device_get_hw_address(int device_no, mac_addr hw_address)
{
    int i;
    /* Getting the index of the device gets its base address as a side effect */
    am79c960_device_get_index(device_no);
    for(i=0; i < MAC_ADDRESS_SIZE; i++) {
	hw_address[i] = (char) inportb(ethernetbase + HW_ADDR_OFFSET + i);
    }
}

/* am79c960_device_stop - Stop the device (preempts continued DMA activitity)
 *
 * Argument:
 *
 *     device_no - device number
 */

static void
am79c960_device_stop(int device_no) {

    int index;

    /* Getting the index of the device gets its base address as a side effect */
    am79c960_device_get_index(device_no);

/*
 *  Tell the world which device.
 */
    printf("Stopping Am79C960 network device %d.\n", device_no);

    index = inportw(ethernetbase + RAP_OFFSET); /* Save Index Register */
    WriteRReg(0, ReadRReg(0)|4); /* Stop the device */
    outportw(ethernetbase + RAP_OFFSET, index); /* Restore Index Register */
    return;
}

/* am79c960_device_init_module - initialise the module 
 */
void am79c960_device_init_module(void)
{
    int i;
    int j;
    int base;

    /* Allocate buffers for detected am79c960 devices */
    j = 0;
    for (base = 0x360; base >= 0x300; base -= 0x20) {
      outportw(base + RAP_OFFSET, 88);
      if (inportw(base + RAP_OFFSET) != 88)
	continue;
      if (inportw(base + RDP_OFFSET) == 0x3003) {
	PRTRACE2("Am79C960 detected at 0x%x\n", base);
	AM79C960_base[j].detected = TRUE;
	init_block[j] = net_buffer_alloc(LANCE_INIT_BLOCK_SIZE);
	rring[j] = net_buffer_alloc(sizeof(LANCE_RECV_BD) * RRING_SIZE);
	sring[j] = net_buffer_alloc(sizeof(LANCE_SEND_BD) * SRING_SIZE);
	for(i=0; i < RRING_SIZE; i++) /* initialise the RX buffer pointers array */
	  current_rring_buffers[j][i] = net_buffer_alloc(ETHER_BUFFER_SIZE);
	for(i=0; i < SRING_SIZE; i++) /* initialise the TX buffer pointers array */
	  current_sring_buffers[j][i] = net_buffer_alloc(ETHER_BUFFER_SIZE);

	AM79C960_base[j].base_addr = base;

	/*
	 * DMA channel and IRQ should probably be autodetected
	 * here instead of using predefined addresses.
	 */
	switch (base) {
	case 0x360:
	  AM79C960_base[j].interrupt_line = AM79C960_BASE360_INTERRUPT;
	  AM79C960_base[j].dma_channel = AM79C960_BASE360_DMACHANNEL;
	  break;

	case 0x340:
	  AM79C960_base[j].interrupt_line = AM79C960_BASE340_INTERRUPT;
	  AM79C960_base[j].dma_channel = AM79C960_BASE340_DMACHANNEL;
	  break;

	case 0x320:
	  AM79C960_base[j].interrupt_line = AM79C960_BASE320_INTERRUPT;
	  AM79C960_base[j].dma_channel = AM79C960_BASE320_DMACHANNEL;
	  break;

	case 0x300:
	  AM79C960_base[j].interrupt_line = AM79C960_BASE300_INTERRUPT;
	  AM79C960_base[j].dma_channel = AM79C960_BASE300_DMACHANNEL;
	  break;
	}

	if (++j > AM79C960_MAX_DEVICES-1) break;
      }
    }

/*
 *  Now initialise the device index (in case this is the second time
 *  that we are called (via ebuff)).
 */
    for(i = 0; i < AM79C960_MAX_DEVICES; i++) 
      active_base[i] = FALSE;

}

#endif
