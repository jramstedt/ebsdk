/*
 * file:	dp264_info.c
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
 *
 *
 * Abstract:	This file dumps out the information about the machine
 *              state, the PAL, the system, and the console itself.
 *
 * Author:	Eric Rasmussen
 *
 * Modifications:
 *
 *	er	09-Sep-1998	Added info 8 - dump_tsunami_csrs().
 *	er	13-Jul-1998	Adapted from Rawhide information.c
 *
 */

#include    "cp$src:platform.h"
#include    "cp$src:common.h"
#include    "cp$inc:prototypes.h"
#include    "cp$src:alpha_def.h"
#include    "cp$src:kernel_def.h"
#include    "cp$src:pb_def.h"
#include    "cp$src:sb_def.h"
#include    "cp$src:ub_def.h"
#include    "cp$src:dynamic_def.h"
#include    "cp$src:stddef.h"
#include    "cp$src:msg_def.h"
#include    "cp$src:ev_def.h"
#include    "cp$src:mem_def.h"
#include    "cp$src:hwrpb_def.h"
#include    "cp$src:pal_def.h"
#include    "cp$src:impure_def.h"
#include    "cp$src:alphascb_def.h"
#include    "cp$src:ctype.h"
#include    "cp$src:gct.h"
#include    "cp$src:fru50_def.h"

#include    "cp$src:platform_cpu.h"
#include    "cp$inc:platform_io.h"
#include    "cp$src:tsunami.h"
#include    "cp$src:tt_def.h"
#include    "cp$src:sym_def.h"
#include    "cp$src:aprdef.h"

#define DEBUG_FRU5	1

#define DTYPE 1
#define QTYPE 2

#undef SCB_ENTRIES
#define SCB_ENTRIES 512

extern struct HWRPB *hwrpb;
extern config_tree;
extern int cpu_mask;
extern int ___verylast;
extern int console_mode[];
extern unsigned __int64 mem_size;
extern int spl_kernel;
extern int primary_cpu;
extern int heap_size;
extern struct QUEUE _align (QUADWORD) pollq;	/* poll queue */
extern __int64 memory_high_limit;
extern struct QUEUE tt_pbs;
extern struct TTPB *console_ttpb;
extern struct SCBV *scbv; 
extern char *isp1020_fw_version;

extern int read_with_prompt( 
    char *prompt, 
    int maxbuf, 
    char *buf,
    struct FILE *fp_in, 
    struct FILE *fp_out, 
    int echo );

extern void show_version( );
extern int getbit64( unsigned int pos, unsigned __int64 base );
extern int tt_poll( );
 

struct HLTCOD {
    char *txt;
    int  code;
};

struct HLTCOD hlttxt[] = {
      "RESET",                 0,
      "HW_HALT (^P)",          1,
      "KSP_INVAL",             2,
      "SCBB_INVAL",            3,
      "PTBR_INVAL",            4,
      "SW_HALT (Halt inst)",   5,
      "DBL_MCHK",              6,
      "MCHK_FROM_PAL",         7,
      "START",                64,
      "CALLBACK",             66,
      "MPSTART",              68,
      "UNKNOWN",            0xFF 
};

struct csr_table {
    char *name;
    __int64 offset;
};

extern struct csr_table *dc21272_tables[4];
extern char *dc21272_names[4];

void about_the_console( void );
void dump_bitmap( void );
void dump_pal_info( void );
void dump_short_imp( void ); 
void dump_long_imp( void );
void dump_logout( void );
void hwrpb_parallel( void );
void dump_tsunami_csrs( void );
void dump_page_tables( void );
void dump_console( void );
void dump_scb( void );
void showfrutree( void );

struct ITAB {
  void (*rtn)();
  char *label;
};

struct ITAB itab[] = {
  { about_the_console, "About the console" },
  { dump_bitmap, "Bitmap" },
  { dump_pal_info, "PAL symbols" },
  { dump_short_imp, "IMPURE area (abbreviated)" },
  { dump_long_imp, "IMPURE area (full)" },
  { dump_logout, "LOGOUT area" },
  { hwrpb_parallel, "Per Cpu HWRPB areas" },
  { dump_page_tables, "Page Tables" },
  { dump_tsunami_csrs, "DECchip 21272 CSRs" },
  { dump_console, "Console internals" },
  { dump_scb, "Console SCB" },
#if DEBUG_FRU5
  { showfrutree, "GCT/FRU 5" },
#endif
  { 0, 0 }
};

#if DEBUG_FRU5
#define itab_items	12
#else
#define itab_items	11
#endif


void printrange( int state, int start_pfn, int end_pfn )
{
   char *page_status[] = { "BAD ","GOOD" };
   unsigned __int64 start_addr, end_addr;

   start_addr = ( unsigned __int64 )start_pfn;
   start_addr = start_addr * HWRPB$_PAGESIZE;
   end_addr = ( unsigned __int64 )end_pfn;
   end_addr = end_addr * HWRPB$_PAGESIZE;
   printf( "%s from page %x:%x (address range %016x:%016x) \n",
             page_status[state], start_pfn, end_pfn, start_addr, end_addr );
}


/*+
 * ============================================================================
 * = show_bitmap - compressed display of bits of a bitmap                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Display bits in a compressed display.   This is meant to be used by
 *      PFN bitmap and MEMDSC bitmap display routines.
 *	                    
 * FORM OF CALL:
 *  
 *	showbitmap( int *bitmap[], int bitmapsiz, int start_pfn )
 *  
 * RETURNS:
 *
 *	NONE                            
 *       
 * ARGUMENTS:
 *
 *     bitmap - pointer to a base of bits that are interpreted as a bitmap
 *     bitmapsiz - the number of bits in the bitmap
 *     start_pfn - system pfn that the first bit of the bitmap corresponds to.
 *
-*/
void showbitmap( unsigned __int64 bitmap, int bitmapsiz, int start_pfn )
{
    unsigned int i;
    unsigned int cnt;
    int state = 0;
    int bit;
    
    cnt = 1;
    state = getbit64( 0, bitmap );
    for ( i = 1; i < bitmapsiz; i++ ) {
	if ( killpending() ) 
	    return;
	bit = getbit64( i, bitmap );
	if ( state == bit ) {
	    cnt = cnt + 1;
	} else {			/* state changed */
	    printrange( state, start_pfn + i - cnt, start_pfn + i - 1);
	    cnt = 1;
	    state = bit;
	}
    }
    i = bitmapsiz;
    printrange( state, start_pfn + i - cnt, start_pfn + i - 1);
}


/*+
 * ============================================================================
 * = showmemdesc - display information about the memory descriptor structure  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Display information (mostly for debug) about the memory descriptor
 *      structure.
 *  
 * FORM OF CALL:
 *  
 *	showmemdesc()                                          
 *  
 * RETURNS:
 *
 *	NONE
 *       
 * ARGUMENTS:
 *       
 *      NONE
 *
-*/
void showmemdesc( void )
{
    struct MEMDSC *memdsc;
    int i;

    if ( hwrpb ) {
	memdsc = ( int )hwrpb + ( int )*hwrpb->MEM_OFFSET;                     
	printf( "\nHWRPB: %x    MEMDSC:%x   Cluster count: %d\n",
		    hwrpb, memdsc, *memdsc->CLUSTER_COUNT );

	for ( i = 0; i < *memdsc->CLUSTER_COUNT; i++ ) {
	    printf( "\nCLUSTER: %d   ", i ); 
	    if ( *memdsc->CLUSTER[i].USAGE == 1 ) {
		printf( "Usage: Console\n" );
		printf( "BITMAP_VA: n/a  BITMAP_PA: n/a  " );
	    }  
	    else if ( *memdsc->CLUSTER[i].USAGE == 0 ) {
		printf( "Usage: System\n" );
		printf( "BITMAP_VA: %x  BITMAP_PA: %16x  ",
			*memdsc->CLUSTER[i].BITMAP_VA,
			( *( unsigned __int64 * )memdsc->CLUSTER[i].BITMAP_PA ) );
	    }
	    else {
		printf( "*** Unknown Cluster type ***\n" );
	    }
	    printf( "START_PFN: %x  PFN_COUNT:%x \n", 
		    *memdsc->CLUSTER[i].START_PFN,
		    *memdsc->CLUSTER[i].PFN_COUNT );
	    if ( *memdsc->CLUSTER[i].USAGE == 0 ) {
		showbitmap( *( unsigned __int64 * )memdsc->CLUSTER[i].BITMAP_PA,
			    *memdsc->CLUSTER[i].PFN_COUNT,
			    *memdsc->CLUSTER[i].START_PFN );
	    }
	}
    }                         
}      


