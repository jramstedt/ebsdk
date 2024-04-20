/* file:	lx164_mcheck.c
 *
 * Copyright (C) 1996, 1997, 1998 by
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
 *      Alpha SRM Console Firmware
 *
 *  MODULE DESCRIPTION:     
 *
 *	Platform specific Machine Check and Exception handlers.
 *
 *  AUTHORS:
 *
 *	Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *      20-Nov-1996
 *
 *  MODIFICATION HISTORY:
 *
 *	16-Feb-1998	ER  Fixing I/O abort timeout problem when probing
 *			    SWXCR RAID controller.
 *	19-Mar-1997	ER  Re-work machine check handling
 *--
 */                                                

/* $INCLUDE_OPTIONS$ */
#include	"cp$inc:platform_io.h"
/* $INCLUDE_OPTIONS_END$ */
#include        "cp$src:kernel_def.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:alphascb_def.h"
#include 	"cp$src:alpha_def.h"
#include 	"cp$src:aprdef.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:msg_def.h"
/* Platform specific definition files */
#include        "cp$src:platform.h"
#include 	"cp$src:platform_cpu.h"
#include	"cp$src:pyxis_main_csr_def.h"
#include 	"cp$src:pyxis_mem_csr_def.h"

/*
** NOTE !!!!!
**
**  The undefine CC must be done to get a clean build
**
**  The file hwrpb_def.h defines the symbol CC which 
**  also gets defined by ev5_defs.sdl
**
**  We are in a catch 22, we can't change the ev5_defs
**  without affecting PALcode and we can't change hwrpb_def
**  without affecting the console source pool
*/
#undef  CC
#include	"cp$src:ev5_defs.h"

/*
** PALcode Machine Check Codes stored in logout frame.
** PALcode uses codes 0 - 255.
** Platform-specific codes from 256 and up.
*/
#define MchkBase	    0x200

#define mchk$c_corr_ecc	    MchkBase+0x01   /* Correctable ECC Error */
#define mchk$c_unc_ecc	    MchkBase+0x03   /* Uncorrectable ECC Error */
#define mchk$c_cpu_pe	    MchkBase+0x05   /* CPU Detected Parity Error */
#define mchk$c_mem_nem	    MchkBase+0x07   /* Non-Existent Memory Error */
#define mchk$c_pci_serr	    MchkBase+0x09   /* PCI System Error */
#define mchk$c_pci_perr	    MchkBase+0x0B   /* PCI Parity Error */
#define mchk$c_pci_adr_pe   MchkBase+0x0D   /* PCI Address Parity Error */
#define mchk$c_m_abort	    MchkBase+0x0F   /* PCI Master Abort Error */
#define mchk$c_t_abort	    MchkBase+0x11   /* PCI Target Abort Error */
#define mchk$c_pa_pte_inv   MchkBase+0x13   /* PCI Scatter/Gather PA PTE Invalid Error */
#define mchk$c_ioa_timeout  MchkBase+0x17   /* PCI I/O Abort Timeout Error */
#define mchk$c_isa_serr	    MchkBase+0x19   /* ISA System Error */
#define mchk$c_isa_iochk    MchkBase+0x1B   /* ISA IOCHK Error */

/*
** CPU Machine Check Codes
*/
#define mchk$c_tperr		0x80	    /* Tag Parity Error */
#define mchk$c_tcperr		0x82	    /* Tag Control Parity Error */
#define mchk$c_herr		0x84	    /* Hardware Error */
#define mchk$c_ecc_c		0x86	    /* Correctable ECC Error */
#define mchk$c_ecc_nc		0x88	    /* Non-Correctable ECC Error */
#define mchk$c_unknown		0x8A	    /* Unknown Error */
#define mchk$c_cacksoft		0x8C	    /* CACK Soft Error */
#define mchk$c_bugcheck		0x8E	    /* Bugcheck Error */
#define mchk$c_os_bugcheck	0x90	    /* OS Bugcheck Error */
#define mchk$c_dcperr		0x92	    /* Dcache Parity Error */
#define mchk$c_icperr		0x94	    /* Icache Parity Error */
#define mchk$c_retryable_ird	0x96	    /* Retryable Istream Read Error */
#define mchk$c_proc_hrd_err	0x98	    /* Processor Hard Error */
#define mchk$c_scperr		0x9A	    /* Scache Parity Error */
#define mchk$c_bcperr		0x9C	    /* Bcache Parity Error */

