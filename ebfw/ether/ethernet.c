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
static char *rcsid = "$Id: ethernet.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif

/*
 * $Log: ethernet.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.11  1997/05/19  15:16:05  fdh
 * Modified ethernet_process_one_packet to return the size of
 * the packet read rather than TRUE or FALSE.
 *
 * Revision 1.10  1996/03/16  20:07:13  fdh
 * Fixed up PRTRACE output.
 *
 * Revision 1.9  1995/11/09  21:42:53  cruz
 * Changed type of a variable.
 *
 * Revision 1.8  1995/10/20  18:43:21  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.7  1995/10/12  18:18:57  cruz
 * Removed include for prtrace.h
 *
 * Revision 1.6  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.5  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.4  1994/06/03  20:05:12  fdh
 * Replaced <stdio.h> with <stddef.h>.
 *
 * Revision 1.3  1994/01/19  10:55:52  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.2  1993/10/01  16:01:20  berent
 * Modified structure of frame reading functions
 *
 * Revision 1.1  1993/08/06  09:53:48  berent
 * Initial revision
 *
 *
 */

#include "ether.h"



/* Define the structure of an ethernet frame header; this is defined through constants to avoid any possible
 * problems with allignment etc.
 */

#define EH_DESTINATION_OFFSET 0
#define EH_SOURCE_OFFSET (EH_DESTINATION_OFFSET+MAC_ADDRESS_SIZE)
#define EH_PROTOCOL_ID_OFFSET (EH_SOURCE_OFFSET+MAC_ADDRESS_SIZE)
#define EH_HEADER_SIZE (EH_PROTOCOL_ID_OFFSET+ETHERNET_PROTOCOL_ID_SIZE)

/* Define the table of registered protocols */
#define MAX_REGISTERED_ETHERNET_PROTOCOLS 20

static struct       
{ 
    int in_use;     
    ethernet_packet_handler processing_function;
    ethernet_protocol_id protocol_id;
    void * protocol_data;
} protocol_table[MAX_ETHER_DEVICES][MAX_REGISTERED_ETHERNET_PROTOCOLS];

/* ethernet_register_protocol - registers an ethernet protocol to be recognised by this module
 *
 * Arguments:
 *
 *    device_no             - device on which the protocol is to be registered
 *    protocol_id           - protocol id to be recognised by this module
 *    processing_function   - function to process received packets; if NULL cancels registration of protocol.
 *    protocol_data         - arbitrary data to be passed back when a packet is received
 */
int ethernet_register_protocol(int device_no,
			       ethernet_protocol_id protocol_id,
			       ethernet_packet_handler processing_function,
			       void * protocol_data)
{
    /* The function scans the table of protocols to see if this protocol is already registered,
     * if it is the entry is updated; if not it puts an entry for the protocol in the first free
     * slot. When a packet is received the table is scanned from begining to end so this should
     * mean that the most commonly used protocols (assumed to be the first ones registered) are
     * found first.
     */
    int first_free_slot = MAX_REGISTERED_ETHERNET_PROTOCOLS; /* Used to keep track of whether their are any free slots
								 and if so where; the value MAX_REGISTERED_PROTOCOLS is
								 used to mean that there are no free slots */
    int current_slot;

    /* Scan from end backwards to simplify setting first_free_slot */
    for(current_slot = MAX_REGISTERED_ETHERNET_PROTOCOLS-1; current_slot >= 0; current_slot--) {
	if(protocol_table[device_no][current_slot].in_use) {
	    if(memcmp(protocol_table[device_no][current_slot].protocol_id, protocol_id, ETHERNET_PROTOCOL_ID_SIZE) == 0) {
		/* Entry already exists; update it */
		if(processing_function == NULL) {
		    /* Remove the entry */
		    protocol_table[device_no][current_slot].in_use = FALSE;
		} else {
		    /* Update the entry */
		    protocol_table[device_no][current_slot].processing_function = processing_function;
		    protocol_table[device_no][current_slot].protocol_data = protocol_data;
		}
		return TRUE;
	    }
	} else {
	    first_free_slot = current_slot;
	}
    }
    /* If this point is reached a new entry is needed */
    if(first_free_slot != MAX_REGISTERED_ETHERNET_PROTOCOLS) {
	/* There is a slot free; fill it in */
	memcpy(protocol_table[device_no][first_free_slot].protocol_id, protocol_id, ETHERNET_PROTOCOL_ID_SIZE);
	protocol_table[device_no][first_free_slot].processing_function = processing_function;
	protocol_table[device_no][first_free_slot].protocol_data = protocol_data;
	protocol_table[device_no][first_free_slot].in_use = TRUE;
	PRTRACE3("New ethernet protocol registered %x,%x\n", protocol_id[0], protocol_id[1]);
	return TRUE;
    }
    else {
	PRTRACE3("Failed to register ethernet protocol %x,%x\n", protocol_id[0], protocol_id[1]);
	return FALSE;
    }
}
    