void imp_out( char * label, int offset, int size )
{
    struct impure *IMPURE;
    int i, j;
    char fieldname[20];
    int *impadr;    
    int *x;  

    for ( i = 0; i < size; i++ ) {
	memset( fieldname, 0x20, 20 );  /* fill with spaces */
	memcpy( fieldname, label, strlen( label ) ); 
	if ( i == 1 ) {
	    x = ( int )fieldname + strlen( label );
	    memcpy( x, "+4", 2 );
	}        
	printf( "%20.20s", fieldname );
	for ( j = 0; j < MAX_PROCESSOR_ID; j++ ) {                     
	    if ( ( 1 << j ) & cpu_mask ) {
		IMPURE = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE + 
		            ( j * PAL$IMPURE_SPECIFIC_SIZE );
		impadr = ( int )IMPURE + offset;
		printf( " %08x", impadr[i] );                  
	    }
	}
	printf( " : %04x", offset + ( i * 4 ) );
	printf( "\n" );
    }                             
}


void crd_out( char *label, int offset, int size )
{
    struct crd_logout *crd_frame;
    int i, j;
    char fieldname[25];
    int *mchkadr;    
    int *x;  

    for ( i = 0; i < size; i++ ) {
	memset( fieldname, 0x20, 25 );  /* fill with spaces */
	memcpy( fieldname, label, strlen( label ) ); 
	if ( i == 1 ) {
	    x = ( int )fieldname + strlen( label );
	    memcpy( x, "+4", 2 );
	}        
	printf( "%25.25s", fieldname );
	for ( j = 0; j < MAX_PROCESSOR_ID; j++ ) {                     
	    if ( ( 1 << j ) & cpu_mask ) {
		crd_frame = SLOT$_PAL_LOGOUT + mchk$crd_base + ( j * SLOT$_PAL_LOGOUT_LEN );
		mchkadr = ( int )crd_frame + offset;
		printf( " %08x", mchkadr[i] );                  
	    }
	}
	printf( " : %04x", offset + ( 4 * i ) + mchk$crd_base );
	printf( "\n" );
    }
}


void lg_out( char *label, int offset, int size )
{
    struct logout *logout_frame;
    int i, j;
    char fieldname[25];
    int *mchkadr;    
    int *x;  

    for ( i = 0; i < size; i++ ) {
	memset( fieldname, 0x20, 25 );  /* fill with spaces */
	memcpy( fieldname, label, strlen( label ) ); 
	if ( i == 1 ) {                               
	    x = ( int )fieldname + strlen( label );
	    memcpy( x, "+4", 2 );
	}
	printf( "%25.25s", fieldname );
	for ( j = 0; j < MAX_PROCESSOR_ID; j++ ) {
	    if ( ( 1 << j ) & cpu_mask ) {
		logout_frame = SLOT$_PAL_LOGOUT + mchk$mchk_base + ( j * SLOT$_PAL_LOGOUT_LEN );
		mchkadr = ( int )logout_frame + offset;
		printf( " %08x", mchkadr[i] );                  
	    }
	}
	printf( " : %04x", offset + ( 4 * i ) + mchk$mchk_base );
	printf( "\n" );
    }
}


void slot_out( char * label, int offset, int type )
{
    struct HWRPB *hwrpbp = hwrpb;
    struct SLOT *slot;
    int i, j;
    char fieldname[20];
    int *data_adr;    
    int *x;     
    int size;

    size = 2;
    if ( type == DTYPE )
	size = 1;

    for ( i = 0; i < size; i++ ) {
	memset( fieldname, 0x20, 20 );	/* fill with spaces */
	memcpy( fieldname, label, strlen( label ) ); 
	if ( i == 1 ) {
	    x = ( int )fieldname + strlen( label );
	    memcpy( x, "+4", 2 );
	}        
	printf( "%20.20s", fieldname );
	for ( j = 0; j < MAX_PROCESSOR_ID; j++ ) {                     
	    if ( ( 1 << j ) & cpu_mask ) {
		slot = ( void * )( ( int )hwrpbp + *hwrpbp->SLOT_OFFSET + 
					j * sizeof( struct SLOT ) );
		data_adr = ( int )slot + offset;
		printf( " %08x", data_adr[i] );                  
	    }
	}
	printf( "\n" );
    }
}


/*+
 * ============================================================================
 * = info - Display info about various console state.                         =
 * ============================================================================
 *
 *
 * OVERVIEW:
 *                              
 *   Display information about many system resources, machine state, and
 *   console parameters.
 *  
 * 
 * COMMAND FMT: info 2 Z 0 info
 *
 * COMMAND FORM:
 *
 *	info ( [n] )
 *
 * COMMAND TAG: info 0 RXBZ info
 *
 *   COMMAND ARGUMENT(S):
 *
 *	<n>	    - info number
 *
 * COMMAND OPTION(S):
 *
 * COMMAND EXAMPLE(S):
 *~
 *	>>>info
 *
 *~
 * FORM OF CALL:
 *  
 *	info ( int argc, char *argv[] )
 *  
 * RETURN CODES:
 *
 * ARGUMENTS:
 *
 * 	int argc 	- Number of arguments on the command line.
 *	char *argv[] 	- Array of pointers to argument strings.
 *
 * SIDE EFFECTS:
 *
 *
-*/

void info( int argc, char *argv[] )
{
    int	i;
    struct SLOT *slot;
    char ans[4];
    int selection = argv;

    if ( argc == 0 ) {
	itab[selection].rtn();
	return;  
    }
    if ( argc < 2 ) {
       i = 0;
       while ( itab[i].rtn ) {
         printf( "\t%2d. %s\n", i, itab[i].label );
         i++;
       }
       i = read_with_prompt( "Enter selection: ", sizeof( ans ), ans, 0, 0, 1 );
    } 
    else {
       strcpy( ans, argv[1] );
    }
    selection = atoi( ans );
    if ( ( selection < 0 ) || ( selection >= itab_items ) )
	return;
              
    itab[selection].rtn();
}


/*+
 * ============================================================================
 * = about_the_console - display information about the console                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *     Display information about the console.
 *             
 * FORM OF CALL:
 *  
 *     about_the_console();                                          
 *  
 * RETURNS:
 *
 *   	void                                             
 *       
 * ARGUMENTS:
 *       
 *      NONE.
 *
-*/

