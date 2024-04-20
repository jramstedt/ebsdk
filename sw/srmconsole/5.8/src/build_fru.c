/*
 *  file:    build_fru.c
 *
 * Copyright (C) 1998 by
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
 */
/*
 *++
 *  FACILITY:
 *
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      The build_fru.c module contains routines to build generic
 *	information on platform specific EEPROMs.  EEPROMs have
 *	to conform to the JEDEC specification layout.
 *                                        
 *  AUTHORS:
 *
 *      Nigel J Croxon
 *
 *  CREATION DATE:
 *  
 *      11-Oct-1998
 *
 *  MODIFICATION HISTORY:
 *
 *
 *  11-Oct-1998		NJC	Generic routine calls to build fru data.
 *
 *--
 */

/*Include files*/
#include "cp$src:ansi_def.h"
#include "cp$src:prdef.h"
#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:common.h"
#include "cp$src:pb_def.h"
#include "cp$src:probe_io_def.h"
#include "cp$inc:prototypes.h"
#include "cp$inc:platform_io.h"
#include "cp$src:ctype.h"
#include "cp$src:msg_def.h"
#include "cp$src:qscan_def.h"
#include "cp$src:ev_def.h"
#define DEFINE_GCT_IDS&NAMES 1
#include "cp$src:platform_fru.h"
#include "cp$src:gct.h"
#include "cp$src:gct_tree_init.h"
#include "cp$src:gct_routines.h"
#include "cp$src:gct_util.h"
#include "cp$src:gct_nodeid.h"
#include "cp$src:gct_tree.h"
#include "cp$src:hwrpb_def.h"
#include "cp$src:jedec_def.h"
#include "cp$src:pal_def.h"

#if (WILDFIRE)
#include    "cp$src:get_console_base.h"
#include    "cp$src:wildfire_csr_def.h"
#include    "cp$src:wildfire_sharedram.h"
#endif

extern struct HWRPB *hwrpb;
extern gct_root;
extern struct QUEUE vpd_data_q;

struct VPDDATA {
    struct QUEUE q;
    int adr;
    int hose;
    int slot;
};



#define NUM_ARGS 6
#define QS 0		/* Sys Serial num */
#define QE 1		/* Errors */
#define QA 2
#define QV 3		/* Verbose */
#define QO 4		/* Override */
#define QMAX 5

#define DEC_ID 0xB5
#define CPQ_ID 0x4A



