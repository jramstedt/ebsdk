#ifndef __ROMHEAD_H_LOADED
#define __ROMHEAD_H_LOADED
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
/*---------------------------------------------------------------------
 *        [ Copyright (c) 1999 Alpha Processor Inc.] - Unpublished Work
 *          All rights reserved
 * 
 *    This file contains source code written by Alpha Processor, Inc.
 *    It may not be used without express written permission. The
 *    expression of the information contained herein is protected under
 *    federal copyright laws as an unpublished work and all copying
 *    without permission is prohibited and may be subject to criminal
 *    and civil penalties. Alpha Processor, Inc.  assumes no
 *    responsibility for errors, omissions, or damages caused by the use
 *    of these programs or from use of the information contained herein.
 *  
 *-------------------------------------------------------------------*/

/*
 *
 *	Special ROM header
 *	==================
 *	The System ROM can contain multiple ROM images, each with
 *	its own header. That header tells the SROM where to load
 *	the image and also if it has been compressed with the
 *	"makerom" tool.  For System ROMs which contain a single
 *	image, the header is optional.  If the header does not
 *	exist the complete System ROM is loaded and executed at
 *	physical address zero.
 *                                                         +-- Offset
 *                                                         |         Header
 *	31                                             0   |     +-- Revisions
 *	+-----------------------------------------------+  |     |   Supported
 *	|   VALIDATION PATTERN 0x5A5AC3C3               | 0x00  all
 *	+-----------------------------------------------+
 *	|   INVERSE VALIDATION PATTERN 0xA5A53C3C       | 0x04  all
 *	+-----------------------------------------------+
 *	|   HEADER SIZE (Bytes)                         | 0x08  all
 *	+-----------------------------------------------+
 *	|   IMAGE CHECKSUM                              | 0x0C  all
 *	+-----------------------------------------------+
 *	|   IMAGE SIZE (Memory Footprint)               | 0x10  all
 *	+-----------------------------------------------+
 *	|   DECOMPRESSION FLAG                          | 0x14  all
 *	+-----------------------------------------------+
 *	|   DESTINATION ADDRESS LOWER LONGWORD          | 0x18  all
 *	+-----------------------------------------------+
 *	|   DESTINATION ADDRESS UPPER LONGWORD          | 0x1C  all
 *	+-----------------------------------------------+
 *     	|   FIRMWARE ID <15:8> | HEADER REV <7:0>       | 0x20  1+
 *	|   Reserved <31:24>   | HEADER REV EXT <23:16> |
 *	+-----------------------------------------------+
 *	|   ROM IMAGE SIZE                              | 0x24  1+
 *	+-----------------------------------------------+
 *	|   OPTIONAL FIRMWARE ID <31:0>                 | 0x28  1+
 *	+-----------------------------------------------+
 *	|   OPTIONAL FIRMWARE ID <63:32>                | 0x2C  1+
 *	+-----------------------------------------------+
 *	|   ROM OFFSET<31:2>     | ROM OFFSET VALID<0>  | 0x30  2+
 *	+-----------------------------------------------+
 *	|   HEADER CHECKSUM (excluding this field)      | 0x34  1+
 *	+-----------------------------------------------+
 *
 *	VALIDATION PATTERN
 *	------------------
 *	The first quadword contains a special signature pattern
 *	that is used to verify that this "special" ROM header
 *	has been located.  The pattern is 0x5A5AC3C3A5A53C3C.
 *
 *	HEADER SIZE (Bytes)
 *	-------------------
 *	The header size is the next longword.  This is provided
 *	to allow for some backward compatibility in the event that
 *	the header is extended in the future.  When the header
 *	is located, current versions of SROM code determine where
 *	the image begins based on the header size.  Additional data
 *	added to the header in the future will simply be ignored
 *	by current SROM code. Additionally, the header size = 0x20
 *	implies version 0 of this header spec.  For any other size
 *      see HEADER REVISION to determine header version.
 *      
 *      
 *	IMAGE CHECKSUM
 *	--------------
 *	The next longword contains the image checksum.  This is
 *	used to verify the integrity of the ROM.  Checksum is computed
 *      in the same fashion as the header checksum.
 *      Although this field was provided with version 0 of this header
 *      spec, the checksum was not really computed until version 1.
 *
 *	IMAGE SIZE (Memory Footprint)
 *	-----------------------------
 *	The image size reflects the size of the image after it has
 *	been loaded into memory from the ROM. See ROM IMAGE SIZE.
 *
 *	DECOMPRESSION FLAG
 *	------------------
 *	The decompression flag tells the SROM code if the makerom
 *	tool was used to compress the ROM image with a "trivial
 *	repeating byte algorithm".  The SROM code contains routines
 *	which perform this decompression algorithm.  Other
 *	compression/decompression schemes may be employed which work
 *	independently from this one.
 *
 *	DESTINATION ADDRESS
 *	-------------------
 *	This quadword contains the destination address for the
 *	image.  The SROM code  will begin loading the image at this
 *	address and subsequently begin its execution there.
 *
 *	HEADER REV
 *	----------
 *	The revision of the header specifications used in this
 *	header.  This is necessary to provide compatibility to
 *	future changes to this header spec.  Version 0 headers
 *	are identified by the size of the header. See HEADER SIZE.
 *	For Version 1 or greater headers this field must be set to
 *	a value of 1.  The header revision for version 1 or greater
 *	headers is determined by the sum of this field and the
 *	HEADER REV EXT field. See HEADER REV EXT.  
 *
 *	FIRMWARE ID
 *	-----------
 *	The firmware ID is a byte that specifies the firmware type.
 *	This facilitates image boot options necessary to boot
 *	different operating systems.
 *
 *		  firmware
 *	firmware    type
 *	--------  --------
 *	  DBM	     0	     Alpha Evaluation Boards Debug Monitor
 *	  WNT        1       Windows NT Firmware
 *	  SRM        2       Alpha System Reference Manual Console
 *	  FSB	     6	     Alpha Evaluation Boards Fail-Safe Booter
 *	  Milo       7       Linux Miniloader
 *	  SROM      10       Serial ROM (SROM) Image 
 *
 *	HEADER REV EXT
 *	--------------
 *	The header revision for version 1 or greater headers is
 *	determined by the sum of this field and the HEADER REV
 *	field. See HEADER REV.  
 *
 *	ROM IMAGE SIZE
 *	--------------
 *	The ROM image size reflects the size of the image as it is
 *	contained in the ROM. See IMAGE SIZE.
 *
 *	OPTIONAL FW ID
 *	--------------
 *	This is an optional field that can be used to provide
 *	additional firmware information such as firmware revision
 *	or a character descriptive string up to 8 characters.
 *
 *	ROM OFFSET
 *	----------
 *	This field specifies the default ROM offset to be used
 *	when programming the Image into the ROM.
 *
 *	ROM OFFSET VALID
 *	----------------
 *	The lower bit of the ROM OFFSET field should be set when
 *	the ROM OFFSET field is specified.  When no ROM OFFSET is
 *	specified the ROM OFFSET and VALID fields will contain zero.
 *
 *	HEADER CHECKSUM
 *	---------------
 *	The checksum of the header.  This is used to validate
 *	the presence of a header beyond the validation provided
 *	by the validation pattern.  See VALIDATION PATTERN.
 *	The header checksum is computed from the beginning of
 *	the header up to but excluding the header checksum
 *	field itself.  If there are future versions of this
 *	header the header checksum should always be the last
 *	field defined in the header.  The checksum algorithm used
 *	is compatible with the standard BSD4.3 algorithm provided
 *	on most implementations of Unix.  Algorithm: The checksum
 *	is rotated right by one bit around a 16 bit field before
 *	adding in the value of each byte.
 *
 */

