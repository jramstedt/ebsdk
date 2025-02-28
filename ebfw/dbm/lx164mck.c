
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
__attribute__((unused)) static const char *rcsid = "$Id: lx164mck.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * $Log: lx164mck.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1997/03/12  15:54:36  fdh
 * Modified to use CIA definitions provided in the EB164 PALcode.
 * One less PALcode modification.
 *
 * Revision 1.1  1997/02/21  03:19:41  bissen
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
  {"SC_ADDR",		LAF_Q_SC_ADDR},
  {"SC_STAT",		LAF_Q_SC_STAT},
  {"BC_TAG_ADDR",	LAF_Q_BC_TAG_ADDR},
  {"EI_ADDR",		LAF_Q_EI_ADDR},
  {"FILL_SYNDROME",	LAF_Q_FILL_SYNDROME},
  {"EI_STAT",		LAF_Q_EI_STAT},
  {"MCHK_CODE",		LAF_Q_MCHK_CODE},
  {"LD_LOCK",		LAF_Q_LD_LOCK},
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

  i = 0;
  while (LafSysData[i].Name != NULL) {
    printf(" %15s: %08X",
	   LafSysData[i].Name, *(ul *) ((long) Frame + LafSysData[i].Offset));
    if (((i+1)%3) == 0) printf("\n");
    ++i;
  }

#define HAE_MEM 0x400
printf("\tHAE_MEM: %08x\n", in_ioc_csr(HAE_MEM));

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
	 "EI_STAT", *(ul *) ((long) Frame + LAS_Q_EI_STAT),
	 "EI_ADDR", *(ul *) ((long) Frame + LAS_Q_EI_ADDR));

  printf("%15s: %016lX %15s: %016lX\n",
	 "FILL_SYNDROME", *(ul *) ((long) Frame + LAS_Q_FILL_SYNDROME),
	 "ISR", *(ul *) ((long) Frame + LAS_Q_ISR));
}


void ParseLogout(LogoutFrame_t *Frame)
{
  ul value;

  value = *(ul *) ((long) Frame + LAF_Q_ICPERR);

  if (value & (1ul<<ICPERR_V_DPE))
    printf("ICache Data Parity Error\n");

  if (value & (1ul<<ICPERR_V_TPE))
    printf("ICache Tag Parity Error\n");

  if (value & (1ul<<ICPERR_V_TMR))
    printf("ICache Timeout Error\n");


  value  = *(ul *) ((long) Frame + LAF_Q_DCPERR);

  if (value & (1ul<<DCPERR_V_LOCK)) {
    if (value & ((1ul<<DCPERR_V_DP0)|(1ul<<DCPERR_V_DP1)))
      printf("DCache Data Parity Error\n");

    if (value & ((1ul<<DCPERR_V_TP0)|(1ul<<DCPERR_V_TP1)))
	printf("DCache Tag Parity Error\n");
  }


  value = *(ul *) ((long) Frame + LAF_Q_SC_STAT);

  if (value & SC_M_TPERR)
    printf("SCache Tag Parity Error\n");

  if (value & SC_M_DPERR)
    printf("SCache Data Parity Error\n");

  if (value & SC_M_CMD)
    printf("SCache Command Parity Error\n");


  value = *(ul *) ((long) Frame + LAF_Q_EI_STAT);

  if (value & (1ul<<EI_V_SEO_HRD_ERR))
    printf("Second EI hard error\n");

  if (value & (1ul<<EI_V_EI_PAR_ERR))
    printf("EI command/address parity error\n");

  if (value & (1ul<<EI_V_UNC_ECC_ERR))
    printf("Uncorrectable ECC error\n");

  if (value & (1ul<<EI_V_BC_TC_PERR))
    printf("BCache tag control parity error\n");

  if (value & (1ul<<EI_V_BC_TPERR))
    printf("BCache Tag parity error\n");


  value = *(ul *) ((long) Frame + LAF_Q_CIA_ERR);

  if (value & (1ul<<pyxis_err_v_err_valid)) {
    if (value & (1ul<<pyxis_err_v_un_corr_err))
      printf("Uncorrectable ECC Error\n");

    if (value & (1ul<<pyxis_err_v_cpu_pe))
      printf("SysBus Parity Error\n");

    if (value & (1ul<<pyxis_err_v_mem_nem))
      printf("NonExistent Memory Error\n");

    if (value & (1ul<<pyxis_err_v_pci_serr))
      printf("PCI bus system error\n");

    if (value & (1ul<<pyxis_err_v_perr))
      printf("PCI bus parity error\n");

    if (value & (1ul<<pyxis_err_v_pci_addr_pe))
      printf("PCI address parity error\n");

    if (value & (1ul<<pyxis_err_v_rcvd_mas_abt))
      printf("PCI Master Abort\n");

    if (value & (1ul<<pyxis_err_v_rcvd_tar_abt))
      printf("PCI Target Abort\n");

    if (value & (1ul<<pyxis_err_v_pa_pte_inv))
      printf("Invalid scatter-gather PTE\n");

    if (value & (1ul<<pyxis_err_v_ioa_timeout))
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
	      cServe(0, 0, CSERVE_K_RD_BCCTL) & ~0x8010ul,
	      cServe(0, 0, CSERVE_K_RD_BCCFG),
	      0);
    break;

  case StateOff:
    wr_bcache(3,
	      cServe(0, 0, CSERVE_K_RD_BCCTL) | 0x8010ul,
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
    WriteL(0x8740008280, 0xffb); mb();
    WriteL(0x8740000100, ReadL(0x8740000100) | 0x1c00u); mb();
    break;

  case StateOff:
    WriteL(0x8740008280, 0x000); mb();
    WriteL(0x8740000100, ReadL(0x8740000100) & ~0x1c00u); mb();
    break;

  case StateCPU:
  default:
    return;
  }
}
