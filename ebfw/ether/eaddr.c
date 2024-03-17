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
static char *rcsid = "$Id: eaddr.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif

/*
 * $Log: eaddr.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.10  1997/06/30  14:10:19  pbell
 * Moved the location of objects in the toy nvram.
 *
 * Revision 1.9  1995/11/10  21:48:48  fdh
 * Uppercased Ethernet messages for Debbie.
 *
 * Revision 1.8  1995/10/20  18:41:31  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.7  1995/08/25  20:07:32  fdh
 * Fixed a comment line.
 *
 * Revision 1.6  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.5  1994/07/11  18:34:18  fdh
 * fixup incompatibility in get_eaddr() with BBRAM_READ macro.
 *
 * Revision 1.4  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.3  1994/06/22  15:11:46  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.2  1994/06/17  19:36:25  fdh
 * Clean-up...
 *
 * Revision 1.1  1994/03/16  00:15:37  fdh
 * Initial revision
 *
 */

#include "ether.h"

#ifdef NEED_ETHERNET_ADDRESS
#include "bbram.h"


mac_addr eaddr = {0,0,0,0,0,0};			/* the default Ethernet address */


void get_eaddr()
{
    ui i;
    ui val;

    for (i = 0; i < MAC_ADDRESS_SIZE; i++) {
	BBRAM_READ(BBRAM_MACADDR + i, val);
	eaddr[i] = (unsigned char) val;
      }
    BBRAM_READ(BBRAM_MACADDR + MAC_ADDRESS_SIZE, val);
    /* if the BBRAM is corupted reset the value to default 0 */
    if( val != BBRAM_MACADDR_COOKIE( eaddr ) )
        for (i = 0; i < MAC_ADDRESS_SIZE; i++) {
	    eaddr[i] = 0;
}


BOOLEAN ethernet_address(ui argc, ub *s) 
{
    mac_addr d;
    ui i, j;
    unsigned char c;

/*
 *  What we're expecting in hexarg[1] is a string looking like
 *  nn-nn-nn-nn-nn-nn of valid ROM ID.  If there is no string
 *  then simply display the current default Ethernet mac address.
 *  If the string contains all zeros then blow the default away.
 *
 *  So, first parse the string.
 */
    if (argc < 2) {
/*
 *  No argument, just tell the user what we've got.
 */
	printf("Default Ethernet address = ");
	ethernet_printaddress(eaddr);
	printf("\n");
    } else {
/*
 *  A MAC address of some sort has been supplied.
 */
      for (i = 0; i < MAC_ADDRESS_SIZE; i++) {
	d[i] = 0;
/*
 *  Parse each pair of numbers.
 */
	for (j = 0; j < 2; j++) {
/*
 *  Check for valid hex digits.
 */
	    if ((*s >= '0') && (*s <= '9'))
		c = *s - '0';
	    else {
		if ((*s >= 'a') && (*s <= 'f'))
		    c = *s - 'a' + 10;
		else {
		    if ((*s >= 'A') && (*s <= 'F'))
			c = *s - 'A' + 10;
		    else {
			printf("ERROR: invalid Ethernet MAC address\n");
			return FALSE;
		    }
		}
	    }
/*
 *  The digit is valid and c contains the translated
 *  (from ascii) hex number;
 */
	    d[i] |= c << ((1 - j) * 4);
	    s++;
	}
/*
 *  Just move s on to jump the hyphon.
 */
	s = s + 1;
      }			/* end of do parse */
/*
 *  We now have a valid MAC address in d[].  If it is
 *  all zero's then we scrub the value in BBRAM.
 */
      j = 0;
      for (i = 0; i < MAC_ADDRESS_SIZE; i++) {
      	j = j | d[i]; 
	eaddr[i] = d[i];
      }
      if (j == 0) {
	printf("Clearing the default Ethernet address\n");
      } else {
      	printf("Setting the default ethernet address to ");
      	ethernet_printaddress(d);
      	printf("\n");
      }

      /*
       *  Now write the address to the BBRAM
       */
      for (i = 0; i < MAC_ADDRESS_SIZE; i++) 
	BBRAM_WRITE(BBRAM_MACADDR + i, d[i]);
      BBRAM_WRITE(BBRAM_MACADDR + MAC_ADDRESS_SIZE, BBRAM_MACADDR_COOKIE( d ));
    }			/* end of else setting/clearing */

    return TRUE;
}
#endif /* NEED_ETHERNET_ADDRESS */
