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
static char *rcsid = "$Id: arp.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif

/*
 * DESCRIPTION:
 *     This module implements the ARP protocol as described in RFC 826. Only
 *     MAC style hardware addresses and IP style protocol addresses are
 *     supported. In addition this module allows other modules to query 
 *     the information received through the ARP protocol.
 *
 * HISTORY:
 * $Log: arp.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.19  1997/05/19  17:15:08  fdh
 * Modified to use sleep functions which are based on the cycle counter
 * to control timing instead of the gettime functions which currently
 * use the RTC.
 *
 * Revision 1.18  1995/10/20  20:05:37  cruz
 * Cleanup for lint
 *
 * Revision 1.17  1995/10/20  18:40:10  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.16  1995/10/18  14:02:55  cruz
 * Moved the debug init code to prtrace.c
 *
 * Revision 1.15  1995/10/17  15:36:32  cruz
 * Replaced old debugging code with the new code.
 * Added (but left disabled) routine netwatch which
 * waits for a packet to be received and dispatches to
 * the correct protocol handler.  This is useful for
 * debugging with the new commands.
 *
 * Revision 1.14  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.13  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.12  1994/06/22  15:11:46  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.11  1994/06/17  19:36:25  fdh
 * Clean-up...
 *
 * Revision 1.10  1994/06/03  20:05:36  fdh
 * Replaced <stdio.h> with <stddef.h>.
 *
 * Revision 1.9  1994/04/02  06:04:57  fdh
 * Replace random() with rand().
 *
 * Revision 1.8  1994/03/16  11:01:47  rusling
 * Remove extranious debug printf() statement.
 *
 * Revision 1.7  1994/01/21  09:43:45  rusling
 * Minor fixup.
 *
 * Revision 1.6  1994/01/20  17:23:18  rusling
 * Fixed up syntax error (oops).
 *
 * Revision 1.4  1994/01/19  10:55:52  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.3  1993/11/30  10:14:17  rusling
 * Modified to use net_buff.h and net_buffer_allocate().
 *
 * Revision 1.2  1993/10/01  15:57:53  berent
 * corrected failure return from -1 to FALSE
 *
 * Revision 1.1  1993/08/06  09:45:33  berent
 * Initial revision
 *
 *
 */

#include "ether.h"

/*
 * Protocol structures and field values
 * ====================================
 */

/* Arp protocol fields. Defined using offset constants etc. to avoid any
 * allignment or byte ordering problems when using structures. All offsets
 * are from the start of the ARP packet (not from the start of the ethernet
 * frame) Note that  since this implementation can only be used for resolving 
 * IP addresses over LANs all address sizes (hardware and protocol) are fixed.
 *
 */
#define AR_HRD_OFFSET 0                  /* Hardware address space - must be Ethernet address space */
#define AR_PRO_OFFSET (AR_HRD_OFFSET+2)  /* Protocol address space - must be IP; value is ethernet protocol id
					    of IP (0x0800) */
#define AR_HLN_OFFSET (AR_PRO_OFFSET+2)  /* Byte length of each hardware address; will be 6 */
#define AR_PLN_OFFSET (AR_HLN_OFFSET+1)  /* Byte length of each protocol address; will be 4 */
#define AR_OP_OFFSET  (AR_PLN_OFFSET+1)   /* opcode */
#define AR_SHA_OFFSET (AR_OP_OFFSET +2)  /* Source hardware address */
#define AR_SPA_OFFSET (AR_SHA_OFFSET+MAC_ADDRESS_SIZE)  /* Source IP address */
#define AR_THA_OFFSET (AR_SPA_OFFSET+IP_ADDRESS_SIZE)   /* Target hardware address (if known) */
#define AR_TPA_OFFSET (AR_THA_OFFSET+MAC_ADDRESS_SIZE)  /* Target protocol address */
#define AR_PACKET_SIZE (AR_TPA_OFFSET+IP_ADDRESS_SIZE)  /* Total size of arp packets */

