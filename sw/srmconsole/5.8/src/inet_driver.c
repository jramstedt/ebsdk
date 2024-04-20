/* file:	inet_driver.c
 *
 * Copyright (C) 1992, 1993 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha Firmware
 *
 *  ABSTRACT:	BOOTP and TFTP driver.
 *
 *      Implements TFTP and BOOTP Internet protocols, plus the pieces of
 *      IP, UDP, and ARP necessary to support them.
 *
 *  AUTHORS:
 *
 *      Charlie DeVane
 *
 *  CREATION DATE:
 *  
 *      10-Jun-1992
 *
 *  MODIFICATION HISTORY:
 *
 *	cjd	10-Jun-1992	Initial version
 *
 *      cjd     23-Jun-1992     bug fix: bootp_receive() dequeues request
 *                              to ignore extra responses after first
 *
 *      dda     05-May-1993     Add printf's to bootp_open and tftp_open
 *                              to clarify bootp/tftp operations.
 *
 *      dda     05-May-1993     Incorporate two bug fixes made by cjd.
 *                              Bug fix #1: init_pidb() must not mark entry
 *                              as inited if fopen(bootp) fails.
 *                              Bug fix #2: tftp_open() must check to be
 *                              certain that init_picb() succeeded
 *
 *      L Kopp  25-Jan-1995     Add patch to change hardware type from
 *                              HTYPE_ETHER10 to HTYPE_IEEE8025 for bootp
 *                              and ARP protocols with token ring dwa0 etc.
 *
 *
 *
 *
 *	MAR	30-Oct-1995	Update bootp behaviorto conform to rfc1542.
 *				In tftp_open() code was expanded with:
 *				- check if client ip address is on a different
 *				subnet as server
 *				-  parse the vendor information area for an ip-mask
 *				and for ip-router addresses. *				
 *				A global variable gateway_used was added to arp_resolve
 *                              the server with the proper ip address -- the address
 *				of the server if on the same subnet or the address of
 *				a router of on a different subnet and the router ip
 *				address supplied from the vendor information area
 *
 *
 * 	MAR	3-Nov-1995	At the request of Farrell Woods, update bootp
 *                              function to arp_resolve the giaddr when no ip
 *                              address is found in the vendor info area, if this
 *                              fails then arp_resolve the siaddr, if this fails
 *                              then return bootp failed.
 *
 * 	MAR	26-Feb-1996	Initialize the pidb each time bootp is invoked 
 *                              from the console to ensure each invocation of bootp 
 *				from the console is a 2-stage operation -- a broadcast 
 *				followed by a tftp -- rather than only a tftp-stage 
 *				on subsequent invocations of bootp from the console. 
 *				This change satisfies QAR 46361 in the AFW_QAR notes file.
 *
 *	jje	16-may-1996 	Initialize gateway_used to 0 near the top of 
 *				tftp_open() to ensure that when pidb is inited
 *				via bootp, the bootp request packet goes out
 *				as a normal broadcast (see net_send()).
 *				Also, initialized tag to 0 to ensure that the
 *				WHILE loop in tftp_open() that parses the
 *				subnet mask from bootp vendor data area makes
 *				at least one iteration.  Clear arp_cache on 
 *  				tftp_open to force ARP so that server knows 
 * 				client when bootp skipped via nvram init.
 *				Changed logic used to locate server locally
 *				or behind a gateway.  Cleared bootp database
 *				on tftp_open() to ensure old vendor data is
 *				not used; needed for nvram init.  Added 
 *				xxxx_def_subnetmask to provide subnet mask on
 *				nvram inited tftp.  Removed "default" netmask
 *				and instead derive netmask from yiaddr and OR
 *				in subnet mask if given in bootp reply (or 
 *				xxxx_def_subnetmask for nvram init). 
 *
 *	jje	5-June-1996 	Use inetaddr_validate for validating 
 *				ewx0_def_subnetmask ev.  No longer OR supplied
 *				subnet mask with class network mask.  If a 
 *				subnet mask is supplied in bootp reply or in 
 *				ewx0_def_subnetmask, then just use it verbatim. 
 *--
 */

/* 
 * 
 * INTRODUCTION
 *
 *
 * This module implements TFTP and BOOTP clients as protocol drivers for the
 * Alpha console firmware to support network bootstrapping in an Internet
 * environment. Supporting these TFTP and BOOTP requires pieces of UDP, IP and
 * ARP.
 *
 * An important point to note is that Internet booting is a two-stage operation.
 * First, BOOTP provides the client with information needed to obtain an
 * image.  Then, the client uses a second protocol: TFTP, to obtain the
 * image.  Both BOOTP and TFTP use UDP (User Datagram Protocol) as the primary
 * transport mechanism to send datagrams to other application programs.
 *
 * BOOTP
 * -----
 * BOOTP is a standard protocol in the TCP/IP suite. It operates in the
 * client-server paradigm and requires only a single packet exchange.  
 * The machine that sends the BOOTP request is the client and any machine 
 * that sends a reply is the server.  A 300 byte database in the same format 
 * as the BOOTP message is used to store the received packet.  Once a BOOTP 
 * packet is broadcast and received, the database is marked as inited thus 
 * ending the first-stage of the Inet Booting operation.
 *
 * The BOOTP database structure, <emphasis>(bootp_pkt_t) specified in
 * inet.h , has the following format:
 *
 * BOOTP MESSAGE FORMAT
 * --------------------
 *
 *        client = 0 used by gateways
 *       /         hardware address length
 *      |        /        hardware address type
 *      |        |       /         1 = bootrequest, 2 = bootreply
 *      |        |       |        /
 *  31  |        |       |        |   0
 *  +-------+--------+--------+--------+
 *  |  HOPS |  HLEN  | HTYPE  |  OP    |
 *  +-------+--------+--------+--------+
 *  |          TRANSACTION ID          |
 *  +----------------+-----------------+
 *  |    UNUSED      |  SECONDS        | seconds since client started booting
 *  +----------------+-----------------+
 *  |         CLIENT IP ADDRESS        | your address
 *  +----------------------------------+
 *  |          YOUR IP ADDRESS         |
 *  +----------------------------------+
 *  |         SERVER IP ADDRESS        |
 *  +----------------------------------+
 *  |        GATEWAY IP ADDRESS        |
 *  +----------------------------------+
 *  |CLIENT HARDWARE ADDRESS (16 bytes)|
 *  |                   .              |
 *  |                   .              |
 *  +----------------------------------+
 *  |  SERVER HOST NAME (64 bytes)     |
 *  |                   .              |null term string
 *  |                   .              |
 *  +----------------------------------+
 *  |  BOOT FILE NAME (128 bytes)      |
 *  |                   .              |null term string
 *  |                   .              |
 *  +----------------------------------+
 *  |  VENDOR-SPECIFIC AREA (64 bytes) |
 *  |                   .              |
 *  |                   .              |
 *  +----------------------------------+
 *
 * TFTP Protocol
 * -------------
 * The second stage of Inet booting uses TFTP protocols to get the memory 
 * image.  This protocol simply takes the information in the bootp packet 
 * (or uses a filename specified in the command string or boot_file) and gets 
 * the file from the server.
 *
 * The first packet sent requests a file transfer and establishes the connection
 * between client and server.  The packet specifies a file name and whether the 
 * file will be read (transferred to the client) or written (we do not 
 * currently support writes). 
 *
 * TFTP depends only on the unreliable, connectionless datagram delivery service
 * (UDP).
 *
 * TFTP accepts one parameter: the host address concatenated to the file name of
 * the remote file to be read.
 *
 * Internet Booting Hierarchy
 * --------------------------
 * The following prioritized list shows the different ways of Internet Booting
 * from an initialized system.
 *
 * 1. Filename specified as named boot;  e.g. boot -file "filename" eza0.  
 * 
 *  If the filename includes a "/", it must be specified as "//".  Here's an 
 *  example:
 *            >>> boot -file //var//adm//ris//ris0.alpha//vmunix eza0
 * Note:
 *   This format is only used when specifying the named boot via 
 *   "-file filename" or loading the filename into the environment variable, 
 *    "boot_file".
 *
 * 2. Filename placed in environment variable "boot_file".  
 *
 *    This really operates as a named boot, the only difference being that the 
 *    file name has is taken from "boot_file". For ex.
 *
 *       >>> set boot_file //var//adm//ris//ris0.alpha//vmunix 
 *       >>> boot eza0
 *
 * 3.  Filename placed in ev "eza0_inetfile".
 *
 *     Only TFTP protocol is used in this case; the bootp pkt must have been 
 *     inited.  When a file name has been placed in this environment variable, 
 *     only the second stage of an Inet boot will occur.  All other fields of 
 *     the bootp packet should contain valid information; either from a 
 *     previous Inet boot or via manual loading.
 *
 * 4.  Filename placed in ev "eza0_bootp_file". 
 *
 *     The file name specified in this environ variable will become the filename
 *     specified in the outgoing bootp request packet.  For example:
 *
 *      >>> set eza0_bootp_file /var/adm/ris/ris0.alpha/vmunix.old
 *
 * 5.  Filename not specified - e.g "boot eza0". 
 *
 *      None of the above environment variables are written, so a 2-stage boot 
 *      occurs (any server that receives the request will reply)
 *
 *
 * A complete description of the Internet protocols is beyond the scope of
 * this document.  An excellent description may be found in Douglas Comer
 * "Internetworking with TCP/IP, Vol I, Principles, Protocols and
 * Architecture", second edition, Prentice Hall.
 *
 * Note that behavior of this firmware depends in part upon behavior of the
 * software running on the server host, which varies from server to
 * server.  For example, the exact format of the file specification used with
 * TFTP depends on the server: the Ultrix TFTP server requires a partial path
 * name, and the OSF server requires a complete path name.  Unix systems
 * frequently name the TFTP server "tftpd" and the BOOTP server "bootpd"; see
 * the appropriate system documentation for server details.
 *
 * SETUP
 *
 * For bootp and tftp to operate reliably, several network parameters,
 * contained in environment variables, must be properly configured.  The
 * Internet protocols are robust and thus may work intermittently if the
 * parameters are misconfigured, which can make debugging a misconfiguration
 * difficult.  So here's what you need to know to get the inet software
 * working.  Note that each network interface has a complete set of variables
 * to itself and so each variable is prefixed with the name of the interface.
 * The following discussion uses "eza0" for specificity; you should substitute
 * for "eza0" the name of the actual interface being configured.
 *
 * The variable "eza0_protocols" should include the string "BOOTP" to enable
 * BOOTP, TFTP, etc.  (The variable may also include "MOP" to enable MOP.)
 * Leaving the variable empty or including both strings will enable all
 * protocols (currently just inet and decnet).  In particular, if not enabled
 * the inet software will not be invoked for booting.  Also, the network
 * driver may not enable reception of broadcast packets, which breaks ARP.
 *
 * Each interface has a small database of information required to operate the
 * inet software on that interface.  Internally the database is kept in a 300
 * byte structure having the same format as a BOOTP packet.  This database can
 * be directly read and written in binary form through the BOOTP protocol
 * driver; more on that later.  The four most important fields of the database
 * can be accessed in a friendlier fashion through the environment variables
 * "eza0_inetaddr", "eza0_sinetaddr", "eza0_ginetaddr", "eza0_inetfile".  The
 * first three are the Internet addresses for the interface (eza0), the remote
 * server host, and remote gateway host, respectively.  These variables use
 * Internet standard dotted decimal notation; e.g., "16.123.16.53".
 * "Eza0_inetfile" contains a file to be booted and is formatted simply as a
 * string.
 *
 * The most important of these four is the local address, "eza0_inetaddr".
 * TFTP and ARP will not operate properly without the correct address.
 * "Eza0_ginetaddr" is the address of an Internet gateway on the local
 * network.  TFTP cannot communicate beyond the local network if this gateway
 * address is not correct.  "Eza0_sinetaddr" is the address of a server, which
 * may or may not be on the local network.  Ordinarily this is the server from
 * which to boot.  This is the default remote host contacted by TFTP.
 * "Eza0_inetfile" is ordinarily the file to be booted.  This should
 * be a fully qualified file name, according to whatever rules are specified
 * by the TFTP server on the remote host. This is the default file name
 * requested by TFTP.
 *
 * The interface database must be initialized somehow before TFTP can be used.
 * The database can be initialized by manually setting the four database
 * variables, by explicitly invoking BOOTP, or automatically on the first
 * invocation of TFTP.  Whether initialization occurs on the first TFTP
 * depends on whether the database has been marked as initialized.  The
 * database will be marked as initialized on the first occurrence of any of
 * three events: the invocation of TFTP, the invocation of BOOTP, or the
 * setting of any of the four database environment variables.
 *
 * The most common case is the invocation of TFTP.  When TFTP is invoked and
 * the database has not been marked initialized then the database will be
 * automatically initialized by one of two methods, as specified by the
 * environment variable "eza0_inet_init".  If "eza0_inet_init" is set to
 * "bootp" (the default) the BOOTP protocol driver will be invoked to
 * initialize the database by broadcasting a BOOTP request and storing the
 * response in the database.  If "eza0_inet_init" is set to "nvram" then the
 * database will be initialized by copying the contents of four nonvolatile
 * default variables into the four database variables.  The four nonvolatile
 * default variables are "eza0_def_inetaddr", "eza0_def_sinetaddr",
 * "eza0_def_ginetaddr" and "eza0_def_inetfile".  These variables obviously
 * must be set in advance, for example:
 *
 * 	set eza0_def_inetaddr 16.123.16.53
 * 	set eza0_def_sinetaddr 16.123.16.242
 * 	set eza0_def_ginetaddr 16.123.16.242
 * 	set eza0_def_inetfile bootfiles/vmunix
 * 	set eza0_inet_init nvram
 *
 * When BOOTP is invoked (either explicitly or via the automatic
 * initialization discussed above) the database is marked as initialized.  In
 * the usual case where BOOTP is successfully invoked without the
 * "nobroadcast" parameter (ie, as "bootp/eza0" or "bootp:broadcast/eza0") the
 * received reply packet is copied into the database, thus initializing it.
 * If the "nobroadcast" parameter is specified (ie, "bootp:nobroadcast/eza0")
 * then no request is broadcast and thus no reply is received to copy into the
 * database.  However, the database is still marked initialized, so a
 * following TFTP will not automatically initialize the database.
 *
 * When one of the four database environment variables is set the database is
 * marked as initialized.  Thus a following TFTP will not automatically
 * initialize the database, regardless of whether the environment variables
 * were set to sensible values.
 *
 * TFTP, BOOTP, and ARP all use retransmission to improve robustness.  If an
 * initial transmission is not answered appropriately, the protocol software
 * will retransmit.  Each protocol has an environment variable which controls
 * the number of retries before giving up.  The variables are named
 * "eza0_arp_tries", "eza0_bootp_tries", and "eza0_tftp_tries".  The default
 * value of these is 3, which translates to an average of 12 seconds before
 * failing (see the discussion of retransmission timing below).  If the value
 * of one of these variables is less than 1, the protocol will fail
 * immediately.  Machines located on very busy networks or associated with
 * heavily loaded servers may need these variables set higher.
 *
 * The retransmission algorithms use a randomized exponential backoff delay.
 * If the first try fails a second try will occur about 4 seconds later.  A
 * third try would come after another 8 seconds, a fourth after 16 seconds,
 * and so forth up to 64 seconds.  These times are actually averages, however,
 * since random jitter of about +/- 50% is added to each delay.  This implies
 * that with "eza0_arp_tries" set to 3 ARP will fail if it doesn't get a
 * response within about 12 seconds on average, but the actual timeout will be
 * somewhere between 6 and 18 seconds.
 *
 * USE
 * ---
 * The normal use of BOOTP and TFTP is for bootstrapping across a network.
 * However, they may be explicitly invoked as protocol drivers.  The bootp and
 * tftp protocols must be followed by a network in the protocol tower.
 *
 * Reading from the bootp protocol driver reads the 300 byte binary database
 * associated with the network.  If the parameter "broadcast" is specified, or
 * if no parameter is specified, the protocol will first broadcast a BOOTP
 * request and update the data base with the reply.  If the parameter
 * "nobroadcast" is specified then the broadcast and update are omitted, and
 * the existing content of the database is read.  Writing to the protocol will
 * overwrite the database.  (When manipulating the database by hand it is
 * usually easier to use the database environment variables.) Some examples
 * follow.  Note that the "hd" (hex dump) command is used to display the
 * binary data.
 *
 * Examine the current contents of eza0's database:
 * 	hd bootp:nobroadcast/eza0
 * Update the database from a BOOTP server and examine the result:
 * 	hd bootp/eza0
 * Update the database from a BOOTP server but don't look at the result
 * 	cat bootp/eza0 >nl
 * Copy the contents of (binary) file foo into eza0's database:
 * 	cat foo >bootp/eza0
 *
 * When a BOOTP request is broadcast, the environment variable
 * "eza0_bootp_server" is copied into the "sname" field of the request packet
 * and the variable "eza0_bootp_file" is copied into the "file" field of the
 * request packet.  The exact interpretation of these fields depends on the
 * BOOTP server.  The "sname" field should be the name of a specific host
 * which the local machine wants to boot from.  If it doesn't matter which
 * server answers, then the variable "eza0_bootp_server" should be left empty.
 * The server should use the "file" field in the request to decide which boot
 * file to specify in the response.  For example, the client could supply a
 * generic name like "unix" or "lat", and the server would respond with the
 * fully qualified file path to be used with TFTP.  If a machine will always
 * be booting the same file then "eza0_bootp_file" can be left empty.
 *
 * The tftp protocol driver is used to read files across the network.  Writes
 * are currently unimplimented. Tftp accepts one parameter, the host address
 * concatenated to the file name of the remote file to be read.  The host
 * address is specified in dotted decimal notation and is separated from the
 * file name by ':'.  If the file name includes '/' they must be doubled to
 * '//'.  The following example displays the file '/usr/foo/bar' from the host
 * whose address is 16.123.16.242:
 *
 * 	cat tftp:16.123.16.242://usr//foo//bar/eza0
 *
 * For convenience the address could be saved in an environment variable:
 *
 * 	set ktrose 16.123.16.242
 * 	cat tftp:$ktrose://usr//foo//bar/eza0
 *
 * If no parameter is specified tftp uses the file name and server address
 * from the interface database (ie, "eza0_sinetaddr" and "eza0_inetfile").
 *
 * Note that when booting with tftp, the boot command passes the contents of
 * the environment variable "boot_file" as the parameter to tftp.  If
 * "boot_file" does not have the correct format tftp will fail.  The most
 * common use is probably to leave "boot_file" empty in which case tftp will
 * default to using "eza0_sinetaddr" and "eza0_inetfile", as above.
 *
 *   ENVIRONMENT VARIABLE(S):
 *
 *
 *  ez*0_protocols - NV :
 *	Determines which network protocols are enabled for booting and other
 *	functions.  Legal values include "BOOTP", "MOP", and "BOOTP,MOP".  A
 *	null value is equivalent to "BOOTP,MOP".
 *
 *  ez*0_inet_init - NV :
 *	Determines whether the interface's internal internet database is
 *	initialized from nvram or from a network server (via the BOOTP
 *	protocol).  Legal values are "nvram" and "bootp"; default is
 *	"bootp".
 *
 * ez*0_inetaddr - DY : Sets the interface's internet address by accessing
 *	the local address field of the interface's internal internet
 *	database.
 *
 *  ez*0_sinetaddr - DY :
 *	Accesses the server address field of the interface's internal
 *	internet database.  This is normally the address of the BOOTP and
 *	TFTP server. This variable supplies the default remote address for
 *	TFTP transactions.
 *
 *  ez*0_inetfile - DY :
 *	Accesses the file name field of the interface's internal internet
 *	database.  This is normally the file to be booted from the TFTP
 *	server. This variable supplies the default remote file name for TFTP
 *	transactions.
 *
 *  ez*0_ginetaddr - DY :
 *	Accesses the gateway address field of the interface's internal
 *	internet database.  This is normally the address of the local
 *	network's gateway to other networks.
 *
 *  ez*0_def_inetaddr - NV :
 *	Supplies the initial value for ez*0_inetaddr when the interface's
 *	internal internet database is initialized from nvram (ie,
 *	ez*0_inet_init is set to "nvram").
 *
 *  ez*0_def_sinetaddr - NV :
 *	Supplies the initial value for ez*0_sinetaddr when the interface's
 *	internal internet database is initialized from nvram (ie,
 *	ez*0_inet_init is set to "nvram").
 *
 *  ez*0_def_ginetaddr - NV :
 *	Supplies the initial value for ez*0_ginetaddr when the interface's
 *	internal internet database is initialized from nvram (ie,
 *	ez*0_inet_init is set to "nvram").
 *
 *  ez*0_def_inetfile - NV :
 *	Supplies the initial value for ez*0_inetfile when the interface's
 *	internal internet database is initialized from nvram (ie,
 *	ez*0_inet_init is set to "nvram").
 *
 *  ez*0_bootp_server - NV :
 *	Supplies the server name to be included in a BOOTP request.  This
 *	can be set to the name of the server from which the machine is to be
 *	booted, or can be left empty.
 *
 *  ez*0_bootp_file - NV :
 *	Supplies the generic file name to be included in a BOOTP request.
 *	The BOOTP server will return a fully qualified file name for
 *	booting. This can be left empty.
 *
 *  ez*0_arp_tries - NV :
 *	Sets the number of transmissions that are attempted before the ARP
 *	protocol fails.  Values less than 1 cause the protocol to fail
 *	immediately.  Default value is 3, which translates to an average of
 *	12 seconds before failing.  Interfaces on busy networks may need
 *	higher values.
 *
 *  ez*0_bootp_tries - NV :
 *	Sets the number of transmissions that are attempted before the BOOTP
 *	protocol fails.  Values less than 1 cause the protocol to fail
 *	immediately.  Default value is 3, which translates to an average of
 *	12 seconds before failing.  Interfaces on busy networks may need
 *	higher values.
 *
 *  ez*0_tftp_tries - NV :
 *	Sets the number of transmissions that are attempted before the TFTP
 *	protocol fails.  Values less than 1 cause the protocol to fail
 *	immediately.  Default value is 3, which translates to an average of
 *	12 seconds before failing.  Interfaces on busy networks may need
 *	higher values.
 *
 */
