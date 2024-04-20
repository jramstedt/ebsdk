/*
 * ovly_def.h
 *
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
 * Abstract:	Overlay Definitions
 *
 * Author:	John R. Kirchoff
 *
 * Modifications:
 *
 *	jrk	2-Sep-1993	Initial entry.
 */
 
#define TURBO_FLASH_ADR	0x90000000	/* Beginning of Flash Roms */

/*
 * The Turbo flash image headers are as follows:
 *	0 / test pattern	0xaa5500ff
 *      4 / header checksum
 *      8 / id			CPAL, FRRC, CNSL
 *     12 / copy size
 *     16 / copy checksum
 *     20 / load address
 *     24 / index pointer
 *     28 / creation time
 */

#define fihdr_test		0
#define fihdr_cksum		4
#define fihdr_id		8
#define fihdr_c_size		12
#define fihdr_c_cksum		16
#define fihdr_load_adr		20
#define fihdr_index_ptr		24
#define fihdr_ctime		28
#define fihdr_size		32

#define fihdr_pattern		0xaa5500ff

/*
 * The index block is as follows:
 *     ... / "
 *	-8 / 1st image offset
 *	-4 / 0th image offset
 *	 0 / image count
 *	 4 / Image minor version number
 *	 6 / Image sequence number
 *	 8 / Image major version number
 *	10 / Image variant number
 *	12 / Image checksum
 */

#define index_icount	0
#define index_minor	4
#define index_sequence	6
#define index_major	8
#define index_variant	10
#define image_checksum	12

/*
 * The compressed file header is as follows:
 *	  0/ HF01 	identifier
 *	  4/ file spec	filename, 32 chars
 *        8/  "		trucates filename
 *       12/  "
 *       16/  "
 *       20/  "
 *       24/  "
 *       28/  "
 *       32/  "
 *	 36/ olength	length of original uncompressed file.
 *	 40/ otime	creation time of original uncompressed file.
 *	 44/ checksum	checksum of original uncompressed file.
 *       48/ clength	length of compressed file.
 *       52/ data	compressed data
 */

#define file_offset		4
#define olength_offset		36
#define otime_offset		40
#define checksum_offset		44
#define clength_offset		48
#define data_offset		52

/*
 * The DLB block is as follows:
 *   0 / id		DLB0
 *   4 / index block address
 *   8 / length of DLB
 *   c / creation time
 *  10 / overlays
 */

#define dlb_id			0
#define dlb_iblock_adr		4
#define dlb_length		8
#define dlb_creation_time	12 
#define dlb_overlays		16

/*
 * The DLL file header is as follows:
 *	  0/ DLL0		type
 *	  4/ file spec		filename, 32 chars
 *        8/  "			trucates filename
 *       12/  "
 *       16/  "
 *       20/  "
 *       24/  "
 *       28/  "
 *       32/  "
 *	 36/ dll_length		length of DLL file.
 *	 40/ otime		creation time of original file.
 *	 44/ checksum		checksum of original file.
 */

/*
 * The ARC flash image header is as follows:
 *   0	/ id		AFR1			ARC Flash Rom 1
 *   4	/ file spec	max of 32 chars
 *  36	/ creation time
 *  40	/ header length
 *  44  / data length
 *  48  / variant number / major version number
 *  52	/ sequence number / minor version number
 *  56  / checksum
 *  60  / reserved
 *  64  /    data ...
 */

#define arc_id			0
#define arc_file_spec		4
#define arc_creation_time	36 
#define arc_header_length	40
#define arc_data_length		44
#define arc_major		48
#define arc_variant		50
#define arc_minor		52
#define arc_sequence		54
#define arc_checksum		56
#define arc_header_size		64
#define arc_data		64

/*
 * The ARC flash image trailer is as follows:
 *   end -4 	/ id		AFR1		ARC Flash Rom 1
 *   end -8	/ begin offset
 */

#define arc_trailer_id			4
#define arc_trailer_offset		8
#define arc_trailer_size		8

/*
 * flash_s_index -- flash index table
 */
struct flash_s_table {
    int offset;				/* offset into flash */
    int type;				/* file type */
    int fname[8];			/* file name */
    int olength;			/* original length */
    int clength;			/* compressed length */
    int actual_address;			/* actual in memory address */
    int ref_count;			/* reference counter */
};

/*
 * master_cmd_s_table -- master command table
 */
struct master_cmd_s_table {
	int *cmdptr;			/* command table pointer */
	int *hlpptr;			/* help table pointer */
};

/*
 * master_sym_s_table -- master symbol table
 */

struct sym_s_table {
    int	  value;
    char  *name;
};

struct master_sym_s_table {
	int *symptr;			/* Symbol table pointer */
};

/*
 * master_dst_s_table -- master driver startup table
 */

struct dst_s_table {
    int	  (*startup)();
    char  *name;
    int   phase;
    char  *device;
    char  *addit;
};

struct master_dst_s_table {
	int *dstptr;			/* Driver startup table pointer */
};

/* overlay offset definitions */

#define ovl_off_startup		0x10
#define ovl_off_shutdown	0x20
#define ovl_off_id		0x30
#define ovl_off_ver		0x34
#define ovl_off_reloc_fix	0x38
#define ovl_off_routine_fix	0x3C
#define ovl_off_address_fix	0x40
#define ovl_off_storage_fix	0x44
#define ovl_off_entry_count	0x48
#define ovl_off_data_count	0x4C
#define ovl_off_onetime		0x50
#define ovl_off_rtn		0x54

#define ovl_off_symid		0xc
#define ovl_off_symver		0x10
#define ovl_off_symadr		0x14

/* overlay call block */

struct ovly_call_block {
    int name[32];		/* overlay name */
    int offset;			/* offset */
};