/*
 * Constants used in the ARP protocol 
 */

/* Op codes; defined in RFC 826 */
static char const ar_op_request[2] = {0x00, 0x01};
static char const ar_op_reply[2] = {0x00, 0x02};

/* Ethernet hardware type - see RFC 1340 */
char const arp_hardware_ether[2] = {0x00, 0x01};

/* Standard ethernet protocol ids - from RFC 1340 */
static ethernet_protocol_id ether_protocol_arp = {0x08, 0x06};

/* Broadcast MAC address */
static mac_addr broadcast_mac_address = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

ip_addr local_ip_broadcast_address = {0xFF,0xFF,0xFF,0xFF};

/*
 * Module data structures
 * ======================
 *
 */ 

/*
 * ARP table.  Contains ethernet<->ip address pairs. There is a copy for
 * each ethernet device.
 */
#define ARP_TABLE_SIZE 5

static struct arp_data {  
  ip_addr     ip_address;
  mac_addr    enet_addr;
  enum        {isempty, isvalid} flags;
} arp_table[MAX_ETHER_DEVICES][ARP_TABLE_SIZE];

/* List of local ip addresses; assumed to be one per ethernet device,
 * note that static data initialisation will initialise all of these
 * to 0 which in ip means "I don't know my own address"
 */
static ip_addr device_ip_address[MAX_ETHER_DEVICES];

static unsigned char * current_arp_buffer;

#ifdef TRACE_ENABLE

/*
 *  These are the values that the variable udp_debug can have.
 *  Note that any OR combination of them is allowed, therefore,
 *  ALL of the following defines must be in powers of 2.
 */
#define DISABLED                0
#define REPORT_TABLE_UPDATES   (1 << 0)
#define REPORT_ARP_NOT_ADDRESSED_TO_ME (1 << 1)
#define ANNOUNCE_ENTRY          (1 << 2)
#define ANNOUNCE_EXIT           (1 << 3)
#define REPORT_ERRORS           (1 << 4)
#define SHOW_BUFFER_ADDRESS     (1 << 5)
#define SHOW_TABLE_LOOKUPS      (1 << 6)
#define SHOW_NETWORK_LOOKUPS    (1 << 7)
#define SHOW_IP_TO_MAC_LOOKUPS  (1 << 8)
#define SHOW_TABLE_LOOKUPS_FROM_NETWORK_RQST (1 << 9)
#define DUMP_IN_PACKETS         (1 << 10)
#define DUMP_OUT_PACKETS        (1 << 11)
#define ALL                     ((1 << 12) - 1)

/*
 *  List of all the possible values for the variable "arp_debug"
 */
VARIABLE_VALUES arp_debug_table[] = 
{
    {DISABLED,          "DISABLED"},    /* Always the first entry. */
    {REPORT_TABLE_UPDATES, "REPORT_TABLE_UPDATES"},
    {REPORT_ARP_NOT_ADDRESSED_TO_ME, "REPORT_ARP_NOT_ADDRESSED_TO_ME"},
    {ANNOUNCE_ENTRY,    "ANNOUNCE_ENTRY"},
    {ANNOUNCE_EXIT,     "ANNOUNCE_EXIT"},
    {REPORT_ERRORS,     "REPORT_ERRORS"},
    {SHOW_BUFFER_ADDRESS, "SHOW_BUFFER_ADDRESS"},
    {SHOW_TABLE_LOOKUPS,  "SHOW_TABLE_LOOKUPS"},
    {SHOW_NETWORK_LOOKUPS,  "SHOW_NETWORK_LOOKUPS"},
    {SHOW_IP_TO_MAC_LOOKUPS, "SHOW_IP_TO_MAC_LOOKUPS"},
    {SHOW_TABLE_LOOKUPS_FROM_NETWORK_RQST, "SHOW_TABLE_LOOKUPS_FROM_NETWORK_RQST"},
    {DUMP_IN_PACKETS,   "DUMP_IN_PACKETS" },
    {DUMP_OUT_PACKETS,  "DUMP_OUT_PACKETS"},
    {ALL,               "ALL"},
    {0,                 NULL}           /* Always the last entry.  */
};

