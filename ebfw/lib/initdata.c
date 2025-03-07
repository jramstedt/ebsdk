
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
__attribute__((unused)) static const char *rcsid = "$Id: initdata.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: initdata.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.26  1998/08/26  18:35:01  thulin
 * Add support for CLUBS
 *
 * Revision 1.25  1998/06/26  18:45:58  gries
 * new check for valid signature
 *
 * Revision 1.24  1997/12/15  20:44:02  pbell
 * Merged dp264 changes.
 *
 * Revision 1.23  1997/05/01  20:46:50  pbell
 * Added 21164PC specific changes for SX.
 *
 * Revision 1.22  1997/02/21  03:39:44  fdh
 * Added ExitData definition for exit()'s call to longjmp().
 *
 * Revision 1.21  1996/05/22  15:58:04  fdh
 * Changed UserPutChar, UserGetChar, and UserCharAv to be initialized data.
 *
 * Revision 1.20  1996/01/15  20:50:34  cruz
 * Updated for filename changed.
 *
 * Revision 1.19  1995/11/29  19:39:05  cruz
 * Initialized fields in sysdata for DC21164 and added print
 * statements for them in DumpSysData().
 *
 * Revision 1.18  1995/10/31  16:44:25  cruz
 * Moved variable prtrace_enable to prtrace.c
 *
 * Revision 1.17  1995/10/30  16:55:36  cruz
 * Casted functions to (void *) rather than (ul) when filling
 * in the callback table.
 *
 * Revision 1.16  1995/10/26  23:45:56  cruz
 * Added casting to dispatch table.
 *
 * Revision 1.15  1995/10/20  14:00:44  fdh
 * Added DumpSysData().
 *
 * Revision 1.14  1995/09/12  21:34:26  fdh
 * Modified UserGetChar to return an int instead of a char.
 *
 * Revision 1.13  1995/09/02  06:28:07  fdh
 * Removed WaitUs() from callbacks.
 *
 * Revision 1.12  1995/08/25  19:47:20  fdh
 * Implemented Debug Monitor callbacks.  They get
 * setup through initdata().
 *
 * Revision 1.11  1994/11/24  04:11:19  fdh
 * Re-claim revision 1.9 changes.
 *
 * Revision 1.10  1994/11/23  19:59:57  cruz
 * Modified to support eb164.
 *
 * Revision 1.9  1994/11/22  21:58:26  fdh
 * Don't include PALcode include files for make depend.
 *
 * Revision 1.8  1994/11/07  12:05:39  rusling
 * Now include system.h as well as lib.h
 *
 * Revision 1.7  1994/08/08  00:28:55  fdh
 * Fixed bug in zeroing uninitialized data.
 * Added prtrace_enable that is used to control
 * debug prints when TRACE_ENABLE is defined.
 *
 * Revision 1.6  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.5  1994/08/03  19:42:30  fdh
 * Fixups around the linker defined symbols _edata and _end.
 *
 * Revision 1.4  1994/07/13  14:19:39  fdh
 * Added initialization for data structure used tohold pointers
 * to SROM interface parameters.  PalImpureBase is also initialized
 * here.
 *
 * Revision 1.3  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 */

#include "system.h"
#include "lib.h"
#include "callback.h"
#include "ladbxusr.h"
#include "paldata.h"
#include "palcsrv.h"
#include "setjmp.h"

jmp_buf ExitData;

typedef enum CALLBACKS {
  CBIDPutChar,
  CBIDGetChar,
  CBIDCharAv,
  CBIDtime,
  CBIDmalloc_summary,
  CBIDmalloc,
  CBIDfree,
  CBIDrealloc,
  CBIDladbx_poll
} CallBack_t;

void **CBarray;

void (*UserPutChar)(char c) = PutChar;
int (*UserGetChar)(void) = GetChar;
int  (*UserCharAv)(void) = CharAv;
time_t (*Usertime)(time_t *out);
void (*Usermalloc_summary)(char *string);
void *(*Usermalloc)(size_t size);
void (*Userfree)(void *pointer);
void *(*Userrealloc)(void *pointer, size_t size);
void (*Userladbx_poll)(void);


/*
 *  This structure contains system information 
 *  passed up by the SROM.
 */
sysdata_t sysdata;

ul PalImpureBase;
ul CPU_cycles_per_usecs = 150; /* Default cycle count for timers. */

