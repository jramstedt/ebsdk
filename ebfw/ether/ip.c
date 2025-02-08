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
__attribute__((unused)) static const char *rcsid = "$Id: ip.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif

/*
 * This module provides a limited end system implementation of the IP protocol. 
 * In particular it does not support the receipt of fragmented datagrams; and 
 * ignores all IP header options.
 * 
 * $Log: ip.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.14  1995/10/20  18:43:51  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.13  1995/10/18  14:02:30  cruz
 * Moved the debug init code to prtrace.c
 *
 * Revision 1.12  1995/10/16  17:04:17  cruz
 * Changed compiled switch for new debugging code
 *
 * Revision 1.11  1995/10/13  20:00:14  cruz
 * Removed #include for prtrace.h.
 * Replaced old PRTRACE debugging statements with new ones that
 * can be turned on/off from user interface.
 *
 * Revision 1.10  1995/10/10  14:12:47  cruz
 * Modified argument definition for ip_printaddress to use the
 * correct type.
 *
 * Revision 1.9  1995/10/05  15:37:38  cruz
 * Fixed bug in the computation of the checksum.  Added code
 * to take care of a possible carry which needed to be added
 * back into the sum.
 *
 * Revision 1.8  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.7  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.6  1994/06/22  15:11:46  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.5  1994/06/17  19:36:25  fdh
 * Clean-up...
 *
 * Revision 1.4  1994/06/03  20:07:49  fdh
 * Replaced <stdio.h> with <stddef.h>.
 *
 * Revision 1.3  1994/01/19  10:55:52  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.2  1993/11/02  22:00:53  fdh
 * Fixup ip address printf statement.
 *
 * Revision 1.1  1993/08/06  09:57:17  berent
 * Initial revision
 *
 *
 */

#include "ether.h"

/* 
 * IP header structure
 */
#define IPH_VER_IHL_OFFSET   0                                     /* Version and header size */
#define IPH_SERVICE_OFFSET   (IPH_VER_IHL_OFFSET+1)                /* Service type */
#define IPH_LENGTH_OFFSET    (IPH_SERVICE_OFFSET+1)                /* Total packet length */
#define IPH_ID_OFFSET        (IPH_LENGTH_OFFSET+2)                  /* ID for fragmentation */
#define IPH_FRAG_FLAG_OFFSET (IPH_ID_OFFSET+2)                     /* Fragmentation control */
#define IPH_TTL_OFFSET       (IPH_FRAG_FLAG_OFFSET+2)              /* Time to live */
#define IPH_PROTOCOL_OFFSET  (IPH_TTL_OFFSET+1)                    /* Upper layer protocol id */
#define IPH_CHKSUM_OFFSET    (IPH_PROTOCOL_OFFSET+1)               /* Header checksum */
#define IPH_SOURCE_OFFSET    (IPH_CHKSUM_OFFSET+2)                 /* Source address */
#define IPH_DEST_OFFSET      (IPH_SOURCE_OFFSET+IP_ADDRESS_SIZE)   /* Destination address */
#define IPH_OPTIONS_OFFSET   (IPH_DEST_OFFSET+IP_ADDRESS_SIZE)     /* Start of options, if any */

/* Standard ethernet protocol ids - from RFC 1340 */
ethernet_protocol_id ether_protocol_ip = {0x08, 0x00};

/* IP address to use when I don't know my own. */
ip_addr own_ip_addr_unknown_address = {0,0,0,0};

/* Define the table of registered protocols */
#define MAX_REGISTERED_IP_PROTOCOLS 20

typedef struct
{
    ip_addr ip_address; /* IP address of device */
    struct { 
	int in_use;
	ip_packet_handler processing_function;
	ip_protocol_id protocol_id;
	void * protocol_data;
    } protocol_table[MAX_REGISTERED_IP_PROTOCOLS];
} device_entry;

static device_entry device_table[MAX_ETHER_DEVICES];

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
#define INIT_INFO               (1 << 5)
#define NOTIFY_OF_PACKETS_TO_OTHERS (1 << 6)
#define SHOW_ADDRESSES_ON_INPUT (1 << 7)
#define SHOW_ADDRESSES_ON_OUTPUT (1 << 8)
#define ALL                     ((1 << 9) - 1)
/*
 *  List of all the possible values for the variable "udp_debug"
 */
