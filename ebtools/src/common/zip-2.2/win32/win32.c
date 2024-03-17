/*

 Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/*
 * WIN32 specific functions for ZIP.
 *
 * The WIN32 version of ZIP heavily relies on the MSDOS and OS2 versions,
 * since we have to do similar things to switch between NTFS, HPFS and FAT.
 */


#include "zip.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <windows.h>
#ifdef __RSXNT__
#  include "win32/rsxntwin.h"
#endif
#include "win32/win32zip.h"

#define A_RONLY    0x01
#define A_HIDDEN   0x02
#define A_SYSTEM   0x04
#define A_LABEL    0x08
#define A_DIR      0x10
#define A_ARCHIVE  0x20


#define EAID     0x0009


#ifndef UTIL

extern int noisy;

#if 0           /* Currently, this is not used at all */
#ifdef USE_EF_UT_TIME
local time_t FileTime2utime(FILETIME *pft);
#endif
#endif /* never */


/* FAT / HPFS detection */

int IsFileSystemOldFAT(char *dir)
{
  char root[4];
  char vname[128];
  DWORD vnamesize = sizeof(vname);
  DWORD vserial;
  DWORD vfnsize;
  DWORD vfsflags;
  char vfsname[128];
  DWORD vfsnamesize = sizeof(vfsname);

    /*
     * We separate FAT and HPFS+other file systems here.
     * I consider other systems to be similar to HPFS/NTFS, i.e.
     * support for long file names and being case sensitive to some extent.
     */

    strncpy(root, dir, 3);
    if ( isalpha(root[0]) && (root[1] == ':') ) {
      root[0] = to_up(dir[0]);
      root[2] = '\\';
      root[3] = 0;
    }
    else {
      root[0] = '\\';
      root[1] = 0;
    }

    if ( !GetVolumeInformation(root, vname, vnamesize,
                         &vserial, &vfnsize, &vfsflags,
                         vfsname, vfsnamesize)) {
        fprintf(mesg, "zip diagnostic: GetVolumeInformation failed\n");
        return(FALSE);
    }

    return vfnsize <= 12;
}

/* access mode bits and time stamp */

int GetFileMode(char *name)
{
DWORD dwAttr;
#ifdef __RSXNT__        /* RSXNT/EMX C rtl uses OEM charset */
  char *ansi_name = (char *)alloca(strlen(name) + 1);

  OemToAnsi(name, ansi_name);
  name = ansi_name;
#endif

  dwAttr = GetFileAttributes(name);
  if ( dwAttr == 0xFFFFFFFF ) {
    fprintf(mesg, "zip diagnostic: GetFileAttributes failed\n");
    return(0x20); /* the most likely, though why the error? security? */
  }
  return(
          (dwAttr&FILE_ATTRIBUTE_READONLY  ? A_RONLY   :0)
        | (dwAttr&FILE_ATTRIBUTE_HIDDEN    ? A_HIDDEN  :0)
        | (dwAttr&FILE_ATTRIBUTE_SYSTEM    ? A_SYSTEM  :0)
        | (dwAttr&FILE_ATTRIBUTE_DIRECTORY ? A_DIR     :0)
        | (dwAttr&FILE_ATTRIBUTE_ARCHIVE   ? A_ARCHIVE :0));
}


#if 0           /* Currently, this is not used at all */

#ifdef USE_EF_UT_TIME
#  define UNIX_TIME_ZERO_HI  0x019DB1DE
#  define UNIX_TIME_ZERO_LO  0xD53E8000
#  define NT_QUANTA_PER_UNIX 10000000
#  define FTQUANTA_PER_UT_L  (NT_QUANTA_PER_UNIX & 0xFFFF)
#  define FTQUANTA_PER_UT_H  (NT_QUANTA_PER_UNIX >> 16)
#  define UNIX_TIME_UMAX_HI  0x0236485E
#  define UNIX_TIME_UMAX_LO  0xD4A5E858

