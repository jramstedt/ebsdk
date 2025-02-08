/*****************************************************************************

       Copyright 1993, 1994, 1995 Digital Equipment Corporation,
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
__attribute__((unused)) static const char *rcsid = "$Id: bootp.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif

/*
 * This module implements the bootp protocol. Note that it only handles
 * the case of local boot servers.  It does not support booting through
 * a gateway.
 * 
 * $Log: bootp.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.18  1997/05/19  17:15:25  fdh
 * Modified to use sleep functions which are based on the cycle counter
 * to control timing instead of the gettime functions which currently
 * use the RTC.
 *
 * Revision 1.17  1995/10/20  18:40:54  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.16  1995/10/18  12:49:41  fdh
 * Modified to use CheckForChar(3) to check for Control-C pressed.
 *
 * Revision 1.15  1995/10/13  20:06:01  cruz
 * Removed include of prtrace.h
 *
 * Revision 1.14  1995/09/05  18:39:26  fdh
 * use general console port routines rather than uart routines.
 *
 * Revision 1.13  1995/08/25  20:07:32  fdh
 * Fixed a comment line.
 *
 * Revision 1.12  1994/12/06  22:21:58  cruz
 * Added the ability to abort BOOTP retries by pressing Control-C
 *
 * Revision 1.11  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.10  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.9  1994/06/24  15:22:33  rusling
 * Fixed up terminal problem when WNT warnings "fixed".
 *
 * Revision 1.8  1994/06/22  15:11:46  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.7  1994/06/17  19:36:25  fdh
 * Clean-up...
 *
 * Revision 1.6  1994/06/03  18:57:49  fdh
 * Replaced <stdio.h> with <stddef.h>.
 *
 * Revision 1.5  1994/04/02  06:04:15  fdh
 * Replace random() with rand().
 *
 * Revision 1.4  1994/01/19  10:55:52  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.3  1993/11/30  10:14:17  rusling
 * Modified to use net_buff.h and net_buffer_allocate().
 *
 * Revision 1.2  1993/10/01  15:59:33  berent
 * Made bootp tolerate send failures
 *
 * Revision 1.1  1993/08/06  09:47:08  berent
 * Initial revision
 *
 *
 */

#include "ether.h"
#include "console.h"    /* Included for prototype of CheckForChar() */

/*
 * Structure of a bootp packet (see rfc 951)
 */
#define BOOTPP_OP_OFFSET     0
#define BOOTPP_HTYPE_OFFSET  (BOOTPP_OP_OFFSET+1)
#define BOOTPP_HLEN_OFFSET   (BOOTPP_HTYPE_OFFSET+1)
#define BOOTPP_HOPS_OFFSET   (BOOTPP_HLEN_OFFSET+1)
#define BOOTPP_XID_OFFSET    (BOOTPP_HOPS_OFFSET+1)
#define BOOTPP_SECS_OFFSET   (BOOTPP_XID_OFFSET+4)
#define BOOTPP_CIADDR_OFFSET (BOOTPP_SECS_OFFSET+4)
#define BOOTPP_YIADDR_OFFSET (BOOTPP_CIADDR_OFFSET+4)
#define BOOTPP_SIADDR_OFFSET (BOOTPP_YIADDR_OFFSET+4)
#define BOOTPP_GIADDR_OFFSET (BOOTPP_SIADDR_OFFSET+4)
#define BOOTPP_CHADDR_OFFSET (BOOTPP_GIADDR_OFFSET+4)
#define BOOTPP_SNAME_OFFSET  (BOOTPP_CHADDR_OFFSET+16)
#define BOOTPP_FILE_OFFSET   (BOOTPP_SNAME_OFFSET+64)
#define BOOTPP_VEND_OFFSET   (BOOTPP_FILE_OFFSET+128)
#define BOOTPP_PACKET_SIZE   (BOOTPP_VEND_OFFSET+64)

/* OP codes */
#define BOOTP_OP_REQUEST 1
#define BOOTP_OP_REPLY 2

/* BOOTPP ethernet hardware type (same as for ARP except 1 byte) */
#define BOOTP_ETHERNET_HTYPE 1

/* UDP ports used by bootp */

static udp_port udp_port_bootp_server={0,67};
static udp_port udp_port_bootp_client={0,68};

static unsigned char * current_bootp_buffer;
    

/*
 * Static control data
 */

static int bootp_response_received;
static unsigned char *received_server_addr;
static char *received_file_name;
static ui xid;

/*
 * Internal functions
 */

