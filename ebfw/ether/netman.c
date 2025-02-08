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
__attribute__((unused)) static const char *rcsid =
  "$Id: netman.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif

/*
 * This module provides functions for selecting which ethernet device is used and
 * for initialising it when neccessary.
 *
 * $Log: netman.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.25  1997/06/02  04:24:56  fdh
 * Moved monitor_ether_device to the external
 * initialized data section.  It is now initialized
 * before the un-itialized is zeroed to make debug
 * monitor restarts work better.
 *
 * Revision 1.24  1997/02/20  13:58:31  fdh
 * Modified to set monitor device only when there is at least one
 * device registered.
 *
 * Revision 1.23  1996/06/03 17:43:40  fdh
 * Modified to allow the DC21040 driver module to find and
 * register all DC21x4x devices.
 *
 * Revision 1.22  1996/05/22  21:44:32  fdh
 * Modified am79c960 conditionals.
 *
 * Revision 1.21  1995/12/15  21:22:47  cruz
 * Print a linefeed after stopping all network devices.
 *
 * Revision 1.20  1995/12/12  21:14:49  cruz
 * Changed netman_start_monitor_device() to stop initializing if
 * it detects no network device.
 *
 * Revision 1.19  1995/12/07  23:45:55  cruz
 * Added netman_stop_all_devices().
 *
 * Revision 1.18  1995/11/27  15:58:05  cruz
 * Removed explicit initialization of etherdump and etherprom variables.
 *
 * Revision 1.17  1995/10/27  15:13:32  cruz
 * Moved declaration of etherdump and etherprom from cmd.c to here.
 *
 * Revision 1.16  1995/10/20  18:46:08  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.15  1995/10/18  18:09:34  fdh
 * Added netman_stop_monitor_device().
 *
 * Revision 1.14  1994/11/07  12:32:17  rusling
 * Include system.h and lib.h in the correct orde.
 *
 * Revision 1.13  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.12  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.11  1994/06/17  19:36:25  fdh
 * Clean-up...
 *
 * Revision 1.10  1994/06/03  20:05:25  fdh
 * Replaced <stdio.h> with <stddef.h>.
 *
 * Revision 1.9  1994/03/24  15:18:01  rusling
 * Made the 21040 the first device installed.
 *
 * Revision 1.8  1994/01/19  10:55:52  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.7  1993/11/30  10:14:17  rusling
 * Modified to use net_buff.h and net_buffer_allocate().
 *
 * Revision 1.6  1993/11/26  16:45:02  rusling
 * Fixed up device number allocation when netman_setup() is
 * called more than once (via ebuff commands).
 *
 * Revision 1.5  1993/11/24  13:51:54  rusling
 * Fixed up netman_ether_device so that you can select
 * any *registered* device.
 *
 * Revision 1.4  1993/11/22  13:18:42  rusling
 * Added in support for 21040 PCI device.
 *
 * Revision 1.3  1993/11/19  17:00:11  fdh
 * Include system header file and add in conditionals...
 *
 * Revision 1.2  1993/10/01  16:03:01  berent
 * Added calls to new module initialisation functions
 *
 * Revision 1.1  1993/08/10  10:45:02  berent
 * Initial revision
 *
 */

#include "ether.h"
#ifdef NEEDPCI
#include "pci.h"
#endif

#ifdef NEEDDEBUGGER
extern void ladbx_server_init_module(void);
#endif

int monitor_ether_device = -1;

BOOLEAN etherdump;      /* Print ethernet traffic if TRUE */
BOOLEAN etherprom;      /* Enable promiscuous mode if TRUE */

static int registered_devices;  /* This variable must be initialized to 0! */
static int device_initialised[MAX_ETHER_DEVICES];

/* netman_setup - initialise the networking modules and register the devices
 */
void netman_setup(void) 
{
    int i;

/*
 *  Bug fix:  this routine may be called more than once (via
 *  an ebuff change command.  If it is, then make sure that
 *  devices are registered with the same numbers each time.
 */
    registered_devices = 0;

    net_buffer_init_module();
    ether_device_init_module();

#ifdef NEEDAM79C960
    am79c960_device_init_module();
#endif
#ifdef NEEDWD3003
    wd3003_device_init_module();
#endif
#ifdef NEED21040
    DECchip_21040_device_init_module();
#endif
    arp_init_module();
    tftp_init_module();
    bootp_init_module();
#ifdef NEEDDEBUGGER
    ladbx_server_init_module();
#endif
    PRTRACE1("Modules initialised\n");

#ifdef NEED21040
/*
 *  For each of the 21040s, register a device, we don't
 *  care what slot number matches with what device.
 */
    registered_devices = registered_devices
      + DECchip_21040_device_register(registered_devices);
#endif

#ifdef NEEDAM79C960
    registered_devices = registered_devices
      + am79c960_device_register(registered_devices);
#endif

#ifdef NEEDWD3003
    wd3003_device_register(registered_devices++);
    PRTRACE1("Secondary wd3003 registered\n");
#endif

    for (i = 0; i < MAX_ETHER_DEVICES; i++) {
	device_initialised[i] = FALSE;
    }

    if (registered_devices > 0)
      netman_set_monitor_device(0);
}
/* netman_set_monitor_device - set up the device to be used by the monitor
 *
 * Argument:
 *    device_no - The monitor device number.
 * 
 * Return value:
 *    TRUE - successful
 *    FALSE - bad device number
 */
int netman_set_monitor_device(int device_no)
{
    if (device_no >= 0 && device_no < registered_devices) {
	monitor_ether_device = device_no;
	return TRUE;
    }
    return FALSE;
}
/* netman_start_monitor_device - start the monitor device and all protocol handlers for it
 */
int netman_start_monitor_device(void) 
{

    if (!ether_device_init(monitor_ether_device)) return FALSE;
    PRTRACE2("device started, device:%d\n", monitor_ether_device);
    arp_init(monitor_ether_device);
    PRTRACE2("ARP initialised, device:%d\n", monitor_ether_device);
    ip_init(monitor_ether_device);
    PRTRACE2("IP initialised, device:%d\n", monitor_ether_device);
    udp_init(monitor_ether_device);
    PRTRACE2("UDP initialised, device:%d\n", monitor_ether_device);

    device_initialised[monitor_ether_device] = TRUE;
    return TRUE;
}

/* netman_stop_monitor_device - stop the monitor device
 */
void netman_stop_monitor_device(void) 
{
    ether_device_stop(monitor_ether_device);
    PRTRACE1("device stopped\n");
    device_initialised[monitor_ether_device] = FALSE;
}

/* netman_stop_all_devices - stops all the ethernet devices
 */
void netman_stop_all_devices(void) 
{
int device_no;
    for (device_no = 0; device_no < registered_devices; device_no++) {
        ether_device_stop(device_no);
        device_initialised[device_no] = FALSE;
    }
    printf("\n");
}

/* netman_monitor_device_started  - tells other modules whether the monitor device has been started
 *
 * Return value:
 *    TRUE - it has been initialised before
 *    FALSE - it has never been initialised
 */
int netman_monitor_device_started(void) 
{
    return device_initialised[monitor_ether_device];
}
