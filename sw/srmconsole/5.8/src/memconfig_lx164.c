/* file:	memconfig_lx164.c
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
 *
 * Facility:	Alpha SRM Console Firmware
 *
 * Abstract:	This module contains platform specific routines that size, 
 *		configure, and test memory.
 *
 * Author:	Eric Rasmussen
 *                  
 * Created:	20-Nov-1996
 *
 * Modification History:
 *
 */

/* Debug stuff */
#define ISP 0

#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:mem_def.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:ev_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:pal_def.h"
/* Platform specific include files */      
#include	"cp$inc:platform_io.h"
#include	"cp$src:platform_cpu.h"
#include	"cp$src:impure_def.h"
#include	"cp$src:pyxis_mem_csr_def.h"
#include	"cp$src:memory_chunk.h"

extern struct ZONE *memzone;          
extern struct ZONE defzone;
extern unsigned int memory_low_limit;
extern unsigned int memory_high_limit;
extern unsigned int high_heap_size;
extern unsigned int heap_size;
extern int robust_mode;

/* regexp.c */
extern int getbit( unsigned int pos, unsigned int *base );

/* kernel_support.c */
extern int twos_checksum( int *addr, int count, unsigned int *checksum );

/* toy_driver.c */
extern int rtc_read( int offset );

extern int HWRPB;
extern unsigned __int64 mem_size;

int memory_tested;  /* number of pages tested in powerup path */
int make_high_chunk;

int memdiag( void );
int mark_bad_mem( void );

/*+
 * ============================================================================
 * = get_bank_size - Get the size of the specified memory bank                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Get the size (in MBytes) of the specified memory bank. 
 *
 * FORM OF CALL:
 *
 *      get_bank_size( int bank )
 *
 * RETURN CODES:
 *
 *	Size of bank in MBytes, otherwise, zero (bank disabled).
 *
 * ARGUMENTS:
 *
 *      int bank - Memory bank number
 *
 * FUNCTIONAL DESCRIPTION
 *
 *      This routine will read the appropriate bank address register,
 *	and extract the bank size field of that register.
 *
-*/
uint64 get_bank_size( int bank )
{
    unsigned __int64 BaseCSR = pyxis_k_mem_csr_base;
    union BCR bcr;
    uint64 size;

    bcr.bcr_l_whole = ( int )READ_MEM_CSR( bank, CSR_BCR );
    if ( bcr.bcr_r_field.bcr_v_bank_enable ) {
	size = ( 1 << ( 10 - ( bcr.bcr_r_field.bcr_v_bank_size & 7 ) ) );
        return( size * 1024 * 1024 );
    }
    return( 0 );
}


/*+
 * ============================================================================
 * = get_bank_base - Get the base address of the specified memory bank        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Get the base address of the specified memory bank.
 *
 * FORM OF CALL:
 *
 *      get_bank_base( int bank )
 *
 * RETURN CODES:
 *
 *	Base address of memory bank, otherwise, zero.
 *
 * ARGUMENTS:
 *
 *      int bank - Memory bank number
 *
 * FUNCTIONAL DESCRIPTION
 *
 *      This routine will read the appropriate bank address 
 *	register and extract the base address field of that 
 *	register.  The base address value returned represents 
 *	bits<33:24> of the physical byte address of the first 
 *	byte in the bank.
 *
-*/
uint64 get_bank_base( int bank )
{
    unsigned __int64 BaseCSR = pyxis_k_mem_csr_base;
    union BBAR bbar;

    bbar.bbar_l_whole = ( int )READ_MEM_CSR( bank, CSR_BBAR );
    return( bbar.bbar_r_field.bbar_v_base_addr << 24 );
}


