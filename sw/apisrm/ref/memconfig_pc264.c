/* file:	memconfig_pc264.c
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
 *
 *
 * Abstract:	This module contains routines that size, configure, and
 *		test memory for the PC264 Platform only.
 *
 * Author:	Eric Goehl
 *                  
 * Modifications:
 *
 *	ER	07-Oct-1998	Implement high heap support.
 *	EGG	30-Oct-1996	Initial port taken from memconfig_eb164.c
 */

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
/* PC264 specific files */      
#include	"cp$inc:platform_io.h"
#include	"cp$src:platform_cpu.h"
#include	"cp$src:impure_def.h"
#include	"cp$src:tsunami.h"
#include	"cp$src:memory_chunk.h"

/*Debug stuff*/
#define DEBUG 0

extern struct ZONE *memzone;          
extern struct ZONE defzone;                             
extern unsigned int memory_low_limit;
extern unsigned int memory_high_limit;
extern unsigned int high_heap_size;
extern unsigned int heap_size;
extern unsigned __int64 mem_size;

extern int getbit64( unsigned int pos, unsigned __int64 base );

int high_heap;
int HWRPB;

/*+
 * ============================================================================
 * = get_array_size - Get the size of the specified memory array              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Get the size (in MBytes) of the specified memory array. 
 *
 * FORM OF CALL:
 *
 *      get_array_size( int array )
 *
 * RETURN CODES:
 *
 *	Size of array in MBytes, otherwise, zero (array disabled)
 *
 * ARGUMENTS:
 *
 *      int array - array number
 *
 * FUNCTIONAL DESCRIPTION
 *
 *      This routine will read the appropriate array address register,
 *	and extract the array size field of that register.
 *
-*/
uint64 get_array_size( int array )
{
    uint64 size;

    size = ( ( ReadTsunamiCSR( CSR_AAR0 + ( array * 0x40 ) ) ) >> 12 ) & 7;
    if ( size != 0 )
        return( ( ( uint64 )1 << ( size + 3 ) ) * 1024 * 1024 );
    else
	return( 0 );
}


/*+
 * ============================================================================
 * = get_array_base - Get the base address of the specified memory array      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Get the base address of the specified memory array.
 *
 * FORM OF CALL:
 *
 *      get_array_base( int array )
 *
 * RETURN CODES:
 *
 *	Base address of memory array, otherwise, zero.
 *
 * ARGUMENTS:
 *
 *      int array - array number
 *
 * FUNCTIONAL DESCRIPTION
 *
 *      This routine will read the appropriate array address 
 *	register and extract the base address field of that 
 *	register.  The base address value returned represents 
 *	bits<34:24> of the physical byte address of the first 
 *	byte in the array.
 *
-*/
uint64 get_array_base( int array )
{
    return( ReadTsunamiCSR( CSR_AAR0 + ( array * 0x40 ) ) & aar_m_addr );
}


/*+
 * ============================================================================
 * = collect_memory_chunks - Collect chunks of memory in a system with holes  =
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
    struct CHUNK_MAP array_map[MAX_MEMORY_ARRAY];

    for ( i = 0; i < MAX_MEMORY_ARRAY; i++ ) {
	array_map[i].base = get_array_base( i );
        array_map[i].size = get_array_size( i );
        if ( array_map[i].size == 0 )
	    array_map[i].base = ( int64 )-1;
    }
#if 0
    pprintf( "before sort \n" );
    for ( i = 0; i < MAX_MEMORY_ARRAY; i++ ) {
        pprintf( "Array %d base = %16x\n", i, array_map[i].base );
        pprintf( "Array %d size = %16x\n", i, array_map[i].size );
    }
#endif

/* 
 * Sort the memory arrays in order of base address 
 */
    k = MAX_MEMORY_ARRAY;
    not_done = 1;
    while ( not_done ) {
    	k = k - 1;
	not_done = 0;
	for ( j = 0;  j < k;  j++ ) {
	    if ( array_map[j].base > array_map[j+1].base ) {
	    	temp = array_map[j];
		array_map[j] = array_map[j+1];
		array_map[j+1] = temp;
		not_done = 1;
	    }
	}
    }
