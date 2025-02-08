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

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: isa_buff.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif

/*
 * This module allocates buffers to which ISA devices can DMA
 *
 * $Log: isa_buff.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.5  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.4  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.3  1994/06/03  20:10:11  fdh
 * Replaced <stdio.h> with <stddef.h>.
 * Removed unnecessary declaration.
 *
 * Revision 1.2  1993/10/01  16:02:18  berent
 * made the base address for the buffers user controllable
 *
 * Revision 1.1  1993/08/06  10:01:29  berent
 * Initial revision
 *
 *
 */

#include <stddef.h>
#include "isa_buff.h"

extern char * etherbuffers;

static char * next_buffer_pointer;

/* isa_buffer_init_module - initialise the module
 */
void isa_buffer_init_module(void)
{
    /* Allocate all buffers above 1M since the range below 1M is used for programmed I/O */
    next_buffer_pointer = etherbuffers;
}

/* isa_buffer_alloc - allocate a region of memory accessable by ISA DMA devices
 *
 * Argument:
 *    size - number of bytes to allocate
 *
 * Result:
 *    Pointer to start of buffer
 */
void * isa_buffer_alloc(int size)
{
    void * result = (void *) next_buffer_pointer;
    next_buffer_pointer += size;
    return result;
}


