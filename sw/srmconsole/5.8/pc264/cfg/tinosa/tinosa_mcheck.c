/* file:	pc264_mcheck.c
 *
 * Copyright (C) 1997, 1998 by
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
 *      16-Jul-1997
 *
 *  MODIFICATION HISTORY:
 *
 *	19-Jun-1998	ER	Added mcheck_handler_680().
 *	29-Apr-1998	ER	New and improved DC21272 CSR dump.
 *	18-Mar-1998	ER	Change for new system-specific logout frame definitions.
 *	27-Jan-1998	ER	Rework machine check parsing and handling
 *--
 */                                                

#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:common.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:alphascb_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$inc:prototypes.h"
#include 	"cp$src:ev6_pc264_logout_def.h"
#include 	"cp$src:platform_cpu.h"
#include	"cp$inc:platform_io.h"
#include	"cp$src:goby.h"

/* 
** Global Variable Definitions 
*/

struct HQE mchq_660;	/* Machine Check Handler Queue */

struct LOCK _align (QUADWORD) spl_mchk = {0, IPL_SYNC, 0, 0, 0, 0, 0};

uint8 ev_ecc_syndromes [] = {
        0xce,0xcb,0xd3,0xd5,0xd6,0xd9,0xda,0xdc,        /* data bits 0 to 7 */
        0x23,0x25,0x26,0x29,0x2a,0x2c,0x31,0x34,        /* data bits 8 to 15 */
        0x0e,0x0b,0x13,0x15,0x16,0x19,0x1a,0x1c,        /* data bits 16 to 23 */
        0xe3,0xe5,0xe6,0xe9,0xea,0xec,0xf1,0xf4,        /* data bits 24 to 31 */
        0x4f,0x4a,0x52,0x54,0x57,0x58,0x5b,0x5d,        /* data bits 32 to 39 */
        0xa2,0xa4,0xa7,0xa8,0xab,0xad,0xb0,0xb5,        /* data bits 40 to 47 */
        0x8f,0x8a,0x92,0x94,0x97,0x98,0x9b,0x9d,        /* data bits 48 to 55 */
        0x62,0x64,0x67,0x68,0x6b,0x6d,0x70,0x75,        /* data bits 56 to 63 */
        0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,        /* check bits 0 to 7 */
};

/* 
** External Data Definitions 
*/

extern int diag$r_evs;
extern struct LOCK spl_kernel;
extern int in_console;
extern int primary_cpu;

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
** Local Data Definitions
*/
struct logout logout_parse;

char *pchip_error_txt[] = {
	"Lost Error",
	"SERR",
	"PERR",
	"Retry Timeout as PCI Target",
	"Invalid Scatter/Gather Page Table Entry",
	"PCI Address Parity Error",
	"Target Abort",
	"PCI Read Data Parity Error",
	"No Devsel",
	"Retry Timeout as PCI Master",
	"Uncorrectable ECC Error",
	"Correctable ECC Error",
};

/*
** Forward Declarations 
*/

void pc264_pc_dump( void );
void pc264_csr_dump( struct logout *frame );
void pc264_cpu_dump( struct logout *frame );
void pc264_system_dump( int display, struct logout *frame );
void mcheck_handler( int vector );
void mcheck_handler_620( int vector );
void mcheck_handler_630( int vector );
int parse_error( struct common_logout in_common, struct sys_logout in_system, int report_correctable );
int machine_handle_nxm( struct logout *logout );

static int memtst( uint64 *p, uint64 n );


static int memtst( uint64 *p, uint64 n )
{
    while ( n > 0 )
	if ( *p++ )
	    return( 1 );
	else
	    n -= 8;
    return( 0 );
}