/* ethernet_process_one_packet - reads and processes one packet from the ethernet
 *
 * Arguments:
 *
 *    device_no - device to poll
 *    input_buffer - free buffer usable by the ethernet module
 *    output_buffer - returned free buffer that may be used by the caller.
 *
 * Return value:
 *    TRUE - packet received and processed
 *    FALSE - nothing to receive.
 *
 * DESCRIPTION:
 *    This function checks whether there are any packets available to be processed on
 *    the ethernet device. If there are it reads and processes one packet. The caller
 *    must give it a buffer to work with (input_buffer) and will be returned a, possibly
 *    different, buffer (output_buffer) Note that this returned buffer does not contain the 
 *    received packet, or any other meaningfull data; it is returned simply to ensure that caller 
 *    does not run short of buffers.  By calling this function the caller gives up ownership
 *    of input_buffer. The caller must not access input_buffer after calling this function.
 *
 */
int ethernet_process_one_packet(int device_no, 
			        unsigned char input_buffer[ETHER_BUFFER_SIZE], 
				unsigned char ** output_buffer )
{
    unsigned char * data_buffer;
    int const count = ether_device_read(device_no, input_buffer, &data_buffer);

    /* Set default for buffer to be returned */
    *output_buffer = data_buffer;

    if(count > 0) {
      ethernet_input(device_no, data_buffer, count, data_buffer, output_buffer);

      /* Packet read, return count whether it was passed on the next layer or
	 not since this tells the caller that there may be more packets */
    }
    return count;
}

/* ethernet_input - processes the packet passed to it as an ethernet frame
 *
 * Arguments:
 *
 *    device_no - device on which frame was received
 *    frame - the frame to be processed
 *    size - size of frame
 *    input_buffer - the buffer containing the frame
 *    balance_buffer - returned free buffer that may be used by the caller.
 *
 */
void ethernet_input(int device_no,
		    unsigned char * frame,
		    int size,
		    unsigned char input_buffer[ETHER_BUFFER_SIZE], 
		    unsigned char ** balance_buffer ) 
{
    /* Search for the protocol id in the table */
    int i;
    mac_addr source;
    int current_entry;

    PRTRACE1("Packet received: destination address ");
    for (i=0;i<MAC_ADDRESS_SIZE;i++) 
      PRTRACE3("%s%02x", (i==0 ? "" : "-"), frame[EH_DESTINATION_OFFSET+i]);
    PRTRACE1("\n");

    for(current_entry = 0; current_entry < MAX_REGISTERED_ETHERNET_PROTOCOLS; current_entry++) {
	if(protocol_table[device_no][current_entry].in_use && 
	   memcmp(protocol_table[device_no][current_entry].protocol_id,
		  frame + EH_PROTOCOL_ID_OFFSET, 
		  ETHERNET_PROTOCOL_ID_SIZE) == 0 ) {
		
	    /* Copy the source address out of the buffer to avoid problems when the buffer is reused */
	    memcpy(source, frame + EH_SOURCE_OFFSET, MAC_ADDRESS_SIZE);

	    /* Pass on the packet by calling the protocol's processing function */
	    (*(protocol_table[device_no][current_entry].processing_function))
	      (protocol_table[device_no][current_entry].protocol_data,
	       device_no,
	       frame + EH_HEADER_SIZE,
	       size - EH_HEADER_SIZE,
	       source,
	       input_buffer,
	       balance_buffer);
		
	    /* And stop scanning the table since there can only be one entry per protocol id */
	    break;
	}
    }
}

