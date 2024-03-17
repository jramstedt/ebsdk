
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
static char *rcsid = "$Id: ffexec.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * $Log: ffexec.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.78  1998/06/26  18:22:46  gries
 * changes for pc264
 *
 * Revision 1.77  1997/12/15  20:52:49  pbell
 * Updated for dp264.
 *
 * Revision 1.76  1997/05/01  19:43:47  pbell
 * Cleaned up warnings on WriteB and WriteW usage
 *
 * Revision 1.75  1997/04/11  03:06:56  fdh
 * Modified the jtopal function to check for and skip
 * over a ROM header.
 *
 * Revision 1.74  1997/04/10  17:21:38  fdh
 * Added conditional for Linux.
 *
 * Revision 1.73  1997/02/25 16:26:06  fdh
 * Clear ieee emulation signature in calls to wrfen().
 *
 * Revision 1.72  1997/02/21  00:50:21  fdh
 * Added conditions to disable the floating point unit when using
 * IEEE emulation code for performing all floating point
 * operations.
 *
 * Revision 1.71  1996/11/17  12:30:08  fdh
 * Fixed a simple comment.
 *
 * Revision 1.70  1996/08/22  14:02:42  fdh
 * Moved I/O port stuff to another module.
 *
 * Revision 1.69  1996/06/13  03:38:13  fdh
 * Added support for the BROADCAST port type.
 *
 * Revision 1.68  1996/05/22  22:08:09  fdh
 * Added support for the SROM serial port.
 *
 * Revision 1.67  1996/02/14  18:55:01  cruz
 * Changed ChecksumMem to return the computed checksum and not
 * print it out.
 *
 * Revision 1.66  1996/01/26  00:05:00  cruz
 * Rewrote csrv() so it wouldn't have any system
 * dependent calls.  This routine should really be removed.
 *
 * Revision 1.65  1995/12/15  21:50:57  cruz
 * Modified PrintVersion to print out the debug monitor version number, not
 * just the date it was created.
 *
 * Revision 1.64  1995/12/13  23:31:26  cruz
 * Make code wait for character before reading it.
 *
 * Revision 1.63  1995/12/07  23:47:31  cruz
 * Call netman_stop_all_devices instead of netman_stop_monitor_device
 * before jtopal.
 *
 * Revision 1.62  1995/11/30  20:04:55  cruz
 * Modified ChangeMem to allow the user to press RETURN on an
 * empty line to leave the current location unchanged.
 *
 * Revision 1.61  1995/11/09  21:56:28  cruz
 * Fixed erroneous comparison of strcmp to NULL.
 *
 * Revision 1.60  1995/11/06  23:13:22  fdh
 * Use printf modifiers for printing unsigned long values.
 *
 * Revision 1.59  1995/11/01  15:49:26  cruz
 * Changed FillMem() so that it also writes the last address of the
 * range specified.
 *
 * Revision 1.58  1995/10/31  18:52:50  cruz
 * Removed const keyword from reference to compile_date and
 * compile_time variables.
 *
 * Revision 1.57  1995/10/30  16:49:45  cruz
 * Removed some routines not in use.
 *
 * Revision 1.56  1995/10/26  22:31:09  cruz
 * Added casting and fixed prototypes.
 *
 * Revision 1.55  1995/10/24  21:32:09  cruz
 * Removed declaration of sysdata which is already declared in initdata.c
 *
 * Revision 1.54  1995/10/24  18:31:58  fdh
 * Corrected argument passing for ExecuteProgram().
 *
 * Revision 1.53  1995/10/23  20:13:19  cruz
 * Removed declaration of external variable "stopped" since it's
 * not included in ladebug.h
 *
 * Revision 1.52  1995/10/22  05:05:49  fdh
 * Use PrintPorts() instead of printf().
 *
 * Revision 1.51  1995/10/18  18:03:05  fdh
 * Stop Ethernet device to preempt DMA activity
 * before executing new image.
 *
 * Revision 1.50  1995/10/18  12:56:03  fdh
 * Added a cast to pass the proper data type to PutChar().
 * Renamed arguments passed to ExecuteProgram to avoid a
 * conflict with a global definition.
 *
 * Revision 1.49  1995/10/10  14:22:06  fdh
 * Renamed ladbx.h to ladebug.h.
 *
 * Revision 1.48  1995/10/04  23:37:53  fdh
 * Moved file operations to file.c
 *
 * Revision 1.47  1995/10/04  19:37:24  cruz
 * Changed informational message for LoadFile routine.
 *
 * Revision 1.46  1995/10/03  20:49:55  fdh
 * Updated internal function prototypes.
 *
 * Revision 1.45  1995/10/03  03:10:40  fdh
 * Removed nttypes.h.
 * Modified ChangeMem() to use get_command().
 *
 * Revision 1.44  1995/09/28  20:26:57  cruz
 * Added routine that implements the flread and flwrite commands.
 *
 * Revision 1.43  1995/09/21  16:04:55  cruz
 * The fopen command now takes in a new mode when
 * specifying a directory path, therefore, the FileDirectory
 * routine had to be changed.
 *
 * Revision 1.42  1995/09/15  19:29:51  cruz
 * Added routines for doing file i/o.
 *
 * Revision 1.41  1995/09/12  21:25:42  fdh
 * Call edit_cmdline() instead of MonGetChar()
 *
 * Revision 1.40  1995/09/05  18:33:40  fdh
 * Removed some unnecessary include files.
 *
 * Revision 1.39  1995/09/02  03:38:56  fdh
 * Replaced obsoleted call with calls to standard routines.
 *
 * Revision 1.38  1995/08/31  21:59:18  fdh
 * Pass command line arguments to ExecuteProgram().
 *
 * Revision 1.37  1995/08/30  23:17:19  fdh
 * Fixup debugger conditions after a User program exits.
 * - Restore breakpoint instructions and set stopped = TRUE.
 *
 * Revision 1.36  1995/08/25  20:13:23  fdh
 * Tip command accepts arguments like com1, COM2 as well as 1 or 2.
 * Setty command accepts arguments like graphics, COM1, com2
 * as well as 0, 1, or 2.
 *
 * Revision 1.35  1995/05/19  20:36:18  fdh
 * Added function to return PALcode Halt Code Strings.
 *
 * Revision 1.34  1995/04/20  16:51:40  cruz
 * Added a default condition for the cserve call..
 *
 * Revision 1.33  1995/02/27  19:18:49  fdh
 * Removed O/S conditionals for the version timestamp.
 *
 * Revision 1.32  1995/01/25  19:38:56  fdh
 * Modified ChecksumMem() to use BSD4.3 checksum
 * algorithm.
 *
 * Revision 1.31  1994/12/01  15:47:59  fdh
 * Corrected logic around printing compared data.
 *
 * Revision 1.30  1994/11/23  20:16:04  fdh
 * Modified for eb164 cserves
 *
 * Revision 1.29  1994/11/17  14:08:41  fdh
 * Allow longword granularity in memory compare.
 *
 * Revision 1.28  1994/11/09  19:12:13  fdh
 * Created CompareMem().
 *
 * Revision 1.27  1994/08/08  00:55:49  fdh
 * added call to kbdcontinue() to PrintMem().
 *
 * Revision 1.26  1994/08/05  20:18:01  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.25  1994/07/25  14:49:30  fdh
 * Changed SROM revisions "Experimental" message.
 *
 * Revision 1.24  1994/07/13  14:28:30  fdh
 * Modified PrintVersion() to use the sysdata structure.
 *
 * Revision 1.23  1994/07/11  17:43:48  fdh
 * Fix up conversion between different integral types.
 *
 * Revision 1.22  1994/07/11  14:24:21  fdh
 * Don't print SROM revision: UNKNOWN unless tracing is enabled.
 *
 * Revision 1.21  1994/07/01  14:53:02  fdh
 * Added RCS version decoding...
 *
 * Revision 1.20  1994/06/24  14:37:18  rusling
 * Fixed pl infinite loop problem in printMem().
 *
 * Revision 1.19  1994/06/24  14:29:55  berent
 * IPL when starting user program changed to 4 (from 0)
 *
 * Revision 1.18  1994/06/23  14:34:47  rusling
 * Fixed up WNT compilation warnings (again).
 *
 * Revision 1.17  1994/06/23  13:43:51  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.16  1994/06/20  18:01:45  fdh
 * Modified to use new ANSI compliant PALcode definitions.
 *
 * Revision 1.15  1994/06/17  19:36:54  fdh
 * Clean-up...
 *
 * Revision 1.14  1994/06/14  09:39:20  rusling
 * changed date and time to compile_date and compile_time
 * to avoid clashing with the system routine time().
 *
 * Revision 1.13  1994/06/03  20:23:41  fdh
 * Added lib.h include file and some external declarations.
 * Removed the MoveBlock() routine to use memmove from
 * the Standard C Library instead.
 *
 * Revision 1.12  1994/04/06  04:27:46  fdh
 * Modified for updated cserve.h file.
 * Make sure that floating point is enabled incase
 * it was disabled by the user program.
 *
 * Revision 1.11  1994/04/03  00:38:36  fdh
 * Changed repeat argument to number of iterations for
 * for PrintMem().
 *
 * Revision 1.10  1994/03/24  21:34:09  fdh
 * Fixed up placement of NEEDDEBUGGER conditionals.
 *
 * Revision 1.9  1994/03/24  21:12:21  fdh
 * Moved csrv() to ffexec.c and fixed masks.
 *
 * Revision 1.8  1994/03/18  21:38:15  fdh
 * Slight modification to the version line...
 *
 * Revision 1.7  1994/03/18  17:13:41  rusling
 * Tell the user which OS the DBM was built on.
 *
 * Revision 1.6  1994/03/17  12:14:03  rusling
 * Changed so that version works on WNT.
 *
 * Revision 1.5  1994/01/19  10:50:12  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.4  1993/11/19  16:58:14  fdh
 * Removed kbdcontinue().  Moved to fftty.c
 *
 * Revision 1.3  1993/10/03  01:08:10  berent
 * Merge in development by Anthony Berent
 *
 *>> Revision 1.3  1993/10/01  10:50:56  berent
 *>> Ensure that applications start with interrupts enabled; and with the correct
 *>> system entry points; and that interrupts are disabled when they finish.
 *
 * Revision 1.2  1993/06/09  20:23:05  fdh
 * Added kbdcontinue() function used to prompt user to continue
 * or abort printing more than a reasonable amount of information
 * to the screen at once.
 *    i.e. Hit any key to continue. Control-C to quit...
 *
 * Revision 1.1  1993/06/08  19:56:40  fdh
 * Initial revision
 *
 */


