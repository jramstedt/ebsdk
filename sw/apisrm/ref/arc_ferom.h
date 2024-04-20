/*
 * Copyright (C) 1993 by
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
 * Abstract:	structures in ARC NVRAM, from fwp.h in ARC NT sources
 *             
 * Author:	David M. Robinson (davidro) 29-Aug-1991  (Microsoft)
 *
 * Modifications:
 *
 *   		jrd    15-April-1992	Modified for Alpha/Jensen hooks.  
 *
 *		jeg	01-Jul-1993	initial port to common console.
 *  Copyright (c) 1991  Microsoft Corporation
 */






/**/
/* The non-volatile configuration table structure.*/
/**/

typedef struct _NV_CONFIGURATION {

    /**/
    /* First Page*/
    /**/

    COMPRESSED_CONFIGURATION_PACKET Packet[NUMBER_OF_ENTRIES];
    UCHAR Identifier[LENGTH_OF_IDENTIFIER];
    UCHAR Data[LENGTH_OF_DATA];
    UCHAR Checksum1[4];
    UCHAR Environment[LENGTH_OF_ENVIRONMENT];
    UCHAR Checksum2[4];

    /**/
    /* Second Page*/
    /**/

    UCHAR EisaData[LENGTH_OF_EISA_DATA];
    UCHAR Checksum3[4];

} NV_CONFIGURATION, *PNV_CONFIGURATION;


/**/
/* Component Data Structure, environment variables.*/
/* These contain their own checksums.*/
/**/
#define NVRAM_CONFIGURATION     0xC0000
#define NVRAM_EISADATA		0xC1000
