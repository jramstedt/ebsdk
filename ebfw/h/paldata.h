#ifndef PALDATA_H_LOADED
#define PALDATA_H_LOADED
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
 *  $Id: paldata.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 *  $Log: paldata.h,v $
 *  Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 *  Initial CVS checkin
 *
 * Revision 1.3  1997/08/25  20:52:08  fdh
 * Removed opsoleted include file...
 *
 * Revision 1.2  1997/07/10  00:34:49  fdh
 * included ev6_imp.h.
 *
 * Revision 1.1  1997/02/21  03:55:38  fdh
 * Initial revision
 *
 */

#ifndef MAKEDEPEND
#include "impure.h"
#endif

#ifndef LANGUAGE_ASSEMBLY
extern ul PalImpureBase;
extern ul *SavedIntegerRegisters;
extern ul *SavedFloatingPointRegisters;
extern ul *SavedPALTempRegisters;
extern ul *SavedExceptionAddressRegister;
extern ul SavedProcessorStatusRegister;
#endif /* LANGUAGE_ASSEMBLY */
#endif /* PALDATA_H_LOADED */
