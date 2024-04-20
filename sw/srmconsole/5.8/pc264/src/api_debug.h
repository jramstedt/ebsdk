/* file:    api_debug.h
 *
 * Copyright (C) 1990 by
 * Digital Equipment Corporation,Maynard,Massachusetts.
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
 *      API Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *  AUTHORS:
 *
 *  Chris Gearing
 *
 *  CREATION DATE:
 *  
 *  Q4 2000
 *
 *
 *  MODIFICATION HISTORY:
  
 *
 *--
 */

/*Include files*/

#define APIDB_SIMCOMDBG	0x01

#if SHARK
#define APIDB_IDEPIO	0x02
#define APIDB_UARTFIX	0x04

#define APIDB_SPCBITS	0xf0
#define APIDB_SPCVAL	0xa0
#define APIDB_DEFBITS	(APIDB_IDEPIO)
#endif

#if TINOSA
#define APIDB_ALLPCI	0x02

#define APIDB_SPCBITS	0xfc
#define APIDB_SPCVAL	0xa8
#define APIDB_DEFBITS	(APIDB_ALLPCI)

#endif

#if SWORDFISH
#define APIDB_ADPDIS	0x02			/* Adaptec disabled on UP2000 */

#define APIDB_SPCBITS	0xfc
#define APIDB_SPCVAL	0xa8

#define APIDB_DEFBITS	(0)

#endif

#if NAUTILUS
#define APIDB_SPCBITS	0xfe
#define APIDB_SPCVAL	0xaa

#define APIDB_DEFBITS	(0)
#endif

#if GLXYTRAIN
#define APIDB_SPCBITS	0xfe
#define APIDB_SPCVAL	0xaa

#define APIDB_DEFBITS	(0)
#endif

#ifndef APIDB_SPCBITS
#define APIDB_SPCBITS	0xfe
#define APIDB_SPCVAL	0xaa

#define APIDB_DEFBITS	(0)
#endif

