
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
__attribute__((unused)) static const char *rcsid = "$Id: pReg.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * $Log: pReg.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.12  1997/02/21  03:10:43  fdh
 * The file is now obsolete.
 *
 * Revision 1.11  1995/12/01  20:22:03  cruz
 * Fixed printing of PC and PS for DC21164 in printReg().
 * This is a temporary fix and should really be corrected in
 * ladebug/kutils.s
 *
 * Revision 1.10  1995/11/30  22:49:17  cruz
 * Updated ChangeMem to use constants for determining location of
 * the register areas.
 *
 * Revision 1.9  1995/11/06  22:11:26  cruz
 * Changed the format string for printing 64-bit quantities.
 *
 * Revision 1.8  1995/02/08  01:31:01  fdh
 * Don't include PALcode files for makedepend.
 *
 * Revision 1.7  1995/01/17  21:35:31  cruz
 * Changed PrintReg command to use constants instead of
 * hardwired offsets from base.
 *
 * Revision 1.6  1994/08/05  20:18:01  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.5  1994/06/23  13:43:51  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.4  1994/06/17  19:36:54  fdh
 * Clean-up...
 *
 * Revision 1.3  1994/03/28  14:55:04  fdh
 * Corrected pointer to register save state area.
 *
 * Revision 1.2  1994/01/19  10:50:12  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.1  1993/06/08  19:56:41  fdh
 * Initial revision
 *
 */


#include "system.h"
#include "mon.h"
#include "lib.h"
#include "paldata.h"

#define GPREGGROUP  0
#define FPREGGROUP  1
#define PTPREGGROUP 2

/*
This routine reads the saved registers 
*/
void printReg(ul * register_array, int group)
{
  int i,j,x=0;
  char *str;
  double *double_array = (double *) register_array;

/* CNS_Q_xxx are offsets from CNS_Q_BASE expressed in bytes. */
  switch(group) {
  case PTPREGGROUP:
    printf("\nPAL Temp Registers\n");
    x = CNS_Q_PT / 8;
    str = "pt";
    break;
  case FPREGGROUP:
    printf("\nFloating Point Registers\n");
    x = CNS_Q_FPR / 8;
    str = "f";
    break;
  case GPREGGROUP:
    printf("\nGeneral Purpose Registers\n");
    x = CNS_Q_GPR / 8;
    str = "r";
    break;
  default:
    printf("\nUnknown Register Group\n");
    return;
  }
  printf("register file @: %08x\n", register_array+x);

  for (i=0; i<32; i+=4) {
    printf("%s%02d: ", str, i);
    for (j=0; j<4; j++) {
      if (group == FPREGGROUP)
	printf("%10.5g ", double_array[x++]);
      else
	printf("%016lx ", register_array[x++]);
    }
    printf("\n");
  }

  printf("PC:  %016lx   ",  *SavedExceptionAddressRegister);
  printf("PS:  %016lx\n\n", SavedProcessorStatusRegister
);
}

void changeReg(ul * register_array, int group, int index, ul data)
{
  switch(group) {
  case PTPREGGROUP:
    index += (CNS_Q_PT / 8);
    break;
  case FPREGGROUP:
    index += (CNS_Q_FPR / 8);
    break;
  case GPREGGROUP:
    index += (CNS_Q_GPR / 8);
    break;
  }
  register_array[index] = data;
}
