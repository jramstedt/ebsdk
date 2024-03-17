#ifndef __ISA_BUS_H_LOADED
#define __ISA_BUS_H_LOADED
/*****************************************************************************

       Copyright 1993, 1994, 1995 Digital Equipment Corporation,
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
 * $Id: isa_bus.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $
 */

/*
 * MODULE DESCRIPTION:
 *
 *    Parameters and logicals for ISA bus driver dependencies in EB64 monitor
 *
 * HISTORY:
 *
 * $Log: isa_bus.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.7  1996/08/06  22:08:47  fdh
 * Added a few definitions
 *
 * Revision 1.6  1996/05/26  20:03:36  fdh
 * Modified definitions.
 *
 * Revision 1.5  1995/10/20  18:44:09  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.4  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.2  1994/06/03  20:09:09  fdh
 * Removed unnecessary include file.
 *
 * Revision 1.1  1993/08/06  10:15:04  berent
 * Initial revision
 *
 *
 */

/*
 * Define offset addresses need to reset the
 * Programmable Interrupt Controller - 2 x 82C59A cascaded megacells
 */

#define IO_82C59_PIC1 0xA0
#define IO_82C59_PIC2 0x20

/*
 * Define DMA controller offset addresses, specific
 * interest is to write the masks for DMA2
 */

#define IO_82C37_DMA1_CH   0x00
#define IO_82C37_DMA1_CSR  0x08
#define IO_82C37_DMA1_WRMSK 0x0A
#define IO_82C37_DMA1_MODE 0x0B

#define IO_82C37_DMA2_CH   0xC0
#define IO_82C37_DMA2_CSR  0xD0
#define IO_82C37_DMA2_WRMSK 0xD4
#define IO_82C37_DMA2_MODE 0xD6

#define DMA_DEMAND_MODE		0
#define DMA_SINGLE_MODE		1
#define DMA_BLOCK_MODE		2
#define DMA_CASCADE_MODE	3

#endif /* __ISA_BUS_H_LOADED */