#include "system.h"
#include "lib.h"
#include "mon.h"
#include "ladebug.h"
#include "bbram.h"
#include "ctype.h"
#include "console.h"
#include "file.h"
#include "ether.h"
#include "romhead.h"
#include "callback.h"
#include "palcsrv.h"
#include "paldata.h"
#include "palosf.h"
#include "interlock.h"


/* global variables */
ul default_address;
static  volatile int lock=0;

extern char compile_date[], compile_time[], dbm_version[];
extern BOOLEAN SromPortInitialized;
extern int cpu_count;
/* Internal Function Prototypes */
static void PrintLine(int size , ul addr , int silent);
static void PrintPlace(int size , ul place);

/* Utility routines */

static void PrintLine(int size, ul addr, int silent)
{
    int    i;
    ul   data[2];
    ui *alw;
    uw *ahw;
    ub *abyte;

    abyte = (ub *) data;
    ahw = (uw *) data;
    alw = (ui *) data;

    data[0] = ReadQ(addr);
    data[1] = ReadQ(addr + sizeof(ul));

    if (silent) return;

    printf("%08lx: ", addr);

    if (size == 3)
      {
        PQ(data[0]);
        PutSpace();
        PQ(data[1]);
        PutSpace();
      }
    if (size == 2)
      for (i = 0; i < 4; i += 1)
      {
        PL(alw[i]);
        PutSpace();
      }
    if (size == 1)
      for (i = 0; i < 8; i += 1)
      {
        PW(ahw[i]);
        PutSpace();
      }
    if (size == 0)
      for (i = 0; i < 16; i += 1)
      {
        PB(abyte[i]);
        PutSpace();
      }
    for (i = 0; i < 16; i += 1)
      PutChar ((char) (_isprint(abyte[i]) ? abyte[i] : '.'));
    PutCR();
}

