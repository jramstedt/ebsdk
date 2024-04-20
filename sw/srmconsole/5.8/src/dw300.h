/*
 * Copyright (C) 1992 by
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
 * Abstract:    DW300 port driver specific definitions
 *
 * Author:  Lowell Kopp
 *
 * Modifications:
 *
 *  lek 14-Oct-1994 Initial entry.
 */


typedef unsigned char UBYTE;
typedef unsigned short int USHORT;
typedef unsigned long ULONG;

typedef unsigned char SA[6];    /* station address */

#define MAX_DATA_SIZE   1500    /* Ethernet info field, 802.3 is 1492 */
/* ENET Data Link Frame Format */

#define ENET_HDR_SIZE  16
typedef struct _enet_frame {
    SA      dest_addr;
    SA      source_addr;
    UBYTE   protocol_type_0;
    UBYTE   protocol_type_1;
    UBYTE   len [2];
    UBYTE   data [MAX_DATA_SIZE];
    }  ETHERNET_FRAME;

/* IEEE 802.3 Data Link Frame Format */ 
#define IEEE_HDR_SIZE 14
#define IEEE_LLC_SIZE 8  /* dsap .. pid-4 */
typedef struct _ieee_frame {
    SA    dest_addr;    /*Destination */
    SA    source_addr;  /*Source */
    UBYTE len [2];      /*length */
    UBYTE dsap;     /* Destination Service Access Point */
    UBYTE ssap;     /* Source Service Access Point */
    UBYTE cntr;     /* Unnumbered Information */
    UBYTE pid_0;    /* IEEE Protocol Prefix [0] */
    UBYTE pid_1;    /* IEEE Protocol Prefix [1] */
    UBYTE pid_2;    /* IEEE Protocol Prefix [2] */
    UBYTE pid_3;    /* IEEE Protocol  [3] */
    UBYTE pid_4;    /* IEEE Protocol  [4] */
    UBYTE data [MAX_DATA_SIZE-8];   /* Transmit data */
    } IEEE_FRAME;


#define MAX_ENET_LEN  1502      /* 1500 + 2, or 1492 + 10 */
#define ROUTING_INFO_SIZE   18  /* max sixe of source routing info  */
/* Token Ring frame header */

#define TR_HDR_SIZE  14
typedef struct _tr_header {
    UBYTE   ac;                 /* access control                    (0x10) */
    UBYTE   fc;                 /* frame control                     (0x40) */
    SA      dest_addr;          /* destination address */
    SA      source_addr;        /* source address */
    UBYTE   ri [ROUTING_INFO_SIZE];   /* Routing info */
    } TOKEN_RING_HEADER;

/* use this struct as a template to access LLC fields, after variable length routing
 * info field */
typedef struct _llc_header {
/* These are really SNAP SAP fields but they must be provided on every
 * packet we send/receive */
    UBYTE   dsap;               /* destination service access point  (0xaa) */
    UBYTE   ssap;               /* source service access point       (0xaa) */
    UBYTE   ui;                 /* unnumbered information type pkt   (0x03) */
    UBYTE   oui [3];            /* Organizationally unique identifier (0x00,00,00 ) */
    UBYTE   pid [2];            /* Protocol Identifier                      */
    UBYTE   data [MAX_DATA_SIZE];
    } IEEE_LLC_HEADER;


#define ENET     1
#define IEEE     2

/*
 *  Note the correlation of multicast addresses to Token Ring
 *   Functional Addresses:
 *
 *  Multicast           802.5 Functional    Protocol
 *  (canonical)         (reversed)          Type       Description
 *  -----------         ----------------    --------   -----------
 *  ab-00-00-01-00-00   c0:00:40:00:00:00   60-01      MOP dump/load assistance
 *  ab-00-00-02-00-00   c0:00:20:00:00:00   60-02      MOP remote console
 *  cf-00-00-00-00-00                       90-00      MOP loopback
 *  ff-ff-ff-ff-ff-ff                                  console multicast
 *
 *
 */

 /* If source routing enabled, set MSb of SA */

      /*
      * If RIF hasn't been determined, send out a request using
      * Single Route Broadcast, nonbroadcast return.  This makes
      * the frame appear only once in each ring.
      *
      * Reference:
      *
      * RI control field ==  ri[0] and ri[1]
      * ---------
      *   ri[0]
      *      b7:5 == Broadcast type
      *         000 ==> nonbroadcast (specific route)
      *         100 ==> all routes explore (ARE), nonbroadcast return
      *         110 ==> Single-route broadcast (SRE), broadcast return
      *         111 ==> Single-route broadcast (SRE), nonbroadcast return
      *      b4:0 == Current length of RIF (2-18)
      * ---------
      *   ri[1]
      *      b7   == Direction
      *           0 ==> Out, relative to client (initially 0)
      *           1 ==> In
      *      b6:4 == Longest frame (initally 111)
      *         000 ==> up to 516 bytes in information field
      *         001 ==> up to 1470 bytes
      *         010 ==> up to 2052 bytes
      *         011 ==> up to 4472 bytes
      *         100 ==> up to 8144 bytes
      *         101 ==> up to 11407 bytes
      *         110 ==> up to 17800 bytes
      *         111 ==> inital broadcast value
      *      b3:0 == reserved == 0000
      */


