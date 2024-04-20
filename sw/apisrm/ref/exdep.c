/* file:	exdep.c
 *
 * Copyright (C) 1990 by
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
 *
 * Abstract:	Examine/deposit commands and support routines.
 *
 * Authors:	Console Firmware Team
 *
 * Modifications:
 *
 *	jrk	10-Apr-95	Remove old history and Cobra
 */

#include    "cp$src:platform.h"
#include    "cp$src:kernel_def.h"	/* Kernel */
#include    "cp$src:aprdef.h"		/* Alpha IPR defs */
#include    "cp$src:common.h"		/* Common defs */
#include    "cp$src:dynamic_def.h"	/* Dynamic memory */
#include    "cp$src:parse_def.h"	/* Parser */
#include    "cp$src:msg_def.h"		/* Message Facility */
#include    "cp$src:ev_def.h"		/* Environment Variables */
#include    "cp$src:exdep_def.h"	/* Examine/Deposit */
#include    "cp$src:ctype.h"
#include    "cp$src:alpha_def.h"	/* Alpha architecture defs */
#include    "cp$src:alphaps_def.h"	/* Alpha Processor Status */
#include "cp$inc:kernel_entry.h"

#define	MAX_QUALS	14
/* 
 * Examine/Deposit data structure.  Most fields in this structure are 64 bits
 * to allow for devices that need more than 32 bits worth of address, such
 * vmem_driver and large disks.  Care should be taken to inusure that the
 * data field is aligned on a quadword boundary.
 */
struct exdep_struct {
	INT		address;	/* 64 bit address			*/
	INT		data_index;	/* index over multiple datums on deposit*/
	INT		data_count;	/* number of datums on command line	*/
	INT		range_count;	/* Number of succeeding addresses to operate on */
	INT		incsize;	/* Increment size               	*/
	INT		datatype;	/* Data type (byte, word, long etc).	*/
	INT		examine;	/* Set for examine command      	*/
	INT		require_ftell;	/* do an ftell to get the address	*/
	INT		value;		/* last datum				*/
	INT		adqual;		/* address qualifier (if present)	*/
	unsigned char	data [256];	/* Data to deposit (written by EXAMINE)	*/
	struct FILE	*fp;		/* file that is accessed		*/
	unsigned char	space [MAX_NAME]; /* Address space (driver name)	*/
	struct QSTRUCT	qs[MAX_QUALS];	/* Qualifier structure for QSCAN 	*/
};

#include "cp$inc:prototypes.h"

#define DEBUG 0

/* Declare externally-defined functions: */

extern exdep_handle_exception(void);
extern INT ftell ();
extern int	exdep_inited;		/* exdep_init already run flag */

/* 
 * Examine/Deposit command qualifiers, also used to index into the data
 * length field.  Data width qualifiers should be listed first, as these
 * are used to index other data structures.
 */
#define	QUAL_BYTE	0	/* Data type = byte		*/
#define	QUAL_WORD	1	/* Data type = word		*/
#define	QUAL_LONGWORD	2	/* Data type = longword		*/
#define	QUAL_QUADWORD	3	/* Data type = quadword		*/
#define	QUAL_OCTAWORD	4	/* Data type = octaword		*/
#define	QUAL_HEXAWORD	5	/* Data type = hexaword		*/
#define	QUAL_DECODE	6	/* Data type = instruction	*/
#define	QUAL_N		7	/* Address range specifier	*/
#define	QUAL_STEPSIZE	8	/* step size                    */
#define	QUAL_PHYSICAL	9	/* physical access		*/
#define	QUAL_VIRTUAL    10	/* virtual access		*/
#define	QUAL_GPR	11	/* general purpose registers	*/
#define	QUAL_FPR	12	/* floating point registers	*/
#define	QUAL_IPR	13	/* internal processor registers */

#define	MIN_TYPE	QUAL_BYTE	/* range for data types */
#define	MAX_TYPE	QUAL_DECODE

#define	MIN_ADDRESS	QUAL_PHYSICAL	/* range for address specifiers */
#define MAX_ADDRESS	QUAL_IPR

/* 
 * QSCAN qualifiers.
 */
#define	QSCAN_QUAL_STR  "b w l q o h d %xn %xs physical virtual gpr fpr ipr"

/*
 * Table of filenames that are associated with address specifiers.
 */
char *address_specifiers [MAX_ADDRESS - MIN_ADDRESS + 1] = {
	"pmem",		/* QUAL_PHYSICAL	*/
	"vmem",		/* QUAL_VIRTUAL 	*/
	"gpr",		/* QUAL_GPR		*/
	"fpr",		/* QUAL_FPR		*/
	"ipr",		/* QUAL_IPR		*/
};

/*
 * Tables of data type sizes, indexed by data type or qualifier index
 */
int datalengths [] = {
	1,		/* byte		*/
	2,		/* word		*/
	4,		/* longword	*/
	8,		/* quadword	*/
	16,		/* octaword	*/
	32,		/* hexaword	*/
	4,		/* decode	*/
};

/* 
 * Examine/deposit environment variables:
 */
char	ev_exdep_location []	= "exdep_location";	/* Last location		*/
char	ev_exdep_space []	= "exdep_space";	/* Last address space		*/
char	ev_exdep_type []	= "exdep_type";		/* Last data type		*/
char	ev_exdep_size []	= "exdep_size";		/* Last data size		*/
char	ev_exdep_data []	= "exdep_data";		/* Last data value		*/

/* 
 * Names of examine/deposit drivers that are needed for the symbol table
 */
char	pmem_driver	[] = "pmem"	    ;/* Physical memory			*/
char	vmem_driver	[] = "vmem"	    ;/* Virtual memory			*/
char	gbus_driver	[] = "gbus"	    ;/* Laser GBUS			*/
char	gpr_driver	[] = "gpr"	    ;/* General Purpose Registers	*/
char	fpr_driver	[] = "fpr"	    ;/* Floating-Point Registers	*/
char	ipr_driver	[] = "ipr"	    ;/* Internal Processor Registers	*/
char	pt_driver	[] = "pt"	    ;/* PAL Temporary Registers		*/
char	psr_driver	[] = "psr"	    ;/* Processor Status Registers	*/


/*
 * The maximum number of datums allowed on command line is limited by the
 * number of hexawords that can fit in the data buffer.
 */
#define	MAX_DATACOUNT (256/32)


/*
 * The following symbol table associates strings with a filename and an
 * offset so that users may say things such as "examine PC" and not have to
 * know about drivers, protocol towers etc.
 *
 * Offsets are always encoded as byte offsets into the driver, even though
 * the data that the driver supports may have a natural size of 4 or 8 (as
 * in the case of register drivers).  Thus, register 3 has an
 * offset of 24 or 12 into the gpr driver on ALPHA and VAX respectively.
 */