/*+
 * ============================================================================
 * = buildfru - Build a FRU EEPROM data structure from command arguments.     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	BUILDFRU initializes IICbus EEPROM data structures for the named FRU.
 *	This command utilizes user supplied data to build the FRU descriptor.
 *	It also initializes SDD and TDD error logs.  If the environment
 *	variable SYS_SERIAL_NUM is valid, it will also initialize that field
 *	as well as the SMM value for the system.
 *
 *	There are three major areas of the EEPROM that can be initialized, 
 *	the FRU generic data, the FRU specific data, and the system specific
 *	data.  Each of these areas has its own checksum, which is recalculated
 *	any time that segment of the EEPROM is written.
 *	For details on the internal layout of the various FRU EEPROMs see the
 *	"AlphaServer FRU IIC EEPROM Usage Specification".
 *
 *	When the standard BUILDFRU command is executed the FRU EEPROM
 *	is first flooded with zeroes and then the generic data, the system
 *	specific data, and EEPROM format version information is written and
 *	checksums are updated.   For certain FRUs, such as Alpha CPU modules,
 *	there is additional FRU "specific" data that can be entered using the
 *	option, -S.  This data is written to the appropriate region and its
 *	corresponding checksum is updated.
 *
 *	Although this operation is typically performed in manufacturing,
 *	if it is performed in the field the information supplied on the
 *	bar code label for a specific module should be used.  Care should be
 *	taken to make sure that the data entered actually matches the FRU.
 *
 *	Each platform comprises a unique FRU hierarchy.  That is, any system
 *	can be decomposed into a collection of FRUs.  Some FRUs carry other
 *	FRUs.  For instance, a motherboard is a FRU, but it carries a number of
 *	children FRUs.  A child, such as a memory carrier module, may carry
 *	a number of its own children, DIMMs.  So the naming convention for FRUs
 *	in a given system, represents the assembly hierarchy.
 *	
 *	The following is the general form of a FRU name.
 *	
 *      <frun>[.<frun>[.<frun>[.<frun>[<.frun>]]]]
 *
 *	where "fru" is a placeholder for the appropriate FRU type at that level
 *	for a given platform and 'n' is the number of that FRU instance on that 
 *	branch of the system hierarchy.
 *
 *	Clippers are a relatively bounded systems and their FRU assembly
 *	hierarchy has 3 defined levels. The defined FRU types at the various
 *	levels is listed here. 
 *##
 *	1 SMB0,CPB0,JIO0,OCP0,SBM0,PWR(0-2),FAN(0-6)
 *	2 CPU(0-3),MMB(0-3),PCI(0-9)
 *	3 DIM(0-7)
 *#
 *	For Wildfire systems the hierarchy has 5 levels and the allowable
 *	FRU types at the various levels is listed here.
 *##
 *	1 C16(0),C32(0),C08(0),PIO(0),XIO(0-7) 
 *	2 FBX(0-3),QPD(0-1),IOM(0-F),IOX(0-F),PWR(0-3),OCP(0-1),FAN0,ACX
 *	3 QBB(0-1),PBM(0-F),RPS(0-2)
 *	4 CPU(0-3),MEM(0-3),DIR,GPM,IOR,CLK,PSM,MAN,AUX,SIO,PCI(0-D)
 *	5 DIM(0-7)
 *#
 * COMMAND FORM:
 *            
 *   	buildfru ( <fru_name> <part_num> <serial_num> [<model> [<alias>]]
 *			or
 *		-s <fru_name> <offset> <byte> [<byte>...] )
 *
 * COMMAND ARGUMENT(S):
 *
 *   <fru_name> - Specifies the console name for this FRU.  This name reflects
 *	the position of the FRU in the assembly hierarchy and has the form:
 *      <frun>[.<frun>[.<frun>[.<frun>[<.frun>]]]], where "fru" is a placeholder
 *	for the FRU type at that level and "n" is the instance of that FRU type
 *	at that level.  This format allows for reasonable location and
 *	identification of FRUs.  Platforms may have a different number of 
 *	assembly levels and FRU types, hence, each platforms FRU name space
 *	is unique.
 *
 *   <part_num> - Specifies this FRU's Digital 2-5-2.4 part number.
 *	This ASCII string should be 16 characters (extras are truncated).
 *	This field should not contain any embedded spaces. If there is a need
 *	to insert a space, then the entire argument string must be enclosed
 *	in double quotes.  Note, that this field contains the FRU revision,
 *	and in some cases an embedded space is allowed between the part number
 *	and the revision.
 *
 *   <serial_num> - Specifies this FRU's serial number.
 *	This ASCII string must be 10-12 characters (extras are truncated).
 *	Note, the manufacturing location and date are extracted from this field.
 *
 *   <model> - Specifies this FRU's model name or number or a.k.a. name.
 *	This ASCII string may be up to 10 characters (extras are truncated).
 *	This field is optional, unless <alias> is specified.
 * 
 *   <alias> - Specifies this FRU's Compaq alias number, if one exists. 
 *	This ASCII string may be up to 16 characters (extras are truncated).
 *	This field is optional.
 * 
 *   <offset> - Specifies the beginning byte offset (hex) within this FRU's
 *	EEPROM, where the following supplied data bytes are to be written.
 * 
 *   <byte>... - Specifies the data byte(s) (up to 16) to be written. Atleast
 *	one data byte must be supplied after the offset.
 *
 *   COMMAND OPTION(S):
 *
 *	-s - Specifies that this raw data should be written to the EEPROM
 *	this option is typically used to apply any FRU specific data.
 *
 *   COMMAND EXAMPLE(S):
 *
 * In the following example, some Clipper FRU EEPROMs are built.
 * Note, the addition of some FRU specific information for the CPUs. 
 *~
 * P00>>> buildfru smb0 54-B2001-AA.A01 AY23401234 sysmthrbrd xyz0001
 * P00>>> buildfru smb0.cpu0 54-B2001-AA.A01 AY23402234 clipprcpu abc1111
 * P00>>> buildfru -s smb0.cpu0 00 90 09 22 32
 *~
 * Below is an example of building some Wildfire CPUs.
 *~
 * P00>>> buildfru cab0.qbb0.cpu0 54-B2001-AA.A01 AY234012345
 * P00>>> buildfru cab0.qbb0.cpu1 54-B2001-AA.A01 AY234012346
 * P00>>> buildfru cab0.qbb0.cpu2 54-B2001-AA.A01 AY234012347
 * P00>>> 
 *~
 *   COMMAND REFERENCES:
 *
 *	clear_error, show error, show fru
 *
 * COMMAND FMT:   buildfru 3 DZ 0 buildfru
 *
 * COMMAND TAG:   buildfru 0 RXBZ buildfru
 *
 * FORM OF CALL:
 *  
 *	build_ee_log(int argc, char *argv)
 *  
 * RETURN CODES:
 *
 *	msg_success - successful completion
 *	msg_failure - otherwise
 *       
 * ARGUMENTS:
 *
 *	argc - Count of the number of command line parameters
 *	argv - Array of pointers to the command line parameters
 *
 * SIDE EFFECTS:
 *
 *	Performs operations on the serial control bus.
 *
-*/
#if (!WILDFIRE)
int buildfru (int argc, char *argv[]) {
    struct QSTRUCT          qual[QMAX];
    unsigned int	    i, j, k=3, p = 0, q = 0, t = 0, status, whomfg;
    int                     localOffset, flag=0, convert, addIndex=0;
    unsigned int            return_checksum=0;
    char                    *pLocal;
    unsigned char           localBuf[2], mfgLocBuf[10], mfgAlias[16], mfgPart[19];
    unsigned char           mfgDate[2] = {0,0}, mfgAssSer[4] = {0,0};
    unsigned char           c[2], b[5];
    int                     asem[6];
    FRU_NAME_LIST           *pFru_names;
    int			    supports_iic = 0;

    status = qscan( &argc, argv, "-", "s e a v o", qual );
    if( status != msg_success )	{
	err_printf( status );
	return( status );
    }

    for (j=1; j < argc; j++)
	for (i=0; argv[j][i]=toupper(argv[j][i]); i++);	/* Ensure its uppercase*/


    /* Flag list  flag = 0 - string
    *                  = 1 - integer
    *                  = 2 - verbose
    */
    if (qual[QV].present) flag = 2;

    /* First line of defense
    *  Check to see if device supports EEPROMS
    */
    pFru_names   =  fru_names;
    supports_iic = support_iic_fru(argv[1], pFru_names);
    if (supports_iic == -1) {
 	printf("could not find FRU Identifier %s\n",argv[1]);
	return msg_failure;
    }

    /*
    *
    *  Check for -s qualifier, build specific data
    */

    if (qual[QS].present) {
	if (argc < 4) {
	    printf("not enough arguments\n");
	    return msg_failure;
	}
	printf(" Building FRU -s on %s\n",argv[1]);
	for (j=0; j< argc - 3; j++) {
	    convert = xtoi(argv[k]);
	    addIndex = xtoi(argv[2]);
	    localOffset = addIndex + j;
	    status = write_jedec(argv[1], convert, localOffset, 1, flag);
	    k++;
	    if (status != msg_success) break;
	}
	/* Do checksuming */
	if ( localOffset <= 0x3F) {
	    return_checksum = jedec_checksum(argv[1], 0, 0x3E);
	    status = write_jedec(argv[1], return_checksum, offsetof(struct _JEDEC, jedec_b_checksum0to62), 1, flag);
	}
	if ( ( localOffset >= 0x40) && ( localOffset <= 0x7F) ) {
	    return_checksum = jedec_checksum(argv[1], 0x40, 0x3E);
	    status = write_jedec(argv[1], return_checksum, offsetof(struct _JEDEC, jedec_b_checksum64to126), 1, flag);
	}
	if ( localOffset >= 0x80 ) {
	    return_checksum = jedec_checksum(argv[1], 0x80, 0x7E);
	    status = write_jedec(argv[1], return_checksum, offsetof(struct _JEDEC, jedec_b_checksum128to254), 1, flag);
	}
	/*
	*   Call GCT to rebuild the config tree (because of this cmd)
	*/
	return msg_success;
     }
 
    /* Build -e to clear just the TDD and SDD area */
    if (qual[QE].present) {
	if (argc < 2) {
	    printf("not enough arguments\n");
	    return msg_failure;
	}
	printf(" Clearing TDD and SDD area on %s\n",argv[1]);
	for (j=144; j< 253; j++) {
	    localOffset = j;
	    status = write_jedec(argv[1], 0, localOffset, 1, flag);
	    if (status != msg_success) break;
	}
	/* Do checksuming */
	return_checksum = jedec_checksum(argv[1], 0x80, 0x7E);
	status = write_jedec(argv[1], return_checksum, offsetof(struct _JEDEC, jedec_b_checksum128to254), 1, flag);

	/*
	*   Call GCT to rebuild the config tree (because of this cmd)
	*/
	gct();

	return msg_success;
     }

     /* We normally need 3 arguents (command + 3 args == 4) */
     if (argc < 4) {
	printf("not enough arguments\n");
	return msg_failure;
     }

    /* Serial numbers must hast 10-12 chars */
    if (( strlen(argv[3]) <= 9 ) || ( strlen(argv[3]) >= 13 ))  {
        err_printf(msg_insuff_params);
        return msg_failure;
    }
    /* Clear Manufacturer Location to Manufacturing Specific Data */
    for (j=72; j< 126; j++) {
	    localOffset = j;
	    status = write_jedec(argv[1], 0, localOffset, 1, flag);
	    if (status != msg_success) break;
    }
    /* Clear second half of eeprom
    */
    for (j=128; j< 256; j++) {
	    localOffset = j;
	    status = write_jedec(argv[1], 0, localOffset, 1, flag);
	    if (status != msg_success) break;
    }
    /* set the Manufacturer Location byte
    */
    whomfg = 0;
    if( strncmp( argv[3], "NI", 2 ) == 0 ) whomfg = 1;	/* Salem, NH.       */
    if( strncmp( argv[3], "AY", 2 ) == 0 ) whomfg = 2;	/* Ayr, Scotland    */
    if( strncmp( argv[3], "ZG", 2 ) == 0 ) whomfg = 3;	/* Dec, Sinapore    */
    if( strncmp( argv[3], "S3", 2 ) == 0 ) whomfg = 4;	/* Compaq Asia = CA */
    write_jedec(argv[1], whomfg, offsetof(struct _JEDEC, jedec_b_manuf_location), 1, flag);

    if ( (qual[QO].present == 0) && ( whomfg == 0) ) {
	printf("BAD MFG site code\n");
	return msg_failure;
    }

    if (qual[QO].present) whomfg = 0;

    /* write to device mfg part class and Space Space
    */

    if ( whomfg != 0) {
	for (j=0; j < 17; j++) {
	    localOffset = j;
	    status = write_jedec(argv[1], 0x20, offsetof(struct _JEDEC, jedec_b_manuf_part_class + j), 1, flag);
	}
	write_jedec(argv[1], argv[2], offsetof(struct _JEDEC, jedec_b_manuf_part_class), 0, flag);
	write_jedec(argv[1],    "  ", offsetof(struct _JEDEC, jedec_b_manuf_part_space), 0, flag);
    } else {
	write_jedec(argv[1], argv[2], offsetof(struct _JEDEC, jedec_b_manuf_spec_alias), 0, flag);
    }

    /* Ripe apart  arg 3 (serial number) into its pieces
    *   AY84412345
    */
    if ( whomfg != 0 ) {
	/* write the Mfg Build Year
	*/
	j = 0;
	strncpy(mfgDate,argv[3]+2,1);
	c[0] = (int *)*mfgDate;
	j = c[0] & 0x0f;
	write_jedec(argv[1], j, offsetof(struct _JEDEC, jedec_b_manuf_date_y), 1, flag); 

	/* write the Mfg Build Week
	*/
	strncpy(mfgDate,argv[3]+3,2);
	b[0] = (int *)*mfgDate;
	strncpy(mfgDate,argv[3]+4,2);
	b[1] = (int *)*mfgDate;
	*c = ( b[1] & 15 ) | (( b[0] & 15) << 4);
	write_jedec(argv[1], *c, offsetof(struct _JEDEC, jedec_b_manuf_date_m), 1, flag);

	/* write the assembly serial number
	*/
	strncpy(b, argv[3]+5, 5);
	for (j=0; j<5; j++) {
	    t |= ((b[j] - 32) & 0x3f) << ( 6 * j );
	}
	asem[0] = t;
	write_jedec(argv[1], asem, offsetof(struct _JEDEC, jedec_b_assembly_serialnum), 0, flag);
    } else {
	strncpy(mfgLocBuf, argv[3], 16);
	write_jedec(argv[1], mfgLocBuf, offsetof(struct _JEDEC, jedec_b_manuf_spec_model), 0, flag);
    }

    /* write mfg model
    */
    if (argc > 4 ) {
	if ( whomfg != 0) {
	    strncpy(mfgLocBuf, argv[4], 10);
	    write_jedec(argv[1], mfgLocBuf, offsetof(struct _JEDEC, jedec_b_manuf_spec_model), 0, flag);
	}
	else {
	    strncpy(mfgPart, argv[4], 18);
	    write_jedec(argv[1], mfgPart, offsetof(struct _JEDEC, jedec_b_manuf_part_class), 0, flag);
	}
    }

    /* write mfg alias
    */
    if (argc > 5 ) {
	if ( whomfg != 0) {
	    strncpy(mfgAlias, argv[5], 16);
	    write_jedec(argv[1], mfgAlias, offsetof(struct _JEDEC, jedec_b_manuf_spec_alias), 0, flag);
	}
    }

    /*  Write COMPAQ's JEDEC ID code
    *  B5 = DEC,  4A = CPQ
    */
    if ( whomfg != 0) {
	localOffset = offsetof(struct _JEDEC, jedec_b_manuf_JEDEC_ID);
	status = write_jedec(argv[1], CPQ_ID, localOffset, 1, flag); localOffset++;
	status = write_jedec(argv[1], 0xff, localOffset, 1, flag); localOffset++;
	status = write_jedec(argv[1], 0xff, localOffset, 1, flag); localOffset++;
	status = write_jedec(argv[1], 0xff, localOffset, 1, flag); localOffset++;
	status = write_jedec(argv[1], 0xff, localOffset, 1, flag); localOffset++;
	status = write_jedec(argv[1], 0xff, localOffset, 1, flag); localOffset++;
	status = write_jedec(argv[1], 0xff, localOffset, 1, flag); localOffset++;
	status = write_jedec(argv[1], 0xff, localOffset, 1, flag);
	status = write_jedec(argv[1], CPQ_ID, offsetof(struct _JEDEC, jedec_b_dec_flag_id), 1, flag);
    }
    status = write_jedec(argv[1], CPQ_ID, offsetof(struct _JEDEC, jedec_b_manuf_spec_dec_JEDEC_ID), 1, flag);

    if (qual[QO].present)
	status = write_jedec(argv[1], CPQ_ID, offsetof(struct _JEDEC, jedec_b_dec_flag_id), 1, flag);

    /* Revision of Read-Only area (byte 126, x7E and byte 254, xFE)
    *  x20 = Revision V2.0
    *  x21 = Revision V2.1
    */
    status = write_jedec(argv[1], 0x20, offsetof(struct _JEDEC, jedec_b_revision_ro_data), 1, flag);
    status = write_jedec(argv[1], 0x21, offsetof(struct _JEDEC, jedec_b_rev_rw_area), 1, flag);

    /*
    *  Time to CheckSum
    */
    return_checksum = jedec_checksum(argv[1], 0x40, 0x3E);
    status = write_jedec(argv[1], return_checksum, offsetof(struct _JEDEC, jedec_b_checksum64to126), 1, flag);

    return_checksum = jedec_checksum(argv[1], 0x80, 0x7E);
    status = write_jedec(argv[1], return_checksum, offsetof(struct _JEDEC, jedec_b_checksum128to254), 1, flag);

    /*
    *   Call GCT to rebuild the config tree (because of this cmd)
    */
    gct();

 }
