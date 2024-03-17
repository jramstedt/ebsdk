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

/*
 *  $Id: romhead.h,v 1.2 1999/04/14 20:51:21 paradis Exp $;
 */

/*
 * $Log: romhead.h,v $
 * Revision 1.2  1999/04/14 20:51:21  paradis
 * Added type 11 for server management console
 *
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.32  1997/05/31  03:28:37  fdh
 * Added provisions for platform specific overrides
 * for the definitions which determine the flash usage.
 *
 * Revision 1.31  1997/05/30  03:49:03  fdh
 * Corrected some typos.
 *
 * Revision 1.30  1997/05/30  03:44:57  fdh
 * Added SROM and VxWorks firmware IDs.
 *
 * Revision 1.29  1996/05/22  15:51:29  fdh
 * Added ROM offset definitions for systems which only
 * support one primary flash image.  Also added definitions
 * for using headers with the experimental signature.
 *
 * Revision 1.28  1996/04/29  15:26:47  fdh
 * Added Definitions for Fail-Safe Booter.  Redefined ROM offsets
 * for various Firmware types.
 *
 * Revision 1.27  1996/02/15  15:39:05  fdh
 * Added not to Image Checksum description.
 *
 * Revision 1.26  1996/02/07  18:57:01  cruz
 * Added prototype for IsHeaderValid().
 *
 * Revision 1.25  1995/12/15  21:37:17  cruz
 * Changed default rom offset for NT to 30000.
 *
 * Revision 1.24  1995/11/30  02:18:43  fdh
 * Modified the header specification to include a HEADER REV EXT
 * field to accomodate previously released SROMs which always
 * expected the HEADER REV field to contain a value of 1.
 * Therefore, if this header is modified in the future the
 * HEADER REV EXT field must be incremented to maintain backward
 * compatibility.
 *
 * Revision 1.23  1995/11/16  22:05:03  cruz
 * Added prototype for routine HasRomSignature().
 *
 * Revision 1.22  1995/11/15  23:49:25  fdh
 * Added Milo as the firmware type used to boot Linux.
 *
 * Revision 1.21  1995/11/14  19:14:49  cruz
 * Added prototypes for fwid_match and fwid_dump
 *
 * Revision 1.20  1995/10/30  16:58:18  cruz
 * Commented out routine(s) not in use.
 *
 * Revision 1.19  1995/10/20  18:37:56  fdh
 * Replaced OSF strings with Digital uNIX strings.
 *
 * Revision 1.18  1995/09/19  20:38:31  fdh
 * Added the ROM OFFSET and ROM OFFSET VALID fields to the
 * rom header.  Bumped the header spec revision to version 2.
 *
 * Revision 1.17  1995/08/07  17:33:01  fdh
 * Added support for the Linux Miniloader.
 *
 * Revision 1.16  1995/03/05  00:51:01  fdh
 * Fixed up a couple of MACROS.
 *
 * Revision 1.15  1995/03/04  05:55:57  fdh
 * Bracket COMPUTE_CHECKSUM Macro.
 *
 * Revision 1.14  1995/02/27  15:36:21  fdh
 * Removed ul definitions. Replaced with structs of ui's.
 *
 * Revision 1.13  1995/02/16  22:06:06  fdh
 * Changed Open VMS to OpenVMS because of some trademark junk.
 *
 * Revision 1.12  1995/02/14  21:19:33  cruz
 * Fixed up header picture to match implementation.
 *
 * Revision 1.11  1995/02/10  02:19:03  fdh
 * Created COMPUTE_CHECKSUM Macro.
 * Corrected prototypes.
 *
 * Revision 1.10  1995/02/08  00:34:44  fdh
 * Added ROMH_VERSION and ROTATE_RIGHT macros.
 *
 * Revision 1.9  1995/02/07  22:28:09  fdh
 * Changed Windows NT alias from "WNT" to "NT".
 *
 * Revision 1.8  1995/02/07  04:54:33  fdh
 * Added ostype definitions.
 * Modified ROM_HEADER_CHECKSUM Macro.
 *
 * Revision 1.7  1995/02/06  02:47:40  fdh
 * Added prototypes for romhead.c routines.
 *
 * Revision 1.6  1995/02/05  01:53:05  fdh
 * Modified the definition for the ROM header.
 * Added a MACRO that can be used to access the
 * ROM header checksum which can be in different location
 * for different versions of the header.
 *
 * Revision 1.5  1995/02/02  20:05:31  fdh
 * Moved fwid_array[] initialization to header file.
 *
 * Revision 1.4  1995/01/31  23:44:00  fdh
 * Added field for "OPTIONAL FW ID, continued" to make
 * the optional field really 7 bytes.
 *
 * Revision 1.3  1995/01/31  23:21:25  fdh
 * Updated...
 *
 * Revision 1.2  1995/01/23  22:34:32  fdh
 * Extended the ROM header spec.  This is now version one
 * which is a super-set of the version 0 header.
 *
 * Revision 1.1  1994/11/19  03:47:29  fdh
 * Initial revision
 *
 *
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
 *	  SMC	    11	     Server Management Console
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

#include "local.h"       /* Include environment specific definitions */

