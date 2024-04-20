/* file:	amdflash_support.c
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
 *	Driver support functions for AMD flash ROM devices.
 *
 *  AUTHORS:
 *
 *	Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *      12-Mar-1997
 *
 *  MODIFICATION HISTORY:
 *
 *	wcc	5-Sep-1997	Alow for common pool platform  definitions 
 *				to define the EEROM area.
 *
 *
 *--
 */
/* $INCLUDE_OPTIONS$ */
#include	"cp$src:platform.h"
#include	"cp$inc:platform_io.h"
/* $INCLUDE_OPTIONS_END$ */
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:amdflash_def.h"
#include	"cp$src:romhead.h"

#define FLASH_DEBUG 0

/*
** Platform-specific definitions for reading and
** writing a byte from/to the flash ROM address 
** space.
*/
#define rb( x )	    in_flash( NULL, x )
#define wb( x, y )  out_flash( NULL, x, y )

typedef enum { WRITE, ERASE } FLASH_OPERATIONS;

/*
********************************************************************************
**              AMD Am29F080 (1 MB) / Am29F016 (2 MB) Flash ROM               **
********************************************************************************
**
**  Command Sequence Definitions
*/
static AMD_FLASH_COMMAND Am29F0xx_command_auto_select[ ] =
    {
	{ 0x5555, 0xAA },
	{ 0x2AAA, 0x55 },
	{ 0x5555, 0x90 },
	{ 0x0000, 0x00 }	/* End of command sequence */
    };

static AMD_FLASH_COMMAND Am29F0xx_command_reset_read[ ] =
    {
	{ 0x5555, 0xAA },
	{ 0x2AAA, 0x55 },
	{ 0x5555, 0xF0 },
	{ 0x0000, 0x00 }	/* End of command sequence */
    };

static AMD_FLASH_COMMAND Am29F0xx_command_byte_program[ ] = 
    {
	{ 0x5555, 0xAA },
	{ 0x2AAA, 0x55 },
	{ 0x5555, 0xA0 },
	{ 0x0000, 0x00 }	/* End of command sequence */
    };

static AMD_FLASH_COMMAND Am29F0xx_command_sector_erase[ ] =
    {
	{ 0x5555, 0xAA },
	{ 0x2AAA, 0x55 },
	{ 0x5555, 0x80 },
	{ 0x5555, 0xAA },
	{ 0x2AAA, 0x55 },
	{ 0x0000, 0x00 }	/* End of command sequence */
    };

/*
** AMD Am29F080 Flash ROM Commands
*/
static AMD_FLASH_COMMANDS Am29F080_commands =
    {
	Am29F0xx_command_reset_read,
	Am29F0xx_command_byte_program,
	Am29F0xx_command_sector_erase
    };

/*
** AMD Am29F080 Flash ROM Sector Architecture
**
**    Sector   Start    End     Sector
**    Number   Addr     Addr     Size
**    ------  -------  -------  ------
**       0    0X00000  0X0FFFF  64 KB
**       1    0X10000  0X1FFFF  64 KB
**       2    0X20000  0X2FFFF  64 KB
**       3    0X30000  0X3FFFF  64 KB
**       4    0X40000  0X4FFFF  64 KB
**       5    0X50000  0X5FFFF  64 KB
**       6    0X60000  0X6FFFF  64 KB
**       7    0X70000  0X7FFFF  64 KB
**       8    0X80000  0X8FFFF  64 KB
**       9    0X90000  0X9FFFF  64 KB
**      10    0XA0000  0XAFFFF  64 KB
**      11    0XB0000  0XBFFFF  64 KB
**      12    0XC0000  0XCFFFF  64 KB
**      13    0XD0000  0XDFFFF  64 KB
**      14    0XE0000  0XEFFFF  64 KB
**      15    0XF0000  0XFFFFF  64 KB
*/
static AMD_SECTOR_INFORMATION Am29F080_layout[ ] = 
    {
	{ 0x00000, 0x10000 },  /* Sector 0  */
	{ 0x10000, 0x10000 },  /* Sector 1  */
	{ 0x20000, 0x10000 },  /* Sector 2  */
	{ 0x30000, 0x10000 },  /* Sector 3  */
	{ 0x40000, 0x10000 },  /* Sector 4  */
	{ 0x50000, 0x10000 },  /* Sector 5  */
	{ 0x60000, 0x10000 },  /* Sector 6  */
	{ 0x70000, 0x10000 },  /* Sector 7  */
	{ 0x80000, 0x10000 },  /* Sector 8  */
	{ 0x90000, 0x10000 },  /* Sector 9  */
	{ 0xA0000, 0x10000 },  /* Sector 10 */
	{ 0xB0000, 0x10000 },  /* Sector 11 */
	{ 0xC0000, 0x10000 },  /* Sector 12 */
	{ 0xD0000, 0x10000 },  /* Sector 13 */
	{ 0xE0000, 0x10000 },  /* Sector 14 */
	{ 0xF0000, 0x10000 },  /* Sector 15 */
	{ 0x00000, 0x00000 }
    };

