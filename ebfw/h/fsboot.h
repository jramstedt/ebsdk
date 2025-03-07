#ifndef __FSBOOT_H_LOADED
#define __FSBOOT_H_LOADED
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
 *  $Id: fsboot.h,v 1.4 1999/02/11 23:04:38 gries Exp $;
 *
 * $Log: fsboot.h,v $
 * Revision 1.4  1999/02/11 23:04:38  gries
 * hanges to fsboot to lok for dp264* pc264* *
 *
 * Revision 1.3  1999/02/11 22:38:15  gries
 * nge atl_target to target
 *
 * Revision 1.2  1999/01/21 19:04:55  gries
 * First Release to cvs gries
 *
 * Revision 1.12  1998/12/29  16:09:24  gries
 * changes for brick and dp264 vs pc264
 *
 * Revision 1.11  1998/10/08  13:48:00  gries
 * added #define FSB_tst_PREFIX          0
 *
 * Revision 1.10  1998/10/08  13:37:39  gries
 * added:
 * #define FSB_tst_ENTRY           0x8000
 * #define FSB_tst_STRINGS         "tst.img", "tst.rom", NULL
 *
 * Revision 1.9  1997/12/15  20:48:24  pbell
 * Updated for dp264.
 *
 * Revision 1.8  1997/04/11  03:31:54  fdh
 * Added additional filename strings to support booting
 * .rom files.  Also added strings to support the Linux
 * Mini-Loader (Milo).
 *
 * Revision 1.7  1997/02/21  03:16:28  fdh
 * Modified the include file list.
 *
 * Revision 1.6  1996/08/20  02:50:57  fdh
 * Changed a typedef name to avoid a conflict.
 *
 * Revision 1.5  1996/07/11  16:05:23  fdh
 * Modified to make attempts at booting more alternative images
 * from the floppy.  The Fail-Safe Booter will now attempt to
 * boot, in the following order...
 *
 * 	"fwupdate.exe" - Firmware Update Utility
 * 	"XXXXXdbm.cmp" - Alpha Evaluation Board Debug Monitor
 * 	"XXXXXsrm.sys" - SRM Console
 * 	"XXXXXnt.cmp"  - Windows NT Firmware
 *
 * Revision 1.4  1996/05/21  16:16:10  fdh
 * Added prototype for fsboot();
 *
 * Revision 1.3  1996/02/26  19:34:51  fdh
 * Moved some general definitions from fsbmain.c to here.
 *
 * Revision 1.2  1996/02/21  21:41:39  fdh
 * Included beepcode.h.
 *
 * Revision 1.1  1995/10/04  23:38:42  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "lib.h"
#include "mon.h"
#include "ledcodes.h"
#include "beepcode.h"
#include "palcsrv.h"
#include "paldata.h"
#include "palosf.h"

extern int main(int argc, char *argv[]);
extern DBM_STATUS LoadAFile(char * SourceFile , char * Destination);

typedef  struct ImageData {
  ul destination;
  int prefix;
  const char **name;
} Image_t;

#define FSB_FWU_ENTRY		0x900000
#define FSB_DBM_ENTRY		0x300000
#define FSB_WNT_ENTRY		0x300000
#define FSB_SRM_ENTRY		0x900000
#define FSB_MILO_ENTRY		0x300000
#define FSB_tst_ENTRY		0x8000

#define FSB_FWU_STRINGS		"fwupdate.exe", NULL
#define FSB_DBM_STRINGS		"dbm.cmp", "dbm.rom", NULL
#define FSB_WNT_STRINGS		"nt.rom", "nt.cmp", NULL
#define FSB_SRM_STRINGS		"srm.sys", "srm.rom", NULL
#define FSB_MILO_STRINGS	"milo", "milo.rom", NULL
#define FSB_tst_STRINGS		"tst.img", "tst.rom", NULL
/*
 * This value selects prefixes when appropriate.
 */
#ifdef DP264
#define FSB_FWU_PREFIX		0
#define FSB_DBM_PREFIX		2
#define FSB_SRM_PREFIX		2
#define FSB_WNT_PREFIX		2
#define FSB_MILO_PREFIX		0
#define FSB_tst_PREFIX		0
#else
#define FSB_FWU_PREFIX		0
#define FSB_DBM_PREFIX		1
#define FSB_SRM_PREFIX		1
#define FSB_WNT_PREFIX		2
#define FSB_MILO_PREFIX		0
#define FSB_tst_PREFIX		0
#endif
#ifndef ALT_TARGET_NAME

/* dp264 sometimes aliases to eb64p */
#ifdef DP264
#define ALT_TARGET_NAME		"dp264"
#endif

/* pc64 sometimes aliases to eb64p */
#ifdef PC64
#define ALT_TARGET_NAME		"eb64p"
#endif

/* eb66p sometimes aliases to eb66 */
#ifdef EB66P
#define ALT_TARGET_NAME		"eb66"
#endif

#endif /* ALT_TARGET_NAME */


#ifndef ALT_TARGET_NAME
#define ALT_TARGET_NAME		TARGET_NAME
#endif


extern int fsboot(ul *destaddr);

#endif /* __FSBOOT_H_LOADED */
