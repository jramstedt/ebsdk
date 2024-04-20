/*
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 */

/*
**++
**  FACILITY:  
**  
**	Alpha SRM Console Firmware
**
**  MODULE DESCRIPTION:
**
**      Definitions for the AMD flash ROM driver.
**
**  AUTHORS:
**
**      Eric A. Rasmussen
**
**  CREATION DATE:  11-Mar-1997
**
**  MODIFICATION HISTORY:
**
**      {@tbs@}...
**--
*/
#ifndef __AMDFLASH_DEF_H
#define __AMDFLASH_DEF_H

/*
**
**  MACRO DEFINITIONS
**
*/
#define AMD_MANUFACTURER_ID	    0x01
#define FUJITSU_MANUFACTURER_ID	0x04
#define AMD_TIMEOUT_VALUE	    5000000

typedef struct _AMD_FLASH_COMMAND {
    unsigned int offset;
    unsigned char data;
    unsigned char pad[3];
} AMD_FLASH_COMMAND;

typedef struct _AMD_FLASH_COMMANDS {
    AMD_FLASH_COMMAND *reset_read_command;
    AMD_FLASH_COMMAND *byte_program_command;
    AMD_FLASH_COMMAND *sector_erase_command;
} AMD_FLASH_COMMANDS;
 
typedef struct _AMD_SECTOR_INFORMATION {
    unsigned int offset;
    unsigned int size;
} AMD_SECTOR_INFORMATION;
 
typedef struct _AMD_FLASH_DEVICE {
    char *device_name;
    unsigned char device_id;
    unsigned int device_size;
    unsigned char erased_data;
    unsigned char sector_erase_confirm;
    unsigned char device_id_offset;
    AMD_SECTOR_INFORMATION *layout;
    AMD_FLASH_COMMANDS *commands;
} AMD_FLASH_DEVICE;

typedef struct _FLASH_DRIVER {
    unsigned char *device_name;
    unsigned int ( *set_read_mode_function )( unsigned int offset );
    unsigned int ( *write_byte_function )( unsigned int offset, unsigned char data );
    unsigned int ( *erase_block_function )( unsigned int offset );
    unsigned int ( *block_align_function )( unsigned int offset );
    unsigned char ( *read_byte_function )( unsigned int offset );
    unsigned int ( *block_size_function )( unsigned int offset );
    unsigned int ( *get_last_error_function )( void );
    unsigned int device_size;
    unsigned char erased_data;
} FLASH_DRIVER;

#endif	/* __AMDFLASH_DEF_H */