static struct exdep_symbol_table_struct {		/* Define the symbol table */
    char *symbol;
    char *space;
    INT location;
} exdep_symbol_table[] = {

#if SABLE*0

#define NUM_SIO_HACKS ( NUM_T2 + NUM_PCEB + NUM_TULIP + NUM_SCSI )  
/*
** T2 registers
*/ 

#define NUM_T2 26
    {	"iocsr",	pmem_driver,	0x8E000000 +   0 * 32},
    {	"cerr1",	pmem_driver,	0x8E000000 +   1 * 32},
    {	"cerr2",	pmem_driver,	0x8E000000 +   2 * 32},
    {	"cerr3",	pmem_driver,	0x8E000000 +   3 * 32},
    {	"PERR1",	pmem_driver,	0x8E000000 +   4 * 32},
    {	"PERR2",	pmem_driver,	0x8E000000 +   5 * 32},
    {	"PSCR",		pmem_driver,	0x8E000000 +   6 * 32},
    {	"HAEO_1",	pmem_driver,	0x8E000000 +   7 * 32},
    {	"HAEO_2",	pmem_driver,	0x8E000000 +   8 * 32},
    {	"HBASE",	pmem_driver,	0x8E000000 +   9 * 32},
    {	"WBASE1",	pmem_driver,	0x8E000000 +  10 * 32},
    {	"WMASK1",	pmem_driver,	0x8E000000 +  11 * 32},
    {	"TBASE1",	pmem_driver,	0x8E000000 +  12 * 32},
    {	"WBASE2",	pmem_driver,	0x8E000000 +  13 * 32},
    {	"WMASK2",	pmem_driver,	0x8E000000 +  14 * 32},
    {	"TBASE2",	pmem_driver,	0x8E000000 +  15 * 32},
    {	"TLBBR",	pmem_driver,	0x8E000000 +  16 * 32},
    {	"IVRPR",	pmem_driver,	0x8E000000 +  17 * 32},
    {	"TDR0",		pmem_driver,	0x8E000300 },
    {	"TDR1",		pmem_driver,	0x8E000320 },
    {	"TDR2",		pmem_driver,	0x8E000340 },
    {	"TDR3",		pmem_driver,	0x8E000360 },
    {	"TDR4",		pmem_driver,	0x8E000380 },
    {	"TDR5",		pmem_driver,	0x8E0003a0 },
    {	"TDR6",		pmem_driver,	0x8E0003c0 },
    {	"TDR7",		pmem_driver,	0x8E0003e0 },
    {	"IVR", 		pmem_driver,	0xA0000480 },
/* 
** PCEB registers
*/
#define NUM_PCEB 40
    {	"VENID", 	pmem_driver,	0x90040008 },
    {	"DEVID", 	pmem_driver,	0x90040048 },
    {	"PCICMD",	pmem_driver,	0x90040088 },
    {	"PCISTS", 	pmem_driver,	0x900400C8 },
    {	"REVID", 	pmem_driver,	0x90040100 },
    {	"MLTIM", 	pmem_driver,	0x900401A0 },
    {	"PCICON", 	pmem_driver,	0x90040800 },
    {	"ARBCON", 	pmem_driver,	0x90040820 },
    {	"ARBPRI", 	pmem_driver,	0x90040840 },
    {	"MCSCON", 	pmem_driver,	0x90040860 },
    {	"MCSBOH", 	pmem_driver,	0x90040880 },
    {	"MCSBOH", 	pmem_driver,	0x900408A0 },
    {	"MCSTOM", 	pmem_driver,	0x900408C0 },
    {	"EADCIM", 	pmem_driver,	0x90040908 },
    {	"IORTC", 	pmem_driver,	0x90040980 },
    {	"MAR1", 	pmem_driver,	0x90040A80 },
    {	"MAR2", 	pmem_driver,	0x90040AA0 },
    {	"MAR3", 	pmem_driver,	0x90040AC0 },
    {	"PDCON", 	pmem_driver,	0x90040B00 },
    {	"EADCX", 	pmem_driver,	0x90040B20 },
    {	"EPMRA", 	pmem_driver,	0x90040B80 },
    {	"EPMB1", 	pmem_driver,	0x90040C08 },
    {	"EPML1", 	pmem_driver,	0x90040C48 },
    {	"EPMB2", 	pmem_driver,	0x90040C88 },
    {	"EPML2", 	pmem_driver,	0x90040CC8 },
    {	"EPMB3", 	pmem_driver,	0x90040D08 },
    {	"EPML3", 	pmem_driver,	0x90040D48 },
    {	"EPMB4", 	pmem_driver,	0x90040D88 },
    {	"EPML5", 	pmem_driver,	0x90040DC8 },
    {	"EPIOB1", 	pmem_driver,	0x90040E08 },
    {	"EPIOL1", 	pmem_driver,	0x90040E48 },
    {	"EPIOB2", 	pmem_driver,	0x90040E88 },
    {	"EPIOL2", 	pmem_driver,	0x90040EC8 },
    {	"EPIOB3", 	pmem_driver,	0x90040F08 },
    {	"EPIOL3", 	pmem_driver,	0x90040F48 },
    {	"EPIOB4", 	pmem_driver,	0x90040F88 },
    {	"EPIOL5", 	pmem_driver,	0x90040FC8 },
    {	"BTMR", 	pmem_driver,	0x90041008 },
    {	"ELTCR", 	pmem_driver,	0x90041080 },
    {	"PTCR", 	pmem_driver,	0x9004118  },

/*
** TULIP REGISTERS
*/
#define NUM_TULIP 5+16+14+8+1+2

/* TULIP CONFIGURATION REGISTERS                         (5) */
    {	"CFID", 	pmem_driver,	0x90010018  },   
    {	"CFCS", 	pmem_driver,	0x90010098  },
    {	"CFRV", 	pmem_driver,	0x90010118  },
    {	"CFLT", 	pmem_driver,	0x90010198  },
    {	"CBIO", 	pmem_driver,	0x90010218  },

/* TULIP REGISTERS                                       (16) */ 
    {	"CSR0", 	pmem_driver,	0xA0140018  },   
    {	"CSR1", 	pmem_driver,	0xA0140118  },
    {	"CSR2", 	pmem_driver,	0xA0140218  },
    {	"CSR3", 	pmem_driver,	0xA0140318  },
    {	"CSR4", 	pmem_driver,	0xA0140418  },
    {	"CSR5", 	pmem_driver,	0xA0140518  },
    {	"CSR6", 	pmem_driver,	0xA0140618  },
    {	"CSR7", 	pmem_driver,	0xA0140718  },
    {	"CSR8", 	pmem_driver,	0xA0140818  },
    {	"CSR9", 	pmem_driver,	0xA0140918  },
    {	"CSR10", 	pmem_driver,	0xA0140A18  },
    {	"CSR11", 	pmem_driver,	0xA0140B18  },
    {	"CSR12", 	pmem_driver,	0xA0140C18  },
    {	"CSR13", 	pmem_driver,	0xA0140D18  },
    {	"CSR14", 	pmem_driver,	0xA0140E18  },
    {	"CSR15", 	pmem_driver,	0xA0140F18  },
/* INTERNAL DIAGNOSTIC I/O ADDRESS SPACE ( DMA BLOCK )   (14)*/
    {	"TDES1_ADD", 	pmem_driver,	0xA0141118  },
    {	"TDES2_ADD", 	pmem_driver,	0xA0141218  },
    {	"TDES3_ADD", 	pmem_driver,	0xA0141318  },
    {	"TDES_BUF1_ADD",pmem_driver,	0xA0141418  },
    {	"TDES_BUF2_ADD",pmem_driver,	0xA0141518  },
    {	"RDES1_ADD", 	pmem_driver,	0xA0141718  },
    {	"RDES2_ADD", 	pmem_driver,	0xA0141818  },
    {	"RDES3_ADD", 	pmem_driver,	0xA0141918  },
    {	"RDES_BUF1_ADD",pmem_driver,	0xA0141A18  },
    {	"RDES_BUF2_ADD",pmem_driver,	0xA0141B18  },
    {	"TDES_STAT", 	pmem_driver,	0xA0141C18  },
    {	"TDES_COMM", 	pmem_driver,	0xA0141D18  },
    {	"RDES_STAT_OFF",pmem_driver,	0xA0141E18  },
    {	"RDES_COMM_OFF",pmem_driver,	0xA0141F18  },
/* INTERNAL DIAGNOSTICS I/O ADDRESS SPACE ( FIFO BLOCK ) (8)*/
    {	"RXFIFO_RD", 	pmem_driver,	0xA0142018  },
    {	"RXFIFO_WR", 	pmem_driver,	0xA0142118  },
    {	"RXFIFO_STAT", 	pmem_driver,	0xA0142218  },
    {	"TXFIFO_WR", 	pmem_driver,	0xA0142318  },
    {	"TXFIFO_RD", 	pmem_driver,	0xA0142418  },
    {	"TXFIFO_STAT", 	pmem_driver,	0xA0142518  },
    {	"TXFIFO_TEST", 	pmem_driver,	0xA0142618  },
    {	"RXFIFO_TEST", 	pmem_driver,	0xA0142718  },
/* INTERNAL DIAGNOSTICS I/O ADDRESS SPACE ( PCI BLOCK )  (1)*/
    {	"PCI_DIAG", 	pmem_driver,	0xA0142F18  },
/* INTERNAL DIAGNOSTICS I/O ADDRESS SPACE ( RXM BLOCK )  (2)*/
/* ADD_REC starting address ONLY  */
    {	"ADD_REC", 	pmem_driver,	0xA0144018  },
    {	"RXM_STAT", 	pmem_driver,	0xA0147118  },

/*
** SCSI (NCR810) registers
*/
#define NUM_SCSI 6+56

/* CONFIGURATION REGISTERS                               (6)  */
    {	"DEVID", 	pmem_driver,	0x90020018  },
    {	"CSR",  	pmem_driver,	0x90020098  },
    {	"REVID", 	pmem_driver,	0x90020118  },
    {	"MLTIM", 	pmem_driver,	0x90020198  },
    {	"BASE0IO", 	pmem_driver,	0x90020218  },
    {	"BASE1MEM", 	pmem_driver,	0x90020298  },
/* SCSI OPERATING REGISTER                               (56) */
    {	"SCNTL0", 	pmem_driver,	0x90021000  },
    {	"SCNTL1", 	pmem_driver,	0x90021020  },
    {	"SCNTL2", 	pmem_driver,	0x90021040  },
    {	"SCNTL3", 	pmem_driver,	0x90021060  },
    {	"SCID", 	pmem_driver,	0x90021080  },
    {	"SXFER", 	pmem_driver,	0x900210A0  },
    {	"SDID", 	pmem_driver,	0x900210C0  },
    {	"GPREG", 	pmem_driver,	0x900210E0  },
    {	"SFBR", 	pmem_driver,	0x90021100  },
    {	"SOCL", 	pmem_driver,	0x90021120  },
    {	"SSID", 	pmem_driver,	0x90021140  },
    {	"SBCL", 	pmem_driver,	0x90021160  },
    {	"DSTAT", 	pmem_driver,	0x90021180  },
    {	"SSTAT0", 	pmem_driver,	0x900211A0  },
    {	"SSTAT1", 	pmem_driver,	0x900211C0  },
    {	"SSTAT2", 	pmem_driver,	0x900211E0  },
    {	"DSA",  	pmem_driver,	0x90021218  },
    {	"ISTAT", 	pmem_driver,	0x90021280  },
    {	"CTEST0", 	pmem_driver,	0x90021300  },
    {	"CTEST1", 	pmem_driver,	0x90021320  },
    {	"CTEST2", 	pmem_driver,	0x90021340  },
    {	"CTEST3", 	pmem_driver,	0x90021360  },
    {	"TEMP", 	pmem_driver,	0x90021398  },
    {	"DFIFO", 	pmem_driver,	0x90021400  },
    {	"CTEST4", 	pmem_driver,	0x90021420  },
    {	"CTEST5", 	pmem_driver,	0x90021440  },
    {	"CTEST6", 	pmem_driver,	0x90021460  },
    {	"DBC",  	pmem_driver,	0x90021498  },
    {	"DCMD", 	pmem_driver,	0x900214E0  },
    {	"DNAD", 	pmem_driver,	0x90021518  },
    {	"DSP",  	pmem_driver,	0x90021598  },
    {	"DSPS", 	pmem_driver,	0x90021618  },
    {	"SCRATCHA", 	pmem_driver,	0x90021698  },
    {	"DMODE", 	pmem_driver,	0x90021700  },
    {	"DIEN", 	pmem_driver,	0x90021720  },
    {	"DWT",  	pmem_driver,	0x90021740  },
    {	"DCNTL", 	pmem_driver,	0x90021760  },
    {	"ADDER", 	pmem_driver,	0x90021798  },
    {	"SIEN0", 	pmem_driver,	0x90021800  },
    {	"SIEN1", 	pmem_driver,	0x90021820  },
    {	"SIST0", 	pmem_driver,	0x90021840  },
    {	"SIST1", 	pmem_driver,	0x90021860  },
    {	"SLPAR", 	pmem_driver,	0x90021880  },
    {	"MACNTL", 	pmem_driver,	0x900218C0  },
    {	"GPCNTL", 	pmem_driver,	0x900218E0  },
    {	"STIME0", 	pmem_driver,	0x90021900  },
    {	"STIME1", 	pmem_driver,	0x90021920  },
    {	"RESPID", 	pmem_driver,	0x90021940  },
    {	"STEST0", 	pmem_driver,	0x90021980  },
    {	"STEST1", 	pmem_driver,	0x900219A0  },
    {	"STEST2", 	pmem_driver,	0x900219C0  },
    {	"STEST3", 	pmem_driver,	0x900219E0  },
    {	"SIDL", 	pmem_driver,	0x90021A00  },
    {	"SODL", 	pmem_driver,	0x90021A80  },
    {	"SBDL", 	pmem_driver,	0x90021B00  },
    {	"SCRATCHB", 	pmem_driver,	0x90021B98  },

/* end if sable */
#endif

    {	"R16",		gpr_driver,	16},
    {	"R17",		gpr_driver,	17},
    {	"R18",		gpr_driver,	18},
    {	"R19",		gpr_driver,	19},
    {	"R20",		gpr_driver,	20},
    {	"R21",		gpr_driver,	21},
    {	"R22",		gpr_driver,	22},
    {	"R23",		gpr_driver,	23},
    {	"R24",		gpr_driver,	24},
    {	"R25",		gpr_driver,	25},
    {	"R26",		gpr_driver,	26},
    {	"R27",		gpr_driver,	27},
    {	"R28",		gpr_driver,	28},
    {	"R29",		gpr_driver,	29},
    {	"R30",		gpr_driver,	30},
    {	"R31",		gpr_driver,	31},
    {	"AI",		gpr_driver,	25},
    {	"RA",		gpr_driver,	26},
    {	"PV",		gpr_driver,	27},
    {	"FP",		gpr_driver,	29},
    {	"SP",		gpr_driver,	30},
    {	"RZ",		gpr_driver,	31},
    {	"F0",		fpr_driver,  	0},
    {	"F1",		fpr_driver,  	1},
    {	"F2",		fpr_driver,  	2},
    {	"F3",		fpr_driver,  	3},
    {	"F4",		fpr_driver,  	4},
    {	"F5",		fpr_driver,  	5},
    {	"F6",		fpr_driver,  	6},
    {	"F7",		fpr_driver,  	7},
    {	"F8",		fpr_driver,  	8},
    {	"F9",		fpr_driver,  	9},
    {	"F10",		fpr_driver, 	10},
    {	"F11",		fpr_driver, 	11},
    {	"F12",		fpr_driver, 	12},
    {	"F13",		fpr_driver, 	13},
    {	"F14",		fpr_driver, 	14},
    {	"F15",		fpr_driver, 	15},
    {	"F16",		fpr_driver, 	16},
    {	"F17",		fpr_driver, 	17},
    {	"F18",		fpr_driver, 	18},
    {	"F19",		fpr_driver, 	19},
    {	"F20",		fpr_driver, 	20},
    {	"F21",		fpr_driver, 	21},
    {	"F22",		fpr_driver, 	22},
    {	"F23",		fpr_driver, 	23},
    {	"F24",		fpr_driver, 	24},
    {	"F25",		fpr_driver, 	25},
    {	"F26",		fpr_driver, 	26},
    {	"F27",		fpr_driver, 	27},
    {	"F28",		fpr_driver, 	28},
    {	"F29",		fpr_driver, 	29},
    {	"F30",		fpr_driver, 	30},
    {	"F31",		fpr_driver, 	31},
    {	"PT0",		pt_driver,	0},
    {	"PT1",		pt_driver,	1},
    {	"PT2",		pt_driver,	2},
    {	"PT3",		pt_driver,	3},
    {	"PT4",		pt_driver,	4},
    {	"PT5",		pt_driver,	5},
    {	"PT6",		pt_driver,	6},
    {	"PT7",		pt_driver,	7},
    {	"PT8",		pt_driver,	8},
    {	"PT9",		pt_driver,	9},
    {	"PT10",		pt_driver,	10},
    {	"PT11",		pt_driver,	11},
    {	"PT12",		pt_driver,	12},
    {	"PT13",		pt_driver,	13},
    {	"PT14",		pt_driver,	14},
    {	"PT15",		pt_driver,	15},
    {	"PT16",		pt_driver,	16},
    {	"PT17",		pt_driver,	17},
    {	"PT18",		pt_driver,	18},
    {	"PT19",		pt_driver,	19},
    {	"PT20",		pt_driver,	20},
    {	"PT21",		pt_driver,	21},
    {	"PT22",		pt_driver,	22},
    {	"PT23",		pt_driver,	23},
    {	"PT24",		pt_driver,	24},
    {	"PT25",		pt_driver,	25},
    {	"PT26",		pt_driver,	26},
    {	"PT27",		pt_driver,	27},
    {	"PT28",		pt_driver,	28},
    {	"PT29",		pt_driver,	29},
    {	"PT30",		pt_driver,	30},
    {	"PT31",		pt_driver,	31},
    {	"PS",		ipr_driver,	APR$K_PS},
    {	"PC",		psr_driver,	0},
    {	"ASN",		ipr_driver,	APR$K_ASN},
    {	"ASTEN",   	ipr_driver,   	APR$K_ASTEN},
    {	"ASTSR",   	ipr_driver,   	APR$K_ASTSR},
    {	"FEN",		ipr_driver,	APR$K_FEN},
    {	"IPIR",		ipr_driver,	APR$K_IPIR},
    {	"IPL",		ipr_driver,	APR$K_IPL},
    {	"MCES",		ipr_driver,	APR$K_MCES},
    {	"PCBB",		ipr_driver,	APR$K_PCBB},
    {	"PRBR",		ipr_driver,	APR$K_PRBR},
    {	"PTBR",		ipr_driver,	APR$K_PTBR},
    {	"SCBB",		ipr_driver,	APR$K_SCBB},
    {	"SIRR",		ipr_driver,	APR$K_SIRR},
    {	"SISR",		ipr_driver,	APR$K_SISR},
    {	"TBCHK",   	ipr_driver,   	APR$K_TBCHK},
    {	"TBIA",		ipr_driver,	APR$K_TBIA},
    {	"TBIAP",   	ipr_driver,   	APR$K_TBIAP},
    {	"TBIS",		ipr_driver,	APR$K_TBIS},
    {	"KSP",		ipr_driver,	APR$K_KSP},
    {	"ESP",		ipr_driver,	APR$K_ESP},
    {	"SSP",		ipr_driver,	APR$K_SSP},
    {	"USP",		ipr_driver,	APR$K_USP},
    {	"VPTB",		ipr_driver,	APR$K_VPTB},
    {	"WHAMI",   	ipr_driver,   	APR$K_WHAMI},
    {	"R0",		gpr_driver,	0},
    {	"R1",		gpr_driver,	1},
    {	"R2",		gpr_driver,	2},
    {	"R3",		gpr_driver,	3},
    {	"R4",		gpr_driver,	4},
    {	"R5",		gpr_driver,	5},
    {	"R6",		gpr_driver,	6},
    {	"R7",		gpr_driver,	7},
    {	"R8",		gpr_driver,	8},
    {	"R9",		gpr_driver,	9},
    {	"R10",		gpr_driver,	10},
    {	"R11",		gpr_driver,	11},
    {	"R12",		gpr_driver,	12},
    {	"R13",		gpr_driver,	13},
    {	"R14",		gpr_driver,	14},
    {	"R15",		gpr_driver,	15},
    {	0,		0,		0}	/* LAST entry in symbol table */
};