/*
** AMD Am29F080 Flash ROM Device Definition
*/
static AMD_FLASH_DEVICE Am29F080_device_info =
    {
	"AMD Am29F080",     /* Device Name	    */
	0xD5,		    /* Device ID	    */
	0x100000,	    /* Device Size (1 MB)   */
	0xFF,		    /* Erased Data	    */
	0x30,		    /* Sector Erase Confirm */
	0x01,		    /* Device ID Offset	    */
	Am29F080_layout,    /* Device Layout	    */
	&Am29F080_commands  /* Device Commands	    */
    };

/*
** AMD Am29F016 Flash ROM Commands
*/
static AMD_FLASH_COMMANDS Am29F016_commands =
    {
	Am29F0xx_command_reset_read,
	Am29F0xx_command_byte_program,
	Am29F0xx_command_sector_erase
    };

/*
** AMD Am29F016 Flash ROM Sector Architecture
**
**    Sector   Start      End     Sector
**    Number   Addr       Addr     Size
**    ------  --------  --------  ------
**       0    0X000000  0X00FFFF  64 KB
**       1    0X010000  0X01FFFF  64 KB
**       2    0X020000  0X02FFFF  64 KB
**       3    0X030000  0X03FFFF  64 KB
**       4    0X040000  0X04FFFF  64 KB
**       5    0X050000  0X05FFFF  64 KB
**       6    0X060000  0X06FFFF  64 KB
**       7    0X070000  0X07FFFF  64 KB
**       8    0X080000  0X08FFFF  64 KB
**       9    0X090000  0X09FFFF  64 KB
**      10    0X0A0000  0X0AFFFF  64 KB
**      11    0X0B0000  0X0BFFFF  64 KB
**      12    0X0C0000  0X0CFFFF  64 KB
**      13    0X0D0000  0X0DFFFF  64 KB
**      14    0X0E0000  0X0EFFFF  64 KB
**      15    0X0F0000  0X0FFFFF  64 KB
**      16    0X100000  0X10FFFF  64 KB
**      17    0X110000  0X11FFFF  64 KB
**      18    0X120000  0X12FFFF  64 KB
**      19    0X130000  0X13FFFF  64 KB
**      20    0X140000  0X14FFFF  64 KB
**      21    0X150000  0X15FFFF  64 KB
**      22    0X160000  0X16FFFF  64 KB
**      23    0X170000  0X17FFFF  64 KB
**      24    0X180000  0X18FFFF  64 KB
**      25    0X190000  0X19FFFF  64 KB
**      26    0X1A0000  0X1AFFFF  64 KB
**      27    0X1B0000  0X1BFFFF  64 KB
**      28    0X1C0000  0X1CFFFF  64 KB
**      29    0X1D0000  0X1DFFFF  64 KB
**      30    0X1E0000  0X1EFFFF  64 KB
**      31    0X1F0000  0X1FFFFF  64 KB
*/
static AMD_SECTOR_INFORMATION Am29F016_layout[ ] = 
    {
	{ 0x000000, 0x10000 },  /* Sector 0  */
	{ 0x010000, 0x10000 },  /* Sector 1  */
	{ 0x020000, 0x10000 },  /* Sector 2  */
	{ 0x030000, 0x10000 },  /* Sector 3  */
	{ 0x040000, 0x10000 },  /* Sector 4  */
	{ 0x050000, 0x10000 },  /* Sector 5  */
	{ 0x060000, 0x10000 },  /* Sector 6  */
	{ 0x070000, 0x10000 },  /* Sector 7  */
	{ 0x080000, 0x10000 },  /* Sector 8  */
	{ 0x090000, 0x10000 },  /* Sector 9  */
	{ 0x0A0000, 0x10000 },  /* Sector 10 */
	{ 0x0B0000, 0x10000 },  /* Sector 11 */
	{ 0x0C0000, 0x10000 },  /* Sector 12 */
	{ 0x0D0000, 0x10000 },  /* Sector 13 */
	{ 0x0E0000, 0x10000 },  /* Sector 14 */
	{ 0x0F0000, 0x10000 },  /* Sector 15 */
	{ 0x100000, 0x10000 },  /* Sector 16 */
	{ 0x110000, 0x10000 },  /* Sector 17 */
	{ 0x120000, 0x10000 },  /* Sector 18 */
	{ 0x130000, 0x10000 },  /* Sector 19 */
	{ 0x140000, 0x10000 },  /* Sector 20 */
	{ 0x150000, 0x10000 },  /* Sector 21 */
	{ 0x160000, 0x10000 },  /* Sector 22 */
	{ 0x170000, 0x10000 },  /* Sector 23 */
	{ 0x180000, 0x10000 },  /* Sector 24 */
	{ 0x190000, 0x10000 },  /* Sector 25 */
	{ 0x1A0000, 0x10000 },  /* Sector 26 */
	{ 0x1B0000, 0x10000 },  /* Sector 27 */
	{ 0x1C0000, 0x10000 },  /* Sector 28 */
	{ 0x1D0000, 0x10000 },  /* Sector 29 */
	{ 0x1E0000, 0x10000 },  /* Sector 30 */
	{ 0x1F0000, 0x10000 },  /* Sector 31 */
	{ 0x00000, 0x00000 }
    };

