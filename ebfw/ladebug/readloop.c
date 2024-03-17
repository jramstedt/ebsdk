
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
static char *rcsid = "$Id: readloop.c,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $";
#endif

/*
 * $Log: readloop.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.24  1997/08/17  11:23:43  fdh
 * Included palosf.h instead of osf.h.
 *
 * Revision 1.23  1997/03/14  18:40:33  fdh
 * Modified to check the existance of an ethernet device
 * before trying to read ethernet packets.
 *
 * Revision 1.22  1997/02/23  11:48:15  fdh
 * Removed the delay when disconnecting.  Results in cleaner
 * disconect.
 *
 * Revision 1.21  1997/02/20  16:18:20  fdh
 * When interrupts are disabled while reading packets
 * save and restore the original ipl.
 *
 * Insure that the noDebugger switch is not enabled.
 *
 * When a client request a disconnect send the reply
 * before actually disconnecting.  Also give a little
 * time for retry requests to be send.
 *
 * Revision 1.20  1996/05/22  21:53:11  fdh
 * Moved machine check handling to other modules.
 *
 * Revision 1.19  1995/12/12  21:20:01  cruz
 * Abort ladebug initialization if no network device is present.
 *
 * Revision 1.18  1995/10/23  22:23:53  cruz
 * Included mon.h which was inadvertantly deleted in the last cleanup.
 *
 * Revision 1.17  1995/10/23  20:51:31  cruz
 * Performed some clean up.
 *
 * Revision 1.16  1995/10/10  14:16:45  fdh
 * Renamed ladbx.h to ladebug.h.
 *
 * Revision 1.15  1995/08/30  23:07:28  fdh
 * Made app_poll() an external function.
 *
 * Revision 1.14  1995/08/25  20:09:01  fdh
 * Removed "ladbxapp.h".
 *
 * Revision 1.13  1994/11/22  21:59:50  fdh
 * Don't include PALcode include files for make depend.
 *
 * Revision 1.12  1994/11/07  12:45:01  rusling
 * Now include both system.h and lib.h
 *
 * Revision 1.11  1994/08/05  20:17:45  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.10  1994/07/11  19:50:13  fdh
 * Included "ladebug.h" and removed unreferenced variables.
 *
 * Revision 1.9  1994/06/29  16:16:22  berent
 * Added reporting of unexpected traps and exceptions.  Also
 * added   clearing of machine checks before the monior attempts
 * to continue from a machine check
 *
 * Revision 1.8  1994/06/28  20:12:18  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.7  1994/06/27  12:09:06  berent
 * Added in checks for unexpected interrupts (including machine check)
 * and code to output messages reporting such interrupts.
 *
 * Revision 1.6  1994/06/23  10:42:33  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.5  1994/06/17  19:36:43  fdh
 * Clean-up...
 *
 * Revision 1.4  1994/03/09  12:48:33  berent
 * NT fixes
 *
 * Revision 1.3  1993/11/30  10:16:02  rusling
 * Moved to use net_buffer rather than isa_buffer.
 *
 * Revision 1.2  1993/10/03  00:34:55  berent
 * Merge in development by Anthony Berent
 *
 * Revision 1.1  1993/06/08  19:56:38  fdh
 * Initial revision
 *
 */

#include "ladebug.h"
#include "ether.h"
#include "mon.h"
#include "palosf.h"		/* For exception definitions. */

volatile int dbug_remotely;
static ip_addr remote_ip_address;
static ip_addr clientIPAddress;
static int is_connected;
static udp_port udp_local_port;
static udp_port old_port = {UDP_PORT >> 8, UDP_PORT & 0xFF};
static udp_port connect_port = {UDP_CONNECT_PORT >> 8, UDP_CONNECT_PORT & 0xFF};
static unsigned char * current_buffer;

typedef void (* poll_dbm) (address_value stop_pc);

/* Interrupt routine; called from kutil. Arguments are the arguments passed to 
 * the low level interrupt routine by the PAL code 
 * data_received should be called from the ethernet interrupt handler
 * to handle a data_received interrupt. It clears the interrupt and
 * processes any received debug packets.
 */
void data_received(unsigned int arg0, ui arg1, ui * arg2)
{
    int save_ipl;

    /* arg0 gives the interrupt type (see architecture manual). If this
       is a RTC interrupt try to read some data*/
    if(arg0 == INT_K_CLK ) {	/* Clock interrupt */
      /* Prevent any more interrupts from happening (at any IPL) */
      save_ipl = swpipl(7);
    
      /* Clear any ethernet interrupt there may be in the ethernet device */
      if (monitor_ether_device != (-1)) {
	ether_device_clear_interrupts(monitor_ether_device);

	/* Read any packets that have arrived */
	read_packets();
      }

      swpipl(save_ipl);		/* Restore ipl */
    }
    else 
      UnexpectedInterruptHandler(arg0, arg1, arg2);
}