/*+
 * ============================================================================
 * = collect_memory_chunks - Collect chunks of contiguous memory              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      This routine collects memory into chunks of contiguous space.  The
 *      chunks are listed from lowest base address to highest.
 *
 * FORM OF CALL:
 *
 *      collect_memory_chunks( chunk_map )
 *
 * RETURN CODES:
 *
 *      msg_success
 *
 * ARGUMENTS:
 *
 *      chunk_map - a structure that defines the base address, size,
 *		    and partition information of the memory chunks.
 *
-*/
int collect_memory_chunks( struct CHUNK_MAP *chunk_map )
{
    int i, j, k;
    int not_done;
    struct CHUNK_MAP temp;
    struct CHUNK_MAP bank_map[LX164$MEM_TOTAL];

    for ( i = 0; i < LX164$MEM_TOTAL; i++ ) {
	bank_map[i].base = get_bank_base( i );
        bank_map[i].size = get_bank_size( i );
        if ( bank_map[i].size == 0 )
	    bank_map[i].base = ( int64 )-1;
    }
#if 0
    pprintf( "before sort \n" );
    for ( i = 0; i < LX164$MEM_TOTAL; i++ ) {
        pprintf( "Bank %d base = %16x\n", i, bank_map[i].base );
        pprintf( "Bank %d size = %16x\n", i, bank_map[i].size );
    }
#endif

/* 
 * Sort the memory banks in order of base address 
 */
    k = LX164$MEM_TOTAL;
    not_done = 1;
    while ( not_done ) {
    	k = k - 1;
	not_done = 0;
	for ( j = 0;  j < k;  j++ ) {
	    if ( bank_map[j].base > bank_map[j+1].base ) {
	    	temp = bank_map[j];
		bank_map[j] = bank_map[j+1];
		bank_map[j+1] = temp;
		not_done = 1;
	    }
	}
    }
#if 0
    pprintf( "after sort \n" );
    for ( i = 0; i < LX164$MEM_TOTAL; i++ ) {
        pprintf( "Bank %d base = %16x\n", i, bank_map[i].base );
        pprintf( "Bank %d size = %16x\n", i, bank_map[i].size );
    }
#endif

/* 
 * Collect the contiguous memory chunks 
 */
    for ( j = 0, i = 0; j < MAX_MEMORY_CHUNK; j++ ) {
	chunk_map[j].base = ( uint64 )-1;
	chunk_map[j].size = 0;
	for ( ;  i < LX164$MEM_TOTAL; i++ ) {
	    if ( bank_map[i].size != 0 ) {
		if ( chunk_map[j].size == 0 ) {
		    chunk_map[j].base = bank_map[i].base;
		    chunk_map[j].size = bank_map[j].size;
		}
		else if ( ( chunk_map[j].base + chunk_map[j].size ) == bank_map[i].base ) {
		    chunk_map[j].size += bank_map[i].size;
		}
		else {
		    break;
		}
            }
	}
    }
/* 
 * Make sure that chunk zero is smaller than 1GB 
 * If it is larger split it into two chunks.     
 */
    if ( ( chunk_map[0].size > ( uint64 )0x40000000 ) && make_high_chunk ) {
        for ( i = MAX_MEMORY_CHUNK - 2; i >= 1; i-- )
            chunk_map[i+1] = chunk_map[i];
 
        chunk_map[1].size =  chunk_map[0].size - ( uint64 )0x40000000;
        chunk_map[1].base =  ( uint64 )0x40000000;
        chunk_map[0].size =  ( uint64 )0x40000000;
    }
#if 0
    pprintf( "after collect \n" );
    for ( i = 0; i < MAX_MEMORY_CHUNK; i++ ) {
        pprintf( "Chunk %d base = %16x\n", i, chunk_map[i].base );
        pprintf( "Chunk %d size = %16x\n", i, chunk_map[i].size );
    }
#endif

    return msg_success;
}

                                                    
/*+                    
 * ============================================================================
 * = memconfig - configure memory                                             =
 * ============================================================================
 *
 * OVERVIEW:
 *                                          
 *	Configure and test all of main memory.  Set up the main memory zone.
 *
 * FORM OF CALL:
 *
 *	memconfig( )
 *
 * RETURN CODES:
 *
 *	msg_success
 *
 * ARGUMENTS:
 *
 *	none
 *
 * SIDE EFFECTS:
 *                               
 *	none
 *              
-*/
int memconfig( void ) 
{
    unsigned int size;
    unsigned int base, high_base;
    unsigned int min_base;
    struct CHUNK_MAP chunk_map[MAX_MEMORY_CHUNK];

    make_high_chunk = 0;
/*
** Run the memory diagnostics
*/
    if ( !robust_mode ) {
	memdiag( );                       
    } else {
	mem_size = 0x1000000;
    }

    size = heap_size - MIN_HEAP_SIZE;
    base = memory_low_limit;
#if HIGH_BOOT_PTEPAGES
    min_base = 2 * 1024 * 1024;
#else
    min_base = 2 * 1024 * 1024 - ( BOOT_PTEPAGES * HWRPB$_PAGESIZE );
#endif
    if ( min_base < base )
    	min_base = base;

    memory_low_limit = ( base + size + HWRPB$_PAGESIZE - 1 ) & ~( HWRPB$_PAGESIZE - 1 );
    size += memory_low_limit - ( base + size );
    if ( memory_low_limit > min_base ) {
    	if ( rtc_read( 0x22 ) != 0x35 ) {
	    make_high_chunk = 1;
	}
    }
    else {
    	size += min_base - memory_low_limit;
	memory_low_limit = min_base;
	make_high_chunk = 0;
    }

    collect_memory_chunks( &chunk_map );
    memory_high_limit = chunk_map[0].size;

    if ( ( memory_low_limit > min_base ) && ( rtc_read( 0x22 ) != 0x35 ) ) {
    	high_heap_size = memory_low_limit - min_base;
	memory_low_limit -= high_heap_size;
	size -= high_heap_size;
	if ( size )
	    dyn$_expand( &defzone, base, size, 1 );

	high_base = memory_high_limit - high_heap_size;
	memory_high_limit -= high_heap_size;
	dyn$_expand( &defzone, high_base, high_heap_size, 1 );
    }
    else {
	dyn$_expand( &defzone, base, size, 1 );
    }
/*
** Bring up the main memory heap.
*/
    memzone = dyn$_malloc( sizeof ( *memzone ), DYN$K_NOOWN, 0, 0, (struct ZONE *)0 );
    base = memory_low_limit;
    size = memory_high_limit - base;
    dyn$_init( memzone, base, size, 0 );
    return( msg_success );
}


