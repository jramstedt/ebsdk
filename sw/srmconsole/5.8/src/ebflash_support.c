/* file:	ebflash_support.c
 *
 * Copyright (C) 1997 by
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
 *	Routines used to access the Intel 28F008SA flash ROM.
 *
 *  AUTHORS:
 *
 *	Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *      30-Dec-1997
 *
 *  MODIFICATION HISTORY:
 *
 *
 *--
 */
#include	"cp$src:platform.h"
#include	"cp$inc:platform_io.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:ebflash_def.h"
#include	"cp$src:romhead.h"

#define FLASH_DEBUG 0

#define SEGMASK	    0xF0000

/* Local routine declarations */

unsigned int init_flash_rom( void );
unsigned int read_flash_rom( unsigned int src, int size, unsigned char *dst );
unsigned int write_flash_rom( unsigned int dst, int size, unsigned char *src );

static unsigned int flash_erase( unsigned int segment );
static unsigned int flash_program( unsigned int segment, unsigned char *buf, unsigned int length );
static void read_flash( unsigned int src, unsigned char *dst );
static void write_flash( unsigned int dst, unsigned char value );
static unsigned int flash_bank_select( unsigned int addr );
static void reset_flash( void );
static unsigned char read_flash_status( void );
static unsigned int clear_flash_status( void );
static unsigned int flash_decode( unsigned char status );

/* Local data declarations */

static int last_bank = -1;


unsigned int init_flash_rom( void )
{
    return msg_success;
}


/*+
 * ============================================================================
 * = read_flash_rom - Read bytes from the flash memory device                 =
 * ============================================================================
 *
 * OVERVIEW:
 * 
 *	This is the low level routine to read a specified number of bytes
 *	from the flash memory device. 
 *
 * FORM OF CALL:
 *  
 *	read_flash_rom( unsigned int src, int size, unsigned char *dest )
 *  
 * RETURN CODES:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 *   	unsigned int src    - Location in flash memory region to read from.
 *	int size	    - Number of bytes to read
 *	unsigned char *dst  - Returned data buffer
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
unsigned int read_flash_rom( unsigned int src, int size, unsigned char *dst ) 
{
#if FLASH_DEBUG
    qprintf( "Reading %d bytes from flash location %x\n", size, src );
#endif
    while ( size > 0 ) {
	read_flash( src, dst );
	src += FLASH_BYTE;
	dst++;
	size--;
    }
    return msg_success;

} /* read_flash_rom */


/*+
 * ============================================================================
 * = write_flash_rom - Write bytes to the flash memory device                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This is the low level routine to write a specified number of 
 *	bytes to the flash memory device.
 *
 * FORM OF CALL:
 *  
 *	write_flash_rom( unsigned int dst, int size, unsigned char *src )
 *  
 * RETURN CODES:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 *   	unsigned int dst    - Location in flash memory region to write.
 *	int size	    - Number of bytes to write
 *	unsigned char *src  - Data to be written
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
unsigned int write_flash_rom( unsigned int dst, int size, unsigned char *src ) 
{
    unsigned int offset = 0;
    unsigned int n_bytes = 0;
    unsigned int i, segment, n_segments;
    char *eerom = 0;
    int eerom_flag = 0;
/*
** If we are writing the EEROM, then we need to do a read-modify-write
** operation to preserve the contents of the ISA configuration table
** and envrionment variables stored there.
*/
    if ( ( dst & SEGMASK ) == EEROM_BASE ) {
	eerom_flag = 1;
	eerom = dyn$_malloc( EEROM_LENGTH, DYN$K_SYNC );
	read_flash_rom( EEROM_BASE, EEROM_LENGTH, eerom );
	memcpy( eerom + ( dst & ~SEGMASK ), src, size );
	src = eerom;
	size = EEROM_LENGTH;
    }

    if ( ( dst & SEGMASK ) % SEGSIZE ) {
	err_printf ( "Destination 0x%08X is not on a %d segment boundary\n",
		    dst & SEGMASK, SEGSIZE );
	return msg_failure;
    }
    segment = dst / SEGSIZE;
/*
** Compute the number of segments to write
*/
    n_segments = size / SEGSIZE;
    if ( size % SEGSIZE )
	n_segments++;
#if FLASH_DEBUG
    qprintf( "Writing %d bytes, %d segment(s), starting at flash location %x\n", size, n_segments, dst );