struct mchk_entry_type {
    int mchk_code;
    char *mchk_text;
} mchk_table[] = 
    { 
    mchk$c_corr_ecc,	    "Correctable ECC Error",
    mchk$c_unc_ecc,	    "Uncorrectable ECC Error",
    mchk$c_cpu_pe,	    "CPU Bus Parity Error",
    mchk$c_mem_nem,	    "Non-Existant Memory Error",
    mchk$c_pci_serr,	    "PCI Bus System Error (SERR#)",
    mchk$c_pci_perr,	    "PCI Bus Data Parity Error",
    mchk$c_pci_adr_pe,	    "PCI Bus Address Parity Error",
    mchk$c_m_abort,	    "PCI Bus Master Abort Error",
    mchk$c_t_abort,	    "PCI Bus Target Abort Error",
    mchk$c_pa_pte_inv,	    "Invalid PTE on Scatter/Gather Error",
    mchk$c_ioa_timeout,	    "I/O Read/Write Abort Timeout Error",
    mchk$c_isa_serr,	    "ISA Bus System Error (SERR#)",
    mchk$c_isa_iochk,	    "ISA Bus I/O Error (IOCHK#)",

    mchk$c_tperr,	    "CPU Tag Parity Error",
    mchk$c_tcperr,	    "CPU Tag Control Parity Error",
    mchk$c_herr,	    "CPU Hardware Error",
    mchk$c_ecc_c,	    "CPU Correctable ECC Error",
    mchk$c_ecc_nc,	    "CPU Non-Correctable ECC Error",
    mchk$c_unknown,	    "CPU Unknown Error",
    mchk$c_cacksoft,	    "CPU CACK Soft Error",
    mchk$c_bugcheck,	    "CPU Bugcheck Error",
    mchk$c_os_bugcheck,	    "CPU OS Bugcheck Error",
    mchk$c_dcperr,	    "CPU Dcache Parity Error",
    mchk$c_icperr,	    "CPU Icache Parity Error",
    mchk$c_retryable_ird,   "CPU Retryable I-Read Error",
    mchk$c_proc_hrd_err,    "CPU Processor Hard Error",
    mchk$c_scperr,	    "CPU Scache Parity Error",
    mchk$c_bcperr,	    "CPU Bcache Parity Error"
    };

/* 
** Global Variable Definitions 
*/
int MachineCheckFlag;
int ExpectMachineCheck = 0;
int ioa_timeout_flag;

struct HQE mchq_660;	/* Machine Check Handler Queue */

struct LOCK _align (QUADWORD) spl_mchk = {0, IPL_SYNC, 0, 0, 0, 0, 0};

int ev_single_bit_syn [] = {
	0xce,0xcb,0xd3,0xd5,0xd6,0xd9,0xda,0xdc,0x23,0x25,0x26,0x29,0x2a,   /* bits 0-12  */
	0x2c,0x31,0x34,0x0e,0x0b,0x13,0x15,0x16,0x19,0x1a,0x1c,0xe3,0xe5,   /* bits 13-25 */
	0xe6,0xe9,0xea,0xec,0xf1,0xf4,0x4f,0x4a,0x52,0x54,0x57,0x58,0x5b,   /* bits 26-38 */
	0x5d,0xa2,0xa4,0xa7,0xa8,0xab,0xad,0xb0,0xb5,0x8f,0x8a,0x92,0x94,   /* bits 39-51 */
	0x97,0x98,0x9b,0x9d,0x62,0x64,0x67,0x68,0x6b,0x6d,0x70,0x75,	    /* bits 52-63 */
	0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x00};			    /* check bits 0-7 */
/* 
** External Data Definitions 
*/

extern int diag$r_evs;
extern struct LOCK spl_kernel;

/*
** External Function Prototype Declarations
*/

/* From kernel_alpha.mar */
extern unsigned __int64 mfpr_mces( void );
extern void mtpr_mces( int value );
extern int spinlock( struct LOCK *spl_kernel );
extern int spinunlock( struct LOCK *spl_kernel );
extern struct PCB *getpcb( void );

/* From ie.c */
extern void walk_stack( int *fp, int *sp, __int64 pc, struct ALPHA_CTX *acp, int p_flag );

/* From alpha_pc.c */
extern int whoami( void );

/* From printf.c */
extern int pprintf( char *format, ... );

extern int do_bpt( void );

/*
** Forward Declarations 
*/

