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
 * Abstract:	Non-Volatile RAM Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	jrh	01-Mar-1994	Add Alcor support
 *
 *	jmp	11-29-93	Add avanti support
 *
 *	pel	25-Aug-1993	replace ESC_EEROM_BASE with NVRAM_BASE.
 *				Add NVRAM_PAGE_REG; replace ESC_EEROM_LENGTH 
 *				w/ .bld value EEROM_LENGTH.
 *
 *	jeg	06-Aug-1993	add ARC config tree/ECU NVRAM definition
 *
 *	cbf     29-Jan-1993     bug fix - mark nvram areas 43,44+45 unused
 *                              create new variable password (46) 44 was
 *                              password = string, 46 is password = integer
 *
 *	cjd	08-Jun-1992	Add inet variables
 *
 *	sfs	04-Mar-1992	Initial entry.
 */

#ifndef NVRAM_DEF
#define NVRAM_DEF 1

#if !MIKASA
#include "cp$inc:platform_io.h"
#endif

#if RAWHIDE || PINNACLE
#define ARC_NVRAM_V1 1
#else
#define ARC_NVRAM_V1 0
#endif

#if ARC_NVRAM_V1
#define ARC_NVRAM_SIGNATURE 0x865D8546
#define ARC_NVRAM_VERSION 1
#define ARC_NVRAM_CHECKSUM_SIZE 3 * 1024 - 4
#define ARC_NVRAM_OFFSET 0xC00
#else
#ifndef ARC_NVRAM_OFFSET
#define ARC_NVRAM_OFFSET (0)
#endif
#ifndef ARC_NVRAM_LENGTH
#define ARC_NVRAM_LENGTH (0)
#endif
#ifndef ISACFG_NVRAM_OFFSET
#define ISACFG_NVRAM_OFFSET (0)
#endif
#ifndef ISACFG_NVRAM_LENGTH
#define ISACFG_NVRAM_LENGTH (0)
#endif
#endif

#define NVRAM_VERSION 6
#define OLD_NVRAM_VERSION 5
#define OLD_EV_VALUE_LEN 128

/* for NVRAM accessed via the ESC and SIO chips */

#define NVRAM_BASE	0x800  	/* base address */
#define NVRAM_PAGE_REG  0xC00  	/* base address */

struct srm_nvram {
    unsigned short int checksum;
    unsigned short int version;
    unsigned char text[NVRAM_EV_LEN-4];
    } ;

#if SABLE || ALCOR || MIKASA || TURBO || RAWHIDE

/* Defines for configuration tables.*/
#if ARC_NVRAM_V1
#define NUMBER_OF_ENTRIES 	33
#define LENGTH_OF_IDENTIFIER 	496
#define LENGTH_OF_DATA		2048
#define LENGTH_OF_EISA_DATA	2032
#else
#if SABLE || ALCOR || TURBO || MIKASA
#define NUMBER_OF_ENTRIES 	40
#define LENGTH_OF_IDENTIFIER 	( 1024 - (40*16) - 8 )
#define LENGTH_OF_DATA		2048
#define LENGTH_OF_ENVIRONMENT 	1024
#define LENGTH_OF_EISA_DATA	2044
#endif
#endif

#define MAXIMUM_ENVIRONMENT_VALUE 256
#define MAX_NUMBER_OF_ENVIRONMENT_VARIABLES 28

#define NO_IDENTIFIER 0xffff
#define Flags$m_Eisa 128

/* per slot information on EISA modules */
typedef struct _EISA_SLOT {
    unsigned char slot_id[8];
    unsigned char *p_cfg;
    } EISA_SLOT, *PEISA_SLOT;

/* ARC configuration tree structs */

/* The compressed configuration packet structure used to store configuration*/
/* data in NVRAM.*/

typedef struct _COMPRESSED_CONFIGURATION_PACKET {
    unsigned char Parent;
    unsigned char Class;
    unsigned char Type;
    unsigned char Flags;
    unsigned long Key;
    unsigned char Version;
    unsigned char Revision;
    unsigned short ConfigurationDataLength;
    unsigned short Identifier;
    unsigned short ConfigurationData;    /* regular or eisa configuration */
    } COMPRESSED_CONFIGURATION_PACKET, *PCOMPRESSED_CONFIGURATION_PACKET;


#if !ARC_NVRAM_V1
struct arc_nvram {
    COMPRESSED_CONFIGURATION_PACKET Packet[NUMBER_OF_ENTRIES];
    unsigned char Identifier[LENGTH_OF_IDENTIFIER];
    unsigned char Data[LENGTH_OF_DATA];
    unsigned char Checksum1[4];
    unsigned char Environment[LENGTH_OF_ENVIRONMENT];
    unsigned char Checksum2[4];
    unsigned char EisaData[LENGTH_OF_EISA_DATA];
    unsigned char Checksum3[4];
    } ;
#else
struct arc_nvram {
    unsigned int  Signature;
    unsigned char Version;
    unsigned char Packet_Array_Size;
    unsigned char Identifer_Array_Size;
    unsigned char Eisa_Data_Size;
    COMPRESSED_CONFIGURATION_PACKET Packet[NUMBER_OF_ENTRIES];
    unsigned char Identifier[LENGTH_OF_IDENTIFIER];
    unsigned char EisaData[LENGTH_OF_EISA_DATA];
    unsigned long Spare;
    unsigned char Checksum[4];
    } ;
#endif
#endif

#endif /* NVRAM_DEF */
