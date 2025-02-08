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
__attribute__((unused)) static const char *rcsid = "$Id: udp.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif


/*
 * MODULE DESCRIPTION:
 * 
 *     UDP protocol interface for EB64 monitor
 *
 * HISTORY:
 * 
 * $Log: udp.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.15  1995/10/20  20:04:34  cruz
 * Cleanup for lint
 *
 * Revision 1.14  1995/10/20  18:47:17  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.13  1995/10/18  14:00:33  cruz
 * Moved the debug init code to prtrace.c
 *
 * Revision 1.12  1995/10/16  17:04:07  cruz
 * Changed compiled switch for new debugging code
 *
 * Revision 1.11  1995/10/13  20:04:37  cruz
 * Removed #include prtrace.h.
 * Replaced PRTRACE debugging with new macros that allow
 * more user control.
 *
 * Revision 1.10  1995/10/05  15:38:54  cruz
 * Fixed bug in the computation of the checksum.  Added code
 * to take care of a possible carry which needed to be added
 * back into the sum.
 *
 * Revision 1.9  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.8  1994/07/21  17:54:26  fdh
 * Include ethernet.c for ethernet_printpackets().
 *
 * Revision 1.7  1994/07/15  10:27:23  berent
 * Fixed problem with calculating the checksum of packets
 * containing and odd number of bytes
 *
 * Revision 1.6  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.5  1994/06/20  14:42:15  fdh
 * Fixup preprocessor #include conditionals.
 *
 * Revision 1.4  1994/06/17  19:36:25  fdh
 * Clean-up...
 *
 * Revision 1.3  1994/06/03  20:12:32  fdh
 * Replaced <stdio.h> with <stddef.h>.
 *
 * Revision 1.2  1994/01/19  10:55:52  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.1  1993/08/06  10:07:14  berent
 * Initial revision
 *
 *
 */

#include "ether.h"

/*
 * UDP header structure (see RFC 768)
 */

#define UDPH_SOURCE_OFFSET 0
#define UDPH_DESTINATION_OFFSET (UDPH_SOURCE_OFFSET+UDP_PORT_SIZE)
#define UDPH_LENGTH_OFFSET      (UDPH_DESTINATION_OFFSET+UDP_PORT_SIZE)
#define UDPH_LENGTH_SIZE        2
#define UDPH_CHKSUM_OFFSET      (UDPH_LENGTH_OFFSET + UDPH_LENGTH_SIZE)
#define UDPH_CHKSUM_SIZE        2
#define UDPH_HEADER_SIZE        (UDPH_CHKSUM_OFFSET + UDPH_CHKSUM_SIZE)     


#define UDP_IP_PROTOCOL_ID 17

#define MAX_REGISTERED_UDP_PORTS 20

static struct { 
    int in_use;
    udp_packet_handler processing_function;
    udp_port port;
    void * protocol_data;
} udp_table[MAX_ETHER_DEVICES][MAX_REGISTERED_UDP_PORTS];

/* UDP Statistics */
static struct {
    int frames_received;
    int bad_lengths;
    int bad_chksums;
    int forwarded;
    int send_requests;
    int bad_send_lengths;
} udp_stats[MAX_ETHER_DEVICES];

#ifdef TRACE_ENABLE

/*
 *  These are the values that the variable udp_debug can have.
 *  Note that any OR combination of them is allowed, therefore,
 *  ALL of the following defines must be in powers of 2.
 */
#define DISABLED                0
#define DUMP_IN_PACKETS         (1 << 0)
#define DUMP_OUT_PACKETS        (1 << 1)
#define ANNOUNCE_ENTRY          (1 << 2)
#define ANNOUNCE_EXIT           (1 << 3)
#define REPORT_ERRORS           (1 << 4)
#define SHOW_CHKSUM_COMP        (1 << 5)
#define SHOW_PORTS_ON_INPUT     (1 << 6)
#define SHOW_PORTS_ON_OUTPUT    (1 << 7)
#define ALL                     ((1 << 8) - 1)

/*
 *  List of all the possible values for the variable "udp_debug"
 */