void lx164_unexpected_int( void );
void mcheck_handler( int vector );
void mcheck_handler_620( int vector );
void mcheck_handler_630( int vector );
void mcheck_save( struct MCHK$LOGOUT *mchk_frame, struct PAL$LOGOUT *logout_frame );
int ev_check_syndrome( unsigned int fl );


/*+
 * ============================================================================
 * = lx164_pc_dump - Display information concerned with the PC.               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	If an exception or interrupt is associated with a process
 *	or syncronized with the PC, this routine will display all
 *	the information that is associated with it.
 *
 * FORM OF CALL:
 *
 *	lx164_pc_dump( ); 
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void lx164_pc_dump( void )
{
    struct PCB *pcb;	    /* Pointer to process context */
    struct ALPHA_CTX *acp;  /* Pointer to Alpha hardware context */
    int *fp, *sp, pc;
/*
** Get the current process/hardware context
*/
    pcb = getpcb( );
    acp = pcb->pcb$a_acp;
/*
** Display the PC and associated information
*/
    pprintf( "\nProcess %s, pcb = %08X\n", pcb->pcb$b_name, pcb );
    pprintf( " pc: %016X  ps: %016X\n",
		*( unsigned __int64 * )( acp->acx$q_pc ), 
		*( unsigned __int64 * )( acp->acx$q_ps ) );
/*
** Display exception registers
*/
    pprintf( " r2: %016X  r5: %016X\n",
		*( unsigned __int64 * )( acp->acx$q_exc_r2 ), 
		*( unsigned __int64 * )( acp->acx$q_exc_r5 ) );
    pprintf( " r3: %016X  r6: %016X\n",
		*( unsigned __int64 * )( acp->acx$q_exc_r3 ),
		*( unsigned __int64 * )( acp->acx$q_exc_r6 ) );
    pprintf( " r4: %016X  r7: %016X\n",
		*( unsigned __int64 * )( acp->acx$q_exc_r4 ),
		*( unsigned __int64 * )( acp->acx$q_exc_r7 ) );
/*
** Walk the stack
*/
    fp = ( int * )acp->acx$q_r29[ 0 ];
    sp = ( int * )acp->acx$q_r30[ 0 ];
    pc = ( int )acp->acx$q_pc[ 0 ] - 4;

    walk_stack( fp, sp, pc, acp, 0 );
}


/*+
 * ============================================================================
 * = lx164_pyxis_dump - Dump the system specific CSRs.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will dump all the system specific CSRs including all
 *	I/O and memory CSRs. The values will come from the PALcode logout 
 *	frame.
 *
 * FORM OF CALL:
 *
 *	lx164_pyxis_dump( logout_frame ); 
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *	struct PAL$LOGOUT *logout_frame - Pointer to the PALcode logout frame
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
lx164_pyxis_dump( struct PAL$LOGOUT *logout_frame )
{
    unsigned int i;
    unsigned __int64 BaseCSR = pyxis_k_mem_csr_base;
/*
** Display the Pyxis Error Registers
*/
    pprintf( "\nPyxis Error Registers\n" );

    pprintf( "     ERR: %08X     STAT: %08X  ERR_MASK: %08X\n",
		*( unsigned int * )&( logout_frame->pyxis_err[0] ),
		*( unsigned int * )&( logout_frame->pyxis_stat[0] ),
		*( unsigned int * )&( logout_frame->pyxis_err_mask[0] ) );

    pprintf( " ECC_SYN: %08X     MEAR: %08X      MESR: %08X\n",
		*( unsigned int * )&( logout_frame->ecc_syn[0] ),
		*( unsigned int * )&( logout_frame->mear[0] ),
		*( unsigned int * )&( logout_frame->mesr[0] ) );

    pprintf( "PCI_ERR0: %08X PCI_ERR1: %08X  PCI_ERR2: %08X\n",
		*( unsigned int * )&( logout_frame->pci_err0[0] ),
		*( unsigned int * )&( logout_frame->pci_err1[0] ),
		*( unsigned int * )&( logout_frame->pci_err2[0] ) );