ui arp_debug;

#define ARP_DEBUG_PRINT(cond, str) DEBUG_PRINT(arp_debug, cond, str)
#define ARP_DEBUG_DO(cond, str) DEBUG_DO(arp_debug, cond, str)
#else
#define ARP_DEBUG_PRINT(cond, str)
#define ARP_DEBUG_DO(cond, str) 
#endif /* TRACE_ENABLE */


/* prototypes for internal functions. */
static int arp_lookup(const int device_no , const ip_addr ip);
static void arp_query_network(const int device_no , const ip_addr ip);
static void arp_input(void * protocol_data, int device_no, unsigned char * packet,
		      int size, mac_addr source, unsigned char frame_buffer[ETHER_BUFFER_SIZE],
		      unsigned char ** balance_buffer);

#ifdef TRACE_ENABLE
static void arp_printpacket (unsigned char * packet, char *dir);
#endif

/*
 * Internal functions
 * ==================
 */
#ifdef TRACE_ENABLE
/* arp_printpacket - Prints an ARP packet
 */
static void arp_printpacket (unsigned char * packet, char *dir) 
{
    int operation;

    printf("\nARP :  HW  | Prot | HLen | PLen | Operation :\n");
    printf("%-3s : %4d | %4d | %4d | %4d | ", dir, ntoh(&packet[AR_HRD_OFFSET],2),
           ntoh(&packet[AR_PRO_OFFSET],2), packet[AR_HLN_OFFSET], packet[AR_PLN_OFFSET]);
    operation = ntoh(&packet[AR_OP_OFFSET],2);
    switch(operation) {
    case 1:
        printf(" ARP RQST ");
        break;
    case 2:
        printf(" ARP REPLY");
        break;
    case 3:
        printf("RARP RQST ");
        break;
    case 4:
        printf("RARP REPLY");
        break;
    default:
        printf("%d", operation);
    }
    printf(":\n    : Sender's HardAddr | Sender's IP  :\n    : ");
    ethernet_printaddress(&packet[AR_SHA_OFFSET]);
    printf(" | ");
    ip_printaddress(&packet[AR_SPA_OFFSET]);
    printf(" :\n    : Target's HardAddr | Target's IP  :\n    : ");
    ethernet_printaddress(&packet[AR_THA_OFFSET]);
    printf(" | ");
    ip_printaddress(&packet[AR_TPA_OFFSET]);
    printf(" :\n");
}
#endif /* TRACE_ENABLE */

/* arp_lookup - internal function to search for an ip address in the table
 *
 * Argument:
 *    device_no - device number on which we are planning to send
 *    ip        - ip address to search for
 *
 * Returned value:
 *    +ve or 0 - Index into table of entry containing value
 *    -1       - Not found
 */
static int arp_lookup(int const device_no, ip_addr const ip)
{
  int j;

  ARP_DEBUG_DO(SHOW_TABLE_LOOKUPS, {printf("ARP lookup of IP (%d): ", device_no); ip_printaddress(ip);});    

  for (j=0;j<ARP_TABLE_SIZE;j++){
      if (arp_table[device_no][j].flags==isvalid && 
          memcmp(arp_table[device_no][j].ip_address,ip,IP_ADDRESS_SIZE)==0) {
          ARP_DEBUG_PRINT(SHOW_TABLE_LOOKUPS, (" FOUND\n"));
          return j;
      }
  }
  ARP_DEBUG_PRINT(SHOW_TABLE_LOOKUPS, (" NOT FOUND\n"));
  return -1;
}

/*
 * arp_query_network - internal function to ask whether anybody on the network
 *                     recognises an address. 
 *
 * Arguments:
 *     device_no - which device to query
 *     ip        - ip address required
 */