#define NETTRACE        0
#define IPTRACE         0
#define UDPTRACE        0
#define BOOTPTRACE      0
#define TFTPTRACE       0
#define TFTP_READ_TRACE 0
#define ARPTRACE        0
#define CKSUMTRACE      0
#define OPEN_NET_TRACE  0
#define MALLOCTRACE     0
#define EVTRACE         0

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:inet.h"
#include "cp$src:prdef.h"
#include "cp$src:dynamic_def.h"                                                                 
#include "cp$src:msg_def.h"
#include "cp$src:ni_env.h"
#include "cp$src:ni_dl_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:ctype.h"            /* for isdigit(), isspace() */
#include "cp$inc:kernel_entry.h"

#define __INET__	1
protoif(__INET__)
#include "cp$inc:prototypes.h"

#if MODULAR
extern struct QUEUE pidbq;            /* per-interface database */
extern struct QUEUE bootpq;           /* list of open bootps */
extern struct QUEUE tftpq;            /* list of open tftps */
extern struct SEMAPHORE inet_lock;
extern struct TIMERQ arp_tq;
extern struct SEMAPHORE arp_lock;
extern unsigned short bootp_htype;  /* used in bootp packet, patching hard-wired ethernet */
#endif

/*
 * global variables
 */
#if !MODULAR
struct QUEUE pidbq;            /* per-interface database */
struct QUEUE bootpq;           /* list of open bootps */
struct QUEUE tftpq;            /* list of open tftps */
struct SEMAPHORE inet_lock;
struct TIMERQ arp_tq;
struct SEMAPHORE arp_lock;
unsigned short bootp_htype;  /* used in bootp packet, patching hard-wired ethernet */
#endif

long gateway_used;

/* Indicates that bootp step was skipped and that bootp info area was 
** initialized from nvram variables.  
*/
int nvram_bootp_init; 


int inet_random_seed;
arp_cache_t arp_cache[1];

extern unsigned int	_align ( QUADWORD ) poweron_msec[2];

extern int ev_initing;

char inet_eb[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

/*
 * helpful shortcuts
 */
#define isanet(fp) ((fp)->ip->dva->net_device)
#define freepkt( p,fp ) ndl_deallocate_msg( ( fp )->ip, p, NDL$K_VERSION_3, \
					    NDL$K_DONT_INC_LENGTH )
#undef trace
#define trace( a, b ) ( ( b ) ? pprintf( a ) : 1 )
#define BOOTP_RETRY_FOREVER -1

#if MALLOCTRACE
void *mymalloc( int size );
void *myfree( void *t );

void *mymalloc( int size )
{
  void *t = dyn$_malloc ( size, DYN$K_SYNC|DYN$K_NOOWN );
  pprintf( "malloc %08x %d\n", t, size );
  return t;
}
void *myfree( void *t )
{
  pprintf( "free %08x\n", t );
  free ( t );
}
#define malloc(x) mymalloc(x)
#define free(x) myfree(x)
#else
#define malloc( x ) dyn$_malloc ( x, DYN$K_SYNC|DYN$K_NOOWN )
#endif

#define new( T ) ( T* )malloc( sizeof( T ) )

int null_procedure ();

/*
 * local function prototypes, listed by topic
 */

/*
 * driver init
 */
int inet_init( void );
void arp_init( void );

#if !MODULAR
int bootp_init( void );
int tftp_init( void );

/*
 * bootp driver interface
 */
int bootp_open( struct FILE *fp, char *info, char *next, char *mode );
int bootp_close( struct FILE *fp );
int bootp_read( struct FILE *fp, int size, int number, unsigned char *c );
int bootp_write( struct FILE *fp, int size, int number, unsigned char *c );

struct DDB bootp_ddb = {
	"bootp",			/* how this routine wants to be called	*/
	bootp_read,		/* read routine				*/
	bootp_write,		/* write routine			*/
	bootp_open,		/* open routine				*/
	bootp_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	1,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};

/*
 * tftp driver interface
 */
int tftp_open( struct FILE *fp, char *info, char *next, char *mode );
int tftp_close( struct FILE *fp );
int tftp_read( struct FILE *fp, int size, int number, unsigned char *c );
int tftp_ack( struct FILE *fp );
int tftp_write( struct FILE *fp, int size, int number, unsigned char *c );

struct DDB tftp_ddb = {
	"tftp",			/* how this routine wants to be called	*/
	tftp_read,		/* read routine				*/
	tftp_write,		/* write routine			*/
	tftp_open,		/* open routine				*/
	tftp_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	1,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};

#endif		/* !MODULAR */

/*
 * network transmission path
 */
int udp_send( void *pkt,           /* pointer to message */
	     int size,             /* size of message */
	     unsigned int iaddr,   /* destination ip address */
	     int src,              /* source port - net order */
	     int dst,              /* destination port - net order */
	     struct FILE *net );
int udp_sendv( iovec_t *iovec,      /* iovec containing packet */
	     unsigned int iaddr,   /* destination ip address */
	     int src,              /* source port - net order */
	     int dst,              /* destination port - net order */
	     struct FILE *net );
int ip_sendv( iovec_t *iovec,     /* iovec containing packet */
	    unsigned int iaddr,   /* destination ip address */
	    int protocol,         /* source protocol (eg, UDP ) */
	    struct FILE *net );   /* interface to send over */
void ip_route( unsigned int *iaddr, struct FILE *net );
int net_send( iovec_t *iovec,      /* iovec containing packet */
	      unsigned int iaddr,   /* destination ip address */
	      int protocol,         /* source protocol -- ip, arp, etc */
	      struct FILE *net );

int add_iovec( iovec_t *iovec, void *base, int len );
void *del_iovec( iovec_t *iovec );
int size_iovec( iovec_t *iovec );

/*
 * network receive path
 */
void ip_receive( struct FILE *fp, unsigned char *msg, 
		unsigned char *src, int size, int version );
void udp_receive( struct FILE *fp, char *msg, int size, unsigned short cksum,
		 char *pkt );