static void PrintPlace(int size, ul place)
{
  printf("%08lx: ", place);
  PutSpace();
  if (size == 3) PQ(ReadQ(place));
  if (size == 2) PL(ReadL(place));
  if (size == 1) PW(ReadW(place));
  if (size == 0) PB(ReadB(place));
  printf(": ");
}

void ExecuteProgram(Function_t place, int _argc , char * _argv[])
{
  int retval;
  int save_ipl;

  imb();

  printf("Executing at 0x%06x...\n\n", place);
  
  retval = (*place)(_argc, *_argv);

  restore_breakpoint_instrs();
  stopped = TRUE;		/* Set stopped when the User program exits. */

#ifndef __EMULATE_FLOATING_POINT
/*
 * Make sure that floating point is enabled incase
 * it was disabled by the user program.
 */
  wrfen(1, (ul) 0);
#endif /* __EMULATE_FLOATING_POINT */

  printf("program returned %d, (0x%x)\n", retval, retval);
}

void PrintMem(int size, ul first, ul last, ul iterations, int silent)
{
    ul savfirst;
    int lines_printed;

    first &= (ul)(-16);  /* align pointer */
    savfirst = first;

    lines_printed = 0;
    do {
       first = savfirst;
       while (first <= last) {
	   PrintLine (size, first, silent);
	   if (!silent && ++lines_printed && (lines_printed%20) == 0)
	     if (!kbdcontinue()) return;
	   first += 16;
       }
       if (iterations != 0) iterations = iterations - 1;
    } while (iterations != 0);
    default_address = (ul) first;
}