static void arp_query_network(int const device_no,ip_addr const ip)
{
    /* Broadcast an arp query message.  Then read the input stream until
     * an arp message is received or 5 seconds elapses.
     */
    
    /* Declare an initial buffer and a static variable in which a record of the current buffer can
     * be maintained 
     */

    int count;
    mac_addr my_mac_address;
    unsigned char * const arp_packet = current_arp_buffer + ethernet_data_offset();

    ARP_DEBUG_PRINT(ANNOUNCE_ENTRY, ("arp_query_network entered\n"));
    /*
     * Set up a request packet
     */
    memcpy(arp_packet+AR_HRD_OFFSET, arp_hardware_ether, 2);
    memcpy(arp_packet+AR_PRO_OFFSET, ether_protocol_ip, ETHERNET_PROTOCOL_ID_SIZE);
    arp_packet[AR_HLN_OFFSET] = MAC_ADDRESS_SIZE;
    arp_packet[AR_PLN_OFFSET] = IP_ADDRESS_SIZE;
    memcpy(arp_packet+AR_OP_OFFSET, ar_op_request, 2);
    
    /* Get the device's MAC address and copy it in */
    ether_device_get_hw_address(device_no, my_mac_address);
    memcpy(arp_packet + AR_SHA_OFFSET, my_mac_address, MAC_ADDRESS_SIZE);

    /* Copy in the device's IP address */
    memcpy(arp_packet + AR_SPA_OFFSET, device_ip_address[device_no], IP_ADDRESS_SIZE);

    /* And the requested IP address */
    memcpy(arp_packet + AR_TPA_OFFSET, ip, IP_ADDRESS_SIZE); 

    /*
     * Transmit the packet.
     */
    ARP_DEBUG_DO(SHOW_NETWORK_LOOKUPS, {printf("Sending ARP request for IP (%d): ", device_no); ip_printaddress(ip);printf("\n");});
    ARP_DEBUG_DO(DUMP_OUT_PACKETS,  {arp_printpacket(arp_packet, "OUT");});

    /* Repeat send until it succeeds */
    ARP_DEBUG_PRINT(SHOW_BUFFER_ADDRESS, ("Arp buffer before send 0x%X\n", current_arp_buffer));
    while( ! ethernet_write(device_no,
			    arp_packet,
			    broadcast_mac_address,
			    ether_protocol_arp,
			    AR_PACKET_SIZE,
			    current_arp_buffer,
			    &current_arp_buffer));
    ARP_DEBUG_PRINT(SHOW_BUFFER_ADDRESS, ("Arp buffer after send 0x%X\n", current_arp_buffer));

    /*
     * Now read packets for up to five seconds or when arp_lookup returns
     * a positive value.
     */
    count = 0;
    /* Timeout 5*20000*50usec = 5 seconds */
    while( (count++ < 20000*5) && arp_lookup(device_no, ip)<0) {
      if(ethernet_process_one_packet(device_no, current_arp_buffer, &current_arp_buffer)) {
        ARP_DEBUG_PRINT(SHOW_BUFFER_ADDRESS, ("Arp buffer after processing packet 0x%X\n", current_arp_buffer));;
      }
      usleep(50);
    }
    ARP_DEBUG_PRINT(ANNOUNCE_EXIT, ("arp_query_network exited normally\n"));
}
/* arp_input - arp received packet processor.
 *
 * Arguments:
 *     protocol_data - protocol data passed to module when protocol registered; not used
 *     device_no     - device on which packet was received;
 *     packet        - packet, with ethernet header removed;
 *     size          - size of packet;
 *     source        - source MAC address of packet;
 *     frame_buffer  - original buffer containing packet.
 *     balance_buffer- buffer returned to maintain buffer balance buffer.
 *
 * Description:
 *     Processes a received ARP packet. For a detailed explanation of the 
 *     interface see ethernet.h.
 *
 * Note:
 *     This function is declared static to prevent other modules calling it directly (rather
 *     than through function pointers.).
 */
