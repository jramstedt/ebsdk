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
static char *rcsid = "$Id: tftp.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif

/*
 * This module implements a subset of the tftp protocol.  It will only handle 
 * read requests initiated by this end. The file read is loaded into memory.  
 * 
 * $Log: tftp.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.20  1998/04/06  18:20:50  gries
 * I changed it
 *
 * Revision 1.19  1997/12/19  22:05:42  pbell
 * Fixed a bug found by Jeffw.  Errors occurring before the first
 * data packet will now be recognized.
 *
 * Revision 1.18  1997/05/19  17:18:35  fdh
 * Modified to use sleep functions which are based on the cycle counter
 * to control timing instead of the gettime functions which currently
 * use the RTC.
 *
 * Revision 1.17  1995/11/28  16:17:01  cruz
 * Updated to compute the size of the file being loaded.
 *
 * Revision 1.16  1995/10/20  18:46:46  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.15  1995/10/18  13:59:56  cruz
 * Moved the debug init code to prtrace.c
 *
 * Revision 1.14  1995/10/16  17:03:49  cruz
 * Changed compiled switch name for new debugging code.
 *
 * Revision 1.13  1995/10/13  19:39:39  cruz
 * Removed #include for prtrace.h which is now included in lib.h.
 * Replaced old PRTRACEx statements with new debug macros which
 * can control what gets printed out from the user interface.
 *
 * Revision 1.12  1995/10/10  15:14:00  cruz
 *  Due to an incorrect argument to memcpy, an overwrite of
 * another variable was occurring.
 * Changed code so we can ignore an error message that does
 * not come from our server.  The problem occurred when the
 * server got 2 requests and a connection between the client
 * and the server is established.  The second request may
 * timeout (on the server side) and cause an error message
 * to be sent to the client.  The way the code used to be, the
 * error message would cause the first connection to be
 * aborted, even though, it didn't need to.
 *
 * Revision 1.11  1994/11/18  16:01:27  fdh
 * Modified tftp error message.
 *
 * Revision 1.10  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.9  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.8  1994/06/22  15:11:46  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.7  1994/06/17  19:36:25  fdh
 * Clean-up...
 *
 * Revision 1.6  1994/06/03  20:12:22  fdh
 * Replaced <stdio.h> with <stddef.h>.
 *
 * Revision 1.5  1994/01/20  15:45:47  rusling
 * Fixed bugs:  some memcpy()'s had insufficiant
 * arguments.
 *
 * Revision 1.4  1994/01/19  10:55:52  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.3  1993/11/30  10:14:17  rusling
 * Modified to use net_buff.h and net_buffer_allocate().
 *
 * Revision 1.2  1993/10/01  16:00:05  berent
 * Tolerate send failures
 *
 * Revision 1.1  1993/08/06  10:05:50  berent
 * Initial revision
 *
 *
 */
#include "ether.h"
#include "console.h"    /* Included for prototype of CheckForChar() */

/*
 * Structure of a tftp packet (see rfc 783)
 */
#define TFTPP_OP_CODE_OFFSET 0

/* The rest of the packet varies with the packet type. */

/* 
 * RRQ/WRQ 
 */

#define TFTPP_FILE_OFFSET (TFTPP_OP_CODE_OFFSET+2) 

/* Offset of mode string depends on length of file name */

/*
 * DATA and ACK (ack has no data field)
 */
#define TFTPP_BLOCK_OFFSET (TFTPP_OP_CODE_OFFSET+2)
#define TFTPP_DATA_OFFSET (TFTPP_BLOCK_OFFSET+2)

#define TFTP_MAX_BLOCK_SIZE 512


/*
 * ERROR 
 */
#define TFTPP_ERRORCODE_OFFSET (TFTPP_OP_CODE_OFFSET+2)
#define TFTPP_ERRMSG_OFFSET (TFTPP_ERRORCODE_OFFSET+2)

/* OP codes */
#define TFTP_OP_RRQ   1
#define TFTP_OP_WRQ   2
#define TFTP_OP_DATA  3
#define TFTP_OP_ACK   4
#define TFTP_OP_ERROR 5