local time_t FileTime2utime(FILETIME *pft)
{
#if defined(__GNUC__) || defined(ULONG_LONG_MAX)
    unsigned long long NTtime;

    NTtime = ((unsigned long long)pft->dwLowDateTime +
              ((unsigned long long)pft->dwHighDateTime << 32))

    /* underflow and overflow handling */
    if (NTtime <= ((unsigned long long)UNIX_TIME_ZERO_LO +
                   ((unsigned long long)UNIX_TIME_ZERO_HI << 32)))
        return (time_t)0;
    if (NTtime >= ((unsigned long long)UNIX_TIME_UMAX_LO +
                   ((unsigned long long)UNIX_TIME_UMAX_HI << 32)))
        return (time_t)ULONG_MAX;

    NTtime -= ((unsigned long long)UNIX_TIME_ZERO_LO +
               ((unsigned long long)UNIX_TIME_ZERO_HI << 32));
    return (time_t)(NTtime / (unsigned long long)NT_QUANTA_PER_UNIX);
#else /* "unsigned long long" may not be supported */
    ulg rlo, rhi;
    unsigned int carry = 0;
    unsigned int blo, bm1, bm2, bhi;

    if ((pft->dwHighDateTime < UNIX_TIME_ZERO_HI) ||
        ((pft->dwHighDateTime == UNIX_TIME_ZERO_HI) &&
         (pft->dwLowDateTime <= UNIX_TIME_ZERO_LO)))
        return (time_t)0;
    if ((pft->dwHighDateTime > UNIX_TIME_UMAX_HI) ||
        ((pft->dwHighDateTime == UNIX_TIME_UMAX_HI) &&
         (pft->dwLowDateTime <= UNIX_TIME_UMAX_LO)))
        return (time_t)ULONG_MAX;

    /* ... */
    return (time_t)((ulg)blo + ((ulg)bhi) << 16));
#endif
}
#endif /* USE_EF_UT_TIME */


