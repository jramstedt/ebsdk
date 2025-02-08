#ifndef __ETHER_H_LOADED
#define __ETHER_H_LOADED
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

/*
 *  $Id: ether.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $;
 */

/*
 *  Contains all the externally visible function prototypes & variables
 *  for the modules that make up the ether directory.
 *
 * $Log: ether.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.13  1996/05/26  20:01:26  fdh
 * Included reference to isa_ether_device_map.
 *
 * Revision 1.12  1996/05/22  21:28:06  fdh
 * Added am79c960 conditional.
 *
 * Revision 1.11  1995/12/12  21:18:38  cruz
 * Changed prototype for netman_start_monitor_device() to return a value..
 *
 * Revision 1.10  1995/12/07  23:45:19  cruz
 * Added netman_stop_all_devices().
 *
 * Revision 1.9  1995/10/31  22:47:19  cruz
 * Added #ifdef NEED21040 around prototypes for 21040 driver routines.
 *
 * Revision 1.8  1995/10/30  16:58:33  cruz
 * Commented out routine(s) not in use.
 *
 * Revision 1.7  1995/10/27  15:42:33  cruz
 * Commented out reference to ether_device_number() since noone is currently using it.
 *
 * Revision 1.6  1995/10/27  15:14:09  cruz
 * Added extern reference for etherdump and etherprom variables.
 *
 * Revision 1.5  1995/10/20  19:03:01  cruz
 * Consolidated the function prototypes and external variable declarations
 * of all the modules in the ether directory into this file.
 *
 * Revision 1.4  1994/08/05  20:13:47  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/06/28  20:08:21  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.2  1994/06/20  14:18:59  fdh
 * Fixup header file preprocessor #include conditionals.
 *
 * Revision 1.1  1994/06/20  12:54:45  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "lib.h"
#include "address.h"


#define MAX_ETHER_DEVICES 10    /* Max number of ethernet devices supported. */

/*
 * Global variables declaration go here
 */

/* assign memory for the device_no => ISA IO port base address table */
/* this table is ETHERNET SPECIFIC at present */

extern int isa_ether_device_map[MAX_ETHER_DEVICES];

 
/*
 *  Their space is allocated in netman.c
 */
extern BOOLEAN etherdump;		/* debug - print Ethernet traffic if */
                                        /* TRUE and PRTRACE enabled */
extern BOOLEAN etherprom;		/* Enable promiscuous mode if TRUE */


/*::::::::::::::
DEC21040.c
::::::::::::::*/
#ifdef NEED21040
extern int DECchip_21040_device_register(int device_no);
extern void DECchip_21040_device_init_module(void );
#endif

/*::::::::::::::
am79c960.c
::::::::::::::*/
#ifdef NEEDAM79C960
extern int am79c960_device_register(int device_no);
extern void am79c960_device_init_module(void );
#endif

/*::::::::::::::
arp.c
::::::::::::::*/
extern void arp_init(int device_no);
extern void arp_set_device_addr(int device_no , ip_addr ip);
extern int arp_resolve(int device_no , ip_addr ip , mac_addr mac);
extern void arp_init_module(void );
extern void arp_show(void );


/*::::::::::::::
bootp.c
::::::::::::::*/
extern int bootp_boot(int device_no , const char input_file_name[128] , ip_addr server_addr , char boot_file_name[128]);
extern void bootp_init_module(void );

/*::::::::::::::
eaddr.c
::::::::::::::*/
#ifdef NEED_ETHERNET_ADDRESS
extern void get_eaddr(void);
extern int ethernet_address(ui argc , ub * s);
extern mac_addr eaddr;
#endif /* NEED_ETHERNET_ADDRESS */


/*::::::::::::::
edevice.c
::::::::::::::*/
extern int ether_device_init(int device_no);
extern int ether_device_read(int device_no, unsigned char balance_buffer[ETHER_BUFFER_SIZE], 
			     unsigned char ** frame_buffer);
extern int ether_device_write(int device_no, unsigned char * frame,  int size, 
			      unsigned char frame_buffer[ETHER_BUFFER_SIZE],
			      unsigned char ** balance_buffer);
#ifdef NOT_IN_USE
extern int ether_device_flush(int device_no);
#endif
extern void ether_device_print_stats(int device_no);
extern void ether_device_preg(int device_no);
extern void ether_device_clear_interrupts(int device_no);
extern void ether_device_get_hw_address(int device_no , mac_addr hw_address);
extern void ether_device_register(int device_no,
				  int (* init_func)(int device_no),
				  int (* read_func)(int device_no,
						    unsigned char balance_buffer[ETHER_BUFFER_SIZE], 
						    unsigned char ** frame_buffer),
				  int (* write_func)(int device_no,
						     unsigned char * frame,
						     int size, 
						     unsigned char frame_buffer[ETHER_BUFFER_SIZE],
						     unsigned char ** balance_buffer),
				   int (* flush_func)(int device_no),
				  void (* stats_func)(int device_no),
				  void (* preg_func)(int device_no),
				  void (* clear_interrupts_func)(int device_no),
				  void (* hw_addr_func)(int device_no, mac_addr hw_address) ,
				  void (* stop_func)(int device_no),
				  char *description
				  );