/*+
 * ============================================================================
 * = memdiag - Powerup memory diagnostics                                     =
 * ============================================================================
 *
 * OVERVIEW:       
 *
 *	The Powerup memory diagnostics are called from the routine.  When this
 *	routine exits, memory has been turned on, cleaned up (no parity/ecc
 *	errors left) and memsize filled in.
 *
 * FORM OF CALL:
 *
 *	memdiag( )
 *
 * RETURN CODES:                                            
 *   
 *	msg_success	
 *	msg_failure
 *
 * ARGUMENTS:
 *
 *	none
 *
 * SIDE EFFECTS:
 *
 *	none
 *
-*/
int memdiag( void ) 
{
    struct impure *impure_ptr;
    unsigned int i, signature;

    mem_size = 0;
/*
** Get the number of contiguous bytes of good memory from
** parameter passed in R17 by the SROM and stored in the
** impure scratch area by PALcode.
*/
    impure_ptr = ( struct impure * )( PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE );
    signature = *( unsigned __int64 * )( impure_ptr->cns$srom_signature );
    if ( ( signature >> 16 ) == 0xDECB ) {
	mem_size = 
	    *( unsigned __int64 * )( impure_ptr->cns$srom_mem_size );
    }
    else {
	for ( i = 0;  i < LX164$MEM_TOTAL;  i++ ) {
	    mem_size += get_bank_size( i );
	}
    }
    return( msg_success );
}          


void io_config( void ) {

}


/*
 *++
 * =========================================================================
 * = mark_bad_mem - allocate bad memory in the console heap		   =
 * =========================================================================
 *
 *  FUNCTIONAL DESCRIPTION:
 *
 *	This routine allocates the bad areas in the console heap area.
 *	The console can run with bad memory in the heap.  The pages to be 
 * 	marked were previously identified by memdiag, and are passed to this
 *	routine by a private mechanism.
 *
 *  FORMAL PARAMETERS:
 *
 *	None
 *
 *  RETURN VALUE:
 *      
 *      NONE      
 *
 *  IMPLICIT INPUTS:
 *
 *      Defzone
 *
 *  IMPLICIT OUTPUTS:
 *
 *      None
 *
 *  SIDE EFFECTS:
 *
 *	CONSOLE_DISPATCH structure will recieve the bitmap 
 *	address, size, and checksum.
 *
 *                
 *--
 */
int mark_bad_mem( void ) 
{
    return( msg_success );
}