long GetTheFileTime(char *name, iztimes *z_ut)
{
HANDLE h;
FILETIME Modft, Accft, Creft, lft;
WORD dh, dl;
#ifdef __RSXNT__        /* RSXNT/EMX C rtl uses OEM charset */
  char *ansi_name = (char *)alloca(strlen(name) + 1);

  OemToAnsi(name, ansi_name);
  name = ansi_name;
#endif

  h = CreateFile(name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if ( h != INVALID_HANDLE_VALUE ) {
    GetFileTime(h, &Creft, &Accft, &Modft);
    CloseHandle(h);
#ifdef USE_EF_UT_TIME
    if (z_ut != NULL) {
      z_ut->atime = FileTime2utime(&Accft);
      z_ut->mtime = FileTime2utime(&Modft);
      z_ut->ctime = FileTime2utime(&Creft);
    }
#endif
    FileTimeToLocalFileTime(&ft, &lft);
    FileTimeToDosDateTime(&lft, &dh, &dl);
    return(dh<<16) | dl;
  }
  else
    return 0L;
}

#endif /* never */


void ChangeNameForFAT(char *name)
{
  char *src, *dst, *next, *ptr, *dot, *start;
  static char invalid[] = ":;,=+\"[]<>| \t";

  if ( isalpha(name[0]) && (name[1] == ':') )
    start = name + 2;
  else
    start = name;

  src = dst = start;
  if ( (*src == '/') || (*src == '\\') )
    src++, dst++;

  while ( *src )
  {
    for ( next = src; *next && (*next != '/') && (*next != '\\'); next++ );

    for ( ptr = src, dot = NULL; ptr < next; ptr++ )
      if ( *ptr == '.' )
      {
        dot = ptr; /* remember last dot */
        *ptr = '_';
      }

    if ( dot == NULL )
      for ( ptr = src; ptr < next; ptr++ )
        if ( *ptr == '_' )
          dot = ptr; /* remember last _ as if it were a dot */

    if ( dot && (dot > src) &&
         ((next - dot <= 4) ||
          ((next - src > 8) && (dot - src > 3))) )
    {
      if ( dot )
        *dot = '.';

      for ( ptr = src; (ptr < dot) && ((ptr - src) < 8); ptr++ )
        *dst++ = *ptr;

      for ( ptr = dot; (ptr < next) && ((ptr - dot) < 4); ptr++ )
        *dst++ = *ptr;
    }
    else
    {
      if ( dot && (next - src == 1) )
        *dot = '.';           /* special case: "." as a path component */

      for ( ptr = src; (ptr < next) && ((ptr - src) < 8); ptr++ )
        *dst++ = *ptr;
    }

    *dst++ = *next; /* either '/' or 0 */

    if ( *next )
    {
      src = next + 1;

      if ( *src == 0 ) /* handle trailing '/' on dirs ! */
        *dst = 0;
    }
    else
      break;
  }

  for ( src = start; *src != 0; ++src )
    if ( (strchr(invalid, *src) != NULL) || (*src == ' ') )
      *src = '_';
}

char *GetLongPathEA(char *name)
{
    return(NULL); /* volunteers ? */
}

int IsFileNameValid(x)
char *x;
{
    WIN32_FIND_DATA fd;
    HANDLE h;
#ifdef __RSXNT__        /* RSXNT/EMX C rtl uses OEM charset */
    char *ansi_name = (char *)alloca(strlen(x) + 1);

    OemToAnsi(x, ansi_name);
    x = ansi_name;
#endif

    if ((h = FindFirstFile(x, &fd)) == INVALID_HANDLE_VALUE)
        return FALSE;
    FindClose(h);
    return TRUE;
}

char *getVolumeLabel(drive, vtime, vmode, vutim)
  int drive;    /* drive name: 'A' .. 'Z' or '\0' for current drive */
  ulg *vtime;   /* volume label creation time (DOS format) */
  ulg *vmode;   /* volume label file mode */
  time_t *vutim;/* volume label creationtime (UNIX format) */

/* If a volume label exists for the given drive, return its name and
   pretend to set its time and mode. The returned name is static data. */
{
  char rootpath[4];
  static char vol[14];
  ulg fnlen, flags;

  *vmode = A_ARCHIVE | A_LABEL;           /* this is what msdos returns */
  *vtime = dostime(1980, 1, 1, 0, 0, 0);  /* no true date info available */
  *vutim = dos2unixtime(*vtime);
  strcpy(rootpath, "x:\\");
  rootpath[0] = drive;
  if (GetVolumeInformation(drive ? rootpath : NULL, vol, 13, NULL,
                           &fnlen, &flags, NULL, 0))
#ifdef __RSXNT__        /* RSXNT/EMX C rtl uses OEM charset */
    return (AnsiToOem(vol, vol), vol);
#else
    return vol;
#endif
  else
    return NULL;
}

#endif /* !UTIL */



int ZipIsWinNT(void)    /* returns TRUE if real NT, FALSE if Win95 or Win32s */
{
    static DWORD g_PlatformId = 0xFFFFFFFF; /* saved platform indicator */

    if (g_PlatformId == 0xFFFFFFFF) {
        /* note: GetVersionEx() doesn't exist on WinNT 3.1 */
        if (GetVersion() < 0x80000000)
            g_PlatformId = TRUE;
        else
            g_PlatformId = FALSE;
    }
    return (int)g_PlatformId;
}


#ifndef UTIL
#ifdef __WATCOMC__
#  include <io.h>
#  define _get_osfhandle _os_handle
/* gaah -- Watcom's docs claim that _get_osfhandle exists, but it doesn't.  */
#endif

/*
 * return TRUE if file is seekable
 */
int fseekable(fp)
FILE *fp;
{
    return GetFileType((HANDLE)_get_osfhandle(fileno(fp))) == FILE_TYPE_DISK;
}
#endif /* !UTIL */


#if 0 /* seems to be never used; try it out... */
char *StringLower(char *szArg)
{
  char *szPtr;
/*  unsigned char *szPtr; */
  for ( szPtr = szArg; *szPtr; szPtr++ )
    *szPtr = lower[*szPtr];
  return szArg;
}
#endif /* never */


#ifdef __WATCOMC__

/* This papers over a bug in Watcom 10.6's standard library... sigh. */
/* Apparently it applies to both the DOS and Win32 stat()s.          */
/* I believe this has been fixed for the following Watcom release.   */

int stat_bandaid(const char *path, struct stat *buf)
{
  char newname[4];
  if (!stat(path, buf))
    return 0;
  else if (!strcmp(path, ".") || (path[0] && !strcmp(path + 1, ":."))) {
    strcpy(newname, path);
    newname[strlen(path) - 1] = '\\';   /* stat(".") fails for root! */
    return stat(newname, buf);
  } else
    return -1;
}

/* Watcom 10.6's getch() does not handle Alt+<digit><digit><digit>. */
/* Note that if PASSWD_FROM_STDIN is defined, the file containing   */
/* the password must have a carriage return after the word, not a   */
/* Unix-style newline (linefeed only).  This discards linefeeds.    */

int getch(void)
{
  HANDLE stin;
  DWORD rc;
  unsigned char buf[2];
  int ret = -1;

#  ifdef PASSWD_FROM_STDIN
  DWORD odemode = ~0;
  stin = GetStdHandle(STD_INPUT_HANDLE);
  if (GetConsoleMode(stin, &odemode))
    SetConsoleMode(stin, ENABLE_PROCESSED_INPUT);  /* raw except ^C noticed */
#  else
  if (!(stin = CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)))
    return -1;
  SetConsoleMode(stin, ENABLE_PROCESSED_INPUT);    /* raw except ^C noticed */
#  endif
  if (ReadFile(stin, &buf, 1, &rc, NULL) && rc == 1)
    ret = buf[0];
  /* when the user hits return we get CR LF.  We discard the LF, not the CR,
   * because when we call this for the first time after a previous input
   * such as the one for "replace foo? [y]es, ..." the LF may still be in
   * the input stream before whatever the user types at our prompt. */
  if (ret == '\n')
    if (ReadFile(stin, &buf, 1, &rc, NULL) && rc == 1)
      ret = buf[0];
#  ifdef PASSWD_FROM_STDIN
  if (odemode != ~0)
    SetConsoleMode(stin, odemode);
#  else
  CloseHandle(stin);
#  endif
  return ret;
}
#endif /* __WATCOMC__ */