void about_the_console( void )
{  
    show_version( );

    printf( "Qlogic ISP1020 Firmware version %s\n", isp1020_fw_version );

    printf( "\nThis system utilizes two consoles, the SRM console for OpenVMS and Digital UNIX,\n" );
    printf( "and AlphaBIOS for Windows NT support. The OS_TYPE environment variable is used\n" );
    printf( "to select between the two consoles. If OS_TYPE is set to NT, the SRM console\n" );
    printf( "auto-loads AlphaBIOS shortly after I/O configuration and testing. To skip the\n" );
    printf( "AlphaBIOS auto-load, depress the front panel HALT button and reset your machine.\n" );
    printf( "The CONSOLE environment variable is used to select between graphics and serial\n" );
    printf( "display devices.\n\n" );
}


/*+
 * ============================================================================
 * = dump_bitmap - display information about the console bitmap structures    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *     This is the routine that implements the ">>> DUMP BITMAP" command.
 *  
 *  
 * FORM OF CALL:
 *  
 *     dump_bitmap();                                          
 *  
 * RETURNS:
 *
 *   	void                                             
 *       
 * ARGUMENTS:
 *       
 *      NONE.
 *
-*/
void dump_bitmap( void )
{
    showmemdesc( );
}


void dump_impure( int full )
{
    #define $L(y) imp_out( "y", &IMPURE->/**/y, 1 )
    #define $Q(y) imp_out( "y", &IMPURE->/**/y, 2 )

    char label[12];
    struct impure *IMPURE = 0;                    
    int i, j;                                    
    int gprbase, fprbase, ptbase;

    printf( "                    " ); 
    for ( j = 0; j < MAX_PROCESSOR_ID; j++ ) {           
	if ( ( 1 << j ) & cpu_mask ) {
	    printf( "  cpu%02d ", j );                  
	}
    }
    printf( "\n" );
    printf( "per_cpu impure area " ); 
    for ( j = 0; j < MAX_PROCESSOR_ID; j++ ) {          
	if ( ( 1 << j ) & cpu_mask ) {
	    printf( " %08x", PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE + 
		    ( j * PAL$IMPURE_SPECIFIC_SIZE ) );                  
	}
    }
    printf("\n");

    IMPURE = 0;
    imp_out( "cns$flag", &IMPURE->cns$flag, QTYPE );
    imp_out( "cns$hlt",  &IMPURE->cns$hlt,  QTYPE );
    if ( full ) {
	gprbase = &IMPURE->cns$gpr[0][0];
	for ( i = 0; i < 32; i++ ) { 
	    sprintf( label, "cns$gpr[%d]", i );
	    imp_out( label, gprbase + ( sizeof( REGDATA ) * i ) , QTYPE );
	} 
	fprbase = &IMPURE->cns$fpr[0][0];
	for ( i = 0; i < 32; i++ ) { 
	    sprintf( label, "cns$fpr[%d]", i );
	    imp_out( label, fprbase + ( sizeof( REGDATA ) * i ), QTYPE );
	} 
    }
    imp_out( "cns$mchkflag", &IMPURE->cns$mchkflag, QTYPE );
    if ( full ) {
	imp_out( "cns$pt", &IMPURE->cns$pt[0], QTYPE );
	imp_out( "cns$whami", &IMPURE->cns$whami, QTYPE );
	imp_out( "cns$scc", &IMPURE->cns$scc, QTYPE );
	imp_out( "cns$prbr", &IMPURE->cns$prbr, QTYPE );
	imp_out( "cns$ptbr", &IMPURE->cns$prbr, QTYPE );
	imp_out( "cns$trap", &IMPURE->cns$trap, QTYPE );
	imp_out( "cns$halt_code", &IMPURE->cns$halt_code, QTYPE );
	imp_out( "cns$ksp", &IMPURE->cns$ksp, QTYPE );
	imp_out( "cns$scbb", &IMPURE->cns$scbb, QTYPE );
	imp_out( "cns$pcbb", &IMPURE->cns$pcbb, QTYPE );
	imp_out( "cns$vptb", &IMPURE->cns$vptb, QTYPE );
	ptbase = &IMPURE->cns$pt_pad[0][0];
	for ( i = 0; i < 13; i++ ) { 
	    sprintf( label, "cns$pt_pad[%d]", i );
	    imp_out( label, ptbase + ( sizeof( REGDATA ) * i ), QTYPE );
	} 
	imp_out( "cns$shadow4",  &IMPURE->cns$shadow4,  QTYPE );
	imp_out( "cns$shadow5",  &IMPURE->cns$shadow5,  QTYPE );
	imp_out( "cns$shadow6",  &IMPURE->cns$shadow6,  QTYPE );
	imp_out( "cns$shadow7",  &IMPURE->cns$shadow7,  QTYPE );
	imp_out( "cns$shadow20", &IMPURE->cns$shadow20, QTYPE );
	imp_out( "cns$p_temp",   &IMPURE->cns$p_temp,   QTYPE );
	imp_out( "cns$p_misc",   &IMPURE->cns$p_misc,   QTYPE );
	imp_out( "cns$shadow23", &IMPURE->cns$shadow23, QTYPE );
    }
    imp_out( "cns$fpcr", &IMPURE->cns$fpcr, QTYPE );
    imp_out( "cns$va", &IMPURE->cns$va, QTYPE );
    imp_out( "cns$va_ctl", &IMPURE->cns$va_ctl, QTYPE );
    imp_out( "cns$exc_addr", &IMPURE->cns$exc_addr, QTYPE );
    imp_out( "cns$ier_cm", &IMPURE->cns$ier_cm, QTYPE );
    imp_out( "cns$sirr", &IMPURE->cns$sirr, QTYPE );
    imp_out( "cns$isum", &IMPURE->cns$isum, QTYPE );
    imp_out( "cns$exc_sum", &IMPURE->cns$exc_sum, QTYPE );
    imp_out( "cns$pal_base", &IMPURE->cns$pal_base, QTYPE );
    imp_out( "cns$i_ctl", &IMPURE->cns$i_ctl, QTYPE );
    imp_out( "cns$pctr_ctl", &IMPURE->cns$pctr_ctl, QTYPE );
    imp_out( "cns$process_context", &IMPURE->cns$process_context, QTYPE );
    imp_out( "cns$i_stat", &IMPURE->cns$i_stat, QTYPE );
    imp_out( "cns$dtb_alt_mode", &IMPURE->cns$dtb_alt_mode, QTYPE );
    imp_out( "cns$mm_stat", &IMPURE->cns$mm_stat, QTYPE );
    imp_out( "cns$m_ctl", &IMPURE->cns$m_ctl, QTYPE );
    imp_out( "cns$dc_ctl", &IMPURE->cns$dc_ctl, QTYPE );
    imp_out( "cns$dc_stat", &IMPURE->cns$dc_stat, QTYPE );
    imp_out( "cns$write_many", &IMPURE->cns$write_many, QTYPE );
    imp_out( "cns$cchip_misc", &IMPURE->cns$cchip_misc, QTYPE );
    imp_out( "cns$pchip0_err", &IMPURE->cns$pchip0_err, QTYPE );
    imp_out( "cns$pchip1_err", &IMPURE->cns$pchip1_err, QTYPE );
}

void dump_short_imp( void )
{
   dump_impure( 0 );
}                 

void dump_long_imp( void )
{
   dump_impure( 1 );
}


