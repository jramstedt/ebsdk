#define Debug_Dual
/*****************************************************************************

       Copyright 1993, 1994, 1995 Digital Equipment Corporation,
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
static char *rcsid = "$Id: main.c,v 1.3 1999/01/21 21:16:35 paradis Exp $";
#endif

/*
 * $Log: main.c,v $
 * Revision 1.3  1999/01/21 21:16:35  paradis
 * Added cixes for Visual C++
 * Added conditional compilation for DP264-specific stuff
 * Removed fpu.c from WIN32 builds
 *
 * Revision 1.2  1999/01/21 19:04:40  gries
 * First Release to cvs gries
 *
 * Revision 1.36  1999/01/19  16:43:24  gries
 * Start srm hack startup code.
 *
 * Revision 1.35  1998/12/17  19:23:01  gries
 * added detect for isa (goldrush vs webbrick)
 *
 * Revision 1.34  1998/10/08  13:32:15  gries
 * added #ifdef Debug_Dual around   *second_stack= (ul *)malloc(10000);
 *   *second_cpu_lock= 0;
 *
 * Revision 1.33  1998/09/22  13:57:13  gries
 * add write to goldrush lcd
 *
 * Revision 1.32  1998/06/26  18:22:23  gries
 * changes for pc264
 *
 * Revision 1.31  1998/04/06  18:14:06  gries
 * changes for large memory systems
 *
 * Revision 1.30  1997/12/15  20:52:50  pbell
 * Updated for dp264.
 *
 * Revision 1.30  1997/11/19  19:54:24  gries
 * added waitforever
 *
 * Revision 1.29  1997/08/13  14:20:39  fdh
 * Disable FEN when built with __EMULATE_FLOATING_POINT defined.
 *
 * Revision 1.28  1997/08/12  05:32:10  fdh
 * Added hooks to override the default bootadr setting.
 * Added the __NO_FLOATING_POINT conditional.
 *
 * Revision 1.27  1997/07/02  12:53:30  fdh
 * Changed the default bootadr (IMAGE_BASE) to 0x300000.
 *
 * Revision 1.26  1997/06/30  14:10:59  pbell
 * Moved the location of objects in the toy nvram.
 *
 * Revision 1.25  1997/06/02  04:30:20  fdh
 * Rearranged to establish interrupt and exception handlers
 * earlier in order to handle expected machine checks encountered
 * during the PCI bus configuration.
 *
 * Revision 1.24  1997/04/30  16:45:43  pbell
 * Changed mempool boundary varriables to ul type.
 *
 * Revision 1.23  1997/03/17  18:59:55  fdh
 * Print info about memory heap.
 *
 * Revision 1.22  1997/02/25  21:24:38  fdh
 * Replaced code which inits the debugger with a
 * call to kinitdebugger() defined elsewhere.
 *
 * Revision 1.21  1997/02/21  19:48:08  fdh
 * Removed extra newline from puts calls.
 *
 * Revision 1.20  1997/02/21  03:09:56  fdh
 * Added new pointers to data areas shared with PALcode.
 *
 * Modified the CommandExec loop.
 *
 * Revision 1.19  1996/08/16  17:18:31  fdh
 * Added LED codes for console inited and powerup init done.
 * Added beep code for powerup init done.
 *
 * Revision 1.18  1996/05/22  22:17:30  fdh
 * Flush typeahead characters before printing prompt.
 *
 * Revision 1.17  1995/12/15  19:32:47  cruz
 * Changed DECChip string to Alpha to reflect current names.
 *
 * Revision 1.16  1995/12/01  15:31:55  cruz
 * Fixed bug that caused saved_register_array to be miscomputed.
 *
 * Revision 1.15  1995/11/27  16:07:37  cruz
 * Added initialization of regmode and etherbuffers to main().
 *
 * Revision 1.14  1995/11/09  21:56:28  cruz
 * *** empty log message ***
 *
 * Revision 1.13  1995/10/31  18:33:05  fdh
 * Truncate cpu speed to 2 decimal places...
 *
 * Revision 1.12  1995/10/31  16:16:39  fdh
 * Disabled DumpSysData at startup.
 *
 * Revision 1.11  1995/10/26  22:11:10  cruz
 * Fixed function prototype, moved declaration of regmode to
 * dis.c and removed declaration of external variable sysdata
 * since it's now in lib.h
 *
 * Revision 1.10  1995/10/24  18:04:20  fdh
 * Moved a global declaration for remote debugging
 * to a header file.  Included console.h.
 *
 * Revision 1.9  1995/10/23  19:55:36  cruz
 * Moved stopped and child_state to kernel.c
 *
 * Revision 1.8  1995/10/23  15:20:37  fdh
 * Condensed startup messages a little bit.
 *
 * Revision 1.7  1995/10/22  05:03:42  fdh
 * Communicate with graphics and serial ports until the user
 * selects one by default.
 *
 * Revision 1.6  1995/10/20  18:49:52  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.5  1995/10/20  13:59:41  fdh
 * Dump out SROM parameters at startup.
 *
 * Revision 1.4  1995/10/10  14:23:46  fdh
 * Renamed ladbx.h to ladebug.h.
 *
 * Revision 1.3  1995/09/29  20:20:21  fdh
 * Flush any typeahead characters before printing prompt.
 *
 * Revision 1.2  1995/09/15  20:22:46  fdh
 * Print prompt before calling get_command() and
 * call Lookup() after get_command() exits.  Also
 * return argc from get_command().
 *
 * Revision 1.1  1995/09/05  18:29:37  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "lib.h"
#include "mon.h"
#include "console.h"
#include "bbram.h"
#include "ledcodes.h"
#include "beepcode.h"
#include "ether.h"
#include "ladebug.h"
#include "paldata.h"
#include "interlock.h"
#include "tsunami.h"

#ifndef DEFAULT_BOOTADR
#define	DEFAULT_BOOTADR	0x300000
#endif

/*
 * This register array is provided for saving the 32 general purpose registers
 * at different points of operation of the Debug Monitor.  This area is shared
 * by PALcode, and debugger breakpoints.
 */