#define ROM_H_SIGNATURE 0x5A5AC3C3
#define ROM_H_REVISION  2
#define DEFAULT_ROM_H_REVISION 2
#define ROM_H_BRANCH_SIGNATURE 0xc3e00000
#define ROM_H_BRANCH_OFFSET_MASK 0x1fffff

typedef union {
  struct {
    /*
     * Version 0 definition of the ROM header.
     */
    struct {
      ui signature;		/* validation signature                    */
      ui csignature;		/* inverse validation signature            */
      ui hsize;			/* header size                             */
      ui checksum;		/* checksum                                */
      ui size;			/* image size (Memory Footprint)           */
      ui decomp;		/* decompression algorithm                 */
      struct {
	ui low;
	ui high;
      } destination; 	/* destination address                     */
    } V0;

    /*
     * Version 1 extensions to the ROM header.
     */
    struct {
      char hversion;		/* ROM header version   (Byte 0)            */
      char fw_id;		/* Firmware ID          (Byte 1)            */
      char hversion_ext;	/* header version ext   (Byte 2)            */
      char reserved;		/* Reserved             (Byte 3)            */
      ui rimage_size;		/* ROM image size                           */
      union {
	char id[8];		/* Optional Firmware ID (character array)   */
	struct {
	  ui low;
	  ui high;
	} id_S;
      } fwoptid;
    } V1;

    /*
     * Version 2 extensions to the ROM header.
     */
    struct {
      ui rom_offset;		/* ROM Offset<31:2>, ROM Offset Valid<0> */
    } V2;

    /*
     * Future extensions to the header should be included before
     * this header checksum.  (See HEADER CHECKSUM description)
     */
    ui hchecksum;		/* Header checksum, (Always last entry)     */
  } romh;
  ui romh_array[1];		/* To allow longword access to the data     */
} romheader_t;

/*
 * Registered Firmware types.
 */
#define FW_DBM 0
#define FW_WNT 1
#define FW_SRM 2
#define FW_FSB 6
#define FW_LNX 7
#define FW_VXW 8
#define FW__SR 10
#define FW_SMC 11

#define FW_DBM_STRINGS "Alpha Evaluation Board Debug Monitor", "DBM", "Debug Monitor", "Monitor", NULL
#define FW_WNT_STRINGS "Windows NT Firmware",                  "WNT", "NTFW", "ARC", "NT", NULL
#define FW_SRM_STRINGS "Alpha SRM Console",                    "SRM", "VMS", "UNIX", "OSF", NULL
#define FW_FSB_STRINGS "Alpha Evaluation Board Fail-Safe Booter", "FSB", "Fail-Safe Booter", NULL
#define FW_LNX_STRINGS "Linux Miniloader (Milo)",              "Linux", "Milo", NULL
#define FW_VXW_STRINGS "VxWorks, Real-Time Operating System",  "VxWorks", NULL
#define FW__SR_STRINGS "Serial ROM (SROM)",              "SROM", NULL
#define FW_SMC_STRINGS "Server Management Console", "SMC", NULL


#ifndef FW_FSB_OFFSET
#define FW_FSB_OFFSET 0x00000
#endif

#ifndef FW_SMC_OFFSET
#define FW_SMC_OFFSET 0x00000
#endif