/*+
 * ============================================================================
 * = dump_pal_info - display information about the pal                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	dump_system_info   
 *
 * FORM OF CALL:
 *
 *	dump_system_info        
 *
 * RETURNS:
 *	
 *	                   
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	
 *
-*/                        
void dump_pal_info( void )
{
    #define $x(y) printf( "\t%08x\t%s \n", y, "y" )

    $x( PAL$HWRPB_BASE );
    $x( PAL$IMPURE_BASE );
    $x( PAL$IMPURE_COMMON_SIZE );
    $x( PAL$IMPURE_SPECIFIC_SIZE );
    $x( PAL$LOGOUT_BASE );
    $x( PAL$LOGOUT_SPECIFIC_SIZE );
    $x( PAL$TEMPS_BASE );
    $x( PAL$TEMPS_SPECIFIC_SIZE );
    $x( PAL$PAL_BASE );
    $x( PAL$PAL_SIZE );
    $x( PAL$OSFPAL_BASE );
    $x( PAL$OSFPAL_SIZE );
    $x( PAL$CONSOLE_BASE );
    $x( SLOT$_PAL_LOGOUT );
    $x( SLOT$_PAL_LOGOUT_LEN );
    $x( IMPURE$PCB_OFFSET );

    $x(mchk$cpu_base);
    $x(mchk$sys_base);
    $x(mchk$size);
    $x(mchk$crd_base);
    $x(mchk$mchk_base);
    $x(pal$logout_area);
}


void dump_logout( void )
{
    struct crd_logout *crd_frame = 0;
    struct logout *logout_frame = 0;
    int i, j;

    printf( "                       " ); 
    for ( j = 0; j < MAX_PROCESSOR_ID; j++ ) {                     
	if ( (1 << j ) & cpu_mask ) {
	    printf( "   cpu%02d ", j );                  
	}
    }
    printf( "\n" );
    printf( "per_cpu logout area      " ); 
    for ( j = 0; j < MAX_PROCESSOR_ID; j++ ) {                     
	if ( ( 1 << j ) & cpu_mask )  {
	    printf( " %08x", SLOT$_PAL_LOGOUT + j * SLOT$_PAL_LOGOUT_LEN );
	}
    }
    printf( "\n" );

#define $L(y) crd_out( "y", &crd_frame->/**/y, 1 )
#define $Q(y) crd_out( "y", &crd_frame->/**/y, 2 )

    $Q( mchk$crd_flag );
    $Q( mchk$crd_offsets );
    $Q( mchk$crd_mchk_code );
    $Q( mchk$crd_i_stat );
    $Q( mchk$crd_dc_stat );
    $Q( mchk$crd_c_addr );
    $Q( mchk$crd_dc1_syndrome );
    $Q( mchk$crd_dc0_syndrome );
    $Q( mchk$crd_c_stat );
    $Q( mchk$crd_c_sts );
    $Q( mchk$crd_mm_stat );

    $Q( mchk$crd_os_flags );
    $Q( mchk$crd_cchip_dir );
    $Q( mchk$crd_cchip_misc );
    $Q( mchk$crd_pchip0_err );
    $Q( mchk$crd_pchip1_err );

#define $L(y) lg_out( "y", &logout_frame->/**/y, 1 )
#define $Q(y) lg_out( "y", &logout_frame->/**/y, 2 )

    $Q( mchk$flag );
    $Q( mchk$offsets );
    $Q( mchk$mchk_code );

    $Q( mchk$i_stat );
    $Q( mchk$dc_stat );
    $Q( mchk$c_addr );
    $Q( mchk$dc1_syndrome );
    $Q( mchk$dc0_syndrome );
    $Q( mchk$c_stat );
    $Q( mchk$c_sts );
    $Q( mchk$mm_stat );
    $Q( mchk$exc_addr );
    $Q( mchk$ier_cm );
    $Q( mchk$isum );
    $Q( mchk$reserved_0 );
    $Q( mchk$pal_base );
    $Q( mchk$i_ctl );
    $Q( mchk$process_context );
    $Q( mchk$reserved_1 );
    $Q( mchk$reserved_2 );

    $Q( mchk$os_flags );
    $Q( mchk$cchip_dir );
    $Q( mchk$cchip_misc );
    $Q( mchk$pchip0_err );
    $Q( mchk$pchip1_err );
}


void hwrpb_parallel( void )
{
    struct SLOT *slot = 0;             
    struct HWRPB *hwrpbp = hwrpb;
    int i, j;

    if ( !hwrpb )
	return;

    printf( "                    " ); 
    for ( j = 0; j < MAX_PROCESSOR_ID; j++) {                     
	if ( ( 1 << j ) & cpu_mask )  {
	     printf( "  cpu%02d  ", j );                  
	}
    }
    printf( "\n" );
    printf( "per_cpu slot adr    " ); 
    for ( j = 0; j < MAX_PROCESSOR_ID; j++) {                     
	if ( ( 1 << j ) & cpu_mask ) {
	    slot = ( void * )( ( int )hwrpbp + *hwrpbp->SLOT_OFFSET + 
	                       j * sizeof( struct SLOT ) );
	    printf( " %08x", slot );                  
	}
    }
    printf( "\n" );
    slot = 0;
    slot_out( "VMS_HWPCB.KSP", &slot->HWPCB.VMS_HWPCB.KSP, QTYPE );
    slot_out( "VMS_HWPCB.ESP", &slot->HWPCB.VMS_HWPCB.ESP, QTYPE );
    slot_out( "VMS_HWPCB.SSP", &slot->HWPCB.VMS_HWPCB.SSP, QTYPE );
    slot_out( "VMS_HWPCB.USP", &slot->HWPCB.VMS_HWPCB.USP, QTYPE );
    slot_out( "VMS_HWPCB.PTBR", &slot->HWPCB.VMS_HWPCB.PTBR, QTYPE );
    slot_out( "VMS_HWPCB.ASN", &slot->HWPCB.VMS_HWPCB.ASN, QTYPE );
    slot_out( "VMS_HWPCB.ASTEN_SR", &slot->HWPCB.VMS_HWPCB.ASTEN_SR, QTYPE );
    slot_out( "VMS_HWPCB.FEN", &slot->HWPCB.VMS_HWPCB.FEN, QTYPE );
    slot_out( "VMS_HWPCB.CC", &slot->HWPCB.VMS_HWPCB.CC, QTYPE );
    slot_out( "Processor State", &slot->STATE, QTYPE );
    slot_out( "PAL_MEM_LEN", &slot->PAL_MEM_LEN, QTYPE );
    slot_out( "PAL_SCR_LEN", &slot->PAL_SCR_LEN, QTYPE );
    slot_out( "PAL_MEM_ADR", &slot->PAL_MEM_ADR, QTYPE );
    slot_out( "PAL_SCR_ADR", &slot->PAL_SCR_ADR, QTYPE );
    slot_out( "PAL_REV", &slot->PAL_REV, QTYPE );
    slot_out( "CPU_TYPE", &slot->CPU_TYPE, QTYPE );
    slot_out( "CPU_VAR", &slot->CPU_VAR, QTYPE );
    slot_out( "CPU_REV", &slot->CPU_REV, QTYPE );
    slot_out( "SERIAL_NUM", &slot->SERIAL_NUM[0], QTYPE );
    slot_out( "SERIAL_NUM", &slot->SERIAL_NUM[1], QTYPE );
    slot_out( "PAL_LOGOUT", &slot->PAL_LOGOUT, QTYPE );
    slot_out( "PAL_LOGOUT_LEN", &slot->PAL_LOGOUT_LEN, QTYPE );
    slot_out( "HALT_PCBB", &slot->HALT_PCBB, QTYPE );
    slot_out( "HALT_PC", &slot->HALT_PC, QTYPE );
    slot_out( "HALT_PS", &slot->HALT_PS, QTYPE );
    slot_out( "HALT_ARGLIST", &slot->HALT_ARGLIST, QTYPE );
    slot_out( "HALT_RETURN", &slot->HALT_RETURN, QTYPE );
    slot_out( "HALT_VALUE", &slot->HALT_VALUE, QTYPE );
    slot_out( "HALTCODE", &slot->HALTCODE, QTYPE );
    slot_out( "RSVD_SW", &slot->RSVD_SW, QTYPE );
    slot_out( "RXLEN", &slot->RXLEN, 1 ); 
    slot_out( "TXLEN", &slot->TXLEN, 1 ); 
}

