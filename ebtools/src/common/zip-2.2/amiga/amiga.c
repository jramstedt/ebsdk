/*

 Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden, Igor Mandrichenko, Paul Kienitz and
 John Bush.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/* OS specific routines for AMIGA platform.
 *
 * John Bush    <John.Bush@east.sun.com>  BIX: jbush
 * Paul Kienitz <kie@pacbell.net>
 *
 * History:
 *
 * Date     DoBee    Comments
 * -------  -------- -----------------------------------------------
 * 21Jan93  JBush    Original coding.
 *                   Incorporated filedate.c (existing routine).
 *
 * 31Jan93  JBush    Made filedate.c include unconditional.
 *
 * 18Jul93  PaulK    Moved Aztec _abort() here from stat.c because we
 *                   can't share the same one between Zip and UnZip.
 *                   Added close_leftover_open_dirs() call to it.
 *
 * 17Apr95  PaulK    Added Amiga internal version string so that
 *                   installer programs can compare the version being
 *                   installed to see if the copy the user already has
 *                   is older or newer.  Added Prestart_Hook to support
 *                   debug tracing in deflate.a.
 *
 *  6May95  PaulK    Added GetComment() for filenote support.
 *
 * 12Nov95  PaulK    Added #define ZIP in front of filedate.c, for
 *                   new options in there; removed declare of set_con()
 *                   since echon() no longer expands to it (or anything).
 *
 * 12Feb96  PaulK    Removed call of echon() entirely.
 *
 * 12Jul97  PaulK    Made both Aztec and SAS define USE_TIME_LIB for filedate.c
 *
 * 26Aug97  PaulK    Added ClearIOErr_exit()
 */

#include <stdlib.h>
#include "ziperr.h"
void ziperr(int c, char *h);

/* ============================================================ */
/* filedate.c is an external file, since it's shared with UnZip */

#if defined(AZTEC_C) || defined(__SASC)
#  define USE_TIME_LIB
#endif

#define ZIP
#if !defined(UTIL) && !defined(USE_TIME_LIB)
#  define NO_MKTIME
#endif
#include "amiga/filedate.c"

#ifdef AZTEC_C
#  define setenv BOGUS_INCOMPATIBLE_setenv
#  include <fcntl.h>
#  undef setenv
#  ifdef DEBUG
#    define PRESTART_HOOK
#  endif
#endif

/* ============================================================ */
/* the same applies to stat.c                                   */

#include "amiga/stat.c"

/* the following handles cleanup when a ^C interrupt happens: */

void _abort(void)               /* called when ^C is pressed */
{
    close_leftover_open_dirs();
    ziperr(ZE_ABORT, "^C");
}

void ClearIOErr_exit(int e)     /* EXIT is defined as this */
{
    if (!e)
        ((struct Process *) FindTask(NULL))->pr_Result2 = 0;
    /* we clear IoErr() since we are successful, in a 1.x-compatible way */
    exit(e);
}


/* Make sure the version number here matches the number in revision.h */
/* as closely as possible in strict decimal "#.#" form:               */
const char version_id[] = "\0$VER: Zip 2.2 ("
#  include "env:VersionDate"
")\r\n";

/* call this with an arg of NULL to free storage: */

char *GetComment(char *filename)
{
    BPTR lk;
    static struct FileInfoBlock *fib = NULL;

    if (!filename) {
        if (fib) FreeMem(fib, sizeof(*fib));
        fib = NULL;
        return NULL;
    }
    if (!fib) {
        if (!(fib = AllocMem(sizeof(*fib), MEMF_PUBLIC)))
            ziperr(ZE_MEM, "was checking filenotes");
    }
    if (!(lk = Lock(filename, ACCESS_READ)))
        return NULL;
    if (!Examine(lk, fib))
        fib->fib_Comment[0] = '\0';
    UnLock(lk);
    return fib->fib_Comment[0] ? &fib->fib_Comment[0] : NULL;
}
