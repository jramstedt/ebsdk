/*
 * Copyright (C) 1997 by
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	USB Port Block (PB) Definitions
 *
 * Author:	Kevin LeMieux
 *
 * Modifications:
 */

volatile struct usb_pb {
    struct pb pb;
    unsigned int *usbp;
    struct SEMAPHORE owner_s;
    struct TIMERQ ctrl_t;
    unsigned int *bus_sw;
    int cb_pid;
    struct SEMAPHORE callback_complete_s;
    int usb_configure;
    int hub_configure;
    int reconfigure;
    struct QUEUE rq;		   /* queue for any resources that must be */
				   /* freed on shutdown */
    };
