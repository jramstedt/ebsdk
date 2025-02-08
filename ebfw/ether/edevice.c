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
__attribute__((unused)) static const char *rcsid = "$Id: edevice.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif

/*
 * This module provides a common interface to all ethernet devices
 *
 * $Log: edevice.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.19  1996/06/13  03:29:40  fdh
 * Added the isa_ether_device_map definition.
 *
 * Revision 1.18  1995/11/10  21:50:19  fdh
 * Uppercased Ethernet messages for Debi.
 *
 * Revision 1.17  1995/10/30  16:53:25  cruz
 * Commented out routine(s) not in use.
 *
 * Revision 1.16  1995/10/27  15:42:15  cruz
 * Commented out ether_device_number since noone is currently using it.
 *
 * Revision 1.15  1995/10/20  20:05:02  cruz
 * Cleanup for lint
 *
 * Revision 1.14  1995/10/20  18:42:12  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.13  1995/10/18  18:06:16  fdh
 * Added the ether_device_stop() function.
 *
 * Revision 1.12  1995/10/12  18:19:46  cruz
 * Removed include for prtrace.h
 *
 * Revision 1.11  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.10  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.9  1994/06/22  15:11:46  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.8  1994/06/03  20:04:41  fdh
 * Replaced <stdio.h> with <stddef.h>.
 *
 * Revision 1.7  1994/01/19  10:55:52  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.6  1993/11/22  14:57:43  rusling
 * Added support for eshow command, plus some post
 * merge fixups.
 *
 *
 * Revision 1.2  1993/10/01  15:58:43  berent
 * Added module initialisation function
 *
 * Revision 1.1  1993/08/06  09:51:52  berent
 * Initial revision
 *
 *
 */

#include "ether.h"


/* Device registration structure */
static struct {
    int entry_in_use;
    int (* init_func)(int device_no);
    int (* read_func)(int device_no,
		      unsigned char balance_buffer[ETHER_BUFFER_SIZE], 
		      unsigned char ** frame_buffer);
    int (* write_func)(int device_no,
		       unsigned char * frame,
		       int size, 
		       unsigned char frame_buffer[ETHER_BUFFER_SIZE],
		       unsigned char ** balance_buffer);
    int (* flush_func)(int device_no);
    void (* stats_func)(int device_no);
    void (* preg_func)(int device_no);
    void (* clear_interrupts_func)(int device_no);
    void (* hw_addr_func)(int device_no, mac_addr hw_address);    
    void (* stop_func)(int device_no);
    char *description;
} device_table[MAX_ETHER_DEVICES];


/* assign memory for the device_no => ISA IO port base address table */
/* this table is ETHERNET SPECIFIC at present */

int isa_ether_device_map[MAX_ETHER_DEVICES];


/* ether_device_init - initialises an ethernet device
 *
 * Arguments:
 *
 *    device_no    - device number
 *
 * Returned value
 *    TRUE         - initialised succesfully
 *    FALSE        - error on initialisation
 */ 
int ether_device_init(int device_no)
{
    if(device_no >= 0 &&
       device_no < MAX_ETHER_DEVICES &&
       device_table[device_no].entry_in_use) {
	return (*device_table[device_no].init_func)(device_no);
    } else {
	printf("Error: request received to initialize unknown device %d\n", device_no);
	return FALSE;
    }
}
	

/* ether_device_read - reads a frame from an ethernet device
 *
 * Arguments:
 *
 *    device_no      - device number
 *    balance_buffer - spare buffer to maintain buffer balance
 *    frame_buffer   - returned buffer containing frame
 *
 * Returned value:
 *
 *    positive     - size of frame read; frame copied into buffer if <= size.
 *       0         - nothing to read
 *    negative     - error on read
 *
 * DESCRIPTION:
 *    This function reads a frame from the device if there is one ready to be read. The
 *    frame read is returned in frame_buffer.  To maintain the driver's pool of buffers
 *    the caller must give the driver an unused buffer (balance_buffer) before the
 *    driver returns the frame.  Once this function has been called balance_buffer must
 *    not be used by the calling software.
 *
 *    On return frame_buffer will always point to a valid buffer.  If the read has failed
 *    (result <= 0) the contents of this buffer will not be valid.  Note that whether or
 *    not the read succeeds it is undefined whether the same buffer will be returned as
 *    frame_buffer as was passed down as balance_buffer.
 */
