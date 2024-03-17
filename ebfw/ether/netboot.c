
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
static char *rcsid = "$Id: netboot.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif

/*
 * $Log: netboot.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.13  1997/12/15  20:49:47  pbell
 * Updated for dp264.
 *
 * Revision 1.12  1995/12/12  21:16:40  cruz
 * Abort netload if it detects no network device.
 *
 * Revision 1.11  1995/11/28  16:16:17  cruz
 * Updated load message to include the size of the file loaded.
 *
 * Revision 1.10  1995/10/20  18:45:41  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.9  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.8  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.7  1994/06/22  15:11:46  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.6  1994/06/17  19:36:25  fdh
 * Clean-up...
 *
 * Revision 1.5  1994/01/19  10:55:52  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.4  1993/11/02  22:00:26  fdh
 * Fixup netboot messages.
 *
 * Revision 1.3  1993/08/10  10:43:55  berent
 * Use a global instead of a constant for monitor ethernet device to allow it to change
 *
 * Revision 1.2  1993/08/06  10:55:32  berent
 * changed MONITOR_ETHER_PORT to MONITOR_ETHER_DEVICE for readability.
 *
 * Revision 1.1  1993/08/06  10:02:49  berent
 * Initial revision
 *
 */

#include "ether.h"
#include "mon.h"     /* Needed for bootadr */
#include "interlock.h"

/* global variables */
static  volatile int lock=0,return_value=0;

int netboot(int argc, char *file)
{
    char filename[128];
    ip_addr server_addr;
    ip_addr my_addr;
    int bootp_result;
    int file_size;
    int i;

  if (atomic_inc(1,&lock)==0)
  {
    return_value= 0;
    if (! netman_monitor_device_started())
      if (!netman_start_monitor_device())
    {
       return_value= -1;
       lock= 0;
       return -1;
    }
    if(argc > 1 ) {
	bootp_result = bootp_boot(monitor_ether_device,
				  file,
				  server_addr,
				  filename);
    }
    else {
	bootp_result = bootp_boot(monitor_ether_device,
				  "",
				  server_addr,
				  filename);
    }
    if(! bootp_result) {
	printf("\nBootP failed\n");
        return_value= -1;
        lock =0;
	return -1;
    }

    printf("\nLoading %s at 0x%X ", filename, bootadr); 

    ip_get_device_address(monitor_ether_device,my_addr);
    printf("\n My IP address:     ");
    ip_printaddress(my_addr);
    
    printf("\n Server IP address: ");
    ip_printaddress(server_addr);
    printf("\n");

    if((file_size = tftp_load(monitor_ether_device,
		 filename,
		 server_addr,
		 (unsigned char *) bootadr))) {
	printf("\nFile loaded successfully.  Size = 0x%X (%d)\n\n", file_size, file_size);
        lock =0;
	return 0;
    }
    else {
	printf("TFTP failed\n");
        return_value= -1;
        lock =0;
	return -1;
    }
    lock =0;
  }
  else
  {
    while(lock) i++;
    return return_value;
  }
}