#else
int buildfru (int argc, char *argv[]) {
    return msg_success;
}
#endif

/*+
 * ============================================================================
 * = show fru - Show the FRUs in a system.                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Display a table of the Field Replaceable Units (FRUs) in the system.
 *      For each FRU show its FRU name (according to the firmware), part
 *      number, serial number, error status, and any miscellaneous data.
 *
 *      SHOW FRU displays a table of information which is preceeded by a line
 *      of column headers described in more detail below.
 *#o
 *      o "FRUname" is the console name for the field replaceable unit (FRU),
 *        and reflects the location of that FRU in the assembly hierarchy of
 *        the system.
 *      o "Err" indicates whether the FRU has any errors logged against it.
 *        Healthy FRUs have a "00" in this column, FRUs with errors have a
 *        non-zero value, which is a bitmask of possible error conditions
 *        for that FRU.  The meaning of each bit is described below.
 *##
 *              If Bit 0 is 1, a FAILURE
 *              If Bit 1 is 1, a TDD error has been logged.
 *              If Bit 2 is 1, at least one SDD error has been logged.
 *              If Bit 3 is 1, FRU EEPROM is unreadable.
 *              If Bit 4 is 1, checksum failure on bytes 0-62.
 *              If Bit 5 is 1, checksum failure on bytes 64-126.
 *              If Bit 6 is 1, checksum failure on bytes 128-254.
 *              If Bit 7 is 1, FRUs system serial does not match, system's.
 *		    ...
 *		    If Bit n is 1, tbd...note the byte could grow if needed.
 *#
 *      o "Part#" column contains the FRUs part number in ASCII.  For Digital
 *        FRUs this is the 2-5-2.4 part number.  For other non-Digital FRUs
 *        such as DIMMs, the vendor specific part number field is displayed
 *        in ASCII.
 *      o "Serial#" column displays the part serial number.  For Digital FRUs
 *        this has the form XXYWWNNNNN, where XX is the manufacturing location
 *        code, YWW is the year and week, and NNNNN is the sequence number.
 *        For other FRUs, the four byte sequence number is displayed in hex.
 *      o "Misc." column displays option FRU information.  For Digital
 *        For other non-Digital FRUs the manufacturer's name is displayed here.
 *      o "Other" column displays other optional data.  For Digital
 *        FRUs the Compaq 6.3 part alias number (if one exists) is displayed.
 *        For other non-Digital FRUs, the year and week number are displayed.
 *#
 *      Generally all FRUs are displayed which have electronically readable
 *      FRU EEPROMs.  In some cases, FRUs without EEPROMs maybe displayed
 *      if their presence can be detected by inference.  In these cases, only
 *      the base part number will be displayed.
 *
 *   COMMAND FORM:
 *
 *       show fru ( [-e] )
 *
 *   COMMAND ARGUMENT(S):
 *
 *       None
 *
 *   COMMAND OPTION(S):
 *
 *       -e - Specifies that only FRUs which have errors logged are displayed.
 *
 *   COMMAND EXAMPLE(S):
 *
 * Typing SHOW FRU displays all the FRU information for the system.
 * Here is an example of a Clipper system.
 *~
 * P00>>> show fru
 * FRUname        Err Part#              Serial#      Misc.      Other
 * SMB0           00  54-28357-01.B02    NI87654321   Motherload COMPAQ08765432`
 * SMB0.CPU0      00  54-24773-02.A01    AY76543210   CPU_Module COMPAQ98765432`
 * SMB0.CPU1      01  54-24773-02.A01    AY76543211   CPU_Module COMPAQ98765432`
 * SMB0.MMB1      00  54-24773-02.A01    AY76543210   Mem_Module COMPAQ98765432`
 * SMB0.MMB1.DIM1 00  ASCIIDUMPOFPART#   00ABCDEF     HITACHI    98-41
 * SMB0.MMB1.DIM2 00  ASCIIDUMPOFPART#   00ABCDEF     HITACHI    98-41
 * SMB0.MMB1.DIM3 00  ASCIIDUMPOFPART#   00ABCDEF     HITACHI    98-41
 * SMB0.MMB1.DIM4 00  ASCIIDUMPOFPART#   00ABCDEF     HITACHI    98-41
 * CPB0           00  54-25573-01.B      NI76543212   ES30LAL    543212
 * CPB0.PCI0      00
 * CPB0.PCI3      00
 * CPB0.PCI5      00
 * PWR0           00  54-00000-01        AY78978979   ES40       9789789
 * PWR1           00  54-00000-01        AY78978978   ES40       9789789
 * PWR2           00  54-00000-01        AY78978977   ES40       9789789
 * FAN1           00  B2262-33           -            Fan
 * FAN2           00  B2262-33           -            Fan
 * FAN3           00  B2262-33           -            Fan
 * FAN4           01  B2262-33           -            Fan
 * FAN5           00  B2262-33           -            Fan
 * FAN6           00  B2262-33           -            Fan
 * JIO0           00  54-25575-01        -            Junk I/O
 * P00>>> show fru -e
 * SMB0.CPU1      01  54-24773-02.A01    AY76543211   CPU_Module COMPAQ98765432`
 * FAN4           01  B2262-33           -            Fan
 * P00>>>
 *~
 * Here is an example of a Wildfire system.
 *~
 * P00>>> show fru
 * FRUname        Err Part#              Serial#      Misc.      Other
 * QBB0           00  AB-B2345-BA.0001   AY012XY345   B2345-BA
 * QBB0.CPU0      00  AB-B2000-BA.0001   AY000XY001   B2000-BA
 * QBB0.CPU1      00  AB-B2000-BA.0001   AY000XY002   B2000-BA
 * QBB0.MEM0      00  AB-B1000-BB.0001   AY999MA066   B1000-BA
 * QBB0.MEM0.DIM0 00  ASCIIDUMPOFPART#   00ABCDEF     HITACHI    98-41
 * QBB0.MEM0.DIM1 00  ASCIIDUMPOFPART#   00ABCDEF     HITACHI    98-41
 * QBB0.MEM0.DIM2 00  ASCIIDUMPOFPART#   00ABCDEF     HITACHI    98-41
 * QBB0.MEM0.DIM3 00  ASCIIDUMPOFPART#   00ABCDEF     HITACHI    98-41
 * QBB0.MEM1      00  AB-B1000-BB.0001   AY999MA366   B1000-BA
 * QBB0.MEM1.DIM0 00  ASCIIDUMPOFPART#   00ABCDEF     HITACHI    98-41
 * QBB0.MEM1.DIM1 00  ASCIIDUMPOFPART#   00ABCDEF     HITACHI    98-41
 * QBB0.MEM1.DIM2 00  ASCIIDUMPOFPART#   00ABCDEF     HITACHI    98-41
 * QBB0.MEM1.DIM3 00  ASCIIDUMPOFPART#   00ABCDEF     HITACHI    98-41
 * QBB0.IOR0      00  AB-I9900-BC.0001   KN111QT009   I9900-BC
 * QBB0.IOR1      00  AB-I9900-BC.0001   KN111QT008   I9900-BC
 * QBB0.DIR       00  AB-C3300-BC.0001   KN222QT001   C3300-BC
 * QBB0.PSM       00  AB-P0100-BC.0001   KN444QT003   P0100-BC
 * QBB0.PWR       00  AB-P0100-BC.0001   KN444QT003   P0100-BC
 * QBB0.AUX       00  AB-P0100-BC.0001   KN444QT003   P0100-BC
 * QBB0.CLK       00  AB-P0100-BC
 * FAN0           00  AB-BLO00-BC
 *~
 * To display subsets of FRU information filter the output using the
 * console GREP command with an appropriate match string.
 *~
 * P00>>> show fru | grep CPU
 * QBB0.CPU0      00  AB-B2000-BA.0001   AY000XY001   B2000-BA
 * QBB0.CPU1      00  AB-B2000-BA.0001   AY000XY002   B2000-BA
 * QBB1.CPU0      00  AB-B2000-BA.0001   AY000XY001   B2000-BA
 * P00>>>
 *~
-*/
#if (!WILDFIRE)
int show_fru (int argc, char *argv[]) {

  int                 j = 0, i = 6, m = 0, status, sp, config_addr =0, flag=0, *intbuf;
  unsigned int        exp = 0, rcv = 0;
  GCT_ROOT_NODE       *pRoot;
  GCT_HANDLE          fru_root = 0, node = 0;
  GCT_NODE            *pNode, *pNext;
  GCT_FRU_DESC_NODE   *pFrudesc;
  GCT_TLV             *pTLV, *pTMP;
  FRU_NAME_LIST       *pFru_names; 
  char                *fruname;
  unsigned char       buffer[16], hd_buffer[64];
  unsigned __int64    tdd_head_value;
  unsigned int        rtn_checksum=0, rtn_id=0;
  struct QSTRUCT      qual[QMAX];


  status = qscan( &argc, argv, "-", "s e v", qual );
  if( status != msg_success )	{
	err_printf( status );
	return( status );
  }

  if (qual[QV].present) flag = 2;
  if (qual[QS].present)
	printf("FRUname           Sys_serial_num\n");
  else  printf("FRUname         E  Part#              Serial#    Model/Other Alias/Misc\n");

  pFru_names = fru_names;
  pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS( 0);
  status = gct$find_node(0,&fru_root, FIND_BY_TYPE,NODE_FRU_ROOT,  0, 0, 0, 0);
  pNode = _GCT_MAKE_ADDRESS(fru_root);
  
  /* Walk the tree until we have seen the FRU_ROOT twice */
  pNext = pNode;
  while (pNext && (j < 2)) { 

	/* find the next Node */
	status = gct$find_node(0,&node, FIND_ANY, pNext , 0, 0, 0, 0);
	pNext = _GCT_MAKE_ADDRESS(node);
	pFrudesc = _GCT_MAKE_ADDRESS(node);

	/* Look only for type=15 (gct_fru_desc nodes)
	*/
	if (pNext->type == 0x15) {
	    m = 0;
	    /* Do-While we still have nodes to compare against in the list */
	    while ( pFru_names[m].fru_id != 0) {
		/* Does our just found node's ID Match in the list ?  */
		if (pNext->id == pFru_names[m].fru_id) {
		    fruname = pFru_names[m].name;
		    if ( support_iic_fru(fruname, pFru_names) ) {
			/* display only sys_serial if -s present */
			if (qual[QS].present) {
			    status = read_jedec(fruname, &buffer, 16, offsetof( struct _JEDEC, jedec_b_sys_serialnumb), flag);
			    printf("%-14.14s    %-16.16s\n", fruname, buffer );
			} else {
			    if ( (!qual[QE].present) || ((qual[QE].present) && ((pFrudesc->fru_desc_info.fru_diag_flag & 0xff) != 0) )) {
				printf("%-14.14s  %02.2x ",pFru_names[m].name, (pFrudesc->fru_desc_info.fru_diag_flag & 0xff) );
				pTLV = ( GCT_TLV * ) &pFrudesc->fru_desc_info.fru_info;
		 		_ACCESS_TLV( pTLV, 2, pTMP );
				FruPrintTLV("%-18.18s ", pTMP->tlv_value);
				_ACCESS_TLV( pTLV, 3, pTMP );
				FruPrintTLV("%-10.10s ", pTMP->tlv_value);
			        _ACCESS_TLV( pTLV, 1, pTMP );
			        FruPrintTLV("%-10.10s  ", pTMP->tlv_value);
			        _ACCESS_TLV( pTLV, 0, pTMP );
			        FruPrintTLV("%-16.16s", pTMP->tlv_value);
			        printf("\n");
			    }
			} /* end - QUAL -s */

		    } else { 	/* end- support_iic - else */
			if ( (!qual[QS].present) && (!qual[QE].present) ) {
			    printf("%-14.14s  00 ",pFru_names[m].name );
			    pTLV = ( GCT_TLV * ) &pFrudesc->fru_desc_info.fru_info;
			    _ACCESS_TLV( pTLV, 2, pTMP );
			    FruPrintTLV("%-18.18s ", pTMP->tlv_value);
	 		    _ACCESS_TLV( pTLV, 3, pTMP );
			    FruPrintTLV("%-10.10s ", pTMP->tlv_value);
			    _ACCESS_TLV( pTLV, 1, pTMP );
			    FruPrintTLV("%-10.10s  ", pTMP->tlv_value);
			    _ACCESS_TLV( pTLV, 0, pTMP );
			    FruPrintTLV("%-16.16s", pTMP->tlv_value);
			    printf("\n");
			}
		    }
		} /* end ID match */
	        m++;
	    }
	}
	/* Give us a bailout path */
	if ( killpending () ) return msg_ctrlc;
	if (pNext->type == 0x14) j++;
  }
  return msg_success;
}

