#ifndef __EDRIVER_H_LOADED
#define __EDRIVER_H_LOADED
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
 * $Id: edriver.h,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $
 */

/*
 * MODULE DESCRIPTION:
 * 
 *     Parameters used to service Ethernet device drivers on the EB64
 *
 * HISTORY:
 * 
 * $Log: edriver.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1995/10/20  18:43:01  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.3  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.2  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.1  1993/08/06  10:12:44  berent
 * Initial revision
 *
 *
 */

/* Ethernet and IEEE 802.3 packets have the following format for
 * the MAC (Media Access Control) layer
 *
 *     Preamble (supplied by physical device) 7 octets (0x55) for carrier
 *     recover plus one octet (0xD5) Start_of_Frame_Delimiter
 *
 *     6 octet destination address
 *
 *     6 octet source address
 *
 *     2 octet Protocol_type field (Ethernet) or Length field (802.3)
 *             Valid protocol types are all illegal lengths ie. >1500
 *
 *     48-1500 octet data field including padding if necessary
 *
 *     4 octet frame check sequence (FCS)
 *
 * All octets are sent LSB first "on the wire"
 *
 * If the first bit of the destination address sent is "1", it is known
 * as a multicast address, or group address. The "all 1's" is a special
 * case known as the broadcast address
 *
 * For information - the 2nd bit is a "local bit" used by protocols which
 * define local MAC address spaces eg. DECnet phase IV.
 */

#define MAX_ENET_PACKET 1514     /* 6+6+2+1500; does not include the FCS */
#define MIN_ENET_PACKET 60       /* 6+6+2+46; does not include the FCS */

 
         /* Define the MAC header structure for Ethernet */

typedef struct{
  mac_addr destination;
  mac_addr source;
  ethernet_protocol_id protocol;
} ehternet_header_t;

#define ETHERNET_HEADER_SIZE	14
#endif /* __EDRIVER_H_LOADED */