VARIABLE_VALUES udp_debug_table[] = 
{
    {DISABLED,          "DISABLED"},    /* Always the first entry. */
    {DUMP_IN_PACKETS,   "DUMP_IN_PACKETS" },
    {DUMP_OUT_PACKETS,  "DUMP_OUT_PACKETS"},
    {ANNOUNCE_ENTRY,    "ANNOUNCE_ENTRY"},
    {ANNOUNCE_EXIT,     "ANNOUNCE_EXIT"},
    {REPORT_ERRORS,     "REPORT_ERRORS"},
    {SHOW_CHKSUM_COMP,  "SHOW_CHKSUM_COMP"},
    {SHOW_PORTS_ON_INPUT, "SHOW_PORTS_ON_INPUT"},
    {SHOW_PORTS_ON_OUTPUT, "SHOW_PORTS_ON_OUTPUT"},
    {ALL,               "ALL"},
    {0,                 NULL}           /* Always the last entry.  */
};

ui udp_debug;

#define UDP_DEBUG_PRINT(cond, str) DEBUG_PRINT(udp_debug, cond, str)
#define UDP_DEBUG_DO(cond, str) DEBUG_DO(udp_debug, cond, str)
#else
#define UDP_DEBUG_PRINT(cond, str)
#define UDP_DEBUG_DO(cond, str) 
#endif /* TRACE_ENABLE */

static void udp_input(void * protocol_data,
                      int device_no,
                      unsigned char * packet,
                      int size,
                      ip_addr source,
                      ip_addr destination,
                      unsigned char frame_buffer[ETHER_BUFFER_SIZE],
                      unsigned char ** balance_buffer);
static int udp_register_port(int device_no , udp_port port , udp_packet_handler processing_function , void * protocol_data);

#ifdef TRACE_ENABLE
static int udp_printpacket (unsigned char * packet, char *dir);
#endif

/*
 * Internal functions
 */


#ifdef TRACE_ENABLE
/*
 *  udp_printpacket - Prints a UDP Packet header
 */
static int udp_printpacket (unsigned char * packet, char *dir) 
{
    int i;
    printf("UDP : SrcPort | DestPort | Length | Chksum | The first 8 bytes of data :\n");
    printf("%3s : %7d | %8d | %6d | 0x%04X | ", dir, 
           ntoh(&packet[UDPH_SOURCE_OFFSET], UDP_PORT_SIZE),
           ntoh(&packet[UDPH_DESTINATION_OFFSET], UDP_PORT_SIZE),
           ntoh(&packet[UDPH_LENGTH_OFFSET], UDPH_LENGTH_SIZE),
           ntoh(&packet[UDPH_CHKSUM_OFFSET], UDPH_CHKSUM_SIZE));
    for (i=0; i <8; i++)
        printf("%02X ", packet[UDPH_HEADER_SIZE+i]);
    printf(":\n");
}
#endif /* TRACE_ENABLE */

/* udp_input - UDP packet processing function
 *
 * Arguments to packet processing functions:
 *     protocol_data - protocol data passed to module when protocol registered
 *     device_no     - device on which packet was received;
 *     packet        - packet, with IP header removed;
 *     size          - size of packet;
 *     source        - source IP address of packet;
 *     destination   - IP address to which the packet was addressed; this is needed by UDP.
 *     frame_buffer  - original buffer containing packet.
 *     balance_buffer- buffer returned to maintain buffer balance.
 *
 */
