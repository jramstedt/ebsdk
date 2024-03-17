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

#ifndef LINT
static char *rcsid = "$Id: net_buff.c,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $";
#endif

/*
 * This module allocates buffers to which network devices can DMA.
 * The base address of the allocation heap is defined by the
 * ebuff command.
 *
 * NOTE: It is the responsibility of each device to check that
 * any buffers allocated are viable for it.  
 *
 * $Log: net_buff.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.8  1995/11/27  16:07:01  cruz
 * Removed explicit initialization of etherbuffers.  It's now
 * initialized in main().
 *
 * Revision 1.7  1995/10/21  05:27:15  fdh
 * Corrected the data type for an assignment.
 *
 * Revision 1.6  1995/10/20  20:04:51  cruz
 * Cleanup for lint
 *
 * Revision 1.5  1995/10/20  18:45:13  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.4  1994/08/05  20:17:17  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/06/28  20:11:47  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.2  1994/06/03  20:11:50  fdh
 * Replaced <stdio.h> with <stddef.h>.
 * Removed unnecessary declaration
 *
 * Revision 1.1  1993/11/30  10:14:17  rusling
 * Initial revision
 *
 * Revision 1.2  1993/10/01  16:02:18  berent
 * made the base address for the buffers user controllable
 *
 * Revision 1.1  1993/08/06  10:01:29  berent
 * Initial revision
 *
 *
 */

#include "ether.h"

char *etherbuffers;

static char * next_buffer_pointer;

/* net_buffer_init_module - initialise the module
 */
void net_buffer_init_module(void)
{
/* 
 * Allocate all buffers above 1M since the range below 1M is used for 
 * programmed I/O 
 */
    next_buffer_pointer = etherbuffers;
}

/* net_buffer_alloc - allocate a region of memory accessable by ISA DMA devices
 *
 * Argument:
 *    size - number of bytes to allocate
 *
 * Result:
 *    Pointer to start of buffer
 */
void * net_buffer_alloc(int size)
{
    void * result = (void *) next_buffer_pointer;
    next_buffer_pointer += size;
    return result;
}