/* Error codes; only those used here are defined */
#define TFTP_ERR_UNDEFINED        0
#define TFTP_ERR_ACCESS_VIOLATION 2
#define TFTP_ERR_ILLEGAL_OP       4
#define TFTP_ERR_UNKNOWN_TID      5

/* UDP port used by tftp to connect to server */

udp_port const udp_port_tftp_connect={0,69};

/*
 * Other constants 
 */
#define RETRANSMIT_DELAY 3
#define MAX_RETRANSMIT_COUNT 4

/*
 * Static control data
 */

static unsigned char * next_block_addr; /* Where in memory to put the next block of data */
static int next_block_number; /* The number of the expected next block */
static int load_finished; /* Status flag; load finished (succeeded or failed) */
static int load_successful; /* TRUE if load succeeded; only valid when load finished set */
static ip_addr server_addr; /* IP address of tftp server */
static udp_port server_port; /* UDP port assigned by tftp server, learnt from first data packet */
static udp_port client_port; /* Local temporary UDP port for tftp */
static udp_port not_seen_port = {0,0}; /* used to indicate that a source port is not available */

static unsigned char * current_tftp_buffer; 

/* Data for retransmission */
static char saved_packet_copy[ETHER_BUFFER_SIZE];
static int saved_packet_size;
static udp_port saved_dest_port;
static ip_addr saved_dest_addr;
static int retransmit_timer;
static int retransmit_count;

static int file_size;

#ifdef TRACE_ENABLE

/*
 *  These are the values that the variable tftp_debug can have.
 *  Note that any OR combination of them is allowed, therefore,
 *  ALL of the following defines must be in powers of 2.
 */
#define DISABLED                0
#define PACKET_INFO_IN          (1 << 0)
#define PACKET_INFO_OUT         (1 << 1)
#define ANNOUNCE_ENTRY          (1 << 2)
#define ANNOUNCE_EXIT           (1 << 3)
#define REPORT_ERRORS           (1 << 4)
#define MONITOR_WRONG_SRC_PORT  (1 << 5)
#define MONITOR_TRANSFER_TO_MEM (1 << 6)
#define SHOW_ADDRESSES_ON_INPUT (1 << 7)
#define SHOW_ADDRESSES_ON_OUTPUT (1 << 8)
#define FILE_LOAD_INFO          (1 << 9)
#define ALL                     ((1 << 10) - 1)
/*
 *  List of all the possible values for the variable "udp_debug"
 */
VARIABLE_VALUES tftp_debug_table[] = 
{
    {DISABLED,          "DISABLED"},    /* Always the first entry. */
    {PACKET_INFO_IN,    "PACKET_INFO_IN"},
    {PACKET_INFO_OUT,   "PACKET_INFO_OUT"},
    {ANNOUNCE_ENTRY,    "ANNOUNCE_ENTRY"},
    {ANNOUNCE_EXIT,     "ANNOUNCE_EXIT"},
    {REPORT_ERRORS,     "REPORT_ERRORS"},
    {MONITOR_WRONG_SRC_PORT, "MONITOR_WRONG_SRC_PORT"},
    {MONITOR_TRANSFER_TO_MEM, "MONITOR_TRANSFER_TO_MEM"},
    {SHOW_ADDRESSES_ON_INPUT, "SHOW_ADDRESSES_ON_INPUT"},
    {SHOW_ADDRESSES_ON_OUTPUT, "SHOW_ADDRESSES_ON_OUTPUT"},
    {FILE_LOAD_INFO,    "FILE_LOAD_INFO"},
    {ALL,               "ALL"},
    {0,                 NULL}           /* Always the last entry.  */
};

ui tftp_debug;

#define TFTP_DEBUG_PRINT(cond, str) DEBUG_PRINT(tftp_debug, cond, str)
#define TFTP_DEBUG_DO(cond, str) DEBUG_DO(tftp_debug, cond, str)

#else

#define TFTP_DEBUG_PRINT(cond, str)
#define TFTP_DEBUG_DO(cond, str) 

#endif /* TRACE_ENABLE */