/* bootp_input - handles received bootp packets
 *
 * Arguments:
 *     protocol_data - protocol data passed to module when protocol registered
 *     device_no     - device on which packet was received;
 *     packet        - packet, with UDP header removed;
 *     size          - size of packet;
 *     source_port   - source udp port of packet;
 *     source_address- source ip address of packet;
 *     frame_buffer  - original buffer containing packet;
 *     balance_buffer- buffer returned to maintain buffer balance.
 *
 * DESCRIPTION:
 *     Processes a bootp packet received on the client UDP port. Note that the client port is only registered
 *     as a UDP port when there is a bootp transaction in progress so I can assume the static data is valid.
 */
static void bootp_input(void * protocol_data,
			int device_no,
			unsigned char *packet,
			int size,
			udp_port source_port,
			ip_addr source_address,
			unsigned char frame_buffer[ETHER_BUFFER_SIZE],
			unsigned char ** balance_buffer)
{
    ip_addr my_original_ip_addr;
    mac_addr my_mac_addr;

    /* Set up the buffer to be returned (always frame buffer since this function never keeps it or passes it on) */
    *balance_buffer = frame_buffer;

    /* Check that the packet is a reply packet; assuming everybody is obeying the rules this is actually overkill
     * since requests will be sent to the server port. 
     */
    if(packet[BOOTPP_OP_OFFSET] != BOOTP_OP_REPLY) {
	PRTRACE1("Unexpected bootp request received\n");
	return;
    }
    /* Check that the xid maches */
    if((ui)(packet[BOOTPP_XID_OFFSET]  << 24  | 
	packet[BOOTPP_XID_OFFSET+1] << 16 | 
	packet[BOOTPP_XID_OFFSET+2]  << 8 |
	packet[BOOTPP_XID_OFFSET+3]) != xid) {

	PRTRACE1("Response to old bootp request received\n");
	return;
    }
    /* For completeness check the hardware address type and size and that the client IP and MAC addresses are correct */
    ip_get_device_address(device_no, my_original_ip_addr);
    ether_device_get_hw_address(device_no, my_mac_addr);

    if(packet[BOOTPP_HTYPE_OFFSET] != BOOTP_ETHERNET_HTYPE ||
       packet[BOOTPP_HLEN_OFFSET] != MAC_ADDRESS_SIZE ||
       memcmp(packet+BOOTPP_CIADDR_OFFSET, my_original_ip_addr, IP_ADDRESS_SIZE) != 0 ||
       memcmp(packet+BOOTPP_CHADDR_OFFSET, my_mac_addr, MAC_ADDRESS_SIZE) != 0) {

	PRTRACE1("Bootp response not addressed to me\n");
	return;
    }
    /* This is the response we wanted; remember we have found it */
    bootp_response_received = TRUE;

#ifdef PRTRACE
    PRTRACE1("Received Bootp response: \n");
    ethernet_printpacket(packet - udp_data_offset(), size+udp_data_offset());
    PRTRACE1("\n");
#endif
    /* pick the usefull information out of it */
    strncpy(received_file_name, (char *)packet+BOOTPP_FILE_OFFSET, 128);
    memcpy(received_server_addr, (char *)packet+BOOTPP_SIADDR_OFFSET, IP_ADDRESS_SIZE);
    
    /* Check whether to set the device's new IP address */
    if(memcmp(my_original_ip_addr,own_ip_addr_unknown_address, IP_ADDRESS_SIZE) == 0) {
	/* I don't know my own IP address; so get it from the packet */
	ip_set_device_addr(device_no, packet+BOOTPP_YIADDR_OFFSET);
    }

}

#define RETRY_DELAY 3 /* Time, in seconds, between attempts */

/* bootp_boot - performs a bootp transaction
 *
 * Arguments:
 *     device_no       - device on which request should be sent
 *     input_file_name - file name supplied by caller; or null for default. Null terminated;
 *     server_addr     - returned server IP address
 *     boot_file_name  - returned boot file name supplied by server
 *     
 * Returned value:
 *     TRUE   - bootp successful; server_addr and boot_file_name valid;
 *     FALSE  - bootp failed; server_addr and boot_file_name invalid.
 */