/* Declare functions defined in this module: */

exdep_cmd (int argc, char *argv[]);
int exdep_init (void);
int prepend_protocol(struct exdep_struct *exdep, char *path);
read_shell_defaults (struct exdep_struct *exdep);
int symbol_lookup(char *sym, struct exdep_struct *exdep);
write_shell_defaults (struct exdep_struct *exdep);

/*+
 * ============================================================================
 * = exdep_cmd - Examine and deposit common read and write routine.           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is called by examine and deposit to implement either of
 *	these commands.  It gets the command parameters, reads or writes the
 *	location (or range of locations), and for examines, prints the results.
 *	If successful, the shell-level environment variables are updated.
 *
 * FORM OF CALL:
 *  
 *	exdep_cmd ( argc, *argv[], *exdep )
 *  
 * RETURN CODES:
 *
 *	msg_success - If the examine or deposit was successfully executed.
 *	other       - Error status passed back from a called routine.
 *
 * ARGUMENTS:
 *
 *      int argc		   - Number of arguments on the command line.
 *	char *argv[]		   - Array of pointers to argument strings.
 *	struct exdep_struct *exdep - Pointer to examine/deposit data structure.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int exdep_cmd (int argc, char *argv[]) {
    int status, status2;
    int i;
    int byte;
    int datalength;
    int found_data_qual;
    int found_address_qual;
    struct exdep_struct *exdep;
    char *dp;
    char file[MAX_NAME*4];
    char data [32];

    if (!exdep_inited) {
	exdep_init();		/* create ex dep environment variables */
	exdep_inited = 1;
    }

    /* malloc so that we're aligned. */
    exdep = dyn$_malloc (sizeof (struct exdep_struct), DYN$K_SYNC|DYN$K_FLOOD);

    /*
     * figure out if we're examine or deposit by looking at the
     * first letter of the command.
     */
    if ((argv [0] [0] == 'e') || (argv [0] [0] == 'E')) {
	exdep->examine = 1;
    }

    /* 
     * Initialize address, data type, and address space to those of the last
     * examine/deposit.  These values are stored in the shell's context:
     */
    if ((status = read_shell_defaults(exdep)) != msg_success)
	return status;

    /*
     * Call QSCAN to parse and remove the command qualifiers:
     */
    if ((status = qscan(&argc, argv, "-", QSCAN_QUAL_STR, exdep->qs)) != msg_success) {
    	err_printf (status);
	return status;
    }

    /*
     * Check for user-specified range-count
     */
    if (exdep->qs[QUAL_N].present) {
        exdep->range_count = exdep->qs[QUAL_N].value.integer;
    }

    /*
     * Check for user-specified data type. Rely on qscan flags having the same
     * order as the definitions for BYTE, WORD etc.  Complain if more than one
     * datatype was specified.
     */
    found_data_qual = 0;
    for (i = MIN_TYPE; i <= MAX_TYPE; ++i) {
    	if (exdep->qs[i].present) {
	    exdep->datatype = i;
	    found_data_qual++;
	}
    }

    /*
     * Check for a qualifier that specifies an address.
     */
    found_address_qual = 0;
    for (i = MIN_ADDRESS; i <= MAX_ADDRESS; ++i) {
    	if (exdep->qs[i].present) {
	    exdep->adqual = i;
	    found_address_qual++;
	}
    }
    if ((found_address_qual > 1) || (found_data_qual > 1)) {
	err_printf(msg_exdep_conflict);
	return msg_exdep_conflict;
    }

    /*
     * Over ride the address space if there is a 
     * qualifier
     */
    if (found_address_qual) {
        sprintf (exdep->space, "%s", address_specifiers [exdep->adqual - MIN_ADDRESS]);
    }

   /*
    * If an argument is present, treat it as a generic "address".
    * This can take one of several forms, which are evaluated in
    * the following order:
    *	1) symbolic references ("r0", "pc" etc)
    *   2) special characters such as *, +, - and @
    *	3) hex numbers, with an implicit filename of pmem
    *   4) protocol towers.
    *
    * Note that the above priority means that "e dad" causes
    * physical memory 0x00000dad to be referenced, and that this
    * prevents a file called "dad" from being examined.
    *
    * If no argument is present, we'll have inherited the previous
    * context.
    */
    if (argc > 1) {

	/*
	 * Symbolic reference; sets up filename and offset.
	 */
     	if (symbol_lookup(argv[1], exdep)) {
	    ;

	/* Indirect through previous datum in current space. 
	 * The current context contains the last datum from
	 * the previous invocation of examine/deposit.
	 */
	} else if (strcmp (argv [1], "@") == 0) {
	    exdep->address = exdep->value;

	/* next logical address in current space */
	} else if (strcmp (argv [1], "+") == 0) {
	    ;

	/* current address in current space */
	} else if (strcmp (argv [1], "*") == 0) {
	    exdep->address -= exdep->incsize;

	/* previous address in current space */
	} else if (strcmp (argv [1], "-") == 0) {
	    exdep->address -= 2 * exdep->incsize;

	/* hexadecimal offset within current space */
	} else if (strspn (argv [1], "0123456789abcdefABCDEF") == strlen (argv [1])) {
	    status = common_convert (argv [1], 16, &exdep->address, sizeof (exdep->address));
	    if (status) {
		err_printf (status);
		return status;
	    }

	/* Treat as a filename, in which case the driver will do the
	 * fseek on the fopen call.  We remind ourselves that we'll need to ask
	 * the driver later on.
	 */
	} else {
	    exdep->address = 0;
	    strcpy (exdep->space, argv [1]);
	    exdep->require_ftell = 1;

	    /* Don't allow address space qualifiers with filenames */
	    if (found_address_qual) {
		err_printf (msg_exdep_conflict);
		return msg_exdep_conflict;
	    }
	}

    }

    /*
     * Certain drivers handle only one data size.  Enforce this
     * by looking at the driver's bs (block size) field.
     */
    if (!strncmp_nocase(exdep->space, gpr_driver, 3) ||
	!strncmp_nocase(exdep->space, ipr_driver, 3) ||
	!strncmp_nocase(exdep->space, fpr_driver, 3) ||
	!strncmp_nocase(exdep->space, pt_driver, 2) ||
	!strncmp_nocase(exdep->space, psr_driver, 3)) {
	exdep->datatype = QUAL_QUADWORD;
    }
    if (!strcmp_nocase(exdep->space, gbus_driver)) {
	exdep->datatype = QUAL_BYTE;
    }

    /*
     * Check for user-specified size increment override:
     */
    if (exdep->qs[QUAL_STEPSIZE].present) {
    	exdep->incsize = exdep->qs[QUAL_STEPSIZE].value.integer;
    } else {
	exdep->incsize = datalengths [exdep->datatype];
    if (!strncmp_nocase(exdep->space, gpr_driver, 3) ||
	!strncmp_nocase(exdep->space, ipr_driver, 3) ||
	!strncmp_nocase(exdep->space, fpr_driver, 3) ||
	!strncmp_nocase(exdep->space, pt_driver, 2)) {
		exdep->incsize = 1;
	}
    }

    /*
     * Treat all remaining arguments as data to be deposited.
     */
    if (!exdep->examine) {
	if (argc <= 2) {
	    err_printf(msg_dep_param_missing);
	    return msg_dep_param_missing;
	}

	/* 
	 * store the next numeric datum into the data buffer
	 */
	dp = &exdep->data [0];
	for (i=2; i<argc; i++) {
	    if (exdep->data_count >= MAX_DATACOUNT) {
		err_printf (msg_extra_params);
		return msg_extra_params;
	    }
	    if (common_convert(argv[i], 16, dp, datalengths [exdep->datatype]) != msg_success) {
		err_printf(msg_dep_baddata);
		return msg_dep_baddata;
	    }
	    dp += datalengths [exdep->datatype];
	    exdep->data_count++;
	}

	/*
	 * remember the last datum typed in, zero extended.
	 */
	exdep->value = 0;
	memcpy (
	    &exdep->value,
	    dp - datalengths [exdep->datatype],
	    min (datalengths [exdep->datatype], sizeof (exdep->value))
	);


    /*
     * Deposit snarfs up all remaining arguments, but examine must check
     * for extraneous arguments:
     */
    } else if (argc > 2) {
	err_printf(msg_exdep_badparam);
	return msg_exdep_badparam;
    }


    /*
     * Open up the file for examine, and leave it open for the duration of the
     * command.
     */
    if (exdep->examine) {
	prepend_protocol(exdep, file);

	if ((exdep->fp = fopen(file, "r")) == NULL) {
	    err_printf(msg_ex_devopenrderr, file);
	    return msg_ex_devopenrderr;
	}

    /*
     * Open up the file for deposit and leave it open for the
     * duration of the command.
     */
    } else {
    	if ((exdep->fp = fopen(exdep->space, "r+")) == NULL) {
	    err_printf(msg_dep_devopenwrerr, exdep->space);
	    return msg_dep_devopenwrerr;
	}
    }

    /*
     * If the driver positioned us on the fopen, we have to ask it where we
     * are (exdep wants to own the address).
     */
    if (exdep->require_ftell) {
	exdep->address = ftell (exdep->fp);
    }

    /*
     * Bump up the range count if it isn't large enough to handle all the
     * data on the command line
     */
    if (!exdep->examine && (exdep->data_count > (exdep->range_count+1))) {
	exdep->range_count = exdep->data_count - 1;
    }

    /*
     * Execute the command:
     */
    exdep->data_index = 0;
    dp = & exdep->data [0];
    do {

	/*
	 * position ourselves in the file
	 */
	memcpy (exdep->fp->offset, &exdep->address, sizeof (exdep->address));

	/*
	 * Read for examine.  The protocol tower for reads always writes an
	 * ascii string into the data buffer.
	 */
	if (exdep->examine) {
	    if (fread(&exdep->data, 1, sizeof (BIGCHUNK), exdep->fp) == 0) {
		if (exdep->fp->status != 0)
		    err_printf(exdep->fp->status, exdep->address);
		else
		    err_printf(msg_ex_devreaderr,
			datalengths [exdep->datatype],
			exdep->address,
			file);
		status = msg_ex_devreaderr;
		break;
	    }

	    /* Render the data */
	    printf("%s", exdep->data);

	/*
	 * write for deposit
	 */
	} else {
	    if (fwrite (dp, datalengths [exdep->datatype], 1, exdep->fp) == 0) {
		if (exdep->fp->status != 0)
		    err_printf(exdep->fp->status, exdep->address);
		else
		    err_printf(msg_dep_devwriteerr,
			datalengths [exdep->datatype],
			exdep->address,
			exdep->space);
		return msg_dep_devwriteerr;
	    }

	    /* advance to next datum */
	    exdep->data_index++;
	    dp += datalengths [exdep->datatype];
	    if (exdep->data_index >= exdep->data_count) {
		exdep->data_index = 0;
		dp = & exdep->data [0];
	    }
	}


	/*
	 * Advance by the step size if it was specified, otherwise let
	 * the driver tell how much it advanced.  This accomodates
	 * instruction decode where the amount of advance depends on the 
	 * length of the instruction (on VAX).
	 */
	if (exdep->qs [QUAL_STEPSIZE].present) {
	    exdep->address += exdep->incsize;
	} else {
	    exdep->address = ftell (exdep->fp);
	}

	if (killpending()) break;
    } while (--exdep->range_count >= 0);

    /*
     * Remember the numerical value of the last data examined/deposited,
     * needed for a subsequent "@".  For examine, the data is returned as
     * ascii, and we assume that the data is the last field, in hex, and
     * delineated by whitespace. In the case of instruction decode, jam a
     * 0 into the data.
     */
    if (exdep->examine) {
	if (exdep->datatype == QUAL_DECODE) {
	    exdep->value = 0;
	} else {
	    /* count the fields and then take the last one */
	    i = 0;
	    while (strelement (exdep->data, i, " \n", data)) {
		common_convert (data, 16, &exdep->value, sizeof (exdep->value));
		i++;
	    }
	}
    } else {
	; /* already captured for deposit */
    }

    fclose (exdep->fp);

    /*
     * Copy relevant state to the shell level
     */

    status2 = write_shell_defaults(exdep);
    if (status != msg_success) 
	return status;
    else
	return status2;
}