VARIABLE_VALUES ip_debug_table[] = 
{
    {DISABLED,          "DISABLED"},    /* Always the first entry. */
    {DUMP_IN_PACKETS,   "DUMP_IN_PACKETS" },
    {DUMP_OUT_PACKETS,  "DUMP_OUT_PACKETS"},
    {ANNOUNCE_ENTRY,    "ANNOUNCE_ENTRY"},
    {ANNOUNCE_EXIT,     "ANNOUNCE_EXIT"},
    {REPORT_ERRORS,     "REPORT_ERRORS"},
    {INIT_INFO,         "INIT_INFO"},
    {NOTIFY_OF_PACKETS_TO_OTHERS, "NOTIFY_OF_PACKETS_TO_OTHERS"},
    {SHOW_ADDRESSES_ON_INPUT, "SHOW_ADDRESSES_ON_INPUT"},
    {SHOW_ADDRESSES_ON_OUTPUT, "SHOW_ADDRESSES_ON_OUTPUT"},
    {ALL,               "ALL"},
    {0,                 NULL}           /* Always the last entry.  */
};

ui ip_debug;

#define IP_DEBUG_PRINT(cond, str) DEBUG_PRINT(ip_debug, cond, str)
#define IP_DEBUG_DO(cond, str) DEBUG_DO(ip_debug, cond, str)

#else

#define IP_DEBUG_PRINT(cond, str)
#define IP_DEBUG_DO(cond, str) 

#endif /* TRACE_ENABLE */

/*
 * Internal functions
 */

#ifdef TRACE_ENABLE
/* ip_printpacket - Prints the header of an IP packet
 */
static void ip_printpacket (unsigned char * packet, char *dir) 
{
    char ip_str[20];
    ip_addr address;

    printf("    :------------------------------------------------------------------:\n");
    printf("IP  : Vers|HLen|Servc| Length| Ident |Flg| FragOff| TTL |Proto| Chksum :\n");
    printf("%-3s : %3d | %2d | %3d | %5d | %5d | %d | 0x%04X | %3d | %3d | 0x%04X :\n", dir, 
           (packet[IPH_VER_IHL_OFFSET] >> 4) & 0xF,
           packet[IPH_VER_IHL_OFFSET] & 0xF,
           packet[IPH_SERVICE_OFFSET],
           ntoh(&packet[IPH_LENGTH_OFFSET], 2),
           ntoh(&packet[IPH_ID_OFFSET], 2),
           (packet[IPH_FRAG_FLAG_OFFSET] & ~0x1f) >> 5,
           ((packet[IPH_FRAG_FLAG_OFFSET] & 0x1F) << 8) + packet[IPH_FRAG_FLAG_OFFSET+1],
           packet[IPH_TTL_OFFSET],
           packet[IPH_PROTOCOL_OFFSET],
           ntoh(&packet[IPH_CHKSUM_OFFSET], 2));

    printf("    : Source IP Addr | Dest IP Address %33s\n", ":");
    memcpy(address, packet+IPH_SOURCE_OFFSET, IP_ADDRESS_SIZE);
    sprintf(ip_str, "%d.%d.%d.%d",address[0], address[1],address[2],address[3]);
    printf("    :%15s | ", ip_str);
    memcpy(address, packet+IPH_DEST_OFFSET, IP_ADDRESS_SIZE);
    sprintf(ip_str, "%d.%d.%d.%d",address[0], address[1],address[2],address[3]);
    printf("%14s %34s\n", ip_str, ":");
    printf("    :------------------------------------------------------------------:\n");
}
#endif /* TRACE_ENABLE */

/* ip_input - ip packet processing function. Read and process a received packet
 *
 * Arguments:
 *
 *     dt_entry      - device table entry for device on which packet was received;
 *     device_no     - device on which packet was received;
 *     packet        - packet, with ethernet header removed;
 *     size          - size of packet;
 *     source        - source MAC address of packet;
 *     frame_buffer  - original buffer containing packet.
 *     balance_buffer- buffer returned to maintain buffer balance buffer.
 *
 * Description:
 *     Processes a received IP packet. For a detailed explanation of the 
 *     interface see ethernet.h.
 *
 * Note:
 *     This function is declared static to prevent other modules calling it directly (rather
 *     than through function pointers.).
 */