/* ethernet_write - sends a packet on an ethernet
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
 *    This function writes an ethernet data packet to the device  To maintain the caller's 
 *    pool of buffers the driver must give the caller an unused buffer (balance_buffer) 
 *    in return for the buffer containing the frame to be transmitted.  
 *
 *    If the send succeeds then frame_buffer must not be accessed by the caller after this
 *    call.  If the send can't be queued then frame_buffer will remain valid and
 *    the returned value of balance_buffer is undefined and must not be used.
 *
 */
int ethernet_write(int device_no, 
		   unsigned char * packet, 
		   mac_addr destination,
		   ethernet_protocol_id protocol_id,
		   int size, 
		   unsigned char frame_buffer[ETHER_BUFFER_SIZE],
		   unsigned char ** balance_buffer)
{
    mac_addr source;

    /* Work out where the start of the ethernet header should be */
    unsigned char * const header_start = packet - EH_HEADER_SIZE;

    /* Check that there is enough space in the buffer for the header */

    if(header_start < frame_buffer) {
	PRTRACE1("Packet won't fit in buffer");
	return FALSE;
    }
    
    /* Get the source address */
    ether_device_get_hw_address(device_no, source);

    /* Set up the header */
    memcpy(header_start + EH_SOURCE_OFFSET, source, MAC_ADDRESS_SIZE);
    memcpy(header_start + EH_DESTINATION_OFFSET, destination, MAC_ADDRESS_SIZE);
    memcpy(header_start + EH_PROTOCOL_ID_OFFSET, protocol_id, ETHERNET_PROTOCOL_ID_SIZE);

    /* And send it */
    return ether_device_write(device_no, header_start, size + EH_HEADER_SIZE, frame_buffer, balance_buffer);
}  

/* ethernet_data_offset - returns the offset of the data in an ethernet frame
 *
 * Return value:
 *    Offset.
 */
int ethernet_data_offset(void)
{
    return EH_HEADER_SIZE;
}
/* ethernet_printpacket - print the contents of an ethernet frame
 *
 * Arguments:
 *    p          - the frame
 *    frame_size - its size
 */
void ethernet_printpacket(unsigned char *p, int frame_size)
{
    int i = 0;
    int j,k;

    printf("packet dump: \n");
    printf("tx %d bytes to ", frame_size);
    for (k=0; k < MAC_ADDRESS_SIZE; k++) printf("%x-",*p++);
    printf(" from ");
    for (k=0; k < MAC_ADDRESS_SIZE; k++) printf("%x-",*p++);

    printf(" protocol");
    for (k=0; k < ETHERNET_PROTOCOL_ID_SIZE; k++) printf("%02x",*p++);
    printf("\n");
   
    for(i= 0; i < frame_size - EH_HEADER_SIZE; i++) {
	printf(" %02x", *(p + i) & 0xff);
	if (((i+1) % 16) == 0) {
	    printf(" ");
	    for (j=i-15; j<=i; j++) {
		char  c = (*(p + j) & 0xff);
		printf("%c",(c >= 0x20 && c < 0x7f) ? c : '.');
	    }
	    printf("\n");
	}
    }
    /* Tidy up the last line if necessary */
    if (((i+1) % 16) != 0) {
	for (j=i; j % 16 != 0; j++) {
	    printf("   ");
	}
	printf(" ");
	for (j=(i/16)*16; j<=i; j++) {
	    char  c = (*(p + j) & 0xff);
	    printf("%c",(c >= 0x20 && c < 0x7f) ? c : '.');
	}
	printf("\n");
    }
}

/* ethernet_printaddress - utility to print an ethernet address 
 *
 * Argument:
 *    address - MAC address to be printed
 */
void ethernet_printaddress(mac_addr address)
{
  printf("%02x-%02x-%02x", address[0], address[1], address[2]);
  printf("-%02x-%02x-%02x", address[3], address[4], address[5]);
}
   
/* ntoh - Converts from network to host 
 */
ui ntoh(unsigned char *array, unsigned int size)
{
    unsigned int i;
    ui value = 0;
    for (i=1; i <= size; i++)
        value |= array[i-1] << (8 * (size - i));
    return value;
}