int bootp_boot(int device_no, 
	       char input_file_name[128], 
	       ip_addr server_addr, 
	       char boot_file_name[128]) 
{
    ip_addr my_ip_addr;
    mac_addr my_mac_addr;
    int cancel_bootp;    /* Determines if bootp should be stopped. */
    
    int delay_count = 0; /* Delay counter  */
    int send_count = 0; /* Count of attempts at finding a bootp host */
    int time_passed = 0; /* So that it gets set to 0 before the first send */

    /* Find out who we are (if we know) */
    
    ip_get_device_address(device_no, my_ip_addr);
    ether_device_get_hw_address(device_no, my_mac_addr);

    /* Set up the xid; this should be the same for all attempts */
    xid = rand() % 0xffffffff;

    /* Remember that we haven't yet received a reply */
    bootp_response_received = FALSE;

    /* Tell udp that we want bootp client packets */
    udp_register_well_known_port(device_no,
				 udp_port_bootp_client,
				 bootp_input,
				 NULL);

    /* Remember where to put the usefull data if we get a reply */
    received_server_addr = server_addr;
    received_file_name = boot_file_name;

    /* Now make up to 4 attempts at sending a bootp packet */
    
    printf("Attempting BOOTP.");
    cancel_bootp = FALSE;
    
    while(send_count++ < 4 && ! bootp_response_received && !cancel_bootp) {

	unsigned char * packet;
	packet = current_bootp_buffer + udp_data_offset();

	/* Give the user some dots to keep him quiet */
	printf("..");

	/* Set up a packet */

	memset(current_bootp_buffer, 0, BOOTPP_PACKET_SIZE+udp_data_offset());
	packet[BOOTPP_OP_OFFSET] = BOOTP_OP_REQUEST;
	packet[BOOTPP_HTYPE_OFFSET] = BOOTP_ETHERNET_HTYPE;
	packet[BOOTPP_HLEN_OFFSET] = MAC_ADDRESS_SIZE;
	packet[BOOTPP_HOPS_OFFSET] = 0;
	packet[BOOTPP_XID_OFFSET] = (unsigned char)((xid & 0xff000000) >> 24);
	packet[BOOTPP_XID_OFFSET+1] = (unsigned char)((xid & 0x00ff0000) >> 16);
	packet[BOOTPP_XID_OFFSET+2] = (unsigned char)((xid & 0x0000ff00) >> 8);
	packet[BOOTPP_XID_OFFSET+3] = (unsigned char)(xid & 0x000000ff);

	packet[BOOTPP_SECS_OFFSET] = (time_passed & 0xff00) >> 8;
	packet[BOOTPP_SECS_OFFSET+1] = time_passed & 0x00ff;
	memcpy(packet + BOOTPP_CIADDR_OFFSET, my_ip_addr, IP_ADDRESS_SIZE);
	memcpy(packet + BOOTPP_CHADDR_OFFSET, my_mac_addr, MAC_ADDRESS_SIZE);
	strncpy((char *)packet + BOOTPP_FILE_OFFSET, input_file_name, 128);

	/* All other fields can be left as zero */

	/* send it */

	if( ! udp_write(device_no,
			packet,
			udp_port_bootp_server,
			local_ip_broadcast_address,
			udp_port_bootp_client,
			BOOTPP_PACKET_SIZE,
			current_bootp_buffer,
			&current_bootp_buffer)) {
	    /* If the send failed keep the old buffer and leave the time out mechianism to retry */
	    PRTRACE1("Send failed\n");
	}
	PRTRACE1("\nBootp request sent\n");
	/* Try to get a response */
	delay_count = 0;
	/* Timeout RETRY_DELAY*20000*50usec = RETRY_DELAY in seconds */
	while(!bootp_response_received && (delay_count++ < 20000*RETRY_DELAY) && !cancel_bootp) {
	    /* poll the network */
	  cancel_bootp = CheckForChar(3); /* Check for Control-C */
	  ethernet_process_one_packet(device_no, current_bootp_buffer, &current_bootp_buffer);
	  usleep(50);
	}
	if (!bootp_response_received && (send_count == 1))
	   printf("\nBootp Error, retrying (Press Control-C to abort).\n");

    }
    /* Either I have received a response or I have run out of retrys; ether way tell UDP we aren't interested in any more responses */
    udp_delete_port(device_no, udp_port_bootp_client);

    if(bootp_response_received) {
	/* The data from the response will have already been put in the right places; nothing more to do */
	return TRUE;
    }
    else if (cancel_bootp) 
       {
	  printf("Bootp Aborted.\n");
	  return (FALSE);
       }
    
    else {
	/* Tell the user */
	printf("No response\n");
	/* and exit */
	return FALSE;
    }
}

/* bootp_init_module - initialise the module 
 */
void bootp_init_module(void)
{
    /* Allocate an initial BOOTP buffer */
    current_bootp_buffer = net_buffer_alloc(ETHER_BUFFER_SIZE);
}
