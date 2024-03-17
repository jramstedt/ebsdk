/*
 Copyright (C) 1996 Mike White
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/
/* Windows Info-ZIP Unzip DLL module
 *
 * Author: Mike White
 *
 * Original: 1996
 *
 * This module has the entry points for "unzipping" a zip file.
 */

/*---------------------------------------------------------------------------

  This file is the WINDLL replacement for the generic ``main program source
  file'' unzip.c.

  See the general comments in the header part of unzip.c.

  Copyrights:  see accompanying file "COPYING" in UnZip source distribution.
               (This software is free but NOT IN THE PUBLIC DOMAIN.  There
               are some restrictions on commercial use.)

  ---------------------------------------------------------------------------*/

#include <windows.h>
#ifdef __RSXNT__
#  include "win32/rsxntwin.h"
#endif
#ifdef __BORLANDC__
#include <dir.h>
#endif
#define UNZIP_INTERNAL
#include "unzip.h"
#include "crypt.h"
#include "version.h"
#include "windll.h"
#include "structs.h"
#include "consts.h"

/* Added type casts to prevent potential "type mismatch" error messages. */
#ifdef REENTRANT
#  undef G
#  undef __G
#  undef __G__
#  define G                   (*(struct Globals *)pG)
#  define __G                 (struct Globals *)pG
#  define __G__               (struct Globals *)pG,
#endif

HANDLE hwildZipFN;
HANDLE hInst;               /* current instance */
LPDCL lpDCL;
HANDLE hDCL;
LPUSERFUNCTIONS lpUserFunctions;
int fNoPrinting = 0;
extern jmp_buf dll_error_return;

/* For displaying status messages and error messages */
int UZ_EXP DllMessagePrint(zvoid *pG, uch *buf, ulg size, int flag);

/* For displaying files extracted to the display window */
int DllDisplayPrint(zvoid *pG, uch *buf, ulg size, int flag);
DLLPRNT *lpPrint;

/* Dummy sound function for those applications that don't use sound */
void WINAPI DummySound(void);

#ifndef UNZIPLIB
/*  DLL Entry Point */

#ifdef __BORLANDC__
#pragma argsused
/* Borland seems to want DllEntryPoint instead of DllMain like MSVC */
#define DllMain DllEntryPoint
#endif
#ifdef WIN32
BOOL WINAPI DllMain( HINSTANCE hInstance,
                     DWORD dwReason,
                     LPVOID plvReserved)
#else
int FAR PASCAL LibMain( HINSTANCE hInstance,
                        WORD wDataSegment,
                        WORD wHeapSize,
                        LPSTR lpszCmdLine )
#endif
{
#ifndef WIN32
/* The startup code for the DLL initializes the local heap(if there is one)
 * with a call to LocalInit which locks the data segment.
 */

if ( wHeapSize != 0 )
   {
   UnlockData( 0 );
   }
hInst = hInstance;
return 1;   /* Indicate that the DLL was initialized successfully. */
#else
BOOL rc = TRUE;
switch( dwReason )
   {
   case DLL_PROCESS_ATTACH:
      // DLL is loaded. Do your initialization here.
      // If cannot init, set rc to FALSE.
      hInst = hInstance;
      break;

   case DLL_PROCESS_DETACH:
      // DLL is unloaded. Do your cleanup here.
      break;
   default:
      break;
   }
return rc;
#endif
}

#ifdef __BORLANDC__
#pragma argsused
#endif
int FAR PASCAL WEP ( int bSystemExit )
{
return 1;
}
#endif /* !UNZIPLIB

/* DLL calls */

/*
    ExtractOnlyNewer  = true if you are to extract only newer
    SpaceToUnderscore = true if convert space to underscore
    PromptToOverwrite = true if prompt to overwrite is wanted
    fQuiet    = quiet flag. 1 = few messages, 2 = no messages, 0 = all messages
    ncflag    = write to stdout if true
    ntflag    = test zip file
    nvflag    = verbose listing
    nUflag    = "update" (extract only newer/new files)
    nzflag    = display zip file comment
    ndflag    = all args are files/dir to be extracted
    noflag    = overwrite all files
    naflag    = do end-of-line translation
    nZIflag   = get Zip Info if TRUE
    C_flag    = be case insensitive if TRUE
    fPrivilege = restore ACL's if 1, use privileges if 2
    lpszZipFN = zip file name
    lpszExtractDir = directory to extract to; NULL means: current directory
*/