/*
 * Internal functions
 */
/* tftp_retransmit - repeats the last send 
 *
 * Arguments:
 *     device_no     - device to transmit on
 *     frame_buffer  - buffer containing frame
 *     balance_buffer- buffer returned to maintain buffer balance
 */
static void tftp_retransmit(int const device_no,
			    unsigned char frame_buffer[ETHER_BUFFER_SIZE],
			    unsigned char ** const balance_buffer)
{

    TFTP_DEBUG_PRINT(ANNOUNCE_ENTRY, ("tftp_retransmit entered\n"));
    TFTP_DEBUG_DO(SHOW_ADDRESSES_ON_OUTPUT, {printf("Resending TFTP packet to IP: "); ip_printaddress(saved_dest_addr);printf("\n");});    
    TFTP_DEBUG_PRINT(REPORT_ERRORS, ("Retransmitting packet.\n"));

    /* Set up default returned buffer */
    *balance_buffer = frame_buffer;

    /* Check whether I have transmitted too often already */
    if(retransmit_count++ == MAX_RETRANSMIT_COUNT) {
	printf("TFTP error: no response from server\n");
	load_finished = TRUE;
	load_successful = FALSE;
	return;
    }
    /* Copy the saved packet into the buffer */
    memcpy(frame_buffer + udp_data_offset(), saved_packet_copy, saved_packet_size);
    /* And send it */
    /* Send the original */
    if( ! udp_write(device_no,
		    frame_buffer + udp_data_offset(), 
		    saved_dest_port, 
		    saved_dest_addr, 
		    client_port,
		    saved_packet_size, 
		    frame_buffer, 
		    balance_buffer)) {
	/* If the send failed keep the old buffer and leave the time out mechianism to retry */
        TFTP_DEBUG_PRINT(REPORT_ERRORS, ("Retransmit failed.\n"));
	*balance_buffer = frame_buffer;
    }
    /* Remember when we need to repeat it again */
    retransmit_timer = 0;
}
/* tftp_send_packet - sends a single, already built, tftp packet for the first time keeping a copy of it for retrys 
 *
 * Arguments:
 *     device_no     - device on which to transmit the packet
 *     packet        - the packet including the TFTP header
 *     size          - size of the packet
 *     dest_port     - destination UDP port
 *     dest_addr     - destination IP address
 *     frame_buffer  - buffer containing frame
 *     balance_buffer- buffer returned to maintain buffer balance
 *
 * Returned value:
 *     TRUE - send succeeded 
 *     FALSE - send failed
 */
static int tftp_send_packet(int const device_no,
			    unsigned char * const packet,
			    udp_port const dest_port,
			    ip_addr const dest_addr,
			    int const size,
			    unsigned char frame_buffer[ETHER_BUFFER_SIZE],
			    unsigned char ** const balance_buffer)
			    
{
    int result;

    TFTP_DEBUG_PRINT(ANNOUNCE_ENTRY, ("tftp_send_packet entered\n"));
    TFTP_DEBUG_DO(SHOW_ADDRESSES_ON_OUTPUT, {printf("Sending TFTP packet to IP: "); ip_printaddress(dest_addr);printf("\n");});    

    /* Keep a copy */
    memcpy(saved_packet_copy, packet, size);
    saved_packet_size = size;
    memcpy(saved_dest_port, dest_port, UDP_PORT_SIZE);
    memcpy(saved_dest_addr, dest_addr, IP_ADDRESS_SIZE);

    /* Send the original */
    result = udp_write((int)device_no, 
	(unsigned char *)packet, 
	saved_dest_port, 
	saved_dest_addr, 
	client_port, 
	(int)size, 
	frame_buffer, 
	(unsigned char **)balance_buffer) ;

    if (!result) {
	/* If the send failed keep the old buffer and leave the time out mechianism to retry */
        TFTP_DEBUG_PRINT(REPORT_ERRORS, ("Send failed.\n"));
	*balance_buffer = frame_buffer;
    }

	      
    /* Remember when we need to repeat it */
    retransmit_timer = 0;
    
    /* and that we haven't yet repeated it */
    retransmit_count = 0;
    TFTP_DEBUG_PRINT(ANNOUNCE_EXIT, ("tftp_send_packet exited normally\n"));
    return result;
}
/* tftp_send_rrq - sends an rrq message 
 *
 * Arguments:
 *     device_no     - device on which packet should be sent
 *     file_name     - name of file to be read
 *     dest_port     - UDP destination port
 *     dest_addr     - IP destination address
 *     frame_buffer  - buffer containing packet
 *     balance_buffer- buffer returned to maintain buffer balance 
 */