void initdata(int process)
{
  
#ifndef _WIN32
/*
 * Currently on Windows NT neither edata or _edata
 * is defined.  This is normally a linker defined symbol
 * which points to the first location after the initialized
 * data section.  The uninitialized data section should be
 * zeroed.
 */

#ifndef ISP_SIMULATION
    bzero((char *)&_EDATA, (uintptr_t)&_END - (uintptr_t)&_EDATA);
#endif
#endif
  
#ifdef TRACE_ENABLE
/*
 * Disable tracing.  It can be enabled
 * later with the prtrace command.
 */
  prtrace_enable = 0;
#endif

  PalImpureBase = (ul) (CNS_Q_BASE + cServe(0, 0, CSERVE_K_RD_IMPURE));

#ifdef DC21064
  sysdata.abox_ctl  = (ul *)(PalImpureBase + (ul)CNS_Q_ABOX_CTL);
  sysdata.biu_ctl   = (ul *)(PalImpureBase + (ul)CNS_Q_BIU_CTL);
#endif /* DC21064 */

#ifdef DC21066
  sysdata.abox_ctl  = (ul *)(PalImpureBase + (ul)CNS_Q_ABOX_CTL);
  sysdata.bcr0      = (ui *)(PalImpureBase + (ul)CNS_L_BCR0);
  sysdata.bcr1      = (ui *)(PalImpureBase + (ul)CNS_L_BCR1);
  sysdata.bcr2      = (ui *)(PalImpureBase + (ul)CNS_L_BCR2);
  sysdata.bcr3      = (ui *)(PalImpureBase + (ul)CNS_L_BCR3);
  sysdata.bmr0      = (ui *)(PalImpureBase + (ul)CNS_L_BMR0);
  sysdata.bmr1      = (ui *)(PalImpureBase + (ul)CNS_L_BMR1);
  sysdata.bmr2      = (ui *)(PalImpureBase + (ul)CNS_L_BMR2);
  sysdata.bmr3      = (ui *)(PalImpureBase + (ul)CNS_L_BMR3);
#endif /* DC21066 */

#ifdef DC21164
  sysdata.bc_ctl    = (ul *)(PalImpureBase + (ul)CNS_Q_BC_CTL);
  sysdata.bc_cfg    = (ul *)(PalImpureBase + (ul)CNS_Q_BC_CFG);
  sysdata.bc_cfg_off= (ul *)(PalImpureBase + (ul)CNS_Q_BC_CFG_OFF);
#endif /* DC21164 */

#ifdef DC21164PC
  sysdata.bc_cfg2    = (ul *)(PalImpureBase + (ul)CNS_Q_BC_CFG2);
  sysdata.bc_cfg    = (ul *)(PalImpureBase + (ul)CNS_Q_BC_CFG);
#endif /* DC21164 */

#ifdef DC21264
  sysdata.dc_ctl    = (ul *)(PalImpureBase + (ul)CNS__DC_CTL);
#endif /* DC21264 */

  sysdata.srom_rev  = (ul *)(PalImpureBase + (ul)CNS_Q_SROM_REV);
  sysdata.proc_id   = (ul *)(PalImpureBase + (ul)CNS_Q_PROC_ID);
  sysdata.mem_size  = (ul *)(PalImpureBase + (ul)CNS_Q_MEM_SIZE);
  sysdata.cycle_cnt = (ul *)(PalImpureBase + (ul)CNS_Q_CYCLE_CNT);
  sysdata.signature = (ul *)(PalImpureBase + (ul)CNS_Q_SIGNATURE);
  sysdata.proc_mask = (ul *)(PalImpureBase + (ul)CNS_Q_PROC_MASK);
  sysdata.sysctx    = (ul *)(PalImpureBase + (ul)CNS_Q_SYSCTX);
  sysdata.valid     = (((*sysdata.signature >> 20) & (ul)0xFFF) == (ul)0xDEC);

#if defined(DP264) || defined(CLUBS)
#ifdef ISP_SIMULATION
    *sysdata.srom_rev  = 0xaa00000000000504;
    *sysdata.proc_id   = 0;
    *sysdata.mem_size  = MINIMUM_SYSTEM_MEMORY;
    *sysdata.cycle_cnt = 1*1000*1000*1000;
    *sysdata.signature = 0xdecb0001;
    *sysdata.proc_mask = 0;
    *sysdata.sysctx    = 0;
    sysdata.valid     = (((*sysdata.signature >> 16) & (ul)0xFFFF) == (ul)0xDECB);
#else
#if 0
    *sysdata.srom_rev  = 0xaa00000000000503;
    *sysdata.proc_id   = 0;
    *sysdata.mem_size  = MINIMUM_SYSTEM_MEMORY;
    *sysdata.cycle_cnt = (1000*1000)/(400);
    *sysdata.signature = 0xdecb0001;
    *sysdata.proc_mask = 0;
    *sysdata.sysctx    = 0;
    sysdata.valid     = (((*sysdata.signature >> 16) & (ul)0xFFFF) == (ul)0xDECB);
#endif
#endif /* ISP_SIMULATION */
#endif /* DP264 || CLUBS*/

  if (sysdata.valid)
    CPU_cycles_per_usecs = 1000000/(*sysdata.cycle_cnt);

  /* Initialize Callback table */
  CBarray = (void *)(DBMENTRY-2*sizeof(CBarray));

  if (process) {
    /* User Mode */
    UserPutChar =  (void (*)(char c))  *(CBarray-CBIDPutChar);
    UserGetChar =  (int (*)(void))  *(CBarray-CBIDGetChar);
    UserCharAv  =  (int  (*)(void)) *(CBarray-CBIDCharAv);
    Usertime    =  (time_t (*)(time_t *out)) *(CBarray-CBIDtime);
    Usermalloc_summary = (void (*)(char *string)) *(CBarray-CBIDmalloc_summary);
    Usermalloc = (void *(*)(size_t size)) *(CBarray-CBIDmalloc);
    Userfree =  (void (*)(void *pointer)) *(CBarray-CBIDfree);
    Userrealloc = (void *(*)(void *pointer, size_t size)) *(CBarray-CBIDrealloc);
    Userladbx_poll = (void (*)(void)) *(CBarray-CBIDladbx_poll);
  }
  else {
    /* Debug Monitor Mode */
    *(CBarray-CBIDPutChar) = (void *) PutChar;
    *(CBarray-CBIDGetChar) =  (void *) GetChar;
    *(CBarray-CBIDCharAv) =   (void *) CharAv;
    *(CBarray-CBIDtime) =   (void *) time;
    *(CBarray-CBIDmalloc_summary) =   (void *) malloc_summary;
    *(CBarray-CBIDmalloc) =  (void *)malloc;
    *(CBarray-CBIDfree) =   (void *)free;
    *(CBarray-CBIDrealloc) =  (void *)realloc;
    *(CBarray-CBIDladbx_poll) =  (void *)ladbx_poll;
  }
}