#define ROM_H_SIGNATURE 0x5A5AC3C3U
#define ROM_H_REVISION  2
#define DEFAULT_ROM_H_REVISION 2
#define ROM_H_BRANCH_SIGNATURE 0xc3e00000
#define ROM_H_BRANCH_OFFSET_MASK 0x1fffff

/* Structure used by Compaq Firmware */
typedef struct {
    unsigned char build_minute;
    unsigned char build_hour;
    unsigned char build_day;
    unsigned char build_month;
    unsigned char build_year;
    unsigned char revision;
    unsigned char minor_version;
    unsigned char major_version;
} cpq_fwid_t;

/* Structure used by API Firmware */
typedef struct {
    unsigned char major;	/* Firmware major version */
    unsigned char minor;	/* Firmware minor version */
    unsigned char rev;		/* Firmware miniscule version */
    unsigned char api_category;	/* API part number category */
    unsigned char api_major;	/* API part number major byte */
    unsigned char api_minor;	/* API part number minor byte */
    unsigned char api_rev;	/* API part revision number (0-9) */
    unsigned char api_letter;	/* letter (A-Z starting from 0=A) */
} api_fwid_t;

typedef union {
    struct {
	/*
	 * Version 0 definition of the ROM header.
	 */
	struct {
	    unsigned signature;		/* validation signature */
	    unsigned csignature;	/* inverse validation signature */
	    unsigned hsize;		/* header size */
	    unsigned checksum;		/* checksum */
	    unsigned size;		/* image size (Memory Footprint) */
	    unsigned decomp;		/* decompression algorithm */
	    struct {
		unsigned low;
		unsigned high;
	    } destination;		/* destination address */
	} V0;

	/*
	 * Version 1 extensions to the ROM header.
	 */
	struct {
	    char hversion;		/* ROM header version   (Byte 0) */
	    char fw_id;			/* Firmware ID          (Byte 1) */
	    char hversion_ext;		/* header version ext   (Byte 2) */
	    char reserved;		/* Reserved             (Byte 3) */
	    unsigned rimage_size;	/* ROM image size */

	    /* The firmware optional ID is a bit of a mess due to a mishmash
	     * of conflicting formats with no clear indication which one a
	     * particular firmware header is following.
	     * The various options are presented here.
	     */
	    union {

		/* Optional Firmware ID (character array)   */
		char id[8];

		/* Structure used by Compaq Firmware */
		cpq_fwid_t fw_id_S;

		/* Structure used by API Firmware */
		api_fwid_t api_id_S;

	    } fwoptid;
	} V1;

	/*
	 * Version 2 extensions to the ROM header.
	 */
	struct {
	    unsigned rom_offset;    /* ROM Offset<31:2>, ROM Offset Valid<0> */
	} V2;

	/*
	 * Future extensions to the header should be included before
	 * this header checksum.  (See HEADER CHECKSUM description)
	 */
	unsigned hchecksum;	/* Header checksum, (Always last entry)     */
    } romh;
    unsigned romh_array[1];	/* To allow longword access to the data     */
} romheader_t;