/*+
 * ============================================================================
 * = prepend_protocol - Prepend protocol driver for examine rendering.	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Select a  protocol based on data type and address space and prepend it
 * 	to the address space for a pathname to fopen.
 *
 * FORM OF CALL:
 *  
 *	prepend_protocol(exdep, path);
 *  
 * RETURN CODES:
 *
 *	msg_success - If successful.
 *
 * ARGUMENTS:
 *
 *      struct exdep_struct *exdep  - Pointer to examine/deposit data structure.
 *	char *path		    - Pointer to pathname buffer for output string.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int prepend_protocol(struct exdep_struct *exdep, char *path) {
    char file [MAX_NAME*4];
    char *info, *next;

    /* Table associating "terminating" drivers (pmem, vmem, gpr, etc.) with
     * protocol drivers to render the output for examine:
     */
    static struct examine_protocol_struct {
    	char *tdriver;			/* Name of terminating driver */
	char *pdriver;			/* Name of protocol driver */
    } examine_protocol[] = {
	{psr_driver,	"psr_render:\"%x %x \"/%s"},	 /* PSR driver requires special rendering */
	{vmem_driver,	"vmem_render:\"%x %x \"/%s"},	 /* VMEM driver requires special rendering */
	{ NULL,	    	"examine_render:\"%x %x \"/%s"} /* All others use default rendering */
    };
    int i;

    /*
     * If we're doing instruction decode, prepend decode driver.
     */
    if (exdep->datatype == QUAL_DECODE) {
    	sprintf (path, "decode/%s", exdep->space);
	return msg_success;
    }

    /*
     * Otherwise, select the protocol based on the address space
     * We have to pick off the driver name from the protocol tower.
     */
    validate_filename (exdep->space, file, &info, &next);
    for (i = 0; examine_protocol[i].tdriver; i++) {
	if (!strcmp_nocase(file, examine_protocol[i].tdriver)) {
	    break;
	}
    }
    sprintf(path,
	examine_protocol[i].pdriver,
	datalengths [exdep->datatype],
	1,
	exdep->space
    );

    return msg_success;
}

