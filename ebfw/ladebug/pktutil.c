/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
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
static char *rcsid = "$Id: pktutil.c,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $";
#endif

/*
 * $Log: pktutil.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.10  1995/10/31  18:48:38  cruz
 * Commented out some unused routines.
 *
 * Revision 1.9  1995/10/23  20:50:58  cruz
 * Moved packet dumping code to pkthandl.c
 *
 * Revision 1.8  1995/09/26  22:34:22  cruz
 * Changed the PRTRACE statements in DumpPackets to printf
 * statements so that we don't have to define PRTRACE here
 * when debugging pkthandle.c
 * Added routine to convert the reply number to string.
 *
 * Revision 1.7  1994/11/07  12:44:43  rusling
 * Now include both system.h and lib.h.
 *
 * Revision 1.6  1994/08/05  20:17:45  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.5  1994/06/28  20:12:18  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.4  1994/06/23  10:42:33  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.3  1994/06/17  19:36:43  fdh
 * Clean-up...
 *
 * Revision 1.2  1994/04/07  14:54:31  fdh
 * Added RCS identifiers
 *
 * Revision 1.1  1994/03/09 12:48:33 berent
 * Initial revision
 */


/* This file contains packet handling utilities for reading and writing fields of packets. 
 */

/*
 * History:
 *  24-Nov-1993 A.N. Berent   Routines extracted from pkthandl.c
 */

#include "ladebug.h"

 
uw ReadNetShort(ub * dataP)
{
  return (dataP[0] << 8) + dataP[1];
}
ui ReadNetInt(ub * dataP)
{
  return (dataP[0] << 24) + (dataP[1] << 16) + (dataP[2] << 8) + dataP[3];
}

#ifdef NOT_IN_USE
ul ReadNetLong(ub * dataP)
{
  return (dataP[0] << 56) + (dataP[1] << 48) + (dataP[2] << 40) + (dataP[3] << 32) +
         (dataP[4] << 24) + (dataP[5] << 16) + (dataP[6] << 8) + dataP[7];
}
#endif

void WriteNetShort(ub * dataP, uw s)
{
  dataP[0] = (s >> 8) & 0xFF;
  dataP[1] = s & 0xFF;
}
void WriteNetInt(ub * dataP, ui s)
{
  dataP[0] = (ub)(s >> 24) & 0xFF;
  dataP[1] = (ub)(s >> 16) & 0xFF;
  dataP[2] = (ub)(s >> 8) & 0xFF;
  dataP[3] = (ub)s & 0xFF;
}

#ifdef NOT_IN_USE
void WriteNetLong(ub * dataP, ul s)
{
  dataP[0] = (ub)(s >> 56) & 0xFF;
  dataP[1] = (ub)(s >> 48) & 0xFF;
  dataP[2] = (ub)(s >> 40) & 0xFF;
  dataP[3] = (ub)(s >> 32) & 0xFF;
  dataP[4] = (ub)(s >> 24) & 0xFF;
  dataP[5] = (ub)(s >> 16) & 0xFF;
  dataP[6] = (ub)(s >> 8) & 0xFF;
  dataP[7] = (ub)s & 0xFF;
}


/* These routines are needed; rather than simple casts; to avoid allignment traps */

uw ReadLocalShort(ub * dataP)
{
  return (dataP[1] << 8) + dataP[0];
}
#endif

ui ReadLocalInt(ub * dataP)
{
  return (dataP[3] << 24) + (dataP[2] << 16) + (dataP[1] << 8) + dataP[0];
}
ul ReadLocalLong(ub * dataP)
{
  return (((unsigned long) dataP[7]) << 56) + 
         (((unsigned long)dataP[6]) << 48) + 
	 (((unsigned long)dataP[5]) << 40) + 
	 (((unsigned long)dataP[4]) << 32) +
         (((unsigned long)dataP[3]) << 24) + 
	 (((unsigned long)dataP[2]) << 16) + 
	 (((unsigned long)dataP[1]) << 8) + 
	 ((unsigned long) dataP[0]);
}

#ifdef NOT_IN_USE
void WriteLocalShort(ub *  dataP, uw s)
{
  dataP[1] = (ub)(s >> 8) & 0xFF;
  dataP[0] = (ub)s & 0xFF;
}
#endif
void WriteLocalInt(ub * dataP, ui s)
{
  dataP[3] = (ub)(s >> 24) & 0xFF;
  dataP[2] = (ub)(s >> 16) & 0xFF;
  dataP[1] = (ub)(s >> 8) & 0xFF;
  dataP[0] = (ub)s & 0xFF;
}
void WriteLocalLong(ub * dataP, ul s)
{
  dataP[7] = (ub)(s >> 56) & 0xFF;
  dataP[6] = (ub)(s >> 48) & 0xFF;
  dataP[5] = (ub)(s >> 40) & 0xFF;
  dataP[4] = (ub)(s >> 32) & 0xFF;
  dataP[3] = (ub)(s >> 24) & 0xFF;
  dataP[2] = (ub)(s >> 16) & 0xFF;
  dataP[1] = (ub)(s >> 8) & 0xFF;
  dataP[0] = (ub)s & 0xFF;
}