/*
 *++
 * =========================================================================
 * = build_bitmap - create a VMS style bitmap                              =
 * =========================================================================
 *
 *  FUNCTIONAL DESCRIPTION:
 *
 *	This routine marks the bad pages in the bitmap created for memory
 *	cluster 1. The errors are read from the IIC EEPROM and the 
 *	corresponding pages are marked bad in the bitmap.  The checksum
 *	for the bitmap and the memory descriptor are recalculated.
 *      The console memzone will also be updated to reflect any bad
 *	memory detected.
 *
 *  FORMAL PARAMETERS:
 *                                                                       
 *	struct HWRPB *hwrpb	- address to build hwrpb
 *
 *  RETURN VALUE:
 *      
 *      NONE
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      A VMS stype bitmap.
 *
 *  SIDE EFFECTS:
 *
 *	None
 *   
 *   
 *--
 */
#define BITMAP_DEBUG 0
int build_bitmap( struct HWRPB *hwrpb ) 
{
    struct MEMDSC *memdsc;

#if BITMAP_DEBUG
    pprintf( "start of build_bitmap\n" );
#endif

    /* Get the address of the memory descriptors */
    memdsc = ( struct MEMDSC * )( ( int )hwrpb + *( unsigned __int64 * )hwrpb->MEM_OFFSET );

    /* checksum the bitmap */
    twos_checksum( *( unsigned __int64 * )memdsc->CLUSTER[1].BITMAP_PA,
		   ( *( unsigned __int64 * )memdsc->CLUSTER[1].PFN_COUNT + 31 ) / 32,
		   ( unsigned __int64 * )memdsc->CLUSTER[1].BITMAP_CHKSUM );

#if BITMAP_DEBUG
    pprintf( "after memdsc->CLUSTER[1].BITMAP_CHKSUM = %x\n",
		*( unsigned __int64 * )memdsc->CLUSTER[1].BITMAP_CHKSUM);
#endif

    *( unsigned __int64 * )memdsc->CHECKSUM = 0;
    /* Checksum the Memory Descriptor area */
    twos_checksum( ( int )memdsc, sizeof( struct MEMDSC ) / 4, ( unsigned __int64 * )memdsc->CHECKSUM );

#if BITMAP_DEBUG
    pprintf( "after *memdsc->CHECKSUM = %x\n", *( unsigned __int64 * )memdsc->CHECKSUM );
#endif

    return( msg_success );
}


/*+
 * ============================================================================
 * = addr_to_brd - identifies the board given an address 		      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Find the associated board of the inputted address
 *
 * FORM OF CALL:
 *
 *	addr_to_brd(addr)
 *
 * RETURN CODES:
 *
 *	the board number which contains the address
 *
 * ARGUMENTS:
 *
 *	int addr - the address to trace to a board
 *
 * SIDE EFFECTS:
 *
 *	none
 *
-*/
int addr_to_brd( int addr )
{
    return( 0 );
}


#define EDC_DEBUG 0
/*+
 * ============================================================================
 * = turn_off_edc - Turn off the EDC reporting				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	The EDC reporting is turned off on the specified board.
 *      Both correctable and uncorrectable errors are turned off.
 *
 * FORM OF CALL:
 *
 *      turn_off_edc( brd );
 *
 * RETURN CODES:
 *
 *     	msg_success - edc mode was turned off
 *
 * ARGUMENTS:
 *
 *	int brd - Memory Module to turn off edc
 *
 * SIDE EFFECTS:
 *
 *   	None
 *
-*/
int turn_off_edc( int brd )
{
    /* no way to turn off ECC on a board by board basis */
    return( msg_success );
}


/*+
 * ============================================================================
 * = turn_on_edc - Turn on the EDC reporting				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	The EDC reporting is turned on on the specified board.
 *      Both correctable and uncorrectable errors are turned on.
 *
 * FORM OF CALL:
 *
 *      turn_on_edc( brd );
 *
 * RETURN CODES:
 *
 *     	msg_success - edc mode was turned on
 *
 *
 * ARGUMENTS:
 *
 *	int brd - Memory Module to turn on edc
 *
 * SIDE EFFECTS:
 *
 *   	None
 *
-*/
int turn_on_edc( int brd ) {

    /* No way to turn ECC on a board by board basis */
    return( msg_success );
}