BOOL WINAPI Unz_SetOpts(pG, C)
zvoid * pG;
LPDCL C;
{
    G.qflag=C->fQuiet;  /* Quiet flag */
    G.pfnames = &fnames[0];       /* assign default file name vector */
    G.pxnames = &fnames[1];

    G.jflag = !C->ndflag;
    G.cflag = C->ncflag;
    G.overwrite_all = C->noflag;
    G.tflag = C->ntflag ;
    G.vflag = C->nvflag;
    G.zflag = C->nzflag;
    G.uflag = C->nUflag;
    G.aflag = C->naflag;
    G.C_flag = C->C_flag;
    G.uflag = C->ExtractOnlyNewer;
#ifdef WIN32
    G.X_flag = C->fPrivilege;
#endif
    G.overwrite_none = !G.overwrite_all;
    G.sflag = C->SpaceToUnderscore; /* Translate spaces to underscores? */
    if (C->nZIflag)
      {
      G.zipinfo_mode = TRUE;
      G.hflag = TRUE;
      G.lflag = 10;
      G.qflag = 2;
      }
    else
      {
      G.zipinfo_mode = FALSE;
      }

    G.extract_flag = (!G.zipinfo_mode &&
                      !G.cflag && !G.tflag && !G.vflag && !G.zflag
#ifdef TIMESTAMP
                      && !G.T_flag
#endif
                     );

    if (C->lpszExtractDir != NULL)
       {
       G.dflag = TRUE;
       if (G.extract_flag)
          {
#ifndef CRTL_CP_IS_ISO
          char *pExDirRoot = (char *)malloc(strlen(C->lpszExtractDir)+1);

          if (pExDirRoot == NULL)
              return FALSE;
          ISO_TO_INTERN(C->lpszExtractDir, pExDirRoot);
#else
#  define pExDirRoot C->lpszExtractDir
#endif
          G.create_dirs = !G.fflag;
          if (checkdir(__G__ pExDirRoot, ROOT) > 2)
             {
             return FALSE;
             }
          }
       }
    else
       {
       G.dflag = FALSE;
       }

/* G.wildzipfn needs to be initialized so that do_wild does not wind
   up clearing out the zip file name when it returns in process.c
*/
    if ((hwildZipFN = GlobalAlloc(GMEM_MOVEABLE, FILNAMSIZ))== (HGLOBAL)NULL)
        return FALSE;

    G.wildzipfn = GlobalLock(hwildZipFN);
    lstrcpy(G.wildzipfn, C->lpszZipFN);
    _ISO_INTERN(G.wildzipfn);

    return TRUE;    /* set up was OK */
}

void FreeDllMem(__GPRO)
{
    if (G.wildzipfn) {
        GlobalUnlock(hwildZipFN);
        G.wildzipfn = NULL;
    }
    if (hwildZipFN)
        hwildZipFN = GlobalFree(hwildZipFN);

    G.zipinfo_mode = FALSE;
}

int WINAPI windll_unzip(int ifnc, char **ifnv, int xfnc, char **xfnv,
   DCL far*C, USERFUNCTIONS far *lpUserFunc)
{
int retcode;
CONSTRUCTGLOBALS();

if (!Unz_Init((zvoid *)&G, lpUserFunc))
   {
   DESTROYGLOBALS();
   return PK_BADERR;
   }

if (C->lpszZipFN == NULL) /* Something has screwed up, we don't have a filename */
   {
   DESTROYGLOBALS();
   return PK_NOZIP;
   }

lpDCL = C;

Unz_SetOpts((zvoid *)&G, C);

/* Here is the actual call to "unzip" the files (or whatever else you
 * are doing.)
 */
retcode = Unz_Unzip((zvoid *)&G, ifnc, ifnv, xfnc, xfnv);

DESTROYGLOBALS();
return retcode;
}


BOOL WINAPI Unz_Init(pG, lpUserFunc)
zvoid *pG;
USERFUNCTIONS far * lpUserFunc;
{
lpUserFunctions = lpUserFunc;
lpPrint = lpUserFunc->print;
G.message = DllMessagePrint;
G.sound = lpUserFunc->sound;
if (G.sound == NULL)
   G.sound = DummySound;
G.replace = lpUserFunc->replace;

if (!lpPrint ||
    !G.sound ||
    !G.replace)
    return FALSE;

return TRUE;
}

