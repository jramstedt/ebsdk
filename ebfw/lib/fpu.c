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
static char *rcsid = "$Id: fpu.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: fpu.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.5  1997/12/15  20:44:02  pbell
 * Merged dp264 changes.
 *
 * Revision 1.4  1997/06/09  18:58:08  fdh
 * Corrected a typo.
 *
 * Revision 1.3  1997/06/09  15:35:30  fdh
 * Cleaned up function definitions.
 *
 * Revision 1.2  1997/02/25  02:44:47  fdh
 * Report calls to stubs.
 *
 * Revision 1.1  1997/02/21  03:28:57  gries
 * Initial revision
 *
 */

#ifndef __GNUC__
#include <machine/fpu.h>
#include <c_asm.h>
#endif

#ifndef __NO_FLOATING_POINT
void ieee_set_fp_control(const long fp_control)
{
#if __GNUC__
  __asm__ (
    "mt_fpcr  %0;"
    : 
    : "f" (*(double *)&fp_control)
  );
#else
  dasm("mt_fpcr %0;",*(double *)&fp_control);
#endif
}

long ieee_get_fp_control(void)
{
  double t;

#if __GNUC__
  __asm__ (
    "mf_fpcr  %0;"
    : "=f" (t)
  );
#else
  t= dasm("mf_fpcr %f0;");
#endif

  return *(long *)&t;
}
#endif /* __NO_FLOATING_POINT */

/*
void ieee_set_state_at_signal(unsigned long fp_control, unsigned long fpcr)
{
  puts("ieee_set_state_at_signal() called");
}

int ieee_get_state_at_signal(unsigned long *fp_control, unsigned long *fpcr)
{
  puts("ieee_get_state_at_signal() called");
  return 0;
}
*/
void ieee_ignore_state_at_signal(void)
{
  puts("ieee_ignore_state_at_signal() called");
}