/*
** AMD Am29F016 Flash ROM Device Definition
*/
static AMD_FLASH_DEVICE Am29F016_device_info =
    {
	"AMD Am29F016",     /* Device Name	    */
	0xAD,		    /* Device ID	    */
	0x200000,	    /* Device Size (2 MB)   */
	0xFF,		    /* Erased Data	    */
	0x30,		    /* Sector Erase Confirm */
	0x01,		    /* Device ID Offset	    */
	Am29F016_layout,    /* Device Layout	    */
	&Am29F016_commands  /* Device Commands	    */
    };

/* 
********************************************************************************
**               AMD Am29LV800T / Am29LV800B (1 MB) Flash ROM                 **
********************************************************************************
**
**  Command Sequence Definitions (Byte Programming Mode Only)
*/
static AMD_FLASH_COMMAND Am29LV800_command_auto_select[ ] =
    {
	{ 0xAAAA, 0xAA },
	{ 0x5555, 0x55 },
	{ 0xAAAA, 0x90 },
	{ 0x0000, 0x00 }	/* End of command sequence */
    };

static AMD_FLASH_COMMAND Am29LV800_command_reset_read[ ] =
    {
	{ 0xAAAA, 0xF0 },
	{ 0x0000, 0x00 }	/* End of command sequence */
    };

static AMD_FLASH_COMMAND Am29LV800_command_byte_program[ ] =
    {
	{ 0xAAAA, 0xAA },
	{ 0x5555, 0x55 },
	{ 0xAAAA, 0xA0 },
	{ 0x0000, 0x00 }	/* End of command sequence */
    };

static AMD_FLASH_COMMAND Am29LV800_command_sector_erase[ ] =
    {
	{ 0xAAAA, 0xAA },
	{ 0x5555, 0x55 },
	{ 0xAAAA, 0x80 },
	{ 0xAAAA, 0xAA },
	{ 0x5555, 0x55 },
	{ 0x0000, 0x00 }	/* End of command sequence */
    };

/*
** AMD Am29LV800 Flash ROM Commands
*/
static AMD_FLASH_COMMANDS Am29LV800_commands =
    {
	Am29LV800_command_reset_read,
	Am29LV800_command_byte_program,
	Am29LV800_command_sector_erase
    };

