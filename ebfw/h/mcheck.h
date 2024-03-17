#ifndef __MCHECK_H_LOADED
#define __MCHECK_H_LOADED
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
 *  $Id: mcheck.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 *
 *  $Log: mcheck.h,v $
 *  Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 *  Initial CVS checkin
 *
 * Revision 1.3  1998/08/03  17:32:58  gries
 * mor better machine check
 *
 * Revision 1.2  1997/06/02  04:25:59  fdh
 * Moved more of the machine check declarations
 * into this file.
 *
 * Revision 1.1  1996/05/22  21:29:08  fdh
 * Initial revision
 *
 */

typedef struct {
  ui FrameSize;			/* Bytes, including this field */
  ui FrameFlags;		/* <31> = Retry, <30> = Second Error */
  ui CpuOffset;			/* Offset to CPU-specific into */
  ui SystemOffset;		/* Offset to system-specific info */
  ui MCHK_Code;
  ui MCHK_Frame_Rev;
  ul I_STAT;
  ul DC_STAT;
  ul C_ADDR;
  ul DC1_SYNDROME;
  ul DC0_SYNDROME;
  ul C_STAT;
  ul C_STS;
  ul RESERVED0;
  ul EXC_ADDR;
  ul IER_CM;
  ul ISUM;
  ul MM_STAT;
  ul PAL_BASE;
  ul I_CTL;
  ul PCTX;
} LogoutFrame_t;

extern int MachineCheckExpected;
extern int ExpectedMachineCheckTaken;

/*::::::::::::::
mcheck.c
::::::::::::::*/
extern void UnexpectedInterruptHandler(unsigned int arg0, ui arg1, ui *arg2);
extern void SetMcheck(State_t state);
extern State_t ConfigStateID(char *arg);

/*::::::::::::::
xxxxmck.c
::::::::::::::*/
extern void DumpLogout(LogoutFrame_t *LAF);
extern void DumpShortLogout(LogoutFrame_t *LAF);
extern void ParseLogout(LogoutFrame_t *Frame);
extern void SetMcheckSystem(State_t state);

#endif /* __MCHECK_H_LOADED */
