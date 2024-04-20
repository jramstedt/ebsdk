/*
 * file:	console_hal_data.h
 *
 * Copyright (C) 1990, 1991, 1992, 1993 by
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
 * Abstract:	The code contains data to help abstract the hardware
 *              that the console uses.  It should only ever be included
 *              once.  No more than 1 file should include this.  That file
 *              should be KERNEL.C.
 *
 * Author:	Dennis Mazur
 *
 * Modifications:
 *	
 *	djm	27-May-1993	Initial. 
 */

/*+***************************************************************************/
/*                           SYS_BUS_ADR                                     */
/*****************************************************************************/

#if 0
#if NEON||TURBO              
char *sys_bus_name = "LSB";
int sys_high_adr   = 0x00000003;
int sys_bus_adr[SYS_BUS_CNT] = 
		     { 0xF8000000,           /* slot 0 */
                       0xF8400000,           /* slot 1 */
                       0xF8800000,           /* slot 2 */
                       0xF8C00000,           /* slot 3 */
                       0xF9000000,           /* slot 4 */
                       0xF9400000,           /* slot 5 */
                       0xF9800000,           /* slot 6 */
                       0xF9C00000,           /* slot 7 */
                       0xFA000000            /* slot 8 */
                     };
int mbx_ptr_adr[5] = 
		     { 0xFA000C00,           /* slot 4  emulated */
		       0xFA000C00,           /* slot 5  emulated */
		       0xFA000C00,           /* slot 6  emulated */
		       0xFA000C00,           /* slot 7  emulated */
		       0xFA000C00            /* slot 8 */
                     };
#endif
#endif

#if TURBO
char *sys_bus_name = "TLSB";
int sys_high_adr   = 0x000000FF;
int sys_bus_adr[SYS_BUS_CNT] = 
		     { 0x88000000,           /* slot 0 */
                       0x88400000,           /* slot 1 */
                       0x88800000,           /* slot 2 */
                       0x88C00000,           /* slot 3 */
                       0x89000000,           /* slot 4 */
                       0x89400000,           /* slot 5 */
                       0x89800000,           /* slot 6 */
                       0x89C00000,           /* slot 7 */
                       0x8A000000            /* slot 8 */
                     };
int mbx_ptr_adr[5] = 
		     { 0x89000C00,           /* slot 4 */
                       0x89400C00,           /* slot 5 */
                       0x89800C00,           /* slot 6 */
                       0x89C00C00,           /* slot 7 */
                       0x8A000C00            /* slot 8 */
                     };
#endif

#if CALYPSO
char *sys_bus_name = "XMI";
int sys_high_adr   = 0x00000000;
int sys_bus_adr[SYS_BUS_CNT] = 
		     { 0x21800000,           /* slot 0 */
                       0x21880000,           /* slot 1 */
                       0x21900000,           /* slot 2 */
                       0x21980000,           /* slot 3 */
                       0x21A00000,           /* slot 4 */
                       0x21A80000,           /* slot 5 */
                       0x21B00000,           /* slot 6 */
                       0x21B80000,           /* slot 7 */
                       0x21C00000,           /* slot 8 */
                       0x21C80000,           /* slot 9 */
                       0x21D00000,           /* slot A */
                       0x21D80000,           /* slot B */
                       0x21E00000,           /* slot C */
                       0x21E80000,           /* slot D */
                       0x21F00000,           /* slot E */
                       0x21F80000            /* slot F */
                     };
int mbx_ptr_adr[5] = 
		     { 0xFA000C00,           /* slot 4  emulated */
		       0xFA000C00,           /* slot 5  emulated */
		       0xFA000C00,           /* slot 6  emulated */
		       0xFA000C00,           /* slot 7  emulated */
		       0xFA000C00            /* slot 8 */
                     };
#endif
/*-***************************************************************************/