#else
#define DEFINE_GCT_IDS&NAMES 1

int show_fru (int argc, char *argv[]) {
  struct QSTRUCT           qual[QMAX];
  int                      i = 0, bigloop = 0, sp, j, m, h, k;
  int                      status, flag=0, box;
  GCT_HANDLE               root = 0, node = 0;
  GCT_NODE                 *pNode, *pNext, *pFnode, *pHeader;
  GCT_FRU_DESC_NODE        *pFru_desc;
  GCT_HD_EXT               *pHd_ext;
  GCT_SUBPACK              *pSubpack;
  unsigned char		   *buf;
  struct HWRPB             *hwrpb;
  GCT_NAMES                *pNames;
  GCT_TLV                  *pTLV, *pTMP;
  GCT_WF_ID_LIST           *pQbb_id, *pPci_id, *pMem_id, *pDim_id, *pMisc_id, *pOdd_id;
  unsigned __int64         new_id;
  GCT_NODE_ID_FRU_DESC     *pNodeid_fru_desc;

  status = qscan( &argc, argv, "-", "s e v", qual );
  if( status != msg_success )	{
	err_printf( status );
	return( status );
  }

  if (qual[QV].present) flag = 2;
  if (qual[QS].present)
	printf("FRUname           Sys_serial_num\n");
  else  printf("FRUname           E  Part#              Serial#    Model/Other Alias/Misc\n");

  pNames = gct_type_names;
  pQbb_id = &wf_qbb_id_fru_list;
  pPci_id = &wf_pci_id_fru_list;
  pDim_id = &wf_dimm_id_fru_list;
  pOdd_id = &wf_odd_id_fru_list;
  pMisc_id = &wf_misc_id_fru_list;

  buf = hwrpb->FRU_OFFSET[0] + 0x2000;
  pHeader = (GCT_NODE *) buf;
  status = gct$find_node(0,&root, FIND_BY_TYPE, NODE_ROOT,  0, 0, 0, 0);
  pNode = _GCT_MAKE_ADDRESS(root);

  while (pNode && (bigloop < 1)) { 

    /* find the next Node */
    status = gct$find_node(0,&node, FIND_ANY, pNode, 0, 0, 0, 0);
    pNode = (struct GCT_FRU_DESC_NODE *) _GCT_MAKE_ADDRESS(node);

    /* Look only for type=15 (gct_fru_desc nodes)
    */
    if ( ( pNode->node_flags & NODE_PRESENT ) && (pNode->type == 0x15)) {

	/* Turn on Reverse Video */
	if ( pNode->node_flags & NODE_PWR_DOWN ) printf("\x1B\x5B\x37\x6D");

	pFru_desc = (struct GCT_FRU_DESC_NODE *) pNode;
	pTLV = &pFru_desc->fru_desc_info.fru_info.tlv_tag;

	    /* Check for Misc Adapters */
	    m = 0;
	    while ( pMisc_id[m].wf_id != 0) {
		if ( pNode->id == pMisc_id[m].wf_id ) {
			    printf("%-16.16s  %02.2x ",pMisc_id[m].name, (pFru_desc->fru_desc_info.fru_diag_flag & 0xff) );
			    pTLV = &pFru_desc->fru_desc_info.fru_info.tlv_tag;
			    _ACCESS_TLV( pTLV, 2, pTMP );
			    FruPrintTLV("%-18.18s ", pTMP->tlv_value);
			    _ACCESS_TLV( pTLV, 3, pTMP );
			    FruPrintTLV("%-10.10s ", pTMP->tlv_value);
			    _ACCESS_TLV( pTLV, 1, pTMP );
			    FruPrintTLV("%-10.10s  ", pTMP->tlv_value);
			    _ACCESS_TLV( pTLV, 0, pTMP );
			    FruPrintTLV("%-16.16s", pTMP->tlv_value);
			    printf("\n");
			    break;
		}
		m++;
	    }

	    /* Check for Dimm's */
	    for ( h = 0; h < 8; h++ ) {
		for ( j = 0; j < 4; j++ ) {
		    for ( k = 0; k < 8; k++ ) {
			new_id = pQbb_id[h].wf_id;
			pNodeid_fru_desc = &new_id;
			pNodeid_fru_desc->node_id_fru_desc_subassembly = j;
			pNodeid_fru_desc->node_id_fru_desc_slot = pDim_id[ ( j * 8 ) + k].wf_id;
			if ( pNode->id == new_id ) {
			    printf("%s.%s.%-6.6s  %02.2x ",pQbb_id[h].name,pOdd_id[ j + 4 ].name,pDim_id[k].name, 
				  (pFru_desc->fru_desc_info.fru_diag_flag & 0xff) );
			    _ACCESS_TLV( pTLV, 2, pTMP );
			    FruPrintTLV("%-18.18s ", pTMP->tlv_value);
			    _ACCESS_TLV( pTLV, 3, pTMP );
			    FruPrintTLV("%-10.10s ", pTMP->tlv_value);
			    _ACCESS_TLV( pTLV, 1, pTMP );
			    FruPrintTLV("%-10.10s  ", pTMP->tlv_value);
			    _ACCESS_TLV( pTLV, 0, pTMP );
			    FruPrintTLV("%-16.16s", pTMP->tlv_value);
			    printf("\n");
			    break;
			}
		    }  /* end DImm loop */
		}  /* end Mem loop */
	    } /* end Qbb loop */



	    /* Check for DIR Dimm's & GP & etc.  */
	    for ( h = 0; h < 8; h++ ) {
		    for ( k = 0; k < 24; k++ ) {
			new_id = pQbb_id[h].wf_id;
			pNodeid_fru_desc = &new_id;
			pNodeid_fru_desc->node_id_fru_desc_slot = pOdd_id[ k].wf_id;
			if ( (k >= 8) && ( k <= 15) )
			    pNodeid_fru_desc->node_id_fru_desc_subassembly = 4;
			else
			    pNodeid_fru_desc->node_id_fru_desc_subassembly = 0xFF;

			if ( pNode->id == new_id ) {
			    if ( (k >= 8) && ( k <= 15) )
				printf("%s.DIR0.%-6.6s  %02.2x ",pQbb_id[h].name,pOdd_id[ k].name, (pFru_desc->fru_desc_info.fru_diag_flag & 0xff) );
			    else
				printf("%s.%-11.11s  %02.2x ",pQbb_id[h].name,pOdd_id[ k].name, (pFru_desc->fru_desc_info.fru_diag_flag & 0xff) );

			    _ACCESS_TLV( pTLV, 2, pTMP );
			    FruPrintTLV("%-18.18s ", pTMP->tlv_value);
			    _ACCESS_TLV( pTLV, 3, pTMP );
			    FruPrintTLV("%-10.10s ", pTMP->tlv_value);
			    _ACCESS_TLV( pTLV, 1, pTMP );
			    FruPrintTLV("%-10.10s  ", pTMP->tlv_value);
			    _ACCESS_TLV( pTLV, 0, pTMP );
			    FruPrintTLV("%-16.16s", pTMP->tlv_value);
			    printf("\n");
			    break;
			}
		    }  /* end DImm loop */
	    } /* end Qbb loop */


	    /* looking for PCI - BackPlanes */
	    m = 0;
	    while ( pPci_id[m].wf_id != 0) {
		box = (pNode->id & 0xff000000) >> 24;
		if ( (pNode->id & 0xFFFFFFFF00FFFFFF) == pPci_id[m].wf_id  ) {
		    printf("PBP%x.%-11.11s  %02.2x ", box, pPci_id[m].name, (pFru_desc->fru_desc_info.fru_diag_flag & 0xff) );
		    pFru_desc = (struct GCT_FRU_DESC_NODE *) pNode;
		    pTLV = &pFru_desc->fru_desc_info.fru_info.tlv_tag;
		    _ACCESS_TLV( pTLV, 2, pTMP );
		    FruPrintTLV("%-18.18s ", pTMP->tlv_value);
		    _ACCESS_TLV( pTLV, 3, pTMP );
		    FruPrintTLV("%-10.10s ", pTMP->tlv_value);
		    _ACCESS_TLV( pTLV, 1, pTMP );
		    FruPrintTLV("%-10.10s  ", pTMP->tlv_value);
		    _ACCESS_TLV( pTLV, 0, pTMP );
		    FruPrintTLV("%-16.16s", pTMP->tlv_value);
		    printf("\n");
		    break;
		}
		m++;
	    }


    } /* IF FRU_Desc */
  
    /* Turn off Reverse Video */
    if ( pNode->node_flags & NODE_PWR_DOWN ) printf("\x1B\x5B\x32\x37\x6D");

    if ( killpending () ) return 0;
    if (pNode->type == 1) bigloop++;

  }

  return 0;

}
#endif