static void ip_input(void * dt_entry,
		     int device_no,
		     unsigned char * packet,
		     int size,
		     mac_addr source,
		     unsigned char frame_buffer[ETHER_BUFFER_SIZE],
		     unsigned char ** balance_buffer)
{
    device_entry * const device_table_entry =  dt_entry;
    int const header_size = packet[IPH_VER_IHL_OFFSET] & 0x0f; /* In 32 bit units */
    int const packet_size = (packet[IPH_LENGTH_OFFSET] << 8) + packet[IPH_LENGTH_OFFSET+1]; /* In bytes */
    int header_index;
    unsigned int header_sum = 0;
    int current_entry;
    ip_addr ip_source;
    ip_addr ip_dest;

    IP_DEBUG_PRINT(ANNOUNCE_ENTRY, ("ip_input entered\n"));
    IP_DEBUG_DO(DUMP_IN_PACKETS,  {ip_printpacket(packet, "IN");});
    IP_DEBUG_DO(SHOW_ADDRESSES_ON_INPUT, {printf("IP IN: "); ip_printaddress(&packet[IPH_SOURCE_OFFSET]); printf(" ==> "); ip_printaddress(&packet[IPH_DEST_OFFSET]); printf("\n");});    

    /* Set up the default buffer to be returned */
    *balance_buffer = frame_buffer;
 
    /*
     * Start by checking that the header is valid
     */
    if((packet[IPH_VER_IHL_OFFSET] >> 4) != 4 ||     /* IP protocol version 4 */  
       header_size < 5 ||    /* Header length at least 5 * 32 bits */
       (packet[IPH_FRAG_FLAG_OFFSET] & 0x3f) != 0 || /* Not fragmented (last fragment 
							& most significant part of offset 0) */
       packet[IPH_FRAG_FLAG_OFFSET+1] != 0 ||        /* Not fragmented (least significant part of offset 0) */
       packet[IPH_TTL_OFFSET] == 0  ||                /* Time to live > 0 */
       packet_size > size ||                         /* packet fits in frame */
       packet_size < header_size * 4                 /* packet longer than header */
       ) {
	/* Bad packet header */
	IP_DEBUG_PRINT(REPORT_ERRORS, ("Bad IP header size\n"));
        IP_DEBUG_PRINT(ANNOUNCE_EXIT, ("ip_input exited because bad header size.\n"));
	return;
    }
    /* 
     * Check the destination address; if the device's address is not yet known accept anything. If
     * it is accept only the device's address and network and subnet broadcasts. Acceptable 
     * broadcast addresses start with bytes matching this device's address; and end with bytes 
     * containing 0xff (see rfc1340; this implementation will actually accept some addresses 
     * that are not valid broadcast addresses)
     */
    if(memcmp(device_table_entry -> ip_address, own_ip_addr_unknown_address, IP_ADDRESS_SIZE) != 0) {
	int address_index;
	for(address_index = 0; address_index < IP_ADDRESS_SIZE; address_index++)
	    if(packet[IPH_DEST_OFFSET + address_index] != device_table_entry -> ip_address[address_index])
	      break;

	/* Now check the remaining bytes (if any) for equality to 0xff; if the address has already
           been matched this loop will never execute */

	for(;address_index < IP_ADDRESS_SIZE ; address_index++)
	  if(packet[IPH_DEST_OFFSET + address_index] != 0xff) {
	      /* Not device or broadcast address; ignore packet */
              IP_DEBUG_PRINT(NOTIFY_OF_PACKETS_TO_OTHERS, ("Packet received addressed to unknown address: "));
              IP_DEBUG_DO(NOTIFY_OF_PACKETS_TO_OTHERS, {ip_printaddress(&packet[IPH_DEST_OFFSET]);});
              IP_DEBUG_PRINT(NOTIFY_OF_PACKETS_TO_OTHERS, ("\n"));
              IP_DEBUG_PRINT(ANNOUNCE_EXIT, ("ip_input exited because packet not for us.\n"));      
	      return;
	  }
    }
    /* 
     * Check the checksum; the ones complement sum of the 16 bit header words should be 0xffff; 
     * To calculate the ones complement sum calculate the 32 bit sum and add the top and
     * bottom words together.
     */
    for(header_index = 0; header_index < header_size * 4; header_index += 2) {
	header_sum += (packet[header_index] << 8) + packet[header_index+1];
    }
    header_sum = (header_sum & 0xffff) + ((header_sum >> 16) & 0xffff);    
    header_sum += (header_sum >> 16);  /* Add in possible carry. */
    if(header_sum != 0xffff) {
	/* Bad packet header checksum */
        IP_DEBUG_PRINT(REPORT_ERRORS, ("Bad IP packet header checksum, final sum 0x%X \n", header_sum));
        IP_DEBUG_PRINT(ANNOUNCE_EXIT, ("ip_input exited because of a bad chksum)\n"));      
	return;
    }

    /*
     * Packet acceptable. Process it.
     */

    /* Search for the protocol id in the table */
    for(current_entry = 0; current_entry < MAX_REGISTERED_IP_PROTOCOLS; current_entry++) {
	if(device_table_entry -> protocol_table[current_entry].in_use &&
	   device_table_entry -> protocol_table[current_entry].protocol_id == packet[IPH_PROTOCOL_OFFSET]) {
	  
	    /* Copy the addresses to avoid problems if the packet is reused */
	    memcpy(ip_source, packet+IPH_SOURCE_OFFSET, IP_ADDRESS_SIZE);
	    memcpy(ip_dest, packet+IPH_DEST_OFFSET, IP_ADDRESS_SIZE);

	    /* Pass on the packet by calling the protocol's processing function */
	    (*(device_table_entry -> protocol_table[current_entry].processing_function))
	      (device_table_entry -> protocol_table[current_entry].protocol_data,
	       device_no,
	       packet + header_size * 4,
	       packet_size - header_size * 4,
	       ip_source,
	       ip_dest,
	       frame_buffer,
	       balance_buffer);

	    /* And stop scanning the table since there can only be one entry per protocol id */
	    break;
	}
    }
    IP_DEBUG_PRINT(ANNOUNCE_EXIT, ("ip_input exited normally.\n")); 
}
  