static int tftp_send_rrq(int const device_no,
			 char * const file_name,
			 udp_port const dest_port,
			 ip_addr const dest_addr,
			 unsigned char frame_buffer[ETHER_BUFFER_SIZE],
			 unsigned char ** const balance_buffer)
{
    /* Create the packet */
    unsigned char * const packet=frame_buffer+udp_data_offset();
    
    TFTP_DEBUG_PRINT(PACKET_INFO_OUT,  ("TFTP: DestPort | PcktType | Filename :\nOUT : %8d |    RRQ   | %s :\n",  ntoh(dest_port, UDP_PORT_SIZE), file_name));
    memset(frame_buffer, 0, ETHER_BUFFER_SIZE);

    packet[TFTPP_OP_CODE_OFFSET] = 0;
    packet[TFTPP_OP_CODE_OFFSET+1] = TFTP_OP_RRQ;
    strcpy((char *)packet + TFTPP_FILE_OFFSET, file_name);
    strcpy((char *)packet + TFTPP_FILE_OFFSET + strlen(file_name) + 1, "octet");
    return tftp_send_packet(device_no, 
			    packet, 
			    dest_port,
			    dest_addr,
			    TFTPP_FILE_OFFSET + strlen(file_name) + 7,
			    frame_buffer,
			    balance_buffer);
}
/* tftp_send_ack - sends an ack message 
 *
 * Arguments:
 *     device_no     - device on which packet should be sent
 *     block         - number of block to be acked
 *     dest_port     - UDP destination port
 *     dest_addr     - IP destination address
 *     frame_buffer  - buffer containing packet
 *     balance_buffer- buffer returned to maintain buffer balance 
 */
static int tftp_send_ack(int const device_no,
			 int block,
			 udp_port const dest_port,
			 ip_addr const dest_addr,
			 unsigned char frame_buffer[ETHER_BUFFER_SIZE],
			 unsigned char ** const balance_buffer)
{
    /* Create the packet */
    unsigned char * const packet=frame_buffer+udp_data_offset();

    TFTP_DEBUG_PRINT(PACKET_INFO_OUT,  ("TFTP: DestPort | PcktType | Block # :\nOUT : %8d |    ACK   | %7d :\n",  ntoh(dest_port, UDP_PORT_SIZE), block));
    memset(frame_buffer, 0, ETHER_BUFFER_SIZE);

    packet[TFTPP_OP_CODE_OFFSET] = 0;
    packet[TFTPP_OP_CODE_OFFSET+1] = TFTP_OP_ACK;
    packet[TFTPP_BLOCK_OFFSET] = block >> 8;
    packet[TFTPP_BLOCK_OFFSET+1] = block &0x00ff;

    return tftp_send_packet(device_no, 
			    packet, 
			    dest_port,
			    dest_addr,
			    TFTPP_DATA_OFFSET,
			    frame_buffer,
			    balance_buffer);
}
/* tftp_send_error - sends an rrq message 
 *
 * Arguments:
 *     device_no     - device on which packet should be sent
 *     block         - number of block to be acked
 *     dest_port     - UDP destination port
 *     dest_addr     - IP destination address
 *     frame_buffer  - buffer containing packet
 *     balance_buffer- buffer returned to maintain buffer balance 
 */