/*+
 * ============================================================================
 * = Show Error - Show Error information                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>> show error
 *	>>> 
 *~
 *   COMMAND REFERENCES:
 *
 *
 * FORM OF CALL:
 *  
 *	show error(int argc, char *argv)
 *  
 * RETURN CODES:
 *
 *	msg_success - successful completion
 *	msg_failure - otherwise
 *       
 * ARGUMENTS:
 *
 *	argc - Count of the number of command line parameters
 *	argv - Array of pointers to the command line parameters
 *
 * SIDE EFFECTS:
 *
 *	Performs operations on the serial control bus.
 *
-*/
#if (!WILDFIRE)
int show_error (int argc, char *argv[]) {

  int                 j = 0, i, m = 0, status = 0, sp, config_addr =0;
  int                 flag=0, *intbuf;
  unsigned int        exp = 0, rcv = 0;
  GCT_ROOT_NODE       *pRoot;
  GCT_HANDLE          fru_root = 0, node = 0;
  GCT_NODE            *pNode, *pNext;
  GCT_FRU_DESC_NODE   *pFrudesc;
  GCT_TLV             *pTLV, *pTMP;
  FRU_NAME_LIST       *pFru_names; 
  char                *fruname;
  unsigned char       buffer[16], hd_buffer[96];
  unsigned __int64    tdd_head_value;


  pFru_names = fru_names;
  pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS( 0);
  status = gct$find_node(0,&fru_root, FIND_BY_TYPE,NODE_FRU_ROOT,  0, 0, 0, 0);
  pNode = _GCT_MAKE_ADDRESS(fru_root);
  
  pNext = pNode;
  while (pNext && (j < 2)) { 

	status = gct$find_node(0,&node, FIND_ANY, pNext , 0, 0, 0, 0);
	pNext = _GCT_MAKE_ADDRESS(node);
	pFrudesc = _GCT_MAKE_ADDRESS(node);

	/* Look only for type=15 (gct_fru_desc nodes)
	*/
	if (pNext->type == 0x15) {
		m = 0;
		while ( pFru_names[m].fru_id != 0) {
		    if (pNext->id == pFru_names[m].fru_id) {
			fruname = pFru_names[m].name;

			if ( pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_FAILURE) {
			    printf("%-14.14s Hardware Failure\n",pFru_names[m].name);
			}

			/* check checksum before displaying */
			exp = (jedec_checksum(fruname, 0x80, 0x7E) & 0xFF);
			status = read_jedec(fruname, &buffer, 1, offsetof( struct _JEDEC, jedec_b_checksum128to254), flag);
			intbuf = &buffer;
			rcv = intbuf[0] & 0xFF;
			if ( ( pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_TDD_ERR) &&  ( exp == rcv) ) {
			    status = read_jedec(fruname, &buffer, 1, offsetof( struct _JEDEC, jedec_b_tdd_log_head), flag);
			    if ( (int *)*buffer == 0 ) {}
			    else {
				intbuf = &buffer;
				printf("%-14.14s TDD - Type: %d ",pFru_names[m].name, intbuf[0] & 0x7F );
				status = read_jedec(fruname, &buffer, 1, offsetof( struct _JEDEC, jedec_b_tdd_log_head +1), flag);
				printf("Test: %d  ",(int *)*buffer);
				status = read_jedec(fruname, &buffer, 1, offsetof( struct _JEDEC, jedec_b_tdd_log_head +2), flag);
				printf("SubTest: %d  ",(int *)*buffer);
				status = read_jedec(fruname, &buffer, 1, offsetof( struct _JEDEC, jedec_b_tdd_log_head +3), flag);
				printf("Error: %d\n",(int *)*buffer);
				status = read_jedec(fruname, &hd_buffer, 16, offsetof( struct _JEDEC, jedec_b_tdd_log_data), 0);
				hd_helper(getpcb()->pcb$a_stdout, 0, hd_buffer, 16, 1, 1);
			    }
			} /* end TDD error */

			/* check checksum before displaying */
			exp = (jedec_checksum(fruname, 0x80, 0x7E) & 0xFF);
			status = read_jedec(fruname, &buffer, 1, offsetof( struct _JEDEC, jedec_b_checksum128to254), flag);
			intbuf = &buffer;
			rcv = intbuf[0] & 0xFF;
			if ( (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_SDD_ERR) && ( exp == rcv) ) {
			    status = read_jedec(fruname, &buffer, 1, offsetof( struct _JEDEC, jedec_b_sdd_log_ctrl), flag);
			    if ( (int *)*buffer == 0 ) {}
			    else {
				intbuf = &buffer;
				printf("%-14.14s SDD - Type: %d LastLog: %d  Overwrite: %d\n", pFru_names[m].name,
				    intbuf[0] & 0x0e, (intbuf[0] >> 4), (intbuf[0] >> 6));
				status = read_jedec(fruname, &hd_buffer, 88, offsetof( struct _JEDEC, jedec_b_sdd_log_block0), 0);
				hd_helper(getpcb()->pcb$a_stdout, 0, hd_buffer, 88, 1, 1);
			    }
			} /* end SDD error */

			if (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_CHKSUM_0_62) {
			    exp = (jedec_checksum(fruname, 0, 0x3E) & 0xFF);
			    status = read_jedec(fruname, &buffer, 1, offsetof( struct _JEDEC, jedec_b_checksum0to62), flag);
			    intbuf = &buffer;
			    rcv = intbuf[0] & 0xFF;
			    printf("%-14.14s Bad checksum   0 to 62  EXP:%02.2x  RCV:%02.2x\n",pFru_names[m].name,exp,rcv);
			    status = read_jedec(fruname, &hd_buffer, 64, offsetof( struct _JEDEC,jedec_b_fru_spec), 0);
			    hd_helper(getpcb()->pcb$a_stdout, 0, hd_buffer, 64, 1, 1);
			}
			if (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_CHKSUM_64_126) {
			    exp = (jedec_checksum(fruname, 0x40, 0x3E) & 0xFF);
			    status = read_jedec(fruname, &buffer, 1, offsetof( struct _JEDEC, jedec_b_checksum64to126), flag);
			    intbuf = &buffer;
			    rcv = intbuf[0] & 0xFF;
			    printf("%-14.14s Bad checksum  64 to 126  EXP:%02.2x  RCV:%02.2x\n",pFru_names[m].name,exp,rcv);
			    status = read_jedec(fruname, &hd_buffer, 64, offsetof( struct _JEDEC,jedec_b_manuf_JEDEC_ID), 0);
			    hd_helper(getpcb()->pcb$a_stdout, 0, hd_buffer, 64, 1, 1);
			}
			if (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_CHKSUM_128_254) {
			    exp = (jedec_checksum(fruname, 0x80, 0x7E) & 0xFF);
			    status = read_jedec(fruname, &buffer, 1, offsetof( struct _JEDEC, jedec_b_checksum128to254), flag);
			    intbuf = &buffer;
			    rcv = intbuf[0] & 0xFF;
			    printf("%-14.14s Bad checksum 128 to 254  EXP:%02.2x  RCV:%02.2x\n",pFru_names[m].name,exp,rcv);
			    status = read_jedec(fruname, &hd_buffer, 64, offsetof( struct _JEDEC,jedec_b_sys_serialnumb), 0);
			    hd_helper(getpcb()->pcb$a_stdout, 0, hd_buffer, 64, 1, 1);
			    status = read_jedec(fruname, &hd_buffer, 64, 0xC0, 0);
			    hd_helper(getpcb()->pcb$a_stdout, 0, hd_buffer, 64, 1, 1);
			}

			if ( pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_SYS_SERIAL_MISMATCH) {
			    printf("%-14.14s SYS_SERIAL_NUM Mismatch\n",pFru_names[m].name);
			}


		    }
		m++;
		}
	}
	if (pNext->type == 0x14) j++;
  }
    return 0;
}
#else
int show_error (int argc, char *argv[]) {
  int                      i = 0, bigloop = 0, sp, j, m, h, k, invest=0;
  int                      status, flag=0, box;
  GCT_HANDLE               root = 0, node = 0;
  GCT_NODE                 *pNode, *pHeader;
  GCT_FRU_DESC_NODE        *pFru_desc;
  struct HWRPB             *hwrpb;
  GCT_NAMES                *pNames;
  GCT_TLV                  *pTLV;
  GCT_WF_ID_LIST           *pQbb_id, *pPci_id, *pMem_id, *pDim_id, *pMisc_id, *pOdd_id;
  unsigned __int64         new_id;
  GCT_NODE_ID_FRU_DESC     *pNodeid_fru_desc;
  uint64                   buffer, length, fru_id, operational_code, stat;
  unsigned char		   *buf, *buff;

  printf("FRUname           E     Symptom\n");

  pNames = gct_type_names;
  pQbb_id = &wf_qbb_id_fru_list;
  pPci_id = &wf_pci_id_fru_list;
  pDim_id = &wf_dimm_id_fru_list;
  pOdd_id = &wf_odd_id_fru_list;
  pMisc_id = &wf_misc_id_fru_list;

  buf = hwrpb->FRU_OFFSET[0] + 0x2000;
  pHeader = (GCT_NODE *) buf;
  status = gct$find_node(0,&root, FIND_BY_TYPE, NODE_ROOT,  0, 0, 0, 0);
  pNode = _GCT_MAKE_ADDRESS(root);

  while (pNode && (bigloop < 1)) { 

    /* find the next Node */
    status = gct$find_node(0,&node, FIND_ANY, pNode, 0, 0, 0, 0);
    pNode = (struct GCT_FRU_DESC_NODE *) _GCT_MAKE_ADDRESS(node);

    /* Look only for type=15 (gct_fru_desc nodes)
    */
    if ( ( pNode->node_flags & NODE_PRESENT ) && (pNode->type == 0x15)) {

	pFru_desc = (struct GCT_FRU_DESC_NODE *) pNode;
	pTLV = &pFru_desc->fru_desc_info.fru_info.tlv_tag;


	/* Look for errors */
	if ( pFru_desc->fru_desc_info.fru_diag_flag != 0 ) {

	    /* Check for Misc Adapters */
	    m = 0;
	    while ( pMisc_id[m].wf_id != 0) {
		if ( pNode->id == pMisc_id[m].wf_id ) {
			    printf("%-16.16s  %02.2x ",pMisc_id[m].name, (pFru_desc->fru_desc_info.fru_diag_flag & 0xff) );
			    break;
		}
		m++;
	    }

	    /* Check for Dimm's */
	    for ( h = 0; h < 8; h++ ) {
		for ( j = 0; j < 4; j++ ) {
		    for ( k = 0; k < 8; k++ ) {
			new_id = pQbb_id[h].wf_id;
			pNodeid_fru_desc = &new_id;
			pNodeid_fru_desc->node_id_fru_desc_subassembly = j;
			pNodeid_fru_desc->node_id_fru_desc_slot = pDim_id[ ( j * 8 ) + k].wf_id;
			if ( pNode->id == new_id ) {
			    printf("%s.%s.%-6.6s  %02.2x ",pQbb_id[h].name,pOdd_id[ j + 4 ].name,pDim_id[k].name, 
				  (pFru_desc->fru_desc_info.fru_diag_flag & 0xff) );
			    break;
			}
		    }  /* end DImm loop */
		}  /* end Mem loop */
	    } /* end Qbb loop */


	    /* Check for DIR Dimm's & GP & etc.  */
	    for ( h = 0; h < 8; h++ ) {
		    for ( k = 0; k < 24; k++ ) {
			new_id = pQbb_id[h].wf_id;
			pNodeid_fru_desc = &new_id;
			pNodeid_fru_desc->node_id_fru_desc_slot = pOdd_id[ k].wf_id;
			if ( (k >= 8) && ( k <= 15) )
			    pNodeid_fru_desc->node_id_fru_desc_subassembly = 4;
			else
			    pNodeid_fru_desc->node_id_fru_desc_subassembly = 0xFF;

			if ( pNode->id == new_id ) {
			    if ( (k >= 8) && ( k <= 15) )
				printf("%s.DIR0.%-6.6s  %02.2x ",pQbb_id[h].name,pOdd_id[ k].name, (pFru_desc->fru_desc_info.fru_diag_flag & 0xff) );
			    else
				printf("%s.%-11.11s  %02.2x ",pQbb_id[h].name,pOdd_id[ k].name, (pFru_desc->fru_desc_info.fru_diag_flag & 0xff) );
			    break;
			}
		    }  /* end DImm loop */
	    } /* end Qbb loop */


	    /* looking for PCI - BackPlanes */
	    m = 0;
	    while ( pPci_id[m].wf_id != 0) {
		box = (pNode->id & 0xff000000) >> 24;
		if ( (pNode->id & 0xFFFFFFFF00FFFFFF) == pPci_id[m].wf_id  ) {
		    printf("PBP%x.%-6.6s       %02.2x ", box, pPci_id[m].name, (pFru_desc->fru_desc_info.fru_diag_flag & 0xff) );
		    break;
		}
		m++;
	    }


	    if ( pFru_desc->fru_desc_info.fru_diag_flag & DIAG_FLAG_FAILURE )
		printf("  Hardware Device Failure\n");

	    if ( pFru_desc->fru_desc_info.fru_diag_flag & DIAG_FLAG_TDD_ERR )  {
		printf(" TDD Error Logged\n");
		length = 0x20;
		fru_id = pFru_desc->hd_fru_desc.id >> 48;
		operational_code = 0x69;
		buff = malloc_noown(0x21);
		stat = fru_read_eerom( buff, length, fru_id, operational_code );
		hd_helper(getpcb()->pcb$a_stdout, 0, buff, 0x20, 1, 1);	
		free (buff);
	    }

	    if ( pFru_desc->fru_desc_info.fru_diag_flag & DIAG_FLAG_SDD_ERR ) {
		printf(" SDD Error Logged\n");
		length = 88;
		fru_id = pFru_desc->hd_fru_desc.id >> 48;
		operational_code = 0x19;
		buff = malloc_noown(90);
		stat = fru_read_eerom( buff, length, fru_id, operational_code );
		hd_helper(getpcb()->pcb$a_stdout, 0, buff, 0x20, 1, 1);	
		free (buff);
	    }

	    if ( pFru_desc->fru_desc_info.fru_diag_flag & DIAG_FLAG_CHKSUM_0_62 ) 
		printf(" Checksum Error between bytes 0-62\n");
	    if ( pFru_desc->fru_desc_info.fru_diag_flag & DIAG_FLAG_CHKSUM_64_126 ) 
		printf(" Checksum Error between bytes 64-126\n");
	    if ( pFru_desc->fru_desc_info.fru_diag_flag & DIAG_FLAG_CHKSUM_128_254 ) 
		printf(" Checksum Error between bytes 128-254\n");
	    if ( pFru_desc->fru_desc_info.fru_diag_flag & DIAG_FLAG_SYS_SERIAL_MISMATCH )
		printf(" SYS_SERIAL_NUM Mismatch\n");
	    

	} /* end if errors found */


    } /* IF FRU_Desc */
  
    if ( killpending () ) return 0;
    if (pNode->type == 1) bigloop++;

  }

  return 0;

}
#endif