#ifndef WINDLL
/******************************/
/*  Function version_local()  */
/******************************/

void version_local()
{
    static ZCONST char CompiledWith[] = "Compiled with %s%s for %s%s%s%s.\n\n";
#if defined(_MSC_VER) || defined(__WATCOMC__)
    char buf[80];
#if defined(_MSC_VER) && _MSC_VER > 900
    char buf2[80];
#endif
#endif

    printf(CompiledWith,

#ifdef _MSC_VER  /* MSC == MSVC++, including the SDK compiler */
      (sprintf(buf, "Microsoft C %d.%02d ", _MSC_VER/100, _MSC_VER%100), buf),
#  if (_MSC_VER == 800)
        "(Visual C++ v1.1)",
#  elif (_MSC_VER == 850)
        "(Windows NT v3.5 SDK)",
#  elif (_MSC_VER == 900)
        "(Visual C++ v2.x)",
#  elif (_MSC_VER > 900)
        (sprintf(buf2, "(Visual C++ v%d.%d)", _MSC_VER/100 - 6,
                 _MSC_VER%100/10), buf2),
#  else
        "(bad version)",
#  endif
#elif defined(__WATCOMC__)
#  if (__WATCOMC__ % 10 > 0)
/* We do this silly test because __WATCOMC__ gives two digits for the  */
/* minor version, but Watcom packaging prefers to show only one digit. */
        (sprintf(buf, "Watcom C/C++ %d.%02d", __WATCOMC__ / 100,
                 __WATCOMC__ % 100), buf), "",
#  else
        (sprintf(buf, "Watcom C/C++ %d.%d", __WATCOMC__ / 100,
                 (__WATCOMC__ % 100) / 10), buf), "",
#  endif /* __WATCOMC__ % 10 > 0 */
#elif defined(__TURBOC__)
#  ifdef __BORLANDC__
     "Borland C++",
#    if (__BORLANDC__ == 0x0452)   /* __BCPLUSPLUS__ = 0x0320 */
        " 4.0 or 4.02",
#    elif (__BORLANDC__ == 0x0460)   /* __BCPLUSPLUS__ = 0x0340 */
        " 4.5",
#    elif (__BORLANDC__ == 0x0500)   /* __TURBOC__ = 0x0500 */
        " 5.0",
#    else
        " later than 5.0",
#    endif
#  else /* !__BORLANDC__ */
     "Turbo C",
#    if (__TURBOC__ >= 0x0400)     /* Kevin:  3.0 -> 0x0401 */
        "++ 3.0 or later",
#    elif (__TURBOC__ == 0x0295)     /* [661] vfy'd by Kevin */
        "++ 1.0",
#    endif
#  endif /* __BORLANDC__ */
#elif defined(__GNUC__)
#  ifdef __RSXNT__
#    if defined(__EMX__)
      "rsxnt(emx)+gcc ",
#    elif defined(__DJGPP__)
      (sprintf(buf, "rsxnt(djgpp) v%d.%02d / gcc ", __DJGPP__, __DJGPP_MINOR__),
       buf),
#    elif defined(__GO32__)
      "rsxnt(djgpp) v1.x / gcc ",
#    else
      "rsxnt(unknown) / gcc ",
#    endif
#  else
      "gcc ",
#  endif
      __VERSION__,
#else
      "unknown compiler (SDK?)", "",
#endif

      "Windows 95 / Windows NT", "\n(32-bit)",

#ifdef __DATE__
      " on ", __DATE__
#else
      "", ""
#endif
    );

    return;

} /* end function version_local() */
#endif /* !WINDLL */
