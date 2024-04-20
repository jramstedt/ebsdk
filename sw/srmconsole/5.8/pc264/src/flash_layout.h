/* file:	flash_layout.h
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
 * Abstract:	Pci definitions.
 *
 * Author: Eric Goehl
 *
 * Modifications:
 *
 *	er	24-Sep-1997	Added io_get_max_hose() macro. Cleaned up
 *			        for use with tsunami_io.c
 *	er	25-Jul-1997	Changed hose probe order via HOSE macro.
 *	er	14-Jul-1997	Cleaned up, fixed PCI_MIN_SLOT and
 *				PCI_MAX_SLOT constants.
 *	egg	13-Nov-1996	Initial port of eb164_io.h
 *
 */
#ifndef flash_layout_h		/* Prevent multiple includes. */
#define flash_layout_h

#if GLXYTRAIN
#define FLASH_ROM_START			0x000000
#define FLASH_ROM_SIZE			0x200000	/* size of device in bytes (2Mb) */
#define FLASH_ROM_END			0x200000
#define SECTOR_SIZE				0x000000	/* Just a placed holder not used anymore due to variable sector lengths. */

#define SRMROM_SECTOR_START		0x0C0000
#define SRMROM_SIZE				0x0B0000

#define DIAGSROM_SECTOR_START	0x030000
#define DIAGSROM_SIZE			0x010000

#define EEROM_SECTOR_START		0x1E0000
#define EEROM_SIZE				0x010000
#endif

#if NAUTILUS
#define FLASH_ROM_START			0x000000 
#define FLASH_ROM_SIZE			0x200000	/* size of device in bytes (2Mb) */
#define FLASH_ROM_END			0x200000
#define SECTOR_SIZE				0x000000	/* Just a placed holder not used anymore due to variable sector lengths. */

#define DIAGSROM_SECTOR_START	0x060000 
#define DIAGSROM_SIZE			0x040000

#define SRMROM_SECTOR_START		0x0A0000 
#define SRMROM_SIZE				0x0B0000

#define EEROM_SECTOR_START		0x1F0000
#define EEROM_SIZE				0x010000
#endif

#if TINOSA
#define FLASH_ROM_START			0x000000 
#define FLASH_ROM_SIZE			0x200000	/* size of device in bytes (2Mb) */
#define FLASH_ROM_END			0x200000
#define SECTOR_SIZE				0x000000	/* Just a placed holder not used anymore due to variable sector lengths. */
                                    
#define SRMROM_SECTOR_START		0x0C0000 
#define SRMROM_SIZE				0x0B0000

#define DIAGSROM_SECTOR_START	0x030000 
#define DIAGSROM_SIZE			0x010000

#define EEROM_SECTOR_START		0x1E0000
#define EEROM_SIZE				0x010000
#endif

#if SWORDFISH
#define FLASH_ROM_START	        0x000000 
#define SECTOR_SIZE				0x010000 

#define SROM_SECTOR_START		0x000000 
#define SROM_SIZE				0x040000
                                    
#define EEROM_SECTOR_START    	0x050000 
#define EEROM_SIZE				0x010000
                                    
#define SRMROM_SECTOR_START		0x060000 
#define SRMROM_SIZE				0x0B0000

#define NTROM_SECTOR_START		0x170000 
#define NTROM_SIZE				0x090000

#define FLASH_ROM_END	        0x1F0000

#define	FLASH_ROM_SIZE    		0x200000     /* size of device in bytes (2Mb) */
#endif

#if SHARK
#define FLASH_ROM_START	        0x000000
#define SECTOR_SIZE				0x010000 

#define SROM_SECTOR_START		0x000000 
#define SROM_SIZE				0x040000
                                    
#define EEROM_SECTOR_START    	0x040000 
#define EEROM_SIZE				0x010000

#define DIAGSROM_SECTOR_START	0x050000 
#define DIAGSROM_SIZE			0x040000

#define SRMROM_SECTOR_START		0x090000 
#define SRMROM_SIZE				0x0B0000

#define FLASH_ROM_END	        0x1F0000

#define	FLASH_ROM_SIZE    		0x200000     /* size of device in bytes (2Mb) */
#endif


#ifndef FLASH_ROM_START
/* 
** PC264 definitions required for from_driver.c 
** Am29F016 2Mb flash
*/
#define FLASH_ROM_START	        0x0 
#define SECTOR_SIZE				0x10000 

#define SROM_SECTOR_START		0x0 
#define SROM_SIZE				0x40000
                                    
#define FSROM_SECTOR_START		0x40000
#define FSROM_SIZE				0x10000
                                    
#define EEROM_SECTOR_START    	0x50000 
#define EEROM_SIZE				0x10000
                                    
#define SRMROM_SECTOR_START		0x60000 
#define SRMROM_SIZE				0x110000

#define NTROM_SECTOR_START		0x170000 
#define NTROM_SIZE				0x90000

#define FLASH_ROM_END	        0x1F0000

#define	FLASH_ROM_SIZE    		0x200000     /* size of device in bytes (2Mb) */
#endif

#endif