/*+
 * ============================================================================
 * = clear_error - Clear error information in the serial control bus EEROM.   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Clear the serial control bus EEROM error log events stored in each
 *	module in the system. 
 *
 *   COMMAND FMT: clear_error 7 Z 0 clear_error
 *
 *   COMMAND FORM:
 *  
 *   	clear_error ( {all,cpu0,cpu1,io,mem0,mem1,mem2,mem3} )
 *  
 *   COMMAND TAG: clear_error 6*1024 RXBZ clear_error
 *
 *   COMMAND ARGUMENT(S):
 *   
 *	<module>... - Specifies the module(s) for which the error log is
 *		cleared.  This is a required argument, if all is specified the
 *		FRU information for all modules in the system will be cleared.
 *		Module names are cpu0, cpu1, io, mem0, mem1, mem2, and mem3.
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *                           
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>clear_error all   # Clear error information for all system modules.
 *	>>>
 *	>>>clear_error mem3  # Clear error information for a memory module.
 *	>>> 
 *~
 *   COMMAND REFERENCES:
 *
 *	build, show error, show fru
 *
 * FORM OF CALL:
 *  
 *  clear_error(int argc, char *argv)
 *  
 * RETURN CODES:
 *
 *  msg_success - successful completion
 *  msg_failure - otherwise
 *       
 * ARGUMENTS:
 *
 *  argc - Count of the number of command line parameters
 *  argv - Array of pointers to the command line parameters
 *
 * SIDE EFFECTS:
 *
 *  Performs operations on the serial control bus.
 *
-*/
#if (!WILDFIRE)
int clear_error (int argc, char *argv[]) {
    struct QSTRUCT          qual[QMAX];
    int			    i, j=0, k=3, p = 0, q = 0, t = 0, status;
    int                     localOffset, flag=0, vflag=0, convert, addIndex=0;
    int			    supports_iic = 0, m = 0;
    unsigned int            exp = 0;
    unsigned int            return_checksum=0;
    FRU_NAME_LIST           *pFru_names;
    GCT_ROOT_NODE           *pRoot;
    GCT_HANDLE              fru_root = 0, node = 0;
    GCT_NODE                *pNode, *pNext;
    GCT_FRU_DESC_NODE       *pFrudesc;
    char                    *fruname;
    struct EVNODE           evn, *evp=&evn;
    unsigned __int64        nodeid, *pNodeid;


    if (argc < 1) {
	printf("not enough arguments\n");
	return msg_failure;
    }

    status = qscan( &argc, argv, "-", "s e a v o", qual );
    if( status != msg_success )	{
	err_printf( status );
	return( status );
    }

    if (qual[QV].present) vflag = 1;
    if (qual[QO].present) flag = 1;

    for (i=0; argv[1][i]=toupper(argv[1][i]); i++);	/* Ensure its uppercase*/
    pFru_names   =  fru_names;

    if ( strcmp( argv[1], "ALL") == 0 ) {
	pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS( 0);
	status = gct$find_node( 0, &fru_root, FIND_BY_TYPE, NODE_FRU_ROOT, 0, 0, 0, 0 );
	pNode = _GCT_MAKE_ADDRESS(fru_root);

	pNext = pNode;
	while (pNext && (j < 2)) { 

	    status   = gct$find_node( 0, &node, FIND_ANY, pNext, 0, 0, 0, 0 );
	    pNext    = _GCT_MAKE_ADDRESS(node);
	    pFrudesc = _GCT_MAKE_ADDRESS(node);

	    if (pNext->type == 0x15) {
		m = 0;
		while ( pFru_names[m].fru_id != 0) {

		    /* Look for match in list */
		    if (pNext->id == pFru_names[m].fru_id) {
			fruname = pFru_names[m].name;
			if (vflag) printf("FRU Identifier... %s\n",fruname);

			/*
			 * Check for TDD and SDD error bits
			 */
			if (vflag) printf("\tchk TDD & SDD...\n");
			if ( (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_TDD_ERR) ||
			     (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_SDD_ERR) ) {

			    if (vflag) printf("\tFixing TDD & SDD...\n");
			    for (k=144; k< 253; k++) {
				localOffset = k;
				status = write_jedec(fruname, 0, localOffset, 1, flag);
			    }
			    exp = (jedec_checksum(fruname, 0x80, 0x7E) & 0xFF);
			    status = write_jedec(fruname, exp, offsetof(struct _JEDEC, jedec_b_checksum128to254), 1, flag);
			}

			/*
			 * Check for first checksum error bit
			 */
			if (vflag) printf("\tchk chksum #1...\n");
			if (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_CHKSUM_0_62) {
			    if (vflag) printf("\tFixing Chksum 0to62...\n");
			    exp = (jedec_checksum(fruname, 0, 0x3E) & 0xFF);
			    status = write_jedec(fruname, exp, offsetof(struct _JEDEC, jedec_b_checksum0to62), 1, flag);
			}

			/*
			 * Check for second checksum error bit
			 */
			if (vflag) printf("\tchk chksum #2...\n");
			if (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_CHKSUM_64_126) {
			    if (vflag) printf("\tFixing Chksum 64to126...\n");
			    exp = (jedec_checksum(fruname, 0x40, 0x3E) & 0xFF);
			    status = write_jedec(fruname, exp, offsetof(struct _JEDEC, jedec_b_checksum64to126), 1, flag);
			}

			/*
			 * Check for third checksum error bit
			 */
			if (vflag) printf("\tchk chksum #3...\n");
			if (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_CHKSUM_128_254) {
			    if (vflag) printf("\tFixing Chksum 128to254...\n");
			    exp = (jedec_checksum(fruname, 0x80, 0x7E) & 0xFF);
			    status = write_jedec(fruname, exp, offsetof(struct _JEDEC, jedec_b_checksum128to254), 1, flag);
			}

			/*
			 * Check for a system serial number mismatch 
			 */
			if (vflag) printf("\tchk sys serial...\n");
			if (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_SYS_SERIAL_MISMATCH) {
			    if (vflag) printf("\tFixing SysSerialNum...\n");
			    write_jedec(fruname, 00, offsetof(struct _JEDEC, jedec_b_sys_serialnumb), 1, flag);
			    exp = (jedec_checksum(fruname, 0x80, 0x7E) & 0xFF);
			    status = write_jedec(fruname, exp, offsetof(struct _JEDEC, jedec_b_checksum128to254), 1, flag);
			}

		    }
		    m++;
		}
	    }
	    if (pNext->type == 0x14) j++;

	} /* end While */

	clear_platform_errors();

    }
    else {

	/* First line of defense
	*  Check to see if device supports EEPROMS
	*  The command line specified a FRUname
	*/
	supports_iic = support_iic_fru(argv[1], pFru_names);
	if (supports_iic == -1) {
 	    printf("could not find FRU Identifier %s\n",argv[1]);
	    return msg_failure;
	}

	nodeid = find_fru_id( argv[1], pFru_names);
	pNodeid = &nodeid;
	status   = gct$find_node(0,&node, FIND_BY_ID, NODE_FRU_DESC, 0, nodeid, 0, 0);
	pFrudesc = _GCT_MAKE_ADDRESS(node);

	if (vflag) printf("FRU Identifier... %s\n",argv[1]);

	/*
	 * Check for TDD and SDD error bits
	 */
	if ( (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_TDD_ERR) ||
	     (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_SDD_ERR) ) {

	    if (vflag) printf("\tFixing TDD & SDD...\n");
	    for (j=144; j< 253; j++) {
		localOffset = j;
		status = write_jedec(argv[1], 0, localOffset, 1, flag);
	    }
	    exp = (jedec_checksum(argv[1], 0x80, 0x7E) & 0xFF);
	    status = write_jedec(argv[1], exp, offsetof(struct _JEDEC, jedec_b_checksum128to254), 1, flag);
	}

	/*
	 * Check for first checksum error bit
	 */
	if (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_CHKSUM_0_62) {
	    if (vflag) printf("\tFixing Chksum 0to62...\n");
	    exp = (jedec_checksum(argv[1], 0, 0x3E) & 0xFF);
	    status = write_jedec(argv[1], exp, offsetof(struct _JEDEC, jedec_b_checksum0to62), 1, flag);
	}

	/*
	 * Check for second checksum error bit
	 */
	if (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_CHKSUM_64_126) {
	    if (vflag) printf("\tFixing Chksum 64to126...\n");
	    exp = (jedec_checksum(argv[1], 0x40, 0x3E) & 0xFF);
	    status = write_jedec(argv[1], exp, offsetof(struct _JEDEC, jedec_b_checksum64to126), 1, flag);
	}

	/*
	 * Check for third checksum error bit
	 */
	if (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_CHKSUM_128_254) {
	    if (vflag) printf("\tFixing Chksum 128to254...\n");
	    exp = (jedec_checksum(argv[1], 0x80, 0x7E) & 0xFF);
	    status = write_jedec(argv[1], exp, offsetof(struct _JEDEC, jedec_b_checksum128to254), 1, flag);
	}

	/*
	 * Check for a system serial number mismatch 
	 */
	if (pFrudesc->fru_desc_info.fru_diag_flag & DIAG_FLAG_SYS_SERIAL_MISMATCH) {
	    if (vflag) printf("\tFixing SysSerialNum...\n");
	    write_jedec(argv[1], 00, offsetof(struct _JEDEC, jedec_b_sys_serialnumb), 1, flag);
	    exp = (jedec_checksum(argv[1], 0x80, 0x7E) & 0xFF);
	    status = write_jedec(argv[1], exp, offsetof(struct _JEDEC, jedec_b_checksum128to254), 1, flag);
	}

	clear_platform_errors();

    }

    /*
    *   Call GCT to rebuild the config tree (because of this cmd)
    */
    gct();

    return msg_success;
 }