void bootp_receive( struct FILE *fp, char *msg, int size, char *pkt );
void tftp_receive( struct FILE *fp, unsigned char *msg, int size, 
		  unsigned short sport, unsigned short dport, char *pkt );

/*
 * arp code
 */
int arp_resolve( long iaddr, char *haddr, int hlen, struct FILE *net );
void arp_receive( struct FILE *fp, unsigned char *msg, 
		      unsigned char *src, int size, int version );

/*
 * network and per-interface-data-base
 */
struct FILE *open_net( char *next );
pidb_t *make_pidb( struct FILE *net );
void init_pidb( struct FILE *net );
pidb_t *lookup_pidb( struct FILE *net );
unsigned int hostid( struct FILE *net );
unsigned int gateid( struct FILE *net );

/*
 * sleep, timing, backoffs
 */
int inet_sleep( struct TIMERQ *tq, int secs );
unsigned long inet_time( unsigned long start );
int inet_backoff( int *backoff );
int inet_init_backoff( int *backoff );

/*
 * environment variables 
 */
#if !MODULAR
int inet_set_env( char *name );
#endif
int inetaddr_validate( char **dda );
int inetaddr_init( char *dda );
int inetaddr_rd( char *name, struct EVNODE *evp, struct EVNODE *evp_copy );
int inetaddr_wr( char *name, struct EVNODE *evp );
int string2inetaddr( unsigned long *addr, char *s );
int inetaddr2string( unsigned long addr, char *s );
int inet_protocols_validate( char **s );

/*
 * miscellaneous
 */
int inet_random( void );
unsigned short inet_cksum( unsigned short *buf, int bufsize );
void hexdump(void *p, int size);
int parse_inet_filename( char *string, unsigned long *remote_addr, char *filename );

/*
** Local macros ...
*/

/*
**	class_mask - returns the network mask for an internet address
**	based on the address class:
**
**			range 				   mask
**	Class A:   0.0.0.0 - 127.255.255.255    	000000FF
**	Class B:   128.0.0.0 - 191.255.255.255		0000FFFF
**	Class C:   192.0.0.0 - 223.255.255.255		00FFFFFF
**
**	WARNING: assumes ia is valid
**
*/
#define class_mask(ia) (			\
	(((ia) & 0x000000C0) == 0x000000C0) ? 	\
	    (0x00FFFFFF) : 			\
	((ia) & 0x00000080) ?			\
	    (0x0000FFFF) : (0x000000FF)		\
	)


int inet_init( void )
{

  bootp_init();
  bootpq.flink = &bootpq;
  bootpq.blink = &bootpq;
  tftp_init();
  tftpq.flink = &tftpq;
  tftpq.blink = &tftpq;

  arp_init();
  inet_random_seed = poweron_msec[0];
  krn$_seminit( &inet_lock, 1, "inet_lock" );
  pidbq.flink = &pidbq;
  pidbq.blink = &pidbq;

  gateway_used = 0;

  return msg_success;
} /* inet_init() */

#if !MODULAR
int bootp_init( void )
{
  struct INODE *inode;
  
  allocinode( bootp_ddb.name, 1, &inode );
  inode->dva = & bootp_ddb;
  inode->attr = ATTR$M_READ|ATTR$M_WRITE;
  INODE_UNLOCK (inode);
  bootpq.flink = &bootpq;
  bootpq.blink = &bootpq;
  return msg_success;
} /* bootp_init() */

int tftp_init( void )
{
  struct INODE *inode;
  
  allocinode( tftp_ddb.name, 1, &inode );
  inode->dva = & tftp_ddb;
  inode->attr = ATTR$M_READ|ATTR$M_WRITE;
  INODE_UNLOCK (inode);
  tftpq.flink = &tftpq;
  tftpq.blink = &tftpq;

  return msg_success;
} /* tftp_init() */
#endif			/* !MODULAR */

void arp_init( void )
{
  krn$_seminit( &arp_lock, 1, "arp_lock" );
  krn$_seminit( &arp_tq.sem, 0, "arp_sleep" );
  krn$_init_timer( &arp_tq );
  memset( arp_cache, 0, sizeof( arp_cache ) );
} /* arp_init() */

/*+
 * ============================================================================
 * = bootp_open - Broadcast BOOTP Request Packet                              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is called to broadcast a BOOTP Request packet in order
 *      to provide the client with information needed to obtain an image.  It
 *      does this by copying the packet into a 300 byte database (same format 
 *      as bootp request packet).  Bootp_open is the first-stage of a two-stage
 *      operation that comprises an Internet boot.
 *
 *      The BOOTP request packet contains several optional parameters including
 *      a server name and a generic file name.  A server reponds with a BOOTP
 *      replay packet containing, among other things, the client's IP address,
 *      the server's IP address, a gateway IP address, and the file path
 *      of a file to boot.  Routine tftp_open which comprises the second-
 *      stage of an Internet boot uses the TFTP protocol to pull over
 *      a specific file from a specifiec server for booting.
 *
 * FORM OF CALL:
 *
 *	Example from inet_driver/init_pidb
 *                       .
 *                       .
 *                     bootp = fopen(newfile, "r");  where
 *           newfile specifies pathname bootp/eza0.0.0.6.0
 *
 * RETURNS:
 *
 *	Integer  msg_success or msg_failure
 *
 * ARGUMENTS:    
 *
 *	struct FILE *fp - File pointer to tftp stucture "bootp_t"
 *	char *info - specify 'broadcast' or null to transmit bootp pkt
 *
 *      char *next - pathname
 *
 *      char *mode - Access mode Read 'r' or Write 'w' 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int bootp_open( struct FILE *fp, char *info, char *next, char *mode )
{
  struct FILE *net;
  pidb_t *pie;
  bootp_t *req;
  int broadcast = 0;
  int status = msg_success;
  trace( "bootp_open\n", BOOTPTRACE );

  if( !info || !strcmp( info, "" ) || !strcmp( info, "broadcast" ) )
    broadcast = 1;
  else if( strcmp( info, "nobroadcast" ) )
    {
      err_printf( "\'%s\' is not a legal parameter for bootp.\n", info );
      return msg_failure;
    }
  if( strchr( mode, 'w' ) )   /* never broadcast when opened for write */
    broadcast = 0;

  bootp_htype = HTYPE_ETHER10;   /* patch for TOKEN RING / ETHERNET */
  if (strncmp_nocase( next, "dw", 2 ) == 0)
    bootp_htype = HTYPE_IEEE8025;
  
  net = open_net( next );
  if( !net )
    return msg_failure;
  
  pie = lookup_pidb( net );
  if( ! pie ) 
    {
      trace( "lookup_pidb failed, making a new entry\n", BOOTPTRACE );
      pie = make_pidb( net );
    }
  req = new( bootp_t );
  req->pie = pie;
  req->xid = inet_random();
  req->fp = fp;
  fp->misc = req;

  krn$_seminit( &( req->tq.sem ), 0, "bootp_sleep" );
  krn$_init_timer( &( req->tq ) );
  
  krn$_wait( &inet_lock );
  req->blink = bootpq.blink;
  req->flink = &bootpq;
  req->blink->flink = req;
  bootpq.blink = req;
  krn$_post( &inet_lock );
  
  if( broadcast )
    {
      int tries;
      bootp_pkt_t *pkt;  /* declare pointer to a bootp message packet struct */
      int backoff;
      unsigned long start;
      int notfirst = 0;
      struct EVNODE *evptr;
      char varname[32];                                                                                        
      
      inet_init_backoff( &backoff );
      start = inet_time( 0 );
      
      /* Allocate space for a 300-byte bootp msg pkt and initialize several 
      *  fields.
      */
      pkt = new( bootp_pkt_t );  
      memset( pkt, 0, sizeof( bootp_pkt_t ) );
      pkt->op = BOOTPREQUEST;
      pkt->xid = req->xid;
      pkt->htype = (char) bootp_htype;    /* patch for TOKEN RING / ETHERNET */
      pkt->hlen = HLEN_ETHER10;    /* TOKEN RING & ETHERNET have same len */

      arp_resolve( 0, pkt->chaddr, 6, net );						

      /*
       * Read environment variables for 
       * server name, (e.g. ez*0_bootp_server) and 
       * file name,   (e.g. ez*0_bootp_file)
       * to place in outgoing bootp request packet "pkt"
       *
       * Load bootp tries integer if set in environ var.
       */
      evptr = new( struct EVNODE );
      sprintf( varname, "%.4s_bootp_server", net->ip->name );
      if( msg_success == ev_read( varname, &evptr, 0 ) )
	strncpy( pkt->sname, evptr->value.string, 64 );
      sprintf( varname, "%.4s_bootp_file", net->ip->name );
      if( msg_success == ev_read( varname, &evptr, 0 ) )
	strncpy( pkt->file, evptr->value.string, 128 );
      sprintf( varname, "%.4s_bootp_tries", net->ip->name );
      if( msg_success == ev_read( varname, &evptr, 0 ) )
	tries = evptr->value.integer;
      else
	tries = BOOTP_DEF_TRIES;
      free( evptr );

      /* Check environ var ez*0_bootp_file for file name. Display it with outgoing 
      *   bootp  broadcast message.
      */
     if(pkt->file[0] == '\0')
        printf("\nBroadcasting BOOTP Request...\n");
     else
        printf("\nBroadcasting BOOTP Request for file: %s\n",pkt->file);

      /*
      * Call routine to execute User Datagram Protocol (UDP) to act as messenger
      * for BOOPT User Datagrams
      *
      */

      while( tries-- > 0 )
	{
	  int i;
	  pkt->secs = htons( inet_time( start ) );
	  status = udp_send( pkt, sizeof( bootp_pkt_t ), INADDR_BROADCAST, 
			    htons( BOOTPCLIENT ), htons( BOOTPSERVER ), net );
	  if( status != msg_success )
	    break;
	  i = inet_backoff( &backoff );
	  if( notfirst ) 
#if 0
	    err_printf( "Retrying bootp; next attempt in %d seconds.\n", i );
#else
	    print_progress();
#endif
	  else
	    notfirst = 1;
#if BOOTPTRACE
	  pprintf( "sending packet at time %d; next retry at %d; delay is %d\n", 
		  inet_time( start ), inet_time( start )+i, i );
#endif
	  i = inet_sleep( &(req->tq), i );
	  if( killpending() ) 
	    {
	      trace( "bootp killed\n", BOOTPTRACE );
	      status = msg_failure;
	      break;
	    }
	  if( i ) 
	    {
	      trace( "bootp exit\n", BOOTPTRACE);
	      status = msg_success;
	      break;
	    }
	  status = msg_failure; /* in case we timeout */
	}
      free( pkt );
    }
  fclose( net );
  if( status != msg_success )
    {
      bootp_close( fp );
      return msg_failure;
    }

  pie->inited = 1;  /* Indicate that received BOOTP Packet has been inited  */
  req->offset = &pie->bootp;
  req->size = sizeof( bootp_pkt_t );
  return msg_success;

} /* end bootp_open() */


int bootp_close( struct FILE *fp )
{
  bootp_t *req = fp->misc;
  pidb_t *pie = req->pie;

  trace( "bootp_close()...\n", BOOTPTRACE );

  krn$_wait( &inet_lock );
  if( req->flink )
    {
      req->blink->flink = req->flink;
      req->flink->blink = req->blink;
    }
  krn$_post( &inet_lock );
  krn$_semrelease( &( req->tq.sem ) );
  krn$_stop_timer( &( req->tq ) );
  krn$_release_timer( &( req->tq ) );
  free( req );
  return msg_success;

} /* end bootp_close() */


int bootp_read( struct FILE *fp, int size, int number, unsigned char *c )
{
  bootp_t* req = fp->misc;
  int i;
#if BOOTPTRACE
  pprintf( "bootp_read()...\n" );
#endif
  i = size * number;
  if( i > req->size ) 
    i = req->size;
  memcpy( c, req->offset, i );
  req->size -= i;
  req->offset += i;
  return i;
} /* bootp_read() */

int bootp_write( struct FILE *fp, int size, int number, unsigned char *c )
{
  bootp_t* req;
  int i;
#if BOOTPTRACE
  pprintf( "bootp_write()...\n" );
#endif
  req = fp->misc;
  i = size * number;
  if( i > req->size ) 
    i = req->size;
  memcpy( req->offset, c, i );
  req->size -= i;
  req->offset += i;
  return i;
} /* bootp_write() */



