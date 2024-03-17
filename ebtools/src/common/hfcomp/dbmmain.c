/*****************************************************************************

Copyright © 1994, Digital Equipment Corporation, Maynard, Massachusetts. 

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
**
** FACILITY:	
**
**	EBxx Software Tools - makerom
** 
** FUNCTIONAL DESCRIPTION:
** 
**     entry point from palcode to the stand-alone decompressor
**     for firmware loaded from flash or rom
** 
** AUTHOR: Ken Curewitz
**
** CREATION-DATE: 01-Feb-1995
** 
** MODIFIED BY: 
** 
*/
#ifndef lint
static char *RCSid = 
  "$Id: dbmmain.c,v 1.1.1.1 1998/12/29 21:36:25 paradis Exp $";
#endif

/*
 * $Log: dbmmain.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:25  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1995/12/07  07:43:18  fdh
 * Removed hardcoded address for the base of the compressed image.
 * This is now defined in header.h.
 * Now returns the pointer to the base of the decompressed image.
 *
 * Revision 1.3  1995/02/28  03:27:27  fdh
 * Changed offset to compressed image.
 *
 * Revision 1.2  1995/02/01  20:16:35  curewitz
 * initial version
 *
 */
#include "decomp.h"
#undef DEBUG

extern unsigned int decompress();

unsigned int
main()
{
  unsigned int target_address;

#ifdef DEBUG
  uart_init();
  printf("Welcome to decompress.\n");
#endif
  target_address = decompress();
#ifdef DEBUG
  printf("returned from decompress.\n");
#endif
  return(target_address);
}