void CompareMem(ul first, ul last, ul copy)
{
    ui   data[4];
    ub *abyte;
    int lines_printed;
    int i;

    abyte = (ub *) data;

    first &= (ul)(-4);  /* align pointer */
    copy &= (ul)(-4);  /* align pointer */

    lines_printed = 0;
    while (first <= last) {
      data[0]=ReadL(first);
      data[1]=ReadL(first+4);
      data[2]=ReadL(copy);
      data[3]=ReadL(copy+4);
      if ((data[0] != data[2]) || (data[1] != data[3])) {

	printf("%08lx: ", first);
	for (i = 0; i < 8; i += 1) {
	  PB(abyte[i]);
	  PutSpace();
	}

	PutSpace();
	PutSpace();

	printf("%08lx: ", copy);
	for (i = 8; i < 16; i += 1) {
	  PB(abyte[i]);
	  PutSpace();
	}

	PutCR();

	++lines_printed;
	if ((lines_printed%20) == 0)
	  if (!kbdcontinue()) return;
      }

      first += sizeof(ul);
      copy += sizeof(ul);
    }
}

void ChangeMem(int size, ul place)
{
  int gc_argc;

  place &= (ul)(-1<<size);  /* align pointer */

  while (TRUE) {
    PrintPlace(size, place);
    gc_argc = get_command();
    if (gc_argc && !hexargok[0]) break;
    if (gc_argc) {
        if (size == 3) WriteQ (place, hexarg[0]);
        if (size == 2) WriteL (place, (ui)hexarg[0]);
        if (size == 1) WriteW (place, (uw)hexarg[0]);
        if (size == 0) WriteB (place, (ub)hexarg[0]);
    }
    place += (1 << size);
  }

  default_address = (ul) place;
}

void FillMem(ul first, ul last, ui value)
{
  first &= (ul)(-1<<2);		/* longword align pointer */
  while (first <= last) {
    WriteL ( first, value);
    first += sizeof(ui);
  }
  default_address = last;
}

typedef enum REVISIONS {
  UNKNOWN,
  UNLOCKED,
  LOCKED,
  LITERAL
} Rev_t;