/*+
 * ============================================================================
 * = pc264_pc_dump - Display information concerned with the PC.               =
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
 *	pc264_pc_dump( ); 
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
void pc264_pc_dump( void )
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
    pprintf( " PC: %016X  PS: %016X\n",
		*( uint64 * )( acp->acx$q_pc ), 
		*( uint64 * )( acp->acx$q_ps ) );
/*
** Display exception registers
*/
    pprintf( " R2: %016X  R5: %016X\n",
		*( uint64 * )( acp->acx$q_exc_r2 ), 
		*( uint64 * )( acp->acx$q_exc_r5 ) );
    pprintf( " R3: %016X  R6: %016X\n",
		*( uint64 * )( acp->acx$q_exc_r3 ),
		*( uint64 * )( acp->acx$q_exc_r6 ) );
    pprintf( " R4: %016X  R7: %016X\n",
		*( uint64 * )( acp->acx$q_exc_r4 ),
		*( uint64 * )( acp->acx$q_exc_r7 ) );
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
 * = pc264_csr_dump - Dump the system specific CSRs.                          =
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
 *	pc264_csr_dump( logout_frame ); 
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *	struct logout *frame - Pointer to the PALcode logout frame
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void pc264_csr_dump( struct logout *frame )
{
    int reg;

    pprintf( "\nCPU %d\n", whoami( ) );
    pprintf( "GOBY CSRs:\n" );

	for (reg = 0; reg < 256; reg+=4)
	{
		if ((reg % 16) == 0)
			pprintf("\n");

		pprintf("%02x=%08x    ",reg,NBridgeReadPciConfig(GOBY_PCI_DEVICE_ID,reg,4));
	}
}


/*+
 * ============================================================================
 * = pc264_cpu_dump - Dump the CPU specific CSRs/IPRs.		              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will dump all the CPU CSRs and IPRs. The values 
 *	are extracted from the PALcode logout frame.
 *
 * FORM OF CALL:
 *
 *	pc264_cpu_dump( logout_frame ); 
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

void pc264_cpu_dump( struct logout *frame )
{
    pprintf ( "\nAlpha 21264 IPRs (CPU %d):\n", whoami( ) );

    pprintf( "I_STAT:         %016X    DC_STAT:        %016X\n",
		frame->common.i_stat, frame->common.dc_stat );
    pprintf( "C_ADDR:         %016X    DC1_SYNDROME:   %016X\n",
		frame->common.c_addr, frame->common.dc1_syndrome );
    pprintf( "DC0_SYNDROME:   %016X    C_STAT:         %016X\n",
		frame->common.dc0_syndrome, frame->common.c_stat );
    pprintf( "C_STS:          %016X    MM_STAT:        %016X\n",
		frame->common.c_sts, frame->common.mm_stat );

    if ( frame->common.sys_offset == sizeof( struct common_logout ) )
	return;

    pprintf( "EXC_ADDR:       %016X    IER_CM:         %016X\n",
		frame->exc_addr, frame->ier_cm );
    pprintf( "ISUM:           %016X    PAL_BASE:       %016X\n",
		frame->isum, frame->pal_base );
    pprintf( "I_CTL:          %016X    P_CTX:          %016X\n",
		frame->i_ctl, frame->process_context );
}


/*+
 * ============================================================================
 * = pc264_system_dump - Dump the entire state of the system.                 =
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
 *	pc264_system_dump( display, logout_frame ); 
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *	int display - display mask:
 *		      PC264$DISPLAY_CPU   = Display CPU registers.
 *		      PC264$DISPLAY_SYS   = Display System registers.
 *		      PC264$DISPLAY_PC    = Display PC info.
 *
 *	struct logout *frame - Pointer to the PALcode logout frame
 *
 * SIDE EFFECTS:                  
 *
 *      None                  
 *
-*/

