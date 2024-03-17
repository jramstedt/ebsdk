/*
 * @DEC_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: protocol.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.5  1994/06/28  20:12:18  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.4  1994/06/03  20:13:14  fdh
 * Clean up #endif
 *
 * Revision 1.3  1994/03/09  12:48:33  berent
 * Moved utility functions to pktutil.c. Fixed for NT compilation.
 * This is now identical to the version used in the OSF/1 server
 *
 * Revision 3.1.2.4  92/06/11  15:27:00  Mike_McNally
 * 	1.2 ULTRIX IFT merge
 * 	[92/06/11  07:21:40  Mike_McNally]
 * 
 * $EndLog$
 */
#ifndef PROTOCOLH
#define PROTOCOLH

/*
 * This file now describes the protocol in terms of offset constants.  C
 * structures are avoided since these are compiler dependent.  Multibyte fields
 * understood as integers by both the server and the client are sent most
 * significant byte first. At present the only such fields are the sequence
 * number, the data count for memory access, and the return value.  All other
 * multibyte fields (including process ids and addresses) are sent in target
 * machine byte order.  Where such a field is wider than the values it holds
 * for a particular targets (e.g. addresses for targets with 4 byte addresses)
 * the data will always be placed at the _target's_ least significant end of
 * the field.
 */

#define DEBUGPROTOCOLVERSIONNUMBER		2

/*
 * Process info; used in reply to connect, connect insist, and load messages
 */

/* Processor type */

#define PROCESSOR_ALPHA 0
#define PROCESSOR_MIPS 1

/* Process type */

#define PROCESS_TYPE_KERNEL 1
#define PROCESS_TYPE_USER 2

/* The process info consists of the processor type followed by the process type */

#define PROCESS_INFO_PROCESSOR_OFFSET 0
#define PROCESS_INFO_PROCESSOR_SIZE 1
#define PROCESS_INFO_TYPE_OFFSET 1
#define PROCESS_INFO_TYPE_SIZE 1


/*
 * Process state; used in reply to probe messages
 */

#define PROCESS_STATE_PROCESS_RUNNING 0
#define PROCESS_STATE_PROCESS_AT_BREAK 1
#define PROCESS_STATE_PROCESS_SUSPENDED 2
#define PROCESS_STATE_PROCESS_TERMINATED 3
#define PROCESS_STATE_PROCESS_LOADING 4
#define PROCESS_STATE_LOAD_FAILED 5

#define PROCESS_STATE_OFFSET 0
#define PROCESS_STATE_SIZE 1

/* The register array is used in the set registers request and the get registers response. 
 * For machine independence the register set is treated simply as an array of bytes
 * the size and order of the registers in this array for each machine is documented
 * elsewhere
 */


#define MAX_REGISTER_ARRAY_SIZE 1024

/*
 * Use the mem_data union element for the read and 
 * write commands.
 */

#define MEM_DATA_ADDRESS_SIZE 8
#define MEM_DATA_ADDRESS_OFFSET 0
#define MEM_DATA_COUNT_SIZE 4 
#define MEM_DATA_COUNT_OFFSET (MEM_DATA_ADDRESS_SIZE+MEM_DATA_ADDRESS_OFFSET)
#define MEM_DATA_CONTENT_OFFSET (MEM_DATA_COUNT_SIZE+MEM_DATA_COUNT_OFFSET)

/*
 * Max amount of data that can be requested in a read
 * or write command.
 */
#define MAXMEMDATA 	1024

/*
 * Use the bpt_data union element for the setbreakpoint,
 * clearbreakpoint, and enumerate breakpoint commands.
 */


#define BPT_DATA_ADDRESS_SIZE 8
#define BPT_DATA_ADDRESS_OFFSET 0

/*
 * Each command may not use each field in the DebugPacket
 * structure.  Any field unused should be zero.  Use of
 * fields is as follows:
 *
 *	command - identifies which remote function is required.
 *		When the server responds to the command he sets the
 *		highest bit of the command to one.  If the get
 *		thread state command is 1, the response to a get
 *		thread state command is 0x8001.
 *
 * 	seqnum - sequence number set by debugger, looked at but
 *		not modified by server.
 *
 *	process_id - identifies the target of an operation.
 *		Not modified by server except by load command.
 *
 *	return_value - modified by server, set to the error value
 *		the call generates (zero is OK).
 */