static void arp_input(void * protocol_data,
		      int device_no,
		      unsigned char * packet,
		      int size,
		      mac_addr source,
		      unsigned char frame_buffer[ETHER_BUFFER_SIZE],
		      unsigned char ** balance_buffer)
{
    int merge_flag = FALSE;
    int tidx;
    mac_addr my_mac_address;

    ARP_DEBUG_PRINT(ANNOUNCE_ENTRY, ("arp_input entered\n"));
    ARP_DEBUG_DO(DUMP_IN_PACKETS,  {arp_printpacket(packet, "IN");});

    /*
     * Quit if the hardware type isn't ethernet or if the protocol
     * type isn't IP.
     */
    if(memcmp(packet+AR_HRD_OFFSET, arp_hardware_ether, 2) != 0 ||
       memcmp(packet+AR_PRO_OFFSET, ether_protocol_ip, ETHERNET_PROTOCOL_ID_SIZE) != 0) {
	*balance_buffer=frame_buffer;
        ARP_DEBUG_PRINT(REPORT_ERRORS, ("ARP packet received with unknown hardware or protocol type: \n"));
	ARP_DEBUG_PRINT(REPORT_ERRORS, ("Hardware type: 0x%X\nProtocol type: 0x%X\n", ntoh(&packet[AR_HRD_OFFSET],2),ntoh(&packet[AR_PRO_OFFSET], 2)));
        ARP_DEBUG_PRINT(ANNOUNCE_EXIT, ("arp_input exited because of wrong hardware or protocol.\n"));
	return;
    }

    /*
     * Next check whether we already know about the IP source address
     */

    for (tidx=0;tidx<ARP_TABLE_SIZE;tidx++) {
	if ((arp_table[device_no][tidx].flags==isvalid) &&
	    (memcmp(arp_table[device_no][tidx].ip_address, packet+AR_SPA_OFFSET, IP_ADDRESS_SIZE)==0)){

	    /*  An entry already exists for this address; update it. */
	    
	    memcpy(arp_table[device_no][tidx].enet_addr, packet+AR_SHA_OFFSET, MAC_ADDRESS_SIZE);
	    merge_flag = TRUE;
	    break;
	}
    }
  
    /* Nothing more to do unless we are the target */
    if(memcmp(packet+AR_TPA_OFFSET, device_ip_address[device_no], IP_ADDRESS_SIZE) != 0) {
	*balance_buffer=frame_buffer;
	ARP_DEBUG_DO(REPORT_ARP_NOT_ADDRESSED_TO_ME, {printf("ARP packet ignored; I am not the target, IP: "); ip_printaddress(packet+AR_TPA_OFFSET);printf(" is.\n");});
        ARP_DEBUG_PRINT(ANNOUNCE_EXIT, ("arp_input exited because arp not addressed to me.\n"));
	return;
    }

    /*
     * If we are the target, continue processing.
     */
    if (!merge_flag){
	/*
	 * No entry from the sender is already in the table.
	 * Insert the sender protocol and hardware address pair in the
	 * table.
	 * If we don't find an empty slot, pick a victum slot at random.
	 * Enter the sender data in the table.
	 */
	for (tidx=0;tidx<ARP_TABLE_SIZE;tidx++)
	  if (arp_table[device_no][tidx].flags==isempty)
	    break;
	if (tidx>=ARP_TABLE_SIZE)
	  tidx = rand() % ARP_TABLE_SIZE;

	memcpy(arp_table[device_no][tidx].ip_address,packet+AR_SPA_OFFSET, IP_ADDRESS_SIZE);
	memcpy(arp_table[device_no][tidx].enet_addr, packet+AR_SHA_OFFSET ,MAC_ADDRESS_SIZE);
	arp_table[device_no][tidx].flags=isvalid;

        ARP_DEBUG_PRINT(REPORT_TABLE_UPDATES, ("Table entry %d for device %d updated. IP = ", tidx, device_no));
        ARP_DEBUG_DO(REPORT_TABLE_UPDATES, {ip_printaddress(arp_table[device_no][tidx].ip_address); printf(" MAC = "); ethernet_printaddress(arp_table[device_no][tidx].enet_addr); printf("\n");}); 
    }

    /*
     * If the packet is a request packet we must send a response
     * packet.
     */
    if (memcmp(packet+AR_OP_OFFSET, ar_op_request, 2)== 0){
	/*
	 * Move sender data to target data.
	 * Put our address into the sender fields.
	 * Set the opcode to reply.
	 * Transmit the packet.
	 */

	/* Copy target hardware and protocol addresses from the original source addresses*/
	memcpy(packet + AR_THA_OFFSET, packet + AR_SHA_OFFSET, MAC_ADDRESS_SIZE);
	memcpy(packet + AR_TPA_OFFSET, packet + AR_SPA_OFFSET, IP_ADDRESS_SIZE);

	/* The new source hardware address is the device's hardware address */
	ether_device_get_hw_address(device_no, my_mac_address);
	memcpy(packet + AR_SHA_OFFSET, my_mac_address, MAC_ADDRESS_SIZE);

	/* The new source protocol address is the device's protocol address */
	memcpy(packet + AR_SPA_OFFSET, device_ip_address[device_no], IP_ADDRESS_SIZE);

	/* The new op code is reply */
	memcpy(packet + AR_OP_OFFSET, ar_op_reply, 2);

        ARP_DEBUG_PRINT(SHOW_TABLE_LOOKUPS_FROM_NETWORK_RQST, ("\nNetwork request for ARP lookup.\nReturning IP = "));
        ARP_DEBUG_DO(SHOW_TABLE_LOOKUPS_FROM_NETWORK_RQST, {ip_printaddress(device_ip_address[device_no]); printf(" MAC = "); ethernet_printaddress(my_mac_address); printf(" to MAC =");ethernet_printaddress(packet+AR_THA_OFFSET);printf("\n");}); 
	
	/* All other fields are identical to the received packet; send the packet */
	ethernet_write(device_no,
		       packet,
		       packet + AR_THA_OFFSET,
		       ether_protocol_arp,
		       AR_PACKET_SIZE,
		       frame_buffer,
		       balance_buffer);
    }
    ARP_DEBUG_PRINT(ANNOUNCE_EXIT, ("arp_input exited normally.\n"));
}