/*+
 * ============================================================================
 * = tftp_open - Implement Trivial File Transfer Protocol                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine, which comprises the second-stage of an Internet boot
 *      uses the TFTP protocol to read in the file for booting.  Tftp_open
 *      uses the server address and file name supplied by the BOOTP server in
 *      the BOOTP reply.  The tftp driver prints out the client and server IP
 *      addresses and the file path being used for the transfer.
 *  
 *     If a named boot is attempted ( -file 'filename', or via environ var
 *     "boot_file") the file name will be passed to the tftp driver.
 *
 * FORM OF CALL:
 *
 *	Example from boot.c/boot_network
 *                       .
 *                       .
 *                     fp = fopen(newfile, "sr");  where
 *           newfile is tftp:verbose:'name of file'/eza0.0.0.6.0
 * 
 * RETURNS:
 *
 *	Integer  msg_success or msg_failure
 *
 *
 * ARGUMENTS:   
 *
 *	struct FILE *fp - File pointer to tftp stucture "tftp_t"
 *	char *info - specify 'verbose' for tftp output display 
 *		     If its a null address we use the DUMP/LOAD multicast
 *		     address
 *
 *      char *next - filename passed from named file (-file) or from
 *                   environ var boot_file.
 *
 *      char *mode - Read 'r' or Write 'w' (currently only reads supported)
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int tftp_open( struct FILE *fp, char *info, char *next, char *mode )
{
  int read = 0;
  int write = 0;
  unsigned int netmask = 0, netmask_len = 0;
  unsigned char *bp_vendor_ptr,router_found,count,tag,skip_count;
  unsigned char *netmask_ptr = (unsigned char *)(&netmask) ;
  int router_length,*router_ptr,router_count;
  char netmask_str[80]; /* used for printing netmask value */
  char gateway_str[80]; /* used for printing gateway address */

  if( strchr( mode, 'r' ) )
    read = 1;
  if( strchr( mode, 'w' ) )
    write = 1;
  if( read && write )
    {
      err_printf( "Cannot both read and write tftp.\n" );
      return msg_failure;
    }
  if( read )
  {
      struct FILE *net;
      pidb_t *pie;
      char *mode = "octet";
      char filename[128];    /* storage for filename from bootp packet */
      int remote_addr;
      int pkt_leng = 0;
      int backoff;
      char *req_pkt, *p;
      ip_hdr_t *ip;
      udp_hdr_t *udp;
      tftp_hdr_t *tftp;
      tftp_t *req;
      int tries;
      int notfirst = 0;
      struct EVNODE *evptr;
      int verbose = FALSE;
      char varname[32];
      int status;		
      char haddr[6];		
      /*   
       * 0xffff <= xid <= 1024 to avoid well known ports - RFC 1060 pp 9, 13 
       */
      unsigned short xid = 0;

      net = open_net( next );
      if( !net ) {
	return  msg_failure;
      }      
      pie = lookup_pidb( net );			/* copy per-interface data base */
      if( !pie )
	pie = make_pidb( net );

      /*
      * Ensure bootp packet database is not inited to ensure 2-stage booting 
      * rather then just doing the second stage -- tftp. re:QAR 46361 
      * When the bootp packet database has not been inited, call a routine that will
      * broadcast the bootp packet and intialize the database.
      * If successful display the received BOOTP request packet file name.
      *
      */

      /* Init gateway_used=0 so that bootp broadcast will be normally 
      ** broadcast, and init tag=0 so that the search through the vendor area 
      ** in the bootp database for the subnet mask will not terminate 
      ** prematurely. 
      ** 
      ** Init the bootp_htype in case exx0_inet_init set to "nvram", in 
      ** which case bootp_open() will not be called.
      ** 
      ** Zero out the bootp packet for this pie so that we won't take action
      ** based on stale data from an earlier boot.  This could happen if a
      ** real bootp boot is done, followed by an nvram-initialized bootp boot.
      */

      pie->inited = 0;
      gateway_used = 0;  
      tag = 0;
      memset( &pie->bootp, 0, sizeof( bootp_pkt_t ) ); /* clear bootp packet */
      

      bootp_htype = HTYPE_ETHER10;   /* patch for TOKEN RING / ETHERNET */
      if (strncmp_nocase( next, "dw", 2 ) == 0)
        bootp_htype = HTYPE_IEEE8025;

      /* Zap the arp cache to ensure that the tftp_open will involve an arp
      ** request to the tftp server.  This is necessary because if the bootp
      ** phase was skipped, the server's arp cache may not otherwise know our
      ** hardware address.  Forcing the ARP here ensures that the server will
      ** know our hardware address.
      */
      memset( arp_cache, 0, sizeof( arp_cache ) );   


       if( !pie->inited )
       {
            init_pidb( net );
              if (pie->inited && !nvram_bootp_init)
               /* If bootp packet was inited then                           */ 
               /*  display the received  BOOTP request packet file name     */
                printf("Received BOOTP Packet File Name is: %s\n",pie->bootp.file);
       }
  
      /* If bootp failed then indicate failure and return */
      if (!pie->inited)
      {
         fclose(net);
         return msg_failure;
      }

      krn$_wait( &inet_lock );
      strcpy( filename, pie->bootp.file );
      remote_addr = pie->bootp.siaddr;

      krn$_post( &inet_lock );

      /*
       * check for "verbose" switch, which must come first in "info" string
       */
      if( info && strcmp( info, "" ) && strstr( info, "verbose:" ) == info )
      {
	 info += strlen( "verbose:" );
	 verbose = TRUE;
      }

      if( parse_inet_filename( info, &remote_addr, filename ) != msg_success )
      {
	  err_printf("Bad file name.\n");
	  fclose( net );
	  return msg_failure;
      }

      /* Display information about the TFTP (second stage of IP boot) 
      *  operation. For example:
      *
      *    local inet address: 16.123.16.71
      *    remote inet address: 16.123.16.54
      *    Tftp Read File Name: /var/adm/ris/ris0.alpha/vmunix
      */
      if( verbose )
      {
	  char buf[16];
	  inetaddr2string( ntohl( hostid( net ) ), buf );
	  printf( "local inet address: %s\n", buf );
	  inetaddr2string( ntohl( remote_addr ), buf );
	  printf( "remote inet address: %s\n", buf );
           /* Indicate the file that TFTP will use.  */
	  printf( "TFTP Read File Name: %s\n", filename );   
      }
      pkt_leng = 2 + strlen( filename ) + 1 + strlen( mode ) + 1;
      req_pkt = malloc( pkt_leng );
      tftp = req_pkt;
      tftp->op = htons( TFTPRRQ );
      p = tftp;
      p += 2;
      strcpy( p, filename );
      p += strlen( filename ) + 1;
      strcpy( p, mode );
      p += strlen( mode ) + 1;
      if( p != req_pkt + pkt_leng ) pprintf( "bad packet size!!! \n" );

      req = new( tftp_t );
      req->fp = fp;
      fp->misc = req;
      req->net = net;
      req->pie = pie;
      req->direction = TFTPRRQ;
      req->xid = xid;
      req->rem_port = htons( TFTPSERVER );
      req->rem_iaddr = remote_addr;
      req->block = 0;
      req->pkt = NULL;
      req->offset = NULL;
      req->size = 0;
      req->last = 0;
      req->error = 0;
      krn$_seminit( &req->tq.sem, 0, "tftp_sleep" );
      krn$_init_timer( &req->tq );

      evptr = new( struct EVNODE );
      sprintf( varname, "%.4s_tftp_tries", net->ip->name );
      if( msg_success == ev_read( varname, &evptr, 0 ) )
	req->tries = evptr->value.integer;
      else
	req->tries = TFTP_DEF_TRIES;
      free( evptr );

      krn$_wait( &inet_lock );
      req->blink = tftpq.blink;
      req->flink = &tftpq;
      tftpq.blink->flink = req;
      tftpq.blink = req;
      krn$_post( &inet_lock );

      inet_init_backoff( &backoff );
      tries = req->tries;


/*+-------------------------------------------------------------------------------------------------------------------------------*/
/*	
*     Arp resolve for ip address of siaddr. If this fails the use address of ip-router 1 thru n, if this fails use gateway addr
*/      

  bp_vendor_ptr = &pie->bootp.vend[4];				/* assign first tag from vendor information field */
  router_count = 0;	
  count = 4;		       		/* vif packet length is 64 - 1st 4 bytes is magic cookie - skip over magic cookie*/
  
    /*
    ** If bootp info came from nvram rather than bootp broadcast, then 
    ** fetch the default subnet mask from the nvram ev.  Otherwise, the subnet
    ** mask will come in the bootp reply from the host.
    */

    if (nvram_bootp_init)
    { 
      sprintf( varname, "%.4s_def_subnetmask", net->ip->name );
      if( msg_success == ev_read( varname, &evptr, 0 ) )
	{
          unsigned long iaddr;
          int status;

          status = string2inetaddr( &iaddr, evptr->value.string );
          iaddr = htonl( iaddr );
          if( status == msg_success )
	    {
	      netmask = iaddr;
	    }
	}
      else
	{
	  printf("?? %.4s_def_subnetmask not defined.\n", net->ip->name );
	  return msg_failure;
	}

    }


    while ( (count < 60) && ( tag != 1 ) ) 
    {
       tag = *bp_vendor_ptr;
       if (tag == 1)
       {  						 /* get netmask */
          netmask_len = *(bp_vendor_ptr + 1);
	  if (netmask_len > 4) {
	      printf("Invalid subnet mask in BOOTP information\n");
 	      return msg_failure;
	  }
	  memcpy(netmask_ptr,(bp_vendor_ptr + 2),netmask_len);
       }
       else
       {
	  if ((tag == 0) || (tag == 255))
	  {   /* tag 0 and 255 have no data field */
	     skip_count = 1;
	  }
	  else
	  {
	     /* Skip over data length + 2(tag and length bytes) */
	     skip_count = *(bp_vendor_ptr + 1) + 2;
	  }
	  bp_vendor_ptr += skip_count;
	  count += skip_count;
       }
    } /* end of while loop .*/

    /*
    ** If no subnet mask provide either by the def_subnetmask ev 
    ** (ewx0_inet_init="nvram") or in the bootp reply (ewx0_inet_init="bootp"),
    ** then set the netmask to the class mask. 
    */

    if (!netmask) netmask = class_mask(pie->bootp.yiaddr);

    if (verbose)
         {
           inetaddr2string( htonl(netmask), netmask_str);
           printf("netmask = %s\n",netmask_str);
         }

	/* remote_addr is siaddr at this point. 
	 * netmask either class network mask or supplied subnet mask, either
         * from the vendor data area or if bootp area was inited from nvram
         * then from the exx0_def_subnetmask ev.  
         */

   status = msg_failure;
   if ( (pie->bootp.yiaddr & netmask) ==  (pie->bootp.siaddr & netmask) )
   { /* server is on same subnet as client */
      status = arp_resolve( (long)remote_addr, pie->bootp.chaddr, 6, net );	/* arp_resolve siaddr */
      if ((status == msg_success) && (verbose || TFTPTRACE))
              printf("Server is on same subnet as client.\n");
   }

   if (status != msg_success)
   {  /* maybe different subnet. Try to arp ar router */
      bp_vendor_ptr = &pie->bootp.vend[4]; 				/* 1st tag  */
      router_found = 0; count = 4;

      while ( (count < 60) && (!router_found) )
      { /* check vendor information area for router tag */
         tag = *bp_vendor_ptr;
         if ( tag == 3 )  						/* router tag */
         {
            router_length = *(bp_vendor_ptr + 1);
	    bp_vendor_ptr = bp_vendor_ptr + 2; 		/* point to first router addr */
	    count += 2;
	    router_ptr = (int *)bp_vendor_ptr;
	  
	    while ( (router_count < router_length) && (!router_found) )
	    {
	      status = arp_resolve( (long)*router_ptr, haddr, 6/*&hlen*/, net );	/* arp_resolve router addr */
              if( status != msg_success ) 					
	      {  /* Try next router in list*/
		 *router_ptr++;
		 router_count += 4;
	      	 count += 4;
	      }
	      else
	      {  /* arp_resolve ok */
  	         pie->bootp.giaddr = *router_ptr;	   
  	      	 gateway_used = *router_ptr;
	         remote_addr =  *router_ptr++;	 	  
                 router_found = 1;
	      }
	    } /*end of while loop*/
	 } /* end of if tag == 3*/
         else
         {
	   /* Skip over field if tag is 0 (pad field) or 255 (end  */
	   /* field). These fields have no data.		      */
	   if ((tag == 0) || (tag == 255))
	   {
	         skip_count = 1;
	   }
	   else
	   {
	   	/* Skip over data length + 2(tag and length bytes)  */
	      skip_count = *(bp_vendor_ptr + 1) + 2;
	   }
	   bp_vendor_ptr += skip_count;
	   count += skip_count;
	}
     } /* end of while loop */

     /* Arp_respove the gateway if no router ip address was found in the vendor extension area
      *  If this fails then arp_resolve the server. If this fails then fail bootp.
     */
     if( (pie->bootp.giaddr != 0) && (!router_found) )
     {
              
	status = arp_resolve( (long)pie->bootp.giaddr, haddr, 6, net );	/* arp_resolve giaddr */
        if ( status != msg_success )
        {
           arp_resolve( (long)pie->bootp.siaddr, haddr, 6, net );	/* arp_resolve siaddr */
            if ( status != msg_success )
            {
               printf("Server not found. Boot failed\n");
	       return msg_failure;
            }
	    else
	    {
               gateway_used = pie->bootp.siaddr;
	       return msg_failure;
	    }
        }
        else
	{
           gateway_used = pie->bootp.giaddr;
           router_found = 1;
        }
     }
  } /* end of else statment -- maybe on different subnet */
/*--------------------------------------------------------------------------------------------------------------------------------*/
/* If in verbose mode, then print out some information regarding
** which gateway will be used for the tftp.
*/


     if (verbose && gateway_used)
         {
           printf("Server is NOT on same subnet as client...\n");
           inetaddr2string( htonl(gateway_used), gateway_str);
           printf("  Router used = %s\n",gateway_str);
         }

      while( tries-- > 0 )
      {
	 int wakeup, status;
	 xid = 1024 + ( inet_random() % ( 0xffff - 1024 ) );
	 req->xid = xid;
	 if( notfirst )
	    print_progress();
	 else
	    notfirst = 1;
	 status = udp_send( req_pkt, pkt_leng, pie->bootp.siaddr, xid, htons( TFTPSERVER ), net ); 
	 if ( status != msg_success ) goto TFTP_KILLED;
         
         wakeup = inet_sleep( &req->tq, inet_backoff( &backoff ) );
	 if( killpending() ) goto TFTP_KILLED;
	 if( wakeup == 2 ) goto TFTP_KILLED;
	 if( wakeup == 1 ) goto TFTP_EXIT;
      } 

    TFTP_KILLED:
      trace( "tftp open killed, erred, or timed out\n", TFTPTRACE);
      free( req_pkt );
      if( req->error )
	{
	  err_printf( "Tftp error %d: %s.\n", req->error - 1, req->offset );
	}
      krn$_wait( &inet_lock );
      req->flink->blink = req->blink;
      req->blink->flink = req->flink;
      krn$_post( &inet_lock );
      krn$_stop_timer( &req->tq );
      krn$_release_timer( &req->tq );
      krn$_semrelease( &req->tq.sem );
      if( req->pkt ) freepkt( req->pkt, net );
      free( req );
      return msg_failure;

    TFTP_EXIT:
      trace( "tftp open succeeded\n", TFTPTRACE );
#if 0
      krn$_post( &req->tq.sem, 1 );                /* enable first tftp_read */
#endif
      free( req_pkt );
      return msg_success;
    }

  if( write )
    {
      err_printf( "Tftp writes not supported.\n" );
      return msg_failure;
      /* send request */
      /* receive acknowledge */
    }
} /* end tftp_open() */