void DumpSysData(void)
{
#ifdef DC21064
	printf("abox_ctl : %16lX\n", *sysdata.abox_ctl );
	printf("biu_ctl  : %16lX\n", *sysdata.biu_ctl );
#endif /* DC21064 */

#ifdef DC21066
	printf("abox_ctl : %16lX\n", *sysdata.abox_ctl );
	printf("bcr0     : %16X\t", *sysdata.bcr0     );	printf("bcr1     : %16X\n", *sysdata.bcr1     );
	printf("bcr2     : %16X\t", *sysdata.bcr2     );	printf("bcr3     : %16X\n", *sysdata.bcr3     );
	printf("bmr0     : %16X\t", *sysdata.bmr0     );	printf("bmr1     : %16X\n", *sysdata.bmr1     );
	printf("bmr2     : %16X\t", *sysdata.bmr2     );	printf("bmr3     : %16X\n", *sysdata.bmr3     );
#endif /* DC21066 */

#ifdef DC21164
	printf("bc_ctl   : %16lX\n", *sysdata.bc_ctl );
	printf("bc_cfg   : %16lX\t", *sysdata.bc_cfg );         printf("bc_cfgoff: %16lX\n", *sysdata.bc_cfg_off );
#endif /* DC21164 */

#ifdef DC21164PC
	printf("bc_cfg   : %16lX\t", *sysdata.bc_cfg );         printf("bc_cfg2: %16lX\n", *sysdata.bc_cfg2 );
#endif /* DC21164 */

#ifdef DC21264
	printf("dc_ctl   : %16lX\n", *sysdata.dc_ctl );
#endif /* DC21264 */

	printf("srom_rev : %16lX\t", *sysdata.srom_rev );	printf("proc_id  : %16lX\n", *sysdata.proc_id  );
	printf("mem_size : %16lX\t", *sysdata.mem_size );	printf("cycle_cnt: %16lX\n", *sysdata.cycle_cnt);
	printf("signature: %16lX\t", *sysdata.signature);	printf("proc_mask: %16lX\n", *sysdata.proc_mask);
	printf("sysctx   : %16lX\t", *sysdata.sysctx   );	printf("valid    : %16lX\n", sysdata.valid    );
	printf("\n");
}