/*
 * Externally callable functions
 * =============================
 */


/* arp_init - initialises the arp module; and registers it with the
 * ethernet protocol handler.
 *
 * Argument:
 *    device_no - device number on which arp should be registered.
 */
void arp_init(int device_no)
{
    int entry_no;
    
    /* Clear out all tables */
    memcpy(device_ip_address[device_no], own_ip_addr_unknown_address, 
	   IP_ADDRESS_SIZE);
    for(entry_no=0; entry_no < ARP_TABLE_SIZE; entry_no++) {
	arp_table[device_no][entry_no].flags = isempty;
    }

    /* Register the ARP protocol with the ethernet protocol handler */
    ethernet_register_protocol(device_no, ether_protocol_arp, 
	arp_input, NULL);
}
	

/* arp_set_device_addr - tells arp a device's ip address.
 *
 * Arguments:
 *     device_no - device number
 *     ip        - ip address
 */
void arp_set_device_addr(int device_no,ip_addr ip)
{
    memcpy(device_ip_address[device_no], ip, IP_ADDRESS_SIZE);
}
 
/* arp_resolve - converts an ip address into an ethernet address
 *
 * Arguments:
 *     device_no - device on which the message is to be sent
 *     ip        - ip address
 *     mac       - returned ethernet MAC address
 *
 * Returned value:
 *     TRUE      - address resolved; eaddr valid
 *     FALSE     - Unable to resolve address.
 */