/*+
 * ============================================================================
 * = parse_inet_filename - Handle Filename                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * Parse an inet filename, which may have a host address in 
 * dotted.decimal.notation prefixed to it.
 *
 * If string is null 
 *   just return.
 * If prefix of string is a valid internet address (followed by one or 
 *         more ':'),
 *   write address into remote_addr;
 * If remaining suffix is null
 *      return
 *   else
 *      strcpy suffix into filename
 *
 * return msg_failure if suffix to copy into filename is more than 127 chars
 * (restriction is based on 128 byte filenames in bootp packets)
 *
 * Valid examples:		internet address	filename
 *
 * "1.1.1.1::yomamma"		1.1.1.1			yomamma
 * "1.1.1.1::"			1.1.1.1			default
 * bootfile                     default                 bootfile
 * "1.1.1.1"			default			1.1.1.1
 * "125.12.13.heres:atrickyone"	default			125.12.13.258:atrickyone
 * "125.12.13.258:atrickyone"	default			125.12.13.258:atrickyone
 * may36::bootfile		default			may36::bootfile
 *
 *
 *
 * FORM OF CALL:
 *
 *       parse_inet_filename( info, &remote_addr, filename )
 *
 * RETURNS:
 *
 *	Integer  msg_success or msg_failure
 *
 * ARGUMENTS:
 *
 * ARGUMENTS:
 *
 *      char *string - named file to boot via (-file) or boot_file environ var
 *
 *      unsigned long *remote_addr - remote inet addr in dotted decimal 
 *                                   notation, i.e. 16.123.16.54
 *
 *      char *filename  - from bootp packet filename field:
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int parse_inet_filename( char *string, unsigned long *remote_addr, char *filename )
{
  char *p;
  
  p = string;
  if( !p || !*p )                /* if null do nothing */
    return msg_success;
  
  /*
   * handle prefix
   */
  while( *p && *p != ':' )       /* search for prefix like "255.255.255.255:" */
    p++;
  if( *p )
    {
      unsigned long iaddr;
      *p = '\0';                 /* terminate prefix for string2inetaddr() */
      if( string2inetaddr( &iaddr, string ) == msg_success )
	{
	  *remote_addr = htonl( iaddr );    /* copy address */
	  *p = ':';              /* undo side effect */
	  while( *p == ':' )     /* skip over any remaining ':' */
	    p++;
	}
      else
	{
	  *p = ':';              /* undo side effect */
	  p = string;
	}
      
    }
  else
    {
      p = string;
    }

  /*
   * p nows points to any remaining suffix which will be used as a filename
   * we have undone any modifications to the original string
   */
  
  /*
   * handle suffix
   */
  if( *p )                       /* if not null copy to filename */
    {
      if( strlen( p ) > 127 )
	{
	  return msg_failure;
	}
      strcpy( filename, p );
    }
  return msg_success;
  
} /* parse_inet_filename() */

/*
 * terminate connections
 */
int tftp_close( struct FILE *fp )
{
  tftp_t *tftp = fp->misc;
  krn$_wait( &inet_lock );
  tftp->flink->blink = tftp->blink;
  tftp->blink->flink = tftp->flink;
  krn$_post( &inet_lock );
  krn$_stop_timer( &tftp->tq );
  krn$_release_timer( &tftp->tq );
  krn$_semrelease( &tftp->tq.sem );
  if( tftp->pkt ) freepkt( tftp->pkt, tftp->net );
  fclose(tftp->net);
  free( tftp );
  return msg_success;
} /* tftp_close() */


int tftp_read( struct FILE *fp, int size, int number, unsigned char *c )
{
  tftp_t *req = fp->misc;
  int rd_size = size * number;
  int total = 0;

#if TFTPTRACE || TFTP_READ_TRACE
  pprintf( "tftp_read() size %d number %d\n", size, number);
#endif

  while( total < rd_size && (!req->last || req->size) )
    {

#if TFTPTRACE || TFTP_READ_TRACE
      pprintf( "total %d rd_size %d last %d size %d\n", 
	      total, rd_size, req->last,req->size );
#endif

      if( req->size > rd_size - total )
	{
	  trace( "tftp_read() leftover data in packet\n", 
		TFTPTRACE || TFTP_READ_TRACE );
	  memcpy( c, req->offset, rd_size - total );
	  req->size -= rd_size - total;
	  req->offset += rd_size - total;
	  total = rd_size;
	}
      else  /* exhaust packet */
	{
	  trace( "tftp_read() exhausting packet\n", 
		TFTPTRACE || TFTP_READ_TRACE);
	  memcpy( c, req->offset, req->size );
	  total += req->size;
	  c += req->size;
	  req->size = 0;
	  req->offset = NULL;
#if MALLOCTRACE
	  pprintf( "freepkt %08x\n", req->pkt );
#endif
	  if( req->pkt ) freepkt( req->pkt, req->net );
	  req->pkt = NULL;

	  if( req->last )
	    {
	      tftp_ack( fp );
	    } 
	  else
	    {
	      int tries = req->tries;
	      int backoff;
	      int notfirst = 0;

	      inet_init_backoff( &backoff );
	      while( tries-- > 0 )
		{
		  int wakeup;
		  if( notfirst )
		    print_progress();
		  else
		    notfirst = 1;
		  tftp_ack( fp );
		  wakeup = inet_sleep( &req->tq, inet_backoff( &backoff ) );
		  if( killpending() || wakeup == 2 ) 
		    {
		      trace( "tftp_read() kill/err/time out\n", TFTPTRACE );
		      if( req->error )
			{
			  err_printf( "Tftp error %d: %s.\n", 
				   req->error - 1, req->offset );
			  fp->status = msg_failure;
			}
		      return total;
		    }
		  if( wakeup == 1 ) break;
		}
	      if(tries<=0)
		{
		  fp->status = msg_failure;
		  return total;
                }
	    }
	}
    }
  return total;
} /* tftp_read() */

int tftp_write( struct FILE *fp, int size, int number, unsigned char *c )
{
  err_printf( "Writes to tftp not supported.\n" );
  return 0;
} /* tftp_write() */

/*
 * transmit acknowledge appropriate for current state of transfer
 */
void tftp_ack( struct FILE *fp )
{
  tftp_ack_t ack;
  int status;
  tftp_t *misc = fp->misc;

  trace( "sending tftp ack\n", TFTPTRACE );
  ack.op = htons( TFTPACK );
  ack.block = htons( misc->block );
  status = udp_send( &ack, sizeof( ack ), misc->rem_iaddr, misc->xid, 
		     misc->rem_port, misc->net );
  if( status != msg_success )
    {
      err_printf( "Udp_send failed.\n" );
    }
} /* tftp_ack() */


/*
 * sets up iovec and calls udp_sendv
 */
int udp_send( void *pkt,           /* pointer to message */
	     int size,             /* size of message */
	     unsigned int iaddr,   /* destination ip address */
	     int src,              /* source port - net order */
	     int dst,              /* destination port - net order */
	     struct FILE *net )    /* interface to send over */
{
  iovec_t iovec[MAX_IOVEC];
  memset( iovec, 0, sizeof( iovec ) );
  add_iovec( iovec, pkt, size );
  return udp_sendv( iovec, iaddr, src, dst, net );
} /* udp_send() */

int udp_sendv( iovec_t *iovec,      /* iovec containing packet */
	     unsigned int iaddr,   /* destination ip address */
	     int src,              /* source port - net order */
	     int dst,              /* destination port - net order */
	     struct FILE *net )    /* interface to send over */
{
  udp_hdr_t udp_hdr;
  unsigned long sum, i;
  unsigned char tmp[2];
  int status, size;
  
  trace( "udp_sendv\n", UDPTRACE);
  status = add_iovec( iovec, &udp_hdr, sizeof( udp_hdr ) );
  if( status != msg_success )
    return msg_failure;
  size = size_iovec( iovec );
  udp_hdr.udp_sport = src;
  udp_hdr.udp_dport = dst;
  udp_hdr.udp_len = htons( size );
  udp_hdr.udp_sum = 0;

  /*
   * compute check sum: pseudo-hdr + everything in iovec
   */
  sum = 0;
  {
    udp_pseudo_hdr_t ps_hdr;
    ps_hdr.st.src_iaddr = iaddr;
    ps_hdr.st.dst_iaddr = hostid( net );
    ps_hdr.st.zero = 0;
    ps_hdr.st.proto = IP_UDP_PROTOCOL;
    ps_hdr.st.len = htons( size );
    for( i = 0; i < 6; i++)
      sum += ps_hdr.shorts[i];
  }
#if UDPTRACE
  pprintf( "sum of udp pseudo-header is %04x\n", sum );
#endif

  for( i = 0; i < MAX_IOVEC && iovec[i].iov_base; i++ )
    sum += 0xffff & ~inet_cksum( iovec[i].iov_base, iovec[i].iov_len );
  sum = ( sum >> 16 ) + ( sum & 0xffff );
  sum += ( sum >> 16 );
  if( sum != 0xffff ) 
    sum = ~sum;
  udp_hdr.udp_sum = sum;

  status = ip_sendv( iovec, iaddr, IP_UDP_PROTOCOL, net );
  del_iovec( iovec );
  return status;
} /* udp_sendv() */

ip_hdr_t ip_template = {         /* defaults for ip headers */
  sizeof( ip_hdr_t )/4,          /* Header Length */         
  IPVERSION, 			 /* Version */               
  0, 				 /* Type of service */       
  htons( sizeof ( ip_hdr_t ) ),  /* Total length */          
  0,  				 /* Identification */        
  0,  				 /* Fragment offset field */ 
  STDTTL,  			 /* Time to live */          
  0,  				 /* Protocol */              
  0,  				 /* Checksum */              
  0,  				 /* IP source address */     
  0 };				 /* IP destination address */

int ip_sendv( iovec_t *iovec,     /* iovec containing packet */
	    unsigned int iaddr,   /* destination ip address */
	    int protocol,         /* source protocol (eg, UDP ) */
	    struct FILE *net )    /* interface to send over */
{
  int i, size, status;
  unsigned long addr;
  pidb_t *pie;
  ip_hdr_t ip_hdr;
  
  trace( "ip_sendv\n", IPTRACE );
  status = add_iovec( iovec, &ip_hdr, sizeof( ip_hdr ) );
  if( status != msg_success )
    return msg_failure;
  size = size_iovec( iovec );
  memcpy( &ip_hdr, &ip_template, sizeof( ip_hdr_t ) );
  ip_hdr.ip_len = htons( size );
  ip_hdr.ip_id = inet_random();
  ip_hdr.ip_p = protocol;
  ip_hdr.ip_src = hostid( net );
  ip_hdr.ip_dst = iaddr;
  ip_hdr.ip_sum = inet_cksum( ( unsigned short* )&ip_hdr, sizeof( ip_hdr_t ) );
  ip_route( &iaddr, net );
  status = net_send( iovec, iaddr, DODIP_PROTOCOL, net );
  del_iovec( iovec );
  return status;
} /* ip_sendv() */

/*
 * if iaddr is not on the same network as net replace iaddr with gateway address
 */
void ip_route( unsigned int *iaddr, struct FILE *net )
{
  unsigned long mask;
  unsigned long addr = ntohl( *iaddr );

  if( !( addr & 0x80000000 ) )      /* class A address */
    mask = 0xff000000;
  else if( !( addr & 0x40000000 ) ) /* class B address */
    mask = 0xffff0000;
  else if( !( addr & 0x20000000 ) ) /* class C address */
    mask = 0xffffff00;
  else                              /* class D/E address (multicast or rsved)*/
    mask = 0xffffffff;
  
  if( mask & addr != mask & ntohl( hostid( net ) ) )
    *iaddr = gateid( net );
  
} /* ip_route() */

/*
 * send packet over network
 * this routine should hide driver detail from layers above
 * (like the fact that the ndl driver is hardwired for ethernet)
 * returns msg_success or failure code from arp
 */
int net_send( iovec_t *iovec,      /* iovec containing packet */
	      unsigned int iaddr,   /* destination ip address */
	      int protocol,         /* source protocol -- ip, arp, etc */
	      struct FILE *net )    /* interface to send over */
{
  int i, size;
  char *pkt, *p;
  char haddr[16];
  int hlen = sizeof( haddr );
  int status;
  
  trace( "net_send\n", NETTRACE );
  size = size_iovec( iovec );
  p = pkt = ndl_allocate_msg( net->ip, NDL$K_VERSION_3,NDL$K_DONT_INC_LENGTH );
  for( i = 0; i < MAX_IOVEC &&  iovec[i].iov_base; i++ )
    {
      char *b = iovec[i].iov_base;
      int s = iovec[i].iov_len;
      memcpy( p, b, s );
      p += s;
    }
  if ( gateway_used )
     status = arp_resolve( gateway_used, haddr, 6/*&hlen*/, net );
  else
  status = arp_resolve( iaddr, haddr, 6/*&hlen*/, net );
  if( status != msg_success ) 
    return status;

  ndl_send( net, NDL$K_VERSION_3, pkt, size, NDL$K_DONT_INC_LENGTH, haddr, protocol );
  return msg_success;
} /* net_send() */

/*
 * add buffer to head of iovec
 */
int add_iovec( iovec_t *iovec, void *base, int len )
{
  int i = 0;
  trace( "add_iovec\n", 0 );
  while( i < MAX_IOVEC && iovec[i].iov_base )
    i++;
  if( i == MAX_IOVEC )
    return msg_failure;
  while( i > 0 )
    {
      iovec[i].iov_base = iovec[i-1].iov_base;
      iovec[i].iov_len = iovec[i-1].iov_len;
      i--;
    }
  iovec[0].iov_base = base;
  iovec[0].iov_len = len;
  return msg_success;
} /* add_iovec() */

/*
 * remove buffer from head of iovec
 * return pointer to deleted buffer
 */
void *del_iovec( iovec_t *iovec )
{
  int i;
  void *buf;
  trace( "del_iovec\n", 0 );
  buf = iovec[0].iov_base;

  if( !buf ) 
    return buf;

  for( i = 1; i < MAX_IOVEC && iovec[i].iov_base; i++ )
    {
      iovec[i-1].iov_base = iovec[i].iov_base;
      iovec[i-1].iov_len = iovec[i].iov_len;
    }
  if( i != MAX_IOVEC )
    {
      iovec[i-1].iov_base = NULL;
      iovec[i-1].iov_len = 0;
    }
  return buf;
} /* del_iovec() */