/*
 * Registered Firmware types.
 */
typedef enum _fwids {	FW_DBM=0,		/* Debug Monitor */
			FW_WNT=1,		/* AlphaBIOS / ARC Firmware */
			FW_SRM=2,		/* SRM Console */
			FW_FSB=6,		/* Fail-safe boot firmware */
			FW_MILO=7,		/* Milo (Linux Miniloader) */
			FW_VXW=8,		/* VxWorks */
			FW_DIAG=9,		/* Diagnostics */
			FW_SR=10,		/* SROM Code */
			FW_LINUX=11,		/* Linux kernel */
			FW_CBOX=12,		/* CBOX config table */
			FW_OSFPAL=13,		/* OSF PALcode */
			FW_NVRAM=15,		/* NVRAM data area */
			FW_NVLOG=16,		/* NVRAM diagnostic log */
			FW_NVENV=17,		/* NVRAM diagnostic env vars */
			FW_INITRD=20,		/* Linux InitRD image */
			FW_UNKNOWN=-1,
			FW_EMPTY=-2		/* Empty region of ROM */
} FWID;


/*
 * The ROM header checksum should always be assigned to the last
 * field in the header.  Therefore, when reading headers created
 * by various versions of makerom the ROM header checksum can be
 * in different locations.  This macro can be used to access the
 * ROM header checksum in the proper location.
 */
#define ROM_HEADER_CHECKSUM(x) \
  ((x)->romh_array[((x)->romh.V0.hsize - sizeof((x)->romh.hchecksum))/sizeof(unsigned)])

/*
 * Macro to provide the header version number
 */
#define ROMH_VERSION(x) ((x)->romh.V0.hsize == 0x20 ? 0 : \
			 ((x)->romh.V1.hversion+(x)->romh.V1.hversion_ext))

/*
 * Macro to assist in computing the BSD4.3 style checksum.
 */
#define ROTATE_RIGHT(x) if ((x) & 1) (x) = ((x) >>1) + 0x8000; else (x) = (x) >>1;

/*
 * Macro used to increment the checksum
 * by a new byte while keeping the total
 * checksum within the 16 bit range.
 */
#define COMPUTE_CHECKSUM(c,k) \
   {ROTATE_RIGHT(k); k += (ub) c; k &= 0xffff;}

#endif /* __ROMHEAD_H_LOADED */