/*+
 * ============================================================================
 * = exdep_init - Initialize examine/deposit specific environment variables.  = 
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine should be called by the shell as part of its
 *	initialization in order to create the environment variables required
 *	to support the examine/deposit commands for that shell instantiation.
 *
 * FORM OF CALL:
 *  
 *	exdep_init ()
 *  
 * RETURN CODES:
 *
 *	msg_success   -	Environment variables were successfully created.
 *	Other	      -	Failure returned by ev_write.
 *
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      New environment variables are created.
 *
-*/
int exdep_init (void) {
    int stat;

    int i;
    int *lp;

    /* clear upper longwords */
    for (i = 0; exdep_symbol_table[i].symbol; i++) {
	lp = & exdep_symbol_table [i].location;
	*(lp + 1) = 0; 
    }

#if SABLE*0
    /*
    * patch the symbol table so that alpha symbols have true 64 bit addresses
    */
    for (i=0; i<NUM_SIO_HACKS; i++) {
	lp = & exdep_symbol_table [i].location;
	*(lp + 1) = 3;
    }
#endif

    /* Create the environment variable for the location referenced by the last examine or deposit command: */
    if ((stat = ev_write(ev_exdep_location, "0", EV$K_STRING | EV$K_NOWRITE | EV$K_NODELETE)) != msg_success)
	return stat;

    /* Create the environment variable for the data value referenced by the last examine or deposit command: */
    if ((stat = ev_write(ev_exdep_data, "0", EV$K_STRING | EV$K_NOWRITE | EV$K_NODELETE)) != msg_success)
	return stat;

#if !MODULAR
    /* Create the environment variable for the data type referenced by the last examine or deposit command: */
    if ((stat = ev_write(ev_exdep_type, QUAL_QUADWORD, EV$K_INTEGER | EV$K_NOWRITE | EV$K_NODELETE)) != msg_success)
	return stat;

    /* Create the environment variable for the data size referenced by the last examine or deposit command: */
    if ((stat = ev_write(ev_exdep_size, QUAL_QUADWORD, EV$K_INTEGER | EV$K_NOWRITE | EV$K_NODELETE)) != msg_success)
	return stat;
#else
    /* Create the environment variable for the data type referenced by the last examine or deposit command: */
    if ((stat = ev_write(ev_exdep_type, QUAL_LONGWORD, EV$K_INTEGER | EV$K_NOWRITE | EV$K_NODELETE)) != msg_success)
	return stat;

    /* Create the environment variable for the data size referenced by the last examine or deposit command: */
    if ((stat = ev_write(ev_exdep_size, QUAL_LONGWORD, EV$K_INTEGER | EV$K_NOWRITE | EV$K_NODELETE)) != msg_success)
	return stat;
#endif
    /* Create the environment variable for the address space referenced by the last examine or deposit command: */
    if ((stat = ev_write(ev_exdep_space, pmem_driver, EV$K_STRING | EV$K_NOWRITE | EV$K_NODELETE)) != msg_success)
	return stat;

    return msg_success;
}