/*
 * tally size of all bufs in iovec
 */
int size_iovec( iovec_t *iovec )
{
  int size, i;
  trace( "size_iovec\n", 0 );
  size = 0;
  for( i = 0; i < MAX_IOVEC && iovec[i].iov_base; i++ )
    size += iovec[i].iov_len;
  return size;
} /* size_iovec() */

void ip_receive( struct FILE *fp, unsigned char *msg, 
		unsigned char *src, int size, int version )
{
  int i;
  int err = 0;
  ip_hdr_t ip;

  trace( "ip_receive\n", IPTRACE );

  if( size < sizeof( ip_hdr_t ) ) 
    {
      err = 1; 
      goto DISCARD;
    }
  memcpy( &ip, msg, sizeof( ip_hdr_t ) );
  if( ip.ip_v != IPVERSION ) 
    {
      err = 2; 
      goto DISCARD;
    }
  i = inet_cksum( &ip, 4*ip.ip_hl );
  if( i && i!=0xffff ) {
    err = 3; 
    goto DISCARD;
  }
  if( ntohs( ip.ip_off ) & IP_FRAGS )  /* no fragments allowed */
    {
      err = 4; 
      goto DISCARD;
    }
  if( hostid( fp ) &&                  /* we know our address */
     (int)ip.ip_dst != (int)hostid( fp ) &&      /* and it's not for us */
     (int)ip.ip_dst != INADDR_BROADCAST )   /* and it's not a broadcast */
    {
#if IPTRACE
      pprintf( "hostid: %08x dst: %08x src: %08x broadcast: %08x\n",
	      hostid( fp ), ip.ip_dst, ip.ip_src, INADDR_BROADCAST );
#endif
      err = 5;
      goto DISCARD;
    }
  if( size < ntohs( ip.ip_len ) )
    {
      err = 6;
      goto DISCARD;
    }
  if ( ip.ip_p == IP_UDP_PROTOCOL )
    {
      udp_pseudo_hdr_t ps_hdr;
      unsigned short cksum;

      ps_hdr.st.src_iaddr = ip.ip_src;
      ps_hdr.st.dst_iaddr = ip.ip_dst;
      ps_hdr.st.zero = 0;
      ps_hdr.st.proto = IP_UDP_PROTOCOL;
      ps_hdr.st.len = htons( ntohs( ip.ip_len ) - 4*ip.ip_hl );
      cksum = inet_cksum( ps_hdr.shorts, sizeof( udp_pseudo_hdr_t ) );
      udp_receive( fp, msg + 4*ip.ip_hl, ntohs( ip.ip_len ) - 4*ip.ip_hl, 
		  cksum, msg );
      return;
    }
  else
    {
#if IPTRACE
      pprintf( "ip protocol is %02x\n", ip.ip_p );
      hexdump( msg, size );
      err = 7;
#endif
    }
 DISCARD:
#if IPTRACE
  pprintf( "ip_receive() discarded packet for error %d\n", err );
#endif
#if MALLOCTRACE
  pprintf( "freepkt %08x\n", msg );
#endif
  freepkt( msg, fp );
  return;
} /* ip_receive() */

void udp_receive( struct FILE *fp, char *msg, int size, unsigned short cksum,
		 char *pkt )
{
  int i;
  int err = 0;
  udp_hdr_t udp;

  trace( "udp_receive\n", UDPTRACE );

  if( size < sizeof( udp_hdr_t ) ) 
    {
      err = 1; 
      goto DISCARD;
    }
  memcpy( &udp, msg, sizeof( udp_hdr_t ) );
  if( size < ntohs( udp.udp_len ) )
    {
      err = 2;
      goto DISCARD;
    }
  if( udp.udp_sum )
    {
      unsigned short sum = cksum + inet_cksum( msg, ntohs(udp.udp_len) );
      if( sum && sum != 0xffff )
	{
	  err = 3;
	  goto DISCARD;
	}
    }    
  if ( udp.udp_dport == ntohs( BOOTPCLIENT ) ) 
    {
      bootp_receive( fp, (msg + sizeof( udp_hdr_t )), (size - sizeof( udp_hdr_t )), pkt );
      return;
    }
  else
    {
      tftp_receive( fp, msg + sizeof( udp_hdr_t ), size - sizeof( udp_hdr_t ), 
		   udp.udp_sport, udp.udp_dport, pkt );
      return;
    }
 DISCARD:
#if UDPTRACE
  pprintf( "udp_receive() discarded packet for error %d\n", err );
#endif
#if MALLOCTRACE
  pprintf( "freepkt %08x\n", msg );
#endif
  freepkt( pkt, fp );
  return;
} /* udp_receive() */

void bootp_receive( struct FILE *fp, char *msg, int size, char *pkt )
{
  int err = 0;
  bootp_pkt_t *bootp;    /* pointer to start of bootp pkt */
  bootp_t *p;
  long bootp_xid;
  char haddr[6];

  /*
   * validate bootp packet
   */
  trace("checking bootp packet\n", BOOTPTRACE );

  if( size < sizeof( bootp_pkt_t ) )
    {
      err = 1;
      goto DISCARD;
    }
  bootp = msg;
  if( bootp->op != BOOTPREPLY ) 
    {
      err = 2;
      goto DISCARD;
    }
  if( bootp->htype != (char) bootp_htype )    /* patch for TOKEN RING / ETHERNET */
    {
      err = 3; 
      goto DISCARD;
    }
  if( bootp->hlen != HLEN_ETHER10 )  /* TOKEN RING & ETHERNET have same len */
    {
      err = 4; 
      goto DISCARD;
    }
  arp_resolve( 0, haddr, HLEN_ETHER10, fp );
  if( memcmp( bootp->chaddr, haddr, HLEN_ETHER10 ) ) 
    {
      err = 5; 
      goto DISCARD;
    }
  memcpy( &bootp_xid, &bootp->xid, 4 );

#if BOOTPTRACE
  pprintf( "searching bootp request list for xid %08x net %s\n", bootp_xid, fp->ip->name );
#endif

  krn$_wait( &inet_lock );
  for( p = bootpq.flink; p != &bootpq; p = p->flink )
    {
#if BOOTPTRACE
      pprintf( "bootp request: xid %08x net %s\n", p->xid, p->pie->ip->name);
#endif
      if( p->xid != bootp_xid ) continue;
      if( p->pie->ip != fp->ip ) continue;
      break;
    }
  if( p == &bootpq ) 
    {
      krn$_post( &inet_lock );
      err = 6; 
      goto DISCARD;
    }
  trace("copying bootp packet\n",BOOTPTRACE);
  memcpy( &( p->pie->bootp ), bootp, sizeof( bootp_pkt_t ) );
  p->blink->flink = p->flink;
  p->flink->blink = p->blink;
  p->flink = NULL;
  p->blink = NULL;
  krn$_post( &inet_lock );
  krn$_post( &( p->tq.sem ), 1 );
#if MALLOCTRACE
  pprintf( "freepkt %08x\n", pkt );
#endif
  freepkt( pkt, fp );
  return;
 DISCARD:
#if BOOTPTRACE
  pprintf( "bootp_receive() discarded packet for error %d\n", err );
#endif
#if MALLOCTRACE
  pprintf( "freepkt %08x\n", pkt );
#endif
  freepkt( pkt, fp );
  return;
} /* bootp_receive() */

void tftp_receive( struct FILE *fp, unsigned char *msg, int size, 
		  unsigned short sport, unsigned short dport, char *pkt )
{
  /*
   * should we check for expected rem_port?
   * could break on opens, when first reply packet contains data + port
   */
  int err = 0;
  tftp_t *p;
  tftp_hdr_t tftp;

  trace("tftp_receive\n",TFTPTRACE);

  krn$_wait( &inet_lock );
  for( p = tftpq.flink; p != &tftpq; p = p->flink )
    {
      if( dport != p->xid ) continue;
      if( p->pie->ip != fp->ip ) continue;
      break;
    }
  if( p == &tftpq )        /* no request for this packet */
    {
      krn$_post( &inet_lock );
      err = 1; 
      goto DISCARD;
    }

  if( size < sizeof( tftp_hdr_t ) ) 
    {
      err = 2; 
      goto DISCARD;
    }
  memcpy( &tftp, msg, sizeof( tftp_hdr_t ) );

#if TFTPTRACE      
  pprintf("tftp opcode is %04x\n", ntohs( tftp.op ) );
#endif

  switch( ntohs( tftp.op ) ) {
  case TFTPDATA: 
    if( ntohs( tftp.param ) != p->block + 1 ) 
      {
#if TFTPTRACE
	pprintf("received block is %d; expected block %d\n",
		ntohs( tftp.param ), p->block + 1);
#endif
	krn$_post( &inet_lock );
	err = 3; 
	goto DISCARD;
      }
    if( p->rem_port != sport && p->rem_port != htons( TFTPSERVER ) )
      {
	krn$_post( &inet_lock );
	/* send error packet? */
	err = 4; 
	goto DISCARD;
      }
    /*
     * packet looks ok now
     */
    if( p->rem_port == htons( TFTPSERVER ) )
      p->rem_port = sport;
    p->block++;
    p->pkt = pkt;
    p->offset = msg + sizeof( tftp_hdr_t );
    p->size = size - sizeof( tftp_hdr_t );
    if( p->size < 512 ) p->last = 1;
    krn$_stop_timer( &p->tq );
    krn$_post( &p->tq.sem, 1 );             /* post with 1 means ok */
    krn$_post( &inet_lock );
    return;
  case TFTPERROR:
    p->pkt = pkt;
    p->offset = msg + sizeof( tftp_hdr_t ); /* point to error message */
    p->size = 0;
    p->last = 1;
    p->error = ntohs( tftp.param ) + 1;     /* set error */
    krn$_stop_timer( &p->tq );
    krn$_post( &p->tq.sem, 2 );             /* post with 2 means error */
    krn$_post( &inet_lock );
    return;
  case TFTPRRQ:
  case TFTPWRQ:
  case TFTPACK:
  default:
    /*
     * should probably send an error packet for some of these cases
     */
    krn$_post( &inet_lock );
    err = 5; 
    goto DISCARD;
  }
 DISCARD:
#if TFTPTRACE
  pprintf( "tftp_receive() discarded packet for error %d\n", err );
#endif
#if MALLOCTRACE
  pprintf( "freepkt %08x\n", msg );
#endif
  freepkt( pkt, fp );
  return;
} /* tftp_receive() */

/*
 * arp code
 */
arp_pkt_t arp_template = {       /* defaults for arp packets */
  htons( HTYPE_ETHER10 ),        /* hardware protocol */
  htons( DODIP_PROTOCOL ),       /* software protocol */
  HLEN_ETHER10,                  /* hardware address length */
  PLEN_DODIP,                    /* software address length */
  htons( ARP_REQUEST ),          /* op code */
  { 0, 0, 0, 0, 0, 0 },          /* src hardware address */
  0,                             /* src software address */
  { 0, 0, 0, 0, 0, 0 },          /* dst hardware address */
  0 };                           /* dst software address */

/*
 * Given an Internet address, find corresponding hardware address.
 *
 * Handles broadcast (255.255.255.255) and local address (0.0.0.0) as well.
 * All hardware address requests, including accesses to local station address, 
 * are handled by arp_resolve.
 * Since underlying ni driver is hardwired for ethernet, this code is too.
 */
int arp_resolve( long iaddr, char *haddr, int hlen, struct FILE *net )
{
  if( hlen != 6 )
    {
      err_printf( "Arp_resolve: sorry, we are hardwired for ethernet.\n" );
      return msg_failure;
    }
  if( iaddr == INADDR_BROADCAST )
    {
      trace( "arp_resolve - request is for broadcast address\n", ARPTRACE );
      memcpy( haddr, inet_eb, hlen );
      return msg_success;
    }
  if( iaddr == 0 )
    {
      trace( "arp_resolve - request is for local address\n", ARPTRACE );
      ndl_get_nisa( net, haddr );
      return msg_success;
    }
  krn$_wait( &arp_lock );
  if( arp_cache[0].valid && iaddr == arp_cache[0].iaddr )
    {
      trace( "arp_resolve - had it in the cache\n", ARPTRACE );
      memcpy( haddr, &arp_cache[0].haddr, hlen );
      krn$_post( &arp_lock );
      return msg_success;
    }
  else
    {
      int tries;
      int backoff;
      int notfirst = 0;
      struct EVNODE *evptr;
      char varname[32];
      arp_pkt_t pkt;
      iovec_t iovec[MAX_IOVEC];

      trace( "arp_resolve - setting up for broadcast\n" , ARPTRACE );
      arp_cache[0].valid = 0;                     /* set up cache */
      arp_cache[0].iaddr = iaddr;                 /* address requested */

      arp_template.htype = htons( bootp_htype );  /* patch for TOKEN RING / ETHERNET */
      memcpy( &pkt, &arp_template, sizeof( pkt ) ); /* build arp packet */
      ndl_get_nisa( net, pkt.src_haddr );
      
      pkt.src_iaddr = hostid( net );
      pkt.dst_iaddr = iaddr;

      memset( iovec, 0, sizeof( iovec ) );
      add_iovec( iovec, &pkt, sizeof( pkt ) );

      evptr = new( struct EVNODE );
      sprintf( varname, "%.4s_arp_tries", net->ip->name );
      if( msg_success == ev_read( varname, &evptr, 0 ) )
	tries = evptr->value.integer;
      else
	tries = ARP_DEF_TRIES;
      free( evptr );

      inet_init_backoff( &backoff );

      while( tries-- > 0 )
	{
	  int wakeup;
#if ARPTRACE
	  trace( "arp_resolve - broadcasting for address\n", ARPTRACE );
	  hexdump( &pkt, sizeof( arp_pkt_t ) );
#endif
	  if( notfirst )
	    print_progress();
	  else
	    notfirst = 1;
	  net_send( iovec, INADDR_BROADCAST, ARP_PROTOCOL, net );
	  wakeup = inet_sleep( &arp_tq, inet_backoff( &backoff ) );
	  if( killpending() ) goto ARP_KILLED;
	  if( wakeup ) goto ARP_EXIT;
	}
    ARP_KILLED:
      trace( "arp killed or timed out\n", ARPTRACE );
      krn$_post( &arp_lock );
      return msg_failure;
    ARP_EXIT:
      trace( "arp succeeded\n", ARPTRACE );
      memcpy( haddr, arp_cache[0].haddr, hlen );
      krn$_post( &arp_lock );
      return msg_success;
    }
} /* arp_resolve() */