#endif
    if ( clear_flash_status( ) != msg_success )
	return msg_failure;
/*
** Erase and program each segment
*/
    for ( i = 0; i < n_segments; i++ ) {
	if ( flash_erase( segment ) != msg_success ) {
	    err_printf ( "Segment %02d had incomplete erasures\n", segment );
	    return msg_failure;
	}
	if ( ( size - SEGSIZE ) >= 0 ) {
	    n_bytes = SEGSIZE;
	    size = size - SEGSIZE;
	}
	else {
	    n_bytes = size;
	}
	if ( flash_program( segment, ( ( unsigned int )src + offset ), n_bytes ) != msg_success ) {
	    err_printf ( "Segment %02d had programming failures\n", segment );
	    return msg_failure;
	}
	offset += SEGSIZE;
	segment++;
    }

    reset_flash( );

    if ( eerom_flag )
	dyn$_free( eerom, DYN$K_SYNC );

    return msg_success;

} /* write_flash_rom */


/*+
 * ============================================================================
 * = flash_erase - erase a flash ROM segment. 				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine will erase a flash ROM segment.
 *
 * FORM OF CALL:
 *  
 *	flash_erase( segment )
 *  
 * RETURN CODES:
 *
 *      msg_success or msg_failure
 *       
 * ARGUMENTS:
 *
 *	unsigned int segnum - Flash segment to erase
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static unsigned int flash_erase( unsigned int segment )
{
    int i;
    unsigned int offset;
    unsigned char value;

    offset = segment * SEGSIZE;

#if FLASH_DEBUG
    qprintf( "Erasing flash segment %02d\n", segment );
#else
    write_flash( offset, ERASE_SETUP );
    write_flash( offset, ERASE_CONFIRM );
    if ( flash_decode( read_flash_status( ) ) != msg_success )
	return msg_failure;
#endif
/* 
** Verify how many cells were erased 
*/
#if FLASH_DEBUG
    qprintf( "Verifying erase of flash segment %02d\n", segment );
#else
    for ( i = 0; i < SEGSIZE; i++ ) {
	read_flash( offset + i, &value );
	if ( value != 0xFF )
	    return msg_failure;
    }
#endif

    return msg_success;

} /* flash_erase */


/*+
 * ============================================================================
 * = flash_program - program a flash ROM segment.		       	      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine programs a flash ROM segment with the specified data. 
 *
 * FORM OF CALL:
 *  
 *	flash_program( segment, buf, length )
 *  
 * RETURN CODES:
 *
 *      msg_success or msg_failure
 *       
 * ARGUMENTS:
 *
 *	unsigned int segment	- flash ROM segment to write
 *	unsigned char *buf 	- pointer to buffer containing data to write
 *	unsigned int length	- number of bytes to write
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static unsigned int flash_program( unsigned int segment, unsigned char *buf, unsigned int length )
{
    int i;
    unsigned int offset;
    unsigned char value;

    offset = segment * SEGSIZE;

#if FLASH_DEBUG
    qprintf( "Programming %d bytes in segment %d\n", length, segment );
#else
    for ( i = 0; i < length; i++ ) {
	write_flash( offset + i, BYTE_WRITE );
	write_flash( offset + i, buf[i] );
	if ( flash_decode( read_flash_status( ) ) != msg_success )
	    return msg_failure;
    }
#endif
/* 
** Verify the programming ... 
*/
#if FLASH_DEBUG
    qprintf ( "Verifying programming of flash segment %02d\n", segment );
#else
    for ( i = 0; i < length; i++ ) {
	read_flash( offset + i, &value );
	if ( value != buf[i] )
	    return msg_failure;
    }
#endif

    return msg_success;

} /* flash_program */


/*+
 * ============================================================================
 * = read_flash - Read a byte from the flash ROM			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This is the low level routine to read a byte from the flash ROM.
 *
 * FORM OF CALL:
 *  
 *	read_flash( src, dst )
 *  
 * RETURNS:
 *
 *
 * ARGUMENTS:
 *
 *
-*/
static void read_flash( unsigned int src, unsigned char *dst )
{
    *dst = inmemb( NULL, ( unsigned __int64 )( FLASH_BASE + flash_bank_select( src ) ) ); 

} /* read_flash */