#ifdef FSB_SINGLE_FLASH_IMAGE

#ifndef FW_DBM_OFFSET
#define FW_DBM_OFFSET 0x10000
#endif

#ifndef FW_WNT_OFFSET
#define FW_WNT_OFFSET 0x10000
#endif

#ifndef FW_SRM_OFFSET
#define FW_SRM_OFFSET 0x10000
#endif

#ifndef FW_LNX_OFFSET
#define FW_LNX_OFFSET 0x10000
#endif

#ifndef FW_VXW_OFFSET
#define FW_VXW_OFFSET 0x10000
#endif

#ifndef FW__SR_OFFSET
#define FW__SR_OFFSET 0x00000
#endif

#ifndef FW_DEF_OFFSET
#define FW_DEF_OFFSET 0x10000
#endif

#else  /* FSB_SINGLE_FLASH_IMAGE */

#ifndef FW_DBM_OFFSET
#define FW_DBM_OFFSET 0x00000
#endif

#ifndef FW_WNT_OFFSET
#define FW_WNT_OFFSET 0x30000
#endif

#ifndef FW_SRM_OFFSET
#define FW_SRM_OFFSET 0x80000
#endif

#ifndef FW_LNX_OFFSET
#define FW_LNX_OFFSET 0x30000
#endif

#ifndef FW_VXW_OFFSET
#define FW_VXW_OFFSET 0x30000
#endif

#ifndef FW__SR_OFFSET
#define FW__SR_OFFSET 0x00000
#endif

#ifndef FW_DEF_OFFSET
#define FW_DEF_OFFSET 0x30000
#endif

#endif /* FSB_SINGLE_FLASH_IMAGE */


typedef struct fw_id {
  int firmware_id;
  char **id_string;
  ui default_fw_offset;
} fw_id_t;

extern fw_id_t fwid_array[];

#define FW_OSTYPE_DBM 0
#define FW_OSTYPE_WNT 1
#define FW_OSTYPE_VMS 2
#define FW_OSTYPE_OSF 3
#define FW_OSTYPE_FSB 6
#define FW_OSTYPE_LNX 7
#define FW_OSTYPE_VXW 8
#define FW_OSTYPE__SR 10
#define FW_OSTYPE_SMC 11

#define OS_DBM_STRINGS "Alpha Evaluation Board Debug Monitor", "DBM"
#define OS_WNT_STRINGS "The Windows NT Operating System",      "NT"
#define OS_VMS_STRINGS "OpenVMS",                              "VMS"
#define OS_OSF_STRINGS "Digital UNIX, formerly DEC OSF/1",     "UNIX"
#define OS_FSB_STRINGS "Fail-Safe Booter",                     "FSB"
#define OS_LNX_STRINGS "Linux",                                "Milo"
#define OS_VXW_STRINGS "VxWorks. Real-Time Operating System",  "VxWorks"
#define OS__SR_STRINGS "Serial ROM (SROM)",                    "Srom"
#define OS_SMC_STRINGS "Server Management Console",            "SMC"

typedef struct os_types {
  int ostype;
  int firmware_id;
  char **id_string;
} ostype_t;

extern ostype_t ostype_array[];

/*
 * The ROM header checksum should always be assigned to the last
 * field in the header.  Therefore, when reading headers created
 * by various versions of makerom the ROM header checksum can be
 * in different locations.  This macro can be used to access the
 * ROM header checksum in the proper location.
 */
#define ROM_HEADER_CHECKSUM(x) \
  ((x)->romh_array[((x)->romh.V0.hsize - sizeof((x)->romh.hchecksum))/sizeof(ui)])

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

/*
 * romheader.c prototypes
 */
extern fw_id_t * fwid_match_i(int fwid);
extern ostype_t * ostype_match(char * arg);
extern ostype_t * ostype_match_i(int ostype);
extern void ostype_dump(char * pre);
extern ui compute_romh_chksum(romheader_t * header);
extern int dumpHeader(romheader_t * header);
extern int HasRomSignature (romheader_t *RomImageHeader);
extern fw_id_t * fwid_match(char * arg);
extern void fwid_dump(char * pre);
extern int IsHeaderValid (romheader_t *RomImageHeader);
#endif /* __ROMHEAD_H_LOADED */