void PrintVersion(void)
{
  int i;
  int rev;
  Rev_t state;
#define RCS_LOCK_SIGNATURE 0x55
#define RCS_REV_LEVELS 8

#ifdef _WIN32
  const char os_string[] = "Windows NT";
#endif
#ifdef __osf__
  const char os_string[] = "Digital Unix";
#endif
#ifdef __linux__
  const char os_string[] = "Linux";
#endif

  printf(" Version %s built on %s %s %s\n", dbm_version, os_string, compile_date, compile_time);

  PRTRACE2(" Encoded SROM revision: %016x\n", *sysdata.srom_rev);

  rev = (int)((*sysdata.srom_rev>>(8*(RCS_REV_LEVELS-1)))&0xff);
  PRTRACE2(" RCS_LOCK_SIGNATURE: 0x%x\n", rev);

  if (rev == (int)(RCS_LOCK_SIGNATURE&0xff)) {
    state = LOCKED;
  }
  else {
    if (isalpha(rev))
      state = LITERAL;
  }

  if (rev == (int)(~RCS_LOCK_SIGNATURE&0xff))
    state = UNLOCKED;

  if (state == UNKNOWN) {
    PRTRACE1(" SROM revision: UNKNOWN\n");
    return;
  }

  printf(" SROM revision: ");

  if (isalpha(rev))
    UserPutChar(rev);

  for (i=0; i<RCS_REV_LEVELS-1; ++i)
    {
      rev = (int)(*sysdata.srom_rev>>(8*i))&0xff;
      if (rev == 0) break;
      printf("%s%d", (i==0)?"":".", rev);
    }
  printf("%s\n", (state==LOCKED) ? "(Experimental)" : "");
}

void SRM_hack_Waitforever(ul *PalImpureBase);

void jToPal(ul destaddr)
{
  int i= 1;
  volatile ul real_destaddr= destaddr;
  ul second_address;

  romheader_t * RomImageHeader = (romheader_t *) destaddr;
  if (atomic_inc(1,&lock)==0)
  {
    printf("atomic_inc  ret %d\n", lock);
    netman_stop_all_devices();	/* Stop Ethernet devices */
    if ((RomImageHeader->romh.V0.signature == ROM_H_SIGNATURE) &&
      (RomImageHeader->romh.V0.csignature == (ui) ~ROM_H_SIGNATURE)) {
      real_destaddr += RomImageHeader->romh.V0.hsize;
    }

    printf("Jumping to 0x%06lx...0x%06lx\n\n", real_destaddr,*(ul *)real_destaddr);
    while (i && (lock<cpu_count))
	i++;
    lock =0;
  imb();
  }
  else
  {
    second_address= (ul)SRM_hack_Waitforever;
    second_address+= 8;
    printf("second Jumping to 0x%06x...0x%06lx\n\n", second_address,*(ul *)second_address);
    imb();
    cServe(second_address, 0, CSERVE_K_JTOPAL);
  }

  cServe(real_destaddr, 0, CSERVE_K_JTOPAL);

#ifndef __EMULATE_FLOATING_POINT
/*
 * Make sure that floating point is enabled incase
 * it was disabled by the user program.
 */
  wrfen(1, (ul) 0);
#endif /* __EMULATE_FLOATING_POINT */
}

int get_bootadr(void)
{
  ui val=0,x, v;
  
  for (x=0;x<4;x++) {
    BBRAM_READ(BBRAM_BOOTADR + x, v);
    val |= v << (x * 8);
  }
  return val;
}

char *halt_code_string(int halt_code)
{
  char *halt_string;
  switch(halt_code) {
  case HLT_K_RESET:
    halt_string = "Reset...";
    break;
  case HLT_K_HW_HALT:
    halt_string = "Hardware halt";
    break;
  case HLT_K_KSP_INVAL:
    halt_string = "Invalid Kernel Stack Pointer";
    break;
  case HLT_K_SCBB_INVAL:
    halt_string = "Invalid System Control Block Base";
    break;
  case HLT_K_PTBR_INVAL:
    halt_string = "Invalid Page Table Base Register";
    break;
  case HLT_K_SW_HALT:
    halt_string = "Software Halt";
    break;
  case HLT_K_DBL_MCHK:
    halt_string = "Double Machine Check";
    break;
  case HLT_K_MCHK_FROM_PAL:
    halt_string = "Machine Check from PALcode";
    break;
  default:
    halt_string = "Unknown";
  }
  return(halt_string);
}