/*
** AMD Am29LV800T Flash ROM Sector Architecture
**
**    Sector   Start    End     Sector
**    Number   Addr     Addr     Size
**    ------  -------  -------  ------
**       0    0x00000  0x0FFFF  64 KB
**       1    0x10000  0x1FFFF  64 KB
**       2    0x20000  0x2FFFF  64 KB
**       3    0x30000  0x3FFFF  64 KB
**       4    0x40000  0x4FFFF  64 KB
**       5    0x50000  0x5FFFF  64 KB
**       6    0x60000  0x6FFFF  64 KB
**       7    0x70000  0x7FFFF  64 KB
**       8    0x80000  0x8FFFF  64 KB
**       9    0x90000  0x9FFFF  64 KB
**      10    0xA0000  0xaFFFF  64 KB
**      11    0xB0000  0xbFFFF  64 KB
**      12    0xC0000  0xcFFFF  64 KB
**      13    0xD0000  0xdFFFF  64 KB
**      14    0xE0000  0xeFFFF  64 KB
**      15    0xF0000  0xF7FFF  32 KB
**      16    0xF8000  0xF9FFF   8 KB
**      17    0xFA000  0xFBFFF   8 KB
**      18    0xFC000  0xFFFFF  16 KB
*/
static AMD_SECTOR_INFORMATION Am29LV800T_layout[ ] = 
    {
	{ 0x00000, 0x10000 },  /* Sector 0  */
	{ 0x10000, 0x10000 },  /* Sector 1  */
	{ 0x20000, 0x10000 },  /* Sector 2  */
	{ 0x30000, 0x10000 },  /* Sector 3  */
	{ 0x40000, 0x10000 },  /* Sector 4  */
	{ 0x50000, 0x10000 },  /* Sector 5  */
	{ 0x60000, 0x10000 },  /* Sector 6  */
	{ 0x70000, 0x10000 },  /* Sector 7  */
	{ 0x80000, 0x10000 },  /* Sector 8  */
	{ 0x90000, 0x10000 },  /* Sector 9  */
	{ 0xA0000, 0x10000 },  /* Sector 10 */
	{ 0xB0000, 0x10000 },  /* Sector 11 */
	{ 0xC0000, 0x10000 },  /* Sector 12 */
	{ 0xD0000, 0x10000 },  /* Sector 13 */
	{ 0xE0000, 0x10000 },  /* Sector 14 */
	{ 0xF0000, 0x08000 },  /* Sector 15 */
	{ 0xF8000, 0x02000 },  /* Sector 16 */
	{ 0xFA000, 0x02000 },  /* Sector 17 */
	{ 0xFC000, 0x04000 },  /* Sector 18 */
	{ 0x00000, 0x00000 }
    };

/*
** AMD Am29LV800T Flash ROM Device Definition
*/
static AMD_FLASH_DEVICE Am29LV800T_device_info =
    {
	"AMD Am29LV800T",   /* Device Name	    */
	0xDA,		    /* Device ID	    */
	0x100000,	    /* Device Size (1 MB)   */
	0xFF,		    /* Erased Data	    */
	0x30,		    /* Sector Erase Confirm */
	0x02,		    /* Device ID Offset	    */
	Am29LV800T_layout,  /* Device Layout	    */
	&Am29LV800_commands /* Device Commands	    */
    };

