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
__attribute__((unused)) static const char *rcsid = "$Id: fsboot.c,v 1.4 1999/02/11 23:06:02 gries Exp $";
#endif

/*
 * $Log: fsboot.c,v $
 * Revision 1.4  1999/02/11 23:06:02  gries
 * changes to fsboot to lok for dp264* pc264* *
 *
 * Revision 1.3  1999/02/03 16:14:20  gries
 * add include system.h such that dp264.h would be included
 *
 * Revision 1.2  1999/01/21 19:05:06  gries
 * First Release to cvs gries
 *
 * Revision 1.16  1998/12/17  19:26:58  gries
 * added code to move .rom files
 *
 * Revision 1.15  1998/10/08  13:39:23  gries
 * added code for lcd writes
 * for goldrush
 *
 * Revision 1.14  1997/04/24  20:49:29  pbell
 * Removed old varriables
 *
 * Revision 1.13  1997/04/11  15:54:22  fdh
 * Modified the search order for the firmware images.
 * 	1. Firmware Update Utility
 * 	2. Windows NT Firmware
 * 	3. Alpha Evaluation Board Debug Monitor
 * 	4. SRM Console
 * 	5. Linux Mini-Loader
 *
 * Revision 1.12  1997/04/11  03:31:09  fdh
 * Added additional filename strings to support booting
 * .rom files.  Also added strings to support the Linux
 * Mini-Loader (Milo).
 *
 * Revision 1.11  1996/08/20  02:50:57  fdh
 * Changed a typedef name to avoid a conflict.
 *
 * Revision 1.10  1996/07/11  14:59:27  fdh
 * Modified to make attempts at booting more alternative images
 * from the floppy.  The Fail-Safe Booter will now attempt to
 * boot, in the following order...
 *
 * 	"fwupdate.exe" - Firmware Update Utility
 * 	"XXXXXdbm.cmp" - Alpha Evaluation Board Debug Monitor
 * 	"XXXXXsrm.sys" - SRM Console
 * 	"XXXXXnt.cmp"  - Windows NT Firmware
 *
 * Revision 1.9  1996/02/26  19:33:55  fdh
 * Created a callable fsboot() function that can be called
 * from fsbmain.c or the Debug Monitor.
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

#include "system.h"
#include "fsboot.h"
#include "romhead.h"  /* includes lib.h */

static const char *fwu_strings[] = {FSB_FWU_STRINGS};
static const char *dbm_strings[] = {FSB_DBM_STRINGS};
static const char *wnt_strings[] = {FSB_WNT_STRINGS};
static const char *srm_strings[] = {FSB_SRM_STRINGS};
static const char *milo_strings[] = {FSB_MILO_STRINGS};
static const char *tst_strings[] = {FSB_tst_STRINGS};
extern int isa_present;
/*
** This array determines the search order for the
** various firmware images.  The actual string
** definitions determine the order of the various
** aliases of the different firmware images.
**/
static const Image_t ImageArray[] = {
  {FSB_FWU_ENTRY, FSB_FWU_PREFIX, fwu_strings},	/* Firmware Update Utility */
  {FSB_WNT_ENTRY, FSB_WNT_PREFIX, wnt_strings},	/* Windows NT Firmware */
  {FSB_DBM_ENTRY, FSB_DBM_PREFIX, dbm_strings},	/* Alpha Evaluation Board Debug Monitor */
  {FSB_SRM_ENTRY, FSB_SRM_PREFIX, srm_strings},	/* SRM Console */
  {FSB_MILO_ENTRY, FSB_MILO_PREFIX, milo_strings}, /* Linux Mini-Loader */
  {FSB_tst_ENTRY, FSB_tst_PREFIX, tst_strings}, /* tst-Loader */
  {0, 0, NULL}			/* terminator */
};

int fsboot(ul *destaddr)
{
  char fsbFilename[25];
  DBM_STATUS FileLoaded = FALSE;
  int idx,prefix;
  int j;
  romheader_t *header;

  outLed(led_k_uart_inited);	/* UARTs initialized */

  outLed(led_k_load_fwupdate);  /* Loading firmware update tool */
#ifdef DP264
  if(isa_present)
  {
    srom_init_lcd();
    srom_access_lcd(0x80,0);
    srom_access_lcd('F',1);
    srom_access_lcd('S',1);
    srom_access_lcd('B',1);
  }
#endif
  while (FileLoaded == FALSE) {

    BeepCode(beep_k_load_fwupdate); /* Beep to inform the user. */

    idx = 0;
    while (ImageArray[idx].name != NULL) {
      j = 0;
      while (ImageArray[idx].name[j] != NULL) 
      {
        prefix=ImageArray[idx].prefix;
#ifdef DP264
        while(prefix>=0)
        {
#endif
        *fsbFilename = '\0';	/* Clear name string */

        /* Prepend target name */
        if (prefix == 1)
#ifdef DP264
        if((!isa_present) && (ImageArray[idx].name[j][0]=='s') && (ImageArray[idx].name[j][1]=='r'))
          {
          strcpy(fsbFilename, "DS10");
        }
        else
#endif
          strcpy(fsbFilename, TARGET_NAME);

        /* Prepend target name alias if there is one */
        if (prefix == 2)
          strcpy(fsbFilename, ALT_TARGET_NAME);

        strcat(fsbFilename, ImageArray[idx].name[j]);

        *destaddr = ImageArray[idx].destination;

#ifdef DP264
  if(isa_present)
  {
          srom_access_lcd(0xC0,0);
          for (int k=0;k<8;k++)
            srom_access_lcd(' ',1);
          srom_access_lcd(0xC0,0);
          k= 0;
          while (ImageArray[idx].name[j][k])
          {
            srom_access_lcd(ImageArray[idx].name[j][k],1);
            k++;
            if (k>=8) break;
          }
   }
#endif
        if ((FileLoaded = LoadAFile(fsbFilename, (char *) *destaddr))) {
          header= (romheader_t *)*destaddr;
          if(IsHeaderValid(header))
          {
            int *from, *to;
            size_t size, i;
            uintptr_t temp; 

            size = header->romh.V0.size/sizeof(*to);
            from = (int *)((*destaddr)+header->romh.V0.hsize);
            temp = header->romh.V0.destination.high;
            temp = (temp << 32) + header->romh.V0.destination.low;
            to = (int *)temp;
            if (to < from)
              for(i = 0; i < size; ++i) 
                  to[i] = from[i];
            else
              for(i = size; i-- > 0;)
                  to[i] = from[i];

            *destaddr = temp;
          }
          outLed(idx);		/* Indicates which image was loaded */
          msleep(500);		/* Pause to display LED */
          return(TRUE);
        }
#ifdef DP264
        prefix--;
        }
#endif

        ++j;
      }
      ++idx;
    }
  }
  return(FALSE);
}