/*+
 * ============================================================================
 * = read_shell_defaults - Get the "sticky" command options.                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine gets saved information about the last examine/deposit
 *	from the shell to be used as defaults for the current examine or
 *	deposit command.
 *
 *      References environment variables ev_exdep_location, ev_exdep_type,
 *	ev_exdep_size, and ev_exdep_space.
 *
 * FORM OF CALL:
 *  
 *	read_shell_defaults ( *exdep )
 *  
 * RETURN CODES:
 *
 *	msg_success	  - If successful.
 *	OTHER		  - Failure returned by ev_read.
 *
 * ARGUMENTS:
 *
 *      struct exdep_struct *exdep - Pointer to examine/deposit data structure.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int read_shell_defaults (struct exdep_struct *exdep) {
    struct EVNODE *evp;
    int stat;

    /* Create temporary EVNODE for ev_read routine: */
    evp = dyn$_malloc(sizeof(struct EVNODE), DYN$K_SYNC);

    /* Read the last examine/deposit location: */
    if ((stat = ev_read(ev_exdep_location, &evp, EV$K_SYSTEM)) != msg_success)
	return stat;
    common_convert (evp->value.string, 16, &exdep->address, sizeof (exdep->address));

    /* Read the last data type: */
    if ((stat = ev_read(ev_exdep_type, &evp, EV$K_SYSTEM)) != msg_success)
	return stat;
    exdep->datatype = evp->value.integer;

    /* Read the last data size: */
    if ((stat = ev_read(ev_exdep_size, &evp, EV$K_SYSTEM)) != msg_success)
	return stat;
    exdep->incsize = evp->value.integer;

    /* Read the last address space: */
    if ((stat = ev_read(ev_exdep_space, &evp, EV$K_SYSTEM)) != msg_success)
	return stat;
    strcpy(exdep->space, evp->value.string);

    /* read the last datum  (used for "@" ) */
    if ((stat = ev_read(ev_exdep_data, &evp, EV$K_SYSTEM)) != msg_success)
	return stat;
    common_convert (evp->value.string, 16, &exdep->value, sizeof (exdep->value));

    dyn$_free(evp, DYN$K_SYNC);

    return msg_success;
}


/*+
 * ============================================================================
 * = write_shell_defaults - Make the current command options "stick".         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine stores information about the current examine/deposit
 *	to the shell so that they may be used as defaults for the next
 *	examine or deposit command.
 *
 *      Updates the environment variables ev_exdep_location, ev_exdep_type,
 *	ev_exdep_size, and ev_exdep_space.
 *
 * FORM OF CALL:
 *  
 *	write_shell_defaults ( *exdep )
 *  
 * RETURN CODES:
 *
 *	msg_success   -	If successful.
 *	OTHER	      -	Failure returned by ev_write.
 *
 * ARGUMENTS:
 *
 *      struct exdep_struct *exdep - Pointer to examine/deposit structure.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int write_shell_defaults (struct exdep_struct *exdep) {
    int stat;
    char file [MAX_NAME];
    char *info, *next;

    /* Update the current location environment variable: */
    sprintf (file, "%.*X", sizeof(exdep->address) * 2, exdep->address);
    if ((stat = ev_write(ev_exdep_location, file, EV$K_STRING)) != msg_success)
	return stat;

    /* Update the current data value environment variable: */
    sprintf (file, "%.*X", sizeof(exdep->value) * 2, exdep->value);
    if ((stat = ev_write(ev_exdep_data, file, EV$K_STRING)) != msg_success)
	return stat;

    /* Update the current data type environment variable: */
    if ((stat = ev_write(ev_exdep_type, exdep->datatype, EV$K_INTEGER)) != msg_success)
	return stat;

    /* Update the current data size environment variable: */
    if ((stat = ev_write(ev_exdep_size, exdep->incsize, EV$K_INTEGER)) != msg_success)
	return stat;

    /* Update the current address space environment variable
     * We pick off the info field (the information in the info field has
     * already be assimilated in the file offset).
     */
    validate_filename (exdep->space, file, &info, &next);
    if ((stat = ev_write(ev_exdep_space, exdep->space, EV$K_STRING)) != msg_success)
	return stat;

    return msg_success;
}

