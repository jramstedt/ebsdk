#ifndef __ERRNO_H_LOADED
#define __ERRNO_H_LOADED
/*****************************************************************************

       Copyright 1995 Digital Equipment Corporation,
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
 *  $Id: errno.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $
 */

/*
 *      This file implements the standard C errno.h file.
 *      Author:
 *          Rogelio R. Cruz, Digital Equipment Corporation, 9/11/95
 *
 *   $Log: errno.h,v $
 *   Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 *   Initial CVS checkin
 *
 * Revision 1.3  1997/02/21  04:16:07  fdh
 * included dbmtypes.h.
 *
 * Revision 1.2  1995/11/09  21:50:51  cruz
 * Changed type of errno to "si" instead of "ui".
 *
 * Revision 1.1  1995/09/15  19:28:11  cruz
 * Initial revision
 *
 *
 */


/*
 *  The errno variable is partition into 4 components of the size
 *  specified below.
 *
 *    Bits 0-3 (inclusive) -> Error1 (file operation errors)
 *    Bits 4-7             -> Error2 (not used)
 *    Bits 8-11            -> Error3 (fat driver errors)
 *    Bits 12-15           -> Error4 (floppy driver errors)
 */

#define ERROR1_START_BIT        0x0     /* Bit Number */
#define ERROR1_START_VALUE      (1<<ERROR1_START_BIT)
#define ERROR2_START_BIT        0x4     /* Bit Number */
#define ERROR2_START_VALUE      (1<<ERROR2_START_BIT)
#define ERROR3_START_BIT        0x8     /* Bit Number */
#define ERROR3_START_VALUE      (1<<ERROR3_START_BIT)
#define ERROR4_START_BIT        0x12     /* Bit Number */
#define ERROR4_START_VALUE      (1<<ERROR4_START_BIT)

#define ERROR1_MASK             (0xF)
#define ERROR2_MASK             (0xF)
#define ERROR3_MASK             (0xF)
#define ERROR4_MASK             (0xF)

#define NO_ERRORS               0x0


/* ======================================================================
 * =                      VARIABLE DECLARATIONS                         =
 * ======================================================================
 */

#include "dbmtypes.h"
extern si errno;

#endif /* __ERRNO_H_LOADED */
