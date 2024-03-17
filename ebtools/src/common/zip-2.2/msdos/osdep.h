/*

 Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Onno van der Linden, Christian Spieler and Kai Uwe Rommel.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/* The symbol DOS is used throughout the Zip source to identify code portions
 * specific to the MSDOS port.
 * Just to make sure, we check that it is set.
 * (Currently, this should should not be neccessary, since currently it has
 * to be set on the compiler command line to get this file read in.)
 */
#ifndef DOS
#  define DOS
#endif

/* The symbol MSDOS is consistently used in the generic source files
 * to identify code to support for MSDOS (and MSDOS related) stuff.
 * e.g: FAT or (FAT like) file systems,
 *      '\\' as directory separator in paths,
 *      "\r\n" as record (line) terminator in text files, ...
 *
 * IMPORTANT Note:
 *  This symbol is not unique for the MSDOS port !!!!!!
 *  It is also defined by ports to some other OS which are (to some extend)
 *  considered DOS compatible.
 *  Examples are: OS/2 (OS2), Windows NT and Windows 95 (WIN32).
 *
 */
#ifndef MSDOS
#  define MSDOS
#endif

/* Power C is similar to Turbo C */
#ifdef __POWERC
#  define __TURBOC__
#endif /* __POWERC */

/* Automatic setting of the common Microsoft C idenfifier MSC.
 * NOTE: Watcom also defines M_I*86 !
 */
#if defined(_MSC_VER) || (defined(M_I86) && !defined(__WATCOMC__))
#  ifndef MSC
#    define MSC                 /* This should work for older MSC, too!  */
#  endif
#endif

#if !defined(__GO32__) && !defined(__EMX__)
#  define NO_UNISTD_H
#endif

#if defined(__WATCOMC__) && defined(__386__)
#  define WATCOMC_386
#endif

#ifdef WINDLL
#  define MSWIN
#  define MEMORY16
#  define ZIPERR(errcode, msg)  return(ziperr(errcode, msg), (errcode))
#endif


#if !defined(__EMX__) && !defined(__GO32__) && !defined(WATCOMC_386)
#if !defined(WINDLL)
#  define MSDOS16 /* 16 bit MSDOS only */
#  define MEMORY16
#endif
#endif

#if !defined(NO_ASM) && !defined(ASMV)
#  define ASMV
#endif

#if !defined(NO_EF_UT_TIME) && !defined(USE_EF_UT_TIME)
#  define USE_EF_UT_TIME
#endif

#ifdef MEMORY16
#  ifndef NO_ASM
#    define ASM_CRC 1
#  endif /* ?NO_ASM */
#  ifdef __TURBOC__
#    include <alloc.h>
#    if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
#      if defined(DYNAMIC_CRC_TABLE) && defined(DYNALLOC_CRCTAB)
        error: No dynamic CRC table allocation with Borland C far data models.
#      endif /* DYNAMIC_CRC_TABLE */
#    endif /* Turbo/Borland C far data memory models */
#    define nearmalloc malloc
#    define nearfree   free
#    define DYN_ALLOC
#  else /* !__TURBOC__ */
#    include <malloc.h>
#    define nearmalloc _nmalloc
#    define nearfree   _nfree
#    define farmalloc  _fmalloc
#    define farfree    _ffree
#  endif /* ?__TURBOC__ */
#  define MY_ZCALLOC 1
#  if defined(USE_ZLIB) && !defined(USE_OWN_CRCTAB)
#    define USE_OWN_CRCTAB
#  endif
#endif /* MEMORY16 */


#define USE_CASE_MAP

#define ROUNDED_TIME(time)  (((time) + 1) & (~1))
#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : procname(n))

#define FOPR "rb"
#define FOPM "r+b"
#define FOPW "wb"

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#ifdef ZCRYPT_INTERNAL
#  ifdef WINDLL
#    define ZCR_SEED2     (unsigned)3141592654L /* use PI as seed pattern */
#  else
#    ifndef __GO32__
#      include <process.h>      /* getpid() declaration for srand seed */
#    endif
#  endif
#endif

/*
 * djgpp 1.x did not declare these
 */
#if defined(__GO32__) && !defined(__DJGPP__)
char *strlwr(char *);
int setmode(int, int);
#endif

#ifdef __WATCOMC__
#  define NO_MKTEMP
#  define HAS_OPENDIR
#  define SSTAT stat_bandaid
   int stat_bandaid(const char *path, struct stat *buf);

/* Get asm routines to link properly without using "__cdecl": */
#  ifdef __386__
#    ifdef ASMV
#      pragma aux match_init    "_*" parm caller [] modify []
#      pragma aux longest_match "_*" parm caller [] value [eax] \
                                      modify [eax ecx edx]
#    endif
#    ifndef USE_ZLIB
#      pragma aux crc32         "_*" parm caller [] value [eax] modify [eax]
#      pragma aux get_crc_table "_*" parm caller [] value [eax] \
                                      modify [eax ecx edx]
#    endif /* !USE_ZLIB */
#  else /* !__386__ */
#    ifdef ASMV
#      pragma aux match_init    "_*" parm caller [] loadds modify [ax bx]
#      pragma aux longest_match "_*" parm caller [] loadds value [ax] \
                                      modify [ax bx cx dx es]
#    endif /* ASMV */
#    ifndef USE_ZLIB
#      pragma aux crc32         "_*" parm caller [] value [ax dx] \
                                      modify [ax bx cx dx es]
#      pragma aux get_crc_table "_*" parm caller [] value [ax] \
                                      modify [ax bx cx dx]
#    endif /* !USE_ZLIB */
#  endif /* ?__386__ */
#endif /* __WATCOMC__ */

/*
 * Wrapper function to get around the MSC7 00:00:00 31 Dec 1899 time base,
 * see msdos.c for more info
 */

#if defined(_MSC_VER) && _MSC_VER == 700
#  define localtime(t) msc7_localtime(t)
#endif