/*
** Display the Pyxis Memory Control Registers
*/
    pprintf( "\nPyxis Memory Control Registers\n" );

    pprintf( "MCR:   %08X\n", ( unsigned int )READ_IO_CSR( CSR_MCR ) );  

    for ( i = 0; i < LX164$MEM_TOTAL; i++ ) {
	pprintf( "BBAR%d: %08X\n", i, ( unsigned int )READ_MEM_CSR( i, CSR_BBAR ) );	
	pprintf( "BCR%d:  %08X\n", i, ( unsigned int )READ_MEM_CSR( i, CSR_BCR ) );
	pprintf( "BTR%d:  %08X\n", i, ( unsigned int )READ_MEM_CSR( i, CSR_BTR ) );
    }
}


/*+
 * ============================================================================
 * = lx164_cpu_dump - Dump the CPU specific CSRs/IPRs.		              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will dump all the CPU CSRs and IPRs. The values 
 *	are extracted from the PALcode logout frame.
 *
 * FORM OF CALL:
 *
 *	lx164_cpu_dump( logout_frame ); 
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *	struct PAL$LOGOUT *logout_frame - Pointer to the PALcode logout frame
 *
 * SIDE EFFECTS:
 *
 *      None
 *                                
-*/

void lx164_cpu_dump( struct PAL$LOGOUT *logout_frame )
{
    pprintf ("\nAlpha 21164 IPRs:\n");

    pprintf( "EXC_ADDR:    %016X EXC_SUM:     %016X\n",
			*( unsigned __int64 * )( logout_frame->exc_addr ),
			*( unsigned __int64 * )( logout_frame->exc_sum ) );

    pprintf( "EXC_MASK:    %016X ISR:         %016X\n",
			*( unsigned __int64 * )( logout_frame->exc_msk ),
			*( unsigned __int64 * )( logout_frame->isr ) );

    pprintf( "ICSR:        %016X IC_PERR_STAT:%016X\n",
			*( unsigned __int64 * )( logout_frame->icsr ),
			*( unsigned __int64 * )( logout_frame->ic_perr_stat ) );

    pprintf( "DC_PERR_STAT:%016X VA:          %016X\n",
			*( unsigned __int64 * )( logout_frame->dc_perr_stat ),
			*( unsigned __int64 * )( logout_frame->va ) );

    pprintf( "MM_STAT:     %016X SC_ADDR:     %016X\n",
			*( unsigned __int64 * )( logout_frame->mm_stat ),
			*( unsigned __int64 * )( logout_frame->sc_addr ) );

    pprintf( "SC_STAT:     %016X BC_TAG_ADDR: %016X\n",
			*( unsigned __int64 * )( logout_frame->sc_stat ),
			*( unsigned __int64 * )( logout_frame->bc_tag_addr ) );

    pprintf( "EI_ADDR:     %016X FILL_SYN:    %016X\n",
			*( unsigned __int64 * )( logout_frame->ei_addr ),
			*( unsigned __int64 * )( logout_frame->fill_syn ) );

    pprintf( "EI_STAT:     %016X LD_LOCK:     %016X\n",
			*( unsigned __int64 * )( logout_frame->ei_stat ),
			*( unsigned __int64 * )( logout_frame->ld_lock ) );

    pprintf( "PAL_BASE:    %016X\n",
			*( unsigned __int64 * )( logout_frame->pal_base ) );
}


/*+
 * ============================================================================
 * = lx164_system_dump - Dump the entire state of the system.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will display the CSRs and other pertinent information
 *	pertaining to the system.  A flag is passed in to tell the routine
 *	which sections of the system are to be displayed.
 *
 * FORM OF CALL:
 *                                
 *	lx164_system_dump( display, logout_frame ); 
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *	int display - display mask:
 *		      LX164$DISPLAY_CPU   = Display CPU registers.
 *		      LX164$DISPLAY_PYXIS = Display Pyxis registers.
 *		      LX164$DISPLAY_PC    = Display PC info.
 *
 *	struct PAL$LOGOUT *logout_frame - Pointer to the PALcode logout frame
 *
 * SIDE EFFECTS:                  
 *
 *      None
 *
-*/

void lx164_system_dump( int display, struct PAL$LOGOUT *logout_frame )
{
    unsigned int i;

    pprintf( "\nMachine Check Logout Frame @ 0x%x Code = 0x%x\n",
	      logout_frame, logout_frame->mchk_code[0] );

    for ( i = 0;  i * sizeof( struct mchk_entry_type ) < sizeof( mchk_table );  i++) {
    	if ( mchk_table[i].mchk_code == logout_frame->mchk_code[0] ) {
	    pprintf( "\nMachine Check Code - 0x%x %s\n",
		      mchk_table[i].mchk_code, mchk_table[i].mchk_text );
	    break;
	}
    }
/*
** Check if asked display the CPU registers
*/
    if ( display & LX164$DISPLAY_CPU )
	lx164_cpu_dump( logout_frame );
/*
** Check if asked display the Pyxis registers
*/
    if ( display & LX164$DISPLAY_PYXIS )
	lx164_pyxis_dump( logout_frame );
/*
** Check if asked display the PC information
*/
    if ( display & LX164$DISPLAY_PC )
	lx164_pc_dump( );
}


