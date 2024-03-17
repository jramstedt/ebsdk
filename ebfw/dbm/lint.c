
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
 *  This is a dummy file used when "linting" the debug monitor files.  It
 *  contains routines/variables that are implemented in assembly language
 *  (which lint can't process) and/or calls to C routines that get called from
 *  either assembly language or other files on which lint is not run. 
 *
 * $Log: lint.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.8  1997/06/10  18:16:40  fdh
 * Removed obsoleted declarations.
 *
 * Revision 1.7  1996/07/11  14:28:00  fdh
 * Added a call to the abort() stub.
 *
 * Revision 1.6  1996/06/13  03:36:20  fdh
 * Added use of a few functions.
 *
 * Revision 1.5  1996/05/25  20:32:40  fdh
 * Added ostype_dump.
 *
 * Revision 1.4  1996/05/22  22:07:17  fdh
 * Added calls to functions that lint reports are unused.
 * Modified args passed to lintme for use by the call to
 * vprintf.
 *
 * Revision 1.3  1996/01/15  20:40:27  cruz
 * Updated for name change of a header file.
 *
 * Revision 1.2  1995/10/31  22:45:49  cruz
 * Updated.
 *
 * Revision 1.1  1995/10/31  18:57:46  cruz
 * Initial revision
 *
 * 
 */
#ifdef LINT

#include "lib.h"
#include "ether.h"
#include "ladebug.h"
#include "asmstuff.h"
#include "mon.h"
#include "pci.h"
#include "rom.h"
#include "callback.h"
#include <stdarg.h>

#include "halp.h"

#ifdef NEEDFLASHMEMORY
#include "fwup.h"
#include "flash8k.h"

PFLASH_DRIVER HalpFlashDriver;
#endif


ul _fbss,  _end, _edata;
char __start; 
extern volatile int volx;

/*
 *  Place holders for assembly routines (asmstuff.s)
 */
void mb(void){ return; }
ui rpcc(void){return 0;}
ul getsp(void){return 0;}
ul cServe(ul f, ul g, ul t, ...){return 0;}
void wrfen(ui g){return;}
void swppal(ul g, ul h, ul j, ul k){return;}
void halt(void){return;}
void wait_cycles(ul cycles){return;}
int swpipl(ui t){return 0;}
void wrmces(ul y){return;}
ui rdmces(void){return 0;}
void CleanBCache(ul r){return;}
void exit (int status){return;}

/*
 *  Place holders for assembly routines (kutil.s)
 */
void kutilinitbreaks(register_value *t){return;}

/*
 *  Place holders for assembly routines (ladbxusr.s)
 */
void ladbx_poll(void){return;}

/*
 *  Place holders for routines pulled in from the host's c libraries.
 */
char  *strcpy(char * y, const char *u){return NULL;}
char  *strncpy(char *y, const char *u, size_t i){return NULL;}
int   strcmp(const char *y, const char *u){return 0;}
int   strncmp(const char *y,const char *u,size_t j){return 0;}
char *strncat(char *s1, const char *s2, size_t n){return NULL;}
char *strcat(char *s1, const char *s2){return NULL;}
char *strchr(const char *s, int c){return NULL;}
char *strrchr(const char *s, int c){return NULL;}
char *strstr(const char *s1, const char *s2){return NULL;}
void *memmove(void *s1, const void *s2, size_t n){return NULL;}
int rand (void){return 0;}

/*
 *  Call all the C routines that get called from non-C modules here
 *  to avoid the warning about a function being defined but never used.
 */
void lintme(char *fmt, ...) {
   initdata(0);
   ntoh(NULL,0);
   rpcc();
   getsp();
   exit(0);
   halt();
   sleep(0);
   strcat(NULL, NULL);
   app_poll(0);
   swppal(0,0,0,0);
   kreenter(); ktrap(0,0,0,0);
   data_received(NULL, NULL, NULL);
   vgainit();
   ispunct(0); islower(0); isgraph(0); iscntrl(0); isxdigit(0); isalnum(0); isascii(0);
   CleanBCache(0);
   (*UserCharAv)(); (*Usertime)(NULL);
   (*Usermalloc_summary)(NULL); (*Usermalloc)(0);
   (*Userfree)(NULL); (*Userrealloc)(NULL, 0);
   (*Userladbx_poll)();
   c8514init();
   DisplayClose();
   _edata=0;

   WRITE_PORT_UCHAR (NULL, 0);
   WRITE_REGISTER_UCHAR (NULL, 0);
   READ_REGISTER_UCHAR (NULL);
   WRITE_REGISTER_ULONG (NULL, 0);
   READ_REGISTER_ULONG (NULL);

   in_ioc_csr(0);
   out_ioc_csr(0,0);
#ifdef NEEDFLASHMEMORY
   Am29F040_Initialize(NULL);
   I28F008SA_Initialize(NULL);
   WriteFlashUCHAR(volx,0);
   HalpFlashDriver = 0;
#endif

   InPciCfgB(0, 0, 0, 0);
   InPciCfgW(0, 0, 0, 0);
   InPciCfgL(0, 0, 0, 0);
   OutPciCfgB(0, 0, 0, 0,0);
   OutPciCfgW(0, 0, 0, 0,0);
   OutPciCfgL(0, 0, 0, 0,0);

   outdmemw(0, 0);
   indmemw(0);

   romimage_size(NULL);
   ostype_dump(NULL);

   fwid_dump(NULL);
   BeepCode(0);

   EnableIsaDmaChannel(0);

   SMCInit();

   intr_enable(0);

   isa_ether_device_map[0] = 0;

   {
     va_list ap;
     va_start(ap, fmt);
     vprintf(fmt, ap);
     va_end(ap);
   }

   abort( );
}

#endif