/*
** AMD Am29LV800B Flash ROM Sector Architecture
**
**    Sector   Start    End     Sector
**    Number   Addr     Addr     Size
**    ------  -------  -------  ------
**       0    0X00000  0X03FFF  16 KB
**       1    0X04000  0X05FFF   8 KB
**       2    0X06000  0X07FFF   8 KB
**       3    0X08000  0X0FFFF  32 KB
**       4    0X10000  0X1FFFF  64 KB
**       5    0X20000  0X2FFFF  64 KB
**       6    0X30000  0X3FFFF  64 KB
**       7    0X40000  0X4FFFF  64 KB
**       8    0X50000  0X5FFFF  64 KB
**       9    0X60000  0X6FFFF  64 KB
**      10    0X70000  0X7FFFF  64 KB
**      11    0X80000  0X8FFFF  64 KB
**      12    0X90000  0X9FFFF  64 KB
**      13    0XA0000  0XAFFFF  64 KB
**      14    0XB0000  0XBFFFF  64 KB
**      15    0XC0000  0XCFFFF  64 KB
**      16    0XD0000  0XDFFFF  64 KB
**      17    0XE0000  0XEFFFF  64 KB
**      18    0XF0000  0XFFFFF  64 KB
*/
static AMD_SECTOR_INFORMATION Am29LV800B_layout[ ] = 
    {
	{ 0x00000, 0x04000 },  /* Sector 0  */
	{ 0x04000, 0x02000 },  /* Sector 1  */
	{ 0x06000, 0x02000 },  /* Sector 2  */
	{ 0x08000, 0x08000 },  /* Sector 3  */
	{ 0x10000, 0x10000 },  /* Sector 4  */
	{ 0x20000, 0x10000 },  /* Sector 5  */
	{ 0x30000, 0x10000 },  /* Sector 6  */
	{ 0x40000, 0x10000 },  /* Sector 7  */
	{ 0x50000, 0x10000 },  /* Sector 8  */
	{ 0x60000, 0x10000 },  /* Sector 9  */
	{ 0x70000, 0x10000 },  /* Sector 10 */
	{ 0x80000, 0x10000 },  /* Sector 11 */
	{ 0x90000, 0x10000 },  /* Sector 12 */
	{ 0xA0000, 0x10000 },  /* Sector 13 */
	{ 0xB0000, 0x10000 },  /* Sector 14 */
	{ 0xC0000, 0x10000 },  /* Sector 15 */
	{ 0xD0000, 0x10000 },  /* Sector 16 */
	{ 0xE0000, 0x10000 },  /* Sector 17 */
	{ 0xF0000, 0x10000 },  /* Sector 18 */
	{ 0x00000, 0x00000 }
    };

/*
** AMD Am29LV800B Flash ROM Device Definition
*/
static AMD_FLASH_DEVICE Am29LV800B_device_info =
    {
	"AMD Am29LV800B",   /* Device Name	    */
	0x5B,		    /* Device ID	    */
	0x100000,	    /* Device Size (1 MB)   */
	0xFF,		    /* Erased Data	    */
	0x30,		    /* Sector Erase Confirm */
	0x02,		    /* Device ID Offset	    */
	Am29LV800B_layout,  /* Device Layout	    */
	&Am29LV800_commands /* Device Commands	    */
    };

/*
** List of Known Auto Select Command Sequences 
*/
static AMD_FLASH_COMMAND *auto_select_sequences[ ] =
    {
	Am29F0xx_command_auto_select,
	Am29LV800_command_auto_select,
	NULL
    };

/*
** List of Supported Flash ROM Devices
*/
static AMD_FLASH_DEVICE *supported_devices[ ] =
    {
	&Am29LV800T_device_info,
	&Am29LV800B_device_info,
	&Am29F080_device_info,
	&Am29F016_device_info,
	NULL
    };

/*
** Forward Declarations
*/
unsigned int AMD_set_read_mode( unsigned int offset );
unsigned int AMD_write_byte( unsigned int offset, unsigned char data );
unsigned int AMD_erase_sector( unsigned int offset );
unsigned int AMD_sector_align( unsigned int offset );
unsigned char AMD_read_byte( unsigned int offset );
unsigned int AMD_sector_size( unsigned int offset );
unsigned int AMD_get_last_error( void );

/*
** The Flash Driver Structure
*/
FLASH_DRIVER flash_driver = 
    {
	NULL,			/* Device Name - filled in by init routine  */
	AMD_set_read_mode,	/* Set Read Mode function		    */
	AMD_write_byte,		/* Write Byte function			    */
	AMD_erase_sector,	/* Erase Block function			    */
	AMD_sector_align,	/* Align Block function			    */
	AMD_read_byte,		/* Read Byte function			    */
	AMD_sector_size,	/* Block Size function			    */
	AMD_get_last_error,	/* Get Last Error function		    */
	0,			/* Device Size - filled in by init routine  */
	0			/* Erased Data - filled in by init routine  */
    };

/*
** Flash Driver Information
*/
static struct _FLASH_DRIVER_INFORMATION {
    FLASH_DRIVER *fdp;
    AMD_FLASH_DEVICE *flp;
    unsigned int last_error;
} driver_info = { NULL, NULL, 0 };


static void command_sequence ( unsigned int offset, AMD_FLASH_COMMAND *cmd )
{
    unsigned int i, j;

    j = offset & ~( flash_driver.device_size - 1 );
    for ( i = 0;  ( cmd[i].offset != 0 ) || ( cmd[i].data != 0 );  i++ ) {
    	wb( cmd[i].offset | j, cmd[i].data );
    }
}