/*+
 * ============================================================================
 * = lx164_unexpected_int - Handle all unexpected interrupts.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle all unexpected interrupts.
 *
 * FORM OF CALL:
 *
 *	lx164_unexpected_int( ); 
 *
 * RETURNS:
 *
 *      None
 *                                
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void lx164_unexpected_int( void )
{
    pprintf("\n*** Unexpected interrupt .................\n");
    do_bpt();
}                                 


/*+                                                                           
 * ============================================================================
 * = mcheck_handler - Machine check handler.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle all machine checks.  The various machine
 *	check SCB vectors and their meaning are as follows:
 *
 *	PALcode offset 0x100 = Interrupt	
 *
 *	System Detected
 *	660 (uncorrectable)
 *
 *	PALcode offset 0x400 = Machine Check
 *
 *	Processor Detected
 *	670 (uncorrectable)
 *
 * FORM OF CALL:           
 *           
 *	mcheck_handler( vector ); 
 *
 * RETURNS:  
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      int vector - SCB vector 
 *
 * SIDE EFFECTS:
 *
 *      None
 *           
-*/

void mcheck_handler( int vector )
{        
    struct MCHK$LOGOUT mchk_frame;	/* Machine check frame */
    struct HQE *mqe;			/* Pointer to machine check 660 handler queue */
    struct PAL$LOGOUT *logout_frame;	/* Pointer to the PALcode logout frame */
    struct PCB *pcb;			/* Pointer to a process context */
    struct ALPHA_CTX *acp;		/* Pointer to an Alpha hardware context */
/*
** Syncronize machine checks
*/
    spinlock( &spl_mchk );
/*
** Get the current process/hardware context
*/
    pcb = getpcb( );   
    acp = pcb->pcb$a_acp;
/*
** Get the logout frame
*/
    logout_frame = SLOT$_PAL_LOGOUT
		    + whoami() * SLOT$_PAL_LOGOUT_LEN
		    + acp->acx$q_exc_r4[0];
/*
** Save some info in the machine check frame
*/
    mcheck_save( &mchk_frame, logout_frame );
/*
** See if anyone has handlers
*/
    spinlock( &spl_kernel );
    mqe = mchq_660.flink;
    spinunlock( &spl_kernel );
    while ( mqe != &( mchq_660 ) ) {
/*
** Call the handler
*/
	mqe->handler( mqe->param, &mchk_frame );
/*
** Get the next element
*/ 
	spinlock( &spl_kernel );
	mqe = mqe->flink;
	spinunlock( &spl_kernel );
    }
/*
** If this is an I/O abort timeout error, set a flag to fix a problem with 
** PCI probes to the SWXDR RAID controller.
*/
    if ( ( logout_frame->pyxis_err[0] & 0xFFFFFFFF ) & pyxis_err_m_ioa_timeout )
	ioa_timeout_flag = 1;
/*
** Do some system handlers
*/
    if ( ExpectMachineCheck == FALSE ) {
	if ( vector == SCB$Q_PR_MCHK ) {
	    pprintf( "\n*** Unexpected Processor Machine Check through vector %3X\n", vector << 4 );
	    lx164_system_dump( LX164$DISPLAY_CPU | LX164$DISPLAY_PYXIS | LX164$DISPLAY_PC, logout_frame );
	}
	else {
	    pprintf( "\n*** Unexpected System Machine Check through vector %3X\n", vector << 4 );
	    lx164_system_dump( LX164$DISPLAY_CPU | LX164$DISPLAY_PYXIS, logout_frame );
	}
/*
** Syncronize mchecks
*/
	spinunlock( &spl_mchk );
/*
** Set the machine check in progress flag
*/
	mtpr_mces( 1 );
/* 
** DO NOT RETURN on unexpected machine checks 
*/
	pprintf( "\n" );
	pprintf( "*** Console Stopped because of %3X Machine Check  ***\n", vector << 4 );
	pprintf( "*** Press HALT Button to return to console !! ***\n" );
	while ( 1 ) {
	    int i;
	    i = i << 1;
	}
    }
    else {
/*
** Syncronize machine checks
*/
	spinunlock( &spl_mchk );
/*
** Set the machine check in progress flag
*/
	mtpr_mces( 1 );
    }
}