ul *SavedIntegerRegisters;
ul *SavedFloatingPointRegisters;
ul *SavedPALTempRegisters;
ul *SavedExceptionAddressRegister;
ul SavedProcessorStatusRegister;

extern int isa_present;
int *second_cpu_lock,cpu_count=0;
int halt_code = -1;
ul InitialStackPointer = (ul) -1L;
ul mem_size = 0;

ul bootadr;
ul **second_stack;
//volatile ul secondary_cpu_function= 0;
volatile void (*secondary_cpu_function)(void);
extern char __start;
extern int storage_initialized;
extern ul storage_pool_start;
extern ul storage_pool_end;

char MessageString[256];

static char *Prompt;

/*
 * Read: To make the Debug Monitor debuggable under the
 * Debug Monitor's debugger and ladebug.  The noDebugger
 * symbol can be used to disable the some of the debugger
 * features in the Debug Monitor. This allows one to debug
 * a second copy of the Debug Monitor (Debuggee) with the
 * Debug Monitor (Debugger).  Simply assign a TRUE value to
 * the noDebugger symbol before stepping into main() and the
 * functions which affect the state of the (Debugger) will
 * be disabled.
 */
int noDebugger = 0;

int main(int argc, char *argv[])
{
  ui val;
  ul heapsize;

#ifdef __EMULATE_FLOATING_POINT
  wrfen(0, (ul) 0);
#endif /* __EMULATE_FLOATING_POINT */

  /* Data shared w/PALcode */
  SavedIntegerRegisters =		(ul *) (PalImpureBase + CNS_Q_GPR);
  SavedFloatingPointRegisters =		(ul *) (PalImpureBase + CNS_Q_FPR);
  SavedPALTempRegisters =		(ul *) (PalImpureBase + CNS_Q_PT);
  SavedExceptionAddressRegister =	(ul *) (PalImpureBase + CNS_Q_EXC_ADDR);

  /* Initialize calls to malloc. */
  storage_initialized = FALSE;
  init_storage_pool();
  heapsize = storage_pool_end - storage_pool_start;

/*
 * Init variable for specifying the type of the register names to 
 * be used: software (1) or hardware names (0).
 */
  regmode = 0;

/*
 * The call to kinitdebugger() will establish
 * interrupt and exception handlers as well as
 * the breakpoint table, etc.
 */
#ifdef NEEDDEBUGGER
  kinitdebugger();
#endif

/*
 * Initialize whatever buses this device has.
 */
  outLed(led_k_init_IO);   /* About to complete IO bus initialization */
  IOBusInit();
  outLed(led_k_IO_inited);  /* IO bus initialization complete */

/*
 * Now initialize the console port then we can print messages to
 * the screen etc.
 */
  console_port_init(); 
  outLed(led_k_console_inited);  /* Console ports initialized */

  puts("\n\n===============================================================");
  if (argc)
  {
#ifdef CNS__PLACE_HOLDER18
#ifdef Debug_Dual
   printf("Primary PalImpureBase %lx\n",argv);
   printf("Cpu Lock %lx %lx\n",argv+(CNS__PLACE_HOLDER18/8),CNS__PLACE_HOLDER18);
#endif
   second_cpu_lock= *(int **)(argv+(CNS__PLACE_HOLDER18/8));
   cpu_count= *second_cpu_lock;
   printf("Booting with %d Cpu(s)\n",cpu_count);
   second_stack= (ul **)second_cpu_lock;
   second_stack++;
   secondary_cpu_function= NULL;
   DumpSysData();
#else /* CNS__PLACE_HOLDER18 not defined so the system should not be dual */
   printf("\n\nThis firmware does not support Dual CPUs but it thinks that there is a second CPU ! !\n\n");
#endif /* CNS__PLACE_HOLDER18 */
  }
  printf("PALcode Startup Code: %d (%s)\n",
	 halt_code, halt_code_string(halt_code));
  printf("PALcode Base Address: 0x%04lx, PALcode Data Segment: 0x%04lx\n",
	 *(ul *)(PalImpureBase + CNS_Q_PAL_BASE),
	 PalImpureBase);
  if (heapsize > 0) {
    printf("Memory Heap: %d%s, start=0x%04lx, end=0x%04lx\n",
	   (heapsize>(1024*1024)) ? heapsize/(1024*1024) : heapsize/1024,
	   (heapsize>(1024*1024)) ? "MB" : "KB",
	   storage_pool_start, storage_pool_end);
  }
  printf("Debug Monitor Entry Address: 0x%04lx\n", &__start);
#ifdef DP264
  if(isa_present)
  {
    srom_init_lcd();
    srom_access_lcd(0x80,0);
    srom_access_lcd('F',1);
    srom_access_lcd('D',1);
    srom_access_lcd('H',1);
  }
#endif


  printf("Initial Stack Pointer: 0x%lx\n", InitialStackPointer);

  if (sysdata.valid) {
    int tmp = 1000000000/(*sysdata.cycle_cnt);
    tmp = tmp/10 + (tmp%10 >= 5 ? 1 : 0);
    printf("CPU Speed: %d psec (%d.%0.2d MHZ)\n",
	   *sysdata.cycle_cnt, tmp/100, tmp%100 );
    /* DumpSysData(); */
  }
			   
  outLed(led_k_checking_rtc);  /* Checking Real Time Clock */
  InitRTC();

  /* Initialize the ethernet interface and protocol handlers */
  etherbuffers = (char *) DMA_BUFF_BASE;
  netman_setup();

/*
 * Read back any valid things stored in BBRAM.
 */
    bootadr = get_bootadr();

    BBRAM_READ(BBRAM_BOOTADR + 4, val);
    if (val != BBRAM_BOOTADR_COOKIE(bootadr))
	bootadr = DEFAULT_BOOTADR;
   
    BBRAM_READ(BBRAM_RMODE, regmode);
    BBRAM_READ(BBRAM_RMODE + 1, val);
    if (val != BBRAM_RMODE_COOKIE(regmode))
        regmode = 0;
   
#ifdef NEED_ETHERNET_ADDRESS
    get_eaddr();    /* this function checks for bad data in the BBRAM */
#endif

#ifdef BANNER
    printf ("\n%s\n", BANNER);
#else
    puts("\nAlpha Evaluation Board Debug Monitor");
#endif
    PrintVersion();

    memdetect();

    if (sysdata.valid) mem_size = (ul) (*sysdata.mem_size);
    printf(" Bootadr: 0x%lx   Memory Size: 0x%lx (%dMB)\n\n",
	   bootadr, mem_size, mem_size/(1024*1024));
#ifdef Debug_Dual
  *second_stack= (ul *)malloc(10000);
  *second_cpu_lock= 0;
#endif
  if (noDebugger)
    Prompt = "Debuggee> ";
  else {
#ifdef PROMPT
    Prompt = PROMPT;
#else
    Prompt = "DBM> ";
#endif
  }

  BeepCode(beep_k_init_done); /* Beep to inform the user. */
  outLed(led_k_init_done);  /* Powerup init done */
  puts("\007");		/* Send the terminal a beep */

  while (TRUE)
    CommandExec();

  /* We should never get here... */
  puts("Debug Monitor Panic (Exiting...)");
  return 0;
 }