/*+
 * ============================================================================
 * = symbol_lookup - Match input symbol to one in examine/deposit table.      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Simple lookup function to find a match between the input symbol and
 *	a known symbol for address parameters of examine/deposit.
 *	If found, the function will automatically update the address and
 *	space fields of the given exdep data structure.
 *
 * FORM OF CALL:
 *  
 *	symbol_lookup ( *sym, *exdep )
 *  
 * RETURN CODES:
 *
 *      0   - Symbol not found.
 *	1   - Symbol found.
 *
 * ARGUMENTS:
 *
 *      char *sym		   - Pointer to input symbol (string).
 *	struct exdep_struct *exdep - Pointer to examine/deposit structure
 *				     to be updated if symbol is found.
 *
 * SIDE EFFECTS:
 *
 *      none
 *
-*/
int symbol_lookup(char *sym, struct exdep_struct *exdep) {
    int i;

    /* Just use a basic linear search.
     * Use case-independent string compare, but don't allow abbreviations.
     */
    for (i = 0; exdep_symbol_table[i].symbol; i++) {
	if (strcmp_nocase(sym, exdep_symbol_table[i].symbol) == 0) {
	    exdep->address = exdep_symbol_table[i].location;
	    strcpy(exdep->space, exdep_symbol_table[i].space);
	    return 1;
	}
    }
    return 0;
}

/*+
 * ============================================================================
 * = get_exdep_symbol - Find an examine/deposit symbol.                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This function searches the examine/deposit symbol table for a match
 *	based on address space and location and returns a pointer to the
 *	corresponding symbol (string) if found.
 *
 *      References exdep_symbol_table.
 *
 * FORM OF CALL:
 *  
 *	get_exdep_symbol ( loc, space )
 *  
 * RETURNS:
 *
 *      Returns pointer to string or NULL if no match.
 *
 * ARGUMENTS:
 *
 *      int *loc	- address of location to search for in table.
 *	char *space - Address space to search for in table.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
char *get_exdep_symbol(INT loc, char *space)
{
    int i;

    for (i = 0; exdep_symbol_table[i].symbol; i++) {
    	if ((!strcmp_nocase(exdep_symbol_table[i].space, space)) &&
	     (exdep_symbol_table[i].location == loc)) {
	    return exdep_symbol_table[i].symbol;
	}
    }
    return NULL;
}


/*+
 * ============================================================================
 * = deposit - Write data to a specified address.                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Writes data to an address that you specify: a memory 
 *	location, a register, a device, or a file.   
 *
 *	After initialization, if you have not specified a data address 
 *	or size, the default address space is physical memory,
 *	the default data size is a quadword, and the default address is zero.
 * 
 *	You specify an address or "device" by concatenating the 
 *	device name with the address, for example, PMEM:0 and by 
 *	specifying the size of the space to be written to. 
 * 
 *	If you do not specify an address, the data is written to  
 *	the current address, in the current data size (the last 
 *	previously specified address and data size).
 *
 *	If you specify a conflicting device, address, or data size, 
 *	the console ignores the command and issues an error response.
 *  
 *	The display line consists of the device name,
 *	the hexadecimal address (or offset within the device),
 *	and the examined data also in hexadecimal.
 *
 *	The EXAMINE command supports most of the same options.  
 *	Additionally, EXAMINE supports instruction decoding, 
 *	the -d option, which disassembles instructions beginning 
 *	at the current address.
 *
 *   COMMAND FMT: exdep_cmd 1 B 0 deposit
 *
 *   COMMAND FORM:
 *  
 *	deposit ( [-{b,w,l,q,o,h}] [-{physical,virtual,gpr,fpr,ipr}]
 *                [-n <count>] [-s <step>]
 *                [<device>:]<address>  <data> )
 *  
 *   COMMAND TAG: exdep_cmd 0 RXB d deposit
 *
 *   COMMAND ARGUMENT(S):
 *
 *	[<device>:] - The optional device name (or address space)
 *		selects the device to access.
 *		The following platform independent devices supported:
 *#o
 *		o pmem: Physical memory.
 *      	o vmem: Virtual memory.  All access and protection checking
 * 			occur.  If  the  access  would  not be allowed to a
 *			program  running  with  the current PS, the console
 *			issues  an error message.  If memory mapping is not
 *			enabled,  virtual  addresses  are equal to physical
 *			addresses.
 *		o gpr: General Purpose Register set, R0-R31.  The data
 *			size defaults to -q.
 *		o fpr: Floating Point Register set, F0-F31.  The data size
 *			defaults to -q.
 *		o pt: PAL Temporary Register set, PT0-PT31.  The data
 *			size defaults to -q.
 *#
 * 		The following platform dependent devices are supported:
 *#o
 *#
 *
 *  <address> -	An address that specifies the offset within a device
 *		into which data is deposited.
 *		The address may be any valid hexadecimal offset in the device's
 *		address space.
 *
 *		Note: Memory addresses of the form: Fdd,
 * 		where dd is a 1-2 digit decimal number, must be
 * 		preceded  by  a  0  to  prevent recognition as a Floating-Point
 * 		Register.  Ex: 0f0 is a valid memory address; f0 is not.
 *
 *		The address may also be any legal symbolic address.
 *		Hovever, a symbolic name cannot be used if the address space
 *		was explicitly specified.
 *		The following forms are valid symbolic addresses:
 *#o
 *  o gpr-name	- A symbol representing a General Purpose Register.
 * 		The size defaults to quadword; address space defaults to gpr.
 * 		The following symbols are recognized:
 *		R0, R1, ..., R31, AI, RA, PV, FP, SP, and RZ.
 *  o fpr-name	- A symbol representing the Floating Point Register.
 * 		The size defaults to quadword; address space defaults to fpr.
 * 		The following symbols are recognized: F0, F1, ..., F31.
 *  o ipr-name	- A symbol representing the Internal Processor Register.
 * 		The size defaults to quadword; address space defaults to ipr.
 * 		The following symbols are recognized: PS, ASN, ASTEN, ASTSR,
 * 		AT, FEN, IPIR, IPL, MCES, PCBB, PRBR, PTBR, SCBB, SIRR, SISR,
 * 		TBCHK, TBIA, TBIAP, TBIS, ESP, SSP, USP, and WHAMI.
 *  o pt-name	- A symbol representing the PALtemp Register.  The data size
 * 		defaults to  quadword; address space defaults to pt.  The
 *		following symbols are recognized: PT0, PT1, ...  PT31.
 *  o PC	- Program Counter (execution address).  The last address,
 * 		size, and type are unchanged.
 *  o	"+"	- The location immediately following the last
 * 		location referenced in an examine or deposit.  For
 * 		references to physical or virtual memory, the
 * 		location is the last address plus the size of the
 * 		last reference.  For other address spaces, the
 * 		address is the last address referenced plus one.
 *  o 	"-"	- The location immediately preceding the last
 * 		location referenced in an examine or deposit.  For
 * 		references to physical or virtual memory, the
 * 		location is the last address minus the size of the
 * 		last reference.  For other address spaces, the
 * 		address is the last address referenced minus one.
 *  o 	"*"	- The location last referenced by an examine or
 * 		deposit.
 *  o 	"@"	- The location addressed by the last location
 * 		referenced in an examine or deposit.
 *# 
 * 
 *   <data>  -  The data to be deposited.
 *		If the specified data is larger than the deposit data size,
 *		the console ignores the command and issues an error response.
 *		If the specified data is smaller than the deposit data size,
 *		it is extended on the left with zeros.
 *
 *   COMMAND OPTION(S):
 *
 *	-b -	The data type is byte.
 *	-w -	The data type is word.
 *	-l -	The data type is longword.
 *	-q -	The data type is quadword.
 *	-o -	The data type is octaword.
 *	-h -	The data type is hexaword.
 *	-physical - The address space is physical memory.
 *	-virtual  - The address space is virtual memory.
 *	-gpr      - The address space is general purpose registers.
 *	-fpr      - The address space is floating point registers.
 *	-ipr      - The address space is internal processor registers.
 *	-n <count> - Specifies the number of consecutive locations, val (hex), to
 *		modify.  The console deposits to the first address, then to the
 *		specified number of succeeding addresses.
 *	-s <step> - Specifies the address increment size (hex).
 *		Normally this defaults to the data size, but is overriden
 *	        by the presence of this qualifier.
 *		This qualifier is not inherited.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>d -b -n 1FF pmem:0 0       ! Clear first 512 bytes of physical memory.
 *	>>>d -l -n 3 vmem:1234 5      ! Deposit 5 into four longwords starting at virtual memory address 1234.
 *	>>>d -n 8 R0 FFFFFFFF         ! Loads GPRs R0 through R8 with -1.
 *	>>>d -l -n 10 -s 200 pmem:0 8 ! Deposit 8 in the first longword of the first 17 pages in physical memory.
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	examine
 *
 * FORM OF CALL:
 *  
 *	deposit( argc, *argv[] )
 *  
 * RETURNS:
 *
 *	Returns status from exdep_cmd.
 *       
 * ARGUMENTS:
 *
 *      int argc      -	Number of arguments on the command line.
 *	char *argv[]  -	Array of pointers to argument strings.
 *
 * SIDE EFFECTS:
 *
 *	This routine is started by the shell to execute the DEPOSIT
 *	command.  It statically allocates an exdep structure, clears a flag
 *	in the exdep structure to indicate an DEPOSIT command, and then
 *	passes control to exdep_cmd, which handles both EXAMINEs and
 *	DEPOSITs.
 *
-*/