/*+                                                                           
 * ============================================================================
 * = mcheck_handler_620 - Machine check handler for vector 620.               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle machine checks directed to vector
 *	620 (System correctable error).  This is executed as a result
 *	of a system correctable error.  This vector is setup during
 *	the idle process init.
 *
 * FORM OF CALL:           
 *           
 *	mcheck_handler_620( ); 
 *
 * RETURNS:  
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      int vector - SCB vector 
 *
 * SIDE EFFECTS:
 *
 *      None
 *           
-*/

void mcheck_handler_620( int vector )
{            
    struct PAL$LOGOUT_620 *logout_frame;    /* Pointer to the PALcode logout frame */
    struct PCB *pcb;			    /* Pointer to a process context */
    struct ALPHA_CTX *acp;		    /* Pointer to an Alpha hardware context */
/*
** Syncronize machine checks 
*/
    spinlock( &spl_mchk );
/*
** Get the current process/hardware context
*/
    pcb = getpcb( );   
    acp = pcb->pcb$a_acp;
/*
** Get the logout frame
*/
    logout_frame = SLOT$_PAL_LOGOUT
		    + whoami() * SLOT$_PAL_LOGOUT_LEN
		    + acp->acx$q_exc_r4[0];
/*
** Display the information
*/
    if ( diag$r_evs & m_rpt_full ) {
	pprintf( "\nSystem correctable error through vector %3X.\n",vector << 4 );

	pprintf( "\nPYXIS_ERR:  %08X  ECC_SYN:  %08X\n",
			*( unsigned int * )&( logout_frame->pyxis_err[0] ),
			*( unsigned int * )&( logout_frame->ecc_syn[0] ) );

	pprintf( "\nMEAR:       %08X  MESR:     %08X\n",
			*( unsigned int * )&( logout_frame->mear[0] ),
			*( unsigned int * )&( logout_frame->mesr[0] ) );

	pprintf( "\nMCES:       %016X\n", mfpr_mces( ) );
    }
/*
** Syncronize machine checks
*/
    spinunlock( &spl_mchk );
/*
** Set the machine check flag
*/
    mtpr_mces( 2 );
}


/*+                                                                           
 * ============================================================================
 * = mcheck_handler_630 - Machine check handler for vector 630.               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle machine checks directed to vector
 *	630 (Processor correctable error).  This is excuted as a result
 *	of a processor correctable error.  The vector is setup during
 *	the idle process init.
 *
 * FORM OF CALL:           
 *           
 *	mcheck_handler_630( ); 
 *
 * RETURNS:  
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      int vector - SCB vector 
 *
 * SIDE EFFECTS:
 *
 *      None
 *           
-*/

void mcheck_handler_630( int vector )
{            
    struct PAL$LOGOUT_630 *logout_frame;    /* Pointer to the PALcode logout frame */
    struct PCB *pcb;			    /* Pointer to a process context */
    struct ALPHA_CTX *acp;		    /* Pointer to an Alpha hardware context */
/*
** Syncronize machine checks
*/
    spinlock( &spl_mchk );
/*
** Get the current process/hardware context 
*/
    pcb = getpcb( );   
    acp = pcb->pcb$a_acp;
/*
** Get the logout frame
*/
    logout_frame = SLOT$_PAL_LOGOUT
		    + whoami() * SLOT$_PAL_LOGOUT_LEN
		    + acp->acx$q_exc_r4[0];
/*
** Display the information
*/
    if ( diag$r_evs & m_rpt_full ) {
	pprintf( "\nProcessor correctable error through vector %3X.\n", vector << 4 );

	pprintf( "EI_STAT:  %016X  EI_ADDR: %016X\n",
			*( unsigned __int64 * )( logout_frame->ei_stat ),
			*( unsigned __int64 * )( logout_frame->ei_addr ) );

	pprintf( "FILL_SYN: %016X  ISR:     %016X\n",
			*( unsigned __int64 * )( logout_frame->fill_syn ),
			*( unsigned __int64 * )( logout_frame->isr ) );

	if ( logout_frame->ei_stat[ 0 ] & 0x80000000 )
	    if ( ev_check_syndrome( *( unsigned int * )&( logout_frame->fill_syn[0] ) ) == msg_success )
		pprintf( "\nProcessor detected single bit ECC error in %s\n",
			( logout_frame->ei_stat[0] & 0x40000000 ) ? "Memory" : "Bcache" );
    }
/*
** Syncronize machine checks
*/
    spinunlock( &spl_mchk );
/*
** Set the machine check flag
*/
    mtpr_mces( 4 );
}