static void udp_input(void * protocol_data,
                      int device_no,
                      unsigned char * packet,
                      int size,
                      ip_addr source,
                      ip_addr destination,
                      unsigned char frame_buffer[ETHER_BUFFER_SIZE],
                      unsigned char ** balance_buffer)
{
    int port_index;

    /* Extract the UDP size */
    int udp_size = (packet[UDPH_LENGTH_OFFSET] << 8) + packet[UDPH_LENGTH_OFFSET+1];

    udp_stats[device_no].frames_received++;

    UDP_DEBUG_PRINT(ANNOUNCE_ENTRY, ("udp_input entered\n"));
    UDP_DEBUG_DO(DUMP_IN_PACKETS,  {udp_printpacket(packet, "IN");})
    UDP_DEBUG_DO(SHOW_PORTS_ON_INPUT, {printf("UDP IN %d ==> %d\n", ntoh(&packet[UDPH_SOURCE_OFFSET], UDP_PORT_SIZE),  ntoh(&packet[UDPH_DESTINATION_OFFSET], UDP_PORT_SIZE));});

    /* Initialise the returned buffer for error cases */

    *balance_buffer = frame_buffer;
    
    /* Check that the size is valid */
    if(udp_size < UDPH_HEADER_SIZE || udp_size > size) {
	udp_stats[device_no].bad_lengths++;
	UDP_DEBUG_PRINT(REPORT_ERRORS, ("Bad UDP header size\n"));
        UDP_DEBUG_PRINT(ANNOUNCE_EXIT, ("udp_input exited\n"));
	return;
    }

    /* Check whether there is a checksum in the packet; a zero checksum means not used */
    if(packet[UDPH_CHKSUM_OFFSET] != 0 || packet[UDPH_CHKSUM_OFFSET+1] != 0) {
	/* There is a checksum; check it.
	 *
	 * To check the checksum the packet has a psedo header added to it. The 16 bit
         * ones' complement sum of the words (16 bit alligned 16 bit fields) of the complete
	 * packet with the psedo header on the front should be (+ or -) 0.
	 *
	 * The psedo header consists of the following fields:
	 *     IP source address (4 bytes)
	 *     IP destination address (4 bytes)
	 *     Zero filler (1 byte)
	 *     IP protocol id (1 byte)
	 *     UDP packet length; including real header but not psedo header (2 bytes)
	 *
	 * The ones' complemement sum is calculated by calculating the 32 bit unsigned sum of
	 * the words and then adding the top 16 bits to the bottom 16 bits.  Since we know that
         * the checksum is nonzero the result of this will never be zero so it is only
	 * compared with -0 (0xFFFF) to check the checksum.
	 */
	
	unsigned int sum;
	int packet_index;
	
	/* Calculate the sum of the psedo header */
	sum = ((source[0] + source[2] + destination[0] + destination[2]) << 8) +
	      source[1] + source[3] + destination[1] + destination[3] + UDP_IP_PROTOCOL_ID + udp_size;
	
        UDP_DEBUG_PRINT(SHOW_CHKSUM_COMP, ("Sum of dummy header 0x%X\n",sum));
	/* Add in the real packet */
	for(packet_index=0; packet_index < udp_size - 1; packet_index += 2) {
	    sum += (packet[packet_index] << 8) + packet[packet_index+1];
	}

	/* If the packet contains an odd number of bytes add in the last byte */
	if(udp_size % 2 == 1) {
	    sum += packet[packet_index] << 8;
	}

        UDP_DEBUG_PRINT(SHOW_CHKSUM_COMP, ("Sum before 1's complement conversion 0x%X\n",sum));
	
	/* Convert to a 1's complement sum and check it */
	sum = (sum & 0xffff) + (sum >> 16);
	sum += (sum >> 16);  /* Add in possible carry. */
	if(sum != 0xffff) {
	    /* The checksum is wrong */
	    udp_stats[device_no].bad_chksums++;
            UDP_DEBUG_PRINT(REPORT_ERRORS, ("Bad UDP checksum, sum = 0x%X\n", sum));
	    ethernet_printpacket(frame_buffer, size+ip_data_offset());
            UDP_DEBUG_PRINT(ANNOUNCE_EXIT, ("udp_input exited\n"));
	    return;
	}
    }
    /* Packet correct - find out if the port is in use */
    for(port_index = 0; port_index < MAX_REGISTERED_UDP_PORTS; port_index++) {
	if(udp_table[device_no][port_index].in_use &&
	   memcmp(packet+UDPH_DESTINATION_OFFSET, udp_table[device_no][port_index].port, UDP_PORT_SIZE) == 0) {
	    
	    udp_port source_port;
	    
	    udp_stats[device_no].forwarded++;
	    /* Copy the source port so that it remains readable in the called routine even when the packet is reused */
	    memcpy(source_port, packet+UDPH_SOURCE_OFFSET, UDP_PORT_SIZE);
	    
	    /* Call the packet handler bound to the port */
	    (*udp_table[device_no][port_index].processing_function)(udp_table[device_no][port_index].protocol_data,
								     device_no,
								     packet+UDPH_HEADER_SIZE,
								     udp_size-UDPH_HEADER_SIZE,
								     source_port,
								     source,
								     frame_buffer,
								     balance_buffer);
	    /* There can only be one entry per port so exit from the search */
	    break;
	}
    }
    UDP_DEBUG_PRINT(ANNOUNCE_EXIT, ("udp_input exited\n"));
}