#define PACKET_PROTOCOL_VERSION_OFFSET 0
#define PACKET_PROTOCOL_VERSION_SIZE 1
#define PACKET_RETRANS_COUNT_OFFSET 1
#define PACKET_RETRANS_COUNT_SIZE 1
#define PACKET_COMMAND_OFFSET 2
#define PACKET_COMMAND_SIZE 2
#define PACKET_SEQNUM_OFFSET 4
#define PACKET_SEQNUM_SIZE 4
#define PACKET_PROCESS_ID_OFFSET 8
#define PACKET_PROCESS_ID_SIZE 4
#define PACKET_RETURN_VALUE_OFFSET 12
#define PACKET_RETURN_VALUE_SIZE 4
#define PACKET_SPECIFIC_DATA_OFFSET 16



/* define a packet size that will fit on Ethernet but big enough to take all possible packets */
#define MAX_PACKET_SIZE 1400

typedef struct {
    unsigned char packet[MAX_PACKET_SIZE];
} DebugPacket;

/*
 * Macros for the packet types (commands).
 */

/*
 *  Unconnected mode commands
 */

/* Load process loads a process by name and connects to it. Returns PID and process info. Servers
 * must implement either load process or connecttoprocess (or both).  
 * The fields of Load process are, in order, client user name, server user name, program name, 
 * number of program arguments, program arguments, file name for standard input 
 * (empty if not redirected), file name for standard output (empty if not redirected),
 * file name for standard error (empty if not redirected).  "Number of program arguments" is a 1 
 * byte unsigned integer; all other fields are null terminated strings.  The response returns the
 * process info. 
 */
#define LOADPROCESS			0x01

/* Connect to process connects to a process by PID if no other debugger is connected to it.
 * Fields are client user name and server user name. Returns process info. 
 */

#define CONNECTTOPROCESS		0x02

/* Connect insist connects to a process by PID throwing off other debuggers. Fields are client
 * user name and server user name. Returns process info.
 */
#define CONNECTTOPROCESSINSIST	        0x03

/* 
 * Connected mode commands; available whether running or stopped
*/

#define CONNECTED			0x80	/* not a command, use for comparisons */

/* Probe process gets the state of a process */
#define PROBEPROCESS			0x81

/* Disconnect from process does what you expect; note that it should fail if disconnecting will */
/* kill the process. The fate of breakpoints after disconnection is server dependent; as is */
/* the behaviour of the the process after disconnection if it is stopped at the time of */
/* disconnection */
#define DISCONNECTFROMPROCESS	        0x82

/* Stop process stops a running process */

#define STOPPROCESS			0x83

/* Kill process kills a process and then disconnects the client from the server.  This should fail */
/* if it can't kill the process. It should be possible either to kill or to disconnect from every */
/* process.  For some processes both may be possible */

#define KILLPROCESS                     0x84

/*
 * Stopped mode commands; only available when connected and stopped
 */
#define STOPPED				0xA0	/* not a command, use for comparisons */

/* Continue a stopped process */
#define CONTINUEPROCESS			0xA1

/* Single step */
#define STEP				0xA2

/* Set breakpoint; takes breakpoint data as argument */
#define SETBREAKPOINT			0xA3

/* Clear breakpoint; ditto */
#define CLEARBREAKPOINT			0xA4

/* Get next breakpoint; ditto; returns updated breakpoint data */
#define GETNEXTBREAKPOINT		0xA5

/* Get registers; returns register set (including pseudo-registers) */
#define GETREGISTERS			0xA6

/* Set registers; sets register set */
#define SETREGISTERS			0xA7

/* Read memory */
#define READ				0xA8

/* Write memory */
#define WRITE				0xA9

/* The packet type of replies is the packet type of the corresponding command 
 * ored with REPLY_MASK (i.e. with the top bit set).
 */
#define REPLY_MASK			0x8000

#define UDP_PORT 0x5407                 /* Old ladebug port */
#define UDP_CONNECT_PORT 410            /* New ladebug port for correct use of UDP ports */

/* 
 * Error codes 
 */

/* Normal reply */
#define REPLY_OK                        0

/* Bad PID specified for any command except load */
#define REPLY_BAD_PID                   1

/* Command can't be acted upon because of lack of resources */
#define REPLY_NO_RESOURCES              2

/* Command can only be used once connected to process; may indicate
   some other client has done a connect to process insist */
#define REPLY_NOT_CONNECTED             3

/* Command can only be used when process is stopped */
#define REPLY_NOT_STOPPED               4

/* The command refers to an invalid or protected address */
#define REPLY_BAD_ADDRESS               5
 
/* Command not implemented */
#define REPLY_NOT_IMPLEMENTED           6

/* Can't load object with given name */
#define REPLY_BAD_LOAD_NAME             7

/* Connect refused because some client is already connected */
#define REPLY_ALREADY_CONNECTED         8

/* Can't disconnect from the process without killing it */
#define REPLY_CANT_DISCONNECT           9

/* Can't kill the process (but should be able to disconnect) */
#define REPLY_CANT_KILL                 10

#endif /* PROTOCOLH */