/*+
 * ============================================================================
 * = mcheck_save - Save some machine check information.                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will save information needed to parse machine
 *	checks. Parsing of the non CPU registers will be done using
 *	this frame.
 *
 * FORM OF CALL:
 *
 *	 mcheck_save( mchk_frame, logout_frame ); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct MCHK$LOGOUT *mchk_frame - pointer to the machine check frame.
 *	struct PAL$LOGOUT *logout_frame - pointer to the PALcode logout frame.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void mcheck_save( struct MCHK$LOGOUT *mchk_frame, struct PAL$LOGOUT *logout_frame )
{
    *( unsigned __int64 * )( mchk_frame->ei_stat ) =
    *( unsigned __int64 * )( logout_frame->ei_stat );

    *( unsigned __int64 * )( mchk_frame->ei_addr ) =
    *( unsigned __int64 * )( logout_frame->ei_addr );

    *( unsigned __int64 * )( mchk_frame->pyxis_err ) =
    *( unsigned __int64 * )( logout_frame->pyxis_err );

    *( unsigned __int64 * )( mchk_frame->pyxis_stat ) =
    *( unsigned __int64 * )( logout_frame->pyxis_stat );

    *( unsigned __int64 * )( mchk_frame->pyxis_err_mask ) =
    *( unsigned __int64 * )( logout_frame->pyxis_err_mask );

    *( unsigned __int64 * )( mchk_frame->ecc_syn ) =
    *( unsigned __int64 * )( logout_frame->ecc_syn );

    *( unsigned __int64 * )( mchk_frame->mear ) =
    *( unsigned __int64 * )( logout_frame->mear );

    *( unsigned __int64 * )( mchk_frame->mesr ) =
    *( unsigned __int64 * )( logout_frame->mesr );

    *( unsigned __int64 * )( mchk_frame->pyxis_err_data ) =
    *( unsigned __int64 * )( logout_frame->pyxis_err_data );

    *( unsigned __int64 * )( mchk_frame->pci_err0 ) =
    *( unsigned __int64 * )( logout_frame->pci_err0 );

    *( unsigned __int64 * )( mchk_frame->pci_err1 ) =
    *( unsigned __int64 * )( logout_frame->pci_err1 );

    *( unsigned __int64 * )( mchk_frame->pci_err2 ) =
    *( unsigned __int64 * )( logout_frame->pci_err2 );

    *( unsigned __int64 * )( mchk_frame->sio_nmisc ) =
    *( unsigned __int64 * )( logout_frame->sio_nmisc );
}


/*+
 * ============================================================================
 * = ev_check_syndrome - Check the ECC syndrome bits.                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will check the ECC syndrome bits for a single
 *	bit or uncorrectable error.
 *
 * FORM OF CALL:
 *
 *	 ev_check_syndrome( logout_frame ); 
 *
 * RETURNS:
 *
 *      msg_success - If its a single bit error.
 *      msg_failure - If its an uncorrectable error.
 *
 * ARGUMENTS:
 *
 *	unsigned int fl - contents of FILL_SYN register
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

#define EV5$_FILL_SYN_LO 0x000000ff
#define EV5$_FILL_SYN_HI 0x0000ff00

int ev_check_syndrome( unsigned int fl )
{
    int *tp;	/* Syndrome table pointer */
    int synv;	/* Syndrome value */
/*
** If the low syndrome is in the table return a success
*/
    tp = ev_single_bit_syn;
    synv = fl & EV5$_FILL_SYN_LO;
    while ( *tp )
	if ( *tp++ == synv )
	    return ( msg_success );
/*
** If the high syndrome is in the table return a success
*/
    tp = ev_single_bit_syn;
    synv = ( fl & EV5$_FILL_SYN_HI ) >> 8;
    while ( *tp )
	if ( *tp++ == synv )
	    return ( msg_success );
/*
** Else, it's not a single bit error
*/
    return ( msg_failure );
}