/* udp_register_port - internal function to register a port
 *
 * Arguments:
 *    device_no            - device for which the port is to be registered
 *    port                 - port to be registered
 *    processing_function  - function to process packets received on this port
 *    protocol_data        - arbitrary data to be passed back when a packet is received
 *
 * Returned value:
 *    TRUE - protocol registered
 *    FALSE - failed to register protocol
 */

static int udp_register_port(int device_no,
			     udp_port port,
			     udp_packet_handler processing_function,
			     void * protocol_data) {
    /* The function scans the table of port to see if this port is already registered,
     * if it is the entry is updated; if not it puts an entry for the port in the first free
     * slot. When a packet is received the table is scanned from begining to end so this should
     * mean that the most commonly used ports (assumed to be the first ones registered) are
     * found first.
     */
    int first_free_slot = MAX_REGISTERED_UDP_PORTS; /* Used to keep track of whether their are any free slots
					    and if so where; the value MAX_REGISTERED_UDP_PORTS is
					    used to mean that there are no free slots */
    int current_slot;

    /* Scan from end backwards to simplify setting first_free_slot */
    for(current_slot = MAX_REGISTERED_UDP_PORTS-1; current_slot >= 0; current_slot--) {
	if(udp_table[device_no][current_slot].in_use) {
	    if(memcmp(udp_table[device_no][current_slot].port, port, UDP_PORT_SIZE) == 0) {
		/* Entry already exists; fail */
		return FALSE;
	    }
	} else {
	    first_free_slot = current_slot;
	}
    }
    /* If this point is reached a new entry is needed */
    if(first_free_slot != MAX_REGISTERED_UDP_PORTS) {
	/* There is a slot free; fill it in */
	memcpy(udp_table[device_no][first_free_slot].port, port, UDP_PORT_SIZE);
	udp_table[device_no][first_free_slot].processing_function = processing_function;
	udp_table[device_no][first_free_slot].protocol_data = protocol_data;
	udp_table[device_no][first_free_slot].in_use = TRUE;
	return TRUE;
    }
    else {
	return FALSE;
    }
}

 

/*
 * Externally callable functions
 */

/* udp_init - initialise udp protocol module
 *
 * Argument:
 *    device_no - device number on which udp should be enabled.
 */
void udp_init(int device_no)
{
    int port_index;

    /* Register the protocol with ip */
    ip_register_protocol(device_no,
			 (ip_protocol_id) UDP_IP_PROTOCOL_ID,
			 udp_input,
			 NULL);
    /* Clear out the device's port list */
    for(port_index=0; port_index < MAX_REGISTERED_UDP_PORTS; port_index++)
      udp_table[device_no][port_index].in_use = FALSE;
    udp_stats[device_no].frames_received=0;
    udp_stats[device_no].bad_lengths=0;
    udp_stats[device_no].bad_chksums=0;
    udp_stats[device_no].forwarded=0;
    udp_stats[device_no].send_requests=0;
    udp_stats[device_no].bad_send_lengths=0;
}

/* udp_register_well_known_port - tells the udp module what to do with packets received for a particular
 *                                well known port number.
 *
 * Arguments:
 *
 *    device_no             - device number on which port should be registered.
 *    port                  - port to register.
 *    processing_function   - function to process received packets.
 *    protocol_data         - arbitrary data to be passed back when a packet is received
 *
 * Returned value:
 *    TRUE   - protocol registered
 *    FALSE  - unable to register protocol
 */