extern int ether_device_read(int device_no, 
			     unsigned char balance_buffer[ETHER_BUFFER_SIZE], 
			     unsigned char ** frame_buffer)
{
    if(device_no >= 0 &&
       device_no < MAX_ETHER_DEVICES &&
       device_table[device_no].entry_in_use) {
	return (*device_table[device_no].read_func)(device_no, balance_buffer, frame_buffer);
    } else {
	printf("Error: request received to read from unknown device %d\n", device_no);
	*frame_buffer = balance_buffer;
	return -1;
    }
}

/* ether_device_write - queue a frame to be sent on an ethernet device
 *
 * Arguments:
 *
 *    device_no         - device number
 *    frame_buffer      - buffer containing frame
 *    frame             - the frame itself
 *    size              - size of frame
 *    balance_buffer    - returned buffer to maintain buffer balance
 *
 * Returned value:
 *
 *    TRUE -  succefully queued for sending
 *    FALSE - unable to send
 *
 * DESCRIPTION:
 *    This function writes a frame to the device  To maintain the caller's pool of buffers
 *    the driver must give the caller an unused buffer (balance_buffer) in return for
 *    the buffer containing the frame to be transmitted.  
 *
 *    If the send succeeds then frame_buffer must not be accessed by the caller after this
 *    call.  If the send can't be queued then frame_buffer will remain valid and
 *    the returned value of balance_buffer is undefined and must not be used.
 *
 */
extern int ether_device_write(int device_no, 
			      unsigned char * frame,
			      int size, 
			      unsigned char frame_buffer[ETHER_BUFFER_SIZE],
			      unsigned char ** balance_buffer)
{
    if(device_no >= 0 &&
       device_no < MAX_ETHER_DEVICES &&
       device_table[device_no].entry_in_use) {
	return (*device_table[device_no].write_func)(device_no, frame, size,  frame_buffer, balance_buffer);
    } else {
	printf("Error: request received to write to unknown device %d\n", device_no);
	return FALSE;
    }
}

#ifdef NOT_IN_USE
/* ether_device_flush - wait for all writes on an ethernet device to complete
 *
 * Argument:
 *
 *    device_no - device to be flushed, -1 means flush all devices
 */
int ether_device_flush(int device_no)
{
    if(device_no >= 0 &&
       device_no < MAX_ETHER_DEVICES &&
       device_table[device_no].entry_in_use) {
	return (*device_table[device_no].flush_func)(device_no);
    }

    printf("Error: request received to flush unknown device %d\n", device_no);    
    return 0;
}
#endif

/* ether_device_print_stats - print device statistics
 *
 * Argument:
 *
 *     device_no - device number, -1 means all devices
 */
extern void ether_device_print_stats(int device_no)
{
    if(device_no >= 0 &&
       device_no < MAX_ETHER_DEVICES &&
       device_table[device_no].entry_in_use) {
	(*device_table[device_no].stats_func)(device_no);
    } else {
	printf("Error: request received to print the statistics for an unknown device %d\n", device_no);
    }
}
/* ether_device_preg - print device registers
 *
 * Argument:
 *
 *     device_no - device number
 */
void ether_device_preg(int device_no)
{
    if(device_no >= 0 &&
       device_no < MAX_ETHER_DEVICES &&
       device_table[device_no].entry_in_use) {
	(*device_table[device_no].preg_func)(device_no);
    } else {
	printf("Error: request received to print the registers for an unknown device %d\n", device_no);
    }
}

/* ether_device_clear_interrupts - clear all interrupts from an ethernet device
 *
 * Argument:
 *
 *     device_no - device number, -1 means all devices
 */
void ether_device_clear_interrupts(int device_no)
{
    if(device_no >= 0 &&
       device_no < MAX_ETHER_DEVICES &&
       device_table[device_no].entry_in_use) {
	(*device_table[device_no].clear_interrupts_func)(device_no);
    } else {
	printf("Error: request received to clear the interrupts on an unknown device %d\n", device_no);
    }
}

/* ether_device_get_hw_address - gets the hardware mac address of the device
 *
 * Arguments:
 *
 *     device_no - device_number
 *     hw_address - returned hardware address
 */
