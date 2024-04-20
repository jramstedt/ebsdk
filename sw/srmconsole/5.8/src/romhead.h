#ifndef __ROMHEAD_H_LOADED
#define __ROMHEAD_H_LOADED

/* file:	romhead.h
 *
 * Copyright (C) 1996 by
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 *
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha Firmware for Digital Semiconductor Motherboards
 *
 *  MODULE DESCRIPTION:     
 *
 *	Firmware ROM header definitions
 *
 *  AUTHOR:
 *
 *	Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *      1-May-1996
 *
 *  MODIFICATION HISTORY:
 *
 *	er	1-May-1996	Initial Revision
 *
 *
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
 *-- 
 */

#define ROM_H_SIGNATURE 0x5A5AC3C3
#define ROM_H_REVISION  2


typedef union {
  struct {
    /*
     * Version 0 definition of the ROM header.
     */
    struct {
      unsigned int signature;	/* validation signature                    */
      unsigned int csignature;	/* inverse validation signature            */
      unsigned int hsize;	/* header size                             */
      unsigned int checksum;	/* checksum                                */
      unsigned int size;	/* image size (Memory Footprint)           */
      unsigned int decomp;	/* decompression algorithm                 */
      struct {
	unsigned int low;
	unsigned int high;
      } destination;		/* destination address                     */
    } V0;

    /*
     * Version 1 extensions to the ROM header.
     */
    struct {
      char hversion;		/* ROM header version   (Byte 0)            */
      char fw_id;		/* Firmware ID          (Byte 1)            */
      char hversion_ext;	/* header version ext   (Byte 2)            */
      char reserved;		/* Reserved             (Byte 3)            */
      unsigned int rimage_size;	/* ROM image size                           */
      union {
	char id[8];		/* Optional Firmware ID (character array)   */
	struct {
	  unsigned int low;
	  unsigned int high;
	} id_S;
        struct {
	  unsigned char build_minute;
	  unsigned char build_hour;
	  unsigned char build_day;
	  unsigned char build_month;
	  unsigned char build_year;
	  unsigned char revision;
	  unsigned char minor_version;
	  unsigned char major_version;
        } fw_id_S;
      } fwoptid;
    } V1;

    /*
     * Version 2 extensions to the ROM header.
     */
    struct {
      unsigned int rom_offset;	/* ROM Offset<31:2>, ROM Offset Valid<0> */
    } V2;

    /*
     * Future extensions to the header should be included before
     * this header checksum.  (See HEADER CHECKSUM description)
     */
    unsigned int hchecksum;	/* Header checksum, (Always last entry)     */
  } romh;
  unsigned int romh_uint[1];	/* To allow longword access to the data     */
  unsigned short romh_short[1]; /* To allow word access to the data	    */
  unsigned char romh_char[1];	/* To allow byte access to the data	    */
} romheader_t, *romheader_tp;

typedef struct fw_id {
  int firmware_id;
  char **id_string;
  unsigned int default_fw_offset;
} fw_id_t;

typedef struct os_types {
  int ostype;
  int firmware_id;
  char **id_string;
} ostype_t;

/*
 * The ROM header checksum should always be assigned to the last
 * field in the header.  Therefore, when reading headers created
 * by various versions of makerom the ROM header checksum can be
 * in different locations.  This macro can be used to access the
 * ROM header checksum in the proper location.
 */
#define ROM_HEADER_CHECKSUM(x) \
  ((x)->romh_uint[((x)->romh.V0.hsize - sizeof((x)->romh.hchecksum))/sizeof(unsigned int)])

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
   {ROTATE_RIGHT(k); k += (unsigned char) c; k &= 0xffff;}

#endif /* __ROMHEAD_H_LOADED */