static void set_last_error( unsigned int error )
{
    driver_info.last_error = error;
}


static unsigned int check_status( 
    unsigned int offset,
    FLASH_OPERATIONS operation,
    unsigned char data )
{
    unsigned char tmp;
    unsigned int pass = FALSE;
    unsigned int i;
/*
**  AMD Data Polling Algorithm:
**
**                  Start
**                    |
**                    |
**                    V
**    +---------->Read Byte
**    |               |
**    |               |
**    |               V
**    |          DQ7 == Data? ----------+
**    |               |          Yes    |
**    |               | No              |
**    |               |                 |
**    |               V                 |
**    +---------- DQ5 == 1?             |
**         No         |                 |
**                    | Yes             |
**                    |                 |
**                    V                 |
**                Read Byte             |
**                    |                 |
**                    |                 |
**                    V                 |
**               DQ7 == Data? --------->+
**                    |          Yes    |
**                    | No              |
**                    |                 |
**                    V                 V
**                  Fail              Pass
**
**  A timeout has been added to avoid an infinite loop
**
*/
    for ( i = AMD_TIMEOUT_VALUE;  !pass && ( i > 0 );  i-- ) {
    	tmp = driver_info.fdp->read_byte_function( offset );
	if ( ( ( tmp ^ data ) & 0x80 ) == 0 ) {
	    pass = TRUE;
	}
	else if ( tmp & 0x20 ) {
	    tmp = driver_info.fdp->read_byte_function( offset );
	    if ( ( ( tmp ^ data ) & 0x80 ) == 0 ) {
	    	pass = TRUE;
	    }
	    else {
	    	break;
	    }
	}
	krn$_micro_delay( 1 );
    }
    if ( pass ) {
    	return msg_success;
    }
    else {
    	set_last_error( operation | 0x80000000 );
	return msg_failure;
    }
}


unsigned int AMD_set_read_mode( unsigned int offset )
{
    command_sequence( offset, driver_info.flp->commands->reset_read_command );
    return msg_success;
}


unsigned int AMD_write_byte( unsigned int offset, unsigned char data )
{
    unsigned int status = msg_success;

#if !FLASH_DEBUG
    command_sequence( offset, driver_info.flp->commands->byte_program_command );
    wb( offset, data );
    status = check_status( offset, WRITE, data );
    driver_info.fdp->set_read_mode_function( offset );
#endif
    return status;
}


unsigned int AMD_erase_sector( unsigned int offset )
{
    unsigned int status = msg_success;

#if FLASH_DEBUG
    unsigned int sector;

    sector = ( offset / driver_info.fdp->block_size_function( offset ) ) - 1;
    qprintf( "Erasing sector %d at offset %x\n", sector, offset );
#else
    command_sequence( offset, driver_info.flp->commands->sector_erase_command );
    wb( offset, driver_info.flp->sector_erase_confirm );
    status = check_status( offset, ERASE, driver_info.fdp->erased_data );
    driver_info.fdp->set_read_mode_function( offset );
#endif
    return status;
}


unsigned int AMD_sector_align( unsigned int offset )
{
    AMD_SECTOR_INFORMATION *sa;
    unsigned int base = 0;
    unsigned int i, j;

    j = offset & ( flash_driver.device_size - 1 );
    sa = driver_info.flp->layout;
    for ( i = 0;  sa[i].size != 0;  i++ ) {
    	if ( ( j >= sa[i].offset ) && ( j < ( sa[i].offset + sa[i].size ) ) ) {
	    base = sa[i].offset;
	    base |= ( offset & ~( flash_driver.device_size - 1 ) );
	    break;
	}
    }
    return base;
}


unsigned char AMD_read_byte( unsigned int offset )
{
    return( rb( offset ) );
}


unsigned int AMD_sector_size( unsigned int offset )
{
    AMD_SECTOR_INFORMATION *sa;
    unsigned int size = 0;
    unsigned int i;

    sa = driver_info.flp->layout;
    offset &= ( flash_driver.device_size - 1 );
    for ( i = 0;  sa[i].size != 0;  i++ ) {
    	if ( ( offset >= sa[i].offset ) && ( offset < ( sa[i].offset + sa[i].size ) ) ) {
	    size = sa[i].size;
	    break;
	}
    }
    return size;
}