void arp_receive( struct FILE *fp, unsigned char *msg, 
		      unsigned char *src, int size, int version )
{
  arp_pkt_t *pkt = msg;
  long myaddr;
  unsigned short op;

#if ARPTRACE 
  trace( "arp_receive()\n", ARPTRACE );
  hexdump( msg, size );
#endif

  myaddr = hostid( fp );
  memcpy( &op, &pkt->op, 2 );  /* fix alignment problem */
  switch( ntohs( op ) ) {
  case ARP_REQUEST:
    {
      long dst_iaddr;
      memcpy( &dst_iaddr, &pkt->dst_iaddr, 4 );  /* fix alignment problem */
      if( myaddr && ( myaddr == dst_iaddr ) )
	{
	  op = htons( ARP_RESPONSE );
	  memcpy( &pkt->op, &op, 2 );
	  memcpy( &pkt->dst_iaddr, &pkt->src_iaddr, 4 );
	  memcpy( pkt->dst_haddr, pkt->src_haddr, pkt->hlen );
	  memcpy( &pkt->src_iaddr, &myaddr, 4 );
	  arp_resolve( 0, pkt->src_haddr, pkt->hlen, fp );
	  trace( "replying to ARP_REQUEST\n", ARPTRACE );
	  ndl_send( fp, NDL$K_VERSION_3, msg, size,
		   NDL$K_DONT_INC_LENGTH, src, ARP_PROTOCOL );
	  trace( "ARP_REQUEST handled\n", ARPTRACE );
	  return;
	}
      else
	trace( "ARP_REQUEST dropped\n", ARPTRACE );
    }
    break;
  case ARP_RESPONSE:
    {
      long src_iaddr;
      memcpy( &src_iaddr, &pkt->src_iaddr, 4 );  /* fix alignment problem */
      if( src_iaddr == arp_cache[0].iaddr )
	{
	  memcpy( arp_cache[0].haddr, pkt->src_haddr, pkt->hlen );
	  arp_cache[0].valid = 1;
#if MALLOCTRACE	  
	  pprintf( "freepkt %08x\n", msg );
#endif
	  freepkt( msg, fp );
	  trace( "ARP_RESPONSE accepted\n", ARPTRACE );
	  krn$_post( &arp_tq.sem, 1 );
	  return;
	}
      else
	trace( "ARP_RESPONSE dropped - unrequested\n", ARPTRACE );
    }
    break;
  case RARP_REQUEST:
    trace( "RARP_REQUEST dropped\n", ARPTRACE );
    break;
  case RARP_RESPONSE:
    trace( "RARP_RESPONSE dropped\n", ARPTRACE );
    break;
  default:
    trace( "arp_receive() dropped arp packet - unknown opcode\n", ARPTRACE );
    break;
  }
#if MALLOCTRACE
  pprintf( "freepkt %08x\n", msg );
#endif
  freepkt( msg, fp );
} /* arp_receive() */

/*
 * open next protocol and make sure it's a network we support
 * returns FILE * if successful; returns NULL otherwise.
 */
struct FILE *open_net( char *next )
{
  struct FILE *net;
  net = fopen( next, "sr+" );
  if( !net ) {
#if OPEN_NET_TRACE
    pprintf("couldn't open net %s\n", next);
#endif
    return NULL;
  }
  if( !isanet( net ) ) {
#if OPEN_NET_TRACE
    pprintf("%s is not a network.\n", next);
#endif
    fclose( net );
    return NULL;
  }
  return net;
} /* open_net() */


/*
 * add an entry to the per-interface data base
 * set up listener routines on that interface
 * assume 'net' is a valid network interface
 * returns entry on success; returns NULL otherwise.
 */
pidb_t *make_pidb( struct FILE *net )
{
  pidb_t *pie;

  /*
   * create database entry and insert it
   */
  pie = new( pidb_t );
  if ( !pie ) return NULL;
  memset( pie, 0, sizeof( pidb_t ) );
  pie->ip = net->ip;
  krn$_wait( &inet_lock );
  pie->blink = pidbq.blink;
  pie->flink = &pidbq;
  pidbq.blink->flink = pie;
  pidbq.blink = pie;
  krn$_post( &inet_lock );
  /*
   * start up receivers
   */
  ndl_set_rcv( net->ip, DODIP_PROTOCOL, inet_eb, ip_receive );
  ndl_set_rcv( net->ip, ARP_PROTOCOL, inet_eb, arp_receive );

  return pie;
} /* make_pidb() */



/*+
 * ============================================================================
 * = init_pidb - Initialize Per Interface Data Base                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is called to initialize a PIDB entry using a bootp
 *      request packet (default case) or nvram.  If successful, the pidb
 *      database structure's (pidb_t) inited field is set to one.
 *
 * FORM OF CALL:
 *
 *	Example from inet_driver/tftp_open
 *                       .
 *                       .
 *                      init_pidb(net)
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct FILE *net - File pointer needed for access to the per-interface
 *                         database (pidb_t).
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void init_pidb( struct FILE *net )
{
  pidb_t *pie;
  struct EVNODE *evptr;
  int status;
  char varname[32];

  /*
   * Initialize bootp database entry; the default value for
   * ez*0_inet_init is "bootp", which means that we init
   * a network bootp server.
   * 
   */
  evptr = new( struct EVNODE );
  sprintf( varname, "%.4s_inet_init", net->ip->name );
  status = ev_read( varname, &evptr, 0 );
  if ( (status == msg_success) && (!strcmp( "nvram", evptr->value.string )) )
    {
      /* 
       * copy info from nvram ev's into regular ev's
       */
      sprintf( varname, "%.4s_def_inetaddr", net->ip->name );
      if( msg_success == ev_read( varname, &evptr, 0 ) )
        {
          sprintf( varname, "%.4s_inetaddr", net->ip->name );
          ev_write( varname, evptr->value.string, 0, 0 );
        }
      sprintf( varname, "%.4s_def_sinetaddr", net->ip->name );
      if( msg_success == ev_read( varname, &evptr, 0 ) )
        {
          sprintf( varname, "%.4s_sinetaddr", net->ip->name );
          ev_write( varname, evptr->value.string, 0, 0 );
        }
      sprintf( varname, "%.4s_def_ginetaddr", net->ip->name );
      if( msg_success == ev_read( varname, &evptr, 0 ) )
        {
          sprintf( varname, "%.4s_ginetaddr", net->ip->name );
          ev_write( varname, evptr->value.string, 0, 0 );
        }

      sprintf( varname, "%.4s_def_inetfile", net->ip->name );
      if( msg_success == ev_read( varname, &evptr, 0 ) )
        {
          sprintf( varname, "%.4s_inetfile", net->ip->name );
          ev_write( varname, evptr->value.string, 0, 0 );
        }

       /* Mark entry as inited on non-bootp path if success */
       pie = lookup_pidb(net);
       if (pie)
          pie->inited = 1;
       nvram_bootp_init = 1;

    }     
  else
    {
     /*
      * Open bootp/net for side effect of initializing database.
      * Note that bootp_open() will set "pie->inited" on success.
      */
     struct FILE *bootp;
     char *newfile;
     newfile = malloc( strlen( net->ip->name ) + strlen( "bootp/" ) + 1 );
     sprintf( newfile, "bootp/%s", net->ip->name );
     nvram_bootp_init = 0;

     /* fopen will send us to bootp_open      */
     bootp = fopen( newfile, "r" );
     free( newfile );
     if( bootp ) 
       fclose( bootp );
    }
  free( evptr );

} /* end init_pidb() */


/*
 * try to find this interface's entry in the per-interface data base
 * returns entry if found; returns NULL otherwise.
 */
pidb_t *lookup_pidb( struct FILE *net )
{
  pidb_t *pie;
  krn$_wait( &inet_lock );
  for ( pie = pidbq.flink; pie != &pidbq; pie = pie->flink )
    {
      if ( net->ip == pie->ip ) 
	{
	  krn$_post( &inet_lock );
	  return pie;
	}
    }
  krn$_post( &inet_lock );
  return NULL;
} /* lookup_pidb() */
/*
 * return ip address of net if known, else 0
 */
unsigned int hostid( struct FILE *net )
{
  unsigned int iaddr;
  pidb_t *pie;
  trace( "hostid\n", 0 );
  if( !net )
    return 0;
  pie = lookup_pidb( net );
  if( !pie )
    return 0;
  krn$_wait( &inet_lock );
  iaddr = pie->bootp.yiaddr;
  krn$_post( &inet_lock );
  return iaddr;
} /* hostid() */

/*
 * return ip address of gateway if known, else 0
 */
unsigned int gateid( struct FILE *net )
{
  unsigned int iaddr;
  pidb_t *pie;
  trace( "gateid\n", 0 );
  if( !net )
    return 0;
  pie = lookup_pidb( net );
  if( !pie )
    return 0;
  krn$_wait( &inet_lock );
  iaddr = pie->bootp.giaddr;
  krn$_post( &inet_lock );
  return iaddr;
} /* gateid() */
	
/*
 * sleep on tq.sem until woken up
 * returns sem value on success ( woken up ) 
 * otherwise returns 0 ( timed out or killed )
 *
 * timer posts semaphore with value of TIMEOUT$K_SEM_VAL
 * whoever wakes us up posts with a value greater than 0
 * assume:
     krn$_seminit( &( tq.sem ), 0, "inet_sleep" );
     krn$_init_timer( &tq );
 * should always test killpending() immediately after inet_sleep()
 */
int inet_sleep( struct TIMERQ *tq, int secs )
{
  int wakeup = 0;
  while( secs-- > 0 )
    {
      krn$_start_timer( tq, 1000 );
      wakeup = krn$_wait( &( tq->sem ) ) & (~TIMEOUT$K_SEM_VAL);
      krn$_stop_timer( tq );
      if( wakeup ) return wakeup;
      if( killpending() ) return 0;
    }
  return 0;
} /* inet_sleep() */

/*
 * return time in seconds since start
 */
unsigned long inet_time( unsigned long start ) {
  return poweron_msec[0]/1000 - start;
} /* inet_time() */

/*
 * compute random backoff time 
 * average starts at ~4 seconds, increases exponentially to ~64 seconds
 *	backoff			range	average
	2	2 + 0..3	2..5	3.5
	4	4 + 0..7	4..11	7.5
	8	8 + 0..15	8..23	15.5
	16	16 + 0..31	16..47	31.5
	32	32 + 0..63	32..95	63.5
 * see RFC 951 section 7.2
 */
int inet_backoff( int *backoff ) {
  int i = *backoff;
  int j = i<<1;
  if ( i<32 ) *backoff = j;
  return i + ( ( j-1 ) & inet_random() );
}
void inet_init_backoff( int *backoff ) {
  *backoff = 2;
}

#if !MODULAR
extern ev_sev( );
extern ev_sev_init( );

struct set_param_table inet_init_table[] = {
	{"bootp", 0},
	{"nvram", 0},
	{0}
};

struct env_table inet_protocols_ev = {
	0, "MOP",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ROUTINE,
	inet_protocols_validate, ev_sev, 0, ev_sev_init
};

struct env_table inet_bootp_tries_ev = {
	0, 3,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init
};

struct env_table inet_arp_tries_ev = {
	0, 3,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init
};

struct env_table inet_tftp_tries_ev = {
	0, 3,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init
};

struct env_table inet_inetaddr_ev = {
	0, "0.0.0.0",
	EV$K_NODELETE | EV$K_STRING | EV$K_ROUTINE,
	inetaddr_validate, inetaddr_wr, inetaddr_rd, inetaddr_init
};

struct env_table inet_ginetaddr_ev = {
	0, "0.0.0.0",
	EV$K_NODELETE | EV$K_STRING | EV$K_ROUTINE,
	inetaddr_validate, inetaddr_wr, inetaddr_rd, inetaddr_init
};

struct env_table inet_sinetaddr_ev = {
	0, "0.0.0.0",
	EV$K_NODELETE | EV$K_STRING | EV$K_ROUTINE,
	inetaddr_validate, inetaddr_wr, inetaddr_rd, inetaddr_init
};

struct env_table inet_inetfile_ev = {
	0, "",	
	EV$K_NODELETE | EV$K_STRING,
	0, inetaddr_wr, inetaddr_rd, inetaddr_init
};

struct env_table inet_def_inetaddr_ev = {
	0, "0.0.0.0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ROUTINE,
	inetaddr_validate, ev_sev, 0, ev_sev_init
};

struct env_table inet_def_ginetaddr_ev = {
	0, "0.0.0.0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ROUTINE,
	inetaddr_validate, ev_sev, 0, ev_sev_init
};

struct env_table inet_def_sinetaddr_ev = {
	0, "0.0.0.0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ROUTINE,
	inetaddr_validate, ev_sev, 0, ev_sev_init
};

struct env_table inet_def_inetfile_ev = {
	0, "",	
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init
};

struct env_table inet_def_subnetmask_ev = {
	0, "0.0.0.0",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE | EV$K_ROUTINE,
	inetaddr_validate, ev_sev, 0, ev_sev_init
};

struct env_table inet_inet_init_ev = {
	0, "bootp",
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	inet_init_table, ev_sev, 0, ev_sev_init
};

struct env_table inet_bootp_server_ev = {
	0, "",	
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init
};

struct env_table inet_bootp_file_ev = {
	0, "",	
	EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_sev, 0, ev_sev_init
};