#else
int clear_error (int argc, char *argv[]) {
    printf("Perform command at the SCM prompt\n");
    return msg_success;
}
#endif

/*
 *  Generic JEDEC checksum
 * 
 * Checksum is calculated using the algorithm described in JEDEC Standard 21-C
 * on page 4.1-2-2 paragraph 4.3.  "Sum the binary values of the bytes covered
 * by the checksum and eliminate all but the lower order byte"
 *
*/

int jedec_checksum( char *fruname, int start, int length) { 

    unsigned int            i, status, flag=0, sum = 0;
    char                    buffer = {0};

    if (length == 0) {
	printf("BAD Length value for checksum\n");
	return msg_failure;
    }

    for (i=0; i<length+1; i++) {
	status = read_jedec(fruname, &buffer, 1, start+i, flag);
	sum += (unsigned int)buffer;
    }
    return sum;
}



/*+
 * ============================================================================
 * = find_fru_id - Search the input string array for the input string        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  If found the ID of the matching string is returned. if not found an error
 *  message is displaied and  and a -1 is returned. 
 *
 *  The match is required up to the number of characters in the input list 
 *  entry.
 *
 * FORM OF CALL:
 *  
 *  find_fru_id(char *string, char *list[])
 *  
 * RETURNS:
 *
 *  index of matching string if uniquely found -1 otherwise
 *       
 * ARGUMENTS:
 *
 *  string - pointer to the token to be matched
 *  list   - array of string pointers across which to match string
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

unsigned __int64 find_fru_id(char *string, struct _fru_name_list list[]) { 

  int                 i, m=0;
  unsigned __int64    status = -1;
  char                fruname[20];
  char                stringname[20];

  if( strlen(string) == 0)
        return (-1);

  sprintf (fruname, "%s", string);
  for (i=0; fruname[i]=toupper(fruname[i]); i++);  /* UpperCase */

   while ( list[m].fru_id != 0) {
    if (!strcmp (fruname, list[m].name)) {
	status = list[m].fru_id;
	break;
    }
   m++;
  }

  return (status);

}  /* end  */


/*+
 * ============================================================================
 * = find_fru_name - Search the input string array for the input string       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  If found, the NAME of the matching string is returned. if not found an error
 *  message is displaied and  and a -1 is returned. 
 *
 *  The match is required up to the number of characters in the input list 
 *  entry.
 *
 * FORM OF CALL:
 *  
 *  find_fru_name(char *string, char *list[])
 *  
 * RETURNS:
 *
 *  index of matching string if uniquely found -1 otherwise
 *       
 * ARGUMENTS:
 *
 *  string - pointer to the token to be matched
 *  list   - array of string pointers across which to match string
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/
void find_fru_name(unsigned __int64 nodeid, struct _fru_name_list list[], char *fruname) { 

  int                 m=0;

   while ( list[m].fru_id != 0) {
	if ( nodeid == list[m].fru_id) {
	    sprintf(fruname,"%s", list[m].name);
	    break;
	}
	m++;
   }

}  /* end  */


/*+
 * ============================================================================
 * = support_iic_fru - Search the input string for iic support                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  find_name preforms a case independent search of the input string
 *  If Support is found, a 1 is returned. if not found an error
 * message is displaied and  and a -1 is returned. 
 *
 *
 * FORM OF CALL:
 *  
 *  support_iic_fru(char *string, char *list[])
 *  
 * RETURNS:
 *
 *  value if string supports IIC addresses, otherwise -1
 *       
 * ARGUMENTS:
 *
 *  string - pointer to the token to be matched
 *  list   - array of string pointers across which to match string
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

unsigned int support_iic_fru( char *string, struct _fru_name_list list[]) { 

  int                 i, m = 0;
  unsigned __int64    status = -1;
  char                fruname[20];
  char                stringname[20];

  if( strlen(string) == 0)
        return (-1);

  sprintf (fruname, "%s", string);
  for (i=0; fruname[i]=toupper(fruname[i]); i++);  /* UpperCase */

   while ( list[m].fru_id != 0) {
    if (!strcmp (fruname, list[m].name)) {
	status = list[m].iic_support;
	break;
    }
   m++;
  }

  return (status);
}  /* end  */