unsigned int AMD_get_last_error ( void )
{
    return( driver_info.last_error );
}


unsigned int init_flash_rom ( void )
{
    unsigned int i, j;
    unsigned char id;
    unsigned int offset = 0;
/*
** Try all the known auto-select command sequences until a device is found.
*/
    for ( i = 0; auto_select_sequences[i] != NULL;  i++ ) {
/*
** Send the auto-select command sequence and read the manufacturer's ID.
*/
    	command_sequence( offset, auto_select_sequences[i] );
	id = rb( 0 );
	if ( id == AMD_MANUFACTURER_ID ) {
/*
** It's an AMD part, search through the list of supported flash devices to
** try and find a match for the device ID.
*/
	    for ( j = 0;  supported_devices[j] != NULL;  j++ ) {
	    	id = rb( supported_devices[j]->device_id_offset );
		if ( id == supported_devices[j]->device_id ) {
/*
** A supported device was found, initialize the flash driver structure ...
*/
		    flash_driver.device_name = supported_devices[j]->device_name;
		    flash_driver.device_size = supported_devices[j]->device_size;
		    flash_driver.erased_data = supported_devices[j]->erased_data;
#if FLASH_DEBUG
		    qprintf( "\n%s\tsize = %x\terase data = %x\n", 
			      flash_driver.device_name,
			      flash_driver.device_size,
			      flash_driver.erased_data );
#endif
/*
** Fill in the driver information structure
*/
		    driver_info.fdp = &flash_driver;
		    driver_info.flp = supported_devices[j];
		    driver_info.last_error = 0;
/*
** Set the flash ROM device to read mode
*/
		    driver_info.fdp->set_read_mode_function( offset );
		    return msg_success;
		}
	    }
	}
    }
    return msg_failure;
}


unsigned int read_flash_rom( unsigned int src, int size, unsigned char *dst )
{
    int i;

#if FLASH_DEBUG
    qprintf( "Reading %d bytes from flash location %x\n", size, src );
#endif
    for ( i = 0; i < size; i++ ) {
    	dst[i] = driver_info.fdp->read_byte_function( src );
	src++;
    }
    return msg_success;
}


unsigned int write_flash_rom( unsigned int dst, int size, unsigned char *src )
{
    unsigned int base = 0xFFFFFFFF;
    unsigned int i;
    unsigned char *buf = NULL;

#if EEROM_SIZE
/*
** If we are writing the EEROM device, then we need to do a 
** read-modify-write operation on the whole block to preserve
** the contents of the ISA configuration table and environment
** variables stored there.
** Only do this code if we support an EEROM in the flash. 
*/
    if ( ( driver_info.fdp->block_align_function( dst ) >= EEROM_SECTOR_START ) &&
	 ( driver_info.fdp->block_align_function( dst ) < ( EEROM_SECTOR_START + EEROM_SIZE ) ) ) {
	buf = ( char * )dyn$_malloc( EEROM_SIZE, DYN$K_SYNC, 0, 0, 
	 ( struct ZONE * )0 );
	read_flash_rom( EEROM_SECTOR_START, EEROM_SIZE, buf );
	memcpy( buf + ( dst - EEROM_SECTOR_START ), src, size );
	src = buf;
	dst = EEROM_SECTOR_START;
	size = EEROM_SIZE;
    }
#endif

/*
** Write the flash block(s)
*/
#if FLASH_DEBUG
    i = size / driver_info.fdp->block_size_function( dst );
    if ( size % driver_info.fdp->block_size_function( dst ) )
	i++;
    qprintf( "Writing %d bytes, %d block(s), starting at flash location %x\n", size, i, dst );
#endif
    for ( i = 0;  i < size;  i++ ) {
	if ( driver_info.fdp->block_align_function( dst ) != base ) {
	    base = driver_info.fdp->block_align_function( dst );
	    if ( driver_info.fdp->erase_block_function( base ) != msg_success ) {
	    	return msg_failure;
	    }
	}
	if ( driver_info.fdp->write_byte_function( dst, src[i] ) != msg_success ) {
	    return msg_failure;
	}
	dst++;
    }
    if ( buf != NULL ) {
    	dyn$_free( buf, DYN$K_SYNC );
    }
    return msg_success;
}