struct inet_ev_pairs { struct env_table *et; char *name; } inet_ev_pairs[] = {
	{ &inet_protocols_ev, "_protocols" },
	{ &inet_bootp_tries_ev, "_bootp_tries" },
	{ &inet_arp_tries_ev, "_arp_tries" },
	{ &inet_tftp_tries_ev, "_tftp_tries" },
	{ &inet_inetaddr_ev, "_inetaddr" }, 
	{ &inet_ginetaddr_ev, "_ginetaddr" }, 
	{ &inet_sinetaddr_ev, "_sinetaddr" }, 
	{ &inet_inetfile_ev, "_inetfile" }, 
	{ &inet_def_inetaddr_ev, "_def_inetaddr" }, 
	{ &inet_def_ginetaddr_ev, "_def_ginetaddr" }, 
	{ &inet_def_sinetaddr_ev, "_def_sinetaddr" }, 
	{ &inet_def_inetfile_ev, "_def_inetfile" }, 
        { &inet_def_subnetmask_ev, "_def_subnetmask" },
	{ &inet_inet_init_ev, "_inet_init" }, 
	{ &inet_bootp_server_ev, "_bootp_server" }, 
	{ &inet_bootp_file_ev, "_bootp_file" }, 
	{ 0, 0 }
};

/*
 * Initialize environment variables.
 */
void inet_set_env( char *name )
    {
    char ev_name[32];
    struct inet_ev_pairs *iep;
    struct env_table *et;

    for( iep = &inet_ev_pairs; et = iep->et; iep++ )
	{
	sprintf( ev_name, "%4.4s%s", name, iep->name );
	et->ev_name = ev_name;
	ev_init_ev( et, 0 );
	}
    }
#endif
/*
 * verify that argument is a valid dotted.decimal.internet.address
 * returns msg_success if argument is valid; otherwise returns msg_failure 
 */
int inetaddr_validate( char **dda )
{
#if EVTRACE && 0
  pprintf( "inetaddr_validate: %s\n", *dda );
#endif
  return string2inetaddr( NULL, *dda );
} /* inetaddr_validate() */

/*
 * Inet_protocols_validate() is the validation routine called for 
 * the environment variables "eza0_protocols" and "ezb0_protocols".
 * These variables should contain a list of zero or more protocol names.  
 * The list is separated by spaces, tabs or commas.
 * We don't care about redundant entries or multiple commas; eg, "mop,,,mop"
 * is acceptable. If the string contains only whitespace it is truncated to 
 * the null string; eg, ",,,,,,," is truncated to "".
 * Examples:

 set eza0_protocols     ""
>>> sho eza0_protocols
eza0_protocols      	                
>>> set eza0_protocols     "mop"
>>> sho eza0_protocols
eza0_protocols      	MOP             
>>> set eza0_protocols     "bootp"
>>> sho eza0_protocols
eza0_protocols      	BOOTP           
>>> set eza0_protocols     "mop,bootp"
>>> sho eza0_protocols
eza0_protocols      	MOP,BOOTP       
>>> set eza0_protocols     "mop bootp"
>>> sho eza0_protocols
eza0_protocols      	MOP,BOOTP       
>>> set eza0_protocols     "bootp,mop"
>>> sho eza0_protocols
eza0_protocols      	MOP,BOOTP       
>>> set eza0_protocols     "bootp mop"
>>> sho eza0_protocols
eza0_protocols      	MOP,BOOTP       
>>> set eza0_protocols     "   bootp  ,  mop    "
>>> sho eza0_protocols
eza0_protocols      	MOP,BOOTP       
>>> set eza0_protocols     "   mop  ,  bootp    "
>>> sho eza0_protocols
eza0_protocols      	MOP,BOOTP       
>>> set eza0_protocols     "mOp"
>>> sho eza0_protocols
eza0_protocols      	MOP             
>>> set eza0_protocols     "bOOtP"
>>> sho eza0_protocols
eza0_protocols      	BOOTP           
>>> set eza0_protocols     ",,,,,,,,mop,  ,,,,,mop,,, ,,bootp"
>>> sho eza0_protocols
eza0_protocols      	MOP,BOOTP       
>>> set eza0_protocols     "    ,,"
>>> sho eza0_protocols
eza0_protocols      	                
>>> set eza0_protocols     "mop mom"
'mom' is not supported; supported protocols are 'MOP' and 'BOOTP'.
bad value - eza0_protocols not modified
>>> sho eza0_protocols
eza0_protocols      	                
>>> set eza0_protocols     "decnet"
'decnet' is not supported; supported protocols are 'MOP' and 'BOOTP'.
bad value - eza0_protocols not modified
>>> sho eza0_protocols
eza0_protocols      	                
>>> set eza0_protocols     "inet"
'inet' is not supported; supported protocols are 'MOP' and 'BOOTP'.
bad value - eza0_protocols not modified
>>> sho eza0_protocols
eza0_protocols      	                
>>> set eza0_protocols     "decnet inet"
'decnet' is not supported; supported protocols are 'MOP' and 'BOOTP'.
bad value - eza0_protocols not modified
>>> sho eza0_protocols
eza0_protocols      	                
>>> set eza0_protocols     "mop inet"
'inet' is not supported; supported protocols are 'MOP' and 'BOOTP'.
bad value - eza0_protocols not modified
>>> sho eza0_protocols
eza0_protocols      	                
>>> set eza0_protocols     "decnet bootp"
'decnet' is not supported; supported protocols are 'MOP' and 'BOOTP'.
bad value - eza0_protocols not modified
>>> sho eza0_protocols
eza0_protocols      	                
>>> set eza0_protocols     "mop;bootp"
'mop;bootp' is not supported; supported protocols are 'MOP' and 'BOOTP'.
bad value - eza0_protocols not modified
>>> sho eza0_protocols
eza0_protocols      	                
>>>

 *
 *
 */
int inet_protocols_validate( char **s )
{
  int leng, i;
  char *buf;
  int protocols;
  char *proto_strings[] = { "", "MOP", "BOOTP", "MOP,BOOTP" };

  leng = strlen( *s ) + 1;
  buf = malloc( leng );

  protocols = 0;
  i = 0;
  while( strelement( *s, i++, " \t,", buf ) )
    {
      if (strcmp_nocase( buf, proto_strings[1] ) == 0)
        {
          protocols |= 1;
	}
      else if (strcmp_nocase( buf, proto_strings[2] ) == 0)
        {
          protocols |= 2;
	}
      else
        {	
          err_printf( "'%s' is not supported;", buf );
          err_printf( " supported protocols are '%s' and '%s'.\n",
                     proto_strings[1], proto_strings[2] );
          free(buf);
          return msg_failure;
	}
    }
  strcpy( *s, proto_strings[ protocols ] );
  free(buf);
  return msg_success;
} /* inet_protocols_validate() */

/*
 * called when environment variable is inited
 * does nothing at present
 */
int inetaddr_init( char *dda )
{
  trace( "inetaddr_init\n", EVTRACE );
  return msg_success;
} /* inetaddr_init() */

/*
 * read from pidb and return as appropriately formatted string
 * evp is original EVNODE in EVNODE list
 * evp_copy is EVNODE returned to caller of ev_read 
 * put value in evp_copy->value.string
 */
int inetaddr_rd( char *name, struct EVNODE *evp, struct EVNODE *evp_copy )
{
  char *param, *netname;
  struct FILE *net;
  int status;
  pidb_t *pie;

  trace( "inetaddr_rd\n", EVTRACE );
  if (ev_initing)
    return msg_success;

  /*
   * break name into network name and parameter name
   */
  netname = malloc( strlen( name ) + 1 );
  strcpy( netname, name );
  param = strchr( netname, '_' );
  if( !param ||                       /* no '_' */
     param == netname ||              /* '_' at start */
     *(param + 1) == 0 )              /* '_' at end */
    {
      free( netname );
      return msg_failure;
    }
  *param++ = '\0';

  /*
   * locate database entry
   */
  net = open_net( netname );
  if( !net )
    {
      free( netname );
      return msg_failure;
    }
  pie = lookup_pidb( net );
  if( !pie )
    {
#if 1
      pie = make_pidb( net );
#else
      return msg_failure;
#endif
    }
  
  /*
   * copy appropriate data from database
   */
  status = msg_success;
  krn$_wait( &inet_lock );
  if( !strcmp( param, "inetfile" ) )
    strncpy( evp_copy->value.string, pie->bootp.file, 
	    sizeof( evp_copy->value.string ) );
  else
    {
      unsigned long iaddr;
      if( !strcmp( param, "inetaddr" ) )
	iaddr = pie->bootp.yiaddr;
      else if( !strcmp( param, "sinetaddr" ) )
	iaddr = pie->bootp.siaddr;
      else if( !strcmp( param, "ginetaddr" ) )
	iaddr = pie->bootp.giaddr;
      else
	status = msg_failure;
      if( status == msg_success )
	status = inetaddr2string( ntohl( iaddr ), evp_copy->value.string );
    }
  krn$_post( &inet_lock );
  fclose( net );
  free( netname );
  return status;
} /* inetaddr_rd() */

/*
 * convert appropriately formatted string and write to pidb 
 */
int inetaddr_wr( char *name, struct EVNODE *evp )
{
  char *param, *netname;
  struct FILE *net;
  int status;
  pidb_t *pie;

  trace( "inetaddr_wr\n", EVTRACE );
  if (ev_initing)
    return msg_success;

  /*
   * break name into network name and parameter name
   */
  netname = malloc( strlen( name ) + 1 );
  strcpy( netname, name );
  param = strchr( netname, '_' );
  if( !param ||                       /* no '_' */
     param == netname ||              /* '_' at start */
     *(param + 1) == 0 )              /* '_' at end */
    {
      free( netname );
      return msg_failure;
    }
  *param++ = '\0';

  /*
   * locate database entry
   */
  net = open_net( netname );
  if( !net )
    {
      free( netname );
      return msg_failure;
    }
  pie = lookup_pidb( net );
  if( !pie )
    {
#if 1
      pie = make_pidb( net );
#else
      return msg_failure;
#endif
    }
  
  /*
   * convert data and copy into database
   */
  status = msg_success;
  krn$_wait( &inet_lock );
  if( !strcmp( param, "inetfile" ) )
    strncpy( pie->bootp.file, evp->value.string,  
	    sizeof( pie->bootp.file ) );
  else
    { 
      unsigned long iaddr;
      status = string2inetaddr( &iaddr, evp->value.string );
      iaddr = htonl( iaddr );
      if( status == msg_success )
	{
	  if( !strcmp( param, "inetaddr" ) )
	    pie->bootp.yiaddr = iaddr;
	  else if( !strcmp( param, "sinetaddr" ) )
	    pie->bootp.siaddr = iaddr;
	  else if( !strcmp( param, "ginetaddr" ) )
	    pie->bootp.giaddr = iaddr;
	  else
	    status = msg_failure;
	}
    }
  krn$_post( &inet_lock );
  fclose( net );
  free( netname );
  pie->inited = 1;
  return status;
} /* inetaddr_wr() */

/*
 * convert string in dotted.decimal.internet.format to binary (host byte order)
 * returns msg_failure if string has bad format or is not a legal address
 * otherwise returns msg_success on success
 * if addr is NULL, then just validate address string
 */
int string2inetaddr( unsigned long *addr, char *s )
{
  int i, a;
  unsigned long iaddr;

#if EVTRACE && 0
  pprintf( "string2inetaddr: %s", s );
#endif
  for( i = 0; i < 4; i++ )        /* convert dotted decimal address */
    {
      a = 0;
      while( isdigit( *s ) )
	{
	  a = a * 10 + *s - '0';
	  if( a > 0xff ) 
	    {
#if EVTRACE && 0
	      pprintf( "byte %d overflowed\n", i );
#endif
	      return msg_failure;
	    }
	  s++;
	}
      iaddr = (iaddr << 8) + a;
      if( i < 3 && *s++ != '.' ) 
	{
#if EVTRACE && 0
	  pprintf( "expected a dot after byte %d\n", i );
#endif
	  return msg_failure;
	}
    }
  if( *s != 0 )
    {
#if EVTRACE && 0
      pprintf( "excess chars at end\n" );
#endif
      return msg_failure;
    }
  if( addr ) *addr = iaddr;
  trace( "ok\n", EVTRACE && 0);
  return msg_success;
} /* string2inetaddr() */

/*
 * Convert an int to a null terminated string in dotted.decimal.internet.format.
 * Must pass in string buffer at least 16 bytes long.
 * Addr is treated as an int, not a char[4], which implies host byte ordering.
 */
int inetaddr2string( unsigned long addr, char *s )
{

  trace( "inetaddr2string\n", EVTRACE );

  sprintf( s, "%d.%d.%d.%d", 
	  (addr >> 24) & 0xff,
	  (addr >> 16) & 0xff,
	  (addr >> 8) & 0xff,
	  addr & 0xff
	  );
  return msg_success;
} /* inetaddr2string() */

/*
 * random number generator
 */
int inet_random( void ) {
  return inet_random_seed = random( inet_random_seed, 32 );
}

/*
 * compute Internet style one's complement check sum:
 * "16 bit one's complement of the one's complement sum of 
 *  all 16 bit words in the header"
 * bufsize is number of bytes
 */
unsigned short inet_cksum( unsigned short *buf, int bufsize )
{
  int odd = bufsize & 1;
  unsigned long sum = 0;
#if CKSUMTRACE
  pprintf( "inet_cksum: buf:%08x bufsize: %d\n", buf, bufsize );
#endif
  bufsize >>= 1;
  while ( bufsize-- ) 
    sum += *buf++;
  if ( odd ) {
    char tmp[2];
    tmp[0] = *( char* )buf;
    tmp[1] = 0;
    sum += *( unsigned short* )tmp;
  }
  sum = ( sum >> 16 ) + ( sum & 0xffff );
  sum += ( sum >> 16 );
  /* assert( 0x0000 <= sum && sum <= 0xffff ); */
  return ~sum;
} /* inet_cksum() */

#define LINESIZE 16 /* bytes dumped per line */
void hexdump(void *p, int size)
{
  int i;
  for (i=0; i<size; i++) {
    if ((i%LINESIZE)==0) pprintf("%-4d  ",i);
    pprintf("%02x",(((char*)p)[i])&0xff);
    pprintf("%c", ((i+1)%LINESIZE) ? ' ' : '\n');
  }
  pprintf("\n");
} /* hexdump() */

void inet_shutdown (void)
{
#if MODULAR
}
void inet_shutdown1 (void)
{
#endif
  krn$_semrelease ( &arp_lock );
  krn$_semrelease ( &arp_tq.sem );
  krn$_release_timer( &arp_tq );

  return;
}

protoendif(__INET__)