#if 0
    pprintf( "after sort \n" );
    for ( i = 0; i < MAX_MEMORY_ARRAY; i++ ) {
        pprintf( "Array %d base = %16x\n", i, array_map[i].base );
        pprintf( "Array %d size = %16x\n", i, array_map[i].size );
    }
#endif

/* 
 * Collect the contiguous memory chunks 
 */
    for ( j = 0, i = 0; j < MAX_MEMORY_CHUNK; j++ ) {
	chunk_map[j].base = ( uint64 )-1;
	chunk_map[j].size = 0;
	for ( ;  i < MAX_MEMORY_ARRAY; i++ ) {
	    if ( array_map[i].size != 0 ) {
		if ( chunk_map[j].size == 0 ) {
		    chunk_map[j].base = array_map[i].base;
		    chunk_map[j].size = array_map[j].size;
		}
		else if ( ( chunk_map[j].base + chunk_map[j].size ) == array_map[i].base ) {
		    chunk_map[j].size += array_map[i].size;
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
    if ( ( chunk_map[0].size > ( uint64 )0x40000000 ) && high_heap ) {
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
 *	memdiag()
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
int memdiag() 
{
    struct impure *impure_ptr;
    unsigned int signature;
    int i;

    impure_ptr = (struct impure *)(PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE + whoami()*PAL$IMPURE_SPECIFIC_SIZE);
#if DEBUG                               
    {          
      pprintf("CPU %d specific impure pointer: 0x%x.\n", whoami(), (uint64)impure_ptr);
      pprintf("srom dc_ctl: 0x%x.\n", *(unsigned __int64 *)(impure_ptr->cns$dc_ctl));      
      pprintf("srom srom_rev: 0x%x.\n", *(unsigned __int64 *)(impure_ptr->cns$srom_srom_rev));
      pprintf("srom proc_id: 0x%x.\n", *(unsigned __int64 *)(impure_ptr->cns$srom_proc_id));  
      pprintf("srom mem_size: 0x%x.\n", *(unsigned __int64 *)(impure_ptr->cns$srom_mem_size));
      pprintf("srom cycle_cnt: 0x%x.\n", *(unsigned __int64 *)(impure_ptr->cns$srom_cycle_cnt));
      pprintf("srom signature: 0x%x.\n", *(unsigned __int64 *)(impure_ptr->cns$srom_signature));
      pprintf("srom proc_mask: 0x%x.\n", *(unsigned __int64 *)(impure_ptr->cns$srom_proc_mask));
      pprintf("srom sysctx: 0x%x.\n", *(unsigned __int64 *)(impure_ptr->cns$srom_sysctx));      
    }          
#endif                                  

    mem_size = 0;
/*
** Get the number of contiguous bytes of good memory from
** parameter passed in R17 by the SROM and stored in the
** impure scratch area by PALcode.
*/  
    mem_size = *(unsigned __int64 *)(impure_ptr->cns$srom_mem_size);
    signature = *(unsigned __int64 *)(impure_ptr->cns$srom_signature);
    if ((signature >> 20) == 0xDEC && mem_size != 0 ) 
    {
	mem_size =  
	    *(unsigned __int64 *)(impure_ptr->cns$srom_mem_size);
    }               
    else {          
	for ( i = 0;  i < MAX_MEMORY_ARRAY;  i++ )
	    mem_size += get_array_size( i );
    }               
                    
    qprintf("%d Meg of system memory\n", (mem_size/(1024*1024))); 
                    
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
 *	memconfig()
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
void memconfig( ) 
{  
    unsigned int size;
    unsigned int base, high_base;
    unsigned int min_base;
    struct CHUNK_MAP chunk_map[MAX_MEMORY_CHUNK];
     
/*
 * Run the memory diagnostics
 */
    memdiag();                       
/*
 * Bring up a second heap, much larger than the first, in high memory
 */
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

#if EGG_CHANGE
    if ( memory_low_limit <= min_base )
    	high_heap = 0; 
    else
    	high_heap = 1;
#else
    if ( (memory_low_limit > min_base) && ( rtc_read( 0x22 ) != 0x35 )   )
      	high_heap = 1;     
    else             
      	high_heap = 0;
#endif    
    collect_memory_chunks( &chunk_map );
    memory_high_limit = chunk_map[0].size;
          
    if (( memory_low_limit > min_base ) && ( rtc_read( 0x22 ) != 0x35 )  ) {
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
	dyn$_expand (&defzone, base, size, 1);
    }
/*
 * Bring up the main memory heap.
 */
    memzone = dyn$_malloc (sizeof (*memzone), DYN$K_NOOWN);
    base = memory_low_limit;
    size = memory_high_limit - base;
    dyn$_init (memzone, base, size, 0);

    HWRPB = 0x2000;              
    sys_irq_init( );                                  
    build_hwrpb( ( struct HWRPB * )HWRPB );      
    build_bitmap( ( struct HWRPB * )HWRPB );     
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
 *--
 */
#define BITMAP_DEBUG 0
int build_bitmap( struct HWRPB *hwrpb ) {

struct MEMDSC *memdsc;

#if BITMAP_DEBUG
    pprintf("start of build_bitmap\n");
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

    return msg_success;
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
 *	addr_to_brd( addr )
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
 *	int brd -  Memory Module to turn off edc
 *
 * SIDE EFFECTS:
 *
 *   	None
 *
-*/
int turn_off_edc( int brd )
{
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
 * ARGUMENTS:
 *
 *	int brd -  Memory Module to turn on edc
 *
 * SIDE EFFECTS:
 *
 *   	None
 *
-*/
int turn_on_edc( int brd ) 
{
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
 *      get_bcache_size();
 *                                  
 * RETURN CODES:                                  
 *                                       
 *     	The bcache size in megabytes.
 *   
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
int get_bcache_size()
{                                                                
    struct impure *impure_ptr;
    unsigned __int64 write_many_csr;
    int id;
    int bc_enable;
    int bc_size = 0;

    id = whoami( );
    impure_ptr = ( struct impure * )( PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE + ( id * PAL$IMPURE_SPECIFIC_SIZE ) );
/*
** The Cbox WRITE_MANY CSR is passed in from the SROM in the upper 
** 48 bits of the DC_CTL value.
*/
    write_many_csr = *( unsigned __int64 * )( impure_ptr->cns$dc_ctl ) >> 16;
    bc_enable = ( int )( ( write_many_csr >> 30 ) & 0x01 );
    if ( bc_enable ) {
	bc_size = ( int )( ( write_many_csr >> 32 ) & 0x0F );
	if ( bc_size == 1 )
	    bc_size = 2;
	else if ( bc_size == 3 )
	    bc_size = 4;
	else
	    bc_size = 0;
    }
    bc_size = bc_size * 0x100000;
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
 *   	show memory ( )
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
 *
 *      Array #      Size      Base Addr  
 *      -------   ----------   ---------
 *         0        128 MB     000000000      
 *         1        128 MB     008000000
 *         2        128 MB     010000000
 *         3        128 MB     018000000
 *	Total Bad Pages 0
 *	Total Good Memory = 512 MBytes
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

showmem ( int argc, char *argv[] ) 
{
    struct MEMDSC *memdsc;
    struct HWRPB *hwrpb;
    int i, j, bad;

    if ( argc != 2 ) {
	printf( "Incorrect number of arguments for show memory\n" );
	return( msg_failure );
    }
    printf( "\nArray #       Size     Base Addr\n" );
    printf( "-------    ----------  ---------\n" );
    for ( i = 0;  i < MAX_MEMORY_ARRAY;  i++ ) {
	if ( get_array_size( i ) != 0 ) {
	    printf( "   %d         %3d MB    %09X\n", 
		    i, 
		    get_array_size( i ) / ( 1024 * 1024 ), 
		    get_array_base( i ) 
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
	    if ( getbit64( j, *memdsc->CLUSTER[i].BITMAP_PA ) == 0 ) {
		bad++;
	    }
	}
    }
    printf( "\nTotal Bad Pages = %d\n", bad );
    printf( "Total Good Memory = %d MBytes\n", ( mem_size / ( 1024 * 1024) ) - ( ( bad * 8192 ) / ( 1024 * 1024 ) ) );
    return( msg_success );
}
