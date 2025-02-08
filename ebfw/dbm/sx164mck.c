
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
__attribute__((unused)) static const char *rcsid = "$Id: sx164mck.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * $Log: sx164mck.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1997/05/23  12:48:56  fdh
 * Merged dc21164p.h into dc21164.h.
 *
 * Revision 1.3  1997/05/22  18:35:18  fdh
 * Modified to use CIA definitions provided in the EB164 PALcode.
 * One less PALcode modification.
 *
 * Revision 1.2  1997/05/19  17:20:04  fdh
 * Fixed up machine check output.
 *
 * Revision 1.1  1997/05/02  19:39:56  pbell
 * Initial revision
 *
 * Revision 1.3  1996/06/24  14:54:50  fdh
 * Modified to print 32 bit PYXIS registers as longwords.
 *
 * Revision 1.2  1996/06/13  03:37:29  fdh
 * Colapsed logout frame dump to conserve space.
 *
 * Revision 1.1  1996/05/22  22:08:28  fdh
 * Initial revision
 *
 */

#include "lib.h"
#include "mon.h"
#include "pyxis.h"
#include "mcheck.h"

#ifndef MAKEDEPEND
#include "impure.h"		/* For logout area */
#include "cserve.h"
#include "dc21164.h"
#endif

#define MEM_MCR(x)        (((ul)MC_MCR_BASE<<MC_MCR_BASE_SHIFT)|(ul)(x))
#define _IN_MEM_MCR(p)    (ReadL(MEM_MCR(p)))
#define _OUT_MEM_MCR(p,d) WriteL((MEM_MCR(p)),d);mb();

#define MEM_ERROR(x)        (((ul)MC_ERROR_BASE<<MC_ERROR_BASE_SHIFT)|(ul)(x))
#define _IN_MEM_ERROR(p)    (ReadL(MEM_ERROR(p)))
#define _OUT_MEM_ERROR(p,d) WriteL((MEM_ERROR(p)),d);mb();

typedef struct {
  char *Name;
  int  Offset;
} LogoutFrameData_t;

LogoutFrameData_t LafCpuData[] = {
  {"EXC_ADDR",		LAF_Q_EXC_ADDR},
  {"EXC_SUM",		LAF_Q_EXC_SUM},
  {"EXC_MASK",		LAF_Q_EXC_MASK},
  {"PAL_BASE",		LAF_Q_PAL_BASE},
  {"ISR",			LAF_Q_ISR},
  {"ICSR",			LAF_Q_ICSR},
  {"ICPERR",		LAF_Q_ICPERR},
  {"DCPERR",		LAF_Q_DCPERR},
  {"VA",			LAF_Q_VA},
  {"MM_STAT",		LAF_Q_MM_STAT},
  {"BC_ADDR",		LAF_Q_BC_ADDR},
  {"BC_STAT",		LAF_Q_BC_STAT},
  {"MCHK_CODE",		LAF_Q_MCHK_CODE},
  {  NULL, 0}
};

LogoutFrameData_t LafSysData[] = {
  {"CPU_ERR0",		LAF_Q_CPU_ERR0},
  {"CPU_ERR1",		LAF_Q_CPU_ERR1},
  {"PYXIS_ERR",		LAF_Q_CIA_ERR},
  {"PYXIS_STAT",	LAF_Q_CIA_STAT},
  {"ERR_MASK",		LAF_Q_ERR_MASK},
  {"PYXIS_SYN",		LAF_Q_CIA_SYN},
  {"MEM_ERR0",		LAF_Q_MEM_ERR0},
  {"MEM_ERR1",		LAF_Q_MEM_ERR1},
  {"PCI_ERR0",		LAF_Q_PCI_ERR0},
  {"PCI_ERR1",		LAF_Q_PCI_ERR1},
  {"PCI_ERR2",		LAF_Q_PCI_ERR2},
  {  NULL, 0}
};

void DumpLogout(LogoutFrame_t *Frame)
{
  int i;
  Beep(100,1000);

#if 0
  printf("Logout area address = 0x%x, size = 0x%x\n",
	 Frame, Frame->FrameSize);
#endif

  i = 0;
  while (LafCpuData[i].Name != NULL) {
    printf(" %15s: %016lX",
	   LafCpuData[i].Name, *(ul *) ((long) Frame + LafCpuData[i].Offset));
    if (((i+1)%2) == 0) printf("\n");
    ++i;
  }
  printf("\n");
  i = 0;
  while (LafSysData[i].Name != NULL) {
    printf(" %15s: %08X",
	   LafSysData[i].Name, *(ul *) ((long) Frame + LafSysData[i].Offset));
    if (((i+1)%3) == 0) printf("\n");
    ++i;
  }

#if 0
  PrintMem(2, (ul) Frame,
	   (ul) (Frame+(Frame->FrameSize/4)), 0 ,0);
#endif

  printf("\n");

  /* Clear the PYXIS Error register */
  _OUT_MEM_ERROR(MC_PYXIS_ERR, _IN_MEM_ERROR(MC_PYXIS_ERR))
}