/*+                                                                   
 * ============================================================================
 * = get_bcache_size - Determines the bcache size of the CPU module	      =
 * ============================================================================
 *                                                
 * OVERVIEW:                                    
 *                                                  
 *	Determines the bcache size of the CPU module
 *                   
 * FORM OF CALL:                          
 *                                                                    
 *      get_bcache_size( );
 *                                  
 * RETURN CODES:                                  
 *                                       
 *     	The bcache size in megabytes.
 *                                                          
 * ARGUMENTS:                                          
 *                                                                
 *	None
 *                                            
 * SIDE EFFECTS:                                         
 *                                                      
 *   	None	                                      
 *                                                  
-*/                                            
int get_bcache_size( void )
{                                                                
    struct impure *impure_ptr;
    unsigned int signature;
    unsigned __int64 bc_cfg = 0;
    int bc_size = 0;

    impure_ptr = ( struct impure *)( PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE );
    signature = *( unsigned __int64 * )( impure_ptr->cns$srom_signature );
    if ( ( signature >> 16 ) == 0xDECB ) {
	bc_cfg = 
	    *( unsigned __int64 * )( impure_ptr->cns$srom_bc_cfg );
    }
    if ( bc_cfg & 0x7 ) {
	bc_size = ( 1 << ( ( bc_cfg & 0x7 ) - 1 ) ) * ( 1024 * 1024 );
    }
    return( bc_size );
}


/*+         
 * ============================================================================
 * = show memory - Show memory configuration.                                 =
 * ============================================================================
 *
 * OVERVIEW:                                 
 *
 *	Shows the main memory configuration on a board by board basis.
 *	Also reports the addresses of bad pages defined by the bitmap.
 *
 *	Additionally, shows the normally inaccessible areas of memory,
 *   	such as, the PFN bitmap pages, the console memory pages, PALcode,
 *	and shared data structures such as the HWRPB.
 *
 *   COMMAND FORM:
 *  
 *   	show memory( )
 *  
 *   COMMAND ARGUMENT(S):
 *          
 *	None
 *
 *   COMMAND OPTION(S):
 *
 *	None                                 
 *
 *   COMMAND EXAMPLE(S):
 *
 *	In the following example, a system's memory is displayed. 
 *~
 *	>>>show memory
 *      Bank     Size    Base Addr
 *      ------   -----   ---------
 *        0       32Mb   00000000      
 *        1      Not Installed
 *        2      Not Installed
 *        3      Not Installed
 *	Total Bad Pages 0
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *
 *	show config
 *
 * FORM OF CALL:
 *  
 *	showmem( argc, *argv[] )
 *          
 * RETURN CODES:
 *
 *	msg_success - success
 *       
 * ARGUMENTS:                                
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *           
 * SIDE EFFECTS:
 *      
 *      None
 *
-*/                                                        
int showmem( int argc, char *argv[] ) 
{
    struct MEMDSC *memdsc;
    struct HWRPB *hwrpb;
    int i, j, bad, size;

    if ( argc != 2 ) {
	printf( "Incorrect number of arguments for show memory\n" );
	return( msg_failure );
    }

    printf( "\n%d MBytes of system memory\n\n", ( mem_size / ( 1024 * 1024 ) ) );
    printf( "Bank #       Size     Base Addr\n" );
    printf( "------    ----------  ---------\n" );
    for ( i = 0; i < LX164$MEM_TOTAL; i++ ) {
	if ( get_bank_size( i ) != 0 ) {
	    printf( "  %d         %3d MB    %09X\n", 
		    i, 
		    get_bank_size( i ) / ( 1024 * 1024 ), 
		    get_bank_base( i ) 
	    );
	}
    }
/* 
** Summarize the bad pages in a cluster.  Ignore undefined bitmaps. 
*/
    bad = 0;
    hwrpb = ( struct HWRPB * )HWRPB;
    memdsc = ( struct MEMDSC * )( ( int )hwrpb + *( unsigned __int64 * )hwrpb->MEM_OFFSET );
                                        
    for ( i = 0; i < *memdsc->CLUSTER_COUNT; i++ ) {	
	for ( j = 0; *memdsc->CLUSTER[i].BITMAP_PA && j < *memdsc->CLUSTER[i].PFN_COUNT; j++ ) {
	    if ( getbit( j, *memdsc->CLUSTER[i].BITMAP_PA ) == 0 ) {
		bad++;
	    }
	}
    }
    printf( "\nTotal Bad Pages = %d\n", bad );
    printf( "Total Good Memory = %d MBytes\n", ( mem_size / ( 1024 * 1024) ) - ( ( bad * 8192 ) / ( 1024 * 1024 ) ) );

    printf( "\nBcache Size = %d MB\n", get_bcache_size( ) / ( 1024 * 1024 ) );

    return( msg_success );
}