static int tftp_send_error(int const device_no,
			   int error_code,
			   char * error_text,
			   udp_port const dest_port,
			   ip_addr const dest_addr,
			   unsigned char frame_buffer[ETHER_BUFFER_SIZE],
			   unsigned char ** const balance_buffer)
{
    /* Create the packet */
    unsigned char * const packet=frame_buffer+udp_data_offset();

    TFTP_DEBUG_PRINT(PACKET_INFO_OUT,  ("TFTP: DestPort | PcktType | Error # | Message :\nOUT : %8d |   ERROR  | %7d | %s :\n",  ntoh(dest_port, UDP_PORT_SIZE), error_code, error_text));
    memset(frame_buffer, 0, ETHER_BUFFER_SIZE);

    packet[TFTPP_OP_CODE_OFFSET] = 0;
    packet[TFTPP_OP_CODE_OFFSET+1] = TFTP_OP_ERROR;
    packet[TFTPP_ERRORCODE_OFFSET] = error_code >> 8;
    packet[TFTPP_ERRORCODE_OFFSET+1] = error_code & 0x00ff;
    strcpy((char *)packet+TFTPP_ERRMSG_OFFSET, error_text);
    return tftp_send_packet(device_no, 
			    packet, 
			    dest_port,
			    dest_addr,
			    TFTPP_ERRMSG_OFFSET+strlen(error_text) + 1,
			    frame_buffer,
			    balance_buffer);
}
/* tftp_input - handles received tftp packets
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
 *     Processes a tftp packet received on the client UDP port. Note that the client port is only registered
 *     as a UDP port when there is a tftp transaction in progress so I can assume the static data is valid.
 */
