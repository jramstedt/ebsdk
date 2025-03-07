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

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: mcheck.c,v 1.2 1999/01/21 19:05:07 gries Exp $";
#endif

/* 
 * $Log: mcheck.c,v $
 * Revision 1.2  1999/01/21 19:05:07  gries
 * First Release to cvs gries
 *
 * Revision 1.5  1999/01/19  16:54:16  gries
 * more better mcheck
 *
 * Revision 1.4  1997/08/17  11:21:52  fdh
 * Included palosf.h instead of osf.h.
 *
 * Revision 1.3  1997/06/02  13:06:17  fdh
 * Added the NEEDPCI condition around call to IOPCIClearNODEV();
 *
 * Revision 1.2  1997/06/02  04:33:05  fdh
 * Modified to handle "expected machine checks" such
 * as those that might occur during the PCI bus configuration.
 *
 * Revision 1.1  1996/05/22  20:25:16  fdh
 * Initial revision
 *
 */

#include "lib.h"
#include "mcheck.h"
#include "ladebug.h"
#include "palosf.h"

int corrected_error_count;
int MachineCheckExpected;
int ExpectedMachineCheckTaken;

void UnexpectedInterruptHandler(unsigned int arg0, ui arg1, ui *arg2)
{
  ui save_hae,ipl_state;
  save_hae = SetHAE(0);		/* Put HAE back into a known state. */

  /* In all other cases, except correctable errors, print out
     a message then stop as if a breakpoint is encountered */
  switch(arg0) {
  case INT_K_IP:		/* Interprocessor interrupt */
    printf("\nInterprocessor interrupt \n");
    break;
  case INT_K_MCHK:	/* Machine check */
    if (MachineCheckExpected) {
/*
 *  When noDebugger is asserted this code runs in the scope of the
 *  debugger.  Therefore this point is not reachable because
 *  MachineCheckExpected is asserted in the debuggee but is not
 *  visible by the execption handler in the debugger.
 */
      ExpectedMachineCheckTaken = TRUE;
      MachineCheckExpected = FALSE;

      PRTRACE1("UnexpectedInterruptHandler() Expected Machine Check taken.\n");

      /* Clear the machine check before returning */
#ifdef NEEDPCI
      IOPCIClearNODEV();
#endif
      wrmces(MCES_M_SCE|MCES_M_PCE|MCES_M_MIP);
      SetHAE(save_hae);		/* Restore HAE value. */
      return;
    }

    if (corrected_error_count<10)
      printf("\n%s: Interrupt vector = 0x%x\n",
	   SCB_vector_string(arg1), arg1);

    switch(arg1) {
    case SCB_Q_SYSERR:		/* System Correctable Machine Check */
    case SCB_Q_PROCERR:		/* Processor Correctable Machine Check */
      DumpShortLogout((LogoutFrame_t *) arg2); /* Dump the Logout Frame */
      ++corrected_error_count;
      wrmces(MCES_M_SCE|MCES_M_PCE);
      SetHAE(save_hae);		/* Restore HAE value. */
      ipl_state = swpipl(6);
      if (ipl_state < 6)
        swpipl(ipl_state);
      return;

    case SCB_Q_SYSMCHK:		/* System Machine Check */
    case SCB_Q_PROCMCHK:	/* Processor Machine Check */
    default:
//      ParseLogout((LogoutFrame_t *) arg2); /* Parse the Logout Frame */
      DumpLogout((LogoutFrame_t *) arg2); /* Dump the Logout Frame */

	/* Clear the machine check before returning */
#ifdef Debugging_mchk
	wrmces(MCES_M_MIP|MCES_M_SCE|MCES_M_PCE);
      SetHAE(save_hae);		/* Restore HAE value. */
      ipl_state = swpipl(6);
      if (ipl_state < 6)
        swpipl(ipl_state);
      return;
#else
	wrmces(MCES_M_MIP);
#endif

    }
    break;
  case INT_K_DEV:		/* I/O device interrupt */
    printf("\nI/O device interrupt; Interrupt vector = %x\n",arg1);
    break;
  case INT_K_PERF:		/* Performance counter */
    printf("\nPerformance counter interrupt; Interrupt vector = %x\n", arg1);
    break;
  default:
    printf("\nUnknown interrupt type; a0 = %x, a1 = %x, a2 = %x\n", arg0, arg1, arg2);
  }

  /* If I stop now the saved registers etc. will be wrong; so put a breakpoint on the next
     instruction (as if I had received a stop command). */
  stopped = FALSE;
  kstop();

  SetHAE(save_hae);		/* Restore HAE value. */
}

void SetMcheck(State_t state)
{
  int ipl_state;

/* Set Machine Error Summary Register State */
  switch(state) {
  case StateOn:
    printf("Enabling Machine Checks %x %x\n",rdmces(), rdmces() & ~(MCES_M_DPC|MCES_M_DSC));
    wrmces(rdmces() & ~(MCES_M_DPC|MCES_M_DSC));
    break;

  case StateSystem:
    printf("Enabling System Machine Checks\n");
    wrmces(rdmces() & ~MCES_M_DSC);
    break;

  case StateCPU:
    printf("Enabling CPU Machine Checks\n");
    wrmces(rdmces() & ~MCES_M_DPC);
    break;

  case StateOff:
    printf("Disabling Machine Checks\n");
    wrmces(rdmces() | (MCES_M_DPC|MCES_M_DSC));
    break;

  default:
    printf("Unknown Machine Check State\n");
    break;
  }

/* Lower IPL if necessary */
  switch(state) {
  case StateOn:
  case StateSystem:
  case StateCPU:
    ipl_state = swpipl(6);
    if (ipl_state < 6)
      swpipl(ipl_state);
    break;

  case StateOff:
    /* Needn't bother raising IPL. MCES masking will be sufficient. */
    break;

  default:
    break;
  }

  SetMcheckSystem(state);
}

char *SCB_vector_string(int SCB_vector)
{
  char *vector_string;
  switch(SCB_vector) {
  case SCB_Q_SYSERR:
    vector_string = "System Correctable Machine Check";
    break;
  case SCB_Q_PROCERR:
    vector_string = "Processor Correctable Machine Check";
    break;
  case SCB_Q_SYSMCHK:
    vector_string = "System Machine Check";
    break;
  case SCB_Q_PROCMCHK:
    vector_string = "Processor Machine Check";
    break;
  default:
    vector_string = "Unknown SCB vector";
  }
  return(vector_string);
}

State_t ConfigStateID(char *arg)
{
  int i;

  i = 0;
  while ((arg[i] = toupper((int)arg[i])) != '\0') ++i;

  if (strcmp(arg, "ON") == 0)
    return StateOn;

  if (strcmp(arg, "OFF") == 0)
    return StateOff;

  if (strcmp(arg, "SYSTEM") == 0)
    return StateSystem;

  if (strcmp(arg, "CPU") == 0)
    return StateCPU;

  return -1;
}