void pc264_system_dump( int display, struct logout *frame )
{
    pprintf( "\nMachine Check Logout Frame @ 0x%x Code = 0x%x\n",
	      frame, frame->common.mchk_code );
/*
** Check if asked display the CPU registers
*/
    if ( display & PC264$DISPLAY_CPU )
	pc264_cpu_dump( frame );
/*         
** Check if asked display the System CSRs
*/                
    if ( display & PC264$DISPLAY_SYS )
	pc264_csr_dump( frame );
/*         
** Check if asked display the PC information
*/         
    if ( display & PC264$DISPLAY_PC )
	pc264_pc_dump( );
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
 *	System Detected
 *	660 (uncorrectable)
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
    int id;	  			/* CPU id */
    struct HQE *mqe;			/* Pointer to machine check 660 handler queue */
    struct logout *frame;		/* Pointer to the PALcode logout frame */
    struct PCB *pcb;			/* Pointer to a process context */
    struct ALPHA_CTX *acp;		/* Pointer to an Alpha hardware context */
                  
    id = whoami( );
/*                
** Synchronize machine checks
*/                
    spinlock( &spl_mchk );
    if ( mchq_660.flink == &mchq_660 ) {
/*                
**  If the primary is not in the console, and we
**  are not the primary, then don't complain.
*/                
	if ( !( in_console & (1 << primary_cpu ) ) && ( id != primary_cpu ) ) {
	    spinunlock( &spl_mchk );
	    mtpr_mces( 1 );
	    return;
	}         
    }             
/*                
** Get the current process/hardware context
*/                
    pcb = getpcb( );   
    acp = pcb->pcb$a_acp;
/*                
** Get the logout frame
*/
    frame = acp->acx$q_exc_r4[0] +
	    SLOT$_PAL_LOGOUT + id * SLOT$_PAL_LOGOUT_LEN;
/*
** See if anyone has handlers
*/
    for ( mqe = mchq_660.flink; mqe != &mchq_660; mqe = mqe->flink ) 
	mqe->handler( mqe->param, frame );
/*
** Do some system handlers
*/
    if ( parse_error( frame->common, frame->system, diag$r_evs & m_rpt_full ) == msg_failure ) {
	if ( vector == SCB$Q_PR_MCHK ) {
	    pprintf( "\n*** Unexpected Processor Machine Check through vector %3X\n", vector << 4 );
	    pc264_system_dump( PC264$DISPLAY_CPU | PC264$DISPLAY_SYS | PC264$DISPLAY_PC, frame );
	}
	else {
	    pprintf( "\n*** Unexpected System Machine Check through vector %3X\n", vector << 4 );
	    pc264_system_dump( PC264$DISPLAY_CPU | PC264$DISPLAY_SYS, frame );
	}
    }
/*
** Synchronize mchecks
*/
    spinunlock( &spl_mchk );
/*
** Clear the machine check in progress flag
*/
    mtpr_mces( 1 );
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
    int id;				    /* CPU id */
    struct crd_logout *frame;		    /* Pointer to the PALcode logout frame */
    struct PCB *pcb;			    /* Pointer to a process context */
    struct ALPHA_CTX *acp;		    /* Pointer to an Alpha hardware context */

    id = whoami( );
/*
** Synchronize machine checks 
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
    frame = acp->acx$q_exc_r4[0] +
	    SLOT$_PAL_LOGOUT + id * SLOT$_PAL_LOGOUT_LEN;
/*
** Display the information
*/                   
    if ( parse_error( frame->common, frame->system, diag$r_evs & m_rpt_full ) == msg_failure ) {
	pprintf( "\nSystem correctable error through vector %3X.\n",vector << 4 );
	pc264_system_dump( PC264$DISPLAY_CPU | PC264$DISPLAY_SYS, frame );
    }
/*
** Synchronize machine checks
*/
    spinunlock( &spl_mchk );
/*
** Clear the machine check flag
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
    int id;				    /* CPU id */
    struct crd_logout *frame;		    /* Pointer to the PALcode logout frame */
    struct PCB *pcb;			    /* Pointer to a process context */
    struct ALPHA_CTX *acp;		    /* Pointer to an Alpha hardware context */

    id = whoami( );
/*
** Synchronize machine checks
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
    frame = acp->acx$q_exc_r4[0] +    
	    SLOT$_PAL_LOGOUT + id * SLOT$_PAL_LOGOUT_LEN;
/*                                    
** Display the information            
*/                                    
    if ( parse_error( frame->common, frame->system, diag$r_evs & m_rpt_full ) == msg_failure ) {
	pprintf( "\nProcessor correctable error through vector %3X.\n", vector << 4 );     
	pc264_system_dump( PC264$DISPLAY_CPU, frame );
                                      
        if (frame->common.c_stat)
        {       
          extern void print_dimm_number(uint64 C_ADDR,int byte_number);
          
          if (frame->common.dc0_syndrome)
	     print_dimm_number(frame->common.c_addr,
                   get_byte_number_from_ecc(frame->common.dc0_syndrome));
          if (frame->common.dc1_syndrome)                               
  	     print_dimm_number(frame->common.c_addr,                   
                   get_byte_number_from_ecc(frame->common.dc1_syndrome));
        }                                  
    }               
/*                  
** Synchronize machine checks
*/                  
    spinunlock( &spl_mchk );
/*                  
** Clear the machine check flag
*/                                                               
    mtpr_mces( 4 );        
}                                                                                          


