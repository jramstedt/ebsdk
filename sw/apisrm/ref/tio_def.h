/*
* Module Header:            File Name:    TIO_DEF.H
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
* and  should  not  be  construed  as a commitment by digital equipment
* corporation.
*
* Digital assumes no responsibility for the use  or  reliability of its
* software on equipment which is not supplied by digital.
*
*
* Abstract:
*
*	Data structures definitions file for Turbo-Laser diagnostics
*
* Author:      C.T. Olson
*
* Modifications:
*
*	cto	08-Jul-1993	Initial version from TMAIN.H
*/

/* Note that ordering of includes is important here; tiop.h, tlsb.h and
*	tlep.h must be included before this file so the defs/structs are
*	defined
*/

/* Fix .SDL upper bit masks */
#undef BIT_M_HOSEn_RESET
#define BIT_M_HOSEn_RESET 0x80000000
#undef BIT_M_INTR_NSES
#define BIT_M_INTR_NSES 0x80000000
#undef BIT_M_VOLT_MARG
#define BIT_M_VOLT_MARG 0x80000000


/* TIOP CSRs structure */
typedef struct _tiop_csr
{
	struct TLSB tlsb;	/* TLSB required CSRs */
	struct TIOP tiop;       /* TIOP specific CSRs */
} Tiop_csr;

/* TLEP CSRs structure */
typedef struct _tlep_csr
{
	struct TLSB tlsb;	/* TLSB required CSRs */
	struct TLEP tlep;       /* TLEP specific CSRs */
} Tlep_csr;

/* TLMEM CSRs structure */
typedef struct _tlmem_csr
{
	struct TLSB tlsb;	/* TLSB required CSRs */
	struct TMEM$REG tmem;   /* TLMEM specific CSRs */
} Tmem_csr;

/*++
*  GLOBAL DOUBLE HEXAWORD STRUCTURE
*
*
*--
*/
struct QW_BUF {
    u_int LW[2];
    };

struct OW_BUF {
    struct QW_BUF qW[2];
    };

struct HW_BUF {
    struct OW_BUF OW[2];
    };

struct DHW_BUF {
    struct HW_BUF HW[2];
    };

struct Csr_modify {
	unsigned int addr;		/* Address of CSR */
	unsigned int wmask;             /* Modify bits for that CSR */
	unsigned int hwinit;            /* H/W inited read only bits */
};

struct Csr_error {
	unsigned int cause;		/* Address of CSR to cause error */
	unsigned int effect_adr;        /* Effect address (error CSR read) */
	unsigned int effect_bits;       /* Affected bits when error forced */
	unsigned int hook_adr;          /* Hook CSR address */
	unsigned int hook_bit;          /* Hook CSR bit to force error */
};

struct Tiop_err_regs {
	unsigned int regadr;		/* Physical Address of register */
	unsigned int value;		/* Data in register on error */
	unsigned int valid;		/* Valid error data mask (don't ignore) */
	char name[32];			/* Name of register for printing */
};


/*++
**************************************************************************
* NEON SPORT WARNING:
*	To make NEON sport work, a copy of this structure was copied into
*	$neon:lnp_c_routines.c.  If you change this structure, make sure
*	you copy it into $neon:lnp_c_routines.c.
**************************************************************************
**************************************************************************
*
*  GLOBAL VARIABLE DECLARATIONS
*  	Any inter-module GLOBAL variables should be allocatd from HEAP SPACE 
*	and not statically allocated within ROM CODE executable images.  
*	To accomplished this, use the typedef struct "globals_struct" and 
*	place your GLOBAL variables within this structure.   Later in the
*	programs initialize code section, memory will be malloc'ed per the
*	(sizeof externals). 
*--
*/

struct GLOBALS_STRUCT {
    /*
    * Main Pointers
    */
    struct CONFIG_STRUCT config_tbl[MAX_HOSE_COUNT];  

    u_int g$_hose_sel;		/* Physical hose under test for this TIOP 
				*	Note that this is not the 0-20 hose
				*	number used by the config routines,
				* 	but rather is limited to 0-3 for use 
				*	by mailbox routines
				*/