static void tftp_input(void * protocol_data,
		       int device_no,
		       unsigned char * packet,
		       int size,
		       udp_port source_port,
		       ip_addr source_address,
		       unsigned char frame_buffer[ETHER_BUFFER_SIZE],
		       unsigned char ** balance_buffer)
{
    int received_block_number;

    TFTP_DEBUG_PRINT(ANNOUNCE_ENTRY, ("tftp_input entered\n"));
    TFTP_DEBUG_DO(SHOW_ADDRESSES_ON_INPUT, {printf("TFTP packet from "); ip_printaddress(source_address);printf("\n");});    

    *balance_buffer = frame_buffer;

    /* Check top byte of op code valid (0) */
    if(packet[TFTPP_OP_CODE_OFFSET] != 0) {
	tftp_send_error(device_no,
			TFTP_ERR_ILLEGAL_OP,
			"Illegal TFTP operation", 
			source_port, 
			source_address, 
			frame_buffer, 
			balance_buffer);
	load_finished = TRUE;
	load_successful = FALSE;
	printf("TFTP error: bad or unexpected packet received\n");
	return;
    }

    switch(packet[TFTPP_OP_CODE_OFFSET+1]) {
    case TFTP_OP_DATA:
	/* Check that the packet comes from the right place; I only know the source port after receiving the first block */
        TFTP_DEBUG_PRINT(PACKET_INFO_IN,  ("TFTP: SrcPort | PcktType | Block # :\nIN  : %7d |   DATA   | %7d :\n",  ntoh(source_port, UDP_PORT_SIZE), ntoh(&packet[TFTPP_BLOCK_OFFSET], 2)));

	if(memcmp(source_address, server_addr, IP_ADDRESS_SIZE) != 0 ||
	   (next_block_number != 1 && memcmp(source_port, server_port, UDP_PORT_SIZE) != 0)) {
	    tftp_send_error(device_no,
			    TFTP_ERR_UNKNOWN_TID, 
			    "Unknown TFTP transfer id", 
			    source_port, 
			    source_address, 
			    frame_buffer, 
			    balance_buffer);
	    /* Don't give up; packets from the wrong source port can appear if the RRQ message was duplicated (see rfc 783) */ 
            TFTP_DEBUG_PRINT(MONITOR_WRONG_SRC_PORT,  ("Wrong source port.  Skipping packet.\n"));
            TFTP_DEBUG_PRINT(ANNOUNCE_EXIT, ("tftp_input exited because of wrong source port.\n"));
	    return;
	}
	received_block_number = packet[TFTPP_BLOCK_OFFSET] << 8 | packet[TFTPP_BLOCK_OFFSET+1];

	/* Check the block number */
	if(received_block_number == next_block_number) {
            TFTP_DEBUG_PRINT(MONITOR_TRANSFER_TO_MEM, ("TFTP block #%d of size %d bytes will be copied to memory at 0x%X\n", received_block_number, size - TFTPP_DATA_OFFSET, next_block_addr));
	    /* Expected block; copy it into memory */
	    memcpy(next_block_addr, packet+TFTPP_DATA_OFFSET, size - TFTPP_DATA_OFFSET);
            file_size += size - TFTPP_DATA_OFFSET;
	    
	    /* Check for special case of last block */
	    if(size - TFTPP_DATA_OFFSET != TFTP_MAX_BLOCK_SIZE) {
		load_finished = TRUE;
		load_successful = TRUE;
	    }
	    /* If this is the first block remember the server's port */
	    if(next_block_number == 1) {
                if (memcmp(server_port, not_seen_port, UDP_PORT_SIZE) != 0) {
                    tftp_send_error(device_no,
                                    TFTP_ERR_UNDEFINED,
                                    "Connection already exists", 
                                    source_port, 
                                    source_address, 
                                    frame_buffer, 
                                    balance_buffer);
                }
		memcpy(server_port, source_port, UDP_PORT_SIZE);
	    }
	    tftp_send_ack(device_no,
			  next_block_number,
			  source_port, 
			  source_address, 
			  frame_buffer, 
			  balance_buffer);
	    /* Print a # every 20 blocks to keep the user happy */
	    if(next_block_number % 20 == 0) {
		PutChar('#');
	    }
	    /* And prepare to receive more */
	    next_block_number++;
	    next_block_addr += size - TFTPP_DATA_OFFSET;

            /*  Allow the user to abort by pressing Control-C */
            if (CheckForChar(3)) {
                printf("\nFile load aborted by user.\n");
                load_finished = TRUE;
                load_successful = FALSE;
            }

	}
	else if(received_block_number < next_block_number) {
	    /* Repeat of previous packet; ignore */
            TFTP_DEBUG_PRINT(REPORT_ERRORS, ("A duplicate packet was received.  It will be ignored.\n"));
	}
	else {
	    /* All other values are illegal */
	    tftp_send_error(device_no,
			    TFTP_ERR_ILLEGAL_OP, 
			    "Illegal TFTP operation", 
			    source_port, 
			    source_address, 
			    frame_buffer, 
			    balance_buffer);
	    printf("TFTP error: block received out of sequence\n");
	    load_finished = TRUE;
	    load_successful = FALSE;
	}
	break;
    case TFTP_OP_ERROR:
        TFTP_DEBUG_PRINT(PACKET_INFO_IN,  ("TFTP: SrcPort | PcktType :\nIN  : %7d |  ERROR  :\n",  ntoh(source_port, UDP_PORT_SIZE)));
        /* Ignore errors that are not from our server */
	if(memcmp(source_address, server_addr, IP_ADDRESS_SIZE) != 0 ||
	    ((memcmp(source_port, server_port, UDP_PORT_SIZE) != 0) &&
	     (memcmp(server_port, not_seen_port, UDP_PORT_SIZE) != 0))) {
            TFTP_DEBUG_PRINT(MONITOR_WRONG_SRC_PORT,  ("Wrong source port or server address.  Skipping packet.\n"));
            TFTP_DEBUG_PRINT(ANNOUNCE_EXIT, ("tftp_input exited because of wrong source port.\n"));
            return;
        }
	printf("TFTP error: \"%s\"\n", packet + TFTPP_ERRMSG_OFFSET);
	load_finished = TRUE;
	load_successful = FALSE;
	break;
    default:
	/* RRQs and WRQs should only be received on the connection port (and hence will be ignored), ACKs should
	   never be received during a read request, and all other op codes are illegal  */
	tftp_send_error(device_no,
			TFTP_ERR_ILLEGAL_OP, 
			"Illegal TFTP operation", 
			source_port, 
			source_address, 
			frame_buffer, 
			balance_buffer);
	printf("TFTP error: bad or unexpected packet received\n");
	load_finished = TRUE;
	load_successful = FALSE;
	break;
    }
    TFTP_DEBUG_PRINT(ANNOUNCE_EXIT, ("tftp_input exited normally.\n"));
}