/*+                                                                           
 * ============================================================================
 * = mcheck_handler_680 - Machine check handler for vector 680.               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle machine checks directed to vector
 *	680 (system event).  This is excuted as a result of some
 *	type of system event, such as, power supply or fan failure,
 *	or an over temperature condition.  The vector is setup during 
 *	the idle process init.
 *
 * FORM OF CALL:           
 *           
 *	mchk_handler_680( ); 
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

void mcheck_handler_680( int vector )
{            
    int i, id;
    int *ptr;
    struct PCB *pcb;
    struct ALPHA_CTX *acp;
    struct logout *logout;
    struct sys_logout *sys_logout;

    id = whoami( );

    /* Synchronize */
    spinlock( &spl_mchk );

    /* Get the logout frame */
    pcb = getpcb( );
    acp = pcb->pcb$a_acp;
    logout = acp->acx$q_exc_r4[0] +
            SLOT$_PAL_LOGOUT + id * SLOT$_PAL_LOGOUT_LEN;

    sys_logout = ( uint64 )logout + logout->common.sys_offset;
    pprintf( "\nSystem Event Frame at %x, System-specific portion at %x\n", logout, sys_logout );
    for ( i = 0, ptr = ( int * )sys_logout;  i < 10;  i++, ptr += 2 ) {
    	pprintf( "%x : %08x.%08x\n", ptr, ptr[1], ptr[0] );
    }

    /* Synchronize */
    spinunlock( &spl_mchk );
}


/*+
 * ============================================================================
 * = parse_error - machine check parser.				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will parse all machine check errors.
 *
 * FORM OF CALL:
 *
 *	 parse_error( in_common, in_system, report_correctable ); 
 *
 * RETURNS:
 *
 *      int msg_success - Didn't find any errors.
 *
 * ARGUMENTS:
 *
 *	struct common_logout in_logout - common portion of logout frame.
 *	struct sys_logout in_system - system specific portion of logout frame.
 *	int report_correctable  - report correctable flag
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int parse_error( struct common_logout in_common, struct sys_logout in_system, int report_correctable )
{
    int id;
    int i, source, failing_bit;
    struct logout *logout;

    id = whoami( );
/*
**  Copy the supplied frames, keeping only error bits.
*/
    logout = &logout_parse;
    memset( logout, 0, sizeof( *logout ) );

    logout->common.i_stat = in_common.i_stat & 0x0000000060000000;
    logout->common.dc_stat = in_common.dc_stat & 0x000000000000000F;
    logout->common.c_stat = in_common.c_stat & 0x000000000000001F;
    logout->common.mm_stat = in_common.mm_stat & 0x0000000000000400;
                          
    logout->system.cchip_misc = in_system.cchip_misc & 0x0000000010000000;
    logout->system.pchip0_err = in_system.pchip0_err & 0x0000000000000FFF;
    logout->system.pchip1_err = in_system.pchip1_err & 0x0000000000000FFF;
