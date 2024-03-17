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
static char *rcsid = "$Id: fsbmain.c,v 1.2 1999/01/21 19:04:39 gries Exp $";
#endif

/*
 * $Log: fsbmain.c,v $
 * Revision 1.2  1999/01/21 19:04:39  gries
 * First Release to cvs gries
 *
 * Revision 1.10  1999/01/19  16:45:15  gries
 * extern int isa_present; for web brick
 *
 * Revision 1.9  1998/12/17  19:17:44  gries
 * added code to detect isa (for goldrush vs webbrick)
 *
 * Revision 1.8  1998/08/03  17:27:25  gries
 * added volatile to ul destaddr;
 *
 * Revision 1.7  1998/08/03  17:26:09  gries
 * Make work in non_debug_mode
 *
 * Revision 1.6  1997/12/15  20:52:49  pbell
 * Updated for dp264.
 *
 * Revision 1.6  1997/11/19  19:56:29  gries
 * added wait forever
 *
 * Revision 1.5  1997/06/02  04:30:55  fdh
 * Added a stub for EstablishInitData().
 *
 * Revision 1.4  1997/04/11  03:29:24  fdh
 * Modified to detect and skip over ROM headers.
 * Initialize call memory storage heap.
 *
 * Revision 1.3  1997/02/25  02:20:25  fdh
 * Added stub for puts().
 *
 * Revision 1.2  1996/07/11  14:15:34  fdh
 * Modified to only do ISA and basic PCI bus initialization.
 * Thus the PCI bus configuration is not done.  This gets around
 * problems booting other programs which get confused by the way
 * that the PCI devices might have been configured.  Other programs
 * must configure the PCI bus themselves.
 *
 * Revision 1.1  1996/02/26  19:31:42  fdh
 * Initial revision
 *
 * Revision 1.8  1996/02/21  21:41:12  fdh
 * Added call to BeepCode().
 *
 * Revision 1.7  1995/12/17  22:51:17  fdh
 * Removed FWUPDATE_FILE_ALT definition.
 *
 * Revision 1.6  1995/12/02  12:58:57  fdh
 * Added alternate firmware update file and entry point definitions.
 * This is loaded from the floppy as an alternative when the primary
 * firmware is not present on the floppy.
 *
 * Revision 1.5  1995/11/27  19:23:53  cruz
 * Output ledcode value before loading fwupdate file.
 *
 * Revision 1.4  1995/11/09  21:56:28  cruz
 * Added casting in initialization of InitialStackPointer.
 *
 * Revision 1.3  1995/10/26  21:50:17  cruz
 * Added a return value for main().
 *
 * Revision 1.2  1995/10/05  01:09:39  fdh
 * Initialize calls to malloc().
 *
 * Revision 1.1  1995/10/04  23:38:32  fdh
 * Initial revision
 *
 */

#include "fsboot.h"
#include "romhead.h"

extern int isa_present;
int halt_code = -1;
ul InitialStackPointer = (ul) -1L;
extern int storage_initialized;

/* Stubs needed for initdata() */
void ladbx_poll(void) { return;}

#ifndef EB_DEBUG_FSBOOT
/* Stubs needed to disable printing */
int printf(const char *f, ...) { return 0;}
int sprintf(char *str, const char *f, ...) { return 0;}
void PutChar(char c) { return;}
int GetChar(void) { return 0;}
int CharAv(void) { return 0;}
int puts(const char *f) {return 0;}
#else
volatile int stopped;
void kstop(void){;}
void DumpLogout(...){;}
void DumpShortLogout(...){;}
void ParseLogout(...){;}
void SetMcheckSystem(...){;}
BOOLEAN IsMousePresent( int PortBase ){ return 0;}
#endif

int main(int argc, char *argv[])
{
  romheader_t * RomImageHeader;
  volatile ul destaddr;

  /* Initialize calls to malloc. */
  storage_initialized = FALSE;
  init_storage_pool();

  outLed(led_k_init_IO);   /* About to complete IO bus initialization */

/*
 * Initialize the ISA Bus.
 */
  ISABusInit();

/*
 * Only do basic PCI initialization.
 */
#ifdef NEEDPCI
  PCIBusInit();
#endif

  outLed(led_k_IO_inited);  /* IO bus initialization complete */

#ifdef EB_DEBUG_FSBOOT
/*
 * Now initialize the console port then we can print messages to
 * the screen etc.
 */
  console_port_init(); 
#endif

  if (fsboot(&destaddr)) {
    RomImageHeader = (romheader_t *) destaddr;
    if ((RomImageHeader->romh.V0.signature == ROM_H_SIGNATURE) &&
	(RomImageHeader->romh.V0.csignature == (ui) ~ROM_H_SIGNATURE)) {
      destaddr += RomImageHeader->romh.V0.hsize;
    }

    printf("Jumping to 0x%06x...\n\n", destaddr); 
    cServe(destaddr, 0, CSERVE_K_JTOPAL);
  }

  return 0;
}

void Waitforever(void)
{
  while(1)
  ;
}
void EstablishInitData (void)
{
}