/* tftp_load - load a file from a remote host
 *
 * Arguments:
 *     device_no       - device on which request should be sent
 *     file_name       - name of file to be loaded. Null terminated;
 *     tftp_server_addr- server IP address
 *     load_addr       - address of start of region into which the file should be loaded.
 *     
 * Returned value:
 *     TRUE   - tftp successful.
 *     FALSE  - tftp failed.
 */
int tftp_load(int device_no, 
	      char file_name[128], 
	      ip_addr tftp_server_addr, 
	      unsigned char * load_addr)
{
    TFTP_DEBUG_PRINT(ANNOUNCE_ENTRY, ("tftp_load entered\n"));
    TFTP_DEBUG_DO(FILE_LOAD_INFO,{printf ("Loading file %s on device %d from server: ", file_name, device_no); ip_printaddress(tftp_server_addr); printf("\n");});

    /* Set up static variables for starting load */
    next_block_addr = load_addr;
    next_block_number = 1;
    load_finished = FALSE;
    memcpy(server_addr, tftp_server_addr, IP_ADDRESS_SIZE);
    file_size = 0;

    /* Create a local UDP port for tftp */
    if( !udp_create_port(device_no, tftp_input, NULL, client_port)) {
	TFTP_DEBUG_PRINT(REPORT_ERRORS, ("Failed to create port for TFTP transfer\n"));
        TFTP_DEBUG_PRINT(ANNOUNCE_EXIT, ("tftp_load exited because of port creation failure.\n"));
	return FALSE;
    }

    TFTP_DEBUG_PRINT(FILE_LOAD_INFO, ("TFTP will use UDP port: %d\nSending Read Request.\n", ntoh(client_port, UDP_PORT_SIZE)));

    /* Send the request retrying until the send succeeds; note that this sets retransmit_timer */
    if(! tftp_send_rrq(device_no,
			  file_name,
			  udp_port_tftp_connect,
			  server_addr, 
			  current_tftp_buffer, 
			  &current_tftp_buffer)) {
	printf("TFTP request not sent\n");
	udp_delete_port(device_no, client_port);
	TFTP_DEBUG_PRINT(REPORT_ERRORS, ("Failed to send TFTP Read Request\n"));
        TFTP_DEBUG_PRINT(ANNOUNCE_EXIT, ("tftp_load exited because of failure to send RRQ.\n"));
	return 0;
    }
    memcpy(server_port, not_seen_port, UDP_PORT_SIZE);
    /* 
     * Read and process packets until either the whole file has been received; or a send times out;
     * note that each time a TFTP packet is sent by the received packet processor the retransmit_timer
     * is reset 
     */
    TFTP_DEBUG_PRINT(FILE_LOAD_INFO, ("TFTP Read Request sent.  Waiting for data.\n"));
    while(TRUE) {
        retransmit_timer = 0;
	/* Timeout RETRANSMIT_DELAY*20000*50usec = RETRANSMIT_DELAY in seconds */
	while(!load_finished && (retransmit_timer++ < 20000*RETRANSMIT_DELAY)) {
	  ethernet_process_one_packet(device_no, current_tftp_buffer, &current_tftp_buffer);
	  usleep(50);
	}
	if(load_finished) break;
	/* retransmit time has expired; retransmit the last packet sent */
	tftp_retransmit(device_no, current_tftp_buffer, &current_tftp_buffer);
    }
    /* The load has finished; disconnect the UDP port */
    udp_delete_port(device_no, client_port);

    /* return the result */
    TFTP_DEBUG_PRINT(ANNOUNCE_EXIT, ("tftp_load exited normally.\n"));
    if (load_successful && file_size)
      return file_size;
    else
      return 0;
}
/* tftp_init_module - initialise the module 
 */
void tftp_init_module(void)
{
    /* Allocate an initial BOOTP buffer */
    current_tftp_buffer = net_buffer_alloc(ETHER_BUFFER_SIZE);
}