/*
 * Externally callable functions
 */

void ip_init(int device_no)
{
    int entry_no;
    
    /* Clear out all table entries for the device  */
    memcpy(device_table[device_no].ip_address, own_ip_addr_unknown_address, IP_ADDRESS_SIZE);
    for(entry_no = 0; entry_no < MAX_REGISTERED_IP_PROTOCOLS; entry_no++) {
	device_table[device_no].protocol_table[entry_no].in_use = FALSE;
    }

    /* Register the IP protocol with the ethernet protocol handler */
    ethernet_register_protocol(device_no, 
			       ether_protocol_ip, 
			       ip_input,
			       &device_table[device_no]);
    IP_DEBUG_PRINT(INIT_INFO, ("IP initialised on device %d device table entry %x", device_no,  &device_table[device_no]));
}
	

/* ip_set_device_addr - tells ip a device's ip address.
 *
 * Arguments:
 *     device_no - device number
 *     ip        - ip address
 */
void ip_set_device_addr(int device_no,ip_addr ip)
{
    /* Remember the address */
    memcpy(device_table[device_no].ip_address, ip, IP_ADDRESS_SIZE);

    /* And tell the arp module */
    arp_set_device_addr(device_no, ip);
}

/* ip_register_protocol - registers an IP protocol to be recognised in received frames by this module
 *
 * Arguments:
 *
 *    protocol_id           - protocol id to be recognised by this module
 *    processing_function   - function to process received packets; if NULL cancels registration of protocol.
 *    protocol_data         - arbitrary data to be passed back when a packet is received
 *
 * Returned value:
 *    TRUE   - protocol registered
 *    FALSE  - unable to register protocol
 */
int ip_register_protocol(int device_no,
			 ip_protocol_id protocol_id,
			 ip_packet_handler processing_function,
			 void * protocol_data)