int find_halt_text( int hltcod )
{
    int i = 0;
 
    while ( hlttxt[i].code != hltcod ) {
	if ( hlttxt[i].code == 0xFF ) {
	    break;
	}  
    i++;
    }
    return i;
}


void dump_console( void )
{
    struct HWRPB *hwrpbp;
    struct MEMDSC *memdsc;
    int *l1pt;
    int *l2pt;
    int *l3pt0;
    int *l3pt1;
    __int64 base;
    __int64 size;
    int i, j;
    struct TTPB *ttpb;      
    char *cmode[2] = {"INTERRUPT","POLLED"};
    struct POLLQ *pqp;
    struct SLOT *slot;
    int halt_code;

    printf( "___verylast: %08x\n", &___verylast );
    printf( "MIN_HEAP_SIZE : %08x\n", MIN_HEAP_SIZE );
    printf( "HEAP_SIZE : %08x\n", HEAP_SIZE );
    size = heap_size - MIN_HEAP_SIZE;
    printf( "Extended heap size : %08x\n", size );
    printf( "memory_high_limit : %x\n", memory_high_limit );

    printf( "MAX_ELBUF : %08x\n", MAX_ELBUF );
    printf( "MODULAR : %08x\n", MODULAR );
    printf( "SECURE : %08x\n", SECURE );
    printf( "SYMBOLS: %08x\n", SYMBOLS );
    printf( "XDELTA_ON : %08x\n", XDELTA_ON );
    printf( "XDELTA_INITIAL : %08x\n", XDELTA_INITIAL );

    if ( hwrpb ) {
	hwrpbp = hwrpb;
	memdsc = ( int )hwrpbp + *hwrpbp->MEM_OFFSET;
	l1pt = ( *memdsc->CLUSTER[0].PFN_COUNT - BOOT_PTEPAGES ) << 13;
	l2pt = ( int )l1pt + BOOT_L1PAGES  * 8192;
	l3pt0 = ( int )l2pt + BOOT_L2PAGES  * 8192;
	l3pt1 = ( int )l3pt0 + BOOT_L30PAGES * 8192;
	base =  ( int )l3pt1 + BOOT_L31PAGES * 8192;
	printf( "OS will be loaded at %08x\n", base );
    }

    if ( console_mode[primary_cpu] == INTERRUPT_MODE ) {
	printf( "Console IPL is low (INTERRUPT mode)\n" );
    } 
    else {
	printf( "Console IPL is high (POLLED mode)\n" );
    }

    if ( console_ttpb->mode == DDB$K_INTERRUPT ) {
	printf( "Console UART is in INTERRUPT mode\n" );
    } 
    else {
	printf( "Console UART is in POLLED mode\n" );
    }   

    printf( "\nTT ports known:\n" );
    spinlock( &spl_kernel );
    ttpb = tt_pbs.flink;
    spinunlock( &spl_kernel );
    while ( ttpb != &tt_pbs.flink ) {
        printf( "\ttt port: %s mode:%s\n", ttpb->ip->name, cmode[ttpb->mode] );
	spinlock( &spl_kernel );
	ttpb = ttpb->ttpb.flink;
	spinunlock( &spl_kernel );
    }

    /*
     * Walk down the polling queue.
     */
    printf( "\nPOLLQ contains:\n" );
    pqp = pollq.flink;
    while ( ( void * )pqp != ( void * )&pollq.flink ) {
	printf( "\tRoutine: %08x ", pqp->routine );
#if SYMBOLS
	printf( "%s", sym_table_lookup(pqp->routine) );
#endif
        if ( pqp->routine == ( int * )tt_poll ) {
           ttpb = pqp->param;
           printf( "  tt port: %s\n", ttpb->ip->name );
	} 
	else {
           printf( "\n" );
	}
	pqp = pqp->flink;
    }

    if ( hwrpb ) {
	printf( "\nLast Console Entry Reason:\n" );
	for ( j = 0; j < MAX_PROCESSOR_ID; j++ ) {                     
	    if ( ( 1 << j ) & cpu_mask ) {
		slot = ( void * )( ( int )hwrpbp + *hwrpbp->SLOT_OFFSET + 
				    j * sizeof( struct SLOT ) );
		halt_code = *slot->HALTCODE;
		i = find_halt_text( halt_code );
		printf( "    cpu%02d entry reason:%x  %s \n", j, halt_code, hlttxt[i].txt );                  
	    }
	}
    }
}


void display_pt_level( int level, int ptbase, unsigned __int64 vadr )
{
    int i;
    int ptadr;
    unsigned __int64 ptentry;
    int pfn, v, _for, fow, foe, asm;
    unsigned __int64 padr, lvadr;
    int nptes;
    int lfield[] = { 0, 33, 23, 13 };

    printf( "Level %d page table\n", level );
    nptes = 8192 / 8;
    for ( i = 0; i < nptes; i++ ) {
	check_kill( );
	ptadr = ptbase + ( i * 8 );
	ptentry = *( unsigned __int64 * )ptadr;
	if ( ptentry == 0 )
	    continue;
	pfn = ( ptentry >> 32 );
	padr = ( pfn * 8 * 1024 );
	lvadr = ( ( unsigned __int64 )i << lfield[level] ) + vadr;
	v = ptentry & 0x1;
	_for = ptentry & 0x2;
	fow = ptentry & 0x4;
	foe = ptentry & 0x8;
	asm = ptentry & 0x10;
	printf( "L%d %8x: %016x  pfn:%x (%x), virt:%016x\n", level, ptadr,
		ptentry, pfn, padr, lvadr );
	if ( ( pfn != ptbase ) && ( level != 3 ) ) {
	    display_pt_level( level + 1, padr, lvadr );
	}
    }
}

/*+
 * ============================================================================
 * = dump_page_tables - display the console created page tables               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Display information (mostly for debug) about the console created  
 *      page tables that are used at boot time to map various memory. 
 *  
 * FORM OF CALL:
 *  
 *	dump_page_tables();                                    
 *  
 * RETURNS:
 *
 *	void                                             
 *       
 * ARGUMENTS:
 *       
 *      NONE.
 *
-*/

void dump_page_tables( void )
{
   struct MEMDSC *memdsc;
   int *l1pt, *l2pt, *l3pt0, *l3pt1;
   int base;
   int ptbr;    

   if ( hwrpb ) {
	memdsc = ( int )hwrpb + ( int )*hwrpb->MEM_OFFSET;
	printf( "BOOT_BSSPAGES: %d (decimal)\n", BOOT_BSSPAGES );
	printf( "BOOT_STACKPAGES: %d (decimal)\n", BOOT_STACKPAGES );
	printf( "BOOT_GUARDPAGES: %d (decimal)\n", BOOT_GUARDPAGES );
	printf( "BOOT_L1PAGES: %d (decimal)\n", BOOT_L1PAGES );
	printf( "BOOT_L2PAGES: %d (decimal)\n", BOOT_L2PAGES );
	printf( "BOOT_L30PAGES: %d (decimal)\n", BOOT_L30PAGES );
	printf( "BOOT_L31PAGES: %d (decimal)\n", BOOT_L31PAGES );
	printf( "BOOT_PTEPAGES: %d (decimal)\n", BOOT_PTEPAGES );

	l1pt = ( *memdsc->CLUSTER[0].PFN_COUNT - BOOT_PTEPAGES ) << 13;
	l2pt = ( int )l1pt + BOOT_L1PAGES * 8192;
	l3pt0 = ( int )l2pt + BOOT_L2PAGES  * 8192;
	l3pt1 = ( int )l3pt0 + BOOT_L30PAGES * 8192;
	base =  ( int )l3pt1 + BOOT_L31PAGES * 8192;

	printf( "l1pt = %08x\n", l1pt );
	printf( "l2p2 = %08x\n", l2pt );
	printf( "l3pt0 = %08x\n", l3pt0 );
	printf( "l3pt1 = %08x\n", l3pt1 );
	printf( "base = %08x\n", base );

	read_ipr( APR$K_PTBR, &ptbr );
	printf( "ptbr = %08x  (%08x)\n", ptbr, ptbr << 13 );
	display_pt_level( 1, ( ptbr << 13 ), 0 );
    }
}