/* read_packets reads and processes any received debug packets */
void read_packets(void)
{
    /* Read packets until nothing to read */
    while(ethernet_process_one_packet(monitor_ether_device, current_buffer, &current_buffer));
}    

static void ladbx_input(void * protocol_data,
			int device_no,
			unsigned char * packet,
			int size,
			udp_port source_port,
			ip_addr source_address,
			unsigned char frame_buffer[ETHER_BUFFER_SIZE],
			unsigned char ** balance_buffer)
{
    PRTRACE2("Debug packet received; sp = %x\n",getsp());
    memcpy(remote_ip_address, source_address, IP_ADDRESS_SIZE);
    PRTRACE2("About to process packet: sp = %x\n",getsp());
    ProcessPacket((DebugPacket *)packet);
    PRTRACE2("Packet processed: sp = %x\n",getsp());

    /*
     *  Incase user connected to the debugger with
     *  ladebug and inadvertently disabled the debugger.
     */
    noDebugger = FALSE;

    udp_write(device_no,
	      packet,
	      source_port,
	      source_address,
	      udp_local_port,
	      sizeof(DebugPacket),
	      frame_buffer,
	      balance_buffer);
}
static void ladbx_connect_input(void * protocol_data,
				int device_no,
				unsigned char * packet,
				int size,
				udp_port source_port,
				ip_addr source_address,
				unsigned char frame_buffer[ETHER_BUFFER_SIZE],
				unsigned char ** balance_buffer)
{
    /* Something received on connect port; create a new local port */

    udp_create_port(monitor_ether_device, ladbx_input, NULL, udp_local_port);

    ladbx_input(protocol_data,
		device_no,
		packet,
		size,
		source_port,
		source_address,
		frame_buffer,
		balance_buffer);

    /* Check that we are now actually connected; and delete ports no longer needed */

    if(this_client_is_connected()) {
	udp_delete_port(monitor_ether_device, old_port);
    }
    else {
	udp_delete_port(monitor_ether_device, udp_local_port);
    }
}

int this_client_is_connected(void)
{
    return(is_connected && memcmp(remote_ip_address,clientIPAddress,IP_ADDRESS_SIZE) == 0);
}

int a_client_is_connected(void)
{
    return(is_connected);
}

void set_connected(void) {

    is_connected = TRUE;
    memcpy(clientIPAddress,remote_ip_address,IP_ADDRESS_SIZE);

    printf("Client connected : client's IP address is ");
    ip_printaddress(clientIPAddress);
    printf("\n");

    /* The connect port is no longer needed */
    udp_delete_port(monitor_ether_device, connect_port);
}

void disconnect_client(void)
{
    read_packets();		/* Read any residual packets. */

    dbug_remotely = FALSE;
    is_connected = FALSE;
    udp_delete_port(monitor_ether_device, udp_local_port);
    printf("Client disconnected\n");
}

void enable_ladbx_msg(void)
{
    printf("Enabling remote debug communications\n");
    
    if(! netman_monitor_device_started()) 
      if (!netman_start_monitor_device()) return;

    /* Enable both the old ladebug port and the new connection port */
    if(! udp_register_well_known_port(monitor_ether_device,
				      old_port,
				      ladbx_input,
				      NULL)) {
	printf("Failed to register old style ladebug port\n");
    }
    if(! udp_register_well_known_port(monitor_ether_device,
				 connect_port,
				 ladbx_connect_input,
				 NULL)) {
	printf("Failed to register ladebug connect port\n");
    }	

    /* Set up default values for the local and remote ports; these will be changed if the initial message
     * arrives on the connect port
     */
    memcpy(udp_local_port, old_port, UDP_PORT_SIZE);    
}

void app_poll(address_value stop_pc)
{
    int save_ipl;

    /* Save the address at which to stop if there is a stop command */
    saved_user_pc = stop_pc;

    /* Poll for data */
    /* Prevent any more interrupts from happening (at any IPL) */
    save_ipl = swpipl(7);
    
    /* Clear any ethernet interrupt there may be in the ethernet device */
    if (monitor_ether_device > 0)
      ether_device_clear_interrupts(monitor_ether_device);

    read_packets();
    swpipl(save_ipl);		/* Restore ipl */

    /* Reset the stop address */
    saved_user_pc = 0;
}


void ladbx_server_init_module(void)
{
    current_buffer = net_buffer_alloc(ETHER_BUFFER_SIZE);
}

