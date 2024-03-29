#ifndef __ISA_BUFFER_H_LOADED
#define __ISA_BUFFER_H_LOADED
/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
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
 * $Id: isa_buff.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $
 */

/*
 * MODULE DESCRIPTION:
 * 
 *     Buffer allocator for ISA DMA buffers. Note that this buffer allocator
 *     does not allow buffers to be freed.
 *
 * HISTORY:
 * 
 * $Log: isa_buff.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1994/08/05  20:13:47  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.2  1994/06/20  14:18:59  fdh
 * Fixup header file preprocessor #include conditionals.
 *
 * Revision 1.1  1993/08/06  10:31:10  berent
 * Initial revision
 *
 *
 */


/* isa_buffer_init_module - initialise the module
 */
extern void isa_buffer_init_module(void);

/* isa_buffer_alloc - allocate a region of memory accessable by ISA DMA devices
 *
 * Argument:
 *    size - number of bytes to allocate
 *
 * Result:
 *    Pointer to start of buffer
 */
extern void * isa_buffer_alloc(int size);

#endif /* __ISA_BUFFER_H_LOADED */