void dump_scb( void )
{
    int i;

    for ( i = 0; i < SCB_ENTRIES; i++ ) {
      if ( scbv[i].scbv$l_pd ){
         printf( "(%x) %x/  %x", &scbv[i], i, scbv[i].scbv$l_pd );
#if SYMBOLS
         printf( " = %s\n", sym_table_lookup( scbv[i].scbv$l_pd ) );
#else
	 printf( "\n" );
#endif
      }
   }
}

void dump_tsunami_csrs( void )
{
    int i, j;
    struct csr_table *t;

    for ( j = 0;  j < 4;  j++ ) {
    	pprintf( "\n%-7.7s CSRs: ", dc21272_names[j] );
	for ( t = dc21272_tables[j], i = 0;  t && t->name;  t++, i++ ) {
	    if ( i == 0 ) {
	    	pprintf( "%23.16x\n", t->offset & 0xFFFFFFFF000 );
	    }
	    pprintf( "%-20.20s %016X", t->name, ReadTsunamiCSR( t->offset ) );
	    pprintf( " : %04x\n", t->offset & 0xFFFF );
	}
	if ( i & 1 )
	    pprintf( "\n" );
    }
}

#if DEBUG_FRU5

char *node_types[] = {
    "",
    "Root",
    "HW Root",
    "SW Root", 
    "Template Root",
    "Community",
    "Partition",
    "SBB",
    "Pseudo",
    "CPU",
    "Memory Sub",
    "Memory Desc",
    "Memory Ctrl",
    "IOP",
    "Hose",
    "Bus",
    "I/O Ctrl",
    "Slot",
    "CPU Module",
    "Power/Environ",
    "FRU Root",
    "FRU Desc",
    "SMB",
    "CAB",
    "Chassis",
    "Expansion Chasis",
    "Interconnect Switch",
    ""
};