/*+
 * ============================================================================
 * = examine - Display data at a specified address.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	The EXAMINE command displays the contents of an address 
 *	that you specify: either a memory location, a register, 
 *	a device, or a file.   
 *
 *	After initialization, if you have not specified a data address 
 *	or size, the default address space is physical memory,
 *	the default data size is a quadword, and the default address is zero.
 *
 *	You specify an address or "device" by concatenating the 
 *	device name with the address, for example, PMEM:0, and by 
 *	specifying the size of the data to be displayed. 
 * 
 *	If you do not specify an address, the system displays the 
 *	data at an address that is based on the current address and 
 *	data size (the last previously specified address and data size).
 * 
 *	If you specify a conflicting device, address, or data size,  
 *	the console ignores the command and issues an error response.
 *  
 *	The display line consists of the device name,
 *	the hexadecimal address (or offset within the device),
 *	and the examined data, also in hexadecimal.
 *
 *	EXAMINE uses the same options as DEPOSIT. Additionally, 
 *	the EXAMINE command supports instruction decoding, 
 *	the -d option, which disassembles instructions
 *	beginning at the current address.
 *
 *   COMMAND FMT: exdep_cmd 1 B 0 examine
 *
 *   COMMAND FORM:
 *  
 *	examine ( [-{b,w,l,q,o,h,d}] [-{physical,virtual,gpr,fpr,ipr}]
 *                [-n <count>] [-s <step>]
 *                [<device>:]<address> )
 *  
 *   COMMAND TAG: exdep_cmd 0 RXB e examine
 *
 *   COMMAND ARGUMENT(S):
 *
 *	[device:] - The optional "device" name (or address space)
 *		selects the device to access (see DEPOSIT).
 *
 *	address - The address specifies the first location to examine
 *		within the current device.
 *		The address can be any legal address specifier (see DEPOSIT).
 *
 *   COMMAND OPTION(S):
 *
 *	-b -	The data size is byte.
 *	-w -	The data size is word.
 *	-l -	The data size is longword.
 *	-q -	The data size is quadword.
 *	-o -	The data size is octaword.
 *	-h -	The data size is hexaword.
 *	-d -	The data displayed is the decoded macro instruction.
 *		Alpha instruction decode (-d) does not recognize
 *		machine-specific PAL instructions.
 *	-physical - The address space is physical memory.
 *	-virtual  - The address space is virtual memory.
 *	-gpr      - The address space is general purpose registers.
 *	-fpr      - The address space is floating point registers.
 *	-ipr      - The address space is internal processor registers.
 *	-n <count> - Specifies the number of consecutive locations to examine.
 *	-s <step> - Specifies the address increment size, val (hex).
 *		Normally this defaults to the data size, but is overriden
 *	        by the presence of this option.
 *		This option is not "sticky".
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>e r0                                             ! Examine R0 by symbolic address.
 *      gpr:                0 (    R0) 0000000000000002
 *      >>>e -g 0                                           ! Examine R0 by address space.
 *      gpr:                0 (    R0) 0000000000000002
 *      >>>e gpr:0                                          ! Examine R0 by device name.
 *      gpr:                0 (    R0) 0000000000000002
 *	>>>examine pc                                       ! Examine the PC.
 *	gpr: 0000000F (    PC) FFFFFFFC
 *	>>>examine sp                                       ! Examine the SP.
 *	gpr: 0000000E (    SP) 00000200
 *	>>>examine psl                                      ! Examine the PSL.
 *			CM TP FPD IS CURMOD PRVMOD IPL DV FU IV T N Z V C
 *	PSL 00000000     0  0  0   0 KERNEL KERNEL  00  0  0  0 0 0 0 0 0
 *	>>>examine -n 5 R7                                  ! Examine R7 through R12.
 *	gpr: 00000007 (    R7) 00000000
 *	gpr: 00000008 (    R8) 00000000
 *	gpr: 00000009 (    R9) 801D9000
 *	gpr: 0000000A (   R10) 00000000
 *	gpr: 0000000B (   R11) 00000000
 *	gpr: 0000000C (    AP) 00000000
 *	>>>examine ipr:11                                   ! Examine the SCBB, IPR 17.
 *	ipr: 00000011 (  SCBB) 2004A000
 *	>>>examine scbb					    ! Examine the SCBB using symbolic name.
 *	ipr: 00000011 (  SCBB) 2004A000
 *	>>>examine pmem:0                                   ! Examine physical address 0.
 *	pmem: 00000000 00000000
 *	>>>examine -d 40000                                 ! Examine with instruction decode.
 *	pmem: 00040000	11 BRB     20040019
 *	>>>examine -d -n 5  40019                           ! Disassemble from branch.
 *	pmem: 00040019  D0 MOVL    I^#20140000,@#20140000
 *	pmem: 00040024  D2 MCOML   @#20140030,@#20140502
 *	pmem: 0004002F  D2 MCOML   S^#0E,@#20140030
 *	pmem: 00040036  7D MOVQ    R0,@#201404B2
 *	pmem: 0004003D  D0 MOVL    I^#201404B2,R1
 *	pmem: 00040044  DB MFPR    S^#2A,B^44(R1)
 *	>>>examine					    ! Look at next instruction.
 *	pmem: 20040048   DB MFPR    S^#2B,B^48(R1)
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	deposit, hd
 *
 * FORM OF CALL:
 *  
 *	examine( argc, *argv[] )
 *  
 * RETURNS:
 *
 *	Returns status from exdep_cmd.
 *       
 * ARGUMENTS:
 *
 *      int argc      -	Number of arguments on the command line.
 *	char *argv[]  -	Array of pointers to argument strings.
 *
 * SIDE EFFECTS:
 *
 *	This routine is started by the shell to execute the EXAMINE
 *	command.  It allocates an exdep structure off the stack, sets a flag in
 *	the exdep structure to indicate an EXAMINE command, and then passes
 *	control to exdep_cmd, which handles both EXAMINEs and DEPOSITs.
 *
-*/