void CommandExec (void)
{
  void (*function)(void);

  if (FlushFunction != NULL)
    (*FlushFunction)();		/* Flush any typeahead characters */

  printf(Prompt);

  if (!get_command()) return;

  /* now search for the command, and call it, if it exists */
  function = Lookup();
  if (function != NULL) (*function)();
}
#include "palcsrv.h"
#include "paldata.h"
#include "palosf.h"
void SRM_hack_Waitforever(ul *PalImpureBase);

void Waitforever(ul *PalImpureBase)
{
  void (*function)(void);
  ul second_address;

   printf("Seconary PalImpureBase %lx\n",PalImpureBase);
   *second_cpu_lock= 0;
/*    printf("\nsecondary_cpu_function %lx %lx\n",secondary_cpu_function,&secondary_cpu_function); */
#if 0
    second_address= (ul)SRM_hack_Waitforever;
    second_address+= 8;
    printf("second Jumping to 0x%06x...0x%06lx\n\n", second_address,*(ul *)second_address);
    imb();
    cServe(second_address, 0, CSERVE_K_JTOPAL);
#endif
    while (TRUE)
    {
      while(secondary_cpu_function== NULL);
      function=   secondary_cpu_function;
/*      printf("\nsecondary_cpu_function %lx\n",secondary_cpu_function); */
      secondary_cpu_function= NULL;
      (*function)();
    }
}
#ifndef _WIN32
void SRM_hack_WakeUp()
{
#ifdef __GNUC__
  __asm__ (
    "mb;\n\t"
    "ldah $1, 0x400f($31);\n\t"
    "lda  $1, 0xe000($1);\n\t"
    "sll  $1, 13, $1;\n\t"
    "subq $31, 1, $2;\n\t"
    "sll  $2, 47, $2;\n\t"
    "bis  $2, $1, $1;\n\t"
    "lda  $2, 0x5a($31);\n\t"
    "stb  $2, 0x80($1);\n\t"
    "bis	$31, $16, $30;\n\t"
    "bis	$31, $17, $27;"
    :
    :
    : "$1", "$2", "$16", "$17", "$27"
  );
#else
  asm("	mb;\
	ldah    %t0, 0x400f(%zero);\
	lda     %t0, 0xe000(%t0);\
	sll     %t0, 13, %t0;\
	subq    %zero, 1, %t1;\
        sll     %t1, 47, %t1;\
	bis	%t1, %t0, %t0;\
	lda	%t1, 0x5a(%zero);\
	stb	%t1, 0x80(%t0);\
	bis	%zero, %a0, %sp;\
	bis	%zero, %a1, %t12;\
	",*second_stack,Waitforever);
//	addq	%t12, 8, %t12;\
//.long	0x47FF041f;	\
//.long   0x77FF1310;	\
//.long   0xF7E00000;	\
//.long   0x7Bfb2000;",*second_stack,Waitforever);
    while (TRUE)
    {
	;
    }   
#endif
}