/*
**  Check for known footprints, and clear them.
*/
    if ( logout->common.i_stat & 0x0000000060000000 ) {
	if ( report_correctable )
	    pprintf( "\nEV6 Icache Data or Tag Parity Error on CPU %d\n", id );
	logout->common.i_stat &= ~0x0000000060000000;
    }
    if ( logout->common.mm_stat & 0x0000000000000400 ) {
	if ( report_correctable )
	    pprintf( "\nEV6 Dcache Tag Parity Error on CPU %d\n", id );
	logout->common.mm_stat &= ~0x0000000000000400;
    }
    if ( logout->common.dc_stat & 0x0000000000000003 ) {
	pprintf( "\nEV6 Uncorrectable Dcache Tag Parity Error on CPU %d\n", id );
	logout->common.dc_stat &= ~0x0000000000000003;
    }
    if ( logout->common.c_stat & 0x0000000000000010 ) {
	pprintf( "\nUncorrectable Double Bit Error on CPU %d\n", id );
	logout->common.c_stat &= ~0x0000000000000010;
    }
    if ( logout->system.cchip_misc & 0x0000000010000000 ) {
	pprintf( "\nNon-existent memory address error from " );
	source = ( in_system.cchip_misc & 0x00000000e0000000 ) >> 29;
	if ( source <= 3 )
	    pprintf( "CPU %d\n", source );
	else if ( source == 4 )
	    pprintf( "Pchip 0\n" );
	else if ( source == 5 )
	    pprintf( "Pchip 1\n" );
	else
	    pprintf( "?\n" );
    }
    if ( logout->system.pchip0_err ) {
	pprintf( "\nError detected by Pchip 0\n" );
	pprintf( "    PERROR = %016x\n", in_system.pchip0_err );
	for ( i = 0; i < 12; i++ ) {
	    failing_bit = 1 << i;
	    if ( logout->system.pchip0_err & failing_bit ) {
		pprintf( "        %s\n", pchip_error_txt[i] );
 		logout->system.pchip0_err &= ~failing_bit;
	    }
	}
    }
    if ( logout->system.pchip1_err ) {
	pprintf( "\nError detected by Pchip 1\n" );
	pprintf( "    PERROR = %016x\n", in_system.pchip1_err );
	for ( i = 0; i < 12; i++ ) {
	    failing_bit = 1 << i;
	    if ( logout->system.pchip1_err & failing_bit ) {
		pprintf( "        %s\n", pchip_error_txt[i] );
 		logout->system.pchip1_err &= ~failing_bit;
	    }
	}
    }
/*
**  All bits in the frames should be clear now, or else there's
**  an error we don't recognize.
*/
    if ( memtst( logout, sizeof( *logout ) ) )
	return( msg_failure );
    else
	return( msg_success );
}


/*+
 * ============================================================================
 * = machine_handle_nxm - Platform specific nxm handler		              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle all platform specific work needed to clean
 *	up from a NXM.
 *
 * FORM OF CALL:
 *
 *	machine_handle_nxm( ); 
 *
 * RETURNS:
 *
 *      None
 *                                
 * ARGUMENTS:
 *
 *	struct logout *logout - address of the logout frame.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int machine_handle_nxm( struct logout *logout )
{
    int status;

    status = msg_failure;
    if ( logout->system.cchip_misc & 0x0000000000100000 ) {
	if ( ( ( logout->system.cchip_misc >> 22 ) & 0x3 ) == 2 )
	    logout->system.pchip0_err &= ~0x0000000000000140;
	else 
	    if ( ( ( logout->system.cchip_misc >> 22 ) & 0x3 ) == 3 )
		logout->system.pchip1_err &= ~0x0000000000000140;
	logout->system.cchip_misc &= ~0x0000000000100000;
	status = msg_success;
    }
    return( status );
}

/*
** =================== DIMM Decode code follows ================================
*/ 