int arp_resolve(int device_no, ip_addr ip, mac_addr mac)
{
    int i,j;
    /*
     * If the ip address is broadcast, use broadcast ethernet address.
     * Look up the address in the table.  If found, return it.  If not
     * found, call arp_transact to get address from a server.
     * Repeat the loop 3 times.
     */

    ARP_DEBUG_PRINT(ANNOUNCE_ENTRY, ("arp_resolve entered\n"));
    ARP_DEBUG_DO(SHOW_IP_TO_MAC_LOOKUPS, {printf("Resolving IP (%d): ", device_no); ip_printaddress(ip);});


    if (memcmp(ip,local_ip_broadcast_address,IP_ADDRESS_SIZE) == 0){
	memset( (unsigned char *)mac , 0xff ,6);
        ARP_DEBUG_DO(SHOW_IP_TO_MAC_LOOKUPS, {printf(" HA = "); ethernet_printaddress(mac); printf("\n");});
        ARP_DEBUG_PRINT(ANNOUNCE_EXIT, ("arp_resolve exited normally.\n"));
	return TRUE;
    }
    
    for (i=0;i<3;i++){
	j = arp_lookup(device_no, ip);
	if (j>=0){
	    memcpy((char*) mac, (char*) arp_table[device_no][j].enet_addr, MAC_ADDRESS_SIZE);
            ARP_DEBUG_DO(SHOW_IP_TO_MAC_LOOKUPS, {printf(" HA = "); ethernet_printaddress(mac); printf("\n");});
            ARP_DEBUG_PRINT(ANNOUNCE_EXIT, ("arp_resolve exited normally.\n"));
	    return TRUE;
	}
	arp_query_network(device_no, ip);
    }
    /* repeat the check a fouth time incase the final query worked */
    j = arp_lookup(device_no, ip);
    if (j>=0){
	memcpy((char*) mac, (char*) arp_table[device_no][j].enet_addr, MAC_ADDRESS_SIZE);
        ARP_DEBUG_DO(SHOW_IP_TO_MAC_LOOKUPS, {printf(" HA = "); ethernet_printaddress(mac); printf("\n");});
        ARP_DEBUG_PRINT(ANNOUNCE_EXIT, ("arp_resolve exited normally.\n"));
	return TRUE;
    }
    printf("arp resolve failed\n");
    ARP_DEBUG_PRINT(ANNOUNCE_EXIT, ("arp_resolve exited normally.\n"));
    return FALSE;
}
/* arp_init_module - initialise the module 
 */
void arp_init_module(void)
{
    /* Allocate an initial ARP buffer */
    current_arp_buffer = net_buffer_alloc(ETHER_BUFFER_SIZE);
}

/* arp_show - function to show all the entries in the arp table.
 *
 * Argument:
 *    None.
 *
 * Returned value:
 *    None.
 */
void arp_show(void)
{
  int i, j;
  int atleastone;

  printf("\nArp Table Contents (at 0x%08X):\n", arp_table);
  for (i=0;i<MAX_ETHER_DEVICES;i++) {
/*
 *  Only print out the table contents if there are any.
 */
	atleastone = 0;
	for (j=0;j<ARP_TABLE_SIZE;j++) 
	    if (arp_table[i][j].flags==isvalid) atleastone++;

	if (atleastone > 0) {
	    printf("\n\tEthernet Device %d\n", i);
	    for (j=0;j<ARP_TABLE_SIZE;j++) {
	        if (arp_table[i][j].flags==isvalid) {
		    printf("\tIP Address: ");
		    ip_printaddress(arp_table[i][j].ip_address);
		    printf("\n\tMAC Address: ");
		    ethernet_printaddress(arp_table[i][j].enet_addr);
		    printf("\n");		
	        }
	    }
	}
  }
}


#if 0
/* waits for a packet to be received and dispatches to the correct protocol */
void netwatch()
{
    int cancel_watch = FALSE;
    int device_no = 0;

    if(! netman_monitor_device_started()) netman_start_monitor_device();

    while(!cancel_watch) {
        if (CharAv())	/* If a key has been pressed, then check if it was */
          cancel_watch = (GetChar() == 3);   /* Control-C */
        ethernet_process_one_packet(device_no, current_arp_buffer, &current_arp_buffer);
    }
}
#endif