void SRM_hack_Waitforever(ul *PalImpureBase)
{
 asm("        mb;\
.long	0x47e11400; \
.long	0x77e02840; \
.long	0x201f2086; \
.long	0x24000050; \
.long	0x213f01a0; \
.long	0x25290008; \
.long	0x49231729; \
.long	0x77e01110; \
.long	0x21290080; \
.long	0x6fc91000; \
.long	0x213f0130; \
.long	0x25290008; \
.long	0x49231729; \
.long	0x21290a00; \
.long	0x20bf0045; \
.long	0x7ca91000; \
.long	0x21290040; \
.long	0x47ff041f; \
.long	0x47ff041f; \
.long	0x7ca91000; \
.long	0x60004000; \
.long	0x47c07009; \
.long	0x20bf0a00; \
.long	0x209f0a40; \
.long	0x452404c5; \
.long	0x209f0130; \
.long	0x24840008; \
.long	0x48831724; \
.long	0x40850405; \
.long	0x209f0100; \
.long	0x41240649; \
.long	0x249f8000; \
.long	0x48821724; \
.long	0x44890404; \
.long	0x63e4e800; \
.long	0xe7e0000f; \
.long	0x47ff041f; \
.long	0x60004000; \
.long	0x6c850000; \
.long	0x47ff041f; \
.long	0xf09ffffb; \
.long	0x47ff041f; \
.long	0x60004000; \
.long	0x6f290000; \
.long	0x4720341b; \
.long	0x47ff041f; \
.long	0x60004000; \
.long	0x47ff041f; \
.long	0x77ff1310; \
.long	0xf7e00000; \
.long	0x7bfb2000; \
.long	0xe7fffff0; \
       mb;");
}
#endif

void EstablishInitData (void)
{
/*
 * A -1 value indicates that the ethernet devices have not yet been
 * registered.  This is necessary before the call to initdata() when
 * interrupts are already enabled before starting the Debug Monitor
 * such as when it code is running under control of a remote debugger.
 * When interrupts are enabled the ethernet device is polled on RTC
 * interrupts if there is one registered.  
 */
  monitor_ether_device = -1;
}