{
    /* The function scans the table of protocols to see if this protocol is already registered,
     * if it is the entry is updated; if not it puts an entry for the protocol in the first free
     * slot. When a packet is received the table is scanned from begining to end so this should
     * mean that the most commonly used protocols (assumed to be the first ones registered) are
     * found first.
     */
    int first_free_slot = MAX_REGISTERED_IP_PROTOCOLS; /* Used to keep track of whether their are any free slots
								 and if so where; the value MAX_REGISTERED_PROTOCOLS is
								 used to mean that there are no free slots */
    int current_slot;

    /* Scan from end backwards to simplify setting first_free_slot */
    for(current_slot = MAX_REGISTERED_IP_PROTOCOLS-1; current_slot >= 0; current_slot--) {
	if(device_table[device_no].protocol_table[current_slot].in_use) {
	    if(device_table[device_no].protocol_table[current_slot].protocol_id == protocol_id) {
		/* Entry already exists; update it */
		if(processing_function == NULL) {
		    /* Remove the entry */
		    device_table[device_no].protocol_table[current_slot].in_use = FALSE;
		} else {
		    /* Update the entry */
		    device_table[device_no].protocol_table[current_slot].processing_function = processing_function;
		    device_table[device_no].protocol_table[current_slot].protocol_data = protocol_data;
		}
		return TRUE;
	    }
	} else {
	    first_free_slot = current_slot;
	}
    }
    /* If this point is reached a new entry is needed */
    if(first_free_slot != MAX_REGISTERED_IP_PROTOCOLS) {
	/* There is a slot free; fill it in */
	device_table[device_no].protocol_table[first_free_slot].protocol_id = protocol_id;
	device_table[device_no].protocol_table[first_free_slot].processing_function = processing_function;
	device_table[device_no].protocol_table[first_free_slot].protocol_data = protocol_data;
	device_table[device_no].protocol_table[first_free_slot].in_use = TRUE;
	return TRUE;
    }
    else {
	return FALSE;
    }
}
/* ip_write - sends a packet on an IP network
 *
 * Arguments:
 *
 *    device_no - device on which to send the packet
 *    packet    - pointer to data packet to be sent
 *    destination - destination address for packet
 *    protocol_id - protocol id for packet
 *    size        - size of packet
 *    frame_buffer - buffer containing packet
 *    balance_buffer - buffer returned to maintain buffer balance
 *
 * Return values:
 *
 *    TRUE  - send successfull
 *    FALSE - Error occured
 *
 * DESCRIPTION:
 *    This function writes an ip data packet to the device  To maintain the caller's 
 *    pool of buffers the driver must give the caller an unused buffer (balance_buffer) 
 *    in return for the buffer containing the frame to be transmitted.  
 *
 *    If the send succeeds then frame_buffer must not be accessed by the caller after this
 *    call.  If the send can't be queued then frame_buffer will remain valid and
 *    the returned value of balance_buffer is undefined and must not be used.
 *
 */
