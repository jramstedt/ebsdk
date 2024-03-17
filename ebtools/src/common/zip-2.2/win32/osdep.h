/*

 Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/* Automatic setting of the common Microsoft C idenfifier MSC.
 * NOTE: Watcom also defines M_I*86 !
 */
#if defined(_MSC_VER) || (defined(M_I86) && !defined(__WATCOMC__))
#  ifndef MSC
#    define MSC                 /* This should work for older MSC, too!  */
#  endif
#endif

#if defined(__WATCOMC__) && defined(__386__)
#  define WATCOMC_386
#endif

#ifndef MSDOS
/*
 * Windows 95 (and Windows NT) file systems are (to some extend)
 * extensions of MSDOS. Common features include for example:
 *      FAT or (FAT like) file systems,
 *      '\\' as directory separator in paths,
 *      "\r\n" as record (line) terminator in text files, ...
 */
#  define MSDOS
/* inherit MS-DOS file system etc. stuff */
#endif

#define USE_CASE_MAP
#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : procname(n))
#define BROKEN_FSEEK
#define HAVE_FSEEKABLE

/* File operations--use "b" for binary if allowed or fixed length 512 on VMS */
#define FOPR "rb"
#define FOPM "r+b"
#define FOPW "wb"

#if !defined(NO_EF_UT_TIME) && !defined(USE_EF_UT_TIME)
#  define USE_EF_UT_TIME
#endif
#ifdef __RSXNT__
#  ifndef NO_NTSD_WITH_RSXNT
#    define NO_NTSD_WITH_RSXNT  /* RSXNT windows.h does not yet support NTSD */
#  endif
#else /* !__RSXNT__ */
#  ifndef NTSD_EAS
#    define NTSD_EAS
#  endif
#endif /* ?__RSXNT__ */

#ifdef WINDLL
#ifndef NO_ASM
#   define NO_ASM
#endif
#ifndef MSWIN
#   define MSWIN
#endif
#ifndef REENTRANT
#   define REENTRANT
#endif
#   define ZIPERR(errcode, msg) return(ziperr(errcode, msg), (errcode))
#endif

/* Enable use of optimized x86 assembler version of longest_match() for
   MSDOS, WIN32 and OS2 per default.  */
#if !defined(NO_ASM) && !defined(ASMV)
#  define ASMV
#endif

#if !defined(__GO32__) && !defined(__EMX__)
#  define NO_UNISTD_H
#endif

/* Get types and stat */
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <malloc.h>

#ifdef ZCRYPT_INTERNAL
#  ifdef WINDLL
#    define ZCR_SEED2     (unsigned)3141592654L /* use PI as seed pattern */
#  else
#    include <process.h>        /* getpid() declaration for srand seed */
#  endif
#endif

#ifdef __WATCOMC__
#  define NO_MKTEMP
#  define SSTAT stat_bandaid
   int stat_bandaid(const char *path, struct stat *buf);

/* Get asm routines to link properly without using "__cdecl": */
#  ifdef __386__
#    ifdef ASMV
#      pragma aux match_init    "_*" parm caller [] modify []
#      pragma aux longest_match "_*" parm caller [] value [eax] \
                                      modify [eax ecx edx]
#    endif
#    if defined(ASM_CRC) && !defined(USE_ZLIB)
#      pragma aux crc32         "_*" parm caller [] value [eax] modify [eax]
#      pragma aux get_crc_table "_*" parm caller [] value [eax] \
                                      modify [eax ecx edx]
#    endif /* ASM_CRC && !USE_ZLIB */
#  endif /* __386__ */
#endif /* __WATCOMC__ */
