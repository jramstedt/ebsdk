/* file:	smm_def.sdl                                                 */
/*                                                                          */
/* Copyright (C) 1993, 1994 by                                                    */
/* Digital Equipment Corporation, Maynard, Massachusetts.                   */
/* All rights reserved.                                                     */
/*                                                                          */
/* This software is furnished under a license and may be used and copied    */
/* only  in  accordance  of  the  terms  of  such  license  and with the    */
/* inclusion of the above copyright notice. This software or  any  other    */
/* copies thereof may not be provided or otherwise made available to any    */
/* other person.  No title to and  ownership of the  software is  hereby    */
/* transferred.                                                             */
/*                                                                          */
/* The information in this software is  subject to change without notice    */
/* and  should  not  be  construed  as a commitment by digital equipment    */
/* corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by digital.                  */
/*                                                                          */
/*                                                                          */
/* Abstract:	Sable Memory Module data structure definitions.             */
/*                                                                          */
/* Author:	Harold Buckingham                                           */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	hcb	10-Jan-1994	Added bank0_failure flag in INTLV structure */
/*                                                                          */
/*	hcb	27-Sep-1993	Make size unsigned 			    */
/*                                                                          */
/*	hcb	29-Jul-1993	Add error logging info			    */
/*                                                                          */
/*	hcb	20-Jul-1993	remove read/write cpu csr macro		    */
/*                                                                          */
/*	hcb	22-Jun-1993	Add support stuctures for EDC diag	    */
/*                                                                          */
/*	hcb	16-Jun-1993	CSICs are here only use Sable csr space	    */
/*                                                                          */
/*	hcb	24-May-1993	Hack memory CSR access to read Cobra space  */
/*                                                                          */
/*	hcb	23-Apr-1993	Initial entry.                              */
/*                                                                          */
                                   
/*** MODULE $cmmdef ***/
/*General program constants                                                 */

/* Read/write mem csr macro */
/* Address of memory csr c on module in slot s */
/* s - memory slot of desired board */
/* c - offset in bytes into csrs on module */
/* v - value to write to the csr */

#if GAMMA
#define mem_base_addr(s) ((0x88000000)+((three+0x80)<<32)+(s<<24))
#else
#define mem_base_addr(s) ((0x88000000)+(three<<32)+(s<<24))
#endif

#define MEM_CSR_BASE(s) mem_base_addr(s)
#define READ_MEM_CSR(s,c)  (*(volatile unsigned __int64 *)(MEM_CSR_BASE(s)+c))
#define WRITE_MEM_CSR(s,c,v) (*(volatile unsigned __int64 *)(MEM_CSR_BASE(s)+c) = v)
#define _READ_MEM_CSR(c)  (*(volatile unsigned __int64 *)(p_mem_csr+c))
#define _WRITE_MEM_CSR(c,v) (*(volatile unsigned __int64 *)(p_mem_csr+c) = v)

/*                                                                          */
/* Cobra Memory Module powerup state codes                                  */
/*                                                                          */
#define BANK_PER_BOARD		4
#define MIN_BANK_SIZE		0x800000
#define BASE_ADDR_MPLY		0x1000000  
#define SMM$K_CSIC1_ERR 	72 
#define SMM$K_CSIC2_ERR 	73
#define CSIC_PASS1		4               
#define CSIC_PASS2		5               
#define CSICP1_BANK_SEL		25         
#define TEST_OFFSET		32                    
#define CMM$K_CMIC1_ERR	SMM$K_CSIC1_ERR 
#define CMM$K_CMIC2_ERR	SMM$K_CSIC2_ERR
/* These are defines for the EEPROM error logs */
#define SMM_CSIC_EVEN		0 
#define SMM_CSIC_ODD		1
#define SMM_CRD_ERR		0
#define SMM_UNCRD_ERR		1
#define SMM_UNCRD_NOID_ERR	2
#define SMM_OTHER_ERR		3
#define SABLE_FIRMWARE		1

#define SMM_CRD_ERR_THRESHOLD	4
#define SMM_UNCRD_ERR_THRESHOLD	0
#define SMM_BANK_ERR_THRESHOLD	1
#define SMM_MAX_LOG_BITS	2

/* constants used to ID failing SIMM Module */
#define SIMM_ID_SYND		1
#define SIMM_ID_BITS		2
#define SIMM_PER_BORAD		32
#define SIMM_TO_J_NUM		35

#define DEMI_MAX_MEMORY		0x28000000


/*                                                                          */
/* These structures are used for error logging and recording the error      */
/* information into the IIC EEPROM.                                         */
/*                                                                          */

struct SMM_ERR_PKT {
    int fail_brd;
    unsigned int fail_address;
    int error_type;
    int asic_id;
    int bank_num;
    int source_id;
    union {
	unsigned __int64 corr_synd;
	struct {
	    unsigned int bit1;
	    unsigned int bit2;
	} uncor;
    } bits;
};                
/* Masks used with above structure to reduce to the IIC EEPROM format */
/* The error sturcture is heavily used in the memory diagnostics but */
/* only needs to be reduced before writing to the EEPROM.  By using */
/* longword access bit manipulation is avoided except during the write */
/* to the EEPROM. */
#define SOURCE_ID_MASK		0x00e00000
#define BANK_NUM_MASK		0x00180000
#define ASIC_ID_MASK		0x00040000
#define ERROR_TYPE_MASK		0x00030000
#define BAD_BIT1_MASK		0x000000ff
#define BAD_BIT2_MASK		0x0000ff00 
#define CORR_SYND_MASK		0x0000ffff

#define SOURCE_ID_SHIFT		0x15
#define BANK_NUM_SHIFT		0x13
#define ASIC_ID_SHIFT		0x12
#define ERROR_TYPE_SHIFT	0x10
#define BAD_BIT1_SHIFT		0x0
#define BAD_BIT2_SHIFT		0x8
#define CORR_SYND_SHIFT		0x0

#define SROM_PASS		0x0
#define SROM_FAIL		0x1

struct SMM_ERR_LOG {
    int log_num;
    struct SMM_ERR_PKT pkt [15];
};
struct CSM {
    int cmm_init_flag;
    int inprogress;
    int present;
    int chip_rev;
    int powerup_state;
    int config_state;
    int ramsize;
    int num_banks;
    struct SMM_ERR_LOG *err_log_ptr;
    int simms_installed;
    int dram_speed;
};
struct SMM_TEST {
    unsigned int start_addr;
    unsigned int fail_addr;
    int incr;     
    int bank_incr;     
    int len;
    int pass;       
    int data_cmp_flag;       
    int fail_brd;
    int base_mod;
};                      
struct INTLV {
    unsigned int size;
    int slot;
    unsigned int base_addr;
    int intlv_mode;
    int intlv_unit;
    int bank0_failure;
};