int WINAPI Unz_Unzip(pG, ifnc, ifnv, xfnc, xfnv)
zvoid *pG;
int ifnc;
char **ifnv;
int xfnc;
char **xfnv;
{
int retcode;
#ifndef CRTL_CP_IS_ISO
char **intern_ifv, **intern_xfv;
#endif

G.process_all_files = (ifnc == 0 && xfnc == 0);         /* for speed */
G.filespecs = ifnc;
G.xfilespecs = xfnc;

if (ifnc > 0) {
#ifdef CRTL_CP_IS_ISO
    G.pfnames = ifnv;
#else /* !CRTL_CP_IS_ISO */
      {
        int f_cnt;
        unsigned bufsize = 0;

        intern_ifv = (char **)malloc((ifnc+1)*sizeof(char **));
        if (intern_ifv == (char **)NULL)
            {
            FreeDllMem(__G);
            return PK_BADERR;
            }

        for (f_cnt = ifnc; --f_cnt >= 0;)
            bufsize += strlen(ifnv[f_cnt]) + 1;
        intern_ifv[0] = (char *)malloc(bufsize);
        if (intern_ifv[0] == (char *)NULL)
            {
            free(intern_ifv);
            FreeDllMem(__G);
            return PK_BADERR;
            }

        for (f_cnt = 0; f_cnt < ifnc; f_cnt++)
            {
            ISO_TO_INTERN(ifnv[f_cnt], intern_ifv[f_cnt]);
            }
        intern_ifv[ifnc] = (char *)NULL;
        G.pfnames = intern_ifv;
      }
#endif /* ?CRTL_CP_IS_ISO */
    }

if (xfnc > 0) {
#ifdef CRTL_CP_IS_ISO
    G.pxnames = xfnv;
#else /* !CRTL_CP_IS_ISO */
      {
        int f_cnt;
        unsigned bufsize = 0;

        intern_xfv = (char **)malloc((xfnc+1)*sizeof(char **));
        if (intern_xfv == (char **)NULL)
            {
            if (ifnc > 0)
                {
                free(intern_ifv[0]);
                free(intern_ifv);
                }
            FreeDllMem(__G);
            return PK_BADERR;
            }

        for (f_cnt = xfnc; --f_cnt >= 0;)
            bufsize += strlen(xfnv[f_cnt]) + 1;
        intern_xfv[0] = (char *)malloc(bufsize);
        if (intern_xfv[0] == (char *)NULL)
            {
            free(intern_xfv);
            if (ifnc > 0)
                {
                free(intern_ifv[0]);
                free(intern_ifv);
                }
            FreeDllMem(__G);
            return PK_BADERR;
            }

        for (f_cnt = 0; f_cnt < xfnc; f_cnt++)
            {
            ISO_TO_INTERN(xfnv[f_cnt], intern_xfv[f_cnt]);
            }
        intern_xfv[xfnc] = (char *)NULL;
        G.pxnames = intern_xfv;
      }
#endif /* ?CRTL_CP_IS_ISO */
    }

/*---------------------------------------------------------------------------
    Okey dokey, we have everything we need to get started.  Let's roll.
  ---------------------------------------------------------------------------*/

retcode = setjmp(dll_error_return);
if (retcode)
   {
#ifndef CRTL_CP_IS_ISO
   if (xfnc > 0)
      {
      free(intern_xfv[0]);
      free(intern_xfv);
      }
   if (ifnc > 0)
      {
      free(intern_ifv[0]);
      free(intern_ifv);
      }
#endif
   FreeDllMem(__G);
   return PK_BADERR;
   }

retcode = process_zipfiles(__G);
#ifndef CRTL_CP_IS_ISO
if (xfnc > 0)
   {
   free(intern_xfv[0]);
   free(intern_xfv);
   }
if (ifnc > 0)
   {
   free(intern_ifv[0]);
   free(intern_ifv);
   }
#endif
FreeDllMem(__G);
return retcode;
}


int win_fprintf(FILE *file, unsigned int size, char far *buffer)
{
if ((file != stderr) && (file != stdout))
   {
   return write(fileno(file),(char far *)(buffer),size);
   }
if (lpPrint != NULL)
   return lpPrint((LPSTR)buffer, size);
else
   return (int)size;
}

/**********************************
 * Function DllMessagePrint()     *
 *                                *
 * Send messages to status window *
 **********************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
int UZ_EXP DllMessagePrint(pG, buf, size, flag)
    zvoid *pG;      /* globals struct:  always passed */
    uch *buf;       /* preformatted string to be printed */
    ulg size;       /* length of string (may include nulls) */
    int flag;       /* flag bits */
{
if (fNoPrinting)
   return (int)size;
if (lpPrint != NULL)
   return lpPrint((LPSTR)buf, size);
else
   return (int)size;
}

/********************************
 * Function DllDisplayPrint()   *
 *                              *
 * Send files to display window *
 ********************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
int DllDisplayPrint(pG, buf, size, flag)
    zvoid *pG;      /* globals struct:  always passed */
    uch *buf;       /* preformatted string to be printed */
    ulg size;       /* length of string (may include nulls) */
    int flag;       /* flag bits */
{
return lpPrint((LPSTR)buf, size);
}


/**********************************
 * Function UzpPassword()         *
 *                                *
 * Prompt for decryption password *
 **********************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
int UZ_EXP UzpPassword(pG, rcnt, pwbuf, size, zfn, efn)
    zvoid *pG;          /* globals struct: always passed */
    int *rcnt;          /* retry counter */
    char *pwbuf;        /* buffer for password */
    int size;           /* size of password buffer */
    ZCONST char *zfn;   /* name of zip archiv */
    ZCONST char *efn;   /* name of archiv entry being processed */
{
#if CRYPT
    LPSTR m;

    if (*rcnt == 0) {
        *rcnt = 2;
        m = "Enter password for: ";
    } else {
        (*rcnt)--;
        m = "Password incorrect--reenter: ";
    }

    return (*lpUserFunctions->password)((LPSTR)pwbuf, size, m, (LPSTR)efn);
#else /* !CRYPT */
    return IZ_PW_ERROR; /* internal error, function should never get called */
#endif /* ?CRYPT */
} /* end function UzpPassword() */

/* Turn off all messages to the calling application */
void WINAPI UzpNoPrinting(int f)
{
fNoPrinting = f;
}

/* Dummy sound function for those applications that don't use sound */
void WINAPI DummySound(void)
{
}