void DumpShortLogout(LogoutFrame_t *Frame)
{
  printf("%15s: %016lX %15s: %016lX\n",
	 "BC_STAT", *(ul *) ((long) Frame + LAS_Q_BC_STAT),
	 "BC_ADDR", *(ul *) ((long) Frame + LAS_Q_BC_ADDR));

  printf("%15s: %016lX\n",
	 "ISR", *(ul *) ((long) Frame + LAS_Q_ISR));
}


void ParseLogout(LogoutFrame_t *Frame)
{
  ul value;

  value = *(ul *) ((long) Frame + LAF_Q_ICPERR);

  if (value & (1<<ICPERR_V_DPE))
    printf("ICache Data Parity Error\n");

  if (value & (1<<ICPERR_V_TPE))
    printf("ICache Tag Parity Error\n");

  if (value & (1<<ICPERR_V_TMR))
    printf("ICache Timeout Error\n");


  value  = *(ul *) ((long) Frame + LAF_Q_DCPERR);

  if (value & (1<<DCPERR_V_LOCK)) {
    if (value & ((1<<DCPERR_V_DP0)|(1<<DCPERR_V_DP1)))
      printf("DCache Data Parity Error\n");

    if (value & ((1<<DCPERR_V_TP0)|(1<<DCPERR_V_TP1)))
	printf("DCache Tag Parity Error\n");
  }


  value = *(ul *) ((long) Frame + LAF_Q_BC_STAT);

  if (value & BCSTAT_M_DATA_PAR_ERR)
    printf("Data Parity error\n");

  if (value & BCSTAT_M_TAG_PAR_ERR)
    printf("TAG parity error\n");

  if (value & BCSTAT_M_MEMORY)
    printf("System Memory error\n");

  if (value & BCSTAT_M_MULTI_ERR)
    printf("Multiple errors detected\n");


  value = *(ul *) ((long) Frame + LAF_Q_CIA_ERR);

  if (value & (1<<pyxis_err_v_err_valid)) {
    if (value & (1<<pyxis_err_v_un_corr_err))
      printf("Uncorrectable ECC Error\n");

    if (value & (1<<pyxis_err_v_cpu_pe))
      printf("SysBus Parity Error\n");

    if (value & (1<<pyxis_err_v_mem_nem))
      printf("NonExistent Memory Error\n");

    if (value & (1<<pyxis_err_v_pci_serr))
      printf("PCI bus system error\n");

    if (value & (1<<pyxis_err_v_perr))
      printf("PCI bus parity error\n");

    if (value & (1<<pyxis_err_v_pci_addr_pe))
      printf("PCI address parity error\n");

    if (value & (1<<pyxis_err_v_rcvd_mas_abt))
      printf("PCI Master Abort\n");

    if (value & (1<<pyxis_err_v_rcvd_tar_abt))
      printf("PCI Target Abort\n");

    if (value & (1<<pyxis_err_v_pa_pte_inv))
      printf("Invalid scatter-gather PTE\n");

    if (value & (1<<pyxis_err_v_ioa_timeout))
      printf("I/O Timeout\n");
  }
  printf("\n");
}

void SetMcheckSystem(State_t state)
{

/* Set the BC_CONTROL register */
  switch(state) {
  case StateOn:
  case StateCPU:
    wr_bcache(3,
	      cServe(0, 0, CSERVE_K_RD_BCCFG2),
	      cServe(0, 0, CSERVE_K_RD_BCCFG),
	      0);
    break;

  case StateOff:
    wr_bcache(3,
	      cServe(0, 0, CSERVE_K_RD_BCCFG2),
	      cServe(0, 0, CSERVE_K_RD_BCCFG),
	      0);
    break;

  case StateSystem:
  default:
    return;
  }

/* Set the PYXIS_CTRL register */
  switch(state) {
  case StateOn:
  case StateSystem:
    WriteL(0x8740008280, 0xBF8); mb();
    WriteL(0x8740000100, ReadL(0x8740000100) | 0xc00); mb();
    break;

  case StateOff:
    WriteL(0x8740008280, 0x000); mb();
    WriteL(0x8740000100, ReadL(0x8740000100) & ~0xc00); mb();
    break;

  case StateCPU:
  default:
    return;
  }
}