int udp_register_well_known_port(int device_no,
				 udp_port port,
				 udp_packet_handler processing_function,
				 void * protocol_data) {
    return udp_register_port(device_no, port, processing_function, protocol_data);
}

/* udp_create_new_port - tells the udp module to create a new port; and what to do with data received
 *                       on that port.  The created port number will be outside the well known port 
 *                       number range.
 *
 * Arguments:
 *
 *    device_no             - device number on which port should be created.
 *    processing_function   - function to process received packets
 *    protocol_data         - arbitrary data to be passed back when a packet is received
 *    port                  - returned port number
 *
 * Returned value:
 *    TRUE   - protocol registered
 *    FALSE  - unable to register protocol
 */
int udp_create_port(int device_no,
		    udp_packet_handler processing_function,
		    void * protocol_data,
		    udp_port port) 
{
    static unsigned int next_port = 0x8000;
    port[0] = next_port >> 8;
    port[1] = (next_port++) & 0xff;
    if(next_port == 0x10000) next_port = 0x8000;
    return udp_register_port(device_no, port, processing_function, protocol_data);
}

/* udp_delete_port - deletes a previously created port
 *
 * Argument:
 *    device_no   - device number on which port is registered.
 *    port        - port to be deleted.
 */
void udp_delete_port(int device_no, udp_port port)
{
    int port_index;

    /* Find the port */
    for(port_index = 0; port_index < MAX_REGISTERED_UDP_PORTS; port_index++) {
	if(udp_table[device_no][port_index].in_use &&
	   memcmp(port, udp_table[device_no][port_index].port, UDP_PORT_SIZE) == 0) {

	    /* Port found; delete it */
	    udp_table[device_no][port_index].in_use = FALSE;
	    break;
	}
    }
}

/* udp_write - sends a packet on a UDP port
 *
 * Arguments:
 *
 *    device_no        - device on which to send the packet
 *    packet           - pointer to data packet to be sent
 *    destination_port - UDP port to which the packet should be sent
 *    destination_addr - IP address to which the packet should be sent
 *    source_port      - UDP source port
 *    size             - size of packet
 *    frame_buffer     - buffer containing packet
 *    balance_buffer   - buffer returned to maintain buffer balance
 *
 * Return values:
 *
 *    TRUE  - send successfull
 *    FALSE - Error occured
 *
 * DESCRIPTION:
 *    This function writes an udp data packet to the device.  To maintain the caller's 
 *    pool of buffers the driver must give the caller an unused buffer (balance_buffer) 
 *    in return for the buffer containing the frame to be transmitted.  
 *
 *    If the send succeeds then frame_buffer must not be accessed by the caller after this
 *    call.  If the send can't be queued then frame_buffer will remain valid and
 *    the returned value of balance_buffer is undefined and must not be used.
 *
 */