/*+
 * ============================================================================
 * = FruPrintTLV - Routine to use ISPRINT during TLV dumping                  =
 * ============================================================================
 *
 *  
 */
void FruPrintTLV(char *a, char *c) {
    char  buffer[80];
    int   i;

    for (i=0; c[i] != 0; i++) {
	if ( isprint( c[i] ) )
	    buffer[i] = c[i];
	else
	    buffer[i] = '?';
    }
    buffer[i] = 0;
    printf(a,buffer);
}


/*+
 * ============================================================================
 * = find_pci_vpd - Find the pci vpd information		              =
 * ============================================================================
 *
 * OVERVIEW:         
 *
 *	This function will find VPD information and return it in a pointer
 *	to that infoormation. The size of this information being the first 
 *	longword in front of that data.
 *  
 *  
 * FORM OF CALL:
 *
 *	find_pci_vpd( pb )
 *
 * RETURN CODES:
 *
 *      0    - Indicating no VPD information. 
 *	addr - Pointer to VPD information malloced in memory.
 *
 * ARGUMENTS:
 *
 *	struct	pb *pb	- Port block structure.
 *                     
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

char *find_pci_vpd(struct pb *pb)
{
    int i, j, k, m, n;
    int er_offset = 0x30;
    int er_base;
    int last_pcir = 0;
    char *vpd_data;
    int vpd_size = 0;

    /* Expansion rom register offset is different on PCI to PCI bridges. */

    if (incfgw(pb, 0x0a) == 0x0604)
	er_offset = 0x38;

    /* Preallocate an area of VPD data. Make realloc work better. */

    vpd_data = (char *) malloc_noown(32);

    /*
     * If we have an expansion rom.
     */
    if ((er_base = incfgl(pb, er_offset)) != 0) {
	outcfgl(pb, er_offset, er_base | 1);	/* enable it. */
	/*
	 * Check for valid expansion rom header...
	 */
	if ((inmemw(pb, er_base) == 0xAA55) && 
	  (inmemw(pb, er_base + 0x18) % 4 == 0)) {
	    i = inmemw(pb, er_base + 0x18);
	    while (!last_pcir) {

		/* if PCIR */

		if (inmeml(pb, i + er_base) == 0x52494350) {
		    /*
		     * Process some VPD data if we got some...
		     */
		    if (j = inmemw(pb, i + er_base + 8)) {
			for (k = 0; k != 0x79;) {
			    k = inmemb(pb, j + er_base);
			    if (k == 0x82 || k == 0x90) {
				n = (inmemb(pb, j + 1 + er_base) | 
				  (inmemb(pb, j + 2 + er_base) << 8)) + 3;
				if (k == 0x90) {
				    vpd_data = (char *) dyn$_realloc(vpd_data,
				      vpd_size + n + 4,
				      DYN$K_SYNC | DYN$K_NOOWN);
				    for (m = 0; m < n; m++) {
					vpd_data[4 + vpd_size + m] = 
					  inmemb(pb, j + m + er_base);
				    }
				    vpd_size += n;
				}
				j += n;
			    } else { 
				/* handle the case of a bad vpd area. */
				if (k != 0x79)
				    k = 0x79;
			    }
			}
		    }

		    /* if not last PCIR */

		    if (inmemb(pb, i + er_base + 0x15) & 0x80)
			last_pcir = 1;
		    else {
			j = inmemw(pb, er_base + i + 0x10) * 0x200;
			i = j;
		    }
		} else
		    last_pcir = 1;
	    }
	}
	outcfgl(pb, er_offset, er_base);	/* disable it. */
    }
    if (vpd_size)
	*(int *) vpd_data = vpd_size;
    else {
	free(vpd_data);
	vpd_data = (char *) 0;
    }
    return (vpd_data);
}


/* 
 * ============================================================================
 *   Probe PCI device for VPD data
 * ============================================================================
 *
 */
void fru5_pci_vpd( int hose ) {

    int                  i, j, n, slot, function, max_function, cur_ind=0;
    struct pb            *pb;
    char                 *vpd_data;
    GCT_TLV              *pTLV;
    struct VPDDATA       *v;

    if ( hose == 0) {
	for( slot = 0; slot <= 5; slot++ ) { 
	    max_function = 0;
	    for( function = 0; function <= 7; function++ ) {
		pb = get_matching_pb( hose, 0, slot, function, 0 );
		if( pb ) {
		    if( pb->type == TYPE_PCI ) {
			vpd_data = (char *) find_pci_vpd( pb );
			if (vpd_data) {
			    v = malloc_noown( sizeof (struct VPDDATA) );
			    v->adr = vpd_data;
			    v->hose = hose;
			    v->slot = slot;
			    insq( &v->q.flink, vpd_data_q.blink );
			}
		    } /* end if PCI */
		}
	    }
	}
    } /* end if first bus */

    if ( hose == 1) {
	for( slot = 0; slot <= 6; slot++ ) { 
	    max_function = 0;
	    for( function = 0; function <= 7; function++ ) {
 		pb = get_matching_pb( hose, 0, slot, function, 0 );
		if( pb ) {
		    if( pb->type == TYPE_PCI ) {
			vpd_data = (char *) find_pci_vpd( pb );
			if (vpd_data) {
			    v = malloc_noown( sizeof (struct VPDDATA) );
			    v->adr = vpd_data;
			    v->hose = hose;
			    v->slot = slot;
			    insq( &v->q.flink, vpd_data_q.blink );
			}
		    } /* end if PCI */
		}
	    }
	}
    } /* end if second bus */

    /* vpd_data_dump_queue(); */
} /* End PCI VPD */


void vpd_data_dump_queue() {
    struct VPDDATA *v;
    v = vpd_data_q.flink;
    while (v != &vpd_data_q.flink) {
	pprintf("element (%x) hose (%x) slot (%x) found\n",v->adr,v->hose,v->slot);
	v = v->q.flink;
    }
}

/* 
 *
 * Return the pointer to the buffer which matches the hose and slot 
 *
 */
char *fru5_get_vpd_data( int hose, int slot) {
    int found = 0;
    char *buffaddr;
    struct VPDDATA *v;

    v = vpd_data_q.flink;

    while ( v != &vpd_data_q.flink) {
	if (( hose == v->hose ) && ( slot == v->slot )) {
	    remq( v );
	    found = 1;
	    break;
	} else {
 	    v = v->q.flink;
	}
    }
    if (found) {
	/* pprintf("(%x) element removed from queue hose %x slot %x\n", v->adr, v->hose, v->slot); */
	buffaddr = v->adr;
	free( v );
	return (buffaddr);
    }
    return (0);
}

char *fru5_vpd_data_tlv( char *vpd_data, int tlv_num) {
    int             i, j, vpd_size, startpoint = 0, size, rippoint;
    int             found_ec = 0, found_fn = 0;
    char            temp_buff[126], buffer[32], *buffwalk;
    char            fn_buff[20];
    char            ec_buff[5];


    /*  There are only 5 TLVs, if passed the 6th
     *  then FREE the malloc 
     */
    if (tlv_num == 6) {
	free(vpd_data);
	return (0);
    }

    vpd_size = *(int *) vpd_data;
    memcpy(temp_buff, &vpd_data[4], vpd_size);
    buffwalk = &temp_buff;

    for ( i = 0; i < vpd_size; i++ ) {

	rippoint = startpoint + i + 3;
	size = buffwalk[rippoint - 1];

	switch ( buffwalk[rippoint] ) {
	    case 'M':
		if ( buffwalk[rippoint + 1] == 'N') {
		    if (tlv_num == 1) {
			for (j=0; j< buffwalk[rippoint + 2]; j++)
			    buffer[j] = buffwalk[rippoint + 3 + j];
			buffer[j] = 0;
		    }
		break;
		}
	    case 'P':
		if ( buffwalk[rippoint + 1] == 'N') {
		    if (tlv_num == 2){
			for (j=0; j< buffwalk[rippoint + 2]; j++) 
			    buffer[j] = buffwalk[rippoint + 3 + j];
			buffer[j] = 0;
		    }
		break;
		}
	    case 'F':
		if ( buffwalk[rippoint + 1] == 'N') {
		    found_fn = 1;
		    if (tlv_num == 3) {
			for (j=0; j< buffwalk[rippoint + 2]; j++)
			    buffer[j] = buffwalk[rippoint + 3 + j];
			buffer[j] = 0;
		    }
		break;
		}
	    case 'S':
		if ( buffwalk[rippoint + 1] == 'N') {
		    if (tlv_num == 4) {
			for (j=0; j< buffwalk[rippoint + 2]; j++)
			    buffer[j] = buffwalk[rippoint + 3 + j];
			buffer[j] = 0;
		    }
		break;
		}
	    case 'R':
		if ( buffwalk[rippoint + 1] == 'M') {
		    if (tlv_num == 5) {
			for (j=0; j< buffwalk[rippoint + 2]; j++)
			    buffer[j] = buffwalk[rippoint + 3 + j];
			buffer[j] = 0;
		    }
		break;
		}
	    case 'E':
		if ( buffwalk[rippoint + 1] == 'C') {
		    found_ec = 1;
		    for (j=0; j< buffwalk[rippoint + 2]; j++)
			ec_buff[j] = buffwalk[rippoint + 3 + j];
		    ec_buff[j] = 0;
		    break;
		}
	}
    } /* end for size */


    /* Special Case 
     * Add EC to FN string 
     */
    /* TLV 3, w/ EC w/ FN, so append EC to FN */
    if ( (tlv_num == 3) && (found_ec) && (found_fn) ) {	
	sprintf(fn_buff, "%s:", buffer);
	strcat(fn_buff, ec_buff);
	strcpy(buffer, fn_buff);
    }

    /* TLV 2, w/ EC but no FN, so append EC to PN */
    if ( (tlv_num == 2) && (found_ec) && (!found_fn) ) {
	sprintf(fn_buff, "%s:", buffer);
	strcat(fn_buff, ec_buff);
	strcpy(buffer, fn_buff);
    }

    /*  There are only 5 TLVs, if passed the 6th
     *  then FREE the malloc 
     */
    if (tlv_num == 6)
	free(vpd_data);

    return (buffer);

} /* end fru5_vpd_data_tlv */