/*+
 * ============================================================================
 * = write_flash - Write a byte of data to the flash ROM		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This is the low level routine to write a byte of data to 
 *	the flash ROM.
 *
 * FORM OF CALL:
 *  
 *	write_flash( dst, value )
 *  
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *
 * SIDE EFFECTS:
 *
-*/
static void write_flash( unsigned int dst, unsigned char value )
{
    outmemb( NULL, ( unsigned __int64 )( FLASH_BASE + flash_bank_select( dst ) ), value ); 

} /* write_flash */


/*+
 * ============================================================================
 * = flash_bank_select - select a flash ROM bank                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	This is the low level routine to perform proper bank switching.
 *	It is called by other routines in this module only.
 *
 * FORM OF CALL:
 *  
 *	flash_bank_select( addr )
 *  
 * RETURN CODES:
 *
 *       
 * ARGUMENTS:
 *
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
static unsigned int flash_bank_select( unsigned int offset ) 
{
    int bank;

    bank = offset / ( FLASH_ROM_SIZE / 2 );
    if ( bank != last_bank ) {
	last_bank = bank;
	if ( bank == 0 ) {
/*
** Clear address bit 19 to address the lower 512K
*/
	    outportb( NULL, 0x800, 0 );
	}
	else {
/*
** Set address bit 19 to address the upper 512K
*/
	    outportb( NULL, 0x800, 1 );
	}
    }
    return( bank ? offset - ( FLASH_ROM_SIZE / 2 ) : offset );

} /* flash_bank_select */


/*+
 * ============================================================================
 * = reset_flash - Put the flash device in READ_RESET mode.		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Reset the flash device to a known state.
 *
 * FORM OF CALL:
 *  
 *	reset_flash( )
 *  
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *
-*/
static void reset_flash ( void ) 
{    
    write_flash( 0, READ_RESET );

} /* reset_flash */


/*+
 * ============================================================================
 * = read_flash_status - Read the status register of the flash memory device  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Read the status register until the device is READY or the read
 *	of the status register times out.
 *
 * FORM OF CALL:
 *  
 *	read_flash_status( )
 *  
 * RETURNS:
 *
 *	The value of the status register.
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	The device is left in READ_RESET mode.
 *
-*/
static unsigned char read_flash_status( void )
{
    unsigned int i, j;
    unsigned char status;

    i = j = 0;
    do {
	write_flash( 0, READ_STATUS );
	read_flash( 0, &status );
#if 0
	err_printf( "Reading flash status, status = %X\n", status );
#endif
	if ( status & SR_READY ) 
	    break;
	krn$_micro_delay( 1 );
	if ( ++i >= 1000 ) { i = 0; j++; }
#if 0
	err_printf( "Loop again, i = %d, j = %d\n", i, j );
#endif
    } while ( j < 5000 );

    if ( j >= 5000 )
	err_printf( "Flash error: Read status timed out\n" );

    reset_flash( );

    return status;

} /* read_flash_status */


/*+
 * ============================================================================
 * = clear_flash_status - Clear the status register			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Clear the status register.
 *
 * FORM OF CALL:
 *  
 *	clear_flash_status( )
 *  
 * RETURNS:
 *
 *	msg_success or msg_failure.
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *
-*/
static unsigned int clear_flash_status ( void )
{
    write_flash( 0, CLEAR_STATUS );
    return( flash_decode( read_flash_status( ) ));

} /* clear_flash_status */


/*+
 * ============================================================================
 * = flash_decode  - decode the contents of the status register. 	      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine will decode the 28F008SA status register
 *	and print out an error message.
 *
 * FORM OF CALL:
 *  
 *	flash_decode( status )
 *  
 * RETURN CODES:
 *
 *      msg_success or msg_failure
 *       
 * ARGUMENTS:
 *
 *	unsigned char status - contents of the status register
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static unsigned int flash_decode ( unsigned char status ) 
{
    if ( ( status & SR_READY ) == 0) {
	err_printf( "Flash error: Device not ready\n" );
	return msg_failure;
    }
    if ( !( status & ~SR_READY ) ) {
	return msg_success;
    }
    if ( status & SR_VPP_LOW ) {
	err_printf( "Flash error: low voltage detected\n" );
    }
    if ( status & SR_WRITE_ERR ) {
	err_printf( "Flash error: byte write error\n" );
    }
    if ( status & SR_ERASE_ERR ) {
	err_printf( "Flash error: error in block erase\n" );
    }
    return msg_failure;

} /* flash_decode */