int udp_write(int device_no, 
	      unsigned char * packet, 
	      udp_port destination_port,
	      ip_addr destination,
	      udp_port source_port,
	      int size, 
	      unsigned char frame_buffer[ETHER_BUFFER_SIZE],
	      unsigned char ** balance_buffer) 
{
    /* Check that there is enough space in the buffer */
    unsigned char * const udp_packet = packet - UDPH_HEADER_SIZE;
    int const udp_size = size+UDPH_HEADER_SIZE;
    unsigned int sum;
    int packet_index;
    ip_addr source;
    int ip_status;

    udp_stats[device_no].send_requests++;
    UDP_DEBUG_PRINT(ANNOUNCE_ENTRY, ("udp_write entered\n"));
    UDP_DEBUG_DO(SHOW_PORTS_ON_OUTPUT, {printf("UDP OUT: %d ==> %d\n", ntoh(&packet[UDPH_SOURCE_OFFSET], UDP_PORT_SIZE),  ntoh(&packet[UDPH_DESTINATION_OFFSET], UDP_PORT_SIZE));});

    if(udp_packet - frame_buffer < ip_data_offset()) {
	udp_stats[device_no].bad_send_lengths++;
	UDP_DEBUG_PRINT(REPORT_ERRORS, ("No space for UDP header\n"));
        UDP_DEBUG_PRINT(ANNOUNCE_EXIT, ("udp_write exited\n"));
	return FALSE;
    }
    /* Fill in the header fields */
    memcpy(udp_packet + UDPH_SOURCE_OFFSET, source_port, UDP_PORT_SIZE);
    memcpy(udp_packet + UDPH_DESTINATION_OFFSET, destination_port, UDP_PORT_SIZE);
    udp_packet[UDPH_LENGTH_OFFSET] = udp_size >> 8;
    udp_packet[UDPH_LENGTH_OFFSET+1] = udp_size & 0xFF;

    /* To calculate the checksum set the checksum field to zero */
    udp_packet[UDPH_CHKSUM_OFFSET] = 0;
    udp_packet[UDPH_CHKSUM_OFFSET+1] = 0;

    /* Calculate the checksum
     *
     * To calculate the checksum the packet has a psedo header added to it. The checksum is then
     * the 16 bit ones' complement of the ones' complement sum of the words (16 bit alligned 16 
     * bit fields) of the complete packet with the psedo header on the front.
     *
     * The psedo header consists of the following fields:
     *     IP source address (4 bytes)
     *     IP destination address (4 bytes)
     *     Zero filler (1 byte)
     *     IP protocol id (1 byte)
     *     UDP packet length; including real header but not psedo header (2 bytes)
     *
     * The ones' complemement sum is calculated by calculating the 32 bit unsigned sum of
     * the words and then adding the top 16 bits to the bottom 16 bits.  If the result is
     * +0 (0x0000) it is converted to -0 (0xFFFF) since a checksum field of 0x0000 means
     * no checksum.
     */

    /* Get the source address */
    
    ip_get_device_address(device_no, source);

    /* Calculate the sum of the psedo header */
    
    sum = ((source[0] + source[2] + destination[0] + destination[2]) << 8) +
      source[1] + source[3] + destination[1] + destination[3] + UDP_IP_PROTOCOL_ID + udp_size;
    
    UDP_DEBUG_PRINT(SHOW_CHKSUM_COMP, ("Sum of dummy header 0x%X\n",sum));
    
    /* Add in the real packet */
    for(packet_index=0; packet_index < udp_size - 1; packet_index += 2) {
	sum += (udp_packet[packet_index] << 8) + udp_packet[packet_index+1];
    }
    
    /* If the packet contains an odd number of bytes add in the last byte */
    if(udp_size % 2 == 1) {
	sum += udp_packet[packet_index] << 8;
    }
    
    UDP_DEBUG_PRINT(SHOW_CHKSUM_COMP, ("Sum before 1's complement conversion 0x%X\n",sum));

    /* Convert to a 1's complement sum */
    sum = (sum & 0xffff) + (sum >> 16);
    sum += (sum >> 16);   /* Add in possible carry. */

    UDP_DEBUG_PRINT(SHOW_CHKSUM_COMP, ("Sum after 1's complement conversion 0x%X\n",sum));

    /* Take its complement */
    sum = ~sum;

    /* Check for special case of +0 */
    if(sum == 0x0000) sum = 0xffff;

    UDP_DEBUG_PRINT(SHOW_CHKSUM_COMP, ("Checksum = 0x%X\n",sum));

    /* Write it to the checksum field in the header */
    udp_packet[UDPH_CHKSUM_OFFSET] = sum >> 8;
    udp_packet[UDPH_CHKSUM_OFFSET+1] = sum & 0xff;
    
    UDP_DEBUG_DO(DUMP_OUT_PACKETS,  {udp_printpacket(packet, "OUT");});

    /* Packet complete; send it */
    ip_status = ip_write(device_no, udp_packet, destination, (ip_protocol_id) UDP_IP_PROTOCOL_ID, 
                         udp_size, frame_buffer, balance_buffer);
    return ip_status;
    UDP_DEBUG_PRINT(ANNOUNCE_EXIT, ("udp_write exited\n"));
}
    

/* udp_data_offset - returns the offset at which udp data should be placed in a new ethernet frame
 *
 * Return value:
 *    Offset.
 */
int udp_data_offset(void) {
    return ip_data_offset() + UDPH_HEADER_SIZE;
}