int ip_write(int device_no, 
	     unsigned char * packet, 
	     ip_addr destination,
	     ip_protocol_id protocol_id,
	     int size, 
	     unsigned char frame_buffer[ETHER_BUFFER_SIZE],
	     unsigned char ** balance_buffer)
{
    /* Work out where to put the IP header; we are not going to put any options in the packet */
    unsigned char * const header = packet - IPH_OPTIONS_OFFSET;
    int const ip_packet_size = size + IPH_OPTIONS_OFFSET;
    static short int packet_id = 0; /* Used to give each packet a unique id */
    int header_index;
    unsigned int header_sum = 0;
    mac_addr destination_mac_addr;
    int  ether_status;

    IP_DEBUG_PRINT(ANNOUNCE_ENTRY, ("ip_write entered\n"));

    /* Check that we have enough space in the buffer for the header */
    if(header - frame_buffer <  ethernet_data_offset()) {
	*balance_buffer = frame_buffer;
	IP_DEBUG_PRINT(REPORT_ERRORS, ("No space for IP header in frame"));
        IP_DEBUG_PRINT(ANNOUNCE_EXIT, ("ip_write exited because of no space for header.\n"));
	return FALSE;
    }

    /* fill in the header */
    header[IPH_VER_IHL_OFFSET] = (4 << 4) + 5; /* Version 4 and header size 5 (no options) */
    header[IPH_SERVICE_OFFSET] = 0; /* Precedence: Routine, Normal Delay, Normal Throughput, Normal Reliablility */
    header[IPH_LENGTH_OFFSET] = ip_packet_size >> 8; /* MS byte of packet size */
    header[IPH_LENGTH_OFFSET+1] = ip_packet_size & 0xff; /* LS byte of packet size */
    header[IPH_ID_OFFSET] = packet_id >> 8; /* MS byte of packet id */
    header[IPH_ID_OFFSET+1] = (packet_id++) & 0xff; /* LS byte; increment id so that it is different next time */
    header[IPH_FRAG_FLAG_OFFSET] = 0x40; /* Not fragmented but allow fragmentation */
    header[IPH_FRAG_FLAG_OFFSET+1] = 0;
    header[IPH_TTL_OFFSET] = 255; /* This a new packet so give it the maximum time to live */
    header[IPH_PROTOCOL_OFFSET] = protocol_id; /* Protocol id from argument */
    header[IPH_CHKSUM_OFFSET] = 0; /* Checksum to 0 for now to make calculation easier later */
    header[IPH_CHKSUM_OFFSET+1] = 0;

    /* The destination address comes from the arguments */
    memcpy(header+IPH_DEST_OFFSET, destination, IP_ADDRESS_SIZE);

    /* The source address is the device's IP address */
    memcpy(header+IPH_SOURCE_OFFSET, device_table[device_no].ip_address, IP_ADDRESS_SIZE);

    IP_DEBUG_DO(SHOW_ADDRESSES_ON_OUTPUT, {printf("IP OUT: "); ip_printaddress(&header[IPH_SOURCE_OFFSET]); printf(" ==> "); ip_printaddress(&header[IPH_DEST_OFFSET]); printf("\n");});    

    /* Header complete; work out the checksum. This is "the 16 bit one's complement of the one's
     * complement of all 16 bit words in the header" (rfc 791).
     *
     * To calculate the one's complement sum calculate the 32 bit unsigned sum and add the top and
     * bottom words together.
     */

    for(header_index = 0; header_index < IPH_OPTIONS_OFFSET; header_index += 2) {
	header_sum += (header[header_index] << 8) + header[header_index+1];
    }
    header_sum = (header_sum & 0xffff) + (header_sum >> 16);
    header_sum += (header_sum >> 16);  /* Add in possible carry. */
    if(header_sum == 0xffff)header_sum = 0;

    /* Now take its one's complement and put it in the packet */
    header_sum = ~header_sum;
    header[IPH_CHKSUM_OFFSET] = header_sum >> 8;
    header[IPH_CHKSUM_OFFSET+1] = header_sum & 0xff;

    /* Packet complete; use arp to work out the MAC address to send it to */
    if(!arp_resolve(device_no, header+IPH_DEST_OFFSET, destination_mac_addr)) {
	IP_DEBUG_PRINT(REPORT_ERRORS, ("ARP lookup failed\n"));
        IP_DEBUG_PRINT(ANNOUNCE_EXIT, ("ip_write exited because of ARP failure.\n"));
	return FALSE;
    }

    IP_DEBUG_DO(DUMP_IN_PACKETS,  {ip_printpacket(header, "OUT");})
    /* And send it */
    ether_status = ethernet_write(device_no,
			  header,
			  destination_mac_addr,
			  ether_protocol_ip,
			  ip_packet_size,
			  frame_buffer,
			  balance_buffer);

    IP_DEBUG_PRINT(ANNOUNCE_EXIT, ("ip_write exited normally.\n"));
    return ether_status;
}

/* ip_data_offset - returns the offset at which ip data should be placed in a new ethernet frame
 *
 * Return value:
 *    Offset.
 */
int ip_data_offset(void)
{
    return ethernet_data_offset()+IPH_OPTIONS_OFFSET;
}
/* ip_get_address - gets the address that will be used as the source address of transmitted packets.
 *
 * Arguments:
 *     device_no - device number of device on which packet is to be sent.
 *     ip        - returned ip address
 *
 * Note:
 *     This function exists so that UDP can calculate header checksums.
 */
void ip_get_device_address(int device_no, ip_addr ip)
{
    memcpy(ip,device_table[device_no].ip_address, IP_ADDRESS_SIZE);
}
/* ip_printaddress - utility to print an ethernet address 
 *
 * Argument:
 *    address - IP address to be printed
 */
void ip_printaddress(ip_addr address)
{
  printf("%d.%d.%d.%d",address[0], address[1],address[2],address[3]);
}
    