    /*
    * SPORT'S EXTENDED Error message Environment variables.
    * 	This area can/will be used during real world extended error printing
    */
    u_int          start_rese_area; /* Reserved portion info */
    u_int          end_rese_area;
    void           *nodespace;     /* Pointer to nodespace */
    int   	   quals_flag;
    u_int          dat;
    int            passcount;
    int            expect2;        /* Rest of 64 bit-ness */
    int            rcvd2;
    int            failadr2;
    int            exception_flag;
    int            powerup;        /* Powerup or not? */
    int            scratch;        /* Use this as scratch */

    /*
    * CPU Pointers
    */
    struct TLSB		*tlep_tlsb;	/* TLEP TLSB CSRs */
    struct TLEP 	*tlep_sp;	/* TLEP specific CSRs */

    /*
    * I/O module CSRs for TIOP/hose under test
    */
    struct TLSB 	*tiop_tlsb;	/* TIOP TLSB CSRs */
    struct TIOP 	*tiop_sp;       /* TIOP specific CSRs */

    volatile Tmem_csr	*tmem_ptr;	/* TMEM CSRs */

    /* MBX pointers */
    struct MBOX    *mb_ptr;		/* MailBox structure */
    struct LB_MBOX *lb_ptr;		/* Loopback MailBox structure */

    /*
    * LAMB Pointers and DMA buffers
    */
    struct LAMB$REG_STRUCT *lamb_node_csr;	/* LAMB Node    Space CSR Registers */
    struct LAMB$REG_STRUCT *lamb_private_csr;	/* LAMB Private Space CSR Registers */
    union  LAMB$REG_UNION  *lamb_mbox_csr;	/* MailBox mapped LAMB CSR Registers */

    /*
    * FLAG Pointers
    */
    struct FLAG$REG_STRUCT *flag_local_csr;	/* FLAG CSR Space Registers */
    union  FLAG$REG_UNION  *flag_mbox_csr;	/* FLAG Mailbox register map */

    /*
    * Miscellaneous global variables
    */
    u_int return_msg;		/* test result message to return to LDE */
    u_int cpu_node_id;		/* holds the CPU's node ID... return from whoami() */
    u_int tiop_node_id;		/* holds the TIOP under test's node ID */
    u_int sizing_done;		/* skips hose port sizing if already done */
    u_int seed;			/* linear congruential generator seed */
    u_int EEPROM_Write;		/* Enable EEPROM Diagnostic area writes */
    u_int enable_cleanup;	/* cmd line qual/param ok... do cleanup coding */
    u_int mb_timeout;		/* Capability to disable polling for loopback */
    u_int itiop_present;	/* Indicates module under test is an ITIOP */
    /*
    * The console does not guarantee what the next available SCB offset 
    * is so we must fumble and save the next free SCB offset for each MUT.
    */
    u_int SCB$L_TIOP;		/* holds FIRST free TIOP  SCB vector */
    u_int SCB$L_RBA;		/* holds NEXT  free RBA  SCB vector */

    u_int nrst_done;		/* Was a node reset performed on TIOP?  If so,
				*	check all HW CSRs after node reset,
				*	restore SW state.  Else, skip certain
				*	CSRs when checking init values
				*/

    void  *csrvals; 		/* Pointer to a number of Csr_modify 
				*	structures used to restore SW state
				*	after testing.  CSRs whose values are
				*	overwritten can be saved in this.
				*	The number of CSRs determines the
				*	size to malloc
				*/

				/* TIOP CSRs information to dump on error */
    struct Tiop_err_regs terr_regs[13]; 

				/* dedicated isr "MODE" for RBA tests */ 
    u_long hose_isr_mode; 
				/* dedicated isr "STATUS" for RBA tests */ 
    u_long hose_isr_status; 

				/* dedicated isr "MODE" for IOP */ 
    u_long IOP_isr_mode; 
				/* dedicated isr "STATUS" for IOP */ 
    u_long IOP_isr_status; 

    u_long LSB_isr_mode;	/* ISR mode for this process */

    u_int rm_arrays;		/* Boolean - true if RM arrays are present */
};    /* end STRUCT */