/*+
 * ============================================================================
 * = showfrutree - display information about the galaxy fru structure  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Display information (mostly for debug) about the galaxy fru
 *      structure.
 *  
 * FORM OF CALL:
 *  
 *	showfrutree()
 *  
 * RETURNS:
 *
 *	NONE
 *       
 * ARGUMENTS:
 *       
 *      NONE
 *
-*/
void showfrutree( void )
{
  int                      j = 0, i = 6, status, sp;
  GCT_ROOT_NODE            *pRoot;
  GCT_HANDLE               hw_root = 0, node = 0;
  GCT_NODE                 *pNode, *pNext, *pFnode, *gct_root;
  GCT_HD_EXT               *pHd_ext;
  GCT_FRU_DESC_NODE        *pFrudesc;
  GCT_BUFFER_HEADER        *pHeader;
  GCT_TLV                  *pTLV;
  GCT_SUBPACK              *pSubpack;
  SUBPKT_CPU_FRU5          *pCpu_fru5;
  SUBPKT_MEMORY_FRU5	   *pMemory_fru5;
  SUBPKT_BUSBRIDGE_FRU5	   *pBridge_fru5;
  SUBPKT_PCI_FRU5	   *pPci_fru5;
  SUBPKT_CACHE_FRU5	   *pCache_fru5;
  SUBPKT_POWER_FRU5        *pPower_fru5;
  SUBPKT_BUS_FRU5	   *pBus_fru5;
  SUBPKT_CSL_FRU5          *pCsl_fru5;
  SUBPKT_PCI_VPD_FRU5      *pVpd_fru5;
  SUBPKT_SMB_FRU5	   *pSmb_fru5;
  SUBPKT_RMC_FRU5	   *pRmc_fru5;
  SUBPKT_SENSOR_FRU5       *pSensor_fru5;
  char                     buffer[80];


  pHeader = (GCT_BUFFER_HEADER *) config_tree;
  gct_root = config_tree + GCT__BUFFER_HEADER_SIZE;
  pRoot = (GCT_ROOT_NODE *) ((GCT_NODE *)((char *) gct_root ));
  status = gct$find_node(0,&hw_root, FIND_BY_TYPE,NODE_HW_ROOT,  0, 0, 0, 0);

  printf("GCT_BUFFER_HEADER\n");
  printf("addr of config tree       %x\n",config_tree);
  printf("buffer_cksum              %x\n",pHeader->buffer_cksum);
  printf("buffer_size               %x\n",pHeader->buffer_size);
  printf("rev_major                 %x\n",pHeader->rev_major);
  printf("rev_minor                 %x\n",pHeader->rev_minor);
  printf("galaxy_enable             %x\n",pHeader->galaxy_enable);

  printf("\nGCT_ROOT_NODE\n");
  printf("Root->lock                %x\n",pRoot->lock);
  printf("Root->transient_level     %x\n",pRoot->transient_level);
  printf("Root->Current_level       %x\n",pRoot->current_level);
  printf("Root->console_req         %x\n",pRoot->console_req);
  printf("Root->min_alloc           %x\n",pRoot->min_alloc);
  printf("Root->min_align           %x\n",pRoot->min_align);
  printf("Root->base_alloc          %x\n",pRoot->base_alloc );
  printf("Root->base_align          %x\n",pRoot->base_align);
  printf("Root->max_phys_addr       %x\n",pRoot->max_phys_address);
  printf("Root->mem_size            %x\n",pRoot->mem_size);
  printf("Root->platform_type       %x\n",pRoot->platform_type);
  printf("Root->platform_name       %x\n",pRoot->platform_name);
  printf("Root->primary_instance    %x\n",pRoot->primary_instance);
  printf("Root->first_free          %x\n",pRoot->first_free);
  printf("Root->high_limit          %x\n",pRoot->high_limit);
  printf("Root->lookaside           %x\n",pRoot->lookaside);
  printf("Root->available           %x\n",pRoot->available);
  printf("Root->max_partition       %x\n",pRoot->max_partition);
  printf("Root->partitions          %x\n",pRoot->partitions);
  printf("Root->communities         %x\n",pRoot->communities);
  printf("Root->max_plat_partition  %x\n",pRoot->max_platform_partition);
  printf("Root->max_frag            %x\n",pRoot->max_fragments);
  printf("Root->max_desc            %x\n",pRoot->max_desc);
  printf("Root->galaxy_id           %x\n",pRoot->galaxy_id);
  printf("Root->bindings            %x\n\n",pRoot->bindings);

  /* Next printout section */
  /* make ptr to HW_ROOT   */
  pNode = _GCT_MAKE_ADDRESS(hw_root);
  printf("GCT Depth View:\n");
  
  pNext = pNode;
  while (pNext && (j < 2)) {

	status = gct$find_node(0,&node, FIND_ANY, pNext , 0, 0, 0, 0);
	pNext = _GCT_MAKE_ADDRESS(node);

	for (i=0; i< pNext->depth;i++) {
	    printf("    ");
	}
	printf("%s ID %016.016x ", node_types[pNext->type], pNext->id);
	if (pNext->subtype != 0)
	    printf("subtype %x ",pNext->subtype);
	if (pNext->hd_extension != 0) {
            printf("hd ext %x ",pNext->hd_extension);
            pHd_ext = &pNext->type + pNext->hd_extension;
            if (pHd_ext->fru != 0)
		printf("fru %x ",pHd_ext->fru);
        }
	if (pNext->type == 0x15) {
	    printf("handle %x ",node);
	}
	printf("\n");
	if (pNext->type == 2) j++;
	if (killpending()) return;
  }

  /* next printout section */
  buffer[0] = 'N';
  read_with_prompt("\ndump each node ? (Y/<N>) ",80,buffer,0,0,1);
  if (toupper(buffer[0]) == 'Y') {
	j = 0;
	/* make ptr to HW_ROOT */
	pNode = _GCT_MAKE_ADDRESS(hw_root);
	printf("GCT each node view:\n");
  	pNext = pNode;
	while (pNext && (j < 2)) {
		status = gct$find_node(0,&node, FIND_ANY, pNext , 0, 0, 0, 0);
		pNext = _GCT_MAKE_ADDRESS(node);
		printf("\nGCT_NODE:          %x\n", pNext);
		printf("->type             %x\n", ((GCT_NODE *) pNext)->type);
		printf("->subtype          %x\n", ((GCT_NODE *) pNext)->subtype);
		printf("->size             %x\n", ((GCT_NODE *) pNext)->size);
		printf("->hd_extension     %x\n", ((GCT_NODE *) pNext)->hd_extension);
		printf("->owner            %x\n", ((GCT_NODE *) pNext)->owner);
		printf("->current_owner    %x\n", ((GCT_NODE *) pNext)->current_owner);
		printf("->id               %16.16x\n",((GCT_NODE *) pNext)->id);
		printf("->node_flags       %x\n", ((GCT_NODE *) pNext)->node_flags);
		printf("->config           %x\n", ((GCT_NODE *) pNext)->config);
		printf("->affinity         %x\n", ((GCT_NODE *) pNext)->affinity);
		printf("->parent           %x\n", ((GCT_NODE *) pNext)->parent);
		printf("->next_sib         %x\n", ((GCT_NODE *) pNext)->next_sib);
		printf("->prev_sib         %x\n", ((GCT_NODE *) pNext)->prev_sib);
		printf("->child            %x\n", ((GCT_NODE *) pNext)->child);
		printf("->depth            %x\n", ((GCT_NODE *) pNext)->depth);
		if (((GCT_NODE *)pNext)->hd_extension != 0) {
		    pHd_ext = &pNext->type + pNext->hd_extension;
		    printf("->fru_count        %x\n",pHd_ext->fru_count);
		    printf("->fru              %x\n",pHd_ext->fru);
		    printf("->subpkt_count     %x\n",pHd_ext->subpkt_count);
		    printf("->subpkt_offset    %x\n",pHd_ext->subpkt_offset);
		    if (pHd_ext->subpkt_offset) {
			pSubpack = &pNext->type + pHd_ext->subpkt_offset;
			for (sp=0; sp < pHd_ext->subpkt_count; sp++) {
			    printf("Subpkt->subpack_length %x\n",pSubpack->subpack_length);
			    printf("Subpkt->subpack_class  %x\n",pSubpack->subpack_class);
			    printf("Subpkt->subpack_type   %x\n",pSubpack->subpack_type);
			    printf("Subpkt->subpack_rev    %x\n",pSubpack->subpack_rev);	
			    switch (pSubpack->subpack_type) {
			    	case SUBPACK_PROCESSOR:
				    pCpu_fru5 = (SUBPKT_CPU_FRU5 *)pSubpack;
				    printf("Subpkt->processor_id        %x\n",pCpu_fru5->processor_id);
				    printf("Subpkt->processor_family    %x\n",pCpu_fru5->processor_family);
				    printf("Subpkt->cpu_state           %x\n",pCpu_fru5->cpu_state);
				    printf("Subpkt->ovms_pal            %x\n",pCpu_fru5->ovms_palcode_revision);
				    printf("Subpkt->dunix_pal           %x\n",pCpu_fru5->dunix_palcode_revision);
				    printf("Subpkt->wnt_pal             %x\n",pCpu_fru5->wnt_palcode_revision);
				    printf("Subpkt->alpha_type          %x\n",pCpu_fru5->alpha_type);
				    printf("Subpkt->procesor_variation  %x\n",pCpu_fru5->processor_variation);
				    pTLV = (GCT_TLV *)&pCpu_fru5->processor_manufacturer;
				    dump_tlv(pTLV);
				    _NEXT_TLV(pTLV);
				    dump_tlv(pTLV);
				    _NEXT_TLV(pTLV);
				    dump_tlv(pTLV);
				    _NEXT_TLV(pTLV);
				    break;
				case SUBPACK_MEMORY:
				    pMemory_fru5 = (SUBPKT_MEMORY_FRU5 *)pSubpack;
				    printf("Subpkt->mem_id            %x\n",pMemory_fru5->mem_id);
				    printf("Subpkt->register_count    %x\n",pMemory_fru5->register_count);
				    break;
				case SUBPACK_SYS_BUS_BRIDGE:
				    pBridge_fru5 = (SUBPKT_BUSBRIDGE_FRU5 *)pSubpack;
				    printf("Subpkt->bridge_level      %x\n",pBridge_fru5->bridge_level);
				    printf("Subpkt->bridge_type       %x\n",pBridge_fru5->bridge_type);
				    printf("Subpkt->register_count    %x\n",pBridge_fru5->register_count);
				    printf("Subpkt->physical_addr     %x\n",pBridge_fru5->physical_address);
				    break;
				case SUBPACK_PCI_DEVICE:
				    pPci_fru5 = (SUBPKT_PCI_FRU5 *)pSubpack;
				    printf("Subpkt->pci_config_addr   %x\n",pPci_fru5->pci_config_addr);
				    printf("Subpkt->space0            %x\n",pPci_fru5->pci_head.pci_space0);
				    printf("Subpkt->space1            %x\n",pPci_fru5->pci_head.pci_space1);
				    printf("Subpkt->space2            %x\n",pPci_fru5->pci_head.pci_space2);
				    printf("Subpkt->space3            %x\n",pPci_fru5->pci_head.pci_space3);
				    printf("Subpkt->space4            %x\n",pPci_fru5->pci_head.pci_space4);
				    printf("Subpkt->space5            %x\n",pPci_fru5->pci_head.pci_space5);
				    printf("Subpkt->space6            %x\n",pPci_fru5->pci_head.pci_space6);
				    printf("Subpkt->space7            %x\n",pPci_fru5->pci_head.pci_space7);
				    break;
				case SUBPACK_CACHE:
				    pCache_fru5 = (SUBPKT_CACHE_FRU5 *)pSubpack;
				    printf("Subpkt->cache_level       %x\n",pCache_fru5->cache_level);
				    printf("Subpkt->cache_speed       %x\n",pCache_fru5->cache_speed);
				    printf("Subpkt->cache_size        %x\n",pCache_fru5->cache_size);
				    printf("Subpkt->size_avail        %x\n",pCache_fru5->size_avail);
				    printf("Subpkt->cache_wp          %x\n",pCache_fru5->cache_wp);
				    printf("Subpkt->cache_ec          %x\n",pCache_fru5->cache_ec);
				    printf("Subpkt->cache_type        %x\n",pCache_fru5->cache_type);
				    printf("Subpkt->cache_state       %x\n",pCache_fru5->cache_state);
				    break;
				case SUBPACK_POWER:
				    pPower_fru5 = (SUBPKT_POWER_FRU5 *)pSubpack;
				    printf("Subpkt->fru_count         %x\n",pPower_fru5->fru_count);
				    printf("Subpkt->fru               %x\n",pPower_fru5->fru);
				    printf("Subpkt->ps_type           %x\n",pPower_fru5->ps_type);
				    printf("Subpkt->ps_id             %x\n",pPower_fru5->ps_id);
				    printf("Subpkt->ps_status         %x\n",pPower_fru5->ps_status);
				    break;
				case SUBPACK_BUS:
				    pBus_fru5 = (SUBPKT_BUS_FRU5 *)pSubpack;
				    printf("Subpkt->bus_id            %x\n",pBus_fru5->bus_id);
				    printf("Subpkt->register_count    %x\n",pBus_fru5->register_count);
				    break;
				case SUBPACK_CONSOLE:
				    pCsl_fru5 = (SUBPKT_CSL_FRU5 *)pSubpack;
				    printf("Subpkt->reset_reason      %x\n",pCsl_fru5->reset_reason);
				    printf("Subpkt->ev_count          %x\n",pCsl_fru5->ev_count);
				    pTLV = (GCT_TLV *)&pCsl_fru5->srm_console_part_number;
				    while (pTLV->tlv_tag) {
					dump_tlv(pTLV);
					_NEXT_TLV(pTLV);
				    }
				    break;
				case SUBPACK_PCI_VPD:
				    pVpd_fru5 = (SUBPKT_PCI_VPD_FRU5 *)pSubpack;
				    printf("Subpkt->load_id           %x\n", pVpd_fru5->load_id);
				    printf("Subpkt->ROM_level         %x\n", pVpd_fru5->ROM_level);
				    printf("Subpkt->ROM_rev           %x\n", pVpd_fru5->ROM_rev);
				    printf("Subpkt->net_addr          %x\n", pVpd_fru5->net_addr);
				    printf("Subpkt->dev_driv_level    %x\n", pVpd_fru5->dev_driv_level);
				    printf("Subpkt->diag_level        %x\n", pVpd_fru5->diag_level);
				    printf("Subpkt->load_ucode_level  %x\n", pVpd_fru5->load_ucode_level);
				    printf("Subpkt->bin_func_num      %x\n", pVpd_fru5->bin_func_num);
				    break;
				case SUBPACK_SMB:
				    pSmb_fru5 = (SUBPKT_SMB_FRU5 *)pSubpack;
				    printf("Subpkt->smb_id            %x\n", pSmb_fru5->smb_id);
				    printf("Subpkt->register_count %x\n", pSmb_fru5->register_count);
				    break;
				case SUBPACK_RMC:
				    pRmc_fru5 = (SUBPKT_RMC_FRU5 *)pSubpack;
				    printf("Subpkt->rmc_id            %x\n", pRmc_fru5->rmc_id);
				    printf("Subpkt->rmc_pic_hw_rev    %x\n", pRmc_fru5->rmc_pic_hw_rev);
				    printf("Subpkt->rmc_pic_fw_rev    %x\n", pRmc_fru5->rmc_pic_fw_rev);
				    break;
				case SUBPACK_SENSOR:
				    pSensor_fru5 = (SUBPKT_SENSOR_FRU5 *)pSubpack;
				    printf("Subpkt->fru_count         %x\n",pSensor_fru5->fru_count);
				    printf("Subpkt->fru_offset        %x\n",pSensor_fru5->fru_offset);
				    printf("Subpkt->console_id        %x\n",pSensor_fru5->console_id);
				    printf("Subpkt->sensor_class      %x\n",pSensor_fru5->sensor_class);
				    printf("Subpkt->sensor_prop       %x\n",pSensor_fru5->sensor_properties);
				    break;
			    	default:
				    printf("Unknown subpacket type = %x\n", pSubpack->subpack_type);
			    } /* end switch */

			    pSubpack = (char *)pSubpack + pSubpack->subpack_length;

			    if (killpending())
				return;
	
			}  /* end for-loop */
		    }  /* end if */
	
		    if (pHd_ext->fru) {
			pFnode = _GCT_MAKE_ADDRESS(pHd_ext->fru);
			printf("node FRU Desc addr %x:\n",pFnode);
			printf("->type       %x\n",((GCT_NODE *) pFnode)->type);
			printf("->subtype    %x\n",((GCT_NODE *) pFnode)->subtype);
			printf("->size       %x\n",((GCT_NODE *) pFnode)->size);
			printf("->hd_ext     %x\n",((GCT_NODE *) pFnode)->hd_extension);
			printf("->owner      %x\n",((GCT_NODE *) pFnode)->owner);
			printf("->curr_own   %x\n",((GCT_NODE *) pFnode)->current_owner);
			printf("->id         %16.16x\n",((GCT_NODE *) pFnode)->id);
			printf("->node_flag  %x\n",((GCT_NODE *) pFnode)->node_flags);
			printf("->config     %x\n",((GCT_NODE *) pFnode)->config);
			printf("->affinity   %x\n",((GCT_NODE *) pFnode)->affinity);
			printf("->parent     %x\n",((GCT_NODE *) pFnode)->parent);
			printf("->next_sib   %x\n",((GCT_NODE *) pFnode)->next_sib);
			printf("->prev_sib   %x\n",((GCT_NODE *) pFnode)->prev_sib);
			printf("->child      %x\n",((GCT_NODE *) pFnode)->child);
			printf("->depth      %x\n",((GCT_NODE *) pFnode)->depth);
			pFrudesc = _GCT_MAKE_ADDRESS(pHd_ext->fru);
			printf("->fru_diagflag    %x\n",pFrudesc->fru_desc_info.fru_diag_flag);
			printf("->fru_diagfailure %x\n",pFrudesc->fru_desc_info.fru_diag_failure_info);
			pTLV = (GCT_TLV *)&pFrudesc->fru_desc_info.fru_info;
			while (pTLV->tlv_tag) {
			  dump_tlv(pTLV);
			  _NEXT_TLV(pTLV);
			}
		    }
		}
		if (killpending())
		    return;

	    if ((pNext->type == 2) || (pNext->type == 0x14) || (pNext->type == 0x15)) j++;
	  }

  }  /* end question */
  
  buffer[0] = 'N';
  read_with_prompt("\ndump binary ? (Y/<N>) ",80,buffer,0,0,1);
  if (toupper(buffer[0]) != 'Y') return;

  /* create a binary output for analysis */
  printf("\n\t.title	GCT_FRU_BINARY\n");
  printf("	.psect  data, noexe,wrt,quad\n");
  printf("	GCTFRU:\n");

  for (i=0;i<800;i++) {

	printf("\t.long ^x%x, ",*pHeader);

	(int)pHeader += 4;
	printf(" ^x%x, ",*pHeader);
	(int)pHeader += 4;
	printf(" ^x%x, ",*pHeader);
	(int)pHeader += 4;
	printf(" ^x%x, ",*pHeader);
	(int)pHeader += 4;
	printf(" ^x%x, ",*pHeader);
	(int)pHeader += 4;
	printf(" ^x%x, ",*pHeader);
	(int)pHeader += 4;
	printf(" ^x%x, ",*pHeader);
	(int)pHeader += 4;
	printf(" ^x%x\n",*pHeader);
	(int)pHeader += 4;

	if (killpending())
	    return;
  }

  /* Finish the binary with a header to compile */
  printf("	.ALIGN	QUAD\n");
  printf("	.psect  code, exe,nowrt\n");
  printf("	.entry	start, ^m<>\n");
  printf("	.end    start\n");

  return;

}
#endif