void ether_device_get_hw_address(int device_no, mac_addr hw_address)
{
    if(device_no >= 0 &&
       device_no < MAX_ETHER_DEVICES &&
       device_table[device_no].entry_in_use) {
	(*device_table[device_no].hw_addr_func)(device_no, hw_address);
    } else {
	printf("Error: request received to get the hardware address of an unknown device %d\n", device_no);
    }
}

/* ether_device_stop - disables a device
 *
 * Argument:
 *
 *     device_no - device number
 */
void ether_device_stop(int device_no)
{
    if(device_no >= 0 &&
       device_no < MAX_ETHER_DEVICES &&
       device_table[device_no].entry_in_use) {
	(*device_table[device_no].stop_func)(device_no);
    } else {
	printf("Error: Unknown device %d\n", device_no);
    }
}

#ifdef NOT_IN_USE
/* ether_device_next - get the next valid device number
 *
 * Argument:
 *
 *    previous_device_number - previous device number; or -1 if no previous device.
 *
 * Result:
 *
 *    next valid device number or -1 if no more devices.
 *
 * Description:
 *    The purpose of this function it to allow the device table to be scanned.
 *    If it called initially with -1 and then repeatedly called with its previous
 *    return value as its argument it will return each valid device number 
 *    precisely once before returning -1.
 *
 * Notes:
 *    1) The device numbers will not neccesary be returned in assending order
 *    2) If previous_device_number is not the number of a valid device (or -1) the 
 *        result is undefined.
 */
int ether_device_number(int previous_device_number)
{
    /* Loop doing nothing until we find the next entry marked as in use */
    while(++previous_device_number < MAX_ETHER_DEVICES &&
	  ! device_table[previous_device_number].entry_in_use) ;

    /* Return either the valid device number we have found; or -1 */
    return (previous_device_number == MAX_ETHER_DEVICES) ? -1 : previous_device_number;
}
#endif

/* ether_device_register - register a new ethernet device
 *
 * Arguments:
 *     device_no              - device_number
 *     init_func              - initialisation function
 *     read_func              - read function
 *     write_func             - write function
 *     flush_func             - flush function
 *     stats_func             - print statistics function
 *     preg_func              - print device registers function
 *     clear_interrupts_func  - clear interrupts function
 *     hw_addr_func           - get hardware address function
 *     description            - description of device.
 */
void ether_device_register(int device_no,
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
			   void (* hw_addr_func)(int device_no, mac_addr hw_address),
			   void (* stop_func)(int device_no),
			   char *description)
{
    if(device_no >= 0 &&
       device_no < MAX_ETHER_DEVICES) {
	if(!device_table[device_no].entry_in_use)
	  {
	      device_table[device_no].entry_in_use = TRUE;
	      device_table[device_no].init_func = init_func;
	      device_table[device_no].read_func = read_func;
	      device_table[device_no].write_func = write_func;
	      device_table[device_no].flush_func = flush_func;
	      device_table[device_no].stats_func = stats_func;
	      device_table[device_no].preg_func = preg_func;
	      device_table[device_no].clear_interrupts_func = clear_interrupts_func;
	      device_table[device_no].hw_addr_func = hw_addr_func;
	      device_table[device_no].stop_func = stop_func;
	      device_table[device_no].description = description;
	  }
	else {
	    printf("Error: attempt to register device %d twice", device_no);
	}
    }
    else {
	printf("Error: attempt to register device with illegal device number %d", device_no);
    }
}
/* ether_device_init_module - initialise or reinitialise the ether_device module
 */
void ether_device_init_module(void)
{
    int i;
    for(i=0; i<MAX_ETHER_DEVICES; i++) {
	device_table[i].entry_in_use = FALSE;
    }
}

/*
 * ether_device_show - show all of the ethernet devices that have been
 * registered.
 */
void ether_device_show(void)
{
  extern int monitor_ether_device;
  int i;

  printf("All registered Ethernet devices:\n\n");
  printf("\tNet\tType\n");
  printf("\tDevice\n");

  for (i = 0; i < MAX_ETHER_DEVICES; i++)
    if (device_table[i].entry_in_use)
      printf("\t%d%s\t%s\n", i, 
	     monitor_ether_device == i ? "*" : "", 
	     device_table[i].description);
}