extern void ether_device_init_module(void );
extern void ether_device_show(void);
extern void ether_device_stop(int device_no);

#ifdef NOT_IN_USE
extern int ether_device_number(int previous_device_number);
#endif

/*::::::::::::::
ethernet.c
::::::::::::::*/
typedef void (* ethernet_packet_handler)(void * protocol_data,
              int device_no, unsigned char * packet, int size,
              mac_addr source, unsigned char frame_buffer[ETHER_BUFFER_SIZE],
	      unsigned char ** balance_buffer);

extern int ethernet_register_protocol(int device_no , ethernet_protocol_id protocol_id , 
                                      ethernet_packet_handler processing_function , void * protocol_data);
extern int ethernet_process_one_packet(int device_no, unsigned char input_buffer[ETHER_BUFFER_SIZE], 
				       unsigned char ** output_buffer );
extern void ethernet_input(int device_no, unsigned char * frame, int size,
			   unsigned char input_buffer[ETHER_BUFFER_SIZE], 
			   unsigned char ** balance_buffer );
extern int ethernet_write(int device_no, unsigned char * packet, mac_addr destination,
			  ethernet_protocol_id protocol_id, int size, 
			  unsigned char frame_buffer[ETHER_BUFFER_SIZE],
			  unsigned char ** balance_buffer);
extern int ethernet_data_offset(void );
extern void ethernet_printpacket(unsigned char * p , int frame_size);
extern void ethernet_printaddress(mac_addr address);
extern ui ntoh(unsigned char * array , unsigned int size);

/*::::::::::::::
ip.c
::::::::::::::*/
typedef void (* ip_packet_handler)(void * protocol_data,
				   int device_no,
				   unsigned char * packet,
				   int size,
				   ip_addr source,
				   ip_addr destination,
				   unsigned char frame_buffer[ETHER_BUFFER_SIZE],
				   unsigned char ** balance_buffer);

extern void ip_init(int device_no);
extern void ip_set_device_addr(int device_no , ip_addr ip);
extern int ip_register_protocol(int device_no , ip_protocol_id protocol_id , 
                                ip_packet_handler processing_function , void * protocol_data);
extern int ip_write(int device_no, unsigned char * packet, ip_addr destination,
		    ip_protocol_id protocol_id, int size, 
		    unsigned char frame_buffer[ETHER_BUFFER_SIZE],
		    unsigned char ** balance_buffer);
extern int ip_data_offset(void );
extern void ip_get_device_address(int device_no , ip_addr ip);
extern void ip_printaddress(ip_addr address);


/*::::::::::::::
net_buff.c
::::::::::::::*/
extern char * etherbuffers;
extern void * net_buffer_alloc(int size);
extern void net_buffer_init_module(void );


/*::::::::::::::
netboot.c
::::::::::::::*/
extern int netboot(int argc , const char * file);


/*::::::::::::::
netman.c
::::::::::::::*/
extern int monitor_ether_device;
extern void netman_setup(void );
extern int netman_set_monitor_device(int device_no);
extern int netman_start_monitor_device(void );
extern int netman_monitor_device_started(void );
extern void netman_stop_monitor_device(void );
extern void netman_stop_all_devices(void );

/*::::::::::::::
tftp.c
::::::::::::::*/
extern int tftp_load(int device_no , char file_name[128] , ip_addr tftp_server_addr , unsigned char * load_addr);
extern void tftp_init_module(void );


/*::::::::::::::
udp.c
::::::::::::::*/
typedef void (* udp_packet_handler)(void * protocol_data,
				   int device_no,
				   unsigned char * packet,
				   int size,
				   udp_port source_port,
				   ip_addr source_address,
				   unsigned char frame_buffer[ETHER_BUFFER_SIZE],
				   unsigned char ** balance_buffer);

extern void udp_init(int device_no);
extern int udp_register_well_known_port(int device_no , udp_port port ,
                                        udp_packet_handler processing_function , 
                                        void * protocol_data);
extern int udp_create_port(int device_no , udp_packet_handler processing_function ,
                           void * protocol_data , udp_port port);
extern void udp_delete_port(int device_no , udp_port port);
extern int udp_write(int device_no, unsigned char * packet, udp_port destination_port,
		     ip_addr destination_addr, udp_port source_port, int size, 
		     unsigned char frame_buffer[ETHER_BUFFER_SIZE],
		     unsigned char ** balance_buffer);
extern int udp_data_offset(void );

#endif /* __ETHER_H_LOADED */