int byte_number_error_count[8];
int bank_number_error_count[4];
int dimm_number_error_count[16];

int syndrome_75_to_byte[8] = {
	2,1,4,7,6,5,0,3};

int bit_to_syndrome[72] ={
	0xce,
	0xcb,
	0xd3,
	0xd5,
	0xd6,
	0xd9,
	0xda,
	0xdc,
	0x23,
	0x25,
	0x26,
	0x29,
	0x2a,
	0x2c,
	0x31,
	0x34,
	0x0e,
	0x0b,
	0x13,
	0x15,
	0x16,
	0x19,
	0x1a,
	0x1c,
	0xe3,
	0xe5,
	0xe6,
	0xe9,
	0xea,
	0xec,
	0xf1,
	0xf4,
	0x4f,
	0x4a,
	0x52,
	0x54,
	0x57,
	0x58,
	0x5b,
	0x5d,
	0xa2,
	0xa4,
	0xa7,
	0xa8,
	0xab,
	0xad,
	0xb0,
	0xb5,
	0x8f,
	0x8a,
	0x92,
	0x94,
	0x97,
	0x98,
	0x9b,
	0x9d,
	0x62,
	0x64,
	0x67,
	0x68,
	0x6b,
	0x6d,
	0x70,
	0x75,
	0x01,
	0x02,
	0x04,
	0x08,
	0x10,
	0x20,
	0x40,
	0x80};
/* Decoder for memory errors */
/* Tables:
Syndrome Hi or Lo determines low or high 127 bits
- sets a Low or High Flag
Decoded syndrome to bit in error, bit 3 determines even or odd byte
- sets the Odd or Even Flag
            
Decoder for DIMM Slot Number             
          
*/        
uint64 array_address_register[3];
void get_array_address_register(void)
{               
   array_address_register[0]= get_array_base(0);
   array_address_register[1]= get_array_base(1);
   array_address_register[2]= get_array_base(2);
}                          
                                               
void print_dimm_number(uint64 C_ADDR,int byte_number)
{               
  int dimm,i,j,bank=3,index[3]= {0,1,2};

    get_array_address_register();
    for (i=0;i<3;i++){ 
      array_address_register[i]&= ~(0xfffffful);
    }
    for (i=1;i<3;i++)
    if (array_address_register[index[i]]<array_address_register[index[0]])
    {     
      j= index[i];
      index[i]= index[0];
      index[0]= j;
    }     
    for (i=2;i<3;i++)
    if (array_address_register[index[i]]<array_address_register[index[1]])
    {     
      j= index[i];
      index[i]= index[1];
      index[1]= j;
    }
    if (array_address_register[index[3]]<array_address_register[index[2]])
    {
      j= index[3];
      index[3]= index[2];
      index[2]= j;      
    }                   
    dimm= ((byte_number & 1) << 1) + ((C_ADDR >> 4) & 1);
    for (i=0;i<2;i++)   
    { 
      if ((C_ADDR >= array_address_register[index[i]])
       & (array_address_register[index[i+1]]>C_ADDR))
      bank= index[i];    
    }                    
    pprintf("C_ADDR %x\nBank %d Dimm %d byte number %d in error\n",
	C_ADDR,bank,dimm,byte_number);
    byte_number_error_count[byte_number]++;
    bank_number_error_count[bank]++;
    dimm_number_error_count[(bank<<2) + dimm]++;
}                        
                               
                         
int get_byte_number_from_ecc(int low)
{                                                                                
int i,j,k,byte_number= -1;
                          
  j= 1;                   
    for(i= 0;i<8;i++)     
    {                     
      if (low==j) byte_number= i;
      j+= j;              
    }                     
    if (byte_number<0)    
    {                     
      byte_number= syndrome_75_to_byte[(low>>5) & 7];
    }                     
    return byte_number;   
}                         
                          
                          
                          
