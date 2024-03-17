#ifndef __LEDCODES_H_LOADED
#define __LEDCODES_H_LOADED
/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

/*
 *  $Id: ledcodes.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 */

/*
 * $Log: ledcodes.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.7  1997/08/12  02:35:37  fdh
 * Added codes associated with the SROM serial port initialization.
 *
 * Revision 1.6  1997/07/10  00:28:04  fdh
 * Added floppy LED codes.
 *
 * Revision 1.5  1996/08/16  17:42:55  fdh
 * Added the following definitions...
 *  led_k_keyboard_inited   0xFA    Keyboard initialized
 *  led_k_graphics_failed   0xF9    Graphics failed
 *  led_k_console_inited    0xF8    Console ports initialized
 *  led_k_checking_rtc      0xF7    Checking Real Time Clock
 *  led_k_init_done         0x00    powerup init done
 *
 * Revision 1.4  1995/11/27  23:51:41  fdh
 * Changed flash command LED code to resolve conflict with
 * Fail-Safe booter LED-code.
 *
 * Revision 1.3  1995/11/27  19:14:01  cruz
 * Added code for loading firmware update tool.
 *
 * Revision 1.2  1994/11/28  19:39:44  fdh
 * Added Startup LED codes.
 *
 * Revision 1.1  1994/11/08  21:42:29  fdh
 * Initial revision
 *
 */

#define led_k_ksp_initialized   0xFF    /* Kernel Stack Pointer Initialized */
#define led_k_sysdata_inited    0xFE    /* System Data Structure Initialized */
#define led_k_init_IO           0xFD    /* About to complete IO bus initialization */
#define led_k_IO_inited         0xFC    /* IO bus initialization complete */
#define led_k_uart_inited       0xFB    /* UARTs initialized */
#define led_k_keyboard_inited   0xFA    /* Keyboard initialized */
#define led_k_graphics_failed   0xF9    /* Graphics failed */
#define led_k_console_inited    0xF8    /* Console ports initialized */
#define led_k_checking_rtc      0xF7    /* Checking Real Time Clock */

#define led_k_autobaud		0xAB	/* Waiting for SROM Port Autobaud */
#define led_k_autobaud_done	0xA0	/* SROM Port Autobaud Complete */

#define led_k_init_done         0x00    /* powerup init done */

/*
 * Special ledcodes used in fsboot prg.
 */
#define led_k_load_fwupdate     0xBF    /* Loading firmware update tool from floppy. */


/*
 * Flash command LED codes.
 */
#define led_k_flash_entered	0xB0	/* Flash Utility Entered */
#define led_k_flash_found	0xB2	/* A FLASH ROM was found. */
#define led_k_erase_flash	0xB3	/* About to erase flash. */
#define led_k_write_flash	0xB4	/* About to write flash. */
#define led_k_verify_flash	0xB5	/* About to verify flash. */
#define led_k_flash_exit	0xB6	/* Program finished. */


/*
 * FLOPPY LED codes.
 */
#define led_k_floppy_read_error		0xE0
#define led_k_floppy_write_error	0xE1

#endif /* __LEDCODES_H_LOADED */
